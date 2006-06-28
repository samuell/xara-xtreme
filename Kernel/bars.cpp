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
// The kernel implementation of bars

/*
*/


#include "camtypes.h"
#include "camelot.h"
//#include "bars.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "ensure.h"
#include "oilfiles.h"
//#include "ccfile.h"
//#include "markn.h"
#include "camframe.h"
//#include "dockbar.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "mario.h"      // read as "menu.h", gets the bindings for menu strings
//#include "msg.h"
//#include "simon.h"
//#include "fixst256.h"
//#include "tool.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "prefs.h"
#include "camelot.h"
#include "gallery.h"
//#include "sgallery.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "resource.h"
//#include "barsdlgs.h"
//#include "chris.h"
//#include "errors.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "keymsg.h"
#include "keypress.h"
#include "basebar.h"
#include "product.h"
#include "selector.h"
#include "bubbleid.h"
#include "vkextra.h"
#include "ctrllist.h"
//#include "camsdkfn.h"		// for equivalent winoil 'mappings' of standard OS calls
						// (e.g.  MoveWindow, GetClientRect, etc.)

//#include "will2.h"

//#include "registry.h"	// SaveRegistryEntries
#include "cversion.h"	// CAMELOT_VERSION_MAJOR CAMELOT_VERSION_MINOR

#include "biasdlg.h"	// for CBiasGainDialog ()

CC_IMPLEMENT_DYNCREATE( DialogBarOp , DialogOp )
//CC_IMPLEMENT_DYNCREATE( HiddenBarOp , DialogBarOp )
CC_IMPLEMENT_DYNCREATE( InformationBarOp, DialogBarOp)
CC_IMPLEMENT_DYNCREATE( SystemBarOp , DialogBarOp )

CC_IMPLEMENT_DYNAMIC( BarMsg, Msg )
CC_IMPLEMENT_DYNAMIC( BarItem, ListItem )
CC_IMPLEMENT_DYNAMIC( BarControlBase, BarItem )
CC_IMPLEMENT_DYNAMIC( BarControl, BarControlBase )
CC_IMPLEMENT_DYNAMIC( BarSeparator, BarItem )
CC_IMPLEMENT_DYNAMIC( BarNewPage, BarItem )

CC_IMPLEMENT_DYNAMIC( BarLineFeed, BarItem )
CC_IMPLEMENT_DYNAMIC( BarToolButton, BarControlBase)
CC_IMPLEMENT_DYNAMIC( DDeckerTop, BarItem )
CC_IMPLEMENT_DYNAMIC( DDeckerBottom, BarItem )
CC_IMPLEMENT_DYNAMIC( DDeckerEnd, BarItem )

#define new CAM_DEBUG_NEW

DockBarType			InformationBarOp::Dock   = DOCKBAR_TOP;
UINT32 				InformationBarOp::Slot   = DEFAULT_TOOL_INFOBAR_SLOT;
UINT32 				InformationBarOp::Offset = 0;									  
wxPoint 			InformationBarOp::FloatPos;
InformationBarOp*	InformationBarOp::CurrentInfoBarOp = NULL;
BOOL				InformationBarOp::Visible = TRUE;
BOOL				InformationBarOp::Hidden = FALSE;
BOOL				DialogBarOp::SystemStateChanged = TRUE; 
PathName			DialogBarOp::ValidPath;
DialogBarOp *		DialogBarOp::AllBarsOp = NULL;

static String_256 DefaultBarsConfigPath;
static BOOL ShowSystemBars = FALSE;
static String_64 BarsVersion;

static struct
{
	TCHAR*  Token;
} TokenTable[] = 
{
	{_T("Bar")},
	{_T("EndBar")},
	{_T("Control")},
	{_T("H")},
	{_T("V")},
	{_T("B")},
	{_T("Separator")},
	{_T("LineFeed")},
	{_T("Left")},
	{_T("Right")},
	{_T("Top")},
	{_T("Bottom")},
	{_T("Float")},
	{_T("DockingBar")},
	{_T("Big")},
	{_T("Small")},
	{_T("Gallery")},
	{_T("EndGallery")},
	{_T("DDeckerTop")},
	{_T("DDeckerBottom")},
	{_T("DDeckerEnd")},
	{_T("Visible")},
	{_T("Invisible")},
	{_T("StatusBar")},
	{_T("ColourBar")},
	{_T("ScrollBars")},
	{_T("FullScreen")},
	{_T("On")},
	{_T("Off")},
	{_T("InfoBar")},
	{_T("NewPage")}
};


/********************************************************************************************

>   static TokenIndex FindToken(const TCHAR* Token)

	Author:     Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:    6/4/94
	Inputs:     Token= token that we want to read up to.
	Outputs:    -
	Returns:    Token index represented by Token, or TOKEN_NONE if no match found
	Purpose:    Looks up the token index for the given string in the token table
	Errors:     -
	SeeAlso:    -

********************************************************************************************/

static TokenIndex FindToken(const TCHAR* Token)
{
	for (INT32 i=0;i<NUM_TOKENS;i++)
	{
		if (camStrcmp(TokenTable[i].Token,Token) == 0)
			return ((TokenIndex)i);
	}

	return (TOKEN_NONE);
}


/********************************************************************************************

>   void DialogBarOp::SetSystemStateChanged(BOOL State = TRUE) 

	Author:     Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:    28/3/94
	Inputs:     New SystemChangedState.
	Outputs:    -
	Returns:    -
	Purpose:    Constructor
	Errors:     -
	SeeAlso:    -

********************************************************************************************/

void DialogBarOp::SetSystemStateChanged(BOOL State) 
{
	SystemStateChanged = State;
	if (State)
		ControlList::Get()->Changed();
} 

/********************************************************************************************

>BOOL DialogBArOp::ShouldUpdateBarState()

	Author:     Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:    28/3/94
	Inputs:     Class = the message handler class list to add this object to.
	Outputs:    -
	Returns:    SystemStateChanged
	Purpose:    -
	Errors:     -
	SeeAlso:    -

********************************************************************************************/

BOOL DialogBarOp::ShouldUpdateBarState()
{
 return SystemStateChanged == TRUE;
 
}

/********************************************************************************************

>   DialogBarOp::DialogBarOp(CCRuntimeClass* Class = CC_RUNTIME_CLASS(DialogBarOp))

	Author:     Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:    28/3/94
	Inputs:     Class = the message handler class list to add this object to.
	Outputs:    -
	Returns:    -
	Purpose:    Constructor
	Errors:     -
	SeeAlso:    -

********************************************************************************************/


DialogBarOp::DialogBarOp(CCRuntimeClass* Class): DialogOp(_R(IDD_BLANKBAR), MODELESS,0,SAFECLASS(Class))
{
	String_32 str = String_32(_R(IDS_K_BARS_NONAME));
	DialogBarOpInit(str);
}


/********************************************************************************************

>   DialogBarOp::DialogBarOp(UINT32 StringID, CCRuntimeClass* Class = CC_RUNTIME_CLASS(DialogBarOp))

	Author:     Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:    28/3/94
	Inputs:     StringID 	= ID of name of the dialog bar op.
				Class 		= the message handler class list to add this object to.
	Outputs:    -
	Returns:    -
	Purpose:    Constructor
	Errors:     -
	SeeAlso:    -

********************************************************************************************/

DialogBarOp::DialogBarOp(UINT32 StringID, CCRuntimeClass* Class): DialogOp(_R(IDD_BLANKBAR), MODELESS,0,SAFECLASS(Class))
{
	String_32 str = String_32(StringID);
	DialogBarOpInit(str);
}


/********************************************************************************************

>   DialogBarOp::DialogBarOp(String_32& NewName,CCRuntimeClass* Class = CC_RUNTIME_CLASS(DialogBarOp))

	Author:     Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:    28/3/94
	Inputs:     NewName = Name of this bar
				Class = the message handler class list to add this object to.
	Outputs:    -
	Returns:    -
	Purpose:    Constructor
	Errors:     -
	SeeAlso:    -

********************************************************************************************/

DialogBarOp::DialogBarOp(String_32& NewName,CCRuntimeClass* Class): DialogOp(_R(IDD_BLANKBAR), MODELESS,0,SAFECLASS(Class))
{
	DialogBarOpInit(NewName);
}


/********************************************************************************************

>   void DialogBarOp::DialogBarOpInit(String_32& NewName)

	Author:     Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:    26/4/94
	Inputs:     NewName = Name of this bar
	Outputs:    -
	Returns:    -
	Purpose:    Common routine called by all Constructors
	Errors:     -
	SeeAlso:    -

********************************************************************************************/

void DialogBarOp::DialogBarOpInit(String_32& NewName)
{
	Name = NewName;
	Dock = DOCKBAR_TOP;
	Slot = 0;
	Offset = 0;
	FloatPos = wxPoint(100,100);
	BarRect = wxRect(0, 0, 0, 0);
	BarOrientation = Horizontal; 
	InitiallyVisible = TRUE;

	// Report new bar to any dialog ops...
	BROADCAST_TO_CLASS( BarMsg(BAR_CREATE,this), DialogOp );
}


/********************************************************************************************

>   DialogBarOp::~DialogBarOp()

	Author:     Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:    28/3/94
	Inputs:     -
	Outputs:    -
	Returns:    -
	Purpose:    Destructor
	Errors:     -
	SeeAlso:    -

********************************************************************************************/

DialogBarOp::~DialogBarOp()
{
//	if (IsUserName("MarkN")) TRACE( _T("~DialogBarOp\n"));
	DeleteAllBarItems();
}


/********************************************************************************************

>   LISTPOS DialogBarOp::AddBarItem(BarItem* pBarItem)

	Author:     Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:    28/3/94
	Inputs:     pBarItem = ptr to a bar item to add to the dialog bar
	Outputs:    -
	Returns:    The position of the new bar item in the bar
	Purpose:    Adding a bar item to a bar.
	Errors:     -
	SeeAlso:    -

********************************************************************************************/

LISTPOS DialogBarOp::AddBarItem(BarItem* pBarItem)
{
	BarItemList.AddTail(pBarItem);
	return (BarItemList.FindPosition(pBarItem));
}

/********************************************************************************************

>   BarItem* DialogBarOp::RemoveBarItem(LISTPOS here)

	Author:     Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:    28/3/94
	Inputs:     here = position of the bar item to be removed from the list
	Outputs:    -
	Returns:    pBarItem = ptr to the bar item that has been removed.
	Purpose:    Removes the bar item from the list of bar items n the bar.
				This does NOT delete the item, or any memory associated with it.  It is just delinked.
	Errors:     Debug build     - An ENSURE failure if the bar item is not found.
				Retail build    - NULL is returned if the bar item is not found.
	SeeAlso:    -

********************************************************************************************/

BarItem* DialogBarOp::RemoveBarItem(LISTPOS here)
{
	BarItem* pBarItem = (BarItem*) BarItemList.FindItem(here);
	ENSURE(pBarItem != NULL,"Couldn't remove bar item. Item not found.");
	
	BarItemList.RemoveItem(pBarItem);

	return (pBarItem);
}

/********************************************************************************************

>   BarItem* DialogBarOp::RemoveBarItem(BarItem * thisItem)

	Author:     Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:    28/3/94
	Inputs:     here = position of the bar item to be removed from the list
	Outputs:    -
	Returns:    pBarItem = ptr to the bar item that has been removed.
	Purpose:    Removes the bar item from the list of bar items n the bar.
				This does NOT delete the item, or any memory associated with it.  It is just delinked.
	Errors:     Debug build     - An ENSURE failure if the bar item is not found.
				Retail build    - NULL is returned if the bar item is not found.
	SeeAlso:    -

********************************************************************************************/

BarItem* DialogBarOp::RemoveBarItem(BarItem * thisItem)
{
	return (BarItem *)BarItemList.RemoveItem(thisItem);
}

/********************************************************************************************

>   void DialogBarOp::InsertItemAfter(LISTPOS here,BarItem * thisItem)

	Author:     Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:    28/3/94
	Inputs:     here    = position of the bar item to be moved.
				newpos  = the new pos of the bar item
	Outputs:    -
	Returns:    -
	Purpose:    .
	Errors:     -
	SeeAlso:    -

********************************************************************************************/

void DialogBarOp::InsertItemAfter(LISTPOS here,BarItem * thisItem)
{

	UINT32 NoItems = GetNumBarItems();
	if(here == -1)
		BarItemList.AddHead(thisItem);
	else if ((unsigned)here >= NoItems)
		BarItemList.AddTail(thisItem);
	else
		BarItemList.InsertAfter(here,thisItem);
}

/********************************************************************************************

>   BOOL DialogBarOp::HideDisableGadgetAndResuffleBar (CWindowID WinID)

	Author:     Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:    30/9/99
	Inputs:     WinID - the handle to the control that this operation is to be performed on
	Outputs:    -
	Returns:    TRUE - if operation successful (i.e.  could be applied to this infobar)
				FALSE - if operation was NOT successful
	Purpose:    This function is the saviour of camelots GUI infobars - several of which
				have become too crowded, and as such; their controls no longer fit into
				the 'static' docked infobar.  The function hides the requested control
				and automatically resuffles those controls that occur after it.
				For example:	Combo1, Combo2, Combo3;  call on Combo1 gives:
								Combo2, Combo3
				If you are calling this operation - you MUST ensure that the infobar
				(which is WinID's parent) has been handled within winoil\basebar2.cpp.
				Refer (basebar2.cpp) line 1311 for the code that MUST be checked/edited
				(or search on DialogBarOp::HideDisableGadgetAndResuffleBar).
				If you want to see an example of how to use this function correctly,
				consult tool\filltool.cpp - since this is the first place the function
				was used within camelot.  TIP:  I would recommend that you hide the
				controls in a left to right order (i.e.  leftmost first).
	Errors:     -
	SeeAlso:    BOOL DialogBarOp::ShowEnableGadgetAndResuffleBar (CWindowID WinID)
				BOOL DialogBarOp::ResetHiddenGadgetStates ()

********************************************************************************************/

BOOL DialogBarOp::HideDisableGadgetAndResuffleBar (CWindowID WinID)
{	
	/*KernelBarPos* 	pKernelBarPos;
	DockBarType 	Dock;
	
	if (GetMainFrame()->GetBarPosInfo(GetParent ((HWND) WinID),&Dock,&pKernelBarPos))
	{
		SetDockBarType(Dock);
		if (Dock == DOCKBAR_FLOAT)
			SetFloatingCPoint(wxPoint(pKernelBarPos->x,pKernelBarPos->y));
		else
		{
			SetSlot(pKernelBarPos->Slot);
			SetOffset(pKernelBarPos->Position);
		}
	}

	// Save this info bar's data in the static vars
	// These vars are used by
	DialogBarOp::Dock 			= GetDockBarType();
	DialogBarOp::Slot 			= GetSlot();
	DialogBarOp::Offset 		= GetOffset();
	DialogBarOp::FloatPos 		= GetFloatingCPoint();
	
	switch (GetDockBarType ())
	{
		case DOCKBAR_LEFT:		// This should not be possible for infobars
		return (FALSE);			// NOT a chance!
		case DOCKBAR_RIGHT:		// This should not be possible for infobars
		return (FALSE);			// NOT a chance!
		case DOCKBAR_TOP:		
			// handle internally - so that caller does NOT have to worry ....
			EnableGadget (CamalotSDKGetDlgCtrlID (WinID), FALSE);
		break;					// will process this one (cause its of static size)
		case DOCKBAR_BOTTOM:
			// handle internally - so that caller does NOT have to worry ....
			EnableGadget (CamalotSDKGetDlgCtrlID (WinID), FALSE);
		break;					// will process this one (cause its of static size)
		case DOCKBAR_FLOAT:
			// handle internally - so that caller does NOT have to worry ....
			EnableGadget (CamalotSDKGetDlgCtrlID (WinID), FALSE);
		return (FALSE);			// we will NOT process this because of the problems
								// it causes
		case DOCKBAR_INVALID:	// god knows !!!!
		return (FALSE);
	}
	
	RECT windowRect;

	CamalotSDKGetWindowRect (WinID, &windowRect);

	POINT topLeft;
	topLeft.x = windowRect.left;
	topLeft.y = windowRect.top;

	CamalotSDKScreenToClient (CamalotSDKGetParent (WinID), &topLeft);

	INT32 xLength = windowRect.right - windowRect.left;

	BarItem* hideControl = GetBarItem ((HWND) WinID);
	BarControl* hideBControl = NULL;

	if (hideControl->IsKindOf(CC_RUNTIME_CLASS(BarControl)))
	{
		hideBControl = (BarControl*) hideControl;
	}
	
	// firstly, lets hide the gadget that this operation has been requested for ....

	if (hideBControl->IsVisible () == TRUE)
	{
		hideBControl->SetVisibleState (FALSE);

		CamalotSDKShowWindow (WinID, SW_HIDE);

		//BarItem* hideControl = GetBarItem ((HWND) WinID);
		BarItem* prevControl = GetPtrBarPrev (hideControl);
		BarItem* nextControl = GetPtrBarNext (hideControl);

		if (prevControl->IsKindOf(CC_RUNTIME_CLASS(BarSeparator)))
		{	
			if (!(GetMainFrame()->GetDockBar(GetDockBarType ())->IsBigControls()))
			{
				xLength += SMALL_SEPARATOR;
			}
			else
			{
				xLength += LARGE_SEPARATOR;
			}
		}

		while (nextControl != NULL)
		{
			CWindowID barCtrlID  = NULL;
			BarControl* barCtrl = NULL;

			if (nextControl->IsKindOf(CC_RUNTIME_CLASS(BarControl)))
			{
				barCtrl = (BarControl*) nextControl;
				barCtrlID = barCtrl->GetWinID ();

				CamalotSDKGetWindowRect (barCtrlID, &windowRect);
				topLeft.x = windowRect.left;
				topLeft.y = windowRect.top;
			
				CamalotSDKScreenToClient (CamalotSDKGetParent (WinID), &topLeft);
				topLeft.x -= xLength;
				//windowRect.right -= xLength;

				BarItem* controlSep = GetPtrBarPrev (nextControl);
				
				INT32 sepLength = 0;

				if (controlSep->IsKindOf(CC_RUNTIME_CLASS(BarSeparator)))
				{	
					if (!(GetMainFrame()->GetDockBar(GetDockBarType ())->IsBigControls()))
					{
						sepLength += SMALL_SEPARATOR;
					}
					else
					{
						sepLength += LARGE_SEPARATOR;
					}
				}

				//InvalidateRect (GetParent ((HWND) WinID), &windowRect, TRUE);

				CamalotSDKMoveWindow ((HWND) barCtrlID, topLeft.x, topLeft.y,
									  windowRect.right - windowRect.left,
									  windowRect.bottom - windowRect.top, TRUE);*/

				/*RECT sepRect;
				sepRect.right = windowRect.left;
				sepRect.left = sepRect.right - sepLength;
				sepRect.top = windowRect.top;
				sepRect.bottom = windowRect.bottom;

				InvalidateRect (GetParent ((HWND) WinID), &sepRect, TRUE);*/

			// need to do this - cause camelots bubble help system sometimes gets
			// broken otherwise ....

			//InvalidateRect (GetParent ((HWND) hideControl), NULL, TRUE);
			//BringWindowToTop ((HWND) barCtrlID);

/*			}

			nextControl = (BarItem*) BarItemList.GetNext(nextControl);
		}
		return (TRUE);
	}
	else
	{
		return (FALSE);
	}*/
	
	return (TRUE);//FALSE);
}

/********************************************************************************************

>   BOOL DialogBarOp::ShowEnableGadgetAndResuffleBar (CWindowID WinID)

	Author:     Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:    30/9/99
	Inputs:     WinID - the handle to the control that this operation is to be performed on
	Outputs:    -
	Returns:    TRUE - if operation successful (i.e.  could be applied to this infobar)
				FALSE - if operation was NOT successful
	Purpose:    Does the opposite of
				BOOL DialogBarOp::HideDisableGadgetAndResuffleBar (CWindowID WinID)
	Errors:     -
	SeeAlso:    BOOL DialogBarOp::ShowDisableGadgetAndResuffleBar (CWindowID WinID)
				BOOL DialogBarOp::ResetHiddenGadgetStates ()

********************************************************************************************/

BOOL DialogBarOp::ShowEnableGadgetAndResuffleBar (CWindowID WinID)
{
	// firstly - we need to check to see if the control is of the bar is of the
	// correct docking type - BEFORE we continue ....

/*	KernelBarPos* 	pKernelBarPos;
	DockBarType 	Dock;

	if (GetMainFrame()->GetBarPosInfo(GetParent ((HWND) WinID),&Dock,&pKernelBarPos))
	{
		SetDockBarType(Dock);
		if (Dock == DOCKBAR_FLOAT)
			SetFloatingCPoint(wxPoint(pKernelBarPos->x,pKernelBarPos->y));
		else
		{
			SetSlot(pKernelBarPos->Slot);
			SetOffset(pKernelBarPos->Position);
		}
	}

	// Save this info bar's data in the static vars
	// These vars are used by
	DialogBarOp::Dock 			= GetDockBarType();
	DialogBarOp::Slot 			= GetSlot();
	DialogBarOp::Offset 		= GetOffset();
	DialogBarOp::FloatPos 		= GetFloatingCPoint();

	RECT windowRect;

	CamalotSDKGetWindowRect ((HWND) WinID, &windowRect);

	switch (GetDockBarType ())
	{
		case DOCKBAR_LEFT:		// This should not be possible for infobars
		return (FALSE);			// NOT a chance!
		case DOCKBAR_RIGHT:		// This should not be possible for infobars
		return (FALSE);			// NOT a chance!
		case DOCKBAR_TOP:
			// handle internally - so that caller does NOT have to worry ....
			EnableGadget (CamalotSDKGetDlgCtrlID (WinID), TRUE);
		break;					// will process this one (cause its of static size)
		case DOCKBAR_BOTTOM:
			// handle internally - so that caller does NOT have to worry ....
			EnableGadget (CamalotSDKGetDlgCtrlID (WinID), TRUE);
		break;					// will process this one (cause its of static size)
		case DOCKBAR_FLOAT:
			// handle internally - so that caller does NOT have to worry ....
			EnableGadget (CamalotSDKGetDlgCtrlID (WinID), TRUE);
		return (FALSE);			// we will NOT process this because of the problems
								// it causes
		case DOCKBAR_INVALID:	// god knows !!!!
		return (FALSE);
	}

	POINT topLeft;
	topLeft.x = windowRect.left;
	topLeft.y = windowRect.top;

	CamalotSDKScreenToClient (CamalotSDKGetParent (WinID), &topLeft);

	INT32 xLength = windowRect.right - windowRect.left;

	BarItem* hideControl = GetBarItem ((HWND) WinID);
	BarControl* hideBControl = NULL;

	if (hideControl->IsKindOf(CC_RUNTIME_CLASS(BarControl)))
	{
		hideBControl = (BarControl*) hideControl;
	}
	
	// firstly, lets hide the gadget that this operation has been requested for ....

	if (hideBControl->IsVisible () == FALSE)
	{
		hideBControl->BarControl::SetVisibleState (TRUE);
		
		CamalotSDKShowWindow (WinID, SW_SHOW);

		//BarItem* hideControl = GetBarItem ((HWND) WinID);
		BarItem* prevControl = GetPtrBarPrev (hideControl);
		BarItem* nextControl = GetPtrBarNext (hideControl);

		if (prevControl->IsKindOf(CC_RUNTIME_CLASS(BarSeparator)))
		{	
			if (!(GetMainFrame()->GetDockBar(GetDockBarType ())->IsBigControls()))
			{
				xLength += SMALL_SEPARATOR;
			}
			else
			{
				xLength += LARGE_SEPARATOR;
			}
		}

		while (nextControl != NULL)
		{
			CWindowID barCtrlID  = NULL;
			BarControl* barCtrl = NULL;

			if (nextControl->IsKindOf(CC_RUNTIME_CLASS(BarControl)))
			{
				barCtrl = (BarControl*) nextControl;
				barCtrlID = barCtrl->GetWinID ();

			CamalotSDKGetWindowRect (barCtrlID, &windowRect);
			topLeft.x = windowRect.left;
			topLeft.y = windowRect.top;
			
			CamalotSDKScreenToClient (CamalotSDKGetParent (WinID), &topLeft);
			topLeft.x += xLength;
			//windowRect.right -= xLength;

			MoveWindow ((HWND) barCtrlID, topLeft.x, topLeft.y,
						windowRect.right - windowRect.left,
						windowRect.bottom - windowRect.top, TRUE);

			}

			nextControl = (BarItem*) BarItemList.GetNext(nextControl);
		}
		return (TRUE);
	}
	else
	{
		return (FALSE);
	}*/
	
	return (TRUE);//FALSE);
}

/********************************************************************************************

>   BOOL DialogBarOp::ResetHiddenGadgetStates ()

	Author:     Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:    15/10/99
	Inputs:     WinID - the handle to the control that this operation is to be performed on
	Outputs:    -
	Returns:    TRUE - if function traversed all controls in the dialogbar
				FALSE - if operation contains NO controls
	Purpose:    This function is required to fix the errors that originate on 'dynamic'
				infobars when we change to another tool - or drag the infobar into a floating
				one; and then make it dockable again.  The problems result from the fact that
				we need to reset our visibility flags (in these cases) in order for the
				HideDisableGadgetAndResuffleBar and ShowDisableGadgetAndResuffleBar to do
				their stuff correctly.  This function handles this ....

				This function should only be called from the following places:

				1)	Inside of BaseBar::Recreate (DockBarType DockBarTyp, BaseBar* BaseBar,
					DialogBarOp* Op,BOOL CanCopy ).  Although (in this file) this is the
					only place the function needs to be called, YOU MUST UPDATE THE LOGIC
					THAT IS RESPONSIBLE FOR CALLING THE FUNCTION.  This is located at about
					line 1248.

				2)	Inside of your InfoBars Tool::SelectChange(BOOL isSelected) - within
					the isSelected FALSE part of this code, you need to add the following
					line:

					(your infobar op)->ResetHiddenGadgetStates ();
	
	Errors:     -
	SeeAlso:    BOOL DialogBarOp::HideDisableGadgetAndResuffleBar (CWindowID WinID)
				BOOL DialogBarOp::ShowDisableGadgetAndResuffleBar (CWindowID WinID)

********************************************************************************************/

BOOL DialogBarOp::ResetHiddenGadgetStates ()
{
	/*BarItem* theItem = GetPtrBarHead ();

	if (theItem != NULL)
	{
		while (theItem != NULL)
		{
			if (theItem->IsKindOf(CC_RUNTIME_CLASS(BarControl)))
			{
				BarControl* theBCItem = (BarControl*) theItem;

				if (theBCItem->IsVisible () == FALSE)
				{
					theBCItem->SetVisibleState (TRUE);
					//theBCItem->SetRequireResetOfVisibleState (TRUE);
				}
			}
			theItem = (BarItem*) BarItemList.GetNext (theItem);
		}
		//SetRequireResetOfHiddenControls (TRUE);
		return (TRUE);
	}
	else
	{
		return (FALSE);
	}*/
	return (TRUE);//FALSE);
}

/********************************************************************************************

>   void DialogBarOp::MoveBarItem(LISTPOS here,LISTPOS newpos)

	Author:     Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:    28/3/94
	Inputs:     here    = position of the bar item to be moved.
				newpos  = the new pos of the bar item
	Outputs:    -
	Returns:    -
	Purpose:    Moves the bar item at 'here' to a new position in the bar. All items that were
				at positions from 'newpos' onwards before the call and shuffled along one position
				to accommodate the move.
	Errors:     -
	SeeAlso:    -

********************************************************************************************/

void DialogBarOp::MoveBarItem(LISTPOS here,LISTPOS newpos)
{
	if (here == newpos) return;

	BarItem* pBarItem = RemoveBarItem(here);

	if (pBarItem == NULL) return;

	if ((unsigned)newpos >= GetNumBarItems())
		BarItemList.AddTail(pBarItem);
	else
		BarItemList.InsertBefore(newpos,pBarItem);
}

/********************************************************************************************

>   void DialogBarOp::DeleteAllBarItems()

	Author:     Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:    7/4/94
	Inputs:     -
	Outputs:    -
	Returns:    -
	Purpose:    Removes and deletes all bar items attached to this bar.
				It's main function is for tidying up after errors during the
				reconstruction of the bars system when the tokenised ASCII
				file is read in.
	Errors:     -
	SeeAlso:    -

********************************************************************************************/

void DialogBarOp::DeleteAllBarItems()
{
//	if (IsUserName("MarkN")) TRACE( _T("DeleteAllBarItems()\n"));

	BarItem* pNextBarItem = (BarItem*) BarItemList.GetHead();
	BarItem* pThisBarItem;
	
	while (pNextBarItem != NULL)
	{
		pThisBarItem = pNextBarItem;
		pNextBarItem = (BarItem*) BarItemList.GetNext(pThisBarItem);

		BarItemList.RemoveItem(pThisBarItem);
		//if (IsUserName("MarkN")) TRACE( _T("deleting pThisBarItem (%p)\n"),pThisBarItem);
		delete pThisBarItem;
	}
}

/********************************************************************************************

>   BarItem * DialogBarOp::GetBarItem(CWindowID WinID)

	Author:     Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:    7/4/94
	Inputs:     -
	Outputs:    -
	Returns:    -
	Purpose:    Removes and deletes all bar items attached to this bar.
				It's main function is for tidying up after errors during the
				reconstruction of the bars system when the tokenised ASCII
				file is read in.
	Errors:     -
	SeeAlso:    -

********************************************************************************************/

BarItem * DialogBarOp::GetBarItem(CWindowID WinID)
{
//	if (IsUserName("MarkN")) TRACE( _T("DeleteAllBarItems()\n"));

	BarItem* pNextBarItem = (BarItem*) BarItemList.GetHead();
	BarItem* pThisBarItem;
	
	while (pNextBarItem != NULL)
	{
		pThisBarItem = pNextBarItem;
		pNextBarItem = (BarItem*) BarItemList.GetNext(pThisBarItem);
		if (pThisBarItem->IsKindOf(CC_RUNTIME_CLASS(BarControl)))
		{
			BarControl* BarCtrl = (BarControl*)pThisBarItem;
			if(WinID == BarCtrl->GetWinID())
			{
				return pThisBarItem;
 			}
		}
	}

	return (NULL);
	
//	ERROR2(NULL,"Can't find bar item");
}


/********************************************************************************************

>   BOOL DialogBarOp::DeleteBarItem(CWindowID WinID)

	Author:     Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:    7/4/94
	Inputs:     -
	Outputs:    -
	Returns:    -
	Purpose:    Removes and deletes all bar items attached to this bar.
				It's main function is for tidying up after errors during the
				reconstruction of the bars system when the tokenised ASCII
				file is read in.
	Errors:     -
	SeeAlso:    -

********************************************************************************************/

BOOL DialogBarOp::DeleteBarItem(CWindowID WinID)
{
//	if (IsUserName("MarkN")) TRACE( _T("DeleteAllBarItems()\n"));

	BarItem* pNextBarItem = (BarItem*) BarItemList.GetHead();
	BarItem* pThisBarItem;
	
	while (pNextBarItem != NULL)
	{
		pThisBarItem = pNextBarItem;
		pNextBarItem = (BarItem*) BarItemList.GetNext(pThisBarItem);
		if (pThisBarItem->IsKindOf(CC_RUNTIME_CLASS(BarControl)))
		{
			BarControl* BarCtrl = (BarControl*)pThisBarItem;
		 	if(WinID == BarCtrl->GetWinID())
			{
				BarItemList.RemoveItem(pThisBarItem);
				delete pThisBarItem;
				return TRUE;
			}
		}
	}
	
	ERROR2(FALSE,"Can't find bar item to delete");
}
/********************************************************************************************

>   BarItem* DialogBarOp::GetPtrBarItem(LISTPOS here)

	Author:     Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:    28/3/94
	Inputs:     here    = position of the bar item.
	Outputs:    -
	Returns:    ptr to the bar item at position 'here'
	Purpose:    Allows you to get your mits on a bar item.
	Errors:     Debug build     - An ENSURE failure if a bar item is not found.
				Retail build    - NULL is returned if a bar item is not found.
	SeeAlso:    -

********************************************************************************************/

BarItem* DialogBarOp::GetPtrBarItem(LISTPOS here)
{
	BarItem* pBarItem = (BarItem*) BarItemList.FindItem(here);
	ENSURE(pBarItem != NULL,"Couldn't get ptr bar item. Item not found.");
	
	return (pBarItem);
}

/********************************************************************************************

>   BarItem* DialogBarOp::GetPtrBarHead()

	Author:     Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:    20/5/94
	Inputs:     -
	Outputs:    -
	Returns:    ptr to the head of baritem list
	Purpose:    Allows you to get your mits on a bar item.
	Errors:     Debug build     - An ENSURE failure if a bar item is not found.
				Retail build    - NULL is returned if a bar item is not found.
	SeeAlso:    -

********************************************************************************************/

BarItem* DialogBarOp::GetPtrBarHead()
{
	BarItem* pBarItem = (BarItem*) BarItemList.GetHead();
	//ENSURE(pBarItem != NULL,"Couldn't get ptr bar item. Item not found.");

	return (pBarItem);
}

/********************************************************************************************

>   BarItem* DialogBarOp::IsListEmpty()

	Author:     Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:    20/5/94
  	Inputs:     -
	Outputs:    -
	Returns:    TRUE if List is empty FALSE otherwise
	Purpose:   	-
	Errors:    	-
	SeeAlso:    -

********************************************************************************************/

BOOL DialogBarOp::IsListEmpty()
{
	return (BarItemList.IsEmpty());
}

/********************************************************************************************

>   BarItem* DialogBarOp::GetPtrBarNext(BarItem * ThisItem )

	Author:     Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:    20/5/94
	Inputs:     -
	Outputs:    -
	Returns:    ptr to the next item in baritem list
	Purpose:    Allows you to get your mits on a bar item.
	Errors:     -
	SeeAlso:    -

********************************************************************************************/

BarItem* DialogBarOp::GetPtrBarNext(BarItem * ThisItem)
{
	BarItem* pBarItem = (BarItem*) BarItemList.GetNext(ThisItem);
	return (pBarItem);
}
/********************************************************************************************

>   BarItem* DialogBarOp::GetPtrBarPrev(BarItem * ThisItem )

	Author:     Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:    20/5/94
	Inputs:     -
	Outputs:    -
	Returns:    ptr to the previous item in baritem list
	Purpose:    Allows you to get your mits on a bar item.
	Errors:     -
	SeeAlso:    -

********************************************************************************************/

BarItem* DialogBarOp::GetPtrBarPrev(BarItem * ThisItem)
{
	BarItem* pBarItem = (BarItem*) BarItemList.GetPrev(ThisItem);
	return (pBarItem);
}


/********************************************************************************************

>   BarItem* DialogBarOp::InsertItemBefore(BarItem * ThisItem ,BarItem * newItem)

	Author:     Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:    20/5/94
	Inputs:     -
	Outputs:    -
	Returns:    -
	Purpose:    Allows you to insert a bar item.
	Errors:     -
	SeeAlso:    -

********************************************************************************************/

BarItem * DialogBarOp::InsertItemBefore(BarItem * here ,BarItem * newItem)
{
	ListItem * item;
	item = BarItemList.InsertBefore((ListItem*) here, (ListItem*) newItem); 
	return (BarItem *)item;
}

/********************************************************************************************

>   DWORD DialogBarOp::GetNumBarItems() const

	Author:     Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:    28/3/94
	Inputs:     -
	Outputs:    -
	Returns:    Num bar items in the bar
	Purpose:    You can find out just how many pesky bar items are in this bar with this call.
	Errors:     -
	SeeAlso:    -

********************************************************************************************/

DWORD DialogBarOp::GetNumBarItems() const
{
	return (BarItemList.GetCount());
}


/********************************************************************************************

>   void DialogBarOp::SetName(String_32& str)

	Author:     Mark_Neves (Xara Group Ltd) <camelotdev@xara.com> (& Phil)
	Created:    12/4/94
	Inputs:     Reference to string containing new name for bar
	Outputs:    -
	Returns:    -
	Purpose:    "I name this bar..."
	Errors:     -
	SeeAlso:    -

********************************************************************************************/

void DialogBarOp::SetName(String_32& str)
{
	Name = str;

	// Report changes to bar to any dialog ops...
	BROADCAST_TO_CLASS( BarMsg(BAR_CHANGESTATE,this), DialogOp );

}


/********************************************************************************************

   BOOL Gallery::Write(CCLexFile& file)

	Author:     Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:    12/10/94
	Inputs:     file = a file to Write the dialog bar to.
	Outputs:    -
	Returns:    TRUE if successful, FALSE otherwise
	Purpose:    Write the Gallery to the file in a tokenized form
	Errors:     -
	SeeAlso:    bars.ini

********************************************************************************************/

/*	The old Gallery class is defunct - now replaced by SuperGallery
BOOL Gallery::Write(CCLexFile& file)
{
	BOOL ok = TRUE;

	String_256 str;
	if (ok) ok = file.PutToken(TokenTable[TOKEN_GALLERY].Token);
	
	if (ok) ok = file.PutString((String_256)GetRuntimeClass()->m_lpszClassName);

	if (ok) ok = file.PutString(GetName());

	if (ok) ok = file.PutToken(TokenTable[TOKEN_DOCKBAR_FLOAT].Token);

	if (ok) ok = file.PutToken(FloatPos.x);
	if (ok) ok = file.PutToken(FloatPos.y);
	
	file.IncIndent();
	
	BarItem* pBarItem = (BarItem*) BarItemList.GetHead();
	while (pBarItem != NULL && ok)
	{
		if (ok) ok = file.PutNewLine();
	
		if (ok) ok = pBarItem->Write(file);
	
		pBarItem = (BarItem*)BarItemList.GetNext(pBarItem);
	}
	
	file.DecIndent();
	
	if (ok) ok = file.PutNewLine();

	if (ok) ok = file.PutToken(TokenTable[TOKEN_GALLERY_END].Token);

	if (ok) ok = file.PutNewLine();
	if (ok) ok = file.PutNewLine();
	return ok;
}
*/


#ifndef EXCLUDE_GALS
/********************************************************************************************

>   BOOL SuperGallery::Write(CCLexFile& file)

	Author:     Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:    12/10/94
	Inputs:     file = a file to Write the dialog bar to.
	Outputs:    -
	Returns:    TRUE if successful, FALSE otherwise
	Purpose:    Write the SuperGallery to the file in a tokenized form
	Errors:     -
	SeeAlso:    bars.ini

********************************************************************************************/

BOOL SuperGallery::Write(CCLexFile& file)
{
	BOOL ok = TRUE;
#ifndef EXCLUDE_FROM_XARALX
PORTNOTE("galleries", "Disable Lexfile writing")
	String_256 str;
	if (ok) ok = file.PutToken(TokenTable[TOKEN_GALLERY].Token);
	
	if (ok) ok = file.PutString((String_256)GetRuntimeClass()->GetClassName());

	if (ok) ok = file.PutString(GetName());

	if (ok) ok = file.PutToken(TokenTable[TOKEN_DOCKBAR_FLOAT].Token);

	if (ok) ok = file.PutToken(FloatPos.x);
	if (ok) ok = file.PutToken(FloatPos.y);
	
	file.IncIndent();
	
	BarItem* pBarItem = (BarItem*) BarItemList.GetHead();
	while (pBarItem != NULL && ok)
	{
		if (ok) ok = file.PutNewLine();
	
		if (ok) ok = pBarItem->Write(file);
	
		pBarItem = (BarItem*)BarItemList.GetNext(pBarItem);
	}
	
	file.DecIndent();
	
	if (ok) ok = file.PutNewLine();

	if (ok) ok = file.PutToken(TokenTable[TOKEN_GALLERY_END].Token);

	if (ok) ok = file.PutNewLine();
	if (ok) ok = file.PutNewLine();
#endif
	return ok;
}

#endif

/********************************************************************************************

>   BOOL DialogBarOp::Write(CCLexFile& file)

	Author:     Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:    28/3/94
	Inputs:     file = a file to Write the dialog bar to.
	Outputs:    -
	Returns:    TRUE if successful, FALSE otherwise
	Purpose:    Write the dialog bar to the file in a tokenized form
	Errors:     -
	SeeAlso:    -

********************************************************************************************/

BOOL DialogBarOp::Write(CCLexFile& file)
{
#pragma message( __LOCMSG__ "DialogBarOp::Write(CCLexFile& file) - do nothing" )
	TRACE( _T("Warning - DialogBarOp::Write(CCLexFile& file) called") );
/*	BOOL ok = TRUE;
	// we don't want to write out the control bank
	if(IsAllBarsOp())
		return ok;
	String_256 str;

	if (ok) ok = file.PutToken(TokenTable[TOKEN_BAR].Token);
	if (ok) ok = file.PutString(Name);
	KernelBarPos* 	pKernelBarPos = NULL;
	
	if(IsVisible())
		GetMainFrame()->GetBarPosInfo(WindowID,&Dock,&pKernelBarPos);
	
	switch (Dock)
	{
		case DOCKBAR_TOP    : str = TokenTable[TOKEN_DOCKBAR_TOP].Token;    break;
		case DOCKBAR_BOTTOM : str = TokenTable[TOKEN_DOCKBAR_BOTTOM].Token; break;
		case DOCKBAR_LEFT   : str = TokenTable[TOKEN_DOCKBAR_LEFT].Token;   break;
		case DOCKBAR_RIGHT  : str = TokenTable[TOKEN_DOCKBAR_RIGHT].Token;  break;
		case DOCKBAR_FLOAT  : str = TokenTable[TOKEN_DOCKBAR_FLOAT].Token;  break;
		default             : ok  = FALSE;                break;
	}

	if (ok) ok = file.PutToken(str);
	
	if(IsVisible())
	{
		SetDockBarType(Dock);
		if(pKernelBarPos)
		{
			if (Dock == DOCKBAR_FLOAT)
				SetFloatingCPoint(wxPoint(pKernelBarPos->x,pKernelBarPos->y));
			else
			{
				SetSlot(pKernelBarPos->Slot);
				SetOffset(pKernelBarPos->Position);
			}
		}
	}
	if (Dock == DOCKBAR_FLOAT)
	{
		if (ok) ok = file.PutToken(FloatPos.x);
		if (ok) ok = file.PutToken(FloatPos.y);
	}
	else
	{
		if (ok) ok = file.PutToken(Slot);
		if (ok) ok = file.PutToken(Offset);
	}

	if(IsVisible())
	{
		if (ok) ok = file.PutToken(TokenTable[TOKEN_VISIBLE].Token);
	}
	else
	{
		if (ok) ok = file.PutToken(TokenTable[TOKEN_INVISIBLE].Token);
	}


	file.IncIndent();

	BarItem* pBarItem = (BarItem*) BarItemList.GetHead();
	while (pBarItem != NULL && ok)
	{
		if (ok) ok = file.PutNewLine();
		if (ok) ok = pBarItem->Write(file);
		pBarItem = (BarItem*)BarItemList.GetNext(pBarItem);
	}

	file.DecIndent();
	if (ok) ok = file.PutNewLine();

	if (ok) ok = file.PutToken(TokenTable[TOKEN_BAR_END].Token);
	if (ok) ok = file.PutNewLine();
	if (ok) ok = file.PutNewLine();

	return ok; */

	return false;
}



/********************************************************************************************

>   BOOL DialogBarOp::Read(CCLexFile& file)

	Author:     Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:    7/4/94
	Inputs:     file = a file to read the dialog bar from.
	Outputs:    -
	Returns:    TRUE if successful, FALSE otherwise
	Purpose:    Read the dialog bar from the file in a tokenized form
	Errors:     -
	SeeAlso:    -

********************************************************************************************/

BOOL DialogBarOp::Read(CCLexFile& file)
{
	TokenIndex Token;
	const TCHAR* TokenBuf = file.GetTokenBuf();
	BOOL ok;

	// Read the bar's name
	ok = file.GetSimpleToken();

	if (ok)
	{
		// The token is either a string or an ID...
		LexTokenType TokenType = file.GetTokenType();

		switch (TokenType)
		{
			case TOKEN_NORMAL:
				{
					INT32 ID = 0;
					ok = (camSscanf(TokenBuf, _T("%li"), &ID) == 1);
					if(ID == 0)
					{
						Name.operator=((const TCHAR*) TokenBuf);
					}
					else
					{
						Name=ID;
					}
				}
				break;
	
			case TOKEN_STRING:
			default:
				Name.operator=((const TCHAR*) TokenBuf);
				break;
		}
	}

	// Read the bar's docking position
	if (ok) ok = file.GetSimpleToken();

	if (ok)
	{
		Token = FindToken(TokenBuf);
		switch (Token)
		{
			case TOKEN_DOCKBAR_TOP    : Dock = DOCKBAR_TOP;    break;
			case TOKEN_DOCKBAR_BOTTOM : Dock = DOCKBAR_BOTTOM; break;
			case TOKEN_DOCKBAR_LEFT   : Dock = DOCKBAR_LEFT;   break;
			case TOKEN_DOCKBAR_RIGHT  : Dock = DOCKBAR_RIGHT;  break;
			case TOKEN_DOCKBAR_FLOAT  : Dock = DOCKBAR_FLOAT;  break;

			default : 
				ok = FALSE;
				TRACE( _T("Unrecognised docking bar position : '%s'"),TokenBuf);
				break;
		}

		if (ok)
		{
			// If the bar is floating, then read in the floating X and Y coords
			// Otherwise, read in the bar's docked slot and offset.
			if (Dock == DOCKBAR_FLOAT)
			{
				ok = file.GetSimpleToken();
				if (ok)
				{	
					ok = (camSscanf(TokenBuf, _T("%li"),&FloatPos.x) == 1);
					if (!ok) TRACE( _T("Expected the bar's floating x pos, but got '%s'\n"),TokenBuf);
				}

				if (ok) ok = file.GetSimpleToken();
				if (ok)
				{
					ok = (camSscanf(TokenBuf, _T("%li"),&FloatPos.y) == 1);
					if (!ok) TRACE( _T("Expected the bar's floating y pos, but got '%s'\n"),TokenBuf);
				}
			}
			else
			{
				ok = file.GetSimpleToken();
				if (ok)
				{
					ok = (camSscanf(TokenBuf, _T("%li"),&Slot) == 1);
					if (!ok) TRACE( _T("Expected the bar's slot, but got '%s'\n"),TokenBuf);
				}

				if (ok) ok = file.GetSimpleToken();
				if (ok)
				{
					ok = (camSscanf(TokenBuf, _T("%li"),&Offset) == 1);
					if (!ok) TRACE( _T("Expected the bar's offset, but got '%s'\n"),TokenBuf);
				}
			}
		}


		if (ok)
		{
			// Default to being invisible (stop galleries opening by default)
			InitiallyVisible = FALSE;
			if ( (GetRuntimeClass() == CC_RUNTIME_CLASS(DialogBarOp)) ||
				 (GetRuntimeClass() == CC_RUNTIME_CLASS(SystemBarOp)) )
			{	
				ok = file.GetSimpleToken();
				if (ok)
				{
					Token = FindToken(TokenBuf);
					switch (Token)
					{
						case TOKEN_VISIBLE   : InitiallyVisible = TRUE;  break;
						case TOKEN_INVISIBLE : break;
						default : 
							ok = FALSE;
							TRACE( _T("Expected '%s' or '%s', but got '%s'\n"),TokenTable[TOKEN_VISIBLE].Token,TokenTable[TOKEN_INVISIBLE].Token,TokenBuf);
							break;
					}
				}
			}

			BOOL finished = FALSE;
			while (!finished && ok)
			{
				BarItem* pBarItem=NULL;

				ok = file.GetSimpleToken();
				if (ok)
				{
					Token = FindToken(TokenBuf);

					switch (Token)
					{
						case TOKEN_BAR_CONTROL   : pBarItem = new BarControl;    break;
						case TOKEN_BAR_SEPARATOR : pBarItem = new BarSeparator;  break;
						case TOKEN_BAR_NEWPAGE   : pBarItem = new BarNewPage;  break;
						case TOKEN_BAR_LINEFEED  : pBarItem = new BarLineFeed;   break;
						case TOKEN_DDECKER_TOP   : pBarItem = new DDeckerTop;   break;
						case TOKEN_DDECKER_BOTTOM: pBarItem = new DDeckerBottom;break;
						case TOKEN_DDECKER_END   : pBarItem = new DDeckerEnd;   break;
						case TOKEN_BAR_END       : // NB! RUNS ON!!!     
						case TOKEN_GALLERY_END	 : finished = TRUE;              break;
						default : 
							ok = FALSE;
							TRACE( _T("Unexpected token string : '%s'\n"),TokenBuf);
							break;
					}

					if (!finished && ok)
					{
						ok = (pBarItem != NULL);
						if (ok) ok = pBarItem->Read(file);
						if (ok) AddBarItem(pBarItem);

						if (!ok) 
						{
							if (pBarItem != NULL) delete pBarItem;
							DeleteAllBarItems();
							return FALSE;
						}
					}
				}
			}
		}
	}

	if (!ok)
	{
		// If something when wrong, tell the debug dudes and give as much info as possible
		TRACE( _T("\n\nError reading a bar : \n"));
		TRACE( _T(" Token buffer = %s\n"),TokenBuf);
		TRACE( _T(" Line buffer  = %s\n"),file.GetLineBuf());
	}


	return (ok);
}


/********************************************************************************************

>   BOOL DialogBarOp::Read(LoadRegistryEntries& Loader)

	Author:     Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:    20/2/97
	Inputs:     Loader = the class to use to load the data from the registry.
	Outputs:    -
	Returns:    TRUE if successful, FALSE otherwise
	Purpose:    Read the dialog bar or super gallery definition from the registry
	Errors:     -
	SeeAlso:    -

********************************************************************************************/

BOOL DialogBarOp::Read(LoadRegistryEntries& Loader)
{
#pragma message( __LOCMSG__ "BaseTextClass::PreOpProcessing - do nothing" )
	TRACE( _T("Warning - BaseTextClass::PreOpProcessing called") );
/*	BOOL ok = TRUE;

	HKEY hRegKey = Loader.GetRegKey();
	if (hRegKey == NULL)
	{
		//ERROR3("DialogBarOp::Read failed to get registry key");
		return FALSE;
	}

	// Read the name of the bar from the registry
	String_256 BarName;
	ok = GetRegString(hRegKey, TEXT("Name"), &BarName);
	Name = BarName;

	DockBarType ReadDock = (DockBarType)GetRegInt(hRegKey, TEXT("Dock"));
	// check that the value read in is ok
	switch (ReadDock)
	{
		case DOCKBAR_TOP:
		case DOCKBAR_BOTTOM:
		case DOCKBAR_LEFT:
		case DOCKBAR_RIGHT:
		case DOCKBAR_FLOAT:
			Dock = ReadDock;
			break;
		default : 
			ok = FALSE;
			TRACE( _T("Unrecognised bar position : '%d'"),ReadDock);
			break;
	}

	if (Dock == DOCKBAR_FLOAT)
	{
		FloatPos.x = GetRegDword(hRegKey, TEXT("x"));
		FloatPos.y = GetRegDword(hRegKey, TEXT("y"));
	}
	else
	{
		Slot = GetRegDword(hRegKey, TEXT("Slot"));
		Offset = GetRegDword(hRegKey, TEXT("Offset"));
	}

	INT32 Width = GetRegDword(hRegKey, TEXT("Width"));
	INT32 Height = GetRegDword(hRegKey, TEXT("Height"));
	if (Width > 0 && Height > 0)
	{
		BarRect.width  = Width;
		BarRect.height = Height;
	}

	// Visibility applies to all bars so get the visibility of the bar
	InitiallyVisible = GetRegBool(hRegKey, TEXT("Visibility"));

	// Now read in the controls for that bar
	String_256 data;
	BOOL finished = FALSE;
	BarItem* pBarItem = NULL;
	while (!finished && ok)
	{
		// blank our reading string
		data.Empty();

		// get the next line from the registry
		ok = Loader.LoadNextEntry(&data);
		if (ok)
		{
			// Get the first word up to the next white space character from the string
			String_256 FoundToken;
			INT32 index = data.FindNextChar(' ', 0);
			if (index == -1)
			{
				// There is no white space character so use whole word
				FoundToken = data;
			}
			else
			{
				// stick the word into our string
				data.Left(&FoundToken, index);
			}

			// cant do a switch, must do it the hard way
			if (FoundToken == String_256(TokenTable[TOKEN_BAR_CONTROL].Token))
				pBarItem = new BarControl;
			else if (FoundToken == String_256(TokenTable[TOKEN_BAR_SEPARATOR].Token))
				pBarItem = new BarSeparator;
			else if (FoundToken == String_256(TokenTable[TOKEN_BAR_NEWPAGE].Token))
				pBarItem = new BarNewPage;
			else if (FoundToken == String_256(TokenTable[TOKEN_BAR_LINEFEED].Token))
				pBarItem = new BarLineFeed;
			else if (FoundToken == String_256(TokenTable[TOKEN_DDECKER_TOP].Token))
				pBarItem = new DDeckerTop;
			else if (FoundToken == String_256(TokenTable[TOKEN_DDECKER_BOTTOM].Token))
				pBarItem = new DDeckerBottom;
			else if (FoundToken == String_256(TokenTable[TOKEN_DDECKER_END].Token))
				pBarItem = new DDeckerEnd;
			else if (FoundToken == String_256(TokenTable[TOKEN_BAR_END].Token))
				finished = TRUE;  
			else if (FoundToken == String_256(TokenTable[TOKEN_GALLERY_END].Token))
				finished = TRUE;
			else
			{
				ok = FALSE;
				TRACE( _T("Unexpected token string : '%s'\n"),(TCHAR*)FoundToken);
			}

			if (!finished && ok && pBarItem != NULL)
			{
				// Ask the bar item to parse the rest of its details from the string
				// First transfer the remainder of the string into a new one skipping
				// the space after the details
				String_256 BarItemDef;
				data.Mid(&BarItemDef, index + 1, data.Length() - (index + 1));
				// and now ask the bar item to parse it
				if (ok) ok = pBarItem->Read(&BarItemDef);
				if (ok) AddBarItem(pBarItem);

				if (!ok) 
				{
					if (pBarItem != NULL) delete pBarItem;
					DeleteAllBarItems();
					return FALSE;
				}
			}
		}
		else
			finished = TRUE;
	}

	return TRUE; */

	return true;
}

/********************************************************************************************

>   BOOL DialogBarOp::Write(SaveRegistryEntries& Saver)

	Author:     Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:    20/2/97
	Inputs:     Saver = the class to use to save the data into the registry.
	Outputs:    -
	Returns:    TRUE if successful, FALSE otherwise
	Purpose:    Write the dialog bar to the registry
	Errors:     -
	SeeAlso:    -

********************************************************************************************/

BOOL DialogBarOp::Write(SaveRegistryEntries& Saver)
{
#pragma message( __LOCMSG__ "DialogBarOp::Write(SaveRegistryEntries& Saver) - do nothing" )
	TRACE( _T("Warning - DialogBarOp::Write(SaveRegistryEntries& Saver) called") );
/*	//ERROR2IF(hSubKey == NULL,FALSE,"DialogBarOp::Write bad sub key");

	BOOL ok = TRUE;
	// we don't want to write out the control bank
	if (IsAllBarsOp())
		return ok;

	HKEY hRegKey = Saver.GetRegKey();
	if (hRegKey == NULL)
	{
		ERROR3("DialogBarOp::Write failed to get registry key");
		return FALSE;
	}

	// save out the name into the registry
	ok = ok && SetRegString(hRegKey, TEXT("Name"), Name);

	ok = ok && SetRegString(hRegKey, TEXT("Type"), TokenTable[TOKEN_BAR].Token);

	KernelBarPos* pKernelBarPos = NULL;
	if(IsVisible())
		GetMainFrame()->GetBarPosInfo(WindowID,&Dock,&pKernelBarPos);

	ok = ok && SetRegInt(hRegKey, TEXT("Dock"), Dock);

	if (IsVisible())
	{
		SetDockBarType(Dock);
		if (pKernelBarPos)
		{
			if (Dock == DOCKBAR_FLOAT)
				SetFloatingCPoint(wxPoint(pKernelBarPos->x,pKernelBarPos->y));
			else
			{
				SetSlot(pKernelBarPos->Slot);
				SetOffset(pKernelBarPos->Position);
			}
		}
	}

	if (Dock == DOCKBAR_FLOAT)
	{
		ok = ok && SetRegDword(hRegKey, TEXT("x"), FloatPos.x);
		ok = ok && SetRegDword(hRegKey, TEXT("y"), FloatPos.y);
	}
	else
	{
		ok = ok && SetRegDword(hRegKey, TEXT("Slot"), Slot);
		ok = ok && SetRegDword(hRegKey, TEXT("Offset"), Offset);
	}

	// save the visibility of the bar
	ok = ok && SetRegBool(hRegKey, TEXT("Visibility"), IsVisible());

	// save out the items on the bar
	// get the first control on this bar
	BarItem* pBarItem = (BarItem*) BarItemList.GetHead();
	String_256 data;
	while (pBarItem != NULL && ok)
	{
		data.Empty();
		// get the bar control to put its details into a string for us
		if (ok) ok = pBarItem->Write(&data);
		// now write that out to the registry
		Saver.SaveNextEntry(&data);

		// get the next control on this bar
		pBarItem = (BarItem*)BarItemList.GetNext(pBarItem);
	}

	// The saver will automatically shut down our use of the key
	// so just return the result of the saving to the user
	return ok; */

	return false;
}

#ifndef EXCLUDE_GALS
/********************************************************************************************

>   BOOL SuperGallery::Write(SaveRegistryEntries& Saver)

	Author:     Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:    20/2/97
	Inputs:     Saver = the class to use to save the data into the registry.
	Outputs:    -
	Returns:    TRUE if successful, FALSE otherwise
	Purpose:    Write the SuperGallery to the registry
	Errors:     -
	SeeAlso:    -

********************************************************************************************/

BOOL SuperGallery::Write(SaveRegistryEntries& Saver)
{
#pragma message( __LOCMSG__ "SuperGallery::Write(SaveRegistryEntries& Saver) - do nothing" )
	TRACE( _T("Warning - SuperGallery::Write(SaveRegistryEntries& Saver) called") );
/*	BOOL ok = TRUE;

	HKEY hRegKey = Saver.GetRegKey();
	if (hRegKey == NULL)
	{
		ERROR3("SuperGallery::Write failed to get registry key");
		return FALSE;
	}

	// save out the name into the registry
	ok = ok && SetRegString(hRegKey, TEXT("Name"), GetName());

	String_256 str = GetRuntimeClass()->m_lpszClassName;
	ok = ok && SetRegString(hRegKey, TEXT("ClassName"), str);

	ok = ok && SetRegString(hRegKey, TEXT("Type"), TokenTable[TOKEN_GALLERY].Token);

	KernelBarPos* pKernelBarPos = NULL;
	GetMainFrame()->GetBarPosInfo(WindowID,&Dock,&pKernelBarPos);
	SetDockBarType(Dock);
	if (pKernelBarPos)
	{
		if (Dock == DOCKBAR_FLOAT)
			SetFloatingCPoint(wxPoint(pKernelBarPos->x,pKernelBarPos->y));
		else
		{
			SetSlot(pKernelBarPos->Slot);
			SetOffset(pKernelBarPos->Position);
		}
	}

	ok = ok && SetRegInt(hRegKey, TEXT("Dock"), Dock);

	if (Dock == DOCKBAR_FLOAT)
	{
		ok = ok && SetRegDword(hRegKey, TEXT("x"), FloatPos.x);
		ok = ok && SetRegDword(hRegKey, TEXT("y"), FloatPos.y);
	}
	else
	{
		ok = ok && SetRegDword(hRegKey, TEXT("Slot"), Slot);
		ok = ok && SetRegDword(hRegKey, TEXT("Offset"), Offset);
	}

	wxRect BarRect = GetMainFrame()->GetDockBar(Dock)->GetBarRect(WindowID);

	if (BarRect.width > 0 && BarRect.height > 0)
	{
		ok = ok && SetRegDword(hRegKey, TEXT("Width"), BarRect.width);
		ok = ok && SetRegDword(hRegKey, TEXT("Height"), BarRect.height);
	}
	else if (GallerySize.x > 0 && GallerySize.y > 0)
	{
		ok = ok && SetRegDword(hRegKey, TEXT("Width"), GallerySize.x);
		ok = ok && SetRegDword(hRegKey, TEXT("Height"), GallerySize.y);
	}

	// save the visibility of the bar
	ok = ok && SetRegBool(hRegKey, TEXT("Visibility"), IsVisible());
	
	// save out the items on the bar
	// get the first control on this bar
	BarItem* pBarItem = (BarItem*) BarItemList.GetHead();
	String_256 data;
	while (pBarItem != NULL && ok)
	{
		data.Empty();
		// get the bar control to put its details into a string for us
		if (ok) ok = pBarItem->Write(&data);
		// now write that out to the registry
		Saver.SaveNextEntry(&data);
	
		// get the next control on this bar
		pBarItem = (BarItem*)BarItemList.GetNext(pBarItem);
	}
	
	// The saver will automatically shut down our use of the key
	// so just return the result of the saving to the user
	return ok; */

	return false;
}

#endif // EXCLUDE_GALS

/********************************************************************************************

>   BOOL DialogBarOp::MakeControlBank()

	Author:     Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:    24/5/95
	Inputs:     -
	Outputs:    -
	Returns:    TRUE if successful, FALSE otherwise
	Purpose:    Build a control bar that contains all the controls defined in the bars.ini
				resource
	Errors:     -
	SeeAlso:    -

********************************************************************************************/

BOOL DialogBarOp::MakeControlBank()
{
#pragma message( __LOCMSG__ "DialogBarOp::MakeControlBank - do nothing" )
	TRACE( _T("Warning - DialogBarOp::MakeControlBank called") );
/*	// create the bar
	if(DialogBarOp::AllBarsOp == NULL)
	{
		AllBarsOp = new DialogBarOp(String_32(_R(IDS_CONTROLBANKNAME)));
		AllBarsOp->SetDockBarType(DOCKBAR_FLOAT);
		AllBarsOp->InitiallyVisible = FALSE;
	}
	ERROR2IF(AllBarsOp==NULL,FALSE,"NULL AllBarsOp");
	
	BOOL ok = TRUE,open=FALSE,finished=FALSE,InBar= FALSE; 
	INT32 Count = 0 ;

	CCResTextFile file;	// Resource File
	DialogBarOpCreate BarCreate;

 	if (ok) ok = open = file.open(_R(IDM_DEFAULT_BARS), _R(IDT_CAM_BARS_RES));
	if(!open)
		return FALSE;
	
	file.InitLexer();					// Initialise lexing routines
   	file.SetDelimiters("\r\n");			// Set token delimiting characters
	file.SetCommentMarker(';');			// Set comment marker char
	file.SetWhitespace(" \t");			// Set whitespace chars
	file.SetStringDelimiters("\"\"");	// Set string delimiters

	TokenIndex Token;
	const TCHAR* TokenBuf = file.GetTokenBuf();
	BarItem* pBarItem;

	while (ok)
	{
		pBarItem = NULL;
		ok = file.GetSimpleToken();

		if (ok)
		{
			Token = FindToken(TokenBuf);
			switch (Token)
			{
				case TOKEN_BAR			 : 
					InBar = TRUE; break;
				case TOKEN_BAR_CONTROL   : 
					if(InBar)
						pBarItem = new BarControl; Count ++;  break;
				case TOKEN_BAR_SEPARATOR : 
					if(InBar)					
						pBarItem = new BarSeparator;  break;
				case TOKEN_BAR_END       :
					InBar = FALSE ; break;
			}


			if (pBarItem && InBar)
			{
				if( Count == 11 )
				{
					BarLineFeed* pBarItem = new BarLineFeed;
					AllBarsOp->AddBarItem(pBarItem);
	 				Count =0;
				}
			
				if (ok) ok = pBarItem->Read(file);
				if (ok) AllBarsOp->AddBarItem(pBarItem);
				if (!ok) 
				{
					if (pBarItem != NULL) delete pBarItem;
					AllBarsOp->DeleteAllBarItems();
					if (open) file.close();
					return FALSE;
				}
			}
		
		}// end if ok
	}// end while
	
	if(open) file.DeinitLexer();
	
	if (open) file.close();
	return TRUE; */

	return false;
}

/********************************************************************************************

>   static BOOL DialogBarOp::Init()

	Author:     Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:    18/2/94
	Returns:    TRUE if all went OK, FALSE otherwise
	Purpose:    Adds the operation to the list of all known operations

********************************************************************************************/

BOOL DialogBarOp::Init()
{
	BOOL ok;
	ok = (RegisterOpDescriptor(
								0, 
								_R(IDS_MARKN_EMPTY),
								CC_RUNTIME_CLASS(DialogBarOp), 
								OPTOKEN_DLGBAROP,
								NULL,		// No GetState fn
								0,          /* help ID */
								_R(IDBBL_NOOP), /* bubble ID */
								0           /* bitmap ID */
								)
			); 

	if (Camelot.DeclareSection( _T("DebugFlags"), 10))
		Camelot.DeclarePref( NULL, _T("ShowSystemBars"), &ShowSystemBars, FALSE, TRUE );
	if (Camelot.DeclareSection(_T("BarsPath"),2))
		Camelot.DeclarePref(_T("BarsPath"), _T("DefaultBarsConfigPath"), &DefaultBarsConfigPath);
	if (Camelot.DeclareSection(_T("BarsVer"),2))
		Camelot.DeclarePref(_T("BarsVer"), _T("BarsVersion"), &BarsVersion);
	return (ok);
}


/********************************************************************************************

>   static OpState DialogBarOp::GetState(String_256* Description, OpDescriptor*)

	Author:     Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:    18/2/94
	Outputs:    Description - GetState fills this string with an approriate description
				of the current state of the push tool
	Returns:    The state of the operation, so that menu items (ticks and greying can be
				done properly
	Purpose:    Find out the state of the operation at the specific time

********************************************************************************************/

OpState DialogBarOp::GetState(String_256* Description, OpDescriptor*)
{
	OpState State;
	
	return State;
}

/********************************************************************************************

>   static BOOL DialogBarOp::WriteBars(CCLexFile& file)

	Author:     Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:    6/4/94
	Inputs:     file = a file object
	Outputs:    -
	Returns:    TRUE if successful, FALSE otherwise
	Purpose:    Writes all the bars in tokenized form to the given file
	Errors:     -
	SeeAlso:    -

********************************************************************************************/

BOOL DialogBarOp::WriteBars(CCLexFile& file)
{
	BOOL            ok = TRUE;
	List*           pList = MessageHandler::GetClassList(CC_RUNTIME_CLASS(DialogBarOp));
	DialogBarOp*    pDialogBarOp = (DialogBarOp*)pList->GetHead();

	while (pDialogBarOp != NULL && ok)
	{
		if (!pDialogBarOp->IsKindOf(CC_RUNTIME_CLASS(SystemBarOp))
			&&!pDialogBarOp->IsKindOf(CC_RUNTIME_CLASS(InformationBarOp)))
			if(!pDialogBarOp->IsListEmpty())
				ok = pDialogBarOp->Write(file);
		pDialogBarOp = (DialogBarOp*)pList->GetNext(pDialogBarOp);
	}

	return (ok);
}

/********************************************************************************************

>	void DialogBarOp::SetCurrentOrientation(Orientation	BarDirection) 

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/4/94
	Inputs:		BarDirection: The orientation of the bar 
	Outputs:	-
	Returns:	-
	Purpose:	To inform the DialogBarOp of its current orientation (horizontal or vertical)
	Errors:		-
	SeeAlso:	-

********************************************************************************************/


void DialogBarOp::SetCurrentOrientation(Orientation	Orient) 
{
	BarOrientation = Orient; 	
}

/********************************************************************************************

>	MsgResult DialogBarOp::Message(Msg* Msg)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/4/94
	Inputs:		Msg: The message to handle
	Outputs:	-				   
	Returns:	-
	Purpose:	The default DialogBarOp Message handler 
				This should be overriden for InformationBar DialogBarOps which will have a
				structure identical to DialogOps. 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

MsgResult DialogBarOp::Message(Msg* Msg)
{
	BOOL DestroyWindow = FALSE; // Set to TRUE if we should destroy the bars window  
	// allow Cancel / BarDeath messages through even for ops with no window
	if(MESSAGE_IS_A(Msg,KeyMsg))
	{
	 	KeyMsg* KMsg = (KeyMsg*)Msg;
 	 	if( KMsg->pKeyPress->GetVirtKey() == CAMKEY(ESCAPE) || 
			KMsg->pKeyPress->GetVirtKey() == CAMKEY(CANCEL) )
		{
			if(BaseBar::EscPressed( KMsg->pKeyPress->IsPress() ))
				return EAT_MSG;
		}
 	 	else if( KMsg->pKeyPress->GetVirtKey() == CAMKEY(CONTROL) )
		{
			if(BaseBar::CtlPressed( KMsg->pKeyPress->IsPress() ))
				return EAT_MSG;
		}
		else if( KMsg->pKeyPress->GetVirtKey() == CAMKEY(MENU) )
		{
			if(BaseBar::AltPressed( KMsg->pKeyPress->IsPress() ))
		 		return EAT_MSG;
		}
		
	}	

	else if (IS_OUR_DIALOG_MSG(Msg)||
		 Msg->IsKindOf(CC_RUNTIME_CLASS(DialogMsg)) &&
			((DialogMsg*)Msg)->DlgMsg == DIM_BAR_DEATH )

	{
		
		// It's a dialog message 
		DialogMsg* DlgMsg = (DialogMsg*)Msg; 
		if(DlgMsg->DlgMsg==DIM_BAR_DEATH)
		{
			if(IS_KIND_OF(InformationBarOp))
				return (DLG_EAT_IF_HUNGRY(DlgMsg)); 
		
		 	if(IsAllBarsOp())
				return (DLG_EAT_IF_HUNGRY(DlgMsg)); 
		}		 	 

// The equivalend of the following now done in DialogOp so "bar" controls (i.e. ones with
// associated OpDescriptors can live in classes derived from DialogOp (not DialogBarOp)
#if 0
		OpDescriptor* OpDesc = NULL;
		BarControl* BarCtrl = NULL;

		// Traverse the list of all BarItems
		for (BarItem* CurrentBarItem = (BarItem*)BarItemList.GetHead();
			 CurrentBarItem != NULL; 
			 CurrentBarItem = (BarItem*)BarItemList.GetNext((ListItem*)CurrentBarItem ))
		{
			if (CurrentBarItem->IsKindOf(CC_RUNTIME_CLASS(BarControl)))
			{
				BarCtrl = (BarControl*)CurrentBarItem;
			 	BarControlInfo BarCtrlInfo = BarCtrl->GetBarControlInfo(BarOrientation == Horizontal);  
			 	   
				// Obtain the OpDescriptor
				OpDesc = (BarCtrl)->
						GetOpDescriptor(BarOrientation == Horizontal);
				
				//ENSURE(OpDesc != NULL, "DialogBarOp has a NULL OpDescriptor"); 
				if (OpDesc != NULL)
				{
					OpDesc->SetBarControlInfo(BarCtrlInfo);
					SendMessageToBarControl(OpDesc, DlgMsg, BarCtrlInfo.ControlID, BarCtrl->GetUniqueGadgetID() );
				}
			}
		}
#endif
		
		if (DlgMsg->DlgMsg == DIM_CANCEL||DlgMsg->DlgMsg == DIM_BAR_DEATH)
		{
PORTNOTE("other", "Removed GetBarPosInfo usage")
#ifndef EXCLUDE_FROM_XARALX
			KernelBarPos* 	pKernelBarPos;
			DockBarType 	Dock;
        	if (GetMainFrame()->GetBarPosInfo(WindowID,&Dock,&pKernelBarPos))
			{
				SetDockBarType(Dock);
				if (Dock == DOCKBAR_FLOAT)
					SetFloatingCPoint(wxPoint(pKernelBarPos->x,pKernelBarPos->y));
				else
				{
					SetSlot(pKernelBarPos->Slot);
					SetOffset(pKernelBarPos->Position);
				}
			}
#endif
			
			// This is new and may cause problems - it used to only be set on
			// a cancel message to a control but now overides default close handling
			// allowing bars and galleries to be closed on a single click

			DestroyWindow = TRUE;
		}
		
		if (DestroyWindow && WindowID != NULL)
		{
			// Destroy the bar window							 
			CCamApp::GetDlgManager()->Delete(WindowID, this);
			WindowID = NULL;
		}

		if (DlgMsg->DlgMsg == DIM_CANCEL||DlgMsg->DlgMsg == DIM_BAR_DEATH)
		{
			// Report bar is closing to interested parties
			BROADCAST_TO_CLASS(BarMsg(BAR_CLOSE,this),DialogOp);
			// This should only be called if we really want to destroy the bar list
			// So far only used when we are about to read in another *.ini file	
		}
		if(DlgMsg->DlgMsg == DIM_BAR_DEATH)
		{
		
			End();
			return(OK);
		}
		return DialogOp::Message(Msg);
	
	//	return (DLG_EAT_IF_HUNGRY(DlgMsg));  
	
	}
		
	// We must destroy the dialog if the application is dying
	else if (MESSAGE_IS_A(Msg,DeathMsg))
	{
		// Camelot is ending, so end the Op
		End();
		return OK;
		// ** Note: We do not need to send an OpDescControlDestroyMsg to all OpDescriptors on
		// receipt of a DeathMsg as a DIM_CANCEL message should have already sent these !
	}

	return DialogOp::Message(Msg);
}

/********************************************************************************************

>	UINT32 DialogBarOp::GetUniqueID()

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/4/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	This function returns a uniqueID  i.e. highest + 1. 
	Errors:		-
	SeeAlso:	DialogBarOp::UpdateStateOfAllBars


********************************************************************************************/

UINT32 DialogBarOp::GetUniqueID()
{
	UINT32 HighestSoFar = 100;
	UINT32 FoundID =0;

	if (WindowID != NULL)  // If the WindowID is NULL then the DialogBar has not been created
	{
		for (BarItem* CurrentBarItem = (BarItem*)BarItemList.GetHead();
				 CurrentBarItem != NULL; 
				 CurrentBarItem = (BarItem*)BarItemList.GetNext( (ListItem*)CurrentBarItem ))
		{
			if (CurrentBarItem->IsKindOf(CC_RUNTIME_CLASS(BarControl)))
			{
				BarControl* BarCtrl = (BarControl*)CurrentBarItem;
		 	   	FoundID = BarCtrl->GetUniqueGadgetID();
		 	   	if(FoundID >HighestSoFar)
		 	   		HighestSoFar = FoundID; 				
				
			}
		}
	} 
	return HighestSoFar + 1;
}

/********************************************************************************************

>	virtual void DialogBarOp::UpdateState()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/4/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	This function gets called during idle time. It calls the GetState fn associated
				with each controls OpDescriptor. The button gets disabled if the operation
				is not executable. 
	Errors:		-
	SeeAlso:	DialogBarOp::UpdateStateOfAllBars


********************************************************************************************/

void DialogBarOp::UpdateState()
{
	if (WindowID != NULL)  // If the WindowID is NULL then the DialogBar has not been created
	{
		String_256 Desc; 
		// Call the GetState fn for all controls
		for (BarItem* CurrentBarItem = (BarItem*)BarItemList.GetHead();
				 CurrentBarItem != NULL; 
				 CurrentBarItem = (BarItem*)BarItemList.GetNext( (ListItem*)CurrentBarItem ))
		{
			if (CurrentBarItem->IsKindOf(CC_RUNTIME_CLASS(BarControl)))
			{
				BarControl* BarCtrl = (BarControl*)CurrentBarItem;
		 	   
				// Obtain the OpDescriptor
				OpDescriptor* OpDesc = (BarCtrl)->
						GetOpDescriptor(BarOrientation == Horizontal);

				if (OpDesc != NULL)
				{
					OpState NewState = OpDesc->GetOpsState(&Desc); 

					// if a description has been returned
					/*
					// Special disabled string description stuff cannot be implemented yet
					// it requires shield technology
					if (!ButtonDesc.IsEmpty() && (newState.Greyed))
					{
						String_256 disabledStub(_R(IDS_DISABLED_BECAUSE));
			
						WhyDisabled = disabledStub;

						// Set up reason why item is disabled
						WhyDisabled += ButtonDesc;
				    }
		   			*/
					// Set the controls greyed state
					if (BarCtrl->IsEnabled() != !NewState.Greyed)
					{
						// The controls state has changed 
						EnableGadget(BarCtrl->
							GetUniqueGadgetID(), !NewState.Greyed);
						BarCtrl->SetEnabledState(!NewState.Greyed); 
					}
					// set the control's ticked state
					// First determine if the control can be ticked
					if (IsGadgetTickable(BarCtrl->GetUniqueGadgetID()))
					{
						// All tickable gadget's must have a BOOL value (TRUE = ticked)
						BOOL Selected = GetLongGadgetValue(BarCtrl->GetUniqueGadgetID(),0,1);
						if(Selected != NewState.Ticked)
						{
							// We need to change the ticked state
							SetBoolGadgetSelected(BarCtrl->GetUniqueGadgetID(),NewState.Ticked!=0);
						}
					}
 
				}
			}
		}
	}
}


/********************************************************************************************

>	static void DialogBarOp::UpdateStatesOfAllBars()


	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/4/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	This static function gets called during	idle time
				If the system state has changed then it updates the enabled state of all 
				visible DialogBars 
	Errors:		-
	SeeAlso:	DialogBarOp::UpdateState
	SeeAlso:	DialogBarOp::SetSystemStateChanged

********************************************************************************************/

void DialogBarOp::UpdateStateOfAllBars()
{
	// Determine if we need to update the state of the bars.
	if (ShouldUpdateBarState())
	{
	/*	if (IsUserName("Simon"))
		{
			TRACE( _T("The state of the bars is being updated")); 
		}
	  */
		List* pBarList = MessageHandler::GetClassList(CC_RUNTIME_CLASS(DialogBarOp));
		ListItem*  pCurrentBar = pBarList->GetHead();
		while (pCurrentBar != NULL)
		{
			((DialogBarOp*)pCurrentBar)->UpdateState(); 
 			pCurrentBar = pBarList->GetNext(pCurrentBar);
		}
		// The bars now reflect the system state 
		SetSystemStateChanged(FALSE); 
	}			
}



/********************************************************************************************

>	void DialogBarOp::Delete()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com> (using Simon code)
	Created:	29/4/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	The DialogBarOp Delete method
				Deletes the window and resets WindowID to NULL.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/


void DialogBarOp::Delete()
{
	if (WindowID != NULL)
	{
		PORTNOTETRACE("dialog","Warning - DialogBarOp::Delete called and disabled");
#ifndef EXCLUDE_FROM_XARALX
		KernelBarPos* 	pKernelBarPos;
		DockBarType 	Dock;

		if (GetMainFrame()->GetBarPosInfo(WindowID,&Dock,&pKernelBarPos))
		{
			SetDockBarType(Dock);
			if (Dock == DOCKBAR_FLOAT)
				SetFloatingCPoint(wxPoint(pKernelBarPos->x,pKernelBarPos->y));
			else
			{
				SetSlot(pKernelBarPos->Slot);
				SetOffset(pKernelBarPos->Position);
			}
		}
#endif
		CCamApp::GetDlgManager()->Delete(WindowID, this);
		WindowID = NULL;
	}
}
/********************************************************************************************

>	static BOOL DialogBarOp::SetValidPath(String_256 FileName)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/10/94
	Inputs:		FileName	This dictates whether we look for the full screen or normal
							mode file. Should be either "Normal" or "Clean" when "Clear"
							corresponds to the full screen operation.
	Outputs:	-
	Returns:	TRUE if successful
	Purpose:	looks for bars.con files  first in prefs path then in windows dir
				if found sets ValidPath and return TRUE else return FALSE 
	Errors:		

	Note : These functions will fail badly with path names > 255

********************************************************************************************/

 BOOL DialogBarOp::SetValidPath(String_256 FileName)
 {
#pragma message( __LOCMSG__ "DialogBarOp::SetValidPath - do nothing" )
	TRACE( _T("Warning - DialogBarOp::SetValidPath called") );
/*	 // new naming convention = PRODUCT_BASENAME.cle/nor
	// I've left the above intact as I can't get hold of the files 
	// I need to do this properly - this function is called with either "normal" or "clean"

	// Updated for doing the CorelXARA product 28/9/95
	// File was called XStudio.nor or .cle (or XViewer.nor or .cle)
	// We will now call them all CorelX##.ini where #### in this case is either BN or BF 
	String_256 File( _T("") );
	if(!FileName.CompareTo( _T("normal") ))
		File = PRODUCT_BARSNORMAL_INIFILE;
	else
		File = PRODUCT_BARSFULL_INIFILE;

#if defined(__WXMSW__)
	const UINT32 cchMaxPath = _MAX_PATH;
#else
	const UINT32 cchMaxPath = 1 + pathconf( "/", _PC_PATH_MAX );
#endif
	TCHAR *szBuff = static_cast<TCHAR *>( alloca( cchMaxPath * sizeof(TCHAR) ) );
	CCDiskFile file;
	BOOL ok = FALSE;

	// first check prefs path

	ok = DefaultBarsConfigPath.Length() > 0;
	if (ok)
	{
		String_256 Temp = DefaultBarsConfigPath;
		Temp += _T("\\");
		Temp += File;
		camStrcpy( szBuff, Temp );
		TRACEUSER( "chris", _T("Loading Bars from Prefs path\n"));
		if (_taccess(szBuff, 0) == 0)
		{
			// yes - use this name
			ValidPath.SetPathName(szBuff);
			return TRUE;
		}
	}
	
	// either there is no pref or we can't find the file . try windows directory

	UINT32 Len = GetWindowsDirectory((LPSTR)szBuff, cchMaxPath - 1);
	if ((Len > 0) && (Len < cchMaxPath))
	{
		String_256 Temp = _T("\\") + File;
		camStrcat(szBuff, Temp );
		TRACEUSER( "chris", _T("Loading Bars from Windows path\n"));
		if (_taccess(szBuff, 0) == 0)
		{
			// yes - use this name
			ValidPath.SetPathName(szBuff);
			return TRUE;
		}
	}

	// oops .. Can't find a bars file anywhere !!! */
	return FALSE;
 }

/********************************************************************************************

>	static BOOL DialogBarOp::LoadDefaultBars()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com> & MarkN
	Created:	20/4/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if successful
	Purpose:	loads in, and then creates initial bars
				This function must get called after the docking bars have been created 
	Errors:		-
	SeeAlso:	LoadBarsFromFile,LoadBarsFromRes,CreateBars

********************************************************************************************/

BOOL DialogBarOp::LoadDefaultBars()
{  
	BOOL ok=TRUE;
	if (ok)  ok = LoadBarsFromRes();  	// When we are able to read BARS.INI from file, delete
										// this line and uncomment the two above
	if (ok)  ok = CreateBars();
	return ok;

}

/********************************************************************************************

>	static BOOL DialogBarOp::LoadNamedBars(String_256 FileName)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com> & MarkN
	Created:	20/4/94
	Inputs:		FileName	This dictates whether we look for the full screen or normal
							mode file. Should be either "Normal" or "Clean" when "Clear"
							corresponds to the full screen operation.
	Outputs:	-
	Returns:	TRUE if successful
	Purpose:	loads in, and then creates initial bars
				This function must get called after the docking bars have been created 
	Errors:		-
	SeeAlso:	LoadBarsFromFile,LoadBarsFromRes,CreateBars

********************************************************************************************/

BOOL DialogBarOp::LoadNamedBars(String_256 FileName)
{  
#pragma message( __LOCMSG__ "DialogBarOp::LoadNamedBars - do nothing" )
	TRACE( _T("Warning - DialogBarOp::LoadNamedBars called") );
/*	// Simon - In dire emergency please uncomment this back in - Chris.
		
//	if (ok)  ok = LoadBarsFromRes();  	// When we are able to read BARS.INI from file, delete
//										// this line and uncomment the two above
//	if (ok)  ok = CreateBars();
//	return ok;
	
	BOOL ok=FALSE;
	BOOL LoadFromRes =FALSE;
	BOOL BadBarsFile=FALSE;
	BOOL FileOpened = FALSE;
	
	#ifndef STANDALONE
	if(!ReadResVersion())
	{
		// look for bars config file
	   	ok = LoadNamedBarFile((TCHAR *)FileName,&FileOpened);

		if(FileOpened)
			// Note if it's a bad bars file
			BadBarsFile=!ok;
	}
	else
		ok = FALSE;
	#endif

	// Failed so we'd better load from resources !!
	if(!ok)
	{
	  	// kill all bars ( except infobar at the mo..) in case any were created with a bad bars file
		if (BadBarsFile)
		{
			BROADCAST_TO_CLASS(DialogMsg(NULL, DIM_BAR_DEATH, NULL ),DialogBarOp);
		}

		// load default bars from the bound in resource files
		LoadFromRes = ok = LoadBarsFromRes();
		ERROR2IF(!ok,FALSE,"Serious error - unable to load bars from bound in resource files");
	}

	if (ok) ok = CreateBars();

	// I dont like this very much but it gets round the need to distribute 2 config files
	// we can probably lose this for the release - note it should also go if we shift up 
	// to multiple config files Chris. 
	if(ok)
	{
	 	if(!(FileName.CompareTo("clean")))
	 	{
			if(!GetMainFrame()->IsFullScreenMode())
			{
				GetMainFrame()->DoFullScreenMode(TRUE);
				if(LoadFromRes)
				{
					GetMainFrame()->ShowStatusBar(FALSE);
					GetMainFrame()->ShowScrollBars(FALSE);	
				}
			}
		}
	}

	// If we come across a bad bars file, tell the user
	ERROR1IF(BadBarsFile,ok,_R(IDE_BADBARSFILE));

	return (ok); */

	return false;
}



/********************************************************************************************

>	static BOOL DialogBarOp::LoadBars()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com> & MarkN
	Created:	20/4/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if successful
	Purpose:	loads in, and then creates initial bars
				This function must get called after the docking bars have been created 
	Errors:		-
	SeeAlso:	LoadBarsFromFile,LoadBarsFromRes,CreateBars

********************************************************************************************/

BOOL DialogBarOp::LoadBars()
{  
#pragma message( __LOCMSG__ "DialogBarOp::LoadBars - do nothing" )
	TRACE( _T("Warning - DialogBarOp::LoadBars called") );
//	return LoadNamedBars(GetMainFrame()->InFullScreenMode() ? "clean" : "normal");

	return false;
}

/********************************************************************************************

>	static BOOL DialogBarOp::WipeRegistrySettingsForBar(String_256 * pBarName)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/2/97
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if successful
	Purpose:	Wipes bar registry settings for the named section.
				Called if the user selects default settings.
	Errors:		-


********************************************************************************************/

BOOL DialogBarOp::WipeRegistrySettingsForBar(TCHAR * pBarName)
{
#pragma message( __LOCMSG__ "DialogBarOp::WipeRegistrySettingsForBar - do nothing" )
	TRACE( _T("Warning - DialogBarOp::WipeRegistrySettingsForBar called") );
/*	ERROR2IF(pBarName == NULL,FALSE,"WipeRegistrySettingsForBar bar section name");
	// Wipe all the bars settings from the registry.
	// We need to do sub-directories first as you cannot wipe a directory which has sub-directories
	// (well it is rumoured that you cannot do this on NT but it works on 95!)
	
	// Wipe the named sub sections
	String_256 MainKeyName = pBarName;
	HKEY hRegKey = OpenRegKey(hAppStateRegKey, MainKeyName);
	// Was that section there?
	if (hRegKey != NULL)
	{
		// yes so wipe the named sub-sections
		DeleteRegKey(hRegKey, PRODUCT_REGISTRYKEY_BARSETTINGS );
		DeleteRegKey(hRegKey, TokenTable[TOKEN_INFO_BAR].Token );

		// delete all the numbered sections which contain the bar definitions
		BOOL Finished = FALSE;
		INT32 i = 0;
		TCHAR buf[10];
		while (!Finished)
		{ 
			// Make up the bar number for this index
			wsprintf(buf, TEXT("%d"), i + 1);
			String_256 name(buf);
			if (!DeleteRegKey(hRegKey, name))
				Finished = TRUE;

			i++;
		}

		CloseRegKey(hRegKey);

		// Wipe the main key
		DeleteRegKey(hAppStateRegKey, MainKeyName);
	}

	return TRUE; */

	return true;
}

/********************************************************************************************

>	static BOOL DialogBarOp::WipeBarFiles()

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/10/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if successful
	Purpose:	Wipes bar config files from disc - called if the user selects default settings.
	Errors:		-


********************************************************************************************/

BOOL DialogBarOp::WipeBarFiles()
{  
//	BOOL ok = TRUE;
	BOOL UnlinkOK = TRUE;

#pragma message( __LOCMSG__ "DialogBarOp::WipeBarFiles - do nothing" )
	TRACE( _T("Warning - DialogBarOp::WipeBarFiles called") );
/*#ifdef SAVE_PREFS_TO_REGISTRY
	// Wipe all the bars settings from the registry.
	// We need to do sub-directories first as you cannot wipe a directory which has sub-directories
	// (well it is rumoured that you cannot do this on NT but it works on 95!)
	WipeRegistrySettingsForBar(PRODUCT_REGISTRYKEY_BARSFULL);
	WipeRegistrySettingsForBar(PRODUCT_REGISTRYKEY_BARSNORMAL);

	// We must wipe the ini file as well as otherwise when the code checks to 
	// see if the registry settings and then falls back on the ini file if they
	// are not there, then we wont be using the default settings.
#endif

	String_256 FilePath;
	//Find the path to a valid xstudio.nor file
	ok = SetValidPath("normal");
	// the file must exist... so we can delete it
	if(ok)
	{	
		FilePath = ValidPath.GetPath();
		// delete the file
		if(_unlink(FilePath) !=0)
			UnlinkOK = FALSE;
	}

	//Find the path to a valid xstudio.cle file
	ok = SetValidPath("clean");
	if(ok)
	{
		FilePath = ValidPath.GetPath();
		// delete the file
		if(_unlink(FilePath) !=0)
			UnlinkOK = FALSE;
	
	}
	
	ERROR2IF(UnlinkOK==FALSE,FALSE,"Couldn't delete a bars config file"); */
	
	return UnlinkOK;
}

/********************************************************************************************

>	static BOOL DialogBarOp::LoadNamedBarFile(String_256 FileName ,BOOL * Opened)

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/10/94
	Inputs:		FileName - file to load
				Opened 	 - flag.. did we open the file
	Outputs:	-
	Returns:	TRUE if successful
	Purpose:	loads in bar defs from *.confile.
				This function must get called after the docking bars have been created 
	Errors:		-
	SeeAlso:	LoadBars,LoadBarsFromRes,CreateBars


********************************************************************************************/

BOOL DialogBarOp::LoadNamedBarFile(String_256 FileName ,BOOL * Opened)
{  
#pragma message( __LOCMSG__ "DialogBarOp::LoadNamedBarFile - do nothing" )
	TRACE( _T("Warning - DialogBarOp::LoadNamedBarFile called") );
/*	BOOL ok = TRUE;
	BOOL open = FALSE;
	UINT32 NumBars = 0;
	UINT32 NumDockingBars = 0; 
	
#ifdef SAVE_PREFS_TO_REGISTRY
	// Try and read the separate bars settings from the registry
	// First work out the section name we require and then open it up ready for reading
	String_256 KeySectionName;
	if (!FileName.CompareTo("normal"))
		KeySectionName = PRODUCT_REGISTRYKEY_BARSNORMAL;
	else
		KeySectionName = PRODUCT_REGISTRYKEY_BARSFULL;

	HKEY hRegKey = OpenRegKey(hAppStateRegKey, KeySectionName);
	// Was that section there?
	// If we failed then read bars from the ini file
	if (hRegKey != NULL)
	{
		// Say that we have opened the file and try to read in definitions
		// so if we fail then the caller gets a chance to destruct things.
		*Opened = TRUE;
		// We opened the key so its present. Read all the sub sections from there.
		ok = ok && ReadDockingBarsFromRegistry(hRegKey);
		ok = ok && ReadWorkSpaceInfoFromRegistry(hRegKey);
		ok = ok && ReadBarsFromRegistry(hRegKey);
		CloseRegKey(hRegKey);
	}
	else
#endif
	{
		// If the main ini file is in the registry then don't even think about using ini files.
		// This is because the version number of the bars is stored in the ini file!!!! We will
		// have already checked the one stored in the registry rather than the one in the
		// ini file. So this would be an unsafe operation.
		// If we are not using the registry then we will have used the ini file version number
		// and so it is safer.
		if (CCamApp::AreWeUsingTheRegistry())
			return FALSE;
		
		// Read all of the data from the ini file

		//Find the path to a valid *.con file
		ok = SetValidPath((TCHAR *)FileName);
		
		if(!ok)
			return ok;

		PathName path  = ValidPath;
		CCDiskFile file;
		DialogBarOpCreate BarCreate;
		if (ok) ok = open = file.open(path,ios::in);
		if (ok) ok = ReadBarsFromFile(file,BarCreate,&NumBars,&NumDockingBars);
		if (ok) ok = ((NumBars > 0) && (NumDockingBars == NUM_DOCKBARS));
		if (open) file.close();
		if(!ok)
			TRACEUSER( "Neville", _T("Can't find a bars file - loading from resources\n"));
		* Opened = open;
	}

	return ok; */

	return false;
}

/********************************************************************************************

>	static BOOL DialogBarOp::ReadResVersion()

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/4/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if bars.ini is a new revision
	Purpose:	Reads Revision number of bars.ini checks whether this is the same as
				the version in prefs 
	Errors:		-
	SeeAlso:	LoadBarsFromFile,LoadBars,CreateBars

********************************************************************************************/

BOOL DialogBarOp::ReadResVersion()
{  
#pragma message( __LOCMSG__ "DialogBarOp::ReadResVersion- do nothing" )
	TRACE( _T("Warning - DialogBarOp::ReadResVersion called") );
/*	BOOL ok = TRUE,open=FALSE; 
	BOOL NewVersion = FALSE;
	CCResTextFile file;	// Resource File

 	if (ok) ok = open = file.open(_R(IDM_DEFAULT_BARS), _R(IDT_CAM_BARS_RES));
	String_64 Version;
	String_64 Temp;
	ok = file.InitLexer();				// Initialise lexing routine
	file.SetStringDelimiters("$$");
	const TCHAR* TokenBuf = file.GetTokenBuf();
	if (ok) ok = file.GetSimpleToken();
	if (ok) Temp=((const TCHAR*) TokenBuf);
	// this is a bit naff but RCS adds an extra white space at the end of the
	// the string when it substitutes
	Temp.Left(&Version,Temp.Length()-1);
	if(Version.CompareTo(BarsVersion))
	{
		camStrcpy(BarsVersion,Version);
	 	NewVersion = TRUE;
	}
	if (open) file.close();

	return NewVersion; */

	return false;
}

/********************************************************************************************

>	static BOOL DialogBarOp::LoadBarsFromRes()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/4/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if successful
	Purpose:	loads in bars from the bars.ini  resource that's bound into the exe.
				This function must get called after the docking bars have been created 
	Errors:		-
	SeeAlso:	LoadBarsFromFile,LoadBars,CreateBars

********************************************************************************************/

BOOL DialogBarOp::LoadBarsFromRes()
{  
	BOOL ok = TRUE;
//if (IsUserName("MarkN")) TRACE( _T("DialogBarOp::LoadBarsFromRes()\n"));
#pragma message( __LOCMSG__ "DialogBarOp::ReadResVersion- do nothing" )
	TRACE( _T("Warning - DialogBarOp::ReadResVersion called") );
/*
	BOOL open=FALSE; 
	UINT32 NumBars;
	UINT32 NumDockingBars;
	CCResTextFile file;	// Resource File
	DialogBarOpCreate BarCreate;

 	if (ok) ok = open = file.open(_R(IDM_DEFAULT_BARS), _R(IDT_CAM_BARS_RES));
	if (ok) ok = ReadBarsFromFile(file,BarCreate,&NumBars,&NumDockingBars);
	if (ok) ok = ((NumBars > 0) && (NumDockingBars == NUM_DOCKBARS));
	
	if (open) file.close(); */

	return ok; 
}

/********************************************************************************************

>	static BOOL DialogBarOp::CreateBars()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/4/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if successful
	Purpose:	Creates initial bars loaded in using either LoadBarsFromFile or
				LoadBarsFromRes.
				Only DialogBarOps are created. All derived classes are ignored, such
				as InformationBarOps and SystemBarOps.
				This function must get called after the docking bars have been created 
	Errors:		-
	SeeAlso:	LoadBarsFromFile,LoadBarsFromRes,LoadBars

********************************************************************************************/

BOOL DialogBarOp::CreateBars()
{


	List*       pList = MessageHandler::GetClassList(CC_RUNTIME_CLASS(DialogBarOp));
	ListItem*   pListItem = pList->GetHead();
//	ListItem*   pListItem = pList->GetTail();

	OILFixedDockingBar::DoNotTidyBars ();

	while (pListItem != NULL)
	{
		DialogBarOp* pDialogBarOp = (DialogBarOp*)pListItem;

		
//		TCHAR* p=pDialogBarOp->Name;

		if 	(	(pDialogBarOp->GetRuntimeClass() == CC_RUNTIME_CLASS(DialogBarOp))
	/*		#ifdef _DEBUG
			||  (	pDialogBarOp->GetRuntimeClass() == CC_RUNTIME_CLASS(SystemBarOp)
					&& ShowSystemBars
				)
			#endif*/
			|| (pDialogBarOp->IsKindOf(CC_RUNTIME_CLASS(SuperGallery)))
			)								    
		{
			if ((pDialogBarOp->GetNumBarItems() > 0) && pDialogBarOp->InitiallyVisible)
			{
				if (!pDialogBarOp->Create())
				{
					ENSURE(FALSE, "Failed to create bar"); 
					return FALSE; 
				}
			}
		}
//		pListItem = pList->GetPrev(pListItem);
	   	pListItem = pList->GetNext(pListItem);
	}

	OILFixedDockingBar::AllowTidyBars ();

	return TRUE; 
}

/********************************************************************************************

>	static DialogBarOp* DialogBarOp::FindDialogBarOp(String_32& DialogBarOpName, INT32 limit = -1)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/4/94
	Inputs:		DialogBarOpName = Name of op to find
				limit	if -1 then it means use the whole name for the check (default)
						if > 0 then limit the check to the limit number of characters
	Outputs:	-
	Returns:	ptr to DialogBarOp
				NULL is returned if not found
	Purpose:	Looks for a given DialogBarOp by using its name 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

DialogBarOp* DialogBarOp::FindDialogBarOp(String_32& DialogBarOpName, INT32 limit)
{
	List*       	pList = MessageHandler::GetClassList(CC_RUNTIME_CLASS(DialogBarOp));
	DialogBarOp*	pDialogBarOp = (DialogBarOp*)pList->GetHead();
	
	String_32		OpName;
	while (pDialogBarOp != NULL)
	{
		if (limit > 0)
			pDialogBarOp->Name.Left(&OpName, limit);
		else
			OpName = pDialogBarOp->Name;
		
		if (OpName == DialogBarOpName)
			return (pDialogBarOp);

		pDialogBarOp = (DialogBarOp*)pList->GetNext(pDialogBarOp);
	}

	return NULL;
}


/********************************************************************************************

	static UINT32 DialogBarOp::FindUniqueBarNumber()

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/4/94
	Inputs:		DialogBarOpName = Name of op to find
	Outputs:	-
	Returns:	unique nuber to put in new bar name
	Purpose:	- 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

UINT32 DialogBarOp::FindUniqueBarNumber()
{
	List*       	pList = MessageHandler::GetClassList(CC_RUNTIME_CLASS(DialogBarOp));
	DialogBarOp*	pDialogBarOp = (DialogBarOp*)pList->GetHead();
	UINT32 Highest = 0;
	String_32 name;
	String_32 prefix;
	String_32 number;
	while (pDialogBarOp != NULL)
	{
		name = pDialogBarOp->GetName();
		name.Left(&prefix,3);
		if(!prefix.CompareTo(_T("Bar")))
		{	
			//Highest ++;
			name.Right(&number,name.Length()-4);
			TCHAR *pszMark;
			UINT32 SoFar = camStrtol( (const TCHAR *)number, &pszMark, 10 );
			if(SoFar>=Highest)
				Highest = SoFar;
		}
		pDialogBarOp = (DialogBarOp*)pList->GetNext(pDialogBarOp);
	}

	return Highest+1;
} 

/********************************************************************************************

>	static DialogBarOp* DialogBarOp::FindDialogBarOp(String_32& DialogBarOpName)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/4/94
	Inputs:		DialogBarOpName = Name of op to find
	Outputs:	-
	Returns:	ptr to DialogBarOp
				NULL is returned if not found
	Purpose:	Looks for a given DialogBarOp by using its name 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

DialogBarOp* DialogBarOp::FindDialogBarOp( UINT_PTR ID )
{
	List*       	pList = MessageHandler::GetClassList(CC_RUNTIME_CLASS(DialogBarOp));
	DialogBarOp*	pDialogBarOp = (DialogBarOp*)pList->GetHead();

	while (pDialogBarOp != NULL)
	{
		if (pDialogBarOp->WindowID == (CWindowID)ID)
			return (pDialogBarOp);

		pDialogBarOp = (DialogBarOp*)pList->GetNext(pDialogBarOp);
	}

	return NULL;
} 


//-----

#ifdef _DEBUG

#include "speedtst.h"

#define MY_FILE_NAME "C:\\bars.ini"

void DialogBarOp::TestStart()
{
/*
	if (!IsUserName("MarkN")) return;

	TRACE( _T("DialogBarOp::TestStart()\n"));

	BOOL ok = TRUE;
	String_256 str = MY_FILE_NAME;
	PathName path;
	CCDiskFile file;

	if (ok) ok = path.SetPathName(str);
	if (ok) ok = file.open(path,ios::in);
	if (ok) ok = ReadBarsFromFile(file);
	if (ok) file.close();

	{
		List*       pList = MessageHandler::GetClassList(CC_RUNTIME_CLASS(DialogBarOp));
		ListItem*   pListItem = pList->GetHead();
		INT32         c=0;
		while (pListItem != NULL)
		{
			c++;
			((DialogBarOp *)pListItem )->Create();
			TRACE( _T("pListItem = %p\n"),pListItem);
			pListItem = pList->GetNext(pListItem);
		}

		TRACE( _T("Num dialogbarops = %d\n"),c);
	}
*/
}



void DialogBarOp::TestEnd()
{
#pragma message( __LOCMSG__ "DialogBarOp::TestEnd - do nothing" )
	TRACE( _T("Warning - DialogBarOp::TestEnd called") );
/*	if( !Error::IsUserName("MarkN") )
		return;

	TRACE( _T("DialogBarOp::TestEnd()\n"));

	BOOL ok = TRUE;
	String_256 str = MY_FILE_NAME;
	PathName path;
	CCDiskFile file;
	if (ok) ok = path.SetPathName(str);
	if (ok) ok = file.open(path,ios::out);
	if (ok) ok = DialogBarOp::WriteBarsToFile(file);
	if (ok) file.close(); */
}

/*
	DialogBarOp* pTestBar = new DialogBarOp;
	if (pTestBar == NULL) return;

	String_32 str = "xxxxxx";
	pTestBar->SetName(str);
	pTestBar->SetDockBarType(DOCKBAR_BOTTOM);

	BarControl*   pBarCtrl1=NULL;
	BarControl*   pBarCtrl2=NULL;
	BarControl*   pBarCtrl3=NULL;
	BarSeparator* pBarSep=NULL;
	BarLineFeed*  pBarLF=NULL;

	pBarCtrl1   = new BarControl;
	pBarCtrl2   = new BarControl;
	pBarCtrl3   = new BarControl;
	pBarSep     = new BarSeparator;
	pBarLF      = new BarLineFeed;

	if (pBarCtrl1!=NULL && pBarCtrl2!=NULL && pBarCtrl3!=NULL && pBarSep!=NULL && pBarLF!=NULL)
	{
		SimpleBarControl SBCtrl;
		SBCtrl.BarCtrlInfo.ResourceID   = 0;
		SBCtrl.BarCtrlInfo.ControlID    = 0;
		SBCtrl.BarCtrlInfo.ToolID       = 0;

		SBCtrl.pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_SNAPTOGRID);    
		pBarCtrl1->SetHorzControl(SBCtrl);
		pBarCtrl1->SetVertControl(SBCtrl);
		pTestBar->AddBarItem(pBarCtrl1);

		pTestBar->AddBarItem(pBarSep);

		SBCtrl.pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_SNAPTOGRID);    
		pBarCtrl2->SetHorzControl(SBCtrl);
		pBarCtrl2->SetVertControl(SBCtrl);
		pTestBar->AddBarItem(pBarCtrl2);

		pTestBar->AddBarItem(pBarLF);

		SBCtrl.pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_SHOWGRID);  
		pBarCtrl3->SetHorzControl(SBCtrl);
		pBarCtrl3->SetVertControl(SBCtrl);
		pTestBar->AddBarItem(pBarCtrl3);
	}

	if (pBarCtrl1 != NULL) delete pBarCtrl1;
	if (pBarCtrl2 != NULL) delete pBarCtrl2;
	if (pBarCtrl3 != NULL) delete pBarCtrl3;
	if (pBarSep   != NULL) delete pBarSep;
	if (pBarLF    != NULL) delete pBarLF;

	pTestBar->End();
*/

/*

	if (IsUserName("MarkN"))
	{
		BOOL ok = TRUE;
	
		CCDiskFile file(256);
		String_256 pathnamestr = "C:\\bars.ini";
		PathName pathname(pathnamestr);

		ok = file.open(pathname,ios::out,filebuf::sh_write);
	
		if (ok)
		{
			String_256 str = "Case 3453657\n";
			file.write(str,str.Length());
		}
		else
		{
			TRACE( _T("Unable to open %s\n"),(TCHAR*)pathnamestr);
		}
		file.close();
	}

}
*/

#endif // _DEBUG


/********************************************************************************************

>	static BOOL DialogBarOp::WriteNamedBars(String_256 str)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/4/94
	Inputs:		File name
	Outputs:	-
	Returns:	Error condition
	Purpose:	writes current bars set up to *.ini file 
	Errors:		write failed
	SeeAlso:	-

********************************************************************************************/

BOOL DialogBarOp::WriteNamedBars(String_256 FileName)
{

#ifdef STANDALONE

	// Standalone doesn't need to write any bars out
	return TRUE;

#else
	BOOL ok = TRUE;

#ifdef SAVE_PREFS_TO_REGISTRY
	// Check to see which section we are trying to access and then open the relevant key
	// for that section
	String_256 KeySectionName;
	if (!FileName.CompareTo("normal"))
		KeySectionName = PRODUCT_REGISTRYKEY_BARSNORMAL;
	else
		KeySectionName = PRODUCT_REGISTRYKEY_BARSFULL;

	HKEY hRegKey = CreateRegKey(hAppStateRegKey, KeySectionName);
	if (hRegKey != NULL)
	{
		// Write the workspace settings into the registry
		// These use the BarSettings sections
		ok = ok && WriteWorkSpaceInfoToRegistry(hRegKey);
		ok = ok && WriteDockingBarsToRegistry(hRegKey);
		// These write out to their own sections
		ok = ok && WriteBarsToRegistry(hRegKey);
	}
	CloseRegKey(hRegKey);

#else	
	// File was called XStudio.nor or .cle (or XViewer.nor or .cle)
	// We will now call them all CorelX##.ini where #### in this case is either BN or BF
	String_256 File("");

	if(!FileName.CompareTo( _T("normal") ))
		File = PRODUCT_BARSNORMAL_INIFILE;
	else
		File = PRODUCT_BARSFULL_INIFILE;

	// first look for a prefs path
#if defined(__WXMSW__)
	const UINT32 cchMaxPath = _MAX_PATH;
#else
	const UINT32 cchMaxPath = 1 + pathconf( "/", _PC_PATH_MAX );
#endif
	TCHAR *szBuff = static_cast<TCHAR *>(alloca( cchMaxPath * sizeof(TCHAR) ));
	if (DefaultBarsConfigPath.Length() > 0)
	{
		camStrcpy( szBuff, DefaultBarsConfigPath );
		camStrcpy( szBuff, _T("\\") );
		camStrcpy( szBuff, File );
	}
	// no entry use windows dir
	else
	{
#if defined(__WXMSW__)
		UINT32 Len = GetWindowsDirectory(szBuff, cchMaxPath - 1);
#else
		camStrcpy( szBuff, _T("~/.XaraLX") );
		UINT32 Len = camStrlen( szBuff );
#endif
		
		if ((Len > 0) && (Len < cchMaxPath))
		{
			camStrcat( szBuff, _T("\\") );
			camStrcat( szBuff, File );
		}	
	}

	PathName path;

	CCDiskFile file;
	if (ok) ok = path.SetPathName(szBuff);
	if (ok) ok = file.open(path,ios::out);
	if (ok) ok = DialogBarOp::WriteBarsToFile(file);
	if (ok) file.close();
	
	// the write has failed probably due to  disc full
	// we'll delete the corrupt file and set an error.
#pragma message( __LOCMSG__ "Removed _tremove on non-Windows OS" )
	#if defined(__WXMSW__)
	if(!ok)
		_tremove(szBuff);
	#endif
	
	ERROR1IF(!ok,FALSE,_R(IDE_CANT_SAVE_BARS));
#endif

	// return the result to the caller
	return ok;
#endif
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

// -----------------------------------------------------------------------------------------
// BarControlBaseMethods
// Called to obtain the bar item's BubbleID

/********************************************************************************************

>	UINT32 BarControlBase::GetBubbleID(BOOL Horz)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/5/94
	Inputs:		Horz: The orientation of the bar
	Outputs:	The controls current BubbleID
	Returns:	Bubble help resource string associated with the control
	Purpose:	In derived classes will return the bubble help resource string currently
				associated with the control. In the base class an ENSURE occurs.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

UINT32 BarControlBase::GetBubbleID(BOOL Horz)
{
	ENSURE(FALSE, "Pure virtual BarControlBase::GetBubbleID called"); 
	return 0; 	
}
  

// ( Bodge) Don't no what this is (If it's the status bar description then it will
//  be required) ?	(Mark could you confirm this)

/********************************************************************************************

>	UINT32 BarControlBase::GetStatusID(BOOL Horz)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/5/94
	Inputs:		Horz: The orientation of the bar
	Outputs:	-
	Returns:	-
	Purpose:	
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

UINT32 BarControlBase::GetStatusID(BOOL Horz)
{
	ENSURE(FALSE, "Pure virtual BarControlBase::GetStatusID called"); 
	return 0;
} 

// -----------------------------------------------------------------------------------------
// BarControlMethods

/********************************************************************************************

>   void BarControl::SetHorzOpDesc(OpDescriptor* pOpDesc)

	Author:     Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:    19/4/94
	Inputs:     pOpDesc = ptr to op desc to use for horizontal control
	Outputs:    -
	Returns:    -
	Purpose:    Sets the info regarding the horizontal version of this control
	Errors:     -
	SeeAlso:    -

********************************************************************************************/

void BarControl::SetHorzOpDesc(OpDescriptor* pOpDesc)
{
	SetOpDesc(&Horizontal,pOpDesc);
}

/********************************************************************************************

>   void BarControl::SetVertOpDesc(OpDescriptor* pOpDesc)

	Author:     Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:    19/4/94
	Inputs:     pOpDesc = ptr to op desc to use for vertical control
	Outputs:    -
	Returns:    -
	Purpose:    Sets the info regarding the vertical version of this control
	Errors:     -
	SeeAlso:    -

********************************************************************************************/

void BarControl::SetVertOpDesc(OpDescriptor* pOpDesc)
{
	SetOpDesc(&Vertical,pOpDesc);
}


/********************************************************************************************

>   void BarControl::SetOpDesc(SimpleBarControl* pSimpleBC, OpDescriptor* pOpDesc)

	Author:     Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:    19/4/94
	Inputs:     pSimpleBC = the simple bar control that should be updated      
				pOpDesc = ptr to op desc to use for given control 
	Outputs:    -
	Returns:    -
	Purpose:    Sets the info for this control using the given op desc
				It's intended that this be called when a bar control is added
				to a bar via the toolbar dialogue interface.
	Errors:     -
	SeeAlso:    -

********************************************************************************************/

void BarControl::SetOpDesc(SimpleBarControl* pSimpleBC, OpDescriptor* pOpDesc)
{
	// Get ptr to default control info
	const BarControlInfo* pBarCtrlInfo = pOpDesc->GetBarControlInfo();

	// Copy the default info stored in the OpDesc bar ctrl info into
	// this bar control.
	pSimpleBC->BarCtrlInfo.ResourceID   = pBarCtrlInfo->ResourceID;
	pSimpleBC->BarCtrlInfo.ControlID    = pBarCtrlInfo->ControlID;
	pSimpleBC->BarCtrlInfo.ToolID       = pBarCtrlInfo->ToolID;

	// Copy ptr to the OpDesc.
	pSimpleBC->pOpDesc                  = pOpDesc;
}


/********************************************************************************************

>   BarControlInfo BarControl::GetBarControlInfo(BOOL horz)

	Author:     Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:    28/3/94
	Inputs:     horz - if TRUE  = the info on the horz version of this control is returned
					 - if FALSE = the info on the vert version of this control is returned
	Outputs:    -
	Returns:    A BarControlInfo class describing the horz/vert control completely
	Purpose:    Allows you to get hold of all the info you need about the required control
	Errors:     -
	SeeAlso:    -

********************************************************************************************/

BarControlInfo BarControl::GetBarControlInfo(BOOL horz)
{
	if (horz)
		return Horizontal.BarCtrlInfo;
	else
		return Vertical.BarCtrlInfo;
}

/********************************************************************************************

>	OpDescriptor* BarControl::GetOpDescriptor(BOOL horz)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/3/94
	Inputs:		horz - if TRUE	= the OpDescriptor on the horz version of this control is returned
				     - if FALSE	= the OpDescriptor on the vert version of this control is returned
	Outputs:	-
	Returns:	BarControl's OpDescriptor

	Purpose:	For finding the BarControl's opDescriptor
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

OpDescriptor* BarControl::GetOpDescriptor(BOOL horz)
{
	if (horz)
		return Horizontal.pOpDesc; 
	else
		return Vertical.pOpDesc;
}

/********************************************************************************************

>	UINT32 BarControl::GetBubbleID(BOOL horz)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/5/94
	Inputs:		horz - if TRUE	= the BubbleID on the horz version of this control is returned
				     - if FALSE	= the BubbleID on the vert version of this control is returned
	Outputs:	-
	Returns:	BarControl's BubbleID

	Purpose:	For finding the BarControl's BubbleID
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

UINT32 BarControl::GetBubbleID(BOOL horz)
{
	if (horz)
		return Horizontal.BubbleID; 
	else
		return Vertical.BubbleID;
}

/********************************************************************************************

>	UINT32 BarControl::GetStatusID(BOOL horz)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/5/94
	Inputs:		horz - if TRUE	= the StatusID on the horz version of this control is returned
				     - if FALSE	= the StatusID on the vert version of this control is returned
	Outputs:	-
	Returns:	BarControl's StatusID

	Purpose:	For finding the BarControl's StatusID
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

UINT32 BarControl::GetStatusID(BOOL horz)
{
	if (horz)
		return Horizontal.StatusID; 
	else
		return Vertical.StatusID;
}



/********************************************************************************************

>   BOOL BarControl::Write(String_256 * pString)

	Author:     Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:    20/2/97
	Outputs:	pString = string to put the data for this control into 
	Returns:    TRUE if successful, FALSE otherwise
	Purpose:    Writes the control to the given string
				This will be in the form of:-
					Control H "BreakShapes" 0 126 220 V "BreakShapes" 0 126 220
	Errors:     -
	SeeAlso:    -

********************************************************************************************/

#define QUOTE "\""

BOOL BarControl::Write(String_256 * pString)
{
	ERROR2IF(pString == NULL,FALSE,"Write given null string");

	BOOL ok = TRUE;
	TCHAR buf[10];

	// Add that its a bar control
	*pString += TokenTable[TOKEN_BAR_CONTROL].Token;
	*pString += _T(" ");

	// save out the horizontal state starting with the horizontal token
	*pString += TokenTable[TOKEN_BAR_CONTROL_HORZ].Token;
	*pString += _T(" ");

	if (Horizontal.pOpDesc)
	{
		*pString += wxT(QUOTE);
		*pString += Horizontal.pOpDesc->Token;
		*pString += wxT(QUOTE) wxT(" ");
	}
	else
	{
		camSnprintf(buf, 9, TEXT("%d "), Horizontal.BubbleID);
		*pString += buf;
		camSnprintf(buf, 9, TEXT("%d "), Horizontal.StatusID);
		*pString += buf;
	}
	camSnprintf(buf, 9, TEXT("%d "), Horizontal.BarCtrlInfo.ToolID);
	*pString += buf;
	camSnprintf(buf, 9, TEXT("%d "), Horizontal.BarCtrlInfo.ResourceID);
	*pString += buf;
	camSnprintf(buf, 9, TEXT("%d "), Horizontal.BarCtrlInfo.ControlID);
	*pString += buf;

	*pString += TokenTable[TOKEN_BAR_CONTROL_VERT].Token;
	*pString += _T(" ");
	if(Vertical.pOpDesc)
	{
		*pString += wxT(QUOTE);
		*pString += Vertical.pOpDesc->Token;
		*pString += wxT(QUOTE) wxT(" ");
	}
	else
	{
		camSnprintf(buf, 9, TEXT("%d "), Vertical.BubbleID);
		*pString += buf;
		camSnprintf(buf, 9, TEXT("%d "), Vertical.StatusID);
		*pString += buf;
	}
	camSnprintf(buf, 9, TEXT("%d "), Vertical.BarCtrlInfo.ToolID);
	*pString += buf;
	camSnprintf(buf, 9, TEXT("%d "), Vertical.BarCtrlInfo.ResourceID);
	*pString += buf;
	camSnprintf(buf, 9, TEXT("%d "), Vertical.BarCtrlInfo.ControlID);
	*pString += buf;

	return ok;
}

/********************************************************************************************

>   BOOL BarControl::Read(String_256 * pString)

	Author:     Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:    20/2/97
	Inputs:		pString = string to read the data for this control out of 
	Returns:    TRUE if successful, FALSE otherwise
	Purpose:    Reads the control from the given string
				This will be in the form of:-
					Control H "BreakShapes" 0 126 220 V "BreakShapes" 0 126 220
				
				Code ideas taken from PrintMarksCache::DecodeMarkFormat. Thanks Mike!
	Errors:     -
	SeeAlso:    PrintMarksCache::DecodeMarkFormat;

********************************************************************************************/

BOOL BarControl::Read(String_256 * pString)
{
	ERROR2IF(pString == NULL,FALSE,"BarControl::Read given null parameters!");

	// We will enter here having read in the Control section of the string so we just
	// need to decode the following sections.
	// This can consist of an H section and/or a V section.

	// This text must not be internationalised
	//const TCHAR SPACE	= TEXT(' ');
	const TCHAR H		= TEXT('H');
	const TCHAR V		= TEXT('V');
	const TCHAR B		= TEXT('B');

	INT32 pos = 0;

	// Everything is hunky dory at the moment! ER NOT!
	BOOL AllIsWell = FALSE;

	// Turn on the exceptions function in the string reading class
	StringBase::EnableExceptionHandler();

	String_256 Name(*pString);

	try 
	{
		// See if we have an H or B as the descriptor of the control
		// B defines both controls at the same time
		if (Name[pos] == H)
		{
			// See if there is an H command and then toast it and the supposed trailing space
			// if that is successful
			pos = Name.FindNextChar(H);
			Name.Remove(0, pos+2);

			AllIsWell = ReadSimpleBarControl(&Name, &Horizontal);

			if (AllIsWell)
			{
				// See if there is an V command and then toast it and the supposed trailing space
				// if that is successful
				pos = Name.FindNextChar(V);
				Name.Remove(0, pos+2);

				AllIsWell = ReadSimpleBarControl(&Name, &Vertical);
			}
		}
		else
		{
			// Read in both controls as one
			pos = Name.FindNextChar(B);
			Name.Remove(0, pos+2);

			AllIsWell = ReadSimpleBarControl(&Name, &Horizontal);

			// if horz and vert defs are the same, make vert = horz version.
			Vertical = Horizontal;
		}
	}
	catch( ... )
	{
		// An exception has been thrown which means
		// we failed to parse this string, so simply report failure
		AllIsWell = FALSE;
	}

	// Switch off the exception state finally, (and of course pop it)!
	StringBase::DisableExceptionHandler();

	return AllIsWell;
}

/********************************************************************************************

>   BOOL BarControl::ReadSimpleBarControl(String_256 *pString, SimpleBarControl* pSBC)

	Author:     Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:    20/2/97
	Inputs:		pString = string to read the data for this control out of 
				pSBC = ptr to a SimpleBarControl to fill using tokens from 'file'
	Outputs:    -
	Returns:    TRUE if successful, FALSE otherwise
	Purpose:    Reads a tokenized form of the simple bar control from the given string
	Errors:     -
	SeeAlso:    -

********************************************************************************************/

BOOL BarControl::ReadSimpleBarControl(String_256 *pString, SimpleBarControl* pSBC)
{
	ERROR2IF(pString == NULL || pSBC == NULL,FALSE,"ReadSimpleBarControl given null parameters!");

	String_256 OpName;
	INT32 pos = 0;
	// This text must not be internationalised
	const TCHAR QUOTES	= TEXT('"');

	// See if a string follows or if a bubble and status ids follow
	if ((*pString)[pos] == QUOTES)
	{
		// Go to the first quote of the first string
		pos = pString->FindNextChar(QUOTES, pos);
		pString->Remove(0, pos+1);

		// Now find the last quote of the first string
		pos = pString->FindNextChar(QUOTES);
		// Read the operation name into OpName
		pString->Left(&OpName, pos);
		pString->Remove(0, pos+1);
		pos = 0;

		// Try and find the OpDescriptor that should be in the system
		pSBC->pOpDesc  = OpDescriptor::FindOpDescriptor(OpName);
		pSBC->BubbleID = _R(IDBBL_NOOP);
		pSBC->StatusID = 0;
		if ((OpName.Length() > 0) && (pSBC->pOpDesc == NULL))
		{
			// If there is an op desc token string, but no op desc was found, fail
			TRACE( _T("Can't find op descriptor! OpToken of missing op = %s\n"),(TCHAR*)OpName);
			return FALSE;
		}
	}
	else
	{
		// Its as simple control
		// No op descriptor associated with this control
		pSBC->pOpDesc  = NULL;		

		// Read the bubble and status bar text IDs
		pSBC->BubbleID = pString->ConvertToInteger(pos);
		pSBC->StatusID = pString->ConvertToInteger(pos);
		pString->Remove(0,pos);
		pos = 0;
	}

	// The next three tokens are the tool ID, resource ID, and the control ID
	pSBC->BarCtrlInfo.ToolID = pString->ConvertToInteger(pos);
	pSBC->BarCtrlInfo.ResourceID = pString->ConvertToInteger(pos);
	pSBC->BarCtrlInfo.ControlID = pString->ConvertToInteger(pos);
	pString->Remove(0,pos);
	pos = 0;

	return TRUE;
}

/********************************************************************************************

>   BOOL BarControl::Write(CCLexFile& file)

	Author:     Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:    28/3/94
	Inputs:     file = a file object
	Outputs:    -
	Returns:    TRUE if successful, FALSE otherwise
	Purpose:    Writes a tokenized form of the control to the given file
	Errors:     -
	SeeAlso:    -

********************************************************************************************/

BOOL BarControl::Write(CCLexFile& file)
{
	BOOL ok = TRUE;

		if (ok) ok = file.PutToken(TokenTable[TOKEN_BAR_CONTROL].Token);

		if (ok) ok = file.PutToken(TokenTable[TOKEN_BAR_CONTROL_HORZ].Token);

		if(Horizontal.pOpDesc)
		{
			if (ok) ok = file.PutString(Horizontal.pOpDesc->Token);
		}
		else
		{
			if (ok) ok = file.PutToken(Horizontal.BubbleID);
			if (ok) ok = file.PutToken(Horizontal.StatusID);
		}
		if (ok) ok = file.PutToken(Horizontal.BarCtrlInfo.ToolID);
		if (ok) ok = file.PutToken(Horizontal.BarCtrlInfo.ResourceID);
		if (ok) ok = file.PutToken(Horizontal.BarCtrlInfo.ControlID);

		if (ok) ok = file.PutToken(TokenTable[TOKEN_BAR_CONTROL_VERT].Token);
		if(Vertical.pOpDesc)
		{
			if (ok) ok = file.PutString(Vertical.pOpDesc->Token);
		}
		else
		{
			if (ok) ok = file.PutToken(Vertical.BubbleID);
			if (ok) ok = file.PutToken(Vertical.StatusID);
		}
		if (ok) ok = file.PutToken(Vertical.BarCtrlInfo.ToolID);
		if (ok) ok = file.PutToken(Vertical.BarCtrlInfo.ResourceID);
		if (ok) ok = file.PutToken(Vertical.BarCtrlInfo.ControlID);
	return (ok);
}



/********************************************************************************************

>   BOOL BarControl::Read(CCLexFile& file)

	Author:     Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:    7/4/94
	Inputs:     file = a file object
	Outputs:    -
	Returns:    TRUE if successful, FALSE otherwise
	Purpose:    Reads a tokenized form of the control from the given file
	Errors:     -
	SeeAlso:    -

********************************************************************************************/


BOOL BarControl::Read(CCLexFile& file)
{
	TokenIndex Token = TOKEN_NONE;
	const TCHAR* TokenBuf = file.GetTokenBuf();
	BOOL ok;

	ok = file.GetSimpleToken();		// Read the first token
	if (ok)
	{
		Token = FindToken(TokenBuf);	
		// Check that next token defines either the horz or both versions of the control
		ok = ((Token == TOKEN_BAR_CONTROL_HORZ) || (Token == TOKEN_BAR_CONTROL_BOTH));
		if (!ok) TRACE( _T("Expected a '%s' or a '%s', but found '%s'."),TokenTable[TOKEN_BAR_CONTROL_HORZ].Token,TokenTable[TOKEN_BAR_CONTROL_BOTH].Token,TokenBuf);

		if (ok)	ok = ReadSimpleBarControl(file,&Horizontal);
	}

	if (ok && Token == TOKEN_BAR_CONTROL_BOTH)
		// if horz and vert defs are the same, make vert = horz version.
		Vertical = Horizontal;
	else if (ok)
	{
		// otherwise, read the vert definition
		ok = file.GetSimpleToken();
		if (ok)
		{
			Token = FindToken(TokenBuf);
			ok = (Token == TOKEN_BAR_CONTROL_VERT);
			if (!ok) TRACE( _T("Expected a '%s', but found '%s'."),TokenTable[TOKEN_BAR_CONTROL_VERT].Token,TokenBuf);

			if (ok) ok = ReadSimpleBarControl(file,&Vertical);
		}
	}

	if (!ok)
	{
		// If something when wrong, tell the debug dudes and give as much info as possible
		//ENSURE(FALSE,"Error reading a bar control. See TRACE output for details");
		TRACE( _T("\n\nError reading a bar control:\n"));
		TRACE( _T(" Token buffer = %s\n"),TokenBuf);
		TRACE( _T(" Line buffer  = %s\n"),file.GetLineBuf());
	}

	return (ok);
}

/********************************************************************************************

>   BOOL BarControl::ReadSimpleBarControl(CCLexFile& file,SimpleBarControl* pSBC)

	Author:     Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:    7/4/94
	Inputs:     file = a file object
				pSBC = ptr to a SimpleBarControl to fill using tokens from 'file'
	Outputs:    -
	Returns:    TRUE if successful, FALSE otherwise
	Purpose:    Reads a tokenized form of the simple bar control from the given file
	Errors:     -
	SeeAlso:    -

********************************************************************************************/

BOOL BarControl::ReadSimpleBarControl(CCLexFile& file,SimpleBarControl* pSBC)
{
	const TCHAR* TokenBuf = file.GetTokenBuf();
	BOOL ok;

	ok = file.GetSimpleToken();

	if (ok)
	{
		// The first token is either a string describing the op descriptor or
		// the ID for the bubble help message
		LexTokenType TokenType = file.GetTokenType();

		switch (TokenType)
		{
			case TOKEN_STRING :
				pSBC->pOpDesc  = OpDescriptor::FindOpDescriptor( const_cast<TCHAR*>(TokenBuf) );
				pSBC->BubbleID = _R(IDBBL_NOOP);
				pSBC->StatusID = 0;

				if ((TokenBuf[0] != 0) && (pSBC->pOpDesc == NULL))
				{
					// If there is an op desc token string, but no op desc was found, fail
					TRACE( _T("Can't find op descriptor! OpToken of missing op = %s\n"),TokenBuf);
					ok = FALSE;
				}

				break;

			case TOKEN_NORMAL :
				pSBC->pOpDesc  = NULL;		// No op descriptor associated with this control

				// Read the bubble and status bar text IDs
				ok = (camSscanf(TokenBuf, _T("%li"),&pSBC->BubbleID) == 1);
				if (!ok) TRACE( _T("Expected a bubble ID, but got '%s'\n"),TokenBuf);

				if (ok) ok = file.GetSimpleToken();
				if (ok)
				{
				 	ok = (camSscanf(TokenBuf,_T("%li"),&pSBC->StatusID) == 1);
					if (!ok) TRACE( _T("Expected a status ID, but got '%s'\n"),TokenBuf);
				}

				break;

			default :
				ok = FALSE;	// What kind of token is this? Get out of here!
				TRACE( _T("Don't recognise this token '%s'. Wrong type\n"),TokenBuf);
				break;
		}

		// The next three tokens are the tool ID, resource ID, and the control ID
		if (ok) ok = file.GetSimpleToken();
		if (ok)
		{
			// Interpret the next token as an INT32 (dec,hex, or oct)
			ok = (camSscanf(TokenBuf, _T("%li"),&pSBC->BarCtrlInfo.ToolID) == 1);
			if (!ok) TRACE( _T("Expected a tool ID, but got '%s'\n"),TokenBuf);

			if (ok) ok = file.GetSimpleToken();
			if (ok)
			{
				// Interpret the next token as an INT32 (dec,hex, or oct)
				ok = (camSscanf(TokenBuf, _T("%li"),&pSBC->BarCtrlInfo.ResourceID) == 1);
				if (!ok) TRACE( _T("Expected a resource ID, but got '%s'\n"),TokenBuf);

                if (ok) ok = file.GetSimpleToken();
				if (ok)
				{
					// Interpret the next token as an INT32 (dec,hex, or oct)
					ok = (camSscanf(TokenBuf,_T("%li"),&pSBC->BarCtrlInfo.ControlID) == 1);
					if (!ok) TRACE( _T("Expected a control ID, but got '%s'\n"),TokenBuf);
				}
			}
		}
	}

	return ok;
}


//--------
//--------
//--------

/********************************************************************************************

>   BOOL BarNewPage::Write(CCLexFile& file)

	Author:     Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:    28/3/94
	Inputs:     file = a file object
	Outputs:    -
	Returns:    TRUE if successful, FALSE otherwise
	Purpose:    Writes a tokenized form of the separator to the given file
	Errors:     -
	SeeAlso:    -

********************************************************************************************/

BOOL BarNewPage::Write(CCLexFile& file)
{
	return (file.PutToken(TokenTable[TOKEN_BAR_NEWPAGE].Token));
}

/********************************************************************************************

>   BOOL BarNewPage::Write(String_256 * pString)

	Author:     Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:    20/2/97
	Outputs:	pString = string to put the data for this control into 
	Returns:    TRUE if successful, FALSE otherwise
	Purpose:    Writes the control to the given string
	Errors:     -
	SeeAlso:    -

********************************************************************************************/

BOOL BarNewPage::Write(String_256 * pString)
{
	ERROR2IF(pString == NULL,FALSE,"Write given null string");
	*pString += TokenTable[TOKEN_BAR_NEWPAGE].Token;
	return TRUE;
}

/********************************************************************************************

>   BOOL BarSeparator::Write(CCLexFile& file)

	Author:     Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:    28/3/94
	Inputs:     file = a file object
	Outputs:    -
	Returns:    TRUE if successful, FALSE otherwise
	Purpose:    Writes a tokenized form of the separator to the given file
	Errors:     -
	SeeAlso:    -

********************************************************************************************/

BOOL BarSeparator::Write(CCLexFile& file)
{
	return (file.PutToken(TokenTable[TOKEN_BAR_SEPARATOR].Token));
}

/********************************************************************************************

>   BOOL BarSeparator::Write(String_256 * pString)

	Author:     Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:    20/2/97
	Outputs:	pString = string to put the data for this control into 
	Returns:    TRUE if successful, FALSE otherwise
	Purpose:    Writes the control to the given string
	Errors:     -
	SeeAlso:    -

********************************************************************************************/

BOOL BarSeparator::Write(String_256 * pString)
{
	ERROR2IF(pString == NULL,FALSE,"Write given null string");
	*pString += TokenTable[TOKEN_BAR_SEPARATOR].Token;
	return TRUE;
}

/********************************************************************************************

>   BOOL DDeckerTop::Write(CCLexFile& file)

	Author:     Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:    28/3/94
	Inputs:     file = a file object
	Outputs:    -
	Returns:    TRUE if successful, FALSE otherwise
	Purpose:    Writes a tokenized form of a DDeckerTop to the given file
	Errors:     -
	SeeAlso:    -

********************************************************************************************/

BOOL DDeckerTop::Write(CCLexFile& file)
{
	return (file.PutToken(TokenTable[TOKEN_DDECKER_TOP].Token));
}

/********************************************************************************************

>   BOOL DDeckerTop::Write(String_256 * pString)

	Author:     Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:    20/2/97
	Outputs:	pString = string to put the data for this control into 
	Returns:    TRUE if successful, FALSE otherwise
	Purpose:    Writes the control to the given string
	Errors:     -
	SeeAlso:    -

********************************************************************************************/

BOOL DDeckerTop::Write(String_256 * pString)
{
	ERROR2IF(pString == NULL,FALSE,"Write given null string");
	*pString += TokenTable[TOKEN_DDECKER_TOP].Token;
	return TRUE;
}

/********************************************************************************************

>   BOOL DDeckerBottom::Write(CCLexFile& file)

	Author:     Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:    28/3/94
	Inputs:     file = a file object
	Outputs:    -
	Returns:    TRUE if successful, FALSE otherwise
	Purpose:    Writes a tokenized form of a DDeckerBottom to the given file
	Errors:     -
	SeeAlso:    -

********************************************************************************************/

BOOL DDeckerBottom::Write(CCLexFile& file)
{
	return (file.PutToken(TokenTable[TOKEN_DDECKER_BOTTOM].Token));
}

/********************************************************************************************

>   BOOL DDeckerBottom::Write(String_256 * pString)

	Author:     Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:    20/2/97
	Outputs:	pString = string to put the data for this control into 
	Returns:    TRUE if successful, FALSE otherwise
	Purpose:    Writes the control to the given string
	Errors:     -
	SeeAlso:    -

********************************************************************************************/

BOOL DDeckerBottom::Write(String_256 * pString)
{
	ERROR2IF(pString == NULL,FALSE,"Write given null string");
	*pString += TokenTable[TOKEN_DDECKER_BOTTOM].Token;
	return TRUE;
}

/********************************************************************************************

>   BOOL DDeckerEnd::Write(CCLexFile& file)

	Author:     Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:    28/3/94
	Inputs:     file = a file object
	Outputs:    -
	Returns:    TRUE if successful, FALSE otherwise
	Purpose:    Writes a tokenized form of a DDeckerEnd to the given file
	Errors:     -
	SeeAlso:    -

********************************************************************************************/

BOOL DDeckerEnd::Write(CCLexFile& file)
{
	return (file.PutToken(TokenTable[TOKEN_DDECKER_END].Token));
}

/********************************************************************************************

>   BOOL DDeckerEnd::Write(String_256 * pString)

	Author:     Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:    20/2/97
	Outputs:	pString = string to put the data for this control into 
	Returns:    TRUE if successful, FALSE otherwise
	Purpose:    Writes the control to the given string
	Errors:     -
	SeeAlso:    -

********************************************************************************************/

BOOL DDeckerEnd::Write(String_256 * pString)
{
	ERROR2IF(pString == NULL,FALSE,"Write given null string");
	*pString += TokenTable[TOKEN_DDECKER_END].Token;
	return TRUE;
}

//--------
//--------
//--------

/********************************************************************************************

>   BOOL BarLineFeed::Write(CCLexFile& file)

	Author:     Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:    28/3/94
	Inputs:     file = a file object
	Outputs:    -
	Returns:    TRUE if successful, FALSE otherwise
	Purpose:    Writes a tokenized form of the linefeed to the given file
	Errors:     -
	SeeAlso:    -

********************************************************************************************/

BOOL BarLineFeed::Write(CCLexFile& file)
{
	return (file.PutToken(TokenTable[TOKEN_BAR_LINEFEED].Token));
}

/********************************************************************************************

>   BOOL DDeckerEnd::Write(String_256 * pString)

	Author:     Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:    20/2/97
	Outputs:	pString = string to put the data for this control into 
	Returns:    TRUE if successful, FALSE otherwise
	Purpose:    Writes the control to the given string
	Errors:     -
	SeeAlso:    -

********************************************************************************************/

BOOL BarLineFeed::Write(String_256 * pString)
{
	ERROR2IF(pString == NULL,FALSE,"Write given null string");
	*pString += TokenTable[TOKEN_BAR_LINEFEED].Token;
	return TRUE;
}

//--------
//--------
//--------


/********************************************************************************************

>   BarToolButton::BarToolButton()

	Author:     Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:    25/5/94
	Inputs:     -
	Outputs:    -
	Returns:    -
	Purpose:    Inits BarToolButton member vars
	Errors:     -
	SeeAlso:    -

********************************************************************************************/

BarToolButton::BarToolButton()
{
	ToolID 			= 0;
	Bitmap 			= 0;
	BitmapActive 	= 0;
    BubbleID 		= 0;
	StatusID 		= 0;
}
	

/********************************************************************************************

>   UINT32 BarToolButton::GetBitmap(BitmapType Type)

	Author:     Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:    10/5/94
	Inputs:     Type = the type of bitmap you want (Active or Inactive)
	Outputs:    -
	Returns:    The Bitmap ID of the desired bitmap
	Purpose:    Lets you get hold of the bitmaps within a BarToolButton.
	Errors:     -
	SeeAlso:    -

********************************************************************************************/

UINT32 BarToolButton::GetBitmap(BitmapType Type)
{ 
	if (Type == Active) 
		return BitmapActive; 
	else
		return Bitmap;
}

/********************************************************************************************

>   void BarToolButton::SetBitmap(BitmapType Type, UINT32 ThisBitmap)

	Author:     Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:    10/5/94
	Inputs:     Type       = the type of bitmap you want to set (Active or Inactive)
				ThisBitmap = the bitmap ID you want to set
	Outputs:    -
	Returns:    -
	Purpose:    Lets you set the bitmap IDs within a BarToolButton.
	Errors:     -
	SeeAlso:    -

********************************************************************************************/

void BarToolButton::SetBitmap(BitmapType Type, UINT32 ThisBitmap)
{ 
	switch (Type)
	{
		case Active   : BitmapActive = ThisBitmap; break;
		case Inactive : Bitmap 		 = ThisBitmap; break;
	}
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------


/********************************************************************************************

>   DockingBar::DockingBar()

	Author:     Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:    5/4/94
	Inputs:     -
	Outputs:    -
	Returns:    -
	Purpose:    The constructor
	Errors:     -
	SeeAlso:    -
				  
********************************************************************************************/

/*DockingBar::DockingBar()
{
	DockingBarList.AddTail(this);
}
*/

/********************************************************************************************

>   DockingBar::~DockingBar()

	Author:     Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:    5/4/94
	Inputs:     -
	Outputs:    -
	Returns:    -
	Purpose:    The destructor
	Errors:     -
	SeeAlso:    -

********************************************************************************************/

/*DockingBar::~DockingBar()
{
	DockingBarList.RemoveItem(this);
}
*/
/********************************************************************************************

>   BOOL DockingBar::HasBigControls()

	Author:     Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:    5/4/94
	Inputs:     -
	Outputs:    -
	Returns:    -
	Purpose:    Find out the size of controls used by all bars docked with this docking bar
	Errors:     -
	SeeAlso:    -

********************************************************************************************/

/*BOOL DockingBar::HasBigControls()
{
	return DockingBarFlags.Big;
}
*/
/********************************************************************************************

>   void DockingBar::SetBigControlsState(BOOL Big)

	Author:     Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:    5/4/94
	Inputs:     Big - if TRUE,  controls should be big in this docking bar.
					- if FALSE, they should be small in this docking bar
	Outputs:    -
	Returns:    -
	Purpose:    Sets the size of controls used by all bars docked with this docking bar
	Errors:     -
	SeeAlso:    -

********************************************************************************************/

/*void DockingBar::SetBigControlsState(BOOL Big)
{
	DockingBarFlags.Big = Big;
}
*/
/********************************************************************************************

>   BOOL DockingBar::Write(CCLexFile& file)

	Author:     Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:    5/4/94
	Inputs:     file = a file object
	Outputs:    -
	Returns:    TRUE if successful, FALSE otherwise
	Purpose:    Writes a tokenized form of the docking bar to the given file
	Errors:     -
	SeeAlso:    -

********************************************************************************************/

/*BOOL DockingBar::Write(CCLexFile& file)
{
	String_256 str;
	BOOL ok;

	ok = file.PutToken(TOKEN_DOCKING_BAR);

	switch (Dock)
	{
		case DOCKBAR_TOP    : str = TOKEN_DOCKBAR_TOP;    break;
		case DOCKBAR_BOTTOM : str = TOKEN_DOCKBAR_BOTTOM; break;
		case DOCKBAR_LEFT   : str = TOKEN_DOCKBAR_LEFT;   break;
		case DOCKBAR_RIGHT  : str = TOKEN_DOCKBAR_RIGHT;  break;
		case DOCKBAR_FLOAT  : str = TOKEN_DOCKBAR_FLOAT;  break;
		default             : ok  = FALSE;                break;
	}

	if (ok) ok = file.PutToken(str);

	if (ok)
	{
		if (DockingBarFlags.Big)
			ok = file.PutToken(TOKEN_BIG);
		else
			ok = file.PutToken(TOKEN_SMALL);
	}

	return (ok);
}
*/
/********************************************************************************************

>   static BOOL DockingBar::WriteDockingBars(CCLexFile& file)

	Author:     Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:    6/4/94
	Inputs:     file = a file object
	Outputs:    -
	Returns:    TRUE if successful, FALSE otherwise
	Purpose:    Writes all the docking bars in tokenized form to the given file
	Errors:     -
	SeeAlso:    -

********************************************************************************************/

/*BOOL DockingBar::WriteDockingBars(CCLexFile& file)
{
	BOOL ok;

	ok = file.InitLexer();

	DockingBar* pDockingBar = (DockingBar*)DockingBarList.GetHead();

	while (pDockingBar != NULL && ok)
	{
		ok = pDockingBar->Write(file);
		if (ok) ok = file.PutNewLine();
		pDockingBar = (DockingBar*)DockingBarList.GetNext(pDockingBar);
	}

	file.DeinitLexer();

	return (ok);
}
*/

//-----------------------------------------------------------------
//-----------------------------------------------------------------
//-----------------------------------------------------------------

/********************************************************************************************

>   BOOL DialogBarOp::WriteBarsToRegistry(HKEY hSubKey)

	Author:     Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:    19/2/97
	Inputs:     hSubKey		the parent key to use in the registry
	Outputs:    -
	Returns:    -
	Purpose:    Asks each relevent kernel bar representation to write its definition out.
				Each bar is written into a numbered section which is a sub-section of the
				supplied key. The numbering is 1..n. Each bar should write out some standard
				named sections and then write out any bar items as separate numbered lines
				into their section. The helper class SaveRegistryEntries can be used to handle
				this automatically.
	Errors:     -
	SeeAlso:    SaveRegistryEntries;

********************************************************************************************/

BOOL DialogBarOp::WriteBarsToRegistry(HKEY hSubKey)
{
#pragma message( __LOCMSG__ "DialogBarOp::WriteBarsToRegistry - do nothing" )
	TRACE( _T("Warning - DialogBarOp::WriteBarsToRegistry called") );
/*	ERROR2IF(hSubKey == NULL,FALSE,"WriteBarsToRegistry bad registry key!");

	BOOL ok = TRUE;

	//if (ok) ok = DialogBarOp::WriteDockingBarsToRegistry();
	//if (ok) ok = DialogBarOp::WriteWorkSpaceInfoToFile();

	// save out a version number of the bars, the current build number
	// the hSubKey should be opened and ready for writing
	ok = ok && SetRegDword(hSubKey, TEXT("VersionMajor"), CAMELOT_VERSION_MAJOR);
	ok = ok && SetRegDword(hSubKey, TEXT("VersionMinor"), CAMELOT_VERSION_MINOR);

	ok = ok && InformationBarOp::Write(hSubKey);

	// Go through all the bars and if they are the correct type then ask them to write
	// themselves out to the registry.
	List* pList = MessageHandler::GetClassList(CC_RUNTIME_CLASS(DialogBarOp));
	
	// Get the first bar in the list
	DialogBarOp* pDialogBarOp = (DialogBarOp*)pList->GetHead();
	// Loop through all bars
	INT32 i = 0;
	TCHAR buf[10];
	while (pDialogBarOp != NULL && ok)
	{
		// Use the same method as before, nasty but should ensure we do everything as intended
		if (!pDialogBarOp->IsKindOf(CC_RUNTIME_CLASS(SystemBarOp)) &&
			!pDialogBarOp->IsKindOf(CC_RUNTIME_CLASS(InformationBarOp)))
		{
			// we don't want to write out the control bank or any list items that are blank
			if (!pDialogBarOp->IsListEmpty() && !pDialogBarOp->IsAllBarsOp())
			{
				// write out the data for that bar using the counter number as the key name
				wsprintf(buf, TEXT("%d"), i + 1);
				String_256 name(buf);
				SaveRegistryEntries Saver;
				Saver.StartSaving(&name, hSubKey);
				
				ok = pDialogBarOp->Write(Saver);

				Saver.StopSaving();

				// Only increment our count after we have written a valid bar definition section
				i++;
			}
		}

		// Get the next bar in the list
		pDialogBarOp = (DialogBarOp*)pList->GetNext(pDialogBarOp);
	}

	return (ok); */

	return true;
}

/********************************************************************************************

>   BOOL DialogBarOp::ReadBarsFromRegistry(HKEY hSubKey)

	Author:     Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:    19/2/97
	Inputs:     hSubKey		the parent key to use in the registry
	Outputs:    -
	Returns:    -
	Purpose:    Reads the bar definitions from the specified section in the registry.
				Each bar is written into a numbered section which is a sub-section of the
				supplied key. The numbering is 1..n. So we need to search for these numbered
				sections until we fail to find one.
	Errors:     -
	SeeAlso:    LoadRegistryEntries;

********************************************************************************************/

BOOL DialogBarOp::ReadBarsFromRegistry(HKEY hSubKey)
{
#pragma message( __LOCMSG__ "DialogBarOp::ReadBarsFromRegistry - do nothing" )
	TRACE( _T("Warning - DialogBarOp::ReadBarsFromRegistry called") );
/*	ERROR2IF(hSubKey == NULL,FALSE,"ReadBarsFromRegistry bad registry key!");

	BOOL ok = TRUE;
	INT32 NumBarsRead = 0;

	ok = ok && InformationBarOp::Read(hSubKey);

	// Now go through all the numbered bar entries and read in all the ones that exist
	// These will contain all the gallery and tool bar definitions
	DialogBarOpCreate BarCreate;
	BOOL Finished = FALSE;
	INT32 i = 0;
	TCHAR buf[10];
	while (!Finished && ok)
	{ 
		// Make up the bar number for this index
		wsprintf(buf, TEXT("%d"), i + 1);
		String_256 name(buf);
		// check to see if the registry key exists
		LoadRegistryEntries Loader;
		if (Loader.StartLoading(&name, hSubKey))
		{
			// The key exists and so we must have a bars or a super gallery definition 
			HKEY hRegKey = Loader.GetRegKey();
			ERROR2IF(hRegKey == NULL, FALSE,"ReadBarsFromRegistry Failed to get reg key");

			// This is the dialog bar op that we are going to create
			DialogBarOp* pDialogBarOp = NULL;

			// Get the type of bar we are dealing with
			String_256 BarType;
			ok = GetRegString(hRegKey, TEXT("Type"), &BarType);
			if (BarType == String_256(TokenTable[TOKEN_BAR].Token))
			{
				// Create DialogBarOp or derived class using BarCreate object
				pDialogBarOp = BarCreate.Create();
			}
			else if (BarType == String_256(TokenTable[TOKEN_GALLERY].Token))
			{
				// Its a super gallery so get the class name that was saved as well
				String_256 ClassName;
				ok = ok && GetRegString(hRegKey, TEXT("ClassName"), &ClassName);
				// Use this to create the new class
				CCRuntimeClass * pCCRuntimeClass = CCObject::GetRuntimeClassByName( ClassName );
				if (pCCRuntimeClass != NULL)
				{
					// Go and create the object of the correct runtime class
					pDialogBarOp = (DialogBarOp *) pCCRuntimeClass->CreateObject();
					// Check it's a real gallery. We ensure on debug builds, but just ignore the
					// entry in retail builds.
					//
					// This should only ever happen if ccobject.cpp is broken or someone's put an
					// incorrectly derived, or non DYNA_CREATE class in BARS.INI
					//
					ok = (pDialogBarOp->IsKindOf(CC_RUNTIME_CLASS(SuperGallery)));
				}
				else
					TRACE( _T("Registry definition contains unknown gallery class : '%s'\n"),(TCHAR *)ClassName);
			}
			else
			{
				ERROR3("Unknown bar type in DialogBarOp::Read");
				return FALSE;
			}

			if (pDialogBarOp != NULL)
			{
				// Read definition and inc num bars read
				// ask the bar to extract all the details from this section
				ok = ok && pDialogBarOp->Read(Loader);
				
				if (ok)
					NumBarsRead++;
				else
					pDialogBarOp->End();
			}
			else
				ok = FALSE;

			// stop the loader class, only required if the key was opened
			Loader.StopLoading();
		}
		else
		{
			// The key does not exist and so we must have reached the end of the list
			Finished = TRUE;
		}

		// increment out count to step onto the next bars definition
		i++;
	}

	return (ok); */

	return false;
}

/********************************************************************************************

>   static BOOL DialogBarOp::WriteDockingBarsToRegistry(HKEY hSubKey)

	Author:     Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:    19/2/97
	Inputs:		hSubKey		the registery section to save the data into
	Outputs:    -
	Returns:    -
	Purpose:    Writes the persistant docking data retrieved from the docking system.
	Errors:     -
	SeeAlso:    -

********************************************************************************************/

BOOL DialogBarOp::WriteDockingBarsToRegistry(HKEY hSubKey)
{
#pragma message( __LOCMSG__ "DialogBarOp::WriteDockingBarsToRegistry - do nothing" )
	TRACE( _T("Warning - DialogBarOp::WriteDockingBarsToRegistry called") );
/*	ERROR2IF(hSubKey == NULL,FALSE,"WriteDockingBarsToRegistry bad registry key!");

	BOOL				ok = TRUE;
	CCamFrame		   *pMainFrame = GetMainFrame();

	if (pMainFrame == NULL)
		return FALSE;

	// Save it into the registry
	String_256 SectionName;
	// First open/create a new sub-key under the app-state key for holding the data.
	HKEY hRegKey = CreateRegKey(hSubKey, PRODUCT_REGISTRYKEY_BARSETTINGS);
	if (hRegKey == NULL)
	{
		ERROR3("DialogBarOp::WriteWorkSpaceInfoToRegistry failed to get registry key");
		return FALSE;
	}

	for (INT32 i = 0; i < NUM_DOCKBARS && ok; i++)
	{
		OILDockingBar* pDockBar = pMainFrame->GetDockBar((DockBarType)i);
		if (ok) ok = (pDockBar != NULL);

		// save out the details on that bar's status i.e. big or small
		ok = ok && SetRegBool(hRegKey, TokenTable[TOKEN_DOCKBAR_LEFT+i].Token, pDockBar->IsBigControls());
	}

	// Close down the key
	CloseRegKey(hRegKey);

	return (ok); */

	return false;
}

/********************************************************************************************

>   static BOOL DialogBarOp::ReadDockingBarsFromRegistry(HKEY hSubKey)

	Author:     Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:    19/2/97
	Inputs:		hSubKey		the registery section to read the data from
	Outputs:    -
	Returns:    -
	Purpose:    Reads in the persistant docking data retrieved from the docking system.
	Errors:     -
	SeeAlso:    -

********************************************************************************************/

BOOL DialogBarOp::ReadDockingBarsFromRegistry(HKEY hSubKey)
{
#pragma message( __LOCMSG__ "DialogBarOp::ReadDockingBarsFromRegistry - do nothing" )
	TRACE( _T("Warning - DialogBarOp::ReadDockingBarsFromRegistry called") );
/*	ERROR2IF(hSubKey == NULL,FALSE,"ReadDockingBarsFromRegistry bad registry key!");

	BOOL ok = TRUE;
	CCamFrame* pMainFrame = GetMainFrame();

	if (pMainFrame == NULL)
		return FALSE;

	// Save it into the registry
	// First open/create a new sub-key under the app-state key for holding the data.
	HKEY hRegKey = CreateRegKey(hSubKey, PRODUCT_REGISTRYKEY_BARSETTINGS);
	if (hRegKey == NULL)
	{
		//ERROR3("DialogBarOp::ReadDockingBarsFromRegistry failed to get registry key");
		return FALSE;
	}

	DockBarType DockType;
	for (INT32 i = 0; i < NUM_DOCKBARS && ok; i++)
	{
		OILDockingBar* pDockBar = pMainFrame->GetDockBar((DockBarType)i);
		if (ok) ok = (pDockBar != NULL);

		switch (i)
		{
			case 0:	DockType = DOCKBAR_LEFT;		break;
			case 1:	DockType = DOCKBAR_RIGHT;		break;
			case 2:	DockType = DOCKBAR_TOP;			break;
			case 3:	DockType = DOCKBAR_BOTTOM;		break;
			case 4:	DockType = DOCKBAR_FLOAT;		break;
			default:
				TRACEUSER( "Neville", _T("ReadDockingBarsFromRegistry bad dockbar type"));
				ok = FALSE;
				break;
		}
		
		if (ok)
		{
			// get the details on that bar's status i.e. big or small
			BOOL State = GetRegBool(hRegKey, TokenTable[TOKEN_DOCKBAR_LEFT+i].Token);

			OILDockingBar* pDockBar = pMainFrame->GetDockBar(DockType);
			// Set the dockbar control size - default to small
			pDockBar->SetBigControlState(State);
		}
	}

	// Close down the key
	CloseRegKey(hRegKey);

	return (ok); */

	return false;
}

/********************************************************************************************

>   static BOOL DialogBarOp::WriteWorkSpaceInfoToRegistry(HKEY hSubKey)

	Author:     Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:    19/2/97
	Inputs:		hSubKey		the registery section to save the data into
	Outputs:    -
	Returns:    -
	Purpose:    Writes workspace data to registry ( Full Screen Mode etc..) 
	Errors:     -
	SeeAlso:    -

********************************************************************************************/

BOOL DialogBarOp::WriteWorkSpaceInfoToRegistry(HKEY hSubKey)
{
#pragma message( __LOCMSG__ "DialogBarOp::WriteWorkSpaceInfoToRegistry - do nothing" )
	TRACE( _T("Warning - DialogBarOp::WriteWorkSpaceInfoToRegistry called") );
/*	ERROR2IF(hSubKey == NULL,FALSE,"WriteWorkSpaceInfoToRegistry bad registry key!");

	BOOL ok = TRUE;
	CCamFrame* pMainFrame = GetMainFrame();

	if (pMainFrame == NULL)
		return FALSE;
	
	// Save it into the registry
	// First open/create a new sub-key under the app-state key for holding the data.
	HKEY hRegKey = CreateRegKey(hSubKey, PRODUCT_REGISTRYKEY_BARSETTINGS);
	if (hRegKey == NULL)
	{
		ERROR3("DialogBarOp::WriteWorkSpaceInfoToRegistry failed to get registry key");
		return FALSE;
	}

	// Save the value
	ok = ok && SetRegBool(hRegKey, TokenTable[TOKEN_FULLSCREEN].Token, pMainFrame->IsFullScreenMode());
	ok = ok && SetRegBool(hRegKey, TokenTable[TOKEN_STATUS_BAR].Token, pMainFrame->IsStatusBarVisible());
	ok = ok && SetRegBool(hRegKey, TokenTable[TOKEN_COLOUR_BAR].Token, pMainFrame->IsColourBarVisible());
	ok = ok && SetRegBool(hRegKey, TokenTable[TOKEN_SCROLL_BARS].Token, pMainFrame->AreScrollBarsVisible());

	// Close down the key
	CloseRegKey(hRegKey);

	return (ok); */

	return false;
}

/********************************************************************************************

>   static BOOL DialogBarOp::ReadWorkSpaceInfoFromRegistry()

	Author:     Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:    19/2/97
	Inputs:		hSubKey		the registery section to read the data from
	Outputs:    -
	Returns:    -
	Purpose:    Reads workspace data from the registry ( Full Screen Mode etc..) 
	Errors:     -
	SeeAlso:    -

********************************************************************************************/

BOOL DialogBarOp::ReadWorkSpaceInfoFromRegistry(HKEY hSubKey)
{
#pragma message( __LOCMSG__ "DialogBarOp::ReadWorkSpaceInfoFromRegistry - do nothing" )
	TRACE( _T("Warning - DialogBarOp::ReadWorkSpaceInfoFromRegistry called") );
/*	ERROR2IF(hSubKey == NULL,FALSE,"ReadWorkSpaceInfoFromRegistry bad registry key!");

	BOOL ok = TRUE;
	CCamFrame* pMainFrame = GetMainFrame();

	if (pMainFrame == NULL)
		return FALSE;
	
	// Save it into the registry
	// First open/create a new sub-key under the app-state key for holding the data.
	HKEY hRegKey = OpenRegKey(hSubKey, PRODUCT_REGISTRYKEY_BARSETTINGS);
	if (hRegKey == NULL)
	{
		//ERROR3("DialogBarOp::ReadWorkSpaceInfoFromRegistry failed to get registry key");
		return FALSE;
	}

	// Save the value
	BOOL Show = TRUE;
	Show = GetRegBool(hRegKey, TokenTable[TOKEN_FULLSCREEN].Token);
	pMainFrame->DoFullScreenMode(Show);
	
	Show = GetRegBool(hRegKey, TokenTable[TOKEN_STATUS_BAR].Token);
	pMainFrame->ShowStatusBar(Show);

	Show = GetRegBool(hRegKey, TokenTable[TOKEN_COLOUR_BAR].Token);
	pMainFrame->ShowColourBar(Show);

	Show = GetRegBool(hRegKey, TokenTable[TOKEN_SCROLL_BARS].Token);
	pMainFrame->ShowScrollBars(Show);

	// Close down the key
	CloseRegKey(hRegKey);

	return (ok); */

	return false;
}

/********************************************************************************************
// Old ini file related saving code
********************************************************************************************/

/********************************************************************************************

>   static BOOL DialogBarOp::WriteBarsToFile(CCLexFile& file)

	Author:     Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:    6/4/94
	Inputs:     file = file to write bar tokens to
	Outputs:    -
	Returns:    -
	Purpose:    Writes the entire kernel's bar representation to the given file.
				It is written in an ASCII tokenized form so that CCLexFile can be used
				to read the file back in when reconstructing the bars system.
	Errors:     -
	SeeAlso:    -

********************************************************************************************/

BOOL DialogBarOp::WriteBarsToFile(CCLexFile& file)
{
	BOOL ok = TRUE;

PORTNOTE("other","WriteBarsToFile - Removed")
#if !defined(EXCLUDE_FROM_XARALX)

	ok = file.InitLexer();

	if (ok) ok = DialogBarOp::WriteDockingBarsToFile(file);
	if (ok) ok = DialogBarOp::WriteWorkSpaceInfoToFile(file);
	if (ok) ok = InformationBarOp::WriteStatic(file);
	if (ok) ok = DialogBarOp::WriteBars(file);

	file.DeinitLexer();
#endif

	return (ok);
}

/********************************************************************************************

>   static BOOL DialogBarOp::WriteDockingBarsToFile(CCLexFile& file)

	Author:     Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:    7/4/94
	Inputs:     file = file to write persistant docking data to
	Outputs:    -
	Returns:    -
	Purpose:    Writes the persistant docking data retrieved from the docking system.
	Errors:     -
	SeeAlso:    -

********************************************************************************************/

BOOL DialogBarOp::WriteDockingBarsToFile(CCLexFile& file)
{
#pragma message( __LOCMSG__ "DialogBarOp::WriteDockingBarsToFile - do nothing" )
	TRACE( _T("Warning - DialogBarOp::WriteDockingBarsToFile called") );
/*	BOOL ok = TRUE;
	CCamFrame* pMainFrame = GetMainFrame();

	ok = (pMainFrame != NULL);

	for (INT32 i=0;i<NUM_DOCKBARS && ok;i++)
	{
		OILDockingBar* pDockBar = pMainFrame->GetDockBar((DockBarType)i);
		if (ok) ok = (pDockBar != NULL);

		if (ok) ok = file.PutToken(TokenTable[TOKEN_DOCKING_BAR].Token);
		if (ok) ok = file.PutToken(TokenTable[TOKEN_DOCKBAR_LEFT+i].Token);
		if (ok)
		{
			if (pDockBar->IsBigControls())
				ok = file.PutToken(TokenTable[TOKEN_BIG].Token);
			else
				ok = file.PutToken(TokenTable[TOKEN_SMALL].Token);
		}
		if (ok) ok = file.PutNewLine();
	}

	if (ok) ok = file.PutNewLine();

	return (ok); */

	return false;
}

/********************************************************************************************

>   static BOOL DialogBarOp::WriteWorkSpaceInfoToFile(CCLexFile& file)

	Author:     Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:    11/10/94
	Inputs:     file = file to write persistant docking data to
	Outputs:    -
	Returns:    -
	Purpose:    Writes workspace data to ini file ( Full Screen Mode etc..) 
	Errors:     -
	SeeAlso:    -

********************************************************************************************/

BOOL DialogBarOp::WriteWorkSpaceInfoToFile(CCLexFile& file)
{
#pragma message( __LOCMSG__ "DialogBarOp::WriteWorkSpaceInfoToFile - do nothing" )
	TRACE( _T("Warning - DialogBarOp::WriteWorkSpaceInfoToFile called") );
/*	BOOL ok = TRUE;
	CCamFrame* pMainFrame = GetMainFrame();

	ok = (pMainFrame != NULL);
	
	if (ok) ok = file.PutToken(TokenTable[TOKEN_FULLSCREEN].Token);
	if(pMainFrame->IsFullScreenMode())
	{
		if (ok) ok = file.PutToken(TokenTable[TOKEN_ON].Token);
	}
	else
	{
		if (ok) ok = file.PutToken(TokenTable[TOKEN_OFF].Token);
	}
	if (ok) ok = file.PutNewLine();

	if (ok) ok = file.PutToken(TokenTable[TOKEN_STATUS_BAR].Token);
	
	if(pMainFrame->IsStatusBarVisible())
	{
		if (ok) ok = file.PutToken(TokenTable[TOKEN_VISIBLE].Token);
	}
	else
	{
		if (ok) ok = file.PutToken(TokenTable[TOKEN_INVISIBLE].Token);
	}
	if (ok) ok = file.PutNewLine();

	if (ok) ok = file.PutToken(TokenTable[TOKEN_COLOUR_BAR].Token);
	if(pMainFrame->IsColourBarVisible())
	{
		if (ok) ok = file.PutToken(TokenTable[TOKEN_VISIBLE].Token);
	}
	else
	{
		if (ok) ok = file.PutToken(TokenTable[TOKEN_INVISIBLE].Token);
	}
	
	if (ok) ok = file.PutNewLine();

	if (ok) ok = file.PutToken(TokenTable[TOKEN_SCROLL_BARS].Token);
	if(pMainFrame->AreScrollBarsVisible())
	{
		if (ok) ok = file.PutToken(TokenTable[TOKEN_VISIBLE].Token);
	}
	else
	{
		if (ok) ok = file.PutToken(TokenTable[TOKEN_INVISIBLE].Token);
	}
	
	if (ok) ok = file.PutNewLine();
	if (ok) ok = file.PutNewLine();

	return (ok); */

	return false;
}

/********************************************************************************************

>   static BOOL DialogBarOp::FlushUpToToken(CCLexFile& file,TokenIndex Token)

	Author:     Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:    19/4/95
	Inputs:     file = file to read bar tokens from
				Token= token that we want to read up to.
	Outputs:    -
	Returns:    TRUE  = Able to read upto token OR end of file
				FALSE = Failed to read the bar info without error. The error cases are -
						1. File error (e.g. file open, disk fault, etc)

	Purpose:    This reads the file until it finds the token given, or it finds the end of file.
				This is used to skip over bar definitions that ReadBarsFromFile() knows it can ignore,
				e.g. old versions of bars, extinct class names, etc
	Errors:     -
	SeeAlso:    DialogBarOp::ReadBarsFromFile()

********************************************************************************************/

BOOL DialogBarOp::FlushUpToToken(CCLexFile& file,TokenIndex Token)
{
	BOOL finished = FALSE,ok = TRUE;

	// Get ptr to token buffer. Guaranteed not to move until next
	// InitLexer() call
	const TCHAR* TokenBuf = file.GetTokenBuf();

	while (!finished && ok)
	{
		ok = file.GetSimpleToken();

		if (ok)
			finished = (file.GetTokenType() == TOKEN_EOF) || (FindToken(TokenBuf) == Token);
	}

	return ok;
}

/********************************************************************************************

>   static BOOL DialogBarOp::ReadBarsFromFile(	CCLexFile& file,
												BarCreate& BarCreate,
												UINT32* pNumBarsRead = NULL,
												UINT32* pNumDockingBarsRead = NULL)

	Author:     Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:    6/4/94
	Inputs:     file 				= file to read bar tokens from
				BarCreate			= reference to class for creation of object to read
				pNumBarsRead        = where to put num bars, galleries read on exit
				pNumDockingBarsRead = where to put num dock bars read on exit
	Outputs:    Num bars read is stored in *pNumBarsRead if != NULL
				Num dock bars read is stored in *pNumDockingBarsRead if != NULL
	Returns:    TRUE  = At least one bar and all docking bars have been read from
						the file without error.
				FALSE = Failed to read the bar info without error. The error cases are -
						1. If a syntax error occurs during reading of file
						2. File error (e.g. file open, disk fault, etc)

	Purpose:    Reads the bars represented in the given file.
				It reads the file in an ASCII tokenized form using CCLexFile::GetSimpleToken(),
				and reconstructs the bars defined.

				This also reads galleries from your favorite bars.ini file (or equivalent).
				They are (of course) created without reference to your BarCreate function as the
				create function is cunningly encoded within the gallery definition itself.

	Errors:     -
	SeeAlso:    -

********************************************************************************************/

BOOL DialogBarOp::ReadBarsFromFile(	CCLexFile& file,
									BarCreate& BarCreate,
									UINT32* pNumBarsRead,
									UINT32* pNumDockingBarsRead)
{
#pragma message( __LOCMSG__ "DialogBarOp::ReadBarsFromFile - do nothing" )
	TRACE( _T("Warning - DialogBarOp::ReadBarsFromFile called") );
/*	BOOL ok  = TRUE;                    // Indicates a serious error if FALSE
	BOOL eof = FALSE;                   // TRUE if end of file is found
	UINT32 NumBarsRead = 0;				// Count of number of bars read
	UINT32 NumDockingBarsRead = 0;		// Count of number of docking bars read
	
	ok = file.InitLexer();				// Initialise lexing routines

	file.SetDelimiters("\r\n");			// Set token delimiting characters
	file.SetCommentMarker(';');			// Set comment marker char
	file.SetWhitespace(" \t");			// Set whitespace chars
	file.SetStringDelimiters("\"\"");	// Set string delimiters

	// Get ptr to token buffer. Guaranteed not to move until next
	// InitLexer() call
	const TCHAR* TokenBuf = file.GetTokenBuf();

	while (ok && !eof)
	{
		// Get keyword token and token type
		BOOL main_token_ok     = file.GetSimpleToken();
		LexTokenType TokenType = file.GetTokenType();

		if (!main_token_ok)
		{
			// If no main token, check for EOF
			// If EOF then everything's ok
			eof = (TokenType == TOKEN_EOF);
			ok  = eof;
		}           

		if (main_token_ok)
		{
			// Get token index from our list of tokens
			TokenIndex Token = FindToken(TokenBuf);

			switch (Token)
			{
				case TOKEN_BAR :
				{
					// Create DialogBarOp or derived class using BarCreate object
					DialogBarOp* pDialogBarOp = BarCreate.Create();

					// Read definition and inc num bars read
					ok = (pDialogBarOp != NULL);
					if (ok) ok = pDialogBarOp->Read(file);
					if (ok) NumBarsRead++;
					if (!ok && (pDialogBarOp != NULL)) pDialogBarOp->End();
				}
				break;
				case TOKEN_INFO_BAR :
				{
					// Create DialogBarOp or derived class using BarCreate object
										// Read definition and inc num bars read
					 ok = InformationBarOp::Read(file);
				}
				break;

				case TOKEN_GALLERY :
				{
					// New type by Alex; the only difference is where we get the type from
					// (it's encoded in ASCII in BARS.INI)
					CCRuntimeClass *pCCRuntimeClass;

					ok = file.GetSimpleToken();
					// Get a pointer to the corresponding runtime class object.
					if (ok)
					{
						ok = ((pCCRuntimeClass = CCObject::GetRuntimeClassByName( TokenBuf )) != NULL);
						if (!ok)
						{
							TRACE( _T("Resource file contains unknown gallery class : '%s'\n"),TokenBuf);
//							ok = FlushUpToToken(file,TOKEN_GALLERY_END);
//							break;
						}
					}

					if (ok) 
					{
						// Go and create the object of the correct runtime class
						DialogBarOp* pDialogBarOp = (DialogBarOp *) pCCRuntimeClass->CreateObject();

						// Read definition and inc num bars read
						ok = (pDialogBarOp != NULL);
						if (!ok) TRACE( _T("Unable to create an object called '%s'\n"),TokenBuf);

						// Check it's a real gallery. We ensure on debug builds, but just ignore the
						// entry in retail builds.
						//
						// This should only ever happen if ccobject.cpp is broken or someone's put an
						// incorrectly derived, or non DYNA_CREATE class in BARS.INI
						//
						if (ok) 
						{
							ok = (pDialogBarOp->IsKindOf(CC_RUNTIME_CLASS(SuperGallery)));
#ifdef _DEBUG
							if (!ok) TRACE( _T("'%s' is not defined as an instance of a SuperGallery"));
#endif
						}

						// Note that the read function is likely to be overridden so this can read
						// gellery specific parameters
						if (ok) ok = pDialogBarOp->Read(file);
						if (ok) NumBarsRead++;
						if (!ok && (pDialogBarOp != NULL)) pDialogBarOp->End();
					} 
				}
				break;
				
				case TOKEN_STATUS_BAR :
				{
					ok = file.GetSimpleToken();
					if (ok)
					{
						BOOL Show;
						Token = FindToken(TokenBuf);
						switch(Token)
						{
							case TOKEN_VISIBLE  :	Show = TRUE;		break;
							case TOKEN_INVISIBLE:	Show = FALSE;		break; 
				 		 }
						 GetMainFrame()->ShowStatusBar(Show);						
					}
				 

				}
				break;
				case TOKEN_COLOUR_BAR :
				{
					ok = file.GetSimpleToken();
					if (ok)
					{
						BOOL Show;
						Token = FindToken(TokenBuf);
						switch(Token)
						{
							case TOKEN_VISIBLE  :	Show = TRUE;		break;
							case TOKEN_INVISIBLE:	Show = FALSE;		break; 
				 		 }
						 GetMainFrame()->ShowColourBar(Show);						
					}
				 

				}
				break;
				
				case TOKEN_SCROLL_BARS :
				{
					ok = file.GetSimpleToken();
					if (ok)
					{
						BOOL Show;
						Token = FindToken(TokenBuf);
						switch(Token)
						{
							case TOKEN_VISIBLE  :	Show = TRUE;		break;
							case TOKEN_INVISIBLE:	Show = FALSE;		break; 
				 		 }
						 GetMainFrame()->ShowScrollBars(Show);						
					}
				 

				}
				break;
				
				case TOKEN_FULLSCREEN :
				{
					ok = file.GetSimpleToken();
					if (ok)
					{
						BOOL Mode;
						Token = FindToken(TokenBuf);
						switch(Token)
						{
							case TOKEN_ON  :	Mode = TRUE;		break;
							case TOKEN_OFF:		Mode = FALSE;		break; 
				 		 }
						 GetMainFrame()->DoFullScreenMode(Mode);						
					}
				}
				break;
				
				case TOKEN_DOCKING_BAR :
				{
					DockBarType DockType;

					// Which bar are we refering to ??
					ok = file.GetSimpleToken();
					if (ok)
					{
						Token = FindToken(TokenBuf);
						switch(Token)
						{
							case TOKEN_DOCKBAR_LEFT:	DockType=DOCKBAR_LEFT;		break;
							case TOKEN_DOCKBAR_RIGHT:	DockType=DOCKBAR_RIGHT;		break;
						 	case TOKEN_DOCKBAR_TOP:		DockType=DOCKBAR_TOP;		break;
						 	case TOKEN_DOCKBAR_BOTTOM:	DockType=DOCKBAR_BOTTOM;	break;
						 	case TOKEN_DOCKBAR_FLOAT:	DockType=DOCKBAR_FLOAT;		break;

							default:
								TRACE( _T("No dockbar type in bars.ini"));
								ok = FALSE;
								break;
						}
						if (ok)	ok = file.GetSimpleToken();
						// Check for Small/Big controls
						if (ok)
						{
							Token = FindToken(TokenBuf);
				
							OILDockingBar* pDockBar = GetMainFrame()->GetDockBar(DockType);
							// Set the dockbar control size - default to small
							pDockBar->SetBigControlState(Token == TOKEN_BIG);

							// Inc num docking bars read
							NumDockingBarsRead++;
						}
					}
				}
				break;
			}
		}
	}

	if (!ok)
	{
		// If something when wrong, tell the debug dudes and give as much info as possible
		//ENSURE(FALSE,"Error reading a bar control. See TRACE output for details");
		TRACE( _T("\n\nError reading bars from a file"));
		TRACE( _T(" Token buffer = %s\n"),TokenBuf);
		TRACE( _T(" Line buffer  = %s\n"),file.GetLineBuf());
	}


	// We are now finished with the lexer
	file.DeinitLexer();

	// Relay num bars and docking bars back to caller if they want the info
	if (pNumBarsRead        != NULL) *pNumBarsRead        = NumBarsRead;
	if (pNumDockingBarsRead != NULL) *pNumDockingBarsRead = NumDockingBarsRead;

	return (ok); */

	return false;
}



/********************************************************************************************

>	BOOL DialogBarOp::AllowMulSelInListBox()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the list box associated with the dialog bar op should allow multiple
				selection or not
	Purpose:	Linked heavilly with the BaseBar class and the creation of list boxes associated
				with the dialog bar op of a gallery.
				This gives the derived dialog bar op the opportunity to have a say to whether the list
				box should allow multiple selection or not.
				This base version always returns TRUE
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL DialogBarOp::AllowMulSelInListBox()
{
	return TRUE;
}

 /********************************************************************************************

>	BOOL DialogBarOp::IsAllBarsOp()

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/5/95
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if this is the special bar that contains all controls
	Purpose:	-
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL DialogBarOp::IsAllBarsOp()
{
	
	return this == AllBarsOp;
		
	//return Name == String_32(_R(IDS_CONTROLBANKNAME));

}
/****************************************************************************************
static BOOL DialogBarOp::AddOpToAll( OpDescriptor* pHorzOpDesc, 
									 OpDescriptor* pVertOpDesc = NULL)

	Author:     Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:    16/5/95
	Inputs:     Pointer to the Horz OpDescriptor to connect to a bar
				Pointer to the Vert OpDescriptor to connect to a bar (should be NULL
				  if Vert is same as Horz op desc)
	Outputs:    -
	Returns:    TRUE if added OK.
				FALSE otherwise.
	Purpose:    DEAD.
	Errors:     -
	SeeAlso:    -

******************************************************************************************/

BOOL DialogBarOp::AddOpToAll(OpDescriptor* pHorzOpDesc,OpDescriptor* pVertOpDesc)
{
	return FALSE;
}




//-----------------------------------------------------------------
//-----------------------------------------------------------------
//-----------------------------------------------------------------
//-----------------------------------------------------------------
// SystemBarOp methods


/********************************************************************************************

>   SystemBarOp::SystemBarOp()

	Author:     Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:    26/4/94
	Inputs:     -
	Outputs:    -
	Returns:    -
	Purpose:    Dummy constructor for DYNCREATE
	Errors:     -
	SeeAlso:    -

********************************************************************************************/

SystemBarOp::SystemBarOp()
{

}

/********************************************************************************************

>   SystemBarOp::SystemBarOp(UINT32 BarNameID, SystemBarType GrpBarID)

	Author:     Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:    26/4/94
	Inputs:     BarNameID = ID of string used as the name of the bar (appears in
							toolbars dialogue)
				GrpBarID  = The group which this system belongs (e.g. File,	Edit, etc)
	Outputs:    -
	Returns:    -
	Purpose:    Constructor
	Errors:     -
	SeeAlso:    -

********************************************************************************************/

SystemBarOp::SystemBarOp(	UINT32 BarNameID, 
							SystemBarType GrpBarID,
							DockBarType DockType,
							UINT32 Slot,
							INT32 Offset) : 
							DialogBarOp(BarNameID, CC_RUNTIME_CLASS(DialogBarOp) )
{
	GroupBarID = GrpBarID;

	SetDockBarType(DockType);
	SetSlot(Slot);
	SetOffset(Offset);
}

/********************************************************************************************

>   BOOL SystemBarOp::Write(CCLexFile& file)

	Author:     Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:    15/4/94
	Inputs:     file = a file to Write the dialog bar to.
	Outputs:    -
	Returns:    FALSE always because system bars can't be serialised
	Purpose:    Trap calls to write out system bars
	Errors:     -
	SeeAlso:    -

********************************************************************************************/

BOOL SystemBarOp::Write(CCLexFile& file)
{
	return (FALSE);
}


/********************************************************************************************

>   BOOL SystemBarOp::Read(CCLexFile& file)

	Author:     Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:    15/4/94
	Inputs:     file = a file to read the dialog bar from.
	Outputs:    -
	Returns:    FALSE always because this routine should never be called!
	Purpose:    Trap call to read bars in the system bar class
	Errors:     -
	SeeAlso:    -

********************************************************************************************/

BOOL SystemBarOp::Read(CCLexFile& file)
{
	return (FALSE);
}

/********************************************************************************************

>   BOOL SystemBarOp::Write(SaveRegistryEntries& Saver)

	Author:     Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:    20/2/97
	Inputs:     Saver = the class to use to write out the registry data.
	Outputs:    -
	Returns:    FALSE always because system bars can't be serialised
	Purpose:    Trap calls to write out system bars
	Errors:     -
	SeeAlso:    -

********************************************************************************************/

BOOL SystemBarOp::Write(SaveRegistryEntries& Saver)
{
	return (FALSE);
}


/********************************************************************************************

>   BOOL SystemBarOp::Read(LoadRegistryEntries& Loader)

	Author:     Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:    20/2/97
	Inputs:     Loader = the class to use to load in the registry data.
	Outputs:    -
	Returns:    FALSE always because this routine should never be called!
	Purpose:    Trap call to read bars in the system bar class
	Errors:     -
	SeeAlso:    -

********************************************************************************************/

BOOL SystemBarOp::Read(LoadRegistryEntries& Loader)
{
	return (FALSE);
}



/******************************************************************************************

>   static SystemBarOp* SystemBarOp::FindType(SystemBarType SystemBarID)

	Author:     Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:    17/4/94
	Inputs:     Type of system bar to find
	Outputs:    -
	Returns:    Pointer to the system bar of the specified type.
				NULL is returned if system bar is not found.
	Purpose:    To find one of the permanently created system bars which are used to group
				OpDescriptors so that they can be presented to the user in a sensible way.
				This routine searches the list of DialogBarOps created by the message 
				manager to distribute messages to DialgBarOps.
	Errors:     -
	SeeAlso:    -

******************************************************************************************/

SystemBarOp* SystemBarOp::FindType(SystemBarType SystemBarID)
{
	// Search the list of DialogBarOps for the specified system bar...
	List* pBarList = MessageHandler::GetClassList(CC_RUNTIME_CLASS(DialogBarOp));
	ENSURE(pBarList,"Can't find list of DialogBarOps in SystemBarOp::FindType");

	DialogBarOp* pBar = (DialogBarOp*)pBarList->GetHead();      // Get ptr to head of list
	while (pBar)                                                // While there are BarOps
	{
		if (pBar->IsKindOf(CC_RUNTIME_CLASS(SystemBarOp)))      // test class
		{
			if (((SystemBarOp*)pBar)->GroupBarID == SystemBarID)// test type
			{
				return ( (SystemBarOp*)pBar );                  // If they match then return
			}                                                   // a ptr to this BarOp
		}
		pBar = (DialogBarOp*)pBarList->GetNext(pBar);           // else skip to next bar
	}                                                           // and continue...

	return (NULL);                                              // If no match was found
}




/******************************************************************************************

>   static BOOL SystemBarOp::Connect(SystemBarType SystemBarID,
									 OpDescriptor* pHorzOpDesc, 
									 OpDescriptor* pVertOpDesc = NULL)

	Author:     Phil_Martin (Xara Group Ltd) <camelotdev@xara.com> (& MarkN)
	Created:    17/4/94
	Inputs:     Type of system bar to find and connect OpDescriptor to.
				Pointer to the Horz OpDescriptor to connect to a bar
				Pointer to the Vert OpDescriptor to connect to a bar (should be NULL
				  if Vert is same as Horz op desc)
	Outputs:    -
	Returns:    TRUE if connection succeeded.
				FALSE otherwise.
	Purpose:    To connect an OpDescriptor to the specified system bar.
	Errors:     -
	SeeAlso:    -

******************************************************************************************/

BOOL SystemBarOp::Connect(SystemBarType SystemBarID,OpDescriptor* pHorzOpDesc,OpDescriptor* pVertOpDesc)
{
	BOOL ConnectedOK = FALSE;                           // Setup return value

	SystemBarOp* pSystemBar = SystemBarOp::FindType(SystemBarID);   // Find the bar specified by the ID
	if (pSystemBar)
		ConnectedOK = SystemBarOp::Connect(pSystemBar,pHorzOpDesc,pVertOpDesc);

	return (ConnectedOK);                               // If no match was found
}




/******************************************************************************************

>   static BOOL SystemBarOp::Connect(SystemBarOp*,  OpDescriptor* pHorzOpDesc, 
													OpDescriptor* pVertOpDesc = NULL)

	Author:     Phil_Martin (Xara Group Ltd) <camelotdev@xara.com> (& MarkN)
	Created:    17/4/94
	Inputs:     Pointer to system bar to connect OpDescriptor to.
				Pointer to the Horz OpDescriptor to connect to a bar
				Pointer to the Vert OpDescriptor to connect to a bar (should be NULL
				  if Vert is same as Horz op desc)
	Outputs:    -
	Returns:    TRUE if connection succeeded.
				FALSE otherwise.
	Purpose:    To connect an OpDescriptor to the specified system bar.
	Errors:     -
	SeeAlso:    -

******************************************************************************************/

BOOL SystemBarOp::Connect(SystemBarOp* pSystemBar,OpDescriptor* pHorzOpDesc,OpDescriptor* pVertOpDesc)
{
	BOOL ConnectedOK = FALSE;                           // Setup return value

	if (pSystemBar)
	{
		BarControl* pBarCtrl = new BarControl();
		if (pBarCtrl)
		{
			// Set horz op desc
			pBarCtrl->SetHorzOpDesc(pHorzOpDesc);

			// Set vert op desc depending on pVertOpDesc
			if (pVertOpDesc != NULL)
				pBarCtrl->SetVertOpDesc(pVertOpDesc);
			else
				pBarCtrl->SetVertOpDesc(pHorzOpDesc);

			// Add the BarControl to to the system bar 
			pSystemBar->AddBarItem(pBarCtrl);

			ConnectedOK = TRUE;
		}
	}

	return (ConnectedOK);                               // If no match was found
}

/******************************************************************************************

>   static BOOL SystemBarOp::Separate(SystemBarType SystemBarID)

	Author:     Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:    19/4/94
	Inputs:     Type of system bar to find and connect OpDescriptor to.
	Outputs:    -
	Returns:    TRUE if connection succeeded.
				FALSE otherwise.
	Purpose:    To connect a separator to the specified system bar.
	Errors:     -
	SeeAlso:    -

******************************************************************************************/

BOOL SystemBarOp::Separate(SystemBarType SystemBarID)
{
	BOOL ConnectedOK = FALSE;                           // Setup return value

	SystemBarOp* pSystemBar = SystemBarOp::FindType(SystemBarID);   // Find the bar specified by the ID
	if (pSystemBar)
		ConnectedOK = SystemBarOp::Separate(pSystemBar);

	return (ConnectedOK);                               // If no match was found
}




/******************************************************************************************

>   static BOOL SystemBarOp::Separate(SystemBarOp* pSystemBar)

	Author:     Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:    19/4/94
	Inputs:     Pointer to system bar to connect OpDescriptor to.
	Outputs:    -
	Returns:    TRUE if connection succeeded.
				FALSE otherwise.
	Purpose:    To connect a separator to the specified system bar.
	Errors:     -
	SeeAlso:    -

******************************************************************************************/

BOOL SystemBarOp::Separate(SystemBarOp* pSystemBar)
{
	BOOL ConnectedOK = FALSE;                           // Setup return value

	if (pSystemBar)
	{
		BarSeparator* pBarSep = new BarSeparator();
		if (pBarSep)
		{
			// Add the BarControl to to the system bar 
			pSystemBar->AddBarItem(pBarSep);

			ConnectedOK = TRUE;
		}
	}

	return (ConnectedOK);                               // If no match was found
}

/********************************************************************************************

>	static BOOL SystemBarOp::Init()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/4/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Registers SystemBarOp OpDescriptor
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL SystemBarOp::Init()
{
	
	BOOL ok;
	ok = 	(RegisterOpDescriptor(
								0, 
								_R(IDS_MARKN_EMPTY),
								CC_RUNTIME_CLASS(SystemBarOp), 
								OPTOKEN_DLGBAROP,
								NULL // No GetState
	
								)
	/*
	   		&&
	  		new SystemBarOp( _R(IDS_FILE_BAR), SYSTEMBAR_FILE )
	  		&&
	  		new SystemBarOp( _R(IDS_EDIT_BAR), SYSTEMBAR_EDIT )
	  		&&
	  		new SystemBarOp( _R(IDS_ATTRIBUTE_BAR), SYSTEMBAR_ATTRIBUTE )
	  		&&
	  		new SystemBarOp( _R(IDS_ARRANGE_BAR), SYSTEMBAR_ARRANGE )
	  		&&
	  		new SystemBarOp( _R(IDS_UTILS_BAR), SYSTEMBAR_UTILITIES )
	  		&&
	  		new SystemBarOp( _R(IDS_WINDOW_BAR), SYSTEMBAR_WINDOW )
	  		&&
	  		new SystemBarOp( _R(IDS_HELP_BAR), SYSTEMBAR_HELP )
	*/
		 	); 

	return ok; 
}

/********************************************************************************************

>	MsgResult SystemBarOp::Message(Msg* Msg)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/5/94
	Inputs:		Msg: The message to handle
	Outputs:	-				   
	Returns:	-
	Purpose:	The default SystemBarOp Message handler
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

MsgResult SystemBarOp::Message(Msg* Msg)
{
	// We must destroy the dialog if the application is dying
//	if (MESSAGE_IS_A(Msg,DeathMsg))
//		End();							// Camelot is ending, so end the Op

//  	return OK;
	return (DialogBarOp::Message(Msg));
}


/********************************************************************************************/
// InformationBarOp methods 

/********************************************************************************************

>	InformationBarOp::InformationBarOp(CCRuntimeClass* Class = CC_RUNTIME_CLASS(DialogBarOp)): DialogBarOp(Class) 
												 
	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/4/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	InformationBarOp constructor (Identical to DialogOp)
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

InformationBarOp::InformationBarOp(CCRuntimeClass* Class): DialogBarOp(Class) 
{
}

/********************************************************************************************

>	InformationBarOp::InformationBarOp(String_32& NewName,CCRuntimeClass* Class = CC_RUNTIME_CLASS(DialogBarOp): 
	DialogBarOp(NewName, Class) 

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/4/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	InformationBarOp constructor (Identical to DialogOp)
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

InformationBarOp::InformationBarOp(String_32& NewName,CCRuntimeClass* Class): 
	DialogBarOp(NewName, Class) 
{
}


/********************************************************************************************

>	virtual MsgResult InformationBarOp::Message(Msg* Message)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/4/94
	Inputs:		Message
	Outputs:	-
	Returns:	-
	Purpose:	The InformationBars message handler is used to handle class wide events,
				such as broadcasting a message when an infobar is closed, etc
	Errors:		-
	SeeAlso:	DialogOp::Message

********************************************************************************************/

MsgResult InformationBarOp::Message(Msg* Message)
{
//	if (IsUserName("MarkN")) TRACE( _T("InformationBarOp::Message()\n"));

	if (IS_OUR_DIALOG_MSG(Message))
	{
		DialogMsg* Msg = (DialogMsg*)Message;

		// Check if the message is a CANCEL
		if (Msg->DlgMsg == DIM_CANCEL)
		{
			
			//if (IsUserName("MarkN")) TRACE( _T("InformationBarOp::Message(DIM_CANCEL)\n"));
			
			InformationBarOp::Visible = FALSE;
	
			// Report bar is closing to interested parties
			BROADCAST_TO_CLASS(BarMsg(BAR_CLOSE,this),DialogOp);
		}
		else if (Msg->DlgMsg == DIM_CREATE)
		{
		}

		return (DLG_EAT_IF_HUNGRY(Msg)); 
	}
	else if (MESSAGE_IS_A(Message,DeathMsg)) 	// We must destroy the dialog if the application is dying
	{
		End();
		return OK;
	}

	// Not a message for our dialog
	return OK; 
}

/********************************************************************************************

>	virtual void HandleProfileButtonClick (CBiasGainGadget& Gadget, CGadgetID ProfileGadget)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/1/2000
	Inputs:		CBiasGainGadget& and its GadgetID (for added safety)
	Outputs:	-
	Returns:	-
	Purpose:	Handles the opening/closing of the profile gadgets corrensponding profile
				dialog when clicked.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
void InformationBarOp::HandleProfileButtonClick (CBiasGainGadget& Gadget, CGadgetID ProfileGadgetID)
{
	if (!(Gadget.IsDialogOpen ()))
	{
		CBiasGainDlg* pDialog_m  =  new CBiasGainDlg();
		BOOL bMany = FALSE, bAllSameType = TRUE;
								
		CProfileBiasGain* appliedBiasGain = GetProfileFromSelection (ProfileGadgetID, &bMany, &bAllSameType);

		if (bAllSameType != FALSE)
		{
			pDialog_m->InvokeVia (Gadget, appliedBiasGain, bMany);
		}
	}
	else
	{
		Gadget.CloseDialog ();
	}
}


/********************************************************************************************

>	virtual void HandleProfileSelChangingMsg (CBiasGainGadget& Gadget, CGadgetID ProfileGadgetID)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/1/2000
	Inputs:		CBiasGainGadget& and its GadgetID (for added safety)
	Outputs:	-
	Returns:	-
	Purpose:	Handles the updating/closing of the profile gadgets corrensponding profile
				dialog when a SelChangingMsg is received.  This function does one of three
				things:
				
				1)	If an object with a profile is selected, and we shift-select another such
					object; then if the profiles are identical, updates the profile dialog to
					reflect the common profile.
				2)	If an object with a profile is selected, and we shift-select another such
					object; then if the profiles are NOT identical, updates the profile dialog
					to reflect the MANY profiles option.
				3)	If an object with a profile is selected, and we shift-select another
					DIFFERENT object (e.g.  we had a blend object selected, and shift-select a
					trivial quickshape), closes down the profile dialog since the second
					object does NOT contain a profile (thereby making it an undesirable to try
					and apply a (common) profile to it).

				These three things are also held when we are dealing with more than two objects.

				The ability to detect these three cases is integrated within
				GetProfileFromSelection () - which is called internally.

				This function also posses the ability to close the profile dialog under the
				following circumstances:

				1)  Selecting no objects.
				2)  Selecting a single object (of a different type to the previous).

				WARNING:  this function behaves funny when trying to debug it!
					
	Errors:		-
	SeeAlso:	InformationBarOp::GetProfileFromSelection ()

********************************************************************************************/

void InformationBarOp::HandleProfileSelChangingMsg (CBiasGainGadget& Gadget, CGadgetID ProfileGadgetID)
{
	BOOL bMany = FALSE, bAllSameType = TRUE;
	CProfileBiasGain* appliedBiasGain = GetProfileFromSelection (ProfileGadgetID, &bMany, &bAllSameType);

	if (Gadget.IsDialogOpen ())
	{
		if ((appliedBiasGain != NULL) || (bMany == TRUE))
		{
			if (bAllSameType == TRUE)
			{
				// handles case where we click on another blend
				Gadget.ReInitialiseDialog (appliedBiasGain, bMany);
			}
			else
			{
				// handles case where we click on a non-blend object
				Gadget.CloseDialog ();
				EnableGadget (ProfileGadgetID, FALSE);
			}
		}
		else
		{	
			// CGS:  wo there!  when selecting another object, everything is deselected - and then just that node
			// selected.  This is a problem, since if we proceed regardless, the profile dialog closes (when it
			// possibly shouldn't have).  As such, I now check for this occurring ....
			
			if (bAllSameType)
			{
				if (!SelectorTool::GetNormalClick_CheckProfileDialog ())
				{
					// user click on no object at all - close the profile dialog ....
					Gadget.CloseDialog ();
					EnableGadget (ProfileGadgetID, FALSE);
				}
			}
			else
			{
				// handles case where we click on a non-same type object
				Gadget.CloseDialog ();
				EnableGadget (ProfileGadgetID, FALSE);
			}
		}
	}
	else if (bAllSameType == TRUE)
	{
//		EnableGadget (ProfileGadgetID, TRUE);
	}
	else if (bAllSameType == FALSE)
	{
		EnableGadget (ProfileGadgetID, FALSE);
	}
}


/********************************************************************************************

>	virtual CProfileBiasGain* GetProfileFromSelection (CGadgetID ProfileGadgetID, BOOL* bMany, BOOL* bAllSameType)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/1/2000
	Inputs:		The GadgetID of the CBiasGainGadget that we are dealing with.
	Outputs:	bMany - returned as TRUE if we have MANY profiles selected.
				bAllSameType - returned as TRUE if objects within selection are all of the
				same type.
	Returns:	Ptr to common CProfileBiasGain, or NULL if there is NOT one.
	Purpose:	Is responsible for determining which of conditions 1-3 apply as detailed
				within HandleProfileSelChangingMsg ().  The (correct) implementation of this
				function is NOT trivial!!!!  This function MUST be overidden in every
				derived InformationBarOp class that encorperates profiles since this base
				class one ONLY returns NULL.

				Now listen carefully, since I will now tell you the MUST HAVE requirements
				for implementing an overide of this function.

				1)	First things first, you MUST scan (in a loop) each object within the 
					current selection.
				2)	The first object scanned MUST have its profile stored (as a basis for
					comparison).
				3)	The first objects profile MUST be compared against each other objects
					profile; AND if found to be different for ANY one, *bMany = TRUE.  If
					the loop terminates successfully (i.e.  not executing *bMany = TRUE);
					then *bMany = FALSE.  This requirement handles conditions 1-2.
				4)	In parallel with 3), the function MUST also check to if the scanned
					object is of the same type as the first; AND if they are different sets
					*bAllSameType = FALSE.  Otherwise, upon successful loop termination,
					*bAllSameType = TRUE.  This requirement handles condition 3.

				Requirement 4, is non-trivial due to the fashion in which NodeShadow (and
				like) exist within the Camelot tree.  Take a look at tools\blndtool.cpp
				for a robust implementation of this function (which I also wrote).

				The implementation of the function should also give some thought to the ID
				of the gadget - so that you may distingush between the profile value to read
				on infobars that have multiple profile gadgets.  Again take a look at
				tools\blndtool.cpp for an example of this.

				NOTE:  IF YOU DO NOT DO AS I HAVE OUTLAID ABOVE, THEN YOUR PROFILE GAGDET 
				WILL NOT WORK AS DESIRED, AND TO BE HONEST I AM GETTING TIRED OF SEEING
				CODE THAT DOES NOT FOLLOW ANY STANDARD APPROACH FOR COMMON FEATURES WITHIN
				INFOBARS, WHICH I AM FORCING PEOPLE TO USE THESE FUNCTIONS WITHIN INFOBARS.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

CProfileBiasGain* InformationBarOp::GetProfileFromSelection (CGadgetID ProfileGadgetID, BOOL* bMany, BOOL* bAllSameType)
{
	return (NULL);
}





/********************************************************************************************

>	void CloseProfileDialog (CBiasGainGadget& Gadget)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/1/2000
	Inputs:		a CBiasGainGadget&
	Outputs:	-
	Returns:	-
	Purpose:	Handles the closing of the profile gadgets corrensponding profile
				dialog.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void InformationBarOp::CloseProfileDialog (CBiasGainGadget& Gadget)
{
	if (Gadget.IsDialogOpen ())
	{
		Gadget.CloseDialog ();
	}
}

/********************************************************************************************

>	BOOL InformationBarOp::Create()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/4/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	The InformationBarOps Create method
				This method has been overridden to de-specialize the DialogBarOp. 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/


BOOL InformationBarOp::Create()
{
	if (Tool::IsToolChanging()||GetMainFrame()->IsChangingViewMode())
	{
		SetDockBarType(InformationBarOp::Dock);
		SetSlot(InformationBarOp::Slot);
		SetOffset(InformationBarOp::Offset);
		SetFloatingCPoint(InformationBarOp::FloatPos);
	}

	InformationBarOp::CurrentInfoBarOp = this;

	if (InformationBarOp::Visible)
		return (DialogOp::Create());
	else
		return TRUE;
}

/********************************************************************************************

>	BOOL InformationBarOp::SetInfo()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com> (using Simon code)
	Created:	28/4/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Stores info bar data  
	Errors:		-
	SeeAlso:	InformationBarOp::Create(),DialogBarOp::Delete()

********************************************************************************************/


void InformationBarOp::SetInfo()
{
	
	SetDockBarType(InformationBarOp::Dock);
	SetSlot(InformationBarOp::Slot);
	SetOffset(InformationBarOp::Offset);
	SetFloatingCPoint(InformationBarOp::FloatPos);
 }

 /********************************************************************************************

>	BOOL InformationBarOp::GetInfo()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com> (using Simon code)
	Created:	28/4/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Stores info bar data in Static variables
	Errors:		-
	SeeAlso:	InformationBarOp::Create(),DialogBarOp::Delete()

********************************************************************************************/


void InformationBarOp::GetInfo()
{
#pragma message( __LOCMSG__ "InformationBarOp::GetInfo - do nothing" )
	TRACE( _T("Warning - InformationBarOp::GetInfo called") );
/*	KernelBarPos* 	pKernelBarPos;
	DockBarType 	Dock;
	
	if (WindowID != NULL)
	{
		if (GetMainFrame()->GetBarPosInfo(WindowID,&Dock,&pKernelBarPos))
		{
			SetDockBarType(Dock);
			if (Dock == DOCKBAR_FLOAT)
				SetFloatingCPoint(wxPoint(pKernelBarPos->x,pKernelBarPos->y));
			else
			{
				SetSlot(pKernelBarPos->Slot);
				SetOffset(pKernelBarPos->Position);
			}
		}

		// Save this info bar's data in the static vars
		// These vars are used by
		InformationBarOp::Dock 			= GetDockBarType();
		InformationBarOp::Slot 			= GetSlot();
		InformationBarOp::Offset 		= GetOffset();
		InformationBarOp::FloatPos 		= GetFloatingCPoint();
	} */
 }


/********************************************************************************************

>	BOOL InformationBarOp::Delete()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com> (using Simon code)
	Created:	28/4/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	The InformationBarOps Delete method
				Stores info bar data and calls DialogBarOp::Delete()
	Errors:		-
	SeeAlso:	InformationBarOp::Create(),DialogBarOp::Delete()

********************************************************************************************/


void InformationBarOp::Delete()
{
	if (WindowID != NULL)
	{

		PORTNOTETRACE("dialog", "Warning - InformationBarOp::Delete called");
#ifndef EXCLUDE_FROM_XARALX
		KernelBarPos* 	pKernelBarPos;
		DockBarType 	Dock;
		if (GetMainFrame()->GetBarPosInfo(WindowID,&Dock,&pKernelBarPos))
		{
			SetDockBarType(Dock);
			if (Dock == DOCKBAR_FLOAT)
				SetFloatingCPoint(wxPoint(pKernelBarPos->x,pKernelBarPos->y));
			else
			{
				SetSlot(pKernelBarPos->Slot);
				SetOffset(pKernelBarPos->Position);
			}
		}

		// Save this info bar's data in the static vars
		// These vars are used by
		InformationBarOp::Dock 			= GetDockBarType();
		InformationBarOp::Slot 			= GetSlot();
		InformationBarOp::Offset 		= GetOffset();
		InformationBarOp::FloatPos 		= GetFloatingCPoint();
#endif		
		DialogBarOp::Delete();
	}
 }

/********************************************************************************************

>	static BOOL InformationBarOp::Read(CCLexFile& file)

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/10/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE If InfoBar Data read OK
	Purpose:	Read InfoBar Data from Ini file 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL InformationBarOp::Read(CCLexFile& file)
{
#pragma message( __LOCMSG__ "InformationBarOp::Read - do nothing" )
	TRACE( _T("Warning - InformationBarOp::Read called") );
/*	TokenIndex Token;
	const TCHAR* TokenBuf = file.GetTokenBuf();
	BOOL ok;

	// Read the bar's docking position
	ok = file.GetSimpleToken();

	if (ok)
	{
		Token = FindToken(TokenBuf);
		switch (Token)
		{
			case TOKEN_DOCKBAR_TOP    : InformationBarOp::Dock = DOCKBAR_TOP;    break;
			case TOKEN_DOCKBAR_BOTTOM : InformationBarOp::Dock = DOCKBAR_BOTTOM; break;
			case TOKEN_DOCKBAR_LEFT   : InformationBarOp::Dock = DOCKBAR_LEFT;   break;
			case TOKEN_DOCKBAR_RIGHT  : InformationBarOp::Dock = DOCKBAR_RIGHT;  break;
			case TOKEN_DOCKBAR_FLOAT  : InformationBarOp::Dock = DOCKBAR_FLOAT;  break;

			default : 
				ok = FALSE;
				TRACE( _T("Unrecognised Info bar position : '%s'"),TokenBuf);
				break;
		}

		if (ok)
		{
			// If the bar is floating, then read in the floating X and Y coords
			// Otherwise, read in the bar's docked slot and offset.
			if (InformationBarOp::Dock == DOCKBAR_FLOAT)
			{
				ok = file.GetSimpleToken();
				if (ok)
				{	
					ok = (camSscanf(TokenBuf,"%li",&InformationBarOp::FloatPos.x) == 1);
					if (!ok) TRACE( _T("Expected the Infobar's floating x pos, but got '%s'\n"),TokenBuf);
				}

				if (ok) ok = file.GetSimpleToken();
				if (ok)
				{
					ok = (camSscanf(TokenBuf,"%li",&InformationBarOp::FloatPos.y) == 1);
					if (!ok) TRACE( _T("Expected the Infobar's floating y pos, but got '%s'\n"),TokenBuf);
				}
			}
			else
			{
				ok = file.GetSimpleToken();
				if (ok)
				{
					ok = (camSscanf(TokenBuf,"%li",&InformationBarOp::Slot) == 1);
					if (!ok) TRACE( _T("Expected the Infobar's slot, but got '%s'\n"),TokenBuf);
				}

				if (ok) ok = file.GetSimpleToken();
				if (ok)
				{
					ok = (camSscanf(TokenBuf,"%li",&InformationBarOp::Offset) == 1);
					if (!ok) TRACE( _T("Expected the Infobar's offset, but got '%s'\n"),TokenBuf);
				}
			}
		}
		if (ok)
		{
			ok = file.GetSimpleToken();
			if (ok)
			{
				Token = FindToken(TokenBuf);
				switch (Token)
				{
					case TOKEN_VISIBLE   : InformationBarOp::Visible = TRUE;  break;
					case TOKEN_INVISIBLE : InformationBarOp::Visible = FALSE; break;
					default : 
						ok = FALSE;
						TRACE( _T("Expected '%s' or '%s', but got '%s'\n"),TokenTable[TOKEN_VISIBLE].Token,TokenTable[TOKEN_INVISIBLE].Token,TokenBuf);
						break;
				}
			}
		}
	}
	if(ok)
		if(CurrentInfoBarOp)
			CurrentInfoBarOp->SetInfo();
	
	if(CurrentInfoBarOp && CurrentInfoBarOp->WindowID )
	{
		KernelBarPos NewPos;
		if (CurrentInfoBarOp->Dock == DOCKBAR_FLOAT)
		{
			NewPos.x=InformationBarOp::FloatPos.x;
			NewPos.y=InformationBarOp::FloatPos.y;
	
		}
		else
		{
			NewPos.Slot= InformationBarOp::Slot;
			NewPos.Position= InformationBarOp::Offset;
		}
		GetMainFrame()->MoveBar(CurrentInfoBarOp->WindowID,InformationBarOp::Dock,NewPos);		
	}
	return ok; */

	return false;
}


/********************************************************************************************

>   static BOOL InformationBarOp::Write(CCLexFile& file)

	Author:     Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:    20/10/94
	Inputs:     file = a file to Write the dialog bar to.
	Outputs:    -
	Returns:    TRUE if successful, FALSE otherwise
	Purpose:    Write the Infobar  static data to the file in a tokenized form
	Errors:     -
	SeeAlso:    -

********************************************************************************************/

BOOL InformationBarOp::WriteStatic(CCLexFile& file)
{
	BOOL ok = TRUE;
	String_256 str;

	if (ok) ok = file.PutToken(TokenTable[TOKEN_INFO_BAR].Token);
			
	if(IsVisible())
	{
		if(CurrentInfoBarOp)
			CurrentInfoBarOp->GetInfo();
	}
	switch (InformationBarOp::Dock)
	{
		case DOCKBAR_TOP    : str = TokenTable[TOKEN_DOCKBAR_TOP].Token;    break;
		case DOCKBAR_BOTTOM : str = TokenTable[TOKEN_DOCKBAR_BOTTOM].Token; break;
		case DOCKBAR_LEFT   : str = TokenTable[TOKEN_DOCKBAR_LEFT].Token;   break;
		case DOCKBAR_RIGHT  : str = TokenTable[TOKEN_DOCKBAR_RIGHT].Token;  break;
		case DOCKBAR_FLOAT  : str = TokenTable[TOKEN_DOCKBAR_FLOAT].Token;  break;
		default             : ok  = FALSE;                break;
	}

	if (ok) ok = file.PutToken(str);

	if (Dock == DOCKBAR_FLOAT)
	{
		if (ok) ok = file.PutToken(InformationBarOp::FloatPos.x);
		if (ok) ok = file.PutToken(InformationBarOp::FloatPos.y);
	}
	else
	{
		if (ok) ok = file.PutToken(InformationBarOp::Slot);
		if (ok) ok = file.PutToken(InformationBarOp::Offset);
	}

	if(IsVisible())
	{
		if (ok) ok = file.PutToken(TokenTable[TOKEN_VISIBLE].Token);
	}
	else
	{
		if (ok) ok = file.PutToken(TokenTable[TOKEN_INVISIBLE].Token);
	}

 	if (ok) ok = file.PutNewLine();
	if (ok) ok = file.PutNewLine();

	return ok;
}

/********************************************************************************************

>   static BOOL InformationBarOp::Read(HKEY hSubKey)

	Author:     Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:    20/2/97
	Inputs:     hSubKey = the key to use as the parent key when reading from the registry.
	Outputs:    -
	Returns:    TRUE if successful, FALSE otherwise
	Purpose:    read the Infobar static data from the registry
	Errors:     -
	SeeAlso:    -

********************************************************************************************/

BOOL InformationBarOp::Read(HKEY hSubKey)
{
#pragma message( __LOCMSG__ "InformationBarOp::Read - do nothing" )
	TRACE( _T("Warning - InformationBarOp::Read called") );
/*	// Create our main token under the information bar name
	HKEY hRegKey = OpenRegKey(hSubKey, TokenTable[TOKEN_INFO_BAR].Token);
	if (hRegKey == NULL)
	{
		//ERROR3("InformationBarOp::Read failed to get registry key");
		return FALSE;
	}

	BOOL ok = TRUE;
	DockBarType Dock = (DockBarType)GetRegInt(hRegKey, TEXT("Dock"));
	// check that the value read in is ok
	switch (Dock)
	{
		case DOCKBAR_TOP:
		case DOCKBAR_BOTTOM:
		case DOCKBAR_LEFT:
		case DOCKBAR_RIGHT:
		case DOCKBAR_FLOAT:
			InformationBarOp::Dock = Dock;
			break;
		default : 
			ok = FALSE;
			TRACE( _T("Unrecognised Info bar position : '%d'"),Dock);
			break;
	}

	if (Dock == DOCKBAR_FLOAT)
	{
		InformationBarOp::FloatPos.x = GetRegDword(hRegKey, TEXT("x"));
		InformationBarOp::FloatPos.y = GetRegDword(hRegKey, TEXT("y"));
	}
	else
	{
		InformationBarOp::Slot = GetRegDword(hRegKey, TEXT("Slot"));
		InformationBarOp::Offset = GetRegDword(hRegKey, TEXT("Offset"));
	}

	// get the visibility of the bar
	InformationBarOp::Visible = GetRegBool(hRegKey, TEXT("Visibility"));

	// Shut down our use of the key
	CloseRegKey(hRegKey);

	// if things were ok then set this information as the current set
	if (ok && CurrentInfoBarOp)
		CurrentInfoBarOp->SetInfo();

	if (CurrentInfoBarOp && CurrentInfoBarOp->WindowID )
	{
		KernelBarPos NewPos;
		if (CurrentInfoBarOp->Dock == DOCKBAR_FLOAT)
		{
			NewPos.x = InformationBarOp::FloatPos.x;
			NewPos.y = InformationBarOp::FloatPos.y;
	
		}
		else
		{
			NewPos.Slot = InformationBarOp::Slot;
			NewPos.Position = InformationBarOp::Offset;
		}
		GetMainFrame()->MoveBar(CurrentInfoBarOp->WindowID, InformationBarOp::Dock,NewPos);		
	}

	return ok; */

	return false;
}

/********************************************************************************************

>   static BOOL InformationBarOp::Write(HKEY hSubKey)

	Author:     Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:    20/2/97
	Inputs:     hSubKey = the key to use as the parent key when writing to the registry under.
	Outputs:    -
	Returns:    TRUE if successful, FALSE otherwise
	Purpose:    Write the Infobar static data to the registry
	Errors:     -
	SeeAlso:    -

********************************************************************************************/

BOOL InformationBarOp::Write(HKEY hSubKey)
{
#pragma message( __LOCMSG__ "InformationBarOp::Write - do nothing" )
	TRACE( _T("Warning - InformationBarOp::Write called") );
/*	// Ensure the section is blank before we start saving any data
	DeleteRegKey(hSubKey, TokenTable[TOKEN_INFO_BAR].Token);

	// Create our main token under the information bar name
	HKEY hRegKey = CreateRegKey(hSubKey, TokenTable[TOKEN_INFO_BAR].Token);
	if (hRegKey == NULL)
	{
		ERROR3("InformationBarOp::Write failed to get registry key");
		return FALSE;
	}
			
	if (IsVisible())
	{
		if (CurrentInfoBarOp)
			CurrentInfoBarOp->GetInfo();
	}

	BOOL ok = TRUE;
	ok = ok && SetRegInt(hRegKey, TEXT("Dock"), InformationBarOp::Dock);

	if (Dock == DOCKBAR_FLOAT)
	{
		ok = ok && SetRegDword(hRegKey, TEXT("x"), InformationBarOp::FloatPos.x);
		ok = ok && SetRegDword(hRegKey, TEXT("y"), InformationBarOp::FloatPos.y);
	}
	else
	{
		ok = ok && SetRegDword(hRegKey, TEXT("Slot"), InformationBarOp::Slot);
		ok = ok && SetRegDword(hRegKey, TEXT("Offset"), InformationBarOp::Offset);
	}

	// save the visibility of the bar
	ok = ok && SetRegBool(hRegKey, TEXT("Visibility"), IsVisible());

	// Shut down our use of the key
	CloseRegKey(hRegKey);

	return ok; */

	return false;
}

/********************************************************************************************

>	static BOOL InformationBarOp::IsVisible()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/5/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if a InformationBarOp is on screen
	Purpose:	Used by the tool bar customize system to find out if a tool info bar is
				visible or not. 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL InformationBarOp::IsVisible()
{
	return (InformationBarOp::Visible);
}

/********************************************************************************************

>	static void InformationBarOp::SetVisibility(BOOL Open)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/5/94
	Inputs:		Open - 	if TRUE then open current InformationBarOp in its last pos if it is
						currently closed.
						if FALSE, it is closed
	Outputs:	-
	Returns:	-
	Purpose:	Used by the tool bar customize system to open and close a tool bar.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
/*
void InformationBarOp::SetVisibility(BOOL Open)
{

	if (Open)
	{
		if (!InformationBarOp::Hidden)
		{
			InformationBarOp::Hidden = TRUE;
			if (InformationBarOp::CurrentInfoBarOp != NULL)
			{
				InformationBarOp::CurrentInfoBarOp->Create();
				//InformationBarOp::CurrentInfoBarOp->Open();
			}
		}
	}
	else
	{
		if (InformationBarOp::Hidden)
		{
			SelectorTool::UnCacheInfoBar();
			if (InformationBarOp::CurrentInfoBarOp != NULL)
			{
				InformationBarOp::CurrentInfoBarOp->Delete();
			 	//InformationBarOp::CurrentInfoBarOp->Close();
			}
			InformationBarOp::Hidden = FALSE;
		}
	}
}
*/

void InformationBarOp::SetVisibility(BOOL Open,BOOL Force)
{

	if (Open)
	{
		if (!InformationBarOp::Visible || Force)
		{
			InformationBarOp::Visible = TRUE;
			if (InformationBarOp::CurrentInfoBarOp != NULL)
			{
				InformationBarOp::CurrentInfoBarOp->Create();
			}
		}
	}
	else
	{
		if (InformationBarOp::Visible || Force)
		{
		#ifndef STANDALONE
			SelectorTool::UnCacheInfoBar(TRUE);
		#endif
			if (InformationBarOp::CurrentInfoBarOp != NULL)
			{
				InformationBarOp::CurrentInfoBarOp->Delete();
			}
			InformationBarOp::Visible = FALSE;
		}
	}
}
