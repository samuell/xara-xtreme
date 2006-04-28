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

#include "camtypes.h"  
//#include "bfxatom.h"
#include "bmapprev.h"
//#include "bmpfiltr.h"
//#include "resource.h"
//#include "bmpres.h"		// general bitmap filter based resources
//#include "bmpreres.h"
#include "bmpexprw.h"	// for BitmapExportPreviewDialog
#include "exjpeg.h"
//#include "filtrres.h"	// _R(IDS_BMPPREFS_DPITOOSMALL) _R(IDS_BMPPREFS_DPITOOBIG)
//#include "giffiltr.h"
#include "helpuser.h"
//#include "xshelpid.h"
//#include "helppath.h"
#include "layer.h"
#include "lineattr.h"
//#include "makebmp.h"
#include "page.h"
#include "pngfiltr.h"
#include "spread.h"
//#include "will3.h"
#include "fileutil.h"
#include "filtrmgr.h"	//For FilterManager::FindFilterFromID
#include "filtimag.h"	//The imagemap filter
//#include "resimmap.h"	//For imagemap resources
//#include "mfcdlg.h"	
#include "sgliboil.h"	//For SGLibOil::FileExists()
#include "strlist.h"	//For class StringListItem
#include "statline.h"	// for the status line
#include "aprps.h"
//#include "opbevel.h"
#include "ngprop.h"
//#include "dlgcol.h"		// DialogColourInfo
//#include "desnotes.h"	//  Design notes
//#include "registry.h"
#include "gpalopt.h"
#include "osrndrgn.h"
//#include "rikdlg.h"		// _R(IDB_SLIDERBASE), _R(IDB_SLIDERSLIDER)
//#include "justin.h"		// for _R(IDS_ZOOM_INFO_FORMAT) a % readout
#include "oilfltrs.h"	// for find filter ext
//#include "exprwres.h"	// for _R(IDS_BITMAPPREVIEWDIALOG)
#include "units.h"

DECLARE_SOURCE("$Revision$");

CC_IMPLEMENT_DYNCREATE(BmapPrevDlg, DialogTabOp)

#define new CAM_DEBUG_NEW

//typedef UINT32 JPEG_QUALITY;

const CDlgMode BmapPrevDlg::Mode	= MODAL;			// Mode of the dialog  
const UINT32 BmapPrevDlg::IDD			= _R(IDD_BMAPPREVDLG);	// Used to uniquely identify the tabbed dialog
														// there is no actual resource associated with
														// this ID.
#define BUBBLE_TIME				800
#define BUBBLE_HELP_TIMMER_ID	1000

#define BMPEXP_MAXWIDTH			16383
#define BMPEXP_MAXHEIGHT		16383
#define BMPEXP_MAXDPI			5000
#define BMPEXP_MINDPI			5

PathName BmapPrevDlg::m_pthExport;

BOOL BmapPrevDlg::m_bUseExistingPalette			= FALSE;
//ExportFileType BmapPrevDlg::m_OriginalFileType	= GIF_TYPE;
BOOL BmapPrevDlg::m_bClickedOnExport			= FALSE;
BOOL BmapPrevDlg::m_bIsOrderedDither			= TRUE;
BOOL BmapPrevDlg::m_bIsCreateBitmap				= FALSE;
BOOL BmapPrevDlg::m_bSlicingImage				= FALSE;
INT32 BmapPrevDlg::m_NumOfColoursUserAskedFor		= 256;
BitmapExportOptions* BmapPrevDlg::m_pExportOptions = NULL; // static ptr to export options is NULL if it is not handling an export object

bool BmapPrevDlg::m_MovedWindow = false;


struct ChildWindowParameters
{
	wxWindow* FirstID;
	wxWindow* SecondID;
	INT32 Distance;
	INT32 TopOffset;
};


BOOL BmapPrevDlg::GetNumberOfTransparentColours()
{
//	ASSERT(FALSE);
	return 0;
}

INT32 BmapPrevDlg::GetTransparentColour(INT32 Index)
{
//	ASSERT(FALSE);
	return 0;
}

/********************************************************************************************
>	OpState	BmapPrevDlg::GetState(String_256*, OpDescriptor*)
	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/11/94
	Purpose:	The standard GetState fn, does nothing
********************************************************************************************/
OpState	BmapPrevDlg::GetState(String_256*, OpDescriptor*)
{    
	OpState OpSt;
	return(OpSt);
}
		 

/********************************************************************************************
>	BOOL BmapPrevDlg::Init()
	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/11/94
	Purpose:	The standard Init fn for the op. 
********************************************************************************************/
BOOL BmapPrevDlg::Init()
{  
	return (RegisterOpDescriptor(	0,
 									_R(IDS_BMAPPREVDLG), 
									CC_RUNTIME_CLASS(BmapPrevDlg),
 									OPTOKEN_GIFTABDLG,
 									BmapPrevDlg::GetState));
} 

/********************************************************************************************
>	virtual BOOL BmapPrevDlg::RegisterYourPagesInOrderPlease()
	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/11/94
	Returns:	return TRUE if all calls to AddAPage returned TRUE
	Purpose:	This virtual function will get called to give you a chance to add pages to your
				DialogTabOp. This function will usually make multiple calls to AddAPage to 
				register the initial set of pages to be contained in the tab dialog. The pages
				should all be the same size and be registered in the left to right order in which
				you wish them to appear. 

				This function get's called directly from the Create method, Don't call it 
				explicitly yourself. If you return FALSE from this method then the Create 
				method will fail. 

	SeeAlso:	DialogTabOp::AddPage, DialogOp::Create
********************************************************************************************/
BOOL BmapPrevDlg::RegisterYourPagesInOrderPlease()
{
	BOOL ok = TRUE;

	// flag indicating we are doing a bitmap export
	BOOL bExportABitmap = (m_pExportOptions->GetSelectionType() == ABITMAP);

	if (bExportABitmap)
	{
		ok = ok && AddAPage(_R(IDD_TBITMAPOPTIONS));
	}
	else
	{
PORTNOTE("other","Removed registry usage")
#ifndef EXCLUDE_FROM_XARALX
		// Not exporting a bitmap from the bitmap gallery
		AddAPage(_R(IDD_TPALETTE));
#endif

		if (!m_bSlicingImage)
			ok = AddAPage(_R(IDD_TBITMAPSIZE));
	}

	// add the imagemap and browser pages
	if (ok && !bExportABitmap && m_FilterType != MAKE_BITMAP_FILTER && !m_bSlicingImage)
		ok =  AddAPage(_R(IDD_TBITMAPOPTIONS)) && AddAPage(_R(IDD_TIMAPOPTIONS)) && AddAPage(_R(IDD_TBROWSER));

	// design notes stuff on the image map page is needed in image slicing
	if (ok && m_bSlicingImage)
		ok = AddAPage(_R(IDD_TIMAPOPTIONS));

	return (ok); 
}


/********************************************************************************************
>	BmapPrevDlg::BmapPrevDlg(): DialogTabOp(BmapPrevDlg::IDD, BmapPrevDlg::Mode) 
	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/11/94
	Purpose:	The Constructor of the DialogTabOp derived class simply sets modality of the
				dialog, and it's ID.
********************************************************************************************/
BmapPrevDlg::BmapPrevDlg()
  : DialogTabOp(BmapPrevDlg::IDD, BmapPrevDlg::Mode, CC_RUNTIME_CLASS(DialogOp), 0)
  ,	m_pPreviewDlg(NULL)
  , m_DialogWnd(NULL)
  , m_bImageMapTabCreated(false)
  , m_bBrowserPreviewTabCreated(false)
  , m_ClipBoardUsage(CLIPBOARD_NONE)	// which of the two clipboard check boxes are set
  , m_MouseInsidePaletteControl(true)	// at this point we don't know where the mouse is so this is a safe option
  ,	m_pBubbleWnd(0)						// used by bubble help
  , m_LastCursorOverControlID(0)		// used by bubble help
  , m_pBubbleTimer(0)					// timer is only created when we need it
{
	// nasty bit to quickly read the user last used value from the registry
	m_ColorModelType = MODEL_INVALID;
PORTNOTE("other","Removed registry usage")
#ifndef EXCLUDE_FROM_XARALX
	HKEY optionsKey = OpenRegKeyReadOnly(hAppStateRegKey, TEXT("Options"));
	if (optionsKey)
	{
		HKEY palettesKey = OpenRegKeyReadOnly(optionsKey, TEXT("Palettes"));
		if (palettesKey)
		{
			m_ColorModelType = (ColourTypes)GetRegInt(palettesKey, TEXT("Display"));
			CloseRegKey(palettesKey);
		}
		CloseRegKey(optionsKey);
	}
#endif
	if (m_ColorModelType != MODEL_RGBHEX	&& m_ColorModelType != MODEL_RGBPERCENT && 
		m_ColorModelType != MODEL_RGBNUMBER	&& m_ColorModelType != MODEL_HSV)
	{
		m_ColorModelType = MODEL_RGBPERCENT;	// fall back value
	}

	//  The first time that the dialog appears, we want the palette displayed in 'By Use'
	//  mode, so we have to work out the number of times each colour is used in the image.
	m_bPopularityValuesWanted = TRUE;

	m_LockSizeUpdates = FALSE;

	// Have to initialise a static variable in the palette right-click menu class.
//	OpPalettePopupCommand::m_pDialog = this;
}

    
/******************************************************************************************
>	BmapPrevDlg::~BmapPrevDlg()
	Author:		Alex_Price (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/07/99
	Purpose:	Destructor required to reclaim dynamically allocated memory.
******************************************************************************************/
BmapPrevDlg::~BmapPrevDlg()
{
	//  If a bubble-help window exists, then reclaim the memory.
PORTNOTE("other","Removed BubbleHelpWnd usage")
#if !defined(EXCLUDE_FROM_XARALX)
	delete m_pBubbleWnd;
#endif
	delete m_pBubbleTimer;
/*	
	if( m_FilterType == MAKE_BITMAP_FILTER )
	{
		//  Do nothing at the moment.
		//  In this case, we reset the variable later on.
	}
	else
	{
		//  Reset the following variable
		GRenderOptPalette::UseOldPalette = TRUE;
	}
*/
PORTNOTE("other","Removed registry usage")
#if !defined(EXCLUDE_FROM_XARALX)
	HKEY optionsKey = OpenRegKey( hAppStateRegKey, TEXT("Options") );
	if (!optionsKey)
		optionsKey = CreateRegKey(hAppStateRegKey, TEXT("Options") );
	if (optionsKey)
	{
		HKEY palettesKey = OpenRegKey(optionsKey, TEXT("Palettes") );
		if (!palettesKey)
			palettesKey = CreateRegKey(optionsKey, TEXT("Palettes") );
		if (palettesKey )
		{
			//  Set the relevant value in the key.
			SetRegInt(palettesKey, TEXT("Display"), m_ColorModelType);
			CloseRegKey(palettesKey);
		}
		CloseRegKey(optionsKey);
	}
#endif
/*
	// If we are not dealing with the create bitmap copy dialog, then reset all the 
	// transparency settings back to their default values.
	//
	// Graeme (12/7/00) - I've changed this so that it doesn't clear up the
	// transparency values when the dialogue is slicing images. This is because it
	// kills off the transparency settings before it's finished with them. In this
	// case clean-up takes place in OpSlice::Do ().
//	if( !m_bIsCreateBitmap && !m_bSlicingImage )
//		CancelTransparency();
*/
}


/********************************************************************************************
>	void BmapPrevDlg::DoWithParam(OpDescriptor*, OpParam* pParam)
	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/11/94
	Purpose:	Like all Do functions invokes the BmapPrevDlg
********************************************************************************************/
void BmapPrevDlg::DoWithParam(OpDescriptor*, OpParam* pParam)
{
	BOOL ok = TRUE;

	ERROR3IF(pParam == NULL, "pParam = NULL");

	// remember the options

	// this should be the only place where the export options are deleted
	// so delete them if we have a new and valid and different set of options to replace them
	if (m_pExportOptions && m_pExportOptions != (BitmapExportOptions *)(void *)pParam->Param1)
	{
		delete m_pExportOptions;
		m_pExportOptions = NULL;
	}

	m_pExportOptions = (BitmapExportOptions *)(void*)pParam->Param1;

	// remember the bitmap filter
	m_pBmpFilter = (BaseBitmapFilter *)(void*)pParam->Param2;

	// safety check
	ERROR3IF((m_pExportOptions == NULL) || (m_pBmpFilter == NULL), "No filter or export options");
	if ((m_pExportOptions == NULL) || (m_pBmpFilter == NULL))
	{
		End();
		return;
	}

	// get the filter ID
	m_FilterType = m_pExportOptions->GetFilterType();
	//  The following is to stop the BitmapExportPreviewDialog from trying to
	//  generate popularity values when we are exporting a jpeg ( and hence there is no palette );
	if( m_FilterType == JPEG )
		m_bPopularityValuesWanted = FALSE;

	// iniitialize the preview dialog
	m_pPreviewDlg = NULL;
	m_DialogWnd = NULL;
	
	m_CurrentPreview = 0;
	m_bDpiSupported	= TRUE; 

	// assume for the meantime that we are not doing a create bitmap copy
	m_bIsCreateBitmap = FALSE;
	
	// oh ye virtuals
	switch ( m_FilterType ) 
	{
		case GIF:
		case TI_GIF:
		{
PORTNOTETRACE("other", "Removed CamelotEPSFilter check" );
#if !defined(EXCLUDE_FROM_XARALX)
			GIFExportOptions* pGIFOptions = (GIFExportOptions*)m_pExportOptions;
			ERROR3IF(!pGIFOptions->IS_KIND_OF(GIFExportOptions), "pGIFOptions isn't");

			m_bDpiSupported = FALSE;
//			m_bTransparent = pGIFOptions->WantTransparent();

			// > 8bpp not leled by the gif filter
			if (pGIFOptions->GetDepth() > 8)
				pGIFOptions->SetDepth(8);
#endif
			break;
		}
		case MAKE_BITMAP_FILTER:
		{
PORTNOTETRACE("other", "Removed CamelotEPSFilter check" );
#if !defined(EXCLUDE_FROM_XARALX)
			MakeBitmapExportOptions* pMkBOptions = (MakeBitmapExportOptions*)m_pExportOptions;
			ERROR3IF(!pMkBOptions->IS_KIND_OF(MakeBitmapExportOptions), "pMkBOptions isn't");

			m_bDpiSupported = TRUE;
//			m_bTransparent = pMkBOptions->WantTransparent();

			// Find the png filter 
			Filter *pFilter = Filter::GetFirst();
			while (pFilter != NULL)
			{
				if ((pFilter->GetFlags().CanExport) && 
					(IS_A(pFilter,PNGFilter)))
					// This is the filter!
					break;

				// Try the next filter
				pFilter = Filter::GetNext(pFilter);
			}

			// check for no filter found
			if (pFilter == NULL)
				ok = FALSE;

			// convert the options to png options
			PNGExportOptions *pPNGOptions = new PNGExportOptions(PNG, NULL);
			if (pPNGOptions == NULL)
				ok = FALSE;
			else if (ok)
			{
				// copy the values across
				pPNGOptions->CopyFromMasked(pMkBOptions);

				// delete the old options (these would be the makebitmap ones)
				if (m_pExportOptions)
					delete m_pExportOptions;

				// set the new png options as current
				m_pExportOptions = pPNGOptions;

				// remove the export options from this filter
				// which deletes the export options
				//if (m_pBmpFilter)
				//	m_pBmpFilter->SetExportOptions(NULL);

				// remember the filter
				m_pBmpFilter = (BaseBitmapFilter*)pFilter;

				m_pBmpFilter->SetExportOptions(pPNGOptions);

			}
			m_bIsCreateBitmap = TRUE;
#endif
			break;
		}
		case PNG: 
		{
//			PNGExportOptions* pPNGOptions = (PNGExportOptions*)m_pExportOptions;
//			ERROR3IF(!pPNGOptions->IS_KIND_OF(PNGExportOptions), "pPNGOptions isn't");
			// 1, 4, 8, 24 and 32 bpp
//			m_bTransparent = pPNGOptions->WantTransparent();

			m_bDpiSupported = TRUE;
			break;
		}

		case BMP_UNCOMPRESSED:
			m_bDpiSupported = TRUE;
			break;

		case JPEG:
			m_bDpiSupported = TRUE;
			m_pExportOptions->SetDepth(24); // init to 24bpp
			break;

		default:
			m_bDpiSupported = FALSE;
			break;
	}

	// Create the TAB dialog please
	if (ok)
		ok = Create(); 

	if (!ok)
	{
		m_pExportOptions->MarkInvalid();
		//  Need to use this to indicate that the user clicked on Cancel after having
		//  changed the file type.
		m_bClickedOnExport = FALSE;
		End(); 
	}
} 

void BmapPrevDlg::SetButtonsText()
{
	CDlgResID PageID = GetCurrentPageID();	// Get currently selected Tab id
	TalkToPage(0);						// Select the main tab

	UINT32 ResID;

	if (NamedExportProp::m_fApplyNotExport)
		switch (m_CurrentPreview)
		{
			case 1:		ResID = _R(IDS_PREVIEW_APPLYA);			break;
			case 2:		ResID = _R(IDS_PREVIEW_APPLYB);			break;
			default:	ResID = _R(IDS_PREVIEW_APPLY);			break;
		}
	else if (m_FilterType == MAKE_BITMAP_FILTER)
		switch (m_CurrentPreview)
		{
			case 1:		ResID = _R(IDS_PREVIEW_SAVEA);			break;
			case 2:		ResID = _R(IDS_PREVIEW_SAVEB);			break;
			default:	ResID = _R(IDS_SAVE_BUTTON);			break;
		}
	else
		switch (m_CurrentPreview)
		{
			case 1:		ResID = _R(IDS_PREVIEW_EXPORTA);		break;
			case 2:		ResID = _R(IDS_PREVIEW_EXPORTB);		break;
			default:	ResID = _R(IDS_EXPORT_BUTTON);			break;
		}

	SetStringGadgetValue(wxID_OK, ResID);
	
	// Use the tabbed dialogs 'Apply' button as a 'Preview' button
	SetStringGadgetValue(_R(ID_APPLY_NOW), _R(IDS_PREVIEW_BUTTON));

	TalkToPage(PageID);			// Select the originally selected tab
}

void BmapPrevDlg::SetPreviewButtonState(bool enabled)
{
	TRACEUSER( "Gerry", _T("SetPreviewButtonState matches = %d\n"), m_pExportOptions->DoesTempFileMatchExportOptions());
	
	CDlgResID PageID = GetCurrentPageID();		// Get currently selected Tab id
	TalkToPage(0);							// Select the main tab
	EnableGadget(_R(ID_APPLY_NOW), !m_pExportOptions->DoesTempFileMatchExportOptions() /*enabled == 1*/);
	TalkToPage(PageID);							// Select the originally selected tab
}


/********************************************************************************************
>	BOOL CALLBACK EnumChildProc(wxWindow* hChild, LPARAM lParam )
	Author:		Stefan_Stoykov (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/07/97
	Inputs:		hChild - a child window
				lParam - pointer to some parameters
	Purpose:	Windows calls this function in turns, passing a handle each child window.
				Each child window (which is part of the original tabed dialog is moved down
				to leave space for the preview. 
	Returns:	TRUE, if successful, FALSE otherwise
	See Also:	BmapPrevDlg::OnCreate
	NOTE:		This is oily code and should become part of the dialog manager
********************************************************************************************/
PORTNOTE("other","Removed Windows style function EnumChildProc")
#ifndef EXCLUDE_FROM_XARALX
BOOL CALLBACK EnumChildProc(wxWindow* hChild, LPARAM lParam )
{
	// First get our parameters back
	ChildWindowParameters* pParam = (ChildWindowParameters*)lParam;
	if (pParam == NULL)
		return FALSE;

	//Get the ID of our parent window
	wxWindow* hParent = (wxWindow*)pParam->FirstID;
	if (hParent == NULL)
		return FALSE;

	// first find the true parent of the child window
	// and then compare the parent with our dialog
	if (GetParent(hChild) != hParent)
		return TRUE;

	//If this is the Preview dialog child window, return TRUE
	if (GetParent(hChild) == (wxWindow*) pParam->SecondID)
		return TRUE;

	//move the window

	// get the size of the child window
	wxRect Rect;
	GetWindowRect(hChild, &Rect);

	wxRect Parent;
	GetWindowRect(hParent, &Parent);
	
	INT32 Offset = pParam->Distance - pParam->TopOffset;
	MoveWindow(hChild, Rect.GetLeft() - Parent.GetLeft(), Rect.GetTop() - Parent.GetTop() + Offset,
					   Rect.GetRight() - Rect.GetLeft(), Rect.GetBottom() - Rect.GetTop(), TRUE);

	return TRUE;
}
#endif						

/********************************************************************************************
>	BOOL BmapPrevDlg::OnCreate()
	Author:		Stefan_Stoykov (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/7/97
	Purpose:	Handles the DIM_CREATE message. Creates the preview dialog and moves the tabs, 
				so the preview dialog goes above them.
	Returns:	TRUE, if successful, FALSE otherwise
	NOTE:		The window movement code is oily code and should become part of the dialog manager
				along with the above function, EnumChildProc.
********************************************************************************************/
BOOL BmapPrevDlg::OnCreate()
{
	// If we need to add a preview section then go and do it now
	if ((m_FilterType == JPEG) || (m_pExportOptions->GetSelectionType() != ABITMAP))
	{
PORTNOTETRACE("other","Preview section NOT setup");
#ifndef EXCLUDE_FROM_XARALX
		//First get the size of the current window
		wxRect Rect;
		GetWindowRect(WindowID, &Rect);

		//Now, the current window contains only the Tabs
		//section of the dialog. We need to insert the Preview
		//section of the dialog above the Tabs section. This
		//means doing the following:

		//1. Increasing the height of the dialog by the
		//	 height of the Preview section
		//2. Moving the Tabs section downwards, by a distance
		//	 equal to the height of the Preview section
		//3. Inserting the Preview section at the top of the
		//	 dialog

		//Also, if this is the first time the dialog
		//has been created, we must recentre the dialog on 
		//screen to compensate for the fact that the Preview
		//section has been added. We do this at the same
		//time as step 1.

		//So...

		//First, we need to get the height of the 
		//Preview section

		//To do this, we create the Preview dialog object
		m_pPreviewDlg = new BitmapExportPreviewDialog(WindowID);
		ERROR2IF(m_pPreviewDlg == NULL, FALSE, "No preview dialog!");

		//And start the dialog up
		m_pPreviewDlg->InitPreviewDialog(this, m_pBmpFilter);

		// Get the window ID
		m_DialogWnd = m_pPreviewDlg->WindowID;
		ERROR2IF(m_DialogWnd == NULL, FALSE, "No preview dialog!");

		//And get the Preview Dialog's rectangle
		wxRect rectPreview;
		GetWindowRect(m_DialogWnd, &rectPreview);

		//First we must get the height of the Preview section
		INT32 lPreviewHeight = rectPreview.GetBottom() - rectPreview.GetTop();
		ERROR2IF(lPreviewHeight <= 0, FALSE, "Preview Dialog has no height");

		if (!m_MovedWindow)
		{ 
			// Move the window to take account of it's incressed size (only need to do this
			// the first time the dialog is run) and incress its size
			::MoveWindow(
				WindowID,									// The window
				Rect.GetLeft(),									// Horiz position
				Rect.GetTop() - lPreviewHeight / 2,				// Vertical position
				Rect.GetRight() - Rect.GetLeft(),						// Width
				Rect.GetBottom() - Rect.GetTop() + lPreviewHeight,	// Height
				TRUE										// Repaint option
			);
			m_MovedWindow = true;
		}
		else
		{
			// Just incress the window's size
			::MoveWindow(
				WindowID,									// The window
				Rect.GetLeft(),									// Horiz position
				Rect.GetTop(),									// Vertical position
				Rect.GetRight() - Rect.GetLeft(),						// Width
				Rect.GetBottom() - Rect.GetTop() + lPreviewHeight,	// Height
				TRUE										// Repaint option
			);
		}

		// Now step 2. We need to move the tabbed section
		// downwards by the height of the Preview dialog.
		// Remember, take into account the height of the tabs which will be the top offset
		ChildWindowParameters cwpToPass;
		cwpToPass.FirstID	= WindowID;
		cwpToPass.SecondID	= m_DialogWnd;
		cwpToPass.Distance	= lPreviewHeight;
		cwpToPass.TopOffset	= 28;

		EnumChildWindows(WindowID, EnumChildProc, (LPARAM)&cwpToPass);

		//Right and...er...move the Preview section again.
		//I've no idea how the preview section got moved,
		//but it does, and moving it again does actually
		//produce the right effect, so let's not question
		//why.
		MoveWindow(m_DialogWnd, 0, 0, rectPreview.GetRight() - rectPreview.GetLeft(), rectPreview.GetBottom() - rectPreview.GetTop(), TRUE);
		
		// enable our window (which has been disabled when the preview window was created)
		EnableWindow(WindowID, TRUE);
#endif
	}

	// set the title bar
	if (m_FilterType != MAKE_BITMAP_FILTER) 
	{
		// Set up the title of the dialog box according to the passed in string which
		// is the name of the filter plus export bitmap options.
		String_256 Temp = ""; //*(m_pExportOptions->GetFilterName());
		Temp.Load(m_pExportOptions->GetFilterNameStrID()); // which is safer than the ptr into whatever
		Temp += String_256(_R(IDN_EXPORTBMPOPTS));

		DialogManager::SetTitlebarName(WindowID, &Temp); // set the title bar for the window
	}
	else
	{
		// set up the title string for the MakeBitmap filter
		String_256 Temp(_R(IDS_MAKEBMPOPTS));
		DialogManager::SetTitlebarName(WindowID, &Temp); // set the title bar
	}

	// set the OK and preview button
	SetButtonsText();

	// calculate a dpi value if dpi is not supported
	if (!m_bDpiSupported)							// Doesn't support dpi...
	{
		// and ensure that its always the number of dpi as defined by a pixel unit
		DocUnitList* pDocUnitList =	DocUnitList::GetCurrentDocUnitList();
		ERROR3IF(pDocUnitList == 0, "BmapPrevDlg::OnCreate - no pDocUnitList!");
		Unit* pPixelUnit = pDocUnitList->FindUnit(PIXELS);
		ERROR3IF(pPixelUnit == 0, "BmapPrevDlg::OnCreate - no pixel units!");
		Unit* pInchUnit = pDocUnitList->FindUnit(INCHES);
		ERROR3IF(pInchUnit == 0, "BmapPrevDlg::OnCreate - no inch units!");
		double newDpi = (pPixelUnit->GetMillipoints() > 0)
							? pInchUnit->GetMillipoints() / pPixelUnit->GetMillipoints()
							: 96.0;
		m_pExportOptions->SetDPI(newDpi);
	}

	AddControlsToHelper();

	UpdateCurrentTab();

PORTNOTE("other","Removed Windows style function EnumChildProc")
#ifndef EXCLUDE_FROM_XARALX
	m_PaletteControl.Init(GetReadWriteWindowID());
#endif

	return TRUE;
}

/******************************************************************************************
>	void BmapPrevDlg::DoBubbleHelp()
	Author:		Alex_Price (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/07/99
	Purpose:	This function displays bubble help (if available) for the control
				that the mouse is over
******************************************************************************************/
void BmapPrevDlg::DoBubbleHelp()
{
PORTNOTE("other","Removed registry usage")
#if !defined(EXCLUDE_FROM_XARALX)
	//  If a bubble help window exists from a previous call to this function then delete it
	delete m_pBubbleWnd;
#endif
	m_pBubbleWnd = 0;
	
	//  Are we over a control?  This is the case if this variable is non-zero.
	if ( !m_LastCursorOverControlID )
		return;

	//  This holds the ID of the string we want to display in the bubble-help window.
	UINT32 bubbleID;

	//  Get the bubble-help string from the gadget ID.
	if( m_LastCursorOverControlID == _R(IDC_T2_LOCKED_COLOUR) )			//  'Locked Colour' button.
	{
		bubbleID = _R(IDS_LOCKED_COLOUR);
	}
	else
	if( m_LastCursorOverControlID == _R(IDC_T2_WEB_SAFE_COLOUR) )		//  'Web Safe Colour' button.
	{
		bubbleID = _R(IDS_WEB_SAFE_COLOUR);
	}
	else
	if( m_LastCursorOverControlID == _R(IDC_T2_TRANSPARENT_COLOUR) )	//  'Transparent Colour' button.
	{
		bubbleID = _R(IDS_TRANSPARENT_COLOUR);
	}
	else
	if( m_LastCursorOverControlID == _R(IDC_T2_TRANSPARENT_BACKGROUND) )
	{
		bubbleID = _R(IDS_TRANSPARENT_BACKGROUND);
	}
	else
	if( m_LastCursorOverControlID == _R(IDC_T2_DELETE_COLOUR) )			//  'Delete Colour' button.
	{
		bubbleID = _R(IDS_DELETE_COLOUR);
	}
	else
	if( m_LastCursorOverControlID == _R(IDC_T2_RESTORE_COLOUR) )		//  'Restore Colour' button.
	{
		bubbleID = _R(IDS_RESTORE_COLOUR);
	}
	else
	if( m_LastCursorOverControlID == _R(IDC_T2_SYSTEM_COLOURS) )		//  'Add System Colors' button.
	{
		bubbleID = _R(IDS_SYSTEM_COLOURS);
	}
	else 																// Any other control.
	{
		m_LastCursorOverControlID = 0;
		bubbleID = 0;
	}

	//  If no bubble-help required, then return.
	if (bubbleID == 0)
		return;
	
	//  Load the bubble-help string into a variable.
	String_256 bubbleText(bubbleID);

PORTNOTE("other","Removed registry usage")
#if !defined(EXCLUDE_FROM_XARALX)
	//  Make a new bubble help window
	m_pBubbleWnd = new BubbleHelpWnd;
	if (!m_pBubbleWnd)
	{
		TRACEALL( _T("Unable to create bubble help window in bitmap export dialog\n"));
		return;
	}

	// Create the actual window
	if (!m_pBubbleWnd->Create())
	{
		TRACEALL( _T("Could not Init bubble help window in bitmap export dialog\n"));
		return;
	}

	//  Set the text for the window
	if (!m_pBubbleWnd->SetText(bubbleText))
		return;  // no bubble help for this item

	//  Show the window
	m_pBubbleWnd->Show();
#endif
}

void BmapPrevDlg::PrepareBubbleHelp()
{
	if (m_pBubbleTimer)
		m_pBubbleTimer->Sample();			// reuse the timer created on the last mouse move message
	else
		m_pBubbleTimer = new MonotonicTime; // it is OK for new to fail as this will just return a null
											// pointer and therefore there will be no bubble help
PORTNOTE("other","Removed registry usage")
#if !defined(EXCLUDE_FROM_XARALX)
	// Create a new 
	delete m_pBubbleWnd;
	m_pBubbleWnd = 0;
#endif
}

void BmapPrevDlg::UpdateStatusBar(UINT32 id)
{
	// now do the status line messages		
	UINT32 statusTextID;

	// Find the status message ID from the Gadget ID
	if( id == _R(IDC_T2_LOCKED_COLOUR) )			// 'Locked Colour' button
	{
		statusTextID = _R(IDS_LOCKED_COLOUR_STATUS_TEXT);
	}
	else
	if( id == _R(IDC_T2_WEB_SAFE_COLOUR) )			// 'Web Safe Colour' button
	{
		statusTextID = _R(IDS_WEB_SAFE_COLOUR_STATUS_TEXT);
	}
	else
	if( id == _R(IDC_T2_TRANSPARENT_COLOUR) )		// 'Transparent Colour' button
	{
		statusTextID = _R(IDS_TRANSPARENT_COLOUR_STATUS_TEXT);
	}
	else
	if( id == _R(IDC_T2_TRANSPARENT_BACKGROUND) )
	{
		statusTextID = _R(IDS_TRANSPARENT_BACKGROUND_STATUS_TEXT);
	}
	else
	if( id == _R(IDC_T2_DELETE_COLOUR) )			// 'Delete Colour' button
	{
		statusTextID = _R(IDS_DELETE_COLOUR_STATUS_TEXT);
	}
	else
	if( id == _R(IDC_T2_RESTORE_COLOUR) )			// 'Restore Colour' button
	{
		statusTextID = _R(IDS_RESTORE_COLOUR_STATUS_TEXT);
	}
	else
	if( id == _R(IDC_T2_SYSTEM_COLOURS) )			// 'Add System Colours' button
	{
		statusTextID = _R(IDS_SYSTEM_COLOURS_STATUS_TEXT);
	}
	else											// Nothing - Just show 'Preview...'
	{
		statusTextID = _R(IDS_BITMAPPREVIEWDIALOG);
	}

	//  Put up some status line help
	StatusLine* pStatusLine = GetApplication()->GetpStatusLine();
	if ( pStatusLine != NULL )
	{
		String_256	strStatus(statusTextID);
		pStatusLine->UpdateText( &strStatus, FALSE );
	}
	else
		ERROR3("pStatusLine == NULL");
}


/********************************************************************************************
>	void BmapPrevDlg::SetPaletteSelection(INT32 index)
	Author:		Jonathan_Payne (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/12/2000
	Purpose:	Set the selected colour in the palette control
********************************************************************************************/
void BmapPrevDlg::SetPaletteSelection(INT32 index)
{
PORTNOTETRACE("other", "BmapPrevDlg::SetPaletteSelection - do nothing" );
#if !defined(EXCLUDE_FROM_XARALX)	
	if (GetCurrentPageID() != _R(IDD_TPALETTE))
		return;
	
	ReDrawInfoType info;
	GetKernelRenderedGadgetInfo(_R(IDC_T2_PALETTE_CONTROL), &info);
	m_PaletteControl.SetSelectedCell(&info, index);
	RefreshPaletteLinkedControls();
#endif
}

/********************************************************************************************
>	void BmapPrevDlg::SetPaletteHighlight(INT32 index)
	Author:		Jonathan_Payne (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/12/2000
	Purpose:	Set the highlighted colour in the palette control
********************************************************************************************/
void BmapPrevDlg::SetPaletteHighlight(INT32 index)
{
PORTNOTETRACE("other", "BmapPrevDlg::SetPaletteSelection - do nothing" );
#if !defined(EXCLUDE_FROM_XARALX)	
	if (GetCurrentPageID() != _R(IDD_TPALETTE))
		return;

	ReDrawInfoType info;
	GetKernelRenderedGadgetInfo(_R(IDC_T2_PALETTE_CONTROL), &info);
	m_PaletteControl.SetHighlightedCell(&info, index);
	RefreshPaletteLinkedControls();
#endif
}

/********************************************************************************************
>	void BmapPrevDlg::UpdateCurrentTab()
	Author:		Simon_Knight (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/12/00
	Purpose:	Updates the details in the tab to match the export options
********************************************************************************************/
void BmapPrevDlg::UpdateCurrentTab()
{
	CDlgResID PageID = GetCurrentPageID();	// Get currently selected Tab id
	if(PageID == _R(IDD_TBITMAPSIZE) )
	{
		//TRACEUSER( "Simonk", _T("Update tab SIZE\n"));
		// Refresh the size tab
		RefreshBitmapSizeTab();
	}
	else
	if(PageID == _R(IDD_TPALETTE) )
	{
		//TRACEUSER( "Simonk", _T("Update tab PALETTE\n"));
		RefreshPaletteOptionsTab();
	}
	else
	if(PageID == _R(IDD_TIMAPOPTIONS) )
	{
		//TRACEUSER( "Simonk", _T("Update tab IMAGE MAP\n"));
		// Refresh the image map tab
		RefreshImageMapTab();
	}
	else
	if(PageID == _R(IDD_TBROWSER) )
	{
		//TRACEUSER( "Simonk", _T("Update tab BROWSER\n"));
		// Refresh browser tab
	}
	else
	if(PageID == _R(IDD_TBITMAPOPTIONS) )
	{
		//TRACEUSER( "Simonk", _T("Update tab OPTIONS\n"));
		// for this tab the init works just like the refresh
		InitBitmapOptionsTab();
	}
	else
	{
		//TRACEUSER( "Simonk", _T("Update tab NULL\n"));
	}
}


/********************************************************************************************
>	BOOL BmapPrevDlg::DoPreview()
	Author:		Stefan_Stoykov (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/7/97
	Purpose:	Handles the Apply button. Gets the export options from the gadgets and passes 
				them to the preview dialog.
	Returns:	TRUE, if successful, FALSE otherwise
********************************************************************************************/
BOOL BmapPrevDlg::DoPreview()
{
PORTNOTETRACE("other", "BmapPrevDlg::DoPreview - do nothing - returning true" );
#if !defined(EXCLUDE_FROM_XARALX)
	//	TRACEUSER( "Gerry", _T("DoPreview 0x%08x\n"), m_pExportOptions);

	CDlgResID PageID = GetCurrentPageID();	// Get currently selected Tab id
	if (m_pPreviewDlg == NULL)
		return FALSE;

	BOOL ok = (m_pExportOptions != NULL);

	if (ok && !m_pExportOptions->DoesTempFileMatchExportOptions())
	{
		// Set the flag early as otherwise the set active bitmap call in the OnExport system
		// generates a second preivew
		m_pExportOptions->SetTempFileMatchesExportOptions(TRUE);

		ok = m_pPreviewDlg->OnExport( m_pExportOptions );
	}

	if (!ok) m_pExportOptions->SetTempFileMatchesExportOptions(FALSE);

	if (ok)
	{
		// if first time set the first bitmap as active
		if (m_CurrentPreview == 0)
			m_CurrentPreview = 1;

		// now do the automatic browser preview
//		if ((m_pPreviewDlg != NULL) && m_bDoPreviewInBrowser)
//			m_pPreviewDlg->DoBrowserPreview(t_BrowserOptions);
	}

	// Disable the preview button as the dialog settings currently match the preview
	SetPreviewButtonState(false);

	// Update the palette as it may have been changed when the preview was generated
	m_PaletteControl.RenderSoon();

	TalkToPage(PageID);

	return ok;
#endif
	return true;
}


/********************************************************************************************
>	MsgResult BmapPrevDlg::Message( Msg* Message)
	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/2/94
	Inputs:		Message: The message 
	Returns:	OK	 		  Message handled ok (return this even if you don't need to 
							  respond to the message). 

				FAIL 		  Something terrible happened whilst processing the message
				 			  eg. we ran out of memory. You must set ERROR if you
				 			  are returning this value. 

				EAT_MSG       The Message was handled ok but don't send it to any
				 			  more MessageHandlers. 

	Purpose:	This is the message handler for the BmapPrevDlg
********************************************************************************************/

MsgResult BmapPrevDlg::Message( Msg* Message)
{
	// handle the message
	if (!IS_OUR_DIALOG_MSG(Message))
		return OK;

	if (m_DialogWnd != NULL)
		m_DialogWnd->Enable( TRUE );
	
	DialogMsg* Msg = ((DialogMsg*) Message); 

	BOOL EndDialog = FALSE;		// TRUE if we should quit the dialog
//	BOOL CommitValues = FALSE; 	// TRUE if we should commit the dialog values

	// Catch mouse move messages for all tabs here so we can do all the bubble help / status
	// bar stuff for all the tabs (note: DIM_MOUSE_MOVE still goes to the tabs, this is just
	// a hook to process it a little first)
	if (Msg->DlgMsg == DIM_MOUSE_MOVE && m_LastCursorOverControlID != Msg->GadgetID)
	{
		m_LastCursorOverControlID = Msg->GadgetID;

		UpdateStatusBar(Msg->GadgetID);
		PrepareBubbleHelp();
	}

	if (Msg->DlgMsg == DIM_TIMER)
	{
		if (m_pBubbleTimer && m_pBubbleTimer->Elapsed(BUBBLE_TIME))
		{
			// Delete the timer as it is no longer needed
			delete m_pBubbleTimer;
			m_pBubbleTimer = 0;

			// Show the bubble help window
			DoBubbleHelp();
		}
	}

	// Determine from what page the message originated
	if( Msg->PageID == _R(IDD_TBITMAPSIZE) )
		HandleBitmapSizeTabMsg(Msg); 
PORTNOTE("other", "Remove palette usage" )
#if !defined(EXCLUDE_FROM_XARALX)
	else
	if( Msg->PageID == _R(IDD_TPALETTE) )
		HandlePaletteTabMsg(Msg);
#endif
	else
	if( Msg->PageID == _R(IDD_TIMAPOPTIONS) )
		HandleImageMapTabMsg(Msg); 
	else
	if( Msg->PageID == _R(IDD_TBROWSER) )
		HandleBrowserPreviewTabMsg(Msg); 
	else
	if( Msg->PageID == _R(IDD_TBITMAPOPTIONS) )
		HandleBitmapOptionsTabMsg(Msg);
	else
	if( Msg->PageID == 0 )
	{
		// A message generated from the tabbed dialog itself
		switch (Msg->DlgMsg)
		{
			case DIM_CREATE:
			{
				if (!OnCreate()) // initialise
				{
					ERROR3("Problem in OnCreate()");
					return FAIL;
				}
				// disable browser preview for now
//				BOOL Old = m_bDoPreviewInBrowser; // remember the value of the flag
//				m_bDoPreviewInBrowser = FALSE; // disable browser preview
//				DoPreview(); // do preview
//				m_bDoPreviewInBrowser = Old; // restore the setting

PORTNOTE("other", "Remove bubble help timer usage" );
#if !defined(EXCLUDE_FROM_XARALX)
			//  Set up the bubble-help timer.
				SetTimer(BUBBLE_HELP_TIMMER_ID, 100);
#endif
				break;
			}

			case DIM_COMMIT:		// Close dialog and insert the bitmap in the document
				EndDialog = TRUE;
PORTNOTE("other", "Remove bubble help timer usage" );
#if !defined(EXCLUDE_FROM_XARALX)
				KillTimer(BUBBLE_HELP_TIMMER_ID);
#endif
				BmapPrevDlg::m_bClickedOnExport = TRUE;
				ImageMapOnCommit();		// Save the image map values
				break;

			case DIM_SOFT_COMMIT:		// Update the preview
				EndDialog = FALSE;
				DoPreview();
				BmapPrevDlg::m_bClickedOnExport = FALSE;
				break; 

			case DIM_CANCEL:		// Close dialog and don't insert the bitmap in the document
				EndDialog = TRUE;
PORTNOTE("other", "Remove bubble help timer usage" );
#if !defined(EXCLUDE_FROM_XARALX)
				KillTimer(BUBBLE_HELP_TIMMER_ID);
#endif
				BmapPrevDlg::m_bClickedOnExport = FALSE;
				// we have cancelled so these options are not valid
				m_pExportOptions->MarkInvalid();
				break;

			case DIM_LFT_BN_CLICKED:
			case DIM_RGT_BN_CLICKED:
				// If they clicked on the help button
				if (Msg->GadgetID == wxID_HELP)
					OnHelpButtonClicked();
				break;

			default:
				break;
		}
	}
	else
		ERROR3("Message from unknown tab dialog page"); 

	if (EndDialog)	// Dialog communication over 
	{	
		// set the proper state in the export options to indicate which button was clicked
		if (Msg->DlgMsg == DIM_COMMIT)
		{
			//  Now, the ModifiedAfterLastExport variable used to tell us whether the options had
			//  been changed since the user last pressed the Preview button. This does not
			//  apply now since the preview is always up to date, so we will ignore it.
			if (m_FilterType == MAKE_BITMAP_FILTER)
			{
				// we need to get rid of any temp files
				if (m_pExportOptions->HasTempFile())
				{
					// get the path name
					PathName Path = m_pExportOptions->GetPathName();

					// if a valid path, delete it
					if (Path.IsValid())
						FileUtil::DeleteFile(&Path);

					m_pExportOptions->SetPathName(NULL);
				}
			}

			// set the extention as we may have changed file type
			// some extentions are in the form jpg,jpeg
			// just use the first example
			String_256 Ext = m_pExportOptions->FindBitmapFilterForTheseExportOptions()->pOILFilter->FilterExt;
			for (INT32 i = 0; i < 256 && Ext[i]; i++)
			{
				if (Ext[i] == ',')
				{
					Ext.Remove(i, Ext.Length()-i);
					break;
				}
			}
			BmapPrevDlg::m_pthExport.SetType ( Ext );

			m_pExportOptions->SetPutHTMLTagOnClipboard(m_ClipBoardUsage == CLIPBOARD_HTML);

			// we have committed these values so mark them as valid
			m_pExportOptions->MarkValid();
		} // End DIM_COMMIT.
		else 
		{
			// check if jpeg export dialog has been canceled 
			if (m_FilterType == JPEG)
			{
				// set our KernelBitmap pointer to NULL.
				JPEGExportOptions::SetKernelBitmap(NULL);
			}
		}

		// tell the preview dialog to shut down			
		if (m_DialogWnd != NULL)
			BROADCAST_TO_CLASS( DialogMsg(m_DialogWnd, Msg->DlgMsg, 0, 0, 0), DialogOp );
		m_DialogWnd = NULL;

		Close();
		End(); 	 	// End of dialog 
	}

	return DLG_EAT_IF_HUNGRY(Msg);   // I return EAT_MSG unless the message needs to be sent to all dialogs 
}

/********************************************************************************************
>	void BmapPrevDlg::OnHelpButtonClicked()
	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/7/97
	Purpose:	Called when the help button is clicked.
				Brings up a help page that depends on the filter type being used.
********************************************************************************************/
void BmapPrevDlg::OnHelpButtonClicked()
{
	//Fetch a help page relevant to the filter we're using to export
	switch (m_FilterType) 
	{
		case GIF:
		case TI_GIF:
			HelpUserTopic(_R(IDS_HELPPATH_Dialog_Bitmap_Export_GIF));
			break;
		
		case PNG:
			HelpUserTopic(_R(IDS_HELPPATH_Dialog_Bitmap_Export_PNG));
			break;

		case MAKE_BITMAP_FILTER:
			HelpUserTopic(_R(IDS_HELPPATH_Command_Create_Bitmap_Copy));
			break;
		
		case BMP_UNCOMPRESSED:
			HelpUserTopic(_R(IDS_HELPPATH_Dialog_Bitmap_Export_BMP));
			break;

		case JPEG:
			HelpUserTopic(_R(IDS_HELPPATH_Dialog_Bitmap_Export_JPEG));
			break;
	}
}

// //////////////////////////////////////////////////////////////////////////////////////// //
// //////////////////////////////////////////////////////////////////////////////////////// //
// Palette Options Tab																		//
// //////////////////////////////////////////////////////////////////////////////////////// //
// //////////////////////////////////////////////////////////////////////////////////////// //

/********************************************************************************************
>	void BmapPrevDlg::InitPaletteTab()
	Author:		Craig_Hamilton (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/11/00
	Purpose:	Calls methods to intialise the palette tab controls
********************************************************************************************/
void BmapPrevDlg::InitPaletteTab()
{
	// Initialise the dithering list.
	InitPaletteDitheringList();

	// Initialise the palette list.
	InitPalettePaletteList();

	// Initialise the colour depth list.
	InitPaletteColourDepthList();

	// Initialise the colour model list.
	InitPaletteColourModelList();

	// Initialise the button strip
	InitPaletteButtonStrip();

	// Initialise the palette sort list
	InitPaletteSortList();
}

/********************************************************************************************
>	void BmapPrevDlg::InitPaletteDitheringList()
	Author:		Jonathan_Payne (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/12/2000
	Purpose:	Sets up the palette dithering list on creation of the dialog
********************************************************************************************/
void BmapPrevDlg::InitPaletteDitheringList()
{
	// All of this is done in the RefreshPaletteDitherList function as
	// the dithering depends on the colour depth etc.
}

/********************************************************************************************
>	void BmapPrevDlg::InitPalettePaletteList()
	Author:		Jonathan_Payne (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/12/2000
	Purpose:	Sets up the palette palette list (browser/optimised/web snap) on creation of
				the dialog
********************************************************************************************/
void BmapPrevDlg::InitPalettePaletteList()
{
	// Make sure the list is empty
	DeleteAllValues( _R(IDC_PALETTE_COMBO) );

	//  Add the necessary strings to the list. 
	//  This is the complete collection of them.
	SetStringGadgetValue( _R(IDC_PALETTE_COMBO), _R(IDS_PALETTE_BROWSER),	FALSE, 0 );
	SetStringGadgetValue( _R(IDC_PALETTE_COMBO), _R(IDS_PALETTE_OPTIMISED),	FALSE, 1 );
	SetStringGadgetValue( _R(IDC_PALETTE_COMBO), _R(IDS_PALETTE_WEB_SNAP),	FALSE, 2 );

	SetComboListLength(_R(IDC_PALETTE_COMBO));
}

/********************************************************************************************
>	void BmapPrevDlg::InitPaletteColourDepthList()
	Author:		Jonathan_Payne (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/12/2000
	Purpose:	Sets up the palette colour depth list on creation of the dialog
********************************************************************************************/
void BmapPrevDlg::InitPaletteColourDepthList()
{
	// This case is handled in the refresh case
}

/********************************************************************************************
>	void BmapPrevDlg::InitPaletteColourModelList()
	Author:		Craig_Hamilton (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/11/00
	Purpose:	This initialies the colour model list. We use the member variable
				m_ColorModelType instead of having this data kept in the export options since
				it only has a bearing on how the colour information is represented to the user,
				not how the graphic is exported.
********************************************************************************************/
void BmapPrevDlg::InitPaletteColourModelList()
{
	//  Make sure that the list is empty.
	DeleteAllValues(_R(IDC_COLOR_MODEL_LIST));

	//  Add the necessary strings to the list. 
	//  This is the complete collection of them.
	SetStringGadgetValue( _R(IDC_COLOR_MODEL_LIST), _R(IDS_COLOR_DISPLAY_RGB_HEX),		FALSE, 0 );
	SetStringGadgetValue( _R(IDC_COLOR_MODEL_LIST), _R(IDS_COLOR_DISPLAY_RGB_NUMBER),	FALSE, 1 );
	SetStringGadgetValue( _R(IDC_COLOR_MODEL_LIST), _R(IDS_COLOR_DISPLAY_RGB_PERCENT),	FALSE, 2 );
	SetStringGadgetValue( _R(IDC_COLOR_MODEL_LIST), _R(IDS_COLOR_DISPLAY_HSV),			FALSE, 3 );

	SetComboListLength( _R(IDC_COLOR_MODEL_LIST) );
}

/********************************************************************************************
>	void BmapPrevDlg::InitPaletteButtonStrip()
	Author:		Jonathan_Payne (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/12/2000
	Purpose:	Sets the bitmaps for the buttons on the palette tab on creation of the dialog
********************************************************************************************/
void BmapPrevDlg::InitPaletteButtonStrip()
{
	SetGadgetBitmaps(_R(IDC_T2_LOCKED_COLOUR),			0, 0 );
	SetGadgetBitmaps(_R(IDC_T2_WEB_SAFE_COLOUR),		0, 0 );
	SetGadgetBitmaps(_R(IDC_T2_TRANSPARENT_COLOUR),		0, 0 );
	SetGadgetBitmaps(_R(IDC_T2_TRANSPARENT_BACKGROUND),	0, 0 );
	SetGadgetBitmaps(_R(IDC_T2_DELETE_COLOUR),			0, 0 );
	SetGadgetBitmaps(_R(IDC_T2_RESTORE_COLOUR),			0, 0 );
	SetGadgetBitmaps(_R(IDC_T2_SYSTEM_COLOURS),			0, 0 );
}

/******************************************************************************************
>	void BmapPrevDlg::InitSortList()
	Author:		Alex_Price (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/06/99
	Purpose:	Constructs and initialises the sort drop-list in the palette tab on
				creation of the bitmap export dialog.
******************************************************************************************/
void BmapPrevDlg::InitPaletteSortList()
{
	//  Make sure that the list is empty.
	DeleteAllValues( _R(IDC_T2_SORT_LIST) );

	//  Add the necessary strings to the list. 
	//  This is the complete collection of them.
	SetStringGadgetValue( _R(IDC_T2_SORT_LIST), _R(IDS_SORT_BY_USES),		FALSE, 0 );
	SetStringGadgetValue( _R(IDC_T2_SORT_LIST), _R(IDS_SORT_BY_LUMINANCE),	FALSE, 1 );
	SetStringGadgetValue( _R(IDC_T2_SORT_LIST), _R(IDS_SORT_BY_HUE),		FALSE, 2 );

	SetComboListLength( _R(IDC_T2_SORT_LIST) );
}

/********************************************************************************************
>	void BmapPrevDlg::HandlePaletteOptionsTabMsg(DialogMsg* Msg)
	Author:		Stefan_Stoykov (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/7/97
	Inputs:		Msg: The message sent from page 1 of the dialog
	Purpose:	All messages generated from the tabbed dialog's palette tab get processed here
********************************************************************************************/
PORTNOTE("other","BmapPrevDlg::HandlePaletteTabMsg - Windows message handler")
#ifndef EXCLUDE_FROM_XARALX
void BmapPrevDlg::HandlePaletteTabMsg(DialogMsg* Msg)
{
	CDlgResID PageID = GetCurrentPageID();	// Get currently selected Tab id
	TalkToPage(_R(IDD_TPALETTE));  // The Coords Page identifier
	switch (Msg->DlgMsg)
	{
		case DIM_CREATE:
		{
			InitPaletteTab();
			break;
		}
		case DIM_SELECTION_CHANGED:
		{
			if( Msg->GadgetID == _R(IDC_DITHERING_COMBO) )
				HandlePaletteDitheringListChange();
			else
			if( Msg->GadgetID == _R(IDC_PALETTE_COMBO) )
				HandlePalettePaletteListChange();
			else
			if( Msg->GadgetID == _R(IDC_COLOUR_DEPTH_COMBO) )
				HandlePaletteColourDepthListChange();
			else
			if( Msg->GadgetID == _R(IDC_T2_SORT_LIST) )
				HandlePaletteSortListChange();
			else
			if( Msg->GadgetID == _R(IDC_COLOR_MODEL_LIST) )
				HandlePaletteColourModelListChange();

			RefreshPaletteOptionsTab();	// Changes may have had an effect other controls
			break;
		}

		case DIM_TEXT_CHANGED:
			if (PageID != _R(IDD_TPALETTE) || m_LockSizeUpdates)
				break;

		case DIM_KILL_FOCUS:
		{
			if (Msg->GadgetID == _R(IDC_COLOURS_USED_EDIT))
			{
				HandlePaletteColoursUsedChange();
				RefreshPaletteLinkedControls();
			}
			else
			if (Msg->GadgetID == _R(IDC_RED_EDIT) || Msg->GadgetID == _R(IDC_GREEN_EDIT) || Msg->GadgetID == _R(IDC_BLUE_EDIT))
			{
				HandlePaletteColourEditChange(Msg->GadgetID);
				ReDrawInfoType info;
				GetKernelRenderedGadgetInfo(_R(IDC_T2_PALETTE_CONTROL), &info);
				m_PaletteControl.RedrawSelectedCell(&info);
			}
			TalkToPage(PageID);
			break;
		}

		case DIM_REDRAW:
		{
			if (Msg->GadgetID == _R(IDC_T2_PALETTE_CONTROL))
				m_PaletteControl.Render(reinterpret_cast<ReDrawInfoType *>(Msg->DlgMsgParam));
			TalkToPage(PageID);
			break;
		}

		case DIM_MOUSE_MOVE:
		{
			// Need to tell the palette about mouse movement both inside and outsite of the palette control
			// (so it knows when to remove the highlight thing)

			INT32 startColour	= m_PaletteControl.GetMouseOverColour();
			INT32 endColour	= BitmapExportPaletteControl::INVALID_COLOUR_VALUE;

			if (Msg->GadgetID == _R(IDC_T2_PALETTE_CONTROL))
			{
				m_MouseInsidePaletteControl = true;
				m_PaletteControl.MsgMouseMove(reinterpret_cast<ReDrawInfoType *>(Msg->DlgMsgParam));
				endColour = m_PaletteControl.GetMouseOverColour();
			}
			else if (m_MouseInsidePaletteControl == true)
			{
				// This is the first mouse move message since the mouse has left the palette
				m_MouseInsidePaletteControl = false;
				ReDrawInfoType info;
				GetKernelRenderedGadgetInfo(_R(IDC_T2_PALETTE_CONTROL), &info);
				m_PaletteControl.SetHighlightedCell(&info, BitmapExportPaletteControl::INVALID_COLOUR_VALUE);
			}

			if (startColour != endColour)
				RefreshPaletteColoursEdit();

			TalkToPage(PageID);
			break;
		}

		case DIM_LFT_BN_DOWN:
		{
			if (Msg->GadgetID == _R(IDC_T2_PALETTE_CONTROL))
			{
				if (m_PaletteControl.MsgMouseLeftButtonDown(reinterpret_cast<ReDrawInfoType *>(Msg->DlgMsgParam)))
				{
					// The message resulted in a change to the selection
					RefreshPaletteLinkedControls();
				}
			}
			break;
		}

		case DIM_LFT_BN_CLICKED:
		{
			switch(Msg->GadgetID)
			{
				case _R(IDC_T2_LOCKED_COLOUR):			HandlePaletteLockedColourButtonChange();			break;
				case _R(IDC_T2_WEB_SAFE_COLOUR):		HandlePaletteWebSafeColourButtonChange();			break;
				case _R(IDC_T2_TRANSPARENT_BACKGROUND):	HandlePaletteTransparentBackgroundButtonChange();	break;
				case _R(IDC_T2_TRANSPARENT_COLOUR):		HandlePaletteTransparentColourButtonChange();		break;
				// Gap in button bar
				case _R(IDC_T2_DELETE_COLOUR):			HandlePaletteDeleteColourButtonChnage();			break;
				case _R(IDC_T2_RESTORE_COLOUR):			HandlePaletteRestoreColourButtonChange();			break;
				// Gap in button bar
				case _R(IDC_T2_SYSTEM_COLOURS):			HandlePaletteSystemColoursButtonChnage();			break;
			}
			RefreshPaletteOptionsTab();	// Changes may have had an effect other controls
			break;
		}

		case DIM_RGT_BN_UP:
		{
		/*	
			I have disabled the palette right click menu as some of the manipulations it does on the palette are
			a bit iffy, for example toggling lockedness only does half the things it needes to (see the handler
			for the locking button).  -- Jonathan 22/12/2000


			if (Msg->GadgetID == _R(IDC_T2_PALETTE_CONTROL))
			{
				m_PaletteControl.MsgMouseRightButtonUp(reinterpret_cast<ReDrawInfoType *>(Msg->DlgMsgParam), this);
				RefreshPaletteLinkedControls();
			}
		*/
			break;
		}

		case DIM_SET_ACTIVE:
			UpdateCurrentTab();
			break;

		default:
			TalkToPage(PageID);
			break;
	};
}
#endif

/******************************************************************************************
>	void BmapPrevDlg::HandleDitherListChange()
	Author:		Alex_Price (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/06/99
	Purpose:	When the selection in the Dither drop list changes, this function is 
				called. It does all the necessary work in response to this change.
******************************************************************************************/
void BmapPrevDlg::HandlePaletteDitheringListChange()
{
	//  Get the user's selection
	String_16 DitherSelected = GetStringGadgetValue(_R(IDC_DITHERING_COMBO) );

	if( DitherSelected.IsIdentical(String_16(_R(IDS_DITHER_NONE))))
	{
		m_pExportOptions->SetDither(XARADITHER_NONE);
	}
	else if( DitherSelected.IsIdentical( String_16( _R(IDS_DITHER_ORDERED))))
	{
		m_pExportOptions->SetDither(XARADITHER_ORDERED);
	}
	else if( DitherSelected.IsIdentical( String_16( _R(IDS_DITHER_ERROR_DIFFUSED))))
	{
		m_pExportOptions->SetDither(XARADITHER_ERROR_DIFFUSION);
	}
	else
	{
		ERROR3("Impossible dither type selected");
	}

	// Invalidate the palette and therefor force a redraw of the the preview
	m_pExportOptions->InvalidatePalette();
}

/********************************************************************************************
>	void BmapPrevDlg::HandlePalettePaletteListChange()
	Author:		Jonathan_Payne (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/12/2000
	Purpose:	Responds to changes in the list of palette types.
********************************************************************************************/
void BmapPrevDlg::HandlePalettePaletteListChange()
{
	//  Get the user's selection
	String_16 PaletteSelected = GetStringGadgetValue(_R(IDC_PALETTE_COMBO));

	if(PaletteSelected.IsIdentical(String_16(_R(IDS_PALETTE_BROWSER))))
	{
		m_pExportOptions->SetToUseBrowserPalette(TRUE);
		m_pExportOptions->SetWebSnapPaletteAmount(0);
	}
	else
	{
		if(PaletteSelected.IsIdentical( String_16(_R(IDS_PALETTE_OPTIMISED))))
		{
			m_pExportOptions->SetToUseBrowserPalette(FALSE);
			m_pExportOptions->SetWebSnapPaletteAmount(0);
		}
		else if(PaletteSelected.IsIdentical( String_16(_R(IDS_PALETTE_WEB_SNAP))))
		{
			m_pExportOptions->SetToUseBrowserPalette(FALSE);
			m_pExportOptions->SetWebSnapPaletteAmount(12);
		}

		// Quick fix so ordered dithering is not used with palette types other
		// than browser
		if (m_pExportOptions->GetDither() == XARADITHER_ORDERED)
		{
			m_pExportOptions->SetDither(XARADITHER_ERROR_DIFFUSION);
		}
	}
}

/********************************************************************************************
>	void BmapPrevDlg::HandleColorDepthListChange()
	Author:		Jonathan_Payne (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/11/2000
	Purpose:	When the selection in the Color depth drop list changes, this function is 
				called. It does all the necessary work in response to this change.
********************************************************************************************/
void BmapPrevDlg::HandlePaletteColourDepthListChange()
{
	String_16 ColorDepthSelected = GetStringGadgetValue(_R(IDC_COLOUR_DEPTH_COMBO));

	INT32 newDepth;

	if		(ColorDepthSelected.IsIdentical(String_16(_R(IDS_COLOR_DEPTH_32))))
		newDepth = 32;
	else if	(ColorDepthSelected.IsIdentical(String_16(_R(IDS_COLOR_DEPTH_24))))
		newDepth = 24;
	else if	(ColorDepthSelected.IsIdentical(String_16(_R(IDS_COLOR_DEPTH_8))))
		newDepth = 8;
	else if	(ColorDepthSelected.IsIdentical(String_16(_R(IDS_COLOR_DEPTH_4))))
	{
		newDepth = 4;

		// Quick fix so ordered dithering is not used in 16 colour mode
		// as it does not work
		if (m_pExportOptions->GetDither() == XARADITHER_ORDERED)
		{
			m_pExportOptions->SetDither(XARADITHER_ERROR_DIFFUSION);
		}

	}
	else if	(ColorDepthSelected.IsIdentical(String_16(_R(IDS_COLOR_DEPTH_1))))
		newDepth = 1;
	else
	{
		ERROR3("Colour depth was not understood");
		return;	// Give up
	}

	m_pExportOptions->SetDepth(newDepth);
}

PORTNOTE("other","BmapPrevDlg::HandlePaletteLockedColourButtonChange - uses colour palette")
#ifndef EXCLUDE_FROM_XARALX
void BmapPrevDlg::HandlePaletteLockedColourButtonChange()
{
	if (m_PaletteControl.GetSelectedColour() != BitmapExportPaletteControl::INVALID_COLOUR_VALUE)
	{
		m_PaletteControl.SetSelectedColourToggleLocked();
//		m_pExportOptions->InvalidatePalette(); // unlocking a colour means this colour could become anything
//												// locking is safe but we dont check for that

		// set the preedited colour to match the edited colour
		SetPreEditedPaletteColour();
	}
	SetPreviewButtonState(true); // set the preview button
}
#endif

PORTNOTE("other","BmapPrevDlg::HandlePaletteWebSafeColourButtonChange - uses colour palette")
#ifndef EXCLUDE_FROM_XARALX
void BmapPrevDlg::HandlePaletteWebSafeColourButtonChange()
{
	if (m_PaletteControl.GetSelectedColour() != BitmapExportPaletteControl::INVALID_COLOUR_VALUE)
	{
		m_PaletteControl.SetSelectedColourWebSafe();
		m_pExportOptions->SetTempFileMatchesExportOptions(FALSE); // we have edited the palette
	}
	SetPreviewButtonState(true); // set the preview button
}
#endif

/********************************************************************************************
>	void BmapPrevDlg::HandleTransparentBackgroundButtonChange()
	Author:		Jonathan_Payne (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/11/2000
	Purpose:	Handles a change in the transparency button
********************************************************************************************/
void BmapPrevDlg::HandlePaletteTransparentBackgroundButtonChange()
{
	BOOL TransValue = GetLongGadgetValue(_R(IDC_T2_TRANSPARENT_BACKGROUND), 0, 1 );
	m_pExportOptions->SetBackgroundTransparency(TransValue);
	SetPreviewButtonState(true); // set the preview button
}

/********************************************************************************************
>	void BmapPrevDlg::HandlePaletteTransparentColourButtonChange()
	Author:		Jonathan_Payne (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/12/2000
	Purpose:	Makes the current selected colour in the palette control transparent.
********************************************************************************************/
PORTNOTE("other","BmapPrevDlg::HandlePaletteTransparentColourButtonChange - uses colour palette")
#ifndef EXCLUDE_FROM_XARALX
void BmapPrevDlg::HandlePaletteTransparentColourButtonChange()
{
	if (m_PaletteControl.GetSelectedColour() != BitmapExportPaletteControl::INVALID_COLOUR_VALUE)
	{
		// toggle the control for the colour
		if (m_PaletteControl.SetSelectedColourToggleTransparent())
		{
			// the colour toggled was the background colour so turn background transp off if required
			if (m_pExportOptions->IsBackgroundTransparent())
				m_pExportOptions->SetBackgroundTransparency(FALSE);
		}
		m_pExportOptions->SetTempFileMatchesExportOptions(FALSE); // we have edited the palette
		SetPreviewButtonState(true); // set the preview button
	}
}
#endif

/********************************************************************************************
>	void BmapPrevDlg::HandlePaletteDeleteColourButtonChnage()
	Author:		Jonathan_Payne (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/12/2000
	Purpose:	Deletes the currently selected colour in the palette control.
********************************************************************************************/
PORTNOTE("other","BmapPrevDlg::HandlePaletteDeleteColourButtonChnage - uses colour palette")
#ifndef EXCLUDE_FROM_XARALX
void BmapPrevDlg::HandlePaletteDeleteColourButtonChnage()
{
	if (m_PaletteControl.GetSelectedColour() != BitmapExportPaletteControl::INVALID_COLOUR_VALUE)
		m_PaletteControl.SetSelectedColourToggleDeleted();

	m_pExportOptions->SetTempFileMatchesExportOptions(FALSE);
//	m_pExportOptions->InvalidatePalette();
	SetPreviewButtonState(true); // set the preview button
}
#endif

/********************************************************************************************
>	void BmapPrevDlg::HandlePaletteRestoreColourButtonChange()
	Author:		Jonathan_Payne (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/12/2000
	Purpose:	Undelete the selected colour in the palette control (this function is the
				same as the delete function as the deleted state is a toggle).  This
				function relies on its button only being enabled when the colour can be
				restored.
********************************************************************************************/
PORTNOTE("other","BmapPrevDlg::HandlePaletteRestoreColourButtonChange - uses colour palette")
#ifndef EXCLUDE_FROM_XARALX
void BmapPrevDlg::HandlePaletteRestoreColourButtonChange()
{
	if (m_PaletteControl.GetSelectedColour() != BitmapExportPaletteControl::INVALID_COLOUR_VALUE)
		m_PaletteControl.SetSelectedColourToggleDeleted();

	m_pExportOptions->InvalidatePalette();
	SetPreviewButtonState(true); // set the preview button
}
#endif

/********************************************************************************************
>	void BmapPrevDlg::HandlePaletteSystemColoursButtonChnage()
	Author:		Jonathan_Payne (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/12/2000
	Purpose:	Responds to the user pressing the system colours button.
********************************************************************************************/
void BmapPrevDlg::HandlePaletteSystemColoursButtonChnage()
{
	m_pExportOptions->SetToUseSystemPalette(GetLongGadgetValue(_R(IDC_T2_SYSTEM_COLOURS), 0, 1));
	m_pExportOptions->InvalidatePalette();
	SetPreviewButtonState(true); // set the preview button
}

/********************************************************************************************
>	void BmapPrevDlg::HandlePaletteSortListChange()
	Author:		Jonathan_Payne (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/12/2000
	Purpose:	This function responds to the user changing the current sort method and tells
				the palette control about the change.
********************************************************************************************/
PORTNOTE("other","BmapPrevDlg::HandlePaletteSortListChange - uses colour palette")
#ifndef EXCLUDE_FROM_XARALX
void BmapPrevDlg::HandlePaletteSortListChange()
{
	//  Get the user's selection
	String_16 sort = GetStringGadgetValue(_R(IDC_T2_SORT_LIST));

	if(sort.IsIdentical(String_16(_R(IDS_SORT_BY_USES))))
	{
		m_PaletteControl.SetCurrentSortType(BitmapExportPaletteInterface::SORT_USE);
	}
	else if(sort.IsIdentical( String_16(_R(IDS_SORT_BY_LUMINANCE))))
	{
		m_PaletteControl.SetCurrentSortType(BitmapExportPaletteInterface::SORT_LUMINANCE);
	}
	else if(sort.IsIdentical( String_16(_R(IDS_SORT_BY_HUE))))
	{
		m_PaletteControl.SetCurrentSortType(BitmapExportPaletteInterface::SORT_HUE);
	}
	else
	{
		ERROR3("Unknown item selected");
	}

//	m_PaletteControl.RenderSoon();
}
#endif

/********************************************************************************************
>	void BmapPrevDlg::HandlePaletteColourModelListChange()
	Author:		Jonathan_Payne (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/12/2000
	Purpose:	Updates m_ColorModelType to take account of a newly selected colour model.
********************************************************************************************/
void BmapPrevDlg::HandlePaletteColourModelListChange()
{
	String_16 sort = GetStringGadgetValue(_R(IDC_COLOR_MODEL_LIST));

	if(sort.IsIdentical(String_16(_R(IDS_COLOR_DISPLAY_RGB_HEX))))
		m_ColorModelType = MODEL_RGBHEX;
	else if(sort.IsIdentical( String_16(_R(IDS_COLOR_DISPLAY_RGB_NUMBER))))
		m_ColorModelType = MODEL_RGBNUMBER;
	else if(sort.IsIdentical( String_16(_R(IDS_COLOR_DISPLAY_RGB_PERCENT))))
		m_ColorModelType = MODEL_RGBPERCENT;
	else if(sort.IsIdentical( String_16(_R(IDS_COLOR_DISPLAY_HSV))))
		m_ColorModelType = MODEL_HSV;
	else
		ERROR3("Unknown item selected");
}

void BmapPrevDlg::HandlePaletteColoursUsedChange()
{
	BOOL valid = FALSE;
	INT32 colours = GetLongGadgetValue(_R(IDC_COLOURS_USED_EDIT), 2, 256, 0, &valid);
	if (valid)
		m_pExportOptions->SetNumberOfUserRequestedColours(colours);

	SetPreviewButtonState(true); // set the preview button
}

PORTNOTE("other","BmapPrevDlg::HandlePaletteColourEditChange - uses colour palette")
#ifndef EXCLUDE_FROM_XARALX
void BmapPrevDlg::HandlePaletteColourEditChange(CGadgetID id)
{
	BOOL valid;

	String_256 str = GetStringGadgetValue(id, &valid);

	if (!valid)
		return;	// don't change the value so when the control is refreshed it is set
				// back to the old value

	ExtendedPalette	*palette = BmapPrevDlg::m_pExportOptions->GetExtendedPalette();
	ERROR3IF(!palette, "There is no palette - This should never happen");

	BYTE *pR, *pG, *pB;
	pR = &(palette->Data[m_PaletteControl.GetSelectedColour()].Red);
	pG = &(palette->Data[m_PaletteControl.GetSelectedColour()].Green);
	pB = &(palette->Data[m_PaletteControl.GetSelectedColour()].Blue);

	BYTE startR = *pR;
	BYTE startG = *pG;
	BYTE startB = *pB;

	BYTE *pColourWeAreChanging;	// not used in HSV mode

	switch (id)
	{
		case _R(IDC_RED_EDIT):		pColourWeAreChanging = pR;			break;
		case _R(IDC_GREEN_EDIT):	pColourWeAreChanging = pG;			break;
		case _R(IDC_BLUE_EDIT):		pColourWeAreChanging = pB;			break;
	}
	
	switch (m_ColorModelType)
	{
		case MODEL_RGBHEX:
		{
			INT32 t;
			sscanf(str, "%x", &t);
			if (t >= 0 && t < 256)
				*pColourWeAreChanging = t;
			break;
		}

		case MODEL_RGBPERCENT:
		{
			float t;
			sscanf(str, "%f", &t);
			double tt = t * (255.0 / 100.0); // convert from percent to 0-255
			if (tt >= 0 && tt < 256)
				*pColourWeAreChanging = tt + 0.5; // adding 0.5 so number rounds to nearest INT32
			break;
		}
		case MODEL_RGBNUMBER:
		{
			INT32 t;
			sscanf(str, "%u", &t);
			if (t >= 0 && t < 256)
				*pColourWeAreChanging = t;
			break;
		}

		case MODEL_HSV:
		{
			INT32 newValue;							// New value of colour component

			if (id == _R(IDC_RED_EDIT))					// H component
			{
				INT32 t;								// temp INT32 varaible
				sscanf(str, "%u", &t);				// read as an in
				newValue = t;						// value to be set (check range before setting)
			}
			else									// S & V components
			{
				float t;							// temp float variable (using float to keep sscanf happy)
				sscanf(str, "%f", &t);				// read value as a float
				t *= (float)(255.0 / 100.0);		// convert from percent to 0-255
				newValue = t + 0.5;					// value to be set (check range before setting)
			}


			if (newValue >= 0 && ((id == _R(IDC_RED_EDIT) && newValue < 360) || (id != _R(IDC_RED_EDIT) && newValue < 256)))
			{
				INT32 h, s, v;
				DocColour colour(*pR, *pG, *pB);
				colour.GetHSVValue(&h, &s, &v);
				switch (id)
				{
					case _R(IDC_RED_EDIT):		colour.SetHSVValue(newValue, s, v);		break;
					case _R(IDC_GREEN_EDIT):	colour.SetHSVValue(h, newValue, v);		break;
					case _R(IDC_BLUE_EDIT):		colour.SetHSVValue(h, s, newValue);		break;
				}
				INT32 newR, newG, newB;
				colour.GetRGBValue(&newR, &newG, &newB);
				*pR = newR; *pG = newG; *pB = newB;
			}
			break;
		}

		default:
		{
			ERROR3("You've broken it");
			break;
		}
	} // end of switch (m_ColorModelType)

	// Lock the colour it the user has managed to change it
	if ((startR != *pR || startG != *pG || startB != *pB))
	{
		// set the colour that it was before in the pre edit fields
		SetPreEditedPaletteColour(startR, startG, startB);
		// lock the colour
		if (!m_PaletteControl.GetSelectedColourLocked())
			m_PaletteControl.SetSelectedColourToggleLocked();

		// we have edited the palette so the options now longer match the image
		m_pExportOptions->SetTempFileMatchesExportOptions(FALSE);
		SetPreviewButtonState(true); // set the preview button
	}
}
#endif

/********************************************************************************************
>	void BmapPrevDlg::RefreshPaletteOptionsTab()
	Author:		Jonathan_Payne (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/12/2000
	Purpose:	This function refreshes the status of all the windows in the palette tab to
				take account of any possible changes.
********************************************************************************************/
void BmapPrevDlg::RefreshPaletteOptionsTab()
{
PORTNOTETRACE("other","BmapPrevDlg::RefreshPaletteOptionsTab - Do nothing");
#if !defined(EXCLUDE_FROM_XARALX)
//	if (m_pExportOptions->GetSupportsPalette())
//	{
		if (!m_pExportOptions->IsPaletteValid())
			DoPreview();
		else
			m_PaletteControl.RenderSoon();
//	}


	RefreshPaletteDitheringList();
	RefreshPalettePaletteList();
	RefreshPaletteColourDepthList();
	RefreshPaletteSortList();
	RefreshPaletteLinkedControls();

	SetPreviewButtonState(true);
#endif
}

/********************************************************************************************
>	void BmapPrevDlg::RefreshPaletteLinkedControls()
	Author:		Jonathan_Payne (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/12/2000
	Purpose:	This function refreshes controls on the palette dialog that are in some way
				related to operations on the palette.  This function is used when the user
				changes the selected colour or does anything else that might invalidate these
				controls.
********************************************************************************************/
void BmapPrevDlg::RefreshPaletteLinkedControls()
{
PORTNOTETRACE("other","BmapPrevDlg::RefreshPaletteLinkedControls - Do nothing");
#if !defined(EXCLUDE_FROM_XARALX)
	RefreshPaletteColourModelList();
	RefreshPaletteButtonStrip();
	RefreshPaletteColoursUsedEdit();
	RefreshPaletteColoursEdit();
#endif
}

/********************************************************************************************
>	void BmapPrevDlg::RefreshPaletteDitheringList()
	Author:		Jonathan_Payne (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/12/2000
	Purpose:	This function updates the dithering list control on the palette tab.  It has
				to add all the strings to the control again as some changes (for example
				changing the palette type) require different lists of dithering types to be
				shown.
********************************************************************************************/
void BmapPrevDlg::RefreshPaletteDitheringList()
{
	// Make sure the list is empty
	DeleteAllValues(_R(IDC_DITHERING_COMBO));

	// Set the first string
	SetStringGadgetValue( _R(IDC_DITHERING_COMBO), _R(IDS_DITHER_NONE), FALSE);

	// Find out what types of dithering are currently supported
	DWORD dithers = m_pExportOptions->GetSupportedDithers();

	// If there are no dither types supported, stop now
	if (!dithers)
	{
		SetSelectedDropListItem(_R(IDC_DITHERING_COMBO), _R(IDS_DITHER_NONE));
	}
	else
	{
		//  Add the necessary strings to the list
		if (dithers & BitmapExportOptions::ORDERED_DITHER)
			SetStringGadgetValue( _R(IDC_DITHERING_COMBO), _R(IDS_DITHER_ORDERED), FALSE);

		if (dithers & BitmapExportOptions::ERROR_DITHER)
			SetStringGadgetValue( _R(IDC_DITHERING_COMBO), _R(IDS_DITHER_ERROR_DIFFUSED), FALSE);

		switch (m_pExportOptions->GetDither())
		{
			case XARADITHER_ORDERED:
			case XARADITHER_ORDERED_GREY:
				SetSelectedDropListItem(_R(IDC_DITHERING_COMBO), _R(IDS_DITHER_ORDERED));
				break;

			case XARADITHER_ERROR_DIFFUSION:
				SetSelectedDropListItem(_R(IDC_DITHERING_COMBO), _R(IDS_DITHER_ERROR_DIFFUSED));
				break;

			case XARADITHER_NONE:
				SetSelectedDropListItem(_R(IDC_DITHERING_COMBO), _R(IDS_DITHER_NONE));
				break;

			default:
			case XARADITHER_SIMPLE:
				ERROR3("Unknown dithering type used");
				break;
		}
	}

	// Enable the control if there are any dithering options
	EnableGadget(_R(IDC_DITHERING_COMBO), dithers != 0);

	// Make the drop list the correct length
	SetComboListLength( _R(IDC_DITHERING_COMBO) );
}

void BmapPrevDlg::RefreshPalettePaletteList()
{
	if (!m_pExportOptions->GetSupportsPalette())
	{
		EnableGadget(_R(IDC_PALETTE_COMBO), FALSE);
		return;
	}
	else
	{
		EnableGadget(_R(IDC_PALETTE_COMBO), TRUE);
	}

	// Set the selected item.
	if(m_pExportOptions->IsUsingBrowserPalette() == TRUE)
		SetSelectedDropListItem(_R(IDC_PALETTE_COMBO), _R(IDS_PALETTE_BROWSER));
	else if (m_pExportOptions->GetWebSnapPaletteAmount() > 0)
		SetSelectedDropListItem(_R(IDC_PALETTE_COMBO), _R(IDS_PALETTE_WEB_SNAP));
	else
		SetSelectedDropListItem(_R(IDC_PALETTE_COMBO), _R(IDS_PALETTE_OPTIMISED));
}

void BmapPrevDlg::RefreshPaletteColourDepthList()
{
	// Make sure the list is empty
	DeleteAllValues( _R(IDC_COLOUR_DEPTH_COMBO) );

	INT32 nColourDepths = 0;

	DWORD colourDepths = m_pExportOptions->GetSupportedColourDepths();

	if (colourDepths & BitmapExportOptions::COLOUR_32BIT)
		SetStringGadgetValue(_R(IDC_COLOUR_DEPTH_COMBO), _R(IDS_COLOR_DEPTH_32), FALSE, nColourDepths++);
	if (colourDepths & BitmapExportOptions::COLOUR_24BIT)
		SetStringGadgetValue(_R(IDC_COLOUR_DEPTH_COMBO), _R(IDS_COLOR_DEPTH_24), FALSE, nColourDepths++);
//	if (colourDepths & BitmapExportOptions::COLOUR_16BIT)
//		SetStringGadgetValue(_R(IDC_COLOUR_DEPTH_COMBO), _R(IDS_COLOR_DEPTH_16), FALSE, nColourDepths++);
	if (colourDepths & BitmapExportOptions::COLOUR_256)
		SetStringGadgetValue(_R(IDC_COLOUR_DEPTH_COMBO), _R(IDS_COLOR_DEPTH_8), FALSE, nColourDepths++);
	if (colourDepths & BitmapExportOptions::COLOUR_16)
		SetStringGadgetValue(_R(IDC_COLOUR_DEPTH_COMBO), _R(IDS_COLOR_DEPTH_4), FALSE, nColourDepths++);
	if (colourDepths & BitmapExportOptions::COLOUR_2)
		SetStringGadgetValue(_R(IDC_COLOUR_DEPTH_COMBO), _R(IDS_COLOR_DEPTH_1), FALSE, nColourDepths++);

	SetComboListLength(_R(IDC_COLOUR_DEPTH_COMBO));

	switch( m_pExportOptions->GetDepth() )
	{
		case 1:		SetSelectedDropListItem( _R(IDC_COLOUR_DEPTH_COMBO), _R(IDS_COLOR_DEPTH_1) );	break;
		case 4:		SetSelectedDropListItem( _R(IDC_COLOUR_DEPTH_COMBO), _R(IDS_COLOR_DEPTH_4) );	break;
		case 8:		SetSelectedDropListItem( _R(IDC_COLOUR_DEPTH_COMBO), _R(IDS_COLOR_DEPTH_8) );	break;
		case 24:	SetSelectedDropListItem( _R(IDC_COLOUR_DEPTH_COMBO), _R(IDS_COLOR_DEPTH_24) );	break;
		case 32:	SetSelectedDropListItem( _R(IDC_COLOUR_DEPTH_COMBO), _R(IDS_COLOR_DEPTH_32) );	break;
		default:	SetSelectedDropListItem( _R(IDC_COLOUR_DEPTH_COMBO), _R(IDS_COLOR_DEPTH_8) );	break;
	}

	if (nColourDepths > 1)
		EnableGadget(_R(IDC_COLOUR_DEPTH_COMBO), TRUE);
}

PORTNOTE("other","BmapPrevDlg::RefreshPaletteColoursUsedEdit - uses colour palette")
#ifndef EXCLUDE_FROM_XARALX
void BmapPrevDlg::RefreshPaletteColoursUsedEdit()
{
	BOOL active = m_pExportOptions->GetSupportsPalette();

	EnableGadget(_R(IDC_COLOURS_USED_EDIT), active);

	// the m_LockSizeUpdates is moonlighting as a lock for palette text controls too
	m_LockSizeUpdates = TRUE;

	if (active)
	{
		ExtendedPalette	*palette = BmapPrevDlg::m_pExportOptions->GetExtendedPalette();
		ERROR3IF(!palette, "There is no palette - This should never happen");

		if (m_pExportOptions->DoesTempFileMatchExportOptions())
			// if we are uptodate go with the number of colours in the palette
			SetLongGadgetValue(_R(IDC_COLOURS_USED_EDIT), palette->NumberOfColours);
		else
			// only set the value here if it doesn't has focus
			if (::GetFocus() != GetDlgItem(GetReadWriteWindowID(), (INT32)_R(IDC_COLOURS_USED_EDIT)))
				SetLongGadgetValue(_R(IDC_COLOURS_USED_EDIT), m_pExportOptions->GetNumberOfUserRequestedColours());
	}
	else
		SetStringGadgetValue(_R(IDC_COLOURS_USED_EDIT), &String_16(_T("")));

	m_LockSizeUpdates = FALSE;
}
#endif

/********************************************************************************************
>	void BmapPrevDlg::RefreshPaletteColourModelList()
	Author:		Craig_Hamilton (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/11/2000
	Purpose:	This sets the colour model list. We use the member variable m_ColorModelType
				instead of having this data kept in the export options since it only has a
				bearing on how the colour information is represented to the user, not how the
				graphic is exported.
********************************************************************************************/
PORTNOTE("other","BmapPrevDlg::RefreshPaletteColourModelList - uses colour palette")
#ifndef EXCLUDE_FROM_XARALX
void BmapPrevDlg::RefreshPaletteColourModelList()
{
	switch(m_ColorModelType)
	{
		case MODEL_RGBHEX:		SetSelectedDropListItem(_R(IDC_COLOR_MODEL_LIST), _R(IDS_COLOR_DISPLAY_RGB_HEX) );		break;
		case MODEL_RGBNUMBER:	SetSelectedDropListItem(_R(IDC_COLOR_MODEL_LIST), _R(IDS_COLOR_DISPLAY_RGB_NUMBER));	break;
		case MODEL_RGBPERCENT:	SetSelectedDropListItem(_R(IDC_COLOR_MODEL_LIST), _R(IDS_COLOR_DISPLAY_RGB_PERCENT));	break;
		case MODEL_HSV:			SetSelectedDropListItem(_R(IDC_COLOR_MODEL_LIST), _R(IDS_COLOR_DISPLAY_HSV));			break;
	}

	EnableGadget(_R(IDC_COLOR_MODEL_LIST), m_pExportOptions->GetSupportsPalette());
}
#endif

/********************************************************************************************
>	void BmapPrevDlg::RefreshPaletteColoursEdit()
	Author:		Jonathan_Payne (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/12/2000
	Purpose:	Set the values of the three colour edit boxes based on the currently selected
				palette entry and on the currently selected colour model.
********************************************************************************************/
PORTNOTE("other","BmapPrevDlg::RefreshPaletteColoursEdit - uses colour palette")
#ifndef EXCLUDE_FROM_XARALX
void BmapPrevDlg::RefreshPaletteColoursEdit()
{
	BOOL bPalette		= m_pExportOptions->GetSupportsPalette();
	BOOL bSelection		= m_PaletteControl.GetSelectedColour()	!= BitmapExportPaletteControl::INVALID_COLOUR_VALUE;
	BOOL bHighlight		= m_PaletteControl.GetMouseOverColour()	!= BitmapExportPaletteControl::INVALID_COLOUR_VALUE;

	// the m_LockSizeUpdates is moonlighting as a lock for palette text controls too
	m_LockSizeUpdates = TRUE;

	EnableGadget(_R(IDC_RED_EDIT),		bPalette && bSelection && !m_bIsCreateBitmap && !m_bSlicingImage);
	EnableGadget(_R(IDC_GREEN_EDIT),	bPalette && bSelection && !m_bIsCreateBitmap && !m_bSlicingImage);
	EnableGadget(_R(IDC_BLUE_EDIT),		bPalette && bSelection && !m_bIsCreateBitmap && !m_bSlicingImage);

	if (bPalette && (bSelection || bHighlight))
	{
		ExtendedPalette	*palette = BmapPrevDlg::m_pExportOptions->GetExtendedPalette();
		ERROR3IF(!palette, "There is no palette - This should never happen");

		BYTE r, g, b;
		if (bHighlight)
		{
			r = palette->Data[m_PaletteControl.GetMouseOverColour()].Red;
			g = palette->Data[m_PaletteControl.GetMouseOverColour()].Green;
			b = palette->Data[m_PaletteControl.GetMouseOverColour()].Blue;
		}
		else
		{
			r = palette->Data[m_PaletteControl.GetSelectedColour()].Red;
			g = palette->Data[m_PaletteControl.GetSelectedColour()].Green;
			b = palette->Data[m_PaletteControl.GetSelectedColour()].Blue;
		}

		switch (m_ColorModelType)
		{
			case MODEL_RGBHEX:
			{
				TCHAR str[16];	// Temp string buffer (of abitary size)
				sprintf(str, "%02x", r);	SetStringGadgetValue(_R(IDC_RED_EDIT),		&String_16(str));
				sprintf(str, "%02x", g);	SetStringGadgetValue(_R(IDC_GREEN_EDIT),	&String_16(str));
				sprintf(str, "%02x", b);	SetStringGadgetValue(_R(IDC_BLUE_EDIT),		&String_16(str));
				break;
			}
			case MODEL_RGBPERCENT:
			{
				double p = 100.0 / 255.0;
				SetDoubleGadgetValue(_R(IDC_RED_EDIT),		r * p);
				SetDoubleGadgetValue(_R(IDC_GREEN_EDIT),	g * p);
				SetDoubleGadgetValue(_R(IDC_BLUE_EDIT),		b * p);
				break;
			}
			case MODEL_RGBNUMBER:
			{
				SetLongGadgetValue(_R(IDC_RED_EDIT),		r);
				SetLongGadgetValue(_R(IDC_GREEN_EDIT),		g);
				SetLongGadgetValue(_R(IDC_BLUE_EDIT),		b);
				break;
			}
			case MODEL_HSV:
			{
				INT32 h, s, v;
				DocColour colour(r, g, b);
				colour.GetHSVValue(&h, &s, &v);
				SetLongGadgetValue(_R(IDC_RED_EDIT),		h);				// H
				double p = 100.0 / 255.0;
				SetDoubleGadgetValue(_R(IDC_GREEN_EDIT),	s * p);			// S (as percentage)
				SetDoubleGadgetValue(_R(IDC_BLUE_EDIT),		v * p);			// V (as percentage)
				break;
			}
		}
	}
	else
	{
		SetStringGadgetValue(_R(IDC_RED_EDIT),		&String_16(""));
		SetStringGadgetValue(_R(IDC_GREEN_EDIT),	&String_16(""));
		SetStringGadgetValue(_R(IDC_BLUE_EDIT),		&String_16(""));
	}

	if(m_ColorModelType == MODEL_HSV)
	{
		//  Display 'H', 'S', and 'V' beside the colour edit boxes
		SetStringGadgetValue(_R(IDC_T2STATIC7), _R(IDS_H_LABEL));
		SetStringGadgetValue(_R(IDC_T2STATIC8), _R(IDS_S_LABEL));
		SetStringGadgetValue(_R(IDC_T2STATIC9), _R(IDS_V_LABEL));
	}
	else
	{
		//  Display 'R', 'G', and 'B' beside the colour edit boxes
		SetStringGadgetValue(_R(IDC_T2STATIC7), _R(IDS_R_LABEL));
		SetStringGadgetValue(_R(IDC_T2STATIC8), _R(IDS_G_LABEL));
		SetStringGadgetValue(_R(IDC_T2STATIC9), _R(IDS_B_LABEL));
	}

	m_LockSizeUpdates = FALSE;
}
#endif

PORTNOTE("other","BmapPrevDlg::RefreshPaletteButtonStrip - uses colour palette")
#ifndef EXCLUDE_FROM_XARALX
void BmapPrevDlg::RefreshPaletteButtonStrip()
{
	if (m_PaletteControl.GetSelectedColour() == BitmapExportPaletteControl::INVALID_COLOUR_VALUE
		|| m_bSlicingImage)
	{
		EnableGadget(_R(IDC_T2_LOCKED_COLOUR),				FALSE);
		EnableGadget(_R(IDC_T2_WEB_SAFE_COLOUR),			FALSE);
		EnableGadget(_R(IDC_T2_TRANSPARENT_COLOUR),			FALSE);
		EnableGadget(_R(IDC_T2_DELETE_COLOUR),				FALSE);
		EnableGadget(_R(IDC_T2_RESTORE_COLOUR),				FALSE);
	}
	else
	{
		// Locked colour button
		EnableGadget(_R(IDC_T2_LOCKED_COLOUR),				m_pExportOptions->GetSupportsPalette());
		SetLongGadgetValue(_R(IDC_T2_LOCKED_COLOUR),		m_PaletteControl.GetSelectedColourLocked());

		// Web safe button
		EnableGadget(_R(IDC_T2_WEB_SAFE_COLOUR),			m_pExportOptions->GetSupportsPalette());
		SetLongGadgetValue(_R(IDC_T2_WEB_SAFE_COLOUR),		FALSE);

		// Make colour transparent button
		EnableGadget(_R(IDC_T2_TRANSPARENT_COLOUR),			m_pExportOptions->GetSupportsPalette()
															&& m_pExportOptions->GetSupportsTransparency());
		SetLongGadgetValue(_R(IDC_T2_TRANSPARENT_COLOUR),	m_PaletteControl.GetSelectedColourTransparent());

		// Delete colour button
		EnableGadget(_R(IDC_T2_DELETE_COLOUR),				m_pExportOptions->GetSupportsPalette()
															&& !m_PaletteControl.GetSelectedColourDeleted());
		SetLongGadgetValue(_R(IDC_T2_DELETE_COLOUR),		FALSE);

		// Restore colour button
		EnableGadget(_R(IDC_T2_RESTORE_COLOUR),				m_pExportOptions->GetSupportsPalette()
															&& m_PaletteControl.GetSelectedColourDeleted());
		SetLongGadgetValue(_R(IDC_T2_RESTORE_COLOUR),		FALSE);
	}

	// Make background transparent button
	EnableGadget(_R(IDC_T2_TRANSPARENT_BACKGROUND),			m_pExportOptions->GetSupportsTransparency());
	SetLongGadgetValue(_R(IDC_T2_TRANSPARENT_BACKGROUND),	m_pExportOptions->IsBackgroundTransparent());

	// System colours button
	EnableGadget(_R(IDC_T2_SYSTEM_COLOURS),					m_pExportOptions->GetSupportsPalette());
	SetLongGadgetValue(_R(IDC_T2_SYSTEM_COLOURS),			m_pExportOptions->IsUsingSystemPalette());
}
#endif

PORTNOTE("other","BmapPrevDlg::RefreshPaletteSortList - uses colour palette")
#ifndef EXCLUDE_FROM_XARALX
void BmapPrevDlg::RefreshPaletteSortList()
{
	if (!m_pExportOptions->GetSupportsPalette())
	{
		EnableGadget(_R(IDC_T2_SORT_LIST), FALSE);
		return;
	}
	else
	{
		EnableGadget(_R(IDC_T2_SORT_LIST), TRUE);
	}
	
	switch (m_PaletteControl.GetCurrentSortType())
	{
		case BitmapExportPaletteInterface::SORT_USE:
			SetSelectedDropListItem(_R(IDC_T2_SORT_LIST), _R(IDS_SORT_BY_USES));
			break;

		case BitmapExportPaletteInterface::SORT_LUMINANCE:
			SetSelectedDropListItem(_R(IDC_T2_SORT_LIST), _R(IDS_SORT_BY_LUMINANCE));
			break;

		case BitmapExportPaletteInterface::SORT_HUE:
			SetSelectedDropListItem(_R(IDC_T2_SORT_LIST), _R(IDS_SORT_BY_HUE));
			break;

		default:
			ERROR3("How did this happen?");
			break;
	}
}
#endif

// //////////////////////////////////////////////////////////////////////////////////////// //
// //////////////////////////////////////////////////////////////////////////////////////// //
// Bitmap Size Tab																			//
// //////////////////////////////////////////////////////////////////////////////////////// //
// //////////////////////////////////////////////////////////////////////////////////////// //

/********************************************************************************************

>	void BmapPrevDlg::HandleBitmapSizeTabMsg(DialogMsg* Msg)

	Author:		Stefan_Stoykov (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/7/97
	Inputs:		Msg: The message sent from page 1 of the dialog
	Outputs:	-
	Returns:	-
	Purpose:	All messages generated from the tabbed dialog's page 1 get processed here
	Errors:		-
	Scope:		private
	SeeAlso:	BmapPrevDlg::Message

********************************************************************************************/
void BmapPrevDlg::HandleBitmapSizeTabMsg(DialogMsg* Msg)
{
	CDlgResID PageID = GetCurrentPageID();	// Get currently selected Tab id
	TalkToPage(_R(IDD_TBITMAPSIZE)); 

	switch (Msg->DlgMsg)
	{
		case DIM_CREATE:
		{
			InitBitmapSizeTab();
			break;
		}

		case DIM_TEXT_CHANGED:
		{
			if( Msg->GadgetID == _R(IDC_T1EDIT1) )
				HandleBitmapSizeWidthChange();
			else
			if( Msg->GadgetID == _R(IDC_T1EDIT2) )
				HandleBitmapSizeHeightChange();
			else
			if( Msg->GadgetID == _R(IDC_T1EDIT3) )
				HandleBitmapSizeScaleFactorChange();
			else
			if( Msg->GadgetID == _R(IDC_T1COMBO1) )	
					if (PageID == _R(IDD_TBITMAPSIZE) && !m_LockSizeUpdates)
						HandleBitmapSizeDPIChange();
			break;
		}

		case DIM_SELECTION_CHANGED:
		{
			if( Msg->GadgetID == _R(IDC_T1COMBO1) )
				HandleBitmapSizeDPIChange();
			break;
		}

		case DIM_LFT_BN_CLICKED:
		{
PORTNOTE("other","Hopefully this is no longer needed")
#ifndef EXCLUDE_FROM_XARALX
			// This is a nasty hack/bodge
			// I keep on getting sent this DIM_LFT_BN_CLICKED msg when the user
			// changes views - or shortly after. The user didn't click on
			// any of the radio buttons bellow despite the msg saying we did.
			// So I check that the cursor is over the control that the user
			// has just clicked on before passing the msg to be processed.
			// sjk 2/1/01
			wxRect r;
			wxRect wndpos;
			GetGadgetPosition(Msg->GadgetID, &r);
			GetWindowRect(GetReadWriteWindowID(), &wndpos);
			POINT CurPos;
			GetCursorPos(&CurPos);

			BOOL inctrl = FALSE;

			if (CurPos.x >= wndpos.GetLeft() + r.GetLeft() && 
				CurPos.x <= wndpos.GetLeft() + r.GetRight() &&
				CurPos.y >= wndpos.GetTop() + r.GetTop() && 
				CurPos.y <= wndpos.GetTop() + r.GetBottom())
				inctrl = TRUE;

			if (inctrl)
			{
#endif
				if( Msg->GadgetID == _R(IDC_T1RADIO2) )
					HandleBitmapSizeAreaToSaveChange(DRAWING);
				else
				if( Msg->GadgetID == _R(IDC_T1RADIO3) )
					HandleBitmapSizeAreaToSaveChange(SELECTION);
				else
				if( Msg->GadgetID == _R(IDC_T1RADIO1) )
					HandleBitmapSizeAreaToSaveChange(SPREAD);
				else
				if( Msg->GadgetID == _R(IDC_T1RADIO_MINIMISE_AA) )
					HandleBitmapSizeAntiAliasingChange(TRUE);
				else
				if( Msg->GadgetID == _R(IDC_T1RADIO_MAINTAIN_AA) )
					HandleBitmapSizeAntiAliasingChange(FALSE);
				else
				if( Msg->GadgetID == _R(IDC_T1_PUTHTMLTAGONCLIPBOARD) )
					HandleBitmapSizePutHTMLChange();
//			}
			break;
		}

		case DIM_FOCUS_LOST:
		{
			if( Msg->GadgetID == _R(IDC_T1EDIT1) ||
				Msg->GadgetID == _R(IDC_T1EDIT2) ||
				Msg->GadgetID == _R(IDC_T1COMBO1) )
			{
				HandleBitmapSizeDPIChange();
			}
			TalkToPage(PageID);			
			break;
		}

		case DIM_SET_ACTIVE:
			UpdateCurrentTab();
			break;

		default:
			TalkToPage(PageID);
			break;

	}
}

void BmapPrevDlg::InitBitmapSizeTab()
{
	InitBitmapSizeDPIList();

	InitBitmapSizeSelectionRadioGroup();

	InitBitmapSizeAntiAlisingRadioGroup();

	InitBitmapSizePutHTMLTick();
}

void BmapPrevDlg::InitBitmapSizePutHTMLTick()
{
	if (m_FilterType == MAKE_BITMAP_FILTER)
	{
		HideGadget(_R(IDC_T1_PUTHTMLTAGONCLIPBOARD), TRUE);
		HideGadget(_R(IDC_STATIC_HTML), TRUE);
	}
	else
	{
		SetLongGadgetValue(_R(IDC_T1_PUTHTMLTAGONCLIPBOARD), m_ClipBoardUsage == CLIPBOARD_HTML);
	}
}

void BmapPrevDlg::InitBitmapSizeDPIList()
{
	//  If we are slicing an image, then the controls below are all
	//  disabled. Therefore, just return, since we can't do anything else
	if (m_bSlicingImage)
		return;

	m_LockSizeUpdates = TRUE;

	// Make sure the list is empty
	DeleteAllValues(_R(IDC_T1COMBO1));

	bool bHasDPI =	m_pExportOptions->GetFilterNameStrID() != _R(IDN_FILTERNAME_GIF) &&
					m_pExportOptions->GetFilterNameStrID() != _R(IDT_FILTERNAME_BMP);

	if (!bHasDPI)
	{
		SetStringGadgetValue(_R(IDC_T1COMBO1), _R(IDS_DPI_SCREEN), FALSE, -1);
		SetComboListLength(_R(IDC_T1COMBO1));
	}
	else
	{
		//  Set the initial value to be displayed from the current export options set.
		//  Also, store the depth in a variable which will be used in HandleColorDepthListChange().
		double dpi = m_pExportOptions->GetDPI();
		if (dpi != 50 && dpi != 75 && dpi != 96 && dpi != 100 && dpi != 150 &&
			dpi != 200 && dpi != 200 && dpi != 250 && dpi != 300)
			SetDoubleGadgetValue(_R(IDC_T1COMBO1), dpi);
		SetLongGadgetValue(_R(IDC_T1COMBO1), 50);
		SetLongGadgetValue(_R(IDC_T1COMBO1), 75);
		SetLongGadgetValue(_R(IDC_T1COMBO1), 96);
		SetLongGadgetValue(_R(IDC_T1COMBO1), 100);
		SetLongGadgetValue(_R(IDC_T1COMBO1), 150);
		SetLongGadgetValue(_R(IDC_T1COMBO1), 200);
		SetLongGadgetValue(_R(IDC_T1COMBO1), 250);
		SetLongGadgetValue(_R(IDC_T1COMBO1), 300);

		SetDoubleGadgetValue(_R(IDC_T1COMBO1), dpi, FALSE, -1);

		SetComboListLength(_R(IDC_T1COMBO1));
	}

	// gifs and bmps dont really have a dpi value so hide dpi drop down but leave the size
	EnableGadget(_R(IDC_T1COMBO1), bHasDPI);
	EnableGadget(_R(IDC_T1STATIC2), bHasDPI);

	m_LockSizeUpdates = FALSE;
}

void BmapPrevDlg::InitBitmapSizeSelectionRadioGroup()
{
	BOOL lock = m_LockSizeUpdates;
	m_LockSizeUpdates = TRUE;

	SetLongGadgetValue(_R(IDC_T1RADIO1), FALSE);
	SetLongGadgetValue(_R(IDC_T1RADIO2), FALSE);
	SetLongGadgetValue(_R(IDC_T1RADIO3), FALSE);

	// Set up the save spread/selection radio buttons
	if (m_FilterType == MAKE_BITMAP_FILTER)
	{
		// Disable all buttons
		EnableGadget(_R(IDC_T1RADIO3), FALSE);
		EnableGadget(_R(IDC_T1RADIO2), FALSE);
		EnableGadget(_R(IDC_T1RADIO1), FALSE);
	}
	else
	{
		// If selection is True then enable the spread/selection/drawing buttons otherwise
		// disable the selection button and allow choice between spread or drawing, drawing
		// being the default.
		switch (m_pExportOptions->GetSelectionType())
		{
			case DRAWING:
				SetLongGadgetValue(_R(IDC_T1RADIO2), TRUE);
				break;
			case SELECTION:
				SetLongGadgetValue(_R(IDC_T1RADIO3), TRUE);
				break;
			default:
			case SPREAD:
				SetLongGadgetValue(_R(IDC_T1RADIO1), TRUE);
				break;
			case ABITMAP:
				// Disable all buttons
				EnableGadget(_R(IDC_T1RADIO3), FALSE);
				EnableGadget(_R(IDC_T1RADIO2), FALSE);
				EnableGadget(_R(IDC_T1RADIO1), FALSE);
				EnableGadget(_R(IDC_T1COMBO1), FALSE);
				break;
		}
	}

	m_LockSizeUpdates = lock;
}

void BmapPrevDlg::InitBitmapSizeAntiAlisingRadioGroup()
{
	BOOL lock = m_LockSizeUpdates;
	m_LockSizeUpdates = TRUE;

	// If selection is True then enable the spread/selection/drawing buttons otherwise
	// disable the selection button and allow choice between spread or drawing, drawing
	// being the default.
	switch (m_pExportOptions->GetAntiAliasing())
	{
		case MAINTAIN_SCREEN_AA:
			SetLongGadgetValue(_R(IDC_T1RADIO_MAINTAIN_AA), TRUE);
			SetLongGadgetValue(_R(IDC_T1RADIO_MINIMISE_AA), FALSE);
			break;
		case MINIMISE_VISIBLE_AA:
			SetLongGadgetValue(_R(IDC_T1RADIO_MAINTAIN_AA), FALSE);
			SetLongGadgetValue(_R(IDC_T1RADIO_MINIMISE_AA), TRUE);
			break;
	}

	m_LockSizeUpdates = lock;
}

WinRect BmapPrevDlg::GetExportSize(double dpi)
{
	SelRange* pSelection = GetApplication()->FindSelection();

	// deal with exporting no selection as the drawing
	if (pSelection->Count() == 0 && (m_pExportOptions->GetSelectionType() == SELECTION || m_pExportOptions->GetSelectionType() == ABITMAP))
		m_pExportOptions->SetSelectionType(DRAWING);

	DocRect	ExportObjectSize;
	if (m_pExportOptions->GetSelectionType() == DRAWING)
	{
		// get the whole drawing size for now
		ExportObjectSize = BaseBitmapFilter::GetSizeOfDrawing();

	}
	else if (m_pExportOptions->GetSelectionType() == SELECTION || m_pExportOptions->GetSelectionType() == ABITMAP)
	{
		RangeControl rc		= pSelection->GetRangeControlFlags();
		RangeControl rcTemp	= rc;
		rcTemp.PromoteToParent = TRUE;
		pSelection->Range::SetRangeControl(rcTemp);

		// Get the bounding rectangle for the selection
		ExportObjectSize = pSelection->GetBoundingRect();

		pSelection->Range::SetRangeControl(rc);
	}
	else if (m_pExportOptions->GetSelectionType() == SPREAD)
	{
		// get the whole drawing size for now
		ExportObjectSize = BaseBitmapFilter::GetSizeOfSpread(Document::GetSelectedSpread());
	}
	else if (m_pExportOptions->GetSelectionType() == PAGE)
	{
		Page *pPage = Document::GetSelectedSpread()->FindFirstPageInSpread(); 
		if (pPage)
			ExportObjectSize = pPage->GetPageRect();
	}

	Matrix Identity;	// default construction is an identity matrix
	return OSRenderRegion::BitmapDocRectToWin( Identity, ExportObjectSize, dpi );
}


void BmapPrevDlg::HandleBitmapSizeDPIChange()
{
	if (m_LockSizeUpdates)
		return;

	bool bHasDPI =	m_pExportOptions->GetFilterNameStrID() != _R(IDN_FILTERNAME_GIF) &&
					m_pExportOptions->GetFilterNameStrID() != _R(IDT_FILTERNAME_BMP);
	if (!bHasDPI)
		return;

		
	BOOL valid;

	double value = GetDoubleGadgetValue(_R(IDC_T1COMBO1), BMPEXP_MINDPI, BMPEXP_MAXDPI, 
						0 /*_R(IDS_BMPPREFS_INVALIDDPI)*/ ,&valid);
	if (valid == FALSE)
		return;

//	// check if the pixel fields have valid values
//	if (ValidPixels == FALSE)
//	{
//		InformError(_R(IDS_BMPPREFS_DPITOOSMALL));
//		return;
//	}

	if (value == m_pExportOptions->GetDPI())
		return;

	WinRect	Rect = GetExportSize(value);

	// check the new width
	if (Rect.GetWidth() < 1 || Rect.GetWidth() > BMPEXP_MAXWIDTH)
		return;

	// and check the new height
	if (Rect.GetHeight() < 1 || Rect.GetHeight() > BMPEXP_MAXHEIGHT)
		return;

	// If returned value in range then set the new default
	m_pExportOptions->SetDPI(value);

	m_LockSizeUpdates = TRUE;
	SetLongGadgetValue(_R(IDC_T1EDIT1), Rect.GetWidth());
	SetLongGadgetValue(_R(IDC_T1EDIT2), Rect.GetHeight());
	m_LockSizeUpdates = FALSE;

	// set the size of the export into the export options
	m_pExportOptions->SetPixelOutputSize(Rect.GetWidth(), Rect.GetHeight());
	// JPNOTE remove the next line soon
	SetPreviewButtonState(true);
}

void BmapPrevDlg::HandleBitmapSizeWidthChange()
{
	if (m_LockSizeUpdates)
		return;

	BOOL valid = TRUE;
	INT32 Width  = GetLongGadgetValue(_R(IDC_T1EDIT1), 0, BMPEXP_MAXWIDTH, 0, &valid);
	if (!valid || Width < 1)
		return;

	double dpi = m_pExportOptions->GetDPI();
	WinRect	Rect = GetExportSize(dpi);
	if (dpi < 5 || Rect.GetWidth() == 0)
		return;

	// calc the new dpi
	double Resolution = ((double) Width * dpi) / ((double) Rect.GetWidth());
	if ((Resolution < BMPEXP_MINDPI) || (Resolution > BMPEXP_MAXDPI))
		return; // new dpi out of range

	// calc the new height
	INT32 Height = (INT32) ceil((Resolution * (double)Rect.GetHeight()) / dpi);

	if (Height < 1 || Height > BMPEXP_MAXHEIGHT)
		return;

	m_pExportOptions->SetDPI(Resolution);
	m_LockSizeUpdates = TRUE;

	SetLongGadgetValue(_R(IDC_T1EDIT2), Height);

	bool bHasDPI =	m_pExportOptions->GetFilterNameStrID() != _R(IDN_FILTERNAME_GIF) &&
					m_pExportOptions->GetFilterNameStrID() != _R(IDT_FILTERNAME_BMP);
	if (bHasDPI)
		SetDoubleGadgetValue(_R(IDC_T1COMBO1), Resolution, FALSE, -1);

	bool bHasScaleFactor =	m_pExportOptions->GetFilterNameStrID() == _R(IDN_FILTERNAME_GIF) ||
							m_pExportOptions->GetFilterNameStrID() == _R(IDT_FILTERNAME_BMP);
	if (bHasScaleFactor)
		RefreshBitmapSizeScaleFactor();
	
	m_LockSizeUpdates = FALSE;

	// set the size of the export into the export options
	m_pExportOptions->SetPixelOutputSize(Width, Height);
	SetPreviewButtonState(true);
}

void BmapPrevDlg::HandleBitmapSizeHeightChange()
{
	if (m_LockSizeUpdates)
		return;

	BOOL valid = TRUE;
	INT32 Height = GetLongGadgetValue(_R(IDC_T1EDIT2), 0, BMPEXP_MAXHEIGHT, 0, &valid);
	if (!valid || Height < 1)
		return;

	double dpi = m_pExportOptions->GetDPI();
	WinRect	Rect = GetExportSize(dpi);
	if (dpi < BMPEXP_MINDPI || Rect.GetHeight() == 0)
		return;

	// calc the new dpi
	double Resolution = ((double) Height * dpi) / ((double) Rect.GetHeight());
	if ((Resolution < BMPEXP_MINDPI) || (Resolution > BMPEXP_MAXDPI))
		return; // new dpi out of range

	// calc the new Width
	INT32 Width = (INT32) ceil((Resolution * (double)Rect.GetWidth()) / dpi);

	if (Width < 1 || Width > BMPEXP_MAXWIDTH)
		return;

	m_pExportOptions->SetDPI(Resolution);
	m_LockSizeUpdates = TRUE;

	SetLongGadgetValue(_R(IDC_T1EDIT1), Width);

	bool bHasDPI =	m_pExportOptions->GetFilterNameStrID() != _R(IDN_FILTERNAME_GIF) &&
					m_pExportOptions->GetFilterNameStrID() != _R(IDT_FILTERNAME_BMP);
	if (bHasDPI)
		SetDoubleGadgetValue(_R(IDC_T1COMBO1), Resolution, FALSE, -1);

	bool bHasScaleFactor =	m_pExportOptions->GetFilterNameStrID() == _R(IDN_FILTERNAME_GIF) ||
							m_pExportOptions->GetFilterNameStrID() == _R(IDT_FILTERNAME_BMP);
	if (bHasScaleFactor)
		RefreshBitmapSizeScaleFactor();
	
	m_LockSizeUpdates = FALSE;

	// set the size of the export into the export options
	m_pExportOptions->SetPixelOutputSize(Width, Height);
	SetPreviewButtonState(true);
}

void BmapPrevDlg::HandleBitmapSizeScaleFactorChange()
{
	if (m_LockSizeUpdates)
		return;

	bool bHasScaleFactor =	m_pExportOptions->GetFilterNameStrID() == _R(IDN_FILTERNAME_GIF) ||
							m_pExportOptions->GetFilterNameStrID() == _R(IDT_FILTERNAME_BMP);
	if (!bHasScaleFactor)
		return;

	double sf = GetDoubleGadgetValue(_R(IDC_T1EDIT3), 1, 5000);
	double dpi = m_pExportOptions->GetDPI();

	WinRect	rect = GetExportSize(dpi);

	if (dpi < 5 || rect.GetHeight() == 0)
		return;

	// calc the new dpi
	double newDPI = (sf * 96.0) / 100.0;
	if ((newDPI < 5.0) || (newDPI > 3000.0))
		return; // new dpi out of range

	m_pExportOptions->SetDPI(newDPI);

	// Force the size and DPI boxes to update
	InitBitmapSizeDPIList();

	// Force width and heigth to update
	WinRect wr = GetExportSize(m_pExportOptions->GetDPI());

	m_LockSizeUpdates = TRUE;
	SetLongGadgetValue(_R(IDC_T1EDIT1), wr.GetWidth());
	SetLongGadgetValue(_R(IDC_T1EDIT2), wr.GetHeight());
	m_LockSizeUpdates = FALSE;

	SetPreviewButtonState(true);
}

void BmapPrevDlg::HandleBitmapSizeAreaToSaveChange(SelectionType type)
{
	if (m_LockSizeUpdates)
		return;
	m_pExportOptions->SetSelectionType(type);
	// reset the radio blobs
	InitBitmapSizeSelectionRadioGroup();
	SetPreviewButtonState(true);
}

void BmapPrevDlg::HandleBitmapSizeAntiAliasingChange(BOOL DoMinimise)
{
	if (m_LockSizeUpdates)
		return;
	m_pExportOptions->SetAntiAliasing(DoMinimise ? MINIMISE_VISIBLE_AA : MAINTAIN_SCREEN_AA);
	InitBitmapSizeAntiAlisingRadioGroup();
	SetPreviewButtonState(true);
}

void BmapPrevDlg::HandleBitmapSizePutHTMLChange()
{
	if (m_ClipBoardUsage != CLIPBOARD_HTML)
		m_ClipBoardUsage = CLIPBOARD_HTML;
	else if (m_ClipBoardUsage == CLIPBOARD_HTML)
		m_ClipBoardUsage = CLIPBOARD_NONE;
	else
		ERROR3("This should not be possible");
	InitBitmapSizePutHTMLTick();
}

// the options may have changed
// update all the ctrls on this tab to show the new options
void BmapPrevDlg::RefreshBitmapSizeTab()
{
	// set the interlocking dpi controls
	m_LockSizeUpdates = TRUE;

	HandleBitmapSizeDPIChange();
	
	if (m_pPreviewDlg )
	{
		m_LockSizeUpdates = TRUE;
		WinRect wr = GetExportSize(m_pExportOptions->GetDPI());
		SetLongGadgetValue(_R(IDC_T1EDIT1), wr.GetWidth());
		SetLongGadgetValue(_R(IDC_T1EDIT2), wr.GetHeight());
		m_LockSizeUpdates = FALSE;
	}
	InitBitmapSizePutHTMLTick();
	InitBitmapSizeAntiAlisingRadioGroup();
	InitBitmapSizeSelectionRadioGroup();
	InitBitmapSizeDPIList();

	RefreshBitmapSizeScaleFactor();

	m_LockSizeUpdates = FALSE;
}

void BmapPrevDlg::RefreshBitmapSizeScaleFactor()
{
	bool bHasScaleFactor =	m_pExportOptions->GetFilterNameStrID() == _R(IDN_FILTERNAME_GIF) ||
							m_pExportOptions->GetFilterNameStrID() == _R(IDT_FILTERNAME_BMP);

	EnableGadget(_R(IDC_T1EDIT3), bHasScaleFactor);
	EnableGadget(_R(IDC_T1STATIC6), bHasScaleFactor);
	EnableGadget(_R(IDC_T1STATIC7), bHasScaleFactor);

	if (bHasScaleFactor)
	{
		// Bodge alert: To simulate a scale factor with out killing all the dodgy export code
		// we are going to use the DPI to do the job.
		double sf = (100.0 * m_pExportOptions->GetDPI()) / 96.0;
		SetDoubleGadgetValue(_R(IDC_T1EDIT3), sf);
	}
	else
	{
		SetDoubleGadgetValue(_R(IDC_T1EDIT3), 100);
	}
}


// //////////////////////////////////////////////////////////////////////////////////////// //
// //////////////////////////////////////////////////////////////////////////////////////// //
// Browser Preview Tab																		//
// //////////////////////////////////////////////////////////////////////////////////////// //
// //////////////////////////////////////////////////////////////////////////////////////// //

void BmapPrevDlg::HandleBrowserPreviewTabMsg(DialogMsg* Msg)
{
PORTNOTETRACE("other","BmapPrevDlg::BrowserPreviewGetOptions - Do nothing");
#if !defined(EXCLUDE_FROM_XARALX)
	CDlgResID PageID = GetCurrentPageID();	// Get currently selected Tab id
	TalkToPage(_R(IDD_TBROWSER));  // The Coords Page identifier

	switch(Msg->DlgMsg)
	{
		case DIM_CREATE:
		{
			m_bBrowserPreviewTabCreated = true;
			// Initialize these variables with the global values.
			// This allows the values to be stored in the program preferences.
//			BrowserBackground Bgr	=	PreviewInBrowserTab::g_Background;
//			BOOL Info				=	PreviewInBrowserTab::g_InfoInHtmlStub;
//			BOOL Imagemap			=	PreviewInBrowserTab::g_Imagemap;

			// Removed - Ranbir. We no longer want to assign thse variables default values. 
			//t_BrowserOptions.Get(&Bgr, &Info, &Imagemap);

			// set the background radiobuttons
			SetLongGadgetValue(_R(IDC_T5RADIO1), PreviewInBrowserTab::g_Background == BROWSER_BGR_NONE);     
			SetLongGadgetValue(_R(IDC_T5RADIO2), PreviewInBrowserTab::g_Background == BROWSER_BGR_DOC);     
			SetLongGadgetValue(_R(IDC_T5RADIO3), PreviewInBrowserTab::g_Background == BROWSER_BGR_BITMAP);     
			SetLongGadgetValue(_R(IDC_T5RADIO4), PreviewInBrowserTab::g_Background == BROWSER_BGR_CHECKER);     

			// Setup checkboxes
			SetBoolGadgetSelected(_R(IDC_T5CHECK1), PreviewInBrowserTab::g_InfoInHtmlStub);     
			SetBoolGadgetSelected(_R(IDC_T5CHECK2), PreviewInBrowserTab::g_Imagemap);     

//			SetBoolGadgetSelected(_R(IDC_BROWSER_PREVIEW), m_bDoPreviewInBrowser);     
			EnableGadget(_R(IDC_BROWSER_PREVIEW), FALSE); // as this 'feature' is broken

			break;
		}
		case DIM_LFT_BN_CLICKED:
		{
			if( Msg->GadgetID == _R(IDC_IMMEDIATE_BROWSER_PREVIEW) )
			{
				// click on the 'Browser Preview' button

				if (m_pPreviewDlg)
				{
					// get the browser options
					BrowserPreviewOptions BrowserOptions;
					BrowserPreviewGetOptions(&BrowserOptions);  				
					
					BOOL ok = TRUE;
					// Shold check here if we need to regenerate preview and if so do so
					// ok = DoPreview();
					
					// do the browser preview
					if (ok) m_pPreviewDlg->DoBrowserPreview(BrowserOptions);
				}
			}
			else
			if( Msg->GadgetID == _R(IDC_BROWSER_PREVIEW) )
			{
				// check whether to do preview in browser
//				m_bDoPreviewInBrowser = GetLongGadgetValue(_R(IDC_BROWSER_PREVIEW), 0, 0);
			}
		}
		break;

		case DIM_SET_ACTIVE:
			UpdateCurrentTab();
			break;

		default:
			TalkToPage(PageID);
			break;
	}
#endif
}

/********************************************************************************************
>	void BmapPrevDlg::BrowserPreviewGetOptions(BrowserPreviewOptions  &BrowserOptions)
	Author:		Stefan_Stoykov (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/7/97
	Outputs:	BrowserOptions - receives the browser options stored in the Browser tab's controls
	Purpose:	Gets the values of the browser controls.
********************************************************************************************/
void BmapPrevDlg::BrowserPreviewGetOptions(BrowserPreviewOptions *pBrowserOptions)
{
PORTNOTETRACE("other","BmapPrevDlg::BrowserPreviewGetOptions - Do nothing");
#ifndef EXCLUDE_FROM_XARALX
	if (!m_bBrowserPreviewTabCreated)
	{
		// The tab has not been created so give up
		return;
	}

	TalkToPage(_R(IDD_TBROWSER));

	// get the background choice
	BrowserBackground Bgr = BROWSER_BGR_NONE;
	if (GetLongGadgetValue(_R(IDC_T5RADIO1), 0, 0))
		Bgr = BROWSER_BGR_NONE;
	else if (GetLongGadgetValue(_R(IDC_T5RADIO2), 0, 0))
		Bgr = BROWSER_BGR_DOC;
	else if (GetLongGadgetValue(_R(IDC_T5RADIO3), 0, 0))
		Bgr = BROWSER_BGR_BITMAP;
	else if (GetLongGadgetValue(_R(IDC_T5RADIO4), 0, 0))
		Bgr = BROWSER_BGR_CHECKER;

	// get the info choice
	BOOL Info = GetLongGadgetValue(_R(IDC_T5CHECK1), FALSE, TRUE, 0);

	// get the imagemap choice
	BOOL Imagemap = GetLongGadgetValue(_R(IDC_T5CHECK2), FALSE, TRUE, 0);

	//Now get the imagemap options to use. First get a pointer to the Imagemap filter
	ImagemapFilter* imageMap = (ImagemapFilter*)Filter::FindFilterFromID(FILTERID_IMAGEMAP);

	if(imageMap==NULL)
	{
		ERROR2RAW("BmapPrevDlg::GetBrowserOptions - no Imagemap Filter");
		return;
	}

	//Get the current set of imagemap options from that filter
	ImagemapFilterOptions ifoOptionsToUse=imageMap->GetFilterOptions();

	//Now get the options from our imagemap tab
	ImageMapGetOptions(&ifoOptionsToUse);

	// Store these new values in the global variables,
	// this allows us to save the details in the program preferences.
	PreviewInBrowserTab::g_Background		= Bgr;
	PreviewInBrowserTab::g_InfoInHtmlStub	= Info;
	PreviewInBrowserTab::g_Imagemap			= Imagemap;

//	// Broadcast a bitmap preview properties changed message.
//	BROADCAST_TO_ALL(BrowserPrvwChgdMsg(BrowserPrvwChgdMsg::BrowserPrvwPropState::PROPERTIES_CHANGED));

	//And set those options into the browser options
	pBrowserOptions->Set(Bgr, Info, Imagemap, ifoOptionsToUse);
#endif
}

// //////////////////////////////////////////////////////////////////////////////////////// //
// //////////////////////////////////////////////////////////////////////////////////////// //
// Image Map Tab																			//
// //////////////////////////////////////////////////////////////////////////////////////// //
// //////////////////////////////////////////////////////////////////////////////////////// //
void BmapPrevDlg::HandleImageMapTabMsg(DialogMsg* Msg)
{
PORTNOTETRACE("other","BmapPrevDlg::HandleImageMapTabMsg - Do nothing");
#ifndef EXCLUDE_FROM_XARALX
	CDlgResID PageID = GetCurrentPageID();	// Get currently selected Tab id
	TalkToPage(_R(IDD_TIMAPOPTIONS));  // The Coords Page identifier

	switch(Msg->DlgMsg)
	{
		case DIM_CREATE:
		{
			m_bImageMapTabCreated = true;
			ImageMapOnCreate();
			break;
		}

		case DIM_TEXT_CHANGED:
		{
			if (Msg->GadgetID == _R(IDC_IMAPTAB_PATH))
			{
				ImageMapOnFileNameChanged();
			}
			break;
		}

		case DIM_LFT_BN_CLICKED:
		{
			if( Msg->GadgetID == _R(IDC_IMAPTAB_EXPORTTOCLIPBOARD) ||
				Msg->GadgetID == _R(IDC_IMAPTAB_EXPORTTOFILE) )
			{
				//Get the state of the two Export... switches
				BOOL fFile		= GetLongGadgetValue(_R(IDC_IMAPTAB_EXPORTTOFILE), FALSE, TRUE);
				BOOL fClipboard	= GetLongGadgetValue(_R(IDC_IMAPTAB_EXPORTTOCLIPBOARD), FALSE, TRUE);

				if (fClipboard)
					m_ClipBoardUsage = CLIPBOARD_IMAGEMAP;
				else if (m_ClipBoardUsage == CLIPBOARD_IMAGEMAP)
					m_ClipBoardUsage = CLIPBOARD_NONE;

				//And enable the imagemap options gadgets if either of them is on
				//Otherwise, disable the gadgets
				ImageMapEnableOptions(fFile || fClipboard);
				ImageMapEnableExportToFileOptions(fFile);
			}
			else
			if( Msg->GadgetID == _R(IDC_IMAPTAB_BROWSE) )
			{
				ImageMapOnBrowseClicked();
			}
			else
			if( Msg->GadgetID == _R(IDC_IMAPTAB_ADDDESIGNNOTE) )
			{
				//  User clicked on the 'Use Design Notes' checkbox

				BOOL bCurrentState = m_pExportOptions->GetCanUseDesignNotes();
				//  Flip the value of the bitmap export options variable.
				bCurrentState = !bCurrentState;

				m_pExportOptions->SetCanUseDesignNotes( bCurrentState );

				SetRegistryFlagForDesignNotes(bCurrentState);
			}
			break;
		}

		case DIM_SET_ACTIVE:
			UpdateCurrentTab();
			break;

		default:
			TalkToPage(PageID);
			break;

	}
#endif
}

/********************************************************************************************
>	void BmapPrevDlg::ImageMapOnCreate()
	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/7/97
	Purpose:	This function is called when the imagemap tab is first called up by
				the user.

				It sets up the controls on the tab to appropriate defaults.
********************************************************************************************/
void BmapPrevDlg::ImageMapOnCreate()
{
PORTNOTETRACE("other","BmapPrevDlg::ImageMapOnCreate - Do nothing");
#ifndef EXCLUDE_FROM_XARALX
	//Make sure we're talking to the right page
	TalkToPage(_R(IDD_TIMAPOPTIONS));

	//  Set the initial state of the 'Design Notes' checkbox
	//  Depending on whether the DreamWeaver programme is available or not, and whether the file
	//  has been named or not, we can enable/disable the appropriate controls.
	wxString DocName = _T("\\") + Document::GetCurrent()->GetPathName();	//  File name

	BOOL bUsesDesignNotes	= UsesDesignNotes();			//  Is Dreamweaver 3 installed?
	BOOL bFileSaved			= DocName != _("\\");			//  Has the current document been saved?

	SetLongGadgetValue( _R(IDC_IMAPTAB_ADDDESIGNNOTE), m_pExportOptions->GetCanUseDesignNotes() );
	if( (bUsesDesignNotes && bFileSaved) /*|| !m_pExportOptions->GetCanUseDesignNotes()*/)
	{
		//  Everything we need is here, so enable both controls
		// these controls are always enabled when this  tab is visable - it gives feedback if it is ticked when it shouldn't be
		//EnableGadget( _R(IDC_IMAPTAB_DESIGN_NOTES), TRUE );
		//EnableGadget( _R(IDC_IMAPTAB_ADDDESIGNNOTE), TRUE );
		//  Since the controls are enabled, we must decide whether to
		//  tick the checkbox or not.

		//  Make sure that the 2 text labels are hidden
		HideGadget( _R(IDC_IMAPTAB_STATIC3), TRUE );
		HideGadget( _R(IDC_IMAPTAB_STATIC4), TRUE );
	}
	else
	{
		//  Can't do a design note, so disable both controls
		//EnableGadget( _R(IDC_IMAPTAB_DESIGN_NOTES), FALSE );
		//EnableGadget( _R(IDC_IMAPTAB_ADDDESIGNNOTE), FALSE );
		//  Display a text message that informs the user what the problem is.
		if( !bUsesDesignNotes )		//  Dreamweaver 3 is not installed
		{
			//  Show the appropriate text label
			HideGadget( _R(IDC_IMAPTAB_STATIC3), FALSE );
			//  Hide the other one
			HideGadget( _R(IDC_IMAPTAB_STATIC4), TRUE );
		}
		else	//  Document not saved yet.
		{
			//  Show the appropriate text label
			HideGadget( _R(IDC_IMAPTAB_STATIC4), FALSE );
			//  Hide the other one
			HideGadget( _R(IDC_IMAPTAB_STATIC3), TRUE );
		}
	}

	if (m_bSlicingImage)
	{
		//  If we are doing image slicing, then all the controls 
		//  in this tab have to be disabled.

		//  'Export image map to':
		EnableGadget(_R(IDC_EXP_IMAPTAB_OPTIONS),		FALSE);
		EnableGadget(_R(IDC_IMAPTAB_EXPORTTOCLIPBOARD),	FALSE);
		EnableGadget(_R(IDC_IMAPTAB_INSERT),			FALSE);
		EnableGadget(_R(IDC_IMAPTAB_EXPORTTOFILE),		FALSE);
		EnableGadget(_R(IDC_IMAPTAB_REPLACE),			FALSE);
		EnableGadget(_R(IDC_IMAPTAB_PATH),				FALSE);
		EnableGadget(_R(IDC_IMAPTAB_BROWSE),			FALSE);

		//  'Dreamweaver integration': Should be enabled
		EnableGadget( _R(IDC_IMAPTAB_DESIGN_NOTES), TRUE );
		EnableGadget( _R(IDC_IMAPTAB_ADDDESIGNNOTE), TRUE );

		//  'Image map options':
		EnableGadget( _R(IDC_IMAPTAB_OPTIONS), FALSE );
		EnableGadget( _R(IDC_IMAPTAB_STATIC1), FALSE );
		EnableGadget( _R(IDC_IMAPTAB_MAPNAME), FALSE );
		EnableGadget( _R(IDC_IMAPTAB_STATIC2), FALSE );
		EnableGadget( _R(IDC_IMAPTAB_APPROX), FALSE );
		EnableGadget( _R(IDC_IMAPTAB_RECTANGLES), FALSE );

		return;
	}

	//First find the imagemap filter
	ImagemapFilter *pImagemapFilter = (ImagemapFilter*)Filter::FindFilterFromID(FILTERID_IMAGEMAP);

	if(!pImagemapFilter)
	{
		ERROR2RAW("BmapPrevDlg::OnCreateImagemap - no Imagemap Filter");
		return;
	}

	//Get the current set of ImagemapFilterOptions
	ImagemapFilterOptions ifoCurrent=pImagemapFilter->GetFilterOptions();

	//First get the value for the ExportToFile switch
	BOOL fFile=ifoCurrent.ExportToFile();

	SetLongGadgetValue(_R(IDC_IMAPTAB_EXPORTTOFILE),		fFile);

	//And make sure we're on the right page again
	TalkToPage(_R(IDD_TIMAPOPTIONS));

//	//Now the "File name" edit field
//	//Is the bitmap we are exporting the same as the last bitmap we exported?
//	String_256 str1=m_pthExport.GetPath(FALSE);
//	String_256 str2=m_pthLastExport.GetPath(FALSE);
//
//	BOOL fIsSameAsLastExport=(str1==str2);
//
//	if (fIsSameAsLastExport)
//	{
//		//Yes. So use the last imagemap path we exported to
//		String_256 strToUse=m_pthLastImagemapExport.GetPath(FALSE);
//		SetStringGadgetValue(_R(IDC_IMAPTAB_PATH), &strToUse, FALSE, -1);
//	}
//	else
//	{

	//No. So use the same path as the bitmap we are exporting to, with the
	//extension changed to "htm"
	PathName pthToUse=m_pthExport;

	pthToUse.SetType("htm");
	
	String_256 strToUse=pthToUse.GetPath(FALSE);
	SetStringGadgetValue(_R(IDC_IMAPTAB_PATH), strToUse, FALSE, -1);

//	}

	//Set up the Approximate... combo box
	DeleteAllValues(_R(IDC_IMAPTAB_APPROX));

	SetStringGadgetValue(_R(IDC_IMAPTAB_APPROX), String_256(_R(IDS_IMAGEMAP_APPROX_VERYCLOSELY)), FALSE, 0);
	SetStringGadgetValue(_R(IDC_IMAPTAB_APPROX), String_256(_R(IDS_IMAGEMAP_APPROX_CLOSELY)), FALSE, 1);
	SetStringGadgetValue(_R(IDC_IMAPTAB_APPROX), String_256(_R(IDS_IMAGEMAP_APPROX_APPROXIMATELY)), FALSE, 2);
	SetStringGadgetValue(_R(IDC_IMAPTAB_APPROX), String_256(_R(IDS_IMAGEMAP_APPROX_NOTATALL)), FALSE, 3);
	
	SetComboListLength(_R(IDC_IMAPTAB_APPROX));

	switch (ifoCurrent.m_ffApprox)
	{
		case FF_VERYCLOSELY:
			SetSelectedValueIndex(_R(IDC_IMAPTAB_APPROX), 0);
			break;

		case FF_CLOSELY:
			SetSelectedValueIndex(_R(IDC_IMAPTAB_APPROX), 1);
			break;

		case FF_APPROXIMATELY:
			SetSelectedValueIndex(_R(IDC_IMAPTAB_APPROX), 2);
			break;

		case FF_NOTATALL:
			SetSelectedValueIndex(_R(IDC_IMAPTAB_APPROX), 3);
			break;

		default:
			ERROR2RAW("ImagemapDlg::SetOptions - Unrecognised Curve Approx Factor");
			break;
	}

	//And the "Map Name..." combo box
	DeleteAllValues(_R(IDC_IMAPTAB_MAPNAME));

	//Now the Insert/Replace radio buttons
	//Note that although we set the values of these controls here, we set
	//whether the controls are displayed or not under "OnFileNameChanged" below
	SetLongGadgetValue(_R(IDC_IMAPTAB_INSERT), ifoCurrent.Insert());
	SetLongGadgetValue(_R(IDC_IMAPTAB_REPLACE), !ifoCurrent.Insert());

	//This function sets up the drop-down list, by parsing the file referred to in the
	//pathname edit field, getting the names of any imagemaps referred to in that file
	//and putting those names in the drop-down box
	ImageMapOnFileNameChanged(); 

	//And now to set up the name that actually appears in the edit field portion 
	//of the combo box
//	if (fIsSameAsLastExport)
//	{
//		//Use the last map name
//		SetStringGadgetValue(_R(IDC_IMAPTAB_MAPNAME), &ifoCurrent.m_strName, FALSE, -1);
//	}
//	else
	{
		//Use the file name of the bitmap, with the extension removed, 
		//as the map name
		PathName pthToUse=m_pthExport;

		String_256 strToUse=pthToUse.GetFileName(FALSE);
		SetStringGadgetValue(_R(IDC_IMAPTAB_MAPNAME), strToUse, FALSE, -1);
	}

	SetComboListLength(_R(IDC_IMAPTAB_MAPNAME));
	
	//And get the rectangle box
	SetLongGadgetValue(_R(IDC_IMAPTAB_RECTANGLES), ifoCurrent.ExportAllRectangles());

	//Now if we are exporting an imagemap (either to file or to clipboard)
	//then ungrey all the imagemap options. Otherwise grey them all.
	ImageMapEnableOptions(fFile || m_ClipBoardUsage == CLIPBOARD_IMAGEMAP);

	//And if we are exporting an imagemap to the file, then ungrey the
	//file name edit field and the browse button, otherwise grey them both
	ImageMapEnableExportToFileOptions(fFile);

	RefreshImageMapTab();
#endif
}

void BmapPrevDlg::RefreshImageMapTab()
{
	SetLongGadgetValue(_R(IDC_IMAPTAB_EXPORTTOCLIPBOARD), m_ClipBoardUsage == CLIPBOARD_IMAGEMAP);
}


/********************************************************************************************
  >	void BmapPrevDlg::ImageMapGetOptions(ImagemapFilterOptions* pOptions)
	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/7/97
	Purpose:	Gets the values from the Imagemap tab and puts them into
				the pOptions structure.
********************************************************************************************/
void BmapPrevDlg::ImageMapGetOptions(ImagemapFilterOptions* pOptions)
{
	ERROR3IF(m_bSlicingImage, "This should not be called if we are slicing");

	if (!m_bImageMapTabCreated)
	{
		// The tab has not been created so give up
		return;
	}

	TalkToPage(_R(IDD_TIMAPOPTIONS)); 

	//Get the values of the ExportToFile and ExportToClipboard buttons
	BOOL fFile		= GetLongGadgetValue(_R(IDC_IMAPTAB_EXPORTTOFILE), FALSE, TRUE);
	BOOL fClipboard = m_ClipBoardUsage == CLIPBOARD_IMAGEMAP;

	//And set our variables into the options object
	pOptions->m_fFile=fFile;
	pOptions->m_fClipboard=fClipboard;

	//So, if we are exporting to a file
	if (fFile)
	{
		//Then remember the path to export to
		pOptions->m_pthFile=GetStringGadgetValue(_R(IDC_IMAPTAB_PATH));

		//And if that file exists
		if (SGLibOil::FileExists(&(pOptions->m_pthFile)))
		{
			//Then get the values of the insert/replace radio buttons
			pOptions->m_fInsert=GetLongGadgetValue(_R(IDC_IMAPTAB_INSERT), FALSE, TRUE);
		}
	}

	//And if we are exporting either to a file or to the clipboard
	if (fFile || fClipboard)
	{
		//Then get the values from the dialog and set them
		//into the ImagemapFilterOptions object

		//The map name
		pOptions->m_strName=GetStringGadgetValue(_R(IDC_IMAPTAB_MAPNAME));

		//The "Approximate curves with lines" drop down box
		switch (GetSelectedValueIndex(_R(IDC_IMAPTAB_APPROX)))
		{
			case 0:
				pOptions->m_ffApprox=FF_VERYCLOSELY;
				break;

			case 1:
				pOptions->m_ffApprox=FF_CLOSELY;
				break;

			case 2:
				pOptions->m_ffApprox=FF_APPROXIMATELY;
				break;

			case 3:
				pOptions->m_ffApprox=FF_NOTATALL;
				break;

			default:
				ERROR2RAW("ImagemapDlg::GetOptions - Unrecognised drop-down box index");
				break;
		}

		//The "save clickable areas as rectangles" radio button
		pOptions->m_fAllRectangles=(GetLongGadgetValue(_R(IDC_IMAPTAB_RECTANGLES), 0, 1)==1);
	}
}


/********************************************************************************************
>	void BmapPrevDlg::OnCommitImagemap()
	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/7/97
	Purpose:	This function is called when the OK button is clicked.
				It takes the values from the controls on the imagemap tab and sets
				them back into the Imagemap filter.
********************************************************************************************/
void BmapPrevDlg::ImageMapOnCommit()
{
PORTNOTETRACE("other","BmapPrevDlg::ImageMapOnCommit - Do nothing");
#ifndef EXCLUDE_FROM_XARALX
	// Check if we have opened the tab or not. If we haven't then it is not a good
	// idea to get get values from an uninitialised tab!
	if (!m_bImageMapTabCreated)
		return;

	// we do not do image slicing as well as image map stuff
	// one or the other please
	if (m_bSlicingImage)
		return;
		
	// Get a pointer to the imagemap filter
	ImagemapFilter *pImagemapFilter=(ImagemapFilter*)Filter::FindFilterFromID(FILTERID_IMAGEMAP);

	if(pImagemapFilter==NULL)
	{
		ERROR2RAW("BmapPrevDlg::OnCommitImagemap - no Imagemap Filter");
		return;
	}

	//Now get the current set of ImagemapFilterOptions from the Imagemap filter
	ImagemapFilterOptions ifoCurrent=pImagemapFilter->GetFilterOptions();

	//And put the values from the imagemap tab into that object
	ImageMapGetOptions(&ifoCurrent);

	// If this is not set, the user does not get any warning when they try and
	// export with an image map but don't set a web address.
	ifoCurrent.m_fReportErrors = TRUE;

	//And set the options back into the filter
	pImagemapFilter->SetFilterOptions(ifoCurrent);
/*
	//Now, if we are exporting either to a file or to the clipboard
	if (ifoCurrent.m_fFile || ifoCurrent.m_fClipboard)
	{
		//Then update some varialbes

		//Firstly, the last bitmap path we exported to while exporting an imagemap
		m_pthLastExport=m_pthExport;

		//Second, the last path we exported an imagemap to
		m_pthLastImagemapExport=ifoCurrent.m_pthFile;

		//And thirdly, the last map name
		m_strLastMapName=ifoCurrent.m_strName;
	}
*/
#endif
}

/********************************************************************************************
>	void BmapPrevDlg::EnableImagemapOptions(BOOL fValue)
	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/7/97
	Inputs:		fValue - whether to grey or ungrey the Imagemap Options controls
	Purpose:	Greys or ungreys all the Imagemap Options controls
********************************************************************************************/
void BmapPrevDlg::ImageMapEnableOptions(BOOL fValue)
{
	EnableGadget(_R(IDC_IMAPTAB_MAPNAME),		fValue);
	EnableGadget(_R(IDC_IMAPTAB_APPROX),		fValue);
	EnableGadget(_R(IDC_IMAPTAB_RECTANGLES),	fValue);
}

/********************************************************************************************
>	void BmapPrevDlg::EnableExportToFileOptions(BOOL fValue)
	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/7/97
	Inputs:		fValue - whether to grey or ungrey the Export To File controls
	Purpose:	Greys or ungreys all the Export To File controls
********************************************************************************************/
void BmapPrevDlg::ImageMapEnableExportToFileOptions(BOOL fValue)
{
	EnableGadget(_R(IDC_IMAPTAB_PATH),		fValue);
	EnableGadget(_R(IDC_IMAPTAB_BROWSE),	fValue);
	EnableGadget(_R(IDC_IMAPTAB_INSERT),	fValue);
	EnableGadget(_R(IDC_IMAPTAB_REPLACE),	fValue);
}

/********************************************************************************************
>	void BmapPrevDlg::ImageMapOnBrowseClicked()
	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/7/97
	Purpose:	This function is called when the "Browse..." button is clicked.

				It brings up a file dialog to allow the user to enter a file to save into.
				Then it puts the path the user entered into the pathname field.
********************************************************************************************/
void BmapPrevDlg::ImageMapOnBrowseClicked()
{
PORTNOTETRACE("other","BmapPrevDlg::ImageMapOnBrowseClicked - Do nothing");
#ifndef EXCLUDE_FROM_XARALX
	//First get our default filename from the Path edit field
	String_256 strFile=GetStringGadgetValue(_R(IDC_IMAPTAB_PATH));
	PathName pthFile(strFile);

	//Now create a text character pointer
	const TCHAR* pcBuffer=NULL;

	//And if we have a valid filename
	if (pthFile.IsValid())
		//Then set that pointer to point at our file name
		pcBuffer=strFile;

	//If the filename is not valid, then the text buffer pointer stays at NULL

	//And now get our string which will initialise our filters
	String_256 strFilters(_R(IDS_IMAPTAB_BROWSEFILTERSTRING));

	//Now create a Save dialog object
	MFCFileDialog dlgSave(FALSE, "htm", pcBuffer, OFN_HIDEREADONLY, (TCHAR*) strFilters);

	//Open it
	dlgSave.DoModal();

	//Now get the path name that the user chose
	String_256 strChosen=dlgSave.GetPathName();

	//Talk to the right page
	TalkToPage(_R(IDD_TIMAPOPTIONS)); 
	
	//And put that path name string into the filename field
	SetStringGadgetValue(_R(IDC_IMAPTAB_PATH), strChosen, FALSE, -1);

	//Then call the function that we call every time the filename changes
	ImageMapOnFileNameChanged();
#endif
}

/********************************************************************************************
>	void BmapPrevDlg::OnFileNameChanged()
	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/7/97
	Purpose:	This function is called whenever the pathname field changes

				It checks if the file exists. If so, it

				a. Brings up the Insert/Replace radio buttons
				b. Parses the file to get the names of all the imagemaps, and
					puts those names in the drop-down list of the Name combo box
********************************************************************************************/
void BmapPrevDlg::ImageMapOnFileNameChanged()
{
PORTNOTETRACE("other","BmapPrevDlg::ImageMapOnFileNameChanged - Do nothing");
#ifndef EXCLUDE_FROM_XARALX
	//Return if the imagemap tab isn't created yet
	if (!m_bImageMapTabCreated)
		return;

	TalkToPage(_R(IDD_TIMAPOPTIONS));

	//First let's get the path of the file in the edit field
	String_256 strOpen=GetStringGadgetValue(_R(IDC_IMAPTAB_PATH));

	PathName pthOpen;

	//If that file exists...
	if (pthOpen.SetPathName(strOpen) && SGLibOil::FileExists(&pthOpen))
	{
		//Yes it does

		//So first display the Insert/Replace radio buttons
		HideGadget(_R(IDC_IMAPTAB_INSERT), FALSE);
		HideGadget(_R(IDC_IMAPTAB_REPLACE), FALSE);

		//We want to parse this file to find out all the names of imagemaps in it

		//Now create a list to put all the map names in
		List lstStrings;

		//First find the imagemap filter
		ImagemapFilter *pImagemapFilter = (ImagemapFilter*)Filter::FindFilterFromID(FILTERID_IMAGEMAP);

		if(!pImagemapFilter)
		{
			ERROR2RAW("BmapPrevDlg::OnCreateImagemap - no Imagemap Filter");
			return;
		}

		pImagemapFilter->SearchFileForImagemaps(pthOpen, &lstStrings);

		//Now we want to iterate through all the items in the list and put them
		//into the Map Name combo box

		//So first clear all items in the combo box
		DeleteAllValues(_R(IDC_IMAPTAB_MAPNAME));

		//And get the first item in the list
		StringListItem* pThisItem=(StringListItem*) lstStrings.GetHead();
		INT32 i=0;

		//And until we get to the end of the list
		while (pThisItem)
		{
			//Add the string into the combo box
			String_256 strToUse=pThisItem->GetString();

			SetStringGadgetValue(_R(IDC_IMAPTAB_MAPNAME), strToUse, FALSE, i);

			//And increase our current position
			i++;

			//And get the next item
			pThisItem=(StringListItem*) lstStrings.GetNext(pThisItem);
		}

		//Delete our list
		lstStrings.DeleteAll();

		SetComboListLength(_R(IDC_IMAPTAB_MAPNAME)); 

		//And set the default map name into the list, which is the name of the
		//bitmap file without the extension
		String_256 strDefault=pthOpen.GetFileName(FALSE);
		SetStringGadgetValue(_R(IDC_IMAPTAB_MAPNAME), strDefault, FALSE, -1);
		
	} //END if the file exists
	else
	{
		//No, the file doesn't exist

		//So hide the insert/replace radio buttons
		HideGadget(_R(IDC_IMAPTAB_INSERT), TRUE);
		HideGadget(_R(IDC_IMAPTAB_REPLACE), TRUE);
	}
#endif
}

// //////////////////////////////////////////////////////////////////////////////////////// //
// //////////////////////////////////////////////////////////////////////////////////////// //
// Bitmap Options Tab																		//
// //////////////////////////////////////////////////////////////////////////////////////// //
// //////////////////////////////////////////////////////////////////////////////////////// //

void BmapPrevDlg::HandleBitmapOptionsTabMsg(DialogMsg* Msg)
{
	CDlgResID PageID = GetCurrentPageID();	// Get currently selected Tab id
	TalkToPage(_R(IDD_TBITMAPOPTIONS)); 
	switch(Msg->DlgMsg)
	{
	case DIM_CREATE:
		SetGadgetRange( _R(IDC_OPTIONSTAB_JPEG_SLIDER), 0, 100 );
		SetGadgetBitmaps( _R(IDC_OPTIONSTAB_JPEG_SLIDER), _R(IDB_SLIDERBASE), _R(IDB_SLIDERSLIDER) );
		InitBitmapOptionsTab();
		break;

	case DIM_LFT_BN_CLICKED:
		HandleBitmapOptionsTicks(Msg->GadgetID);
		break;

	case DIM_SLIDER_POS_IDLE:
	case DIM_SLIDER_POS_SET:
		if (m_pExportOptions->GetFilterNameStrID() == _R(IDS_JPG_EXP_FILTERNAME))
		{
			INT32 SliderPos = GetLongGadgetValue(_R(IDC_OPTIONSTAB_JPEG_SLIDER), 0, 100);
			((JPEGExportOptions*) m_pExportOptions)->SetQuality(100-SliderPos);
			HandleBitmapOptionsTicks(Msg->GadgetID);		
			DoPreview();
		}
		break;

	case DIM_SLIDER_POS_CHANGING:
		if (m_pExportOptions->GetFilterNameStrID() == _R(IDS_JPG_EXP_FILTERNAME))
		{
			INT32 SliderPos = GetLongGadgetValue(_R(IDC_OPTIONSTAB_JPEG_SLIDER), 0, 100);
			String_256 QualityPercent = "";
			QualityPercent.MakeMsg(_R(IDS_ZOOM_INFO_FORMAT), 100-SliderPos);

			SetStringGadgetValue(_R(IDC_OPTIONSTAB_JPEG_TEXT4), QualityPercent);
		}
		break;

	case DIM_SET_ACTIVE:
		UpdateCurrentTab();
		break;

	default:
		TalkToPage(PageID);
		break;
	}
}

// this function ticks the bits dependant on which graphic type we are
void BmapPrevDlg::InitBitmapOptionsTab()
{
	// is the slider visible
	// this is only shown for jpegs
	BOOL ShowSlider = m_pExportOptions->GetFilterNameStrID() == _R(IDS_JPG_EXP_FILTERNAME);

	HideGadget(_R(IDC_OPTIONSTAB_JPEG_TEXT1), !ShowSlider);
	HideGadget(_R(IDC_OPTIONSTAB_JPEG_TEXT2), !ShowSlider);
	HideGadget(_R(IDC_OPTIONSTAB_JPEG_TEXT3), !ShowSlider);
	HideGadget(_R(IDC_OPTIONSTAB_JPEG_TEXT4), !ShowSlider);
	HideGadget(_R(IDC_OPTIONSTAB_JPEG_SLIDER), !ShowSlider);

	if (ShowSlider)
	{
		INT32 Quality = 100-((JPEGExportOptions*) m_pExportOptions)->GetQuality();
		SetLongGadgetValue(_R(IDC_OPTIONSTAB_JPEG_SLIDER), Quality);
		String_256 QualityPercent = "";
		QualityPercent.MakeMsg(_R(IDS_ZOOM_INFO_FORMAT), 100-Quality);

		SetStringGadgetValue(_R(IDC_OPTIONSTAB_JPEG_TEXT4), QualityPercent);
	}

	if( m_pExportOptions->GetFilterNameStrID() == _R(IDS_JPG_EXP_FILTERNAME) )
	{
		SetStringGadgetValue(_R(IDC_OPTIONSTAB_CHECK1), _R(IDS_OPTIONS_TAB_PROGRESSIVE));
		SetStringGadgetValue(_R(IDC_OPTIONSTAB_CHECK2), _R(IDS_OPTIONS_TAB_ORIGINAL_JPEG));
		HideGadget(_R(IDC_OPTIONSTAB_CHECK1), FALSE);
		HideGadget(_R(IDC_OPTIONSTAB_CHECK2), TRUE);
		HideGadget(_R(IDC_OPTIONSTAB_CHECK3), TRUE);
		SetLongGadgetValue(_R(IDC_OPTIONSTAB_CHECK1), ((JPEGExportOptions*) m_pExportOptions)->DoAsProgressive());
	}
	else
	if( m_pExportOptions->GetFilterNameStrID() == _R(IDT_FILTERNAME_BMP) )
	{
		HideGadget(_R(IDC_OPTIONSTAB_CHECK1), TRUE);
		HideGadget(_R(IDC_OPTIONSTAB_CHECK2), TRUE);
		HideGadget(_R(IDC_OPTIONSTAB_CHECK3), TRUE);
	}
//	default:
//	case _R(IDN_FILTERNAME_GIF):
//	case _R(IDS_FILTERNAME_PNG):
	else
	{
		SetStringGadgetValue(_R(IDC_OPTIONSTAB_CHECK1), _R(IDS_OPTIONS_TAB_INTERLACED));
		SetStringGadgetValue(_R(IDC_OPTIONSTAB_CHECK2), _R(IDS_OPTIONS_TAB_TRANSPARENT));
		SetStringGadgetValue(_R(IDC_OPTIONSTAB_CHECK3), _R(IDS_OPTIONS_TAB_LAYERFILES));
		HideGadget(_R(IDC_OPTIONSTAB_CHECK1), FALSE);
		HideGadget(_R(IDC_OPTIONSTAB_CHECK2), FALSE);
		HideGadget(_R(IDC_OPTIONSTAB_CHECK3), FALSE);
		SetLongGadgetValue(_R(IDC_OPTIONSTAB_CHECK1), ((MaskedFilterExportOptions*) m_pExportOptions)->WantInterlaced());
		SetLongGadgetValue(_R(IDC_OPTIONSTAB_CHECK2), m_pExportOptions->IsBackgroundTransparent());
		SetLongGadgetValue(_R(IDC_OPTIONSTAB_CHECK3), m_pExportOptions->GetSeparateLayerFiles());
	}

}

void BmapPrevDlg::HandleBitmapOptionsTicks(UINT32 tick)
{
	BOOL NewState = GetLongGadgetValue(tick,0,1);

	UINT32	idFilterName = m_pExportOptions->GetFilterNameStrID();

	if( idFilterName == _R(IDS_JPG_EXP_FILTERNAME) )
		if (tick == _R(IDC_OPTIONSTAB_CHECK1))
			((JPEGExportOptions*) m_pExportOptions)->SetMakeProgressive(NewState);
	else
	if( idFilterName == _R(IDT_FILTERNAME_BMP) ) 
	{
		// Do nothing
	}
	else
	{
		if (tick == _R(IDC_OPTIONSTAB_CHECK1))
			((MaskedFilterExportOptions*) m_pExportOptions)->SetMakeInterlaced(NewState);
		else if (tick == _R(IDC_OPTIONSTAB_CHECK2))
			m_pExportOptions->SetBackgroundTransparency(NewState);
		else if (tick == _R(IDC_OPTIONSTAB_CHECK3))
			m_pExportOptions->SetSeparateLayerFiles(NewState);
	}

	// show the controls as they now are
	InitBitmapOptionsTab();
	// reset the preview button
	SetPreviewButtonState(true);
}

// //////////////////////////////////////////////////////////////////////////////////////// //
// //////////////////////////////////////////////////////////////////////////////////////// //
// Utility functions																		//
// //////////////////////////////////////////////////////////////////////////////////////// //
// //////////////////////////////////////////////////////////////////////////////////////// //
/*
BOOL BmapPrevDlg::DeleteItemFromDropList( INT32 ListID, INT32 ItemID )
{
	//  Look for the item in the list
	INT32 Position = GetPositionOfItemInDropList( ListID, ItemID );
	//  If found, then remove it from the list
	if( Position >= 0 )
	{
		DeleteValue( ListID, FALSE, Position );
		return TRUE;
	}
	return FALSE;	//  It didn't work
}
*/
BOOL BmapPrevDlg::SetSelectedDropListItem( INT32 ListID, INT32 ItemID )
{
	//  Is ItemID in the drop list ListID
	INT32 Position = GetPositionOfItemInDropList( ListID, ItemID );
	//  If we have found it, the select it
	if( Position >= 0 )
	{
		SetSelectedValueIndex( ListID, Position );
		return TRUE;
	}
	return FALSE;	//  It didn't work
}

INT32 BmapPrevDlg::GetPositionOfItemInDropList( INT32 ListID, INT32 ItemID )
{
	//  A few variables.
	String_256 ListItemString = "";
	INT32 Index = -1;
	INT32 NumberOfListItems = 0;
	//  Get the number of entries in the list
	GetValueCount( ListID, &NumberOfListItems );
	//  Go through all the entries, and see if you can find the one which matches.
	for( INT32 i = 0; i < NumberOfListItems; i++ )
	{
		//  Get the string at the specified index.
		ListItemString = GetStringGadgetValue( ListID, NULL, i );
		//  If this item is the one we want, then return its index now.
		if( ListItemString.IsIdentical( String_256( ItemID ) ) )
			return i;
	}
	//  We haven't found the item, so return -1 to say this.
	return Index;
}

// locked colours and edited colours need to store their pre-edited colour
// so that the renderer knows where the original colour would have been before
// the user changed it. see.
// this fn Sets the pre-edited colour of the index to match the existing colour
// or to match the params passed in if any were
// sjk (22/12/00)
void BmapPrevDlg::SetPreEditedPaletteColour(INT32 r, INT32 g, INT32 b)
{
PORTNOTETRACE("other","BmapPrevDlg::SetPreEditedPaletteColour - Do nothing");
#ifndef EXCLUDE_FROM_XARALX
	// set the preedited colour to match the edited colour
	ExtendedPalette	*palette = m_pExportOptions->GetExtendedPalette();
	if (palette == NULL)
		return;

	INT32 index = m_PaletteControl.GetSelectedColour();
	if (index <0 || index >= palette->NumberOfColours)
		return;

	// if this value has been edited before (set to something from the default black)
	// - dont do it again since it means it has just
	// been edited twice and the original is still the original
	if (palette->Data[index].PreEditedRed != 0 || palette->Data[index].PreEditedGreen != 0 || palette->Data[index].PreEditedBlue != 0)
		return;

	palette->Data[index].PreEditedRed = (r == -1 ? palette->Data[index].Red : r);
	palette->Data[index].PreEditedGreen = (g == -1 ? palette->Data[index].Green : g);
	palette->Data[index].PreEditedBlue = (b == -1 ? palette->Data[index].Blue : b);
#endif
}
