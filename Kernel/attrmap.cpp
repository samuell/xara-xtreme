// $Id: attrmap.cpp 662 2006-03-14 21:31:49Z alex $
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
// Created by MarkN 11/1/96
// Implements the CCAttrMap class

#include "camtypes.h"
#include "attrmap.h"
#include "nodeattr.h"
#include "ink.h"
#include "lineattr.h"
#include "attrmgr.h"
#include "attrval.h"
#include "spread.h"
#include "nodedoc.h"
#include "glatable.h"

CC_IMPLEMENT_DYNAMIC(CCAttrMap, CCObject)

/********************************************************************************************

>	void CCAttrMap::DeleteAttributes()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/1/96
	Inputs:		
	Outputs:	-
	Returns:	-
	Purpose:	Deletes all the attributes within this map

				NOTE: THIS CAN BE DANGEROUS.  Only delete the attributes within this map if
				you know for sure that they are not being used.  This usually means only delete
				if the attrs in this map are not in the tree.

				The blend system generates attrs out of the tree, so they can be deleted.
				NodeRenderableInk::FindAppliedAttributes() finds a list of attrs in the tree
				and therefore should not be deleted via this call.

	SeeAlso:	CCAttrMap::Copy()

********************************************************************************************/

void CCAttrMap::DeleteAttributes()
{
	CCRuntimeClass	   *pType;
	void			   *pVal;

	// iterating all (key, value) pairs
	for( iterator Pos = GetStartPosition(); Pos != GetEndPosition(); )
	{
		// Get attr at position Pos
		GetNextAssoc( Pos, pType, pVal );

		if (pVal != NULL)
		{
			RemoveKey(pType);
			// Must cast to NodeAttribute before deleting, otherwise wrong destructor gets called
			delete (NodeAttribute*)pVal;
		}
	}
}

/********************************************************************************************

>	CCAttrMap* CCAttrMap::Copy()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/1/96
	Inputs:		
	Outputs:	-
	Returns:	Ptr to a new CCAttrMap, or NULL if it fails
	Purpose:	Copies all the attributes within this map into a new CCAttrMap.
				A ptr to the new map is returned, or NULL if it fails

				The new map contains a list of attrs that are not attached in the tree. If the
				attrs are not needed or used (e.g. not inserted into the tree) make sure you
				delete the attrs via DeleteAttributes() before deleting the CCAttrMap, otherwise
				memory leaks will occur.

	SeeAlso:	CCAttrMap::DeleteAttributes()

********************************************************************************************/

CCAttrMap* CCAttrMap::Copy()
{
	CCAttrMap		   *pNewAttrMap = new CCAttrMap( GetCount() );
	if( pNewAttrMap != NULL )
	{
		// iterating all (key, value) pairs
		for( iterator Pos = GetStartPosition(); Pos != GetEndPosition(); )
		{
			CCRuntimeClass *pType;
			void	   *pVal;
			GetNextAssoc( Pos, pType, pVal );

			// Get the attribute value of this attribute
			NodeAttribute* pAttr = (NodeAttribute*)pVal;

			// Copy the attribute
			NodeAttribute* pNewAttr = (NodeAttribute*) (pAttr->SimpleCopy());

			// Stick the new attr into the new attr map
			if (pNewAttr != NULL)
				pNewAttrMap->SetAt(pNewAttr->GetAttributeType(),pNewAttr);
		}
	}

	pNewAttrMap->attrMapCreator = attrMapCreator;

	return pNewAttrMap;
}

/********************************************************************************************

>	static CCAttrMap* CCAttrMap::MakeAppliedAttrMap(NodeRenderableInk* pInkNode,
													BOOL ExcludeIndirectlyAppliedGLAs = TRUE,
													BOOL bStrictEffectStatus = TRUE)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/1/96
	Inputs:		pInkNode = ptr to an ink node
				ExcludeIndirectlyAppliedGLAs - whether they should be excluded from this attrmap
	Outputs:	-
	Returns:	Ptr to a new CCAttrMap, or NULL if it fails
	Purpose:	This creates a new CCAttrMap and fills it with ptrs to applied attrs of pInkNode.

				pInkNode must be a node that is inserted in the tree, and is not hidden.

				It is the caller's responsibility to delete the returned CCAttrMap.
				Note: As the map contains ptrs to attrs in the tree.  Do not call DeleteAttributes()
				on the returned CCAttrMap

	SeeAlso:	CCAttrMap::Copy()

********************************************************************************************/

CCAttrMap* CCAttrMap::MakeAppliedAttrMap(NodeRenderableInk* pInkNode,
										 BOOL ExcludeIndirectlyAppliedGLAs,
										 BOOL bStrictEffectStatus)
{
	ERROR2IF(pInkNode == NULL,NULL,"pInkNode is NULL");
	// ERROR2IF(pInkNode->FindParent() == NULL,NULL,"pInkNode has no parent");

	CCAttrMap* pAttrMap = new CCAttrMap(30);
	if (pAttrMap != NULL)
	{
		if (!pInkNode->FindAppliedAttributes(pAttrMap,
											 5000,
											 NULL,
											 ExcludeIndirectlyAppliedGLAs,
											 bStrictEffectStatus))
		{
			delete pAttrMap;
			pAttrMap = NULL;
		}
		else
		{
			pAttrMap->attrMapCreator = (Node*) pInkNode;
		}
	}

	return pAttrMap;
}

/********************************************************************************************

>	void CCAttrMap::Transform(TransformBase& Trans)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/4/99
	Inputs:		Trans
	Outputs:	-
	Returns:	-
	Purpose:	Applies the given transform to all the attrs in this map
	SeeAlso:	-

********************************************************************************************/

void CCAttrMap::Transform(TransformBase& Trans)
{
	CCRuntimeClass	   *pType;
	void			   *pVal;
	for( iterator Pos = GetStartPosition(); Pos != GetEndPosition(); )
	{
		GetNextAssoc(Pos,pType,pVal);
		if (pVal != NULL)
		{
			NodeAttribute* pNodeAttr = (NodeAttribute *)pVal;
			pNodeAttr->Transform(Trans);
		}
	}
}

/********************************************************************************************

>	void CCAttrMap::TransformForBrush(TransformBase& Trans)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/4/99
	Inputs:		Trans
	Outputs:	-
	Returns:	-
	Purpose:	Applies the given transform to all the attrs in this map that need to be rendered
				at every step of a brush or a blend.  This allows us to tile as things such as bitmap
				fills remain in the same place always
	SeeAlso:	-

********************************************************************************************/

void CCAttrMap::TransformForBrush(TransformBase& Trans)
{
	CCRuntimeClass	   *pType;
	void			   *pVal;
	for( iterator Pos = GetStartPosition(); Pos != GetEndPosition(); )
	{
		GetNextAssoc(Pos,pType,pVal);
		if (pVal != NULL)
		{
			NodeAttribute* pNodeAttr = (NodeAttribute *)pVal;
			
			// check that we are not about to set line width to zero
			if( pNodeAttr->IsALineWidthAttr() && Trans.TransLines != FALSE )
			{
				double Test =  Trans.GetScalar().MakeDouble() * (double)((AttrLineWidth*)pNodeAttr)->Value.LineWidth;
		//		TRACEUSER( "Diccon", _T("Scale line width by %f\n"), Test);
				if (Test <= 1.0)
				{
				//	TRACEUSER( "Diccon", _T("Setting line width scaling OFF\n"));
					Trans.TransLines = FALSE;
				}
			}

			if (pNodeAttr->NeedsToRenderAtEachBrushStroke())
				pNodeAttr->Transform(Trans);
		}
	}
}

/********************************************************************************************

>	void CCAttrMap::TransformForBrush(TransformBase& Trans)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/4/99
	Inputs:		Trans
	Outputs:	-
	Returns:	-
	Purpose:	Applies the transform to all the attributes that do not have to render for each
				brush step.
	SeeAlso:	-

********************************************************************************************/

void CCAttrMap::TransformBrushFills(TransformBase& Trans)
{
	CCRuntimeClass	   *pType;
	void			   *pVal;
	for( iterator Pos = GetStartPosition(); Pos != GetEndPosition(); )
	{
		GetNextAssoc(Pos,pType,pVal);
		if (pVal != NULL)
		{
			NodeAttribute* pNodeAttr = (NodeAttribute *)pVal;
			
			// check that we are not about to set line width to zero
			if( pNodeAttr->IsALineWidthAttr() && Trans.TransLines != FALSE )
			{
				INT32 Test = labs( INT32(Trans.GetScalar().MakeDouble() * ((AttrLineWidth*)pNodeAttr)->Value.LineWidth) );		
				if (Test <= 10)
					Trans.TransLines = FALSE;
			}

			if (!pNodeAttr->NeedsToRenderAtEachBrushStroke())
				pNodeAttr->Transform(Trans);
		}
	}


}


/********************************************************************************************

>	BOOL CCAttrMap::WillScaleLineWidthToZero(TransformBase& Trans)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/4/99
	Inputs:		Trans - the transformation we wish to test
	Outputs:	-
	Returns:	TRUE if this transformation will scale the line width in this map to zero or not
	Purpose:	As above, this can be useful information as if we pass GDraw a line width of zero
				it reverts to zero quality.
	SeeAlso:	-

********************************************************************************************/

BOOL CCAttrMap::WillScaleLineWidthToZero(TransformBase& Trans)
{
	// first look up the line width
	AttrLineWidth* pLineWidth = NULL;
	Lookup( CC_RUNTIME_CLASS(AttrLineWidth), (void*&)pLineWidth );
	
	// if we don't have a line width then we're obviously not going to scale it to zero
	if (pLineWidth == NULL)
		return FALSE;

	// also look up the stroke colour, if it is fully transparent then the line
	// is invisible anyway
	AttrStrokeColour* pStrokeCol = NULL;
	Lookup( CC_RUNTIME_CLASS(AttrStrokeColour), (void*&)pStrokeCol );
	if (pStrokeCol && pStrokeCol->Value.Colour.IsTransparent())
		return FALSE;

	double ScaleFactor = Trans.GetScalar().MakeDouble();
	INT32 ScaleVal = (INT32)(ScaleFactor * (double)pLineWidth->Value.LineWidth);
	if (ScaleVal < 1)
		return TRUE;

	return FALSE;
}

/********************************************************************************************

>	void CCAttrMap::Render(RenderRegion * pRegion, BOOL RenderOffscreenAttributes = TRUE)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/4/99
	Inputs:		pRegion						render-region to render into.
				RenderOffscreenAttributes	whether or not to render offscreen attributes,
											eg feathers.
	Outputs:	-
	Returns:	-
	Purpose:	Renders the attribute map into the given render region

	Notes:		Karim 15/11/2000
				Modified so that I can render an attribute map *without*
				rendering any offscreen attributes contained therein.

	SeeAlso:	-

********************************************************************************************/
void CCAttrMap::Render(RenderRegion * pRegion, BOOL RenderOffscreenAttributes)
{
	// OK, we have found the full quota of attributes.  Now render them all.
	iterator			pos = GetStartPosition();
	while( pos != GetEndPosition() )
	{
		CCRuntimeClass *pKey;
		void		   *pVal;
		GetNextAssoc( pos, pKey, pVal );

		NodeAttribute* pAttr = (NodeAttribute*)pVal;

		if ( pAttr->CanBeAppliedToObject() &&
			 pAttr->RenderSubtree(pRegion)==SUBTREE_ROOTONLY )
		{
			// render all attributes, unless we've been specifically asked
			// not to render offscreen attributes.
			if (RenderOffscreenAttributes || !pAttr->IsAnOffscreenAttribute())
				pAttr->Render(pRegion);
		}
	}
}

/********************************************************************************************

>	void CCAttrMap::BuildListOfAttributes(List * pList)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/4/99
	Inputs:		The list to use
	Outputs:	-
	Returns:	-
	Purpose:	Builds a list of NodeListItems of all the attributes in this attribute map
	Notes:		Includes all attributes - even those which can't be applied to objects
				Use CanBeAppliedToObject to test
	SeeAlso:	-

********************************************************************************************/
void CCAttrMap::BuildListOfAttributes(List * pList)
{
	iterator			pos = GetStartPosition();

	NodeListItem	   *pItem = NULL;

	while( pos != GetEndPosition() )
	{
		CCRuntimeClass *pKey;
		void		   *pVal;
		GetNextAssoc( pos, pKey, pVal );

		pItem = new NodeListItem((NodeAttribute *)pVal);

		pList->AddTail(pItem);
	}
}

/********************************************************************************************

>	NodeAttribute* CCAttrMap::FindAttribute(CCRuntimeClass* Class)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/6/2000
	Inputs:		The attribute type that we are to find.
	Outputs:	The attribute that matches this attribute type
	Returns:	-
	Purpose:	Call to retrieve a ptr to the attribute of the supplied type
	Notes:		
	SeeAlso:	-

********************************************************************************************/

NodeAttribute* CCAttrMap::FindAttribute(NodeAttribute *pTypeAttr)//CCRuntimeClass* Class)
{
	void* ptrToAttr = NULL;

	if( Lookup( pTypeAttr->GetAttributeType(), ptrToAttr ) )
	{
		return (NodeAttribute *)ptrToAttr;
	}
	
	return (NULL);
}

/********************************************************************************************

>	NodeAttribute * CCAttrMap::ReplaceAttribute(NodeAttribute *pNewAttr)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/12/99
	Inputs:		The attribute to replace
	Outputs:	The old attribute - in case it needs deleting
	Returns:	-
	Purpose:	Directly replaces the attribute in this map with the given one
	Notes:		
	SeeAlso:	-

********************************************************************************************/

NodeAttribute * CCAttrMap::ReplaceAttribute(NodeAttribute *pNewAttr)
{
	void* pOldAttr = NULL;
	if( Lookup( pNewAttr->GetAttributeType(), pOldAttr ) )
	{
		// We need to Remove and Delete the existing Stroke Colour Attr
		RemoveKey( pNewAttr->GetAttributeType() );
	}
	
	// Add the new attr into the attr map
	SetAt( pNewAttr->GetAttributeType(), (void*)pNewAttr );	

	return (NodeAttribute *)pOldAttr;
}




/********************************************************************************************

>	void CCAttrMap::RemoveAttribute(CCRuntimeClass* pAttrClass)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	04/11/2005
	Inputs:		The attribute class to remove
	Outputs:	-
	Returns:	-
	Purpose:	Removes and Deletes the given attribute type
	Notes:		
	SeeAlso:	-

********************************************************************************************/

void CCAttrMap::RemoveAttribute(CCRuntimeClass* pAttrClass)
{
	NodeAttribute* pOldAttr = NULL;
	if( Lookup( pAttrClass, (void*&)pOldAttr ) )
	{
		if (pOldAttr)
			delete pOldAttr;

		// We need to Remove and Delete the existing Stroke Colour Attr
		RemoveKey( pAttrClass );
	}
}




/********************************************************************************************

>	static CCAttrMap * CCAttrMap::MakeAttrMapFromRenderRegion(RenderRegion * pRegion)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/2/2000
	Inputs:		The render region to get the attribute map from
	Outputs:	An attribute map (copied) from the render region
	Returns:	-
	Purpose:	Makes an attribute out of the render region's current attribute state
	Notes:		You MUST call DeleteAttributes afterwards to release memory - delete is
				not sufficient
	SeeAlso:	-

********************************************************************************************/
CCAttrMap * CCAttrMap::MakeAttrMapFromRenderRegion(RenderRegion * pRegion)
{
	CCAttrMap * pMap = new CCAttrMap;
	ENSURE(pMap,"No mem for attrmap");
	if(!pMap)
		return NULL;

	// let's get every attribute in the render region
	AttributeValue * pAttrVal = NULL;
	NodeAttribute * pNewAttr = NULL;

	for (UINT32 i = 0; i < ATTR_FIRST_FREE_ID; i++)
	{
		pAttrVal = pRegion->GetCurrentAttribute(i);

		// make a new node out of this attribute value
		pNewAttr = pAttrVal->MakeNode();
		
		// Karim 12/04/2000
		// AttributeValues _do_not_have_to_have_ a corresponding NodeAttribute,
		// so can we please *check* that MakeNode didn't just return NULL!
		if (pNewAttr != NULL)
		{
			if(!pNewAttr->IsLinkedToNodeGeometry())
				pMap->SetAt(pNewAttr->GetAttributeType(),pNewAttr);
			else
			{
				delete pNewAttr;
				pNewAttr = AttributeManager::GetDefaultAttribute((AttrIndex) i);
				if(pNewAttr)
				{
					ENSURE(pNewAttr->IsLinkedToNodeGeometry(),"Incorrect NodeAttribute returned by GetDefaultAttribute");
					pMap->SetAt(pNewAttr->GetAttributeType(), pNewAttr);
				}
				else
					return NULL;
			}
		}
	}

	pMap->attrMapCreator = NULL;

	return pMap;
}

void CCAttrMap::ApplyAttributesToNode(NodeRenderableInk * pInk)
{
	iterator			pos = GetStartPosition();

	while( pos != GetEndPosition() )
	{
		CCRuntimeClass *pKey;
		void		   *pVal;
		GetNextAssoc(pos, pKey, pVal);

		NodeAttribute * pAttr = (NodeAttribute *)pVal;

		// copy the attribute
		if( pAttr->CanBeAppliedToObject() )
		{
			NodeAttribute * pAttrCopy = NULL;
			pAttr->NodeCopy((Node **)(&pAttrCopy));
			
			pAttrCopy->AttachNode(pInk, LASTCHILD);

			// nb now that GLAs have an independent flag to indicate when
			// they are copied from the default, it is safe to fix linkages
			// this ensures that GLA defaults get copied when BlendRefs are
			// made from complex nodes, and that they are found when MakeAppliedAttributes
			// calls FindAppliedAttributes on the unattached subtree
			// TODO??
			// What about just copying compound node's Parent pointers to 
			// pseudo attach the tree, rather than copying applied attrs to unattached tree
			// Just need to watch when deleting that it doesn't do anything to the
			// parents pointers - which it shouldn't surely?
			pAttrCopy->LinkToGeometry(pInk);
		}
	}
}

// Required for all GLA's
// NB must only be called on attrmaps with copied attributes
// DO NOT CALL on maps with 'live' attributes - ie pointers to attrs in the tree
// (eg a mpa made by MakeAppliedAttrMap(pInk))
void CCAttrMap::PostBlendInit(Path* pPath, CCRuntimeClass* pCreatorClass)
{
	INT32 curr = 0;

	// NB this function is called on a Blended attr map
	// The Blend() function should return false if it doesn't want to be in the blended attr map
	// ie default GLAs shouldn't be in here!

	do
	{
		CCRuntimeClass *pType = (CCRuntimeClass *)GeometryLinkedAttributeClasses[curr];
		void		   *pVal;
				
		if( Lookup( pType, pVal ) )
		{
			if (pVal != NULL)
			{
				NodeAttribute * pAttr = (NodeAttribute *)pVal;

				// call the attributes routine to initialise itself
				if(!pAttr->PostDynCreateInit(this, pPath, pCreatorClass))
				{
					// make sure the node is not going to try and render itself
					TRACEUSER( "Ilan", wxT("Dynamically created attribute failed to post init itself. Removing attribute from map.\n") );
					// NB map must contain copied attrs
					RemoveKey( pType );
					// Must cast to NodeAttribute before deleting, otherwise wrong destructor gets called
					delete (NodeAttribute*)pVal;
				}
			}
		}

		curr++;
	}
	while(curr<NumGLAs);
}

// Required for all GLA's
void CCAttrMap::PostBlendDeinit()
{
	INT32 curr = 0;

	do
	{
		CCRuntimeClass *pType = (CCRuntimeClass *)GeometryLinkedAttributeClasses[curr];
		void		   *pVal;
				
		if( Lookup( pType, pVal ) )
		{
			if (pVal != NULL)
			{
				NodeAttribute * pAttr = (NodeAttribute *)pVal;

				pAttr->PostDynCreateDeInit();
			}
		}

		curr++;
	}
	while(curr<NumGLAs);
}


void CCAttrMap::AttachGeometryLinkedAttrs(Node* pContext)
{
	INT32 curr = 0;

	do
	{
		CCRuntimeClass *pType = (CCRuntimeClass *)GeometryLinkedAttributeClasses[curr];
		void		   *pVal;
				
		if (Lookup(pType,pVal))
		{
			if (pVal != NULL)
			{
				((NodeAttribute*)pVal)->LinkToGeometry(pContext);
			}
		}

		curr++;
	}
	while(curr<NumGLAs);
}

void CCAttrMap::RemoveGeometryLinkedAttrs()
{
	INT32 curr = 0;

	do
	{
		CCRuntimeClass *pType = (CCRuntimeClass *)GeometryLinkedAttributeClasses[curr];
		void		   *pVal;
				
		if( Lookup( pType, pVal ) )
		{
			if (pVal != NULL)
			{
				RemoveKey( pType );
				delete (NodeAttribute*)pVal;
			}
		}

		curr++;
	}
	while(curr<NumGLAs);
}



/********************************************************************************************

>	BOOL CCAttrMap::IsSeeThrough()

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/02/2001

	Returns:	TRUE if at least one of our attrs IsSeeThrough(),
				FALSE otherwise.

	Purpose:	Test each of the attributes in this map for see-through-ness.
				This is not the same as transparency - what it means is: if I draw something
				in white, then draw its outline as retrieved from PathBecomeA in black over
				the top, will I still be able to see white bits?
				IsSeeThrough() therefore depends partly on the implementation of PathBecomeA.

				Examples:
					Transparency is always see-through unless it is a zero transparency.
					Line width is not see through - PathBecomeA can cope with this attr.
					Fill colour is not see-through unless it is COLOUR_NONE.
					Brush attrs are normally see-through, as PathBecomeA ignores them.

	See Also:	NodeRenderableInk::IsSeeThrough()

********************************************************************************************/
BOOL CCAttrMap::IsSeeThrough()
{
	CCRuntimeClass	   *pKey;
	void			   *pVal;
	BOOL				bIsSeeThrough = FALSE;

	for ( iterator	pos = GetStartPosition();
					pos != GetEndPosition() && !bIsSeeThrough;	)
	{
		GetNextAssoc(pos, pKey, pVal);
		NodeAttribute* pAttr = (NodeAttribute*)pVal;

		bIsSeeThrough = pAttr->IsSeeThrough(FALSE);
	}

	return bIsSeeThrough;
}
