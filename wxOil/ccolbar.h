// $Id: ccolbar.h 669 2006-03-15 12:22:15Z gerry $
// CColBar.h - The CColourBar class - A status-bar like colour strip bar thing
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


#ifndef INC_CCOLBAR
#define INC_CCOLBAR

#include "collist.h"
#include "doccolor.h"
#include "gbrush.h"
#include "pump.h"
#include "dragtrgt.h"

class CColourBar;

/*********************************************************************************************

>	class 	EditButtonDragTarget : public OilDragTarget

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>.
	Created:	2/1/95

	Purpose:	An instantiation of this class is created by each entity which wishes to 
				provide a 'destination' to which the mouse can go to complete a drag.
				WinoilDragTargets are used as a base class from which to derive drag targets
				for WINOIL entities (an optional wxWindow*)

	Notes:		Drag targets are destructed automatically when a drag finishes by the drag
				manager with which they were registered.

				To remove a drag target at any time, destruct it - it automatically deregisters
				and cleans up.
				
	SeeAlso:	DragManagerOp::StartDrag; DragInformation; DragTarget

	Documentation:	Docs\Specs\DragMan.doc

*********************************************************************************************/

class EditButtonDragTarget : public OilDragTarget
{
friend class DragManagerOp;
	
CC_DECLARE_DYNAMIC(EditButtonDragTarget)

public:	// Public interface
	EditButtonDragTarget(wxWindow* TheWindow, wxRect *ClientArea = NULL);
	
	virtual UINT32 GetCursorID();
	virtual BOOL GetStatusLineText(String_256 * TheText);

protected:
	// Process a drag-related event
	BOOL ProcessEvent(DragEventType Event,
						DragInformation *pDragInfo,
						wxPoint *pMousePos, KeyPress* pKeyPress);

	// Internal helper function
	BOOL CanDropHere(DragInformation *pDragInfo);
};





/*********************************************************************************************

>	class 	ColourLineDragTarget : public OilDragTarget

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/11/95

	Purpose:	An instantiation of this class is created by each entity which wishes to 
				provide a 'destination' to which the mouse can go to complete a drag.
				WinoilDragTargets are used as a base class from which to derive drag targets
				for WINOIL entities (an optional wxWindow*)

	Notes:		Drag targets are destructed automatically when a drag finishes by the drag
				manager with which they were registered.

				To remove a drag target at any time, destruct it - it automatically deregisters
				and cleans up.
				
	SeeAlso:	DragManagerOp::StartDrag; DragInformation; DragTarget

	Documentation:	Docs\Specs\DragMan.doc

*********************************************************************************************/

class ColourLineDragTarget : public OilDragTarget
{
friend class DragManagerOp;
	
CC_DECLARE_DYNAMIC(ColourLineDragTarget)

public:	// Public interface
	ColourLineDragTarget(wxWindow* TheWindow, wxRect *ClientArea = NULL);

	virtual UINT32 GetCursorID();
	virtual BOOL GetStatusLineText(String_256 * TheText);

protected:
	// Process a drag-related event
	BOOL ProcessEvent(DragEventType Event,
						DragInformation *pDragInfo,
						wxPoint *pMousePos, KeyPress* pKeyPress);
};





/********************************************************************************************

>	class ColourBarMsgHandler : public MessageHandler

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/05/94
	Purpose:	Intercepts messages for the Colour Bar
				(Keeps the Colourabout which document is selected)
				
********************************************************************************************/

class ColourBarMsgHandler : public MessageHandler
{
friend class CCColorBar;
public:
	CC_DECLARE_DYNAMIC( ColourBarMsgHandler ) 
	ColourBarMsgHandler();					// Constructor

protected:
	MsgResult Message(Msg* Msg);			// Overridden message reciever
};




// Colour Bar drag information storage:
// Drags occur when dragging the scroll bar, and for implementing auto-repeat on buttons
typedef struct
{
	BOOL	MouseCaptured; 			// TRUE if we have captured the mouse (are dragging)
	UINT32	TimerOn;				// 0 or a timer ID if we have a timer event pending
	wxPoint	LastMousePos;			// The last recorded mouse position
	INT32	DragItem;  				// The cell/button/area in which the drag started
	UINT32	AnchorOffset;			// Offset of drag anchor point (Pixel/CellIndex)
	BOOL	Adjust;					// This is an 'adjust' drag
} CBDragInfo;


typedef enum						// Colour types (for drawing 'diamond' indicators)
{
	CBCOLOUR_LINE,					// Line colour
	CBCOLOUR_FILL,					// FlatFill or GradStart colour
	CBCOLOUR_END,					// Grad end colour

	CBCOLOUR_START					// Grad start colour - NOTE that normally we only use
									// line/fill/end, except when drawing diamonds in which
									// case Fill means full diamond while Start means a
									// up-arrow half-diamond shape.
} CBColourType;


// Forward declarations
class Cursor;
class AttrFillGeometry;
class AttrStrokeColour;
class ColourSGallery;
class SGDisplayLibColour;

/********************************************************************************

>	class CColourBar : public CControlBar

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/6/94
	Purpose:	The CColourBar window - a window like the status bar,
				which contains a strip of colours that can be used in the
				selected Document.

 ********************************************************************************/

// Hierarchy:  wxWindow -> CColourBar


class CColourBar : public wxWindow
{
	DECLARE_DYNAMIC_CLASS(CColourBar)

friend class ColourBarMsgHandler;
friend class ColourDragInformation;
friend class EditButtonDragTarget;
friend class ColourLineDragTarget;

public:				// Construction/destruction and kernel interface
	CColourBar();
	virtual ~CColourBar();

	BOOL Create(wxWindow* parent, UINT32 id);

	virtual wxSize DoGetBestSize() const;

//	void OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHandler);

	// winoil\gbrush.cpp interface - force a redraw when gbrush is toggled on/off
	static void RedrawGBrushAreas(void);

	// Kernel interface methods
	void SetSizes(INT32 NewCellSize, INT32 NewScrollbarSize);	// To set the colour cell/scrollbar sizes
	static void Show(BOOL HideOrShow = FALSE);		  		// To hide/show the window
	static void SetDisplayMode(INT32 NewDisplayMode = -1);	// To set new display mode

	static BOOL GetStatusLineText(String_256 *Result);		// To get status line help text

	static CColourBar* GetColourBar() { return(TheColourBar); }

protected:			// Windows OIL message handlers and related functions
	void DoPaint(wxDC* pDC);

	void OnPaint(wxPaintEvent &event);

	void OnSize(wxSizeEvent &event);
//	LRESULT OnSizeParent(WPARAM, LPARAM lParam);
	void OnLButtonDown(wxMouseEvent& event);
	void OnRButtonDown(wxMouseEvent& event);
	void OnLButtonDblClk(wxMouseEvent& event);
	void OnRButtonDblClk(wxMouseEvent& event);
	void OnLButtonUp(wxMouseEvent& event);
	void OnRButtonUp(wxMouseEvent& event);
	void OnMouseMove(wxMouseEvent& event);
	void OnTimer(wxTimerEvent& event);
//	void OnCancelMode(void);


protected:			// Internal data members
	static CColourBar *TheColourBar;	// The 1 and only instantiation of this class
	static BOOL BarIsVisible;			// TRUE if the bar thinks it is/should be visible

	INT32 m_cxLeftBorder;
	INT32 m_cyTopBorder;
	INT32 m_cxRightBorder;
	INT32 m_cyBottomBorder;

	UINT32 LeftmostColour;				// Index of leftmost colour in the display
	UINT32 TotalNumColours;				// Num of displayable cols in cur. col. list

	INT32 LastLineCell;					// Last cell we redrew line-colour diamond in
	INT32 LastFillCell;					// Last cell we redrew (start-grad-)fill-colour diamond in
	INT32 LastEndCell;					// Last cell we redrew end-grad-fill-colour diamond in
	BOOL LastDiamondShape;				// Last diamond shape (plus/diamond) drawn

	INT32 IndentedButton;				// CLICKED_NOTHING or id of currently indented button

	ColourList *m_pCurrentColourList;	// Current list of colours to display

	ColourBarMsgHandler *MsgHandler;

	wxRect WindowRect;					// whole redrawable window bounds
	wxRect StripRect;					// The colour strip
	wxRect IndicatorRect;				// The current colour indicator
	wxRect ScrollBarRect;				// The proportional scroll bar
	wxRect EditButtonRect;				// The 'edit colour' button
	wxRect NewButtonRect;				// The 'new colour' button

	INT32 OldBarHeight;					// The last known height of the bar window

	wxTimer m_DragTimer;

	ColourSGallery *m_pColourGallery;	// Current colour gallery storing fixed colour lists

public:
	INT32 GetNumberOfColours();
	
	static BOOL EnsureLibraryColoursPresent();

	// colour picker custom control interface functions ....
	
	BOOL IsColourPickerOverStripRect (wxWindow* colourPicker, wxPoint mousePt);
	BOOL DoColourPickerColour (wxWindow* colourPicker, wxPoint mousePt);

protected:			// Internal subroutine functions
	BOOL HasNoScrollBar(INT32 DisplayedColours, INT32 MaxColours, wxRect *pStripRect = NULL);

	void CalculateRects(void);
	void CalculateRectsInternal(wxRect *pWindowRect,
							wxRect *pStripRect, wxRect *pIndicatorRect, wxRect *pScrollBarRect,
							wxRect *pEditButtonRect, wxRect *pNewButtonRect);

	BOOL CalculateSausageRect(wxRect *Result);
	void CalculateNoColourCellRect(wxRect *Rect);

	static INT32 GetCurrentBarHeight(void);
	INT32  CalculateNewBarHeight(wxRect *AvailableRect);

	INT32  CalculateColourCellIndex(IndexedColour *TheColour);
	INT32  CalculateColourCellIndex(DocColour *TheColour);

	void OnAnyButtonUp(const wxPoint &point);
	void OnAnyButtonDown(const wxPoint &point, INT32 Modifier);
	void OnAnyButtonDblClk(wxPoint point, INT32 Modifier);
	void EditAColour(ColourList *DisplayList, IndexedColour *TheColour, BOOL LineColour = FALSE);

	INT32  WhereIsMouse(wxPoint MousePos, wxRect *TheRectItIsIn = NULL, BOOL *ToLeft = NULL);
	void ApplyColour(IndexedColour *TheColour, BOOL SetLineColour);
	void CellClicked(INT32 CellIndex, BOOL SetLineColour);
	static TCHAR *HelpCallbackHandler(wxWindow* Window, INT32 Item, void *This);

	// This returns the nth document colour
	IndexedColour *FindColourByIndex(UINT32 Index);

	// This is the equivalent to return the nth library colour
	// Index should include the document colours as these will be removed
	DocColour * FindLibColourByIndex(UINT32 Index, SGDisplayLibColour **ppLibColour = NULL);
	IndexedColour * FindLibIndexedColourByIndex(UINT32 Index);

protected:			// Internal redraw handlers
	void PaintColourCell(wxDC *pDC, wxRect *Rect, DocColour *RectCol);
	void PaintNoColourCell(wxDC *pDC);
	void PaintIndicatorDiamond(wxDC *pDC, const wxRect &CellRect, CBColourType ColourType,
								INT32 CellIndex, BOOL DiamondType);
	void PaintColourStrip(wxDC *pDC, const wxRect &ClipRect);
	void PaintIndicators(wxDC *pDC);
	void PaintEditOrNewButton(wxDC *pDC, BOOL Normal, BOOL IsEditButton);
	void PaintScrollButton(wxDC *pDC, INT32 ButtonID, BOOL Normal);
	void PaintScrollBar(wxDC *pDC);
	void PaintLibColourCell(wxDC *pDC, wxRect *Rect, DocColour *RectCol, BOOL SpotColour = FALSE);

	void ForceRedrawOfCell(INT32 CellIndex);
	void ForceRedrawOfCell(DocColour *TheColour);
	void ForceRedrawOfRect(const wxRect &RectToDraw);
	void ScrollTheStrip(INT32 ScrollBy, BOOL RedrawAlways = FALSE);


protected:			// Drag/AutoRepeat support functions
	CBDragInfo DragInfo;

	void CaptureTheMouse(void);
	void SetATimer(BOOL Delay);
	void ReleaseAllClaims(void);

	BOOL InitPrefs(void);


protected:			// Push-mode stuff
	Cursor *PushCursor;


protected:			// GDraw Brush stuff
	GBrush GDrawBrush;

	wxBrush *GetColourBrush(COLORREF rgb);


protected:			// Interface for friend class ColourBarMsgHandler
	static void PaletteHasChanged(ColourList *ChangedList = NULL);
	static void SelectionHasChanged(void);

	void SelectionHasChangedInternal(void);

	DECLARE_EVENT_TABLE()
};


#endif 


