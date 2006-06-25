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
// NodeMouldGroup implementation. 

// Comments
//
// NodeMouldGroup is a node which lives inside NodeMould. Its purpose is
// to contain all mould source objects and thus by its simple structure allow
// messages to be passed to all mould shape producers without having to keep
// a list of who does what etc. Messages can be sent to the NodeMouldGroup
// which will then send them on to all children.

/*
*/


#include "camtypes.h"
#include "ndmldgrp.h"
#include "nodemldr.h"
//#include "mike.h"
//#include "mario.h"
//#include "tim.h"
#include "nodepath.h"
#include "nodebmp.h"
//#include "matrix.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "trans2d.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "nativeps.h"		// The old style EPS native filter, used in v1.1
#include "aw_eps.h"

#include "cxftags.h"
//#include "cxfdefs.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "cxfrec.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "camfiltr.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "webster.h"
#include "taglists.h"

#include "extender.h"

DECLARE_SOURCE("$Revision$");

CC_IMPLEMENT_DYNAMIC(NodeMouldGroup,NodeGroup )  

// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW


/*********************************************************************************************

>    NodeMouldGroup::NodeMouldGroup() 

     Author:	Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
     Created:	5/12/94
     Inputs:	
     Outputs:	
     Returns:	
     Purpose: 	This constructor creates a NodeMouldGroup linked to no other, with all status
		  	  	flags false and an uninitialised bounding rectangle.           
			
     Errors:    

**********************************************************************************************/
 
NodeMouldGroup::NodeMouldGroup(): NodeGroup()
{
}


/***********************************************************************************************

>	void NodeMouldGroup::NodeMouldGroup
	(
		Node* ContextNode,  
		AttachNodeDirection Direction,  
		BOOL Locked = FALSE, 
		BOOL Mangled = FALSE,  
		BOOL Marked = FALSE, 
		BOOL Selected = FALSE, 
	)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
    Created:	5/12/94
    Inputs: 	ContextNode: Pointer to a node which this node is to be attached to.     
    
				Direction: 
			
					Specifies the direction in which this node is to be attached to the 
					ContextNode. The values this variable can take are as follows: 
									  
					PREV      : Attach node as a previous sibling of the context node
					NEXT      : Attach node as a next sibling of the context node
					FIRSTCHILD: Attach node as the first child of the context node
					LASTCHILD : Attach node as a last child of the context node                               
					      
				The remaining inputs specify the status of the node: 
			
				Locked:     Is node locked ?
				Mangled:    Is node mangled ?
				Marked:     Is node marked ?
				Selected:   Is node selected ?
			
    Outputs: -
    Returns: - 
    Purpose: 	This method initialises the node and links it to ContextNode in the
			 	direction specified by Direction. All necessary tree links are
			 	updated.     
			 
    Errors:  	An assertion error will occur if ContextNode is NULL


***********************************************************************************************/

NodeMouldGroup::NodeMouldGroup(Node* ContextNode,  
					 AttachNodeDirection Direction,  
					 BOOL Locked, 
				 	 BOOL Mangled,  
					 BOOL Marked, 
					 BOOL Selected   
			   ):NodeGroup(ContextNode, Direction, Locked, Mangled, Marked, 
			 	Selected) 
{ 
} 


/*********************************************************************************************

>    NodeMouldGroup::~NodeMouldGroup() 

     Author:	Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
     Created:	5/11/94
     Inputs:	
     Outputs:	
     Returns:	
     Purpose: 	This destructor calls the parent class destructor

**********************************************************************************************/
 
NodeMouldGroup::~NodeMouldGroup()
{
}



/*******************************************************************************************

>	virtual String NodeMouldGroup::Describe(BOOL Plural, BOOL Verbose = TRUE)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/12/94
	Inputs:		Plural: Flag indicating if the string description should be plural or
						singular. 
	Outputs:	-
	Retuns:		Description of the mould group node 
	Purpose:	To return a description of the Mould object in either the singular or the 
				plural. This method is called by the DescribeRange method.
				
				The description will always begin with a lower case letter.   
				
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

String NodeMouldGroup::Describe(BOOL Plural, BOOL Verbose) 
{     
	if (Plural)
		return(String(_R(IDS_MOULDGROUP_DESCRP)));  
	else
		return(String(_R(IDS_MOULDGROUP_DESCRS))); 
}; 




/***********************************************************************************************

> Node* NodeMouldGroup::SimpleCopy()  

    Author:     Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
    Created:	5/12/94
	Inputs:     -  
    Outputs:    -
    Returns:    A copy of the node, or NULL if memory has run out 
		 
    Purpose:    This method returns a shallow copy of the node with all Node pointers NULL. 
				The function is virtual, and must be defined for all derived classes.  
	            
	Errors:     If memory runs out when trying to copy, then ERROR is called with an out of memory
				error and the function returns NULL.                                                                      
                                                                                 
**********************************************************************************************/

Node* NodeMouldGroup::SimpleCopy()
{
	NodeMouldGroup* pCopyOfNode = new NodeMouldGroup();
	ERROR1IF(pCopyOfNode == NULL,NULL,_R(IDE_NOMORE_MEMORY)); 
	CopyNodeContents(pCopyOfNode);
	return (pCopyOfNode);
}   


/***********************************************************************************************

>	void NodeMouldGroup::CopyNodeContents(NodeMouldGroup* pCopyOfNode)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/12/94
	Inputs:		pCopyOfNode - The node to copy data to
	Outputs:	-
	Returns:	-
	Purpose:	Copies the data from this node to pCopyOfNode by first calling the base class 
				to get it to copy its stuff, and then copying its own stuff
	Scope:		protected
	SeeAlso:	NodeGroup::CopyNodeContents

***********************************************************************************************/

void NodeMouldGroup::CopyNodeContents(NodeMouldGroup* pCopyOfNode)
{
	ERROR3IF(pCopyOfNode==NULL,"NodeMouldGroup::CopyNodeContents() passed a null node");
	if (pCopyOfNode==NULL)
		return;

	// just call the parent copy function for the mo.
	NodeGroup::CopyNodeContents(pCopyOfNode);
}


   
/***********************************************************************************************
>   void NodeMouldGroup::PolyCopyNodeContents(NodeRenderable* pNodeCopy)

    Author:     Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
    Created:    18/12/2003
    Outputs:    -
    Purpose:    Polymorphically copies the contents of this node to another
	Errors:     An assertion failure will occur if NodeCopy is NULL
    Scope:      protected
								     
***********************************************************************************************/

void NodeMouldGroup::PolyCopyNodeContents(NodeRenderable* pNodeCopy)
{
	ENSURE(pNodeCopy, "Trying to copy a node's contents into a NULL node");
	ENSURE(IS_A(pNodeCopy, NodeMouldGroup), "PolyCopyNodeContents given wrong dest node type");

	if (IS_A(pNodeCopy, NodeMouldGroup))
		CopyNodeContents((NodeMouldGroup*)pNodeCopy);
}



/********************************************************************************************

>	virtual UINT32 NodeMouldGroup::GetNodeSize() const

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/12/94
	Inputs:		-
	Outputs:	-
	Returns:	The size of the node in bytes
	Purpose:	For finding the size of the node 
				
	SeeAlso:	Node::GetSubtreeSize

********************************************************************************************/

UINT32 NodeMouldGroup::GetNodeSize() const 
{     
	return (sizeof(NodeMouldGroup)); 
}  


/********************************************************************************************

>	BOOL NodeMouldGroup::ShouldBeRendered()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/07/95
	Returns:	FALSE => this node should not be rendered.
	Purpose:	Indicate to the caller that this node should never be rendered.
				This is mainly used during printing where due to complications to do with
				banding and transparency, we cannot use NeedsToRender() to filter out
				such nodes, so we use this function instead.
	SeeAlso:	Node::ShouldBeRendered

********************************************************************************************/

BOOL NodeMouldGroup::ShouldBeRendered() const
{
	// Don't render the un-moulded objects!
	return FALSE;
}


/********************************************************************************************

>	DocRect NodeMouldGroup::GetBoundingRect(BOOL DontUseAttrs=FALSE, BOOL HitTest=FALSE)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/01/95
	Inputs:		DontUseAttrs - TRUE if you don't want to use the nodes attrs to calculate
				the bounding rect (defaults will be used). Defaults to FALSE.
				HitTest      - TRUE if being called during HitTest
	Returns:	A null rectangle
	Purpose:	Returns a completely null rectangle

********************************************************************************************/

DocRect NodeMouldGroup::GetBoundingRect(BOOL DontUseAttrs, BOOL HitTest)
{
	// a rect to put the new version of the bounding rect into
	DocRect Rect;
	return Rect;
}

/********************************************************************************************

>	DocRect NodeMouldGroup::GetBlobBoundingRect()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/01/95
	Returns:	DocRect - The bounding rect of the mould group and its blobs
	Purpose:	Returns a completely null rectangle

********************************************************************************************/

DocRect NodeMouldGroup::GetBlobBoundingRect()
{
	DocRect Rect;
	return Rect;
}


/********************************************************************************************

>	DocRect NodeMouldGroup::GetChildrensBounds(BOOL DontUseAttrs = FALSE)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/01/95
	Inputs:		DontUseAttrs - TRUE if you don't want to use the nodes attrs to calculate
				the bounding rect (defaults will be used). Defaults to FALSE.
	Returns:	A null rectangle
	Purpose:	Returns the bounding rect of this object. For internal use only

********************************************************************************************/

DocRect NodeMouldGroup::GetChildrensBounds(BOOL DontUseAttrs)
{
	return NodeRenderableBounded::GetBoundingRect(DontUseAttrs);
}




/********************************************************************************************

>	BOOL NodeMouldGroup::OnClick( DocCoord PointerPos, ClickType Click, ClickModifiers ClickMods,
						     Spread* pSpread )

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/12/94
	Inputs:		PointerPos 	- The Location of the mouse pointer at the time of the click
				Click 		- The type of click received (single, double, drag etc)
				ClickMods 	- The modifiers to the click (eg shift, control etc )
	Returns:	TRUE 	- if the node claims the click as its own
				FALSE 	- if it is not interested in the click
	Purpose:	Does nothing at the moment - Just returns FALSE.
		
********************************************************************************************/

BOOL NodeMouldGroup::OnClick( DocCoord PointerPos, ClickType Click, ClickModifiers ClickMods,
						Spread* pSpread )
{
	// we did not use the click, so let someone else try
	return FALSE;
}


/***********************************************************************************************

>	BOOL NodeMouldGroup::HidingNode()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/12/94
	Inputs:		-
	Outputs:	-
	Purpose:	Called whenever the node gets hidden. The function will call all base class
				derivations of this object and then all children connected to this object
				in the tree.

***********************************************************************************************/

BOOL NodeMouldGroup::HidingNode()
{
	// Call the base class first
	if (!NodeGroup::HidingNode())
		return FALSE;

	BOOL ok = TRUE;
/*
	Node* pNode = FindFirstChild();
	while (pNode != NULL && ok)
	{
		// send the hiding node message on to our children
		ok = pNode->HidingNode();
		pNode = pNode->FindNext();
	}
*/
	return ok;
}


/***********************************************************************************************

>	BOOL NodeMouldGroup::ShowingNode()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/12/94
	Inputs:		-
	Outputs:	-
	Purpose:	Called whenever the node gets shown after it's been hidden.

***********************************************************************************************/

BOOL NodeMouldGroup::ShowingNode()
{
	// Call the base class first
	if (!NodeGroup::ShowingNode())
		return FALSE;

	// and then all the child objects
	BOOL ok = TRUE;

/*
	Node* pNode = FindFirstChild();
	while (pNode != NULL && ok)
	{
		ok = pNode->ShowingNode();
		pNode = pNode->FindNext();
	}
*/
	return ok;
}


/********************************************************************************************

>	BOOL NodeMouldGroup::NeedsToRender(RenderRegion *pRender)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/01/95
	Inputs:		pRender - A pointer to the current render region (null if none)
	Returns:	TRUE  => This node should be rendered,
				FALSE => This node does not need to be rendered.
	Purpose:	Virtual function - this function will indicate to the caller whether or not
				we want to render the given node, according to the information passed in.
	SeeAlso:	Node::NeedsToRender

********************************************************************************************/

SubtreeRenderState NodeMouldGroup::RenderSubtree(RenderRegion* pRender, Node** ppNextNode, BOOL bClip)
{
	if (pRender && pRender->RenderPaperAsInk())
		return SUBTREE_ROOTANDCHILDREN;

	return SUBTREE_NORENDER;
}


/********************************************************************************************

>	void NodeMouldGroup::Transform( TransformBase& Trans )

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/01/95
	Inputs:		Trans - A transformation object
	Purpose:	Will allow certain transformations through to the original objects. So
				INT32 as the mould shape and source bbox are updated in the parent above
				us then all will be well.

********************************************************************************************/

void NodeMouldGroup::Transform( TransformBase& Trans )
{
	// there are only two types of transform that dont alter the
	// relationship of the bounding box with respect to its objects.
	// These are the only ones I can allow through otherwise the 
	// mould will shear.
	// Unfortunately Scales don't seem to work too well either
	// so we're gonna have to put up with simple translations only
	// ok I've commented this lot out for now until someone complains
	// about it... Moving the source objects about is a bit of a pain
	// as during undo the final position of the object is weird.
	// (See bug 4630 for instance)

	if (IS_A(&Trans, Trans2DMatrix))
	{
		Trans2DMatrix* t = (Trans2DMatrix*)(&Trans);
		Matrix m = t->GetMatrix();
		
		if (m.IsTranslation())
			NodeRenderableInk::TransformChildren(Trans);			
	}
}


/********************************************************************************************

>	BOOL NodeMouldGroup::NeedsToExport(RenderRegion* pRender, BOOL VisibleLayersOnly = FALSE,
									   BOOL CheckSelected = FALSE)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/03/95
	Inputs:		pRender - A pointer to the current export region
				VisibleLayersOnly - TRUE => remove nodes which are on invisible layers
								   - FALSE => export everything
				CheckSelected - TRUE => we check if object selected and only export selected bjects
							  - FALSE => we don't bother checking for selection or not
	Returns:	TRUE => please export me.
	Purpose:	Virtual function - this version will return FALSE for exporting to 
				ArtWorks or Camelot EPS render regions.

********************************************************************************************/

BOOL NodeMouldGroup::NeedsToExport(RenderRegion* pRender, BOOL VisibleLayersOnly, BOOL CheckSelected)
{
	if (pRender==NULL)
		return TRUE;

#if !defined(EXCLUDE_FROM_RALPH)
	// only export to native documents!
	if (pRender->GetRuntimeClass() == CC_RUNTIME_CLASS(NativeRenderRegion))
		return TRUE;
#endif	
	return FALSE;
}


/*********************************************************************************************

>    void NodeMouldGroup::PreExportRender(RenderRegion* pRegion)

     Author:	Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
     Created:	14/03/95
     Inputs:	pRegion = ptr to the export render region to export to
     Outputs:	
     Returns:	
     Purpose: 	Called before this node or any of its children have been rendered to the 
     			export region. This outputs the "start mould source objects" command.
				Supports ArtWorks EPS and Camelot EPS
     Errors:    

**********************************************************************************************/
 
void NodeMouldGroup::PreExportRender(RenderRegion* pRegion)
{
#if !defined(EXCLUDE_FROM_RALPH)
	if (pRegion->IS_KIND_OF(NativeRenderRegion))
	{
		PreExportNATEPS(pRegion);
		return;
	} 

	if (pRegion->IS_KIND_OF(ArtWorksEPSRenderRegion))
	{
		PreExportAWEPS(pRegion);
		return;
	}
#endif
}

/*********************************************************************************************

>    BOOL NodeMouldGroup::ExportRender(RenderRegion* pRegion)

     Author:	Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
     Created:	14/03/95
     Inputs:	pRegion = ptr to the export render region to export to
     Outputs:	
     Returns:	TRUE if ok, FALSE if something went wrong
     Purpose: 	Called after this node and all of its children have been rendered to the
     			export region. This outputs the "end mould source objs" command.
				Supports ArtWorks EPS and Camelot EPS
     Errors:    

**********************************************************************************************/
 
BOOL NodeMouldGroup::ExportRender(RenderRegion* pRegion) 
{
#if !defined(EXCLUDE_FROM_RALPH)
	if (pRegion->IS_KIND_OF(NativeRenderRegion))
		return PostExportNATEPS(pRegion);

	if (pRegion->IS_KIND_OF(ArtWorksEPSRenderRegion))
		return PostExportAWEPS(pRegion);
#endif	
	return FALSE;
}


/*********************************************************************************************

	BOOL NodeMouldGroup::Pre/Post ExportNATEPS(RenderRegion* pRegion)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/03/95
	Purpose:	Export delimiter tokens for this particular object.
				The format is defined as
					csmp
						path description
					cemp

**********************************************************************************************/

void NodeMouldGroup::PreExportNATEPS(RenderRegion* pRegion)
{
#if !defined(EXCLUDE_FROM_RALPH)
	EPSExportDC *pDC = (EPSExportDC *) pRegion->GetRenderDC();
	pDC->OutputToken(_T("csso"));			// Camelot "start mould source" token
	pDC->OutputNewLine();
#endif
}


BOOL NodeMouldGroup::PostExportNATEPS(RenderRegion* pRegion)
{
#if !defined(EXCLUDE_FROM_RALPH)
	EPSExportDC *pDC = (EPSExportDC *) pRegion->GetRenderDC();
	pDC->OutputToken(_T("ceso"));			// Camelot "end mould source" token
	pDC->OutputNewLine();
#endif
	return TRUE;
}


/*********************************************************************************************

	void NodeMouldGroup::Pre/Post ExportAWEPS(RenderRegion* pRegion)

    Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
    Created:	14/03/95

**********************************************************************************************/

void NodeMouldGroup::PreExportAWEPS(RenderRegion* pRegion)
{
}

BOOL NodeMouldGroup::PostExportAWEPS(RenderRegion* pRegion)
{
	return TRUE;
}


/********************************************************************************************

>	virtual ChangeCode NodeMouldGroup::OnChildChange(ObjChangeParam* pParam)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/06/95
	Inputs:		pParam	= pointer to a object change parameter class
	Returns:	CC_OK		if we have successfully processed the change.
				CC_FAIL		if we cannot handle this particular change and must prevent the
							child from continuing
	Purpose:	This function should be overridden in derived object classes.
				Composite objects can use this function to respond to one of their children
				undergoing a change. They should return CC_FAIL whenever they are unable to
				cope with the change.

	SeeAlso:	WarnParentOfChange(),AllowOp();

********************************************************************************************/

ChangeCode NodeMouldGroup::OnChildChange(ObjChangeParam* pParam)
{
	return CC_OK;
}

/********************************************************************************************

>	virtual BOOL NodeMouldGroup::WritePreChildrenWeb(BaseCamelotFilter* pFilter)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/7/96
	Inputs:		pFilter = ptr to the filter to write to
	Returns:	TRUE is a record has been written, FALSE otherwise
	Purpose:	Writes out a records associated with this node

				It either writes out an envelope or perspective record, depending on the geometry.

				The only piece of data that needs to be saved out with a node mould is the
				error threshold.

********************************************************************************************/

BOOL NodeMouldGroup::WritePreChildrenWeb(BaseCamelotFilter* pFilter)
{
	ERROR2IF(pFilter == NULL,FALSE,"NULL filter param");

	// WEBSTER - markn 10/2/97
	// Mould bounds record

	BOOL ok = TRUE;

	// Add a description of the TAG_MOULD_BOUNDS record, for older importers that don't understand this record
	pFilter->AddTagDescription(TAG_MOULD_BOUNDS,_R(IDS_TAG_MOULD_BOUNDS));

	// Write out the bounds record
	DocRect Rect = GetChildrensBounds(FALSE);
	
	CamelotFileRecord BoundsRec(pFilter,TAG_MOULD_BOUNDS,TAG_MOULD_BOUNDS_SIZE);

	ok = BoundsRec.Init();

	if (ok) ok = BoundsRec.WriteCoord(Rect.lo);
	if (ok) ok = BoundsRec.WriteCoord(Rect.hi);
	if (ok) ok = pFilter->Write(&BoundsRec);

	// Write out the actual mould group record
	CXaraFileRecord Rec(TAG_MOULD_GROUP,TAG_MOULD_GROUP_SIZE);

	if (ok) ok = pFilter->Write(&Rec);

	if (!ok)
		pFilter->GotError(_R(IDE_FILE_WRITE_ERROR));

	return ok;
}

//------------------------------------------------------------------
// See comments for NodeMouldGroup::WritePreChildrenWeb(BaseCamelotFilter* pFilter)
BOOL NodeMouldGroup::WritePreChildrenNative(BaseCamelotFilter* pFilter)
{
	return WritePreChildrenWeb(pFilter);
}

