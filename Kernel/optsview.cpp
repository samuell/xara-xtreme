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
// Implementation of the View tab of the options dialog box

/*

*/

#include "camtypes.h"
//WEBSTER-ranbirr-13/11/96
#ifndef WEBSTER
#include "app.h"		// Camelot object
#include "docview.h"
#include "appprefs.h"
//#include "prefsdlg.h"	// dialog/gadget ids
//#include "optsres.h"	// error strings
#include "optsmsgs.h"	// error strings
#include "optsview.h"
#include "app.h"		// Camelot object
#include "units.h"		// units
#include "ccolbar.h"	// show new state of colour bar
//#include "jason.h"		// _R(IDS_COLCONTEXTNAME)
#include "colcontx.h"	// For ColourContexts
#include "palman.h"
//#include "coldlog.h"
#include "bmpcomp.h"	// for BitmapList

CC_IMPLEMENT_DYNAMIC(ViewTab, OptionsTabs)   
			  
// This is not compulsory, but you may as well put it in so that the correct version
// of your file can be registered in the .exe
DECLARE_SOURCE("$Revision$");

/********************************************************************************************

>	ViewTab::ViewTab() 


	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/12/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	ViewTab constructor. Creates a non-undoable operation
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

ViewTab::ViewTab()
{   
	// Set up our private variables to known states
	GreyStatus = TRUE;

	// Variables to store the old control states.
	OldShowTransparency = TRUE;	
 	OldBackgroundRedraw = TRUE; 
	OldPropScroll = TRUE;
}        

/********************************************************************************************

>	ViewTab::~ViewTab()


	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/12/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	ViewTab destructor
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
																				
ViewTab::~ViewTab()
{   
}        

/********************************************************************************************

>	BOOL ViewTab::Init()


	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/12/94
	Inputs:		-
	Outputs:	-
	Returns:	True if initialised ok, False if failed.
	Purpose:	ViewTab initialisation routine
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
																				
BOOL ViewTab::Init()
{   
	return TRUE;
}        


/******************************************************************************************

>	CDlgResID ViewTab::GetPageID()()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/12/94
	Inputs:		-
	Outputs:	-
	Returns:	The dialog ID of this tab section.	  
	Purpose:	Allows the options dialog code to determine the dialog ID of this section.
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

CDlgResID ViewTab::GetPageID()
{
	return _R(IDD_OPTSTAB_VIEW);
}

/******************************************************************************************

>	BOOL ViewTab::IsDocumentOption()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/12/94
	Inputs:		-
	Outputs:	-
	Returns:	Returns True if this tab is a document option.	  
	Purpose:	Allows the document options dialog code to determine if this tab belongs
				to the group of document options.
	Errors:		-
	SeeAlso:	IsProgramOption();

******************************************************************************************/

BOOL ViewTab::IsDocumentOption()
{
	return FALSE;	// This tab is not a document option 
}

/******************************************************************************************

>	BOOL ViewTab::IsProgramOption()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/12/94
	Inputs:		-
	Outputs:	-
	Returns:	Returns True if this tab is a program option.	  
	Purpose:	Allows the program options dialog code to determine if this tab belongs
				to the group of program options.
	Errors:		-
	SeeAlso:	IsProgramOption();

******************************************************************************************/

BOOL ViewTab::IsProgramOption()
{
	return TRUE;	// This tab is a program option 
}


/******************************************************************************************

>	BOOL ViewTab::DecodeModelListItem(BOOL *AutoColourModel, ColourModel *DisplayModel)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/4/95
	Inputs:		-
	Outputs:	AutoColourModel	Is the automatic selection state
				DisplayModel	Is the colour model chosen
	Returns:	True if taken from list ok, False otherwise.	  
	Purpose:	Takes the selected item in the automatic colour model sleection list and
				translates it into the two items required:-
					- Automatic colour model selection flag
					- colour model selected 
	Errors:		-
	SeeAlso:	CommitSection;

******************************************************************************************/

BOOL ViewTab::DecodeModelListItem(BOOL *AutoColourModel, ColourModel *DisplayModel)
{
TRACEUSER( "Neville", _T("ViewTab::DecodeModelListItem\n"));
	ERROR3IF(pPrefsDlg == NULL, "ViewTab::DecodeModelListItem called with no dialog pointer");

	// Decode the selected item in the automatic colour model list into the two preference
	// values we require
	
	// Set up default values in case of early exit
	*AutoColourModel = FALSE;
	*DisplayModel = COLOURMODEL_HSVT;

	// Get the number of the selected item in the list
	WORD Index;
	pPrefsDlg->GetValueIndex(_R(IDC_OPTS_AUTOCOLOUR), &Index);

	// If the selected item is zero then Automatic has been chosen
	if (Index == 0)
	{
		// Set up the return variables
		*AutoColourModel = TRUE;
		*DisplayModel = COLOURMODEL_HSVT;
	}
	else if ((Index > 0) && (Index < (MAX_COLOURMODELS + 1)))
	{
		// Remove the automatic from the list
		Index--;
		
		// Work out the colour model required
		ColourContextArray ColContexts;
		ColourContext::GetGlobalDefaults(&ColContexts);

		// Variable for holding the selected model number
		INT32 NewModelNum;

		// Work through the colour model list an index number of times
		for (NewModelNum = 0; NewModelNum < MAX_COLOURMODELS; NewModelNum++)
		{
			if (ColContexts.Context[NewModelNum] != NULL)
			{
				if ((Index--) <= 0)
					break;
			}
		}

		// Set up the return variables
		*AutoColourModel = FALSE;
		*DisplayModel = (ColourModel) NewModelNum;
	}
	else
	{
		// something nasty has happened	
		return FALSE;
	}

	return TRUE;
	
}	


/******************************************************************************************

>	BOOL ViewTab::CommitBitmapSmoothingFlag( BOOL bNewSetting )

	Author:		Andy_Hills (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/11/00
	Inputs:		bNewSetting		new bitmap smoothing setting
	Outputs:	-
	Returns:	TRUE on success
	Purpose:	Commits the 'allow bitmap smoothing when scaled up' setting.
				All bitmaps which are affected by the setting will be redrawn.
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

BOOL ViewTab::CommitBitmapSmoothingFlag( BOOL bNewSetting )
{
	if (!pDocument)
		return(TRUE);
	
	// Invalidate any affected bitmaps.
	/* BOOL bOldSetting =*/ pDocument->GetBitmapSmoothing();

	// Enable the new setting
	pDocument->SetBitmapSmoothing(bNewSetting);


	// get the document's list of bitmaps
	BitmapList *pBitmapList = pDocument->GetBitmapList();
	ERROR3IF( (!pBitmapList), "ViewTab::CommitBitmapSmoothingFlag - couldn't get bitmap list" );
	DWORD ListSize = pBitmapList->GetCount();

	// get the first bitmap in the list
	KernelBitmap *pBitmap = (KernelBitmap*) (pBitmapList->GetHead());

	// for every KernelBitmap in the current document 
	while(ListSize>0)
	{
		// validate the bitmap ptr
		ERROR3IF( (!(pBitmap->IsKindOf(CC_RUNTIME_CLASS(KernelBitmap)))), "ViewTab::CommitBitmapSmoothingFlag - pBitmap is not a KernelBitmap" );
		
		// This is now redundant.
		// Since the document bitmap smoothing flag now affects all bitmaps,
		// but the individual bitmap smoothing flags only affect bitmaps which
		// are scaled up, the rule for whether or not we invalidate a bitmap
		// is much more complicated (we can't just invalidate all references to
		// one bitmap, since some of them may be scaled up, others scaled down,
		// etc.)
		// So we will invalidate ALL bitmaps instead!
		
		/*
		// work out whether the bitmap was being smoothed before the change
		BOOL bWasSmoothed    = (pBitmap->GetInterpolation() && bOldSetting);
		BOOL bWillBeSmoothed = (pBitmap->GetInterpolation() && bNewSetting);

		// if the bitmap was smoothed but isn't now (or vice versa) then invalidate it
		if( bWasSmoothed != bWillBeSmoothed )
		{
			// all nodes which use the bitmap will be redrawn
			pBitmap->GetActualBitmap()->InvalidateAllReferences(pDocument);
		}
		*/

		pBitmap->GetActualBitmap()->InvalidateAllReferences(pDocument);

		// get the next bitmap in the list
		pBitmap = (KernelBitmap*) (pBitmapList->GetNext(pBitmap));
		ListSize--;
	}

	return TRUE;
}


/******************************************************************************************

>	BOOL ViewTab::CommitSection()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/12/94
	Inputs:		-
	Outputs:	-
	Returns:	True if values in dialog box ok, False otherwise.	  
	Purpose:	Takes the values in the View section tab of the options dialog box and
				sets the associated	preference values accordingly
				Called when ok is pressed on the dialog box.
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

BOOL ViewTab::CommitSection()
{
TRACEUSER( "Neville", _T("commit view section\n"));
	ERROR3IF(pPrefsDlg == NULL, "ViewTab::CommitSection called with no dialog pointer");

	BOOL ok = pPrefsDlg->TalkToPage(_R(IDD_OPTSTAB_VIEW));	// The View View identifier
	if (!ok)
		return TRUE;	// Talk to View failed to return now

	// Section = View

	// Ok has been pressed so take the values from this section of the dialog box
	BOOL Valid=TRUE;		// Flag for validity of value
	BOOL State=FALSE;		// Flag for state of button/switch
	BOOL SetOk=TRUE;		// Preference value set ok

	// Section = Display

	// Now the colour bar mode combo box
	INT32 ColourBarMode = 0;
	ColourBarMode = pPrefsDlg->GetSelectedValueIndex(_R(IDC_OPTS_COLOURBAR));
TRACEUSER( "Neville", _T("commit colour bar mode='%d' \n"),ColourBarMode);
	SetOk = Camelot.SetPrefValue(TEXT("Displays"), TEXT("ColourBarMode"), &ColourBarMode);
	ERROR2IF(!SetOk,2,_R(IDE_OPTS_SETPREF_VIEW));

#ifndef STANDALONE
	CColourBar::SetDisplayMode();		// Try and get the colour bar to update itself
#endif

	// Now tell other users that there is a possibly new colour bar display mode
	// in operation and so update any necessary items. 
	BROADCAST_TO_ALL(OptionsChangingMsg(pDocument, OptionsChangingMsg::NEWCOLOURBARMODE));

	// Now set up the other preference values according to the switches

	// Automatic colour model selection in colour editor
	BOOL AutoColourModel;
	ColourModel DisplayModel;
	DecodeModelListItem(&AutoColourModel, &DisplayModel);

	// added by CGS ....
	// since the default colour model can now be set from within the colour editor (coldlog.cpp)
	// we now need to force it from in here as well (otherwise the colour editor will save the one
	// that it is currently using - and NOT the one that we have just applied here!

PORTNOTE("other", "Disabled Colour Editor dialog")
#ifndef EXCLUDE_FROM_XARALX
	OpDescriptor* pOpDescriptor = (OpDescriptor*) OpDescriptor::FindOpDescriptor( OPTOKEN_COLOUREDITDLG );

	String_256 Dummy;
	OpState ColourEditDlgState = pOpDescriptor->GetOpsState(&Dummy);

	if (ColourEditDlgState.Ticked)
	{
		ColourEditDlg* pColourEditDlg = ColourEditDlg::GetColourEditDlg ();
		ASSERT (pColourEditDlg);

		pColourEditDlg->SetDefaultDisplayModel (DisplayModel);
	}
#endif

PORTNOTE("other", "Disabled colour editor model preference writing")
#ifndef EXCLUDE_FROM_XARALX
	SetOk = Camelot.SetPrefValue(TEXT("Displays"), TEXT("ColourEditorAutoModel"), &AutoColourModel);
	INT32 DisplayModelPref = (INT32)DisplayModel;	
	SetOk = SetOk && Camelot.SetPrefValue(TEXT("Displays"), TEXT("ColourEditorDisplayModel"), &DisplayModelPref);
	ERROR2IF(!SetOk,2,_R(IDE_OPTS_SETPREF_VIEW));
#endif

	// Error diffused view option
	BOOL ViewDither = 2;
	ViewDither = GetSelectedDither();
	SetOk = Camelot.SetPrefValue(_T("Screen"), _T("ViewDither"), &ViewDither);
	ERROR2IF(!SetOk,2,_R(IDE_OPTS_SETPREF_VIEW));

	PaletteManager::RedrawAllPalettedWindows(NULL);

	// Section = Bitmaps

	// The 'allow bitmap smoothing when scaled up' setting has changed.
	BOOL bSmoothing = ! pPrefsDlg->GetBoolGadgetSelected(_R(IDC_OPTS_SMOOTH_BITMAPS), 0, NULL, -1 );
	ok = CommitBitmapSmoothingFlag(bSmoothing);
	if (!ok) return FALSE;

	INT32 m_compoundConvertToEditableShapesDPI = pPrefsDlg->GetLongGadgetValue(_R(IDC_OPTS_COMPOUNDDPI), 10, 1000, _R(IDE_OPTS_INVALIDCOMPOUNDDPISIZE), &Valid);
	if (Valid)
	{
		BOOL SetOk = Camelot.SetPrefValue(TEXT("Displays"), TEXT("CompCToEDPI"), &m_compoundConvertToEditableShapesDPI);
		ERROR2IF(!SetOk,2,_R(IDE_OPTS_SETPREF_TUNE));
	}


	// Automatic colour line, gallery scroll to colour selected
	State = pPrefsDlg->GetLongGadgetValue(_R(IDC_OPTS_AUTOSCROLL), 0, 1, 0, &Valid);
	SetOk = Camelot.SetPrefValue(TEXT("Displays"), TEXT("AutoScrollColours"), &State);
	ERROR2IF(!SetOk,2,_R(IDE_OPTS_SETPREF_VIEW));

// Removed 18/12/95 as non longer required
//	// Show progress hourglass
//	State = pPrefsDlg->GetLongGadgetValue(_R(IDC_OPTS_HOURGLASS), 0, 1, 0, &Valid);
////	SetOk = Camelot.SetPrefValue(TEXT("Displays"), TEXT("Hourglass"), &State);
//		// We need a special access routine for this pref, as we want to create & destroy as
//	// it changes value.
//	extern BOOL ProgressCursorSetPref(BOOL fNewState);
//	SetOk = ProgressCursorSetPref(State);
//	ERROR2IF(!SetOk,2,_R(IDE_OPTS_SETPREF_VIEW));

PORTNOTE("other", "Disabled progress bar writing")
#ifndef EXCLUDE_FROM_XARALX
	// Show progress bar.
	State = pPrefsDlg->GetLongGadgetValue(_R(IDC_OPTS_PROGRESS), 0, 1, 0, &Valid);
	SetOk = Camelot.SetPrefValue(TEXT("Displays"), TEXT("ProgressBar"), &State);
	ERROR2IF(!SetOk,2,_R(IDE_OPTS_SETPREF_VIEW));
#endif

	// Section = Window

	// Now try and set the current view's state, if there is one.  	
	DocView *pView = DocView::GetSelected();
	BOOL BackgroundRedraw = TRUE;

	// Proportional scrollbar state option (for main view) removed 10/10/05 by Marc.
	// The current scrollbars are always proportional so it didn't make sense
	
	if (pView != NULL)
	{
		// Get the background redraw state and set new state accordingly
		BackgroundRedraw	= pPrefsDlg->GetLongGadgetValue(_R(IDC_OPTS_BACKREDRAW), 0, 1, 0, &Valid);
		pView->SetForeBackMode(BackgroundRedraw);
		// The preference value is set from the default document.
		//SetOk = Camelot.SetPrefValue(TEXT("Rendering"), TEXT("BackgroundRendering"), &BackgroundRedraw);
		//ERROR2IF(!SetOk,2,_R(IDE_OPTS_SETPREF_VIEW));
	}

	// Read the state of the interactive fill editing switch and set the new value in the preferences
	BOOL InteractiveFill = pPrefsDlg->GetLongGadgetValue(_R(IDC_OPTS_INTERACTIVEFILL), 0, 1, 0, &Valid);
	SetOk = Camelot.SetPrefValue(TEXT("Dragging"), TEXT("InteractiveFillEditing"), &InteractiveFill);
	ERROR2IF(!SetOk,2,_R(IDE_OPTS_SETPREF_VIEW));

// Removed 29/8/95 as transparency prompt dead
	// Read the state of the auto transparancy switch and set the new value in the preferences
//	BOOL AutoTransparency = pPrefsDlg->GetLongGadgetValue(_R(IDC_OPTS_AUTOTRANS), 0, 1, 0, &Valid);
//	SetOk = Camelot.SetPrefValue(TEXT("Rendering"), TEXT("AutoTransparency"), &AutoTransparency);
//	ERROR2IF(!SetOk,2,_R(IDE_OPTS_SETPREF_VIEW));

	// Read the state of the show transparancy switch and set the new value in the
	// selected document. Read vack value afterwards as the user will have been asked to 
	// confirm the operation 
// Removed 27/8/96; Transparency Control is DEAD 
/*
	BOOL ShowTransparency = pPrefsDlg->GetLongGadgetValue(_R(IDC_OPTS_SHOWTRANS), 0, 1, 0, &Valid);
	if (pDocument != NULL)
	{
		// According to the state of the switch, try to enable or disable transparency
		BOOL NewState;
		if (ShowTransparency)
			NewState = pDocument->TransparencyState(Enable);
		else
			NewState = pDocument->TransparencyState(Disable);
*/
		// If transparancy is present and the user has tried to turn it off then they will
		// have been prompted as to whether they want to or not. Therefore, we must reset
		// the switch state according to thevalue returned to us.

// removed 27/8/96 Transparency Control is DEAD 
//		pPrefsDlg->SetLongGadgetValue(_R(IDC_OPTS_SHOWTRANS), NewState);

// Removed 29/8/95 as transparency prompt dead
		// Just in case, we will check the state of the automatic transparency switch
//		BOOL NewAutoTransparency = FALSE;
//		BOOL ReadOk = Camelot.GetPrefValue(TEXT("Rendering"), TEXT("AutoTransparency"),
//										   &NewAutoTransparency);
//		if (NewAutoTransparency != 0)
//			pPrefsDlg->SetLongGadgetValue(_R(IDC_OPTS_AUTOTRANS), TRUE);
//		else
//			pPrefsDlg->SetLongGadgetValue(_R(IDC_OPTS_AUTOTRANS), FALSE);
//	} 
// Removed this rampant ERROR2 when no documents are present
//	else
//		ERROR2(FALSE,_R(IDE_OPTS_READPREF_VIEW));


	// Set the document altered state true if any of the document/view based items have been
	// changed but only if we have a valid document and view.
	if (pView != NULL && pDocument != NULL)
	{
		//removed 27/8/96 Transparency Control DEAD 
		if (
	//		OldShowTransparency != ShowTransparency || 
		 	OldBackgroundRedraw != BackgroundRedraw
		   )
		{
			pDocument->SetModified(TRUE);   	
		}
		   
		// Now remember the new states
	//	OldShowTransparency = ShowTransparency;	removed 27/8/96 transparency control DEAD
	 	OldBackgroundRedraw = BackgroundRedraw;
	}

	return TRUE;
}

/********************************************************************************************

>	BOOL ViewTab::GreySection()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/2/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Called when the user has closed all documents 
	Errors:		-
	SeeAlso:	ViewTab::UngreySection;

********************************************************************************************/

BOOL ViewTab::GreySection()
{
TRACEUSER( "Neville", _T("GreySection in ViewTab section\n"));
	ERROR2IF(pPrefsDlg == NULL,FALSE,"ViewTab::GreySection called with no dialog pointer");

	BOOL ok = pPrefsDlg->TalkToPage(_R(IDD_OPTSTAB_VIEW));	// The ViewTab identifier
	if (!ok)
		return TRUE;	// Talk to page failed to return now

	// Make sure the information field displaying the name of the current document
	// is correct.
	String_256	DocumentName(_R(IDT_OPTS_VIEW_INFO)); 
	DocumentName +=	*GetDocumentName();
	pPrefsDlg->SetStringGadgetValue(_R(IDC_OPTS_INFO), DocumentName);

	// Only update if we are not already grey 
	if (GreyStatus == TRUE)
		return TRUE;

	// Call our central greying/ungreying function
	ok = ChangeControlStatus(FALSE);
	
	GreyStatus = TRUE;

	return ok;
} 

/********************************************************************************************

>	BOOL ViewTab::UngreySection()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/2/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Called when the user has selected a new document or the title has been changed.
	Errors:		-
	SeeAlso:	ViewTab::UngreySection;

********************************************************************************************/

BOOL ViewTab::UngreySection()
{
TRACEUSER( "Neville", _T("UngreySection in ViewTab section\n"));
	ERROR2IF(pPrefsDlg == NULL,FALSE,"ViewTab::UngreySection called with no dialog pointer");

	BOOL ok = pPrefsDlg->TalkToPage(_R(IDD_OPTSTAB_VIEW));	// The ViewTab identifier
	if (!ok)
		return TRUE;	// Talk to page failed to return now

	// Make sure the information field displaying the name of the current document
	// is correct.
	String_256	DocumentName(_R(IDT_OPTS_VIEW_INFO)); 
	DocumentName +=	*GetDocumentName();
	pPrefsDlg->SetStringGadgetValue(_R(IDC_OPTS_INFO), DocumentName);

	BOOL bSmoothing = TRUE;
	if (pDocument)
		bSmoothing = pDocument->GetBitmapSmoothing();
	pPrefsDlg->SetBoolGadgetSelected(_R(IDC_OPTS_SMOOTH_BITMAPS), !bSmoothing, 0 );

	// Only update if we are not already ungrey 
	if (GreyStatus == FALSE)
		return TRUE;

	// Call our central greying/ungreying function
	ok = ChangeControlStatus(TRUE);
	
	GreyStatus = FALSE;

	return ok;
}

/********************************************************************************************

>	BOOL ViewTab::ChangeControlStatus(const BOOL Status)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/2/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Called to grey/ungrey all controls on this tab. 
	Errors:		-
	SeeAlso:	ViewTab::GreySection;	ViewTab::UngreySection;

********************************************************************************************/

BOOL ViewTab::ChangeControlStatus(const BOOL Status)
{
	// Grey/ungrey the controls
//	pPrefsDlg->EnableGadget(_R(IDC_OPTS_VIEWINFO), Status);

//	pPrefsDlg->EnableGadget(_R(IDC_OPTS_DISPLAYGROUP), Status);
////	pPrefsDlg->EnableGadget(_R(IDC_OPTS_HOURGLASS), Status);
//	pPrefsDlg->EnableGadget(_R(IDC_OPTS_PROGRESS), Status);

//	pPrefsDlg->EnableGadget(_R(IDC_OPTS_COLOURLINEGROUP), Status);
//	pPrefsDlg->EnableGadget(_R(IDC_OPTS_COLOURBAR), Status);

//	pPrefsDlg->EnableGadget(_R(IDC_OPTS_COLOUREDITOR), Status);
//	pPrefsDlg->EnableGadget(_R(IDC_OPTS_AUTOCOLOUR), Status);
//	pPrefsDlg->EnableGadget(_R(IDC_OPTS_AUTOCOLOURTXT), Status);
//	pPrefsDlg->EnableGadget(_R(IDC_OPTS_AUTOSCROLL), Status);

//	pPrefsDlg->EnableGadget(_OPTS_INTERACTIVEFILL, Status);
//	pPrefsDlg->EnableGadget(_R(IDC_OPTS_AUTOTRANS), Status);

	pPrefsDlg->EnableGadget(_R(IDC_OPTS_INFO), Status);
//	removed transparency control 27/8/96 
//	pPrefsDlg->EnableGadget(_R(IDC_OPTS_SHOWTRANS), Status);
	pPrefsDlg->EnableGadget(_R(IDC_OPTS_SCROLLBARS), Status);
	pPrefsDlg->EnableGadget(_R(IDC_OPTS_BACKREDRAW), Status);

	pPrefsDlg->EnableGadget(_R(IDC_OPTS_SMOOTH_BITMAPS), Status);

	return TRUE;
}			

/********************************************************************************************

>	BOOL ViewTab::UpdateSection(String_256 *DocumentName)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/2/95
	Inputs:		Name of document
	Outputs:	-
	Returns:	-
	Purpose:	Called when we have switched to a new document and need to update all the 
				controls on this tab. Should only init the tab if the page is present.  
				The document name allows the info field on the tab to be filled in correctly.
	Errors:		-
	SeeAlso:	ViewTab::GreySection; ViewTab::UngreySection; ViewTab::InitSection;

********************************************************************************************/

BOOL ViewTab::UpdateSection(String_256 *DocumentName)
{
TRACEUSER( "Neville", _T("ViewTab::UpdateSection\n"));
	ERROR2IF(pPrefsDlg == NULL,FALSE,"ViewTab::UpdateSection called with no dialog pointer");

	BOOL ok = pPrefsDlg->TalkToPage(_R(IDD_OPTSTAB_VIEW));	// The ViewTab identifier
	if (!ok)
		return TRUE;		// page not present

	// Remove any lists that we have created.
	//pPrefsDlg->DeleteAllValues(_R(IDC_OPTS_COLOURBAR));

	// Update any document/view specific bits
	GetDisplayStyleForView();

	// Set up the information field to reflect the current document name
	String_256	DocName(_R(IDT_OPTS_VIEW_INFO)); 
	DocName +=	*DocumentName;
	pPrefsDlg->SetStringGadgetValue(_R(IDC_OPTS_INFO), DocName);

	BOOL bSmoothing = TRUE;
	if (pDocument)
		bSmoothing = pDocument->GetBitmapSmoothing();
	pPrefsDlg->SetBoolGadgetSelected(_R(IDC_OPTS_SMOOTH_BITMAPS), !bSmoothing, 0 );

	// Call the normal init section code.
	// Not required are only a few items which are document based and there are no fields
	// displayed using units.
	//return InitSection();
	return ok;
}


/********************************************************************************************

>	BOOL ViewTab::UpdateTransparencyState(const BOOL NewState)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/2/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Called when the transparency state has changed and so we must update the
				switch state.
	Errors:		-
	SeeAlso:	ViewTab::UpdateSection;

********************************************************************************************/
// removed 27/8/96; Transparency Control DEAD  
/*
BOOL ViewTab::UpdateTransparencyState(const BOOL NewState)
{
TRACEUSER( "Neville", _T("ViewTab::UpdateTransparencyState\n"));
	ERROR2IF(pPrefsDlg == NULL,FALSE,"ViewTab::UpdateTransparencyState called with no dialog pointer");

	BOOL ok = pPrefsDlg->TalkToPage(_R(IDD_OPTSTAB_VIEW));	// The ViewTab identifier
	if (!ok)
		return TRUE;		// page not present

	// Set the specified new state for the document's transparency state
	if (NewState != 0)
		pPrefsDlg->SetLongGadgetValue(_R(IDC_OPTS_SHOWTRANS), TRUE);
	else
		pPrefsDlg->SetLongGadgetValue(_R(IDC_OPTS_SHOWTRANS), FALSE);

	// If the show transparency button is turned off and the automatic
	// transparency is on then force it off.
//	BOOL AutoTrans = pPrefsDlg->GetLongGadgetValue(_R(IDC_OPTS_AUTOTRANS), 0, 1, 0, &Valid);
//	if (!NewState && AutoTrans)
//	{
//		pPrefsDlg->SetLongGadgetValue(_R(IDC_OPTS_AUTOTRANS), FALSE);
//	}

	return(TRUE);
}	*/

/********************************************************************************************

>	BOOL ViewTab::HandleMsg(DialogMsg* Msg)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/12/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Handles all the View options tabs messages 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL ViewTab::HandleMsg(DialogMsg* Msg)
{
TRACEUSER( "Neville", _T("HandleViewMsg\n"));
	ERROR2IF(Msg == NULL,FALSE,"ViewTab::Message null message received");
	ERROR2IF(pPrefsDlg == NULL,FALSE,"ViewTab::HandleMsg called with no dialog pointer");

	BOOL ok = pPrefsDlg->TalkToPage(_R(IDD_OPTSTAB_VIEW));	// The View View identifier
	if (!ok)
		return TRUE;		// page not present

	switch(Msg->DlgMsg)
	{
		case DIM_CREATE:	// Initialise controls
			GreyStatus = FALSE;	// we are ungreyed by default
			ok = InitSection();
			if (!ok)
				InformError();
			break;
		case DIM_LFT_BN_CLICKED:
			OptionsTabs::SetApplyNowState(TRUE);
//			switch (Msg->GadgetID)
//			{
//				case _R(IDC_OPTS_SHOWTRANS):
//					// If the show transparency button is turned off and the automatic
//					// transparency is on then force it off.
//					BOOL Valid;
//					BOOL ShowTrans = pPrefsDlg->GetLongGadgetValue(_R(IDC_OPTS_SHOWTRANS), 0, 1, 0, &Valid);
//					BOOL AutoTrans = pPrefsDlg->GetLongGadgetValue(_R(IDC_OPTS_AUTOTRANS), 0, 1, 0, &Valid);
//					if (!ShowTrans && AutoTrans)
//					{
//						pPrefsDlg->SetLongGadgetValue(_R(IDC_OPTS_AUTOTRANS), FALSE);
//					}
//					break;
//			}
//			break;
		case DIM_SELECTION_CHANGED_COMMIT:
		case DIM_TEXT_CHANGED:
			OptionsTabs::SetApplyNowState(TRUE);
			break;
		default:
			break;
	}

	return TRUE;
}  

/******************************************************************************************

>	BOOL ViewTab::GetDisplayStyleForView()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/3/95
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if successful, else FALSE
	Purpose:	Reads the attributes associated with the current view. These include:-
				 - background redraw state
				 - proportional scroll bars state  
				 - transparency state  
	Errors:		-
	SeeAlso:	AppPrefsDlg; OptionsTabs;

******************************************************************************************/

BOOL ViewTab::GetDisplayStyleForView()
{
	// Now the document's transparency state
	if (pDocument != NULL)
	{
//removed 27/8/96; Transparency Control DEAD  
//		BOOL ShowTransparency = pDocument->TransparencyState(NoChange);
//		if (ShowTransparency != 0)
//			pPrefsDlg->SetLongGadgetValue(_R(IDC_OPTS_SHOWTRANS), TRUE);
//		else
//			pPrefsDlg->SetLongGadgetValue(_R(IDC_OPTS_SHOWTRANS), FALSE);

		// We must have a selected document to have a selected view
		DocView *pView = DocView::GetSelected();
		if (pView != NULL)
		{
		 	BOOL BackgroundRedraw; 
			// Set up the background redraw switch
			BackgroundRedraw = pView->GetForeBackMode();
			//ReadOk = Camelot.GetPrefValue(TEXT("Rendering"), TEXT("BackgroundRendering"), &BackgroundRedraw);
	TRACEUSER( "Neville", _T("get background redraw '%d'\n"),BackgroundRedraw);
			// Seems to return -1 so cannot use value directly
			if (BackgroundRedraw == FALSE)
				pPrefsDlg->SetLongGadgetValue(_R(IDC_OPTS_BACKREDRAW), FALSE);
			else
				pPrefsDlg->SetLongGadgetValue(_R(IDC_OPTS_BACKREDRAW), TRUE);
			//ERROR2IF(!ReadOk,FALSE,_R(IDE_OPTS_READPREF_VIEW));

			// Remember these values for later use
		 	OldBackgroundRedraw = BackgroundRedraw; 
		}

		// Remember this value for later use
//		OldShowTransparency = ShowTransparency; removed 27/8/96 transparency control DEAD
	}
	else
	{
		// Ensure that the dodgy options are greyed and unuseable 
		GreySection();	
		return FALSE;
	}
		
	// Everything went well.
	return TRUE;
}	



/******************************************************************************************

>	BOOL ViewTab::InitAutoColourModelList(ColourModel DisplayModel, INT32 *SelectedIndex)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/4/95
	Inputs:		DisplayModel	preference for the colour model	to be used
	Outputs:	SelectedIndex	returned index into the list
	Returns:	TRUE if successful, else FALSE
	Purpose:	Sets initial list for the automatic colour model selection but also returns
				the colour model passed in as an index into the list available.
	Errors:		-
	SeeAlso:	InitSection;

******************************************************************************************/

BOOL ViewTab::InitAutoColourModelList(ColourModel DisplayModel, INT32 *SelectedIndex)
{
TRACEUSER( "Neville", _T("ViewTab::InitAutoColourModelList\n"));
	ERROR2IF(pPrefsDlg == NULL,FALSE,"ViewTab::InitAutoColourModelList called with no dialog pointer");

	// Wipe the list back to nothing, just in case
	pPrefsDlg->DeleteAllValues(_R(IDC_OPTS_AUTOCOLOUR));
	
	ColourContextArray ColContexts;
	ColourContext::GetGlobalDefaults(&ColContexts);

	// General variables
	String_64 NameString;
	INT32 Index = 0;
	String_32 ModelName;
	// Set up decent return value in case of early exit
	*SelectedIndex = 0;

	// First item in the list is an automatic.
	ModelName = String(_R(IDT_OPTS_AUTOMATIC));  	// automatic colour model	
	NameString.MakeMsg(_R(IDS_COLCONTEXTNAME), (TCHAR *) ModelName);
	pPrefsDlg->SetStringGadgetValue(_R(IDC_OPTS_AUTOCOLOUR), NameString, FALSE, Index);

	for (INT32 i = 0; i < MAX_COLOURMODELS; i++)
	{
		if (ColContexts.Context[i] != NULL)
		{
			ColContexts.Context[i]->GetModelName(&ModelName);

			NameString.MakeMsg(_R(IDS_COLCONTEXTNAME), (TCHAR *) ModelName);
			pPrefsDlg->SetStringGadgetValue(_R(IDC_OPTS_AUTOCOLOUR), NameString, FALSE, Index + 1);

			// If the colour model is equal to the entry model then note the index for this item. 
			if (i == (INT32)DisplayModel)
				*SelectedIndex = Index;
	
			Index++;
		}
	}		

	// Set the length of the list
	pPrefsDlg->SetComboListLength(_R(IDC_OPTS_AUTOCOLOUR));

	return TRUE;
}

/******************************************************************************************

>	BOOL ViewTab::InitSection()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/12/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if successful, else FALSE
	Purpose:	Sets initial values for the View section of the options dialog box.
				This section includes the:-
				Window 
				 - background redraw option
				 - proportional scroll bars options  
				Display
				 - Colour bar mode selector
				 - show system hourglass
				 - show progress bar    
	Errors:		-
	SeeAlso:	AppPrefsDlg; OptionsTabs;

******************************************************************************************/

BOOL ViewTab::InitSection()
{
TRACEUSER( "Neville", _T("ViewTab::InitSection\n"));
	ERROR2IF(pPrefsDlg == NULL,FALSE,"ViewTab::InitSection called with no dialog pointer");

	BOOL ReadOk = FALSE; 	// Flag to say whether the preference value was read ok 

	// Make sure the information field displaying the name of the current document
	// is correct.
	String_256	DocumentName(_R(IDT_OPTS_VIEW_INFO)); 
	DocumentName +=	*GetDocumentName();
	pPrefsDlg->SetStringGadgetValue(_R(IDC_OPTS_INFO), DocumentName);

	// Section = Display

	// Now the colour bar mode combo box
	INT32 ColourBarMode = 0; 
	ReadOk = Camelot.GetPrefValue(TEXT("Displays"), TEXT("ColourBarMode"), &ColourBarMode);
TRACEUSER( "Neville", _T("set colour bar mode '%d'\n"),ColourBarMode);
	// Set up the list of available options
	pPrefsDlg->DeleteAllValues(_R(IDC_OPTS_COLOURBAR));
	pPrefsDlg->SetStringGadgetValue(_R(IDC_OPTS_COLOURBAR), String_32(_R(IDN_COLBAR_SMALL)));
	pPrefsDlg->SetStringGadgetValue(_R(IDC_OPTS_COLOURBAR), String_32(_R(IDN_COLBAR_MEDIUM)));
	pPrefsDlg->SetStringGadgetValue(_R(IDC_OPTS_COLOURBAR), String_32(_R(IDN_COLBAR_MEDSCROLL)));
	pPrefsDlg->SetStringGadgetValue(_R(IDC_OPTS_COLOURBAR), String_32(_R(IDN_COLBAR_LARGE)));
	pPrefsDlg->SetComboListLength(_R(IDC_OPTS_COLOURBAR));
	// Set up the default option displayed
	BOOL ok = pPrefsDlg->SetSelectedValueIndex(_R(IDC_OPTS_COLOURBAR),ColourBarMode);
	ERROR2IF(!ReadOk,FALSE,_R(IDE_OPTS_READPREF_VIEW));

	
	// Now the colour editor model selection
	BOOL ColourEditorAutoModel = FALSE; 
	ColourModel DisplayModel = COLOURMODEL_HSVT;
	INT32 DisplayModelPref = (INT32)DisplayModel;
	INT32 SelectedIndex = 0;

PORTNOTE("other", "Disabled colour editor model preference reading")
#ifndef EXCLUDE_FROM_XARALX
	// First check if automatic is set.
	ReadOk = Camelot.GetPrefValue(TEXT("Displays"), TEXT("ColourEditorAutoModel"), &ColourEditorAutoModel);
TRACEUSER( "Neville", _T("set colour editor auto model mode '%d'\n"),ColourEditorAutoModel);
	// Now get the display model preference 
	ReadOk = ReadOk && Camelot.GetPrefValue(TEXT("Displays"), TEXT("ColourEditorDisplayModel"), &DisplayModelPref);
TRACEUSER( "Neville", _T("set colour editor model '%d'\n"),DisplayModelPref);
#endif
	DisplayModel = (ColourModel)DisplayModelPref;
	// Set up the list, passing in the default colour model, returning an index into the list
	// of available colour models
	InitAutoColourModelList(DisplayModel, &SelectedIndex);	
TRACEUSER( "Neville", _T("set colour editor list item '%d'\n"),SelectedIndex);
	if (ColourEditorAutoModel != 0)
	{
		// Automatic colour model selection so select the first in the list
		pPrefsDlg->SetSelectedValueIndex(_R(IDC_OPTS_AUTOCOLOUR), 0);
	}
	else
	{
		// Not automatic so select item in list taking into account extra automatic option
		// which is the first in the list.
		pPrefsDlg->SetSelectedValueIndex(_R(IDC_OPTS_AUTOCOLOUR), SelectedIndex + 1);
	}
	ERROR2IF(!ReadOk,FALSE,_R(IDE_OPTS_READPREF_VIEW));
	
	// Automatic colour line, gallery scroll to colour selected
	BOOL ColourAutoScroll = FALSE;
	ReadOk = Camelot.GetPrefValue(TEXT("Displays"), TEXT("AutoScrollColours"), &ColourAutoScroll);
	if (ColourAutoScroll != 0)
		pPrefsDlg->SetLongGadgetValue(_R(IDC_OPTS_AUTOSCROLL), TRUE);
	else
		pPrefsDlg->SetLongGadgetValue(_R(IDC_OPTS_AUTOSCROLL), FALSE);
	ERROR2IF(!ReadOk,FALSE,_R(IDE_OPTS_READPREF_VIEW));


	// Now set up the other buttons according to the values set in the variables
// Removed 18/12/95 as non longer required
//	BOOL HourGlass = FALSE;
//	ReadOk = Camelot.GetPrefValue(TEXT("Displays"), TEXT("Hourglass"), &HourGlass);
//	if (HourGlass != 0)
//		pPrefsDlg->SetLongGadgetValue(_R(IDC_OPTS_HOURGLASS), TRUE);
//	else
//		pPrefsDlg->SetLongGadgetValue(_R(IDC_OPTS_HOURGLASS), FALSE);
//	ERROR2IF(!ReadOk,FALSE,_R(IDE_OPTS_READPREF_VIEW));

PORTNOTE("other", "Disabled progress bar reading")
#ifndef EXCLUDE_FROM_XARALX
	BOOL ProgressBar = FALSE;
	ReadOk = Camelot.GetPrefValue(TEXT("Displays"), TEXT("ProgressBar"), &ProgressBar);
	if (ProgressBar != 0)
		pPrefsDlg->SetLongGadgetValue(_R(IDC_OPTS_PROGRESS), TRUE);
	else
		pPrefsDlg->SetLongGadgetValue(_R(IDC_OPTS_PROGRESS), FALSE);
	ERROR2IF(!ReadOk,FALSE,_R(IDE_OPTS_READPREF_VIEW));
#endif

	// Section = Bitmaps
	BOOL bSmoothing = TRUE;
	if (pDocument)
		bSmoothing = pDocument->GetBitmapSmoothing();
	pPrefsDlg->SetBoolGadgetSelected(_R(IDC_OPTS_SMOOTH_BITMAPS), !bSmoothing, 0 );

	INT32 m_compoundConvertToEditableShapesDPI;
	ReadOk = Camelot.GetPrefValue(TEXT("Displays"), TEXT("CompCToEDPI"), &m_compoundConvertToEditableShapesDPI);
	pPrefsDlg->SetLongGadgetValue(_R(IDC_OPTS_COMPOUNDDPI), m_compoundConvertToEditableShapesDPI);
	ERROR2IF(!ReadOk,FALSE,_R(IDE_OPTS_READPREF_VIEW));


	// Section = Window

// Removed 29/8/95 as transparency prompt dead
//	BOOL AutoTransparency = FALSE;
//	ReadOk = Camelot.GetPrefValue(TEXT("Rendering"), TEXT("AutoTransparency"), &AutoTransparency);
//	if (AutoTransparency != 0)
//		pPrefsDlg->SetLongGadgetValue(_R(IDC_OPTS_AUTOTRANS), TRUE);
//	else
//		pPrefsDlg->SetLongGadgetValue(_R(IDC_OPTS_AUTOTRANS), FALSE);
//	ERROR2IF(!ReadOk,FALSE,_R(IDE_OPTS_READPREF_VIEW));

	BOOL InteractiveFill = FALSE;
	ReadOk = Camelot.GetPrefValue(TEXT("Dragging"), TEXT("InteractiveFillEditing"), &InteractiveFill);
	if (InteractiveFill != 0)
		pPrefsDlg->SetLongGadgetValue(_R(IDC_OPTS_INTERACTIVEFILL), TRUE);
	else
		pPrefsDlg->SetLongGadgetValue(_R(IDC_OPTS_INTERACTIVEFILL), FALSE);
	ERROR2IF(!ReadOk,FALSE,_R(IDE_OPTS_READPREF_VIEW));
	
	INT32 ViewDither = 2;
	ReadOk = Camelot.GetPrefValue(_T("Screen"), _T("ViewDither"), &ViewDither);
	SetSelectedDither(ViewDither);
	ERROR2IF(!ReadOk,FALSE,_R(IDE_OPTS_READPREF_VIEW));

	// Set up the attributes which are document/view specific
	ok = GetDisplayStyleForView();

	return TRUE;
}


// Array of control IDs for the Get/SetSelectedDither functions

static CGadgetID DitherGadgets[] = {_R(IDC_OPTS_DITHER_NONE),
									_R(IDC_OPTS_DITHER_ORDERED),
									_R(IDC_OPTS_DITHER_DIFFUSED),
									0};



/********************************************************************************************

>	INT32 ViewTab::GetSelectedDither(void)


	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/5/96
	Inputs:		-
	Outputs:	-
	Returns:	The GDraw dither style specified by the controls
	Purpose:	Converts the state of the radio button controls to a dither style
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

INT32 ViewTab::GetSelectedDither(void)
{
	INT32 DitherStyle = 2;	// Default to ordered dither

	CGadgetID Gadget = pPrefsDlg->GetRadioGroupSelected(DitherGadgets);

	if (Gadget ==  _R(IDC_OPTS_DITHER_NONE))
		DitherStyle = 4;
	else if (Gadget == _R(IDC_OPTS_DITHER_ORDERED))
		DitherStyle = 2;
	else if (Gadget == _R(IDC_OPTS_DITHER_DIFFUSED))
		DitherStyle = 3;

	return(DitherStyle);
}


 
/********************************************************************************************

>	void ViewTab::SetSelectedDither(INT32 DitherStyle)


	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/5/96
	Inputs:		DitherStyle - the GDraw dither style
	Outputs:	-
	Returns:	TRUE if it succeeded
	Purpose:	Sets the radio buttons to the correct state for the supplied dither style
	Errors:		-
	SeeAlso:	DialogOp::SetRadioGroupSelected()

********************************************************************************************/

BOOL ViewTab::SetSelectedDither(INT32 DitherStyle)
{
	CGadgetID Gadget = _R(IDC_OPTS_DITHER_ORDERED);

	switch(DitherStyle)
	{
		case 4:
			Gadget = _R(IDC_OPTS_DITHER_NONE);
			break;

		case 1:
		case 2:
			Gadget = _R(IDC_OPTS_DITHER_ORDERED);
			break;

		case 0:
		case 3:
			Gadget = _R(IDC_OPTS_DITHER_DIFFUSED);
			break;

		default:
			break;
	}

	return(pPrefsDlg->SetRadioGroupSelected(DitherGadgets, Gadget));
}


#endif //webster
