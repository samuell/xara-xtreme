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

// The code to position and display the infobar.


#include "camtypes.h"
#include "mainfrm.h"
//#include "tim.h"
//#include "errors.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "ensure.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "app.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "document.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "barsdlgs.h" 
#include "bubbleid.h"

#include <afxpriv.h>

#include "infobar.h"

DECLARE_SOURCE("$Revision$");

CC_IMPLEMENT_DYNCREATE(InfoBarOp, DialogOp)

const CDlgMode InfoBarOp::Mode = MODELESS; // Mode of the dialog

BOOL InfoBarOp::Visible = TRUE;

UINT32 InfoBarOp::IDD = _R(IDD_INFOBAR);

InfoBarOp::InfoBarOp(CDlgResID SubDialogID) :
	DialogOp(InfoBarOp::IDD, InfoBarOp::Mode, SubDialogID)
{

	// Not dragging the info bar intially
	DraggingBar = FALSE;
}


MsgResult InfoBarOp::Message( Msg* Message)
{
/*
	if (Document::GetCurrent() == NULL)
		// No document - can't do anything
		return;

	// Process static Infobar control messages 
	if ((Gadget == _R(IDC_INVOKE_LYR_DLG)) && (DlgMsg == DIM_LFT_BN_CLICKED))
	{
		// Obtain a pointer to the op descriptor for the Layer dialog operation 
		OpDescriptor* OpDesc = OpDescriptor::FindOpDescriptor(CC_RUNTIME_CLASS(LayerDlg)); 
		// Invoke the operation. 
		OpDesc->Invoke();		  
	}   

	// Process active layer selection
	if ((Gadget == _R(IDC_LAYER_COMBO))	&& (DlgMsg == DIM_SELECTION_CHANGED))
	{
		// Obtain the index of the selected item
		WORD Index;  
		GetValueIndex(_R(IDC_LAYER_COMBO), &Index); 
		ChangeActiveLayer(Index); 
	}

	if (Gadget >= _R(ID_FIRST_TOOL_CTRL))
	{
		// Gadgets with IDs of 1000 or greater belong to the tool, not the Info bar, so
		// pass the message on to the tool.
		Tool *pTool = Tool::GetCurrent();
		ENSURE(pTool != NULL, "InfoBarOp::HandleMessage - no Tool is selected!");
		if (pTool != NULL)
			pTool->HandleInfoBarMessage(DlgMsg, Gadget);
	}
	else
	{
		#if 0
		// Handle the dragging of the info bar
		switch (DlgMsg)
		{
			case DIM_RGT_BN_DOWN:
			case DIM_LFT_BN_DOWN:
				GetCursorPos(&DragPos);
				SetCapture(WindowID);
				DraggingBar = TRUE;
				break;

			case DIM_MOUSE_DRAG:
				if (!DraggingBar)
				{
					GetCursorPos(&DragPos);
					SetCapture(WindowID);
					DraggingBar = TRUE;
					return;
				}

				POINT CurrPoint;
				GetCursorPos(&CurrPoint);
				ScrollWindow(WindowID, CurrPoint.x - DragPos.x, 0, NULL, NULL);
				DragPos = CurrPoint;
				break;

			case DIM_RGT_BN_UP:
			case DIM_LFT_BN_UP:
				ReleaseCapture();
				DraggingBar = FALSE;
				break;
		}
		#endif
	}
*/
	return OK; 
}

// -----------------------------------------------------------------------------------------

OpState	InfoBarOp::GetState(String_256*, OpDescriptor*)
{
	OpState OpSt;
	return(OpSt);
}

// -----------------------------------------------------------------------------------------

BOOL InfoBarOp::Init()
{
	// Create the OpDescriptor for this operation class.
	if (!(RegisterOpDescriptor(
	 						   0,
	 						   _R(IDT_VIEWINFOBAR),
							   CC_RUNTIME_CLASS(InfoBarOp),
	 						   OPTOKEN_INFOBAR,
	 						   InfoBarOp::GetState,
	 						   0,	/* help ID */
	 						   _R(IDBBL_INFOBAROP),
	 						   0	/* bitmap ID */)))
	{
		return FALSE; 
	}
	// Declare info bar preference.
	return Camelot.DeclareSection("Windows", 5) &&
		   Camelot.DeclarePref("Windows", "InfoBarIsVisible", &Visible);
}

// -----------------------------------------------------------------------------------------

BOOL InfoBarOp::Create()
{
	return DialogOp::Create();
}

// -----------------------------------------------------------------------------------------

void InfoBarOp::Do(OpDescriptor*)
{
	// Just to make sure info bar is not opened until it is positioned correctly (by
	// InfoBarOp::OnSizeParent()).
	Close();
}

/********************************************************************************************

>	void InfoBarOp::OnSizeParent(WPARAM, LPARAM lParam)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/11/93
	Inputs:		WPARAM - ignored.
				lParam - the special MFC structure that is sent when a parent tells its
				child windows to resize themselves.
	Outputs:	lParam - the structure is updated to reflect how much client area was
				taken by the Info Bar.
	Purpose:	Resize the info bar when the parent window moves/resizes, or when one of
				the children becomes visible/invisible.

				The WM_SIZEPARENT message is sent, which is a special AFX message (from the
				WM_USER address space I think).  The child windows then adjust the structure
				passed to them to steal part of the workspace for themselves, and whatever is
				left over is used for the client area.

				This is all undocumented and was deduced by looking at the MFC files
				WINCORE.CPP, WINFRM.CPP, BARCORE.CPP, and BARDLG.CPP.

********************************************************************************************/

void InfoBarOp::OnSizeParent(WPARAM, LPARAM lParam)
{
	AFX_SIZEPARENTPARAMS FAR* lpLayout = (AFX_SIZEPARENTPARAMS FAR*)lParam;

	if (Visible)
	{
		// Make the window zero size to avoid redraws
		//MoveWindow(WindowID, 0, 0, 0, 0, FALSE);

		// Info Bar is visible - reposition it wrt parent window.
		CRect rect;
		rect.CopyRect(&lpLayout->rect);

		CSize sizeAvail = rect.Size();  // maximum size available
		CSize size;     // maximum requested size
		size.cx = min(32767, sizeAvail.cx);
		size.cy = min(28, sizeAvail.cy);


		// top align (stretch width)
		size.cx = sizeAvail.cx;
		lpLayout->rect.top += size.cy;

		rect.right = rect.left + size.cx;
		rect.bottom = rect.top + size.cy;

		#if _MFC_VER < 0x300
		_AfxRepositionWindow(lpLayout, WindowID, &rect);

		// If the window hasn't actually been opened yet, then open it.
		// This deferral is necessary otherwise the window is opened before being
		// correctly positioned, and a portion of the document view is invalidated
		// and redrawn, which we definitely don't want.
		#if 1
		if (!IsWindowVisible(WindowID))
		{
			lpLayout->hDWP =
				::DeferWindowPos(lpLayout->hDWP, WindowID, NULL, 0, 0, 0, 0,
								 SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOMOVE|SWP_NOSIZE|
								 SWP_SHOWWINDOW);
		}
		#endif
		#else
		lpLayout->hDWP = ::DeferWindowPos(lpLayout->hDWP, WindowID, NULL,
										  rect.left, rect.top,
										  rect.right - rect.left, rect.bottom - rect.top,
										  SWP_NOACTIVATE|SWP_NOZORDER|SWP_SHOWWINDOW);
		#endif
	}
}


/********************************************************************************************

>	void InfoBarOp::Toggle()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/11/93
	Purpose:	Toggle the Info Bar on or off (i.e. make it invisible or visible).

********************************************************************************************/

void InfoBarOp::Toggle()
{
	// Work out whether to open or close the info bar, and do so.
	// (Also update preference variable)
	if (IsOpen())
	{
		Close();
		Visible = FALSE;
	}
	else
	{
		Open();
		Visible = TRUE;
	}

	// Find the main frame, and force the child windows to be repositioned/redrawn.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	pFrame->RecalcLayout();
}

/********************************************************************************************

>	void InfoBarOp::NewCurrentSpread()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/1/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	This function gets called by the LayerManager when the Current Spread is 
				changing. 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void InfoBarOp::NewCurrentSpread()
{
	ENSURE(FALSE,"This function should not be called");
/*
	GadgetRedraw(_R(IDC_LAYER_COMBO),FALSE);	// Turn off drawing until we have finished making changes 
											// to the gadget. 
	// Delete any existing list items 
	INT32 Count; 
	GetValueCount(_R(IDC_LAYER_COMBO),&Count); 
	while (Count != 0)
	{
		DeleteValue(_R(IDC_LAYER_COMBO), FALSE, Count-1); 
		Count--; 
	}

	WORD SelItemIndex = 0; // The list index of the selected item (i.e. the active layer)
	WORD i = 0; 

	BOOL FoundActiveLayer = FALSE; 

	// Traverse the Layer Manager's LyrDetList and add all layer identifiers to the list gadget
	List* LyrList = &((Document::GetCurrent()->GetLayerMgr()).LyrDetList); 
	ListItem* CurLayerDet = LyrList->GetHead();  
	ENSURE(CurLayerDet != NULL, "The LyrDetList is empty"); 
	while (CurLayerDet != NULL)
	{
		// Is the current layer the active layer ? 
		if ( ((LyrDetails*)CurLayerDet)->Status.Flags.Active ) 
		{
			// Ensure that there is only one active layer 
			ENSURE (!FoundActiveLayer, "More than one active layer found"); 
			FoundActiveLayer = TRUE; 
			SelItemIndex = i; 
		}
		// Add string to end of list box 
		SetStringGadgetValue(_R(IDC_LAYER_COMBO),&(((LyrDetails*)CurLayerDet)->Status.StringLayerID)); 

		CurLayerDet = LyrList->GetNext(CurLayerDet); 	
		i++; 
	}

	ENSURE(FoundActiveLayer,"No active layer was found"); 

	GadgetRedraw(_R(IDC_LAYER_COMBO),TRUE);	// Turn drawing back on 

	SetSelectedValueIndex(_R(IDC_LAYER_COMBO), SelItemIndex); // Select the active layer 
*/
} 

/********************************************************************************************

>	void InfoBarOp::DisableLayerList()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/1/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Disables the layer control
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void InfoBarOp::DisableLayerControl()
{
	 EnableGadget(_R(IDC_LAYER_COMBO), FALSE ); 
  	 EnableGadget(_R(IDC_INVOKE_LYR_DLG), FALSE); 
} 

/********************************************************************************************

>	void InfoBarOp::EnableLayerControl()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/1/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Enables the layer control
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void InfoBarOp::EnableLayerControl()
{
	 EnableGadget(_R(IDC_LAYER_COMBO), TRUE ); 
 	 EnableGadget(_R(IDC_INVOKE_LYR_DLG), TRUE ); 
} 

/********************************************************************************************

>	void InfoBarOp::ChangeActiveLayer(WORD NewLayerIndex)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/1/94
	Inputs:		NewLayerIndex: z-order Index of the new active layer. 
	Outputs:	-
	Returns:	-
	Purpose:	Changes the active layer 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void InfoBarOp::ChangeActiveLayer(WORD NewLayerIndex)
{
	ENSURE(FALSE,"This function should not be called");
/*
	INT32 CurrentIndex = 0; 
	// Traverse the Layer Manager's LyrDetList 
	List* LyrList = &((Document::GetCurrent()->GetLayerMgr()).LyrDetList); 
	ListItem* CurLayerDet = LyrList->GetHead();  
	ENSURE(CurLayerDet != NULL, "The LyrDetList is empty"); 
	while (CurLayerDet != NULL)
	{
		if (CurrentIndex == NewLayerIndex)
		{
			// We have found the layer to make active 
			((LyrDetails*)CurLayerDet)->Status.Flags.Active = TRUE; 
		}
		else if ( ((LyrDetails*)CurLayerDet)->Status.Flags.Active )
		{
			// We have found the old active layer 
			((LyrDetails*)CurLayerDet)->Status.Flags.Active = FALSE; 
		}
		CurLayerDet = LyrList->GetNext(CurLayerDet); // Get next layer detail record 
		CurrentIndex ++; 	
	}
	
	// Commit the changes 
	(Document::GetCurrent()->GetLayerMgr()).CommitLayerChanges(); 
*/
}


CC_IMPLEMENT_DYNCREATE(InfoBarViewOp, Operation)

/********************************************************************************************

>	BOOL InfoBarViewOp::Init()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/10/93
	Returns:	TRUE if worked, FALSE if failed (out of memory)
	Purpose:	Declares op descriptor for toggling info bar..
	Errors:		Returns FALSE on failure.
	Scope:		Static

********************************************************************************************/

BOOL InfoBarViewOp::Init()
{
	return (RegisterOpDescriptor(
								 0,
								 _R(IDT_VIEWINFOBAR),
								 CC_RUNTIME_CLASS(InfoBarViewOp),
								 OPTOKEN_VIEWINFOBAR,
								 GetState,
								 0,		/* help ID */
								 _R(IDBBL_VIEWINFOBAROP),
								 0		/* bitmap ID */));
}

/********************************************************************************************

>	void InfoBarViewOp::Do(OpDescriptor*)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/10/93
	Purpose:	Toggles the info bar.

********************************************************************************************/

void InfoBarViewOp::Do(OpDescriptor*)
{
	// Tell the main window to toggle the info bar.
	CMainFrame::pInfoBar->Toggle();

	// This operation is now finished
	End();
}

/********************************************************************************************

>	InfoBarViewOp::InfoBarViewOp()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/10/93
	Purpose:	Constructor for InfoBarViewOp operation. It is not undoable.

********************************************************************************************/

InfoBarViewOp::InfoBarViewOp()
{
}



/********************************************************************************************

>	OpState InfoBarViewOp::GetState(String_256*, OpDescriptor*)

	Author: 	Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/10/93
	Purpose:	This item is always available, so long as a document is visible.
				It is ticked if the info bar is visible.

********************************************************************************************/

OpState InfoBarViewOp::GetState(String_256*, OpDescriptor*)
{
	// Tick the item if the info bar is visible
	OpState OpSt(CMainFrame::pInfoBar->IsOpen());

	return OpSt;
}

