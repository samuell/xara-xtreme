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
// SGLayer.cpp - Layer SuperGallery classes - LayerSGallery and SGDisplayLayer


#include "camtypes.h"

// WEBSTER - markn 15/1/97
// Removed all code not needed in Webster
// This is the same as the code removed from RALPH
//#ifdef WEBSTER
//#define EXCLUDE_GALS
//#endif

#include "app.h"		// For GetApplication()
//#include "gallery.h"	// For _R(IDC_GALLERY_LISTBOX)
//#include "galres.h"
//#include "galstr.h"
#include "layergal.h"
#include "sgdrag.h"
#include "sginit.h"
#include "sglayer.h"
#include "sgmenu.h"

#include "ink.h"
#include "layer.h"
//#include "markn.h"
//#include "resource.h"
//#include "barsdlgs.h" 
//#include "simon.h"
#include "sprdmsg.h"
#include "spread.h"
#include "dragmgr.h"
#include "docvmsg.h"

#include "ccdc.h"		// For render-into-dialogue support
#include "fillval.h"
#include "grnddib.h"
#include "guides.h"
#include "docview.h"
#include "layerprp.h"
#include "layermsg.h"
//#include "richard2.h"
//#include "andy.h"
#include "sgframe.h"	// SGDisplayFrame
#include "aprps.h"
//#include "will2.h"		// for _R(IDS_LAYERGAL_GALLNAME)
//#include "sliceres.h"
#include "layermgr.h"

#include "helpuser.h"
#include "xshelpid.h"
//#include "helppath.h"

void Beep();

#ifndef EXCLUDE_GALS
// Implement the dynamic class bits...
CC_IMPLEMENT_DYNCREATE(LayerSGallery, SuperGallery)
CC_IMPLEMENT_DYNAMIC(SGLayerGroup, SGDisplayGroup)
CC_IMPLEMENT_DYNAMIC(SGDisplayLayer, SGDisplayItem)
#else
CC_IMPLEMENT_DYNCREATE(LayerSGallery, Operation)
#endif

// This line mustn't go before any CC_IMPLEMENT_... macros
#define new CAM_DEBUG_NEW



#ifdef _DEBUG
// DO_SNAP_BUTTON - set to (1) for a snap-button for each layer, (0) for no snap-button support
#define DO_SNAP_BUTTON (0)
#else
#define DO_SNAP_BUTTON (0)
#endif


#ifndef EXCLUDE_GALS

/***********************************************************************************************

>	SGLayerGroup::SGLayerGroup(SuperGallery *ParentGal,
					Document *ParentDoc = NULL, Library *ParentLib = NULL, BOOL IsForeground = TRUE);

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/10/94

	Inputs:		ParentGal - points to the SuperGallery object which 'owns' this node
				ParentDoc - NULL, or a pointer to the document this group references
				ParentLib - NULL, or a pointer to the library this group references

	Purpose:	SGLayerGroup constructor. Initialises the Group's parent pointers to
				point at its parent(s). Note that generally speaking, one of ParentDoc,
				ParentLib will be NULL, and the other will be non-NULL.

	Notes:		This constructor does nothing - jusat passes the call on to the baseclass
				constructor of identical proportions.

	SeeAlso:	SGDisplayGroup::SGDisplayGroup

***********************************************************************************************/

SGLayerGroup::SGLayerGroup(SuperGallery *ParentGal,
							Document *ParentDoc, Library *ParentLib,BOOL IsForeground)
				: SGDisplayGroup(ParentGal, ParentDoc, ParentLib)
{
	Foreground = IsForeground;
}



/***********************************************************************************************

>	virtual BOOL SGLayerGroup::HandleEvent(SGEventType EventType, void *EventInfo,
											 SGMiscInfo *MiscInfo)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/2/95

	Inputs:		See SGDisplayNode::HandleEvent

	Returns:	TRUE if the event was handled successfully
				FALSE if it was not

	Purpose:	Handles a SuperGallery DisplayTree event

	Notes:		This overrides the SGDisplayNGroup::HandleEvent method, to give a group-type
				thing which does wierd stuff for layer galleries (i.e. does not fold,
				and gives column titles in the 'group' display)

	SeeAlso:	SGDisplayGroup::HandleEvent

***********************************************************************************************/

BOOL SGLayerGroup::HandleEvent(SGEventType EventType, void *EventInfo,
								 SGMiscInfo *MiscInfo)
{
	// And handle the specific event
	switch (EventType)
	{
		case SGEVENT_REDRAW:
			{
				DocRect MyRect(FormatRect);	// Rely on cached format info being correct

				SGRedrawInfo *RedrawInfo = GetRedrawInfo(EventType, EventInfo);

				if (IMustRedraw(RedrawInfo))
				{
					StartRendering(RedrawInfo, MiscInfo);

					DocColour Col(192L, 192L, 192L);	//128, 128, 128);

					RedrawInfo->Renderer->SetLineWidth(0);
					RedrawInfo->Renderer->SetLineColour(RedrawInfo->Transparent);
					RedrawInfo->Renderer->SetFillColour(Col);
					RedrawInfo->Renderer->DrawRect(&MyRect);

					ReadGroupTitle();		// (Re)Cache the title text

					// Line up above indented items below
					MyRect.lo.x += IndentWidth + MiscInfo->PixelSize;

					// Render column titles above the 'locked' and 'invisible' columns
					DocRect VisibleBtn;
					DocRect LockedBtn;
					DocRect SnapBtn;
					SGDisplayLayer::CalculateButtonRects(MiscInfo, &MyRect,
														 &VisibleBtn, &LockedBtn, &SnapBtn);

					DrawBitmap(RedrawInfo->Renderer, &VisibleBtn, _R(IDB_LGAL_VISIBLE));
					DrawBitmap(RedrawInfo->Renderer, &LockedBtn,  _R(IDB_LGAL_LOCKED));
#if DO_SNAP_BUTTON
					DrawBitmap(RedrawInfo->Renderer, &SnapBtn,    _R(IDB_LGAL_SNAP));
#endif

					// And finally, render the title text
					MyRect.lo.x += SG_GapBeforeText;	// Leave a small gap before text
					if (MyRect.lo.x < MyRect.hi.x)		// If still room left, draw title
					{
						RedrawInfo->Renderer->SetFixedSystemTextColours(&RedrawInfo->Foreground, &Col);
						
						String_256 NewTitleText;

						UINT32 IDS = _R(IDS_LAYERGAL_FOREGROUND);
						if (!IsForeground())
							IDS = _R(IDS_LAYERGAL_BACKGROUND);

						NewTitleText.MakeMsg(IDS, (TCHAR *)TitleText);
						RedrawInfo->Renderer->DrawFixedSystemText(&NewTitleText, MyRect);
					}

					StopRendering(RedrawInfo, MiscInfo);
				}

				// Check if the cliprect overlaps any of our children - if not, then we
				// can return now, passing the event on quickly without bothering to give
				// it to our children.
				if (GetChild() == NULL ||
					!ChildArea.IsIntersectedWith(RedrawInfo->Bounds))
				{
					return(FALSE);
				}
			}
			break;


		case SGEVENT_MOUSECLICK:
			// Mouse clicks on this group type are ignored - but we claim them so that the
			// event is not passed on around the tree unnecessarily.
			{
				SGMouseInfo *MouseInfo = GetMouseInfo(EventType, EventInfo);
				if (FormatRect.ContainsCoord(MouseInfo->Position))
					return(TRUE);
			}
			break;		


		default:
			// For all other purposes (formatting etc), let the base class handle the event
			return(SGDisplayGroup::HandleEvent(EventType, EventInfo, MiscInfo));
			break;
	}

	// Pass the event on to my children, as appropriate, and return the result
	return(GiveEventToMyChildren(EventType, EventInfo, MiscInfo));
}









// -- Static variables
SGDisplayLayer *SGDisplayLayer::LastClickedLayer = NULL;


/***********************************************************************************************

>	SGDisplayLayer::SGDisplayLayer()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/1/95 (base generated in sgbase.cpp)

	Purpose:	SGDisplayLayer constructor
				DON'T call this constructor. It ERROR3's. Call the other constructor

***********************************************************************************************/

SGDisplayLayer::SGDisplayLayer()
{
	ERROR3("Illegal call on default SGDisplayLayer constructor - call the other one!");
	TheLayer = NULL;
}



/***********************************************************************************************

>	SGDisplayLayer::SGDisplayLayer(Layer *LayerToDisplay)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/1/95 (base generated in sgbase.cpp)

	Inputs:		LayerToDisplay - The Layer this item will display

	Purpose:	SGDisplayLayer constructor

***********************************************************************************************/

SGDisplayLayer::SGDisplayLayer(Layer *LayerToDisplay)
{
	ERROR3IF(LayerToDisplay == NULL,
				"SGDisplayLayer - trying to construct me with a NULL parameter is bad");

	TheLayer = LayerToDisplay;
}



/***********************************************************************************************

>	SGDisplayLayer::~SGDisplayLayer()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/1/95 (base generated in sgbase.cpp)

	Purpose:	SGDisplayLayer destructor

	Notes:		Ensures the LastClickedLayer memory is not left pointing at this layer

***********************************************************************************************/

SGDisplayLayer::~SGDisplayLayer()
{
	// Make sure LastClickedLayer pointer is not left in an invalid state
	if (this == LastClickedLayer)
		LastClickedLayer = NULL;
}



/***********************************************************************************************

>	virtual void SGDisplayLayer::CalculateMyRect(SGFormatInfo *FormatInfo,
												SGMiscInfo *MiscInfo)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/1/95 (base generated in sgbase.cpp)

	Inputs:		FormatInfo - The formatting info from which to calculate my position/size
				MiscInfo - As usual, the useful misc info struct

	Outputs:	member variable FormatRect - is returned filled in with the size/position of
				this Layer item's display area. This is dependent upon the current display
				mode and format state

				FormatInfo will be updated as a result of the formatting operation

	Purpose:	Shared code for Layer items to calculate where they will appear in the
				grand scheme of things

	Notes:		Layers supply only one display mode ("full info")

	Scope:		private (for use of SGDisplayLayer class only)

***********************************************************************************************/

void SGDisplayLayer::CalculateMyRect(SGFormatInfo *FormatInfo, SGMiscInfo *MiscInfo)
{
	INT32 XSize = SG_InfiniteWidth;
	INT32 YSize = SG_DefaultSmallIcon;

	CalculateFormatRect(FormatInfo, MiscInfo, XSize, YSize);
}



/***********************************************************************************************

>	static void SGDisplayLayer::CalculateButtonRects(INT32 ButtonGap, DocRect *MyRect,
													DocRect *VisibleBtn, DocRect *LockedBtn,
													DocRect *SnapBtn = NULL)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/2/95

	Inputs:		ButtonGap - The gap between buttons, in MILLIPOINTS (usually TwoPixels)
				MyRect - A rectangle vbased on FormatRect, from which to carve out 2 buttons

	Outputs:	MyRect - The used portion of MyRect will have been subtracted
				VisibleBtn - Will contain the button rectangle for the 'Visible' button
				LockedBtn - Will contain the button rectangle for the 'Locked' button
				SnapBtn - if non-NULL, will return containing the snap-button position

	Purpose:	Shared method to determine where the visible/locked buttons will be displayed

	Scope:		private

***********************************************************************************************/

void SGDisplayLayer::CalculateButtonRects(SGMiscInfo *MiscInfo, DocRect *MyRect,
											DocRect *VisibleBtn, DocRect *LockedBtn, DocRect *SnapBtn)
{
	INT32 ButtonGap = MiscInfo->PixelSize * 2;
	INT32 ButtonSize = MiscInfo->PixelSize * 16;			// The bitmap glyphs are 16x16 pixels

	// First, render the 'visible' and 'locked' buttons at the left end of our rectangle
	*VisibleBtn = *MyRect;
	VisibleBtn->hi.x = VisibleBtn->lo.x + ButtonSize;	// Make it a square

	INT32 Excess = VisibleBtn->Height() - ButtonSize;	// Center it in the Y axis
	Excess = GridLock(MiscInfo, Excess / 2);
	VisibleBtn->lo.y += Excess;
	VisibleBtn->hi.y = VisibleBtn->lo.y + ButtonSize;

	MyRect->lo.x = VisibleBtn->hi.x + ButtonGap;	// And exclude it from 'MyRect'


	*LockedBtn = *MyRect;
	LockedBtn->hi.x = LockedBtn->lo.x + ButtonSize;	// Make it a square
	LockedBtn->lo.y = VisibleBtn->lo.y;				// & use same y coords as for visible button
	LockedBtn->hi.y = VisibleBtn->hi.y;

	MyRect->lo.x = LockedBtn->hi.x + ButtonGap;		// And exclude it from 'MyRect'

#if DO_SNAP_BUTTON
	if (SnapBtn != NULL)
	{
		*SnapBtn = *MyRect;
		SnapBtn->hi.x = SnapBtn->lo.x + ButtonSize;	// Make it a square
		SnapBtn->lo.y = VisibleBtn->lo.y;				// & use same y coords as for visible button
		SnapBtn->hi.y = VisibleBtn->hi.y;

		MyRect->lo.x = SnapBtn->hi.x + ButtonGap;		// And exclude it from 'MyRect'
	}
#else
	// If snapbtn is disabled, fill it in with a sensible rect, just to be safe
	if (SnapBtn != NULL)
		*SnapBtn = *LockedBtn;
#endif


	// And grid align the 3 resulting rectangles...
	GridLockRect(MiscInfo, VisibleBtn);
	GridLockRect(MiscInfo, LockedBtn);
	if (SnapBtn != NULL)
		GridLockRect(MiscInfo, SnapBtn);
	GridLockRect(MiscInfo, MyRect);
}



/***********************************************************************************************

>	virtual void SGDisplayLayer::HandleRedraw(SGRedrawInfo *RedrawInfo,
												SGFormatInfo *FormatInfo)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/1/95 (base generated in sgbase.cpp)

	Inputs:		RedrawInfo	- The information on the kernel-rendered redraw area
				FormatInfo	- The formatting information structure

				member variable FormatRect should be set up (before calling this method)
				to be the rectangle in which to draw this item

	Purpose:	SGDisplayLayer item redraw method - removed from the main HandleEvent
				method merely to make the code tidier.

	Scope:		private

***********************************************************************************************/

void SGDisplayLayer::HandleRedraw(SGRedrawInfo *RedrawInfo, SGMiscInfo *MiscInfo)
{
	StartRendering(RedrawInfo, MiscInfo);

	DocRect MyRect(FormatRect);		// Get my redraw position from the cached FormatRect

	RenderRegion *Renderer = RedrawInfo->Renderer;

	INT32 OnePixel  = (INT32) DevicePixels(MiscInfo, 1);
	INT32 TwoPixels = (INT32) DevicePixels(MiscInfo, 2);

	Renderer->SetLineWidth(0);
	Renderer->SetLineColour(RedrawInfo->Transparent);
	Renderer->SetFillColour(DocColour(191L, 191L, 191L));

	// Calculate and redraw the buttons
	// NOTE:
	// The buttons are usually blank grey rectangles, or a rectangle with an eye/lock
	// glyph on them. However, to hint at what the buttons are for, the selected layer
	// also shows the buttons in a greyed state

	DocRect VisibleBtn;
	DocRect LockedBtn;
	DocRect SnapBtn;

	CalculateButtonRects(MiscInfo, &MyRect, &VisibleBtn, &LockedBtn, &SnapBtn);

	// Find out the state of the layer's flags
	BOOL ActiveAndVisEd = (GetDisplayedLayer()->IsActive() && Layer::ActiveLayerVisibleAndEditable);
	BOOL Visible 		= GetDisplayedLayer()->IsVisible();
	BOOL Locked  		= GetDisplayedLayer()->IsLocked();
	BOOL Guide			= GetDisplayedLayer()->IsGuide();
	Document* pDoc = (Document*)GetDisplayedLayer()->FindOwnerDoc();

	ERROR3IF(pDoc == NULL,"Displayed layer doesn't have an owner doc");
	if (pDoc == NULL) return;

	// Draw 'visible' button (The glyph is "on" when the layer is visible)
	if (Visible)
	{
		if (!Guide && (ActiveAndVisEd || pDoc->IsAllVisible()))
			DrawBitmap(Renderer, &VisibleBtn, _R(IDB_LGAL_TICKFORCED));
		else
			DrawBitmap(Renderer, &VisibleBtn, _R(IDB_LGAL_TICKON));
	}
	else
		DrawBitmap(Renderer, &VisibleBtn, _R(IDB_LGAL_TICKOFF));

	// Draw 'Locked' button (Glyph is on if the layer is unlocked)
	if (!Locked)
	{
		if (ActiveAndVisEd || pDoc->IsMultilayer())
			DrawBitmap(Renderer, &LockedBtn, _R(IDB_LGAL_TICKFORCED));
		else
			DrawBitmap(Renderer, &LockedBtn, _R(IDB_LGAL_TICKON));
	}
	else
		DrawBitmap(Renderer, &LockedBtn, _R(IDB_LGAL_TICKOFF));

#if DO_SNAP_BUTTON
	// Draw 'Snap' button
	if (Snap)
	{
		if (FALSE)		// **** MARKN NEEDS TO FIX THIS ****
			DrawBitmap(Renderer, &SnapBtn, _R(IDB_LGAL_TICKFORCED));	// Globally "forced to be ticked"
		else
			DrawBitmap(Renderer, &SnapBtn, _R(IDB_LGAL_TICKON));		// Just ticked normally
	}
	else
		DrawBitmap(Renderer, &SnapBtn, _R(IDB_LGAL_TICKOFF));			// Just ticked normally
#endif

	Layer* pLayer = GetDisplayedLayer();
	ERROR3IF(pLayer == NULL, "I'm displaying a NULL layer? Serious screwup has occurred!");
	if (pLayer == NULL)
		return;

	DocColour Foreground = RedrawInfo->Foreground;
	DocColour Background = RedrawInfo->Background;
	DocColour SelForeground = RedrawInfo->SelForeground;
	DocColour SelBackground = RedrawInfo->SelBackground;

	if (pLayer->IsGuide())
	{
		Foreground 		= DocColour(COLOUR_RED);
		SelBackground  	= DocColour(COLOUR_RED);
	}

	if (pLayer->IsPageBackground())
	{
		Foreground 		= DocColour(COLOUR_MIDGREY);
		SelBackground  	= DocColour(COLOUR_MIDGREY);
	}

	// Set up the colours for rendering our text, and fill the background if selected
	if (Flags.Selected)
	{
		// Fill the entire background with the 'selected' colour, so we don't
		// get gaps between bits of text or uneven rectangles in multiple selections
		Renderer->SetFillColour(SelBackground);
		Renderer->DrawRect(&MyRect);
		Renderer->SetFixedSystemTextColours(&SelForeground,&SelBackground);
	}
	else
		Renderer->SetFixedSystemTextColours(&Foreground, &Background);

	MyRect.lo.x += SG_GapBeforeText;	// Leave a small gap before text begins

	// And render the text
	String_256 MyText = pLayer->GetLayerID();
	Renderer->DrawFixedSystemText(&MyText, MyRect);				

	StopRendering(RedrawInfo, MiscInfo);
}



/***********************************************************************************************

>	void SGDisplayLayer::ChangeRangeState(SGDisplayLayer *Start, SGDisplayLayer *End,
											BOOL Range, INT32 WhichButton, SGMiscInfo *MiscInfo)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/8/95

	Inputs:		Start	- Points to the node, or start of a range of nodes, to set
				End		- NULL or points to the end of the range
				Range	- TRUE to do a whole range, FALSE to do only the Start node
						  This is used to override the range description.
				WhichButton - 	0 to change VISIBLE state
								1 to change LOCKED state
								2 to change SNAP state
				MiscInfo - The usual (see HandleEvent)

	Purpose:	To handle extension-clicks on layers, we set the state of a range of
				layers rather than just the one clicked. The range usually extends to the
				layer which was previously clicked.

	Notes:		This scans the sibling list containing Start, and sets the states of all
				layers it finds between start and end. If End is not found then Start is
				treated as a single item.

	Scope:		private

***********************************************************************************************/

void SGDisplayLayer::ChangeRangeState(SGDisplayLayer *Start, SGDisplayLayer *End,
										BOOL Range, INT32 WhichButton, SGMiscInfo *MiscInfo)
{
	ERROR3IF(Start == NULL || MiscInfo == NULL, "Illegal NULL params");

	SuperGallery *ParentGallery = GetParentGallery();
	ERROR3IF(ParentGallery == NULL, "No parent gallery?! Weird!");

	// Determine the new state based on inverting this item's state
	BOOL NewState = TRUE;
	switch (WhichButton)
	{
		case 0:		// Visible state
			NewState = !(GetDisplayedLayer()->GetVisibleFlagState());
			break;
		
		case 1:		// Locked sate
			NewState = !(GetDisplayedLayer()->GetLockedFlagState());
			break;

		case 2:		// Snap state
#if DO_SNAP_BUTTON
				// **** MARKN NEEDS TO FIX THIS ****
				NewState = !(GetDisplayedLayer()->GetVisibleFlagState());
#endif
			break;

		default:
			ERROR3("Illegal WhichButton parameter to SGDisplayLayer::ChangeRangeState (must be 0..2)");
			break;
	}

	// Scan the sibling list to find the start and end nodes, and determine if end comes
	// before start (in which case we'll need to swap them over before continuing)
	SGDisplayLayer *Ptr = NULL;

	if (!Range || End == NULL)
		End = Start;

	BOOL FoundStart	= FALSE;
	BOOL FoundEnd	= FALSE;
	BOOL MustSwap	= FALSE;

	Ptr = (SGDisplayLayer *) (Start->GetParent()->GetChild());
	while (Ptr != NULL)
	{
		if (Ptr == Start)
			FoundStart = TRUE;

		if (Ptr == End)
		{
			FoundEnd = TRUE;
			MustSwap = !FoundStart;		// If not found Start first, then the range is backwards
		}

		Ptr = (SGDisplayLayer *) Ptr->GetNext();
	}

	if (!FoundStart)		// Couldn't even find the start node!
	{
		ERROR3("Start node not found");
		return;
	}

	if (!FoundEnd)			// No end node? Make the range one item only
		End = Start;

	if (MustSwap)			// Range is backwards - swap the pointers
	{
		Ptr = Start;
		Start = End;
		End = Ptr;
	}


	// Now scan the list from Start to End, setting all the layers (inclusive)
	Ptr = Start;	
	
	while (Ptr != NULL)
	{
		DocRect MyRect(Ptr->FormatRect);
		DocRect VisibleBtn;
		DocRect LockedBtn;
		DocRect SnapBtn;
		Ptr->CalculateButtonRects(MiscInfo, &MyRect, &VisibleBtn, &LockedBtn, &SnapBtn);

		switch (WhichButton)
		{
			case 0:		// Visible state
				Ptr->DoChangeState(LAYER_VISIBLE, NewState);
				ParentGallery->ForceRedrawOfArea(&VisibleBtn);
				break;

			case 1:		// Locked state
				Ptr->DoChangeState(LAYER_LOCKED, NewState);
				ParentGallery->ForceRedrawOfArea(&LockedBtn);
				break;

			case 2:		// Snap state
#if DO_SNAP_BUTTON
				// **** MARKN NEEDS TO FIX THIS ****
				Ptr->DoChangeState(LAYER_VISIBLE, NewState);
				ParentGallery->ForceRedrawOfArea(&SnapBtn);
#endif
				break;
		}

		if (Ptr == End)					// Slight strangeness to handle (Start==End) case
			Ptr = NULL;									// Have hit and processed End, so stop
		else
			Ptr = (SGDisplayLayer *) Ptr->GetNext();	// Have not, so go on to next layer
	}
}



/***********************************************************************************************

>	virtual BOOL SGDisplayLayer::HandleEvent(SGEventType EventType, void *EventInfo,
											 SGMiscInfo *MiscInfo)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/1/95 (base generated in sgbase.cpp)

	Inputs:		EventType - An enumerated value describing what type of event is to be processed

				EventInfo - A structure describing the event (may be NULL). The exact thing
							pointed at by this pointer depends upon the event type:

							MonoOn
							Event				Thing EventInfo points at
							SGEVENT_FORMAT		(SGFormatInfo *)
							SGEVENT_REDRAW		(SGRedrawInfo *)
							SGEVENT_MOUSECLICK	(SGMouseInfo *)
							MonoOff
				Use the provided SGDisplayNode::Get[Format]Info() inlines to retrieve this
				information - they provide useful error/type checking, and hide the cast

				MiscInfo - always provided. Contains a few useful bits of info that may be
				needed for all event types.

	Outputs:	FormatInfo is updated as appropriate

	Returns:	TRUE if the event was handled successfully
				FALSE if it was not

	Purpose:	Handles a SuperGallery DisplayTree event

	Notes:		This overrides the pure virtual SGDisplayNode::HandleEvent method

				A node need not handle a specific event - if it does not handle it, it
				should return FALSE.

				Redraw and Formatting handlers should never return TRUE, as this will
				prevent the event from continuing through the tree.

				Non-leaf-nodes must call SGDisplayNode::GiveEventToMyChildren in order
				to pass the event dow the tree. THIS node is a leaf-node, so it doesn't.

	SeeAlso:	SGDisplayNode::HandleEvent

***********************************************************************************************/

BOOL SGDisplayLayer::HandleEvent(SGEventType EventType, void *EventInfo,
								  SGMiscInfo *MiscInfo)
{
	switch (EventType)
	{
		case SGEVENT_FORMAT:
			{
				SGFormatInfo *FormatInfo = GetFormatInfo(EventType, EventInfo);
				CalculateMyRect(FormatInfo, MiscInfo);		// Cache our FormatRect for later use
			}
			break;


		case SGEVENT_REDRAW:
			{
				DocRect MyRect(FormatRect);		// Rely on FormatRect being cached from above
				SGRedrawInfo *RedrawInfo = GetRedrawInfo(EventType, EventInfo);

				if (IMustRedraw(RedrawInfo))	// only redraw if we intersect the clip rect
					HandleRedraw(RedrawInfo, MiscInfo);
			}
			break;		// exit and return FALSE to pass the redraw event on


		case SGEVENT_MOUSECLICK:
			{
				SGMouseInfo *Mouse = GetMouseInfo(EventType, EventInfo);

				SuperGallery *ParentGallery = GetParentGallery();
				ERROR3IF(ParentGallery == NULL, "No parent gallery?! Weird!");

				if (FormatRect.ContainsCoord(Mouse->Position))
				{
					DocRect MyRect(FormatRect);
					DocRect VisibleBtn;
					DocRect LockedBtn;
					DocRect SnapBtn;
					Document* pDoc = (Document*)GetDisplayedLayer()->FindOwnerDoc();
					ERROR2IF(pDoc == NULL,TRUE,"Can't find owner doc for layer");
					ERROR2IF(!IS_A(pDoc,Document),TRUE,"Owner doc is not a Document");

					Layer* pLayer = GetDisplayedLayer();
					ERROR2IF(pLayer == NULL,TRUE,"GetDisplayedLayer() returned NULL");

					// Find out the state of the layer's flags
					BOOL ActiveAndVisEd = (pLayer->IsActive() && Layer::ActiveLayerVisibleAndEditable);
					BOOL Visible 		= pLayer->GetVisibleFlagState();
					BOOL Locked  		= pLayer->GetLockedFlagState();
					BOOL Guide			= pLayer->IsGuide();
					BOOL PageBackground = pLayer->IsPageBackground();


					CalculateButtonRects(MiscInfo, &MyRect, &VisibleBtn, &LockedBtn, &SnapBtn);

					// Ignore menu clicks on the Visible button
					if (!Mouse->MenuClick && VisibleBtn.ContainsCoord(Mouse->Position))
					{
						// Only change the actual setting if the AllVisible flag is clear
						// and it's not the active layer with the 'active layer is visible and edtable' pref on
					 	if (Guide || (!pDoc->IsAllVisible() && !ActiveAndVisEd))
						{
							ChangeRangeState(this, LastClickedLayer, Mouse->Extend, 0, MiscInfo);
//							DoChangeState(LAYER_VISIBLE,!Visible);
//							ParentGallery->ForceRedrawOfArea(&VisibleBtn);

							// Remember this as the "anchor" layer for extend-click setting of ranges
							LastClickedLayer = this;
						}
						else
						{
							UINT32 IDS = _R(IDS_LAYER_NOCHANGE_VISIBLE);

							if (ActiveAndVisEd)
								IDS = _R(IDS_LAYER_NOCHANGE_VISIBLE_ACTIVE);

							String_256 msg(IDS);
							GetApplication()->UpdateStatusBarText(&msg);
							Beep();
						}

						break;
					}

					// Ignore menu clicks on the Locked button
					if (!Mouse->MenuClick && LockedBtn.ContainsCoord(Mouse->Position))
					{
						// Only change the actual setting if the Multilayer flag is clear
						// and it's not the active layer with the 'active layer is visible and editable' pref on
						// Now disallow the user the edit the page background layer.
						if (!PageBackground && !pDoc->IsMultilayer() && !ActiveAndVisEd)
						{
							ChangeRangeState(this, LastClickedLayer, Mouse->Extend, 1, MiscInfo);
//							DoChangeState(LAYER_LOCKED,!Locked);
//							ParentGallery->ForceRedrawOfArea(&LockedBtn);

							// Remember this as the "anchor" layer for extend-click setting of ranges
							LastClickedLayer = this;
						}
						else
						{
							UINT32 IDS = _R(IDS_LAYER_NOCHANGE_LOCKED);

							if (PageBackground)
								IDS = _R(IDS_LAYER_NOCHANGE_BACKGROUND);

							if (ActiveAndVisEd)
								IDS = _R(IDS_LAYER_NOCHANGE_LOCKED_ACTIVE);

							String_256 msg(IDS);
							GetApplication()->UpdateStatusBarText(&msg);
							Beep();
						}

						break;
					}

#if DO_SNAP_BUTTON
					// Ignore menu clicks on the Snap button
					if (!Mouse->MenuClick && SnapBtn.ContainsCoord(Mouse->Position))
					{
						// Only change the actual setting if the AllVisible flag is clear
						// and it's not the active layer with the 'active layer is visible and edtable' pref on
					 	
						// **** MARKN NEEDS TO FIX THIS ****
						// Mark: This currently all sets the visible state of the layer.
					 	if (!pDoc->IsAllVisible() && !ActiveAndVisEd)
						{
							ChangeRangeState(this, LastClickedLayer, Mouse->Extend, 2, MiscInfo);
//							DoChangeState(LAYER_VISIBLE,!Visible);
//							ParentGallery->ForceRedrawOfArea(&VisibleBtn);

							// Remember this as the "anchor" layer for extend-click setting of ranges
							LastClickedLayer = this;
						}
						else
						{
							UINT32 IDS = _R(IDS_LAYER_NOCHANGE_VISIBLE);

							if (ActiveAndVisEd)
								IDS = _R(IDS_LAYER_NOCHANGE_VISIBLE_ACTIVE);

							String_256 msg(IDS);
							GetApplication()->UpdateStatusBarText(&msg);
							Beep();
						}

						break;
					}
#endif

					// Only treat clicks as selection clicks if they are to the right of the tick buttons
					// This is easy, as MyRect excludes the buttons
					if (MyRect.ContainsCoord(Mouse->Position))
					{
						// In the layer gallery the selection can never be more than one item,
						// so all clicks are treated like Select clicks.
						if (!Flags.Selected)
						{
							// Make the layer we're displaying the active one
							Layer* pLayer = GetDisplayedLayer();
							LayerSGallery::MakeActiveLayer(pLayer);

							// Now update the selection states...
							ParentGallery->SelectItems(FALSE);	// Deselect everything else

							// Repaint the list box now. This is because if there is a large
							// distance between the old selection and the new one, we get a huge
							// redraw cliprect, so get a (slow) complete redraw, instead of two
							// small redraws. It is thus better to break the redraw into 2 steps
							// so that we are more likely to get 2 fast redraws than one slow one.
							//ParentGallery->PaintGadgetNow(_R(IDC_GALLERY_LISTBOX));
							ParentGallery->PaintListNow();
						
							// And select myself, and the new active layer
							SetSelected(TRUE);

							// My selection state has changed, so I'd better redraw myself
							ForceRedrawOfMyself();

							// And inform the parent gallery that the selection has changed, so
							// it can shade/unshade buttons as appropriate, etc
							ParentGallery->SelectionHasChanged();
						}

						// Remember this as the "anchor" layer for extend-click setting of ranges
						LastClickedLayer = this;

						// Start a drag on the layer item
						SGListDragInfo* pDragInfo = new SGListDragInfo(ParentGallery,this,Mouse,Mouse->MenuClick);

						if (pDragInfo != NULL)
							DragManagerOp::StartDrag(pDragInfo);
					}

					return(TRUE);		// Claim this event - nobody else can own this click
				}
			}
			break;


		default:
			// Let the base class handle all unknown/unwanted events
			return(SGDisplayItem::HandleEvent(EventType, EventInfo, MiscInfo));
	}

	// Default return value: We do not claim this event, so it will be passed on to others
	return(FALSE);
}



/***********************************************************************************************

>	void SGDisplayLayer::DoChangeState(OpLayerGalReason Reason,BOOL NewState)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/8/94
	Inputs:		Reason 	= reason code definiting what 'NewState' represents
				NewState= boolean that determines the new state of the flag defined by 'Reason'

	Returns:	-
	Purpose:	Called when a flag changes for a layer gallery item
	
	Notes:		Does not redraw the layer in the display list

***********************************************************************************************/

void SGDisplayLayer::DoChangeState(OpLayerGalReason Reason,BOOL NewState)
{
	LayerSGallery* pGal = (LayerSGallery*)GetParentGallery();
	ERROR3IF(!IS_A(pGal,LayerSGallery),"Parent not a layer gallery");
	if (!IS_A(pGal,LayerSGallery)) return;

	Layer* pLayer = GetDisplayedLayer();
	pGal->DoChangeLayerState(pLayer,Reason,NewState);
}



/***********************************************************************************************

>	void SGDisplayLayer::DoChangeLocked(BOOL Locked)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com> (Jason, ripped from the LayerGallery, 1/2/95)
	Created:	8/8/94
	Inputs:		Locked - TRUE means make the displayed layer locked
						 FALSE means unlock

	Returns:	-
	Purpose:	Called when the Locked button is clicked in a layer gallery item.
				Changes the lockedness of the displayed layer.

	Notes:		Does not redraw the layer in the display list

***********************************************************************************************/
/*
void SGDisplayLayer::DoChangeLocked(BOOL Locked)
{
	Layer* pLayer = GetDisplayedLayer();

	BOOL PrevLocked = pLayer->IsLocked();
	pLayer->SetLocked(Locked);
	BOOL PostLocked = pLayer->IsLocked();

	if (PrevLocked != PostLocked)
		NodeRenderableInk::DeselectAllOnLayer(pLayer); 
}

*/



/***********************************************************************************************

>	virtual void SGDisplayLayer::MoveAfter(SGDisplayNode *NodeToMove)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com> (from a Jason func)
	Created:	21/3/95

	Inputs:		NodeToMove - the node to move

	Purpose:	MOVES the given node (to a different position in the DisplayTree) as the
				previous (left) sibling of this node. If the node is not linked into
				a tree, it is effectively just inserted.

				

	Errors:		ERROR3 and quiet exit if NodeToMove == NULL

	SeeAlso:	SuperGallery; SGDisplayNode::MoveAfter; SGDisplayNode::InsertAfter;
				SGDisplayNode::AddItem

***********************************************************************************************/

void SGDisplayLayer::MoveAfter(SGDisplayNode *NodeToMove)
{
	ERROR3IF(NodeToMove == NULL, "Illegal NULL param");
	if (NodeToMove == this || NodeToMove == NULL)
		return;

	// Call base class to shift around the display item
	SGDisplayItem::MoveAfter(NodeToMove);

	// And move the associated layer in the docuemnt tree
	MoveLayer(NodeToMove,FALSE,FALSE);
}



/***********************************************************************************************

>	virtual void SGDisplayLayer::MoveBefore(SGDisplayNode *NodeToMove)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com> (from a Jason func)
	Created:	21/3/95

	Inputs:		NodeToMove - the node to move

	Purpose:	MOVES the given node (to a different position in the DisplayTree) as the
				previous (left) sibling of this node. If the node is not linked into
				a tree, it is effectively just inserted.

	Notes:		This base class method simply delinks the item and relinks it elsewhere
				in the display tree. However, derived classes will override this method
				so that moving display items can have a further effect of also rearranging
				the displayed "real" items. Before/After moving the real item, the
				derived class can then call this baseclass method to complete the action.
		
				Take care when moving items between groups (e.g. if an item is "moved"
				from one docuemnt to another, it could be a bad thing, so be very
				careful in derived classes to take appropriate action)

				Any attempt to move an item before *itself* is queitly ignored

	Errors:		ERROR3 and quiet exit if NodeToMove == NULL

	SeeAlso:	SuperGallery; SGDisplayNode::MoveBefore; SGDisplayNode::InsertBefore;
				SGDisplayNode::AddItem

***********************************************************************************************/

void SGDisplayLayer::MoveBefore(SGDisplayNode *NodeToMove)
{
	ERROR3IF(NodeToMove == NULL, "Illegal NULL param");
	if (NodeToMove == this || NodeToMove == NULL)
		return;

	// Call base class to shift around the display item
	SGDisplayItem::MoveBefore(NodeToMove);

	// And move the associated layer in the docuemnt tree
	MoveLayer(NodeToMove,TRUE,FALSE);
}



/***********************************************************************************************

>	virtual void SGDisplayLayer::MoveLayer(SGDisplayNode *NodeToMove,BOOL Before,BOOL ToggleBackground)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/3/95

	Inputs:		NodeToMove 	= the node to move (which specifies the layer to move)
				Before		= if TRUE the layer is moved previous (before) this layer, otherwise
								the layer is moved next (after) this layer
				ToggleBacground = TRUE if the background flag should be toggled when moved

	Purpose:	This moves the given layer either before or after this layer, in an undoable way

	Errors:		ERROR3 and quiet exit if NodeToMove == NULL

	SeeAlso:	SuperGallery; SGDisplayNode::MoveAfter; SGDisplayNode::InsertAfter; SGDisplayNode::AddItem

***********************************************************************************************/

void SGDisplayLayer::MoveLayer(SGDisplayNode *NodeToMove,BOOL Before,BOOL ToggleBackground)
{
	ERROR3IF((!IS_A(NodeToMove,SGDisplayLayer) && !IS_A(NodeToMove,SGDisplayFrame)),"The node to move is not a SGDisplayLayer");
	if (!IS_A(NodeToMove,SGDisplayLayer) && !IS_A(NodeToMove,SGDisplayFrame))
		return;

	Layer* pLayerToMove = ((SGDisplayLayer*)NodeToMove)->GetDisplayedLayer();
	Layer* pThisLayer   = GetDisplayedLayer();

	Spread* pSpread = pLayerToMove->FindParentSpread();
	ERROR3IF(pSpread == NULL,"Parent spread is NULL");
	if (pSpread == NULL)
		return;	
 	
	OpLayerGalReason Reason;

	if (pLayerToMove == pThisLayer)
	{
		// if the context node and the layer to move are the same, and we don't have to toggle
		// the background flag, just return (give errors in debug mode)
		ERROR3IF(!ToggleBackground,"Not moving the layer OR toggling the background flag, so why did you call?");
		if (!ToggleBackground)
			return;

		Reason = LAYER_TOGGLEBACKGROUND;
	}
	else
		Reason = LAYER_MOVE;
	

	OpLayerGalParam Param(Reason,pSpread);

	Param.pLayer 			= pLayerToMove;
	Param.pContextNode		= pThisLayer;
	Param.MoveRedraw 		= TRUE;
	Param.ToggleBackground	= ToggleBackground;
	Param.pLayerSGal		= (LayerSGallery*)GetParentGallery();

	if (Before)
		Param.AttDir = NEXT;	// next means insert-before-this
	else
		Param.AttDir = PREV;	// prev means insert-after-this

	OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_LAYERGALCHANGE); 
	if (pOpDesc != NULL)
		pOpDesc->Invoke((OpParam*)&Param);
	else
	{
		ENSURE(FALSE,"Couldn't find OPTOKEN_LAYERGALCHANGE op descriptor");
	}
}

#endif // EXCLUDE_GALS

/********************************************************************************************

>	LayerSGallery::LayerSGallery()
												 
	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/1/95 (base generated in sgbase.cpp)
	Purpose:	LayerSGallery default constructor

********************************************************************************************/

LayerSGallery::LayerSGallery()
{
#ifndef EXCLUDE_GALS
	pSpread = NULL;
	pDoc = NULL;
	DisplayForeground = NULL;	// Group of foreground layers
	DisplayBackground = NULL;	// Group of background layers

	OldGuideLayerState = -1;
	NewGuideLayerState = -1;
	m_NewDocBorn = FALSE;
#endif
} 



/********************************************************************************************

>	LayerSGallery::~LayerSGallery()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/1/95 (base generated in sgbase.cpp)
	Purpose:	LayerSGallery destructor.

********************************************************************************************/

LayerSGallery::~LayerSGallery()
{
}

#ifndef EXCLUDE_GALS

/********************************************************************************************

>	void LayerSGallery::CreateNewSubtree(Document *ParentDoc, SGDisplayGroup *ExistingGroup)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/1/95 (base generated in sgbase.cpp)

	Inputs:		ParentDoc - The document to create a display subtree for
				ExistingGroup - NULL (creates a new group for this document), or
				a pointer to the existing group-node for this document (in which case
				it clears all displayitems from the group and rebuilds it in place - this
				stops the display group moving around the tree at random!)

	Purpose:	Internal call. This takes the Layer list of the given document and 
				creates a DisplayTree subtree from it. This subtree is then added to
				the DisplayTree. Note that this does not force a redraw of the list - 
				after making this call, you should also call ForceRedrawOfList

	Notes:		Passing in a NULL parent document pointer results in an ERROR3 -
				the function returns without doing anything in retail builds

	SeeAlso:	SuperGallery::ForceRedrawOfList

********************************************************************************************/

void LayerSGallery::CreateNewSubtree(Document *ParentDoc, SGDisplayGroup *ExistingGroup)
{
	if (DisplayTree == NULL)
		return;

TRACEUSER( "Neville", _T("LayerSGallery::CreateNewSubtree Just about to DestroySubtree\n"));
	// The layer gallery only ever shows one document, so destroy the entire tree
	// and start again from scratch (note that this doesn't destroy the root node)
	DisplayTree->DestroySubtree();

	// All groups will be destroyed by DestroySubtree(), so we'd better forget them
	DisplayForeground = NULL;
	DisplayBackground = NULL;
	
TRACEUSER( "Neville", _T("LayerSGallery::CreateNewSubtree completed DestroySubtree\n"));

 	if (ParentDoc == NULL)
		return;

	SGDisplayLayer  *DisplayLayer = NULL;

	EnsureActiveLayerIntegrity();

	// Create the foreground layer group
	DisplayForeground = new SGLayerGroup(this, ParentDoc,NULL,TRUE);	// Create new Group
	if (DisplayForeground == NULL)										// Total failure - abort
		return;
	DisplayTree->AddItem(DisplayForeground);							// Add new group to tree

	// Create the background layer group
	DisplayBackground = new SGLayerGroup(this, ParentDoc,NULL,FALSE);	// Create new Group
	if (DisplayBackground == NULL)										// Total failure - abort
		return;
	DisplayTree->AddItem(DisplayBackground);							// Add new group to tree
	

	Spread* pSelSpread = GetSelectedSpread();
	ERROR3IF(pSelSpread == NULL,"No selected spread in LayerSGallery");
	if (pSelSpread == NULL) return;

	Layer* pLayer		= pSelSpread->FindLastLayer();
	Layer* pActiveLayer	= NULL;
	Layer* pGuideLayer	= NULL;

	while (pLayer != NULL)
	{
		// Keep a ptr to the guide layer, if there is one
		if (pLayer->IsGuide())
			pGuideLayer = pLayer;

		DisplayLayer = new SGDisplayLayer(pLayer);

		if (DisplayLayer != NULL)
		{
			if (pLayer->IsBackground())
				DisplayBackground->AddItem(DisplayLayer);
			else
				DisplayForeground->AddItem(DisplayLayer);
		}

		if (pLayer->IsActive())
		{
			ERROR3IF(pActiveLayer != NULL,
						"Found more than one active layer in the selected spread");
			pActiveLayer = pLayer;

			DisplayLayer->SetSelected(TRUE);	// Make the active layer selected
		}

		pLayer = pLayer->FindPrevLayer();
	}

	// The new state of the Guide Layer is now the old state
	OldGuideLayerState = NewGuideLayerState;

	if (pGuideLayer == NULL)
		NewGuideLayerState = -1;						// If there's no guide layer, set new state to be -1
	else
		NewGuideLayerState = pGuideLayer->IsVisible();	// New state is based on the guide layer's visibility

	if (IsVisible())
	{
		SetBoolGadgetSelected(_R(IDC_BTN_MULTILAYER),!(ParentDoc->IsMultilayer() == 0));
		SetBoolGadgetSelected(_R(IDC_BTN_ALLVISIBLE),!(ParentDoc->IsAllVisible() == 0));
	}

	TRACEUSER( "Neville", _T("LayerSGallery::CreateNewSubtree exit\n"));
}



/********************************************************************************************

>	BOOL LayerSGallery::PreCreate(void)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com> (Jason)
	Created:	2/8/94
	Returns:	TRUE if it is successful; FALSE to abort creation of the window

	Purpose:	Layer SuperGallery PreCreate handler

	Notes:		This method is called whenever the gallery window is about to be
				created.

********************************************************************************************/

BOOL LayerSGallery::PreCreate(void)
{
	NewSelectedSpread();		// Cache the selected spread info

	// If there isn't already one, create a DisplayTree
	if (DisplayTree == NULL)
	{
		DisplayTree = new SGDisplayRootScroll(this);	// New root node, with a scrollbar
		if (DisplayTree == NULL)
			return(FALSE);
	}

	// The layer sgallery is a splitter, in that it only ever shows information for the
	// selected document, so there is no point trying to call this method for loads of
	// documents as the other galleries do.
	CreateNewSubtree(Document::GetSelected());

	return(TRUE);
}



/********************************************************************************************

>	BOOL LayerSGallery::PostCreate(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/1/95 (base generated in sgbase.cpp)

	Returns:	TRUE if the Gallery initialised successfully
				FALSE if it should not be opened due to a failure to initialise

	Purpose:	The LayerSGallery PostCreate handler. This overrides the base class
				PostCreate function. It is called at the very end of the
				SuperGallery::Create method, after the window has been created and opened.

********************************************************************************************/

BOOL LayerSGallery::PostCreate(void)
{
	Document *ParentDoc = Document::GetSelected();
	if (ParentDoc != NULL)
	{
		SetBoolGadgetSelected(_R(IDC_BTN_MULTILAYER),!(ParentDoc->IsMultilayer() == 0));
		SetBoolGadgetSelected(_R(IDC_BTN_ALLVISIBLE),!(ParentDoc->IsAllVisible() == 0));
	}

	EnableGadget(_R(IDC_GALLERY_COPY),ParentDoc != NULL && !IsSelectedItemGuideLayer());

	return(TRUE);
}



/********************************************************************************************

>	virtual BOOL LayerSGallery::ApplyAction(SGActionType Action)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/1/95 (base generated in sgbase.cpp)

	Inputs:		Action - Indicates what action to apply

	Returns:	TRUE to indicate successful handling of the action, or
				FALSE to indicate failure

	Purpose:	Applies certain conventional gallery actions (usually associated with
				gallery buttons, for new, edit, delete, etc)

	SeeAlso:	SGActionType

********************************************************************************************/

BOOL LayerSGallery::ApplyAction(SGActionType Action)
{
	// No display tree? Better forget about it then!
	if (DisplayTree == NULL)
		return(FALSE);

	// Determine useful info - this is usually needed for most actions, so always get it
	Document		*SelectedDoc = Document::GetSelected();
	SGDisplayGroup	*DocumentGroup = DisplayTree->FindSubtree(this, SelectedDoc, NULL);
	SGDisplayLayer	*FirstSelected = NULL;
	
	if (DocumentGroup != NULL)
		FirstSelected = (SGDisplayLayer *) DocumentGroup->FindNextSelectedItem(NULL);

	// Now, process the action	TO DO! - see Colour gallery for examples
	switch(Action)
	{
		case SGACTION_CREATE:
			DoCreateNewItem();
			break;

		case SGACTION_EDIT:
			DoChangeName();
			break;

		case SGACTION_DELETE:
			if (PrepareToDelete())
				DoDeleteSelection();
			break;

//		case SGACTION_APPLY:
//		case SGACTION_REDEFINE:
//		case SGACTION_DISPLAYMODECHANGED:
//			These operations are not provided by the layer gallery
//			break;

		default:
			return(SuperGallery::ApplyAction(Action));
	}

	return(TRUE);
}



/********************************************************************************************

>	virtual MsgResult LayerSGallery::Message(Msg* Message)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/1/95 (base generated in sgbase.cpp)
	Inputs:		Message - The message to handle

	Purpose:	A standard message handler, really.

	Notes:		Any messages that this does not handle must be passed down to the
				SuperGallery base class message handler.

				NOTE WELL that the SuperGallery base class handler does some funky things
				for us - see SuperGallery::Message - such as deleting our display subtree
				for any document which dies (which, uncannily, would explain why they go
				away like that when you close documents ;-), and shading the gallery when
				there are no documents present. [To override this behaviour in these cases,
				you should respond to the message, and return OK rather than calling the
				base class message handler]

	SeeAlso:	SuperGallery::Message

********************************************************************************************/

MsgResult LayerSGallery::Message(Msg* Message)
{
	if (IS_OUR_DIALOG_MSG(Message))
	{
		DialogMsg* Msg = (DialogMsg*)Message;

		switch (Msg->DlgMsg)
		{
			case DIM_CREATE:
				SGInit::UpdateGalleryButton(OPTOKEN_DISPLAYLAYERGALLERY, TRUE);
				break;

			case DIM_CANCEL:
				SGInit::UpdateGalleryButton(OPTOKEN_DISPLAYLAYERGALLERY, FALSE);
				break;
		}
	}


	// If we have no displaytree, then we have not been shown, or something terrible has
	// happened, so we don't bother handling any of these messages.
	if (DisplayTree == NULL)
		return(SuperGallery::Message(Message));


	if (IS_OUR_DIALOG_MSG(Message))
	{
		DialogMsg* Msg = (DialogMsg*)Message;
		INT32 state;

		switch (Msg->DlgMsg)
		{
			case DIM_LFT_BN_CLICKED:
				//TRACE( _T("Gadget %d clicked\n"),Msg->GadgetID);
				switch(Msg->GadgetID)
				{
					case _R(IDC_BTN_MULTILAYER):
						state = GetLongGadgetValue(_R(IDC_BTN_MULTILAYER),0,1);
						DoChangeLayerState(NULL,LAYER_MULTILAYER,state);
//						DoChangeMultilayer(state);
//						ForceRedrawOfList();
						break;

					case _R(IDC_BTN_ALLVISIBLE):
						state = GetLongGadgetValue(_R(IDC_BTN_ALLVISIBLE),0,1);
						DoChangeLayerState(NULL,LAYER_ALLVISIBLE,state);
//						DoChangeAllVisible(state);
//						ForceRedrawOfList();
						break;

					case _R(IDC_GALLERY_UPONE):
						DoMoveLayer(MOVELAYER_UPONE);
						break;

					case _R(IDC_GALLERY_DOWNONE):
						DoMoveLayer(MOVELAYER_DOWNONE);
						break;

//					case _R(IDC_GALLERY_NAME):
//						DoChangeName();
//						break;

					case _R(IDC_GALLERY_PROPERTIES):
						DoLayerProperties();
						break;

					case _R(IDC_GALLERY_COPY):
						DoCopyLayer();
						break;

					case _R(IDC_LAYER_MOVE_TO_ACTIVE):
						DoMoveSelectionToActiveLayer();
						break;
												
#ifndef WEBSTER
					case _R(IDC_GALLERY_HELP):		// Show help page
						HelpUserTopic(_R(IDS_HELPPATH_Gallery_Layer));
						break;
#endif					
				}
				break;
		}
	}
	else if (MESSAGE_IS_A(Message, OpMsg))
	{
		switch (((OpMsg*)Message)->MsgType)
		{
			case OpMsg::AFTER_UNDO:
			case OpMsg::AFTER_REDO:
				EnsureSelSpreadSelectionIntegrity();
				break;
		}
	}
	else if (MESSAGE_IS_A(Message, SpreadMsg))
	{
		SpreadMsg*	pSpreadMsg = (SpreadMsg*) Message;

		switch (pSpreadMsg->Reason)
		{
			case SpreadMsg::SELCHANGED:
				NewSelectedSpread(pSpreadMsg->pNewSpread);  // gets selected doc too
				if (pDoc != NULL && IsVisible())
				{
TRACEUSER( "Neville", _T("SpreadMsg::SELCHANGED CreateNewSubtree\n"));
					CreateNewSubtree(pDoc);
					ForceRedrawOfList();
				}
				break;


			case SpreadMsg::LAYERCHANGES:
				if (IsVisible())
				{
					if (pSpreadMsg->pNewSpread == pSpread)
					{
						if (pSpread->FindFirstLayer() != NULL)
						{
							EnsureActiveLayerIntegrity();
							if (pDoc != NULL && IsVisible())
							{
TRACEUSER( "Neville", _T("SpreadMsg::LAYERCHANGES CreateNewSubtree\n"));
								CreateNewSubtree(pDoc);
								ForceRedrawOfList();
							}
						}
					}
				}
				else
				{
					if (pSpreadMsg->pNewSpread->FindFirstLayer() != NULL)
					{
						pSpread = pSpreadMsg->pNewSpread;
						EnsureActiveLayerIntegrity();
					}
				}
				break;
		}
	}
	else if (MESSAGE_IS_A(Message, DocChangingMsg))
	{
		DocChangingMsg *Msg = (DocChangingMsg *) Message;
		switch (Msg->State)
		{
				case DocChangingMsg::DocState::TITLECHANGED:
// Showstopper removed, Jason, 28/3/96
// When the doc title changes, we simply need to redraw to update the group titles.
// Calling CreateNewSubtree will destroy the existing tree - this is really really bad
// when the document title changed as a result of us (e.g.) locking a layer, because the
// message broadcats hits us while we're still in the middle of traversing the tree,
// we then delete the tree out from under ourselves, and are then in a highly dodgy state!
//				CreateNewSubtree(Msg->pChangingDoc);
				ForceRedrawOfList();
				break;
		
			// This message is sent when a new or just opened document is stable.
			case DocChangingMsg::DocState::SELCHANGED:
			{
				if (Msg->pNewDoc == NULL)
				{
					// No documents around any more, so we must wipe the display tree
					// The base class will shade the gallery for us.
					TRACEUSER( "Neville", _T("DocChangingMsg::DocState::SELCHANGED CreateNewSubtree\n"));
					CreateNewSubtree(NULL);
					ForceRedrawOfList();
				}
				else 
				{
			#ifdef WEBSTER
					// There is an active document, so ensure the gallery is unshaded
					ShadeGallery(FALSE);
			#else
					//Do not process the message if it has been sent due to the creation of a new document.
					if ( !GetNewDocBorn() )
					{
						// flag variables.
						BOOL LayerMode = FALSE;	
					
						// Get the changing document pointer.
						Document* pDoc = Msg->pNewDoc;

						if(pDoc)
						{
							// Determine the document mode.
							LayerMode = IsLayerMode(pDoc);

							// If we are no longer in Layer mode then
							// close the Layer gallery and open the Frame gallery.
							if( !LayerMode && IsVisible() )
							{
								// Close the layer gallery.
								if( CloseLayerGallery() )
								{
									// Open the frame gallery.
									OpenFrameGallery();
								}
							}
						}
					}
					
					// If the gallery is visible, then ensure the gallery is unshaded.
					if(IsVisible())
					{
						// There is an active document, so ensure the gallery is unshaded
						ShadeGallery(FALSE);
					}
					
				#endif
				}
				break;
			}
		#ifndef WEBSTER
			case DocChangingMsg::DocState::BORN:
			{
				// When a new document is created, a SELCHANGED
				// message is broadcasted, for Camelot2 frame/layer integration
				// we do not wish to process the message. 
				// We therefore set this flag to notify us when a new document
				// is born.

				// Ensure we are no longer in Frame mode and the frame gallery is open.
				if( IsVisible() )
				{
					// Set the flag to true.
					// This is set to false in BORNANDSTABLE. 
					SetNewDocBorn(TRUE);
				}
				break;
			}

			case DocChangingMsg::DocState::BORNANDSTABLE:
			{
				// flag variables.
				BOOL LayerMode = FALSE;	
				BOOL GalleryClosed = FALSE;

				// Get the changing document pointer.
				Document* pChangingDoc = Msg->pChangingDoc;

				if(pChangingDoc)
				{
					// Determine the document mode.
					LayerMode = IsLayerMode(pChangingDoc);

					// If we are no longer in Layer mode then
					// close the Layer gallery and open the Frame gallery.
					if( !LayerMode && IsVisible() )
					{
						// Close the layer gallery.
						if(CloseLayerGallery())
						{
							// Open the frame gallery.
							OpenFrameGallery();

							// Now we have processed this message, it is safe to process SELCHANGED messages.
							SetNewDocBorn(FALSE);
						}
					}
				}
				break;
			}

			case DocChangingMsg::DocState::KILLED:
			case DocChangingMsg::DocState::ABOUTTODIE:
			{
				// The doc. is about to die, therefore, set this falg to false.
				SetNewDocBorn(FALSE);
			break;
			}

		#endif
		}
	}


	else if (MESSAGE_IS_A(Message, DocViewMsg))
	{
		DocViewMsg *Msg = (DocViewMsg *) Message;
		switch (Msg->State)
		{
			case DocViewMsg::DocViewState::SELABOUTTOCHANGE: 
				OldGuideLayerState = GetGuideLayerState(Msg->pNewDocView);
				break;

			case DocViewMsg::DocViewState::SELCHANGED: 
			{
				if (IsVisible())
				{
					NewGuideLayerState = GetGuideLayerState(Msg->pNewDocView);
					if (OldGuideLayerState != NewGuideLayerState)
					{
						TRACEUSER( "Neville", _T("DocViewMsg::DocViewState::SELCHANGED CreateNewSubtree\n"));
						CreateNewSubtree(Msg->pNewDocView ? Msg->pNewDocView->GetDoc() ? NULL);
						ForceRedrawOfList();
					}
				}
			}
			break;
		}
	}
	else if (MESSAGE_IS_A(Message, LayerMsg))
	{
		LayerMsg *pMsg = (LayerMsg *) Message;

		switch ( pMsg->Reason )
		{
			// The active layer has changed.
			case LayerMsg::LayerReason::ACTIVE_LAYER_CHANGED:
			{
				EnableGadget(_R(IDC_GALLERY_COPY),pMsg->pNewLayer != NULL && !pMsg->pNewLayer->IsGuide()); 
			}
			break;

			// The visibility of a layer has changed, probably by an external source
			// so update the display
			case LayerMsg::LayerReason::LAYER_VISIBILITY_CHANGED:
			{
				ForceRedrawOfList(); 
			}
			break;

			// The active layer has been changed externally to the gallery.
			case LayerMsg::LayerReason::UPDATE_ACTIVE_LAYER:
			{
				// We must now check whether the selected layer has been changed externally
				// to us. May happen if an animated GIF is being loaded onto layers
				SGDisplayLayer *pDisplayItem = NULL;
				if (DisplayForeground != NULL)
					pDisplayItem = GetSelectedLayerGalItem();
				if (pDisplayItem != NULL)
				{
					if (pDisplayItem->GetDisplayedLayer() != pMsg->pNewLayer)
					{
						Layer* pNewActiveLayer = pMsg->pNewLayer;
						// Search for the new display item to match this new active layer 
						SGDisplayLayer *pWantedDispLayer = NULL;
						SGDisplayLayer *pDispLayer = ((SGDisplayFrame *) DisplayForeground->GetChild());
						while (pDispLayer != NULL)						
						{
							if (pDispLayer->GetDisplayedLayer() == pNewActiveLayer)
								pWantedDispLayer = pDispLayer;
							pDispLayer = ((SGDisplayFrame *)pDispLayer->GetNext());
						}
						
						// If we found the required display item then go and select it
						if (pWantedDispLayer)
						{
							// Now update the selection states...
							SelectItems(FALSE);	// Deselect everything else
							// Something has changed so we must go and select this new item
							pWantedDispLayer->SetSelected(TRUE);
						}
					}
				}
			}
			break;
		}
	}

	return(SuperGallery::Message(Message));
}    

/********************************************************************************************

>	INT32 LayerSGallery::GetGuideLayerState(DocView* pDocView)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/9/95
	Inputs:		pDocView = ptr to docview (This holds visibility info for guide layers)
	Outputs:	-
	Returns:	 0 = Guide layer is invisible
				 1 = Guide layer is visible
				-1 = Guide layer doesn't exist

	Purpose:	This returns the current state of the guide layer.
				It is used to determine whether we need to recreate and redraw the layer gallery
				when the selected view changes.

********************************************************************************************/

INT32 LayerSGallery::GetGuideLayerState(DocView* pDocView)
{
	Spread* pSelSpread = GetSelectedSpread();

	if (pSelSpread != NULL)
	{
		Layer* pLayer = pSelSpread->FindFirstGuideLayer();
		if (pLayer != NULL)
			return (pLayer->IsVisible(pDocView));
	}

	return -1;
}

	

/********************************************************************************************

>	void LayerSGallery::NewSelectedSpread(Spread* pNewSelSpread = NULL)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/8/94
	Inputs:		pNewSelSpread = ptr to the spread the layer gal should take as the
				selected one; NULL will read the global Selected spread value.
	Outputs:	-
	Returns:	-
	Purpose:	Central place for setting the layer gal's selected spread.
				Also takes a copy of the selected document

********************************************************************************************/

void LayerSGallery::NewSelectedSpread(Spread* pNewSelSpread)
{
	if (pNewSelSpread == NULL)
		pSpread = Document::GetSelectedSpread();
	else
		pSpread = pNewSelSpread;
	
	pDoc = Document::GetSelected();
}




/***********************************************************************************************

>	void LayerSGallery::EnsureSelSpreadSelectionIntegrity()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/8/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	This makes sure that all the invisible and/or locked layers in the spread
				have no selection. It calls NodeRenderableInk::DeselectAllOnLayer() on each 
				layer that should NOT have a selection, just in case.
				Currently only called after an undo or redo op, because it could be quite slow

***********************************************************************************************/

void LayerSGallery::EnsureSelSpreadSelectionIntegrity()
{
	if (pSpread == NULL) return;

	// Get the first layer in the spread
	Layer* pLayer = pSpread->FindFirstLayer();	

	while (pLayer != NULL)
	{
		// If the layer is not visible OR the layer is locked, make sure there is no selection
		if (!pLayer->IsVisible() || pLayer->IsLocked())
			NodeRenderableInk::DeselectAllOnLayer(pLayer);

		pLayer = pLayer->FindNextLayer();
	}
}

#endif // EXCLUDE_GALS

/***********************************************************************************************

>	static void LayerSGallery::ForceRedrawLayer(Document* pDoc,Layer* pLayer)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/8/94
	Inputs:		pDoc   = ptr that contains this layer
				pLayer = ptr to the layer you want to redraw
	Outputs:	-
	Returns:	-
	Purpose:	Does a ForceRedraw on the layer in the given doc

***********************************************************************************************/

void LayerSGallery::ForceRedrawLayer(Document* pDoc,Layer* pLayer)
{
	Spread* pSpread = pLayer->FindParentSpread();

	if (pSpread != NULL)
	{
		DocRect rect = pLayer->GetUnionBlobBoundingRect();
		pDoc->ForceRedraw(pSpread, rect, FALSE, pLayer);			// Don't allow caches to retain backgrounds

		if (pLayer->IsGuide())
		{
			Node* pNode = pLayer->FindFirstChild(CC_RUNTIME_CLASS(NodeGuideline));
			while (pNode != NULL)
			{
				rect = ((NodeGuideline*)pNode)->GetRenderRect();
				pDoc->ForceRedraw(pSpread, rect, FALSE, pLayer);	// Don't allow caches to retain backgrounds
				pNode = pNode->FindNext(CC_RUNTIME_CLASS(NodeGuideline));
			}
		}
	}
}



/***********************************************************************************************

>	static void LayerSGallery::SetActive(Document* pDoc,Layer* pLayer,BOOL state)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/8/94
	Inputs:		pDoc   = ptr to doc containing layer
				pLayer = ptr that contains this layer
				state  = TRUE means make active, FALSE means make inactive
	Outputs:	-
	Returns:	-
	Purpose:	Calls pLayer->SetActive(state), redrawing and changing the layer's selection
				if necessary.
				DON'T CALL THIS FUNC!! USE MakeActiveLayer() INSTEAD!!!!

***********************************************************************************************/

void LayerSGallery::SetActive(Document* pDoc,Layer* pLayer,BOOL state)
{
	// This flag is TRUE if the selection on the layer should be cleared
	BOOL ClearSelection = FALSE;

	// get state before active change
	BOOL PrevVisibleState = pLayer->IsVisible();
	BOOL PrevLockedState  = pLayer->IsLocked();

	// Change active state
	pLayer->SetActive(state);

	// get state after active change
	BOOL PostVisibleState = pLayer->IsVisible();
	BOOL PostLockedState  = pLayer->IsLocked();

	if (PrevVisibleState != PostVisibleState)
	{
		// If the visible state has changed, redraw the layer
		LayerSGallery::ForceRedrawLayer(pDoc,pLayer);

		// If it has just become invisible, clear the selection
		ClearSelection = (ClearSelection || !PostVisibleState);
	}

	// if it has just become locked, clear the selection
	if (PrevLockedState != PostLockedState)
		ClearSelection = (ClearSelection || PostLockedState);

	// Clear the selection if needed
	if (ClearSelection)
		NodeRenderableInk::DeselectAllOnLayer(pLayer); 
}



/***********************************************************************************************

>	static void LayerSGallery::MakeActiveLayer(Layer* pNewActiveLayer)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/94
	Inputs:		pNewActiveLayer = ptr to the layer you want to be active
	Outputs:	-
	Returns:	-
	Purpose:	Makes the layer active.
				Firstly it will inactivate any other active layers, forcing it to redraw if necessary
				Then it will set this layer to be active, again forcing a redraw if necessary

				If the given new layer is already active, nothing happens

				Note: pNewActiveLayer must be in the currect doc

***********************************************************************************************/

void LayerSGallery::MakeActiveLayer(Layer* pNewActiveLayer, BOOL TellAll)
{
	Spread* pSpread = pNewActiveLayer->FindParentSpread();
	ERROR3IF(pSpread == NULL,"Eh up, the layer has no parent spread");
	if (pSpread == NULL) return;

	Document *pDoc = pSpread->FindParentDocument();
	if (pDoc == NULL) return;
		
	{
		// Remove any other active layers, redrawing if necessary.
		Layer* pLayer = pSpread->FindFirstLayer();
		while (pLayer != NULL)
		{
			if (pLayer->IsActive() && pLayer != pNewActiveLayer)
				LayerSGallery::SetActive(pDoc,pLayer,FALSE);

			pLayer = pLayer->FindNextLayer();
		}
	}

	// Make sure the new layer is active, redrawing if necessary
	if (!pNewActiveLayer->IsActive())
		LayerSGallery::SetActive(pDoc,pNewActiveLayer,TRUE);

#if defined(EXCLUDE_FROM_RALPH) || defined(EXCLUDE_FROM_XARALX)
	//ERROR3("LayerSGallery::MakeActiveLayer supressed broadcast - bad !");
#else
	if (TellAll)
		BROADCAST_TO_ALL(LayerMsg(pNewActiveLayer,LayerMsg::LayerReason::ACTIVE_LAYER_CHANGED));
#endif
}




/***********************************************************************************************

>	static void LayerSGallery::EnsureActiveLayerIntegrity(Spread* pSpread,Layer** ppActiveLayer = NULL)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/3/95
	Inputs:		pSpread = ptr to spread
	Outputs:	-
	Returns:	-
	Purpose:	This checks that there is one and only one active layer in the given spread.
				If there isn't an active layer, the first layer is made active.
				If there is more than on active layer, all but the first active layer are
				made inactive.

***********************************************************************************************/

void LayerSGallery::EnsureActiveLayerIntegrity(Spread* pSpread,Layer** ppActiveLayer)
{
	if (pSpread == NULL) return;

	// Get the first layer in the spread
	Layer* pFirstLayer = pSpread->FindFirstLayer();	

	// Not found an active layer yet
	Layer* pActiveLayer = NULL;

	// Start the search from the first layer
	Layer* pLayer = pFirstLayer;

	while (pLayer != NULL && pActiveLayer == NULL)
	{
		if (pLayer->IsActive())
			pActiveLayer = pLayer;	// Found the active layer, so make a note of it

		pLayer = pLayer->FindNextLayer();
	}

	// If we haven't found any active layers, make the last (top) one active
	// WEBSTER - markn 23/1/97
	// In Webster, always pick the top layer to be the active layer
//#ifndef WEBSTER
	if (pActiveLayer == NULL)
//#endif // WEBSTER
		pActiveLayer = pSpread->FindLastLayer();

	// If we have an active layer, ensure that it really is the one and only active layer
	if (pActiveLayer != NULL)
		LayerSGallery::MakeActiveLayer(pActiveLayer);

	// If caller wants the ptr to the active layer
	if (ppActiveLayer != NULL)
		*ppActiveLayer = pActiveLayer;
}

			

/***********************************************************************************************

>	static void LayerSGallery::MakeTopLayerActive(Spread* pSpread)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/1/95
	Inputs:		pSpread = Spread which should have its top layer made active
	Outputs:	-
	Returns:	-
	Purpose:	Makes the layer active.
				Firstly it will inactivate any other active layers, forcing it to redraw if necessary
				If the new layer is already active, nothing else happens
				Then it will set this layer to be active, again forcing a redraw if necessary

***********************************************************************************************/

void LayerSGallery::MakeTopLayerActive(Spread* pSpread)
{
	// Check for NULL spread
	if (pSpread == NULL)
		return;

	// The top layer is the last layer in the spread's child list
	// so find the last layer and make it active
	Layer* pLayer = pSpread->FindLastLayer();
	if (pLayer != NULL)
		MakeActiveLayer(pLayer);
}


/********************************************************************************************

>	static String_256 LayerSGallery::CreateUniqueLayerID()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com> (based on a Simon routine)
	Created:	4/8/94
	Inputs:		-
	Outputs:	-
	Returns:	A LayerID which is unique for the layer galleries spread. 
	Purpose:	Used to generate a LayerID which is unique for the spread. The name 
				generated is normally of the form "Layer n", where n is the number of layers 
				in the spread + 1. If "Layer n" already exists on the spread then 
				"Layer m" is generated where m is the smallest integer greater than n such 
				that the id is unique.     
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

String_256 LayerSGallery::CreateUniqueLayerID(Spread* pSpread)
{
	if (pSpread == NULL) return String_256("");

	String_256 UniqueLayerID; 
	INT32 NumLayers = 0; 

	Layer* pLayer = pSpread->FindFirstLayer();

	// Calculate how many layers are on the current spread
	while (pLayer != NULL)
	{
		// Don't cound the guide layer - it's LayerID (aka Name) is "Guides"
		if(!pLayer->IsGuide())
		{
			NumLayers++;
		}
		pLayer = pLayer->FindNextLayer();
	}

	INT32 NewLayerNum = NumLayers+1; 

	BOOL Unique;  // Flag used to indicate if the id generated is unique or not 
	do 
	{
		// Construct a first attempt at a unique layer id 'Layer n' 
		// where n = the number of layers on the current spread + 1	 
		UniqueLayerID.MakeMsg(_R(IDS_SGLAYER_LAYER_NUM), /*TEXT("Layer #1%ld"),*/NewLayerNum);
	
		// Check that UniqueLayerID is indeed unique 
		Unique = TRUE; // Until we know better 
		pLayer = pSpread->FindFirstLayer();
		while (pLayer != NULL)
		{	
           	if (UniqueLayerID == pLayer->GetLayerID())
			{
				Unique = FALSE; 

				// UniqueLayerID is not unique so increment NewLayerNum and try again 
				NewLayerNum++;      
				break; 
			}
			pLayer = pLayer->FindNextLayer();
		}
		
	} while (!Unique); 

	return(UniqueLayerID); 
}

#ifndef EXCLUDE_GALS

/***********************************************************************************************

>	BOOL LayerSGallery::CheckVarsAreValid()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/8/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE  - the vars are ok
				FALSE - one or more of the vars is invalid 
	Purpose:	A routine that checks to see if critical vars are OK.
				All it does at the mo is check that pSpread and pDoc are not NULL.
				In debug builds, an ENSURE will go off if something goes wrong

***********************************************************************************************/

BOOL LayerSGallery::CheckVarsAreValid()
{
	return ((pSpread != NULL) && (pDoc != NULL));
}


/***********************************************************************************************

>	void LayerSGallery::EnsureActiveLayerIntegrity()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	This checks that there is one and only one active layer in the gallery's spread.
	SeeAlso:	LayerSGallery::EnsureActiveLayerIntegrity(Spread* pSpread)

***********************************************************************************************/

void LayerSGallery::EnsureActiveLayerIntegrity()
{
	LayerSGallery::EnsureActiveLayerIntegrity(pSpread);
}


/***********************************************************************************************

>	SGDisplayLayer *LayerSGallery::GetSelectedLayerGalItem()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/8/94
	Inputs:		-
	Outputs:	-
	Returns:	A ptr to the selected layer gallery item.
				NULL if there is no selectet item
	Purpose:	Gets the selected layer gallery item
				In debug builds, an ENSURE will go off if there is no selected gal item

***********************************************************************************************/

SGDisplayLayer *LayerSGallery::GetSelectedLayerGalItem()
{
	SGDisplayLayer* pDispLayer = NULL;

	if (CheckVarsAreValid())
	{
		ERROR3IF(DisplayForeground == NULL || DisplayBackground == NULL,"Fore and/or background group(s) are/is NULL");
		if (DisplayForeground == NULL || DisplayBackground == NULL)
			return NULL;

		// Look in the foreground group first
		pDispLayer = ((SGDisplayLayer *) DisplayForeground->FindNextSelectedItem(NULL));

		// If no joy, have a look in the background group
		if (pDispLayer == NULL)
			pDispLayer = ((SGDisplayLayer *) DisplayBackground->FindNextSelectedItem(NULL));

		ERROR3IF(pDispLayer == NULL,"Can't find the selected layer item in the layer gallery");

//		SGDisplayGroup	*DocumentGroup = DisplayTree->FindSubtree(this, pDoc, NULL);
//		if (DocumentGroup != NULL)
//			return((SGDisplayLayer *) DocumentGroup->FindNextSelectedItem(NULL));
	}

	return(pDispLayer);
}



/***********************************************************************************************

>	BOOL LayerSGallery::HasLayerGotChildren(Layer* pLayer)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/8/94
	Inputs:		pLayer = the layer you want to check
	Outputs:	-
	Returns:	TRUE  - The layer has children the user may be interested in - i.e. children
						that actually render something.
				FALSE - The layer has no rendering children
	Purpose:	Scans the layer for children that render something on the screen.
				It ignores attributes, hidden nodes, etc - nodes that don't render anything themselves.
				
				Code moved into Layer::HasLayerGotRenderableChildren 21/7/97

***********************************************************************************************/

BOOL LayerSGallery::HasLayerGotChildren(Layer* pLayer)
{
	ERROR2IF(pLayer == NULL,FALSE,"HasLayerGotChildren bad param!");
	return pLayer->HasLayerGotRenderableChildren();
}

/***********************************************************************************************

>	BOOL LayerSGallery::PrepareToDelete(void)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/8/94
	Purpose:	Scans the current Gallery selection. Possibly asks the user for
				confirmation about the delete. Possibly adjusts the selection as
				appropriate. Then returns a value to indicate whether the remaining
				selection should be deleted.
	SeeAlso:	LayerSGallery::DoDeleteSelection

***********************************************************************************************/
 
BOOL LayerSGallery::PrepareToDelete(void)
{
	SGDisplayLayer* pLayerGalItem = GetSelectedLayerGalItem();

	if (pLayerGalItem != NULL)
	{
		if (HasLayerGotChildren(pLayerGalItem->GetDisplayedLayer()) != NULL)
		{
			// If some are in use, determine if the user really wants to delete them
			INT32 ButtonPressed = AskQuestion(_R(IDS_LAYER_CONSTAINSOBJECTS),
											_R(IDS_LAYER_DELETE), _R(IDB_CANCEL), 0, 0,
											2, 2);
										  
			if (ButtonPressed == 1)		// Force-delete all items, so go ahead and delete
				return(TRUE);

			// Delete was cancelled (or Help pressed) - return FALSE
			return(FALSE);
		}
		else
			return(TRUE);
	}
	
	return (pLayerGalItem != NULL);
}



/***********************************************************************************************

>	BOOL LayerSGallery::DoDeleteSelection(void)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/8/94
	Returns:	FALSE if it couldn't delete the selection. (This should, if at all possible
				leave the entire selection undeleted in this case)

	Purpose:	FORCIBLY deletes the current gallery selection. By the time this function
				is called, PrepareToDelete has made sure that it is safe to continue.
				This must simply invoke a forced delete upon the selection.
				Deletion of colours occurs by hiding (with undo)

	SeeAlso:	LayerSGallery::PrepareToDelete; ColourManager::HideColours

***********************************************************************************************/
 
BOOL LayerSGallery::DoDeleteSelection(void)
{
	SGDisplayLayer* pLayerGalItem = GetSelectedLayerGalItem();

	if (pLayerGalItem != NULL)
	{
		OpLayerGalParam Param(LAYER_DELETE,pSpread);
		Param.pLayer = pLayerGalItem->GetDisplayedLayer();

		OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_LAYERGALCHANGE); 
		if (pOpDesc != NULL)
			pOpDesc->Invoke((OpParam*)&Param);
		else
		{
			ERROR3("Couldn't find OPTOKEN_LAYERGALCHANGE op descriptor");
		}
		return (TRUE);
	}

	return (pLayerGalItem != NULL);
}





/***********************************************************************************************

>	void LayerSGallery::DoCreateNewItem(void)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/8/94

	Purpose:	This function is meant to be overridden in derived classes
				It creates a new item and adds it to the Gallery DisplayList
				The item may be created by cloning/deriving/basing-on the
				selected item, depending on the type of Gallery concerned.

***********************************************************************************************/

void LayerSGallery::DoCreateNewItem(void)
{
	if (!CheckVarsAreValid())
		return;
	LayerDlgParam Param(LAYER_NEW,pSpread,NULL);
	LayerNameDlg::StartUp(&Param);

}

/***********************************************************************************************

>	void LayerSGallery::DoChangeName()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/94
	Inputs:		-
	Returns:	-
	Purpose:	Pops up the name changing dialog for the selected layer

***********************************************************************************************/

void LayerSGallery::DoChangeName()
{
	SGDisplayLayer* pLayerGalItem = GetSelectedLayerGalItem();

 	if (pLayerGalItem != NULL)
	{
		LayerDlgParam Param(LAYER_CHANGE_NAME,NULL,pLayerGalItem->GetDisplayedLayer());
		LayerNameDlg::StartUp(&Param);
	}
}




/***********************************************************************************************

>	void LayerSGallery::DoCopyLayer()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/94
	Inputs:		-
	Returns:	-
	Purpose:	Pops up the name changing dialog for the selected layer, and if OK is clicked
				a copy of the layer is made with the new name.

***********************************************************************************************/

void LayerSGallery::DoCopyLayer()
{
	SGDisplayLayer* pLayerGalItem = GetSelectedLayerGalItem();

 	if (pLayerGalItem != NULL)
	{
		LayerDlgParam Param(LAYER_COPY,NULL,pLayerGalItem->GetDisplayedLayer());
		LayerNameDlg::StartUp(&Param);
	}
}


/***********************************************************************************************

>	void LayerSGallery::DoLayerProperties()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/10/95
	Inputs:		-
	Returns:	-
	Purpose:	Opens the layer properties dlg

***********************************************************************************************/

void LayerSGallery::DoLayerProperties()
{
#ifndef STANDALONE
	// layer property tabs not present in viewer
	OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_LAYERPROPERTYTABS);

	if (pOpDesc != NULL)
	{
		String_256 Str;
		OpState State = pOpDesc->GetOpsState(&Str);
		if (!State.Greyed)
			pOpDesc->Invoke();
		else
		{
// Changed for Frame/Layer integration - RanbirR
//#ifdef WEBSTER
//			GIFAnimationPropertyTabs * pTabHandler = GIFAnimationPropertyTabsDlg::GetGIFAnimationPropertiesTabs();
//#else
			LayerPropertyTabs * pTabHandler = LayerPropertyTabsDlg::GetLayerPropertiesTabs();	
//#endif // webster -RanbirR
			if (pTabHandler)
			{
				DialogTabOp* pDlg =  pTabHandler->GetTabbedDlg();
				if (pDlg != NULL)
					pDlg->Open();
			}
		}
	}
#endif
}

/***********************************************************************************************

>	void LayerSGallery::DoCreateGuideLayer()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/10/95
	Inputs:		-
	Returns:	-
	Purpose:	Creates a guide layer

***********************************************************************************************/

void LayerSGallery::DoCreateGuideLayer()
{
#ifndef WEBSTER
 	OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_GUIDELINE);
	ERROR3IF(pOpDesc == NULL,"FindOpDescriptor(OPTOKEN_GUIDELINE) failed");

	if (pOpDesc != NULL)
	{
		OpGuidelineParam GuidelineParam;

		GuidelineParam.Method = GUIDELINEOPMETHOD_CREATE_GUIDE_LAYER;

		pOpDesc->Invoke(&GuidelineParam);
	}
#endif // WEBSTER
}

/***********************************************************************************************

>	void LayerSGallery::DoChangeAllVisible(BOOL AllVisible)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/8/94
	Inputs:		AllVisible - TRUE  means all layers become visible
						   	  FALSE means only visible layers are visible

	Returns:	-
	Purpose:	Called when the AllVisible button is clicked in the layers gallery.
				Effects the visibility of all layers, regardless of their visibility

***********************************************************************************************/

void LayerSGallery::DoChangeAllVisible(BOOL AllVisible)
{
	if (!CheckVarsAreValid()) return;

	// Get the state prior to the change
	BOOL PrevAllVisible = pDoc->IsAllVisible();

	// If no change, return
	if (PrevAllVisible == AllVisible) return;

	Layer* pLayer = pSpread->FindFirstLayer();

	while (pLayer != NULL)
	{
		// Do not include the page background layer in this process as this is very bad!
		if (!pLayer->IsPageBackground())
		{
			pDoc->SetAllVisible(PrevAllVisible);
			BOOL PrevVisible = pLayer->IsVisible();
			BOOL PrevLocked  = pLayer->IsLocked();

			pDoc->SetAllVisible(AllVisible);
			BOOL PostVisible = pLayer->IsVisible();
			BOOL PostLocked  = pLayer->IsLocked();

			if (PrevVisible != PostVisible)
			{
				LayerSGallery::ForceRedrawLayer(pDoc,pLayer);

				if (!PostVisible)
					NodeRenderableInk::DeselectAllOnLayer(pLayer); 
			}
		}

		pLayer = pLayer->FindNextLayer();
	}

	pDoc->SetAllVisible(AllVisible);

	// inform that visible layers have changed
	BROADCAST_TO_ALL(LayerMsg(pLayer, LayerMsg::LayerReason::LAYER_VISIBILITY_CHANGED));
}



/***********************************************************************************************

>	void LayerSGallery::DoChangeMultilayer(BOOL Multilayer)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/8/94
	Inputs:		Multilayer - TRUE  means all layers become unlocked
						   	 FALSE means only locked layers are locked

	Returns:	-
	Purpose:	Called when the Multilayer button is clicked in the layers gallery.
				Effects the locked state of all layers, regardless of their individual locked state

***********************************************************************************************/

void LayerSGallery::DoChangeMultilayer(BOOL Multilayer)
{
	if (!CheckVarsAreValid()) return;

	// Get the state prior to the change
	BOOL PrevMultilayer = pDoc->IsMultilayer();

	// If no change, return
	if (PrevMultilayer == Multilayer) return;

	Layer* pLayer = pSpread->FindFirstLayer();
	BOOL PrevLocked;
	BOOL PostLocked;

	while (pLayer != NULL)
	{
		// Do not include the page background layer in this process as this is very bad!
		if (!pLayer->IsPageBackground())
		{
			pDoc->SetMultilayer(PrevMultilayer);
			PrevLocked = pLayer->IsLocked();
			pDoc->SetMultilayer(Multilayer);
			PostLocked = pLayer->IsLocked();

			if ((PrevLocked != PostLocked) && PostLocked)
				NodeRenderableInk::DeselectAllOnLayer(pLayer); 
		}

		pLayer = pLayer->FindNextLayer();
	}

	pDoc->SetMultilayer(Multilayer);
}



/***********************************************************************************************

>	static void LayerSGallery::DoChangeVisible(Layer* pLayer,BOOL Visible)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com> (Jason, ripped from the LayerGallery, 1/2/95)
	Created:	8/8/94
	Inputs:		pLayer = the layer to change
				Visible - TRUE means make the displayed layer visible
						  FALSE means make invisiblr

	Returns:	-
	Purpose:	Called when the visible button is clicked in a layer gallery item.
				Changes the visibility of the displayed layer.

	Notes:		Does not redraw the layer in the display list

***********************************************************************************************/

void LayerSGallery::DoChangeVisible(Layer* pLayer,BOOL Visible)
{
	ERROR3IF(pLayer == NULL,"pLayer == NULL");
	if (pLayer == NULL) return;

	BOOL PrevVisible = pLayer->IsVisible();
	pLayer->SetVisible(Visible);
	BOOL PostVisible = pLayer->IsVisible();

	if (PrevVisible != PostVisible)
	{
		// Force a redraw if the layer's visibility has changed
		LayerSGallery::ForceRedrawLayer(pLayer->FindDocument(), pLayer);

		// inform that visible layers have changed
		BROADCAST_TO_ALL(LayerMsg(pLayer, LayerMsg::LayerReason::LAYER_VISIBILITY_CHANGED));

		// If layer has just become invisible, remove the selections on the layer 
		if (!PostVisible)
			NodeRenderableInk::DeselectAllOnLayer(pLayer); 
	}
}

/***********************************************************************************************

>	static void LayerSGallery::DoChangeLocked(Layer* pLayer,BOOL Locked)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com> (Jason, ripped from the LayerGallery, 1/2/95)
	Created:	8/8/94
	Inputs:		pLayer = the layer to change
				Locked - TRUE means make the displayed layer locked
						 FALSE means unlock

	Returns:	-
	Purpose:	Called when the Locked button is clicked in a layer gallery item.
				Changes the lockedness of the displayed layer.

	Notes:		Does not redraw the layer in the display list

***********************************************************************************************/

void LayerSGallery::DoChangeLocked(Layer* pLayer,BOOL Locked)
{
	ERROR3IF(pLayer == NULL,"pLayer == NULL");
	if (pLayer == NULL) return;

	BOOL PrevLocked = pLayer->IsLocked();
	pLayer->SetLocked(Locked);
	BOOL PostLocked = pLayer->IsLocked();

	if (PrevLocked != PostLocked)
		NodeRenderableInk::DeselectAllOnLayer(pLayer); 
}


/***********************************************************************************************

>	void LayerSGallery::DoChangeLayerStatus(Layer* pLayer,const LayerStatus& NewStatus)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/8/94
	Inputs:		pLayer = ptr to the layer to change
				NewStatus = reference to the status that the selected layer should
							take on board.
	Returns:	-
	Purpose:	General undoable way of changing the selected layer's status

***********************************************************************************************/

void LayerSGallery::DoChangeLayerStatus(Layer* pLayer, const LayerStatus& NewStatus)
{
	if (!CheckVarsAreValid()) return;

	OpLayerGalParam Param(LAYER_CHANGE, pSpread);

	Param.pLayer = pLayer;
	Param.Status = NewStatus;

	OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_LAYERGALCHANGE); 
	if (pOpDesc != NULL)
		pOpDesc->Invoke((OpParam*)&Param);
	else
	{
		ENSURE(FALSE,"Couldn't find OPTOKEN_LAYERGALCHANGE op descriptor");
	}
}

/***********************************************************************************************

>	void LayerSGallery::DoChangeLayerState(Layer* pLayer,OpLayerGalReason Reason,BOOL NewState)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/3/95
	Inputs:		pLayer = ptr to the layer to change
				Reason = the flag which to change
				NewState = The new state of the flag to change
	Returns:	-
	Purpose:	Undoable way of changing the layer's state

***********************************************************************************************/

void LayerSGallery::DoChangeLayerState(Layer* pLayer,OpLayerGalReason Reason,BOOL NewState)
{
	if (!CheckVarsAreValid()) return;

	OpLayerGalParam Param(Reason, pSpread);

	Param.pLayer 	= pLayer;
	Param.NewState  = NewState;
	Param.pLayerSGal= this;

	OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_LAYERGALCHANGE); 
	if (pOpDesc != NULL)
		pOpDesc->Invoke((OpParam*)&Param);
	else
	{
		ERROR3("Couldn't find OPTOKEN_LAYERGALCHANGE op descriptor");
	}
}




/***********************************************************************************************

>	void LayerSGallery::DoMoveLayer(MoveLayerType Reason)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/94
	Inputs:		Reason = how the layer should be moved
	Returns:	-
	Purpose:	Called when the Locked button is clicked in the layers gallery.
				Changes the lockedness of the selected layer, if there is one.

***********************************************************************************************/

void LayerSGallery::DoMoveLayer(MoveLayerType Reason)
{
	SGDisplayLayer* pLayerGalItem = GetSelectedLayerGalItem();

	if (pLayerGalItem != NULL)
	{
		OpLayerGalParam Param(LAYER_MOVE, pSpread);

		Param.pLayer = pLayerGalItem->GetDisplayedLayer();

		switch (Reason)
		{
			case (MOVELAYER_UPONE) :
				Param.pContextNode	= Param.pLayer->FindNextLayer();
				Param.AttDir		= NEXT;
				break;

			case (MOVELAYER_DOWNONE) :
				Param.pContextNode	= Param.pLayer->FindPrevLayer();
				Param.AttDir 		= PREV;
				break;

			default:
				ENSURE(FALSE,"Unrecognised MoveLayer reason");
				break;
		}

		if (Param.pContextNode != NULL)
		{
			// Redraw after the op if both this layer and the one it's moving in front/behind
			// is also visible
			Param.MoveRedraw = ((Layer*)Param.pContextNode)->IsVisible() &&
									Param.pLayer->IsVisible();

			// The above won't work in an undo or redo, so always redraw
			Param.MoveRedraw = TRUE;

			OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_LAYERGALCHANGE); 
			if (pOpDesc != NULL)
				pOpDesc->Invoke((OpParam*)&Param);
			else
			{
				ENSURE(FALSE,"Couldn't find OPTOKEN_LAYERGALCHANGE op descriptor");
			}
		}
	}
}


/***********************************************************************************************

>	BOOL LayerSGallery::IsSelectedItemGuideLayer()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/95
	Inputs:		-
	Returns:	TRUE if the selected layer is a guide layer or no selected layer, FALSE otherwise
	Purpose:	General function for finding out if the selected item is a guide layer
	Notes:		Need not be overridden - the default action is to do nothing extra
				to the normal shade/unshade operation.
	SeeAlso:	Gallery::ShadeGallery; Gallery::UnshadeGallery

***********************************************************************************************/

BOOL LayerSGallery::IsSelectedItemGuideLayer()
{
	SGDisplayLayer* pItem = GetSelectedLayerGalItem();
	Layer* pLayer = NULL;

	if (pItem != NULL)
		pLayer = pItem->GetDisplayedLayer();

	if (pLayer != NULL)
		return (pLayer->IsGuide());

	return TRUE;
}

/***********************************************************************************************

>	void LayerSGallery::DoShadeGallery(BOOL ShadeIt)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/9/94
	Inputs:		ShadeIt -	TRUE if the gallery is being shaded
							FALSE if the gallery is being unshaded
	Purpose:	Called by the base class whenever the shaded status of the gallery
				is changed, to allow derived galleries to shade/unshade any extra
				controls that they provide in the window. ONLY called if the gallery
				is actually open/visible.

	Notes:		Need not be overridden - the default action is to do nothing extra
				to the normal shade/unshade operation.
	SeeAlso:	Gallery::ShadeGallery; Gallery::UnshadeGallery

***********************************************************************************************/

void LayerSGallery::DoShadeGallery(BOOL ShadeIt)
{
	BOOL fEnableMove = FALSE;
	OpDescriptor* pDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_MOVE_SEL_TO_ACTIVE_LAYER);
	if (pDesc != 0)
	{
		String_256 str;
		OpState os = pDesc->GetOpsState(&str);
		fEnableMove = !os.Greyed;
	}

	EnableGadget(_R(IDC_LAYER_MOVE_TO_ACTIVE),	!ShadeIt && fEnableMove);
	EnableGadget(_R(IDC_GALLERY_UPONE),			!ShadeIt);
	EnableGadget(_R(IDC_GALLERY_DOWNONE),		!ShadeIt);
	EnableGadget(_R(IDC_GALLERY_PROPERTIES),	!ShadeIt);
	EnableGadget(_R(IDC_GALLERY_COPY),			!ShadeIt && !IsSelectedItemGuideLayer());

	EnableGadget(_R(IDC_BTN_MULTILAYER),		!ShadeIt);
	EnableGadget(_R(IDC_BTN_ALLVISIBLE),		!ShadeIt);

	//EnableGadget(_R(IDC_GALLERY_NAME),		!ShadeIt);
}


/********************************************************************************************

>	virtual SGDisplayItem *LayerSGallery::CopyDisplayItem(SGDisplayItem *SourceItem, 
									SGDisplayGroup *DestGroup,
									SGDisplayItem *TargetPosition = NULL)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com> (from a Jason func)
	Created:	21/3/95

	Inputs:		SourceItem - The item to copy elsewhere in the tree (see below)

				DestGroup - The group into which the item should be inserted

				TargetPosition - NULL (to add to the end of the sibling list), or
				points to the item BEFORE which the SourceItem will be copied.

	Returns:	NULL (failed) or a pointer to the new (copied) display item

	Purpose:	"Copies" the existing node in the tree in an appropriate fashion.
				
				This method is normally called when a gallery-organising drag completes,
				and it is discovered that the dragged item(s) have been dragged to a 
				different display group.

	Notes:		This method should be overridden by derived galleries to provide
				appropriate behaviour (some galleries (e.g colour) will copy the real-item
				that the given display-item references to the new group (document), while
				other galleries (layer) may just move the item after all).

				Note the handy InsertCopiedItem and MoveBefore/After methods which
				are available to take all of the hard work out of copying/moving items!

				See the body of this method in the source code for example pseudocode.
				For real code, see the Colour Gallery (sgcolour.cpp)

	SeeAlso:	SuperGallery::InsertCopiedItem; SGDisplayItem::MoveBefore;
				SGDisplayItem::MoveAfter; SGColour::CopyDisplayItem

********************************************************************************************/

SGDisplayItem *LayerSGallery::CopyDisplayItem(SGDisplayItem *SourceItem, 
						SGDisplayGroup *DestGroup, SGDisplayItem *TargetPosition)
{
	ERROR3IF(SourceItem == NULL || DestGroup == NULL, "Illegal NULL param");
	if (SourceItem == NULL || DestGroup == NULL)
		return(NULL);

	ERROR3IF(!IS_A(DestGroup,SGLayerGroup),"DestGroup is not a SGLayerGroup");
	ERROR3IF(!IS_A(SourceItem,SGDisplayLayer),"SourceItem is not a SGDisplayLayer");
	if (!IS_A(DestGroup,SGLayerGroup) || !IS_A(SourceItem,SGDisplayLayer))
		return(NULL);

	SGLayerGroup* 	pDestLayerGroup  = (SGLayerGroup*)DestGroup;
	SGDisplayLayer* pSourceLayerItem = (SGDisplayLayer*)SourceItem;

	SGDisplayItem* 	pDestItem 	= TargetPosition;
	BOOL 			Before 		= TRUE;

	if (pDestItem == NULL)
	{
		pDestItem = (SGDisplayItem*) pDestLayerGroup->GetChild();
		Before = TRUE;

		if (pDestItem == NULL)
		{
			SGDisplayNode* pNode = SourceItem;
			while (pNode != NULL)
			{
				pDestItem = (SGDisplayItem*) pNode;
				if (pDestLayerGroup->IsForeground())
					pNode = pNode->GetPrevious();
				else
					pNode = pNode->GetNext();
			}

			Before = pDestLayerGroup->IsForeground();
		}
		else
		{
			SGDisplayItem* pLastDestItem = NULL;
			while (pDestItem != NULL)
			{
				pLastDestItem = pDestItem;
				pDestItem = (SGDisplayItem*)(pDestItem->GetNext());
			}

			pDestItem = pLastDestItem;
			Before = FALSE;
		}
	}

	ERROR3IF(pDestItem == NULL,"pDestItem is still NULL");
	if (pDestItem == NULL)
		return(NULL);

	// Move the display item around
	SourceItem->RemoveFromTree();		// Delink from current displaytree position
	SuperGallery::InsertCopiedItem(SourceItem, DestGroup, TargetPosition);

	if (pDestItem != NULL)
	{
		ERROR3IF(!IS_A(pDestItem,SGDisplayLayer),"SourceItem is not a SGDisplayLayer");
		if (!IS_A(pDestItem,SGDisplayLayer))
			return(NULL);

		SGDisplayLayer* pDestLayerItem = (SGDisplayLayer*)pDestItem;
		pDestLayerItem->MoveLayer(pSourceLayerItem,Before,TRUE);
	}

	return(SourceItem);
}



/********************************************************************************************

>	virtual BOOL LayerSGallery::InitMenuCommands(void)
												 
	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/9/95

	Returns:	TRUE for success

	Purpose:	Initialises any menu commands that this gallery needs.

	Notes:		Will only create the menu commands once - further calls in the future
				will return TRUE immediately wihtout doing anything.

********************************************************************************************/

BOOL LayerSGallery::InitMenuCommands(void)
{
	static BOOL MenusInitialised = FALSE;

	BOOL ok = TRUE;

	if (!MenusInitialised)
	{
		// Initialise menu command Ops
		// "Special" entries for over-list menu
		ok = ok && InitMenuCommand((StringBase *) &SGCmd_Copy, 		_R(IDS_SGMENU_COPY));
		ok = ok && InitMenuCommand((StringBase *) &SGCmd_Delete, 	_R(IDS_SGMENU_DELETE));
		ok = ok && InitMenuCommand((StringBase *) &SGCmd_New, 		_R(IDS_NEW_LAYER));
		ok = ok && InitMenuCommand((StringBase *) &SGCmd_Create, 	_R(IDS_CREATE_GUIDE_LAYER));
		ok = ok && InitMenuCommand((StringBase *) &SGCmd_Properties,_R(IDS_SGMENU_PROPERTIES));

		MenusInitialised = TRUE;
	}

	return(ok);
}



/********************************************************************************************

>	virtual BOOL LayerSGallery::BuildCommandMenu(GalleryContextMenu *TheMenu, SGMenuID MenuID)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/9/95

	Inputs:		TheMenu - The menu to add commands to
				MenuID  - The type of menu (over-list or from-options-button) to create

	Returns:	TRUE if it succeeded

	Purpose:	To build a menu of commands to be popped up over the gallery.
	
	Notes:		The Layer gallery provides only a simple item-pop-up menu

********************************************************************************************/

BOOL LayerSGallery::BuildCommandMenu(GalleryContextMenu *TheMenu, SGMenuID MenuID)
{
 	BOOL ok = TRUE;

	if (MenuID == SGMENU_OVERITEM)
	{
		// Over-list menu
		SGDisplayLayer* pLayerGalItem = GetSelectedLayerGalItem();

#ifndef STANDALONE
// no layer properties tab present on standalone version
		if (pLayerGalItem != NULL)
		{
// Changed for Frame/Layer integration.
//#ifdef WEBSTER
//			GIFAnimationPropertyTabs * pTabHandler = GIFAnimationPropertyTabsDlg::GetGIFAnimationPropertiesTabs();
//#else
			LayerPropertyTabs * pTabHandler = LayerPropertyTabsDlg::GetLayerPropertiesTabs();
//#endif //webster
			if (pTabHandler)
			{
				pTabHandler->SetCurrentLayer(pLayerGalItem->GetDisplayedLayer());
			}
		}
#endif
		ok = ok && AddCommand(TheMenu, (StringBase *) &SGCmd_New);
		ok = ok && AddCommand(TheMenu, (StringBase *) &SGCmd_Copy);
		ok = ok && AddCommand(TheMenu, (StringBase *) &SGCmd_Delete,TRUE);
		ok = ok && AddCommand(TheMenu, (StringBase *) &SGCmd_Create);
#ifndef STANDALONE
		ok = ok && AddCommand(TheMenu, (StringBase *) &SGCmd_Properties);
#endif
	}

	return(ok);
}



/********************************************************************************************

>	virtual OpState LayerSGallery::GetCommandState(StringBase *CommandID, String_256 *ShadeReason)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/9/95

	Inputs:		CommandID - TheString ID of the command
	Outputs:	ShadeReason - If you return (OpState.Greyed == TRUE) then this should be filled
				ion with the reason that the item is shaded/greyed.

	Returns:	An OpState indicating the current menu item state.

	Purpose:	To determine the state of a given menu item. This method is an exact
				parallel to an Op's GetState method (in fact, it is called by an Op's GetState)
	
	Notes:		Override this method to provide state info for your special commands
				Call the base class for unknown commands to allow it to handle them for you

				The base class handles all of these (maybe more - see the base class help)
					Properties, Sort, Find;
					New, Edit, Delete, Redefine;
					NextGroup, PrevGroup, FoldGroup, UnfoldGroup;

********************************************************************************************/

OpState LayerSGallery::GetCommandState(StringBase *CommandID, String_256 *ShadeReason)
{
	OpState State;

	if (AmShaded)		// No commands available while the gallery is shaded
	{
		State.Greyed = TRUE;
		return(State);
	}

	if (*CommandID == SGCmd_Copy 	|| // --- Copy/Delete/Properties
		*CommandID == SGCmd_Delete	||
		*CommandID == SGCmd_Properties)	
	{
		SGDisplayLayer* pItem = GetSelectedLayerGalItem();

		if (pItem == NULL)
		{
			State.Greyed = TRUE;
			ShadeReason->MakeMsg(_R(IDS_SGSHADE_SINGLECOL));
		}
		else if (*CommandID == SGCmd_Copy)			// Can't copy Guide layers
			State.Greyed = IsSelectedItemGuideLayer();
#ifndef STANDALONE
		else if (*CommandID == SGCmd_Properties)	// Shade when dlg is open
		{
			//State.Greyed = (LayerPropertyTabs::GetTabbedDlg() != NULL);
//#ifdef WEBSTR
//			GIFAnimationPropertyTabs * pTabHandler = GIFAnimationPropertyTabsDlg::GetGIFAnimationPropertiesTabs();
//#else
			LayerPropertyTabs * pTabHandler = LayerPropertyTabsDlg::GetLayerPropertiesTabs();
//#endif //webster RanbirR
			if (pTabHandler)
				State.Greyed = (pTabHandler->GetTabbedDlg() != NULL);
			else
				State.Greyed = TRUE;	
		}
#endif
	}
	else if (*CommandID == SGCmd_Create)
	{
		Spread* pSpread = Document::GetSelectedSpread();
		Layer* pGuideLayer = NULL;

		if (pSpread != NULL)
			pGuideLayer = pSpread->FindFirstGuideLayer();

		State.Greyed = (pGuideLayer != NULL);
	}


	// Don't call the base class 'cos we don't want any other commands

	return(State);
}



/********************************************************************************************

>	virtual void LayerSGallery::DoCommand(StringBase *CommandID)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/9/95

	Inputs:		CommandID - The String ID of the command

	Purpose:	To apply a given command when it is chosen from the menu.
	
	Notes:		Override this method to provide handling for your special commands.
				Call the base class if you don't recognise the command, so that it can
				handle standard commands.

				The base class handles all of these (maybe more - see the base class help)
					Properties, Sort, Find;
					New, Edit, Delete, Redefine; (it calls ApplyAction as appropriate)
					NextGroup, PrevGroup, FoldGroup, UnfoldGroup;

********************************************************************************************/

void LayerSGallery::DoCommand(StringBase *CommandID)
{
	if (*CommandID == SGCmd_Copy)
		DoCopyLayer();
	else if (*CommandID == SGCmd_New)
		DoCreateNewItem();
	else if (*CommandID == SGCmd_Delete)
	{
		if (PrepareToDelete())
			DoDeleteSelection();
	}
	else if (*CommandID == SGCmd_Properties)
		DoLayerProperties();
	else if (*CommandID == SGCmd_Create)
		DoCreateGuideLayer();

	// Don't call the base class 'cos we don't want any other commands
}

/********************************************************************************************

>	BOOL LayerSGallery::CloseLayerGallery()

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/10/97
	Inputs:		-
	Returns:	TRUE if sucessful, else FALSE.
	
********************************************************************************************/

BOOL LayerSGallery::CloseLayerGallery()
{
	String_32 Name(_R(IDS_LAYERGAL_GALLNAME));
	DialogBarOp* pDialogBarOp = DialogBarOp::FindDialogBarOp(Name);

	// Ensure a valid ptr.
	if (pDialogBarOp == NULL)
		return FALSE;
	
	if (pDialogBarOp->GetRuntimeClass() == CC_RUNTIME_CLASS(LayerSGallery))
	{
		GIFAnimationPropertyTabs::SetFrameGalleryOpen(FALSE);

		// Toggle the visible state of the gallery window
		pDialogBarOp->SetVisibility( FALSE );
	}
	else
	{
		ERROR3("Got the frame gallery but it's not of the LayerSGallery class");
	}

	SGInit::UpdateGalleryButton(OPTOKEN_DISPLAYLAYERGALLERY, FALSE);

	// Everything ok.
	return TRUE;
}

/********************************************************************************************

>	BOOL LayerSGallery::OpenFrameGallery()

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/10/97
	Inputs:		-
	Returns:	TRUE if the document is Frame based, else FALSE.

********************************************************************************************/

BOOL LayerSGallery::OpenFrameGallery()
{
	// Find the OP. for the Layer gallery.
	OpDescriptor* OpDesc;
	OpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_DISPLAYFRAMEGALLERY);

	// Ensure we get a ptr to the OP.
	if(!OpDesc)
		return FALSE;

	// ok, now open the layergallery.
	OpDesc->Invoke();		 

	// Lets go back.
	return TRUE;
}


/********************************************************************************************

>	BOOL LayerSGallery::IsLayerMode()

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/10/97
	Inputs:		Pointer to the document.
	Returns:	TRUE if the document is Frame based, else FALSE.

********************************************************************************************/

BOOL LayerSGallery::IsLayerMode(Document* pDoc)
{
	ERROR3IF(pDoc == NULL,"pDoc == NULL");

	// flag to determine the document mode.
	BOOL LayerMode = TRUE;

	// Get a ptr to the selected spread
	Spread* pSpread = pDoc->GetSelectedSpread();

	// Ensure a valid spread ptr.
	if(pSpread)
	{
		// Are there any frame layers?
		Layer* pFrameLayer = pSpread->FindFirstFrameLayer();	

		//If a frame layer exists, then this is an animation doc.
		if (pFrameLayer)
			LayerMode = FALSE;
	}

	// return the document mode.
	return LayerMode;
}






/********************************************************************************************

>	BOOL LayerSGallery::DoMoveSelectionToActiveLayer()

	Author:		Simon_Knight (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/5/00
	Inputs:		-
	Returns:	TRUE if op exists

********************************************************************************************/
BOOL LayerSGallery::DoMoveSelectionToActiveLayer()
{
	// Find the OP. for the Layer gallery.
	OpDescriptor* OpDesc;
	OpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_MOVE_SEL_TO_ACTIVE_LAYER);

	// Ensure we get a ptr to the OP.
	if(!OpDesc)
		return FALSE;

	// ok, now open the layergallery.
	OpDesc->Invoke();		 

	// Lets go back.
	return TRUE;
}




#endif // EXCLUDE_GALS
