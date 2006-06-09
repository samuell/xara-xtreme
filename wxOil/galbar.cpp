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
// Implementation of the GalleryBar class

/*
*/
 
#include "camtypes.h"
#include "galbar.h"
#include "gallery.h"
//#include "sgallery.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "ensure.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "mainfrm.h"
#include "oilmods.h"
#include "dlgmgr.h"
#include "bitbutn.h"
//#include "resource.h" // temp
#include "childbar.h"
#include "bbutdef.h"
#include "ctrlhelp.h"
//#include "bars.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "camelot.h"
#include "ctl3d.h"
#include "fonts.h"
//#include "app.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "ed.h"

#define MIN(a,b)	(((a)>(b)) ? (b) : (a))
#define MAX(a,b)	(((a)>(b)) ? (a) : (b))

// The gallery should be large enough for the row of buttons and a bit of the listbox
// The default also minimises the chance of making the listbox too small for the scrollbar
// to fit inside it.
#define MINIMUM_GALLERY_HEIGHT 40

void Ctl3dSubClassCtl(HWND Hwnd); // for some reason not in an .h file


IMPLEMENT_DYNCREATE( GalleryBar,BaseBar )


/********************************************************************************************
>	void PatB(CDC* hDC, INT32 x, INT32 y, INT32 dx, INT32 dy, COLORREF rgb)

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
********************************************************************************************/

static void NEAR PASCAL PatB(CDC* cDC, INT32 x, INT32 y, INT32 dx, INT32 dy, COLORREF rgb)
{
	RECT rect;
	rect.left	= x;
	rect.top	= y;
	rect.right	= x + dx;
	rect.bottom	= y + dy;

	cDC->SetBkColor(rgb);
	cDC->ExtTextOut(0, 0, ETO_OPAQUE, &rect, NULL, 0, NULL);
}


// -----------------------------------------------------------------------------------------
// Message map - maintained by Class Wizard.

BEGIN_MESSAGE_MAP(GalleryBar, BaseBar)
	//{{AFX_MSG_MAP(GalleryBar)
	ON_WM_NCCALCSIZE()
	ON_WM_NCPAINT()
//	ON_WM_NCACTIVATE()
//	ON_WM_PAINT()
	ON_WM_NCHITTEST()
	ON_WM_MOUSEMOVE()
	ON_WM_NCLBUTTONDOWN()
//	ON_WM_LBUTTONDOWN()
//	ON_WM_LBUTTONDBLCLK()	
//	ON_WM_LBUTTONUP()
//	ON_WM_NCRBUTTONDOWN()
//	ON_MESSAGE( WM_RESIZECONTROLS,DoResize )
//	ON_MESSAGE(WM_CTL_SETFOCUS, SetControlFocus)
//	ON_WM_DESTROY( )
//	ON_WM_MOUSEACTIVATE( )
//	ON_WM_ACTIVATE( )
//	ON_WM_ACTIVATEAPP( )
//	ON_WM_PARENTNOTIFY( )
	ON_WM_MOUSEWHEEL()
//	ON_WM_SIZE()
	ON_MESSAGE(WM_GETBARSIZE,GetBarSize)
	ON_MESSAGE(WM_SETGALLERYSIZE,SetGallerySize)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/********************************************************************************************

>	void GalleryBar::GalleryBar() 
	 
	Author:		Alex
	Created:	1/7/94		  
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
GalleryBar::GalleryBar() : BaseBar()
{
	
	GallerySize.cx = 1;	// This will make it appear at the minimum width for now
	GallerySize.cy = 256;
	ListBoxHWND = NULL;
	ListFont = FontFactory::GetCFont(STOCKFONT_GALLERYLIST);
}



LRESULT GalleryBar::SetGallerySize(WPARAM cx, LPARAM cy)
{
	// Set this gallery to be the new size
	GallerySize.cx = cx;
	GallerySize.cy = cy - TitleBarHeight;

	FormatLineFeeds(cx);
	PositionControls();

	return(0);
}


/********************************************************************************************

>	void GalleryBar::OnMouseMove(UINT32 Flags, CPoint Point)
	 
	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/4/94		  
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
void GalleryBar::OnMouseMove(UINT32 Flags, CPoint Point)
{

	static INT32 LastFormatWidth=0;
	
	// always use screen co-ords 
	ClientToScreen(&Point); 

	// We are trying to start a drag
	if (DragType == TRYING)
	{
		if (CanStartDrag(Point))// Ok we have moved the mouse far enough from the start drag 
		{
			// release the mouse this will be captured again in StartDrag
			ReleaseCapture();
			// XOR the initial drag rectangle 
			PaintXORDragRect(OldDragRect,ParentDockBarType);
			//start the drag for real from the original point
	 		StartDrag(StartDragPoint,MOVE);
		}		 
	}
	if (DragType != NONE && DragType != TRYING)
	{
		// A drag is taking place 						
		// Calculate the new drag rect
//		TRACEUSER( "Gerry", _T("DragPoint     = (%d, %d)\n"), Point.x, Point.y);
//		TRACEUSER( "Gerry", _T("GallerySize   = (%d, %d)\n"), GallerySize.cx, GallerySize.cy);
		 
		CRect NewDragRect = OldDragRect;

		switch (DragType)
		{
			case MOVE:
			{
				// Try out the shapes of horz and vert bars to see if the touch a
				// docking bar...
				DockBarType TempDockBarType = BarCloseToDock(Point);
				if ( TempDockBarType != DOCKBAR_INVALID )
				{
					// Bar is close enough to dock to change shape...
					DragDockBarType = TempDockBarType;				// Set new shape type
					CalcDragOffsetDirection(DragDockBarType, Point);// Alter offset directions
					DragOffset = CalcDragOffset(GetFormatRect(DragDockBarType),
												Point,
												DragOffsetFromLeft,
												DragOffsetFromTop);	// Calc new offsets
				}
				else /* bar floats either inside or outside */
				{
					// Don't change directions of offset into floating shape!
					DragDockBarType = DOCKBAR_FLOAT;
					DragOffset = CalcDragOffset(GetFormatRect(DragDockBarType),
												Point,
												DragOffsetFromLeft,
												DragOffsetFromTop);	// Calc new offsets
				}
				// Set the new bar outline rect according to whatever was computed above
				// and position it on screen according to the cursor position and the
				// offsets computed above...
				NewDragRect = GetFormatRect(DragDockBarType);
				NewDragRect.OffsetRect(Point.x-DragOffset.x,Point.y - DragOffset.y); 

//				DragDockBarType = DOCKBAR_FLOAT;
//		    	NewDragRect = GetFormatRect(DragDockBarType);
//				NewDragRect.OffsetRect(Point.x-DragOffset.x,Point.y - DragOffset.y); 
			 	break;
			}

			case GROW_NORTH:
			{
				INT32 NewWidth;

				GallerySize.cx = MAX(0,DragStartRect.right-DragStartRect.left);
//				GallerySize.cy = MAX(0,DragStartRect.bottom-Point.y-TITLE_BAR_HEIGHT+BorderOffset.y);
				GallerySize.cy = MAX(0,DragStartRect.bottom-Point.y-TITLE_BAR_HEIGHT);
				NewWidth = GallerySize.cx ;// -((BAR_BORDERSIZE*2)+(BorderWidth*2));
				FormatLineFeeds(NewWidth);
				FormatBar(FALSE,USE_BAR);  

				CRect Temp = GetFormatRect(DragDockBarType);
				CRect TempNew(DragStartRect.left,
							  DragStartRect.bottom - Temp.Height(),
							  DragStartRect.left + Temp.Width(),
							  DragStartRect.bottom);

				NewDragRect = TempNew; 
			  	LastFormatWidth = NewWidth;
			  	break;
			}
			case GROW_NORTHWEST:
			{
				INT32 NewWidth;

				GallerySize.cx = MAX(0,DragStartRect.right-Point.x+BorderOffset.x);
				GallerySize.cy = MAX(0,DragStartRect.bottom-Point.y-TITLE_BAR_HEIGHT+BorderOffset.y);
				NewWidth = GallerySize.cx ;//-((BAR_BORDERSIZE*2)+(BorderWidth*2));
				FormatLineFeeds(NewWidth);
				FormatBar(FALSE,USE_BAR);  

				CRect Temp = GetFormatRect(DragDockBarType);
				CRect TempNew(DragStartRect.right - Temp.Width(),
							  DragStartRect.bottom - Temp.Height(),
							  DragStartRect.right,
							  DragStartRect.bottom);

				NewDragRect = TempNew; 
			  	LastFormatWidth = NewWidth;
			  	break;
			}
			case GROW_NORTHEAST:
			{
				INT32 NewWidth;

				
				GallerySize.cx = MAX(0,Point.x-DragStartRect.left+BorderOffset.x);
				GallerySize.cy = MAX(0,DragStartRect.bottom-Point.y-TITLE_BAR_HEIGHT+BorderOffset.y);
				NewWidth = GallerySize.cx ;//-((BAR_BORDERSIZE*2)+(BorderWidth*2));
				FormatLineFeeds(NewWidth);
				FormatBar(FALSE,USE_BAR);  

				CRect Temp = GetFormatRect(DragDockBarType);
				CRect TempNew(DragStartRect.left,
							  DragStartRect.bottom - Temp.Height(),
							  DragStartRect.left + Temp.Width(),
							  DragStartRect.bottom);


				NewDragRect = TempNew; 
			  	LastFormatWidth = NewWidth;
			  	break;
			}
			case GROW_SOUTH:
			{
				INT32 NewWidth;

				GallerySize.cx = MAX(0,DragStartRect.right-DragStartRect.left);
				GallerySize.cy = MAX(0,Point.y-DragStartRect.top-TITLE_BAR_HEIGHT+BorderOffset.y);
				NewWidth = GallerySize.cx ;//-((BAR_BORDERSIZE*2)+(BorderWidth*2));

				FormatLineFeeds(NewWidth);
				FormatBar(FALSE,USE_BAR);  
				CRect Temp = GetFormatRect(DragDockBarType);
				Temp.OffsetRect(DragStartRect.left,DragStartRect.top); 
				NewDragRect = Temp; 
	    		LastFormatWidth = NewWidth;
		    	break;

			}
			case GROW_SOUTHEAST:
			{
				INT32 NewWidth;

				GallerySize.cx = MAX(0,Point.x-DragStartRect.left+BorderOffset.x);
				GallerySize.cy = MAX(0,Point.y-DragStartRect.top-TITLE_BAR_HEIGHT+BorderOffset.y);
		
				NewWidth = GallerySize.cx ;//-((BAR_BORDERSIZE*2)+(BorderWidth*2));

				FormatLineFeeds(NewWidth);
				FormatBar(FALSE,USE_BAR);  
				CRect Temp = GetFormatRect(DragDockBarType);
				Temp.OffsetRect(DragStartRect.left,DragStartRect.top); 
				NewDragRect = Temp; 
	    		LastFormatWidth = NewWidth;
		    	break;

			}
	
			case GROW_SOUTHWEST:
			{
				INT32 NewWidth;

				GallerySize.cx = MAX(0,DragStartRect.right-Point.x+BorderOffset.x);
				GallerySize.cy = MAX(0,Point.y-DragStartRect.top-TITLE_BAR_HEIGHT+BorderOffset.y);
		
				NewWidth = GallerySize.cx ;//-((BAR_BORDERSIZE*2)+(BorderWidth*2));

				FormatLineFeeds(NewWidth);
				FormatBar(FALSE,USE_BAR);  
				
				CRect Temp = GetFormatRect(DragDockBarType); 
				CRect TempNew(DragStartRect.right - Temp.Width(),
							  DragStartRect.top,
							  DragStartRect.right,
							  DragStartRect.top + Temp.Height());
		
				NewDragRect = TempNew; 
	    		LastFormatWidth = NewWidth;
				break; 
			}
            			
			case GROW_EAST:
			{
				INT32 NewWidth;

				GallerySize.cx = MAX(0,Point.x-DragStartRect.left+BorderOffset.x);
//				GallerySize.cy = MAX(0,DragStartRect.bottom-DragStartRect.top-TITLE_BAR_HEIGHT);
				NewWidth = GallerySize.cx; //-((BAR_BORDERSIZE*2)+(BorderWidth*2));

				FormatLineFeeds(NewWidth);
				FormatBar(FALSE,USE_BAR);
				  
				CRect Temp = GetFormatRect(DragDockBarType);
				Temp.OffsetRect(DragStartRect.left,DragStartRect.top); 
				NewDragRect = Temp; 
	    		LastFormatWidth = NewWidth;
			    break; 
			}

			case GROW_WEST:
			{
				INT32 NewWidth;

				GallerySize.cx = MAX(0,DragStartRect.right-Point.x+BorderOffset.x);
//				GallerySize.cy = MAX(0,DragStartRect.bottom-DragStartRect.top-TITLE_BAR_HEIGHT);
				NewWidth = GallerySize.cx;  //-((BAR_BORDERSIZE*2)+(BorderWidth*2));

				FormatLineFeeds(NewWidth);
				FormatBar(FALSE,USE_BAR);  
				
				CRect Temp = GetFormatRect(DragDockBarType); 
				CRect TempNew(DragStartRect.right - Temp.Width(),
							  DragStartRect.top,
							  DragStartRect.right,
							  DragStartRect.top + Temp.Height());
		
				NewDragRect = TempNew; 
    			LastFormatWidth = NewWidth;
				break; 
			}

		}

		if(NewDragRect!=OldDragRect)
		{
//			TRACEUSER( "Gerry", _T("NewDragRect   = (%d, %d) - (%d, %d)\n"), NewDragRect.left, NewDragRect.top, NewDragRect.Width(), NewDragRect.Height());
//			PaintDragRect(NewDragRect,OldDragRect,DOCKBAR_FLOAT,DOCKBAR_FLOAT);
			PaintDragRect(NewDragRect,OldDragRect,DragDockBarType,OldDragBarType);
			OldDragBarType=DragDockBarType;
			OldDragRect = NewDragRect; 
		}
	}
													  
}



/****************************************************************************

>	UINT32 GalleryBar::OnNcHitTest(CPoint point)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/11/2004

	Inputs:		point		- Screen coordinate of point to test
	Returns:	
	Purpose:	Determines the part of window the point is over.

****************************************************************************/

UINT32 GalleryBar::OnNcHitTest(CPoint point)
{
	if (DragType != NONE)
		return HTCLIENT; 

	if (CurrentDlgBarMode == DIALOG)
	{
		return(BaseBar::OnNcHitTest(point));
	}
	else
	{
		// Get the window dimensions
		CRect rect;
		GetWindowRect(&rect);
		CRect InsideBorderRect = rect;
		InsideBorderRect.InflateRect(-BorderWidth,-BorderWidth);
		CPoint NonClientPoint (point.x - rect.left, point.y - rect.top); 

//		TRACE( _T("NcHitTest (%d, %d)\n"), NonClientPoint.x, NonClientPoint.y);

		// Determine what active area the cursor is over
		if (SysMenuRect.PtInRect(NonClientPoint))
		{
			return HTSYSMENU; 
		}
		else if (TopBorderRect.PtInRect(NonClientPoint))
		{
			return HTTOP; 
		}
		else if (BottomBorderRect.PtInRect(NonClientPoint))
		{
			return HTBOTTOM; 
		}
		else if (LeftBorderRect.PtInRect(NonClientPoint))
		{
			return HTLEFT; 
		}
		else if (RightBorderRect.PtInRect(NonClientPoint))
		{
			return HTRIGHT; 
		}
		else if (CaptionBarRect.PtInRect(NonClientPoint))
		{
			return HTCAPTION; 
		}
	}

	// Normal client area hit 
	return HTCLIENT; 
}

/********************************************************************************************

>	void GalleryBar::OnNcLButtonDown(UINT32 nHitTest, CPoint point)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/4/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void GalleryBar::OnNcLButtonDown(UINT32 nHitTest, CPoint point)
{

	ENSURE(DragType == NONE, "There should not be a drag going on"); 
	//SetFocus();
//	if (CurrentDlgBarMode == DIALOG)
//	{
		switch (nHitTest)
		{
			case HTCLIENT:
			{
				CWnd::OnNcLButtonDown(nHitTest, point);
				break;  
			}
		
			case HTSYSMENU:
			{
				// Send a message to the GalleryBarOp to inform it that cancel has been selected
				DialogMsg tmp(this->GetSafeHwnd(), DIM_CANCEL, NULL);
				pOp->Message(&tmp); 
				break;    	
			}

			// The following hit test values all start a drag

			case HTCAPTION:
			{
				StartDrag(point,MOVE); 
				break; 
			}

			case HTTOP:
			{
				StartDrag(point, GROW_NORTH); 
				break; 
			}

			case HTBOTTOM:
			{
				StartDrag(point, GROW_SOUTH); 
				break; 
			}

			case HTLEFT:
			{
				StartDrag(point, GROW_WEST); 
				break; 
			}

			case HTRIGHT:
			{
				StartDrag(point, GROW_EAST); 
				break; 
			}

			
			case HTTOPLEFT:
			{
				StartDrag(point, GROW_NORTHWEST); 
				break; 
			}

			case HTBOTTOMLEFT:
			{
				StartDrag(point, GROW_SOUTHWEST); 
				break; 
			}

			case HTTOPRIGHT:
			{
				StartDrag(point, GROW_NORTHEAST); 
				break; 
			}

			case HTBOTTOMRIGHT:
			{
				StartDrag(point, GROW_SOUTHEAST); 
				break; 
			}
			default:
			{
				ENSURE(FALSE,"Unknown Hit test value"); 
				break;  
			}

		}
//	}
//	else 
//	{
//		// Don't do anything special with bar non client messages
//		CWnd::OnNcLButtonDown(nHitTest, point);
//	}
		 
	return; 	

}


/********************************************************************************************

>	void GalleryBar::AdjustFormattedBarSize(CSize* pBarSize)

	Author:		Alex
	Created:	1/7/94
	Inputs:		Pointer to the bar size record to adjust 
	Outputs:	-
	Returns:	None
	Purpose:	Adjusts the proposed new size of a bar once formatting has taken place   
	Errors:		-
	SeeAlso:	GalleryBar::AdjustFormattedBarSize

This function adjust the size of a bar after the formatting has taken place.

The size of the formatted rectangle is in BarSize. We output the new bar size.

********************************************************************************************/

void GalleryBar::AdjustFormattedBarSize(CSize* pBarSize)
{
//	TRACE( _T("AFBS BarSize (%d, %d)\n"), pBarSize->cx, pBarSize->cy);
//	TRACE( _T("AFBS GalSize (%d, %d)\n"), GallerySize.cx, GallerySize.cy);
	GallerySize.cy = pBarSize->cy = MAX((pBarSize->cy + MINIMUM_GALLERY_HEIGHT), GallerySize.cy); 
//	GallerySize.cy = pBarSize->cy = MAX(pBarSize->cy, GallerySize.cy); 
	GallerySize.cx = pBarSize->cx = MAX(pBarSize->cx, GallerySize.cx);
//	INT32 Newx =  MAX(pBarSize->cx, GallerySize.cx);
//	CRect Temp = GetFormatRect(DOCKBAR_BOTTOM);
//	if(Newx >=Temp.Width())
//		  GallerySize.cx = pBarSize->cx =Newx;
	
	if (pOp->IsKindOf(CC_RUNTIME_CLASS(SuperGallery)))
		((SuperGallery * )pOp)->SetGallerySize(GallerySize);
#ifdef _DEBUG
	else
		ERROR3("GalleryBar::AdjustFormattedBarSize - I'm not a SuperGallery!?!");
#endif

//	TRACE( _T("AFBS NewSize (%d, %d)\n"), pBarSize->cx, pBarSize->cy);
}



/********************************************************************************************

>	void GalleryBar::PositionExtraControls(CSize BarSize, INT32 xpos, INT32 ypos)

	Author:		Alex
	Created:	1/7/94
	Inputs:		Bar size record, x & y coord of current format position. 
	Outputs:	-
	Returns:	None
	Purpose:	Moves extra controls about once they have been created   
	Errors:		-
	SeeAlso:	BaseBar::PositionExtraControls, GalleryBar::CreateExtraControls

This function does nothing in the base class, but may be overridden, for instance by Gallery
Bars.

********************************************************************************************/

void GalleryBar::PositionExtraControls(CSize BarSize, INT32 xpos, INT32 ypos)
{
	::SetWindowPos(ListBoxHWND,NULL,
					BAR_BORDERSIZE,
					ypos + BAR_BORDERSIZE,
					BarSize.cx - 2 * BAR_BORDERSIZE -  (BorderWidth * 2),
					BarSize.cy-ypos - 2 * BAR_BORDERSIZE - (BorderWidth * 2),	// Move the control 
					SWP_NOZORDER);
}

/********************************************************************************************

>	INT32 GalleryBar::CreateExtraControls()

	Author:		Alex
	Created:	1/7/94
	Inputs:		None 
	Outputs:	TRUE if successful, else FALSE
	Returns:	None
	Purpose:	Allows subclasses to create additional bar controls   
	Errors:		-
	SeeAlso:	BaseBar::CreateExtraControls, GalleryBar::PositionExtraControls

This function does nothing in the base class, but may be overridden, for instance by Gallery
Bars.

********************************************************************************************/

INT32 GalleryBar::CreateExtraControls()
{
	OILDockingBar * ThisDock =GetMainFrame()->GetDockBar(ParentDockBarType);

	DWORD ExtraStyleFlags = 0;
	// Ask op if the list box can have multiple selections or not.
	if (pOp->AllowMulSelInListBox())
		ExtraStyleFlags = LBS_EXTENDEDSEL | LBS_MULTIPLESEL;

	HWND CreatedWindow = NULL;
	if (pOp->IsKindOf(CC_RUNTIME_CLASS(SuperGallery)))
	{
		// Create a kernel-redrawn control for the list box of a SuperGallery
		CreatedWindow = ::CreateWindowEx(
							//#if WIN32
							//	pCurrentControl->dwExtendedStyle,
							//#else
								0,
							//#endif
							   "cc_DialogDraw", 	// Class name
							   0,			// Window title
							   WS_BORDER | WS_CHILD |WS_VISIBLE,
							   0,
							   0,
							   10,
							   10, // Control gets resized anyway, but should be cleared up
							   GetSafeHwnd(),				// this window is its parent 
							   (HMENU)_R(IDC_GALLERY_LISTBOX),
							   AfxGetApp()->m_hInstance,
							   0 //pCreationData 				
							   );
	}


	if (CreatedWindow == NULL) 
		return FALSE;

	Ctl3dSubClassCtl(CreatedWindow);

	ListBoxHWND = CreatedWindow;
   /*
	ControlHelpInfo Info;
	Info.pOpDesc = NULL;
	Info.BubbleID = _R(IDBBL_NOOP);
	Info.StatusID = 0;
	Info.Parent = GetSafeHwnd();

	ControlHelper::NotifyControlCreated(CreatedWindow,&Info); 
   */

	CSize NewGallerySize;
	
	if (pOp->IsKindOf(CC_RUNTIME_CLASS(SuperGallery)))
		NewGallerySize =((SuperGallery *) pOp)->GetGallerySize();

	if (NewGallerySize.cx == 0 && NewGallerySize.cy == 0)
	{
		NewGallerySize = pOp->GetBarRect().BottomRight();
	}

	if (NewGallerySize.cx > 0)
		GallerySize.cx = NewGallerySize.cx; 
	if (NewGallerySize.cy > 0)
		GallerySize.cy = NewGallerySize.cy; 
	::SendMessage( CreatedWindow, WM_SETFONT, (WPARAM)ListFont->m_hObject, 0);
	return TRUE ;	

}
/********************************************************************************************

>	virtual BOOL GalleryBar::SetStatusLineText()

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/11/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if I have put text on the StatusLine FALSE otherwise
	Purpose:	actually put text on the status line
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL GalleryBar::SetStatusLineText()
{
	switch (DragType)
	{
		case  	MOVE :
			GetApplication()->UpdateStatusBarText(&(String_256(_R(IDS_STATUSLINE_GALLERYDRAG))),FALSE); 
			return TRUE;
		case  	GROW_NORTH:
		case 	GROW_SOUTH:
		case	GROW_EAST:
		case	GROW_WEST:
			GetApplication()->UpdateStatusBarText(&(String_256(_R(IDS_STATUSLINE_BARRESIZE))),FALSE); 
			return TRUE;
		case NONE:
			return FALSE;
	}

	return FALSE;
}

/********************************************************************************************

>	virtual BOOL GalleryBar::GetStatusText(String_256 * StatusText)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/11/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the buffer is valid FALSE otherwise
	Purpose:	put stauts text in a buffer
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL GalleryBar::GetStatusText(String_256 * StatusText)
{
	switch (DragType)
	{
		case  	MOVE :
			* StatusText = String_256(_R(IDS_STATUSLINE_GALLERYDRAG)); 
			return TRUE;
		case  	GROW_NORTH:
		case 	GROW_SOUTH:
		case	GROW_EAST:
		case	GROW_WEST:
			* StatusText = String_256(_R(IDS_STATUSLINE_BARRESIZE)); 
			return TRUE;
		case NONE:
			return FALSE;
	}

	return FALSE;
}



BOOL GalleryBar::OnMouseWheel(UINT32 nFlags, short zDelta, CPoint pt)
{
	TRACEUSER( "Matt", _T("MouseWheel scrolled in GalleryBar!\n"));
	return TRUE;
}


CRect GalleryBar::GetFormatRect(DockBarType BarType)
{
	OILDockingBar* DockBar = GetMainFrame()->GetDockBar(BarType);
	if (BarType == DOCKBAR_FLOAT)
	{
		OILDockingBar* DockBar = GetMainFrame()->GetDockBar(BarType);
		if (DockBar->IsBigControls())
		{
			if (FloatLargeRect.IsRectEmpty())
				FormatFloatingBar(FALSE,FORCE_LARGE);
			return FloatLargeRect;
		}
		else
		{
			if (FloatSmallRect.IsRectEmpty())
				FormatFloatingBar(FALSE,FORCE_SMALL);
			return FloatSmallRect;
		}
	}
	else
	{
		if (DockBar->IsBigControls())
		{
			if (VerticalLargeRect.IsRectEmpty())
				FormatVerticalBar(FALSE,FORCE_LARGE);
			return VerticalLargeRect;
		}
		else
		{
			if (VerticalSmallRect.IsRectEmpty())
				FormatVerticalBar(FALSE,FORCE_SMALL);
			return VerticalSmallRect;
		}
	}

	return CRect(0,0,0,0);
	
//	return(BaseBar::GetFormatRect(BarType));
//	return(CRect(0, 0, GallerySize.cx, GallerySize.cy));
}


/********************************************************************************************

>	LRESULT GalleryBar::GetBarSize(WPARAM DockType, LPARAM ControlSize )

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/5/94
	Inputs:		from windows
	Outputs:	-
	Returns:	-
	Purpose:	To return a pointer to a rectangle of bar size 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

LRESULT GalleryBar::GetBarSize(WPARAM DockType , LPARAM ControlS)
{
	DockBarType ThisDock = (DockBarType) DockType;
	ControlSizes ControlSize = (ControlSizes) ControlS;

	if (ThisDock == DOCKBAR_FLOAT)
	{
		if (ControlSize == LARGE_CONTROLS)
			return (LRESULT) &FloatLargeRect;
		else
			return (LRESULT) &FloatSmallRect;
	}
	else
	{
		if(ControlSize == LARGE_CONTROLS)
			return (LRESULT) &VerticalLargeRect ;
		else
			return (LRESULT) &VerticalSmallRect ; 
	}

	return(0);
}


/********************************************************************************************

>	BOOL GalleryBar::PositionControls(BOOL DoRedraw = TRUE)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/5/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Format and place controls according to docking bar type
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL GalleryBar::PositionControls(BOOL DoRedraw)
{
	// Position the controls in the bar
	// We also format the other type of bar so that strange things
	// don't happen to the window size when docking/undocking
	switch(ParentDockBarType)
	{
	case DOCKBAR_FLOAT:
		FormatFloatingBar(TRUE,USE_BAR);
		FormatVerticalBar(FALSE,USE_BAR);
		pOp->SetCurrentOrientation(Horizontal); 
		break;

	case DOCKBAR_LEFT:
	case DOCKBAR_RIGHT:
		FormatVerticalBar(TRUE,USE_BAR);
		FormatFloatingBar(FALSE,USE_BAR);
		pOp->SetCurrentOrientation(Horizontal); 
		break;
	
	default:
		ENSURE(FALSE,"Unknown Parent DockBar");
	}
	// Size the window correctly but don't redraw
	if (DoRedraw)
		SetBarSize(ParentDockBarType, DoRedraw);
	return TRUE ;
}


BOOL GalleryBar::FormatBar(BOOL MoveWindows,ForceControlSize ForceSize)
{
	BOOL bRetVal = FALSE;
	switch(ParentDockBarType)
	{
		case DOCKBAR_FLOAT:
			bRetVal = FormatFloatingBar(MoveWindows, ForceSize);
			break;

		case DOCKBAR_LEFT:
		case DOCKBAR_RIGHT:
			bRetVal = FormatVerticalBar(MoveWindows, ForceSize);
			break;
	}
	return(bRetVal);
}	


/********************************************************************************************

>	BOOL GalleryBar::FormatHorizontalBar(BOOL MoveWindows,ForceControlSize Force)

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
BOOL GalleryBar::FormatHorizontalBar(BOOL MoveControls, ForceControlSize ForceSize)
{
	return(TRUE);
//	return(FormatFloatingBar(MoveControls, ForceSize));
}

/********************************************************************************************

>	BOOL GalleryBar::FormatVerticalBar(BOOL MoveWindows,ForceControlSize Force)

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
BOOL GalleryBar::FormatVerticalBar(BOOL MoveControls, ForceControlSize ForceSize)
{
//	return(FormatFloatingBar(MoveControls, ForceSize));

//	TRACEUSER( "Gerry", _T("FormatVerticalBar\n"));

	HWND hcontrol;
 	CSize BarSize(0,0);
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
		ForceSize = ThisDock->IsBigControls() ? FORCE_LARGE : FORCE_SMALL;
	}

	if (ForceSize == FORCE_LARGE)
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
		 		CRect WindSize;

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
//						yOffset = (ControlHeight / 2) - WindSize.Height()+1;
						yOffset = (ControlHeight / 2) - DDeckerHeight + 1 ;
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
					yOffset = (ControlHeight - WindSize.Height()) / 2; //get offset to center control
        			break;
				}
				
       			if (MoveControls)
		            ::SetWindowPos(hcontrol,NULL,xpos,ypos + yOffset,0,0,	// Move the control 
								   SWP_SHOWWINDOW|SWP_NOSIZE|SWP_NOZORDER|SWP_NOREDRAW);
				
				xpos += WindSize.Width()-1;
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
		BarSize.cx = MaxWidth + BAR_BORDERSIZE + (1 * 2) + 1 ;
		BarSize.cy = ypos + ControlHeight + BAR_BORDERSIZE + (1 * 2)  ;

	}		
	// Set a Minimum Bar Height
	if (BarSize.cy < ControlHeight + (BAR_BORDERSIZE)+ (1 * 2))
		BarSize.cy = ControlHeight + (BAR_BORDERSIZE)+ (1 * 2);	

	// ----------------------------
	// Set a Minimum Bar Width

	String_32 Name = pOp->GetName();

	INT32 SmallestWidth = ControlWidth + (BAR_BORDERSIZE * 2)+ (1 * 2);

	// If this an empty bar make sure we can see the title
	if (pOp->IsListEmpty())
	{ 
	 	// Select the title font into a screen dc and get the text extent of the title
	 	CDC *dc = GetWindowDC();

		CFont * OldFont=dc->SelectObject(TitleFont);

		CSize TextSize=dc->GetTextExtent(Name,camStrlen(Name));

		dc->SelectObject(OldFont);

		ReleaseDC(dc);

		// Check that the title string will be visible UIC
		if ((TextSize.cx + TitleBarHeight + (1 * 4) ) > SmallestWidth)
			SmallestWidth = TextSize.cx + TitleBarHeight + (1 * 4);
	}

	
	if (BarSize.cx < SmallestWidth)
		BarSize.cx = SmallestWidth ;	

	AdjustFormattedBarSize(&BarSize);
	if (MoveControls) 
		PositionExtraControls(BarSize,xpos,ypos+ControlHeight);

	// Cache the rectangle
	if (ForceSize == FORCE_LARGE)
	{
		VerticalLargeRect.right = BarSize.cx;
		VerticalLargeRect.bottom = BarSize.cy + TitleBarHeight;
	}
	else if (ForceSize == FORCE_SMALL)
	{
		VerticalSmallRect.right = BarSize.cx;
		VerticalSmallRect.bottom = BarSize.cy + TitleBarHeight;
	}
	return TRUE;
}


/********************************************************************************************

>	CRect GalleryBar::SetBarSize(DockBarType BarType,BOOL DoRedraw)

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

CRect GalleryBar::SetBarSize(DockBarType BarType ,BOOL DoRedraw)
{
	CRect temp;
	if (BarType == DOCKBAR_FLOAT)
	{
		if (ParentDockBar->IsBigControls())		
			temp = FloatLargeRect;
		else
			temp = FloatSmallRect;
	}
	else
	{
		if (ParentDockBar->IsBigControls())		
			temp = VerticalLargeRect;
		else
			temp = VerticalSmallRect;
	}

	UINT32 flags = SWP_NOMOVE|SWP_NOZORDER;
	if (!DoRedraw)
		flags |= SWP_NOREDRAW;
	SetWindowPos(NULL,0,0,temp.right,temp.bottom,flags);

	return temp;
}


void GalleryBar::OnNcPaint()
{
	if (CurrentDlgBarMode == DIALOG)
	{
		PaintWin95DialogNonClient(Active);
	}
	else
	{
		PaintWin95BarNonClient(Active);
	}
}

void GalleryBar::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp)
{
	if (CurrentDlgBarMode == DIALOG)
	{
		BaseBar::OnNcCalcSize(bCalcValidRects, lpncsp);
	}
	else
	{
		lpncsp->rgrc[0].left += 1;
		lpncsp->rgrc[0].top += TitleBarHeight + 1;	// UIC
		lpncsp->rgrc[0].right -= 1;
		lpncsp->rgrc[0].bottom -= 1;

//		CWnd::OnNcCalcSize(bCalcValidRects, lpncsp);
//		lpncsp->rgrc[0].top += TitleBarHeight;
	}
}


void GalleryBar::PaintWin95BarNonClient(BOOL IsActive)
{
	// When the GalleryBar is in BAR mode we have to paint the fancy non-client area. 

	// Get the size of the window	
	CRect WinRect;
	GetWindowRect(&WinRect);

	// Set up the active non-client regions 
	SysMenuRect = CRect(WinRect.Width() - TitleBarHeight, 
			  	         3, 
			  		     WinRect.Width() - 2, 
			  		     TitleBarHeight);

//	TRACE( _T("SMRG = (%d, %d)\n"), SysMenuRect.Width(), SysMenuRect.Height());

	TopBorderRect = CRect(0, 0, WinRect.Width(), 2);

	BottomBorderRect = CRect(0, WinRect.Height() - 2, WinRect.Width(), WinRect.Height());

	LeftBorderRect = CRect(0, 0, 2, WinRect.Height());
				 		 
	RightBorderRect = CRect(WinRect.Width() - 2, 0, WinRect.Width(), WinRect.Height());

	CaptionBarRect = CRect(1, 1, WinRect.Width() - 1, TitleBarHeight + 1);

	// Get a context onto the entire window area
	CDC *dc = GetWindowDC();
	
	// -------------------------------------------------------------------------------------
	// Paint the borders frame

//	CRect WholeRect(0, 0, WinRect.Width(), WinRect.Height());
//	CBrush TempBrush(GetSysColor(COLOR_INACTIVECAPTION));
//	dc->FillRect(&WholeRect, &TempBrush);

//	CBrush FrameBrush(GetSysColor(COLOR_BTNFACE));

//	dc->FillRect(&TopBorderRect, &FrameBrush);
//	dc->FillRect(&BottomBorderRect, &FrameBrush);
//	dc->FillRect(&LeftBorderRect, &FrameBrush);
//	dc->FillRect(&RightBorderRect, &FrameBrush);
	
	DWORD TopLeftCol = GetSysColor(COLOR_BTNHIGHLIGHT);
	DWORD BottomRightCol = GetSysColor(COLOR_BTNSHADOW);

	PatB(dc, 0, 0, 1, WinRect.Height(), TopLeftCol);
	
	PatB(dc, 0, 0, WinRect.Width(), 1, TopLeftCol);
	
	PatB(dc, WinRect.Width()-1, 0, 1, WinRect.Height(), BottomRightCol);
	
	PatB(dc, 0, WinRect.Height()-1, WinRect.Width(), 1, BottomRightCol);

	// Paint the caption
	// make sure that DrawCaption shows the correct title
	SetWindowText(pOp->GetName());
	
	UINT32 DrawFlags = DC_TEXT|DC_SMALLCAP;
	
	if (IsActive)
		DrawFlags |= DC_ACTIVE;

	// draw the caption
	::DrawCaption(GetSafeHwnd(), dc->m_hDC, &CaptionBarRect, DrawFlags);
	
	// draw the new stretchy close button
	dc->DrawFrameControl(&SysMenuRect, DFC_CAPTION, DFCS_CAPTIONCLOSE);

	// Clean up release the DC
	ReleaseDC(dc) ;
	
}	
