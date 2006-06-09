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
//#include "ensure.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "dockbar.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "childbar.h"
#include "camframe.h"
//#include "camafx.h"
//#include "errors.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "basebar.h"

#define IS_CHICAGO ((LOBYTE(LOWORD(GetVersion()))) > 3)

#define MIN_GALLERY_SLOT_WIDTH 100

BOOL OILFixedDockingBar::allowTidyBars = TRUE;

#if 0

/////////////////////////////////////////////////////////////////////////////////////////////
//									 OILDockingBar Base Class														   //
/////////////////////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(OILDockingBar ,CControlBar)

#ifdef _DEBUG
	#undef THIS_FILE
	static char BASED_CODE THIS_FILE[] = __FILE__;
#endif



// A pointer to the last dock which has changed size
// used to reduce the number of recalc layouts
OILDockingBar * OILDockingBar::ChangedDock=NULL;

// Flag used as above
BOOL OILDockingBar::DoRecalc = FALSE;
 
/////////////////////////////////////////////////////////////////////////////////////////////
//		OILDockingBar Message Map
/////////////////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP( OILDockingBar,CControlBar)
	//{{AFX_MSG_MAP( OILDockingBar )
	//}}AFX_MSG_MAP
END_MESSAGE_MAP( )


/////////////////////////////////////////////////////////////////////////////////////////////
//		OILDockingBar Creation Destruction and Initialisation
/////////////////////////////////////////////////////////////////////////////////////////////

/********************************************************************************************

>	OILDockingBar::OILDockingBar()

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/3/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Creates an OILDockingBar object.
	Errors:		-
	SeeAlso:	

********************************************************************************************/
                    
OILDockingBar::OILDockingBar()
{ 
	ControlSize=SMALL_CONTROLS;
	HasBigControls = FALSE;
}
 
/********************************************************************************************

>	OILDockingBar::~OILDockingBar()

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/3/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Destroys an OILDockingBar object and deletes all BarList items
	Errors:		-
	SeeAlso:	

********************************************************************************************/

OILDockingBar::~OILDockingBar()
{
	ChildBars.DeleteAll();
}
 

/******************************************************************************************
>	void OILDockingBar::OnUpdateCmdUI()

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/3/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	- 
	Errors:		-
	SeeAlso:	

********************************************************************************************/

void OILDockingBar::OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler)
{

}


/********************************************************************************************

>	BOOL OILDockingBar::RemoveBar(OILChildBar * ThisBar)

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/3/94
	Inputs:		-
	Outputs:	-
	Returns:	Succed or Fail
	Purpose:	Remove OILChildBar from List and delete ListItem if successful					
	Errors:		-
	SeeAlso:	

********************************************************************************************/

BOOL OILDockingBar::RemoveBar(OILChildBar* ThisBar)
{
	ERROR2IF(ThisBar ==NULL,FALSE,"Trying to remove a NULL Child Bar");
	// Remove a child Bar from the list
	if (ChildBars.RemoveItem((ListItem*)ThisBar)!=NULL)
	{

		delete ThisBar;		// Delete ChildBar object
		return TRUE;

	}
	else					// Trying to delete an unknown bar
	{
		ERROR2IF(TRUE,FALSE,"Failed to Remove Child Bar from list");
	}
}

/********************************************************************************************

>	BOOL OILDockingBar::SetNewHwnd(HWND OldHwnd,HWND NewHwnd)

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/3/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if success FALSE otherwise
	Purpose:	change the hwnd of a bar list item
	Errors:		-
	SeeAlso:	

********************************************************************************************/

BOOL OILDockingBar::SetNewHwnd(HWND OldHwnd,HWND NewHwnd)
{
	OILChildBar * ThisChild =(OILChildBar *) ChildBars.GetHead(); 
   	while (ThisChild != NULL)
	{

		if (ThisChild->GetHwnd()==OldHwnd)	// found the bar
		{
			ThisChild->SetHwnd(NewHwnd);
			return TRUE;
		}
		ThisChild =(OILChildBar *) ChildBars.GetNext(ThisChild);	

	}// end while
	ENSURE(ThisChild!=NULL,"Couldn't find HWND in BarList");
	return FALSE;
}


/********************************************************************************************

>	OILChildBar * OILDockingBar::IsBarInList(HWND)

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/3/94
	Inputs:		-
	Outputs:	-
	Returns:	Found Child or NULL
	Purpose:	determine whether a childbar is in this docks list
	Errors:		-
	SeeAlso:	

********************************************************************************************/

OILChildBar * OILDockingBar::IsBarInList(HWND hwnd)
{
	OILChildBar * ThisChild =(OILChildBar *) ChildBars.GetHead(); 
   	while (ThisChild != NULL)
	{

		if (ThisChild->GetHwnd()==hwnd)	// found the bar
			return ThisChild;

		ThisChild =(OILChildBar *) ChildBars.GetNext(ThisChild);	

	}// end while

	return NULL;
}

  /********************************************************************************************

>	OILChildBar * OILDockingBar::IsBarInList(StringBase * Name )

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/3/94
	Inputs:		-
	Outputs:	-
	Returns:	Found Child or NULL
	Purpose:	determine whether a childbar is in this docks list
	Errors:		-
	SeeAlso:	

********************************************************************************************/

OILChildBar * OILDockingBar::IsBarInList(StringBase *Name)
{
	OILChildBar * ThisChild =(OILChildBar *) ChildBars.GetHead(); 
   	while (ThisChild != NULL)
	{
		char buff[64];
		::GetWindowText(ThisChild->GetHwnd(),buff,63); 
		if (String_32(buff)== *Name)	// found the bar
			return ThisChild;

		ThisChild =(OILChildBar *) ChildBars.GetNext(ThisChild);	

	}// end while

	return NULL;
}
/********************************************************************************************

>	HWND OILDockingBar::IsScreenPointInBar(CPoint pnt)

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/3/94
	Inputs:		-
	Outputs:	-
	Returns:	Found HWND or NULL
	Purpose:	determine whether a screen point is in a childbar of this dock
	Errors:		-
	SeeAlso:	

********************************************************************************************/

HWND OILDockingBar::IsScreenPointInBar(CPoint pnt) 
{

	OILChildBar * ThisChild =(OILChildBar *) ChildBars.GetHead(); 
	HWND hwnd;
	while (ThisChild != NULL)
	{
		if (hwnd = ThisChild->GetHwnd())	// found the bar
		{
			CRect sRect;
			::GetWindowRect(hwnd,&sRect);
			if(sRect.PtInRect(pnt))
			{
				return hwnd;
			}
   		}
		ThisChild =(OILChildBar *) ChildBars.GetNext(ThisChild);	
	}// end while

	return NULL;
}
/********************************************************************************************

>	KernelBarPos * OILDockingBar::GetBarPos(HWND)

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/3/94
	Inputs:		-
	Outputs:	-
	Returns:	Found Position 
	Purpose:	To return a Pointer to the KernelBarPos of a Child dialogbar
	Errors:		-
	SeeAlso:	

********************************************************************************************/

KernelBarPos * OILDockingBar::GetBarPos(HWND hwnd)
{
	OILChildBar * ThisChild;
	if ((ThisChild=IsBarInList(hwnd))!=NULL)
	
		return ThisChild->GetBarPos();
	else
	
		return NULL;	
}


 /********************************************************************************************

>	CRect OILDockingBar::GetBarRect(StringBase * Name)

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/3/94
	Inputs:		-
	Outputs:	-
	Returns:	Found Position 
	Purpose:	To return the rectangle of the specified bar
	Errors:		-
	SeeAlso:	

********************************************************************************************/

CRect OILDockingBar::GetBarRect(StringBase * Name)
{
	OILChildBar * ThisChild;
	if ((ThisChild=IsBarInList(Name))!=NULL)
		return ThisChild->GetRectFromBar();
	else
		return CRect(0, 0, 0, 0);
}



/****************************************************************************

>	CRect OILDockingBar::GetBarRect(HWND hwnd)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	01/12/2004

	Inputs:		hwnd		- 
	Returns:	
	Purpose:	To return the rectangle of the specified bar

****************************************************************************/

CRect OILDockingBar::GetBarRect(HWND hwnd)
{
	OILChildBar * ThisChild;
	if ((ThisChild=IsBarInList(hwnd))!=NULL)
		return ThisChild->GetRectFromBar();
	else
		return CRect(0, 0, 0, 0);
}


/********************************************************************************************

>	KernelBarPos * OILDockingBar::GetBarPos(StringBase * Name)

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/3/94
	Inputs:		-
	Outputs:	-
	Returns:	Found Position 
	Purpose:	To return a Pointer to the KernelBarPos of a Child dialogbar
	Errors:		-
	SeeAlso:	

********************************************************************************************/

KernelBarPos * OILDockingBar::GetBarPos(StringBase * Name)
{
	OILChildBar * ThisChild;
	if ((ThisChild=IsBarInList(Name))!=NULL)
	
		return ThisChild->GetBarPos();
	else
	
		return NULL;	
}

/********************************************************************************************

>	void OILDockingBar::SetBigControlState(BOOL Big)

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/3/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Sets Big Control state
	Errors:		-
	SeeAlso:	
													  
********************************************************************************************/

void OILDockingBar::SetBigControlState(BOOL Big )
{
	HasBigControls = Big;
	if (Big)
		ControlSize=LARGE_CONTROLS;
	else
		ControlSize=SMALL_CONTROLS;

	// tell all child windows that control sizes have changed
	OILChildBar * ThisChild =(OILChildBar *) ChildBars.GetHead(); 
   	while (ThisChild != NULL)
	{
		ThisChild->Resize(ControlSize);
		ThisChild =(OILChildBar *) ChildBars.GetNext(ThisChild);	
	}// end while
	
}

/********************************************************************************************

>	BOOL OILDockingBar::IsBigControls()

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/4/94
	Inputs:		-
	Outputs:	-
	Returns:	True if BigControls
	Purpose:	Determines whether DockingBar contains large controls
	Errors:		-
	SeeAlso:	

********************************************************************************************/

BOOL OILDockingBar::IsBigControls()
{

	return	HasBigControls;
}



/////////////////////////////////////////////////////////////////////////////////////////////
// 				OILFixedDockingBar
/////////////////////////////////////////////////////////////////////////////////////////////


IMPLEMENT_DYNAMIC(OILFixedDockingBar ,OILDockingBar)



/////////////////////////////////////////////////////////////////////////////////////////////
//		OILFixedDockingBar Message Map
/////////////////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP( OILFixedDockingBar, OILDockingBar )
	//{{AFX_MSG_MAP( OILFixedDockingBar )
	ON_MESSAGE(WM_SIZEPARENT, OnSizeParent) 
	//}}AFX_MSG_MAP
END_MESSAGE_MAP( )

/********************************************************************************************

>	BOOL OILFixedDockingBar::Create(CWnd* pParentWnd,DockBarType ReqType, UINT32 id)

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/3/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Creates the OILDockingBar window.
	Errors:		-
	SeeAlso:	

********************************************************************************************/

BOOL OILFixedDockingBar::Create(CWnd* pParentWnd,DockBarType ReqType, UINT32 id)
{
//	BackBrush.CreateSolidBrush(GetSysColor(COLOR_BTNFACE));
	BOOL CreatedWindow;
	ENSURE(pParentWnd,"DockingBar must have a Parent");

	ENSURE((ReqType == DOCKBAR_LEFT) ||
		   (ReqType == DOCKBAR_RIGHT) ||
		   (ReqType == DOCKBAR_TOP) ||
		   (ReqType == DOCKBAR_BOTTOM),
		   "OILFIxedDockingBar constructor: Invalid DockingBar type");

	DockType=ReqType; 	// Type of this instance
	Parent=pParentWnd;	// Parent of this bar
	CRect rect;			// Rectangle for Create
	rect.SetRectEmpty();

	#if _MSC_VER < 1200
		#ifndef WIN32
			DWORD
		#endif
			m_dwStyle = WS_CHILD|WS_VISIBLE |WS_CLIPCHILDREN;
	#else
		// VC6 doesn't like any style flags other than CBRS_ flags set 
		// for CControlBar-derived windows
		m_dwStyle = 0;
	#endif // _MSC_VER

	// create the HWND
	CreatedWindow = CWnd::Create(NULL,
						0,
						m_dwStyle,
						rect,Parent,
						id,	NULL);


	#if _MSC_VER >= 1200
	// Must make the control bar visible in VC6 builds
	ShowWindow(SW_SHOW);
	#endif // _MSC_VER

	return CreatedWindow;
}



/********************************************************************************************

>	OILFixedDockingBar::OILFixedDockingBar()

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/3/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Creates an OILDockingBar object.
	Errors:		-
	SeeAlso:	

********************************************************************************************/
                    
OILFixedDockingBar::OILFixedDockingBar()
{ 
	NoOfSlots = 0; 
	ControlSize = SMALL_CONTROLS;
	HasBigControls = FALSE;
	DockSize.cy = DockSize.cx = 0;
	Position.x = Position.y = -1;
	allowTidyBars = TRUE;
	for (INT32 i = 0; i < MAX_SLOTS; i++)
		SlotSize[i] = SMALL_CONTROLS;
}

/********************************************************************************************

>	BOOL OILFixedDockingBar::DeleteBars()

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/3/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	deletes all BarList items
	Errors:		-
	SeeAlso:	

********************************************************************************************/

BOOL OILFixedDockingBar::DeleteBars()
{
	NoOfSlots = 0; 
	for (INT32 i = 0; i < MAX_SLOTS; i++)
		SlotSize[i] = ControlSize;
	DockSize.cy = DockSize.cx = 0;
	Position.x = Position.y = -1;
	ChildBars.DeleteAll();
	return TRUE;
} 

/////////////////////////////////////////////////////////////////////////////////////////////
//  Message Handling                                              
////////////////////////////////////////////////////////////////////////////////////////////


/********************************************************************************************

>	void OILFixedDockingBar::DoPaint(WPARAM, LPARAM lParam)

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/3/94
	Inputs:		pointer to a device context
	Outputs:	-
	Returns:	-
	Purpose:	Paints docking bar background and border	  
	Errors:		-
	SeeAlso:	

********************************************************************************************/
void OILFixedDockingBar::DoPaint(CDC* pDC)
{
	
	ASSERT_VALID(pDC);
		
	CRect rect;
	GetClientRect(rect); 								// get the size of this bar
	CBrush 	hbrFrame (GetSysColor(COLOR_BTNFACE));
	
	
	COLORREF Outline;
	if(IS_CHICAGO)
		Outline = GetSysColor(COLOR_BTNSHADOW);
	else
		Outline = GetSysColor(COLOR_WINDOWFRAME);

	CPen hpen(PS_SOLID,1,Outline);

	// paint bodge to avoid double lines
		
	CBrush * hOldBrush = pDC->SelectObject(&hbrFrame);
	CPen   * hOldPen = pDC->SelectObject(&hpen);
	
	//pDC->FillRect(&rect,&hbrFrame);
	
	
 	switch (DockType)
	{

		case DOCKBAR_LEFT:
			pDC->Rectangle(rect.left-1,rect.top-1,rect.right,rect.bottom+1);
			break;
		case DOCKBAR_TOP:
			pDC->Rectangle(rect.left-1,rect.top-1,rect.right+1,rect.bottom);
			break;
		case DOCKBAR_RIGHT:
			pDC->Rectangle(rect.left,rect.top-1,rect.right+1,rect.bottom+1);
			break;
		case DOCKBAR_BOTTOM:
			pDC->Rectangle(rect.left-1,rect.top,rect.right+1,rect.bottom+1);
			break;

	}
	
	pDC->SelectObject(hOldBrush);
	pDC->SelectObject(hOldPen);
}


/********************************************************************************************

>	void OILFixedDockingBar::RepositionWindow(AFX_SIZEPARENTPARAMS FAR* lpLayout, 
				HWND hWnd, LPCRECT lpRect)

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/9/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Is called from OnSizeParent to reposition the docking bars - replacing
				Afx_RepositionWindow which forces a total redraw !
	Errors:		-

********************************************************************************************/


void OILFixedDockingBar::RepositionWindow(AFX_SIZEPARENTPARAMS FAR* lpLayout, 
				HWND hWnd, LPCRECT lpRect)
{
	
	if(hWnd == NULL)
	{
		ENSURE(FALSE,"Reposition NULL Window ");
		return ;
	}
	if(lpRect == NULL)
	{	
		ENSURE(FALSE,"NULL Reposition Rectangle");
		return;
	}
	
	CRect InvalRect(0,0,0,0);

	CRect Check(lpRect); 
	// check whether this is an empty bar
	BOOL Empty = Check.Width() == 0 || Check.Height() == 0;
		
	// Reposition the window but dont force a redraw	
	// we use SetWindowPos rather than DeferWindowPos - this allows us
	// to position the window and then invalidate a part of it

	if (!Empty) // calculate a rectangle to invalidate
	{
		CRect rectOld;
		GetWindowRect(rectOld);             //old window position in screen coords
		GetParent()->ClientToScreen(&Check);

		// no change - get out of here
		if(Check==rectOld)
			return ;

		switch (DockType)
   		{
		case DOCKBAR_TOP:
			if(Check.left == rectOld.left)
			{
				if (Check.right > rectOld.right )
					InvalRect.SetRect(rectOld.right,Check.top,Check.right,Check.bottom);
				else if(Check.right < rectOld.right )
					InvalRect.SetRect(Check.right,Check.top,rectOld.right,Check.bottom);
				else
					InvalRect.SetRect(Check.left,Check.top,rectOld.right,Check.bottom);
			}
			else //do whole area
			{
			   InvalRect.SetRect(min(rectOld.left,Check.left),
			   					 min(rectOld.top,Check.top),
			   					 max(rectOld.right,Check.right),
			   					 max(rectOld.bottom,Check.bottom));
			}				
			break;
		case DOCKBAR_BOTTOM:
			if(	Check.left	!=	rectOld.left	||	Check.top	!=	rectOld.top)
				InvalRect.SetRect(Check.left,Check.top,Check.right,Check.bottom);
			else
				InvalRect.SetRect(rectOld.right,Check.top,Check.right,Check.bottom);
		
			break;
		case DOCKBAR_LEFT:
			if(Check.top == rectOld.top)
			{
				if (Check.bottom > rectOld.bottom )
					InvalRect.SetRect(Check.left,rectOld.bottom,Check.right,Check.bottom);
				else 
					InvalRect.SetRect(Check.left,Check.bottom,Check.right,rectOld.bottom);
			}
			else //do whole area
			{
			   InvalRect.SetRect(min(rectOld.left,Check.left),
			   					 min(rectOld.top,Check.top),
			   					 max(rectOld.right,Check.right),
			   					 max(rectOld.bottom,Check.bottom));
			}
			break;
		case DOCKBAR_RIGHT:
			if(	Check.left	!=	rectOld.left	||	Check.top	!=	rectOld.top)
				InvalRect.SetRect(Check.left,Check.top,Check.right,Check.bottom);
			else
				InvalRect.SetRect(Check.left,rectOld.bottom,Check.right,Check.bottom);
			break;
		case DOCKBAR_FLOAT:
			InvalRect.SetRect(rectOld.left,rectOld.bottom,rectOld.right,rectOld.bottom);
			break;
		}
	}
	
	SetWindowPos(NULL, lpRect->left, 
		lpRect->top, lpRect->right - lpRect->left, lpRect->bottom - lpRect->top,
		SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOREDRAW);
	
	
	if(!Empty)
	{
		// we need a touch of bit twiddling here to make sure that 
		// paint messages are sent out to the dock and their bars
		INT32 OldStyle = ::GetWindowLong(hWnd,GWL_STYLE);
		::SetWindowLong(hWnd,GWL_STYLE,OldStyle & ~WS_CLIPCHILDREN);
		
		//Convert back to parent coords
		GetParent()->ScreenToClient(&InvalRect);
	
    	// invalidate  'new' area
		GetParent()->InvalidateRect(&InvalRect,FALSE);
		GetParent()->UpdateWindow();
		
		// set the style back to it's original state 
 		::SetWindowLong(hWnd,GWL_STYLE,OldStyle);
	}
}

/********************************************************************************************

>	LRESULT OILFixedDockingBar::OnSizeParent(WPARAM, LPARAM lParam)

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/3/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Afx private Message  - Resizes Docking Bar and Parent MDIClient Window
	Errors:		-
	SeeAlso: 	MFC\src\barcore.ccp - MFC TechNote 24	

********************************************************************************************/


LRESULT OILFixedDockingBar::OnSizeParent(WPARAM, LPARAM lParam)
{
	AFX_SIZEPARENTPARAMS FAR* lpLayout = (AFX_SIZEPARENTPARAMS FAR*)lParam;
	CRect rect(0,0,0,0);
	CRect LayoutRect(lpLayout->rect);

	SetSize(LayoutRect,&rect);

	switch (DockType)
   	{

	case DOCKBAR_LEFT:
		lpLayout->rect.left += TotalSlotSizes();// left-hand side of this window
//		if(IS_CHICAGO)
//			lpLayout->rect.left--;
		break;
	case DOCKBAR_RIGHT:
		lpLayout->rect.right -= TotalSlotSizes();// right-hand side of this window
		break;
	case DOCKBAR_TOP:
		lpLayout->rect.top += TotalSlotSizes();//  top of this window
//		if(IS_CHICAGO)
//			lpLayout->rect.top--;
		break;
	case DOCKBAR_BOTTOM:
		lpLayout->rect.bottom -= TotalSlotSizes();// bottom of this window
//		if(IS_CHICAGO)
//			lpLayout->rect.bottom ++;	
		break;
	default:
		ENSURE(0, "Unknown Dockingbar type!");
		break;
	}
	
	// call our Reposition window for more redraw control
	//_Afx
	RepositionWindow(lpLayout, m_hWnd, &rect);
	return 0;
}



/////////////////////////////////////////////////////////////////////////////////////////////
//			OILFixedDocking Bar General Functions
/////////////////////////////////////////////////////////////////////////////////////////////


INT32 OILFixedDockingBar::TotalSlotSizes(INT32 Slot /* = -1 */)
{
	if (Slot == -1)
		Slot = NoOfSlots;

	// Sum slot sizes
	INT32 TotalSize = 0;
	for (INT32 i = 0; i < Slot; i++)
		TotalSize += SlotSize[i];

	return(TotalSize);
}

/********************************************************************************************

>	void OILFixedDockingBar::SetBigControlState(BOOL Big)

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/3/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Sets Big Control state
	Errors:		-
	SeeAlso:	

********************************************************************************************/

void OILFixedDockingBar::SetBigControlState(BOOL Big )
{
	// check that state has actually changed
	if (Big != HasBigControls )
	{
  		::LockWindowUpdate(GetMainFrame()->GetSafeHwnd());
  		HasBigControls = Big;
		ControlSizes NewSize;
		if (Big)
			NewSize = LARGE_CONTROLS;
		else
			NewSize = SMALL_CONTROLS;

		// Set any slots that are the old size to new size
		// TODOG: This needs to change
		for (INT32 i = 0; i < MAX_SLOTS; i++)
		{
			if (SlotSize[i] == ControlSize)
				SlotSize[i] = NewSize;
		}
//		for (INT32 i = 0; i < MAX_SLOTS; i++)
//		{
//			SlotSize[i] = NewSize;
//		}

		// Call the base class to update the ControlSize member
		// and resize the child bars
		OILDockingBar::SetBigControlState(Big);
		
		// have to be a bit careful here DialogBar will delete and add new items
		// to this list when it receives the RESIZECONTROLS Message 
		OILChildBar * ThisChild =(OILChildBar *) ChildBars.GetHead();
		OILChildBar * Next; 
	 	OILChildBar * EndList =(OILChildBar *) ChildBars.GetTail(); 

	 	while (ThisChild != NULL)
		{
			Next =(OILChildBar *) ChildBars.GetNext(ThisChild);
			::SendMessage(ThisChild->GetHwnd(), WM_RESIZECONTROLS, 0,0);
			ThisChild = Next;
			if (ThisChild == NULL)
				break;
			if (ThisChild == EndList)
			{
				::SendMessage(ThisChild->GetHwnd(), WM_RESIZECONTROLS, 0,0);
				break;
			}		
		}

		// update the bars for the new slot sizes
		ThisChild =(OILChildBar *) ChildBars.GetHead(); 
	   	while (ThisChild != NULL)
		{
			ThisChild->UpdatePos();
			ThisChild =(OILChildBar *) ChildBars.GetNext(ThisChild);	
		}
		
		// check for any overlap
		if (Big)
			CheckAndCorrectOverlap();

		::LockWindowUpdate(NULL);
		((CFrameWnd*)Parent)->RecalcLayout();
	}
}

/********************************************************************************************
			
>	BOOL OILFixedDockingBar::IsScreenPointinDock(CPoint here)

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/3/94
	Inputs:		Screen Point
	Outputs:	-		
	Returns:	TRUE/FALSE
	Purpose:	determines whether a screen point is currently in or near a fixed dockingbar
	Errors:		-
	SeeAlso:	

********************************************************************************************/
           
BOOL OILFixedDockingBar::IsScreenPointInDock(CPoint here)
{
	CRect rect(0,0,DockSize.cx,DockSize.cy);
	// virtual rectangle to give a positive hit when the
	// real window has no size
	if (NoOfSlots==0)
	{
		switch (DockType)
		{
		case DOCKBAR_LEFT:
			rect.right = ControlSize;
			break;
		case DOCKBAR_RIGHT:
			rect.left =  - (INT32)ControlSize;
			break;
  		case DOCKBAR_TOP:
			rect.bottom = ControlSize;						
			break;
		case DOCKBAR_BOTTOM:
			rect.top = - (INT32)ControlSize;						
			break;
		}
    }
	ClientToScreen(&rect);
	return rect.PtInRect(here);

}


/********************************************************************************************

>	void OILFixedDockingBar::GetDockScreenRect(CRect * BarRect)

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/3/94
	Inputs:		Screen rect
	Outputs:	-		
	Returns:	-
	Purpose:	determines whether a screen rectangle intersects a fixed dockingbar
	Errors:		-
	SeeAlso:	

********************************************************************************************/
           
void OILFixedDockingBar::GetDockScreenRect(CRect * BarRect)
{
	CRect rect ; 
	GetClientRect(&rect);
	ClientToScreen(&rect);			//screen coords of this dock
	BarRect->CopyRect(rect);
}





/********************************************************************************************

>	BOOL OILFixedDockingBar::IsScreenRectInDock(CRect BarRect)

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/3/94
	Inputs:		Screen rect
	Outputs:	-		
	Returns:	TRUE/FALSE
	Purpose:	determines whether a screen rectangle intersects a fixed dockingbar
	Errors:		-
	SeeAlso:	

********************************************************************************************/
           
BOOL OILFixedDockingBar::IsScreenRectInDock(CRect BarRect)
{

	CRect rect(0,0,DockSize.cx,DockSize.cy);
	if (NoOfSlots==0)  				//docking bar has no width/height 
	{

		switch (DockType)			//make a rectangle for the intersection test
		{
		case DOCKBAR_LEFT:
		case DOCKBAR_RIGHT:
			rect.right = 1;
			break;
  		case DOCKBAR_TOP:
		case DOCKBAR_BOTTOM:
			rect.bottom = 1;						
			break;
		}

    }
	ClientToScreen(&rect);			//screen coords of this dock
	rect&=BarRect;		  			//do intersection
	return !(rect.IsRectEmpty());
}



/********************************************************************************************

>	void OILFixedDockingBar::SetSize()

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/3/94
	Inputs:		MDIClient area
	Outputs:	new bar size		
	Returns:	-
	Purpose:	Sizes a DockingBar Window according to type and the MainFrame Client Window .
	Errors:		-
	SeeAlso:	

********************************************************************************************/
                    
void OILFixedDockingBar::SetSize(CRect ClientRect,CRect * rect)
{

	rect->CopyRect(&ClientRect);	// Copy	MDIFrame Rect into target rect
	switch	(DockType)				// now size according to type
	{

	case DOCKBAR_LEFT:
		rect->right =ClientRect.left + TotalSlotSizes();
		break;
	case DOCKBAR_RIGHT:
		rect->left = ClientRect.right - TotalSlotSizes();
		break;
  	case DOCKBAR_TOP:
		rect->bottom = ClientRect.top + TotalSlotSizes();
		break;
	case DOCKBAR_BOTTOM:
		rect->top = ClientRect.bottom - TotalSlotSizes();
		break;

    }
	//Keep a record of Bar size
	DockSize.cx=rect->right-rect->left;
	DockSize.cy=rect->bottom-rect->top;
}
   
/********************************************************************************************

>	INT32 OILFixedDockingBar::PointToSlot(CPoint ThisPoint);

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/3/94
	Inputs:		a point	
	Outputs:	-		
	Returns:	slot number according to dock type
	Purpose:	determine Slot number according to DockType given a Client Coord
	Errors:		-
	SeeAlso:	

********************************************************************************************/
                    
INT32 OILFixedDockingBar::PointToSlot(CPoint* ThisPoint)
{
	if (ControlSize == 0)
		return(0);

	INT32 Value = 0;
	// return a slot number according to dock type and control size
	switch	(DockType)
	{
		case DOCKBAR_TOP:
		case DOCKBAR_BOTTOM:
			Value = ThisPoint->y;
		   	break;

		case DOCKBAR_LEFT:
		case DOCKBAR_RIGHT:
			Value = ThisPoint->x;
			break;
    }

	INT32 Total = 0;
	// Loop through slots until size totals more than coord
	for (INT32 i = 0; i < MAX_SLOTS; i++)
	{
		Total += SlotSize[i];
		if (Total > Value)
			return(i);
	}
	
	return(0);
}


/********************************************************************************************

>	void OILFixedDockingBar::BarPosToClient();

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/3/94
	Inputs:		a point	
	Outputs:	Client x/y given slot and position		
	Returns:	-
	Purpose:	determine Client coordinate in dock from a KernelBarPos
	Errors:		-
	SeeAlso:	

********************************************************************************************/
                    
void  OILFixedDockingBar::BarPosToClient(KernelBarPos Pos,CPoint * Point)
{
	// return a slot number according to dock type and control size
	switch	(DockType)
	{
		case DOCKBAR_TOP:
		case DOCKBAR_BOTTOM:
			Point->x = Pos.Position;
			Point->y = TotalSlotSizes(Pos.Slot);
		   	break;

		case DOCKBAR_LEFT:
		case DOCKBAR_RIGHT:
			Point->y = Pos.Position;
			Point->x = TotalSlotSizes(Pos.Slot);
			break;
    }
}

/********************************************************************************************

>	void OILFixedDockingBar::BarPosToScreen(KernelBarPos Pos,CPoint * Point);

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/3/94
	Inputs:		a point	
	Outputs:	Screen x/y given slot and position		
	Returns:	-
	Purpose:	determine Screen coordinate in dock from a KernelBarPos
	Errors:		-
	SeeAlso:	

********************************************************************************************/
                    
void  OILFixedDockingBar::BarPosToScreen(KernelBarPos Pos,CPoint * Point)
{
	BarPosToClient(Pos, Point);		// Get the client position
	ClientToScreen(Point);			// And convert to screen coords
}

/********************************************************************************************

>	INT32 OILFixedDockingBar::PointToPosition()

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/3/94
	Inputs:		CPoint
	Outputs:	-
	Returns:	position in slot according to dock type
	Purpose:	determine pixel offset inslot according to Docktype given a client coord
	Errors:		-
	SeeAlso:	

********************************************************************************************/

INT32 OILFixedDockingBar::PointToPosition(CPoint* ThisPoint)
{
	//return pixel offset in slot according to dock type
	switch	(DockType)
	{
		case DOCKBAR_TOP:
		case DOCKBAR_BOTTOM:
			return ThisPoint->x;
			break;

		case DOCKBAR_LEFT:
		case DOCKBAR_RIGHT:
			return ThisPoint->y;
			break;
    }
	return 0;
 }                  



 ////////////////////////////////////////////////////////////////////////////////////////////
 //	Child Bar Placement 
 ////////////////////////////////////////////////////////////////////////////////////////////

/********************************************************************************************

>	INT32 OILFixedDockingBar::GetIntersectPercent(OILChildbar * ThisBar)

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/3/94
	Inputs:		-
	Outputs:	-
	Returns:	Percentage of interesect area with other bars in this dock
	Purpose:	
	Errors:		-
	SeeAlso:	

********************************************************************************************/

INT32 OILFixedDockingBar::GetIntersectPercent(OILChildBar * ThisBar)
{
	
	CRect  CheckRect;											
	CRect  TempNew;																								 	
	CRect  ThisRect = ThisBar->GetRectClient();
	CPoint ThisPoint = ThisBar->GetPos();

	INT32 ThisArea = ThisRect.right*ThisRect.bottom;
	INT32 FoundArea = 0;
	CSize FoundSize;

	OILChildBar* ThisChild = (OILChildBar *) ChildBars.GetHead(); 
   	while (ThisChild != NULL)
	{
		if (ThisBar != ThisChild)				// Dont check against itself !! 
		{
			TempNew = ThisRect+ThisPoint;		// New bar rect	+ point offset	
											// this will be overwritten by the &= operation
			CheckRect = ThisChild->GetRectClient()+ThisChild->GetPos();// Found bar origin +point offset
	    	TempNew &= CheckRect;				// Intersection of Found and New Bar
	    	if (!TempNew.IsRectEmpty())		// Check for overlap
			{
				FoundSize = TempNew.Size();
				FoundArea += FoundSize.cx*FoundSize.cy;	// total area so far
			}
		}

		ThisChild =(OILChildBar *) ChildBars.GetNext(ThisChild);		
	}

	if (ThisArea==0)
		return 0;

	return (INT32) ceill(((float)(FoundArea*100))/(float)ThisArea);// return percentage overlap
}


/********************************************************************************************

>	void OILFixedDockingBar::PutInSlot(CPoint * point, INT32 Size)

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/3/94
	Inputs:		-
	Outputs:	pointer to new bar slot position
	Returns:	-
	Purpose:	snap an x or y coordinate to nearest slot
	Errors:		-
	SeeAlso:	

********************************************************************************************/

void OILFixedDockingBar::PutInSlot(CPoint* position, INT32 Size)
{
//	TRACE( _T("PutInSlot x - %d  y - %d\n"), position->x, position->y);
	
	INT32 PointSlot = PointToSlot(position);
//	TRACE( _T("PointSlot = %d\n"), PointSlot);
	INT32 SlotStart = TotalSlotSizes(PointSlot);
//	TRACE( _T("SlotStart = %d\n"), SlotStart);
	INT32 MidPoint = SlotStart + (SlotSize[PointSlot] / 2);

	INT32 BarMidPoint = 0;
//	TRACE( _T("MidPoint = %d\n"), MidPoint);
	INT32 SlotEnd = SlotStart + SlotSize[PointSlot];
//	TRACE( _T("SlotEnd = %d\n"), SlotEnd);

	switch (DockType)
   	{
 	case DOCKBAR_BOTTOM:						   	// snap y to nearest slot
	case DOCKBAR_TOP:
		BarMidPoint = position->y + (Size / 2);
		if (position->x < 0)
			position->x = 0; 

		if (position->y < MidPoint)
		{
	       	position->y = SlotStart;
		}
		else
		{
			position->y = SlotStart + SlotSize[PointSlot];
		}
 		break;

	case DOCKBAR_LEFT:						   	// snap y to nearest slot
	case DOCKBAR_RIGHT:
		BarMidPoint = position->x + (Size / 2);

		if (position->y < 0)
			position->y = 0; 

		if (BarMidPoint < SlotEnd)
		{
	       	position->x = SlotStart;
		}
		else
		{
			position->x = SlotStart + SlotSize[PointSlot];
		}
		
/*		if (position->x < MidPoint)
		{
	       	position->x = SlotStart;
		}
		else
		{
			position->x = SlotStart + SlotSize[PointSlot];
		}*/
 		break;
	}
//	TRACE( _T("NewPos x - %d  y - %d\n"), position->x, position->y);
}

/********************************************************************************************

>	OILFixedDockingBar::ShiftBarPos(CPoint * position, INT32 Amount)

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/3/94
	Inputs:			
	Outputs:	-
	Returns:	Success / Fail 
	Purpose:	Shift a bar by given amount - checks new position is in dock bounds 
				note this means the current visible size of the dock 	
	Errors:		-
	SeeAlso:	

********************************************************************************************/

BOOL OILFixedDockingBar::ShiftBarPos(CPoint * position, INT32 Amount)
{
	INT32 NewPos;

	switch (DockType)
   	{
	case DOCKBAR_LEFT:								// check that new position is within dockbar
	case DOCKBAR_RIGHT:
		NewPos = position->y + Amount;				// shift y position
		if (NewPos < 0 || NewPos >= DockSize.cy)	// test bounds
			return FALSE;							// outside dock
		position->y = NewPos;				 		// OK set position
    	break;

	case DOCKBAR_TOP:
	case DOCKBAR_BOTTOM:
		NewPos = position->x + Amount;
		if (NewPos < 0 || NewPos >= DockSize.cx)
			return FALSE;
		position->x = NewPos;				 		// OK set position
 		break;
	}

	return TRUE;
}

/********************************************************************************************

>	OILFixedDockingBar::ShiftBarSlot(CPoint * position, INT32 NoSlots)

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/5/94
	Inputs:			
	Outputs:	-
	Returns:	Success / Fail 
	Purpose:	Shift a bar by the given number of slots   	
	Errors:		-
	SeeAlso:	

********************************************************************************************/

BOOL OILFixedDockingBar::ShiftBarSlot(CPoint* position, INT32 NoSlots)
{
	INT32 StartSlot = PointToSlot(position);
	INT32 NewPos = TotalSlotSizes(NoSlots + StartSlot);

	switch (DockType)
   	{
	case DOCKBAR_LEFT:
	case DOCKBAR_RIGHT:
		position->x = NewPos;		// OK set position
    	break;

	case DOCKBAR_TOP:
	case DOCKBAR_BOTTOM:
		position->y = NewPos;	 	// OK set position
 		break;
	}

	return TRUE;
}

/********************************************************************************************

>	OILFixedDockingBar::CheckAndCorrectOverlap()

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/4/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	To tidy up any posible overlap after changing the size of the bars
				( between Small/Large controls etc..) 		
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void OILFixedDockingBar::CheckAndCorrectOverlap()
{
 	DoRecalc = FALSE ;
	INT32 	RightOfLastBar 	= 0;  
	INT32 	ThisBarStart = 0;
	INT32 	ThisSlot	= 0;
	INT32 	LastSlot	= 0;
	INT32 	AmountToMove = 0;
	BOOL	TidyUpSlots = FALSE ;
	INT32 index=0;

	OILChildBar * ThisChild = (OILChildBar *) ChildBars.GetHead();
	if (ThisChild == NULL)
		return ;
	CPoint ThisPos = ThisChild->GetPos();
	RightOfLastBar = PointToPosition( &ThisPos ) + ThisChild->GetExtent();
	LastSlot = PointToSlot( &ThisPos );	

	while (ThisChild != NULL)
	{
		// get next bar
		ThisChild = (OILChildBar *) ChildBars.GetNext( ThisChild );	

		// end of list 
		if (ThisChild == NULL)
			break;

		// get positional info on this bar
		ThisPos = ThisChild->GetPos();
		ThisBarStart = PointToPosition( &ThisPos );
		ThisSlot = PointToSlot( &ThisPos );

		if ( ThisSlot == LastSlot )
		{
			// calculate overlap
			AmountToMove = RightOfLastBar-ThisBarStart;

			// check for overlap
			if ( AmountToMove > 0 )	
			{
				// can we move this bar within this slot ?
				if (ShiftBarPos( &ThisPos, AmountToMove ))
				{
				 	ThisChild->SetPos( ThisPos );
				}
				else
				{
					// Insert a slot and move there
					InsertSlot( ThisSlot + 1 );
				 	ShiftBarSlot( &ThisPos,1 );
				 	ThisChild->SetPos( ThisPos );
					TidyUpSlots = TRUE;
					RightOfLastBar = ThisBarStart = 0;
				}
	 		}
		}
				
		RightOfLastBar = PointToPosition( &ThisPos ) + ThisChild->GetExtent();
    	LastSlot = ThisSlot;
	
	}

	if (TidyUpSlots)
		TidySlots();
	if (DoRecalc)
		((CFrameWnd*)Parent)->RecalcLayout();		
	DoRecalc = FALSE ;
}


/********************************************************************************************

>	OILFixedDockingBar::CanShuffleForward(OILChildBar * Start,OILChildBar * End, INT32 Amount)

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/3/94
	Inputs:		-
	Outputs:	-
	Returns:	Success / Fail if bar can be shuffled backward without going out of dock bounds
    			NB a bar can extend beyond the dock but can never be placed entirely outside
				its dock.
	Purpose:	determine  whether bars in this slot can be shuffled along without
				leaving the dock	 		
	Errors:		-
	SeeAlso:	CanShuffleBackward / CanShuffleBackward / DoShuffleForward

********************************************************************************************/

BOOL OILFixedDockingBar::CanShuffleForward(OILChildBar * Start,OILChildBar * End, INT32 Amount)
{

 	if (Start==NULL)
		return FALSE;

	INT32 	RightOfBar = 0;
	INT32 	LeftOfBar = 0;
	CPoint 	FoundPos;
	INT32 	AmountToMove = Amount;

	OILChildBar * ThisChild = Start;

	while (ThisChild != End)
	{

		FoundPos = ThisChild->GetPos();
		if (!ShiftBarPos(&FoundPos,AmountToMove))// check if we're shuffling out of bounds
			   return FALSE;					// we've run out of room 

  		RightOfBar = PointToPosition(&FoundPos)  + ThisChild->GetExtent();
		
		ThisChild =(OILChildBar *) ChildBars.GetNext(ThisChild);	

		if (ThisChild == End || ThisChild == NULL)
			LeftOfBar = RightOfBar;
		else
			LeftOfBar = PointToPosition(&(ThisChild->GetPos()));
	
		AmountToMove = RightOfBar - LeftOfBar;

		if (AmountToMove == 0)				  						// break if nothing to move
		  break;

	}
	if (AmountToMove > 0)
		return FALSE;
	return TRUE;
}

/********************************************************************************************

>	OILFixedDockingBar::DoShuffleForward(OILChildBar * Start,OILChildBar * End, INT32 Amount)

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/3/94
	Inputs:		-
	Outputs:	-
	Returns:	Success / Fail if bar would be placed below dockstart
	Purpose:	Attempts to make space in slot for a new bar by shuffling all bars to the left
				each iteration resets Amount to the new overlap with the previous bar	 			
	Errors:		-	
	SeeAlso:	CanShuffleBackward / CanShuffleBackward / DoShuffleForward

********************************************************************************************/

BOOL OILFixedDockingBar::DoShuffleForward(OILChildBar * Start,OILChildBar * End, INT32 Amount)
{

	if (Start==NULL)
		return FALSE;

	if (!CanShuffleForward(Start,End,Amount))
		return FALSE;

	INT32 RightOfBar;
	INT32 LeftOfBar;
	CPoint FoundPos;
	INT32 AmountToMove = Amount;					 	// set to initial overlap
	OILChildBar * ThisChild = Start;

	while (ThisChild!=End)
	{
	
		FoundPos=ThisChild->GetPos();

		if (ShiftBarPos(&FoundPos,AmountToMove))		//check for valid movement
			ThisChild->SetPos(FoundPos);	   		    //move the bar

		RightOfBar = PointToPosition(&FoundPos)  + ThisChild->GetExtent();

		ThisChild =(OILChildBar *) ChildBars.GetNext(ThisChild);	

		if (ThisChild == End || ThisChild == NULL)
			LeftOfBar = RightOfBar;
		else
			LeftOfBar = PointToPosition(&(ThisChild->GetPos()));

		AmountToMove = RightOfBar - LeftOfBar;			// find overlap	

		if (AmountToMove == 0)
		  break;								  	// break if nothing to move
	
	}
	if (AmountToMove>0)
		return FALSE;
	return TRUE;
}

/********************************************************************************************

>	OILFixedDockingBar::CanShuffleBackward(OILChildBar * Start,OILChildBar * End, INT32 Amount)

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/3/94
	Inputs:		-
	Outputs:	-
	Returns:	Success / Fail if bar can be shuffled backward without going out of dock bounds
				NB a bar can extend beyond the dock but can never be placed entirely outside
				its dock.
	Purpose:  	determine  whether bars in this slot can be shuffled along without
				leaving the dock	
	Errors:		-
	SeeAlso:	DoShuffleBackward / CanShuffleForward / DoShuffleForward	

********************************************************************************************/

BOOL OILFixedDockingBar::CanShuffleBackward(OILChildBar * Start,OILChildBar * End, INT32 Amount)
{
	
	if (Start==NULL)
		return FALSE;

	INT32 RightOfBar;
	INT32 LeftOfBar;
	CPoint FoundPos;
	INT32 AmountToMove=Amount;					  // set to initial overlap
	OILChildBar * ThisChild= Start;

	while (ThisChild!=End)
	{
	
		FoundPos=ThisChild->GetPos();
		if (!ShiftBarPos(&FoundPos,-AmountToMove))	//check for valid movement
			return FALSE;						 	//we've	run out of room

		LeftOfBar=PointToPosition(&FoundPos);
		ThisChild =(OILChildBar *) ChildBars.GetPrev(ThisChild);	
		if (ThisChild==NULL)						   // break if end of list
			break;

		RightOfBar = PointToPosition(&(ThisChild->GetPos()))  + ThisChild->GetExtent();

		AmountToMove=RightOfBar-LeftOfBar;
		if (AmountToMove<=0)						  // break if nothing to move
		  break;

	}
	return TRUE;
}

/********************************************************************************************

>	OILFixedDockingBar::DoShuffleBackward(OILChildBar * Start,OILChildBar * End, INT32 Amount)

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/3/94
	Inputs:		-
	Outputs:	-
	Returns:	Success/Fail if bar has been shuffled forwards 
	Purpose: 	Move all bars in this slot <<<<			
	Errors:		-
	SeeAlso:	CanShuffleBackward / CanShuffleForward / DoShuffleForward	

********************************************************************************************/

BOOL OILFixedDockingBar::DoShuffleBackward(OILChildBar * Start,OILChildBar * End, INT32 Amount)
{

	if (Start==NULL)
		return FALSE;

	if (!CanShuffleBackward(Start,End,Amount))
		return FALSE;

	INT32 RightOfBar;
	INT32 LeftOfBar;
	CPoint FoundPos;
	INT32 AmountToMove=Amount;
	OILChildBar * ThisChild= Start;

	while (ThisChild!=End)
	{

		FoundPos=ThisChild->GetPos();
		if (ShiftBarPos(&FoundPos,-AmountToMove))   // Check valid move
			ThisChild->SetPos(FoundPos);		   // Move the bar

		LeftOfBar=PointToPosition(&(ThisChild->GetPos()));
		ThisChild =(OILChildBar *) ChildBars.GetPrev(ThisChild);	
		if (ThisChild==NULL)					 // break if end of list
			break;

		RightOfBar = PointToPosition(&(ThisChild->GetPos()))  + ThisChild->GetExtent();

		AmountToMove=RightOfBar-LeftOfBar;
		if (AmountToMove<=0)					 // break if nothing to move
		  break;

	}
	return TRUE;
}

 /********************************************************************************************

>	OILFixedDockingBar::GetOverLap(OILChildBar * ThisBar,INT32 SlotNo, INT32 * ForwardOverlap,
									INT32 * BackwardOverlap)

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/3/94
	Inputs:		New ChildBar - Slot No 
	Outputs:	ForwardOverlap BackwardOverlap
	Returns:	Pointer to nearest bar if found 
	Purpose:	
	Errors:		
	SeeAlso:	

********************************************************************************************/

OILChildBar * OILFixedDockingBar::GetOverLap(OILChildBar * ThisBar,INT32 SlotNo,
											INT32 * ForwardOverlap,INT32 * BackwardOverlap)
{
	ENSURE(SlotNo >= 0 && SlotNo < NoOfSlots,"Slot Out Of Range");

	CPoint FoundPos;							// current bar pos
	OILChildBar * StartSlot = GetSlotHead(SlotNo);// get pointers to head of this slot and next	
	OILChildBar *StartNextSlot = GetSlotHead(SlotNo+1);

	// failed to find any bars on this slot
	if (StartSlot == NULL)
		return FALSE;
	
	INT32 CheckPos = 0;																					// position of current bar in list
	INT32 ThisPos=PointToPosition(&(ThisBar->GetPos()));// initial position of new bar
	OILChildBar * ThisChild = StartSlot;			// Set to start of first bar in this slot
	OILChildBar * FoundNearest = NULL;			   // will return a pointer to the closest bar 
	*BackwardOverlap = 0;						   // initalise Overlaps
	*ForwardOverlap = 0;

	// check the list until	we get to the end of the list / get to the end of this slot 
	// or find a bar with a higher position than the new one
	while (TRUE)
	{
		//TRACE( _T("Check %d against %d ThisChild %p"),ThisPos,CheckPos,ThisChild);
		if (ThisChild != NULL)
		{
			CheckPos = PointToPosition(&(ThisChild->GetPos()));
		}
		if (ThisPos<CheckPos||ThisChild == StartNextSlot||ThisChild == NULL)
		{
			// Ok we've gone past it
			if (ThisChild == NULL)	 // check if we're past the end of the list
			{
				ThisChild = (OILChildBar *) ChildBars.GetTail();
			}																															   
			else
			{
				ThisChild =(OILChildBar *) ChildBars.GetPrev(ThisChild);
			}                                                       
			if (ThisChild==NULL)			// Special case -  front of list
			{
				*BackwardOverlap = 0;
				FoundNearest = NULL;
            	ThisChild =(OILChildBar *) ChildBars.GetHead( );
			}
			else						// Check for backwards overlap
			{
				// get bottom right corner of current bar
				FoundPos = ThisChild->GetPos();
				CheckPos = PointToPosition(&FoundPos)+ ThisChild->GetExtent();
				// keep a pointer to found bar 
				FoundNearest = ThisChild;
				//TRACE( _T("FOUND BAR  %d on slot %d"),PointToPosition(FoundNearest->GetPos()),
				//	PointToSlot(FoundNearest->GetPos()));
				*BackwardOverlap = CheckPos - ThisPos  ;
				ThisChild =(OILChildBar *) ChildBars.GetNext(ThisChild);
			}				
			// is there another bar to the right in this slot	
			if (ThisChild != NULL && ThisChild != StartNextSlot)
			{ 
				// if so check for forward overlap
				FoundPos = ThisBar->GetPos() ;
				CheckPos = PointToPosition(&FoundPos)+ ThisBar->GetExtent();
				INT32 NextPos	= PointToPosition(&(ThisChild->GetPos()));
 				if (CheckPos>NextPos)
					  *ForwardOverlap = CheckPos-NextPos  ;
			}
			break;  // break from while loop if nearest bar or end of list found 
		}
		ThisChild =(OILChildBar *) ChildBars.GetNext(ThisChild);	
	}// end while

	if (*BackwardOverlap < 0)
		*BackwardOverlap = 0;

	if (*ForwardOverlap < 0)
		* ForwardOverlap = 0;
	return FoundNearest;
}



/****************************************************************************

>	OILChildBar* OILFixedDockingBar::GetGalleryOverLap(OILChildBar* ThisBar,
					INT32 SlotNo,INT32 * ForwardOverlap,INT32 * BackwardOverlap)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/2004

	Inputs:		ThisBar		- pointer to a OILChildBar
				SlotNo		- 
				ForwardOverlap- pointer to a INT32 
				BackwardOverlap- pointer to a INT32 
	Returns:	NULL in times of grief
	Purpose:	

****************************************************************************/

BOOL OILFixedDockingBar::GetGalleryOverLap(OILChildBar* ThisBar,INT32 SlotNo,
											INT32 * ForwardOverlap,INT32 * BackwardOverlap,
											OILChildBar** ppPrevious, OILChildBar** ppNext)
{
//	ENSURE(SlotNo >= 0 && SlotNo < NoOfSlots,"Slot Out Of Range");

	CPoint FoundPos;							// current bar pos

	*BackwardOverlap = 0;						   // initalise Overlaps
	*ForwardOverlap = 0;
	if (ppPrevious)
		*ppPrevious = NULL;
	if (ppNext)
		*ppNext = NULL;

	INT32 ThisMin = ThisBar->GetKPPos();				// initial position of new bar
	INT32 ThisMax = ThisMin + ThisBar->GetExtent();	// initial position of new bar

//	TRACE( _T("ThisMin = %d\n"), ThisMin);

	OILChildBar* StartSlot = GetSlotHead(SlotNo); // get pointer to head of this slot
	// failed to find any bars on this slot
	if (StartSlot == NULL)
	{
		*BackwardOverlap = -ThisMin;
		*ForwardOverlap = ThisMax - DockSize.cy;
		return TRUE;
	}

	OILChildBar* CheckBar = StartSlot;				// Set to start of first bar in this slot

	// check the list until	we get to the end of the list / get to the end of this slot 
	// or find a bar with a higher position than the new one
	while (CheckBar && (CheckBar->GetKPSlot() == SlotNo) &&
			(CheckBar == ThisBar || CheckBar->GetKPPos() < ThisMin))
	{
		CheckBar = (OILChildBar*)ChildBars.GetNext(CheckBar);
	}

//	if (CheckBar)
//	{
//		TRACE( _T("CheckBar at %d\n"), CheckBar->GetKPPos());
//	}
//	else
//	{
//		TRACE( _T("CheckBar is NULL\n"));
//	}

	// At this point we must be in one of the following states
	OILChildBar* PreviousBar = CheckBar ? (OILChildBar*)ChildBars.GetPrev(CheckBar) : (OILChildBar*)ChildBars.GetTail();
	if (PreviousBar == ThisBar)
		PreviousBar = (OILChildBar*)ChildBars.GetPrev(PreviousBar);
	if (PreviousBar && PreviousBar->GetKPSlot() != SlotNo)
		PreviousBar = NULL;

//	if (PreviousBar)
//	{
//		TRACE( _T("PreviousBar at %d\n"), PreviousBar->GetKPPos());
//	}
//	else
//	{
//		TRACE( _T("PreviousBar is NULL\n"));
//	}

	if (!CheckBar || CheckBar->GetKPSlot() != SlotNo)
	{
		// CheckBar is NULL implies that the previous bar is in this slot
		// CheckBar->GetKPSlot() != SlotNo implies the same as above
		// Calculate forward overlap from extent of dock (no bars after us in slot)
		// Calculate backward overlap from PreviousBar

		*ForwardOverlap = ThisMax - DockSize.cy;
		if (PreviousBar)
		{
			if (ppPrevious)
				*ppPrevious = PreviousBar;
			*BackwardOverlap = (PreviousBar->GetKPPos() + PreviousBar->GetExtent()) - ThisMin;
		}
		else
		{
			*BackwardOverlap = -ThisMin;
		}
	}
	else
	{
		// Calculate forward overlap with CheckBar
		// Calculate backward overlap with PreviousBar or from zero if NULL or wrong slot

		if (ppNext)
			*ppNext = CheckBar;
		*ForwardOverlap = ThisMax - CheckBar->GetKPPos();

		if (PreviousBar)
		{
			if (ppPrevious)
				*ppPrevious = PreviousBar;
			*BackwardOverlap = (PreviousBar->GetKPPos() + PreviousBar->GetExtent()) - ThisMin;
		}
		else
		{
			*BackwardOverlap = -ThisMin;
		}
	}

	return TRUE;
}


/********************************************************************************************

>	OILFixedDockingBar::PlaceBarInSlot(OILChildBar * ThisBar)

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/3/94
	Inputs:		New ChildBar - Slot No
	Outputs:	-
	Returns:	Success / Fail 
	Purpose:	attempts to shuffle bars in a given slot allowing a new bar to be placed with
				no overlap
	Errors:		Assert ThisBar != NULL
	SeeAlso:	

********************************************************************************************/
BOOL OILFixedDockingBar::PlaceBarInSlot(OILChildBar * ThisBar, INT32 SlotNo)
{
	ENSURE(ThisBar != NULL,"Trying To Place A Null Bar");
	ENSURE(SlotNo >= 0 && SlotNo < NoOfSlots,"Slot Out of Range");

	CPoint FoundPos;									// current bar pos
	CPoint LastPos ;									// copy of last pos
	// get pointers to head of this slot and next
	OILChildBar * StartSlot=GetSlotHead(SlotNo);
	OILChildBar * StartNextSlot=GetSlotHead(SlotNo+1);
	BOOL DoneShuffleForward = FALSE;
	if (StartSlot==NULL)
		return FALSE;									// failed to find any bars on this slot

	INT32 ThisPos=PointToPosition(&(ThisBar->GetPos()));		// initial position in slot
	INT32 BackwardOverlap ;								 
	INT32 ForwardOverlap ;
	INT32 ForwardDone	;
	// get a pointer to the nearest bar and overlaps on either side
	OILChildBar * FoundNearest=GetOverLap(ThisBar,SlotNo,&ForwardOverlap,&BackwardOverlap);
	
	ForwardDone = ForwardOverlap;

	if (FoundNearest==NULL) 	// trying to insert at the front of a slot
	{
		FoundPos=ThisBar->GetPos();
		// can we place the new bar in the space at the front of the slot
		if (ShiftBarPos(&FoundPos,-ForwardOverlap))
			ThisBar->SetPos(FoundPos);
		// if not can we shuffle the bars in the slot to make space
		else 
		{
			if (!(DoneShuffleForward =DoShuffleForward(StartSlot,StartNextSlot, ForwardOverlap)))
				return FALSE;    	//shuffle failed
		}
	}
	else	// trying to insert between two bars
	{
	
		/*TRACE( _T("Insert between bars %d on slot %d back overlap %d forward %d"),
			PointToPosition(FoundNearest->GetPos()),PointToSlot(FoundNearest->GetPos()),
							BackwardOverlap,ForwardOverlap);
		*/					
		if (ForwardOverlap > 0)
		{
			// try to shuffle all bars in this slot  ahead of new bar to make space
			if (!(DoneShuffleForward =DoShuffleForward((OILChildBar *) ChildBars.GetNext(FoundNearest),
								 StartNextSlot, ForwardOverlap))) 
    	    {
				LastPos = FoundPos=ThisBar->GetPos();
				//TRACE( _T("failed ->>> shift -  moving <<<- and attempting whole shuffle <<<-"));
				if (ShiftBarPos(&FoundPos,-ForwardOverlap))
				{
					ThisBar->SetPos(FoundPos);
					FoundNearest = GetOverLap(ThisBar,SlotNo,&ForwardOverlap,&BackwardOverlap);
					if (BackwardOverlap > 0)
					{
						if (!DoShuffleBackward(FoundNearest,
											(OILChildBar *)ChildBars.GetPrev(StartSlot),
											BackwardOverlap))
						{
							// return to state before failure
							ThisBar->SetPos(LastPos);
							return FALSE;
						}
					}
				}
				else
				{	
					// return to state before failure
					ThisBar->SetPos(LastPos); 
					return FALSE;
				}
				return FALSE;
			}
    	}// end if ForwardOverlap>0
		
		
		if (BackwardOverlap > 0)
		{
			// try to shuffle all bars in this slot  behind new bar to make space
			if (!DoShuffleBackward(FoundNearest,(OILChildBar *) ChildBars.GetPrev(StartSlot),
								  BackwardOverlap))
			{
			 	LastPos = FoundPos = ThisBar->GetPos();
				//TRACE( _T("failed <<<- shift -  moving->>> and attempting whole shuffle ->>>"));
				if (ShiftBarPos(&FoundPos,BackwardOverlap))
				{
					ThisBar->SetPos(FoundPos);
					FoundNearest = GetOverLap(ThisBar,SlotNo,&ForwardOverlap,&BackwardOverlap);
					if (ForwardOverlap > 0)
					{
						if (!DoShuffleForward((OILChildBar *) ChildBars.GetNext(FoundNearest),
									 StartNextSlot, ForwardOverlap))
						{
							// return to state before fail - we may have shuffled the bars >>> 
							ThisBar->SetPos(LastPos);
							if (DoneShuffleForward)
								DoShuffleBackward((OILChildBar *) ChildBars.GetNext(FoundNearest),
							                   	 StartNextSlot, ForwardDone);
							return FALSE;
						}
					}
				}
				else 
				{
					// return to state before fail
					ThisBar->SetPos(LastPos);
					if (DoneShuffleForward)
						DoShuffleForward((OILChildBar *) ChildBars.GetNext(FoundNearest),
				            			 StartNextSlot, ForwardDone);
					return FALSE;
				}
			} 
		}// end if BackwardOverlap>0

	}// end else  (FoundNearest != NULL)		
	return TRUE;
}



/****************************************************************************

>	BOOL OILFixedDockingBar::PlaceGalleryInSlot(OILChildBar * ThisBar, INT32 SlotNo)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/2004

	Inputs:		ThisBar		- pointer to a OILChildBar 
				SlotNo		- 
	Returns:	TRUE if ok, FALSE if bother
	Purpose:	

****************************************************************************/

BOOL OILFixedDockingBar::PlaceGalleryInSlot(OILChildBar * ThisBar, INT32 SlotNo)
{
	ENSURE(ThisBar != NULL,"Trying To Place A Null Bar");
//	ENSURE(SlotNo >= 0 && SlotNo < NoOfSlots,"Slot Out of Range");

	if (!allowTidyBars)
		return(TRUE);

	CPoint FoundPos;									// current bar pos
	CPoint LastPos ;									// copy of last pos
	BOOL DoneShuffleForward = FALSE;

	INT32 ThisPos = ThisBar->GetKPPos();		// initial position in slot
	// get a pointer to the nearest bars and overlaps on either side
	INT32 BackwardOverlap ;								 
	INT32 ForwardOverlap ;
	OILChildBar* PreviousBar = NULL;
	OILChildBar* NextBar = NULL;

	GetGalleryOverLap(ThisBar,SlotNo,&ForwardOverlap,&BackwardOverlap, &PreviousBar, &NextBar);
//	TRACE( _T("GalleryOverlap - Back = %d, Fore = %d, Prev = %p, Next = %p\n"), BackwardOverlap, ForwardOverlap, PreviousBar, NextBar);

	// If both Next and Previous are NULL then this is the only bar in the slot and we can shortcut
	if (!PreviousBar && !NextBar)
	{
		ThisBar->GetBarPos()->Position = 0;
		ThisBar->UpdatePos();
		::SendMessage(ThisBar->GetHwnd(), WM_SETGALLERYSIZE, ThisBar->GetBarSize(), DockSize.cy);
		return(TRUE);
	}

	// First off we scan through all the bars in the slot and 
	// shuffle all the bars away from this one

	BOOL bMoved = FALSE;
	OILChildBar* FirstBar = GetSlotHead(SlotNo);
	OILChildBar* LastBar = NULL;		// pointer to last bar in slot
	OILChildBar* TempBar = FirstBar;
	while (TempBar && TempBar->GetKPSlot() == SlotNo)
	{
		LastBar = TempBar;
		TempBar = (OILChildBar*)ChildBars.GetNext(TempBar);
	}
	
	if (PreviousBar)
	{
		OILChildBar* MoveBar = FirstBar;
		OILChildBar* EndBar = (OILChildBar*)ChildBars.GetNext(PreviousBar);
		INT32 CurPos = 0;
		while (MoveBar && MoveBar != EndBar)
		{
			if (MoveBar->GetBarPos()->Position != CurPos)
			{
				MoveBar->GetBarPos()->Position = CurPos;
				MoveBar->UpdatePos();
				bMoved = TRUE;
			}
			CurPos += MoveBar->GetExtent();
			MoveBar = (OILChildBar*)ChildBars.GetNext(MoveBar);
		}
	}

	if (NextBar)
	{
		OILChildBar* MoveBar = LastBar;
		OILChildBar* EndBar = (OILChildBar*)ChildBars.GetPrev(NextBar);
		INT32 CurEnd = DockSize.cy;
		while (MoveBar && MoveBar != EndBar)
		{
			INT32 CurPos = CurEnd - MoveBar->GetExtent();
			if (MoveBar->GetBarPos()->Position != CurPos)
			{
				MoveBar->GetBarPos()->Position = CurPos;
				MoveBar->UpdatePos();
				bMoved = TRUE;
			}
			CurEnd = CurPos;
			MoveBar = (OILChildBar*)ChildBars.GetPrev(MoveBar);
		}
	}

	if (bMoved)
	{
		GetGalleryOverLap(ThisBar,SlotNo,&ForwardOverlap,&BackwardOverlap, &PreviousBar, &NextBar);
//		TRACE( _T("GalleryOverlapAfterShuffle - Back = %d, Fore = %d, Prev = %p, Next = %p\n"), BackwardOverlap, ForwardOverlap, PreviousBar, NextBar);
	}

	// If Previous is NULL and there is a backward overlap
	if (!PreviousBar && BackwardOverlap != 0)
	{
		// Move and resize this bar to fill the start of the slot
		ThisBar->GetBarPos()->Position = 0;
		ThisBar->UpdatePos();
		::SendMessage(ThisBar->GetHwnd(), WM_SETGALLERYSIZE, ThisBar->GetBarSize(), ThisBar->GetExtent() - BackwardOverlap);

		GetGalleryOverLap(ThisBar,SlotNo,&ForwardOverlap,&BackwardOverlap, &PreviousBar, &NextBar);
//		TRACE( _T("GalleryOverlapNoPrev - Back = %d, Fore = %d, Prev = %p, Next = %p\n"), BackwardOverlap, ForwardOverlap, PreviousBar, NextBar);
	}

	// If Next is NULL and there is a forward overlap
	if (!NextBar && ForwardOverlap != 0)
	{
		// Resize this bar to fill the end of the slot
		::SendMessage(ThisBar->GetHwnd(), WM_SETGALLERYSIZE, ThisBar->GetBarSize(), ThisBar->GetExtent() - ForwardOverlap);
		INT32 TempBack = 0;
		INT32 TempFor = 0;
		GetGalleryOverLap(ThisBar,SlotNo,&TempFor,&TempBack, NULL, NULL);
		if (TempFor > 0)
		{
			ThisBar->GetBarPos()->Position -= TempFor;
			ThisBar->UpdatePos();
		}
		GetGalleryOverLap(ThisBar,SlotNo,&ForwardOverlap,&BackwardOverlap, &PreviousBar, &NextBar);
//		TRACE( _T("GalleryOverlapNoNext - Back = %d, Fore = %d, Prev = %p, Next = %p\n"), BackwardOverlap, ForwardOverlap, PreviousBar, NextBar);
	}

	// If Previous is not NULL then the bars before need to be resized to fit in the available space
	if (BackwardOverlap != 0)
	{
		if (PreviousBar)
		{
			if (BackwardOverlap > 0)
			{
				INT32 AmountToShrink = BackwardOverlap;
				INT32 AmountToMove = 0;
				OILChildBar* ShrinkBar = PreviousBar;
				while (AmountToShrink > 0 && ShrinkBar && ShrinkBar->GetKPSlot() == SlotNo)
				{
					INT32 OldExt = ShrinkBar->GetExtent();
					::SendMessage(ShrinkBar->GetHwnd(), WM_SETGALLERYSIZE, ShrinkBar->GetBarSize(), ShrinkBar->GetExtent() - AmountToShrink);
					INT32 AmountShrunk = OldExt - ShrinkBar->GetExtent();
					AmountToShrink -= AmountShrunk;
					if (AmountToShrink > 0)
					{
						if (ShrinkBar->GetBarPos()->Position > 0)
						{
							AmountToMove = (ShrinkBar->GetBarPos()->Position < AmountToShrink) ? ShrinkBar->GetBarPos()->Position : AmountToShrink;
							ShrinkBar->GetBarPos()->Position -= AmountToMove;
							ShrinkBar->UpdatePos();
							AmountToShrink = AmountToMove;
						}
					}
					ShrinkBar = (OILChildBar*)ChildBars.GetPrev(ShrinkBar);
				}

				if (AmountToShrink > 0)
				{
					// We still have some backward overlap so set the first bar to 0 
					// and move each bar upto This by the forward overlap
					OILChildBar* MoveBar = GetSlotHead(SlotNo);
					OILChildBar* EndBar = (OILChildBar*)ChildBars.GetNext(PreviousBar);
					INT32 TempBack = 0;
					INT32 TempFor = 0;
					MoveBar->GetBarPos()->Position = 0;
					MoveBar->UpdatePos();
					MoveBar = (OILChildBar*)ChildBars.GetNext(MoveBar);
					while (MoveBar && MoveBar != EndBar)
					{
						GetGalleryOverLap(MoveBar,SlotNo,&TempFor,&TempBack, NULL, NULL);
						if (TempBack != 0)
						{
							MoveBar->GetBarPos()->Position += TempBack;
							MoveBar->UpdatePos();
						}
						MoveBar = (OILChildBar*)ChildBars.GetNext(MoveBar);
					}

					GetGalleryOverLap(ThisBar,SlotNo,&TempFor,&TempBack, NULL, NULL);
					ThisBar->GetBarPos()->Position += TempBack;
					ThisBar->UpdatePos();
					GetGalleryOverLap(ThisBar,SlotNo,&ForwardOverlap,&BackwardOverlap, &PreviousBar, &NextBar);
//					TRACE( _T("GalleryOverlapShrunkPrev - Back = %d, Fore = %d, Prev = %p, Next = %p\n"), BackwardOverlap, ForwardOverlap, PreviousBar, NextBar);
				}
			}
			else
			{
				// Need to fill up the gap
				::SendMessage(PreviousBar->GetHwnd(), WM_SETGALLERYSIZE, PreviousBar->GetBarSize(), PreviousBar->GetExtent() - BackwardOverlap);
			}
		}
		else
		{
			// No windows before so move position and size of ThisBar to fill the start of the slot
			ThisBar->GetBarPos()->Position = 0;
			ThisBar->UpdatePos();
			INT32 OldExt = ThisBar->GetExtent();
			::SendMessage(ThisBar->GetHwnd(), WM_SETGALLERYSIZE, ThisBar->GetBarSize(), ThisBar->GetExtent() - BackwardOverlap);
			INT32 NewExt = ThisBar->GetExtent();
			if (NewExt != (OldExt - BackwardOverlap))
			{
//				TRACE( _T("Failed to fit ThisBar ar start of slot\n"));
			}
		}
	}

	// If Next is not NULL then the bars after need to be resized to fit in the available space
	if (ForwardOverlap != 0)
	{
		if (NextBar)
		{
			if (ForwardOverlap > 0)
			{
				INT32 AmountToShrink = ForwardOverlap;
				INT32 AmountToMove = 0;
				OILChildBar* ShrinkBar = NextBar;
				while (AmountToShrink > 0 && ShrinkBar && ShrinkBar->GetKPSlot() == SlotNo)
				{
					INT32 OldExt = ShrinkBar->GetExtent();
					::SendMessage(ShrinkBar->GetHwnd(), WM_SETGALLERYSIZE, ShrinkBar->GetBarSize(), ShrinkBar->GetExtent() - AmountToShrink);
					INT32 AmountShrunk = OldExt - ShrinkBar->GetExtent();
					
					ShrinkBar->GetBarPos()->Position += AmountToShrink;
					ShrinkBar->UpdatePos();
					AmountToShrink -= AmountShrunk;

					ShrinkBar = (OILChildBar*)ChildBars.GetNext(ShrinkBar);
				}

				if (AmountToShrink > 0)
				{
					// We still have some forward overlap so shrink this bar
					// and move each bar upto the end to fill the gaps
					// This means that all bars after this are the smallest possible size

					// If we can't shrink this bar enough then loop backwards from PreviousBar
					// shrinking each bar in turn until enough space has been made
					
					INT32 OldExt = ThisBar->GetExtent();
					::SendMessage(ThisBar->GetHwnd(), WM_SETGALLERYSIZE, ThisBar->GetBarSize(), ThisBar->GetExtent() - AmountToShrink);
					INT32 AmountShrunk = OldExt - ThisBar->GetExtent();
					AmountToShrink -= AmountShrunk;

					if (AmountToShrink > 0)
					{
						ThisBar->GetBarPos()->Position -= AmountToShrink;
						ThisBar->UpdatePos();
						
						OILChildBar* ShrinkBar = PreviousBar;
						while (ShrinkBar && ShrinkBar->GetKPSlot() == SlotNo && AmountToShrink > 0)
						{
							INT32 OldExt = ShrinkBar->GetExtent();
							::SendMessage(ShrinkBar->GetHwnd(), WM_SETGALLERYSIZE, ShrinkBar->GetBarSize(), ShrinkBar->GetExtent() - AmountToShrink);
							INT32 AmountShrunk = OldExt - ShrinkBar->GetExtent();
							if (AmountShrunk > 0)
							{
								AmountToShrink -= AmountShrunk;
								// Shift all bars upto nextbar by AmountShrunk
								OILChildBar* MoveBar = (OILChildBar*)ChildBars.GetNext(ShrinkBar);
								while (MoveBar && MoveBar != NextBar)
								{
									MoveBar->GetBarPos()->Position -= AmountShrunk;
									MoveBar->UpdatePos();
									MoveBar = (OILChildBar*)ChildBars.GetNext(MoveBar);
								}
							}
							ShrinkBar = (OILChildBar*)ChildBars.GetPrev(ShrinkBar);
						}
					}

					INT32 TempBack = 0;
					INT32 TempFor = 0;
					OILChildBar* LastBar = NULL;
					OILChildBar* MoveBar = NextBar;
					MoveBar->GetBarPos()->Position = ThisBar->GetBarPos()->Position + ThisBar->GetExtent();
					MoveBar->UpdatePos();
					MoveBar = (OILChildBar*)ChildBars.GetNext(MoveBar);
					while (MoveBar && MoveBar->GetKPSlot() == SlotNo)
					{
						GetGalleryOverLap(MoveBar,SlotNo,&TempFor,&TempBack, NULL, NULL);
						if (TempBack != 0)
						{
							MoveBar->GetBarPos()->Position += TempBack;
							MoveBar->UpdatePos();
						}
						LastBar = MoveBar;
						MoveBar = (OILChildBar*)ChildBars.GetNext(MoveBar);
					}

					if (LastBar && TempFor + TempBack > 0)
					{
						::SendMessage(LastBar->GetHwnd(), WM_SETGALLERYSIZE, LastBar->GetBarSize(), LastBar->GetExtent() - (TempFor + TempBack));
					}
				}
		
			}
			else
			{
				// There is a gap before the next bar so move and resize the next bar to
				// fill it up
				NextBar->GetBarPos()->Position += ForwardOverlap;
				NextBar->UpdatePos();
				::SendMessage(NextBar->GetHwnd(), WM_SETGALLERYSIZE, NextBar->GetBarSize(), NextBar->GetExtent() - ForwardOverlap);
			}
		}
		else
		{
			// No windows after so change size of ThisBar to fill the end of the slot
			::SendMessage(ThisBar->GetHwnd(), WM_SETGALLERYSIZE, ThisBar->GetBarSize(), ThisBar->GetExtent() - ForwardOverlap);
		}
	}

	return TRUE;
}




/********************************************************************************************

>	OILFixedDockingBar::TidySlots()

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/3/94
	Inputs:		-
	Outputs:	-
	Returns:	- 
	Purpose:	move bars to fill any empty slots  
	Errors:		-
	SeeAlso:	

********************************************************************************************/

void OILFixedDockingBar::TidySlots()
{	 
	INT32 LastSlot = -1;
	INT32 FoundSlot = 0;
	INT32 NumSlots = NoOfSlots;					// Remember original number of slots
	CPoint FoundPos;							// current bar pos

	if (allowTidyBars)
	{
/*		OILChildBar* FoundBar = (OILChildBar*)ChildBars.GetHead();
   		while (FoundBar != NULL)
		{
			FoundPos = FoundBar->GetPos();
			FoundSlot = PointToSlot(&FoundPos);
			if (FoundSlot > LastSlot+1)		// check for empty slots 
			{
				LastSlot ++;				   	// skip last none empty slot
				while (LastSlot < FoundSlot)
				{			
					RemoveSlot(LastSlot++);	// remove empty slots
//					LastSlot++;
				}
			}

			LastSlot = FoundSlot;
			FoundBar = (OILChildBar*)ChildBars.GetNext(FoundBar);
		} // end while
*/
/*
				INT32 TempF = 0;
				INT32 TempB = 0;
				OILChildBar* TempPrev = NULL;
				OILChildBar* TempNext = NULL;
				GetGalleryOverLap(NextBar, SlotNo, &TempF, &TempB, &TempPrev, &TempNext);
				if (TempF > 0)
				{
					// Next bar is overlapping forward
					// Shrink ThisBar by overlap and move next backwards
					NextBar->GetBarPos()->Position -= TempF;
					NextBar->UpdatePos();
					::SendMessage(ThisBar->GetHwnd(), WM_SETGALLERYSIZE, ThisBar->GetBarSize(), ThisBar->GetExtent() - TempF);
				}
*/		
		
		
		// Reset this to correct value for current bars
		NoOfSlots = CountSlots();
		// For each slot we do a tidy on the slot
		for (FoundSlot = 0; FoundSlot < NoOfSlots; FoundSlot++)
		{
			OILChildBar* CheckBar = GetSlotHead(FoundSlot); // get pointer to head of this slot
			if (CheckBar)
			{
				if (SlotSize[FoundSlot] != ControlSize)
				{
					OILChildBar* ThisBar = CheckBar;

					// Now check through again in reverse order
					OILChildBar* LastBar = NULL;		// pointer to last bar in slot
					while (ThisBar && ThisBar->GetKPSlot() == FoundSlot)
					{
						LastBar = ThisBar;
						ThisBar = (OILChildBar*)ChildBars.GetNext(ThisBar);
					}

					ThisBar = LastBar;
					// check the list until	we get to the end of the list / get to the end of this slot 
					while (ThisBar && (ThisBar->GetKPSlot() == FoundSlot))
					{
						INT32 BackwardOverlap = 0;
						INT32 ForwardOverlap = 0;
						OILChildBar* PreviousBar = NULL;
						OILChildBar* NextBar = NULL;

						GetGalleryOverLap(ThisBar,FoundSlot,&ForwardOverlap,&BackwardOverlap, &PreviousBar, &NextBar);
//						TRACE( _T("TidyOverlapB - This = %p, Back = %d, Fore = %d, Prev = %p, Next = %p\n"), ThisBar, BackwardOverlap, ForwardOverlap, PreviousBar, NextBar);

						BOOL bCheck = FALSE;
						// Move the bar by BackwardOverlap
						if (BackwardOverlap != 0)
						{
							ThisBar->GetBarPos()->Position += BackwardOverlap;
							ThisBar->UpdatePos();
							bCheck = TRUE;
						}
						// Resize the bar by (-ForwardOverlap-BackwardOverlap)
						INT32 SizeDiff = -(ForwardOverlap+BackwardOverlap);
						if (SizeDiff != 0)
						{
							::SendMessage(ThisBar->GetHwnd(), WM_SETGALLERYSIZE, ThisBar->GetBarSize(), ThisBar->GetExtent() + SizeDiff);
							bCheck = TRUE;
						}						

						if (bCheck)
						{
							GetGalleryOverLap(ThisBar,FoundSlot,&ForwardOverlap,&BackwardOverlap, &PreviousBar, &NextBar);
//							TRACE( _T("CheckOverlap - This = %p, Back = %d, Fore = %d, Prev = %p, Next = %p\n"), ThisBar, BackwardOverlap, ForwardOverlap, PreviousBar, NextBar);
							if (ForwardOverlap > 0)
							{
								// Move the bar backwards by ForwardOverlap
								ThisBar->GetBarPos()->Position -= ForwardOverlap;
								ThisBar->UpdatePos();
							}
						}

						ThisBar = (OILChildBar*)ChildBars.GetPrev(ThisBar);
					}
		


					// First we do a forward scan removing any overlaps as we go
					// check the list until	we get to the end of the list / get to the end of this slot 
					ThisBar = CheckBar;
					while (ThisBar && (ThisBar->GetKPSlot() == FoundSlot))
					{
						INT32 BackwardOverlap = 0;
						INT32 ForwardOverlap = 0;
						OILChildBar* PreviousBar = NULL;
						OILChildBar* NextBar = NULL;

						GetGalleryOverLap(ThisBar,FoundSlot,&ForwardOverlap,&BackwardOverlap, &PreviousBar, &NextBar);
//						TRACE( _T("TidyOverlapF - This = %p, Back = %d, Fore = %d, Prev = %p, Next = %p\n"), ThisBar, BackwardOverlap, ForwardOverlap, PreviousBar, NextBar);

						// Move the bar by BackwardOverlap
						if (BackwardOverlap != 0)
						{
							ThisBar->GetBarPos()->Position += BackwardOverlap;
							ThisBar->UpdatePos();
						}
						// Resize the bar by (-ForwardOverlap-BackwardOverlap)
						INT32 SizeDiff = -(ForwardOverlap+BackwardOverlap);
						if (SizeDiff != 0)
						{
							::SendMessage(ThisBar->GetHwnd(), WM_SETGALLERYSIZE, ThisBar->GetBarSize(), ThisBar->GetExtent() + SizeDiff);
						}						

						ThisBar = (OILChildBar*)ChildBars.GetNext(ThisBar);
					}

				
				}
			}
			else if (FoundSlot < NoOfSlots)
			{
				RemoveSlot(FoundSlot--);		// No bars so remove the slot and go back a slot
				NoOfSlots--;
			}
		}
	}
	
	INT32 SlotsNow = CountSlots();	// check if number of slots has changed

	if (SlotsNow != NumSlots)		// and RecalcLayout if it has
	{
		NoOfSlots = SlotsNow;
		DoRecalc = TRUE;
 	}	
	else
	{
		OILChildBar * ThisChild =(OILChildBar *) ChildBars.GetHead(); 
   	
   		while (ThisChild != NULL)
		{
			ThisChild->InvalidateIfMoved();
			ThisChild =(OILChildBar *) ChildBars.GetNext(ThisChild);	
	
		}// end while
	}
}


/********************************************************************************************

>	OILFixedDockingBar::CountSlots()

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/3/94
	Inputs:		-
	Outputs:	-
	Returns:	number of slots in this dock
	Purpose:	Count how many slots are in this bar 
	Errors:		-
	SeeAlso:	

********************************************************************************************/

INT32 OILFixedDockingBar::CountSlots()
{
	if (ChildBars.IsEmpty())		// no bars in list
		return(0);

	OILChildBar* FoundBar = (OILChildBar*)ChildBars.GetTail();
	if (FoundBar == NULL)			// Shouldn't happen but I'll check to make sure
		return(0);

	return(FoundBar->GetKPSlot() + 1);

/*	INT32 FoundSlot = 0;
	INT32 HighSlot = 0;
	OILChildBar * FoundBar =(OILChildBar *) ChildBars.GetHead();

   	while (FoundBar != NULL)   // find the highest slot number in the list
	{
	
		FoundSlot = PointToSlot(&(FoundBar->GetPos()));
		if (FoundSlot > HighSlot)
			HighSlot = FoundSlot;

		FoundBar =(OILChildBar *) ChildBars.GetNext(FoundBar);
	
	} // end while
	
	return HighSlot+1;		// slots are numbered from 0 so add 1 to return value
*/
}

/********************************************************************************************

>	OILFixedDockingBar::RemoveSlot(INT32 SlotNo)

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/3/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Remove a slot - shuffle all ChildBars beyond this slot backwards
	Errors:		ASSERT slot number >0
	SeeAlso:	

********************************************************************************************/

void OILFixedDockingBar::RemoveSlot(INT32 SlotNo)
{ 
	ASSERT(SlotNo >= 0 && SlotNo < NoOfSlots);
//	INT32 FoundSlot;
	CRect FoundRect;
	CPoint FoundPos;
	OILChildBar * FoundBar =(OILChildBar *) ChildBars.GetHead();
	INT32 MoveSize = SlotSize[SlotNo];		// The size of the slot being removed

	// Set the last slot to the default size
	SlotSize[MAX_SLOTS-1] = ControlSize;
	// And copy the slot sizes down in the array
	for (INT32 i = SlotNo; i < (MAX_SLOTS - 1); i++)
	{
		SlotSize[i] = SlotSize[i+1];
	}

	// look for bars in or above slot to be removed
	// shuffle any found back one slot
	while (FoundBar != NULL)
	{
		KernelBarPos* pPos = FoundBar->GetBarPos();
//		FoundPos = FoundBar->GetPos();
//		FoundSlot = PointToSlot(&FoundPos);

		ERROR3IF(pPos->Slot == SlotNo, "Deleting non-empty slot");
		
		switch (DockType)				 	// check if found slot => remove slot
		{									// if so pull back one slot
			case DOCKBAR_TOP:
			case DOCKBAR_BOTTOM:
				if (pPos->Slot > SlotNo)
				{
//				 	FoundPos.y -= MoveSize;
					pPos->Slot -= 1;		// Move it back a slot
					FoundBar->UpdatePos();
				}
			   	break;
			case DOCKBAR_LEFT:
			case DOCKBAR_RIGHT:
				if (pPos->Slot > SlotNo)
				{
//				 	FoundPos.x -= MoveSize;
					pPos->Slot -= 1;		// Move it back a slot
					FoundBar->UpdatePos();
				}
            	break;
		}// end switch
//		FoundBar->SetPos(FoundPos);
		FoundBar =(OILChildBar *) ChildBars.GetNext(FoundBar);
	
	} // end while
}

/********************************************************************************************

>	OILFixedDockingBar::InsertSlot(INT32 SlotNo, INT32 NewSize = -1)

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/3/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Insert a Slot - shuffle all ChildBars  beyond this slot number	forwards
	Errors:		ASSERT Slot number >0
	SeeAlso:	

********************************************************************************************/

void OILFixedDockingBar::InsertSlot(INT32 SlotNo, INT32 NewSize)
{ 
	ASSERT(SlotNo >= 0 && SlotNo < NoOfSlots);
	KernelBarPos* pPos = NULL;

	if (NewSize == -1)
		NewSize = ControlSize;

	// First we add up all the slot sizes
	INT32 TotalSize = TotalSlotSizes();
	CRect MainRect;
	GetMainFrame()->GetClientRect(&MainRect);
	INT32 MaxSize = (MainRect.Width() * 48) / 100;

	INT32 AmountToShrink = MaxSize - TotalSize - NewSize;
	// If the new slot will make the total too large then
	if (AmountToShrink > 0)
	{
		// If the new slot is a gallery then try changing NewSize
		if (NewSize != ControlSize)
		{
			// Calculate the new size for this slot
			NewSize = (NewSize - AmountToShrink) < MIN_GALLERY_SLOT_WIDTH ? MIN_GALLERY_SLOT_WIDTH : NewSize - AmountToShrink;
			// Recalculate the amount to shrink
			AmountToShrink = MaxSize - TotalSize - NewSize;
		}

		// If not a gallery or couldn't shrink enough then find another slot to shrink
		if (AmountToShrink > 0)
		{
			for (INT32 i = 0; i < NoOfSlots; i++)
			{
				if (SlotSize[i] != ControlSize)
				{
					INT32 OldSize = SlotSize[i];
					if (OldSize > MIN_GALLERY_SLOT_WIDTH)
					{
						// Calculate the new size for this slot
						INT32 SetSize = (OldSize - AmountToShrink) < MIN_GALLERY_SLOT_WIDTH ? MIN_GALLERY_SLOT_WIDTH : OldSize - AmountToShrink;

						// Resize the slot
						SetSlotSize(i, SetSize);
						
						// Recalculate the amount to shrink
						AmountToShrink -= (OldSize - SlotSize[i]);
					}
				}

				if (AmountToShrink == 0)
					break;
			}
		}
	}

	// Copy the slot sizes up in the array
	for (INT32 i = (MAX_SLOTS - 1); i > SlotNo; i--)
	{
		SlotSize[i] = SlotSize[i-1];
	}
	// And set the slot inserted to the default size
	SlotSize[SlotNo] = NewSize;
	
	OILChildBar* FoundBar = (OILChildBar*) ChildBars.GetHead();
	// look for bars in or above slot to be inserted
	// shuffle any found forward one slot
   	while (FoundBar != NULL)
	{
		pPos = FoundBar->GetBarPos();
		if (pPos->Slot >= SlotNo)
		{
			(pPos->Slot)++;
			FoundBar->UpdatePos();
		}// end if
		FoundBar = (OILChildBar*) ChildBars.GetNext(FoundBar);
	} // end while
}

/********************************************************************************************

>	OILFixedDockingBar::GetSlotHead(CPoint ThisPoint)

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/3/94
	Inputs:		-
	Outputs:	-
	Returns:	A Pointer to the first ChildBar in a given slot -  NULL if none found
	Purpose:	Find the first bar in this slot and return a pointer to it	 
	Errors:		-
	SeeAlso:	

********************************************************************************************/

OILChildBar * OILFixedDockingBar::GetSlotHead(INT32 SlotNo)
{
	CPoint FoundPos;
	INT32 FoundSlot;
  	OILChildBar * FoundBar =(OILChildBar *) ChildBars.GetHead();

   	while (FoundBar != NULL)   // find first bar in this slot
	{
//		FoundPos = FoundBar->GetPos();
//		FoundSlot = PointToSlot(&FoundPos);
		FoundSlot = FoundBar->GetKPSlot();
		if (FoundSlot == SlotNo)
			return FoundBar;
		else if (FoundSlot > SlotNo)
			return NULL;

		FoundBar = (OILChildBar*) ChildBars.GetNext(FoundBar);
	}

	return NULL;		// no bars found
}

/********************************************************************************************

>	OILFixedDockingBar::IsPointInBar(CPoint ThisPoint)

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/3/94
	Inputs:		-
	Outputs:	-
	Returns:	A Pointer to a ChildBar if one is found	- NULL if none found
	Purpose:	-	 
	Errors:		-
	SeeAlso:	

********************************************************************************************/

OILChildBar * OILFixedDockingBar::IsPointInBar(CPoint ThisPoint)
{
	
	CPoint FoundPos;
	CRect FoundRect;
  	OILChildBar * FoundBar =(OILChildBar *) ChildBars.GetHead();

   	while (FoundBar != NULL)
	{
	
		FoundPos = FoundBar->GetPos();		// get FoundBar position and size
		FoundRect = FoundBar->GetRectClient();
		FoundRect += FoundPos;				// offset rect 
		if (FoundRect.PtInRect(ThisPoint))	// check for point in found rectangle
			return FoundBar;
		
		FoundBar =(OILChildBar *) ChildBars.GetNext(FoundBar);	
	
	}
	return NULL;
}

/********************************************************************************************

>	OILFixedDockingBar::PositionBar(OILChildBar * ThisBar)

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/3/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Positions a Control Bar in this dock according to percentage overlap with
				other bars  - If overlap > 30 a new slot will be inserted - If overlap >0
				< 30 it will attempt to shuffle other bars to make space
	Errors:		-
	SeeAlso:	

********************************************************************************************/

void OILFixedDockingBar::PositionBar(OILChildBar * ThisBar)
{
	ASSERT(ThisBar != NULL);
//	CPoint ThisPos = ThisBar->GetPos();						//`get initial slot and position
//	INT32 ThisSlot = PointToSlot(&ThisPos);
	INT32 ThisSlot = ThisBar->GetBarPos()->Slot;

	// First we determine if this slot is suitable for this bar
	// e.g. dropping a gallery into a slot with normal bars is bad
	INT32 ThisSize = ThisBar->GetBarSize();
//	TRACE( _T("PB# ThisSize = %d\n"), ThisSize);

	INT32 SizeOfBarsInSlot = 0;
	INT32 NumBarsInSlot = 0;
	INT32 FoundSlot = 0;
	OILChildBar* FoundBar = GetSlotHead(ThisSlot);
   	while (FoundBar != NULL)   // find the largest bar in this slot
	{
		FoundSlot = FoundBar->GetBarPos()->Slot;
		if (FoundSlot > ThisSlot)	// Gone past the bars in this slot so stop
		{
			break;
		}
		else if (FoundSlot == ThisSlot)
		{
			NumBarsInSlot++;
			INT32 FoundSize = FoundBar->GetBarSize();
			if (FoundSize > SizeOfBarsInSlot)
				SizeOfBarsInSlot = FoundSize;
		}
		FoundBar = (OILChildBar*)ChildBars.GetNext(FoundBar);
	} // end while
//	TRACE( _T("PB# SizeOfBarsInSlot = %d\n"), SizeOfBarsInSlot);

	if (ThisSize != ControlSize)
	{
		// If there are no bars in this slot then
		if (SizeOfBarsInSlot == 0)
		{
			// set this slot to the required size for this bar
			SetSlotSize(ThisSlot, ThisSize);
		}
		else
		{
			// If this bar already contains a gallery then add this bar to this slot
			if (SizeOfBarsInSlot == ControlSize)
			{
				InsertSlot(ThisSlot, ThisSize);
			}
			else
			{
				// set this slot to the required size for this bar
				SetSlotSize(ThisSlot, ThisSize);
			}

		}
		// Make sure this gets updated as it isn't in the slot yet
		ThisBar->UpdatePos();
		::SendMessage(ThisBar->GetHwnd(), WM_SETGALLERYSIZE, SlotSize[ThisSlot], ThisBar->GetExtent());

		PlaceGalleryInSlot(ThisBar, ThisSlot);

		return;
	}

	// At this point we must be positioning a normal bar

	// If this slot contains galleries then insert a new slot
	if (SizeOfBarsInSlot != 0 && SizeOfBarsInSlot != ControlSize)
	{
		InsertSlot(ThisSlot);								// just insert a new slot
		return;
	}

	INT32 AreaOverlapPercent = GetIntersectPercent(ThisBar); 	// get Percentage Overlap
	
	if (AreaOverlapPercent > 30)
	{
//		TRACE( _T("PB# > 30 inserting slot %d\n"),ThisSlot);
		InsertSlot(ThisSlot);								// just insert a new slot  
	}
	else if (AreaOverlapPercent > 0)
	{
//		TRACE( _T("PB# > 0 trying to place %d\n"),ThisSlot);
	
		if (!PlaceBarInSlot(ThisBar,ThisSlot))// try to shuffle and make space for this bar
		{
//			TRACE( _T("PB# Couldn't place - inserting slot %d\n"),ThisSlot);
			InsertSlot(ThisSlot);			  // couldn't shuffle so insert a new slot
		}
	
	} // do nothing if we find no overlaping bars
	
} 


/********************************************************************************************

>	OILFixedDockingBar::PlaceBar(HWND hdialog,CPoint here, CSize size)

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/3/94
	Inputs:		WindowHandle CPoint	
	Outputs:	-
	Returns:	Success/Fail
	Purpose:   	Creates a new Child Bar  then inserts it in the list - it also checks
				for overlap with any other bars and shuffles them until a suitable
				position is found	
	Errors:		-
	SeeAlso:	

********************************************************************************************/

BOOL OILFixedDockingBar::PlaceBar(HWND hdialogbar,CPoint here, CSize size)
{
//	TRACE( _T("PlaceBar x - %d  y - %d\n"), here.x, here.y);
	
	DoRecalc = FALSE;
	OILChildBar * NewChild;
	//check the new window is not already attached to this bar
	if (IsBarInList(hdialogbar)!=NULL)
	{
	
		TRACEALL( _T("ChildBar with this handle is already attached to this window\n"));
		return FALSE;
	
	}
	if ((NewChild= new OILChildBar(hdialogbar,DockType,ControlSize))==NULL)
	{

  		TRACE( _T("Couldn't create new ChildBar"));
   		return FALSE;

	}
	
	ScreenToClient(&here); 		// convert to client coordinates
	PutInSlot(&here, NewChild->GetBarSize());			// pull to the nearest slot and set initial positions
	NewChild->SetPos(here);		// fix initial position and size
	PositionBar(NewChild);		// decide where to place the bar
	InsertBarInList(NewChild);	// insert bar into list
	TidySlots();	// tidy slots - shuffle bars to fill empty slots 

	if (ChangedDock != NULL && ChangedDock != this)
		((OILFixedDockingBar*)ChangedDock)->TidySlots();

	if (DoRecalc && ! GetMainFrame()->IsChangingViewMode() )
		((CFrameWnd*)Parent)->RecalcLayout();		
	DoRecalc = FALSE ;
	ChangedDock = NULL;

	#ifdef _DEBUG
		if (IsUserName("GerryX"))
		{
			Dump_List();	
		}
	#endif
	return TRUE;
}

/********************************************************************************************

>	OILFixedDockingBar::AppendToSlot(INT32 SlotNo,CPoint * here)

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/5/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Tags  this bar onto the end of the last bar in SlotNo 
	Errors:		-
	SeeAlso:	

********************************************************************************************/

void OILFixedDockingBar::AppendToSlot(INT32 SlotNo,CPoint * here)
{
	INT32 FoundSlot = 0;
	INT32 LastPos = 0;
	INT32 LastSlot = 0;
	INT32 size = 0 ;
	KernelBarPos* pFoundPos = NULL;

	OILChildBar* FoundBar = (OILChildBar*)ChildBars.GetHead();
   	while (FoundBar != NULL)   // find the highest slot number in the list
	{
		pFoundPos = FoundBar->GetBarPos();
		FoundSlot = pFoundPos->Slot;
		if (FoundSlot > SlotNo)
		{
			break;
		}
		else if (FoundSlot == SlotNo)
		{
			LastPos = pFoundPos->Position + FoundBar->GetExtent();
		}
	}	

/*	while (FoundBar != NULL)   // find the highest slot number in the list
	{
		pFoundPos = FoundBar->GetBarPos();
		FoundSlot = pFoundPos->Slot;
		if (FoundSlot > LastSlot)
			LastPos = 0;
		// gone past this slot
		if (FoundSlot > SlotNo)
			break;
		// get position
		switch(DockType)
		{
			case DOCKBAR_TOP:
			case DOCKBAR_BOTTOM:
				size = FoundBar->GetRectClient().Width() ;
				break;

			case DOCKBAR_LEFT:
			case DOCKBAR_RIGHT:
				size = FoundBar->GetRectClient().Height();
				break;

		}
		LastPos=PointToPosition(&(FoundBar->GetPos()))+ size;
		// new slot - reset LastPos
	
	 	LastSlot = FoundSlot;
	 	FoundBar =(OILChildBar *) ChildBars.GetNext(FoundBar);
		
	} // end while
	
	if (LastSlot < SlotNo)
		LastPos =0;
*/	
	switch	(DockType)
	{
		case DOCKBAR_TOP:
		case DOCKBAR_BOTTOM:
			here->x = LastPos;
			break;

		case DOCKBAR_LEFT:
		case DOCKBAR_RIGHT:
			here->y = LastPos;
			break;
    }	
	
}

/********************************************************************************************

>	OILFixedDockingBar::PlaceBar(HWND hdialog,KernelBarPos Position, CSize size)

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/3/94
	Inputs:		Window Handle and KernelBarPos 
	Outputs:	
	Returns:	Success/Fail
	Purpose:	Overloaded function - creates a new Child Bar and inserts it in the list
				If BarPos.Position is -1 the bar will be forced into the given slot 
	Errors:		-
	SeeAlso:	

********************************************************************************************/
BOOL OILFixedDockingBar::PlaceBar(HWND hdialogbar,KernelBarPos BarPos, CSize size)
{	
//	if (DockType == DOCKBAR_RIGHT)
//	{
//		TRACE( _T("PlaceBar slot - %d  pos - %d\n"), BarPos.x, BarPos.y);
//	}

	DoRecalc = FALSE ;
	CPoint here;
	// get a client coordinate for this slot/offset
	BarPosToClient(BarPos,&here);									 
//	TRACE( _T("Client x - %d  y - %d\n"), here.x, here.y);
	OILChildBar * NewChild;
	//check the new window is not already attached to this bar
	if (IsBarInList(hdialogbar)!=NULL)
	{
		ReleaseBar(hdialogbar,FALSE);	
	}
	if ((NewChild= new OILChildBar(hdialogbar,DockType,ControlSize))==NULL)
	{
  		TRACE( _T("Couldn't create new ChildBar"));
   		return FALSE;
	}

	// check whether we need to force this bar into given slot
	if (BarPos.Position == -1)
	{
		if (BarPos.Slot > NoOfSlots -1)
		{
			BarPos.Slot = NoOfSlots -1;
			BarPosToClient(BarPos,&here);		
//			TRACE( _T("Client x - %d  y - %d\n"), here.x, here.y);
		}
//		ScreenToClient(&here); 				// convert to client coordinates
		AppendToSlot(BarPos.Slot,&here);	// Get available position on this slot
	   	NewChild->SetPos(here);				//
		InsertBarInList(NewChild);			// insert bar into list
		INT32 SlotsNow = CountSlots();		// check if number of slots has changed
		if (SlotsNow != NoOfSlots)			// and RecalcLayout if it has
		{
			NoOfSlots = SlotsNow;
  			DoRecalc = TRUE;
		}	
	}
	else
	{
//		ScreenToClient(&here); 		// convert to client coordinates
		PutInSlot(&here, NewChild->GetBarSize());			// pull to the nearest slot and set initial positions
		NewChild->SetPos(here);		// fix initial position and size
		PositionBar(NewChild);		// decide where to place the bar
		InsertBarInList(NewChild);	// insert bar into list
		TidySlots();				// tidy slots - shuffle bars to fill empty slots 
	}
	
	// check whether another dock has changed
	// if so tidy slot s and recalc layout etc..
	if (ChangedDock != NULL && ChangedDock != this)
		((OILFixedDockingBar*)ChangedDock)->TidySlots();
	
	if (DoRecalc && ! GetMainFrame()->IsChangingViewMode())
		((CFrameWnd*)Parent)->RecalcLayout();		
	DoRecalc = FALSE ;
	
	ChangedDock = NULL;

	#ifdef _DEBUG
		if (IsUserName("GerryX"))
		{
			Dump_List();	
		}
	#endif
	return TRUE;
}

/********************************************************************************************

>	OILFixedDockingBar::PlaceBarAt(HWND hdialog,KernelBarPos Position)

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/3/94
	Inputs:		Window Handle and KernelBarPos 
	Outputs:	
	Returns:	Success/Fail
	Purpose:	
	Errors:		-
	SeeAlso:	

********************************************************************************************/


BOOL OILFixedDockingBar::PlaceBarAt(HWND hdialogbar,KernelBarPos BarPos)
{

	// Find this child bar 
	OILChildBar * ThisChild = (OILChildBar *) ChildBars.GetHead();
	while (ThisChild != NULL)				
	{
		if (ThisChild->GetHwnd() == hdialogbar)	//found the bar
		{
			break;
		}			
		ThisChild =(OILChildBar *) ChildBars.GetNext(ThisChild);	
	}
  	
  	ENSURE(ThisChild!=NULL,"Can't find Bar in DockingBar List");
	
	ThisChild->SetPos(BarPos);	// set child position

	return TRUE;
}


/********************************************************************************************

>	OILFixedBar::InsertBarInList(OILChildBar * ThisBar)

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/3/94
	Inputs:		-
	Outputs:	-
	Returns:	Success
	Purpose:	Inserts a ChildBar in the list in slot and position order 
	Errors:		-
	SeeAlso:	

********************************************************************************************/
 
BOOL OILFixedDockingBar::InsertBarInList(OILChildBar * ThisBar)
{

	ASSERT(ThisBar!=NULL);
	CPoint ThisPos=ThisBar->GetPos();
	CPoint FoundPos;
	OILChildBar * FoundBar =(OILChildBar *) ChildBars.GetHead();

	INT32 FoundSlot;
	INT32 FoundPosition;
	// get insert bar Slot and Position	
	INT32 ThisSlot = PointToSlot(&ThisPos);
	INT32 ThisPosition = PointToPosition(&ThisPos);

   	while (FoundBar != NULL)
	{
		FoundPos = FoundBar->GetPos();
		FoundSlot = PointToSlot(&FoundPos);
		FoundPosition = PointToPosition(&FoundPos);
		if (FoundSlot > ThisSlot)		// past target slot - just insert before Found slot
		{
		 	 ChildBars.InsertBefore((ListItem *)FoundBar,(ListItem *)ThisBar);
			 return TRUE;
		}
		else if (FoundSlot == ThisSlot)		// Found a bar in the target slot
		{
			if (FoundPosition > ThisPosition)	// Put in position order
			{
				ChildBars.InsertBefore((ListItem *)FoundBar,(ListItem *)ThisBar);
			   	return TRUE;
			}
		}
		FoundBar =(OILChildBar *) ChildBars.GetNext(FoundBar);
	} // end while

	// nothing found - Add to tail 
	ChildBars.AddTail((ListItem *)ThisBar);				
	return TRUE;
}

/******************************************************************************************
>	OILFixedDockingBar::ReleaseBar(HWND hdialog,BOOL DoTidy)

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/3/94
	Inputs:		-
	Outputs:	-
	Returns:	Success/Fail
	Purpose:	Search for a ChildBar with a given window handle - if found
				call RemoveBar to remove from list and delete Child Object	
	Errors:		-
	SeeAlso:	

********************************************************************************************/ 

BOOL	OILFixedDockingBar::ReleaseBar(HWND hdialog,BOOL DoTidy)
{
	DoRecalc = FALSE ;
	// Find this child bar
	OILChildBar * ThisChild =(OILChildBar *) ChildBars.GetHead(); 
   	while (ThisChild != NULL)				
	{
		if (ThisChild->GetHwnd() == hdialog)	//found the bar
		{
			if (!RemoveBar(ThisChild))		//remove the bar
				return FALSE;
			ChangedDock = this;			//"this" will be used later to force a layout tidy
			if (DoTidy)
				TidySlots();
			if (DoRecalc && ! GetMainFrame()->IsChangingViewMode())
			{
				// there is not point in doing this if camelot is dying!
				// especially since this fails up our floating toolbar!
				if (!(GetApplication()->CamelotIsDying ()))
				{
					((CFrameWnd*)Parent)->RecalcLayout();		
				}
			}
			DoRecalc = FALSE ;
			return TRUE;
		}			
		ThisChild =(OILChildBar *) ChildBars.GetNext(ThisChild);	
	}// end while
	
  return FALSE;
}



/****************************************************************************

>	BOOL OILFixedDockingBar::SetSlotSize(INT32 SlotNo, INT32 NewSize)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/10/2004

	Inputs:		SlotNo		- Index of the slot to be resized
				NewSize		- New size of the slot
	Returns:	TRUE if ok, FALSE if failed for any reason
	Purpose:	Changes the size of a slot

****************************************************************************/

BOOL OILFixedDockingBar::SetSlotSize(INT32 SlotNo, INT32 NewSize)
{
	if (SlotNo < 0 ||
		SlotNo >= MAX_SLOTS ||
		NewSize < ControlSize)
		return(FALSE);
	
//	if (SlotSize[SlotNo] != NewSize)
//	{
		// First we add up all the slot sizes
		if (NewSize < MIN_GALLERY_SLOT_WIDTH)
			NewSize = MIN_GALLERY_SLOT_WIDTH;
		INT32 OldSize = (SlotNo < NoOfSlots) ? SlotSize[SlotNo] : 0;
		INT32 TotalSize = TotalSlotSizes();
		CRect MainRect;
		GetMainFrame()->GetClientRect(&MainRect);
		INT32 MaxSize = (MainRect.Width() * 48) / 100;
		INT32 AmountToShrink = (NewSize > OldSize) ? (TotalSize - OldSize + NewSize) - MaxSize : 0;
		if (AmountToShrink < 0)
			AmountToShrink = 0;

		// If the new slot will make the total too large then
		if (AmountToShrink > 0)
		{
			for (INT32 i = 0; i < NoOfSlots; i++)
			{
				if (i != SlotNo && SlotSize[i] != ControlSize)
				{
					INT32 OldSize = SlotSize[i];
					if (OldSize > MIN_GALLERY_SLOT_WIDTH)
					{
						// Calculate the new size for this slot
						INT32 SetSize = (OldSize - AmountToShrink) < MIN_GALLERY_SLOT_WIDTH ? MIN_GALLERY_SLOT_WIDTH : OldSize - AmountToShrink;

						// Resize the slot
						SetSlotSize(i, SetSize);
						
						// Recalculate the amount to shrink
						AmountToShrink -= (OldSize - SlotSize[i]);
					}
				}

				if (AmountToShrink == 0)
					break;
			}
		}

		NewSize -= AmountToShrink;
		if (NewSize >= MIN_GALLERY_SLOT_WIDTH)
		{
			SlotSize[SlotNo] = NewSize;
		}

		KernelBarPos* FoundPos;
		OILChildBar* FoundBar = (OILChildBar*) ChildBars.GetHead();
		DoRecalc = TRUE;
		
		// Look for bars in or above the slot to be resized
		// If in the slot then resize them
		// If above the slot then move by the difference of the sizes
   		while (FoundBar != NULL)
		{
			FoundPos = FoundBar->GetBarPos();
			if (FoundPos->Slot == SlotNo)
			{
				::SendMessage(FoundBar->GetHwnd(), WM_SETGALLERYSIZE, NewSize, FoundBar->GetExtent());
			}
			else //	if (FoundPos->Slot > SlotNo)
			{
				FoundBar->UpdatePos();
			}
			FoundBar =(OILChildBar*) ChildBars.GetNext(FoundBar);
		
		} // end while
//	}

	return(TRUE);
}




/////////////////////////////////////////////////////////////////////////////////////////////
// 				OILFloaters
/////////////////////////////////////////////////////////////////////////////////////////////


IMPLEMENT_DYNAMIC(OILFloaters ,OILDockingBar)


/////////////////////////////////////////////////////////////////////////////////////////////
//		OILFloaters Message Map
/////////////////////////////////////////////////////////////////////////////////////////////
 
BEGIN_MESSAGE_MAP( OILFloaters, OILDockingBar )
	//{{AFX_MSG_MAP( OILFloaters )
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_MESSAGE(WM_SIZEPARENT, OnSizeParent) 
	//}}AFX_MSG_MAP
END_MESSAGE_MAP( )

/********************************************************************************************

>	OILFloaters::Create(CWnd * pParentWnd)

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/3/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Assigns a Parent window and Sets DockBar Type
	Errors:		-
	SeeAlso:	

********************************************************************************************/

BOOL OILFloaters::Create(CWnd* pParentWnd)
{
	
	ASSERT_VALID(pParentWnd);   // must have a parent

	Parent = pParentWnd;		// Parent of this bar

	DockType = DOCKBAR_FLOAT; 	// Type of this instance
	return TRUE;
} 

/********************************************************************************************

>	OILFloaters::OILFloaters()

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/3/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Creates and inits a OILFloaters object.
	Errors:		-
	SeeAlso:	

********************************************************************************************/
                    
OILFloaters::OILFloaters()
{ 
	ControlSize = SMALL_CONTROLS;
	HasBigControls = FALSE; 
}


/********************************************************************************************

>	BOOL OILFloaters::DeleteBars()

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/3/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	deletes all BarList items
	Errors:		-
	SeeAlso:	

********************************************************************************************/

BOOL OILFloaters::DeleteBars()
{
	ControlSize = SMALL_CONTROLS;
	HasBigControls = FALSE;
	ChildBars.DeleteAll();
	return TRUE;
}
/********************************************************************************************

>	OILFloaters::PlaceBar(HWND hdialog,CPoint here, CSize size = CSize(0,0))

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/3/94
	Inputs:		WindowHandle CPoint	
	Outputs:	-
	Returns:	Success/Fail
	Purpose:   	Creates a new Child Bar  then inserts it in the list - it also checks
				for overlap with any other bars and shuffles them until a suitable
				position is found;	
	Errors:		-
	SeeAlso:	

********************************************************************************************/

BOOL OILFloaters::PlaceBar(HWND hdialogbar,CPoint here, CSize size)
{
//	TRACE( _T("PlaceBar x - %d  y - %d  w - %d  h - %d\n"), here.x, here.y, size.cx, size.cy);
	DoRecalc = FALSE ;
	OILChildBar * NewChild;

	if ((NewChild = new OILChildBar(hdialogbar,DockType,ControlSize))==NULL)
	{
  		TRACE( _T("Couldn't create new ChildBar/n"));
   		return FALSE;
	}
	
	NewChild->SetPos(here, size);		// fix initial position and size
	InsertBarInList(NewChild);	// insert bar into list
	
	if (ChangedDock != NULL && ChangedDock != this)
		((OILFixedDockingBar*)ChangedDock)->TidySlots();

	if (DoRecalc)
		((CFrameWnd*)Parent)->RecalcLayout();		
	DoRecalc = FALSE ;
	ChangedDock=NULL;

	#ifdef _DEBUG
		if (IsUserName("GerryX"))
		{
			Dump_List();
		}
	#endif
	
	return TRUE;
}


/********************************************************************************************

>	OILFloaters::PlaceBar(HWND hdialog,KernelBarPos Position, CSize size)

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/3/94
	Inputs:		Window Handle and KernelBarPos 
	Outputs:	
	Returns:	Success/Fail
	Purpose:	Overloaded function - creates a new Child Bar and inserts it in the list
				it assumes that there will be no overlap with other bars - this should only
				be called when positioning bars from file as these will have already been
				positioned and shuffled etc by the other PlaceBar function.
	Errors:		-
	SeeAlso:	

********************************************************************************************/

BOOL OILFloaters::PlaceBar(HWND hdialogbar,KernelBarPos BarPos, CSize size)
{
//	TRACE( _T("PlaceBar slot - %d  pos - %d Type - %d\n"), BarPos.x, BarPos.y, BarPos.Dock);
	
	DoRecalc = FALSE ;
	OILChildBar * NewChild;
	if ((NewChild= new OILChildBar(hdialogbar,DockType,ControlSize))==NULL)
	{
  		TRACEALL( _T("Couldn't create new ChildBar\n"));
   		return FALSE;
	}
	
	NewChild->SetPos(BarPos);	// set child position
	InsertBarInList(NewChild);	// insert in list
	
	if (ChangedDock != NULL && ChangedDock != this)
		((OILFixedDockingBar*)ChangedDock)->TidySlots();
	
	if (DoRecalc)
		((CFrameWnd*)Parent)->RecalcLayout();		
	DoRecalc = FALSE ;
	
	ChangedDock=NULL;
	
	#ifdef _DEBUG
		if (IsUserName("GerryX"))
		{
			Dump_List();
		}
	#endif
	return TRUE;
}

/********************************************************************************************

>	OILFloaters::InsertBarInList(OILChildBar * ThisBar)

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/3/94
	Inputs:		-
	Outputs:	-
	Returns:	Success
	Purpose:	Inserts a ChildBar at the tail of the list 
	Errors:		-
	SeeAlso:	

********************************************************************************************/
 
BOOL OILFloaters::InsertBarInList(OILChildBar * ThisBar)
{
	
	ASSERT(ThisBar!=NULL);
	//Add to tail 
	ChildBars.AddTail((ListItem *)ThisBar);				

	return TRUE;
}

/******************************************************************************************
>	OILFloatersBar::ReleaseBar(HWND hdialog,BOOL DoTidy)

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/3/94
	Inputs:		-
	Outputs:	-
	Returns:	Success/Fail
	Purpose:	Search for a ChildBar with a given window handle - if found
				call RemoveBar to remove from list and delete Child Object	
	Errors:		-
	SeeAlso:	

********************************************************************************************/ 

BOOL	OILFloaters::ReleaseBar(HWND hdialog,BOOL DoTidy)
{
	
	OILChildBar * ThisChild =(OILChildBar *) ChildBars.GetHead(); 
   	while (ThisChild != NULL)				
	{
	
		if (ThisChild->GetHwnd() == hdialog)	//found the bar
		{
			RemoveBar(ThisChild);			//remove the bar
			return TRUE;
		}			
		ThisChild =(OILChildBar *) ChildBars.GetNext(ThisChild);	
	
	}// end while
	//TRACE( _T("Remove Bar failed %d"),hdialog);
	return FALSE;
}

/********************************************************************************************

>	BOOL OILFloaters::RecreateBarsAfterSettingChange()

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/3/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Sets Big Control state
	Errors:		-
	SeeAlso:	
													  
********************************************************************************************/
// UIC 

BOOL OILFloaters::RecreateBarsAfterSettingChange()
{
	
	if(!BaseBar::HaveNonClientMetricsChanged())
		return FALSE;

	OILChildBar * ThisChild =(OILChildBar *) ChildBars.GetHead();
	OILChildBar * Next; 
	OILChildBar * EndList =(OILChildBar *) ChildBars.GetTail();
	

	while (ThisChild != NULL)
	{
		Next =(OILChildBar *) ChildBars.GetNext(ThisChild);
		::SendMessage(ThisChild->GetHwnd(), WM_RESIZECONTROLS, 0,0);
		
		ThisChild = Next;
		if (ThisChild == NULL) break;
		//ENSURE(ThisChild != NULL,"Bar Item = NULL");
		if (ThisChild == EndList)
		{
			::SendMessage(ThisChild->GetHwnd(), WM_RESIZECONTROLS, 0,0);
			break;
		}		
	}

 return TRUE;
}
/********************************************************************************************

>	OILFloaters::SetBigControlState(BOOL Big)

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/3/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Sets Big Control state
	Errors:		-
	SeeAlso:	
													  
********************************************************************************************/

void OILFloaters::SetBigControlState(BOOL Big )
{
	if (Big != HasBigControls)
	{
		HasBigControls = Big;
		if (Big)

			ControlSize = LARGE_CONTROLS;
		else
	
			ControlSize = SMALL_CONTROLS;

		// have to be a bit careful here DialogBar will delete and add new items
		// to this list when it receives the RESIZECONTROLS Message 
		OILChildBar * ThisChild =(OILChildBar *) ChildBars.GetHead();
		OILChildBar * Next; 
	 	OILChildBar * EndList =(OILChildBar *) ChildBars.GetTail();
	 	

	 	while (ThisChild != NULL)
		{
			Next =(OILChildBar *) ChildBars.GetNext(ThisChild);
			::SendMessage(ThisChild->GetHwnd(), WM_RESIZECONTROLS, 0,0);
			
			ThisChild = Next;
			if (ThisChild == NULL) break;
			//ENSURE(ThisChild != NULL,"Bar Item = NULL");
			if (ThisChild == EndList)
			{
				::SendMessage(ThisChild->GetHwnd(), WM_RESIZECONTROLS, 0,0);
				break;
			}		
		}
	}
}
 

 
/////////////////////////////////////////////////////////////////////////////////////////////
// OILFixedDockingBar diagnostics
/////////////////////////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
void OILFixedDockingBar::AssertValid() const
{
	CControlBar::AssertValid();
}

void OILFixedDockingBar::Dump(CDumpContext& dc) const
{
	CControlBar::Dump(dc);
	AFX_DUMP1(dc, "\nControlSize = ", (UINT32)ControlSize);
	AFX_DUMP1(dc, "\nNo Of Slots  = ", (UINT32)NoOfSlots);
	AFX_DUMP1(dc, "\nWidth = ", (UINT32)DockSize.cx);
	AFX_DUMP1(dc, "\nHeight = ", DockSize.cy);
}

void OILFixedDockingBar::Dump_List()
{

	CPoint ThisPos;
	KernelBarPos * KPos;
	
	TRACE( _T("FIXED DOCK BAR ----------\n"));
	OILChildBar * FoundBar =(OILChildBar *) ChildBars.GetHead();
   	while (FoundBar != NULL)
	{
		KPos=FoundBar->GetBarPos();
		TRACE( _T("Kernel Data slot - %d  pos - %d DockType - %d\n"),KPos->Slot,KPos->Position,KPos->Dock);
		ThisPos=FoundBar->GetPos();
	 	TRACE( _T("Dump Bar    slot - %d  pos - %d\n"),PointToSlot(&ThisPos),PointToPosition(&ThisPos));
		FoundBar =(OILChildBar *) ChildBars.GetNext(FoundBar);	
	}
	TRACE( _T("....................................................................................\n"));

 }

#endif

/////////////////////////////////////////////////////////////////////////////////////////////
// OILDockingBar diagnostics
/////////////////////////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
void OILDockingBar::AssertValid() const
{
	CControlBar::AssertValid();
}

void OILDockingBar::Dump(CDumpContext& dc) const
{
	CControlBar::Dump(dc);

}

 void OILDockingBar::Dump_List()
 {

 }

#endif

/////////////////////////////////////////////////////////////////////////////////////////////
// OILFloaters diagnostics
/////////////////////////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
void OILFloaters::AssertValid() const
{
	CControlBar::AssertValid();
}

void OILFloaters::Dump(CDumpContext& dc) const
{
	CControlBar::Dump(dc);

}

void OILFloaters::Dump_List()
{
	
	CPoint ThisPos;
	KernelBarPos * KPos;
	
	TRACE( _T("..... FLOATING BARS  .....\n "));
	OILChildBar * FoundBar =(OILChildBar *) ChildBars.GetHead();
   	while (FoundBar != NULL)
	{
		KPos=FoundBar->GetBarPos();
		TRACE( _T("Kernel Data slot - %d  pos - %d Type - %d\n"),KPos->x,KPos->y,KPos->Dock);
		ThisPos=FoundBar->GetPos();
	 	TRACE( _T("Dump Bar Hwnd %d\n"),FoundBar->GetHwnd());
		FoundBar =(OILChildBar *) ChildBars.GetNext(FoundBar);	
	}
	TRACE( _T("..................................................................................\n"));

}

#endif

#endif
