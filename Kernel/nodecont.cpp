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

/*
//*/

#include "camtypes.h"
#include "nodecont.h"
#include "saveeps.h"

#ifdef BUILDSHADOWS

// code headers
#include "docview.h"
#include "objchge.h"
//#include "osrndrgn.h"

// resource headers
//#include "simon.h"
//#include "mario.h"
#include "transop.h"

// Save/load
#include "cxfdefs.h"	// TAG_SHADOWCONTROLLER_SIZE
#include "cxftags.h"	// TAG_SHADOWCONTROLLER
#include "cxfrec.h"		// CXaraFileRecord
#include "rechshad.h"	// ShadowRecordHandler
//#include "shadres.h"
#include "opshadow.h"
#include "ops.h"
#include "txtattr.h"
#include "attrbev.h"
#include "lineattr.h"
#include "spread.h"
#include "attrappl.h"
#include "blobs.h"
#include "app.h"
#include "opgrad.h"
#include "opbevel.h"
#include "nbevcont.h"
#include "moldtool.h"
#include "attrmap.h"
#include "nodetext.h"
//#include "opcntr.h"
#include "blndtool.h"
#include "ncntrcnt.h"
#include "document.h"
#include "extender.h"
#include "ngcore.h"		// NameGallery, for stretching functionality
#include "cmxrendr.h"
//#include "cmxexdc.h"

#include "nodemold.h"
#include "nodecntr.h" // for ContourBecomeA
#include "textops.h"

#include "blendatt.h"
#include "blndhelp.h"
#include "nodebldr.h"
#include "pathops.h"
#include "brshattr.h"
#include "fthrattr.h"
#include "pathndge.h"
//#include "mrhbits.h"
#include "slicehelper.h"

#include "pbecomea.h"
#include "nodeshad.h"
#include "nodeliveeffect.h"
#include "ophist.h"

CC_IMPLEMENT_DYNAMIC(NodeShadowController, NodeEffect)
CC_IMPLEMENT_DYNAMIC(ShadowRecordHandler, CamelotRecordHandler)
CC_IMPLEMENT_DYNAMIC(ShadowNodeTreeFactory, CompoundNodeTreeFactory)
 
// Give this file in memory dumps
// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW

// Karim 26/07/2000 uncomment this line for better-but-slower glow eor-drags.
//#define GLOW_EOR_DRAG_PATHS

// eor drag path flatness
const MILLIPOINT GlowShadowDragPathFlatness = 2048;

// code which uses 'default' shadow settings should really use these constants.
const ShadowType	NodeShadowController::DefaultType			= SHADOWTYPE_WALL;	// Wall shadow.
const MILLIPOINT	NodeShadowController::DefaultWallOffsetX	= 5 * 750;	// 5 pixels.
const MILLIPOINT	NodeShadowController::DefaultWallOffsetY	= -5 * 750;	// 5 pixels.
const MILLIPOINT	NodeShadowController::DefaultGlowWidth		= 4 * 750;	// 4 pixels.
const double		NodeShadowController::DefaultFloorAngle		= PI / 4.0;	// 45 degrees clockwise.
const float			NodeShadowController::DefaultFloorHeight	= 0.5;		// half shadowed object's height.
const MILLIPOINT	NodeShadowController::DefaultBlurSize		= 6 * 750;	// 6 pixels.
const double		NodeShadowController::DefaultDarkness		= 0.25;		// 25% darkness (75% transparent).
const double		NodeShadowController::DefaultScale			= 1.0;		// actually unused - always set at 1.0 .
const MILLIPOINT	NodeShadowController::DefaultFeatherWidth	= 2 * 750;	// 2 pixels.

//MILLIPOINT			NodeShadowController::DefaultPixelsPerInch	= 0;		// Automatic pixels per inch for capturing/cacheing

/*********************************************************************************************

>    NodeShadowController::NodeShadowController() 

     Author:    Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
     Created:   20/11/96
     Inputs:    -
     Outputs:   
     Returns:   -
			  
     Purpose: This constructor creates a NodeShadowController 
			
     Errors:    -

**********************************************************************************************/
 
NodeShadowController::NodeShadowController() : NodeEffect()
{
	// Initialise member vars.
	m_OffsetX	= DefaultWallOffsetX;
	m_OffsetY	= DefaultWallOffsetY;
	m_FloorShadowAngle	= DefaultFloorAngle;
	m_FloorShadowHeight	= DefaultFloorHeight;
	m_GlowWidth			= DefaultGlowWidth;
	m_FeatherWidth		= DefaultFeatherWidth;

	SetPenumbraWidth(DefaultBlurSize);
	m_ShadowType	= DefaultType;
	m_Scale			= DefaultScale;

	m_ShadowSelectionChanged = FALSE;
	m_PreviousRect.MakeEmpty();

	m_GlowDragPath.Initialise();
	m_PerformedExtend = FALSE;

	m_strPostProID = POSTPRO_ID_SHADOW;

#ifdef _DEBUG
	myShadowID = -1;
	myShadowBecomeAID = -1;
#endif

	TRACEUSER( "MarkH", _T("Just Made a NodeShadowController Class!\n"));
}
	
NodeShadowController::~NodeShadowController()
{
#ifdef _DEBUG

	if (myShadowID > -1)
	{
		TCHAR			strId[100];
		camSnprintf( strId, 100, _T("Popping NodeShadowController ID:  %i\n"), myShadowID );
			
		TRACEUSER( "ChrisS", strId);
	}

#endif
}

/***********************************************************************************************

>   void NodeShadowController::NodeShadowController()
	(
		Node* ContextNode,  
		AttachNodeDirection Direction,  
		BOOL Locked = FALSE, 
		BOOL Mangled = FALSE,  
		BOOL Marked = FALSE, 
		BOOL Selected = FALSE, 
	)

    Author:  Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
    Created: 20/11/96
    
    Inputs: ContextNode: Pointer to a node which this node is to be attached to.     
    
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
			
    Outputs:   -
    Returns:   - 
    Purpose: This method initialises the node and links it to ContextNode in the
			 direction specified by Direction. All necessary tree links are
			 updated.     
			 
    Errors:  An assertion error will occur if ContextNode is NULL


***********************************************************************************************/

NodeShadowController::NodeShadowController(Node* ContextNode,  
					 AttachNodeDirection Direction,  
					 BOOL Locked, 
				 	 BOOL Mangled,  
					 BOOL Marked, 
					 BOOL Selected   
			   ) : NodeEffect(ContextNode, Direction, Locked, Mangled, Marked, 
			 	Selected) 
{ 
	// Initialise member vars
	m_OffsetX			= DefaultWallOffsetX;
	m_OffsetY			= DefaultWallOffsetY;
	m_GlowWidth			= DefaultGlowWidth;
	m_FeatherWidth		= DefaultFeatherWidth;

	SetPenumbraWidth (4 * 750);				// 4 Pixels
	m_ShadowSelectionChanged = FALSE;
	m_PreviousRect.MakeEmpty();
	m_ShadowType = SHADOWTYPE_WALL;
	m_Scale		= 1.0;

	m_GlowDragPath.Initialise();

	m_strPostProID = POSTPRO_ID_SHADOW;

#ifdef _DEBUG
	myShadowID = -1;
	myShadowBecomeAID = -1;
#endif

	TRACEUSER( "MarkH", _T("Just Made a NodeShadowController Class!\n"));
} 


/********************************************************************************************

>	virtual void NodeShadowController::RenderEorDrag( RenderRegion* pRegion )

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/8/99
	Inputs:		pRegion - A Pointer to a RenderRegion to render to
	Purpose:	Renders the EOR drag for shadows
	SeeAlso:	NodeRenderableInk::RenderEorDrag

********************************************************************************************/
void NodeShadowController::RenderEorDrag( RenderRegion* pRender )
{
	// invoke the dragging code for the whole shadow - first, render all children
	// depth first
	// Shadows will render themselves
	RenderEorDragChildren(pRender);
}



/********************************************************************************************

>	virtual BOOL NodeShadowController::AllowOp(	ObjChangeParam *pParam,
												BOOL SetOpPermissionState = TRUE,
												BOOL DoPreTriggerEdit = TRUE)

	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>; Karim 20/01/2000
	Created:	20/11/96
	Inputs:		pParam					describes the way an op wants to change the node
				SetOpPermissionState	if TRUE the OpPermission of nodes should be set
				DoPreTriggerEdit		if TRUE then NameGallery::PreTriggerEdit is called.
										*Must* be TRUE if the calling Op may make any nodes
										change their bounds, eg move, line width, cut.
										Use TRUE if unsure.
	Purpose:	
	SeeAlso:	

********************************************************************************************/
BOOL NodeShadowController::AllowOp(ObjChangeParam *pParam, BOOL SetOpPermissionState,
														   BOOL DoPreTriggerEdit)
{
	ERROR2IF(pParam == NULL, FALSE, "NodeShadowController::AllowOp(); NULL input(s) !!");

	// Flags whether or not we decide to allow the Op.
	BOOL allowed = TRUE;

	// Get a ptr to the changing op - it will be used in a few places.
	UndoableOperation* pChangeOp = pParam->GetOpPointer();

	if (pParam->GetDirection() != OBJCHANGE_CALLEDBYCHILD ||
		pParam->GetCallingChild() != NULL)
	{
		BOOL AnyAllowed = AllowOp_AccountForCompound( pParam,
													  SetOpPermissionState,
													  DoPreTriggerEdit );

		// If called by a parent, just pass this result back.
		if (pParam->GetDirection() == OBJCHANGE_CALLEDBYPARENT)
			return AnyAllowed;
	}

	// At this point we must have been called directly by the op or via a child AllowOp().

	// See whether our parent allows the Op.
	if (allowed && Parent != NULL)
	{
		ObjChangeDirection OldDirection = pParam->GetDirection();
		pParam->SetCallingChild(this);
		pParam->SetDirection(OBJCHANGE_CALLEDBYCHILD);
		allowed = Parent->AllowOp(pParam, SetOpPermissionState, DoPreTriggerEdit);
		pParam->SetDirection(OldDirection);
	}

	// ok, Shadows dislike certain types of Op.
	if (pChangeOp != NULL)
	{
PORTNOTE("other", "Removed use of OpCreateNewMould and OpRemoveBlend from NodeShadowController::AllowOp")
#if !defined(EXCLUDE_FROM_XARALX)
		// no direct moulding, please.
		if (pChangeOp->IS_KIND_OF(OpCreateNewMould))
		{
			if (pParam->GetDirection() == OBJCHANGE_CALLEDBYOP)
				allowed = FALSE;
		}

		// if asked to remove a blend, tell the user why we cannot.
		// we check SetOpPermissionState so that the user is allowed
		// to attempt the blend, instead of it being greyed out.
		else if (SetOpPermissionState)
		{
			if (pChangeOp->IS_KIND_OF(OpRemoveBlend))
			{
				::InformWarning(_R(IDS_BLENDINSIDESHADOW));
				allowed = FALSE;
			}
		}
#endif
	}

	else if (	pParam->GetChangeFlags().MultiReplaceNode && 
				pParam->GetDirection() == OBJCHANGE_CALLEDBYOP	)
	{
		allowed = FALSE;
	}

	// if necessary, set permissions for OnChildChange.
	if (SetOpPermissionState)
		SetOpPermission(allowed ? PERMISSION_ALLOWED : PERMISSION_DENIED, TRUE);

	// If we're ok so far and were asked to do a PreTriggerEdit, then
	// determine whether the Op may change the bounds of some nodes.
	// If it may, then call NameGallery::PreTriggerEdit.
	if (allowed && DoPreTriggerEdit)
	{
		// if the Op is non-NULL then query its MayChangeNodeBounds() method.
		UndoableOperation* pChangeOp = pParam->GetOpPointer();
		if (pChangeOp != NULL && pChangeOp->MayChangeNodeBounds() && NameGallery::Instance())
		{
			allowed = NameGallery::Instance()->PreTriggerEdit(pChangeOp, pParam, this);
		}
	}

	return allowed;
}




/********************************************************************************************

>	void NodeShadowController::Transform( TransformBase& Trans )

	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/11/96
	Inputs:		Trans - A transformation object
	Purpose:	Transforms the group object and all its children.

********************************************************************************************/

void NodeShadowController::Transform( TransformBase& Trans )
{
#ifndef NEW_SHADOW_RENDER
	TransformBase *pTrans = &Trans;

	Trans2DMatrix *p2D = NULL;

	if (pTrans->IS_KIND_OF(Trans2DMatrix))
	{
		p2D = (Trans2DMatrix *)pTrans;
		
		if (p2D->GetMatrix().IsTranslation())
		{
			// do nothing
			InvalidateBoundingRect();

			Trans.Transform(&BoundingRectangle.lo, 1);
			Trans.Transform(&BoundingRectangle.hi, 1);
			NodeEffect::Transform(Trans);

			// and transform the glow drag path if necessary
			if (m_ShadowType == SHADOWTYPE_GLOW)
			{
				Trans.Transform(m_GlowDragPath.GetCoordArray(), m_GlowDragPath.GetNumCoords());
			}

			return;
		}
		else
		{
			Matrix pMat = p2D->GetMatrix();
			FIXED16 Elem[4];
			INT32 LElem[2];

			pMat.GetComponents(Elem, LElem);

			double ScaleX = Elem[0].MakeDouble();
			double ScaleY = Elem[3].MakeDouble();
			
			if (Elem[1].MakeDouble() == 0 && Elem[2].MakeDouble() == 0)
			{
				m_OffsetX = (MILLIPOINT)(((double)m_OffsetX) * ScaleX);
				m_OffsetY = (MILLIPOINT)(((double)m_OffsetY) * ScaleY);

				if (ScaleX == ScaleY)
				{
					SetPenumbraWidth ((MILLIPOINT)(((double)m_PenumbraWidth) * fabs(ScaleX)));
					m_GlowWidth = (MILLIPOINT)(((double)m_GlowWidth) * fabs(ScaleX));
					m_FeatherWidth = (MILLIPOINT)(((double)m_FeatherWidth) * fabs(ScaleX));
				}
			}
		}
	}

	// Delete the cached shadow infomation

	NodeShadow* pShadow = GetShadow();
	if (pShadow != NULL)
	{
		// Transform all the children
		NodeEffect::Transform(Trans);

		// now, reformat all text nodes under me
		List TextNodeList;
		NodeListItem * pItem = NULL;
							
		BevelTools::GetAllNodesUnderNode(this, &TextNodeList, 
					CC_RUNTIME_CLASS(TextStory));

		pItem = (NodeListItem *)TextNodeList.GetHead();
		
		while (pItem)
		{
			if (pItem->pNode)
			{
				((TextStory *)pItem->pNode)->FormatAndChildren(NULL, FALSE);
			}
			
			pItem = (NodeListItem *)TextNodeList.GetNext(pItem);
		}
		
		TextNodeList.DeleteAll();

		RegenerateNode(NULL, FALSE, FALSE);
	}	
#else	// NEW_SHADOW_RENDER

	if (IsCapturingChildren())
	{
		CBitmapCache* pBitmapCache = Camelot.GetBitmapCache();
		ERROR3IF(pBitmapCache==NULL, "Can't find BitmapCache");

		// Translate the cached bitmaps
		CCachedBitmap cbmp;
		CBitmapCacheKey inky(this, GetPixelWidth(), 0);						// Get cached BMP for this ORIGINAL node at our dpi
		BOOL bFound = pBitmapCache->Lookup(inky, cbmp);
		if (bFound)
		{
			cbmp.Transform(Trans);
			pBitmapCache->StoreBitmap(inky, cbmp);
		}
		else
			Trans.bHaveTransformedCached = FALSE;
	}
	else
	{
		// Old-style shadows need the transformed outlines of their objects during dragging
		// So we must ensure transformation...
		Trans.bTransformYourChildren = TRUE;
	}

	// Preserve current state of transformed cache flag
//	BOOL bTransCache = Trans.bHaveTransformedCached;

	// Transform all the children and invalidate my bounds...
	NodeEffect::Transform(Trans);

//	Trans.bHaveTransformedCached = bTransCache;

	if (Trans.IS_KIND_OF(Trans2DMatrix))
	{
		Trans2DMatrix* p2D = (Trans2DMatrix*)&Trans;
		
		if (Trans.IsTranslation())
		{
			// do nothing
			InvalidateBoundingRect();

			Trans.Transform(&BoundingRectangle.lo, 1);
			Trans.Transform(&BoundingRectangle.hi, 1);

			// and transform the glow drag path if necessary
			if (m_ShadowType == SHADOWTYPE_GLOW)
			{
				Trans.Transform(m_GlowDragPath.GetCoordArray(), m_GlowDragPath.GetNumCoords());
			}
		}
		else
		{
			Matrix pMat = p2D->GetMatrix();
			FIXED16 Elem[4];
			INT32 LElem[2];

			pMat.GetComponents(Elem, LElem);

			double ScaleX = Elem[0].MakeDouble();
			double ScaleY = Elem[3].MakeDouble();
			
			if (Elem[1].MakeDouble() == 0 && Elem[2].MakeDouble() == 0)
			{
				m_OffsetX = (MILLIPOINT)(((double)m_OffsetX) * ScaleX);
				m_OffsetY = (MILLIPOINT)(((double)m_OffsetY) * ScaleY);

				if (ScaleX == ScaleY)
				{
					SetPenumbraWidth ((MILLIPOINT)(((double)m_PenumbraWidth) * fabs(ScaleX)));
					m_GlowWidth = (MILLIPOINT)(((double)m_GlowWidth) * fabs(ScaleX));
					m_FeatherWidth = (MILLIPOINT)(((double)m_FeatherWidth) * fabs(ScaleX));
				}
			}
		}
	}

	// Update the cached shadow information and reformat
	NodeShadow* pShadow = NULL;
	if (!Trans.IsTranslation())
	{
		// -------------------------------------------------
		// now, reformat all text nodes under me
		List TextNodeList;
		NodeListItem * pItem = NULL;
		BevelTools::GetAllNodesUnderNode(this, &TextNodeList, 
					CC_RUNTIME_CLASS(TextStory));

		pItem = (NodeListItem *)TextNodeList.GetHead();
		
		while (pItem)
		{
			if (pItem->pNode)
				((TextStory *)pItem->pNode)->FormatAndChildren(NULL, FALSE);
			
			pItem = (NodeListItem *)TextNodeList.GetNext(pItem);
		}
		
		TextNodeList.DeleteAll();

		// -------------------------------------------------
		// Ensure shadow gets rebuilt
		if (!IsCapturingChildren())
			RegenerateNode(NULL, FALSE, FALSE);
	}

	if (IsCapturingChildren())
	{
		// Recreate shadows using the cached (transformed) bitmap
		CBitmapCache* pBitmapCache = Camelot.GetBitmapCache();
		if (this->IsTopShadowController())
		{
			// And we must remake all of our shadows whether we remade the basic bitmap
			// or not...
			pShadow = FindBottomShadow();
			BOOL bRelease = FALSE;
			while (pShadow)
			{
				// First try to transform the shadow without regenerating
				BOOL bOK = pShadow->TransformShadow(Trans);

				// If that didn't work then we must regenerate (by simply releasing all cached data)
				if (!bOK)
				{
					pShadow->DeleteCache();
					bRelease = TRUE;			// Remains TRUE for all controllers to the top of the stack
				}

				if (bRelease)
				{
					NodeCompound* pController = pShadow->GetParentController();
					if (pController)
					{
						// Can't call ReleaseCached because it will ignore the bSelf flag because it is being dragged
						// So release the data directly... ARGH!!!!
						// pController->ReleaseCached(FALSE, FALSE, TRUE, TRUE);	// Self and Derived data
						if (pBitmapCache!=NULL)
						{
							CBitmapCacheKey inky(pController, 42);
							pBitmapCache->RemoveAllOwnedBitmaps(inky, FALSE, CACHEPRIORITY_PERMANENT);
						}
					}
				}

				pShadow = FindShadowAbove(pShadow);
			}
		}
	}

	// If the current transform is a translation then we have successfully
	// transformed our cached info so don't allow our children to modify the state of
	// bHaveTransformedCached
//	if (IsCapturingChildren())
//	{
//		if (bTransCache && Trans.IsTranslation())
//			Trans.bHaveTransformedCached = TRUE;
//		//else leave the flag alone!
//
//	}
//	else
//	{
//		Trans.bHaveTransformedCached = FALSE;
//	}
#endif
}




/********************************************************************************************
>	DocRect NodeShadowController::GetBlobBoundingRect()

	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/11/96
	Returns:	DocRect - The bounding rect of the group and its blobs
	Purpose:	Finds the bounding rect of the group along with its blobs. Its main use is
				to determine which areas of the document to invalidate
********************************************************************************************/

DocRect NodeShadowController::GetBlobBoundingRect()
{
	DocRect dr = GetBoundingRect(TRUE, FALSE);

	dr = dr.Union(NodeRenderableInk::GetBlobBoundingRect());

	BlobManager* BlobMgr = GetApplication()->GetBlobManager();

//	DocView * pView = DocView::GetCurrent();

	if (BlobMgr)
	{
		if (m_ShadowType == SHADOWTYPE_GLOW)
		{
			dr.Inflate(BlobMgr->GetBlobSize() * 3);
		}
		else
		{
			dr.Inflate(BlobMgr->GetBlobSize());
		}
	}

	return dr;


}




/********************************************************************************************

>	virtual String NodeShadowController::Describe(BOOL Plural, BOOL Verbose = TRUE)

	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/11/96
	Inputs:		Plural: Flag indicating if the string description should be plural or
						singular. 
	Outputs:	-
	Retuns:		Description of the group node 
	Purpose:	To return a description of the Group object in either the singular or the 
				plural. This method is called by the DescribeRange method.
				
				The description will always begin with a lower case letter.   
				
	Errors:		(Need to do this)
	SeeAlso:	-

********************************************************************************************/

String NodeShadowController::Describe(BOOL Plural, BOOL Verbose)
{
	if (Plural)
		return(String(_R(IDS_SHADOWNODENAME_PLURAL)));
	else
		return(String(_R(IDS_SHADOWNODENAME)));
};




/***********************************************************************************************
> Node* NodeShadowController::SimpleCopy()  

    Author:     Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
    Created:    20/11/96
	Inputs:     -  
    Outputs:    
    Returns:    A copy of the node, or NULL if memory has run out 
		 
    Purpose:    This method returns a shallow copy of the node with all Node pointers NULL. 
				The function is virtual, and must be defined for all derived classes.  
	            
	Errors:     If memory runs out when trying to copy, then ERROR is called with an out of memory
				error and the function returns NULL.                                                                      
                                                                                 
**********************************************************************************************/

Node* NodeShadowController::SimpleCopy()
{
	NodeShadowController* NodeCopy; 
	NodeCopy = new NodeShadowController();
	ERRORIF(NodeCopy == NULL, _R(IDE_NOMORE_MEMORY), NULL); 

	CopyNodeContents(NodeCopy);

	return (NodeCopy);
}   




/********************************************************************************************
>	void NodeShadowController::CopyNodeContents(NodeShadowController* pNewNode)

 	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/01/2004
	Inputs:		-
	Returns:	
	Purpose:	Sort of backwards copy constructor

********************************************************************************************/
void NodeShadowController::CopyNodeContents(NodeShadowController* pNewNode)
{
	pNewNode->m_ShadowSelectionChanged = m_ShadowSelectionChanged;

	pNewNode->m_sShadowName = m_sShadowName;

	pNewNode->m_PreviousRect = m_PreviousRect;

	pNewNode->m_PerformedExtend = m_PerformedExtend;

	// Remember to copy or blank cached path data without leaking memory...
	pNewNode->m_GlowDragPath.Initialise(m_GlowDragPath.GetNumCoords());
	pNewNode->m_GlowDragPath.CopyPathDataFrom(&m_GlowDragPath);

	pNewNode->SetOffsetX(m_OffsetX);
	pNewNode->SetOffsetY(m_OffsetY);

	pNewNode->SetPenumbraWidth(m_PenumbraWidth);

	pNewNode->SetFloorShadowAngle(m_FloorShadowAngle);
	pNewNode->SetFloorShadowHeight(m_FloorShadowHeight);
	pNewNode->SetShadowType(m_ShadowType);
	pNewNode->SetWallShadowScale(m_Scale);
	pNewNode->SetGlowWidth(m_GlowWidth);
	pNewNode->SetFeatherWidth(m_FeatherWidth);

	// These member assignments shouldn't be here! They are members of NodeCompound!
	pNewNode->m_bBlendStartNode = m_bBlendStartNode;
	pNewNode->m_bBlendEndNode   = m_bBlendEndNode;
	pNewNode->m_pBlendCreatedByNode = m_pBlendCreatedByNode;	

	NodeEffect::CopyNodeContents(pNewNode);

#ifdef NEW_SHADOW_RENDER
	if (IsCapturingChildren())
	{
		CopyCached(pNewNode, GetPixelWidth(), 0);	// Copy cached bitmaps, options 0 and 1 (original and processed)
	}
#endif
}




/***********************************************************************************************
>   void NodeShadowController::PolyCopyNodeContents(NodeRenderable* pNodeCopy)

    Author:     Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
    Created:    18/12/2003
    Outputs:    -
    Purpose:    Polymorphically copies the contents of this node to another
	Errors:     An assertion failure will occur if NodeCopy is NULL
    Scope:      protected
								     
***********************************************************************************************/

void NodeShadowController::PolyCopyNodeContents(NodeRenderable* pNodeCopy)
{
	ENSURE(pNodeCopy, "Trying to copy a node's contents into a NULL node");
	ENSURE(IS_A(pNodeCopy, NodeShadowController), "PolyCopyNodeContents given wrong dest node type");

	if (IS_A(pNodeCopy, NodeShadowController))
		CopyNodeContents((NodeShadowController*)pNodeCopy);
}



   
/********************************************************************************************

>	virtual UINT32 NodeShadowController::GetNodeSize() const

	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/11/96
	Inputs:		-
	Outputs:	-
	Returns:	The size of the node in bytes
	Purpose:	For finding the size of the node 
				
	SeeAlso:	Node::GetSubtreeSize

********************************************************************************************/

UINT32 NodeShadowController::GetNodeSize() const 
{     
	return (sizeof(NodeShadowController)); 
}  


/********************************************************************************************

>	virtual BOOL NodeShadowController::CanBecomeA(BecomeA* pBecomeA) 


	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/11/96
	Inputs:		InkClass: The class	of object
				pNumObjects = ptr to place number of objects of type pClass that will be created (Note: can be NULL).
							  *pNumObects in undefined on entry
	Outputs:	-
	Returns:	TRUE if the node, or any of its children can transmogrify themselves to become 
				an InkClass object

	Purpose:	This function is used by the convert to shapes operation. It determines if 
				the node or any of its children can convert themselves into an InkClass object. 

				The number you put into pNumObjects (if it's not NULL) should exactly equal the total number
				of pClass objects you create.  It should NOT contain any additional objects you may produce
				such as group objects for containing the pClass object, or attributes.

				Also, the entry value of *pNumObjects cannot be assumed to be 0.

	Errors:		-

********************************************************************************************/

BOOL NodeShadowController::CanBecomeA(BecomeA* pBecomeA)
{
	BOOL bOK=FALSE;

	Node* Current = FindFirstChild(); 
	while (Current != NULL)
	{
		if (Current->CanBecomeA(pBecomeA))				// Increments count
		{
			bOK = TRUE;
			if (!pBecomeA->IsCounting())
				return TRUE;							// Don't want total, so return now
		}

		Current = Current->FindNext();
	}

	return bOK;
}


/********************************************************************************************

>	virtual BOOL NodeShadowController::DoBecomeA(BecomeA* pBecomeA)

	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/11/96
	Inputs:		pBecomeA = 	ptr to a class that contains all the info needed to become a new
							type of node.
	Outputs:	-
	Returns:	TRUE if the object has been transformed, FALSE if we run out of memory

	Purpose:	Transforms the object into another type of object. 
				Note: changed 7/10/94 by MarkN to take the pBecomeA param, so that more data could be passed
				to these functions in the future without causing massive rebuilds due to the editing of
				node.h

				14/6/95 (Markn): This now localises attributes before calling it's children, then factors them out
				at the end.  This is so that common attrs trickle down to ALL leaf nodes, no matter how deep 
				they may be.  This is important when a child node is a blend, because the action of DoBecomeA()
				in the blend's case creates new child attrs for the path it creates, hence making any common
				attrs illegal.
	Errors:		-
	SeeAlso:	Node::CanBecomeA

********************************************************************************************/

BOOL NodeShadowController::DoBecomeA(BecomeA* pBecomeA)
{
	ERROR2IF(pBecomeA == NULL,FALSE,"NULL BecomeA ptr");

//	NodeGroup * pGroup = NULL;
	Node * pNode = NULL;
//	Node * pNextNode = NULL;
//	NodeHidden * pHidden = NULL;

	UndoableOperation* pOp = pBecomeA->GetUndoOp();

	// New behaviour: Leave shadow in the tree if the BecomeA claims
	// it will leave the result node in position in the tree
	if (pBecomeA->ResultsStayInPlace() && pBecomeA->IsCombineBecomeA())
	{
		return NodeEffect::DoBecomeA(pBecomeA);
	}

	switch (pBecomeA->GetReason())
	{
	case BECOMEA_REPLACE:
		{
//			ERROR3IF(pOp == NULL, "No operation");

			// if any of our children are selected, then we'll have to remember to select
			// the group created in our place.
			BOOL fSelectBecomeAGroup = IsParentOfSelected();

			// call NodeGroup's implementation first, to convert children to paths.
			NodeCompound::DoBecomeA(pBecomeA);

			// now turn into a group.
			NodeGroup* pGroup = BecomeAGroup(pOp);

			// select the group but don't bother redrawing blobs an' suchlike.
			if (fSelectBecomeAGroup && pGroup != NULL)
				pGroup->Select(FALSE);
		}
		break;

		case BECOMEA_PASSBACK:
		{
			// special behaviour for blends.
			if (pBecomeA->IsBlendBecomeA())
			{
				CompoundNodeBlendBecomeA MyBecomeA(BECOMEA_PASSBACK, CC_RUNTIME_CLASS(NodePath),
					NULL, FALSE, this, pBecomeA);
				
				// now that the become a is set up call all my sub nodes with this
				Node * pChild = FindFirstChild();
				
				while (pChild)
				{
					if (pChild->IsAnObject() && !pChild->NeedsParent(NULL))
					{
						MyBecomeA.ResetCount();
						
						if (pChild->CanBecomeA(&MyBecomeA))
						{
							// tell the become a that we're starting a block
							MyBecomeA.SetNumPathNodes(MyBecomeA.GetCount());
							MyBecomeA.ResetCount();
							
							pChild->DoBecomeA(&MyBecomeA);
						}
					}
					
					pChild = pChild->FindNext();
				}
			}

			// if we're doing shadow silhouettes, then make a silhouette from our shadowed
			// node to pass back, then apply the shadow transform to that silhouette and
			// pass it back again, for the shadowed node.
			// OPTIMISATION: glow shadows enclose the shadowed object,
			// so in that case, we needn't return its path.
			else if (pBecomeA->DoShadowSilhouettes())
			{
				BOOL bGlowOptimisations = (GetShadowType() == SHADOWTYPE_GLOW);

				NodePath* pShadowPath	= NULL;
				NodePath* pInkPath		= NULL;
				NodeRenderableInk* pShadowedNode = GetShadowedNode();

				Path WorkPath;
				WorkPath.Initialise();
				PathBecomeA baInfo(BECOMEA_PASSBACK, CC_RUNTIME_CLASS(NodePath), NULL, FALSE,
										&WorkPath, PathBecomeA::ADD_OUTLINES);

				BOOL	ok = pShadowedNode->DoBecomeA(&baInfo);
				if (ok)	ok = NodePath::CreateFromPath(&pShadowPath, &WorkPath, pBecomeA->GetUndoOp());

				if (ok && !bGlowOptimisations)
						ok = pShadowPath->NodeCopy((Node**)&pInkPath);

				if (ok)	ok = ApplyShadowTransform(&pShadowPath->InkPath);
				if (ok)	ok = pShadowPath->DoBecomeA(pBecomeA);

				if (ok && !bGlowOptimisations)
						ok = pInkPath->DoBecomeA(pBecomeA);

				if (pShadowPath != NULL)
				{
					delete pShadowPath;
					pShadowPath = NULL;
				}
				if (pInkPath != NULL)
				{
					delete pInkPath;
					pInkPath = NULL;
				}

				return ok;
			}

			else
			{
				// Sequentially ask the children of the blend to DoBecomeA
				// This is all that's required because the child objects are only passing back
				// the new node type, and NOT replacing themselves in the tree
				// This also ensures the receipient gets the list of paths in render order
				pNode = FindFirstChild();
				while (pNode != NULL)
				{
					if (!pNode->DoBecomeA(pBecomeA))
						return FALSE;
					
					pNode = pNode->FindNext();
				}
			}
		}
		break;
		default: break;
	}

	return TRUE;

}



/********************************************************************************************

>	BOOL NodeShadowController::ApplyShadowTransform(Path* pTargetPath)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/10/2000
	Inputs:		pTargetPath		the path which will be transformed.

	Outputs:	if successful, pTargetPath will be changed, to represent the result of
				shadowing pTargetPath.

	Returns:	TRUE if successful, FALSE otherwise.

	Purpose:	Apply the shadow transformation to the given Path.
				The resultant path does *not* currently take penumbra width into account.

	Errors:		ERROR2 if pTargetPath is NULL.

********************************************************************************************/
BOOL NodeShadowController::ApplyShadowTransform(Path* pTargetPath)
{
	ERROR2IF(pTargetPath == NULL, FALSE,
			"NodeShadowController::ApplyShadowTransform; NULL param!");

	switch (GetShadowType())
	{
	// translate pTargetPath by the wall offset.
	case SHADOWTYPE_WALL:
		pTargetPath->Translate(GetWallShadowOffset());
		break;

	// contour pTargetPath outwards by the glow width.
	// for some reason, the contour code expects twice the width,
	// as if it thinks it's applying a line width...
	case SHADOWTYPE_GLOW:
	{
		Path ContouredPath;
		ContouredPath.Initialise();
		pTargetPath->InitializeContourValues(2 * GetGlowWidth());
		if (pTargetPath->GetContourForStep(&ContouredPath) > 0)
		{
			pTargetPath->ClearPath();
			pTargetPath->CloneFrom(ContouredPath);
		}

		break;
	}

	// shear pTargetPath and reduce its height.
	case SHADOWTYPE_FLOOR:
	{
		DocRect InkRect;
		pTargetPath->GetTrueBoundingRect(&InkRect);

		DocCoord dcOffset(-InkRect.Centre().x, -InkRect.lo.y);

		Matrix ToOrigin = Matrix::CreateTransMatrix(dcOffset);
		Matrix FromOrigin = Matrix::CreateTransMatrix(-dcOffset);
		Matrix ScaleMat = Matrix::CreateScaleMatrix(1.0, GetFloorShadowHeight());
		Matrix ShearMat = Matrix::CreateShearMatrix(GetFloorShadowAngle());

		Matrix FloorTransform(ToOrigin * ScaleMat * ShearMat * FromOrigin);

		FloorTransform.transform(pTargetPath->GetCoordArray(), pTargetPath->GetNumCoords());

		break;
	}

	case SHADOWTYPE_FEATHER:
	{
		// No transform for feather? (Yet)
		break;
	}

	default:
		ERROR3("NodeShadowController::ApplyShadowTransform; Unrecognised shadow type!");
		return FALSE;
	}

	return TRUE;
}



/********************************************************************************************

>	virtual void NodeShadowController::SelectInRect(const DocRect& Rect, SelStateAction st)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14 April 2000
	Inputs:		Rect	const reference to the bounding rect to use.
				st		the selection state, ie SET/TOGGLE/CLEAR.
	Outputs:	This node may have its selection status changed, depending on the inputs.

	Purpose:	Helper method for the static fn SelectAllInRect() used by marquee select.
				This method changes the selection state of this node according to the given
				bounding rectangle and sel-state action. Most nodes will want to use the
				default behaviour, which is to select themselves if their bounds lie within
				the given bounding rectangle. If you want to do something special with the
				marquee select, then override this method.

	Notes:		This method is meant to be called solely from SelectAllInRect() - if you wish
				to call it from elsewhere, it *may* work as you expect, but it is a good idea
				that you check your assumptions on what preconditions are necessary.

	Errors:		ERROR3 in DEBUG if st holds an invalid state.
	See also:	SelectAllInRect().

********************************************************************************************/
void NodeShadowController::SelectInRect(const DocRect& Rect, SelStateAction st)
{
	// try to select each of our NodeRenderableInk children.
	NodeRenderableInk* pInkChild = NULL;
	Node* pChild = FindFirstChild();
	while (pChild != NULL)
	{
		if (pChild->IsAnObject())
		{
			pInkChild = (NodeRenderableInk*)pChild;
			if (Rect.ContainsRect(pInkChild->GetBoundingRect()))
			{
				switch (st)
				{
				case CLEAR:
					if (pInkChild->MarqueeSelectNode())
					{
						pInkChild->DeSelect(TRUE);
					}
					break;

				case SET:
					if (pInkChild->MarqueeSelectNode())
					{
						pInkChild->Select(TRUE);
					}
					break;

				case TOGGLE:
					if (pInkChild->MarqueeSelectNode())
					{
						if (pInkChild->IsSelected())
							pInkChild->DeSelect(TRUE);
						else
							pInkChild->Select(TRUE);
					}
					break;

				default:
					ERROR3("NodeShadowController::SelectInRect; Unknown SelStateAction!\n");
					return;
				}
			}
		}
		pChild = pChild->FindNext();
	}
}



/********************************************************************************************

>	BOOL NodeShadowController::OnClick( DocCoord PointerPos, ClickType Click, ClickModifiers ClickMods,
						     Spread* pSpread )

	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/11/96
	Inputs:		PointerPos - The Location of the mouse pointer at the time of the click
				Click - The type of click received (single, double, drag etc)
				ClickMods - The modifiers to the click (eg shift, control etc )
	Returns:	TRUE if the node claims the click as its own and FALSE if it is
				not interested in the click
	Purpose:	Determines if the user has started a drag on one of the groups blobs.
				If they have then it starts the groups resize operation
		
********************************************************************************************/

BOOL NodeShadowController::OnClick( DocCoord PointerPos, ClickType Click, ClickModifiers ClickMods,
						Spread* pSpread )
{
	// we did not use the click, so let someone else try
	return FALSE;
}




/********************************************************************************************

>	virtual ChangeCode NodeShadowController::OnChildChange(ObjChangeParam* pParam)

	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/11/96
	Inputs:		pParam	= pointer to a object change parameter class
	Returns:	CC_OK		if we have successfully processed the change.
				CC_FAIL		if we cannot handle this particular change and must prevent the
							child from continuing
	Purpose:	This function should be overridden in derived object classes.
				Composite objects can use this function to respond to one of their children
				undergoing a change. They should return CC_FAIL whenever they are unable to
				cope with the change.

				Groups will hide themselves if the op has left it in a state where it has no bounds,
				i.e. all it's node renderable bounded children are gone.

	SeeAlso:	WarnParentOfChange(),AllowOp();

********************************************************************************************/
ChangeCode NodeShadowController::OnChildChange(ObjChangeParam* pParam)
{
	if (m_PerformedExtend)
	{
		// this means that we have done an extend
		// which added the regen action into the action list
		// so no need to regen the node
		// just reset the flag and leave (sjk 4/8/2000)
		m_PerformedExtend = FALSE;
		return CC_OK;
	}

	if (!pParam)
	{
		return NodeEffect::OnChildChange(pParam);
	}

	// get the op out of the parameter
	if (!pParam->GetOpPointer())
	{
		TRACEUSER( "DavidM", _T("OnChildChange - no op\n"));
		// force a redraw of me

		Document * pDoc = Document::GetCurrent();

		if (pParam->GetChangeFlags().RegenerateNode && pParam->GetChangeType() == OBJCHANGE_FINISHED)
		{
			RegenerateNode(NULL, FALSE, FALSE);
		}
			
		pDoc->ForceRedraw(FindParentSpread(), GetBoundingRect(), FALSE, this);

		return NodeEffect::OnChildChange(pParam);
	}

	if (pParam->GetChangeType() != OBJCHANGE_FINISHED)
		return NodeEffect::OnChildChange(pParam);

	UndoableOperation * pUndoOp = NULL;

	if (pParam->GetOpPointer()->IS_KIND_OF(UndoableOperation))
	{
		pUndoOp = pParam->GetOpPointer();
	}

	if (!pUndoOp)
	{
		return NodeEffect::OnChildChange(pParam);
	}

	NodeAttribute * pAttrib = NULL;
	NodeListItem  * pItem	= NULL;

	// check for the op being a transparent attribute application
	if (pUndoOp->IS_KIND_OF(OpApplyAttrib) &&
		!pUndoOp->IS_KIND_OF(OpEditFill))
	{
		CCRuntimeClass * pClass = ((OpApplyAttrib *)pUndoOp)->GetValueChangeType();
		if (pClass)
		{
			// regenerate me !
			if (pClass->IS_KIND_OF(AttrTranspChange) ||
				pClass->IS_KIND_OF(AttrTranspFillGeometry) ||
				pClass->IS_KIND_OF(AttrBevelIndent) ||
				pClass->IS_KIND_OF(AttrBitmapTranspFill) ||
				pClass->IS_KIND_OF(AttrTextureTranspFill) ||
				pClass->IS_KIND_OF(AttrFillGeometry) ||
				pClass->IS_KIND_OF(AttrStartCap) ||
				pClass->IS_KIND_OF(AttrStartArrow) ||
				pClass->IS_KIND_OF(AttrDashPattern) ||
				pClass->IS_KIND_OF(AttrEndArrow) ||
				pClass->IS_KIND_OF(AttrJoinType) ||
				pClass->IS_KIND_OF(AttrBrushType)
				)
			{
				RegenerateNode(pUndoOp, FALSE, FALSE);
				return CC_OK;
			}
		}
		else
		{
			TRACEUSER( "DavidM", _T("3!\n"));
		
			if (pUndoOp->IS_KIND_OF(OpEditFill))
			{
				pAttrib = ((OpEditFill *)pUndoOp)->GetGradFill();

				if (pAttrib)
				{
					if (pAttrib->IsATranspFill())
					{
						TRACEUSER( "DavidM", _T("Trans fill edit\n"));
						RegenerateNode(pUndoOp, FALSE, FALSE);
						return CC_OK;
					}
				}
			}
		}
	}

	Node * pParent = NULL;
	List ChildList;
	BOOL bRegen = FALSE;

	if (pUndoOp->IS_KIND_OF (OpPathNudge))
	{
		RegenerateNode(pUndoOp, FALSE, FALSE);
	}
	else if (pUndoOp->IS_KIND_OF(TransOperation) && 
		!pUndoOp->IS_KIND_OF(OpMovePathPoint))
	{
		if (IsSelected())
			return CC_OK;
		
		// a trans operation has occured - do nothing & leave it up to
		// the transform function
		// first, invalidate my region
// Don't wildly invalidate all the damn time!
// This can cause LiveEffects to be uncached during translation when they can
// translate themeselves perfectly well...
//		pUndoOp->DoInvalidateNodeRegion(this, TRUE);

		// find out if a select-inside drag has occurred (e.g. when a group is shadowed
		// and an object inside the group is selected and dragged then a regeneration 
		// should occur. This isn't taken care of by the transform method as this method
		// isn't called by the children.

		// first, get all the children
		BevelTools::GetAllNodesUnderNode(this, &ChildList, CC_RUNTIME_CLASS(NodeRenderableInk));
		
		pItem = (NodeListItem *)ChildList.GetHead();

		bRegen = FALSE;

		while (pItem)
		{
			// is it selected implying that it is being transformed
			if (pItem->pNode->IsSelected())
			{
				// get & test its parent
				pParent = pItem->pNode->FindParent();
				
				while (pParent)
				{
					// does the parent transform with children ?
					if (pParent->ShouldITransformWithChildren())
					{
						// is the parent this object ?
						if (pParent == this)
						{
							// don't regenerate
							bRegen = FALSE;
							
							// break out of the loop
							pParent = NULL;
						}
						else
						{
							// go on to the next parent 
							pParent = pParent->FindParent();
						}
					}
					else
					{
						// yep, regenerate ! 
						bRegen = TRUE;

						// break the loop
						pParent = NULL;
					}
				}
			}

			pItem = (NodeListItem *)ChildList.GetNext(pItem);
		}

		ChildList.DeleteAll();

		if (bRegen)
		{
			RegenerateNode(pUndoOp, FALSE, FALSE);
		}

		return CC_OK;
	}
	else if (pUndoOp && (pUndoOp->IsKindOf(CC_RUNTIME_CLASS(OpCut)) || pUndoOp->IsKindOf(CC_RUNTIME_CLASS(OpPaste))) )
	{
		RegenerateNode(pUndoOp, FALSE, FALSE);
		return CC_OK;
	}

	Document * pDoc = Document::GetCurrent();

	if (pParam->GetChangeType() == OBJCHANGE_FINISHED)
	{
		if (pParam->GetChangeFlags().RegenerateNode ||
			pParam->GetChangeFlags().DeleteNode)
		{
			Spread* pSpread = FindParentSpread();
			if (pSpread)
			{
				if (pDoc)
					pDoc->ForceRedraw(pSpread, BoundingRectangle, FALSE, this);

				ReleaseCached();
				pUndoOp->DoInvalidateRegion(pSpread, GetBlobBoundingRect());
				RegenerateNode(pUndoOp, FALSE, FALSE);
				pUndoOp->DoInvalidateRegion(pSpread, GetBlobBoundingRect());
			
				if (pDoc)
					pDoc->ForceRedraw(pSpread, GetBoundingRect(), FALSE, this);
			}

			return NodeEffect::OnChildChange(pParam);
		}
	}

// Why do we do this????
	if (pUndoOp)
	{
		Spread * pSpread = (Spread *)FindParent(CC_RUNTIME_CLASS(Spread));

		if (pDoc && pSpread)
		{
			ReleaseCached(TRUE, FALSE, FALSE, TRUE);	// Parents and Derived		// Fix for 11304
			pDoc->ForceRedraw(pSpread, BoundingRectangle, FALSE, this, FALSE);
		}

//		pUndoOp->DoInvalidateRegion(FindParentSpread(), BoundingRectangle);
	}

	// check for a darkness change - in which case, just invalidate and don't re-render
	if (pUndoOp)
	{
		if (pUndoOp->IS_KIND_OF(OpApplyAttribToNode))
		{
			// find out if the attribute it's applying is a transparency attribute
			if (((OpApplyAttribToNode *)pUndoOp)->GetValueChangeType() == CC_RUNTIME_CLASS(AttrTranspChange))
			{
				// exit
				TRACEUSER( "Karim", _T("Should this code be running ?\n"));
				return CC_OK;
			}
		}
	}

	// return the change code
	return NodeEffect::OnChildChange(pParam);
}



/********************************************************************************************
  >	virtual BOOL NodeShadowController::WritePreChildrenWeb(BaseCamelotFilter* pFilter)

 	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/11/96
	Inputs:		pFilter = ptr to the filter
	Returns:	TRUE if record is written, FALSE if not
	Purpose:	Writes the group record to the filter
	SeeAlso:	-
********************************************************************************************/
BOOL NodeShadowController::WritePreChildrenWeb(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter == NULL,FALSE,"NULL filter param");

	CXaraFileRecord Rec(TAG_SHADOWCONTROLLER, TAG_SHADOWCONTROLLER_SIZE);

	BOOL ok = TRUE;

	if (ok) ok = Rec.Init();
	if (ok) ok = Rec.WriteBYTE(m_ShadowType);
	if (ok) ok = Rec.WriteINT32(m_PenumbraWidth);
	// Call the Transformed version as otherwise we get offset from page origin for free!
	// This is not something that we desire.
	if (ok) ok = Rec.WriteINT32(m_OffsetX);
	if (ok) ok = Rec.WriteINT32(m_OffsetY);

	// Karim 13/06/2000
	// Changing FloorShadowAngle to be stored in radians for precision - the down side
	// of this is that the file-format uses a INT32, so we must use some conversion methods
	// to store/retrieve the double shadow value in a INT32 without loss of much precision.
	if (ok) ok = Rec.WriteINT32(this->FloorAngleToINT32(m_FloorShadowAngle));

	INT32 Height = (INT32)(m_FloorShadowHeight * 100);
	
	if (ok) ok = Rec.WriteINT32(Height);

	INT32 Scale = (INT32)(m_Scale * 100.0);

	if (ok) ok = Rec.WriteINT32(Scale);		

	if (m_ShadowType==SHADOWTYPE_FEATHER)
	{
		if (ok) ok = Rec.WriteINT32(m_FeatherWidth);
	}
	else
	{
		if (ok) ok = Rec.WriteINT32(m_GlowWidth);
	}
		
 	if (ok)
	{
		ok = ok && pFilter->Write(&Rec);
	}

	return ok;
#else
	return FALSE;
#endif
}

BOOL NodeShadowController::WritePreChildrenNative(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT
	return WritePreChildrenWeb(pFilter);
#else
	return FALSE;
#endif
}



/********************************************************************************************
>	inline double NodeShadowController::FloorAngleFromINT32(INT32 Angle)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/06/2000
	Inputs:		Angle	a INT32 value in which the floor angle has been stored,
						via FloorAngleToINT32().

	Returns:	The floor-shadow angle in radians, stored in a double.

	Purpose:	Unpack the floor-shadow angle, a double value, from the given INT32.
				Allows us to store this value within a INT32, while maintaining some accuracy.

	See also:	FloorAngleToINT32().
********************************************************************************************/
inline double NodeShadowController::FloorAngleFromINT32(INT32 Angle)
{
	// divide the angle by a million, and ensure it is in the range -2*PI to 2*PI.
	return fmod((double)Angle / 1000000.0, 2.0 * PI);
}



/********************************************************************************************
>	inline INT32 NodeShadowController::FloorAngleToINT32(double Angle)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/06/2000
	Inputs:		Angle	a double value representing the floor-shadow angle in radians.

	Returns:	A INT32 value into which the given Angle has been packed.

	Purpose:	Pack the floor-shadow angle, a double value, into a INT32.
				Allows us to store this value within a INT32, while maintaining some accuracy.

	See also:	FloorAngleFromINT32().
********************************************************************************************/
inline INT32 NodeShadowController::FloorAngleToINT32(double Angle)
{
	// ensure the angle is in the range -2*PI to 2*PI, scale it up by a million,
	// so we're still well within the range of a INT32, and return the result.
	return (INT32)( 1000000.0 * fmod(m_FloorShadowAngle, 2.0 * PI) );
}



/********************************************************************************************

>	void NodeShadowController::ResetShadowSettings()

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/06/2000
	Purpose:	Resets the settings for this shadow to their default values.

	Notes:		Only settings which correspond to the type of this shadow are changed.

********************************************************************************************/
void NodeShadowController::ResetShadowSettings()
{
	// we won't complain too much if we can't get our shadow, as it's not vital here.
	NodeShadow* pShadow = GetShadow();
	ERROR3IF(pShadow == NULL,
			"NodeShadowController::ResetShadowSettings; where's my NodeShadow gone?!?");

	// we need to redraw the areas covered by our shadow before and after the change.
	DocRect drInvalidArea;
	if (pShadow != NULL)
		drInvalidArea = GetShadow()->GetBoundingRect();

	// reset the appropriate settings to defaults.
	switch (GetShadowType())
	{
	case SHADOWTYPE_WALL:
		SetWallShadowOffset(DocCoord(DefaultWallOffsetX, DefaultWallOffsetY), TRUE);
		break;
	case SHADOWTYPE_GLOW:
		SetGlowWidth(DefaultGlowWidth);
		break;
	case SHADOWTYPE_FLOOR:
		SetFloorShadowAngle(DefaultFloorAngle);
		SetFloorShadowHeight(DefaultFloorHeight);
		break;
	case SHADOWTYPE_FEATHER:
		SetFeatherWidth(DefaultFeatherWidth);
		break;
	default:
		ERROR3("NodeShadowController::ResetShadowSettings; unrecognised shadow type!");
		break;
	}

	if (pShadow != NULL)
		drInvalidArea = drInvalidArea.Union(pShadow->GetBoundingRect());

	// if there's any area to redraw, then do so please :o)
	if (!drInvalidArea.IsEmpty())
	{
		Document* pDoc = Document::GetCurrent();
		Spread* pSpread = FindParentSpread();
		if (pDoc != NULL && pSpread != NULL)
			pDoc->ForceRedraw(pSpread, drInvalidArea, FALSE, pShadow);
	}
}



/********************************************************************************************
>	NodeRenderableInk* NodeShadowController::GetShadowedNode() const

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/06/2000
	Returns:	The node which we are shadowing, or NULL if things go tremendously wrong.
	Purpose:	Obtain the node which has been given a shadow via this NodeShadowController.
	Errors:		ERROR3 if we cannot find a node which we have shadowed.
********************************************************************************************/
NodeRenderableInk* NodeShadowController::GetShadowedNode() const
{
	// Our shadowed node is defined as the NodeRenderableInk after our NodeShadow child.
	Node* pKid = GetShadow();
	if (pKid != NULL)
	{
		pKid = pKid->FindNext();
		while (pKid != NULL && !pKid->IsAnObject())
			pKid = pKid->FindNext();
	}

	// pKid is now either NULL, or is the node we're looking for.
// This is legal in some transition cases so don't error
//	ERROR3IF(pKid == NULL, "NodeShadowController::GetShadowedNode; Can't find shadowed node!");
#ifdef _DEBUG
if (pKid==NULL)
{
	TRACE( _T("NodeShadowController::GetShadowedNode; Can't find shadowed node!"));
}
#endif
	return (NodeRenderableInk*)pKid;
}

 
/********************************************************************************************

>	virtual NodeRenderableInk* NodeShadowController::GetInkNodeFromController() const

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/10/2000
	Inputs:		-
	Outputs:	-
	Returns:	The ink node that is used by bevels, shadows and contours
	Purpose:	Ok, I know that this duplicates GetShadowedNode but there is no equivalent
				for bevels and contours so I thought I would make a virtual function and 
				eventually replace GetShadowedNode instead.
				
	SeeAlso:	

********************************************************************************************/

NodeRenderableInk* NodeShadowController::GetInkNodeFromController() const
{
	return GetShadowedNode();
}


/********************************************************************************************

>	virtual NodeRenderableInk* NodeShadowController::GetObjectToApplyTo(CCRuntimeClass* AttrType)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/01/2005
	Inputs:		-
	Outputs:	-
	Returns:	The ink node that the specified attribute should be applied to
	Purpose:	Allows Compounds/Controllers/Effects nodes to re-point attribute application
				at one of their associated nodes instead of themselves...
	SeeAlso:	

********************************************************************************************/

NodeRenderableInk* NodeShadowController::GetObjectToApplyTo(CCRuntimeClass* pAttrType)
{
	// Yes, I know this is a mess... Wouldn't be required if we had IsATranspFill...
	// (See also IsValidEffectAttr)
	// Allow transparent attributes to be applied directly to me...
/*	if (pAttrType == CC_RUNTIME_CLASS(AttrBitmapTranspFill) ||
		pAttrType == CC_RUNTIME_CLASS(AttrConicalTranspFill) ||
		pAttrType == CC_RUNTIME_CLASS(AttrFlatTranspFill) ||
		pAttrType == CC_RUNTIME_CLASS(AttrFourColTranspFill) ||
		pAttrType == CC_RUNTIME_CLASS(AttrLinearTranspFill) ||
		pAttrType == CC_RUNTIME_CLASS(AttrRadialTranspFill) ||
		pAttrType == CC_RUNTIME_CLASS(AttrSquareTranspFill) ||
		pAttrType == CC_RUNTIME_CLASS(AttrTextureTranspFill) ||
		pAttrType == CC_RUNTIME_CLASS(AttrThreeColTranspFill) ||
		pAttrType == CC_RUNTIME_CLASS(AttrTranspFillGeometry) ||
		pAttrType == CC_RUNTIME_CLASS(AttrStrokeTransp) ||

		pAttrType == CC_RUNTIME_CLASS(AttrTranspChange) ||
		pAttrType == CC_RUNTIME_CLASS(AttrStrokeTranspChange)
		)
		return NodeEffect::GetObjectToApplyTo(pAttrType);
*/
	NodeRenderableInk* pInk = GetInkNodeFromController();
	if (pInk)
	{
		NodeRenderableInk* pNode = pInk->GetObjectToApplyTo(pAttrType);
		if (pNode!=this)
			return pNode;
	}

	return NodeEffect::GetObjectToApplyTo(pAttrType);
}




/********************************************************************************************
>	NodeShadow* NodeShadowController::GetShadow()

 	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/12/96
	Inputs:		-
	Returns:	Pointer to child shadow (NULL if not found!)
	Purpose:	Returns a type correct pointer to child shadow
********************************************************************************************/
NodeShadow* NodeShadowController::GetShadow() const
{
	NodeShadow* pBob = (NodeShadow*)FindFirstChild(CC_RUNTIME_CLASS(NodeShadow));

	return pBob;
}




/***********************************************************************************************

>	INT32 NodeShadowController::ComplexHide(UndoableOperation* pOp, Node* pNextInRange)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/8/99
	Inputs:		See base class
	Purpose:	Hides this node

***********************************************************************************************/
INT32 NodeShadowController::ComplexHide(UndoableOperation* pOp, Node* pNextInRange)
{
	NodeHidden * pHidden = NULL;
	BOOL ok = pOp->DoHideNode(this, TRUE, &pHidden, TRUE);

	if (ok)
		return 1;

	return -1;
}



/***********************************************************************************************

>	void NodeShadow::ShowDebugTreeDetails() const

 	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/12/97
	Purpose:	Displays debugging info of the tree
	SeeAlso:	NodeRenderableInk::ShowDebugTreeDetails

***********************************************************************************************/
void NodeShadowController::ShowDebugTreeDetails() const
{                     
#ifdef _DEBUG
	// Display a bit of debugging info
	// For now, we will just call the base class version
	TRACEALL( _T("NodeShadowController  ") );
	NodeRenderableInk::ShowDebugTreeDetails();	
#endif
}




/********************************************************************************************

  >	NodeShadow* NodeShadowController::GetDebugDetails(StringBase* Str)

 	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/12/97
	Outputs:	Str - extra debug information
	Purpose:	Give more information to the debug tree about this node.

********************************************************************************************/
void NodeShadowController::GetDebugDetails(StringBase* Str) 
{          
#ifdef _DEBUG
	NodeRenderableInk::GetDebugDetails(Str); 
	
	if (Str == NULL)
		return;

	String_256 TempStr; 

	TempStr._MakeMsg(TEXT("Penumbra width = #1%d\r\n"),m_PenumbraWidth);   
	(*Str)+=TempStr; 

	/*
	
	(*Str)+= TEXT("Current render method ");
	switch (m_ShadowType)
	{
		case RANDOM:			TempStr = TEXT("Render n copies around a centrepoint (Fails on large penumbras)"); break;
		case LINEWIDTHS:		TempStr = TEXT("Render decreasing linewidths with decreasing transparency (Fails on transparent shadows)"); break;
		case STROKEPATHTOPATH:	TempStr = TEXT("Render StrokePathToPath linewidths (Far too slow (on complex shapes))"); break;
		case FLATTENEXPAND:		TempStr = TEXT("Flatten and expand paths (Fails on inverted paths)"); break;
		case BITMAPFILTER:		TempStr = TEXT("Bitmap shadower"); break;
		default:				TempStr = TEXT("Unknown"); break;
	}
	(*Str) += TempStr;
	*/
#endif
}	


//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------

ShadowRecordHandler::~ShadowRecordHandler()
{
	TRACEUSER( "MarkH", _T("Killing off ShadowRecordHandler!\n"));
}

/********************************************************************************************

>	virtual UINT32* ShadowRecordHandler::GetTagList()

 	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/12/97
	Inputs:		-
	Returns:	Ptr to a list of tag values, terminated by CXFRH_TAG_LIST_END
	Purpose:	Provides the record handler system with a list of records handled by this
				handler
	SeeAlso:	-

********************************************************************************************/

UINT32* ShadowRecordHandler::GetTagList()
{
	static UINT32 TagList[] = {	TAG_SHADOWCONTROLLER,
								TAG_SHADOW,
								CXFRH_TAG_LIST_END};

	return (UINT32*)&TagList;
}

/********************************************************************************************

>	virtual BOOL ShadowRecordHandler::HandleRecord(CXaraFileRecord* pCXaraFileRecord)

 	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/12/97
	Inputs:		pCXaraFileRecord = ptr to record to handle
	Returns:	TRUE if handled successfuly
				FALSE otherwise
	Purpose:	Handles the given record.
	SeeAlso:	-

********************************************************************************************/

BOOL ShadowRecordHandler::HandleRecord(CXaraFileRecord* pCXaraFileRecord)
{
	ERROR2IF(pCXaraFileRecord == NULL,FALSE,"pCXaraFileRecord is NULL");

	BOOL ok = TRUE;

	switch (pCXaraFileRecord->GetTag())
	{
		case TAG_SHADOWCONTROLLER:
			ok = HandleShadowControllerRecord(pCXaraFileRecord);
			break;

		case TAG_SHADOW:
			ok = HandleShadowRecord(pCXaraFileRecord);
			break;

		default:
			ok = FALSE;
			ERROR3_PF(("I don't handle records with the tag (%d)\n",pCXaraFileRecord->GetTag()));
			break;
	}

	return ok;
}


/********************************************************************************************

BOOL ShadowRecordHandler::HandleShadowControllerRecord(CXaraFileRecord* pCXaraFileRecord)

 	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/12/97
	Inputs:		pCXaraFileRecord = ptr to record to handle
	Returns:	TRUE if handled successfuly
				FALSE otherwise
	Purpose:	Handles the given record.
				The record has to be a shadow controller record
	SeeAlso:	-

********************************************************************************************/

BOOL ShadowRecordHandler::HandleShadowControllerRecord(CXaraFileRecord* pCXaraFileRecord)
{
	ERROR2IF(pCXaraFileRecord == NULL,FALSE,"pCXaraFileRecord is NULL");
	ERROR2IF(pCXaraFileRecord->GetTag() != TAG_SHADOWCONTROLLER,FALSE,"I don't handle this tag type");

	TRACEUSER( "MarkH", _T("Reading in NodeShadowController!\n"));

	MILLIPOINT	Width	= 0;
	INT32 XOffset		= 0;
	INT32 YOffset		= 0;
	INT32 INT32FloorShadowAngle = 0;
	INT32 FloorShadowHeight = 0;
	INT32 Scale			= 0;
	INT32 GlowWidth		= 0;
	ShadowType		Type	=	 SHADOWTYPE_WALL;

	BYTE temp;
	BOOL	ok = pCXaraFileRecord->ReadBYTE(&temp);
	Type = (ShadowType)temp;
	if (ok) ok = pCXaraFileRecord->ReadINT32(&Width);
	// Call the Transformed version as otherwise we get offset from page origin for free!
	// This is not something that we desire.
	if (ok) ok = pCXaraFileRecord->ReadINT32(&XOffset);
	if (ok) ok = pCXaraFileRecord->ReadINT32(&YOffset);

	// Karim 13/06/2000
	// Changing FloorShadowAngle to be stored in radians for precision - the down side
	// of this is that the file-format uses a INT32, so we must use some conversion methods
	// to store/retrieve the double shadow value in a INT32 without loss of much precision.
	if (ok) ok = pCXaraFileRecord->ReadINT32(&INT32FloorShadowAngle);

	if (ok) ok = pCXaraFileRecord->ReadINT32(&FloorShadowHeight);
	if (ok) ok = pCXaraFileRecord->ReadINT32(&Scale);
	if (ok) ok = pCXaraFileRecord->ReadINT32(&GlowWidth);

	if (ok)
	{
		NodeShadowController* pController = new NodeShadowController;
		if (pController != NULL)
		{
			pController->SetShadowType(Type);
			pController->SetOffsetX(XOffset);
			pController->SetOffsetY(YOffset);
			pController->SetPenumbraWidth(Width);
			pController->SetFloorShadowAngle(pController->FloorAngleFromINT32(INT32FloorShadowAngle));
			pController->SetFloorShadowHeight((float)(((double)FloorShadowHeight) / 100.0));
			pController->SetWallShadowScale((float)(((double)Scale) / 100.0));
			if (Type==SHADOWTYPE_FEATHER)
				pController->SetFeatherWidth(GlowWidth);
			else
				pController->SetGlowWidth(GlowWidth);
			pController->SetSelected(FALSE);

			ok = InsertNode(pController);
		}
		else
			ok = FALSE;
	}

	return ok;
}



/********************************************************************************************

>	BOOL ShadowRecordHandler::HandleShadowRecord(CXaraFileRecord* pCXaraFileRecord)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/12/97
	Inputs:		pCXaraFileRecord	ptr to record to handle.

	Returns:	TRUE if handled successfully,
				FALSE otherwise.
	Purpose:	Handles the given record.
				The record has to be a shadow record.

	Notes:		Karim 06/02/2001
				Added code to read extra Darkness parameter.
				Because this was not part of the original file format, this needs to be
				read in with no error checking for reading past end of buffer,
				to remain compatible with older file formats. When reading in an older
				file format, the darkness value will then not be read, and will default
				to whatever it was initialised with (currently fully black).

********************************************************************************************/
BOOL ShadowRecordHandler::HandleShadowRecord(CXaraFileRecord* pCXaraFileRecord)
{
	ERROR2IF(pCXaraFileRecord == NULL,FALSE,"pCXaraFileRecord is NULL");
	ERROR2IF(pCXaraFileRecord->GetTag() != TAG_SHADOW,FALSE,"I don't handle this tag type");

	TRACEUSER( "MarkH", _T("Reading in NodeShadow!\n"));
	
	double	Bias	 = 0;
	double	Gain	 = 0;
	double	Darkness = 1.0;

	// No data to read at present
	BOOL ok = TRUE;
	if (ok) ok = pCXaraFileRecord->ReadDOUBLE(&Bias);
	if (ok) ok = pCXaraFileRecord->ReadDOUBLE(&Gain);
	if (ok) ok = pCXaraFileRecord->ReadDOUBLEnoError(&Darkness);

	NodeShadow* pShadow = new NodeShadow;
	CProfileBiasGain Profile;
	Profile.SetBias((AFp)Bias);
	Profile.SetGain((AFp)Gain);

	if (pShadow != NULL)
	{
		pShadow->SetBiasGain(Profile);
		pShadow->SetDarkness(Darkness);
		ok = InsertNode(pShadow);
	}
	else
	{
		ok = FALSE;
	}

	return ok;
}



/********************************************************************************************

>	virtual void ShadowRecordHandler::GetRecordDescriptionText(CXaraFileRecord* pRecord,StringBase* pStr)

 	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/12/97
	Inputs:		pRecord = ptr to a record
				pStr = ptr to string to update
	Returns:	-
	Purpose:	This provides descriptions for the shadow records.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

#ifdef XAR_TREE_DIALOG
void ShadowRecordHandler::GetRecordDescriptionText(CXaraFileRecord* pRecord,StringBase* pStr)
{
	/*
	if (pStr == NULL || pRecord == NULL)
		return;

	char s[256];

	//	 Call base class first
	CamelotRecordHandler::GetRecordDescriptionText(pRecord,pStr);

	switch (pRecord->GetTag())
	{
		case TAG_SHADOWCONTROLLER:
		{
			ShadowType	Type		= NodeShadowController::RenderMethod;
			MILLIPOINT	Width		= 0;
			INT32		XOffset		= 0;
			INT32		YOffset		= 0;
			Matrix		Offset;

			pRecord->ReadBYTE(&Type);
			pRecord->ReadINT32(&Width);
			// Call the Transformed version as otherwise we get offset from page origin for free!
			// This is not something that we desire.
			pRecord->ReadMatrixTrans(&Offset, XOffset, YOffset);

			switch (Type)
			{
				case RANDOM:			(*pStr) += TEXT("Render n copies around a centrepoint (Fails on large penumbras)"); break;
				case LINEWIDTHS:		(*pStr) += TEXT("Render decreasing linewidths with decreasing transparency (Fails on transparent shadows)"); break;
				case STROKEPATHTOPATH:	(*pStr) += TEXT("Render StrokePathToPath linewidths (Far too slow (on complex shapes))"); break;
				case FLATTENEXPAND:		(*pStr) += TEXT("Flatten and expand paths (Fails on inverted paths)"); break;
				case BITMAPFILTER:		(*pStr) += TEXT("Bitmap shadower"); break;
				default:				(*pStr) += TEXT("Unknown"); break;
			}
			(*pStr) += "\r\n";

			camSprintf(s,_T("Width\t\t= %d\r\n"),Width);
			(*pStr) += s;
			(*pStr) += "\r\n";
		}
		break;
	}
	*/
}

#endif


/********************************************************************************************

>	BOOL NodeShadowController::PostImport()


	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/4/99
	Inputs:		
	Returns:	True if successful
	Purpose:	Does post-import stuff

	Notes:		Karim 07/02/2001
				Is this really necessary? Must try removing this fn and seeing if there
				is actually any point in having it!

********************************************************************************************/
BOOL NodeShadowController::PostImport()
{
	// ok, the entire shadow tree structure has now been set up,
	// so force regeneration of the shadow.
	NodeShadow* pShadow = GetShadow();

	if (pShadow == NULL)
	{
		ERROR3("NodeShadowController::PostImport; Cannot find shadow child node!");
		return FALSE;
	}

	pShadow->GenerateShadow();

	return TRUE;
}




/********************************************************************************************

>	BOOL NodeShadowController::PostDuplicate(UndoableOperation* pOp)


	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/4/99
	Inputs:		
	Returns:	True if successful
	Purpose:	Does post-import stuff

********************************************************************************************/
BOOL NodeShadowController::PostDuplicate(UndoableOperation* pOp)
{
	// force a regeneration of the shadow
	NodeShadow * pShadow = (NodeShadow *)FindFirstChild(CC_RUNTIME_CLASS(NodeShadow));

	if (!pShadow)
	{
		ERROR3("Post duplicate - cannot find shadow child node !");
		return FALSE;
	}

	pShadow->DeleteCache();
	pShadow->GenerateShadow();

	NodeRenderableInk::PostDuplicate(pOp);

	IsBoundingRectValid = FALSE;
	DocRect dr = GetBoundingRect(TRUE, FALSE);

	return TRUE;
}




/********************************************************************************************

>	BOOL NodeShadowController::RegenerateNode(UndoableOperation * pUndoOp)


	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/4/99
	Inputs:		
	Returns:	TRUE to indicate that the node was changed, FALSE if nothing changed
	Purpose:	Sets up a regenerate node action, in response to a 

********************************************************************************************/
BOOL NodeShadowController::RegenerateNode(UndoableOperation * pUndoOp, BOOL bCacheRender,
										  BOOL bInformParents)
{
	Spread * pSpread = (Spread *)FindParent(CC_RUNTIME_CLASS(Spread));

	if (!pSpread)
		pSpread = Document::GetSelectedSpread();

	if (!pSpread)
		ERROR2(FALSE, "Can't regenerate node with no spread");
	
	if (pUndoOp)
	{
		// invalidate my region
//		ReleaseCached();
		ReleaseCached(TRUE, FALSE, TRUE, TRUE);		// Fix for 11304
		pUndoOp->DoInvalidateRegion(pSpread, BoundingRectangle);

		ChangePenumbraSizeAction * pAction = NULL;

		if (ChangePenumbraSizeAction::Init(pUndoOp,
									   pUndoOp->GetUndoActionList(),
									   this,
									   m_PenumbraWidth,
									   &pAction, TRUE, bCacheRender) != AC_OK)
		{
			return FALSE;
		}
		else
		{
			// invalidate the region afterwards too
			pUndoOp->DoInvalidateRegion(pSpread, GetBoundingRect(TRUE, FALSE));

			return TRUE;
		}
	}
	else
	{
		// just regen anyway
		// inform parents of before
		
		if (bInformParents)
		{
			PreInformParentsOfRegenerate();
		}

		Document * pDoc = Document::GetCurrent();

		if (pDoc)
		{
			pDoc->ForceRedraw(pSpread, BoundingRectangle, FALSE, this, FALSE);	// Added FALSE params for 2nd part of Job 11021
			pDoc->ForceRedraw(pSpread, GetShadow()->GetBoundingRect(TRUE, FALSE), FALSE, this, FALSE);
		}

		BOOL ok = this->GetShadow()->GenerateShadow();

		InvalidateBoundingRect(TRUE);

		if (pDoc)
		{
			pDoc->ForceRedraw(pSpread, GetBoundingRect(TRUE, FALSE), FALSE, this, FALSE);
		}

		// inform parents of change afterwards
		if (bInformParents)
		{
			PostInformParentsOfRegenerate();		
		}

		return ok;
	} 
	return TRUE;
}



/********************************************************************************************

>	UINT32 NodeShadowController::GetShadowTransp()

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	06/02/2001

	Returns:	Our NodeShadow's transparency value: 0 for opaque, 255 for transparent.

	Errors:		ERROR3 if we have no NodeShadow, returning 0 (opaque).

********************************************************************************************/
UINT32 NodeShadowController::GetShadowTransp()
{
	NodeShadow* pShadow = GetShadow();

	if (pShadow == NULL)
	{
		ERROR3("NodeShadowController::GetShadowDarkness; No NodeShadow present!");
		return 0;
	}

	return pShadow->GetTransp();
}



/********************************************************************************************

>	virtual DocRect NodeShadowController::ValidateExtend(const ExtendParams& ExtParams)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	06/12/1999
	Inputs:		ExtParams		description parameters for the extension.
	Outputs:	
	Returns:	TRUE if extending this Node and its children is a reversible operation,
				FALSE otherwise.
	Purpose:	Tests the reversibility of an Extend operation applied to this node.

				In the case of a NodeShadowController, this function is identical to
				Node's implementation, except that this controller's own NodeShadow
				child is ignored. This allows the NodeShadow to tell its parent to
				extend, without fear of infinite recursion.
	Errors:		
	See also:	IsTypeExtendible(), Extend().

********************************************************************************************/
DocRect NodeShadowController::ValidateExtend(const ExtendParams& ExtParams)
{
	Node* pBob = GetShadow();
	DocRect drMinExtend(INT32_MAX, INT32_MAX, INT32_MAX, INT32_MAX), drThisMinExtend;
	for (	Node* pChildNode = FindFirstChild();
			pChildNode != NULL;
			pChildNode = pChildNode->FindNext()	)
	{
		if (pChildNode == pBob)
			continue;

		drThisMinExtend = pChildNode->ValidateExtend(ExtParams);
		if (drMinExtend.lo.x > drThisMinExtend.lo.x) drMinExtend.lo.x = drThisMinExtend.lo.x;
		if (drMinExtend.lo.y > drThisMinExtend.lo.y) drMinExtend.lo.y = drThisMinExtend.lo.y;
		if (drMinExtend.hi.x > drThisMinExtend.hi.x) drMinExtend.hi.x = drThisMinExtend.hi.x;
		if (drMinExtend.hi.y > drThisMinExtend.hi.y) drMinExtend.hi.y = drThisMinExtend.hi.y;
	}
	return drMinExtend;
}



/********************************************************************************************

>	virtual void NodeShadowController::Extend(const ExtendParams& ExtParams)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	06/12/1999
	Inputs:		ExtParams		description parameters for the extension.
	Outputs:	Some of this node's children may have their dimensions altered.
	Returns:	
	Purpose:	Perform an Extend operation on this Node, and its children if appropriate.

				In the case of a NodeShadowController, this function is identical to
				Node's implementation, except that this controller's own NodeShadow
				child is ignored. This allows the NodeShadow to tell its parent to
				extend, without fear of infinite recursion.
	Errors:		
	See also:	

********************************************************************************************/
void NodeShadowController::Extend(const ExtendParams& ExtParams)
{
	Node* pBob = GetShadow();
	for (	Node* pChildNode = FindFirstChild();
			pChildNode != NULL;
			pChildNode = pChildNode->FindNext()	)
	{
		if (pChildNode == pBob)
			continue;

		pChildNode->Extend(ExtParams);
	}
	// Add an action to regen the shadow here
	// and instead of regening the shadow when the m_PerformedExtend occurs in the child change
	// cut out and let the action do the stuff
	m_PerformedExtend = TRUE; 
	RegenerateNode(ExtParams.pOp, FALSE, FALSE);
}




/***********************************************************************************************

>	BOOL NodeShadowController::EndBlendStep(BlendNodeParam * pParam)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>/Chris Snook
	Created:	12/8/99
	Inputs:		See base class
	Purpose:	Starts the blending of a step

***********************************************************************************************/
BOOL NodeShadowController::EndBlendStep(BlendNodeParam * pParam)
{
	// CGS:  firstly, are we just rendering, or are we making shapes?

	if (!pParam->GetHandleBecomeA ())
	{	
		// were just rendering ....

		BOOL okToRender = TRUE;
		
		// get hold of the current path processor ....
		if (pParam->GetPathProcessor ())
		{
			ERROR2IF(!pParam->GetPathProcessor ()->IS_KIND_OF(SumAllPathsPathProcessor),
					 FALSE, "First path processor isn't a sum all paths path processor");

			// grab the path processor
			SumAllPathsPathProcessor * pProc = (SumAllPathsPathProcessor *) pParam->GetPathProcessor ();

			pProc->SetEnabled(FALSE);		// disable it

			// we can now get on and generate intermediate blend steps.  NOTE:  this node contour
			// controller should not be touched at all - since it is held within the tree
			// so we make a copy that we can play with ....
				
			Node* copy = NULL;
			this->NodeCopy (&copy);

			NodeShadowController* ptrCopy = (NodeShadowController*) copy;

			if (!ptrCopy) { return (FALSE); }		// out of memory

			// find the contour
			NodeShadow * pShadowCopy = (NodeShadow *)ptrCopy->FindFirstChild(CC_RUNTIME_CLASS(NodeShadow));
			ERROR2IF(pShadowCopy == NULL, FALSE, "Can't find the contour node");

#ifdef _DEBUG		// special debug info
			INT32 id = ++NodeShadowController::contourLastID;
			ptrCopy->myContourID = id;
//			char strId [100];
//			wsprintf (strId, "Pushing NodeShadowController ID:  %i\n", ptrCopy->myContourID);
//			TRACEUSER ("ChrisS", strId);
			TRACEUSER("ChrisS", _T("Pushing NodeShadowController ID:  %i\n"), ptrCopy->myContourID);
#endif

	//////////////////////////////////////////////////////////////////////////////////////////////////

			// now the fun begins, lest blend attributes for this particular blend step ....
			
			CCAttrMap * pMap = NULL;						// if pEndControl is a contour as well
			CCAttrMap * pPrimaryShadowAttrMap = NULL;		// if pEndControl is not a contour

			// blend the contour's attributes
			NodeShadowController * pEndControl = (NodeShadowController *)FindAssociatedBlendNode(pParam);

			if (pEndControl)
			{
				// we are blending two shadows YIPEE!		
				
				NodeShadow * pEndShadow = (NodeShadow *)pEndControl->FindFirstChild(CC_RUNTIME_CLASS(NodeShadow));
				NodeRenderableInk * pOrigEndNode = pEndControl->GetBlendCreatedByNode();

				// somethings a bit wrong if either of these fire ....
				ERROR2IF(pOrigEndNode == NULL, FALSE, "Cant find the original contour node which this is based on");
				ERROR2IF(!pOrigEndNode->IS_KIND_OF(NodeShadowController), FALSE, "Original node isn't a NodeShadowController");

				pOrigEndNode = (NodeShadow *)pOrigEndNode->FindFirstChild(CC_RUNTIME_CLASS(NodeShadow));

				BlendPath MyStartPath;
				MyStartPath.Initialise((NodeRenderableInk *)pShadowCopy->PublicCopy(), -1, pShadowCopy, 1, 0, NULL);
				
				BlendPath MyEndPath;
				MyEndPath.Initialise((NodeRenderableInk *)pEndShadow->PublicCopy(), -1, pOrigEndNode, 1, 0, NULL);
				
				BlendNodeParam BNParam;
				BNParam.Init(pParam, &MyStartPath, &MyEndPath);
				
				pMap = new CCAttrMap(30);

				BlendHelpers * pHelp = new BlendHelpers;
				ERRORIF(pHelp == NULL, _R(IDE_NOMORE_MEMORY), FALSE);
				
				// now, perform the blend of the attributes
				if (!pHelp->BlendAttributes(&BNParam, pMap))
				{
					pParam->GetRenderRegion()->PopPathProcessor();
					okToRender = FALSE;
				}
				
				delete pHelp;

				// also blend my attributes & my node contour's attributes
				ptrCopy->BlendParameters ((NodeShadowController*) m_pBlendCreatedByNode, (NodeShadowController*) pOrigEndNode->FindParent(), pParam, pMap);
			}
			else
			{
				// were blending a shadowed node to a non-shadowed node.
				// since we have not generated an attribute map, shadows that are generated
				// render as white ....

				// we need to make a attribute map for *this* NodeShadow, and apply it to
				// the copy ....

				NodeShadow * pContour = (NodeShadow *) FindFirstChild (CC_RUNTIME_CLASS (NodeShadow));

				ERROR2IF (pContour == NULL, FALSE, "Can't find Contour node");

				// build an attribute map for the contour
				pPrimaryShadowAttrMap = CCAttrMap::MakeAppliedAttrMap(pContour);

				// we don't have another node to blend to - but lets not let a little thing like that deter us!	
				ptrCopy->BlendParameters ((NodeShadowController *) m_pBlendCreatedByNode, pParam);
			}

			// are we shadowing a path, or are we shadowing a compound?

			Node* newShadowing = GetShadowDeleteThisNode ();

			if (!newShadowing)			// were shadowing a path
			{
				// scan for the path that original contour was generated with
				NodePath* currentPath = (NodePath *)ptrCopy->FindFirstChild(CC_RUNTIME_CLASS(NodePath));

				// if we find one, we know get on and merge all the paths so that the contour can
				// be generated correctly.

				// we do this by:

				// replacing currentPath by a group that contains all paths on the path processors list
				// (i.e.  we replace it by the blended paths - who have their blended attribute maps
				// applied to them)

				if (currentPath)
				{
					currentPath->CascadeDelete ();
					delete (currentPath);
					currentPath = NULL;

					NodeGroup* newGroup = new NodeGroup ();

					ListItem* pItem = pProc->GetList ()->GetHead ();
					
					Path* blendPath = NULL;
					CCAttrMap* blendMap = NULL;

					while (pItem)
					{
						blendPath = ((SumAllPathsElem *) pItem)->GetPath ();
						blendMap = ((SumAllPathsElem *) pItem)->GetAttrMap ();
						
						NodePath* newPath = new NodePath ();
						INT32 numCoords = blendPath->GetNumCoords ();
						newPath->InkPath.ClearPath ();
						newPath->InkPath.Initialise (numCoords);
						newPath->InkPath.CopyPathDataFrom (blendPath);
						blendMap->ApplyAttributesToNode (newPath);
						newPath->AttachNode(newGroup, LASTCHILD);
						
						pItem = pProc->GetList ()->GetNext (pItem);
						delete ((SumAllPathsElem *) pProc->GetList()->RemoveHead ());
					}
					
					newGroup->AttachNode(pShadowCopy, NEXT);
				}
			}
			else		// were shadowing a compound
			{
				// remove the old compounds subtree
				
				Node* pShadowing = pShadowCopy->FindNext ();

				pShadowing->CascadeDelete ();
				delete (pShadowing);

				// and replace with the new
				
				newShadowing->AttachNode (pShadowCopy, NEXT);
			}

			// apply blended contour attributes ....

			if (pMap)
			{
				pMap->ApplyAttributesToNode (pShadowCopy);
			}

			if (pPrimaryShadowAttrMap)
			{
				pPrimaryShadowAttrMap->ApplyAttributesToNode (pShadowCopy);
			}

			// generate the contour (which will correctly be generated for merged paths if needed)
			
			if (!pShadowCopy->GenerateShadow())
			{
				okToRender = FALSE;
			}

			// now render the contour controller as a complete node (just like the document renders)
			
			if (okToRender)
			{
				RenderRegion* pRegion = pParam->GetRenderRegion();

				if (pRegion)
					pRegion->RenderTreeNoCache(ptrCopy);
			}

			if (pMap)
			{
				pMap->DeleteAttributes ();
				delete pMap;
			}

			if (pPrimaryShadowAttrMap)
			{
				delete pPrimaryShadowAttrMap;
			}

			// CGS:  NodeShadowControllers can safely delete themselves (after I have made the
			// necessary checks to ensure that we also delete any 'linked' bevel/contour ....
			
			if (newShadowing)
			{
				//shadowDeleteThisNode->CascadeDelete ();
				//delete (shadowDeleteThisNode);
				SetShadowDeleteThisNode (NULL);
			}

			ptrCopy->CascadeDelete ();
			delete (ptrCopy);
		}
	}
	else	// were doing a make shapes (i.e.  become a .....)
	{	
		// get hold of the current path processor ....
		if (pParam->GetPathProcessor ())
		{
			ERROR2IF(!pParam->GetPathProcessor ()->IS_KIND_OF(SumAllPathsPathProcessor),
					 FALSE, "First path processor isn't a sum all paths path processor");

			// grab the path processor
			SumAllPathsPathProcessor * pProc = (SumAllPathsPathProcessor *)pParam->GetPathProcessor ();

			pProc->SetEnabled(FALSE);		// disable it

			// we can now get on and generate intermediate blend steps.  NOTE:  this node contour
			// controller should not be touched at all - since it is held within the tree
			// so we make a copy that we can play with ....
				
			Node* copy = NULL;
			this->NodeCopy (&copy);

			NodeShadowController* ptrCopy = (NodeShadowController*) copy;

			if (!ptrCopy) { return (FALSE); }		// out of memory

			// find the contour
			NodeShadow * pShadowCopy = (NodeShadow *)ptrCopy->FindFirstChild(CC_RUNTIME_CLASS(NodeShadow));
			ERROR2IF(pShadowCopy == NULL, FALSE, "Can't find the contour node");

	//////////////////////////////////////////////////////////////////////////////////////////////////

			// now the fun begins, lest blend attributes for this particular blend step ....
			
			CCAttrMap * pMap = NULL;						// if pEndControl is a contour as well
			CCAttrMap * pPrimaryShadowAttrMap = NULL;		// if pEndControl is not a contour

			// blend the contour's attributes
			NodeShadowController * pEndControl = (NodeShadowController *)FindAssociatedBlendNode(pParam);

			if (pEndControl)
			{
				// we are blending two shadows YIPEE!		
				
				NodeShadow * pEndShadow = (NodeShadow *)pEndControl->FindFirstChild(CC_RUNTIME_CLASS(NodeShadow));
				NodeRenderableInk * pOrigEndNode = pEndControl->GetBlendCreatedByNode();

				// somethings a bit wrong if either of these fire ....
				ERROR2IF(pOrigEndNode == NULL, FALSE, "Cant find the original contour node which this is based on");
				ERROR2IF(!pOrigEndNode->IS_KIND_OF(NodeShadowController), FALSE, "Original node isn't a NodeShadowController");

				pOrigEndNode = (NodeShadow *)pOrigEndNode->FindFirstChild(CC_RUNTIME_CLASS(NodeShadow));

				BlendPath MyStartPath;
				MyStartPath.Initialise((NodeRenderableInk *)pShadowCopy->PublicCopy(), -1, pShadowCopy, 1, 0, NULL);
				
				BlendPath MyEndPath;
				MyEndPath.Initialise((NodeRenderableInk *)pEndShadow->PublicCopy(), -1, pOrigEndNode, 1, 0, NULL);
				
				BlendNodeParam BNParam;
				BNParam.Init(pParam, &MyStartPath, &MyEndPath);
				
				pMap = new CCAttrMap(30);

				BlendHelpers * pHelp = new BlendHelpers;
				ERRORIF(pHelp == NULL, _R(IDE_NOMORE_MEMORY), FALSE);
				
				// now, perform the blend of the attributes
				pHelp->BlendAttributes(&BNParam, pMap);
				
				delete pHelp;

				// also blend my attributes & my node contour's attributes
				ptrCopy->BlendParameters ((NodeShadowController*) m_pBlendCreatedByNode, (NodeShadowController*) pOrigEndNode->FindParent(), pParam, pMap);
			}
			else
			{
				// were blending a shadowed node to a non-shadowed node.
				// since we have not generated an attribute map, shadows that are generated
				// render as white ....

				// we need to make a attribute map for *this* NodeShadow, and apply it to
				// the copy ....

				NodeShadow * pContour = (NodeShadow *) FindFirstChild (CC_RUNTIME_CLASS (NodeShadow));

				ERROR2IF (pContour == NULL, FALSE, "Can't find Contour node");

				// build an attribute map for the contour
				pPrimaryShadowAttrMap = CCAttrMap::MakeAppliedAttrMap(pContour);

				// we don't have another node to blend to - but lets not let a little thing like that deter us!	
				ptrCopy->BlendParameters ((NodeShadowController *) m_pBlendCreatedByNode, pParam);
			}

			// are we shadowing a path, or are we shadowing a compound?

			Node* newShadowing = GetShadowThisNode ();
			SetShadowThisNode (NULL);					// ensure that we always reset this

			std::list<NodePath*> pathPtrs;				// a list of the new paths (so that we may call normaliseattributes)

			BOOL normaliseAttributes = FALSE;

			if (!newShadowing)			// were shadowing a path
			{
				// scan for the path that original contour was generated with
				NodePath* currentPath = (NodePath *)ptrCopy->FindFirstChild(CC_RUNTIME_CLASS(NodePath));

				// if we find one, we know get on and merge all the paths so that the contour can
				// be generated correctly.

				// we do this by:

				// replacing currentPath by a group that contains all paths on the path processors list
				// (i.e.  we replace it by the blended paths - who have their blended attribute maps
				// applied to them)

				if (currentPath)
				{
					currentPath->CascadeDelete ();
					delete (currentPath);
					currentPath = NULL;

					NodeGroup* newGroup = new NodeGroup ();

					ListItem* pItem = pProc->GetList ()->GetHead ();
					
					Path* blendPath = NULL;
					CCAttrMap* blendMap = NULL;

					while (pItem)
					{
						blendPath = ((SumAllPathsElem *) pItem)->GetPath ();
						blendMap = ((SumAllPathsElem *) pItem)->GetAttrMap ();
						
						NodePath* newPath = new NodePath ();
						INT32 numCoords = blendPath->GetNumCoords ();
						newPath->InkPath.ClearPath ();
						newPath->InkPath.Initialise (numCoords);
						newPath->InkPath.CopyPathDataFrom (blendPath);
						blendMap->ApplyAttributesToNode (newPath);
						newPath->AttachNode(newGroup, LASTCHILD);

						pathPtrs.push_back (newPath);
						
						pItem = pProc->GetList ()->GetNext (pItem);
						delete ((SumAllPathsElem *) pProc->GetList()->RemoveHead ());
					}
					
					newGroup->AttachNode(pShadowCopy, NEXT);
				}
			}
			else		// were shadowing a compound
			{
				// remove the old compounds subtree
				
				Node* pShadowing = pShadowCopy->FindNext ();

				pShadowing->CascadeDelete ();
				delete (pShadowing);

				// and replace with the new
				
				newShadowing->AttachNode (pShadowCopy, NEXT);

				normaliseAttributes = TRUE;
			}

			// apply blended contour attributes ....

			if (pMap)
			{
				pMap->ApplyAttributesToNode (pShadowCopy);
			}

			if (pPrimaryShadowAttrMap)
			{
				pPrimaryShadowAttrMap->ApplyAttributesToNode (pShadowCopy);
			}

			// generate the contour (which will correctly be generated for merged paths if needed)
			
			pShadowCopy->GenerateShadow();

			// now insert into the tree or passback ....

			BecomeA* accessPtr = pParam->GetHandleBecomeA ()->GetBecomeA ();
			UndoableOperation* pUndoOp = accessPtr->GetUndoOp ();

			// are we wanting to be inserted into the tree?
			if (accessPtr->GetReason () == BECOMEA_REPLACE)
			{
				if (pUndoOp)
				{
					if (!pUndoOp->DoInsertNewNode(ptrCopy,pParam->GetHandleBecomeA ()->GetContextNode (),PREV,TRUE,FALSE,FALSE,TRUE))
					{
						return FALSE;
					}
				}
				else
				{
					ptrCopy->AttachNode(pParam->GetHandleBecomeA()->GetContextNode(), PREV);
				}

				// now normalise all attributes ....
				
				pShadowCopy->NormaliseAttributes ();

				std::list<NodePath*>::iterator i;

				for (i = pathPtrs.begin (); i != pathPtrs.end (); i++)
					(*i)->NormaliseAttributes ();

				if (normaliseAttributes)
				{
					// since were in this piece of code, we are shadowing a bevel or a contour.
					// it is our resonsibility to normalise attribute complete NodePaths that
					// were created by bevels or contours (ask yourself why?  because NodePaths
					// generated in this fashion have NOT been inserted into the document tree
					// yet, and NormaliseAttributes does NOT work on nodes that are NOT in the tree;
					// making it impossible for our child bevel/contour to do the job).

					NodePath* pPath = (NodePath*) SliceHelper::FindNextOfClass(ptrCopy, ptrCopy, CC_RUNTIME_CLASS (NodePath));

					while (pPath)
					{
						pPath->NormaliseAttributes ();
						pPath = (NodePath*) SliceHelper::FindNextOfClass(pPath, ptrCopy, CC_RUNTIME_CLASS (NodePath));
					}

					// same for NodeContours ....

					NodeContour* pContour = (NodeContour*) SliceHelper::FindNextOfClass(ptrCopy, ptrCopy, CC_RUNTIME_CLASS (NodeContour));

					while (pContour)
					{
						pContour->NormaliseAttributes ();
						pContour = (NodeContour*) SliceHelper::FindNextOfClass(pContour, ptrCopy, CC_RUNTIME_CLASS (NodeContour));
					}

					// same for NodeBevels ....

					NodeBevel* pBevel = (NodeBevel*) SliceHelper::FindNextOfClass(ptrCopy, ptrCopy, CC_RUNTIME_CLASS (NodeBevel));

					while (pBevel)
					{
						pBevel->NormaliseAttributes ();
						pBevel = (NodeBevel*) SliceHelper::FindNextOfClass(pBevel, ptrCopy, CC_RUNTIME_CLASS (NodeBevel));
					}
				}

				// if we are required to insert paths into the tree - THEN we MUST call DoBecomeA on
				// ptrCopy.  This call now also creates the bevel bitmap for us

				if (accessPtr->GetInsertComplexBlendStepsAsPaths ())
				{
					ptrCopy->DoBecomeA (accessPtr);
				}
			}
			// or do we just want our paths?
			else if (accessPtr->GetReason () == BECOMEA_PASSBACK)
			{
				ptrCopy->DoBecomeA (accessPtr);
				ptrCopy->SetBlenderNode (pParam->GetNodeBlend ());

#ifdef _DEBUG
				INT32 id = ++NodeShadowController::shadowLastBecomeAID;
				ptrCopy->myShadowBecomeAID = id;
//				char strId [100];
//				wsprintf (strId, "Pushing NodeShadowController PASSBACK ID:  %i\n", ptrCopy->myShadowBecomeAID);
//				TRACEUSER ("ChrisS", strId);
				TRACEUSER ("ChrisS", _T("Pushing NodeShadowController PASSBACK ID:  %i\n"), ptrCopy->myShadowBecomeAID);
#endif
					
				// setup necessary stuff so that we can delete ptrCopy at the relevant time ....
					
				if (AllocatedBlendConsList (LT_BECOMEA_SHADOWSLIST))
				{	
					BlendConsListInsert (LT_BECOMEA_SHADOWSLIST, ptrCopy);
				}
				else
				{
					AllocBlendConsList (LT_BECOMEA_SHADOWSLIST);
					BlendConsListInsert (LT_BECOMEA_SHADOWSLIST, ptrCopy);
				}
			}

			if (pMap)
			{
				pMap->DeleteAttributes ();
				delete pMap;
			}

			if (pPrimaryShadowAttrMap)
			{
				delete pPrimaryShadowAttrMap;
			}
		}
	} 
	return TRUE;
}




/********************************************************************************************

>	BOOL BlendParameters(NodeShadowController * pStart, NodeShadowController * pEnd,
							double Scale, CCAttrMap* pMap)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/3/2000
	Inputs:		pStart	-	the original start shadow controller node (the one in the tree)
				pEnd	-	the original end shadow controller node (the one in the tree)
				Scale	-	Scale factor to blend with
	Outputs:	Sets my & my shadow node child with the appropriate variables
	Returns:	
	Purpose:	Sets my & my shadow node child with the appropriate variables given the blend
				step
	
********************************************************************************************/
BOOL NodeShadowController::BlendParameters(NodeShadowController * pStart, NodeShadowController * pEnd,
							BlendNodeParam * pParam, CCAttrMap* pMap)
{
	double Scale = pParam->GetAttrBlendRatio ();
	double Scale2 = pParam->GetBlendRatio ();
	
	// First, lets get hold of the shadows that we need, and check ....
	
	NodeShadow * pStartShad = (NodeShadow *) pStart->GetShadow ();
	ERROR2IF(!pStartShad, FALSE, "Can't find shadow child node for start");

	NodeShadow * pEndShad = (NodeShadow *) pEnd->GetShadow ();
	ERROR2IF(!pEndShad, FALSE, "Can't find shadow child node for end");

	NodeShadow * pThisShad = (NodeShadow *) GetShadow();
	ERROR2IF(!pThisShad, FALSE, "Can't find shadow child node for blend node");

	// firstly, lets blend the shadow darkness ....
	double startDarkness = pStartShad->GetDarkness();
	double endDarkness	 = pEndShad->GetDarkness();
	double newDarkness	 = startDarkness + Scale * (endDarkness - startDarkness);
	newDarkness =	(newDarkness < 0.0) ? 0.0 :
					(newDarkness > 1.0) ? 1.0 : newDarkness;

	pThisShad->SetDarkness(newDarkness);

	// THEN blend the shadows colour ....

	AttrFillGeometry* pAttrStart = NULL, *pAttrEnd = NULL;
	((NodeRenderableInk*)pStartShad)->FindAppliedAttribute(CC_RUNTIME_CLASS(AttrFillGeometry), (NodeAttribute **)&pAttrStart);
	((NodeRenderableInk*)pEndShad)->FindAppliedAttribute(CC_RUNTIME_CLASS(AttrFillGeometry), (NodeAttribute **)&pAttrEnd);

	ERROR2IF(!pAttrStart, FALSE, "Error - could NOT find start shadow fill!");
	ERROR2IF(!pAttrEnd, FALSE, "Error - could NOT find end shadow fill!");

	AttrFillGeometry* pCurrentFill = NULL;

	// blend the fill attribute.
	if (((NodeRenderableInk*)pThisShad)->FindAppliedAttribute(CC_RUNTIME_CLASS(AttrFillGeometry), (NodeAttribute **)&pCurrentFill))
	{
		pCurrentFill->UnlinkNodeFromTree ();
		delete (pCurrentFill);

		// now create the new one ....

		BlendAttrParam BlendParam;
		
		// Initialise the BlendParam with the end attr and blend ratio
		if (BlendParam.Init(NULL, //pParam->GetRenderRegion(),
							pAttrEnd,Scale,//BlendRatio,
							COLOURBLEND_FADE,//pParam->GetColourBlendType(),
							NULL, NULL))
		{
			// Successfully initialised, so now try blending the attributes
			if (pAttrStart->Blend(&BlendParam))
			{
				// Attrs successfully blended, now get a ptr to the new attr.
				// Once we get the blended attr ptr, it belongs to us, so we have
				// to delete it when it is not needed
				NodeAttribute* pBlendedNodeAttr = BlendParam.GetBlendedAttr();

				if (pBlendedNodeAttr)
				{
					if (pMap)
					{
						delete (pMap->ReplaceAttribute (pBlendedNodeAttr));
					}
				}
			}
		}
	}

	// secondly, lets blend the shadow blur ....

	double StartBlur = (double)pStart->GetPenumbraWidth();
	double EndBlur   = (double)pEnd->GetPenumbraWidth();
	double NewBlur = ((EndBlur - StartBlur) * Scale) + StartBlur;

	SetPenumbraWidth ((MILLIPOINT)NewBlur);

	// thirdly, lets blend the profiles ....

	double StartBias = pStartShad->GetBiasGain().GetBias();
	double EndBias   = pEndShad->GetBiasGain().GetBias();
	double NewBias   = ((EndBias - StartBias) * Scale) + StartBias;

	double StartGain = pStartShad->GetBiasGain().GetGain();
	double EndGain   = pEndShad->GetBiasGain().GetGain();
	double NewGain   = ((EndGain - StartGain) * Scale) + StartGain;

	CProfileBiasGain	BiasGain( NewBias, NewGain );
	pThisShad->SetBiasGain( BiasGain );

	// we should use Scale2 (i.e.  object profile) to ammend the offsets ....

	// let's do the context sensitive stuff
	if (pStart->GetShadowType() == pEnd->GetShadowType())
	{
		switch (pStart->GetShadowType())
		{
		case SHADOWTYPE_WALL:
			{
				// blend the offsets
				double StartOffsetX = (double)pStart->GetOffsetX();
				double EndOffsetX = (double)pEnd->GetOffsetX();
				
				double NewOffsetX = ((EndOffsetX - StartOffsetX) * Scale2) + StartOffsetX;
				
				double StartOffsetY = (double)pStart->GetOffsetY();
				double EndOffsetY = (double)pEnd->GetOffsetY();
				
				double NewOffsetY = ((EndOffsetY - StartOffsetY) * Scale2) + StartOffsetY;
				
				// set my variables
				m_OffsetX = (MILLIPOINT)NewOffsetX;
				m_OffsetY = (MILLIPOINT)NewOffsetY;
			}
			break;
		case SHADOWTYPE_FLOOR:
			{
				double StartAngle = (double)pStart->GetFloorShadowAngle();
				double EndAngle   = (double)pEnd->GetFloorShadowAngle();
				
				double NewAngle = ((EndAngle - StartAngle) * Scale2) + StartAngle;
				
				double StartHeight = (double)pStart->GetFloorShadowHeight();
				double EndHeight = (double)pEnd->GetFloorShadowHeight();
				
				double NewHeight = ((EndHeight - StartHeight) * Scale2) + StartHeight;
				
				// set my variables
				m_FloorShadowAngle = NewAngle;
				m_FloorShadowHeight = (double)NewHeight;
			}
			break;
		case SHADOWTYPE_GLOW:
			{
				double StartGlowWidth = (double)pStart->GetGlowWidth();
				double EndGlowWidth = (double)pEnd->GetGlowWidth();
				
				double NewGlowWidth = ((EndGlowWidth - StartGlowWidth) * Scale2) + StartGlowWidth;
				
				m_GlowWidth = (MILLIPOINT)NewGlowWidth;
			}
			break;
		case SHADOWTYPE_FEATHER:
			{
				double StartFeatherWidth = (double)pStart->GetFeatherWidth();
				double EndFeatherWidth = (double)pEnd->GetFeatherWidth();
				
				double NewFeatherWidth = ((EndFeatherWidth - StartFeatherWidth) * Scale2) + StartFeatherWidth;
				
				m_FeatherWidth = (MILLIPOINT)NewFeatherWidth;
			}
			break;
		default: break;
		}
	}

	return TRUE;
}




/********************************************************************************************

>	BOOL NodeShadowController::BlendParameters(NodeShadowController * pStart, double Scale)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/7/2000
	Inputs:		pStart	-	the original start contour controller node (the one in the tree)
				Scale	-	Scale factor to blend with
	Outputs:	Sets my & my shadow node child with the appropriate variables
	Returns:	
	Purpose:	Sets my & my shadow node child with the appropriate variables given the blend
				step
	
********************************************************************************************/

BOOL NodeShadowController::BlendParameters(NodeShadowController * pStart, BlendNodeParam * pParam)
{
	double Scale = pParam->GetAttrBlendRatio ();
	
	// First, lets get hold of the shadows that we need, and check ....
	
	NodeShadow * pStartShad = (NodeShadow *) pStart->GetShadow ();
	ERROR2IF(!pStartShad, FALSE, "Can't find shadow child node for start");

	NodeShadow * pThisShad = (NodeShadow *) GetShadow();
	ERROR2IF(!pThisShad, FALSE, "Can't find shadow child node for blend node");

	// firstly, lets blend the shadow darkness to fully transparent....
	double startDarkness = pStartShad->GetDarkness();
	double endDarkness	 = 0.0;
	double newDarkness	 = startDarkness + (2 * Scale) * (endDarkness - startDarkness);
	newDarkness =	(newDarkness < 0.0) ? 0.0 :
					(newDarkness > 1.0) ? 1.0 : newDarkness;

	pThisShad->SetDarkness(newDarkness);


	// secondly, lets blend the shadow blur ....

	double StartBlur = (double)pStart->GetPenumbraWidth();
	double EndBlur   = 4 * 750;				// 4 Pixels
	double NewBlur = ((EndBlur - StartBlur) * (2*Scale)) + StartBlur;

	SetPenumbraWidth ((MILLIPOINT) NewBlur);

	// thirdly, lets blend the profiles ....

	CProfileBiasGain defaultBiasGain;

	double StartBias = pStartShad->GetBiasGain().GetBias();
	double EndBias   = defaultBiasGain.GetBias();
	double NewBias   = ((EndBias - StartBias) * (2*Scale)) + StartBias;

	double StartGain = pStartShad->GetBiasGain().GetGain();
	double EndGain   = defaultBiasGain.GetGain();
	double NewGain   = ((EndGain - StartGain) * (2*Scale)) + StartGain;

	CProfileBiasGain	BiasGain( NewBias, NewGain );
	pThisShad->SetBiasGain( BiasGain );
/*
 *	Karim 18/09/2000
 *	Uncommented for a trial run to gauge 'public opinion' on this blend
 *	behaviour. Will comment back out if people dislike it :)
 *
 *	Karim 26/07/2000
 *	Delete this commented code if you don't like it :o)
 *	What it does is blend the wall-offset, floor-vector or glow width
 *	down to zero for a blend from a shadowed to unshadowed node.
 *	The down side is that the blend code does half the blend with start
 *	node and half with the end node, so that blending shadow-to-non gives
 *	intermediate shadows for only half the distance, which doesn't look good.
 */
	switch (pStart->GetShadowType())
	{
	case SHADOWTYPE_WALL:
		{
			// blend the offsets
			double StartOffsetX	= (double)pStart->GetOffsetX();
			double EndOffsetX	= 0.0;

			double NewOffsetX = ((EndOffsetX - StartOffsetX) * (2*Scale)) + StartOffsetX;

			double StartOffsetY	= (double)pStart->GetOffsetY();
			double EndOffsetY	= 0.0;

			double NewOffsetY = ((EndOffsetY - StartOffsetY) * (2*Scale)) + StartOffsetY;

			// set my variables
			m_OffsetX = (MILLIPOINT)NewOffsetX;
			m_OffsetY = (MILLIPOINT)NewOffsetY;
		}
		break;
	case SHADOWTYPE_FLOOR:
		{
			double StartAngle	= (double)pStart->GetFloorShadowAngle();
			double EndAngle		= 0.0;

			double NewAngle = ((EndAngle - StartAngle) * (2*Scale)) + StartAngle;

			double StartHeight	= (double)pStart->GetFloorShadowHeight();
			double EndHeight	= 0.0;

			double NewHeight = ((EndHeight - StartHeight) * (2*Scale)) + StartHeight;

			// set my variables
			m_FloorShadowAngle	= NewAngle;
			m_FloorShadowHeight	= (double)NewHeight;
		}
		break;
	case SHADOWTYPE_GLOW:
		{
			double StartGlowWidth	= (double)pStart->GetGlowWidth();
			double EndGlowWidth		= 0.0;

			double NewGlowWidth = ((EndGlowWidth - StartGlowWidth) * (2*Scale)) + StartGlowWidth;

			m_GlowWidth = (MILLIPOINT)NewGlowWidth;
		}
		break;
	case SHADOWTYPE_FEATHER:
		{
			double StartFeatherWidth	= (double)pStart->GetFeatherWidth();
			double EndFeatherWidth		= 0.0;

			double NewFeatherWidth = ((EndFeatherWidth - StartFeatherWidth) * (2*Scale)) + StartFeatherWidth;

			m_FeatherWidth = (MILLIPOINT)NewFeatherWidth;
		}
		break;
	default:
		break;
	}
/**/
	return (TRUE);
}



/********************************************************************************************

>	virtual NodeRenderableInk * 
			NodeShadowController::CreateTreeFromNodeToBlend(NodeRenderableInk * pNode)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/2/2000
	Inputs:		The node to wrap up
	Outputs:	NULL for failure
	Returns:	The root of the new tree
	Purpose:	Creates a copy of my tree, wrapping up the given node
	
********************************************************************************************/
NodeRenderableInk * 
			NodeShadowController::CreateTreeFromNodeToBlend(NodeRenderableInk * pNode, 
			CCAttrMap * pAttrMap)
{
	// remove all attributes from the node's subtree
	ERROR2IF(pNode->FindParent() != NULL, NULL, "Node shouldn't have parents");

	if (pNode->IsNodePath())
	{
		pNode->CascadeDelete();
	}
	
	// apply the attribute map to this node
	if (pNode->IsNodePath() && pAttrMap)
	{
		pNode->ApplyAttributes(pAttrMap, FALSE);
	}

	// now, lets make a copy of myself & the bevel node
	NodeShadowController * pControl = NULL;

	pControl = (NodeShadowController *)this->PublicCopy();
	ERRORIF(pControl == NULL, _R(IDE_NOMORE_MEMORY), NULL);

	NodeShadow * pShadow = (NodeShadow *)FindFirstChild(CC_RUNTIME_CLASS(NodeShadow));
	ERROR2IF(pShadow == NULL, NULL, "Can't find bevel node");

	// apply the attribute map of the original bevel node to the bevel copy node
	NodeShadow * pCopyShadow = (NodeShadow *)pShadow->PublicCopy();
	ERRORIF(pCopyShadow == NULL, _R(IDE_NOMORE_MEMORY), NULL);
	
	CCAttrMap * pShadowMap = CCAttrMap::MakeAppliedAttrMap(pShadow);
	ERRORIF(pShadowMap == NULL, _R(IDE_NOMORE_MEMORY), NULL);

#ifdef _DEBUG
	CCAttrMap::iterator pos = pShadowMap->GetStartPosition();
	while( pos != pShadowMap->GetEndPosition() )
	{
		CCRuntimeClass *pKey;
		NodeAttribute  *pVal;
		pShadowMap->GetNextAssoc( pos, pKey, (PVOID &)pVal );

		INT32 i = 0;
		i++;
	}
#endif

	pCopyShadow->ApplyAttributes(pShadowMap, FALSE);

	delete pShadowMap;
	
	pNode->AttachNode(pControl, FIRSTCHILD);

	pCopyShadow->AttachNode(pControl, FIRSTCHILD);

	return pControl;
}



/********************************************************************************************

>	void NodeShadowController::SetFloorShadowVector(const DocCoord& dcControlVector)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/06/2000
	Inputs:		Floor-shadow control vector, in a DocCoord (millipoint units).
	Purpose:	Set the floor-shadow control vector.
				This is defined as a vector from the centre of the base of the floor shadow
				to its top-centre point.

				The height and angle which this vector represents will be clipped into range
				if they lie outside an acceptable range for floor-shadow parameters.

	SeeAlso:	MAXSHADOWHEIGHT, MAXSHADOWANGLE.

********************************************************************************************/
void NodeShadowController::SetFloorShadowVector(const DocCoord& dcControlVector)
{
	// the 'height' of the floor shadow is the ratio of the height of this vector
	// to the base height.
	INT32 BaseHeight = GetInsideBoundingRect().Height();
	double HeightRatio = 0.0;
	if (BaseHeight > 0)
		HeightRatio = (double)dcControlVector.y / (double)BaseHeight;
	else
		HeightRatio = 1.0;

	if (HeightRatio > MAXSHADOWHEIGHT)
		HeightRatio = MAXSHADOWHEIGHT;
	else if (HeightRatio < 0.01)
		HeightRatio = 0.01;

	SetFloorShadowHeight((float)HeightRatio);

	// the angle of the floor shadow is the clockwise rotation of the control vector
	// from the vertical, in degrees.
	double Angle = dcControlVector.AngleFromVertical();
	if (Angle < -MAXSHADOWANGLE)
		Angle = -MAXSHADOWANGLE;
	else if (Angle > MAXSHADOWANGLE)
		Angle = MAXSHADOWANGLE;

	SetFloorShadowAngle(Angle);
}



/********************************************************************************************

>	DocCoord NodeShadowController::GetFloorShadowVector()

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/06/2000
	Returns:	Floor-shadow control vector, in a DocCoord (millipoint units).
	Purpose:	Get the floor-shadow control vector.
				This is defined as a vector from the centre of the base of the floor shadow
				to its top-centre.

********************************************************************************************/
DocCoord NodeShadowController::GetFloorShadowVector()
{
	// height of the control vector is 'height' of the floor shadow * BaseHeight.
	// width of the control vector is tan(floor-shadow angle) * control vector height.
	INT32				BaseHeight = GetInsideBoundingRect().Height();
	INT32				Height = INT32(GetFloorShadowHeight() * BaseHeight);
	return DocCoord( INT32(Height * tan( GetFloorShadowAngle() )), Height);
}



/********************************************************************************************

>	void NodeShadowController::SetWallShadowOffset(	const DocCoord& dcOffset,
													BOOL UpdateNodeShadow )
	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/06/2000
	Inputs:		dcOffset			Offset of the wall shadow, in millipoints.
				UpdateNodeShadow	Whether or not the associated NodeShadow should be
									updated - should be TRUE in mose cases.

	Purpose:	Set the offset of the wall shadow from the shadowed object.

********************************************************************************************/
void NodeShadowController::SetWallShadowOffset(	const DocCoord& dcOffset,
												BOOL UpdateNodeShadow )
{
	// we're a bit sunk if we can't find our related NodeShadow.
	NodeShadow* pShadow = GetShadow();
	if (pShadow == NULL)
	{
		ERROR3("NodeShadowController::SetWallShadowOffset; can't find NodeShadow!");
		return;
	}

	// appropriately translate any fills or moveable attrs applied to our NodeShadow.
	Trans2DMatrix transAttrs(	dcOffset.x - m_OffsetX,
								dcOffset.y - m_OffsetY );
	pShadow->TransformChildren(transAttrs);

	// set our wall-shadow offset directly.
	m_OffsetX = dcOffset.x;
	m_OffsetY = dcOffset.y;

	// if necessary, update our render information.
	if (UpdateNodeShadow)
	{
		// we can avoid a regeneration at this point, by doing UpdateAfterWallOffset().
		// unfortunately, we do have to regenerate if one of our parents is a NodeBlend,
		// as the blended shadows must be regenerated correctly.
		Node* pBlendParent = FindParent();
		while (pBlendParent != NULL && !pBlendParent->IsABlend())
			pBlendParent = pBlendParent->FindParent();

		if (pBlendParent != NULL)
			RegenerateNode(NULL);

		else
			pShadow->UpdateAfterWallOffset();
	}
}



/********************************************************************************************

>	DocCoord NodeShadowController::GetWallShadowOffset()

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/06/2000
	Returns:	Offset of the wall shadow, in a DocCoord (millipoint units).
	Purpose:	Get the offset of the wall shadow from the shadowed object.

********************************************************************************************/
DocCoord NodeShadowController::GetWallShadowOffset()
{
	return DocCoord(m_OffsetX, m_OffsetY);
}



void NodeShadowController::PreExportRender(RenderRegion* pRegion)
{
#ifdef DO_EXPORT
PORTNOTE("epsfilter", "Removed use of EPSFilter")
#ifndef EXCLUDE_FROM_XARALX
	if (pRegion->IsKindOf(CC_RUNTIME_CLASS(EPSRenderRegion)))
	{
		// Output "start group" token
		EPSExportDC *pDC = (EPSExportDC *) pRegion->GetRenderDC();
		pDC->OutputToken("u");
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
#endif
}

BOOL NodeShadowController::ExportRender(RenderRegion* pRegion) 
{
#ifdef DO_EXPORT
PORTNOTE("epsfilter", "Removed use of EPSFilter")
#ifndef EXCLUDE_FROM_XARALX
	if (pRegion->IsKindOf(CC_RUNTIME_CLASS(EPSRenderRegion)))
	{
		// Output "end group" token
		EPSExportDC *pDC = (EPSExportDC *) pRegion->GetRenderDC();
		pDC->OutputToken("U");
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
#endif
	// Render thid node in the normal way
	return FALSE;
}

/********************************************************************************************

>	void NodeShadowController::RenderTinyBlobs(RenderRegion* pRender)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/6/94
	Inputs:		pRender - The region to draw the blobs in
	Purpose:	Renders the tiny blobs for a group (A Single blob on the topmost child object
				in the group)

********************************************************************************************/

void NodeShadowController::RenderTinyBlobs(RenderRegion* pRegion)
{
#if !defined(EXCLUDE_FROM_RALPH)
	// get the topmost object in this group and tell it to render its tiny blobs.
	Node* pNode = FindLastChild();
	while (pNode != NULL && !pNode->IsAnObject())
		pNode = pNode->FindPrevious();

	// empty groups are not allowed!
	if (pNode == NULL)
	{
		ERROR3("NodeShadowController::RenderTinyBlobs; This group is empty! Shouldn't be!");
		return;
	}
	else
	{
		((NodeRenderableInk*)pNode)->RenderTinyBlobs(pRegion);
	}

#endif
}


/********************************************************************************************
>	virtual BOOL NodeShadowController::CompareState(NodeEffect* pPPNode)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/11/2004
	Returns:	TRUE if this state matches the state (and type) of the supplied node
	Purpose:	-
	Notes:		Would be more efficient to pass in level xml instead of getting it inside
				this function

********************************************************************************************/

BOOL NodeShadowController::CompareState(NodeEffect* pPPNode)
{
	if (!pPPNode->IsAShadowController())
		return FALSE;

	NodeShadowController* pTest = (NodeShadowController*)pPPNode;

	// ?? Switch on TYPE before doing some of these tests?
	BOOL bSame = (m_OffsetX==pTest->m_OffsetX && m_OffsetY==pTest->m_OffsetY);
	bSame = bSame && (m_PenumbraWidth==pTest->m_PenumbraWidth);
	bSame = bSame && (m_sShadowName==pTest->m_sShadowName && m_ShadowType==pTest->m_ShadowType);
	bSame = bSame && (m_FloorShadowAngle==pTest->m_FloorShadowAngle && m_FloorShadowHeight==pTest->m_FloorShadowHeight);
	bSame = bSame && (m_Scale==pTest->m_Scale);
	bSame = bSame && (m_GlowWidth==pTest->m_GlowWidth);
	bSame = bSame && (m_FeatherWidth==pTest->m_FeatherWidth);

	if (bSame)
	{
		// Do the slower comparisons too
		NodeShadow* pShadow = GetShadow();
		NodeShadow* pTestShadow = pTest->GetShadow();
		bSame = bSame && (pShadow->GetBiasGain() == pTestShadow->GetBiasGain());
		bSame = bSame && (pShadow->GetDarkness() == pTestShadow->GetDarkness());
	}
	
	return (bSame);
}




/********************************************************************************************
>	virtual BOOL NodeShadowController::CompareState(NodeEffect* pPPNode, ShadowFieldMask mask)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/05/2005
	Returns:	TRUE if this state matches the state (and type) of the supplied node
	Purpose:	-
	Notes:		Would be more efficient to pass in level xml instead of getting it inside
				this function

********************************************************************************************/

BOOL NodeShadowController::CompareState(NodeEffect* pPPNode, ShadowFieldMask mask)
{
	if (!pPPNode->IsAShadowController())
		return FALSE;

	NodeShadowController* pTest = (NodeShadowController*)pPPNode;

	// ?? Switch on TYPE before doing some of these tests?
	BOOL bSame = TRUE;
	bSame = bSame && (!mask.OffsetX		|| (m_OffsetX==pTest->m_OffsetX));
	bSame = bSame && (!mask.OffsetY		|| (m_OffsetY==pTest->m_OffsetY));
	bSame = bSame && (!mask.Penumbra	|| (m_PenumbraWidth==pTest->m_PenumbraWidth));
	bSame = bSame && (!mask.Name		|| (m_sShadowName==pTest->m_sShadowName));
	bSame = bSame && (!mask.Type		|| (m_ShadowType==pTest->m_ShadowType));
	bSame = bSame && (!mask.FloorAngle	|| (m_FloorShadowAngle==pTest->m_FloorShadowAngle));
	bSame = bSame && (!mask.FloorHeight	|| (m_FloorShadowHeight==pTest->m_FloorShadowHeight));
	bSame = bSame && (!mask.Scale		|| (m_Scale==pTest->m_Scale));
	bSame = bSame && (!mask.GlowWidth	|| (m_GlowWidth==pTest->m_GlowWidth));
	bSame = bSame && (!mask.FeatherWidth	|| (m_FeatherWidth==pTest->m_FeatherWidth));

	if (bSame)
	{
		// Do the slower comparisons too
		NodeShadow* pShadow = GetShadow();
		NodeShadow* pTestShadow = pTest->GetShadow();
		bSame = bSame && (!mask.BiasGain	|| (pShadow->GetBiasGain() == pTestShadow->GetBiasGain()));
		bSame = bSame && (!mask.Darkness	|| (pShadow->GetDarkness() == pTestShadow->GetDarkness()));
	}
	
	return (bSame);
}




/********************************************************************************************

>	virtual void NodeShadowController::InvalidateBoundingRect(BOOL InvalidateChildBounds = FALSE)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/10/2005
	Inputs:		InvalidateChildBounds - When TRUE all children of the node have their
				bounds invalidated as well (eg. applying a line attribute to a group)

	Purpose:	Marks the bounding rect of this node as invalid, as well as marking this nodes
				parents as invalid as well. Next time a nodes GetBoundingRect() function is
				called the bounding rect will be recalculated if needed.
	Note:		Shadow controller overrides this because if it is being told its bounds
				are out of date that MUST mean that the shadow bounds are also out of date
				(Invalidation usually means the shadowed nodes have changed and the shadow
				ought to have new bounds in sympathy with them)

********************************************************************************************/

void NodeShadowController::InvalidateBoundingRect(BOOL InvalidateChildBounds /* = FALSE */)
{
	// Invalid this nodes bounding rect if we need to and the shadow bounding rect
	if (IsBoundingRectValid)
	{
		NodeShadow* pShadow = GetShadow();
		if (pShadow)
			pShadow->InvalidateBoundingRect(InvalidateChildBounds);
	}

	NodeEffect::InvalidateBoundingRect(InvalidateChildBounds);
}



#ifdef NEW_SHADOW_RENDER
/********************************************************************************************

>	SubtreeRenderState NodeShadowController::RenderSubtree(RenderRegion* pRender, Node** ppNextNode, BOOL bClip)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/03/2005
	Inputs:		pRender - The region render into (or use as context for decisions about subtree)
				ppNextNode - Address of node pointer for next node to render or run to after this
				bClip - flag indicating whether to clip or not
	Purpose:	Do clever stuff on the way into a subtree, possibly modifying rendering
				behaviour.

********************************************************************************************/

SubtreeRenderState NodeShadowController::RenderSubtree(RenderRegion* pRender, Node** ppNextNode, BOOL bClip)
{
	BOOL bRendered = FALSE;

	if (pRender == NULL)							// If no render region supplied, assume we need to be rendered
		return SUBTREE_ROOTANDCHILDREN;

	// If not effect attrs, work the legacy way, so that non-mix transparencies inside the shadowed
	// nodes work
	if (!IsCapturingChildren())
		return SUBTREE_ROOTANDCHILDREN;

	// Only the top ShadowController in a stack needs to capture anything...
	if (!IsTopShadowController())
		return SUBTREE_ROOTANDCHILDREN;

	// Let children render themselves directly if we're not showing filled paths so the user can
	// see their outlines
	if (pRender->RRQuality.GetFillQuality() <= Quality::NoFill)
		return SUBTREE_ROOTANDCHILDREN;

	CBitmapCache* pBitmapCache = Camelot.GetBitmapCache();
	if (pBitmapCache==NULL)
		return SUBTREE_NORENDER;

	// Find out whether we have a cached processed bitmap for this node...
	/*BOOL bFoundCached =*/ FindCachedEffect(pBitmapCache);

	// Go find out about my bounding rectangle
//	// If we don't have a cached bitmap then expand the bounding rect to ensure it will cover an expansion caused by the effect
	// If we do have a cached bitmap then use the bounding rect stored with the processed bitmap
	DocRect BoundingRect = GetBoundingRect();									// This should include the shadow's bounds!
//	if (!bFoundCached)
//		BoundingRect.Inflate((INT32)(GetPixelWidth()*150));						// Bodge to ensure LE is rendered before it grows
	DocRect ClipRect = pRender->GetClipRect();

	if (bClip && !ClipRect.IsIntersectedWith(BoundingRect))	// If not within the clipping rect then
		return SUBTREE_NORENDER;					// Don't render us or our children

	// No need to check BackmostChanged node here because we never capture
	// our background in a 24BPP bitmap we always capture at 32BPP using cfLOCKEDTRANSPARENT

	// Now ask our derived classes to render what they need to render
	bRendered = RenderCachedEffect(pBitmapCache, pRender);

	if (bRendered)
	{
TRACEUSER( "Phil", _T("NSC::RenderSubtree rendered cached @ %fdpi\n"), GetPixelsPerInch());
		return SUBTREE_NORENDER;
	}

	// Any sort of drag prevents new cacheing (to keep drags running smoothly)
// No, shadows are quick enough to be recached during dragging...
//	if (Operation::GetQuickRender(this))
//		return SUBTREE_ROOTANDCHILDREN;

	// We couldn't find or render a cached bitmap then try to cache a new one
	double PixelWidth = GetPixelWidth();

	// Work out how much of the object we propose to capture
	// (This may change during the Capture if we have to fall back to 24BPP opaque capture)
	DocRect viewrect = ClipRect;
	DocRect CaptureRect = GetChildBoundingRect();			// Make sure we get our child bound not result of processing!

	// Inflate by one pixel all round to allow for anti-aliasing effects at the edges
//		CaptureRect.Inflate((INT32)PixelWidth);

	// Only cache if it's worth it!
	if (CaptureRect.Width()>=PixelWidth || CaptureRect.Height()>=PixelWidth)
	{
TRACEUSER( "Phil", _T("NSC::RenderSubtree Capturing cached @ %fdpi\n"), GetPixelsPerInch());
		CaptureFlags caFlags = CaptureFlags(cfLOCKEDTRANSPARENT | cfFULLCOVERAGE);
		pRender->StartCapture(this, CaptureRect, CAPTUREINFO(ctNESTABLE, caFlags), TRUE, FALSE, GetPixelsPerInch());
	}

	return bRendered ? SUBTREE_NORENDER : SUBTREE_ROOTANDCHILDREN;					// Else we must render ourselves and our children
}




/********************************************************************************************

>	void NodeShadowController::RenderAfterSubtree(RenderRegion* pRender)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/09/2004
	Inputs:		pRender - The region to render into
	Purpose:	Capture the group as a cached bitmap

********************************************************************************************/

void NodeShadowController::RenderAfterSubtree(RenderRegion* pRender)
{
	Capture* pCapture = pRender->GetTopCapture();
	if (pCapture==NULL)											// If nothing was captured
		return;													// Then do nothing

	CBitmapCache* pBitmapCache = Camelot.GetBitmapCache();
	if (pBitmapCache==NULL)
	{
		return;
	}

	// Only stop the capture if we started it
	// (means we only capture whole subtrees at the mo.)
	if (pCapture->GetOwner()==this)
	{
		// End this capture:
		// Blit capture to screen
		// Retain bitmap because we will release it ourselves onyl if we fail to cache it
		BOOL bCached = FALSE;
		LPBITMAPINFO lpInfo = NULL;
		LPBYTE lpBits = NULL;
		DocRect CaptureRect = GetChildBoundingRect();				// Set maximum size we allow
		pRender->StopCapture(this, FALSE, FALSE, &lpInfo, &lpBits, &CaptureRect);

		// If the capture gave us back a bitmap, try to cache it
		if (lpInfo && lpBits && CaptureRect.IsValid())
		{
			// Cache the ORIGINAL bitmap as Option 0
			// See also, SetOriginalBitmap
			double PixelWidth = GetPixelWidth();
			CBitmapCacheKey inky(this, PixelWidth, 0);

			CCachedBitmap cbmp;

			cbmp.pbmpBits = lpBits;
			cbmp.pbmpInfo = lpInfo;
			cbmp.SetCachedRect(CaptureRect);
			cbmp.nPriority = CACHEPRIORITY_TEMPBITMAP_HIGH;

			if (cbmp.IsValid())
			{
				pBitmapCache->StoreBitmap(inky, cbmp);
				bCached = TRUE;
			}

			RenderCachedEffect(pBitmapCache, pRender);

		}

		// If we failed to cache the captured bitmap then release it
		if (lpInfo!=NULL && lpBits!=NULL && !bCached)
		{
			FreeDIB(lpInfo, lpBits, NULL, FALSE);
		}
	}
}




/***********************************************************************************************

>	virtual void NodeShadowController::Render(RenderRegion* pRRegion)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/2004
	Inputs:		Pointer to a render region
	Purpose:	Will render the liveeffect iff we have it cached - for hit detection reasons

***********************************************************************************************/

// NOTE: No need for a render function because this effect does not add any pixel data
// to those rendered by its children - just let the children render themselves into
// the hit-detection bitmap
//void NodeShadowController::Render(RenderRegion* pRender)
//{
//	CBitmapCache* pBitmapCache = Camelot.GetBitmapCache();
//	if (pBitmapCache!=NULL && pRender->IsHitDetect() && IsCapturingChildren())
//	{
//		RenderCachedEffect(pBitmapCache, pRender);
//	}
//}




/********************************************************************************************

>	virtual BOOL NodeShadowController::FindCachedEffect(CBitmapCache* pBitmapCache)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/11/2004
	Inputs:		-
	Outputs:	-
	Purpose:	Find all cached items this node needs to call RenderCachedEffect
				succesfully

********************************************************************************************/

BOOL NodeShadowController::FindCachedEffect(CBitmapCache* pBitmapCache)
{
//#pragma message( __LOCMSG__ "Removed CBitmapCache usage" )
//	return false;
	CBitmapCacheKey inky(this, GetPixelWidth(), 0);						// Get cached BMP for this ORIGINAL node at our dpi
	CCachedBitmap cbmp;
	BOOL bFoundCached = pBitmapCache->Lookup(inky, cbmp);

	return bFoundCached;
}




/********************************************************************************************

>	BOOL NodeShadowController::RenderCachedEffect(CBitmapCache* pBitmapCache, RenderRegion* pRender)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/11/2004
	Inputs:		-
	Outputs:	-
	Purpose:	Render the cached version of this node
	Notes:		When several shadow effects are applied on top of each other, it is desirable
				to avoid showing shadows of shadows - that is unrealistic.
				To do that, only the top ShadowController does any rendering, those underneath
				suspend rendering and rely on the top ShadowController to build and render
				their Shadow nodes for them.
	SeeAlso:	NodeShadowController::RenderSubtree

********************************************************************************************/

BOOL NodeShadowController::RenderCachedEffect(CBitmapCache* pBitmapCache, RenderRegion* pRender)
{
	BOOL bRendered = FALSE;

	CBitmapCacheKey inky(this, GetPixelWidth(), 0);						// Get cached BMP for this ORIGINAL node at our dpi
	CCachedBitmap cbmp;
	BOOL bFoundCached = pBitmapCache->Lookup(inky, cbmp);

	// Render the Shadow first...
	NodeShadow* pShadow = GetShadow();
	if (pShadow && bFoundCached)
	{
		double PixelWidth = GetPixelWidth();

		// This is clever...
		// Only render shadows if this is the top-most controller in the stack.
		// In that case, go down the stack and render all the shadows that have not yet been
		// rendered.
		// This means that shadows do not generate shadows!
		if (this->IsTopShadowController())
		{
			LPBITMAPINFO lpInfo = NULL;
			LPBYTE lpBits = NULL;
			DocRect CaptureRect;

			pShadow = FindBottomShadow();
			while (pShadow)
			{
				if (bFoundCached && pShadow->NeedsRemake())
				{
					// Now that we know one of our shadows needs to be regenerated
					// We can decide whether we need to create a "silhouette" bitmap for it 
					if (lpInfo==NULL || lpBits==NULL)
					{
						// Grab this bitmap and its effect attribute into a new bitmap by rendering them into a capture
						// This combined new bitmap will be used as the silhouette bitmap to generate ALL shadows
						CaptureFlags caFlags = CaptureFlags(cfLOCKEDTRANSPARENT | cfPIXWIDTHSCALE);
						CaptureRect = cbmp.GetCachedRect();
						pRender->StartCapture(this, CaptureRect, CAPTUREINFO(ctNESTABLE, caFlags), TRUE, FALSE, 72000.0/PixelWidth);
						pRender->RenderBits(cbmp.pbmpInfo, cbmp.pbmpBits, &cbmp.coord0, 3, TRUE, this);
						pRender->StopCapture(this, FALSE, FALSE, &lpInfo, &lpBits, &CaptureRect);
					}

					if (lpBits && lpInfo)
						pShadow->MakeShadow(lpInfo, lpBits, CaptureRect);
				}
	
				// Render this shadow
				pShadow->RenderShadow(pRender);

				pShadow = FindShadowAbove(pShadow);
			}

			// If we had to make a silhouette bitmap, get rid of it now
			if (lpInfo && lpBits)
				::FreeDIB(lpInfo, lpBits);
		}
	}

	// Render any effect attributes that are attached to us
	// along with the bitmap so that they affect its appearance
	if (bFoundCached)
	{
		bRendered = pRender->RenderBits(cbmp.pbmpInfo, cbmp.pbmpBits, &cbmp.coord0, 3, TRUE, this);
	}

	return bRendered;
}

BOOL NodeShadowController::IsTopShadowController()
{
	Node* pParent = FindParent();
	return (pParent==NULL || !pParent->IsAShadowController());
}

BOOL NodeShadowController::IsInShadowStack()
{
	Node* pNode = FindParent();
	if (pNode && pNode->IsAShadowController())
		return TRUE;

	pNode = GetInkNodeFromController();
	if (pNode && pNode->IsAShadowController())
		return TRUE;

	return FALSE;
}

NodeShadowController* NodeShadowController::FindBottomShadowController()
{
	Node* pChild = GetInkNodeFromController();
	NodeShadowController* pBottomController = this;

	while (pChild && pChild->IsAShadowController())
	{
		pBottomController = (NodeShadowController*)pChild;
		pChild = pBottomController->GetInkNodeFromController();
	}

	return pBottomController;
}

NodeShadow* NodeShadowController::FindBottomShadow()
{
	NodeShadowController* pBottomController = this;
	pBottomController = FindBottomShadowController();

	if (pBottomController)
		return pBottomController->GetShadow();
	else
		return NULL;
}

NodeShadow* NodeShadowController::FindShadowAbove(NodeShadow* pCurrentShadow)
{
	Node* pController = pCurrentShadow->GetParentController();
	pController = pController->FindParent();
	if (pController==NULL || !pController->IsAShadowController())
		return NULL;
	else
		return ((NodeShadowController*)pController)->GetShadow();
}

void NodeShadowController::ReleaseStackedShadows()
{
	NodeShadow* pShadow = FindBottomShadow();
	while (pShadow)
	{
		pShadow->ReleaseCached(FALSE, TRUE, TRUE, TRUE);

		pShadow = FindShadowAbove(pShadow);
	}
}




/********************************************************************************************

>	virtual double NodeShadowController::GetPixelsPerInch()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/09/2004
	Inputs:		- 
	Outputs:	Pixel resolution for this live effect bitmap in pixels per inch (double)
	Retuns:		Description of the group node 
	Purpose:	To get the required/stored pixel res of this node
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

double NodeShadowController::GetPixelsPerInch()
//{
//	// During dragging, reduce resolution to speed up rendering
//	if (IsDragged())
//		return 96.0;
//
//	return (double)NodeShadowController::DefaultPixelsPerInch;
//}
{
//	if (IsDragged())
//		return 96.0;
//
	double dPPI = 0.0;

	if (NodeBitmapEffect::ShadowPixelsPerInch==0)
	{
		// 0 means automatic
		// Automatic means get resolution in context
		// Context means either respond to a fixed resolution above us in the tree
		// or get resolution from current view
		View* pView = View::GetCurrent();
		double dViewPPI = 96.0;
		if (pView)
			dViewPPI = 72000.0 / pView->GetPixelWidth().MakeDouble();

		Node* pParent = this->FindParent();
		while (pParent && !pParent->IsLayer())
		{
			if (pParent->IsBitmapEffect())
			{
				dPPI = ((NodeBitmapEffect*)pParent)->GetPixelsPerInchValue();
				if (dPPI!=0)
					break;

				// If we found an effect at all then ensure we use default effect PPI
				dPPI = dViewPPI;
			}

			pParent = pParent->FindParent();
		}

		// If we found no effects above us then try to operate at zoomed view res
		// for best results on screen.
		if (dPPI==0)
		{
			// See also OpEffectLock::DoEffectNodeOp
			// We should capture at at least 200dpi for printing
			// We should capture at a size that's at least as good as the current zoomed pixel size
			// (We rely on the capture being clipped to keep bitmap sizes under control at high zoom factors)
			// (We should reduce the resolution if the bitmap will be huge...)
			if (pView)
			{
				dPPI = 72000.0 / pView->GetPixelWidth().MakeDouble();
				double dScaledPPI = 72000.0 / pView->GetScaledPixelWidth().MakeDouble();

				// Cap zoomed resolution at 4 * view's native resolution
				if (dScaledPPI > dPPI*4.0)
					dScaledPPI = dPPI*4.0;

				// If zoomed resolution > native resolution then use zoomed else use native
				if (dPPI < dScaledPPI)
					dPPI = dScaledPPI;
			}
		}
	}

	if (dPPI==0)
		dPPI = NodeBitmapEffect::ShadowPixelsPerInch;

	if (dPPI==0)
	{
		// Shouldn't ever reach this clause but just in case...
		ERROR3("GetPixelsPerInch can't get sensible PPI so defaulting to 96");
		dPPI = 96.0;
	}

	return dPPI;
};




/********************************************************************************************

>	virtual double NodeShadowController::GetPixelWidth()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/09/2004
	Inputs:		- 
	Outputs:	Pixel width for live effect bitmap in millipoints (double)
	Retuns:		Description of the group node 
	Purpose:	To get the required/stored pixel width of this node
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

double NodeShadowController::GetPixelWidth()
{
	// During dragging, reduce resolution to speed up rendering
	return 72000.0 / GetPixelsPerInch();
}




/********************************************************************************************

>	virtual BOOL NodeShadowController::ReleaseCached(BOOL bAndParents = TRUE,
													 BOOL bAndChildren = TRUE,
													 BOOL bSelf = TRUE,
													 BOOL bAndDerived = TRUE)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/03/2005
	Returns:	TRUE if captured data was cached
	Purpose:	Protected Helper function
				Use the CaptureManager to capture the results of rendering, cache them
				and associate them with this Ink node
	SeeAlso:	NodeRenderableInk::RenderCached, CaptureCached

********************************************************************************************/

BOOL NodeShadowController::ReleaseCached(BOOL bAndParents, BOOL bAndChildren, BOOL bSelf, BOOL bAndDerived)
{
	if (!IsCapturingChildren())
	{
		NodeShadow * pShadow = (NodeShadow *)FindFirstChild(CC_RUNTIME_CLASS(NodeShadow));
		if (pShadow && bAndDerived && !IsDragged())
		{
			pShadow->DeleteCache();
		}

		return NodeEffect::ReleaseCached(bAndParents, bAndChildren, bSelf, bAndDerived);
	}

	// Don't release cached data if the release request is the result of transformation
	// re-rendering because we can transform our cached data!
	if (!IsDragged())
	{
		CBitmapCache* pBitmapCache = Camelot.GetBitmapCache();
		if (pBitmapCache!=NULL && bSelf)
		{
			CBitmapCacheKey inky(this, 42);
			pBitmapCache->RemoveAllOwnedBitmaps(inky, FALSE, CACHEPRIORITY_PERMANENT);
		}

		// Forcibly remove my Shadow child's cached data as well
		// because he is really just an offshoot of me and has no children of his own
		// so he won't normally be released unless I do it.
		if (bAndDerived)
		{
			ReleaseStackedShadows();
//			NodeShadow* pShadow = this->GetShadow();
//			if (pShadow)
//			{
//				pShadow->ReleaseCached(FALSE, TRUE, TRUE, TRUE);
//			}
		}
	}

	// If we should release our children's caches as well...
	if (bAndChildren)
	{
		Node* pChild = FindFirstChild();
		while (pChild)
		{
			if (pChild->IsBounded())
				((NodeRenderableBounded*)pChild)->ReleaseCached(FALSE, TRUE, TRUE, TRUE);

			pChild = pChild->FindNext();
		}
	}

	// If I can't be cached, neither can my parent...
	if (bAndParents && FindParent() && FindParent()->IsBounded())
		((NodeRenderableBounded*)FindParent())->ReleaseCached(TRUE, FALSE, TRUE, TRUE);

	return TRUE;
}




/********************************************************************************************

>	virtual BOOL NodeShadowController::IsCapturingChildren()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/03/2005
	Inputs:		- 
	Outputs:	-
	Retuns:		TRUE if this shadow works by capturing its children into a bitmap
					 (required if the shadow has effect attributes applied to it)
				FALSE if not (typcially when the children use non-mix transparency)
	Purpose:	To determine the rendering mode of this shadow
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL NodeShadowController::IsCapturingChildren()
{
	// TODO: Factor in non-mix transparency test in here?
	// Shadows should be faster & more controlled if they use the capture system
	return HasEffectAttrs() || IsInShadowStack();
}




/********************************************************************************************

>	virtual DocRect NodeShadowController::GetChildBoundingRect(BOOL bIncludeAttrs = TRUE)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/03/2005
	Returns:	The bounding box of the children of the live effect
	Purpose:	Get the bounding rect of the children without updating BOundingRectangle
				or IsBoundingRectValid for this node...
				Ignore shadows if this is topshadow node and child is a shadowcontroller

********************************************************************************************/

DocRect NodeShadowController::GetChildBoundingRect(BOOL bIncludeAttrs)
{
	NodeShadowController* pController = FindBottomShadowController();
	return pController->GetShadowedNode()->GetBoundingRect(!bIncludeAttrs);
	
/*	// just set it to be an empty rectangle
	DocRect BoundRect(0,0,0,0);

	Node* pNode = FindFirstChild();
	while (pNode!=NULL)
	{
		// Add in the bounding rect of this node with all the others
		if (pNode->IsBounded())
			BoundRect = BoundRect.Union(((NodeRenderableBounded*)pNode)->GetBoundingRect(!bIncludeAttrs));

		// And find the next node
		pNode = pNode->FindNext();
	}

	return BoundRect;
*/
}




#endif




////////////////////////////////////////////////////////////////////////////////////////////
// class ShadowNodeTreeFactory: public CCObject
// implementation
/********************************************************************************************

>	ShadowNodeTreeFactory::ShadowNodeTreeFactory()

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/8/99
	Inputs:		
	Returns:	
	Purpose:	Constructor

********************************************************************************************/
ShadowNodeTreeFactory::ShadowNodeTreeFactory()
{
	m_ShadowType		=	SHADOWTYPE_NONE;
	m_OffsetX			=	0;
	m_OffsetY			=	0;
	m_PenumbraWidth		=	0;
	m_FloorShadowAngle	=	0;
	m_FloorShadowHeight	=	0;
	m_Scale				=	0;
	m_GlowWidth			=	0;
	m_FeatherWidth			=	0;
}

/********************************************************************************************

>	ShadowNodeTreeFactory::~ShadowNodeTreeFactory()

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/8/99
	Inputs:		
	Returns:	
	Purpose:	Destructor

********************************************************************************************/
ShadowNodeTreeFactory::~ShadowNodeTreeFactory()
{
	NodeListItem * pItem = (NodeListItem *)m_AttributeList.GetHead();

	while (pItem)
	{
		delete pItem->pNode;
		pItem->pNode = NULL;

		pItem = (NodeListItem *)m_AttributeList.GetNext(pItem);
	}
	
	m_AttributeList.DeleteAll();
}

/********************************************************************************************

>	void ShadowNodeTreeFactory::SetUpAttributeList(Node * pShadow);

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/8/99
	Inputs:		The node to copy the attribute map from
	Returns:	
	Purpose:	Sets up the attribute map for the appropriate node

********************************************************************************************/
void ShadowNodeTreeFactory::SetUpAttributeList(Node * pShadow)
{
	NodeListItem * pItem = NULL;

	// first, set up the attribute map
	CCAttrMap *pMap = CCAttrMap::MakeAppliedAttrMap((NodeRenderableInk *)pShadow);

	CCAttrMap::iterator pos = pMap->GetStartPosition();

	NodeAttribute * pAttrNode = NULL;
	
	// run through the map, creating copies of the attributes and adding them to the list
	while( pos != pMap->GetEndPosition() )
	{
		CCRuntimeClass *pKey;
		void		   *pVal;
		pMap->GetNextAssoc(pos, pKey, pVal);

		pAttrNode = ((NodeAttribute *)pVal);

		pItem = new NodeListItem;

		if (!pItem)
		{
			ERROR3("Can't create NodeListItem");
			return;
		}

		pAttrNode->NodeCopy(&(pItem->pNode));

		m_AttributeList.AddTail(pItem);
	}

	delete pMap;
	pMap = NULL;
}

/********************************************************************************************

>	NodeCompound *ShadowNodeTreeFactory::CreateNode(List *pList, UndoableOperation * pOp = NULL)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/99
	Inputs:		The list of nodes to shadow, and an undoable operation (if necessary for 
				undo to work)
	Returns:	The compound node created (NULL for failure)
	Purpose:	Given the list of nodes, shadows them with my parameters

********************************************************************************************/
NodeCompound * ShadowNodeTreeFactory::CreateNode(List *pList, UndoableOperation * pOp /* = NULL*/)
{
	NodeShadowController * pControl = NULL;
	NodeShadow			 * pShadow	= NULL;

	NodeListItem * pItem = NULL;
	NodeListItem * pAttrItem = NULL;

//	Node * pInsertNode = NULL;

	BOOL ok = TRUE;
	
	if (pOp)
	{
		// first, create the shadow controller node and the shadow node
		ALLOC_WITH_FAIL(pControl, (new NodeShadowController), pOp);
		ALLOC_WITH_FAIL(pShadow, (new NodeShadow), pOp);

		// move the nodes in the list to be children of the shadow controller node
		pItem = (NodeListItem *)pList->GetHead();

		// insert the controller into the tree
		ok = pOp->DoInsertNewNode(pControl, (Node *)pItem->pNode, NEXT, TRUE);

		while (pItem && ok)
		{
			ok = pOp->DoMoveNode(pItem->pNode, pControl, LASTCHILD);

			pItem = (NodeListItem *)pList->GetNext(pItem);
		}

		// set up the fill attributes for the shadow node
		pAttrItem = (NodeListItem *)m_AttributeList.GetHead();

		while (pAttrItem)
		{
			if (((NodeAttribute *)pAttrItem->pNode)->CanBeAppliedToObject())
			{
				pAttrItem->pNode->CopyNode(pShadow, LASTCHILD);
			}

			pAttrItem = (NodeListItem *)m_AttributeList.GetNext(pAttrItem);
		}		

		// now all nodes are moved, insert the shadow node
		pOp->DoInsertNewNode(pShadow, pControl, FIRSTCHILD, TRUE);

		// set all the variables on the shadow node
		pControl->SetPenumbraWidth(this->m_PenumbraWidth);
		pControl->SetOffsetX(m_OffsetX);
		pControl->SetOffsetY(m_OffsetY);
		pControl->SetGlowWidth(m_GlowWidth);
		pControl->SetFeatherWidth(m_FeatherWidth);
		pControl->SetFloorShadowAngle(m_FloorShadowAngle);
		pControl->SetFloorShadowHeight(m_FloorShadowHeight);
		pControl->SetWallShadowScale(m_Scale);
		pControl->SetShadowType(m_ShadowType);

		// create shadow bitmaps & data
		pControl->RegenerateNode(NULL, FALSE);

		pOp->DoInvalidateNodeRegion(pControl, TRUE);
	}

	if (!ok)
	{
		return NULL;
	}
	
	return pControl;
}

#endif	// BUILDSHADOWS
