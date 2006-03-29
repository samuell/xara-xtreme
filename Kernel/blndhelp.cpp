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
// ppbevel - Definition of bevel BevelPathProcessor class

#include "camtypes.h"
#include "ink.h"
#include "paths.h"
#include "lineattr.h"
#include "blendatt.h"
#include "blndhelp.h"
#include "attrmap.h"
#include "nodebldr.h"
#include "fillattr.h"
//#include "mario.h"
#include "ndbldpth.h"
#include "fixmem.h"
#include "gblend.h"
#include "nodecomp.h"
#include "fthrattr.h"

CC_IMPLEMENT_DYNAMIC(BlendHelpers, CCObject);
CC_IMPLEMENT_DYNAMIC(SumAllPathsPathProcessor, PathProcessor);
CC_IMPLEMENT_DYNCREATE(SumAllPathsElem, ListItem);

BlendHelpers::BlendHelpers()
{
	m_TempArraySize = 0;
	m_pTempCoords = NULL;
	m_pTempVerbs = NULL;
	m_pTempFlags = NULL;
	m_GBlendBuffSize = 0;
	m_pGBlendBuff = NULL;
	m_ArrayLength = 0;
}

BlendHelpers::~BlendHelpers()
{
	DeallocTempBuffers();
}

/********************************************************************************************

>	BOOL BlendHelpers::BlendAttributes(BlendNodeParam * pParam, CCAttrMap* pBlendedAttrMap)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com> based on blender code
	Created:	21/2/2000
	Inputs:		pParam	-	the blend node param
	Outputs:	-
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	Blends the attributes of the two BlendPath objects by the amount specified in BlendRatio
	SeeAlso:	-

********************************************************************************************/
BOOL BlendHelpers::BlendAttributes(BlendNodeParam * pParam, CCAttrMap* pBlendedAttrMap)
{
	// Check entry params
	double BlendRatio = pParam->GetAttrBlendRatio();

	BOOL ok = (pParam != NULL && pBlendedAttrMap != NULL);
	ERROR3IF(!ok,"One or more NULL entry params");
	if (!ok) return FALSE;

	// Find the attributes that are applied to the blend paths

	BlendPath * pBlendPathStart = pParam->GetStartBlendPath();
	BlendPath * pBlendPathEnd   = pParam->GetEndBlendPath();
	ok = (pBlendPathStart != NULL && pBlendPathEnd != NULL);
	ERROR3IF(!ok, "Blend paths are NULL");
	if (!ok) return FALSE;

	ok = (pBlendPathStart->GetCreatedByNode() != NULL && 
		pBlendPathEnd->GetCreatedByNode() != NULL);
	ERROR3IF(!ok, "Blend path created by nodes are NULL");
	if (!ok) return FALSE;

	BOOL startExludeGLAs = TRUE, endExcludeGLAs = TRUE;

	NodeAttribute			* pAttr				= NULL;
	if(pBlendPathStart->GetCreatedByNode()->FindAppliedAttribute(CC_RUNTIME_CLASS(AttrFeather),&pAttr))
	{
		startExludeGLAs = FALSE;
	}
	if(pBlendPathEnd->GetCreatedByNode()->FindAppliedAttribute(CC_RUNTIME_CLASS(AttrFeather),&pAttr))
	{
		endExcludeGLAs = FALSE;
	}

	CCAttrMap* pAttrMapStart = CCAttrMap::MakeAppliedAttrMap(pBlendPathStart->GetCreatedByNode(), startExludeGLAs);
	CCAttrMap* pAttrMapEnd   = CCAttrMap::MakeAppliedAttrMap(pBlendPathEnd->GetCreatedByNode(), endExcludeGLAs);

	if (!pAttrMapStart || !pAttrMapEnd)
		return FALSE;

	// find the attributes on the nodes
	
	if (pParam->GetNodeBlendPath() != NULL)
	{
		Trans2DMatrix* pRotateStart = GetRotateMatrix(pParam->GetNodeStart(),
			360.0 - pParam->GetAngleStart());
		Trans2DMatrix* pRotateEnd   = GetRotateMatrix(pParam->GetNodeEnd(),
			360.0 - pParam->GetAngleEnd());
		if (pRotateStart)	pAttrMapStart->Transform(*pRotateStart);
		if (pRotateEnd)		pAttrMapEnd  ->Transform(*pRotateEnd);

		if (pRotateStart)
			delete pRotateStart;

		if (pRotateEnd)
			delete pRotateEnd;
	}

	// These vars are used as params to the CCAttrMap funcs
	CCRuntimeClass	   *pTypeStart; 
	void			   *pValStart;
	void			   *pValEnd;
	double				OldBlendRatio = pParam->GetBlendRatio();
	// Process each attribute in turn
	CCAttrMap::iterator	PosStart = pAttrMapStart->GetStartPosition();
	CCAttrMap::iterator	EndStart = pAttrMapStart->GetEndPosition();
	for (;PosStart != EndStart;)
	{
		// Get a ptr to the attr at position PosStart in the start node's attr map
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
				
				BlendRatio = pParam->GetObjectRatio();
			
			}
			else
			{
				BlendRatio = pParam->GetInvertedAttributeRatio();
			
			}

		}
		if (pNodeAttrStart->IsAFlatFill() || (pNodeAttrStart->GetRuntimeClass() == CC_RUNTIME_CLASS(AttrLineWidth)))
		{
			BlendRatio = pParam->GetInvertedAttributeRatio();
		}
			
		// Get a blended attribute
		NodeAttribute* pBlendedNodeAttr = NULL;

		// Find an attr of the same type in the end object's attr list,
		// and blend the two attrs together
		pValEnd = NULL;

		if (pAttrMapEnd->Lookup(pTypeStart,pValEnd))
		{
			// We've found a matching end attr, so try to blend it with the start attr

			// Set up the param object to pass to the start attr's blend method
			BlendAttrParam BlendParam;

			NodeAttribute * pEndAttr = (NodeAttribute *)pValEnd;

			// Initialise the BlendParam with the end attr and blend ratio
			if (BlendParam.Init(pParam->GetRenderRegion(),
								pEndAttr,BlendRatio,
								pParam->GetColourBlendType(),
								pAttrMapStart, pAttrMapEnd))
			{
				// Successfully initialised, so now try blending the attributes
				if (pNodeAttrStart->Blend(&BlendParam))
				{
					// Attrs successfully blended, now get a ptr to the new attr.
					// Once we get the blended attr ptr, it belongs to us, so we have
					// to delete it when it is not needed
					pBlendedNodeAttr = BlendParam.GetBlendedAttr();
				}
			}
		}

		// If we have a blended attr, pBlendedNodeAttr != NULL
		if (pBlendedNodeAttr != NULL)
		{
			// Get the type of the blended attr
			CCRuntimeClass *pTypeBlend = pBlendedNodeAttr->GetAttributeType();
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
		}
	}

	if (pParam->GetNodeBlendPath() != NULL)
	{
		Trans2DMatrix* pRotateStart = GetRotateMatrix(pParam->GetNodeStart(),
			pParam->GetAngleStart());
		Trans2DMatrix* pRotateEnd   = GetRotateMatrix(pParam->GetNodeEnd(),
			pParam->GetAngleEnd()  );
		if (pRotateStart)	pAttrMapStart->Transform(*pRotateStart);
		if (pRotateEnd)		pAttrMapEnd	 ->Transform(*pRotateEnd);

		if (pRotateStart)
			delete pRotateStart;
		
		if (pRotateEnd)
			delete pRotateEnd;
	}

	delete pAttrMapStart;
	delete pAttrMapEnd;

	return TRUE;	
}

/********************************************************************************************

> BOOL BlendHelpers::ReallocTempBuffers(UINT32 Size)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/11/94
	Inputs:		Size = Size the temp arrays should be
	Outputs:	-
	Returns:	-
	Purpose:	Allocates memory for the temp path arrays, and sets the size var to 0
				You can use calls to GetCoordArray(), GetVerbArray() and GetFlagArray() to get the alloced arrays.
	SeeAlso:	-

********************************************************************************************/

BOOL BlendHelpers::ReallocTempBuffers(UINT32 Size)
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

>	void BlendHelpers::DeallocTempBuffers()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/11/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Releases memory allocated for the temp path arrays, and sets the size var to 0
	SeeAlso:	-

********************************************************************************************/

void BlendHelpers::DeallocTempBuffers()
{
	if (m_pTempCoords != NULL) { CCFree(m_pTempCoords); m_pTempCoords = NULL; }
	if (m_pTempVerbs  != NULL) { CCFree(m_pTempVerbs);  m_pTempVerbs  = NULL; }
	if (m_pTempFlags  != NULL) { CCFree(m_pTempFlags);  m_pTempFlags  = NULL; }
	m_TempArraySize = 0;

	if (m_pGBlendBuff != NULL) { CCFree(m_pGBlendBuff); m_pGBlendBuff = NULL; }
	m_GBlendBuffSize = 0;
}

/********************************************************************************************

>	DocCoord*  BlendHelpers::GetCoordArray(UINT32 MinSize);

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/11/94
	Inputs:		MinSize = min size the array should be
	Outputs:	-
	Returns:	Ptr to the array, or NULL if can't get memory
	Purpose:	Used to get an array you can write to.
	SeeAlso:	-

********************************************************************************************/

DocCoord* BlendHelpers::GetCoordArray(UINT32 MinSize)
{
	MinSize++;
	if (m_TempArraySize >= MinSize) return m_pTempCoords;

	if (ReallocTempBuffers(MinSize))
		return m_pTempCoords;
	else
		return NULL;
}

/********************************************************************************************

>	PathVerb*  BlendHelpers::GetVerbArray(UINT32 MinSize);

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/11/94
	Inputs:		MinSize = min size the array should be
	Outputs:	-
	Returns:	Ptr to the array, or NULL if can't get memory
	Purpose:	Used to get an array you can write to.
	SeeAlso:	-

********************************************************************************************/

PathVerb* BlendHelpers::GetVerbArray(UINT32 MinSize)
{
	MinSize++;
	if (m_TempArraySize >= MinSize) return m_pTempVerbs;

	if (ReallocTempBuffers(MinSize))
		return m_pTempVerbs;
	else
		return NULL;
}

/********************************************************************************************

>	PathFlags*  BlendHelpers::GetFlagArray(UINT32 MinSize);

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/11/94
	Inputs:		MinSize = min size the array should be
	Outputs:	-
	Returns:	Ptr to the array, or NULL if can't get memory
	Purpose:	Used to get an array you can write to.
	SeeAlso:	-

********************************************************************************************/

PathFlags* BlendHelpers::GetFlagArray(UINT32 MinSize)
{
	MinSize++;
	if (m_TempArraySize >= MinSize) return m_pTempFlags;

	if (ReallocTempBuffers(MinSize))
		return m_pTempFlags;
	else
		return NULL;
}

/********************************************************************************************

>	UINT32*  BlendHelpers::GetGBlendBuff(UINT32 MinSize);

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/11/94
	Inputs:		MinSize = min size the buffer should be
	Outputs:	-
	Returns:	Ptr to the buffer, or NULL if can't get memory
	Purpose:	Used to get a buffer you can write to.
	SeeAlso:	-

********************************************************************************************/

UINT32* BlendHelpers::GetGBlendBuff(UINT32 MinSize)
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

> Trans2DMatrix* BlendHelpers::GetRotateMatrix(NodeRenderableBounded* pNode,double Angle)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com> - altered by DMc
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

Trans2DMatrix* BlendHelpers::GetRotateMatrix(NodeRenderableBounded* pNode,double Angle)
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

>	BOOL BlendHelpers::BlendPaths(BlendNodeParam * pParam, Path * pPath)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/2/2000
	Inputs:		The blend node parameter
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
BOOL BlendHelpers::BlendPaths(BlendNodeParam * pParam, Path * pPath)
{
	// Check entry params
	BlendPath * pBlendPathStart = pParam->GetStartBlendPath();
	BlendPath * pBlendPathEnd   = pParam->GetEndBlendPath();

	ERROR2IF(!pBlendPathStart->GetBlendNode()->IsNodePath(), FALSE,
		"Start blend path's node isn't a node path");

	ERROR2IF(!pBlendPathEnd->GetBlendNode()->IsNodePath(), FALSE,
		"End blend path's node isn't a node path");

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

	Path * pPathEnd = NULL;

	if (pBlendPathEnd->GetBlendNode()->IsNodePath())
	{
		pPathEnd   = &(((NodePath *)pBlendPathEnd->GetBlendNode())->InkPath);
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
	if (pParam->GetOneToOne())
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
						1024,								// Flatness

						pBuff,									// Buffer for GBlend to use
						DestPathSize*sizeof(UINT32));			// The buffer size

	// Blend the paths
	m_ArrayLength = GBlendObj.Blend(pParam->GetBlendRatio(),	// The blend ratio, 0.0 < BlendRatio < 1.0
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
		if (pParam->GetBlendRatio() <= 0.5) 
			pBlendPath = pBlendPathStart;
		else
			pBlendPath = pBlendPathEnd;

		if (pBlendPath->GetOrigMapping() != 0)
			NodeBlender::ReversePath(pDestCoords,pDestVerbs,m_ArrayLength);
	}

	// We need to do some work on the blended path
	if (!NodeBlender::ProcessBlendedPath(pDestCoords,pDestVerbs,pDestFlags,m_ArrayLength,Closed))
		return FALSE;

	Path RetnPath;
	RetnPath.Initialise(m_ArrayLength);

	BOOL Filled  = pPathStart->IsFilled  || pPathEnd->IsFilled;
	BOOL Stroked = pPathStart->IsStroked || pPathEnd->IsStroked;

	RetnPath.MergeTwoPaths(pDestCoords,pDestVerbs,pDestFlags,m_ArrayLength,Filled);

	pPath->ClearPath();
	pPath->CloneFrom(RetnPath);
	pPath->IsFilled = Filled;
	pPath->IsStroked = Stroked;
	
	return TRUE;
}

/////////////////////////////////////////////////////////
// The become a for compound nodes in blends
/********************************************************************************************

>	CompoundNodeBlendBecomeA::CompoundNodeBlendBecomeA(BecomeAReason ThisReason, 
			CCRuntimeClass* pClass, 
			UndoableOperation* pOp, 
			BOOL sel = TRUE, 
			BOOL First = FALSE,
			NodeCompound * pNode
			) : BecomeA(ThisReason, pClass, pOp, sel, First)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/2/2000
	Inputs:		See base class, plus the compound node we're using
	Outputs:	
	Returns:	
	Purpose:	Constructor
	SeeAlso:	-
********************************************************************************************/
BOOL CompoundNodeBlendBecomeA::PassBack(NodeRenderableInk* pNewNode,
		NodeRenderableInk* pCreatedByNode,
		CCAttrMap* pAttrMap)

{
	ERROR2IF(pCreatedByNode == NULL, FALSE, "CompoundNodeBlendBecomeA - no created by node");

	NodeRenderableInk * pNewCompound = NULL;

	if (pAttrMap == NULL)
	{
		CCAttrMap * pAppliedAttrMap = CCAttrMap::MakeAppliedAttrMap(pCreatedByNode);
//		pAppliedAttrMap->RemoveIndirectlyAppliedLayeredAttributes(pCreatedByNode,FALSE);
		pAttrMap = pAppliedAttrMap->Copy();
		delete pAppliedAttrMap;
	}

	pNewCompound = m_pCompound->CreateTreeFromNodeToBlend(pNewNode, pAttrMap);
	
	if (pNewCompound && pNewCompound->IsCompound())
	{
		((NodeCompound *)pNewCompound)->SetBlendCreatedByNode(m_pCompound);

		TRACEUSER( "DavidM", _T("Count %d Num path nodes %d\n"),
			m_Count, m_NumPathNodes);

		if (m_Count == 0)
		{
			((NodeCompound*)pNewCompound)->SetStartBlendGroupNode();
		}
		
		if (m_Count == m_NumPathNodes-1)
		{
			((NodeCompound*)pNewCompound)->SetEndBlendGroupNode();
		}
	}

	m_Count++;

	return m_pBecomeA->PassBack(pNewCompound, pCreatedByNode, pAttrMap);
}

///////////////////////////////////////////////////////////////////////
// SumAllPathsPathProcessor implementation

/********************************************************************************************

>	SumAllPathsPathProcessor::SumAllPathsPathProcessor(BOOL bDrawOriginalPaths)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/2/2000
	Inputs:		Whether to draw the paths passed in or not
	Outputs:	
	Returns:	
	Purpose:	Constructor
	SeeAlso:	-
********************************************************************************************/
SumAllPathsPathProcessor::SumAllPathsPathProcessor(BOOL bDrawOriginalPaths)
{
	m_bDrawPaths = bDrawOriginalPaths;
	m_bEnabled = TRUE;
}

/********************************************************************************************

>	SumAllPathsPathProcessor::~SumAllPathsPathProcessor()

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/2/2000
	Inputs:		
	Outputs:	
	Returns:	
	Purpose:	Destructor
	SeeAlso:	-
********************************************************************************************/
SumAllPathsPathProcessor::~SumAllPathsPathProcessor()
{
	PathList.DeleteAll();
}

/********************************************************************************************

>	void SumAllPathsPathProcessor::ProcessPath(Path *pPath,
							 RenderRegion *pRender,
							 PathShape ShapePath = PATHSHAPE_PATH);

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/2/2000
	Inputs:		The path to draw, the render region and the shape path 
	Outputs:	
	Returns:	
	Purpose:	Constructor
	SeeAlso:	-
********************************************************************************************/
void SumAllPathsPathProcessor::ProcessPath(Path *pPath,
							 RenderRegion *pRender,
							 PathShape ShapePath)
{
	if (!m_bEnabled)
	{
		pRender->DrawPath(pPath, this);
		return;
	}
	
	if (m_bDrawPaths)
	{
		pRender->DrawPath(pPath, this);
	}

	// create a new element and add it to the list
	CCAttrMap *pAttrMap = CCAttrMap::MakeAttrMapFromRenderRegion(pRender);

	SumAllPathsElem * pElem = new SumAllPathsElem(pPath, pAttrMap);

	PathList.AddTail(pElem);
}



/********************************************************************************************

>	void SumAllPathsPathProcessor::InsertSumAllPathsElem (SumAllPathsElem* newElem)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/6/2000
	Inputs:		Element to be inserted
	Outputs:	
	Returns:	
	Purpose:	Allows us to insert paths directly into our list when performing convert to
				editable shapes on blended shadows, bevels, etc.
	SeeAlso:	-
********************************************************************************************/

void SumAllPathsPathProcessor::InsertSumAllPathsElem (SumAllPathsElem* newElem)
{
	PathList.AddTail(newElem);
}

///////////////////////////////////////////////////////////////////////
// SumAllPathsElem implementation

SumAllPathsElem::SumAllPathsElem(Path * pPath, CCAttrMap * pAttrMap)
{
	m_Path.Initialise(pPath->GetNumCoords());
	m_Path.CopyPathDataFrom(pPath);

	m_pAttrMap = pAttrMap;
}

SumAllPathsElem::~SumAllPathsElem()
{
	// don't need to destroy the path, just the attribute map
	m_pAttrMap->DeleteAttributes();
	delete m_pAttrMap;
}








