// $Id$
/* @@tag:xara-cn@@ DO NOT MODIFY THIS LINE
================================XARAHEADERSTART===========================
 
               Xara LX, a vector drawing and manipulation program.
                    Copyright (C) 1993-2006 Xara Group Ltd.
       Copyright on certain contributions may be held in joint with their
              respective authors. See AUTHORS file for details.

LICENSE TO USE AND MODIFY SOFTWARE
----------------------------------

This file is part of Xara LX.

Xara LX is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License version 2 as published
by the Free Software Foundation.

Xara LX and its component source files are distributed in the hope
that it will be useful, but WITHOUT ANY WARRANTY; without even the
implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with Xara LX (see the file GPL in the root directory of the
distribution); if not, write to the Free Software Foundation, Inc., 51
Franklin St, Fifth Floor, Boston, MA  02110-1301 USA


ADDITIONAL RIGHTS
-----------------

Conditional upon your continuing compliance with the GNU General Public
License described above, Xara Group Ltd grants to you certain additional
rights. 

The additional rights are to use, modify, and distribute the software
together with the wxWidgets library, the wxXtra library, and the "CDraw"
library and any other such library that any version of Xara LX relased
by Xara Group Ltd requires in order to compile and execute, including
the static linking of that library to XaraLX. In the case of the
"CDraw" library, you may satisfy obligation under the GNU General Public
License to provide source code by providing a binary copy of the library
concerned and a copy of the license accompanying it.

Nothing in this section restricts any of the rights you have under
the GNU General Public License.


SCOPE OF LICENSE
----------------

This license applies to this program (XaraLX) and its constituent source
files only, and does not necessarily apply to other Xara products which may
in part share the same code base, and are subject to their own licensing
terms.

This license does not apply to files in the wxXtra directory, which
are built into a separate library, and are subject to the wxWindows
license contained within that directory in the file "WXXTRA-LICENSE".

This license does not apply to the binary libraries (if any) within
the "libs" directory, which are subject to a separate license contained
within that directory in the file "LIBS-LICENSE".


ARRANGEMENTS FOR CONTRIBUTION OF MODIFICATIONS
----------------------------------------------

Subject to the terms of the GNU Public License (see above), you are
free to do whatever you like with your modifications. However, you may
(at your option) wish contribute them to Xara's source tree. You can
find details of how to do this at:
  http://www.xaraxtreme.org/developers/

Prior to contributing your modifications, you will need to complete our
contributor agreement. This can be found at:
  http://www.xaraxtreme.org/developers/contribute/

Please note that Xara will not accept modifications which modify any of
the text between the start and end of this header (marked
XARAHEADERSTART and XARAHEADEREND).


MARKS
-----

Xara, Xara LX, Xara X, Xara X/Xtreme, Xara Xtreme, the Xtreme and Xara
designs are registered or unregistered trademarks, design-marks, and/or
service marks of Xara Group Ltd. All rights in these marks are reserved.


      Xara Group Ltd, Gaddesden Place, Hemel Hempstead, HP2 6EX, UK.
                        http://www.xara.com/

=================================XARAHEADEREND============================
 */
// 
// WebPrefs.cpp
//
// This file implements the dialogue box that allows the user to set the export web file
// preferences.

/*
*/

#include "camtypes.h"
#include "app.h"
#include "spread.h"
#include "page.h"
#include "layer.h"

#include "xarprefs.h"
#include "webprefs.h"
#include "webparam.h"

//#include "filtrres.h"	// _R(IDS_WEBPREFSDLG)

#include "ensure.h"
#include "errors.h"
#include "dlgtypes.h"
//#include "resource.h"
//#include "rikdlg.h"		// _R(IDB_SLIDERBASE), _R(IDB_SLIDERSLIDER)
//#include "xardlg.h"		// Resource file for the base dialogue.
//#include "webdlg.h"		// Resource file for the dialogue.

#include "dialogop.h"	// DialogOp header
#include "dlgmgr.h"		// Dialogue manager class
#include "msg.h"
//#include "justin2.h"	// _R(IDS_PERCENT_FORMAT)

#include "bmpcomp.h"	// bitmap list
#include "bitmap.h"		// kernel bitmap 

// This is not compulsory, but you may as well put it in so that the correct version
// of your file can be registered in the .exe
DECLARE_SOURCE("$Revision$");

// An implement to match the Declare in the .h file.
// If you have many classes, it is recommended to place them all together, here at the start of the file
CC_IMPLEMENT_DYNCREATE	( WebPrefsDlg,		NativePrefsDlg )
CC_IMPLEMENT_MEMDUMP	( WebPrefsDlgParam,	OpParam )

// This will get Camelot to display the filename and linenumber of any memory allocations
// that are not released at program exit
#define new CAM_DEBUG_NEW

/******************************************************************************************

>	WebPrefsDlg::WebPrefsDlg () : NativePrefsDlg ( _R(IDD_EXPORTWEBOPTS) )

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/5/96
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	WebPrefsDlg constructor. Creates a non-undoable operation
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

WebPrefsDlg::WebPrefsDlg () : NativePrefsDlg ( _R(IDD_EXPORTWEBOPTS) )
{
	// The base class takes care of all the business.
}

/******************************************************************************************

>	BOOL WebPrefsDlg::CommitDialogValues(WebPrefsDlg* pWebPrefs)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/5/96
	Inputs:		pointer to the dialogue box
	Outputs:	-
	Returns:	True if values in dialog box ok, False otherwise.	  
	Purpose:	Takes the values in the dialog box and sets the return values accordingly
				Called when ok is pressed on the dialog box.
	Errors:		-
	SeeAlso:	WebPrefsDlg::CommitDialogValues;

******************************************************************************************/

BOOL WebPrefsDlg::CommitDialogValues ()
{
	// Ok has been pressed so take the values and set up the static values so that the
	// caller can access them

	BOOL Valid = 0;	// Flag for validity of value
	BOOL State = 0;	// Flag for reading the state of gadgets

	// Work out what is currently selected
	// Graeme (25/1/00) - Split SetSelType up into SetExportSel and SetViewportSel.
	mpParams->SetViewportSel	( GetExportSelection ( _R(IDC_WEBOPTS_DRAWING)) );  
	
	// WEBSTER - markn 28/1/97
	// Always compress file
#ifndef WEBSTER
	BOOL Compression = GetLongGadgetValue(_R(IDC_WEBOPTS_COMPRESS), 0, 1, 0, &Valid);
	mpParams->SetCompression(Compression);
#else
	mpParams->SetCompression(TRUE);  
#endif // WEBSTER

	// Switch is Remove preview bitmap and so logic is reversed
	BOOL ExportPreviewBitmap = !GetLongGadgetValue(_R(IDC_WEBOPTS_PREVIEWBITMAP), 0, 1, 0, &Valid);
	mpParams->SetExportPreviewBitmap(ExportPreviewBitmap);

	BOOL ConvertTextToOutlines = GetLongGadgetValue(_R(IDC_WEBOPTS_TEXTOUTLINES), 0, 1, 0, &Valid);
	mpParams->SetConvertTextToOutlines(ConvertTextToOutlines);

	// No need to convert blends to outlines anymore, because the compact renderer can cope with
	// them now.  (29 today, and wondering whether I should be spending the last days of my twenties doing this)
	mpParams->SetConvertBlendsToOutlines(FALSE);
	
	// WEBSTER - markn 28/1/97
	// Always default to non-minimal format
#ifndef WEBSTER
	BOOL MinimalWebFormat = GetLongGadgetValue(_R(IDC_WEBOPTS_MINIMAL), 0, 1, 0, &Valid);
	mpParams->SetMinimalWebFormat(MinimalWebFormat);
#else
	mpParams->SetMinimalWebFormat(FALSE);
#endif // WEBSTER

	// WEBSTER - markn 28/1/97
	// Always default to remove invisible layers
#ifndef WEBSTER
	BOOL RemoveInvisibleLayers = GetLongGadgetValue(_R(IDC_WEBOPTS_INVISIBLELAYERS), 0, 1, 0, &Valid);
	mpParams->SetRemoveInvisibleLayers(RemoveInvisibleLayers);
#else
	mpParams->SetRemoveInvisibleLayers(TRUE);
#endif // WEBSTER

	// WEBSTER - markn 28/1/97
	// Always default to DON'T remove unused colours
#ifndef WEBSTER
	BOOL RemoveUnusedColours = GetLongGadgetValue(_R(IDC_WEBOPTS_UNUSEDCOLOURS), 0, 1, 0, &Valid);
	mpParams->SetRemoveUnusedColours(RemoveUnusedColours);
#else
	mpParams->SetRemoveUnusedColours(FALSE);
#endif // WEBSTER

	// Get bitmap compression quality slider value
	INT32 Quality = 75;
	INT32 SliderState = GetLongGadgetValue(_R(IDC_WEBOPTS_QUALITY), 0, 100, 0, &Valid);
	if (Valid)
	{
		// Get that quality setting
		Quality = 100 - SliderState; 
	}

	BOOL MinimiseBmpSize = GetLongGadgetValue(_R(IDC_WEBOPTS_MINIMISEBITMAPRES), 0, 1, 0, &Valid);
	if (!MinimiseBmpSize)
	{
		// Set the bitmap compression to a large value to indicate none required.
		// Actually add 101 to the quality value so we remember the old value and signify that the
		// switch is off
		mpParams->SetBmpCompression(Quality + 101);
	}
	else
	{
		// return the value on the slider to the caller
		mpParams->SetBmpCompression(Quality); 
	}

	//Added by Graham 21/5/97
	//Get the state of the "Put HTML on clipboard" radio button
	//This applies in both Webster and camelot
	BOOL PutHTMLOnClipboard = GetLongGadgetValue(_R(IDC_WEBOPTS_HTMLTAG), 0, 1, 0, &Valid);
	if (Valid)
		mpParams->SetHTMLToClipboard(PutHTMLOnClipboard);


	return NativePrefsDlg::CommitDialogValues ();
}

/******************************************************************************************

>	MsgResult WebPrefsDlg::Message(CDlgMessage DlgMsg, CGadgetID Gadget) 


	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/5/96
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Handles all the web preferences dialog's messages
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

MsgResult WebPrefsDlg::Message(Msg* Message)
{
	if (IS_OUR_DIALOG_MSG(Message) && mpParams)
	{
		DialogMsg* Msg = (DialogMsg*)Message;

		// Should now handle the required messages that we respond to
		switch (Msg->DlgMsg)
		{
			case DIM_LFT_BN_CLICKED:
				// A control on the dialog box has been clicked...
				switch (Msg->GadgetID)
				{
					case _R(IDC_WEBOPTS_MINIMISEBITMAPRES):
					{
						BOOL Valid = TRUE;
						BOOL MinimiseBmpSize = GetLongGadgetValue(_R(IDC_WEBOPTS_MINIMISEBITMAPRES), 0, 1, 0, &Valid);

						// Grey the slider according to the minimise bitmaps switch
						EnableGadget(_R(IDC_WEBOPTS_QUALITY), MinimiseBmpSize);
						EnableGadget(_R(IDC_WEBOPTS_PERCENT), MinimiseBmpSize);
						EnableGadget(_R(IDC_WEBOPTS_0PERCENT), MinimiseBmpSize);
						EnableGadget(_R(IDC_WEBOPTS_100PERCENT), MinimiseBmpSize);
					}
					break;

// WEBSTER - Graham 8/7/97
// No left button click handing needed for this button
#ifndef WEBSTER

					case _R(IDC_WEBOPTS_MINIMAL):
					{
						BOOL Valid = TRUE;
						BOOL MinimalWebFormat = GetLongGadgetValue(_R(IDC_WEBOPTS_MINIMAL), 0, 1, 0, &Valid);

						// These switches are only appropriate if we are not in minimal mode
						EnableGadget(_R(IDC_WEBOPTS_INVISIBLELAYERS), !MinimalWebFormat);
						EnableGadget(_R(IDC_WEBOPTS_UNUSEDCOLOURS), !MinimalWebFormat);
					}
					break;
#endif // WEBSTER
				}


			break; // DIM_LFT_BN_CLICKED		

			case DIM_SLIDER_POS_CHANGING:
				// special message for the JPEG qualty control
				// Messages to all the controls, handled individually
				switch (Msg->GadgetID)
				{
					case _R(IDC_WEBOPTS_QUALITY):
					{
						// Find the current quality scroller's position
						TCHAR Str[32];
						BOOL Valid;
						INT32 Result = GetLongGadgetValue(_R(IDC_WEBOPTS_QUALITY), 0, 100, 0, &Valid);
						Result = 100 - Result;

						// Build the Percentage string and set it
						String_32 jcf(_R(IDS_PERCENT_FORMAT));
						wsprintf(Str, jcf, (INT32) Result);
						String_32 PercentStr(Str);
						SetStringGadgetValue(_R(IDC_WEBOPTS_PERCENT), &PercentStr);
					}
					break;
				}
			break; // DIM_SLIDER_POS_CHANGING		
		}
	}

	return NativePrefsDlg::Message(Message); 
}

/******************************************************************************************

>	BOOL WebPrefsDlg::Init ()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/5/96
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	WebPrefsDlg Init method
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

BOOL WebPrefsDlg::Init ()
{
	BOOL InitOK;

	InitOK = RegisterOpDescriptor(
								0,					/* Tool ID */
								_R(IDS_WEBPREFSDLG),
								CC_RUNTIME_CLASS(WebPrefsDlg),
								OPTOKEN_WEBPREFSDLG,
								GetState,
								0,					/* help ID */
								0,  				/* bubble help */
								0,					/* resource ID */
								0					/* control ID */
								);

	return (InitOK);
}

/********************************************************************************************

>	BOOL WebPrefsDlg::Create()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/5/96
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if successful, else FALSE
	Purpose:	WebPrefsDlg create method 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
	   
BOOL WebPrefsDlg::Create()
{                     
	// Pass the responsibility to the baseclass.
	return NativePrefsDlg::Create ();
}

/******************************************************************************************

>	BOOL WebPrefsDlg::CheckIfCompatibleBitmaps(Document* pDoc)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/8/96
	Inputs:		pointer to the document to check
	Outputs:	-
	Returns:	TRUE if compatible bitmaps are present, else FALSE
	Purpose:	Checks if any of the bitmaps in the specified document are JPEGable 
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

BOOL WebPrefsDlg::CheckIfCompatibleBitmaps(Document* pDoc)
{
	ERROR2IF(pDoc == NULL,FALSE,"WebPrefsDlg::CheckIfCompatibleBitmaps null pDoc");

	BitmapList* pBitmaps = pDoc->GetBitmapList();

	// The bitmap list contains bitmaps which may not actually be being used,
	// so lets find the ones that are really being used ...

	BOOL CompatibleBitmapsFound = FALSE;

	if (pBitmaps)
	{
		// Go through all the bitmaps in the list 
		ListItem *Ptr = pBitmaps->GetHead();
		while (Ptr != NULL)
		{
			KernelBitmap* pBmp = (KernelBitmap*)Ptr;

			if (pBmp->IsUsedInDocument(pDoc))
			{
				// If it gets here then you've found a bitmap that is being used in the document
				// Check the colour depth of the bitmap, if its JPEGable then set our flag True
				UINT32 Bpp = pBmp->GetBPP();
				// Check if the bitmap has a transparent colour or masking colour present.
				// If so then we may need to change our saving strategy
				INT32 TransparentColour = -1;
				if (Bpp <= 8)
					pBmp->GetTransparencyIndex(&TransparentColour);
				// See if we have a BitmapSource for the bitmap	
				BitmapSource* pSource = NULL;
				BaseBitmapFilter* pDummyFilter;
				BOOL OriginalSourcePresent = pBmp->GetOriginalSource(&pSource, &pDummyFilter);
				
				// Only JPEGable at present, if 24bpp and 8bpp but not 8bpp with a transparent colour
				if (!OriginalSourcePresent && (Bpp == 24 || (Bpp == 8 && TransparentColour == -1)))
					CompatibleBitmapsFound = TRUE;
			}
			
			Ptr = pBitmaps->GetNext(Ptr);
		}
	}

	return CompatibleBitmapsFound;
}

/******************************************************************************************

>	BOOL WebPrefsDlg::InitDialog(WebPrefsDlg* pWebPrefs)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/5/96
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if successful, else FALSE
	Purpose:	Sets initial dialog values 
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

BOOL WebPrefsDlg::InitDialog ( void )
{
	ERROR2IF(mpParams == NULL, FALSE, "WebPrefsDlg::InitDialog called after duff initialisation?!");

	// Set up the save spread/selection radio buttons
	// Rewritten by Graham 6/8/97

	//First, do we have a selection?
	Application * pApp = GetApplication();

	ERROR2IF(pApp == NULL, FALSE,"WebPrefsDlg::InitDialog -  no application!");
	
	SelRange* pRange = pApp->FindSelection();
	
	ERROR2IF(pRange == NULL, FALSE,"WebPrefsDlg::InitDialog - no selection range!");
	DocRect ClipRect = pRange->GetBoundingRect();
	
	BOOL fThereIsASelection=!ClipRect.IsEmpty();

	//Now, is there a selection?
	if (fThereIsASelection)
	{
		// Yes. So ungrey both the buttons for selecting the nodes to export.
		EnableGadget(_R(IDC_NATIVEOPTS_SELECT), TRUE);
		EnableGadget(_R(IDC_NATIVEOPTS_DRAWING), TRUE);

		// Graeme (25-1-00) - and Also the viewport buttons.
		EnableGadget(_R(IDC_WEBOPTS_SELECT), TRUE);
		EnableGadget(_R(IDC_WEBOPTS_DRAWING), TRUE);

		//Now, which of those buttons should be selected?
		switch ( mpParams->GetExportSel () )
		{
		case SELECTION:
			// Choose the export pair of buttons.
			SetLongGadgetValue(_R(IDC_NATIVEOPTS_DRAWING), FALSE);
			SetLongGadgetValue(_R(IDC_NATIVEOPTS_SELECT), TRUE);
			break;

		default:
		case DRAWING:
			// Choose the export pair of buttons.
			SetLongGadgetValue(_R(IDC_NATIVEOPTS_DRAWING), TRUE);
			SetLongGadgetValue(_R(IDC_NATIVEOPTS_SELECT), FALSE);
			break;
		}

		// Graeme (25-1-00) - Repeat for the viewport selection.
		switch ( mpParams->GetViewportSel () )
		{
		case SELECTION:
			// Choose the viewport pair of buttons.
			SetLongGadgetValue(_R(IDC_WEBOPTS_DRAWING), FALSE);
			SetLongGadgetValue(_R(IDC_WEBOPTS_SELECT), TRUE);
			break;

		default:
		case DRAWING:
			// Choose the viewport pair of buttons.
			SetLongGadgetValue(_R(IDC_WEBOPTS_DRAWING), TRUE);
			SetLongGadgetValue(_R(IDC_WEBOPTS_SELECT), FALSE);
			break;
		}
	}
	else
	{
		//No. So grey the SELECTION button and ungrey
		//the DRAWING button
		EnableGadget(_R(IDC_NATIVEOPTS_SELECT), FALSE);
		EnableGadget(_R(IDC_NATIVEOPTS_DRAWING), TRUE);

		// Graeme (25-1-00) - Added to grey the viewport controls.
		EnableGadget(_R(IDC_WEBOPTS_SELECT), FALSE);
		EnableGadget(_R(IDC_WEBOPTS_DRAWING), TRUE);

		//And we must select the DRAWING button for the export area controls.
		SetLongGadgetValue(_R(IDC_NATIVEOPTS_DRAWING), TRUE);
		SetLongGadgetValue(_R(IDC_NATIVEOPTS_SELECT), FALSE);

		// Graeme (25-1-00) - And set the selection for the viewport area.
		SetLongGadgetValue(_R(IDC_WEBOPTS_DRAWING), TRUE);
		SetLongGadgetValue(_R(IDC_WEBOPTS_SELECT), FALSE);
	}

	// Preview bitmap switch is Remove preview bitmap and so is reverse logic
	SetLongGadgetValue(_R(IDC_WEBOPTS_PREVIEWBITMAP), !mpParams->GetExportPreviewBitmap());

	SetLongGadgetValue(_R(IDC_WEBOPTS_TEXTOUTLINES), mpParams->GetConvertTextToOutlines());

#ifndef WEBSTER
	// WEBSTER - markn 28/1/97
	// Most buttons removed
	SetLongGadgetValue(_R(IDC_WEBOPTS_MINIMAL), mpParams->GetMinimalWebFormat());

	SetLongGadgetValue(_R(IDC_WEBOPTS_INVISIBLELAYERS), mpParams->GetRemoveInvisibleLayers());

	SetLongGadgetValue(_R(IDC_WEBOPTS_UNUSEDCOLOURS), mpParams->GetRemoveUnusedColours());

	// These switches are only appropriate if we are not in minimal mode
	EnableGadget(_R(IDC_WEBOPTS_INVISIBLELAYERS), !mpParams->GetMinimalWebFormat());
	EnableGadget(_R(IDC_WEBOPTS_UNUSEDCOLOURS), !mpParams->GetMinimalWebFormat());
#endif // WEBSTER

	//Graham 21/5/97
	//Set the "Put HTML on clipboard switch"
	SetLongGadgetValue(_R(IDC_WEBOPTS_HTMLTAG), mpParams->GetHTMLToClipboard());

	// Set up the quality slider and percentage display
	INT32 Quality = mpParams->GetBmpCompression();	// Default Quality

	//Graham 8/7/97: If we're in Webster, the default value of the minimise
	//bitmap size switch is FALSE. In CorelXARA, it's TRUE.
	// This has been removed as it breaks the system. The Quality is what really
	// controls this. This means that the user is shown no compression but the quality
	// setting means that it will compress! Neville 10/10/97
	BOOL MinimiseBmpSize = TRUE;
	// If the value is bigger than 100 then minimise bitmap resolution
	// is False
	if (Quality > 100)
	{
		// Move the range back down into the 0-100 range
		// 101 - 201 means JPEG compression off but this is the old JPEG slider position
		Quality -= 101;
		// Double checked that we are in the required range
		if (Quality < 0 || Quality > 100)
			Quality = 100; 
		
		MinimiseBmpSize = FALSE;
	}

	SetLongGadgetValue(_R(IDC_WEBOPTS_MINIMISEBITMAPRES), MinimiseBmpSize);

	// Set the range of the slider control plus a step value of 1
	SetGadgetRange(_R(IDC_WEBOPTS_QUALITY), 0, 100, 1);
	SetGadgetBitmaps(_R(IDC_WEBOPTS_QUALITY), _R(IDB_SLIDERBASE), _R(IDB_SLIDERSLIDER));
	SetLongGadgetValue(_R(IDC_WEBOPTS_QUALITY), 100 - Quality);
	
	// Set the percentage string
	TCHAR Str[32];
	String_32 jcf(_R(IDS_PERCENT_FORMAT));
	_stprintf(Str, jcf, Quality);
	String_32 PercentStr(Str);
	SetStringGadgetValue(_R(IDC_WEBOPTS_PERCENT), &PercentStr);

	Document* pDoc = Document::GetSelected();
	BOOL CompatibleBitmaps = CheckIfCompatibleBitmaps(pDoc);
	if (CompatibleBitmaps)
	{
		EnableGadget(_R(IDC_WEBOPTS_MINIMISEBITMAPRES), TRUE);
	}
	else
	{
		EnableGadget(_R(IDC_WEBOPTS_MINIMISEBITMAPRES), FALSE);
		// Force the JPEG slider to be greyed
		MinimiseBmpSize = FALSE;
	}
	
	// Grey the slider according to the minimise bitmaps switch
	EnableGadget(_R(IDC_WEBOPTS_QUALITY), MinimiseBmpSize);
	EnableGadget(_R(IDC_WEBOPTS_PERCENT), MinimiseBmpSize);
	EnableGadget(_R(IDC_WEBOPTS_0PERCENT), MinimiseBmpSize);
	EnableGadget(_R(IDC_WEBOPTS_100PERCENT), MinimiseBmpSize);

	return TRUE;
}

