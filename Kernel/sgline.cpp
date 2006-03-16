// $Id: sgline.cpp 662 2006-03-14 21:31:49Z alex $
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
// sgline.cpp - Line SuperGallery classes - LineGallery and the LineAttrItem base class.

/*
*/

#include "camtypes.h"       // global types
#include "app.h"

#include "range.h"          // selection & attributes
#include "qualattr.h"
#include "attrmgr.h"

#include "ccdc.h"           // render-into-dialogue support
#include "fillval.h"
#include "grnddib.h"

//#include "galres.h"         // super-gallery support
//#include "galstr.h"
#include "sginit.h"
#include "sgline.h"
#include "sgline2.h"
#include "sgmenu.h"
#include "sglinepr.h"

#include "sgldrag.h"        // dragging support
#include "dragmgr.h"

#include "lattrops.h"      // gadgets and resources
//#include "justin.h"
//#include "mario.h"			// for _R(IDE_NOMORE_MEMORY)
//#include "richard2.h"		// for yet more strings

#ifdef VECTOR_STROKING
#include "sgstroke.h"		// Stroke attribute items
#include "strkattr.h"		// Stroke attributes
#include "ppstroke.h"		// PathProcessorStroke
#include "ppairbsh.h"		// PathProcessorStrokeAirbrush
#include "ppvecstr.h"		// PathProcessorStrokeVector
#include "valfunc.h"		// ValueFunctions
#endif

#include "helpuser.h"
//#include "xshelpid.h"
//#include "helppath.h"

#include "brshcomp.h"        // for the new style brushes
#include "ppbrush.h"
#include "brshattr.h"
#include "sgbrush.h"
#include "lineattr.h"
//#include "rik.h"
#include "brushmsg.h"
#include "brshname.h"
#include "resdll.h"
#include "fileutil.h"
#include "brushop.h"

//#include "freehres.h"		//_R(IDS_FEATURENOTIMPLEMENTED)
//#include "resource.h"		//_R(IDS_OK)

//#include "linegal.h"

// Version control.
DECLARE_SOURCE("$Revision: 662 $");

// Implement the dynamic class bits...
CC_IMPLEMENT_DYNCREATE(LineGallery, SuperGallery)
CC_IMPLEMENT_DYNAMIC(LineAttrItem, SGDisplayItem)
CC_IMPLEMENT_DYNCREATE(OpDisplayLineGallery, Operation);

// This line mustn't go before any CC_IMPLEMENT_... macros
#define new CAM_DEBUG_NEW


// This always points to the line gallery, or NULL if it doesn't exist.
LineGallery* LineGallery::m_pTheGallery = NULL;

// These are the default scaling factors for the four stock sizes of arrowheads.
double LineGallery::m_anArrowScales[4] = { 3.0, 8.0, 13.0, 18.0 };

// initialise the array that holds the filenames of the default brush files
String_256* LineGallery::m_pFileNameArray = NULL;
UINT32 LineGallery::m_NumDefaultFiles = 0;

/********************************************************************************************
>   LineAttrItem::LineAttrItem(const String& strDescription,
							   LineAttritem::TextPosition eTextPos)
	Author:     Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:    27/3/95
	Inputs:     strDescription          text description of this item, eg "0.5 pt"
				eTextPos                the default position for this item's text
										description
	Outputs:    -
	Returns:    -
	Purpose:    Default constructor for a LineAttrItem.
	Errors:     -
	SeeAlso:    SGDisplayItem::SGDisplayItem
********************************************************************************************/

LineAttrItem::LineAttrItem(const String& strDescription, LineAttrItem::TextPosition eTextPos)
  : m_strDescription(strDescription),
	m_eDefaultTextPosition(eTextPos),
	m_fIsCurrentAttrib(FALSE)
{
	// Empty.
}



/********************************************************************************************
>   static BOOL LineAttrItem::UpdateCurrentAttribStatus()

	Author:     Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:    17/4/95
	Inputs:     -
	Outputs:    -
	Returns:    TRUE if no problems encountered, eg. out of memory.
	Purpose:    Updates the "current attribute" status flag of every LineAttrItem
				within the line gallery, setting it to whether each item represents an
				attribute value applying to the current selection.
	Errors:     Out of memory.
	SeeAlso:    LineAttrItem::GetRuntimeClass; LineAttrItem::IsEqualValue
********************************************************************************************/

BOOL LineAttrItem::UpdateCurrentAttribStatus()
{
	// If the line gallery isn't running then we don't bother.
	if (!LineGallery::IsActive()) return TRUE;
	
	// For each group in the line gallery, try to match the current attribute of the
	// selection with the group's items.
	SGDisplayRoot* pRoot = LineGallery::GetInstance()->GetDisplayTree();
	if (pRoot == NULL) return FALSE;

	for (LineAttrGroup* pGroup = (LineAttrGroup*) pRoot->GetChild();
		 pGroup != NULL;
		 pGroup = (LineAttrGroup*) pGroup->GetNext())
	{
		// Get a pointer to the first item within the group, if any.
		LineAttrItem* pItem = (LineAttrItem*) pGroup->GetChild();
		if (pItem == NULL) continue;

		// Get a pointer to the null-terminated array of pointers to run-time classes that
		// this type of item can represent.
		CCRuntimeClass** ppAttribClasses = pItem->GetAttribRuntimeClasses();
		ERROR3IF(ppAttribClasses == NULL,
					"No attribute run-time classes in LineAttrItem::UpdateCurrentAttribStatus");

		// Count how many types of classes the item represents.  Note the null statement.
		for (CCRuntimeClass** pprtc = ppAttribClasses; *pprtc != NULL; pprtc++);
		INT32 nAttribClasses = pprtc - ppAttribClasses;

		// Allocate an array of NodeAttribute pointers for each run-time class.
		typedef NodeAttribute* PATTRIBUTE;
		NodeAttribute** ppCommonAttribs = new PATTRIBUTE[nAttribClasses];
		ERRORIF(ppCommonAttribs == NULL, _R(IDE_NOMORE_MEMORY), FALSE);

		// For each run-time class that appears in *ppAttribClasses, try to find an
		// attribute of the same class that currently applies to the selection.
		for (INT32 i = 0; i < nAttribClasses; i++)
		{
			NodeAttribute* pattr = NULL; // sjk
			SelRange::CommonAttribResult eResult;
			
			eResult = GetApplication()->
						FindSelection()->
							FindCommonAttribute(ppAttribClasses[i], &pattr);

			// See if we found either a common attribute or the "default attribute".
			if (pattr != NULL)
			{
				// Check for nothing really brain-damaged happening.
				ERROR3IF(pattr->GetRuntimeClass() != ppAttribClasses[i],
							"Wrong kind of attribute in LineAttrItem::"
							"UpdateCurrentAttribStatus");

				// There is just one attribute applying, and we got it.
				ppCommonAttribs[i] = pattr;
			}
			else
			{
				// There are either many or none applying, so we aren't interested.
				ppCommonAttribs[i] = NULL;
			}		
		}

		// OK, so now we have an array of all the attributes this type of item is
		// interested in.  Ask each item within the group to compare itself against
		// each attribute we have found, to see if it represents it or not.
		for (; pItem != NULL; pItem = (LineAttrItem*) pItem->GetNext())
		{
			BOOL fOldState = pItem->m_fIsCurrentAttrib;
			pItem->m_fIsCurrentAttrib = pItem->IsEqualValueToAny(ppCommonAttribs);
			if (pItem->m_fIsCurrentAttrib != fOldState) pItem->ForceRedrawOfMyself();
		}

		// All done, we are finished with these kinds of attributes.
		delete[] ppCommonAttribs;
	}

	// Everything went OK.
	return TRUE;
}



/***********************************************************************************************
>   virtual void LineAttrItem::CalculateMyRect(SGFormatInfo* pFormatInfo,
														SGMiscInfo* pMiscInfo)
	Author:     Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:    15/2/95 (base generated in sgbase.cpp)
	Inputs:     pFormatInfo - The formatting info from which to calculate my position/size
				pMiscInfo - As usual, the useful misc info struct
	Outputs:    member variable FormatRect - is returned filled in with the size/position of
				this LineAttrItem item's display area. This is dependent upon the current display
				mode and format state.
				FormatInfo will be updated as a result of the formatting operation
	Purpose:    Shared code for LineAttrItem items to calculate where they will appear in the
				grand scheme of things
	Notes:      LineAttrItems supply only one display mode ("full info")
	Scope:      private (for use of LineAttrItem class only)
***********************************************************************************************/

void LineAttrItem::CalculateMyRect(SGFormatInfo* pFormatInfo, SGMiscInfo* pMiscInfo)
{
	// Find out the extent of this item, including a gap for formatting.
	MILLIPOINT xSize, ySize;
	GetSize(m_eDefaultTextPosition, &xSize, &ySize);
	if (xSize != SG_InfiniteWidth)
		xSize += GetHorzGap();

	ySize += GetVertGap();

	// Let the base class do what it needs to do with this.
	CalculateFormatRect(pFormatInfo, pMiscInfo, xSize, ySize);
}



/********************************************************************************************
>	void LineAttrItem::GetSize(LineAttrItem::TextPosition eTextPos,
						   	   MILLIPOINT* pxSize,
						   	   MILLIPOINT* pySize) const
	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/4/95
	Inputs:		eTextPos		where this item's text description should go
	Outputs:	pxSize			where to store the width of the item
				pySize			where to store the height of the item
	Returns:	-
	Purpose:	Used to calculate how large this item is, in millipoints.
	Errors:		-
	SeeAlso:	LineAttrItem::CalculateMyRect
********************************************************************************************/

void LineAttrItem::GetSize(LineAttrItem::TextPosition eTextPos,
						   MILLIPOINT* pxSize,
						   MILLIPOINT* pySize) const
{
	// Make sure we can put the results somewhere.
	ERROR3IF(pxSize == NULL || pySize == NULL, "Null* passed in LineAttrItem::GetSize");

	// Get the basic size and add in a possible text size as well.
	*pxSize = GetWidth();
	*pySize = GetHeight();
	switch (eTextPos)
	{
		case LEFT:
			if (*pxSize != SG_InfiniteWidth)
			{
				*pxSize += (c_nHorzSizeOfText + c_nHorzGapAfterText);
			}
			break;

		case BELOW:
			*pySize += (c_nVertSizeOfText + c_nVertGapAboveText);
			break;

		case NO_LABEL:
			break;

		default:
			ERROR3("Unexpected case in LineAttrItem::GetSize");
			break;
	}
}



/***********************************************************************************************
>   virtual void LineAttrItem::HandleRedraw(SGRedrawInfo* pRedrawInfo,
													 SGFormatInfo* pFormatInfo)
	Author:     Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:    15/2/95 (base generated in sgbase.cpp)
	Inputs:     pRedrawInfo - The information on the kernel-rendered redraw area
				pFormatInfo - The formatting information structure
				
				Member variable FormatRect should be set up (before calling this method)
				to be the rectangle in which to draw this item
	Purpose:    LineAttrItem item redraw method - removed from the main HandleEvent
				method merely to make the code tidier.

				Justin here: this version sets default attributes within the render region,
				such as foreground & background colour, and then calls the Render() function
				to do the actual drawing.
	Notes:      VERY IMPORTANT: The rendering must be enclosed by calls to StartRendering
				and StopRendering, to ensure that rendering works properly (in the future
				we may change the redraw system to use a GRenderRegion for each individual
				item, rather than one global one for the window, for which these calls will
				be essential)
	Scope:      protected
***********************************************************************************************/

void LineAttrItem::HandleRedraw(SGRedrawInfo* pRedrawInfo, SGMiscInfo* pMiscInfo)
{
	// Remove the gap between the items, shrink by one pixel to ensure clean clipping,
	// and fix the bounds coincide with the position of pixels.
	DocRect drBounds(FormatRect);
	drBounds.Inflate(-(pMiscInfo->PixelSize + GetHorzGap() / 2),
					 -(pMiscInfo->PixelSize + GetVertGap() / 2));
	GridLockRect(pMiscInfo, &drBounds);

	// Set the foreground and background colours according to whether this item is
	// selected or not.
	DocColour dcolForegnd, dcolBackgnd;
	if (Flags.Selected)
	{
		dcolForegnd = pRedrawInfo->SelForeground;
		dcolBackgnd = pRedrawInfo->SelBackground;
	}
	else
	{
		dcolForegnd = pRedrawInfo->Foreground;
		dcolBackgnd = pRedrawInfo->Background;
	}

	// Set the attributes for drawing the formatted rectangle.
	RenderRegion* pRegion = pRedrawInfo->Renderer;
	pRegion->SetLineWidth(pMiscInfo->PixelSize);		// line is one pixel wide
	pRegion->SetFillColour(dcolBackgnd);

	// If we are drawing an item that is a "current attribute" then we want to draw it
	// with a "selected" line around it.  Otherwise we draw it with effectively no line.
	if (m_fIsCurrentAttrib)
	{
		pRegion->SetLineColour(pRedrawInfo->SelBackground);
	}
	else
	{
		// Draw the line the same colour as the fill colour, so it's "invisible".
		pRegion->SetLineColour(dcolBackgnd);
	}

	// Create a closed path that is filled with the fill colour and if necessary has
	// a visible outline.
	Path pth;
	pth.Initialise();
	pth.IsFilled = TRUE;
	pth.IsStroked = m_fIsCurrentAttrib;		// draw outline if "current attribute"

	// "Draw" the bounding rectangle into the path.
	pth.InsertMoveTo(drBounds.lo);
	pth.InsertLineTo(DocCoord(drBounds.lo.x, drBounds.hi.y));
	pth.InsertLineTo(drBounds.hi);
	pth.InsertLineTo(DocCoord(drBounds.hi.x, drBounds.lo.y));
	pth.InsertLineTo(drBounds.lo);
	pth.CloseSubPath();
	
	// Draw the path.
	pRegion->DrawPath(&pth);

	// Reset the rendering attributes back to the normal: standard gallery foreground
	// and background colours.
	pRegion->SetLineColour(dcolForegnd);
	pRegion->SetFixedSystemTextColours(&dcolForegnd, &dcolBackgnd);

	// Shrink the rectangle by a pixel, to make sure it nicely fits within the bounds,
	// lock down, and let the derived class do the rest of the work as it pleases.
	drBounds.Inflate(-pMiscInfo->PixelSize, -pMiscInfo->PixelSize);
	GridLockRect(pMiscInfo, &drBounds);
	Render(pRegion, drBounds, m_eDefaultTextPosition);
}



/********************************************************************************************
>   void LineAttrItem::Render(RenderRegion* pRegion,
							  const DocRect& drBounds,
							  LineAttrItem::TextPosition eTextPos) const
	Author:     Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:    27/3/95
	Inputs:     pRegion         pointer to the RenderRegion to render into
				drBounds        the bounding box to render within
				eTextPos        whether and where to put the text description
	Outputs:    -
	Returns:    -
	Purpose:    Called by the HandleRedraw function.  Can be overridden by derived classes.
				This base class version draws the text describing this item within the
				text bounding box, and a line within the line bounding box.
	Errors:     -
	SeeAlso:    LineAttrItem::HandleRedraw
********************************************************************************************/

void LineAttrItem::Render(RenderRegion* pRegion, const DocRect& drBounds,
						  LineAttrItem::TextPosition eTextPos) const
{
	// Make a copy of the given bounding box rectangle so that we can adjust it later
	// if we render some text within it.
	DocRect drLineBounds(drBounds);
	
	// Next, render the text description of this item, if necessary.
	if (eTextPos != NO_LABEL)
	{
		// We must adjust the bounding box of the line to account for the text rendered
		// into it.  Items display their text on the left of the rendered attribute.
		DocRect drTextBounds(drLineBounds);
		switch (eTextPos)
		{
			case BELOW:
			{
				// Divide the bounds horizontally, text below.
				drTextBounds.hi.y = drTextBounds.lo.y + c_nVertSizeOfText;
				drLineBounds.lo.y = drTextBounds.hi.y + c_nVertGapAboveText;

				// Centre the text bounds.  As there is no "GetTextExtent" function for
				// "system text" we have the obligatory bodge . . .
//				MILLIPOINT nTextLen = (GetDescription().Length() + 1) * c_nAveCharWidth;
				DocRect TextBounds;
				String_256 Desc = GetDescription();
				pRegion->GetFixedSystemTextSize(&Desc, &TextBounds);
				MILLIPOINT nTextLen = TextBounds.Width() / 2;
				MILLIPOINT xMid = (drTextBounds.lo.x + drTextBounds.hi.x) / 2;
				drTextBounds.lo.x = xMid - nTextLen;
				drTextBounds.hi.x = xMid + nTextLen;
				break;
			}

			case LEFT:
				// Divide the bounds vertically, text to the left.
				drTextBounds.hi.x = drTextBounds.lo.x + c_nHorzSizeOfText;
				drLineBounds.lo.x = drTextBounds.hi.x + c_nHorzGapAfterText;
				break;

			default:
				ERROR3("Unexpected case in LineAttrItem::Render");
				return;
		}
	
		// Draw the text description of this item.  Note that the foreground and
		// background colours have already been set by the caller.
		pRegion->DrawFixedSystemText((String*) &GetDescription(), drTextBounds);
	}

	// Set the default line width for lines rendered within the gallery and let a
	// derived class set some specialised rendering attributes for its item. Then draw
	// the line, by default through the centre of the given bounding box.  A derived
	// class may very occasionally need to override this as well (eg. to render join-types).
	pRegion->SaveContext();
		pRegion->SetLineWidth(c_nDefaultLineWidth);
		SetAttributes(pRegion);
		DrawItem(pRegion, drLineBounds);
	pRegion->RestoreContext();
}



/********************************************************************************************
>   virtual void LineAttrItem::DrawItem(RenderRegion* pRegion, const DocRect& drBounds) const

	Author:     Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:    29/3/95
	Inputs:     pRegion             pointer to the RenderRegion to draw into
				drBounds            reference to the bounding box rectangle to draw within
	Outputs:    -
	Returns:    -
	Purpose:    Draws this item within the bouding box.  This default implementation draws
				a single line through the centre of the box, fine for almost all derived
				classes.  You can override this to do something else (cf. the join-types
				attribute item, that must draw two lines that meet).
	Errors:     -
	SeeAlso:    LineAttrItem::Render
********************************************************************************************/

void LineAttrItem::DrawItem(RenderRegion* pRegion, const DocRect& drBounds) const
{
	// Calculate the mid-point of the vertical sides of the bounding box.
	INT32 yMid = (drBounds.lo.y + drBounds.hi.y) / 2;

	// Draw a line at this height from the left to the right edge.
	Path pthLinePath;
	if (pthLinePath.Initialise())
	{
		pthLinePath.InsertMoveTo(DocCoord(drBounds.lo.x, yMid));
		pthLinePath.InsertLineTo(DocCoord(drBounds.hi.x, yMid));
		pthLinePath.IsFilled  = FALSE;
		pthLinePath.IsStroked = TRUE;
		pRegion->DrawPath(&pthLinePath);
	}
}



/********************************************************************************************
>	virtual MILLIPOINT LineAttrItem::GetWidth() const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/4/95
	Inputs:		-
	Outputs:	-
	Returns:	The width of this item, in millipoints.
	Purpose:	Called by the formatting code.
	Errors:		-
	SeeAlso:	LineAttrItem::GetSize; LineAttrItem::CalculateMyRect
********************************************************************************************/

MILLIPOINT LineAttrItem::GetWidth() const
{
	return c_nHorzSizeOfItem;
}



/********************************************************************************************
>	virtual MILLIPOINT LineAttrItem::GetHeight() const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/4/95
	Inputs:		-
	Outputs:	-
	Returns:	The height of this item, in millipoints.
	Purpose:	Called by the formatting code.
	Errors:		-
	SeeAlso:	LineAttrItem::GetSize; LineAttrItem::CalculateMyRect
********************************************************************************************/

MILLIPOINT LineAttrItem::GetHeight() const
{
	return c_nVertSizeOfItem;
}



/********************************************************************************************
>   virtual MILLIPOINT LineAttrItem::GetHorzGap() const

	Author:     Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:    17/4/95
	Inputs:     -
	Outputs:    -
	Returns:    The horizontal gap between this item and another item of the same type, in
				millipoints.
	Purpose:    Called by the formatting code.  This base-class version returns a default
				value, derived classes may override it to make items closer or further
				apart.
	Errors:     -
	SeeAlso:    -
********************************************************************************************/

MILLIPOINT LineAttrItem::GetHorzGap() const
{
	return c_nHorzGapBetweenItems;
}



/********************************************************************************************
>   virtual MILLIPOINT LineAttrItem::GetVertGap() const

	Author:     Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:    17/4/95
	Inputs:     -
	Outputs:    -
	Returns:    The vertical gap between this item and another item of the same type, in
				millipoints.
	Purpose:    Called by the formatting code.  This base-class version returns a default
				value, derived classes may override it to make items closer or further
				apart.
	Errors:     -
	SeeAlso:    -
********************************************************************************************/

MILLIPOINT LineAttrItem::GetVertGap() const
{
	return c_nVertGapBetweenItems;
}



/***********************************************************************************************
>   virtual BOOL LineAttrItem::HandleEvent(SGEventType EventType,
													void* EventInfo,
													SGMiscInfo* pMiscInfo)

	Author:     Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:    15/2/95 (base generated in sgbase.cpp)
	Inputs:     EventType - An enumerated value describing what type of event is to be processed
				EventInfo - A structure describing the event (may be NULL). The exact thing
							pointed at by this pointer depends upon the event type:
							MonoOn
							Event               Thing EventInfo points at
							SGEVENT_FORMAT      (SGFormatInfo *)
							SGEVENT_REDRAW      (SGRedrawInfo *)
							SGEVENT_MOUSECLICK  (SGMouseInfo *)
							MonoOff
				Use the provided SGDisplayNode::Get[Format]Info() inlines to retrieve this
				information - they provide useful error/type checking, and hide the cast

				MiscInfo - always provided. Contains a few useful bits of info that may be
				needed for all event types.
	Outputs:    FormatInfo is updated as appropriate
	Returns:    TRUE if the event was handled successfully
				FALSE if it was not
	Purpose:    Handles a SuperGallery DisplayTree event
	Notes:      This overrides the pure virtual SGDisplayNode::HandleEvent method.
				A node need not handle a specific event - if it does not handle it, it
				should return FALSE.
				Redraw and Formatting handlers should never return TRUE, as this will
				prevent the event from continuing through the tree.
				Non-leaf-nodes must call SGDisplayNode::GiveEventToMyChildren in order
				to pass the event dow the tree. THIS node is a leaf-node, so it doesn't.
	SeeAlso:    SGDisplayNode::HandleEvent
***********************************************************************************************/

BOOL LineAttrItem::HandleEvent(SGEventType EventType, void* EventInfo, SGMiscInfo *MiscInfo)
{
	switch (EventType)
	{
	case SGEVENT_FORMAT:
		{
			SGFormatInfo* FormatInfo = GetFormatInfo(EventType, EventInfo);
			CalculateMyRect(FormatInfo, MiscInfo);      // Cache our FormatRect for later use
		}
		break;


	case SGEVENT_REDRAW:
		{
			// Rely on FormatRect being cached from above
			SGRedrawInfo* RedrawInfo = GetRedrawInfo(EventType, EventInfo);

			// only redraw if we intersect the clip rect
			if (IMustRedraw(RedrawInfo))
			{
				// Now render the item.
				StartRendering(RedrawInfo, MiscInfo);
				RedrawInfo->Renderer->SaveContext();
				HandleRedraw(RedrawInfo, MiscInfo);
				RedrawInfo->Renderer->RestoreContext();
				StopRendering(RedrawInfo, MiscInfo);
			}
		}
		break;      // exit and return FALSE to pass the redraw event on


	case SGEVENT_MOUSECLICK:
		{
			SGMouseInfo *Mouse = GetMouseInfo(EventType, EventInfo);

			if (FormatRect.ContainsCoord(Mouse->Position))
			{
				//OK temporary dodge (see, not a bodge) to stop people doing things with brushes in the
				//line gallery before we get them working!
				LineAttrItem* pItem = this;
	
				{
					// If the colour is in the selected document, then it is safe to
					// do a colour drag - for now, it will only allow drags for the
					// selected doc.
					// Otherwise, the normal click action takes place.
					// If the drag fails (turns into a click) then the normal click action
					// takes place, passed on by the GalleryColourDragInfo::OnClick handler
					SGDisplayGroup *Parent = (SGDisplayGroup *) GetParent();

					if (Mouse->DoubleClick) // || Parent->GetParentDocument() != Document::GetSelected())
						DefaultClickHandler(Mouse, MiscInfo);
					else
					{
						DefaultPreDragHandler(Mouse, MiscInfo);	
	
						GalleryLineDragInfo* pInfo;
						pInfo = new GalleryLineDragInfo(this, Mouse, MiscInfo, Mouse->MenuClick);
						if (pInfo != NULL) DragManagerOp::StartDrag(pInfo);
					}
				}

				return(TRUE);		// Claim this event - nobody else can own this click
			}
		}
		break;

	default:
		// Pass unknown events on to the base class
		return(SGDisplayItem::HandleEvent(EventType, EventInfo, MiscInfo));
	}

	// Default return value: We do not claim this event, so it will be passed on to others
	return FALSE;
}



/***********************************************************************************************
>   virtual void LineAttrItem::DragWasReallyAClick(SGMouseInfo* pMouse,
															SGMiscInfo* pMiscInfo)
	Author:     Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:    10/4/95
	Inputs:     Mouse - The mouse info passed to the original click handler
				MiscInfo - The misc info passed to the original click handler
	Purpose:    Handles a mouse click event. This is a callback function - drags of
				line attributes from the gallery will call this function back if the drag
				turns out to just be a click.
	SeeAlso:    LineAttrItem::HandleEvent; GalleryLineDragInfo::OnClick
***********************************************************************************************/

void LineAttrItem::DragWasReallyAClick(SGMouseInfo* pMouse, SGMiscInfo* pMiscInfo)
{
	// Just get default selection action to be applied for this click.
	DefaultClickHandler(pMouse, pMiscInfo);
}



/********************************************************************************************
>   virtual BOOL LineAttrItem::DefaultClickHandler(SGMouseInfo* pMouse,
												   SGMiscInfo* pMiscInfo)
	Author:     Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:    7/4/95
	Inputs:     (see base-class implementation in SGDisplayItem)
	Outputs:    -
	Returns:    (see base-class implementation in SGDisplayItem)
	Purpose:    Overrides the default behaviour when items in the line gallery are
				clicked on, so that it is possible to simple-click and select items
				in different groups without clearing the selection.
	Errors:     -
	SeeAlso:    SGDisplayItem::DefaultClickHandler
********************************************************************************************/

BOOL LineAttrItem::DefaultClickHandler(SGMouseInfo* pMouse, SGMiscInfo* pMiscInfo)
{
	// Get some objects.
	SuperGallery* pParent = GetParentGallery();
	SGDisplayGroup* pGroup = (SGDisplayGroup*) GetParent();
	ERROR3IF(pGroup == NULL, "No parent group in LineAttrItem::DefaultClickHandler?");
	
	// If there is any other item within the group already selected then
	// deselect it.  We start searching from the first child of this group.
	for (LineAttrItem* pItem = (LineAttrItem*) pGroup->GetChild();
		 pItem != NULL;
		 pItem = (LineAttrItem*) pItem->GetNext())
	{
		 if (pItem != this)	pItem->SetSelected(FALSE);
	}

	// If Adjust, toggle my selection state
	// If Select, set myself selected, and ensure all other items deselected
	// NB. NOTE THAT "ADJUST" IS REALLY WHAT EVERY ONE ELSE CALLS "CONSTRAIN"!!
	// (Well, actually, it's what Windows uses for altering a selection... it's not my
	// fault that the rest of Camelot flies in the face of windows conventions... and
	// as galleries are really "super" windows list boxes, they have to use windows
	// conventions. Note also that pMouse->Adjust is not necessarily fixed to any key
	// but happens under Windows to be attached to ctrl or the right button)
	SetSelected((pMouse->Adjust && !pMouse->DoubleClick) ? !Flags.Selected : TRUE);

	// OK, now we have guaranteed that an item is selected on a double click we can run
	// an Apply action.
	if (pMouse->DoubleClick)
	{
		// Apply the attributes.     
		if (pParent->ApplyAction(pMouse->Adjust ? SGACTION_APPLYADJUST : SGACTION_APPLY) &&
			pMouse->Adjust)
		{
			// Adjust/Ctrl double click of an item. This applies the item and then
			// auto closes the gallery (just like RISC OS and Win95)
			// Auto-close only occurs, however, if the item says it's OK, as arrowheads
			// use the ctrl-double click to apply in a special way, so shouldn't autoclose.

			if (ShouldCloseOnDoubleClick())
			{
				pParent->SetVisibility(FALSE);

				pParent->SetSystemStateChanged();	// Ensure toolbar button pops out again
			}
		}

		return TRUE;
	}

	// Update the pParent to know that we are the new multi-selection anchor
	// (The anchor only changes when single items are (de)selected)
	pParent->SetLastSelectedNode((Flags.Selected) ? this : NULL);

	// Finally, inform the parent gallery that the selection has changed, so it can
	// shade/unshade buttons as appropriate, etc
	pParent->SelectionHasChanged();
	return TRUE;
}



/********************************************************************************************
>   const String& LineAttrItem::GetDescription() const

	Author:     Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:    28/3/95
	Inputs:     -
	Outputs:    -
	Returns:    A reference to a string that describes what this item represents.
	Purpose:    Describes this item, basically.
	Errors:     -
	SeeAlso:    LineAttrItem::LineAttrItem
********************************************************************************************/

const String& LineAttrItem::GetDescription() const
{
	return m_strDescription;
}

/********************************************************************************************
>   void LineAttrItem::SetDescription(StringBase *pNewDescription)

	Author:     Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:    06/03/97
	Inputs:     pNewDescription	- New description of the item
	Purpose:    Changes the item's text in the gallery
********************************************************************************************/

void LineAttrItem::SetDescription(StringBase *pNewDescription)
{
	if(pNewDescription != NULL)
	{
		m_strDescription = *pNewDescription;
	}
}



/********************************************************************************************
>   void LineAttrItem::SetTextPosition(LineAttrItem::TextPosition eNewTextPos)

	Author:     Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:    29/3/95
	Inputs:     eNewTextPos         the new position of the text description
	Outputs:    -
	Returns:    -
	Purpose:    Tells SGDisplayLineAttrItems whether to display themselves with a label
				describing themselves, or not.
	Errors:     -
	SeeAlso:    -
********************************************************************************************/

void LineAttrItem::SetTextPosition(LineAttrItem::TextPosition eNewTextPos)
{
	m_eDefaultTextPosition = eNewTextPos;
}



/********************************************************************************************
>	virtual BOOL LineAttrItem::GetStatusLineHelp(DocCoord* pMousePos, String_256* pResult)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/6/95
	Inputs:		dcMousePos				(not used)
	Outputs:	pResult					where to put the text
	Returns:	TRUE
	Purpose:	Sets the given string to the text description of this line item.
********************************************************************************************/

BOOL LineAttrItem::GetStatusLineHelp(DocCoord*, String_256* pResult)
{
	// Check for rubbish.
	ERROR3IF(pResult == NULL, "Null String_256* passed to LineAttrItem::GetStatusLineHelp");
	
	// Build up the status-line text.  This is composed of the item's text description,
	// followed by the text of the item's group title, without the last pluralising 's'.
	String_256 str;
	if (IsSelected())
		str = String_256(_R(IDS_LINEGAL_SEL_CTRLCLICK));
	else
		str = String_256(_R(IDS_LINEGAL_SEL_CLICK));

	str += GetDescription();
	str += String_8(_R(IDS_SGLINE_STAT_SPACE_SEP)); // " "
	str += ((LineAttrGroup*) GetParent())->GetTitle();
	str.Left(pResult, str.Length() - 1);

	String_256 strAttr(_R(IDS_LINEGAL_ATTR));
	*pResult += strAttr;

//	if (IsSelected())			// removing this test fixes bug #4684 (hee hee!!)
	{
		String_256 strDbl(_R(IDS_LINEGAL_SEL_DBLCLK));
		*pResult += strDbl;
	}
	
	// Success!
	return TRUE;
}



/********************************************************************************************

>	virtual BOOL LineAttrItem::ShouldCloseOnDoubleClick(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/95

	Returns:	TRUE

	Purpose:	When an item is ctrl-double-clicked, the default action is to apply the
				item and close the gallery (for convenience). However, arrowheads do
				a special action for ctrl-double-click (they apply to the other end
				of the line) so they do not want auto-close to occur.

				This function returns TRUE (auto-close) by default, and is overridden
				to return FALSE by derived classes which wish to stop the auto-close action.

	SeeAlso:	LineArrowItem::ShouldCloseOnDoubleClick

********************************************************************************************/

BOOL LineAttrItem::ShouldCloseOnDoubleClick(void)
{
	return(TRUE);
}



/********************************************************************************************
>	virtual void LineAttrItem::ItemSelected() 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/1/2000
	Inputs:		pNewAttr - the attribute that we are about to apply
	Outputs:	-
	Returns:	TRUE if we want the line gallery to go ahead and apply the attribute, 
				FALSE if we applied it ourselves.
	Purpose:	utility fn called when the item is selected.  If your item needs to do anything
				just prior to it becoming selected then override this function.
	Errors:		-
	SeeAlso:    BrushAttrItem::ItemSelected
********************************************************************************************/

BOOL LineAttrItem::ItemSelected(NodeAttribute* pNewAttr)
{
	return TRUE;
}


/********************************************************************************************
>   LineAttrGroup::LineAttrGroup(const String_64& strTitle, SuperGallery* pGal)

	Author:     Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:    28/3/95
	Inputs:     strTitle        the title string to display within the group heading
				pGal            pointer to the owning super-gallery object
	Outputs:    -
	Returns:    -
	Purpose:    Contructs a LineAttrGroup, representing a section heading within
				the line-attributes super-gallery.
	Errors:     -
	SeeAlso:    SGDisplayGroup::SGDisplayGroup
********************************************************************************************/

LineAttrGroup::LineAttrGroup(const String_64& strTitle, SuperGallery* pGal)
  : SGDisplayGroup(pGal, NULL, NULL)
{
	// Set the title (in the base class).
	TitleText = strTitle;
}



/********************************************************************************************
>	const String_64& LineAttrGroup::GetTitle() const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/6/95
	Returns:	A reference to the string containing this group's title.
	Purpose:	Gets this group's title (the text displayed next to the folder icon).
	SeeAlso:	LineAttrItem::GetStatusLineHelp
********************************************************************************************/

const String_64& LineAttrGroup::GetTitle() const
{
	return TitleText;
}



/********************************************************************************************
>   static BOOL LineGallery::Init()

	Author:     Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:    28/3/95
	Inputs:     -
	Outputs:    -
	Returns:    TRUE if gallery initialisation is successful.
	Purpose:    Called when the program starts, loading .INI file settings.
	Errors:     -
	SeeAlso:    -
********************************************************************************************/

BOOL LineGallery::Init()
{
	return Camelot.DeclareSection(TEXT("Displays"), 20)
		&& Camelot.DeclarePref(TEXT("Displays"), TEXT("ArrowScale1"),
							   &m_anArrowScales[0], 0.1, 50.0)
		&& Camelot.DeclarePref(TEXT("Displays"), TEXT("ArrowScale2"),
							   &m_anArrowScales[1], 0.1, 50.0)
		&& Camelot.DeclarePref(TEXT("Displays"), TEXT("ArrowScale3"),
							   &m_anArrowScales[2], 0.1, 50.0)
		&& Camelot.DeclarePref(TEXT("Displays"), TEXT("ArrowScale4"),
							   &m_anArrowScales[3], 0.1, 50.0)
		&& PrepareBrushFolders();
}



/********************************************************************************************
>   static LineGallery* LineGallery::GetInstance()

	Author:     Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:    17/4/95
	Inputs:     -
	Outputs:    -
	Returns:    A pointer to the program's line gallery if it exists, or NULL.
	Purpose:    Public access function to the line gallery.
	Errors:     -
	SeeAlso:    -
********************************************************************************************/

LineGallery* LineGallery::GetInstance()
{
	return m_pTheGallery;
}



/********************************************************************************************
>   static BOOL LineGallery::IsActive()

	Author:     Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:    17/4/95
	Inputs:     -
	Outputs:    -
	Returns:    TRUE if the line gallery is currently visible on-screen, FALSE otherwise.
	Purpose:    Used by the message-handlers of gadgets within the line gallery to
				determine if they should update themselves or not.
	Errors:     -
	SeeAlso:    -
********************************************************************************************/

BOOL LineGallery::IsActive()
{
	return m_pTheGallery != NULL && m_pTheGallery->IsVisible();
}



/********************************************************************************************
>   LineGallery::LineGallery()
												 
	Author:     Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:    15/2/95 (base generated in sgbase.cpp)
	Purpose:    LineGallery default constructor.  Sets a global instance pointer to
				this object.
********************************************************************************************/

LineGallery::LineGallery()
{
	// Remember a pointer to the global instance of this gallery.
	ERROR3IF(m_pTheGallery != NULL, "Gallery already exists in LineGallery::LineGallery?");
	m_pTheGallery = this;
	
	// set to the normal default
	m_PreviousLineWidth = 500;
} 



/********************************************************************************************
>   virtual LineGallery::~LineGallery()

	Author:     Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:    17/4/95
	Inputs:     -
	Outputs:    -
	Returns:    -
	Purpose:    Destroys a line gallery, resetting the global instance pointer to NULL.
	Errors:     -
	SeeAlso:    -
********************************************************************************************/

LineGallery::~LineGallery()
{
	// Make sure nothing is seriously screwed-up.
	ERROR3IF(m_pTheGallery == NULL, "No gallery in LineGallery::~LineGallery?");
	m_pTheGallery = NULL;

	// delete the string array
	if (m_pFileNameArray != NULL)
	{	
		for (INT32 i = 0; i < m_NumDefaultFiles; i++)
			m_pFileNameArray[i].Empty();

		// don't forget that we allocated it with malloc..
		free((void*)m_pFileNameArray);
		
		m_pFileNameArray = NULL;
	}

}



/********************************************************************************************
>   virtual BOOL LineGallery::PreCreate()

	Author:     Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:    15/2/95 (base generated in sgbase.cpp)
	Returns:    TRUE if the Gallery initialised successfully
				FALSE if it should not be opened due to a failure to initialise
	Purpose:    The LineGallery PreCreate handler. This overrides the base class
				PreCreate function. It is called at the very beginning of the
				SuperGallery::Create method, before the window has been created.
********************************************************************************************/

BOOL LineGallery::PreCreate()
{
	// If there isn't already one, create a DisplayTree
	if (DisplayTree != NULL) return TRUE;

	// New root node, with a scrollbar.
	DisplayTree = new SGDisplayRootScroll(this);
	ERRORIF(DisplayTree == NULL, _R(IDE_NOMORE_MEMORY), FALSE);

	// Try to create a title sub-tree for each kind of line attribute.
	BOOL ok = 
			CreateBrushGroups() &&
#ifdef VECTOR_STROKING
			//CreateStrokeTypeGroup() && 
			CreateVarWidthGroup() &&
#endif // VECTOR_STROKING
		//	CreateWidthGroup() &&
			CreateDashGroup() &&
			CreateArrowGroup();
		//	CreateCapGroup() &&
		//	CreateJoinTypeGroup();
	return ok;
}



/********************************************************************************************
>   virtual BOOL LineGallery::PostCreate()

	Author:     Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:    17/4/95
	Inputs:     -
	Outputs:    -
	Returns:    Whatever the base class SuperGallery returns for this function.
	Purpose:    Called after the LineGallery is created.  Forces an update of the "current
				attribute" status of all items.
	Errors:     -
	SeeAlso:    LineAttrItem::UpdateCurrentAttribStatus
********************************************************************************************/

BOOL LineGallery::PostCreate()
{
	// Prime the current attribute status flags of each item.
	return SuperGallery::PostCreate() && LineAttrItem::UpdateCurrentAttribStatus();
}



/********************************************************************************************
>   LineAttrGroup* LineGallery::CreateGroup(const String_256& strTitle, BOOL fOpen)

	Author:     Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:    28/3/95
	Inputs:     strTitle            the title (heading) of the group to create
				fOpen               whether the group "folder" is initially "open"
	Outputs:    -
	Returns:    A pointer to the newly created group, or NULL if it can't be created.
	Purpose:    Creates a new group (heading) within the line-attributes super-gallery.
	Errors:     -
	SeeAlso:    LineAttrGroup::LineAttrGroup
********************************************************************************************/

LineAttrGroup* LineGallery::CreateGroup(const String_256& strTitle, BOOL fOpen)
{
	// Make sure the tree has been initialised.
	ERROR3IF(DisplayTree == NULL, "Root node not allocated in LineGallery::CreateGroup");
	
	// Try to create and set a new group.
	LineAttrGroup* pGroup = new LineAttrGroup(strTitle, this);
	if (pGroup != NULL)
	{
		// NB. note that the "folded" flag is inverted.
//		pGroup->Flags.Folded = (fOpen) ? FALSE : TRUE;
		pGroup->Flags.Folded = true;
		pGroup->Flags.Invisible = false;
		pGroup->Flags.CanSelect = true;

		DisplayTree->AddItem(pGroup);
	}
	
	// Return a pointer to the new group.
	return pGroup;
}



/********************************************************************************************
>   BOOL LineGallery::CreateItem(LineAttrGroup* pGroup, LineAttrItem* pItem)
	Author:     Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:    29/3/95
	Inputs:     pGroup          pointer to the group to add the item to
				pItem           pointer to the item to add to the group
	Outputs:    -
	Returns:    TRUE if the item is successfully added, FALSE if either pGroup or pItem
				is a null pointer.
	Purpose:    Helper function to add items to a line-attribute grouping.
	Errors:     -
	SeeAlso:    LineGallery::CreateGroup
********************************************************************************************/

BOOL LineGallery::CreateItem(LineAttrGroup* pGroup, LineAttrItem* pItem)
{
	// If we were passed a genuine item insert it into the group.
	if (pGroup != NULL && pItem != NULL)
	{
		// Success.
		pGroup->AddItem(pItem);
		return TRUE;
	}
	
	// You failed to allocate your item or group!
	return FALSE;
}



/********************************************************************************************
>   BOOL LineGallery::CreateWidthGroup()

	Author:     Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:    28/3/95
	Inputs:     -
	Outputs:    -
	Returns:    TRUE if successful.
	Purpose:    Creates a new "line width" Group with its associated items.
	Errors:     -
	SeeAlso:    LineGallery::CreateGroup; LineGallery::CreateItem;
				LineWidthItem::LineWidthItem
********************************************************************************************/

BOOL LineGallery::CreateWidthGroup()
{
	// Create the group (heading).
	LineAttrGroup* pGroup = CreateGroup(String_256(_R(IDS_LINEGAL_CREATELINE)), TRUE);
	if (pGroup != NULL)
		pGroup->Flags.Folded = TRUE;

	// Create each item.
	return CreateItem(pGroup, new LineWidthItem(0.25, String_64(_R(IDS_LINEGAL_025PT))))
		&& CreateItem(pGroup, new LineWidthItem(0.5,  String_64(_R(IDS_LINEGAL_05PT))))
		&& CreateItem(pGroup, new LineWidthItem(1.0,  String_64(_R(IDS_LINEGAL_1PT))))
		&& CreateItem(pGroup, new LineWidthItem(2.0,  String_64(_R(IDS_LINEGAL_2PT))))
		&& CreateItem(pGroup, new LineWidthItem(4.0,  String_64(_R(IDS_LINEGAL_4PT))))
		&& CreateItem(pGroup, new LineWidthItem(8.0,  String_64(_R(IDS_LINEGAL_8PT))))
		&& CreateItem(pGroup, new LineWidthItem(12.0, String_64(_R(IDS_LINEGAL_12PT))))
		&& CreateItem(pGroup, new LineWidthItem(16.0, String_64(_R(IDS_LINEGAL_16PT))));
}



/********************************************************************************************
>   BOOL LineGallery::CreateDashGroup()

	Author:     Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:    28/3/95
	Inputs:     -
	Outputs:    -
	Returns:    TRUE if successful.
	Purpose:    Creates a new "dash pattern" Group with all its associated items.
	Errors:     -
	SeeAlso:    LineGallery::CreateGroup; LineGallery::CreateItem;
				LineDashItem::LineDashItem
********************************************************************************************/

BOOL LineGallery::CreateDashGroup()
{
	// Create the group (heading).
	LineAttrGroup* pGroup = CreateGroup(String_256(_R(IDS_LINEGAL_CREATEDASH)), TRUE);
	if (pGroup != NULL)
		pGroup->Flags.Folded = TRUE;

	// Create each item.
	INT32 n = (INT32) DashRec::GetNumStockDashes();
	for (INT32 i = (INT32) SD_SOLID; i < n; i++)
	{
		// If we fail then quit now.
		String_256 str = DashRec::GetStockDashName((StockDash) i);
		if (!CreateItem(pGroup, new LineDashItem((StockDash) i, str))) return FALSE;
	}

	// Success!
	return TRUE;
}



/********************************************************************************************
>   BOOL LineGallery::CreateArrowGroup()

	Author:     Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:    28/3/95
	Inputs:     -
	Outputs:    -
	Returns:    TRUE if successful.
	Purpose:    Creates a new "arrowhead/tail" Group with its associated items.
	Errors:     -
	SeeAlso:    LineGallery::CreateGroup; LineGallery::CreateItem;
				LineStartArrowItem::LineStartArrowItem
********************************************************************************************/

BOOL LineGallery::CreateArrowGroup()
{
	// Create the group.
	LineAttrGroup* pGroup = CreateGroup(String_256(_R(IDS_LINEGAL_CREATEARROW)), TRUE);
	if (pGroup != NULL)
		pGroup->Flags.Folded = TRUE;

	// Create each item.  First the "no-arrowhead" item.
	if (!CreateItem(pGroup, new LineNoEndArrowItem(String_64(_R(IDS_LINEGAL_NOHEAD)))) ||
		!CreateItem(pGroup, new LineNoStartArrowItem(String_64(_R(IDS_LINEGAL_NOTAIL)))))
	{
		return FALSE;
	}

	// Now do the real ones.
	INT32 nTypes = (INT32) ArrowRec::GetNumStockArrows();
	for (INT32 i = (INT32) SA_STRAIGHTARROW; i < nTypes; i++)
	{
		for (INT32 j = 0; j < 4; j++)
		{		
			// Get the text description for this kind of arrowhead.
			String_256 str = ArrowRec::GetStockArrowName((StockArrow) i);
			LineArrowItem* pItem = new LineArrowItem((StockArrow) i, m_anArrowScales[j], str);
			if (!CreateItem(pGroup, pItem)) return FALSE;
		}
	}

	// Success!
	return TRUE;
}



/********************************************************************************************
>   BOOL LineGallery::CreateCapGroup()

	Author:     Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:    28/3/95
	Inputs:     -
	Outputs:    -
	Returns:    TRUE if successful.
	Purpose:    Creates a new "line cap" Group with its associated items.
	Errors:     -
	SeeAlso:    LineGallery::CreateGroup; LineGallery::CreateItem;
				LineCapItem::LineCapItem
********************************************************************************************/

BOOL LineGallery::CreateCapGroup()
{
	// Create the group.
	LineAttrGroup* pGroup = CreateGroup(String_256(_R(IDS_LINEGAL_CREATECAP)), TRUE);
	if (pGroup != NULL)
		pGroup->Flags.Folded = TRUE;

	// Create each item.
	return CreateItem(pGroup, new LineCapItem(LineCapButt,  String_64(_R(IDS_LINEGAL_BUTT))))
		&& CreateItem(pGroup, new LineCapItem(LineCapRound, String_64(_R(IDS_LINEGAL_ROUND))))
		&& CreateItem(pGroup, new LineCapItem(LineCapButt,  String_64(_R(IDS_LINEGAL_SQUARE))));
}



/********************************************************************************************
>   BOOL LineGallery::CreateJoinTypeGroup()

	Author:     Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:    28/3/95
	Inputs:     -
	Outputs:    -
	Returns:    TRUE if successful.
	Purpose:    Creates a new "join type" Group with its associated items.
	Errors:     -
	SeeAlso:    LineGallery::CreateGroup; LineGallery::CreateItem;
				LineJoinTypeItem::LineJoinTypeItem
********************************************************************************************/

BOOL LineGallery::CreateJoinTypeGroup()
{
	// Create the group.
	LineAttrGroup* pGroup = CreateGroup(String_256(_R(IDS_LINEGAL_CREATEJOIN)), TRUE);
	if (pGroup != NULL)
		pGroup->Flags.Folded = TRUE;

	// Create each item.
	return CreateItem(pGroup, new LineJoinTypeItem(MitreJoin,    String_64(_R(IDS_LINEGAL_MITRE))))
		&& CreateItem(pGroup, new LineJoinTypeItem(RoundJoin,    String_64(_R(IDS_LINEGAL_ROUND))))
		&& CreateItem(pGroup, new LineJoinTypeItem(BevelledJoin, String_64(_R(IDS_LINEGAL_BEVEL))));
}



/********************************************************************************************

>   BOOL LineGallery::CreateVarWidthGroup()

	Author:     Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:    3/3/97

	Returns:    TRUE if successful.

	Purpose:    Creates a new "variable width" Group with its associated items.

	SeeAlso:    LineGallery::CreateGroup; LineGallery::CreateItem

********************************************************************************************/

BOOL LineGallery::CreateVarWidthGroup()
{
// Removed from non-stroking builds 2/10/97
#ifdef VECTOR_STROKING

	// Create the group.
	LineAttrGroup *pGroup = CreateGroup(String_256(_R(IDS_LINEGAL_GROUP_VARWIDTH)), TRUE);
	if (pGroup == NULL)
		return(FALSE);
		
	// --- Simple constant-width line ------------------------------------------
	VariableWidthAttrValue *pAttr;
	pAttr = new VariableWidthAttrValue(NULL);
	if (pAttr == NULL || !CreateItem(pGroup, new VarWidthItem(pAttr, String_64(_R(IDS_LINEGAL_VARWIDTH_CONSTANT)))))
		return(FALSE);

	// --- SmoothStroke SS_Fallout ------------------------------------------------------------
	pAttr = new VariableWidthAttrValue(new ValueFunctionSS_Fallout);
	if (pAttr == NULL || !CreateItem(pGroup, new VarWidthItem(pAttr, String_64(_R(IDS_LINEGAL_VARWIDTH_FALLOUT)))))
		return(FALSE);

	// --- SmoothStroke SS_Iron ------------------------------------------------------------
	pAttr = new VariableWidthAttrValue(new ValueFunctionSS_Iron);
	if (pAttr == NULL || !CreateItem(pGroup, new VarWidthItem(pAttr, String_64(_R(IDS_LINEGAL_VARWIDTH_IRON)))))
		return(FALSE);
		
	// --- Ellipse -------------------------------------------------------------
	pAttr = new VariableWidthAttrValue(new ValueFunctionEllipse);
	if (pAttr == NULL || !CreateItem(pGroup, new VarWidthItem(pAttr, String_64(_R(IDS_LINEGAL_VARWIDTH_ELLIPSE)))))
		return(FALSE);

	// --- SmoothStroke SS_SlimBlip ------------------------------------------------------------
	pAttr = new VariableWidthAttrValue(new ValueFunctionSS_SlimBlip);
	if (pAttr == NULL || !CreateItem(pGroup, new VarWidthItem(pAttr, String_64(_R(IDS_LINEGAL_VARWIDTH_SLIMBLIP)))))
		return(FALSE);

	// --- Blip ------------------------------------------------------------
	pAttr = new VariableWidthAttrValue(new ValueFunctionBlip);
	if (pAttr == NULL || !CreateItem(pGroup, new VarWidthItem(pAttr, String_64(_R(IDS_LINEGAL_VARWIDTH_BLIP)))))
		return(FALSE);

	// --- SmoothStroke SS_Cigar ------------------------------------------------------------
	pAttr = new VariableWidthAttrValue(new ValueFunctionSS_Cigar);
	if (pAttr == NULL || !CreateItem(pGroup, new VarWidthItem(pAttr, String_64(_R(IDS_LINEGAL_VARWIDTH_CIGAR)))))
		return(FALSE);

	// --- SmoothStroke SS_Cigar2 ------------------------------------------------------------
	pAttr = new VariableWidthAttrValue(new ValueFunctionSS_Cigar2);
	if (pAttr == NULL || !CreateItem(pGroup, new VarWidthItem(pAttr, String_64(_R(IDS_LINEGAL_VARWIDTH_CIGAR_2)))))
		return(FALSE);
	
	// --- SmoothStroke SS_Cigar3 ------------------------------------------------------------
	pAttr = new VariableWidthAttrValue(new ValueFunctionSS_Cigar3);
	if (pAttr == NULL || !CreateItem(pGroup, new VarWidthItem(pAttr, String_64(_R(IDS_LINEGAL_VARWIDTH_CIGAR_3)))))
		return(FALSE);

	// --- SmoothStroke SS_Convex ------------------------------------------------------------
	pAttr = new VariableWidthAttrValue(new ValueFunctionSS_Convex);
	if (pAttr == NULL || !CreateItem(pGroup, new VarWidthItem(pAttr, String_64(_R(IDS_LINEGAL_VARWIDTH_CONVEX)))))
		return(FALSE);

	// --- Simple S-shaped ramp with none-zero end width------------------------------------------------
	pAttr = new VariableWidthAttrValue(new ValueFunctionRampS2);
	if (pAttr == NULL || !CreateItem(pGroup, new VarWidthItem(pAttr, String_64(_R(IDS_LINEGAL_VARWIDTH_CONVEX_2)))))
		return(FALSE);

	// --- Simple S-shaped ramp ------------------------------------------------
	pAttr = new VariableWidthAttrValue(new ValueFunctionRampS);
	if (pAttr == NULL || !CreateItem(pGroup, new VarWidthItem(pAttr, String_64(_R(IDS_LINEGAL_VARWIDTH_CONVEX_3)))))
		return(FALSE);

	// --- SmoothStroke SS_Concave ------------------------------------------------------------
	pAttr = new VariableWidthAttrValue(new ValueFunctionSS_Concave);
	if (pAttr == NULL || !CreateItem(pGroup, new VarWidthItem(pAttr, String_64(_R(IDS_LINEGAL_VARWIDTH_CONCAVE)))))
		return(FALSE);
		
	// --- Simple linear ramp with non-zero end width --------------------------------------------------
	pAttr = new VariableWidthAttrValue(new ValueFunctionRampL2);
	if (pAttr == NULL || !CreateItem(pGroup, new VarWidthItem(pAttr, String_64(_R(IDS_LINEGAL_VARWIDTH_CONCAVE_2)))))
		return(FALSE);
		
	// --- Simple linear ramp --------------------------------------------------
	pAttr = new VariableWidthAttrValue(new ValueFunctionRampL);
	if (pAttr == NULL || !CreateItem(pGroup, new VarWidthItem(pAttr, String_64(_R(IDS_LINEGAL_VARWIDTH_CONCAVE_3)))))
		return(FALSE);
		
	// --- Teardrop CurvedEnd ------------------------------------------------------------
	pAttr = new VariableWidthAttrValue(new ValueFunctionTeardropCurvedEnd);
	if (pAttr == NULL || !CreateItem(pGroup, new VarWidthItem(pAttr, String_64(_R(IDS_LINEGAL_VARWIDTH_DAB)))))
		return(FALSE);

	// --- Teardrop ------------------------------------------------------------
	pAttr = new VariableWidthAttrValue(new ValueFunctionTeardrop);
	if (pAttr == NULL || !CreateItem(pGroup, new VarWidthItem(pAttr, String_64(_R(IDS_LINEGAL_VARWIDTH_RAINDROP)))))
		return(FALSE);
		
	// --- SmoothStroke SS_Comet ------------------------------------------------------------
	pAttr = new VariableWidthAttrValue(new ValueFunctionSS_Comet);
	if (pAttr == NULL || !CreateItem(pGroup, new VarWidthItem(pAttr, String_64(_R(IDS_LINEGAL_VARWIDTH_COMET)))))
		return(FALSE);
		
	// --- SmoothStroke SS_Petal ------------------------------------------------------------
	pAttr = new VariableWidthAttrValue(new ValueFunctionSS_Petal);
	if (pAttr == NULL || !CreateItem(pGroup, new VarWidthItem(pAttr, String_64(_R(IDS_LINEGAL_VARWIDTH_PETAL)))))
		return(FALSE);
		
	// --- SmoothStroke SS_Reed ------------------------------------------------------------
	pAttr = new VariableWidthAttrValue(new ValueFunctionSS_Reed);
	if (pAttr == NULL || !CreateItem(pGroup, new VarWidthItem(pAttr, String_64(_R(IDS_LINEGAL_VARWIDTH_REED)))))
		return(FALSE);
		
	// --- SmoothStroke SS_Meteor ------------------------------------------------------------
	pAttr = new VariableWidthAttrValue(new ValueFunctionSS_Meteor);
	if (pAttr == NULL || !CreateItem(pGroup, new VarWidthItem(pAttr, String_64(_R(IDS_LINEGAL_VARWIDTH_METEOR)))))
		return(FALSE);
		
	// --- SmoothStroke SS_Torpedo ------------------------------------------------------------
	pAttr = new VariableWidthAttrValue(new ValueFunctionSS_Torpedo);
	if (pAttr == NULL || !CreateItem(pGroup, new VarWidthItem(pAttr, String_64(_R(IDS_LINEGAL_VARWIDTH_TORPEDO)))))
		return(FALSE);

	// --- SmoothStroke SS_Missile ------------------------------------------------------------
	pAttr = new VariableWidthAttrValue(new ValueFunctionSS_Missile);
	if (pAttr == NULL || !CreateItem(pGroup, new VarWidthItem(pAttr, String_64(_R(IDS_LINEGAL_VARWIDTH_MISSILE)))))
		return(FALSE);

	// --- SmoothStroke SS_Goldfish ------------------------------------------------------------
	pAttr = new VariableWidthAttrValue(new ValueFunctionSS_Goldfish);
	if (pAttr == NULL || !CreateItem(pGroup, new VarWidthItem(pAttr, String_64(_R(IDS_LINEGAL_VARWIDTH_GOLDFISH)))))
		return(FALSE);

	// --- SmoothStroke SS_Yacht ------------------------------------------------------------
	pAttr = new VariableWidthAttrValue(new ValueFunctionSS_Yacht);
	if (pAttr == NULL || !CreateItem(pGroup, new VarWidthItem(pAttr, String_64(_R(IDS_LINEGAL_VARWIDTH_YACHT)))))
		return(FALSE);
	
	// --- SmoothStroke SS_Barb ------------------------------------------------------------
	pAttr = new VariableWidthAttrValue(new ValueFunctionSS_Barb);
	if (pAttr == NULL || !CreateItem(pGroup, new VarWidthItem(pAttr, String_64(_R(IDS_LINEGAL_VARWIDTH_BARB)))))
		return(FALSE);

	// --- SmoothStroke SS_Ocean Liner ------------------------------------------------------------
	pAttr = new VariableWidthAttrValue(new ValueFunctionSS_OceanLiner);
	if (pAttr == NULL || !CreateItem(pGroup, new VarWidthItem(pAttr, String_64(_R(IDS_LINEGAL_VARWIDTH_OCEAN_LINER)))))
		return(FALSE);

	// --- Propeller ------------------------------------------------------------
	pAttr = new VariableWidthAttrValue(new ValueFunctionPropeller);
	if (pAttr == NULL || !CreateItem(pGroup, new VarWidthItem(pAttr, String_64(_R(IDS_LINEGAL_VARWIDTH_PROPELLER)))))
		return(FALSE);

	// --- DoubleRampS ------------------------------------------------------------
	pAttr = new VariableWidthAttrValue(new ValueFunctionDoubleRampS);
	if (pAttr == NULL || !CreateItem(pGroup, new VarWidthItem(pAttr, String_64(_R(IDS_LINEGAL_VARWIDTH_BOW_TIE)))))
		return(FALSE);

	// --- BevelEnds ------------------------------------------------------------
	pAttr = new VariableWidthAttrValue(new ValueFunctionBevelEnds);
	if (pAttr == NULL || !CreateItem(pGroup, new VarWidthItem(pAttr, String_64(_R(IDS_LINEGAL_VARWIDTH_BEVELLED_ENDS)))))
		return(FALSE);

	// --- SawTooth ------------------------------------------------------------
	pAttr = new VariableWidthAttrValue(new ValueFunctionSawTooth);
	if (pAttr == NULL || !CreateItem(pGroup, new VarWidthItem(pAttr, String_64(_R(IDS_LINEGAL_VARWIDTH_SAW_TOOTH)))))
		return(FALSE);

	// --- Intestine ------------------------------------------------------------
	pAttr = new VariableWidthAttrValue(new ValueFunctionIntestine);
	if (pAttr == NULL || !CreateItem(pGroup, new VarWidthItem(pAttr, String_64(_R(IDS_LINEGAL_VARWIDTH_INTESTINE)))))
		return(FALSE);

	// --- Decay ------------------------------------------------------------
	pAttr = new VariableWidthAttrValue(new ValueFunctionDecay);
	if (pAttr == NULL || !CreateItem(pGroup, new VarWidthItem(pAttr, String_64(_R(IDS_LINEGAL_VARWIDTH_DECAY)))))
		return(FALSE);

#endif // VECTOR_STROKING

	return(TRUE);
}



/********************************************************************************************

>   BOOL LineGallery::CreateStrokeTypeGroup()

	Author:     Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:    3/3/97

	Returns:    TRUE if successful.

	Purpose:    Creates a new "Stroke type" Group with its associated items.

	SeeAlso:    LineGallery::CreateGroup; LineGallery::CreateItem

********************************************************************************************/

BOOL LineGallery::CreateStrokeTypeGroup()
{
// Removed from non-stroking builds 2/10/97
#ifdef VECTOR_STROKING

	// Create the group.
	LineAttrGroup *pGroup = CreateGroup(String_256(_R(IDS_LINEGAL_GROUP_STROKETYPE)), TRUE);
	if (pGroup == NULL)
		return(FALSE);

	// Create each item.
	StrokeTypeAttrValue *pAttr;

	// --- Simple old-style line -----------------------------------------------
	pAttr = new StrokeTypeAttrValue(NULL);
	if (pAttr == NULL || !CreateItem(pGroup, new StrokeTypeItem(pAttr, String_64(_R(IDS_LINEGAL_STROKETYPE_LINE)))))
		return(FALSE);

	// --- Simple flat-filled stroke --------------------------------------------
	pAttr = new StrokeTypeAttrValue(new PathProcessorStroke);
	if (pAttr == NULL || !CreateItem(pGroup, new StrokeTypeItem(pAttr, String_64(_R(IDS_LINEGAL_STROKETYPE_STROKE)))))
		return(FALSE);
	/*
	// --- Airbrush (L) ---------------------------------------------------------
	PathProcessorStrokeAirbrush	*pAirbrush = new PathProcessorStrokeAirbrush;
	if (pAirbrush == NULL)
		return(FALSE);

	pAirbrush->SetIntensityFunction(new ValueFunctionRampL);
	pAttr = new StrokeTypeAttrValue(pAirbrush);
	if (pAttr == NULL || !CreateItem(pGroup, new StrokeTypeItem(pAttr, String_64(TEXT("Thin airbrush")))))
		return(FALSE);

	// --- Airbrush (S) ---------------------------------------------------------
	pAirbrush = new PathProcessorStrokeAirbrush;
	if (pAirbrush == NULL)
		return(FALSE);

	pAirbrush->SetIntensityFunction(new ValueFunctionRampS);
	pAttr = new StrokeTypeAttrValue(pAirbrush);
	if (pAttr == NULL || !CreateItem(pGroup, new StrokeTypeItem(pAttr, String_64(TEXT("Thick airbrush")))))
		return(FALSE);
	*/
#endif // VECTOR_STROKING

	return(TRUE);
}


/********************************************************************************************

>   BOOL LineGallery::CreateBrushGroups()

	Author:     Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:    9/1/2000

	Returns:    TRUE if successful.

	Purpose:    Creates groups to hold brushes in the line gallery.  Basically we create folders
				for each of the default files that are loaded during start-up, plus one
				extra folder where all subsequent brushes will be put.  Note that PrepareBrushFolders
				must have been called at some point prior to this.

	SeeAlso:    LineGallery::CreateGroup; LineGallery::CreateItem

********************************************************************************************/

BOOL LineGallery::CreateBrushGroups()
{
	// Create the default folder, which will contain the default stroke
	LineAttrGroup *pGroup = CreateGroup(String_256(_R(IDS_BRUSHGROUP_DEFAULT)), TRUE);
	if (pGroup == NULL)
		return(NULL);

	// Create a default 'no brush' brush
	BrushAttrValue *pAttr;

	// --- Simple old-style line -----------------------------------------------
	pAttr = new BrushAttrValue;
	if (pAttr == NULL || !CreateItem(pGroup, new BrushAttrItem(pAttr, String_64(_R(IDS_BRUSHITEM_DEFAULT)))))
		return(FALSE);
	
	//For this release, force the brush group to be invisible and non-selectable
//	pGroup->Flags.Folded = true;
//	pGroup->Flags.Invisible = true;
//	pGroup->Flags.CanSelect = false;
	

	// make groups for all the filenames we stored
	if (m_pFileNameArray != NULL)
	{
		UINT32 Counter = 0;
		String_256 GroupName;
		pGroup = NULL;
	
		while (Counter < m_NumDefaultFiles)
		{
			GroupName = m_pFileNameArray[Counter++];

			// create the group
			pGroup = CreateGroup(GroupName, TRUE);
			if (pGroup == NULL)
				return(FALSE);

			//For this release, force the brush group to be invisible and non-selectable
//			pGroup->Flags.Folded = true;
//			pGroup->Flags.Invisible = true;
//			pGroup->Flags.CanSelect = false;
		}
	}

	return TRUE;
}

/********************************************************************************************
>   virtual BOOL LineGallery::ApplyAction(SGActionType Action)

	Author:     Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:    15/2/95 (base generated in sgbase.cpp)
	Inputs:     Action - Indicates what action to apply

	Returns:	TRUE to indicate successful handling of the action, or
				FALSE to indicate failure (NOTE that this is not necessarily an error, but
				may simply be that we don't support the given action. e.g. if we don't
				handle APPLYADJUST, we'll be called again with a normal APPLY call)

	Purpose:    Applies certain conventional gallery actions (usually associated with
				gallery buttons, for new, edit, delete, etc)
	SeeAlso:    SGActionType
********************************************************************************************/

BOOL LineGallery::ApplyAction(SGActionType Action)
{
	// No display tree? Better forget about it then!
	if (DisplayTree == NULL) return FALSE;

	// Now, process the action  TO DO! - see Colour gallery for examples
	switch (Action)
	{
		case SGACTION_APPLY:
			return OnApplyButton(FALSE);

		case SGACTION_APPLYADJUST:
			return OnApplyButton(TRUE);

		case SGACTION_SETLINEPROPERTIES:
			return TRUE;

		case SGACTION_SETOPTIONS:
		case SGACTION_CREATE:
		case SGACTION_REDEFINE:
		case SGACTION_EDIT:
		case SGACTION_DELETE:
		case SGACTION_DISPLAYMODECHANGED:
			TRACEUSER( "JustinF", _T("Unimplemented feature executed in Line gallery\n"));
			// Drop through to default handler (return FALSE)

		default:
			ERROR3("Unexpected action in LineGallery::ApplyAction");
			return FALSE;
	}

	// Successful processing.
	return TRUE;
}



/********************************************************************************************
>   virtual MsgResult LineGallery::Message(Msg* Message)

	Author:     Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:    15/2/95 (base generated in sgbase.cpp)
	Inputs:     Message - The message to handle
	Purpose:    A standard message handler, really.
	Notes:      Any messages that this does not handle must be passed down to the
				SuperGallery base class message handler.
				NOTE WELL that the SuperGallery base class handler does some funky things
				for us - see SuperGallery::Message - such as deleting our display subtree
				for any document which dies (which, uncannily, would explain why they go
				away like that when you close documents ;-), and shading the gallery when
				there are no documents present. [To override this behaviour in these cases,
				you should respond to the message, and return OK rather than calling the
				base class message handler]
	SeeAlso:    SuperGallery::Message
********************************************************************************************/

MsgResult LineGallery::Message(Msg* pMessage)
{
	// Do we need to update our display of the current attributes.
	if (MESSAGE_IS_A(pMessage, CurrentAttrChangedMsg))
	{
		LineAttrItem::UpdateCurrentAttribStatus();
	}

	// A dialogue box message?
	else if (IS_OUR_DIALOG_MSG(pMessage))
	{
		DialogMsg* pMsg = (DialogMsg*) pMessage;
		switch (pMsg->DlgMsg)
		{
			// Gallery being created.
			case DIM_CREATE:
				SGInit::UpdateGalleryButton(OPTOKEN_DISPLAY_LINE_GALLERY, TRUE);
				break;

			// Gallery being closed.
			case DIM_CANCEL:
				SGInit::UpdateGalleryButton(OPTOKEN_DISPLAY_LINE_GALLERY, FALSE);
				break;

				case DIM_LFT_BN_CLICKED:
				switch(pMsg->GadgetID)
				{
					case _R(IDC_GALLERY_HELP):		// Show help page
					{
						HelpUserTopic(_R(IDS_HELPPATH_Gallery_Line));
					}
				}
				break;

		}
	}
	
	// A document state message?
	else if (MESSAGE_IS_A(pMessage, DocChangingMsg))
	{
		DocChangingMsg* pMsg = (DocChangingMsg*) pMessage;
		if (pMsg->State == DocChangingMsg::DocState::SELCHANGED)
		{
			if (pMsg->pNewDoc != NULL)
			{
				// A new document is selected, so update our selection state.
				SuperGallery::ShadeGallery(FALSE);
				SelectionHasChanged();
			}
			else
			{
				// There is no selected doc, so grey the gallery for now.
				SuperGallery::ShadeGallery(TRUE);
			}
		}
	}
	else if (MESSAGE_IS_A(pMessage, BrushDefMsg))
	{
		UpdateBrushItem(((BrushDefMsg*)pMessage)->m_BrushEdited);
	}
	// Pass on to the base-class for any more handling.
	return SuperGallery::Message(pMessage);
}


/********************************************************************************************
>   void LineGallery::UpdateBrushItem(BrushHandle Handle)

	Author:     Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:    3/4/2000
	Inputs:     Handle - identifier for the brush that needs updating
	Outputs:    -
	Returns:    -
	Purpose:    Searches for a brush item with the same handle as Handle, and asks it to 
				update itself
	Errors:     -
	SeeAlso:    -
********************************************************************************************/

void LineGallery::UpdateBrushItem(BrushHandle Handle)
{
	// we need to make sure the groups have been created
	if (!MakeSureGroupsHaveBeenCreated())
		return;

	// Iterate over each group.
	for (LineAttrGroup* pGroup = (LineAttrGroup*) GetDisplayTree()->GetChild();
		 pGroup != NULL;
		 pGroup = (LineAttrGroup*) pGroup->GetNext())
	{
		// Iterate over each item within the group.
		LineAttrItem* pNextItem;
		
		LineAttrItem* pItem = (LineAttrItem*) pGroup->GetChild();
		
		// if its not a brush we're not interested
		if (!pItem->IsKindOf(CC_RUNTIME_CLASS(BrushAttrItem)))
			pItem = NULL;
		
		while (pItem != NULL)
		{
			// get the next item before we delete anything
			pNextItem = (LineAttrItem*) pItem->GetNext();
		
			// If the item is selected then remove it from the tree before deleting
			if (((BrushAttrItem*)pItem)->GetBrushHandle() == Handle)
				 ((BrushAttrItem*)pItem)->UpdateGalleryItem();

			pItem = pNextItem;
		}
		
	}
}

/********************************************************************************************
>   virtual BOOL LineGallery::OnSelChangingMsg(SelChangeMsg::SelectionState st)

	Author:     Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:    17/4/95
	Inputs:     st              reason code for what's happing to the selection
	Outputs:    -
	Returns:    TRUE if the message is processed successfully.
	Purpose:    Responds to the selection state changing by updating the "current attribute"
				status of the gallery's items.
	Errors:     -
	SeeAlso:    -
********************************************************************************************/
/*
BOOL LineGallery::OnSelChangingMsg(SelChangingMsg::SelectionState st)
{
	return SuperGallery::OnSelChangingMsg(st) && LineAttrItem::UpdateCurrentAttribStatus();
}
*/   

/********************************************************************************************
>   virtual BOOL LineGallery::OnCommonAttrsChangedMsg()

	Author:     Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:    09/02/95
	Returns:    TRUE if the message is processed successfully.
	Purpose:    Responds to the the common attributes changing
********************************************************************************************/

BOOL LineGallery::OnCommonAttrsChangedMsg()
{
	return SuperGallery::OnCommonAttrsChangedMsg() && LineAttrItem::UpdateCurrentAttribStatus();
}
   


/********************************************************************************************
>   virtual RenderRegion* LineGallery::CreateRenderRegion(DocRect* pSize,
														   ReDrawInfoType* pInfo)
	Author:     Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:    29/3/95
	Inputs:     pSize               how big the render region should be
				pInfo               some more information about it
	Outputs:    -
	Returns:    A pointer to a newly created GDraw render-region, or NULL if it fails.
	Purpose:    Creates a GDraw render-region for drawing in a super-gallery.  Overrides
				the default behaviour, which creates an OS render-region.
	Errors:     -
	SeeAlso:    DialogOp::CreateGRenderRegion
********************************************************************************************/

RenderRegion* LineGallery::CreateRenderRegion(DocRect* pSize, ReDrawInfoType* pInfo)
{
	// Try to create a GDraw render-region.
	RenderRegion* pRegion = CreateGRenderRegion(pSize, pInfo);
	if (pRegion != NULL)
	{
		// Try to create a display quality attribute.
		QualityAttribute* pAttr = new QualityAttribute;
		if (pAttr != NULL)
		{
			// If all that worked then turn on anti-aliasing when drawing in the gallery.
			pAttr->QualityValue = Quality::QualityMax;
			pRegion->SetQuality(pAttr, TRUE);
		}
	}
	return pRegion;
}



/********************************************************************************************
>   virtual void LineGallery::DestroyRenderRegion(RenderRegion* pRegion)

	Author:     Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:    29/3/95
	Inputs:     pRegion         pointer to the RenderRegion to destroy
	Outputs:    -
	Returns:    -
	Purpose:    Destroys the given render-region (in this case a GDraw version).
	Errors:     -
	SeeAlso:    DialogOp::DestroyGRenderRegion
********************************************************************************************/

void LineGallery::DestroyRenderRegion(RenderRegion* pRegion)
{   
	DestroyGRenderRegion(pRegion);
}



/********************************************************************************************
>   virtual void LineGallery::SelectionHasChanged()

	Author:     Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:    7/4/95
	Inputs:     -
	Outputs:    -
	Returns:    -
	Purpose:    Called by the base-class SuperGallery whenever the user clicks on an item.
				This version overrides the default bahaviour so that selecting more than
				one item does NOT grey the "Apply" button.
	Errors:     -
	SeeAlso:    -
********************************************************************************************/

void LineGallery::SelectionHasChanged()
{
	// Nothing to do if we are unavailable.
	if (GetDisplayTree() == NULL || AmShaded || !IsVisible()) return;

	// Only grey the "Apply" button when NO items are selected.
	EnableGadget(_R(IDC_GALLERY_APPLY), (GetDisplayTree()->GetSelectedItemCount() > 0));

	// Update the status text to reflect the new selection.
	SetStatusText();
}



/********************************************************************************************
>   virtual BOOL LineGallery::OnApplyButton(BOOL fIsAdjust)

	Author:     Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:    6/4/95
	Inputs:     fIsAdjust		if TRUE then modify action to "adjust" apply
	Outputs:    -
	Returns:    TRUE if the application is successful (i.e. did something)
				FALSE if we failed to do anything useful (not necessarily an error)
	Purpose:    Called by the line gallery when the user clicks the "Apply" button.
				Iterates over each group within the gallery trying to apply any selected
				attributes within the group to the current selection.
	Errors:     Out of memory.
	SeeAlso:    LineGallery::ApplyAction
********************************************************************************************/

BOOL LineGallery::OnApplyButton(BOOL fIsAdjust)
{
	BOOL ActuallyDidSomething = FALSE;

	// Iterate over each group.
	for (LineAttrGroup* pGroup = (LineAttrGroup*) GetDisplayTree()->GetChild();
		 pGroup != NULL;
		 pGroup = (LineAttrGroup*) pGroup->GetNext())
	{
		// Iterate over each item within the group.
		for (LineAttrItem* pItem = (LineAttrItem*) pGroup->GetChild();
			 pItem != NULL;
			 pItem = (LineAttrItem*) pItem->GetNext())
		{
			// If the item is selected then try to apply the attribute it represents
			// to the current selection.
			if (pItem->IsSelected())
			{
				// Ask the item to create a new attribute of the required kind.
				NodeAttribute* pNewAttr = pItem->CreateNewAttribute(fIsAdjust);
				ERRORIF(pNewAttr == NULL, _R(IDE_NOMORE_MEMORY), FALSE);
				
				// ask items if they need to do anything before they become selected
				// Note that if this returns FALSE it means we no longer want to apply
				// the attribute here
				if (pItem->ItemSelected(pNewAttr))
					AttributeManager::AttributeSelected(pNewAttr);
				
				// deselect the item
				pItem->SetSelected(FALSE);

				ActuallyDidSomething = TRUE;
			}
		}
	}

	// Success.
	return(ActuallyDidSomething);
}



/********************************************************************************************
>	void LineGallery::SetStatusText()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/4/95
	Purpose:	Works out what the status text should be from which items are selected in
				the gallery, and updates it on-screen.
	SeeAlso:	LineGallery::SelectionHasChanged
********************************************************************************************/

void LineGallery::SetStatusText()
{
/*	THIS CODE HAS BEEN COMMENTED OUT BECAUSE, ALTHOUGH IT WORKS, THE GALLERY STATUS-LINE
	CODE PROVIDES ITS OWN STATUS-LINE HELP TEXT FOR THE APPLY BUTTON.

	// Update the status line text, according to what is selected in the gallery.
	String_256 strAttribs;
	strAttribs.Empty();

	// Ask each selected item in each group to provide a description of its attribute.
	INT32 nSelected = 0;
	for (LineAttrGroup* pGroup = (LineAttrGroup*) GetDisplayTree()->GetChild();
		 pGroup != NULL;
		 pGroup = (LineAttrGroup*) pGroup->GetNext())
	{
		for (LineAttrItem* pItem = (LineAttrItem*) pGroup->GetChild();
			 pItem != NULL;
			 pItem = (LineAttrItem*) pItem->GetNext())
		{
			// Does this item have something to contribute?
			if (pItem->IsSelected())
			{
				// It does, so if it isn't the first add some punctuation.
				if (nSelected > 0) strAttribs += TEXT(", ");
				strAttribs += pItem->GetDescription();
				nSelected++;
			}
		}
	}

	// Did we find any selected attributes?
	if (nSelected > 0)
	{
		// If there's only one attribute then prepend a singular indefinite article and don't
		// pluralize the "attribute" word.  If not then vice versa.
		String strPlural = TEXT("s"); // Commented out
		if (nSelected == 1)
		{
			String_256 s = TEXT("a "); // Commented out
			strAttribs = (s += strAttribs);
			strPlural.Empty();
		}

		// Format the attribute text into a meaningful sentence and if successful set the
		// status line text with the information.
		String_256 strStatus;
		if (strStatus.MakeMsg(_R(IDS_LINEGAL_STATUS_TEXT_MASK), &strAttribs, &strPlural) != 0)
		{
			// Write the new text.
//			GetApplication()->UpdateStatusBarText(&strStatus);
			TRACEUSER( "JustinF", _T("%s\n"), (LPCTSTR) strStatus);
		}
	}
*/
}



/********************************************************************************************

>	static void LineGallery::AddNewStrokeItem(AttrStrokeType *pNewStroke);

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/3/97

	Purpose:	Finds the line gallery, and adds the given stroke type attribute
				to the end of the stroke type display.

********************************************************************************************/

void LineGallery::AddNewStrokeItem(StrokeTypeAttrValue *pNewStroke)
{
// Removed from non-stroking builds 2/10/97
#ifdef VECTOR_STROKING

	ERROR3IF(pNewStroke == NULL, "Illegal NULL param");

	LineGallery *pGallery = (LineGallery *) LineGallery::GetInstance();
	if (pGallery == NULL)
		return;

	// This will create the groups if they're not there yet
	pGallery->MakeSureGroupsHaveBeenCreated();

	SGDisplayRoot *pRoot = pGallery->GetDisplayTree();
	if (pRoot == NULL)
		return;

	LineAttrGroup *pGroup = (LineAttrGroup *) pRoot->GetChild();
	if (pGroup == NULL)
		return;

#if _DEBUG
	SGDisplayItem *pItem = (SGDisplayItem *) pGroup->GetChild();
	ERROR3IF(pItem == NULL || !pItem->IsKindOf(CC_RUNTIME_CLASS(StrokeTypeItem)),
				"LineGallery::AddNewStrokeItem - I seem to be adding to the wrong group!");
#endif

	// Find the stroke's name, if it has one
	String_64 Name(TEXT("Custom brush"));
	PathProcessorStroke *pProcessor = pNewStroke->GetPathProcessor();
	if (pProcessor != NULL && pProcessor->IsKindOf(CC_RUNTIME_CLASS(PathProcessorStrokeVector)))
	{
		StrokeHandle Handle = ((PathProcessorStrokeVector *)pProcessor)->GetStrokeDefinition();
		StrokeDefinition *pStroke = StrokeComponent::FindStroke(Handle);
		if (pStroke != NULL)
			Name = *pStroke->GetStrokeName();
	}

	StrokeTypeItem * pStrokeTypeItem = new StrokeTypeItem(pNewStroke, Name);
	if (pStrokeTypeItem == NULL)
		return;

	pGallery->CreateItem(pGroup, pStrokeTypeItem);
	pGallery->ReformatAndRedrawIfNecessary();

#endif // VECTOR_STROKING
}


/********************************************************************************************

>	static void LineGallery::AddNewItem(BrushAttrValue *pNewBrush);

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/1/2000

	Purpose:	Goes through the line gallery to find out which group to add the new brush to.
				Basically if the brushes fileID == BRUSHFILE_NONE then it belongs in the default
				folder, otherwise we find the folder name from our array.

********************************************************************************************/

void LineGallery::AddNewBrushItem(BrushAttrValue *pNewBrush)
{
	if (pNewBrush == NULL)
	{
		ERROR3("Illegal NULL param");
		return;
	}

	// find the string for title of the folder from the definition
	
	String_256 GroupString(_R(IDS_BRUSHGROUP_DEFAULT)); // initialise to default string

	BrushDefinition* pDef = pNewBrush->GetBrushDefinition();
	if (pDef == NULL)
	{
		ERROR3("No brush definition in LineGallery::AddNewBrushItem");
		return;
	}
	UINT32 FileID = pDef->GetBrushFileID();
	if (FileID != BRUSHFILE_NONE && FileID <= m_NumDefaultFiles)
	{
		if (m_pFileNameArray != NULL)
			GroupString = m_pFileNameArray[FileID];
	}
	
		
	// now search through the line gallery to find the group with that title
	LineGallery *pGallery = (LineGallery *) LineGallery::GetInstance();
	if (pGallery == NULL)
		return;

	// This will create the groups if they're not there yet
	pGallery->MakeSureGroupsHaveBeenCreated();

	SGDisplayRoot *pRoot = pGallery->GetDisplayTree();
	if (pRoot == NULL)
		return;

	LineAttrGroup *pGroup = (LineAttrGroup *) pRoot->GetChild();
	if (pGroup == NULL)
		return;

	LineAttrGroup* pCopyGroup = pGroup;
	
	// loop through until we find our group.
	String_256 NameString;
	while (pCopyGroup != NULL)
	{
		NameString = (String_256)(pCopyGroup->GetTitle());
		if (NameString.IsIdentical(GroupString))
		{
			pGroup = pCopyGroup;
			break;
			
		}
		pCopyGroup = (LineAttrGroup*)pCopyGroup->GetNext();
	}

	// Find the stroke's name, if it has one
	String_64 Name = TEXT(" ");

	String_32* pName = pDef->GetLineName();
	if (pName != NULL)
		Name  = *pName;
	
	
	BrushAttrItem* pBrushAttrItem = new BrushAttrItem(pNewBrush, Name);
	if (pBrushAttrItem == NULL)
		return;
/*
#ifdef _DEBUG
	pBrushAttrItem->CheckMemory(TRUE);
#endif
	*/
	pGallery->CreateItem(pGroup, pBrushAttrItem);

	// we must invalidate so that brushes are scaled correctly the first
	// time they are drawn
	pGallery->InvalidateCachedFormat();
	pGallery->ReformatAndRedrawIfNecessary();

}


/********************************************************************************************

>	virtual BOOL LibClipartSGallery::InitMenuCommands(void)
												 
	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/03/97

	Returns:	TRUE for success

	Purpose:	Initialises any menu commands that this gallery needs.

	Notes:		Will only create the menu commands once - further calls in the future
				will return TRUE immediately wihtout doing anything.

********************************************************************************************/

BOOL LineGallery::InitMenuCommands(void)
{
	static BOOL MenusInitialised = FALSE;

	BOOL ok = TRUE;

	if (!MenusInitialised)
	{
		// Initialise menu command Ops
		//ok = ok && InitMenuCommand((StringBase *) &SGCmd_Properties, _R(IDS_SGMENU_PROPERTIES));
		ok = ok && InitMenuCommand((StringBase *) &SGCmd_RenameBrush, _R(IDS_SGMENU_RENAMEBRUSH));
		ok = ok && InitMenuCommand((StringBase *) &SGCmd_ExportBrush, _R(IDS_SGMENU_EXPORTBRUSH));
		ok = ok && InitMenuCommand((StringBase *) &SGCmd_Delete, _R(IDS_SGMENU_DELETE));
		MenusInitialised = TRUE;
	}

	return(ok);
}



/********************************************************************************************

>	virtual BOOL LibClipartSGallery::BuildCommandMenu(GalleryContextMenu *TheMenu, SGMenuID MenuID)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/03/97

	Inputs:		TheMenu - The menu to add commands to
				MenuID  - The type of menu (over-list or from-options-button) to create

	Returns:	TRUE if it succeeded

	Purpose:	To build a menu of commands to be popped up over the gallery.

********************************************************************************************/

BOOL LineGallery::BuildCommandMenu(GalleryContextMenu *TheMenu, SGMenuID MenuID)
{
 	BOOL ok = FALSE;

	SGDisplayNode *pItem = DisplayTree->FindNextSelectedItem(NULL);


	// Only pop the properties menu up for Custom Brushes
	if(pItem != NULL && pItem->IsKindOf(CC_RUNTIME_CLASS(StrokeTypeItem)))
	{
		StrokeTypeAttrValue *pAttrType = ((StrokeTypeItem *)pItem)->GetStrokeTypeAttrValue();

		if(pAttrType != NULL)
		{
			PathProcessor *pPP = pAttrType->GetPathProcessor();
			
			if(pPP != NULL && pPP->AllowsPropertiesDialog())
			{
				ok = AddCommand(TheMenu, (StringBase *) &SGCmd_Properties);
			}
		}
	}

	// we want to launch our new edit dialog for the new style brushes, its all new baby
	if (pItem != NULL && pItem->IsKindOf(CC_RUNTIME_CLASS(BrushAttrItem)))
	{
		// make sure its not the default 'no brush'
		BrushDefinition* pDef = ((BrushAttrItem*)pItem)->GetBrushDefinition();
		if ( pDef != NULL)
		{
			ok = AddCommand(TheMenu, (StringBase *) &SGCmd_RenameBrush);
			if (ok) 
				ok = AddCommand(TheMenu, (StringBase *) &SGCmd_ExportBrush);

			// check to see that this is not a default brush before adding delete option 
			UINT32 FileID = pDef->GetBrushFileID();
			if (FileID == BRUSHFILE_NONE && ok)
				 ok = AddCommand(TheMenu, (StringBase *) &SGCmd_Delete);
		}
	}

	return(ok);
}



/********************************************************************************************

>	virtual OpState LibClipartSGallery::GetCommandState(StringBase *CommandID, String_256 *ShadeReason)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/03/97

	Inputs:		CommandID - TheString ID of the command
	Outputs:	ShadeReason - If you return (OpState.Greyed == TRUE) then this should be filled
				ion with the reason that the item is shaded/greyed.

	Returns:	An OpState indicating the current menu item state.

	Purpose:	To determine the state of a given menu item. This method is an exact
				parallel to an Op's GetState method (in fact, it is called by an Op's GetState)

********************************************************************************************/

OpState LineGallery::GetCommandState(StringBase *CommandID, String_256 *ShadeReason)
{
	OpState State;

	if (*CommandID == SGCmd_Properties)
		return(State);

	return(State);
}



/********************************************************************************************

>	virtual void LineGallery::DoCommand(StringBase *CommandID)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/03/97

	Inputs:		CommandID - The String ID of the command

	Purpose:	To apply a given command when it is chosen from the menu.

********************************************************************************************/

void LineGallery::DoCommand(StringBase *CommandID)
{
// Taken out by vector stroking code Neville 2/10/97
#ifdef VECTOR_STROKING
//	if (*CommandID == SGCmd_Properties)
//	{
//		SGalleryLinePropertiesDlg::InvokeDialog(this);
//	}
//	else
#endif // VECTOR_STROKING

	// we are now launching the brush edit dialog from here
	SGDisplayNode *pItem = DisplayTree->FindNextSelectedItem(NULL);
	// we want to launch our new edit dialog for the new style brushes, its all new baby
	if (pItem != NULL && pItem->IsKindOf(CC_RUNTIME_CLASS(BrushAttrItem)))
	{
		// get the brush component from the document 
		Document* pDoc = Document::GetCurrent();
		if (pDoc == NULL)
		{
			return; // not necessarily an error, but we don't wish to proceed
		}
		BrushComponent* pBrushComp = (BrushComponent*)pDoc->GetDocComponent(CC_RUNTIME_CLASS(BrushComponent));
		if (pBrushComp == NULL)
		{
			ERROR3("We've got a document with no brush component");
			return;
		}
		BrushHandle Handle = ((BrushAttrItem*)pItem)->GetBrushHandle();
		if (Handle != BrushHandle_NoBrush)
		{
			if (*CommandID == SGCmd_RenameBrush)
			{
				// launch the naming dialog
				OpDescriptor* pDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_NAME_BRUSH_DLG);
				if (pDesc != NULL)
				{
					// provide the new brush handle as a parameter
					OpParam Param(static_cast<INT32>(Handle), 0);
					pDesc->Invoke(&Param);
				}
			}
			else if (*CommandID == SGCmd_ExportBrush)
			{
				pBrushComp->CopyInkObjectsToClipboard(Handle);
			}
			else if (*CommandID == SGCmd_Delete)
			{
				OpDescriptor* pDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_DELETEBRUSH);
				if (pDesc != NULL)
				{
					// provide the new brush handle as a parameter
					OpParam Param(static_cast<INT32>(Handle), 0);
					pDesc->Invoke(&Param);
				}
			}
		
		}
	}

	//	SuperGallery::DoCommand(CommandID);		// Unknown command- pass to the base class
}



/********************************************************************************************

>   MILLIPOINT LineGallery::SetCurrentLineWidth()

	Author:     Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:    9/1/2000
	Returns:	the current default line width
	Purpose:    queries AttributeManager to get the current default,sets it as the member variable
				and returns it
********************************************************************************************/

MILLIPOINT LineGallery::SetPreviousLineWidth()
{
	Document* pDoc = Document::GetCurrent();
	if (pDoc == NULL)
	{
		ERROR3("No document");
		return -1;
	}

	AttributeManager* pAttrMgr = &(pDoc->GetAttributeMgr());
	if (pAttrMgr == NULL)
	{
		ERROR3("No attribute manager");
		return -1;
	}

	NodeAttribute* pAttr = pAttrMgr->GetCurrentAttribute(CC_RUNTIME_CLASS(NodeRenderableInk), CC_RUNTIME_CLASS(AttrLineWidth));
	if (pAttr == NULL)
		return -1;

	LineWidthAttribute* pVal = (LineWidthAttribute*)pAttr->GetAttributeValue();
	if (pVal == NULL)
		return -1;

	m_PreviousLineWidth =  pVal->LineWidth;
	return m_PreviousLineWidth;
}


/********************************************************************************************

>  BOOL LineGallery::PrepareBrushFolders()

	Author:     Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:    9/1/2000
	Returns:    TRUE if successful.
	Purpose:    Prepares tp create folders in which to put brushes that are loaded in as part of the
				load default brushes process.  Basically scans for .xar files in the 
				/Templates/Brushes/ folders and stores each filename found.
				These filenames are subsequently used to create folders in CreateBrushGroups
	SeeAlso:   

********************************************************************************************/

BOOL LineGallery::PrepareBrushFolders()
{
	// find all .xar files in the /templates/brushes directory and put them in a temp array
	
	// allocate a stupidly large array
	String_256 TempString[1000]; // surely can't be 1000 files!

	// set up the path to our .xar files
	String_256 SearchPath;
	CResDll::GetExecutablePath((TCHAR*)SearchPath);
	String_256 Path = TEXT("\\Templates\\Brushes\\");
	SearchPath += Path;
	String_256 FileSpecifier = SearchPath;
	FileSpecifier += TEXT("*.xar");
	UINT32 Counter = 0;

	// use file util to find the file for us, we need only pass the searchpath
	BOOL ok = FileUtil::StartFindingFiles(&FileSpecifier);

	String_256 Filename;
	INT32 StringLength;
	String_256 ShortName;
	while (ok)
	{
		// FileUtil will put our filename string into Filename
		ok = FileUtil::FindNextFile(&Filename);
		
		if (ok && Counter < 1000)
		{
			// strip the .xar from the string
			StringLength = Filename.Length();
			Filename.Left(&ShortName, StringLength - 4);

//			//Special case - when Xara default brushes are loaded, change the folder group
//			//text to something prettier than "defbrush" - bit of a bodge but it works...
//			if (ShortName == String_256("defbrush"))
//			{
//				ShortName = String_256("Default brushes");
//			}

			TempString[Counter++] = ShortName;
		}
	}

	FileUtil::StopFindingFiles();
			
	// Now allocate our member string array, MFC seems to get very upset when
	// we try to delete an array of String_256's so use malloc
	
	m_pFileNameArray = (String_256*)malloc(sizeof(String_256) * Counter);
	
	if (m_pFileNameArray == NULL)
		return FALSE;

	// copy the strings over
	INT32 size = sizeof(String_256);

	memcpy(m_pFileNameArray, &TempString, size * Counter);
	
	m_NumDefaultFiles = Counter;  // remember how many we got

	return TRUE;
}

/********************************************************************************************

>   MILLIPOINT LineGallery::GetCurrentLineWidth()

	Author:     Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:    9/1/2000
	Returns:	the previous line width member
	Purpose:    as above
********************************************************************************************/

MILLIPOINT LineGallery::GetPreviousLineWidth()
{
	return m_PreviousLineWidth;
}


/********************************************************************************************
>   BOOL LineGallery::DeleteSelection()

	Author:     Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:    17/7/2000
	Inputs:     Handle - the handle of the brush to delete
	Outputs:    -
	Returns:    TRUE if all went well, FALSE if not
	Purpose:    Deletes the brush with the given handle
	Errors:     -
	SeeAlso:    -
********************************************************************************************/

BOOL LineGallery::DeleteBrushItem(BrushHandle Handle)
{
	LineGallery *pGallery = (LineGallery *) LineGallery::GetInstance();
	if (pGallery == NULL)
		return FALSE;
	
	// Iterate over each group.
	for (LineAttrGroup* pGroup = (LineAttrGroup*) GetDisplayTree()->GetChild();
		 pGroup != NULL;
		 pGroup = (LineAttrGroup*) pGroup->GetNext())
	{
		// Iterate over each item within the group.
		LineAttrItem* pNextItem;
		
		LineAttrItem* pItem = (LineAttrItem*) pGroup->GetChild();
		
		// if its not a brush we're not interested
		if (!pItem->IsKindOf(CC_RUNTIME_CLASS(BrushAttrItem)))
			pItem = NULL;
		
		while (pItem != NULL)
		{
			// get the next item before we delete anything
			pNextItem = (LineAttrItem*) pItem->GetNext();
		
			// If the item is selected then remove it from the tree before deleting
			if (((BrushAttrItem*)pItem)->GetBrushHandle() == Handle)
			{
				pItem->RemoveFromTree();
				pItem->DestroySubtree(TRUE);
			}
			pItem = pNextItem;
		}
		
	}


	SelectionHasChanged();
	pGallery->ReformatAndRedrawIfNecessary();
	return TRUE;
}

/********************************************************************************************
>   static BOOL OpDisplayLineGallery::Init()

	Author:     Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:    9/2/95 (base generated in sgbase.cpp)
	Inputs:     -
	Outputs:    -
	Returns:    TRUE if the operation could be successfully initialised 
				FALSE if no more memory could be allocated 
	Purpose:    General-purpose line gallery initialiser method
	Errors:     -
	SeeAlso:    -
********************************************************************************************/

BOOL OpDisplayLineGallery::Init()
{
	return LineGallery::Init()
		&& OpChangeLineJoinOpDesc::Init()
		&& OpChangeLineCapOpDesc::Init()
		&& RegisterOpDescriptor(0,
								_R(IDS_DISPLAY_LINE_GALLERY),
								CC_RUNTIME_CLASS(OpDisplayLineGallery),
								OPTOKEN_DISPLAY_LINE_GALLERY,
								OpDisplayLineGallery::GetState,
								0,                                      /* help ID */
								_R(IDBBL_DISPLAY_LINE_GALLERY),
								0);                                     /* bitmap ID */
}


	
/********************************************************************************************
>   OpState OpDisplayLineGallery::GetState(String_256*, OpDescriptor*)

	Author:     Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:    9/2/95 (base generated in sgbase.cpp)
	Inputs:     -
	Outputs:    -
	Returns:    The state of the OpDisplayLineGallery operation
	Purpose:    For finding the OpDisplayLineGallery's state. 
	Errors:     -
	SeeAlso:    -
********************************************************************************************/

OpState OpDisplayLineGallery::GetState(String_256* UIDescription, OpDescriptor*)
{
	// If the gallery is currently open then the menu item should be ticked.
	OpState OpSt;  
	DialogBarOp* pDialogBarOp = FindGallery();
	if (pDialogBarOp != NULL) OpSt.Ticked = pDialogBarOp->IsVisible();

	// If there are no open documents, you can't toggle the gallery
  	OpSt.Greyed = (Document::GetSelected() == NULL);
	return OpSt;
}



/********************************************************************************************
>   void OpDisplayLineGallery::Do(OpDescriptor*)

	Author:     Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:    9/2/95 (base generated in sgbase.cpp)
	Inputs:     -
	Outputs:    -
	Returns:    -
	Purpose:    Displays the Lines gallery
	Errors:     -
	SeeAlso:    -
********************************************************************************************/

void OpDisplayLineGallery::Do(OpDescriptor*)
{
	DialogBarOp *pDialogBarOp = FindGallery();

	if (pDialogBarOp != NULL)
	{
		// Toggle the visible state of the gallery window
		pDialogBarOp->SetVisibility(!pDialogBarOp->IsVisible());

		// And update the gallery button state
		SGInit::UpdateGalleryButton(OPTOKEN_DISPLAY_LINE_GALLERY, pDialogBarOp->IsVisible());
	}
	
	End();
}



/********************************************************************************************
>   static DialogBarOp* OpDisplayLineGallery::FindGallery()

	Author:     Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:    9/2/95 (base generated in sgbase.cpp)
	Returns:    NULL or a pointer to the Line gallery instance
	Purpose:    Finds the Line gallery class instance
	Notes:      The bars system always keeps one Line gallery alive for us.
				If one is not found, this usually indicates that it can't be found
				in bars.ini: Check that the 'Name' string *exactly* matches the
				title string given in bars.ini.
				Also check that bars.ini indicates the bar is of the LineGallery class
********************************************************************************************/

DialogBarOp* OpDisplayLineGallery::FindGallery()
{
	String_32 Name = _R(IDS_SGLINE_GALLERY_NAME); // "Line gallery";
	DialogBarOp* pDialogBarOp = DialogBarOp::FindDialogBarOp(Name);

	if (pDialogBarOp != NULL)
	{
		if (pDialogBarOp->GetRuntimeClass() == CC_RUNTIME_CLASS(LineGallery))
			return(pDialogBarOp);

		ERROR3("Got the line gallery but it's not of the LineGallery class?");
	}
	else
	{
		ERROR3("Can't find the Line gallery in bars.ini!\n");
	}

	return(NULL);
}
