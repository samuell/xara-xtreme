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
/*
// */

#include "camtypes.h"
//#include "dlgbar.h"
//#include "ensure.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "camframe.h"
#include "oilmods.h"
#include "dlgmgr.h"
//#include "bitbutn.h"
//#include "resource.h" // temp
//#include "barsdlgs.h"
//#include "childbar.h"
//#include "bbutdef.h"
//#include "ctrlhelp.h"
//#include "bars.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "camelot.h"
//#include "ctl3d.h"
//#include "fonts.h"
#include "gallery.h"
//#include "sgallery.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "tool.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "app.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "docview.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "camview.h"
#include "keypress.h"
//#include "bars.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "selector.h"
//#include "reshelp.h"
//#include "dragmgr.h"
//#include "custmsg.h"
//#include "cursor.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "csrstack.h"
//#include "phil.h"
//#include "convert.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "bubbleid.h"
//#include "will2.h"						// for HideDisableGadgetAndResuffleBar and
										// ShowEnableGadgetAndResuffleBar
										// to jump to point where ID's are used search
										// for 'HideDisableGadgetAndResuffleBar' ....

#include "basebar.h"

#ifdef STANDALONE
#include "galbar.h"
#endif

IMPLEMENT_DYNAMIC_CLASS( BaseBar, wxWindow )

// Statics

wxRect BaseBar::OldDragRect(0,0,0,0);

DlgDragType BaseBar::DragType = NONE; 

BOOL BaseBar::InRecreate = FALSE;
BOOL BaseBar::DoingErrorBox = FALSE;

// NEW BUTTON STUFF
UINT32 BaseBar::GalButtonWidth = 0;


wxWindow * BaseBar::LastWindowWithFocus = NULL;

BOOL BaseBar::ControlHasFocus = FALSE;

BOOL BaseBar::IsBackGroundRendering = FALSE;

wxWindow* BaseBar::HwndWithFocus = NULL ;

BaseBar * BaseBar::CurrentBar = NULL;

INT32 BaseBar::TitleBarHeight = 0;  // UIC
INT32 BaseBar::BorderWidth = 0;	  // UIC	


// control dragging stuff
ControlDragState BaseBar::CtlDragState = NO_DRAG;
ControlDragMode BaseBar::CtlDragMode = CUT;

Cursor*  BaseBar::pCurrentCursor = NULL;
Cursor*  BaseBar::pButtonCopyCursor = NULL;
Cursor*  BaseBar::pButtonCutCursor = NULL;
INT32	 BaseBar::CurrentCursorID = 0;
BaseBar* BaseBar::PtrToToolBar = NULL;

// This macro aligns a BYTE ptr to a dword boundary	 
#if WIN32
#define	DWORD_ALIGN(x)	if (((DWORD)(x))&3) x+=4-(((DWORD)(x))&3)
#else
#define	DWORD_ALIGN(x)
#endif

#define NO_REDRAW FALSE

CC_IMPLEMENT_DYNAMIC(FormatPosition, ListItem) 


// Used to be 725, 600
// Used to be 770, 614
enum InfoBarWidths { LARGE_INFO_BAR = 910, SMALL_INFO_BAR = 774 };

#define COMBOHEIGHT  SMALLBUTTONHEIGHT - 2

#define IS_CHICAGO ((LOBYTE(LOWORD(GetVersion()))) > 3) 

#define COMMIT 2

#define Y_BASEUNITS 16
#define X_BASEUNITS 8


//#define IS_CHICAGO 1



/********************************************************************************************

>	BaseBar::GetEnumItemType(BarItem * Item)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/3/94
	Inputs:		the baritem to type
	Outputs:	-
	Returns:	An Enumerated BarType 
	Purpose:	To return an enumerated BarType from a BarItem
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BarItemType	GetEnumItemType(BarItem * Item)
{
	PORTNOTETRACE( "dialog", "GetEnumItemType - do nothing" );
#ifndef EXCLUDE_FROM_XARALX
	if(Item == NULL)
		return UNKNOWN_ITEM;
	if (Item->IsKindOf(CC_RUNTIME_CLASS(BarControl)))
	{
        HWND hcontrol=(HWND)((BarControl*)Item)->GetWinID();	// Get the Window Handle
		
		String_256 ClassNameStr;

		GetClassName(hcontrol, (TCHAR*)ClassNameStr, 255);	// Check type of control
		if (ClassNameStr == String_16(TEXT("cc_BitmapButton")))
			return BITMAPBUTTON;
		if (ClassNameStr == String_16(TEXT("Button")))
			return BUTTON;
		if (ClassNameStr == String_16(TEXT("cc_SmallButton")))
			return SMALLBITMAPBUTTON;
		if (ClassNameStr == String_16(TEXT("Static")))
			return STATIC;
		if (ClassNameStr == String_16(TEXT("ComboBox")))
			return COMBO;											 
		if (ClassNameStr == String_16(TEXT("Edit")))
			return EDIT;
		if (ClassNameStr == String_16(TEXT("cc_StaticBitmap")))
			return STATICBITMAP;
		if (ClassNameStr == String_16(TEXT("cc_Grid")))
			return ROTATEGRID;
		if (ClassNameStr == String_16(TEXT("cc_Text3D")))
			return TEXT3D;
		if (ClassNameStr == String_16(TEXT("cc_Slider")))
			return SLIDER;
		if (ClassNameStr == String_64(TEXT("cc_1dBitmapComboBoxEdit")))
			return BITMAPCOMBO1D;
		if (ClassNameStr == String_64(TEXT("cc_2dBitmapComboBoxEdit")))
			return BITMAPCOMBO2D;
		return OTHERCONTROL;
	}

	if ( Item->IsKindOf(CC_RUNTIME_CLASS(BarSeparator)) )
		return SEPARATOR;
	if ( Item->IsKindOf(CC_RUNTIME_CLASS(BarLineFeed)) )
		return LINEFEED;
   	if ( Item->IsKindOf(CC_RUNTIME_CLASS(DDeckerTop)))
		return DECKER_TOP;
	if ( Item->IsKindOf(CC_RUNTIME_CLASS(DDeckerBottom)))
		return DECKER_BOTTOM;
	if ( Item->IsKindOf(CC_RUNTIME_CLASS(DDeckerEnd)))
		return DECKER_END;
	ENSURE(FALSE,"Unknown Bar Item");
#endif
	return UNKNOWN_ITEM;
}

/********************************************************************************************

>	static BOOL BaseBar::DeclarePreferences()

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/12/95
	Returns:	TRUE if it worked, FALSE if it failed
	Purpose:	Declares any preferences that the BaseBar class needs to decalre

********************************************************************************************/

BOOL BaseBar::DeclarePreferences()
{
	// Use a built-in resource rather than an ini file setting...
	INT32 TempWidth = 72;
	Convert::StringToLong( String_8( _R(IDS_DEFAULT_GAL_BUTTON_SIZE) ), &TempWidth );
	GalButtonWidth = (UINT32)TempWidth;
	return TRUE;

//	GalButtonWidth = 72;
//	return Camelot.DeclareSection("Galleries",2)
//	&& Camelot.DeclarePref("Galleries", "ButtonWidth", &GalButtonWidth);
}
 
/********************************************************************************************

>	BaseBar::BaseBar()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/3/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	The BaseBar constructor 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BaseBar::BaseBar()	 
{
	// Get pointers to Stock Fonts for Title and controls
// 	TitleFont = FontFactory::GetwxFont(STOCKFONT_DIALOGBARTITLE);
//	BarFont = FontFactory::GetwxFont(STOCKFONT_DIALOGBARCLIENT);
//	SmallThinFont = FontFactory::GetwxFont(STOCKFONT_DIALOGBARSMALL);
//	LargeThinFont = FontFactory::GetwxFont(STOCKFONT_DIALOGBARLARGE);

	// Get the System FrameWidth
	BorderWidth = wxSystemSettings::GetMetric( wxSYS_FRAMESIZE_X );
	
	// Get the height of the title bar  UIC
	// under '95 this is system wide    UIC

#if defined(__WXMSW__)
	if(IS_CHICAGO)
	{
		TitleBarHeight = 14;   // UIC
		#if _MFC_VER >= 0x400
	  		TitleBarHeight = wxSystemSettings::GetMetric(SM_CYSMCAPTION);
		#endif
	}
	else
#endif
	{
		TitleBarHeight = 10;   // UIC
	}
	// used in GrowNorth GrowSouth 
	TallestFloat = WidestFloat = 1;

	// init drag state  
	DragType = NONE;
	DragCtlHwnd = NULL;
	
	Active = TRUE;
	
	
	InitFormatRects();
	LastSmallBarPos.x = LastSmallBarPos.y = -1;
	
	// create a bitmap for the drag rendering
#pragma message( __LOCMSG__ "Not using dither pattern" )
//  WORD WindowsGray [] = { 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA } ;
//	BrushBitmap.CreateBitmap (8, 8, 1, 1, (LPSTR) WindowsGray) ;
	BrushBitmap.Create( 8, 8, 1 );
}




/********************************************************************************************

>	BaseBar::~BaseBar()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/3/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	BaseBar destructor
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BaseBar::~BaseBar()
{
	FormatPositions.DeleteAll();
}

/********************************************************************************************

>	BOOL BaseBar::IsBarInRecreate()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/3/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	BaseBar destructor
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL BaseBar::IsBarInRecreate()
{
	return InRecreate;
}


/********************************************************************************************

>	static BOOL BaseBar::IsDoingErrorBox()

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/6/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Returns whether we're doing an error box or not...
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL BaseBar::IsDoingErrorBox()
{
	// If it's an MFC exception, etc...
/*	if(CCamApp::IsDisabled())
		return TRUE;*/

	return DoingErrorBox;
}

/********************************************************************************************

>	static void BaseBar::StartErrorBox()

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/6/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	To set the variable which stops us grabbing back the focus in mainframe if
				the bar / gallery hasn't been created
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void BaseBar::StartErrorBox()
{
	DoingErrorBox = TRUE;
}

/********************************************************************************************

>	static void BaseBar::FinishErrorBox()

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/6/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	To reset the variable which stops us grabbing back the focus in mainframe if
				the bar / gallery hasn't been created
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void BaseBar::FinishErrorBox()
{
	DoingErrorBox = FALSE;
}

/********************************************************************************************

>	BaseBar::SetFormatVariables()

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/5/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Sets up various formating variables according to dockbar type 
				large/small controls etc .. must be called on every create
	Errors:		-
	SeeAlso:	-

********************************************************************************************/


void BaseBar::SetFormatVariables()
{
#pragma message( __LOCMSG__ "BaseBar::SetFormatVariables - do nothing" )
	TRACE( _T("Warning - BaseBar::SetFormatVariables called") );
/*	if (GetMainFrame()->GetDockBar(ParentDockBarType)->IsBigControls())
		SeparatorWidth = LARGE_SEPARATOR;
	else
		SeparatorWidth = SMALL_SEPARATOR; */
}



/********************************************************************************************

>	void BaseBar::CloseCombos()

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/5/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Close any drop-down windows
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

			 
void BaseBar::CloseCombos()
{															 
#pragma message( __LOCMSG__ "BaseBar::CloseCombos - do nothing" )
	TRACE( _T("Warning - BaseBar::CloseCombos called") );
/*	String_256 ClassNameStr;
	HWND CurrentChild;
	HWND NextChild;

	// Get control HWND's 
	for (CurrentChild = ::GetWindow(GetSafeHwnd(),GW_CHILD);
		 CurrentChild != NULL; 
		 CurrentChild = NextChild)
	{
		GetClassName(CurrentChild, (TCHAR*)ClassNameStr, 255);
	   	// check for ComboBox/ListBox
	   	if ( (ClassNameStr == String_8(TEXT("ListBox"))) ||
			 (ClassNameStr == String_8(TEXT("ComboBox"))) ||
			 (ClassNameStr == String_64(TEXT("cc_1dBitmapComboBoxEdit"))) ||
			 (ClassNameStr == String_64(TEXT("cc_2dBitmapComboBoxEdit")))
		   )
		{
   			// send close message
			::SendMessage(CurrentChild,CB_SHOWDROPDOWN,FALSE,0);
		}
		NextChild = ::GetWindow(CurrentChild,GW_HWNDNEXT);
	} */

}




/********************************************************************************************

>	wxRect BaseBar::GetDragFormatRect(DockBarType BarType, wxPoint CursorPos, BOOL ToLeft
																			  BOOL Below)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/5/94
	Inputs:		BarType:			Type of docking bar to create new bar shape for
				CursorPos:			Current cursor position
				(and class statics)
				OldDragRect:		Previous dragged rectangle
				DragOffset:			Offset of cursro into previous rectangle
				DragOffsetFromLeft:	TRUE when cursor is offset from left of previous
				DragOffsetFromTop:	TRUE when cursor is offset from top of previous

	Outputs:	-
	Returns:	-
	Purpose:	Get cached format size and offset it appropriately around the current drag
				position.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

wxRect  BaseBar::GetDragFormatRect(DockBarType BarType, wxPoint CursorPos, BOOL ToLeft,
																			BOOL Below )
{
	wxRect	DragRect(0,0,0,0);
	DragRect = GetFormatRect(BarType);

//	TRACEUSER( "Gerry", _T("FormatRect(%d) = (%d, %d) - (%d, %d)\n"), BarType, DragRect.left, DragRect.top, DragRect.GetRight(), DragRect.GetBottom());

	wxPoint NewOffset = CalcDragOffset(DragRect, CursorPos, ToLeft, Below);

//	TRACEUSER( "Gerry", _T("Offset         = (%d, %d)\n"), NewOffset.x, NewOffset.y);

	DragRect.Offset( CursorPos.x - NewOffset.x, CursorPos.y - NewOffset.y );

//	TRACEUSER( "Gerry", _T("After Offset   = (%d, %d) - (%d, %d)\n"), DragRect.left, DragRect.GetTop(), DragRect.GetRight(), DragRect.GetBottom());

	return ( DragRect );
}




/********************************************************************************************

>	wxPoint BaseBar::CalcDragOffset(wxRect DragRect, wxPoint CursorPos, BOOL ToLeft, BOOL Below)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/5/94
	Inputs:		DragRect:			Current dragged rectangle
				(and class statics)
				OldDragRect:		Previous dragged rectangle
				DragOffset:			Offset of cursor into previous rectangle
				DragOffsetFromLeft:	TRUE when cursor is offset from left of previous
				DragOffsetFromTop:	TRUE when cursor is offset from top of previous
	Outputs:	-
	Returns:	-
	Purpose:	Use the current drag rect the previous one and the current cursor pos to
				calculate the new offsets into the new drag rectangle.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

wxPoint	BaseBar::CalcDragOffset(wxRect DragRect, wxPoint CursorPos, BOOL ToLeft, BOOL Below)
{
	wxPoint	NewOffset;

	if (ToLeft)
	{
		NewOffset.x = DragOffset.x;
		if (NewOffset.x > DragRect.GetWidth())
			NewOffset.x = DragRect.GetWidth()-6;
	}
	else
	{
		NewOffset.x = DragRect.GetWidth()-(OldDragRect.GetWidth()-DragOffset.x);
		if (NewOffset.x < 0)
			NewOffset.x = 6;
	}

	if (Below)
	{
		NewOffset.y = DragOffset.y;
		if (NewOffset.y > DragRect.GetHeight())
			NewOffset.y = DragRect.GetHeight()-6;
	}
	else
	{
		NewOffset.y = DragRect.GetHeight()-(OldDragRect.GetHeight()-DragOffset.y);
		if (NewOffset.y < 0)
			NewOffset.y = 6;
	}

	return ( NewOffset );
}


/********************************************************************************************

>	wxRect BaseBar::BaseBar::UnCacheFormatRects()

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/5/94
	Inputs:		DockBarType
	Outputs:	-
	Returns:	-
	Purpose:	Set all Cache Format rectangles empty
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void  BaseBar::InitFormatRects()
{

	FloatLargeRect		= wxRect();
	VerticalLargeRect	= wxRect();
	HorizontalLargeRect	= wxRect();
	FloatSmallRect		= wxRect();
	VerticalSmallRect	= wxRect();
	HorizontalSmallRect	= wxRect();

}

/********************************************************************************************

>	wxRect BaseBar::GetFormatRect(DockBarType BarType)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/5/94
	Inputs:		DockBarType
	Outputs:	-
	Returns:	Cached Rectangle according to dock and control size
	Purpose:	Get cached format size - if none available format and cache
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

wxRect  BaseBar::GetFormatRect(DockBarType BarType)
{
#pragma message( __LOCMSG__ "BaseBar::GetFormatRect - do nothing" )
	TRACE( _T("Warning - BaseBar::GetFormatRect called") );
/*	OILDockingBar* DockBar = GetMainFrame()->GetDockBar(BarType);
 	switch(BarType)
	{
		case DOCKBAR_FLOAT:
			if (DockBar->IsBigControls())
			{
				if (FloatLargeRect.IsEmpty())
					FormatFloatingBar(FALSE,USE_BAR);
				return FloatLargeRect;
			}
			else
			{
				if (FloatSmallRect.IsEmpty())
					FormatFloatingBar(FALSE,USE_BAR);
				return FloatSmallRect;
			}
 			break;
 		case DOCKBAR_LEFT:
		case DOCKBAR_RIGHT:
			if (DockBar->IsBigControls())
			{
				if (VerticalLargeRect.IsEmpty())
					FormatVerticalBar(FALSE,USE_BAR);
				return VerticalLargeRect;
			}
			else
			{
				if (VerticalSmallRect.IsEmpty())
					FormatVerticalBar(FALSE,USE_BAR);
				return VerticalSmallRect;
 			}
		break;
	
	case DOCKBAR_TOP:
	case DOCKBAR_BOTTOM:
		if (DockBar->IsBigControls())
		{
			if (HorizontalLargeRect.IsEmpty())
				FormatHorizontalBar(FALSE,USE_BAR);
			return HorizontalLargeRect;
		}
		else
		{
			if (HorizontalSmallRect.IsEmpty())
				FormatHorizontalBar(FALSE,USE_BAR);
			return HorizontalSmallRect;
 		}
		break;
	default:
		ENSURE(FALSE,"Unknown Dockbar Type");

	} */
	return wxRect(0,0,0,0) ;
}

/********************************************************************************************

>	DockBarType BaseBar::IsPointerOverDock(wxPoint point)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/5/94
	Inputs:		point in Screen Coordinates
	Outputs:	-
	Returns:	The DockBarType that the pointer is over
	Purpose:	Test whether the pointer is currently over a dockingbar
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

DockBarType BaseBar::IsPointerOverDock(wxPoint point)
{
#pragma message( __LOCMSG__ "BaseBar::IsPointerOverDock - do nothing" )
	TRACE( _T("Warning - BaseBar::IsPointerOverDock called") );
/*	wxRect temp ;
	OILFixedDockingBar * ParentDockBar;
	 
	ParentDockBar = (OILFixedDockingBar * )GetMainFrame()->GetDockBar(DOCKBAR_LEFT); 
	ParentDockBar->GetDockScreenRect(&temp);
  	if (temp.PtInRect(point))
		return DOCKBAR_LEFT;

   	ParentDockBar = (OILFixedDockingBar * )GetMainFrame()->GetDockBar(DOCKBAR_RIGHT); 
	ParentDockBar->GetDockScreenRect(&temp);
   	if (temp.PtInRect(point))
		return DOCKBAR_RIGHT;
	
   	ParentDockBar = (OILFixedDockingBar * )GetMainFrame()->GetDockBar(DOCKBAR_TOP); 
	ParentDockBar->GetDockScreenRect(&temp);
	if (temp.PtInRect(point))
		return DOCKBAR_TOP;
	
   	ParentDockBar = (OILFixedDockingBar * )GetMainFrame()->GetDockBar(DOCKBAR_BOTTOM); 
	ParentDockBar->GetDockScreenRect(&temp);
   	if (temp.PtInRect(point))
		return DOCKBAR_BOTTOM; */

	return DOCKBAR_FLOAT;
}


/********************************************************************************************

>	BaseBar::SetBarSize(DockBarType BarType,BOOL DoRedraw)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/5/94
	Inputs:		BarType - type of dock the bar is attached to
				Redraw - flag to indicate a redraw is needed
	Outputs:	-
	Returns:	new size
	Purpose:	Set the Window size
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

wxRect  BaseBar::SetBarSize(DockBarType BarType ,BOOL DoRedraw)
{
	wxRect temp;

#pragma message( __LOCMSG__ "BaseBar::SetBarSize - do nothing" )
	TRACE( _T("Warning - BaseBar::SetBarSize called") );
/*	switch(BarType)
	{
	case DOCKBAR_FLOAT:
		if (ParentDockBar->IsBigControls())		
			temp = FloatLargeRect;
		else
			temp = FloatSmallRect;
		break;

	case DOCKBAR_LEFT:
	case DOCKBAR_RIGHT:
		if (ParentDockBar->IsBigControls())		
			temp = VerticalLargeRect;
		else
			temp = VerticalSmallRect;
		break;
	
	case DOCKBAR_TOP:
	case DOCKBAR_BOTTOM:
		if (ParentDockBar->IsBigControls())		
			temp = HorizontalLargeRect;
		else
			temp = HorizontalSmallRect;
		break;
	}
	UINT32 flags = SWP_NOMOVE|SWP_NOZORDER;
	if(! DoRedraw)
		flags |= SWP_NOREDRAW;
	SetWindowPos( NULL, 0, 0, temp.GetRight(), temp.GetBottom(), flags ); */

	return temp;
}
  
/********************************************************************************************

>	BaseBar::PositionControls(BOOL DoRedraw = TRUE)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/5/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Format and place controls according to docking bar type
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL  BaseBar::PositionControls(BOOL DoRedraw)
{
	// Position the controls in the bar
 	switch(ParentDockBarType)
	{
	case DOCKBAR_FLOAT:
		FormatFloatingBar(TRUE,USE_BAR);
		pOp->SetCurrentOrientation(Horizontal); 
		break;
	
	case DOCKBAR_LEFT:
	case DOCKBAR_RIGHT:
		FormatVerticalBar(TRUE,USE_BAR);
		pOp->SetCurrentOrientation(Vertical); 
		break;
	
	case DOCKBAR_TOP:
	case DOCKBAR_BOTTOM:
		FormatHorizontalBar(TRUE,USE_BAR);
		pOp->SetCurrentOrientation(Horizontal); 
		break;
	default:
		ENSURE(FALSE,"Unknown Parent DockBar");
	}
	// Size the window correctly
	SetBarSize(ParentDockBarType, DoRedraw);
	return TRUE ;
}


/********************************************************************************************

>	BOOL BaseBar::Show(DialogBarOp* Op)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>	  
	Created:	30/3/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if successful, else FALSE
	Purpose:	Creates a BaseBar window
	Errors:		-
	SeeAlso:	-	   

********************************************************************************************/


BOOL BaseBar::Show(DialogBarOp* Op)
{
#pragma message( __LOCMSG__ "BaseBar::Show - do nothing" )
	TRACE( _T("Warning - BaseBar::Show called") );
/*	if (Op->IS_KIND_OF(InformationBarOp))
		((InformationBarOp*)Op)->SetInfo();
	DockBarType InfoDock;

	InfoDock = Op->GetDockBarType();
	if(InfoDock != ParentDockBarType)
	{
		
		OILDockingBar * NewDock ;
		OILDockingBar * OldDock ;
       	BOOL CanCopy;
       	NewDock = GetMainFrame()->GetDockBar(InfoDock);
		OldDock = GetMainFrame()->GetDockBar(ParentDockBarType);
		CanCopy = NewDock->IsBigControls() == OldDock->IsBigControls();		
		if(HasOrientationChanged(ParentDockBarType,DragDockBarType))
			CanCopy = FALSE;
	
		// dynamically create a new BaseBar/derived Bar
		BaseBar* NewBaseBar = (BaseBar *)GetClassInfo()->CreateObject(); 

		NewBaseBar->Recreate(InfoDock, this, Op,CanCopy); 
		// The current BaseBar dies
		CCamApp::GetDlgManager()->Delete(GetSafeHwnd(),Op);  
		return TRUE;
	}

	KernelBarPos ThisPos;
	ThisPos.Dock = ParentDockBarType;
	 
	if (ParentDockBarType == DOCKBAR_FLOAT)
	{
		// Get x/y position of floating bar
		wxPoint FloatPos = Op->GetFloatingCPoint();
		ThisPos.x=FloatPos.x;
		ThisPos.y=FloatPos.y;
	}
	else
	{
		// Get slot/offset position of docked bar
		ThisPos.Slot = Op->GetSlot(); 
		ThisPos.Position = Op->GetOffset();
 		// Get a pointer to the docking bar
		OILFixedDockingBar * FixedDock = (OILFixedDockingBar*)GetMainFrame()->
										  GetDockBar(ParentDockBarType); 
	}
	// Attach the BaseBar to the docking bar 
	ParentDockBar->PlaceBar(GetSafeHwnd(),ThisPos);
	
	SetWindowText(Op->GetName());
	
	// send a create to info bars  so they can set the state of controls before 
	// we show the window

	// Show the Window
	ShowWindow(SW_SHOW);

	InvalidateRect(NULL,TRUE);  
	UpdateWindow(); */

	return TRUE;
}

/********************************************************************************************

>	BOOL BaseBar::Hide(DialogBarOp* Op)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>	  
	Created:	30/3/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if successful, else FALSE
	Purpose:	Creates a BaseBar window
	Errors:		-
	SeeAlso:	-	   

********************************************************************************************/
BOOL BaseBar::Hide(DialogBarOp* Op)
{
#pragma message( __LOCMSG__ "BaseBar::Hide - do nothing" )
	TRACE( _T("Warning - BaseBar::Hide called") );
/*	BOOL DoTidy = DO_TIDY;

	ShowWindow(SW_HIDE);
	
	if (Op->IS_KIND_OF(InformationBarOp))
		((InformationBarOp*)Op)->GetInfo();

#ifndef STANDALONE	
	// we dont want a tidy (reposition and redraw of bars)
	// if we are just switching out of the Selector Tool
	if(SelectorTool::IsSelectorCaching())
		DoTidy = DONT_TIDY;
#endif
	
	GetMainFrame()->GetDockBar(ParentDockBarType)->ReleaseBar(this->GetSafeHwnd(),DoTidy);
	//TRACE( _T("Hide now %d"),ParentDockBarType); */
	return TRUE;
}

/********************************************************************************************

>	BOOL BaseBar::Create(DialogBarOp* Op)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>	  
	Created:	30/3/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if successful, else FALSE
	Purpose:	Creates a BaseBar window
	Errors:		-
	SeeAlso:	-	   

********************************************************************************************/

BOOL BaseBar::Create(DialogBarOp* Op)
{
#pragma message( __LOCMSG__ "BaseBar::Create - do nothing" )
	TRACE( _T("Warning - BaseBar::Create called") );
/*	InRecreate = TRUE;
	pOp = Op; 

	#ifdef STANDALONE
	if (pOp->IS_KIND_OF(InformationBarOp))
	{	// We don't want any info bars in the standalone version.
		
		InRecreate = FALSE;
		return FALSE;
	}
	#endif

	ParentDockBarType = pOp->GetDockBarType(); // 	Get the current dock bar
	
	//TRACE( _T("Create now %d"),ParentDockBarType);  
	ParentDockBar = GetMainFrame()->GetDockBar(ParentDockBarType); 

	ENSURE(ParentDockBar != NULL, "Could not find docking bar"); 
	
  	// init control size etc ..
  	SetFormatVariables();
  
	KernelBarPos ThisPos;
	ThisPos.Dock = ParentDockBarType;
	 
	BOOL WindowCreated ; 

	wxPoint p ;

	if (ParentDockBarType == DOCKBAR_FLOAT)
//		|| pOp->IsKindOf(CC_RUNTIME_CLASS(SuperGallery)))
	{
		// Get x/y position of floating bar
		wxPoint FloatPos = pOp->GetFloatingwxPoint();

		if (!MonitorFromPoint(FloatPos, MONITOR_DEFAULTTONULL))
		{
			// The float pos is not on any monitor in the system so lets use the top left
			// corner of the monitor nearest the point
			MONITORINFO mi;
			mi.cbSize = sizeof(MONITORINFO);
			if (GetMonitorInfo(MonitorFromPoint(FloatPos, MONITOR_DEFAULTTONEAREST), &mi))
			{
				FloatPos.x = mi.rcWork.left;
				FloatPos.y = mi.rcWork.GetTop();
			}
			else
			{
				FloatPos.x = FloatPos.y = 0;
			}
		}
		
		ThisPos.x=FloatPos.x;
		ThisPos.y=FloatPos.y;
		
	
		//TRACE( _T("FLOAT At %d %d"),ThisPos.Slot,ThisPos.Position);
		// Set bar mode
	 	CurrentDlgBarMode = DIALOG; 

		// Create a popup floater
		WindowCreated = CreateEx(0, // WS_EX_TOOLWINDOW - I think we should've used one of these, Matt 1/11/00
								 NULL,
								 NULL, WS_POPUP |WS_CHILD,// | WS_BORDER, 
				 				 FloatPos.x, FloatPos.y,0,0,
				 				 GetMainFrame()->GetSafeHwnd(),   // Parent is the frame window 
				 				 NULL);
	}
	else
	{
		// Get slot/offset position of docked bar
		ThisPos.Slot = pOp->GetSlot(); 
		ThisPos.Position = pOp->GetOffset();
		CurrentDlgBarMode = BAR; 

		// Get a pointer to the docking bar
		OILFixedDockingBar * FixedDock = (OILFixedDockingBar*)GetMainFrame()->
										  GetDockBar(ParentDockBarType); 
		
		// Get a client coordinate for this slot/offset
		FixedDock->BarPosToScreen(ThisPos,&p);

		// Create a child window of the docking bar
		WindowCreated = CreateEx(0, NULL, 
									NULL, WS_CHILD | WS_BORDER, 
				 				 p.x,p.y,0,0,
				 				 ParentDockBar->GetSafeHwnd(), 
				 				 NULL);
	}

	if (!WindowCreated)
	{
		InRecreate = FALSE;
		return FALSE; 
	}
	
	if (!LoadResourceControls()|| (!CreateExtraControls()))
	{
		// We failed to load the controls 
		// Delete the window, and all children which may have been created 
		CCamApp::GetDlgManager()->Delete(GetSafeHwnd(), Op); 
		InRecreate = FALSE;
		return FALSE; 
	} 				

	if (!PositionControls())
	{
		InRecreate = FALSE;
		return FALSE;
	}
  	// Attach the BaseBar to the docking bar 
	ParentDockBar->PlaceBar(GetSafeHwnd(),ThisPos);
 
	// send a create to info bars  so they can set the state of controls before 
	// we show the window

	pOp->WindowID = (CWindowID)(GetSafeHwnd());
	BROADCAST_TO_CLASS(DialogMsg(GetSafeHwnd(), DIM_CREATE, NULL) ,DialogBarOp);  
	
	// Update the bar's state before showing
	Op->UpdateState();
	
	// Show the Window
	ShowWindow(SW_SHOW);
	::InvalidateRect(GetSafeHwnd(),NULL,TRUE);  
	::UpdateWindow(GetSafeHwnd());

	// inform help system
	ControlHelper::NotifyBarCreated(GetSafeHwnd());							 

	FormatHorizontalBar(FALSE,FORCE_SMALL);
	FormatVerticalBar(FALSE,FORCE_SMALL);
	FormatFloatingBar(FALSE,FORCE_SMALL);
	FormatHorizontalBar(FALSE,FORCE_LARGE);
	FormatVerticalBar(FALSE,FORCE_LARGE);
	FormatFloatingBar(FALSE,FORCE_LARGE);

	InRecreate = FALSE;
	
	SetWindowText(Op->GetName());
	
	// ZZZZ
	LastSmallBarPos =  ThisPos; 
	
	// get a pointer to the toolbar - so that we can do things with it!
	
	if (Op->GetName () == String_32 (TEXT ("Toolbar")))
	{
		PtrToToolBar = this;
	} */
	
	return TRUE; 
}



/********************************************************************************************

>	BaseBar::RecreateAt(KernelBarPos Pos,DockBarType DockBar,
						  BaseBar* BaseBar, DialogBarOp* Op,HWND OldHwnd )

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/5/94
	Inputs:		
				Pos  - Kernel position to place at ((slot/offset)|(x/y))
				Dockbar - Type of dock attached to
				BaseBar - item to recreate
				Op - attached op
				OldHwnd - Hwnd of previous incarnation 
	Outputs:	-
	Returns:	-
	Purpose:	ReCreates an existing bar after a change of controlsize - as the bar stays
				in the same dock we don't need to release / reattach - we need only inform
				the docking bar of the new HWND
	Errors:		-
	SeeAlso:	-

********************************************************************************************/


BOOL BaseBar::RecreateAt(KernelBarPos Pos,DockBarType DockBarTyp,
					  BaseBar* BaseBar, DialogBarOp* Op, wxWindow* OldHwnd) 
{
#pragma message( __LOCMSG__ "BaseBar::RecreateAt - do nothing" )
	TRACE( _T("Warning - BaseBar::RecreateAt called") );
/*	LastSmallBarPos = Pos;
	pOp = Op; 
	ParentDockBarType = DockBarTyp; // Record the current dock bar
	  
	ParentDockBar = GetMainFrame()->GetDockBar(DockBarTyp); 
 	
 	SetFormatVariables();

	wxRect NewBarSize = OldDragRect;	 // Screen coords 
	if (DockBarTyp == DOCKBAR_FLOAT)
//		|| pOp->IsKindOf(CC_RUNTIME_CLASS(SuperGallery)))
	{
		CurrentDlgBarMode = DIALOG; // For now 

		// Create a popup floater
		CreateEx(0,  NULL,
				NULL, WS_POPUP |WS_CHILD, //| WS_BORDER ,
				 Pos.x, Pos.y,0,0, 
				 GetMainFrame()->GetSafeHwnd(),   // Parent is the frame window 
				 NULL);

	}
	 else
	{
		CurrentDlgBarMode = BAR; // For now 
		OILFixedDockingBar * FixedDock = 
							(OILFixedDockingBar*)GetMainFrame()->GetDockBar(DockBarTyp); 
		wxPoint p;
		FixedDock->BarPosToScreen(Pos,&p);
		// Create a child bar
		CreateEx(0, NULL,
				NULL, WS_CHILD |  WS_BORDER ,
				 p.x,p.y,0,0, 
				 ParentDockBar->GetSafeHwnd(), 
				 NULL);
	}
	if (!LoadResourceControls()|| (!CreateExtraControls()))
	{
		// We failed to load the controls 
		// Delete the window, and all children which may have been created 
		CCamApp::GetDlgManager()->Delete(GetSafeHwnd(), Op);  
		return FALSE; 
	} 
	
	if (!PositionControls())
		return FALSE;


	//TRACE( _T("Done Recreate %d"),GetSafeHwnd());
	// Place BaseBar via docking bar 
	if (DockBarTyp == DOCKBAR_FLOAT)
	{
		ParentDockBar->PlaceBar(GetSafeHwnd(),Pos);  
	}
	else
	{
		OILFixedDockingBar * FixedDock = 
							(OILFixedDockingBar*)GetMainFrame()->GetDockBar(DockBarTyp); 
		if ((ParentDockBar->SetNewHwnd(OldHwnd,GetSafeHwnd()))==NULL)
			return FALSE;		
		FixedDock->PlaceBarAt(GetSafeHwnd(),Pos);  
 
	}
	// inform help system
	ControlHelper::NotifyBarCreated(GetSafeHwnd());		
	
		// send a create to info bars  so they can set the state of controls before 
	// we show the window
	
	pOp->WindowID = (CWindowID)(GetSafeHwnd());
	BROADCAST_TO_CLASS(DialogMsg(GetSafeHwnd(), DIM_CREATE, NULL) ,DialogBarOp);  
	
	// Update the bar's state before showing
	Op->UpdateState();

	// Show the Window
	ShowWindow(SW_SHOW);

	::InvalidateRect(GetSafeHwnd(),NULL,TRUE);  
	::UpdateWindow(GetSafeHwnd());
	
	FormatHorizontalBar(FALSE,FORCE_SMALL);
	FormatVerticalBar(FALSE,FORCE_SMALL);
	FormatFloatingBar(FALSE,FORCE_SMALL);
	FormatHorizontalBar(FALSE,FORCE_LARGE);
	FormatVerticalBar(FALSE,FORCE_LARGE);
	FormatFloatingBar(FALSE,FORCE_LARGE);
	
	SetWindowText(Op->GetName());

	if (Op->GetName () == String_32 (TEXT ("Toolbar")))
	{
		PtrToToolBar = this;
	} */

	return TRUE; 
}



/********************************************************************************************

>	BaseBar::Recreate(DockBarType DockBar, BaseBar* BaseBar, DialogBarOp* Op,BOOL CanCopy )

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/3/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	
	Errors:		-
	SeeAlso:	-

********************************************************************************************/


BOOL BaseBar::Recreate(DockBarType DockBarTyp, BaseBar* BaseBar, DialogBarOp* Op,BOOL CanCopy ) 
{
#pragma message( __LOCMSG__ "BaseBar::Recreate - do nothing" )
	TRACE( _T("Warning - BaseBar::Recreate called") );
/*	pOp = Op; 
	ParentDockBarType = DockBarTyp; // Record the current dock ba
	  
	ParentDockBar = GetMainFrame()->GetDockBar(DockBarTyp); 

	SetFormatVariables();

	wxRect NewBarSize = OldDragRect;	 // Screen coords 

	////////////////////////////////////////////////////////////////////////////////

	// 'dynamic' infobar update logic

	////////////////////////////////////////////////////////////////////////////////
	
	String_32 dialogBarOpName = Op->GetName ();

	if (dialogBarOpName == TEXT (String_32 (_R(IDS_FILLTOOL_FILLINFOBARNAME))))
//		|| add the name of your 'dynamic' infobar here
	{
		//pOp->ResetHiddenGadgetStates ();
	}

	////////////////////////////////////////////////////////////////////////////////
	
	if (DockBarTyp == DOCKBAR_FLOAT)
	{
		CurrentDlgBarMode = DIALOG; // For now 

		// Create a popup floater
		CreateEx(0,  NULL,
				NULL, WS_POPUP |WS_CHILD,// | WS_BORDER ,
				 NewBarSize.GetLeft()+(NewBarSize.GetWidth()/2), NewBarSize.GetTop()+(NewBarSize.GetHeight()/2), NewBarSize.GetWidth(), NewBarSize.GetHeight(), 
				 GetMainFrame()->GetSafeHwnd(),   // Parent is the frame window 
				 NULL);

	}
	else
	{
		CurrentDlgBarMode = BAR; // For now 
		
		// Convert the NewBarSize to client coords
		::ScreenToClient(ParentDockBar->GetSafeHwnd(), &NewBarSize.TopLeft()); 
		::ScreenToClient(ParentDockBar->GetSafeHwnd(), &NewBarSize.BottomRight()); 

		// Create a child bar
		CreateEx(0,  NULL, NULL, WS_CHILD|  WS_BORDER ,
				 NewBarSize.GetLeft(), NewBarSize.GetTop(), NewBarSize.GetWidth(), NewBarSize.GetHeight(), 
				 ParentDockBar->GetSafeHwnd(), 
				 NULL); 

	}
	if (CanCopy)
	{
		if (!CopyOrLoadControls(GetSafeHwnd())|| (!CreateExtraControls()))
		{
			// We failed to load the controls 
			// Delete the window, and all children which may have been created 
			CCamApp::GetDlgManager()->Delete(GetSafeHwnd(),Op);  
			return FALSE; 
		} 	
	}
	else
	{
		if (!LoadResourceControls()|| (!CreateExtraControls()))
		{
			// We failed to load the controls 
			// Delete the window, and all children which may have been created 
			CCamApp::GetDlgManager()->Delete(GetSafeHwnd(),Op);  
			return FALSE; 
		} 
	}
	if (! PositionControls())
		return FALSE;

	// Attach the BaseBar to the docking bar 
	ParentDockBar->PlaceBar(GetSafeHwnd(), wxPoint(OldDragRect.GetLeft(), OldDragRect.GetTop()));  
	// inform help system
	
	pOp->WindowID = (CWindowID)(GetSafeHwnd());

	BROADCAST_TO_CLASS(DialogMsg(GetSafeHwnd(), DIM_CREATE, NULL) ,DialogBarOp);  

	wxRect temprect;
	GetClientRect(&temprect);

	// Update the bar's state before showing
	Op->UpdateState();

	//if (Op->GetName () != String_32 (TEXT ("Toolbar")))
	{
		// Show the Window
		ShowWindow(SW_SHOW);

		::InvalidateRect(GetSafeHwnd(),NULL,TRUE);  
		::UpdateWindow(GetSafeHwnd());
	}
	
	ControlHelper::NotifyBarCreated(GetSafeHwnd());		

	FormatHorizontalBar(FALSE,FORCE_SMALL);
	FormatVerticalBar(FALSE,FORCE_SMALL);
	FormatFloatingBar(FALSE,FORCE_SMALL);
	FormatHorizontalBar(FALSE,FORCE_LARGE);
	FormatVerticalBar(FALSE,FORCE_LARGE);
	FormatFloatingBar(FALSE,FORCE_LARGE);
	
	SetWindowText(Op->GetName());
	
	// ZZZZZ
	LastSmallBarPos =  * GetMainFrame()->GetDockBar(ParentDockBarType)->
					 GetBarPos(this->GetSafeHwnd());

	if (Op->GetName () == String_32 (TEXT ("Toolbar")))
	{
		PtrToToolBar = this;
	} */
					 	
	return TRUE; 
}

/********************************************************************************************

>	BaseBar::LoadResourceControls()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/4/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose: 	
	Errors:		-
	SeeAlso:	-

********************************************************************************************/


BOOL BaseBar::LoadResourceControls()
{
	 // Find out how many bar items
	UINT32 NumBarItems = pOp->GetNumBarItems(); 

	BarItem* pBarItem;

	// track ddecker state 
	DDeckerState = DDECKER_NONE; 

//	ThisIsAnInfoBar = ( (pOp->IsKindOf(CC_RUNTIME_CLASS(InformationBarOp)))
//					    || (pOp->IsKindOf(CC_RUNTIME_CLASS(SuperGallery))) );

	ThisIsAnInfoBar = pOp->IsKindOf(CC_RUNTIME_CLASS(InformationBarOp));
	BOOL ThisIsAGallery = pOp->IsKindOf(CC_RUNTIME_CLASS(SuperGallery));

	for (UINT32 i = 0; i< NumBarItems; i++)
	{
		// Get the next BarItem
		pBarItem = pOp->GetPtrBarItem(i); 
		
		ENSURE(pBarItem != NULL, "NumBarItems is incorrect"); 
		// Check whether we need to create thin items for double deckers
		
		if (pBarItem->IsKindOf(CC_RUNTIME_CLASS(BarControl)))
		{
			BarControl * pBarControl = (BarControl*)pBarItem;
			if (ThisIsAnInfoBar || ThisIsAGallery)
			{ 
				// If the DialogBarOp is an InformationBarOp then the Bar controls 
				// control ID will be unique for the InformationBar's Message handler
				pBarControl->SetUniqueGadgetID(
					pBarControl->GetBarControlInfo(pOp->IsHorizontal()).ControlID);  
			}
			else
			{
				// use index as gadget id
				pBarControl->SetUniqueGadgetID(i + 100);  
			}
			// create the control i==0 indicates initialise
			if (!CreateInfoControlFromResource(pBarControl,i==0))
			{
				// We failed to create the control 
				//ENSURE(FALSE,"Failed to create control"); 
				return FALSE ;
			}
		}
		else if (pBarItem->IsKindOf(CC_RUNTIME_CLASS(DDeckerTop)))
		{
			DDeckerState = DDECKER_TOP;
		}
		else if (pBarItem->IsKindOf(CC_RUNTIME_CLASS(DDeckerBottom)))
		{
			DDeckerState = DDECKER_BOTTOM;
		}
		else if (pBarItem->IsKindOf(CC_RUNTIME_CLASS(DDeckerEnd)))
		{
			DDeckerState = DDECKER_NONE;
		}
	}

	// call to free resources;
	CreateInfoControlFromResource(NULL,0);
	return TRUE; 
}; 

/********************************************************************************************

>	BaseBar::CopyOrLoadResourceControls()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/4/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose: 	
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL BaseBar::CopyOrLoadControls(wxWindow* BarHwnd)
{
	PORTNOTETRACE( "dialog", "BaseBar::CopyOrLoadControls - do nothing" );
#ifndef EXCLUDE_FROM_XARALX
	// Find out how many bar items
	UINT32 NumBarItems = pOp->GetNumBarItems(); 

	// track ddecker state 
	DDeckerState = DDECKER_NONE;

	BarItem* pBarItem; 

 	ThisIsAnInfoBar = (pOp->IsKindOf(CC_RUNTIME_CLASS(InformationBarOp)));

	for (UINT32 i = 0; i< NumBarItems; i++)
	{
		// Get the next BarItem
		pBarItem = pOp->GetPtrBarItem(i); 

		ENSURE(pBarItem != NULL, "NumBarItems is incorrect"); 

		// Check whether we need to create thin items for double deckers
	
		if (pBarItem->IsKindOf(CC_RUNTIME_CLASS(BarControl)))
		{
			BarControl * pBarControl = (BarControl*)pBarItem;
			

			// Obtain handle to the controls HWnd
			HWND hWndControl=(HWND)(pBarControl)->GetWinID();  
			// -------------------------------------------------------
			// Find out if the control is copyable
			// - This code may appear a little on the brink of insanity, however
			// it is neccessary cos windows is edit/combo/list set parently 
			// challenged.

			// Determine the control's class
			
			switch (GetEnumItemType(pBarItem))
			{
				case BUTTON:
				case LIST:
				case COMBO:
				case STATIC:
				case EDIT:
				case BITMAPCOMBO1D:		// IF THIS ISN'T HERE - THEN FOR SOME
										// BIZARRE REASON - OUR CONTROL CANNOT ACCESS
										// ITS DATA, AND CAMELOT FALLS OVER!
				case BITMAPCOMBO2D:
	 			{
					// The control cannot be copied, so must be recreated from the
					// resource.

					if (!CreateInfoControlFromResource(pBarControl,i==0))
					{
						// We failed to create the control 
						ENSURE(FALSE,"Failed to create control"); 
						return(FALSE);
					}
				}
				break;
				default :
				{
					// We can safely copy the control to its new parent
					::SetParent(hWndControl, BarHwnd);
				}
				break;
			}
		}
		else if (pBarItem->IsKindOf(CC_RUNTIME_CLASS(DDeckerTop)))
		{
			DDeckerState = DDECKER_TOP;
		}
		else if (pBarItem->IsKindOf(CC_RUNTIME_CLASS(DDeckerBottom)))
		{
			DDeckerState = DDECKER_BOTTOM;
		}
		else if (pBarItem->IsKindOf(CC_RUNTIME_CLASS(DDeckerEnd)))
		{
			DDeckerState = DDECKER_NONE;
		} 
	}
	CreateInfoControlFromResource(NULL,0);
#endif
	return TRUE; 
}; 


/********************************************************************************************

>	BaseBar::GetBarPosition()

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/4/94
	Inputs:		-
	Outputs:	-
	Returns:	Pointer to KernelBarPos Object containing current Slot / Offset
	Purpose: 	- 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

KernelBarPos  * BaseBar::GetBarPosition()
{
#pragma message( __LOCMSG__ "BaseBar::GetBarPosition - do nothing" )
	TRACE( _T("Warning - BaseBar::GetBarPosition called") );
//	return GetMainFrame()->GetDockBar(ParentDockBarType)->GetBarPos(GetSafeHwnd());

	return NULL;
}; 

/********************************************************************************************

>	BaseBar::GetDockBarType()

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/4/94
	Inputs:		-
	Outputs:	-
	Returns:	Parent DockBarType
	Purpose: 	- 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

DockBarType BaseBar::GetDockBarType()
{
 
 return ParentDockBarType;
  
};


/********************************************************************************************

>	BaseBar::IsDoubleDecker(BarItem * ThisControl,BarItem * Start,BarItem * End)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/4/94
	Inputs:		ThisControl Control to test
				Start to return a pointer to first item in the DD
				End to return a pointer to last item in  the DD
	Outputs:	Pointers to first and last controls in this group if one found
	Returns:	DDECKER_NONE / DDECKER_TOP /DDECKER_BOTTOM
	Purpose: 	to determine whether a given control is part of a double decker group 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

DDeckerReadState BaseBar::IsDoubleDecker(BarItem * ThisControl,BarItem * Start,BarItem * End)
{
	BarItem * pBarItem = pOp->GetPtrBarNext(ThisControl) ; 	
	BarItem * LastItem = NULL;
	// Test assumes that the bar list will contain DDeckerTop/DDeckerBottom/DDeckerEnd
	// in that order                           	

	DDeckerReadState State = DDECKER_NONE;
	while ( pBarItem != NULL )
	{

		if (pBarItem->IsKindOf(CC_RUNTIME_CLASS(DDeckerTop)))
		{	
			State = DDECKER_NONE;
		}

		else if (pBarItem->IsKindOf(CC_RUNTIME_CLASS(DDeckerBottom)))
		{	
			State = DDECKER_TOP;
		}

		else if (pBarItem->IsKindOf(CC_RUNTIME_CLASS(DDeckerEnd)))
		{	
			if(State == DDECKER_NONE)
				State = DDECKER_BOTTOM;

			// point End to last control in group
			if(LastItem != NULL && End != NULL)
				End = LastItem;
		}

		// keep a pointer to the last control
		else if ( pBarItem->IsKindOf(CC_RUNTIME_CLASS(BarControl)))
		{
			LastItem = pBarItem;
		}
		pBarItem = pOp->GetPtrBarNext(pBarItem); 
	}

	// point Start to the first control in this group
	if(Start != NULL && State != DDECKER_NONE )
	{
		
		pBarItem = ThisControl; 	
		BarItem * Head = pOp->GetPtrBarHead();

		// run backwards through the list until we find the start 
		// of the double decker or get to the head of the bar list
		while(pBarItem != Head)
		{
			// check for start of double decker
			if (pBarItem->IsKindOf(CC_RUNTIME_CLASS(DDeckerTop)))
			{	
				Start = LastItem;
				break;
			}

			// keep a pointer to the last control
			else if ( pBarItem->IsKindOf(CC_RUNTIME_CLASS(BarControl)))
			{
				LastItem = pBarItem;
			}
			pBarItem = pOp->GetPtrBarPrev(pBarItem); 
		}
	}

	// not a double decker - set Start and End to NULL
	if( State == DDECKER_NONE)
		Start = End = NULL;

   	return State; 
};

/********************************************************************************************

>	BaseBar::TidyBarItemsAfterDrag()

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/4/94
	Returns:	BOOL 
	Purpose: 	Tidy up Linefeeds and Separators after dragging controls 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL BaseBar::TidyBarItemsAfterDrag()
{
	// nothing to tidy
	if(pOp->GetNumBarItems()==0)
		return TRUE;
		 
	// remove any SP's that preceed LF's 
	BarItem * pBarItem = pOp->GetPtrBarHead(); 
	while (pBarItem != NULL)
	{
		if (pBarItem->IsKindOf(CC_RUNTIME_CLASS(BarLineFeed)))
		{
			BarItem * pLastItem = pOp->GetPtrBarPrev(pBarItem);
			if(pLastItem)
			{
				if(pLastItem->IsKindOf(CC_RUNTIME_CLASS(BarSeparator)))
				{
			 		pOp->RemoveBarItem(pLastItem);
					delete pLastItem;
				}
			}
		}
		pBarItem = pOp->GetPtrBarNext(pBarItem); 
	}

	// stip leading and multiple line feeds - we can get into this state after dragging controls around
	BOOL LF = FALSE;
	BOOL FoundControl=FALSE;
	BarItem * pLastControl = NULL;
	pBarItem = pOp->GetPtrBarHead(); 

	while (pBarItem != NULL)
	{
		// keep a record of the last control so that we can strip any trailing LFs
		if ( pBarItem->IsKindOf(CC_RUNTIME_CLASS(BarControl)))
   	 	{
   	 		pLastControl = pBarItem;
			FoundControl = TRUE;
		}
		if (pBarItem->IsKindOf(CC_RUNTIME_CLASS(BarLineFeed)))
		{
			BarItem * pFoundItem = pBarItem;
			pBarItem = pOp->GetPtrBarNext(pBarItem);
			
			// either it's a mulitiple (LF is set) or we haven't found any controls yet
			if(LF || !FoundControl)						
			{
				pOp->RemoveBarItem(pFoundItem);
				delete pFoundItem;
			}
			else
			{
				if(FoundControl)
					LF = TRUE;
			} 
		}
		else
		{
			LF = FALSE;
			pBarItem = pOp->GetPtrBarNext(pBarItem); 
		}
	}

	// stip leading and multiple Separators - we can get into this state after dragging controls around
	BOOL SP = FALSE;
	FoundControl=FALSE;
	pLastControl = NULL;
	pBarItem = pOp->GetPtrBarHead(); 

	while (pBarItem != NULL)
	{
		// keep a record of the last control so that we can strip any trailing SPs
		if ( pBarItem->IsKindOf(CC_RUNTIME_CLASS(BarControl)))
   	 	{
   	 		pLastControl = pBarItem;
			FoundControl = TRUE;
		}
		if (pBarItem->IsKindOf(CC_RUNTIME_CLASS(BarSeparator)))
		{
			BarItem * pFoundItem = pBarItem;
			pBarItem = pOp->GetPtrBarNext(pBarItem);
			// either it's a mulitiple (SP is set) or we haven't found any controls yet
			if(SP || !FoundControl)						
			{
				pOp->RemoveBarItem(pFoundItem);
				delete pFoundItem;
			}
			else
			{
				if(FoundControl)
					SP = TRUE;
			} 
		}
		else
		{
			SP = FALSE;
			pBarItem = pOp->GetPtrBarNext(pBarItem); 
		}
	}

	// we don't need anything after the last control
	// so.... move past it
	pLastControl = pOp->GetPtrBarNext(pLastControl); 
	
	// and delete the rest
	while(pLastControl)
	{
		BarItem * pItem = pLastControl;
		pLastControl = pOp->GetPtrBarNext(pLastControl);
		delete pOp->RemoveBarItem(pItem);
   	}

	return TRUE; 
};

/********************************************************************************************

>	BOOL BaseBar::BaseBar::CacheFormatingData()
	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/2/96
	Inputs:		-
	Outputs:	-
	Returns:	Failure/Success
	Purpose: 	Delete and recache  bar height to width mapping values - these are required
				for vertical sizing -
				There is one limitation with this system - we can't use height values to find every 
				possible format - many widths can map to one height..we compromise and cache the 
				narrowest width for each height
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL BaseBar::CacheFormatingData()
{
	// delete the last cache
	if(!FormatPositions.IsEmpty())
		FormatPositions.DeleteAll();

	FormatPosition * pNewFP = NULL;
	wxSize BarSize(0,0);
	wxSize LastBarSize(0,0);
	INT32 NoLFs = -1;

	// incrementally call FormatLineFeeds - cache the value every time we get a new height
	// when there are no LF's in the bar we have found it's widest format
	for(INT32 TryWidth = 16;NoLFs!=0;TryWidth +=4)
	{
		FormatLineFeeds(TryWidth,&BarSize,&NoLFs);
	
		// there may be many valid widths that map to this height
		// we just cache the first one
		if(BarSize.x != LastBarSize.x)
		{
			LastBarSize = BarSize;
			pNewFP = new FormatPosition( TryWidth, BarSize.x );
			
			ERROR2IF(pNewFP == NULL,FALSE,"Couldn't create Format Position");
			
			// we add to new item to the head so that we can search from the shortest up
			// in  GetBestCachedWidth()
			FormatPositions.AddHead((ListItem *)pNewFP);	
		}
	}
	
	return TRUE;
};

/********************************************************************************************

>	BOOL BaseBar::BaseBar::GetBestCachedWidth(INT32 TargetHeight,INT32 * FoundWidth)
	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/2/96
	Inputs:		TargetHeight:
	Outputs:	A cached Width
	Returns:	Failure/Success
	Purpose: 	given a height return a cached Width value to use during vertical resizing
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL BaseBar::GetBestCachedWidth(INT32 TargetHeight, INT32 * FoundWidth)
{
	ERROR2IF(FormatPositions.IsEmpty(),FALSE,"No Format Cache");
	
	FormatPosition * pFormatPosition=(FormatPosition *) FormatPositions.GetHead(); 

	ERROR2IF(pFormatPosition==NULL,FALSE,"No Format Cache");
  	
	INT32 LastWidth = pFormatPosition->Width;
	
	// find the cached Width that maps onto the target height
	while (pFormatPosition)				
	{
		// break if we have gone past the target height
		if (pFormatPosition->Height>TargetHeight)	//found the bar
			break;
				
		// record this width - we will output the "last" value
		LastWidth = pFormatPosition->Width;
		pFormatPosition =(FormatPosition *) FormatPositions.GetNext(pFormatPosition);	
	
	}// end while

	* FoundWidth = LastWidth;
	
	return TRUE;
};

/********************************************************************************************

>	BaseBar::FormatLineFeeds(INT32 Targetx,wxSize * pBarSize, INT32 * pNoLFs)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/4/94
	Inputs:		TargetWidth
	Outputs:	wxSize* pBarSize:returns the size of the bar - used during vertical resizing
				INT32* pNoLFs:the number of linefeeds in the bar -used during vertical resizing
	Returns:	-
	Purpose: 	Reformat a floating toolbar 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL BaseBar::FormatLineFeeds(INT32 Targetx,wxSize * pBarSize, INT32 * pNoLFs)
{
#pragma message( __LOCMSG__ "BaseBar::FormatLineFeeds - do nothing" )
	TRACE( _T("Warning - BaseBar::FormatLineFeeds called") );
/*	//List is empty	just return 
		if (pOp->IsListEmpty())
		return FALSE;

	INT32 xpos = 0;
	INT32 WidestControl=0;
	INT32 CarryWidth = 0;
	INT32 LastWidth = 0;
	INT32 StartControlX =0;
	INT32 ControlHeight ;
	INT32 BarWidth =0;
	
	if(pNoLFs)
		(* pNoLFs) = 0;

	OILDockingBar * ThisDock ;
	ThisDock = GetMainFrame()->GetDockBar(DOCKBAR_FLOAT);
	
	// are we on a big/small dock
	if (ThisDock->IsBigControls())												
		ControlHeight = LARGEBUTTONHEIGHT;		
    else
		ControlHeight = SMALLBUTTONHEIGHT;		
   		
	INT32 BarHeight = ControlHeight+TitleBarHeight;		// UIC
	
	INT32 DDeckerStart = 0;
	INT32 DDeckerEnd = 0;

	Targetx += BAR_BORDERSIZE;

    BarItem * pBarItem = NULL; 
	BarItem * SeparatorPtr = NULL;
	BarItem * head = NULL;
	BOOL GroupSplit = FALSE;
	BOOL LastItemWasDDTop = FALSE;
	BOOL FirstControlInDecker = FALSE;
	BOOL ItsABarItem = FALSE;
	DDeckerReadState DDeckerState = DDECKER_NONE;
  
	// Remove the existing linefeeds
	pBarItem = pOp->GetPtrBarHead(); 

	while (pBarItem != NULL)
	{
		if ( pBarItem->IsKindOf(CC_RUNTIME_CLASS(BarControl)))
   	 	{
	   	 		wxRect WindSize;
				::GetClientRect((HWND)((BarControl*)pBarItem)->GetWinID(),&WindSize);
				if(WindSize.GetWidth()>WidestControl)
					WidestControl = WindSize.GetWidth();
		}
		if (pBarItem->IsKindOf(CC_RUNTIME_CLASS(BarLineFeed)))
		{
			BarItem * FoundItem = pBarItem;
			pBarItem = pOp->GetPtrBarNext(pBarItem);
			pOp->RemoveBarItem(FoundItem);
			delete FoundItem; 
		}
		else
			pBarItem = pOp->GetPtrBarNext(pBarItem); 
	}
	// we can't make the bar narrower than the widest control
	if(Targetx <(WidestControl+(BAR_BORDERSIZE*2)))
	{
		Targetx = WidestControl + (BAR_BORDERSIZE*2) ;
	}
	//TRACEUSER( "chris", _T("\n -- Format line feeds -- \n\n"));
    // Insert new LineFeeds
	head = pBarItem = pOp->GetPtrBarHead(); 	
	
	while ( pBarItem != NULL )
	{
		
		ENSURE(!pBarItem->IsKindOf(CC_RUNTIME_CLASS(BarLineFeed)),"LineFeed still in List");
		// Double Decker stuff...
		if (pBarItem->IsKindOf(CC_RUNTIME_CLASS(DDeckerTop)))
		{
			DDeckerState = DDECKER_TOP;
			DDeckerStart = xpos;
			LastItemWasDDTop = TRUE;
//			TRACEUSER( "chris", _T("DD Top\n"));
		}
		if (pBarItem->IsKindOf(CC_RUNTIME_CLASS(DDeckerBottom)))
		{
			DDeckerState = DDECKER_BOTTOM;
			DDeckerEnd = xpos;
			// go back to start
			xpos = DDeckerStart;
//			TRACEUSER( "chris", _T("DD Bottom\n"));
		}
		if (pBarItem->IsKindOf(CC_RUNTIME_CLASS(DDeckerEnd)))
		{
			DDeckerState = DDECKER_NONE;
			if(xpos < DDeckerEnd)
				xpos = DDeckerEnd;
//			TRACEUSER( "chris", _T("DD End\n"));
  		}
		if ( pBarItem->IsKindOf(CC_RUNTIME_CLASS(BarSeparator)) )
		{
			if( DDeckerState == DDECKER_NONE)
			{
				if(pBarItem!=head)
				{
					SeparatorPtr = pBarItem;
					CarryWidth = xpos;
				}
			}
			xpos += SeparatorWidth;
		}
		else if ( (ItsABarItem = pBarItem->IsKindOf(CC_RUNTIME_CLASS(BarControl)))||DDeckerState != DDECKER_NONE )
		{
			if ( ItsABarItem)
   	 		{
	   	 		wxRect WindSize;
				::GetClientRect((HWND)((BarControl*)pBarItem)->GetWinID(),&WindSize);

				LastWidth = WindSize.GetWidth();
				xpos += WindSize.GetWidth();
				// we need a record of the widest line
				if(xpos>BarWidth)
						BarWidth= xpos;
				FirstControlInDecker = FALSE;
				if(LastItemWasDDTop)
				{
				   FirstControlInDecker = TRUE;
				   LastItemWasDDTop = FALSE;
				}
//	   	 		TRACEUSER( "chris", _T("Control Size %d"),LastWidth);
			}
//			TRACEUSER( "chris", _T("targetx - %d xpos - %d \n"),Targetx,xpos);
          	// Control overlaps TargetWidth - don't insert at head of list    
		    if ( xpos >= Targetx && pBarItem != head) 
			{
		      	// We can't split if we are in a double decker control 					    
		        BOOL CanSplit = FALSE;
		        if(FirstControlInDecker)
					CanSplit = TRUE;
				if(DDeckerState == DDECKER_NONE)
					CanSplit = TRUE;
//				TRACEUSER( "chris", _T("%s\n",CanSplit ?"Can Split ":"Can't Split"));
				if(CanSplit)
				{
		           	BarItem * NewLineFeed = new BarLineFeed;
					if(pNoLFs)
						(* pNoLFs)++;
    				BarHeight += ControlHeight+VERTICAL_SPACE;
		        	if ( SeparatorPtr != NULL )			// insert a LineFeed before the last separator
					{
						pOp->InsertItemBefore(SeparatorPtr,NewLineFeed);
		    			SeparatorPtr = NULL;
						xpos -= CarryWidth;				// decrease xpos by pos of last separator  
						DDeckerStart = xpos;
						GroupSplit = FALSE;
						BarHeight += SeparatorWidth; 
//						TRACEUSER( "chris", _T("Separator split - xpos %d\n"),xpos);
					}
					else 					// no separator on this line 
		            {
				
						if(!FirstControlInDecker)
							pOp->InsertItemBefore(pBarItem,NewLineFeed);
						else
							pOp->InsertItemBefore(pOp->GetPtrBarPrev(pBarItem),NewLineFeed);
						xpos = LastWidth;				// xpos set to width of last control
						if(FirstControlInDecker)
						 	DDeckerStart = xpos-LastWidth;
						GroupSplit = TRUE ;			   	// we must have split a group
//						TRACEUSER( "chris", _T("Normal split - xpos %d\n"),xpos);
						//LastWidth = 0;
		            }
				}
			}
			else if (GroupSplit && SeparatorPtr)
			{
				
					BarItem * NewLineFeed = new BarLineFeed;
					if(pNoLFs)
						(* pNoLFs)++;
					pOp->InsertItemBefore(SeparatorPtr,NewLineFeed);
		   			SeparatorPtr = NULL;
					xpos -= CarryWidth;				// decrease xpos by pos of last separator
					DDeckerStart = xpos;
					GroupSplit = FALSE;	
//					TRACEUSER( "chris", _T("Group split - xpos %d\n"),xpos);
					BarHeight += ControlHeight+VERTICAL_SPACE +  SeparatorWidth; 
			}
		}
		pBarItem = pOp->GetPtrBarNext(pBarItem); 
	}
	
	HeightEstimate = BarHeight;
	if(pBarSize)
		* pBarSize = wxSize(BarWidth,BarHeight); */
   	return TRUE; 
};


/********************************************************************************************

>	BaseBar::GetNearestControl(wxPoint DropPnt)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/4/94
	Inputs:		-
	Outputs:	-
	Returns:	An index to place the new control in the bar item list
	Purpose: 	Return the nearest control given a mouse position in client coords
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

UINT32 BaseBar::GetNearestControl(wxPoint DropPoint)
{
#pragma message( __LOCMSG__ "BaseBar::GetNearestControl - do nothing" )
	TRACE( _T("Warning - BaseBar::GetNearestControl called") );
/*	HWND hcontrol;			 						// handle of current control
	INT32 ControlIndex = 0;	 						// index in bar item list
	BarItem * pBarItem; 		 					// baritem pointer
	BarItemType ThisControl; 						// type of current item
	DDeckerReadState DDeckerState = DDECKER_NONE;	// enum to record current double decker state
	// check orientation of the bar we are dropping on
	BOOL bIsHorizontal = pOp->IsHorizontal();		
	
	
	// if  the bar is empty it must be the first item on the list
	if (pOp->IsListEmpty())
		return ControlIndex ;

	// get head	item of the bar item list
	pBarItem = pOp->GetPtrBarHead(); 

	while (pBarItem != NULL)
	{
		
		ThisControl = GetEnumItemType(pBarItem);
		// Double Decker stuff...
		switch(ThisControl)
		{
			case DECKER_TOP:
				DDeckerState = DDECKER_TOP;
				break;
			case DECKER_BOTTOM:
				DDeckerState = DDECKER_BOTTOM;
				break;
			case DECKER_END:
				DDeckerState = DDECKER_NONE;
				break;
		}

		if(DDeckerState != DDECKER_TOP)
		{
			if (pBarItem->IsKindOf(CC_RUNTIME_CLASS(BarControl)))
			{
			 
				// get the handle
				hcontrol=(HWND)((BarControl*)pBarItem)->GetWinID();
			
				// Get window rect - screen coords
				wxRect WindSize;
				::GetWindowRect(hcontrol,&WindSize);		   		
			
				// to Client - mouse messages are in client coords
				ScreenToClient(&WindSize);
			
				// expand the rect slightly to the top and bottom
				wxRect HitTest(WindSize.GetLeft(),WindSize.GetTop()-4,WindSize.GetRight(),WindSize.GetBottom()+4);
				
				// check the next item - if it's a line feed we might have a load of empty space to the right
				// of this control - in which case we'll expand the hit test rect to include it
				BarItem * pNextBarItem = pOp->GetPtrBarNext(pBarItem);
				if(pNextBarItem )
				{
					if(bIsHorizontal)
					{
						if(pNextBarItem->IsKindOf(CC_RUNTIME_CLASS(BarLineFeed)))
						{
							wxRect ThisWindowRect;
							GetWindowRect(&ThisWindowRect);
						 	HitTest.GetRight() += (ThisWindowRect.GetRight() - WindSize.GetRight());
						}
						else if (pNextBarItem->IsKindOf(CC_RUNTIME_CLASS(BarControl)))
						{
						  	// get the handle
							HWND hNextControl=(HWND)((BarControl*)pNextBarItem)->GetWinID();
							// Get window rect - screen coords
							wxRect NextWindSize;
							::GetWindowRect(hNextControl,&NextWindSize);
							// to Client - mouse messages are in client coords
							ScreenToClient(&NextWindSize);
						
							// the hit test area should extends from the control's left edge
							// to the next control's left edge
							if(NextWindSize.GetLeft() > HitTest.GetRight())
								HitTest.GetRight() = NextWindSize.GetLeft();
						}
					}
					BarItem * pLastBarItem = pOp->GetPtrBarPrev(pBarItem);
					if(pLastBarItem)
					{
						if (pLastBarItem->IsKindOf(CC_RUNTIME_CLASS(BarSeparator)))
						{
							if(bIsHorizontal)
								HitTest.GetLeft() -= SeparatorWidth;
							else
								HitTest.GetTop() -=SeparatorWidth;
						}
					}	
				}

				// if the mouse is in this area we have found our drop point
				if(HitTest.PtInRect(DropPoint))
					return ControlIndex ;
				
			}
		}
		
		// increment index
		ControlIndex ++;
		
		// we skip past the dragged control as it will be deleted later
		// this avoids the problem where a control is dropped into the
		// correct place only to be shifted to the left after the original control is deleted
		if(DragCtlHwnd==hcontrol)
			ControlIndex ++;
		
		// move to next item
		pBarItem = pOp->GetPtrBarNext(pBarItem); 
	}
	
	// off the end !
	// return index of last item
	return pOp->GetNumBarItems(); */

	return (UINT32)-1;
};


/****************************************************************************

>	BOOL BaseBar::FormatToolBar (INT32 NewWidth)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/11/2004

	Inputs:		NewWidth	- 
	Returns:	TRUE if ok, FALSE if bother
	Purpose:	

****************************************************************************/

BOOL BaseBar::FormatToolBar (INT32 NewWidth)
{
	if (this != PtrToToolBar)
	{
		return (FALSE);
	}
	else
	{
		FormatLineFeeds (NewWidth);
		FormatFloatingBar(FALSE,USE_BAR);

		return (TRUE);
	}
}




/****************************************************************************

>	BOOL BaseBar::FormatBar(BOOL MoveWindows,ForceControlSize ForceSize)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/11/2004

	Inputs:		MoveWindows	- 
				ForceSize	- 
	Returns:	TRUE if ok, FALSE if bother
	Purpose:	

****************************************************************************/

BOOL BaseBar::FormatBar(BOOL MoveWindows,ForceControlSize ForceSize)
{
	switch(ParentDockBarType)
	{
		case DOCKBAR_FLOAT:
			return(FormatFloatingBar(MoveWindows, ForceSize));
			break;

		case DOCKBAR_TOP:
		case DOCKBAR_BOTTOM:
			return(FormatHorizontalBar(MoveWindows, ForceSize));
			break;

		case DOCKBAR_LEFT:
		case DOCKBAR_RIGHT:
			return(FormatVerticalBar(MoveWindows, ForceSize));
			break;

		default:
			break;
	}
	return(FALSE);
}	

	
/********************************************************************************************

>	BaseBar::FormatFloatingBar(BOOL MoveWindows,ForceControlSize Force)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/4/94
	Inputs:		MoveWindows - flag to force a set window pos on the controls
				Force normally we use the current dock size to format the bar - sometimes
				however we need to know how big the bar would be large or small.
	Outputs:	-
	Returns:	TRUE
	Purpose: 	Format a floating toolbar with separators and newlines
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL BaseBar::FormatFloatingBar(BOOL MoveWindows,ForceControlSize ForceSize)
{
//	TRACEUSER( "Gerry", _T("FormatFloatingBar\n"));

#pragma message( __LOCMSG__ "BaseBar::FormatFloatingBar - do nothing" )
	TRACE( _T("Warning - BaseBar::FormatFloatingBar called") );
/*	HWND hcontrol;
 	wxSize BarSize(0,0);
 	INT32 xpos = BAR_BORDERSIZE;
	INT32 ypos = BAR_BORDERSIZE;
	INT32 yOffset	= 0;
	INT32 MaxWidth = 0;
	INT32 DDeckerStart = 0;
	INT32 DDeckerEnd = 0;

	BarItem * pBarItem; 
	BarItem * LastItem = NULL;

	// Check Current Control Sizes in this Docking Bar
	INT32 ControlWidth = 0;
	INT32 ControlHeight = 0;
	ControlSizes ControlSize;

	DDeckerReadState DDeckerState = DDECKER_NONE;
	DDeckerControlHeights DDeckerHeight = SMALL_DDECKERS;

	//if we are dragging a bar check the size of the dock we are over
	// otherwise use control size of parent dock 
	OILDockingBar * ThisDock ;
	DockBarType CurrentType;

	CurrentType = (DragType==MOVE) ? DragDockBarType : ParentDockBarType; 
	ThisDock = GetMainFrame()->GetDockBar(CurrentType);
	
	BarItemType ThisControl;
	BarItemType LastControl	= OTHERCONTROL;

	INT32 LargeWidth  = LARGEBUTTONWIDTH;
	INT32 LargeHeight = LARGEBUTTONHEIGHT;
	INT32 SmallWidth  = SMALLBUTTONWIDTH;
	INT32 SmallHeight = SMALLBUTTONHEIGHT;
	INT32 Control =0;

	// Get the current control size for this dock
	if (ForceSize == USE_BAR)
	{
		if (ThisDock->IsBigControls())												
		{
			ControlWidth = LargeWidth;
			ControlHeight = LargeHeight;		
        }
		else
		{
			ControlWidth = SmallWidth;
			ControlHeight = SmallHeight;		
        }
	}
	else if (ForceSize == FORCE_LARGE)
	{
		ControlWidth = LargeWidth;
		ControlHeight = LargeHeight;		

	}
	else if (ForceSize == FORCE_SMALL)
	{
		ControlWidth = SmallWidth;
		ControlHeight = SmallHeight;		
	}
	
	
	if (ControlHeight == LargeHeight )
	{
		ControlSize = LARGE_CONTROLS;
		DDeckerHeight = LARGE_DDECKERS ;
	}
	else
	{
		ControlSize =  SMALL_CONTROLS;
		DDeckerHeight = SMALL_DDECKERS ;
	}			
	// if list is empty	just set a minimum size 
	if (! pOp->IsListEmpty())
	{
//		TRACEUSER( "chris", _T("\n --- FORMAT ---\n"));
		pBarItem = pOp->GetPtrBarHead(); 

		while (pBarItem != NULL)
		{
			ThisControl = GetEnumItemType(pBarItem);
			// Double Decker stuff...
			switch(ThisControl)
			{
				case DECKER_TOP:
				{
					DDeckerState = DDECKER_TOP;
					// xpos to come back to
					DDeckerStart = xpos;
				}
				break;
				case DECKER_BOTTOM:
				{
					DDeckerState = DDECKER_BOTTOM;
					// extent of double decker
					DDeckerEnd = xpos;
					// go back to start
					xpos = DDeckerStart;
				
				}
				break;
				case DECKER_END:
				{
					DDeckerState = DDECKER_NONE;
					// check for longest line top or bottom ?
					if(xpos < DDeckerEnd)
						xpos = DDeckerEnd;
				}
				break;
				case SEPARATOR:
				// insert a separator normally this is horizontal but if the last control was
				// a LineFeed we do a vertical separator
				{
					if(LastItem)
					{
						if (LastItem->IsKindOf(CC_RUNTIME_CLASS(BarLineFeed)))
							ypos += SeparatorWidth;
						else
							xpos += SeparatorWidth;
					}
//					TRACEUSER( "chris", _T("Separator\n"));
				}
				break;
				case LINEFEED:
				// move down to next line and reset xpos
				{
					ypos += ControlHeight+VERTICAL_SPACE;				// Move down to the next line
					if(IS_CHICAGO && pOp->IsKindOf(CC_RUNTIME_CLASS(SuperGallery)))
						ypos +=4;
					xpos = BAR_BORDERSIZE;
					DDeckerStart = xpos;
				}
				break;
			}
			if (pBarItem->IsKindOf(CC_RUNTIME_CLASS(BarControl)))
			{
//				TRACEUSER( "chris", _T("Control\n"));
				hcontrol=(HWND)((BarControl*)pBarItem)->GetWinID();	// Get the Window Handle
		 		wxRect WindSize;

				::GetClientRect(hcontrol,&WindSize);		   		// Get the window size
				switch (DDeckerState)
				{
				case DDECKER_TOP:
					if(ThisControl == SMALLBITMAPBUTTON)
					{
						if (ControlSize == LARGE_CONTROLS)
							yOffset = (ControlHeight / 2) - sLARGEBUTTONHEIGHT +1; //get offset to center control
        				else
							yOffset = (ControlHeight / 2) - sSMALLBUTTONHEIGHT +1;
						xpos++;
        			}
					else
					{
						yOffset = (ControlHeight / 2) - // WindSize.GetHeight()+1;
									DDeckerHeight + 1 ;
        			}

        			// bodge to center static text
        			if(ThisControl == STATIC)
        				yOffset +=3;

        			break;
				case DDECKER_BOTTOM :
					yOffset = (ControlHeight / 2) ; //get offset to center control
					// bodge to center static text
					if(ThisControl == STATIC)
						yOffset +=3;
					if ( ThisControl == SMALLBITMAPBUTTON)
					{
						yOffset ++ ;
						xpos ++;
					}
					if(IS_CHICAGO)
					{
					 	yOffset ++;
			   		}
					break;
    			case DDECKER_NONE :
					yOffset = (ControlHeight - WindSize.GetHeight()) / 2; //get offset to center control
        			break;
				}
				
       			if (MoveWindows)
		            ::SetWindowPos(hcontrol,NULL,xpos,ypos + yOffset,0,0,	// Move the control 
								   SWP_SHOWWINDOW|SWP_NOSIZE|SWP_NOZORDER|SWP_NOREDRAW);
				
				xpos += WindSize.GetWidth()-1;
				if(ThisControl == STATICBITMAP)
					xpos ++;
				else if(ThisControl == SLIDER)
					xpos +=4;
				LastControl = ThisControl ;

				if(IS_CHICAGO)
				{
					if(ThisControl == EDIT)
						xpos += 2;
				}
		  		
	       	}				

			// keep MaxWidth up to date	
			if (xpos > MaxWidth)
					MaxWidth = xpos;

			// remember last control
			LastItem = pBarItem;

			// move to next item
			pBarItem = pOp->GetPtrBarNext(pBarItem); 
		}

	
 		// 
		BarSize.width = MaxWidth + BAR_BORDERSIZE + (BorderWidth * 2) + 1 ;
		BarSize.cx = ypos + ControlHeight + BAR_BORDERSIZE + (BorderWidth * 2)  ;

	}		
	// Set a Minimum Bar Height
	if (BarSize.cx < ControlHeight + (BAR_BORDERSIZE)+ (BorderWidth * 2))
		BarSize.cx = ControlHeight + (BAR_BORDERSIZE)+ (BorderWidth * 2);	

	// ----------------------------
	// Set a Minimum Bar Width

	String_32 Name = pOp->GetName();

	INT32 SmallestWidth = ControlWidth + (BAR_BORDERSIZE * 2)+ (BorderWidth * 2);

	// If this an empty bar make sure we can see the title
	if (pOp->IsListEmpty())
	{ 
	 	// Select the title font into a screen dc and get the text extent of the title
	 	wxDC *dc = GetWindowDC();

		wxFont * OldFont=dc->SelectObject(TitleFont);

		wxSize TextSize=dc->GetTextExtent(Name,camStrlen(Name));

		dc->SelectObject(OldFont);

		ReleaseDC(dc);

		// Check that the title string will be visible UIC
		if ((TextSize.width + TitleBarHeight + (BorderWidth * 4) ) > SmallestWidth)
			SmallestWidth = TextSize.width + TitleBarHeight + (BorderWidth * 4);
	}

	
	if (BarSize.width < SmallestWidth)
		BarSize.width = SmallestWidth ;	

	AdjustFormattedBarSize(&BarSize);
	if (MoveWindows) 
		PositionExtraControls(BarSize,xpos,ypos+ControlHeight);
	// Cache the rectangle
	if (ForceSize == USE_BAR)
	{
		if (ThisDock->IsBigControls())												
		{
			FloatLargeRect.GetRight() = BarSize.width;
			FloatLargeRect.GetBottom() = BarSize.cx + TitleBarHeight;	// UIC
		}
		else
		{
			FloatSmallRect.GetRight() = BarSize.width;
			FloatSmallRect.GetBottom() = BarSize.cx + TitleBarHeight;	//	UIC
		}
	}
	else if (ForceSize == FORCE_LARGE)
	{
		FloatLargeRect.GetRight() = BarSize.width;
		FloatLargeRect.GetBottom() = BarSize.cx + TitleBarHeight;		// UIC
	}
	else if (ForceSize == FORCE_SMALL)
	{
		FloatSmallRect.GetRight() = BarSize.width;
		FloatSmallRect.GetBottom() = BarSize.cx + TitleBarHeight;		// UIC
	} */
	return TRUE; 
};

/********************************************************************************************

>	BaseBar::FormatHorizontalBar(BOOL MoveWindows,ForceControlSize Force)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/4/94
	Inputs:			
				MoveWindows - flag to force a set window pos on the controls
				Force normally we use the current dock size to format the bar - sometimes
				however we need to know how big the bar would be large or small.
	Outputs:	-
	Returns:	-
	Purpose: 	Format a Horizontal toolbar with separators 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
BOOL BaseBar::FormatHorizontalBar(BOOL MoveControls,ForceControlSize ForceSize)
{
//	TRACEUSER( "Gerry", _T("FormatHorizontalBar\n"));

#pragma message( __LOCMSG__ "BaseBar::FormatHorizontalBar - do nothing" )
	TRACE( _T("Warning - BaseBar::FormatHorizontalBar called") );
/*	BOOL AtFront = TRUE; 	
	HWND hcontrol;
	// Find out how many bar items
	UINT32 NumBarItems = pOp->GetNumBarItems(); 
	INT32 ypos = 0 ;
	INT32 xpos = BAR_BORDERSIZE;
	INT32 yOffset = 0;
	INT32 DDeckerStart = 0;
	INT32 DDeckerEnd = 0;
	wxSize BarSize(0,0);
	BarItem* pBarItem;
	ControlSizes ControlSize;
	DDeckerReadState DDeckerState = DDECKER_NONE;
	DDeckerControlHeights DDeckerHeight = SMALL_DDECKERS;
	INT32 ButtonSize;
	BarItemType	ThisControl ;
	BarItemType LastControl	= OTHERCONTROL;
	// if we are dragging a bar check the size of the dock we are over
	// otherwise use control size of parent dock 

	OILDockingBar * ThisDock ;
	DockBarType CurrentType;

	CurrentType = (DragType==MOVE) ? DragDockBarType : ParentDockBarType; 
	ThisDock = GetMainFrame()->GetDockBar(CurrentType);

	// Get the current control size for this dock
	if (ForceSize == USE_BAR)
	{
		if (ThisDock->IsBigControls())												
			ControlSize = LARGE_CONTROLS;
		else
			ControlSize = SMALL_CONTROLS;				 
	}
	else if (ForceSize == FORCE_LARGE)
	{
		ControlSize = LARGE_CONTROLS;
	}
	else if (ForceSize == FORCE_SMALL)
	{
		ControlSize = SMALL_CONTROLS;	
	}
	
	
	if (ControlSize == LARGE_CONTROLS)
	{
		ButtonSize = LARGEBUTTONWIDTH-1;
		DDeckerHeight = LARGE_DDECKERS ;
	}
	else
	{
		ButtonSize = SMALLBUTTONWIDTH-1;
		DDeckerHeight = SMALL_DDECKERS ;
	}			
	// if list is empty	just set a minimum size 
	if (! pOp->IsListEmpty()||ThisIsAnInfoBar)
	{

		pBarItem = pOp->GetPtrBarHead();
		
		while(pBarItem!=NULL)
		{
			// Get the next BarItem
			ThisControl = GetEnumItemType(pBarItem);
			switch ( ThisControl )
			{
				case DECKER_TOP:
				{
					DDeckerState = DDECKER_TOP;
					// xpos to come back to
					DDeckerStart = xpos;
				}
				break;
				case DECKER_BOTTOM:
				{
					DDeckerState = DDECKER_BOTTOM;
					// extent of double decker
					DDeckerEnd = xpos;
					// go back to start
					xpos = DDeckerStart;
				
				}
				break;
				case DECKER_END:
				{
					DDeckerState = DDECKER_NONE;
					// check for longest line top or bottom ?
					if ( xpos < DDeckerEnd)
						xpos = DDeckerEnd;
				}
				break;
				case SEPARATOR:
				{
					if( ! AtFront ) // don't separate at the front of a bar
					{	
						xpos += SeparatorWidth;
					}
				}
				break;
			}
			
			if (pBarItem->IsKindOf(CC_RUNTIME_CLASS(BarControl)))
			{
				hcontrol=(HWND)((BarControl*)pBarItem)->GetWinID();  		// Get the Window Handle
	
	    		wxRect WindSize;
				::GetClientRect(hcontrol,&WindSize);
				switch (DDeckerState)
				{
				case DDECKER_TOP:
					
					// this is all rather messy because SMALLBITMAPBUTTONS and Chicago
					// buttons don't overlap where normal pre chicago buttons do ...

					if ( ThisControl == SMALLBITMAPBUTTON)
					{
						if (ControlSize == LARGE_CONTROLS)
							yOffset = (ControlSize / 2) - sLARGEBUTTONHEIGHT +1 ; //get offset to center control
        				else
							yOffset = (ControlSize / 2) - sSMALLBUTTONHEIGHT +1 ;	
        				
        				xpos++;
        			}
        			else
					{
						yOffset = (ControlSize / 2) - DDeckerHeight +1;
						
					}
					// bodge to center static text
					if(ThisControl == STATIC)
        				yOffset +=3;

					
        			break;
				case DDECKER_BOTTOM :
	
					yOffset = (ControlSize / 2) ; //get offset to center control
	
					if ( ThisControl == SMALLBITMAPBUTTON)
					{
						yOffset ++ ;
						xpos ++;
					}
					if ( ThisControl == STATIC)
        			{
        			 	yOffset +=3;
					}
					break;
				case DDECKER_NONE :
					yOffset = (ControlSize - WindSize.GetHeight()) / 2; //get offset to center control
        			break;
				}
				if ( MoveControls )
					::SetWindowPos(hcontrol,NULL,xpos,ypos + yOffset,0,0,
								   SWP_SHOWWINDOW|SWP_NOSIZE|SWP_NOZORDER|SWP_NOREDRAW);
       	
       			xpos += WindSize.GetWidth()-1;
				
				if(ThisControl == STATICBITMAP)
					xpos++;

				LastControl = ThisControl ;
				if(IS_CHICAGO)
				{
					if(ThisControl == EDIT)
						xpos += 2;
				}
			}
			AtFront = FALSE;
			pBarItem = pOp->GetPtrBarNext(pBarItem);
		}// end while 

		BarSize.width = xpos + (BAR_BORDERSIZE*2)-1;
		
	}
		// Set a Minimum Bar Size
	if (BarSize.width < ControlSize + (BAR_BORDERSIZE *2))
	{
		BarSize.width = ControlSize + (BAR_BORDERSIZE *2);	
	}
  
	// This test forces horizontal infobars to be a set width 
	if (ThisIsAnInfoBar && ! pOp->IsKindOf(CC_RUNTIME_CLASS(SuperGallery)))
	{
		if(ControlSize == LARGE_CONTROLS)
			BarSize.width = LARGE_INFO_BAR;
		else
			BarSize.width = SMALL_INFO_BAR;
	}

	BarSize.cx = ControlSize +1;
	
	if(IS_CHICAGO)
		BarSize.height--;

	// Cache this bar size
	if (ForceSize == USE_BAR)
	{
		if (ThisDock->IsBigControls())												
		{
			HorizontalLargeRect.GetRight() = BarSize.width;
			HorizontalLargeRect.GetBottom() = BarSize.height;
		}
		else
		{
			HorizontalSmallRect.GetRight() = BarSize.width;
			HorizontalSmallRect.GetBottom() = BarSize.height;
		}
	}
	else if (ForceSize == FORCE_LARGE)
	{
		HorizontalLargeRect.GetRight() = BarSize.width;
		HorizontalLargeRect.GetBottom() = BarSize.height;
	}
	else if (ForceSize == FORCE_SMALL)
	{
		HorizontalSmallRect.GetRight() = BarSize.width;
		HorizontalSmallRect.GetBottom() = BarSize.height;
	} */
	return TRUE; 
}; 
					  



/********************************************************************************************

>	BaseBar::FormatVerticalBar(BOOL MoveWindows,ForceControlSize Force)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/4/94
	Inputs:		
				MoveWindows - flag to force a set window pos on the controls
				Force normally we use the current dock size to format the bar - sometimes
				however we need to know how big the bar would be large or small.
	Outputs:	-
	Returns:	-
	Purpose: 	Format a Vertical toolbar with separators 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/


BOOL BaseBar::FormatVerticalBar(BOOL MoveControls,ForceControlSize ForceSize)
{
//	TRACEUSER( "Gerry", _T("FormatVerticalBar\n"));
#pragma message( __LOCMSG__ "BaseBar::FormatVerticalBar - do nothing" )
	TRACE( _T("Warning - BaseBar::FormatVerticalBar called") );
/*	BOOL AtFront = TRUE; 	
	HWND hcontrol;

	// Find out how many bar items
	UINT32 NumBarItems = pOp->GetNumBarItems(); 
	INT32 xpos = BAR_VERTICAL_CENTEROFFSET;
	INT32 ypos = BAR_BORDERSIZE;
	BarItem* pBarItem;
	BarItemType	ThisControl; 
	ControlSizes ControlSize;
	INT32 ButtonSize;

	wxSize BarSize(0,0) ;
	
	//if we are dragging a bar check the size of the dock we are over
	// otherwise use control size of parent dock 
	OILDockingBar * ThisDock ;
	DockBarType CurrentType;

	CurrentType = (DragType==MOVE) ? DragDockBarType : ParentDockBarType; 
	ThisDock = GetMainFrame()->GetDockBar(CurrentType);

	// Get the current control size for this dock
	if (ForceSize == USE_BAR)
	{
		if (ThisDock->IsBigControls())												
			ControlSize = LARGE_CONTROLS;
		else
			ControlSize = SMALL_CONTROLS;
	}
	else if (ForceSize == FORCE_LARGE)
	{
		ControlSize = LARGE_CONTROLS;
	}
	else if (ForceSize == FORCE_SMALL)
	{
		ControlSize = SMALL_CONTROLS;	
	}

	if (ControlSize == LARGE_CONTROLS)
		ButtonSize = LARGEBUTTONHEIGHT-1;
	else
		ButtonSize = SMALLBUTTONHEIGHT-1;

	// if list is empty	just set a minimum size 
	if (! pOp->IsListEmpty())
	{

		pBarItem = pOp->GetPtrBarHead(); 
		while (pBarItem != NULL)
		{
			ThisControl = GetEnumItemType(pBarItem);
			if (pBarItem->IsKindOf(CC_RUNTIME_CLASS(BarSeparator)))
			{
				if(!AtFront ) // don't separate at the front of a bar
				{
					ypos += SeparatorWidth;
				}
			}
			else if (pBarItem->IsKindOf(CC_RUNTIME_CLASS(BarControl)))
			{
				hcontrol=(HWND)((BarControl*)pBarItem)->GetWinID();		// Get the Window Handle
				wxRect WindSize;
					::GetClientRect(hcontrol,&WindSize);
				INT32 Offset = (ControlSize - WindSize.GetWidth())/2;
				if (MoveControls)
					::SetWindowPos(hcontrol,NULL,Offset,ypos,0,0,
								   SWP_SHOWWINDOW|SWP_NOSIZE|SWP_NOZORDER|SWP_NOREDRAW);

	    	    if(ThisControl == SLIDER)
				{
					ypos += WindSize.GetHeight();
				}
				else
				{
	    	    	ypos += ButtonSize;
				}
            }
			AtFront = FALSE;
			pBarItem = pOp->GetPtrBarNext(pBarItem); 
		}

		BarSize.height = ypos + (BAR_BORDERSIZE*2)-1;
	}
	// Set a Minimum Bar Size
	if (BarSize.height < ControlSize + (BAR_BORDERSIZE *2))
		BarSize.height = ControlSize + (BAR_BORDERSIZE *2);	

	BarSize.width = ControlSize +1;
   
	if(IS_CHICAGO)
		BarSize.width--;

   	if (ForceSize == USE_BAR)
	{
		if (ThisDock->IsBigControls())												
		{
			VerticalLargeRect.GetRight() = BarSize.width;
			VerticalLargeRect.GetBottom() = BarSize.height;
		}
		else
		{
			VerticalSmallRect.GetRight() = BarSize.width;
			VerticalSmallRect.GetBottom() = BarSize.height;
		}
	}
	else if (ForceSize == FORCE_LARGE)
	{
		VerticalLargeRect.GetRight() = BarSize.width;
		VerticalLargeRect.GetBottom() = BarSize.height;
	}
	else if (ForceSize == FORCE_SMALL)
	{
		VerticalSmallRect.GetRight() = BarSize.width;
		VerticalSmallRect.GetBottom() = BarSize.height;
	} */
 
   	return TRUE; 
};

/********************************************************************************************

>	void BaseBar::AdjustFormattedBarSize(wxSize* BarSize)

	Author:		Alex
	Created:	1/7/94
	Inputs:		Pointer to the bar size record to adjust 
	Outputs:	-
	Returns:	None
	Purpose:	Adjusts the proposed new size of a bar once formatting has taken place   
	Errors:		-
	SeeAlso:	GalleryBar::AdjustFormattedBarSize

This function does nothing in the base class, but may be overridden, for instance by Gallery
Bars.

********************************************************************************************/

void BaseBar::AdjustFormattedBarSize(wxSize* BarSize)
{
}

/********************************************************************************************

>	void BaseBar::PositionExtraControls(wxSize BarSize, INT32 xpos, INT32 ypos)

	Author:		Alex
	Created:	1/7/94
	Inputs:		Bar size record, x & y coord of current format position. 
	Outputs:	-
	Returns:	None
	Purpose:	Moves extra controls about once they have been created   
	Errors:		-
	SeeAlso:	GalleryBar::PositionExtraControls, BaseBar::CreateExtraControls

This function does nothing in the base class, but may be overridden, for instance by Gallery
Bars.

********************************************************************************************/

void BaseBar::PositionExtraControls(wxSize BarSize, INT32 xpos, INT32 ypos)
{
}

/********************************************************************************************

>	INT32 BaseBar::CreateExtraControls()

	Author:		Alex
	Created:	1/7/94
	Inputs:		None 
	Outputs:	TRUE if successful, else FALSE
	Returns:	None
	Purpose:	Allows subclasses to create additional bar controls   
	Errors:		-
	SeeAlso:	GalleryBar::CreateExtraControls, BaseBar::PositionExtraControls

This function does nothing in the base class, but may be overridden, for instance by Gallery
Bars.

********************************************************************************************/

INT32 BaseBar::CreateExtraControls()
{
	return TRUE;
}

  


/********************************************************************************************

>	BOOL BaseBar::CreateToolBtn(BarToolButton* pToolBtn)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/4/94
	Inputs:		pToolBtn: The 
	Outputs:	-
	Returns:	TRUE if succesful, FALSE otherwise
	Purpose:	Creates tool bar button control   
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
// This function is probably dead wood
BOOL BaseBar::CreateToolBtn(BarToolButton* pToolBtn)
{
return TRUE ;
} 


DLGITEMTEMPLATE* BaseBar::GetPointerToControl(DLGTEMPLATE* pDlg ,UINT32 CtlId)
{
	PORTNOTETRACE( "dialog", "BaseBar::GetPointerToControl - do nothing - DLGITEMTEMPLATE usage" );
#ifndef EXCLUDE_FROM_XARALX
 	DLGITEMTEMPLATE* pCurrentControl = (DLGITEMTEMPLATE *) (((ADDR)pDlg) + SizeDlgHeader(pDlg));
	WORD NumControls = pDlg->cdit;
	ENSURE(pCurrentControl != NULL, "Hideous Error in Load Controls"); 

	for (UINT32 i = 0; i< NumControls; i++)
	{
	 	if (pCurrentControl->id == CtlId)
	 	{
			return pCurrentControl;
	 	}
		pCurrentControl = (DLGITEMTEMPLATE *)
			(((ADDR)pCurrentControl) + SizeCtrlData(pCurrentControl)); 	
	}
#endif
	return NULL;
}

HINSTANCE BaseBar::GetModuleInst(const BarControlInfo*pBarCtlInfo,UINT32 * ToolModuleID )
{
#pragma message( __LOCMSG__ "BaseBar::GetModuleInst - do nothing" )
	TRACE( _T("Warning - BaseBar::GetModuleInst called") );
/*	*ToolModuleID = Tool::GetModuleID(pBarCtlInfo->ToolID); 

	// tools.dll not loaded, chances are the resources are in with the rest
	if(DLLs[0] == (HINSTANCE)HINSTANCE_ERROR || DLLs[0] == 0)
		return AfxGetResourceHandle();
	
	return ResHelp::GetInstanceFromID( pBarCtlInfo->ResourceID );

#if 0
// old code before Andy got to it
	HINSTANCE ModuleInst;
 	if (pBarCtlInfo->ToolID != 0)  // We want to get control from tool DLL
	{
		// Try to get the tools module ID
		*ToolModuleID = Tool::GetModuleID(pBarCtlInfo->ToolID); 

		ENSURE(*ToolModuleID != 0, "Could not find module ID"); 
		if (*ToolModuleID == 0)
			return NULL; 
		// Now try to get the module's instance
		ModuleInst = OILModule::GetInstance(*ToolModuleID); 
		ENSURE (ModuleInst != (HINSTANCE)HINSTANCE_ERROR, "Could not obtain module instance"); 
		if (ModuleInst == (HINSTANCE)HINSTANCE_ERROR)
			return NULL;
		else
			return ModuleInst; 
	}
	else 
	{
		// get controls from exe
		return  AfxGetApp()->m_hInstance;
	}											   
#endif */

	return HINSTANCE(-1);
}
/********************************************************************************************

>	BOOL BaseBar::CreateInfoControlFromResource(BarControl* pBarControl)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/4/94
	Inputs:		pBarControl:	The BarControl to create
	Outputs:	-
	Returns:	-
	Purpose:	Special case for info bars 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/


BOOL BaseBar::CreateInfoControlFromResource(BarControl* pBarControl,BOOL Init)
{
	PORTNOTETRACE( "dialog", "BaseBar::CreateInfoControlFromResource - do nothing" );
#ifndef EXCLUDE_FROM_XARALX
	const BarControlInfo * pBarCtlInfo;
	static DLGITEMTEMPLATE * pCurrentControl;
	static HINSTANCE 	ModuleInst 	 = NULL;
	static UINT32 ResID =0;
	static UINT32 ControlID; 
	static HRSRC hRes = NULL;
	static DLGTEMPLATE* pDlg;
	static HGLOBAL hGlobalRes = NULL;

	UINT32 		ToolModuleID = 0;
	HINSTANCE ThisModuleInst = NULL;
	
	// DISABLE CACHING FOR NOW !
	Init = TRUE;

	if(pBarControl==NULL)
	{	if(hGlobalRes != NULL)
		{
			// Lets do the nice thing  
			UnlockResource(hGlobalRes);
			FreeResource(hGlobalRes);	
		}
		return FALSE;
	}
	
	// Get control info
	if (pOp->IsKindOf(CC_RUNTIME_CLASS(SuperGallery)))
	{
		pBarCtlInfo = &(pBarControl->GetHorzControl()->BarCtrlInfo);
	}
	else
	{
		switch(ParentDockBarType)
		{
			case DOCKBAR_TOP:
			case DOCKBAR_BOTTOM:
			case DOCKBAR_FLOAT:
				pBarCtlInfo =&(pBarControl->GetHorzControl()->BarCtrlInfo);
				break;
			case DOCKBAR_LEFT:
			case DOCKBAR_RIGHT:
				pBarCtlInfo =&(pBarControl->GetVertControl()->BarCtrlInfo);
				break;
		}
	}
	
	// initialise ModuleInst 
	if(Init)
	{
		ModuleInst = GetModuleInst(pBarCtlInfo,&ToolModuleID);
	}
	// normally just check if we need the same module
	else
	{
		ThisModuleInst = GetModuleInst(pBarCtlInfo,&ToolModuleID);
		if(ThisModuleInst != ModuleInst)
		{
			Init = TRUE;
			ModuleInst = ThisModuleInst;
		}
		if(ModuleInst == NULL)
			ENSURE(FALSE,"Can't find Module Instance");
		if(pBarCtlInfo->ResourceID != ResID)
		{
		 	Init = TRUE;
			ResID = pBarCtlInfo->ResourceID;
		}
	}

	BOOL CreatedControl = FALSE; 

	// Try to find, load and lock the dialog resource
	if(Init)
	{
		if(hGlobalRes!=NULL)
		{
			// Lets do the nice thing  
			UnlockResource(hGlobalRes);
			FreeResource(hGlobalRes);	
		}

		hRes = FindResource(ModuleInst, MAKEINTRESOURCE(pBarCtlInfo->ResourceID), RT_DIALOG);
		if (hRes == NULL)
		{
			ERROR3_PF( ("Could not find RT_DIALOG id %u in %x", pBarCtlInfo->ResourceID, 
																			ModuleInst ) );
			return FALSE;
		}
		// Try to get handle to global block containing resource data 
		hGlobalRes = LoadResource(ModuleInst, hRes);
		ENSURE(hGlobalRes != NULL, "Could not load resource");
		if (hGlobalRes == NULL)
			return FALSE;  

		// Get the dialog resource 
		pDlg = (DLGTEMPLATE *) LockResource(hGlobalRes);
		ENSURE(pDlg != NULL, "Could not lock resource"); 
		if (pDlg == NULL)
			return FALSE; 
		
		pCurrentControl = (DLGITEMTEMPLATE *) (((ADDR)pDlg) + SizeDlgHeader(pDlg));
	}
	
	//TRACE( _T("found %d after %d"),pCurrentControl->id,pBarCtlInfo->ControlID);
    
    if(!Init)
	{
	    if (pCurrentControl->id == pBarCtlInfo->ControlID )
	 	{
			// Yippee ! We have found the control required
			// Lets create it 
			if (CreateControlItem(pCurrentControl,pBarControl,ModuleInst,ToolModuleID))
			{
				return  TRUE; 
			}
			else
			{
				ENSURE(FALSE,"Failed to create control");
				return FALSE;
			}
			pCurrentControl = (DLGITEMTEMPLATE *)
			(((ADDR)pCurrentControl) + SizeCtrlData(pCurrentControl)); 		 
	 	}
	 	else
		{
			Init =TRUE;
			return CreateInfoControlFromResource(pBarControl,TRUE);
		}
	}


	if(Init)
	{
	
		DLGITEMTEMPLATE * ThisControl = GetPointerToControl(pDlg ,pBarCtlInfo->ControlID);
		if(ThisControl==NULL)
		{
			//ENSURE(FALSE,"Can't find Control in Resource");
			ERROR3_PF(("Can't find Control (%d / 0x%x) in Resource (%d / 0x%x)", pBarCtlInfo->ControlID, pBarCtlInfo->ControlID, pBarCtlInfo->ResourceID, pBarCtlInfo->ResourceID));
			return FALSE;
		}
		if (CreateControlItem(ThisControl,pBarControl,ModuleInst,ToolModuleID))
		{
			return TRUE; 
		}
		else
		{
		 	return FALSE;
		}
		pCurrentControl = (DLGITEMTEMPLATE *)
		(((ADDR)ThisControl) + SizeCtrlData(ThisControl)); 		 
	}

	return (CreatedControl);
#else	
	return FALSE;
#endif
}


/********************************************************************************************

>	BOOL BaseBar::CreateControlFromResource(BarControl* pBarControl)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/4/94
	Inputs:		pBarControl:	The BarControl to create
	Outputs:	-
	Returns:	-
	Purpose:	Create 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL BaseBar::CreateControlFromResource(BarControl* pBarControl)
{
	PORTNOTETRACE( "dialog", "BaseBar::CreateControlFromResource - do nothing - Resource usage" );
#ifndef EXCLUDE_FROM_XARALX
	const BarControlInfo*pBarCtlInfo;

	if (pOp->IsKindOf(CC_RUNTIME_CLASS(SuperGallery)))
	{
		pBarCtlInfo = &(pBarControl->GetHorzControl()->BarCtrlInfo);
	}
	else
	{
		switch(ParentDockBarType)
		{
			case DOCKBAR_TOP:
			case DOCKBAR_BOTTOM:
			case DOCKBAR_FLOAT:
				pBarCtlInfo =&(pBarControl->GetHorzControl()->BarCtrlInfo);
				break;
			case DOCKBAR_LEFT:
			case DOCKBAR_RIGHT:
				pBarCtlInfo =&(pBarControl->GetVertControl()->BarCtrlInfo);
				break;
		}
	}

	HINSTANCE 	ModuleInst 	 = NULL; 
	UINT32 		ToolModuleID = 0;
 	
	ModuleInst = GetModuleInst(pBarCtlInfo,&ToolModuleID);

	// -------------------------------------------------------------------------------------
	// Try to find, load and lock the dialog resource

	HRSRC hRes = FindResource(ModuleInst, MAKEINTRESOURCE(pBarCtlInfo->ResourceID), RT_DIALOG);

	ENSURE(hRes != NULL, "Could not find resource"); 
	if (hRes == NULL)
		return FALSE;  

	// Try to get handle to global block containing resource data 
	HGLOBAL hGlobalRes = LoadResource(ModuleInst, hRes);
	ENSURE(hGlobalRes != NULL, "Could not load resource");
	if (hGlobalRes == NULL)
		return FALSE;  

	// Get the dialog resource 
	DLGTEMPLATE* pDlg = (DLGTEMPLATE *) LockResource(hGlobalRes);
	ENSURE(pDlg != NULL, "Could not lock resource"); 
	if (pDlg == NULL)
		return FALSE; 


	// We are totally disinterested in the Dialog itself as it's only a recepticle for storing
	// control resources
	

	BOOL CreatedControl = FALSE; 

	// Loop until we have found the control to create 
	
	WORD NumControls = pDlg->cdit; // The number of items in the dialog

	TRACEUSER( "Simon", _T("Dialog has %d controls"), NumControls); 

	// Get pointer to first control
	DLGITEMTEMPLATE *pCurrentControl = (DLGITEMTEMPLATE *) (((ADDR)pDlg) + SizeDlgHeader(pDlg));

	ENSURE(pCurrentControl != NULL, "Hideous Error in Load Controls"); 

	for (UINT32 i = 0; i< NumControls; i++)
	{
	 	if (pCurrentControl->id == pBarCtlInfo->ControlID)
	 	{
			// Yippee ! We have found the control required
			// Lets create it 
			if (CreateControlItem(pCurrentControl,pBarControl,ModuleInst,ToolModuleID))
			{
				CreatedControl = TRUE; 
			}
			break; 
	 	}
		pCurrentControl = (DLGITEMTEMPLATE *)
			(((ADDR)pCurrentControl) + SizeCtrlData(pCurrentControl)); 	
	}

	// Lets do the nice thing  
	UnlockResource(hGlobalRes);
	FreeResource(hGlobalRes);

	return (CreatedControl);
#else
	return FALSE;
#endif
}


/********************************************************************************************

>	void BaseBar::CreateControlItem(DLGITEMTEMPLATE * pCurrentControl,
									  BarControl * pBarControl, 
									  HINSTANCE ModuleInst,
									  UINT32 ToolModuleID)  

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/4/94
	Inputs:		pCurrentControl: The control to create
				pBarControl	   : Kernel description of control
				ModuleInst	   : Module handle
				ToolModuleID   : The tool's module ID
	Outputs:	-
	Returns:	-
	Purpose:	Creates the control and attaches it as a child of the BaseBars window, 
	Errors:		-
	sCOPE:		private
	SeeAlso:	-

********************************************************************************************/
									 
 BOOL BaseBar::CreateControlItem(DLGITEMTEMPLATE * pCurrentControl,
 									BarControl * pBarControl,
 									HINSTANCE ModuleInst,
 									UINT32 ToolModuleID)
{
#pragma message( __LOCMSG__ "BaseBar::GetModuleInst - do nothing" )
	TRACE( _T("Warning - BaseBar::GetModuleInst called") );
/*	BarItemType	 ThisControlType = OTHERCONTROL;

	BOOL ResizeWithFont = FALSE;

	#if WIN32
	// Get pointer to the data after the DLGITEMTEMPLATE structure.
	WORD *pData = (WORD *) (pCurrentControl + 1);

	// Obtain the class
	LPWSTR pClass;

	if (*pData == 0xFFFF)
	{
		// Convert the atom value into a string

		switch((*(pData+1)))
		{
			case  0x0080:
			{
				pClass = L"Button";
				ThisControlType = BUTTON;
				ResizeWithFont = TRUE;
				break;
			}
			case 0x0081:
			{
				pClass = L"Edit";
				ThisControlType = EDIT;
				ResizeWithFont = TRUE;
				break;
			}
			case 0x0082:
			{
				pClass = L"Static";
				ThisControlType = STATIC;
				ResizeWithFont = TRUE;
				break;
			}
			case 0x0083:
			{
				pClass = L"Listbox";
				ThisControlType = LIST;
				ResizeWithFont = TRUE;
				break;
			}
			case 0x0084:
			{
				pClass = L"Scrollbar";
				ThisControlType = SCROLLBAR;
				break;
			}
			case 0x0085:
			{
				pClass = L"Combobox";
				ThisControlType = COMBO;
				ResizeWithFont = TRUE;
				break;
			}
		}

		pData += 2;
	}											   
	else
	{
		// The class is specified as a string
		pClass = (LPWSTR)pData; 
		pData = (WORD *) MovePastWideStr((LPWSTR) pData);
	}
																   
	// Obtain the windows title 
	LPWSTR pTitle; 
									 
	if (*pData == 0xFFFF)
	{
		// It's a resource
		pTitle = (LPWSTR) (pData+1); 
		
		pData += 2;									   
	}
	else
	{
		pTitle = (LPWSTR) pData; 
		// The title array is a string - skip it
		pData = (WORD *) MovePastWideStr((LPWSTR) pData);
	}

	// Obtain the creation data
	LPVOID pCreationData = (LPVOID*) (pData+2); 


	// ------------------------------------------------------------------------------------

	#else
	// Get pointer to the data after the DLGITEMTEMPLATE structure.
	ADDR pData = (ADDR) (pCurrentControl + 1);
	// Obtain the class

	LPCTSTR pClass; 
	LPCTSTR pTitle;

	if ((*pData) & 0x80)
	{	
		// An atom is specified 
		
		// Convert the atom 
		switch((*(pData+1)))
		{
			case  0x0080:
			{
				pClass = "Button";
				ThisControlType = BUTTON;
				ResizeWithFont = TRUE;
				break;
			}
			case 0x0081:
			{
				pClass = "Edit";
				ThisControlType = EDIT;
				ResizeWithFont = TRUE;
				break;
			}
			case 0x0082:
			{
				pClass = "Static";
				ThisControlType = STATIC;
				ResizeWithFont = TRUE;
				break;
			}
			case 0x0083:
			{
				pClass = "Listbox";
				ThisControlType = LIST;
				ResizeWithFont = TRUE;
				break;
			}
			case 0x0084:
			{
				pClass = "Scrollbar";
				ThisControlType = SCROLLBAR;
				break; 
			}
			case 0x0085:
			{
				pClass = "Combobox";
				ThisControlType = COMBO;
				ResizeWithFont = TRUE;
				break;
			}
		} 
		pData++;
	
	}
	else
	{
		pClass = (LPCTSTR)pData;  // Get the class string  

		pData = (ADDR) MovePastWideStr((LPWSTR) pData);
	}
	
	// Obtain the windows title
	pTitle = (LPCTSTR) pData;
	pData = (ADDR) MovePastWideStr((LPWSTR) pData);

	// Obtain the creation data
	LPVOID pCreationData = (LPVOID*) pData+1; 	// Skip the length

	#endif
	
	// get the dialog base units so that we can convert to pixels 
	INT32 BaseUnits = GetDialogBaseUnits(); 
	
	// A bit of nasty this.. hard code base units in so controls don't change size
	// if the user switches to LARGE FONTS
	
	WORD DialogUnitX = X_BASEUNITS ;//LOWORD((UINT32)BaseUnits); 
    WORD DialogUnitY = Y_BASEUNITS ;//HIWORD((UINT32)BaseUnits);
  
	char AnsiClass[256];
	char AnsiTitle[256];
  
#if WIN32
	// we cannot pass Unicode strings under Win32s, so we convert to multibyte strings.
	// If we ever need a Chinese NT version then we could pass these directly onto
	// ::CreateWindowExW, but currently we always convert down to Ansi
	// INT32 result;
												  
	// really we should use WideCharToMultiByte, but it refuses to work under Win32s 1.1
	// returning zero with a GetLastError of 2, so we use camWcstombs instead.
	camWcstombs( AnsiClass, pClass, sizeof(AnsiClass) );
	camWcstombs( AnsiTitle, pTitle, sizeof(AnsiTitle) );
#else
	// 16-bit version needs no conversion, but we have to copy them because we
	// crap upon them
	camStrcpy( AnsiClass, pClass );
	camStrcpy( AnsiTitle, pTitle );
#endif

	// Check for type here if not already set
	if(ThisControlType == OTHERCONTROL)
	{
		if (camStricmp(AnsiClass, "cc_BitmapButton") == 0)		// Internal
			ThisControlType = BITMAPBUTTON;
		else if (camStricmp(AnsiClass, "cc_SmallButton") == 0)	// Internal
			ThisControlType = SMALLBITMAPBUTTON;
		else if (camStricmp(AnsiClass, "cc_StaticBitmap") == 0)	// Internal
			ThisControlType = STATICBITMAP;
		else if (camStricmp(AnsiClass, "cc_Text3D") == 0)			// Internal
			ThisControlType = TEXT3D;
		else if (camStricmp(AnsiClass, "cc_Grid")==0)				// Internal
			ThisControlType = ROTATEGRID;
		else if (camStricmp(AnsiClass,"cc_Slider")==0)			// Internal
			ThisControlType = SLIDER;
		else if (camStricmp(AnsiClass, "cc_1dBitmapComboBoxEdit") == 0)	// internal
			ThisControlType = BITMAPCOMBO1D;
		else if (camStricmp(AnsiClass, "cc_2dBitmapComboBoxEdit") == 0)	// internal
			ThisControlType = BITMAPCOMBO2D;
	}
	

	DWORD Style = pCurrentControl->style;

	OpDescriptor* OpDesc ;
	if (pOp->IsKindOf(CC_RUNTIME_CLASS(SuperGallery)))
	{
		OpDesc = (pBarControl)->GetOpDescriptor(Horizontal);
	}
	else
	{
		switch(ParentDockBarType)
		{
			case DOCKBAR_TOP:
			case DOCKBAR_BOTTOM:
			case DOCKBAR_FLOAT:
				OpDesc = (pBarControl)-> GetOpDescriptor(Horizontal);
				break;
			case DOCKBAR_LEFT:
			case DOCKBAR_RIGHT:
				OpDesc = (pBarControl)-> GetOpDescriptor(Vertical);
				break;
		}
	}

	if (OpDesc != NULL)
	{
		String_256 Desc; 
		OpState NewState = OpDesc->GetOpsState(&Desc);
		if(NewState.Greyed)
		{	
			Style |= BS_DISABLED;
		}
		else
		{
			Style &= (~BS_DISABLED);
		}
	}

	OILDockingBar * ThisDock =GetMainFrame()->GetDockBar(ParentDockBarType);

	if(	ThisControlType == ROTATEGRID ||
		ThisControlType == TEXT3D)
		ResizeWithFont = TRUE;
	

	if(	ThisControlType == BITMAPBUTTON ||
	    ThisControlType == STATICBITMAP ||
		ThisControlType == SMALLBITMAPBUTTON)
	{
		if (ThisDock->IsBigControls())
		{
			Style &= (~BS_HALFSIZE);
		}
		else
		{
			Style |= BS_HALFSIZE;
		}
	}
	
	// CHICAGO BODGE - this is something to do with dialog units/system font in Chicago
	// but until finalised this will do..
	
	INT32 XSize = (pCurrentControl->cx * DialogUnitX) / 4;
	INT32 YSize = (pCurrentControl->cy * DialogUnitY) / 8;

	Style &= (~WS_VISIBLE);
	HWND CreatedWindow = ::CreateWindowEx(
						#if WIN32
							pCurrentControl->dwExtendedStyle,
						#else
							0,
						#endif
						   AnsiClass, 
						   AnsiTitle,
						   Style,
						   0,
						   0,
						   XSize,
						   YSize,
						   GetSafeHwnd(),				// this window is its parent 
						   (HMENU)pBarControl->GetUniqueGadgetID(), 
						   AfxGetApp()->m_hInstance,
						   pCreationData 				
						   );

	// if the class is a Jason button bar then we need to set its bitmap
	if (CreatedWindow != NULL)
	{
		// Store Hwnd in BarControl list
		pBarControl->SetWinID((CWindowID)CreatedWindow);
		ThisControlType = GetEnumItemType((BarItem *)pBarControl);
		
		// Send old style messages to cc_StaticBitmap because that class doesn't yet have
		// PNG rendering ability
		if (ThisControlType == STATICBITMAP)
		{
			BitmapButtonInfo BtnInfo;

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
			
			// Fill in the BtnInfo structure
			BtnInfo.hAppInstance = ModuleInst;  // We assume that the bitmap resouce can be
											    // found in the same module as this control.
			String_256 Title =("L");
			Title += TEXT(AnsiTitle);
			
			BtnInfo.Bitmap[BMP_L_UNSELECTED] = (LPTSTR)Title;
			BtnInfo.Bitmap[BMP_S_UNSELECTED] = (LPTSTR)AnsiTitle;

  			BtnInfo.Bitmap[BMP_S_SELECTED] = NULL;	// NULL, or name of selected bitmap
			BtnInfo.Bitmap[BMP_L_SELECTED] = NULL;	// NULL, or name of selected bitmap

			BtnInfo.UnselectedIndex=Index;			//  Indices of glyph bitmaps within the
			BtnInfo.SelectedIndex=Index;			//  Unselected/Selected bitmaps
	 
			::SendMessage( CreatedWindow, WM_SETBITMAPS, 0, ((LPARAM) &BtnInfo));
		}

		// Send new style messages to the button classes because they can use PNG
		// resources
		if(	ThisControlType == BITMAPBUTTON ||
			ThisControlType == SMALLBITMAPBUTTON)
		{
			DialogManager::SetGadgetBitmaps(CreatedWindow, ModuleInst);
		}


		BOOL Horz=TRUE;
		if(ParentDockBarType == DOCKBAR_LEFT || ParentDockBarType == DOCKBAR_RIGHT)
		     Horz = FALSE;
	
		// Inform the help system that a control has been created
		ControlHelpInfo Info;
		Info.pOpDesc  = pBarControl->GetOpDescriptor(Horz);
		Info.ModuleID = ToolModuleID;

		// Are we making a tool ?? if so we'll have to set it's select state ...
		if (Info.pOpDesc)
		{
			if ((Info.pOpDesc)->IsKindOf(CC_RUNTIME_CLASS(ToolOpDescriptor)))
			{
				ToolOpDescriptor * ThisOpDesc = (ToolOpDescriptor *)Info.pOpDesc;
				::SendMessage(CreatedWindow, BM_SETSTATE,(WORD)ThisOpDesc->IsCurrentTool(),0L);
			}
		}

		if (Info.pOpDesc == NULL)
		{
			Info.BubbleID = pBarControl->GetBubbleID(Horz);
			Info.StatusID = pBarControl->GetStatusID(Horz);
		}
		else
		{
			Info.BubbleID = _R(IDBBL_NOOP);
			Info.StatusID = 0;
		}

		if ( ThisControlType != BITMAPBUTTON &&
			 DDeckerState == DDECKER_NONE)
		{
			Ctl3dSubClassCtl(CreatedWindow);
		}

		ControlHelper::NotifyControlCreated(CreatedWindow,&Info); 
		
		if(ThisControlType == SLIDER)
		{
			if(Horz == FALSE)
			{
				::SetWindowPos(CreatedWindow,NULL,0,0,YSize,XSize,
											   SWP_NOZORDER|SWP_NOMOVE|SWP_NOREDRAW);

			}


		}
		// Create correctly sized normal and double decker controls
		// Add new controls to here as necessary
		if (ResizeWithFont)
		{
			// Reset the font of this control...
			if(DDeckerState == DDECKER_NONE)
			{
				::SendMessage( CreatedWindow, WM_SETFONT, (WPARAM)BarFont->m_hObject, 0);
				switch (ThisControlType)
				{
					case BUTTON:
					case BITMAPBUTTON:
					{
						wxRect cRect;
						::GetClientRect(CreatedWindow,&cRect);
						if(ParentDockBar->IsBigControls())
							::SetWindowPos(CreatedWindow,NULL,0,0,cRect.GetWidth(),LARGEBUTTONHEIGHT,
											   SWP_NOZORDER|SWP_NOMOVE|SWP_NOREDRAW);
						else
							::SetWindowPos(CreatedWindow,NULL,0,0,cRect.GetWidth(),SMALLBUTTONHEIGHT,
											   SWP_NOZORDER|SWP_NOMOVE|SWP_NOREDRAW);
						break;
					}
					case EDIT:
					case COMBO:
					{
						wxRect cRect;
						::GetClientRect(CreatedWindow,&cRect);
						::SetWindowPos(CreatedWindow,NULL,0,0,cRect.GetWidth(),COMBOHEIGHT,
											   SWP_NOZORDER|SWP_NOMOVE|SWP_NOREDRAW);
						break;
					}
					case ROTATEGRID:
						if(ParentDockBar->IsBigControls())
							::SetWindowPos(CreatedWindow,NULL,0,0,LARGEBUTTONHEIGHT,LARGEBUTTONHEIGHT,
											   SWP_NOZORDER|SWP_NOMOVE|SWP_NOREDRAW);
						else
							::SetWindowPos(CreatedWindow,NULL,0,0,SMALLBUTTONHEIGHT,SMALLBUTTONHEIGHT,
											   SWP_NOZORDER|SWP_NOMOVE|SWP_NOREDRAW);
						break;
				}
		
			}
			// Double decker 
			else
			{
				DDeckerControlHeights DDeckerHeight;
				wxFont * ThisFont ;
				if(ParentDockBar->IsBigControls())
				{
					if (ThisControlType == BITMAPBUTTON ||ThisControlType == BUTTON)
						ThisFont = FontFactory::GetwxFont(STOCKFONT_DIALOGBARLARGE);
					else
						ThisFont = FontFactory::GetwxFont(STOCKFONT_EDITFIELDLARGE);
					::SendMessage( CreatedWindow, WM_SETFONT, (WPARAM)ThisFont->m_hObject, 0);
					DDeckerHeight = LARGE_DDECKERS ;
				}
				else
				{
					if (ThisControlType == BITMAPBUTTON ||ThisControlType == BUTTON)
						ThisFont = FontFactory::GetwxFont(STOCKFONT_DIALOGBARSMALL);
					else
						ThisFont = FontFactory::GetwxFont(STOCKFONT_EDITFIELDSMALL);
					::SendMessage( CreatedWindow, WM_SETFONT, (WPARAM)ThisFont->m_hObject, 0);
					DDeckerHeight = SMALL_DDECKERS ;
				}
	
			
				switch(ThisControlType)
				{
					case BUTTON:
					case BITMAPBUTTON:
					{
						// NEW BUTTON STUFF
						if(pOp->IsKindOf(CC_RUNTIME_CLASS(SuperGallery)))
					   		::SetWindowPos(CreatedWindow,NULL,0,0,GalButtonWidth,DDeckerHeight ,
											   SWP_NOZORDER|SWP_NOMOVE|SWP_NOREDRAW);
						else
						{
							wxRect cRect;
							::GetClientRect(CreatedWindow,&cRect);
							::SetWindowPos(CreatedWindow,NULL,0,0,cRect.GetWidth(),DDeckerHeight ,
											   SWP_NOZORDER|SWP_NOMOVE|SWP_NOREDRAW);
						}

						
						break;
					}
					case EDIT:
					case STATIC:
					{
						wxRect cRect;
						::GetClientRect(CreatedWindow,&cRect);
						::SetWindowPos(CreatedWindow,NULL,0,0,cRect.GetWidth(),DDeckerHeight,
									   SWP_NOZORDER|SWP_NOMOVE|SWP_NOREDRAW);
						break;
					}
					case COMBO:
						::SendMessage(CreatedWindow,CB_SETITEMHEIGHT,(WPARAM)-1,(LPARAM)DDeckerHeight);
						::SendMessage(CreatedWindow,CB_SETITEMHEIGHT,(WPARAM)0,(LPARAM)DDeckerHeight);
						break;
				}
			}	
		}
	}
	return (CreatedWindow != NULL); */

	return false;
}  

/********************************************************************************************

>	LPWSTR BaseBar::MovePastWideStr(LPWSTR pWideStr)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/11/93
	Inputs:		pWideStr - the start of the string to skip
	Returns:	Address of first byte past the end of the string.
	Purpose:	Given a ptr to the first byte in a wide string (or char string if win16)
				return a ptr to the byte past the null.

********************************************************************************************/

LPWSTR BaseBar::MovePastWideStr(LPWSTR pWideStr)
{
	// Skip the string
	while (*pWideStr++)
		;

	// Return the address of the following character
	return pWideStr;
}

/********************************************************************************************

>	size_t BaseBar::SizeDlgHeader(DLGTEMPLATE *pHeader)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/04/94
	Inputs:		pHeader - start of the dialog box header.
	Returns:	Size of the dialog box header.					   
	Purpose:	Given a ptr to DIALOGBOXHEADER, return its size.
	Scope:		private

********************************************************************************************/


size_t BaseBar::SizeDlgHeader(DLGTEMPLATE *pHeader)
{
	PORTNOTETRACE( "dialog", "BaseBar::CreateControlFromResource - do nothing - Resource usage" );
#ifndef EXCLUDE_FROM_XARALX
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

	// then the font stuff
	if ((pHeader->style) & DS_SETFONT)
		pResult = (ADDR) MovePastWideStr((LPWSTR) (pResult + 2));		// skip font size too

	DWORD_ALIGN(pResult);

	// Compute the size of the header and return it.
	return (size_t) (pResult - ((ADDR) pHeader));
#else
	return 0;
#endif
}


/********************************************************************************************

>	size_t BaseBar::SizeCtrlData(DLGITEMTEMPLATE *pData)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/11/93
	Inputs:		pData - the start of the control structure.
	Returns:	Size of the control data.
	Purpose:	Given a ptr to a controldata struct, return its length.
				Under Win32, all strings are UniCode (as they are in the res file).

********************************************************************************************/

size_t BaseBar::SizeCtrlData(DLGITEMTEMPLATE *pControl)
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

>	BaseBar::InformControlsDead()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/4/94
	Inputs:		SrcBaseBar: The dialog to copy the controls from 
	Outputs:	-
	Returns:	-
	Purpose:	Copies the child controls of SrcBaseBar to this BaseBar. It does not
				do any positioning of controls. 
	Scope:		private
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void BaseBar::InformControlsDead()
{														 
#pragma message( __LOCMSG__ "BaseBar::InformControlsDead - do nothing" )
	TRACE( _T("Warning - BaseBar::InformControlsDead called") );
/* 	HWND CurrentChild;
	HWND NextChild; 
	for (CurrentChild = ::GetWindow(GetSafeHwnd(),GW_CHILD);
		 CurrentChild != NULL; 
		 CurrentChild = NextChild)
	{
		ControlHelper::NotifyControlDeleted(CurrentChild);
		NextChild = ::GetWindow(CurrentChild,GW_HWNDNEXT); 
	} */
}

/********************************************************************************************

>	wxRect BaseBar::GetDlgBarRect(DockBarType DockBar, wxRect CurrentBarRect)


	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/4/94
	Inputs:		DockBar: Specifies which docking bar
				CurrentBarRect: The current bar rectangle  
	Outputs:	-
	Returns:	The new bar rectangle

	Purpose:	For finding the screen rectangle defining the shape of the BaseBar when
				over DockBar 
	Errors:		-
	Scope:		private
	SeeAlso:	-

********************************************************************************************/
wxRect BaseBar::GetDlgBarRect(DockBarType DockBarType, wxRect CurrentBarRect)
{
/*	OILDockingBar* DockBar = GetMainFrame()->GetDockBar(DockBarType);
	switch (DockBarType)
	{
		case DOCKBAR_TOP:
		case DOCKBAR_BOTTOM:
			if (DockBar->IsBigControls())		
			{
				if (HorizontalLargeRect.IsEmpty()())
				{	
					FormatHorizontalBar(FALSE,USE_BAR);
				}
			}
			else
			{
				if (HorizontalSmallRect.IsEmpty()())
				{	
					FormatHorizontalBar(FALSE,USE_BAR);
				}
			}
			break;
		case DOCKBAR_LEFT:
		case DOCKBAR_RIGHT:
			if (DockBar->IsBigControls())		
			{
				if (VerticalLargeRect.IsEmpty()())
				{	
					FormatVerticalBar(FALSE,USE_BAR);
				}
			}
			else
			{
				if (VerticalSmallRect.IsEmpty()())
				{	
					FormatVerticalBar(FALSE,USE_BAR);
				}
			}
			break;
		default: 
		{
			ENSURE(DockBarType == DOCKBAR_FLOAT, "Unknown Dockbar type"); 
			if (DockBar->IsBigControls())		
			{
				if (FloatLargeRect.IsEmpty()())
				{	
					FormatFloatingBar(FALSE,USE_BAR);
				}
			}
			else
			{
				if (FloatSmallRect.IsEmpty()())
				{	
					FormatFloatingBar(FALSE,USE_BAR);
				}
			}
		
			break;
           
		} 
	}*/
	wxRect temp =GetFormatRect(DockBarType);  
	temp.Offset(CurrentBarRect.GetLeft(),CurrentBarRect.GetRight());   
//	temp.OffsetRect(CurrentBarRect.GetLeft(), CurrentBarRect.GetTop());
	return temp;
} 

/********************************************************************************************

>	BOOL BaseBar::CanStartDrag(wxPoint point, DlgDragType TypeOfDrag)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/4/94
	Inputs:		CursorPoint: The screen cursor pos
	Outputs:	-
	Returns:	YES / NO
	Purpose:	decides whether we can start a Drag
	Scope:		private
	Errors:		-
	SeeAlso:	-

********************************************************************************************/


BOOL BaseBar::CanStartDrag(wxPoint point)
{
#ifdef STANDALONE
#ifndef EXCLUDE_GALS
	if (!this->IsKindOf(RUNTIME_CLASS(GalleryBar)))
		return FALSE;
#endif
#endif

 return (point.x < (StartDragPoint.x - DRAG_START_MARGIN) ||
		 point.x > (StartDragPoint.x + DRAG_START_MARGIN) ||
		 point.y < (StartDragPoint.y - DRAG_START_MARGIN) ||
		 point.y > (StartDragPoint.y + DRAG_START_MARGIN) 
	     );
}	

/********************************************************************************************

>	void BaseBar::StartDrag(wxPoint point, DlgDragType TypeOfDrag)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/4/94
	Inputs:		CursorPoint: The screen cursor pos
				TypeOfDrag:	 The type of drag being started
	Outputs:	-
	Returns:	-
	Purpose:	Starts a Drag
	Scope:		private
	Errors:		-
	SeeAlso:	-

********************************************************************************************/


void BaseBar::StartDrag(wxPoint point, DlgDragType TypeOfDrag)
{
#pragma message( __LOCMSG__ "BaseBar::StartDrag - do nothing" )
	TRACE( _T("Warning - BaseBar::StartDrag called") );
/*#ifdef STANDALONE
#ifndef EXCLUDE_GALS
	// In StandAlone version we only let the user drag Galleries around
	if (!this->IsKindOf(RUNTIME_CLASS(GalleryBar)))
	{
		return;
	}
#endif
#endif
	
	//ControlHelper::BubbleHelpDisable();
	
	// disable resizing of an empty bar
	if (TypeOfDrag != MOVE)
	{
		if (pOp->IsListEmpty())
			return ;
	}

	ScreenRect.SetRect(0,0,wxSystemSettings::GetMetric(SM_CXSCREEN),wxSystemSettings::GetMetric(SM_CYSCREEN));

	BorderOffset = wxPoint(0,0);

	RECT rect;
	GetWindowRect(&rect);
	DragStartRect=rect;
	DragType = TypeOfDrag;
	CurrentBar = this;
	OldDragBarType = DragDockBarType = ParentDockBarType;



	if (TypeOfDrag == GROW_SOUTH || TypeOfDrag == GROW_NORTH)
	{
		OILDockingBar* DockBar = GetMainFrame()->GetDockBar(DragDockBarType);
		if (DockBar->IsBigControls())		
		{
			WidestFloat = HorizontalLargeRect.GetWidth();
			TallestFloat = VerticalLargeRect.GetHeight();
		}
		else
		{
			WidestFloat = HorizontalSmallRect.GetWidth();
			TallestFloat = VerticalSmallRect.GetHeight();
		}
		CacheFormatingData();
	} 

	switch (TypeOfDrag)
	{ 
		case MOVE :
			// Calculate drag offsets, used only by move drags
			DragOffset =  wxPoint(point.x - rect.GetLeft(), point.y - rect.GetTop());
			DragOffsetFromTop = TRUE;
			DragOffsetFromLeft = TRUE;
			CacheAllDockRects();
			// Close any drop-down windows in this bar
			CloseCombos();
			break;
		case GROW_WEST :
		 	BorderOffset = wxPoint(point.x - rect.GetLeft(),0);
			break;
		case GROW_EAST :
		 	BorderOffset = wxPoint(rect.GetRight() - point.x,0);
			break;
		case GROW_NORTH :
		 	BorderOffset = wxPoint(0,point.y - rect.GetTop());
			break;
		case GROW_SOUTH :
		 	BorderOffset = wxPoint(0,rect.GetBottom() - point.y);
			break;
		case GROW_NORTHWEST :
		 	BorderOffset = wxPoint(point.x - rect.GetLeft(),point.y - rect.GetTop());
			break;
		case GROW_NORTHEAST :
		 	BorderOffset = wxPoint(rect.GetRight() - point.x,point.y - rect.GetTop());
			break;
		case GROW_SOUTHEAST :
		 	BorderOffset = wxPoint(rect.GetRight() - point.x,rect.GetBottom() - point.y);
			break;
		case GROW_SOUTHWEST :
		 	BorderOffset = wxPoint(point.x - rect.GetLeft(),rect.GetBottom() - point.y);
			break;
	}	

//	TRACEUSER( "Gerry", _T("DragPoint     = (%d, %d)\n"), point.x, point.y);
//	TRACEUSER( "Gerry", _T("DragStartRect = (%d, %d) - (%d, %d)\n"), DragStartRect.GetLeft(), DragStartRect.top, DragStartRect.Width(), DragStartRect.GetHeight());
//	TRACEUSER( "Gerry", _T("BorderOffset  = (%d, %d)\n"), BorderOffset.x, BorderOffset.y);

	SetCapture(); // Capture the mouse 
	DragManagerOp::SetDragActive(TRUE);
	// Keep a copy of the rectangle so we can XOR it off
	OldDragRect = rect; 
	PaintDragRect(OldDragRect,wxRect(0,0,0,0),DragDockBarType,DragDockBarType); */
}


// -----------------------------------------------------------------------------------------
// Message map - maintained by Class Wizard.

BEGIN_EVENT_TABLE( BaseBar, wxWindow )
#pragma message( __LOCMSG__ "Removed BaseBar message map" )
/*	ON_WM_NCCALCSIZE()
	ON_WM_NCPAINT()
	ON_WM_NCACTIVATE()
	ON_WM_PAINT()
	ON_WM_NCHITTEST()
	ON_WM_MOUSEMOVE()
	ON_WM_KILLFOCUS()
	ON_WM_SETFOCUS()
	ON_WM_NCLBUTTONDOWN()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()	
	ON_WM_LBUTTONUP()
	ON_WM_NCRBUTTONDOWN()
	ON_MESSAGE( WM_RESIZECONTROLS,DoResize )
	ON_MESSAGE(WM_CTL_SETFOCUS, SetControlFocus)
	ON_MESSAGE(WM_MOVEBAR,MoveBar)
	ON_MESSAGE(WM_GETBARSIZE,GetBarSize)
	ON_MESSAGE(WM_START_CTLDRAG,StartControlDrag)
	ON_MESSAGE(WM_END_CTLDRAG,EndControlDrag)
	ON_WM_DESTROY( )
	ON_WM_DRAWITEM( )
	ON_WM_MOUSEACTIVATE( )
	ON_WM_ACTIVATE( )
	ON_WM_ACTIVATEAPP( )
	ON_WM_PARENTNOTIFY( )
	ON_LBN_KILLFOCUS(_R(IDC_GALLERY_LISTBOX),ListLostFocus)
	ON_LBN_SELCHANGE(_R(IDC_GALLERY_LISTBOX),ListSelChanged)
	ON_WM_MOUSEWHEEL() */
END_EVENT_TABLE()

/********************************************************************************************

>	void PatB(wxDC * hDC, INT32 x, INT32 y, INT32 dx, INT32 dy, COLORREF rgb)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com> (Some MFC dude, actually)
	Created:	14/3/94
	Inputs:		hDC - destination DC
				x,y,dx,dy - rectangle to fill
				rgb - colour to fill with

	Purpose:	Paints a rectangle in the given (dithered) colour
				It looks pretty hideous, but this is how the MFC buttonbar does it...
				The conclusions that this leads to are left as an exercise for the reader.
				(OK, so they're not. It suggest that either MFC sux, or plotting text is
				easier/faster than creating a brush, in which case Windoze sux)

	Scope:		private

********************************************************************************************/

static void PatB( wxDC &DC, INT32 x, INT32 y, INT32 dx, INT32 dy, wxColour &rgb )
{
#pragma message( __LOCMSG__ "PatB - do nothing" )
	TRACE( _T("Warning - PatB called") );
/*	RECT rect;

	rect.x		= x;
	rect.y		= y;
	rect.width	= dx;
	rect.height	= dy;

	cDC->SetBkColor(rgb);
	cDC->ExtTextOut(0, 0, ETO_OPAQUE, &rect, NULL, 0, NULL); */
}


/********************************************************************************************

>	void BaseBar::Paint3dPlinth(wxDC* pDC, wxRect *rect, BOOL PlinthOut)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/8/94
	Inputs:		pDC - The DC to draw into
				rect - The rectangle to draw around the OUTSIDE of with a plinth
				PlinthOut - FALSE to indent, TRUE to pop-out the plinth.
	Outputs:	-
	Returns:	-
	Purpose:	Draws a rectangular 3d plinth(Chicago/Win 3.1 styles)
				around the edge of the given rectangle

********************************************************************************************/

void BaseBar::Paint3dPlinth(wxDC* pDC, wxRect *rect, BOOL PlinthOut)
{
#pragma message( __LOCMSG__ "BaseBar::Paint3dPlinth - do nothing" )
	TRACE( _T("Warning - BaseBar::Paint3dPlinth called") );
/*	if((LOBYTE(LOWORD(GetVersion()))) > 3 )
	{
		// create brush for the background
		wxBrush Back(GetSysColor(COLOR_BTNFACE));
    	
    	// select into dc
		wxBrush * OldBrush = (wxBrush *) pDC->SelectObject(&Back);
		wxRect Fill(rect);
		Fill.Inflate(-1,-1);
		//Fill.OffsetRect(1,1);
		// fill in the background
	    pDC->FillRect(&Fill, &Back);
     	
     	// restore dc state
		pDC->SelectObject(OldBrush);
	
	
		COLORREF TopLeft;
		COLORREF BotRight;
		COLORREF TopLeftInside;
		COLORREF BotRightInside;


		if (!PlinthOut)
		{
			TopLeft			= GetSysColor(COLOR_WINDOWFRAME);
			BotRight		= GetSysColor(COLOR_BTNHIGHLIGHT);
			TopLeftInside	= GetSysColor(COLOR_BTNSHADOW);
			BotRightInside	= GetSysColor(COLOR_BTNFACE);
		}
		else
		{
			TopLeft			= GetSysColor(COLOR_BTNHIGHLIGHT);
			BotRight		= GetSysColor(COLOR_WINDOWFRAME);
			TopLeftInside	= GetSysColor(COLOR_BTNFACE);
			BotRightInside	= GetSysColor(COLOR_BTNSHADOW);
		}
		INT32 x  = rect->GetLeft();
		INT32 y  = rect->GetTop();
		INT32 dx = rect->GetWidth()-1;
		INT32 dy = rect->GetHeight();

		// The plinth is always made up of 4 right-angle elements
		// When indented, the colours are swapped, but the elements retain position
		PatB(pDC, x, y, 1, dy-1, TopLeft);
		PatB(pDC, x, y, dx-1, 1, TopLeft);

		PatB(pDC, x+dx-1, y, 1, dy, BotRight);
		PatB(pDC, x, y+dy-1, dx, 1, BotRight);
	
		PatB(pDC, x+1, y+1, 1, dy-2, TopLeftInside);
		PatB(pDC, x+1, y+1, dx-2, 1, TopLeftInside);
	
		PatB(pDC, x+dx-2, y+1, 1, dy-2, BotRightInside);
		PatB(pDC, x+1, y+dy-2, dx-2, 1, BotRightInside);
	}
	else
	{
		// create brushes /pens etc..
		wxPen Black(PS_SOLID, 0,RGB(0,0,0));
		wxPen Shadow(PS_SOLID, 0, GetSysColor(COLOR_BTNSHADOW));
		wxPen White(PS_SOLID, 0, GetSysColor(COLOR_BTNHIGHLIGHT));
		wxBrush Back(GetSysColor(COLOR_BTNFACE));

		// select into dc
		wxPen *OldPen = (wxPen *) pDC->SelectObject(&Black);
		wxBrush * OldBrush = (wxBrush *) pDC->SelectObject(&Back);
	    
		// fill in the background
	    pDC->FillRect(rect, &Back);
     
		// draw the black outline
		pDC->MoveTo(rect->GetLeft(), rect->bottom-2);
		pDC->LineTo(rect->GetLeft(), rect->GetTop());
		pDC->MoveTo(rect->right-1, rect->bottom-2);
		pDC->LineTo(rect->right-1, rect->GetTop());
		pDC->MoveTo(rect->GetLeft()+1, rect->bottom-1);
		pDC->LineTo(rect->right-1, rect->bottom-1);
		pDC->MoveTo(rect->GetLeft()+1, rect->GetTop());
		pDC->LineTo(rect->right-1, rect->GetTop());


		// draw the 3d slabbing 
		pDC->SelectObject(((PlinthOut) ? &White : &Shadow));
		pDC->MoveTo(rect->GetLeft()+1, rect->bottom-2);
		pDC->LineTo(rect->GetLeft()+1, rect->GetTop()+1);
		pDC->LineTo(rect->right-1, rect->GetTop()+1);

		pDC->SelectObject(((PlinthOut) ? &Shadow : &White));
		pDC->MoveTo(rect->right-2, rect->GetTop()+2);	// Ensure corner pixel is correct
		pDC->LineTo(rect->right-2, rect->bottom-2);
		pDC->LineTo(rect->GetLeft()+1, rect->bottom-2);

		// restore dc state
		pDC->SelectObject(OldPen);
		pDC->SelectObject(OldBrush);
	} */
 
}

/********************************************************************************************

>	void BaseBar::OnDrawItem( INT32 nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct )

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/7/94
	Inputs:		nIDCtl - control id of the gadget to be drawn  
				lpDrawItemStruct - control data
	Outputs:	-
	Returns:	-
	Purpose:	- called when an owner drawn child of this bars needs to be redraw

********************************************************************************************/


#pragma message( __LOCMSG__ "BaseBar::OnDrawItem removed" )
/*void BaseBar::OnDrawItem( INT32 nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct )
{
	// owner drawn text button
	if(lpDrawItemStruct->CtlType == ODT_BUTTON)
	{
		wxRect Rect(lpDrawItemStruct->rcItem);
		wxDC hDC;
		hDC.Attach(lpDrawItemStruct->hDC);
	
		// is the button selected 
		BOOL Selected = lpDrawItemStruct->itemState & ODS_SELECTED;
		BOOL Focus = lpDrawItemStruct->itemState & ODS_FOCUS;
	
		// paint background and border
//		if(lpDrawItemStruct->CtlType== ODT_BUTTON)
			Paint3dPlinth(& hDC,&Rect,!Selected);
		
   		// if selected shift the text x+1 and y+1
		if(Selected)
			Rect.OffsetRect(1, 1);

		// text paint mode transparent
		hDC.SetBkMode(TRANSPARENT);
		
		if ((lpDrawItemStruct->itemState & (ODS_GRAYED | ODS_DISABLED)) != 0)
		{
			// Slight cheat here- using the button shadow colour, rather than dithering
			// the output text as Windows does. This is because we use a small, thin font
			// in these buttons, which does not dither well, and solid colour looks
			// much better (Note: We can't use GRAYTEXT, because it's the same as BTNFACE!)
			hDC.SetTextColor(GetSysColor(COLOR_BTNSHADOW));
		}
		else
		   	hDC.SetTextColor(GetSysColor(COLOR_BTNTEXT));
		
		char wtext[33];
	
		// get the button text
		::GetWindowText(lpDrawItemStruct->hwndItem,(LPTSTR)wtext,32);
	
		// now draw it formated into the button rectangle
 		hDC.DrawText(wtext,-1,&Rect,DT_CENTER|DT_VCENTER|DT_SINGLELINE);

		if(Focus)
		{
			wxBrush Back(RGB(0,0,0));
			// select into dc
			wxBrush * OldBrush = (wxBrush *) hDC.SelectObject(&Back);
	    
			hDC.Ellipse(Rect.GetLeft()+4,Rect.GetTop()+4,Rect.GetLeft()+8,Rect.GetTop()+8);
		   	hDC.SelectObject(OldBrush);
		}
		// must detach else we eat up GDI16 resources
		hDC.Detach();
	}
} */

// ------------------------------------------------------------------------------------------
// Message Handler functions

/********************************************************************************************

>	void BaseBar::OnParentNotify( UINT32 message, LPARAM lParam )

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/7/94
	Inputs:		from Windows
	Outputs:	-
	Returns:	-
	Purpose:	Sets ControlHasFocus flag if input focus with a writable control
	Errors:		-										 
	SeeAlso:	-
	Scope:		Public

********************************************************************************************/
						
#pragma message( __LOCMSG__ "BaseBar::OnParentNotify removed" )
/*void BaseBar::OnParentNotify( UINT32 message, LPARAM lParam )
{
	if(message == WM_LBUTTONDOWN)
	{
		String_256 ClassNameStr;
		POINT Pnt;
		GetCursorPos(&Pnt);
		wxWindow * Cwnd = WindowFromPoint(Pnt); 
		GetClassName(Cwnd->GetSafeHwnd(), (TCHAR*)ClassNameStr, 255);

	   	// check for ComboBox/Edit/ListBox
	   	if ( (ClassNameStr == String_8(TEXT("ListBox"))) || //ZZZ
		 	 (ClassNameStr == String_8(TEXT("Edit"))) ||
			 (ClassNameStr == String_8(TEXT("ComboBox"))) ||
			 (ClassNameStr == String_64(TEXT("cc_1dBitmapComboBoxEdit"))) ||
			 (ClassNameStr == String_64(TEXT("cc_2dBitmapComboBoxEdit")))
		   )

		{
			ControlHasFocus = TRUE;
		}
	}	 
} */

/********************************************************************************************

>	void BaseBar::OnKillFocus(wxWindow * Wnd)

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/7/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	- nothing at the moment
	Errors:		-										 
	SeeAlso:	-
	Scope:		Public

********************************************************************************************/

#pragma message( __LOCMSG__ "BaseBar::OnKillFocus removed" )
/*void BaseBar::OnKillFocus( wxWindow*Wnd )
{
	wxWindow::OnKillFocus(Wnd);
} */

/********************************************************************************************

>	void BaseBar::OnSetFocus(wxWindow * Wnd)

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/7/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	
	Errors:		-										 
	SeeAlso:	-
	Scope:		Public

********************************************************************************************/


#pragma message( __LOCMSG__ "BaseBar::OnSetFocus removed" )
/*void BaseBar::OnSetFocus( wxWindow * Wnd )
{
	wxWindow::OnSetFocus(Wnd);
} */

/********************************************************************************************

>	void BaseBar::OnActivateApp( BOOL  bActive, HTASK  hTask )

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/7/94
	Inputs:		from mfc
	Outputs:	-
	Returns:	-
	Purpose:	Forces bars to be repainted to reflect active state
	Errors:		-										 
	SeeAlso:	-
	Scope:		Public

********************************************************************************************/

#pragma message( __LOCMSG__ "BaseBar::OnActivateApp removed" )
/*void BaseBar::OnActivateApp( BOOL  bActive, HTASK  hTask )
{
	
	Active = bActive;
    ::SendMessage(GetSafeHwnd(), WM_NCPAINT,0,0);
		
	if(!Active && CurrentBar != NULL)
	{
	 	if(CtlDragState==DRAGGING)
		{	
			CurrentBar->PaintDragRect(CurrentBar->OldDragRect,wxRect(0,0,0,0),DOCKBAR_FLOAT,DOCKBAR_FLOAT);
			CurrentBar->TidyUpControlDrag();
		}
		else if(DragType != NONE)
			CurrentBar->TidyUpBarDrag();
	}
} */

/********************************************************************************************

>	void BaseBar::OnActivate( UINT32 state , wxWindow* LastWin, BOOL IsMin)

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/6/94
	Inputs:		from mfc
	Outputs:	-
	Returns:	-
	Purpose:	attempt to force focus to mainframe 
	Errors:		-										 
	SeeAlso:	-
	Scope:		Protected

********************************************************************************************/

#pragma message( __LOCMSG__ "BaseBar::OnActivate removed" )
/*void BaseBar::OnActivate( UINT32 State, wxWindow* pWndOther, BOOL bMinimized ) 
{
	
	//if a control has the focus do nothing
	if(ControlHasFocus)
		return;
	
	
	// pass the focus back to the mainframe
	if ( State == WA_ACTIVE )
	{
  	 	GetMainFrame()->SetActiveWindow();
		return;
	}
	 
	//if(GetMainFrame() == pWndOther) return;

	wxWindow::OnActivate(State, pWndOther,bMinimized);
	
} */

/********************************************************************************************

>	void BaseBar::OnMouseActivate(wxWindow* pDesktopWnd, UINT32 nHitTest, UINT32 message ) 

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/6/94
	Inputs:		from mfc
	Outputs:	-
	Returns:	-
	Purpose:	-
	Errors:		-
	SeeAlso:	-
	Scope:		Protected

********************************************************************************************/

#pragma message( __LOCMSG__ "BaseBar::OnMouseActivate removed" )
/*INT32 BaseBar::OnMouseActivate( wxWindow* pDesktopWnd, UINT32 nHitTest, UINT32 message ) 
{
	
	// fix by chris
	// if camelot isn't active we should just use default handling - i.e. make camelot active 
	// don't just throw away the click.
	if(GetMainFrame()!= GetActiveWindow())
		return wxWindow::OnMouseActivate( pDesktopWnd, nHitTest, message ); 
	
	
	
	String_256 ClassNameStr;
	POINT Pnt;
	GetCursorPos(&Pnt);
	wxWindow * Cwnd = WindowFromPoint(Pnt); 
	GetClassName(Cwnd->GetSafeHwnd(), (TCHAR*)ClassNameStr, 255);
	ControlHasFocus = FALSE;	
	// check for ComboBox/Edit
	if ( (ClassNameStr == String_8(TEXT("ListBox"))) || //ZZZ
		 (ClassNameStr == String_8(TEXT("Edit"))) ||
		 (ClassNameStr == String_8(TEXT("ComboBox"))) ||
		 (ClassNameStr == String_64(TEXT("cc_1dBitmapComboBoxEdit"))) ||
		 (ClassNameStr == String_64(TEXT("cc_2dBitmapComboBoxEdit")))
		)
	{
		ControlHasFocus = TRUE;
	}
	if(ControlHasFocus)
		return MA_ACTIVATE;
	return MA_NOACTIVATE;
	
//	return wxWindow::OnMouseActivate( pDesktopWnd, nHitTest, message );   
} */

/********************************************************************************************
>	void BaseBar::OnDestroy( )

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/5/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	To Notify ControlHelper and DockingBar that BaseBar is about to die 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

#pragma message( __LOCMSG__ "BaseBar::OnDestroy removed" )
/*void BaseBar ::OnDestroy( )
{
 
	// If the current dialog is set to this bar, set the current dlg to NULL
	HWND hwnd = GetSafeHwnd();
	if(hwnd == 0)
		return;
	
 	if (DialogManager::hDlgCurrent == hwnd)
		DialogManager::hDlgCurrent = NULL;

	TRACEUSER( "chris", _T("Destruct Bar %X\n"),hwnd);	    
    // inform Help system
	
	InformControlsDead();

	ControlHelper::NotifyBarDeleted(GetSafeHwnd());

	// release bar from it's dock
	// If the tool is simply changing rather being closed down we don't want to
	// force a tidy 

	BOOL DoTidy = DO_TIDY;
	if (ThisIsAnInfoBar )
		if (Tool::IsToolChanging())
			DoTidy = DONT_TIDY;

	if (Camelot.CamelotIsDying())
	{
		DoTidy = DONT_TIDY; // We can be messy cos camelot is shutting down
	}

	GetMainFrame()->GetDockBar(ParentDockBarType)->ReleaseBar(this->GetSafeHwnd(),DoTidy);
 
}; */

/********************************************************************************************

>	void BaseBar::ListLostFocus(void )

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/5/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Command received when a list box receives the focus 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

#pragma message( __LOCMSG__ "BaseBar::ListLostFocus removed" )
/*void  BaseBar::ListLostFocus(void)
{
 
}*/	

/********************************************************************************************

>	void BaseBar::ListSelChanged(void )

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/5/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Command received when a list box receives the focus - assumes only galleries 
				will have listbox controls 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

#pragma message( __LOCMSG__ "BaseBar::ListSelChanged removed" )
/*void  BaseBar::ListSelChanged(void)
{
//	if( pOp->IsKindOf(CC_RUNTIME_CLASS(SuperGallery)) )
//	{
//		GetMainFrame()->SetActiveWindow();
//	}
}*/

/********************************************************************************************

>	LRESULT BaseBar::DoResize(WPARAM , LPARAM )

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/5/94
	Inputs:		from windows
	Outputs:	-
	Returns:	-
	Purpose:	To recreate this BaseBar after a change in control sizes 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

#pragma message( __LOCMSG__ "BaseBar::DoResize removed" )
/*LRESULT  BaseBar::DoResize(WPARAM , LPARAM)
{
	// ZZZZ
	KernelBarPos * Pos;
	
//	if(LastSmallBarPos.x !=-1 &&LastSmallBarPos.y!=-1)
//		Pos = & LastSmallBarPos;
//	else
	 	Pos = GetMainFrame()->GetDockBar(ParentDockBarType)->
					 GetBarPos(this->GetSafeHwnd()); 
	
   	LastSmallBarPos = * Pos;
	KernelBarPos NewPos = *Pos;

	// create new BaseBar
	BaseBar* NewBaseBar = (BaseBar *)GetRuntimeClass()->CreateObject(); 

	// Recreate the Dialog at the old position
	NewBaseBar->RecreateAt(NewPos,ParentDockBarType, this, pOp,this->GetSafeHwnd()); 
	//TRACE( _T("Recreate %d"),this->GetSafeHwnd());
	// The operations window id has changed 
	CCamApp::GetDlgManager()->Delete(this->GetSafeHwnd(),pOp);  
	//delete this;

 	return 0;
}*/

/********************************************************************************************

>	BOOL BaseBar::HaveNonClientMetricsChanged( )

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/5/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	returns TRUE if Non-Client metrics have changed 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL  BaseBar::HaveNonClientMetricsChanged( )
{
	BOOL returnVal = FALSE;

	#if _MFC_VER >= 0x400
		
		if( TitleBarHeight != wxSystemSettings::GetMetric(SM_CYSMCAPTION))
			returnVal = TRUE;
		if(BorderWidth !=  wxSystemSettings::GetMetric(SM_CXFRAME))
			returnVal = TRUE;
		if(FontFactory::CheckSystemBarFontsChanged())
			returnVal = TRUE;	
 	
	#endif
	
	return returnVal;
}

/********************************************************************************************

>	LRESULT BaseBar::GetBarSize(WPARAM DockType, LPARAM ControlSize )

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/5/94
	Inputs:		from windows
	Outputs:	-
	Returns:	-
	Purpose:	To return a pointer to a rectangle of bar size 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

#pragma message( __LOCMSG__ "BaseBar::GetBarSize removed" )
/*LRESULT  BaseBar::GetBarSize(WPARAM DockType , LPARAM ControlS)
{
	DockBarType ThisDock = (DockBarType) DockType;	
	ControlSizes ControlSize = (ControlSizes ) ControlS;

	switch (ThisDock)
	{
 		case DOCKBAR_TOP:
		case DOCKBAR_BOTTOM:
			if(ControlSize == LARGE_CONTROLS)
				return (LRESULT) &HorizontalLargeRect ;
			else
				return (LRESULT) &HorizontalSmallRect ; 
		case DOCKBAR_LEFT:
		case DOCKBAR_RIGHT:
			if(ControlSize == LARGE_CONTROLS)
				return (LRESULT) &VerticalLargeRect ;
			else
				return (LRESULT) &VerticalSmallRect ; 
	
		case DOCKBAR_FLOAT:
			if(ControlSize == LARGE_CONTROLS)
				return (LRESULT) &FloatLargeRect ;
			else
				return (LRESULT) &FloatSmallRect ; 
	}
	return 0;
} */

/********************************************************************************************

>	LRESULT BaseBar::GetBarSize(DockBarType DockBarType)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/4/2000
	Inputs:		the dockbar type
	Outputs:	-
	Returns:	-
	Purpose:	To return the size of the bar
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

wxRect BaseBar::GetBarsDims (DockBarType DockBarType)
{
	wxRect ReturnRect(0,0,0,0);

#pragma message( __LOCMSG__ "BaseBar::GetBarsDims - do nothing" )
	TRACE( _T("Warning - BaseBar::GetBarsDims called") );
/*	OILDockingBar* FloatBars = GetMainFrame ()->GetDockBar (DockBarType);//DOCKBAR_FLOAT);
	BOOL BigControls = FloatBars->IsBigControls ();
	
	switch (DockBarType)
	{
 		case DOCKBAR_TOP:
		case DOCKBAR_BOTTOM:
			if (BigControls)
				ReturnRect = HorizontalLargeRect;
			else
				ReturnRect = HorizontalSmallRect;
		case DOCKBAR_LEFT:
		case DOCKBAR_RIGHT:
			if (BigControls)
				ReturnRect = VerticalLargeRect;
			else
				ReturnRect = VerticalSmallRect;
	
		case DOCKBAR_FLOAT:
			if (BigControls)
				ReturnRect = FloatLargeRect;
			else
				ReturnRect = FloatSmallRect;
	} */

	return ReturnRect;
}



/********************************************************************************************

>	LRESULT BaseBar::SetControlFocus(WPARAM , LPARAM )

	Author:		everyone...
	Created:	7/5/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	used to be sent by controls to inform bar that a control has got the focus
				not ****ing sure anymore .
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

#pragma message( __LOCMSG__ "BaseBar::GetBarSize removed" )
/*LRESULT BaseBar::SetControlFocus(WPARAM State, LPARAM Hwnd)
{
	if (State == TRUE)
		DialogManager::hDlgCurrent = GetSafeHwnd();
	else
		DialogManager::hDlgCurrent = NULL;

	if(ControlHasFocus == (BOOL)State)
	{
		return 0;
	}
	

	if(State !=COMMIT && HwndWithFocus != (HWND)Hwnd)
		return 0;
	
	// this is sent after return is pressed in a control
	if(State == COMMIT)
		State = FALSE;

 	if(!GetMainFrame()->IsAppActive())
	{
		return 0;
 	}
	ControlHasFocus = State	;
	HwndWithFocus = (HWND)Hwnd ;
	// give the focus back to the main frame
	if(State == FALSE)
	{ 
		GetMainFrame()->SetActiveWindow();
		::HideCaret(NULL);
	}
	
	return 0;
} */											 	

/********************************************************************************************

>	LRESULT BaseBar::StartControlDrag(WPARAM , LPARAM )

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15\11/94
	Inputs:		from windows 
	Outputs:	-
	Returns:	-
	Purpose:	sent by controls to inform bar that a control is about to be dragged
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

#pragma message( __LOCMSG__ "BaseBar::GetBarSize removed" )
/*LRESULT  BaseBar::StartControlDrag(WPARAM State, LPARAM Hwnd)
{
#ifdef STANDALONE
	return FALSE;
#else

	if(!KeyPress::IsAlternativePressed())
		return FALSE;
	// we can't drag items on or off infobars or double deckers (for the mo)
	BarControl * foundControl = (BarControl*)pOp->GetBarItem((HWND)Hwnd);

	if (foundControl)
	{
		// use these to get pointers to first and last item of double decker if found  
		BarControl * DDStart = NULL; 
		BarControl * DDEnd = NULL;
		
		BOOL DoubleDecker = FALSE;
		if(IsDoubleDecker(foundControl,DDStart,DDEnd)!=DDECKER_NONE)
			DoubleDecker = TRUE;
		
		if(!ThisIsAnInfoBar && !DoubleDecker)
		{
			InRecreate = TRUE;
			// keep a record of the ctl hwnd
			DragCtlHwnd = (HWND)Hwnd;
		
			// get the initial outline
			::GetWindowRect(DragCtlHwnd,&OldDragRect);
			
			// and the 'size'
			::GetClientRect(DragCtlHwnd,&DragCtlClientRect);
		
			// calculate the cursor offset into the control
			wxPoint CursorPos;
			GetCursorPos(&CursorPos);
			::ScreenToClient(DragCtlHwnd,&CursorPos);
			DragCtlOffset = CursorPos;
		
			// paint the outline
			PaintDragRect(OldDragRect,wxRect(0,0,0,0),DOCKBAR_FLOAT,DOCKBAR_FLOAT);
		
 			// Dragging Control is on
	 		CtlDragState = DRAGGING;

			// and capture the mouse
	 		SetCapture();
	 		CurrentBar = this;
	//	 	GetMainFrame()->SetActiveWindow();			// BODGE! Removed by Phil 13/10/96 (at 23.12!)
	 		DragManagerOp::SetDragActive(TRUE);
	 		
	 		if(CtlDragMode == COPY)
	 			pCurrentCursor  = new Cursor(_R(IDC_BUTTONDRAGCOPY));
			else
				pCurrentCursor  = new Cursor(_R(IDC_BUTTONDRAGCUT));

			// Did the cursor create ok ?
			if (pCurrentCursor && pCurrentCursor->IsValid())
			{
				CurrentCursorID = CursorStack::GPush(pCurrentCursor, TRUE);	// Push cursor, and display now
			}

		}
 		return 0;
	}

	return 0;
#endif
} */											 	

#pragma message( __LOCMSG__ "BaseBar::GetBarSize removed" )
/*LRESULT  BaseBar::EndControlDrag(WPARAM State, LPARAM Hwnd)
{
	return 0;
} */

/********************************************************************************************

>	LRESULT BaseBar::StartControlDrag(WPARAM , LPARAM )

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15\11/94
	Inputs:		from windows 
	Outputs:	-
	Returns:	-
	Purpose:	sent by controls to inform bar that a control is about to be dragged
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL  BaseBar::SwapCursors(DWORD CursorID)
{
	ERROR2IF(pCurrentCursor==NULL,FALSE,"Null last Cursor in SwapCursors");
	ERROR2IF(CurrentCursorID==0,FALSE,"CurrentID 0 in SwapCursors");

	if(pCurrentCursor)
	{
		CursorStack::GPop(CurrentCursorID);
		delete pCurrentCursor;
		pCurrentCursor = NULL;
		CurrentCursorID = 0;
	}

	pCurrentCursor  = new Cursor(CursorID);

	// Did the cursor create ok ?
	if (pCurrentCursor && pCurrentCursor->IsValid())
	{
		CurrentCursorID = CursorStack::GPush(pCurrentCursor, TRUE);	// Push cursor, and display now
	}
	else
		return FALSE;

return TRUE	;
}											 	


										 	

/********************************************************************************************

>	LRESULT BaseBar::WindowProc( UINT32 message, WPARAM wParam, LPARAM lParam )

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/4/94
	Inputs:		from windows
	Outputs:	-
	Returns:	-
	Purpose:	The BaseBar's WindowProc firstly sends the message to the dialog manager
				so that it can dispatch it to the appropriate DialogBarOp (if neccessary). 
				Then the message is processed as normal. 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

LRESULT BaseBar::WindowProc( UINT32 Message, WPARAM wParam, LPARAM lParam )
{
#pragma message( __LOCMSG__ "BaseBar::WindowProc - do nothing" )
	TRACE( _T("Warning - BaseBar::WindowProc called") );
/*	// First send the message to the dialog manager 
	DialogManager::SendDialogMessage(this,
				       	  			 Message,
				       	 			 wParam,
				       	 			 lParam);


	// this checks to see whether this Window is still around 
	// it is possible that the kernel just deleted this window 
	// no further proceesing if this is the case !
	if(!(IsWindow(GetSafeHwnd()))) return  0;
	// return brush for ctl3d
	switch(Message)
	{
		case WM_COMMAND:
			//TRACEUSER( "chris", _T("WM_COMMAND to %d\n"),GetSafeHwnd());
			// user has closed down a combo list - return focus to mainframe 
			if(HIWORD(wParam)==CBN_CLOSEUP)
				GetMainFrame()->SetFocus();

			break; 
	#ifdef WIN32
		case WM_CTLCOLORBTN:
		case WM_CTLCOLORDLG:
		case WM_CTLCOLOREDIT:
		case WM_CTLCOLORLISTBOX:
		case WM_CTLCOLORMSGBOX:
		case WM_CTLCOLORSCROLLBAR:
		case WM_CTLCOLORSTATIC:
	#else
		case WM_CTLCOLOR:
	#endif
   		HBRUSH hbrush = DoCtl3dCtlColorEx(Message, wParam, lParam);
   		if (hbrush != NULL)
      		return (LRESULT)hbrush;
	}
   	// Now process the message normally
	return(wxWindow::WindowProc( Message, wParam, lParam )); */

	return 0;
}


/********************************************************************************************

>	void BaseBar::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/9/93
	Inputs:		bCalcValidRects - unused
				lpncsp - pointer to the structure containing the window area.
	Outputs:	lpncsp - this contains the adjusted area which indicates which part of the
				window is the client area.  (See documentation on WM_NCCALCSIZE message).
				Although this function has a void return, MFC actually returns 0 to Windows
				in its message dispatcher (but by God you have to search for it).
	Returns:	-
	Purpose:	Calculates the area of the window which corresponds to the client area.  This
				basically includes the whole window area, but excluding any dragbar.
	Errors:		-
	SeeAlso:	BaseBar::OnNcHitTest
	Scope:		Protected

********************************************************************************************/
#pragma message( __LOCMSG__ "BaseBar::OnNcCalcSize removed" )
/*void BaseBar::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp)
{
	// So that we are compatible with different screen modes we will paint the non-client area
	// based on System metrics. 

	if (CurrentDlgBarMode == DIALOG)
	{
		// The dialog bar is a floater so we must specify our own non client area
		lpncsp->rgrc[0].GetLeft() += BorderWidth ;
		lpncsp->rgrc[0].GetTop() += TitleBarHeight + BorderWidth;	// UIC
		lpncsp->rgrc[0].GetRight() -= BorderWidth;
		lpncsp->rgrc[0].GetBottom() -= BorderWidth;
	}
	else
	{
		// The dialog bar is a bar so define the standard non client area for a child with a 
		// border.
		wxWindow::OnNcCalcSize(bCalcValidRects, lpncsp); 
	}									   
} */


/********************************************************************************************

>	void BaseBar::OnNcPaint()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/9/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Paint the non-client area - this involves painting the dragbar and any
				controls it has, if any.
	Errors:		-
	SeeAlso:	BaseBar::PaintDragBar
	Scope:		Protected

********************************************************************************************/
#pragma message( __LOCMSG__ "BaseBar::OnNcPaint removed" )
/*void BaseBar::OnNcPaint()
{
	
	if (CurrentDlgBarMode == DIALOG)
	{
	// nearly-fabby win95 style floating bars
		if(IS_CHICAGO)
			PaintWin95DialogNonClient(Active);
		else
	
			PaintDialogNonClient(Active);
	}
	else  // Normal processing
	{
		if(IS_CHICAGO)
			PaintChicagoBorder();
		else
			wxWindow::OnNcPaint();
	  
	}
										
} */



/******************************************************************************************** UIC
>	void PlotStretchedMaskedBitmap(wxDC* destDC, CBitmap* srcBitmap, wxRect Pos)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/3/94
	Inputs:		destDC    - DC into which the BMP will be plotted
				srcBitmap - source bitmap
				Pos - rectangle to plot into
	Purpose:	Plots the given bitmap (srcDC) into the given rectangle of the destination
				(destDC).
				if srcDC == NULL, then just fills the button with grey, else...
********************************************************************************************/

// Raster Ops, used to...

// Plot colour in the glyph bitmaps transparent (grey) regions
#define ROP_DSPDxax  0x00E20746L

// Plot colour in the glyph bitmaps non-transparent (coloured) regions
#define ROP_PSDPxax  0x00B8074AL


#pragma message( __LOCMSG__ "BaseBar::PlotStretchedMaskedBitmap removed" )
/*BOOL BaseBar::PlotStretchedMaskedBitmap(wxDC* destDC, CBitmap* srcBitmap,wxRect Pos)
{
	
	ERROR2IF(srcBitmap == NULL,FALSE,"NULL Bitmap in PlotMaskedBitmap()");
	ERROR2IF(destDC == NULL,FALSE,"NULL DC in PlotMaskedBitmap()");

	BITMAP bits;
    srcBitmap->GetObject(sizeof(BITMAP),&bits);

	// always fill the entire background with BTNFACE to make sure we don't
	// miss any pixels 

	PatB(destDC,Pos.GetLeft(),Pos.GetTop(), Pos.GetWidth(), Pos.GetHeight(), GetSysColor(COLOR_BTNFACE));
	
	// create a screen dc
	wxDC scrDC ;
	scrDC.CreateCompatibleDC(destDC);
	
	// and select the bitmap into it
	CBitmap* OldSrcBitmap = (CBitmap *) scrDC.SelectObject(srcBitmap);

	// create a mask for the button glyph... First a DC
	wxDC hDCMono ;
	hDCMono.CreateCompatibleDC(NULL);

	// create a mono bitmap for the mask
	CBitmap hbmMono ; 
	hbmMono.CreateBitmap(Pos.GetWidth(),Pos.GetHeight(), 1, 1, NULL);
	
	CBitmap * oldmonobitmap = (CBitmap *)hDCMono.SelectObject(&hbmMono);

	// initalize the bitmapto white (all 1's)
	hDCMono.PatBlt(0, 0,Pos.GetWidth(),Pos.GetHeight(), WHITENESS);

	// plot all non-lightgrey  pixels in the glyph as black in our mask
	scrDC.SetBkColor( RGB(192,192,192));
	hDCMono.StretchBlt(0,0, Pos.GetWidth(),Pos.GetHeight(), &scrDC,
			0, 0,bits.bmWidth, bits.bmHeight, SRCCOPY);

	destDC->SetTextColor(0L);                  // 0's in mono -> 0 (for ROP)
	destDC->SetBkColor((COLORREF)0x00FFFFFFL); // 1's in mono -> 1

	// plot the glyph 
	destDC->StretchBlt( Pos.GetLeft(),Pos.GetTop(),Pos.GetWidth(),Pos.GetHeight(),  &scrDC, 
							0, 0, bits.bmWidth, bits.bmHeight,SRCCOPY);
	
	// fill with the normal BTNFACE colour
	wxBrush HighlightBrush ; 
	HighlightBrush.CreateSolidBrush(GetSysColor(COLOR_BTNFACE));

	wxBrush* hbrOld = (wxBrush *) destDC->SelectObject(&HighlightBrush);
	if (hbrOld != NULL)
	{
		// draw highlight color where we have 0's in the mask
		destDC->BitBlt( Pos.GetLeft(),Pos.GetTop(), Pos.GetWidth(),Pos.GetHeight(),  &hDCMono, 0, 0, ROP_DSPDxax);
		destDC->SelectObject(hbrOld);
	}
	
	// clean up DC's
	hDCMono.SelectObject(oldmonobitmap);
	scrDC.SelectObject(OldSrcBitmap);
	return TRUE;
} */


 /********************************************************************************************

>	void BaseBar::PaintWin95DialogNonClient(BOOL Active)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/8/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	This routine gets called by OnNcPaint to paint the non client area of a 
				Windows 95 style floating bar. 
	Errors:		-
	SeeAlso:	BaseBar::OnNcPaint

********************************************************************************************/


void BaseBar::PaintWin95DialogNonClient(BOOL IsActive)
{
/*	if( pOp->IsKindOf(CC_RUNTIME_CLASS(SuperGallery)) )
	{	
		IsActive = ( this == GetActiveWindow() );
 	}
  */

	// When the BaseBar is in DIALOG mode we have to paint the fancy non-client area. 

	// Get the size of the window	
	wxRect WinRect( GetRect() );

	// Set up the active non-client regions 
	SysMenuRect 	 = wxRect(WinRect.GetWidth() - TitleBarHeight - BorderWidth+2, 
			  	         BorderWidth + 1, 
			  		     WinRect.GetWidth() - BorderWidth , 
			  		     BorderWidth - 2 + TitleBarHeight );
//	TRACE( _T("SMRB = (%d, %d)\n"), SysMenuRect.GetWidth(), SysMenuRect.GetHeight());
	
	TopBorderRect	 = wxRect(0,
				    	 0, 
				 		 WinRect.GetWidth(), 
				 		 BorderWidth); 

	BottomBorderRect = wxRect(0,
				 		 WinRect.GetHeight() - BorderWidth, 
				 		 WinRect.GetWidth(), 
				 		 WinRect.GetHeight()); 

	LeftBorderRect =   wxRect(0,
				   		 0, 
				 		 BorderWidth,
				 		 WinRect.GetHeight());
				 		 
	RightBorderRect =  wxRect(WinRect.GetWidth() - BorderWidth, 
				 		 0,
				 		 WinRect.GetWidth(), 
				 		 WinRect.GetHeight());

	CaptionBarRect =   wxRect(BorderWidth , 
				 		 BorderWidth-1 , 
				 		 WinRect.GetWidth() - BorderWidth + 1, 
				 		 TitleBarHeight+BorderWidth);  


	TopLeftGrowRect =  wxRect(0, 
				 		 0,
				 		 TitleBarHeight  + BorderWidth +1, 
				 		 TitleBarHeight  + BorderWidth);

	TopRightGrowRect =  wxRect(WinRect.GetWidth() - BorderWidth - TitleBarHeight -1 , 
				 		 0,
				 		 WinRect.GetWidth(), 
				 		 TitleBarHeight  + BorderWidth);


	BottomLeftGrowRect = wxRect(0, 
				 		 WinRect.GetHeight() - BorderWidth - TitleBarHeight ,
				 		 TitleBarHeight  + BorderWidth +1 , 
				 		 WinRect.GetHeight());

	BottomRightGrowRect = wxRect(WinRect.GetWidth() - BorderWidth - TitleBarHeight -1  , 
				 		 WinRect.GetHeight() - BorderWidth - TitleBarHeight,
				 		 WinRect.GetWidth(), 
				 		 WinRect.GetHeight());


	// Get a context onto the entire window area
	wxClientDC	dc( this );

	
	// -------------------------------------------------------------------------------------
/*	// Paint the borders	 UIC commented out 

	// Create the pen.
	wxPen BorderPen(PS_NULL, 0, GetSysColor(COLOR_WINDOWFRAME));

	// Get standard border brush
	wxBrush BorderBrush(GetSysColor(COLOR_BTNFACE));
	
	// Select these into the DC
	wxBrush *OldBrush = dc->SelectObject(&BorderBrush);
	wxPen *OldPen = dc->SelectObject(&BorderPen);
	wxFont * OldFont=dc->SelectObject(TitleFont);
	

	// -------------------------------------------------------------------------------------
	// Paint the borders frame  */


	// Get standard hollow brush
	wxBrush FrameBrush( wxSystemSettings::GetColour( wxSYS_COLOUR_BTNFACE ) );

	dc.SetBrush( FrameBrush );
	dc.DrawRectangle( TopBorderRect);
	dc.DrawRectangle( BottomBorderRect);
	dc.DrawRectangle( LeftBorderRect);
	dc.DrawRectangle( RightBorderRect);
	dc.SetBrush( wxNullBrush );

	wxColour TopLeft		= wxSystemSettings::GetColour( wxSYS_COLOUR_BTNHIGHLIGHT );
	wxColour BotRight		= wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOWFRAME );
	wxColour TopLeftInside	= wxSystemSettings::GetColour( wxSYS_COLOUR_BTNFACE );
	wxColour BotRightInside	= wxSystemSettings::GetColour( wxSYS_COLOUR_BTNSHADOW );
	
	INT32 x  = 1;
	INT32 y  = 1;
	INT32 dx = WinRect.GetWidth()-1;
	INT32 dy = WinRect.GetHeight()-1;

	// The plinth is always made up of 4 right-angle elements
	// When indented, the colours are swapped, but the elements retain position
	PatB(dc, x, y, 1, dy-1, TopLeft);
	PatB(dc, x, y, dx-1, 1, TopLeft);

	PatB(dc, x+dx-1, y-1, 1, dy+1, BotRight);
	PatB(dc, x-1, y+dy-1, dx+2, 1, BotRight);

	PatB(dc, x+1, y+1, 1, dy-2, TopLeftInside);
	PatB(dc, x+1, y+1, dx-2, 1, TopLeftInside);

	PatB(dc, x+dx-2, y, 1, dy-1, BotRightInside);
	PatB(dc, x, y+dy-2, dx-2, 1, BotRightInside);

	


	// -------------------------------------------------------------------------------------
/*	// Paint the caption bar
	
	INT32 CaptionColour = IsActive ? COLOR_ACTIVECAPTION: COLOR_INACTIVECAPTION;

	wxBrush CaptionBrush(GetSysColor(CaptionColour));
	
	dc->FillRect(&CaptionBarRect,&CaptionBrush);
*/
	// Make sure weve got diagonal resizing for the galleries  
	if( pOp->IsKindOf(CC_RUNTIME_CLASS(SuperGallery)) )
	{	
		//remove corner grow rects from  border rects
		TopBorderRect.Inflate(-TopLeftGrowRect.GetRight(),0);
		
		LeftBorderRect.Inflate(0,-TopLeftGrowRect.GetRight());

		BottomBorderRect.Inflate(-TopLeftGrowRect.GetRight(),0);
		
		RightBorderRect.Inflate(0,-TopLeftGrowRect.GetRight());
	}

/*	// -------------------------------------------------------------------------------------
	// Paint the Close Button		UIC commnented out next 2 functions

   	Paint3dPlinth(dc,&SysMenuRect,TRUE);
	// Get standard Black Pen
	wxPen BlackPen;
	BlackPen.CreateStockObject(BLACK_PEN);
	dc->SelectObject(&BlackPen);
	
	dc->MoveTo(SysMenuRect.BottomRight()-(wxSize(5,4)));
	dc->LineTo(SysMenuRect.TopLeft()+(wxSize(2,2)));

	dc->MoveTo(SysMenuRect.BottomRight()-(wxSize(6,4)));
	dc->LineTo(SysMenuRect.TopLeft()+(wxSize(1,2)));

	dc->MoveTo(SysMenuRect.BottomRight()-(wxSize(10,4)));
	dc->LineTo(SysMenuRect.TopLeft()+(wxSize(7,2)));
	dc->MoveTo(SysMenuRect.BottomRight()-(wxSize(9,4)));
	dc->LineTo(SysMenuRect.TopLeft()+(wxSize(8,2)));
 
	//	------------------------------------------------------------------------------------
	// Print the Title

	INT32 TextColour = IsActive ? COLOR_CAPTIONTEXT: COLOR_INACTIVECAPTIONTEXT;  

	// Text Bounding Box
	wxRect BoundBox(BorderWidth + 1,
				   BorderWidth - 1,
				   WinRect.GetWidth() - (BorderWidth*2) + 1 - TitleBarHeight,
				   BorderWidth + TitleBarHeight);

	// Get the Dialog Name
	String_32 Name = pOp->GetName();

	// Set the Text Colour
	dc->SetTextColor(GetSysColor(TextColour));
	dc->SetBkMode(TRANSPARENT);

	// Check that the text will not overlap to the left
	wxSize TextSize=dc->GetTextExtent(Name,camStrlen(Name));

	dc->DrawText(Name,camStrlen(Name),BoundBox,DT_LEFT);

	// Clean up and release the DC

	dc->SelectObject(OldBrush);
	dc->SelectObject(OldPen);
	dc->SelectObject(OldFont);
	ReleaseDC(dc);

}

*/
// Paint the caption	UIC 
	
	// make sure that DrawCaption shows the correct title
	SetLabel( pOp->GetName() );
	
#pragma message( __LOCMSG__ "Remove MSW frame redrawing stuff" )
/*	UINT32 DrawFlags = DC_TEXT|DC_SMALLCAP;
	
	if(IsActive)
		DrawFlags|=DC_ACTIVE;

	// draw the caption
	::DrawCaption( this, dc, &CaptionBarRect, DrawFlags );
	
	// draw the new stretchy close button
	dc->DrawFrameControl( &SysMenuRect, DFC_CAPTION, DFCS_CAPTIONCLOSE ); */
}	
	

/********************************************************************************************

>	void BaseBar::PaintDialogNonClient(BOOL Active)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/4/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	This routine gets called by OnNcPaint to paint the non client area of a 
				floating bar. 
	Errors:		-
	SeeAlso:	BaseBar::OnNcPaint

********************************************************************************************/


void BaseBar::PaintDialogNonClient(BOOL IsActive)
{
#pragma message( __LOCMSG__ "BaseBar::PaintDialogNonClient - do nothing" )
	TRACE( _T("Warning - BaseBar::PaintDialogNonClient called") );
/*	// Get the System Frame Size
	BorderWidth=wxSystemSettings::GetMetric(SM_CXFRAME);

	// When the BaseBar is in DIALOG mode we have to paint the fancy non-client area. 

	// Get the size of the window	
	wxRect WinRect( GetRect() );

	// Set up the active non-client regions 
	SysMenuRect 	 = wxRect(BorderWidth-1, 
			  	         BorderWidth-1, 
			  		     BorderWidth + TitleBarHeight+1, 
			  		     BorderWidth-1 + TitleBarHeight + 1);
				  			 
	TopBorderRect	 = wxRect(0,
				    	 0, 
				 		 WinRect.GetWidth(), 
				 		 BorderWidth); 

	BottomBorderRect = wxRect(0,
				 		 WinRect.GetHeight() - BorderWidth, 
				 		 WinRect.GetWidth(), 
				 		 WinRect.GetHeight()); 

	LeftBorderRect =   wxRect(0,
				   		 0, 
				 		 BorderWidth,
				 		 WinRect.GetHeight());
				 		 
	RightBorderRect =  wxRect(WinRect.GetWidth() - BorderWidth, 
				 		 0,
				 		 WinRect.GetWidth(), 
				 		 WinRect.GetHeight());

	CaptionBarRect =   wxRect(BorderWidth - 1 + TitleBarHeight+1, 
				 		 BorderWidth - 1, 
				 		 WinRect.GetWidth() - BorderWidth + 1, 
				 		 BorderWidth + TitleBarHeight);  

	TopLeftGrowRect =  wxRect(0, 
				 		 0,
				 		 TitleBarHeight  + BorderWidth +1, 
				 		 TitleBarHeight  + BorderWidth);

	TopRightGrowRect =  wxRect(WinRect.GetWidth() - BorderWidth - TitleBarHeight -1 , 
				 		 0,
				 		 WinRect.GetWidth(), 
				 		 TitleBarHeight  + BorderWidth);


	BottomLeftGrowRect = wxRect(0, 
				 		 WinRect.GetHeight() - BorderWidth - TitleBarHeight ,
				 		 TitleBarHeight  + BorderWidth +1 , 
				 		 WinRect.GetHeight());

	BottomRightGrowRect = wxRect(WinRect.GetWidth() - BorderWidth - TitleBarHeight -1  , 
				 		 WinRect.GetHeight() - BorderWidth - TitleBarHeight,
				 		 WinRect.GetWidth(), 
				 		 WinRect.GetHeight());


	// Get a context onto the entire window area
	wxClientDC dc( this );

	// -------------------------------------------------------------------------------------
	// Paint the borders

	// Create the pen.
	wxPen BorderPen(PS_NULL, 0, GetSysColor(COLOR_WINDOWFRAME));

	// Get standard border brush
	INT32 BorderColour = IsActive ? COLOR_ACTIVEBORDER: COLOR_INACTIVEBORDER; 
	wxBrush BorderBrush(GetSysColor(BorderColour));

	// Select these into the DC
	dc.SetBrush( BorderBrush );
	dc.SetPen( BorderPen );
	dc.SetFont( TitleFont );

	// Top
	dc.DrawRectangle( TopBorderRect ); 
	// Bottom
	dc.DrawRectangle( BottomBorderRect ); 
	// Left
	dc.DrawRectangle( LeftBorderRect ); 
	// Right
	dc.DrawRectangle( RightBorderRect ); 


	// -------------------------------------------------------------------------------------
	// Paint the borders frame

	wxPen FramePen( GetSysColor(COLOR_WINDOWFRAME), wxSolid );

	// Get standard hollow brush
	wxBrush FrameBrush;
	FrameBrush.CreateStockObject(HOLLOW_BRUSH);

	// Select these into the DC
	dc.SetBrush( FrameBrush );
	dc.SetPen( FramePen );

	dc.DrawRectangle(0, 0, WinRect.GetWidth(), WinRect.GetHeight());

	dc.DrawRectangle(BorderWidth-1, 
				  BorderWidth-1, 
				  WinRect.GetWidth() - BorderWidth + 1, 
				  WinRect.GetHeight() - BorderWidth + 1);

	// Galleries have corner grow controls
	if( pOp->IsKindOf(CC_RUNTIME_CLASS(SuperGallery)) )
	{	
		dc.DrawRectangle(TopLeftGrowRect);
		dc.DrawRectangle(TopRightGrowRect);
	  		
		// Bottom Left Grow 
		dc.DrawLine( BottomLeftGrowRect.GetLeft(),BottomLeftGrowRect.GetTop(),
			BorderWidth -1 ,BottomLeftGrowRect.GetTop() );
		dc.DrawLine( BottomLeftGrowRect.GetRight(),BottomLeftGrowRect.GetBottom(),
			BottomLeftGrowRect.GetRight(),BottomLeftGrowRect.GetBottom() - BorderWidth - 1 );
	
		// Bottom Right Grow
		dc.DrawLine(BottomRightGrowRect.GetRight() - BorderWidth +1,BottomRightGrowRect.GetTop(),
			BottomRightGrowRect.GetRight(),BottomRightGrowRect.GetTop());
		dc.DrawLine(BottomRightGrowRect.GetLeft(),BottomRightGrowRect.GetBottom(),
			BottomRightGrowRect.GetLeft(),BottomLeftGrowRect.GetBottom() - BorderWidth - 1);
		
		//remove corner grow rects from  border rects
		TopBorderRect.Inflate(-TopLeftGrowRect.GetRight(),0);
		
		LeftBorderRect.Inflate(0,-TopLeftGrowRect.GetRight());

		BottomBorderRect.Inflate(-TopLeftGrowRect.GetRight(),0);
		
		RightBorderRect.Inflate(0,-TopLeftGrowRect.GetRight());

	}

	// -------------------------------------------------------------------------------------
	// Paint the Close Button 

	wxBrush ButtonBrush(GetSysColor(COLOR_BTNFACE));
	dc.SetBrush( ButtonBrush );
	dc.DrawRectangle(SysMenuRect);

	// Paint the Shadow
	wxBrush ShadowBrush(GetSysColor(COLOR_BTNSHADOW));
	dc.SetPen(GetStockObject(NULL_PEN));
	dc.SetBrush(&ShadowBrush);
	dc.DrawRectangle(BorderWidth - 1 + (TitleBarHeight/2)-1,
				  BorderWidth + 4,
				  BorderWidth - 1 + (TitleBarHeight/2)+6, 
				  BorderWidth + 8 );

	// Paint the Rectangle/Highlight
	dc.SetPen(GetStockObject(BLACK_PEN));
	dc.SetBrush(GetStockObject(WHITE_BRUSH));
	dc.DrawRectangle(BorderWidth - 1 + (TitleBarHeight/2)-2,
				  BorderWidth + 3,
				  BorderWidth - 1 + (TitleBarHeight/2)+4, 
				  BorderWidth + 6 );


	// -------------------------------------------------------------------------------------
	// Paint the caption bar
	
	INT32 CaptionColour = IsActive ? COLOR_ACTIVECAPTION: COLOR_INACTIVECAPTION;

	wxBrush CaptionBrush(GetSysColor(CaptionColour));
	
	dc.SetBrush(CaptionBrush);
	dc.DrawRectangle(CaptionBarRect);

	//	------------------------------------------------------------------------------------
	// Print the Title

	INT32 TextColour = IsActive ? COLOR_CAPTIONTEXT: COLOR_INACTIVECAPTIONTEXT;  

	// Text Bounding Box
	wxRect BoundBox(BorderWidth +TitleBarHeight,
				   BorderWidth-1,
				   WinRect.GetWidth() - (BorderWidth*2) + 1,
				   BorderWidth + TitleBarHeight);

	// Get the Dialog Name
	String_32 Name = pOp->GetName();

	// Set the Text Colour
	dc.SetTextForeground(GetSysColor(TextColour));
	dc.SetBackgroundMode( wxTRANSPARENT );

	// Check that the text will not overlap to the left
	wxSize TextSize=dc->GetTextExtent(Name,camStrlen(Name));

	if (TextSize.width<BoundBox.GetWidth())
		  dc->DrawText(Name,-1,BoundBox,DT_CENTER|DT_VCENTER|DT_SINGLELINE);  // UIC	
		//dc->DrawText(Name,camStrlen(Name),BoundBox,DT_CENTER); 
	else
		dc->DrawText(Name,camStrlen(Name),BoundBox,DT_LEFT|DT_VCENTER |DT_SINGLELINE); // UIC
		//dc->DrawText(Name,camStrlen(Name),BoundBox,DT_LEFT);

	// Clean up and release the DC

	dc->SelectObject(OldBrush);
	dc->SelectObject(OldPen);
	dc->SelectObject(OldFont); */

}



/********************************************************************************************

>	BOOL BaseBar::OnNcActivate(BOOL Active)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/9/93
	Inputs:		Active - Indicates whether the window is active (TRUE => Active).
	Outputs:	-
	Returns:	Nonzero to continue default processing, 0 to prevent the window being made
				inactive.
	Purpose:	This function is called when the non-client area needs to be updated to
				reflect its active or inactive state.
	Errors:		-
	SeeAlso:	-
	Scope:		Protected

********************************************************************************************/

#pragma message( __LOCMSG__ "BaseBar::OnNcPaint removed" )
/*BOOL BaseBar::OnNcActivate(BOOL IsActive)
{
	
	//PaintDialogNonClient(IsActive); 
	return TRUE;
} */



/********************************************************************************************

>	void BaseBar::OnPaint()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/9/93
	Purpose:	Paints the client area of the dilog bar
	Scope:		Protected

********************************************************************************************/
										 
#pragma message( __LOCMSG__ "BaseBar::OnPaint removed" )
/*void BaseBar::OnPaint()
{
	wxPaintDC  PaintDC(this); // device context for painting
		// Create the brush and pen to paint the client area
	wxBrush Brush(GetSysColor(COLOR_BTNFACE));

	// Get the size of the window client area	
	wxRect WinRect;
	GetClientRect(&WinRect);	

	// Paint the client area
	PaintDC.FillRect(WinRect,&Brush);
	
//	if (IS_CHICAGO && CurrentDlgBarMode != DIALOG)
//		PaintChicagoBorder();

//	wxWindow::OnPaint();
} */


/********************************************************************************************

>	void BaseBar::PaintChicagoBorder()

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/4/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	This routine gets called by OnNcPaint to paint the non client area of a 
				docked Chicago bar. 
	Errors:		-
	SeeAlso:	BaseBar::OnNcPaint

********************************************************************************************/

void BaseBar::PaintChicagoBorder()
{
#pragma message( __LOCMSG__ "BaseBar::PaintChicagoBorder - do nothing" )
	TRACE( _T("Warning - BaseBar::PaintChicagoBorder called") );
/*	wxRect WinRect( GetClientRect() );
	wxRect ParWin( GetParent()->GetRect() );
	wxRect ScreenWin( GetRect() );
	
	DWORD TopCol = GetSysColor(COLOR_BTNHIGHLIGHT);
	DWORD BottomCol = GetSysColor(COLOR_BTNSHADOW);
	DWORD LeftCol = GetSysColor(COLOR_BTNHIGHLIGHT);
	DWORD RightCol = GetSysColor(COLOR_BTNSHADOW);

	// Get a context onto the entire window area
	wxDC *DC = GetWindowDC();
	WinRect.Inflate(1,1);	
	if(!IS_CHICAGO)
	{
		switch(ParentDockBarType)
		{
	   	case DOCKBAR_LEFT:

			break;
		case DOCKBAR_RIGHT:
    	   if(ScreenWin.GetRight()==ParWin.GetRight())
				RightCol = GetSysColor(COLOR_WINDOWFRAME);
			break;
    	case DOCKBAR_TOP:
			if(ScreenWin.GetBottom()==ParWin.GetBottom())
				BottomCol = GetSysColor(COLOR_WINDOWFRAME);
			break;
		case DOCKBAR_BOTTOM:
			
			break;
		}
	}

	PatB(DC,0,0,1,WinRect.GetBottom(),LeftCol);
	
	PatB(DC,0,0,WinRect.GetRight(),1,TopCol);
	
	PatB(DC,WinRect.GetRight(),0,1,WinRect.GetBottom(),RightCol);
	
	PatB(DC,0,WinRect.GetBottom() ,WinRect.GetRight()+1,1,BottomCol); */

}


/********************************************************************************************

>	UINT32 BaseBar::OnNcHitTest(wxPoint point)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/4/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

#pragma message( __LOCMSG__ "BaseBar::OnNcHitTest removed" )
/*UINT32 BaseBar::OnNcHitTest(wxPoint point)
{

	if (DragType != NONE)
		return HTCLIENT; 

	if (CurrentDlgBarMode == DIALOG)
	{
		// Get the window dimensions
		wxRect rect( GetRect() );
		wxRect InsideBorderRect = rect;
		InsideBorderRect.Inflate(-BorderWidth,-BorderWidth);
		wxPoint NonClientPoint (point.x - rect.GetLeft(), point.y - rect.GetTop()); 
  
		// Determine what active area the cursor is over
		if (SysMenuRect.PtInRect(NonClientPoint))
		{
			return HTSYSMENU; 
		}
		else if (TopBorderRect.PtInRect(NonClientPoint))
		{
			if (pOp->IsListEmpty())
				return HTCLIENT;	
			return HTTOP; 
		}
		else if (BottomBorderRect.PtInRect(NonClientPoint))
		{
			if (pOp->IsListEmpty())
				return HTCLIENT;	
			return HTBOTTOM; 
		}
		else if (LeftBorderRect.PtInRect(NonClientPoint))
		{
			if (pOp->IsListEmpty())
				return HTCLIENT;	
			return HTLEFT; 
		}
		else if (RightBorderRect.PtInRect(NonClientPoint))
		{
			if (pOp->IsListEmpty())
				return HTCLIENT;	
			return HTRIGHT; 
		}
		else if (CaptionBarRect.PtInRect(NonClientPoint))
		{
			return HTCAPTION; 
		}
		if( pOp->IsKindOf(CC_RUNTIME_CLASS(SuperGallery)) && !InsideBorderRect.PtInRect(point) )
		{
			if (TopLeftGrowRect.PtInRect(NonClientPoint))
				return HTTOPLEFT;							  
			
			else if (TopRightGrowRect.PtInRect(NonClientPoint))
			   	return HTTOPRIGHT; 
			
			else if (BottomLeftGrowRect.PtInRect(NonClientPoint))
				return HTBOTTOMLEFT;	 
			
			else if (BottomRightGrowRect.PtInRect(NonClientPoint))
				return HTBOTTOMRIGHT;	 
			
			return HTCLIENT; 
		
		}
		else
			// Normal client area hit 
			return HTCLIENT; 
	}
	else // The BaseBar is a bar
	{
		return (wxWindow::OnNcHitTest(point)); 
	} 
	
} */

/********************************************************************************************

>	void BaseBar::OnNcRButtonDown(UINT32 nHitTest, wxPoint point)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/4/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

#pragma message( __LOCMSG__ "BaseBar::OnNcRButtonDown removed" )
/*void BaseBar::OnNcRButtonDown(UINT32 nHitTest, wxPoint point)
{
} */

/********************************************************************************************

>	void BaseBar::OnNcLButtonDown(UINT32 nHitTest, wxPoint point)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/4/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

#pragma message( __LOCMSG__ "BaseBar::OnNcHitTest removed" )
/*void BaseBar::OnNcHitTest(UINT32 nHitTest, wxPoint point)
{

	ENSURE(FALSE,"BaseClass OnNcLButtonDown called");

} */

/********************************************************************************************

>	void BaseBar::OnLButtonDown(UINT32 nFlags, wxPoint point)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/4/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

#pragma message( __LOCMSG__ "BaseBar::OnLButtonDown removed" )
/*void BaseBar::OnLButtonDown(UINT32 nFlags, wxPoint point)
{
	// make sure we're not clicking on a disabled button
	if(this == ChildWindowFromPoint(point))
	{
		//Capture the mouse and go into "trying to start a drag" mode 
		SetCapture();
		DragType = TRYING;

		DragManagerOp::SetDragActive(TRUE);

		//record start point of drag
		ClientToScreen(&point);
		StartDragPoint = point;
	
		GetWindowRect(&OldDragRect);
		PaintDragRect(OldDragRect,wxRect(0,0,0,0),ParentDockBarType,ParentDockBarType); 
	
	}
	else
	{
	 	if(KeyPress::IsAlternativePressed())
		{
			wxWindow * OverWindow;
			OverWindow = ChildWindowFromPoint(point);
			
			if (OverWindow != this && OverWindow != NULL)
			{
				::SendMessage(GetSafeHwnd(),WM_START_CTLDRAG,(WPARAM) 0,
			  						(LPARAM)OverWindow->GetSafeHwnd() );
			}
		}

	}

} */



/********************************************************************************************

>	void BaseBar::OnMouseWheel(UINT32 nFlags, short zDelta, wxPoint point)
											   
	Author:		Priestley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/10/2000

********************************************************************************************/

#pragma message( __LOCMSG__ "BaseBar::OnMouseWheel removed" )
/*BOOL BaseBar::OnMouseWheel(UINT32 nFlags, short zDelta, wxPoint pt)
{
	TRACEUSER( "Matt", _T("MouseWheel Scroll In BaseBar!\n"));
	return TRUE;
} */

/********************************************************************************************

>	void BaseBar::OnLButtonDblClk(UINT32 nFlags, wxPoint point)
											   
	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/5/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

#pragma message( __LOCMSG__ "BaseBar::OnLButtonDblClk removed" )
/*void BaseBar::OnLButtonDblClk( UINT32 nFlags, wxPoint point )
{
	//MessageBeep(MB_OK);
} */

/********************************************************************************************

>	static BOOL BaseBar::GetStatusLineText(String_256 * StatusText)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/11/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if I have put text on the StatusLine FALSE otherwise
	Purpose:	called on idles to check whether I want to put some text on the status bar	
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL BaseBar::GetStatusLineText(String_256 * pStatusText)
{
	ERROR2IF(pStatusText==NULL,FALSE,"BaseBar::GetStatusLineText() - pStatusText==NULL!");

	BOOL ReturnValue=FALSE;
	if (CurrentBar!=NULL)
		ReturnValue=CurrentBar->GetStatusText(pStatusText);
	return ReturnValue;
}

/********************************************************************************************

>	virtual BOOL BaseBar::SetStatusLineText()

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/11/94
	Inputs:		-
	Outputs:	-
	Returns:	FALSE 
	Purpose:	base virtual function should never be called
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL BaseBar::SetStatusLineText()
{
	return FALSE;
}


/********************************************************************************************

>	virtual BOOL BaseBar::GetStatusText(String_256 * StatusText)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/11/94
	Inputs:		-
	Outputs:	-
	Returns:	FALSE 
	Purpose:	base virtual function should never be called.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL BaseBar::GetStatusText(String_256 * StatusText)
{
	return FALSE;
}

/********************************************************************************************

>	static void BaseBar::AltPressed(BOOL state)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/11/94
	Inputs:		Alt state
	Outputs:	-
	Returns:	-
	Purpose:	called from DialogBarOp message handler when Alt is pressed
				allows user to enter control drag state
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL BaseBar::AltPressed(BOOL state)
{
	//TRACEUSER( "chris", _T("Atl state %d\n"),state);

	if(state)
	{
		if(CtlDragState==NO_DRAG)
 		{
 			//CtlDragState=READY;
			return FALSE;
		}
	}
	else
	{
		if(CtlDragState==DRAGGING || CtlDragState == READY)
 		{
			return TRUE;
		}
	}
	return FALSE;
}

/********************************************************************************************

>	static void BaseBar::CancelAllDrags()

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/11/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	called from DialogBarOp message handler when Esc is pressed
				will end any bar type drag
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
BOOL BaseBar::CancelAllDrags()
{
	//TRACEUSER( "chris", _T("Esc state %d\n"),state);
	if(CurrentBar == NULL)
		return FALSE;
	if(CtlDragState==DRAGGING)
	{	
		CurrentBar->PaintDragRect(CurrentBar->OldDragRect,wxRect(0,0,0,0),DOCKBAR_FLOAT,DOCKBAR_FLOAT);
	   	CurrentBar->TidyUpControlDrag();
	}
	else if(DragType != NONE)
	{
		CurrentBar->FormatLineFeeds(CurrentBar->DragStartRect.GetWidth());
		CurrentBar->InitFormatRects();
		CurrentBar->TidyUpBarDrag();
	}

	CtlDragState = NO_DRAG;
	DragType = NONE;
	return FALSE;
}

/********************************************************************************************

>	static void BaseBar::EscPressed(BOOL state)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/11/94
	Inputs:		Esc State
	Outputs:	-
	Returns:	-
	Purpose:	called from DialogBarOp message handler when Esc is pressed
				will end any bar type drag
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
BOOL BaseBar::EscPressed(BOOL state)
{
	//TRACEUSER( "chris", _T("Esc state %d\n"),state);
	if(CurrentBar == NULL)
		return FALSE;
	if(CtlDragState==DRAGGING)
	{	

		CurrentBar->PaintDragRect(CurrentBar->OldDragRect,wxRect(0,0,0,0),DOCKBAR_FLOAT,DOCKBAR_FLOAT);
		CurrentBar->TidyUpControlDrag();
	
		return TRUE;
	}
	else if(DragType != NONE)
	{
		CurrentBar->FormatLineFeeds(CurrentBar->DragStartRect.GetWidth());
		CurrentBar->InitFormatRects();
		CurrentBar->TidyUpBarDrag();
		return TRUE;
	}
	return FALSE;
}

 
/********************************************************************************************

>	static void BaseBar::CtlPressed(BOOL state)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/11/94
	Inputs:		Clt State
	Outputs:	-
	Returns:	-
	Purpose:	called from DialogBarOp message handler when Clt is pressed
				toggles between cut and copy drag control modes
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
BOOL BaseBar::CtlPressed(BOOL state)
{
	//TRACEUSER( "chris", _T("Ctl state %d\n"),state);
	
	if(CtlDragState == NO_DRAG)
		return FALSE;
 	if(state)
	{
		if(CtlDragMode==CUT)
 		{
 			// check we have a cursor to swap !
 			if(pCurrentCursor)
				SwapCursors( _R(IDC_BUTTONDRAGCOPY) );
 			CtlDragMode=COPY;
		}
	}
	else
	{
		if(CtlDragMode==COPY)
 		{
			if(pCurrentCursor)
 				SwapCursors( _R(IDC_BUTTONDRAGCUT) );
	  		CtlDragMode=CUT;
		}
	}
 	return FALSE;
 }

 

/********************************************************************************************

>	BOOL BaseBar::HasOrientationChanged(DockBarType Old,DockBarType New);
											   
	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/5/94
	Inputs:		Old Dock
				New Dock
	Outputs:	-
	Returns:	-
	Purpose:	determine whether two docks have different orientation.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL BaseBar::HasOrientationChanged(DockBarType Old,DockBarType New)
{
	if(Old == New)
		return FALSE ;
	if(  (Old== DOCKBAR_RIGHT ||Old == DOCKBAR_LEFT) &&
		 (New== DOCKBAR_RIGHT ||New == DOCKBAR_LEFT))
		return FALSE;
	if(  (Old== DOCKBAR_TOP ||Old == DOCKBAR_BOTTOM||Old == DOCKBAR_FLOAT) &&
		 (New== DOCKBAR_TOP ||New == DOCKBAR_BOTTOM||New == DOCKBAR_FLOAT))
		return FALSE;	 	
	return TRUE;
}

/********************************************************************************************

>	BOOL BaseBar::TidyUpControlDrag()

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/11/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	called to tidy up at the end of a control drag 
	Errors:		-
	
	SeeAlso:	-

********************************************************************************************/
BOOL BaseBar::TidyUpControlDrag()
{
#pragma message( __LOCMSG__ "BaseBar::TidyUpControlDrag - do nothing" )
	TRACE( _T("Warning - BaseBar::TidyUpControlDrag called") );
/*	CtlDragState = NO_DRAG;
	DragManagerOp::SetDragActive(FALSE);

	// if alt is still held down we should be ready to drag
	if(KeyPress::IsAlternativePressed())
		CtlDragState = READY;
	
	OldDragRect.SetRectEmpty();
	// release mouse
	ReleaseCapture();			   	

	InRecreate = FALSE;

	GetMainFrame()->SetActiveWindow();

	CurrentBar = NULL; 
	
	ERROR2IF(pCurrentCursor==NULL,FALSE,"Null last Cursor in TidyUpControlDrag");		
	
	if(pCurrentCursor!= NULL)
	{
	 	CursorStack::GPop(CurrentCursorID);
		delete pCurrentCursor;
		pCurrentCursor = NULL;
		CurrentCursorID = 0;
	} */

	return TRUE;
}

/********************************************************************************************

>	BOOL BaseBar::TidyUpBarDrag()

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/11/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	called to tidy up at the end of a Bar drag 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
BOOL BaseBar::TidyUpBarDrag()
{
	PaintDragRect(OldDragRect,wxRect(0,0,0,0),OldDragBarType,DOCKBAR_FLOAT);
	InRecreate = TRUE;
	ReleaseMouse();
	DragType = NONE;
	InRecreate = FALSE;
	CurrentBar = NULL;

#pragma message( __LOCMSG__ "Remove DragManagerOp::SetDragActive usage" )
//	DragManagerOp::SetDragActive(FALSE); 
	return TRUE;
}

/********************************************************************************************

>	BOOL BaseBar::DropControl(wxPoint point,HWND OverBar, BOOL InsertSep)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/11/94
	Inputs:		point Client coord at drop point
				OverBar HWND of bar pointer is over
				InsertSep Flags whether this control has an attached Separator
	Outputs:	-
	Returns:	BOOL - TRUE if we should delete the leading separator
	Purpose:	Drops a dragged control onto a given Window
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
BOOL BaseBar::DropControl(wxPoint point, wxWindow* OverBar,BOOL InsertSeparator)
{
#pragma message( __LOCMSG__ "BaseBar::DropControl - do nothing" )
	TRACE( _T("Warning - BaseBar::DropControl called") );
/*	BOOL DeleteSeparator = FALSE;

	// get a wxWindow from this HWND
	BaseBar * DropBar = (BaseBar*)FromHandlePermanent(OverBar);
	DialogBarOp * DropOp;
	
	//Get the DialogOp for this HWND
	DropOp = DialogBarOp::FindDialogBarOp((UINT32)OverBar );
	if(DropOp)
	{
		// make a copy of the control we are dragging 
		BarControl * foundControl = (BarControl*)pOp->GetBarItem(DragCtlHwnd);
		BarControl * newControl = new BarControl(*foundControl);
		UINT32 NewID;
		
		ERROR2IF(foundControl == NULL,FALSE,"Can't find Bar Item");
		ERROR2IF(newControl == NULL,FALSE,"Can't create new Bar Control");
		
		// check orientation of the bar we are dropping on
		BOOL Horizontal = DropOp->IsHorizontal();
		
		// give the control a unique id in this bar
		newControl->SetUniqueGadgetID(NewID=DropOp->GetUniqueID());

		INT32 ControlIndex;
		wxPoint ClientPos = point;
		DropBar->ScreenToClient(&ClientPos);
		
		// find position for new control in bar
		ControlIndex = DropBar->GetNearestControl(ClientPos);
		
		// ****************************************************
		// Insertion / Deletion  of Separators
		
		//have we dropped this control back onto itself ?
		// if so should we 
			//	a) insert a new separator 
			//  b) delete an existing one

		
		// get the window rect of the control we are dragging
		// and convert to Client coords	
		wxRect DragWnd;
		::GetWindowRect(DragCtlHwnd,&DragWnd);		   		
		ScreenToClient(&DragWnd);		
		
		// check the last item
		BarItem* pLastItem = NULL ;
		if((ControlIndex-1)>=0)
			pLastItem = DropOp->GetPtrBarItem(ControlIndex-1);
		
		// if the last item is a separator allow us to drop onto it 
		// by growing the hittest rectangle
		if(pLastItem)
		{
			if(pLastItem->IsKindOf(CC_RUNTIME_CLASS(BarSeparator)))
			{
			 	DragWnd.GetTop()  -= SeparatorWidth;
				DragWnd.GetLeft() -= SeparatorWidth; 
			}				
		}

		// is the mouse over the dragged control or it's preceeding separator
		if(DragWnd.PtInRect(ClientPos))
		{

			wxPoint StartDrag(DragCtlOffset);
			::ClientToScreen(DragCtlHwnd,&StartDrag);
			ScreenToClient(&StartDrag);

			INT32 CheckPos = ClientPos.x;
			INT32 StartPos = StartDrag.x;

			// if the control is at the fromt of a slot we want to use y movement
			// to determine insertion or deletion
			// this is the case if the prev item is a LF or the bar is vertical
			// or if the prev item is a SP and it's prev is a LF (!)
			if(pLastItem)
			{
				if(pLastItem->IsKindOf(CC_RUNTIME_CLASS(BarLineFeed))|| !Horizontal)
				{
				   	CheckPos = ClientPos.y;
					StartPos = StartDrag.y;
				}
				
				if(pLastItem->IsKindOf(CC_RUNTIME_CLASS(BarSeparator)))
				{
					BarItem* pPrevLastItem = NULL ;
					if((ControlIndex -2)>=0)
						pPrevLastItem = DropOp->GetPtrBarItem(ControlIndex-2);
					if(pPrevLastItem)
					{
						if(pPrevLastItem->IsKindOf(CC_RUNTIME_CLASS(BarLineFeed))||!Horizontal)
						{
				   			CheckPos = ClientPos.y;
							StartPos = StartDrag.y;
						}
					}
					
				//	StartPos -= SeparatorWidth;
				}
			}

			// which way have we moved ?
			if(CheckPos>StartPos)
				InsertSeparator = TRUE;
			else
				DeleteSeparator = TRUE;
		}
		
		
		if(InsertSeparator && ControlIndex>0)	  // don't insert a separator at the head of the bar
		{
			
		//	BarItem* LastItem = NULL ;
		//	LastItem = DropOp->GetPtrBarItem(ControlIndex-1);
			if(pLastItem!=NULL)
			{
				if(!pLastItem->IsKindOf(CC_RUNTIME_CLASS(BarSeparator)))
				{
					BarSeparator* newSep=NULL;
					newSep = new BarSeparator;
					if(newSep)
					{
						DropOp->InsertItemAfter(ControlIndex-1,newSep);
				
						//TRACEUSER( "chris", _T("Just added a separator at %d\n"),ControlNumber-1);
						ControlIndex ++;  
					}
				}
			}
		}
		
		// insert the item
		DropOp->InsertItemAfter(ControlIndex-1,(BarItem *)newControl);  

		// create the windows control
		DropBar->CreateControlFromResource(newControl);

		// send a create message to allow the control to init itself
		OpDescControlCreateMsg op(newControl->GetOpDescriptor(Horizontal), 
									newControl->GetBarControlInfo(Horizontal).ControlID, 
									NewID,
		 							DropOp); 
		
		newControl->GetOpDescriptor(Horizontal)->Message(&op);
		if(pOp!=DropOp)
		{
			// format and size the bar we have dropped on
			switch(DropBar->GetDockBarType())
			{
			case DOCKBAR_FLOAT:
			{
	           	DropBar->FormatFloatingBar(TRUE,USE_BAR);
			}
			break;
	       	case DOCKBAR_LEFT:
	  		case DOCKBAR_RIGHT:
	    		DropBar->FormatVerticalBar(TRUE,USE_BAR);
	    		break;
	    	case DOCKBAR_TOP:
			case DOCKBAR_BOTTOM:
				DropBar->FormatHorizontalBar(TRUE,USE_BAR);
				break;
			} 
			DropBar->SetBarSize(DropBar->GetDockBarType());
			DropBar->InvalidateRect(NULL,TRUE);
		}
	}	
	return DeleteSeparator; */

	return false;
}

/********************************************************************************************

>	BOOL BaseBar::EndControlDrag(wxPoint point)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/11/94
	Inputs:		point Client coordinate at end of drag
	Outputs:	-
	Returns:	TRUE if the bar has been deleted FALSE otherwise
	Purpose:	called at the end of a control drag 
				decides where to drop the dragged control... does it and tidies up the drag.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
BOOL BaseBar::EndControlDrag(wxPoint point)
{
#pragma message( __LOCMSG__ "BaseBar::EndControlDrag - do nothing" )
	TRACE( _T("Warning - BaseBar::EndControlDrag called") );
/*	// go to screen co-ords	- we may be dropping on a different bar
	wxPoint sPnt= point;
	ClientToScreen(&sPnt);
	BOOL MovingSeparator = FALSE;
	BOOL DeleteLineFeed = FALSE;
	
	BaseBar * DropBar=NULL;

	// check if the drag has finished over an existing bar
   	HWND OverBar = GetMainFrame()->IsScreenPointInBar(sPnt);

	PaintDragRect(OldDragRect,wxRect(0,0,0,0),DOCKBAR_FLOAT,DOCKBAR_FLOAT);
	
	CtlDragState = NO_DRAG;
	if(OverBar)
	{
		DropBar = (BaseBar*)FromHandlePermanent(OverBar);
		
		// can't drop into an info bar
		// but as the user might think we can... we'll just cancel 
		// the drag
		if(DropBar->IsInfoBar())
		{
		  	TidyUpControlDrag();
			// we haven't deleted the bar so return FALSE
			return FALSE;
		}
		
		// can't drop on the control bank
		if(DropBar->IsBarAllCtlBar())
			OverBar = NULL ;
		
		// we don't want to copy a control onto the same bar !
		if(DropBar==this && CtlDragMode == COPY)
		{
		  	TidyUpControlDrag();
			// we haven't deleted the bar so return FALSE
			return FALSE;
		}
	}
	
	ERROR2IF(pOp==NULL,FALSE,"Null BarOp");


	BarItem * LastItem = NULL;
	BarItem * thisItem = pOp->GetBarItem((CWindowID)DragCtlHwnd);
		
	 
	// Check if the control has an preceeding separator	
	LastItem = pOp->GetPtrBarPrev(thisItem);
	if ( LastItem )
		if ( LastItem->IsKindOf( CC_RUNTIME_CLASS(BarSeparator)))
			MovingSeparator = TRUE;
	

	// drop the control into the bar
	BOOL DeleteSeparator = FALSE;
	BOOL DoTidyOverlap=FALSE;
	
	if (OverBar)
		DeleteSeparator = DropControl(sPnt,OverBar,FALSE ); //MovingSeparator);

	// check for cut mode
	if (( CtlDragMode == CUT || DeleteSeparator == TRUE ) && !pOp->IsAllBarsOp() )
	{
		// delete the separator
		if (MovingSeparator && DeleteSeparator)
		{
			delete pOp->RemoveBarItem(LastItem);
		}
		
		// delete the item from the kernel list
	 	pOp->DeleteBarItem((CWindowID)DragCtlHwnd);
		
		// and destroy the window
		::DestroyWindow(DragCtlHwnd);
		
		// inform subclassing
		ControlHelper::NotifyControlDeleted(DragCtlHwnd);
		
	 	// remove leading, trailing and multiple linefeeds
	 	TidyBarItemsAfterDrag(); 


	
	 	// no controls left on the bar so delete it
	 	if(pOp->IsListEmpty())
		{
			TidyUpControlDrag();
			
			BROADCAST_TO_CLASS(DialogMsg(GetSafeHwnd(), DIM_CANCEL, NULL), DialogOp); 
		  	
			return TRUE;
		}
		// else reformat and invalidate
		else 
		{
		 	switch(GetDockBarType())
			{
				case DOCKBAR_FLOAT:
	           		FormatFloatingBar(TRUE,USE_BAR);
					break;
		    	case DOCKBAR_LEFT:
		  		case DOCKBAR_RIGHT:
		    		DoTidyOverlap=TRUE;
		    		FormatVerticalBar(TRUE,USE_BAR);
		    		break;
		    	case DOCKBAR_TOP:
				case DOCKBAR_BOTTOM:
					DoTidyOverlap=TRUE;
					FormatHorizontalBar(TRUE,USE_BAR);
					break;
			} 
			SetBarSize(GetDockBarType());
			InvalidateRect(NULL,TRUE);
		}
	}
	if(DropBar)	
	{
		OILDockingBar * ThisDock = GetMainFrame()->GetDockBar(DropBar->GetDockBarType() );
		switch(DropBar->GetDockBarType())
		{
	    	case DOCKBAR_LEFT:
	  		case DOCKBAR_RIGHT:
	    	case DOCKBAR_TOP:
			case DOCKBAR_BOTTOM:
			  	((OILFixedDockingBar*)ThisDock)->CheckAndCorrectOverlap();
				break;
		} 
	}
	// tidy up and out of here	
	TidyUpControlDrag(); */
	return FALSE;
}



/********************************************************************************************

>	void BaseBar::OnLButtonUp(UINT32 nFlags, wxPoint point)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/4/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
#pragma message( __LOCMSG__ "BaseBar::OnLButtonUp removed" )
/*void BaseBar::OnLButtonUp(UINT32 nFlags, wxPoint point)
{
	if(CtlDragState == DRAGGING)
	{
		if(EndControlDrag(point))
			return;			// this bar has been deleted we must not call the base handler
	}
	// We never started the drag - just tidy up
	else if (DragType == TRYING)		
	{
	 	OldDragBarType = ParentDockBarType;
	 	TidyUpBarDrag();
	}
	else if (DragType != NONE )
	{
		
		DragManagerOp::SetDragActive(FALSE);
		
		InRecreate = TRUE;
		// End of a drag
		PaintDragRect(OldDragRect,wxRect(0,0,0,0),OldDragBarType,OldDragBarType);
	
		
		// Release the mouse 
		ReleaseCapture();

	
		// Remove this BaseBar from its current docking bar 
		GetMainFrame()->GetDockBar(ParentDockBarType)->ReleaseBar(this->GetSafeHwnd(),DONT_TIDY); 

		// If Bar has changed parent Docking Bar
		if ( ParentDockBarType != DragDockBarType )
		{	
			//Check whether we have moved between docks with different control sizes
			//if we have Recreate() will have to get controls from resources
			OILDockingBar * NewDock ;
			OILDockingBar * OldDock ;
        	BOOL CanCopy;

        	NewDock = GetMainFrame()->GetDockBar(DragDockBarType);
			OldDock = GetMainFrame()->GetDockBar(ParentDockBarType);
			CanCopy = NewDock->IsBigControls() == OldDock->IsBigControls();		
			if(HasOrientationChanged(ParentDockBarType,DragDockBarType))
				CanCopy = FALSE;
			// dynamically create a new BaseBar/derived Bar
			BaseBar* NewBaseBar = (BaseBar *)GetRuntimeClass()->CreateObject(); 
			
			// Recreate the Dialog
			NewBaseBar->Recreate(DragDockBarType, this, pOp,CanCopy); 
		
			// The current BaseBar dies
			CCamApp::GetDlgManager()->Delete(GetSafeHwnd(),pOp);  
			//delete this;
		}
		else
		// just format and move the bar
		{
			
			ParentDockBarType = DragDockBarType;// Record the current dock bar

			// We are no longer dragging
			DragType = NONE; 
		
			// Reposition the controls and redraw the bar
			PositionControls(TRUE);

			wxRect TempRect = GetFormatRect(ParentDockBarType);

			// Attach the BaseBar to the docking bar and position the window 
			GetMainFrame()->GetDockBar(ParentDockBarType)->PlaceBar(GetSafeHwnd(),
										 wxPoint(OldDragRect.GetLeft(), OldDragRect.GetTop()), wxSize(TempRect.GetRight(), TempRect.GetBottom()));

			// CGS (23/1/2001)
			// the following code allows camelot to remember where galleries were
			// dragged to, the next time camelot starts.
			// NOTE:  I'm only doing this for SuperGallery's ....

			if (pOp)
			{
				if (pOp->IsKindOf (CC_RUNTIME_CLASS (SuperGallery)))
				{
					SuperGallery* pGallery = (SuperGallery*) pOp;
					pOp->SetFloatingwxPoint (wxPoint (OldDragRect.GetLeft(), OldDragRect.GetTop()));
				}
			}
		}
		InRecreate = FALSE;
		CurrentBar = NULL;
		return; 
	}

	wxWindow::OnLButtonUp(nFlags, point);
	
} */


/********************************************************************************************

>	void BaseBar::ConvertToolbarOnStartup ()

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/5/2000
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Converts the toolbar from being docked to being floating.
	Errors:		An ERROR3 goes off in debug if this function is called for anything BUT
				the toolbar.
	SeeAlso:	-

********************************************************************************************/

void BaseBar::ConvertToolbarOnStartup ()
{
#pragma message( __LOCMSG__ "BaseBar::ConvertToolbarOnStartup - do nothing" )
	TRACE( _T("Warning - BaseBar::ConvertToolbarOnStartup called") );
/*	ERROR3IF (pOp->GetName () != String_32 (TEXT ("Toolbar")), "BaseBar::ConvertToolbarOnStartup () - Call that a toolbar!");
	
	InRecreate = TRUE;

	// Remove this BaseBar from its current docking bar 
	GetMainFrame()->GetDockBar(ParentDockBarType)->ReleaseBar(this->GetSafeHwnd(),DONT_TIDY); 
	
	// dynamically create a new BaseBar/derived Bar
	BaseBar* NewBaseBar = (BaseBar *)GetClassInfo()->CreateObject();

	if (NewBaseBar)
	{		
		// Recreate the Dialog
		NewBaseBar->Recreate(DOCKBAR_FLOAT, this, pOp, FALSE);
		NewBaseBar->GetDialogBarOp ()->SetDockBarType (DOCKBAR_FLOAT);
			
		// The current BaseBar dies
		CCamApp::GetDlgManager()->Delete(GetSafeHwnd(),pOp);  
		//delete this;

		InRecreate = FALSE;
		CurrentBar = NULL;
		
		PtrToToolBar = NewBaseBar;
	} */
}



/********************************************************************************************

>	void BaseBar::MoveBar(WPARAM Dock, LPARAM Pos)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/10/94
	Inputs:		Dock - DockBarType
				Pos  - KernelBarPos
	Outputs:	-
	Returns:	-
	Purpose:	allows bars to be move between docks on a WM_MOVEBAR message
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

#pragma message( __LOCMSG__ "BaseBar::MoveBar removed" )
/*LRESULT BaseBar::MoveBar(WPARAM Dock, LPARAM Pos)
{	
	DockBarType NewDockType = (DockBarType)Dock;
	KernelBarPos  NewPos;
	NewPos.Slot   =((KernelBarPos * )Pos)->Slot; 
	NewPos.Position =((KernelBarPos * )Pos)->Position; 
	OILDockingBar * NewDock ;
	OILDockingBar * OldDock ;
	BOOL CanCopy;
	wxPoint DropPoint;

	// get a pointer to the new dock
	NewDock = GetMainFrame()->GetDockBar(NewDockType);

	// get a screen coord for the new position
	if(NewDockType!= DOCKBAR_FLOAT)
	{
		((OILFixedDockingBar * )NewDock)->BarPosToClient( NewPos,&DropPoint);
		((OILFixedDockingBar * )NewDock)->ClientToScreen(&DropPoint);
	}
	else
	{
		DropPoint.x = NewPos.Slot;
		DropPoint.y = NewPos.Position;
	}


	// this will be used in recreate to position the bar
	OldDragRect.GetLeft() = DropPoint.x;
	OldDragRect.GetTop() = DropPoint.y;

	// if the new dock has different sized controls
	// we won't be able to copy the bar in Recreate
	OldDock = GetMainFrame()->GetDockBar(ParentDockBarType);
	CanCopy = NewDock->IsBigControls() == OldDock->IsBigControls();		
	if(HasOrientationChanged(ParentDockBarType,DragDockBarType))
		CanCopy = FALSE;
	
	// dynamically create a new BaseBar/derived Bar
	BaseBar* NewBaseBar = (BaseBar *)GetRuntimeClass()->CreateObject(); 

	// Recreate the Dialog
	NewBaseBar->Recreate(NewDockType, this, pOp,CanCopy); 

	// The current BaseBar dies
	CCamApp::GetDlgManager()->Delete(GetSafeHwnd(),pOp);  
	return TRUE;
} */


/********************************************************************************************

>	void BaseBar::OnMouseMove(UINT32 Flags, wxPoint Point)
	 
	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/4/94		  
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
#pragma message( __LOCMSG__ "BaseBar::OnMouseMove removed" )
/*void BaseBar::OnMouseMove(UINT32 Flags, wxPoint Point)
{
	if(CtlDragState == DRAGGING)
	{
	
		// mouse to screen
		ClientToScreen(&Point);

		// control client window size
		wxRect ScreenRect= DragCtlClientRect;
		
		// offset by mouse pos
		ScreenRect.OffsetRect(Point);
		
		// account for initial offset in control
		ScreenRect.OffsetRect(-DragCtlOffset.x,-DragCtlOffset.y);
		
		
		PaintDragRect(ScreenRect,OldDragRect,DOCKBAR_FLOAT,DOCKBAR_FLOAT);
	
		// keep copy
		OldDragRect = ScreenRect;
	}
	// Route MouseMove back to disabled controls to allow status bar help
	else if (DragType == NONE)
	{
		wxWindow * OverWindow;
		OverWindow = ChildWindowFromPoint(Point);
		if (OverWindow != this && OverWindow != NULL)
		{
			ClientToScreen(&Point);
			::ScreenToClient(OverWindow->GetSafeHwnd(),&Point);
		  	::SendMessage(OverWindow->GetSafeHwnd(),WM_MOUSEMOVE,(WPARAM)Flags,
		  						MAKELPARAM((short)Point.y,(short)Point.x));
		}
	}
} */

/********************************************************************************************

>	void BaseBar::CacheAllDockRects()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/5/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Sets all check rectangles for drop hit testing at start of drag
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void BaseBar::CacheAllDockRects()
{
#pragma message( __LOCMSG__ "BaseBar::CacheAllDockRects - do nothing" )
	TRACE( _T("Warning - BaseBar::CacheAllDockRects called") );
/*	OILFixedDockingBar* DockBar;

	DockBar = (OILFixedDockingBar*)GetMainFrame()->GetDockBar(DOCKBAR_TOP);
	DockBar->GetDockScreenRect(&TopRect);

	DockBar = (OILFixedDockingBar*)GetMainFrame()->GetDockBar(DOCKBAR_BOTTOM);
	DockBar->GetDockScreenRect(&BottomRect);

	DockBar = (OILFixedDockingBar*)GetMainFrame()->GetDockBar(DOCKBAR_LEFT);
	DockBar->GetDockScreenRect(&LeftRect);

	DockBar = (OILFixedDockingBar*)GetMainFrame()->GetDockBar(DOCKBAR_RIGHT);
	DockBar->GetDockScreenRect(&RightRect); */
}

/********************************************************************************************

>	DockBarType BaseBar::BarCloseToDock(wxPoint CursorPos)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/5/94
	Inputs:		CursorPos:	The position of the mouse cursor on screen
	Outputs:	-
	Returns:	-
	Purpose:	Tests whether the cursor and its surrounding bar shape rectangle is close
				enough to a docking bar that the bar will stick onto that dock.
				NOTE! This function has the side effect that it adjusts the DragOffset
				before returning TRUE.
				NOTE ALSO! This routine uses SimpleIntersect instead of wxRect::Intersect
				because SimpleIntersect works how I want it to (I wrote it).
	Errors:		-
	Scope:		private
	SeeAlso:	-

********************************************************************************************/

DockBarType BaseBar::BarCloseToDock(wxPoint CursorPos)
{
	wxRect Blobby;
   	
   	// if the user is holding down the Constrain key ( Clt ) we will not allow docking
   	
   	if(KeyPress::IsConstrainPressed())
		return DOCKBAR_INVALID;
	
	if(pOp->IsAllBarsOp())
		return DOCKBAR_INVALID; 
	
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// This Block is an attempt to fix a problem with the general drag algorithm
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	if (DragDockBarType == DOCKBAR_FLOAT)
	{
		BOOL  FromLeft = CursorPos.x > LeftRect.GetLeft() ;
		BOOL  FromTop = CursorPos.y > TopRect.GetTop() ;

		if (!pOp->IsKindOf(CC_RUNTIME_CLASS(SuperGallery)))
		{
			Blobby = GetDragFormatRect(DOCKBAR_TOP, CursorPos,FromLeft, FromTop);
			if (SimpleIntersect(Blobby,TopRect))
			{
				return(DOCKBAR_TOP);
			}															   

			// Test whether a horizontal shape bar would touch the bottom dock
			Blobby = GetDragFormatRect(DOCKBAR_BOTTOM, CursorPos,FromLeft,FromTop);
			if (SimpleIntersect(Blobby,BottomRect))
			{
				return(DOCKBAR_BOTTOM);
			}
		}

		// Don't allow infobars to be dropped on the vertical docking bars...
		if (!ThisIsAnInfoBar)
		{
			// Test whether a vertical shape bar would touch the left dock
			Blobby = GetDragFormatRect(DOCKBAR_LEFT, CursorPos,FromLeft,FromTop);
			if (SimpleIntersect(Blobby,LeftRect))
			{
				return(DOCKBAR_LEFT);
			}

			// Test whether a vertical shape bar would touch the right dock
			Blobby = GetDragFormatRect(DOCKBAR_RIGHT, CursorPos,FromLeft,FromTop);
			if (SimpleIntersect(Blobby,RightRect))
			{
				return(DOCKBAR_RIGHT);
			}
		}

	}
	else
	{
		if (!pOp->IsKindOf(CC_RUNTIME_CLASS(SuperGallery)))
		{
			// Test whether a horizontal shape bar would touch the top dock
			Blobby = GetDragFormatRect(DOCKBAR_TOP, CursorPos, DragOffsetFromLeft, CursorPos.y > TopRect.GetBottom());
			if (SimpleIntersect(Blobby,TopRect))
			{
				return(DOCKBAR_TOP);
			}															   

			// Test whether a horizontal shape bar would touch the bottom dock
			Blobby = GetDragFormatRect(DOCKBAR_BOTTOM, CursorPos, DragOffsetFromLeft, CursorPos.y > BottomRect.GetTop());
			if (SimpleIntersect(Blobby,BottomRect))
			{
				return(DOCKBAR_BOTTOM);
			}
		}

		// Don't allow infobars to be dropped on the vertical docking bars...
		if (!ThisIsAnInfoBar)
		{
			// Test whether a vertical shape bar would touch the left dock
			Blobby = GetDragFormatRect(DOCKBAR_LEFT, CursorPos, CursorPos.x > LeftRect.GetRight(), DragOffsetFromTop);
			if (SimpleIntersect(Blobby,LeftRect))
			{
				return(DOCKBAR_LEFT);
			}

			// Test whether a vertical shape bar would touch the right dock
			Blobby = GetDragFormatRect(DOCKBAR_RIGHT, CursorPos, CursorPos.x > RightRect.GetLeft(), DragOffsetFromTop);
			if (SimpleIntersect(Blobby,RightRect))
			{
				return(DOCKBAR_RIGHT);
			}
		}
	} 
	return(DOCKBAR_INVALID);
}




/********************************************************************************************

>	void BaseBar::CalcDragOffsetDirection(DockBarType BarType, wxPoint CursorPos)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/5/94
	Inputs:		BarType:	Which type of bar we are dealing with
				CursorPos:	The position of the mouse cursor on screen
	Outputs:	-
	Returns:	-
	Purpose:	Resets the two drag offset flags which record which edges of the dragged
				outline should not move when the bar outline changes shape.
	Errors:		-
	Scope:		private
	SeeAlso:	-

********************************************************************************************/

void BaseBar::CalcDragOffsetDirection(DockBarType BarType, wxPoint CursorPos)
{
	wxRect Blobby;

	switch (BarType)
	{
		case DOCKBAR_TOP:
			DragOffsetFromTop = CursorPos.y > TopRect.GetBottom();
//			DragOffsetFromLeft = TRUE;
			break;
		case DOCKBAR_BOTTOM:
			DragOffsetFromTop = CursorPos.y > BottomRect.GetTop();
//			DragOffsetFromLeft = TRUE;
			break;
		case DOCKBAR_LEFT:
			DragOffsetFromLeft = CursorPos.x > LeftRect.GetRight();
			break;
		case DOCKBAR_RIGHT:
			DragOffsetFromLeft = CursorPos.x > RightRect.GetLeft();
			break;
		default:
			ENSURE(FALSE,"CalcDragOffsetDirection called with invalid bar type");
			break;
	}
}




/********************************************************************************************

>	BOOL BaseBar::SimpleIntersect(LPRECT rect1, LPRECT rect2)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/5/94
	Inputs:		rect1,rect2:	rectangles to intersect
	Outputs:	-
	Returns:	-
	Purpose:	Replaces the brain-damaged wxRect::Intersect routine which insists on
				actually comuting the intersection in another rectangle, and most
				worryingly, returns FALSE when one of the rectangles is 0 pixels wide.

				This routine simply tests whether two rectangles intersect and returns TRUE
				if they do. It also returns true no matter what the size of the
				intersection!!! (IE. when it's 0 pixels wide/high).

				Algorithm is: check whether rect1 is completely to left or right of rect2
								and
							  check whether rect1 is completely above or below rect2
	Errors:		-
	Scope:		private
	SeeAlso:	-

********************************************************************************************/

BOOL BaseBar::SimpleIntersect( const wxRect &rect1, const wxRect &rect2)
{
	return( !( ( rect1.GetRight() < rect2.GetLeft() ) ||
			   ( rect1.GetLeft() >= rect2.GetRight() )
			  ) &&
			!( ( rect1.GetBottom() < rect2.GetTop() ) ||
			   ( rect1.GetTop() >= rect2.GetBottom() )
			  )
		   );
}



/********************************************************************************************

>	void BaseBar::PaintDragRect(wxRect Rect)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/4/94
	Inputs:		Rect: The rectangle to draw (screen coords)
	Outputs:	-
	Returns:	-
	Purpose:	Draws an XOR drag rectangle	anywhere on the display
	Errors:		-
	Scope:		private
	SeeAlso:	-

********************************************************************************************/

void BaseBar::PaintDragRect(wxRect Rect,wxRect OldRect,DockBarType DockBar,DockBarType OldBarType)
{
	wxScreenDC	DisplayDC;
	
	wxSize NewSize(1,1);
	wxSize OldSize(1,1);

	if (DockBar == DOCKBAR_FLOAT)
	{
		NewSize.x = NewSize.y = BorderWidth;
	}

	if (OldBarType == DOCKBAR_FLOAT)
	{
		OldSize.x = OldSize.y = BorderWidth;
	}

	// MFC DrawDragRect function
#pragma message( __LOCMSG__ "Remove DrawDragRect usage" )
//	pDisplayDC->DrawDragRect( &Rect, NewSize, &OldRect, OldSize );
	
	/*
	// paint a + to the right side of the box if we are doing a control copy drag
	if(CtlDragState != NO_DRAG)
	{
		if(CtlDragMode == COPY)
		{
		 	
		 	pDragBrush  =  new wxBrush(RGB(255,255,255)) ;
			
			// Select into the DC
			pOldBrush = pDisplayDC->SelectObject(pDragBrush);
			

			pDisplayDC->PatBlt(OldRect.GetRight()+2, OldRect.GetBottom()-8, 5, 1,PATINVERT); 
			pDisplayDC->PatBlt(OldRect.GetRight()+4, OldRect.GetBottom()-10, 1, 5,PATINVERT);

			// draw the +
			pDisplayDC->PatBlt(Rect.GetRight()+2, Rect.GetBottom()-8, 5, 1,PATINVERT); 
			pDisplayDC->PatBlt(Rect.GetRight()+4, Rect.GetBottom()-10, 1, 5,PATINVERT);
	
			pDisplayDC->SelectObject(pOldBrush);
			
			pDragBrush->DeleteObject(); 
			delete pDragBrush; 
		}
	
	}

	*/

	delete pDisplayDC;
}





/********************************************************************************************

>	void BaseBar::PaintXORDragRect(wxRect Rect)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/4/94
	Inputs:		Rect: The rectangle to draw (screen coords)
	Outputs:	-
	Returns:	-
	Purpose:	Draws an XOR drag rectangle	anywhere on the display
	Errors:		-
	Scope:		private
	SeeAlso:	-

********************************************************************************************/

void BaseBar::PaintXORDragRect(wxRect Rect,DockBarType DockBar)
{
#pragma message( __LOCMSG__ "Remove DC::PatBlt usage" )
/*	DWORD dwRop = PATINVERT;

	wxScreenDC	DisplayDC;
	
	// Create a pattern brush from our gray bitmap
	wxBrush		DragBrush( BrushBitmap );

	// Select these into the DC
	DisplayDC.SetBrush( DragBrush );

	if (DockBar==DOCKBAR_FLOAT)
	{
		// left vertical
		pDisplayDC->PatBlt(Rect.GetLeft(), Rect.GetTop(), BorderWidth, Rect.GetHeight(),dwRop); 
		// top horizontal
		pDisplayDC->PatBlt(Rect.GetLeft()+BorderWidth, Rect.GetTop(),
						   Rect.GetWidth()-(BorderWidth*2), BorderWidth,dwRop); 
		// right vertical
		pDisplayDC->PatBlt(Rect.GetRight()-BorderWidth, Rect.GetTop(),
						   BorderWidth, Rect.GetHeight(), dwRop); 
		// bottom horizontal
		pDisplayDC->PatBlt(Rect.GetLeft()+BorderWidth, Rect.GetBottom()-BorderWidth,
						   Rect.GetWidth()-(BorderWidth*2), BorderWidth, dwRop); 
	}
	else
	{
		// left vertical
		pDisplayDC->PatBlt(Rect.GetLeft(), Rect.GetTop(), 1, Rect.GetHeight(), dwRop); 
		// top horizontal
		pDisplayDC->PatBlt(Rect.GetLeft()+1, Rect.GetTop(), Rect.GetWidth()-2, 1, dwRop); 
		// right vertical
		pDisplayDC->PatBlt(Rect.GetRight()-1, Rect.GetTop(), 1, Rect.GetHeight(), dwRop); 
		// bottom horizontal
		pDisplayDC->PatBlt(Rect.GetLeft()+1, Rect.GetBottom()-1, Rect.GetWidth()-2, 1, dwRop); 
    } */
/*
	// paint a + to the right side of the box if we are doing a control copy drag
	if(CtlDragState != NO_DRAG)
	{
		if(CtlDragMode == COPY)
		{
		 	pDisplayDC->SelectObject(pOldBrush);
    		pDragBrush->DeleteObject(); 
		 	delete pDragBrush; 
		 	
		 	pDragBrush  =  new wxBrush(RGB(255,255,255)) ;
			
			// Select into the DC
			pOldBrush = pDisplayDC->SelectObject(pDragBrush);
			
			// draw the +
			pDisplayDC->PatBlt(Rect.GetRight()+2, Rect.GetBottom()-8, 5, 1, dwRop); 
			pDisplayDC->PatBlt(Rect.GetRight()+4, Rect.GetBottom()-10, 1, 5, dwRop);

		}
	
	}

	DisplayDC.SetBrush( wxNullBrush ); */
}




/********************************************************************************************

>	INT32 FAR PASCAL _export BaseBar::EnterSubclassProc(HWND hWnd,
													     UINT32 Message,
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
	Purpose:	This is the window procedure for all edit and combo controls on BaseBars
				It changes the controls so that a commit message is generated whenever 
				the user presses enter or tab in the control

	Errors:		-
	SeeAlso:	-

********************************************************************************************/
/*
INT32 FAR PASCAL _exportBaseBar::EnterSubclassProc(HWND hWnd,
												    UINT32 Message,
											   		UINT32 wParam,
											   		INT32 lParam)
{
	HWND MyDaddy = GetParent(hwnd);

	switch (Message)
	{

		case WM_CHAR:
			// Process this message to avoid message beeps.
			if ((wParam == CAMKEY(RETURN)) || (wParam == CAMKEY(TAB)))
				return FALSE;
			else
				return (CallWindowProc(lpOldProc, hWnd, 
									   Message, wParam, lParam));
	
		case WM_KEYDOWN:
			if ((wParam == CAMKEY(RETURN)) || (wParam == CAMKEY(TAB)))
			{
				// PostMessage();
				return FALSE;
			}
			else 
				return (CallWindowProc(lpOldProc, hWnd, 
									   Message, wParam, lParam));
			break;

	}
	return CallWindowProc((WNDPROC)(CurrentCntrl->lpfnOldProc), hwnd, message, wParam, lParam);
	
}
*/
