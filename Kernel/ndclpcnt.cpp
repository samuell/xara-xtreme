// $Id: ndclpcnt.cpp 662 2006-03-14 21:31:49Z alex $
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
//
// Implementation of NodeClipViewController.
//

#include "camtypes.h"	// pre-compiled header
#include "ndclpcnt.h"	// header file
//#include "clipres.h"	// ClipView resources
#include "nodeclip.h"	// so we can use NodeClipView and PathBecomeA

#include "cxfrec.h"		// save/load
#include "cxftags.h"	//
#include "cxfdefs.h"	//
#include "camfiltr.h"	//

#include "becomea.h"	// make shapes stuff

#include "extender.h"	// extend stuff
//#include "ngcore.h"

#include "nodepath.h"	// NodePath reference
#include "node.h"		// for PreRenderChildren stuff
#include "gdraw.h"		// for GDraw calls
//#include "mario.h"		// for _R(IDE_NOMORE_MEMORY)

#include "ppbevel.h"	// for BevelHelpers functions
#include "attrmap.h"	// AttrMap reference

#include "blobs.h"		// for blob rendering.

#include "objchge.h"	// for ObjChangeType.

#include "lineattr.h"	// for AttrStrokeColour.

#include "docview.h"	// for DocView.
//#include "osrndrgn.h"	// for OSRenderRegion.

//#include "selector.h"	// for the SelectorTool.
#include "spread.h"		// for class Spread

#include "pbecomea.h"	// for PathBecomeA.

#include "saveeps.h"	// for EPSRenderRegion and EPSExportDC
#include "cmxrendr.h"	// for CMXRenderRegion
//#include "cmxexdc.h"	// for CMXExportDC

DECLARE_SOURCE("$Revision: 662 $");

// dynamic class creation stuff.
CC_IMPLEMENT_DYNCREATE(NodeClipViewController, NodeGroup)
CC_IMPLEMENT_DYNCREATE(ClipViewRecordHandler, CamelotRecordHandler)
CC_IMPLEMENT_DYNCREATE(UpdateCachedKeyholePathAction, Action)

// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW

// some constants we require for half-decent path clipping.
const INT32 NodeClipViewController::CLIPVIEW_TOLERANCE		= 1;
const INT32 NodeClipViewController::CLIPVIEW_SOURCEFLATNESS	= 50;
const INT32 NodeClipViewController::CLIPVIEW_CLIPFLATNESS	= 50;

/********************************************************************************************

>	NodeClipViewController::NodeClipViewController()

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28 January 2000
	Purpose:	Default constructor.
	Errors:	
	See also:	

********************************************************************************************/
NodeClipViewController::NodeClipViewController() : NodeGroup()
{
	// state initialisation.
	m_KeyholePath.Initialise();
	MarkKeyholeInvalid();
}



/********************************************************************************************

>	NodeClipViewController::NodeClipViewController(	Node* pContextNode,
													AttachNodeDirection Direction,
													BOOL Locked		= FALSE,
													BOOL Mangled	= FALSE,
													BOOL Marked		= FALSE,
													BOOL Selected	= FALSE )
	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28 January 2000
	Inputs:		pContextNode	points to the node to which to attach this node.
				Direction		the direction in which this node is to be attached to 
								pContextNode. Possible values:

				PREV		Attach node as a previous sibling of the context node.
				NEXT		Attach node as a next sibling of the context node.
				FIRSTCHILD	Attach node as the first child of the context node.
				LASTCHILD	Attach node as a last child of the context node.

				Locked		is node locked?
				Mangled		is node mangled?
				Marked		is node marked?
				Selected	is node selected?

	Purpose:	This method initialises the node and links it to pContextNode in the
				direction specified by Direction. All necessary tree links are updated.
				Most of the work is carried out by base constructors.

	Errors:		An ENSURE error will occur if pContextNode is NULL.
	See also:	

********************************************************************************************/
NodeClipViewController::NodeClipViewController(	Node* pContextNode,  
												AttachNodeDirection Direction,  
												BOOL Locked, 
												BOOL Mangled,  
												BOOL Marked, 
												BOOL Selected )
						: NodeGroup(pContextNode, Direction,
									Locked, Mangled, Marked, Selected) 
{
	// state initialisation.
	m_KeyholePath.Initialise();
	MarkKeyholeInvalid();
}



/********************************************************************************************

>	NodeClipViewController::~NodeClipViewController()

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28 January 2000
	Purpose:	Destructor.
	Errors:	
	See also:	

********************************************************************************************/
NodeClipViewController::~NodeClipViewController()
{
	m_KeyholePath.ClearPath();
}



/********************************************************************************************

>	void NodeClipViewController::GetDebugDetails(StringBase* pStr)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/25/00
	Inputs:		pStr	ptr to the string to hold the debug output.
	Outputs:	pStr gets our debug output put in it.
	Purpose:	Get Debug-tree details for this node.
				In the case of NCVC, we output our current clipping-path data.
	Notes:		Only _implemented_ in DEBUG builds.

********************************************************************************************/
void NodeClipViewController::GetDebugDetails(StringBase* pStr)
{
#ifdef _DEBUG
	// Get base class debug info.
	NodeGroup::GetDebugDetails(pStr);

	// output the data for our keyhole path.
	String_256 TempStr;
	(*pStr) += TEXT( "\r\nClipping Path Data\r\n" );
	m_KeyholePath.GetDebugDetails(pStr);
#endif
}



/********************************************************************************************

>	NodeClipView* NodeClipViewController::GetClipView(BOOL ReportErrors = TRUE)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18 February 2000
	Inputs:		ReportErrors	if set to FALSE then this method will not ERROR3 if it
								cannot find the clipview node.
	Returns:	The ClipView node which is associated with this NodeClipViewController, or
				NULL if unsuccessful.
	Purpose:	Get this controller's clipview node.
	Errors:		ERROR3 if no relevant NodeClipView is found.
	See also:	

********************************************************************************************/
NodeClipView* NodeClipViewController::GetClipView(BOOL ReportErrors)
{
	Node* pClipView = FindFirstChild();
	while (pClipView != NULL && !pClipView->IsANodeClipView())
		pClipView = pClipView->FindNext();

	ERROR3IF(ReportErrors && pClipView == NULL, "NCVC has no ClipView node!");

	return (NodeClipView*)pClipView;
}



/********************************************************************************************

>	BOOL NodeClipViewController::OwnsAsKeyholeNode(const NodeRenderableInk* pTestNode)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/05/2000
	Inputs:		pTestNode	the possible keyhole node.
	Returns:	TRUE if pTestNode is a keyhole node for us,
				FALSE otherwise.
	Purpose:	Test whether the given node is a keyhole node for this NCVC.
	Errors:		In debug, ERROR3 if pTestNode is NULL.
				In release, just return with FALSE.
	See also:	

********************************************************************************************/
BOOL NodeClipViewController::OwnsAsKeyholeNode(const Node* pTestNode)
{
	// input validation.
	if (pTestNode == NULL)
	{
		ERROR3("NCVC::OwnsAsKeyholeNode; NULL entry parameter!");
		return FALSE;
	}

	// Ensure keyhole path is valid (helps hit-detection)
	UpdateKeyholePath();

	// ok, any ink-node to the left of our NodeClipView is a keyhole node.
	Node* pKeyhole	= NULL;
	Node* pClipView	= GetClipView();
	BOOL bFoundKeyhole = FALSE;
	if (pClipView != NULL)
	{
		pKeyhole = pClipView->FindPrevious();
		while (!bFoundKeyhole && pKeyhole != NULL)
		{
			bFoundKeyhole = (pKeyhole->IsAnObject() && pTestNode == pKeyhole);
			pKeyhole = pKeyhole->FindPrevious();
		}
	}

	return bFoundKeyhole;
}



/********************************************************************************************

>	virtual DocRect NodeClipViewController::GetBoundingRect(BOOL DontUseAttrs, BOOL HitTest)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28 January 2000
	Inputs:		DontUseAttrs	TRUE of you want to ignore all the node's attributes.
				HitTest			TRUE if being called during a hit-test.
	Returns:	The bounding rectangle of this node.

	Purpose:	Find this node's bounding rectangle. If the rectangle is known to be valid 
				then it is simply returned. If IsBoundingRectValid() is FALSE then the rect 
				is recalculated before it is returned and the validity flag reset.

				The bounding rect of a NCVC is the union of the bounds of all its keyhole
				nodes.

	See also:	GetBlobBoundingRect().

********************************************************************************************/
DocRect NodeClipViewController::GetBoundingRect(BOOL DontUseAttrs, BOOL HitTest)
{
// DEBUG:
//	TRACEUSER( "Karim", _T("NCVC::GetBoundingRect\n"));

	// ensure our cached keyhole path is up to date.
//	UpdateKeyholePath();

	// start off with a zeroed bounding rect, and union it with the bounds of all keyhole
	// nodes, ie ink-nodes to the left of our NodeClipView.
	BoundingRectangle = DocRect(0, 0, 0, 0);
	NodeClipView* pClipView = GetClipView();
	if (pClipView != NULL)
	{
		Node* pKeyhole = pClipView->FindPrevious();
		while (pKeyhole != NULL)
		{
			if (pKeyhole->IsAnObject())
			{
				IsBoundingRectValid = TRUE;
				BoundingRectangle = BoundingRectangle.Union(((NodeRenderableInk*)pKeyhole)->GetBoundingRect(DontUseAttrs, HitTest));
			}
			pKeyhole = pKeyhole->FindPrevious();
		}
	}

	return BoundingRectangle;
}



/********************************************************************************************

>	virtual DocRect NodeClipViewController::GetBlobBoundingRect()

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28 January 2000
	Returns:	The bounding rectangle of this node with its blobs drawn.
	Purpose:	Get this NCVC's bounding rectangle when its blobs are drawn.
	See also:	GetBoundingRect();

********************************************************************************************/
DocRect NodeClipViewController::GetBlobBoundingRect()
{
	// we return our bounding rectangle, inflated to account for whichever blobs we're
	// currently showing.
	DocRect drBounds = GetBoundingRect();
	BlobManager* pBlobMgr = GetApplication()->GetBlobManager();
	if (pBlobMgr != NULL)
	{
		DocCoord Low	= drBounds.LowCorner();
		DocCoord High	= drBounds.HighCorner();

		BlobStyle bsBlobs = pBlobMgr->GetCurrentInterest(TRUE);
		if (bsBlobs.Object)
		{
			DocRect drBlobSize;
			pBlobMgr->GetBlobRect(Low, &drBlobSize);
			drBounds = drBounds.Union(drBlobSize);
			pBlobMgr->GetBlobRect(High, &drBlobSize);
			drBounds = drBounds.Union(drBlobSize);
			pBlobMgr->GetBlobRect(DocCoord(Low.x, High.y), &drBlobSize);
			drBounds = drBounds.Union(drBlobSize);
			pBlobMgr->GetBlobRect(DocCoord(High.x, Low.y), &drBlobSize);
			drBounds = drBounds.Union(drBlobSize);
		}

		if (bsBlobs.Tiny)
		{
			DocRect drBlobSize;
			pBlobMgr->GetBlobRect(DocCoord(Low.x, High.y), &drBlobSize);
			drBounds = drBounds.Union(drBlobSize);
		}

		if (bsBlobs.ToolObject)
		{
			DocRect drBlobSize;
			pBlobMgr->GetBlobRect(DocCoord(0, 0), &drBlobSize);
			drBounds.Inflate(drBlobSize.Width(), drBlobSize.Height());
		}
	}

	// this well-named method includes the blob-bounds of any child-attrs,
	// but only if we're showing fill blobs (!!?!?).
	IncludeChildrensBoundingRects(&drBounds);

	return drBounds;
}



/********************************************************************************************

>	virtual DocRect NodeClipViewController::GetEorDragBoundingRect()

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03 April 2000
	Returns:	The bounding rect of this ClipView group to draw when dragging it around,
				or a zeroed DocRect if we have no keyhole nodes (a *bad* thing).
	Purpose:	The rectangle calculated is the union of EorDragBoundingRect()'s for each
				of our keyhole nodes.
	Errors:		
	See also:	

********************************************************************************************/
DocRect NodeClipViewController::GetEorDragBoundingRect()
{
	// start off with a zeroed bounding rect, and union it with the bounds of all keyhole
	// nodes, ie ink-nodes to the left of our NodeClipView.
	DocRect drBounds(0, 0, 0, 0);
	NodeClipView* pClipView = GetClipView();
	if (pClipView != NULL)
	{
		Node* pKeyhole = pClipView->FindPrevious();
		while (pKeyhole != NULL)
		{
			if (pKeyhole->IsAnObject())
				drBounds = drBounds.Union(((NodeRenderableInk*)pKeyhole)->GetEorDragBoundingRect());

			pKeyhole = pKeyhole->FindPrevious();
		}
	}

	return drBounds;
}



/********************************************************************************************

>	virtual SubtreeRenderState NodeClipViewController::RenderSubtree(RenderRegion* pRender, Node** ppNextNode = NULL, BOOL bClip = TRUE)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15 February 2000
	Inputs:		
	Outputs:	
	Returns:	PRE_RENDER_CHILDREN.
	Purpose:	Called by the main rendering loop before this node's children are rendered, 
				usually to allow us to muck around with how we want our children to render.

				It is used here to clear up any debris left over from an unfinished hit-test 
				render-loop.
	Errors:		
	See also:	

********************************************************************************************/
SubtreeRenderState NodeClipViewController::RenderSubtree(RenderRegion* pRender, Node** ppNextNode, BOOL bClip)
{
//#pragma message( __LOCMSG__ "NodeClipViewController::RenderSubtree - do nothing" )
//	TRACE( _T("Warning - NodeClipViewController::RenderSubtree called\n") );
	if (pRender && !pRender->IsPrinting())
	{
		// ensure our cached keyhole path is up to date.
		UpdateKeyholePath();

//		return SUBTREE_ROOTANDCHILDREN;
		return NodeGroup::RenderSubtree(pRender, ppNextNode, bClip);
	}
	else
//		return SUBTREE_ROOTANDCHILDREN;
		return NodeGroup::RenderSubtree(pRender, ppNextNode, bClip);

//	return SUBTREE_ROOTANDCHILDREN;
}



/********************************************************************************************

>	virtual void NodeClipViewController::RenderEorDrag(RenderRegion* pRender)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28 January 2000
	Inputs:		pRender		pointer to the render region to render into.
	Purpose:	Render this node as eor-blobs into the given render region.

				What we probably want to do is clip the outlines of all our children, and
				render those outlines into the given render region, however for now we'll
				just render our keyhole node's outline.
	Errors:	
	See also:	See Render() for info.

********************************************************************************************/
void NodeClipViewController::RenderEorDrag(RenderRegion* pRender)
{
	PORTNOTETRACE("other","NodeClipViewController::RenderEorDrag - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	// <RANT>
	// unfortunately, if we opt to do eor-drag-rendering for our kids (and we do),
	// then as well as getting to render for them when we're dragged around, we also
	// get lumbered with rendering for them when *they* get dragged around.
	// this is annoying and the solution should *not* be to do their rendering
	// for them if we're not selected, but that's what we're gonna do.
	// </RANT>

	// determine whether myself or one of my ancestors is selected.
	BOOL fMeOrMyAncestorIsSelected = IsSelected();
	Node* pParent = FindParent();
	while (!fMeOrMyAncestorIsSelected && pParent != NULL)
	{
		fMeOrMyAncestorIsSelected = pParent->IsSelected();
		pParent = pParent->FindParent();
	}

	// do normal eor-drag-rendering for myself (eor-rendering our keyhole path.)
	if (fMeOrMyAncestorIsSelected)
	{
		UpdateKeyholePath();
		pRender->DrawPath(&m_KeyholePath);
	}

	// do eor-drag-rendering for those of my lazy children which are selected.
	RenderEorDragSelectedChildren(this, pRender);
#endif
}

/********************************************************************************************

>	virtual void NodeClipViewController::RenderEorDragSelectedChildren(Node* pParent,
																	   RenderRegion* pRender)
	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/04/2000
	Inputs:		
	Outputs:	
	Returns:	
	Purpose:	Recursively render EOR outlines for those of my children who are selected.
				Note that if we meet a selected child which claims to render its children
				then we let it do so, otherwise we use RenderEorDragChildren() to forcibly
				render its kids.

	Notes:		I get the feeling this vfn should really live in NodeRenderableInk, so if
				things work out then I'll put it there.

	Errors:		
	See also:	RenderEorDragChildren() for something similar...

********************************************************************************************/
void NodeClipViewController::RenderEorDragSelectedChildren(	Node* pParent,
															RenderRegion* pRender )
{
	NodeRenderableInk* pInkKid = NULL;
	Node* pKid = pParent->FindFirstChild();
	while (pKid != NULL)
	{
		if (pKid->IsAnObject())
		{
			pInkKid = (NodeRenderableInk*)pKid;

			// ok, is it selected? if so, render its drag blobs, taking account of bossy
			// controller nodes who want to render their children themselves.
			if (pInkKid->IsSelected())
			{
				pInkKid->RenderEorDrag(pRender);
				if (!pInkKid->ChildrenAreEorDragRenderedByMe())
					pInkKid->RenderEorDragChildren(pRender);
			}

			// nope, not selected - recurse down into it if possible.
			else
			{
				RenderEorDragSelectedChildren(pInkKid, pRender);
			}
		}
		pKid = pKid->FindNext();
	}
}



/********************************************************************************************

>	virtual BOOL NodeClipViewController::ChildrenAreEorDragRenderedByMe()

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	04 April 2000
	Inputs:		
	Outputs:	
	Returns:	TRUE.
	Purpose:	Tells everyone else that I want to be responsible for rendering the drag
				blobs for my children.
	Errors:		
	See also:	

********************************************************************************************/
BOOL NodeClipViewController::ChildrenAreEorDragRenderedByMe()
{
	return TRUE;
}



/********************************************************************************************

>	virtual void NodeClipViewController::RenderTinyBlobs(RenderRegion* pRender)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	06 April 2000
	Inputs:		pRender		the render-region to render into.
	Purpose:	Render our tiny-blob.
				This consists of a solitary blob at the top-left of our bounding rect.

********************************************************************************************/
void NodeClipViewController::RenderTinyBlobs(RenderRegion* pRender)
{
	PORTNOTETRACE("other","NodeClipViewController::RenderTinyBlobs - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	// Get our bounding rect, and from it the position of our tiny blob.
	DocRect drBounds = GetBoundingRect();
	DocCoord dcTinyPos = DocCoord(drBounds.lo.x, drBounds.hi.y);

	// Set our blob's colours and render it.
	pRender->SetFillColour(COLOUR_UNSELECTEDBLOB);
	pRender->SetLineColour(COLOUR_NONE);
	pRender->DrawBlob(dcTinyPos, BT_UNSELECTED);
#endif
}

/********************************************************************************************

>	virtual void NodeClipViewController::RenderToolObjectBlobs(RenderRegion* pRender)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23 March 2000
	Inputs:		pRender		ptr to the region to render into.
	Purpose:	Render our tool-object blobs.

********************************************************************************************/
void NodeClipViewController::RenderToolObjectBlobs(RenderRegion* pRender)
{
	RenderClipViewBlobs(pRender);
}



/********************************************************************************************

>	virtual void NodeClipViewController::RenderClipViewBlobs(RenderRegion* pRender)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23 March 2000
	Inputs:		pRender		the region to render to.
	Purpose:	Render the blobs specifically used by ClipView groups.
				These are currently as follows:
				1.	A keyhole-selection blob, which is rendered at the top-left of our
					bounding rectangle.
				2.	A select-contained-objects blob, which is rendered in the centre of the
					group's bounding rectangle.
	Errors:		
	See also:	

********************************************************************************************/
void NodeClipViewController::RenderClipViewBlobs(RenderRegion* pRender)
{
	PORTNOTETRACE("other","NodeClipViewController::RenderClipViewBlobs - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	// quit immediately unless the current tool is the selector tool.
	Tool* pTool = Tool::GetCurrent();
	if (pTool == NULL || pTool->GetID() != TOOLID_SELECTOR)
		return;

	// get the blob manager and set the colours we'll be using.
	BlobManager* pBlobManager = GetApplication()->GetBlobManager();
	pRender->SetLineColour(COLOUR_NONE);
	pRender->SetFillColour(COLOUR_UNSELECTEDBLOB);

	// plot the keyhole blob at the top-left of our bounding rectangle.
	DocRect drBounds = GetBoundingRect();
	DocCoord dcKeyhole(drBounds.lo.x, drBounds.hi.y);
	pRender->DrawBitmapBlob(dcKeyhole, _R(IDBMP_KEYHOLE));

	// plot the select-inside blob at the centre of our bounding rectangle.
	pRender->DrawBitmapBlob(drBounds.Centre(), _R(IDBMP_CLIPVIEW_SELECT_CONTENTS));

	// BODGE CODE!
	// some of the ClipView blobs are rendered in the same position as our tiny blobs.
	// we don't want the tiny blobs to interfere with these blobs, so we double-render
	// them off the picture (double XOR = 0) whenever these blobs are rendered.
	// this relies on Tiny blobs always being rendered whenever these blobs
	// are rendered (which they have always been up till 06/04/2000)
	RenderTinyBlobs(pRender);
#endif
}

/********************************************************************************************

>	virtual BOOL NodeClipViewController::OnClick(	DocCoord dcClickPos, ClickType cType,
													ClickModifiers cMods, Spread* pSpread )
	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23 March 2000
	Inputs:		dcClickPos		where the click occurred.
				cType			what sort of click it was.
				cMods			what modifier keys were pressed at the time.
				pSpread			the spread the click occurred on.
	Returns:	TRUE if we could handle,
				FALSE if it was just too much for us.

	Purpose:	Handle any click-messages passed to us from a tool, eg if we want to handle
				clicks on our object or tool-object blobs.
				Currently, you can click on an NCVC tool-object blob and either select its
				keyhole node, or select everything *except* its keyhole node. At some point
				in future, you will also be able to directly drag everything except the
				keyhole node (instead of clicking once to select, _then_ dragging).

********************************************************************************************/
BOOL NodeClipViewController::OnClick(	DocCoord dcClickPos, ClickType cType,
										ClickModifiers cMods, Spread* pSpread )
{
	PORTNOTETRACE("other","NodeClipViewController::OnClick - do nothing");
#ifndef EXCLUDE_FROM_XARALX
// DEBUG
//	TRACEUSER( "Karim", _T("NCVC::OnClick\n"), (DWORD)cType);

	// determine what to do with the click.
	// note that we can't do anything unless we have valid pointers to view and blob manager.
	BlobManager* pBlobMgr = GetApplication()->GetBlobManager();
	DocView* pDocView = DocView::GetSelected();
	if (pBlobMgr == NULL || pDocView == NULL)
		return FALSE;

	// currently we only specifically handle ToolObject blobs.
	BlobStyle bsCurrentInterest = pBlobMgr->GetCurrentInterest();
	if (bsCurrentInterest.ToolObject)
	{
		// get bounds rects for the appropriate blobs.
		DocRect drKeyholeBlobRect;
		DocRect drContentsBlobRect;
		DocRect drBounds = GetBoundingRect();
		pBlobMgr->GetBlobRect(DocCoord(drBounds.lo.x, drBounds.hi.y), &drKeyholeBlobRect,
																		FALSE, BT_CLIPVIEW);
		pBlobMgr->GetBlobRect(drBounds.Centre(), &drContentsBlobRect, FALSE, BT_CLIPVIEW);

		// inflate the rects, so the user doesn't need to be pin-point accurate.
		MILLIPOINT nNearRadius = OSRenderRegion::GetHitTestRadius(pDocView);
		drKeyholeBlobRect.Inflate(nNearRadius);
		drContentsBlobRect.Inflate(nNearRadius);

		// ok, lets test!
		if (drKeyholeBlobRect.ContainsCoord(dcClickPos))
		{
			if (HandleKeyholeBlobClick(cType))
				return TRUE;
		}
		else if (drContentsBlobRect.ContainsCoord(dcClickPos))
		{
			if (HandleContentsBlobClick(cType))
				return TRUE;
		}
	}

	// if we haven't handled the click and we're displaying object blobs,
	// hand over to our base implementation.
	if (bsCurrentInterest.Object)
		return NodeGroup::OnClick(dcClickPos, cType, cMods, pSpread);
#endif
	// we couldn't handle the click.
	return FALSE;
}



/********************************************************************************************

>	virtual BOOL NodeClipViewController::HandleKeyholeBlobClick(ClickType cType)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23 March 2000
	Inputs:		
	Outputs:	
	Returns:	TRUE if we handled the click,
				FALSE if we passed.
	Purpose:	Handler for what happens when the user clicks on our keyhole-select blob.

				All we do is try to select our keyhole nodes.
	Errors:		
	See also:	

********************************************************************************************/
BOOL NodeClipViewController::HandleKeyholeBlobClick(ClickType cType)
{
// DEBUG
//	TRACEUSER( "Karim", _T("NCVC::HandleKeyholeBlobClick!\n"));

	// quit immediately unless it's a button-up click with the selector tool.
	Tool* pTool = Tool::GetCurrent();
	if (cType != CLICKTYPE_UP || pTool == NULL || pTool->GetID() != TOOLID_SELECTOR)
		return FALSE;

	// select all our keyhole nodes, which are those nodes to the left of our NodeClipView.
	// if for some reason we can't select any nodes, we'll return FALSE.
	BOOL bChangedSelection	= FALSE;
	Node* pFirstKeyhole		= NULL;
	NodeClipView* pClipView	= GetClipView();
	if (pClipView != NULL)
	{
		Node* pKeyhole		= pClipView->FindPrevious();
		while (pKeyhole != NULL)
		{
			if (pKeyhole->IsAnObject())
			{
				if (!bChangedSelection)
				{
					DeselectAll();
					pFirstKeyhole		= pKeyhole;
					bChangedSelection	= TRUE;
				}

				((NodeRenderableInk*)pKeyhole)->Select(TRUE);
			}
			pKeyhole = pKeyhole->FindPrevious();
		}
	}

	if (bChangedSelection)
	{
		SelRange* pSel = GetApplication()->FindSelection();
		if (pSel != NULL)
			pSel->Update(TRUE, pFirstKeyhole);
	}

	return bChangedSelection;
}



/********************************************************************************************

>	virtual BOOL NodeClipViewController::HandleContentsBlobClick(ClickType cType)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23 March 2000
	Inputs:		
	Outputs:	
	Returns:	TRUE if we handled the click,
				FALSE if we passed.
	Purpose:	Handler for what happens when the user clicks on our contents-select blob.

				We select all our NodeRenderableInk children *except* for our keyhole node.
	Errors:		
	See also:	

********************************************************************************************/
BOOL NodeClipViewController::HandleContentsBlobClick(ClickType cType)
{
	PORTNOTETRACE("other","NodeClipViewController::OnClick - do nothing");
#ifndef EXCLUDE_FROM_XARALX
// DEBUG
//	TRACEUSER( "Karim", _T("NCVC::HandleContentsBlobClick; cType %s\n"),
//													(cType == CLICKTYPE_DRAG)	? "DRAG" :
//													(cType == CLICKTYPE_UP)		? "UP" : "");

	// quit immediately unless it's the selector tool.
	Tool* pTool = Tool::GetCurrent();
	if (pTool == NULL || pTool->GetID() != TOOLID_SELECTOR)
		return FALSE;

	// also quit if we don't like the click type.
	if (cType != CLICKTYPE_DRAG && cType != CLICKTYPE_UP)
		return FALSE;

	// deselect everything, then select all non-keyhole nodes,
	// ie all nodes which are right-siblings of our clipview node.
	BOOL bChangedSelection	= FALSE;
	Node* pFirstKid			= NULL;
	NodeClipView* pClipView	= GetClipView();
	if (pClipView != NULL)
	{
		Node* pKid = pClipView->FindNext();
		while (pKid != NULL)
		{
			if (pKid->IsAnObject())
			{
				if (!bChangedSelection)
				{
					DeselectAll();
					pFirstKid			= pKid;
					bChangedSelection	= TRUE;
				}

				((NodeRenderableInk*)pKid)->Select(TRUE);
			}
			pKid = pKid->FindNext();
		}
	}

	if (bChangedSelection)
	{
		SelRange* pSel = GetApplication()->FindSelection();
		if (pSel != NULL)
			pSel->Update(TRUE, pFirstKid);
	}

	// ok, if we successfully changed the selection, we
	// may need to start a drag with the selector tool.
	if (bChangedSelection && cType == CLICKTYPE_DRAG)
		((SelectorTool*)pTool)->PublicDoTranslate();

	return bChangedSelection;
#else
	return false;
#endif
}

/********************************************************************************************

>	virtual BOOL NodeClipViewController::CanBecomeA(BecomeA* pBecomeA)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28 January 2000
	Inputs:		pClass			pointer to the runtime class of the node type to convert to.
				pNumObjects		pointer in which to return the number of objects of type 
								pClass which would be created under a DoBecomeA(). Can be
								NULL, in which case we only return success or failure.
	Returns:	TRUE if we can change ourself or our children into a pClass type of node,
				FALSE otherwise.

	Purpose:	This method is used by the convert to shapes operation. It determines whether 
				this node or any of its children can convert themselves into a pClass Node.

				The number returned in pNumObjects (if it's not NULL) should exactly equal
				the total number of pClass objects which would be created by the appropriate 
				call to DoBecomeA on this node. It should NOT contain any additional objects
				produced, such as group objects or attributes created by the conversion.

				Unfortunately, in the case of NodeClipViewController this is impossible
				without changing the CanBecomeA mechanism or the rules by which NCVC converts
				its children to shapes. The function for number of shapes is recursive:

				SetOfKidPaths	= {all paths which our children return under DoBecomeA}
				NumObjects		=	#SetOfKidPaths +
									#(members of SetOfKidPaths with non-zero line width)

				, which would require CanBecomeA to return numbers of objects returned with
				and without zero-width outlines.

				We _can_ set upper and lower bounds on the number of objects we'll return -
				either all of our children will have line widths, or none of them will, so
				NodeGroup::CanBecomeA will return a lower bound and twice that number will
				give an upper bound. We choose to return the upper bound, for ease of use
				with DMc's code.
	Errors:		
	See also:	

********************************************************************************************/
BOOL NodeClipViewController::CanBecomeA(BecomeA* pBecomeA)
{
	// to convert to paths we'll need to convert each of our children.

	// we cannot return an accurate number of created objects unless we DoBecomeA on our
	// children. we _can_ however return a minimum or maximum number of objects.
	// David Mc's contour/bevel code does not like being passed a minimum, so we'll
	// give him a maximum instead.

	// minimum = number of paths our children will return.
	// maximum = twice the number of paths our children will return.
	UINT32 GroupNumObjects = pBecomeA->GetCount();
	BOOL bCanDo = NodeGroup::CanBecomeA(pBecomeA);
	pBecomeA->AddCount(pBecomeA->GetCount()-GroupNumObjects);	// Add the same count again to double it

	return bCanDo;
}



/********************************************************************************************

>	virtual BOOL NodeClipViewController::DoBecomeA(BecomeA* pBecomeA)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28 January 2000
	Inputs:		pBecomeA	pointer to a BecomeA structure containing information about the 
				required transformation.
	Outputs:	This node and its children may be hidden and replaced with other node types 
				representing a similar scene, eg a group of paths.
	Returns:	TRUE if successful,
				FALSE otherwise.

	Purpose:	Turn this node into another node of a particular type.
				This method implements make-shapes functionality, which basically means 
				turning itself and its children into paths. Don't hold your breath if you ask 
				it to turn into anything else...

				Defined behaviour for ClipView DoBecomeA is that the keyhole nodes will have
				the DoBecomeA call passed directly on to them. Any nodes which are clipped
				behave differently. If these nodes have an outline, then it is first
				split off from them, into a separate, new shape. Both outline shape and
				original shape are then clipped to the keyhole path and left in the tree or
				passed back. These nodes will also all have their line width set to zero and
				their line colour set to transparent.

	Notes:		If this node cannot locate a full set of attributes (eg it is hidden), you
				may get strange results from this method.

				Karim 14/06/2000
				Just added Silhouette functionality to BecomeA. This means that for PASSBACK,
				the BecomeA object can flag that a silhouette is all that's required, which
				should hopefully lead to better contouring/bevelling with ClipView.
				If we're silhouetting, then we pass the BecomeA on to our our keyhole nodes.

	Errors:		ERROR2 if pBecomeA is NULL.
	See also:	

********************************************************************************************/
BOOL NodeClipViewController::DoBecomeA(BecomeA* pBecomeA)
{
	// validate input parameter.
	ERROR2IF(pBecomeA == NULL, FALSE, "NULL pBecomeA");

	// quit unless we're converting to paths.
	ERROR2IF(!pBecomeA->BAPath(), FALSE, "Attempt to convert to other than a path");

	// ensure our keyhole path is up to date.
	if (!UpdateKeyholePath())
		return FALSE;

	// ok, we do different things depending on what type of BecomeA we are.
	switch (pBecomeA->GetReason())
	{
	case BECOMEA_REPLACE:
// DEBUG
//		TRACEUSER( "Karim", _T("NCVC::DoBecomeA; BECOMEA_REPLACE\n"));
		{
			// if we're doing a BECOMEA_REPLACE, we need an op.
			UndoableOperation* pUndoOp = pBecomeA->GetUndoOp();
//			ERROR2IF(pUndoOp == NULL, FALSE, "Need an UndoOp with BECOMEA_REPLACE");

			// local variables.
			Node*				pKid		= NULL;
			Node*				pNextKid	= NULL;
//			Node*				pAfterKey	= NULL;
//			NodeHidden*			pHiddenKey	= NULL;
			Node*				pClipView	= GetClipView();
			BOOL ok = (pClipView != NULL);

			// localise attributes.
			if (ok)
			{
				if (pUndoOp)
					ok = pUndoOp->DoLocaliseCommonAttributes(this);
				else
					LocaliseCommonAttributes();
			}

			// first, DoBecomeA on our children.
			if (ok)
			{
				// if we only have to get a silhouette, then DoBecomeA on our keyhole nodes,
				// and hide the rest of the children.
				if (pBecomeA->DoSilhouette())
				{
					pKid = FindFirstChild();
					while (ok && pKid != pClipView)
					{
						if (pKid->IsAnObject())
							ok = pKid->DoBecomeA(pBecomeA);
						pKid = pKid->FindNext();
					}
					pKid = pClipView->FindNext();
					while (ok && pKid != NULL)
					{
						pNextKid = pKid->FindNext();
						if (pUndoOp)
							ok = pUndoOp->DoHideNode(pKid, TRUE);
						else
						{
							pKid->CascadeDelete();
							delete pKid;
						}
						pKid = pNextKid;
					}
				}

				// otherwise, DoBecomeA on all of our children.
				else
				{
					pKid = FindFirstChild();
					while (ok && pKid != NULL)
					{
						pNextKid = pKid->FindNext();
						ok = pKid->DoBecomeA(pBecomeA);
						pKid = pNextKid;
					}
				}
			}

			// now separate the outline and fill of each of our non-keyhole children out into
			// new ink-nodes, clipped to our keyhole path. non-keyhole means all nodes to the 
			// right of our NodeClipView, and we do this depth-first, so we don't miss paths
			// hidden underneath group hierarchies.

			// unfortunately, paths can sometimes be completely clipped out of the tree. if
			// this happens then we could end up with empty groups (not ideal behaviour), so
			// we need to check for these and if we find any then we hide them.
			if (ok && !pBecomeA->DoSilhouette())
			{
				BOOL bFinishedAGroup = FALSE;
				pKid = pClipView->FindNextDepthFirst(this);
				while (ok && pKid != NULL && pKid != this)
				{
					pNextKid = pKid->FindNextDepthFirst(this);

					// if we're about to do the last kid in a group, remember this for later.
					if (IS_A(pNextKid, NodeGroup)/*->IsAGroup()*/ && pNextKid == pKid->FindParent())
						bFinishedAGroup = TRUE;
					else
						bFinishedAGroup = FALSE;

					// if this one's a NodePath, process it.
					if (pKid->IsNodePath())
						ok = MakeShapeAndLine((NodePath*)pKid, pBecomeA, TRUE);

					// if we've just finished a group, we need to check whether we left it
					// containing any ink-children - if not, we need to hide it.
					if (ok && bFinishedAGroup)
					{
						BOOL bFoundInkKids = FALSE;
						Node* pInkKid = pNextKid->FindFirstChild();
						while (!bFoundInkKids && pInkKid != NULL)
						{
							bFoundInkKids = pInkKid->IsAnObject();
							pInkKid = pInkKid->FindNext();
						}
						if (!bFoundInkKids)
						{
							// move to the next depth-first node and hide the group.
							pKid = pNextKid;
							pNextKid = pKid->FindNextDepthFirst(this);
							if (pUndoOp)
								ok = pUndoOp->DoHideNode(pKid, TRUE);
							else
							{
								pKid->CascadeDelete();
								delete pKid;
							}
						}
					}

					pKid = pNextKid;
				}
			}

			// make sure that all remaining common attributes are factored back out.
			if (ok)
			{
				if (pUndoOp)
					ok = pUndoOp->DoFactorOutCommonChildAttributes(this);
				else
					FactorOutCommonChildAttributes();
			}

			// mighty-morph into a group (ClipView nodes hidden and replaced by a group node).
			if (ok) ok = (BecomeAGroup(pUndoOp) != NULL);

			// return the result of our deliberations.
			return ok;
		}
		break;

	case BECOMEA_PASSBACK:
// DEBUG
//		TRACEUSER( "Karim", _T("NCVC::DoBecomeA; BECOMEA_PASSBACK\n"));
		{
			// local variables.
			Node *			pKid		= NULL;
			NodeGroup *		pContainer	= NULL;
//			NodeHidden *	pHiddenKey	= NULL;
			NodeClipView *	pClipView	= GetClipView();
			BOOL ok = (pClipView != NULL);

			// passback all our keyhole nodes directly.
			if (ok)
			{
				pKid = FindFirstChild();
				while (ok && pKid != pClipView)
				{
					if (pKid->IsAnObject())
						ok = pKid->DoBecomeA(pBecomeA);
					pKid = pKid->FindNext();
				}
			}

			// quit now if all that is required is our silhouette,
			// as that is fully defined by our keyhole nodes.
			if (pBecomeA->DoSilhouette())
				return ok;

			// make shape copies of all our non-keyhole kids (which are right-siblings
			// of our clipview node) and put the result into a container node.
			if (ok)
			{
				pContainer = new NodeGroup();
				ok = (pContainer != NULL);
			}
			if (ok)
			{
				CopyBecomeA refCopyBecomeA(BECOMEA_PASSBACK, CC_RUNTIME_CLASS(NodePath), NULL);
				refCopyBecomeA.SetContextNode(pContainer);
				pKid = pClipView->FindNext();
				while (ok && pKid != NULL)
				{
					if (pKid->CanBecomeA(&refCopyBecomeA))
						ok = pKid->DoBecomeA(&refCopyBecomeA);

					pKid = pKid->FindNext();
				}
			}

			// convert the outlines of these shapes into independent shapes,
			// clip the output to our keyhole path and pass it all back.
			if (ok)
			{
				pKid = pContainer->FindFirstDepthFirst();
				while (ok && pKid != NULL)
				{
					if (pKid->IsNodePath())
						ok = MakeShapeAndLine((NodePath*)pKid, pBecomeA, TRUE);

					pKid = pKid->FindNextDepthFirst(pContainer);
				}
			}

			// dispose of our child-shapes when we've finished with them.
			if (pContainer != NULL)
			{
				pContainer->CascadeDelete();
				delete pContainer;
				pContainer = NULL;
			}

			return ok;
		}
		break;

	default:
		ERROR2(FALSE, "Unrecognised reason in convert-to-shapes");
		break;
	}

	return FALSE;
}



/********************************************************************************************

>	BOOL NodeClipViewController::MakeShapeAndLine(	NodePath* pNodePath,
													BecomeA* pBecomeA,
													BOOL bClipToKeyhole )
	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/03/2000
	Inputs:		pNodePath		the node whose shape and line we will extract.
				pBecomeA		ptr to a BecomeA - determines how to return the output.
				bClipToKeyhole	whether we should clip the output to our keyhole path.
	Outputs:	
	Returns:	TRUE if successful, FALSE otherwise.
	Purpose:	Given a NodePath, copy it and remove the line width, and if it had any line
				width then create a path from its applied line. If required, clip the copies
				against the keyhole path, and depending on whether doing a BECOMEA_REPLACE or
				a BECOMEA_PASSBACK, either undoably replace the node with the copies or pass
				them back.
	Errors:		ERROR3 returning FALSE if invalid parameters.
	See also:	

********************************************************************************************/
BOOL NodeClipViewController::MakeShapeAndLine(	NodePath* pNodePath,
												BecomeA* pBecomeA,
												BOOL bClipToKeyhole )
{
	PORTNOTETRACE("other","NodeClipViewController::RenderEorDrag - do nothing");
#ifndef EXCLUDE_FROM_XARALX
// DEBUG
//	TRACEUSER( "Karim", _T("NCVC::MakeShapeAndLine\n"));

	// validate inputs.
	if (pNodePath == NULL || pBecomeA == NULL)
	{
		ERROR3("NCVC::MakeShapeAndLine; NULL parameters");
		return FALSE;
	}
	BecomeAReason Reason = pBecomeA->GetReason();
	if (Reason != BECOMEA_REPLACE && Reason != BECOMEA_PASSBACK)
	{
		ERROR3("NCVC::MakeShapeAndLine; Invalid parameters");
		return FALSE;
	}
	UndoableOperation* pUndoOp = pBecomeA->GetUndoOp();
//	if (Reason == BECOMEA_REPLACE && pUndoOp == NULL)
//	{
//		ERROR3("NCVC::MakeShapeAndLine; Invalid parameters");
//		return FALSE;
//	}

	// local variables.
	BOOL		ok				= TRUE;
	NodePath*	pNewNode		= NULL;
	CCAttrMap*	pAttrMap		= NULL;

	// if the path is filled then insert/passback a doctored copy of it,
	// complete with attached copies of all the node's attributes.
	// note that we don't want to find indirectly applied GLAs here (eg feathers).
	if (pNodePath->InkPath.IsFilled)
	{
		if (ok)
		{
			pNewNode = (NodePath*)pNodePath->SimpleCopy();
			ok = (pNewNode != NULL);
		}
		if (ok)
		{
			pAttrMap = NULL;
			ALLOC_WITH_FAIL(pAttrMap, new CCAttrMap, pUndoOp);
			ok = (pAttrMap != NULL);
		}
		if (ok) ok = pNodePath->FindAppliedAttributes(pAttrMap, 5000, NULL, TRUE);
		if (ok) ok = pNewNode->ApplyAttributes(pAttrMap);
		if (ok)
		{
			if (Reason == BECOMEA_REPLACE)
			{
				ok = InsertClippedNode(pNewNode, pNodePath, PREV, pUndoOp, bClipToKeyhole);
				if (ok) pBecomeA->PassBack(pNewNode, this, pAttrMap->Copy());
			}
			else
				ok = PassBackClippedNode(pNewNode, pBecomeA, bClipToKeyhole);
		}
		if (pAttrMap != NULL)
		{
			delete pAttrMap;
			pAttrMap = NULL;
		}
	}

	// if the path has an outline, then stroke its outline and pass it back/insert it,
	// complete with copies of the node's attributes.
	if (ok && !InkHasClearLineColour(pNodePath))
	{
		if (ok)
		{
			INT32 Flatness = EstimatePathFlatness();
			pNewNode = pNodePath->MakeNodePathFromAttributes(Flatness, NULL, TRUE);
			ok = (pNewNode != NULL);
		}
		if (ok)
		{
			pAttrMap = NULL;
			ALLOC_WITH_FAIL(pAttrMap, new CCAttrMap, pUndoOp);
			ok = (pAttrMap != NULL);
		}
		if (ok) ok = pNodePath->FindAppliedAttributes(pAttrMap, 5000, NULL, TRUE);
		if (ok) ok = pNewNode->ApplyAttributes(pAttrMap);
		if (ok) ok = CopyInkFillColourFromLine(pNewNode);
		if (ok) ok = CopyInkTransparencyFromLine(pNewNode);
		if (ok)
		{
			if (Reason == BECOMEA_REPLACE)
			{
				ok = InsertClippedNode(pNewNode, pNodePath, PREV, pUndoOp, bClipToKeyhole);
				if (ok) pBecomeA->PassBack(pNewNode, this, pAttrMap->Copy());
			}
			else
				ok = PassBackClippedNode(pNewNode, pBecomeA, bClipToKeyhole);
		}
		if (pAttrMap != NULL)
		{
			delete pAttrMap;
			pAttrMap = NULL;
		}
	}

	// if we're replacing, and things are ok, then hide the NodePath.
	if (ok && Reason == BECOMEA_REPLACE)
	{
		if (pUndoOp)
			ok = pUndoOp->DoHideNode(pNodePath, FALSE);
		else
		{
			pNodePath->CascadeDelete();
			delete pNodePath;
		}
	}

	return ok;
#else
	return false;
#endif
}

/********************************************************************************************

>	BOOL NodeClipViewController::InsertClippedNode(	NodePath* pNewNode,
												    NodePath* pDestNode,
													AttachNodeDirection Direction,
													UndoableOperation* pUndoOp,
													BOOL bClipToKeyhole )
	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21 March 2000
	Inputs:		pNewNode		the new node to insert.
				pDestNode		where to insert it.
				Direction		before, after, under or over ;o)
				pUndoOp			we need an UndoOp to do all this undoably.
				bClipToKeyhole	should its path be clipped to the keyhole's path first?
	Outputs:	pNewNode is inserted into the tree, with its attributes normalised.
	Returns:	TRUE if success, FALSE otherwise.
	Purpose:	Insert the given node into the tree, having first removed any line width it
				has applied to it, and also optionally clipped its path to our cached keyhole
				path. Note that if we are unsuccessful then pNewNode will be deleted, along
				with any children it may have had.
	Errors:		ERROR3 returning FALSE if invalid parameters.
	See also:	

********************************************************************************************/
BOOL NodeClipViewController::InsertClippedNode(	NodePath* pNewNode,
											    NodePath* pDestNode,
												AttachNodeDirection Direction,
												UndoableOperation* pUndoOp,
												BOOL bClipToKeyhole )
{
	// input validation.
	if (pNewNode == NULL /*|| pUndoOp == NULL*/)
	{
		ERROR3("NCVC::InsertClippedNode; NULL parameters");
		return FALSE;
	}

	// make the node's line transparent, if necessary clip to the keyhole node,
	// and then undoably insert it into the tree.
	// note that we *don't* abort the whole operation if clipping the
	// path went badly, we just don't insert it into the tree.
	BOOL clipok = TRUE;
	BOOL ok		= RemoveInkLineWidth(pNewNode);
	if (ok)
	{
		if (bClipToKeyhole)
		{
			Path* pWorkPath = &(pNewNode->InkPath);
			m_KeyholePath.ClipPathToPath(*pWorkPath, pWorkPath, 2,	CLIPVIEW_TOLERANCE,
																	CLIPVIEW_SOURCEFLATNESS,
																	CLIPVIEW_CLIPFLATNESS);
			if (pWorkPath->GetNumCoords() < 2)
				clipok = FALSE;
			else
				pWorkPath->InitialiseFlags();
		}
		if (clipok)
		{
			if (pUndoOp)
				ok = pUndoOp->DoInsertNewNode(	pNewNode, pDestNode, Direction, FALSE,
												FALSE, pDestNode->IsSelected(), TRUE );
			else
				pNewNode->AttachNode(pDestNode, Direction);
		}
	}

	// tidy up if necessary.
	if ((!clipok || !ok) && pNewNode != NULL)
	{
		pNewNode->CascadeDelete();
		delete pNewNode;
	}
	pNewNode = NULL;

	return ok;
}



/********************************************************************************************

>	BOOL NodeClipViewController::PassBackClippedNode(	NodePath* pNewNode,
														BecomeA* pBecomeA,
														BOOL bClipToKeyhole )
	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21 March 2000
	Inputs:		pNewNode		the node to pass back.
				pBecomeA		contains the passback method we'll use.
				bClipToKeyhole	whether we should clip the node to our keyhole path.
	Outputs:	see Purpose.
	Returns:	TRUE if successful, FALSE otherwise.
	Purpose:	Calls pBecomeA->PassBack() with pNewNode and an appropriate attribute map.
				Before passing back, pNewNode's line width is removed and it is optionally
				clipped to our keyhole path. The attribute map copies all the attributes
				currently applied to pNewNode.
				Any children of pNewNode are deleted before it is passed back.
				If anything goes wrong then pNewNode and its children are deleted.

	Errors:		ERROR3 returning FALSE if invalid parameters.
	See also:	

********************************************************************************************/
BOOL NodeClipViewController::PassBackClippedNode(	NodePath* pNewNode,
													BecomeA* pBecomeA,
													BOOL bClipToKeyhole )
{
	// validate inputs.
	if (pNewNode == NULL || pBecomeA == NULL)
	{
		ERROR3("NCVC::PassBackClippedNode; NULL parameters");
		return FALSE;
	}

	// local variables.
	BOOL		clipok			= TRUE;
	BOOL		ok				= TRUE;
	CCAttrMap*	pAttrMap		= NULL;
	CCAttrMap*	pCopiedAttrs	= NULL;

	// remove line width, copy its attribute map, try to clip to the keyhole
	// node and then pass back the path.
	// NB	although we usually abort if anything goes wrong, we do *not* abort
	//		if we failed to clip to the keyhole path - we just don't pass it back.
	if (ok) ok = RemoveInkLineWidth(pNewNode);
	if (ok)
	{
		pAttrMap = new CCAttrMap();
		ok = (pAttrMap != NULL);
	}
	if (ok) ok = pNewNode->FindAppliedAttributes(pAttrMap);
	if (ok)
	{
		pCopiedAttrs = pAttrMap->Copy();
		ok = (pCopiedAttrs != NULL);
	}
	if (ok)
	{
		pNewNode->CascadeDelete();
		if (bClipToKeyhole)
		{
			Path* pWorkPath = &(pNewNode->InkPath);
			m_KeyholePath.ClipPathToPath(*pWorkPath, pWorkPath, 2,	CLIPVIEW_TOLERANCE,
																	CLIPVIEW_SOURCEFLATNESS,
																	CLIPVIEW_CLIPFLATNESS);
			if (pWorkPath->GetNumCoords() < 2)
				clipok = FALSE;
			else
				pWorkPath->InitialiseFlags();
		}
		if (clipok)
			ok = pBecomeA->PassBack(pNewNode, this, pCopiedAttrs);
	}

	// tidy up if everything didn't go according to plan.
	if (!ok || !clipok)
	{
		if (pNewNode != NULL)
		{
			pNewNode->CascadeDelete();
			delete pNewNode;
		}
		if (pCopiedAttrs != NULL)
		{
			pCopiedAttrs->DeleteAttributes();
			delete pCopiedAttrs;
		}
	}
	if (pAttrMap != NULL)
		delete pAttrMap;

	return ok;
}



/********************************************************************************************

>	BOOL NodeClipViewController::InkHasClearLineColour(NodeRenderableInk* pInkNode)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08 March 2000
	Inputs:		InkNode		const reference to a NodeRenderableInk.

	Returns:	TRUE if we can definitely confirm the node has a transparent line width,
				FALSE for any other result.

	Purpose:	Test whether the line colour of the given NodeRenderableInk is transparent.
	Errors:		
	See also:	

********************************************************************************************/
BOOL NodeClipViewController::InkHasClearLineColour(NodeRenderableInk* pInkNode)
{
	NodeAttribute *pAppliedAttr = NULL;
	pInkNode->FindAppliedAttribute(CC_RUNTIME_CLASS(AttrStrokeColour), &pAppliedAttr);
	if (pAppliedAttr != NULL)
	{
		DocColour* pLineColour = ((AttrStrokeColour*)pAppliedAttr)->GetStartColour();
		if (pLineColour != NULL)
//#pragma message( __LOCMSG__ "Removed DocColour usage" )
			return pLineColour->IsTransparent();
	}
	return FALSE;
}



/********************************************************************************************

>	BOOL NodeClipViewController::RemoveInkLineWidth(NodeRenderableInk* pInk)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09 March 2000
	Inputs:		pInk	pointer to the NodeRenderableInk to work on.
	Outputs:	pInk's line colour should become transparent.
	Returns:	TRUE if successful,
				FALSE otherwise.
	Purpose:	Make the outline of the given node transparent and set its line width to 0.

	NOTE:		This method does *not* record undo information, and it expects attributes
				to have been localised beforehand.
	Errors:		ERROR3 and returns FALSE if pInk is NULL.
	See also:	

********************************************************************************************/
BOOL NodeClipViewController::RemoveInkLineWidth(NodeRenderableInk* pInk)
{
	if (pInk == NULL)
	{
		ERROR3("NCVC::RemoveInkLineWidth; NULL parameter(s)");
		return FALSE;
	}

	NodeAttribute*		pAppliedAttr = NULL;
	DocColour*			pLineColour	 = NULL;
	LineWidthAttribute*	pLineWidth	 = NULL;

	pInk->FindAppliedAttribute(CC_RUNTIME_CLASS(AttrStrokeColour), &pAppliedAttr);
	if (pAppliedAttr != NULL)
		pLineColour = ((AttrStrokeColour*)pAppliedAttr)->GetStartColour();

	pAppliedAttr = NULL;
	pInk->FindAppliedAttribute(CC_RUNTIME_CLASS(AttrLineWidth), &pAppliedAttr);
	if (pAppliedAttr != NULL)
		pLineWidth = (LineWidthAttribute*)pAppliedAttr->GetAttributeValue();

	if (pLineColour != NULL && pLineWidth != NULL)
	{
		*pLineColour = DocColour(COLOUR_TRANS);
		pLineWidth->LineWidth = 0;
		return TRUE;
	}
	
	return FALSE;
}



/********************************************************************************************

>	BOOL NodeClipViewController::CopyInkFillColourFromLine(NodeRenderableInk* pInk)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09 March 2000
	Inputs:		pInk	pointer to the NodeRenderableInk to work on.
	Outputs:	pInk's fill colour should match its line colour.
	Returns:	TRUE if successful,
				FALSE otherwise.
	Purpose:	Obtain the line colour of the given NodeRenderableInk and re-apply it as its
				new fill colour.

	NOTE:		This method is *not* undoable!

	Errors:		ERROR2 returning FALSE if pInk is NULL.
	See also:	

********************************************************************************************/
BOOL NodeClipViewController::CopyInkFillColourFromLine(NodeRenderableInk* pInk)
{
	ERROR2IF(pInk == NULL, FALSE, "NULL parameter(s)");

	NodeAttribute *pLineColourAttr = NULL;
	DocColour* pLineColour = NULL;
//	DocColour* pFillColour = NULL;
	Node* pOldFill = NULL;
	AttrFlatColourFill* pNewFlatFill = NULL;

	// obtain the line-colour attribute and its actual colour value.
	pInk->FindAppliedAttribute(CC_RUNTIME_CLASS(AttrStrokeColour), &pLineColourAttr);
	// DY: bodge to solve problem of creating brushes from clipview objects.  The problem was 
	// that we are using an inknode that is not located in a document so it sometimes does
	// not have an applied stroke colour, so just return true
	
	BOOL ok = (pLineColourAttr != NULL);
	if (ok)
	{
		pLineColour = ((AttrStrokeColour*)pLineColourAttr)->GetStartColour();
		ok = (pLineColour != NULL);
	}
	else
		return TRUE; // safe return 

	// search the children of the object node for its current colour fill attribute.
	if (ok)
	{
		BOOL bFoundFill = FALSE;
		pOldFill = pInk->FindFirstChild();
		while (!bFoundFill && pOldFill != NULL)
		{
			if ( pOldFill->IsAFillAttr() && ((NodeAttribute*)pOldFill)->IsAColourFill() )
				bFoundFill = TRUE;
			else
				pOldFill = pOldFill->FindNext();
		}
	}

	// create a new fill attribute.
	if (ok)
	{
		pNewFlatFill = new AttrFlatColourFill();
		ok = (pNewFlatFill != NULL);
	}

	// if we were successful, then put it all into the tree and remove any
	// previous fill applied.
	if (ok)
	{
		pNewFlatFill->SetStartColour(pLineColour);
		pNewFlatFill->AttachNode(pInk, LASTCHILD);
		if (pOldFill != NULL)
		{
			pOldFill->UnlinkNodeFromTree();
			delete pOldFill;
		}
	}

	// ensure we free allocated memory if unsuccessful.
	else
	{
		if (pNewFlatFill != NULL)
		{
			delete pNewFlatFill;
			pNewFlatFill = NULL;
		}
	}

	return ok;
}



/********************************************************************************************

>	BOOL NodeClipViewController::CopyInkTransparencyFromLine(NodeRenderableInk* pInk)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20 March 2000
	Inputs:		pInk	pointer to the NodeRenderableInk to work on.
	Outputs:	pInk should be given the same transparency as its outline.
	Returns:	TRUE if successful,
				FALSE otherwise.
	Purpose:	Obtain the line transparency of the given NodeRenderableInk and re-apply it
				as a new flat transparency.

	NOTE:		This method is *not* undoable!

	Errors:		ERROR2 returning FALSE if pInk is NULL.
	See also:	

********************************************************************************************/
BOOL NodeClipViewController::CopyInkTransparencyFromLine(NodeRenderableInk* pInk)
{
	ERROR2IF(pInk == NULL, FALSE, "NULL parameter(s)");

	NodeAttribute *pLineTranspAttr = NULL;
	UINT32* pLineTransp = NULL;
//	UINT32* pFillTransp = NULL;
	Node* pOldFill = NULL;
	AttrFlatTranspFill* pNewFlatFill = NULL;

	// obtain the line-transparency attribute and its actual transparency value.
	pInk->FindAppliedAttribute(CC_RUNTIME_CLASS(AttrStrokeTransp), &pLineTranspAttr);
	// DY: bodge to solve problem of creating brushes from clipview objects.  The problem was 
	// that we are using an inknode that is not located in a document so it sometimes does
	// not have an applied stroke transparency, so just return true
	
	BOOL ok = (pLineTranspAttr != NULL);
	if (ok)
	{
		pLineTransp = ((AttrStrokeTransp*)pLineTranspAttr)->GetStartTransp();
		ok = (pLineTransp != NULL);
	}
	else
		return TRUE;

	// search the children of the object node for its current transparency fill attribute.
	if (ok)
	{
		BOOL bFoundFill = FALSE;
		pOldFill = pInk->FindFirstChild();
		while (!bFoundFill && pOldFill != NULL)
		{
			if ( pOldFill->IsAFillAttr() && ((NodeAttribute*)pOldFill)->IsATranspFill() )
				bFoundFill = TRUE;
			else
				pOldFill = pOldFill->FindNext();
		}
	}

	// create a new transparency fill attribute.
	if (ok)
	{
		pNewFlatFill = new AttrFlatTranspFill();
		ok = (pNewFlatFill != NULL);
	}

	// if we were successful, then put it all into the tree and remove any
	// previous fill applied.
	if (ok)
	{
		pNewFlatFill->SetStartTransp(pLineTransp);
		pNewFlatFill->AttachNode(pInk, LASTCHILD);
		if (pOldFill != NULL)
		{
			pOldFill->UnlinkNodeFromTree();
			delete pOldFill;
		}
	}

	// ensure we free allocated memory if unsuccessful.
	else
	{
		if (pNewFlatFill != NULL)
		{
			delete pNewFlatFill;
			pNewFlatFill = NULL;
		}
	}

	return ok;
}



/********************************************************************************************

>	virtual NodeGroup* NodeClipViewController::BecomeAGroup(UndoableOperation* pUndoOp)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07 March 2000
	Inputs:		pUndoOp		pointer to an UndoableOperation
	Outputs:	
	Returns:	TRUE if successful,
				FALSE otherwise.

	Purpose:	We are sometimes required to turn ourself into a normal group node, and that
				is exactly what this method does. If an UndoOp is supplied then it is done 
				in an undoable fashion but it must also work non-undoably if a NULL op pointer 
				is passed in.

********************************************************************************************/
NodeGroup* NodeClipViewController::BecomeAGroup(UndoableOperation* pUndoOp)
{
	// ok!
	BOOL ok = TRUE;

	NodeGroup*		pGroup		= NULL;
	NodeClipView*	pClipView	= GetClipView(FALSE);

	// hide our clipview node if it exists.
	if (ok) ok = (pClipView != NULL);
	if (ok)
	{
		if (pUndoOp)
			ok = pUndoOp->DoHideNode(pClipView, TRUE);
		else
		{
			pClipView->CascadeDelete();
			delete pClipView;		// Scary!
		}
	}

	// call our base class to turn into a group.
	if (ok)	pGroup = NodeGroup::BecomeAGroup(pUndoOp);

	return pGroup;
}



/********************************************************************************************

>	BOOL NodeClipViewController::UpdateKeyholePath()

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13 March 2000
	Inputs:		
	Outputs:	
	Returns:	
	Purpose:	Update our cached outline for our keyhole node.
				Note that this method only tries to update the keyhole path if it is marked
				as invalid. If it is already valid, you just get TRUE back.
				It will also only re-mark the keyhole as valid if it successfully updates
				the keyhole cache.
	Errors:		
	See also:	InvalidateKeyhole().

********************************************************************************************/
BOOL NodeClipViewController::UpdateKeyholePath()
{
// DEBUG
//	TRACEUSER( "Karim", _T("NCVC::UpdateKeyholePath; %s.\n"),
//				IsKeyholeValid() ? "valid" : "invalid");

	if (IsKeyholeValid())
		return TRUE;

	NodeClipView* pClipView = GetClipView();
	if (pClipView == NULL)
		return FALSE;

	// DoBecomeA on all our keyhole nodes (nodes to the left of our clipview node), and
	// add the resulting paths up to get one big keyhole-path.
	// in the interests of accuracy, and because most of the time we expect only to have
	// one keyhole path, we're going to do as little clip-path-to-path'ing as possible.
	BOOL bAddedFirstPath = FALSE;
	Node* pKeyhole = pClipView->FindPrevious();
	while (!bAddedFirstPath && pKeyhole != NULL)
	{
		if (pKeyhole->IsAnObject())
		{
			m_KeyholePath.ClearPath();
			PathBecomeA baInfo(BECOMEA_PASSBACK, CC_RUNTIME_CLASS(NodePath), NULL, FALSE,
								&m_KeyholePath, PathBecomeA::STRIP_OUTLINES );
			baInfo.SetPathsOnly();
			if (pKeyhole->DoBecomeA(&baInfo))
				bAddedFirstPath = TRUE;
		}
		pKeyhole = pKeyhole->FindPrevious();
	}

	// ok, now add any other keyholes which are kicking around.
	BOOL ok = TRUE;
	if (pKeyhole != NULL)
	{
		INT32 Flatness = EstimatePathFlatness();
		Path m_WorkPath;
		m_WorkPath.Initialise();
		while (ok && pKeyhole != NULL)
		{
			if (pKeyhole->IsAnObject())
			{
				m_WorkPath.ClearPath();
				PathBecomeA baInfo(BECOMEA_PASSBACK, CC_RUNTIME_CLASS(NodePath), NULL, FALSE,
									&m_WorkPath, PathBecomeA::STRIP_OUTLINES);
				baInfo.SetPathsOnly();
				if (ok && pKeyhole->DoBecomeA(&baInfo))
					ok = (m_KeyholePath.ClipPathToPath(	m_WorkPath, &m_KeyholePath, 7,
														CLIPVIEW_TOLERANCE,
														Flatness,
														Flatness) != -1);
			}
			pKeyhole = pKeyhole->FindPrevious();
		}
	}

	// if our keyhole path is empty, then we need to hide everything which we're clipping.
	// we'll do this by building an empty keyhole path out of our bounding rect.
	if (m_KeyholePath.GetNumCoords() == 0)
	{
		DocRect drBounds = GetBoundingRect();
		DocCoord drLeft(drBounds.lo.x, (drBounds.lo.y + drBounds.hi.y) / 2);
		DocCoord drRight(drBounds.hi.x, (drBounds.lo.y + drBounds.hi.y) / 2);

		m_KeyholePath.Initialise(3);
		m_KeyholePath.AddMoveTo(drLeft);
		m_KeyholePath.AddLineTo(drRight);
		m_KeyholePath.AddLineTo(drLeft);
		m_KeyholePath.IsFilled = TRUE;
	}

	// ok, setup with the new keyhole path.
	pClipView->SetClipPath(&m_KeyholePath);
	if (ok)
		MarkKeyholeValid();

	return ok;
}



/********************************************************************************************

>	static INT32 NodeClipViewController::EstimatePathFlatness()

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09 March 2000
	Returns:	Flatness required for paths used by ClipView.
	Purpose:	Return an estimation of how flat paths need to be for ClipView groups, based
				on the current document's zoom factor. A default value will be returned if
				there is no current document.

				According to DMc, flatness is the max straight line length when a curve is
				approximated - smaller is smoother.
				We should aim for a flatness of ~0.1 pixels.
	Errors:		
	See also:	

********************************************************************************************/
INT32 NodeClipViewController::EstimatePathFlatness()
{
	// we use a default flatness of 75 millipoints, as this is 0.1 pixels at 100% zoom.
	// (100% zoom = 96 dpi @ 72000 mp per inch ==> 750 mp per dot)
	static const double DefaultFlatness = 75.0;
	DocView* pView		= DocView::GetCurrent();
	double ZoomFactor	= (pView == NULL) ? 1 : pView->GetZoomFactor();
	INT32 Flatness		= (INT32)(ZoomFactor * DefaultFlatness);
	return (Flatness == 0) ? 1 : Flatness;
}



/********************************************************************************************

>	virtual BOOL NodeClipViewController::AllowOp(	ObjChangeParam *pParam,
													BOOL SetOpPermissionState,
													BOOL DoPreTriggerEdit )
	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28 January 2000
	Inputs:		pParam					describes the way an op wants to change the node
				SetOpPermissionState	if TRUE the OpPermission of nodes should be set
				DoPreTriggerEdit		if TRUE then NameGallery::PreTriggerEdit is called.
										*Must* be TRUE if the calling Op may make any nodes
										change their bounds, eg move, line width, cut.
										Use TRUE if unsure.
	Outputs:	See input param descriptions.
	Returns:	TRUE if the Op may continue,
				FALSE to stop the Op in its tracks.
	Purpose:	See Node::AllowOp() for a lengthy explanation.
	Errors:		ERROR2 if pParam is NULL.
	See also:	

********************************************************************************************/
BOOL NodeClipViewController::AllowOp(ObjChangeParam *pParam, BOOL SetOpPermissionState,
															 BOOL DoPreTriggerEdit)
{
	// input validation.
	ERROR2IF(pParam == NULL, FALSE, "NULL pParam");

	// call our base class' AllowOp().
	BOOL allowed = NodeGroup::AllowOp(pParam, SetOpPermissionState, DoPreTriggerEdit);

	// Slight misuse of the AllowOp/OnChildChange system, as that says that only our
	// children are meant to set our permission to PERMISSION_ALLOWED.
	// This will ensure that our OnChildChange() method is called after the op has finished,
	// allowing us to update our keyhole path after any changes.
	if (allowed && SetOpPermissionState)
		SetOpPermission(PERMISSION_ALLOWED, TRUE);

	return allowed;
}



/********************************************************************************************

>	ChangeCode NodeClipViewController::OnChildChange(ObjChangeParam* pParam)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28 January 2000
	Inputs:		pParam	describes the change which occurred.
	Outputs:	This node may regenerate itself to accomodate the change.
	Returns:	CC_OK		if we have successfully processed the change.
				CC_FAIL		if we cannot handle this particular change and must prevent the
							child from continuing.
	Purpose:	Used to notify this node of changes to its children, so that if required it
				can update itself. This forms part of the AllowOp / UpdateChangedNodes
				mechanism.
	Errors:		ERROR2 if pParam is NULL.
	See also:	Node::WarnParentOfChange(), Node::AllowOp()

********************************************************************************************/
ChangeCode NodeClipViewController::OnChildChange(ObjChangeParam* pParam)
{
	// validate input.
	ERROR2IF(pParam == NULL, CC_FAIL, "NULL pParam");

	// we'll need pointers to our ClipView and Keyhole nodes later on.
	NodeClipView* pClipView = GetClipView(FALSE);

	// we need to deal with circumstances which may put us into an inconsistent state.
	// these are:
	//
	//	* No ink-children, or only a ClipView node	==>	hide ourself.
	//	* No keyhole nodes		=>
	//	* No clipped nodes		==>		turn ourself into a group.
	//	* No ClipView node		=>
	//
	UndoableOperation* pUndoOp = pParam->GetOpPointer();
	ObjChangeType cType = pParam->GetChangeType();
	if (cType == OBJCHANGE_FINISHED)
	{
		// count our kids.
		UINT32 NumKeyhole	= 0;
		UINT32 NumClipped	= 0;
		UINT32 NumKids	= 0;
		BOOL fCountingKeyholes = TRUE;
		Node* pKid = FindFirstChild();
		while (pKid != NULL)
		{
			if (pKid->IsAnObject())
			{
				if (pKid == pClipView)
				{
					fCountingKeyholes = FALSE;
				}
				else
				{
					if (fCountingKeyholes)
						NumKeyhole ++;
					else
						NumClipped ++;
				}
			}
			pKid = pKid->FindNext();
		}
		NumKids = NumKeyhole + NumClipped;

		// ok, we hide ourself if we have no ink-kids (ignoring the clipview node).
		if (NumKids == 0)
		{

			// we need an Op in order to do tree manipulations.
			BOOL ok = (pUndoOp != NULL);

			// hide ourself - find our parent before we do though.
			Node* pParent = FindParent();
			if (ok)
				ok = pUndoOp->DoHideNode(this,TRUE);

			// now we're hidden, factor out all attributes on our parent.
			if (ok)
				if (pParent != NULL && pParent->IsCompound())
					ok = pUndoOp->DoFactorOutCommonChildAttributes(((NodeRenderableInk*)pParent),TRUE);

			// TODO: rewind any actions here?
			if (!ok)
				return CC_FAIL;
		}

		// if we're missing any one of our three magic ingredients, we mighty-morph into a group.
		if (pClipView == NULL || NumKeyhole == 0 || NumClipped == 0)
		{
			// we need a valid UndoOp ptr with which to perform the conversion.
			if (pUndoOp != NULL)
				if (BecomeAGroup(pUndoOp))
					return CC_OK;

			return CC_FAIL;
		}

		// the last case is where we have all three ingredients, in which case we
		// invalidate our keyhole path and make a 'mental' note that note that it
		// will need invalidating again whenever the user performs an undo or redo.
		else if (pUndoOp != NULL)
		{
// DEBUG
/*			TRACEUSER( "Karim", _T("NCVC::OnCC;"));

			// direction of change.
			ObjChangeDirection cDirection = pParam->GetDirection();
			switch (cDirection)
			{
			case OBJCHANGE_CALLEDBYCHILD:
				TRACEUSER( "Karim", _T(" OBJCHANGE_CALLEDBYCHILD"));
				break;

			case OBJCHANGE_CALLEDBYPARENT:
				TRACEUSER( "Karim", _T(" OBJCHANGE_CALLEDBYPARENT"));
				break;

			case OBJCHANGE_CALLEDBYOP:
				TRACEUSER( "Karim", _T(" OBJCHANGE_CALLEDBYOP"));
				break;

			default:
				break;
			}

			// type of change.
			TRACEUSER( "Karim", _T(" ;"));
			switch (cType)
			{
			case OBJCHANGE_UNDEFINED:
				TRACEUSER( "Karim", _T(" OBJCHANGE_UNDEFINED"));
				break;

			case OBJCHANGE_STARTING:
				TRACEUSER( "Karim", _T(" OBJCHANGE_STARTING"));
				break;

			case OBJCHANGE_RENDERCURRENTBLOBS:
				TRACEUSER( "Karim", _T(" OBJCHANGE_RENDERCURRENTBLOBS"));
				break;

			case OBJCHANGE_RENDERCHANGEDBLOBS:
				TRACEUSER( "Karim", _T(" OBJCHANGE_RENDERCHANGEDBLOBS"));
				break;

			case OBJCHANGE_FINISHED:
				TRACEUSER( "Karim", _T(" OBJCHANGE_FINISHED"));
				break;

			case OBJCHANGE_IGNORE:
				TRACEUSER( "Karim", _T(" OBJCHANGE_IGNORE"));
				break;

			case OBJCHANGE_FAILED:
				TRACEUSER( "Karim", _T(" OBJCHANGE_FAILED"));
				break;

			default:
				break;
			}

			// set change-flags.
			TRACEUSER( "Karim", _T("; Flags {"));
			ObjChangeFlags cFlags = pParam->GetChangeFlags();
			if (cFlags.Attribute)
				TRACEUSER( "Karim", _T(" Attribute"));
			if (cFlags.CopyNode)
				TRACEUSER( "Karim", _T(" CopyNode"));
			if (cFlags.DeleteNode)
				TRACEUSER( "Karim", _T(" DeleteNode"));
			if (cFlags.MoveNode)
				TRACEUSER( "Karim", _T(" MoveNode"));
			if (cFlags.MultiReplaceNode)
				TRACEUSER( "Karim", _T(" MultiReplaceNode"));
			if (cFlags.RegenerateNode)
				TRACEUSER( "Karim", _T(" RegenerateNode"));
			if (cFlags.ReplaceNode)
				TRACEUSER( "Karim", _T(" ReplaceNode"));
			if (cFlags.TransformNode)
				TRACEUSER( "Karim", _T(" TransformNode"));
			TRACEUSER( "Karim", _T(" }\n"));
*/
			// mark the cached keyhole path for update.
			UpdateCachedKeyholePathAction::Init( pUndoOp,
												 pUndoOp->GetUndoActions(),
												 this );
		}
		else if (!pUndoOp)
		{
			Document * pDoc = Document::GetCurrent();

			if (pDoc)
			{
				if (pParam->GetChangeFlags ().RegenerateNode)
				{
					MarkKeyholeInvalid ();		// CGS:  it is now legitimate for us to do this
				}
				
				Spread * pSpread = (Spread *)FindParent(CC_RUNTIME_CLASS(Spread));
				pDoc->ForceRedraw(pSpread, GetBoundingRect(FALSE, FALSE), FALSE, this);
			}
		}
	}

	// call the base class version for the normal group checks, which will also deal with the
	// case where we have no kids at all.
	return NodeGroup::OnChildChange(pParam);
}



/********************************************************************************************

>	virtual String NodeClipViewController::Describe(BOOL Plural, BOOL Verbose)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28 January 2000
	Inputs:		Plural		whether to pluralise the description.
				Verbose		short or long version.
	Returns:	A string description of this node.
	Purpose:	Get a string description, for use in menus and infobar etc.
	Errors:		
	See also:	

********************************************************************************************/
String NodeClipViewController::Describe(BOOL Plural, BOOL Verbose)
{
	if (Plural)
		return(String(_R(IDS_CLIPVIEW_CONTROLLER_DESCRP)));
	else
		return(String(_R(IDS_CLIPVIEW_CONTROLLER_DESCRS)));
}



/********************************************************************************************

>	virtual UINT32 NodeClipViewController::GetNodeSize() const 

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02 February 2000
	Returns:	The size of the node in bytes.
	Purpose:	Obtain the size of a NodeClipViewController object.

	See also:	Node::GetSubtreeSize

********************************************************************************************/
UINT32 NodeClipViewController::GetNodeSize() const 
{
	return sizeof(NodeClipViewController);
}



/********************************************************************************************

>	void NodeClipViewController::Transform(TransformBase &Trans)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31 January 2000
	Inputs:		Trans	non-const reference to a description of the transformation.
	Outputs:	This node will be transformed appropriately.
	Purpose:	Perform a transformation on this node.
	Errors:		
	See also:	

********************************************************************************************/
void NodeClipViewController::Transform(TransformBase &Trans)
{
	// required because certain processes which transform nodes do not fire off my
	// OnChildChange method (haven't investigated why)
	MarkKeyholeInvalid();

	// ClipViews can't just transform their cached data - they have to
	// also transform their children because clipview's GetBoundingRect
	// always calculates - never returns cached rectangle.
	Trans.bTransformYourChildren = TRUE;

	// ClipViews can't just transform their cached data - they have to
	// also transform their children because clipview's GetBoundingRect
	// always calculates - never returns cached rectangle.
	Trans.bTransformYourChildren = TRUE;

	// ClipViews can't just transform their cached data - they have to
	// also transform their children because clipview's GetBoundingRect
	// always calculates - never returns cached rectangle.
	Trans.bTransformYourChildren = TRUE;

	// transform our children etc, in the same way a normal group would.
	NodeGroup::Transform(Trans);
}



/********************************************************************************************

>	BOOL NodeClipViewController::WritePreChildrenWeb(BaseCamelotFilter* pFilter)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31 January 2000
	Inputs:		pFilter		pointer to a camelot file filter.

	Returns:	TRUE if successful,
				FALSE otherwise.
	Purpose:	Writes this node out to a camelot document.
	Errors:		ERROR2 if pFilter is NULL.
	See also:	

********************************************************************************************/
BOOL NodeClipViewController::WritePreChildrenWeb(BaseCamelotFilter* pFilter)
{
	// validate input.
	ERROR2IF(pFilter == NULL, FALSE, "NULL parameter");
	
	CXaraFileRecord Rec(TAG_CLIPVIEWCONTROLLER, TAG_CLIPVIEW_CONTROLLER_SIZE);

	BOOL	ok = Rec.Init();
	if (ok)	ok = (pFilter->Write(&Rec) != 0);

	return ok;
}



/********************************************************************************************

>	BOOL NodeClipViewController::WritePreChildrenNative(BaseCamelotFilter* pFilter)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31 January 2000
	Inputs:		pFilter		pointer to a camelot file filter.

	Returns:	TRUE if successful,
				FALSE otherwise.
	Purpose:	Writes this node out to a camelot document.
	Errors:		
	See also:	

********************************************************************************************/
BOOL NodeClipViewController::WritePreChildrenNative(BaseCamelotFilter* pFilter)
{
	return WritePreChildrenWeb(pFilter);
}



/********************************************************************************************

>	BOOL NodeClipViewController::PostImport()

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31 January 2000
	Returns:	TRUE if success,
				FALSE if unsuccessful.
	Purpose:	Performs any necessary post-processing once the object has been read in
				from a file.
	Errors:		
	See also:	

********************************************************************************************/
BOOL NodeClipViewController::PostImport()
{
	return UpdateKeyholePath();
}



/********************************************************************************************

>	virtual Node* NodeClipViewController::SimpleCopy()

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09 February 2000
	Returns:	A copy of this node, or
				NULL if unsuccessful.
	Purpose:	Copy this node.
	Errors:		ERROR1 returning NULL if we couldn't allocate memory for the new node.
	See also:	Node::SimpleCopy()

********************************************************************************************/
Node* NodeClipViewController::SimpleCopy()
{
	NodeClipViewController* pNodeCopy = new NodeClipViewController;;
	ERROR1IF(pNodeCopy == NULL, NULL, _R(IDE_NOMORE_MEMORY));
	CopyNodeContents(pNodeCopy);

	return pNodeCopy;
}



/********************************************************************************************

>	virtual void NodeClipViewController::CopyNodeContents(NodeClipViewController* pNodeCopy)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09 February 2000
	Inputs:		pNodeCopy	the node to copy our contents into.
	Purpose:	Copy this node's contents into pNodeCopy.
	Errors:		
	See also:	Node::CopyNodeContents()

********************************************************************************************/
void NodeClipViewController::CopyNodeContents(NodeClipViewController* pNodeCopy)
{
	// call base-class implementation; this will also perform necessary validation for us.
	NodeGroup::CopyNodeContents(pNodeCopy);
}



/***********************************************************************************************
>   void NodeClipViewController::PolyCopyNodeContents(NodeRenderable* pNodeCopy)

    Author:     Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
    Created:    18/12/2003
    Outputs:    -
    Purpose:    Polymorphically copies the contents of this node to another
	Errors:     An assertion failure will occur if NodeCopy is NULL
    Scope:      protected
								     
***********************************************************************************************/

void NodeClipViewController::PolyCopyNodeContents(NodeRenderable* pNodeCopy)
{
	ENSURE(pNodeCopy, "Trying to copy a node's contents into a NULL node");
	ENSURE(IS_A(pNodeCopy, NodeClipViewController), "PolyCopyNodeContents given wrong dest node type");

	if (IS_A(pNodeCopy, NodeClipViewController))
		CopyNodeContents((NodeClipViewController*)pNodeCopy);
}




/********************************************************************************************

>	virtual void NodeClipViewController::PreExportRender(RenderRegion* pRegion)

	Author:		Chris_Gallimore (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8th November 2000
	Inputs:		pRegion - the RenderRegion we're exporting to.
	Purpose:	Starts exporting a clipview group - basically writes a 'q' token out
	Errors:		
	See also:	Node::PreExportRender()

********************************************************************************************/
void NodeClipViewController::PreExportRender(RenderRegion* pRegion)
{
#ifdef DO_EXPORT
	if (pRegion->IsKindOf(CC_RUNTIME_CLASS(EPSRenderRegion)))
	{
		// Output "start group" token
		EPSExportDC *pDC = (EPSExportDC *) pRegion->GetRenderDC();
		pDC->OutputToken(_T("q"));
		pDC->OutputNewLine();
	}
	else if(pRegion->IsKindOf(CC_RUNTIME_CLASS(CMXRenderRegion)))
	{
		// mark start of a group...
		CMXExportDC *pDC = (CMXExportDC *) pRegion->GetRenderDC();
		DocRect BBox = GetBoundingRect();
		pDC->StartGroup(&BBox);
	}
#endif
}

/********************************************************************************************

>	virtual void NodeClipViewController::ExportRender(RenderRegion* pRegion)

	Author:		Chris_Gallimore (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8th November 2000
	Inputs:		pRegion - the RenderRegion we're exporting to.
	Purpose:	Finishes exporting a clipview group - basically write a 'Q' token out.
	Errors:		
	See also:	Node::ExportRender()

********************************************************************************************/
BOOL NodeClipViewController::ExportRender(RenderRegion* pRegion) 
{
#ifdef DO_EXPORT
	if (pRegion->IsKindOf(CC_RUNTIME_CLASS(EPSRenderRegion)))
	{
		// Output "end group" token
		EPSExportDC *pDC = (EPSExportDC *) pRegion->GetRenderDC();
		pDC->OutputToken(_T("Q"));
		pDC->OutputNewLine();
		
		// Tell caller we rendered ourselves ok
		return TRUE;
	}
	else if(pRegion->IsKindOf(CC_RUNTIME_CLASS(CMXRenderRegion)))
	{
		// mark start of a group...
		CMXExportDC *pDC = (CMXExportDC *) pRegion->GetRenderDC();
		pDC->EndGroup();

		return TRUE;
	}
#endif
	// Render this node in the normal way
	return FALSE;
}


//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------



/********************************************************************************************

>	ClipViewRecordHandler::ClipViewRecordHandler()

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09 February 2000
	Purpose:	Constructor

********************************************************************************************/
ClipViewRecordHandler::ClipViewRecordHandler()
{
	// empty.
}



/********************************************************************************************

>	ClipViewRecordHandler::~ClipViewRecordHandler()

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09 February 2000
	Purpose:	Destructor

********************************************************************************************/
ClipViewRecordHandler::~ClipViewRecordHandler()
{
	// empty.
}



/********************************************************************************************

>	virtual UINT32* ClipViewRecordHandler::GetTagList()

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09 February 2000
	Returns:	An array of the tags which we can handle, terminated by CXFRH_TAG_LIST_END.
	Purpose:	Gets the tag list for this handler to handle.

********************************************************************************************/
UINT32* ClipViewRecordHandler::GetTagList()
{
	static UINT32 TagList[] = { TAG_CLIPVIEWCONTROLLER, TAG_CLIPVIEW, CXFRH_TAG_LIST_END };

	return (UINT32*)&TagList;
}



/********************************************************************************************

>	virtual BOOL ClipViewRecordHandler::HandleRecord(CXaraFileRecord* pCXaraFileRecord)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09 February 2000
	Inputs:		pCXaraFileRecord	pointer to the file object.
	Returns:	TRUE if successful,
				FALSE otherwise.

	Purpose:	Creates a NodeClipViewController when the appropriate tag
				is read in from the file.
	Errors:		ERROR1 if a new node cannot be created,
				ERROR2 if pCXaraFileRecord is NULL,
				ERROR3 if we are passed a tag we do not recognise.
	See also:	

********************************************************************************************/
BOOL ClipViewRecordHandler::HandleRecord(CXaraFileRecord* pCXaraFileRecord)
{
	// validate inputs.
	ERROR2IF(pCXaraFileRecord == NULL, FALSE, "NULL parameter passed");

	// at the moment, we only handle the one tag.
	BOOL ok = TRUE;
	UINT32 Tag = pCXaraFileRecord->GetTag();
	switch (Tag)
	{
		case TAG_CLIPVIEWCONTROLLER:
		{
			NodeClipViewController* pNCVC = new NodeClipViewController;
			ERROR1IF(pNCVC == NULL, FALSE, _R(IDE_NOMORE_MEMORY));
			ok = InsertNode(pNCVC);
			break;
		}

		case TAG_CLIPVIEW:
		{
			NodeClipView* pNCV = new NodeClipView;
			ERROR1IF(pNCV == NULL, FALSE, _R(IDE_NOMORE_MEMORY));
			ok = InsertNode(pNCV);
			break;
		}

		default:
			ok = FALSE;
			ERROR3_PF(("ClipView file record handler - unrecognised tag: %d", Tag));
			break;
	}

	return ok;
}



//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------



/********************************************************************************************

>	UpdateCachedKeyholePathAction::UpdateCachedKeyholePathAction()

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20 March 2000
	Purpose:	Constructor.

********************************************************************************************/
UpdateCachedKeyholePathAction::UpdateCachedKeyholePathAction()
{
	m_pNCVC = NULL;
}



/********************************************************************************************

>	UpdateCachedKeyholePathAction::~UpdateCachedKeyholePathAction()

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20 March 2000
	Purpose:	Destructor.

********************************************************************************************/
UpdateCachedKeyholePathAction::~UpdateCachedKeyholePathAction()
{
	m_pNCVC = NULL;
}



/********************************************************************************************

>	ActionCode UpdateCachedKeyholePathAction::Init(	UndoableOperation* pUndoOp,
													ActionList* pActionList,
													NodeClipViewController* pUpdateNCVC )
	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20 March 2000
	Inputs:		pUndoOp			we must either be invoked from within an undoable operation,
								or must at least have a valid pointer to one.
				pActionList		the ActionList which the new Action will be put on. This will
								be either the undo or the redo action list.
				pUpdateNCVC		pointer to the NodeClipViewController which this Action will
								update.

	Outputs:	Puts a copy of this action at the head of the given action list. The upshot
				of this is that when that list, eg undo list, is executed, this action's
				Execute method will be called last (the lists go backwards(??))

	Returns:	AC_FAIL, AC_NORECORD or AC_OK. See Action::Init() in ops.cpp for more detail.

	Purpose:	Action Init function.
				This is the public interface through which the Action is invoked. It should
				call Action::Init to actually construct a new Action and put it on the given
				action list. It should usually also perform the action (we don't actually
				in this case, as this action is a BODGE so we update NCVC's on undo/redo!)
	Errors:		
	See also:	

********************************************************************************************/
ActionCode UpdateCachedKeyholePathAction::Init(	UndoableOperation* pUndoOp,
												ActionList* pActionList,
												NodeClipViewController* pUpdateNCVC )
{
	// call the base class to create and put the action onto the action list.
	ActionCode ac = AC_FAIL;
	UINT32 ActSize = sizeof(UpdateCachedKeyholePathAction);
	UpdateCachedKeyholePathAction* pNewAction = NULL;
	ac = Action::Init(	pUndoOp,
						pActionList,
						ActSize,
						CC_RUNTIME_CLASS(UpdateCachedKeyholePathAction),
						(Action**)&pNewAction  );

	// ok, the action now exists. initialise it with the NCVC to be updated,
	// and mark the NCVC as needing to update itself.
	if (ac != AC_FAIL && pNewAction != NULL)
	{
		pUpdateNCVC->MarkKeyholeInvalid();
		pNewAction->SetNCVCToUpdate(pUpdateNCVC);
	}

	// we want the update action to be called last whenever an action is undone or redone,
	// so it actually needs to go at the other end of the list.
	pActionList->RemoveItem(pNewAction);
	pActionList->AddHead(pNewAction);

	return ac;
}



/********************************************************************************************

>	ActionCode UpdateCachedKeyholePathAction::Execute()

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20 March 2000
	Inputs:		
	Outputs:	
	Returns:	
	Purpose:	Called by Camelot when it meets this Action on an undo/redo ActionList.
				You should undo/redo the action here and record an opposite action in
				the opposite ActionList. If the action is not too complex or you can't be
				bothered with two Actions, then you can use the same Action to both undo and
				redo, passing it appropriate information via Init(), or other means.

				This Action simply causes a specific NodeClipViewController to update its
				cached keyhole path.
	Errors:		
	See also:	

********************************************************************************************/
ActionCode UpdateCachedKeyholePathAction::Execute()
{
	// complain loudly if our state is invalid - however, we can survive without it,
	// so we'll still return a thumbs up anyway.
	ERROR3IF(m_pNCVC == NULL, "UpdateCachedKeyholePathAction::Execute; NULL NCVC pointer!");
	
	// record our 'inverse' action, and as a side effect of this, our NCVC
	// will be told that it needs to update it cached keyhole path.
	ActionCode ac = AC_OK;
	if (m_pNCVC != NULL)
	{
		ac = UpdateCachedKeyholePathAction::Init(	(UndoableOperation*)pOperation,
													pOppositeActLst,
													m_pNCVC );
	}
	return ac;
}



/********************************************************************************************

>	void UpdateCachedKeyholePathAction::SetNCVCToUpdate(NodeClipViewController* pNCVC)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20 March 2000
	Inputs:		pNCVC	ptr to the NCVC to update.
	Purpose:	Tell us which NCVC we will be updating.

********************************************************************************************/
void UpdateCachedKeyholePathAction::SetNCVCToUpdate(NodeClipViewController* pNCVC)
{
	ERROR3IF(pNCVC == NULL, "UpdateCachedKeyholePathAction::SetNCVCToUpdate; NULL param!");
	m_pNCVC = pNCVC;
}

