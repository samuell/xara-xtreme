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
// NodeBlender implementation

/*
*/


#include "camtypes.h"
//#include "ops.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "fillattr.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "document.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "osrndrgn.h"

#include "nodepath.h"
#include "ophist.h"
//#include "becomea.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "trans2d.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "lineattr.h"
#include "blendatt.h"
#include "qualattr.h"
//#include "markn.h"
#include "gblend.h"
#include "gwinding.h"
//#include "docview.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "grnddib.h"
//#include "grndclik.h"
#include "ndbldpth.h"

//#include "fixmem.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "aw_eps.h"
#include "nativeps.h"		// The old style EPS native filter, used in v1.1
#include "cameleps.h"
#include "attrmap.h"
#include "progress.h"

#include "cxftags.h"
//#include "cxfdefs.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "cxfrec.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "camfiltr.h" - in camtypes.h [AUTOMATICALLY REMOVED]

#include "nodeblnd.h"
#include "nodebldr.h"

//#include "bitmap.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "oilbitmap.h"

#include "brshattr.h"
#include "blndhelp.h"
#include "fthrattr.h"
#include "nodecont.h"

#include "brshbeca.h"
#include "strkattr.h"

DECLARE_SOURCE("$Revision$");

// Declare smart memory handling in Debug builds
//#define Swap(a,b,t) { t=a; a=b; b=t; }

#define SWAP(type,a,b) { type x=a; a=b; b=x; }

#define DELPTR(p) if (p != NULL) { delete p; p = NULL; }


//-----------------------------------------------------------

void ListStepper::Init(UINT32 NumInStart,UINT32 NumInEnd,UINT32 Start)
{
	if (NumInStart > NumInEnd)
	{
		Min = NumInEnd;	  pNextInMin = &NextInEnd;
		Max = NumInStart; pNextInMax = &NextInStart;
	}
	else
	{
		Min = NumInStart; pNextInMin = &NextInStart;
		Max = NumInEnd;   pNextInMax = &NextInEnd;
	}

	NextInStart = NextInEnd = Inc = 0;

	for (INT32 dummy;Start > 0;Start--)
		GetNext(&dummy,&dummy);
}

void ListStepper::GetNext(INT32* pNextInStart,INT32* pNextInEnd)
{
	if ((*pNextInMax) < Max)
	{
		*pNextInStart = NextInStart;
		*pNextInEnd   = NextInEnd;

		Inc += Min;
		if (Inc >= Max)
		{
			Inc -= Max;
			(*pNextInMin)++;
		}
		(*pNextInMax)++;
	}
	else
	{
		*pNextInStart = -1;
		*pNextInEnd   = -1;
	}
}

/***********************************************************************************************

>	class BlendSubRenderContext: public SubRenderContext

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/11/94
	Purpose:	Used during rendering to store the render context of a blender node if it has
				taken too long to render so far.

***********************************************************************************************/

class BlendSubRenderContext: public SubRenderContext
{
CC_DECLARE_DYNCREATE(BlendSubRenderContext);
public:
	BlendSubRenderContext() { BlendStep = 1; }

	UINT32 BlendStep;
};

CC_IMPLEMENT_DYNCREATE(BlendSubRenderContext,SubRenderContext);

//-----------------------------------------------------------

// Put my version Number into the About box
DECLARE_SOURCE( "$Revision$" );

CC_IMPLEMENT_DYNCREATE(NodeBlender, NodeRenderableInk)
CC_IMPLEMENT_MEMDUMP(BlendRef,CC_CLASS_MEMDUMP)
CC_IMPLEMENT_MEMDUMP(BlendPath,ListItem)

CC_IMPLEMENT_MEMDUMP(BecomeA,CC_CLASS_MEMDUMP)
CC_IMPLEMENT_MEMDUMP(BlendBecomeA,BecomeA)
CC_IMPLEMENT_MEMDUMP(HandleBecomeA,CC_CLASS_MEMDUMP)
CC_IMPLEMENT_MEMDUMP(NodeCompoundBlendBecomeA,BecomeA)

// Declare smart memory handling in Debug builds
// #define new CAM_DEBUG_NEW

#define MAX_NUM_BLEND_STEPS 999
#define FLATNESS 1024

/***********************************************************************************************

>	NodeBlender::NodeBlender(Node* 	ContextNode,
						AttachNodeDirection Direction,
						const DocRect& 		BoundingRect,
						BOOL				Locked = FALSE,
						BOOL 				Mangled = FALSE,
						BOOL 				Marked = FALSE,
						BOOL 				Selected = FALSE,
						BOOL 				Renderable = FALSE
						)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/10/94
    
	Inputs:		ContextNode: Pointer to a node which this node is to be attached to.
				MonoOn Direction: MonoOff
				Specifies the direction in which the node is to be attached to the
				ContextNode. The values this variable can take are as follows:
								  
				PREV      : Attach node as a previous sibling of the context node
				NEXT      : Attach node as a next sibling of the context node
				FIRSTCHILD: Attach node as the first child of the context node
				LASTCHILD : Attach node as a last child of the context node

				BoundingRect: Bounding rectangle

				The remaining inputs specify the status of the node:
			
				Locked:     Is node locked ?
				Mangled:    Is node mangled ?
				Marked:     Is node marked ?
				Selected:   Is node selected ?

	Purpose:	This constructor initialises the nodes flags and links it to ContextNode in the
				direction specified by Direction. All neccesary tree links are updated.

	Note:		Initialise() must be called before the NodeBlender is in a state in which it can be used.
    SeeAlso: 	Initialise()   
   	Errors: 	An assertion error will occur if ContextNode is NULL

***********************************************************************************************/

NodeBlender::NodeBlender(Node* ContextNode,  
					AttachNodeDirection Direction,  
					BOOL Locked, 
					BOOL Mangled,  
					BOOL Marked, 
					BOOL Selected    
			  ):NodeRenderableInk(ContextNode, Direction, Locked, Mangled, Marked, Selected )  
{                         
	ResetVars();
}

/*********************************************************************************************

>	NodeBlender::NodeBlender() 

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/10/94
	Purpose:	This constructor creates a NodeBlender linked to no other with all status
				flags false and an uninitialized bounding rectangle.           
 	Note:		Initialise() must be called before the NodeBlender is in a state in which it can be 
				used. 		
	SeeAlso:	Initialise()                                                        
 
**********************************************************************************************/
/* Technical Notes:   
	The default constructor is required so that SimpleCopy will work 
*/  

NodeBlender::NodeBlender(): NodeRenderableInk()
{
	ResetVars();
}

/*********************************************************************************************

>	NodeBlender::~NodeBlender() 

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/10/94
	Purpose:	Default deconstructor
 
**********************************************************************************************/

NodeBlender::~NodeBlender()
{
	// Call PostBlend() in case we're trying to be deleted in the middle of rendering.
	PostBlend();

	// Delete the two blend references
	DELPTR(m_pRefStart);
	DELPTR(m_pRefEnd);
}

/*********************************************************************************************

>	void NodeBlender::ResetVars() 

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/11/94
	Purpose:	Resets all the member vars to default values.
				Should only be called by NodeBlender constructors
 
**********************************************************************************************/

void NodeBlender::ResetVars() 
{
	m_pRefStart     	= NULL;
	m_pRefEnd       	= NULL;
	m_pNodeStart		= NULL;
	m_pNodeEnd		= NULL;
	m_ObjIndexStart   = -1;
	m_ObjIndexEnd     = -1;
	m_PathIndexStart 	= -1;
	m_PathIndexEnd   	= -1;

	m_Initialised   	= FALSE;

	m_TempArraySize 	= 0;
	m_pTempCoords 	= NULL;
	m_pTempVerbs 		= NULL;
	m_pTempFlags 		= NULL;
	m_GBlendBuffSize 	= 0;
	m_pGBlendBuff 	= NULL;

	m_ProportionOfPathDistStart = -1.0;
	m_ProportionOfPathDistEnd   = -1.0;

	m_AngleStart = 0.0;
	m_AngleEnd	 = 0.0;

	m_Reversed = FALSE;
	m_BlendedOnCurve = FALSE;
	m_NodeBlendPathIndex = -2; //-2 to deal with the case of the previous file format - see
								// HandleBlendPathRecord for details
}                        
 
/***********************************************************************************************

>	virtual Node* NodeBlender::SimpleCopy()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/10/94
	Returns:	Pointer to a Node
	Purpose:	Makes a copy of all the data in the node

***********************************************************************************************/

Node* NodeBlender::SimpleCopy()
{
	// Make a new NodeBlender and then copy things into it
	NodeBlender* pCopyOfNode = new NodeBlender();
    if (pCopyOfNode)
    	CopyNodeContents(pCopyOfNode);
    
	return pCopyOfNode;
}            


/***********************************************************************************************

>	void NodeBlender::CopyNodeContents(NodeBlender* pCopyOfNode)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/10/94
	Inputs:		pCopyOfNode - The node to copy data to
	Purpose:	Copies the data in this node to pCopyOfNode by first calling the base class to get it to
				copy its stuff, and then copying its own stuff
	Scope:		protected
	SeeAlso:	NodeRenderableInk::CopyNodeContents

***********************************************************************************************/

void NodeBlender::CopyNodeContents(NodeBlender* pCopyOfNode)
{
	NodeRenderableInk::CopyNodeContents(pCopyOfNode);
	
	// Copy contents specific to derived class here

	pCopyOfNode->m_ObjIndexStart	= m_ObjIndexStart;
	pCopyOfNode->m_ObjIndexEnd		= m_ObjIndexEnd;
	pCopyOfNode->m_PathIndexStart	= m_PathIndexStart;
	pCopyOfNode->m_PathIndexEnd		= m_PathIndexEnd;

	pCopyOfNode->m_ProportionOfPathDistStart = m_ProportionOfPathDistStart;
	pCopyOfNode->m_ProportionOfPathDistEnd   = m_ProportionOfPathDistEnd;
	pCopyOfNode->m_NodeBlendPathIndex        = m_NodeBlendPathIndex;
	pCopyOfNode->m_BlendedOnCurve            = m_BlendedOnCurve;
	pCopyOfNode->SetReversed(m_Reversed);

	pCopyOfNode->m_AngleEnd                  = m_AngleEnd;
	pCopyOfNode->m_AngleStart                = m_AngleStart;

	pCopyOfNode->SetUninitialised();
}



/***********************************************************************************************
>   void NodeBlender::PolyCopyNodeContents(NodeRenderable* pNodeCopy)

    Author:     Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
    Created:    18/12/2003
    Outputs:    -
    Purpose:    Polymorphically copies the contents of this node to another
	Errors:     An assertion failure will occur if NodeCopy is NULL
    Scope:      protected
								     
***********************************************************************************************/

void NodeBlender::PolyCopyNodeContents(NodeRenderable* pNodeCopy)
{
	ENSURE(pNodeCopy, "Trying to copy a node's contents into a NULL node");
	ENSURE(IS_A(pNodeCopy, NodeBlender), "PolyCopyNodeContents given wrong dest node type");

	if (IS_A(pNodeCopy, NodeBlender))
		CopyNodeContents((NodeBlender*)pNodeCopy);
}



/***********************************************************************************************

>	void NodeBlender::ShowDebugTreeDetails() const

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/10/94
	Purpose:	Displays debugging info of the tree
	SeeAlso:	NodeRenderableInk::ShowDebugTreeDetails

***********************************************************************************************/
#ifdef _DEBUG
void NodeBlender::ShowDebugTreeDetails() const
{                     
	// Display a bit of debugging info
	// For now, we will just call the base class version
	NodeRenderableInk::ShowDebugTreeDetails();	
}
#endif

/********************************************************************************************

>	void NodeBlender::GetDebugDetails( StringBase* Str )

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/10/94
	Outputs:	Str: String giving debug info about the node
	Purpose:	For obtaining debug information about the Node

********************************************************************************************/

void NodeBlender::GetDebugDetails( StringBase* Str )
{
#ifdef _DEBUG
	// Call base class
	NodeRenderableInk::GetDebugDetails( Str );
	
	(*Str) += TEXT( "\r\nBlender Data Dump\r\n" );

	TCHAR				buf[500];
	camSnprintf( buf, 500,	_T("Blend steps = %ld\r\n")
						_T("m_ObjIndex start = %ld\r\n")
						_T("m_ObjIndex end   = %ld\r\n")
						_T("m_ProportionOfPathDistStart = %f\r\n")
						_T("m_ProportionOfPathDistEnd   = %f\r\n")
						_T("m_AngleStart = %f\r\n")
						_T("m_AngleEnd   = %f\r\n")
						_T("m_NodeBlendPathIndex = %ld\r\n")
						_T("\r\n"),
						GetNumBlendSteps(),
						m_ObjIndexStart,
						m_ObjIndexEnd,
						m_ProportionOfPathDistStart,
						m_ProportionOfPathDistEnd,
						m_AngleStart,
						m_AngleEnd,
						m_NodeBlendPathIndex
						);

	*Str += buf;

	*Str += _T("--------- Start\r\n\r\n");
	GetDebugDetails(Str,m_pRefStart);

	*Str += _T("--------- End\r\n\r\n");
	GetDebugDetails(Str,m_pRefEnd);
#endif
}


/********************************************************************************************

>	void NodeBlender::GetDebugDetails( StringBase* Str,BlendRef* pBlendRef )

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/2/95
	Outputs:	Str: String giving debug info about the node
	Purpose:	For obtaining debug information about the blend reference

********************************************************************************************/

void NodeBlender::GetDebugDetails( StringBase* Str, BlendRef* pBlendRef )
{
#ifdef _DEBUG
	String_256 TempStr;

	TempStr._MakeMsg( _T("Num blend paths = #1%d\r\n"),pBlendRef->GetNumBlendPaths());
	*Str += TempStr;
	BlendPath* pBlendPath = pBlendRef->GetFirstBlendPath();
	while (pBlendPath != NULL)
	{
		Path*      pPath   = pBlendPath->GetPath();
		PathVerb*  pVerbs  = pPath->GetVerbArray();
		PathFlags* pFlags  = pPath->GetFlagArray();
		DocCoord*  pCoords = pPath->GetCoordArray();

		TempStr._MakeMsg( _T("Original Mapping = #1%d\r\n"), pBlendPath->GetOrigMapping());
		*Str += TempStr;

		for (INT32 i=0; i<pPath->GetNumCoords(); i++)
		{
			// Add the info to the string
			TempStr._MakeMsg( TEXT("#1%d.\t#2%d\t#3%ld,\t#4%ld\t"),
							  i, pVerbs[i], pCoords[i].x, pCoords[i].y );
			(*Str) += TempStr;

			if (pFlags[i].IsEndPoint)
			{
				TempStr._MakeMsg( _T(": E\t") );
				(*Str) += TempStr;
			}

			*Str += _T("\r\n");
		}
		pBlendPath = pBlendRef->GetNextBlendPath(pBlendPath);
	}
#endif
}


/********************************************************************************************

>	virtual void NodeBlender::Transform( TransformBase& Trans )

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/10/94
	Inputs:		Trans - The transform Object
	Purpose:	Transforms all the paths attached to this blender node
	SeeAlso:	NodeRenderableInk::Transform()

********************************************************************************************/

void NodeBlender::Transform( TransformBase& Trans )
{
	InvalidateBoundingRect();
	Deinit();
/*
	if (!Initialised) return;

	// The blender's bounding rect is no longer valid
	InvalidateBoundingRect();

	// Transform all the BlendPaths in the two blend references

	if (pRefStart != NULL) pRefStart->Transform(Trans);
	if (pRefEnd   != NULL) pRefEnd  ->Transform(Trans);
*/
}


/***********************************************************************************************

>	BOOL NodeBlender::HidingNode()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/11/94
	Inputs:		-
	Outputs:	-
	Purpose:	Called whenever the node gets hidden.
				It calls the Deinit() member function

***********************************************************************************************/

BOOL NodeBlender::HidingNode()
{
	PostBlend();
	Deinit();
	return TRUE;
}

/***********************************************************************************************

>	BOOL NodeBlender::ShowingNode()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/11/94
	Inputs:		-
	Outputs:	-
	Purpose:	Called whenever the node gets shown after it's been hidden.
				It calls the Reinit() member function

***********************************************************************************************/

BOOL NodeBlender::ShowingNode()
{
	return TRUE;
//	return (Reinit());
}

/***********************************************************************************************

>	void NodeBlender::Render(RenderRegion* pRender)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/10/94
	Inputs:		Pointer to a render region
	Purpose:	Will render the blended steps of this blender node

***********************************************************************************************/

void NodeBlender::Render(RenderRegion* pRender)
{
	pRender->SaveContext();
	CreateBlends(pRender,NULL);		// Only supply a render region, because we're only rendering
	pRender->RestoreContext();
}  

/***********************************************************************************************

>	BOOL NodeBlender::CallBeginBlendStep(BlendNodeParam * pParam)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/3/2000
	Inputs:		pParam		-	the bevel node parameter to pass to all nodes
	Outputs:	-
	Returns:	-
	Purpose:	Calls all begin blend step functions on appropriate nodes

***********************************************************************************************/
BOOL NodeBlender::CallBeginBlendStep(BlendNodeParam * pParam, BOOL* usingSumAllPathsPathProcessor)
{
	if (pParam->GetRenderRegion() == NULL)
	{
		// CGS:  if the RenderRegion == NULL, then we might be doing a convert to shapes
		// test whether or not we are
		if (!pParam->GetHandleBecomeA ())
		{
			return TRUE;	// er - there ain't much we can do
		}
		// else we are in a convert to editable shapes - continue
	}

	NodeRenderableInk * pCompoundStep = pParam->GetStartBlendPath()->GetBlendNode();

	if (!pCompoundStep->IsCompound())
	{
		*usingSumAllPathsPathProcessor = FALSE;
		return TRUE;
	}

	if (((NodeCompound *)pCompoundStep)->IsStartBlendGroupNode())
	{
		if (!((NodeCompound *)pCompoundStep)->BeginBlendStep(pParam))
		{
			return FALSE;
		}
		else
		{
			// we are using a path processor to do our funky stuff ....
			*usingSumAllPathsPathProcessor = TRUE;
		}
	}

	Node * pStep = pCompoundStep->FindFirstChild();

	while (pStep)
	{
		if (pStep->IsCompound())
		{
			NodeCompound * pCompound = (NodeCompound *)pStep;
				
			if (pCompound->IsStartBlendGroupNode())
			{
				if (!pCompound->BeginBlendStep(pParam))
				{
					return FALSE;
				}
				else
				{
					*usingSumAllPathsPathProcessor = TRUE;
				}
			}
		}

		pStep = pStep->FindNext();
	}

	return TRUE;
}

/***********************************************************************************************

>	BOOL NodeBlender::CallEndBlendStep(BlendNodeParam * pParam)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/3/2000
	Inputs:		pCompound	-	the root of the tree to use
				pParam		-	the bevel node parameter to pass to all nodes
	Outputs:	-
	Returns:	-
	Purpose:	Calls all end blend step functions on appropriate nodes

***********************************************************************************************/
BOOL NodeBlender::CallEndBlendStep(BlendNodeParam * pParam)
{
	// we need to do this bottom up	
	if (pParam->GetRenderRegion() == NULL)
	{
		// CGS:  if the RenderRegion == NULL, then we might be doing a convert to shapes
		// test whether or not we are (or were)
		if (!pParam->GetHandleBecomeA ())
		{
			return TRUE;	// er - there ain't much we can do
		}
		// else we are in a convert to editable shapes - continue
	}

	NodeRenderableInk * pCompoundStep =  pParam->GetStartBlendPath()->GetBlendNode();

//	View * pView = View::GetCurrent();

	if (!pCompoundStep->IsCompound())
		return TRUE;

	Node * pStep = pCompoundStep->FindFirstDepthFirst();

	while (pStep && pStep != pCompoundStep)
	{
		if (pStep->IsCompound())
		{
			NodeCompound * pCompound = (NodeCompound *)pStep;
				
			if (pCompound->IsEndBlendGroupNode())
			{
				if (!pCompound->EndBlendStep(pParam))
				{
					return FALSE;
				}
			}
		}

		pStep = pStep->FindNextDepthFirst(pCompoundStep);
	}

	if (((NodeCompound *)pCompoundStep)->IsEndBlendGroupNode())
	{
		if (!((NodeCompound *)pCompoundStep)->EndBlendStep(pParam))
		{
			return FALSE;
		}
	}

	return TRUE;
}

/***********************************************************************************************

>	void NodeBlender::CreateBlends(RenderRegion* pRender,HandleBecomeA* pHandleBecomeA)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/10/94
	Inputs:		pRender 		= ptr to a render region (Can be NULL)
				pHandleBecomeA  = ptr to class to pass each created blended path to (can be NULL)
	Outputs:	-
	Returns:	-
	Purpose:	Will generate the blended steps of this blender node.
				If pRender != NULL, it will render each blend step to it.
				if pHandleBecomeA != NULL, it will pass each blend step to pHandleBecomeA->PassBack() as it's generated

***********************************************************************************************/

void NodeBlender::CreateBlends(RenderRegion* pRender,HandleBecomeA* pHandleBecomeA)
{
	TRACEUSER( "ChrisS", _T("Entering create blends\n"));
	
	// Ensure that we are initialised (if it can be done!!)
	if (!Reinit())
		return;
	if (!m_Initialised) return;		// we MUST handle the this case! (blending NULL to NULL etc.)
	// Check the important member vars, to be on the safe side
	if (m_pRefStart == NULL || m_pRefEnd == NULL)
	{
		ERROR3("Dodgey pRef params");
		return;
	}
	if (m_pNodeStart == NULL || m_pNodeEnd == NULL)
	{
		ERROR3("Dodgey m_pNodeStart and/or m_pNodeEnd params");
		return;
	}

	// If the render region's quality does not state full blend rendering,
	// proceed as if we are not rendering at all (this is done by pretending we haven't
	// got a render region
	if (pRender != NULL && pRender->RRQuality.GetBlendQuality() != Quality::FullBlend)
		pRender = NULL;

	// If we're not rendering and not handling becoming paths, exit now.
	if (pRender == NULL && pHandleBecomeA == NULL)
		return;

	// Only allow background rendering when going to a DIB
	BOOL AllowBackgroundRender = (pRender!=NULL &&
								  ( IS_A(pRender,GRenderDIB) ||
								    pRender->IsHitDetect() )
								  );
	
	// If somebody else has locked the sub render context, then we cannot BG render without stepping on their toes.
	if (AllowBackgroundRender && pRender->IsSubRenderStateLocked())
		AllowBackgroundRender = FALSE;

	// This should stop any problems with blends not fully rendering in separation views
	if (AllowBackgroundRender && pRender->IsPrinting())
		AllowBackgroundRender = FALSE;

	// If we are rendering (i.e. pRender != NULL) do some work with the quality settings
	if (pRender != NULL)
	{
		// If this blend is set for non-antialiased blend steps AND the quality is set for full antialiasing
		// we need to render an attr that means "do everything except antialias"
		if (IsNotAntialiased() && pRender->RRQuality.GetAntialiasQuality() == Quality::FullAntialias)
		{
			// Create a non-antialias quality attr to render
			Quality Qual;
			QualityAttribute *pQualAttrVal = new QualityAttribute(Qual);

			if (pQualAttrVal != NULL)
			{
				// Render the attribute, and mark it Temporary so it will be auto-deleted
				// when it is finished with.
				pRender->SetQuality(pQualAttrVal, TRUE);
			}
		}
	}

	// Get the start and end bounds so we can do fast blended bounds intersection tests
	// on the render region before going ahead with the heavy stuff
	DocRect BoundsStart = m_pNodeStart->GetBoundingRect();
	DocRect BoundsEnd   = m_pNodeEnd  ->GetBoundingRect();

	UINT32 NumBlendSteps = GetNumBlendSteps();

	// Check for silly numbers of blend steps
	if (NumBlendSteps > MAX_NUM_BLEND_STEPS) 
		NumBlendSteps = MAX_NUM_BLEND_STEPS;

	m_BlendStep = IsReversed() ? NumBlendSteps : 1;

	BlendSubRenderContext* pSubRenderContext = NULL;
	if (pRender != NULL)
	{
		pSubRenderContext = (BlendSubRenderContext*)pRender->GetSubRenderState();

//		ERROR3IF(pSubRenderContext != NULL && !IS_A(pSubRenderContext,BlendSubRenderContext),"Sub render context is of wrong type");

		// If we don't own the S.R.C, then we won't treat it as our own
		if (pSubRenderContext != NULL && !IS_A(pSubRenderContext,BlendSubRenderContext))
			pSubRenderContext = NULL;

		if (pSubRenderContext != NULL)
		{
		 	m_BlendStep = pSubRenderContext->BlendStep;
		}
	}

	// Find num blend paths in start and end, and keep hold of the MAX value
	UINT32 NumPathsInStart = m_pRefStart->GetNumBlendPaths();
	UINT32 NumPathsInEnd   = m_pRefEnd  ->GetNumBlendPaths();
	UINT32 MaxNumPaths	 = max(NumPathsInStart,NumPathsInEnd);

	// If either start or end has 0 blend paths, return.
	// DMc - not any more !
	if (min(NumPathsInStart,NumPathsInEnd) == 0)
	{
		return;
	}
	
	// Let the HandleBecomeA class know the max num paths in the start and end objects
	if (pHandleBecomeA != NULL)
		pHandleBecomeA->SetNumPathsPerStep(MaxNumPaths);

	// This object is used to step along the the two lists of objects choosing a pair to go and blend.
	// It mainly comes into play when there are different numbers of objects in the two lists.
	ListStepper Stepper;

	// BlendRatio	  - always a value between 0.0 and 1.0;
	// BlendRatioStep - the value that is added to BlendRatio on every blend step
	double BlendRatio;
	double BlendRatioStep = 1.0/(NumBlendSteps+1);

	Path BlendedPath;
	if (!BlendedPath.Initialise())
		return;

	// Do some stuff before we start blending (if this is the first time this node has been asked to render)
	if (pSubRenderContext == NULL)
	{
		PreBlend();

	}

	// This holds blended attributes that are generated during the blend
	CCAttrMap BlendedAttrMap(30);


	// If we will use background rendering, we must make sure nobody inside the blend tries
	// to do backgrounding as well, as we can only store one sub-render context. This bodge
	// locks the sub render state so that nobody else is allowed to use it. (Quick bodge
	// by Jason to allow strokes to background render, but not crash when they're inside
	// a blend - in this case, individual strokes become non-interruptible, and leave
	// the background render support entirely to the containing blend)
	// We must take care to see that we unlock the state when we are done rendering.
	if (AllowBackgroundRender)
		pRender->LockSubRenderState(TRUE);

	// ContinueRender is set to FALSE if we should stop rendering the blend
	BOOL				ContinueRender = TRUE;
	INT32					Counter = 1;
	UINT32				StopBlendStep = NumBlendSteps +1 ;

	if (IsReversed())
	{
		Counter = -1;
		StopBlendStep = 0;
	}

//	NodeRenderableInk * pLastCreatedByNode = NULL;
//	NodeRenderableInk * pLastBlendStartNode = NULL;

	// the parameter for the blending of nodes
	BlendNodeParam BNParam;

	// Graeme (21-11-00) - Tightened up the check on what was the for loop (and now a while
	// loop), so that it stops when the blend step gets to be equal to, or greater than, the
	// StopBlendStep value. (Or less than if it's a reversed blend.)
	//
	// The original code stopped when the current blend step equalled the stop value, which
	// meant that if the m_BlendStep value leapfrogged StopBlendStep an infinite loop could
	// occur. (The BlendRatio would get out of bounds pretty quick, though, and that would
	// stop the loop within a couple more iterations.)
	while ( ContinueRender &&
		    ( !IsReversed() && m_BlendStep < StopBlendStep ) ||
			( IsReversed() && m_BlendStep > StopBlendStep ) )
	{

		// Calc the blend ratio using the current blend step
		double LinearBlendRatio = BlendRatioStep * m_BlendStep;
		BlendRatio = MapObjectBlendRatio(LinearBlendRatio);
		
		// Get the stepper object ready for the current blend step
		INT32 NextPathStart,NextPathEnd;
		Stepper.Init(NumPathsInStart,NumPathsInEnd,0);

		// Reset the ptrs and subpath IDs of the previously blended blendpath pairs.
		BlendPath* 	pPrevBlendPathStart = NULL;
		BlendPath* 	pPrevBlendPathEnd   = NULL;
		UINT32 		PrevSubPathIDStart 	= 0;
		UINT32 		PrevSubPathIDEnd	= 0;

		// Get the first pair of objects to blend
		Stepper.GetNext(&NextPathStart,&NextPathEnd);

		// The first pair of blend paths become the current blend path pair
		// Get ptrs to these, plus the subpath IDs of these
		BlendPath* 	pCurrBlendPathStart = m_pRefStart->GetBlendPath(NextPathStart);
		BlendPath* 	pCurrBlendPathEnd   = m_pRefEnd  ->GetBlendPath(NextPathEnd  );
		UINT32 		CurrSubPathIDStart  = pCurrBlendPathStart->GetSubPathID();
		UINT32 		CurrSubPathIDEnd    = pCurrBlendPathEnd  ->GetSubPathID();

		// We also need info on the next blend path pair, so get some vars ready
		BlendPath* 	pNextBlendPathStart;
		BlendPath* 	pNextBlendPathEnd;
		UINT32 		NextSubPathIDStart;
		UINT32 		NextSubPathIDEnd;

		// Check to see if the blended bounds intersect with the render region's clip rect
		BOOL BoundsIntersect = TRUE;
		if (pRender != NULL && pHandleBecomeA == NULL)
			BoundsIntersect = CheckBoundsIntersect(pRender,BoundsStart,BoundsEnd,BlendRatio);

		DocRect	 RawBlendedBounds = BlendBounds(BoundsStart,BoundsEnd,BlendRatio,FALSE);
		DocCoord BBCentre = RawBlendedBounds.Centre();

		// CGS:  I have to allocate this here - because otherwise memory becomes toast within
		// subsequent iterations of the following loop ....
		SumAllPathsPathProcessor * ptrProc = new SumAllPathsPathProcessor (FALSE);
		BOOL* usingSumAllPathsPathProcessor = new BOOL;
		*usingSumAllPathsPathProcessor = FALSE;				// and whether we are using it or not
															// this is reset within each call to
		// BeginBlendStep ().  This allows us to select between conventional passback and
		// new SumAllPathsPathProcessor stuff when blending.

		// This thereby allows us to handles (.e.g):

		// simple shape to simple shape
		// shadow to shadow
		// grouped shadow(s) to shadow
		// shadowed group to shadow
		// shadow to simple shape
		// grouped shadow(s) to simple shape
		// shadowed group to simple shape
		// grouped shadow(s) to grouped shadow(s)
		// shadowed group to shadowed group
		// shadowed group to grouped shadow
		
		// you get the general gist ....

		// We need to blend all the blend paths together, so do MaxNumPaths blends
		for (UINT32 NextPath = 0; ContinueRender && BoundsIntersect && (NextPath < MaxNumPaths); NextPath++)
		{
			// Get the next pair of objects to blend
			Stepper.GetNext(&NextPathStart,&NextPathEnd);

			// Ensure GLA's which applied indiviually to child objects (ie individual BlendPaths within 
			// either BlendRef) are removed from the BlendAttrMap when changing to next BlendPath
			// If this doesn't occur then blended GLA left from Blend of previous BlendPath will be
			// applied to subsequent BlendPaths if they're not replaced (ie if subsequent BlendPaths
			// don't have a valid GLA applied)
			BlendedAttrMap.RemoveGeometryLinkedAttrs();

			if (NextPathStart >= 0 && NextPathEnd >= 0)
			{
				// There's another pair to blend, so get ptrs to them, plus their subpath IDs
				pNextBlendPathStart = m_pRefStart->GetBlendPath(NextPathStart);
				pNextBlendPathEnd   = m_pRefEnd  ->GetBlendPath(NextPathEnd  );
				NextSubPathIDStart	= pNextBlendPathStart->GetSubPathID();
				NextSubPathIDEnd 	= pNextBlendPathEnd  ->GetSubPathID();
			}
			else
			{
				// No more blend pairs after this one
				pNextBlendPathStart = NULL;
				pNextBlendPathEnd   = NULL;
				NextSubPathIDStart	= 0;
				NextSubPathIDEnd 	= 0;
			}

			// Check the ptrs to be on the safe side
			// DMc - we don't anymore - since if either are null this means we'll
			// go into the new code
			if (pCurrBlendPathStart == NULL || pCurrBlendPathEnd == NULL)
			{
				if (AllowBackgroundRender)
					pRender->LockSubRenderState(FALSE);			// Unlock the sub-render state
				return;
			}
			
			double AttrBlendRatio = MapAttrBlendRatio(LinearBlendRatio);

			NodeRenderableInk * pStartBlendNode = NULL;
			NodeRenderableInk * pEndBlendNode = NULL;

			if (LinearBlendRatio <= 0.5)	// CGS:  27/7/2000  We used to use BlendRatio for
											// this comparison.  We now use LinearBlendRatio since
											// this avoids the problem of 'compound jumping' when
											// blending from a compound to a non-compound
			{
				BNParam.Init(GetNodeBlend (), pRender, pCurrBlendPathStart, pCurrBlendPathEnd, BlendRatio,
					AttrBlendRatio, m_AngleStart, m_AngleEnd, GetObjectRatio(),
					GetInvertedAttributeRatio(), GetColourBlendType(),
					pCurrBlendPathStart->GetBlendNode(), pCurrBlendPathEnd->GetBlendNode(),
					IsOneToOne(), GetNodeBlendPath(), pHandleBecomeA, ptrProc);

				pStartBlendNode = pCurrBlendPathStart->GetBlendNode();
				pEndBlendNode = pCurrBlendPathEnd->GetBlendNode();
			}
			else
			{
				BNParam.Init(GetNodeBlend (), pRender, pCurrBlendPathEnd, pCurrBlendPathStart, 1.0 - BlendRatio,
					1.0 - AttrBlendRatio, m_AngleStart, m_AngleEnd, 1.0 - GetObjectRatio(),
					1.0 - GetInvertedAttributeRatio(), GetColourBlendType(),
					pCurrBlendPathEnd->GetBlendNode(), pCurrBlendPathStart->GetBlendNode(),
					IsOneToOne(), GetNodeBlendPath(), pHandleBecomeA, ptrProc);

				pStartBlendNode = pCurrBlendPathEnd->GetBlendNode();
				pEndBlendNode = pCurrBlendPathStart->GetBlendNode();
			}

			if (!CallBeginBlendStep(&BNParam, usingSumAllPathsPathProcessor))
			{
				return ;
			}

			if (BlendAttributes(pRender,pCurrBlendPathStart,pCurrBlendPathEnd,&BlendedAttrMap,AttrBlendRatio, BlendRatio, GetNodeBlend ()->GetObjectProfileProcessing ()))
			{
				// Blend the paths together, putting the blended path in BlendedPath
				if (BlendPaths(pCurrBlendPathStart,pCurrBlendPathEnd,BlendRatio))
				{
					// The blended path will be filled  if either of the paths are filled
					// The blended path will be stroked if either of the paths are stroked
					BOOL Filled  = pCurrBlendPathStart->IsFilled()  || pCurrBlendPathEnd->IsFilled();
					BOOL Stroked = pCurrBlendPathStart->IsStroked() || pCurrBlendPathEnd->IsStroked();
					
					// If the current subpath ID is the same as the previous one, and the current path is
					// different to the previous path, dont clear the blended path 
					// (this applies to the start OR the end path)
					BOOL DontClearPath = ((CurrSubPathIDStart == PrevSubPathIDStart && pCurrBlendPathStart != pPrevBlendPathStart) ||
						(CurrSubPathIDEnd   == PrevSubPathIDEnd   && pCurrBlendPathEnd   != pPrevBlendPathEnd  ));
					
					if (!DontClearPath)
						BlendedPath.ClearPath(FALSE);
					
					if (!BlendedPath.MakeSpaceInPath(m_ArrayLength)) goto ExitRender;
					BlendedPath.MergeTwoPaths(m_pTempCoords,m_pTempVerbs,m_pTempFlags,m_ArrayLength,Filled);
					BlendedPath.IsFilled  = Filled;
					BlendedPath.IsStroked = Stroked;
					
					// We haven't yet got a complete path to render or pass back if next path has the same ID and is
					// a different path
					// (this applies to the start OR the end path)
					BOOL NotACompletePath = ((CurrSubPathIDStart == NextSubPathIDStart && pCurrBlendPathStart != pNextBlendPathStart) ||
						(CurrSubPathIDEnd   == NextSubPathIDEnd   && pCurrBlendPathEnd   != pNextBlendPathEnd  ));
					
					if (!NotACompletePath)
					{
						// DY 9/99 added this condition to enable us to blend to 
						// and from blends on paths.
						
						
						if (m_BlendedOnCurve)
						{
							DocCoord PointOnPath;
							double Angle = 0.0;
							if (GetPointOnNodeBlendPath(BlendRatio,&PointOnPath,&Angle))									
							{
								Trans2DMatrix Translate(PointOnPath.x-BBCentre.x,PointOnPath.y-BBCentre.y);
								Trans2DMatrix Rotate(BBCentre,Angle);
								
								if (Angle != 0.0)
								{
									Rotate.Transform( (DocCoord*)BlendedPath.GetCoordArray(), BlendedPath.GetNumCoords() );
									BlendedAttrMap.Transform(Rotate);
								}
								
								Translate.Transform( (DocCoord*)BlendedPath.GetCoordArray(), BlendedPath.GetNumCoords() );
								// not a regular transform for the attributes, as we may want to tile
								BlendedAttrMap.TransformForBrush(Translate);
							}
						}

						// Ilan 4/5/00
						// Call the PostDynCreateInit functions for the attributes which require this
						// NB attrs which don't succeed in init'ing will mark themselves so that they don't NeedToRender()
						// ie the AttrMap can safely be rendered - the united attrs simply won't do their job
						BlendedAttrMap.PostBlendInit(&BlendedPath, GetRuntimeClass());
						
						// CGS:  Now are we doing a convert to editable shapes ????

						if (pHandleBecomeA != NULL)
						{
							// we are - so test if we have a SumAllPathsPathProcessor ....
							
							if (!(*usingSumAllPathsPathProcessor))
							{
								// we don't - so we are not dealing with compound nodes
								// Pass the path back (as usual)
								ContinueRender = pHandleBecomeA->PassBack(&BlendedPath,&BlendedAttrMap,m_BlendStep);
							}
							else
							{	
								// we do - so we are dealing with compound nodes ....

								if  ((pHandleBecomeA->GetBecomeA ()->GetReason () == BECOMEA_REPLACE) ||
									 (pHandleBecomeA->GetBecomeA ()->GetReason () == BECOMEA_PASSBACK)
									)
								{
									Path* pNewBlendedPath = new Path;
									pNewBlendedPath->Initialise (BlendedPath.GetNumCoords ());
									
									pNewBlendedPath->CopyPathDataFrom (&BlendedPath);

									CCAttrMap* pNewBlendedAttrMap = BlendedAttrMap.Copy ();
									
									SumAllPathsElem * pElem = new SumAllPathsElem(pNewBlendedPath, pNewBlendedAttrMap);

									ptrProc->InsertSumAllPathsElem (pElem);

									ContinueRender = TRUE;

									delete (pNewBlendedPath);
								}
							}
						}

						if (pRender != NULL)
						{
							pRender->SaveContext();

							// CGS:

							if (!(*usingSumAllPathsPathProcessor))
							{
								RenderAttributes(pRender,&BlendedAttrMap);
											
								pRender->DrawPath(&BlendedPath);
							}
							else
							{
								Path* pNewBlendedPath = new Path;
								pNewBlendedPath->Initialise (BlendedPath.GetNumCoords ());
									
								pNewBlendedPath->CopyPathDataFrom (&BlendedPath);

								CCAttrMap* pNewBlendedAttrMap = BlendedAttrMap.Copy ();
									
								SumAllPathsElem * pElem = new SumAllPathsElem(pNewBlendedPath, pNewBlendedAttrMap);

								ptrProc->InsertSumAllPathsElem (pElem);

								ContinueRender = TRUE;

								delete (pNewBlendedPath);
							}
						
							pRender->RestoreContext();
						}

						// Ilan 4/5/00
						// Deinitialise the attrs which allocated memory in the PostBlendInit() fn call
						BlendedAttrMap.PostBlendDeinit();
					}
				}
			}

			TRACEUSER( "ChrisS", _T("Blend step == %f\n"), BlendRatio);

  			// restore the blend paths
			if (!CallEndBlendStep(&BNParam))
			{
				return ;
			}
			
			// Set up the vars for the next pair of blend paths
			// The Current vars become the previous vars
			PrevSubPathIDStart 	= CurrSubPathIDStart;
			PrevSubPathIDEnd   	= CurrSubPathIDEnd;
			pPrevBlendPathStart = pCurrBlendPathStart;
			pPrevBlendPathEnd   = pCurrBlendPathEnd;

			// The next vars become the current vars
			CurrSubPathIDStart 	= NextSubPathIDStart;
			CurrSubPathIDEnd   	= NextSubPathIDEnd;
			pCurrBlendPathStart = pNextBlendPathStart;
			pCurrBlendPathEnd   = pNextBlendPathEnd;
		}

		if (usingSumAllPathsPathProcessor != NULL)
		{
			delete (usingSumAllPathsPathProcessor);		// clear out the boolean that was allocated
		}
		
		if (ptrProc != NULL)
		{
			delete (ptrProc);		//  clear out the path processor if we allocated one
		}

		// This section communicates with the render region (if there is one) to see if
		// we have taken too long to render this step, and hence need to update the screen during
		// background redraw.

		// Have we got a render region and have we actually rendered anything on this step?
		if (pRender != NULL && BoundsIntersect)
		{
			View* pView = pRender->GetRenderView();
			if (pView != NULL)
			{
 				if (AllowBackgroundRender && (pRender->IsHitDetect() || !pRender->RenderTreeCanContinue()))
				{
					// CanContinue() has returned FALSE, so we need to try and save the sub render
					// context so that the render region can call us back next time

					// Have we got a sub render context object?
					if (pSubRenderContext == NULL)
					{
						// If not, create a new BlendSubRenderContext object and make this the one
						// the render region should use via SetSubRenderState()
						pSubRenderContext = new BlendSubRenderContext;
						if (pSubRenderContext != NULL && pRender->GetSubRenderState() != pSubRenderContext)
						{
							// Turn off the sub render lock now, or my silly ERROR3 will go off!
							pRender->LockSubRenderState(FALSE);
							pRender->SetSubRenderState(pSubRenderContext);
						}
					}

					if (pSubRenderContext != NULL)
					{
						// If we have a sub render context object, set it up and exit this render
						// call at an appropriate point
						pSubRenderContext->BlendStep = m_BlendStep+Counter;	// Render the next blend step next time
						goto ExitRender;
					}
				}
			}
			
		}

		// Increment the counter.
		m_BlendStep+= Counter;

	} // end while

	if (pRender != NULL && pSubRenderContext != NULL)
	{
		// Here we have finished rendering the whole blender node
		// We must make sure we delete the sub render context object, and set the render region's
		// sub render state to NULL
		delete pSubRenderContext;
		pSubRenderContext = NULL;
		pRender->SetSubRenderState(NULL);
	}
	
	// Do some stuff after the blend
	PostBlend();

ExitRender:
	// delete any attributes that are left in the blended attr map
	BlendedAttrMap.DeleteAttributes();
	
	// Finally, unlock the sub-render context so others may use it (now that it is relatively safe)
	if (AllowBackgroundRender)
		pRender->LockSubRenderState(FALSE);


	TRACEUSER( "ChrisS", _T("Exiting create blends\n"));
}  


/********************************************************************************************

> BOOL NodeBlender::CheckBoundsIntersect(RenderRegion* pRender,DocRect& BoundsStart,DocRect& BoundsEnd,double BlendRatio)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/12/94
	Inputs:		pRender		= ptr to the current render region
				BoundsStart = the blob bounds of start object
				BoundsEnd   = the blob bounds of end object
				BlendRatio  = current blend point (between 0.0 and 1.0)
	Outputs:	-
	Returns:	TRUE if blended bounds intersect with the render region's clip rect.
	Purpose:	Finds the blended bounds that lies at point BlendRatio and tests for intersection.
	SeeAlso:	-

********************************************************************************************/

BOOL NodeBlender::CheckBoundsIntersect(RenderRegion* pRender,DocRect& BoundsStart,DocRect& BoundsEnd,double BlendRatio)
{
	BOOL Clip = TRUE;

	if (pRender != NULL)
	{
		DocRect BlendedBounds = BlendBounds(BoundsStart,BoundsEnd,BlendRatio,TRUE);
		Clip = BlendedBounds.IsIntersectedWith(pRender->GetClipRect());
		//pRender->DrawRect(&BlendedBounds);
	}

	return Clip;
}

/********************************************************************************************

> DocRect NodeBlender::BlendBounds(DocRect& BoundsStart,DocRect& BoundsEnd,double BlendRatio)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/5/99
	Inputs:		BoundsStart = the blob bounds of start object
				BoundsEnd   = the blob bounds of end object
				BlendRatio  = current blend point (between 0.0 and 1.0)
	Outputs:	-
	Returns:	The blended bounds.
	Purpose:	Finds the blended bounds that lies at point BlendRatio and tests for intersection.
				Takes into account the path the blend is following, if it has one.
	SeeAlso:	-

********************************************************************************************/

DocRect NodeBlender::BlendBounds(DocRect& BoundsStart,DocRect& BoundsEnd,double BlendRatio,BOOL UseBlendPath)
{
	DocRect BlendedBounds,OldBoundsStart,OldBoundsEnd;

	if (UseBlendPath)
	{
		OldBoundsStart = BoundsStart;
		OldBoundsEnd   = BoundsEnd;
		RotateBounds(BoundsStart,-m_AngleStart);
		RotateBounds(BoundsEnd,-m_AngleEnd);
	}

	BlendedBounds.lo.x = INT32(BoundsStart.lo.x + ((BoundsEnd.lo.x-BoundsStart.lo.x)*BlendRatio));
	BlendedBounds.lo.y = INT32(BoundsStart.lo.y + ((BoundsEnd.lo.y-BoundsStart.lo.y)*BlendRatio));
	BlendedBounds.hi.x = INT32(BoundsStart.hi.x + ((BoundsEnd.hi.x-BoundsStart.hi.x)*BlendRatio));
	BlendedBounds.hi.y = INT32(BoundsStart.hi.y + ((BoundsEnd.hi.y-BoundsStart.hi.y)*BlendRatio));

	if (UseBlendPath)
	{
		DocCoord PointOnPath;
		double Angle = 0.0;
		if (GetPointOnNodeBlendPath(BlendRatio,&PointOnPath,&Angle))
		{
			DocCoord BBCentre = BlendedBounds.Centre();
			BlendedBounds.Translate(PointOnPath.x-BBCentre.x,PointOnPath.y-BBCentre.y);

			RotateBounds(BlendedBounds,Angle);
		}
	}

	if (UseBlendPath)
	{
		BoundsStart = OldBoundsStart;
		BoundsEnd   = OldBoundsEnd;
	}

	return BlendedBounds;
}

/********************************************************************************************

> void NodeBlender::RotateBounds(DocRect& Bounds,double Angle)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/5/99
	Inputs:		Bounds = the bounds to rotate
				Angle  = the angle in degrees by which to rotate it by
	Outputs:	-
	Returns:	-
	Purpose:	The bounds are updated to contain the rotated version
	SeeAlso:	-

********************************************************************************************/

void NodeBlender::RotateBounds(DocRect& Bounds,double Angle)
{
	if (Angle != 0.0)
	{
		DocCoord BBCentre = Bounds.Centre();

		DocCoord Coords[4];
		Coords[0].x = Bounds.lo.x;	Coords[0].y = Bounds.lo.y;
		Coords[1].x = Bounds.hi.x;	Coords[1].y = Bounds.lo.y;
		Coords[2].x = Bounds.hi.x;	Coords[2].y = Bounds.hi.y;
		Coords[3].x = Bounds.lo.x;	Coords[3].y = Bounds.hi.y;

		Trans2DMatrix Rotate(BBCentre,Angle);
		Rotate.Transform(Coords,4);

		Bounds.lo.x = min(min(Coords[0].x,Coords[1].x), min(Coords[2].x,Coords[3].x));
		Bounds.hi.x = max(max(Coords[0].x,Coords[1].x), max(Coords[2].x,Coords[3].x));
		Bounds.lo.y = min(min(Coords[0].y,Coords[1].y), min(Coords[2].y,Coords[3].y));
		Bounds.hi.y = max(max(Coords[0].y,Coords[1].y), max(Coords[2].y,Coords[3].y));
	}
}

/********************************************************************************************

> Trans2DMatrix* NodeBlender::GetRotateMatrix(NodeRenderableBounded* pNode,double Angle)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/5/99
	Inputs:		pNode = the node who's bounds define the centre of rotation
				Angle = Angle to rotate by, in degrees
	Outputs:	-
	Returns:	ptr to the matrix that will perform the rotation
				or NULL if no rotation is necessary (i.e. Angle % 360.0 == 0.0)
	Purpose:	Function that returns a matrix that will rotate around the centre of the bounds of
				the given node
	SeeAlso:	-

********************************************************************************************/

Trans2DMatrix* NodeBlender::GetRotateMatrix(NodeRenderableBounded* pNode,double Angle)
{
	Trans2DMatrix* pMatrix = NULL;

	if (pNode != NULL && fmod(Angle,360.0) != 0.0)
	{
		DocRect BoundsStart = pNode->GetBoundingRect();
		DocCoord Centre = BoundsStart.Centre();
		pMatrix = new Trans2DMatrix(Centre,Angle);
	}

	return pMatrix;
}

/********************************************************************************************

> DocRect NodeBlender::GetUnionBlendedBounds(DocRect& BoundsStart,DocRect& BoundsEnd)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/5/99
	Inputs:		-
	Outputs:	-
	Returns:	The union of blended bounds.
	Purpose:	Finds the blended bounds that lies at point BlendRatio and tests for intersection.
				Takes into account the path the blend is following, if it has one.
	SeeAlso:	-

********************************************************************************************/

DocRect NodeBlender::GetUnionBlendedBounds(DocRect& BoundsStart,DocRect& BoundsEnd)
{
	DocRect UnionBlendedBounds(0,0,0,0);

	UINT32 NumBlendSteps = GetNumBlendSteps();
	double BlendRatio = 0.0;
	double BlendRatioStep = 1.0/(NumBlendSteps+1);

	for (UINT32 BlendStep = 1; BlendStep <= NumBlendSteps; BlendStep++)
	{
		// Calc the blend ratio using the current blend step
		BlendRatio = BlendRatioStep * BlendStep;

		DocRect BlendedBounds = BlendBounds(BoundsStart,BoundsEnd,BlendRatio,TRUE);

		if (BlendStep == 1)
			UnionBlendedBounds = BlendedBounds;
		else
			UnionBlendedBounds = UnionBlendedBounds.Union(BlendedBounds);
	}

	return UnionBlendedBounds;
}

/********************************************************************************************

>	void NodeBlender::RenderAttributes(RenderRegion* pRender,CCAttrMap* pAttrMap)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/10/94
	Inputs:		pRender = ptr to render region
				pAttrMap = ptr to map of ttributes
	Outputs:	-
	Returns:	-
	Purpose:	Renders the attributes to the given render region
	SeeAlso:	-

********************************************************************************************/

void NodeBlender::RenderAttributes(RenderRegion* pRender,CCAttrMap* pAttrMap)
{
	// Check entry params
	BOOL ok = (pRender != NULL && pAttrMap != NULL);
	ERROR3IF(!ok,"One or more NULL entry params");
	if (!ok) return;

	CCRuntimeClass* pType;
	void* pVal;

	// iterating all (key, value) pairs
	for (CMapPtrToPtr::iterator Pos = pAttrMap->GetStartPosition(); Pos != pAttrMap->GetEndPosition();)
	{
		// Get attr at CMapPtrToPtr::iterator Pos
		pAttrMap->GetNextAssoc(Pos,pType,pVal);

		// Don't render quality attributes, to ensure steps are rendered with the same quality
		// as the start and end objects.
		if (pType != CC_RUNTIME_CLASS(AttrQuality))
		{
			// pVal is actually an attribute, so get a ptr to it and render it
			NodeAttribute* pNodeAttr = (NodeAttribute *)pVal;
		
			// *********+**** BODGE (removed 1/12/94) ****************
			// Ignore these two attrs until I get Wills attr changes, involving unimplemented Restore() funcs
			// 	if (!pNodeAttr->IS_KIND_OF(AttrDashPattern) && !pNodeAttr->IS_KIND_OF(AttrTranspFillMapping))
			//		pNodeAttr->Render(pRender);

		if (pNodeAttr->IsABrush())
		{
			//TRACEUSER( "Diccon", _T("Blend step %d, rendering Brush\n"), m_BlendStep);
			PathProcessor* pPathProc = pRender->GetFirstPathProcessor();
			if (pPathProc != NULL && pPathProc->IsAPathProcessorBrush())
				continue;
		}
			pNodeAttr->Render(pRender);
		}
	}
}

/********************************************************************************************

>	BOOL NodeBlender::BlendAttributes(RenderRegion *pRender, BlendPath* pBlendPathStart,
								  BlendPath* pBlendPathEnd,CCAttrMap* pBlendedAttrMap,
								  double BlendRatio, double objectRatio,
								  BOOL objectProfileProcessing)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/10/94
	Inputs:		pRender				= The Render Region into which we are rendering the blend, or
										NULL if not rendering (i.e. doing a make shapes)
				pBlendPathStart 	= ptr to blend path to blend from
				pBlendPathEnd   	= ptr to blend path to blend to
				pBlendedAttrMap 	= ptr to map to store the blended attributes in
				BlendRatio   		= amount to blend by (0.0 <= BlendPath <= 1.0)
				objectRatio			= the corresponding object ratio (CGS:  allows us to blend
									  object stuff with attributes)
				objectProfileProcessing = whether we are allowed to blend object (i.e.  CMapPtrToPtr::iterator)
									  data of attributes
	Outputs:	-
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	Blends the attributes of the two BlendPath objects by the amount specified in BlendRatio
	SeeAlso:	-

********************************************************************************************/

BOOL NodeBlender::BlendAttributes(RenderRegion *pRender, BlendPath* pBlendPathStart,
								  BlendPath* pBlendPathEnd,CCAttrMap* pBlendedAttrMap,
								  double BlendRatio, double objectRatio,
								  BOOL objectProfileProcessing)
{
	// Check entry params
	BOOL ok = (pBlendPathStart != NULL &&
				pBlendPathEnd != NULL && pBlendedAttrMap != NULL);
	ERROR3IF(!ok,"One or more NULL entry params");
	if (!ok) return FALSE;

	// Find the attributes that are applied to the blend paths
	CCAttrMap* pAttrMapStart = m_pRefStart->FindAppliedAttributes(pBlendPathStart);
	CCAttrMap* pAttrMapEnd   = m_pRefEnd  ->FindAppliedAttributes(pBlendPathEnd);

	// If either are NULL, return FALSE
	if (pAttrMapStart == NULL || pAttrMapEnd == NULL) return FALSE;

//#ifdef _DEBUG
//	AttrTranspFillGeometry* pLineWidth = NULL;
//	pMap->Lookup((void *)CC_RUNTIME_CLASS(AttrLineWidth), (void *&)pIndent);
//	//UINT32* pCol = pLineWidth->GetStartTransp ();
//
//	pAttrMapEnd->Lookup((void*) CC_RUNTIME_CLASS(AttrTranspFillGeometry),(void* &)pLineWidth);
//	//ol = pLineWidth->GetStartTransp ();
//#endif

	pAttrMapStart->attrMapCreator = pBlendPathStart->GetBlendNode ();
	pAttrMapEnd->attrMapCreator = pBlendPathEnd->GetBlendNode ();

	if (GetNodeBlendPath() != NULL)
	{
		Trans2DMatrix* pRotateStart = GetRotateMatrix(m_pNodeStart,360.0 - m_AngleStart);
		Trans2DMatrix* pRotateEnd   = GetRotateMatrix(m_pNodeEnd  ,360.0 - m_AngleEnd  );
		if (pRotateStart)	pAttrMapStart->Transform(*pRotateStart);
		if (pRotateEnd)		pAttrMapEnd  ->Transform(*pRotateEnd);
		DELPTR(pRotateStart);
		DELPTR(pRotateEnd);
	}

	// Diccon 24/5/2000 Brush attributes need to know about stroke colour, so we need to 
	// keep track of both as we find them
	AttrBrushType* pBlendedAttrBrush = NULL;
	AttrStrokeColour* pBlendedStrokeColour = NULL;;
		
	// These vars are used as params to the CCAttrMap funcs
	CCRuntimeClass* pTypeStart;
	void* pValStart;
	void* pValEnd;
	double OldBlendRatio = BlendRatio;
	// Process each attribute in turn
	CMapPtrToPtr::iterator PosStart = pAttrMapStart->GetStartPosition();
	while ( PosStart != pAttrMapStart->GetEndPosition() )
	{
		// Get a ptr to the attr at CMapPtrToPtr::iterator PosStart in the start node's attr map
		pAttrMapStart->GetNextAssoc(PosStart,pTypeStart,pValStart);
		NodeAttribute* pNodeAttrStart = (NodeAttribute *)pValStart;
	
		BlendRatio = OldBlendRatio;	
		// Diccon 10/99 When using non-linear profiles for the objects those attributes
		// that make use of control points were not being profiled, making the objects look strange.
		// to avoid this those attributes now share the same profiles as the objects.
		if (pNodeAttrStart->IsAGradFill())
		{
		
			if (!((AttrFillGeometry*)pNodeAttrStart)->IsAColourFill())
			{
				
				BlendRatio = GetObjectRatio();
			
			}
			else
			{
				BlendRatio = GetInvertedAttributeRatio();
			
			}

		}
		if (pNodeAttrStart->IsAFlatFill() || (pNodeAttrStart->GetRuntimeClass() == CC_RUNTIME_CLASS(AttrLineWidth)))
		{
			BlendRatio = GetInvertedAttributeRatio();
		}
	
		// Get a blended attribute
		NodeAttribute* pBlendedNodeAttr = NULL;

		// Find an attr of the same type in the end object's attr list,
		// and blend the two attrs together
		if (pAttrMapEnd->Lookup(pTypeStart,pValEnd))
		{
			// We've found a matching end attr, so try to blend it with the start attr

			// Set up the param object to pass to the start attr's blend method
			BlendAttrParam BlendParam;

			// Initialise the BlendParam with the end attr and blend ratio
			if (BlendParam.Init(pRender,
								(NodeAttribute *)pValEnd,BlendRatio,objectRatio,objectProfileProcessing,GetColourBlendType(),
								pAttrMapStart, pAttrMapEnd) )
			{
				// Successfully initialised, so now try blending the attributes
				if (pNodeAttrStart->Blend(&BlendParam))
				{
					// Attrs successfully blended, now get a ptr to the new attr.
					// Once we get the blended attr ptr, it belongs to us, so we have
					// to delete it when it is not needed
					pBlendedNodeAttr = BlendParam.GetBlendedAttr();
				}
				else
				{
#ifdef _DEBUG
					void *tmp = NULL;
					ENSURE(!pBlendedAttrMap->Lookup(pNodeAttrStart->GetRuntimeClass(),tmp),"Didn't blend attr, but there's still one in pBlendedAttrMap");
#endif
				}
			}
		}

		// If we have a blended attr, pBlendedNodeAttr != NULL
		if (pBlendedNodeAttr != NULL)
		{
			// Get the type of the blended attr
			CCRuntimeClass* pTypeBlend = pBlendedNodeAttr->GetAttributeType();
			void* pValBlend;

			// If we already have an attr in the blended attr map of the same type,
			// remove it and delete it, before inserting a new attr of this type
			if (pBlendedAttrMap->Lookup(pTypeBlend,pValBlend))
			{
				if (pValBlend != NULL)
				{
					pBlendedAttrMap->RemoveKey(pTypeBlend);
					delete (NodeAttribute*)pValBlend;
				}
			}
			// add it to the blend map
			pBlendedAttrMap->SetAt(pTypeBlend,pBlendedNodeAttr);

			// find out if we are a stroke colour or a brush
			if (pBlendedNodeAttr->IsAStrokeColour())
				pBlendedStrokeColour = (AttrStrokeColour*)pBlendedNodeAttr;
			if (pBlendedNodeAttr->IsABrush())
				pBlendedAttrBrush = (AttrBrushType*)pBlendedNodeAttr;
		}
	}

	if (GetNodeBlendPath() != NULL)
	{
		Trans2DMatrix* pRotateStart = GetRotateMatrix(m_pNodeStart,m_AngleStart);
		Trans2DMatrix* pRotateEnd   = GetRotateMatrix(m_pNodeEnd  ,m_AngleEnd  );
		if (pRotateStart)	pAttrMapStart->Transform(*pRotateStart);
		if (pRotateEnd)		pAttrMapEnd	 ->Transform(*pRotateEnd);
		DELPTR(pRotateStart);
		DELPTR(pRotateEnd);
	}

	// if we got a stroke colour and a brush then tell the brush what the stroke colour is
	if (pBlendedAttrBrush != NULL && pBlendedStrokeColour != NULL)
		pBlendedAttrBrush->SetBlendedStrokeColour(&(pBlendedStrokeColour->Value.Colour));

	return TRUE;
}

/********************************************************************************************

>	BOOL NodeBlender::BlendPaths(BlendPath* pBlendPathStart,BlendPath* pBlendPathEnd,double BlendRatio)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/10/94
	Inputs:		pBlendPathStart = ptr to blend path to blend from
				pBlendPathEnd   = ptr to blend path to blend to
				BlendRatio  	= amount to blend by (0.0 <= BlendPath <= 1.0)
	Outputs:	The blended path is stored in three arrays: the coords, the verbs, and the flags.
				The arrays are:
	
					pTempCoords
					pTempVerbs
					pTempFlags

					ArrayLength = the length of all three arrays

				This allows the caller to decide what to do with the blended path in a very flexible way.

	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	Blends two BlendPath objects by the amount specified in BlendRatio
	SeeAlso:	-

********************************************************************************************/

BOOL NodeBlender::BlendPaths(BlendPath* pBlendPathStart,BlendPath* pBlendPathEnd,double BlendRatio)
{
	// Check entry params
	BOOL    ok = (pBlendPathStart != NULL && pBlendPathEnd != NULL);
 	if (ok) ok = (pBlendPathStart->GetBlendNode() != NULL && pBlendPathEnd->GetBlendNode() != NULL);
	ERROR3IF(!ok,"One or more NULL entry params");
	if (!ok) return FALSE;

	// Get the types of the two paths
	PathTypeEnum PathTypeStart = pBlendPathStart->GetPathType();
	PathTypeEnum PathTypeEnd   = pBlendPathEnd  ->GetPathType();

	// The blended path will be closed if either of the paths is a shape
	BOOL Closed = (PathTypeStart == PATHTYPE_SHAPE) || (PathTypeEnd == PATHTYPE_SHAPE);

	Path * pPathStart = NULL;

	// Find the paths associated with the start and end blend paths
	if (pBlendPathStart->GetBlendNode()->IsNodePath())
	{
		pPathStart = &(((NodePath *)pBlendPathStart->GetBlendNode())->InkPath);
	}
	else
	{
		pPathStart = &(((NodePath *)((NodeCompound *)pBlendPathStart->GetBlendNode())->GetNodeToBlend())->InkPath);
	}

	Path * pPathEnd = NULL;

	if (pBlendPathEnd->GetBlendNode()->IsNodePath())
	{
		pPathEnd   = &(((NodePath *)pBlendPathEnd->GetBlendNode())->InkPath);
	}
	else
	{
		pPathEnd = &(((NodePath *)((NodeCompound *)pBlendPathEnd->GetBlendNode())->GetNodeToBlend())->InkPath);
	}

	// Calculate how large the arrays have to be to store the blended path definition
	INT32 DestPathSize = ((pPathStart->GetNumCoords()+pPathEnd->GetNumCoords())*3)+500;

	// Get some arrays used to hold the blended path data, and error if any are NULL
	DocCoord*  	pDestCoords = GetCoordArray(DestPathSize);
	PathVerb*  	pDestVerbs  = GetVerbArray(DestPathSize);
	PathFlags* 	pDestFlags  = GetFlagArray(DestPathSize);
	UINT32* 		pBuff 		= GetGBlendBuff(DestPathSize);
	if (pDestCoords == NULL || pDestVerbs == NULL || pDestFlags == NULL || pBuff == NULL)
		return FALSE;

	// This section copes with the case when blending a line with a shape.
	// In this case we need to get a temp path the is actually a shape version of the line.
	// The line is simply reversed back onto itself to form a shape that would look identical to the 
	// line if rendered.  This allows the line to appear to open up to the shape when blended.
	Path Shape;
	if (PathTypeStart != PathTypeEnd)
	{
		BOOL ok = FALSE;
		if (!Shape.Initialise()) return FALSE;

		// if going from a line to a shape, convert the start path to a shape
		if (PathTypeStart == PATHTYPE_LINE && PathTypeEnd == PATHTYPE_SHAPE)
		{
			ok = NodeBlender::ConvertLineToShape(pPathStart,&Shape);
			pPathStart = &Shape;
		}

		// if going from a shape to a line, convert the end path to a shape
		if (PathTypeStart == PATHTYPE_SHAPE && PathTypeEnd == PATHTYPE_LINE)
		{
			ok = NodeBlender::ConvertLineToShape(pPathEnd,&Shape);
			pPathEnd = &Shape;
		}

		if (!ok) return FALSE;
	}

	// The blend should do a one-to-one mapping when the OneToOne flag is set AND both paths
	// have the same number of elements
	BOOL OneToOne = FALSE;
	if (IsOneToOne())
		OneToOne = (pBlendPathStart->GetNumElements() == pBlendPathEnd->GetNumElements());

	// Now actually blend the two paths

	GBlend GBlendObj;

	// Define the blend
	GBlendObj.Define(	(PPOINT)pPathStart->GetCoordArray(),	// Specify the start path
						pPathStart->GetVerbArray(),
						pPathStart->GetNumCoords(),

						(PPOINT)pPathEnd  ->GetCoordArray(),	// Specify the end path
						pPathEnd  ->GetVerbArray(),
						pPathEnd  ->GetNumCoords(),

						OneToOne,								// The one-to-one flag
						FLATNESS,								// Flatness

						pBuff,									// Buffer for GBlend to use
						DestPathSize*sizeof(UINT32));			// The buffer size

	// Blend the paths
	m_ArrayLength = GBlendObj.Blend(BlendRatio,					// The blend ratio, 0.0 < BlendRatio < 1.0
									(PPOINT)pDestCoords,		// Array to store blended coords
									pDestVerbs,					// Array to store blended verbs
									DestPathSize);				// The num elements in the two arrays


	// If we're blending a line to another line, we need to make sure that the blended line
	// is going in a direction that corresponds to the source lines.  This ensures attributes
	// that depend on this direction (e.g. start and end arrows) look correct.
	//
	// When creating blend paths of lines, we can detect if the blend path has been reversed,
	// in relation to the original path, by the original mapping value.
	// If it's 0 it has NOT been reversed, otherwise it's been reversed.
	//
	// If the blend ratio is <=0.5, the blended path is closest to the start blend path,
	// so we look at the start blend path's original mapping.
	//
	// If blend ration > 0.5, look at the end blend path's original mapping.
	//
	// The (BlendRation <= 0.5) cut-off point is the same as the cut-off point used in the blending
	// of attributes.
	if (pBlendPathStart->IsLine() && pBlendPathEnd->IsLine())
	{
		BlendPath* pBlendPath;
		if (BlendRatio <= 0.5) 
			pBlendPath = pBlendPathStart;
		else
			pBlendPath = pBlendPathEnd;

		if (pBlendPath->GetOrigMapping() != 0)
			ReversePath(pDestCoords,pDestVerbs,m_ArrayLength);
	}

	// We need to do some work on the blended path
	if (!ProcessBlendedPath(pDestCoords,pDestVerbs,pDestFlags,m_ArrayLength,Closed))
		return FALSE;

	return TRUE;
}


/********************************************************************************************

>	void NodeBlender::ReversePath(DocCoord* pCoords,PathVerb* pVerbs,UINT32 Len)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/1/96
	Inputs:		pCoords = ptr to coords array
				pVerbs  = ptr to verbs array
				Len		= the length of above arrays
	Outputs:	-
	Returns:	-
	Purpose:	This will reverse the path definition given in the two arrays.

				Path flags are not delt with as these are specific to the Path class.
				This function is designed to process raw paths returned from Gavin.
	SeeAlso:	-

********************************************************************************************/

void NodeBlender::ReversePath(DocCoord* pCoords,PathVerb* pVerbs,UINT32 Len)
{
	// First we will reverse all the Coords
	UINT32 LastPos  = Len-1;
	UINT32 FirstPos = 0;
	while (FirstPos<LastPos)
	{
		SWAP(DocCoord,pCoords[FirstPos],pCoords[LastPos]);	// change the order of the coords in the array

		// find the next two coords to swap
		FirstPos++;
		LastPos--;
	}

	// Now reverse the verbs.
	// The first verb always stays as a MoveTo and everything else gets sorted around it
	FirstPos = 1;
	LastPos  = Len-1;
	while (FirstPos<LastPos)
	{
		SWAP(PathVerb,pVerbs[FirstPos],pVerbs[LastPos]);

		// Find the next two verbs to swap
		FirstPos++;
		LastPos--;
	}
}

/********************************************************************************************

>	void NodeBlender::UpdateBlendStartAngles (Trans2DMatrix& trans)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/7/2000
	Inputs:		trans	=	the transformation matrix
	Outputs:	-
	Returns:	-
	Purpose:	Updates a blend on curves start angles after we have rotated the blend.
	SeeAlso:	-

	NOTE:		If problems arise the function should be modified to store undo information.
				However at present, this seems to be generated for us somewhere else; which
				is fine by me.

********************************************************************************************/

void NodeBlender::UpdateBlendStartAngles (Trans2DMatrix& trans)
{
	// only update if these are non-zero
	if ((m_AngleStart != 0.0 || m_AngleEnd != 0.0))
	{
		double ang = (trans.GetRotation ()).MakeDouble ();
		
		m_AngleStart	+= ang;
		m_AngleEnd		+= ang;
	}
}



/********************************************************************************************

>	virtual INT32 NodeBlender::EstimateNodeComplexity (OpParam* details)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/09/2000

	Inputs:		details		any data that should be used for the calculation

	Outputs:	-

	Returns:	an estimate of the nodes complexity

	Purpose:	This function estimates a complexity value for the node.  The complexity
				value is based upon the total length of all paths in the node.

	See Also:	OpBlendNodes::DeterminBlendObjectsProcessorHit ()

********************************************************************************************/

INT32 NodeBlender::EstimateNodeComplexity (OpParam* details)
{
	//m_pNodeStart,&m_pRefStart

	if ((m_pNodeStart == NULL) || (m_pNodeEnd == NULL))
	{
		return (-1);
	}
	
	INT32 complexityEstimate1 = m_pNodeStart->EstimateNodeComplexity (details);
	INT32 complexityEstimate2 = m_pNodeEnd->EstimateNodeComplexity (details);

	UINT32 NumBlendStepsby2 = 0;
	if (!details)
	{
		NumBlendStepsby2 = GetNumBlendSteps()/2;
	}
	else
	{
		NumBlendStepsby2 = (UINT32) (INT32) details->Param1;
	}

	INT32 generatedPathsEstimate = NumBlendStepsby2+1*complexityEstimate1 + NumBlendStepsby2*complexityEstimate2;

	return (generatedPathsEstimate);
}

/********************************************************************************************

>	BOOL NodeBlender::ConvertLineToShape(Path* pPath,Path* pShapePath)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/11/94
	Inputs:		pPath 		= ptr to the line to convert
				pShapePath	= ptr to path to store the shape version of the line
	Outputs:	-
	Returns:	TRUE if OK, FALSE if it goes bang
	Purpose:	This takes a line and creates a shape out of it, placing the shape in pShapePath.

				It does this:
					A copy of the line is made, and reversed.
					The reversed copy has its start movetto element removed.
					A shape is formed by merging the line with the reversed copy.

				This allows us to blend a line with a shape, giving the impression that the line 
				opens out into the shape.
	SeeAlso:	-

********************************************************************************************/

BOOL NodeBlender::ConvertLineToShape(Path* pPath,Path* pShapePath)
{
	ERROR3IF(pPath == NULL || pShapePath == NULL,"One or more NULL entry params");
	if (pPath == NULL || pShapePath == NULL) return FALSE;

	Path ReversePath;
	if (!ReversePath.Initialise(pPath->GetNumCoords())) return FALSE;
	if (!ReversePath.CopyPathDataFrom(pPath)) return FALSE;

	PathVerb* pRevVerb = ReversePath.GetVerbArray();
	PathVerb* pShapeVerb = pShapePath->GetVerbArray();

	ReversePath.Reverse();
	ReversePath.FindStartOfPath();
	ReversePath.DeleteElement();

	pRevVerb = ReversePath.GetVerbArray();
	pShapeVerb = pShapePath->GetVerbArray();

	pShapePath->ClearPath(FALSE);
	if (!pShapePath->MergeTwoPaths(*pPath)) return FALSE;
	if (!pShapePath->MergeTwoPaths(ReversePath)) return FALSE;

	return TRUE;
}

/********************************************************************************************

>	BOOL NodeBlender::ProcessBlendedPath(DocCoord* pCoords,PathVerb* pVerbs,PathFlags* pFlags,UINT32 Len,BOOL Closed)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/11/94
	Inputs:		pCoords = ptr to coords of the blended path
				pVerbs  = ptr to verbs of blended path
				pFlags  = ptr to uninitialised flags array
				Len	    = num verbs in pVerbs
				Closed  = The resultant path should be closed
	Outputs:	pCoords array contains a valid set of coords
				pVerbs array contains a valid set of verbs
				pFlags array contains valid flags for each of the verbs in pVerbs
	Returns:	TRUE if OK, FALSE if it goes bang
	Purpose:	This does any processing necessary on the path generated by GBlend.
				The three arrays can be used to generate renderable paths, and also legal paths
				that can be used to create NodePath objects on the tree.
	SeeAlso:	-

********************************************************************************************/

BOOL NodeBlender::ProcessBlendedPath(DocCoord* pCoords,PathVerb* pVerbs,PathFlags* pFlags,UINT32 Len,BOOL Closed)
{
	if (Len < 1)
		return FALSE;

	// Generate a legal path flags array
	SetPathFlags(pVerbs,pFlags,Len);

	// If closed, make sure there's a PT_CLOSEFIGURE on the last end point
	if (Closed)
		pVerbs[Len-1] = pVerbs[Len-1] | PT_CLOSEFIGURE;		

	return TRUE;
}

/********************************************************************************************

>	void NodeBlender::SetPathFlags(PathVerb* pVerbs,PathFlags* pFlags,UINT32 Len)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/11/94
	Inputs:		pVerbs = ptr to path verbs
				pFlags = ptr to flags
				Len	   = num verbs in pVerbs
	Outputs:	pFlags array contains valid flags for each of the verbs in pVerbs
	Returns:	-
	Purpose:	This generates a legal path flags array based on the given verbs.
				All it does is set the end point flags correctly, leaving all other flags FALSE
	SeeAlso:	-

********************************************************************************************/

void NodeBlender::SetPathFlags(PathVerb* pVerbs,PathFlags* pFlags,UINT32 Len)
{
	UINT32 n;

	// Reset all the path flags to default values
	PathFlags DefaultPathFlags;
	for (n=0; n < Len; n++)
		pFlags[n] = DefaultPathFlags;

	// Scan the verbs, so that the end point flags can be set correctly
	for (n=0; n < Len; n++)
	{
		PathVerb Verb = pVerbs[n] & ~PT_CLOSEFIGURE;
		switch (Verb)
		{
			case PT_LINETO:
			case PT_MOVETO:
				pFlags[n].IsEndPoint = TRUE;
				break;

			case PT_BEZIERTO:
				n += 2;
				ERROR3IF(n>=Len,"Found a PT_BEZIERTO, but third pt is off the end of the array");
				pFlags[n].IsEndPoint = TRUE;
				break;

			default:
				ERROR3_PF(("Illegal path verb found : %c",Verb));
				break;
		}
	}
}

/********************************************************************************************

>	Matrix NodeBlender::MakeMatrix(BlendPath* pBlendPathStart,BlendPath* pBlendPathEnd,double BlendRatio)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com> (clever bit by Mike)
	Created:	20/10/94
	Inputs:		pBlendPathStart = ptr to blend path to blend from
				pBlendPathEnd   = ptr to blend path to blend to
				BlendRation  	= amount to blend by (0.0 <= BlendPath <= 1.0)
	Outputs:	-
	Returns:	A matrix
	Purpose:	Makes the matrix for the transfrom that should be applied to blend paths and attributes
	SeeAlso:	-

********************************************************************************************/

Matrix NodeBlender::MakeMatrix(BlendPath* pBlendPathStart,BlendPath* pBlendPathEnd,double BlendRatio)
{
	// Check entry params
	BOOL    ok = (pBlendPathStart != NULL && pBlendPathEnd != NULL);
	if (ok) ok = (pBlendPathStart->GetCreatedByNode() != NULL && pBlendPathEnd->GetCreatedByNode() != NULL);
	ERROR3IF(!ok,"One or more NULL entry params");
	if (!ok)
	{
		Matrix M;
		return M;
	}

	// Get the bounds of the objects that generated the paths
	DocRect BoundsStart = pBlendPathStart->GetCreatedByNode()->GetBoundingRect(TRUE);
	DocRect BoundsEnd   = pBlendPathEnd  ->GetCreatedByNode()->GetBoundingRect(TRUE);
	
	// The following creates a matrix that transforms the start rect to the end rect
	// dependent on the blend ratio, courtesy of Mike (thanks!)

	double Sx0 = BoundsStart.lo.x;
    double Sy0 = BoundsStart.lo.y;

	double Sx1 = BoundsStart.hi.x;
    double Sy1 = BoundsStart.hi.y;

	double Dx0 = BoundsEnd.lo.x;   
	double Dy0 = BoundsEnd.lo.y;

	double Dx1 = BoundsEnd.hi.x;   
	double Dy1 = BoundsEnd.hi.y;

	double Ex0 = Sx0 + BlendRatio*(Dx0-Sx0);
	double Ey0 = Sy0 + BlendRatio*(Dy0-Sy0);
	double Ex1 = Sx1 + BlendRatio*(Dx1-Sx1);
	double Ey1 = Sy1 + BlendRatio*(Dy1-Sy1);


	double t0 = (Ex1-Ex0)/(Sx1-Sx0);
	double t1 = 0;
	double t2 = 0;
	double t3 = (Ey1-Ey0)/(Sy1-Sy0);

	FIXED16 a(t0);
	FIXED16 b(t1);
	FIXED16 c(t2);
	FIXED16 d(t3);

	INT32 e = (INT32) (Ex0-Sx0*t0);
	INT32 f = (INT32) (Ey0-Sy0*t3);

	Matrix M(a,b,c,d,e,f);

	return (M);
}


/********************************************************************************************

>	double NodeBlender::MapObjectBlendRatio(double BlendRatio)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/6/99
	Inputs:		BlendRatio - the linear blend ration, between 0.0 and 1.0
	Returns:	The mapped blend ratio to use for object blending
	Purpose:	Function that works out what blend ratio to use when creating a blended bath.
				Allows non-linear processing of blends
	SeeAlso:	NodePath::Render; NodeRenderableInk::RenderEorDrag

********************************************************************************************/

double NodeBlender::MapObjectBlendRatio(double BlendRatio)
{
	NodeBlend* pNodeBlend = GetNodeBlend();
	if (pNodeBlend)
	{
		IProfile* pProfile = pNodeBlend->GetObjectProfile();
		if (pProfile)
			return pProfile->MapZeroToOne(BlendRatio);
	}

	return BlendRatio;
}

/********************************************************************************************

>	double NodeBlender::MapAttrBlendRatio(double BlendRatio)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/6/99
	Inputs:		BlendRatio - the linear blend ration, between 0.0 and 1.0
	Returns:	The mapped blend ratio to use for attribute blending
	Purpose:	Function that works out what blend ratio to use when blending attributes
				Allows non-linear processing of blend attributes
	SeeAlso:	NodePath::Render; NodeRenderableInk::RenderEorDrag

********************************************************************************************/

double NodeBlender::MapAttrBlendRatio(double BlendRatio)
{
	NodeBlend* pNodeBlend = GetNodeBlend();
	if (pNodeBlend)
	{
		IProfile* pProfile = pNodeBlend->GetAttrProfile();
		if (pProfile)
			return pProfile->MapZeroToOne(BlendRatio);
	}

	return BlendRatio;
}

/********************************************************************************************

>	double NodeBlender::MapCMapPtrToPtr::iteratorBlendRatio(double BlendRatio)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/6/99
	Inputs:		BlendRatio - the linear blend ration, between 0.0 and 1.0
	Returns:	The mapped blend ratio to use for CMapPtrToPtr::iteratoring the blended shapes
	Purpose:	Function that works out what blend ratio to use when CMapPtrToPtr::iteratoring the blended shapes
				Allows non-linear processing of blend shape CMapPtrToPtr::iteratoring
	SeeAlso:	NodePath::Render; NodeRenderableInk::RenderEorDrag

********************************************************************************************/

double NodeBlender::MapPositionBlendRatio(double BlendRatio)
{
	NodeBlend* pNodeBlend = GetNodeBlend();
	if (pNodeBlend)
	{
		IProfile* pProfile = pNodeBlend->GetPositionProfile();
		if (pProfile)
			return pProfile->MapZeroToOne(BlendRatio);
	}

	return BlendRatio;
}


/********************************************************************************************

>	double NodeBlender::GetObjectRatio()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/10/99
	Inputs:		-
	Returns:	The mapped blend ratio to use for blend objects
	Purpose:	returns the current mapping of objects


********************************************************************************************/

double NodeBlender::GetObjectRatio()
{
	UINT32 NumSteps = GetNumBlendSteps() + 1;
	double StepRatio = 1.0/NumSteps;
	double LinearRatio = StepRatio * m_BlendStep;

	CProfileBiasGain* ObjectProfile = GetNodeBlend()->GetObjectProfile();

	return ObjectProfile->MapZeroToOne(LinearRatio);

}



/********************************************************************************************

>	double NodeBlender::GetInvertedAttributeRatio()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/10/99
	Inputs:		-
	Returns:	The mapped blend ratio to use for blend attributes
	Purpose:	Function that inverts the current attribute profile and returns the mapped value
				This is needed to fix the problem with profiles and transparencies and other fills
				that use control points.
	SeeAlso:	NodePath::Render; NodeRenderableInk::RenderEorDrag

********************************************************************************************/

double NodeBlender::GetInvertedAttributeRatio()
{
	UINT32 NumSteps = GetNumBlendSteps() + 1;
	double StepRatio = 1.0/NumSteps;
	double LinearRatio = StepRatio * m_BlendStep;

	CProfileBiasGain* AttributeProfile = GetNodeBlend()->GetAttrProfile();

	CProfileBiasGain InvertedProfile;
	InvertedProfile.SetBias(-1*(AttributeProfile->GetBias()));
	InvertedProfile.SetGain(-1*(AttributeProfile->GetGain()));
	
    /*double Test =*/  AttributeProfile->MapZeroToOne(LinearRatio);

	return InvertedProfile.MapZeroToOne(LinearRatio);

}


/********************************************************************************************

>	void NodeBlender::RenderEorDrag( RenderRegion* pRender )

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/10/94
	Inputs:		pRender - A Pointer to the current RenderRegion
	Purpose:	Renders a version of the blender node for EORed dragging of blends.
	SeeAlso:	NodePath::Render; NodeRenderableInk::RenderEorDrag

********************************************************************************************/

void NodeBlender::RenderEorDrag( RenderRegion* pRender )
{
	if (!Reinit()) return;
}

/********************************************************************************************

> virtual void NodeBlender::RenderBlendBlobs(RenderRegion* pRender,BOOL RenderStart,BOOL RenderEnd)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/11/94
	Inputs:		pRender = region to render blobs into
				RenderStart = TRUE if blobs of start of blend should be rendered
				RenderEnd   = TRUE if blobs of end   of blend should be rendered
	Outputs:	-
	Returns:	-
	Purpose:	Renders the blobs of the blend paths associated with the given blender
				NOTE: 	This only renders blobs if there's only one blend path in both the start
						and the end of the blend
	SeeAlso:	-

********************************************************************************************/

void NodeBlender::RenderBlendBlobs(RenderRegion* pRender,BOOL RenderStart,BOOL RenderEnd)
{
#if !defined(EXCLUDE_FROM_RALPH)
	if (!Reinit()) return;
	if (m_pRefStart == NULL || m_pRefEnd == NULL) return;

	if (m_pRefStart->GetNumBlendPaths() == 1 && m_pRefEnd->GetNumBlendPaths() == 1)
	{
		Trans2DMatrix*	pRotateStart= NULL;
		Trans2DMatrix*	pRotateEnd	= NULL;

		if (GetNodeBlendPath() != NULL)
		{
			pRotateStart = GetRotateMatrix(m_pRefStart->GetNode(),m_AngleStart);
			pRotateEnd   = GetRotateMatrix(m_pRefEnd  ->GetNode(),m_AngleEnd  );
		}

		if (RenderStart)	m_pRefStart->RenderBlendBlobs(pRender,pRotateStart);
		if (RenderEnd)		m_pRefEnd  ->RenderBlendBlobs(pRender,pRotateEnd);

		BlendPath* pBlendPathStart = m_pRefStart->GetFirstBlendPath();
		BlendPath* pBlendPathEnd   = m_pRefEnd  ->GetFirstBlendPath();

		if (pBlendPathStart != NULL && pBlendPathEnd != NULL)
		{
			DocCoord Start = pBlendPathStart->GetPathCoord(0);
			DocCoord End   = pBlendPathEnd  ->GetPathCoord(0);

			if (pRotateStart)	pRotateStart->Transform(&Start,1);
			if (pRotateEnd)  	pRotateEnd  ->Transform(&End  ,1);

			// Get the scaled pixel size for the view
			FIXED16 ScaledPixelWidth,
					ScaledPixelHeight; // Not used
			pRender->GetRenderView()->GetScaledPixelSize(&ScaledPixelWidth, &ScaledPixelHeight);

			pRender->SetLineWidth((ScaledPixelWidth.MakeLong())*2);
			pRender->SetLineColour(COLOUR_BLACK);
			pRender->DrawLine(Start,End);
		}

		DELPTR(pRotateStart);
		DELPTR(pRotateEnd);
	}

#endif
}




/********************************************************************************************

>	DocRect NodeBlender::GetBoundingRect(BOOL DontUseAttrs=FALSE, BOOL HitTest=FALSE)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/10/94
	Inputs:		DontUseAttrs - TRUE if we should ignore the nodes attributes.
				Defaults to FALSE
				HitTest      - TRUE if being called during HitTest
	Returns:	The nodes bounding rect
	Purpose:	if the bounding rect is valid it is returned, if not, it is recalculated
				and then returned.
	SeeAlso:	NodeBlender::GetBlobBoundingRect

********************************************************************************************/

DocRect NodeBlender::GetBoundingRect(BOOL DontUseAttrs, BOOL HitTest)
{
	if (Parent != NULL && IS_A(Parent,NodeBlend) && !((NodeBlend*)Parent)->AreBoundsValid())
		IsBoundingRectValid = FALSE;

	if (!IsBoundingRectValid || DontUseAttrs)
	{
		// We need ptrs to the start and end objects this blender is blending
		NodeRenderableInk* pStart = NULL;
		NodeRenderableInk* pEnd   = NULL;

		// If we are initialised, get the start and end ink nodes
		if (m_pRefStart != NULL)	pStart = m_pRefStart->GetNode();
		if (m_pRefEnd   != NULL)	pEnd   = m_pRefEnd  ->GetNode();
		
		if (pStart == NULL) pStart = m_pNodeStart;
		if (pEnd == NULL)   pEnd   = m_pNodeEnd;

		// If we don't have ptrs to the start and end objects of this blend, have a look for them
		if (pStart == NULL) pStart = (NodeRenderableInk*)FindPrevious(CC_RUNTIME_CLASS(NodeRenderableInk));
		if (pEnd   == NULL) pEnd   = (NodeRenderableInk*)FindNext(CC_RUNTIME_CLASS(NodeRenderableInk));

		// Check the ink nodes to ensure that they are what we expect.
		ERROR3IF(pStart == NULL,"pStart == NULL");
		ERROR3IF(pEnd   == NULL,"pEnd == NULL");
		ERROR3IF(!pStart->IS_KIND_OF(NodeRenderableInk),"pStart not a NodeRenderableInk");
		ERROR3IF(!pEnd  ->IS_KIND_OF(NodeRenderableInk),"pEnd not a NodeRenderableInk");
		ERROR3IF(pStart->IS_KIND_OF(NodeBlender),"pStart is a NodeBlender");
		ERROR3IF(pEnd  ->IS_KIND_OF(NodeBlender),"pEnd not a NodeBlender");

		// Continue only if we have nodes that we expect
		BOOL ok = 	(pStart != NULL) &&
					(pEnd   != NULL) &&
					(pStart->IS_KIND_OF(NodeRenderableInk)) &&
					(pEnd  ->IS_KIND_OF(NodeRenderableInk)) &&
					(!pStart->IS_KIND_OF(NodeBlender)) && 
					(!pEnd  ->IS_KIND_OF(NodeBlender));

		if (ok)
		{
			// The bounds for the entire blender is the union of the bounds of the start and end objects blended.
			// Assume here that blended paths can't lie outside this bounding rect.

			DocRect BoundsStart = pStart->GetBoundingRect(DontUseAttrs,HitTest);
			DocRect BoundsEnd   = pEnd  ->GetBoundingRect(DontUseAttrs,HitTest);

			DocRect NewBounds;
			NewBounds = BoundsStart;
			NewBounds = NewBounds.Union(BoundsEnd);

			if (GetNodeBlendPath() != NULL)
				NewBounds = NewBounds.Union(GetUnionBlendedBounds(BoundsStart,BoundsEnd));

			// If the caller didn't want bounds included, then return the bounds immediately,
			// and do not cache the bounds rectangle.
			if (DontUseAttrs)
				return(NewBounds);

			// Cache the new bounding rectangle
			IsBoundingRectValid = TRUE;
			BoundingRectangle = NewBounds;
		}
	}


/*	if (!IsBoundingRectValid && Reinit())
	{
		if (pRefStart != NULL &&  pRefStart->GetNode() != NULL &&
			pRefEnd   != NULL &&  pRefEnd  ->GetNode() != NULL)
		{
			BoundingRectangle = pRefStart->GetNode()->GetBoundingRect(DontUseAttrs);
			BoundingRectangle = BoundingRectangle.Union(pRefEnd->GetNode()->GetBoundingRect(DontUseAttrs));

			IsBoundingRectValid = TRUE;
		}
	}
*/

	// return the current state of the bounding rect
	return BoundingRectangle;
}


/********************************************************************************************

>	DocRect NodeBlender::GetBlobBoundingRect()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/10/94
	Returns:	DocRect - Returns the bounding rect of the blender node and its blobs
	Purpose:	At the moment, blender nodes don't have any blobs, so just the standard
				bounding box is returned

********************************************************************************************/

DocRect NodeBlender::GetBlobBoundingRect()
{
	return (GetBoundingRect());
}




/********************************************************************************************

>	virtual UINT32 NodeBlender::GetNodeSize() const

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/10/94
	Returns:	The size of the node in bytes 
	Purpose:	For finding the size of the node 
	SeeAlso:	Node::GetSubtreeSize

********************************************************************************************/

UINT32 NodeBlender::GetNodeSize() const 
{     
	return (sizeof(NodeBlender)); 
}  



/********************************************************************************************

>	BOOL NodeBlender::OnClick( DocCoord PointerPos, ClickType Click, ClickModifiers ClickMods,
						Spread* pSpread )

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/10/94
	Inputs:		PointerPos - The Location of the mouse pointer at the time of the click
				Click - The type of click received (single, double, drag etc)
				ClickMods - The modifiers to the click (eg shift, control etc )
	Returns:	BOOL - TRUE if the node claims the click as its own and FALSE if it is
				not interested in the click
	Purpose:	Allows the Node to respond to clicks by selecting its blobs or starting
				drags etc.
				This functions should be overridden in the all the NodeRenderableInk classes
				so that this version never gets called. Eg the NodePath class might claim
				the click if it happened over one of its unselected blobs.

********************************************************************************************/

BOOL NodeBlender::OnClick( DocCoord PointerPos, ClickType Click, ClickModifiers ClickMods,
						Spread* pSpread )
{
	if (!Reinit()) return FALSE;

	// we did not use the click, so let someone else try
	return FALSE;
}


/********************************************************************************************

>	virtual BOOL NodeBlender::CanBecomeA(BecomeA* pBecomeA) 

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/10/94
	Inputs:		InkClass: The class	of object
				pNumObjects = ptr to place number of objects of type pClass that will be created (Note: can be NULL).
							  *pNumObects in undefined on entry
	Returns:	TRUE if the node, or any of its children can transmogrify themselves to become 
				an InkClass object
	Purpose:	This function is used by the convert to shapes operation. It determines if 
				the node or any of its children can convert themselves into an InkClass object. 

				The number you put into pNumObjects (if it's not NULL) should exactly equal the total number
				of pClass objects you create.  It should NOT contain any additional objects you may produce
				such as group objects for containing the pClass object, or attributes.

				Also, the entry value of *pNumObjects cannot be assumed to be 0.

********************************************************************************************/

BOOL NodeBlender::CanBecomeA(BecomeA* pBecomeA)
{
	// The NodeBlender can become a NodePath
	if (pBecomeA->BAPath())
	{
		if (pBecomeA->IsCounting())
		{
			if (!Reinit()) return FALSE;

			// Check that we have a valid blender
			if (m_pRefStart == NULL || m_pRefEnd == NULL)
				return FALSE;

			UINT32 NumBlendSteps = GetNumBlendSteps();

			// Find num blend paths in start and end, and keep hold of the MAX value
			// The max value is the number of individual paths created for each blend step
			UINT32 NumPathsInStart = m_pRefStart->GetNumBlendPaths();
			UINT32 NumPathsInEnd   = m_pRefEnd  ->GetNumBlendPaths();
			UINT32 MaxNumPaths	 = max(NumPathsInStart,NumPathsInEnd);

			// The number of objects 
//			*pNumObjects = NumBlendSteps * MaxNumPaths;
			pBecomeA->AddCount(NumBlendSteps * MaxNumPaths);
		}

		return TRUE;
	}

	return FALSE;
}

		   
/********************************************************************************************

>	virtual BOOL NodeBlender::DoBecomeA(CCRuntimeClass* InkClass, UndoableOperation* pOp) 

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/10/94
	Inputs:		pBecomeA = 	ptr to a class that contains all the info needed to become a new
							type of node.
	Outputs:	-
	Returns:	TRUE if the object has been transformed, FALSE if we run out of memory

	Purpose:	Transforms the object into another type of object. 
	SeeAlso:	NodeBlender::CanBecomeA

********************************************************************************************/

BOOL NodeBlender::DoBecomeA(BecomeA* pBecomeA)
{
	if (!Reinit()) return TRUE;

	if (!m_Initialised) return TRUE;		// we MUST handle this for future versions !

	// Check for a NULL entry param
	ERROR2IF(pBecomeA == NULL,FALSE,"pBecomeA is NULL");

	// This lump checks that the Reason is one that we understand
	// It also makes sure that we don't have a NULL UndoOp ptr
	BOOL ValidReason = (pBecomeA->GetReason() == BECOMEA_REPLACE || pBecomeA->GetReason() == BECOMEA_PASSBACK);
	ERROR2IF_PF(!ValidReason,FALSE,("Unkown BecomeA reason %d",pBecomeA->GetReason()));

	// Set up a HandleBecomeA object and call CreateBlends().
	// This mechanism means the the same code is used to both render blends and create paths
	// from blends.
	HandleBecomeA HBecomeA(this,this,pBecomeA);
	CreateBlends(NULL,&HBecomeA);

 	switch (pBecomeA->GetReason())
	{
 		case BECOMEA_REPLACE :
		{
			// It's a BECOMEA_REPLACE, so hide this node in an undoable way

			// Can't do it in an undoable way without an Undo Op
//			ERROR2IF(pBecomeA->GetUndoOp() == NULL,FALSE,"GetUndoOp() returned NULL");

			UndoableOperation* pOp = pBecomeA->GetUndoOp();

			// We need to make sure that all the groups we may have produced have their attrs correctly
			// factored out.  This is so the final factoring out done by the NodeBlend node will be able
			// to complete the job.
			// Without this step, leaf nodes can have common child attrs that should be factored up the
			// to a higher level.
			Node* pNode = m_pNodeStart->FindNext();

			while (pNode != NULL && pNode != m_pNodeEnd)
			{
				//if (IS_A (pNode, NodeGroup))
				if (pNode->IsCompoundClass())
				{
					if (pOp)
					{
						if (!pOp->DoFactorOutCommonChildAttributes((NodeCompound*)pNode))
							return FALSE;
					}
					else
					{
						if (!((NodeCompound*)pNode)->FactorOutCommonChildAttributes())
							return(FALSE);
					}
				}

				pNode = pNode->FindNext();
			}

			NodeHidden* pHiddenNodeBlender = NULL;

			// hide this blender node
			if (pOp)
			{
				if (!pOp->DoHideNode(this, TRUE, &pHiddenNodeBlender))
					return FALSE;
			}
			else
			{
				CascadeDelete();
				delete this;
			}
		}
		break;

		case BECOMEA_PASSBACK :
			// Don't do anything for this reason
		break;
		default: break;
	}

	return TRUE;
}

/***********************************************************************************************

> BOOL NodeBlender::Snap(DocCoord* pDocCoord)

    Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
    Created:	11/10/94
	Inputs:     pDocCoord	= a coord in Spread coords
    Outputs:    
    Returns:    TRUE 	- the DocCoord has been snapped .
    			FALSE	- the DocCoord has not been processed.
    			                                  		 
    Purpose:    Snaps to given coord to the nearest point on the blender node.
				Just returns FALSE currently
    Errors:        
	Scope:      public
	       
**********************************************************************************************/

BOOL NodeBlender::Snap(DocCoord* pDocCoord)
{
	return FALSE;
}

/***********************************************************************************************

> BOOL NodeBlender::Snap(DocRect* pDocRect,const DocCoord& PrevCoord,const DocCoord& CurCoord)

    Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
    Created:	11/10/94
	Inputs:     pDocCoord	- the rectangle to snap
				StartDrag	- Start coord of drag
				EndDrag		- End coord of drag
    Outputs:    
    Returns:    TRUE 	- the DocRect been snapped.
    			FALSE	- the DocRect has not been processed.
    			                                  		 
    Purpose:    Snaps the given rect to the nearest CMapPtrToPtr::iterator on the grid, preserving its width
    			and height.
    			The coords of the rect used for the snapping are determined by the PrevCoord and
				CurCoord coords supplied.  This is done to allow the user to control how a
				selection rectangle is snapped to the grid by the direction of his/her last mouse 
				movement.
				To force the bottom left hand corner of the rect to be snapped, 
				supply PrevCoord=(0,0) and CurCoord(-1,-1).

				ALWAYS RETURNS FALSE currently.

	Scope:      public
	       
**********************************************************************************************/

BOOL NodeBlender::Snap(DocRect* pDocRect,const DocCoord& PrevCoord,const DocCoord& CurCoord)
{
	return FALSE;
}

/********************************************************************************************

> BOOL NodeBlender::IsPointOverBlob(DocCoord* pPointerPos,BlendPath** ppBlendPath,INT32* pIndex,BOOL* pAStart);

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/11/94
	Inputs:		pPointerPos = point to check against
				ppBlendPath= ptr to place to store blend path ptr if a match is found
				pIndex	   = ptr to place to store index if match is found
				pAStart	   = ptr to place to store start or end flag
	Outputs:	*ppBlendPath & *pIndex & *pAStart updated if TRUE returned, undefined otherwise
	Returns:	TRUE if match  found, FALSE otherwise
	Purpose:	This sees if the point given lies on a selected blob.
				If a match is found, the ptr to the blend path and index to blob element is returned.
				Also, if found,*pAStart = TRUE if it belongs to the start path, FALSE for the end path

				Markn 26-5-99:	Modified so that if the function returns TRUE, *pPointerPos is updated
								so that it contains the coords of the blob it is over
	SeeAlso:	-

********************************************************************************************/

BOOL NodeBlender::IsPointOverBlob(DocCoord* pPointerPos,BlendPath** ppBlendPath,INT32* pIndex,BOOL* pAStart)
{
#if !defined(EXCLUDE_FROM_RALPH)
	if (m_pRefStart == NULL || m_pRefEnd == NULL || pPointerPos == NULL) return FALSE;

	BOOL Found = FALSE;

	if (m_pRefStart->GetNumBlendPaths() == 1 && m_pRefEnd->GetNumBlendPaths() == 1)
	{
		Trans2DMatrix*	pRotateStart= NULL;
		Trans2DMatrix*	pRotateEnd	= NULL;

		if (GetNodeBlendPath() != NULL)
		{
			pRotateStart = GetRotateMatrix(m_pRefStart->GetNode(),m_AngleStart);
			pRotateEnd   = GetRotateMatrix(m_pRefEnd  ->GetNode(),m_AngleEnd  );
		}

		if (m_pRefStart->IsPointOverBlob(pPointerPos,ppBlendPath,pIndex,pRotateStart))
		{
			Found = TRUE;
			*pAStart = TRUE;
		}

		if (m_pRefEnd->IsPointOverBlob(pPointerPos,ppBlendPath,pIndex,pRotateEnd))
		{
			Found = TRUE;
			*pAStart = FALSE;
		}

		DELPTR(pRotateStart);
		DELPTR(pRotateEnd);
	}

	return Found;
#else
	return FALSE;
#endif
}

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------

/***********************************************************************************************

> BOOL NodeBlender::Initialise(	NodeRenderableInk* 	pNodeStart,
								NodeRenderableInk* 	pNodeEnd,
								INT32 				ThisPathIndexStart,
								INT32 				ThisPathIndexEnd,
								UndoableOperation* 	pUndoOp,
								Progress* 			pProgress,
								BOOL				IgnoreEscape)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/10/94
	Inputs:		pNodeStart 			= ptr to the start of the blend (can be NULL - see later)
				pNodeEnd   			= ptr to the end   of the blend (can be NULL - see later)
				ThisPathIndexStart  = the index into the start path of the blob the user blended from (-1 means none)
				ThisPathIndexEnd    = the index into the end   path of the blob the user blended to	  (-1 means none)
				pUndoOp				= ptr to an undoable op so that initialisation can be done in an undoable way
							  		(can be NULL)
				pProgress			= ptr to progress object for maintaining hour glass
				IgnoreEscape		= TRUE if you don't want this process abortable via the Escape key
	Outputs:	-
	Returns:	TRUE if successfully initialises, FALSE otherwise
	Purpose:	This initialises the blender node so that it is in a state in which it can
				blend the two nodes together.

				This should be called AFTER this NodeBlender and its start and end nodes have been
				placed under the corresponding NodeBlend node. This is so that it can get a reference
				to the nodes relative to the parent NodeBlend node.

				If pNodeStart == NULL, then the previous node is chosen.  If there isn't a previous node
				or the node is not a NodeRenderableInk, FALSE will be returned.

				If pNodeEnd   == NULL, then the next node is chosen.  If there isn't a next node
				or the node is not a NodeRenderableInk, FALSE will be returned.

***********************************************************************************************/

BOOL NodeBlender::Initialise(	NodeRenderableInk* 	pThisNodeStart,
								NodeRenderableInk* 	pThisNodeEnd,
								INT32 				ThisPathIndexStart,
								INT32 				ThisPathIndexEnd,
								UndoableOperation* 	pUndoOp,
								Progress* 			pProgress,
								BOOL 				IgnoreEscape)
{
	// Copy the ink node ptrs into member vars
	m_pNodeStart = pThisNodeStart;
	m_pNodeEnd   = pThisNodeEnd;

	// Copy the path index params into member vars
	m_PathIndexStart = ThisPathIndexStart;
	m_PathIndexEnd   = ThisPathIndexEnd;

	// If the ink nodes are NULL, search for appropriate ink nodes
	if (m_pNodeStart == NULL) m_pNodeStart = (NodeRenderableInk*)FindPrevious();
	if (m_pNodeEnd   == NULL) m_pNodeEnd   = (NodeRenderableInk*)FindNext();

	// Check the ink nodes
	ERROR2IF(m_pNodeStart == NULL,FALSE,"m_pNodeStart == NULL");
	ERROR2IF(m_pNodeEnd   == NULL,FALSE,"m_pNodeEnd == NULL");
	ERROR2IF(!m_pNodeStart->IS_KIND_OF(NodeRenderableInk),FALSE,"m_pNodeStart not a NodeRenderableInk");
	ERROR2IF(!m_pNodeEnd  ->IS_KIND_OF(NodeRenderableInk),FALSE,"m_pNodeEnd not a NodeRenderableInk");
	ERROR2IF(m_pNodeStart->IS_KIND_OF(NodeBlender),FALSE,"m_pNodeStart is a NodeBlender");
	ERROR2IF(m_pNodeEnd  ->IS_KIND_OF(NodeBlender),FALSE,"m_pNodeEnd not a NodeBlender");

	// Delete old BlendRefs we may have (we might be reinitialising)
	DELPTR(m_pRefStart);
	DELPTR(m_pRefEnd);

	// Reset the member vars before we try and initialise the blender
	m_pRefStart		= NULL;
	m_pRefEnd		= NULL;
	m_ObjIndexStart	= -1;
	m_ObjIndexEnd	= -1;
	m_Initialised	= FALSE;

	Trans2DMatrix*	pRotateStart= NULL;
	Trans2DMatrix*	pRotateEnd	= NULL;

	if (GetNodeBlendPath() != NULL)
	{
		pRotateStart = GetRotateMatrix(m_pNodeStart,360.0 - m_AngleStart);
		pRotateEnd   = GetRotateMatrix(m_pNodeEnd  ,360.0 - m_AngleEnd  );
	}

	BOOL ok = TRUE;

	// CGS:  we can have funky blends on paths.  Everything works fine if they both have a path.
	// However, if they do NOT both have paths - then Mr Blender (incorrectly) blends the NodeBlendPath
	// into the paths generated by the other blend!

	// we SHOULD prevent this from happening ....

	if (ok) ok = InitBlendRef(m_pNodeStart,&m_pRefStart,m_PathIndexStart,pUndoOp,pProgress,IgnoreEscape,pRotateStart);
	if (ok) ok = InitBlendRef(m_pNodeEnd  ,&m_pRefEnd  ,m_PathIndexEnd  ,pUndoOp,pProgress,IgnoreEscape,pRotateEnd);

	if (ok) { m_pRefStart->StripRedundantNodeBlendPaths (m_pRefEnd); }

	if (ok) ok = CalcObjIndex(m_pNodeStart,&m_ObjIndexStart);
	if (ok) ok = CalcObjIndex(m_pNodeEnd  ,&m_ObjIndexEnd);

	// Ensure these indexes are kept up to date (needed for cut/copy/paste, export, etc).
	// Note that the following code dumps the indexes computed by the calls to InitBlendRef
	// above and recomputes them before assigning them to m_PathIndexStart and m_PathIndexEnd
	// (A lot of DoBecomeA work to compute two numbers...)
	if (((m_pRefStart->GetNumBlendPaths () > 1) && (m_pRefEnd->GetNumBlendPaths () == 1))
		||
		((m_pRefStart->GetNumBlendPaths () == 1) && (m_pRefEnd->GetNumBlendPaths () > 1)))
	{
		m_PathIndexStart = -1;
		m_PathIndexEnd = -1;
		DELPTR(m_pRefStart);
		DELPTR(m_pRefEnd);

		if (ok) ok = InitBlendRef(m_pNodeStart,&m_pRefStart,m_PathIndexStart,pUndoOp,pProgress,IgnoreEscape,pRotateStart);
		if (ok) ok = InitBlendRef(m_pNodeEnd  ,&m_pRefEnd  ,m_PathIndexEnd  ,pUndoOp,pProgress,IgnoreEscape,pRotateEnd);

		if (ok) { m_pRefStart->StripRedundantNodeBlendPaths (m_pRefEnd); }
	}

	if (ok) m_PathIndexStart = m_pRefStart->GetOrigMapping();
	if (ok) m_PathIndexEnd   = m_pRefEnd  ->GetOrigMapping();

	// Make sure the Initialised flag is TRUE only if we have done everything successfully.
	if (ok) CheckFullyInitialised();
	if (ok) ok = m_Initialised;

	DELPTR(pRotateStart);
	DELPTR(pRotateEnd);

	// Don't need to delete the BlendRefs if things go wrong.
	// They'll get deleted next time it's initialised, or by the deconstructor

	return (ok);
}



/***********************************************************************************************

> BOOL NodeBlender::InitBlendRef(NodeRenderableInk* pNode,BlendRef** ppRef,INT32 Index,UndoableOperation* pUndoOp,Progress* pProgress,BOOL IgnoreEscape,Trans2DMatrix* pMatrix)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/10/94
	Inputs:		pNode 		= ptr to a node in the tree
				ppRef		= ptr to place to store a new BlendRef structure
				Index   	= index into path of blob clicked on (-1 means ignore)
				pUndoOp		= ptr to an undoable op so that initialisation can be done in an undoable way
							  (can be NULL)
				pProgress	= ptr to progress object for maintaining hour glass
				IgnoreEscape= TRUE if you don't want this process abortable via the Escape key
				pMatrix		= transformation to apply to each blend path (can be NULL)
	Outputs:	*ppRef = ptr to an initialised blend reference
	Returns:	TRUE if successfully initialises, FALSE otherwise
	Purpose:	This creates and initialises a blend reference.

***********************************************************************************************/

BOOL NodeBlender::InitBlendRef(NodeRenderableInk* pNode,BlendRef** ppRef,INT32 Index,UndoableOperation* pUndoOp,Progress* pProgress,BOOL IgnoreEscape,Trans2DMatrix* pMatrix)
{
	ERROR2IF(ppRef == NULL,FALSE,"ppRef == NULL");

	ALLOC_WITH_FAIL(*ppRef,new BlendRef,pUndoOp);
	BOOL ok = (*ppRef != NULL);

	if (ok) ok = (*ppRef)->Initialise(pNode,Index,pUndoOp,pProgress,IgnoreEscape,pMatrix);

	return (ok);
}

/********************************************************************************************

>	BOOL NodeBlender::CalcObjIndex(NodeRenderableInk* pInkNode,INT32* pObjIndex)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/10/94
	Inputs:		pInkNode   = ptr to a node that this blender will blend
				pObjIndex = ptr to place to store the relative reference to this node
	Outputs:	*pObjIndex will contain the reference if TRUE is returned, -1 if FALSE is returned
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	Firstly, this will check that this node and pInkNode have the same NodeBlend parent node.
				If all's well, it will scan the children of the NodeBlend, starting with the first child,
				counting until it comes to pInkNode.
				If pInkNode is the first child, *pObjIndex == 0. If the second child, *pObjIndex == 1, etc
	SeeAlso:	-

********************************************************************************************/

BOOL NodeBlender::CalcObjIndex(NodeRenderableInk* pInkNode,INT32* pObjIndex)
{
	ERROR2IF(pInkNode == NULL,FALSE,"pInkNode is NULL");
	ERROR2IF(pObjIndex == NULL,FALSE,"pObjIndex is NULL");

	Node* pParent = pInkNode->FindParent();

	/*if (pInkNode->IsNodeHidden ())
	{
		ERROR3 ("Node is hidden!");
	}*/

	ERROR2IF(pParent == NULL,FALSE,"pInkNode has no parent");

	if (!pParent->IsController ())		// CGS:  we can now have compound nodes in here!
	{
		ERROR2IF(!IS_A(pParent,NodeBlend),FALSE,"pInkNode parent is not a NodeBlend");
		ERROR2IF(pParent != this->FindParent(),FALSE,"pInkNode has different parent to this node");
	}
	else
	{
		// compound might be shadowed - if it is, find the shadow, and check that its parent is the NodeBlend ....
		if (IS_A (pParent->FindParent (), NodeShadowController))
		{
			pParent = pParent->FindParent ()->FindParent ();
		}
		
		ERROR2IF(pParent->FindParent () != this->FindParent(),FALSE,"pInkNode has different parent to this node");
	}

	INT32 Index = 0;
	Node* pNode = pParent->FindFirstChild();

	while (pNode != NULL && pNode != pInkNode)
	{
		if (pNode->IS_KIND_OF(NodeRenderableInk))
			Index++;
		if (pNode->IS_KIND_OF(NodeBlendPath))
			Index--;
		pNode = pNode->FindNext();
	}

	if (pNode == pInkNode)
		*pObjIndex = Index;
	else
		*pObjIndex = -1;

	return (pNode == pInkNode);
}

/********************************************************************************************

>	NodeRenderableInk* NodeBlender::FindObjIndexedNode(INT32 ObjIndex)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/10/94  Adjusted Diccon Yamanaka 16/9/99
	Inputs:		ObjIndex = Relative ref to the object under the parent of this node
	Outputs:	-
	Returns:	Ptr to the node. NULL if couldn't be found or node is of wrong type
	Purpose:	This scans the children of the parent of this blender, until it finds
				the ObjIndex'th NodeRenderableInk. I.e. if ObjIndex == 0, it returns the first child NodeRenderableInk.
				if ObjIndex == 1, returns the second child NodeRenderableInk.
				Fails if:
					ObjIndex >= number of children the parent has
					The node found is not a NodeRenderableInk or is a NodeBlender
	SeeAlso:	-

********************************************************************************************/

NodeRenderableInk* NodeBlender::FindObjIndexedNode(INT32 ObjIndex)
{
	ERROR3IF_PF(ObjIndex < 0,("ObjIndex is -ve!! (%ld)",ObjIndex));
	if (ObjIndex < 0) return NULL;

	Node* pParent = this->FindParent();
	Node* pFoundNode = NULL;

	ERROR3IF(pParent == NULL,"pInkNode has no parent");
	ERROR3IF(!IS_A(pParent,NodeBlend),"parent is not a NodeBlend");

	if (pParent != NULL && IS_A(pParent,NodeBlend))
	{
		Node* pNode = pParent->FindFirstChild();
		while (pNode != NULL && ObjIndex >= 0)
		{
			if (pNode->IS_KIND_OF(NodeRenderableInk))
			{
				//DY 16/9/99 fix to make blending along a curve possible
				// as we insert NodeBlendPath during that operation.
				if (!pNode->IS_KIND_OF(NodeBlendPath))
				{
					if (ObjIndex == 0)	
						pFoundNode = pNode;
					ObjIndex--;
				}
			}
			pNode = pNode->FindNext();
		}
	}

	if (pFoundNode != NULL)
	{
		if (IS_A(pFoundNode,NodeBlender))
		{
			ERROR3("Found node is a NodeBlender");
			pFoundNode = NULL;
		}
		else if (!pFoundNode->IS_KIND_OF(NodeRenderableInk))
		{
			ERROR3("Found node is not a NodeRenderableInk");
			pFoundNode = NULL;
		}
	}
	else
		ERROR3("Couldn't find ref node");

	return ((NodeRenderableInk*)pFoundNode);
}


/********************************************************************************************
>	virtual BOOL NodeBlender::CanSelectAsSimple()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	06/01/95
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the node will allow itself to be selected when it has been "hit"
				FALSE otherwise
	Purpose:	Ask a node whether or not it's prepared to become selected when it's
				clicked on.
				This function is called in the FindSimple routines when they have just detected
				a "hit" on a node.
				This virtual base function is overridden here to indicate that the NodeBlender
				object is definitely NOT prepared to become selected!
	Errors:		-
	SeeAlso:	NodeRenderableInk::FindSimpleAtPoint; NodeRenderableInk::FindCompoundAtPoint
********************************************************************************************/

BOOL NodeBlender::CanSelectAsSimple()
{
	return	FALSE;
}




/********************************************************************************************

>	void NodeBlender::CheckFullyInitialised()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/10/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	This will set the object's Initialise member to TRUE if it is fully initialised,
				i.e. pRefStart & pRefEnd != NULL, and ObjIndexStart and ObjIndexEnd and >= 0
	SeeAlso:	-

********************************************************************************************/

void NodeBlender::CheckFullyInitialised()
{
	m_Initialised = ((m_pRefStart != NULL) &&
				     (m_pRefEnd   != NULL) &&
				     (m_ObjIndexStart >= 0)  &&
				     (m_ObjIndexEnd   >= 0));
}


/********************************************************************************************

>	BOOL NodeBlender::Reinit(NodeRenderableInk* pInkNodeStart = NULL,NodeRenderableInk* pInkNodeEnd = NULL,BOOL ProgressBar = TRUE)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/10/94  adjusted Diccon Yamanaka 16/9/99
	Inputs:		pInkNodeStart = ptr to the node to blend from (can be NULL)
				pInkNodeEnd   = ptr to the node to blend to   (can be NULL)
				ProgressBar	  = if TRUE a progress bar will be displayed.
	Outputs:	-
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	This will try and reinitialise the blender by using either the relative reference
				values ObjIndexStart and ObjIndexEnd (if input params are NULL) or the nodes provided.
				
				If using the indexes, it finds the actual pointers to the nodes that they reference, and calls Initialise()

				If already initialised, it returns TRUE immediately
	SeeAlso:	-

********************************************************************************************/

BOOL NodeBlender::Reinit(NodeRenderableInk* pInkNodeStart,NodeRenderableInk* pInkNodeEnd,BOOL ProgressBar)
{
	if (GetNodeBlend())
	{
		CProfileBiasGain* pObjProfile = GetNodeBlend()->GetObjectProfile();
		if (pObjProfile)
		{
			// this ensures that blend object profile processing is ON if we need it ....
			GetNodeBlend()->RequestObjectProfileProcessing(TRUE);
		}
	}
	
	if (m_Initialised) return TRUE;

	/* Diccon 9/99 changes to permit blending between two blends using arbitrary start and end points
	 mean that NodeBlenders are no longer necessarily CMapPtrToPtr::iteratored in between the two nodes that they are
	 blending.  This means that the third method of finding start and end nodes is now likely to yield
	 incorrect results, with the sole exception of when you are loading an old format file  */

	if (pInkNodeStart == NULL) pInkNodeStart = m_pNodeStart;
	if (pInkNodeEnd   == NULL) pInkNodeEnd   = m_pNodeEnd;

	// If still no start and end node, use the ObjIndex value to find them
	if (pInkNodeStart == NULL && m_ObjIndexStart >= 0) pInkNodeStart = FindObjIndexedNode(m_ObjIndexStart);
	if (pInkNodeEnd   == NULL && m_ObjIndexEnd   >= 0) pInkNodeEnd   = FindObjIndexedNode(m_ObjIndexEnd);

	// the only time we should be using this method is if we are loading old format files with blends in them
	if (pInkNodeStart == NULL) pInkNodeStart = (NodeRenderableInk*)FindPrevious(CC_RUNTIME_CLASS(NodeRenderableInk));
	if (pInkNodeEnd   == NULL) pInkNodeEnd   = (NodeRenderableInk*)FindNext(CC_RUNTIME_CLASS(NodeRenderableInk));
	
	while (pInkNodeEnd->IS_KIND_OF(NodeBlendPath))
		pInkNodeEnd   = (NodeRenderableInk*)pInkNodeEnd->FindNext(CC_RUNTIME_CLASS(NodeRenderableInk));

	
	// In debug builds, inform the user, as something's gone wrong
	ERROR3IF(pInkNodeStart == NULL,"pInkNodeStart is NULL");
	ERROR3IF(pInkNodeEnd   == NULL,"pInkNodeEnd is NULL");
	
	if (pInkNodeStart == NULL || pInkNodeEnd == NULL) return TRUE;

	if (ProgressBar)
	{
		Progress Hourglass(_R(IDS_REINTBLEND));
		return (Initialise(pInkNodeStart,pInkNodeEnd,m_PathIndexStart,m_PathIndexEnd,NULL,&Hourglass,TRUE));
	}
	else
		return (Initialise(pInkNodeStart,pInkNodeEnd,m_PathIndexStart,m_PathIndexEnd,NULL,NULL,TRUE));
}

/********************************************************************************************

>	void NodeBlender::Deinit(BOOL bNodesMayBeChanged = FALSE)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/11/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	This deinits the node, setting its internal Initialised flag to FALSE,
				and deleting any memory that is created during initialisation.
				It's like a combination of the constructor and destructor, leaving it in 
				a state similar to defualt construction.
				The state is such that a call to Reinit() can exactly recreate the blender node.
				Used when hiding NodeBlenders to free up some memory.
	SeeAlso:	-

********************************************************************************************/

void NodeBlender::Deinit(BOOL bNodesMayBeChanged)
{
	// CGS:  we must now delete any intermediate blend step generated stuff ....
	// NOTE:  these lists will only contains items when we have been asked to do
	// a blend of a grouped blend to another grouped blend.

	Node* node = FindPrevious ();

	NodeBlend* node2 = (NodeBlend*) node->FindFirstChild (CC_RUNTIME_CLASS (NodeBlend));

	NodeGroup::KillAllBlendBecomeAConsListItem (node2);

	if (bNodesMayBeChanged)
	{
		m_pNodeStart = NULL;
		m_pNodeEnd = NULL;
	}
	
	if (m_Initialised)
	{
		DELPTR(m_pRefStart);
		DELPTR(m_pRefEnd);

		m_Initialised   = FALSE;
	}
	
	// if we are on a curve then destroy the cached points of the curve
	NodeBlendPath* pNodeBlendPath = GetNodeBlendPath();
	if (pNodeBlendPath != NULL)
		pNodeBlendPath->DestroyCachedInformation();
}

/********************************************************************************************

>	UINT32 NodeBlender::GetNumBlendSteps()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/11/94
	Inputs:		-
	Outputs:	-
	Returns:	Num blend steps for this blender
	Purpose:	Gets the number of blend steps for this blender, by asking the parent blend node
				for its num blend steps
	SeeAlso:	-

********************************************************************************************/

UINT32 NodeBlender::GetNumBlendSteps()
{
	UINT32 NumBlendSteps = 5;

	NodeBlend* pNode = GetNodeBlend();
	if (pNode != NULL)
		NumBlendSteps = pNode->GetNumBlendSteps();
	else
	{
		ERROR3("Parent of blender is not there, or is not a NodeBlend!!!");
	}

	return NumBlendSteps;
}

/********************************************************************************************

>	ColourBlendType NodeBlender::GetColourBlendType()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/3/95
	Inputs:		-
	Outputs:	-
	Returns:	How colours are blended
	Purpose:	Gets the way colours are blended by asking the parent blend node
	SeeAlso:	-

********************************************************************************************/

ColourBlendType NodeBlender::GetColourBlendType()
{
	ColourBlendType ColBlendType = COLOURBLEND_FADE;

	NodeBlend* pNode = GetNodeBlend();
	if (pNode != NULL)
		ColBlendType = pNode->GetColourBlendType();
	else
	{
		ERROR3("Parent of blender is not there, or is not a NodeBlend!!!");
	}

	return ColBlendType;
}

/********************************************************************************************

>	BOOL NodeBlender::IsOneToOne()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/11/94
	Inputs:		-
	Outputs:	-
	Returns:	The one-to-one flag state
	Purpose:	Gets the one-to-one blend mapping state for this blender, by asking the parent blend node
	SeeAlso:	-

********************************************************************************************/

BOOL NodeBlender::IsOneToOne()
{
	BOOL OneToOne = FALSE;

	NodeBlend* pNode = GetNodeBlend();
	if (pNode != NULL)
		OneToOne = pNode->IsOneToOne();
	else
	{
		ERROR3("Parent of blender is not there, or is not a NodeBlend!!!");
	}

	return OneToOne;
}

/********************************************************************************************

>	BOOL NodeBlender::IsNotAntialiased()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/11/94
	Inputs:		-
	Outputs:	-
	Returns:	The NotAntialiased flag state
	Purpose:	Gets the NotAntialiased state for this blender, by asking the parent blend node
	SeeAlso:	-

********************************************************************************************/

BOOL NodeBlender::IsNotAntialiased()
{
	BOOL NotAntialiased = FALSE;

	NodeBlend* pNode = GetNodeBlend();
	if (pNode != NULL)
		NotAntialiased = pNode->IsNotAntialiased();
	else
	{
		ERROR3("Parent of blender is not there, or is not a NodeBlend!!!");
	}

	return NotAntialiased;
}

/********************************************************************************************

>	BOOL NodeBlender::IsTangential()

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/5/96
	Inputs:		-
	Returns:	TRUE if this blend should follow the curve tangentially
	Purpose:	See above
	SeeAlso:	-

********************************************************************************************/

BOOL NodeBlender::IsTangential()
{
	NodeBlend* pNode = GetNodeBlend();
	if (pNode != NULL)
		return pNode->IsTangential();

	return FALSE;
}


/********************************************************************************************

>	INT32 NodeBlender::GetNodeBlendPathIndex()

 	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/9/99
	Inputs:		-
	Returns:	the index to the nodeblendpath to which this blender refers, or -1
				if it is not on a curve
	Purpose:	See above
	SeeAlso:	-

********************************************************************************************/

INT32 NodeBlender::GetNodeBlendPathIndex()
{
	return m_NodeBlendPathIndex;
}


/********************************************************************************************

>	void NodeBlender::SetNodeBlendPathIndex(INT32 Index)

 	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/9/99
	Inputs:		the index to the nodeblendpath to which this blender refers, or -1
				if it is not on a curve
	Returns:	-
	Purpose:	See above
	SeeAlso:	-

********************************************************************************************/

void NodeBlender::SetNodeBlendPathIndex(INT32 Index)
{
	if (Index < -2)
	{
		ERROR3("Invalid index value");
		return;
	}

	m_NodeBlendPathIndex = Index;
	return;
}


/********************************************************************************************

>	BOOL NodeBlender::ReverseEnds()

 	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/9/99
	Inputs:		-
	Returns:	TRUE if successfull, FALSE otherwise
	Purpose:	reverses the blend so that the start node becomes the end node and vice versa
	SeeAlso:	-

********************************************************************************************/

BOOL NodeBlender::ReverseEnds()
{
	NodeRenderableInk* pTemp = NULL;
	pTemp = m_pNodeStart;
	m_pNodeStart = m_pNodeEnd;
	m_pNodeEnd = pTemp;

	SetReversed(!IsReversed());

	INT32 Temp = m_ObjIndexStart;
	m_ObjIndexStart = m_ObjIndexEnd;
	m_ObjIndexEnd = Temp;

	return TRUE;
}


/********************************************************************************************

>	BOOL NodeBlender::PreBlend()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/11/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if it's OK to continue, FALSE otherwise
	Purpose:	This should be called just before blending occurs.
				At the moment it does the following -
					Initialises stuff so you can call GetCoordArray(), GetVerbArray() and GetFlagArray()
	SeeAlso:	-

********************************************************************************************/

BOOL NodeBlender::PreBlend()
{
	// Clean up in case PostBlend() wasn't called after the last PreBlend() call
	PostBlend();
	if (m_pRefStart != NULL) m_pRefStart->PreBlend();
	if (m_pRefEnd   != NULL) m_pRefEnd  ->PreBlend();

	// Allocate the arrays
	return (ReallocTempBuffers(1000));
}

/********************************************************************************************

>	void NodeBlender::PostBlend()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/11/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	This should be called just after blending occurs.
				At the moment it does the following -
					Releases memory allocated for the temp path arrays
	SeeAlso:	-

********************************************************************************************/

void NodeBlender::PostBlend()
{
	if (m_pRefStart != NULL) m_pRefStart->PostBlend();
	if (m_pRefEnd   != NULL) m_pRefEnd  ->PostBlend();
	DeallocTempBuffers();
}

/********************************************************************************************

> BOOL NodeBlender::ReallocTempBuffers(UINT32 Size)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/11/94
	Inputs:		Size = Size the temp arrays should be
	Outputs:	-
	Returns:	-
	Purpose:	Allocates memory for the temp path arrays, and sets the size var to 0
				You can use calls to GetCoordArray(), GetVerbArray() and GetFlagArray() to get the alloced arrays.
	SeeAlso:	-

********************************************************************************************/

BOOL NodeBlender::ReallocTempBuffers(UINT32 Size)
{
	DeallocTempBuffers();

	// Allocate the arrays
	m_TempArraySize = Size;
	m_pTempCoords   = (DocCoord*)  CCMalloc(Size*sizeof(DocCoord));
	m_pTempVerbs    = (PathVerb*)  CCMalloc(Size*sizeof(PathVerb));
	m_pTempFlags    = (PathFlags*) CCMalloc(Size*sizeof(PathFlags));

	// If any of the arrays are not allocated, dealloc the alloced ones, and return FALSE
	if (m_pTempCoords == NULL || m_pTempVerbs == NULL || m_pTempFlags == NULL)
	{
		DeallocTempBuffers();
		return FALSE;
	}

	// It's all OK, so return TRUE
	return TRUE;
}

/********************************************************************************************

>	void NodeBlender::DeallocTempBuffers()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/11/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Releases memory allocated for the temp path arrays, and sets the size var to 0
	SeeAlso:	-

********************************************************************************************/

void NodeBlender::DeallocTempBuffers()
{
	if (m_pTempCoords != NULL) { CCFree(m_pTempCoords); m_pTempCoords = NULL; }
	if (m_pTempVerbs  != NULL) { CCFree(m_pTempVerbs);  m_pTempVerbs  = NULL; }
	if (m_pTempFlags  != NULL) { CCFree(m_pTempFlags);  m_pTempFlags  = NULL; }
	m_TempArraySize = 0;

	if (m_pGBlendBuff != NULL) { CCFree(m_pGBlendBuff); m_pGBlendBuff = NULL; }
	m_GBlendBuffSize = 0;
}

/********************************************************************************************

>	DocCoord*  NodeBlender::GetCoordArray(UINT32 MinSize);

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/11/94
	Inputs:		MinSize = min size the array should be
	Outputs:	-
	Returns:	Ptr to the array, or NULL if can't get memory
	Purpose:	Used to get an array you can write to.
	SeeAlso:	-

********************************************************************************************/

DocCoord* NodeBlender::GetCoordArray(UINT32 MinSize)
{
	MinSize++;
	if (m_TempArraySize >= MinSize) return m_pTempCoords;

	if (ReallocTempBuffers(MinSize))
		return m_pTempCoords;
	else
		return NULL;
}

/********************************************************************************************

>	PathVerb*  NodeBlender::GetVerbArray(UINT32 MinSize);

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/11/94
	Inputs:		MinSize = min size the array should be
	Outputs:	-
	Returns:	Ptr to the array, or NULL if can't get memory
	Purpose:	Used to get an array you can write to.
	SeeAlso:	-

********************************************************************************************/

PathVerb* NodeBlender::GetVerbArray(UINT32 MinSize)
{
	MinSize++;
	if (m_TempArraySize >= MinSize) return m_pTempVerbs;

	if (ReallocTempBuffers(MinSize))
		return m_pTempVerbs;
	else
		return NULL;
}

/********************************************************************************************

>	PathFlags*  NodeBlender::GetFlagArray(UINT32 MinSize);

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/11/94
	Inputs:		MinSize = min size the array should be
	Outputs:	-
	Returns:	Ptr to the array, or NULL if can't get memory
	Purpose:	Used to get an array you can write to.
	SeeAlso:	-

********************************************************************************************/

PathFlags* NodeBlender::GetFlagArray(UINT32 MinSize)
{
	MinSize++;
	if (m_TempArraySize >= MinSize) return m_pTempFlags;

	if (ReallocTempBuffers(MinSize))
		return m_pTempFlags;
	else
		return NULL;
}

/********************************************************************************************

>	UINT32*  NodeBlender::GetGBlendBuff(UINT32 MinSize);

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/11/94
	Inputs:		MinSize = min size the buffer should be
	Outputs:	-
	Returns:	Ptr to the buffer, or NULL if can't get memory
	Purpose:	Used to get a buffer you can write to.
	SeeAlso:	-

********************************************************************************************/

UINT32* NodeBlender::GetGBlendBuff(UINT32 MinSize)
{
	MinSize++;
	if (m_GBlendBuffSize >= MinSize) return m_pGBlendBuff;

	if (m_pGBlendBuff != NULL) CCFree(m_pGBlendBuff);

	m_pGBlendBuff = (UINT32*) CCMalloc(MinSize*sizeof(UINT32));

	if (m_pGBlendBuff != NULL)
		m_GBlendBuffSize = MinSize;
	else
		m_GBlendBuffSize = 0;

	return m_pGBlendBuff;
}

/********************************************************************************************

> BOOL NodeBlender::Remap(DocCoord PosStart,DocCoord PosEnd,DocCoord* pInvPosStart,DocCoord* pInvPosEnd)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/11/94
	Inputs:		PosStart 	= Coord of element to remap start node to
				PosEnd   	= Coord of element to remap end node to
				pInvPosStart= ptr to place to put the coord to use to invert the start mapping
				pInvPosEnd  = ptr to place to put the coord to use to invert the end mapping
	Outputs:	-
	Returns:	TRUE if remapped, FALSE otherwise
	Purpose:	If this blender manages to remap a pair of blend paths, then this will return
				TRUE. 
	SeeAlso:	-

********************************************************************************************/

BOOL NodeBlender::Remap(DocCoord PosStart,DocCoord PosEnd,DocCoord* pInvPosStart,DocCoord* pInvPosEnd)
{
	BOOL Mapped = FALSE;

	if (m_pRefStart != NULL && m_pRefEnd != NULL)
	{
		BlendPath* pBlendPathStart = m_pRefStart->GetFirstBlendPath();
		BlendPath* pBlendPathEnd   = m_pRefEnd  ->GetFirstBlendPath();

		if (pBlendPathStart != NULL && pBlendPathEnd != NULL)
		{
			if (GetNodeBlendPath() != NULL)
			{
				Trans2DMatrix* pRotateStart = GetRotateMatrix(m_pRefStart->GetNode(),360.0 - m_AngleStart);
				Trans2DMatrix* pRotateEnd   = GetRotateMatrix(m_pRefEnd  ->GetNode(),360.0 - m_AngleEnd  );
				if (pRotateStart)	pRotateStart->Transform(&PosStart,1);
				if (pRotateEnd)		pRotateEnd  ->Transform(&PosEnd  ,1);
				DELPTR(pRotateStart);
				DELPTR(pRotateEnd);
			}

			*pInvPosStart = pBlendPathStart->MapPath(PosStart);
			*pInvPosEnd   = pBlendPathEnd  ->MapPath(PosEnd);
			Mapped = TRUE;

			if (GetNodeBlendPath() != NULL)
			{
				Trans2DMatrix* pRotateStart = GetRotateMatrix(m_pRefStart->GetNode(),m_AngleStart);
				Trans2DMatrix* pRotateEnd   = GetRotateMatrix(m_pRefEnd  ->GetNode(),m_AngleEnd  );
				if (pRotateStart)	pRotateStart->Transform(pInvPosStart,1);
				if (pRotateEnd)		pRotateEnd  ->Transform(pInvPosEnd  ,1);
				DELPTR(pRotateStart);
				DELPTR(pRotateEnd);
			}
		}

		// Keep the path mapping index up to date (necessary for cut/copy/paste, export, etc).
		m_PathIndexStart = m_pRefStart->GetOrigMapping();
		m_PathIndexEnd   = m_pRefEnd  ->GetOrigMapping();
	}

	return Mapped;
}

/********************************************************************************************

> BOOL NodeBlender::ConvertAWPathIndexesToCamelot(INT32 *pPathIndexStart,INT32* pPathIndexEnd)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/11/94
	Inputs:		pPathIndexStart = index of element in start node 
				pPathIndexEnd   = index of element in end node
	Outputs:	*pPathIndexStart = possibly modified index for start node
				*pPathIndexEnd   = possibly modified index for end node
	Returns:	TRUE if ok, FALSE otherwise
	Purpose:	This converts ArtWorks path indexes to a Camelot path indexes, for this blender.
				An AW index is just an index to the nth end point, so all that's done is the nth end
				point in the path is found.
	SeeAlso:	-

********************************************************************************************/

BOOL NodeBlender::ConvertAWPathIndexesToCamelot(INT32 *pPathIndexStart,INT32* pPathIndexEnd)
{
	BOOL 	ok = FindPathEndPoint(FindPrevious(),pPathIndexStart);
	if (ok) ok = FindPathEndPoint(FindNext(),    pPathIndexEnd);
	return ok;
}

/********************************************************************************************

> BOOL NodeBlender::FindPathEndPoint(Node* pNodePath,INT32* pIndex)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/11/94
	Inputs:		pNodePath   = ptr to a NodePath
				pIndex		= ptr to INT32 - must contain the number of the endpoint desired (N)
	Outputs:	*pIndex 	= the index to the N'th endpoint.
	Returns:	TRUE if ok, FALSE otherwise
	Purpose:	This finds the N'th end point in the given NodePath.
				This is used to convert AW end point indexes into Camelot end point indexes.
				If *pIndex <=0 on entry, *pIndex = 0 on exit
	SeeAlso:	-

********************************************************************************************/

BOOL NodeBlender::FindPathEndPoint(Node* pNodePath,INT32* pIndex)
{
	ERROR3IF(pIndex == NULL,"pIndex == NULL");
	ERROR3IF(pNodePath == NULL,"pNodePath == NULL");
	if (pIndex == NULL || pNodePath == NULL)
		return FALSE;

//	ERROR3IF(!pNodePath->IS_KIND_OF(NodePath),"pNodePath is not kind of NodePath");
	if (!pNodePath->IS_KIND_OF(NodePath))
		*pIndex = 0;

	// if *pIndex < 0 then this has a special meaning so preserve it and return TRUE
	// (e.g. -1 means find bottom left element) 
	if (*pIndex < 0)
		return TRUE;

	Path* pPath = &(((NodePath*)pNodePath)->InkPath);

	INT32 Index=0;
	for (INT32 n=*pIndex;n>0;n--)
	{
		if (!pPath->FindNextEndPoint(&Index))
			return FALSE;
	}

	*pIndex = Index;

	return TRUE;
}


//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------

/***********************************************************************************************

> BlendRef::BlendRef()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/10/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Default constructor.

***********************************************************************************************/

BlendRef::BlendRef()
{
	m_pNode 			= NULL;
	m_NumBlendPaths 	= 0;
	m_pBlendPathAttrMap = NULL;
}

/***********************************************************************************************

> BlendRef::~BlendRef()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/10/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Default destructor.

***********************************************************************************************/

BlendRef::~BlendRef()
{
	m_BlendPathList.DeleteAll();
	DELPTR(m_pBlendPathAttrMap);
}

/***********************************************************************************************

> BOOL BlendRef::Initialise(NodeRenderableInk* pThisNode,INT32 Index,UndoableOperation* pUndoOp,Progress* pProgress,BOOL IgnoreEscape,Trans2DMatrix* pMatrix)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/10/94
	Inputs:		pThisNode 	= ptr to the node in the tree this BlendRef refers to.
				Index	  	= index into path of blob clicked on (-1 means ignore)
				pUndoOp	  	= ptr to an undoable op so that initialisation can be done in an undoable way
							  (can be NULL)
				pProgress 	= ptr to progress object for maintaining hour glass
				IgnoreEscape= TRUE if you don't want this process abortable via the Escape key
				pMatrix		= transformation to apply to each blend path (can be NULL)
	Outputs:	-
	Returns:	TRUE if successfully initialises, FALSE otherwise
	Purpose:	This inits the BlendRef object with the given node.

***********************************************************************************************/
/*
BOOL BlendRef::Initialise(NodeRenderableInk* pThisNode,INT32 Index,UndoableOperation* pUndoOp,Progress* pProgress,BOOL IgnoreEscape,Trans2DMatrix* pMatrix)
{
	ERROR2IF(pThisNode == NULL,FALSE,"pThisNode == NULL");
	ERROR2IF(!pThisNode->CanBecomeA(CC_RUNTIME_CLASS(NodePath)),FALSE,"pThisNode can't become a NodePath!");

	// Reset the member vars of this blend reference
	m_pNode 		= pThisNode;
	m_NumBlendPaths = 0;
	m_BlendPathList.DeleteAll();

	// Set up a BecomeA derived object, so that we can receive all the paths generated by pNode.
	BlendBecomeA ParamBecomeA(	BECOMEA_PASSBACK, 
								CC_RUNTIME_CLASS(NodePath),
								pUndoOp,
								this,
								Index,
								pProgress,
								IgnoreEscape,
								pMatrix);
	

	// Get pNode to generate its paths for us
 	BOOL ok = m_pNode->DoBecomeA(&ParamBecomeA);

	if (!ok)
	{
		// tidy up if things go wrong
		m_pNode 		  = NULL;
		m_NumBlendPaths = 0;
		m_BlendPathList.DeleteAll();
	}

	return (ok);
}
*/

/***********************************************************************************************

> BOOL BlendRef::Initialise(NodeRenderableInk* pThisNode,INT32 Index,UndoableOperation* pUndoOp,Progress* pProgress,
							BOOL IgnoreEscape,Trans2DMatrix* pMatrix, AttrBrushType* pAttrBrush)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/10/94
	Inputs:		pThisNode 	= ptr to the node in the tree this BlendRef refers to.
				Index	  	= index into path of blob clicked on (-1 means ignore)
				pUndoOp	  	= ptr to an undoable op so that initialisation can be done in an undoable way
							  (can be NULL)
				pProgress 	= ptr to progress object for maintaining hour glass
				IgnoreEscape= TRUE if you don't want this process abortable via the Escape key
				pMatrix		= transformation to apply to each blend path (can be NULL)
				pAttrBrush  = the brush attribute applying to pThisNode, using this parameter indicates that
							  you wish to extract paths from the brush.
	Outputs:	-
	Returns:	TRUE if successfully initialises, FALSE otherwise
	Purpose:	This inits the BlendRef object with the given node.

***********************************************************************************************/

BOOL BlendRef::Initialise(NodeRenderableInk* pThisNode,INT32 Index,UndoableOperation* pUndoOp,Progress* pProgress,BOOL IgnoreEscape,
						 Trans2DMatrix* pMatrix, AttrBrushType* pAttrBrush)
{
	ERROR2IF(pThisNode == NULL,FALSE,"pThisNode == NULL");

	// Reset the member vars of this blend reference
	m_pNode 		= pThisNode;
	m_NumBlendPaths = 0;
	m_BlendPathList.DeleteAll();

	// Set up a BecomeA derived object, so that we can receive all the paths generated by pNode.
	BlendBecomeA ParamBecomeA(	BECOMEA_PASSBACK, 
								CC_RUNTIME_CLASS(NodePath),
								pUndoOp,
								this,
								Index,
								pProgress,
								IgnoreEscape,
								pMatrix);
	
	ERROR2IF(!pThisNode->CanBecomeA(&ParamBecomeA), FALSE, "pThisNode can't become a NodePath!");

	BOOL ok = FALSE;

	if (pAttrBrush != NULL)
	{
		// check to see if it is default, if it is not then let the attribute do the work 
		if (pAttrBrush->GetBrushHandle() != BrushHandle_NoBrush)
			ok = pAttrBrush->DoBecomeA(&ParamBecomeA, pThisNode);
		else
			ok = m_pNode->DoBecomeA(&ParamBecomeA);
	}	
	else
		// Get pNode to generate its paths for us
 		ok = m_pNode->DoBecomeA(&ParamBecomeA);

	if (!ok)
	{
		// tidy up if things go wrong
		m_pNode 		  = NULL;
		m_NumBlendPaths = 0;
		m_BlendPathList.DeleteAll();
	}

	return (ok);
}


/***********************************************************************************************

> BOOL BlendRef::InitialiseForBrush(NodeRenderableInk* pThisNode)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/10/94
	Inputs:		pThisNode 	= ptr to the node in the tree this BlendRef refers to.
			
	Outputs:	-
	Returns:	TRUE if successfully initialises, FALSE otherwise
	Purpose:	This inits the BlendRef object with the given node.

***********************************************************************************************/

BOOL BlendRef::InitialiseForBrush(NodeRenderableInk* pThisNode)
{
	ERROR2IF(pThisNode == NULL,FALSE,"pThisNode == NULL");

	// Reset the member vars of this blend reference
	m_pNode 		= pThisNode;
	m_NumBlendPaths = 0;
	m_BlendPathList.DeleteAll();

	// Set up a BecomeA derived object, so that we can receive all the paths generated by pNode.
	BrushBecomeA MyBecomeA(BECOMEA_PASSBACK, 
								CC_RUNTIME_CLASS(NodePath),
								NULL,
								this);
	
	ERROR2IF(!pThisNode->CanBecomeA(&MyBecomeA), FALSE, "pThisNode can't become a NodePath!");

	// if we've got a stroke attribute applied then we must make a special case
/*	AttrStrokeType* pStroke = NULL;
	AttrVariableWidth* pVarWidth = NULL;
	pThisNode->FindAppliedAttribute(CC_RUNTIME_CLASS(AttrStrokeType), (NodeAttribute**)&pStroke);
	pThisNode->FindAppliedAttribute(CC_RUNTIME_CLASS(AttrVariableWidth), (NodeAttribute**)&pVarWidth);

	BOOL ok = FALSE;
	if (pStroke && pVarWidth && pVarWidth->HasActiveValueFunction())
		ok = pStroke->DoBecomeA(&BecomeA, m_pNode);
	else
	*/
 	BOOL ok = m_pNode->DoBecomeA(&MyBecomeA);

	if (!ok)
	{
		// tidy up if things go wrong
		m_pNode 		  = NULL;
		m_NumBlendPaths = 0;
		m_BlendPathList.DeleteAll();
	}

	return (ok);

}

/***********************************************************************************************

> BOOL BlendRef::AddBlendPath(BlendPath* pBlendPath)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/10/94
	Inputs:		pBlendPath = ptr to blend path to add
	Outputs:	-
	Returns:	TRUE if successfully initialises, FALSE otherwise
	Purpose:	Add the blend path object to this blend ref
				NOTE: Once you add a BlendPath to a BlendRef, it becomes property of the BlendRef
					  which can alter/delete it at any time.
					  DO NOT KEEP A COPY OF THE PTR pBlendPath!!!! It may not be there next time
					  use it.

***********************************************************************************************/

BOOL BlendRef::AddBlendPath(BlendPath* pBlendPath)
{
	m_BlendPathList.AddTail(pBlendPath);
	m_NumBlendPaths++;

	return TRUE;
}

/***********************************************************************************************

> BlendPath* BlendRef::GetFirstBlendPath()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/10/94
	Inputs:		-
	Outputs:	-
	Returns:	First blend path in the list, or NULL if the list is empty
	Purpose:	Lets you get the first blend path in the list

***********************************************************************************************/

BlendPath* BlendRef::GetFirstBlendPath()
{
	return ((BlendPath*)m_BlendPathList.GetHead());
}

/***********************************************************************************************

> BlendPath* BlendRef::GetNextBlendPath(BlendPath* pCurrentBlendPath)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/10/94
	Inputs:		pCurrentBlendPath = ptr the current blend path in the list
	Outputs:	-
	Returns:	Returns the next blend path after pCurrentBlendPath, or NULL if there isn't a next one
	Purpose:	Lets you get the next blend path in the list

***********************************************************************************************/

BlendPath* BlendRef::GetNextBlendPath(BlendPath* pCurrentBlendPath)
{
	return ((BlendPath*)m_BlendPathList.GetNext(pCurrentBlendPath));
}

/***********************************************************************************************

> BlendPath* BlendRef::GetBlendPath(INT32 Index)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/10/94
	Inputs:		Index = index of the list item (0 is the first item)
	Outputs:	-
	Returns:	Returns the blend path who's index is Index, or NULL if there isn't a next one
	Purpose:	Lets you get the nth blend path in the list
	SeeAlso:	GetNumBlendPaths();

***********************************************************************************************/

BlendPath* BlendRef::GetBlendPath(INT32 Index)
{
	if (m_NumBlendPaths <= 0)
		return NULL;

	if (Index >= INT32(m_NumBlendPaths))	{ ERROR3("Index too big"); Index = m_NumBlendPaths-1; }
	if (Index < 0) 						{ ERROR3("Index is -ve "); Index = 0; }

	BlendPath* pBlendPath = GetFirstBlendPath();
	while (pBlendPath != NULL && Index > 0)
	{
		pBlendPath = GetNextBlendPath(pBlendPath);
		Index--;
	}
	ERROR3IF(pBlendPath==NULL,"pBlendPath == NULL");
	return (pBlendPath);
}


/********************************************************************************************

>	void BlendRef::Transform( TransformBase& Trans )

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/10/94
	Inputs:		Trans - The transform Object
	Purpose:	Transforms all the BlendPaths attached to this blend reference

********************************************************************************************/

void BlendRef::Transform( TransformBase& Trans )
{
	BlendPath* pBlendPath = GetFirstBlendPath();

	while (pBlendPath != NULL)
	{
		pBlendPath->Transform(Trans);
		pBlendPath = GetNextBlendPath(pBlendPath);
	}	
}

/********************************************************************************************

>	void BlendRef::RenderBlendBlobs(RenderRegion* pRender,Trans2DMatrix* pMatrix)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/11/94
	Inputs:		pRender = region to render blobs into.
				pMatrix = matrix to apply to the blob coords
	Outputs:	-
	Returns:	-
	Purpose:	Renders all the blend blobs associated with all the blend paths in this blend ref

********************************************************************************************/

void BlendRef::RenderBlendBlobs(RenderRegion* pRender,Trans2DMatrix* pMatrix)
{
	BlendPath* pBlendPath = GetFirstBlendPath();

	while (pBlendPath != NULL)
	{
		pBlendPath->RenderBlendBlobs(pRender,pMatrix);
		pBlendPath = GetNextBlendPath(pBlendPath);
	}	
}

/********************************************************************************************

> BOOL BlendRef::IsPointOverBlob(DocCoord* pPointerPos,BlendPath** ppBlendPath,INT32* pIndex,Trans2DMatrix* pMatrix);

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/11/94
	Inputs:		pPointerPos  = point to check against
				ppBlendPath = ptr to place to store blend path ptr if a match is found
				pIndex	    = ptr to place to store index if match is found
				pMatrix		= matrix to apply to the path points before testing with pointer pos (can be NULL)
	Outputs:	*ppBlendPath & *pIndex, updated if TRUE returned, undefined otherwise
	Returns:	TRUE if match  found, FALSE otherwise
	Purpose:	This sees if the point given lies on a selected blob.
				If a match is found, the ptr to the blend path and index to blob element is returned.

				Markn 26-5-99:	Modified so that if the function returns TRUE, *pPointerPos is updated
								so that it contains the coords of the blob it is over
	SeeAlso:	-

********************************************************************************************/

BOOL BlendRef::IsPointOverBlob(DocCoord* pPointerPos,BlendPath** ppBlendPath,INT32* pIndex,Trans2DMatrix* pMatrix)
{
	if (pPointerPos == NULL) return FALSE;
	if (GetNumBlendPaths() != 1) return FALSE;

	BOOL Found = FALSE;
	BlendPath* pBlendPath = GetFirstBlendPath();

	if (pBlendPath != NULL)
	{
		Found = pBlendPath->IsPointOverBlob(pPointerPos,pIndex,pMatrix);
		if (Found) *ppBlendPath = pBlendPath;
	}

	return Found;
}

/********************************************************************************************

> INT32 BlendRef::GetOrigMapping()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/11/94
	Inputs:		-
	Outputs:	-
	Returns:	The mapping that should be applied to the original path
	Purpose:	The path in question could well have been remapping (i.e. rotated to change the el
				at index 0) many times.  This func returns the mapping (i.e. the index of the el that
				should be at index 0) that should be appied to the original path in order to get the same result.
				If the ref has more than one blend path, -1 is returned, meaning choose a default element as the
				start node (e.g. left most, bottom most element)
	SeeAlso:	-

********************************************************************************************/

INT32 BlendRef::GetOrigMapping()
{
	INT32 Mapping = -1;

	if (GetNumBlendPaths() == 1)
	{		
		BlendPath* pBlendPath = GetFirstBlendPath();

		if (pBlendPath != NULL)
			Mapping = pBlendPath->GetOrigMapping();
	}
		
	return Mapping;
}

/********************************************************************************************

>	void BlendRef::PreBlend()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/11/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Called just before a blender node is about to do its blend.
				It asks all its blend paths to find their applied attributes.

********************************************************************************************/

void BlendRef::PreBlend()
{
	// If we haven't got a blend path attr map yet, make one (don't worry if we fail)
	if (m_pBlendPathAttrMap == NULL && m_NumBlendPaths > 0)
PORTNOTE("other","Maps aren't hash bashed, so have no initial size")
		m_pBlendPathAttrMap = new CMapPtrToPtr(); // (m_NumBlendPaths);
}

/********************************************************************************************

>	void BlendRef::PostBlend()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/11/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Called just after a blender node has done its blend.
				It asks all its blend paths to free their applied attributes.

********************************************************************************************/

void BlendRef::PostBlend()
{
	// Free up any memory used to hold applied attribute tables in the blend paths
	BlendPath* pBlendPath = GetFirstBlendPath();
	while (pBlendPath != NULL)
	{
		pBlendPath->FreeAppliedAttributes();
		pBlendPath = GetNextBlendPath(pBlendPath);
	}	

	// Remove all the items in the map of blend path attr maps
	if (m_pBlendPathAttrMap != NULL)
		m_pBlendPathAttrMap->clear();
}

/********************************************************************************************

>	CCAttrMap* BlendRef::FindAppliedAttributes(BlendPath* pBlendPath)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/11/94
	Inputs:		pBlendPath = the blend path that has the attrs you want
	Outputs:	-
	Returns:	-
	Purpose:	Use this to get the attrs of a blend path that belongs to this blend reference.
				It keeps its own map keyed by the node which created the blend path, so that
				FindAppliedAttributes() is only called once for each node that created blend paths,
				rather than once for every blend path produced by the node (potentially many).

********************************************************************************************/

CCAttrMap* BlendRef::FindAppliedAttributes(BlendPath* pBlendPath)
{
	// If the attrs applied to this blend path are unique to the blend path (i.e. not the same
	// as the attrs applid to the node that created the blend path) return the unique attrs
	if (pBlendPath->UniqueAppliedAttrs())
		return (pBlendPath->FindAppliedAttributes());

	// If we haven't got a map of attr maps yet, try the blend path directly
	if (m_pBlendPathAttrMap == NULL)
		return (pBlendPath->FindAppliedAttributes());

	// pAttrMap will hold the attrs applied to pBlendPath
	CCAttrMap* pAttrMap = NULL;

	// the map of attr maps is keyed by the ptr to the node that created the blend path
	NodeRenderableInk* pCreatedByNode = pBlendPath->GetCreatedByNode();

	if (pCreatedByNode->IsCompound())
	{
		pCreatedByNode = ((NodeCompound *)pCreatedByNode)->GetNodeToBlend();
	}

	if (pCreatedByNode != NULL)
	{
		// Do we have an attr map for pCreatedByNode?
		CMapPtrToPtr::iterator iter = m_pBlendPathAttrMap->find( pCreatedByNode );
		if( m_pBlendPathAttrMap->end() != iter )
			pAttrMap = (CCAttrMap *)iter->second;
		else
		{
			// If not, get the blend path to return us an attr map
			// and put the ptr in our map of attr maps, keyed by pCreatedByNode.
			pAttrMap = pBlendPath->FindAppliedAttributes();
			if (pAttrMap != NULL)
				(*m_pBlendPathAttrMap)[pCreatedByNode] = pAttrMap;
		}
	}

	return pAttrMap;
}



/********************************************************************************************

>	DocRect BlendRef::GetBoundingRect()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/11/99
	Inputs:		-
	Outputs:	-
	Returns:	the bounding rect of all the blendpaths in the blendref
	Purpose:

********************************************************************************************/

DocRect BlendRef::GetBoundingRect()
{
	DocRect Rect;

	BlendPath *pBlendPath = GetFirstBlendPath();
	while (pBlendPath != NULL)
	{
		DocRect PathRect = pBlendPath->m_pPath->GetBoundingRect();
		Rect = Rect.Union(PathRect);

		pBlendPath = GetNextBlendPath(pBlendPath);
	}
	return Rect;
}


/********************************************************************************************

>	void BlendRef::StripRedundantNodeBlendPaths (BlendRef* spouse)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/7/2000
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Strips redundant NodeBlendPaths from the blend refs.  This is needed in the
				case that we are required to blend a blend on a both to a non-blend on a path
				(and vice versa).

********************************************************************************************/

void BlendRef::StripRedundantNodeBlendPaths (BlendRef* spouse)
{
	INT32 numbBlendPathInited = 0, spouseNumbBlendPathInited = 0;
	
	BlendPath *pBlendPath = GetFirstBlendPath();
	while (pBlendPath != NULL)
	{
		if (pBlendPath->GetCreatedViaNodeBlendPath ())
		{
			numbBlendPathInited++;
		}

		pBlendPath = GetNextBlendPath(pBlendPath);
	}

	pBlendPath = spouse->GetFirstBlendPath();
	while (pBlendPath != NULL)
	{
		if (pBlendPath->GetCreatedViaNodeBlendPath ())
		{
			spouseNumbBlendPathInited++;
		}

		pBlendPath = spouse->GetNextBlendPath(pBlendPath);
	}

	// only strip in this case ....
	if (numbBlendPathInited != spouseNumbBlendPathInited)
	{
		if ((numbBlendPathInited == 0) && (spouseNumbBlendPathInited > 0))
		{
			// strip our spouse ....

			pBlendPath = spouse->GetFirstBlendPath();
			while (pBlendPath != NULL)
			{
				if (pBlendPath->GetCreatedViaNodeBlendPath ())
				{
					BlendPath* remover = pBlendPath;
					pBlendPath = spouse->GetNextBlendPath(pBlendPath);
					delete (spouse->m_BlendPathList.RemoveItem (remover));
					spouse->m_NumBlendPaths--;
				}
				else
				{
					pBlendPath = spouse->GetNextBlendPath(pBlendPath);
				}
			}
		}
		else if ((spouseNumbBlendPathInited == 0) && (numbBlendPathInited > 0))
		{
			// strip ourselves ....

			pBlendPath = GetFirstBlendPath();
			while (pBlendPath != NULL)
			{
				if (pBlendPath->GetCreatedViaNodeBlendPath ())
				{
					BlendPath* remover = pBlendPath;
					pBlendPath = GetNextBlendPath(pBlendPath);
					delete (m_BlendPathList.RemoveItem (remover));
					m_NumBlendPaths--;
				}
				else
				{
					pBlendPath = GetNextBlendPath(pBlendPath);
				}
			}
		}
		// else - oh well
	}
}



/********************************************************************************************

>	BOOL BlendRef::RemoveLastBlendPath()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/11/99
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if successful, FALSE if not
	Purpose:	Not a function that most people will have cause to use, removes the tail item in 
				the blendpath list.

********************************************************************************************/

BOOL BlendRef::RemoveLastBlendPath()
{
	if (m_BlendPathList.IsEmpty())
		return FALSE;

	m_BlendPathList.RemoveTail();
	m_NumBlendPaths--;
	
	return TRUE;
}


//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------

/***********************************************************************************************

> BlendPath::BlendPath()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/10/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Default constructor

***********************************************************************************************/

BlendPath::BlendPath()
{
	m_pBlendNode    = NULL;
	m_pPath	      	= NULL;
	m_pCreatedByNode= NULL;
	m_pAppliedAttrs	= NULL;

	m_CloseFigure	= 0;
	m_OrigMapping	= 0;
	m_PathType 		= PATHTYPE_NONE;
	m_SubPathID		= 0;
	m_DirChanged	= FALSE;
	m_NumElements	= -1;
	m_UniqueAttrs	= FALSE;

	m_bDeleteAll    = FALSE;
	m_bFreeAttrMapOnDestruct = FALSE;

	m_AppliedLineWidth = -1;

	m_bCreatedViaNodeBlendPath = FALSE;

	m_pCopyPath = NULL;
	m_pCopyAttrs = NULL;
}

/***********************************************************************************************

> BlendPath::~BlendPath()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/10/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Default destructor

	Notes:		This destructor has actually gotten quite complicated as it Blendpaths are an important
				part of the brush attribute and there are a number of different cases to deal with:
				- The normal case is where the blendpath is part of a blend, in this case it may wish to 
				delete its applied attributes, but more than often will not, as they are still used in the tree.

				The brush has two deletion scenarios:
				- normally the m_bFreeAttributes flag is set, as we wish to ensure the attribute map is 
				deleted but we do not wish to delete the attributes (as they are probably shared)
				- if this blendpath has been created specially as part of a blend between two brush 
				attributes, then the m_bDeleteAll flag will be set, which asks that we delete everything

***********************************************************************************************/

BlendPath::~BlendPath()
{
	// Delete our copies
	DeleteCopyPathAndAttributes();
	
	// Only delete the applied attr map if this is the first sub path that was generated from the
	// source path supplied when making shapes.
	// This assumes that all sub-paths within a given path have the same attributes (far assumption)

	if (m_SubPathNum == 0 || m_bDeleteAll)
	{
		if ((m_UniqueAttrs || m_bDeleteAll) && m_pAppliedAttrs != NULL)
		{
			CCRuntimeClass *pType;
			void	   *pVal;
			// delete all attrs
			CMapPtrToPtr::iterator Pos = m_pAppliedAttrs->GetStartPosition();
			CMapPtrToPtr::iterator End = m_pAppliedAttrs->GetEndPosition();
			for(; Pos != End;)
			{
				// Get attr at CMapPtrToPtr::iterator Pos
				m_pAppliedAttrs->GetNextAssoc( Pos, pType, pVal );
				if (pVal != NULL)
					delete (NodeAttribute*)pVal;
			}
		}
		if (m_pAppliedAttrs	!= NULL)
		{
			delete m_pAppliedAttrs;
			m_pAppliedAttrs = NULL;
		}
	}
	// m_bFreeAttributes should only really be set by brushing code, so unless you are a 
	// brush you should never enter the following section
	if (m_bFreeAttrMapOnDestruct)
	{
		if (m_pAppliedAttrs	!= NULL)
		{
			delete m_pAppliedAttrs;
			m_pAppliedAttrs = NULL;
		}
	}

	if (m_pBlendNode)
	{
		m_pBlendNode->CascadeDelete();
		delete m_pBlendNode;
	}
	
	if (m_bDeleteAll)
	{
		if (m_pPath != NULL)
			delete m_pPath;
	}
	
}

/***********************************************************************************************

> BOOL BlendPath::Initialise(	NodeRenderableInk* pThisBlendNode,
								INT32 Index,
								NodeRenderableInk* pThisCreatedByNode,
								UINT32 ThisSubPathID,
								CCAttrMap* pAttrMap)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/10/94
	Inputs:		pThisBlendNode		= ptr to the node that this BlendPath will use (should NOT be linked in the tree)
				Index			   	= index to path element the path should start on (-1 means bottom left most el)
				pThisCreatedByNode 	= ptr to the node that created pThisNodePath
				ThisSubPathID		= ID for this subpath (all blend paths generated from the same path have the
									  same subpath ID). Must be >=1
				ThisSubPathNum		= The sub path number specifying which sub path of the original path this
									  represents.  The first sub path is numbered 0.
				pAttrMap			= ptr to applied attrs (or NULL if to use attrs applied to pThisCreatedByNode)
	Outputs:	-
	Returns:	TRUE if Successful, FALSE otherwise
	Purpose:	Inits the BlendPath object to use the given NodePath.
				NOTE: pThisNodePath should point to isolated instance, and not to a used NodePath linked into
					  the tree! 
				NOTE: Once you give a BlendPath a NodePath, this becomes property of the BlendPath
					  and can be altered and/or deleted by the BlendPath at any time.
					  DO NOT KEEP A COPY OF THE PTR pThisNodePath!!! If you want a copy of the
					  NodePath at pThisNodePath, make a new copy of the node.

***********************************************************************************************/

BOOL BlendPath::Initialise(	NodeRenderableInk* pThisBlendNode,
							INT32 Index,
							NodeRenderableInk* pThisCreatedByNode,
							UINT32 ThisSubPathID,
							UINT32 ThisSubPathNum,
							CCAttrMap* pAttrMap, 
							BOOL CheckDirection)
{
	if (pThisBlendNode == NULL || pThisCreatedByNode == NULL) return FALSE;

	if (pThisBlendNode->IsNodePath())
		ERROR3IF(ThisSubPathID == 0,"Invalid subpath ID");

	DELPTR(m_pBlendNode);

	m_pBlendNode    = pThisBlendNode;

	if (m_pBlendNode->IsNodePath())
	{
		m_pPath 		= &(((NodePath *)m_pBlendNode)->InkPath);
	
	}
	else
	{
		// get the node to blend out of this path node & use this
		if (pThisBlendNode->IsCompound())
		{
			NodeRenderableInk * pInk = ((NodeCompound *)pThisBlendNode)->GetNodeToBlend();

			if (pInk)
			{
				if (pInk->IsNodePath())
				{
					m_pPath			= &(((NodePath *)pInk)->InkPath);
				}
				else
				{
					ERROR3("Node to blend isn't a node path");
				}
			}
			else
			{
				ERROR3("No node to blend");
			}
		}
	}

	m_pCreatedByNode= pThisCreatedByNode;
	m_SubPathID		= ThisSubPathID;
	m_SubPathNum	= ThisSubPathNum;
	m_pAppliedAttrs	= pAttrMap;
	m_UniqueAttrs	= (pAttrMap != NULL);

	if (m_pPath)
	{
		m_NumElements	= m_pPath->GetNumElements();
		m_PathType 		= m_pPath->GetPathType();
	}
	else
	{
		m_NumElements = 0;
		m_PathType = PATHTYPE_SHAPE;
	}

	// If we don't have an start element index (i.e. == -1) and its a shape (not a line)
	// then pick the bottom left element
	if (m_pPath && CheckDirection)
	{
		
		if (Index < 0 && m_PathType == PATHTYPE_SHAPE) Index = FindBottomLeft();
		
		ProcessVerbs();
		MapPath(Index);
		CorrectDirection(Index);
			
	}
		
	// record the initial CMapPtrToPtr::iterator (used for loading and saving brushes so that we don't get
	// the control coordinates of fill attributes wrong)
	DocRect BRect;

	if (m_pPath)
	{
		BRect = m_pPath->GetBoundingRect();
	}
	else
	{
		BRect = m_pBlendNode->GetBoundingRect(FALSE, FALSE);
	}

	m_BrushStartCoord = BRect.Centre();

	return TRUE;
}

/***********************************************************************************************

> BOOL BlendPath::Initialise( BlendPath *pBlendPath, 
							NodeRenderableInk * pNodeToBlend)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/3/2000
	Inputs:		pBlendPath - the blend path to copy from
	Outputs:	-
	Returns:	TRUE if Successful, FALSE otherwise
	Purpose:	Inits this blend path, replacing the blend node with the one given

***********************************************************************************************/

BOOL BlendPath::Initialise(	BlendPath *pBlendPath, 
							NodeRenderableInk * pNodeToBlend)
{
	m_pPath				= pBlendPath->GetPath();

	pNodeToBlend->NodeCopy((Node **)(&m_pBlendNode));
	m_pCreatedByNode	= pBlendPath->GetCreatedByNode();
	m_OrigMapping		= pBlendPath->GetOrigMapping();
	m_PathType			= pBlendPath->GetPathType();
	m_SubPathID			= pBlendPath->GetSubPathID();
	m_SubPathNum		= pBlendPath->GetSubPathNum();
	m_NumElements		= pBlendPath->GetNumElements();
	m_UniqueAttrs		= pBlendPath->UniqueAppliedAttrs();
	m_DirChanged		= pBlendPath->m_DirChanged;
	m_BrushStartCoord	= pBlendPath->m_BrushStartCoord;
	m_BrushCurrentCoord = pBlendPath->m_BrushCurrentCoord;

	if (m_pAppliedAttrs)
		m_pAppliedAttrs		= pBlendPath->m_pAppliedAttrs->Copy();
	else
		m_pAppliedAttrs		= NULL;

	return TRUE;
}

/********************************************************************************************

>	void BlendPath::CorrectDirection(INT32 Index)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/11/94
	Inputs:		Index = index of the node the blend was attempted from
	Outputs:	pPath potentially reversed so it is rendered in the correct direction
	Returns:	-
	Purpose:	This ensures that the path is going in the correct direction.
				It doesn't matter which direction, as long as the direction is the same for all blend paths.
				
********************************************************************************************/

void BlendPath::CorrectDirection(INT32 Index)
{
	switch (m_PathType)
	{
		case PATHTYPE_SHAPE:
			{
				// Dummy vars for the GWinding() call
				UINT32 l; POINT p;

				// Get the direction of the path
				BOOL Direction = GWinding((PPOINT)m_pPath->GetCoordArray(),m_pPath->GetVerbArray(),m_pPath->GetNumCoords(),FLATNESS,l,p);

				// If clockwise, reverse it so it's anticlockwise (this is ArtWorks compatible!!)
				if (Direction)
					m_pPath->Reverse();

				m_DirChanged = Direction;
			}
			break;

		case PATHTYPE_LINE:
			{
				// Get num coords
				INT32 NumCoords = m_pPath->GetNumCoords();

				if (Index != 0 && Index != (NumCoords-1))
				{
					// Get the coords array of the line
					DocCoord* pCoords = m_pPath->GetCoordArray();

					// Find the coords of the two end points of the line
					DocCoord Coord1 = pCoords[0];
					DocCoord Coord2 = pCoords[NumCoords-1];

					// if the start of the line is less bottom-left than the end, reverse the line.
					if ((Coord1.x + Coord1.y) > (Coord2.x + Coord2.y))
					{
						m_pPath->Reverse();
						m_OrigMapping = NumCoords-1; // Pretend that user mapped to end of line
					}
				}
			}
			break;

		default : ERROR3("Unknown path type"); break;
	}
}

/********************************************************************************************

>	INT32 BlendPath::MapPath(INT32 Index)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/11/94
	Inputs:		Index = index of element you want the path to start from
	Outputs:	pPath potentially rotated so it's first el is the one that was indexed by Index
				OrigMapping will be the mapping that should be applied to the original path
				to get the same effect.
	Returns:	The mapping to apply to invert this mapping
	Purpose:	This will ensure that the path's first element is the element at Index.
				It does this by rotating the path until element at index 'Index' is at index 0.
				The value returned can be used to undo the mapping, i.e. to apply an inverse mapping
				
********************************************************************************************/

INT32 BlendPath::MapPath(INT32 Index)
{
	INT32 NumCoords = m_pPath->GetNumCoords();

	INT32 InverseMapping = 0;

	// Make sure Index is in range
	if (Index > 0 && Index < NumCoords)
	{
		// Check that we're trying to remap an end point
		PathFlags* pFlags  = m_pPath->GetFlagArray();
		PathVerb*  pVerbs  = m_pPath->GetVerbArray();
		DocCoord*  pCoords = m_pPath->GetCoordArray();

		ERROR3IF(!pFlags[Index].IsEndPoint,"Index not on end point");
		if (!pFlags[Index].IsEndPoint)
		{
			// If index is not on an end point, choose the index of the end point that
			// appears before this index.
			INT32 OldIndex = Index;
			INT32 PrevIndex = 0;
			while (PrevIndex < OldIndex)
			{
				Index = PrevIndex;
				if (!m_pPath->FindNextEndPoint(&PrevIndex))
				{
					Index = 0;
					break;
				}
			}
		}

		switch (m_PathType)
		{
			case PATHTYPE_SHAPE:

				if (m_DirChanged)
				{
					DocCoord Pos = pCoords[Index];
					m_pPath->Reverse();
					m_pPath->FindNearestPoint(Pos,POINTFLAG_ENDPOINTS,&Index);
				}

				// Change the path so the el at index 'Index' is now at index 0
				m_pPath->ChangeStartElement(Index);

				// The above call will introduce a PT_CLOSEFIGURE flag into the end verb, so
				// we need to clear this flag from the last verb
				pVerbs[NumCoords-1] &= ~PT_CLOSEFIGURE;

				// Work out a mapping that can be applied to the original path in order to get the same effect
				// This is used when the blender reinitialises after its been deinitialised
				m_OrigMapping += Index;

				if (m_OrigMapping >= (NumCoords-1))
					m_OrigMapping = (m_OrigMapping+1) % NumCoords;

				InverseMapping = NumCoords-Index-1;

				if (m_DirChanged) 
				{
					DocCoord Pos = pCoords[InverseMapping];
					m_pPath->Reverse();
					m_pPath->FindNearestPoint(Pos,POINTFLAG_ENDPOINTS,&InverseMapping);
				}
				break;

			case PATHTYPE_LINE:
				// If it's a line then we will only try and remap if the index is the end element
				// In this case, this just means reversing the path
				if (Index == (NumCoords-1))
				{
					// Make the end of the line the start of the line
					m_pPath->Reverse();

					// The mapping to apply to the orig path to get the same effect if the opposite end point index
					if (m_OrigMapping == 0)
						m_OrigMapping = NumCoords-1;
					else
						m_OrigMapping = 0;

					InverseMapping = NumCoords-1;
				}
				break;

			default : ERROR3("Unknown path type"); break;
		}
	}

	return InverseMapping;
}


/********************************************************************************************

>	DocCoord BlendPath::MapPath(DocCoord Pos)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/2/95
	Inputs:		Pos = coord of element you want the path to start from
	Outputs:	pPath potentially rotated so it's first el is the one that was closest to 'Pos'
				OrigMapping will be the mapping that should be applied to the original path
				to get the same effect.
	Returns:	The mapping to apply to invert this mapping
	Purpose:	This will ensure that the path's first element is the element closest to Pos.
				It does this by rotating the path until element at Pos is at index 0.
				The value returned can be used to undo the mapping, i.e. to apply an inverse mapping
				
********************************************************************************************/

DocCoord BlendPath::MapPath(DocCoord Pos)
{
	DocCoord InversePos(0,0); 

	ERROR2IF(m_pPath == NULL,InversePos,"m_pPath == NULL");
	DocCoord* pCoords = m_pPath->GetCoordArray();
	ERROR2IF(pCoords == NULL,InversePos,"pCoords == NULL");

	// The inverse of this op is to remap the element currently at index 0
	InversePos = pCoords[0];

	INT32 Index = 0;
	INT32 InvIndex = 0;

/*	{
		StringBase Str;
		if (Str.Alloc(50000))
		{
			Str = "";
			pNodePath->GetDebugDetails(&Str);
			TRACE((TCHAR*)Str);
		}
	}
*/
	
	if (m_pPath->FindNearestPoint(Pos,POINTFLAG_ENDPOINTS,&Index))
		InvIndex = MapPath(Index);
	
	InversePos = pCoords[InvIndex];

/*	{
		StringBase Str;
		if (Str.Alloc(50000))
		{
			Str = "";
			pNodePath->GetDebugDetails(&Str);
			TRACE((TCHAR*)Str);
		}
	}
  */
	return InversePos;
}


/********************************************************************************************

>	INT32 BlendPath::FindBottomLeft()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/11/94
	Inputs:		-
	Outputs:	-
	Returns:	The index of the bottom left most element (or -1 if something when wrong)
	Purpose:	This scans the path for the bottom left most element, and returns the index to it
				
********************************************************************************************/

INT32 BlendPath::FindBottomLeft()
{
	DocCoord* pCoords = m_pPath->GetCoordArray();
	if (pCoords == NULL) return -1;

	INT32 BottomLeftIndex = -1;
	INT32 Index;
	BOOL ok;

	DocCoord VeryMostBottomLeft(0x7fffffff,0x7fffffff);
	double   SmallestDist = 1.0e100,ThisDist;

	for (Index=0,ok=TRUE;ok;)
	{
		if (pCoords[Index].x < VeryMostBottomLeft.x) VeryMostBottomLeft.x = pCoords[Index].x;
		if (pCoords[Index].y < VeryMostBottomLeft.y) VeryMostBottomLeft.y = pCoords[Index].y;
		ok = m_pPath->FindNextEndPoint(&Index);
	}

	for (Index=0,ok=TRUE;ok;)
	{
		ThisDist = VeryMostBottomLeft.Distance(pCoords[Index]);
		if (ThisDist < SmallestDist)
		{
			BottomLeftIndex = Index;
			SmallestDist = ThisDist;
		}
		ok = m_pPath->FindNextEndPoint(&Index);
	}

//	return BottomLeftIndex;

	UINT32 u; POINT p;

	// Get the direction of the path
	GWinding((PPOINT)m_pPath->GetCoordArray(),m_pPath->GetVerbArray(),m_pPath->GetNumCoords(),FLATNESS,u,p);

//	TRACEUSER( "Markn", _T("\n"));

	INT32 GIndex=-1;
	DocCoord Coord(p.x,p.y);
	SmallestDist = 1.0e100;
	for (Index=0,ok=TRUE;ok;)
	{
		ThisDist = Coord.Distance(pCoords[Index]);
		if ((ThisDist-SmallestDist) < 5.0)
		{
			// This point is close enough, so remember it's index
			GIndex = Index;

			// Only update the smallest dist if it is indeed the smallest so far
			if (ThisDist < SmallestDist)
				SmallestDist = ThisDist;
		}
		//TRACEUSER( "Markn", _T("(%d) %f - "),Index,ThisDist);
		ok = m_pPath->FindNextEndPoint(&Index);
	}

//	TRACEUSER( "Markn", _T("\n"));
//	TRACEUSER( "Markn", _T("GIndex          = %ld\n"),GIndex);
//	TRACEUSER( "Markn", _T("BottomLeftIndex = %ld\n"),BottomLeftIndex);
//	if (GIndex != BottomLeftIndex)
//	{
//		TRACEUSER( "Markn", _T("*^*^*^*^*^*^*^*^*^*^*^*^*^*^"));
//	}

//	ERROR3IF_PF(GIndex != BottomLeftIndex,("GIndex != BottomLeftIndex: GIndex = %ld, BottomLeftIndex = %ld",GIndex,BottomLeftIndex));

	return GIndex;
//	return BottomLeftIndex;
}

/********************************************************************************************

>	void BlendPath::ProcessVerbs()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/11/94
	Inputs:		-
	Outputs:	Contents of pPath's verb array potentially changed
				CloseFigure = PT_CLOSEFIGURE if one or more PT_CLOSEFIGURE flags are found in the verbs
							  0 otherwise
	Returns:	-
	Purpose:	This processes the verbs so the GBlend can cope with them.
				It clears any PT_CLOSEFIGURE flags it finds.
				If it finds one or more PT_CLOSEFIGURE, CloseFigure = PT_CLOSEFIGURE
				
********************************************************************************************/

void BlendPath::ProcessVerbs()
{
	PathVerb* pPathVerb = m_pPath->GetVerbArray();
	INT32 n = m_pPath->GetNumCoords()-1;

	m_CloseFigure = 0;

	for (;n>=0;n--)
	{
		// Get the next verb
		PathVerb Verb = pPathVerb[n];

		// Have we found a close figure flag yet?
		if (m_CloseFigure == 0) m_CloseFigure = Verb & PT_CLOSEFIGURE;

		// Remove any wierd flags from the verb, as GBlend doesn't like them.
		pPathVerb[n] = Verb & ~PT_CLOSEFIGURE;
	}
}
	


/********************************************************************************************

>	void BlendPath::Transform( TransformBase& Trans )

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/10/94
	Inputs:		Trans - The transform Object
	Purpose:	Transforms the NodePath attached to this blend path, plus any applied attrs it may have

********************************************************************************************/

void BlendPath::Transform( TransformBase& Trans )
{
	if (m_pBlendNode != NULL) m_pBlendNode->Transform(Trans);

	if (m_UniqueAttrs && m_pAppliedAttrs != NULL)
	{
		CCRuntimeClass *pType;
		void		   *pVal;

		// iterating all (key, value) pairs
		for( CMapPtrToPtr::iterator Pos = m_pAppliedAttrs->GetStartPosition(); 
			Pos != m_pAppliedAttrs->GetEndPosition();)
		{
			// Get attr at CMapPtrToPtr::iterator Pos
			m_pAppliedAttrs->GetNextAssoc(Pos,pType,pVal);

			// pVal is actually an attribute, so get a ptr to it and render it
			NodeAttribute* pNodeAttr = (NodeAttribute *)pVal;

			pNodeAttr->Transform(Trans);
		}
	}

}



/********************************************************************************************

>	void BlendPath::Transform( Matrix* pTransMatrix,BOOL TransformLines, BOOL Tile)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/1/2000
	Inputs:		TransMatrix - the matrix describing the transformation
				TransformLineWidth - do we transform the line width
				Tile - flag to say whether to tile or not. If set this means that attributes such as 
				bitmap fills etc. are only rendered once
	Purpose:	Transforms the NodePath attached to this blend path, plus any applied attrs it may have
				Plus it also records transforms in the member variable, so that later they may be undone
				This is specifically required by brush attributes.

********************************************************************************************/

void BlendPath::Transform( Matrix* pTransMatrix, BOOL TransformLines,   BOOL Tile )
{
	Trans2DMatrix Trans(*pTransMatrix);	

	if (m_pBlendNode != NULL) 
		m_pBlendNode->Transform(Trans);
	else
	{
		// added this for the case where we create the blendpath manually, i.e. when blending
		// brush attributes
		DocCoord* pCoords = m_pPath->GetCoordArray();
		INT32 NumCoords = m_pPath->GetNumCoords();
		if (pCoords != NULL && NumCoords > 0)
			Trans.Transform(pCoords, NumCoords);
		else
			ERROR3("No coordinates in BlendPath::Transform");
	}

	CCAttrMap* pAttrMap = FindAppliedAttributes();
	if (pAttrMap != NULL)
	{
		if (Tile)
			pAttrMap->TransformForBrush(Trans);
		else
			pAttrMap->Transform(Trans);
	}
	else
	{
		ERROR3("No applied attributes");
	}

	
	return;
}


/********************************************************************************************

>	BOOL BlendPath::TransformBrushAttributesBeforeSave()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/1/2000
	Inputs:		-
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	Undoes the transformations performed on the attributes of this blendpath.  
				This function must be called prior to saving a brush attribute. If you wish
				to keep using the attribute after the save call TransformBrushAttributesAfterSave
				or else your attributes will be all over the place.
********************************************************************************************/

BOOL BlendPath::TransformBrushAttributesBeforeSave()
{
	if (m_pPath == NULL)
	{
		ERROR2(FALSE, "This blendpath has no path");
		return FALSE;
	}
	DocRect BRect = m_pPath->GetBoundingRect();
	m_BrushCurrentCoord = BRect.Centre();

	Trans2DMatrix Trans(m_BrushStartCoord.x - m_BrushCurrentCoord.x, 
						m_BrushStartCoord.y - m_BrushCurrentCoord.y);

	CCAttrMap* pAttrMap = FindAppliedAttributes();

	if (pAttrMap == NULL)
	{
		ERROR3("No applied attributes");
		return FALSE;
	}
	
	// quick pre-release bodge fix - fractal fills seem to be getting wrongly transformed
	// when we do this, so exclude them
	AttrFractalFill* pFrac = NULL;
	if( pAttrMap->Lookup( CC_RUNTIME_CLASS(AttrFractalFill), (void*&)pFrac ) )
		pAttrMap->RemoveKey( CC_RUNTIME_CLASS(AttrFractalFill) );
	
	pAttrMap->Transform(Trans);

	return TRUE;
}


/********************************************************************************************

>	void BlendPath::TransformBrushAttributesAfterSave()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/1/2000
	Inputs:		-
	Returns:	TRUE if successful , FALSE otherwise
	Purpose:	Undoes the transformations performed on the attributes by the previous function.								
********************************************************************************************/

BOOL BlendPath::TransformBrushAttributesAfterSave()
{
	Trans2DMatrix Trans(m_BrushCurrentCoord.x - m_BrushStartCoord.x, 
						m_BrushCurrentCoord.y - m_BrushStartCoord.y);

	CCAttrMap* pAttrMap = FindAppliedAttributes();

	if (pAttrMap == NULL)
	{
		ERROR3("No applied attributes");
		return FALSE;
	}

	pAttrMap->TransformForBrush(Trans);

	return TRUE;
}

/********************************************************************************************

>	BOOL BlendPath::IsFilled()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/11/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the associated path is filled
	Purpose:	Find out if the path is filled

********************************************************************************************/

BOOL BlendPath::IsFilled()
{
	if (m_pPath != NULL)
		return (m_pPath->IsFilled);
	else
		return FALSE;
}

/********************************************************************************************

>	BOOL BlendPath::IsStroked()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/11/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the associated path is Stroked
	Purpose:	Find out if the path is Stroked

********************************************************************************************/

BOOL BlendPath::IsStroked()
{
	if (m_pPath != NULL)
		return (m_pPath->IsStroked);
	else
		return FALSE;
}

/********************************************************************************************

>	DocCoord BlendPath::GetPathCoord(INT32 Index)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/11/94
	Inputs:		Index = index into the path's coord array of coord required
						If index < 0 or index >= num coords in array, an value of 0 used
	Outputs:	-
	Returns:	The DocCoord at index Index
	Purpose:	Access to path's coord array

********************************************************************************************/

DocCoord BlendPath::GetPathCoord(INT32 Index)
{
	DocCoord Result(0,0);

	if (m_pPath != NULL)
	{
		if (m_DirChanged)
			m_pPath->Reverse();

		DocCoord* 	pCoords = m_pPath->GetCoordArray();
		INT32 		n		= m_pPath->GetNumCoords();
		if (Index >= n || Index < 0) Index = 0;

		if (Index < n && pCoords != NULL)
			Result = pCoords[Index];

		if (m_DirChanged)
			m_pPath->Reverse();
	}

	return Result;
}

/********************************************************************************************

>	void BlendPath::RenderBlendBlobs(RenderRegion* pRender,Trans2DMatrix* pMatrix)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/11/94
	Inputs:		pRender = region to render blobs into.
				pMatrix = matrix to apply to blob coords
	Outputs:	-
	Returns:	-
	Purpose:	Renders the blend blobs by asking the pNodePath's Path to render its path blobs

********************************************************************************************/

void BlendPath::RenderBlendBlobs(RenderRegion* pRender,Trans2DMatrix* pMatrix)
{
#if !defined(EXCLUDE_FROM_RALPH)
	if (m_pPath != NULL)
	{
//		PathVerb* pVerbs  = m_pPath->GetVerbArray();
		DocCoord* pCoords = m_pPath->GetCoordArray();
		INT32      n       = m_pPath->GetNumCoords();
		INT32	  Index	  = 0;

		pRender->SetLineColour(COLOUR_NONE);
		pRender->SetFillColour(COLOUR_UNSELECTEDBLOB);

		INT32 i=0;
		DocCoord* pCoordList = (DocCoord*) malloc(n*sizeof(DocCoord));

		if (pCoordList)
		{
			switch (m_PathType)
			{
				case PATHTYPE_SHAPE:
//					while (m_pPath->FindNextEndPoint(&Index))
//						pRender->DrawBlob(pCoords[Index], BT_UNSELECTED);
					while (m_pPath->FindNextEndPoint(&Index))
						pCoordList[i++] = pCoords[Index];
					break;

				case PATHTYPE_LINE:
//					pRender->DrawBlob(pCoords[0]  , BT_UNSELECTED);
//					pRender->DrawBlob(pCoords[n-1], BT_UNSELECTED);
					pCoordList[i++] = pCoords[0];
					pCoordList[i++] = pCoords[n-1];
					break;

				default:
					ERROR3("Unknown pathtype");
					break;
			}

			if (pMatrix && i > 0)
				pMatrix->Transform(pCoordList,i);

			for (;i>0;i--)
				pRender->DrawBlob(pCoordList[i-1], BT_UNSELECTED);
		}
		DELPTR(pCoordList);
	}
#endif
}

/********************************************************************************************

> BOOL BlendPath::IsPointOverBlob(DocCoord* pPointerPos,INT32* pIndex,Trans2DMatrix* pMatrix)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/11/94
	Inputs:		pPointerPos = point to check against
				pIndex	   = ptr to place to store index if match is found
				pMatrix		= matrix to apply to the path points before testing with pointer pos
	Outputs:	*pIndex, updated if TRUE returned, undefined otherwise
	Returns:	TRUE if match  found, FALSE otherwise
	Purpose:	This sees if the point given lies on a selected blob.
				If a match is found, the index to blob element is updated

				Markn 26-5-99:	Modified so that if the function returns TRUE, *pPointerPos is updated
								so that it contains the coords of the blob it is over
	SeeAlso:	-

********************************************************************************************/

BOOL BlendPath::IsPointOverBlob(DocCoord* pPointerPos,INT32* pIndex,Trans2DMatrix* pMatrix)
{
#if !defined(EXCLUDE_FROM_RALPH)
	BOOL Found = FALSE;

	if (m_pPath != NULL && pIndex != NULL)
	{
		Path* pPath = m_pPath;
		if (pMatrix)
		{
			pPath = new Path;
			if (pPath != NULL && pPath->Initialise())
			{
				pPath->MergeTwoPaths(*m_pPath);
				pMatrix->Transform((DocCoord*)(pPath->GetCoordArray()), pPath->GetNumCoords() );
			}
		}

		Found = pPath->FindNearestPoint(*pPointerPos,POINTFLAG_ENDPOINTS,pIndex);

		if (Found)
		{
			if (m_PathType == PATHTYPE_LINE)
				Found = ((*pIndex == 0) || (*pIndex == (pPath->GetNumCoords()-1)));
			else if (m_DirChanged)
			{
				pPath->Reverse();
				pPath->FindNearestPoint(*pPointerPos,POINTFLAG_ENDPOINTS,pIndex);
				pPath->Reverse();
			}

			if (Found)
			{
				*pPointerPos = GetPathCoord(*pIndex);
				if (pMatrix)
					pMatrix->Transform(pPointerPos,1);
			}
		}

		if (pMatrix) 
			DELPTR(pPath);
	}

	return Found;
#else
	return FALSE;
#endif
}

/********************************************************************************************

> CCAttrMap* BlendPath::FindAppliedAttributes()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/11/94
	Inputs:		-
	Outputs:	-
	Returns:	Ptr to the attributes applied to this blend path
				NULL if unable to get attributes
	Purpose:	This calls FindAppliedAttributes() on the node that created this blend path.
				It will only do this if this is the first call since the last FreeAppliedAttributes()
				or is the first ever call (in other words, it is cached).
	SeeAlso:	-

********************************************************************************************/

CCAttrMap* BlendPath::FindAppliedAttributes()
{
	if (m_pAppliedAttrs == NULL)
	{
		m_pAppliedAttrs = new CCAttrMap(30);
		if (m_pAppliedAttrs != NULL)
		{
			// Ilan 4/5/00
			// Ensure geometry linked attributes which are applied to the entire Blend compound
			// aren't found
			if (!m_pCreatedByNode->FindAppliedAttributes(m_pAppliedAttrs,5000,NULL,TRUE))
			{
				delete m_pAppliedAttrs;
				m_pAppliedAttrs = NULL;
			}
			AttrTranspFillGeometry* pTransp = NULL;
			m_pCreatedByNode->FindAppliedAttribute(CC_RUNTIME_CLASS(AttrTranspFillGeometry), (NodeAttribute**)&pTransp);
		}
	}

	return m_pAppliedAttrs;
}

/********************************************************************************************

> void BlendPath::FreeAppliedAttributes()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/11/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	This will free any memory used to store the applied attributes of this blend path
	SeeAlso:	-

********************************************************************************************/

void BlendPath::FreeAppliedAttributes()
{
	if (!m_UniqueAttrs && m_pAppliedAttrs != NULL)
	{
		delete m_pAppliedAttrs;
		m_pAppliedAttrs = NULL;
	}
}

/********************************************************************************************

>	BOOL BlendPath::MakeAttributeMap()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/9/2000
	Inputs:		-
	Outputs:	-
	Returns:	A copy of the applied attributes of this blendpath, or NULL if we fail

	Purpose:	To make a complete copy of the applied attribute map
	SeeAlso:	-

********************************************************************************************/

CCAttrMap* BlendPath::MakeAttributeMapCopy()
{
	// easiest thing to do is get the attribute pointers the normal way
	// and then copy them one by one
	CCAttrMap* pAttrMap = FindAppliedAttributes();

	if (pAttrMap == NULL)
	{
		ERROR3("Unable to find attributes in BlendPath::MakeAttributeMap");
		return NULL; 
	}

	// make a new map
	CCAttrMap* pCopyMap = new CCAttrMap(30);
	if (pCopyMap == NULL)
		return NULL;

	// now copy each one and set it back to the map
	CCRuntimeClass	   *pType;
	void			   *pVal;
	NodeAttribute* pNodeAttr = NULL;
	NodeAttribute* pAttrCopy = NULL;

	// iterating all (key, value) pairs
	for( CMapPtrToPtr::iterator Pos = pAttrMap->GetStartPosition(); 
		Pos != pAttrMap->GetEndPosition();)
	{
		// Get attr at CMapPtrToPtr::iterator Pos
		pAttrMap->GetNextAssoc(Pos,pType,pVal);

		// pVal is actually an attribute, so get a ptr to it and render it
		pNodeAttr = (NodeAttribute *)pVal;

		pAttrCopy = (NodeAttribute*)pNodeAttr->SimpleCopy();
		if (pAttrCopy == NULL)
		{
			ERROR3("Unable to make attribute copy in BlendPath::MakeAttributeMap");
			DeleteAttributeMapAndAttributes(pCopyMap);
			return NULL;
		}
		
		pCopyMap->SetAt( pAttrCopy->GetAttributeType(), (void *)pAttrCopy );
		pAttrCopy = NULL;
	}

	return pCopyMap;
}


/********************************************************************************************

>	CCAttrMap* BlendPath::UpdateAttributeMapCopy(CCAttrMap* pCopyMap)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/01/2004
	Inputs:		-
	Outputs:	-
	Returns:	A copy of the applied attributes of this blendpath, or NULL if we fail

	Purpose:	To make a complete copy of the applied attribute map
	SeeAlso:	-

********************************************************************************************/

CCAttrMap* BlendPath::UpdateAttributeMapCopy(CCAttrMap* pCopyMap)
{
	// easiest thing to do is get the attribute pointers the normal way
	// and then copy them one by one
	CCAttrMap* pAttrMap = FindAppliedAttributes();

	if (pAttrMap == NULL)
	{
		ERROR3("Unable to find attributes in BlendPath::UpdateAttributeMapCopy");
		return NULL; 
	}

	ERROR2IF(pCopyMap==NULL, NULL, "Unable to find attribute copies in BlendPath::UpdateAttributeMapCopy");

	// now copy the contents of attributes from the original map back into the copy map
	CCRuntimeClass	   *pType;
	void			   *pVal;
	void			   *pCopyVal;
	NodeAttribute	   *pNodeAttr = NULL;
	NodeAttribute	   *pAttrCopy = NULL;

	// iterating all (key, value) pairs
	for( CMapPtrToPtr::iterator Pos = pAttrMap->GetStartPosition(); 
		Pos != pAttrMap->GetEndPosition();)
	{
		// Get attr at CMapPtrToPtr::iterator Pos
		pAttrMap->GetNextAssoc(Pos, pType, pVal);

		pNodeAttr = (NodeAttribute*)pVal;

		BOOL bFound = pCopyMap->Lookup( pType, pCopyVal );
		if (bFound)
		{
			pAttrCopy = (NodeAttribute*)pCopyVal;
			pNodeAttr->PolyCopyNodeContents(pAttrCopy);
		}
	}

	return pCopyMap;
}


/********************************************************************************************

>	void BlendPath::DeleteAttributeMapAndAttributes()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/9/2000
	Inputs:		-
	Outputs:	-
	Returns:	-

	Purpose:	Counterpart to MakeAttributeMap, deletes all the copied attributes and then the
				map.
	SeeAlso:	-

********************************************************************************************/

void BlendPath::DeleteAttributeMapAndAttributes(CCAttrMap* pMap)
{

	if (pMap != NULL)
	{
		pMap->DeleteAttributes();

		delete pMap;
		pMap = NULL;
	}

}

/********************************************************************************************

> void BlendPath::SetFreeAttributeFlag(BOOL Value)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/5/2000
	Inputs:		Value to set the flag to
	Outputs:	-
	Returns:	-
	Purpose:	Sets the flag which indicates whether or not we definitely want to delete
				the attribute map upon destruction of this blendpath.  This was designed
				pretty specifically for use with the Brush code so if you want to use it for
				other purposes you should be careful.
	SeeAlso:	-

********************************************************************************************/

void BlendPath::SetFreeAttributeFlag(BOOL Value)
{
	m_bFreeAttrMapOnDestruct = TRUE;
}

/********************************************************************************************

>	void BlendPath::SetAppliedAttributes(CCAttrMap* pMap)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/3/2000
	Inputs:		pointer to an attribute map to set for
	Outputs:	-
	Returns:	-
	Purpose:	Designed specifically for use when blending brush attributes, which need to 
				generate blendpaths and attribute maps by hand,so to speak.  This function
				allows you to set an external attribute map for this blendpath.
	SeeAlso:	-

********************************************************************************************/

void BlendPath::SetAppliedAttributes(CCAttrMap* pMap)
{
	if (pMap == NULL)
		return;
	
	// get rid of any existing attributes
	FreeAppliedAttributes();
	m_pAppliedAttrs = pMap;
}


/********************************************************************************************

>	void BlendPath::SetPath(Path* pPath)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/3/2000
	Inputs:		pointer to a path to set 
	Outputs:	-
	Returns:	-
	Purpose:	Designed specifically for use when blending brush attributes, which need to 
				generate blendpaths and attribute maps by hand,so to speak.  This function
				allows you to set an external path for this blendpath.
	SeeAlso:	-

********************************************************************************************/

void BlendPath::SetPath(Path* pPath)
{
	if (pPath != NULL)
	{
		m_pPath = pPath;
		m_NumElements	= m_pPath->GetNumElements();
		m_PathType 		= m_pPath->GetPathType();
		m_SubPathNum	= m_pPath->GetNumSubpaths();
	}
}


/********************************************************************************************

>	void BlendPath::DeleteAttributesAndPath()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/3/2000
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Sets m_SubPathID to zero thus ensuring that all the attributes and the attribute
				map are deleted.  We need to do this for blendpaths created by blending brush
				attributes as mentioned in the previous two functions.  I don't recommend use
				of this function under normal circumstances.
	SeeAlso:	-

********************************************************************************************/

void BlendPath::DeleteAttributesAndPath()
{
	m_bDeleteAll = TRUE;
}


/********************************************************************************************

> BitmapFillAttribute* BlendPath::GetAppliedBitmapFill()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/11/94
	Inputs:		-
	Outputs:	-
	Returns:	the bitmap colour fill attribute applying to this blendpath, if there is one
	Purpose:	as above
	SeeAlso:	-

********************************************************************************************/

AttrBitmapFill* BlendPath::GetAppliedBitmapColourFill()
{
	// Get our applied attribute map
	CCAttrMap* pAttrMap = FindAppliedAttributes();
	if (pAttrMap == NULL)
		return NULL;

	// loop through the map until we find an bitmap fill
	CCRuntimeClass	   *pType;
	void			   *pVal;
	NodeAttribute* pNodeAttr = NULL;
	// iterating all (key, value) pairs
	for( CMapPtrToPtr::iterator Pos = pAttrMap->GetStartPosition(); 
		Pos != pAttrMap->GetEndPosition();)
	{
		// Get attr at CMapPtrToPtr::iterator Pos
		pAttrMap->GetNextAssoc(Pos,pType,pVal);

		// pVal is actually an attribute, so get a ptr to it 
		pNodeAttr = (NodeAttribute *)pVal;
	
		if (pNodeAttr->IsABitmapFill() &&!pNodeAttr->IsATranspFill())
			break;

		pNodeAttr = NULL;
	}

	// first get the attribute value from the attribute node
	if (pNodeAttr != NULL)
	{
		return (AttrBitmapFill*)pNodeAttr;
	}

	return NULL;
		
}	


/********************************************************************************************

> BitmapFillAttribute* BlendPath::GetAppliedBitmapTranspFill()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/11/94
	Inputs:		-
	Outputs:	-
	Returns:	the bitmapfill transparent attribute applying to this blendpath, if there is one
	Purpose:	as above
	SeeAlso:	-

********************************************************************************************/

AttrBitmapFill* BlendPath::GetAppliedBitmapTranspFill()
{
	// Get our applied attribute map
	CCAttrMap* pAttrMap = FindAppliedAttributes();
	if (pAttrMap == NULL)
		return NULL;

	// loop through the map until we find an bitmap fill
	CCRuntimeClass	   *pType;
	void			   *pVal;
	NodeAttribute* pNodeAttr = NULL;
	// iterating all (key, value) pairs
	for( CMapPtrToPtr::iterator Pos = pAttrMap->GetStartPosition(); 
		Pos != pAttrMap->GetEndPosition();)
	{
		// Get attr at CMapPtrToPtr::iterator Pos
		pAttrMap->GetNextAssoc(Pos,pType,pVal);

		// pVal is actually an attribute, so get a ptr to it 
		pNodeAttr = (NodeAttribute *)pVal;
	
		if (pNodeAttr->IsABitmapFill() &&pNodeAttr->IsATranspFill())
			break;

		pNodeAttr = NULL;
	}

	// first get the attribute value from the attribute node
	if (pNodeAttr != NULL)
	{
		return (AttrBitmapFill*)pNodeAttr;
	}

	return NULL;
		
}	



/********************************************************************************************

>	void BlendPath::DeleteAttributesAndPath()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/3/2000
	Inputs:		-
	Outputs:	-
	Returns:	the applied linewidth, or -1 indicating that no line width was found
	Purpose:	Returns the cached line width applying to this blendpath
	SeeAlso:	-

********************************************************************************************/

MILLIPOINT BlendPath::GetAppliedLineWidth()
{
	// find the line width attribute
	NodeAttribute* pAttr = NULL;
	m_pCreatedByNode->FindAppliedAttribute(CC_RUNTIME_CLASS(AttrLineWidth), &pAttr);
	if (pAttr != NULL)
		m_AppliedLineWidth = ((AttrLineWidth*)pAttr)->Value.LineWidth;	
	
	return m_AppliedLineWidth;
}


/********************************************************************************************

>	BOOL BlendPath::MakeCopyPathAndAttributes()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/3/2000
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	Because of all the imprecision problems we've been having I've decided it is
				best if we never ever access the orignal blendpath path and attribute data for 
				rendering. Instead what we will do is generate copies of this data and supply it
				to anyone who wants to render with this brush.  The copies will then be destroyed
				after we have finished rendering.

				This function makes copies of the paths and attributes and and assigns them to
				our member pointers.
	SeeAlso:	-

********************************************************************************************/

BOOL BlendPath::MakeCopyPathAndAttributes()
{
	// if we've no path then fail
	ERROR2IF(m_pPath == NULL, FALSE, "Path is NULL in BlendPath::MakeCopyPathAndAttributes");

	// likewise we need the attributes
	CCAttrMap* pAttrMap = FindAppliedAttributes();
	
	ERROR2IF(pAttrMap == NULL, FALSE, "Unable to find attributes in BlendPath::MakeCopyPathAndAttributes");

	// if we've already got them then get rid of the existing ones
	if (m_pCopyPath != NULL || m_pCopyAttrs != NULL)
		DeleteCopyPathAndAttributes();

	// first copy the path
	m_pCopyPath = new Path;

	if (m_pCopyPath == NULL)
		return FALSE;

	if (!m_pCopyPath->Initialise(m_pPath->GetNumCoords()))
	{
		delete m_pCopyPath;
		m_pCopyPath = NULL;
		return FALSE;
	}
	if (!m_pCopyPath->CopyPathDataFrom(m_pPath))
	{
		delete m_pCopyPath;
		m_pCopyPath = NULL;
		return FALSE;
	}

	// get a copy of our attributes
	m_pCopyAttrs = MakeAttributeMapCopy();

	if (m_pCopyAttrs == NULL)
	{
		delete m_pCopyPath;
		m_pCopyPath = NULL;
		return FALSE;
	}
	return TRUE;
}


/********************************************************************************************

>	BOOL BlendPath::UpdateCopyPathAndAttributes()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/3/2000
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	Because of all the imprecision problems we've been having I've decided it is
				best if we never ever access the orignal blendpath path and attribute data for 
				rendering. Instead what we will do is generate copies of this data and supply it
				to anyone who wants to render with this brush.  The copies will then be destroyed
				after we have finished rendering.

				This function makes copies of the paths and attributes and and assigns them to
				our member pointers.
	SeeAlso:	-

********************************************************************************************/

BOOL BlendPath::UpdateCopyPathAndAttributes()
{
	// if we've no path then fail
	ERROR2IF(m_pPath == NULL, FALSE, "Path is NULL in BlendPath::UpdateCopyPathAndAttributes");
	ERROR2IF(m_pCopyPath == NULL, FALSE, "CopyPath is NULL in BlendPath::UpdateCopyPathAndAttributes");

	// likewise we need the attributes
	ERROR2IF(m_pAppliedAttrs==NULL, FALSE, "No applied attrs in BlendPath::UpdateCopyPathAndAttributes");
	
	ERROR2IF(m_pPath->GetNumCoords()!=m_pCopyPath->GetNumCoords(), FALSE, "Mismatching paths in BlendPath::UpdateCopyPathAndAttributes");

	if (!m_pCopyPath->Initialise(m_pPath->GetNumCoords()))
	{
		delete m_pCopyPath;
		m_pCopyPath = NULL;
		return FALSE;
	}
	if (!m_pCopyPath->CopyPathDataFrom(m_pPath))
	{
		delete m_pCopyPath;
		m_pCopyPath = NULL;
		return FALSE;
	}

	// get a copy of our attributes
	m_pCopyAttrs = UpdateAttributeMapCopy(m_pCopyAttrs);

	if (m_pCopyAttrs == NULL)
	{
		delete m_pCopyPath;
		m_pCopyPath = NULL;
		return FALSE;
	}
	return TRUE;
}


/********************************************************************************************

>	void BlendPath::DeleteCopyPathAndAttributes()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/3/2000
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Deletes the copies we made of the paths and attributes
	SeeAlso:	-

********************************************************************************************/

void BlendPath::DeleteCopyPathAndAttributes()
{
	if (m_pCopyPath != NULL)
	{
		delete m_pCopyPath;
		m_pCopyPath = NULL;
	}
	if (m_pCopyAttrs != NULL)
	{
		DeleteAttributeMapAndAttributes(m_pCopyAttrs);
		m_pCopyAttrs = NULL;
	}
}


/********************************************************************************************

>	Path* BlendPath::GetCopyPath()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/3/2000
	Inputs:		-
	Outputs:	-
	Returns:	the copy we made of our path
	Purpose:	access
	SeeAlso:	-

********************************************************************************************/

Path* BlendPath::GetCopyPath()
{
	return m_pCopyPath;
}


/********************************************************************************************

>	Path* BlendPath::GetCopyPath()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/3/2000
	Inputs:		-
	Outputs:	-
	Returns:	the copy we made of our attribute map
	Purpose:	access
	SeeAlso:	-

********************************************************************************************/

CCAttrMap* BlendPath::GetCopyAttributes()
{
	return m_pCopyAttrs;
}

//-------------------------------------------
//-------------------------------------------
//-------------------------------------------
//-------------------------------------------

BecomeA::BecomeA(BecomeAReason ThisReason,
			CCRuntimeClass* pClass,
			UndoableOperation* pOp /* = NULL */,
			BOOL sel /*= TRUE*/,
			BOOL First /*= FALSE*/
			) : Reason(ThisReason), pClassToBecome(pClass), pUndoOp(pOp)
			{	Select = sel;
				pCallNode = NULL;
				AmFirstNode = First;
				fSilhouette = FALSE;
				fShadowSilhouettes = FALSE;
				insertComplexBlendStepsAsPaths = FALSE;
				m_Count = 0;
				m_bIsCounting = FALSE;
				m_bBecomeANodePath = (pClassToBecome==CC_RUNTIME_CLASS(NodePath));
				m_bInPlace = FALSE;
				m_bSecondary = FALSE;
				m_bPathsOnly = FALSE;
			}

BOOL BecomeA::BAPath() const {return m_bBecomeANodePath;}

// This function should be called when Reason == BECOMEA_PASSBACK 
// (see above for a description of this reason code)
BOOL BecomeA::PassBack(NodeRenderableInk* pNewNode,NodeRenderableInk* pCreatedByNode,CCAttrMap* pAttrMap /*=NULL*/)
{
	if (pAttrMap)
	{
		pAttrMap->DeleteAttributes();
		delete pAttrMap;
	}
	return FALSE;
}


/***********************************************************************************************

> NodeRenderableInk * BlendBecomeA::CreateWrapNode(NodeRenderableInk * pNode, 
														NodeCompound *pTree,
														CCAttrMap * pMap)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/3/2000
	Inputs:		pNode	-	the node to wrap up
				pTree	-	the root of the tree to use to wrap the node
				pAttrMap	-	the attribute map to use in the wrapping
	Outputs:	-
	Returns:	TRUE if Successful, FALSE otherwise
	Pupose:		Recursively wraps up the given node using the tree specified
	
***********************************************************************************************/
NodeRenderableInk * BlendBecomeA::CreateWrapNode(NodeRenderableInk * pNode, NodeCompound *pTree,
														CCAttrMap * pMap)
{
	NodeCompound * pCompound = (NodeCompound *)pTree->FindFirstChild(CC_RUNTIME_CLASS(NodeCompound));

	if (pCompound)
	{
		// create a wrapped node for the compound node
		pNode = CreateWrapNode(pNode, pCompound, pMap);
	}

	// create the wrapped node from the given compound node
	return pTree->CreateTreeFromNodeToBlend(pNode, pMap);
}

/***********************************************************************************************

> BOOL BlendBecomeA::PassBack(NodeRenderableInk* pNewNode,NodeRenderableInk* pCreatedByNode,CCAttrMap* pAttrMap)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/10/94
	Inputs:		pNewNode   	   	= ptr to Node that been generated by a node via its DoBecomeA() method
				pCreatedByNode 	= ptr to the node that created pNewNode
				pAttrMap		= ptr to attr map (or NULL if to use applied attrs of pCreatedByNode)
	Outputs:	-
	Returns:	TRUE if Successful, FALSE otherwise
	Purpose:	This is called by each node that is blended. When the blend tool asks an object to turn itself
				into loads of paths, it gives the object a BlendBecomeA ptr. The object calls the PassBack()
				method of BlendBecomeA (i.e. this routine) for each path that is generated.
				This generates a BlendPath for each sub-path in the given NodePath, and adds it to a blend 
				reference object.

				NOTE: pNewNode should point to isolated instance, and not to a used NodePath linked into
					  the tree! 

				NOTE: Once you give a BlendPath a NodePath, this becomes property of the BlendPath
					  and can be altered and/or deleted by the BlendPath at any time.
					  DO NOT KEEP A COPY OF THE PTR pThisNodePath!!! If you want a copy of the
					  NodePath at pThisNodePath, make a new copy of the node.

				NOTE: If it fails, pNewNode will have to be deleted by the caller

***********************************************************************************************/

BOOL BlendBecomeA::PassBack(NodeRenderableInk* pNewNode,NodeRenderableInk* pCreatedByNode,CCAttrMap* pAttrMap)
{
	ERROR2IF(pBlendRef == NULL,FALSE,"pBlendRef == NULL");
	ERROR2IF(pNewNode == NULL,FALSE,"pNewNode == NULL");

	if (pProgress != NULL)
	{
		if (!pProgress->Update(pProgress->GetCount(1),TRUE) && !IgnoreEscape)
			return FALSE;
	}

	Path* pInkPath = NULL;

	Node * pPathNode = NULL;

	// DMc
	// we have a game of let's pretend in here
	// because all passback nodes can only have 1 path node in them,
	// then let's get this node & set up the path appropriately
	if (pNewNode->IsNodePath())
	{
		pInkPath = &(((NodePath*)pNewNode)->InkPath);
		pPathNode = (NodePath *)pNewNode;
	}
	else if (pNewNode->IsCompound())
	{
		NodeRenderableInk * pNodeToBlend = ((NodeCompound *)pNewNode)->GetNodeToBlend();

		if (pNodeToBlend)
		{
			if (pNodeToBlend->IsNodePath())
			{
				pInkPath = &(((NodePath *)pNodeToBlend)->InkPath);
				pPathNode = (NodePath *)pNodeToBlend;
			}
		}
	}

	INT32 NumCoords = pInkPath->GetNumCoords();
	INT32 StartIndex=0,EndIndex=0;
	UINT32 NumSubPaths=0;

	Path TempPath;
	BOOL ok = TempPath.Initialise(NumCoords);

	// All subpaths generated by this call will have the same subpath ID
	SubPathID++;

	NodeRenderableInk * pTreeNode = NULL;

	// This loop interates for each subpath in pInkPath.
	while (ok && StartIndex < NumCoords)
	{
		// Get the start and end indexes for the current subpath
		EndIndex = StartIndex;
		pInkPath->FindEndElOfSubPath(&EndIndex);

		ERROR2IF(EndIndex >= NumCoords,FALSE,"EndIndex gone beyond num coords in path");

		// Clear the temp path we've set up, and copy the subpath into it
		TempPath.ClearPath(FALSE);
		ok = pInkPath->CopySectionTo(&TempPath,StartIndex,(EndIndex-StartIndex)+1);

		if (ok)
		{
			// Make sure the filled and stroked flags are correct
			TempPath.IsFilled  = pInkPath->IsFilled;
			TempPath.IsStroked = pInkPath->IsStroked;

			if (m_pMatrix != NULL)
				m_pMatrix->Transform((DocCoord*)TempPath.GetCoordArray(), TempPath.GetNumCoords() );

			// Create a new NodePath for this subpath
			NodePath* pNewNodePath = new NodePath;
			ok = (pNewNodePath != NULL);
			if (ok)
			{
				// Copy the subpath in TempPath into the new NodePath's path object
				Path* pNewPath = &(pNewNodePath->InkPath);
				ok = pNewPath->Initialise(TempPath.GetNumCoords());

				if (ok) ok = pNewPath->CopyPathDataFrom(&TempPath);

				if (ok)
				{
					// Create a new BlendPath obect
				 	BlendPath* pBlendPath = new BlendPath;
					ok = (pBlendPath != NULL);

					// now, have we been passed a compound node ?
					// if so, wrap up the given node with the new node
					if (pNewNode->IsCompound())
					{
						NodeRenderableInk * pWrapNode = CreateWrapNode(pNewNodePath, (NodeCompound *)pNewNode, pAttrMap);

						if (!pWrapNode)
							return FALSE;

						// set up its created by node
						((NodeCompound *)pWrapNode)->SetBlendCreatedByNode(((NodeCompound *)pNewNode)->GetBlendCreatedByNode());
						
						// now, we can only have start blend nodes on first blend path, and we can
						// only have end blend nodes on the last blend path
						if (NumSubPaths != 0)
						{
							pTreeNode = pWrapNode;

							while (pTreeNode && pTreeNode->IsCompound())
							{
								if (((NodeCompound *)pTreeNode)->IsStartBlendGroupNode())
								{
									((NodeCompound *)pTreeNode)->SetStartBlendGroupNode(FALSE);
								}

								pTreeNode = (NodeCompound *)pTreeNode->FindFirstChild(CC_RUNTIME_CLASS(NodeCompound));
							}
						}

						// reset end blend nodes
						if (EndIndex != NumCoords-1)
						{
							pTreeNode = pWrapNode;

							while (pTreeNode && pTreeNode->IsCompound())
							{
								if (((NodeCompound *)pTreeNode)->IsEndBlendGroupNode())
								{
									((NodeCompound *)pTreeNode)->SetEndBlendGroupNode(FALSE);
								}

								pTreeNode = (NodeCompound *)pTreeNode->FindFirstChild(CC_RUNTIME_CLASS(NodeCompound));
							}
						}							

						if (pWrapNode->IsCompound())
						{
							if (((NodeCompound *)pWrapNode)->IsStartBlendGroupNode())
							{
								TRACEUSER( "ChrisS", _T("BlendBecomeA::Start node\n"));
							}
							
							if (((NodeCompound *)pWrapNode)->IsEndBlendGroupNode())
							{
								TRACEUSER( "ChrisS", _T("BlendBecomeA::End node\n"));
							}
						}
							
						if (ok) ok = pBlendPath->Initialise(pWrapNode,Index,pCreatedByNode,SubPathID,NumSubPaths,NULL);
						if (ok) ok = pBlendRef->AddBlendPath(pBlendPath);
					}
					else
					{
						// Initialise the BlendPath object with the generated NodePath
						// and add the BlendPath to the BlendRef
						if (ok) ok = pBlendPath->Initialise(pNewNodePath,Index,pCreatedByNode,SubPathID,NumSubPaths,pAttrMap);
						if (ok) ok = pBlendRef->AddBlendPath(pBlendPath);

						if (IS_A (pNewNode, NodeBlendPath))
						{
							pBlendPath->SetCreatedViaNodeBlendPath (TRUE);
						}
					}
				}
			}
		}
		Index = -1;
		StartIndex = EndIndex+1;	// the start of the next subpath begins after the end of the last one

		// Get the next sub path
		NumSubPaths++;
	}

	if (pNewNode->IsCompound())
	{
		pAttrMap->DeleteAttributes();
		delete pAttrMap;
	}

	if (ok)
	{
		pBlendRef->SetAWComplex(NumSubPaths > 1);	// This is the 'complex' flag used when exporting to AW EPS
		pNewNode->CascadeDelete();
		delete pNewNode;
		pNewNode = NULL;
	}

	return (ok);
	
}

/***********************************************************************************************

> BOOL HandleBecomeA::PassBack(Path* pBlendedPath,CCAttrMap* pBlendedAttrMap,UINT32 Step)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/10/94
	Inputs:		pBlendedPath 	= ptr to a blended path object
				pBlendedAttrMap = ptr to the blended attributes for this path
				Step			= The step number this path belongs to
	Outputs:	-
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	This takes a path and an attribute map generated by a NodeBlender object,
				and generates a NodePath object, if it wants to become a path object
				If pBecomeA->Reason == BECOMEA_REPLACE,  the NodePath and attributes are added to the tree.
				If pBecomeA->Reason == BECOMEA_PASSBACK, the NodePath is passed to the routine.

***********************************************************************************************/

BOOL HandleBecomeA::PassBack(Path* pBlendedPath,CCAttrMap* pBlendedAttrMap,UINT32 Step)
{
	// Check BecomeA mechanism has been set up
	ERROR3IF(pBecomeA     == NULL,"pBecomeA is NULL");
	ERROR3IF(pNodeBlender == NULL,"pNodeBlender is NULL");
	if (pBecomeA == NULL || pNodeBlender == NULL)
		return FALSE;

	// Check for a NULL entry params
	ERROR3IF(pBlendedPath      == NULL,"pBlendedPath is NULL");
	ERROR3IF(pBlendedAttrMap == NULL,"pBlendedAttrMap is NULL");
	if (pBlendedPath == NULL || pBlendedAttrMap == NULL) return FALSE;

	// This lump checks that the Reason is one that we understand
	// It also makes sure that we don't have a NULL UndoOp ptr
	BOOL ValidReason = (pBecomeA->GetReason() == BECOMEA_REPLACE || pBecomeA->GetReason() == BECOMEA_PASSBACK);
	ERROR3IF_PF(!ValidReason,("Unkown BecomeA reason %d",pBecomeA->GetReason()));
	if (!ValidReason) return FALSE;

	// pBecomeA->Reason is one that we understand.

	BOOL 		Success = TRUE;			// Our success flag (Important that this defaults to TRUE)
	NodePath* 	pNewNodePath = NULL;	// Ptr to a new NodePath, if we get to make one.

	if (pBecomeA->BAPath())
	{
		// We need to create a new NodePath, no matter what the reason.
		
		// Allocate a new NodePath node
		ALLOC_WITH_FAIL(pNewNodePath, (new NodePath), pBecomeA->GetUndoOp());
		Success = (pNewNodePath != NULL);

		// Initialise the path
		if (Success) CALL_WITH_FAIL(pNewNodePath->InkPath.Initialise(pBlendedPath->GetNumCoords(),12), pBecomeA->GetUndoOp(), Success);
		if (Success) CALL_WITH_FAIL(pNewNodePath->InkPath.CopyPathDataFrom(pBlendedPath), pBecomeA->GetUndoOp(), Success);

		// If Success is TRUE, then we now have a new NodePath object that contains this shape's path

		if (Success)
		{
		 	switch (pBecomeA->GetReason())
			{
		 		case BECOMEA_REPLACE :
				{
					// It's a BECOMEA_REPLACE, so put the new NodePath in the tree in an undoable way

					// Can't do it in an undoable way without an Undo Op
//					ERROR2IF_PF(pBecomeA->GetUndoOp() == NULL,FALSE,("GetUndoOp() returned NULL"));

					// Can't do it if we don't have a context node
					ERROR2IF(pContextNode == NULL,FALSE,"pContextNode is NULL");

					// By default, attach the new path as the previous child of the context node
					Node* 				pAttachNode = pContextNode;
					AttachNodeDirection AttachDir 	= PREV;

					// By default, create a hide node action for the new path we're attaching to the tree
					BOOL	HideNode	= TRUE;
					Node*	pNodeToHide	= pNewNodePath;

					// Should we be placing all created paths inside a group?
					if (MakeGroupPerStep())
					{
						// Do we need to create a new group? I.e. is this the first path created for this step of the blend?
						if (Step != LastStep)
						{
							// Remember this step value so we can tell if the next path we receive belongs to the
							// same blend step
							LastStep = Step;

							// Create a new group and place it in the tree next to the context node
							pNodeGroup 	= new NodeGroup;
							Success 	= (pNodeGroup != NULL);
							if (Success)
							{
								// Put group into the tree
								pNodeGroup->AttachNode(pContextNode,PREV);

								// Make a hide node action for this group
								HideNode = TRUE;
								pNodeToHide = pNodeGroup;
							}
						}
						else
							HideNode = FALSE;	// We created a hide node action when this group was created

						// If we have a group node, make sure the new path gets placed in the tree as the
						// group's last child
						if (Success)
						{
							pAttachNode = pNodeGroup;
							AttachDir 	= LASTCHILD;
						}
					}
								
					if (Success)
					{
						ERROR2IF(pAttachNode == NULL,FALSE,"The attach node is NULL!!!");

						// Insert the new NodePath into the tree
						pNewNodePath->AttachNode(pAttachNode,AttachDir);

						// Apply all the attributes to the new path
						pNewNodePath->ApplyAttributes(pBlendedAttrMap,TRUE);

						// Set the bounds  
						pNewNodePath->ValidateBoundingRect();
						pNewNodePath->InvalidateBoundingRect();

						// Create a hide node action to hide the node when we undo 
						if (HideNode)
						{
							ERROR2IF(pNodeToHide == NULL,FALSE,"pNodeToHide is NULL");

							if (pBecomeA->GetUndoOp())
							{
								HideNodeAction* UndoHideNodeAction;     
								Success = (HideNodeAction::Init(pBecomeA->GetUndoOp(),
											  			 pBecomeA->GetUndoOp()->GetUndoActionList(),
									 					 pNodeToHide,
									 					 TRUE, 		 // Include subtree size 
							  		 					 ( Action**)(&UndoHideNodeAction))
							  		  					 != AC_FAIL);
							}
						}

						CCAttrMap* pNewAttrMap = pBlendedAttrMap->Copy();
						if (pNewAttrMap != NULL)
							pBecomeA->PassBack(pNewNodePath, pNodeBlender, pNewAttrMap);
					}
				}
				break;

				case BECOMEA_PASSBACK :
				{
					// We need to create a copy of the blended attributes to give to the caller
					// as they don't exist in the tree, and the ones in pBlendedAttrMap will be
					// deleted eventually.
					CCAttrMap* pNewAttrMap = pBlendedAttrMap->Copy();
					if (pNewAttrMap != NULL)
						Success = pBecomeA->PassBack(pNewNodePath,pNodeBlender,pNewAttrMap);
					else
						Success = FALSE;
					break;
				}

				default:
					break;
			}
		}
	}

	if (!Success)
	{
		if (pNodeGroup != NULL)
		{
			// Delete all the groups children (if it has any) and unlink it from the tree (if it's linked)
			// This is all done by CascadeDelete()
			pNodeGroup->CascadeDelete(); 
			delete pNodeGroup;
			pNodeGroup = NULL;
		}
		else if (pNewNodePath != NULL)
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



//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// Export code...

/*********************************************************************************************

>    void NodeBlender::PreExportRender(RenderRegion* pRegion)

     Author:	Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
     Created:	28/11/94
     Inputs:	pRegion = ptr to the export render region to export to
     Outputs:	
     Returns:	
     Purpose: 	Called before this node or any of its children have been rendered to the export region.
				This outputs the "start blender" command.
				Supports ArtWorks EPS and Camelot EPS
     Errors:    

**********************************************************************************************/
 
void NodeBlender::PreExportRender(RenderRegion* pRegion)
{
#ifdef DO_EXPORT
	// Ensure that we are initialised (if it can be done!!)
	if (!Reinit())
		return;

	if (pRegion->IS_KIND_OF(ArtWorksEPSRenderRegion))
	{
		EPSExportDC *pDC = (EPSExportDC *) pRegion->GetRenderDC();

		if (IS_A(pRegion, CamelotEPSRenderRegion))
		{
			// only want renderable stuff in EPS
			return;
		}
		else if (pRegion->IS_KIND_OF(NativeRenderRegion))
		{
			// Output the Camelot format for blenders
			pDC->OutputValue(INT32(0));			// Reserved INT32 1
			pDC->OutputValue(INT32(0));			// Reserved INT32 2
			pDC->OutputValue(INT32(0));			// Reserved INT32 3
			pDC->OutputValue(INT32(0));			// Reserved INT32 4
			pDC->OutputValue(m_ObjIndexStart);	// Index to object at start of blend
			pDC->OutputValue(m_ObjIndexEnd);	// Index to object at end of blend
			pDC->OutputValue(m_PathIndexStart);	// Path index to el num of start path
			pDC->OutputValue(m_PathIndexEnd);	// Path index to el num of end path
			pDC->OutputToken(_T("csbr"));			// Camelot "start blender" token
			pDC->OutputNewLine();

		}
		else 
		{
			NodeBlend* pNodeBlend = GetNodeBlend();

			if (pNodeBlend != NULL)
			{
				if (pNodeBlend->IsArtWorksEPSCompatible())
				{
					// Output the AW format for blenders, if this blender is ArtWorks-compatible
					pDC->OutputValue(INT32(0));									// Expanded flag
					pDC->OutputValue(INT32(IsComplex()));						// Complex flag
					pDC->OutputValue(INT32(IsOneToOne()));						// The OneToOne flag
					pDC->OutputValue(INT32(GetNumBlendSteps()+1));				// The number of blend steps (+1 for AW)
					pDC->OutputValue(GetAWStartPathIndex());					// Get start path index
					pDC->OutputValue(GetAWEndPathIndex());						// Get end path index
					pDC->OutputToken(_T("asbr"));									// ArtWorks "start blender" token
					pDC->OutputNewLine();

					// Blend is done
					return;
				}
				else
				{
					// Blend cannot be done as AW - just render intermediate shapes.
					Render(pRegion);
				}
			}
			else
			{	ERROR3("Parent is not a NodeBlend"); }
		}
	}

	// These two lines will output the first pair of blendable paths - useful for testing
	//pRefStart->GetFirstBlendPath()->GetNodePath()->Render(pRegion);
	//pRefEnd  ->GetFirstBlendPath()->GetNodePath()->Render(pRegion);
#endif
}

/*********************************************************************************************

>    BOOL NodeBlender::ExportRender(RenderRegion* pRegion)

     Author:	Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
     Created:	28/11/94
     Inputs:	pRegion = ptr to the export render region to export to
     Outputs:	
     Returns:	TRUE if ok, FALSE if something went wrong
     Purpose: 	Called after this node and all of its children have been rendered to the export region.
				This outputs the "end blender" command.
				Supports ArtWorks EPS and Camelot EPS
     Errors:    

**********************************************************************************************/
 
BOOL NodeBlender::ExportRender(RenderRegion* pRegion) 
{
#ifdef DO_EXPORT
	// Ensure that we are initialised (if it can be done!!)
	if (!Reinit())
		return FALSE;

	if (pRegion->IS_KIND_OF(ArtWorksEPSRenderRegion))
	{
		EPSExportDC *pDC = (EPSExportDC *) pRegion->GetRenderDC();

		if (IS_A(pRegion, CamelotEPSRenderRegion))
		{
			// only want renderable stuff in EPS
			return FALSE; // render this as paths.
		}
		else if (pRegion->IS_KIND_OF(NativeRenderRegion))
		{
			pDC->OutputToken(_T("cebr"));				// Camelot "end blender" token
			pDC->OutputNewLine();
		}
		else
		{
			NodeBlend* pNodeBlend = GetNodeBlend();

			if (pNodeBlend != NULL)
			{
				if (pNodeBlend->IsArtWorksEPSCompatible())
				{
					pDC->OutputToken(_T("aebr"));				// ArtWorks "end blender" token
					pDC->OutputNewLine();
				}
			}
		}
		// Tell caller we rendered ourselves ok
		return TRUE;
	}
#endif
	// Render this node in the normal way
	return FALSE;
}

/*********************************************************************************************

>    BOOL NodeBlend::IsArtWorksEPSCompatible()

     Author:	Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
     Created:	28/11/94
     Inputs:	
     Outputs:	
     Returns:	TRUE if this blender is AW compatible
     Purpose: 	A blender is AW EPS compatible if ALL the following conditions are TRUE:
	 				1) This node is between two path nodes.
					2) The blender is blending the two path nodes it lies between
					3) The two path nodes contain the same number of sub paths.

				This is called by the parent blend's IsArtWorksEPSCompatible() func.
     Errors:    

**********************************************************************************************/
 
BOOL NodeBlender::IsArtWorksEPSCompatible()
{
	ERROR2IF(m_pRefStart == NULL,FALSE,"m_pRefStart == NULL");
	ERROR2IF(m_pRefEnd   == NULL,FALSE,"m_pRefEnd   == NULL");

	Node* pPrevNode = FindPrevious();
	Node* pNextNode = FindNext();

	return (pPrevNode == m_pNodeStart 	&&
			pNextNode == m_pNodeEnd		&&
			pPrevNode != NULL			&&
			pNextNode != NULL			&&
			IS_A(pPrevNode,NodePath)	&&
			IS_A(pNextNode,NodePath)	&&
			m_pRefStart->GetNumBlendPaths() == m_pRefEnd->GetNumBlendPaths());
}

/*********************************************************************************************

>    BOOL NodeBlend::IsComplex()

     Author:	Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
     Created:	28/11/94
     Inputs:	
     Outputs:	
     Returns:	TRUE if this blender is complex
     Purpose: 	The blender is "complex" as far as AW EPS is concerned, if either the start path
	 			or the end path of the blend has more that one sub-path.
     Errors:    

**********************************************************************************************/
 
BOOL NodeBlender::IsComplex()
{
	ERROR2IF(m_pRefStart == NULL,FALSE,"m_pRefStart == NULL");
	ERROR2IF(m_pRefEnd   == NULL,FALSE,"m_pRefEnd   == NULL");

	return (m_pRefStart->GetAWComplex() || m_pRefEnd->GetAWComplex());
}

/*********************************************************************************************

> INT32 NodeBlender::GetAWStartPathIndex()

     Author:	Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
     Created:	28/11/94
     Inputs:	
     Outputs:	
     Returns:	AW path index based on the PathIndexStart member var
     Purpose: 	This converts PathIndexStart into an index AW understands (compatible with RISCOS path format)
     Errors:    

**********************************************************************************************/

INT32 NodeBlender::GetAWStartPathIndex()
{
	ERROR2IF(m_pRefStart == NULL,-1,"m_pRefStart == NULL");
	return (GetAWPathIndex(m_pRefStart,m_PathIndexStart));
}

/*********************************************************************************************

> INT32 NodeBlender::GetAWEndPathIndex()

     Author:	Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
     Created:	28/11/94
     Inputs:	
     Outputs:	
     Returns:	AW path index based on the PathIndexEnd member var
     Purpose: 	This converts PathIndexEnd into an index AW understands (compatible with RISCOS path format)
     Errors:    

**********************************************************************************************/

INT32 NodeBlender::GetAWEndPathIndex()
{
	ERROR2IF(m_pRefEnd == NULL,-1,"m_pRefEnd == NULL");
	return (GetAWPathIndex(m_pRefEnd,m_PathIndexEnd));
}

/*********************************************************************************************

> INT32 NodeBlender::GetAWPathIndex(BlendRef *pRef,INT32 PathIndex)

     Author:	Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
     Created:	28/11/94
     Inputs:	pRef = ptr to a blend ref
	 			PathIndex = index into the blend ref's first path
     Outputs:	
     Returns:	AW path index based on PathIndex and the first path in the blend ref
     Purpose: 	This converts the index into an index AW understands (compatible with RISCOS path format)
     Errors:    

**********************************************************************************************/

INT32 NodeBlender::GetAWPathIndex(BlendRef *pRef,INT32 PathIndex)
{
	BlendPath* pBlendPath = pRef->GetFirstBlendPath();
	if (pBlendPath == NULL || pBlendPath->GetPath() == NULL) 
		return -1;

	Path* pPath = pBlendPath->GetPath();
	INT32 AWPathIndex=0,Index=0;
	while (Index < PathIndex)
	{
		pPath->FindNextEndPoint(&Index);
		AWPathIndex++;
	}

	return AWPathIndex;		
}

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------

/********************************************************************************************

  >	virtual BOOL NodeBlender::WritePreChildrenWeb(BaseCamelotFilter* pFilter)

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/5/96
	Inputs:		pFilter = ptr to the filter
	Returns:	TRUE if record is written, FALSE if not
	Purpose:	Writes the blender record to the filter
	SeeAlso:	-

********************************************************************************************/

BOOL NodeBlender::WritePreChildrenWeb(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter == NULL,FALSE,"NULL filter param");

	BOOL ok = TRUE;

	CXaraFileRecord Rec(TAG_BLENDER,TAG_BLENDER_SIZE);

	if (ok) ok = Rec.Init();
	if (ok) ok = Rec.WriteINT32(m_PathIndexStart);
	if (ok) ok = Rec.WriteINT32(m_PathIndexEnd);
	if (ok) ok = pFilter->Write(&Rec);

	// Diccon 9/99 added extra record
	CXaraFileRecord Additional(TAG_BLENDERADDITIONAL, TAG_BLENDERADDITIONAL_SIZE);
	if (ok) ok = Additional.Init();
	if (ok) ok = Additional.WriteINT32(m_BlendedOnCurve);
	
	if (ok) 
	{	
		if (m_NodeBlendPathIndex == -2)
			ok = Additional.WriteINT32(-1);
		else
			ok = Additional.WriteINT32(m_NodeBlendPathIndex);
	}
	if (ok) ok = Additional.WriteINT32(m_ObjIndexStart);
	if (ok) ok = Additional.WriteINT32(m_ObjIndexEnd);

	// Karim 05/02/2001
	// Extra bitfield tacked onto the end of the record.
	// Currently this stores only our reversed-on-path flag.
	if (ok)
	{
		BYTE BitField = IsReversed() ? 1 : 0;
		ok = Additional.WriteBYTE(BitField);
	}

	if (ok) ok = pFilter->Write(&Additional);

	return ok;
#else
	return FALSE;
#endif
}

//--------------------------------------------------------------
// See NodeBlender::WritePreChildrenWeb(BaseCamelotFilter* pFilter)
//
BOOL NodeBlender::WritePreChildrenNative(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT
	return WritePreChildrenWeb(pFilter);
#else
	return FALSE;
#endif
}

/********************************************************************************************

> 	BOOL NodeBlender::WriteBeginChildRecordsNative(BaseCamelotFilter* pFilter)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/5/99
	Inputs:		pFilter = ptr to the filter to write to
	Outputs:	-
	Returns:	TRUE ok, FALSE otherwise
	Purpose:	Begins the child record sequence for the blender

				Blenders need to write out curve CMapPtrToPtr::iteratoring information if the blend
				is on a curve
	SeeAlso:	WritePreChildrenNative()

********************************************************************************************/

BOOL NodeBlender::WriteBeginChildRecordsNative(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter == NULL,FALSE,"NULL filter param");

	if (GetNodeBlendPath() == NULL)
		return NodeRenderableInk::WriteBeginChildRecordsNative(pFilter);

	// First thing to do is write out the Down record
	BOOL ok = (pFilter->WriteZeroSizedRecord(TAG_DOWN));

	// Write out the blender's curve CMapPtrToPtr::iterator proportions
	{
		CamelotFileRecord PropRec(pFilter,TAG_BLENDER_CURVEPROP,TAG_BLENDER_CURVEPROP_SIZE);
		if (ok) ok = PropRec.Init();
		if (ok) ok = PropRec.WriteDOUBLE(m_ProportionOfPathDistStart);
		if (ok) ok = PropRec.WriteDOUBLE(m_ProportionOfPathDistEnd);
		if (ok) ok = pFilter->Write(&PropRec);
	}
		
	// Write out the blender's curve angles
	if (m_AngleStart != 0.0 || m_AngleEnd != 0.0)
	{
		CamelotFileRecord AngleRec(pFilter,TAG_BLENDER_CURVEANGLES,TAG_BLENDER_CURVEANGLES_SIZE);
		if (ok) ok = AngleRec.Init();
		if (ok) ok = AngleRec.WriteDOUBLE(m_AngleStart);
		if (ok) ok = AngleRec.WriteDOUBLE(m_AngleEnd);
		if (ok) ok = pFilter->Write(&AngleRec);
	}

	return ok;
#else
	return TRUE;
#endif //DO_EXPORT
}

/********************************************************************************************

> 	BOOL NodeBlender::WriteEndChildRecordsNative(BaseCamelotFilter* pFilter)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/5/99
	Inputs:		pFilter = ptr to the filter to write to
	Outputs:	-
	Returns:	TRUE ok, FALSE otherwise
	Purpose:	Ends the child record sequence for the blend

				Only does anything if the blend is on a curve

	SeeAlso:	WritePreChildrenNative()

********************************************************************************************/

BOOL NodeBlender::WriteEndChildRecordsNative(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter == NULL,FALSE,"NULL filter param");

	if (GetNodeBlendPath() == NULL)
		return NodeRenderableInk::WriteEndChildRecordsNative(pFilter);

	return pFilter->WriteZeroSizedRecord(TAG_UP);
#else
	return FALSE;
#endif
}

/********************************************************************************************

> 	BOOL NodeBlender::WriteBeginChildRecordsWeb(BaseCamelotFilter* pFilter)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/5/99
	Inputs:		pFilter = ptr to the filter to write to
	Outputs:	-
	Returns:	TRUE ok, FALSE otherwise
	Purpose:	Same as the Native version

	SeeAlso:	WritePreChildrenNative()

********************************************************************************************/

BOOL NodeBlender::WriteBeginChildRecordsWeb(BaseCamelotFilter* pFilter)
{
	return WriteBeginChildRecordsNative(pFilter);
}

/********************************************************************************************

> 	BOOL NodeBlender::WriteEndChildRecordsWeb(BaseCamelotFilter* pFilter)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/5/99
	Inputs:		pFilter = ptr to the filter to write to
	Outputs:	-
	Returns:	TRUE ok, FALSE otherwise
	Purpose:	Same as the Native version

	SeeAlso:	WritePreChildrenNative()

********************************************************************************************/

BOOL NodeBlender::WriteEndChildRecordsWeb(BaseCamelotFilter* pFilter)
{
	return WriteEndChildRecordsNative(pFilter);
}

//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------

/********************************************************************************************

>	NodeBlend* NodeBlender::GetNodeBlend()

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/5/99
	Inputs:		-
	Returns:	ptr to the blend that this blender belongs to, or NULL if it's all gone pear shaped
	Purpose:	Central NodeBlend "getter"
	SeeAlso:	-

********************************************************************************************/

NodeBlend* NodeBlender::GetNodeBlend()
{
	Node* pNode = FindParent();
	if (pNode != NULL && IS_A(pNode,NodeBlend))
		return (NodeBlend*)pNode;

	return NULL;
}

/********************************************************************************************

>	NodeBlendPath* NodeBlender::GetNodeBlendPath()

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/4/99
	Inputs:		-
	Returns:	ptr to the blend path for this blender, or NULL if it fails
	Purpose:	It gets the node blend path from its parent
	SeeAlso:	-

********************************************************************************************/

NodeBlendPath* NodeBlender::GetNodeBlendPath()
{
	if (m_NodeBlendPathIndex > -1)
	{
		NodeBlend* pNode = GetNodeBlend();
		if (pNode != NULL)
			return pNode->GetNodeBlendPath(m_NodeBlendPathIndex);
	}
	return NULL;
}

/********************************************************************************************

>	BOOL NodeBlender::GetPointOnNodeBlendPath(DocCoord* pPoint,double BlendRatio,double* pAngle)

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/5/99
	Inputs:		BlendRatio = the blend ratio to use
	Outputs:	pPoint = ptr to store coord
				pAngle = angle at this point in degrees (ptr can be NULL)
	Returns:	TRUE if it could find the point on the NodeBlendPath
				FALSE if there's no path to blend along, or the point-on-path function fails
	Purpose:	Access function to make it easy to get the correct point in the path
				for this NodeBlender with the given blend ratio
	SeeAlso:	-

********************************************************************************************/

BOOL NodeBlender::GetPointOnNodeBlendPath(double BlendRatio,DocCoord* pPoint,double* pAngle)
{
	ERROR2IF(pPoint == NULL,FALSE,"NULL coord ptr");

	BOOL ok = FALSE;

	NodeBlendPath* pNodeBlendPath = GetNodeBlendPath();

	if (pNodeBlendPath)
	{
		double PropStart = GetProportionOfPathDistStart();
		double PropEnd   = GetProportionOfPathDistEnd();

		double PathLen       = pNodeBlendPath->GetPathLength();
		double PropLen       = PropEnd-PropStart;
		double PathLenOffset = PathLen*PropStart;

		double Tangent = 0.0;
		MILLIPOINT Dist = MILLIPOINT(((PathLen*PropLen) * BlendRatio)+PathLenOffset);
		ok = pNodeBlendPath->GetPointAtDistance(Dist,pPoint,&Tangent);

		if (ok && pAngle)
		{
			if (IsTangential())
				*pAngle = Tangent/PI*180;
			else
				*pAngle = 0.0;
		}
	}

	return ok;
}


/********************************************************************************************

>	BOOL NodeBlender::GetPointFromDistance(double Distance, DocCoord8 pPoint, double* pAngle)

 	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com> 
	Created:	17/9/99
	Inputs:		Distance - the distance along the nodeblendpath 
	Outputs:	pPoint = ptr to store coord
				pAngle = angle at this point in degrees (ptr can be NULL)
	Returns:	TRUE if it could find the point on the NodeBlendPath
				FALSE if there's no path to blend along, or the point-on-path function fails
	Purpose:	Access function to make it easy to get the correct point in the path
				for this NodeBlender with the given distance along the path.
	SeeAlso:	-

********************************************************************************************/

BOOL NodeBlender::GetPointFromDistance(double Distance, DocCoord* pPoint, double* pAngle)
{
	ERROR2IF(pPoint == NULL,FALSE,"NULL coord ptr");

	BOOL ok = FALSE;

	NodeBlendPath* pNodeBlendPath = GetNodeBlendPath();

	if (pNodeBlendPath)
	{
		double Tangent = 0.0;
		ok = pNodeBlendPath->GetPointAtDistance((INT32)Distance,pPoint,&Tangent);

		if (ok && pAngle)
		{
			if (IsTangential())
				*pAngle = Tangent/PI*180;
			else
				*pAngle = 0.0;
		}
	}

	return ok;
}




double NodeBlender::GetProportionOfPathDistStart()
{ 
	if (m_ProportionOfPathDistStart < 0.0) return 0.0;
	return m_ProportionOfPathDistStart;
}

double NodeBlender::GetProportionOfPathDistEnd()
{ 
	if (m_ProportionOfPathDistEnd < 0.0) return 1.0;
	return m_ProportionOfPathDistEnd;
}

void NodeBlender::SetProportionOfPathDistStart(double p)
{ 
	if (p < 0.0) p = 0.0;
	if (p > 1.0) p = 1.0;
	if (p > GetProportionOfPathDistEnd()) p = GetProportionOfPathDistEnd();
	m_ProportionOfPathDistStart = p;
}

void NodeBlender::SetProportionOfPathDistEnd(double p)
{ 
	if (p < 0.0) p = 0.0;
	if (p > 1.0) p = 1.0;
	if (p < GetProportionOfPathDistStart()) p = GetProportionOfPathDistStart();
	m_ProportionOfPathDistEnd = p;
}


double	NodeBlender::GetAngleStart()
{
	return m_AngleStart;
}

double	NodeBlender::GetAngleEnd()
{
	return m_AngleEnd;
}

void NodeBlender::SetAngleStart(double Angle)
{
	m_AngleStart = fmod(Angle,360.0);
}

void NodeBlender::SetAngleEnd(double Angle)
{
	m_AngleEnd = fmod(Angle,360.0);
}



/********************************************************************************************

>	double NodeBlender::GetBlendDistance()

 	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/9/99
	Inputs:		-
	Returns:	distance between the centre of the start and end nodes
	Purpose:	to get the distance between the start and end nodes of the blender 
				when the blend is not on a curve.  Note that it will return an incorrect
				value if the blend is on a curve.
	SeeAlso:	NodeBlendPath::GetPathLength();

********************************************************************************************/

double NodeBlender::GetLinearDistance()
{
	DocCoord StartCentre;
	DocCoord EndCentre;

	BOOL Valid = GetBlendObjectCentres(&StartCentre, &EndCentre);
	if (!Valid)
		return FALSE;
		
	Coord pStart = (Coord)StartCentre;
	Coord pEnd = (Coord)EndCentre;

	double Distance = pStart.Distance(pEnd);
		
	return Distance;
		
}


/********************************************************************************************

>	BOOL NodeBlender::GetBlendDirection(double* Gradient)

 	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/9/99
	Inputs:		-
	Outputs:    Gradient of the line that the blend follows
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	To get a vector of the direction of a linear blend.

********************************************************************************************/

BOOL NodeBlender::GetBlendDirection(double* Gradient)
{
	if (IsBlendedOnCurve())
	{
		ERROR3("Trying to get linear direction of blend on a curve");
		return FALSE;
	}

	DocCoord StartCentre;
	DocCoord EndCentre;

	BOOL Valid = GetBlendObjectCentres(&StartCentre, &EndCentre);
	if (!Valid)
		return FALSE;

	double XDiff = EndCentre.x - StartCentre.x;
	double YDiff = EndCentre.y - StartCentre.y;

	*Gradient = YDiff/XDiff;

	return TRUE;
}


/********************************************************************************************

>	BOOL NodeBlender::GetBlendObjectCentres(DocCoord* pFirstCentre, DocCoord* pLastCentre)

 	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/9/99
	Inputs:		-
	Outputs:    Coordinates of the centres of the first and last objects of this blender
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	To get the centres of the objects that are blended between

********************************************************************************************/

BOOL NodeBlender::GetBlendObjectCentres(DocCoord* pFirstCentre, DocCoord* pEndCentre)
{
	// get the start and end nodes
	NodeRenderableInk* pBlendStart =  GetNodeStart();
	NodeRenderableInk* pBlendEnd = GetNodeEnd();

	if (pBlendStart ==NULL || pBlendEnd == NULL)
	{
		ERROR3IF(pBlendStart ==NULL, "Blender has no start nodes");
		ERROR3IF(pBlendEnd ==NULL, "Blender has no end nodes");
		return FALSE;
	}

	DocRect StartRect = pBlendStart->GetBoundingRect();
	DocRect EndRect = pBlendEnd->GetBoundingRect();

	DocCoord StartCentre = StartRect.Centre();
	DocCoord EndCentre = EndRect.Centre();

	*pFirstCentre = StartCentre;
	*pEndCentre = EndCentre;

	return TRUE;
}
