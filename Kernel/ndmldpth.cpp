// $Id: ndmldpth.cpp 662 2006-03-14 21:31:49Z alex $
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
// NodeMouldPath implementation. This file controls the tree node 'mouldpath'
// which is the shape of the mould for perspective and envelope shapes.

/*
*/

#include "camtypes.h"
#include "ndmldpth.h"
#include "nodepath.h"
#include "nodemold.h"
#include "nodemldr.h"
#include "moldshap.h"
#include "aw_eps.h"
#include "nativeps.h"		// The old style EPS native filter, used in v1.1
#include "blobs.h"
#include "app.h"
//#include "tim.h"
//#include "ngcore.h"		// NameGallery, for stretching functionality

#include "cxftags.h"
#include "cxfrec.h"
#include "camfiltr.h"

DECLARE_SOURCE("$Revision: 662 $");

CC_IMPLEMENT_DYNCREATE( NodeMouldPath, NodePath )  

// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW


/*********************************************************************************************

>    NodeMouldPath::NodeMouldPath() 

     Author:	Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
     Created:	16/01/95
     Purpose: 	This constructor creates a NodeMouldPath linked to no other, with all status
		  	  	flags false and an uninitialised bounding rectangle.           

**********************************************************************************************/
 
NodeMouldPath::NodeMouldPath(): NodePath()
{
}


/***********************************************************************************************

>	void NodeMouldPath::NodeMouldPath
	(
		Node* ContextNode,  
		AttachNodeDirection Direction,  
		BOOL Locked = FALSE, 
		BOOL Mangled = FALSE,  
		BOOL Marked = FALSE, 
		BOOL Selected = FALSE, 
	)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
    Created:	16/01/95
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

NodeMouldPath::NodeMouldPath(Node* ContextNode,  
					 		 AttachNodeDirection Direction,  
					 		 BOOL Locked, 
				 	 		 BOOL Mangled,  
					 		 BOOL Marked, 
							 BOOL Selected   
			   ):NodePath(ContextNode, Direction, Locked, Mangled, Marked, Selected) 
{ 
} 

/*********************************************************************************************

>    NodeMouldPath::~NodeMouldPath() 

     Author:	Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
     Created:	16/01/95
     Purpose: 	Destructor for Node MouldPath. This does nothing at present.

**********************************************************************************************/
 
NodeMouldPath::~NodeMouldPath()
{
}


/***********************************************************************************************

>	virtual Node* NodeMouldPath::SimpleCopy()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/01/95
	Returns:	Pointer to a Node
	Purpose:	Makes a copy of all the data in the node

***********************************************************************************************/

Node* NodeMouldPath::SimpleCopy()
{
	NodeMouldPath* NodeCopy = new NodeMouldPath();
    if (NodeCopy)
    	CopyNodeContents(NodeCopy);
    
	return NodeCopy;
}            


/***********************************************************************************************

>	void NodeMouldPath::CopyNodeContents(NodeMouldPath* NodeCopy)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/5/95
	Inputs:		NodeCopy - The node to copy
	Purpose:	Copies the data in the node by first calling the base class to get it to
				copy its stuff, and then copying its own stuff
	Scope:		protected
	SeeAlso:	NodeRenderableInk::CopyNodeContents

***********************************************************************************************/

void NodeMouldPath::CopyNodeContents( NodeMouldPath* NodeCopy)
{
	NodePath::CopyNodeContents( NodeCopy );
	//Copy contents specific to derived class here
}



/***********************************************************************************************
>   void NodeMouldPath::PolyCopyNodeContents(NodeRenderable* pNodeCopy)

    Author:     Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
    Created:    18/12/2003
    Outputs:    -
    Purpose:    Polymorphically copies the contents of this node to another
	Errors:     An assertion failure will occur if NodeCopy is NULL
    Scope:      protected
								     
***********************************************************************************************/

void NodeMouldPath::PolyCopyNodeContents(NodeRenderable* pNodeCopy)
{
	ENSURE(pNodeCopy, "Trying to copy a node's contents into a NULL node");
	ENSURE(IS_A(pNodeCopy, NodeMouldPath), "PolyCopyNodeContents given wrong dest node type");

	if (IS_A(pNodeCopy, NodeMouldPath))
		CopyNodeContents((NodeMouldPath*)pNodeCopy);
}



/***********************************************************************************************

>	virtual void NodeMouldPath::Render(RenderRegion* pRRegion)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/01/95
	Inputs:		Pointer to a render region
	Purpose:	Overrides inherited function to do nothing.

***********************************************************************************************/

void NodeMouldPath::Render(RenderRegion* pRender)
{            
	if (!InsideMould())
		NodePath::Render(pRender);
}  


/********************************************************************************************

>	virtual void NodeMouldPath::RenderEorDrag( RenderRegion* pRender )

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/01/95
	Inputs:		pRender - A Pointer to the current RenderRegion
	Purpose:	Overrides inherited function to do nothing.

********************************************************************************************/

void NodeMouldPath::RenderEorDrag( RenderRegion* pRender )
{
#if !defined(EXCLUDE_FROM_RALPH)
	//if (!InsideMould())
	BlobManager* pBlobMan = GetApplication()->GetBlobManager();
	BlobStyle CurrBlobs = pBlobMan->GetCurrentInterest();
	if (CurrBlobs.Object)
		NodePath::RenderEorDrag(pRender);
#endif
}


/********************************************************************************************

>	virtual void NodeMouldPath::RenderObjectBlobs(RenderRegion* pRender)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Inputs:		pRender - The region to render the blobs into
	Purpose:	Draws the paths object blobs into the render region supplied

********************************************************************************************/

void NodeMouldPath::RenderObjectBlobs(RenderRegion* pRender)
{
#if !defined(EXCLUDE_FROM_RALPH)
	if (!InsideMould())
		NodePath::RenderObjectBlobs(pRender);
	else
	{
		// set our eor colour.
		pRender->SetLineColour(BLACK);
		// render the path
		pRender->DrawPath(&InkPath);
		// call the parent class render function
		NodePath::RenderObjectBlobs(pRender);
	}
#endif
}



/********************************************************************************************

>	DocRect NodeMouldPath::GetBoundingRect(BOOL DontUseAttrs=FALSE, BOOL HitTest=FALSE)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/01/95
	Inputs:		DontUseAttrs - TRUE if you don't want to use the nodes attrs to calculate
				the bounding rect (defaults will be used). Defaults to FALSE.
				HitTest      - TRUE if being called during HitTest
	Returns:	A null rectangle
	Purpose:	Returns a completely null rectangle

********************************************************************************************/

DocRect NodeMouldPath::GetBoundingRect(BOOL DontUseAttrs, BOOL HitTest)
{
	/* Tricky problems:
	   (1) When importing, the importer translates the imported picture to centre around
	       the drop point. Unfortunately it calls us to find our bbox. We don't have a
		   bbox until the moulder nodes are built, which is not until PostImport time so
		   the importer does a Translate = (Drop - Center) which is X = (0 - C), BAD.
		   So we must return a bbox if their is no moulder in the mould! Nasty Showstopper fix	*/
	
	if (InsideMould())
	{
		// if there is a moulder node to contribute a bbox
		if (FindNext(CC_RUNTIME_CLASS(NodeMoulder)))
		{
			// Then we dont contribute anything!
			DocRect Rect;
			return Rect;
		}
	}
	// Otherwise we do.
	return NodePath::GetBoundingRect(DontUseAttrs);
}


/********************************************************************************************

>	BOOL NodeMouldPath::InsideMould()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/02/95
	Inputs:		-
	Returns:	TRUE if this mould path is inside a mould object ie its immediate parent
				is a mould object
	Purpose:	Determins whether the immediate parent of this node mould path object is
				a mould object.

********************************************************************************************/

BOOL NodeMouldPath::InsideMould()
{
	Node* p = FindParent();
	if (p)
	{
		if (IS_A(p, NodeMould))
			return TRUE;
		if (IS_A(p, NodeHidden))
			return TRUE;
	}
	return FALSE;
}



/*********************************************************************************************

>    BOOL NodeMouldPath::ExportRender(RenderRegion* pRegion)

     Author:	Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
     Created:	14/03/95
     Inputs:	pRegion = ptr to the export render region to export to
     Outputs:	
     Returns:	TRUE if ok, FALSE if something went wrong
     Purpose: 	Called after this node and all of its children have been rendered to the
     			export region. This outputs the "end mould" command.
				Supports ArtWorks EPS and Camelot EPS
     Errors:    

**********************************************************************************************/
 
BOOL NodeMouldPath::ExportRender(RenderRegion* pRegion) 
{
#ifdef DO_EXPORT
	if (pRegion->IS_KIND_OF(NativeRenderRegion))
		return ExportCAMEPS(pRegion);

	if (pRegion->IS_KIND_OF(ArtWorksEPSRenderRegion))
		return ExportAWEPS(pRegion);
#endif	
	return FALSE;
}


/*********************************************************************************************

	BOOL NodeMouldPath::ExportCAMEPS(RenderRegion* pRegion)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/03/95
	Inputs:		pRegion = a pointer to a Camelot EPS render region
	Returns:	TRUE if we've rendered the object
				FALSE if we want the caller to render the object
	Purpose:	Export delimiter tokens for this particular object.
				The format is defined as
					csmp
						path description
					cemp

**********************************************************************************************/

BOOL NodeMouldPath::ExportCAMEPS(RenderRegion* pRegion)
{
#ifdef DO_EXPORT
	EPSExportDC *pDC = (EPSExportDC *) pRegion->GetRenderDC();

	// If we're hanging around in a mould then export the correct tokens
	if (!InsideMould())
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
	return TRUE;
}




/*********************************************************************************************

	BOOL NodeMouldPath::ExportAWEPS(RenderRegion* pRegion)

    Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
    Created:	14/03/95
	Inputs:		pRegion = a pointer to a Camelot EPS render region
	Returns:	TRUE if we've rendered the object
				FALSE if we want the caller to render the object
	Purpose:	Overrides the standard call to Obj->Render so we can render the path
				as we would like

**********************************************************************************************/

BOOL NodeMouldPath::ExportAWEPS(RenderRegion* pRegion)
{
	// just return false for the moment until we write our
	// proper random AWEPS path render function
	return FALSE;
}


/********************************************************************************************

>	virtual INT32 NodeMouldPath::ComplexCopy(CopyStage Stage, Range& RangeToCopy, Node** pOutput)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/5/95
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
				NodeMouldPath object needs to turn itself into a node path.
	SeeAlso		Node::ComplexCopy(), CopyObjects()

********************************************************************************************/

INT32 NodeMouldPath::ComplexCopy(CopyStage Stage, Range& RangeToCopy, Node** pOutput)
{
	ERROR2IF(pOutput==NULL,FALSE,"NodeMouldPath::ComplexCopy() called with NULL output pointer!");

	switch (Stage)
	{
		case COPYOBJECT:
		{	
			// we've been asked to make a copy of ourselves
			NodePath* NodeCopy = new NodePath;
			BOOL ok = (NodeCopy!=NULL);
			if (ok)
			{
				NodePath::CopyNodeContents(NodeCopy);
				// Copy contents specific to derived class here
				NodeCopy->InkPath.IsFilled = TRUE;
			}

			if (ok)
			{
				(*pOutput) = NodeCopy;
				return 1;
			}
			else
				return -1;
			}
			break;

		case COPYFINISHED:
			return 0;
			break;

		default:
			return -1;
			break;
	}
}

/********************************************************************************************

  >	virtual BOOL NodeMouldPath::WritePreChildrenWeb(BaseCamelotFilter* pFilter)

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/7/96
	Inputs:		pFilter = ptr to the filter
	Returns:	TRUE if record is written, FALSE if not
	Purpose:	Writes the mould path record to the filter
	SeeAlso:	-

********************************************************************************************/

BOOL NodeMouldPath::WritePreChildrenWeb(BaseCamelotFilter* pFilter)
{
	ERROR2IF(pFilter == NULL,FALSE,"NULL filter param");

	INT32 NumCoords = InkPath.GetNumCoords();
	INT32 RecordSize = sizeof(INT32)+(sizeof(PathVerb)*NumCoords)+(sizeof(DocCoord)*NumCoords);

	CamelotFileRecord Rec(pFilter,TAG_MOULD_PATH,RecordSize);

	BOOL ok = Rec.Init();

	if (ok) ok = Rec.WritePath(&InkPath);
	if (ok) ok = pFilter->Write(&Rec);

	if (!ok)
		pFilter->GotError(_R(IDE_FILE_WRITE_ERROR));

	return ok;
}

//--------------------------------------------------------------
// See NodeMouldPath::WritePreChildrenWeb(BaseCamelotFilter* pFilter)
//
BOOL NodeMouldPath::WritePreChildrenNative(BaseCamelotFilter* pFilter)
{
	return WritePreChildrenWeb(pFilter);
}


