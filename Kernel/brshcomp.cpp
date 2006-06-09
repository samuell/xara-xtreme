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

// Brush Component and Brush Definition headers

#include "camtypes.h"
//#include "app.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "camfiltr.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "brshcomp.h"
#include "linecomp.h"
//#include "colormgr.h"
//#include "cxfrec.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "cxfrech.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "cxftags.h"
//#include "fixmem.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "layer.h"
//#include "spread.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "brushref.h"
//#include "freehand.h"
//#include "xlong.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "ppbrush.h"
#include "brshattr.h"
//#include "sgline.h"
//#include "loadbrsh.h"
#include "fileutil.h"
//#include "clipint.h"
#include "nodebldr.h"
#include "bldbrdef.h"
//#include "fillattr.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "fillval.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "lineattr.h"
#include "brshdata.h"
//#include "swfrndr.h"
#include "brshname.h"
//#include "resdll.h"
#include "nodeshad.h"
#include "nodecntr.h"
#include "nodebev.h"
//#include "group.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "opdrbrsh.h"
#include "toollist.h"
//#include "freeinfo.h"
#include "brshbeca.h"
//#include "brdlgres.h"

DECLARE_SOURCE("$Revision$");

CC_IMPLEMENT_DYNAMIC(BrushDefinition, LineDefinition)
CC_IMPLEMENT_DYNAMIC(BrushComponent, LineComponent)
CC_IMPLEMENT_DYNAMIC(BrushComponentClass, DocComponentClass)

// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW  

BOOL BrushComponent::m_bCancelNewBrush = FALSE;
String_32 BrushComponent::m_NewName = TEXT("Unnamed brush");


const INT32 MAX_TRANSP_VALUE = 255;
/********************************************************************************************

>	BrushDefinition::BrushDefinition(Node *pBrushTree);

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/2/97

	Inputs:		pBrushTree - A pointer to a Spread node which is the root of a clipart
							  subtree which should be used for this Brush definition. It
							  must obey these rules:
								* It must be a Spread
								* It must not be linked into any other parent tree structure
								* It should contain at least one ink node (or else the Brush
								  will appear "blank"). 
								* It should be attribute complete, or close thereto

	Purpose:	Constructor

	SeeAlso:	BrushComponent::AddNewBrush

********************************************************************************************/

BrushDefinition::BrushDefinition(Node* pBrushTree) 
: LineDefinition(pBrushTree)
{
	ResetMembers();
	InitialiseBrushArray(MAX_BRUSH_OBJECTS);
	if (!GenerateBrush())
	{
		// hmm, not sure what to do here, but you can detect if we failed to initialise
		// by calling IsActivated, if that fails you should delete me.
	}
	
}


/********************************************************************************************

>	BrushDefinition::BrushDefinition()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/2/97

	Purpose:	Default constructor for the purposes of inherited classes, if you just want a
				normal brushdef you should really use the constructor above.  
			
			    5/12/2000 Actually I've just read that it is a poor idea to initialise inside
				constructors, so I think that a reorganisation should occur whereby the construcotr
				should simply assign the brush tree and we should GenerateBrush after the constructor
				returns.

********************************************************************************************/

BrushDefinition::BrushDefinition()
{
	ResetMembers();
}


/********************************************************************************************

>	BrushDefinition::~BrushDefinition()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/2/97

	Purpose:	Destructor

********************************************************************************************/

BrushDefinition::~BrushDefinition()  
{ 
	for( size_t i = 0; i < m_BrushRefPtrArray.size(); i++ )
	{
		// ask the brushref to delete the attribute maps it generated
		if (m_BrushRefPtrArray[i] != NULL)
		{
		//	m_BrushRefPtrArray[i]->DeleteAttributeMapsAndAttributes();
			delete m_BrushRefPtrArray[i];
		}
	}
	m_BrushRefPtrArray.clear();

	

}



/***********************************************************************************************

>	void BrushDefinition::ResetMembers()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com> 
	Created:	6/10/99
	Inputs:     -
	Returns     -
	Purpose:	Initialises the member variables

***********************************************************************************************/

void BrushDefinition::ResetMembers()
{
	m_BrushSpacing          = 10000;
	m_BrushSpacingIncrProp  = 1.0;
	m_BrushSpacingIncrConst = 0;
	m_BrushSpacingMaxRand   = 0;
	m_BrushSpacingRandSeed  = 0;

	m_PathOffsetType      = OFFSET_NONE;
	m_PathOffsetValue     =  0;
	m_PathOffsetIncrProp  = 1.0;
	m_PathOffsetIncrConst = 0;
	m_OffsetTypeRandSeed  = 0;
	m_OffsetValueMaxRand  = 0;
	m_OffsetValueRandSeed = 0;

	m_bRotate             = TRUE;
	m_RotateAngle         = 0;
	m_RotationMaxRand     = 0;
	m_RotationRandSeed	  = 0;
	m_RotAngleIncrConst	  = 0;
	m_RotAngleIncrProp    = 1.0;
	m_RotationMaxPressure = 0;

	m_bTile               = TRUE;

	m_BrushScaling        = 1.0;
	m_BrushScalingIncr    = 1.0;
	m_BrushScalingIncrConst = 0.0;
	m_BrushScalingMaxRand   = 0;
	m_BrushScalingRandSeed  = 0;
	m_ScalingMaxPressure    = 35;
	
	m_BrushHueIncrement     = 0.0;
	m_BrushHueMaxRand       = 0;
	m_BrushHueRandSeed      = 1234;

	m_BrushSatIncrement     = 0.0;
	m_BrushSatMaxRand       = 0;
	m_BrushSatRandSeed      = 5432;	

	m_SequenceType        = SEQ_FORWARD;
	m_SequenceRandSeed    = 0;

	m_NumBrushObjects     = 0;
	m_bInitOk             = FALSE;

	m_LargestBoundingBox.MakeEmpty();

	m_Name = TEXT("Custom Brush");

	m_TimeStampPeriod = 0;

	m_BrushTransparency  = 100;
	m_TranspMaxPressure  = 0;

	m_DefaultFileID = BRUSHFILE_NONE;

	m_bActivated = FALSE;
}



/***********************************************************************************************

>	BOOL BrushDefinition::GenerateBrush()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com> 
	Created:	13/12/99
	Inputs:     -
	Returns     TRUE if successful, FALSE otherwise
	Purpose:	goes through the input spread and calls AddNewObject() for each ink object found
				We try to make some intelligent guesses about what to set the starting spacing and
				offset values depending on the nature of the nodes.  The following rules apply:
				- if there are multiple ink objects then the spacing is 1.1 * the largest dimension 
				of the largest bounding box
				- if there is only one object and it contains a positive line width spacing is as above
				- if there is only one object with no line width then spacing is 10000 millipoint
				- defaulf offset is always the largest dimension of the largest BBOX.

***********************************************************************************************/

BOOL BrushDefinition::GenerateBrush()
{
	if (m_pTree == NULL)
		return FALSE;

	// get a node that points at the first child of the layer
	Node* pNode = m_pTree->FindFirstChild();
	pNode       = pNode->FindFirstChild();
	

	AttrLineWidth* pLineWidth = NULL;
	while (pNode != NULL)
	{
		if (pNode->IsAnObject())
		{
			// it has been drawn to my attention that it is possible to end up with a brush object
			// that has no line width attribute, which leads to all kinds of problems.  So here we will
			// search for one and if we don't find one we will add one.
			((NodeRenderableInk*)pNode)->FindAppliedAttribute(CC_RUNTIME_CLASS(AttrLineWidth), (NodeAttribute**)&pLineWidth);
			if (!pLineWidth)
			{
				pLineWidth = new AttrLineWidth(pNode, LASTCHILD);
				if (pLineWidth)
				{
					pLineWidth->Value.LineWidth = 501;
					pLineWidth = NULL;
				}
			}

			// we don't want to make brushes from brushes, instead we want to convert them
			// to shapes first, so use the routine to do that.
			NodeGroup* pGroup = NULL;
			ReplaceBrushWithGroup((NodeRenderableInk*)pNode, &pGroup);
			if (pGroup != NULL)
			{
				// we got a group, so we have to delete the original node
				pGroup->AttachNode(pNode, NEXT);
				pGroup->FactorOutCommonChildAttributes();
				// unhook from the tree
				pNode->CascadeDelete();

				// swap the pointers
				Node* pDelete = pNode;
				pNode = pGroup;
				
				delete pDelete;
			}

			if (!AddNewObject((NodeRenderableInk*)pNode))
				return FALSE;
		}
		pNode = pNode->FindNext();
	}
	CalculateMaxScaling();

	m_bActivated = TRUE;
	return TRUE;

}


/***********************************************************************************************

>	BOOL BrushDefinition::ReplaceBrushWithGroup(NodeRenderableInk* pInk, NodeGroup* pGroup)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com> 
	Created:	13/12/99
	Inputs:     pInk - the inknode that we are going to generate a brush from
	Outputs:	if pInk has a brush attribute applied to it then we will generate a new group 
				from that brush
	Returns     TRUE if successful, FALSE otherwise
	Purpose:	We are no longer allowed to make brushes from brushes, because of problems
				when saving and loading.  Instead if we have a brush then we will convert
				it to shapes and use those instead.

	Note:		So you actually have to delete pInk yourself if this function outputs a group.
				I had to do that to get the recursion to work.  Still I'm sure you can handle it.
***********************************************************************************************/

BOOL BrushDefinition::ReplaceBrushWithGroup(NodeRenderableInk* pInk, NodeGroup** ppGroup)
{
	ERROR2IF(pInk == NULL, FALSE, "Invalid inputs to BrushDefinition::ReplaceBrushWithGroups");

	BOOL  ok = TRUE;
	// if we're compound then search through all our children first
	if (pInk->IsCompound())
	{
		NodeGroup* pNodeGroup = NULL;
		Node* pChild = pInk->FindFirstChild();
		Node* pDelete = NULL;		
		
		while (pChild)
		{
			if (pChild->IsAnObject())
			{
				if (ok) ok = ReplaceBrushWithGroup((NodeRenderableInk*)pChild, &pNodeGroup);
				if (pNodeGroup)
				{
					// attach the new group next to the child
					pNodeGroup->AttachNode(pChild, NEXT);

					pNodeGroup->FactorOutCommonChildAttributes();

					// unhook the child
					pChild->CascadeDelete();
					
					// we're ready to delete the original child
					pDelete =  pChild;

					// swap the pointer
					pChild = pNodeGroup;	

					delete pDelete;
				}
			}
			pChild = pChild->FindNext();
			pNodeGroup = NULL;
		}
	}

	AttrBrushType* pAttrBrush = NULL;
	pInk->FindAppliedAttribute(CC_RUNTIME_CLASS(AttrBrushType), (NodeAttribute**)&pAttrBrush);

	if (pAttrBrush && pAttrBrush->GetBrushHandle() != BrushHandle_NoBrush && ok)
	{
		//Turn the brush into a group with lots of nodepaths
		BrushBecomeAGroup BecomeA(BECOMEA_PASSBACK, CC_RUNTIME_CLASS(NodePath), NULL);
		ok = pAttrBrush->DoBecomeA(&BecomeA, pInk);

		// the brush will create a group out of itself and we want to retrieve that
		NodeGroup* pBrushGroup = BecomeA.GetNodeGroup();
		pBrushGroup->FactorOutCommonChildAttributes();
		pBrushGroup->NormaliseAttributes();
		if (pBrushGroup != NULL && ok)
		{
			*ppGroup = pBrushGroup;
		}
	}
	return ok;
}

/***********************************************************************************************

>	BOOL BrushDefinition::RegenerateBrush()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com> 
	Created:	13/12/99
	Inputs:     -
	Returns     TRUE if successful, FALSE otherwise
	Purpose:	This function deletes the existing brushrefs and recreates them from scratch from 
				m_pTree.

	Notes:		This function is necessary because of the inaccuracies in our matrix transformation
				system which make it impossible to repeatedly scale without either growing our objects.
				Therefore it is recommended that after every render where an object is scaled this function
				should be called to regenerate the brush.
				Obviously the potential for disaster is very high for instance we run out of memory
				in the middle of the operation.

***********************************************************************************************/

BOOL BrushDefinition::RegenerateBrush()
{
	/*  As we now always take copies of the brush objects, this is scheduled for demolition...

	// first we need to transform all the attributes back to where they began
	PreExportSubTree();

	// make a temporary array to hold the existing elements, as we do not wish 
	// to delete them until we are sure regeneration went ok
	CTypedPtrArray <CPtrArray, BrushRef*> TempArray;
	
	TempArray.SetSize(MAX_BRUSH_OBJECTS, -1);

	// fill the array with our existing objects
	BrushRef* pRef = NULL;
	INT32 i = 0;
	for (i = 0; i < TempArray.GetSize(); i++)
	{
		pRef = m_BrushRefPtrArray[i];
		TempArray.SetAt(i, pRef);
		m_BrushRefPtrArray.SetAt(i, NULL); // set existing pointer to null
	}

	m_NumBrushObjects = 0;

	// now remake ourselves
	if  (GenerateBrush() == TRUE)
	{
		// delete the existing brushrefs
		for (i = 0; i < TempArray.GetSize(); i++)
		{
			if (TempArray[i] != NULL)
			{
				delete TempArray[i];
			}
		}
		TempArray.RemoveAll();
		
		return TRUE;
	}

	// ok so something went wrong, not much we can do except for restore the original objects
	// and hope for the best
	ERROR3("Uh oh - Regenerate Brush failed");
	pRef = NULL;
	i= 0;
	for (i = 0; i < TempArray.GetSize(); i++)
	{
		pRef = TempArray[i];
		m_BrushRefPtrArray.SetAt(i, pRef); // set existing pointer to null
		TempArray[i] = NULL;
	}
	*/
	return TRUE;
}



/***********************************************************************************************

>	BOOL BrushDefinition::IsActivated()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com> 
	Created:	13/12/99
	Inputs:     -
	Returns     the member indicating whether this definition is active
	Purpose:	To see if this definition was initialised ok or if it was subsequently deactivated
				for some reason

***********************************************************************************************/

BOOL BrushDefinition::IsActivated()
{
	return m_bActivated;
}


/***********************************************************************************************

>	void BrushDefinition::SetActivated(BOOL Value)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com> 
	Created:	13/12/99
	Inputs:     Value - the value to set
	Returns     -
	Purpose:	To set this definition activated or not

***********************************************************************************************/

void BrushDefinition::SetActivated(BOOL Value)
{
	m_bActivated = Value;
}

/***********************************************************************************************

>	BOOL BrushDefinition::AddNewObject(NodeRenderableInk* pInkNode)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com> 
	Created:	13/12/99
	Inputs:     pInkNode - node to add
	Returns     TRUE if successful, FALSE otherwise
	Purpose:	creates a brushref object from the inknode and its attributes, then adds the
				brushref to the member array

***********************************************************************************************/

BOOL BrushDefinition::AddNewObject(NodeRenderableInk* pInkNode)
{

	if (pInkNode == NULL)
	{
		ERROR3("Ink node is NULL");
		return FALSE;
	}

	if (m_NumBrushObjects >= MAX_BRUSH_OBJECTS)
	{
		ERROR3("Cannot exceed MAX_BRUSH_OBJECTS");
		return FALSE;
	}

	BrushRef *pNewBrushRef = new BrushRef;

	if (pNewBrushRef == NULL)
	{
		ERROR3("Failed to allocate brushref");
		return FALSE;
	}
	

	
	if (!pNewBrushRef->Initialise(pInkNode))
	{
		ERROR3("Failed to initialise brushref");
		return FALSE;
	}

	m_BrushRefPtrArray[m_NumBrushObjects++] = pNewBrushRef;

	return TRUE;

}


/***********************************************************************************************

>	void BrushDefinition::InitialiseBrushArray(UINT32 NumObjects)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com> 
	Created:	6/10/99
	Inputs:		Number of objects to size the array at
	Returns:	-
	Purpose:	Clears out the m_BrushRefPtrArray if it is not empty, and sets the size 

***********************************************************************************************/

void BrushDefinition::InitialiseBrushArray(UINT32 NumObjects) 
{
	size_t				i = 0;
	while( i < m_BrushRefPtrArray.size() )
	{
		delete m_BrushRefPtrArray[i++];
	}
	m_BrushRefPtrArray.clear();

	m_BrushRefPtrArray.resize( NumObjects );

	// fill the array with NULL objects so that we can check later
	// to see if our allocations have worked
	i = 0;
	while( i < m_BrushRefPtrArray.size() )
	{
		m_BrushRefPtrArray[i++] = NULL;
	}

}


/********************************************************************************************

>	BOOL BrushDefinition::IsDifferent(BrushDefinition *pOther)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/99

	Inputs:		pOther - the Brush to compare this Brush to

	Returns:	TRUE if they're different in any way,
				FALSE if they are identical definitions

	Purpose:	Determine if 2 BrushDefinitions are considered different.
				Used when adding Brushes to the global list, so that like Brushes
				can be merged.

********************************************************************************************/

BOOL BrushDefinition::IsDifferent(LineDefinition *pOther)
{
	ERROR3IF(pOther == NULL, "Illegal NULL param");

//	String_32* pName = pOther->GetLineName();
	if (!m_Name.IsIdentical(*pOther->GetLineName()))
		return(TRUE);

	if (pOther->GetLineTree() == NULL || m_pTree == NULL)
	{
		ERROR3("BrushDefinition has not been properly initialised");
		return(TRUE);
	}

	/*
	// --- Check to see if the brush bounds are equal
	DocRect OtherBounds = ((Spread *)(pOther->GetLineTree()))->GetBoundingRect();
	DocRect Bounds = ((Spread *)m_pTree)->GetBoundingRect();
	if (Bounds != OtherBounds)
		return(TRUE);
	*/
	// --- Check the subtrees node-for-node to see if they are the same
	Node *pCurNode1 = m_pTree->FindFirstDepthFirst();
	Node *pCurNode2 = pOther->GetLineTree()->FindFirstDepthFirst();

	while (pCurNode1 != NULL && pCurNode2 != NULL)
	{
		// if they are fill attributes then we want to translate both attributes to the origin.
		// This prevents us getting duplication of brushes when we have the 
		// same thing in a different place, note that occasionally this fails due to rounding errors.
		if (pCurNode1->IsAFillAttr() && pCurNode2->IsAFillAttr() && 
			!((AttrFillGeometry*)pCurNode1)->IsAStrokeColour() && 
			!((AttrFillGeometry*)pCurNode1)->IsAStrokeTransp())
		{
			AttrFillGeometry* pFill1 = (AttrFillGeometry*)pCurNode1;
			AttrFillGeometry* pFill2 = (AttrFillGeometry*)pCurNode2;
			DocCoord* pStartPoint1 = pFill1->GetStartPoint();
			DocCoord* pStartPoint2 = pFill2->GetStartPoint();
		
			// translate them both to 0,0
			if (pStartPoint1 && pStartPoint2)
			{
				Trans2DMatrix Trans1(-pStartPoint1->x, -pStartPoint1->y);
				Trans2DMatrix Trans2(-pStartPoint2->x, -pStartPoint2->y);
				pFill1->Transform(Trans1);
				pFill2->Transform(Trans2);
				BOOL Different = FALSE;
				if (pCurNode1->IsDifferent(pCurNode2))
					Different = TRUE;

				Trans1.Invert();
				Trans2.Invert();
				pFill1->Transform(Trans1);
				pFill2->Transform(Trans2);
				if (Different == TRUE)
					return TRUE;
			}

		}
		else
			// likewise if they are an object then translate from the centre of the bounding
			// rect to the origin
		    if (pCurNode1->IsAnObject() && pCurNode2->IsAnObject())
			{
				NodeRenderableInk* pInk1 = (NodeRenderableInk*)pCurNode1;
				NodeRenderableInk* pInk2 = (NodeRenderableInk*)pCurNode2;
				DocRect BRect1 = ((NodeRenderableInk*)pCurNode1)->GetBoundingRect();
				DocRect BRect2 = ((NodeRenderableInk*)pCurNode2)->GetBoundingRect();
				DocCoord Coord1 = BRect1.Centre();
				DocCoord Coord2 = BRect2.Centre();
				Trans2DMatrix Trans1(-Coord1.x, -Coord1.y);
				Trans2DMatrix Trans2(-Coord2.x, -Coord2.y);
				pInk1->Transform(Trans1);
				pInk2->Transform(Trans2);
				BOOL Different = FALSE;
				if (pCurNode1->IsDifferent(pCurNode2))
					Different = TRUE;
				
				Trans1.Invert();
				Trans2.Invert();
				pInk1->Transform(Trans1);
				pInk2->Transform(Trans2);
				if (Different ==TRUE)
					return TRUE;
			}
		else 
		{
			// otherwise just use the regular == operator
			if (pCurNode1->IsDifferent(pCurNode2))
				return(TRUE);
		}

		// And go to the next node in both brushes
		pCurNode1 = pCurNode1->FindNextDepthFirst(m_pTree);
		pCurNode2 = pCurNode2->FindNextDepthFirst(pOther->GetLineTree());
	}

	if (!AreBrushParametersIdentical((BrushDefinition*)pOther))
		return TRUE;

	// If we did the entire search and both pointers ended up NULL simultaneously, then
	// we have an exact match
	if (pCurNode1 == NULL && pCurNode2 == NULL)
		return(FALSE);

	return(TRUE);
}


/********************************************************************************************

>	BOOL BrushDefinition::IsDifferent(BrushDefinition *pOther)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/99

	Inputs:		pOther - the Brush to compare this Brush to

	Returns:	TRUE if they're different 
				FALSE if they are identical 

	Purpose:	Compares the member variables of the two brushes

********************************************************************************************/

BOOL BrushDefinition::AreBrushParametersIdentical(BrushDefinition* pOther)
{
	if (m_BrushSpacing			!= pOther->m_BrushSpacing			||
		m_BrushSpacingIncrProp	!= pOther->m_BrushSpacingIncrProp	||
		m_BrushSpacingIncrConst != pOther->m_BrushSpacingIncrConst	||
		m_BrushSpacingMaxRand   != pOther->m_BrushSpacingMaxRand	||
		m_BrushSpacingRandSeed  != pOther->m_BrushSpacingRandSeed	||
		m_SpacingMaxPressure    != pOther->m_SpacingMaxPressure		||
		
		m_bTile					!= pOther->m_bTile					||
		m_bRotate				!= pOther->m_bRotate				||
		
		m_RotateAngle			!= pOther->m_RotateAngle			||
		m_RotAngleIncrProp		!= pOther->m_RotAngleIncrProp		||
		m_RotAngleIncrConst		!= pOther->m_RotAngleIncrConst		||
		m_RotationMaxRand		!= pOther->m_RotationMaxRand		||
		m_RotationRandSeed		!= pOther->m_RotationRandSeed		||
		m_RotationMaxPressure	!= pOther->m_RotationMaxPressure	||

		m_PathOffsetType		!= pOther->m_PathOffsetType			||
		m_PathOffsetValue		!= pOther->m_PathOffsetValue		||
		m_PathOffsetIncrProp	!= pOther->m_PathOffsetIncrProp		||
		m_PathOffsetIncrConst	!= pOther->m_PathOffsetIncrConst	||
		m_OffsetTypeRandSeed	!= pOther->m_OffsetTypeRandSeed		||
		m_OffsetValueMaxRand	!= pOther->m_OffsetValueMaxRand		||
		m_OffsetValueRandSeed	!= pOther->m_OffsetValueRandSeed	||
		
		m_BrushScaling			!= pOther->m_BrushScaling			||
		m_BrushScalingIncr		!= pOther->m_BrushScalingIncr		||
		m_BrushScalingIncrConst != pOther->m_BrushScalingIncrConst	||
		m_BrushScalingMaxRand	!= pOther->m_BrushScalingMaxRand	||
		m_BrushScalingRandSeed	!= pOther->m_BrushScalingRandSeed	||
		m_ScalingMaxPressure	!= pOther->m_ScalingMaxPressure		||

		m_BrushHueIncrement		!= pOther->m_BrushHueIncrement		||
		m_BrushHueMaxRand		!= pOther->m_BrushHueMaxRand		||
		m_BrushHueRandSeed		!= pOther->m_BrushHueRandSeed		||
		m_HueMaxPressure		!= pOther->m_HueMaxPressure			||

		m_BrushSatIncrement		!= pOther->m_BrushSatIncrement		||
		m_BrushSatMaxRand		!= pOther->m_BrushSatMaxRand		||
		m_BrushSatRandSeed		!= pOther->m_BrushSatRandSeed		||
		m_SatMaxPressure		!= pOther->m_SatMaxPressure			||

		m_SequenceType			!= pOther->m_SequenceType			||
		m_SequenceRandSeed		!= pOther->m_SequenceRandSeed		||

		m_TimeStampPeriod		!= pOther->m_TimeStampPeriod)
		return FALSE;
	else
		return TRUE;
}


/********************************************************************************************

>	BrushDefinition* BrushDefinition::Copy()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/5/2000

	Inputs:		-

	Returns:	Pointer to new brush definition if succesful, NULL if it fails
	Purpose:	Makes a copy of this definition, inknodes and all.
	
********************************************************************************************/

BrushDefinition* BrushDefinition::Copy()
{	
	// we need to make a range in order to copy the nodes to the new brush definition
	// so we need the start and end nodes of the range
	Node* pStart = NULL;
	Node* pNext = NULL;
	Node* pLast = NULL;

	// don't forget we start on a spread
	Node* pTree = GetLineTree();
	if (pTree != NULL)
		pTree = pTree->FindFirstChild();  // get the layer
	if (pTree != NULL)
			pTree = pTree->FindFirstChild();  // get the first ink object
	
	if (pTree == NULL)
	{
		ERROR3("Tree node is NULL in BrushDefinition::Copy()");
		return NULL;
	}
	
	pNext = pStart = pTree;
	while (pNext != NULL)
	{
		pNext->SetSelected(FALSE);  // we need to do this so that Range::FindFirst doesn't return NULL
		pLast = pNext;
		pNext = pLast->FindNext();
	}


	// make a range out of our subtree as thats the easiest way to copy all the nodes
	RangeControl rc;
	rc.Unselected = TRUE;
	rc.Selected = FALSE;
	Range BrushRange(pStart, pLast, rc);
	
	// make a new spread to attach our copy tree to
	Spread* pSpread = new Spread;
	if (pSpread == NULL)
	{
		ERROR3("Failed to allocate spread in BrushDefinition::Copy");
		return NULL;
	}

	Layer			   *pLayer = new Layer( pSpread, FIRSTCHILD, String_256( TEXT("Diccon did this") ) );
	if (pLayer == NULL)
	{
		ERROR3("Failed to allocate layer in BrushDefinition::Copy");
		delete pSpread;
		return NULL;
	}

	// we need to reset our attributes in the same way that we need to with exporting.
	// If we don't do this then attributes with control points end up vanishing
	PreExportSubTree();

	if (!pLayer->CopyComplexRange(BrushRange))
	{
		ERROR3("Failed to copy range in BrushDefinition::Copy");
		delete pSpread;
		delete pLayer;
		return NULL;
	}

	PostExportSubTree();

	// lets allocate ourselves a new empty definition
	BrushDefinition* pNewBrushDef = new BrushDefinition(pSpread);

	if (pNewBrushDef == NULL)
	{
		ERROR3("Failed to allocate brush definition in BrushDefinition* BrushDefinition::Copy");
		delete pSpread;
		delete pLayer;
		return NULL;
	}

	if (!pNewBrushDef->IsActivated())
	{
		ERROR3("Brush definition failed to initialise");
		delete pNewBrushDef;
		delete pSpread;
		delete pLayer;
		return NULL;
	}

	return pNewBrushDef;
}

/***********************************************************************************************

>	BOOL BrushDefinition::UsesPressure()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com> 
	Created:	16/6/2000
	Inputs:     -
	Returns     TRUE if this brush has one of its pressure variables set
	Purpose:	as above
***********************************************************************************************/

BOOL BrushDefinition::UsesPressure()
{
	if (m_ScalingMaxPressure != 0  ||
		m_RotationMaxPressure != 0 ||
		m_TranspMaxPressure != 0)
		return TRUE;

	return FALSE;
}


/***********************************************************************************************

>	static BOOL BrushDefinition::ObjectCanCreateBrush(NodeRenderableInk* pObject)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com> 
	Created:	16/6/2000
	Inputs:     pObject - the object to test
	Returns     TRUE if we are allowed to make a brush out of this object, FALSE otherwise
	Purpose:	To determine if this ink node can be made into a brush, currently returns TRUE for
				except for bevels, shadows, and contours, which are too damn awkward.
***********************************************************************************************/

BOOL BrushDefinition::ObjectCanCreateBrush(NodeRenderableInk* pObject)
{
	ERROR2IF(pObject == NULL, FALSE, "Object is NULL in BrushDefinition::ObjectCanCreateBrush");

	// only way to do it is some ugly IKO's
	if (pObject->IS_KIND_OF(NodeShadow))
		return FALSE;
	if (pObject->IS_KIND_OF(NodeContour))
		return FALSE;
	if (pObject->IS_KIND_OF(NodeBevel))
		return FALSE;

	// if its compound then check all the children
	if (pObject->IsCompound())
	{
		Node* pChild = pObject->FindFirstChild();
		while (pChild)
		{
			if (pChild->IsAnObject())
			{
				if (!ObjectCanCreateBrush((NodeRenderableInk*)pChild))
					return FALSE;
			}
			pChild = pChild->FindNext();
		}
	}

	return TRUE;

}


/*********************************************************************************************

>    BOOL BrushDefinition::StartRender()

     Author:    Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
     Created:   4/9/2000
     Inputs:    -
     Outputs:   -
     Returns:   TRUE if successful, FALSE if not.  If we return FALSE then don't even think
				about rendering with this brush.

     Purpose:   Basically due to imprecision in the arithmetic system we ended up corrupting the
				brush data if we used it too much.  So to get around this we never transform
				the original brush data, instead we make copies of it each time we want to render
				a version of this brush.
				This function generates the data copies that we need to render this brush, make 
				sure you call StopRender() to get rid of them.
**********************************************************************************************/
       
BOOL BrushDefinition::StartRender()
{
	BrushRef* pBrushRef = GetFirstBrushRef();
	BOOL ok = TRUE;
	while (pBrushRef != NULL)
	{
		if (ok)
			ok = pBrushRef->MakeCopiesForRendering();
		pBrushRef = GetNextBrushRef();
	}

	return ok;
	
}



/*********************************************************************************************

>    void BrushDefinition::StopRender()

     Author:    Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
     Created:   4/9/2000
     Inputs:    -
     Outputs:   -
     Returns:   -

     Purpose:   Call this when you have finished rendering a node with this brush, it deletes
				the copied data that we used for rendering
**********************************************************************************************/
       
void BrushDefinition::StopRender()
{
	BrushRef* pBrushRef = GetFirstBrushRef();
	while (pBrushRef != NULL)
	{
		pBrushRef->DeleteRenderCopies();
		pBrushRef = GetNextBrushRef();
	}

	return;

}

/***********************************************************************************************

>	void BrushDefinition::CalculateMaxScaling()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com> 
	Created:	13/12/99
	Inputs:     -
	Returns     -
	Purpose:	Calculates the maximum scaling value possible by dividing the maximum size value by
				the longest side of the bounding rect of the largest object
***********************************************************************************************/

void BrushDefinition::CalculateMaxScaling()
{
	DocRect BRect = GetLargestBoundingBox();
	if (BRect.IsEmpty())
	{
		//ERROR3("Bounding rect is empty in BrushDefinition::CalculateMaxScaling");
		return;
	}
	MILLIPOINT LongestSide = BRect.Height() > BRect.Width() ? BRect.Height() : BRect.Width();
	m_MaxScaling = MAX_BRUSH_SIZE  / LongestSide;

}


/***********************************************************************************************

>	double BrushDefinition::GetMaxScaling()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com> 
	Created:	13/12/99
	Inputs:     -
	Returns     the maximum scaling value
	Purpose:	as above
***********************************************************************************************/

double BrushDefinition::GetMaxScaling()
{
	return m_MaxScaling;
}

/********************************************************************************************

>	BOOL BrushDefinition::SetBrushSpacing(double Spacing) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     the spacing value to set
	Returns:	TRUE if successful, FALSE if the spacing is invalid 
	Purpose:	To set the spacing between brush objects
	SeeAlso:	-

********************************************************************************************/

BOOL BrushDefinition::SetSpacing(MILLIPOINT Spacing)
{
	if (Spacing < MIN_BRUSH_SPACING || Spacing > MAX_BRUSH_SPACING)
		return FALSE;
	
	m_BrushSpacing = Spacing;

	return TRUE;

}



/********************************************************************************************

>	MILLIPOINT BrushDefinition::GetBrushSpacing() 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Returns:	the spacing between the brush objects 
	Purpose:	As above

********************************************************************************************/

MILLIPOINT BrushDefinition::GetSpacing()
{
	return m_BrushSpacing;
}



/********************************************************************************************

>	BOOL PathProcessorBrush::SetSpacingIncrProp(double Incr) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     the spacing increment value to set
	Returns:	TRUE if we can draw 10 objects, FALSE if the spacing is invalid 
	Purpose:	To set the proportional spacing increment between brush objects
	SeeAlso:	-

********************************************************************************************/

BOOL BrushDefinition::SetSpacingIncrProp(double Incr)
{
	if (Incr <= 0)
		return FALSE;
	
	if (Incr != 1.0)
	{
		double TenIncr = pow(Incr, 10.0);
		double TenthSpacing = m_BrushSpacing * TenIncr;
		if (TenthSpacing >= MAX_BRUSH_SPACING || TenthSpacing < MIN_BRUSH_SPACING)
		{
			ERROR3("Illegal increment value");
			return FALSE;
		}
	}
	m_BrushSpacingIncrProp = Incr;
	return TRUE;
}



/********************************************************************************************

>	MILLIPOINT BrushDefinition::GetSpacingIncrProp() 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Returns:	the proportional spacing increment between the brush objects 
	Purpose:	As above

********************************************************************************************/

double BrushDefinition::GetSpacingIncrProp()
{
	return m_BrushSpacingIncrProp;
}


/********************************************************************************************

>	BOOL PathProcessorBrush::SetSpacingIncrConst(MILLIPOINT Incr) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     the constant spacing increment value to set
	Returns:	TRUE if we can draw 10 objects, FALSE if the spacing is invalid 
	Purpose:	To set the proportional spacing increment between brush objects
	SeeAlso:	-

********************************************************************************************/

BOOL BrushDefinition::SetSpacingIncrConst(MILLIPOINT Incr)
{
	if (Incr <= 0)
		return TRUE;

	MILLIPOINT TenthSpacing = 10 * Incr;
	if (TenthSpacing >= MAX_BRUSH_SPACING || TenthSpacing < MIN_BRUSH_SPACING)
	{
		ERROR3("Illegal increment value");
		return FALSE;
	}
	m_BrushSpacingIncrConst = Incr;
	return TRUE;
}


/********************************************************************************************

>	MILLIPOINT BrushDefinition::GetSpacingIncrConst() 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Returns:	the constant spacing increment between the brush objects 
	Purpose:	As above

********************************************************************************************/

MILLIPOINT BrushDefinition::GetSpacingIncrConst()
{
	return m_BrushSpacingIncrConst;
}



/********************************************************************************************

>	UINT32 BrushDefinition::GetSpacingMaxRand() 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Returns:	the maximum percentage randomness to be applied to spacing 
	Purpose:	As above

********************************************************************************************/

UINT32 BrushDefinition::GetSpacingMaxRand()
{
	return m_BrushSpacingMaxRand;
}


/********************************************************************************************

>	BOOL BrushDefinition::SetSpacingMaxRand(UINT32 Value) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs::	the maximum percentage randomness to be applied to spacing 
	Returns:	TRUE if Value is within the legal limits, otherwise FALSE
	Purpose:	As above

********************************************************************************************/

BOOL BrushDefinition::SetSpacingMaxRand(UINT32 Value)
{
	if (Value < MIN_BRUSH_RAND || Value > MAX_BRUSH_RAND)
		return FALSE;
	
	m_BrushSpacingMaxRand = Value;
	return TRUE;
}


/********************************************************************************************

>	UINT32 BrushDefinition::GetSpacingRandSeed() 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Returns:	the seed used to determine the sequence of random numbers for spacing
	Purpose:	As above

********************************************************************************************/

UINT32 BrushDefinition::GetSpacingRandSeed()
{
	return m_BrushSpacingRandSeed;
}


/********************************************************************************************

>	void BrushDefinition::SetSpacingRandSeed(UINT32 Seed) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:		the seed to use for our spacing RNG
	Returns:	-
	Purpose:	As above

********************************************************************************************/

void BrushDefinition::SetSpacingRandSeed(UINT32 Seed)
{
	m_BrushSpacingRandSeed=Seed; // "=Seed" added by AMB 2006-01-06
}

/********************************************************************************************

>	PathOffset BrushDefinition::GetPathOffsetType() 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     -
	Returns:	The type of pathoffset this brush is using 
	Purpose:	as above
	SeeAlso:	-

********************************************************************************************/

PathOffset BrushDefinition::GetPathOffsetType()
{
	return m_PathOffsetType;
}



/********************************************************************************************

>	void BrushDefinition::SetPathOffsetType(PathOffset Offset) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs		Offset - the offset type to set
	Returns:	- 
	Purpose:	to set the offset type for this brush
********************************************************************************************/
void BrushDefinition::SetPathOffsetType(PathOffset Offset)
{
	m_PathOffsetType = Offset;
}


/********************************************************************************************

>	MILLIPOINT BrushDefinition::GetPathOffsetValue() 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     -
	Returns:	The value of the path offset used by this brush 
	Purpose:	as above
	SeeAlso:	-

********************************************************************************************/

MILLIPOINT BrushDefinition::GetPathOffsetValue()
{
	return m_PathOffsetValue;
}



/********************************************************************************************

>	void BrushDefinition::SetPathOffsetValue(MILLIPOINT Value) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs		Value - the offset value to set
	Returns:	- 
	Purpose:	to set the offset distance for this brush
********************************************************************************************/
void BrushDefinition::SetPathOffsetValue(MILLIPOINT Value)
{
	m_PathOffsetValue = Value;
}


/********************************************************************************************

>	MILLIPOINT BrushDefinition::GetPathOffsetIncrConst() 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     -
	Returns:	The value of the constant path offset increment used by this brush 
	Purpose:	as above
	SeeAlso:	-

********************************************************************************************/

MILLIPOINT BrushDefinition::GetPathOffsetIncrConst()
{
	return m_PathOffsetIncrConst;
}



/********************************************************************************************

>	BOOL BrushDefinition::SetPathOffsetIncrConst(MILLIPOINT Value) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs		Value - the offset increment value to set
	Returns:	- 
	Purpose:	to set the offset distance for this brush
********************************************************************************************/

BOOL BrushDefinition::SetPathOffsetIncrConst(MILLIPOINT Value)
{
	if (Value < 0 || Value > MAX_BRUSH_SPACING)
		return FALSE;

	m_PathOffsetIncrConst = Value;
	return TRUE;
}


/********************************************************************************************

>	double BrushDefinition::GetPathOffsetIncrProp() 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     -
	Returns:	The value of the proportional path offset increment used by this brush 
	Purpose:	as above
	SeeAlso:	-

********************************************************************************************/

double BrushDefinition::GetPathOffsetIncrProp()
{
	return m_PathOffsetIncrProp;
}



/********************************************************************************************

>	BOOL BrushDefinition::SetPathOffsetIncrProp(double Incr) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs		incr - the proportional offset increment value to set
	Returns:	- 
	Purpose:	to set the offset distance for this brush
********************************************************************************************/

BOOL BrushDefinition::SetPathOffsetIncrProp(double Incr)
{
	if (Incr < 0 )
		return FALSE;
	// test to see if 10 objects takes us to the limit
	double TenIncr = pow(Incr, 10.0);
	if ((m_PathOffsetValue * TenIncr) >= MAX_BRUSH_SPACING)
		return FALSE;

	m_PathOffsetIncrProp = Incr;
	return TRUE;
}


/********************************************************************************************

>	UINT32 BrushDefinition::GetOffsetValueMaxRand() 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Returns:	the maximum percentage randomness to be applied to Offset
	Purpose:	As above

********************************************************************************************/

UINT32 BrushDefinition::GetOffsetValueMaxRand()
{
	return m_OffsetValueMaxRand;
}


/********************************************************************************************

>	BOOL BrushDefinition::SetOffsetValueMaxRand(UINT32 Value) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs::	the maximum percentage randomness to be applied to Offset
	Returns:	TRUE if Value is within the legal limits, otherwise FALSE
	Purpose:	As above

********************************************************************************************/

BOOL BrushDefinition::SetOffsetValueMaxRand(UINT32 Value)
{
	if (Value < MIN_BRUSH_RAND || Value > MAX_BRUSH_RAND)
		return FALSE;
	
	m_OffsetValueMaxRand = Value;
	return TRUE;
}


/********************************************************************************************

>	UINT32 BrushDefinition::GetOffsetValueRandSeed() 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Returns:	the seed used to determine the sequence of random numbers for offset value
	Purpose:	As above

********************************************************************************************/

UINT32 BrushDefinition::GetOffsetValueRandSeed()
{
	return m_OffsetValueRandSeed;
}


/********************************************************************************************

>	BOOL BrushDefinition::SetOffsetValueRandSeed(UINT32 Value) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/9
	Inputs	:	the seed used to determine the sequence of random numbers for offset value
	Returns:	TRUE if Value is valid, which it always is at the moment
	Purpose:	As above
				
********************************************************************************************/

void BrushDefinition::SetOffsetValueRandSeed(UINT32 Value)
{
	m_OffsetValueRandSeed = Value;
	
}


/********************************************************************************************

>	UINT32 BrushDefinition::GetOffsetTypeRandSeed() 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Returns:	the seed used to determine the sequence of random numbers for offset value
	Purpose:	As above

********************************************************************************************/

UINT32 BrushDefinition::GetOffsetTypeRandSeed()
{
	return m_OffsetTypeRandSeed;
}


/********************************************************************************************

>	BOOL BrushDefinition::SetOffsetTypeRandSeed(UINT32 Value) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/9
	Inputs	:	the seed used to determine the sequence of random numbers for offset value
	Returns:	TRUE if Value is valid, which it always is at the moment
	Purpose:	As above
				
********************************************************************************************/

void BrushDefinition::SetOffsetTypeRandSeed(UINT32 Value)
{
	m_OffsetValueRandSeed = Value;

}


/********************************************************************************************

>	void BrushDefinition::SetTiling(BOOL Value) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     whether or not the brush tiles
	Returns:	-
	Purpose:	as above
	SeeAlso:	-

********************************************************************************************/

void BrushDefinition::SetTiling(BOOL Value)
{
	m_bTile = Value;
}



/********************************************************************************************

>	BOOL BrushDefinition::GetTiling() 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     
	Returns:	whether or not the brush tiles
	Purpose:	as above
	SeeAlso:	-

********************************************************************************************/

BOOL BrushDefinition::IsTiled()
{
	return m_bTile;
}



/********************************************************************************************

>	BOOL BrushDefinition::SetRotationAngle(double Angle) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     the angle of rotation to set
	Returns:	TRUE if the angle is within the stated bounds
	Purpose:	as above
	SeeAlso:	-

********************************************************************************************/

BOOL BrushDefinition::SetRotationAngle(double Angle)
{
/*	if ((Angle <= MIN_ANGLE) || (Angle >= MAX_ANGLE))
	{
		ERROR3("Invalid angle");
		return FALSE;
		
	}*/
	m_RotateAngle = Angle;
	return TRUE;
}

/********************************************************************************************

>	double BrushDefinition::GetRotationAngle() 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     -
	Returns:	the angle of rotation that has been set
	Purpose:	as above
	SeeAlso:	-

********************************************************************************************/

double BrushDefinition::GetRotationAngle()
{
	return m_RotateAngle;
}


/********************************************************************************************

>	BOOL BrushDefinition::SetRotAngleIncrConst(double Angle) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     the incremental angle of rotation to set
	Returns:	TRUE if the angle is within the stated bounds
	Purpose:	as above
	SeeAlso:	-

********************************************************************************************/

BOOL BrushDefinition::SetRotationIncrConst(double Angle)
{
	m_RotAngleIncrConst = Angle;

	return TRUE;
}

/********************************************************************************************

>	double BrushDefinition::GetRotAngleIncrConst() 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     -
	Returns:	the increment to the angle of rotation 
	Purpose:	as above
	SeeAlso:	-

********************************************************************************************/

double BrushDefinition::GetRotationIncrConst()
{
	return m_RotAngleIncrConst;
}


/********************************************************************************************

>	BOOL BrushDefinition::SetRotAngleIncrProp(double Angle) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     the incremental angle of rotation to set
	Returns:	TRUE if the angle is within the stated bounds
	Purpose:	as above
	SeeAlso:	-

********************************************************************************************/

BOOL BrushDefinition::SetRotationIncrProp(double Value)
{
	m_RotAngleIncrProp = Value;

	return TRUE;
}

/********************************************************************************************

>	double BrushDefinition::GetRotAngleIncrProp() 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     -
	Returns:	the increment to the angle of rotation 
	Purpose:	as above
	SeeAlso:	-

********************************************************************************************/

double BrushDefinition::GetRotationIncrProp()
{
	return m_RotAngleIncrProp;
}

/********************************************************************************************

>	UINT32 BrushDefinition::GetRotationMaxRand() 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     -
	Returns:	the maximum amount of randomness to be applied to the rotation angle
	Purpose:	as above
	SeeAlso:	-

********************************************************************************************/

UINT32 BrushDefinition::GetRotationMaxRand()
{
	return m_RotationMaxRand;
}


/********************************************************************************************

>	BOOL BrushDefinition::SetRotationMaxRand(UINT32 Value) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     the maximum amount of randomness (as a %) to be applied to rotation
	Returns:	-
	Purpose:	as above
	SeeAlso:	-

********************************************************************************************/

BOOL BrushDefinition::SetRotationMaxRand(UINT32 Value)
{
	m_RotationMaxRand = Value;
	return TRUE;
}


/********************************************************************************************

>	UINT32 BrushDefinition::GetRotationRandSeed() 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     -
	Returns:	the  seed to use to generate the  rotation RNG
	Purpose:	as above
	SeeAlso:	-

********************************************************************************************/

UINT32 BrushDefinition::GetRotationRandSeed()
{
	return m_RotationRandSeed;
}



/********************************************************************************************

>	BOOL BrushDefinition::SetRotationRandSeed(UINT32 Seed) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     the seed to use
	Returns:	-
	Purpose:	as above
	SeeAlso:	-

********************************************************************************************/

void BrushDefinition::SetRotationRandSeed(UINT32 Seed)
{
	m_RotationRandSeed = Seed;
}

/********************************************************************************************

>	UINT32 BrushDefinition::GetRotationMaxPressure() 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     -
	Returns:	the  seed to use to generate the  rotation RNG
	Purpose:	as above
	SeeAlso:	-

********************************************************************************************/

UINT32 BrushDefinition::GetRotationMaxPressure()
{
	return m_RotationMaxPressure;
}



/********************************************************************************************

>	BOOL BrushDefinition::SetRotationMaxPressure(UINT32 Seed) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     the seed to use
	Returns:	-
	Purpose:	as above
	SeeAlso:	-

********************************************************************************************/

BOOL BrushDefinition::SetRotationMaxPressure(UINT32 Value)
{
	if (Value < MIN_PRESSURE_EFFECT || Value > MAX_PRESSURE_EFFECT)
		return FALSE;
	m_RotationMaxPressure = Value;
	return TRUE;
}


/********************************************************************************************

>	BOOL BrushDefinition::IsRotated() 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     
	Returns:	whether or not the brush objects are rotated
	Purpose:	as above
	SeeAlso:	-

********************************************************************************************/

BOOL BrushDefinition::IsRotated()
{
	return m_bRotate;
}


/********************************************************************************************

>	void BrushDefinition::SetRotation(BOOL Value) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     whether or not the brush rotates
	Returns:	-
	Purpose:	as above
	SeeAlso:	-

********************************************************************************************/

void BrushDefinition::SetRotated(BOOL Value)
{
	m_bRotate = Value;
}



/********************************************************************************************

>	bool BrushDefinition::SetScaling(double Scale) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     the scale factor to set
	Returns:	TRUE if Scale is within the permitted bounds
	Purpose:	to set the member that determine to what scale of the original object each 
				brush object is drawn
	SeeAlso:	-

********************************************************************************************/

BOOL BrushDefinition::SetBrushScaling(double Scale)
{
	if (Scale <= MIN_BRUSH_SCALE || Scale >= MAX_BRUSH_SCALE)
		return FALSE;
	
	m_BrushScaling = Scale;
	return TRUE;
}


/********************************************************************************************

>	double BrushDefinition::GetBrushScaling() 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     -
	Returns:	the scaling factor to use when drawing the objects
	Purpose:	as above
	SeeAlso:	-

********************************************************************************************/

double BrushDefinition::GetBrushScaling()
{
	return m_BrushScaling;
}


/********************************************************************************************

>	double BrushDefinition::GetBrushScalingIncr() 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     -
	Returns:	the scaling factor increment to use when drawing the objects
	Purpose:	as above
	SeeAlso:	-

********************************************************************************************/

double BrushDefinition::GetBrushScalingIncr()
{
	return m_BrushScalingIncr;
}



/********************************************************************************************

>	bool BrushDefinition::SetBrushScalingincr(double incr) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     the scale factor increment to set
	Returns:	TRUE unless incr is negative
	Purpose:	to set the member that determine the increment to scaling applied
				to each brush object
				
	SeeAlso:	-

********************************************************************************************/

BOOL BrushDefinition::SetBrushScalingIncr(double Incr)
{
	if (Incr <= 0)
	{
		ERROR3("Negative scaling increment");
		return FALSE;
	}

	m_BrushScalingIncr = Incr;
	return TRUE;
}


/********************************************************************************************

>	double BrushDefinition::GetBrushScalingIncrConst() 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     -
	Returns:	the scaling factor increment to use when drawing the objects
	Purpose:	as above
	SeeAlso:	-

********************************************************************************************/

double BrushDefinition::GetBrushScalingIncrConst()
{
	return m_BrushScalingIncrConst;
}



/********************************************************************************************

>	bool BrushDefinition::SetBrushScalingincrConst(INT32 Incr) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     the scale factor increment to set
	Returns:	TRUE unless Incr >= 100
	Purpose:	to set the member that determine the increment to scaling applied
				to each brush object
				
	SeeAlso:	-

********************************************************************************************/

BOOL BrushDefinition::SetBrushScalingIncrConst(double Incr)
{
	if (abs((INT32)Incr) >= 100)
	{
		return FALSE;
	}
	m_BrushScalingIncrConst = Incr;
	return TRUE;
}

/********************************************************************************************

>	UINT32 BrushDefinition::GetScalingMaxRand() 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Returns:	the maximum percentage randomness to be applied to Scaling 
	Purpose:	As above

********************************************************************************************/

UINT32 BrushDefinition::GetScalingMaxRand()
{
	return m_BrushScalingMaxRand;
}


/********************************************************************************************

>	BOOL BrushDefinition::SetScalingMaxRand(UINT32 Value) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs::	the maximum percentage randomness to be applied to Scaling 
	Returns:	TRUE if Value is within the legal limits, otherwise FALSE
	Purpose:	As above

********************************************************************************************/

BOOL BrushDefinition::SetScalingMaxRand(UINT32 Value)
{
	if (Value < MIN_BRUSH_RAND || Value > MAX_BRUSH_RAND)
		return FALSE;
	
	m_BrushScalingMaxRand = Value;
	return TRUE;
}


/********************************************************************************************

>	UINT32 BrushDefinition::GetScalingRandSeed() 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Returns:	the seed used to determine the sequence of random numbers for Scaling
	Purpose:	As above

********************************************************************************************/

UINT32 BrushDefinition::GetScalingRandSeed()
{
	return m_BrushScalingRandSeed;
}


/********************************************************************************************

>	void BrushDefinition::SetScalingRandSeed(UINT32 Seed) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:		the seed to use for our scaling RNG
	Returns:	-
	Purpose:	As above

********************************************************************************************/

void BrushDefinition::SetScalingRandSeed(UINT32 Seed)
{
	m_BrushScalingRandSeed = Seed;
}


/********************************************************************************************

>	UINT32 BrushDefinition::GetScalingMaxPressure() 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Returns:	the maximum percentage effect that pressure will have on Scaling 
	Purpose:	As above

********************************************************************************************/

UINT32 BrushDefinition::GetScalingMaxPressure()
{
	return m_ScalingMaxPressure;
}


/********************************************************************************************

>	BOOL BrushDefinition::SetScalingMaxPressure(UINT32 Pressure) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:		the maximum percentage effect that pressure will have on Scaling 
	Returns:	-
	Purpose:	As above

********************************************************************************************/

BOOL BrushDefinition::SetScalingMaxPressure(UINT32 Max)
{
	if (Max < MIN_PRESSURE_EFFECT || Max > MAX_PRESSURE_EFFECT)
		return FALSE;
	m_ScalingMaxPressure = Max;
	return TRUE;
}



/********************************************************************************************

>	double BrushDefinition::GetHueIncrement()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     -
	Returns:	the hue increment for this brush
	Purpose:	as above
	SeeAlso:	-

********************************************************************************************/

double BrushDefinition::GetHueIncrement()
{
	return m_BrushHueIncrement;
}


/********************************************************************************************

>	double BrushDefinition::GetHueIncrement()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     the hue increment for this brush
	Returns:	-
	Purpose:	as above
	SeeAlso:	-

********************************************************************************************/

BOOL BrushDefinition::SetHueIncrement(double Incr)
{
	m_BrushHueIncrement = Incr;
	return TRUE;
}


/********************************************************************************************

>	UINT32 BrushDefinition::GetHueMaxRand()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     -
	Returns:	the hue increment random amount for this brush
	Purpose:	as above
	SeeAlso:	-

********************************************************************************************/

UINT32 BrushDefinition::GetHueMaxRand()
{
	return m_BrushHueMaxRand;
}



/********************************************************************************************

>	BOOL BrushDefinition::SetHueMaxRand(UINT32 Rand)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     the hue increment random amount for this brush
	Returns:	-
	Purpose:	as above
	SeeAlso:	-

********************************************************************************************/

BOOL BrushDefinition::SetHueMaxRand(UINT32 Rand)
{
	m_BrushHueMaxRand = Rand;
	return TRUE;
}


/********************************************************************************************

>	UINT32 BrushDefinition::GetHueRandSeed()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     -
	Returns:	the hue increment random amount for this brush
	Purpose:	as above
	SeeAlso:	-

********************************************************************************************/

UINT32 BrushDefinition::GetHueRandSeed()
{
	return m_BrushHueRandSeed;
}



/********************************************************************************************

>	BOOL BrushDefinition::SetHueRandSeed(UINT32 Rand)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     the hue increment random amount for this brush
	Returns:	-
	Purpose:	as above
	SeeAlso:	-

********************************************************************************************/

BOOL BrushDefinition::SetHueRandSeed(UINT32 Rand)
{
	m_BrushHueRandSeed = Rand;
	return TRUE;
}


/********************************************************************************************

>	double BrushDefinition::GetSatIncrement()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     -
	Returns:	the Saturation increment for this brush
	Purpose:	as above
	SeeAlso:	-

********************************************************************************************/

double BrushDefinition::GetSatIncrement()
{
	return m_BrushSatIncrement;
}


/********************************************************************************************

>	double BrushDefinition::GetSatIncrement()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     the Saturation increment for this brush
	Returns:	-
	Purpose:	as above
	SeeAlso:	-

********************************************************************************************/

BOOL BrushDefinition::SetSatIncrement(double Incr)
{
	m_BrushSatIncrement = Incr;
	return TRUE;
}


/********************************************************************************************

>	UINT32 BrushDefinition::GetSatMaxRand()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     -
	Returns:	the Saturation increment random amount for this brush
	Purpose:	as above
	SeeAlso:	-

********************************************************************************************/

UINT32 BrushDefinition::GetSatMaxRand()
{
	return m_BrushSatMaxRand;
}



/********************************************************************************************

>	BOOL BrushDefinition::SetSatMaxRand(UINT32 Rand)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     the Saturation increment random amount for this brush
	Returns:	-
	Purpose:	as above
	SeeAlso:	-

********************************************************************************************/

BOOL BrushDefinition::SetSatMaxRand(UINT32 Rand)
{
	m_BrushSatMaxRand = Rand;
	return TRUE;
}


/********************************************************************************************

>	UINT32 BrushDefinition::GetSatRandSeed()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     -
	Returns:	the Saturation increment random amount for this brush
	Purpose:	as above
	SeeAlso:	-

********************************************************************************************/

UINT32 BrushDefinition::GetSatRandSeed()
{
	return m_BrushSatRandSeed;
}



/********************************************************************************************

>	BOOL BrushDefinition::SetSatRandSeed(UINT32 Rand)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     the Saturation increment random amount for this brush
	Returns:	-
	Purpose:	as above
	SeeAlso:	-

********************************************************************************************/

BOOL BrushDefinition::SetSatRandSeed(UINT32 Rand)
{
	m_BrushSatRandSeed = Rand;
	return TRUE;
}



/********************************************************************************************

>	INT32 BrushDefinition::GetBrushTransparency()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     Value - the value to set
	Returns:	our transparency value 
	Purpose:	as above
	SeeAlso:	-

********************************************************************************************/

INT32 BrushDefinition::GetBrushTransparency()
{
	return m_BrushTransparency;
}

/********************************************************************************************

>	BOOL BrushDefinition::SetBrushTransparency(INT32 Value)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     Value - the value to set
	Returns:	our transparency value (-1 indicates unused)
	Purpose:	as above
	SeeAlso:	-

********************************************************************************************/

BOOL BrushDefinition::SetBrushTransparency(INT32 Value)
{
	if (Value > MAX_TRANSP_VALUE)
		return FALSE;

	m_BrushTransparency = Value;
	return TRUE;
}


/********************************************************************************************

>	BOOL BrushDefinition::SetTransparencyPressure(UINT32 Value)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     Value - the value to set
	Returns:	
	Purpose:	Sets the extent to which pressure affects transparency
	SeeAlso:	-

********************************************************************************************/

BOOL BrushDefinition::SetTransparencyPressure(UINT32 Value)
{
	m_TranspMaxPressure = Value;
	return TRUE;
}


/********************************************************************************************

>	BOOL BrushDefinition::GetTransparencyPressure(UINT32 Value)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     Value - the value to set
	Returns:	
	Purpose:	Sets the extent to which pressure affects transparency
	SeeAlso:	-

********************************************************************************************/

UINT32 BrushDefinition::GetTransparencyPressure()
{
	return m_TranspMaxPressure;
}


/********************************************************************************************

>	SequenceType BrushDefinition::GetSequenceType() 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     -
	Returns:	the type of sequence we are using for mutliple ink objects
	Purpose:	as above
	SeeAlso:	-

********************************************************************************************/

SequenceType BrushDefinition::GetSequenceType()
{
	return m_SequenceType;
}


/********************************************************************************************

>	void BrushDefinition::SetSequenceType(SequenceType Type) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     the type of sequence to use
	Returns:	-
	Purpose:	as above
	SeeAlso:	-

********************************************************************************************/

void BrushDefinition::SetSequenceType(SequenceType Type)
{
	m_SequenceType = Type;
}


/********************************************************************************************

>	void BrushDefinition::SetSequenceSeed(UINT32 Seed) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     the random seed to use for sequences
	Returns:	-
	Purpose:	as above
	SeeAlso:	-

********************************************************************************************/

void BrushDefinition::SetSequenceSeed(UINT32 Seed)
{
	m_SequenceRandSeed = Seed;
}


/********************************************************************************************

>	UINT32 BrushDefinition::GetSequenceSeed) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     -
	Returns:	the seed we are using to generate random sequences
	Purpose:	as above
	SeeAlso:	-

********************************************************************************************/

UINT32 BrushDefinition::GetSequenceSeed()
{
	return m_SequenceRandSeed;
}

/********************************************************************************************

>	UINT32 BrushDefinition::GetNumBrushObjects() 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     -
	Returns:	the number of brush objects used by this brush
	Purpose:	as above
	SeeAlso:	-

********************************************************************************************/

UINT32 BrushDefinition::GetNumBrushObjects() const
{
	return m_NumBrushObjects;
}


/********************************************************************************************

>	UINT32 BrushDefinition::GetBrushFileID() 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     -
	Returns:	the ID of the default file that this brush was created from (if there is one)
	Purpose:	as above
	SeeAlso:	-

********************************************************************************************/

UINT32 BrushDefinition::GetBrushFileID()
{
	return m_DefaultFileID;
}

/********************************************************************************************

>	void BrushDefinition::SetBrushFileID(UINT32 ID)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     ID - the ID of the default file that this file is being created from
	Returns:	-
	Purpose:	as above, this should only be called as part of the start-up process.
********************************************************************************************/

void BrushDefinition::SetBrushFileID(UINT32 ID)
{
	m_DefaultFileID = ID;
}

/********************************************************************************************

>	BrushRef* BrushDefinition::GetFirstBrushRef() 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     -
	Returns:	the first in the brushref array
	Purpose:	as above
	SeeAlso:	-

********************************************************************************************/

BrushRef* BrushDefinition::GetFirstBrushRef()
{
	if( m_BrushRefPtrArray.empty() )
	{
		ERROR3("The array is empty!");
		return NULL;
	}
	m_LastBrushRefRetrieved = 0;
	return m_BrushRefPtrArray[0];
}



/********************************************************************************************

>	BrushRef* BrushDefinition::GetNextBrushRef() 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     -
	Returns:	the next element in the brushref array, as kept track of by 
				m_LastBrushRefRetrieved
	Purpose:	as above
	SeeAlso:	-

********************************************************************************************/

BrushRef* BrushDefinition::GetNextBrushRef()
{
	// have we hit the end?
	if( m_LastBrushRefRetrieved >= m_NumBrushObjects - 1 )
		return NULL;

	// quick check so we don't violate
	if( m_LastBrushRefRetrieved >= (UINT32)m_BrushRefPtrArray.size() - 1 )
	{
		ERROR3("Overran the array");
		return NULL;
	}
	return m_BrushRefPtrArray[++m_LastBrushRefRetrieved];
}


/********************************************************************************************
0
>	BrushRef* BrushDefinition::GetBrushRef(UINT32 Index) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     Index - the array element to get
	Returns:	the brushref at position Index in the array, or NULL if the index is invalid
	Purpose:	as above
	SeeAlso:	-

********************************************************************************************/

BrushRef* BrushDefinition::GetBrushRef(UINT32 Index)
{
	// check that the index is ok
	if( Index > m_NumBrushObjects || Index > (UINT32)( m_BrushRefPtrArray.size() - 1 ) )
	{
		ERROR3("Invalid index");
		return NULL;
	}

	// update our member
	m_LastBrushRefRetrieved = Index;
	return m_BrushRefPtrArray[Index];
}


/********************************************************************************************

>	BOOL BrushDefinition::BrushContainsGroup() 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     -
	Returns:	TRUE, if the brush is made from one or more groups, FALSE if not
	Purpose:	as above
	SeeAlso:	-

********************************************************************************************/

BOOL BrushDefinition::BrushContainsGroup()
{
	for (UINT32 i = 0; i < m_NumBrushObjects; i++)
	{
		if (m_BrushRefPtrArray[i]->GetNumBlendPaths() > 1)
			return TRUE;
	}
	return FALSE;
}

/********************************************************************************************

>	DocRect BrushDefinition::GetLargestBoundingBox() 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     -
	Returns:	the largest bounding box of all the objects in this definition
	Purpose:	as above
	SeeAlso:	-

********************************************************************************************/

DocRect BrushDefinition::GetLargestBoundingBox()
{
	// if the member is not empty then we have done this search so return it now
	if (!m_LargestBoundingBox.IsEmpty())
		return m_LargestBoundingBox;

	// otherwise search through the brushrefs
	BrushRef* pBrushRef = GetFirstBrushRef();
	DocRect   BRect;
	DocRect   BiggestRect;
	XLONG	  BiggestArea = 0;
	while (pBrushRef != NULL)
	{	
		BRect = pBrushRef->GetBoundingRect();
		XLONG RectHeight = BRect.Height();
		XLONG RectWidth = BRect.Width();
		XLONG ThisRectArea = RectHeight * RectWidth;
		
		if (ThisRectArea > BiggestArea)
		{
			BiggestRect = BRect;
			BiggestArea = ThisRectArea;
			
		}
		pBrushRef = GetNextBrushRef();

	}
	m_LargestBoundingBox = BiggestRect;
	return BiggestRect;
}



/********************************************************************************************

>	DocRect BrushDefinition::GetLargestPossibleRect(BOOL AdjustForLineWidth = FALSE)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     AdjustForLineWidth - do we include scaling to the current line width in our
				calculations?
	Returns:	the largest bounding box of all the objects in this definition, plus adjustments
				for variations in scale
	
	Purpose:	To find out the largest possible bounding box that this brush can achieve, this we
				get the largest ink rect and then perform the maximum scaling, rotation etc.
				Note that ScalingIncrConst will be will be excluded from this test as we will
				never be able to know what affect it will have in advance of drawing the path.

	Note:		Only use this function to determine the size of the bounding box, the location
				information should not be used.
	SeeAlso:	-

********************************************************************************************/

DocRect BrushDefinition::GetLargestPossibleRect(BOOL AdjustForLineWidth)
{
	DocRect BRect = GetLargestBoundingBox();
	
	if (BRect.IsEmpty())  // just quit now
		return BRect;
	
	double Scale = 1.0;

PORTNOTE("other","Removed OpDrawBrush usage")
#ifndef EXCLUDE_FROM_XARALX
	MILLIPOINT LineWidth = GetDefaultLineWidth(TRUE);
	
	// if we are including the scale to linewidth then work that out here
	if (AdjustForLineWidth)
	{
		MILLIPOINT CurrentLineWidth = OpDrawBrush::GetCurrentLineWidthIfNotDefault();
		if (CurrentLineWidth != -1 && CurrentLineWidth > 501) // -1 indicates the default linewidth, hence no scaling
		{
			Scale = (double)CurrentLineWidth / (double)LineWidth;
			// otherwise use the current line width
			LineWidth = CurrentLineWidth;
		}
	}
#endif
	
	// increase the size if we have randomness or pressure
	UINT32 MaxRand = GetScalingMaxRand();
	double dRand = ((double)MaxRand * 0.01) + 1.0;
	UINT32 MaxPressure = GetScalingMaxPressure();
	double dPressure = ((double)MaxPressure * 0.01) + 1.0;
	
	Scale *= dRand;
	Scale *= dPressure;

	if (Scale != 1.0)
	{
		double Height = (double)BRect.Height() * Scale;
		double Width = (double)BRect.Width() * Scale;

		BRect.lo.x = 0;
		BRect.hi.x = (MILLIPOINT)Height;
		BRect.lo.y = 0;
		BRect.hi.y = (MILLIPOINT)Width;
	}
//	DocRect BRect(0, 0, LineWidth* 1.1, LineWidth* 1.1);

	// as the shape might get rotated we make sure the bitmap is big enough to cope
	// so make a temporary rect, rotate it and expand it, and union it with the original
	if (m_bRotate || m_RotateAngle || m_RotAngleIncrConst || m_RotationMaxRand)
	{
		DocRect TempRect = BRect;
		BrushAttrValue::RotateBounds(90, &TempRect);
		BRect = BRect.Union(TempRect);
		TempRect = BRect;
		BrushAttrValue::RotateBounds(45, &TempRect);
		BRect = BRect.Union(TempRect);
	}
	
	
	return BRect;
}

/********************************************************************************************

>	double BrushDefinition::GetTimeStampingPeriod() 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     -
	Returns:	the time stamping period for this brush, if zero then timestamping is disabled
	Purpose:	as above
	SeeAlso:	-

********************************************************************************************/

double BrushDefinition::GetTimeStampingPeriod()
{
	return m_TimeStampPeriod;
}



/********************************************************************************************

>	BOOL BrushDefinition::SetTimeStampingPeriod(double Period) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     time period to set
	Returns:	TRUE if period is valid, else FALSE
	Purpose:	as above
	SeeAlso:	-

********************************************************************************************/

BOOL BrushDefinition::SetTimeStampingPeriod(double Period)
{
	if (Period < MIN_TIMESTAMP || Period > MAX_TIMESTAMP)
		return FALSE;

	m_TimeStampPeriod = Period;
	return TRUE;
}

/********************************************************************************************

>	MILLIPOINT BrushDefinition::GetDefaultLineWidth() 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     -
	Returns:	the height of the largest bounding box, to be used as a default line width
	Purpose:	as above
	SeeAlso:	-

********************************************************************************************/

MILLIPOINT BrushDefinition::GetDefaultLineWidth(BOOL IgnorePressure)
{
	DocRect BRect = GetLargestBoundingBox();
	MILLIPOINT Height = BRect.Height();
	MILLIPOINT Width = BRect.Width();
//	double dHeight = (double)Height;
//	double dWidth = (double)Width;

	double ScaleFactor = 1.0;

	// firstly, if we are rotated, or have random rotation, then use the longest side
//	if (m_RotationMaxRand > 0 || m_RotateAngle != 0.0 || !m_bRotate)
	{
		if (Width > Height)
			Height = Width;
	}
	
	
	// next increase by our maximum possible random scaling
	if (m_BrushScalingMaxRand > 0)
	{
		// find out the random range
		UINT32 Lower = 0;
		UINT32 Upper = 0;
		PathProcessorBrush::GetRandomLimits(100, m_BrushScalingMaxRand, &Lower, &Upper);
		
		//Use the upper limit
		ScaleFactor *= ((double)Upper * 0.01);
	}

	// and our maximum possible pressure effect
/*	if (m_ScalingMaxPressure > 0 && !IgnorePressure)
	{
		ScaleFactor *= ((double)m_ScalingMaxPressure * 0.01 + 1);
		
	}
*/

	MILLIPOINT NewHeight = (MILLIPOINT)((double)Height * ScaleFactor);

	// if we have random offset then there will be some effect, this is separate from the offset amount
	if (m_OffsetValueMaxRand > 0)
	{
		// rand value is a percentage of object size
		double RandMultiplier = (double)m_OffsetValueMaxRand * 0.01;
		double OffsetAmount = (double)Height * RandMultiplier;
		
		// remember that it goes both sides
		MILLIPOINT mpAmount = (MILLIPOINT)(2*OffsetAmount);
		NewHeight += mpAmount;
	}

	// if we are offset then add that amount
	switch (m_PathOffsetType)
	{
		case OFFSET_NONE:
			break;
		case OFFSET_LEFT:
		case OFFSET_RIGHT:
		case OFFSET_ALTERNATE:
		{
			MILLIPOINT OffsetIncr = 2 * m_PathOffsetValue;
			NewHeight += OffsetIncr;
		}
		break;
		case OFFSET_RANDOM:
			break;
		default:
			ERROR3("Invalid offset type");
	}

	return NewHeight;
}



/********************************************************************************************

>	void BrushDefinition::InitialiseBrushData() 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     -
	Returns:	-
	Purpose:	retrieves data from the path processor and puts it into our brushdata object
	SeeAlso:	-

********************************************************************************************/

void BrushDefinition::InitialiseBrushData(BrushData* pData)
{
	pData->m_BrushHandle = 100;
	
	pData->m_BrushScaling = m_BrushScaling;
	pData->m_BrushScalingIncr = m_BrushScalingIncr;
	pData->m_BrushScalingMaxRand = m_BrushScalingMaxRand;
	pData->m_BrushScalingRandSeed = m_BrushScalingRandSeed;
	pData->m_ScalingMaxPressure = m_ScalingMaxPressure;

	pData->m_bRotate = m_bRotate;
	pData->m_RotateAngle = m_RotateAngle;
	pData->m_RotAngleIncrConst = m_RotAngleIncrConst;
	pData->m_RotAngleIncrProp  = m_RotAngleIncrProp;
	pData->m_RotationMaxRand = m_RotationMaxRand;
	pData->m_RotationRandSeed = m_RotationRandSeed;

	pData->m_BrushSpacing  = m_BrushSpacing;
	pData->m_BrushSpacingIncrConst = m_BrushSpacingIncrConst;
	pData->m_BrushSpacingIncrProp = m_BrushSpacingIncrProp;
	pData->m_BrushSpacingMaxRand = m_BrushSpacingMaxRand;
	pData->m_BrushSpacingRandSeed = m_BrushSpacingRandSeed;

	pData->m_PathOffsetType = m_PathOffsetType;
	pData->m_PathOffsetValue = m_PathOffsetValue;
	pData->m_PathOffsetIncrConst = m_PathOffsetIncrConst;
	pData->m_PathOffsetIncrProp = m_PathOffsetIncrProp;
	pData->m_OffsetValueMaxRand = m_OffsetValueMaxRand;
	pData->m_OffsetValueRandSeed = m_OffsetValueRandSeed;
	pData->m_OffsetTypeRandSeed = m_OffsetTypeRandSeed;

	pData->m_bTile = m_bTile;

	pData->m_SequenceType = m_SequenceType;
	pData->m_SequenceRandSeed = m_SequenceRandSeed;

	pData->m_bUseLocalFillColour = FALSE;
	
	pData->m_BrushHueIncrement = m_BrushHueIncrement;
	pData->m_BrushHueMaxRand   = m_BrushHueMaxRand;
	pData->m_BrushHueRandSeed = m_BrushHueRandSeed;

	pData->m_BrushSatIncrement = m_BrushSatIncrement;
	pData->m_BrushSatMaxRand   = m_BrushSatMaxRand;
	pData->m_BrushSatRandSeed  = m_BrushSatRandSeed;

	pData->m_BrushTransparency = m_BrushTransparency;
	pData->m_TranspMaxPressure = m_TranspMaxPressure;

	if (m_TimeStampPeriod <=0)
	{
		pData->m_bTimeStampBrushes	= FALSE;
		pData->m_TimeStampPeriod	= UINT32(-1);
	}
	else
	{
		pData->m_bTimeStampBrushes = TRUE;
		pData->m_TimeStampPeriod = (UINT32)m_TimeStampPeriod;
	}

	pData->m_Name = *GetLineName();
}



/********************************************************************************************

>	BrushData BrushDefinition::GetBrushData() 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     -
	Returns:	our brush data object
	Purpose:	gets the latest data from our path processor, copies it into our BrushData object 
				which is then returned.  Note that you are responsible for deleting this object.
	SeeAlso:	-

********************************************************************************************/

BrushData* BrushDefinition::GetBrushData()
{
	BrushData* pNewData = new BrushData;
	if (pNewData != NULL)
		InitialiseBrushData(pNewData);
	return pNewData;
}

/********************************************************************************************

>	BrushData BrushDefinition::GetBrushData() 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     -
	Returns:	our brush data object
	Purpose:	gets the latest data from our path processor, copies it into our BrushData object 
				which is then returned.  Note that you are responsible for deleting this object.
	SeeAlso:	-

********************************************************************************************/

void BrushDefinition::SetMembersFromData(BrushData Data)
{
	m_BrushSpacing          = Data.m_BrushSpacing;
	m_BrushSpacingIncrProp  = Data.m_BrushSpacingIncrProp;
	m_BrushSpacingIncrConst = Data.m_BrushSpacingIncrConst;
	m_BrushSpacingMaxRand   = Data.m_BrushSpacingMaxRand;
	m_BrushSpacingRandSeed  = Data.m_BrushSpacingRandSeed;

	m_PathOffsetType      = Data.m_PathOffsetType;
	m_PathOffsetValue     = Data.m_PathOffsetValue;
	m_PathOffsetIncrProp  = Data.m_PathOffsetIncrProp;
	m_PathOffsetIncrConst = Data.m_PathOffsetIncrConst;
	m_OffsetTypeRandSeed  = Data.m_OffsetTypeRandSeed;
	m_OffsetValueMaxRand  = Data.m_OffsetValueMaxRand;
	m_OffsetValueRandSeed = Data.m_OffsetValueRandSeed;

	m_bRotate             = Data.m_bRotate;
	m_RotateAngle         = Data.m_RotateAngle;
	m_RotationMaxRand     = Data.m_RotationMaxRand;
	m_RotationRandSeed	  = Data.m_RotationRandSeed;
	m_RotAngleIncrConst	  = Data.m_RotAngleIncrConst;
	m_RotAngleIncrProp    = Data.m_RotAngleIncrProp;
	m_RotationMaxPressure = Data.m_RotationMaxPressure;

	m_bTile               = Data.m_bTile;

	m_BrushScaling			= Data.m_BrushScaling;
	m_BrushScalingIncr		= Data.m_BrushScalingIncr;
	m_BrushScalingIncrConst = Data.m_BrushScalingIncrConst;
	m_BrushScalingMaxRand   = Data.m_BrushScalingMaxRand;
	m_BrushScalingRandSeed  = Data.m_BrushScalingRandSeed;
	m_ScalingMaxPressure    = Data.m_ScalingMaxPressure;
	
	m_BrushHueIncrement     = Data.m_BrushHueIncrement;
	m_BrushHueMaxRand       = Data.m_BrushHueMaxRand;
	m_BrushHueRandSeed      = Data.m_BrushHueRandSeed;

	m_BrushSatIncrement     = Data.m_BrushSatIncrement;
	m_BrushSatMaxRand       = Data.m_BrushSatMaxRand;
	m_BrushSatRandSeed      = Data.m_BrushSatRandSeed;

	m_BrushTransparency     = Data.m_BrushTransparency;
	m_TranspMaxPressure     = Data.m_TranspMaxPressure;

	m_SequenceType        = Data.m_SequenceType;
	m_SequenceRandSeed    = Data.m_SequenceRandSeed;

	m_LargestBoundingBox.MakeEmpty();

	m_Name = TEXT("Custom Brush");

	m_TimeStampPeriod = Data.m_TimeStampPeriod;


}


/********************************************************************************************

>	UINT32 BrushDefinition::GetFirstRandomNumber(UINT32 Seed = 0) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     a seed to use to generate the random number sequence
	Returns:	the first number in a random number sequence
	Purpose:	Seeds srand with either the seed supplied or by using time (i.e. a random seed)
	SeeAlso:

********************************************************************************************/

UINT32 BrushDefinition::GetFirstRandomNumber(UINT32 Seed)
{
	// seed the random number generator
	if (Seed == 0)
		srand( (unsigned)time( NULL ) );
	else
		srand((unsigned)Seed);

	return (UINT32)rand();
}


/********************************************************************************************

>	UINT32 BrushDefinition::GetNextRandomNumber() 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     -
	Returns:	the next number in an already seeded random number sequence
	Purpose:	as above, note that you MUST have already called GetFirstRandomNumber()
	SeeAlso:

********************************************************************************************/

UINT32 BrushDefinition::GetNextRandomNumber()
{
	return (UINT32)rand();
}


/********************************************************************************************

>	BOOL  BrushDefinition::CopyInkTreeToClipboard() 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:		-
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	copies the ink objects of this brush definition to the clipboard so they can be
				used elsewhere
	SeeAlso:	-

********************************************************************************************/

BOOL BrushDefinition::CopyInkTreeToClipboard()
{
	PORTNOTETRACE("clipboard","BrushDefinition::CopyInkTreeToClipboard - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	// first see if we can get the clipboard
	InternalClipboard *Clipboard = InternalClipboard::Instance();
	if (Clipboard == NULL || !Clipboard->PrepareForCopy())
	{
		ERROR3("Error copying to clipboard");
		return FALSE;
	}

	if (m_pTree == NULL)
	{
		ERROR3("No ink objects");
		return FALSE;
	}

	// remember that we store the ink nodes in a spread, so the first child
	// will be the layer, which we do not want to copy
	Node* pFirstNode = m_pTree->FindFirstChild();
	if (pFirstNode == NULL)
	{
		ERROR3("No layer underneath spread");
		return FALSE;
	}
	pFirstNode = pFirstNode->FindFirstChild();
	
	// Just be sure its an ink node
	if (pFirstNode == NULL || !pFirstNode->IsAnObject())
	{
		ERROR3("No ink object to copy");
		return FALSE;
	}

	// we want to set up a range to copy to the clipboard, so find the rightmost sibling 
	// of our node

	// We need to set them unselected so that range::findfirst doesn't return NULL

	Node* pLastNode = pFirstNode;
	Node* pNextNode = pLastNode->FindNext();
	while (pNextNode != NULL)
	{
		pNextNode->SetSelected(FALSE);
		pLastNode = pNextNode;
		pNextNode = pLastNode->FindNext();
	}
	pFirstNode->SetSelected(FALSE);
	DocCoord Centre;
	if (pFirstNode->IsKindOf(CC_RUNTIME_CLASS(NodeRenderableInk)))
	{
		DocRect BRect = ((NodeRenderableInk*)pFirstNode)->GetBoundingRect();
		Centre = BRect.Centre();
		TRACEUSER( "Diccon", _T("Ink node centre = %d, %d\n"), Centre.x, Centre.y);
	}
	Node* pChild = pFirstNode->FindFirstChild(CC_RUNTIME_CLASS(AttrRadialTranspFill));
	if (pChild != NULL)
	{
		DocCoord *pStartPoint = ((AttrRadialTranspFill*)pChild)->GetStartPoint();
		if (pStartPoint != NULL)
			TRACEUSER( "Diccon", _T("StartPoint pre-export = %d, %d\n"), pStartPoint->x, pStartPoint->y);
	}


	// that should give us our range, now set up the range control, just use the default
	RangeControl rc;
	rc.Unselected = TRUE;
	rc.Selected = FALSE;

	Range BrushRange(pFirstNode, pLastNode, rc);

	PreExportSubTree();
	
	BOOL ok =  Clipboard->CopyObjects(BrushRange);
	BOOL StillOk = Clipboard->CopyCompleted();

	PostExportSubTree();

	return (ok && StillOk);
#endif
	return false;
}
	
/********************************************************************************************

>	BOOL BrushDefinition::ExportBrush(BaseCamelotFilter *pFilter)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/12/99

	Inputs:		pFilter - the BaseCamelotFilter filter that is being used to export a file.
				Handle - the handle assigned to the brush
	Returns:	TRUE if export was successful
				FALSE if export was aborted - no error is set, as in this case, it usually
				means that the Brush has been deleted, and is being treated as an old-style
				line. In this case, the caller should simply not bother exporting the
				attribute using the Brush definition.

	Purpose:	To export a Brush definition. 

	Notes:		Do not call this function directly, it is better to call it via 
				BrushComponent::ExportBrush as this ensures that each brush is only saved once

********************************************************************************************/

BOOL BrushDefinition::ExportBrush(BaseCamelotFilter* pFilter,  BrushHandle Handle)
{
	ERROR2IF(pFilter == NULL, FALSE, "Filter is null");
	ERROR2IF(Handle == BrushHandle_NoBrush, FALSE, "Trying to export null brush");

	BOOL ok = TRUE;

	PreExportSubTree();
	CXaraFileRecord Record(TAG_BRUSHDEFINITION, TAG_BRUSHDEFINITION_SIZE);
	if (ok) ok = Record.Init();
	
	// first write the handle
	if (ok) ok = Record.WriteUINT32(Handle);
	if (ok)	pFilter->Write(&Record);

	// then write the subtree - note that this may be moved to the superclass
	if (ok)
	{
		Node* pExportNode = NULL;
		if (m_pTree == NULL)
			ok = FALSE;
		else
		{
			pExportNode = m_pTree->FindFirstChild();	// Find the Layer
			if (pExportNode != NULL)
				pExportNode = pExportNode->FindFirstChild();	// Find the inknode
		}

		// Write out the clipart subtree. We have to encapsulate it in DOWN and UP
		// records ourselves
		CXaraFileRecord DownRec(TAG_DOWN, 0);
		if (ok)	pFilter->Write(&DownRec);

		if (ok) ok = pFilter->WriteNodes(pExportNode);

		CXaraFileRecord UpRec(TAG_UP, 0);
		if (ok)	pFilter->Write(&UpRec);
		
	}
	PostExportSubTree();

	// now write the data, note that the handle is written here also as they
	// are retrieved separately and will need to be reconciled
	CXaraFileRecord DataRecord((INT32)TAG_BRUSHDATA, (INT32)TAG_BRUSHDATA_SIZE);
	if (ok) ok = DataRecord.Init();
	
	BYTE Flags = 0;
	if (m_bTile) 
		Flags |= TAG_BRUSHTILE_FLAG;
	if (m_bRotate)
		Flags |= TAG_BRUSHROTATE_FLAG;


	INT32 Offset = (INT32)m_PathOffsetType;

	if(ok)	ok = DataRecord.WriteUINT32((UINT32)Handle);
	if (ok) ok = DataRecord.WriteINT32(m_BrushSpacing);
	if (ok) ok = DataRecord.WriteBYTE(Flags);
	if (ok) ok = DataRecord.WriteDOUBLE(m_RotateAngle);
	if (ok) ok = DataRecord.WriteINT32(Offset);
	if (ok) ok = DataRecord.WriteINT32(m_PathOffsetValue); 
	if (ok) ok = DataRecord.WriteUnicode((TCHAR *)*(GetLineName()));
	if (ok) ok = DataRecord.WriteDOUBLE(m_BrushScaling);
	if (ok) pFilter->Write(&DataRecord);
	
	// write the additional data
	CXaraFileRecord NextRec(TAG_MOREBRUSHDATA, TAG_MOREBRUSHDATA_SIZE);
	if (ok) ok = NextRec.Init();
	if (ok) ok = NextRec.WriteDOUBLE(m_BrushSpacingIncrProp);
	if (ok) ok = NextRec.WriteINT32(m_BrushSpacingIncrConst);
	if (ok) ok = NextRec.WriteINT32((INT32)m_BrushSpacingMaxRand);
	if (ok) ok = NextRec.WriteINT32((INT32)m_BrushSpacingRandSeed);

	if (ok) ok = NextRec.WriteDOUBLE(m_BrushScalingIncr);
	if (ok) ok = NextRec.WriteINT32((INT32)m_BrushScalingMaxRand);
	if (ok) ok = NextRec.WriteINT32((INT32)m_BrushScalingRandSeed);

	if (ok) ok = NextRec.WriteINT32((INT32)m_SequenceType);
	if (ok) ok = NextRec.WriteINT32((INT32)m_SequenceRandSeed);

	if (ok) ok = NextRec.WriteDOUBLE(m_PathOffsetIncrProp);
	if (ok) ok = NextRec.WriteINT32(m_PathOffsetIncrConst);
	if (ok) ok = NextRec.WriteINT32((INT32)m_OffsetTypeRandSeed);
	if (ok) ok = NextRec.WriteINT32((INT32)m_OffsetValueMaxRand);
	if (ok) ok = NextRec.WriteINT32((INT32)m_OffsetValueRandSeed);
	if (ok) ok = pFilter->Write(&NextRec);

	CXaraFileRecord AnotherRec(TAG_EVENMOREBRUSHDATA, TAG_EVENMOREBRUSHDATA_SIZE);
	if (ok) ok = AnotherRec.Init();
	if (ok) ok = AnotherRec.WriteINT32((INT32)m_RotationMaxRand);
	if (ok) ok = AnotherRec.WriteINT32((INT32)m_RotationRandSeed);
	if (ok) ok = pFilter->Write(&AnotherRec);
	
	CXaraFileRecord PressureRec(TAG_BRUSHPRESSUREINFO, TAG_BRUSHPRESSUREINFO_SIZE);
	if (ok) ok = PressureRec.Init();
	if (ok) ok = PressureRec.WriteINT32((INT32)m_ScalingMaxPressure);
	if (ok) ok = PressureRec.WriteINT32((INT32)m_SpacingMaxPressure);
	if (ok) ok = PressureRec.WriteINT32((INT32)m_OffsetMaxPressure);
	if (ok) ok = PressureRec.WriteINT32((INT32)m_RotationMaxPressure);
	if (ok) ok = PressureRec.WriteINT32((INT32)m_HueMaxPressure);
	if (ok) ok = PressureRec.WriteINT32((INT32)m_SatMaxPressure);
	if (ok) ok = PressureRec.WriteINT32((INT32)m_TimeStampMaxPressure);
	if (ok) ok = pFilter->Write(&PressureRec);
	
	// now write our transparency info
	CXaraFileRecord TranspRec(TAG_BRUSHTRANSPINFO, TAG_BRUSHTRANSPINFO_SIZE);
	if (ok) ok = TranspRec.Init();
	if (ok) ok = TranspRec.WriteINT32(INT32(m_BrushTransparency));
	if (ok) ok = TranspRec.WriteINT32(INT32(m_TranspMaxPressure));
	if (ok) ok = TranspRec.WriteDOUBLE(m_RotAngleIncrConst);
	if (ok) ok = TranspRec.WriteDOUBLE(m_BrushScalingIncrConst);
	if (ok) ok = TranspRec.WriteINT32(INT32(m_BrushHueMaxRand));
	if (ok) ok = TranspRec.WriteINT32(INT32(m_BrushHueRandSeed));
	if (ok) ok = TranspRec.WriteINT32(INT32(m_BrushSatMaxRand));
	if (ok) ok = TranspRec.WriteINT32(INT32(m_BrushSatRandSeed));
	if (ok) ok = pFilter->Write(&TranspRec);

	// Finally, mark this stroke as having been written out so we don't do it again
	if (ok) SetIOStore(TRUE);



	return ok;

}



/********************************************************************************************

>	BOOL BrushDefinition::PreExportSubTree() 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     -
	Returns:	TRUE if successful, FALSE if something went wrong
	Purpose:	To perform various functions just prior to saving the subtree of the brush definition.
				The function loops through the BrushRefs, finding the blendpaths of each and 
				transforming their attribute maps back to the position that they were in when the 
				brush was created.
	SeeAlso:	-

	Notes:      This function is required because if the attribute map is not saved out at the exact
				position it was created it will become 'out of sync' with the blendpath it applies to.
				Recall that upon reimporting the brush the blendpaths are generated from scratch
				whereas the attributes are not.
********************************************************************************************/

BOOL BrushDefinition::PreExportSubTree()
{
	BrushRef* pBrushRef = GetFirstBrushRef();
	ERROR2IF(pBrushRef== NULL, FALSE, "No brushref");
	while (pBrushRef != NULL)
	{
		BlendPath * pBlendPath = pBrushRef->GetFirstBlendPath();
		while (pBlendPath != NULL)
		{
			if (!pBlendPath->TransformBrushAttributesBeforeSave())
				ERROR2(FALSE, "Somethings gone wrong");
			pBlendPath = pBrushRef->GetNextBlendPath(pBlendPath);
		}
		pBrushRef = GetNextBrushRef();
	}
	return TRUE;
}



/********************************************************************************************

>	BOOL BrushDefinition::PostExportSubTree() 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     -
	Returns:	TRUE if successful, FALSE if something went wrong
	Purpose:	To perform various functions just prior to saving the subtree of the brush definition.
				The function loops through the BrushRefs, finding the blendpaths of each and undoes the
				transformation performed by PreExportSubTree.
				This is required because to continue using the brush in our current document we want 
				to use its current position
			
	SeeAlso:	-

	Notes:      This function is required because if the attribute map is not saved out at the exact
				position it was created it will become 'out of sync' with the blendpath it applies to.
				Recall that upon reimporting the brush the blendpaths are generated from scratch
				whereas the attributes are not.
********************************************************************************************/

BOOL BrushDefinition::PostExportSubTree()
{
	BrushRef* pBrushRef = GetFirstBrushRef();

	while (pBrushRef != NULL)
	{
		BlendPath * pBlendPath = pBrushRef->GetFirstBlendPath();
		while (pBlendPath != NULL)
		{
			if (!pBlendPath->TransformBrushAttributesAfterSave())
				ERROR3("Somethings gone wrong");
			pBlendPath = pBrushRef->GetNextBlendPath(pBlendPath);
		}
		pBrushRef = GetNextBrushRef();
	}
	return TRUE;
}








/*-------------------------------------------------------------------------------------------
---------------------------------------------------------------------------------------------
---------------------------------------------------------------------------------------------
   The BrushComponent Class

*/


/********************************************************************************************

>	BOOL BrushComponentClass::Init()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/99
	Returns:	TRUE if all went well;
				FALSE if not.
	Purpose:	Register the Brush document component with the main application.
	Errors:		Out of memory.
	SeeAlso:	DocComponent

********************************************************************************************/

BOOL BrushComponentClass::Init()
{
	// Instantiate a component class to register with the application.
	BrushComponentClass *pClass = new BrushComponentClass;
	if (pClass == NULL)
		return(FALSE);

	// Register it
	GetApplication()->RegisterDocComponent(pClass);

	return(TRUE);
}


/********************************************************************************************

>	BOOL BrushComponentClass::LoadDefaultBrushes()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/99
	Returns:	TRUE if all went well;
				FALSE if not.
	Purpose:	Loads default brushes from a file
	Errors:		Fail to load file	

********************************************************************************************/

BOOL BrushComponentClass::LoadDefaultBrushes()
{
	PORTNOTETRACE("other","BrushComponentClass::LoadDefaultBrushes - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	// lets try loading the default brushes here
	LoadBrushDirect LoadBrush;
	
	// set up the search path, file util allows us just to pass the path/*.xar
	String_256 SearchPath;
	CResDll::GetExecutablePath((TCHAR*)SearchPath);
	String_256 Path = TEXT("\\Templates\\Brushes\\");
	SearchPath += Path;
	String_256 FileSpecifier = SearchPath;
	FileSpecifier += TEXT("*.xar");

	BOOL ok = FileUtil::StartFindingFiles(&FileSpecifier);
	BrushComponent::BeginLoadingDefaultFiles();
	String_256 Filename;
	FilePath oFilePath;
	String_16 Extension;
	while (ok)
	{
		// fileutil will give us the next filename
		ok = FileUtil::FindNextFile(&Filename);
		String_256 FullPath = SearchPath;
		FullPath += Filename;
		if (ok)
		{
			oFilePath.SetFilePath(FullPath);
			Extension = oFilePath.GetType();
			Extension.toLower();
			if (oFilePath.IsValid() && Extension==String_16("xar"))
			{
				BrushComponent::LoadNextDefaultFile();
				ok = LoadBrush.Execute((TCHAR*)FullPath);
			}
		}
	}
	BrushComponent::EndLoadingDefaults();
	FileUtil::StopFindingFiles();
#endif
	return TRUE;
}
/********************************************************************************************

>	void BrushComponentClass::DeInit()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/99

	Returns:	TRUE if all went well;
				FALSE if not.

	Purpose:	De-initialises the vector Brush provider system

********************************************************************************************/

void BrushComponentClass::DeInit()
{
	BrushComponent::DeleteList();
}



/********************************************************************************************

>	BOOL BrushComponentClass::AddComponent(BaseDocument *pDocument)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/99
	Inputs:		pDocument - the document to add the component to.
	Returns:	TRUE if the Brush component was added ok;
				FALSE if not.
	Purpose:	Add a Brush component to the specified document.
	Errors:		Out of memory
	SeeAlso:	PrintComponentClass

********************************************************************************************/

BOOL BrushComponentClass::AddComponent(BaseDocument *pDocument)
{
	ERROR2IF(pDocument==NULL, FALSE, "NULL document passed to BrushCompClass:Add");

	// Check to see if this document already has a colour list; if so, leave it alone.
	if (pDocument->GetDocComponent(CC_RUNTIME_CLASS(BrushComponent)) != NULL)
		return(TRUE);

	// Ok - create the print mark component.
	BrushComponent *pComponent = new BrushComponent;
	if (pComponent == NULL)
		return(FALSE);

	// All ok - add the component to the document.
	pDocument->AddDocComponent(pComponent);

	return(TRUE);
}


/*--------------------------------------------------------------------------------------------
----------------------------------------------------------------------------------------------
----------------------------------------------------------------------------------------------
		The BrushComponent class

*/

UINT32 BrushComponent::m_CurrentDefaultFile = BRUSHFILE_NONE;

/********************************************************************************************

>	BrushComponent::BrushComponent()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/99

	Purpose:	Constructor

********************************************************************************************/

BrushComponent::BrushComponent() : LineComponent()
{
	m_pImportBrushDef = NULL;
	
}



/********************************************************************************************

>	BrushComponent::~BrushComponent()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/99

	Purpose:	Destructor

********************************************************************************************/

BrushComponent::~BrushComponent()
{
	if (m_pImportBrushDef != NULL)
		delete m_pImportBrushDef;
}


/********************************************************************************************

>	BOOL BrushComponent::BeginLoadingDefaultFiles(UINT32 NumFiles)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     -
	Returns:	TRUE 
	Purpose:	Lets the component know that we are starting to load default brushes
	Notes:		What happens here is that every brush added to the component whilst 
				m_CurrentDefaultBrush != BRUSHFILE_NONE will be assigned m_CurrentDefaultBrush
				as its default file ID.
				Note again, as you can tell from the code this can only be done once.
********************************************************************************************/

BOOL BrushComponent::BeginLoadingDefaultFiles()
{
	m_CurrentDefaultFile = BRUSHFILE_NONE;
	return TRUE;
}


/********************************************************************************************

>	BOOL BrushComponent::LoadDefaultFile(String_256 Filename)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:    -
	Returns:	TRUE if all goes well
	Purpose:	Increments our default file counter  
	SeeAlso:	-

********************************************************************************************/

BOOL BrushComponent::LoadNextDefaultFile()
{	
	// is this the first file?
	if (m_CurrentDefaultFile == BRUSHFILE_NONE)
		m_CurrentDefaultFile = 0;
	else
		m_CurrentDefaultFile++;

	return TRUE;
}
	


/********************************************************************************************

>	void BrushComponent::EndLoadingDefaults()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     -
	Returns:	-
	Purpose:	Finishes loading default files
	SeeAlso:	-

********************************************************************************************/

void BrushComponent::EndLoadingDefaults()
{
	m_CurrentDefaultFile = BRUSHFILE_NONE;
}

/********************************************************************************************

>	AttrBrushType* BrushComponent::CreateAttributeNode(BrushHandle Handle) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     Handle to the brush definition which we want to base this brush on
	Returns:	Attribute node based on this definition
	Purpose:	-
	SeeAlso:	-

********************************************************************************************/

AttrBrushType* BrushComponent::CreateAttributeNode(BrushHandle Handle)
{
	BrushDefinition* pBrushDef = FindBrushDefinition(Handle);
	if (pBrushDef == NULL)
		return NULL;

	AttrBrushType* pNewNode = new AttrBrushType;
	if (pNewNode == NULL)
		return NULL;

	BrushAttrValue* pVal = (BrushAttrValue*)pNewNode->GetAttributeValue();
	if (pVal == NULL)
		return NULL;

	PathProcessorBrush* pPathProc = new PathProcessorBrush;
	if (pPathProc == NULL)
		return NULL;
	pBrushDef->CopyDataToProcessor(pPathProc);
	pVal->SetPathProcessor(pPathProc);

	// tell it which brush definition to use
	pPathProc->SetBrushDefinition(Handle);
	return pNewNode;
}


/********************************************************************************************

>	BOOL  BrushComponent::CopyInkObjectsToClipboard(BrushHandle Handle) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     Handle to the brush definition which we want to send to the clipboard
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	copies the ink objects of this brush definition to the clipboard so they can be
				used elsewhere
	SeeAlso:	-

********************************************************************************************/

BOOL BrushComponent::CopyInkObjectsToClipboard(BrushHandle Handle)
{
	if (Handle == BrushHandle_NoBrush || Handle >= m_Used)
	{
		ERROR3("Invalid brush handle");
		return FALSE;
	}

	BrushDefinition* pBrushDef = (BrushDefinition*)m_pLineArray[Handle];
	if (pBrushDef == NULL)
	{
		ERROR3("Brush definition is NULL");
		return FALSE;
	}
	
	return pBrushDef->CopyInkTreeToClipboard();
}

/********************************************************************************************

>	static AttrBrushType* BrushComponent::CreateNode(BrushHandle Handle) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     Handle to the brush definition which we want to base this brush on
	Returns:	Attribute node based on this definition
	Purpose:	static version of above
	SeeAlso:	-

********************************************************************************************/

AttrBrushType* BrushComponent::CreateNode(BrushHandle Handle)
{
	BrushDefinition* pBrushDef = FindBrushDefinition(Handle);
	if (pBrushDef == NULL)
		return NULL;

	AttrBrushType* pNewNode = new AttrBrushType;
	if (pNewNode == NULL)
		return NULL;

	BrushAttrValue* pVal = (BrushAttrValue*)pNewNode->GetAttributeValue();
	if (pVal == NULL)
		return NULL;

	PathProcessorBrush* pPathProc = new PathProcessorBrush;
	if (pPathProc == NULL)
		return NULL;
	pBrushDef->CopyDataToProcessor(pPathProc);
	pVal->SetPathProcessor(pPathProc);

	// tell it which brush definition to use
	pPathProc->SetBrushDefinition(Handle);
	return pNewNode;
}


/********************************************************************************************

>	BOOL BrushComponent::StartExport(BaseCamelotFilter *pFilter)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/99

	Inputs:		pFilter - the BaseCamelotFilter filter that is being used to export a file.
	Returns:	TRUE if the component was able to prepare for exporting;
				FALSE if not (e.g. out of memory)

	Purpose:	Called before an export is started

********************************************************************************************/

BOOL BrushComponent::StartExport(BaseCamelotFilter *pFilter)
{
//	return TRUE; // for now
#if !defined(EXCLUDE_FROM_RALPH)
	
	if (pFilter == NULL)
	{
		ERROR3("BrushComponent::StartExport filter is null!");
		return(TRUE);
	}

	// ****!!!!TODO - if we're multi-threadig, this probably needs to be a critical section
	// because the list is global

	// Flag all Brushs as not having been saved. When we save one, we set its flag so that
	// we don't try to save it a second time.
	if (m_pLineArray != NULL)
	{
		for (UINT32 Index = 0; Index < m_Used; Index++)
		{
			if (m_pLineArray[Index] != NULL)
					m_pLineArray[Index]->SetIOStore(FALSE);
		}

		// Write out an atomic tag definition in front of the vector Brush definition.
		// ****!!!!TODO - This should really only be done just before we export the first
		// BrushDEFINITION tag, but it is not properly supported by the export system as yet.
		
		//TODO : Make tags for brush components!!
		
		BOOL ok = TRUE;
		CXaraFileRecord Rec(TAG_ATOMICTAGS, TAG_ATOMICTAGS_SIZE);
		if (ok) ok = Rec.Init();
		if (ok) ok = Rec.WriteUINT32(TAG_BRUSHDEFINITION);
		if (ok)	pFilter->Write(&Rec);		// And write out the record
		
	}
#endif
	return(TRUE);
}


/********************************************************************************************

>	static BOOL ImportLine(CXaraFileRecord* pRecord)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/99

	Inputs:		pRecord - The TAG_BRUSHDEFINITION record to import

	Returns:	TRUE if Import was successful

	Purpose:	To import a vector Line definition

********************************************************************************************/

BOOL BrushComponent::StartImportBrush(CamelotRecordHandler *pHandler, CXaraFileRecord *pRecord, 
									  UINT32 ImportHandle)
{
	m_ImportHandle = 0xffffffff;
	
	if (m_pImportPreviousContext != NULL)
	{
	//	delete m_pImportPreviousContext;
		m_pImportPreviousContext = NULL;
	}

	if (m_pImportNewLine != NULL)
	{
		m_pImportNewLine->CascadeDelete();
		m_pImportNewLine = NULL;
	}

	// Create a spread and set up the import system to import the brush into it
	// If this fails, then it'll just find somewhere "sensible" to import into
	m_pImportNewLine = new Spread;
	if (m_pImportNewLine == NULL)
		return(FALSE);

	Layer *pLineLayer = new Layer(m_pImportNewLine, FIRSTCHILD, String_256(TEXT("Jason did this")));
	if (pLineLayer == NULL)
	{
		delete m_pImportNewLine;
		m_pImportNewLine = NULL;
		return(FALSE);
	}

	// Now, remember where we were importing into, and point the importer at our brush tree
	m_pImportPreviousContext = pHandler->GetInsertContext();
	pHandler->SetInsertContextNode(pLineLayer);

	// check to see if our import brushdef already exists, if so delete it
	if (m_pImportBrushDef != NULL)
		delete m_pImportBrushDef;
	// make the new brush definition
	m_pImportBrushDef = new BrushDefinition(m_pImportNewLine);
	if (m_pImportBrushDef == NULL)
		return(FALSE);



	// Remember the handle used for the Brush in the file with the Brush so that
	// we can map any incoming handles into the real handles we are using internally.
	m_pImportBrushDef->SetIOStore(ImportHandle & 0x00ffffff);


	/* in case you were wondering, after this function the import goes as follows:
	- Ink nodes etc. are imported into m_pImportNewLine
	- Brush data is imported
	- EndImportBrush is called, where the new brush definition is inserted
	*/
	


	return(TRUE);
}


/********************************************************************************************

>   BOOL BrushComponent::EndImportBrush(CamelotRecordHandler *pHandler);

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/99

	Inputs:		pRecord - The TAG_BRUSHDEFINITION record to import

	Returns:	TRUE if Import was successful

	Purpose:	To import a vector brush definition

********************************************************************************************/

BOOL BrushComponent::EndImportBrush(CamelotRecordHandler *pHandler)
{
	ERROR2IF(m_pImportPreviousContext == NULL,FALSE, "EndImportBrush - something hasn't gone too well");
	ERROR2IF(m_pImportBrushDef == NULL, FALSE, "ImportBrushDef is NULL at EndImportBrush");
	
	// Restore the previous import context node
	pHandler->RestoreInsertContext(m_pImportPreviousContext);
	delete m_pImportPreviousContext;
	m_pImportPreviousContext = NULL;

	// --- Now, convert all IndexedColours (which are document-dependent) into standalone DocColours
	// This wouldn't be necessary, except that the DocColours we saved have magically been turned
	// back into local Indexedcolours by the export/import process.
	// BLOCK

	LineDefinition::ConvertIndexedColours((Spread*)m_pImportNewLine);

	// Create a new Brush definition from the imported brush	
	if (m_pImportBrushDef->GenerateBrush() != TRUE)
		return FALSE;
	
	m_pImportBrushDef->CalculateMaxScaling();

	// add the item,
	/*BrushHandle NewHandle =*/ AddNewItem(m_pImportBrushDef); 

	CleanUpAfterImport();

	return TRUE;
	
	
}






/********************************************************************************************

>	BOOL ExportBrush(BaseCamelotFilter *pFilter, BrushHandle Handle)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/12/99

	Inputs:		pFilter - the BaseCamelotFilter filter that is being used to export a file.
				Handle  - The Brush to be exported

	Returns:	TRUE if export was successful
				FALSE if export was aborted - no error is set, as in this case, it usually
				means that the Brush has been deleted, and is being treated as an old-style
				line. In this case, the caller should simply not bother exporting the
				attribute using the Brush definition.

	Purpose:	To export a vector Brush definition. 

	Notes:		Brush definitions (like colours) are only saved out when a node in the
				tree is found which makes use of the Brush. You should call this function
				before exporting any attribute which uses the Brush definition. It
				automatically checks if the Brush has already been saved, and will not save
				the definition more than once.

				When saving your reference to the Brush, save out the Brush's Handle
				as it's unique ID word.

********************************************************************************************/

BOOL BrushComponent::ExportLine(BaseCamelotFilter *pFilter, BrushHandle Handle)
{
	ERROR3IF(pFilter == NULL, "Illegal NULL params");

	// Find the stroke, and baulk if it's all gone wrong
	BrushDefinition *pBrush = (BrushDefinition*)FindDefinition(Handle);
	if (pBrush == NULL)
	{
		ERROR3("Attempt to save a deleted or non-existent stroke");
		return(FALSE);
	}

	// Check if the definition has already been saved, in which case we don't need to do anything more
	if (pBrush->ReadIOStore())
		return(TRUE);

	// We've got a stroke definition now, get it to write itself to the file
	return pBrush->ExportBrush(pFilter, Handle);
	
}


/********************************************************************************************

>	static BrushHandle BrushComponent::FindImportedBrush(UINT32 ImportedHandle)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/12/99

	Inputs:		Importedhandle - The handle which is used in the file being imported
								 to reference the stroke.

	Returns:	The BrushHandle of the loaded stroke, or BrushHandle_NoBrush
				if the stroke could not be found.

	Purpose:	To match up a handle from the currently importing file to the
				real internal BrushHandle of the imported BrushDefinition

********************************************************************************************/

BrushHandle BrushComponent::FindImportedBrush(UINT32 ImportedHandle)
{
	ImportedHandle &= 0x00ffffff;	// Only the bottom 24 bits are relevant
	for (UINT32 Index = 0; Index < m_Used; Index++)
	{
		if (m_pLineArray[Index] != NULL && m_pLineArray[Index]->ReadIOStore() == ImportedHandle)
			return((BrushHandle)Index);
	}

	return(BrushHandle_NoBrush);
}



/********************************************************************************************

>	static BrushDefinition *BrushComponent::FindDefinition(UINT32 Handle, BOOL IncludeDeactiveated);

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/99

	Inputs:		Handle - A handle which uniquely identifies the new stroke
				IncludeDeactivated - flag to say that we wish to include deactivated definitions, 
				defaults to false
	Returns:	A pointer to a StrokeDefinition for that Stroke.
				If the stroke has been deleted, NULL will be returned, in which case,
				you should gracefully degrade (by rendering an old style Brush in place of
				the stroke)

	Purpose:	Find a Brush definition, given its unique identity handle

********************************************************************************************/

BrushDefinition *BrushComponent::FindBrushDefinition(BrushHandle Handle, BOOL IncludeDeactivated)
{
	if (Handle < 0)
	{
		ERROR3("Negative brush handle");
		return NULL;
	}
	if (Handle == BrushHandle_NoBrush)
		return(NULL);

	if (Handle >= m_Used)
	{
		ERROR3("Out of range Brush handle");
		return(NULL);
	}
	
	BrushDefinition* pDef = ((BrushDefinition*)m_pLineArray[Handle]);
	
	if (!pDef->IsActivated() && !IncludeDeactivated)
		pDef = NULL;

	return pDef;
}


/******************************************************************************************

>	static BOOL BrushComponent::ExpandArray(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/2/97

	Outputs:	On succesful exit, the member array of BrushDefinition pointers will be bigger
	Returns:	FALSE if it failed to allocate memory

	Purpose:	(Internal method)
				Expands the storage structure of the Brush list to allow more entries to be
				used. Called automatically by AddNewBrush as necessary.

	Notes:		Internal storage is an array of pointers to BrushDefinitions
				NULL pointers beyond (& including) "Used" indicate free slots.
				NULL pointers before "Used" indicate deleted Brushs - these slots
				should NOT be re-used, as there may still be references to them in
				the system.

******************************************************************************************/

BOOL BrushComponent::ExpandArray(void)
{
	// ****!!!!TODO - if we're multi-threading, this probably needs to be a critical section
	// because the list is global
	
	const INT32 AllocSize = m_CurrentSize + 64;

	if (m_pLineArray == NULL)
	{
		m_pLineArray = (LineDefinition **) CCMalloc(AllocSize * sizeof(LineDefinition *));
		if (m_pLineArray == NULL)
			return(FALSE);
	}
	else
	{
		// We have an array - we must make it larger
		LineDefinition **pNewBuf = (LineDefinition **)
									CCRealloc(m_pLineArray, AllocSize * sizeof(LineDefinition *));
		if (pNewBuf == NULL)
			return(FALSE);

		m_pLineArray = pNewBuf;
	}

	// Success. Initalise all the new pointers to NULL
	for (UINT32 i = m_Used; i < m_CurrentSize; i++)
			m_pLineArray[i] = NULL;

	// Update the current size value, and return success
	m_CurrentSize = (UINT32)AllocSize;
	return(TRUE);
	

}




/********************************************************************************************

>	BrushDefinition *BrushComponent::GetImportedBrushDefinition()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/99
	Inputs:		-
	Returns:	A pointer to the brush definition that is currently in the process of being 
				imported.

	Purpose:	As above, note that this function is only useful in the period between 
				StartImportBrush and EndImportBrush being called. At all other times
				m_pImportBrushDef should be NULL.

********************************************************************************************/

BrushDefinition* BrushComponent::GetImportBrushDefinition()
{
	return m_pImportBrushDef;
}



/********************************************************************************************

>	void BrushComponent::CleanUpAfterImport()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/99
	Inputs:		-
	Returns:	-

	Purpose:	Deletes the objects we created for import.  Goes without saying that
				calling this function before you have added the brushdef to the component is
				pretty silly.

********************************************************************************************/

void BrushComponent::CleanUpAfterImport()
{
	// don't delete the brushdef as it is now being used in the component
	if (m_pImportBrushDef != NULL)
		 m_pImportBrushDef = NULL;

	if (m_pImportNewLine != NULL)
		m_pImportNewLine = NULL;
}


/********************************************************************************************

>	void BrushDefinition::CopyDataToPathProcessor(PathProcessorBrush* pPathProc) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     pPathProc - path processor to receive the data
	Returns:	-
	Purpose:	To copy all of the numeric data about this brush to a path processor 
	SeeAlso:	-

********************************************************************************************/

void BrushDefinition::CopyDataToProcessor(PathProcessorBrush* pPathProc)
{
	if (pPathProc == NULL)
	{
		ERROR3("Path processor is NULL");
		return;
	}
	
	// spacing members
	pPathProc->SetSpacing(m_BrushSpacing);
	pPathProc->SetSpacingIncrProp(m_BrushSpacingIncrProp);	
	pPathProc->SetSpacingIncrConst(m_BrushSpacingIncrConst);
	pPathProc->SetSpacingMaxRand(m_BrushSpacingMaxRand);
	pPathProc->SetSpacingRandSeed(m_BrushSpacingRandSeed);
	
	// offset members
	pPathProc->SetPathOffsetType(m_PathOffsetType);
	pPathProc->SetPathOffsetValue(m_PathOffsetValue);
	pPathProc->SetPathOffsetIncrConst(m_PathOffsetIncrConst);
	pPathProc->SetPathOffsetIncrProp(m_PathOffsetIncrProp);
	pPathProc->SetOffsetTypeRandSeed(m_OffsetTypeRandSeed);
	pPathProc->SetOffsetValueMaxRand(m_OffsetValueMaxRand);
	pPathProc->SetOffsetValueRandSeed(m_OffsetValueRandSeed);
	
	
	// scaling members
	pPathProc->SetBrushScaling(m_BrushScaling);
	pPathProc->SetScalingMaxRand(m_BrushScalingMaxRand);
	pPathProc->SetScalingRandSeed(m_BrushScalingRandSeed);
	pPathProc->SetScalingMaxPressure(m_ScalingMaxPressure);
	pPathProc->SetMaxScaling(m_MaxScaling);
	pPathProc->SetBrushScalingIncr(m_BrushScalingIncr);
	pPathProc->SetBrushScalingIncrConst(m_BrushScalingIncrConst);
	

	// rotation
	pPathProc->SetRotationMaxRand(m_RotationMaxRand);
	pPathProc->SetRotationRandSeed(m_RotationRandSeed);
	pPathProc->SetRotationIncrConst(m_RotAngleIncrConst);
	pPathProc->SetRotationIncrProp(m_RotAngleIncrProp);
	pPathProc->SetRotationMaxPressure(m_RotationMaxPressure);
	pPathProc->SetRotationAngle(m_RotateAngle);

	
	// sequence
	pPathProc->SetSequenceType(m_SequenceType);
	pPathProc->SetSequenceSeed(m_SequenceRandSeed);

	// Hue/Saturation
	pPathProc->SetHueIncrement(m_BrushHueIncrement);
	pPathProc->SetHueMaxRand(m_BrushHueMaxRand);
	pPathProc->SetHueRandSeed(m_BrushHueRandSeed);
	pPathProc->SetSatIncrement(m_BrushSatIncrement);
	pPathProc->SetSatMaxRand(m_BrushSatMaxRand);
	pPathProc->SetSatRandSeed(m_BrushSatRandSeed);

	// Transparency
	pPathProc->SetBrushTransparency(m_BrushTransparency);
	pPathProc->SetTransparencyPressure(m_TranspMaxPressure);

	// Other
	pPathProc->SetRotated(m_bRotate);
	pPathProc->SetTiling(m_bTile);
}


/********************************************************************************************

>	static LineHandle LineComponent::AddNewItem(LineDefinition *pLine, BOOL AskName = FALSE)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/99

	Inputs:		pLine - (may not be NULL) A Line definition to add, which now
						  belongs to the LineComponent, so you mustn't delete it!
				AskName - a flag saying whether or not we should prompt the user for a name, 
				defaults to FALSE
	Returns:	A handle which uniquely identifies the new Line, or
				LineHandle_NoLine if we ran out of memory for storing Lines

	Purpose:	Adds the given Line to the global Line list.

	Notes:		Also read the notes in LineComponent::AddNewItem
				This function overrides the base class but actually uses it
				to do the actual adding.  functinality added here is that
				it works out whether or not the brush has actually been added
				and if so adds it to the line gallery

********************************************************************************************/

LineHandle BrushComponent::AddNewItem(LineDefinition *pItem, BOOL AskName)
{
	// just make sure its the right type
	ERROR2IF(!pItem->IsKindOf(CC_RUNTIME_CLASS(BrushDefinition)), 
			BrushHandle_NoBrush, "Attempting to pass in wrong definition");
	String_32 DefaultName(_R(IDS_BRUSH_DEFAULT_NAME));
	m_NewName = DefaultName;
	m_bCancelNewBrush = FALSE;
	if (AskName == TRUE)
	{
		// launch the naming dialog, it will either set m_NewName or tell us to cancel
		OpDescriptor* pDesc = OpDescriptor::FindOpDescriptor( OPTOKEN_INITNAME_BRUSH_DLG );
		if (pDesc != NULL)
		{
			pDesc->Invoke();
		}
	
		// did they cancel it?
		if (m_bCancelNewBrush)
		{
			m_bCancelNewBrush = FALSE;
			return BrushHandle_NoBrush;
		}
		
		pItem->SetLineName(&m_NewName);
	}

	// stash the old number of brushes, which will be incremented if we make a new one
	UINT32 OldNumBrushes = m_Used;

	// item is added in the base class
	LineHandle NewHandle = LineComponent::AddNewItem(pItem);
 
	// have we actually added a new brush?
	if (NewHandle >= OldNumBrushes && NewHandle != BrushHandle_NoBrush)
	{		
		// tell it the filename ID
		((BrushDefinition*)pItem)->SetBrushFileID(m_CurrentDefaultFile);

		// make a new atttibute to add to the line gallery
		PathProcessorBrush* pNewPathProc = new PathProcessorBrush;
		if (pNewPathProc == NULL)
		{
			ERROR3("Failed to allocate path processor");
			return NewHandle;
		}

		// tell it which brush definition to use
		pNewPathProc->SetBrushDefinition(NewHandle);

		// transfer all the data
		((BrushDefinition*)pItem)->CopyDataToProcessor(pNewPathProc);
		// make a new attribute value for use by the line gallery
		BrushAttrValue* pVal = new BrushAttrValue(pNewPathProc);
		if (pVal == NULL)
		{
			ERROR3("Unable to allocate new BrushAttrVal");
			return NewHandle;
		}
	
		// add it to the line gallery
PORTNOTE("dialog","Removed LineGallery usage")
#ifndef EXCLUDE_FROM_XARALX
		LineGallery::AddNewBrushItem(pVal);	
#endif

PORTNOTE("other","BrushComponent::AddNewItem - Removed tool handling")
#ifndef EXCLUDE_FROM_XARALX
		// get the freehand tool
		ToolListItem* pToolItem = Tool::Find(TOOLID_FREEHAND);
		ERROR2IF(pToolItem == NULL, NewHandle, "Unable to get tool item in DeactivateBrushDefAction::Init");

		FreeHandTool* pTool = (FreeHandTool*)pToolItem->m_pTool;
		ERROR2IF(pTool == NULL,	NewHandle, "Unable to get tool in DeactivateBrushDefAction::Init");
		
		// get the infobar
		FreeHandInfoBarOp* pInfoBar = ((FreeHandTool*)pTool)->GetInfoBar();
		ERROR2IF(pInfoBar == NULL, NewHandle, "Unable to get InfoBar in DeactivateBrushDefAction::Init");

		// add to the freehand infobar
		pInfoBar->AddBrush(NewHandle);
#endif
	}
	return NewHandle;
}


/********************************************************************************************

>	String_32 BrushComponent::GetUniqueName()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/99
	Inputs:		pName - the name that we wish to use, defaults to NULL in which
				case we use the default name
	Returns:	A unique name which can be given to the brush

	Purpose:	When naming a new brush we want to give it the name supplied, 
				however this may well be taken in which case we want to add
				a number to the default name

********************************************************************************************/

String_32 BrushComponent::GetUniqueName(String_32* pName)
{
	// essentially what we're doing is going through all the brush definitions and
	// retrieving the portion of name that will could be equal to the default name.
	// We count the number of default names that we find and once we're finished 
	// we use this as the suffix for the new name

	// Get the default name from the resources
	String_32 DefaultName(_R(IDS_BRUSH_DEFAULT_NAME));

	if (pName == NULL)
		pName = &DefaultName;
	
	INT32 DefaultSize = pName->Length();

	BrushDefinition* pDef = NULL;
	String_32* pDefName = NULL;
	String_32 LeftMost;
	UINT32 Counter = 0;
	INT32 DefaultCounter = 0;

	// loop though the definitions
	while (Counter < m_Used)
	{
		pDef = FindBrushDefinition(Counter);
		if (pDef != NULL)
		{		
			pDefName = pDef->GetLineName();
			if (pDefName == NULL)
			{
				ERROR3("Brush definition with no name in BrushComponent::GetUniqueName");
				break;
			}
			// test to see if the leftmost portion of the name is the same as our default
			pDefName->Left(&LeftMost, DefaultSize);
			if (LeftMost.IsIdentical(*pName))
				DefaultCounter++;

			LeftMost.Empty(); // wipe the default
		}
		Counter++;
	}

	String_32 UniqueName(*pName);
	
	// do we have to concatenate a number?
	if (DefaultCounter > 0)
	{
		String_32 Concat;
		Convert::LongToString(DefaultCounter, &Concat);
		UniqueName+=Concat;
	}
	return UniqueName;
}




/********************************************************************************************

>	BOOL LineComponent::NameIsUnique(const String_32& NameString)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com> 
	Created:	3/7/2000

	Inputs:		NameString - the string to test
	Returns:	TRUE if none of the definitions in the component have this string as their name,
				FALSE if one of them does

	Purpose:	To find out if someone has already gotten this name

********************************************************************************************/

BOOL BrushComponent::NameIsUnique(const String_32& NameString)
{
	if (NameString.IsEmpty())
		ERROR3("Empty string in BrushComponent::NameIsUnique");
	
	BOOL Unique = TRUE;
	UINT32 Counter = 0;
	String_32* pDefName = NULL;
	BrushDefinition* pDef = NULL;
	while (Counter < m_Used)
	{
		pDef = FindBrushDefinition(Counter);
		if (pDef != NULL)
		{
			pDefName = pDef->GetLineName();
			if( pDefName != NULL )
			{
				if (NameString == *pDefName)
				{
					Unique = FALSE;
					break;
				}
			}
		}
		Counter++;
	}
	return Unique;

}

void BrushComponent::SetNewName(String_32 NewName)
{
	m_NewName = NewName;
}

void BrushComponent::CancelNewBrush()
{
	m_bCancelNewBrush = TRUE;
}
