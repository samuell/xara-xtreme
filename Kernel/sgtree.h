// $Id: sgtree.h 662 2006-03-14 21:31:49Z alex $
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
// SGTree.h - super gallery DisplayTree classes

#ifndef INC_SGTREE
#define INC_SGTREE


#ifndef INC_DOCCOLOR
#include "doccolor.h"
#endif



// Pre-declarations
class Document;			// defined in document.h
class Library;			// defined in sgscan.h

class SuperGallery;

class SGDisplayNode;
class SGDisplayGroup;	// defined below, but forward referenced
class SGDisplayItem;

struct ReDrawInfoType;	// kernel-rendered-dialogue information struct
class DialogColourInfo;

class DragMessage;		// Drag message class (see dragmsg.h)
class ThumbMessage;		// Drag message class (see thumbmsg.h)


	// Some suggested constants for sizing items in your gallery, in MILLIPOINTS
	// Do not change these constants, as they may affect multiple galleries.
const INT32 SG_InfiniteWidth		=      0;	// Infinite width item- DO NOT CHANGE!

const INT32 SG_DefaultLargeIcon	=  16000;	// Size of a large icon
const INT32 SG_DefaultSmallIcon	=  12000;	// Size of a small icon
const INT32 SG_GapBeforeText		=   3000;	// Horz gap between icon and start of text
const INT32 SG_GapAboveText		=  14400;	// Gap above text (when underneath the icon)
const INT32 SG_DefaultNameText	= 100000;	// Space for brief text (e.g. item name)


const INT32 DefaultGroupHeight	= 12000;	// Default millipoint height of groups
const INT32 DefaultGroupWidth	=     0;	// Default width - infinite

const INT32 DefaultItemHeight	= 12000;	// Default millipoint height of items
const INT32 DefaultItemWidth		= 30000;	// Default width

const INT32 InterLineGap 		=  1200;	// Gap between items in a group
const INT32 UpTreeGap			=  3000;	// Gap after a group's items
const INT32 IndentWidth			= 10000;	// Size of indent for successive tree levels





/***********************************************************************************************

<	SGFormatInfo

	Comment:	MonoOn
				typedef struct
				{
					INT32 MaxWidth;
					INT32 WindowHeight
					INT32  DisplayMode;
					INT32 PixelSize;
				} SGMiscInfo;
				MonoOff

				This structure is passed into SGDisplayNode class HandleEvent method.
				It is ALWAYS passed in, and is always valid.

				MaxWidth gives the width of the window display in millipoints. If items are
				small enough this width may allow several items to format next to each other
				along a single line.

				WindowHeight gives the height of the window display in millipoints. This is
				mainly only needed for calculating scrollbar information.

				DisplayMode indicates which display mode (e.g. Large/Small icons, Full info)
				the items should be displaying themselves in.

				PixelSize holds the size in millipoints of a display pixel in our virtual
				rendering coordinates. We must grid-lock all our rendering to multiples of
				this size to ensure that nasty aliasing effects (items jumping further apart
				as we scroll the list, etc) do not occur. It also means we can specify gaps
				and line widths, etc, in terms of "a pixel" we so desire.

				A structure is used for easy addition of other information at a later date

	SeeAlso:	SuperGallery; SGDisplayNode::HandleEvent

***********************************************************************************************/

typedef struct
{
	INT32	MaxWidth;			// The maximum available width, in millipoints (Window Width)
	INT32	WindowHeight;		// The height in millipoints of the region displayed onscreen
	INT32		DisplayMode;		// The display mode to be reformatted for
	INT32	PixelSize;			// Size of an output pixel in millipoints. We MUST gridlock
								// all plotting to multiples of this size.
} SGMiscInfo;



/***********************************************************************************************

<	SGSortKey

	Comment:	This structure is passed into SGDisplayNode classes when a sort operation
				is being applied. It describes how items should be sorted.

				MonoOn
				const INT32 MaxSGSortKeys		= 2;

				typedef struct
				{
					INT32		SortKey;
					BOOL	Reversed;
				} SGSortKey;


				const INT32 SGSORTKEY_NONE	= 0;

				const INT32 SGSORTKEY_BYNAME	= 1;
				MonoOff

				SortKeys are ALWAYS used in arrays of MaxSGSortKeys length.
				i.e. SGSortKey SortInfo[MaxSGSortKeys];

				Each entry in the SortKey array gives a sort key, specified as as an
				integer, where 0 means 'no sorting', and other integers are defined
				by the specific gallery/display item. By default, the value 1 is used
				to mean 'sort by name', and default gallery handlers are provided by
				the base classes to provide this sort mode.

				Each entry also has a Reversed flag which indicates whether the related
				sort mode should be used in reverse.

				The MaxSGSortKeys constant is used to allow us to add or remove extra
				sort keys at will. All references to sort keys should be made by loops
				which scan from 0 to MaxSGSortkeys-1.

	SeeAlso:	SuperGallery; SGDisplayNode::AddItem

***********************************************************************************************/

const INT32 MaxSGSortKeys = 2;			// We allow a primary and secondary key

const INT32 SGSORTKEY_NONE	= 0;		// Sort-key value for no-sorting
const INT32 SGSORTKEY_BYNAME	= 1;		// Sort key value for sort-by-name, if supplied

typedef struct
{
	INT32		SortKey;
	BOOL	Reversed;
} SGSortKey;



/***********************************************************************************************

<	SGFormatInfo

	Comment:	MonoOn
				typedef struct
				{
					INT32	LinePos;
					INT32	LineHeight;
					INT32	AvailableWidth;
					INT32		IndentLevel;
					DocRect	InvalidBounds;
				} SGFormatInfo;
				MonoOff
	
				This structure is passed into SGDisplayNode classes when the SuperGallery
				asks the tree to calculate and cache item formatting information.
				Items calculate formatting (into 'lines' across the window) using a simple
				but reasonably generic system, similar to foratting paragraphs of text.
				As many sibling items as possible will fit themselves side by side across
				each 'line' - overflow continues on the next line. Moving up or down
				the tree acts as a 'newline', forcing the next item to move to a new line.
				Generally speaking, leaf-nodes of the tree are the only nodes of non-
				'infinite' width (groups always fill entire lines), though there is no
				reason (other than style, taste, panache, and common decency) why they
				can't be shorter, and mix with other items on a single line.

				LinePos, LineHeight are kept up to date with the Y position and size of the
				currently formatting display 'line'.

				AvailableWidth contains the remaining width in this line - if an item cannot
				fit within AvailableWidth, it will format onto the next line (except when
				NeededWidth >= MaxWidth, when it takes the entire line for itself & truncates)

				IndentLevel indicates the current tree depth; used to indent items in sub-
				categories further each time.

				InvalidBounds is 'Empty' on entry to the formatting pass. As each item
				determines its format rect, it checks against its cached FormatRect, and
				if it has moved/resized since the last format, it includes its own y extent
				within the InvalidBounds rectangle. Thus, on exit, it is a rectangle which is
				either 'Empty' or encloses all invalid regions of the display list. Used
				to redraw only those portions of the list which have actually changed.

				A structure is used for easy addition of other information at a later date

	SeeAlso:	SuperGallery; SGDisplayNode::HandleEvent

***********************************************************************************************/

typedef struct
{
	INT32	LinePos;			// Topmost Y position of the current 'line'
								// (Y goes from 0 at the top to -extent at the bottom of the list)
	INT32	LineHeight;			// Minimum height of the current 'line' of the display
	INT32	AvailableWidth;		// Available width left on the current 'line' of the display
	INT32		IndentLevel;		// Current tree depth, used for adding indentation

	BOOL	AccumulateBounds;	// TRUE if the InvalidBounds should be accumulated, FALSE for speed
	SGDisplayNode *LastInvalidNode;	// If the previous node was invalid, this points to it. At any
								// other time, this should be NULL. This is to fix a problem where
								// the gap at the end of a group may not be redrawn, so the next
								// group has to include that gap in InvalidBounds on its behalf
	DocRect InvalidBounds;		// These bounds are accumulated during the format. Each item
								// which discovers its bounds have changed since the last format
								// updates this rectange to include itself. This gives a "changed
								// area" which needs to be redrawn to fix the display.

} SGFormatInfo;



/***********************************************************************************************

<	SGRedrawInfo

	Comment:	MonoOn
				typedef struct
				{
					DocRect Bounds
					RenderRegion *Renderer;
					BOOL BgRedraw;
					DocColour Foreground;
					DocColour Background;
					DocColour Transparent;
				} SGRedrawInfo;
				MonoOff

				This structure is passed down through the SuperGallery Display Tree by the
				SGDisplayNode::HandleEvent method, when processing REDRAW events. It includes
				the redraw clipping region which needs to be redrawn, plus a formatting
				information structure to allow the items to calculate their layout.

				Bounds indicates the 'clip rect' for the current redraw. Items outside
				Bounds should not be redrawn at all.

				Renderer points to a render region with which you can draw stuff

				Foreground, Background, Transparent, are predefined colours which should
				be used for the redraw.	These are read from host OS settings, so should
				be used in preference to constant colours where possible, in order to
				be as host-OS friendly as possible.
				
				A structure is used for easy addition of other information at a later date

	SeeAlso:	SuperGallery; SGDisplayNode::HandleEvent; SGReformatInfo

***********************************************************************************************/

typedef struct
{
	DocRect			Bounds;			// The bounds in which we are redrawing
	RenderRegion	*Renderer;		// The RndRgn to call to render stuff

									// Pre-defined colours
	DocColour		Foreground;		//   Foreground (black)
	DocColour		Background;		//   Background (white)
	DocColour		SelForeground;	//	 Foreground when selected (white)
	DocColour		SelBackground;	//	 Background when selected (dark blue)
	DocColour		Transparent;	//   Transparent (no colour)
} SGRedrawInfo;



/***********************************************************************************************

<	SGMouseInfo

	Comment:	MonoOn
				typedef struct
				{
					DocCoord	Position;
					BOOL		Adjust;
					BOOL		Extend;
					BOOL		DoubleClick;
					BOOL		MenuClick;
				} SGMouseInfo;
				MonoOff

				This structure is passed down through the SuperGallery Display Tree by the
				SGDisplayNode::HandleEvent method, when processing MOUSE events.

				Position contains the current x,y position of the mouse in MILLIPOINTS

				Adjust is TRUE for an 'adjust' (ctrl) click or FALSE for a normal
				(left button) click

				Extend is TRUE if it was a 'selection extending' click (shift-click in
				windows-listbox way of operation)

				DoubleClick is TRUE if this mouse event closely follows another one, so
				that it looks like this is actually part two of a double-click.

				MenuClick is TRUE if the click will be showing a menu (in which case, a
				drag MUST NOT be started or the menu will never appear) - this occurs for
				right-button clicks.

	SeeAlso:	SGDisplayGroup::HandleEvent; SGEventType

***********************************************************************************************/

typedef struct
{
	DocCoord	Position;
	BOOL		Adjust;
	BOOL		Extend;
	BOOL		DoubleClick;
	BOOL		MenuClick;
} SGMouseInfo;



/***********************************************************************************************

<	SGMouseInfo

	Comment:	MonoOn
				typedef struct
				{
					DocCoord		Position;
					INT32			ClosestSoFar;
					SGDisplayNode	*Claimant;
				} SGMouseInfo;
				MonoOff

				This structure is passed down through the SuperGallery Display Tree by the
				SGDisplayNode::HandleEvent method, when processing CLAIMPOINT events. This
				event allows the parent gallery to determine which item lies under the
				mouse pointer, etc (i.e. hit-testing for drags)

				Position contains the x,y position, in MILLIPOINTS, of a point to be
				hit-tested.

				ClosestSoFar records the distance from the nearest edge of the FormatRect
				of Claimant

				Any visible display node which contains the given point inside its
				FormatRect should set (Claimant = this), and return TRUE.

				Otherwise, if the node is closer to the point than 'ClosestSoFar' then
				they should set ClosestSoFar to this distance, set Claimant = this,
				and return FALSE to allow the rest of the tree to hit test the point.

	SeeAlso:	SGDisplayGroup::HandleEvent; SGEventType

***********************************************************************************************/

typedef struct
{
	DocCoord		Position;
	INT32			ClosestSoFar;
	SGDisplayNode	*Claimant;
} SGClaimPointInfo;



/***********************************************************************************************

<	SGEventInfo

	Comment:	Used by SuperGallery DisplayTrees

				This would ideally be a union of all data structures that can be passed into
				the DisplayTree HandleEvent methods. However, you can't easily do unions of
				things containing classes with copy constructors, so it isn't a union.
				Instead, the HandleEvent function has to cast this pointer to the appropriate
				type for the event being processed. These are:


				'NULL' indicates that there is no specific information passed in, and this
				parameter of the HandleEvent method will be NULL for these event types.

	SeeAlso:	SuperGallery; SGDisplayNode::HandleEvent; SGEventType

***********************************************************************************************/

typedef void *SGEventInfo;



/***********************************************************************************************

<	SGEventType

	Comment:	Used by SuperGallery DisplayTrees

				This is an enumeration of event types which can be passed to SGDisplayNodes
				to be handled by their HandleEvent methods. Along with this, a 'SGMiscInfo'
				structure is always passed in containing relevant information, as well as
				extra data which is event-specific:

				The data passed in SGDisplayNode::HandleEvent's 'EventInfo' pointer is 
				determined by the event type. The data will be:

				MonoOn
				SGEVENT_FORMAT		NULL
				SGEVENT_REDRAW		(SGRedrawInfo *)
				[SGEVENT_BGREDRAW	NULL] -- This event has been replaced by a virtual method
				SGEVENT_BGFLUSH		NULL					- May have NULL MiscInfo
				SGEVENT_MOUSECLICK	(SGMouseInfo *)
				SGEVENT_DRAGSTARTED	(DragMessage *)
				SGEVENT_CLAIMPOINT	(SGMouseInfo *)
				SGEVENT_THUMBMSG	(ThumbMessage *)		- May have NULL MiscInfo
				MonoOff

				NULL indicates that for this event type, a NULL pointer is passed in the
				EventInfo parameter.

	SeeAlso:	SuperGallery; SGDisplayNode::HandleEvent; SGEventInfo

***********************************************************************************************/

typedef enum
{
	SGEVENT_FORMAT,			// A request to just format the item (a dummy redraw run)
	SGEVENT_REDRAW,			// A request to redraw a portion of the displayed list
//	SGEVENT_BGREDRAW,		// A request to handle background redraws (handled by base class)
	SGEVENT_BGFLUSH,		// A request to flush (forget) background redraws (handled by base class)
	SGEVENT_MOUSECLICK,		// A request to determine what action should be taken for a click
	SGEVENT_DRAGSTARTED,	// A DRAGSTARTED DragMessage has been recieved
	SGEVENT_CLAIMPOINT,		// Asks the item enclosing a given position to claim the event
	SGEVENT_THUMBMSG,		// Passes a library ThumbMessage onto all items in the tree
} SGEventType;





/***********************************************************************************************

<	SGDisplayFlags

	Comment:	This structure simply holds a bitfield of flags relevant to SuperGallery
				Display classes. Although some classes do not use some of the flags, they
				all share the same flag structure, so that it can be treated in a more
				generic manner - it's reasonably convenient, too.

	SeeAlso:	SuperGallery; SGDisplayGroup; SGDisplayList; SGDisplayItem

***********************************************************************************************/

typedef struct
{
	UINT32 Invisible			: 1;	// Set if this node/item is entirely invisible
	UINT32 ReadOnly			: 1;	// Set if this node/item is read-only
	UINT32 Modified			: 1;	// Set if this node/item has been modified
	UINT32 Selected			: 1;	// Set if this node/item is selected
	UINT32 CanSelect			: 1;	// Set if this node will allow itself to be selected
	UINT32 Folded				: 1;	// Set if this node is folded
	UINT32 RedrawPending		: 1;	// Set if this node is waiting for background redraw
	UINT32 Virtualised		: 1;	// Set if this node is waiting for background redraw

	UINT32 HandleEventCount	: 8;	// DEBUG counter- how many re-entrant calls to HandleEvent
											// are currently in progress. Used to detect "suicides"

	UINT32 Reserved			: 16;	// Reserved for future expansion (all bits default to 0)
} SGDisplayFlags;







/***********************************************************************************************

>	class SGDisplayNode : public CCobject

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/10/94
	Purpose:	This DisplayTree node type is used by the SuperGallery
				This is a virtual class from which all nodes in the DisplayTree are derived.

				Before using DisplayTrees, or deriving classes from this one, please read
				Docs\specs\GalLib.doc, which has a section on how these nodes function.

	SeeAlso:	SuperGallery; SGDisplayGroup; SGDisplayList

***********************************************************************************************/

class CCAPI SGDisplayNode : public CCObject
{                    
	CC_DECLARE_DYNAMIC(SGDisplayNode)

public:		// Construction/Destruction
	SGDisplayNode();
	~SGDisplayNode();

public:		// Tree navigation methods
	inline SGDisplayNode* GetParent() const;
	virtual SGDisplayNode* GetChild() const;
	inline SGDisplayNode* GetNext() const;
	inline SGDisplayNode* GetPrevious() const;
	virtual SuperGallery *GetParentGallery() const;
		// Finds the gallery whch 'owns' this node and the tree it is in


protected:
	virtual void SetChild(SGDisplayNode *NewChild);
		// Internal method for setting the child pointer in derived classes

	virtual void InsertInternal(SGDisplayNode *NodeToInsert,
								SGDisplayNode *PrevNode, SGDisplayNode *NextNode);
		// Internal generic function to insert a node/subtree


public:		// Tree generation methods
	virtual void AddItem(SGDisplayNode *NodeToInsert, SGSortKey *SortInfo = NULL);
		// Inserts the given node into this subtree. If SortInfo == NULL or
		// NodeToInsert is not an SGDisplayItem, it is added as the last child of
		// this node. Otherwise, it is inserted into the subtree of SGDisplayItems
		// at the point 'specified' by SortFunction

	virtual void InsertAfter(SGDisplayNode *NodeToInsert);
		// Inserts the given node as this node's next (right) sibling

	virtual void InsertBefore(SGDisplayNode *NodeToInsert);
		// Inserts the given node as this node's previous (left) sibling

	virtual void MoveAfter(SGDisplayNode *NodeToInsert);
		// MOVEs the given node from its current position to be this node's next sibling

	virtual void MoveBefore(SGDisplayNode *NodeToInsert);
		// MOVEs the given node from its current position to be this node's previous sibling
	

	virtual void RemoveFromTree(void);
		// Removes (de-links) this node from the tree; use prior to deleting

	virtual void DestroySubtree(BOOL IncludingThisNode = TRUE);
		// De-links and DELETEs the entire subtree from this node (inclusive) downwards
		// NOTE that this will allow you to delete the root node of a tree - Use with care!

	virtual SGDisplayGroup *FindSubtree(SuperGallery *ParentGal,
										Document *ParentDoc, Library *ParentLib);
		// Finds the first _SGDisplayGroup_ node in the subtree which has parents matching the
		// specified parameters. Used to find a dcument/library's subtree.

	virtual BOOL SetFoldedState(BOOL NewState, BOOL ForceRedraw = TRUE);
		// Folds/Unfolds the given node, returning TRUE if this call actually changed the state

	inline BOOL IsSelected(void);
	virtual void SetSelected(BOOL IsSelected = TRUE);
		// Handle setting/reading the selection state of a Node


public:		// Sorting support methods
	virtual INT32 CompareTo(SGDisplayNode *Other, INT32 SortKey);
			// Compares 'this' to 'Other' in a manner specified by 'SortKey', and returns
			// -1 (I am lesser), 0 (we are equal), or +1 (I am greater) 


public:		// Searching support methods
	virtual void GetNameText(String_256 *Result);
	virtual void GetFullInfoText(String_256 *Result);
		// Returns the name/full-info text for this item, to support simple searching
		// operations, and generic redraw methods for library items.

	virtual void GetKeyWords(String_256 *Result);
		// Returns the keyword string for this item, to support simple keyword searching
		// operations.

public:		// Help methods
	virtual BOOL GetBubbleHelp(DocCoord *MousePos, String_256 *Result);
		// Returns a bubble help string (or returns FALSE if it couldn't supply one)
	
	virtual BOOL GetStatusLineHelp(DocCoord *MousePos, String_256 *Result);
		// Returns a status-line help string (or returns FALSE if it couldn't)

		
public:		// Redraw/Layout-calculation/Click-handling
	virtual void StartRendering(SGRedrawInfo *RedrawInfo, SGMiscInfo *MiscInfo);
	virtual void StopRendering(SGRedrawInfo *RedrawInfo, SGMiscInfo *MiscInfo);
		// Methods which MUST be called by all derived node types before/after doing
		// any rendering

	inline SGFormatInfo	*GetFormatInfo(SGEventType EventType, void *EventInfo);
	inline SGRedrawInfo	*GetRedrawInfo(SGEventType EventType, void *EventInfo);
	inline SGMouseInfo	*GetMouseInfo(SGEventType EventType, void *EventInfo);
	inline DragMessage	*GetDragInfo(SGEventType EventType, void *EventInfo);
	inline SGClaimPointInfo	*GetClaimPointInfo(SGEventType EventType, void *EventInfo);
	inline ThumbMessage	*GetThumbMsgInfo(SGEventType EventType, void *EventInfo);
		// Get various info blocks from the 'eventinfo' pointer, with ERROR3 checking

	virtual	BOOL HandleEvent(SGEventType EventType, void *EventInfo,
							 SGMiscInfo *MiscInfo);
		// Requests that the node handles a given event (redraw, click, etc)

	virtual void DragWasReallyAClick(SGMouseInfo *Mouse, SGMiscInfo *MiscInfo);
		// Called by the drag system (SGListDragInfo - see sgdrag.h) when a drag
		// has turned out to be a simple click

	virtual void GetFormatRect(DocRect *LayoutRect);
		// Returns the rectangle position at which this node last formatted itself

	virtual void ForceRedrawOfMyself(BOOL bEraseBkg = TRUE);
		// Requests that the parent gallery forces this item's region of the screen to redraw

	virtual void ForceRedrawOfMyselfAndChildren(void);
		// Same as above, but redraws this item and all its children. Only useful for
		// SGDisplayGroup nodes (and may not work for other node types)

	virtual void SelectItems(BOOL SelectThem, BOOL Exclusive = FALSE,
						Document *ParentDocument = NULL, Library *ParentLibrary = NULL);
		// Selects/Deselects all items inside/outside a given doc/lib range

	virtual void SelectGroups(BOOL SelectThem, BOOL Exclusive,
						Document *ParentDocument, Library *ParentLibrary);
		// Selects/Deselects all groups inside/outside a given doc/lib range

	virtual void SelectRangeGroups(SGDisplayGroup *PrimeNode, SGDisplayGroup *AnchorNode);
		// Provides group range selection

	virtual void SelectRangeItems(SGDisplayItem *PrimeNode, SGDisplayItem *AnchorNode);
		// Provides item range selection

protected:		// Internal (derived class) helper methods
	virtual BOOL GiveEventToMyChildren(SGEventType EventType, void *EventInfo,
										 SGMiscInfo *MiscInfo);
		// Interface for derived classes: Used to pass events to all children
		// of a node (saves derived classes having to bother duplicating tree-scan code)

	virtual void NewLine(SGFormatInfo *FormatInfo, SGMiscInfo *MiscInfo);
		// Updates the FormatInfo structure ready for the start of a new line


	virtual void CalculateFormatRect(SGFormatInfo *FormatInfo, SGMiscInfo *MiscInfo,
										INT32 ItemWidth, INT32 ItemHeight);
		// Calculates the display rectangle which a node occupies


	BOOL IMustRedraw(SGRedrawInfo *RedrawInfo);
		// Determines if an item overlaps the redraw bounds and must therefore redraw itself


	inline INT32 DevicePixels(SGMiscInfo *MiscInfo, INT32 NumPixels);
		// Returns the size in millipoints that 'NumPixels' device pixels will be

	inline static INT32 GridLock(SGMiscInfo *MiscInfo, INT32 Coordinate);
		// Snaps a millipoint coordinate onto a device-pixel coordinate boundary

	static void GridLockRect(SGMiscInfo *MiscInfo, DocRect *Rect);
		// As GridLock, but does all 4 coordinates of the given rectangle in one go

	void DrawPlinth(SGRedrawInfo *RedrawInfo, SGMiscInfo *MiscInfo,
							DialogColourInfo *RedrawColours,
							DocRect *ButtonRect,
							BOOL Indented = FALSE,
							UINT32 GlyphResourceID = 0);
		// Draw Windows-95 style button plinth, with an optional bitmap glyph

	void DrawSelectionOutline(SGRedrawInfo *RedrawInfo, SGMiscInfo *MiscInfo,
								DocRect *BoundsRect, INT32 Width = 0);
		// Draws a (default: 2-pixel) wide black border just inside the given rectangle

	void DrawBitmap(RenderRegion *Renderer, DocRect *BoundsRect, UINT32 ResID);
		// Draw a kernel-bitmap-resource bitmap into the gallery window


protected:		// Derived class methods for background-redraw support
	virtual void RegisterForBGRedraw(void);
		// Registers this node, which will be called back later, to do background redraw

	virtual BOOL DoBGRedraw(SGMiscInfo *MiscInfo);
		// Causes an immediate redraw of this node if it is pending background redraw
		// Called automatically by the base class event handlers at appropriate times,
		// but can also be used if you want to force immediate redraw of a pending item.

	virtual void DeregisterForBGRedraw(void);
		// Ensures that this node is not registered for BG redraws. Used when flushing


	virtual BOOL ShouldIDrawForeground(BOOL ForceForeground = FALSE);
		// Called when redrawing to know whether you should do the first pass or second of BG redraw
		// Rerturns TRUE if you should redraw the second pass (i.e. act as if in FG mode)

	static SGDisplayNode *CurrentBGRenderNode;
	static BOOL 		 BGRenderClaimed;


public:			// SuperGallery interface for doing a background redraw pass
	SGDisplayNode *DoBGRedrawPass(SGMiscInfo *MiscInfo);
		// Scans for the next node in the tree which should be background redrawn, redraws it, and
		// returns a pointer to be used on the next pass.

public:
	virtual BOOL DefaultPreDragHandler(SGMouseInfo *Mouse, SGMiscInfo *MiscInfo);
	virtual BOOL DefaultClickHandler(SGMouseInfo *Mouse, SGMiscInfo *MiscInfo,
										BOOL AfterDrag = FALSE, BOOL AdjustDoubleClick = TRUE);
		// Provides default handling of a click for selection purposes. Call this
		// after doing special things like detecting hits on buttons within items,
		// so that your list selects just like all the others.


public:			// Flags indicating the status of this node
	SGDisplayFlags Flags;
	

private:	// Private data
	SGDisplayNode *Parent;				// Tree links
	SGDisplayNode *Next;
	SGDisplayNode *Previous;
	// (Note: Child links are special cased in Root/Group to reduce memory usage in display items)

protected:
	DocRect FormatRect;					// The cached rect of where we last formatted to
	static BOOL BkgEraseMode;		// Flag indicating whether the background is erased on redrawing   			

#ifdef _DEBUG
public:
	virtual void DumpSubtree(INT32 TreeLevel = 1);	// Debugging - dumps out the (sub)tree to the debugger
#endif
};



/***********************************************************************************************

>	inline SGDisplayNode *SGDisplayNode::GetParent() const

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/10/94
	Returns:	A pointer to the resquested SGDisplayNode object, or NULL

	Purpose:	Finds the parent of this DisplayTree Node.
				Returns NULL if you have reached the boundary of the tree

	SeeAlso:	SuperGallery; SGDisplayNode::GetChild;
				SGDisplayNode::GetNext; SGDisplayNode::GetPrevious

***********************************************************************************************/

SGDisplayNode *SGDisplayNode::GetParent() const
{
	return Parent;
}



/***********************************************************************************************

>	inline SGDisplayNode *SGDisplayNode::GetNext() const

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/10/94
	Returns:	A pointer to the resquested SGDisplayNode object, or NULL

	Purpose:	Finds the next (right) sibling of this DisplayTree Node.
				Returns NULL if you have reached the boundary of the tree

	SeeAlso:	SuperGallery; SGDisplayNode::GetParent; SGDisplayNode::GetChild;
				SGDisplayNode::GetPrevious

***********************************************************************************************/

SGDisplayNode *SGDisplayNode::GetNext() const
{
	return Next;
}



/***********************************************************************************************

>	inline SGDisplayNode *SGDisplayNode::GetPrevious() const

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/10/94
	Returns:	A pointer to the resquested SGDisplayNode object, or NULL

	Purpose:	Finds the previous (left) sibling of this DisplayTree Node.
				Returns NULL if you have reached the boundary of the tree

	SeeAlso:	SuperGallery; SGDisplayNode::GetParent; SGDisplayNode::GetChild;
				SGDisplayNode::GetNext;

***********************************************************************************************/

SGDisplayNode *SGDisplayNode::GetPrevious() const
{
	return Previous;
}



/***********************************************************************************************

>	inline static INT32 SGDisplayNode::GridLock(SGMiscInfo *MiscInfo, INT32 Coordinate)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/10/94

	Inputs:		MiscInfo - A structure containing information relevant to gridlocking.
				This is passed into all HandleEvent calls.

				Coordinate - the coord to be snapped onto the pixel grid

	Purpose:	In order to avoid rounding errors in the mapping between millipoints and
				output device pixels from causing coordinates to alias to different pixels
				when in different positions, all SuperGallery display coordinates must be
				snapped onto a grid of pixel-positions. This function does this snapping.

	SeeAlso:	SGDisplayNode::GridLockRect; SGDisplayNode::DevicePixels

***********************************************************************************************/

INT32 SGDisplayNode::GridLock(SGMiscInfo *MiscInfo, INT32 Coordinate)
{
	return(Coordinate - (Coordinate % MiscInfo->PixelSize));
}



/***********************************************************************************************

>	inline INT32 SGDisplayNode::DevicePixels(SGMiscInfo *MiscInfo, INT32 NumPixels)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/10/94

	Inputs:		MiscInfo - A structure containing information relevant to gridlocking.
				This is passed into all HandleEvent calls.

				NumPixels - the number of 'pixels' you want

	Purpose:	Because we work in millipoints, it is difficult to get the right values
				for 'thin' lines (1-device pixel thick) or thin gaps (e.g. 1 or 2 pixels)
				This function uses the FormatInfo to determine the size, in millipoints, of
				device pixels, and multiplies this by the input parameter, to give the
				size in millipoints of that many device pixels.

	SeeAlso:	SGDisplayNode::GridLock; SGDisplayNode::GridLockRect

***********************************************************************************************/

INT32 SGDisplayNode::DevicePixels(SGMiscInfo *MiscInfo, INT32 NumPixels)
{
	return(NumPixels * MiscInfo->PixelSize);
}



/***********************************************************************************************

>	inline SGFormatInfo *SGDisplayNode::GetFormatInfo(SGEventType EventType, void *EventInfo)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/1/95

	Inputs:		EventType - The type of the current event
				EventInfo - TheEventInfo for the current event

	Purpose:	Extracts certain event information from the event information passed in
				to a SGDisplayNode::HandleEvent() method. Not only gets the info for you,
				but checks the validity (ERROR3's if you've asked for the wrong type of
				information for the current event type, and if the information is NULL)

***********************************************************************************************/

SGFormatInfo *SGDisplayNode::GetFormatInfo(SGEventType EventType, void *EventInfo)
{
	ERROR3IF(EventType != SGEVENT_FORMAT, "FormatInfo not available for this event type!");
	ERROR3IF(EventInfo == NULL, "EventInfo for this event was NULL!");
	return((SGFormatInfo *) EventInfo);
}


/***********************************************************************************************

>	inline SGRedrawInfo *SGDisplayNode::GetRedrawInfo(SGEventType EventType, void *EventInfo)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/1/95

	Inputs:		EventType - The type of the current event
				EventInfo - TheEventInfo for the current event

	Purpose:	Extracts certain event information from the event information passed in
				to a SGDisplayNode::HandleEvent() method. Not only gets the info for you,
				but checks the validity (ERROR3's if you've asked for the wrong type of
				information for the current event type, and if the information is NULL)

***********************************************************************************************/

SGRedrawInfo *SGDisplayNode::GetRedrawInfo(SGEventType EventType, void *EventInfo)
{
	ERROR3IF(EventType != SGEVENT_REDRAW, "RedrawInfo not available for this event type!");
	ERROR3IF(EventInfo == NULL, "EventInfo for this event was NULL!");
	return((SGRedrawInfo *) EventInfo);
}


/***********************************************************************************************

>	inline SGMouseInfo *SGDisplayNode::GetMouseInfo(SGEventType EventType, void *EventInfo)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/1/95

	Inputs:		EventType - The type of the current event
				EventInfo - TheEventInfo for the current event

	Purpose:	Extracts certain event information from the event information passed in
				to a SGDisplayNode::HandleEvent() method. Not only gets the info for you,
				but checks the validity (ERROR3's if you've asked for the wrong type of
				information for the current event type, and if the information is NULL)

***********************************************************************************************/

SGMouseInfo *SGDisplayNode::GetMouseInfo(SGEventType EventType, void *EventInfo)
{
	ERROR3IF(EventType != SGEVENT_MOUSECLICK, "MouseInfo not available for this event type!");
	ERROR3IF(EventInfo == NULL, "EventInfo for this event was NULL!");
	return((SGMouseInfo *) EventInfo);
}



/***********************************************************************************************

>	inline DragMessage *SGDisplayNode::GetDragInfo(SGEventType EventType, void *EventInfo)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/2/95

	Inputs:		EventType - The type of the current event
				EventInfo - TheEventInfo for the current event

	Purpose:	Extracts certain event information from the event information passed in
				to a SGDisplayNode::HandleEvent() method. Not only gets the info for you,
				but checks the validity (ERROR3's if you've asked for the wrong type of
				information for the current event type, and if the information is NULL)

***********************************************************************************************/

DragMessage *SGDisplayNode::GetDragInfo(SGEventType EventType, void *EventInfo)
{
	ERROR3IF(EventType != SGEVENT_DRAGSTARTED, "DragInfo not available for this event type!");
	ERROR3IF(EventInfo == NULL, "EventInfo for this event was NULL!");
	return((DragMessage *) EventInfo);
}



/***********************************************************************************************

>	inline SGClaimPointInfo *SGDisplayNode::GetClaimPointInfo(SGEventType EventType,
																void *EventInfo)
	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/3/95

	Inputs:		EventType - The type of the current event
				EventInfo - TheEventInfo for the current event

	Purpose:	Extracts certain event information from the event information passed in
				to a SGDisplayNode::HandleEvent() method. Not only gets the info for you,
				but checks the validity (ERROR3's if you've asked for the wrong type of
				information for the current event type, and if the information is NULL)

***********************************************************************************************/

SGClaimPointInfo *SGDisplayNode::GetClaimPointInfo(SGEventType EventType, void *EventInfo)
{
	ERROR3IF(EventType != SGEVENT_CLAIMPOINT, "ClaimPointInfo not available for this event type!");
	ERROR3IF(EventInfo == NULL, "EventInfo for this event was NULL!");
	return((SGClaimPointInfo *) EventInfo);
}



/***********************************************************************************************

>	inline ThumbMessage *SGDisplayNode::GetThumbMsgInfo(SGEventType EventType,
																void *EventInfo)
	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/3/95

	Inputs:		EventType - The type of the current event
				EventInfo - TheEventInfo for the current event

	Purpose:	Extracts certain event information from the event information passed in
				to a SGDisplayNode::HandleEvent() method. Not only gets the info for you,
				but checks the validity (ERROR3's if you've asked for the wrong type of
				information for the current event type, and if the information is NULL)

***********************************************************************************************/

ThumbMessage *SGDisplayNode::GetThumbMsgInfo(SGEventType EventType, void *EventInfo)
{
	ERROR3IF(EventType != SGEVENT_THUMBMSG, "ThumbMsg info not available for this event type!");
	ERROR3IF(EventInfo == NULL, "EventInfo for this event was NULL!");
	return((ThumbMessage *) EventInfo);
}



/***********************************************************************************************

>	inline BOOL SGDisplayNode::IsSelected(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/10/94

	Purpose:	Returns TRUE if this item is selected, FALSE if it is not

	SeeAlso:	SGDisplayItem::SetSelected

***********************************************************************************************/

BOOL SGDisplayNode::IsSelected(void)
{
	return(Flags.Selected);
}








/***********************************************************************************************

>	class SGDisplayRoot : public SGDisplayNode

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/10/94
	Purpose:	This DisplayTree node type is used by the SuperGallery
				The top (root) level of the DisplayTree is made up of a single SGDisplayRoot
				node. This node is used as a single entry-point for all tree-wide actions
				which the SuperGallery applies to it (redraw, reformatting, etc)

				By default, the root node is completely invisible on the display, and is just
				used for internal tidiness. However, galleries could override this class to
				make a new root type (e.g. you could override the redraw of the entire tree
				in certain display modes, to give a "show one item at a time" mode, etc)

				If the root node has no children, it becomes visible, redrawing itself as
				a piece of grey text reading 'No items' or similar.

	SeeAlso:	SGDisplayRootScroll
	SeeAlso:	SuperGallery; SGDisplayNode; SGDisplayGroup; SGDisplayItem;

***********************************************************************************************/

class CCAPI SGDisplayRoot : public SGDisplayNode
{
	CC_DECLARE_DYNAMIC(SGDisplayRoot);

protected:
	SGDisplayRoot();							// Default constructor - DO NOT USE


public:
	SGDisplayRoot(SuperGallery *ParentGal);		// Proper constructor


public:		// Access methods
	virtual SGDisplayNode *GetChild(void) const;
		// Returns a pointer to this node's first child

protected:
	virtual void SetChild(SGDisplayNode *NewChild);
		// Internal method for setting the child pointer in derived classes

public:
	virtual SuperGallery *GetParentGallery(void) const;
		// Finds the gallery whch 'owns' this node and the tree it is in
		// Overridden, because Root nodes know this information

	// Override insert/remove calls - the root never dies, and cannot have siblings
	// Insertions simply call AddItem to add the node as the last child of the root node
	virtual void InsertAfter(SGDisplayNode *NodeToInsert);
	virtual void InsertBefore(SGDisplayNode *NodeToInsert);
	virtual void RemoveFromTree(void);
	virtual void DestroySubtree(BOOL IncludingThisNode = TRUE);

	virtual void InitFormatInfo(SGFormatInfo *FormatInfo, SGMiscInfo *MiscInfo);
		// Initialises the FormatInfo structure ready for passing to ProcessEvent methods

	virtual	BOOL HandleEvent(SGEventType EventType, void *EventInfo,
							 SGMiscInfo *MiscInfo);
		// Handles a display list event (redraw, mouse click, etc)

	virtual INT32 CalculateListExtent(SGFormatInfo *FormatInfo, SGMiscInfo *MiscInfo);
		// Calculates the extent the list would fill if it were to be redrawn now

	virtual INT32 GetCachedListExtent(void);
		// Retrieves the cached list extent. Only valid after a FORMAT has been done by the
		// tree (or a call to CalculateListExtent)

	virtual void SetScrollOffset(INT32 NewOffset, SGMiscInfo *MiscInfo);
	virtual INT32 GetScrollOffset(void);
		// Set and retrieve the scroll offsets - DOES NOTHING (return 0) in this base class!

	virtual void RedrawScrollBar(SGMiscInfo *MiscInfo);
		// Forces a redraw of the scroll bar area (if any)

	virtual SGDisplayItem *FindNextSelectedItem(SGDisplayNode *CurrentItem = NULL,
													BOOL *SkipGroup = NULL);
		// Scans the selection over the entire display tree

	virtual INT32 GetSelectedItemCount(void);
		// Counts the number of selected items in the entire display tree

	virtual SGDisplayGroup *FindNextSelectedGroup(SGDisplayNode *CurrentGroup = NULL);
		// Scans the group selection over the entire display tree

	virtual INT32 GetSelectedGroupCount(void);
		// Counts the number of selected groups in the entire display tree

	virtual void SelectRange(SGDisplayNode *PrimeNode, SGDisplayNode *AnchorNode);
		// this works out if we're selecting groups or nodes...

	static SGDisplayNode *FindNextItemInTree(SGDisplayNode *StartItem);
		// return the next item, skipping virtualised groups

protected:
	SGDisplayNode *Child;

	SuperGallery *ParentGallery;

	INT32 ScrollExtent;			// The (positive) Y size of the display list in its current form
};






// Indented button types for Scroll bar buttons
typedef enum
{
	IBUTTON_NONE,
	IBUTTON_UP,
	IBUTTON_DOWN
} IndentedButtonValue;


/***********************************************************************************************

>	class SGDisplayRootScroll : public SGDisplayRoot

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/1/95
	Purpose:	This DisplayTree node type is used by the SuperGallery

				It is identical to the DisplayRoot, except it provides a proportional scroll
				bar on the right edge of the display area, which automatically handles
				everything to do with scrolling.

				i.e. to get a gallery witha scrollbar, simply use this root node type, and
				everything just happens automatically for you.

	SeeAlso:	SuperGallery; SGDisplayRoot; SGDisplayNode; SGDisplayGroup; SGDisplayItem;

***********************************************************************************************/

class CCAPI SGDisplayRootScroll : public SGDisplayRoot
{
	CC_DECLARE_DYNAMIC(SGDisplayRootScroll);

public:
	SGDisplayRootScroll(SuperGallery *ParentGal);


public:		// Access methods
	virtual	BOOL HandleEvent(SGEventType EventType, void *EventInfo,
							 SGMiscInfo *MiscInfo);
		// Handles a display list event (redraw, mouse click, etc)

	virtual void SetScrollOffset(INT32 NewOffset, SGMiscInfo *MiscInfo);
	virtual INT32 GetScrollOffset(void);
		// Set and retrieve the scroll offsets

	virtual void RedrawScrollBar(SGMiscInfo *MiscInfo);
		// Forces a redraw of the scroll bar area (if any)

public:		// Access methods for SGScrollDragTarget class
	virtual BOOL CalculateScrollRects(SGMiscInfo *MiscInfo,
										BOOL Translate,
										DocRect *UpButton, DocRect *DownButton,
										DocRect *Sausage,  DocRect *PageUp = NULL,
										DocRect *PageDown = NULL, DocRect *ScrollRect = NULL);

	INT32 IndentedButton;		// An identifier as to which scroll button (if any) is indented


protected:
	virtual BOOL CalculateSausageRect(SGMiscInfo *MiscInfo,
										DocRect *ScrollBarRect, DocRect *Result);
		// Determines the position/size of the proportional scroll bar 'sausage'

private:
	INT32 ScrollOffset;		// The offset (positive) to the displayed portion of the list
};






/***********************************************************************************************

>	class SGDisplayGroup : public SGDisplayNode

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/10/94
	Purpose:	This DisplayTree node type is used by the SuperGallery
				The top level(s) of the DisplayTree are made up of a number of SGDisplayGroup
				nodes, which divide the tree up into categories (and, potentially hierarchical
				groups within categories).

				Groups have special properties, including a Title, which they will redraw,
				and pointers to their parent Document, Library, and SuperGallery.

	SeeAlso:	SuperGallery; SGDisplayNode; SGDisplayRoot; SGDisplayItem

***********************************************************************************************/

class CCAPI SGDisplayGroup : public SGDisplayNode
{
	CC_DECLARE_DYNAMIC(SGDisplayGroup);


public:
	SGDisplayGroup();
	SGDisplayGroup(SuperGallery *ParentGal,
					Document *ParentDoc = NULL, Library *ParentLib = NULL);

public:
	virtual SGDisplayNode* GetChild(void) const;
		// Returns a pointer to this node's first child

protected:
	virtual void SetChild(SGDisplayNode *NewChild);
		// Internal method for setting the child pointer in derived classes

public:
	inline	Document *GetParentDocument(void);
		// Returns NULL or the Document that 'owns' this subtree

	inline	Library *GetParentLibrary(void);
		// Returns NULL or the Library that 'owns' this subtree

	virtual SuperGallery *GetParentGallery(void) const;
		// Returns the SuperGallery that 'owns' this subtree

	virtual void DragWasReallyAClick(SGMouseInfo *Mouse, SGMiscInfo *MiscInfo);
		// Called by the drag system (SGListDragInfo - see sgdrag.h) when a drag
		// has turned out to be a simple click

	virtual	BOOL HandleEvent(SGEventType EventType, void *EventInfo,
							 SGMiscInfo *MiscInfo);
		// Handles a display list event (redraw, mouse click, etc)

	virtual void ForceRedrawOfMyselfAndChildren(void);
		// Overrides the default, to draw this node and all children thereof

	virtual SGDisplayItem *FindNextSelectedItem(SGDisplayItem *CurrentItem = NULL);
		// Finds the first and subsequent selected child item(s) of a group

	virtual INT32 GetSelectedItemCount(void);
		// Determines how many child items of this group are selected

	virtual void SelectItems(BOOL SelectThem, BOOL Exclusive = FALSE,
						Document *ParentDocument = NULL, Library *ParentLibrary = NULL);
		// Selects/Deselects all items inside/outside a given doc/lib range

	virtual void SelectGroups(BOOL SelectThem, BOOL Exclusive,
						Document *ParentDocument, Library *ParentLibrary);
		// Selects/Deselects all groups inside/outside a given doc/lib range

	virtual BOOL SetFoldedState(BOOL NewState, BOOL ForceRedraw = TRUE);
		// Folds/Unfolds the given node, returning TRUE if this call actually changed the state

	void GetChildArea(DocRect *Result);
		// Reads the group's child area (the bounding rectangle of the group's children) rectangle
		// If the group is folded (or has not been reformatted since folding) this is Empty


public:	// Functions for virtualisation of groups (building groups but not adding items until we need them)
	virtual BOOL CanVirtualise(void);
		// Can this group virtualise out to disk ?

	virtual BOOL Virtualise(void);
		// Virtualise out

	virtual BOOL DeVirtualise(void);
		// Virtualise back in

	void SetVirtualisedState(BOOL NewValue) {Flags.Virtualised = NewValue;};
		// Set the Virtualised variable

	BOOL IsVirtualised(void) {return Flags.Virtualised;};
		// Is this group virtualised out to disk ?

public:
	virtual BOOL DefaultPreDragHandler(SGMouseInfo *Mouse, SGMiscInfo *MiscInfo);
	virtual BOOL DefaultClickHandler(SGMouseInfo *Mouse, SGMiscInfo *MiscInfo, BOOL AfterDrag = FALSE);
		// Provides default handling of a click for selection purposes. Call this
		// after doing special things like detecting hits on buttons within items,
		// so that your list selects just like all the others.

protected:		// Internal handlers
	virtual void ReadGroupTitle(void);
		// (Re)Reads a group's title text from the parent doc/lib


protected:
	SGDisplayNode*  Child;

	String_64		TitleText;

	Document 		*ParentDocument;			// NULL or pointer to document this represents
	Library			*ParentLibrary;				// NULL or pointer to library this represents

	SuperGallery	*ParentGallery;				// Pointer to the SuperGallery who owns us

	DocRect			ChildArea;					// Rectangle containing my children
};



/***********************************************************************************************

>	inline Document *SGDisplayGroup::GetParentDocument(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/10/94

	Returns:	The parent document, or NULL

	Purpose:	Returns the document which this node (and its subtree) references

	SeeAlso:	SGDisplayGroup::GetParentGallery; SGDisplayGroup::GetParentLibrary

***********************************************************************************************/

Document *SGDisplayGroup::GetParentDocument(void)
{
	return(ParentDocument);
}



/***********************************************************************************************

>	inline Library *SGDisplayGroup::GetParentLibrary(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/10/94

	Returns:	The parent library, or NULL

	Purpose:	Returns the Library which this node (and its subtree) references

	SeeAlso:	SGDisplayGroup::GetParentGallery; SGDisplayGroup::GetParentDocument

***********************************************************************************************/

Library *SGDisplayGroup::GetParentLibrary(void)
{
	return(ParentLibrary);
}




/***********************************************************************************************

>	class SGDisplayItem : public SGDisplayNode

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/10/94
	Purpose:	This DisplayTree node is used by the SuperGallery
				It is responsible for providing the ability to redraw one item displayed in
				the gallery, and remembering information on that item (such as its selection
				state, etc).

				Generally, SGDisplayItems should only occur as leaf-nodes - this base class
				overrides the methods of SGDisplayNodes in order to discourage any attempt
				to make these nodes non-leaf-nodes.

				This class should be used to derive new DisplayItem classes from for each of
				the SuperGalleries. For example, the Froodle SuperGallery might provide two
				types of SGDisplayItem:
				MonoOn
					FroodleSGDisplayItem	(which holds a pointer to a Froodle in a document)
					LibFroodleSGDisplayItem	(which holds the information on a Froodle in an
											open FroodleLibrary file)
				MonoOff

	SeeAlso:	SuperGallery; SGDisplayNode; SGDisplayRoot; SGDisplayGroup

***********************************************************************************************/

class CCAPI SGDisplayItem : public SGDisplayNode
{
	CC_DECLARE_DYNAMIC(SGDisplayItem);

public:
	SGDisplayItem();

public:
	virtual	BOOL HandleEvent(SGEventType EventType, void *EventInfo,
							 SGMiscInfo *MiscInfo);
		// Handles a display list event (redraw, mouse click, etc)

public:
	virtual BOOL DefaultPreDragHandler(SGMouseInfo *Mouse, SGMiscInfo *MiscInfo);
	virtual BOOL DefaultClickHandler(SGMouseInfo *Mouse, SGMiscInfo *MiscInfo,
										BOOL AfterDrag = FALSE, BOOL AdjustDoubleClick = TRUE);
		// Provides default handling of a click for selection purposes. Call this
		// after doing special things like detecting hits on buttons within items,
		// so that your list selects just like all the others.

public:		// Overridden tree generation methods - disallow certain insertions
	virtual void RemoveFromTree(void);
		// Ensures the item is deselected and informs the parent gallery if necessary

	virtual void AddItem(SGDisplayNode *NodeToInsert, SGSortKey *SortInfo = NULL);
		// Inserts the given node into this subtree. ERRORS, as we can't have children!
};


#endif

