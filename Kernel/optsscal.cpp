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
// Implementation of the Scale/Dimension tab of the options dialog box

/*

*/

#include "camtypes.h"
//WEBSTER-ranbirr-13/11/96
#ifndef WEBSTER
#include "appprefs.h"
//#include "prefsdlg.h"	// dialog/gadget ids
//#include "optsres.h"	// error strings
#include "optsscal.h"
#include "optsmsgs.h"
//#include "app.h"		// Camelot object
#include "docview.h"
#include "spread.h"
//#include "nev.h"
#include "document.h"	// document object

CC_IMPLEMENT_DYNAMIC(ScaleTab, OptionsTabs)   
			  
// This is not compulsory, but you may as well put it in so that the correct version
// of your file can be registered in the .exe
DECLARE_SOURCE("$Revision$");

/********************************************************************************************

>	ScaleTab::ScaleTab() 


	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/1/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	ScaleTab constructor. Creates a non-Scaleable operation
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

ScaleTab::ScaleTab()
{   
	pDimScale = NULL;
}        

/********************************************************************************************

>	ScaleTab::~ScaleTab()


	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/1/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	ScaleTab destructor
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
																				
ScaleTab::~ScaleTab()
{   
}        

/********************************************************************************************

>	BOOL ScaleTab::Init()


	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/1/95
	Inputs:		-
	Outputs:	-
	Returns:	True if initialised ok, False if failed.
	Purpose:	ScaleTab initialisation routine
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
																				
BOOL ScaleTab::Init()
{   
	return TRUE;
}        


/******************************************************************************************

>	CDlgResID ScaleTab::GetPageID()()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/1/95
	Inputs:		-
	Outputs:	-
	Returns:	The dialog ID of this tab section.	  
	Purpose:	Allows the options dialog code to determine the dialog ID of this section.
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

CDlgResID ScaleTab::GetPageID()
{
	return _R(IDD_OPTSTAB_SCALE);
}

/******************************************************************************************

>	BOOL ScaleTab::IsDocumentOption()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/1/95
	Inputs:		-
	Outputs:	-
	Returns:	Returns True if this tab is a document option.	  
	Purpose:	Allows the document options dialog code to determine if this tab belongs
				to the group of document options.
	Errors:		-
	SeeAlso:	IsProgramOption();

******************************************************************************************/

BOOL ScaleTab::IsDocumentOption()
{
	return TRUE;	// This tab is not a document option 
}

/******************************************************************************************

>	BOOL ScaleTab::IsProgramOption()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/1/95
	Inputs:		-
	Outputs:	-
	Returns:	Returns True if this tab is a program option.	  
	Purpose:	Allows the program options dialog code to determine if this tab belongs
				to the group of program options.
	Errors:		-
	SeeAlso:	IsProgramOption();

******************************************************************************************/

BOOL ScaleTab::IsProgramOption()
{
	return TRUE;	// This tab is a program option 
}


/******************************************************************************************

>	BOOL ScaleTab::CommitSection()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/1/95
	Inputs:		-
	Outputs:	-
	Returns:	True if values in dialog box ok, False otherwise.	  
	Purpose:	Takes the values in the Scale section tab of the options dialog box and
				sets the associated	preference values accordingly
				Called when ok is pressed on the dialog box.
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

BOOL ScaleTab::CommitSection()
{
TRACEUSER( "Neville", _T("commit Scale section\n"));
	ERROR3IF(pPrefsDlg == NULL, "ScaleTab::CommitSection called with no dialog pointer");

	BOOL ok = pPrefsDlg->TalkToPage(_R(IDD_OPTSTAB_SCALE));	// The Scale tab identifier
	if (!ok)
		return TRUE;	// Talk to View failed to return now

	// Ok has been pressed so take the values from this section of the dialog box
	// Takes the values in the dialog and sets the DimScale object accordingly.
	BOOL Valid=TRUE;		// Flag for validity of value
	BOOL State=FALSE;		// Flag for state of button/switch
	BOOL SetOk=TRUE;		// Preference value set ok

	// Section = Scale settings

	// Now check that we have the selected view still, just in case it has switched
	// without us being told about it or even we have no current document/view.
	// This may be a valid state now, so do not complain about it. 
	DocView* pCurrentView = DocView::GetSelected();
	if (pCurrentView != NULL)
	{
		// Only if there is a current view do we read the values.
		String_256 DrawingStr;
		String_256 RealStr;
		//TCHAR* pDrawingStr = DrawingStr;
		//TCHAR* pRealStr = RealStr;
		BOOL Active;

		// Get the values from the dialog box
		Active 		= pPrefsDlg->GetLongGadgetValue(_R(IDC_OPTS_USESCALEFACTOR),0,1,0, &Valid);
		DrawingStr  = pPrefsDlg->GetStringGadgetValue(_R(IDC_OPTS_DRAWINGSCALE), &Valid);
		RealStr 	= pPrefsDlg->GetStringGadgetValue(_R(IDC_OPTS_REALSCALE), &Valid);

		Spread* pSpread = pCurrentView->GetFirstSelectedSpread();
		// If no selected spread then use the visible spread
		if (pSpread == NULL)
			pSpread = pCurrentView->GetVisibleSpread();

		// Only do the chnage if we have a valid spread pointer and we have changed something.
		if ( (pSpread != NULL) &&
			 (
			 	(OldActiveState != Active) ||
			 	(OldDrawingStr != DrawingStr) ||
			 	(OldRealStr != RealStr)
			 )
		   )
		{
			pDimScale = pSpread->GetPtrDimScale();
			if (pDimScale != NULL)
			{
				// Only if active is set do we need to try and set new strings
				// and hence a new drawing scale
				if (Active)
				{
					// Dim Scales can only be 32 characters long
					String_32 DrawingStr32 = "";
					String_32 RealStr32 = "";
					// Check if read in strings are longer than this
					if (DrawingStr.Length() > DrawingStr32.MaxLength())
					{
						InformError(_R(IDE_OPTS_INVALIDDRAWSCALE));
						return FALSE;
					}
					if (RealStr.Length() > RealStr32.MaxLength())
					{
						InformError(_R(IDE_OPTS_INVALIDREALSCALE));
						return FALSE;
					}

					RealStr32 = RealStr;
					DrawingStr32 = DrawingStr;

					// Try and set these strings as new drawing and real scales strings
					SetOk = pDimScale->SetDrawingScaleStr(DrawingStr32);
					if (!SetOk)
					{
						InformError(_R(IDE_OPTS_INVALIDDRAWSCALE));
						return FALSE;
					}

					SetOk = SetOk && pDimScale->SetRealScaleStr(RealStr32);
					if (!SetOk)
					{
						InformError(_R(IDE_OPTS_INVALIDREALSCALE));
						return FALSE;
					}
				
					// Now try to convert these into a new scaling factor
					if (SetOk)
						SetOk = SetOk && pDimScale->SetScaleFactor();
				
					// If we failed in any of the conversions then warn the user and fail
					if (!SetOk)
					{
						InformError(_R(IDE_OPTS_INVALIDSCALING));
						return FALSE;
					}
				}
				
				// Set up a possibly new active state
				pDimScale->SetActiveState(Active);

			}

			// Now tell other users of units/scaling factors that there they are likely
			// to need to update any currently displayed units. 
			Document *pCurrentDoc = (Document *)pSpread->FindOwnerDoc();
			BROADCAST_TO_ALL(OptionsChangingMsg(pCurrentDoc, OptionsChangingMsg::OptionsState::NEWUNITS));
// unfortunately pScopeDoc is NULL!, and since pCurrentDoc is calulated I thought it ought to use! - Ed 17/10/95
//			BROADCAST_TO_ALL(OptionsChangingMsg(pScopeDocument, OptionsChangingMsg::OptionsState::NEWUNITS));

			// Make sure the document is marked as modified.
			pCurrentDoc->SetModified(TRUE);
			
			// And note the new states
			OldActiveState = Active;
			OldDrawingStr = DrawingStr;
			OldRealStr = RealStr;
		}
	}
//	else
//		ERROR2(FALSE,_R(IDE_OPTS_READPREF_SCALE));
	
	return TRUE;
}

/********************************************************************************************

>	BOOL ScaleTab::GreySection()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/2/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Called when the user has closed all documents 
	Errors:		-
	SeeAlso:	ScaleTab::UngreySection;

********************************************************************************************/

BOOL ScaleTab::GreySection()
{
TRACEUSER( "Neville", _T("GreySection in ScaleTab section\n"));
	ERROR2IF(pPrefsDlg == NULL,FALSE,"ScaleTab::GreySection called with no dialog pointer");

	BOOL ok = pPrefsDlg->TalkToPage(_R(IDD_OPTSTAB_SCALE));	// The ScaleTab identifier
	if (!ok)
		return TRUE;	// Talk to page failed to return now

	// Make sure the information field displaying the name of the current document
	// is correct.
	String_256	DocumentName(_R(IDT_OPTS_SCALING_INFO)); 
	DocumentName +=	*GetDocumentName();
	pPrefsDlg->SetStringGadgetValue(_R(IDC_OPTS_INFO), &DocumentName);

	// Only update if we are not already grey 
	if (GreyStatus == TRUE)
		return TRUE;

	// Call our central greying/ungreying function
	ok = ChangeControlStatus(FALSE);
	
	GreyStatus = TRUE;

	return ok;
} 

/********************************************************************************************

>	BOOL ScaleTab::UngreySection()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/2/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Called when the user has selected a new document 
	Errors:		-
	SeeAlso:	ScaleTab::UngreySection;

********************************************************************************************/

BOOL ScaleTab::UngreySection()
{
TRACEUSER( "Neville", _T("UngreySection in ScaleTab section\n"));
	ERROR2IF(pPrefsDlg == NULL,FALSE,"ScaleTab::UngreySection called with no dialog pointer");

	BOOL ok = pPrefsDlg->TalkToPage(_R(IDD_OPTSTAB_SCALE));	// The ScaleTab identifier
	if (!ok)
		return TRUE;	// Talk to page failed to return now

	// Make sure the information field displaying the name of the current document
	// is correct.
	String_256	DocumentName(_R(IDT_OPTS_SCALING_INFO)); 
	DocumentName +=	*GetDocumentName();
	pPrefsDlg->SetStringGadgetValue(_R(IDC_OPTS_INFO), &DocumentName);

	// Only update if we are not already ungrey 
	if (GreyStatus == FALSE)
		return TRUE;

	// Call our central greying/ungreying function
	ok = ChangeControlStatus(TRUE);
	
	GreyStatus = FALSE;

	return ok;
}

/********************************************************************************************

>	BOOL ScaleTab::ChangeControlStatus(const BOOL Status)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/2/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Called to grey/ungrey all controls on this tab. 
	Errors:		-
	SeeAlso:	ScaleTab::GreySection;	ScaleTab::UngreySection;

********************************************************************************************/

BOOL ScaleTab::ChangeControlStatus(const BOOL Status)
{
	// Grey/ungrey the information fields
	pPrefsDlg->EnableGadget(_R(IDC_OPTS_INFO), Status);

	// Grey/ungrey the controlling Switch
	pPrefsDlg->EnableGadget(_R(IDC_OPTS_USESCALEFACTOR), Status);

	// Grey/ungrey the display fields 
	pPrefsDlg->EnableGadget(_R(IDC_OPTS_DRAWINGSCALE), Status);
	pPrefsDlg->EnableGadget(_R(IDC_OPTS_REALSCALE), Status);

	// Grey/ungrey the static text fields accordingly 
	pPrefsDlg->EnableGadget(_R(IDC_OPTS_SCALEGROUP), Status);
	pPrefsDlg->EnableGadget(_R(IDC_OPTS_SCALEEG), Status);
	pPrefsDlg->EnableGadget(_R(IDC_OPTS_REALWORLD), Status);
	pPrefsDlg->EnableGadget(_R(IDC_OPTS_DRAWING), Status);
	pPrefsDlg->EnableGadget(_R(IDC_OPTS_SCALETO), Status);
	pPrefsDlg->EnableGadget(_R(IDC_OPTS_SCALETO2), Status);

	return TRUE;
}			

/********************************************************************************************

>	BOOL ScaleTab::UpdateSection(String_256 *DocumentName)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/2/95
	Inputs:		Name of document
	Outputs:	-
	Returns:	-
	Purpose:	Called when we have switched to a new document and need to update all the 
				controls on this tab. Should only init the tab if the page is present.  
				The document name allows the info field on the tab to be filled in correctly.
	Errors:		-
	SeeAlso:	ScaleTab::GreySection; ScaleTab::UngreySection; ScaleTab::InitSection;

********************************************************************************************/

BOOL ScaleTab::UpdateSection(String_256 *DocumentName)
{
TRACEUSER( "Neville", _T("ScaleTab::UpdateSection\n"));
	ERROR2IF(pPrefsDlg == NULL,FALSE,"ScaleTab::UpdateSection called with no dialog pointer");

	BOOL ok = pPrefsDlg->TalkToPage(_R(IDD_OPTSTAB_SCALE));	// The ScaleTab identifier
	if (!ok)
		return TRUE;		// page not present

	// Remove any lists that we have created.
	//pPrefsDlg->DeleteAllValues(_R(IDC_OPTS_SIZELIST));

	// Call the normal init section code
	return InitSection();
}

/********************************************************************************************

>	BOOL ScaleTab::HandleMsg(DialogMsg* Msg)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/1/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Handles all the Scale options tabs messages 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL ScaleTab::HandleMsg(DialogMsg* Msg)
{
TRACEUSER( "Neville", _T("HandleScaleMsg\n"));
	ERROR2IF(Msg == NULL,FALSE,"ScaleTab::Message null message received");
	ERROR2IF(pPrefsDlg == NULL,FALSE,"ScaleTab::HandleMsg called with no dialog pointer");

	BOOL ok = pPrefsDlg->TalkToPage(_R(IDD_OPTSTAB_SCALE));	// The Scale tab identifier
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
			switch (Msg->GadgetID)
			{
				case _R(IDC_OPTS_USESCALEFACTOR):
					{
						BOOL Valid;
						BOOL Active = pPrefsDlg->GetLongGadgetValue(_R(IDC_OPTS_USESCALEFACTOR),0,1,0,&Valid);
						// Grey/ungrey the editable fields accordingly 
						pPrefsDlg->EnableGadget(_R(IDC_OPTS_DRAWINGSCALE), Active);
						pPrefsDlg->EnableGadget(_R(IDC_OPTS_REALSCALE), Active);
						// Grey/ungrey the static text fields accordingly 
						pPrefsDlg->EnableGadget(_R(IDC_OPTS_SCALEEG), Active);
						pPrefsDlg->EnableGadget(_R(IDC_OPTS_REALWORLD), Active);
						pPrefsDlg->EnableGadget(_R(IDC_OPTS_DRAWING), Active);
						pPrefsDlg->EnableGadget(_R(IDC_OPTS_SCALETO), Active);
						pPrefsDlg->EnableGadget(_R(IDC_OPTS_SCALETO2), Active);
						pPrefsDlg->EnableGadget(_R(IDC_OPTS_SCALEGROUP), Active);
					}
					break;						
			}
			break; 
		case DIM_SELECTION_CHANGED:
		case DIM_TEXT_CHANGED:
			OptionsTabs::SetApplyNowState(TRUE);
			break;
	}

	return TRUE;
}  


/******************************************************************************************

>	BOOL ScaleTab::InitSection()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/1/95
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if successful, else FALSE
	Purpose:	Sets initial values for the Scale section of the options dialog box.
				This section includes the:-
	Errors:		-
	SeeAlso:	AppPrefsDlg; OptionsTabs;

******************************************************************************************/

BOOL ScaleTab::InitSection()
{
TRACEUSER( "Neville", _T("ScaleTab::InitSection\n"));
	ERROR2IF(pPrefsDlg == NULL,FALSE,"ScaleTab::InitSection called with no dialog pointer");

	BOOL ReadOk = FALSE; 	// Flag to say whether the preference value was read ok 

	// Make sure the information field displaying the name of the current document
	// is correct.
	String_256	DocumentName(_R(IDT_OPTS_SCALING_INFO)); 
	DocumentName +=	*GetDocumentName();
	pPrefsDlg->SetStringGadgetValue(_R(IDC_OPTS_INFO), &DocumentName);

	// Section = Scale settings

	DocView* pSelectedView = DocView::GetSelected();
	// This may now be a valid state so must not error
	//ERROR3IF(pSelectedView == NULL,"ScaleTab::InitSection Where's the current view eh?");
	if (pSelectedView != NULL)
	{
		Spread* pSpread = pSelectedView->GetFirstSelectedSpread();

		// If no selected spread then use the visible spread
		if (pSpread == NULL)
			pSpread = pSelectedView->GetVisibleSpread();

		// If no selected spread then use the first spread
		// Of course, this should not be here but above routines seem to return
		// null a bit too often for my liking
		if (pSpread == NULL)
		{
TRACEUSER( "Neville", _T("ScaleTab::InitSection BODGE! using 1st spread\n"));
			Document* pSelectedDoc = Document::GetSelected();
			if (pSelectedDoc !=NULL )
				pSpread = pSelectedDoc->FindFirstSpread();
		}

		// Go and get a pointer to the scaling values
		if (pSpread != NULL)
		{
			pDimScale = pSpread->GetPtrDimScale();
			if (pDimScale != NULL)
			{
				// And now show the initial state of the controls given this
				// scaling 
				ShowScaleDetails();
			}
		}
	}
	else
	{
		// If no current view then ensure section is greyed
		GreySection();
	}

	return TRUE;
}

/********************************************************************************************

>	void ScaleTab::ShowScaleDetails()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/6/94
	Inputs:		- 
	Outputs:	-
	Returns:	-
	Purpose:	Shows scale factor details in the dialog
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL ScaleTab::ShowScaleDetails()
{
TRACEUSER( "Neville", _T("ScaleTab::ShowScaleDetails\n"));
	ERROR2IF(pPrefsDlg == NULL,FALSE,"ScaleTab::ShowScaleDetails called with no dialog pointer");
	ERROR2IF(pDimScale == NULL,FALSE,"ScaleTab::ShowScaleDetails pDimScale is NULL");
	
	String_32 Str;

	// Get current state of dimension scaling to see whether it is active or
	// noton this view. 
	BOOL Active = pDimScale->IsActive();
	pPrefsDlg->SetLongGadgetValue(_R(IDC_OPTS_USESCALEFACTOR), (INT32)Active);
	// remember entry states in our class variables
	OldActiveState = Active;

	Str = pDimScale->GetDrawingScaleStr();
	pPrefsDlg->SetStringGadgetValue(_R(IDC_OPTS_DRAWINGSCALE), &Str);
	pPrefsDlg->EnableGadget(_R(IDC_OPTS_DRAWINGSCALE), Active);
	// remember entry states in our class variables
	OldDrawingStr = Str;

	Str = pDimScale->GetRealScaleStr();
	pPrefsDlg->SetStringGadgetValue(_R(IDC_OPTS_REALSCALE), &Str);
	pPrefsDlg->EnableGadget(_R(IDC_OPTS_REALSCALE), Active);
	// remember entry states in our class variables
	OldRealStr = Str;

	// Grey/ungrey the static text fields accordingly 
	pPrefsDlg->EnableGadget(_R(IDC_OPTS_SCALEEG), Active);
	pPrefsDlg->EnableGadget(_R(IDC_OPTS_REALWORLD), Active);
	pPrefsDlg->EnableGadget(_R(IDC_OPTS_DRAWING), Active);
	pPrefsDlg->EnableGadget(_R(IDC_OPTS_SCALETO), Active);
	pPrefsDlg->EnableGadget(_R(IDC_OPTS_SCALETO2), Active);
	pPrefsDlg->EnableGadget(_R(IDC_OPTS_SCALEGROUP), Active);
	
	return TRUE;
}

#endif //webster
