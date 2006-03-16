// $Id: nodeclip.cpp 662 2006-03-14 21:31:49Z alex $
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
// Implementation of NodeClipView.
//

#include "camtypes.h"	// pre-compiled header
#include "nodeclip.h"	// header file
//#include "clipres.h"	// ClipView resources

#include "cxfrec.h"		// save/load
#include "cxftags.h"	//
#include "camfiltr.h"	//

//#include "mario.h"		// for _R(IDE_NOMORE_MEMORY)

DECLARE_SOURCE("$Revision: 662 $");

// dynamic class creation stuff.
CC_IMPLEMENT_DYNCREATE(NodeClipView, NodeRenderableInk)

// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW

/********************************************************************************************

>	NodeClipView::NodeClipView()

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10 February 2000
	Purpose:	Default constructor.
	Errors:	
	See also:	

********************************************************************************************/
NodeClipView::NodeClipView() : NodeRenderableInk()
{
	// member variable initialisation.
	m_pRegion	= NULL;
	m_pContext	= NULL;
	m_bRenderingForward = FALSE;
	m_pSavedRegion	= NULL;
	m_bGDrawClipRegionSet = FALSE;

	m_ClipRegionAttribute.SetResponsibleForGrouping (FALSE);
}



/********************************************************************************************

>	NodeClipView::NodeClipView(	Node* pContextNode,
								AttachNodeDirection Direction,
								BOOL Locked		= FALSE,
								BOOL Mangled	= FALSE,
								BOOL Marked		= FALSE,
								BOOL Selected	= FALSE )
	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10 February 2000
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
NodeClipView::NodeClipView(	Node* pContextNode,  
							AttachNodeDirection Direction,  
							BOOL Locked, 
							BOOL Mangled,  
							BOOL Marked, 
							BOOL Selected )
			: NodeRenderableInk(pContextNode, Direction, Locked, Mangled, Marked, Selected) 
{
	// member variable initialisation.
	m_pRegion	= NULL;
	m_pContext	= NULL;
	m_bRenderingForward = FALSE;
	m_pSavedRegion	= NULL;
	m_bGDrawClipRegionSet = FALSE;

	m_ClipRegionAttribute.SetResponsibleForGrouping (FALSE);
}



/********************************************************************************************

>	NodeClipView::~NodeClipView()

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10 February 2000
	Purpose:	Destructor.
	Errors:	
	See also:	

********************************************************************************************/
NodeClipView::~NodeClipView()
{
	// delete and clear member variables.
	if (m_pRegion	!= NULL)
		delete [] m_pRegion; m_pRegion = NULL;
	if (m_pContext	!= NULL)
		delete [] ((BYTE *)m_pContext); m_pContext = NULL;
}



/********************************************************************************************

>	virtual DocRect NodeClipView::GetBoundingRect(BOOL DontUseAttrs, BOOL HitTest)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10 February 2000
	Inputs:		DontUseAttrs	TRUE of you want to ignore all the node's attributes.
				HitTest			TRUE if being called during a hit-test.
	Returns:	The bounding rectangle of this node.

	Purpose:	Find this node's bounding rectangle. If the rectangle is known to be valid 
				then it is simply returned. If IsBoundingRectValid is FALSE then the rect 
				is recalculated before it is returned and the validity flag reset.
	See also:	

********************************************************************************************/
DocRect NodeClipView::GetBoundingRect(BOOL DontUseAttrs, BOOL HitTest)
{
	// we want our Render() fuction to be called whenever that of our parent NCVC is called,
	// therefore we need the same size bounding rect.
	Node* pNCVC = FindParent();
	if (pNCVC != NULL && pNCVC->IsANodeClipViewController())
		return ((NodeRenderableInk*)pNCVC)->GetBoundingRect(DontUseAttrs, HitTest);
	else
		return DocRect(0, 0, 0, 0);
}



/********************************************************************************************

>	virtual DocRect NodeClipView::GetBlobBoundingRect()

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10 February 2000
	Returns:	The bounding rectangle of this node with its blobs drawn.
	Purpose:	Get this node's bounding rectangle when its blobs are drawn.
	Errors:	
	See also:	GetBoundingRect().

********************************************************************************************/
DocRect NodeClipView::GetBlobBoundingRect()
{
	// TODO:
	//	need to investigate exactly when this method is used, as I don't think I should 
	//	implement it for an invisible node, but I'm not sure...

	return DocRect(0, 0, 0, 0);
}



/********************************************************************************************

>	BOOL NodeClipView::NeedsToRender(RenderRegion* pRender)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05 May 2000
	Inputs:		pRender		the render-region we're rendering into.
	Returns:	TRUE always.

	Purpose:	Whether or not we need to render. Most NodeRenderableBounded's do a check
				on intersection with the render-region's clip-rect here, however we may
				sometimes need to render when we *don't* intersect the clip-rect, but one of 
				our right-siblings does. So we'll just return TRUE from this whatever.

	Notes:		In future it may pay to test the bounds of all our right-siblings, however
				what if there are nested ClipView's? I get the feeling that not rendering
				in that case would probably screw things up for on of our multi-clipped 
				grand-children.

********************************************************************************************/

SubtreeRenderState NodeClipView::RenderSubtree(RenderRegion* pRender, Node** ppNextNode, BOOL bClip)
{
	return SUBTREE_ROOTANDCHILDREN;
}



/********************************************************************************************

>	virtual void NodeClipView::Render(RenderRegion* pRender)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10 February 2000
	Inputs:		pRender		points to a render region to render into.

	Purpose:	Render method. See NodeClipViewController::Render() for complete details
				on what should happen when.

				If we think we're in a forward rendering loop, then set GDraw's clipping
				region by rendering our ClipRegionAttribute.

				If we think we're in a backward rendering loop, then do nothing.
	Errors:		
	See also:	NodeClipViewController::Render()

********************************************************************************************/
void NodeClipView::Render(RenderRegion* pRender)
{
// DEBUG
//	TRACEUSER( "Karim", _T("NCV::Render; Tag %d\n"), Tag);

	// In the hit-test render-loop we do *not*
	// want to directly cause any clipping. at all other times, we render away.
//#pragma message( __LOCMSG__ "Removed RenderRegion usage" )
	if (!pRender->IsHitDetect())
		SetGDrawClippingRegion(pRender);
}



/********************************************************************************************

>	void RenderClipAttr(RenderRegion* pRender)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13 April 2000
	Inputs:		
	Outputs:	
	Returns:	
	Purpose:	
	Errors:		
	See also:	

********************************************************************************************/
void NodeClipView::RenderClipAttr(RenderRegion* pRender)
{
	m_ClipRegionAttribute.Render(pRender);
}



/********************************************************************************************

>	void NodeClipView::SetGDrawClippingRegion()

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/02/2000
	Inputs:		pRender		points to the region which we shall be clipping inside.
	Outputs:	GDraw's current clipping region may have been changed by this method.
	Returns:	
	Purpose:	Work out the path of my keyhole node, save GDraw's rendering context and
				tell GDraw to clip to that path.

	Errors:		ERROR3 if pKeyhole cannot be converted to paths.
				In release builds, we just quit, with no clipping.
	See also:	RestoreGDrawClippingRegion()

********************************************************************************************/
void NodeClipView::SetGDrawClippingRegion(RenderRegion* pRender)
{
	m_ClipRegionAttribute.Render(pRender);
}



/********************************************************************************************

>	BOOL NodeClipView::RestoreGDrawClippingRegion(BOOL fFirstInRenderLoop = FALSE)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/02/2000
	Inputs:		fFirstInRenderLoop		whether or not we are being called at the start of a
										ClipView render-process.
	Returns:	TRUE if we restored the region and did it successfully,
				FALSE otherwise.
	Purpose:	If GDraw's clipping region has been set but not yet restored, then attempt
				to restore it from the copy we made when it was set.

	Errors:		
	See also:	SetGDrawClippingRegion()

********************************************************************************************/
BOOL NodeClipView::RestoreGDrawClippingRegion(BOOL fFirstInRenderLoop)
{
	return TRUE;
}



/********************************************************************************************

>	virtual void NodeClipView::RenderEorDrag(RenderRegion* pRender)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10 February 2000
	Inputs:		pRender		pointer to the render region to render into.
	Purpose:	Render this node as eor-blobs into the given render region.

	Errors:	
	See also:	See Render() for info.

********************************************************************************************/
void NodeClipView::RenderEorDrag(RenderRegion* pRender)
{
}



/********************************************************************************************

>	virtual String NodeClipView::Describe(BOOL Plural, BOOL Verbose)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11 April 2000
	Inputs:		Plural		whether to pluralise the description.
				Verbose		short or long version.
	Returns:	A string description of this node.
	Purpose:	Get a string description, for use in menus and infobar etc.
	Errors:		
	See also:	

********************************************************************************************/
String NodeClipView::Describe(BOOL Plural, BOOL Verbose)
{
	if (Plural)
		return(String(_R(IDS_CLIPVIEW_DESCRP)));
	else
		return(String(_R(IDS_CLIPVIEW_DESCRS)));
}



/********************************************************************************************

>	virtual UINT32 NodeClipView::GetNodeSize() const 

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10 February 2000
	Returns:	The size of the node in bytes.
	Purpose:	Obtain the size of a NodeClipView object.

	See also:	Node::GetSubtreeSize

********************************************************************************************/
UINT32 NodeClipView::GetNodeSize() const 
{
	return sizeof(NodeClipView);
}



/********************************************************************************************

>	void NodeClipView::Transform(TransformBase &Trans)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10 February 2000
	Inputs:		Trans	non-const reference to a description of the transformation.
	Outputs:	This node will be transformed appropriately.
	Purpose:	Perform a transformation on this node.
	Errors:		
	See also:	

********************************************************************************************/
void NodeClipView::Transform(TransformBase &Trans)
{
	// leave it up to the base class...
	NodeRenderableInk::Transform(Trans);
}



/********************************************************************************************

>	BOOL NodeClipView::WritePreChildrenWeb(BaseCamelotFilter* pFilter)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10 February 2000
	Inputs:		pFilter		pointer to a camelot file filter.

	Returns:	TRUE if successful,
				FALSE otherwise.
	Purpose:	Writes this node out to a camelot document.
	Errors:		ERROR2 if pFilter is NULL.
	See also:	

********************************************************************************************/
BOOL NodeClipView::WritePreChildrenWeb(BaseCamelotFilter* pFilter)
{
	// validate input.
	ERROR2IF(pFilter == NULL, FALSE, "NULL parameter");
	
	CXaraFileRecord Rec(TAG_CLIPVIEW, TAG_CLIPVIEW_SIZE);

	BOOL	ok = Rec.Init();
	if (ok)	ok = (pFilter->Write(&Rec) != 0);

	return ok;
}



/********************************************************************************************

>	BOOL NodeClipView::WritePreChildrenNative(BaseCamelotFilter* pFilter)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10 February 2000
	Inputs:		pFilter		pointer to a camelot file filter.

	Returns:	TRUE if successful,
				FALSE otherwise.
	Purpose:	Writes this node out to a camelot document.
	Errors:		
	See also:	

********************************************************************************************/
BOOL NodeClipView::WritePreChildrenNative(BaseCamelotFilter* pFilter)
{
	return WritePreChildrenWeb(pFilter);
}



/********************************************************************************************

>	BOOL NodeClipView::PostImport()

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10 February 2000
	Returns:	TRUE if success,
				FALSE if unsuccessful.
	Purpose:	Performs any necessary post-processing once the object has been read in
				from a file.
	Errors:		
	See also:	

********************************************************************************************/
BOOL NodeClipView::PostImport()
{
	// re-initialise our pointer to our keyhole node.
	return TRUE;
}



/********************************************************************************************

>	virtual Node* NodeClipView::SimpleCopy()

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10 February 2000
	Returns:	A copy of this node, or
				NULL if unsuccessful.
	Purpose:	Copy this node.
	Errors:		ERROR1 returning NULL if we couldn't allocate memory for the new node.
	See also:	Node::SimpleCopy()

********************************************************************************************/
Node* NodeClipView::SimpleCopy()
{
	NodeClipView* pNodeCopy = new NodeClipView;
	ERROR1IF(pNodeCopy == NULL, NULL, _R(IDE_NOMORE_MEMORY));
	CopyNodeContents(pNodeCopy);

	return pNodeCopy;
}



/********************************************************************************************

>	virtual void NodeClipView::CopyNodeContents(NodeClipView* pNodeCopy)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10 February 2000
	Inputs:		pNodeCopy	the node to copy our contents into.
	Purpose:	Copy this node's contents into pNodeCopy.
	Errors:		
	See also:	Node::CopyNodeContents()

********************************************************************************************/
void NodeClipView::CopyNodeContents(NodeClipView* pNodeCopy)
{
	// call base-class implementation; this will also perform necessary validation for us.
	NodeRenderableInk::CopyNodeContents(pNodeCopy);

	// TODO:
	//	Any other relevant copying.
}



/***********************************************************************************************
>   void NodeClipView::PolyCopyNodeContents(NodeRenderable* pNodeCopy)

    Author:     Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
    Created:    18/12/2003
    Outputs:    -
    Purpose:    Polymorphically copies the contents of this node to another
	Errors:     An assertion failure will occur if NodeCopy is NULL
    Scope:      protected
								     
***********************************************************************************************/

void NodeClipView::PolyCopyNodeContents(NodeRenderable* pNodeCopy)
{
	ENSURE(pNodeCopy, "Trying to copy a node's contents into a NULL node");
	ENSURE(IS_A(pNodeCopy, NodeClipView), "PolyCopyNodeContents given wrong dest node type");

	if (IS_A(pNodeCopy, NodeClipView))
		CopyNodeContents((NodeClipView*)pNodeCopy);
}



