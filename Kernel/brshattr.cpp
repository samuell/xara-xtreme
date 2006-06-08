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

// Brush Attribute implementation

#include "camtypes.h"
#include "brshattr.h"
#include "brshcomp.h"
//#include "rik.h"     // for the strings
#include "basestr.h"
#include "samplist.h"

// Native file load/save includes
#include "camfiltr.h"
#include "cxfdefs.h"
#include "cxfrec.h"
#include "cxftags.h"
//#include "tim.h"			// For _R(IDE_FILE_WRITE_ERROR)
#include "ink.h"

// various includes
#include "lineattr.h"
#include "ndbldpth.h"  // for cpathpointinfo
#include "fillval.h"
#include "nodepath.h"
#include "blendatt.h"
#include "pbecomea.h"
#include "group.h"
#include "brpress.h"
#include "pen.h"
//#include "colormgr.h"
//#include "freehand.h"
#include "progress.h"
#include "nodetext.h"
#include "nodetxtl.h" // for our text bodges
#include "nodetxts.h"
#include "ppbrush.h"

#ifdef NEWFASTBRUSHES
	#include "nodebmp.h"
#endif


CC_IMPLEMENT_DYNCREATE(AttrBrushType, NodeAttribute)
CC_IMPLEMENT_DYNAMIC(BrushAttrValue, AttributeValue)
CC_IMPLEMENT_DYNAMIC(BrushAttrRecordHandler, CamelotRecordHandler);


// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW   


/*-------------------------------------------------------------------------------------------
---------------------------------------------------------------------------------------------
--------------------------------BrushAttrValue class implementation--------------------------
--------------------------------------------------------------------------------------------*/


/********************************************************************************************

>	BrushAttrValue::BrushAttrValue(PathProcessorBrush *pPathProcessor = NULL);

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/99

	Inputs:		-
	Purpose:	Default Constuctor for BrushAttrValue

********************************************************************************************/

BrushAttrValue::BrushAttrValue()
{
#ifdef NEWFASTBRUSHES
	pCachedBitmap = NULL;
#endif
	m_pProcessor = NULL;
#ifdef BRUSHPOINTSCACHE
	m_pCachedPoints = NULL;
#endif
	m_pTimeStampList = NULL;
	
	m_pPressureVals = NULL;
	m_CachedBRect = DocRect(0,0,0,0);

	m_pPressureSampler = NULL;
}



/********************************************************************************************

>	BrushAttrValue::BrushAttrValue(PathProcessorBrush *pPathProcessor = NULL);

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/99

	Inputs:		pPathProcessor - the stroke path-processor which will do the stroking
				effect "applied" by this attribute. NULL indicates that this attribute
				does no stroking (i.e. that the path should have an old-style "line")

				NOTE that this object is NOW OWNED by this BrushAttrValue, and will be
				deleted when the attribute value is deleted.

	Purpose:	Default Constuctor for BrushAttrValue

********************************************************************************************/

BrushAttrValue::BrushAttrValue(PathProcessorBrush *pPathProcessor)
{
#ifdef NEWFASTBRUSHES
	pCachedBitmap = NULL;
#endif
	// Remember our processor, and let it know that we "own" it
	m_pProcessor = pPathProcessor;
//	if (m_pProcessor != NULL)
//		m_pProcessor->SetParentAttribute(this);
#ifdef BRUSHPOINTSCACHE
	m_pCachedPoints = NULL;
#endif
	m_pTimeStampList = NULL;


	m_pPressureVals  = NULL;
	m_CachedBRect = DocRect(0,0,0,0);

	m_pPressureSampler = NULL;
}


/********************************************************************************************

>	BrushAttrValue::~BrushAttrValue()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/99

	Purpose:	Destructor
				Deletes any attached PathProcessor (see the constructor)

********************************************************************************************/

BrushAttrValue::~BrushAttrValue()
{
#ifdef NEWFASTBRUSHES
	if (pCachedBitmap != NULL)
	{
		delete (pCachedBitmap->GetBitmap ());
		delete (pCachedBitmap);
		pCachedBitmap = NULL;
	}
#endif
	if (m_pProcessor != NULL)
	{
		delete m_pProcessor;
		m_pProcessor = NULL;
	}
	FlushCache();
	FlushTimeStampCache();
	FlushPressureCache();

}



/********************************************************************************************

>	static BOOL BrushAttrValue::Init(void)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/99

	Returns:	TRUE if it initilised successfully

	Purpose:	Registers a default attribute of this type with the attribute manager

********************************************************************************************/

BOOL BrushAttrValue::Init(void)
{
	// The default attribute is one that has no effect (i.e. produces old-style "lines")
	BrushAttrValue *pAttr = new BrushAttrValue;
	if (pAttr == NULL)
		return FALSE;

	UINT32 ID = AttributeManager::RegisterDefaultAttribute(CC_RUNTIME_CLASS(AttrBrushType),
															pAttr);

	if (ID == ATTR_BAD_ID)
		return FALSE;
	ENSURE(ID == ATTR_BRUSHTYPE, "Incorrect ID for attribute!");

	//ERROR2IF(ID == ATTR_BAD_ID, FALSE, "Bad ID when Initialising BrushAttrValue");
	//ENSURE(ID == ATTR_BRUSHTYPE, "Incorrect Attribute ID");
	return(TRUE);
}



/********************************************************************************************

>	virtual void BrushAttrValue::Render(RenderRegion *pRegion,  BOOL Temp)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/99

	Inputs:		pRegion - the render region to render this attribute into.
				
	Purpose:	Sets the BrushAttrValue attribute for the given render region.

	Notes:		This attribute makes itself current in the render region, and
				also (possibly) adds a PathProcessor to handle stroking of
				all future rendered paths (until the attr is "restored")

	SeeAlso:	BrushAttrValue::Restore

********************************************************************************************/

void BrushAttrValue::Render(RenderRegion *pRegion, BOOL Temp)
{
	// Stack the current attribute and set ourselves up as the new one
	pRegion->SaveAttribute(ATTR_BRUSHTYPE, this, Temp);
	
#ifdef BRUSHPOINTSCACHE
	// if we don't yet have a cache then make one here
	if (m_pCachedPoints == NULL)
		InitialisePointsCache();
#endif

	// Find if we have a path processor to do the stroking, and if we do, 
	// stack a copy of it (a copy must be used to be thread-safe & bgrender-safe)
	if (m_pProcessor != NULL)
	{
	//	TRACEUSER("Diccon", _T("Rendering brush attribute\n") );
	
		// make a clone and push it onto the stack
		PathProcessorBrush *pNewProcessor = m_pProcessor->Clone();
		if (pNewProcessor != NULL)
		{
			//TRACEUSER("Diccon", _T("Pushing path processor\n") );
#ifdef NEWFASTBRUSHES
			pNewProcessor->SetLinkedAttribute (this);
#endif
			pRegion->PushPathProcessor(pNewProcessor);
		}
		
	}
}



/********************************************************************************************

>	virtual void BrushAttrValue::Restore(RenderRegion *pRegion, BOOL Temp)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/99

	Inputs:		pRegion - the render region to restore the attribute into.
				Temp    - TRUE if this is a temporary attribute, FALSE if it is
						  permanent (e.g. it's in a document tree).

	Purpose:	Restores the BrushAttrValue attribute for the given render region. 

	Notes:		This attribute makes sure it removes any PathProcessor it added
				to handle path stroking in BrushAttrValue::Render

********************************************************************************************/

void BrushAttrValue::Restore(RenderRegion *pRegion, BOOL Temp)
{
	pRegion->RestoreAttribute(ATTR_BRUSHTYPE, this, Temp);
}



/********************************************************************************************

>	void BrushAttrValue::GoingOutOfScope(RenderRegion *pRegion)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/1/97

	Inputs:		pRegion - the render region the attribute is in use by

	Purpose:	A sister function to Render()

				This is called by a render region when an attribute goes out of
				scope, i.e. when the attribute is no longer in use and is popped
				off the stack for the last time. (Do NOT confuse this with being
				pushed onto the render stack when overridden by another attr)

				It gives the attribute a chance to remove any PathProcessor(s)
				it added to the RenderRegion when it was Render()ed.

	Notes:		This attribute makes sure it removes any PathProcessor it added
				to handle path stroking in BrushAttrValue::Render

	SeeAlso:	BrushAttrValue::Render()

********************************************************************************************/

void BrushAttrValue::GoingOutOfScope(RenderRegion *pRegion)
{
	if (m_pProcessor != NULL)
	{
		//TRACEUSER("Diccon", _T("Popping path processor\n") );
		pRegion->PopPathProcessor();
	}
}



/********************************************************************************************

>	virtual void BrushAttrValue::SimpleCopy(AttributeValue *pValue)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/99

	Inputs:		pValue - pointer to the AttributeValue to copy

	Purpose:	See AttributeValue::SimpleCopy

********************************************************************************************/

void BrushAttrValue::SimpleCopy(AttributeValue *pValue)
{
	ERROR3IF(!IS_A(pValue, BrushAttrValue),
				"Invalid Attribute value passed to BrushAttrValue::SimpleCopy");

	// Just uses the assignment operator
	*this = *((BrushAttrValue *) pValue);



}

/********************************************************************************************

>	BOOL BrushAttrValue::Blend(BlendAttrParam* pBlendParam)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/3/2000
	Inputs:		pBlendParam - contains the blend info, 
	Outputs:	pBlendParama -  containing the blended attributes
	Returns:	TRUE - Blend went ok, the blended attr val is valid; FALSE if not.
	Purpose:	Blends between two brush attributes. In fact at the moment it just blends
				between the parameters in the two path processors.  At some point it should
				also blend between the ink objects and attributes.
			
	Errors:		Out of memory.

********************************************************************************************/

BOOL BrushAttrValue::Blend(BlendAttrParam* pBlendParam)
{
	ERROR2IF(pBlendParam == NULL, FALSE, "BlendAttrParam is NULL in BrushAttrValue::Blend");

	// First get the fill that we are blending to
	BrushAttrValue* pOtherBrush = 
				(BrushAttrValue*)pBlendParam->GetOtherAttrVal();
	if (pOtherBrush == NULL)
		return FALSE;

	// it is ok if we have null path processors as that merely indicates a default line
	PathProcessorBrush* pOtherPPB = pOtherBrush->GetPathProcessor();
	PathProcessorBrush* pThisPPB = GetPathProcessor();

	// Now make a new object of the same type as this one
	//CCRuntimeClass* ObjectType = GetRuntimeClass();
	BrushAttrValue* pNewAttr = new BrushAttrValue; //(BrushAttrValue*)ObjectType->CreateObject();

	if (pNewAttr == NULL)
	{
		// Fail if we couldn't create the new fill
		pBlendParam->SetBlendedAttrVal(NULL);
		return FALSE;
	}

	// Make the new fill an exact copy of this one
	pNewAttr->SimpleCopy(this);

	// make a new path processor
	PathProcessorBrush* pBlendedPPB = new PathProcessorBrush;
	
	if (pBlendedPPB == NULL)
	{
		delete pNewAttr;
		return FALSE;
	}
	
	pBlendedPPB->SetBrushDefinition(BrushHandle_NoBrush);

	// find out what point along the blend we are at
	double BlendRatio = pBlendParam->GetBlendRatio();

	// we need to ask the brush component to make a blended brush definition for us
	Document* pDoc = Document::GetCurrent();
	ERROR2IF(pDoc == NULL, FALSE, "Wheres the document?");
	BrushComponent* pBrushComp = (BrushComponent*)pDoc->GetDocComponent(CC_RUNTIME_CLASS(BrushComponent));
	ERROR2IF(pBrushComp == NULL, FALSE, "No brush component");	
	
	// don't want to blend the variables any more, lets just copy them
	BrushDefinition* pThisDef = GetBrushDefinition();
	BrushDefinition* pOtherDef = pOtherBrush->GetBrushDefinition();
	BrushHandle ThisHandle = BrushHandle_NoBrush;
	BrushHandle OtherHandle = BrushHandle_NoBrush;

	if (pThisPPB != NULL)
		ThisHandle = pThisPPB->GetBrushDefinitionHandle();

	if (pOtherPPB != NULL)
		OtherHandle = pOtherPPB->GetBrushDefinitionHandle();
	
	if (BlendRatio <= 0.5)
	{
		{
			if (pThisDef != NULL)
			{
				// if we have a ppb then copy the data across
				if (pThisPPB != NULL)
				{
					BrushData ThisData;
					pThisPPB->CopyBrushData(&ThisData);
					pBlendedPPB->CopyDataFromObject(&ThisData);	
				}
				else // otherwise go with the definition
					pThisDef->CopyDataToProcessor(pBlendedPPB);
				
				pBlendedPPB->SetBrushDefinition(GetBrushHandle());
				pBlendedPPB->SetMaxScaling(pThisDef->GetMaxScaling());
			}
			else
			{
				// turns out we're not really a brush so delete our processor
				delete pBlendedPPB;
				pBlendedPPB = NULL;
			}
		}
	}
	else
	{
	/*	if (ThisHandle == OtherHandle)
		{
			if (pOtherPPB != NULL)
			{
				delete pBlendedPPB;
				pBlendedPPB = pOtherPPB->Clone();
			}
			//pBlendedPPB->SetBrushDefinition(BrushHandle_NoBrush);
		}
		else */
		{
			if (pOtherDef != NULL)
			{
				if (pOtherPPB != NULL)
				{
					BrushData OtherData;
					pOtherPPB->CopyBrushData(&OtherData);
					pBlendedPPB->CopyDataFromObject(&OtherData);

					// little hack that we need to ensure the scaling is correct
					AttrBrushType* pAttrBrush = pOtherPPB->GetParentAttribute();
					if (pAttrBrush != NULL)
					{
						MILLIPOINT LineWidth = pAttrBrush->GetAppliedLineWidth();
						pOtherPPB->ScaleToValue(LineWidth);
					}

				}
				else
					pOtherDef->CopyDataToProcessor(pBlendedPPB);
				pBlendedPPB->SetMaxScaling(pOtherDef->GetMaxScaling());
			}
			else
			{
				// special case to deal with blend between a brushed and non=brushed object
				delete pBlendedPPB;
				pBlendedPPB = NULL;
				pNewAttr->SetPathProcessor(NULL);
				pBlendParam->SetBlendedAttrVal(pNewAttr);
				return TRUE;
			}
			pBlendedPPB->SetBrushDefinition(pOtherBrush->GetBrushHandle());
		}
	}

	// trudge through all the variables and blend them
	/*
	// blend scaling variables
	double BlendedScaling = BlendDouble(m_pProcessor->GetBrushScaling(), pOtherPPB->GetBrushScaling(), BlendRatio);
	pBlendedPPB->SetBrushScaling(BlendedScaling);
	double BlendedScalingIncr = BlendDouble(m_pProcessor->GetBrushScalingIncr(), pOtherPPB->GetBrushScalingIncr(), BlendRatio);
	pBlendedPPB->SetBrushScalingIncr(BlendedScalingIncr);

	// blend spacing variables
	MILLIPOINT BlendedSpacing = BlendINT32(m_pProcessor->GetSpacing(), pOtherPPB->GetSpacing(), BlendRatio);
	pBlendedPPB->SetSpacing(BlendedSpacing);
	MILLIPOINT BlendedSpacingIncrConst = BlendINT32(m_pProcessor->GetSpacingIncrConst(), pOtherPPB->GetSpacingIncrConst(), BlendRatio);
	pBlendedPPB->SetSpacingIncrConst(BlendedSpacingIncrConst);
	double BlendedSpacingIncrProp = BlendDouble(m_pProcessor->GetSpacingIncrProp(), pOtherPPB->GetSpacingIncrProp(), BlendRatio);
	pBlendedPPB->SetSpacingIncrProp(BlendedSpacingIncrProp);
	UINT32 BlendedSpacingMaxRand = BlendUINT32(m_pProcessor->GetSpacingMaxRand(), pOtherPPB->GetSpacingMaxRand(), BlendRatio);
	pBlendedPPB->SetSpacingMaxRand(BlendedSpacingMaxRand);

	// for the random seed don't blend, just use one or the other 
	if (BlendRatio <= 0.5)
		pBlendedPPB->SetSpacingRandSeed(m_pProcessor->GetSpacingRandSeed());
	else
		pBlendedPPB->SetSpacingRandSeed(pOtherPPB->GetSpacingRandSeed());

	// Offset variables
	MILLIPOINT BlendedOffsetValue = BlendINT32(m_pProcessor->GetPathOffsetValue(), pOtherPPB->GetPathOffsetValue(), BlendRatio);
	pBlendedPPB->SetPathOffsetValue(BlendedOffsetValue);
	MILLIPOINT BlendedOffsetIncrConst = BlendINT32(m_pProcessor->GetPathOffsetIncrConst(), pOtherPPB->GetPathOffsetIncrConst(), BlendRatio);
	pBlendedPPB->SetPathOffsetIncrConst(BlendedOffsetIncrConst);
	double BlendedOffsetIncrProp = BlendDouble(m_pProcessor->GetPathOffsetIncrProp(), pOtherPPB->GetPathOffsetIncrProp(), BlendRatio);
	pBlendedPPB->SetPathOffsetIncrProp(BlendedOffsetIncrProp);
	UINT32 BlendedOffsetMaxRand = BlendUINT32(m_pProcessor->GetOffsetValueMaxRand(), pOtherPPB->GetOffsetValueMaxRand(), BlendRatio);
	pBlendedPPB->SetOffsetValueMaxRand(BlendedOffsetMaxRand);

	// don't blend offset type, just use one or the other, likewise random seed
	if (BlendRatio <= 0.5)
	{
		pBlendedPPB->SetPathOffsetType(m_pProcessor->GetPathOffsetType());
		pBlendedPPB->SetOffsetTypeRandSeed(m_pProcessor->GetOffsetTypeRandSeed());
		pBlendedPPB->SetOffsetValueRandSeed(m_pProcessor->GetOffsetValueRandSeed());
	}
	else
	{
		pBlendedPPB->SetPathOffsetType(pOtherPPB->GetPathOffsetType());
		pBlendedPPB->SetOffsetTypeRandSeed(pOtherPPB->GetOffsetTypeRandSeed());
		pBlendedPPB->SetOffsetValueRandSeed(pOtherPPB->GetOffsetValueRandSeed());
	}

	// rotation 
	double BlendedRotateAngle = BlendDouble(m_pProcessor->GetRotationAngle(), pOtherPPB->GetRotationAngle(), BlendRatio);
	pBlendedPPB->SetRotationAngle(BlendedRotateAngle);
	
	// do all the flags at once
	if (BlendRatio <= 0.5)
	{
		pBlendedPPB->SetRotated(m_pProcessor->IsRotated());
		pBlendedPPB->SetTiling(m_pProcessor->IsTiled());
		pBlendedPPB->SetUseLocalFillColour(m_pProcessor->GetUseLocalFillColour());
		pBlendedPPB->SetScaleToLineWidth(m_pProcessor->IsScalingToLineWidth());
		pBlendedPPB->SetBrushDefinition(m_pProcessor->GetBrushDefinitionHandle());
	}
	else
	{
		pBlendedPPB->SetRotated(pOtherPPB->IsRotated());
		pBlendedPPB->SetTiling(pOtherPPB->IsTiled());
		pBlendedPPB->SetUseLocalFillColour(pOtherPPB->GetUseLocalFillColour());
		pBlendedPPB->SetScaleToLineWidth(pOtherPPB->IsScalingToLineWidth());
		pBlendedPPB->SetBrushDefinition(pOtherPPB->GetBrushDefinitionHandle());
	}
	*/
	// whew, think thats the lot, now attach the processor to the new attribute
	pNewAttr->SetPathProcessor(pBlendedPPB);

	// and attach the attribute to the blend param
	pBlendParam->SetBlendedAttrVal(pNewAttr);

	return TRUE;
}



/********************************************************************************************

>	BOOL BrushAttrValue::DoBecomeA(BecomeA* pBecomeA, Node* pParent)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/3/2000
	Inputs:		pBecomeA - the object that tells us what to become, and recieves the results
				pParent - the node that this attribute is applied to
	Outputs:	
	Returns:	TRUE if everything went ok,
	Purpose:	To convert our brush into its individual paths and attributes, works in a very
				similar way to the bounding box, by finding the paths of our parent node and
				asking our path processor to do the calculations
		
********************************************************************************************/

BOOL BrushAttrValue::DoBecomeA(BecomeA* pBecomeA, Node* pParent)
{
	ERROR2IF(pBecomeA == NULL, FALSE, "BecomeA pointer is NULL in BrushAttrValue::DoBecomeA");
	ERROR2IF(pParent == NULL, FALSE, "Parent node is NULL in BrushAttrValue::DoBecomeA");

	if (m_pProcessor == NULL)
		return FALSE;

	BOOL Success = FALSE;
//	UINT32 Dummy = 0;
	// if we have a nodepath then we can simply use its member path, otherwise we
	// have to ask it to become a nodepath
	if (pParent->IsNodePath())
	{	
		// we have a special calculation for nodepaths, because we can use their path
		Success =  m_pProcessor->DoBecomeA(pBecomeA, &((NodePath*)pParent)->InkPath, pParent);
	}
	else
	{
		BecomeA TestBecomeAPath(BECOMEA_TEST, CC_RUNTIME_CLASS(NodePath));
		if (pParent->CanBecomeA(&TestBecomeAPath))
		{
			// we must be applied to some arbitrary shape.  The thing is that we need to have
			// a path to pass to the PPB, so want to get a passback of all the paths in 
			// the node	
			
			// we need to allocate a path
			Path* pPath = new Path;
			if (pPath != NULL && pPath->Initialise())
			{
				PathBecomeA BecomeAPath(BECOMEA_PASSBACK, CC_RUNTIME_CLASS(NodePath), NULL, FALSE, pPath);
				if (pParent->DoBecomeA(&BecomeAPath))
				{
					Success =  m_pProcessor->DoBecomeA(pBecomeA, pPath, pParent);
				}
				delete pPath;
			}
		}
	}
	return Success;
}

/********************************************************************************************

>	static UINT32 BrushAttrValue::BlendUINT32(UINT32 First, UINT32 Second, double BlendRatio)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/99
	Inputs:		First - the first number to blend
				Second - the second number to blend
				BlendRatio - the proportion of first to second number
	Returns:	the blended result

	Purpose:	to blend between two numbers and return the result

********************************************************************************************/

UINT32 BrushAttrValue::BlendUINT32(UINT32 First, UINT32 Second, double BlendRatio)
{	
	double InvRatio = 1 - BlendRatio;
	double BlendedVal = ((double)First * InvRatio) + ((double)Second*BlendRatio);

	return (UINT32)BlendedVal;
}


/********************************************************************************************

>	static INT32 BrushAttrValue::BlendINT32(INT32 First, INT32 Second, double BlendRatio)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/99
	Inputs:		First - the first number to blend
				Second - the second number to blend
				BlendRatio - the proportion of first to second number
	Returns:	the blended result

	Purpose:	to blend between two numbers and return the result

********************************************************************************************/

INT32 BrushAttrValue::BlendINT32(INT32 First, INT32 Second, double BlendRatio)
{
	double InvRatio = 1 - BlendRatio;
	double BlendedVal = ((double)First * InvRatio) + ((double)Second*BlendRatio);

	return (INT32)BlendedVal;

}


/********************************************************************************************

>	static double BrushAttrValue::BlendDouble(double First, double Second, double BlendRatio)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/99
	Inputs:		First - the first number to blend
				Second - the second number to blend
				BlendRatio - the proportion of first to second number
	Returns:	the blended result

	Purpose:	to blend between two numbers and return the result

********************************************************************************************/

double BrushAttrValue::BlendDouble(double First, double Second, double BlendRatio)
{
	double InvRatio = 1 - BlendRatio;
	double BlendedVal = ((double)First * InvRatio) + ((double)Second*BlendRatio);

	return BlendedVal;

}


/********************************************************************************************

>	void BrushAttrValue::InitialisePointsCache()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/99

	Inputs:		-

	Purpose:	allocates the map to store cached points

********************************************************************************************/

void BrushAttrValue::InitialisePointsCache()
{
	
#ifdef BRUSHPOINTSCACHE
	// get rid of the existing one (if there is one)
	FlushCache();
	//allocate a new map
	m_pCachedPoints = new PointsMap;
PORTNOTE("other","Not use hash based map, InitHashTable makes no sense")
#ifndef EXCLUDE_FROM_XARALX
	if (m_pCachedPoints != NULL)
		m_pCachedPoints->InitHashTable(1277);		// Init hash table size to a suitably large prime number
#endif
#endif
	
}


/********************************************************************************************

>	void BrushAttrValue::SetPointsCache(PointsMap * pMap)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/99

	Inputs:		pointer to list to use as cache

	Purpose:	sets the cache member variable
********************************************************************************************/

void BrushAttrValue::SetCache(PointsMap* pMap)
{
#ifdef BRUSHPOINTSCACHE
	FlushCache();
	m_pCachedPoints = pMap;
#endif
}


/********************************************************************************************

>	void BrushAttrValue::SetPressureCache(CDistanceSampler *pSampler)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/99

	Inputs:		pointer to array to use as cache

	Purpose:	sets the pressure array memeber variable
********************************************************************************************/

void BrushAttrValue::SetPresssureCache(CDistanceSampler *pSampler)
{
	// Do we want to delete any existing cache?
	if (m_pPressureSampler != NULL)
	{
		delete m_pPressureSampler;
	}
	m_pPressureSampler = pSampler;
}


/********************************************************************************************

>	void BrushAttrValue::FlushPressureCache()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/99
	Inputs:		-
	Returns:	-
	Purpose:	clears out and deletes the cache of pressure values
********************************************************************************************/

void BrushAttrValue::FlushPressureCache()
{
	if (m_pPressureSampler != NULL)
	{
		delete m_pPressureSampler;
		m_pPressureSampler = NULL;
	}
}
/********************************************************************************************

>	PressureArray* BrushAttrValue::GetPressureCache()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/99

	Returns:	the pressure cache

	Purpose:	as above
********************************************************************************************/

CDistanceSampler* BrushAttrValue::GetPressureCache()
{
	return m_pPressureSampler;
}


/********************************************************************************************

>	UINT32 BrushAttrValue::GetPressureValue(UINT32 Index)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/99

	Returns:	the pressure value at that index, or PRESSURE_INVALID if index is out of range

	Purpose:	as above
********************************************************************************************/

UINT32 BrushAttrValue::GetPressureValue(UINT32 Index)
{
	if (m_pPressureVals == NULL)
		return PRESSURE_INVALID;
	if (Index >= (UINT32)m_pPressureVals->size())
		return PRESSURE_INVALID;

	PressureItem Item = (*m_pPressureVals)[Index];
	return Item.m_Pressure;
}


/********************************************************************************************

>	BOOL BrushAttrValue::SetPressureValue(UINT32 Index, UINT32 Value)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/99
	Inputs:		Index - the index in to the array
				Value - the value to set
	Returns:	TRUE if all went well

	Purpose:	as above
********************************************************************************************/

BOOL BrushAttrValue::SetPressureValue(UINT32 Index, PressureItem Value)
{
	if (m_pPressureVals == NULL)
		return FALSE;

	size_t				NumValues = m_pPressureVals->size();
	if (Index > NumValues)
		return FALSE;

	// if index is one greater than our current size then we can expanf
	if (Index == NumValues)
		m_pPressureVals->push_back( Value );
	else
		(*m_pPressureVals)[Index] = Value;

	return FALSE;
}




/********************************************************************************************

>	static void BrushAttrValue::ShuffleArrayUp(PressureArray* pArray, UINT32 StartIndex)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/99
	Inputs:		pArray - the array to shuffle
				StartIndex - the index to shuffle from
				
	Returns:	-

	Purpose:	shuffles each element in the array up by one, assumes that there is already 
				a spare element at the top of the array.
********************************************************************************************/

void BrushAttrValue::ShuffleArrayUp(PressureArray* pPressure, UINT32 StartIndex)
{
	if (pPressure == NULL)
	{
		ERROR3("Pressure array is NULL");
		return;
	}

	size_t				NumValues = pPressure->size();
	if (StartIndex >= NumValues)
	{
		ERROR3("Start index is greater than size of array");
		return;
	}
	
	// we count down from the top value and move
	// each one up, recall that the extra array element has already been allocated
	PressureItem Item;
	for( size_t i = NumValues - 2; i >= StartIndex; i-- )
	{
		Item = (*pPressure)[i];
	//	if (i+1 >= NumValues)
	//	{
	//		TRACEUSER("Diccon", _T("Adding array element at %d\n"), i+1);
	//		pPressure->SetAtGrow(i+1, Item);
	//	}
	//	else
			(*pPressure)[i] = Item;
	}
}



/********************************************************************************************

>	void BrushAttrValue::CalculatePressureArrayProportionalDistances(MILLIPOINT PathLength)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/99
	Inputs:		The length of the path to which this attribute applies
	Returns:	-
	Purpose:	Only use when we are utilising the path stamping cache.  This fn. proceeds
				through the list and works out the proportional distance along the path for
				each object.
	See Also:   TimeStampBrushPoint 
********************************************************************************************/

void BrushAttrValue::CalculatePressureArrayProportions(MILLIPOINT PathLength)
{
	if (m_pPressureVals == NULL || PathLength <= 0)
	{
		ERROR3("Error in calculating proportional distance");
		return;
	}
	PressureItem Item;
	for (size_t i = 0; i < m_pPressureVals->size(); i++)
	{
		Item = (*m_pPressureVals)[i];
		if (Item.m_Distance > PathLength)
			ERROR3("Item distance is greater than path length in BrushAttrValue::CalculatePressureArrayProportions");
		Item.m_Proportion = Item.m_Distance  / (double) PathLength;
		(*m_pPressureVals)[i] =  Item;
	}
}

/********************************************************************************************

>	bool BrushAttrValue::DeletePressurePoints(MILLIPOINT Start, MILLIPOINT End, SampleArray* pRemovedPoints)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     Start - distance from which to delete
				End - the distance to stop deleting at
				pRemovedPoints - an array to put the deleted points in (defaults to null)
	Returns:	TRUE if successful, FALSE if not
	Purpose:	removes points from the Pressure Sampler list between the provided distances
	SeeAlso:	-

********************************************************************************************/

BOOL BrushAttrValue::DeletePressurePoints(MILLIPOINT StartDistance, MILLIPOINT EndDistance, 
										   SampleArray* pRemovedPoints)
{
	if (StartDistance <0 || EndDistance < 0 || EndDistance <= StartDistance)
	{
		ERROR3("Invalid inputs to BrushAttrValue::DeletePressureingPoints");
		return FALSE;
	}

	if (m_pPressureSampler == NULL)
		return FALSE;

	// find out what indexes correspond to our distances
	INT32 StartIndex = m_pPressureSampler->GetInternalIndexFromDistance(StartDistance);
	INT32 EndIndex   = m_pPressureSampler->GetInternalIndexFromDistance(EndDistance);

	if (StartIndex == -1 || EndIndex == -1)
	{
		ERROR3("Attempting to remove from over the end of the array");
		return FALSE;
	}
	INT32 NumPoints = EndIndex - StartIndex;
	if (NumPoints == 0)
		return TRUE;  // ours not to reason why...

	// set the size of the sample array
	if (pRemovedPoints != NULL)
		pRemovedPoints->resize( NumPoints );

	// ask the sampler to do its thing
	return m_pPressureSampler->RemoveData(StartIndex, EndIndex, pRemovedPoints);
}


/********************************************************************************************

>	bool BrushAttrValue::DeletePressurePoints(MILLIPOINT Start, UINT32 NumPoints, PressureList* pList = NULL) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     StartIndex - the index where we want to start deleting
				NumPoints - the number to delete
				pRemovedPoints - an array to put the deleted points in (defaults to null)
	Returns:	TRUE if successful, FALSE if not
	Purpose:	removes points from the Pressure Sampler between the provided distances
	SeeAlso:	-

********************************************************************************************/

BOOL BrushAttrValue::DeletePressurePoints(UINT32 StartIndex, size_t NumPoints, 
										   SampleArray* pRemovedPoints)
{
	if (NumPoints < 1)
	{
		TRACEUSER( "Diccon", _T("Num points = %d in DeletePressurePoints\n"), NumPoints);
		return FALSE;
	}

	if (m_pPressureSampler == NULL)
		return FALSE;

	UINT32 EndIndex = StartIndex + NumPoints;

	// check that this doesn't overrun the array
	if (EndIndex > m_pPressureSampler->GetNumItems())
	{
		ERROR3("Attempting to remove over the end of the array");
		return FALSE;
	}

	// set the size of the sample array
	if (pRemovedPoints != NULL)
		pRemovedPoints->resize( NumPoints );

	// set the retrieval rate
	m_pPressureSampler->SetRetrievalSampleRate(1.0);

	// ask the sampler to do its thing
	return m_pPressureSampler->RemoveData(StartIndex, EndIndex, pRemovedPoints);
	
}


/********************************************************************************************

>	bool Bru5shAttrValue::AddPressurePoints(PressureList* pNewPoints, MILLIPOINT Start) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     pNewPoints - the points to add
				Start - distance from which to add the points, note that if this is negative then
				all the new points will be added to the very start
			
	Returns:	TRUE if successful, FALSE if not
	Purpose:	inserts the list of points after the distance given
	SeeAlso:	-

********************************************************************************************/
	
BOOL BrushAttrValue::AddPressurePoints(CSampleData* pNewPoints, MILLIPOINT StartDistance)
{
	// couple of checks
	if (pNewPoints == NULL)
	{
		ERROR3("Invalid inputs to BrushAttrValue::AddPressurePoints");
		return FALSE;
	}

	if (m_pPressureSampler == NULL)
		return FALSE;
	
	// find out where we're inserting
	INT32 StartIndex = m_pPressureSampler->GetInternalIndexFromDistance(StartDistance);
	if (StartIndex == -1)
	{
		ERROR3("Attempting to insert over the end of the array in BrushAttrValue::AddPressurePoints");
		return FALSE;
	}

	// ask the sampler to insert for us
	return m_pPressureSampler->InsertNewData((UINT32)StartIndex, pNewPoints);
}


/********************************************************************************************

>	BOOL BrushAttrValue::AddPressurePoints(PressureList* pNewPoints, UINT32 StartIndex)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     pNewPoints - the points to add
				StartIndex - the TimeStampList::iterator	in the list where we want to insert the new points
			
	Returns:	TRUE if successful, FALSE if not
	Purpose:	inserts the list of points after the index given
	SeeAlso:	-

********************************************************************************************/
	
BOOL BrushAttrValue::AddPressurePoints(CSampleData* pNewPoints, UINT32 StartIndex)
{
	if (pNewPoints == NULL)
	{
		ERROR3("Invalid inputs to BrushAttrValue::AddPressurePoints");
		return FALSE;
	}

	if (m_pPressureSampler == NULL)
		return FALSE;

	if (StartIndex > m_pPressureSampler->GetNumItems())
	{
		ERROR3("Attempting to insert over the end of the array in BrushAttrValue::AddPressurePoints");
		return FALSE;
	}

	return m_pPressureSampler->InsertNewData(StartIndex, pNewPoints);
}




/********************************************************************************************

>	void BrushAttrValue::SetPressureUpdateType(ListUpdateType Update) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     Update can be:
				UPDATE_NONE, 
				UPDATE_LIST - means that points have been added or removed to the Pressureing list
				UPDATE_PROPORTION - means that the underlying path has changed but our list has not
				UPDATE_TimeStampList::iterator	- means (usually) that the path has been reversed but our points are the same
				UPDATE_LISTANDTimeStampList::iterator	- take a guess
	Returns:	-
	Purpose:	These flags determine what will happen in the NodePathUpdate function which is called
				after the freehand/brush or bezier tool is used to edit a brushed nodepath
	SeeAlso:	-

********************************************************************************************/

void BrushAttrValue::SetPressureUpdateType(ListUpdateType Update)
{
	m_PressureUpdateType = Update;
}


/********************************************************************************************

>	ListUpdateType GetPressureUpdateType()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     -
	Returns:	the member, described above
	Purpose:	As above
	SeeAlso:	-

********************************************************************************************/

ListUpdateType BrushAttrValue::GetPressureUpdateType()
{
	return m_PressureUpdateType;
}


		
/********************************************************************************************


>	BOOL BrushAttrValue::CopyPressureList()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/5/2000
	Inputs:		-
	Returns:	pointer to a newly allocated pressure cache, or NULL if it fails
	Purpose:	To make a carbon copy of our pressure cache and all the items in it. To be used
				as part of the operator= sequence.

	SeeAlso:	BrushAttrValue::operator=
********************************************************************************************/

CDistanceSampler* BrushAttrValue::CopyPressureCache()
{
	if (m_pPressureSampler == NULL)
		return NULL;

	return m_pPressureSampler->MakeCopy();
}



/********************************************************************************************

>	INT32 BrushAttrValue::GetPressureListIndexAtDistance(MILLIPOINT Distance)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/5/2000
	Inputs:     Distance - the distance along the path that we wish to find the pressure
				list index for
			
	Returns:	the index into our pressure list, or -1 for an error
	Purpose:	Finds the pressure list index corresponding to the brush object that would
				appear at Distance along the brushed path.  Or rather asks the processor to 
				do it.
	SeeAlso:	PathProcessorBrush::GetObjectCountToDistance

********************************************************************************************/
	
INT32 BrushAttrValue::GetPressureListIndexAtDistance(MILLIPOINT Distance)
{
	// some safety checks
	if (m_pPressureSampler == NULL)
		return -1;

	if (Distance < 0 )
		return -1;

	if (m_pProcessor == NULL)
		return -1;

	INT32 Index = m_pPressureSampler->GetInternalIndexFromDistance(Distance);
	
	return Index;
}


/*------------------------------------------------------------------------------------------
------------Time Stamp List functions---------------------------------------------------------
--------------------------------------------------------------------------------------------*/


/********************************************************************************************

>	void BrushAttrValue::SetTimeStampList(TimeStampList* pList)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/99

	Inputs:		pointer to list to use as cache

	Purpose:	sets the timestamp cache member variable
********************************************************************************************/

void BrushAttrValue::SetTimeStampList(TimeStampList* pList)
{
	m_pTimeStampList = pList;
}


/********************************************************************************************

>	BOOL BrushAttrValue::IsTimeStamping

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/99

	Inputs:		-
	Returns:	TRUE if we are using the timestamp cache, FALSE otherwise (we never use both)
	Purpose:	as above
********************************************************************************************/

BOOL BrushAttrValue::IsTimeStamping()
{
	return (m_pTimeStampList != NULL);
}


/********************************************************************************************

>	void BrushAttrValue::CalculateProportionalDistances(MILLIPOINT PathLength)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/99
	Inputs:		The length of the path to which this attribute applies
	Returns:	-
	Purpose:	Only use when we are utilising the path stamping cache.  This fn. proceeds
				through the list and works out the proportional distance along the path for
				each object.
	See Also:   TimeStampBrushPoint 
********************************************************************************************/

void BrushAttrValue::CalculateProportionalDistances(MILLIPOINT PathLength)
{
	if (m_pTimeStampList == NULL || PathLength <= 0)
	{
		ERROR3("Error in calculating proportional distance");
		return;
	}

	TimeStampList::iterator	ListPos = m_pTimeStampList->begin();
	TimeStampList::iterator	LastPos = ListPos;
	TimeStampBrushPoint TSBP;
	while( ListPos != m_pTimeStampList->end() )
	{
		LastPos = ListPos;
		TSBP = *ListPos; ++ListPos;
		TSBP.WorkOutProportion( PathLength );
		*LastPos = TSBP;
	}
}


/********************************************************************************************

>	void BrushAttrValue::FlushTimeStampCache()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/99
	Inputs:		-
	Returns:	-
	Purpose:	deletes the timestamp cache if we have one
********************************************************************************************/

void BrushAttrValue::FlushTimeStampCache()
{
	if (m_pTimeStampList != NULL)
	{
		m_pTimeStampList->clear();
		delete m_pTimeStampList;
		m_pTimeStampList = NULL;
	}
}


/********************************************************************************************

>	void BrushAttrValue::TransformTimeStamp(TransformBase& Transform) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     the transformation to perform
	Returns:	-
	Purpose:	transforms all the points in the timestamping cache
	SeeAlso:	-

********************************************************************************************/

void BrushAttrValue::TransformTimeStampList(TransformBase& Trans)
{
	if (m_pTimeStampList == NULL)
		return;

	// pretty straightforward, just get the points out of the list and transform them
	TimeStampBrushPoint TSBP;
	TimeStampList::iterator	ListPos = m_pTimeStampList->begin();

	while( ListPos != m_pTimeStampList->end() )
	{
		TimeStampList::iterator	OldPos = ListPos;
		TSBP = *ListPos; ++ListPos;
		Trans.Transform(&TSBP.m_Point, 1);
		*OldPos = TSBP;
	}

	

}


/********************************************************************************************

>	bool BrushAttrValue::DeleteTimeStampPoints(MILLIPOINT Start, MILLIPOINT End) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     Start - distance from which to delete
				End - the distance to stop deleting at
	Returns:	TRUE if successful, FALSE if not
	Purpose:	removes points from the timestamping list between the provided distances
	SeeAlso:	-

********************************************************************************************/

BOOL BrushAttrValue::DeleteTimeStampPoints(MILLIPOINT StartDistance, MILLIPOINT EndDistance, 
										   TimeStampList* pRemovedPoints)
{
	if (StartDistance <0 || EndDistance < 0 || EndDistance <= StartDistance)
	{
		ERROR3("Invalid inputs to BrushAttrValue::DeleteTimeStampingPoints");
		return FALSE;
	}

	if (m_pTimeStampList == NULL)
		return FALSE;

	TimeStampList::iterator	ListPos = m_pTimeStampList->begin();
	TimeStampList::iterator	LastPos = ListPos;
	TimeStampBrushPoint		TSBP;
	MILLIPOINT				Distance = 0;
	INT32						count = 0;
	INT32						removecount = 0;
	TRACEUSER( "Diccon", _T("Deleting from %d, to %d\n"), StartDistance, EndDistance );
	TRACEUSER( "Diccon", _T("Num points pre-remove = %d\n"), m_pTimeStampList->size() );
	while( ListPos != m_pTimeStampList->end() )
	{
		LastPos = ListPos;
		TSBP = *ListPos; ++ListPos;
		Distance = TSBP.m_Distance;
		//TRACEUSER("Diccon", _T("Count %d, Distance = %d\n"), count, Distance);
		if (Distance >= StartDistance && Distance <= EndDistance)
		{
			// if we're sending the removed points back, add this one to the list
			if (pRemovedPoints != NULL)
				pRemovedPoints->push_back( TSBP );

			// remove it from the main list
			m_pTimeStampList->erase( LastPos );
			removecount++;
			TRACEUSER("Diccon", _T("Removing point %d\n"), count);
		}
		count++;
	}
	TRACEUSER("Diccon", _T("Removed %d points\n"), removecount);

	return TRUE;
}


/********************************************************************************************

>	bool BrushAttrValue::DeleteTimeStampPoints(MILLIPOINT Start, UINT32 NumPoints, TimeStampList* pList = NULL) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     Start - distance from which to delete
				NumPoints - the number to delete
				pList - a list to put the deleted points in (defaults to null)
	Returns:	TRUE if successful, FALSE if not
	Purpose:	removes points from the timestamping list between the provided distances
	SeeAlso:	-

********************************************************************************************/

BOOL BrushAttrValue::DeleteTimeStampPoints(MILLIPOINT StartDistance, UINT32 NumPoints, 
										   TimeStampList* pRemovedPoints)
{
	if (StartDistance <0 || NumPoints < 1)
	{
		ERROR3("Invalid inputs to BrushAttrValue::DeleteTimeStampingPoints");
		return FALSE;
	}

	if (m_pTimeStampList == NULL)
		return FALSE;

	TimeStampList::iterator	ListPos = m_pTimeStampList->begin();
	TimeStampList::iterator	LastPos = ListPos;
	TimeStampBrushPoint		TSBP;
	MILLIPOINT				Distance = 0;
	UINT32						count = 0;
	NumPoints --;
	while( ListPos != m_pTimeStampList->end() )
	{
		LastPos = ListPos;
		TSBP = *ListPos; ++ListPos;
		Distance = TSBP.m_Distance;
		count++;
		if (Distance >= StartDistance)
		{
			
			TimeStampList::iterator	RemPos = LastPos;
			while( NumPoints && LastPos != m_pTimeStampList->end() )
			{
				TRACEUSER("Diccon", _T("Removing point %d\n"), count);
				if (count == m_pTimeStampList->size())
				{
					//INT32 i = 1; // why is this here AMB 2006-01-31
				}
				// if we're sending the removed points back, add this one to the list
				if (pRemovedPoints != NULL)
					pRemovedPoints->push_back(TSBP);
				RemPos = LastPos;
				TSBP = *LastPos; ++LastPos;
				TimeStampBrushPoint Test = *RemPos;
		//		TRACEUSER("Diccon", _T("Removing point at %d\n"), Test.m_Distance);
				// remove it from the main list
				m_pTimeStampList->erase( RemPos );
				NumPoints--;
				count++;
			}
		
		}
		if (NumPoints <= 0 || LastPos == m_pTimeStampList->end())
			break;
	}
	return TRUE;
}

/********************************************************************************************

>	bool Bru5shAttrValue::AddTimeStampPoints(TimeStampList* pNewPoints, MILLIPOINT Start) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     pNewPoints - the points to add
				Start - distance from which to add the points, note that if this is negative then
				all the new points will be added to the very start
			
	Returns:	TRUE if successful, FALSE if not
	Purpose:	inserts the list of points after the distance given
	SeeAlso:	-

********************************************************************************************/
	
BOOL BrushAttrValue::AddTimeStampPoints(TimeStampList* pNewPoints, MILLIPOINT StartDistance)
{
	if (pNewPoints == NULL)
	{
		ERROR3("Invalid inputs to BrushAttrValue::DeleteTimeStampingPoints");
		return FALSE;
	}

	if (m_pTimeStampList == NULL)
		return FALSE;

	TimeStampList::iterator	ListPos = m_pTimeStampList->begin();
	TimeStampList::iterator	LastPos = ListPos;
	
	TimeStampBrushPoint TSBP;
	TimeStampBrushPoint NewPoint;
	MILLIPOINT Distance;
	
	TRACEUSER("Diccon", _T("Num points pre-add = %d\n"), m_pTimeStampList->size());

	// first find the TimeStampList::iterator	at which we want to insert the new points
	if (StartDistance > 0)
	{
		while( ListPos != m_pTimeStampList->end() )
		{
			LastPos = ListPos;
			TSBP = *ListPos; ++ListPos;
			Distance = TSBP.m_Distance;
			if (Distance > StartDistance)
				break;
				
		}
	}
	else
		LastPos = m_pTimeStampList->begin();

	TimeStampList::iterator	NewListPos = pNewPoints->begin();

	while( NewListPos != m_pTimeStampList->end() )
	{
		// get the point from the new list
		NewPoint = *NewListPos; ++NewListPos;
				
		// insert it into the old list, recall that LastPos stores the element just before
		// StartDistance was exceeded
		if (StartDistance > 0)
		{
			m_pTimeStampList->insert( LastPos, NewPoint );
			// increment our list position
			++LastPos;
		}
		else
			m_pTimeStampList->push_front( NewPoint );
	}

	TRACEUSER("Diccon", _T("Ended with %d points\n"), m_pTimeStampList->size());

	return TRUE;
}

/********************************************************************************************

>	bool BrushAttrValue::RecalculateTimeStampList(Path* pPath)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     pPath - the path of the nodepath of this timestamped brush
				
			
	Returns:	TRUE if successful, FALSE if not
	Purpose:	If time stamped lists are edited they can get out of shape, call this function
				to make sure the list is all in order.  Note that it can be a bit slow
				Extra note, depending on exactly how the path was edited our update method differs.
				RepositionPointsByProportion is used when the path was edited by no points were
				added or removed from the timestamping list.  If points were added then the other
				method should be used.
	SeeAlso:	-

********************************************************************************************/

BOOL BrushAttrValue::RecalculateTimeStampList(Path* pPath)
{
	if (m_pTimeStampList == NULL)
		return TRUE;
	if (pPath == NULL)
		return FALSE;
	BOOL Retval = TRUE;
	switch (m_TimeStampUpdateType)
	{
		case UPDATE_NONE:
			break;
		case UPDATE_PROPORTION:
			if (!RepositionPointsByProportion(pPath))
				Retval = FALSE;
		break;
		case UPDATE_LIST:
			if (!RecalculateDistances(pPath))
			{
				Retval =  FALSE;
				break;
			}
			if (!SortTimeStampingList())
			{
				Retval = FALSE;
				break;
			}
		//	if (!RecalculateTangents(pPath))
		//	{
		//		Retval = FALSE;
		//		break;
		//	}
		break;
		case UPDATE_POSITION:
			if (!RepositionPointsByPosition(pPath))
				Retval = FALSE;
		break;
		case UPDATE_LISTANDPOSITION:
			if (!RepositionPointsByPosition(pPath))
			{
				Retval = FALSE;
				break;
			}
			if (!RecalculateDistances(pPath))
			{
				Retval =  FALSE;
				break;
			}
			if (!SortTimeStampingList())
			{
				Retval = FALSE;
				break;
			}
		//	if (!RecalculateTangents(pPath))
		//	{
		//		Retval = FALSE;
		//		break;
		//	}
		break;
	}			

//	if (!GetRidOfCrapFromList(pPath))
//		return FALSE;
	return Retval;
}



/********************************************************************************************

>	bool BrushAttrValue::RecalculateDistances(Path* pPath)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     pPath - the path of the nodepath of this timestamped brush
				
			
	Returns:	TRUE if successful, FALSE if not
	Purpose:	for a timestamped brush, this fn. goes along the path and recalculates 
				the distances of the coordinates along the line.  
	SeeAlso:	-

********************************************************************************************/

BOOL BrushAttrValue::RecalculateDistances(Path* pPath)
{
	if (pPath == NULL || m_pTimeStampList == NULL)
	{
		ERROR3("Input pointers are NULL");
		return FALSE;
	}

	double PathLength = (MILLIPOINT)pPath->GetPathLength();
	MILLIPOINT Distance;
	
	TimeStampList::iterator	ListPos = m_pTimeStampList->begin();
	TimeStampList::iterator	LastPos;
	TimeStampBrushPoint TSBPoint;

	while( ListPos != m_pTimeStampList->end() )
	{
		LastPos = ListPos;
		TSBPoint = *ListPos; ++ListPos;
		// get the distance along the path
		if (pPath->GetDistanceToPoint(TSBPoint.m_Point, &Distance))
		{
			TSBPoint.m_Distance = Distance;
			TSBPoint.m_Proportion = (double)TSBPoint.m_Distance / PathLength;
		//	TRACEUSER("Diccon", _T("New Prop: %f\n"), TSBPoint.m_Proportion);
			*LastPos = TSBPoint;
		}
		else
			TRACEUSER("Diccon", _T("Point %d, %d, not found in line\n"), TSBPoint.m_Point.x, TSBPoint.m_Point.y);
	}
	
	return TRUE;
}

/********************************************************************************************

>	bool BrushAttrValue::RecalculateTangents(Path* pPath)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     pPath - the path of the nodepath of this timestamped brush
				
			
	Returns:	TRUE if successful, FALSE if not
	Purpose:	for a timestamped brush, this fn. goes along the path and recalculates 
				the tangents of the coordinates along the line.  
	SeeAlso:	-

********************************************************************************************/

BOOL BrushAttrValue::RecalculateTangents(Path* pPath)
{
	if (pPath == NULL || m_pTimeStampList == NULL)
	{
		ERROR3("Input pointers are NULL");
		return FALSE;
	}

//	double PathLength = (MILLIPOINT)pPath->GetPathLength();
	
	double Tangent;
	DocCoord DummyCoord;

	TimeStampList::iterator	ListPos = m_pTimeStampList->begin();
	TimeStampBrushPoint TSBPoint;

	while( ListPos != m_pTimeStampList->end() )
	{
		TSBPoint = *ListPos; ++ ListPos;
		// we need to use GetPointAtDistance to retrieve the tangent
		if (pPath->GetPointAtDistance(TSBPoint.m_Distance, &DummyCoord, &Tangent))
			TSBPoint.m_Tangent = Tangent;
	
	}
	return TRUE;
}	


/********************************************************************************************

>	bool BrushAttrValue::SortTimeStampingList()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     -
	Returns:	TRUE if successful, FALSE if not
	Purpose:	The TimeStamping points lists can become rather disorderly if you edit them a 
				lot.  This function sorts them by order of distance using quicksort.  Make sure you call 
				RecalculateDistances() first.
	SeeAlso:	-

********************************************************************************************/

BOOL BrushAttrValue::SortTimeStampingList()
{
	if (m_pTimeStampList == NULL)
		return FALSE;

	// first we need to copy the elements out of the list into an array
	size_t NumElements = m_pTimeStampList->size();
	
	// make the array
	TimeStampBrushPoint* pPointArray = new TimeStampBrushPoint[NumElements];
	if (pPointArray == NULL)
		return FALSE;

	TimeStampList::iterator	ListPos = m_pTimeStampList->begin();

	TimeStampBrushPoint PointInfo;
	UINT32 Counter = 0;

	// copy the points into the array
	while( ListPos != m_pTimeStampList->end() )
	{
		PointInfo = *ListPos; ++ListPos;

		pPointArray[Counter++] = PointInfo;
	}

	// quick sort 'em
	qsort((void*)pPointArray, NumElements, sizeof(TimeStampBrushPoint), TimeStampBrushPoint::ComparePointInfo);


	// now put 'em back in the list
	m_pTimeStampList->clear();
	
	for (unsigned i = 0; i < NumElements; i++)
	{
		m_pTimeStampList->push_back(pPointArray[i]);
	
	//	TRACEUSER("Diccon", _T("Sorted distance = %d\n"), pPointArray[i].m_Distance);
	}

	delete pPointArray;
	return TRUE;
}	


/********************************************************************************************

>	bool BrushAttrValue::GetRidOfCrapFromList(Path* pPath)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     pPath = the path that this timestamped brush supposedly follows
	Returns:	TRUE if successful, FALSE if not
	Purpose:	The TimeStamping points lists can become rather disorderly if you edit them a 
				lot.  My woefully inadequate undo/redo actions for editing them can still leave
				some rogue points hanging around. This function proceeds through the list
				removing any points that are not on the path.
	SeeAlso:	-

********************************************************************************************/

BOOL BrushAttrValue::GetRidOfCrapFromList(Path* pPath)
{
	if (m_pTimeStampList == NULL || pPath == NULL)
		return FALSE;

	TimeStampBrushPoint Point;
	TimeStampList::iterator	ListPos = m_pTimeStampList->begin();
	TimeStampList::iterator	LastPos = ListPos;
	MILLIPOINT Distance;

	while( ListPos != m_pTimeStampList->end() )
	{
		LastPos = ListPos;
		Point = *ListPos; ++ListPos;
		if (!pPath->GetDistanceToPoint(Point.m_Point, &Distance))
		{
			m_pTimeStampList->erase( LastPos );
			TRACEUSER("Diccon", _T("Removing point at %d\n"), Point.m_Distance);
		}
	}
	return TRUE;
}


/********************************************************************************************

>	bool BrushAttrValue::RepositionPointsByProportion(Path* pPath)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     pPath = the path that this timestamped brush supposedly follows
	Returns:	TRUE if successful, FALSE if not
	Purpose:	This function moves all the points in the timestamping list to their proportional
				TimeStampList::iterator	along pPath. 

	SeeAlso:	-

********************************************************************************************/

BOOL BrushAttrValue::RepositionPointsByProportion(Path* pPath)
{
	if (m_pTimeStampList == NULL || pPath == NULL)
		return FALSE;

	TimeStampBrushPoint Point;
	TimeStampList::iterator	ListPos = m_pTimeStampList->begin();
	TimeStampList::iterator	LastPos = ListPos;
	
	double PathLength = pPath->GetPathLength();
	MILLIPOINT PointDistance = 0;
	double Tangent = 0;
	DocCoord NewPosition;
	UINT32 Counter = 0;

	while( ListPos != m_pTimeStampList->end() )
	{
		LastPos = ListPos;
		Point = *ListPos; ++ListPos;
		// work out the new distance along the given path with the existing proportion
		PointDistance = (MILLIPOINT)(Point.m_Proportion * PathLength);
		if (PointDistance >= 0)
		{
			// get the coordinate and tangent for this distance
			if (pPath->GetPointAtDistance(PointDistance, &NewPosition, &Tangent))
			{
				// assign the new data to the point
				Point.m_Point = NewPosition;
				Point.m_Tangent = Tangent;
				Point.m_Distance = PointDistance;
				// put the point back in the list
				*LastPos = Point;
			}
			else
			{
				TRACEUSER("Diccon", _T("Failed to find point on line with distance %d\n"), PointDistance);
			}
		}
		else
		{
			TRACEUSER("Diccon", _T("Negative path distance at point %d\n"), Counter);
		}
		Counter++;
	}
	return TRUE;
}


/********************************************************************************************

>	bool BrushAttrValue::RepositionPointsByPosition(Path* pPath)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     pPath = the path that this timestamped brush supposedly follows
	Returns:	TRUE if successful, FALSE if not
	Purpose:	This function assumes that all the points in the timestamping list are in the
				path but that their distance and proportion information is incorrect.  Therefore
				it runs through the list and finds the correct distance and proportion.
				This is useful for those times when the path editing functions conveniently decide to 
				go and reverse the paths because they feel like it.  

	SeeAlso:	-

********************************************************************************************/

BOOL BrushAttrValue::RepositionPointsByPosition(Path* pPath)
{
	if (m_pTimeStampList == NULL || pPath == NULL)
		return FALSE;

	TimeStampBrushPoint Point;
	TimeStampList::iterator	ListPos = m_pTimeStampList->begin();
	TimeStampList::iterator	LastPos = ListPos;
	
	double PathLength = pPath->GetPathLength();
	MILLIPOINT PointDistance = 0;
//	double Tangent = 0;
	DocCoord NewPosition;
	UINT32 Counter = 0;

	while( ListPos != m_pTimeStampList->end() )
	{
		LastPos = ListPos;
		Point = *ListPos; ++ListPos;
	
		if (pPath->GetDistanceToPoint(Point.m_Point, &PointDistance))
		{
			Point.m_Distance = PointDistance;
			Point.m_Proportion = (double)PointDistance / PathLength;
			// put the point back in the list
			*LastPos = Point;
		}
		else
		{
			TRACEUSER("Diccon", _T("Failed to get distance to point at %d\n"), Counter);
		}
		Counter++;
	}
	return TRUE;
}



/********************************************************************************************

>	TimeStampList* BrushAttrValue::CopyTimeStampList()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/5/2000

	Returns:	Pointer to the new list, or null if it fails

	Purpose:	Copy our timestamping list

	SeeAlso:	operator=

********************************************************************************************/

TimeStampList* BrushAttrValue::CopyTimeStampList()
{
	if (m_pTimeStampList == NULL)
		return NULL;
	// allocate a new list of our own
	TimeStampList* pCopyList = new TimeStampList;
	if (pCopyList != NULL)
	{
		TimeStampBrushPoint TBSP;
		TimeStampList::iterator	ListPos = m_pTimeStampList->begin();
		// copy the points
		while( ListPos != m_pTimeStampList->end() )
		{
			TBSP = *ListPos; ++ListPos;
			pCopyList->push_back(TBSP);
		}	
	}
	return pCopyList;
}

/********************************************************************************************

>	virtual NodeAttribute *BrushAttrValue::MakeNode()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/99

	Returns:	Pointer to the new node, or NULL if out of memory.

	Purpose:	Make a new attribute node for this type of attr value - see base class

	SeeAlso:	AttributeValue::MakeNode

********************************************************************************************/

NodeAttribute *BrushAttrValue::MakeNode()
{
	// Create new attribute node
	AttrBrushType *pAttr = new AttrBrushType;
	if (pAttr == NULL)
		return NULL;

	// Copy attribute value (if any) into the new node.
	if (pAttr->GetAttributeValue() != NULL)
		pAttr->GetAttributeValue()->SimpleCopy(this);


	return(pAttr);
}



/********************************************************************************************

>	virtual BOOL BrushAttrValue::IsDifferent(AttributeValue *pAttr)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/99

	Purpose:	Determines if this AttrValue is different from the given one

	Errors:		ERROR3 if the two attributes are not of the same type

	SeeAlso:	AttributeValue::IsDifferent

********************************************************************************************/

BOOL BrushAttrValue::IsDifferent(AttributeValue *pAttr)
{
	ERROR3IF(!pAttr->IsKindOf(CC_RUNTIME_CLASS(BrushAttrValue)),
		   		"Different attribute types in BrushAttrValue::IsDifferent()");

	// Check they are NOT the same using the == operator
	return ( !(*((BrushAttrValue *)pAttr) == *this) );
}



/********************************************************************************************

>	virtual BrushAttrValue &BrushAttrValue::operator=(BrushAttrValue &Attrib)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/99

	Inputs:		Attrib - the attribute to copy

	Purpose:	Assignment operator

********************************************************************************************/

BrushAttrValue &BrushAttrValue::operator=(BrushAttrValue &Attrib)
{
	// Get rid of our old processor (if any)
	if (m_pProcessor != NULL)
		delete m_pProcessor;
	m_pProcessor = NULL;

	// Copy the other attr's processor. If this fails, we'll get back a NULL pointer,
	// and will simply "convert" into a simple no-stroke attribute.
	if (Attrib.m_pProcessor != NULL)
		m_pProcessor = Attrib.m_pProcessor->Clone();

	// if we're timestamping then we need to clone the list of points
	if (Attrib.IsTimeStamping())
		m_pTimeStampList = CopyTimeStampList();

	// if we have pressure data then we need to copy that also
	m_pPressureSampler = Attrib.CopyPressureCache();

	return(*this);
}



/********************************************************************************************

>	virtual INT32 BrushAttrValue::operator==(const BrushAttrValue &Attrib)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/99

	Inputs:		Attrib - the attribute to compare this attribute with

	Returns:	TRUE if the attributes are considered equal

	Purpose:	Comparison operator

********************************************************************************************/

INT32 BrushAttrValue::operator==(const BrushAttrValue &Attrib)
{
	ERROR3IF(!Attrib.IsKindOf(CC_RUNTIME_CLASS(BrushAttrValue)),
				"Other attribute value isn't an BrushAttrValue");

	BrushAttrValue *Other = (BrushAttrValue *) &Attrib;

	// two attributes can never have the same timestamp or pressure caches
	// If that is the case then why is it checking the points cache and 
	// not the pressure cache!?? I'm changing this to check the "correct" cache
	// Gerry (21/11/05)

	// The points cache doesn't get copied in the = operator so don't check it
//	if (GetCache() != NULL && GetCache()->GetCount() != 0)
//		return(FALSE);
//	if (Other->GetCache() != NULL && Other->GetCache()->GetCount != 0)
//		return(FALSE);

	if (GetPressureCache() != NULL && GetPressureCache()->GetCount() != 0)
		return(FALSE);
	if (Other->GetPressureCache() != NULL && Other->GetPressureCache()->GetCount() != 0)
		return(FALSE);

	if (m_pTimeStampList != NULL && m_pTimeStampList->size() != 0)
		return FALSE;
	if (Other->GetTimeStampList() != NULL && Other->GetTimeStampList()->size() != 0)
		return FALSE;

	// Equal if they both have same processor (only applies if they are both NULL)
	if (m_pProcessor == Other->GetPathProcessor())
		return(TRUE);

	// Otherwise, if one of them is NULL, they can't be the same
	if (m_pProcessor == NULL || Other->GetPathProcessor() == NULL)
		return(FALSE);

	// Finally, ask the processors if they are of the same type
	return(!m_pProcessor->IsDifferent(Other->GetPathProcessor()));
}



/********************************************************************************************

>	void BrushAttrValue::SetPathProcessor(PathProcessorStroke *pNewProcessor)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/99

	Inputs:		pNewProcessor - The new PathProcessorStroke to be used by this attr.
				May be NULL, in which case it sets "old style lines" stroking mode.

	Purpose:	To set the path processor used by this object. The processor is now
				"owned" by this attribute, and will be auto-deleted upon destruction

********************************************************************************************/

void BrushAttrValue::SetPathProcessor(PathProcessorBrush *pNewProcessor)
{
	if (m_pProcessor != NULL)
		delete m_pProcessor;
	m_pProcessor = pNewProcessor;

}


/********************************************************************************************

>	void BrushAttrValue::ScaleToLineWidth(MILLIPOINT Width)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/99

	Inputs:		Width - the width to set

	Purpose:	asks the processor to scale to this line width.  Note that the relevant flag
				must also be set

********************************************************************************************/

void BrushAttrValue::ScaleToLineWidth(MILLIPOINT Width)
{
	if (m_pProcessor != NULL)
	{
		BOOL Pressure = m_pPressureSampler != NULL;
		m_pProcessor->ScaleToValue(Width, !Pressure);
	}
}

/********************************************************************************************

>	BrushDefinition* BrushAttrValue::GetBrushDefintion()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/99

	Inputs:		-
	Outputs:	-
	Returns:	The brush definition used by our path processor, or null if we haven't been
				assigned one yet
	Purpose:	as above

********************************************************************************************/

BrushDefinition* BrushAttrValue::GetBrushDefinition()
{
	if (m_pProcessor != NULL)
	{
		return m_pProcessor->GetOurBrushDefinition();
	}
	return NULL;
}


/********************************************************************************************

>	BrushHandle BrushAttrValue::GetBrushHandle() 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     -
	Returns:	handle of the brush definition used by this brush (can be NULL)
	Purpose:	interface to the pathprocessor
	SeeAlso:	-

********************************************************************************************/

BrushHandle BrushAttrValue::GetBrushHandle()
{
	if (m_pProcessor == NULL)
		return BrushHandle_NoBrush;
	return m_pProcessor->GetBrushDefinitionHandle();
}


/*********************************************************************************************

>    DocRect BrushAttrValue::SetCachedRect(DocRect Rect)

     Author:    Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
     Created:   4/4/95
     Inputs:    Rect - the bounding rect of this attribute
     Outputs:   -
     Returns:   
     Purpose:   This is designed to be called by the path processor, which calculates the
				bounding rect of this attribute, arbitrarily setting bounding rects by other
				objects is not recommended
**********************************************************************************************/
       
void BrushAttrValue::SetCachedRect(DocRect Rect)
{
	m_CachedBRect = Rect;
}


/*********************************************************************************************

>    void BrushAttrValue::TransformCachedRect(TransformBase& Trans)

     Author:    Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
     Created:   4/4/95
     Inputs:    Trans - the transformation to perform
     Outputs:   -
     Returns:   
     Purpose:   Because it takes such a long time to calculate the bounding rect of a brushed object
				we prefer to calculate it once and then simply transform it.
**********************************************************************************************/
       
void BrushAttrValue::TransformCachedRect(TransformBase& Trans)
{

	
	// if we have offsets from the path then simply transforming it doesn't work,
	// better to recalculate it
	if (m_pProcessor != NULL)
	{
		if (m_pProcessor->GetPathOffsetType() != OFFSET_NONE)
			m_CachedBRect.MakeEmpty();
	}

	if (!m_CachedBRect.IsEmpty())
	{
		DocCoord Coords[4];
		Coords[0].x = m_CachedBRect.lo.x;	Coords[0].y = m_CachedBRect.lo.y;
		Coords[1].x = m_CachedBRect.hi.x;	Coords[1].y = m_CachedBRect.lo.y;
		Coords[2].x = m_CachedBRect.hi.x;	Coords[2].y = m_CachedBRect.hi.y;
		Coords[3].x = m_CachedBRect.lo.x;	Coords[3].y = m_CachedBRect.hi.y;

		Trans.Transform(Coords,4);
	
		m_CachedBRect.lo.x = min(min(Coords[0].x,Coords[1].x), min(Coords[2].x,Coords[3].x));
		m_CachedBRect.hi.x = max(max(Coords[0].x,Coords[1].x), max(Coords[2].x,Coords[3].x));
		m_CachedBRect.lo.y = min(min(Coords[0].y,Coords[1].y), min(Coords[2].y,Coords[3].y));
		m_CachedBRect.hi.y = max(max(Coords[0].y,Coords[1].y), max(Coords[2].y,Coords[3].y));
	}
	
}
/*********************************************************************************************

>    DocRect NodeAttribute::GetAttrBoundingRect(NodeRenderableInk* pParent)

     Author:    Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
     Created:   4/4/95
     Inputs:    pParent, the parent Ink node that this attribute will effect.
     Outputs:   -
     Returns:   The Bounding rect of the attribute (NULL rectangle is it doesn't have any).
     Purpose:   Virtual function for determining the bounding rect of an attribute.  It achieves this
				by giving a path to the path processor which tells it the bounding rect.
	 		
	 SeeAlso:	NodeAttribute::EffectsParentBounds

**********************************************************************************************/
       
DocRect BrushAttrValue::GetAttrBoundingRect(NodeRenderableInk* pParent, CCAttrMap* pAttribMap)
{
	BrushDefinition* pBrushDef = GetBrushDefinition();
	if (pBrushDef == NULL)
		return DocRect(0, 0, 0, 0);

	// get the biggest BBox of the brush objects
	DocRect BRect = pBrushDef->GetLargestBoundingBox();
	DocRect ReturnRect(0,0,0,0);
	
//	if (!m_CachedBRect.IsEmpty())
	//	return m_CachedBRect;
	
	
	// get our path processor
	PathProcessorBrush* pPPB = GetPathProcessor();
	if (pPPB == NULL)
		return DocRect(0,0,0,0);

	if (m_pTimeStampList != NULL)
	{
		ReturnRect =  pPPB->TimeStampBoundingBox();
		m_CachedBRect = ReturnRect;
		return ReturnRect;
	}

	if (pParent->IsNodePath())
	{	
#ifdef NEWFASTBRUSHES
		if (pCachedBitmap)
		{
			ReturnRect = pCachedBitmap->GetBoundingRect ();
		}
		else
		{
#endif
		// we have a special calculation for nodepaths, because we can use their path
		ReturnRect =  pPPB->CalculateBoundingBox(&((NodePath*)pParent)->InkPath, pParent);
#ifdef NEWFASTBRUSHES
		}
#endif
		m_CachedBRect = ReturnRect;
		return ReturnRect;
	}

	BecomeA TestBecomeAPath(BECOMEA_TEST, CC_RUNTIME_CLASS(NodePath));
	if (pParent->CanBecomeA(&TestBecomeAPath))
	{
		// we must be applied to some arbitrary shape.  The thing is that we need to have
		// a path to pass to the PPB, so want to get a passback of all the paths in 
		// the node	
		
		// we need to allocate a path
		Path* pPath = new Path;
		if (pPath != NULL && pPath->Initialise())
		{
			PathBecomeA BecomeAPath(BECOMEA_PASSBACK, CC_RUNTIME_CLASS(NodePath), NULL, FALSE, pPath);
			if (pParent->DoBecomeA(&BecomeAPath))
			{
				ReturnRect =  pPPB->CalculateBoundingBox(pPath, pParent);
			}
			delete pPath;
		}
		m_CachedBRect = ReturnRect;
		return ReturnRect;
	}

	if (pParent->IsATextChar())
	{	
	PORTNOTETRACE("text","BrushAttrValue::GetAttrBoundingRect - removed IsATextChar code");
#ifndef EXCLUDE_FROM_XARALX
		// text always has to be different doesn't it..
	
		// create a format region to keep an attribute stack
		FormatRegion FRegion;
		FormatRegion* pFormatRegion=&FRegion;

		if (!pFormatRegion->Init(pParent))
		{
			ERROR3("Failed to init format region");
			return ReturnRect;
		}

		pFormatRegion->SaveContext();

		NodePath* pNodePath = NULL;
		TextChar* pTextChar = (TextChar*)pParent;
		pTextChar->RenderChildAttrs(pFormatRegion);
		
		// create a nodepath that we can use to give the the PPB
		if (pTextChar->CreateNodePath(&pNodePath, pFormatRegion))
		{
			if (pNodePath != NULL)
			{
				ReturnRect = pPPB->CalculateBoundingBox(&(pNodePath->InkPath), pParent);
				delete pNodePath;
			}
		}
		pFormatRegion->RestoreContext();
	
		m_CachedBRect = ReturnRect;
		return ReturnRect;
#endif
	}

	// Still nothing? Last resort time.  
	if (ReturnRect.IsEmpty())
	{
		ReturnRect = pParent->GetBoundingRect(TRUE);
	
		double Scaling = 1.0;
		PathProcessorBrush* pPathProc = GetPathProcessor();
	
		if (pPathProc != NULL)
			Scaling = pPathProc->GetBrushScaling();
		// hack it a bit just to make sure
		if (!ReturnRect.IsEmpty())
			ReturnRect.Inflate((MILLIPOINT)((BRect.Width() / 2) * Scaling) , (MILLIPOINT)((BRect.Height() / 2) * Scaling));

		if (pPathProc->GetPathOffsetType() != OFFSET_NONE)
			ReturnRect.Inflate(pPathProc->GetPathOffsetValue(), pPathProc->GetPathOffsetValue());
	}

	m_CachedBRect = ReturnRect;
	return ReturnRect;
}	




/********************************************************************************************

> static void BrushAttrValue::RotateBounds(double Angle, DocRect* pBounds)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/2/2000
	Inputs:		Bounds = the bounds to rotate
				Angle  = the angle in degrees by which to rotate it by
	Outputs:	Bounds is updated
	Returns:	TRUE, unless you try to pass in a dud rect
	Purpose:	The bounds are updated to contain the rotated version
	SeeAlso:	-

********************************************************************************************/

BOOL BrushAttrValue::RotateBounds(double Angle, DocRect* pBounds, DocCoord* pCentre)
{
	ERROR2IF(pBounds == NULL, FALSE, "DocRect is NULL");
//	if (Angle != 0.0)
	{

		DocCoord BBCentre;
		if (pCentre == NULL)
			BBCentre = pBounds->Centre();
		else
			BBCentre = *pCentre;

		DocCoord Coords[4];
		Coords[0].x = pBounds->lo.x;	Coords[0].y = pBounds->lo.y;
		Coords[1].x = pBounds->hi.x;	Coords[1].y = pBounds->lo.y;
		Coords[2].x = pBounds->hi.x;	Coords[2].y = pBounds->hi.y;
		Coords[3].x = pBounds->lo.x;	Coords[3].y = pBounds->hi.y;

		Trans2DMatrix Rotate(BBCentre,Angle);
		Rotate.Transform(Coords,4);

		pBounds->lo.x = min(min(Coords[0].x,Coords[1].x), min(Coords[2].x,Coords[3].x));
		pBounds->hi.x = max(max(Coords[0].x,Coords[1].x), max(Coords[2].x,Coords[3].x));
		pBounds->lo.y = min(min(Coords[0].y,Coords[1].y), min(Coords[2].y,Coords[3].y));
		pBounds->hi.y = max(max(Coords[0].y,Coords[1].y), max(Coords[2].y,Coords[3].y));
	}
	return TRUE;
}

/********************************************************************************************

>	MILLIPOINT BrushAttrValue::GetDefaultLineWidth(BOOL IgnorePressure)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/99

	Inputs:		-
	Outputs:	-
	Returns:	the default line width as defined by the largest bounding box of the brush definition,
				or -1 if this attribute has no brush definition
	Purpose:	as above

********************************************************************************************/

MILLIPOINT BrushAttrValue::GetDefaultLineWidth(BOOL IgnorePressure)
{
	if (m_pProcessor == NULL)
		return - 1;

	return m_pProcessor->GetBrushSize(IgnorePressure);
}

/********************************************************************************************

>	void BrushAttrValue::FlushCache() 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     -
	Returns:	-
	Purpose:	clears out the cached points
	SeeAlso:	-

********************************************************************************************/

void BrushAttrValue::FlushCache()
{
#ifdef BRUSHPOINTSCACHE
	if (m_pCachedPoints != NULL)
	{
		m_pCachedPoints->clear();
		delete m_pCachedPoints;
		m_pCachedPoints = NULL;
	}

#endif

}

/********************************************************************************************

>	void BrushAttrValue::TransformCache(TransformBase& Trans)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/11/2000
	Inputs:     transfromation matrix
	Returns:	-
	Purpose:	transforms the cached path.  Attempts to optimise straight transform case.
	SeeAlso:	-

********************************************************************************************/

void BrushAttrValue::TransformCache(TransformBase& Trans)
{
	if (m_pCachedPoints)
	{
		BrushPointInfo	PointInfo;
		PointsMap::iterator	pos = m_pCachedPoints->begin();

		TransformBase *pBase = &Trans;
		if (pBase->IS_KIND_OF(Trans2DMatrix))
		{
			Trans2DMatrix* transMatrix = (Trans2DMatrix*) pBase;
			// check for a change in aspect
			if (transMatrix->GetMatrix().IsTranslation())
			{
				// optimise translate ....

				DocCoord trans = transMatrix->GetTranslation ();

				while( pos != m_pCachedPoints->end() )
				{
					pos->second.m_Point.x += trans.x;
					pos->second.m_Point.y += trans.y;

					++pos;
				}
			}
			else
			{
				while( pos != m_pCachedPoints->end() )
				{
					Trans.Transform( &pos->second.m_Point, 1 );

					++pos;
				}
			}
		}
		else
		{
			while( pos != m_pCachedPoints->end() )
			{
				Trans.Transform( &pos->second.m_Point, 1 );

				++pos;
			}
		}
	}
}



/********************************************************************************************

>	void BrushAttrValue::SetTimeStampUpdateType(ListUpdateType Update) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     Update can be:
				UPDATE_NONE, 
				UPDATE_LIST - means that points have been added or removed to the timestamping list
				UPDATE_PROPORTION - means that the underlying path has changed but our list has not
				UPDATE_TimeStampList::iterator	- means (usually) that the path has been reversed but our points are the same
				UPDATE_LISTANDTimeStampList::iterator	- take a guess
	Returns:	-
	Purpose:	These flags determine what will happen in the NodePathUpdate function which is called
				after the freehand/brush or bezier tool is used to edit a brushed nodepath
	SeeAlso:	-

********************************************************************************************/

void BrushAttrValue::SetTimeStampUpdateType(ListUpdateType Update)
{
	m_TimeStampUpdateType = Update;
}


/********************************************************************************************

>	ListUpdateType GetTimeStampUpdateType()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     -
	Returns:	the member, described above
	Purpose:	As above
	SeeAlso:	-

********************************************************************************************/

ListUpdateType BrushAttrValue::GetTimeStampUpdateType()
{
	return m_TimeStampUpdateType;
}

		
/********************************************************************************************


>	BOOL BrushAttrValue::WriteTimeStampList(BaseCamelotFilter *pFilter)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/99
	Inputs:		pfilter - filter to write to
	Returns:	TRUE if the list is correctly written
	Purpose:	If this brush uses timestamping then this function writes the timestamped
				list to the filter provided.  

	SeeAlso:	AttrBrushType::WritePreChildrenNative; BrushAttrRecordHandler::HandleRecord

********************************************************************************************/

BOOL BrushAttrValue::WriteTimeStampList(BaseCamelotFilter* pFilter)
{
	ERROR2IF(pFilter == NULL, FALSE, "Filter is NULL in BrushAttrValue::WriteTimeStampList");

	if (m_pTimeStampList == NULL)
		return TRUE; // return TRUE else we'll spring an error

	INT32 NumObjects = (INT32)m_pTimeStampList->size();
	// make ourselves a record
	CXaraFileRecord Record ( static_cast<INT32> ( TAG_TIMESTAMPBRUSHDATA ),
							 static_cast<INT32> ( TAG_TIMESTAMPBRUSHDATA_SIZE ) );
	BOOL ok = Record.Init();

	// the first entry in the record is how many objects there are
	if (ok) ok = Record.WriteINT32(NumObjects);
	
	// now write the data
	TimeStampBrushPoint ListItem;
	TimeStampList::iterator	ListPos = m_pTimeStampList->begin();
	while( ok && ListPos != m_pTimeStampList->end() )
	{
		ListItem = *ListPos; ++ListPos;
		ok = ListItem.WriteNative(&Record);
	}

	if (ok) ok = pFilter->Write(&Record);

	return ok;
}

		
/********************************************************************************************


>	BOOL BrushAttrValue::WritePressureData(BaseCamelotFilter *pFilter)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/99
	Inputs:		pfilter - filter to write to
	Returns:	TRUE if the list is correctly written
	Purpose:	If this attribute contains pressure data then it will write out that data to 
				the filter provided.

	SeeAlso:	AttrBrushType::WritePreChildrenNative; BrushAttrRecordHandler::HandleRecord

********************************************************************************************/

BOOL BrushAttrValue::WritePressureData(BaseCamelotFilter* pFilter)
{
	ERROR2IF(pFilter == NULL, FALSE, "Filter is NULL in BrushAttrValue::WriteTimeStampList");

	if (m_pPressureSampler == NULL)
		return TRUE; // return TRUE else we'll spring an error

	// make ourselves a record
	CXaraFileRecord Record ( static_cast<INT32> ( TAG_BRUSHPRESSURESAMPLEDATA ),
							 static_cast<INT32> ( TAG_BRUSHSAMPLEDATA_SIZE ) );
	BOOL ok = Record.Init();

	if (ok) ok = m_pPressureSampler->WriteNative(&Record);

	if (ok) ok = pFilter->Write(&Record);

	return ok;

}

/*--------------------------------------------------------------------------
----------------------------------------------------------------------------
			AttrBrushType class
*/


/********************************************************************************************

>	AttrBrushType::AttrBrushType()

    Author:     Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
    Created:    7/1/97

	Purpose:	Default constructor for AttrBrushType

********************************************************************************************/

AttrBrushType::AttrBrushType()
{
	m_pFreeTool = NULL;
	m_pLocalStrokeColour = NULL;
	m_pBlendedStrokeColour = NULL;
	m_LocalTranspType = 0;
	m_LocalTranspValue = 0;
}



/********************************************************************************************

>	AttrBrushType::AttrBrushType(Node *ContextNode,
										AttachNodeDirection Direction,
										BOOL Locked,
										BOOL Mangled,
										BOOL Marked,
										BOOL Selected)
    Author:     Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
    Created:    7/1/97

	Purpose:	Constructs an AttrBrushType Attribute

********************************************************************************************/

AttrBrushType::AttrBrushType(Node *ContextNode,
									AttachNodeDirection Direction,
									BOOL Locked,
									BOOL Mangled,
									BOOL Marked,
									BOOL Selected)
				: NodeAttribute(ContextNode, Direction, Locked, Mangled, Marked, Selected)
{
	m_pFreeTool = NULL;
	m_pLocalStrokeColour = NULL;
	m_pBlendedStrokeColour = NULL;
	m_LocalTranspType = 0;
	m_LocalTranspValue = 0;
}


/********************************************************************************************

>	AttrBrushType::~AttrBrushType()
    Author:     Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
    Created:    7/1/97

	Purpose:	destructor

********************************************************************************************/

AttrBrushType::~AttrBrushType()
{
	// if we are a blended attribute then our processor must destroy its blended brushdefinition
	/*PathProcessorBrush* pPPB =*/ m_Value.GetPathProcessor();
	
	// ask our attrval to delete its path processor
	m_Value.SetPathProcessor(NULL);
	
}


/********************************************************************************************

>	virtual void AttrBrushType::Render(RenderRegion *pRender)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/1/97

	Purpose:	Renders this attribute (by simply calling the Render function of
				its contained AttributeValue)

********************************************************************************************/

void AttrBrushType::Render(RenderRegion *pRender)
{

	// what it says
	FindNearestStrokeTransp();

	// tell the processor who it belongs to 
	PathProcessorBrush* pPPB = m_Value.GetPathProcessor();
	if (pPPB != NULL)
		pPPB->SetParentAttribute(this);

	if (m_pFreeTool != NULL)
	{
		String_32 ProgressString = _T("Processing Brush");
		Progress Hourglass(&ProgressString, -1, FALSE);
		TRACEUSER("Diccon", _T("About to do first render\n") );
	
		GetAttributeValue()->Render(pRender);

		// if we have pressure then we must invalidate our parent nodepath
		Node* pParent = FindParent();
		if (pParent != NULL && pParent->IsNodePath() && ContainsPressureCache())
		{
			((NodeRenderableInk*)pParent)->InvalidateBoundingRect();
		}
		m_pFreeTool = NULL;
	}
	else
	{
#ifdef NEWFASTBRUSHES
		BrushAttrValue* pAttr = (BrushAttrValue*) GetAttributeValue();
		pAttr->SetBoundsParent (FindParent ());
#endif
		GetAttributeValue()->Render(pRender);
	}
}

/********************************************************************************************

>	SubtreeRenderState AttrBrushType::RenderSubtree(RenderRegion *pRender, Node** ppNextNode, BOOL bClip = TRUE)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/02/94
	Inputs:		pRender - A pointer to the current render region
	Returns:	TRUE  => This node should be rendered,
				FALSE => This node does not need to be rendered.
	Purpose:	Basically we want to avoid the situation of recursive path processing for 
				brushes.  Therefore if there is already a path processor of the same type
				as this one on the stack then we should not render

	SeeAlso:	Node::NeedsToRender

********************************************************************************************/

SubtreeRenderState AttrBrushType::RenderSubtree(RenderRegion *pRender, Node** ppNextNode, BOOL bClip)
{
//	BOOL Retval = TRUE;
	PathProcessorBrush* pPPB = GetPathProcessor();
	if (pPPB != NULL)
	{
		// pRender can legally be null, but we don't want to proliferate passing null pointers around
		if (pRender != NULL && pPPB->IsThisPathProcessorOnTheStack(pRender))
			return SUBTREE_NORENDER;
	}

	return SUBTREE_ROOTONLY;
}


/*********************************************************************************************

>    BOOL AttrBrushType::EffectsParentBounds() const

     Author:    Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
     Created:   20/12/99
     Inputs:    -
     Outputs:   -
     Returns:   TRUE if the node will effect the bounds of it's parent.
     Purpose:   Virtual function for determining if the node will effect it's parent bounds.
	 			eg. ArrowHeads.
	 SeeAlso:	NodeAttribute::GetAttrBoundingRect

**********************************************************************************************/
       
BOOL AttrBrushType::EffectsParentBounds()
{
	return TRUE;
}


/*********************************************************************************************

>    DocRect AttrBrushType::GetAttrBoundingRect(NodeRenderableInk* pParent)

     Author:    Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
     Created:   20/12/99
     Inputs:    pParent, the parent Ink node that this attribute will effect.
				pAttribMap - not used
     Outputs:   -
     Returns:   The Bounding rect of the attribute (NULL rectangle is it doesn't have any).
     Purpose:   Virtual function for determining the bounding rect of an attribute.
	 			Simply calls the brush value which does the calculation
	 SeeAlso:	NodeAttribute::EffectsParentBounds

**********************************************************************************************/
       
DocRect AttrBrushType::GetAttrBoundingRect(NodeRenderableInk* pParent, CCAttrMap* pAttribMap )
{
	ERROR2IF(pParent == NULL, DocRect(0,0,0,0) , "Parent node is NULL in AttrBrushType::GetAttrBoundingRect");

	BrushAttrValue* pVal = (BrushAttrValue*)GetAttributeValue();
	if (pVal == NULL)
		return DocRect(0, 0, 0, 0);
	else
	{
		// bit of a hack - the PPB needs to know that we are its parents, and if we have not yet rendered then
		// it may not know that yet
		PathProcessorBrush* pPPB = pVal->GetPathProcessor();
		if (pPPB != NULL)
		{
			pPPB->SetParentAttribute(this);
			
			// set the line width
			MILLIPOINT Width = GetAppliedLineWidth();
			if (Width > 500)
			{
				SetValueToScaleTo(Width);
			}
			else
				return DocRect(0, 0, 0, 0);  // if its too small then just return as we'll generate infinite objects
		}
		
	}
	return pVal->GetAttrBoundingRect(pParent);

}

/***********************************************************************************************

>	virtual void AttrBrushType::CopyNodeContents(AttrBrushType *NodeCopy)

    Author:     Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
    Created:    13/12/99

    Outputs:    NodeCopy - returned containing a copy of this node
		 
    Purpose:	Copies the node's contents to the node pointed to by NodeCopy

***********************************************************************************************/

void AttrBrushType::CopyNodeContents(AttrBrushType *NodeCopy)
{
	// Let the base class do its bit
	NodeAttribute::CopyNodeContents(NodeCopy);

	// And then copy our Value
	*(NodeCopy->GetAttributeValue()) = *(GetAttributeValue());
}



/***********************************************************************************************
>   void AttrBrushType::PolyCopyNodeContents(NodeRenderable* pNodeCopy)

    Author:     Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
    Created:    18/12/2003
    Outputs:    -
    Purpose:    Polymorphically copies the contents of this node to another
	Errors:     An assertion failure will occur if NodeCopy is NULL
    Scope:      protected
								     
***********************************************************************************************/

void AttrBrushType::PolyCopyNodeContents(NodeRenderable* pNodeCopy)
{
	ENSURE(pNodeCopy, "Trying to copy a node's contents into a NULL node");
	ENSURE(IS_A(pNodeCopy, AttrBrushType), "PolyCopyNodeContents given wrong dest node type");

	if (IS_A(pNodeCopy, AttrBrushType))
		CopyNodeContents((AttrBrushType*)pNodeCopy);
}



/***********************************************************************************************

>	virtual INT32 AttrBrushType::operator==(const NodeAttribute &NodeAttrib); 

    Author:     Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
    Created:    13/12/99

	Inputs:		NodeAttrib - The node to compare this node to
	Returns:	TRUE if the nodes are considered equal

    Purpose:	Comparison operator - determines if the AttributeValues	of both objects are ==

***********************************************************************************************/

INT32 AttrBrushType::operator==(const NodeAttribute &NodeAttrib)
{
	// First check they are of the same type
	if (((NodeAttribute*)&NodeAttrib)->GetAttributeType() != GetAttributeType())
		return FALSE;

	// Make a more sensible pointer
	AttrBrushType *Attr = (AttrBrushType *) &NodeAttrib;

	// Now let the AttributeValues compare themselves
	return( *((BrushAttrValue *) Attr->GetAttributeValue())  ==
			*((BrushAttrValue *) GetAttributeValue()) );
}



/********************************************************************************************

>	virtual Node *AttrBrushType::SimpleCopy()

    Author:     Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
    Created:    13/12/99

    Returns:    A copy of the node, or NULL if memory runs out 

    Purpose:    This method returns a shallow copy of the node with all Node pointers NULL. 
				The function is virtual, and must be defined for all derived classes.  

********************************************************************************************/

Node *AttrBrushType::SimpleCopy()
{
	AttrBrushType* NodeCopy = new AttrBrushType;
	if (NodeCopy == NULL)
		return(NULL);

	// Call the base class
	NodeAttribute::CopyNodeContents(NodeCopy);

	// And call our AttributeValue to copy itself too
	NodeCopy->GetAttributeValue()->SimpleCopy(GetAttributeValue());
    
	// tell the new path processor who is boss
	PathProcessorBrush* pPPB = NodeCopy->GetPathProcessor();
	if (pPPB != NULL)
		pPPB->SetParentAttribute(NodeCopy);

	return(NodeCopy);
}



/********************************************************************************************

>	virtual UINT32 AttrBrushType::GetAttrNameID(void)

    Author:     Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
    Created:    13/12/99

	Returns:	Attribute description string-resource ID

	Purpose:	Retrieves a string resource ID describing this attribute

********************************************************************************************/

UINT32 AttrBrushType::GetAttrNameID(void)
{
	return(_R(IDS_ATTRBRUSH));
}

/********************************************************************************************
>	virtual BOOL AttrStokeType::NeedsTransparency() const

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/2/97
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if this Attribute requires transparency mode to render properly.
	Purpose:	Strokes like the airbrush require transparency to be turned on to work.
	Errors:		-
********************************************************************************************/

BOOL AttrBrushType::NeedsTransparency() const
{
	BOOL SoWeDoReallyNeedTransparencyThenDoWe = FALSE;
	
	PathProcessorBrush *pPathProc = m_Value.GetPathProcessor();
	if(pPathProc != NULL)
	{
		SoWeDoReallyNeedTransparencyThenDoWe = pPathProc->NeedsTransparency();
	}

	return SoWeDoReallyNeedTransparencyThenDoWe;
}


/********************************************************************************************

>	virtual void AttrBrushType::GetDebugDetails(StringBase *Str)

    Author:     Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
    Created:    13/12/99

	Outputs:	On return, Str is filled in with details on this node

	Purpose:	Produces debug details about this node

********************************************************************************************/

void AttrBrushType::GetDebugDetails(StringBase *Str)
{
#ifdef _DEBUG
	NodeAttribute::GetDebugDetails(Str);

	String_256 TempStr;
	TempStr._MakeMsg( _T("\r\nBrush #1%s\r\n"),
						(m_Value.GetPathProcessor() == NULL) ? _T("old-style line") : _T("new stroke:") );
	*Str += TempStr;

	if (m_Value.GetPathProcessor() != NULL)
	{
		TempStr._MakeMsg( _T("  #1%s\r\n"),
							m_Value.GetPathProcessor()->GetRuntimeClass()->GetClassName() );
		*Str += TempStr;
	}

	//TempStr._MakeMsg("\r\nNeedsTransparency=#1%s\r\n", NeedsTransparency() ? "TRUE" : "FALSE");
	//*Str += TempStr;

	TempStr._MakeMsg( _T("\r\nDefinition Handle=#1%d\r\n"), GetBrushHandle() );
	*Str += TempStr;
#endif
}

/********************************************************************************************

>	BOOL AttrBrushType::Blend(BlendAttrParam* pBlendParam)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/3/2000
	Inputs:		pBlendParam - contains the blend info, 
	Outputs:	pBlendParama -  containing the blended attributes
	Returns:	TRUE - Blend went ok, the blended attr val is valid; FALSE if not.
	Purpose:	Blends between two brush attributes. In fact at the moment it just blends
				between the parameters in the two path processors.  At some point it should
				also blend between the ink objects and attributes.
			
	Errors:		Out of memory.

********************************************************************************************/

BOOL AttrBrushType::Blend(BlendAttrParam* pBlendParam)
{
	// Check entry param isn't NULL
	ERROR3IF(pBlendParam == NULL,"NULL entry param");
	if (pBlendParam == NULL) return FALSE;

	// Get the Value member to blend to the Value member of the other NodeAttribute.
	// If it succeeds, ask the blended Value to make a NodeAttribute out of itself.

	if (GetAttributeValue()->Blend(pBlendParam))
	{
		// Get the blended attr val. After this call, the ptr is our reponsibility
		// so we have to delete it if it's no longer needed
		AttributeValue* pBlendedAttrVal = pBlendParam->GetBlendedAttrVal();

		if (pBlendedAttrVal != NULL)
		{
			// We have a blended attr val, so ask it to make a NodeAttribute out of itself
			// and set the pBlendParam's blended NodeAttribute ptr to it
			NodeAttribute* pBlendedAttr = pBlendedAttrVal->MakeNode();
			pBlendParam->SetBlendedAttr(pBlendedAttr);

			if (pBlendedAttr != NULL)
			{
				// We were able to make a blended NodeAttribute
				// so delete the blended attr val, and return TRUE
				((BrushAttrValue*)pBlendedAttrVal)->SetPathProcessor(NULL);
				delete pBlendedAttrVal;
				return TRUE;
			}
			else
			{
				// Couldn't make a blended NodeAttribute, so give the blended attr val back
				// and return FALSE
				pBlendParam->SetBlendedAttrVal(pBlendedAttrVal);
				return FALSE;
			}
		}
	}

	return FALSE;
}



/********************************************************************************************

>	virtual BOOL AttrBrushType::DoBecomeA(BecomeA* pBecomeA, Node* pParent)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/3/2000
	Inputs:		pBecomeA - the becomeA object that tells us what to do
				pParent - the node that this attribute is applied to
	Outputs:	
	Returns:	TRUE if everything went ok,
	Purpose:	Its a little unusual for attributes to have their own dobecomea function but 
				the brush requires one because the node that it is applied to will not know 
				anything about it.  It is designed to be called by the node that has this 
				brush applied to it.
				Note that all the real work goes on in the attribute value and path processor.

********************************************************************************************/

BOOL AttrBrushType::DoBecomeA(BecomeA* pBecomeA, Node* pParent)
{
	ERROR2IF(pBecomeA == NULL, FALSE, "BecomeA pointer is NULL in AttrBrushType::DoBecomeA");
	ERROR2IF(pParent == NULL, FALSE, "Parent node is NULL in AttrBrushType::DoBecomeA");
	PathProcessorBrush* pPPB = m_Value.GetPathProcessor();
	if (pPPB)
		pPPB->SetParentAttribute(this);
	
	return m_Value.DoBecomeA(pBecomeA, pParent);
}

/********************************************************************************************

>	virtual UINT32 AttrBrushType::GetNodeSize() const

    Author:     Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/99

	Returns:	The size of this node, in bytes

	Purpose:	For finding the size of the node, in bytes

********************************************************************************************/

UINT32 AttrBrushType::GetNodeSize() const
{
	return(sizeof(AttrBrushType));
}


/********************************************************************************************

>	virtual UINT32 AttrBrushType::GetNodeSize() const

    Author:     Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/99

	Returns:	The size of this node, in bytes

	Purpose:	For finding the size of the node, in bytes

********************************************************************************************/

String_32 AttrBrushType::GetBrushName()
{
	String_32 Name = TEXT(" ");

	BrushDefinition* pDef = GetBrushDefinition();
	if (pDef != NULL)
	{
		String_32* pName = pDef->GetLineName();
		if (pName != NULL)
			Name = *pName;
	}
	return Name;
}

/********************************************************************************************

>	void AttrBrushType::FindNearestStrokeColour()

    Author:     Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/99

	Returns:	-

	Purpose:	Locates the nearest stroke colour attribute, takes the colour and stores
				it in our member variable.  Note that it also converts the colour if it is
				an IndexedColour.

********************************************************************************************/

void AttrBrushType::FindNearestStrokeColour()
{
	NodeRenderableInk* pParent = (NodeRenderableInk*)FindParent();
	if (pParent == NULL)
	{
	//	ERROR3("Wheres my parent?!");  Might be a default, so don't error
		return;
	}
	// if its a controller we want its ink node
	if (pParent->IsCompoundClass())
	{
		NodeRenderableInk* pInk = ((NodeCompound*)pParent)->GetInkNodeFromController();
		if (pInk)
			pParent = pInk;
	}
	
	
	NodeAttribute* pStrokeColour = NULL;
	
	// get the attribute
	pParent->FindAppliedAttribute(CC_RUNTIME_CLASS(AttrStrokeColour), &pStrokeColour);

	if (pStrokeColour == NULL)
		return;

	// get the stroke colour
	StrokeColourAttribute* pVal = (StrokeColourAttribute*)pStrokeColour->GetAttributeValue();
	if (pVal != NULL)
	{
		// we're only interested in the actual colour
		m_pLocalStrokeColour = &(pVal->Colour);
	}
	else
		TRACEUSER("Diccon", _T("Unable to find local stroke colour\n") );

}


/********************************************************************************************

>	void AttrBrushType::FindNearestStrokeTransp()

    Author:     Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/99
	Returns:	-
	Purpose:	Finds the local stroke transparency and caches the transparency value and type
********************************************************************************************/

void AttrBrushType::FindNearestStrokeTransp()
{
	m_LocalTranspValue = 0;
	m_LocalTranspType = 0;

	NodeRenderableInk* pParent = (NodeRenderableInk*)FindParent();
	if (pParent == NULL || !pParent->IsAnObject())
	{
	//	ERROR3("Wheres my parent?!");  Might be a default, so don't error
		return;
	}

	// if its a controller we want its ink node
	if (pParent->IsCompoundClass())
	{
		NodeRenderableInk* pInk = ((NodeCompound*)pParent)->GetInkNodeFromController();
		if (pInk)
			pParent = pInk;
	}

	NodeAttribute* pStrokeTransp = NULL;

	// get the attribute
	pParent->FindAppliedAttribute(CC_RUNTIME_CLASS(AttrStrokeTransp), &pStrokeTransp, FALSE);

	if (pStrokeTransp == NULL)
		return;

	// get the stroke transparency
	StrokeTranspAttribute* pVal = (StrokeTranspAttribute*)pStrokeTransp->GetAttributeValue();
	if (pVal != NULL)
	{
		// we're only interested in the actual colour
		m_LocalTranspValue = *pVal->GetStartTransp();
		m_LocalTranspType = pVal->GetTranspType();
	}	
}

/********************************************************************************************

>	DocColour AttrBrushType::GetLocalStrokeColour()

    Author:     Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/99

	Returns:	the colour of the stroke that this attribute is applied to, or a default 'blank'
				colour if we don't have a stroke

	Purpose:	Returns either the blended or local stroke colour.  Given that the blended colour
				is null unless we are in a blend it takes priority when it does exist.

********************************************************************************************/

DocColour AttrBrushType::GetLocalStrokeColour()
{
	DocColour Col;
	if (m_pBlendedStrokeColour != NULL)
		Col = * m_pBlendedStrokeColour;
	else if (m_pLocalStrokeColour != NULL)
		Col = *m_pLocalStrokeColour;

	return Col;
}


/********************************************************************************************

>	void AttrBrushType::SetBlendedStrokeColour(DocColour* pCol)

    Author:     Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/99
	Inputs:		pointer to stroke colour in the blend
	Returns:	-
	Purpose:	Tells the attribute that pCol is the stroke colour for the current blend step.
********************************************************************************************/

void AttrBrushType::SetBlendedStrokeColour(DocColour* pCol)
{
	m_pBlendedStrokeColour = pCol;
}


/********************************************************************************************

>	UINT32 AttrBrushType::GetLocalTranspValue()

    Author:     Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/99

	Returns:	the value of the local stroke transparency that we cached
	Purpose:	-
********************************************************************************************/

UINT32 AttrBrushType::GetLocalTranspValue()
{
	return m_LocalTranspValue;
}


/********************************************************************************************

>	DocColour AttrBrushType::GetLocalStrokeColour()

    Author:     Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/99

	Returns:	the type of the local stroke transparency that we cached
	Purpose:	-
********************************************************************************************/

UINT32 AttrBrushType::GetLocalTranspType()
{
	return m_LocalTranspType;
}

/********************************************************************************************

>	BrushDefinition* AttrBrushType::GetBrushDefinition() 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     -
	Returns:	the brush definition used by this brush (can be NULL)
	Purpose:	interface to the BrushAttrValue
	SeeAlso:	-

********************************************************************************************/

BrushDefinition* AttrBrushType::GetBrushDefinition()
{
	return m_Value.GetBrushDefinition();
}


/********************************************************************************************

>	BrushHandle AttrBrushType::GetBrushHandle() 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     -
	Returns:	handle of the brush definition used by this brush (can be NULL)
	Purpose:	interface to the BrushAttrValue
	SeeAlso:	-

********************************************************************************************/

BrushHandle AttrBrushType::GetBrushHandle()
{
	return m_Value.GetBrushHandle();
}


/********************************************************************************************

>	virtual BOOL AttrBrushType::IsABrush() const

    Author:     Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/99
	Returns:	TRUE
	Purpose:	For identifying this as a brush attribute

********************************************************************************************/

BOOL AttrBrushType::IsABrush() const
{
	return TRUE;
}


/********************************************************************************************

>	MILLIPOINT AttrBrushType::GetDefaultLineWidth(BOOL IgnorePressure) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     -
	Returns:	the height of the largest object in this brush
	Purpose:	convenient interface to the attribute value
	SeeAlso:	-

********************************************************************************************/

MILLIPOINT AttrBrushType::GetDefaultLineWidth(BOOL IgnorePressure)
{
	return m_Value.GetDefaultLineWidth(IgnorePressure);
}

/********************************************************************************************

>	BOOL AttrBrushType::SetSpacing(MILLIPOINT NewSpacing) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     the spacing value to set
	Returns:	TRUE if successful, FALSE if not
	Purpose:	To set the spacing between brush objects
	SeeAlso:	-

********************************************************************************************/

BOOL AttrBrushType::SetSpacing(MILLIPOINT NewSpacing)
{
	PathProcessorBrush* pPathProc = m_Value.GetPathProcessor();
	if (pPathProc == NULL)
		return FALSE;


	return pPathProc->SetSpacing(NewSpacing);
}


/********************************************************************************************

>	MILLIPOINT AttrBrushType::GetScaling() 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     -
	Returns:	the current brush spacing value
	Purpose:	-
	SeeAlso:	-

********************************************************************************************/

MILLIPOINT AttrBrushType::GetSpacing()
{
	PathProcessorBrush* pPathProc = m_Value.GetPathProcessor();
	if (pPathProc == NULL)
		return -1;
	return (MILLIPOINT)pPathProc->GetSpacing();
}



/********************************************************************************************

>	BOOL AttrBrushType::SetScaling(double NewScaling) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     the scaling value to set
	Returns:	TRUE if successful, FALSE if not
	Purpose:	To set the brush scaling
	SeeAlso:	-

********************************************************************************************/

BOOL AttrBrushType::SetScaling(double NewScale)
{
	PathProcessorBrush* pPPB = m_Value.GetPathProcessor();
	if (pPPB == NULL)
		return FALSE;
	return pPPB->SetBrushScaling(NewScale);
}

/********************************************************************************************

>	BOOL AttrBrushType::SetPathOffsetType(PathOffset NewOffsetType) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs		Offset - the offset type to set
	Returns:	- 
	Purpose:	to set the offset type for this brush
********************************************************************************************/

BOOL AttrBrushType::SetPathOffsetType(PathOffset NewOffsetType)
{
	PathProcessorBrush* pPathProc = m_Value.GetPathProcessor();
	if (pPathProc == NULL)
		return FALSE;
	pPathProc->SetPathOffsetType(NewOffsetType);
	return TRUE;
}

/********************************************************************************************

>	BOOL AttrBrushType::SetPathOffsetValue(MILLIPOINT Value) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs		Value - the offset value to set
	Returns:	- 
	Purpose:	to set the offset distance for this brush
********************************************************************************************/

BOOL AttrBrushType::SetPathOffsetValue(MILLIPOINT NewValue)
{
	PathProcessorBrush* pPathProc = m_Value.GetPathProcessor();
	if (pPathProc == NULL)
		return FALSE;
	pPathProc->SetPathOffsetValue(NewValue);
	return TRUE;
}


/********************************************************************************************

>	BOOL AttrBrushType::SetTiling(BOOL Value) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     whether or not the brush tiles
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	When a brush tiles it means that the attribute types:
				- 3 colour fill
				- 4 colour fill
				- bitmap fill
				are only rendered once per brush, rather than for every brush step
	SeeAlso:	-

********************************************************************************************/

BOOL AttrBrushType::SetTiled(BOOL Tiled)
{
	PathProcessorBrush* pPathProc = m_Value.GetPathProcessor();
	if (pPathProc == NULL)
		return FALSE;
	pPathProc->SetTiling(Tiled);
	return TRUE;
}


/********************************************************************************************

>	BOOL AttrBrushType::SetRotateAngle(double Angle) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     the angle of rotation to set
	Returns:	TRUE if the angle is within the stated bounds
	Purpose:	as above
	SeeAlso:	-

********************************************************************************************/

BOOL AttrBrushType::SetRotateAngle(double NewAngle)
{
	PathProcessorBrush* pPathProc = m_Value.GetPathProcessor();
	if (pPathProc == NULL)
		return FALSE;
	return pPathProc->SetRotationAngle(NewAngle);
}


/********************************************************************************************

>	BOOL AttrBrushType::SetRotated(BOOL Value) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     whether or not the brush rotates
	Returns:	-
	Purpose:	as above
	SeeAlso:	-

********************************************************************************************/

BOOL AttrBrushType::SetRotated(BOOL Rotated)
{
	PathProcessorBrush* pPathProc = m_Value.GetPathProcessor();
	if (pPathProc == NULL)
		return FALSE;
	pPathProc->SetRotated(Rotated);
	return TRUE;
}


/********************************************************************************************

>	PathProcessorBrush* AttrBrushType::GetPathProcessor() 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     -
	Returns:	the path processor belonging to the attribute value
	Purpose:	as above
	SeeAlso:	-

********************************************************************************************/

PathProcessorBrush* AttrBrushType::GetPathProcessor()
{
	return m_Value.GetPathProcessor();
}



/********************************************************************************************

>	void AttrBrushType::InitialiseBrushData() 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     -
	Returns:	-
	Purpose:	retrieves data from the path processor and puts it into our brushdata object
	SeeAlso:	-

********************************************************************************************/

void AttrBrushType::InitialiseBrushData()
{
	PathProcessorBrush* pPPB = GetPathProcessor();
	if (pPPB != NULL)
		pPPB->CopyBrushData(&m_BrushData);
}



/********************************************************************************************

>	BrushData AttrBrushType::GetBrushData() 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     -
	Returns:	our brush data object
	Purpose:	gets the latest data from our path processor, copies it into our BrushData object 
				which is then returned;
	SeeAlso:	-

********************************************************************************************/

BrushData AttrBrushType::GetBrushData()
{
	InitialiseBrushData();
	return m_BrushData;
}



/********************************************************************************************

>	void AttrBrushType::SetBrushData(BrushData Data) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     Data object to set
	Returns:	-
	Purpose:	Not quite sure why you would use this, as there is no link from the brush data object
				to the path processor at the moment.  hmm
	SeeAlso:	-

********************************************************************************************/

void AttrBrushType::SetBrushData(BrushData Data)
{
	m_BrushData = Data;
}


/********************************************************************************************

>	void AttrBrushType::GetMaxScalingFromDefinition()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:		-
	Returns:	-
	Purpose:	Obtains the max scaling value from the brush definition and sets it to the
				path processor.  This is not needed upon brush creation however it is required
				if you are importing a brush.
	SeeAlso:	-

********************************************************************************************/

void AttrBrushType::GetMaxScalingFromDefinition()
{
	BrushDefinition* pBrushDef = GetBrushDefinition();
	if (pBrushDef != NULL)
	{
		double Max = pBrushDef->GetMaxScaling();
		PathProcessorBrush* pPPB = GetPathProcessor();
		if (pPPB != NULL)
			pPPB->SetMaxScaling(Max);
	}
}


/********************************************************************************************

>	BOOL AttrBrushType::CanUsePointsCache() 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:		-
	Returns:	TRUE if the we are allowed to use the points cache in the brushattrvalue, 
				FALSE if not.
	Purpose:	We are not allowed to use the points cache if this attribute is applied to multiple
				ink nodes as a part of a group.  The reason for this is that the paths of the ink
				nodes are passed in to the path processor one by one, this means that the points
				along the first path are cached, and because the cache exists these points are 
				re-used for all subsequent ink objects, meaning that only the first object gets
				drawn.
				Further conditions may be added to this at a later date.
	See also:   PathProcessorBrush::GetPointAtDistance			

********************************************************************************************/

BOOL AttrBrushType::CanUsePointsCache()
{
	Node* pParent = FindParent();
	if (pParent != NULL)
	{
		if  (pParent->IsCompound()
			|| pParent->IsABaseTextClass()
			|| pParent->IsAContour()
			)
		return FALSE;
	}

	return TRUE;
}

/********************************************************************************************

>	void AttrBrushType::GetCurrentLineWidth() 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     -
	Returns:	the width of the nearest linewidth attribute, or -1 if there isn't one (unlikely)
	Purpose:	as above
	SeeAlso:	-

********************************************************************************************/

MILLIPOINT AttrBrushType::GetAppliedLineWidth()
{
	MILLIPOINT ReturnVal = 0;
	//first get the parent
	NodeRenderableInk* pInk = (NodeRenderableInk*)FindParent();

	// if we're under a nodeshadow controller then we need to ask it for the right ink node
	if (pInk != NULL && pInk->IsCompoundClass())
	{
		NodeRenderableInk* pInkNode = ((NodeCompound*)pInk)->GetInkNodeFromController();
		if (pInkNode)
			pInk = pInkNode;
	}

	if (pInk != NULL)
	{
		// find the applied AttrLineWidth
		NodeAttribute* pAttr = NULL;
		BOOL Found = pInk->FindAppliedAttribute(CC_RUNTIME_CLASS(AttrLineWidth), &pAttr);
		if (Found && (pAttr != NULL))
		{
			LineWidthAttribute* pVal = (LineWidthAttribute*)((AttrLineWidth*)pAttr)->GetAttributeValue();
			if (pVal != NULL)
				ReturnVal = pVal->LineWidth;
		}

		// double check that we don't have a transparent stroke colour
		AttrStrokeColour* pStrokeCol = NULL;
		pInk->FindAppliedAttribute(CC_RUNTIME_CLASS(AttrStrokeColour), (NodeAttribute**)&pStrokeCol, FALSE);
		if (pStrokeCol)
		{
			if (pStrokeCol->Value.Colour.IsTransparent())
				ReturnVal = 0;
		}
	}

	return ReturnVal;
}

/********************************************************************************************

>	void AttrBrushType::SetValueToScaleTo(MILLIPOINT Value) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     linewidth to scale to
	Returns:	-
	Purpose:	as above
	SeeAlso:	-

********************************************************************************************/

void AttrBrushType::SetValueToScaleTo(MILLIPOINT Value)
{
	m_Value.ScaleToLineWidth(Value);
}


/********************************************************************************************

>	void AttrBrushType::SetUseLocalTransp(BOOL UseTransp) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     whether or not to use local transparency
	Returns:	-
	Purpose:	interface to the PPB, sets a member
	SeeAlso:	PathProcessorBrush::RenderAttributes

********************************************************************************************/

void AttrBrushType::SetUseLocalTransp(BOOL UseTransp)
{
	PathProcessorBrush* pPPB = m_Value.GetPathProcessor();
	if (pPPB)
		pPPB->SetUseLocalTransp(UseTransp);
}

/********************************************************************************************

>	void AttrBrushType::Transform(TransformBase& Transform) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     the transformation to perform
	Returns:	-
	Purpose:	Currently this simply flushes the cached points map.
	SeeAlso:	-

********************************************************************************************/

void AttrBrushType::Transform(TransformBase &Trans)
{
#ifdef NEWFASTBRUSHES
	if (m_Value.GetCachedBitmap ())
	{
		m_Value.GetCachedBitmap ()->Transform (Trans);
	}
//	else
//	{
#endif
	
	if (!Trans.bSolidDrag)
	{
		// CGS:  my new TransformCache function is three
		// times faster than doing it this way!
		m_Value.TransformCache(Trans);
		m_Value.TransformTimeStampList(Trans);
		m_Value.TransformCachedRect(Trans);
	}
	else
	{
		// Phil, 10/03/2004
		// It may be faster but it doesn't do the job properly during solid dragging.
		// The following version works better for solid dragging and ought to be
		// reasonably fast because it still uses the cached bounding rect
		FlushCache();
		m_Value.TransformCachedRect(Trans);
	}

//#ifdef NEWFASTBRUSHES
//}
//#endif

}

/********************************************************************************************

>	void AttrBrushType::FlushCache() 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     -
	Returns:	-
	Purpose:	interface to the BrushAttrValue, tells it to flush the cache of its path processor
	SeeAlso:	-

********************************************************************************************/

void AttrBrushType::FlushCache()
{
	m_Value.FlushCache();	
}


/********************************************************************************************

>	BOOL AttrBrushType::OnReplaceAttribute(AttrBrushType* pOther)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     pOther - the attribute we are about to replace
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	If we are applying this attribute to a node that already has a brush attribute then
				we wish to retain certain information,so copy it over before we do the replacing.
	SeeAlso:	-

********************************************************************************************/

BOOL AttrBrushType::OnReplaceAttribute(AttrBrushType* pOther)
{
	ERROR2IF(pOther == NULL, FALSE, "Null input pointer to AttrBrushType::OnReplaceAttribute");

	PathProcessorBrush* pOldPPB = pOther->GetPathProcessor();
	PathProcessorBrush* pNewPPB = GetPathProcessor();
	if (pOldPPB != NULL && pNewPPB != NULL)
	{
		pNewPPB->SetUseLocalFillColour(pOldPPB->GetUseLocalFillColour());
		pNewPPB->SetUseNamedColours(pOldPPB->GetUseNamedColours());
	}

	// if we have a pressure cache then we want to copy that, also use local colour flags
	CDistanceSampler* pPressure = pOther->GetPressureCache();
	if (pPressure)
	{
		CDistanceSampler* pCopyPressure = pPressure->MakeCopy();
		if (pCopyPressure)
			SetPressureCache(pCopyPressure);
	}

	return TRUE;
}

/********************************************************************************************

>	void AttrBrushType::NodePathUpdated(Path* pPAth) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     InkPath of the nodepath
	Returns:	-
	Purpose:	interface to the BrushAttrValue, called when a nodepath is updated, either flushes
				the cache or recalculates the timestamping points distances
	SeeAlso:	-

********************************************************************************************/

void AttrBrushType::NodePathUpdated(Path* pPath)
{
	// so far as coordinates go, if we are not timestamping then we just flush our cache
	if (!IsTimeStamping())
		m_Value.FlushCache();
	else
		m_Value.RecalculateTimeStampList(pPath);

	
	// clear our cached bounding rect
	m_Value.SetCachedRect(DocRect(0,0,0,0));
}	


/********************************************************************************************

>	BOOL AttrBrushType::IsParentSelected(); 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     -
	Returns:	TRUE if any ancestor of the brush attribute is selected, FALSE otherwise
	Purpose:	To find out if this brush belongs to an ink node that is selected. We simpy
				scan up the tree until we find a selected node or hit the layer.
	SeeAlso:	-

********************************************************************************************/

BOOL AttrBrushType::IsParentSelected()
{
	BOOL Retval = FALSE;
	Node* pParent = FindParent();
	while (pParent != NULL && !pParent->IsLayer())
	{
		if (pParent->IsSelected())
		{
			Retval = TRUE;
			break;
		}
		pParent = pParent->FindParent();
	}
	return Retval;
}

/********************************************************************************************

>	void AttrBrushType::SetCache(PointsMap* pMap) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     -
	Returns:	-
	Purpose:	interface to the BrushAttrValue, sets the cache
	SeeAlso:	-

********************************************************************************************/

void AttrBrushType::SetCache(PointsMap* pMap)
{
	m_Value.SetCache(pMap);
}

/********************************************************************************************

>	PointsMap* AttrBrushType::GetCache() 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     -
	Returns:	pointer to the points map of the brushattrvalue
	Purpose:	interface to the BrushAttrValue, gets the cache
	SeeAlso:	-

********************************************************************************************/

PointsMap* AttrBrushType::GetCache()
{
	return m_Value.GetCache();
}


/********************************************************************************************

>	PressureArray* AttrBrushType::GetPressureCache() 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     -
	Returns:	pointer to the points map of the brushattrvalue
	Purpose:	interface to the BrushAttrValue, gets the pressure cache
	SeeAlso:	-

********************************************************************************************/

BOOL AttrBrushType::ContainsPressureCache()
{
	return (m_Value.GetPressureCache() != NULL);
}

/********************************************************************************************

>	PressureArray* AttrBrushType::GetPressureCache() 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     -
	Returns:	pointer to the points map of the brushattrvalue
	Purpose:	interface to the BrushAttrValue, gets the pressure cache
	SeeAlso:	-

********************************************************************************************/

CDistanceSampler* AttrBrushType::GetPressureCache()
{
	return m_Value.GetPressureCache();
}


/********************************************************************************************

>	void AttrBrushType::SetPressureCache(PressureArray* pArray) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     array holding pressure values
	Returns:	-
	Purpose:	interface to the BrushAttrValue, sets the cache
	SeeAlso:	-

********************************************************************************************/

void AttrBrushType::SetPressureCache(CDistanceSampler* pCache)
{
	m_Value.SetPresssureCache(pCache);
}

/********************************************************************************************

>	BOOL AttrBrushType::ReversePressureCache()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/5/2000
	Inputs:     -
	Outputs:	-
	Returns:	TRUE if all went well, otherwise false
	Purpose:	Asks the attribute value to reverse its pressure list
	See Also:   BrushAttrValue::ReversePressureList
********************************************************************************************/

BOOL AttrBrushType::ReversePressureCache()
{
	CDistanceSampler* pCache = GetPressureCache();
	return pCache->ReverseData();
}

/********************************************************************************************

>	TimeStampList* AttrBrushType::GetTimeStampList() 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     -
	Returns:	pointer to the timestamp points list of the brushattrvalue
	Purpose:	interface to the BrushAttrValue, gets the cache
	SeeAlso:	-

********************************************************************************************/

TimeStampList* AttrBrushType::GetTimeStampList()
{
	return m_Value.GetTimeStampList();
}

/********************************************************************************************

>	BOOL AttrBrushType::IsTimeStamping() 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     -
	Returns:	TRUE if we are timestamping, or FALSE if not
	Purpose:	interface to the BrushAttrValue
	SeeAlso:	-

********************************************************************************************/

BOOL AttrBrushType::IsTimeStamping()
{
	return m_Value.IsTimeStamping();
}


/********************************************************************************************

>	void AttrBrushType::ClearCachedRect() 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     -
	Returns:	-
	Purpose:	interface to the BrushAttrValue, wipes the cached bounding rect
	SeeAlso:	-

********************************************************************************************/

void AttrBrushType::ClearCachedRect()
{
	m_Value.SetCachedRect(DocRect(0,0,0,0));
}


void AttrBrushType::SetFreeHandTool(FreeHandTool* pTool)
{
	m_pFreeTool = pTool;
}

/********************************************************************************************

> static void AttrBrushType::RotateBounds(double Angle, DocRect* pBounds)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/2/2000
	Inputs:		Bounds = the bounds to rotate
				Angle  = the angle in degrees by which to rotate it by
	Outputs:	Bounds is updated
	Returns:	TRUE, unless you try to pass in a dud rect
	Purpose:	The bounds are updated to contain the rotated version
	SeeAlso:	-

********************************************************************************************/

BOOL AttrBrushType::RotateBounds(double Angle, DocRect* pBounds)
{
	ERROR2IF(pBounds == NULL, FALSE, "DocRect is NULL");
	if (Angle != 0.0)
	{
		DocCoord BBCentre = pBounds->Centre();

		DocCoord Coords[4];
		Coords[0].x = pBounds->lo.x;	Coords[0].y = pBounds->lo.y;
		Coords[1].x = pBounds->hi.x;	Coords[1].y = pBounds->lo.y;
		Coords[2].x = pBounds->hi.x;	Coords[2].y = pBounds->hi.y;
		Coords[3].x = pBounds->lo.x;	Coords[3].y = pBounds->hi.y;

		Trans2DMatrix Rotate(BBCentre,Angle);
		Rotate.Transform(Coords,4);

		pBounds->lo.x = min(min(Coords[0].x,Coords[1].x), min(Coords[2].x,Coords[3].x));
		pBounds->hi.x = max(max(Coords[0].x,Coords[1].x), max(Coords[2].x,Coords[3].x));
		pBounds->lo.y = min(min(Coords[0].y,Coords[1].y), min(Coords[2].y,Coords[3].y));
		pBounds->hi.y = max(max(Coords[0].y,Coords[1].y), max(Coords[2].y,Coords[3].y));
	}
	return TRUE;
}

/********************************************************************************************

>	virtual BOOL AttrBrushType::WritePreChildrenWeb(BaseCamelotFilter *pFilter)
>	virtual BOOL AttrBrushType::WritePreChildrenNative(BaseCamelotFilter *pFilter)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/99

	Inputs:		pFilter - filter to write to

	Returns:	TRUE if the Node has written out a record to the filter

	Purpose:	Writes out a record that represents the node, to either Native or Web
				file format.

				This function is called before any of the AttrBrushType's children
				are written to the filter.

				If the AttrBrushType writes out a record successfully to the file,
				it will return TRUE.

				If the AttrBrushType chooses not to write itself to the filter
				(e.g. because it is not appropriate	for this filter), then this
				function will return FALSE.

	Notes:		Simple Brushs (not using fancy bitmap/vector brushes) just
				write out a 4-byte (UINT32) record containing the stroke type.
				Defined stroke types at this time are:
					0 - Simple variable width stroke

	SeeAlso:	Node::WritePreChildrenNative; Node::WritePreChildrenWeb;
				StrokeAttrRecordHandler::HandleRecord

********************************************************************************************/

BOOL AttrBrushType::WritePreChildrenWeb(BaseCamelotFilter *pFilter)
{
	return(WritePreChildrenNative(pFilter));
}

BOOL AttrBrushType::WritePreChildrenNative(BaseCamelotFilter *pFilter)
{
	//return TRUE;

	ERROR2IF(pFilter == NULL, FALSE, "Illegal NULL param");
	
	BOOL ok = TRUE;
	PathProcessorBrush *pProcessor = m_Value.GetPathProcessor();
	
	if (pProcessor != NULL)
	{
		// first try and get the definition stored in the brush component
		BrushHandle Handle = pProcessor->GetBrushDefinitionHandle();
		
		// also check that the definition exists, as we do not want to save out deactivated brushes
		BrushDefinition* pDef = BrushComponent::FindBrushDefinition(Handle);

		if (Handle != BrushHandle_NoBrush && pDef != NULL)
		{
			// now get the brush component from the document (bit longwinded this but it
			// ensures that each brush only gets written once)
			Document* pDoc = Document::GetCurrent();
			ERROR2IF(pDoc == NULL, FALSE, "Er, wheres the document gone?");
			BrushComponent* pBrushComp = (BrushComponent*)pDoc->GetDocComponent(CC_RUNTIME_CLASS(BrushComponent));
			ERROR2IF(pBrushComp == NULL, FALSE, "Couldn't find brush component");
			
			// finally lets write the definition
			ok = pBrushComp->ExportLine(pFilter, Handle);
				
			if (ok) ok = pProcessor->WriteNative(pFilter);

			// now write our timestamping and pressure data
			if (IsTimeStamping())
				if (ok) ok = m_Value.WriteTimeStampList(pFilter);

			if (ContainsPressureCache())
				if (ok) ok = m_Value.WritePressureData(pFilter);

		}
	}
	else
	{
		//ERROR3("Trying to write empty attribute");
	//	return FALSE;
	}
	 
	return ok;
}






/*------------------------------------------------------------------------------------------
-----------------BRUSH ATTRIBUTE RECORD HANDLER --------------------------------------------
--------------------------------------------------------------------------------------------*/


// initialise our static
AttrBrushType* BrushAttrRecordHandler::m_pLastAttribute = NULL;


/********************************************************************************************

>	virtual UINT32 *BrushAttrRecordHandler::GetTagList()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/1/97

	Returns:	A list of tag values, terminated by CXFRH_TAG_LIST_END

	Purpose:	Provides the record handler system with a list of records handled by this
				handler - all StrokeType attributes:
				
	SeeAlso:	BrushTypeAttrRecordHandler::HandleRecord

********************************************************************************************/

UINT32 *BrushAttrRecordHandler::GetTagList()
{
	static UINT32 TagList[] =
	{
		TAG_BRUSHDEFINITION,
		TAG_BRUSHATTR,
		TAG_BRUSHDATA,
		TAG_MOREBRUSHDATA,
		TAG_MOREBRUSHATTR,
		TAG_EVENMOREBRUSHDATA,
		TAG_EVENMOREBRUSHATTR,
		TAG_TIMESTAMPBRUSHDATA,
		TAG_BRUSHPRESSUREINFO,
		TAG_BRUSHATTRPRESSUREINFO,
		TAG_BRUSHPRESSUREDATA,
		TAG_BRUSHPRESSURESAMPLEDATA,
		TAG_BRUSHATTRFILLFLAGS,
		TAG_BRUSHTRANSPINFO, 
		TAG_BRUSHATTRTRANSPINFO,
		CXFRH_TAG_LIST_END
	};

	return(TagList);
}



/********************************************************************************************

>	virtual BOOL BrushAttrRecordHandler::HandleRecord(CXaraFileRecord *pCXaraFileRecord)

 	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/12/99

	Inputs:		pCXaraFileRecord - The record to handle - may not be NULL
	Returns:	TRUE if handled successfuly

	Purpose:	Handles loading of the given AttrBrushType attribute record



********************************************************************************************/

BOOL BrushAttrRecordHandler::HandleRecord(CXaraFileRecord *pCXaraFileRecord)
{
	ERROR3IF(pCXaraFileRecord == NULL, "pCXaraFileRecord is NULL");

//	NodeAttribute* pNewNode = NULL;
	BOOL RetVal = TRUE;
	switch (pCXaraFileRecord->GetTag())
	{
		case TAG_BRUSHDEFINITION:
		{
			// get the brush component from the document
			Document* pDoc = Document::GetCurrent();
			ERROR2IF(pDoc == NULL, FALSE, "Wheres the document?");
			BrushComponent* pBrushComp = (BrushComponent*)pDoc->GetDocComponent(CC_RUNTIME_CLASS(BrushComponent));
			ERROR2IF(pBrushComp == NULL, FALSE, "No brush component");
			UINT32 ImportHandle = 0;		
			BOOL ok = pCXaraFileRecord->ReadUINT32(&ImportHandle);
			
			if (ok) 
				RetVal =  pBrushComp->StartImportBrush(this, pCXaraFileRecord, ImportHandle);
		}
		break;
		case TAG_BRUSHDATA:
			RetVal =  HandleBrushDataRecord(pCXaraFileRecord);
			break;
		case TAG_BRUSHATTR:
			RetVal = HandleBrushAttributeRecord(pCXaraFileRecord);
			break;
		case TAG_MOREBRUSHDATA:
			RetVal =  HandleMoreBrushDataRecord(pCXaraFileRecord);
			break;
		case TAG_MOREBRUSHATTR:
			RetVal =  HandleMoreBrushAttrRecord(pCXaraFileRecord);
			break;
		case TAG_EVENMOREBRUSHDATA:
			RetVal = HandleEvenMoreBrushData(pCXaraFileRecord);
			break;
		case TAG_EVENMOREBRUSHATTR:
			RetVal = HandleEvenMoreBrushAttr(pCXaraFileRecord);
			break;
		case TAG_TIMESTAMPBRUSHDATA:
			RetVal = HandleTimeStampData(pCXaraFileRecord);
			break;
		case TAG_BRUSHPRESSUREINFO:
			RetVal = HandlePressureInfo(pCXaraFileRecord);
			break;
		case TAG_BRUSHATTRPRESSUREINFO:
			RetVal = HandleAttrPressureInfo(pCXaraFileRecord);
			break;
		case TAG_BRUSHPRESSUREDATA:
			RetVal = HandlePressureData(pCXaraFileRecord);
			break;
		case TAG_BRUSHPRESSURESAMPLEDATA:
			RetVal = HandlePressureSampleData(pCXaraFileRecord);
			break;
		case TAG_BRUSHATTRFILLFLAGS:
			RetVal = HandleAttrFillFlags(pCXaraFileRecord);
			break;
		case TAG_BRUSHTRANSPINFO:
			RetVal = HandleTranspInfo(pCXaraFileRecord);
			break;
		case TAG_BRUSHATTRTRANSPINFO:
			RetVal = HandleAttrTranspInfo(pCXaraFileRecord);
			break;
		default:
			ERROR3_PF(("I don't handle records with the tag (%d)\n", pCXaraFileRecord->GetTag()));
			break;
	}

	return(RetVal);
}


/********************************************************************************************

>	 BOOL BrushAttrRecordHandler::HandleBrushAttributeRecord(CXaraFileRecord* pRecord)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/12/99

	Inputs:		pRecord - the file record holding the data
	Returns:	TRUE if brush attribute was successfully read and created, otherwise FALSE
	Purpose:	Reads in data which it uses to create a new AttrBrushType node

********************************************************************************************/

BOOL BrushAttrRecordHandler::HandleBrushAttributeRecord(CXaraFileRecord* pRecord)
{
	ERROR2IF(pRecord == NULL, FALSE, "Record is NULL");
	
	// first read in all the data
	UINT32 Handle = 0x01000000;
	INT32 TempOffsetType = 0;
	INT32 TempOffsetValue = 0;
	INT32 TempSpacing = 10000;
	BYTE Flags;
	double TempRotateAngle = 0.0;
	double TempScaling = 1.0;
	// Import the data from the record
	if (!pRecord->ReadUINT32(&Handle) || !pRecord->ReadINT32(&TempSpacing) ||
		!pRecord->ReadBYTE(&Flags) || !pRecord->ReadDOUBLE(&TempRotateAngle) 
		|| !pRecord->ReadINT32(&TempOffsetType) || !pRecord->ReadINT32(&TempOffsetValue)
		|| !pRecord->ReadDOUBLE(&TempScaling))
	{
		return(FALSE);
	}

	// allocate tge new node
	NodeAttribute* pNewNode = new AttrBrushType;
	ERROR2IF(pNewNode == NULL, FALSE, "Unable to allocate node attribute");
				
	// get the brush component from the document
	Document* pDoc = Document::GetCurrent();
	ERROR2IF(pDoc == NULL, FALSE, "Wheres the document?");
	BrushComponent* pBrushComp = (BrushComponent*)pDoc->GetDocComponent(CC_RUNTIME_CLASS(BrushComponent));
	ERROR2IF(pBrushComp == NULL, FALSE, "No brush component");

	// find the brush definition that this brush uses			
	BrushHandle NewHandle = pBrushComp->FindImportedBrush(Handle);	

	// allocate a new path processor
	PathProcessorBrush* pNewPathProc = new PathProcessorBrush;
	ERROR2IF(pNewPathProc == NULL, FALSE, "Unable to allocate path processor");
	// tell it which brush definition to use
	pNewPathProc->SetBrushDefinition(NewHandle);

	// transfer all the data
	pNewPathProc->SetSpacing(TempSpacing);
	PathOffset Offset = (PathOffset)TempOffsetType;
	pNewPathProc->SetPathOffsetType(Offset);
	pNewPathProc->SetPathOffsetValue(TempOffsetValue);
	pNewPathProc->SetRotationAngle(TempRotateAngle);
	pNewPathProc->SetBrushScaling(TempScaling);

	BOOL bTile = Flags & TAG_BRUSHTILE_FLAG;
	BOOL bRotate = Flags & TAG_BRUSHROTATE_FLAG;
	pNewPathProc->SetRotated(bRotate);
	pNewPathProc->SetTiling(bTile);

	// now allocate the processor to the AttributeValue
	BrushAttrValue* pBrushAttrValue = (BrushAttrValue*)pNewNode->GetAttributeValue();
	ERROR2IF(pBrushAttrValue == NULL, FALSE, "Brush attribute value is NULL");
	pBrushAttrValue->SetPathProcessor(pNewPathProc);
	
	// tell the processor who's the boss
	pNewPathProc->SetParentAttribute((AttrBrushType*)pNewNode);

	InsertNode(pNewNode);


	// set the static member, as there may be more data to come
	m_pLastAttribute = (AttrBrushType*)pNewNode;

	m_pLastAttribute->GetMaxScalingFromDefinition();

	return TRUE;

}				


/********************************************************************************************

>	 BOOL BrushAttrRecordHandler::HandleBrushAttributeRecord(CXaraFileRecord* pRecord)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/12/99

	Inputs:		pRecord - the file record holding the data
	Returns:	TRUE if all went well, otherwise FALSE
	Purpose:	Reads in data and assigns it to the previously created BrushDefinition.

********************************************************************************************/

BOOL BrushAttrRecordHandler::HandleBrushDataRecord(CXaraFileRecord* pRecord)
{
	ERROR2IF(pRecord == NULL, FALSE, "Record is NULL");
	
	// first read in all the data
	UINT32 Handle = 0x01000000;
	INT32 TempOffsetType = 0;
	INT32 TempOffsetValue = 0;
	INT32 TempSpacing = 10000;
	BYTE Flags;
	String_32 TempName;
	double TempScaling = 1.0;
	double TempRotateAngle = 0.0;
	// Import the data from the record
	if (!pRecord->ReadUINT32(&Handle) || !pRecord->ReadINT32(&TempSpacing) ||
		!pRecord->ReadBYTE(&Flags) || !pRecord->ReadDOUBLE(&TempRotateAngle) 
		|| !pRecord->ReadINT32(&TempOffsetType) || !pRecord->ReadINT32(&TempOffsetValue) 
		|| !pRecord->ReadUnicode(&TempName) || !pRecord->ReadDOUBLE(&TempScaling))
	{
		return(FALSE);
	}
	
	// convert the variables that were not stored as their original type
	PathOffset Offset = (PathOffset)TempOffsetType;
	BOOL bTile = Flags & TAG_BRUSHTILE_FLAG;
	BOOL bRotate = Flags & TAG_BRUSHROTATE_FLAG;

	// If there is a name to be imported, read it as well
	//if (!pRecord->ReadUnicode(&TempName))
	//	return(FALSE);

	// We remember the imported flags and suchlike, which we will use in EndImportLine.
//	TempName.Left((StringBase *)&m_ImportedName, 31);

	// Now we need to retrieve the previously generated brush definition and assign all
	// this lovely data to it. Note that for this to be successful it is vital that
	// BrushComponent::StartImportBrush be called prior to this in the import

	// get the brush component from the document
	Document* pDoc = Document::GetCurrent();
	ERROR2IF(pDoc == NULL, FALSE, "Wheres the document?");
	BrushComponent* pBrushComp = (BrushComponent*)pDoc->GetDocComponent(CC_RUNTIME_CLASS(BrushComponent));
	ERROR2IF(pBrushComp == NULL, FALSE, "No brush component");

	// find the brush definition that is currently being imported
	BrushDefinition* pBrushDef = pBrushComp->GetImportBrushDefinition();

	if (pBrushDef != NULL)
	{
		// assign the new data to it
		pBrushDef->SetSpacing(TempSpacing);
		pBrushDef->SetTiling(bTile);
		pBrushDef->SetRotated(bRotate);
		pBrushDef->SetRotationAngle(TempRotateAngle);
		pBrushDef->SetPathOffsetType(Offset);
		pBrushDef->SetPathOffsetValue(TempOffsetValue);
		pBrushDef->SetLineName(&TempName);
		pBrushDef->SetBrushScaling(1.0);
	}
//	else
//		return FALSE;

	
	return TRUE; 
}


/********************************************************************************************

>	 BOOL BrushAttrRecordHandler::HandleMoreBrushDataRecord(CXaraFileRecord* pRecord)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/12/99

	Inputs:		pRecord - the file record holding the data
	Returns:	TRUE if all went well, otherwise FALSE
	Purpose:	Reads in data and assigns it to the previously created BrushDefinition.

********************************************************************************************/

BOOL BrushAttrRecordHandler::HandleMoreBrushDataRecord(CXaraFileRecord* pRecord)
{
	ERROR2IF(pRecord == NULL, FALSE, "Record is NULL");	

	// get the variables to read into
	double SpacingIncrProp = 1.0;
	INT32 SpacingIncrConst  = 0;
	INT32 SpacingMaxRand    = 0;
	INT32 SpacingRandSeed   = 0;
	double ScalingIncr     = 1.0;
	INT32 ScalingMaxRand    = 0;
	INT32 ScalingRandSeed   = 0;
	INT32 Sequence  = SEQ_FORWARD;
	INT32 SequenceRandSeed  = 0;
	double OffsetIncrProp  = 1.0;
	INT32 OffsetIncrConst   = 0;
	INT32 OffsetTypeRandSeed = 0;
	INT32 OffsetValMaxRand   = 0;
	INT32 OffsetValRandSeed  = 0;

	if (!pRecord->ReadDOUBLE(&SpacingIncrProp) || !pRecord->ReadINT32(&SpacingIncrConst) ||
		!pRecord->ReadINT32(&SpacingMaxRand) || !pRecord->ReadINT32(&SpacingRandSeed) ||
		!pRecord->ReadDOUBLE(&ScalingIncr) || !pRecord->ReadINT32(&ScalingMaxRand) ||
		!pRecord->ReadINT32(&ScalingRandSeed) || !pRecord->ReadINT32(&Sequence) ||
		!pRecord->ReadINT32(&SequenceRandSeed) || !pRecord->ReadDOUBLE(&OffsetIncrProp) ||
		!pRecord->ReadINT32(&OffsetIncrConst) || !pRecord->ReadINT32(&OffsetTypeRandSeed) || 
		!pRecord->ReadINT32(&OffsetValMaxRand) || !pRecord->ReadINT32(&OffsetValRandSeed)
		)
		return FALSE;

	// get the brush component from the document
	Document* pDoc = Document::GetCurrent();
	ERROR2IF(pDoc == NULL, FALSE, "Wheres the document?");
	BrushComponent* pBrushComp = (BrushComponent*)pDoc->GetDocComponent(CC_RUNTIME_CLASS(BrushComponent));
	ERROR2IF(pBrushComp == NULL, FALSE, "No brush component");

	// find the brush definition that is currently being imported
	BrushDefinition* pBrushDef = pBrushComp->GetImportBrushDefinition();

	if (pBrushDef != NULL)
	{
		pBrushDef->SetSpacingIncrProp(SpacingIncrProp);
		pBrushDef->SetSpacingIncrConst((UINT32)SpacingIncrConst);
		pBrushDef->SetSpacingMaxRand((UINT32)SpacingMaxRand);
		pBrushDef->SetSpacingRandSeed((UINT32)SpacingRandSeed);
		pBrushDef->SetBrushScalingIncr(ScalingIncr);
		pBrushDef->SetScalingMaxRand((UINT32)ScalingMaxRand);
		pBrushDef->SetScalingRandSeed((UINT32)ScalingRandSeed);
		pBrushDef->SetSequenceType((SequenceType)Sequence);
		pBrushDef->SetSequenceSeed(SequenceRandSeed);
		pBrushDef->SetPathOffsetIncrProp(OffsetIncrProp);
		pBrushDef->SetPathOffsetIncrConst((UINT32)OffsetIncrConst);
		pBrushDef->SetOffsetTypeRandSeed((UINT32)OffsetTypeRandSeed);
		pBrushDef->SetOffsetValueMaxRand((UINT32)OffsetValMaxRand);
		pBrushDef->SetOffsetValueRandSeed((UINT32)OffsetValRandSeed);
	}

	return TRUE;

}


/********************************************************************************************

>	 BOOL BrushAttrRecordHandler::HandleMoreBrushAttrRecord(CXaraFileRecord* pRecord)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/12/99

	Inputs:		pRecord - the file record holding the data
	Returns:	TRUE if all went well, otherwise FALSE
	Purpose:	reads in the same data as previous fn. but assigns it to the path processor
				of 

********************************************************************************************/

BOOL BrushAttrRecordHandler::HandleMoreBrushAttrRecord(CXaraFileRecord* pRecord)
{
	ERROR2IF(pRecord == NULL, FALSE, "Record is NULL");	
	// we must have a node to import to 
	ERROR2IF(m_pLastAttribute == NULL, FALSE, "No last attribute to import to");

	// get some variables to read into
	double SpacingIncrProp = 1.0;
	INT32 SpacingIncrConst  = 0;
	INT32 SpacingMaxRand    = 0;
	INT32 SpacingRandSeed   = 0;
	double ScalingIncr     = 1.0;
	INT32 ScalingMaxRand    = 0;
	INT32 ScalingRandSeed   = 0;
	INT32 Sequence  = SEQ_FORWARD;
	INT32 SequenceRandSeed  = 0;
	double OffsetIncrProp  = 1.0;
	INT32 OffsetIncrConst   = 0;
	INT32 OffsetTypeRandSeed = 0;
	INT32 OffsetValMaxRand   = 0;
	INT32 OffsetValRandSeed  = 0;
	INT32 UseLocalFill		= -1;

	if (!pRecord->ReadDOUBLE(&SpacingIncrProp) || !pRecord->ReadINT32(&SpacingIncrConst) ||
		!pRecord->ReadINT32(&SpacingMaxRand) || !pRecord->ReadINT32(&SpacingRandSeed) ||
		!pRecord->ReadDOUBLE(&ScalingIncr) || !pRecord->ReadINT32(&ScalingMaxRand) ||
		!pRecord->ReadINT32(&ScalingRandSeed) || !pRecord->ReadINT32(&Sequence) ||
		!pRecord->ReadINT32(&SequenceRandSeed) || !pRecord->ReadDOUBLE(&OffsetIncrProp) || !pRecord->ReadINT32(&OffsetIncrConst) ||
		!pRecord->ReadINT32(&OffsetTypeRandSeed) || !pRecord->ReadINT32(&OffsetValMaxRand) ||
		!pRecord->ReadINT32(&OffsetValRandSeed) || !pRecord->ReadINT32(&UseLocalFill)
		)
		return FALSE;

	// now get the path processor belonging to the last attribute
	PathProcessorBrush* pPathProc = m_pLastAttribute->GetPathProcessor();
	
	// if there isn't a path processor thats ok, as it may be default
	if (pPathProc == NULL)
		return TRUE;
	else
	{
		pPathProc->SetSpacingIncrProp(SpacingIncrProp);
		pPathProc->SetSpacingIncrConst((UINT32)SpacingIncrConst);
		pPathProc->SetSpacingMaxRand((UINT32)SpacingMaxRand);
		pPathProc->SetSpacingRandSeed((UINT32)SpacingRandSeed);
		pPathProc->SetBrushScalingIncr(ScalingIncr);
		pPathProc->SetScalingMaxRand((UINT32)ScalingMaxRand);
		pPathProc->SetScalingRandSeed((UINT32)ScalingRandSeed);
		pPathProc->SetSequenceType((SequenceType)Sequence);
		pPathProc->SetSequenceSeed(SequenceRandSeed);
		pPathProc->SetPathOffsetIncrProp(OffsetIncrProp);
		pPathProc->SetPathOffsetIncrConst((UINT32)OffsetIncrConst);
		pPathProc->SetOffsetTypeRandSeed((UINT32)OffsetTypeRandSeed);
		pPathProc->SetOffsetValueMaxRand((UINT32)OffsetValMaxRand);
		pPathProc->SetOffsetValueRandSeed((UINT32)OffsetValRandSeed);
		BOOL UseFill = UseLocalFill > 0 ? TRUE : FALSE;
		pPathProc->SetUseLocalFillColour(UseFill);

		// gotta set these for the time being
		pPathProc->SetRotationMaxRand(0);
		pPathProc->SetRotationRandSeed(0);
	}

	return TRUE;
}



/********************************************************************************************

>	 BOOL BrushAttrRecordHandler::HandleEvenMoreBrushData(CXaraFileRecord* pRecord)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/12/99

	Inputs:		pRecord - the file record holding the data
	Returns:	TRUE if all went well, otherwise FALSE
	Purpose:	Reads in data and assigns it to the previously created BrushDefinition.

********************************************************************************************/

BOOL BrushAttrRecordHandler::HandleEvenMoreBrushData(CXaraFileRecord* pRecord)
{
	ERROR2IF(pRecord == NULL, FALSE, "Record is NULL");	

	INT32 RotationMaxRand = 0;
	INT32 RotationRandSeed = 0;

	if (!pRecord->ReadINT32(&RotationMaxRand) || !pRecord->ReadINT32(&RotationRandSeed)) 
		return FALSE;

	// get the brush component from the document
	Document* pDoc = Document::GetCurrent();
	ERROR2IF(pDoc == NULL, FALSE, "Wheres the document?");
	BrushComponent* pBrushComp = (BrushComponent*)pDoc->GetDocComponent(CC_RUNTIME_CLASS(BrushComponent));
	ERROR2IF(pBrushComp == NULL, FALSE, "No brush component");

	// find the brush definition that is currently being imported
	BrushDefinition* pBrushDef = pBrushComp->GetImportBrushDefinition();

	if (pBrushDef != NULL)
	{
		pBrushDef->SetRotationMaxRand((UINT32)RotationMaxRand);
		pBrushDef->SetRotationRandSeed((UINT32)RotationRandSeed);
	}

	// tell the component we've finished importing
	return TRUE; 
}


/********************************************************************************************

>	 BOOL BrushAttrRecordHandler::HandleEvenMoreBrushAttr(CXaraFileRecord* pRecord)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/12/99

	Inputs:		pRecord - the file record holding the data
	Returns:	TRUE if all went well, otherwise FALSE
	Purpose:	Reads in data and assigns it to the previously created node

********************************************************************************************/

BOOL BrushAttrRecordHandler::HandleEvenMoreBrushAttr(CXaraFileRecord* pRecord)
{
	ERROR2IF(pRecord == NULL, FALSE, "Record is NULL");	
	
	// we must have a node to import to 
	ERROR2IF(m_pLastAttribute == NULL, FALSE, "No last attribute to import to");
	
	INT32 RotationMaxRand = 0;
	INT32 RotationRandSeed = 0;
	BYTE ScaleFlag        = 0;
	if (!pRecord->ReadINT32(&RotationMaxRand) || !pRecord->ReadINT32(&RotationRandSeed)
		|| !pRecord->ReadBYTE(&ScaleFlag)) 
		return FALSE;


	// now get the path processor belonging to the last attribute
	PathProcessorBrush* pPathProc = m_pLastAttribute->GetPathProcessor();
	
	// if there isn't a path processor thats ok, as it may be default
	if (pPathProc == NULL)
		return TRUE;

	pPathProc->SetRotationMaxRand((UINT32)RotationMaxRand);
	pPathProc->SetRotationRandSeed((UINT32)RotationRandSeed);

	BOOL ScaleToLineWidth = ScaleFlag & TAG_SCALETOWIDTH_FLAG;
	pPathProc->SetScaleToLineWidth(ScaleToLineWidth);


	// set the random hue & sat to zero until we do a tag
	/*pPathProc->SetHueMaxRand(0);
	pPathProc->SetHueRandSeed(0);
	pPathProc->SetSatMaxRand(0);
	pPathProc->SetSatRandSeed(0);
	pPathProc->SetHueIncrement(1.0);
	pPathProc->SetSatIncrement(1.0);
	*/
	return TRUE;
}


/********************************************************************************************

>	BOOL BrushAttrRecordHandler::HandlePressureInfo(CXaraFileRecord* pRecord)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/12/99

	Inputs:		pRecord - the file record holding the data
	Returns:	TRUE if all went well, otherwise FALSE
	Purpose:	Reads in data and assigns it to the previously created BrushDefinition.

********************************************************************************************/

BOOL BrushAttrRecordHandler::HandlePressureInfo(CXaraFileRecord* pRecord)
{
	ERROR2IF(pRecord == NULL, FALSE, "Record is NULL");	
	
	INT32 ScalingPressure  = 0;
	INT32 SpacingPressure  = 0;
	INT32 OffsetPressure   = 0;
	INT32 RotationPressure = 0;
	INT32 HuePressure      = 0;
	INT32 SatPressure      = 0;
	INT32 TimePressure     = 0;

	if (!pRecord->ReadINT32(&ScalingPressure) || !pRecord->ReadINT32(&SpacingPressure)  ||
		!pRecord->ReadINT32(&OffsetPressure) || !pRecord->ReadINT32(&RotationPressure) ||
		!pRecord->ReadINT32(&HuePressure) || !pRecord->ReadINT32(&SatPressure) ||
		!pRecord->ReadINT32(&TimePressure))
	return FALSE;

	// get the brush component from the document
	Document* pDoc = Document::GetCurrent();
	ERROR2IF(pDoc == NULL, FALSE, "Wheres the document?");
	BrushComponent* pBrushComp = (BrushComponent*)pDoc->GetDocComponent(CC_RUNTIME_CLASS(BrushComponent));
	ERROR2IF(pBrushComp == NULL, FALSE, "No brush component");

	// find the brush definition that is currently being imported
	BrushDefinition* pBrushDef = pBrushComp->GetImportBrushDefinition();

	// currently we only make use of the scaling pressure value, others to follow when I have
	// time to implement them
	if (pBrushDef != NULL)
	{
		pBrushDef->SetScalingMaxPressure((UINT32)ScalingPressure);
		pBrushDef->SetRotationMaxPressure((UINT32)RotationPressure);
	}

	return TRUE;
}


/********************************************************************************************

>	BOOL BrushAttrRecordHandler::HandleAttrPressureInfo(CXaraFileRecord* pRecord)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/12/99

	Inputs:		pRecord - the file record holding the data
	Returns:	TRUE if all went well, otherwise FALSE
	Purpose:	Reads in data and assigns it to the previously created node

********************************************************************************************/

BOOL BrushAttrRecordHandler::HandleAttrPressureInfo(CXaraFileRecord* pRecord)
{
	ERROR2IF(pRecord == NULL, FALSE, "Record is NULL");	
	
	// we must have a node to import to 
	ERROR2IF(m_pLastAttribute == NULL, FALSE, "No last attribute to import to");
	
	INT32 ScalingPressure  = 0;
	INT32 SpacingPressure  = 0;
	INT32 OffsetPressure   = 0;
	INT32 RotationPressure = 0;
	INT32 HuePressure      = 0;
	INT32 SatPressure      = 0;
	INT32 TimePressure     = 0;

	if (!pRecord->ReadINT32(&ScalingPressure) || !pRecord->ReadINT32(&SpacingPressure) ||
		!pRecord->ReadINT32(&OffsetPressure) || !pRecord->ReadINT32(&RotationPressure) ||
		!pRecord->ReadINT32(&HuePressure) || !pRecord->ReadINT32(&SatPressure) ||
		!pRecord->ReadINT32(&TimePressure))
	return FALSE;

	// now get the path processor belonging to the last attribute
	PathProcessorBrush* pPathProc = m_pLastAttribute->GetPathProcessor();
	
	// if there isn't a path processor thats ok, as it may be default
	if (pPathProc == NULL)
		return TRUE;
	else
	{
		pPathProc->SetScalingMaxPressure((UINT32)ScalingPressure);
		pPathProc->SetRotationMaxPressure((UINT32)RotationPressure);
	}

	return TRUE;
}


/********************************************************************************************

>	 BOOL BrushAttrRecordHandler::HandleTimeStampData(CXaraFileRecord* pRecord)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/12/99

	Inputs:		pRecord - the file record holding the data
	Returns:	TRUE if all went well, otherwise FALSE
	Purpose:	Reads in timestamp data and assigns it to the last attribute we created

********************************************************************************************/

BOOL BrushAttrRecordHandler::HandlePressureData(CXaraFileRecord* pRecord)
{
	ERROR2IF(pRecord == NULL, FALSE, "Record is NULL");	
	
	// we must have a node to import to 
	ERROR2IF(m_pLastAttribute == NULL, FALSE, "No last attribute to import to");
	
	INT32 NumObjects = 0;
	
	if (!pRecord->ReadINT32(&NumObjects))
		return FALSE;

	// must be some mistake, but ours not to reason why
	if (NumObjects <= 0)
		return TRUE;

	/*
	// otherwise we must make ourselves a list
	PressureList* pNewList = new PressureList;
	
	PressureItem TheItem;
	INT32 Pressure;
	double Proportion;
	INT32 Distance;

	BOOL ok = TRUE;
	while ((NumObjects-- > 0) && ok)
	{
		if (ok) ok = pRecord->ReadINT32(&Pressure);
		if (ok) ok = pRecord->ReadDOUBLE(&Proportion);
		if (ok) ok = pRecord->ReadINT32(&Distance);
		if (ok) 
		{
			TheItem.m_Pressure = Pressure;
			TheItem.m_Proportion = Proportion;
			TheItem.m_Distance = Distance;
			pNewList->push_back(TheItem);
		}
	}

	if (pNewList && ok)
	{
		BrushAttrValue* pVal = (BrushAttrValue*)m_pLastAttribute->GetAttributeValue();
		if (pVal)
			pVal->SetPressureList(pNewList);
		else
		{
			pNewList->RemoveAll();
			delete pNewList;
		}
		pNewList = NULL;
	}
	*/
	return TRUE;
}


/********************************************************************************************

>	 BOOL BrushAttrRecordHandler::HandleTimeStampData(CXaraFileRecord* pRecord)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/12/99

	Inputs:		pRecord - the file record holding the data
	Returns:	TRUE if all went well, otherwise FALSE
	Purpose:	Reads in timestamp data and assigns it to the last attribute we created

********************************************************************************************/

BOOL BrushAttrRecordHandler::HandleTimeStampData(CXaraFileRecord* pRecord)
{
	ERROR2IF(pRecord == NULL, FALSE, "Record is NULL");	
	
	// we must have a node to import to 
	ERROR2IF(m_pLastAttribute == NULL, FALSE, "No last attribute to import to");

	// first find out how many objects we have
	INT32 NumObjects = 0;

	if (!pRecord->ReadINT32(&NumObjects))
		return FALSE;

	// must be some mistake, but ours not to reason why
	if (NumObjects <= 0)
		return TRUE;

	// otherwise we must make ourselves a list
	TimeStampList* pNewList = new TimeStampList;

	// we may fail to get the list, however we must still read in the data or the next
	// record will be really screwed

	DocCoord Point;
	double Tangent;
	double Proportion;
	MILLIPOINT Distance;

	TimeStampBrushPoint TSBP;
	BOOL ok = TRUE;
	while ((NumObjects-- > 0) && ok)
	{
		if (ok) ok = pRecord->ReadCoord(&Point);
		if (ok) ok = pRecord->ReadDOUBLE(&Tangent);
		if (ok) ok = pRecord->ReadDOUBLE(&Proportion);
		if (ok) ok = pRecord->ReadINT32(&Distance);
		TRACEUSER("Diccon", _T("Read Point %d, %d\n"), Point.x, Point.y);
		if (pNewList != NULL && ok)
		{
			TSBP.m_Distance = Distance;
			TSBP.m_Point = Point;
			// not quite sure why I have to make this atrocious hack.  It seems that 
			// somewhere between being written and being read the millipoint values
			// had 576000 added to them.
			TSBP.m_Point.x -= 576000;
			TSBP.m_Point.y -= 576000;
			TSBP.m_Proportion = Proportion;
			TSBP.m_Tangent = Tangent;
			TSBP.m_Pressure = MAXPRESSURE / 2; //??
			pNewList->push_back(TSBP);
		}
	}

	if (pNewList)
	{
		BrushAttrValue* pVal = (BrushAttrValue*)m_pLastAttribute->GetAttributeValue();
		if (pVal)
			pVal->SetTimeStampList(pNewList);
		else
		{
			pNewList->clear();
			delete pNewList;
		}
		pNewList = NULL;
	}
	return ok;
		
}


/********************************************************************************************

>	BOOL BrushAttrRecordHandler::HandleAttrFillFlags(CXaraFileRecord* pRecord)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/6/2000

	Inputs:		pRecord - the file record holding the data
	Returns:	TRUE if all went well, otherwise FALSE
	Purpose:	Reads in data, and assigns it to the attribute that we just created

********************************************************************************************/

BOOL BrushAttrRecordHandler::HandleAttrFillFlags(CXaraFileRecord* pRecord)
{
	ERROR2IF(pRecord == NULL, FALSE, "Record is NULL");	
	
	// we must have a node to import to 
	ERROR2IF(m_pLastAttribute == NULL, FALSE, "No last attribute to import to");	

	BYTE Flags = 0;
	
	if (!pRecord->ReadBYTE(&Flags))
		return FALSE;

	BOOL bUseLocalFill = Flags & BRUSHFILLFLAG_LOCALFILL;
	BOOL bUseLocalTransp = Flags & BRUSHFILLFLAG_LOCALTRANSP;
	BOOL bUseNamed = Flags & BRUSHFILLFLAG_NAMEDCOL;
	
	// we need to get the path processor
	PathProcessorBrush* pPPB = m_pLastAttribute->GetPathProcessor();
	if (pPPB == NULL)
		return FALSE;

	pPPB->SetUseLocalFillColour(bUseLocalFill);
	pPPB->SetUseLocalTransp(bUseLocalTransp);
	pPPB->SetUseNamedColours(bUseNamed);

	return TRUE;

}


/********************************************************************************************

>	 BOOL BrushAttrRecordHandler::HandleTranspInfo(CXaraFileRecord* pRecord)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/12/99

	Inputs:		pRecord - the file record holding the data
	Returns:	TRUE if all went well, otherwise FALSE
	Purpose:	Reads in data pertaining to transparency, and other assorted things

********************************************************************************************/

BOOL BrushAttrRecordHandler::HandleTranspInfo(CXaraFileRecord* pRecord)
{
	ERROR2IF(pRecord == NULL, FALSE, "Record is NULL");	
	
	INT32 Transp			  = 0;
	INT32 TranspPressure   = 0;
	double RotIncr		  = 0.0;
	double ScaleIncr	  = 0.0;
	INT32 HueMaxRand		  = 0;
	INT32 HueRandSeed	  = 0;
	INT32 SatMaxRand       = 0;
	INT32 SatRandSeed	  = 0;

	if (!pRecord->ReadINT32(&Transp) || !pRecord->ReadINT32(&TranspPressure) ||
		!pRecord->ReadDOUBLE(&RotIncr) || !pRecord->ReadDOUBLE(&ScaleIncr) ||
		!pRecord->ReadINT32(&HueMaxRand) || !pRecord->ReadINT32(&HueRandSeed) ||
		!pRecord->ReadINT32(&SatMaxRand) || !pRecord->ReadINT32(&SatRandSeed))
	return FALSE;

	// get the brush component from the document
	Document* pDoc = Document::GetCurrent();
	ERROR2IF(pDoc == NULL, FALSE, "Wheres the document?");
	BrushComponent* pBrushComp = (BrushComponent*)pDoc->GetDocComponent(CC_RUNTIME_CLASS(BrushComponent));
	ERROR2IF(pBrushComp == NULL, FALSE, "No brush component");

	// find the brush definition that is currently being imported
	BrushDefinition* pBrushDef = pBrushComp->GetImportBrushDefinition();

	// give the data to the definition
	if (pBrushDef != NULL)
	{
		pBrushDef->SetBrushTransparency(Transp);
		pBrushDef->SetTransparencyPressure(UINT32(TranspPressure));
		pBrushDef->SetRotationIncrConst(RotIncr);
		pBrushDef->SetBrushScalingIncrConst(ScaleIncr);
		pBrushDef->SetHueMaxRand(UINT32(HueMaxRand));
		pBrushDef->SetHueRandSeed(UINT32(HueRandSeed));
		pBrushDef->SetSatMaxRand(UINT32(SatMaxRand));
		pBrushDef->SetSatRandSeed(UINT32(SatRandSeed));
	}
	else
		ERROR3("Unable to find brush definition in BrushAttrRecordHandler::HandleTranspInfo");

	// this is the end of this brush import
	m_pLastAttribute = NULL;
	
	return pBrushComp->EndImportBrush(this);

}


/********************************************************************************************

>	 BOOL BrushAttrRecordHandler::HandleAttrTranspInfo(CXaraFileRecord* pRecord)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/12/99

	Inputs:		pRecord - the file record holding the data
	Returns:	TRUE if all went well, otherwise FALSE
	Purpose:	Reads in data pertaining to transparency, and other assorted things

********************************************************************************************/

BOOL BrushAttrRecordHandler::HandleAttrTranspInfo(CXaraFileRecord* pRecord)
{
	ERROR2IF(pRecord == NULL, FALSE, "Record is NULL");	
	
	// we must have a node to import to 
	ERROR2IF(m_pLastAttribute == NULL, FALSE, "No last attribute to import to");	

	// read in the data
	INT32 Transp			  = 0;
	INT32 TranspPressure   = 0;
	double RotIncr		  = 0.0;
	double ScaleIncr	  = 0.0;
	INT32 HueMaxRand		  = 0;
	INT32 HueRandSeed	  = 0;
	INT32 SatMaxRand       = 0;
	INT32 SatRandSeed	  = 0;

	if (!pRecord->ReadINT32(&Transp) || !pRecord->ReadINT32(&TranspPressure) ||
		!pRecord->ReadDOUBLE(&RotIncr) || !pRecord->ReadDOUBLE(&ScaleIncr) ||
		!pRecord->ReadINT32(&HueMaxRand) || !pRecord->ReadINT32(&HueRandSeed) ||
		!pRecord->ReadINT32(&SatMaxRand) || !pRecord->ReadINT32(&SatRandSeed))
	return FALSE;

	// we need to get the path processor
	PathProcessorBrush* pPPB = m_pLastAttribute->GetPathProcessor();

	if (pPPB != NULL)
	{
		pPPB->SetBrushTransparency(Transp);
		pPPB->SetTransparencyPressure(UINT32(TranspPressure));
		pPPB->SetRotationIncrConst(RotIncr);
		pPPB->SetBrushScalingIncrConst(ScaleIncr);
		pPPB->SetHueMaxRand(UINT32(HueMaxRand));
		pPPB->SetHueRandSeed(UINT32(HueRandSeed));
		pPPB->SetSatMaxRand(UINT32(SatMaxRand));
		pPPB->SetSatRandSeed(UINT32(SatRandSeed));
	}
	else
		ERROR3("Unable to find path processor in BrushAttrRecordHandler::HandleAttrTranspInfo");

	return TRUE;
}


/********************************************************************************************

>	 BOOL BrushAttrRecordHandler::HandleTimeStampData(CXaraFileRecord* pRecord)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/12/99

	Inputs:		pRecord - the file record holding the data
	Returns:	TRUE if all went well, otherwise FALSE
	Purpose:	Reads in timestamp data and assigns it to the last attribute we created

********************************************************************************************/

BOOL BrushAttrRecordHandler::HandlePressureSampleData(CXaraFileRecord* pRecord)
{
	ERROR2IF(pRecord == NULL, FALSE, "Record is NULL");	
	
	// we must have a node to import to 
	ERROR2IF(m_pLastAttribute == NULL, FALSE, "No last attribute to import to");

	// first find out how many objects we have
	INT32 NumObjects = 0;

	if (!pRecord->ReadINT32(&NumObjects))
		return FALSE;

	// must be some mistake, but ours not to reason why
	if (NumObjects <= 0)
		return TRUE;

	// make a new pressure sampler
	CDistanceSampler* pSampler = new CDistanceSampler;

	if (pSampler == NULL)
		return FALSE;

	if (!pSampler->InitialiseData(NumObjects))
	{
		delete pSampler;
		return FALSE;
	}
	pSampler->SetCollectionSampleRate(1);

	CSampleItem TheItem;
	DocCoord	TheCoord;
	MILLIPOINT	TheDistance = 0;
	INT32		ThePressure = 0;
	BOOL ok = TRUE;
	BOOL KeepAdding = TRUE;
	UINT32 Count = 0;
	while (ok && KeepAdding)
	{
		if (ok) ok = pRecord->ReadINT32(&ThePressure);
		if (ok) ok = pRecord->ReadCoord(&TheCoord);
		if (ok) ok = pRecord->ReadINT32(&TheDistance);

		TheItem.m_Pressure	= (UINT32)ThePressure;
		TheItem.m_Coord		= TheCoord;
		TheItem.m_Distance	= TheDistance;
		
		if (ok) KeepAdding = pSampler->SetNext(TheItem);
		Count++;
	}

	pSampler->SetDistanceSoFar (TheItem.m_Distance);

	if (ok)
		m_pLastAttribute->SetPressureCache(pSampler);
	else
		delete pSampler;

	return ok;
}



/********************************************************************************************

>	virtual BOOL BrushAttrRecordHandler::BeginSubtree(UINT32 Tag)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/12/99

	Inputs:		Tag = tag value of the tag this handler last handled
	Returns:	TRUE if this func wants exclusive handling of the tag
				FALSE otherwise
	Purpose:	Informs the record handler that a subtree is following a tag of type 'Tag'

				If you override this func and you do not wish other parts of the system to be informed of the
				subtree start, you should return TRUE

********************************************************************************************/

BOOL BrushAttrRecordHandler::BeginSubtree(UINT32 Tag)
{
	// We only want to know about following subtrees when doing stroke definition records.
	// If we are doing one, then we grab the subtree so nobody else can faff about with it
	if (Tag != TAG_BRUSHDEFINITION)
		return(FALSE);

	return(TRUE);
}



/********************************************************************************************

>	virtual BOOL BrushAttrRecordHandler::EndSubtree(UINT32 Tag)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/12/99

	Inputs:		Tag = tag value of the tag this handler last handled
	Returns:	TRUE if this func wants exclusive handling of the tag
				FALSE otherwise

	Purpose:	Informs the record handler that a subtree that followed a tag of type 'Tag' has ended

				If you override this func and you do not wish other parts of the system to be
				informed of the subtree end, you should return TRUE

********************************************************************************************/

BOOL BrushAttrRecordHandler::EndSubtree(UINT32 Tag)
{
	// We only want to know about following subtrees when doing custom print mark records.
	// If we are doing one, then we grab the subtree so nobody else can faff about with it
	if (Tag != TAG_BRUSHDEFINITION)
		return(FALSE);
	
	// get the brush component from the document
	Document* pDoc = Document::GetCurrent();
	ERROR2IF(pDoc == NULL, FALSE, "Wheres the document?");
	BrushComponent* pBrushComp = (BrushComponent*)pDoc->GetDocComponent(CC_RUNTIME_CLASS(BrushComponent));
	ERROR2IF(pBrushComp == NULL, FALSE, "No brush component");

	return TRUE; //pBrushComp->EndImportBrush(this);
	
}



/********************************************************************************************

>	virtual void BrushAttrRecordHandler::GetRecordDescriptionText(CXaraFileRecord* pRecord,StringBase* pStr)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/12/99

	Inputs:		pRecord = ptr to a record
				pStr = ptr to string to update

	Returns:	-
	Purpose:	Provides descriptions for the brush attribute records.

	Notes:		This function is only present in _DEBUG builds

********************************************************************************************/

#if XAR_TREE_DIALOG

void BrushAttrRecordHandler::GetRecordDescriptionText(CXaraFileRecord* pRecord, StringBase* pStr)
{
	if (pStr == NULL || pRecord == NULL)
		return;

	// Call base class first to output the tag and size
	CamelotRecordHandler::GetRecordDescriptionText(pRecord, pStr);

	UINT32 Tag = pRecord->GetTag();
	INT32 RecordNumber = pRecord->GetRecordNumber();


}



#endif
