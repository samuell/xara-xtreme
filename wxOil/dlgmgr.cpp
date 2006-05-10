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

// Implementation of the DialogManager class (bodge central)

// Include files
#include "camtypes.h"

#include "dlgmgr.h"
#include "dialogop.h"
//#include "simon.h"
#include "fixst256.h"
#include "fixstr8.h"
#include "list.h"
#include "ensure.h"
#include "errors.h"
#include "infobar.h"
#include "fixmem.h"
#include "msg.h"
#include "bars.h"
//#include "dlgbar.h"
//#include "cheklist.h"
//#include "custmsg.h"
//#include "sgallery.h"
//#include "galbar.h"
#include "document.h"
#include "docview.h"
#include "ccdc.h"
//#include "bitbutn.h"
//#include "bitc1ded.h"
//#include "fonts.h"
//#include "oilprog.h"	// beep()
//#include "ctrlhelp.h"
#include "camelot.h"
#include "camframe.h"
//#include "palman.h"
#include "stack.h"
//#include "dropdown.h"	// Colour/Font dropdown combo box support
#include "unicdman.h"
#include "appprefs.h"
#include "helpuser.h"
//#include "textres.h"	// required so we know what an _R(IDC_FONT_COMBO) is.
//#include "fontdrop.h"	// required so we know what a FontDropItem is.
//#include "brdlgres.h"
//#include "dlgcthlp.h"
//#include "customlist.h"
//#include "customedit.h"
#include "dlgevt.h"
#include "cartprov.h"
#include "cartctl.h"
#include "osrndrgn.h"
#include "dlgtypes.h"
#include "statline.h"

DECLARE_SOURCE("$Revision$");

CC_IMPLEMENT_DYNAMIC(CGadgetImageList, CCObject);

IMPLEMENT_CLASS(OurPropSheet, wxPropertySheetDialog);


// Place all statics here please, ordered by class
// Statics

// DialogManager statics
List DialogManager::DiscardStrList;
List DialogManager::ScrollPageIncList;
List DialogManager::DialogPositionList;
List DialogManager::DlgTagOpToPropShtList;

wxWindow   *DialogManager::pDlgCurrent = NULL;   // Required for IsDialogMessage handling

// The ActiveDialogStack is used to restore previously active dialogs after a Modal dialog
// is closed.
ActiveDlgStateStack DialogManager::ActiveDlgStack;

// When the user clicks with the right mouse button on a dual function button BN_RGT_CLICKED
// is returned as the notification code.
#define BN_RGT_CLICKED 6


class Node;

/********************************************************************************************

>	DialogManager::DialogManager()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/12/93
	Purpose:	DialogManager constructor. It allocates our special Property atom.

********************************************************************************************/

DialogManager::DialogManager()
{
	// we must use a unique string so we don't clash with anyone else
}

/********************************************************************************************

>	BOOL		DialogManager::Create(DialogOp* DlgOp,
									HINSTANCE MainInstance, CDlgResID MainDlgID,
									HINSTANCE SubInstance,  CDlgResID SubDlgID,
									CDlgMode Mode, INT32 OpeningPage, CWindowID ParentWnd)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/8/93

	Inputs:		DlgOp:			The DialogOp we are creating a window for

			    The following inputs may soon become defunct

				MainInstance:	Instance handle of the module that contains the dialog
								defined by MainDlgID.
				MainDlgID:		Resource identifier of the main dialog box
				SubInstance:	Instance handle of the module that contains the dialog
								defined by SubDlgID.
				SubDlgID:		Resource identifier of the secondary dialog box to merge
								with the main one (0 if none).
				Mode:			Dialog mode (Modal, Modeless)
				OpeningPage:	Index of the tabbed page which we need to open (0 if none).

	Returns:	TRUE if the Dialog/Bar could be created, else FALSE

	Purpose:	The create method creates a Dialog box and positions it

				If the Dialog is Modal then the dialog is displayed, to initialise the dialog
				you must respond to the DIM_CREATE message.

				If the Dialog is Modeless then the Open method needs to be called to make the
				dialog visible.

				If the dialog has not been created before then it is positioned centrally on
				the screen. Otherwise the dialog's position is restored to the position it
				was at the last time it was deleted.

				If SubDlgID is non-0, then this dialog is merged with the main one during
				the creation of the dialog.  If it is 0, then no merging is done (the
				DialogOp::Create() function should take care of all this), and SubInstance
				is ignored.

				If it is a tabbed dialog that is being created then we can now specify the
				opening tab. Usually, this will be the first one if this box has not been
				opened before or the one selected when it was closed. This parameter allows
				this to be overriden.

				Note that this function sets the DialogOps window ID


				Note: It is important that all dialogs do not have the Visible property set.

	Errors:		An Error will be set if this function fails

	SeeAlso:	DialogOp::Create



********************************************************************************************/

BOOL DialogManager::Create(DialogOp* DlgOp,
						   /* HINSTANCE MainInstance, */ CDlgResID MainDlgID,
						   /* HINSTANCE SubInstance, */  CDlgResID SubDlgID,
						   CDlgMode Mode, INT32 OpeningPage, CWindowID ParentWnd)
{
	ERROR2IF(!DlgOp, FALSE, _T("Create Passed Null DialogOp"));
	ERROR2IF(DlgOp->pEvtHandler, FALSE, _T("Window has already been created. Having two is greedy"));

	DlgOp->pEvtHandler = new DialogEventHandler(DlgOp);
	ERRORIF(!DlgOp->pEvtHandler || !DlgOp->pEvtHandler->pDialogOp, FALSE, _R(IDE_CANNOT_CREATE_DIALOG));

	BOOL wxAUImanaged = FALSE;
	if ( DlgOp->IsABar() || DlgOp->IsAGallery() )
	{
		BOOL modal = DlgOp->IsModal();
		ERROR2IF(modal, FALSE, "Attempting to create a wxAUImanaged Dialog that is modal");
		// They wanted a bar. Well, the main difference to us is we let wxAUI manage it.
		wxAUImanaged = TRUE;
	}

//	ERROR2IF( DlgOp->IS_KIND_OF(DialogBarOp), FALSE, _T("Bar creation not yet supported"));
//	ERROR2IF( DlgOp->IS_KIND_OF(DialogTabOp), FALSE, _T("Tabbed dialogs not yet supported"));
	ERROR2IF( SubDlgID !=0, FALSE, _T("Merging of dialogs not yet supported"));

    // if no parent dialog window specified use the main frame window
	if ((ParentWnd == NULL) || wxAUImanaged)
		ParentWnd = GetMainFrame();

	const TCHAR*	pDialogName = NULL;
	wxWindow*		pDialogWnd = NULL;

	if( DlgOp->IS_KIND_OF(DialogTabOp) )
		pDialogWnd = CreateTabbedDialog( (DialogTabOp*)DlgOp, Mode, OpeningPage );
	else
	{
		pDialogName=CamResource::GetObjectNameFail(MainDlgID);
		ERROR1IF(pDialogName == NULL, FALSE, _R(IDE_CANNOT_CREATE_DIALOG));

		if (wxAUImanaged)
			pDialogWnd = wxXmlResource::Get()->LoadPanel((wxWindow *)ParentWnd, pDialogName);
		else
			pDialogWnd = wxXmlResource::Get()->LoadDialog((wxWindow *)ParentWnd, pDialogName);
	}

	CreateRecursor(pDialogWnd);

	ERROR1IF(pDialogWnd == NULL, FALSE, _R(IDE_CANNOT_CREATE_DIALOG));
	pDialogWnd->Hide();
	CamArtProvider::Get()->EnsureChildBitmapsLoaded(pDialogWnd);

	// On the Mac, panels etc. are by default transparent; fix them up
#ifdef __WXMAC__
	pDialogWnd->SetBackgroundStyle(wxBG_STYLE_COLOUR);
	pDialogWnd->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
#endif

	// Note that we might one day want to create (say) wxPanels, or wxToolbars instead above
	// It deosn't matter to us, we just want a wxWindow

	DlgOp->pEvtHandler->pwxWindow = pDialogWnd;
	DlgOp->pEvtHandler->wxAUImanaged = wxAUImanaged;
	DlgOp->pEvtHandler->ID =MainDlgID;
	// Set the DialogOp's WindowID
	DlgOp->WindowID = (CWindowID)pDialogWnd;
	pDialogWnd->PushEventHandler(DlgOp->pEvtHandler);

	// Register all the child controls
	ControlList::Get()->RegisterWindowAndChildren(pDialogWnd, DlgOp);

	ControlList::Get()->ReflectAllStates(); // might as well do the processing before the bar / dialog appears

	// we call this directly now
	BOOL ok = PostCreate(DlgOp);

	if( ok && 
		Mode == MODAL && 
		pDialogWnd->IsKindOf( CLASSINFO(wxDialog) ) )
	{
		((wxDialog *) pDialogWnd)->ShowModal();
	}

#ifdef USE_WXAUI
	if (wxAUImanaged)
	{
		// We really should take a wxPaneInfo() as an additional parameter to this function to allow this sort
		// of stuff to be specified. Or try and retrieve it from the DialogBarOp or similar. Anyway, for now
		// give it some default parameters
		wxPaneInfo paneinfo;
		paneinfo.DestroyOnClose(FALSE);
		if (DlgOp->IsABar())
		{			
			if (DlgOp->IsKindOf(CC_RUNTIME_CLASS(StatusLine)))
				paneinfo.Bottom().Layer(1).Row(2).LeftDockable(FALSE).RightDockable(FALSE).Floatable(FALSE).Movable(FALSE).Gripper(FALSE).CaptionVisible(FALSE).PaneBorder(FALSE);
			else	
			{
				paneinfo.ToolbarPane().Fixed();
				if (DlgOp->IsVertical())
				{
					paneinfo.Left().Layer(0).GripperTop().TopDockable(FALSE).BottomDockable(FALSE);
				}
				else
				{
					paneinfo.Top().Layer(1).Row(2).LeftDockable(FALSE).RightDockable(FALSE);
				}
			}
		}
		else
		{
			paneinfo.Layer(3).GripperTop().TopDockable(FALSE).BottomDockable(FALSE).Float();
		}

		if (DlgOp->IsKindOf(CC_RUNTIME_CLASS(InformationBarOp)))
		{
			paneinfo.Floatable(FALSE);	// temporarilly do not allow Info Bars to float as they can be closed
										// which means they can't be reopened (no UI), and wxAUI rightly objects to the
										// tool switch that deletes them deleting the window.
		}

		wxString Title = pDialogWnd->GetTitle();
		if (Title.IsEmpty()) Title = pDialogWnd->GetLabel(); // because wxPanel doesn't seem to support a title
		if (Title.IsEmpty())
		{
			// Finally, in desperation, we (mis-)use the tooltip string because now the wx folks have removed
			// the label, even though it's needed for accessibility. Aarrghh
			wxToolTip* pTip = pDialogWnd->GetToolTip();
			if (pTip) Title=pTip->GetTip();
		}
		if (Title.IsEmpty())
			 Title = wxString(CamResource::GetText(_R(IDS_ANONYMOUSBARTITLE)));

		wxSizer * pSizer = pDialogWnd->GetSizer();
		if (pSizer)
		{
			pSizer->SetSizeHints(pDialogWnd);
			pDialogWnd->SetSizerAndFit(pSizer);
		}

		CCamFrame::GetFrameManager()->AddPane(pDialogWnd, paneinfo.
			Name(pDialogName).Caption(Title).
			PinButton(TRUE));

		CCamFrame::GetMainFrame()->UpdateFrameManager();
	}
#endif

	return ok;
}

/********************************************************************************************

>	static void DialogManager::CreateRecursor(wxWindow * pwxWindow)


	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	02/12/2005
	Inputs:		pWindow - pointer to window to process
	Outputs:	None
	Returns:	None
	Purpose:	Initialize platform dependent resources
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void DialogManager::CreateRecursor(wxWindow * pwxWindow)
{
	// Process this one
	wxPlatformDependent::Get()->InitWindow(pwxWindow);

	// Now process children if any
	wxWindowList::Node * pNode = pwxWindow->GetChildren().GetFirst();
	while (pNode)
	{
		CreateRecursor(pNode->GetData());
		pNode = pNode->GetNext();
	}
	return;
}


/********************************************************************************************

>	BOOL DialogManager::PostCreate(DialogOp * pDialogOp);

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/9/94
	Inputs:		DialogWnd: The dialogs window ID, NULL if dialog failed to be created
	Returns:	-
	Purpose:	This function will get called after a dialog has been created. If a modeless
				dialog has been created then it gets called directly from the Create method.
				For a modal dialog however it gets called after receiving a WM_INIT_DIALOG
				message. It completes the creation process.

				(for now on wxWindows we are simply calling it from Create)

	Scope:		private

********************************************************************************************/

BOOL DialogManager::PostCreate(DialogOp * pDialogOp)
{
	ERROR2IF( !pDialogOp || !pDialogOp->pEvtHandler || !pDialogOp->pEvtHandler->pwxWindow,
			   FALSE, _T("Bad DialogOp / EvtHandler in DialogManager::PostCreate()"));

	wxWindow * pDialogWnd = pDialogOp->pEvtHandler->pwxWindow;

	// If the dialog has been created before then its position will have to be reset
	INT32 DlgX=0; // Dialog box X position
	INT32 DlgY=0; // Dialog box Y position
	CDlgResID ActivePage=0; // Active page for tabbed dialogs

	BOOL CreatedBefore = FALSE; // TRUE if the dialog has been created before

	// Search the DialogPositionList to see if the dialog has been created before
	DialogPosition* DlgPos = FindDialogPositionRecord(pDialogOp->pEvtHandler->ID);
	if (DlgPos != NULL)
	{
		DlgX = DlgPos->LastX;
		DlgY = DlgPos->LastY;
		ActivePage = DlgPos->ActivePage;
		CreatedBefore = TRUE;
	}

	// Get the size of the dialog box (Required for the SetWindowPos function)
	wxRect	DialogRect( pDialogWnd->GetRect() );
	INT32	DialogWidth  = DialogRect.GetWidth();
	INT32	DialogHeight = DialogRect.GetHeight();

	OurPropSheet*	pPropSheet = NULL;
	{
		DlgTagOpToPropShtItem*	pItem = (DlgTagOpToPropShtItem*)DlgTagOpToPropShtList.GetHead();
		if( NULL != pItem &&
			pDialogOp->IS_KIND_OF(DialogTabOp) )
		{
			pPropSheet = pItem->pPropertySheet;
		}
	}

	// Create the WindowIDItem which will be stored in the DialogPosition.
	CWindowIDItem *pWinID = new CWindowIDItem;
	if( NULL == pWinID )
	{
		// We need to destroy the dialog window
		pDialogWnd->PopEventHandler(FALSE);
		pDialogOp->pEvtHandler->Destroy();
		pDialogWnd->Destroy();
		ERROR1(FALSE, _R(IDS_OUT_OF_MEMORY));
	}

	if (!CreatedBefore) // If this is the first time the dialog has been created then position
						// it centrally on the screen
	{
		// Get the size of the screen
		INT32			ScreenWidth  = wxSystemSettings::GetMetric( wxSYS_SCREEN_X );
		INT32			ScreenHeight = wxSystemSettings::GetMetric( wxSYS_SCREEN_Y );

		// Centre the dialog box
		DlgX = (ScreenWidth - DialogWidth) / 2;
		DlgY = (ScreenHeight - DialogHeight) / 2;

		// Create a DialogPosition record
		DlgPos = new DialogPosition;
		if (DlgPos == NULL)
		{
			// We need to destroy the dialog window
			pDialogWnd->PopEventHandler(FALSE);
			pDialogOp->pEvtHandler->Destroy();
			pDialogWnd->Destroy();
			return FALSE; // Failed to created DialogPosition record
		}
		DlgPos->DlgResourceID = pDialogOp->pEvtHandler->ID;

		// Even though the position is recorded when the dialog is deleted. It is neccessary
		// to record it here also because another dialog with the same resource ID could be
		// created before this dialog is deleted.
		DlgPos->LastX = DlgX;
		DlgPos->LastY = DlgY;

		DlgPos->ActivePage = 0;
		DlgPos->ActivePageIndex=0;

		if (pDialogOp->IS_KIND_OF(DialogTabOp))
		{
			// Record the active page.
			ActivePage = ((wxNotebookPage*)pPropSheet->GetActivePage())->GetId();

			if(pPropSheet != NULL)
			{
				OurPropShtPage *pPage = (OurPropShtPage *)pPropSheet->GetActivePage();
				if(pPage != NULL)
					ActivePage = ((wxNotebookPage*)pPropSheet->GetActivePage())->GetId();
			}

			DlgPos->ActivePage = ActivePage;
			DlgPos->ActivePageIndex = 0;	 // 0 is always the first page
		}
		// Add the position record to the DialogPositionList
		DialogPositionList.AddHead((ListItem*)DlgPos);
	}

	// Store the Dialog window handle in the position record.
	pWinID->DlgWin = pDialogWnd;
	DlgPos->DlgWinList.AddTail( pWinID );

		// Position the dialog
	pDialogWnd->SetSize(DlgX, DlgY, DialogWidth, DialogHeight);

		// In japan we need to set the font so it dosen't use the default ANSI MS San Serif
PORTNOTE("dialog","Removed FontFactory usage")
#ifndef EXCLUDE_FROM_XARALX
	if( UnicodeManager::IsDBCSOS() )
		FontFactory::ApplyFontToWindow( DialogWnd, STOCKFONT_DIALOG ); */
#endif

	// BROADCAST a create message to the active page
	if (pDialogOp->IS_KIND_OF(DialogTabOp))
	{
PORTNOTE("dialog","Removed what looks like some MFC quirkyness")
#ifndef EXCLUDE_FROM_XARALX
		// MarkH 28/6/99 - New check to Set the ActivePage to the ActiveIndex!
		// All related to the proplems with upgrading the MFC with VC6!
		if(pPropSheet != NULL)
			(OurPropShtPage*)pPropSheet->GetBookCtrl()->SetActivePage(pPropSheet->GetActiveIndex());
#endif

		BROADCAST_TO_CLASS(DialogMsg(pDialogOp->WindowID, DIM_CREATE, 0, 0, ActivePage) ,DialogOp);
	}

	// Inform the Dialog that it has been created so that it can be initialised
	// Note that for DialogTabOp's seperate Create messages are sent for each page
	// from the OurPropShtPage OnCreate handler.
	// Alex moved this inside the if statement
	BROADCAST_TO_CLASS( DialogMsg( pDialogOp->WindowID, DIM_CREATE, 0 ) ,DialogOp );

	if( pDialogOp->IS_KIND_OF(DialogTabOp) && 
		NULL != pPropSheet && 
		pPropSheet->GetActivePage() )
	{
		// MarkH 28/6/99 - New bit to get the active page working in tab boxes!
		// All related to the proplems with upgrading the MFC with VC6!
		if(pPropSheet != NULL)
		{
			OurPropShtPage *pPage = (OurPropShtPage *)pPropSheet->GetActivePage();
			if(pPage != NULL)
			{
				ActivePage = ((wxNotebookPage*)pPropSheet->GetActivePage())->GetId();
			}
		}

		BROADCAST_TO_CLASS( DialogMsg( pDialogOp->WindowID, DIM_SET_ACTIVE, 0, 0, ActivePage ), DialogOp );
	}

	// If the dialog which has just been created is modal then disable all other
	// dialogs.

	if( !GetMainFrame()->IsEnabled() )
	{
		EnableAllDialogs(FALSE, pDialogWnd);
	}

	return TRUE; // Success
}


/********************************************************************************************

>	static DialogPosition* DialogManager::FindDialogPositionRecord(CDlgResID DialogID)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/12/94
	Inputs:		DialogID: The dialog's resource id
	Returns:	The DialogPosition record for the dialog, or NULL if the dialog has not been
				created before
	Purpose:	Searches the DialogPositionList to see if this dialog has been created before
				if it has then a pointer to its DialogPosition record is returned,
				else NULL is retuned.
	Scope:		private

********************************************************************************************/

DialogPosition* DialogManager::FindDialogPositionRecord(CDlgResID DialogID)
{
	// Search the DialogPositionList to see if the dialog has been created before
	DialogPosition* DlgPos = (DialogPosition*)(DialogPositionList.GetHead());
	while (DlgPos != NULL)
	{
		if (DlgPos->DlgResourceID == DialogID) // The dialog has been created before
		{
			return DlgPos;
			break;
		}
		// Get the next DialogPosition record
		DlgPos = (DialogPosition*)(DialogPositionList.GetNext((ListItem*)DlgPos));
	}
	return NULL; // Dialog has not been created before
}



/********************************************************************************************

>	void DialogManager::Open(CWindowID WindowID, DialogOp* pDlgOp)
	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/8/93
	Inputs:		WindowID: Dialog's window identifier
				pDlgOp:	  The DialogOp
	Purpose:    The Open method displays a modeless dialog box.
				If the dialog is modal then it will have already been opened in the Create
				method.
	SeeAlso:	DialogOp::Open

********************************************************************************************/

void DialogManager::Open(CWindowID WindowID, DialogOp* pDlgOp)
{
	// Determine if we are opening a bar
PORTNOTE("dialog","Removed DialogBarOp usage")
#ifndef EXCLUDE_FROM_XARALX
	if (pDlgOp->IsKindOf(CC_RUNTIME_CLASS(DialogBarOp)))
	{
		// Find the BaseBar object
		BaseBar			*pCWnd = (wxWindow *)WindowID;
		ENSURE( pCWnd != NULL, "Could not find bar object" );
		// Show the bar window
		if( pCWnd != NULL )
		{
			pCWnd->Show( (DialogBarOp *)pDlgOp );
		}
	}
	else
#endif
	if( !pDlgOp->IsModal() ) // The create method opens a modal dialog
	{
		( (wxWindow *)WindowID )->Show( true ); // Show the hidden dialog
	}

	if (pDlgOp->pEvtHandler->wxAUImanaged)
		CCamFrame::GetMainFrame()->UpdateFrameManager();

}


/********************************************************************************************

>	void DialogManager::Close(CWindowID WindowID, DialogOp* pDlgOp)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/8/93
	Inputs:		WindowID: Dialog's window identifier
				pDlgOp:	  The DialogOp
	Purpose:	The close method removes a modeless dialog from the display but keeps all system
				resources associated with it. It hides the dialog.
				It can be called on a modal dialog but it does nothing.
	SeeAlso:	DialogOp::Close

********************************************************************************************/

void DialogManager::Close(CWindowID WindowID, DialogOp* pDlgOp)
{
	// Hide the dialog

	// Determine if we are hiding a bar
PORTNOTE("dialog","Removed DialogBarOp usage")
#ifndef EXCLUDE_FROM_XARALX
	if (pDlgOp->IsKindOf(CC_RUNTIME_CLASS(DialogBarOp)))
	{
		// Find the BaseBar object
		wxWindow* pCWnd = CWnd::FromHandlePermanent(WindowID);
		ENSURE(pCWnd != NULL, "Could not find bar object");
		// Show the bar window
		if (pCWnd != NULL)
		{
			((BaseBar*)pCWnd)->Hide((DialogBarOp*)pDlgOp);
		}
	}
	else
#endif
	if (!(pDlgOp->IsModal())) // The delete method closes a modal dialog
	{
		ENSURE(WindowID != NULL,"NULL WindowID");
		( (wxWindow *)WindowID )->Show( false );
	}

	if (pDlgOp->pEvtHandler->wxAUImanaged)
		CCamFrame::GetMainFrame()->UpdateFrameManager();
}

/********************************************************************************************

>	static BOOL DialogManager::BringToTop(CWindowID WindowID)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/6/95
	Inputs:		-
	Returns:	FALSE if the function failed.
	Purpose:	This function brings an open dialog to the top of the z-order

********************************************************************************************/

BOOL DialogManager::BringToTop(CWindowID WindowID, DialogOp* pDlgOp)
{
	ERROR2IF(!WindowID, FALSE, "BringToTop called on a dialog without a window");
	( (wxWindow *)WindowID )->Raise();
	if (pDlgOp->pEvtHandler->wxAUImanaged)
		CCamFrame::GetMainFrame()->UpdateFrameManager();
	return TRUE;
}



/********************************************************************************************

>	static void DialogManager::Event (DialogEventHandler *pEvtHandler, wxEvent &event)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/9/93
	Purpose:	OnCommand message handler. Translates a windows Command message into a DIM
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void DialogManager::Event (DialogEventHandler *pEvtHandler, wxEvent &event)
{
	WXTYPE EventType = event.GetEventType();
//	CDlgMessage DIM = DIM_NONE;
	ResourceID id = event.GetId();
	UINT_PTR DlgMsgParam = 0;
	INT32 PageID = 0;
	BOOL HandleMessage=FALSE;
	BOOL Defer=TRUE;

	if (!pEvtHandler->pwxWindow || !pEvtHandler->pDialogOp)
	{
		// We are in the process of destruction
		event.Skip();
		return;
	}

	// First handle events we previously asked to defer processing of
	if (event.IsKindOf(CLASSINFO(wxCamDialogEvent)) && (EventType == wxEVT_CAMDIALOG_DEFERREDMSG))
	{
		// We posted this event and asked it to come back later, and it duly has
		wxCamDialogEvent * pDialogEvent = (wxCamDialogEvent *)(&event);
		pDialogEvent->msg.DlgWndID = pEvtHandler->pwxWindow; // this ensures we are using a valid window pointer
		// Send it around
		BROADCAST_TO_CLASS( DialogMsg(pDialogEvent->msg), DialogOp );
		return;
	}

	wxWindow * pGadget = NULL;
	if (id) pGadget = GetGadget(pEvtHandler->pwxWindow, id);

	// Try and find-out whether our control is part of a tabbed dialog page
	if( NULL != pGadget )
	{
		// pEvtHandler->pwxWindow maybe our immediate wxPanel\wxDialog, but won't
		// be in case of tabbed dialog
		wxWindow*	pDialog = pGadget->GetParent();
		while( NULL != pDialog && !pDialog->IsKindOf( CLASSINFO(wxDialog) ) && 
			!pDialog->IsKindOf( CLASSINFO(wxPanel) ) )
		{
			pDialog = pDialog->GetParent();
		}

		// Could this be part of a tabbed dialog?
		if( NULL != pDialog && pDialog->IsKindOf( CLASSINFO(wxPanel) ) )
		{
			// A parent of type wxBookCtrlBase would synch it
			wxWindow *pDialogParent = pDialog->GetParent();
			if( NULL != pDialogParent && pDialogParent->IsKindOf( CLASSINFO(wxBookCtrlBase) ) )
				PageID = pDialog->GetId();
		}
	}

	// Make up a default message
	DialogMsg msg(pEvtHandler->pwxWindow, DIM_NONE, id, DlgMsgParam, PageID);

	if (!event.IsKindOf(CLASSINFO(wxMouseEvent))) // MouseEvents are too noisy
	{
		TRACEUSER("amb",_T("event %d(%s) received, ID=%d(%s), wxw=%llx"), EventType, DialogEventHandler::GetEventName(EventType), id,
					CamResource::GetObjectName((ResourceID)id), pEvtHandler->pwxWindow);
	}

	if (
		(EventType == wxEVT_LEFT_DCLICK) ||
		(EventType == wxEVT_MIDDLE_DCLICK) ||
		(EventType == wxEVT_RIGHT_DCLICK) ||
		FALSE)
	{
		// OK, these are a bit deadly. We expected there to be TWO mouse up mouse downs. People
		// don't seem to hang off double clicks themselves, but do their own double click handling
		// (why oh why). So we generate an extra mouse down and mouse up, and sending them to
		// ourselves. This may not be necessary on all platforms.
		wxMouseEvent *MouseDown = (wxMouseEvent *)(event.Clone());
		wxMouseEvent *MouseUp = (wxMouseEvent *)(event.Clone());
		if (MouseDown && MouseUp)
		{
			if (EventType == wxEVT_LEFT_DCLICK)
			{
				MouseDown->SetEventType(wxEVT_LEFT_DOWN);
				MouseUp->SetEventType(wxEVT_LEFT_UP);
			}
			else if	(EventType == wxEVT_MIDDLE_DCLICK)
			{
				MouseDown->SetEventType(wxEVT_MIDDLE_DOWN);
				MouseUp->SetEventType(wxEVT_MIDDLE_UP);
			}
			else
			{
				MouseDown->SetEventType(wxEVT_RIGHT_DOWN);
				MouseUp->SetEventType(wxEVT_RIGHT_UP);
			}
			
			//MouseDown.SetEventObject(pEvtHandler->pwxWindow);
			// MouseUp.SetEventObject(pEvtHandler->pwxWindow);
			// set it for processing later
			pEvtHandler->pwxWindow->GetEventHandler()->ProcessEvent(*MouseDown);
			pEvtHandler->pwxWindow->GetEventHandler()->ProcessEvent(*MouseUp);
		}
		if (MouseDown) delete MouseDown;
		if (MouseUp) delete MouseUp;
	}	

	/* Here is a list of possible command events
	wxEVT_COMMAND_BUTTON_CLICKED
	wxEVT_COMMAND_CHECKBOX_CLICKED
	wxEVT_COMMAND_CHOICE_SELECTED
	wxEVT_COMMAND_LISTBOX_SELECTED
	wxEVT_COMMAND_LISTBOX_DOUBLECLICKED
	wxEVT_COMMAND_CHECKLISTBOX_TOGGLED
	wxEVT_COMMAND_TEXT_UPDATED	// only with WXWIN_COMPATIBILITY_EVENT_TYPES
	wxEVT_COMMAND_TEXT_ENTER	// only with WXWIN_COMPATIBILITY_EVENT_TYPES
	wxEVT_COMMAND_TEXT_URL		// only with WXWIN_COMPATIBILITY_EVENT_TYPES
	wxEVT_COMMAND_TEXT_MAXLEN	// only with WXWIN_COMPATIBILITY_EVENT_TYPES
	wxEVT_COMMAND_MENU_SELECTED
	wxEVT_COMMAND_SLIDER_UPDATED
	wxEVT_COMMAND_RADIOBOX_SELECTED
	wxEVT_COMMAND_RADIOBUTTON_SELECTED
	wxEVT_COMMAND_SCROLLBAR_UPDATED // Obselete - see wxWVT_SCROLL
	wxEVT_COMMAND_VLBOX_SELECTED
	wxEVT_COMMAND_COMBOBOX_SELECTED
	wxEVT_COMMAND_TOOL_RCLICKED
	wxEVT_COMMAND_TOOL_ENTER
	wxEVT_COMMAND_SPINCTRL_UPDATED

	We can't use switch on these - GRRR!
	*/

 	if (
		(EventType == wxEVT_COMMAND_BUTTON_CLICKED) ||
		FALSE)
	{
		// We should cope with Right Button here
		if ((ResourceID)id == _R(wxID_OK) )
		{
			msg.DlgMsg = DIM_COMMIT;
			HandleMessage = TRUE;
		}
		else if (id == _R(ID_CC_APPLY_NOW))
		{
			// Clicking on the apply now button is the same as a soft commit
			msg.DlgMsg = DIM_SOFT_COMMIT;
			HandleMessage = TRUE;
		}
		else if (id == _R(wxID_CANCEL))
		{
			msg.DlgMsg = DIM_CANCEL;
			// Do not defer processing of clicks on the close button because the default handler may destroy the window on
			// exit from this call
			Defer=FALSE; 
			HandleMessage = TRUE;
		}
		else
		{
			msg.DlgMsg = DIM_LFT_BN_CLICKED;
			HandleMessage = TRUE;
		}
	}
	else if (
		(EventType == wxEVT_COMMAND_CHOICE_SELECTED) ||
		(EventType == wxEVT_COMMAND_LISTBOX_SELECTED) ||
		(EventType == wxEVT_COMMAND_CHECKLISTBOX_TOGGLED) ||
//		We skip this because it's generated when we change the text ourselves. We should probably do something more subtle
//		(EventType == wxEVT_COMMAND_TEXT_UPDATED) ||	// only with WXWIN_COMPATIBILITY_EVENT_TYPES
		(EventType == wxEVT_COMMAND_TEXT_URL) ||		// only with WXWIN_COMPATIBILITY_EVENT_TYPES
//		(EventType == wxEVT_COMMAND_TEXT_MAXLEN) ||		// only with WXWIN_COMPATIBILITY_EVENT_TYPES
		(EventType == wxEVT_COMMAND_MENU_SELECTED) ||
//		(EventType == wxEVT_COMMAND_SLIDER_UPDATED) ||
		(EventType == wxEVT_COMMAND_RADIOBOX_SELECTED) ||
		(EventType == wxEVT_COMMAND_VLBOX_SELECTED) ||
		(EventType == wxEVT_COMMAND_COMBOBOX_SELECTED) ||
		(EventType == wxEVT_COMMAND_SPINCTRL_UPDATED) ||
		((
		  (EventType == wxEVT_SCROLL_CHANGED) || 
		  (EventType == wxEVT_SCROLL_THUMBTRACK) || 
		  (EventType == wxEVT_SCROLL_THUMBRELEASE) ||
		  (EventType == wxEVT_SCROLL_LINEUP) ||
		  (EventType == wxEVT_SCROLL_LINEDOWN) ||
		  (EventType == wxEVT_SCROLL_PAGEUP) ||
		  (EventType == wxEVT_SCROLL_PAGEDOWN)
		) && !(pGadget && pGadget->IsKindOf(CLASSINFO(wxSlider)))) || // Don't handle slider scroll stuff here
		(EventType == wxEVT_COMMAND_TREE_SEL_CHANGED) ||
		FALSE)
	{
		msg.DlgMsg = DIM_SELECTION_CHANGED;
		msg.DlgMsgParam = NO_COMMIT;
		HandleMessage = TRUE;
	}
	else
	if( EventType == wxEVT_COMMAND_TEXT_UPDATED && 	// only with WXWIN_COMPATIBILITY_EVENT_TYPES
		pGadget == wxWindow::FindFocus() )
	{
		msg.DlgMsg = DIM_TEXT_CHANGED;
		HandleMessage = TRUE;
	}
	else if(
		(EventType == wxEVT_COMMAND_TEXT_ENTER) ||
		FALSE)
	{
		msg.DlgMsg = DIM_SELECTION_CHANGED;
		msg.DlgMsgParam = ENTER_COMMIT;
		HandleMessage = TRUE;
	}
	else if(
		(( (EventType == wxEVT_SCROLL_THUMBTRACK) ||
		   (EventType == wxEVT_SCROLL_LINEUP) ||
		   (EventType == wxEVT_SCROLL_LINEDOWN) ||
		   (EventType == wxEVT_SCROLL_PAGEUP) ||
		   (EventType == wxEVT_SCROLL_PAGEDOWN)
		 ) && (pGadget && pGadget->IsKindOf(CLASSINFO(wxSlider)))) || // Handle slider movements - note SCROLL_CHANGED always comes later
		FALSE)
	{
		msg.DlgMsg = DIM_SLIDER_POS_CHANGING;
		HandleMessage = TRUE;
	}
	else if(
		((EventType == wxEVT_SCROLL_THUMBRELEASE || EventType == wxEVT_SCROLL_CHANGED) && (pGadget && pGadget->IsKindOf(CLASSINFO(wxSlider)))) || // Handle slider changes
		FALSE)
	{
		msg.DlgMsg = DIM_SLIDER_POS_SET;
		HandleMessage = TRUE;
	}
	else if(
		(EventType == wxEVT_COMMAND_CHECKBOX_CLICKED) ||
		(EventType == wxEVT_COMMAND_RADIOBUTTON_SELECTED) ||
		FALSE)
	{
		msg.DlgMsg = DIM_LFT_BN_CLICKED; // apparently not a DIM_SELECTION_CHANGED - the click itself is eaten by the radio control - please do not change - AMB
		HandleMessage = TRUE;
	}
	else if (
		(EventType == wxEVT_COMMAND_LISTBOX_DOUBLECLICKED) ||
		FALSE)
	{
		msg.DlgMsg = DIM_SELECTION_CHANGED_COMMIT;
		HandleMessage = TRUE;
	}
	else if (
		(EventType == wxEVT_LEFT_DOWN) ||
		FALSE)
	{
		msg.DlgMsg = DIM_LFT_BN_DOWN;
		HandleMessage = TRUE;
	}	
	else if (
		(EventType == wxEVT_LEFT_UP) ||
		FALSE)
	{
		msg.DlgMsg = DIM_LFT_BN_UP;
		HandleMessage = TRUE;
	}	
	else if (
		(EventType == wxEVT_RIGHT_DOWN) ||
		FALSE)
	{
		msg.DlgMsg = DIM_RGT_BN_DOWN;
		HandleMessage = TRUE;
	}	
	else if (
		(EventType == wxEVT_RIGHT_UP) ||
		FALSE)
	{
		msg.DlgMsg = DIM_RGT_BN_UP;
		HandleMessage = TRUE;
	}	
	else if (
		(EventType == wxEVT_MIDDLE_DOWN) ||
		FALSE)
	{
		msg.DlgMsg = DIM_MID_BN_DOWN;
		HandleMessage = TRUE;
	}	
	else if (
		(EventType == wxEVT_MIDDLE_UP) ||
		FALSE)
	{
		msg.DlgMsg = DIM_MID_BN_UP;
		HandleMessage = TRUE;
	}	
	else if (
		(EventType == wxEVT_MOTION) ||
		FALSE)
	{
		msg.DlgMsg = ((wxMouseEvent *)&event)->Dragging()?DIM_MOUSE_DRAG:DIM_MOUSE_MOVE;
		HandleMessage = TRUE;
	}	
	else if (
		(EventType == wxEVT_MOUSEWHEEL) ||
		FALSE)
	{
		msg.DlgMsg = (((wxMouseEvent *)&event)->GetWheelRotation()>0)?DIM_MOUSEWHEEL_UP:DIM_MOUSEWHEEL_DOWN;
		HandleMessage = TRUE;
	}	
	else if (
		(EventType == wxEVT_MOVE) ||
		FALSE)
	{
		msg.DlgMsg = DIM_DLG_MOVED;
		HandleMessage = TRUE;
	}	
	else if (
		(EventType == wxEVT_SIZE) ||
		FALSE)
	{
		msg.DlgMsg = DIM_DLG_RESIZED;
		HandleMessage = TRUE;
	}	
	else if (
		((EventType == wxEVT_CAMDIALOG_REDRAW) && (pGadget)) ||
		FALSE)
	{
		if (CCamApp::IsDisabled())
		{
			TRACE( _T("kernel-rendered gadget repaint has been aborted: the system is disabled (due to an error/ensure?)\n"));
			HandleMessage = FALSE;
		}
		else
		{
			// HDC hDC = pInfo->PaintInfo.hdc;
			// HPALETTE OldPalette = PaletteManager::StartPaintPalette(hDC);
		
			ReDrawInfoType ExtraInfo;
		
			ExtraInfo.pMousePos = NULL;		// No mouse position info for redraw events


			// Build a CC dc out of it for rendering to the screen
			// Get a MFC CDC to put the DC in
			CCPaintDC MyDc(pGadget);

			ExtraInfo.pDC = &MyDc;
		
			// The devices DPI
			ExtraInfo.Dpi = OSRenderRegion::GetFixedDCPPI(MyDc).GetHeight();

			// How big the window is
			wxSize WindowSize = pGadget->GetClientSize();
			ExtraInfo.dx = (((INT32)WindowSize.GetWidth())*72000) / ExtraInfo.Dpi;
			ExtraInfo.dy = (((INT32)WindowSize.GetHeight())*72000) / ExtraInfo.Dpi;
		
			MyDc.BeginDrawing();

			wxRegionIterator upd(pGadget->GetUpdateRegion()); // get the update rect list

			BOOL Stop = FALSE;

			while (upd && !Stop)
			{
				// Alternatively we can do this:
				wxRect ClipRect(upd.GetRect());
				// Should we clip this to the WindowSize here? For reasons which are not entirely clear, setting the
				// ClipRect breaks GRenderRegions. But if we don't set the clip rect, it breaks (at least some)
				// code that uses OSRenderRegion (sigh). Right now this is too painful to debug, so instead we
				// cop out, and ask the control whether or not it would like a ClipRect set. Those that say no
				// will paint the entire area, so we only give them one call

				BOOL UseClipRect = (pGadget->IsKindOf(CLASSINFO(wxCamDrawControl)))
									&& (((wxCamDrawControl*)pGadget)->GetStyle() & wxCDCS_SETCLIPRECT);

				if (UseClipRect)
				{
					MyDc.SetClippingRegion(ClipRect);
					ClipRect.Inflate(1,1); // work around wxRect problems.
				}
				else
				{
					ClipRect = wxRect(WindowSize);
					Stop = TRUE; // cease drawing after this one
				}
				
				DocRect DocClipRect;
			
				// Convert to millipoints, Also need to flip the y coords to get a
				// rectangle in with the origin in the bottom left.
				DocClipRect.lo.x = (ClipRect.GetLeft() * 72000) / ExtraInfo.Dpi;
				DocClipRect.lo.y = ExtraInfo.dy - ((ClipRect.GetBottom() * 72000) / ExtraInfo.Dpi);
			
				DocClipRect.hi.x = (ClipRect.GetRight() * 72000) / ExtraInfo.Dpi;
				DocClipRect.hi.y = ExtraInfo.dy - ((ClipRect.GetTop() * 72000) / ExtraInfo.Dpi);
			
				// Set the pointer in the extra info structure
				ExtraInfo.pClipRect = &DocClipRect;
			
				// Build the message and send it to the dialog op
				// It is up to the dialog op to build a render region etc and attach the CCDC to it
				// and to tidy the region up after it has finished drawing in it CDlgMessage
				BROADCAST_TO_CLASS(DialogMsg(pEvtHandler->pwxWindow, DIM_REDRAW, id, (UINT_PTR)(void *)&ExtraInfo, PageID), DialogOp);
				
				upd ++ ;
			}
		
			MyDc.EndDrawing();		
		
			// if (OldPalette)
			//	PaletteManager::StopPaintPalette(hDC, OldPalette);
		}
	}

	//case	wxEVT_COMMAND_TOOL_RCLICKED:
	//case	wxEVT_COMMAND_TOOL_ENTER:


	// Handle filling in ExtraInfo on redraw events
	if ((msg.DlgMsg != DIM_NONE) && pGadget && pGadget->IsKindOf(CLASSINFO(wxCamDrawControl)) && event.IsKindOf(CLASSINFO(wxMouseEvent)))
	{
		switch (msg.DlgMsg)
		{
			case DIM_LFT_BN_DOWN:
			case DIM_LFT_BN_UP:
			case DIM_LFT_BN_CLICKED:
			case DIM_RGT_BN_DOWN:
			case DIM_RGT_BN_UP:
			case DIM_RGT_BN_CLICKED:
			case DIM_MID_BN_DOWN:
			case DIM_MID_BN_UP:
			case DIM_MID_BN_CLICKED:
			case DIM_MOUSE_DRAG:
			case DIM_MOUSE_MOVE:
			case DIM_MOUSEWHEEL_UP:
			case DIM_MOUSEWHEEL_DOWN:
			{

				// HDC hDC = pInfo->PaintInfo.hdc;
				// HPALETTE OldPalette = PaletteManager::StartPaintPalette(hDC);
			
				ReDrawInfoType ExtraInfo;
			
				ExtraInfo.pMousePos = NULL;		// No mouse position info for redraw events
		
		
				// Build a CC dc out of it for rendering to the screen
				// Get a MFC CDC to put the DC in
				CCPaintDC MyDc(pGadget);
		
				ExtraInfo.pDC = NULL;
			
				// The devices DPI
				ExtraInfo.Dpi = OSRenderRegion::GetFixedDCPPI(MyDc).GetHeight();
		
				// How big the window is
				wxSize WindowSize = pGadget->GetClientSize();
				ExtraInfo.dx = (((INT32)WindowSize.GetWidth())*72000) / ExtraInfo.Dpi;
				ExtraInfo.dy = (((INT32)WindowSize.GetHeight())*72000) / ExtraInfo.Dpi;

				// Work out the MILLIPOINT coordinates of the mouse position
				// Note that the Y value is flipped, as the kernel-origin is at the bottom left
				INT32 XPos = ((wxMouseEvent *)(&event))->GetX();
				INT32 YPos = ((wxMouseEvent *)(&event))->GetY();
	
				DocCoord MousePos;
				MousePos.x = (XPos * 72000) / ExtraInfo.Dpi;
				MousePos.y = ExtraInfo.dy - ((YPos * 72000) / ExtraInfo.Dpi);
				ExtraInfo.pMousePos = &MousePos;

				BROADCAST_TO_CLASS(DialogMsg(pEvtHandler->pwxWindow, msg.DlgMsg, id, (UINT_PTR)(void *)&ExtraInfo, PageID), DialogOp);

				msg.DlgMsg = DIM_NONE; // Stop further processing
			}

			default:
				break;
		}
	}


	// If we have a message to send, then send it (or defer it for later)
	if (msg.DlgMsg != DIM_NONE)
	{

		if (Defer)
		{
			// We should send the message out later - we use the same ID
			wxCamDialogEvent deferredevent (wxEVT_CAMDIALOG_DEFERREDMSG, event.GetId(), msg);
			deferredevent.SetEventObject(pEvtHandler->pwxWindow);
			// set it for processing later
			pEvtHandler->pwxWindow->GetEventHandler()->AddPendingEvent(deferredevent);
		}
		else
		{
			BROADCAST_TO_CLASS( DialogMsg(msg), DialogOp );
		}
	}
	
	// If we haven't marked this message as handled, call Skip() so that others can handle
	// it
	if (!HandleMessage) event.Skip(); // we didn't handle it
	return;
}







/********************************************************************************************

>	static BOOL DialogManager::IsGadgetTickable(CWindowID WindowID,
						  						CGadgetID Gadget)


	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/6/95
	Inputs:		WindowID: Dialog's window identifier
				Gadget:	  The Gadget to test

	Purpose:	Determines if the gadget is of a type that can be ticked

********************************************************************************************/

BOOL DialogManager::IsGadgetTickable(CWindowID WindowID,
					  				 CGadgetID Gadget)
{
	wxWindow * pGadget = GetGadget(WindowID, Gadget);
	if (!pGadget) return FALSE;

	// Not sure why this is an exhaustive list
	return ( pGadget->IsKindOf(CLASSINFO(wxCheckBox)) ||
		 	 pGadget->IsKindOf(CLASSINFO(wxButton)) ||
		 	 (pGadget->IsKindOf(CLASSINFO(wxCamArtControl)) && (((wxCamArtControl*)pGadget)->GetStyle() & wxCACS_PUSHBUTTON)) ||
		 	 pGadget->IsKindOf(CLASSINFO(wxBitmapButton))
			 );
}


/********************************************************************************************

>	static BOOL DialogManager::ColourPickerAbort(CWindowID WindowID, CGadgetID Gadget)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/1/2000
	Inputs:		WindowID:	Dialog's window identifier
				Gadget:		The colourpicker that this request is made upon (which is checked
							to ensure that it is a custom colour picker).

	Purpose:	Instructs camelots custom colour control (which I also wrote) to 'shutdown'
				and (indirectly) return control to the colour editor dialog.

********************************************************************************************/

BOOL DialogManager::ColourPickerAbort(CWindowID WindowID, CGadgetID Gadget, WPARAM wParam)
{
	PORTNOTETRACE("dialog","DialogManager::ColourPickerAbort - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	// Currently the gadget is only tickable if it is a button
	HWND hGadget = GetDlgItem((HWND)WindowID, (INT32)Gadget);
	String_256 ClassNameStr;  // The control type

	// Find out the class type of the gadget
	GetClassName(hGadget, (TCHAR*)ClassNameStr, 255);

	if (ClassNameStr == String_8(TEXT("cc_colPicker")))
	{
		BOOL RetVal = FALSE;

		RetVal = SendMessage(hGadget, WM_COLOURPICKERABORT, wParam, 0);

		return (RetVal);
	}

	return (TRUE);
	ENSURE(FALSE, "Calling ColourPickerAbort for an invalid control");
#endif
	return FALSE;
}


/********************************************************************************************

>	DialogManager::Delete(CWindowID WindowID, DialogOp* pDlgOp)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/8/93
	Inputs:		WindowID: Dialog's window identifier
				pDlgOp:	  The DialogOp
	Purpose:	The delete method will delete all system resources and other information
				which is kept about the dialog. It also records the dialogs current position
				so that it can be restored the next time the dialog is created.
	SeeAlso:	DialogOp::Delete

********************************************************************************************/

void DialogManager::Delete(CWindowID WindowID, DialogOp* pDlgOp)
{
	ERROR2IF (!pDlgOp, (void)0, "No dialog op to DialogManager::Delete()");
	ERROR2IF (!WindowID, (void)0, "No window to DialogManager::Delete()");

	if (!pDlgOp->pEvtHandler)
	{
		ERROR3("DialogManager::Delete() No dialog op event handler - has this window been deleted twice?");
		return;
	}

	// If we've already been destroyed (by something else) - a situation which should never happen - then
	// return without doing anything
	if (pDlgOp->pEvtHandler && pDlgOp->pEvtHandler->m_GrimReaperSent)
	{
		TRACEALL(_T("DialogManager::Delete() Window has been deleted by something else, then Delete() called"));
		return;
	}

	if (((wxWindow *)WindowID)->IsBeingDeleted())
	{
		ERROR3("Trying to delete a window that is already being deleted, has an event handler, but has not sent grim reaper");
		return;
	}

	// See if the dialogs has a position record  (If it's a DialogBarOp it won't have one)
	DialogPosition* DlgPos = (DialogPosition*)DialogPositionList.GetHead();
	CWindowIDItem* WinID;
	while(DlgPos != NULL)
	{
		// Search the DlgWinList for the DlgPos for WindowID
		WinID = (CWindowIDItem*)DlgPos->DlgWinList.GetHead();
		while (WinID != NULL)
		{
			if ((WinID->DlgWin) == WindowID) // Found the dialogs position record
			{
				// The Dialogs window is about to be destroyed so delete the WinID from the
				// DlgWinList of DlgPos.
				delete(DlgPos->DlgWinList.RemoveItem((ListItem*)WinID));
				goto FoundPos; // What a rebel
			}
			WinID = (CWindowIDItem*)DlgPos->DlgWinList.GetNext((ListItem*)WinID);
		}
		// Get the next position record
		DlgPos = ((DialogPosition*)DialogPositionList.GetNext((ListItem*)DlgPos));
	}
	// No DialogPosition record was found so must be a DialogBarOp


	FoundPos:

	wxWindow		   *pCWnd = (wxWindow *)WindowID;


	if (DlgPos != NULL)
	{
		// Record the dialog's position so that it can be restored if the dialog is created again
   		wxRect			DialogRect( pCWnd->GetRect() );
		DlgPos->LastX = DialogRect.x;
		DlgPos->LastY = DialogRect.y;
		DlgPos->ActivePage = 0;
		DlgPos->ActivePageIndex = 0;

		// If the dialog is tabbed then we need to record the active page as well
		// We can't find the runtime class of the DialogOp at this point because Delete can be called
		// from its destructor
PORTNOTE("dialog","Removed OurPropSheet usage")
#ifndef EXCLUDE_FROM_XARALX
		if ((pCWnd != NULL) && (pCWnd->IsKindOf(RUNTIME_CLASS(OurPropSheet))))
		{
			OurPropSheet* pPropSheet = (OurPropSheet*)pCWnd;
			OurPropShtPage* pPage = (OurPropShtPage*)pPropSheet->GetActivePage();
			if (pPage)
			{
				DlgPos->ActivePage = pPage->GetPageID();
				// Store the pages index as well
				GetPageWindow(WindowID, DlgPos->ActivePage, &(DlgPos->ActivePageIndex));
			}
			else
				ERROR3("There is no active page");
		}
#endif
	}

	if (pDlgOp->pEvtHandler->wxAUImanaged)
	{
		// Remove the bar from wxAUI
		CCamFrame::GetMainFrame()->GetFrameManager()->DetachPane(pCWnd);
		CCamFrame::GetMainFrame()->UpdateFrameManager();
	}

	// Delete all discardable strings associated with the dialog
	DlgDiscardString* DiscardStr = (DlgDiscardString*)(DiscardStrList.GetHead());
	while (DiscardStr != NULL) // While there are still strings to delete
	{
		DlgDiscardString* Next = (DlgDiscardString*)(DiscardStrList.GetNext(DiscardStr));
		if (DiscardStr->DlgWindow == WindowID)  // The string belongs to the dialog being
												// deleted
		{
			delete (DiscardStr->pStr); // Delete the string
			delete(DiscardStrList.RemoveItem(DiscardStr)); // Delete the DiscardStr record
		}
	 	DiscardStr = Next;  // Get next string record
	}

	// Delete all scrollPageInc information associated with the dialog
	ScrollPageInc* PgInc = (ScrollPageInc*)(ScrollPageIncList.GetHead());
	while (PgInc != NULL)
	{
		ScrollPageInc* Next = (ScrollPageInc*)(ScrollPageIncList.GetNext((ListItem*)PgInc));
		if( PgInc->pDlgWindow == WindowID )	// The ScrollPageInc record belongs to the dialog
											// being deleted.
			delete (ScrollPageIncList.RemoveItem( (ListItem*)PgInc) );	// Delete the
																		// ScrollPageInc record
		PgInc = Next; // Get next record
	}

	// Delete all ControlInfo records
	List* ControlInfoList = GetControlList( (wxWindow *)WindowID );

	// Remove new-form control list
	ControlList::Get()->RemoveWindowAndChildren((wxWindow *)WindowID);

	// We are about to destroy the window. Disconnecting our event handler sounds like a good
	// idea at this point, as Destroy() does not destroy the window immediately, so there
	// is a possibility of receiving further events
	((wxWindow *)WindowID)->PopEventHandler(FALSE); // leave the DialogOp's destructor to delete it
	pDlgOp->pEvtHandler->Destroy();

#ifndef EXCLUDE_FROM_XARALX
	// Is the window  a CWnd (i.e a basebar, or a Property sheet)
	if (pCWnd != NULL)
	{
PORTNOTE("dialog","Removed OurPropSheet usage")
		if (pCWnd->IsKindOf(RUNTIME_CLASS(OurPropSheet)))
		{
			DeletePropShtDetails((DialogTabOp*)pDlgOp);
			OurPropSheet* ps = (OurPropSheet*)pCWnd;
			if (ps->IsModal() && ps->IsKindOf(CLASSINFO(wxDialog)))
)
			{
				// End the Modal
				ps->EndDialog(TRUE);
				// We cannot destroy the property sheet here because we are in its
				// DoModal function. It will be destroyed in the Create method.
			}
			else
			{
				pCWnd->DestroyWindow();
				delete pCWnd;
			}
		}
		else
		{
			pCWnd->Destroy();
		}
	}
	else
#endif


	if (pDlgOp->IsModal() && WindowID->IsKindOf(CLASSINFO(wxDialog)))
	// A normal Modal
	{
		( (wxDialog *)WindowID )->EndModal( TRUE );
		( (wxWindow *)WindowID )->Destroy();
	}
	else
	{
		( (wxWindow *)WindowID )->Destroy();
	}

	if (ControlInfoList)
	{
		// Its one of our special windows with an attached list

		while (!ControlInfoList->IsEmpty())
			delete(ControlInfoList->RemoveHead());
		// Delete the ControlInfo list
		delete (ControlInfoList);
		ControlInfoList = NULL;
	}

	// Restore the active/disabled window state
	if (pDlgOp->IsModal())
	{
		// DialogManager::RestoreActiveDialogState();
	}

	DefaultKeyboardFocus();
	// All spick and span
}

/********************************************************************************************

>	void DialogManager::DeletePropShtDetails(DialogTabOp* pOp)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/11/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Destroys info about the property sheet associated with pOp
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void DialogManager::DeletePropShtDetails(DialogTabOp* pOp)
{
	PORTNOTETRACE("dialog","DialogManager::DeletePropShtDetails - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	// Try to find the property sheet record
	DlgTagOpToPropShtItem* Item = (DlgTagOpToPropShtItem*)(DlgTagOpToPropShtList.GetHead());
	while (Item != NULL)
	{
		if (Item->pDialogTabOp == pOp)
		{
			// Delete this Item
			delete(DlgTagOpToPropShtList.RemoveItem(Item));
			return;
		}
		// Find the next property sheet record
		Item = (DlgTagOpToPropShtItem*)(DlgTagOpToPropShtList.GetNext(Item));
	}
	ERROR3("Could not find property sheet record");
#endif
}

/********************************************************************************************

>	BOOL DialogManager::SetComboListLength( CWindowID WindowID,
							                CGadgetID Gadget)

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/7/94
	Inputs:		WindowID: 	 Dialog box window identifier
				Gadget: 	 Identifier of the gadget
	Returns:	-
	Purpose:

	Errors:		If the function is called on an invalid control then an ENSURE failure will
				occur in a DEBUG build.

	SeeAlso:	DialogOp::SetComboListLength

********************************************************************************************/

void DialogManager::SetComboListLength(CWindowID WindowID,CGadgetID Gadget)
{
	// This function existed to resize the combo box, but under wx combo boxes resize
	// automagically when their contents are adjusted.
	return;
}

// -----------------------------------------------------------------------------------------
// The following functions are called by same named functions in the DialogOp
// class. The DialogOp functions are provided for ease of use and do not have a
// CWindowID argument.




/********************************************************************************************

>	void DialogManager::SetGadgetBitmaps(CWindowID WindowID, CGadgetID Gadget,
										UINT32 Bitmap1, UINT32 Bitmap2)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/10/94
	Inputs:		WindowID - Dialog box window identifier
				Gadget - Identifier of the gadget
				Bitmap1 - The resourse ID of the first bitmap
				Bitmap2 - The resourse ID of the second bitmap
	Purpose:	This function will set the bitmaps associated with a gadget.

				This will only work for cc_Slider and cc_BitmapButton controls

				cc_Slider:
				In this case Bitmap1 is used as the Base of the slider and Bitmap2 is used
				as the little slideing bar.	 (Drop the 'e' before 'ing' Rik)

				cc_BitmapButton:
				Bitmap1 & Bitmap2 are ignored. Instead, the bitmaps are specified by the window
				title text, in the same way as bitmap buttons in bars. (added by Markn 27/3/95)

********************************************************************************************/

void DialogManager::SetGadgetBitmaps(CWindowID WindowID, CGadgetID Gadget, UINT32 Bitmap1, UINT32 Bitmap2)
{
	PORTNOTETRACE("dialog","DialogManager::SetGadgetBitmaps - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	// Find out about the Gadget
    HWND hGadget = GetDlgItem((HWND)WindowID, (INT32)Gadget);

	// Find out the class type of the gadget
	String_256 ClassNameStr;
	GetClassName(hGadget, (TCHAR*)ClassNameStr, 255);

	// Only do something if it is a slider
	if ((ClassNameStr == String_16(TEXT("cc_Slider"))))
	{
		// Fill in the details of the bitmaps
		SliderBitmapInfo BitmapInfo;
		BitmapInfo.hAppInstance = AfxGetResourceHandle(); //AfxGetApp()->m_hInstance;
		BitmapInfo.BaseBmpStr = MAKEINTRESOURCE(Bitmap1);
		BitmapInfo.SliderBmpStr = MAKEINTRESOURCE(Bitmap2);

		// Send a message to the slider, telling to use the bitmaps
		SendMessage(hGadget, WM_SET_CHANNEL_WIDTH, 8, 0);
		SendMessage(hGadget, WM_SET_SLIDERBITMAPS, 0, (INT32)&BitmapInfo);

	}

	// Send new style messages to the button classes because they can use PNG
	// resources
	if (ClassNameStr == String_16(TEXT("cc_BitmapButton"))
		|| ClassNameStr == String_16(TEXT("cc_SmallBitmapButton")) )
	{
		SetGadgetBitmaps(hGadget, AfxGetResourceHandle());
	}
#endif
}

/********************************************************************************************

>	void DialogManager::SetGadgetBitmap(CWindowID WindowID, CGadgetID Gadget,
										ResourceID Bitmap)

	Author:		Alex Bligh <alex@alex.org.uk>
	Created:	07/05/2006
	Inputs:		WindowID - Dialog box window identifier
				Gadget - Identifier of the gadget
				Bitmap - The resourse ID of the bitmap, or 0 for default;
	Purpose:	This function will set the bitmaps associated with a gadget.

				This works only for bitmap buttons

********************************************************************************************/

void DialogManager::SetGadgetBitmap(CWindowID WindowID, CGadgetID Gadget, ResourceID Bitmap)
{
	wxWindow* pGadget = GetGadget(WindowID, Gadget);
	if (!pGadget) return;

	if ( pGadget->IsKindOf(CLASSINFO(wxCamArtControl))
		)
	{
		((wxCamArtControl *)pGadget)->SetBitmapId(Bitmap);
		pGadget->Refresh();
	}
}

/********************************************************************************************

>	ResourceID DialogManager::SetGadgetBitmap(CWindowID WindowID, CGadgetID Gadget)

	Author:		Alex Bligh <alex@alex.org.uk>
	Created:	07/05/2006
	Inputs:		WindowID - Dialog box window identifier
				Gadget - Identifier of the gadget
	Returns:	The resourse ID of the bitmap
	Purpose:	This function will get the bitmaps associated with a gadget.

********************************************************************************************/

ResourceID DialogManager::GetGadgetBitmap(CWindowID WindowID, CGadgetID Gadget)
{
	wxWindow* pGadget = GetGadget(WindowID, Gadget);
	if (!pGadget) return 0;

	if ( pGadget->IsKindOf(CLASSINFO(wxCamArtControl))
		)
	{
		return ((wxCamArtControl *)pGadget)->GetBitmapId();
	}
	return 0;
}

/********************************************************************************************

>	void DialogManager::SetGadgetBitmaps(CWindowID WindowID, CGadgetID Gadget,
										const CGadgetImageList& imagelist)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/Feb/2006
	Inputs:		WindowID - Dialog box window identifier
				Gadget - Identifier of the gadget
				imagelist - the list of images
	Purpose:	This function will set the bitmaps associated with a gadget.

********************************************************************************************/

void DialogManager::SetGadgetBitmaps(CWindowID WindowID, CGadgetID Gadget, const CGadgetImageList& imagelist)
{
	wxWindow* pGadget = GetGadget(WindowID, Gadget);
	if (!pGadget) return;

	if ( pGadget->IsKindOf(CLASSINFO(wxTreeCtrl))
		)
	{
		wxImageList* plist = new wxImageList(imagelist.GetWidth(), imagelist.GetHeight());

		CamArtProvider* pCAP = CamArtProvider::Get();
		ERROR3IF(pCAP==NULL, "Can't find CamArtProvider!");

		ListItem* pItem = NULL;
		ResourceID resID;
		pItem = imagelist.FindFirstBitmap(&resID);
		while (pItem)
		{
			wxBitmap* pBitmap = pCAP->FindBitmap(resID);
			plist->Add((const wxBitmap&) *pBitmap);

			pItem = imagelist.FindNextBitmap(pItem, &resID);
		}
		
		((wxTreeCtrl*)pGadget)->AssignImageList(plist);		// Tree control takes ownership of the list

		return;
	}

	ERROR3("SetTreeGadgetItem called on non-tree gadget");
	return;
}


/********************************************************************************************

>	void DialogManager::SetBitmapButtonIndexes(CWindowID WindowID, CGadgetID Gadget,
										UINT32 SelectedIndex, UINT32 UnselectedIndex)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/9/99
	Inputs:		WindowID - Dialog box window identifier
				Gadget - Identifier of the gadget
				SelectedIndex - The index number into the bitmap strip for the glyph
								representing the buttons selected state
				UnselectedIndex - The index number into the bitmap strip for
								  the unselected glyph
	Purpose:	This function allows you to specify bitmaps for both the
				selected and unselected states of a bitmap button. Most likely you have
				already selected your bitmap for the selected state in your
				resource file, however this allows you to specify the unselected state also.



********************************************************************************************/

void DialogManager::SetBitmapButtonIndexes(CWindowID WindowID, CGadgetID Gadget,
									UINT32 UnselectedIndex, UINT32 SelectedIndex)

{
	PORTNOTETRACE("dialog","DialogManager::SetBitmapButtonIndexes - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	// Find out about the Gadget
    HWND hGadget = GetDlgItem((HWND)WindowID, (INT32)Gadget);

	// Find out the class type of the gadget
	String_256 ClassNameStr;
	GetClassName(hGadget, (TCHAR*)ClassNameStr, 255);


	// Send new style messages to the button classes because they can use PNG
	// resources
	if (ClassNameStr == String_16(TEXT("cc_BitmapButton"))
		|| ClassNameStr == String_16(TEXT("cc_SmallBitmapButton")) )
	{
		SetGadgetBitmaps(hGadget, AfxGetResourceHandle(), SelectedIndex, UnselectedIndex);
	}
#endif
}

/********************************************************************************************

>	void DialogManager::SetGadgetBitmaps( HWND hGadget,
										  HINSTANCE hResInstance,
										  INT32 SelectedIndex = -1,
										  INT32 UnselectedIndex = -1 )

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/03/2004
	Inputs:		HWND - window handle of gadget
				HINSTANCE - Resource module handle
				INT32 - index of selected item or -1 to derive from gadget title string
				INT32 - index of unselected itemor -1 to derive from gadget title string
	Outputs:	-
	Returns:	-
	Purpose:	Send custom WM_SETBITMAPEX message to custom controls
	Errors:		-
	SeeAlso:	DialogManager::SetGadgetBitmaps, DialogManager::SetBitmapButtonIndexes

********************************************************************************************/
/*void DialogManager::SetGadgetBitmaps( wxWindow *pGadget,
									  // HINSTANCE hResInstance, //
									  INT32 SelectedIndex,
									  INT32 UnselectedIndex)
{
	PORTNOTETRACE("dialog","DialogManager::SetGadgetBitmaps - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	char AnsiTitle[256];
	GetWindowText(hGadget,AnsiTitle,255);
	BitmapButtonInfoEx BtnInfo;

	// find the position of the specifier's separator
	INT32 SeparatorPos = 0;
	while (AnsiTitle[SeparatorPos] != 0 && AnsiTitle[SeparatorPos] != '.')
		SeparatorPos++;

	INT32 Index = 0;		// Default to using icon index 0 if no '.' separator was found
	if (AnsiTitle[SeparatorPos] == '.')
	{
		// We found a separator. In that case, truncate the resource string at the '.'
		// so we can extract the bitmap name, and convert the text after the '.' into
		// an integer index.
		Index = _ttoi( AnsiTitle + SeparatorPos+1 );
		AnsiTitle[SeparatorPos] = 0;
	}

	if (SelectedIndex=-1) SelectedIndex = Index;
	if (UnselectedIndex=-1) UnselectedIndex = Index;

	// Fill in the BtnInfo structure
	BtnInfo.hAppInstance = hResInstance;

	String_256 Title =("L");
	Title += TEXT(AnsiTitle);

	String_256 Ansi32Title = AnsiTitle;
	Ansi32Title += ("32");

	String_256 Title32 = ("L");
	Title32 += TEXT(Ansi32Title);

	BtnInfo.Bitmap[BBX_BMP_LARGE_UNSELECTED] = (LPTSTR)Title;
	BtnInfo.Bitmap[BBX_BMP_SMALL_UNSELECTED] = (LPTSTR)AnsiTitle;

  	BtnInfo.Bitmap[BBX_BMP_SMALL_SELECTED] = NULL;	// NULL, or name of selected bitmap
	BtnInfo.Bitmap[BBX_BMP_LARGE_SELECTED] = NULL;	// NULL, or name of selected bitmap

	BtnInfo.Bitmap[BBX_BMP32_LARGE_UNSELECTED] = (LPTSTR)Title32;
	BtnInfo.Bitmap[BBX_BMP32_SMALL_UNSELECTED] = (LPTSTR)Ansi32Title;

	String_256 HTitle32 = ("H");					// "H" for highlighted = selected
	HTitle32 += Title32;
	String_256 HATitle32 = ("H");
	HATitle32 += Ansi32Title;

  	BtnInfo.Bitmap[BBX_BMP32_LARGE_SELECTED] = (LPTSTR)HTitle32;
	BtnInfo.Bitmap[BBX_BMP32_SMALL_SELECTED] = (LPTSTR)HATitle32;

	BtnInfo.UnselectedIndex = UnselectedIndex;		//  Indices of glyph bitmaps within the
	BtnInfo.SelectedIndex = SelectedIndex;			//  Unselected/Selected bitmaps

	::SendMessage(hGadget, WM_SETBITMAPEX, 0, ((LPARAM) &BtnInfo));
#endif
}
*/


/********************************************************************************************

>	BOOL DialogManager::SetUnitGadgetValue( CWindowID WindowID,
							                CGadgetID Gadget,
									     	UnitType DlgUnitType,
									    	MILLIPOINT Value,
									    	BOOL EndOfList = TRUE,
			     		 					INT32 ListPos = 0)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/8/93
	Inputs:		WindowID: 	 Dialog box window identifier
				Gadget: 	 Identifier of the gadget
				DlgUnitType: The type of the gadget value (cm, mm, pt, etc.)
				Value: 		 The value to set the gadget to in MILLIPOINTS

				The following inputs only need to be provided if the gadget has multiple
				values, for example a ListBox.

				EndOfList:	 TRUE if the value is to be a added to the end of the gadgets
					  		 value list. (Default = TRUE)
					  		 FALSE if the value is to be added to the ListPos position in the
					  		 list.

				ListPos:     If EndOfList = FALSE then this input specifies the position
							 in the list (Default = 0, so if you want to add the value to
							 the top of the list simply specify EndOfList = FALSE)

							 If the gadget is a ComboBox then setting ListPos = -1 will
							 set the combo box's edit gadget field.

	Outputs:	-
	Returns:	TRUE if the gadgets value could be set as specified
				FALSE if it could not.

	Purpose:	This method is used to set a gadget which should hold a number to a certain
				value. The actual type of the gadget does not matter.

				For example:

				If gadget g1 is a string then

				SetGadgetValue(g1win, g1, pt, 100000) will set g1's value to '100pt'

				If g1 is an integer field then the function would set the gadgets value
				to 100.

                For Windows
                -----------

                The function can be used to set the string values of the following controls. The
                string will have a unit suffix.

                Edit
                Static
                Button
                ListBox
                ComboBox

	Errors:		If the function is called on an invalid control then an ENSURE failure will
				occur in a DEBUG build. In a retail build FALSE is returned.

	SeeAlso:	DialogOp::SetUnitGadgetValue

********************************************************************************************/

BOOL DialogManager::SetUnitGadgetValue( CWindowID WindowID,
							        CGadgetID Gadget,
									UnitType Unit,
									MILLIPOINT Value,
									BOOL EndOfList,
			     		 			INT32 ListPos)
{
	// Convert the unit value into a string
	String_256		StrValue;
//	Convert::MillipointsToString(Value, Unit, 3, &StrValue);
	Convert::MillipointsToString( Value, Unit, &StrValue ); // Display using defaults
	return( SetStringGadgetValue( WindowID, Gadget, StrValue, EndOfList, ListPos ) );
}


/********************************************************************************************
>	BOOL DialogManager::SetDimensionUnitGadgetValue(CWindowID WindowID,
							            	    	CGadgetID Gadget,
											     	UnitType  units,
									    			double    Value,
													Node*     pNode,
			    							 		BOOL      IncludeUnitSpecifier = TRUE,
													BOOL      EndOfList = TRUE,
				     		 						INT32       ListPos = 0)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/10/95
	Inputs:		WindowID: 	 Dialog box window identifier
				Gadget: 	 Identifier of the gadget
				units:		 unit type
				Value: 		 The value to set the gadget to in MILLIPOINTS
				pNode:		 Ptr to the node from whence Value came from
				IncludeUnitSpecifier : 	If TRUE the unit specifier is included
										e.g. "1.27 cm" or "1.27" for TRUE or FALSE respectively

				The following inputs only need to be provided if the gadget has multiple
				values, for example a ListBox.

				EndOfList:	 TRUE if the value is to be a added to the end of the gadgets
					  		 value list. (Default = TRUE)
					  		 FALSE if the value is to be added to the ListPos position in the
					  		 list.

				ListPos:     If EndOfList = FALSE then this input specifies the position
							 in the list (Default = 0, so if you want to add the value to
							 the top of the list simply specify EndOfList = FALSE)

							 If the gadget is a ComboBox then setting ListPos = -1 will
							 set the combo box's edit gadget field.

	Returns:	FALSE if fails

	Purpose:	as SetDimensionGadgetValue() but you can specify the units in which it is displayed
********************************************************************************************/

BOOL DialogManager::SetDimensionUnitGadgetValue(CWindowID WindowID,
												CGadgetID Gadget,
												UnitType  units,
												double    Value,
												Node*     pNode,
												BOOL      IncludeUnitSpecifier,
												BOOL      EndOfList,
												INT32       ListPos)
{
	DimScale* pDimScale = DimScale::GetPtrDimScale(pNode);
	ERROR2IF(pDimScale==NULL,FALSE,"DialogManager::SetDimensionUnitGadgetValue() - pDimScale==NULL");

	String_256 Str;
	BOOL ok = pDimScale->ConvertToUnits(Value,&Str,IncludeUnitSpecifier,-1,units);
	if (ok) ok = SetStringGadgetValue(WindowID,Gadget,Str,EndOfList,ListPos);

	return ok;
}


/********************************************************************************************

>	BOOL DialogManager::SetLongGadgetValue(CWindowID WindowID,
										   CGadgetID Gadget,
										   INT32 Value,
										   BOOL EndOfList = TRUE,
			     						   INT32 ListPos = 0)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/9/93
	Inputs:		WindowID: Dialog box window identifier
				Gadget: Identifier of the gadget
				Value: 	  INT32 value

				The following inputs only need to be provided if the gadget has multiple
				values, for example a ListBox.

				EndOfList:	 TRUE if the value is to be a added to the end of the gadgets
					  		 value list. (Default = TRUE)
					  		 FALSE if the value is to be added to the ListPos position in the
					  		 list.
				ListPos:     If EndOfList = FALSE then this input specifies the position
							 in the list (Default = 0, so if you want to add the value to
							 the top of the list simply specify EndOfList = FALSE)

							 If the gadget is a ComboBox then setting ListPos = -1 will
							 set the combo box's edit gadget field.



	Outputs:	-
	Returns:	TRUE if the gadgets value could be set
				FALSE if it could not

	Purpose:	This function is used to set a numeric gadget value. It performs
				different functions depending on the type of the gadget.

				For Windows
				-----------

				If the gadget is a button it sets the button's current state.

					For check and radio buttons it sets the state to either checked
					(Value = 1) or unchecked (Value = 0). In the case of a 3 state button
					the third state can be specified by a (Value = 2).

					For Push buttons it can be used to set a depressed (Value = 1)/
					normal state(Value = 0)

				If the gadget is a scrollbar it sets the current position on the thumb

				The function can be used to set the values of the following controls.

                Edit
                Static
                Button
                ListBox
                ComboBox
				ScrollBar
				cc_CheckList
				cc_Slider

	Errors:		If the function is called on an invalid control then an ENSURE failure will
				occur in a DEBUG build. In a retail build FALSE is returned.

	SeeAlso:	DialogOp::SetLongGadgetValue

********************************************************************************************/

BOOL DialogManager::SetLongGadgetValue(CWindowID WindowID,
									   CGadgetID Gadget,
									   INT32 Value,
									   BOOL EndOfList,
			     		 			   INT32 ListPos)
{
	wxWindow * pGadget = GetGadget(WindowID, Gadget);
	if (!pGadget) return FALSE;

	if ( pGadget->IsKindOf(CLASSINFO(wxButton)) ||
		 pGadget->IsKindOf(CLASSINFO(wxBitmapButton)) )
	{
		// These bitmap buttons are meant to be tristate
		PORTNOTETRACE("dialog","DialogManager::SetLongGadgetValue on BitmapButton - do nothing");
		return FALSE;
	}

	// Oh if only virtual functions work here. Sadly, they don't, as SetValue is not in wxControl
	if ( pGadget->IsKindOf(CLASSINFO(wxCheckBox)) )
	{
		((wxCheckBox *)(pGadget))->SetValue(Value !=0);
		return TRUE;
	}

	if ( pGadget->IsKindOf(CLASSINFO(wxRadioButton)) )
	{
		((wxRadioButton *)(pGadget))->SetValue( Value != 0 );
		return TRUE;
	}

	if ( pGadget->IsKindOf(CLASSINFO(wxScrollBar)) )
	{
		((wxScrollBar *)(pGadget))->SetThumbPosition(Value);
		return TRUE;
	}

	if ( pGadget->IsKindOf(CLASSINFO(wxSlider)) )
	{
		((wxSlider *)(pGadget))->SetValue(Value);
		return TRUE;
	}

	if ( pGadget->IsKindOf(CLASSINFO(wxGauge)) )
	{
		((wxGauge *)(pGadget))->SetValue(Value);
		return TRUE;
	}

	if ( pGadget->IsKindOf(CLASSINFO(wxCamArtControl)) )
	{
		((wxCamArtControl *)(pGadget))->SetValue(Value);
		return TRUE;
	}


#if 0
	// it seems on an edit box we might be meant to set the garet, but it's difficult to know what's
	// going on here
	else if (ClassNameStr == String_16(TEXT("cc_CustomEdit")))//?
	{
		...
	}
#endif

	// Hmmm - no luck so far, let's try a string

	String_256 StrValue;
	// Convert Value to a string
	Convert::LongToString(Value, &StrValue);
	return(SetStringGadgetValue(WindowID, Gadget, StrValue, EndOfList, ListPos));
}

/********************************************************************************************

>	BOOL DialogManager::SetDoubleGadgetValue(CWindowID WindowID,
										   CGadgetID Gadget,
										   double Value,
										   BOOL EndOfList = TRUE,
			     						   INT32 ListPos = 0)

	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com> (liberally copied from Simon's SetLongGadgetValue)
	Created:	16/12/94
	Inputs:		WindowID: Dialog box window identifier
				Gadget: Identifier of the gadget
				Value: 	  double value

				The following inputs only need to be provided if the gadget has multiple
				values, for example a ListBox.

				EndOfList:	 TRUE if the value is to be a added to the end of the gadgets
					  		 value list. (Default = TRUE)
					  		 FALSE if the value is to be added to the ListPos position in the
					  		 list.
				ListPos:     If EndOfList = FALSE then this input specifies the position
							 in the list (Default = 0, so if you want to add the value to
							 the top of the list simply specify EndOfList = FALSE)

							 If the gadget is a ComboBox then setting ListPos = -1 will
							 set the combo box's edit gadget field.



	Outputs:	-
	Returns:	TRUE if the gadgets value could be set
				FALSE if it could not

	Purpose:	This function is used to set a numeric gadget value. It performs
				different functions depending on the type of the gadget.

				For Windows
				-----------

				If the gadget is a button it sets the button's current state.

					For check and radio buttons it sets the state to either checked
					(Value = 1) or unchecked (Value = 0). In the case of a 3 state button
					the third state can be specified by a (Value = 2).

					For Push buttons it can be used to set a depressed (Value = 1)/
					normal state(Value = 0)

				If the gadget is a scrollbar it sets the current position on the thumb

				The function can be used to set the values of the following controls.

                Edit
                Static
                Button
                ListBox
                ComboBox
				ScrollBar
				cc_CheckList
				cc_Slider

				Note most of these only take settings to integer precision so you might as
				well use SetLongGadgetValue instead (the exception being text fields of
				course).

	Errors:		If the function is called on an invalid control then an ENSURE failure will
				occur in a DEBUG build. In a retail build FALSE is returned.

	SeeAlso:	DialogOp::SetDoubleGadgetValue, DialogMgr::SetLongGadgetValue

********************************************************************************************/

BOOL DialogManager::SetDoubleGadgetValue(CWindowID WindowID,
									   CGadgetID Gadget,
									   double Value,
									   BOOL EndOfList,
			     		 			   INT32 ListPos)
{
	wxWindow * pGadget = GetGadget(WindowID, Gadget);
	if (!pGadget) return FALSE;

	if (( pGadget->IsKindOf(CLASSINFO(wxButton))) ||
		( pGadget->IsKindOf(CLASSINFO(wxBitmapButton))) ||
		( pGadget->IsKindOf(CLASSINFO(wxCheckBox))) ||
		( pGadget->IsKindOf(CLASSINFO(wxRadioButton))) ||
		( pGadget->IsKindOf(CLASSINFO(wxScrollBar))) ||
		( pGadget->IsKindOf(CLASSINFO(wxSlider))) ||
		( pGadget->IsKindOf(CLASSINFO(wxCamArtControl))) ||
		( pGadget->IsKindOf(CLASSINFO(wxGauge)))
		)
		return SetLongGadgetValue( WindowID, Gadget, (INT32)(floor(Value+0.5)), EndOfList, ListPos);

	String_256 StrValue;
	// Convert Value to a string
	Convert::DoubleToString(Value, &StrValue);
	return(SetStringGadgetValue(WindowID, Gadget, StrValue, EndOfList, ListPos));
}

/********************************************************************************************

>	BOOL DialogManager::SetStringGadgetValue(CWindowID WindowID,
								   	   		 CGadgetID Gadget,
								       		 UINT32 IDStr,
								       		 BOOL EndOfList = TRUE,
			     		 			   		 INT32 ListPos = 0)




	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/8/93
	Inputs:		WindowID: Dialog box window identifier
				Gadget: Gadget identifier
				StrID: 	  Resource ID of string

				The following inputs only need to be provided if the gadget has multiple
				values, for example a ListBox.

				EndOfList:	 TRUE if the value is to be a added to the end of the gadgets
					  		 value list. (Default = TRUE)
					  		 FALSE if the value is to be added to the ListPos position in the
					  		 list.

				ListPos:     If EndOfList = FALSE then this input specifies the position
							 in the list (Default = 0, so if you want to add the value to
							 the top of the list simply specify EndOfList = FALSE)

							 If the gadget is a ComboBox then setting ListPos = -1 will
							 set the combo box's edit gadget field.


	Outputs:	-

	Returns:	TRUE if the gadgets value could be set
				FALSE if it could not

	Purpose:	This function is used to set a gadgets value to a resource string. It can only
				be used on gadgets with a string value type.

				For Windows
				-----------

				This function can be used to set the string values of the following controls:

				Button
				Static
				Edit
				ListBox
				ComboBox


	Errors:		If the function is called on an invalid control then an ENSURE failure will
				occur in a DEBUG build. In a retail build FALSE is returned.

	SeeAlso:	DialogOp::SetStringGadgetValue

********************************************************************************************/

BOOL DialogManager::SetStringGadgetValue(CWindowID WindowID,
										 CGadgetID Gadget,
								         UINT32 IDStr,
								         BOOL EndOfList,
			     						 INT32 ListPos)
{
	String_256* StrVal = new String_256(IDStr); // Obtain the string resource
	ERROR1IF(StrVal == NULL, FALSE, _R(IDS_OUT_OF_MEMORY));

	// It is neccessary to store the new strings on the DiscardStrList so that
	// when the dialog is deleted the strings are also deleted.
	DlgDiscardString   *DiscardStr = new DlgDiscardString;
	if( DiscardStr == NULL )
	{
		delete StrVal;
		ERROR1(FALSE, _R(IDS_OUT_OF_MEMORY));
	}

	// If we are setting a string in a gadget on a page within a
	// property sheet then we need to store the windowID of the property sheet
	// This way it will get deleted when the property sheet is deleted in the
	// Delete method

	wxWindow		   *pCWnd = (wxWindow *)WindowID;
	wxWindow		   *WndID = pCWnd;
/*	if (pCWnd != NULL)
	{
		if (pCWnd->IsKindOf(RUNTIME_CLASS(OurPropShtPage)))
		{
			wxWindow* Parent = pCWnd->GetParent();
			ERROR2IF(Parent == NULL, FALSE, "Property page found without parent property sheet");
			ERROR2IF(!(Parent->IsKindOf(RUNTIME_CLASS(OurPropSheet))), FALSE,"Property page parent not a property sheet");
			WndID = Parent->GetSafeHwnd();
		}
	} */

	DiscardStr->DlgWindow = WndID;
	DiscardStr->pStr = StrVal;
	DiscardStrList.AddHead( DiscardStr );

	return (SetStringGadgetValue(WindowID, Gadget, *StrVal, EndOfList, ListPos));
}

/********************************************************************************************

>	static wxWindow * DialogManager::GetGadget(CWindowID WindowID, CGadgetID Gadget)

	Author:		Alex Bligh (alex@alex.org.uk)
	Created:	20/12/2005
	Inputs:		WindowID: Dialog box window identifier
				Gadget: Gadget identifier
				StrID: 	  Resource ID of string

********************************************************************************************/

wxWindow * DialogManager::GetGadget(CWindowID WindowID, CGadgetID Gadget)
{
	ERROR2IF(!WindowID || !WindowID->IsKindOf(CLASSINFO(wxWindow)), FALSE, "Bad Window ID passed");
	wxWindow * pGadget=WindowID->FindWindow(Gadget);
//	TRACEUSER("amb",_T("pwxDialog=0x%016llx Gadget=%d(%s) pGadget=0x%016llx"), WindowID, Gadget, CamResource::GetObjectName((ResourceID)Gadget), pGadget);
	if (!pGadget)
	{
		// Some dialogs seem to consciously do this, EG galleries
//		ERROR3_PF((_T("Bad Gadget ID %d(%s) passed"), Gadget, CamResource::GetObjectName((ResourceID)Gadget)));
		return NULL;
	}
#if 0
	const TCHAR * pGadgetClassName = (const TCHAR *) pGadget->GetClassInfo()->GetClassName();
	TRACEUSER("amb",_T("Gadget is a %s"),pGadgetClassName);
#endif
	return pGadget;
}

/********************************************************************************************

>	static OpDescriptor * DialogManager::GetGadgetOpDescritpor(CWindowID WindowID, CGadgetID Gadget);

	Author:		Alex Bligh (alex@alex.org.uk)
	Created:	20/12/2005
	Inputs:		WindowID: Dialog box window identifier
				Gadget: Gadget identifier
	Returns:	pointer to an OpDescriptor, or NULL if not found

********************************************************************************************/

OpDescriptor * DialogManager::GetGadgetOpDescriptor(CWindowID WindowID, CGadgetID Gadget)
{
	wxWindow * pWindow = GetGadget(WindowID, Gadget);
	if (!pWindow) return NULL;

	// If it's not a wxControl, return
	if (!pWindow->IsKindOf(CLASSINFO(wxControl)))
		return NULL;

	wxControl * pControl = (wxControl *)pWindow;
	return ControlList::Get()->Find(pControl);
}

/********************************************************************************************

>	BOOL DialogManager::SetStringGadgetValue(CWindowID WindowID,
										 CGadgetID Gadget,
								         StringBase* StrVal,
								         BOOL EndOfList,
			     						 INT32 ListPos)
{
	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/8/93
	Inputs:		WindowID: Dialog box window identifier
				Gadget: Gadget identifier
				StrID: 	  Resource ID of string


				The following inputs only need to be provided if the gadget has multiple
				values, for example a ListBox.

				EndOfList:	 TRUE if the value is to be a added to the end of the gadgets
					  		 value list. (Default = TRUE)
					  		 FALSE if the value is to be added to the ListPos position in the
					  		 list.

				ListPos:     If EndOfList = FALSE then this input specifies the position
							 in the list (Default = 0, so if you want to add the value to
							 the top of the list simply specify EndOfList = FALSE)

							 If the gadget is a ComboBox then setting ListPos = -1 will
							 set the combo box's edit gadget field.


	Outputs:	-
	Returns:	TRUE if the gadgets value could be set
				FALSE if it could not

	Purpose:	This function is used to set a gadgets string value. It can only
				be used on gadgets with a string value type.

	            For Windows
				-----------

				This function can be used to set the string values of the following controls:

				Button
				Static
				Edit
				ListBox
				ComboBox


	Errors:		If the function is called on an invalid control then an ENSURE failure will
				occur in a DEBUG build. In a retail build FALSE is returned.

	SeeAlso:	DialogOp::SetStringGadgetValue

********************************************************************************************/

BOOL DialogManager::SetStringGadgetValue(CWindowID WindowID,
										 CGadgetID Gadget,
								         const StringBase& StrVal,
								         BOOL EndOfList,
			     						 INT32 ListPos)
{
	wxWindow * pGadget = GetGadget(WindowID, Gadget);
	if (!pGadget) return FALSE;

	wxString String( (const TCHAR*)StrVal );

	//if ( pGadget->IsKindOf(CLASSINFO(wxControlWithItems)) ) // Includes wxListBox - this seems to have false positives
	if ( pGadget->IsKindOf(CLASSINFO(wxListBox)) ||
		 pGadget->IsKindOf(CLASSINFO(wxComboBox)) ||
		 pGadget->IsKindOf(CLASSINFO(wxChoice))
		)
	{
		if (EndOfList)
		{
			((wxControlWithItems *)pGadget)->Append(String);
			return TRUE;
		}

		if (pGadget->IsKindOf(CLASSINFO(wxComboBox)) && (pGadget->GetWindowStyle() & wxCB_SORT))
		{
			PORTNOTETRACE("dialog", "Mainting original order of combos displayed sorted is not supported");
			pGadget->SetWindowStyle(pGadget->GetWindowStyle() & ~wxCB_SORT);
		}

		if (pGadget->IsKindOf(CLASSINFO(wxListBox)) && (pGadget->GetWindowStyle() & wxLB_SORT))
		{
			PORTNOTETRACE("dialog", "Mainting original order of list boxes displayed sorted is not supported");
			pGadget->SetWindowStyle(pGadget->GetWindowStyle() & ~wxCB_SORT);
		}

		if (ListPos >=0)
		{
			if (
				(pGadget->IsKindOf(CLASSINFO(wxComboBox)) && (pGadget->GetWindowStyle() & wxCB_SORT)) ||
				(pGadget->IsKindOf(CLASSINFO(wxListBox)) && (pGadget->GetWindowStyle() & wxLB_SORT)) ||
				(pGadget->IsKindOf(CLASSINFO(wxChoice)))
				)
			{
				// Control does not support inserting at a position as it is sorted. We should append it and
				// it will appear in the right place
				((wxControlWithItems *)pGadget)->Append(String);
				return TRUE;
			}
			return (((wxControlWithItems *)pGadget)->Insert(String, ListPos) <0 )?FALSE:TRUE;
		}

		if (pGadget->IsKindOf(CLASSINFO(wxComboBox)))
		{
			((wxComboBox *)pGadget)->SetValue(String);
			return TRUE;
		}

		ERROR3("Attempt to insert item into edit field of gadget without one");
		return FALSE;
	}

	if ( pGadget->IsKindOf(CLASSINFO(wxTextCtrl)) )
	{
		((wxTextCtrl *)pGadget)->SetValue(String);
		return TRUE;
	}

	pGadget->SetLabel(String);

	return TRUE;
}



/********************************************************************************************

>	BOOL DialogManager::SetCustomComboGadgetValue(CWindowID WindowID,
												  CGadgetID Gadget,
												  CustomComboBoxControlDataItem* TheItem,
												  BOOL EndOfList,
			     								  INT32 ListPos)
{
	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com> S
	Created:	12/8/99
	Inputs:		WindowID: Dialog box window identifier
				Gadget: Gadget identifier
				TheItem:  The CustomComboBoxControlDataItem that is to be inserted


				EndOfList and ListPos do not actually do anything (as yet),
				BUT have been left a function arguments to avoid having to change
				too many function calls when converting calls on windows
				ComboBoxes into our new custom comboboxes!

				NOTES on ListPos value:

					>= 0		Insert the item into the control (only at end of list)
					== -1		Select the item with this index (TheItem should be
								equal to NULL)

	Outputs:	-
	Returns:	TRUE if the gadgets value could be set
				FALSE if it could not

	Purpose:	This function is used to set a CustomComboBoxes item (i.e.  basically
				insert another item)

	            For Windows
				-----------

				This function can be used to set items in the following controls:

				cc_1dBitmapComboBoxEdit
				cc_2dBitmapComboBoxEdit

	Errors:		If the function is called on an invalid control then an ENSURE failure will
				occur in a DEBUG build. In a retail build FALSE is returned.

	SeeAlso:	nothing else!

********************************************************************************************/

BOOL DialogManager::SetCustomComboGadgetValue(CWindowID WindowID,
											  CGadgetID Gadget,
											  CustomComboBoxControlDataItem* TheItem,
											  BOOL EndOfList,
			     							  INT32 ListPos)
{
	PORTNOTETRACE("dialog","DialogManager::SetCustomComboGadgetValue - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	String_256 ClassNameStr;  // The control type

	HWND hGadget = GetDlgItem((HWND)WindowID, (INT32)Gadget);
	// Find out the class type of the gadget
	GetClassName(hGadget, (TCHAR*)ClassNameStr, 255);

	if ((ClassNameStr == String_64(TEXT("cc_1dBitmapComboBoxEdit"))) ||
	    (ClassNameStr == String_64(TEXT("cc_2dBitmapComboBoxEdit")))   )
	{
		BOOL RetVal = FALSE;

		if (ListPos == 0) // Insert the string in the combo box's edit gadget
		{
			ASSERT (TheItem);

			RetVal = SendMessage (hGadget, WM_CCINSERTBITMAP, 0, (WPARAM) (CustomComboBoxControlDataItem*) TheItem);

		}
		else if (ListPos >= 1)		// but ListPos can be used to set the selected
									// item (other than tbe many/custom representations)
		{
			ASSERT (TheItem == NULL);

			RetVal = SendMessage (hGadget, CB_SETCURSEL, (WPARAM) (ListPos-1), (LPARAM) 0);
    	}
		else if ((ListPos == -1) || (ListPos == -2))
		{
			ASSERT (TheItem == NULL);

			RetVal = SendMessage (hGadget, CB_SETCURSEL, (WPARAM) (ListPos), (LPARAM) 0);
		}

		return (RetVal);
	}
	return (TRUE);
	ENSURE(FALSE, "Calling SetCustomComboGadgetValue for an invalid control");
#endif
	return FALSE;
}

/********************************************************************************************

>	BOOL DialogManager::SelectCustomComboGadgetValueOnString (CWindowID WindowID,
														      CGadgetID Gadget,
														      StringBase* StrVal)
{
	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com> S
	Created:	9/9/99
	Inputs:		WindowID: Dialog box window identifier
				Gadget: Gadget identifier
				StrVal:   The name of the item that is to be selected
	Outputs:	-
	Returns:	TRUE if the item could be selected
				FALSE if it could not

	Purpose:	This function is used to select an item (StrVal) within
				cc_2dBitmapComboBoxEdit custom comboboxes

	Errors:		If the function is called on an invalid control then an ENSURE failure will
				occur in a DEBUG build. In a retail build FALSE is returned.

	SeeAlso:	nothing else!

********************************************************************************************/

BOOL DialogManager::SelectCustomComboGadgetValueOnString (CWindowID WindowID,
														  CGadgetID Gadget,
														  StringBase* StrVal)
{
	PORTNOTETRACE("dialog","DialogManager::SelectCustomComboGadgetValueOnString - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	String_256 ClassNameStr;  // The control type

	HWND hGadget = GetDlgItem((HWND)WindowID, (INT32)Gadget);
	// Find out the class type of the gadget
	GetClassName(hGadget, (TCHAR*)ClassNameStr, 255);

	if( ClassNameStr == String_64( TEXT("cc_2dBitmapComboBoxEdit") ) )
	{
		BOOL RetVal = FALSE;

		RetVal = SendMessage (hGadget, CB_FINDSTRINGEXACT, (WPARAM) (-1), (INT32)(TCHAR*)(*StrVal));

		return (RetVal);
	}
	return (TRUE);
	ENSURE(FALSE, "Calling SelectCustomComboGadgetValueOnString for an invalid control");
#else
	return FALSE;
#endif
}



/********************************************************************************************

>	BOOL DialogManager::SetGadgetRange(CWindowID WindowID,
								  	   CGadgetID Gadget,
								       INT32 Min,
								       INT32 Max,
								       INT32 PageInc = 1)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/9/93
	Inputs:		WindowID: 		Dialog box window identifier
				Gadget:		Gadget identifier
	Outputs:	Min: 			Minimum range value
				Max:			Maximum range value
				PageInc:		For gadgets which allow their value to be increased or
								decreased by a fixed value other than 1.
								Eg. a scroll bar allows the user to move the scroll
								bar's thumb a 'page' at a time. PageInc specifies how many
	 							units the scroll bar's thumb should move.

	Returns:	-
	Purpose:	For setting the range of a gadget.

	            Fow Windows
	            -----------

				For scrollbar or trackbar gadgets this function sets the scrollbar's Min and Max
				values. The PageInc value specifies how many units the scroll bar should move
				when a PAGE message is received.

				This function can also be used to set the range of cc_Slider controls.

	Errors:		If the function is called on an invalid control then an ENSURE failure will
				occur in a DEBUG build. In a retail build FALSE is returned.

	SeeAlso:	DialogOp::SetGadgetRange

********************************************************************************************/


BOOL DialogManager::SetGadgetRange(CWindowID WindowID,
								   CGadgetID Gadget,
								   INT32 Min,
								   INT32 Max,
								   INT32 PgIncr)
{
	wxWindow * pGadget = GetGadget(WindowID, Gadget);
	if (!pGadget) return FALSE;

	if ( pGadget->IsKindOf(CLASSINFO(wxScrollBar)) )
	{
		wxScrollBar * pScroll = (wxScrollBar *)pGadget;
		ERROR3IF(Min !=0 , "Scrollbars with non-zero minimum need to be subclassed");
		pScroll->SetScrollbar(pScroll->GetThumbPosition(), pScroll->GetThumbSize(), Max-Min, PgIncr, TRUE);
		return TRUE;
	}

	if ( pGadget->IsKindOf(CLASSINFO(wxSlider)) )
	{
		((wxSlider *)(pGadget))->SetRange(Min, Max);
		return TRUE;
	}

	if ( pGadget->IsKindOf(CLASSINFO(wxGauge)) )
	{
		ERROR3IF(Min !=0 , "Gauges with non-zero minimum need to be subclassed");
		((wxGauge *)(pGadget))->SetRange(Max-Min);
		return TRUE;
	}

	return FALSE;
}



/********************************************************************************************

>	static BOOL BOOL DialogManager::SetListBoxSelection( CWindowID WindowID, CGadgetID Gadget,
								INT32 Index, BOOL SelectIt, BOOL SingleSelection )

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/10/94
	Inputs:		hGadget: 		Control window handle
				Index:			Index of value to be selected  (0..Number_of_Items - 1)
								OR -1 to {de}select all items in the gadget
				SelectIt:		TRUE to select the item
								FALSE to deselect the item
				SingleSelection:
								TRUE to set the selection state of a single item (clears
								any existing selection)
								FALSE to set selection in a multiple-selection box (does
								not affect the selection other than on this item). Note
								that if this is really a single-selection listbox, this
								flag is effectively always forced to TRUE.

	Returns:	TRUE if the value specified by Index could be selected, FALSE otherwise
	Purpose:	For selecting the Value specified by Index in a list Gadget.
				A subroutine used by the selection-setting DialogManager calls

	Scope:		private

	Errors:		If the function is called on an invalid control then an ERROR3 failure will
				occur in a DEBUG build.

********************************************************************************************/

BOOL DialogManager::SetListBoxSelection( CWindowID WindowID, CGadgetID Gadget, INT32 Index, BOOL SelectIt, BOOL SingleSelection )
{
	wxWindow * pGadget = GetGadget(WindowID, Gadget);
	if (!pGadget) return FALSE;

	if (pGadget->IsKindOf(CLASSINFO(wxListBox)))
	{
		if (SingleSelection)
		{
			((wxListBox *)pGadget)->SetSelection(wxNOT_FOUND);
			// It's not clear from the docs that that actually clears the selection in a list box (it
			// says "doesn't affect other selected items" so try this to be safe
			wxArrayInt sels;
			INT32 i;
			for (i=0; i<((wxListBox *)pGadget)->GetSelections(sels); i++)
				((wxListBox *)pGadget)->Deselect(sels[i]);
		}

		if (Index>=0)
		{
			if (SelectIt)
			{
				((wxListBox *)pGadget)->SetSelection(Index);
			}
			else
			{
				((wxListBox *)pGadget)->Deselect(Index);
			}
			return TRUE;
		}
		else
		{
			INT32 i;
			for (i=0; i<((wxListBox *)pGadget)->GetCount(); i++)
			{
				if (SelectIt)
				{
					((wxListBox *)pGadget)->SetSelection(i);
				}
				else
				{
					((wxListBox *)pGadget)->Deselect(i);
				}
			}			
		}
	}

	ERROR3("SetListBoxSelection called for non-listbox control");
	return(FALSE);
}


/********************************************************************************************

>	BOOL DialogManager::SetBoolGadgetSelected(CWindowID WindowID, CGadgetID Gadget,
												BOOL SelectIt, INT32 Index = 0)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/10/94
	Inputs:		WindowID: 		Dialog box window identifier
				Gadget:		Gadget identifier
				SelectIt:		TRUE to select, FALSE to deselect the control/item
				Index:			The index of the item within a combo or list box which
								you wish to (de)select, or -1 to (de)select all items
								in the list.
	Returns:	TRUE if it succeeded
	Purpose:	For setting the selection state of a gadget, or an item within a list gadget

	            For Windows
	            -----------
				Can be used to set the selection state for any on/off button type things
				Can be used on combo/list boxes to (de)select items. Note that if the listbox
				is a multiple selection box, setting the selection state of an item does NOT
				affect the state of any other items in the list (so to set only one item
				selected you must either deselect all before calling this function, or call
				SetSelectedGadgetIndex.

	Errors:		If the function is called on an invalid control then an ENSURE failure will
				occur in a DEBUG build. In a retail build FALSE is returned.

	SeeAlso:	DialogOp::SetBoolGadgetSelected

********************************************************************************************/

BOOL DialogManager::SetBoolGadgetSelected(CWindowID WindowID, CGadgetID Gadget,
											BOOL SelectIt, INT32 Index)
{
//	INT32 Count = -1;
	wxWindow * pGadget = GetGadget(WindowID, Gadget);
	if (!pGadget) return FALSE;

	if ( pGadget->IsKindOf(CLASSINFO(wxListBox)) ||
		 pGadget->IsKindOf(CLASSINFO(wxComboBox)) ||
		 pGadget->IsKindOf(CLASSINFO(wxChoice))
		)
	{	
		// Support listboxes with multiple selections
		if (pGadget->IsKindOf(CLASSINFO(wxListBox)))
		{
			return SetListBoxSelection(WindowID, Gadget, Index, SelectIt, FALSE);
		}
		
		if (Index>=0)
		{
			if (SelectIt)
			{
				((wxControlWithItems *)pGadget)->SetSelection(Index);
			}
			else
			{
				// We clear the current selection if it is current
				if (((wxControlWithItems *)pGadget)->GetSelection() == Index)
					((wxControlWithItems *)pGadget)->SetSelection(wxNOT_FOUND);
			}
			return TRUE;
		}
		else
		{
			if (SelectIt)
			{
				// Hmmm - we've been asked to set the selection on every item
				// in a control where only one thing can be selected. Give
				// it a go then..
				INT32 i;
				for (i=0; i<((wxControlWithItems *)pGadget)->GetCount(); i++)
					((wxControlWithItems *)pGadget)->SetSelection(i);
			}
			else
			{
				((wxControlWithItems *)pGadget)->SetSelection(wxNOT_FOUND);
			}
		}
	}

	// The following types are cannot be ticked /.unticked. OpDescriptors
	// do this then break the control setting
	if ( pGadget->IsKindOf(CLASSINFO(wxScrollBar))
		|| pGadget->IsKindOf(CLASSINFO(wxSlider)) 
		|| pGadget->IsKindOf(CLASSINFO(wxGauge))  )
		return TRUE;

	// Assume that we can set it via the SetLongGadgetValue function (radio buttons, etc)
	return(SetLongGadgetValue(WindowID, Gadget, SelectIt));
}


/********************************************************************************************

>	BOOL DialogManager::SetSelectedValueIndex(CWindowID WindowID,
			    			    		  CGadgetID Gadget,
						   				  INT32 Index)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/9/93
	Inputs:		WindowID: 		Dialog box window identifier
				Gadget:		Dialog identifier
				Index:			Index of value to be selected  (0..Number_of_Items - 1)
								OR -1 to deselect all values in the gadget (set no-selection)
	Outputs:	-
	Returns:	TRUE if the value specified by Index could be selected, FALSE otherwise
	Purpose:	For selecting the Value specified by Index in a list Gadget

				For Windows
				-----------

				The function can be called on ListBox and ComboBox gadgets to set the
				currently selected value

	Errors:		If the function is called on an invalid control then an ENSURE failure will
				occur in a DEBUG build. In a retail build FALSE is returned.

				NOTE that windows controls cannot be guaranteed to accept more than 32768
				items, as an (INT32) is used to pass the index. These controls generate an
				ENSURE failure at present if values outside the range -1 .. 32767 are given.
				In future, special CC controls for Camelot may not have this restriction.

	SeeAlso:	DialogOp::SetSelectedValueIndex

********************************************************************************************/

BOOL DialogManager::SetSelectedValueIndex(CWindowID WindowID,
			    			    		  CGadgetID Gadget,
						   				  INT32 Index)
{
	if (Index>0)
		return SetBoolGadgetSelected(WindowID, Gadget, TRUE, Index);
	return SetBoolGadgetSelected(WindowID, Gadget, FALSE, -1);
}



/********************************************************************************************

>	BOOL DialogManager::SetSelectedValueRange(CWindowID WindowID,
			    			    		  CGadgetID Gadget,
						   				  WORD StartIndex,
						   				  WORD EndIndex,
						   				  BOOL Selected)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/04/94
	Inputs:		WindowID: 		Dialog box window identifier
				Gadget:		Dialog identifier
				StartIndex:		Index of first value to be selected  (0..Number_of_Items - 1)
				EndIndex:		Index of last value to be selected  (0..Number_of_Items - 1)
				Selected:		Flag specifying whether range should be selected or not.
	Outputs:	-
	Returns:	TRUE if the value specified by Index could be selected, FALSE otherwise
	Purpose:	For selecting the Value specified by Index in a list Gadget

				For Windows
				-----------

				The function can be called on ListBox or cc_CheckList gadgets to set the
				currently selected value

	Errors:		If the function is called on an invalid control then an ENSURE failure will
				occur in a DEBUG build. In a retail build FALSE is returned.

	SeeAlso:	DialogOp::SetSelectedValueRange

********************************************************************************************/

BOOL DialogManager::SetSelectedValueRange(CWindowID WindowID,
			    			    		  CGadgetID Gadget,
						   				  WORD StartIndex,
						   				  WORD EndIndex,
						   				  BOOL Selected)
{
	INT32 i;
	for (i=StartIndex; i<=EndIndex; i++)
		SetBoolGadgetSelected(WindowID, Gadget, Selected, i);
	return (TRUE);
}

/********************************************************************************************

>	BOOL DialogManager::SetDimensionGadgetValue(CWindowID WindowID,
							                	CGadgetID Gadget,
									    		MILLIPOINT Value,
												Node* pNode,
		    							 		BOOL IncludeUnitSpecifier = TRUE,
												BOOL EndOfList = TRUE,
			     		 						INT32 ListPos = 0)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/6/94
	Inputs:		WindowID: 	 Dialog box window identifier
				Gadget: 	 Identifier of the gadget
				Value: 		 The value to set the gadget to in MILLIPOINTS
				pNode:		 Ptr to the node from whence Value came from
				IncludeUnitSpecifier : 	If TRUE the unit specifier is included
										e.g. "1.27 cm" or "1.27" for TRUE or FALSE respectively

				The following inputs only need to be provided if the gadget has multiple
				values, for example a ListBox.

				EndOfList:	 TRUE if the value is to be a added to the end of the gadgets
					  		 value list. (Default = TRUE)
					  		 FALSE if the value is to be added to the ListPos position in the
					  		 list.

				ListPos:     If EndOfList = FALSE then this input specifies the position
							 in the list (Default = 0, so if you want to add the value to
							 the top of the list simply specify EndOfList = FALSE)

							 If the gadget is a ComboBox then setting ListPos = -1 will
							 set the combo box's edit gadget field.

	Outputs:	-
	Returns:	TRUE if the gadgets value could be set as specified
				FALSE if it could not.

	Purpose:	This is the routine to call for displaying a node's dimension
				in a control.  It creates a string that represents 'Value' in the units
				the user wants, possibly scaled up/down if the user has specified a
				dimension scaling factor (e.g. 1 mile = 2 cm)
				Once the string is created, SetStringGadgetValue is called on the control,
				so you should be aware of this routine's features/limitations before calling
				SetDimensionGadgetValue.

	Errors:		If the function is called on an invalid control then an ENSURE failure will
				occur in a DEBUG build. In a retail build FALSE is returned.

	SeeAlso:	DialogOp::SetDimensionGadgetValue
	SeeAlso:	DialogManager::SetStringGadgetValue

********************************************************************************************/

BOOL DialogManager::SetDimensionGadgetValue( CWindowID WindowID,
 							                 CGadgetID Gadget,
									    	 MILLIPOINT Value,
											 Node* pNode,
		    							 	 BOOL IncludeUnitSpecifier,
									    	 BOOL EndOfList,
			     		 					 INT32 ListPos)
{
	DimScale		   *pDimScale = DimScale::GetPtrDimScale(pNode);
	String_256			Str;

	pDimScale->ConvertToUnits( Value, &Str, IncludeUnitSpecifier );
	SetStringGadgetValue( WindowID, Gadget, Str, EndOfList, ListPos );

	return TRUE;
}

/********************************************************************************************

>	BOOL DialogManager::SetMemoryGadgetValue( CWindowID WindowID,
											  CGadgetID Gadget,
									    	  UINT32 Value,
									    	  BOOL EndOfList = TRUE,
			     		 					  INT32 ListPos = 0)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/1/95
	Inputs:		WindowID: 	 Dialog box window identifier
				Gadget: 	 Identifier of the gadget
				Value: 		 The value to set the gadget to in bytes.

				The following inputs only need to be provided if the gadget has multiple
				values, for example a ListBox.

				EndOfList:	 TRUE if the value is to be a added to the end of the gadgets
					  		 value list. (Default = TRUE)
					  		 FALSE if the value is to be added to the ListPos position in the
					  		 list.

				ListPos:     If EndOfList = FALSE then this input specifies the position
							 in the list (Default = 0, so if you want to add the value to
							 the top of the list simply specify EndOfList = FALSE)

							 If the gadget is a ComboBox then setting ListPos = -1 will
							 set the combo box's edit gadget field.

	Outputs:	-
	Returns:	TRUE if the gadgets value could be set as specified
				FALSE if it could not.

	Purpose:	This method is used to set a gadget which should display an amount of memory
				being used or allocated. The value will be rounded to the nearest K or M or G
				byte value. The actual type of the gadget does not matter.

				For example:

				If gadget g1 is a string then

				SetMemoryGadgetValue(g1win, g1, 4*1024) will set g1's value to '4 Mbytes'

                For Windows
                -----------

                The function can be used to set the string values of the following controls.

                Edit
                Static
                Button
                ListBox
                ComboBox

	Errors:		If the function is called on an invalid control then an ENSURE failure will
				occur in a DEBUG build. In a retail build FALSE is returned.

	SeeAlso:	DialogOp::SetMemoryGadgetValue; DialogOp::GetMemoryGadgetValue;

********************************************************************************************/

BOOL DialogManager::SetMemoryGadgetValue( CWindowID WindowID,
										  CGadgetID Gadget,
										  UINT32 Value,
										  BOOL EndOfList,
			     		 				  INT32 ListPos)
{
	String_256 StrValue;
	Convert::BytesToString(&StrValue, Value);
	return (SetStringGadgetValue(WindowID, Gadget, StrValue, EndOfList, ListPos));
}


/********************************************************************************************

>	BOOL DialogManager::SetGadgetHelp( CWindowID WindowID,
			  						   CGadgetID Gadget,
						    		   UINT32 BubbleID,
						    		   UINT32 StatusID,
						    	   	   UINT32 ModuleID = 0)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/4/95
	Inputs:		WindowID: 	 Dialog box window identifier
				Gadget: 	 Identifier of the gadget

	Outputs:	-
	Returns:	TRUE if the gadgets value could be set as specified
				FALSE if it could not.

	Purpose:	Allows the Bubble and Status ID's of a control to be changed at runtime.

	Errors:		Return FALSE if the Gadget could not be found.

	SeeAlso:	DialogOp::SetMemoryGadgetValue; DialogOp::GetMemoryGadgetValue;

********************************************************************************************/

BOOL DialogManager::SetGadgetHelp( CWindowID WindowID,
		  						   CGadgetID Gadget,
					    		   UINT32 BubbleID,
					    		   UINT32 StatusID,
					    	   	   UINT32 ModuleID)
{
	PORTNOTE("dialog","DialogManager::SetGadgetHelp - do nothing")
#ifndef EXCLUDE_FROM_XARALX
	// Find the gadgets window
	HWND hGadget = GetDlgItem((HWND)WindowID, (INT32)Gadget);

	// Setup the Control help Info structure
	ControlHelpInfo Info;
	Info.BubbleID = BubbleID;
	Info.StatusID = StatusID;
	Info.ModuleID = ModuleID;

	// Tell the the control helper to update the ID's in it's table
	// We must also update any child window's entries.
	do
	{
		if (!ControlHelper::NotifyControlChanged(hGadget, &Info))
			return FALSE;

		hGadget = GetWindow(hGadget, GW_CHILD);
	}
	while (hGadget != NULL);
#endif
	return TRUE;
}


//-------------------------------------------------------------------------------------------


/********************************************************************************************

>	MILLIPOINT DialogManager::GetUnitGadgetValue(CWindowID WindowID,
				  		  					 CGadgetID Gadget,
				   		      				 UnitType DefaultType,
				         				     MILLIPOINT StartRange,
			           			     		 MILLIPOINT EndRange,
			          						 UINT32 IDSInvalidMsg,
			       	      					 BOOL* Valid)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/8/93
	Inputs:		WindowID: 		Dialog box window identifier
				Gadget:		Dialog identifier
				TheType: 		The type of the gadgets value
				StartRange: 	Minimum gadget value
				EndRange: 		Maximum gadget value
				InvalidMsgID: 	Message displayed to the user when they have entered
							  	invalid data (either not the correct type or not in the
							  	correct range)
								This may be 0 if you do not wish an error to be reported

	Outputs:	Valid:          Flag indicating if the value entered was valid or not.
								If the Valid parameter is NULL, then validity of the result
								will not be checked, and the actual value of the control
								will be returned even if it was out of range.
								If this flag is returned FALSE then the return value will be 0.

	Returns:    The value entered by the user in MILLIPOINTS. If Valid is FALSE then
				NULL will be returned.

	Purpose:	This function will obtain the gadget value and validate it. Validation will
				check that data has been entered in a correct unit type, and that it is
				in the range StartRange..EndRange. If the user enters an incorrect value
				the InvalidMsgID string will be displayed to the user in a dialog
				box, and Valid will have a FALSE value.

				For Windows
				-----------

				The function can be used to obtain a unit value from the string value of the
				following controls:

				Edit
				ListBox
				ComboBox
				Static

				For ListBox and ComboBox controls the unit value of the currently selected
				listitem is returned. This function would normally be called in response to a
				DIM_SELECTION_CHANGED or DIM_SELECTION_CHANGED_COMMIT message.

	Errors:		If the function is called on an invalid control then an ENSURE failure will
				occur in a DEBUG build. In a retail build FALSE is returned.

	SeeAlso:	DialogOp::GetUnitGadgetValue

********************************************************************************************/

MILLIPOINT DialogManager::GetUnitGadgetValue(CWindowID WindowID,
				  		  				 CGadgetID Gadget,
				   		      			 UnitType DefaultType,
				         				 MILLIPOINT StartRange,
			           			     	 MILLIPOINT EndRange,
			          					 UINT32 IDSInvalidMsg,
			       	      				 BOOL* Valid)
{
	MILLIPOINT			Value = 0; 	  // The return value

	BOOL IsValid;

	// Obtain the controls text
	String_256 StrValue = GetStringGadgetValue(WindowID, Gadget, NULL);

	// Convert the string to millipoints if it's valid
	Value = Convert::StringToMillipoints(StrValue, DefaultType, &IsValid);

	if (Valid == NULL)		// If don't want it validated, return the value now
		return(Value);

	if (IsValid)
	{
		// Check that the value is in the range StartRange..EndRange
		if ((Value >= StartRange) && (Value <= EndRange))
		{
			*Valid = TRUE;
			return (Value); // A correct value was entered
		}
	}

	// The value is invalid
	if (IDSInvalidMsg != 0)
		InformWarning(IDSInvalidMsg); // Scold the user, if a message was supplied

	return( 0 );
}

/********************************************************************************************

>	INT32 DialogManager::GetLongGadgetValue(CWindowID WindowID,
						   		   		   CGadgetID Gadget,
		       				       		   INT32 StartRange,
		            			   		   INT32 EndRange,
	           	  				   		   UINT32 IDSInvalidMsg,
	           	     			   		   BOOL* Valid,
	           	     			   		   DialogManager::PFNSTRINGTOINT32 pfnParser = ::StringToLong)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/9/93

	Inputs:		WindowID: 		Dialog box window identifier
				Gadget:		Dialog identifier
				StartRange: 	Minimum gadget value
				EndRange: 		Maximum gadget value
				InvalidMsgID: 	Message displayed to the user when they have entered
							  	invalid data (either not the correct type or not in the
							  	correct range)
								This may be 0 if you do not wish an error to be reported
				pfnParser		optional pointer to a function that converts a StringBase
								to a INT32.  The default is:-
									BOOL ::StringToLong(StringBase* in, INT32* out)
								but any function with a similar prototype will work.
								This function is called for controls that contain text,
								eg. a combo-box.  Ask JustinF about this if you aren't
								sure.

	Outputs:	Valid:          Flag indicating if the value entered was valid or not.
								If the Valid parameter is NULL, then validity of the result
								will not be checked, and the actual value of the control
								will be returned even if it was out of range.
								If this flag is returned FALSE then the return value will be 0.

	Returns:    The value entered by the user. If Valid is FALSE then
				NULL will be returned.

	Purpose:	This function will obtain the gadget value and validate it. Validation will
				check that a correct INT32 value has been entered, and that it is
				in the range StartRange..EndRange. If the user enters an incorrect value
				the InvalidMsgID string will be displayed to the user in a dialog
				box, and Valid will have a FALSE value.

				For Windows
				-----------

				The function can be used to obtain a INT32 value from the string value of the
				following controls:

				Edit
				ListBox
				ComboBox
				Static
				cc_CheckList
				Button

				For ListBox and ComboBox controls the INT32 value of the currently selected
				listitem is returned. This function would normally be called in response to a
				DIM_SELECTION_CHANGED or DIM_SELECTION_CHANGED_COMMIT message.

				You can override the conversion from the string value to the INT32 by
				providing your own pfnParser parameter.   This could, for example,
				remove percentage signs from the text before conversion.

				For Button controls this function returns back the current state of the button

				For ScrollBar controls this function returns back the current position of
				the scroll bars thumb.

				For cc_Slider controls this function returns the current position of the
				bar in the slider.

	Errors:		If the function is called on an invalid control then an ENSURE failure will
				occur in a DEBUG build. In a retail build FALSE is returned.

	SeeAlso:	DialogOp::GetLongGadgetValue

********************************************************************************************/

INT32 DialogManager::GetLongGadgetValue(CWindowID WindowID,
						   		   	   CGadgetID Gadget,
		       				       	   INT32 StartRange,
		            			   	   INT32 EndRange,
	           	  				   	   UINT32 IDSInvalidMsg,
	           	     			   	   BOOL* Valid,
	           	     			   	   DialogManager::PFNSTRINGTOINT32 pfnParser)
{
	wxWindow * pGadget = GetGadget(WindowID, Gadget);
	if (!pGadget) return FALSE;

	if ( pGadget->IsKindOf(CLASSINFO(wxButton)) ||
		 pGadget->IsKindOf(CLASSINFO(wxBitmapButton)) )
	{
		// These bitmap buttons are meant to be tristate
		PORTNOTETRACE("dialog","DialogManager::GetLongGadgetValue on BitmapButton - do nothing");
		return 0;
	}

	if ( pGadget->IsKindOf(CLASSINFO(wxCheckBox)) )
	{
		if (Valid) *Valid=TRUE;
		return ((wxCheckBox *)(pGadget))->GetValue() != 0;
	}

	if ( pGadget->IsKindOf(CLASSINFO(wxRadioButton)) )
	{
		if (Valid) *Valid=TRUE;
		return ((wxRadioButton *)(pGadget))->GetValue() != 0;
	}

	if ( pGadget->IsKindOf(CLASSINFO(wxScrollBar)) )
	{
		if (Valid) *Valid=TRUE;
		return (INT32)(((wxScrollBar *)(pGadget))->GetThumbPosition());
	}

	if ( pGadget->IsKindOf(CLASSINFO(wxSlider)) )
	{
		if (Valid) *Valid=TRUE;
		return (INT32)(((wxSlider *)(pGadget))->GetValue());
	}

	if ( pGadget->IsKindOf(CLASSINFO(wxGauge)) )
	{
		if (Valid) *Valid=TRUE;
		return (INT32)(((wxGauge *)(pGadget))->GetValue());
	}

	if ( pGadget->IsKindOf(CLASSINFO(wxCamArtControl)) )
	{
		if (Valid) *Valid=TRUE;
		return (INT32)(((wxCamArtControl *)(pGadget))->GetValue());
	}

#if 0
	// it seems on an edit box we might be meant to set the garet, but it's difficult to know what's
	// going on here
	else if (ClassNameStr == String_16(TEXT("cc_CustomEdit")))//?
	{
		CCustomEdit* pCEdit = (CCustomEdit*)CWnd::FromHandlePermanent(hGadget);
		Value = pCEdit->GetPos();
		IsValid = ((Value >= StartRange) && (Value <= EndRange));
		TRACEUSER( "Marc", _T("dlgmgr.cpp, getting pos %d\n"),Value);
	}
#endif

	// Hmmm - no luck so far, let's try a string

	// Obtain the controls text
	String_256 StrValue = GetStringGadgetValue(WindowID, Gadget, NULL);

	// Convert the string to an INT32 if it's valid
	INT32 Value=0;
	BOOL IsValid = (*pfnParser)(StrValue, &Value);
	if (IsValid)
	{
		IsValid = ((Value >= StartRange) && (Value <= EndRange));
	}

	if (Valid != NULL)
	{
		*Valid = IsValid;

		if (!IsValid) 	// The value is invalid
		{
			if (IDSInvalidMsg != 0)
				InformWarning(IDSInvalidMsg); // Scold the user, if a message was supplied
			return (0);
		}
	}

	return Value; // Valid input, or caller did not want validation
}


/********************************************************************************************

>	double DialogManager::GetDoubleGadgetValue(CWindowID WindowID,
						   		   		   CGadgetID Gadget,
		       				       		   double StartRange,
		            			   		   double EndRange,
	           	  				   		   UINT32 IDSInvalidMsg,
	           	     			   		   BOOL* Valid,
	           	     			   		   DialogManager::PFNSTRINGTODOUBLE pfnParser = ::StringToDouble)

	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com> (liberally copied from GetLongGadgetValue)
	Created:	16/12/94

	Inputs:		WindowID: 		Dialog box window identifier
				Gadget:		Dialog identifier
				StartRange: 	Minimum gadget value
				EndRange: 		Maximum gadget value
				InvalidMsgID: 	Message displayed to the user when they have entered
							  	invalid data (either not the correct type or not in the
							  	correct range)
								This may be 0 if you do not wish an error to be reported
				pfnParser		optional pointer to a function that converts a StringBase
								to a double.  The default is:-
									double ::StringToDouble(StringBase* in, Double* out)
								but any function with a similar prototype will work.
								This function is called for controls that contain text,
								eg. a combo-box.  Ask JustinF about this if you aren't
								sure.

	Outputs:	Valid:          Flag indicating if the value entered was valid or not.
								If the Valid parameter is NULL, then validity of the result
								will not be checked, and the actual value of the control
								will be returned even if it was out of range.
								If this flag is returned FALSE then the return value will be 0.

	Returns:    The value entered by the user. If Valid is FALSE then
				NULL will be returned.

	Purpose:	This function will obtain the gadget value and validate it. Validation will
				check that a correct double value has been entered, and that it is
				in the range StartRange..EndRange. If the user enters an incorrect value
				the InvalidMsgID string will be displayed to the user in a dialog
				box, and Valid will have a FALSE value.

				For Windows
				-----------

				The function can be used to obtain a double value from the string value of the
				following controls:

				Edit
				ListBox
				ComboBox
				Static
				cc_CheckList
				Button

				For ListBox and ComboBox controls the INT32 value of the currently selected
				listitem is returned. This function would normally be called in response to a
				DIM_SELECTION_CHANGED or DIM_SELECTION_CHANGED_COMMIT message.

				You can override the conversion from the string value to the double by
				providing your own pfnParser parameter.   This could, for example,
				remove percentage signs from the text before conversion.

				For Button controls this function returns back the current state of the button

				For ScrollBar controls this function returns back the current position of
				the scroll bars thumb.

				For cc_Slider controls this function returns the current position of the
				bar in the slider.

				Note for some controls, like sliders, the value comes back as an INT32 converted
				to a double so you might as well use GetLongGadgetValue.

	Errors:		If the function is called on an invalid control then an ENSURE failure will
				occur in a DEBUG build. In a retail build FALSE is returned.

	SeeAlso:	DialogOp::GetDoubleGadgetValue, DialogManager::GetLongGadgetValue

********************************************************************************************/

double DialogManager::GetDoubleGadgetValue(CWindowID WindowID,
						   		   	   CGadgetID Gadget,
		       				       	   double StartRange,
		            			   	   double EndRange,
	           	  				   	   UINT32 IDSInvalidMsg,
	           	     			   	   BOOL* Valid,
	           	     			   	   DialogManager::PFNSTRINGTODOUBLE pfnParser)
{
	wxWindow * pGadget = GetGadget(WindowID, Gadget);
	if (!pGadget) return FALSE;

	if (( pGadget->IsKindOf(CLASSINFO(wxButton))) ||
		( pGadget->IsKindOf(CLASSINFO(wxBitmapButton))) ||
		( pGadget->IsKindOf(CLASSINFO(wxCheckBox))) ||
		( pGadget->IsKindOf(CLASSINFO(wxRadioButton))) ||
		( pGadget->IsKindOf(CLASSINFO(wxScrollBar))) ||
		( pGadget->IsKindOf(CLASSINFO(wxSlider))) ||
		( pGadget->IsKindOf(CLASSINFO(wxGauge)))
		)
		return (double)GetLongGadgetValue( WindowID, Gadget, (INT32)(floor(StartRange+0.5)), (INT32)(floor(EndRange+0.5)), IDSInvalidMsg, Valid);

	String_256 StrValue = GetStringGadgetValue(WindowID, Gadget, NULL);

	// Convert the string to an INT32 if it's valid
	double Value=0.0;
	BOOL IsValid = (*pfnParser)(StrValue, &Value);
	if (IsValid)
	{
		IsValid = ((Value >= StartRange) && (Value <= EndRange));
	}

	if (Valid != NULL)
	{
		*Valid = IsValid;

		if (!IsValid) 	// The value is invalid
		{
			if (IDSInvalidMsg != 0)
				InformWarning(IDSInvalidMsg); // Scold the user, if a message was supplied
			return (0);
		}
	}

	return Value; // Valid input, or caller did not want validation
}

/********************************************************************************************

>	BOOL DialogManager::GetBoolGadgetSelected(CWindowID WindowID,
						   					   CGadgetID Gadget,
						   					   UINT32 IDSInvalidMsg = 0,
						   					   BOOL* Valid = NULL,
						   					   INT32 ListPos = 0)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/4/94

	Inputs:		WindowID: 		Dialog box window identifier
				Gadget:		Dialog identifier
				InvalidMsgID: 	Message displayed to the user when they have entered
							  	invalid data (either not the correct type or not in the
							  	correct range)
								This may be 0 if you do not wish an error to be reported
				ListPos:		Specifies which item in a list to read (defaults to 0)

	Outputs:	Valid:          Flag indicating if the value entered was valid or not.
								If the Valid parameter is NULL, then validity of the result
								will not be checked, and the actual value of the control
								will be returned even if it was "invalid".
								If this flag is returned FALSE then the return value will be 0.

	Returns:    The value entered by the user. If Valid is FALSE then
				FALSE will be returned.

	Purpose:	This function will obtain the gadget value and validate it. Validation will
				check that a correct INT32 value has been entered, and that it is
				in the range StartRange..EndRange. If the user enters an incorrect value
				the InvalidMsgID string will be displayed to the user in a dialog
				box, and Valid will have a FALSE value. [This does not seem correct - AMB]

				For Windows
				-----------

				The function can be used to obtain a BOOL value from the
				following controls:

				Button
				ListBox
				cc_CheckList

				For ListBox/cc_CheckList controls the bool value of the currently selected
				listitem is returned. This function would normally be called in response to a
				DIM_SELECTION_CHANGED or DIM_SELECTION_CHANGED_COMMIT message.

	Errors:		If the function is called on an invalid control then an ENSURE failure will
				occur in a DEBUG build. In a retail build FALSE is returned.

	SeeAlso:	DialogOp::GetLongGadgetValue

********************************************************************************************/

BOOL DialogManager::GetBoolGadgetSelected(CWindowID WindowID,
						   		   			CGadgetID Gadget,
	           	  				   			UINT32 IDSInvalidMsg,
	           	     			   			BOOL* Valid,
	           	     			   			INT32 ListPos)
{
	wxWindow * pGadget = GetGadget(WindowID, Gadget);
	if (!pGadget) return FALSE;

	if ( pGadget->IsKindOf(CLASSINFO(wxListBox)) ||
		 pGadget->IsKindOf(CLASSINFO(wxComboBox)) ||
		 pGadget->IsKindOf(CLASSINFO(wxChoice)) )
	{
		// Support listboxes with multiple selections
		if (pGadget->IsKindOf(CLASSINFO(wxListBox)))
		{
			return ((wxListBox *)pGadget)->IsSelected(ListPos);
		}

		return (((wxControlWithItems *)pGadget)->GetSelection() == ListPos);
	}

	return GetLongGadgetValue(WindowID, Gadget, 0, 1, IDSInvalidMsg, Valid);
}





/********************************************************************************************

>	String_256 DialogManager::GetStringGadgetValue(CWindowID WindowID,
												   CGadgetID Gadget,
												   BOOL* Valid,
												   INT32 ListPos = -1)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/9/93
	Inputs:		WindowID: 		Dialog box window identifier
				Gadget:		Gadget identifier
				ListPos:		Index into list if needed (defaults to -1 to mean read
								the text of the selected item. In the case of a combo
								this is the text in the editable field - which may be
								a value which does not exist in the combo's list.

	Outputs:	Valid:          Flag indicating if the string could be read or not
								You may pass NULL if you don't care about the validity
								(if invalid, the returned string will be a NULL string)

	Returns:	The gadgets string value if Valid is TRUE, else a NULL string

	Purpose:	For finding a gadgets string value. This function can only be used for
				gadgets with a text value.

				For Windows
				-----------

				This function returns the string value for the following controls

				Edit
				ListBox
				ComboBox
				Static
				Button

				For ListBox and ComboBox controls the string value of the currently selected
				listitem is returned. This function would normally be called in response to a
				DIM_SELECTION_CHANGED or DIM_SELECTION_CHANGED_COMMIT message.

	Errors:		If the function is called on an invalid control then an ENSURE failure will
				occur in a DEBUG build. In a retail build FALSE is returned.

	SeeAlso:	DialogOp::GetStringGadgetValue

********************************************************************************************/

String_256 DialogManager::GetStringGadgetValue(CWindowID WindowID,
											   CGadgetID Gadget,
											   BOOL* Valid,
											   INT32 ListPos)
{
	if (Valid)
		*Valid = TRUE;
	String_256 StrVal;

	wxWindow * pGadget = GetGadget(WindowID, Gadget);
	wxString String;

	if (!pGadget) goto invalid;

//	if ( pGadget->IsKindOf(CLASSINFO(wxControlWithItems)) ) // Includes wxListBox - this seems to have false positives
	if ( pGadget->IsKindOf(CLASSINFO(wxListBox)) ||
		 pGadget->IsKindOf(CLASSINFO(wxComboBox)) ||
		 pGadget->IsKindOf(CLASSINFO(wxChoice)) )
	{
		if (ListPos >=0)
		{
			if (ListPos>=((wxControlWithItems *)pGadget)->GetCount()) goto invalid;
			String = ((wxControlWithItems *)pGadget)->GetString(ListPos);
			goto out;
		}

		if (pGadget->IsKindOf(CLASSINFO(wxComboBox)))
		{
			String = ((wxComboBox *)pGadget)->GetValue();
			goto out;
		}

		INT32 sel = ((wxControlWithItems *)pGadget)->GetSelection();
		if ( (sel == wxNOT_FOUND) || (sel < 0)) goto invalid;
		String = ((wxControlWithItems *)pGadget)->GetString(sel);
		goto out;
	}

	if ( pGadget->IsKindOf(CLASSINFO(wxTextCtrl)) )
	{
		String = ((wxTextCtrl *)pGadget)->GetValue();
		goto out;
	}

	String = pGadget->GetLabel();

out:
	StrVal = String.c_str();
	return StrVal;

invalid:
	if (Valid)
		*Valid=FALSE;
	return StrVal;
}

/********************************************************************************************

>	BOOL DialogManager::GetGadgetRange(CWindowID WindowID,
								  	   CGadgetID Gadget,
								       INT32* Min,
								       INT32* Max)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/9/93
	Inputs:		WindowID: 		Dialog box window identifier
				Gadget:		Gadget identifier
	Outputs:	Min: 			Minimum range value
				Max:			Maximum range value

	Returns:	TRUE if the gadget's range could be read, FALSE otherwise

	Purpose:	For obtaining the range of a gadget.

	            For Windows
	            -----------

	            The function returns a ScrollBar control's Min and Max values.

	Errors:		If the function is called on an invalid control then an ENSURE failure will
				occur in a DEBUG build. In a retail build FALSE is returned.

	SeeAlso:	DialogOp::GetGadgetRange

********************************************************************************************/


BOOL DialogManager::GetGadgetRange(CWindowID WindowID,
								   CGadgetID Gadget,
								   INT32* Min,
								   INT32* Max)
{
	INT32 min=0;
	INT32 max=0;

	wxWindow * pGadget = GetGadget(WindowID, Gadget);
	if (!pGadget) return FALSE;

	if ( pGadget->IsKindOf(CLASSINFO(wxScrollBar)) )
	{
		max = ((wxScrollBar *)pGadget)->GetRange();
	}

	if ( pGadget->IsKindOf(CLASSINFO(wxSlider)) )
	{
		min=((wxSlider *)(pGadget))->GetMin();
		max=((wxSlider *)(pGadget))->GetMin();
	}

	if ( pGadget->IsKindOf(CLASSINFO(wxGauge)) )
	{
		max = ((wxGauge *)(pGadget))->GetRange();
	}

	if (Min) *Min=min;
	if (Max) *Max=max;

	return TRUE;
}


/********************************************************************************************

>	MILLIPOINT DialogManager::GetDimensionGadgetValue(  CWindowID WindowID,
							                			CGadgetID Gadget,
														Node* pNode,
														BOOL* Valid)
											   			INT32 ListPos = -1)
	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/6/94
	Inputs:		WindowID: 	 Dialog box window identifier
				Gadget: 	 Identifier of the gadget
				pNode:		 The node the dimenstion is for
	Outputs:	Valid - TRUE if a valid dimension has been read from the control
				Valid may be a NULL pointer if you are uninterested in the result

	Returns:	The internal millipoint representation of the string in this control
	Purpose:	This is the routine to call for getting user-entered dimensions from
				a control.  The value is scaled from user dimensions to an internal millipoint
				value.
				The routine uses DialogManager::GetStringGadgetValue to extract the string from
				the control.  The caller should be aware of the features/limitations of this routine
				before calling GetDimensionGadgetValue.

	Errors:		If the function is called on an invalid control then an ENSURE failure will
				occur in a DEBUG build. In a retail build FALSE is returned.

	SeeAlso:	DialogOp::GetDimensionGadgetValue
	SeeAlso:	DialogManager::GetStringGadgetValue

********************************************************************************************/

MILLIPOINT DialogManager::GetDimensionGadgetValue(  CWindowID WindowID,
							                		CGadgetID Gadget,
													Node* pNode,
													BOOL* Valid,
												    INT32 ListPos)
{
	MILLIPOINT Val=72000;
	DimScale* pDimScale = DimScale::GetPtrDimScale(pNode);
	String_256 Str;

	Str = GetStringGadgetValue(WindowID,Gadget,Valid,ListPos);
	if (Valid != NULL && *Valid)
		*Valid = pDimScale->ConvertToMillipoints(Str,&Val);
	return Val;
}


/********************************************************************************************
BOOL DialogManager::GetDoubleAndUnitGadgetValue(double*   pMPValue,
												double*   pUnitValue,
												UnitType* pUnitType,
								 				CWindowID WindowID,
								 				CGadgetID Gadget,
								 				Node*     pNode)
	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/9/95
	Inputs:		WindowID   -
				Gadget   -
				pNode      - node from which to determine default units (ie page units)
	Outputs:	pMPValue   - value of gadget converted to millipoints (accounting for scaling)
				pUnitValue - value of gadget in terms of units output in pUnitType
				pUnitType  - type of unit the value was specified in (or page units if none)
	Returns:	FALSE if fails
	Purpose:	Read the value from a gadget as a double millipont values accounting for unit scaling
				ALSO read the type of unit specified and the gadget value in terms of these units
				ie 0.5m would return 36000.0, 0.5 and METERS (assuming scaling 1m->1in)
********************************************************************************************/

BOOL DialogManager::GetDoubleAndUnitGadgetValue(double*   pMPValue,
												double*   pUnitValue,
												UnitType* pUnitType,
								 				CWindowID WindowID,
								 				CGadgetID Gadget,
								 				Node*     pNode)
{
	ERROR2IF(  pMPValue==NULL,FALSE,"DialogManager::GetDoubleAndUnitGadgetValue() - pMPValue==NULL");
	ERROR2IF(pUnitValue==NULL,FALSE,"DialogManager::GetDoubleAndUnitGadgetValue() - pUnitValue==NULL");
	ERROR2IF( pUnitType==NULL,FALSE,"DialogManager::GetDoubleAndUnitGadgetValue() - pUnitType==NULL");
	ERROR2IF(     pNode==NULL,FALSE,"DialogManager::GetDoubleAndUnitGadgetValue() - pNode==NULL");
	DimScale* pDimScale = DimScale::GetPtrDimScale(pNode);
	ERROR2IF( pDimScale==NULL,FALSE,"DialogManager::GetDoubleAndUnitGadgetValue() - pDimScale==NULL");

	// separate string into double value and unit type
	BOOL     ok			= TRUE;
	double   UnitValue	= 1.0;
	UnitType units;
	String_256 GadgetString=GetStringGadgetValue(WindowID,Gadget,&ok);
	if (ok) ok=Convert::StringToComponents(GadgetString, &UnitValue, &units);
	if (!ok) return FALSE;

	// if no unit type specified, use units associated with the specified node
	if (units==NOTYPE)
		units=pDimScale->GetUnits();

	// and get the value in millipoints accounting for scaled units
	double MPValue = 1.0;
	ok=pDimScale->ConvertToDouble(GadgetString,&MPValue);
	if (!ok) return FALSE;

	// set outputs and return
	*pMPValue   = MPValue;
	*pUnitValue = UnitValue;
	*pUnitType  = units;

	return TRUE;
}


/********************************************************************************************

>	UINT32 DialogManager::GetMemoryGadgetValue(CWindowID WindowID,
				  		  					  CGadgetID Gadget,
				         					  UINT32 StartRange,
			           			     		  UINT32 EndRange,
			          						  UINT32 IDSInvalidMsg,
			       	      					  BOOL* Valid)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/1/95
	Inputs:		WindowID: 		Dialog box window identifier
				Gadget:		Dialog identifier
				StartRange: 	Minimum gadget value
				EndRange: 		Maximum gadget value
				InvalidMsgID: 	Message displayed to the user when they have entered
							  	invalid data (either not the correct type or not in the
							  	correct range)
								This may be 0 if you do not wish an error to be reported

	Outputs:	Valid:          Flag indicating if the value entered was valid or not.
								If the Valid parameter is NULL, then validity of the result
								will not be checked, and the actual value of the control
								will be returned even if it was out of range.
								If this flag is returned FALSE then the return value will be 0.

	Returns:    The value entered by the user in bytes. If Valid is FALSE then
				NULL will be returned.

	Purpose:	This function allows a memory value to be read back from a control. It will
				cope with the memory being specified in bytes, K, M or G bytes and will also
				validate it. Validation will check that data has been entered in a correct unit
				type, and that it is in the range StartRange..EndRange. If the user enters an
				incorrect value	the InvalidMsgID string will be displayed to the user in a
				dialog box, and Valid will have a FALSE value.

				For Windows
				-----------

				The function can be used to obtain a memory value from the string value of the
				following controls:

				Edit
				ListBox
				ComboBox
				Static

				For ListBox and ComboBox controls the unit value of the currently selected
				listitem is returned. This function would normally be called in response to a
				DIM_SELECTION_CHANGED or DIM_SELECTION_CHANGED_COMMIT message.

	Errors:		If the function is called on an invalid control then an ENSURE failure will
				occur in a DEBUG build. In a retail build FALSE is returned.

	SeeAlso:	DialogOp::GetMemoryGadgetValue; DialogOp::SetMemoryGadgetValue

********************************************************************************************/

UINT32 DialogManager::GetMemoryGadgetValue(CWindowID WindowID,
				  		  				  CGadgetID Gadget,
										  UINT32 StartRange,
			           			     	  UINT32 EndRange,
			          					  UINT32 IDSInvalidMsg,
			       	      				  BOOL* Valid)
{
	BOOL IsValid;

	// Obtain the controls text
	String_256 StrValue = GetStringGadgetValue(WindowID, Gadget, NULL);

	// Convert the string to millipoints if it's valid
	UINT32 Value = Convert::StringToBytes(StrValue, &IsValid);

	if (Valid == NULL)		// If don't want it validated, return the value now
		return(Value);

	if (IsValid)
	{
		// Check that the value is in the range StartRange..EndRange
		if ((Value >= StartRange) && (Value <= EndRange))
		{
			*Valid = TRUE;
			return (Value); // A correct value was entered
		}
	}

	// The value is invalid
	if (IDSInvalidMsg != 0)
		InformWarning(IDSInvalidMsg); // Scold the user, if a message was supplied

	return( 0 );
}

//-------------------------------------------------------------------------------------------

/********************************************************************************************

>	BOOL DialogManager::DeleteAllValues(CWindowID WindowID, CGadgetID Gadget)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/5/94
	Inputs:		WindowID: 		Dialog box window identifier
				Gadget:		Gadget identifier

	Returns:	TRUE if the values could be deleted, else FALSE.
	Purpose:	For deleting all values in a list-gadget

				For Windows
				-----------

				This function can be used to delete list items from ComboBox, ListBox,
				or cc_ListBox controls. ALL items in the given Gadget will be deleted
				(i.e. it resets	the list to containing no items at all. This is equivalent
				to calling DeleteValue for each list item in turn)

	Errors:		If the function is called on an invalid control then an ENSURE failure will
				occur in a DEBUG build. In a retail build FALSE is returned.

	SeeAlso:	DialogOp::DeleteAllValues; DialogManager::DeleteValue

********************************************************************************************/

BOOL DialogManager::DeleteAllValues(CWindowID WindowID, CGadgetID Gadget)
{
	wxWindow * pGadget = GetGadget(WindowID, Gadget);
	if (!pGadget) return FALSE;

	//if ( pGadget->IsKindOf(CLASSINFO(wxControlWithItems)) ) // Includes wxListBox - this seems to have false positives
	if ( pGadget->IsKindOf(CLASSINFO(wxListBox)) ||
		 pGadget->IsKindOf(CLASSINFO(wxComboBox)) ||
		 pGadget->IsKindOf(CLASSINFO(wxChoice))
		)
	{
		((wxControlWithItems *)pGadget)->Clear();
		return TRUE;
	}
	else if ( pGadget->IsKindOf(CLASSINFO(wxTreeCtrl))
		)
	{
		((wxTreeCtrl*)pGadget)->DeleteAllItems();
		return TRUE;
	}

	ERROR3("Invalid control");
	return FALSE;
}



/********************************************************************************************

>	BOOL DialogManager::DeleteValue(CWindowID WindowID,
					 				CGadgetID Gadget,
					 				BOOL EndOfList = TRUE,
					 				INT32 ListPos = 0)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/9/93
	Inputs:		WindowID: 		Dialog box window identifier
				Gadget:		Gadget identifier

				The following inputs only need to be provided if the gadget has multiple
				values, for example a ListBox.

				EndOfList:	 TRUE if the value is to be deleted from the end of the gadgets
					  		 value list. (Default = TRUE)

				ListPos:     If EndOfList = FALSE then this input specifies the position
							 in the list (Default = 0, so if you want to delete the value at
							 the top of the list simply specify EndOfList = FALSE)

	Returns:	TRUE if the value could be deleted, else FALSE.
	Purpose:	For deleting a gadget value

				For Windows
				-----------

				This function can be used to delete list items from ListBox or ComboBox
				controls.

	Errors:		If the function is called on an invalid control then an ENSURE failure will
				occur in a DEBUG build. In a retail build FALSE is returned.

	SeeAlso:	DialogOp::DeleteValue

********************************************************************************************/

BOOL DialogManager::DeleteValue(CWindowID WindowID,
				 				CGadgetID Gadget,
				 				BOOL EndOfList,
				 				INT32 ListPos)
{
	wxWindow * pGadget = GetGadget(WindowID, Gadget);
	if (!pGadget) return FALSE;

	//if ( pGadget->IsKindOf(CLASSINFO(wxControlWithItems)) ) // Includes wxListBox - this seems to have false positives
	if ( pGadget->IsKindOf(CLASSINFO(wxListBox)) ||
		 pGadget->IsKindOf(CLASSINFO(wxComboBox)) ||
		 pGadget->IsKindOf(CLASSINFO(wxChoice))
		)
	{
		if (EndOfList)
		{
			((wxControlWithItems *)pGadget)->Delete(((wxControlWithItems *)pGadget)->GetCount()-1);
		}
		else
		{
			((wxControlWithItems *)pGadget)->Delete(ListPos);
		}
		return TRUE;
	}
	ERROR3("Invalid control");
	return FALSE;
}

/********************************************************************************************

>	BOOL GetValueCount(CWindowID WindowID,
				   	   CGadgetID Gadget,
				   	   INT32* Count)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/9/93
	Inputs:		WindowID: 		Dialog box window identifier
				Gadget:		Gadget identifier
	Outputs:	Count:			The number of items in the gadget

	Returns:	FALSE if an error occurred

	Purpose:	For finding the number of values stored in a gadget

	            For Windows
	            -----------

				This function can be called on ListBox and ComboBox controls to obtain the
				number of values in their lists.

	Errors:		If the function is called on an invalid control then an ENSURE failure will
				occur in a DEBUG build. In a retail build FALSE is returned.

	SeeAlso:	DialogOp::GetValueCount

********************************************************************************************/


BOOL DialogManager::GetValueCount(CWindowID WindowID,
				   				  CGadgetID Gadget,
				 				  INT32* Count)
{
	wxWindow * pGadget = GetGadget(WindowID, Gadget);
	if (!pGadget) return FALSE;

	//if ( pGadget->IsKindOf(CLASSINFO(wxControlWithItems)) ) // Includes wxListBox - this seems to have false positives
	if ( pGadget->IsKindOf(CLASSINFO(wxListBox)) ||
		 pGadget->IsKindOf(CLASSINFO(wxComboBox)) ||
		 pGadget->IsKindOf(CLASSINFO(wxChoice))
		)
	{
		INT32 c = ((wxControlWithItems *)pGadget)->GetCount();
		if (Count) *Count=c;
		return TRUE;
	}
	ERROR3("Invalid control");
	return FALSE;
}

/********************************************************************************************

>	INT32 DialogManager::GetSelectedCount(CWindowID WindowID,CGadgetID Gadget, INT32* Count)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/10/95
	Inputs:		WindowID = ID of window that contains the gadget
				Gadget = ID of list-type gadget
	Returns:	The number of selected items in the list-type gadget, or -1 if it fails
	Purpose:	Returns the number of selected items in a list-type gadget
	SeeAlso:	-

********************************************************************************************/

INT32 DialogManager::GetSelectedCount(CWindowID WindowID,CGadgetID Gadget)
{
//	INT32 Count = -1;
	wxWindow * pGadget = GetGadget(WindowID, Gadget);
	if (!pGadget) return FALSE;

	if ( pGadget->IsKindOf(CLASSINFO(wxListBox)) ||
		 pGadget->IsKindOf(CLASSINFO(wxComboBox)) ||
		 pGadget->IsKindOf(CLASSINFO(wxChoice))
		)
	{
		// Support listboxes with multiple selections
		if (pGadget->IsKindOf(CLASSINFO(wxListBox)))
		{
			wxArrayInt sels;
			return ((wxListBox *)pGadget)->GetSelections(sels);
		}

		return (((wxControlWithItems *)pGadget)->GetSelection() == wxNOT_FOUND)?0:1;

	}

	return -1;
}

/********************************************************************************************

>	INT32 DialogManager::GetFirstSelectedItem(CWindowID WindowID, CGadgetID Gadget)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/10/95
	Inputs:		WindowID = ID of window that contains the gadget
				Gadget = ID of list-type gadget
	Returns:	The index of the firsted selected items in the list-type gadget, or -1 if it fails
	Purpose:	Returns the index of the first selected item in a list-type gadget
	SeeAlso:	-

********************************************************************************************/

INT32 DialogManager::GetFirstSelectedItem( CWindowID WindowID, CGadgetID Gadget )
{
	wxWindow * pGadget = GetGadget(WindowID, Gadget);
	if (!pGadget) return -1;

	if ( pGadget->IsKindOf(CLASSINFO(wxListBox)) ||
		 pGadget->IsKindOf(CLASSINFO(wxComboBox)) ||
		 pGadget->IsKindOf(CLASSINFO(wxChoice)) )
	{
		// Support listboxes with multiple selections
		if (pGadget->IsKindOf(CLASSINFO(wxListBox)))
		{
			wxArrayInt sels;
			if ( ((wxListBox *)pGadget)->GetSelections(sels) )
			{
				return sels[0];
			}
			return -1;
		}

		return ((wxControlWithItems *)pGadget)->GetSelection();

	}

	return -1;
}

/********************************************************************************************

>	INT32* DialogManager::GetSelectedItems(CWindowID WindowID, CGadgetID Gadget)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/10/95
	Inputs:		WindowID = ID of window that contains the gadget
				Gadget = ID of list-type gadget
	Returns:	ptr to the INT32 array, or NULL if it fails
	Purpose:	Returns a ptr to an INT32 array that holds the list of selected indexes.
				The last array entry contains -1.

				NULL is returned if there's no selection, or not enough memory for the required array

				The caller is responsible for deleting the array that's returned.
				e.g
					INT32* pArray = GetSelectedItems(WindowID,Gadget);
					if (pArray != NULL)
					{
						..... // Use the array
						delete [] pArray;
					}
	SeeAlso:	-

********************************************************************************************/

INT32* DialogManager::GetSelectedItems(CWindowID WindowID, CGadgetID Gadget)
{
	INT32* pList = NULL;

	wxArrayInt sels;

	wxWindow * pGadget = GetGadget(WindowID, Gadget);
	if (!pGadget) return NULL;

	if ( pGadget->IsKindOf(CLASSINFO(wxListBox)) ||
		 pGadget->IsKindOf(CLASSINFO(wxComboBox)) ||
		 pGadget->IsKindOf(CLASSINFO(wxChoice)) )
	{
		// Support listboxes with multiple selections
		if (pGadget->IsKindOf(CLASSINFO(wxListBox)))
		{
			((wxListBox *)pGadget)->GetSelections(sels);
		}
		else
		{
			sels[0]=((wxControlWithItems *)pGadget)->GetSelection();
		}

		size_t Count = sels.GetCount();

		pList = new INT32[Count+1];

		if (pList)
		{
			UINT32 i;
			for (i=0; i<Count; i++)
			{
				pList[i]=sels[i];
			}
			pList[Count] = -1;	// terminate the list
		}

		return pList; // may be NULL if no memory

	}
	return NULL;
}

/********************************************************************************************

>	BOOL DialogManager::GetValueIndex(CWindowID WindowID,
				 	   				  CGadgetID Gadget,
				  	   				  INT32* Index)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com> / Jason (Based on Simon's stuff...)
	Created:	11/10/95
	Inputs:		WindowID: 		Dialog box window identifier
				Gadget:		Gadget identifier

    Outputs:	Index: 			The index of the currently selected item in the gadget. The
    							index value starts at 0 for the first item in the list.

								THIS CAN NOW RETURN -1 AS IT SHOULD ALWAYS OF BEEN ABLE TO DO !!!

	Returns:	TRUE if the index value could be read, else FALSE.

	Purpose:	For finding the index of the currently selected item in a gadget

				For Windows
				-----------

				The function can be called for ListBox and ComboBox controls

	Errors:		If the function is called on an invalid gadget then an ENSURE failure will
				occur in a DEBUG build. In a retail build FALSE is returned.

	Notes:		ALWAYS USE THIS ONE IN PREFERENCE TO THE WORD ONE, IE ALWAYS PASS A INT32 IN,
				NOT A WORD !!!

				This function now works internally as well, since comparing a INT32 -1 with a
				word -1 didn't work out too well...

	SeeAlso:	DialogOp::GetValueIndex

********************************************************************************************/

BOOL DialogManager::GetValueIndex(CWindowID WindowID,
				  				  CGadgetID Gadget,
				   				  INT32* Index)
{
	INT32 index = GetFirstSelectedItem(WindowID, Gadget);
	if (Index) *Index=index;
	TRACEUSER("amb", _T("Index is %d"),index);
	return TRUE;
}


/********************************************************************************************

>	BOOL DialogManager::GetValueIndex(CWindowID WindowID,
				 	   				  CGadgetID Gadget,
				  	   				  WORD* Index)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/9/93
	Inputs:		WindowID: 		Dialog box window identifier
				Gadget:		Gadget identifier

    Outputs:	Index: 			The index of the currently selected item in the gadget. The
    							index value starts at 0 for the first item in the list.

	Returns:	TRUE if the index value could be read, else FALSE.

	Purpose:	For finding the index of the currently selected item in a gadget

				For Windows
				-----------

				The function can be called for ListBox and ComboBox controls
				(including our custom combobox cc_1dBitmapComboBoxEdit)

	Errors:		If the function is called on an invalid gadget then an ENSURE failure will
				occur in a DEBUG build. In a retail build FALSE is returned.

	SeeAlso:	DialogOp::GetValueIndex

********************************************************************************************/

BOOL DialogManager::GetValueIndex(CWindowID WindowID,
				  				  CGadgetID Gadget,
				   				  WORD* Index)
{
	if (Index) *Index=GetFirstSelectedItem(WindowID, Gadget);
	return TRUE;
}



/********************************************************************************************

>	BOOL DialogManager::AddDialogControlToHelper(CWindowID WindowID, CGadgetID Gadget)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/7/2000
	Inputs:		WindowID: 		Dialog box window identifier
				Gadget:		Gadget identifier
	Outputs:
	Returns:	TRUE if successful, else FALSE
	Purpose:	To add this control to the static dialog control helper, which will subclass it

********************************************************************************************/

BOOL DialogManager::AddDialogControlToHelper(CWindowID WindowID, CGadgetID Gadget)
{
	// For the time being, we do this by Hide/Unhide
	wxWindow * pGadget = GetGadget(WindowID, Gadget);
	if (!pGadget) return FALSE;
	pGadget->Show(TRUE);
	return TRUE;
}


/********************************************************************************************

>	BOOL DialogManager::RemoveDialogControlFromHelper(CWindowID WindowID, CGadgetID Gadget)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/7/2000
	Inputs:		WindowID: 		Dialog box window identifier
				Gadget:		Gadget identifier
	Outputs:
	Returns:	TRUE if successful, else FALSE
	Purpose:	To add this control to the static dialog control helper, which will subclass it

********************************************************************************************/

BOOL DialogManager::RemoveDialogControlFromHelper(CWindowID WindowID, CGadgetID Gadget)
{
	// For the time being, we do this by Hide/Unhide
	wxWindow * pGadget = GetGadget(WindowID, Gadget);
	if (!pGadget) return FALSE;
	pGadget->Show(FALSE);
	return TRUE;
}

/********************************************************************************************

>	BOOL DialogManager::EnableGadget(CWindowID WindowID, CGadgetID Gadget, BOOL Enabled);

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/9/93
	Inputs:		WindowID: 		Dialog box window identifier
				Gadget:		Gadget identifier
				Enabled:		TRUE if the gadget is to be enabled.
								FALSE if the gadget is to be disabled.
	Outputs:
	Returns:	TRUE if successful, else FALSE
	Purpose:	For enabling/disabling a gadget.

				For Windows
				-----------

				This function can be called for all controls

	Errors:		-
	SeeAlso:	DialogOp::EnableGadget

********************************************************************************************/

BOOL DialogManager::EnableGadget(CWindowID WindowID, CGadgetID Gadget, BOOL Enabled)
{
	wxWindow * pGadget = GetGadget(WindowID, Gadget);
	if (!pGadget) return FALSE;
	pGadget->Enable( FALSE != Enabled );
	return (TRUE);
}

/********************************************************************************************

>	BOOL DialogManager::HideGadget(CWindowID WindowID, CGadgetID Gadget, BOOL Hide)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/9/93
	Inputs:		WindowID: 		Dialog box window identifier
				Gadget:		Gadget identifier
				Enabled:		TRUE if the gadget is to be hidden.
								FALSE if the gadget is to be shown.
	Outputs:	-
	Returns:	TRUE if successful, else FALSE
	Purpose:	For hiding/showing gadgets

				For Windows
				-----------

				This function can be called for all controls

	Errors:		-
	SeeAlso:	DialogOp::HideGadget

********************************************************************************************/

BOOL DialogManager::HideGadget(CWindowID WindowID, CGadgetID Gadget, BOOL Hide)
{
	// For the time being, we do this by Hide/Unhide
	wxWindow * pGadget = GetGadget(WindowID, Gadget);
	if (!pGadget) return FALSE;
	pGadget->Show(!Hide);
	return TRUE;
}

/********************************************************************************************

>	void DialogManager::Layout(CWindowID WindowID, BOOL CanYield=FALSE)

	Author:		Alex Bligh <alex@alex.org.uk>
	Created:	10/05/2006
	Inputs:		WindowID: 		Dialog box window identifier
	Outputs:	-
	Returns:	-
	Purpose:	Relayout dialog - for sizer changes
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void DialogManager::Layout(CWindowID WindowID, BOOL CanYield /*=FALSE*/)
{
	((wxWindow *)WindowID)->Layout();
	if (CanYield)
	{
		// wxWidgets needs a yield to process these, but we can't always yield
		wxWindowDisabler(WindowID);
		wxYieldIfNeeded();
	}
}

/********************************************************************************************

>	BOOL DialogManager::GadgetRedraw(CWindowID WindowID, CGadgetID Gadget, BOOL Redraw)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/9/93
	Inputs:		WindowID: 		Dialog box window identifier
				Gadget:		Gadget identifier
				Redraw:		    TRUE to turn on gadget redraw
								FALSE to turn off gadget redraw
	Outputs:	-
	Returns:	TRUE if successful, else FALSE
	Purpose:	This function sets a gadgets redraw state. If Redraw = FALSE then the gadget
				will not be redrawn when changes are made to it. Conversely if Redraw = TRUE
				then the Gadget will redraw itself after any changes are made.

				For Windows
				-----------

				This function can be called for all controls

	Errors:		-
	SeeAlso:	DialogOp::GadgetRedraw

********************************************************************************************/

BOOL DialogManager::GadgetRedraw(CWindowID WindowID, CGadgetID Gadget, BOOL Redraw)
{
	wxWindow * pGadget = GetGadget(WindowID, Gadget);
	if (!pGadget) return FALSE;

	if( Redraw )
	{
		pGadget->Thaw();
	}
	else
	{
		pGadget->Freeze();
	}

	return TRUE;
}



/********************************************************************************************
>	static BOOL DialogManager::SetKeyboardFocus(CWindowID WindowID, CGadgetID Gadget)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/11/94
	Inputs:		WindowID			ID of the window / dialogue box containing . . .
				Gadget			ID of the "gadget" (control) to receive the focus
	Outputs:	-
	Returns:	TRUE if successful.
	Purpose:	Sets the keyboard focus to the given control.
	Errors:		-
	SeeAlso:	DialogManager::DefaultKeyboardFocus; DialogOp::SetKeyboardFocus
********************************************************************************************/

BOOL DialogManager::SetKeyboardFocus(CWindowID WindowID, CGadgetID Gadget)
{
	// Set the focus to the control within the given window/dialogue box.
	// For the time being, we do this by Hide/Unhide
	wxWindow * pGadget = GetGadget(WindowID, Gadget);
	if (!pGadget) return FALSE;

	pGadget->SetFocus();
	return TRUE;
}



/********************************************************************************************
>	static BOOL DialogManager::CaptureMouse(CWindowID WindowID, CGadgetID Gadget)

	Author:		Stefan_Stoykov (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/04/97
	Inputs:		WindowID			ID of the window / dialogue box containing . . .
				Gadget			ID of the "gadget" (control) to receive the focus
	Outputs:	-
	Returns:	TRUE if successful.
	Purpose:	Allows the given control to Capture the mouse.
	Errors:		-
	SeeAlso:	DialogManager::DefaultKeyboardFocus; DialogOp::SetKeyboardFocus
********************************************************************************************/

BOOL DialogManager::CaptureMouse(CWindowID WindowID, CGadgetID Gadget)
{
	// Set the focus to the control within the given window/dialogue box.
	// For the time being, we do this by Hide/Unhide
	wxWindow * pGadget = GetGadget(WindowID, Gadget);
	if (!pGadget) return FALSE;

	pGadget->CaptureMouse();
	return TRUE;
}



/********************************************************************************************
>	static BOOL DialogManager::ReleaseMouse(CWindowID WindowID, CGadgetID Gadget)

	Author:		Stefan_Stoykov (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/04/97
	Inputs:		WindowID			ID of the window / dialogue box containing . . .
				Gadget			ID of the "gadget" (control) to receive the focus
	Outputs:	-
	Returns:	TRUE if successful.
	Purpose:	Release the mouse captured by the given control
	Errors:		-
	SeeAlso:	DialogManager::DefaultKeyboardFocus; DialogOp::SetKeyboardFocus
********************************************************************************************/

BOOL DialogManager::ReleaseMouse(CWindowID WindowID, CGadgetID Gadget)
{
	// get the handle off the control
	// For the time being, we do this by Hide/Unhide
	wxWindow * pGadget = GetGadget(WindowID, Gadget);
	if (!pGadget) return FALSE;

	// Check whether the control has the mouse capture
	if( wxWindow::GetCapture() == pGadget )
	{
		pGadget->ReleaseMouse();
		return TRUE;
	}
	else
		return FALSE;
}



/********************************************************************************************
>	static BOOL DialogManager::DefaultKeyboardFocus()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/11/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if successful.
	Purpose:	Sets the keyboard focus to the "default" window, which currently is the
				main frame window (which in turn sets it to the active view window).
	Errors:		-
	SeeAlso:	DialogManager::SetKeyboardFocus; DialogOp::SetKeyboardFocus
********************************************************************************************/

BOOL DialogManager::DefaultKeyboardFocus()
{
	// Set the focus to the main window, which will in turn set it to the active view.
	GetMainFrame()->SetFocus();
	return TRUE;
}



/********************************************************************************************
>	static BOOL DialogManager::HighlightText(CWindowID WindowID, CGadgetID Gadget,
											 INT32 nStart = 0, INT32 nEnd = -1)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/11/94
	Inputs:		WindowID			Window/dialogue box ID (handle)
				Gadget			control ID within the window/dialogue box
				nStart			first letter to highlight (by default the very first)
				nEnd			last letter to highlight (by default the very last)
	Outputs:	-
	Returns:	TRUE if successful
	Purpose:	Highlights the given range of text (by default all of it) within a
				control that holds editable text, eg. an edit field.
	Errors:		ERROR3 if you try to highlight text in a control without any, eg. a
				button or a scroller.
	SeeAlso:	DialogOp::HighlightText
********************************************************************************************/

BOOL DialogManager::HighlightText(CWindowID WindowID, CGadgetID Gadget, INT32 nStart, INT32 nEnd)
{
	// Find out the window class of the control.  Only those with editable text can
	// be highlighted.
	// For the time being, we do this by Hide/Unhide
	wxWindow * pGadget = GetGadget(WindowID, Gadget);
	if (!pGadget) return FALSE;

#ifdef _DEBUG
	ERROR3IF( !pGadget->IsKindOf( CLASSINFO(wxListBox) ) &&
		!pGadget->IsKindOf( CLASSINFO(wxComboBox) ) &&
		!pGadget->IsKindOf( CLASSINFO(wxTextCtrl) ),
				"Wrong kind of control in DialogManager::HighlightText");
#endif

	if( pGadget->IsKindOf( CLASSINFO(wxTextCtrl) ) )
	{
		( (wxTextCtrl *)pGadget )->SetSelection( -1, -1 );
	}
	else
	if( pGadget->IsKindOf( CLASSINFO(wxComboBox) ) )
	{
		( (wxComboBox *)pGadget )->SetSelection( -1, -1 );
	}

	return TRUE;
}



/********************************************************************************************
>	static void DialogManager::PaintGadgetNow(CWindowID WindowID, CGadgetID gid)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/9/94
	Inputs:		WindowID			Window identifier
				gid			Gadget (control) identifier,  or zero for the whole window
	Outputs:	-
	Returns:	-
	Purpose:	Immediate paints any invalid areas of the given control (like the Windows
				"UpdateWindow" function).
	Errors:		-
	SeeAlso:	-
********************************************************************************************/

void DialogManager::PaintGadgetNow(CWindowID WindowID, CGadgetID Gadget)
{
	if (!Gadget)
	{
		((wxWindow *)WindowID)->Update();
		wxPlatformDependent::Get()->FixUpdate((wxWindow *)WindowID);
		return;
	}
	// For the time being, we do this by Hide/Unhide
	wxWindow * pGadget = GetGadget(WindowID, Gadget);
	if (!pGadget) return;

	pGadget->Update();
	wxPlatformDependent::Get()->FixUpdate(pGadget);
}



/********************************************************************************************

>	static void DialogManager::InvalidateGadget(CWindowID WindowID, CGadgetID Gadget, BOOL EraseBackground)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/10/94
	Inputs:		WindowID - The Window identifier
				Gadget - The control that requires invalidating
	Purpose:	Invalidates the control so that it will be repainted soon.

********************************************************************************************/

void DialogManager::InvalidateGadget(CWindowID WindowID, CGadgetID Gadget, BOOL EraseBackground /*=TRUE*/)
{
	if (!Gadget)
	{
		((wxWindow *)WindowID)->Refresh(EraseBackground);
		return;
	}
	// For the time being, we do this by Hide/Unhide
	wxWindow * pGadget = GetGadget(WindowID, Gadget);
	if (!pGadget) return;

	pGadget->Refresh(EraseBackground);
}



/********************************************************************************************

>	static void DialogManager::InvalidateGadget(CWindowID WindowID, CGadgetID Gadget,
												RedrawInfoType *ExtraInfo,
												DocRect *InvalidRect);

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/11/94
	Inputs:		WindowID - The Window identifier
				Gadget - The ID of the gadget that you want to be redrawn

				ExtraInfo - The information passed to your DIM_ mouse event handler

				InvalidRect - The MILLIPOINT rectangle to invalidate, in the (0,0)->(dx,dy)
				coordinate space used in ReDrawInfoType structures. (or NULL to invalidate
				the entire window)

	Purpose:	Causes the Dialog Manager to tell the host os to get the cc_DialogDraw
				gadget to be redrawn, over the specfied rectangle. You should recieve
				a DIM_REDRAW message in the not too distant future.

	Notes:		If you are using a Virtual coordinate space which differs from the
				(0,0)->(dx,dy) space that this requires, then you'll need to call
				some conversion methods which do not yet exist!

********************************************************************************************/

void DialogManager::InvalidateGadget(CWindowID WindowID, CGadgetID Gadget,
										ReDrawInfoType *ExtraInfo,
										DocRect *InvalidRect)
{
	if (InvalidRect == NULL)	// No rect - invalidate the entire window
	{
		InvalidateGadget(WindowID, Gadget);
		return;
	}

	if (ExtraInfo == NULL || WindowID == 0)
	{
		ERROR2RAW("DialogManager::InvalidateGadget was passed illegal NULL parameter(s)");
		return;
	}

	ERROR3IF(ExtraInfo->Dpi == 0, "Screen DPI is zero? I think not! Divide-by-zeros imminent!");
	if (!ExtraInfo->Dpi) return;

	INT32 PixelSize = 72000 / ExtraInfo->Dpi;		// Size of a pixel in MILLIPOINTS

	DocRect irect=*InvalidRect;
	if (irect.lo.y > irect.hi.y)
	{
		// not an ERROR3 because this is in rendering code
		TRACEALL( _T("Rectangle upside down in InvalidateGadget\n") );
		// swap over the rect Y co-ords
		INT32 temp=irect.lo.y;
		irect.lo.y=irect.hi.y;
		irect.lo.y=temp;
	}

	wxRect ToRedraw(irect.lo.x / PixelSize, (ExtraInfo->dy-irect.hi.y) / PixelSize,
					(irect.hi.x-irect.lo.x) / PixelSize, (irect.hi.y-irect.lo.y)/PixelSize);

	wxWindow * pGadget = GetGadget(WindowID, Gadget);
	// Invalidate the gadget, but only if we found a legal window to invalidate
	ERROR3IF((!pGadget), "DialogManager::InvalidateGadget - Gadget not valid");

	if (pGadget)
	{
		// GTK seems a bit precious about invalid coordinates, so clip to the client size
		wxRect GadgetRect(pGadget->GetClientSize());
		ToRedraw=ToRedraw.Intersect(GadgetRect);
		pGadget->Refresh(TRUE, &ToRedraw);
	}
}


/********************************************************************************************

>	static void DialogManager::ScrollKernelRenderedGadget(CWindowID WindowID, CGadgetID Gadget,
												DocRect *RectToScroll, DocCoord *ScrollBy)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/1/95
	Inputs:		WindowID - The Window identifier
				Gadget - The ID of the gadget that you want to be redrawn

				RectToScroll - The MILLIPOINT rectangle to scroll, in the (0,0)->(dx,dy)
				coordinate space used in ReDrawInfoType structures. Think of this rectangle
				as the visible portion of the window which will be copied (some of which
				will be scrolled out of view), and let this method worry about invalidating
				the 'hole' left behind to finish off the display.
				This parameter may be NULL, in which case the entire gadget will be scrolled

				ScrollBy - Gives the X and Y amounts in millipoints to scroll the given
				rectangle. I don't know what will happen if neither of these values is zero!

	Outputs:	ScrollBy will be returned with the x and y values grid-locked to the
				underlying display-pixel grid. If you do not use the returned values to
				update your millipoint scroll position, your scroll position will get out
				of sync with the displayed image, and you'll be screwed.

	Purpose:	Causes the Dialog Manager to tell the host os to get the cc_DialogDraw
				gadget to be scrolled, over the specfied rectangle. You should recieve
				a DIM_REDRAW message in the not too distant future, to update any portions
				that 'scroll into view'.

	Notes:		If you are using a Virtual coordinate space which differs from the
				(0,0)->(dx,dy) space that this requires, then you'll need to convert
				your coords.

				If ScrollBy is such that the entire visible region will be scrolled out of
				view, this ends up just invalidating the scrollrect.

				I have not tried scrolling an entire window with this method, so it may
				be that it doesn't work properly (it should work, but you might need to
				set up a proper clipping rectangle to stop masty redraw effects)

				To produce the best results, you should udate your scroll position after
				calling this method and immediately call PaintGadgetNow to minimise the
				time for which the window layout is in a state of flux.

********************************************************************************************/

void DialogManager::ScrollKernelRenderedGadget(CWindowID WindowID, CGadgetID Gadget,
												DocRect *RectToScroll, DocCoord *ScrollBy)
{
	ERROR3IF(ScrollBy == NULL || WindowID == 0 || Gadget == 0,
			"DialogManager::ScrollKernelRenderedGadget - NULL Params are illegal");

	// For the time being, we do this by Hide/Unhide
	wxWindow * pWinToScroll = GetGadget(WindowID, Gadget);
	if (!pWinToScroll) return;

	ERROR3IF( pWinToScroll == 0,
		"DialogManager::ScrollKernelRenderedGadget - Illegal window/gadget");

	if( pWinToScroll == 0 )
		return;

	ReDrawInfoType ExtraInfo;
	GetKernelRenderedGadgetInfo(WindowID, Gadget, &ExtraInfo);

	INT32 PixelSize = 72000 / ExtraInfo.Dpi;		// Size of a pixel in MILLIPOINTS

	INT32 ScrollDX = ScrollBy->x / PixelSize;
	INT32 ScrollDY = ScrollBy->y / PixelSize;

	// Grid-lock the scroll offsets to a multiple of device pixels in size, for return
	ScrollBy->x = ScrollDX * PixelSize;
	ScrollBy->y = ScrollDY * PixelSize;

	if (abs(ScrollBy->x) >= RectToScroll->Width() ||
		abs(ScrollBy->y) >= RectToScroll->Height())
	{
		// Have scrolled far enough that none of the currently visible stuff will be
		// visible after scrolling, so just force redraw the affected rectangle
		// (Actually, the 'else' part should get this right anyway, but I don't
		// want to have to test everything on 6 billion OSes, and it's probably
		// marginally more efficient this way)
		InvalidateGadget(WindowID, Gadget, &ExtraInfo, RectToScroll);
	}
	else
	{
		if (RectToScroll != NULL)
		{
			// Get the base scroll area in Windows OS coords
			wxRect		BaseRect;
			BaseRect.x		= RectToScroll->lo.x / PixelSize;
			BaseRect.width	= ( RectToScroll->hi.x - RectToScroll->lo.x ) / PixelSize;
			BaseRect.y		= ( ExtraInfo.dy - RectToScroll->lo.y ) / PixelSize;
			BaseRect.height	= ( ExtraInfo.dy + RectToScroll->lo.y - RectToScroll->hi.y ) / PixelSize;

			// Copy the scroll area contents with a blit, and invalidate the 'hole'
			pWinToScroll->ScrollWindow( -ScrollDX, -ScrollDY, &BaseRect );
		}
		else
		{
			// Just scroll the entire window client area
			pWinToScroll->ScrollWindow( -ScrollDX, -ScrollDY, NULL );
		}
	}
}



/********************************************************************************************

>	static BOOL DialogManager::GetKernelRenderedGadgetInfo(CWindowID WindowID, CGadgetID Gadget,
															ReDrawInfoType *Result)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/1/95

	Inputs:		WindowID - The parent window identifier
				Gadget - The ID of the gadget that you want info for

	Returns:	FALSE if there was a catastrophic error (it will report an ERROR2 if necessary),
				in which case the returned data is invalid (well, it defaults to 96 Dpi and
				an area of 72000x72000 millipoints, so is 'safe' to use, but probably wrong)
				TRUE under normal conditions

	Outputs:	Result - will be returned filled in with appropriate kernel-rendered-
				dialogue information (dx, dy, and Dpi will be filled in with the appropriate
				values; pMousePOs, pDC, and pClipRect will all be NULL)

	Purpose:	Allows the user access to the same information which is passed in to
				DIM_REDRAW and the kernel-rendered-dialogue mouse-handling messages. This
				is just for convenience so they can calculate stuff at a time other than
				handling those two types of dialogue event (e.g. if a document message
				causes you to have to redraw a small portion of your gadget, you need this
				information to calculate the invalidation rectangle from)

********************************************************************************************/

BOOL DialogManager::GetKernelRenderedGadgetInfo(CWindowID WindowID, CGadgetID Gadget,
												ReDrawInfoType *Result)
{
	ERROR3IF(WindowID == 0 || Gadget == 0 || Result == NULL,
		"DialogManager::GetKernelRenderedGadgetInfo: NULL parameters are illegal!");

	// For the time being, we do this by Hide/Unhide
	wxWindow * pTheWindow = GetGadget(WindowID, Gadget);
	if (!pTheWindow) return FALSE;

	Result->pDC = NULL;
	Result->pClipRect = NULL;
	Result->pMousePos = NULL;

	// Install some 'safe' defaults, just in case of serious error
	Result->dx = Result->dy = 72000;
	Result->Dpi = 96;

	// If a totally rampant call, error (debug) and return failure
	ERROR3IF( NULL == pTheWindow,
				"DialogManager::GetKernelRenderedGadgetInfo - Illegal window/gadget");
	if( pTheWindow == 0 )
		return(FALSE);

	// Get the screen DPI
	wxScreenDC			ScreenDC;
PORTNOTE("dialog","Can't handle different DPIs, using X")
	Result->Dpi = OSRenderRegion::GetFixedDCPPI(ScreenDC).x; // x;

	// Calculate how big the window is, in MILLIPOINTS
	wxSize				WindowSize( pTheWindow->GetClientSize() );

	Result->dx = ( INT32(WindowSize.GetWidth())*72000 ) / Result->Dpi;
	Result->dy = ( INT32(WindowSize.GetHeight())*72000) / Result->Dpi;
	return(TRUE);
}




// -----------------------------------------------------------------------------------------
// Message handler functions

/********************************************************************************************

>	static void DialogManager::SetGadgetIDToFocus(HWND DialogWnd)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/4/94
	Inputs:		DialogWnd: The dialog window
	Outputs:	-
	Returns:	-
	Purpose:	If a child control of the dialog window has the focus then the function
				sets the Gadget to the ID of this control.

	Scope:		private
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void DialogManager::SetGadgetIDToFocus( wxWindow *pDialogWnd )
{
	PORTNOTETRACE("dialog","DialogManager::SetGadgetIDToFocus - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	// Determine which gadget has the focus
	wxWindow		   *pFocusWindow = wxWindow::FindFocus();

	// If the Focus Window is a child of the dialog window then set Gadget equal
	// to the control ID of this window
	wxWindow		   *pCurrentChild;
	wxWindowList	   &listChild = pDialogWnd->GetChildren();
	wxWindowList::iterator iter = listChild.begin();
	wxWindowList::iterator end = listChild.end();
	for(;
		iter != end;
		++iter )
	{
		if( CurrentChild == FocusWindow )
		{
			// The focus is with a gadget
			Gadget = GetDlgCtrlID( FocusWindow );
			break;
		}
	}
#endif
}
/********************************************************************************************

>	static BOOL CustomControlMsg(HWND hdlg, UINT32 wParam, INT32 lParam)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/4/94
	Inputs:		hdlg: Dialog HWND
	Outputs:	-
	Returns:	TRUE if message handled
	Purpose:	Handles custom control messages
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL DialogManager::CustomControlMsg( wxWindow *pDlg, UINT32 wParam, INT32 lParam )
{
	PORTNOTETRACE("dialog","DialogManager::CustomControlMsg - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	String_256 ClassNameStr;
    GetClassName( HWND(lParam), (TCHAR*)ClassNameStr, 255);
	if ((ClassNameStr == String_16(TEXT("cc_BitmapButton")))||
		ClassNameStr == String_16(TEXT("cc_SmallButton")))
	{
		Gadget = HIWORD(wParam);
		switch (LOWORD(wParam))
		{
		case BN_CLICKED:
			DialogManager::DIM = DIM_LFT_BN_CLICKED;
			DialogManager::HandleMessage = TRUE;
			return TRUE;

		case BN_BUTTONUP:
			DialogManager::DIM = DIM_LFT_BN_UP;
			DialogManager::HandleMessage = TRUE;
			return TRUE;

		case WM_MOUSEMOVE:
			DialogManager::DIM = DIM_MOUSE_MOVE;
			DialogManager::HandleMessage = TRUE;
			return TRUE;


		default:
			TRACEUSER( "JustinF", _T("Unknown message ID in DialogManager::CustomControlMsg\n"));
			break;
		}
	}
#endif
	return FALSE; // Not a custom control message
}

/********************************************************************************************

>	void DialogManager::EnableAllDialogs(BOOL Enable, HWND ExceptMe = NULL)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/4/95

	Inputs:		Enable:	When TRUE all dialogs are enabled

				ExceptMe: An optional dialog to exclude. If a modal dialog X has just
						  been created then all dialogs except X will need disabling
	Outputs:	-
	Returns:	-
	Purpose:	Enables or Disables all Dialogs on the DialogOp message handler list.
				This is useful when opening/closing modal dialogs.

********************************************************************************************/

void DialogManager::EnableAllDialogs( BOOL Enable, wxWindow *pExceptMe )
{
	// Obtain a list of all live dialogs
	List			   *pDlgList = MessageHandler::GetClassList( CC_RUNTIME_CLASS(DialogOp) );

	ERROR3IF( pDlgList == NULL, "Could not find the DialogOp Class List" );
	if( NULL != pDlgList )
	{
		ListItem	   *CurrentOp = pDlgList->GetHead();
		DialogOp	   *pDlgOp;
		while( CurrentOp != NULL )
		{
	        if (CurrentOp->IS_KIND_OF(DialogOp)) // They all should be
	        {
	        	pDlgOp = (DialogOp*)CurrentOp;
				// Determine if the dialog has an associated window
				if (pDlgOp->HasWindow())
				{
					if (pDlgOp->WindowID != pExceptMe)
					{
						( (wxWindow *)pDlgOp->WindowID )->Enable( FALSE != Enable );
					}
				}
			}
			CurrentOp = pDlgList->GetNext(CurrentOp); // Get next operation in the live list
		}
	}

	// For some reason, the green bit of code below did not disable common dialogs. Therefore
	// the semi-bodge below was added to disable the active window.

	wxWindow		   *pActiveWindow = wxWindow::FindFocus();
	if( NULL != pActiveWindow )
	{
		if( pActiveWindow != pExceptMe )
		{
			pActiveWindow->Enable( FALSE != Enable );
		}
	}
}



// End of Message handler functions
// ----------------------------------------------------------------------------------------

// Methods for setting the types of edit fields
// ----------------------------------------------------------------------------------------

/********************************************************************************************

>	void DialogManager::SetEditGadgetType(CWindowID WindowID,
									  CGadgetID Gadget,
									  EditGadgetType Type)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/8/93
	Inputs:		WindowID:   Dialog box window identifier
				Gadget:  	The edit gagdet identifier
				Type:       The type of the edit gadget

				UNIT_NUMERIC: Only accept unit chars
				INT_NUMERIC:  Only accept integer chars
				REAL_NUMERIC: Only accept real chars
	Outputs:	-
	Returns:	-
	Purpose:	Subclasses the edit gadget so that it only accepts characters specified by
				the Type parameter.
	Errors:		-
	SeeAlso:	DialogOp::SetEditGadgetType

********************************************************************************************/


void DialogManager::SetEditGadgetType(CWindowID Win,
									  CGadgetID Gadget,
									  EditGadgetType Type)
{
	UINT32 IDSValidChars=0; // Resource ID of valid input characters
	switch (Type)
	{
		// Obtain the resource of the string containing the set of characters which the
		// control should accept.
		case UNIT_NUMERIC: IDSValidChars = _R(IDS_UNIT_NUMERIC_CHARS); break;
		case INT_NUMERIC: IDSValidChars = _R(IDS_INT_NUMERIC_CHARS);   break;
		case REAL_NUMERIC: IDSValidChars = _R(IDS_REAL_NUMERIC_CHARS); break;
	};
	SetEditGadgetType(Win, Gadget, IDSValidChars);
}

// A quick subclass hack to fix requirement that wxTextValidator has data storage
class wxTextValidatorFixed : public wxTextValidator
{
	DECLARE_DYNAMIC_CLASS(wxTextValidatorFixed);
public:
	wxTextValidatorFixed(/*TYPENOTE: Correct*/ long style = wxFILTER_NONE, wxString *val = 0) : wxTextValidator(style, val) {}
	wxTextValidatorFixed(const wxTextValidator& val) : wxTextValidator(val) {}
	~wxTextValidatorFixed(){}
	virtual wxObject *Clone() const { return new wxTextValidatorFixed(*this); }
	virtual bool TransferToWindow(void)	/*TYPENOTE: Correct*/
	{	
		if ( m_stringValue )
		{
			if( !CheckValidator() ) return false;			/*TYPENOTE: Correct*/
			wxTextCtrl *control = (wxTextCtrl *) m_validatorWindow;
			control->SetValue(* m_stringValue);
		}
    return true;											/*TYPENOTE: Correct*/
	}
	virtual bool TransferFromWindow(void)	/*TYPENOTE: Correct*/
	{
		if ( m_stringValue )
		{
			if( !CheckValidator() )	return false;			/*TYPENOTE: Correct*/
			wxTextCtrl *control = (wxTextCtrl *) m_validatorWindow;
			*m_stringValue = control->GetValue();
		}
		return true;										/*TYPENOTE: Correct*/
	}
};
IMPLEMENT_DYNAMIC_CLASS(wxTextValidatorFixed, wxTextValidator);

/********************************************************************************************

>	void DialogManager::SetEditGadgetType(CWindowID WindowID,
				     					  CGadgetID Gadget,
								 		  UINT32 IDSValidChar)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/8/93
	Inputs:		WindowID:   	Dialog box window identifier
				Gadget:  		The edit gagdet identifier
				IDSValidChar:   Resource ID of the string containing the set of characters
								that the edit gadget should accept.

	Outputs:	-
	Returns:	-
	Purpose:	To subclass the edit gadget so that it only accepts characters in the string
				specified by IDSValidChar
	Errors:		-
	SeeAlso:	DialogOp::SetEditGadgetType

********************************************************************************************/


void DialogManager::SetEditGadgetType(CWindowID WindowID,
								 CGadgetID Gadget,
								 UINT32 IDSValidChar)
{
	wxWindow * pGadget = GetGadget(WindowID, Gadget);
	if (!pGadget) return;

	String_256 s;
	if (!s.Load(IDSValidChar))
	{
		ERROR3("Could not load validator string");
		return;
	}
	wxArrayString valstring;
	TCHAR * p = (TCHAR *)s;
	TCHAR c;
	while ((c=*p++)!=0)
	{
		valstring.Add(wxString(c));
	}
	wxTextValidatorFixed validator(wxFILTER_INCLUDE_CHAR_LIST);
	validator.SetIncludes(valstring);
	pGadget->SetValidator(validator);
}

/********************************************************************************************

>	void DialogManager::DualFunctionButton(CWindowID DialogWnd,
									   CGadgetID ButtonGadget)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/9/93
	Inputs:		DialogWnd:		Dialog box window identifier
				ButtonGadget:	Button gadget identifier
	Outputs:	-
	Returns:	-
	Purpose:	To subclass the button gadget so that it accepts right mouse button clicks.
	Errors:		-
	SeeAlso:	DialogOp::DualFunctionButton

********************************************************************************************/

void DialogManager::DualFunctionButton(CWindowID DialogWnd,
									   CGadgetID ButtonGadget)
{
	PORTNOTETRACE("dialog","DialogManager::DualFunctionButton - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	// This function should only be called for a button control
	HWND hGadget = GetDlgItem((HWND)DialogWnd, (INT32)ButtonGadget);
	#if _DEBUG
	String_256 ClassNameStr;
	GetClassName(hGadget, (TCHAR*)ClassNameStr, 255);

	ENSURE((ClassNameStr == String_8(TEXT("Button"))), "DualFunctionButton function called\n"
													  "on a gadget which is not a button");
	#endif

	ControlInfo* ControlInfoPtr; 			   // This structure will be passed to the
											   // RgtMOuseButtonProc.

	ControlInfoPtr = new ControlInfo;
	ControlInfoPtr->pControlWnd = hGadget;
    ControlInfoPtr->lpfnOldProc = (FARPROC) GetWindowLong(hGadget, GWL_WNDPROC);

    // Store the button control info in the dialog's Edit control info list
	GetControlList( DialogWnd )->AddHead(ControlInfoPtr);

	FARPROC lpfnDualFn = MakeProcInstance((FARPROC) DialogManager::RgtMouseButtonProc,
									       AfxGetApp()->m_hInstance);

	// Subclass the control so that it calls the new button proc
	SetWindowLong(hGadget, GWL_WNDPROC, (INT32) lpfnDualFn );
#endif
}

/********************************************************************************************

>	BOOL DialogManager::MakeListBoxDragable(CWindowID DialogWnd,
								 			CGadgetID ListGadget)


	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/12/93
	Inputs:		DialogWnd:	The dialogbox window identifier
				ListGadget:	The Identifier of a listbox gadget
	Outputs:	-
	Returns:	TRUE if successful, otherwise FALSE
	Purpose:
	Errors:		An ENSURE failure will occur if ListGadget is not a ListBox control
	SeeAlso:	DialogOP::MakeListBoxDragable

********************************************************************************************/


BOOL DialogManager::MakeListBoxDragable(CWindowID WindowID,
							 			CGadgetID Gadget)
{
	// This function should only be called for a LISTBOX control
	// For the time being, we do this by Hide/Unhide
	wxWindow * pGadget = GetGadget(WindowID, Gadget);
	if (!pGadget) return FALSE;
#if _DEBUG
	ENSURE( pGadget->IsKindOf( CLASSINFO( wxListBox ) ),
			"MakeListBoxDragable function called\non a gadget which is not a Listbox" );
#endif

	return TRUE;										// pretend worked
}


/********************************************************************************************

>	INT32 FAR PASCAL EXPORT DialogManager::ValidateEditGadgetProc(HWND hwnd,
													    		  UINT32 message,
												   				  UINT32 wParam,
												   				  INT32 lParam)


	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/8/93
	Inputs:		hwnd
				message
				wParam
				lParam
	Outputs:	-
	Returns:	-
	Purpose:	subclassed edit control proc
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

INT32 FAR PASCAL EXPORT DialogManager::ValidateEditGadgetProc( wxWindow *pWnd,
												    		  UINT32 message,
											   				  UINT32 wParam,
											   				  INT32 lParam)
{
	PORTNOTETRACE("dialog","DialogManager::DualFunctionButton - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	// We need to access the control's dialog window to find the valid characters the control
	// should accept.

	HWND DlgWin = GetParent(hwnd);

	// --------------------------------------------------------------------------------------
	// Search the dialog's edit control list to find the information about the control
	List* ControlInfoList = GetControlList(DlgWin);
	ControlInfo* CurrentCntrl = (ControlInfo*)ControlInfoList->GetHead();
	// The lookup must be as quick as possible
	while (CurrentCntrl != NULL)
	{
		if (CurrentCntrl->ControlWnd == hwnd)
			break;
		CurrentCntrl = (ControlInfo*)ControlInfoList->GetNext(CurrentCntrl);
	}

	ENSURE(CurrentCntrl != NULL, "Could not find information for a typed edit control");

    // Load in the valid input characters for the edit control
    String_256 ValidChars;
    ValidChars.Load(CurrentCntrl->IDSValidCh, NULL);

    if (message == WM_CHAR) // Character message
    {

    	BOOL CharValid = FALSE;

    	INT32 LenValidChars = ValidChars.Length();
    	TCHAR KeyCh = ((TCHAR)wParam);
    	for (INT32 i=0; ( (i < LenValidChars) && (!CharValid) ); i++)
    		if (((TCHAR*)ValidChars)[i] == (TCHAR)KeyCh)
			{
				CharValid = TRUE;
			}
		if (!CharValid)
		{
			// Check if the character is a special control character
			if ((KeyCh != (TCHAR)'\b') && // backspace
				(KeyCh != (TCHAR)'\t') && // tab
				(KeyCh != (TCHAR)'\n') && // linefeed
				(KeyCh != (TCHAR)'\r'))   // carriage return

				return (0);   // An invalid character was input.
    	}
    }
	else if (message==WM_DESTROY)
		DeleteControlList( hwnd );

	return CallWindowProc((WNDPROC)(CurrentCntrl->lpfnOldProc), hwnd, message, wParam, lParam);
#else
	return 0;
#endif
}

/********************************************************************************************

>	INT32 FAR PASCAL EXPORT DialogManager::RgtMouseButtonProc(HWND hwnd,
        										     	  UINT32 message,
										   				  UINT32 wParam,
										   				  INT32 lParam)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/9/93
	Inputs:		hwnd
				message
				wParam
				lParam
	Outputs:	-
	Returns:	-
	Purpose:	subclassed button control proc
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
INT32 FAR PASCAL EXPORT DialogManager::RgtMouseButtonProc( wxWindow *pWnd,
        										     	  UINT32 message,
										   				  UINT32 wParam,
										   				  INT32 lParam)
{
	PORTNOTETRACE("dialog","DialogManager::DualFunctionButton - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	// We need to access the control's dialog window to find the old proc
	HWND DlgWin = GetParent(hwnd);

	// --------------------------------------------------------------------------------------
	// Search the dialog's control list to find the information about the control
	List* ControlInfoList = GetControlList(DlgWin);
	ControlInfo* CurrentCntrl = (ControlInfo*)ControlInfoList->GetHead();
	// The lookup must be as quick as possible
	while (CurrentCntrl != NULL)
	{
		if (CurrentCntrl->ControlWnd == hwnd)
			break;
		CurrentCntrl = (ControlInfo*)ControlInfoList->GetNext(CurrentCntrl);
	}

	ENSURE(CurrentCntrl != NULL, "Could not find information for a dual function button");
	if ((message == WM_RBUTTONDOWN) || (message == WM_RBUTTONUP))
	{

		// Set the state of the button
		wParam = (message == WM_RBUTTONDOWN) ? 1:0;
		message = BM_SETSTATE;
		lParam = 0L;
		CallWindowProc((WNDPROC)(CurrentCntrl->lpfnOldProc), hwnd, message, wParam, lParam);

		// Send Command message to the control's parent dialog
		if (wParam == 1) // WM_RBUTTONDOWN message
		#if WIN32
			SendMessage(DlgWin, WM_COMMAND, MAKEINT32(GetWindowLong(hwnd, GWL_ID),BN_RGT_CLICKED), LPARAM(hwnd));
		#else
			SendMessage(DlgWin, WM_COMMAND, GetWindowWord(hwnd, GWW_ID), MAKEINT32(hwnd,BN_RGT_CLICKED));
		#endif
	}
	else if (message==WM_DESTROY)
		DeleteControlList( hwnd );
	return CallWindowProc((WNDPROC)(CurrentCntrl->lpfnOldProc), hwnd, message, wParam, lParam);
#else
	return 0;
#endif
}

/********************************************************************************************

>	static void DialogManager::DeInit()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/9/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Deinitialise the DialogManager
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void DialogManager::DeInit()
{
	// Because this function is called from CCamApp::SaveAllModified, of all places,
	// it gets called twice on system shutdown (once on receipt of WM_QUERYENDSESSION,
	// which itself calls CCamApp::SaveAllModified, and once on receipt of WM_ENDSESSION,
	// which performs a SC_CLOSE system command which ultimately also calls SaveAllModified.
	// This flags is a bodge for v1.1 to stop this happening, a better solution would be
	// to rewrite the dialog manager to be more rational.

	// It's not currently broken this way changed TRACE to ERROR3- Alex

	static BOOL fCalledOnceBodge = FALSE;
	if (!fCalledOnceBodge) fCalledOnceBodge = TRUE;
	else
	{
		ERROR3( wxT("DialogManager::DeInit called twice - please fix me properly sometime\n") );
		return;
	}

	// save current bar state - this should check whether save prefs on exit is set!!!!
PORTNOTE("dialog","Removed IsFullScreenMode usage")
#ifndef EXCLUDE_FROM_XARALX
	if( GetMainFrame()->IsFullScreenMode() )
		DialogBarOp::WriteNamedBars("clean");
	else
		DialogBarOp::WriteNamedBars("normal");
#endif

	// Send a CANCEL message to all open dialogs
	BROADCAST_TO_CLASS(DialogMsg(NULL, DIM_CANCEL, 0), DialogOp);
	// BODGE Special nasty stuff for v1 release.

	// InformationBarOp is completely deaf to the message broadcast above so we must
	// make sure their windows are destroyed (manually)
	InformationBarOp::SetVisibility(FALSE, TRUE);

	// Delete all Dialog position info
	ListItem* DlgPos = DialogPositionList.GetHead();
	ListItem* NextPos;
	while (DlgPos != NULL)
	{
		// Make sure that all dialogs have been deleted.
		ENSURE( ((DialogPosition*)DlgPos)->DlgWinList.IsEmpty(),
			  "Live Dialog box found whilst destoying dialog manager");

		NextPos =  DialogPositionList.GetNext(DlgPos);
		delete DialogPositionList.RemoveItem(DlgPos);
		DlgPos = NextPos;
	}
}

/********************************************************************************************

>	DialogManager::~DialogManager()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/9/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	DialogManager destructor.
	Errors:		-
	SeeAlso:	DialogManager::DeInit

********************************************************************************************/

DialogManager::~DialogManager()
{
}

/********************************************************************************************

>	List* DialogManager::GetControlList(HWND hWnd)

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/4/94
	Inputs:		A Window handle
	Outputs:	-
	Returns:	A pointer to its special list, or NULL if it doesn't have one.
	Purpose:	Hide the mechanism we use to attach our lists to various windows. In fact we
				use window properties.
	Errors:		-
	SeeAlso:	-
	Scope:		Private, static

********************************************************************************************/

List *DialogManager::GetControlList( CWindowID hWnd )
{
	PORTNOTETRACE("dialog","DialogManager::GetControlList - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	HANDLE Prop = GetProp( hWnd, MAKEINTATOM(GetListAtom) );	// Atoms & Properties are fast
	if (Prop)
	{
	#if WIN32
		return (List*)(INT32)Prop;
	#else
		HANDLE Seg = GetProp( hWnd, MAKEINTATOM(GetListAtomSegment) );
		INT32 ptr = MAKEINT32( Prop, Seg );
		return (List*)ptr;
	#endif
	}
	else
#endif
		return NULL;
}

/********************************************************************************************

>	void DialogManager::DeleteControlList(HWND hWnd)

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/11/94
	Inputs:		A Window handle
	Purpose:	Deletes the Properties that are used to store the ControlList. Should be
				called as the very last thing before the Window is vaped e.g. in response
				to a WM_DESTROY message. Destroying it earlier will cause GetControlList
				to return NULL which proves fatal within the various message handlers.
	Scope:		Private, static

********************************************************************************************/

void DialogManager::DeleteControlList( CWindowID hWnd )
{
	PORTNOTETRACE("dialog","DialogManager::DeleteControlList - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	RemoveProp( hWnd, MAKEINTATOM(GetListAtom) );
#ifndef WIN32
	RemoveProp( hWnd, MAKEINTATOM(GetListAtomSegment) );
#endif
#endif
}



// This macro aligns a BYTE ptr to a dword boundary
#if WIN32
#define	DWORD_ALIGN(x)	if( DWORD_PTR(x) % 4 ) x = ADDR(( DWORD_PTR(x) + 3 ) & ~3);
#else
#define	DWORD_ALIGN(x)
#endif


/********************************************************************************************

>	LPWSTR DialogManager::MovePastWideStr(LPWSTR pWideStr)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/11/93
	Inputs:		pWideStr - the start of the string to skip
	Returns:	Address of first byte past the end of the string.
	Purpose:	Given a ptr to the first byte in a wide string (or char string if win16)
				return a ptr to the byte past the null.

********************************************************************************************/

LPWSTR DialogManager::MovePastWideStr(LPWSTR pWideStr)
{
	// Skip the string
	while (*pWideStr++)
		;

	// Return the address of the following character
	return pWideStr;
}

/********************************************************************************************

>	size_t DialogManager::SizeDlgHeader(DLGTEMPLATE *pHeader)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/11/93
	Inputs:		pHeader - start of the dialog box header.
	Returns:	Size of the dialog box header.
	Purpose:	Given a ptr to DIALOGBOXHEADER, return its size.

********************************************************************************************/


size_t DialogManager::SizeDlgHeader(DLGTEMPLATE *pHeader)
{
	// Move past the header
	ADDR pResult = (ADDR) (pHeader + 1);

	// skip szMenuName

	#if WIN32

	pResult = (ADDR) MovePastWideStr((LPWSTR) pResult);

	#else

	switch (*pResult++)
	{
		// See SDK help file (Dialog Box Resource) for details
		case 0:
			break;

		case 0xFF:
			pResult += 2;
			break;

		default:
			pResult += camStrlen((LPTCHAR) (pResult)) + 1;
			break;
	}

	#endif

	// then skip szClassName

	pResult = (ADDR) MovePastWideStr((LPWSTR) pResult);

	// then the caption

	pResult = (ADDR) MovePastWideStr((LPWSTR) pResult);

PORTNOTE("dialog","Removed DLGTEMPLATE usage")
#ifndef EXCLUDE_FROM_XARALX
	// then the font stuff
	if( (pHeader->style) & DS_SETFONT )
		pResult = (ADDR) MovePastWideStr((LPWSTR) (pResult + 2));		// skip font size too
#endif

	DWORD_ALIGN(pResult);

	// Compute the size of the header and return it.
	return (size_t) (pResult - ((ADDR) pHeader));
}


/********************************************************************************************

>	size_t DialogManager::SizeCtrlData(DLGITEMTEMPLATE *pData)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/11/93
	Inputs:		pData - the start of the control structure.
	Returns:	Size of the control data.
	Purpose:	Given a ptr to a controldata struct, return its length.
				Under Win32, all strings are UniCode (as they are in the res file).

********************************************************************************************/

size_t DialogManager::SizeCtrlData(DLGITEMTEMPLATE *pControl)
{
	#if WIN32
	// Get pointer to the data after the DLGITEMTEMPLATE structure.
	WORD *pData = (WORD *) (pControl + 1);

	// Skip the class
	if (*pData == 0xFFFF)
		// There is only one more word - the atom of the window class - skip it.
		pData += 2;
	else
		// The class is specified as a string - skip it
		pData = (WORD *) MovePastWideStr((LPWSTR) pData);

	// Skip the 'title' array
	if (*pData == 0xFFFF)
		// There is only one more word - it is a resource ID - skip it.
		pData += 2;
	else
		// The title array is a string - skip it
		pData = (WORD *) MovePastWideStr((LPWSTR) pData);

	// Skip the creation data.
	UINT32 Extra = (UINT32) *pData;
	pData++;

	#else
	// Get pointer to the data after the DLGITEMTEMPLATE structure.
	ADDR pData = (ADDR) (pControl + 1);

	// Skip the class
	if ((*pData) & 0x80)
		pData++;
	else
		pData = (ADDR) MovePastWideStr((LPWSTR) pData);

	// Skip the 'text' array
	pData = (ADDR) MovePastWideStr((LPWSTR) pData);

	// Skip the creation data.
	BYTE Extra = *pData;
	pData++;

	#endif

	// Get end pointer and double-word align it.
	ADDR pEnd = (ADDR) pData;
	DWORD_ALIGN(pEnd);

	// Compute size of control data and return it.
	ADDR pStart = (ADDR) pControl;
	pEnd += Extra;
	return (size_t) (pEnd - pStart);
}

/********************************************************************************************

>	DLGTEMPLATE *DialogManager::MergeDialog(HINSTANCE MainInst,  CDlgResID MainID,
											HINSTANCE OtherInst, CDlgResID OtherID)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/11/93
	Inputs:		MainInst - handle of the module containing the 'Main' dialog template.
				MainID - ID of the main dialog resource
				OtherInst - handle of the module containing the 'Other' dialog template.
				OtherID - ID of the other dialog resource
	Returns:	Pointer to a block of memory which contains a dialog template which is the
				result of merging the two templates into one.
	Purpose:	Merge two dialogs together into one dialog template.
				The return value points to the new merged dialog, which the caller should
				pass to CreateDialogIndirect().  Once the dialog has been created using this
				block, CCFree() should be called on the block to return it to the system
				pool.
				The significance of the 'Main' and 'Other' names is that the dialog style
				(border, menuname, classname, title and font) is taken from 'Main' - the
				header of 'Other' is discarded.
	SeeAlso:	CCFree()

********************************************************************************************/

DLGTEMPLATE *DialogManager::MergeDialog( /*HINSTANCE MainInst, */  CDlgResID MainID,
										 /*HINSTANCE OtherInst, */ CDlgResID OtherID)
{
	PORTNOTETRACE("dialog","DialogManager::MergeDialog - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	HGLOBAL 			hGlobal[2];
	HRSRC 				hRes[2];
	DLGTEMPLATE        *pDlg[2];
	INT32				HeaderSize[2];
	ADDR				pNew,
						pOld;
	INT32 				Size;
	DLGTEMPLATE		   *pHeader;

	#if WIN32
	WORD 				Count, Total;
	#else
	BYTE				Count, Total;
	#endif

	size_t 				Len[2],
						ItemSize;
	UINT32				xOffset = 0;

	// Find, load and lock the dialog resources
	hRes[0] = FindResource(MainInst, MAKEINTRESOURCE(MainID), RT_DIALOG);
	hGlobal[0] = LoadResource(MainInst, hRes[0]);
	pDlg[0] = (DLGTEMPLATE *) LockResource(hGlobal[0]);

	hRes[1] = FindResource(OtherInst, MAKEINTRESOURCE(OtherID), RT_DIALOG);
	hGlobal[1] = LoadResource(OtherInst, hRes[1]);
	pDlg[1] = (DLGTEMPLATE *) LockResource(hGlobal[1]);

	// Calculate how big they each are, excluding header.
	// We would ideally use GlobalSize to find this, but it doesn't work under NT.

	Total = 0;

	for (UINT32 i = 0; i <= 1; i++)
	{
		Count = pDlg[i]->cdit;
		Total += Count;
		HeaderSize[i] = SizeDlgHeader(pDlg[i]);
		pOld = ((ADDR) pDlg[i]) + HeaderSize[i];
		Len[i] = 0;

		while (Count--)
		{
			DLGITEMTEMPLATE *pControl = (DLGITEMTEMPLATE *) pOld;

			// Find out how big the control is
			ItemSize = SizeCtrlData(pControl);

			// Update the right margin limit if this is the main dialog
			if (i == 0)
			{
				UINT32 xLimit = pControl->x + pControl->cx;
				xOffset= max(xOffset, xLimit);
			}

			// Update the length and move on to the next control
			Len[i] += ItemSize;
			pOld += ItemSize;
		}
	}

	// Allocate a new one, using header size + each data size.
    // The header of 'Main' is used for the new dialog.
	Size = SizeDlgHeader(pDlg[0]);
	pNew = (ADDR) CCMalloc(Size + Len[0] + Len[1]);

	// Copy in the header...
	memcpy( pNew, pDlg[0], (size_t) Size );

	pHeader = (DLGTEMPLATE *) pNew;		// new header now
	pNew += Size;

	// ... then data[0] ...
	memcpy(pNew, ((ADDR) pDlg[0]) + HeaderSize[0], Len[0]);
	pNew += Len[0];

	// ... and then data[1].
	memcpy(pNew, ((ADDR) pDlg[1]) + HeaderSize[1], Len[1]);

	// Fixup count in new header.
	pHeader->cdit = Total;

	// Now move all the controls in the sub-dialog to the right so they don't overlap with
	// the controls in the main dialog.
	Count = pDlg[1]->cdit;

	while (Count--)
	{
		DLGITEMTEMPLATE *pControl = (DLGITEMTEMPLATE *) pNew;

		// Shift the control to the right
		pControl->x += xOffset;

		// Move on to the next control
		pNew += SizeCtrlData(pControl);
	}

	// now its safe to free up the resource handles
	// Note: hRes handles never freed - how can you do this?

	UnlockResource(hGlobal[0]);
	FreeResource(hGlobal[0]);

	UnlockResource(hGlobal[1]);
	FreeResource(hGlobal[1]);

	return pHeader;
#else
	return NULL;
#endif
}


/********************************************************************************************

>	static BOOL DialogManager::IsADialogWindow(HWND hwnd)


	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/3/94
	Inputs:		-
	Outputs:	-
	Returns:	Returns TRUE if hwnd is the window handle of a dialog
	Purpose:	To determine if hwnd is the handle of a dialog
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL DialogManager::IsADialogWindow( wxWindow *pWnd )
{
	return( DialogOp::IsADialogWindow( pWnd ) );
}

/********************************************************************************************

>	static BOOL DialogManager::ModalDialogOpen(DialogOp** pModal)


	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/3/94
	Inputs:		-
	Outputs:	An optional pointer to the modal dialog which is ope
	Returns:	Returns TRUE if a modal dialog is open
	Purpose:	To determine if there is currently an open modal dialog
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL DialogManager::ModalDialogOpen(DialogOp** pModal)
{
	// Obtain the list of all Dialogs
	List* DialogClassList = MessageHandler::GetClassList(CC_RUNTIME_CLASS(DialogOp));
	ENSURE(DialogClassList != NULL, "Could not find DialogOp message handler list");

	// Look for an open modal dialog
	for (MessageHandler* pMsgHandler = (MessageHandler*)DialogClassList->GetHead();
		 pMsgHandler != NULL;
		 pMsgHandler = (MessageHandler*)DialogClassList->GetNext(pMsgHandler))
	{
		ENSURE(pMsgHandler->IsKindOf(CC_RUNTIME_CLASS(DialogOp)),
			"MessageHandler of DialogOp class list is not a DialogOp");
		if (pMsgHandler->IsKindOf(CC_RUNTIME_CLASS(DialogOp)))
		{
			// Ok we got ourselves a DialogOp
			DialogOp* pDlgOp = (DialogOp*)pMsgHandler;
			// Is the dialog open ?
			if (pDlgOp->WindowID != NULL)
			{
				// Is the Dialog modal
				if (pDlgOp->IsModal() )
				{
					if (pModal != NULL)
					{
						*pModal = pDlgOp;
					}
					return TRUE; // found one
				}
			}
		}
	}
	return FALSE; // No modal dialog's open
}

/********************************************************************************************

>	static BOOL DialogManager::IsWindowVisible(CWindowID Win)


	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/11/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if Win is visible
	Purpose:	To test the visibility of Win
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL DialogManager::IsWindowVisible(CWindowID Win)
{
	return ( (wxWindow *)Win )->IsShown();
}

/********************************************************************************************

>	BOOL DialogManager::IsCustomComboDropdownVisible(CWindowID WindowID, CGadgetID Gadget)


	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/2/2000
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if WinID's dropdown is visible
	Purpose:	To test the visibility of WinID's dropdown
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL DialogManager::IsCustomComboDropdownVisible(CWindowID WindowID, CGadgetID Gadget)
{
	PORTNOTETRACE("dialog","DialogManager::IsCustomComboDropdownVisible - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	String_256 ClassNameStr;  // The control type

	HWND hGadget = GetDlgItem((HWND)WindowID, (INT32)Gadget);
	// Find out the class type of the gadget
	GetClassName(hGadget, (TCHAR*)ClassNameStr, 255);

	if ((ClassNameStr == String_64(TEXT("cc_1dBitmapComboBoxEdit"))) ||
	    (ClassNameStr == String_64(TEXT("cc_2dBitmapComboBoxEdit")))   )
	{
		BOOL RetVal = FALSE;

		RetVal = SendMessage (hGadget, WM_CCQUERYDROPDOWN, (WPARAM) (0), (LPARAM) 0);

		return (RetVal);
	}
	return (TRUE);
	ENSURE(FALSE, "Calling IsCustomComboDropdownVisible for an invalid control");
#endif
	return FALSE;
}

/********************************************************************************************

>	BOOL DialogManager::CloseDropdown (CWindowID WindowID, CGadgetID Gadget, BOOL CloseVal)


	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/2/2000
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if things went ok
	Purpose:	Used to close the controls dropdown.  This routine was written because of
				the amount of trouble that I was experiencing with my custom controls
				down to escape key processing (which is viewed as a hotkey within camelot).
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL DialogManager::CloseDropdown (CWindowID WindowID, CGadgetID Gadget, BOOL CloseVal)
{
PORTNOTE("dialog","Programatic CB drop not supported by wx")
	return (TRUE);
}

/********************************************************************************************

>	OurPropSheet* DialogManager::GetPropertySheetFromOp( DialogTabOp* pDialogTabOp )

	Author:		Luke_Hart (xara group ltd) <lukeh@xara.com>
	created:	28/04/06
	inputs:		pdialogtabop: the operation we wish to find the property sheet for
	outputs:	-
	returns:	The associated Property sheet (or NULL if none)
	purpose:	Find the property sheet associated with the passed DialogTabOp
	errors:		-
	seealso:	-

********************************************************************************************/

OurPropSheet* DialogManager::GetPropertySheetFromOp( DialogTabOp* pDialogTabOp )
{
	DlgTagOpToPropShtItem* pTabItem = (DlgTagOpToPropShtItem*)(DlgTagOpToPropShtList.GetHead());
	while (pTabItem != NULL)
	{
		if (pTabItem->pDialogTabOp == pDialogTabOp)
			return pTabItem->pPropertySheet;

		pTabItem = (DlgTagOpToPropShtItem*)(DlgTagOpToPropShtList.GetNext(pTabItem));
	}

	return NULL;
}

/********************************************************************************************

>	BOOL DialogManager::AddAPage(DialogTabOp* pDialogTabOp, CDlgResID DialogResID)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/11/94
	Inputs:		pDialogTabOp: The operation we wish to add a page too
				DialogResID:  The resource ID of the dialog page we wish to add to the
							  DialogTabOp
	Outputs:	-
	Returns:	TRUE if the page was added successfully.
	Purpose:	Adds a page to the tabbed dialog with resource ID DialogResID.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/


BOOL DialogManager::AddAPage(DialogTabOp* pDialogTabOp, CDlgResID DialogResID)
{
	// Try to add the page to the property sheet associated with the DialogTabOp
	// let's try and find it
	OurPropSheet* pPropSheet = GetPropertySheetFromOp( pDialogTabOp );
	ERROR2IF( NULL == pPropSheet, FALSE, _T("Couldn't find DialogTabOp") );
	wxBookCtrlBase*	pNoteBook = pPropSheet->GetBookCtrl();

	// We need to create a page object
	// Because OurPropShtPage is derived from an MFC object we have to cope with exceptions
	wxWindow*	pNewPage;
	wxString	ObjectName;
	try
	{
		const TCHAR* pDialogName=CamResource::GetObjectNameFail( DialogResID );
		ERROR1IF(pDialogName == NULL, FALSE, _R(IDE_CANNOT_CREATE_DIALOG));
		TRACEUSER( "jlh92", _T("Cre tab %s\n"), pDialogName );
		ObjectName = pDialogName;

		pNewPage = wxXmlResource::Get()->LoadPanel( pNoteBook, pDialogName );
		ERROR1IF(pNewPage == NULL, FALSE, _R(IDE_CANNOT_CREATE_DIALOG));
		pNewPage->SetId( DialogResID );

		CamArtProvider::Get()->EnsureChildBitmapsLoaded( pNewPage );
	}
	catch( CMemoryException )
	{
		ERROR1(FALSE, _R(IDS_OUT_OF_MEMORY));
	}

	// Just to  be safe
	ERROR1IF(pNewPage == NULL, FALSE, _R(IDS_OUT_OF_MEMORY));

	wxString Title = pNewPage->GetTitle();
	if (Title.IsEmpty()) 
		Title = pNewPage->GetLabel(); // because wxPanel doesn't seem to support a title
	if( Title.IsEmpty() )
	{
		ResourceID	NameResID = CamResource::GetResourceID( PCTSTR(ObjectName) );
		PCTSTR		pszStringLookup = CamResource::GetTextFail( NameResID );
		if( NULL != pszStringLookup )
		{
			Title = pszStringLookup;
			TRACEUSER( "jlh92", _T("Page (FST) = \"%s\"\n"), pszStringLookup );
		}
	}
	if( Title.IsEmpty() )
	{
		// Finally, in desperation, we (mis-)use the tooltip string because now the wx folks have removed
		// the label, even though it's needed for accessibility. Aarrghh
		wxToolTip* pTip = pNewPage->GetToolTip();
		if (pTip) Title=pTip->GetTip();
	}

	pNoteBook->AddPage( pNewPage, Title );
	return true;
}

/********************************************************************************************

>	static CWindowID DialogManager::GetPageWindow(CWindowID Win, CDlgResID PageID, INT32* PageIndex = NULL)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/11/94
	Inputs:		Win: The windowID
				PageID: A page resource identifier, If this is not NULL then the WindowID
						must be that of a Property sheet. We ENSURE that this is true.
	Outputs:	Optionally returns the page index
	Returns:	Window ID of a page within a property sheet
	Purpose:	Returns the Window ID of a page within a property sheet. If the page
				is NULL then Win is simply returned, It can be called  on a
				non property sheet dialog. Just pretend that they are property sheets without
				any pages.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

CWindowID DialogManager::GetPageWindow(CWindowID Win, CDlgResID PageID, INT32* PageIndex)
{
	if (PageID == 0)
	{
		return Win; // A page is not required
	}

	wxWindow		   *pCWnd = (wxWindow *)Win;
	ERROR3IF( pCWnd == NULL, "This is not a property sheet, the PageID should be NULL" );

	// Ok so Win must be a property sheet, let's make sure
	ERROR3IF( !pCWnd->IsKindOf( CLASSINFO(OurPropSheet) ), "Don't know what this window is" );

	OurPropSheet* pPropertySheet = (OurPropSheet*)pCWnd;
	wxNotebookPage* pCurrentPage;

	// Find the window ID of the page
	wxBookCtrlBase*	pBookCtrl = pPropertySheet->GetBookCtrl();
	for (UINT32 i = 0; i < pBookCtrl->GetPageCount(); i++)
	{
		pCurrentPage = 	(wxNotebookPage*)(pBookCtrl->GetPage(i));
		if (pCurrentPage->GetId() == INT32(PageID) )
		{
			if (PageIndex != NULL)
			{
				*PageIndex = i;
			}
			// Found page so return window id
			return pCurrentPage;
		}
	}
	ERROR3("The page specified does not exist on this tabbed dialog");
	return Win; // Probably safer than NULL
}

/********************************************************************************************

>	BOOL DialogManager::SetTitlebarName(CWindowID Win, String_256* Name)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/12/94
	Inputs:		Win:  The dialog window
				Name: The titlebar text
	Outputs:	-
	Returns:	-
	Purpose:	Sets the titlebar text of the dialog.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL DialogManager::SetTitlebarName( CWindowID Win, String_256* Name )
{
	( (wxWindow *)Win )->SetLabel( (TCHAR *)(*Name) );
	return true;
}

// -----------------------------------------------------------------------------------------
// Functions to Set and KIll timer events

/********************************************************************************************

>	static BOOL DialogManager::SetTimer(HWND hWnd,	UINT32 nIDEvent, UINT32 nElapse,
										void (CALLBACK EXPORT* lpfnTimer)(HWND, UINT32, UINT32, DWORD) = NULL)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/4/97
	Inputs:		hWnd		Handle of window for timer messages
				nIDEvent	Specifies a nonzero timer identifier.
				nElapse		Specifies the time-out value, in milliseconds.
	Returns:	The timer identifier of the new timer if Nonzero, Zero means a problem.
	Purpose:	Allows the user access to setting a timer caller back or event for a dialog box.
				The caller can either specify a call back procedure to be called when the timer
				goes off or if null is specified, a DIM_TIMER message will be sent.
				This maps onto the Windows API/CWnd call.
				The return value is effectively the handle onto the timer system. It must be
				passed to the KillTimer member function to kill the timer. A Nonzero value
				indicates successful allocation of the timer; non-zero implies a problem.

	Note:		DIM_TIMER code is not tested.

	SeeAlso:	DialogOp::KillTimer; DialogOp::SetTimer;

********************************************************************************************/

PORTNOTE("dialog","Removed DialogManager::KillTimer - HWND and timer usage")
#ifndef EXCLUDE_FROM_XARALX
UINT32 DialogManager::SetTimer( CWindowID hWnd, UINT32 nIDEvent, UINT32 nElapse,
							 void (CALLBACK EXPORT* lpfnTimer)(HWND, UINT32, UINT32, DWORD))
{
	// Call the Windows API
	PORTNOTETRACE("dialog","DialogManager::SetTimer - do nothing");
//	return ::SetTimer(hWnd, nIDEvent, nElapse, lpfnTimer);

	return 0;
}
#endif

/********************************************************************************************

>	static BOOL DialogOp::KillTimer( INT32 nIDEvent )

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/4/97
	Inputs:		hWnd		Handle of window for timer messages
				nIDEvent	Specifies a nonzero timer identifier.
	Returns:	True if the event was killed, FALSE if the specified timer event could not be found.
	Purpose:	Allows the user access to killing a timer caller back or event that has been
				set up for a dialog box.
				Kills the timer event identified by nIDEvent from the earlier call to SetTimer.
				Any pending WM_TIMER messages associated with the timer are removed from the
				message queue.
	SeeAlso:	DialogOp::SetTimer; DialogManager::SetTimer;

********************************************************************************************/

PORTNOTE("dialog","Removed DialogManager::KillTimer - HWND and timer usage")
#ifndef EXCLUDE_FROM_XARALX
BOOL DialogManager::KillTimer( CWindowID Wnd, INT32 nIDEvent )
{
	// Call the Windows API
	PORTNOTETRACE("dialog","DialogManager::KillTimer - do nothing");
//	return ::KillTimer(hWnd, nIDEvent);

	return 0;
}
#endif

/********************************************************************************************

>	static BOOL DialogManager::GetWindowPosition(CWindowID WindowID, RECT * pRect)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/6/97
	Inputs:		WindowID	Handle of window
	Outputs:	Rect		pointer to the rectangle to fill in
	Returns:	True if the call was successful, FALSE otherwise.
	Purpose:	Allows the user to get the current window position.
	SeeAlso:	DialogOp::GetWindowPosition; DialogManager::SetWindowPosition;

********************************************************************************************/

BOOL DialogManager::GetWindowPosition( CWindowID WindowID, wxRect *pRect )
{
	*pRect = wxRect(WindowID->GetPosition(), WindowID->GetSize());
	return TRUE;
}

BOOL DialogManager::GetWindowPosition( CWindowID WindowID, RECT *pRect )
{
	wxRect Rect;
	if (!GetWindowPosition(WindowID, &Rect)) return FALSE;
	pRect->bottom=Rect.GetBottom();
	pRect->top=Rect.GetTop();
	pRect->left=Rect.GetLeft();
	pRect->right=Rect.GetRight();
	return TRUE;
}

/********************************************************************************************

>	static BOOL DialogManager::GetGadgetPosition(CWindowID WindowID, CGadgetID Gadget, RECT * pRect)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/6/97
	Inputs:		WindowID	Handle of window
				Gadget		The icon or gadget that we are interested in.
	Outputs:	Rect		pointer to the rectangle to fill in
	Returns:	True if the call was successful, FALSE otherwise.
	Purpose:	Allows the user to get the current position of the specified gadget or icon.
	SeeAlso:	DialogOp::GetGadgetPosition; DialogManager::SetGadgetPosition;

********************************************************************************************/

BOOL DialogManager::GetGadgetPosition( CWindowID WindowID, CGadgetID Gadget, wxRect *pRect )
{
	wxWindow * pGadget = GetGadget(WindowID, Gadget);
	if (!pGadget) return FALSE;

	// We just need to pass back the normal position of the window
	if (pRect)
		*pRect = pGadget->GetSize();

	return TRUE;
}

BOOL DialogManager::GetGadgetPosition( CWindowID WindowID, CGadgetID Gadget, RECT *pRect )
{
	wxRect Rect;
	if (!GetGadgetPosition(WindowID, Gadget, &Rect)) return FALSE;
	pRect->bottom=Rect.GetBottom();
	pRect->top=Rect.GetTop();
	pRect->left=Rect.GetLeft();
	pRect->right=Rect.GetRight();
	return TRUE;
}

/********************************************************************************************

>	static BOOL DialogManager::SetWindowPosition(CWindowID WindowID, const RECT& Rect)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/6/97
	Inputs:		hWnd		Handle of window
				Rect		New rectangle for the window position
	Returns:	True if the call was successful, FALSE otherwise.
	Purpose:	Allows the user to set the current window position.
	SeeAlso:	DialogOp::SetWindowPosition; DialogManager::GetWindowPosition;

********************************************************************************************/
// Technical Note:
// Don't even think of using GetWindowRect or GetClientRect as there are not set equivalents.
// MoveWindow is completely spappy and does not use the same coordinates on client items!!!!
// GetWindowPlacement at least has a SetWindowPlacement form which
// uses the same coordinate system.

BOOL DialogManager::SetWindowPosition(CWindowID WindowID, const wxRect &Rect)
{
	WindowID->SetPosition(Rect.GetTopLeft());
	WindowID->SetSize(Rect.GetSize());
	return TRUE;
}

BOOL DialogManager::SetWindowPosition( CWindowID WindowID, const RECT &Rect )
{
	return SetWindowPosition( WindowID, wxRect(wxPoint(Rect.left, Rect.top), wxPoint(Rect.right, Rect.bottom)));
}

/********************************************************************************************

>	static BOOL DialogManager::SetGadgetPosition(CWindowID WindowID, CGadgetID Gadget, const RECT& Rect)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/6/97
	Inputs:		WindowID	Handle of window
				Gadget		The icon or gadget that we are interested in.
				Rect		New rectangle for the window position
	Returns:	True if the call was successful, FALSE otherwise.
	Purpose:	Allows the user to set the current position of the specified gadget or icon.
	SeeAlso:	DialogOp::SetGadgetPosition; DialogManager::GetGadgetPosition;

********************************************************************************************/

BOOL DialogManager::SetGadgetPosition( CWindowID WindowID, CGadgetID Gadget, const wxRect &Rect )
{
	wxWindow * pGadget = GetGadget(WindowID, Gadget);
	if (!pGadget) return FALSE;

	pGadget->SetSize( Rect );
	return TRUE;
}

BOOL DialogManager::SetGadgetPosition( CWindowID WindowID, CGadgetID Gadget, const RECT &Rect )
{
	return SetGadgetPosition( WindowID, Gadget, wxRect(wxPoint(Rect.left, Rect.top), wxPoint(Rect.right, Rect.bottom)));
}

/********************************************************************************************

>	static INT32 DialogManager::GetScreenDpi()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/6/97
	Returns:	0 or the dpi of the screen.
	Purpose:	Allows the user to find out the screen dpi.
	SeeAlso:	DialogOp::GetScreenDpi;

********************************************************************************************/

INT32 DialogManager::GetScreenDpi()
{
	// Get the screen DPI
	wxScreenDC			ScreenDC;
PORTNOTE("dialog","Can't handle different DPIs, using X")
	return OSRenderRegion::GetFixedDCPPI(ScreenDC).x;
}

/********************************************************************************************

>	static BOOL DialogManager::GetScreenSize(INT32 * pWidth, INT32 * pHeight)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/6/97
	Outputs:	pWidth	the screen width in pixels
				pHeight	the screen height in pixels
	Returns:	True if worked ok, False otherwise.
	Purpose:	Allows the user to find out the screen size.
	SeeAlso:	DialogOp::GetScreenDpi;

********************************************************************************************/

BOOL DialogManager::GetScreenSize(INT32 * pWidth, INT32 * pHeight)
{
	ERROR2IF(pWidth == NULL || pHeight == NULL,FALSE,"GetScreenSize Bad params!");

	// HORZRES	Width, in pixels, of the screen.
	// VERTRES	Height, in raster lines, of the screen.
	INT32				Width  = wxSystemSettings::GetMetric( wxSYS_SCREEN_X );
	INT32				Height = wxSystemSettings::GetMetric( wxSYS_SCREEN_Y );

	// Tell the caller about the new values
	*pWidth = Width;
	*pHeight = Height;

	return TRUE;
}

// -----------------------------------------------------------------------------------------
// Functions to Save and restore the active dialog state

/********************************************************************************************

>	static BOOL DialogManager::RecordActiveDialogState()


	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/7/95
	Returns:	FALSE if we run out of memory
	Purpose:	This function will get called before a MODAL dialog is opened. It records
				which window is currently active, and if the window is a MODAL dialog.
				After a MODAL dialog is closed RestoreActiveDialogState	is called to
				restore the active window.
	Errors:
	SeeAlso:	DialogManager::RestoreActiveDialogState
	SeeAlso:	DialogManager::ActiveDlgStack

********************************************************************************************/

BOOL DialogManager::RecordActiveDialogState()
{
	// The new way of checking if a modal dialog is about is to determine if the
	//ActiveDlgStack is empty
	BOOL IsModal = !(ActiveDlgStack.GetTop() == NULL);
	// Which window is currently active
	wxWindow * ActiveWindow = wxWindow::FindFocus();
	
	// I think we need to look upwards here till we find a TLW
	while (ActiveWindow && !ActiveWindow->IsKindOf(CLASSINFO(wxTopLevelWindow)) && ActiveWindow->GetParent())
	{
		ActiveWindow = ActiveWindow->GetParent();
	}

	// Record this information
	ActiveDlgStateItem* pActiveDlgItem = new ActiveDlgStateItem;
	if (!pActiveDlgItem)
		return FALSE; // Out of memory

	pActiveDlgItem->pActiveWindow	= ActiveWindow;
	pActiveDlgItem->fIsAModalDialog	= IsModal;

	DialogManager::ActiveDlgStack.Push(pActiveDlgItem);
	return TRUE;
}

/********************************************************************************************

>	static BOOL RestoreActiveDialogState();


	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/7/95
	Purpose:	This function is called after a MODAL dialog is closed. it restores the
				Active window.

	SeeAlso:	DialogManager::RecordActiveDialogState

********************************************************************************************/

void DialogManager::RestoreActiveDialogState()
{
	ActiveDlgStateItem *pActiveDlgState = (ActiveDlgStateItem *)ActiveDlgStack.Pop();
	ERROR3IF(!pActiveDlgState, "RestoreActiveDialogState called when stack was empty");
	if (!pActiveDlgState)
		return;

	wxFrame			   *MainFrame = GetMainFrame();	  // Could be NULL (especially during startup)

	if( NULL != pActiveDlgState->pActiveWindow )
	{
		// Enable/Disable the mainframe
		if (MainFrame)
		{
			MainFrame->Enable( !pActiveDlgState->fIsAModalDialog );
		}

		// Enable the active window
		pActiveDlgState->pActiveWindow->Enable( true );

		// Restore the active window
		pActiveDlgState->pActiveWindow->SetFocus();

		if( pActiveDlgState->fIsAModalDialog )
		{
			// Disable all dialogs except the active window
			EnableAllDialogs( FALSE, pActiveDlgState->pActiveWindow );
		}
		else
		{
 			// And all other dialogs
			EnableAllDialogs(TRUE);
		}
	}
	else
	{
		TRACE( wxT("There is no active window to restore. This is expected during startup before the MainFrame has been created\n") );
		// This could be an error situation. Enable everything, so that the user is not locked out.

		// Enable the mainframe
		if( MainFrame )
			MainFrame->Enable( true );

		// And all other dialogs
		EnableAllDialogs( TRUE );
	}
	delete pActiveDlgState;
	return;
}


/********************************************************************************************

>	static CDlgResID GetActivePage()


	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/3/2000
	Returns:	the pageID of the currently active page in an open tabbed dialog, or -1 if there isn't one
	Purpose:	To find out the currently active page in a tabbed dialog

********************************************************************************************/

CDlgResID DialogManager::GetActivePage()
{
	// first check to see if we have a property sheet object
	DlgTagOpToPropShtItem* pTabItem = (DlgTagOpToPropShtItem*)DlgTagOpToPropShtList.GetHead();
	if (pTabItem->pPropertySheet == NULL)
		return CDlgResID(-1);

	OurPropShtPage* pOurPage = pTabItem->pPropertySheet->GetActivePage();
	if (pOurPage == NULL)
		return CDlgResID(-1);

	return pOurPage->GetId();
}


/********************************************************************************************

>	void DialogManager::SetPropertyPageModified(BOOL Modified)


	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/3/2000
	Inputs:		the modified value to set
	Returns:	-
	Purpose:	Sets the modified property of the currently active property page of the current
				tabbed dialog (if it exists)

********************************************************************************************/

void DialogManager::SetPropertyPageModified(BOOL Modified)
{
	DlgTagOpToPropShtItem* pTabItem = (DlgTagOpToPropShtItem*)DlgTagOpToPropShtList.GetHead();
	if (pTabItem->pPropertySheet != NULL)
	{
PORTNOTE( "dialog", "Removed RegisterWindowMessage usage" )
#ifndef EXCLUDE_FROM_XARALX
		wxNotebookPage* pActivePage = pTabItem->pPropertySheet->GetActivePage();
		if (pActivePage != NULL)
			pActivePage->SetModified(Modified);
#endif
	}
}


/********************************************************************************************

>	ControlInfo::~ControlInfo()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/9/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	ControlInfo destructor
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

ControlInfo::~ControlInfo()
{
}

PORTNOTE("dialog","Removed RegisterWindowMessage usage")
#ifndef EXCLUDE_FROM_XARALX

// OurPropSheet methods

/********************************************************************************************

>	OurPropSheet::OurPropSheet(String_256* pName)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/11/94
	Inputs:		pName: The name to be displayed in the dialog's title bar
	Purpose:	Constructs a property sheet

********************************************************************************************/


OurPropSheet::OurPropSheet(String_256* pName, UINT32 SelPage)
	: CPropertySheet((TCHAR*)(*pName), GetMainFrame(), SelPage)	// The main frame is the owner of the sheet
{
}

/********************************************************************************************

>	INT32 OurPropSheet::OnCreate(LPCREATESTRUCT lpCreateStruct)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/11/94
	Purpose:	Serves no purpose (at the moment) all functionality that used to live in here has
				been moved into the OnInitDialog method. It may come in useful one day though.

********************************************************************************************/

INT32 OurPropSheet::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CPropertySheet::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}


/********************************************************************************************

>	void OurPropSheet::OnClose()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/12/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	We override the OnClose method for our property sheet because the base class
				OnClose method will kill the dialog box without telling us, if we are Modal.
				Therefore, the dialog has no chance to clean itself up and will instead be
				set a DIM_CANCEL when the application is closed!
	Errors:		-
	SeeAlso:	-

	Note:		This was true when using the VC2 XARAPROP.h/cpp class. Using the VC4 version
				means that in the modeless case when the user clicks on the close icon on a
				tabbed dialog box, this function gets called. In the modal case, this function
				never seems to get called. We are then left with the op etc. still live.
				CPropertySheet::DoModal calls DestroyWindow() which should be virtual.
				This also sends a WM_DESTROY message. So, we will take advantage of this and
				trap the call, clean our op up and then call the baseclass version.

********************************************************************************************/

void OurPropSheet::OnClose()
{
	// Inform the Dialog that the close icon has been clicked so that it can be be shut
	// down properly

	// Check if the Cancel button is present and disabled i.e. greyed. If it is then ignore
	// the message. Do this becuase we might have greyed the cancel button to say that this
	// is a bad move at this point in time and we do not want the close button coming along
	// and doing the cancel action when it shoudn't.
   	HWND hGadget = ::GetDlgItem(GetSafeHwnd(), IDCANCEL);	// Obtain gadgets window
	ERROR3IF(hGadget == NULL, "Could not find Cancel button control on tabbed dialog");
	BOOL ok = TRUE;
	// Only check if control is present
	if (hGadget)
		ok = ::IsWindowEnabled(hGadget);					// Get current button state

	// Use NULL as the page id so that the main dialog box gets the message.
	if (ok)
		BROADCAST_TO_CLASS(DialogMsg(GetSafeHwnd(), DIM_CANCEL, NULL, 0, NULL), DialogOp);
	else
		Beep();		// Warn the user that this is a bad time to do this

	// Could call the main property sheet close method to get it to do its stuff but
	// hopefully by now the DIM_CANCEL should have done everything that we were
	// interested in.
	//CPropertySheet::OnClose();
}

/********************************************************************************************

>	virtual BOOL OurPropSheet::DestroyWindow()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/7/97
	Purpose:	We override the DestroyWindow method for our property sheet because the base class
				DestroyWindow method will kill the dialog box without telling us, if we are Modal.
				Therefore, the dialog has no chance to clean itself up and will instead be
				set a DIM_CANCEL when the application is closed!
	SeeAlso:	OurPropSheet::OnClose(); CPropertySheet::OnClose();

********************************************************************************************/

BOOL OurPropSheet::DestroyWindow()
{
	// Tell the system that this is a modal close which we wont have been told about
	// If we do this then we get a no active page error 3. Use the OnSysCommand trap instead.
//	if (IsModal())
//		BROADCAST_TO_CLASS(DialogMsg(GetSafeHwnd(), DIM_CANCEL, NULL, 0, NULL), DialogOp);

	return CPropertySheet::DestroyWindow();
}

/********************************************************************************************

>	void OurPropSheet::OnSysCommand(UINT32 nID, LPARAM lparam)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/7/97
	Purpose:	We override the OnSysCommand method for our property sheet because the base class
				OnClose method will kill the dialog box without telling us, if we are Modal.
				Therefore, the dialog has no chance to clean itself up and will instead be
				set a DIM_CANCEL when the application is closed!
	SeeAlso:	OurPropSheet::DestroyWindow(); OurPropSheet::OnClose(); CPropertySheet::OnClose();

********************************************************************************************/

void OurPropSheet::OnSysCommand(UINT32 nID, LPARAM lparam)
{
	// check for clicks on the close icon
	switch (nID & 0xFFF0)
	{
		case SC_CLOSE:
		{
			// The baseclass does this
			/* if (m_bModeless)
			{
				SendMessage(WM_CLOSE);
				return;
			} */
			// So we will do this
			if (!m_bModeless)
			{
				BROADCAST_TO_CLASS(DialogMsg(GetSafeHwnd(), DIM_CANCEL, NULL, 0, NULL), DialogOp);
			}
			break;
		}
	}
	// The baseclass calls this, we have no need as we will call the baseclass
	// Default();

	// Always call the baseclass
	CPropertySheet::OnSysCommand(nID, lparam);
}

/********************************************************************************************

>	void OurPropSheet::OnMouseActivate()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	01/05/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	We override the OnMouseActivate method for our property sheet
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

INT32 OurPropSheet::OnMouseActivate(CWnd* pDesktopWnd, UINT32 nHitTest, UINT32 message)
{
	// currently has no special handling
	return (CPropertySheet::OnMouseActivate(pDesktopWnd, nHitTest, message));
}




/********************************************************************************************

>	void OurPropSheet::OnMouseWheel()

	Author:		Priestley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/10/2000

********************************************************************************************/

BOOL OurPropSheet::OnMouseWheel(UINT32 nFlags, short zDelta, CPoint pt)
{
	TRACEUSER( "Matt", wxT("MouseWheel Movement Detected!\n") );
	return CPropertySheet::OnMouseWheel(nFlags, zDelta, pt);
}


/********************************************************************************************

>	void OurPropSheet::OnActivate(UINT32 nState, CWnd* pWndOther, BOOL bMinimized)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	01/05/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	We override the Activate method for our property sheet
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void OurPropSheet::OnActivate(UINT32 nState, CWnd* pWndOther, BOOL bMinimized)
{
	// currently has no special handling
	CPropertySheet::OnActivate(nState, pWndOther, bMinimized);
}


/********************************************************************************************

>	void OurPropSheet::OnOK()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	01/05/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	We override the OnOK
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
/*
void OurPropSheet::OnOK()
{
	INT32 i = 1;
}
*/

/********************************************************************************************
*/


/********************************************************************************************

>	LRESULT OurPropSheet::WindowProc( UINT32 message, WPARAM wPara-m, LPARAM lParam )

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/4/94			   `
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	The OurPropSheet's WindowProc firstly sends the message to the dialog manager
				so that it can dispatch it to the appropriate DialogBarOp (if neccessary).
				Then the message is processed as normal.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

// BODGE variable - see the next comment below. Never dereference this pointer!
static CWnd *PropSheetWindowProcBodge = NULL;

LRESULT OurPropSheet::WindowProc( UINT32 Message, WPARAM wParam, LPARAM lParam )
{
	// --- START BODGE (Jason, 14/10/96) ---
	// SendDialogMessage can cause the DialogOp to call End() and destruct itself, which in turn
	// destructs us, which in turn means on exit from that function, we no longer exist. If this
	// happens we must not call the base class, or we access violate!
	// So we set a static variable, and in the destructor we change it to flag the fact that we
	// have been destructed. We set the flag to our 'this' pointer so that we can ignore destruction
	// of other OurPropSheets in the meantime (just in case), and have an ERROR3 to warn us of
	// that sort of dangerous occurrence.
	// See Also the destructor (below)
//	ERROR3IF(PropSheetWindowProcBodge != NULL && PropSheetWindowProcBodge != this,
//				"OurPropSheet::WindowProc - reentrantly called for a different PropSheet object!");
	PropSheetWindowProcBodge = this;

	// Now call the Dialogue manager to process the event
	HWND OurHWND = GetSafeHwnd();
	DialogManager::SendDialogMessage(OurHWND, Message, wParam, lParam);

	if (PropSheetWindowProcBodge == NULL)		// If this is NULL, then we've committed suicide!
		return(0);								// Return immediately before we access violate
	// --- END BODGE ---

	if (!::IsWindow(OurHWND))					// If our former window no longer exists, then
		return(0);								// we should not call down to the base class either

	BOOL OldModelessState = m_bModeless;

	// When initialising a modeless property sheet, we don't want MFC to go shrinking the dialog
	// and eating the buttons, so we pretend to be modal whilst it does its OnInitDialog business,
	// then we revert to our old state.
	if(Message == WM_INITDIALOG)
	{
		m_bModeless = FALSE;
		//EnableStackedTabs(FALSE);
	}
/*
	// Added by Craig Hamilton 24/8/00.
	// This code has been added in order to prevent the export dialog closing when the user has
	// entered an incorrect dpi value and has hit the enter key. The export dialog is modal and
	// would normally close, but if we intercept the message and change it then it will not close.
	if(Message == WM_COMMAND && !BmapPrevDlg::m_GetExportOptions)
	{
		Message = WM_SETFOCUS;
	}
	// End added.
*/

	// DY Bodge alert! In the BrushEditDialog we have changed the use of the OK button
	// so we need to prevent it from entering the CProperySheet message loop, because
	// someone (either MFC or dialogop, I'm not sure) tries to kill the dialog.
	// We do this by seeing if the current active page is one of the brush edit dialog pages,
	// and if so we'll return now
	if((Message == WM_COMMAND) && (wParam == (WPARAM)IDOK))
	{
		OurPropShtPage *pPage = GetActivePage();
		if(pPage != NULL)
		{
			CDlgResID PageID = pPage->GetPageID();
			switch (PageID)
			{
				case _R(IDD_BRUSHEDITSPACING):
				case _R(IDD_BRUSHEDITOFFSET):
				case _R(IDD_BRUSHEDITSCALING):
				case _R(IDD_BRUSHEDITEFFECTS):
				case _R(IDD_BRUSHEDITSEQUENCE):
				case _R(IDD_BRUSHEDITFILL):
					return 0;
				default:
					break;
					// don't do anything, just continue as normal
			}
		}
	}

	// Now process the message normally
	LRESULT Res = CPropertySheet::WindowProc( Message, wParam, lParam );

	if(Message == WM_INITDIALOG)
	{
		m_bModeless = OldModelessState;

		HWND SafeHwnd = GetSafeHwnd();

		// The ApplyNow button is disabled by default, we must enable this
   		HWND hGadget = ::GetDlgItem(SafeHwnd, _R(ID_APPLY_NOW));  				 // Obtain gadgets window
		ERROR3IF(hGadget == NULL, "Could not find Apply now button control on tabbed dialog");
		if(hGadget != NULL)
			::EnableWindow(hGadget, TRUE);                           // Enable window

		//hGadget = ::GetDlgItem(SafeHwnd, _R(ID_HELP));  				 // Obtain gadgets window
		hGadget = ::GetDlgItem(SafeHwnd, IDHELP);  				 // Obtain gadgets window
		ERROR3IF(hGadget == NULL, "Could not find Help button control on tabbed dialog");
		if(hGadget != NULL)
			::EnableWindow(hGadget, TRUE);                           // Enable window

		// Check Cancel button's there and enabled
		hGadget = ::GetDlgItem(GetSafeHwnd(), IDCANCEL);	// Obtain gadgets window
		ERROR3IF(hGadget == NULL, "Could not find Cancel button control on tabbed dialog");
		if (hGadget)
		{
			BOOL ok = ::IsWindowEnabled(hGadget);					// Get current button state
			if(!ok)
				::EnableWindow(hGadget, TRUE);                           // Enable window
		}

		if(!m_bModeless)
			DialogManager::PostCreate(GetSafeHwnd());
	}

	if(Message == WM_COMMAND)
	{
		if((wParam == (WPARAM)IDHELP) || (wParam == (WPARAM)_R(ID_HELP)))
		{
			OurPropShtPage *pActivePage = GetActivePage();
			if(pActivePage != NULL)
			{
				CDlgResID PageID = pActivePage->GetPageID();
				HelpUserPropertyPage((UINT32)PageID);
			}
		}
 	}


	return Res;
}


/********************************************************************************************

>	OurPropSheet::~OurPropSheet()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/11/94
	Purpose:	OurPropertySheet destructor, destroys the property sheet and all pages
				that it contains

********************************************************************************************/


OurPropSheet::~OurPropSheet()
{
	// --- BODGE - see our WindowProc, above for details ---
	if (this == PropSheetWindowProcBodge)	// If we're inside the WindowProc, flag the fact
		PropSheetWindowProcBodge = NULL;	// that we have just committed suicide.
	else
		ERROR3("We could be suciding two OurPropSheets simultaneously - this could be bad!");
	// --- END BODGE ---


	for (INT32 i = 0; i < GetPageCount(); i++)
	{
		delete GetPage(i);
	}
}


// OurPropShtPage methods


/********************************************************************************************

>	OurPropShtPage::OurPropShtPage(CDlgResID DialogResID)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/11/94
	Inputs:		DialogResID: The Dialog resource ID associated with this page
	Purpose:	Constructs a property sheet page

********************************************************************************************/


OurPropShtPage::OurPropShtPage(CDlgResID DialogResID) : CPropertyPage(DialogResID)
{
	PageID =  DialogResID;
	CreateMessageSent = FALSE;
}


/********************************************************************************************

>	LRESULT OurPropShtPage::WindowProc( UINT32 message, WPARAM wParam, LPARAM lParam )

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/4/94
	Inputs:		Usual WinProc
	Purpose:	The OurPropShtPage's WindowProc firstly sends the message to the dialog manager
				so that it can dispatch it to the appropriate DialogBarOp (if neccessary).
				Then the message is processed as normal.

********************************************************************************************/

LRESULT OurPropShtPage::WindowProc( UINT32 Message, WPARAM wParam, LPARAM lParam )
{
	// We need to filter out Page InitDialog messages because
	// a. We are not interested in them
	// b. The first InitDialog message we receive when we are creating a property sheet should
	// 	  be that of the Main Property sheet dialog window
	BOOL result;
	if (Message != WM_INITDIALOG)
	{

		// First send the message to the dialog manager
		result = DialogManager::SendDialogMessage(	GetSafeHwnd(),
					  				     	  			Message,
					       	 							wParam,
					       	 			 				lParam);
	}
	// this checks to see whether this Window is still around !
	// it is possible that it has been deleted
	if(!(IsWindow(GetSafeHwnd()))) return  0;

/*
	See note in DialogManager::SendDialogMessage() for WM_CTLCOLOREDIT (Markn 29/3/95)
	if (Message == WM_CTLCOLOREDIT)
		return result;
*/

	// Now process the message normally
	return(CPropertyPage::WindowProc( Message, wParam, lParam ));
}



/********************************************************************************************

>	BOOL OurPropShtPage::OnSetActive()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/11/94
	Purpose:	We override the OnSetActive method for our property pages so that we can
				inform the dialog when a page has been created.

********************************************************************************************/

BOOL OurPropShtPage::OnSetActive()
{
	BOOL ok = CPropertyPage::OnSetActive(); // This will create the page if it was not previously created

	// Find the WindowID of the PropertySheet
	wxWindow* PropSht = GetParent();
	// Note: the ERROR macro's return ok which is probably the safest thing to do at this point
	ERROR2IF(PropSht == NULL, ok, "Property sheet page has no parent");
	ERROR2IF(!(PropSht->IsKindOf(RUNTIME_CLASS(OurPropSheet))), ok,
		"Parent of property page is not a property sheet");


	if (ok && (!CreateMessageSent))
	{
		// Inform the Dialog that a page  has been created so that it can be initialised

		// SetFont
		//FontFactory::ApplyFontToWindow(GetSafeHwnd(),STOCKFONT_DIALOGBARCLIENT);

		// The first time that OnSetActive is called is in the Create method. At this point the
		// DialogTabOp has not got a window id so we will be broadcasting to deaf ears.	This is
		// ok though (no harm done). CreateMessageSent will be set to FALSE for this page in
		// PostCreate
		BROADCAST_TO_CLASS(DialogMsg(PropSht->GetSafeHwnd(), DIM_CREATE, NULL, 0, PageID), DialogOp);
		CreateMessageSent = TRUE;
	}
	BROADCAST_TO_CLASS(DialogMsg(PropSht->GetSafeHwnd(), DIM_SET_ACTIVE, NULL, 0, PageID), DialogOp);
	return ok;
}

/********************************************************************************************

>	BOOL OurPropShtPage::OnKillActive()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/01/94
	Returns:	Nonzero if data was updated successfully, otherwise 0.
				At the moment we always return TRUE from this fn
	Purpose:	Page loosing active status handling

********************************************************************************************/

BOOL OurPropShtPage::OnKillActive()
{
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Find out the state of the ApplyNow button on the property sheet
	wxWindow* PropSht = GetParent();
	ERROR2IF(PropSht == NULL, TRUE, "Property sheet page has no parent");
	// Find the ApplyNow button
	wxWindow* hApplyNow = PropSht->GetDlgItem(_R(ID_APPLY_NOW));
	ERROR2IF(hApplyNow == NULL, TRUE, "Could not find Apply now button control on tabbed dialog");
	return CPropertyPage::OnKillActive();
}

#endif





/********************************************************************************************

>	static BOOL DialogManager::CreateBar(DialogBarOp* DlgOp)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/12/94
	Inputs:		DlgOp:	The DialogBarOp to create
	Outputs:	-
	Returns:	-
	Purpose:	Handles the creation of a bar
	Scope:		private
	Errors:		-
	SeeAlso:	DialogManager::Create

********************************************************************************************/

BOOL DialogManager::CreateBar(DialogBarOp* DlgOp)
{
	PORTNOTETRACE("dialog","DialogManager::CreateBar - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	BaseBar* DBWnd = NULL;
	// We need to create a DialogBar object
	if ( DlgOp->IsKindOf(CC_RUNTIME_CLASS(SuperGallery)) )
	{
#ifdef EXCLUDE_GALS
		return FALSE;
#else
		DBWnd = new GalleryBar();
#endif
	}
	else
	{
    	DBWnd = new DialogBar();
	}


	if (DBWnd != NULL)
	{
		// Attempt to create the window
		if(!(DBWnd->Create((DialogBarOp*)DlgOp)))  // Should set the error code
		{
			delete DBWnd;
		}
	}
	else
	{

		ERROR1(FALSE, _R(IDS_OUT_OF_MEMORY));  // Failed to create the DialogBar
	}
#endif
	return TRUE;
}

/********************************************************************************************

>	static BOOL DialogManager::CreateTabbedDialog(DialogTabOp* DlgOp, CDlgMode Mode, INT32 OpeningPage)

	Author:		Luke_Hart (Xara Group Ltd) <lukeh@xara.com>
	Created:	04/05/06
	Inputs:		DlgTabOp:		The DialogTabOp to re-layout
	Outputs:	-
	Returns:	-
	Purpose:	Force the dialog to relayout after control hide\show
	Scope:		public
	Errors:		-
	SeeAlso:	DialogManager::CreateTabbedDialog

********************************************************************************************/

void DialogManager::RelayoutDialog( DialogTabOp* pDialogTabOp )
{
	OurPropSheet* pPropSheet = GetPropertySheetFromOp( pDialogTabOp );
	if( NULL != pPropSheet )
		pPropSheet->LayoutDialog();
}


/********************************************************************************************

>	static BOOL DialogManager::CreateTabbedDialog(DialogTabOp* DlgOp, CDlgMode Mode, INT32 OpeningPage)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/12/94
	Inputs:		DlgOp:			The DialogTabOp to create
				CDlgMode:		MODAL/MODELESS
				OpeningPage:	Override the page which is active (defaults to -1)
	Outputs:	-
	Returns:	-
	Purpose:	Handles the creation of a tabbed dialog
				Opening page allows a page to be specified that is not the initial page, which
				will be the default if this is the first time the dialog has been opened or if
				it has been opened before then the initial page will be the last one opened.
	Scope:		private
	Errors:		-
	SeeAlso:	DialogManager::Create

********************************************************************************************/

wxWindow* DialogManager::CreateTabbedDialog(DialogTabOp* pTabDlgOp, CDlgMode Mode, INT32 OpeningPage)
{
	// First check if the OpeningPage parameter is not equal to -1, in which case this
	// specifies the active page to be opened.
	// Has to be an index as otherwise we have not specified the pages yet and so cannot
	// convert the PageID into an index which is what the PropertySheets require.
	UINT32 ActivePageIndex = 0;
	if (OpeningPage != -1)
	{
		// Set the index of the page that is to be the active one to the one that has
		// been specified.
		ActivePageIndex = OpeningPage;
	}
	else
	{
PORTNOTE( "dialog", "Remove mainDlgID usage" );
#ifndef EXCLUDE_FROM_XARALX
		// Determine if this dialog has been created before
		DialogPosition* pPosDetails = FindDialogPositionRecord(DialogManager::mainDlgID);
		if (pPosDetails != NULL)
		{
			// The dialog has been created before so find out the index of the active page
			ActivePageIndex = pPosDetails->ActivePageIndex;
		}
#endif
	}

	// ok first try and create the property sheet
	OurPropSheet* pPropertySheet;
	// Because OurPropSheet is derived from an MFC object we have to cope with exceptions
	try
	{
		pPropertySheet = new OurPropSheet( GetMainFrame(), pTabDlgOp->GetName(), ActivePageIndex );
//		pPropertySheet->Create(WS_POPUP | WS_SYSMENU | WS_BORDER | WS_DLGFRAME, 0)
	}
	catch( CMemoryException )
	{
		ERROR1(FALSE, _R(IDS_OUT_OF_MEMORY));
	}

	// Just to  be safe
	ERROR1IF(pPropertySheet == NULL, FALSE, _R(IDS_OUT_OF_MEMORY));

	// We need to store a mapping from the property sheet that we have just created
	// to the DialogTabOp that it is associated with.We can't use a Cwnd to find the
	// property sheet cos we ain't got one yet !

	DlgTagOpToPropShtItem* Item = new DlgTagOpToPropShtItem;
	if (Item == NULL)
	{
		// tidy-up
		delete pPropertySheet;
		ERROR1(FALSE, _R(IDS_OUT_OF_MEMORY));
	}

	// Store details in the Item
	Item->pDialogTabOp = pTabDlgOp;
	Item->pPropertySheet = pPropertySheet;

	// Now add the Item to the 	DlgTagOpToPropShtList
	DlgTagOpToPropShtList.AddHead(Item);

	pPropertySheet->CreateButtons( wxOK|wxCANCEL|wxHELP );

	// This will be done again later, but RegisterYourPagesInOrderPlease may,
	// use it, so we do it here as well
	pTabDlgOp->WindowID = (CWindowID)pPropertySheet;

	// Before we can create the property sheet we must add pages to it.
	// Let's ask the op do do this for us
	if (!(pTabDlgOp->RegisterYourPagesInOrderPlease()))
	{
		// We failed to add pages to the dialog so we must tidy-up and fail
		// Because we have added the  DlgTagOpToPropShtItem to a list
		// we can delete this and the property sheet itself a bit later. If we
		// don't do this then things get too complex.
		return FALSE;
	}

	// Get the dialog sized to fit
	pPropertySheet->LayoutDialog();

	// Now that the pages have been registered, check if the dialog has been opened
	// before. If so force the new ActivePage to be specified rather than the old.
	if (OpeningPage != -1)
	{
PORTNOTE( "dialog", "Remove mainDlgID usage" );
#ifndef EXCLUDE_FROM_XARALX
		// Determine if this dialog has been created before
		DialogPosition* pPosDetails = FindDialogPositionRecord(DialogManager::mainDlgID);
		if (pPosDetails != NULL)
		{
			// The dialog has been created before so check if the specified page was the
			// last active one. If it was then everything should be ok.
//			if (OpeningPage != pPosDetails->ActivePageIndex)
//			{
				ERROR3IF(pPropSheet == NULL, "There is no current PropertySheet");
				OurPropShtPage* pPage = (OurPropShtPage*)pPropSheet->GetActivePage();
				ERROR3IF(pPage == NULL, "There is no active page");
				pPosDetails->ActivePage = pPage->GetPageID();
TRACEUSER( "MarkH", _T("CreateTabbedDialog ActivePage = %d\n"),pPosDetails->ActivePage);
//				pPosDetails->ActivePage = 27666;
				pPosDetails->ActivePageIndex = OpeningPage;
//			}
		}
#endif
	}

	return pPropertySheet;
}






// Old windows routine follows
#if 0
/********************************************************************************************

>   static BOOL CALLBACK EXPORT SendDialogMessage(HWND hDlg,
				       	  						  UINT32 Message,
				       	 						  UINT32 wParam,
				       	 						  INT32 lParam);

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/8/93

	Inputs:		hDlg:  			Dialog window
				UINT32 Message:   Windows message
				UINT32 wParam:    wParam
				INT32 lParam:    lParam
	Outputs:	-
	Returns:	-
	Purpose:	All messages generated by all dialogs in the Windows dialog system will be
				sent to this callback

				The types of this methods variables will change depending on the platform. It
				receives a platform dependant message, translates it into a DIM and then calls
				the DialogOp's RouteMessage function.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL CALLBACK EXPORT DialogManager::SendDialogMessage( wxWindow *pDlg,
				       	  						  UINT32 Message,
				       	 			   			  UINT32 wParam,
				       	 			   			  INT32 lParam )
{
	PORTNOTETRACE("dialog","DialogManager::SendDialogMessage - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	HandleMessage = FALSE;	// Flag indicating if the message received is handled

	INT32 DlgMsgParam = 0;	// Arbitrary INT32 which has meaning with some messages
							// This gets set by the case statements below and then passed
							// on to Operations by the BROADCAST_TO_CLASS macro at the bottom.
	Gadget = 0xFFFFFFF0;

	// ----------------------------------------------------------------------------------
	// Determine if the message was broadcast from a page within a Property sheet, if it
	// was then set the hDlg to that of the property sheet, and find the PageID
	PageID = 0;
	HWND BroadcastWindow = hDlg; // The window we should broadcast to.

	wxWindow* pCWnd = CWnd::FromHandlePermanent(hDlg);

	if (pCWnd != NULL)
	{
		if (pCWnd->IsKindOf(RUNTIME_CLASS(OurPropShtPage)))
		{
			wxWindow* pPropertySheet = pCWnd->GetParent();
			if (pPropertySheet)
			{
				ERROR2IF(!(pPropertySheet->IsKindOf(RUNTIME_CLASS(OurPropSheet))), FALSE,"Property page parent not a property sheet");
				BroadcastWindow = pPropertySheet->GetSafeHwnd(); // The destination of the message
				ERROR2IF(BroadcastWindow == NULL, FALSE, "Property sheet window handle is NULL");
				// We need to find the resource ID of the property sheet page
				PageID = ((OurPropShtPage*)pCWnd)->GetPageID();
			}
			else
			{
				// I'm not sure what this message is,  but we can't do much with it !
				return FALSE;
			}
		}
	}
	// ----------------------------------------------------------------------------------

	BOOL RemoveFocus = FALSE;

//  Removed by Phil on request from Simon, 11/05/94
//	Gadget = 0; // Set to 0 to ensure Info Bar messages are routed ok.

#if WIN16
	WORD Ctl = HIWORD(lParam);

	if (Message == WM_CTLCOLOUR && Ctl == CTLCOLOR_EDIT)
		Message = WM_CTLCOLOREDIT;
#endif

	// Set Current properly by setting it to be the same as Selected. Dialogs should only apply
	// to the Selected Document/View...
	Document::GetSelected()->SetCurrent();
	DocView::GetSelected()->SetCurrent();

	switch (Message)
	{
		case WM_MOVE:
		{
			//DY 4/5/2000 we need to know about this now to update the screen when drawing brushes
			HandleMessage = TRUE;
			DIM = DIM_DLG_MOVED;
		}
		break;

		// This responds to the custom "telephone keypad" message.
		case WM_GRIDBN_PRESSED:
			DIM = DIM_GRID_BUTTON_DOWN;
			HandleMessage = TRUE;
			Gadget = LOWORD(lParam);
			DlgMsgParam = (INT32) HIWORD(lParam);
			break;

		case WM_ACTIVATE:
			// The following bit of code is required so that we only need one IsDialogMessage
			// for many Modeless dialogs
			if (wParam == 0)
			{
				// The dialog is becoming inactive
				hDlgCurrent = NULL;
			}
			else
			{
				hDlgCurrent = hDlg;
			}
			return TRUE;

		case WM_QUERYNEWPALETTE:
			// A Window has become the focus and may select a palette if it likes
			// We always call the palette manager, which makes sure that Gavin's palette
			// is used and GDraw is using the right colours.
			PaletteManager::EnsurePalette(BroadcastWindow, FALSE);
			return(TRUE);

		case WM_PALETTECHANGED:
			// The palette has changed, so ensure we realise our palette in the background
			// but only if it wasn't us who caused this to happen!
			if (hDlg != (HWND)wParam)
				PaletteManager::EnsurePalette(BroadcastWindow, TRUE);
			break;


		case WM_COMMAND:
			Gadget = GET_WM_COMMAND_ID(wParam, lParam);

			// The command ID is only allowed to be a short
			// however the _R(ID_APPLY_NOW) control within a property sheet has a value 59910 so
			// it gets corrupted. So we need to convert it to are own ID value which can fit
			// into a short.
			if (Gadget == _R(ID_APPLY_NOW))
			{
				Gadget = _R(ID_CC_APPLY_NOW);
			}

			// the short casts are here so the sign gets propogated correctly under win32
			if (!CustomControlMsg(hDlg, wParam, lParam))
			{
				DialogManager::Dlg_OnCommand( hDlg,
												(short)Gadget,
												GET_WM_COMMAND_HWND(wParam, lParam),
												(short)GET_WM_COMMAND_CMD(wParam, lParam)
											);

				if (HandleMessage && DIM == DIM_SELECTION_CHANGED)
					RemoveFocus = TRUE;
			}
			break;

		case CBN_CAM_SELCHANGE:
			// Handle a previously sent CBN_SELCHANGE/CBN_DBLCLK on a combo.
			DIM = (CDlgMessage) wParam;
			Gadget = (CGadgetID) lParam;
			HandleMessage = TRUE;
			break;

		case WM_CTL_COMMIT:
			// Handle subclassed bar Edit and Combo controls, send a selection changed message
			Gadget = wParam;
			DIM = DIM_SELECTION_CHANGED;

			// The LPARAM component of the message contains the virtual key code that caused the
			// commit.
			if (lParam == CAMKEY(RETURN)) DlgMsgParam = ENTER_COMMIT;
			else if (lParam == CAMKEY(TAB)) DlgMsgParam = TAB_COMMIT;
			else DlgMsgParam = NO_COMMIT;

			HandleMessage = TRUE;
			RemoveFocus = TRUE;
			break;

		case WM_SIZEPARENT:
			// This should only be received for the InfoBar, so send it on.
			CMainFrame::pInfoBar->OnSizeParent(wParam, lParam);
			break;

		case WM_INITDIALOG:
			if (!lParam)		// Dialog is modal, we need to call PostCreate
			{
				if (!PostCreate(hDlg))
				{
					// We failed to create a modal dialog
					EndDialog(hDlg,FALSE); 	// It is ok to call this in WM_INITDIALOG
				}
			}
			return !FocusSetDuringInit;

		// Win32 and Win16 handle colour messages a bit differently - Win32 has separated them
		// out into different messages.
		// This code causes our dialogs and static controls to have a grey background.

		// A scrollbar message ?
		case WM_VSCROLL:
		case WM_HSCROLL:
			Gadget = GetDlgCtrlID( GET_WM_HSCROLL_HWND(wParam,lParam) );
			HANDLE_WM_VSCROLL(hDlg, wParam, lParam, DialogManager::Dlg_OnScroll);
			break;

		// Handle mouse button up/down events
		// Gadget IDs will be correct for custom controls but not for normal controls
		// So in the left button cases find the control ID from the supplied position
		case WM_LBUTTONDOWN:
		{
			INT32 xPos = LOWORD(lParam);  // horizontal position of cursor
			INT32 yPos = HIWORD(lParam);  // vertical position of cursor
			POINT point = { xPos, yPos };
			HWND HitWnd = ChildWindowFromPoint(hDlg, point);
			if (HitWnd != NULL)
				Gadget = GetDlgCtrlID(HitWnd); // handle of control
			HandleMessage = TRUE;
			DIM = DIM_LFT_BN_DOWN;
			break;
		}
		case WM_LBUTTONUP:
		{
			INT32 xPos = LOWORD(lParam);  // horizontal position of cursor
			INT32 yPos = HIWORD(lParam);  // vertical position of cursor
			POINT point = { xPos, yPos };
			HWND HitWnd = ChildWindowFromPoint(hDlg, point);
			if (HitWnd != NULL)
				Gadget = GetDlgCtrlID(HitWnd); // handle of control
			HandleMessage = TRUE;
			DIM = DIM_LFT_BN_UP;
			//Gadget = GetDlgCtrlID(hDlg); // handle of control
			break;
		}

		// Gadget IDs will be correct for custom controls but not for normal controls
		case WM_RBUTTONDOWN:
			HandleMessage = TRUE;
			DIM = DIM_RGT_BN_DOWN;
			//Gadget = GetDlgCtrlID(hDlg); // handle of control, returns 0
			break;
		case WM_RBUTTONUP:
			HandleMessage = TRUE;
			DIM = DIM_RGT_BN_UP;
			//Gadget = GetDlgCtrlID(hDlg); // handle of control, returns 0
			break;

		case WM_MOUSEWHEEL:
		{
			INT32 xPos = LOWORD(lParam);  // horizontal position of cursor
			INT32 yPos = HIWORD(lParam);  // vertical position of cursor
			POINT point = { xPos, yPos };
			HWND HitWnd = ChildWindowFromPoint(hDlg, point);
			if (HitWnd != NULL)
				Gadget = GetDlgCtrlID(HitWnd); // handle of control
			HandleMessage = TRUE;
			if ((short)HIWORD(wParam) > 0)
			{
				DIM = DIM_MOUSEWHEEL_UP;
			}
			else
			{
				DIM = DIM_MOUSEWHEEL_DOWN;
			}
			break;
		}


		// Handle Rik's slider notification messages...

		case WMN_POSITION_CHANGING:
			Gadget = GetWindowID((HWND)wParam);
			HandleMessage = TRUE;
			DIM = DIM_SLIDER_POS_CHANGING;
			DlgMsgParam = lParam;			// Pass slider pos on to Ops
			break;
		case WMN_POSITION_IDLE:
			Gadget = GetWindowID((HWND)wParam);
			HandleMessage = TRUE;
			DIM = DIM_SLIDER_POS_IDLE;
			DlgMsgParam = lParam;			// Pass slider pos on to Ops
			break;
		case WMN_POSITION_SET:
			Gadget = GetWindowID((HWND)wParam);
			HandleMessage = TRUE;
			DIM = DIM_SLIDER_POS_SET;
			DlgMsgParam = lParam;			// Pass slider pos on to Ops
			break;


		// A Windows font table change has occured.
		// Something or somebody has added or removed a font from the windows
		// font table.

		case WM_FONTCHANGE:
			HandleMessage = TRUE;
			DIM = DIM_FONTCHANGE;
			break;

		// Somebody has asked for a timer event and here we have recieved one.
		case WM_TIMER:
			// Parameters are:-
			// wTimerID = wParam;            // timer identifier
			// tmprc = (TIMERPROC *) lParam; // address of timer callback
			DlgMsgParam = wParam;	// pass on the timer id just in case dialog wants it
			HandleMessage = TRUE;
			DIM = DIM_TIMER;
			break;

//		// Somebody has changed something in the International Section
//		// of the Windows Control Panel. So note the change and force things
//		// to update the current display
//
//		case WM_WININICHANGE:
//		{
//			// Cause the conversion system to reload the values from the system
//			InitConvert();
//			// Now tell people to update themselves
//			Document * pDocument = Document::GetSelected();
//			BROADCAST_TO_ALL(OptionsChangingMsg(pDocument, OptionsChangingMsg::OptionsState::NEWUNITS));
//			break;
//		}


		// Handle mouse moves/drags

		// These were removed by Simon 25/11 cos they are unlikely to be needed any longer and
		// make debugging a pain.

		// MOUSEMOVE was restored, because its needed in the bitmap export options dialog
		case WM_MOUSEMOVE:
		{
			if (!(wParam & (MK_LBUTTON | MK_RBUTTON | MK_MBUTTON)))
			{
				HandleMessage = TRUE;
				DIM = DIM_MOUSE_MOVE;
			}
			break;
		}

		case WM_NOTIFY:
		{
			HWND hChild = GetDlgItem(hDlg, INT32(wParam));
			TCHAR buffer[32];
			GetClassName(hChild, buffer, sizeof(buffer));
			if (String_32(buffer) == String_32(TEXT("SysTreeView32"))) // so the notification comes from a treeview
			{
				LPNMHDR pNMHDR = (LPNMHDR) lParam;
				switch (pNMHDR->code)
				{
					// We could handle much more here, but this is rather useful...
					case TVN_SELCHANGED:
					{
						HandleMessage = TRUE;
						Gadget = wParam;
						DIM = DIM_SELECTION_CHANGED;
						DlgMsgParam = lParam;
					}
				}
			}

			if (String_32(buffer) == (LPCTSTR)CCustomList::WNDCLASSNAME) // so the notification comes from our customlist
			{
				LPNMHDR pNMHDR = (LPNMHDR) lParam;
				CCustomList* pListGadget = CCustomList::GetGadget((CWindowID) hDlg, (CGadgetID) pNMHDR->idFrom);
				BOOL DoubleClick = FALSE;	// Flag to allow sharing of click/double-click code

				if (pListGadget)
				{
					// the gadget is notifying us that something happened
					HandleMessage = TRUE;
					Gadget = wParam;
					if(pNMHDR->code == NM_DBLCLK)
					{
						DIM = DIM_SELECTION_CHANGED_COMMIT;

					}
					else
					{
						DIM = DIM_SELECTION_CHANGED;
					}
				}
				else
				{
					ERROR3("Failed to find list gadget");
				}
			}

			if (String_32(buffer) == String_32(TEXT("msctls_updown32"))) // so the notification comes from an updown control
			{
				NM_UPDOWN* pnmud = (NM_UPDOWN FAR *) lParam;

				if (pnmud->iDelta > 0)
				{
					SpinControlMessageUp = TRUE;
					HandleMessage = FALSE;
				}
				else if (pnmud->iDelta < 0)
				{
					SpinControlMessageDown = TRUE;
					HandleMessage = FALSE;
				}
				else
				{
					ERROR3 ("A Fatal error has occured within the OS!");
				}
			}
		}
		break;




//		case WM_DESTROY:
//			// Call CCListGadget::OnDialogClose() so that it destroys any list controls there may be on the form
//			CCustomList::OnDialogClose((CWindowID) hDlg);
//			break;


		// sent when a change is made to a system color setting - forward message to controls
		case WM_SYSCOLORCHANGE:
		{
			HWND hChild = GetDlgItem(hDlg, INT32(wParam));
			if(hChild)
			{
				::SendMessage(hChild, WM_SYSCOLORCHANGE, 0, 0);
			}
			return TRUE;
			break;
		}

		case WM_DLGDRAWEVENT:	// (Mouse) event from a cc_DialogDraw custom control [Jason: 2/12/94]
		{
			DlgDrawEventInfo *Info = (DlgDrawEventInfo *) lParam;
			ERROR3IF(Info == NULL, "NULL DilaogDraw EventInfo passed to DialogMgr?!");

			switch(Info->msg)
			{
				case WM_LBUTTONDOWN:
					DIM = DIM_LFT_BN_DOWN;
					break;

				case WM_LBUTTONUP:
					DIM = DIM_LFT_BN_UP;
					break;

				case WM_RBUTTONDOWN:
					DIM = DIM_RGT_BN_DOWN;
					break;

				case WM_RBUTTONUP:
					DIM = DIM_RGT_BN_UP;
					break;

				case WM_MOUSEMOVE:
					if (Info->wParam & MK_LBUTTON)
						DIM = DIM_MOUSE_DRAG;
					else
						DIM = DIM_MOUSE_MOVE;
					break;

				case WM_MOUSEWHEEL:
					if ((short) HIWORD(Info->wParam) > 0)
					{
						//MouseWheel scrolled upwards
						DIM = DIM_MOUSEWHEEL_UP;
					}
					else
					{
						//MouseWheel scrolled downwards...
						DIM = DIM_MOUSEWHEEL_DOWN;
					}
					break;

				default:
					ERROR3("Unknown Event type passed in WM_DLGDRAWEVENT message");
					break;
			}

			// Set up a MousePosInfoType containing the click position info, and information
			// that will come in handy (size of the gadget, and screen DPI)
			ReDrawInfoType ExtraInfo;

			ExtraInfo.pDC = NULL;		// No redraw info for mouse events
			ExtraInfo.pClipRect = NULL;

			// Get the screen DPI
			HDC ScreenDC = CreateCompatibleDC(NULL);
			if (ScreenDC == NULL)
			{
				ERROR2RAW("DialogManager::SendDialogMessage: Unable to create screen DC");
				break;
			}
			ExtraInfo.Dpi = GetDeviceCaps(ScreenDC, LOGPIXELSY);
			DeleteDC(ScreenDC);

			// Calculate how big the window is, in MILLIPOINTS
			RECT WindowSize;
			if (!GetClientRect(Info->hwnd, &WindowSize))
			{
				ERROR2RAW("GetClientRect failed in DialogManager::SendDialogMessage");
				break;
			}

			ExtraInfo.dx = (((INT32)WindowSize.right)*72000) / ExtraInfo.Dpi;
			ExtraInfo.dy = (((INT32)WindowSize.bottom)*72000) / ExtraInfo.Dpi;

			// Work out the MILLIPOINT coordinates of the mouse position
			// Note that the Y value is flipped, as the kernel-origin is at the bottom left
			INT32 XPos = (INT32) MAKEPOINTS(Info->lParam).x;
			INT32 YPos = (INT32) MAKEPOINTS(Info->lParam).y;

			DocCoord MousePos;
			MousePos.x = (XPos * 72000) / ExtraInfo.Dpi;
			MousePos.y = ExtraInfo.dy - ((YPos * 72000) / ExtraInfo.Dpi);
			ExtraInfo.pMousePos = &MousePos;

			// Now, send the message to the DialogOp to be handled.
			// Note that we leave HandleMessage == FALSE so the caller doesn't send the
			// message a second time!

			BROADCAST_TO_CLASS(DialogMsg(BroadcastWindow, DIM, Info->ControlID,	(INT32)&ExtraInfo, PageID), DialogOp);
			break;
		}


		case WM_REDRAW :
		{
			if (CCamApp::IsDisabled())
			{
				TRACE( _T("kernel-rendered gadget repaint has been aborted: the system is disabled (due to an error/ensure?)\n"));
				HandleMessage = FALSE;
				break;
			}

			// Find out about the paint message and fill in the details in the kernel message
			RedrawInfo* pInfo = (RedrawInfo*) lParam;

			HDC hDC = pInfo->PaintInfo.hdc;
			HPALETTE OldPalette = PaletteManager::StartPaintPalette(hDC);

			ReDrawInfoType ExtraInfo;

			ExtraInfo.pMousePos = NULL;		// No mouse position info for redraw events

			// Build a CC dc out of it for rendering to the screen
			// Get a MFC CDC to put the DC in
			CCDC MyDc(RENDERTYPE_SCREEN);
			MyDc.Attach(pInfo->PaintInfo.hdc);
			ExtraInfo.pDC = &MyDc;

			// The devices DPI
			ExtraInfo.Dpi = MyDc.GetDeviceCaps(LOGPIXELSY);

			// How big the window is
			RECT WindowSize;
			if (GetClientRect((HWND)wParam, &WindowSize))
			{
				ExtraInfo.dx = (((INT32)WindowSize.right)*72000) / ExtraInfo.Dpi;
				ExtraInfo.dy = (((INT32)WindowSize.bottom)*72000) / ExtraInfo.Dpi;
			}

			// Work out the size of the invalidated region
			RECT* ClipRect = &pInfo->PaintInfo.rcPaint;
			DocRect DocClipRect;

			// Convert to millipoints, Also need to flip the y coords to get a
			// rectangle in with the origin in the bottom left.
			DocClipRect.lo.x = (ClipRect->left * 72000) / ExtraInfo.Dpi;
			DocClipRect.lo.y = ExtraInfo.dy - ((ClipRect->bottom * 72000) / ExtraInfo.Dpi);

			DocClipRect.hi.x = (ClipRect->right * 72000) / ExtraInfo.Dpi;
			DocClipRect.hi.y = ExtraInfo.dy - ((ClipRect->top * 72000) / ExtraInfo.Dpi);

			// Set the pointer in the extra info structure
			ExtraInfo.pClipRect = &DocClipRect;

			// Build the message and send it to the dialog op
			// It is up to the dialog op to build a render region etc and attach the CCDC to it
			// and to tidy the region up after it has finished drawing in it CDlgMessage
			BROADCAST_TO_CLASS(DialogMsg(BroadcastWindow, DIM_REDRAW, pInfo->ControlID, (INT32) &ExtraInfo, PageID), DialogOp);

			// Give back the dc
			MyDc.Detach();

			if (OldPalette)
				PaletteManager::StopPaintPalette(hDC, OldPalette);
			break;
		}

		case WM_DRAWITEM:
			{
				// First, check if this is for a (Colour/Font) DropDown combobox, and return TRUE if it handles it
				if (DropDown::HandleDrawItem(hDlg, wParam, lParam))
					return(TRUE);
				// Nope - it must be a custom control, so pass the message on to it
				LPDRAWITEMSTRUCT lpdis = (LPDRAWITEMSTRUCT) lParam;

				// If the custom control redraws this, we return TRUE so that the OS/MFC
				// does not go rampantly drawing selection rects around the item or anything
				if (SendMessage( lpdis->hwndItem,
								 DialogManager::MsgSlaveDrawItem, wParam, lParam ))
					return(TRUE);
			}
			break;

		case WM_MEASUREITEM:
			{
				// Get the colour drop-down list to handle this. Note that it currently always handles
				// this case - if you need to pass it to another control, check first and drop through
				// to this code if you don't handle it.

				if (DropDown::HandleMeasureItem(hDlg, wParam, lParam))
					return(TRUE);
			}
			break;

		//case WM_WINDOWPOSCHANGING:
		//	static Bodge=0;
		//	if (Bodge >= 1)
		//	{
		//		// If the dialog is modal then do not let the windows Z order change
	    //			WINDOWPOS FAR* pwp = (WINDOWPOS FAR*) lParam;  // address of WINDOWPOS structure
		//		pwp->flags = SWP_NOOWNERZORDER;
		//	}
		//	Bodge++;
		//	break;


		// --- Bodge handlers for input focus handling in the colour editor
		// Thanks to the sheer marvellousness of Windows disloague handling, we can't stop the focus
		// going into our dialogue on MouseActivate (normally, we'd just return MA_NOACTIVATE and be
		// happy without the focus). The 17th and final solution (the only one that works) is that
		// when a click occurs on the window titlebar, or when the window has been moved, we warn the
		// dialogue about it, and the colour dialogue schedules an idle-event processor, which pushes
		// the focus back to the document. This has to be done on idles so that the focus is pushed
		// back immediately after the move occurs (if we change the focus immediately, it goes away
		// until the mouse button is released, at which point Windows kindly gives us the focus again!)

		// Actually (25/2/2000) there is a much more suitable (and easier) way of getting our
		// colour editor dialog to do this - we simply process the WM_EXITSIZEMOVE mesage!
		// The message is common to Windows 96/98 and NT (CGS) ....  I discovered this when I
		// made the profile dialog behave in a similiar fashion.

		case WM_MOUSEACTIVATE:		// Look out for clicks on the title bar
			if (((INT32) LOWORD(lParam)) != HTCAPTION)
				return(FALSE);
			else
				return (MA_NOACTIVATE);		// return this - even if windows don't want to listen
											// to us; cause afterall, its what we want ....

		case WM_EXITSIZEMOVE:
			DIM = DIM_TITLEFOCUSWARN;		// throw that focus baby!
			BROADCAST_TO_CLASS( DialogMsg(BroadcastWindow, DIM, Gadget, DlgMsgParam, PageID), DialogOp );
			// yo buddy, I've handled the message !
			return (TRUE);



		break;
		// added by Karim 30/9/99 to catch text typed into the font combo-box and change the
		// selection within the combo-box to match whatever the user typed as closely as possible.
		case WM_KEYDOWN:
			{
				// check whether we want this message - ie is the font
				// combo-box a child of the message'd window.
				if (::GetDlgItem(hDlg, _R(IDC_FONT_COMBO)) != NULL &&
											(TCHAR)wParam >= 'A' &&
											(TCHAR)wParam <= 'Z')
				{
					// if the character just typed was entered within a specified
					// timeframe, 'cat it to the end of a comparison string.
					static DWORD dwTime = 0;
					static String_256 strCharsTyped = "";

					DWORD dwNow = ::GetTickCount();
					if ((dwNow - dwTime)  > 1000 || strCharsTyped.Length() > 255)
						strCharsTyped.Empty();

					strCharsTyped += (TCHAR)wParam;

					// look for the first font whose description starts with
					// the comparison string.
					BOOL bMatch = FALSE;
					FontDropItem* fontItem = NULL;
					HWND hwndCombo = ::GetDlgItem(hDlg, _R(IDC_FONT_COMBO));
					INT32 numItems = ComboBox_GetCount(hwndCombo);
					for (INT32 i = 0; !bMatch && i < numItems; i ++)
					{
						fontItem = (FontDropItem*)ComboBox_GetItemData(hwndCombo, i);
						if (fontItem != NULL)
						{
							bMatch = (camStrnicmp(fontItem->GetFontName(),
								strCharsTyped,
								camStrlen(strCharsTyped)) == 0);
						}
					}

					// if we found the font, lets go to it
					if (bMatch)
						::SendMessage(hwndCombo, CB_SETCURSEL, i - 1, 0);
					else
						strCharsTyped.Empty();

					// set our timer to the current time.
					dwTime = ::GetTickCount();

					return TRUE;	// this message has been processed.
				}
			}
		break;
	}

	// Handle messages from draggable list boxes...
	static LPDRAGLISTINFO lpdli;
	static UINT32 iItemToMove, iItem;

	if (Message == DialogManager::MsgDragList)
	{
	#if USE_COMMON_CONTROLS
	    lpdli = (LPDRAGLISTINFO)lParam;
    	switch (lpdli->uNotification)
    	{
    		case DL_BEGINDRAG:
        		iItemToMove = LBItemFromPt(lpdli->hWnd, lpdli->ptCursor, TRUE);
        		return DL_MOVECURSOR;
    		case DL_DRAGGING:
	        	iItem = LBItemFromPt(lpdli->hWnd, lpdli->ptCursor, TRUE);
    	    	DrawInsert(hDlg, lpdli->hWnd, iItem);
        		if (iItem!=-1)
            	 	return DL_MOVECURSOR;
	        	return DL_STOPCURSOR;
    		case DL_DROPPED:
        		iItem = LBItemFromPt(lpdli->hWnd, lpdli->ptCursor, TRUE);
 	  	     	//if (iItem!=-1)
				//{
					//SetStringGadgetValue(
					//ListBox_InsertString(hGadget, iItem, (INT32)(TCHAR*)(*StrVal))
    	        	//Use list box messages to insert item at new location
       		 	//}
       		 	DrawInsert(hDlg, lpdli->hWnd, -1);
       			return DL_CURSORSET;
			default:
				if (IsUserName("Simon"))
					TRACE( _T("Unknown draglist message 0x%x\n"), lpdli->uNotification);

    	}
	#endif
    }

	// If the message should be handled then Route it to the appropriate DialogOp
	if (HandleMessage)
	{
		BROADCAST_TO_CLASS( DialogMsg(BroadcastWindow, DIM, Gadget, DlgMsgParam, PageID), DialogOp );
	}
#endif
	return HandleMessage;
}


/********************************************************************************************

>	void DialogManager::Dlg_OnCommand(HWND hwnd, INT32 id, HWND hwndCtl, UINT32 codeNotify)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/9/93
	Purpose:	OnCommand message handler. Translates a windows Command message into a DIM
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void DialogManager::Dlg_OnCommand( wxWindow *pWnd, INT32 id, wxWindow *pWndCtl, INT32 codeNotify )
{
	PORTNOTETRACE("dialog","DialogManager::Dlg_OnCommand - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	String_256 ClassNameStr;
    GetClassName(hwndCtl, (TCHAR*)ClassNameStr, 255);

	// -------------------------------------------------------------------------------------
	// Special processing

	// Commit processing
	if (id == IDOK)
	{
		if (codeNotify == BN_RGT_CLICKED)
		{
			DialogManager::DIM = DIM_SOFT_COMMIT;
			DialogManager::HandleMessage = TRUE;
			return;
		}
		else
		{
			// Either left button clicked or ENTER was pressed in a control.
			DialogManager::DIM = DIM_COMMIT;
			DialogManager::HandleMessage = TRUE;
			SetGadgetIDToFocus(hwnd);
			//Move focus to the next control in the dialog.
         	PostMessage(hwnd, WM_NEXTDLGCTL, 0, 0L) ;
			return;
		}
	}
	else if (id == _R(ID_CC_APPLY_NOW))
	{
		// Clicking on the apply now button is the same as a soft commit
		DialogManager::DIM = DIM_SOFT_COMMIT;
		DialogManager::HandleMessage = TRUE;
		return;

	}
	else if (id == IDCANCEL)
	{
		DialogManager::DIM = DIM_CANCEL;
		DialogManager::HandleMessage = TRUE;
		SetGadgetIDToFocus(hwnd);
		return;
	}

	// -------------------------------------------------------------------------------------
	// Button control messages
	if ((ClassNameStr == String_8(TEXT("Button"))))
	{
		switch (codeNotify)
		{
			case BN_RGT_CLICKED:
				DialogManager::DIM = DIM_RGT_BN_CLICKED;
				DialogManager::HandleMessage = TRUE;
				break;

			case BN_CLICKED:
				DialogManager::DIM = DIM_LFT_BN_CLICKED;
				DialogManager::HandleMessage = TRUE;
				break;
		}
	}
	else if ( (ClassNameStr == String_8(TEXT("ListBox"))) ||  // ListBox control messages
			  (ClassNameStr == String_16(TEXT("cc_CheckList")))
			)
	{
		switch (codeNotify)
		{
			case LBN_ERRSPACE:
				DialogManager::DIM = DIM_OUT_OF_SPACE;
				DialogManager::HandleMessage = TRUE;
				break;
			case LBN_SELCHANGE:
				DialogManager::DIM = DIM_SELECTION_CHANGED;
				DialogManager::HandleMessage = TRUE;
//				::SetFocus(NULL);		// This is done for us by SendDialogMessage
										// If we do it here, DIM_SEL_CHANGED is NOT broadcast!
				break;
			case LBN_DBLCLK:
				DialogManager::DIM = DIM_SELECTION_CHANGED_COMMIT;
				DialogManager::HandleMessage = TRUE;
				break;
			case LBN_SELCANCEL:
				DialogManager::DIM = DIM_SELECTION_CANCELED;
				DialogManager::HandleMessage = TRUE;
				break;
		}
	}
	else if ( (ClassNameStr == String_8(TEXT("ComboBox"))) || // ComboBox control messages
			  (ClassNameStr == String_64(TEXT("cc_1dBitmapComboBoxEdit"))) ||
			  (ClassNameStr == String_64(TEXT("cc_2dBitmapComboBoxEdit")))
			)
	{
//		BOOL SetComboText = FALSE;
		switch (codeNotify)
		{
			case CBN_KILLFOCUS:
				// If a combo box loses the input focus, broadcast this fact. It can be
				// used as a 'confirm' message for the contents of the combo box editable
				// field which will work under all circumstances (pressing return or tab,
				// Alt-tabbing to another application, closing the window, clicking on
				// another control/window/application, etc etc), as opposed to other
				// 'confirm' information which you only get *sometimes*.
				DialogManager::DIM = DIM_FOCUS_LOST;
				DialogManager::HandleMessage = TRUE;
				break;
			case CBN_DROPDOWN:
				// If a combo box loses the input focus, broadcast this fact. It can be
				// used as a 'confirm' message for the contents of the combo box editable
				// field which will work under all circumstances (pressing return or tab,
				// Alt-tabbing to another application, closing the window, clicking on
				// another control/window/application, etc etc), as opposed to other
				// 'confirm' information which you only get *sometimes*.
				DialogManager::DIM = DIM_LISTDROPPED;
				DialogManager::HandleMessage = TRUE;
				break;
			case CBN_ERRSPACE:
				DialogManager::DIM = DIM_OUT_OF_SPACE;
				DialogManager::HandleMessage = TRUE;
				break;
			case CBN_SELCHANGE:
				::PostMessage(hwnd, CBN_CAM_SELCHANGE,
							  (WPARAM) DIM_SELECTION_CHANGED,
							  (LPARAM) Gadget);
				// Removed by Chris 22 March 95
				// fixes can't cursor through combo's bug
				//::SetFocus(NULL);						// don't do this - it's senseless
				break;

			case CBN_DBLCLK:		// A "Simple" listbox item has been double-clicked
			case CBN_SELENDOK:		// The user has confirmed their choice (seems to miss kbd input!)
			case CBN_CLOSEUP:		// The control's dropdown is being closed
				::PostMessage(hwnd, CBN_CAM_SELCHANGE,
							  (WPARAM) DIM_SELECTION_CHANGED_COMMIT,
							  (LPARAM) Gadget);
				break;

			case CBN_EDITCHANGE:						   // Text in edit portion of combo has changed
				DialogManager::DIM = DIM_TEXT_CHANGED;
				DialogManager::HandleMessage = TRUE;
				break;
		}
	}

	else if (ClassNameStr == String_8(TEXT("Edit"))) // ComboBox control messages
	{
		switch (codeNotify)
		{
			case EN_CHANGE:						   // Text in edit control has changed
				DialogManager::DIM = DIM_TEXT_CHANGED;
				DialogManager::HandleMessage = TRUE;
				break;

			case EN_SETFOCUS:						// Edit control has focus (i.e. been clicked on, tabbed to, etc)
				DialogManager::DIM = DIM_SET_FOCUS;
				DialogManager::HandleMessage = TRUE;
				break;

			case EN_KILLFOCUS:						// Edit control looses focus (i.e. another field's been clicked on, tabbed to, etc)
				DialogManager::DIM = DIM_KILL_FOCUS;
				DialogManager::HandleMessage = TRUE;
				break;
		}
	}
#endif
}

/********************************************************************************************

>	void DialogManager::Dlg_OnScroll(HWND hwnd, HWND hwndCtl, UINT32 code, INT32 pos)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/9/93
	Purpose:	OnHScroll and OnVScroll message handler. Translates a windows scroll message
				into a DIM.	This routine is used for Scroll bars and Trackbars
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void DialogManager::Dlg_OnScroll( wxWindow *pWnd, wxWindow *pWndCtl, UINT32 code, INT32 pos )
{
	PORTNOTETRACE("dialog","DialogManager::Dlg_OnScroll - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	// Determine if the control is a scroll bar or track bar
	String_256 ClassNameStr;
	GetClassName(hwndCtl, (TCHAR*)ClassNameStr, 255);
	BOOL ScrollControl = (ClassNameStr == String_16(TEXT("ScrollBar")));
	BOOL SpinControl = (ClassNameStr == String_32(TEXT("msctls_updown32")));

	INT32 Page; // Number of units to move for a PAGE event

	if (!SpinControl)
	{
		// Obtain Page increment information
		ScrollPageInc* PgInc = (ScrollPageInc*)(ScrollPageIncList.GetHead());
		BOOL FoundPageIncDetails = FALSE;
		while (PgInc != NULL)
		{
			if (PgInc->ScrollBarWnd == hwndCtl) // Found details
			{
				FoundPageIncDetails = TRUE;
				Page = PgInc->PageInc;
			}
			PgInc = (ScrollPageInc*)ScrollPageIncList.GetNext((ListItem*)PgInc);
		}
		ENSURE(FoundPageIncDetails, "Scroll bar has not has its range of values set");
	}

	// Obtain scroll bars range	and current thumb position
    INT32 ScrollMin;
    INT32 ScrollMax;

	WORD CurrentThumbPos;

	if (ScrollControl)
	{
    	GetScrollRange(hwndCtl, SB_CTL, &ScrollMin, &ScrollMax);
		CurrentThumbPos = (WORD)GetScrollPos(hwndCtl, SB_CTL);
	}
	else if (SpinControl)
	{
		DWORD ScrollRange = SendMessage (hwndCtl, UDM_GETRANGE, 0, 0);
		ScrollMin = HIWORD (ScrollRange);
		ScrollMax = LOWORD (ScrollRange);
		CurrentThumbPos = (INT32) SendMessage(hwndCtl, UDM_GETPOS, 0, 0);

		if (SpinControlMessageUp)
		{
			code = SB_LINEUP;
		}
		else if (SpinControlMessageDown)
		{
			code = SB_LINEDOWN;
		}
		else
		{
			return;
		}
//		else
//		{
//			ERROR3 ("A serious error has occured!");
//		}
	}
	else // Trackbar
	{
		ScrollMin = (INT32) SendMessage(hwndCtl, TBM_GETRANGEMIN, 0, 0);
		ScrollMax = (INT32) SendMessage(hwndCtl, TBM_GETRANGEMAX, 0, 0);
		CurrentThumbPos = (WORD) SendMessage(hwndCtl, TBM_GETPOS, 0, 0);
	}

    INT32 ScrollBarInc = 0;
	INT32 NewPos;

	switch (code)
	{
		case SB_TOP:   	 // Same as SB_LEFT	TB_TOP
			ScrollBarInc = -(INT32)(CurrentThumbPos - ScrollMin);
		    break;
		case SB_BOTTOM: // Same as SB_RIGHT, TB_BOTTOM
			ScrollBarInc = ScrollMax - CurrentThumbPos;
			break;
		case SB_LINEUP:  // Same as SB_LINELEFT, TB_LINEUP
			ScrollBarInc = -1;
			break;
		case SB_LINEDOWN: // Same as SB_LINERIGHT, TB_LINEDOWN
			ScrollBarInc = 1;
			break;
		case SB_PAGEUP:   // Same as SB_PAGELEFT, TB_PAGEUP
			ScrollBarInc = -Page;
			break;
		case SB_PAGEDOWN:  // Same as SB_PAGERIGHT, TB_PAGERIGHT
			ScrollBarInc = Page;
			break;
		case SB_THUMBTRACK:	// Same as TB_THUMBTRACK
			ScrollBarInc = pos - CurrentThumbPos;
			break;
		case SB_THUMBPOSITION:

			break;
	}

	NewPos = CurrentThumbPos + ScrollBarInc;
	NewPos = max(ScrollMin, min(NewPos, ScrollMax));

	if (ScrollBarInc != 0)
	{
		if (ScrollControl)
		{
			SetScrollPos(hwndCtl, SB_CTL, NewPos, TRUE);
		}
		else if (!SpinControl)
		{
			SendMessage(hwndCtl, TBM_SETPOS,1,(INT32)NewPos );
		}

		// converting all scrollbar messages to DIM_SELECTION_CHANGED seems extremely
		// stupid to me, and in the case of the spin controls this creates a new problem:

		// in dialogs that respond to DIM_SELECTION_CHANGED for the correct purpose, spin
		// controls also enter into this logic - which causes/is extremely undesired behaviour!

		// SOLUTION - I'm adding some new DIM's which will allow me to differentiate ....

		if (ScrollControl)
		{
			DIM = DIM_SELECTION_CHANGED;
		}
		else if (SpinControl)
		{
			if (SpinControlMessageUp)
			{
				DIM = DIM_SPINCONTROLUP;
				SpinControlMessageUp = FALSE;
			}
			else	// MUST be SpinControlMessageDown - otherwise we would have retuned by now!
			{
				SpinControlMessageDown = FALSE;
				DIM = DIM_SPINCONTROLDOWN;
			}
		}
		else
		{
			DIM = DIM_SELECTION_CHANGED;
		}

		HandleMessage = TRUE;
	}
#endif
}


#endif




/********************************************************************************************

>	static CTreeItemID DialogManager::SetTreeGadgetItem(CWindowID wnd,
														 CGadgetID Gadget,
														 CTreeItemID hParent,
														 const StringBase& str,
														 CTreeItemID hInsAfter,
														 INT32 iImage,
														 CCObject* pObj = NULL)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/Feb/2006
	Inputs:		WindowID: Dialog box window identifier
				Gadget: Gadget identifier
				StrID: 	  Resource ID of string
	Outputs:	-
	Returns:	TRUE if the gadgets value could be set
				FALSE if it could not
	Purpose:	To insert an item into a tree control

	Errors:		If the function is called on an invalid control then an ENSURE failure will
				occur in a DEBUG build. In a retail build FALSE is returned.

	SeeAlso:	DialogOp::SetStringGadgetValue

********************************************************************************************/

CTreeItemID DialogManager::SetTreeGadgetItem(CWindowID wnd,
											  CGadgetID Gadget,
											  CTreeItemID hParent,
											  const StringBase& str,
											  CTreeItemID hInsAfter,
											  INT32 iImage,
											  CCObject* pObj)
{
	wxWindow * pGadget = GetGadget(wnd, Gadget);
	if (!pGadget) return CTreeItemID();

	wxString String = str;

	if ( pGadget->IsKindOf(CLASSINFO(wxTreeCtrl))
		)
	{
		CamelotTreeItemData* pData = NULL;
		if (pObj)
			pData = new CamelotTreeItemData(pObj);

		return ((wxTreeCtrl*)pGadget)->InsertItem(hParent, hInsAfter, String, iImage, -1, pData);
	}

	ERROR3("SetTreeGadgetItem called on non-tree gadget");

	return CTreeItemID();
}




/********************************************************************************************

>	static CCObject* DialogManager::GetTreeGadgetItemData(CWindowID wnd, CGadgetID Gadget, CTreeItemID hItem)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/Feb/2006
	Inputs:		WindowID: Dialog box window identifier
				Gadget: Gadget identifier
	Outputs:	-
	Returns:	TRUE if the gadgets value could be set
				FALSE if it could not
	Purpose:	To insert an item into a tree control

	Errors:		If the function is called on an invalid control then an ENSURE failure will
				occur in a DEBUG build. In a retail build FALSE is returned.

	SeeAlso:	DialogOp::SetStringGadgetValue

********************************************************************************************/

CCObject* DialogManager::GetTreeGadgetItemData(CWindowID wnd, CGadgetID Gadget, CTreeItemID hItem)
{
	wxWindow* pGadget = GetGadget(wnd, Gadget);
	if (!pGadget) return NULL;

	if ( pGadget->IsKindOf(CLASSINFO(wxTreeCtrl))
		)
	{
		wxTreeItemData* pData = ((wxTreeCtrl*)pGadget)->GetItemData(hItem);
		if (pData) // && pData->IsKindOf(CLASSINFO(CamelotTreeItemData)))
			return ((CamelotTreeItemData*)pData)->GetObject();

		return NULL;
	}

	ERROR3("SetTreeGadgetItem called on non-tree gadget");

	return NULL;
}




/********************************************************************************************

>	BOOL DialogManager::SelectTreeGadgetItem(CWindowID wnd, CGadgetID Gadget, CTreeItemID hItem, BOOL bNewState)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/Feb/2006
	Inputs:		WindowID	- Dialog box window identifier
				Gadget		- Gadget identifier
				hItem		- ID of item
				bNewState	- Selection state to set
	Outputs:	-
	Returns:	TRUE if the gadget item was selected
				FALSE if it could not
	Purpose:	To select an item in a tree control

********************************************************************************************/

BOOL DialogManager::SelectTreeGadgetItem(CWindowID wnd, CGadgetID Gadget, CTreeItemID hItem, BOOL bNewState)
{
	wxWindow* pGadget = GetGadget(wnd, Gadget);
	if (!pGadget) return FALSE;

	if ( pGadget->IsKindOf(CLASSINFO(wxTreeCtrl))
		)
	{
		((wxTreeCtrl*)pGadget)->SelectItem(hItem, FALSE != bNewState);
		return TRUE;
	}

	ERROR3("SetTreeGadgetItem called on non-tree gadget");
	return FALSE;
}




/********************************************************************************************

>	CTreeItemID	DialogManager::GetTreeGadgetRootItem(CWindowID wnd, CGadgetID Gadget)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/Feb/2006
	Inputs:		WindowID	- Dialog box window identifier
				Gadget		- Gadget identifier
	Outputs:	-
	Returns:	ID of item
	Purpose:	To get the ID of the root item in the tree control

********************************************************************************************/

CTreeItemID	DialogManager::GetTreeGadgetRootItem(CWindowID wnd, CGadgetID Gadget)
{
	wxWindow* pGadget = GetGadget(wnd, Gadget);
	if (!pGadget) return CTreeItemID();

	if ( pGadget->IsKindOf(CLASSINFO(wxTreeCtrl))
		)
	{
		return ((wxTreeCtrl*)pGadget)->GetRootItem();

		return CTreeItemID();
	}

	ERROR3("SetTreeGadgetItem called on non-tree gadget");
	return CTreeItemID();
}




/********************************************************************************************

>	CTreeItemID	DialogManager::GetTreeGadgetFirstSelectedItem(CWindowID wnd, CGadgetID Gadget)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/Feb/2006
	Inputs:		WindowID	- Dialog box window identifier
				Gadget		- Gadget identifier
				hItem		- ID of item
	Outputs:	-
	Returns:	ID of item
	Purpose:	To get the ID of the first selected item in the tree control

********************************************************************************************/

CTreeItemID	DialogManager::GetTreeGadgetFirstSelectedItem(CWindowID wnd, CGadgetID Gadget)
{
	wxWindow* pGadget = GetGadget(wnd, Gadget);
	if (!pGadget) return CTreeItemID();

	if ( pGadget->IsKindOf(CLASSINFO(wxTreeCtrl))
		)
	{
		return ((wxTreeCtrl*)pGadget)->GetSelection();
	}

	ERROR3("SetTreeGadgetItem called on non-tree gadget");
	return CTreeItemID();
}




/********************************************************************************************

>	BOOL DialogManager::TreeGadgetExpandItem(CWindowID wnd, CGadgetID Gadget, CTreeItemID hItem)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/Feb/2006
	Inputs:		WindowID	- Dialog box window identifier
				Gadget		- Gadget identifier
				hItem		- ID of item
				bNewState	- Selection state to set
	Outputs:	-
	Returns:	TRUE if the gadget item was expanded
				FALSE if it could not
	Purpose:	To expand an item in the tree control

********************************************************************************************/

BOOL DialogManager::TreeGadgetExpandItem(CWindowID wnd, CGadgetID Gadget, CTreeItemID hItem)
{
	wxWindow* pGadget = GetGadget(wnd, Gadget);
	if (!pGadget) return FALSE;

	if ( pGadget->IsKindOf(CLASSINFO(wxTreeCtrl))
		)
	{
		((wxTreeCtrl*)pGadget)->Expand(hItem);
		return TRUE;
	}

	ERROR3("SetTreeGadgetItem called on non-tree gadget");
	return FALSE;
}




/********************************************************************************************

>	CTreeItemID	DialogManager::GetTreeGadgetNextVisItem(CWindowID wnd, CGadgetID Gadget, CTreeItemID hItem)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/Feb/2006
	Inputs:		WindowID	- Dialog box window identifier
				Gadget		- Gadget identifier
				hItem		- ID of item
	Outputs:	-
	Returns:	ID of item
	Purpose:	To get the ID of the next visible item in the tree control after a specified item

********************************************************************************************/

CTreeItemID	DialogManager::GetTreeGadgetNextVisItem(CWindowID wnd, CGadgetID Gadget, CTreeItemID hItem)
{
	wxWindow* pGadget = GetGadget(wnd, Gadget);
	if (!pGadget) return CTreeItemID();

	if ( pGadget->IsKindOf(CLASSINFO(wxTreeCtrl))
		)
	{
		return ((wxTreeCtrl*)pGadget)->GetNextVisible(hItem);
	}

	ERROR3("SetTreeGadgetItem called on non-tree gadget");
	return CTreeItemID();
}




/********************************************************************************************

>	CTreeItemID	DialogManager::GetTreeGadgetFirstChildItem(CWindowID wnd, CGadgetID Gadget, CTreeItemID hItem)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/Feb/2006
	Inputs:		WindowID	- Dialog box window identifier
				Gadget		- Gadget identifier
				hItem		- ID of item
	Outputs:	-
	Returns:	ID of item
	Purpose:	To get the ID of the first child item of the specified in the tree control

********************************************************************************************/

CTreeItemID	DialogManager::GetTreeGadgetFirstChildItem(CWindowID wnd, CGadgetID Gadget, CTreeItemID hItem)
{
	wxWindow* pGadget = GetGadget(wnd, Gadget);
	if (!pGadget) return CTreeItemID();

	if ( pGadget->IsKindOf(CLASSINFO(wxTreeCtrl))
		)
	{
		wxTreeItemIdValue sessioncookie;
		return ((wxTreeCtrl*)pGadget)->GetFirstChild(hItem, sessioncookie);
	}

	ERROR3("SetTreeGadgetItem called on non-tree gadget");
	return CTreeItemID();
}




/********************************************************************************************

>	size_t DialogManager::GetTreeGadgetChildrenCount(CWindowID wnd, CGadgetID Gadget, CTreeItemID hItem, BOOL bRecursive)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/Feb/2006
	Inputs:		WindowID	- Dialog box window identifier
				Gadget		- Gadget identifier
				hItem		- ID of item
				bRecursive	- TRUE if should count children recursively
	Outputs:	-
	Returns:	number of children
	Purpose:	To get the number of children of an item in a tree control

********************************************************************************************/

UINT32 DialogManager::GetTreeGadgetChildrenCount(CWindowID wnd, CGadgetID Gadget, CTreeItemID hItem, BOOL bRecursive)
{
	wxWindow* pGadget = GetGadget(wnd, Gadget);
	if (!pGadget) return 0;

	if( pGadget->IsKindOf( CLASSINFO(wxTreeCtrl) ) )
	{
		return UINT32( ((wxTreeCtrl*)pGadget)->GetChildrenCount( hItem, FALSE != bRecursive ) );
	}

	ERROR3("SetTreeGadgetItem called on non-tree gadget");
	return 0;
}




/********************************************************************************************

>	UINT32 DialogManager::GetGadgetImageCount(CWindowID wnd, CGadgetID Gadget)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/Feb/2006
	Inputs:		WindowID	- Dialog box window identifier
				Gadget		- Gadget identifier
	Outputs:	-
	Returns:	Number of images in tree control
	Purpose:	To get the number of images set in the trree control

********************************************************************************************/

UINT32 DialogManager::GetGadgetImageCount(CWindowID wnd, CGadgetID Gadget)
{
	ERROR3("Unimplemented!");
	return 0;
}




/********************************************************************************************

>	ListItem* CGadgetImageList::FindFirstBitmap(ResourceID* presID) const

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/Feb/2006
	Inputs:		-
	Outputs:	presID	- ResourceID of first tiem in list
	Returns:	Pointer to context list item for iteration or NULL
	Purpose:	To get the first resourceID stored in this image list object

********************************************************************************************/

ListItem* CGadgetImageList::FindFirstBitmap(ResourceID* presID) const
{
	ResIDListItem* pItem = (ResIDListItem*)m_BitmapIDList.GetHead();
	if (presID && pItem)
		*presID = pItem->m_ResID;

	return (ListItem*)pItem;
}




/********************************************************************************************

>	ListItem* CGadgetImageList::FindNextBitmap(ListItem* pContextItem, ResourceID* presID) const

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/Feb/2006
	Inputs:		pContextItem - pointer to current context
	Outputs:	ResourceID - resource ID from list item
	Returns:	Pointer to context list item for iteration or NULL
	Purpose:	To get the next resourceID stored in this image list object after the specified item

********************************************************************************************/

ListItem* CGadgetImageList::FindNextBitmap(ListItem* pContextItem, ResourceID* presID) const
{
	ResIDListItem* pItem = (ResIDListItem*)m_BitmapIDList.GetNext(pContextItem);		// (Ahem! Maybe there's a better way to do this...)
	if (presID && pItem)
		*presID = pItem->m_ResID;

	return (ListItem*)pItem;
}


// OurPropSheet methods


BEGIN_EVENT_TABLE( OurPropSheet, wxPropertySheetDialog )
	EVT_NOTEBOOK_PAGE_CHANGED( wxID_ANY, OurPropSheet::OnSetActive )
END_EVENT_TABLE();

/********************************************************************************************

>	OurPropSheet::OurPropSheet(String_256* pName)

	Author:		Simon
	Created:	16/11/94
	Inputs:		pName: The name to be displayed in the dialog's title bar				
	Purpose:	Constructs a property sheet

********************************************************************************************/


OurPropSheet::OurPropSheet( wxWindow* pParentWnd, String_256* pName, UINT32 SelPage )
	: wxPropertySheetDialog( pParentWnd, wxID_ANY, (TCHAR*)*pName )	// The main frame is the owner of the sheet
{
}

/********************************************************************************************

>	OurPropSheet::~OurPropSheet()

	Author:		Simon
	Created:	28/11/94
	Purpose:	OurPropertySheet destructor, destroys the property sheet and all pages 
				that it contains

********************************************************************************************/

OurPropSheet::~OurPropSheet()
{
}


/********************************************************************************************

>	void OurPropSheet::OnClose()

	Author:		Neville
	Created:	23/12/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	We override the OnClose method for our property sheet because the base class
				OnClose method will kill the dialog box without telling us, if we are Modal.
				Therefore, the dialog has no chance to clean itself up and will instead be
				set a DIM_CANCEL when the application is closed!
	Errors:		-
	SeeAlso:	-
	
	Note:		This was true when using the VC2 XARAPROP.h/cpp class. Using the VC4 version
				means that in the modeless case when the user clicks on the close icon on a 
				tabbed dialog box, this function gets called. In the modal case, this function
				never seems to get called. We are then left with the op etc. still live.
				CPropertySheet::DoModal calls DestroyWindow() which should be virtual.
				This also sends a WM_DESTROY message. So, we will take advantage of this and
				trap the call, clean our op up and then call the baseclass version.

********************************************************************************************/

PORTNOTE("dialog","Removed Windows callback - OurPropSheet::OnClose")
#ifndef EXCLUDE_FROM_XARALX
void OurPropSheet::OnClose()
{
	// Inform the Dialog that the close icon has been clicked so that it can be be shut
	// down properly
	
	// Check if the Cancel button is present and disabled i.e. greyed. If it is then ignore
	// the message. Do this becuase we might have greyed the cancel button to say that this
	// is a bad move at this point in time and we do not want the close button coming along
	// and doing the cancel action when it shoudn't.
   	HWND hGadget = ::GetDlgItem(GetSafeHwnd(), IDCANCEL);	// Obtain gadgets window
	ERROR3IF(hGadget == NULL, "Could not find Cancel button control on tabbed dialog"); 
	BOOL ok = TRUE;
	// Only check if control is present
	if (hGadget)
		ok = ::IsWindowEnabled(hGadget);					// Get current button state

	// Use NULL as the page id so that the main dialog box gets the message.
	if (ok)
		BROADCAST_TO_CLASS(DialogMsg(GetSafeHwnd(), DIM_CANCEL, NULL, 0, NULL), DialogOp);
	else
		Beep();		// Warn the user that this is a bad time to do this	
	
	// Could call the main property sheet close method to get it to do its stuff but
	// hopefully by now the DIM_CANCEL should have done everything that we were
	// interested in.
	//CPropertySheet::OnClose();
}
#endif

/********************************************************************************************

>	virtual BOOL OurPropSheet::DestroyWindow()

	Author:		Neville Humphrys
	Created:	24/7/97
	Purpose:	We override the DestroyWindow method for our property sheet because the base class
				DestroyWindow method will kill the dialog box without telling us, if we are Modal.
				Therefore, the dialog has no chance to clean itself up and will instead be
				set a DIM_CANCEL when the application is closed!
	SeeAlso:	OurPropSheet::OnClose(); CPropertySheet::OnClose();

********************************************************************************************/

PORTNOTE("dialog","Removed Windows callback - OurPropSheet::DestroyWindow")
#ifndef EXCLUDE_FROM_XARALX
BOOL OurPropSheet::DestroyWindow()
{
	// Tell the system that this is a modal close which we wont have been told about
	// If we do this then we get a no active page error 3. Use the OnSysCommand trap instead.
//	if (IsModal())
//		BROADCAST_TO_CLASS(DialogMsg(GetSafeHwnd(), DIM_CANCEL, NULL, 0, NULL), DialogOp);

	return CPropertySheet::DestroyWindow();
}
#endif

/********************************************************************************************

>	void OurPropSheet::OnSysCommand(UINT nID, LPARAM lparam)

	Author:		Neville Humphrys
	Created:	24/7/97
	Purpose:	We override the OnSysCommand method for our property sheet because the base class
				OnClose method will kill the dialog box without telling us, if we are Modal.
				Therefore, the dialog has no chance to clean itself up and will instead be
				set a DIM_CANCEL when the application is closed!
	SeeAlso:	OurPropSheet::DestroyWindow(); OurPropSheet::OnClose(); CPropertySheet::OnClose();

********************************************************************************************/

PORTNOTE("dialog","Removed Windows callback - OurPropSheet::OnSysCommand")
#ifndef EXCLUDE_FROM_XARALX
void OurPropSheet::OnSysCommand(UINT nID, LPARAM lparam)
{
	// check for clicks on the close icon
	switch (nID & 0xFFF0)
	{
		case SC_CLOSE:
		{
			// The baseclass does this
			/* if (m_bModeless)
			{
				SendMessage(WM_CLOSE);
				return;
			} */
			// So we will do this
			if (!m_bModeless)
			{
				BROADCAST_TO_CLASS(DialogMsg(GetSafeHwnd(), DIM_CANCEL, NULL, 0, NULL), DialogOp);
			}
			break;
		}
	}
	// The baseclass calls this, we have no need as we will call the baseclass
	// Default();

	// Always call the baseclass
	CPropertySheet::OnSysCommand(nID, lparam); 
}
#endif

void OurPropSheet::OnSetActive( wxNotebookEvent& event )
{
	wxWindow*	pPage = GetBookCtrl()->GetPage( event.GetSelection() );
	if( m_setCreateSent.end() == m_setCreateSent.find( pPage ) )
	{
		BROADCAST_TO_CLASS( DialogMsg( this, DIM_CREATE, 0, 0, pPage->GetId() ), DialogOp );
		m_setCreateSent.insert( pPage );
	}

	BROADCAST_TO_CLASS( DialogMsg( this, DIM_SET_ACTIVE, 0, 0, pPage->GetId() ), DialogOp );
}

