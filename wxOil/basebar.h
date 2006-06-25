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
/* $Log*/

#ifndef INC_BASEBAR
#define INC_BASEBAR

//#include "dockbar.h"  - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "bars.h" - in camtypes.h [AUTOMATICALLY REMOVED]


#ifndef WIN32
#include "dlgmgr.h"		// for DLGTEMPLATE etc
#endif


class Cursor;

#define TITLE_BAR_HEIGHT 10 
#define BAR_BORDERSIZE 4
#define BAR_VERTICAL_CENTEROFFSET 2
#define BAR_HORIZONTAL_CENTEROFFSET 3

#define DO_TIDY TRUE
#define DONT_TIDY FALSE

#define CHANGE_MARGIN 4

#define INFOBAR_WIDTH 600

#define DRAG_START_MARGIN 1

// the gap between lines of controls on floating bars
#define VERTICAL_SPACE 2

enum DDeckerControlHeights { SMALL_DDECKERS = 13, LARGE_DDECKERS  = 16 };

// BaseBars can either be in a dialog or a bar mode
enum BaseBarMode {DIALOG, BAR}; 

// The different drag types 
enum DlgDragType { NONE = 0, MOVE, GROW_NORTH, GROW_SOUTH, GROW_EAST, GROW_WEST,
				GROW_SOUTHEAST,GROW_NORTHEAST,GROW_SOUTHWEST,GROW_NORTHWEST, TRYING };

// The Nearest edge to dragged bar 
enum NearestEdge { LEFT,RIGHT,TOP,BOTTOM,NOEDGE };

enum SeparatorWidths { SMALL_SEPARATOR = 6,LARGE_SEPARATOR = 10};


enum ForceControlSize { USE_BAR,FORCE_SMALL,FORCE_LARGE};

enum DDeckerReadState {DDECKER_NONE,DDECKER_TOP,DDECKER_BOTTOM};

enum ControlDragMode{CUT,COPY};

enum ControlDragState{READY,DRAGGING,NO_DRAG};

typedef enum
{
	BUTTON,
	BITMAPBUTTON,
	SMALLBITMAPBUTTON,
	STATICBITMAP,
	DECKER_TOP,
	DECKER_BOTTOM,
	DECKER_END,
	SEPARATOR,
	LINEFEED,
	COMBO,
	EDIT,
	LIST,
	STATIC,
	SCROLLBAR,
	TEXT3D,
	ROTATEGRID,
	OTHERCONTROL,
	SLIDER,
	BITMAPCOMBO1D,
	BITMAPCOMBO2D,
	UNKNOWN_ITEM 	
} BarItemType;

BarItemType	GetEnumItemType(BarItem * Item);

/********************************************************************************************

>	class FormatPosition : public ListItem

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/5/94
	Purpose:	A class used for holding Format Positions.

********************************************************************************************/

class FormatPosition : public ListItem
{
	CC_DECLARE_DYNAMIC( FormatPosition )

public:
	INT32 Height;
	INT32 Width;

	FormatPosition()
	{
		Height=0;
		Width=0;
 	};
	
	FormatPosition(INT32 w,INT32 h)
	{
		Height =h;
		Width =w;
	};


};



/********************************************************************************************

>	class BaseBar : public CWnd

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/03/94
	SeeAlso:	DialogBarOp

********************************************************************************************/


class BaseBar : public wxWindow
{
public:
	
	BaseBar(); 
	virtual ~BaseBar();

	static BOOL DeclarePreferences();
	static BOOL PlotStretchedMaskedBitmap(wxDC* destDC, wxBitmap* srcBitmap, wxRect Pos); // UIC
	static BOOL HaveNonClientMetricsChanged(); // UIC

	INT32 Create(DialogBarOp* Op);
	BOOL Hide(DialogBarOp* Op);
	BOOL Show(DialogBarOp* Op);

		
	BOOL Recreate(DockBarType DockBarTyp, BaseBar* BaseBar, DialogBarOp* Op,BOOL CanCopy );
	BOOL RecreateAt(KernelBarPos Pos,DockBarType DockBarTyp,
			   BaseBar* BaseBar, DialogBarOp* Op, wxWindow* OldHwnd );  

	static BOOL IsDoingErrorBox();
	static void StartErrorBox();
	static void FinishErrorBox();

	static BOOL IsBarInRecreate();
	static BOOL GetStatusLineText(String_256 * StatusText);

	static BOOL HasGalleryGotFocus(){return GalleryHasFocus;};
		
	static BOOL AltPressed(BOOL state);
	static BOOL CtlPressed(BOOL state);
	static BOOL EscPressed(BOOL state);
	static BOOL CancelAllDrags();
	static void ClearFocusFlags(){ControlHasFocus = FALSE;HwndWithFocus =(wxWindow*)0;}; // used to be ControlHasFocus+=NULL ???

	void ConvertToolbarOnStartup ();
	static BaseBar* GetPtrToToolBar () { return (PtrToToolBar); }
	BOOL FormatToolBar (INT32 NewWidth);
	DialogBarOp* GetDialogBarOp () { return (pOp); }
	wxRect GetBarsDims (DockBarType DockBarType);

	BOOL TidyUpControlDrag();
	BOOL TidyUpBarDrag();

	UINT32 GetNearestControl(wxPoint DropPoint);
	BOOL IsBarActive(){return Active;};
	BOOL IsInfoBar(){ return ThisIsAnInfoBar ;} ;
	BOOL IsBarAllCtlBar(){return pOp->IsAllBarsOp(); };
	
	void SetBarActiveState(BOOL State){ Active = State;};
	
	// Get cached rectangle 
	virtual wxRect GetFormatRect(DockBarType BarType);

	void InitFormatRects();

	String_32 GetBarName(){return pOp->GetName();};

	LRESULT WindowProc( UINT32 message, WPARAM wParam, LPARAM lParam ); 

    DECLARE_DYNAMIC_CLASS( BaseBar )
    DECLARE_EVENT_TABLE()

protected:
	
	// -------------------------------------------------------------------------------------
	// Functions 
	
	
	BOOL EndControlDrag(wxPoint point);
	BOOL DropControl(wxPoint point,wxWindow* OverBar,BOOL InsertSeparator);
	void CacheAllDockRects();

	void Paint3dPlinth(wxDC* pDC, wxRect *rect, BOOL PlinthOut);

	// Test if dragged bar will touch a dock
	virtual DockBarType BarCloseToDock(wxPoint CursorPos);

	// just sets up a few variables must be called at every create ( and recreate etc ..)
	virtual void SetFormatVariables();
							
	// Used to paint the non-client area when in DIALOG mode
	virtual void PaintDialogNonClient(BOOL Active);
	
	virtual void PaintWin95DialogNonClient(BOOL IsActive);

	virtual void PaintChicagoBorder();

	// Paints the XOR drag rectangle 
	void PaintXORDragRect(wxRect Rect, DockBarType DockBar); 
	
	void PaintDragRect(wxRect Rect,wxRect OldRect,DockBarType DockBar,DockBarType OldBarType);

	// The StartDrag function is used to start a move or resize drag
	virtual void StartDrag(wxPoint point, DlgDragType TypeOfDrag);

	// Used to decide whether we can start a drag yet
	virtual BOOL CanStartDrag(wxPoint point);

	virtual DockBarType IsPointerOverDock(wxPoint point);

	// Close all dropdown windows
	void CloseCombos();

   	virtual wxRect GetDragFormatRect(DockBarType BarType, wxPoint CursorPos, BOOL ToLeft, BOOL Below);

	// Get new drag offsets if rectangle has changed shape
	virtual wxPoint CalcDragOffset(wxRect DragRect, wxPoint CursorPos, BOOL ToLeft, BOOL Below);
	virtual void CalcDragOffsetDirection(DockBarType BarType, wxPoint CursorPos);

	BOOL SimpleIntersect( const wxRect &rect1, const wxRect &rrect2 );


	// This function returns back a rectangle defining the shape of the DlgBar if it was dropped 
	// in the specified docking bar. 
	virtual wxRect GetDlgBarRect(DockBarType DockBar, wxRect CurrentBarRect); 

	// Used to inform ControlHelper that controls are about to die
	void InformControlsDead(); 

	// Used to copy or load resource controls
	BOOL CopyOrLoadControls(wxWindow* BarHwnd);

	// Loads the operation's resource controls into the window
	BOOL LoadResourceControls(); 

	// Creates tool bitmap button control
	BOOL CreateToolBtn(BarToolButton* pToolBtn);

	// Position and Format controls
	virtual BOOL PositionControls(BOOL DoRedraw = TRUE);

	HINSTANCE GetModuleInst(const BarControlInfo*pBarCtlInfo,UINT32 * ToolModuleID );
	
	DLGITEMTEMPLATE* GetPointerToControl(DLGTEMPLATE* pDlg ,UINT32 CtlId);
	
	// Creates a control from a resource

	BOOL CreateControlFromResource(BarControl* pBarItem);
	
	BOOL CreateInfoControlFromResource(BarControl* pBarItem,BOOL Init);

	// CreateControlFromResource helper which actually creates the control
	BOOL CreateControlItem(DLGITEMTEMPLATE * pCurrentControl, 
						   BarControl* pBarItem, 
						   HINSTANCE ModuleInst,
						   UINT32 ToolModuleID);

	// Format this bar according to its dock type
	virtual BOOL FormatBar(BOOL MoveWindows,ForceControlSize Force);

	// Format a Floating bar from the kernel BarItem list
	virtual BOOL FormatFloatingBar(BOOL MoveWindows,ForceControlSize Force);

	// Format a Horizontal bar from the kernel BarItem list
	virtual BOOL FormatHorizontalBar(BOOL MoveWindows,ForceControlSize Force);

   	// Format a Vertical bar from the kernel BarItem list
	virtual BOOL FormatVerticalBar(BOOL MoveWindows,ForceControlSize Force);

	// tidy up line feeds after control drag

	BOOL TidyBarItemsAfterDrag();

	// Format LineFeeds in a Floating Bar
 	BOOL FormatLineFeeds(INT32 Targetx,wxSize * pBarSize = NULL,INT32 * pNoLfs = NULL);

	// Return Current (Slot / Offset ) (Floating x / y) position
	KernelBarPos  * GetBarPosition();
 	
	// Return  Parent  DockBarType
	DockBarType GetDockBarType();

	// Return TRUE if dock has switched vertical/horizontal
	BOOL HasOrientationChanged(DockBarType Old,DockBarType New);
	
	// Swap cursors during drag cut/copy
	static BOOL  SwapCursors(DWORD CursorID);

	// Size the actual window
	virtual wxRect SetBarSize(DockBarType BarType,BOOL DoRedraw = TRUE);

	// Adjust the size of a bar post format
	virtual void AdjustFormattedBarSize(wxSize* BarSize);

	// Create any extra controls
	virtual INT32 CreateExtraControls();

	// Position any extra controls
	virtual void PositionExtraControls(wxSize BarSize, INT32 xpos, INT32 ypos);

	// Cache the data we need to improve vertical resizing
	BOOL CacheFormatingData();
	BOOL GetBestCachedWidth(INT32 TargetHeight, INT32 * FoundWidth);

	// set status line text
	virtual BOOL SetStatusLineText();

	// get Status line text in a buffer
	virtual BOOL GetStatusText(String_256 * StatusTextBuf);
	 
	// Resource utility fns
	LPWSTR MovePastWideStr(LPWSTR pWideStr);
	size_t SizeDlgHeader(DLGTEMPLATE *pHeader); 
	size_t SizeCtrlData(DLGITEMTEMPLATE *pControl);



	// -------------------------------------------------------------------------------------
	
	static BaseBar * CurrentBar;

	// We need to store a pointer to the operation so we can inform it when its window changes
	DialogBarOp* pOp; 

	// The docking bar on which the bar is currently placed
	DockBarType ParentDockBarType;

	// The docking bar the dialog bar is over during a drag
	DockBarType DragDockBarType; 
	
	// The docking bar the dialog bar was over last
	DockBarType OldDragBarType;

	DockBarType LastFixedDragType;

	// Actual parent of this bar
	OILDockingBar* ParentDockBar;

	// Used when painting
	wxPaintDC *PaintDC;

	// used to create drag brush
	wxBitmap BrushBitmap ;

	// The current dialog bar mode DIALOG/BAR
	BaseBarMode CurrentDlgBarMode;
	
	wxRect ScreenRect;

	// Rects for Drop Hit Testing
	wxRect TopRect;
	wxRect LeftRect;
	wxRect RightRect;
	wxRect BottomRect;


	// The drag cursors
	static Cursor*  pCurrentCursor;
	static Cursor*  pButtonCopyCursor;
	static Cursor*  pButtonCutCursor;
	static INT32		CurrentCursorID;
	static INT32		BorderWidth ;
	static INT32		TitleBarHeight ;


	// When the DialogBar is in dialog mode the following rectangles specify the dialogs active 
	// regions. They are used for hit detection.  


	wxRect SysMenuRect;
	wxRect TopBorderRect;
	wxRect BottomBorderRect;
	wxRect LeftBorderRect;
	wxRect RightBorderRect;
	wxRect CaptionBarRect;
	wxRect TopLeftGrowRect;
	wxRect BottomLeftGrowRect;
	wxRect TopRightGrowRect;
	wxRect BottomRightGrowRect;

	//current separator width

	SeparatorWidths SeparatorWidth;

	//// The Current System Frame Width
	//INT32 BorderWidth;


	INT32 HeightEstimate;

		// used during resize floatingbar operations
	double WidestFloat;
	double TallestFloat;

	// Used to record x values that will cause a new floating format
	List FormatPositions ;

	wxRect DragStartRect;

	static DlgDragType DragType;  // Set to NONE when no drag is taking place

	static BOOL InRecreate;

	static BOOL DoingErrorBox;		// Used so error boxes created when galleries are opening aren't fatal

	static BOOL ControlHasFocus;
	
	static wxWindow* HwndWithFocus ;

	static BOOL GalleryHasFocus ;

	// NEW BUTTON STUFF
	static UINT32 GalButtonWidth;


	// use this to switch background rendering back on after a drag if needed
	static BOOL IsBackGroundRendering;

	BOOL ListBoxHasFocus;
	
	// mouse position at start of drag
	wxPoint StartDragPoint ;
	
	// DragOffset used when moving window
	wxPoint	DragOffset;   
	BOOL	DragOffsetFromTop;
	BOOL	DragOffsetFromLeft;

	KernelBarPos LastSmallBarPos;

	// Offset used in resize
	wxPoint BorderOffset;

	static wxRect OldDragRect; 

	// Cached Format Rects
    wxRect FloatSmallRect;
	wxRect HorizontalSmallRect;
	wxRect VerticalSmallRect;
	wxRect FloatLargeRect;
	wxRect HorizontalLargeRect;
	wxRect VerticalLargeRect;

PORTNOTE("other", "removed fonts from bars")
#ifndef EXCLUDE_FROM_XARALX
	// Font to paint caption
	wxFont * TitleFont;

 	// Font to use in bars
	wxFont * BarFont;

	// Fonts to use in double decker controls
	wxFont * SmallThinFont;
	wxFont * LargeThinFont;
#endif

	// Is Bar an InfoBar 
	BOOL ThisIsAnInfoBar ;


	// Is Bar painted Active
	BOOL Active;

	static wxWindow * LastWindowWithFocus;

 	// The following windows objects are cached when a drag starts and released 
	// when it ends. 

	wxDC* pDisplayDC; 
	INT32 OldDrawMode; 
	wxBrush * pOldBrush; 
	wxBrush * pDragBrush;


	// used to track DDecker state during Bar creation
	DDeckerReadState DDeckerState;

	DDeckerReadState IsDoubleDecker(BarItem * ThisControl,BarItem * Start,BarItem * End);

	// Control Dragging stuff

	static wxRect 	LastCtlDragRect;
	wxRect	DragCtlClientRect;
	wxWindow*  	DragCtlHwnd;
	BOOL 	IsDraggingControl;
	wxPoint	DragCtlOffset;
	static ControlDragMode CtlDragMode;
	static ControlDragState CtlDragState;

	static BaseBar* PtrToToolBar;

public:

	// This returns TRUE if we are currently dragging a bar or a gallery around.
	static BOOL IsDragging() { return DragType != 0; /* DlgDragType::NONE */ }
};

#endif
												
