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

// SGFrame.cpp - Frame SuperGallery classes - FrameSGallery and SGDisplayFrame


#include "camtypes.h"

#include "app.h"		// For GetApplication()
//#include "gallery.h"	// For _R(IDC_GALLERY_LISTBOX)
//#include "galres.h"
//#include "galstr.h"
#include "layergal.h"
#include "sgdrag.h"
#include "sginit.h"
#include "sgframe.h"
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
//#include "prevwres.h"	// _R(IDS_FRAMEGAL_FRAMES) _R(IDB_FGAL_OVERLAY)
#include "frameops.h"	// OPTOKEN_FRAME_DELETEFRAME ...
#include "prevwdlg.h"
#include "keypress.h"
#include "aprps.h"
//#include "zordops.h"	// OPTOKEN_COMBINELAYERSTOFRAMELAYER
#include "progress.h"
#include "helpuser.h"	//For help button
//#include "xshelpid.h"	//For help button
//#include "helppath.h"

void Beep();

#ifndef EXCLUDE_GALS
// Implement the dynamic class bits...
CC_IMPLEMENT_DYNCREATE(FrameSGallery, LayerSGallery)
CC_IMPLEMENT_DYNAMIC(SGFrameGroup, SGLayerGroup)
CC_IMPLEMENT_DYNAMIC(SGDisplayFrame, SGDisplayLayer)
#else
CC_IMPLEMENT_DYNCREATE(FrameSGallery, Operation)
#endif

// This line mustn't go before any CC_IMPLEMENT_... macros
#define new CAM_DEBUG_NEW

#ifndef EXCLUDE_GALS

/***********************************************************************************************

>	SGFrameGroup::SGFrameGroup(SuperGallery *ParentGal,
					Document *ParentDoc = NULL, Library *ParentLib = NULL, BOOL IsForeground = TRUE);

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com> (Based on Jason's Layer gallery code)
	Created:	16/4/97 (Based on Jason's Layer gallery code)

	Inputs:		ParentGal - points to the SuperGallery object which 'owns' this node
				ParentDoc - NULL, or a pointer to the document this group references
				ParentLib - NULL, or a pointer to the library this group references

	Purpose:	SGFrameGroup constructor. Initialises the Group's parent pointers to
				point at its parent(s). Note that generally speaking, one of ParentDoc,
				ParentLib will be NULL, and the other will be non-NULL.

	Notes:		This constructor does nothing - jusat passes the call on to the baseclass
				constructor of identical proportions.

	SeeAlso:	SGDisplayGroup::SGDisplayGroup

***********************************************************************************************/

SGFrameGroup::SGFrameGroup(SuperGallery *ParentGal,
							Document *ParentDoc, Library *ParentLib,BOOL IsForeground)
				: SGLayerGroup(ParentGal, ParentDoc, ParentLib, IsForeground)
{
	Foreground = IsForeground;
}



/***********************************************************************************************

>	virtual BOOL SGFrameGroup::HandleEvent(SGEventType EventType, void *EventInfo,
											 SGMiscInfo *MiscInfo)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com> (Based on Jason's Layer gallery code)
	Created:	16/4/97 (Based on Jason's Layer gallery code)

	Inputs:		See SGDisplayNode::HandleEvent

	Returns:	TRUE if the event was handled successfully
				FALSE if it was not

	Purpose:	Handles a SuperGallery DisplayTree event

	Notes:		This overrides the SGDisplayNGroup::HandleEvent method, to give a group-type
				thing which does wierd stuff for layer galleries (i.e. does not fold,
				and gives column titles in the 'group' display)

	SeeAlso:	SGDisplayGroup::HandleEvent

***********************************************************************************************/

BOOL SGFrameGroup::HandleEvent(SGEventType EventType, void *EventInfo,
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
					SGDisplayFrame::CalculateButtonRects(MiscInfo, &MyRect,
														 &VisibleBtn, &LockedBtn, &SnapBtn);

					DrawBitmap(RedrawInfo->Renderer, &VisibleBtn, _R(IDB_FGAL_SOLID));
					DrawBitmap(RedrawInfo->Renderer, &LockedBtn,  _R(IDB_FGAL_OVERLAY));

					// And finally, render the title text
					MyRect.lo.x += SG_GapBeforeText;	// Leave a small gap before text
					if (MyRect.lo.x < MyRect.hi.x)		// If still room left, draw title
					{
						RedrawInfo->Renderer->SetFixedSystemTextColours(&RedrawInfo->Foreground, &Col);
						
						String_256 NewTitleText;

						UINT32 IDS = _R(IDS_FRAMEGAL_FRAMES);

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
SGDisplayFrame *SGDisplayFrame::LastClickedLayer = NULL;


/***********************************************************************************************

>	SGDisplayFrame::SGDisplayFrame()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com> (Based on Jason's Layer gallery code)
	Created:	16/4/97 (Based on Jason's Layer gallery code)

	Purpose:	SGDisplayFrame constructor
				DON'T call this constructor. It ERROR3's. Call the other constructor

***********************************************************************************************/

SGDisplayFrame::SGDisplayFrame() : SGDisplayLayer()
{
	// Let the baseclass handle it
}



/***********************************************************************************************

>	SGDisplayFrame::SGDisplayFrame(Layer *LayerToDisplay)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com> (Based on Jason's Layer gallery code)
	Created:	16/4/97 (Based on Jason's Layer gallery code)

	Inputs:		LayerToDisplay - The Layer this item will display

	Purpose:	SGDisplayFrame constructor

***********************************************************************************************/

SGDisplayFrame::SGDisplayFrame(Layer *LayerToDisplay) : SGDisplayLayer(LayerToDisplay) 
{
	// Let the baseclass handle it
}



/***********************************************************************************************

>	SGDisplayFrame::~SGDisplayFrame()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com> (Based on Jason's Layer gallery code)
	Created:	16/4/97 (Based on Jason's Layer gallery code)

	Purpose:	SGDisplayFrame destructor

	Notes:		Ensures the LastClickedLayer memory is not left pointing at this layer

***********************************************************************************************/

SGDisplayFrame::~SGDisplayFrame()
{
	// Make sure LastClickedLayer pointer is not left in an invalid state
	if (this == LastClickedLayer)
		LastClickedLayer = NULL;
}



/***********************************************************************************************

>	virtual void SGDisplayFrame::HandleRedraw(SGRedrawInfo *RedrawInfo,
												SGFormatInfo *FormatInfo)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com> (Based on Jason's Layer gallery code)
	Created:	16/4/97 (Based on Jason's Layer gallery code)

	Inputs:		RedrawInfo	- The information on the kernel-rendered redraw area
				FormatInfo	- The formatting information structure

				member variable FormatRect should be set up (before calling this method)
				to be the rectangle in which to draw this item

	Purpose:	SGDisplayFrame item redraw method - removed from the main HandleEvent
				method merely to make the code tidier.

	Scope:		private

***********************************************************************************************/

void SGDisplayFrame::HandleRedraw(SGRedrawInfo *RedrawInfo, SGMiscInfo *MiscInfo)
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
//	BOOL Visible 		= GetDisplayedLayer()->IsVisible();
//	BOOL Locked  		= GetDisplayedLayer()->IsLocked();
	BOOL Guide			= GetDisplayedLayer()->IsGuide();
	BOOL Solid			= GetDisplayedLayer()->IsSolid();
	BOOL Overlay  		= GetDisplayedLayer()->IsOverlay();
	BOOL Edited 		= GetDisplayedLayer()->IsEdited();
	Document* pDoc = (Document*)GetDisplayedLayer()->FindOwnerDoc();

	ERROR3IF(pDoc == NULL,"Displayed layer doesn't have an owner doc");
	if (pDoc == NULL) return;

	// Draw 'Solid' button (The glyph is "on" when the layer is solid)
	if (Solid)
	{
		//if (!Guide && (ActiveAndVisEd || pDoc->IsAllVisible()))
		//	DrawBitmap(Renderer, &VisibleBtn, _R(IDB_LGAL_TICKFORCED));
		//else
			DrawBitmap(Renderer, &VisibleBtn, _R(IDB_LGAL_TICKON));
	}
	else
		DrawBitmap(Renderer, &VisibleBtn, _R(IDB_LGAL_TICKOFF));

	// Draw 'Overlay' button (Glyph is on if the layer is overlayed)
	if (Overlay)
	{
		// If Solid is on then this is superfluous so render the greyed form
		if (Solid)
			DrawBitmap(Renderer, &LockedBtn, _R(IDB_LGAL_TICKFORCED));
		else
			DrawBitmap(Renderer, &LockedBtn, _R(IDB_LGAL_TICKON));
	}
	else
	{
		// If Solid is on then this is superfluous so render the greyed form
		if (Solid)
			DrawBitmap(Renderer, &LockedBtn, _R(IDB_LGAL_TICKOFFFORCED));
		else
			DrawBitmap(Renderer, &LockedBtn, _R(IDB_LGAL_TICKOFF));
	}

	Layer* pLayer = GetDisplayedLayer();
	ERROR3IF(pLayer == NULL, "I'm displaying a NULL layer? Serious screwup has occurred!");
	if (pLayer == NULL)
		return;

	DocColour Foreground = RedrawInfo->Foreground;
	DocColour Background = RedrawInfo->Background;
	DocColour SelForeground = RedrawInfo->SelForeground;
	DocColour SelBackground = RedrawInfo->SelBackground;

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

	// Find the delay value and add it to the end of the layer/frame name
	BOOL IsHidden = pLayer->IsHiddenFrame();
	if (!IsHidden)
	{
		UINT32 FrameDelay = pLayer->GetFrameDelay();
		String_256 Delay;
		Delay.MakeMsg(_R(IDS_FRAMEDELAYFORMAT), FrameDelay);
		MyText += Delay;
	}
	else
	{
		String_256 HiddenText(_R(IDS_SHOWFRAMEISHIDDEN)); // (Hidden)
		MyText += HiddenText;
	}

#ifdef _DEBUG
	// If the layer is edited then add a star onto it
	if (pLayer->IsEdited())
		MyText += " *";		// DEBUG only so ok
#endif

	Renderer->DrawFixedSystemText(&MyText, MyRect);				

	StopRendering(RedrawInfo, MiscInfo);
}



/***********************************************************************************************

>	void SGDisplayFrame::ChangeRangeState(SGDisplayFrame *Start, SGDisplayFrame *End,
											BOOL Range, INT32 WhichButton, SGMiscInfo *MiscInfo)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com> (Based on Jason's Layer gallery code)
	Created:	16/4/97 (Based on Jason's Layer gallery code)

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

void SGDisplayFrame::ChangeRangeState(SGDisplayFrame *Start, SGDisplayFrame *End,
										BOOL Range, INT32 WhichButton, SGMiscInfo *MiscInfo)
{
	ERROR3IF(Start == NULL || MiscInfo == NULL, "Illegal NULL params");

	SuperGallery *ParentGallery = GetParentGallery();
	ERROR3IF(ParentGallery == NULL, "No parent gallery?! Weird!");

	// Determine the new state based on inverting this item's state
	BOOL NewState = TRUE;
	switch (WhichButton)
	{
		case 0:		// Solid state
			NewState = !(GetDisplayedLayer()->IsSolid());
			break;
		
		case 1:		// Overlayed sate
			NewState = !(GetDisplayedLayer()->IsOverlay());
			break;

		default:
			ERROR3("Illegal WhichButton parameter to SGDisplayFrame::ChangeRangeState (must be 0..2)");
			break;
	}

	// Scan the sibling list to find the start and end nodes, and determine if end comes
	// before start (in which case we'll need to swap them over before continuing)
	SGDisplayFrame *Ptr = NULL;

	if (!Range || End == NULL)
		End = Start;

	BOOL FoundStart	= FALSE;
	BOOL FoundEnd	= FALSE;
	BOOL MustSwap	= FALSE;

	Ptr = (SGDisplayFrame *) (Start->GetParent()->GetChild());
	while (Ptr != NULL)
	{
		if (Ptr == Start)
			FoundStart = TRUE;

		if (Ptr == End)
		{
			FoundEnd = TRUE;
			MustSwap = !FoundStart;		// If not found Start first, then the range is backwards
		}

		Ptr = (SGDisplayFrame *) Ptr->GetNext();
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
				// Notify the gif animation property tabs dlg about this state change.
				// This allows us to sit on the ACTIVE_LAYER_CHANGED message.
				GIFAnimationPropertyTabs::SetChangeLayerState(TRUE);

				// Ask the gallery to change the state of the flag
				Ptr->DoChangeState(LAYER_SOLID, NewState);
				// Do a redraw so we notice the change
				ParentGallery->ForceRedrawOfArea(&VisibleBtn);

				// Set this flag back to false.
				GIFAnimationPropertyTabs::SetChangeLayerState(FALSE);
				break;

			case 1:		// Locked state
				// Notify the gif animation property tabs dlg about this state change.
				// This allows us to sit on the ACTIVE_LAYER_CHANGED message.
				GIFAnimationPropertyTabs::SetChangeLayerState(TRUE);

				// Ask the gallery to change the state of the flag
				Ptr->DoChangeState(LAYER_OVERLAY, NewState);
				// Do a redraw so we notice the change
				ParentGallery->ForceRedrawOfArea(&LockedBtn);

				// Set this flag back to false.
				GIFAnimationPropertyTabs::SetChangeLayerState(FALSE);
				break;
		}

		if (Ptr == End)					// Slight strangeness to handle (Start==End) case
			Ptr = NULL;									// Have hit and processed End, so stop
		else
			Ptr = (SGDisplayFrame *) Ptr->GetNext();	// Have not, so go on to next layer
	}
}



/***********************************************************************************************

>	virtual BOOL SGDisplayFrame::HandleEvent(SGEventType EventType, void *EventInfo,
											 SGMiscInfo *MiscInfo)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com> (Based on Jason's Layer gallery code)
	Created:	16/4/97 (Based on Jason's Layer gallery code)

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

BOOL SGDisplayFrame::HandleEvent(SGEventType EventType, void *EventInfo,
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
					//BOOL Visible 		= pLayer->GetVisibleFlagState();
					//BOOL Locked  		= pLayer->GetLockedFlagState();
					BOOL Guide			= pLayer->IsGuide();
					BOOL Solid			= pLayer->IsSolid();
					//BOOL Guide			= pLayer->IsOverlay();

					CalculateButtonRects(MiscInfo, &MyRect, &VisibleBtn, &LockedBtn, &SnapBtn);

					// Ignore menu clicks on the Visible button
					if (!Mouse->MenuClick && VisibleBtn.ContainsCoord(Mouse->Position))
					{
						// At present always change the actual setting of the Solid flag
					 	if (TRUE) //Guide || (!pDoc->IsAllVisible() && !ActiveAndVisEd))
						{
							ChangeRangeState(this, LastClickedLayer, Mouse->Extend, 0, MiscInfo);
							// Remember this as the "anchor" layer for extend-click setting of ranges
							LastClickedLayer = this;
							
							// We need to force a redraw of the overlay button as it is dependent
							// on the locked state
							ParentGallery->ForceRedrawOfArea(&LockedBtn);
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
						// and it's not the active layer with the 'active layer is visible and edtable' pref on
						if (!Solid) //!pDoc->IsMultilayer() && !ActiveAndVisEd)
						{
							ChangeRangeState(this, LastClickedLayer, Mouse->Extend, 1, MiscInfo);
							// Remember this as the "anchor" layer for extend-click setting of ranges
							LastClickedLayer = this;
						}
						else
						{
							UINT32 IDS = _R(IDS_LAYER_NOCHANGE_OVERLAY);

							String_256 msg(IDS);
							GetApplication()->UpdateStatusBarText(&msg);
							Beep();
						}

						break;
					}

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
							FrameSGallery::MakeActiveLayer(pLayer);

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
							DragManagerOp::StartDrag(pDragInfo, GetListWindow());
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

>	void SGDisplayFrame::DoChangeState(OpLayerGalReason Reason,BOOL NewState)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com> (Based on Mark N's function in the Layer gallery code)
	Created:	16/4/97 (Based on Jason's Layer gallery code)
	Inputs:		Reason 	= reason code definiting what 'NewState' represents
				NewState= boolean that determines the new state of the flag defined by 'Reason'

	Returns:	-
	Purpose:	Called when a flag changes for a layer gallery item
	
	Notes:		Does not redraw the layer in the display list

***********************************************************************************************/

void SGDisplayFrame::DoChangeState(OpLayerGalReason Reason,BOOL NewState)
{
	FrameSGallery* pGal = (FrameSGallery*)GetParentGallery();
	ERROR3IF(!IS_A(pGal,FrameSGallery),"Parent not a frame gallery");
	if (!IS_A(pGal,FrameSGallery)) return;

	Layer* pLayer = GetDisplayedLayer();
	pGal->DoChangeLayerState(pLayer,Reason,NewState);
}

/***********************************************************************************************

>	virtual void SGDisplayFrame::MoveLayer(SGDisplayNode *NodeToMove,BOOL Before,BOOL ToggleBackground)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com> (Based on Mark N's function in the Layer gallery code)
	Created:	16/4/97 (Based on Jason's Layer gallery code)

	Inputs:		NodeToMove 	= the node to move (which specifies the layer to move)
				Before		= if TRUE the layer is moved previous (before) this layer, otherwise
								the layer is moved next (after) this layer
				ToggleBacground = TRUE if the background flag should be toggled when moved

	Purpose:	This moves the given layer either before or after this layer, in an undoable way

	Errors:		ERROR3 and quiet exit if NodeToMove == NULL

	SeeAlso:	SuperGallery; SGDisplayNode::MoveAfter; SGDisplayNode::InsertAfter; SGDisplayNode::AddItem

	Note:		Same as LayerSGallery version but uses FRAME_MOVE

***********************************************************************************************/

void SGDisplayFrame::MoveLayer(SGDisplayNode *NodeToMove,BOOL Before,BOOL ToggleBackground)
{
	ERROR3IF(!IS_A(NodeToMove,SGDisplayFrame),"The node to move is not a SGDisplayFrame");
	if(!IS_A(NodeToMove,SGDisplayFrame))
		return;

	Layer* pLayerToMove = ((SGDisplayFrame*)NodeToMove)->GetDisplayedLayer();
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
		Reason = FRAME_MOVE;
	

	OpLayerGalParam Param(Reason,pSpread);

	Param.pLayer 			= pLayerToMove;
	Param.pContextNode		= pThisLayer;
	Param.MoveRedraw 		= TRUE;
	Param.ToggleBackground	= ToggleBackground;
	Param.pLayerSGal		= (FrameSGallery*)GetParentGallery();

	if (Before)
		Param.AttDir = NEXT;	// next means insert-before-this
	else
		Param.AttDir = PREV;	// prev means insert-after-this

	OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_LAYERGALCHANGE); 
	if (pOpDesc != NULL)
		pOpDesc->Invoke((OpParam*)&Param);
	else
	{
		ERROR3("Couldn't find OPTOKEN_LAYERGALCHANGE op descriptor");
	}
}

#endif // EXCLUDE_GALS



/********************************************************************************************
/// FrameSGallery
********************************************************************************************/




/********************************************************************************************

>	FrameSGallery::FrameSGallery()
												 
	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com> (Based on Jason's Layer gallery code)
	Created:	16/4/97 (Based on Jason's Layer gallery code)
	Purpose:	FrameSGallery default constructor

********************************************************************************************/

FrameSGallery::FrameSGallery()
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

>	FrameSGallery::~FrameSGallery()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com> (Based on Jason's Layer gallery code)
	Created:	16/4/97 (Based on Jason's Layer gallery code)
	Purpose:	FrameSGallery destructor.

********************************************************************************************/

FrameSGallery::~FrameSGallery()
{
}

#ifndef EXCLUDE_GALS

/********************************************************************************************

>	void FrameSGallery::CreateNewSubtree(Document *ParentDoc, SGDisplayGroup *ExistingGroup)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com> (Based on Jason's Layer gallery code)
	Created:	16/4/97 (Based on Jason's Layer gallery code)

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

void FrameSGallery::CreateNewSubtree(Document *ParentDoc, SGDisplayGroup *ExistingGroup)
{
	if (DisplayTree == NULL)
		return;

TRACEUSER( "Neville", _T("FrameSGallery::CreateNewSubtree Just about to DestroySubtree\n"));
	// The layer gallery only ever shows one document, so destroy the entire tree
	// and start again from scratch (note that this doesn't destroy the root node)
	DisplayTree->DestroySubtree();

	// All groups will be destroyed by DestroySubtree(), so we'd better forget them
	DisplayForeground = NULL;
	
TRACEUSER( "Neville", _T("FrameSGallery::CreateNewSubtree completed DestroySubtree\n"));

 	if (ParentDoc == NULL)
		return;

	SGDisplayFrame  *DisplayLayer = NULL;

	EnsureActiveLayerIntegrity();

	// Create the foreground layer group
	DisplayForeground = new SGFrameGroup(this, ParentDoc,NULL,TRUE);	// Create new Group
	if (DisplayForeground == NULL)										// Total failure - abort
		return;
	DisplayTree->AddItem(DisplayForeground);							// Add new group to tree

	Spread* pSelSpread = GetSelectedSpread();
	ERROR3IF(pSelSpread == NULL,"No selected spread in FrameSGallery");
	if (pSelSpread == NULL) return;

	Layer* pLayer					= pSelSpread->FindLastLayer();
	Layer* pActiveLayer				= NULL;
	Layer* pGuideLayer				= NULL;
	BOOL SetActiveLayer				= FALSE;
// Only Webster needs to show a dummy frame layer 
#ifdef WEBSTER
	Layer* pSingleNonFrameLayer		= NULL;
	BOOL ShownSingleNonFrameLayer	= FALSE;
#endif // WEBSTER

	while (pLayer != NULL)
	{
		// If we find a guide layer then note that for future use
		if (pLayer->IsGuide())
		{
			pGuideLayer = pLayer;
		}
		else if (
				 (pLayer->IsPseudoFrame() && pLayer->IsFrame())				// its a proper frame layer
#ifdef WEBSTER
				 || (pLayer->IsPseudoFrame() && !ShownSingleNonFrameLayer)	// its a potential frame layer
#endif // WEBSTER
				)
		{
#ifdef WEBSTER
			// If its not a frame layer then note that we have added our single
			// non-frame layer in.
			if (!pLayer->IsFrame())
			{
				ShownSingleNonFrameLayer = TRUE;
				pSingleNonFrameLayer = pLayer;
			}
#endif // WEBSTER

			// Only add foreground layers and non-guide layers
			DisplayLayer = new SGDisplayFrame(pLayer);

			if (DisplayLayer != NULL)
			{
				// Always add to the foreground layers
				DisplayForeground->AddItem(DisplayLayer);

				if (pLayer->IsActive())
				{
					ERROR3IF(pActiveLayer != NULL,"Found more than one active layer in the selected spread");
					pActiveLayer = pLayer;

					DisplayLayer->SetSelected(TRUE);	// Make the active layer selected
					SetActiveLayer = TRUE;				// Note this fact
				}
			}
		}

		pLayer = pLayer->FindPrevLayer();
	}

	// If we have not set a selected layer then this is very bad
	if (!SetActiveLayer)
	{
		TRACEUSER( "Neville", _T("Active layer is not a frame layer so force it!\n"));
		Layer * pNewActiveLayer = pSpread->FindLastFrameLayer();
#ifdef WEBSTER
		if (pNewActiveLayer == NULL && pSingleNonFrameLayer != NULL)
			pNewActiveLayer = pSingleNonFrameLayer;
#else
//		ERROR3IF(pNewActiveLayer == NULL,"No current active frame layer");
#endif // WEBSTER

		// If we have an active layer, ensure that it really is the one and only active layer
		if (pNewActiveLayer != NULL)
		{
			// Search for the new display item to match this new active layer 
			SGDisplayLayer *pWantedDispLayer = NULL;
			SGDisplayLayer *pDispLayer = ((SGDisplayLayer *) DisplayForeground->GetChild());
			while (pDispLayer != NULL)						
			{
				if (pDispLayer->GetDisplayedLayer() == pNewActiveLayer)
					pWantedDispLayer = pDispLayer;
				pDispLayer = ((SGDisplayLayer *)pDispLayer->GetNext());
			}
			
			// If we found the required display item then go and select it
			if (pWantedDispLayer)
			{
				// Now update the selection states...
				SelectItems(FALSE);	// Deselect everything else
				// Something has changed so we must go and select this new item
				pWantedDispLayer->SetSelected(TRUE);
			}

			// As this can only happen when we have a frame layer which is not active
			// e.g. import a non-frame xar/web document into a non-frame document
			// we must use the layer form of MakeActiveLayer so that the visibility of
			// layers is not affected. 
			LayerSGallery::MakeActiveLayer(pNewActiveLayer);
			//FrameSGallery::MakeActiveLayer(pNewActiveLayer);
		}
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

	TRACEUSER( "Neville", _T("FrameSGallery::CreateNewSubtree exit\n"));
}


/********************************************************************************************

>	BOOL FrameSGallery::PostCreate(void)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com> (Based on Jason's Layer gallery code)
	Created:	16/4/97 (Based on Jason's Layer gallery code)

	Returns:	TRUE if the Gallery initialised successfully
				FALSE if it should not be opened due to a failure to initialise

	Purpose:	The FrameSGallery PostCreate handler. This overrides the base class
				PostCreate function. It is called at the very end of the
				SuperGallery::Create method, after the window has been created and opened.

********************************************************************************************/

BOOL FrameSGallery::PostCreate(void)
{
	Document *ParentDoc = Document::GetSelected();
	if (ParentDoc != NULL)
	{
		SetBoolGadgetSelected(_R(IDC_BTN_MULTILAYER),!(ParentDoc->IsMultilayer() == 0));
		SetBoolGadgetSelected(_R(IDC_BTN_ALLVISIBLE),!(ParentDoc->IsAllVisible() == 0));
	}

	EnableGadget(_R(IDC_GALLERY_COPY),ParentDoc != NULL && !IsSelectedItemGuideLayer());

	UpdateFrameRelatedButtons();

	return(TRUE);
}



/********************************************************************************************

>	virtual BOOL FrameSGallery::ApplyAction(SGActionType Action)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com> (Based on Jason's Layer gallery code)
	Created:	16/4/97 (Based on Jason's Layer gallery code)

	Inputs:		Action - Indicates what action to apply

	Returns:	TRUE to indicate successful handling of the action, or
				FALSE to indicate failure

	Purpose:	Applies certain conventional gallery actions (usually associated with
				gallery buttons, for new, edit, delete, etc)

	SeeAlso:	SGActionType

********************************************************************************************/

BOOL FrameSGallery::ApplyAction(SGActionType Action)
{
	// No display tree? Better forget about it then!
	if (DisplayTree == NULL)
		return(FALSE);

	// Determine useful info - this is usually needed for most actions, so always get it
	Document		*SelectedDoc = Document::GetSelected();
	SGDisplayGroup	*DocumentGroup = DisplayTree->FindSubtree(this, SelectedDoc, NULL);
	SGDisplayFrame	*FirstSelected = NULL;
	
	if (DocumentGroup != NULL)
		FirstSelected = (SGDisplayFrame *) DocumentGroup->FindNextSelectedItem(NULL);

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
			//if (PrepareToDelete()) // No warning on Frame gallery
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

>	virtual MsgResult FrameSGallery::Message(Msg* Message)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com> (Based on Jason's Layer gallery code)
	Created:	16/4/97 (Based on Jason's Layer gallery code)
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

MsgResult FrameSGallery::Message(Msg* Message)
{
	if (IS_OUR_DIALOG_MSG(Message))
	{
		DialogMsg* Msg = (DialogMsg*)Message;

		switch (Msg->DlgMsg)
		{
			case DIM_CREATE:
				SGInit::UpdateGalleryButton(OPTOKEN_DISPLAYFRAMEGALLERY, TRUE);
				break;

				case DIM_CANCEL:
				SGInit::UpdateGalleryButton(OPTOKEN_DISPLAYFRAMEGALLERY, FALSE);
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
						DoChangeLayerState(NULL,FRAME_MULTILAYER,state);
						break;

					case _R(IDC_BTN_ALLVISIBLE):
						state = GetLongGadgetValue(_R(IDC_BTN_ALLVISIBLE),0,1);
						DoChangeLayerState(NULL,FRAME_ALLVISIBLE,state);
						break;

					case _R(IDC_GALLERY_UPONE):
						DoMoveLayer(MOVELAYER_UPONE);
						break;

					case _R(IDC_GALLERY_DOWNONE):
						DoMoveLayer(MOVELAYER_DOWNONE);
						break;

					case _R(IDC_GALLERY_NEW_NODLG):
						DoCreateNewItem();
						break;

					case _R(IDC_GALLERY_COPY_NODLG):
						DoCopyLayer();
						break;

					case _R(IDC_GALLERY_HELP):		// Show help page
						HelpUserTopic(_R(IDS_HELPPATH_Gallery_Frame));
						break;


/*					case _R(IDC_GALLERY_PROPERTIES):
						// Handled by optoken
						DoFrameProperties();
						break; */

/*					case _R(IDC_BMPGAL_SAVE):
						// Handled by optoken
						DoSaveAnimation();
						break; */

/*					case _R(IDC_BMPGAL_PREVIEW):
						// Handled by optoken
						DoPreviewAnimation();
						break; */

/*					case _R(IDC_FRAME_ANIMATION):
						// Handled by optoken
						// Implement ****
						break; */
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
					TRACEUSER( "Neville", _T("frame SpreadMsg::SELCHANGED CreateNewSubtree\n"));
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
								TRACEUSER( "Neville", _T("frame SpreadMsg::LAYERCHANGES CreateNewSubtree\n"));
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
			{
			// Showstopper removed, Jason, 28/3/96
			// When the doc title changes, we simply need to redraw to update the group titles.
			// Calling CreateNewSubtree will destroy the existing tree - this is really really bad
			// when the document title changed as a result of us (e.g.) locking a layer, because the
			// message broadcats hits us while we're still in the middle of traversing the tree,
			// we then delete the tree out from under ourselves, and are then in a highly dodgy state!
			// CreateNewSubtree(Msg->pChangingDoc);
				ForceRedrawOfList();
				break;
			}

			// This message is sent when a new or just opened document is stable.
			case DocChangingMsg::DocState::SELCHANGED:
			{
				if (Msg->pNewDoc == NULL)
				{
					// No documents around any more, so we must wipe the display tree
					// The base class will shade the gallery for us.
					TRACEUSER( "Neville", _T("frame DocChangingMsg CreateNewSubtree\n"));
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
					if (!GetNewDocBorn())
					{
						// The document has changed, do we detect any non-frame layers? 
						BOOL FrameMode = FALSE;	
						
						// Get a ptr to the document.
						Document* pDoc = Msg->pNewDoc;
								
						// Ensure a valid ptr.
						if(pDoc)
						{
							// Determine the document mode.
							FrameMode = IsFrameMode(pDoc);

							// If we are no longer in Frame mode and the frame gallery is open,
							// then close the frame gallery and open the layer gallery.
							if( !FrameMode && IsVisible() )
							{
								// Close the frame gallery.
								if(CloseFrameGallery())
								{
									// Open the the layer gallery.
									OpenLayerGallery();
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

				// Ensure the frame gallery is open.
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
				// The document has changed, do we detect any non-frame layers? 
				BOOL FrameMode = FALSE;	
				BOOL GalleryClosed = FALSE;

				// Get a ptr to the document.
				Document* pChangingDoc = Msg->pChangingDoc;
							
				// Ensure a valid ptr.
				if(pChangingDoc)
				{
					// Determine the document mode.
					FrameMode = IsFrameMode(pChangingDoc);

					// If we are no longer in Frame mode then
					// close the frame gallery and open the layer gallery.
					if( !FrameMode && IsVisible() )
					{
						// Close the frame gallery.
						if(CloseFrameGallery())
						{
							// Open the Layer gallery.
							OpenLayerGallery();

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
						TRACEUSER( "Neville", _T("frame DocViewMsg::DocViewState::SELCHANGED CreateNewSubtree\n"));
						CreateNewSubtree(Msg->pNewDocView->GetDoc());
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
			
				UpdateFrameRelatedButtons();
			}
			break;
			
			// The active layer has been changed externally to the gallery.
			case LayerMsg::LayerReason::UPDATE_ACTIVE_LAYER:
			{
				// We must now check whether the selected layer has been changed externally
				// to us. May happen if an animated GIF is being loaded onto layers
				SGDisplayLayer *pDisplayItem = NULL;
				if (DisplayForeground != NULL && IsVisible())
					pDisplayItem = GetSelectedLayerGalItem();
				if (pDisplayItem != NULL)
				{
					if (pMsg->pNewLayer != NULL && pDisplayItem->GetDisplayedLayer() != pMsg->pNewLayer)
					{
						Layer* pNewActiveLayer = pMsg->pNewLayer;
						// Search for the new display item to match this new active layer 
						SGDisplayLayer *pWantedDispLayer = NULL;
						SGDisplayLayer *pDispLayer = ((SGDisplayLayer *) DisplayForeground->GetChild());
						while (pDispLayer != NULL)						
						{
							if (pDispLayer->GetDisplayedLayer() == pNewActiveLayer)
								pWantedDispLayer = pDispLayer;
							pDispLayer = ((SGDisplayLayer *)pDispLayer->GetNext());
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
			
			// The specified layer has been changed externally to the gallery.
			// So redraw the name.
			case LayerMsg::LayerReason::REDRAW_LAYER:
			{
				TRACEUSER( "Neville", _T("frame LayerMsg::LayerReason::REDRAW_LAYER\n"));
				Layer * pWantedLayer = pMsg->pNewLayer;
				if (pWantedLayer != NULL && DisplayForeground != NULL && IsVisible())
				{
					// Search for the new display item to match this new active layer 
					SGDisplayLayer *pWantedDispLayer = NULL;
					SGDisplayLayer *pDispLayer = ((SGDisplayLayer *) DisplayForeground->GetChild());
					while (pDispLayer != NULL)						
					{
						if (pDispLayer->GetDisplayedLayer() == pWantedLayer)
							pWantedDispLayer = pDispLayer;
						pDispLayer = ((SGDisplayLayer *)pDispLayer->GetNext());
					}

					// If we found it then force a redraw
					if (pWantedDispLayer)
						pWantedDispLayer->ForceRedrawOfMyself();
				}
			}
			break;
		}
	}

	return(SuperGallery::Message(Message));
}    

/***********************************************************************************************

>	void FrameSGallery::EnsureSelSpreadSelectionIntegrity()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com> (Based on Mark N's function in the Layer gallery code)
	Created:	16/4/97 (Based on Jason's Layer gallery code)
	Purpose:	This makes sure that all the invisible and/or locked layers in the spread
				have no selection. It calls NodeRenderableInk::DeselectAllOnLayer() on each 
				layer that should NOT have a selection, just in case.
				Currently only called after an undo or redo op, because it could be quite slow

***********************************************************************************************/

void FrameSGallery::EnsureSelSpreadSelectionIntegrity()
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

>	static void FrameSGallery::SetActive(Document* pDoc,Layer* pLayer,BOOL state)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com> (Based on Mark N's function in the Layer gallery code)
	Created:	16/4/97 (Based on Jason's Layer gallery code)
	Inputs:		pDoc   = ptr to doc containing layer
				pLayer = ptr that contains this layer
				state  = TRUE means make active, FALSE means make inactive
	Outputs:	-
	Returns:	-
	Purpose:	Calls pLayer->SetActive(state), redrawing and changing the layer's selection
				if necessary.
				DON'T CALL THIS FUNC!! USE MakeActiveLayer() INSTEAD!!!!

***********************************************************************************************/

void FrameSGallery::SetActive(Document* pDoc,Layer* pLayer,BOOL state)
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
		FrameSGallery::ForceRedrawLayer(pDoc,pLayer);

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

>	static void FrameSGallery::MakeActiveLayer(Layer* pNewActiveLayer, UndoableOperation * pUndoOp = NULL)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com> (Based on Mark N's function in the Layer gallery code)
	Created:	16/4/97 (Based on Jason's Layer gallery code)
	Inputs:		pNewActiveLayer = ptr to the layer you want to be active
				pUndoOp = if non null, the default, will do the change undoably
	Outputs:	-
	Returns:	-
	Purpose:	Makes the layer active.
				Firstly it will inactivate any other active layers, forcing it to redraw if necessary
				Then it will set this layer to be active, again forcing a redraw if necessary

				If the given new layer is already active, nothing happens

				Note: pNewActiveLayer must be in the currect doc

***********************************************************************************************/

void FrameSGallery::MakeActiveLayer(Layer* pNewActiveLayer,	UndoableOperation * pUndoOp)
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
			{
				if (pUndoOp == NULL)
					FrameSGallery::SetActive(pDoc,pLayer,FALSE);
				else
				{
					// Make layer active undoably
					OpLayerGalParam Param(LAYER_ACTIVE, pSpread);
					Param.NewState = FALSE;
					Param.pLayer = pLayer;
					LayerStateAction::Init(pUndoOp, pUndoOp->GetUndoActions(), Param);
				}
			}

			pLayer = pLayer->FindNextLayer();
		}
	}

	// Go though the layers from the new active layer downwards and set
	// all previous layers non-visible and non-selected
	FixOtherLayersFromActive(pNewActiveLayer, pUndoOp);

	// Make sure the new layer is active, redrawing if necessary
/*	if (!pNewActiveLayer->IsActive())
	{
		if (pUndoOp == NULL)
			FrameSGallery::SetActive(pDoc,pNewActiveLayer,TRUE);
		else
		{
			// Make layer active undoably
			OpLayerGalParam Param(LAYER_ACTIVE, pSpread);
			Param.NewState = TRUE;
			Param.pLayer = pNewActiveLayer;
			LayerStateAction::Init(pUndoOp, pUndoOp->GetUndoActions(), Param);
		}
	} */
	if (pUndoOp == NULL)
	{
		if (!pNewActiveLayer->IsActive())
			FrameSGallery::SetActive(pDoc,pNewActiveLayer,TRUE);
	}
	else
	{
		// Make layer active undoably
		OpLayerGalParam Param(LAYER_ACTIVE, pSpread);
		Param.NewState = TRUE;
		Param.pLayer = pNewActiveLayer;
		LayerStateAction::Init(pUndoOp, pUndoOp->GetUndoActions(), Param);
	}

#if defined(EXCLUDE_FROM_RALPH)
	//ERROR3("FrameSGallery::MakeActiveLayer supressed broadcast - bad !");
#else
	BROADCAST_TO_ALL(LayerMsg(pNewActiveLayer,LayerMsg::LayerReason::ACTIVE_LAYER_CHANGED));
#endif
}


/***********************************************************************************************

>	static void FrameSGallery::FixOtherLayers()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/4/97
	Inputs:		pSpread = ptr to spread
	Outputs:	-
	Returns:	-
	Purpose:	A layer flag such as overlay or solid has been changed and so we need to go
				and find the active layer and ensure that the other layers above and below this
				have the correct visibility and locked settings.

***********************************************************************************************/

void FrameSGallery::FixOtherLayers(Spread* pSpread)
{
	// Check that the class variable storing the current/selected spread is ok
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

	if (pActiveLayer)
		FixOtherLayersFromActive(pActiveLayer);
}

/***********************************************************************************************

>	static void FrameSGallery::FixOtherLayersFromActive(Layer * pActiveLayer,
														UndoableOperation * pUndoOp = NULL)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/4/97
	Inputs:		pActiveLayer = the current active layer
				pUndoOp = if non null, the default, will do the change undoably
	Purpose:	A layer flag such as overlay or solid has been changed and so we need take the 
				passed in current active layer and then ensure that the other layers above and
				below this have the correct visibility and locked settings.

***********************************************************************************************/

void FrameSGallery::FixOtherLayersFromActive(Layer * pActiveLayer,
											 UndoableOperation * pUndoOp)
{
	// This might be the new active layer that we are setting and so
	// it might not have been made active yet!
	if (pActiveLayer == NULL) // || !pActiveLayer->IsActive())
	{
		ERROR3("FixOtherLayersFromActive has been given a bad active layer!");
		return;
	}

	// Only need a spread pointer in the undoable case
	Spread * pSpread = NULL;
	if (pUndoOp != NULL)
		pSpread = pActiveLayer->FindParentSpread();

	// Ensure that the new active layer is going to be visible and unlocked
	// Assume code is called from make active code which will force the redraw
	// and selection changes as required. So do it the simple way
	if (pUndoOp == NULL || pSpread == NULL)
	{
		LayerSGallery::DoChangeVisible(pActiveLayer,TRUE);
		LayerSGallery::DoChangeLocked(pActiveLayer,FALSE);
	}
	else
	{
		// Make visible and locked
		// Only change if required as otherwise we will get an unwanted undo step
//		if (!pActiveLayer->IsVisible())
		{
			OpLayerGalParam Param(LAYER_VISIBLE, pSpread);
			Param.NewState = TRUE;
			Param.pLayer = pActiveLayer;
			LayerStateAction::Init(pUndoOp, pUndoOp->GetUndoActions(), Param);
		}

		// Only change if required as otherwise we will get an unwanted undo step
//		if (pActiveLayer->IsLocked())
		{
			OpLayerGalParam LockedParam(LAYER_LOCKED, pSpread);
			LockedParam.NewState = FALSE;
			LockedParam.pLayer = pActiveLayer;
			LayerStateAction::Init(pUndoOp, pUndoOp->GetUndoActions(), LockedParam);
		}
	}

	// Go though the layers from the new active layer downwards and set
	// all previous layers non-visible and non-selected
	Layer* pLayer = pActiveLayer->FindPrevLayer();
	BOOL FoundSolid = pActiveLayer->IsSolid();
	BOOL OverlayPrevious = FALSE;
	// Solid flag overrides the overlay flag
	if (!FoundSolid)
		OverlayPrevious = pActiveLayer->IsOverlay();
	while (pLayer != NULL)
	{
		if (pLayer->IsPseudoFrame())
		{
			if (pLayer->IsSolid() && !FoundSolid)
			{
				// Stop any later solid frames having any effect
				FoundSolid = TRUE;
				// Stop any overlay having an effect
				OverlayPrevious = FALSE;
				if (pUndoOp == NULL || pSpread == NULL)
				{
					LayerSGallery::DoChangeVisible(pLayer,TRUE);
					LayerSGallery::DoChangeLocked(pLayer,TRUE);
				}
				else
				{
					// Only change if required as otherwise we will get an unwanted undo step
//					if (!pLayer->IsVisible())
					{
						OpLayerGalParam Param(LAYER_VISIBLE, pSpread);
						Param.NewState = TRUE;
						Param.pLayer = pLayer;
						LayerStateAction::Init(pUndoOp, pUndoOp->GetUndoActions(), Param);
					}
					// Only change if required as otherwise we will get an unwanted undo step
//					if (!pLayer->IsLocked())
					{
						OpLayerGalParam LockedParam(LAYER_LOCKED, pSpread);
						LockedParam.NewState = TRUE;
						LockedParam.pLayer = pLayer;
						LayerStateAction::Init(pUndoOp, pUndoOp->GetUndoActions(), LockedParam);
					}
				}
			}
			else
			{
				BOOL NewVisibleState = TRUE;
				if (OverlayPrevious)
					NewVisibleState = TRUE;
				else
					NewVisibleState = FALSE;

				if (pUndoOp == NULL || pSpread == NULL)
				{
					LayerSGallery::DoChangeVisible(pLayer,NewVisibleState);
					LayerSGallery::DoChangeLocked(pLayer,TRUE);
				}
				else
				{
					// Only change if required as otherwise we will get an unwanted undo step
//					if (pLayer->IsVisible() != NewVisibleState)
					{
						OpLayerGalParam Param(LAYER_VISIBLE, pSpread);
						Param.NewState = NewVisibleState;
						Param.pLayer = pLayer;
						LayerStateAction::Init(pUndoOp, pUndoOp->GetUndoActions(), Param);
					}
					// Only change if required as otherwise we will get an unwanted undo step
//					if (!pLayer->IsLocked())
					{
						OpLayerGalParam LockedParam(LAYER_LOCKED, pSpread);
						LockedParam.NewState = TRUE;
						LockedParam.pLayer = pLayer;
						LayerStateAction::Init(pUndoOp, pUndoOp->GetUndoActions(), LockedParam);
					}
				}

				// If we are currently overlaying and this frame
				// is overlay as well then keep overlay on, otherwise
				// turn it off.
				if (OverlayPrevious && pLayer->IsOverlay())
					OverlayPrevious = TRUE;
				else 
					OverlayPrevious = FALSE;
			}

			// Overlay should only affect the previous frame
			//if (!pLayer->IsSolid() && pLayer->IsOverlay())
			//	OverlayPrevious = TRUE;
			//else 
			//	OverlayPrevious = FALSE;
		}

		pLayer = pLayer->FindPrevLayer();
	}

	// Go though the layers from the new active layer upwards and set
	// all layers above to be non-visible and non-selected
	pLayer = pActiveLayer->FindNextLayer();
	while (pLayer != NULL)
	{
		if (pLayer->IsPseudoFrame())
		{
			if (pUndoOp == NULL || pSpread == NULL)
			{
				LayerSGallery::DoChangeVisible(pLayer,FALSE);
				LayerSGallery::DoChangeLocked(pLayer,TRUE);
			}
			else
			{
				// Only change if required as otherwise we will get an unwanted undo step
//				if (pLayer->IsVisible())
				{
					OpLayerGalParam Param(LAYER_VISIBLE, pSpread);
					Param.NewState = FALSE;
					Param.pLayer = pLayer;
					LayerStateAction::Init(pUndoOp, pUndoOp->GetUndoActions(), Param);
				}
				// Only change if required as otherwise we will get an unwanted undo step
//				if (!pLayer->IsLocked())
				{
					OpLayerGalParam LockedParam(LAYER_LOCKED, pSpread);
					LockedParam.NewState = TRUE;
					LockedParam.pLayer = pLayer;
					LayerStateAction::Init(pUndoOp, pUndoOp->GetUndoActions(), LockedParam);
				}
			}
		}

		pLayer = pLayer->FindNextLayer();
	}
}


/***********************************************************************************************

>	static void FrameSGallery::EnsureActiveLayerIntegrity(Spread* pSpread,Layer** ppActiveLayer = NULL)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com> (Based on Mark N's function in the Layer gallery code)
	Created:	16/4/97 (Based on Jason's Layer gallery code)
	Inputs:		pSpread = ptr to spread
	Outputs:	-
	Returns:	-
	Purpose:	This checks that there is one and only one active layer in the given spread.
				If there isn't an active layer, the first layer is made active.
				If there is more than on active layer, all but the first active layer are
				made inactive.

***********************************************************************************************/

/* void FrameSGallery::EnsureActiveLayerIntegrity(Spread* pSpread,Layer** ppActiveLayer)
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
	if (pActiveLayer == NULL)
		pActiveLayer = pSpread->FindLastLayer();

	// If we have an active layer, ensure that it really is the one and only active layer
	if (pActiveLayer != NULL)
		FrameSGallery::MakeActiveLayer(pActiveLayer);

	// If caller wants the ptr to the active layer
	if (ppActiveLayer != NULL)
		*ppActiveLayer = pActiveLayer;
} */

			
/********************************************************************************************

>	static BOOL FrameSGallery::EnsureFrameLayerIntegrity(Spread* pSpread)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/5/97
	Returns:	True if worked ok, False otherwise.
	Purpose:	This function should be called before any frame related actions are carried out
				It will ensure that all the visible layers are frame layers.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL FrameSGallery::EnsureFrameLayerIntegrity(Spread* pSpread)
{
	ERROR2IF(pSpread == NULL,FALSE,"EnsureFrameLayerIntegrity null spread passed in");

	// For the present moment just ensure that the active layer is a frame layer
	//Layer * pActiveLayer = pSpread->FindActiveLayer();
	//if (pActiveLayer)
	//	pActiveLayer->SetFrame(TRUE);

/* #if _DEBUG
	// Find out whether the operation thinks it is required or not
	String_256 ShadeReason;
	OpState State;
	State = OpCombineLayersToFrameLayer::GetState(&ShadeReason, NULL);
	if (!State.Greyed)
	{
		// Try and find the operation we want to use
		OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_COMBINELAYERSTOFRAMELAYER); 
		if (pOpDesc != NULL)
		{
			// We found it, so invoke it with the spread we were given
			OpParam Param;
			Param.Param1 = (INT32)pSpread;
			pOpDesc->Invoke(&Param);
		}
		else
		{
			ERROR3("Couldn't find OPTOKEN_COMBINELAYERSTOFRAMELAYER op descriptor");
		}
	} */

/* #if 0 //_DEBUG
// WARNING only do this in Webster builds as otherwise may be corrupting layers that are in the
// undo buffer which could lead to immense problems.
// This leads to problems in the undo e.g. draw object, import doc with layers and undo back to start
// can mean no layers in doc.
// In Camelot do nothing
#ifdef WEBSTER
	// Ensure that all other non-guide and non-background layers are frame layers
	UINT32 NonFrameLayersFound = 0;
	Layer* pCurrentLayer = pSpread->FindFirstLayer();
	Layer* pNonFrameLayer = NULL; 
	while (pCurrentLayer != NULL)
	{
		if (!pCurrentLayer->IsBackground() && !pCurrentLayer->IsGuide() &&
			!pCurrentLayer->IsPageBackground() && !pCurrentLayer->IsFrame())
		{
			// Note that we have found a non-frame layer.
			pNonFrameLayer = pCurrentLayer;
			NonFrameLayersFound++;
		}

		pCurrentLayer = pCurrentLayer->FindNextLayer();
	}

	if (NonFrameLayersFound > 0 && pNonFrameLayer != NULL)
	{
		TRACEUSER( "Neville", _T("++++++++++EnsureFrameLayerIntegrity found %d non-frame layers\n"),NonFrameLayersFound);

		// Right, we need to move all the nodes from the all non-frame layers and move them
		// to one of the non-frame layers and make this one a frame layer. The user should
		// therefore notice no difference in appearance of the document as they would have
		// been seeing one frame representing all the non-frame layers in the frame gallery.

		// First, mark the non-frame layer found in the above search as a frame layer as this
		// is the one that we are going to use as out destination. Then all other non-frame
		// searches will not find it. Also, mark it as edited.
		pNonFrameLayer->SetFrame(TRUE);
		pNonFrameLayer->SetEdited(TRUE);

		// Start a progress bar
		Progress CombiningProgress(_R(IDS_COMBINING_LAYERS));
		CombiningProgress.Update();

		BOOL UpdateSelection = FALSE;
		Layer* pLayer = pSpread->FindFirstLayer();
		while (pLayer != NULL)
		{
			if (!pLayer->IsBackground() && !pLayer->IsGuide() &&
				!pLayer->IsPageBackground() && !pLayer->IsFrame())
			{
				// Move all the top level nodes to the new layer
				// We MUST include hidden nodes as these will be in undo records
				Node * pNode = pLayer->FindFirstChild();
				while (pNode)
				{
					// Note the node to move
					Node* pNodeToMove = pNode;
					// and the next node in the tree
					pNode = pNode->FindNext();
					// Now move hide the node (should hide it but we are not undoable)
					UpdateSelection = pNodeToMove->IsSelected();
					//if (!DoHideNode(pNodeToMove, FALSE, NULL, FALSE))
					// Find the insertion point, which will be after the last child but
					// BEFORE the insertion node, if present.
					Node * pInsertPoint = pNonFrameLayer->FindLastChild(TRUE);
					if (pInsertPoint)
						pNodeToMove->MoveNode(pInsertPoint, NEXT);
					else
						pNodeToMove->MoveNode(pNonFrameLayer, LASTCHILD);

					// Update the progress display to show we have done something
					CombiningProgress.Update();
				}
				
				// Finally, go and hide that layer
				Layer * pLayerToHide = pLayer;
				// First, invalidate the bounding rect
				pLayerToHide->InvalidateBoundingRect();
				// note the next layer
				pLayer = pLayer->FindNextLayer();
				// and hide the layer itself.
				// Have to do this ourselves as we are not undoable
				pLayerToHide->CascadeDelete();
				delete pLayerToHide;
				pLayerToHide = NULL;
			}
			else
				pLayer = pLayer->FindNextLayer();

			// Update the progress display to show we have done something
			CombiningProgress.Update();
		}

		// Make sure that our new frame layer is up to date
		pNonFrameLayer->InvalidateBoundingRect();

		// If we have changed a selected node then update the selection
		if (UpdateSelection)
		{
			// Update the selection range 
			Camelot.UpdateSelection();
		}

		// We need to update the display as one or more layer items have changed status
		// So tell ourselves about the change. (We are static and so cannot do it directly!)
		BROADCAST_TO_ALL(SpreadMsg(pSpread, SpreadMsg::SpreadReason::LAYERCHANGES));

		// Update the progress display to show we have done something
		CombiningProgress.Update();
	}
#endif // WEBSTER
#endif // DEBUG
*/

// In Webster ensure that all layers are actually frames
// In Camelot do nothing
#ifdef WEBSTER
	// Ensure that all other non-guide and non-background layers are frame layers
	UINT32 NonFrameLayersFound = 0;
	Layer* pCurrentLayer = pSpread->FindFirstLayer();
	while (pCurrentLayer != NULL)
	{
		if (pCurrentLayer->IsPseudoFrame() && !pCurrentLayer->IsFrame())
		{
			pCurrentLayer->SetFrame(TRUE);
			NonFrameLayersFound++;
		}

		pCurrentLayer = pCurrentLayer->FindNextLayer();
	}

	if (NonFrameLayersFound > 0)
	{
		TRACEUSER( "Neville", _T("++++++++++EnsureFrameLayerIntegrity found %d non-frame layers\n"),NonFrameLayersFound);

		// We need to update the display as one or more layer items have changed status
		// So tell ourselves about the change. (We are static and so cannot do it directly!)
		BROADCAST_TO_ALL(SpreadMsg(pSpread, SpreadMsg::SpreadReason::LAYERCHANGES));
	} 
#endif // WEBSTER

	return TRUE;
}

/********************************************************************************************

>	static BOOL FrameSGallery::DoHideNode(Node* pNodeToHide)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/5/97
	Returns:	True if worked ok, False otherwise.
	Purpose:	This function tries to hide the specified node. Copy of the undoable operation
				version as we are not undoable and don't have an op.
				Hide it as it may be mentioned in the undo history.
Note:
This would be safer than deleting the layer nodes but if we use this, which should be the same
as the DoHideNode in undoable operation does, then it leaks the hidden nodes and layers that
are being hidden. So there must be something sneaky going on.

********************************************************************************************/

/* BOOL FrameSGallery::DoHideNode(Node* pNodeToHide)
{
	ERROR2IF(pNodeToHide == NULL,FALSE,"FrameSGallery::DoHideNode Bad node speicifed");

	// Try to hide the Node    
	NodeHidden* pHidden = new NodeHidden(pNodeToHide);
	if (pHidden == NULL)
	{          
		return FALSE; 
	}

	return TRUE;
} */


/********************************************************************************************

>	static String_256 FrameSGallery::CreateUniqueLayerID(Spread* pSpread, const String_256 * pBaseName)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com> (Based on Mark N's function in the Layer gallery code)
	Created:	16/4/97 (Based on Jason's Layer gallery code)
	Inputs:		pSpread		- the spread to use when checking
				pBaseName	- the name to use as the basis of the layer name, defaults to just 'Frame'
	Returns:	A LayerID which is unique for the layer galleries spread. 
	Purpose:	Used to generate a LayerID which is unique for the spread. The name 
				generated is normally of the form "Layer n", where n is the number of layers 
				in the spread + 1. If "Layer n" already exists on the spread then 
				"Layer m" is generated where m is the smallest integer greater than n such 
				that the id is unique.     
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

String_256 FrameSGallery::CreateUniqueLayerID(Spread* pSpread, const String_256 * pBaseName)
{
	if (pSpread == NULL) return String_256("");

	String_256 UniqueLayerID; 
	INT32 NumLayers = 0; 

	Layer* pLayer = pSpread->FindFirstLayer();

	// Calculate how many layers are on the current spread
	while (pLayer != NULL)
	{
		NumLayers++;
		pLayer = pLayer->FindNextLayer();
	}

	INT32 NewLayerNum = NumLayers;
	if (NewLayerNum == 0)
		NewLayerNum = 1; // Always start at one

	BOOL Unique;  // Flag used to indicate if the id generated is unique or not 
	String_256 LayerName(_R(IDS_DEFAULTFRAMENAME));
	// If the caller suggested a base name then use this instead.
	if (pBaseName != NULL)
		LayerName = *pBaseName;

	do 
	{
		// Construct a first attempt at a unique layer id 'Layer n' 
		// where n = the number of layers on the current spread + 1
		UniqueLayerID.MakeMsg(_R(IDS_FRAMENAMEUNIQUEFORM), (TCHAR *)LayerName, NewLayerNum);
		//UniqueLayerID.MakeMsg(_R(IDS_SGLAYER_LAYER_NUM), /*TEXT("Layer #1%ld"),*/NewLayerNum);
	
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

>	SGDisplayFrame *FrameSGallery::GetSelectedLayerGalItem()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com> (Based on Mark N's function in the Layer gallery code)
	Created:	16/4/97 (Based on Jason's Layer gallery code)
	Returns:	A ptr to the selected layer gallery item.
				NULL if there is no selectet item
	Purpose:	Gets the selected layer gallery item
				In debug builds, an ENSURE will go off if there is no selected gal item

***********************************************************************************************/

SGDisplayLayer *FrameSGallery::GetSelectedLayerGalItem()
{
	SGDisplayFrame* pDispLayer = NULL;

	if (CheckVarsAreValid())
	{
		ERROR3IF(DisplayForeground == NULL,"Foreground group is NULL");
		if (DisplayForeground == NULL)
			return NULL;

		// Look in the foreground group first
		pDispLayer = ((SGDisplayFrame *) DisplayForeground->FindNextSelectedItem(NULL));

		ERROR3IF(pDispLayer == NULL,"Can't find the selected layer item in the layer gallery");
	}

	return(pDispLayer);
}

/***********************************************************************************************

>	BOOL FrameSGallery::DoDeleteSelection(void)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com> (Based on Mark N's function in the Layer gallery code)
	Created:	16/4/97 (Based on Jason's Layer gallery code)
	Returns:	FALSE if it couldn't delete the selection. (This should, if at all possible
				leave the entire selection undeleted in this case)

	Purpose:	FORCIBLY deletes the active layer/frame.

	SeeAlso:	FrameSGallery::PrepareToDelete; ColourManager::HideColours

***********************************************************************************************/
 
BOOL FrameSGallery::DoDeleteSelection(void)
{
	OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_FRAME_DELETEFRAME); 
	if (pOpDesc != NULL)
		pOpDesc->Invoke();
	else
	{
		ERROR3("Couldn't find OPTOKEN_FRAME_DELETEFRAME op descriptor");
	}

	return TRUE;
}


/***********************************************************************************************

>	void FrameSGallery::DoCreateNewItem(void)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com> (Based on Mark N's function in the Layer gallery code)
	Created:	16/4/97 (Based on Jason's Layer gallery code)

	Purpose:	This function is meant to be overridden in derived classes
				It creates a new item and adds it to the Gallery DisplayList
				The item may be created by cloning/deriving/basing-on the
				selected item, depending on the type of Gallery concerned.

***********************************************************************************************/
void FrameSGallery::DoCreateNewItem(void)
{
	OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_FRAME_NEWFRAME); 
	if (pOpDesc != NULL)
		pOpDesc->Invoke();
	else
	{
		ERROR3("Couldn't find OPTOKEN_FRAME_NEWFRAME op descriptor");
	}
}

/***********************************************************************************************

>	void FrameSGallery::DoCopyLayer()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com> (Based on Mark N's function in the Layer gallery code)
	Created:	16/4/97 (Based on Jason's Layer gallery code)
	Inputs:		-
	Returns:	-
	Purpose:	Copies the active layer creating a new one with a default frame name.

***********************************************************************************************/

void FrameSGallery::DoCopyLayer()
{
	OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_FRAME_COPYFRAME); 
	if (pOpDesc != NULL)
		pOpDesc->Invoke();
	else
	{
		ERROR3("Couldn't find OPTOKEN_FRAME_COPYFRAME op descriptor");
	}
}

/***********************************************************************************************

>	static void FrameSGallery::DoChangeSoild(Layer* pLayer,BOOL Solid)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com> (Based on Mark N's function in the Layer gallery code)
	Created:	16/4/97 (Based on Jason's Layer gallery code)
	Inputs:		pLayer = the layer to change
				Solid - TRUE means make the displayed layer Solid
						FALSE means make Solid

	Returns:	-
	Purpose:	Called when the Solid button is clicked in a layer gallery item.
				Changes the Solidness of the displayed layer.

	Notes:		Does not redraw the layer in the display list

***********************************************************************************************/

void FrameSGallery::DoChangeSolid(Layer* pLayer,BOOL Solid)
{
	ERROR3IF(pLayer == NULL,"pLayer == NULL");
	if (pLayer == NULL) return;

	BOOL PrevSolid = pLayer->IsSolid();
	pLayer->SetSolid(Solid);
	BOOL PostSolid = pLayer->IsSolid();

	/* if (PrevSolid != PostSolid)
	{
		// Force a redraw if the layer's visibility has changed
		FrameSGallery::ForceRedrawLayer(pLayer->FindDocument(), pLayer);

		// If layer has just become invisible, remove the selections on the layer 
		if (!PostSolid)
			NodeRenderableInk::DeselectAllOnLayer(pLayer); 
	} */

	if (PrevSolid != PostSolid)
	{
		// Mark all frames above this as edited until we reach the last frame
		// OR we hit a solid/background layer.
		// Previewing all the frames copes with dependent frames
		// Previwing single frames does not if you preview the solid frame.
		Layer * pFrame = pLayer->FindNextFrameLayer();
		while (pFrame != NULL && !pFrame->IsSolid())
		{
			// Mark that frame as edited
			pFrame->SetEdited(TRUE);
			pFrame = pFrame->FindNextFrameLayer();
		}
		
		Spread* pSpread = pLayer->FindParentSpread();
		// Go and fix the status of other layers according to this new setting
		FixOtherLayers(pSpread);

		// Ensure that the buttons are in the correct state
		BROADCAST_TO_ALL(LayerMsg(pLayer, LayerMsg::LayerReason::ACTIVE_LAYER_CHANGED));
	}
}

/***********************************************************************************************

>	static void FrameSGallery::DoChangeOverlay(Layer* pLayer,BOOL Overlay)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com> (Based on Mark N's function in the Layer gallery code)
	Created:	16/4/97 (Based on Jason's Layer gallery code)
	Inputs:		pLayer = the layer to change
				Overlay - TRUE means make the displayed layer Overlayed
						  FALSE means make the displayed layer non-Overlayed

	Returns:	-
	Purpose:	Called when the Overlay button is clicked in a layer gallery item.
				Changes the Overlayness of the displayed layer.

	Notes:		Does not redraw the layer in the display list

***********************************************************************************************/

void FrameSGallery::DoChangeOverlay(Layer* pLayer,BOOL Overlay)
{
	ERROR3IF(pLayer == NULL,"pLayer == NULL");
	if (pLayer == NULL) return;

	BOOL PrevOverlay = pLayer->IsOverlay();
	pLayer->SetOverlay(Overlay);
	BOOL PostOverlay = pLayer->IsOverlay();

	//if (PrevOverlay != PostOverlay)
	//	NodeRenderableInk::DeselectAllOnLayer(pLayer); 

	if (PrevOverlay != PostOverlay)
	{
		Spread* pSpread = pLayer->FindParentSpread();
		// Go and fix the status of other layers according to this new setting
		FixOtherLayers(pSpread);

		// Ensure that the buttons are in the correct state
		BROADCAST_TO_ALL(LayerMsg(pLayer, LayerMsg::LayerReason::ACTIVE_LAYER_CHANGED));
	}
}


/***********************************************************************************************

>	void FrameSGallery::DoMoveLayer(MoveLayerType Reason)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com> (Based on Mark N's function in the Layer gallery code)
	Created:	16/4/97 (Based on Jason's Layer gallery code)
	Inputs:		Reason = how the layer should be moved
	Returns:	-
	Purpose:	Called when the Locked button is clicked in the layers gallery.
				Changes the lockedness of the selected layer, if there is one.

	Note:		Same as LayerSGallery version but uses FRAME_MOVE

***********************************************************************************************/

void FrameSGallery::DoMoveLayer(MoveLayerType Reason)
{
	SGDisplayLayer* pLayerGalItem = GetSelectedLayerGalItem();

	if (pLayerGalItem != NULL)
	{
		OpLayerGalParam Param(FRAME_MOVE, pSpread);

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
				ERROR3("Unrecognised MoveLayer reason");
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
				ERROR3("Couldn't find OPTOKEN_LAYERGALCHANGE op descriptor");
			}
		}
	}
}

/***********************************************************************************************

>	void FrameSGallery::DoShadeGallery(BOOL ShadeIt)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com> (Based on Jason's Layer gallery code)
	Created:	16/4/97 (Based on Jason's Layer gallery code)
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

void FrameSGallery::DoShadeGallery(BOOL ShadeIt)
{
	EnableGadget(_R(IDC_GALLERY_UPONE),		!ShadeIt);
	EnableGadget(_R(IDC_GALLERY_DOWNONE),	!ShadeIt);
	EnableGadget(_R(IDC_GALLERY_PROPERTIES),!ShadeIt);
	EnableGadget(_R(IDC_GALLERY_COPY),		!ShadeIt && !IsSelectedItemGuideLayer());
	EnableGadget(_R(IDC_FRAME_DELETE),		!ShadeIt);

	EnableGadget(_R(IDC_BTN_MULTILAYER),	!ShadeIt);
	EnableGadget(_R(IDC_BTN_ALLVISIBLE),	!ShadeIt);

	//EnableGadget(_R(IDC_GALLERY_NAME),		!ShadeIt);

	EnableGadget(_R(IDC_GALLERY_COPY),		!ShadeIt && !IsSelectedItemGuideLayer());

	UpdateFrameRelatedButtons();
}


/********************************************************************************************

>	virtual BOOL FrameSGallery::InitMenuCommands(void)
												 
	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com> (Based on Jason's Layer gallery code)
	Created:	16/4/97 (Based on Jason's Layer gallery code)

	Returns:	TRUE for success

	Purpose:	Initialises any menu commands that this gallery needs.

	Notes:		Will only create the menu commands once - further calls in the future
				will return TRUE immediately wihtout doing anything.

********************************************************************************************/

BOOL FrameSGallery::InitMenuCommands(void)
{
	static BOOL MenusInitialised = FALSE;

	BOOL ok = TRUE;

	if (!MenusInitialised)
	{
		// Initialise menu command Ops
		// "Special" entries for over-list menu
		ok = ok && InitMenuCommand((StringBase *) &SGCmd_Copy, 		_R(IDS_COPYFRAME));
		ok = ok && InitMenuCommand((StringBase *) &SGCmd_Delete, 	_R(IDS_DELETEFRAME));
		ok = ok && InitMenuCommand((StringBase *) &SGCmd_New, 		_R(IDS_NEWFRAME));			// _R(IDS_SGMENU_NEW)
//		ok = ok && InitMenuCommand((StringBase *) &SGCmd_Animation,	_R(IDS_SGMENU_ANIMATION));
//		ok = ok && InitMenuCommand((StringBase *) &SGCmd_Save,		_R(IDS_SGMENU_SAVE));
		ok = ok && InitMenuCommand((StringBase *) &SGCmd_Preview,	_R(IDS_GRABFRAME));			// _R(IDS_SGMENU_PREVIEW));
//		ok = ok && InitMenuCommand((StringBase *) &SGCmd_Browser,	_R(IDS_SGMENU_BROWSER));
		ok = ok && InitMenuCommand((StringBase *) &SGCmd_Properties,_R(IDS_FRAMEPROPERTIES));	//_R(IDS_SGMENU_FRAMEPROPS));
//		ok = ok && InitMenuCommand((StringBase *) &SGCmd_FrameProps,_R(IDS_SGMENU_FRAMEPROPS));
		ok = ok && InitMenuCommand((StringBase *) &SGCmd_Show,		_R(IDS_SGMENU_SHOWFRAME));
		MenusInitialised = TRUE;
	}

	return(ok);
}



/********************************************************************************************

>	virtual BOOL FrameSGallery::BuildCommandMenu(GalleryContextMenu *TheMenu, SGMenuID MenuID)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com> (Based on Jason's Layer gallery code)
	Created:	16/4/97 (Based on Jason's Layer gallery code)

	Inputs:		TheMenu - The menu to add commands to
				MenuID  - The type of menu (over-list or from-options-button) to create

	Returns:	TRUE if it succeeded

	Purpose:	To build a menu of commands to be popped up over the gallery.
	
	Notes:		The Layer gallery provides only a simple item-pop-up menu

********************************************************************************************/

BOOL FrameSGallery::BuildCommandMenu(GalleryContextMenu *TheMenu, SGMenuID MenuID)
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
			GIFAnimationPropertyTabs * pTabHandler = GIFAnimationPropertyTabsDlg::GetGIFAnimationPropertiesTabs();
			//LayerPropertyTabs * pTabHandler = LayerPropertyTabsDlg::GetLayerPropertiesTabs();
			if (pTabHandler)
			{
				pTabHandler->SetCurrentLayer(pLayerGalItem->GetDisplayedLayer());
			}
		}
#endif
		ok = ok && AddCommand(TheMenu, (StringBase *) &SGCmd_New);
		ok = ok && AddCommand(TheMenu, (StringBase *) &SGCmd_Copy);
		ok = ok && AddCommand(TheMenu, (StringBase *) &SGCmd_Delete,TRUE);
#ifndef STANDALONE
//		ok = ok && AddCommand(TheMenu, (StringBase *) &SGCmd_Animation);
//		ok = ok && AddCommand(TheMenu, (StringBase *) &SGCmd_Save);
		ok = ok && AddCommand(TheMenu, (StringBase *) &SGCmd_Preview);
		ok = ok && AddCommand(TheMenu, (StringBase *) &SGCmd_Properties);
//		ok = ok && AddCommand(TheMenu, (StringBase *) &SGCmd_FrameProps);
//		ok = ok && AddCommand(TheMenu, (StringBase *) &SGCmd_Browser);
		ok = ok && AddCommand(TheMenu, (StringBase *) &SGCmd_Show);
#endif
	}

	return(ok);
}



/********************************************************************************************

>	virtual OpState FrameSGallery::GetCommandState(StringBase *CommandID, String_256 *ShadeReason)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com> (Based on Jason's Layer gallery code)
	Created:	16/4/97 (Based on Jason's Layer gallery code)

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

OpState FrameSGallery::GetCommandState(StringBase *CommandID, String_256 *ShadeReason)
{
	OpState State;

	if (AmShaded)		// No commands available while the gallery is shaded
	{
		State.Greyed = TRUE;
		return(State);
	}

	if (*CommandID == SGCmd_Copy 	|| // --- Copy/Delete/Properties
	//	*CommandID == SGCmd_Delete	||
		*CommandID == SGCmd_Show
//		*CommandID == SGCmd_Properties ||
//		*CommandID == SGCmd_Animation ||
//		*CommandID == SGCmd_Browser ||
//		*CommandID == SGCmd_FrameProps	
		)
	{
		SGDisplayLayer* pItem = GetSelectedLayerGalItem();

		if (pItem == NULL)
		{
			State.Greyed = TRUE;
			ShadeReason->MakeMsg(_R(IDS_SGSHADE_SINGLECOL));
		}
		else if (*CommandID == SGCmd_Copy)			// Can't copy Guide layers
			State.Greyed = IsSelectedItemGuideLayer();
		else if (*CommandID == SGCmd_Show)			// Can't hide Guide layers
		{
			State.Greyed = IsSelectedItemGuideLayer();
			Layer * pLayer = pItem->GetDisplayedLayer();
			if (pLayer)
				State.Ticked = !pLayer->IsHiddenFrame();
			else
				State.Greyed = TRUE;
		}
/* #ifndef STANDALONE
		else if (*CommandID == SGCmd_Properties)	// Shade when dlg is open
		{
			//State.Greyed = (LayerPropertyTabs::GetTabbedDlg() != NULL);
			LayerPropertyTabs * pTabHandler = LayerPropertyTabsDlg::GetLayerPropertiesTabs();
			if (pTabHandler)
				State.Greyed = (pTabHandler->GetTabbedDlg() != NULL);
			else
				State.Greyed = TRUE;	
		}
#endif */
	}
/* #if _DEBUG
	else if (*CommandID == SGCmd_Save)
	{
		// Just call the GetState function for the op directly
		return OpSaveAnimatedGIF::GetState(ShadeReason, NULL);
	}
#endif */
	else if (*CommandID == SGCmd_Preview)
	{
		// Just call the GetState function for the op directly
		return OpGrabAllFrames::GetState(ShadeReason, NULL);
	}
	else if (*CommandID == SGCmd_FrameProps || *CommandID == SGCmd_Properties)
	{
		// Call the GetState function for the frame properties op.
		return OpFrameProperties::GetState(ShadeReason, NULL);		
	}
	else if (*CommandID == SGCmd_Delete)
	{
		// Call the GetState function for the frame delete op.
		return OpDeleteFrame::GetState(ShadeReason, NULL);	
	}

	// Don't call the base class 'cos we don't want any other commands

	return(State);
}



/********************************************************************************************

>	virtual void FrameSGallery::DoCommand(StringBase *CommandID)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com> (Based on Jason's Layer gallery code)
	Created:	16/4/97 (Based on Jason's Layer gallery code)

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

void FrameSGallery::DoCommand(StringBase *CommandID)
{
	if (*CommandID == SGCmd_Copy)
		DoCopyLayer();
	else if (*CommandID == SGCmd_New)
		DoCreateNewItem();
	else if (*CommandID == SGCmd_Delete)
	{
		//if (PrepareToDelete()) // No warning on Frame gallery
		DoDeleteSelection();
	}
//	else if (*CommandID == SGCmd_Properties)
//		DoFrameProperties();
	else if (*CommandID == SGCmd_Preview)
	{
		DoPreviewAnimation();
	}
	else if (*CommandID == SGCmd_FrameProps || *CommandID == SGCmd_Properties)
	{
		// Open the Animation properties dlg with the "Frame properties" page selected.
		DoFrameProperties();
	}
	else if (*CommandID == SGCmd_Show)
	{
		// Toggle the state of the show frame in animation flag
		SGDisplayLayer* pItem = GetSelectedLayerGalItem();
		Layer * pLayer = NULL;
		if (pItem)
			pLayer = pItem->GetDisplayedLayer();
		if (pLayer)
		{
			BOOL CurrentState = pLayer->IsHiddenFrame();
			pLayer->SetHiddenFrame(!CurrentState);
			if (pItem)
				pItem->ForceRedrawOfMyself();

			// Ensure that the buttons are in the correct state
			BROADCAST_TO_ALL(LayerMsg(pLayer, LayerMsg::LayerReason::ACTIVE_LAYER_CHANGED));
		}
	}
//	else if (*CommandID == SGCmd_Save)
//		DoSaveAnimation();
	// Implement ****
	//	*CommandID == SGCmd_Animation
	//	*CommandID == SGCmd_Browser

	// Don't call the base class 'cos we don't want any other commands
}

/***********************************************************************************************

>	virtual BOOL FrameSGallery::DoSaveAnimation()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/4/97
	Inputs:		-
	Returns:	True if worked ok, False otherwise
	Purpose:	Saves out the current animation to file.

***********************************************************************************************/

BOOL FrameSGallery::DoSaveAnimation()
{
	// Not required at present
/*	OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_SAVEANIMATEDGIF); 
	if (pOpDesc != NULL)
		pOpDesc->Invoke();
	else
	{
		ERROR3("Couldn't find OPTOKEN_FRAME_COPYFRAME op descriptor");
	}
*/
	return TRUE;
}

/***********************************************************************************************

>	virtual BOOL FrameSGallery::DoPreviewAnimation()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/4/97
	Inputs:		-
	Returns:	True if worked ok, False otherwise
	Purpose:	Previews the current animation.

***********************************************************************************************/

BOOL FrameSGallery::DoPreviewAnimation()
{
	// Check to see if the operation is available
	// No longer required as the button is hooked into the real op
	//String_256 ShadeReason;
	//OpState State;
	
	//State = OpGrabAllFrames::GetState(&ShadeReason, NULL);
	//if (State.Greyed)
	//	return FALSE;

	// Normally we want to invoke the grab frame, but we will sneak in that
	// if the shift key is pressed then we will grab all frames.
	OpDescriptor* pOpDesc = NULL;
	// Is the shift key pressed? 
	if (KeyPress::IsAdjustPressed())
		pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_FRAME_GRABALLFRAMES); 
	else
		pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_FRAME_GRABFRAME); 

	if (pOpDesc != NULL)
		pOpDesc->Invoke();
	else
	{
		ERROR3("Couldn't find OPTOKEN_FRAME_GRABFRAME/_GRABALLFRAMES op descriptor");
	}

	return TRUE;
}

/***********************************************************************************************

>	BOOL FrameSGallery::DoRegenerateSingleFrame()

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/05/97
	Inputs:		-
	Returns:	True if worked ok, False otherwise
	Purpose:	While previewing an animation, if the frame layer properties are changed
				we neeed to regenerate the active frame. This function invokes the correct Op.

*************************************************************************************************/

BOOL FrameSGallery::DoRegenerateSingleFrame()
{
	OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_FRAME_GRABFRAME); 
	GIFAnimationExportParam Param;
	if (pOpDesc != NULL)
		pOpDesc->Invoke(&Param);
	else
	{
		ERROR3("Couldn't find OPTOKEN_FRAME_GRABALLFRAMES op descriptor");
	}

	return TRUE;
}

/***********************************************************************************************

>	BOOL FrameSGallery::DoRegenerateFrames()

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/05/97
	Inputs:		-
	Returns:	True if worked ok, False otherwise
	Purpose:	While previewing an animation, if the spread properties are changed
				we neeed to regenerate the frame layers. 
				This function invokes the correct Op.

*************************************************************************************************/

BOOL FrameSGallery::DoRegenerateFrames()
{
	OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_FRAME_GRABALLFRAMES); 
	if (pOpDesc != NULL)
		pOpDesc->Invoke();
	else
	{
		ERROR3("Couldn't find OPTOKEN_FRAME_GRABALLFRAMES op descriptor");
	}

	return TRUE;
}


/***********************************************************************************************

>	BOOL FrameSGallery::DoFrameProperties()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/5/97
	Inputs:		-
	Returns:	True if worked ok, False otherwise
	Purpose:	Opens the frame properties dlg

***********************************************************************************************/

BOOL FrameSGallery::DoFrameProperties()
{
	OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_GIFANIMPROPERTYTABS);

	if (pOpDesc != NULL)
	{
		// Select the "Frame properties tab".
		GIFAnimationPropertyTabsDlg::SetPageToOpen(GIFAnimationPropertyTabs::FramePropertiesTabNumber);

		String_256 Str;
		OpState State = pOpDesc->GetOpsState(&Str);
		if (!State.Greyed)
			pOpDesc->Invoke();
		else
		{
			GIFAnimationPropertyTabs * pTabHandler = GIFAnimationPropertyTabsDlg::GetGIFAnimationPropertiesTabs();
			if (pTabHandler)
			{
				DialogTabOp* pDlg =  pTabHandler->GetTabbedDlg();
				if (pDlg != NULL)
					pDlg->Open();
			}
		}
	}
	return TRUE;
}

/***********************************************************************************************

>	BOOL FrameSGallery::UpdateFrameRelatedButtons()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/5/97
	Inputs:		-
	Returns:	True if worked ok, False otherwise
	Purpose:	Ensures that they buttons on the gallery up to date in their greying/ungreying
				status.

***********************************************************************************************/

BOOL FrameSGallery::UpdateFrameRelatedButtons()
{
	// This is no longer required for the present as we have hooked the buttons
	// into the real ops that we have.

//	String_256 ShadeReason;
//	OpState State;
	
//	State = OpGrabAllFrames::GetState(&ShadeReason, NULL);
//	EnableGadget(_R(IDC_BMPGAL_PREVIEW), !State.Greyed);

//	State = OpFrameProperties::GetState(&ShadeReason, NULL);
//	EnableGadget(_R(IDC_GALLERY_PROPERTIES), !State.Greyed);

	return TRUE;
}

/********************************************************************************************

>	BOOL FrameSGallery::CloseFrameGallery()

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/10/97
	Inputs:		-
	Returns:	TRUE if sucessful, else FALSE.
	
********************************************************************************************/

BOOL FrameSGallery::CloseFrameGallery()
{
	String_32 Name(_R(IDS_FRAMEGAL_GALLNAME));
	DialogBarOp* pDialogBarOp = DialogBarOp::FindDialogBarOp(Name);

	// Ensure a valid ptr.
	if (pDialogBarOp == NULL)
		return FALSE;
	
	if (pDialogBarOp->GetRuntimeClass() == CC_RUNTIME_CLASS(FrameSGallery))
	{
		GIFAnimationPropertyTabs::SetFrameGalleryOpen(FALSE);

		// Toggle the visible state of the gallery window
		pDialogBarOp->SetVisibility( FALSE );
	}
	else
	{
		ERROR3("Got the frame gallery but it's not of the LayerSGallery class");
	}

	SGInit::UpdateGalleryButton(OPTOKEN_DISPLAYFRAMEGALLERY, FALSE);

	GIFAnimationPropertyTabs::SetFrameGalleryOpen(FALSE);

	// Everything ok.
	return TRUE;
}

/********************************************************************************************

>	BOOL FrameSGallery::OpenLayerGallery()

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/10/97
	Inputs:		-
	Returns:	TRUE if the document is Frame based, else FALSE.

********************************************************************************************/

BOOL FrameSGallery::OpenLayerGallery()
{
	// Find the OP. for the Layer gallery.
	OpDescriptor* OpDesc;
	OpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_DISPLAYLAYERGALLERY);

	// Ensure we get a ptr to the OP.
	if(!OpDesc)
		return FALSE;

	// ok, now open the layergallery.
	OpDesc->Invoke();		 

	// Lets go back.
	return TRUE;
}


/********************************************************************************************

>	BOOL FrameSGallery::IsFrameMode()

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/10/97
	Inputs:		Pointer to the document.
	Returns:	TRUE if the document is Frame based, else FALSE.

********************************************************************************************/

BOOL FrameSGallery::IsFrameMode(Document* pDoc)
{
	ERROR3IF(pDoc == NULL,"pDoc == NULL");

	// flag to determine the document mode.
	BOOL FrameMode = FALSE;

	// Get a ptr to the selected spread
	Spread* pSpread = pDoc->GetSelectedSpread();

	// Ensure a valid spread ptr.
	if(pSpread)
	{
		// Are there any frame layers?
		Layer* pFrameLayer = pSpread->FindFirstFrameLayer();	

		//If a frame layer exists, then this is an animation doc.
		if (pFrameLayer)
			FrameMode = TRUE;
	}

	// return the document mode.
	return FrameMode;
}

#endif // EXCLUDE_GALS


