// $Id$
// NodeBlendPath implementation. This file controls the tree node 'mouldpath'
// which is the shape of the mould for perspective and envelope shapes.
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

/*
*/

#include "camtypes.h"
#include "blobs.h"
#include "rndrgn.h"
#include "app.h"
#include "nodeblnd.h"
#include "objchge.h"
#include "undoop.h"
#include "ndbldpth.h"
#include "tool.h"
//#include "ngcore.h"		// NameGallery, for stretching functionality

//#include "aw_eps.h"
//#include "nativeps.h"		// The old style EPS native filter, used in v1.1
//#include "app.h"

#include "cxftags.h"
#include "cxfrec.h"
#include "camfiltr.h"
//#include "tim.h"
#include "becomea.h"

#include "ophist.h"


DECLARE_SOURCE("$Revision: 662 $");

CC_IMPLEMENT_DYNCREATE(NodeBlendPath,NodePath);
CC_IMPLEMENT_DYNCREATE(InitNodeBlendPathAction,Action);

// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW

// Uncomment this #define out if you want the code to cache the points it reads from the path
// in the function GetPointAtDistance()
// The reason it's been taken out is because the CMap<> template class seems to clash with the
// CAM_DEBUG_NEW mechanism, resulting in potential memory corruption.
#define NBP_CACHEPOINTATDISTANCE

/*********************************************************************************************

>    NodeBlendPath::NodeBlendPath() 

     Author:	Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
     Created:	29/4/99
     Purpose: 	This constructor creates a NodeBlendPath linked to no other, with all status
		  	  	flags false and an uninitialised bounding rectangle.           

**********************************************************************************************/
 
NodeBlendPath::NodeBlendPath(): NodePath()
{
	m_pCachedPointsAtDistance = NULL;
	// Reuse the "Initialise cache" call
	DestroyCachedInformation();
	m_Index = 0;
}

/***********************************************************************************************

>	void NodeBlendPath::NodeBlendPath
	(
		Node* ContextNode,  
		AttachNodeDirection Direction,  
		BOOL Locked = FALSE, 
		BOOL Mangled = FALSE,  
		BOOL Marked = FALSE, 
		BOOL Selected = FALSE, 
	)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
    Created:	29/4/99
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

NodeBlendPath::NodeBlendPath(Node* ContextNode,  
					 		 AttachNodeDirection Direction,  
					 		 BOOL Locked, 
				 	 		 BOOL Mangled,  
					 		 BOOL Marked, 
							 BOOL Selected   
			   ):NodePath(ContextNode, Direction, Locked, Mangled, Marked, Selected) 
{ 
	m_pCachedPointsAtDistance = NULL;
	// Reuse the "Destroy cache" call
	DestroyCachedInformation();
	m_Index = 0;
} 

/*********************************************************************************************

>    NodeBlendPath::~NodeBlendPath() 

     Author:	Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
     Created:	29/4/99
     Purpose: 	Destructor for Node MouldPath. This does nothing at present.

**********************************************************************************************/
 
NodeBlendPath::~NodeBlendPath()
{
	// Reuse the "Destroy cache" call
	DestroyCachedInformation();
}


/***********************************************************************************************

>	virtual Node* NodeBlendPath::SimpleCopy()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/4/99
	Returns:	Pointer to a Node
	Purpose:	Makes a copy of all the data in the node

***********************************************************************************************/

Node* NodeBlendPath::SimpleCopy()
{
	NodeBlendPath* NodeCopy = new NodeBlendPath();
    if (NodeCopy)
    	CopyNodeContents(NodeCopy);
    
	return NodeCopy;
}            


/***********************************************************************************************

>	void NodeBlendPath::CopyNodeContents(NodeBlendPath* NodeCopy)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/4/99
	Inputs:		NodeCopy - The node to copy
	Purpose:	Copies the data in the node by first calling the base class to get it to
				copy its stuff, and then copying its own stuff
	Scope:		protected
	SeeAlso:	NodeRenderableInk::CopyNodeContents

***********************************************************************************************/

void NodeBlendPath::CopyNodeContents( NodeBlendPath* NodeCopy)
{
	NodePath::CopyNodeContents( NodeCopy );
	//Copy contents specific to derived class here
}

/***********************************************************************************************

>	BOOL NodeBlendPath::CopyNodePath(NodePath* pNodePathToCopy)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/4/99
	Inputs:		pNodePathToCopy
	Purpose:	Copies the data in the node by first calling the base class to get it to
				copy its stuff, and then copying its own stuff
	Scope:		protected
	SeeAlso:	NodeRenderableInk::CopyNodeContents

***********************************************************************************************/

BOOL NodeBlendPath::CopyNodePath(NodePath* pNodePathToCopy)
{
	ERROR2IF(pNodePathToCopy == NULL,FALSE,"NULL NodePath ptr");

	BOOL    ok = SetUpPath();
	if (ok) ok = InkPath.MergeTwoPaths(pNodePathToCopy->InkPath);

	return ok;
}



/***********************************************************************************************
>   void NodeBlendPath::PolyCopyNodeContents(NodeRenderable* pNodeCopy)

    Author:     Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
    Created:    18/12/2003
    Outputs:    -
    Purpose:    Polymorphically copies the contents of this node to another
	Errors:     An assertion failure will occur if NodeCopy is NULL
    Scope:      protected
								     
***********************************************************************************************/

void NodeBlendPath::PolyCopyNodeContents(NodeRenderable* pNodeCopy)
{
	ENSURE(pNodeCopy, "Trying to copy a node's contents into a NULL node");
	ENSURE(IS_A(pNodeCopy, NodeBlendPath), "PolyCopyNodeContents given wrong dest node type");

	if (IS_A(pNodeCopy, NodeBlendPath))
		CopyNodeContents((NodeBlendPath*)pNodeCopy);
}



/***********************************************************************************************

>	virtual void NodeBlendPath::Render(RenderRegion* pRRegion)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/4/99  
	Inputs:		Pointer to a render region
	Purpose:	Overrides inherited function to do nothing.
				

***********************************************************************************************/

void NodeBlendPath::Render(RenderRegion* pRender)
{    
	
	//if (!InsideBlend())
		NodePath::Render(pRender);

}  


/********************************************************************************************

>	virtual void NodeBlendPath::RenderEorDrag( RenderRegion* pRender )

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/4/99
	Inputs:		pRender - A Pointer to the current RenderRegion
	Purpose:	Overrides inherited function to do nothing.

********************************************************************************************/

void NodeBlendPath::RenderEorDrag( RenderRegion* pRender )
{
	//if (!InsideBlend())
	BlobManager* pBlobMan = GetApplication()->GetBlobManager();
	BlobStyle CurrBlobs = pBlobMan->GetCurrentInterest();
	if (CurrBlobs.Object)
		NodePath::RenderEorDrag(pRender);
}


/********************************************************************************************

>	virtual void NodeBlendPath::RenderObjectBlobs(RenderRegion* pRender)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/4/99
	Inputs:		pRender - The region to render the blobs into
	Purpose:	Draws the paths object blobs into the render region supplied

********************************************************************************************/

void NodeBlendPath::RenderObjectBlobs(RenderRegion* pRender)
{
//	if (Tool::GetCurrentID() == TOOLID_BEZTOOL)
		NodePath::RenderObjectBlobs(pRender);
//	else
//	{
		// set our eor colour.
//		pRender->SetLineColour(BLACK);
		// render the path
//		pRender->DrawPath(&InkPath);
		// call the parent class render function
//		NodePath::RenderObjectBlobs(pRender);
//	}
}


void NodeBlendPath::RenderBlendBlobs(RenderRegion* pRender)
{
	NodePath::RenderObjectBlobs(pRender);
}


/********************************************************************************************

>	DocRect NodeBlendPath::GetBoundingRect(BOOL DontUseAttrs=FALSE, BOOL HitTest=FALSE)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/4/99
	Inputs:		DontUseAttrs - TRUE if you don't want to use the nodes attrs to calculate
				the bounding rect (defaults will be used). Defaults to FALSE.
				HitTest      - TRUE if being called during HitTest
	Returns:	A null rectangle
	Purpose:	Returns a completely null rectangle

********************************************************************************************/

DocRect NodeBlendPath::GetBoundingRect(BOOL DontUseAttrs, BOOL HitTest)
{
	/* Tricky problems:
	   (1) When importing, the importer translates the imported picture to centre around
	       the drop point. Unfortunately it calls us to find our bbox. We don't have a
		   bbox until the moulder nodes are built, which is not until PostImport time so
		   the importer does a Translate = (Drop - Center) which is X = (0 - C), BAD.
		   So we must return a bbox if their is no moulder in the mould! Nasty Showstopper fix	*/
	
/*	if (InsideBlend())
	{
		// if there is a moulder node to contribute a bbox
		if (FindNext(CC_RUNTIME_CLASS(NodeMoulder)))
		{
			// Then we dont contribute anything!
			DocRect Rect;
			return Rect;
		}
	}
*/
	// Otherwise we do.
	return NodePath::GetBoundingRect(DontUseAttrs);
}


/********************************************************************************************

>	BOOL NodeBlendPath::InsideBlend()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/4/99
	Inputs:		-
	Returns:	TRUE if this blend path is inside a blend object ie its immediate parent
				is a blend object
	Purpose:	Determins whether the immediate parent of this node blend path object is
				a blend object.

********************************************************************************************/

BOOL NodeBlendPath::InsideBlend()
{
	Node* p = FindParent();
	if (p)
	{
		if (IS_A(p, NodeBlend))
			return TRUE;
		if (IS_A(p, NodeHidden))
			return TRUE;
	}
	return FALSE;
}



/*********************************************************************************************

>    BOOL NodeBlendPath::ExportRender(RenderRegion* pRegion)

     Author:	Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
     Created:	29/4/99
     Inputs:	pRegion = ptr to the export render region to export to
     Outputs:	
     Returns:	TRUE if ok, FALSE if something went wrong
     Purpose: 	Called after this node and all of its children have been rendered to the
     			export region. This outputs the "end mould" command.
				Supports ArtWorks EPS and Camelot EPS
     Errors:    

**********************************************************************************************/
 
BOOL NodeBlendPath::ExportRender(RenderRegion* pRegion) 
{
/*#ifdef DO_EXPORT
	if (pRegion->IS_KIND_OF(NativeRenderRegion))
		return ExportCAMEPS(pRegion);

	if (pRegion->IS_KIND_OF(ArtWorksEPSRenderRegion))
		return ExportAWEPS(pRegion);
#endif	
*/
	return FALSE;
}


/*********************************************************************************************

	BOOL NodeBlendPath::ExportCAMEPS(RenderRegion* pRegion)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/4/99
	Inputs:		pRegion = a pointer to a Camelot EPS render region
	Returns:	TRUE if we've rendered the object
				FALSE if we want the caller to render the object
	Purpose:	Export delimiter tokens for this particular object.
				The format is defined as
					csmp
						path description
					cemp

**********************************************************************************************/

BOOL NodeBlendPath::ExportCAMEPS(RenderRegion* pRegion)
{
/*#ifdef DO_EXPORT
	EPSExportDC *pDC = (EPSExportDC *) pRegion->GetRenderDC();

	// If we're hanging around in a mould then export the correct tokens
	if (!InsideBlend())
	{
		NodePath::Render(pRegion);
	}
	else
	{
		pDC->OutputToken("csmp");			// Camelot "start mould path" token
		pDC->OutputNewLine();

		// Call the parent class
		NodePath::Render(pRegion);

		pDC->OutputToken("cemp");			// Camelot "start mould path" token
		pDC->OutputNewLine();
	}
#endif
*/
	return TRUE;
}




/*********************************************************************************************

	BOOL NodeBlendPath::ExportAWEPS(RenderRegion* pRegion)

    Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/4/99
	Inputs:		pRegion = a pointer to a Camelot EPS render region
	Returns:	TRUE if we've rendered the object
				FALSE if we want the caller to render the object
	Purpose:	Overrides the standard call to Obj->Render so we can render the path
				as we would like

**********************************************************************************************/

BOOL NodeBlendPath::ExportAWEPS(RenderRegion* pRegion)
{
	// just return false for the moment until we write our
	// proper random AWEPS path render function
	return FALSE;
}


/********************************************************************************************

>	virtual BOOL NodeBlendPath::AllowOp(ObjChangeParam* pParam,
										BOOL SetOpPermissionState,
										BOOL DoPreTriggerEdit = TRUE)

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com> 20/01/2000
	Created:	29/4/99
	Inputs:		pParam - pointer to the change parameter object
				SetOpPermissionState - TRUE to set the Nodes permission flags
				DoPreTriggerEdit	 - if TRUE then calls NameGallery::PreTriggerEdit.
									   *Must* be TRUE if the calling Op may make any nodes
									   change their bounds, eg move, line width, cut.
									   Use TRUE if unsure.

	Returns:	TRUE if the operation can proceede, FALSE to stop it
	Purpose:	To get node paths inside blend objects to reformat themselves correctly
				when their paths change.
	Also see:	NodeBlendPath::OnEdited()

********************************************************************************************/

BOOL NodeBlendPath::AllowOp(ObjChangeParam* pParam, BOOL SetOpPermissionState,
													BOOL DoPreTriggerEdit)
{
	BOOL ok = TRUE;

	if (SetOpPermissionState)
		ok = OnEdited(pParam);

	if (ok) ok = NodePath::AllowOp(pParam,SetOpPermissionState,DoPreTriggerEdit);

	return ok;
}


/********************************************************************************************

>	virtual BOOL NodeBlendPath::OnEdited(ObjChangeParam* pParam)

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/4/99
	Inputs:		pParam - pointer to the change parameter object
	Returns:	TRUE if ok, FALSE if not
	Purpose:	Informs the blend path it has been edited.

				It records an action that destroys the cached information held by
				this node.
	Also see:	NodeBlendPath::AllowOp()

********************************************************************************************/

BOOL NodeBlendPath::OnEdited(ObjChangeParam* pParam)
{
	ERROR2IF(pParam == NULL,FALSE,"Null param ptr");

	BOOL ok = TRUE;

	// Always destroy the cached info whenever anyone's doing something to this node.
	// When an undoable operation is being performed, this is done by the InitNodeBlendPathAction class
	UndoableOperation* pOp=pParam->GetOpPointer();
	if (pOp)
		ok = (InitNodeBlendPathAction::Init(pOp,pOp->GetUndoActionList(),this)!=AC_FAIL);
	else
		DestroyCachedInformation();

	return ok;
}



/********************************************************************************************

>	virtual BOOL NodeBlendPath::DoBecomeA(BecomeA* pBecomeA)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/10/00
	Inputs:		pBecomeA = 	ptr to info class containing everything a node needs to be able
							to become something else
	Outputs:	-
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	Actually tries to change the node into a different node type.
				This responds to BECOMEA_PASSBACK reason by making a duplicate NodeBlendPath and passing
				it to the pfnPassBack func provided in the pBecomeA class.
				This responds to BECOMEA_REPLACE by making a duplicate NodePath (not NodeBlendPath)
				and inserting into the tree.
	Errors:		-
	SeeAlso:	NodePath::DoBecomeA()

********************************************************************************************/

BOOL NodeBlendPath::DoBecomeA(BecomeA* pBecomeA)
{
	// Check for a NULL entry param
	ERROR2IF_PF(pBecomeA == NULL,FALSE,("pBecomeA is NULL"));

	// This lump checks that the Reason is one that we understand
	// It also makes sure that we don't have a NULL UndoOp ptr
	BOOL ValidReason = (pBecomeA->GetReason() == BECOMEA_REPLACE || pBecomeA->GetReason() == BECOMEA_PASSBACK);
	ERROR2IF_PF(!ValidReason,FALSE,("Unkown BecomeA reason %d",pBecomeA->GetReason()));

	BOOL Success = TRUE;			// Our success flag (Important that this defaults to TRUE)
	NodePath* pNewNodePath = NULL;	// Ptr to a new NodePath, if we get to make one.
	
	if (pBecomeA->BAPath())
	{
		switch (pBecomeA->GetReason())
		{
			case BECOMEA_REPLACE:
			{
				// we need to be a NodePath ....

				// Make a copy of the NodePath of this NodeBlendPath
				CALL_WITH_FAIL(((pNewNodePath = (NodePath*)NodePath::SimpleCopy ()) != NULL), pBecomeA->GetUndoOp(), Success);
				
				if (Success)
				{
					// MUST now copy our children since we need to copy any attributes the path has appllied to it
					CopyChildAttrs (pNewNodePath);
					
					UndoableOperation* pUndoOp = pBecomeA->GetUndoOp();
					// NOT sure if its valid to not have one of these (seems to be assumed in other places)
					// so for now this ERROR2 will fire if there isn't one ....
//					ERROR2IF(pUndoOp == NULL, FALSE, "No op pointer in NodeBlendPath::DoBecomeA");

					if (pUndoOp)
					{
						// hide the text story
						NodeHidden* pNodeHidden;
						if (pUndoOp->DoHideNode(this,FALSE,&pNodeHidden)==FALSE)
							Success = FALSE;
						// Insert the NodeGroup next to the old hidden TextStory
						if (!pUndoOp->DoInsertNewNode(pNewNodePath,pNodeHidden,NEXT,FALSE,FALSE,FALSE,FALSE))
							Success = FALSE;
					}
					else
					{
						pNewNodePath->AttachNode(this, NEXT);
					}

					if (Success)
						pBecomeA->PassBack(pNewNodePath,this);

					if (!pUndoOp)
					{
						CascadeDelete();
						delete this;
					}
				}
			}
			break;

			case BECOMEA_PASSBACK :
			{
				// we need to be a NodePath ....

				// Make a copy of the NodePath of this NodeBlendPath
				CALL_WITH_FAIL(((pNewNodePath = (NodePath*)SimpleCopy()) != NULL), pBecomeA->GetUndoOp(), Success);

				// If successful, pass it back with my attribute map
				if (Success) Success = pBecomeA->PassBack(pNewNodePath,this);
			}
			break;

			default:
				break;
		}
	}

	if (!Success)
	{
		if (pNewNodePath != NULL)
		{
			// Delete all the NodePath's children (if it has any) and unlink it from the tree (if it's linked)
			// This is all done by CascadeDelete()
			pNewNodePath->CascadeDelete(); 
			delete pNewNodePath;
			pNewNodePath = NULL;
		}
	}

	return Success;
}

/********************************************************************************************

>	virtual INT32 NodeBlendPath::ComplexCopy(CopyStage Stage, Range& RangeToCopy, Node** pOutput)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/4/99
	Inputs:		Stage		- COPYOBJECT   if we should make a copy
							- COPYFINISHED once the entire copy operation has completed
				RangeToCopy - Describes the range which is currently being copied.
				pOutput		- a pointer to a pointer to a node. Set this pointer to point
							  at the tree copy
	Returns:	-1	= The routine failed to make a copy.
				 0	= No copy has been made.
				+1	= pOutput points to the copy.
    Purpose:    If the copystage is COPYOBJECT,
       			The node has been called to copy itself and do what ever it needs to to
				make a sensible copy of other items such as attributes.
				The caller (CopyObjects) will not deep copy this node (as this is a complex
				copy and it expects the handler to know what its doing). In this case the
				NodeBlendPath object needs to turn itself into a node path.
	SeeAlso		Node::ComplexCopy(), CopyObjects()

********************************************************************************************/

INT32 NodeBlendPath::ComplexCopy(CopyStage Stage, Range& RangeToCopy, Node** pOutput)
{
	ERROR2IF(pOutput==NULL,FALSE,"NodeBlendPath::ComplexCopy() called with NULL output pointer!");

	switch (Stage)
	{
		case COPYOBJECT:
		{	
			// we've been asked to make a copy of ourselves
			NodePath* NodeCopy = CreateNodePathCopy();
			if (NodeCopy != NULL)
			{
				(*pOutput) = NodeCopy;
				return 1;
			}
			break;
		}

		case COPYFINISHED:
			return 0;
			break;
	}

	return -1;
}

/********************************************************************************************

  >	NodePath* NodeBlendPath::CreateNodePathCopy()

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/5/99
	Inputs:		-
	Returns:	ptr to a NodePath equivalent, or NULL if it fails
	Purpose:	Creates a NodePath that contains the exact same definition.
	SeeAlso:	-

********************************************************************************************/

NodePath* NodeBlendPath::CreateNodePathCopy()
{
	// we've been asked to make a copy of ourselves
	NodePath* pCopy = new NodePath;
	if (pCopy != NULL)
		NodePath::CopyNodeContents(pCopy);

	return pCopy;
}

/********************************************************************************************

  >	virtual BOOL NodeBlendPath::WritePreChildrenWeb(BaseCamelotFilter* pFilter)

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/4/99
	Inputs:		pFilter = ptr to the filter
	Returns:	TRUE if record is written, FALSE if not
	Purpose:	Writes the mould path record to the filter
	SeeAlso:	-

********************************************************************************************/

BOOL NodeBlendPath::WritePreChildrenWeb(BaseCamelotFilter* pFilter)
{
	ERROR2IF(pFilter == NULL,FALSE,"NULL filter param");

	INT32 NumCoords = InkPath.GetNumCoords();
	INT32 RecordSize = sizeof(INT32)+(sizeof(PathVerb)*NumCoords)+(sizeof(DocCoord)*NumCoords);

	CamelotFileRecord Rec(pFilter,TAG_BLEND_PATH,RecordSize);

	BOOL ok = Rec.Init();

	if (ok) ok = Rec.WritePath(&InkPath);
	if (ok) ok = pFilter->Write(&Rec);
	if (ok)
	{
		CamelotFileRecord FilledRec(pFilter, TAG_NODEBLENDPATH_FILLED, sizeof(INT32));
		
		ok = FilledRec.Init();
		if (ok)
		{
			INT32 Filled = InkPath.IsFilled;
			ok = FilledRec.WriteINT32(Filled);
			if (ok) ok = pFilter->Write(&FilledRec);
		}
	}
	if (!ok)
		pFilter->GotError(_R(IDE_FILE_WRITE_ERROR));

	return ok;
}

//--------------------------------------------------------------
// See NodeBlendPath::WritePreChildrenWeb(BaseCamelotFilter* pFilter)
//
BOOL NodeBlendPath::WritePreChildrenNative(BaseCamelotFilter* pFilter)
{
	return WritePreChildrenWeb(pFilter);
}


/********************************************************************************************

 > void NodeBlendPath::DestroyCachedInformation()

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/4/99
	Inputs:		-
	Returns:	-
	Purpose:	Destroys any cached information the node may be storing about the path,
				such as it's length.

				Should be called whenever the node is edited, for example.
	SeeAlso:	-

********************************************************************************************/

void NodeBlendPath::DestroyCachedInformation()
{
	m_CachedPathLength = -1.0;

	if (m_pCachedPointsAtDistance != NULL)
	{
		m_pCachedPointsAtDistance->RemoveAll();
		delete m_pCachedPointsAtDistance;
		m_pCachedPointsAtDistance = NULL;
	}
}

/********************************************************************************************

 > double NodeBlendPath::GetPathLength()

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/4/99
	Inputs:		-
	Returns:	The length of the path stored in this NodeBlendPath
	Purpose:	Access function to underlying Path function
				Also allows the NodeBlendPath to do a bit of caching
	SeeAlso:	-

********************************************************************************************/

double NodeBlendPath::GetPathLength()
{
	if (m_CachedPathLength < 0)
		m_CachedPathLength = InkPath.GetPathLength();
	
	return m_CachedPathLength;
}

/********************************************************************************************
>	BOOL NodeBlendPath::GetPointAtDistance(MILLIPOINT Distance, DocCoord* pPoint)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/4/99
	Inputs:		Distance = a length along the path
	Outputs:	pPoint = the coordinate of the point that distance along the path
				pTangent = tangent at this point (can be NULL)
	Returns:	TRUE/FALSE for success/failure
	Purpose:	Gets the coordinate of a point a certain distance along this path
				Calls the underlying Path class function

				This wrapper allows us to do any caching, if we get round to it.
********************************************************************************************/

BOOL NodeBlendPath::GetPointAtDistance(MILLIPOINT Distance, DocCoord* pPoint, double* pTangent)
{
	ERROR2IF(pPoint == NULL,FALSE,"NULL pPoint param");

	BOOL Found = FALSE;

#ifdef NBP_CACHEPOINTATDISTANCE
	CPathPointInfo	PointInfo;

	if (m_pCachedPointsAtDistance == NULL)
	{
		m_pCachedPointsAtDistance = new CMap<MILLIPOINT,MILLIPOINT&,CPathPointInfo,CPathPointInfo&>;
//		if (m_pCachedPointsAtDistance != NULL)
//			m_pCachedPointsAtDistance->InitHashTable(1277);		// Init hash table size to a suitably large prime number
	}

	if (m_pCachedPointsAtDistance != NULL)
	{
		Found = m_pCachedPointsAtDistance->Lookup(Distance,PointInfo);
		//if (Found)
	//	TRACEUSER( "Diccon", _T("Point %d, %d found in cache\n"), PointInfo.m_Point.x, PointInfo.m_Point.y);

	}

	if (!Found)
	{
		if (InkPath.GetPointAtDistance(Distance,&PointInfo.m_Point,&PointInfo.m_Tangent))
		{
			if (m_pCachedPointsAtDistance != NULL)
				m_pCachedPointsAtDistance->SetAt(Distance,PointInfo);
	//		TRACEUSER( "Diccon", _T("Point %d, %d found from line\n"), PointInfo.m_Point.x, PointInfo.m_Point.y);
			Found = TRUE;
		}
	}

	if (Found)
	{
		*pPoint = PointInfo.m_Point;
		if (pTangent)
			*pTangent = PointInfo.m_Tangent;
	}
#else
	double Tangent = 0.0;
	if (InkPath.GetPointAtDistance(Distance,pPoint,&Tangent))
	{
		if (pTangent)
			*pTangent = Tangent;

		Found = TRUE;
	}
#endif // NBP_CACHEPOINTATDISTANCE

//	if (Found == FALSE)
	//	TRACEUSER( "Diccon", _T("Not found at distance %d\n"), Distance);
	return Found;
}



/********************************************************************************************

 > UINT32 NodeBlendPath::GetPathIndex()

 	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/9/99
	Inputs:		-
	Returns:	The index number of this nodeblendpath
	Purpose:	To find out which NBP this is.
	SeeAlso:	-

********************************************************************************************/

UINT32 NodeBlendPath::GetPathIndex()
{
	return m_Index;
}



/********************************************************************************************

 > INT32 NodeBlendPath::SetPathIndex()

 	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/9/99
	Inputs:		-
	Returns:	The index number of this nodeblendpath, or -1 if it fails
	Purpose:	Sets the index of this NBP by finding out how many NBP's already 
				exist in the parent nodeblend
	SeeAlso:	-

********************************************************************************************/

INT32 NodeBlendPath::SetPathIndex()
{
	Node* pParent = ((Node*)this)->FindParent();
	if ((pParent == NULL) || (!pParent->IS_KIND_OF(NodeBlend)))
	{
		ERROR3("Path has no parent, or it is not a nodeblend");
		return -1;
	}
	
	UINT32 Index = ((NodeBlend*)pParent)->GetNumNodeBlendPaths();

	m_Index = Index;

	return (INT32)Index;
}


/***********************************************************************************************

>	virtual BOOL NodeBlendPath::IsANodeBlendPath()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/11/99
	Inputs:		-
	Purpose:	virtual identifier function
	

***********************************************************************************************/

BOOL NodeBlendPath::IsANodeBlendPath()
{
	return TRUE;
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

/********************************************************************************************

>	ActionCode InitNodeBlendPathAction::Init(UndoableOperation* pOp,ActionList* pActionList,NodeBlend* pThisNodeBlend)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/5/99
	Inputs:		pOp  			= ptr to the operation to which this action belongs
				pNodeBlendPath  = ptr to blend path node involved
				pActionList 	= ptr to action list to which this action should be added
	Outputs:	-
	Returns:	ActionCode, one of AC_OK, AC_NO_RECORD or AC_FAIL
	Purpose:	Simply destroys the cached information for this blend path
	Errors:		-
	SeeAlso:	Action::Init()

********************************************************************************************/

ActionCode InitNodeBlendPathAction::Init(UndoableOperation* pOp,ActionList* pActionList,NodeBlendPath* pNodeBlendPath)
{
	ERROR2IF(pOp			== NULL,AC_FAIL,"No undo op ptr given");
	ERROR2IF(pActionList	== NULL,AC_FAIL,"No action list given");
	ERROR2IF(pNodeBlendPath == NULL,AC_FAIL,"No blend path node given");

	UINT32 ActSize = sizeof(InitNodeBlendPathAction);
	InitNodeBlendPathAction* pNewAction;
	ActionCode Ac = Action::Init(pOp,pActionList,ActSize,CC_RUNTIME_CLASS(InitNodeBlendPathAction),(Action**)&pNewAction);

	if (Ac != AC_FAIL && pNewAction != NULL)
	{
		pNewAction->m_pNodeBlendPath = pNodeBlendPath;
		pNodeBlendPath->DestroyCachedInformation();
	}

	return Ac;
}

/********************************************************************************************

>	ActionCode InitNodeBlendPathAction::Execute();

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/5/99
	Inputs:		-
	Outputs:	-
	Returns:	ActionCode, one of AC_OK, AC_NO_RECORD or AC_FAIL
	Purpose:	Executes the action.  
				Destroys the blend path node's cached info
	Errors:		-
	SeeAlso:	Action::Init()

********************************************************************************************/

ActionCode InitNodeBlendPathAction::Execute()
{
	if (pOperation->IS_KIND_OF(UndoableOperation))
		return InitNodeBlendPathAction::Init((UndoableOperation*)pOperation,pOppositeActLst,m_pNodeBlendPath);
	else
	{
		ERROR3("InitNodeBlendPathAction::Execute() called with op that's not an undoable op");
		return AC_FAIL;
	}
}

