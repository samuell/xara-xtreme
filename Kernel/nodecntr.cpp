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

#include "camtypes.h"
#include "nodecomp.h"
#include "group.h"

// Save/load
#include "cxfrec.h"		// CXaraFileRecord

#include "cxftags.h"

#include "nodecntr.h"
#include "ncntrcnt.h"
//#include "cntres.h"
#include "gdraw.h"
#include "gclips.h"
#include "fillval.h"
#include "lineattr.h"
#include "blobs.h"
#include "objchge.h"
//#include "opbevel.h"
#include "ophist.h"
#include "nodeblnd.h"
#include "ndbldpth.h"
#include "csrstack.h"
#include "ppbevel.h"
//#include "opcntr.h"
#include "nodetxts.h"
//#include "moldtool.h"
#include "extender.h"
//#include "mario.h"
#include "rsmooth.h"
#include "fitcurve.h"
//#include "contmenu.h"
#include "blndhelp.h"
#include "brshattr.h"
#include "pbecomea.h"
#include "ppstroke.h"

#include "gclip.h"

//#include "ngcore.h"		// NameGallery, for stretching functionality

CC_IMPLEMENT_DYNCREATE(NodeContour, NodeRenderableInk)

// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW

#define NEWVERSION

/**********************************************************************************************/

const UINT32 ClipPathToPath_Tolerance   =  30 ;
const UINT32 ClipPathToPath_Flatness    = 200 ;
const UINT32 StrokePathToPath_Flatness  = 200 ;

/***********************************************************************************************

>	NodeContour::NodeContour()

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/8/99
	Purpose:	Constructor

***********************************************************************************************/

NodeContour::NodeContour()
{
	m_pPathList = NULL;
	m_NumPaths = 0;
	m_SourcePath.Initialise();

	IsBoundingRectValid = FALSE;
	m_FirstRender = TRUE;
}

/***********************************************************************************************

>	NodeContour::NodeContour(Node* ContextNode,  
				AttachNodeDirection Direction,  
				BOOL Locked = FALSE, 
				BOOL Mangled = FALSE,  
				BOOL Marked = FALSE, 
				BOOL Selected = FALSE    
				) ;


	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/8/99
	Purpose:	Constructor

***********************************************************************************************/
NodeContour::NodeContour(Node* ContextNode,  
				AttachNodeDirection Direction,  
				BOOL Locked, 
				BOOL Mangled,  
				BOOL Marked, 
				BOOL Selected    
				) 
{
	m_pPathList = NULL;
	m_NumPaths = 0;
	m_SourcePath.Initialise();
	IsBoundingRectValid = FALSE;
	m_FirstRender = TRUE;
}

/***********************************************************************************************

>	NodeContour::~NodeContour()

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/8/99
	Purpose:	Destructor

***********************************************************************************************/

NodeContour::~NodeContour()
{
	if (m_pPathList)
	{
		delete [] m_pPathList;
		m_pPathList = NULL;
	}
}

/***********************************************************************************************

>	DocRect NodeContour::GetBoundingRect(BOOL DontUseAttrs = FALSE, BOOL HitTest = FALSE)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/8/99
	Inputs:		See base class
	Purpose:	Get the bounding rect of this node

***********************************************************************************************/
DocRect NodeContour::GetBoundingRect(BOOL DontUseAttrs, BOOL HitTest)
{
	// sum all the bounding rects & then add the width
	if (!(!IsBoundingRectValid || DontUseAttrs))
	{	
		return BoundingRectangle;
	}	

	DocRect dr;

	NodeContourController * pController = (NodeContourController *)FindParent();

	if (!pController)
		return DocRect(0,0,0,0);

	INT32 Width = pController->GetWidth();

	Node * pNode = FindParent();

	if (!pNode)
		return BoundingRectangle;

	pNode = pNode->FindFirstChild();
	
	while (pNode && pNode != this)
	{
		if (pNode->IsAnObject())
		{
			if (!IsBoundingRectValid)
			{
				((NodeRenderableInk *)pNode)->InvalidateBoundingRect();
			}
			
			dr = dr.Union(((NodeRenderableInk *)pNode)->GetBoundingRect(TRUE, HitTest));
		}
		
		pNode = pNode->FindNext();
	}
	
	if (Width < 0)
	{
		dr.lo.x += Width;
		dr.lo.y += Width;
		dr.hi.x -= Width;
		dr.hi.y -= Width;
	}

	DocRect PathRect;

	// include all the paths in the bounding rect
	for (INT32 i = 0 ; i < m_NumPaths && m_pPathList; i++)
	{
		PathRect = m_pPathList[i].GetBoundingRect();

		if (PathRect.IsValid() && !PathRect.IsEmpty())
		{
			dr = dr.Union(PathRect);
		}
	}

	// take the bounding rect from the paths, if they're present
	if (Width < 0)
	{
		if (m_NumPaths != 0)
		{
			dr = (m_pPathList[m_NumPaths-1].GetBoundingRect());
		}
	}
	else
	{
		if (m_NumPaths != 0)
		{
			dr = (m_pPathList[0].GetBoundingRect());
		}
	}

	if (!dr.IsValid() || dr.IsEmpty())
	{
		dr = ((NodeCompound *)FindParent())->GetInsideBoundingRect();

		// find the centre point
		DocCoord Centre = dr.Centre();

		dr.lo.x = Centre.x - 5;
		dr.hi.x = Centre.x + 5;
		dr.lo.y = Centre.y - 5;
		dr.hi.y = Centre.y + 5;
	}
	
	// find out the line width and adjust the bounding rectangle appropriately
	AttrLineWidth * pAttr = NULL;
	FindAppliedAttribute(CC_RUNTIME_CLASS(AttrLineWidth), (NodeAttribute **)(&pAttr));

	AttrStrokeTransp * pStrokeAttr = NULL;
	FindAppliedAttribute(CC_RUNTIME_CLASS(AttrStrokeTransp), (NodeAttribute **)(&pStrokeAttr));

	AttrStrokeColour * pStrokeColour = NULL;
	FindAppliedAttribute(CC_RUNTIME_CLASS(AttrStrokeColour), (NodeAttribute **)(&pStrokeColour));

	BOOL bTransparent = FALSE;

	if (pStrokeAttr)
	{
		if (pStrokeAttr->Value.Transp == 255)
		{
			bTransparent = TRUE;
		}
	}

	if (pStrokeColour)
	{
		if (pStrokeColour->Value.Colour == DocColour(COLOUR_NONE))
		{
			bTransparent = TRUE;
		}
	}

	if (pAttr && !bTransparent)
	{
		MILLIPOINT LineWidth = pAttr->Value.LineWidth / 2;

		dr.lo.x -= LineWidth;
		dr.lo.y -= LineWidth;
		dr.hi.x += LineWidth;
		dr.hi.y += LineWidth;
	}

	dr.Union(pController->GetInsideBoundingRect());

	IsBoundingRectValid = TRUE;
	BoundingRectangle = dr;

	return dr;
}

/***********************************************************************************************

>	DocRect NodeContour::GetBlobBoundingRect()

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/8/99
	Inputs:		See base class
	Purpose:	Get the blob bounding rect of this node

***********************************************************************************************/
DocRect NodeContour::GetBlobBoundingRect()
{
	DocRect dr = GetBoundingRect(FALSE, FALSE);

	BlobManager * pMgr = GetApplication()->GetBlobManager();

	INT32 Size = 0;

	if (pMgr)
		Size = pMgr->GetBlobSize();

	dr.lo.x -= Size*2;
	dr.lo.y -= Size*2;
	dr.hi.x += Size*2;
	dr.hi.y += Size*2;

	IncludeChildrensBoundingRects(&dr);

	return dr;
}

/***********************************************************************************************

>	void NodeContour::Render(RenderRegion* pRender)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/8/99
	Inputs:		See base class
	Purpose:	Renders this node

***********************************************************************************************/
void NodeContour::Render(RenderRegion* pRender)
{
	double Ratio = 0;
	double StepSize = 0;

	NodeContourController * pControl = (NodeContourController *)FindParent();

	// check we have something to render !
	if (pControl)
	{
		pControl->DisableInsetPathPathProcessor();

		if (pControl->GetInsideBoundingRect().IsEmpty())
			return;
	}
	else
		return;

	if(m_FirstRender)
	{
		GenerateContour();
		m_FirstRender = FALSE;
	}

	// get the first path processor
	ContourNodePathProcessor * pProc = (ContourNodePathProcessor *)pRender->GetFirstPathProcessor();

	if (pProc)
	{
		if (!((PathProcessor *)pProc)->IS_KIND_OF(ContourNodePathProcessor))
			pProc = NULL;
	}

	// do we have anything to render ??
	if (m_NumPaths == 0)
		return;

	pRender->SaveContext();

	// find out the line width
	LineWidthAttribute * pLineWidth = (LineWidthAttribute *)pRender->GetCurrentAttribute(ATTR_LINEWIDTH);

	TRACEUSER( "DaivdM", _T("Contour node render 1\n"));

	MILLIPOINT LineWidth = 0;

	if (pLineWidth)
		LineWidth = pLineWidth->LineWidth;

	if (m_pPathList != NULL && m_NumPaths > 0)
	{
		// do the blended attributes map
		CCAttrMap BlendAttrMap(30);	

		// build the attribute maps for the start & end nodes
		NodeRenderableInk * pEndNode = NULL;
		
		if (pControl->GetWidth() > 0)
			pEndNode = (NodeRenderableInk *)FindPrevious(CC_RUNTIME_CLASS(NodeRenderableInk));
		else
			pEndNode = (NodeRenderableInk *)FindNext(CC_RUNTIME_CLASS(NodeRenderableInk));

		if (!pEndNode)
			return;

		if (pEndNode->IsCompound())
			pEndNode = FindNodeToContour(pEndNode);

		if (pEndNode->IS_KIND_OF(NodeBlend))
			pEndNode = (NodeRenderableInk *)pEndNode->FindFirstChild(CC_RUNTIME_CLASS(NodeRenderableInk));

		// do the blend of the attributes
		CCAttrMap * pAttrMapStart = CCAttrMap::MakeAttrMapFromRenderRegion(pRender);
		WindingRuleAttribute * pWinding = NULL;

		if(!pAttrMapStart)
		{
			ENSURE(FALSE,"Not enough mem to create start attr map for rendering NodeContour.");
			return;
		}
		CCAttrMap * pAttrMapEnd   = CCAttrMap::MakeAppliedAttrMap(pEndNode);

		StepSize = 1.0 / m_NumPaths;
		
		for (INT32 i = m_NumPaths - 1; i >= 0; i--)
		{
			TRACEUSER( "Diccon", _T("Rendering contour path %d\n"), i);
			if (pControl->GetWidth() < 0)
			{
				// outer contour
				TRACEUSER( "DaivdM", _T("Contour node render %d\n"), i);
	
				if (i == m_NumPaths - 1)
				{
					if (pProc)
						pProc->SetActive(FALSE);
					
					pRender->DrawPath(&(m_pPathList[i]));
	
				}
				else
				{
					if (pProc)
						pProc->SetActive(TRUE);
					
					Ratio = (StepSize * i) + StepSize;

					// calculate the correct ratio given the profile for attributes
					CProfileBiasGain Profile = pControl->GetAttrProfile();
					Profile.SetBias(-Profile.GetBias());
					Ratio = (double)Profile.MapZeroToOne((AFp)Ratio);
					
					Ratio = 1.0 - Ratio;
					
					if (!BlendAttributes(pRender, pAttrMapStart, pAttrMapEnd, &BlendAttrMap, 
						Ratio))
					{
						ERROR3("Blend attributes failed");
	
						// pop the path processor first
						pRender->PopPathProcessor();						
						return;

					}

					// Ilan 5/5/00
					// Call the PostDynCreateInit functions for the attributes which require this
					// NB attrs which don't succeed in init'ing will mark themselves so that they don't NeedToRender()
					// ie the AttrMap can safely be rendered - the united attrs simply won't do their job
					BlendAttrMap.PostBlendInit(&(m_pPathList[i]), GetRuntimeClass());

					pRender->SaveContext();
					BlendAttrMap.Render(pRender);

					// Quality Max(Quality::QualityDefault);
					// QualityAttribute * pAttr = new QualityAttribute(Max);
					// pRender->SetQuality(pAttr, FALSE);	
					
					PathProcessor* pPP = pRender->GetFirstPathProcessor();
					if (pPP && pPP->IsAPathProcessorStroke())
						pPP->SetDisabled(TRUE);

					pRender->DrawPath(&(m_pPathList[i]));
					
					if (pPP)
						pPP->SetDisabled(FALSE);

					pRender->RestoreContext();

					// Ilan 5/5/00
					// Deinitialise the attrs which allocated memory in the PostBlendInit() fn call
					BlendAttrMap.PostBlendDeinit();
				}
			}
			else
			{
				if (pProc)
					pProc->SetActive(FALSE);
					
				Ratio = StepSize * i;
				
				// calculate the correct ratio given the profile for attributes
				CProfileBiasGain Profile = pControl->GetAttrProfile();
				Profile.SetBias(-Profile.GetBias());
				Ratio = (double)Profile.MapZeroToOne((AFp)Ratio);
				
				if (Ratio > 0 && Ratio < 1.0)
				{
					if (!BlendAttributes(pRender, pAttrMapStart, pAttrMapEnd, &BlendAttrMap, 
						Ratio))
					{
						ERROR3("Blend attributes failed");
						// pop the path processor first
						pRender->PopPathProcessor();				
						return;
					}
					pRender->SaveContext();
					// Ilan 5/5/00
					// Call the PostDynCreateInit functions for the attributes which require this
					// NB attrs which don't succeed in init'ing will mark themselves so that they don't NeedToRender()
					// ie the AttrMap can safely be rendered - the united attrs simply won't do their job
					BlendAttrMap.PostBlendInit(&(m_pPathList[(m_NumPaths-1) - i]), GetRuntimeClass());

					BlendAttrMap.Render(pRender);
				}
				else if (Ratio == 0)
				{
					pRender->SaveContext();
					pAttrMapStart->Render(pRender);
				}
				else
				{
					pRender->SaveContext();
					pAttrMapEnd->Render(pRender);
				}
						
				// set the winding rule
				pWinding = (WindingRuleAttribute *)pRender->GetCurrentAttribute(ATTR_WINDINGRULE);
				
				pWinding->WindingRule = NegativeWinding;
			
				pRender->DrawPath(&(m_pPathList[(m_NumPaths-1) - i]));
				pRender->RestoreContext();

				if (Ratio > 0 && Ratio < 1.0)
				{
					// Ilan 5/5/00
					// Deinitialise the attrs which allocated memory in the PostBlendInit() fn call
					BlendAttrMap.PostBlendDeinit();
				}
			}
		}

		pAttrMapStart->DeleteAttributes();
		delete pAttrMapStart;
		delete pAttrMapEnd;
		
		BlendAttrMap.DeleteAttributes();
	}
	
	pRender->RestoreContext();

	// pop the path processor
	if (pProc)
		pRender->PopPathProcessor();						
	

	pControl->EnableInsetPathPathProcessor();
}

/***********************************************************************************************

>	NodeRenderableInk * NodeContour::FindNodeToContour(NodeRenderableInk * pInk)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/3/2000
	Inputs:		The node to start searching from
	Purpose:	Does a depth first seach to find a node to take the attribute
				map from when blending

***********************************************************************************************/
NodeRenderableInk * NodeContour::FindNodeToContour(NodeRenderableInk * pInk)
{
	if (!pInk->IsCompound())
		return pInk;

	Node * pNodeStep = pInk->FindFirstDepthFirst();

	AttrFillGeometry * pFill = NULL;

	while (pNodeStep && pNodeStep != pInk)
	{
		if (pNodeStep->IsAnObject())
		{
			if (!pNodeStep->NeedsParent(NULL))
			{
				// does this have a fill attribute ?
				if (((NodeRenderableInk *)pNodeStep)->FindAppliedAttribute(CC_RUNTIME_CLASS(AttrFillGeometry),
					(NodeAttribute **)(&pFill)))
				{				
					// let's check this to see if there's a colour on the node or not
					if (pFill->GetStartColour())
					{
						if (!pFill->GetStartColour()->IsTransparent())
							return (NodeRenderableInk *)pNodeStep;
					}
				}
			}
		}

		pNodeStep = pNodeStep->FindNextDepthFirst(pInk);
	}

	return pInk;
}

/***********************************************************************************************

>	BOOL NodeContour::DoBecomeA(BecomeA* pBecomeA)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/8/99
	Inputs:		See base class
	Purpose:	Turns this node into another node of a particular type

***********************************************************************************************/
BOOL NodeContour::DoBecomeA(BecomeA* pBecomeA)
{
// Check for a NULL entry param							   
	ERROR2IF_PF(pBecomeA == NULL,FALSE,("pBecomeA is NULL"));

	if (!pBecomeA->BAPath())
	{
		return FALSE;
	}

	BOOL ValidReason = (pBecomeA->GetReason() == BECOMEA_REPLACE || pBecomeA->GetReason() == BECOMEA_PASSBACK);
	ERROR2IF_PF(!ValidReason,FALSE,("Unkown BecomeA reason %d",pBecomeA->GetReason()));

	UndoableOperation* pOp = pBecomeA->GetUndoOp();

	double StepSize = 0;
	double Ratio = 0;

	NodeContourController * pController = (NodeContourController *)FindParent();

	if (!pController)
	{
		ERROR3("No contour controller for become a");
		return FALSE;
	}

	INT32 Width = pController->GetWidth();

//	Node * pContourBegin = NULL;

	BOOL bPassback = FALSE;

	if (pBecomeA->GetReason() == BECOMEA_PASSBACK)
		bPassback = TRUE;

//	CCAttrMap * pTmpMap = NULL;

	// turn each path into a node path with the appropriate attributes
	if (m_pPathList != NULL && m_NumPaths > 0)
	{
		// do the blended attributes map
		CCAttrMap BlendAttrMap(30);	

		// build the attribute maps for the start & end nodes
		NodeRenderableInk * pEndNode = NULL;

		if (pController->GetWidth() > 0)
		{
			pEndNode = (NodeRenderableInk *)FindPrevious(CC_RUNTIME_CLASS(NodeRenderableInk));
		}
		else
		{
			pEndNode = (NodeRenderableInk *)FindNext(CC_RUNTIME_CLASS(NodeRenderableInk));
		}

		if (!pEndNode)
		{
			ERROR3("No node present to contour");
			return FALSE;
		}

		// CGS:  need to check whether pEndNode is compound - and if it is, do some extra stuff

		if (pEndNode->IsCompound())
		{
			pEndNode = FindNodeToContour(pEndNode);
		}

		if (pEndNode->IS_KIND_OF(NodeBlend))
		{
			pEndNode = (NodeRenderableInk *)pEndNode->FindFirstChild(CC_RUNTIME_CLASS(NodeRenderableInk));
		}
		
		// do the blend of the attributes
		CCAttrMap * pAttrMapStart = CCAttrMap::MakeAppliedAttrMap(this);
		CCAttrMap * pAttrMapEnd   = CCAttrMap::MakeAppliedAttrMap(pEndNode);

		ERROR3IF (pAttrMapStart == NULL, "pAttrMapStart is NULL!");
		ERROR3IF (pAttrMapEnd == NULL, "pAttrMapEnd is NULL!");
		
		StepSize = 1.0 / ((double)(m_NumPaths));
		
		NodePath* pPath = NULL;
		Node* pInsertContext = this;

		// For DoBecomeA we must return the paths in the order in which they will be placed
		// in the tree and rendered
		// For Outer contours that means: end node, steps, original node
		// For Inner contours that means: original node, steps, end node
		//
		if (Width < 0)
		{
			// outer contour !
			// 
			// End node first
			INT32 i = m_NumPaths-1;

			if (pOp)
			{
				ALLOC_WITH_FAIL(pPath, new NodePath, pOp);
			}
			else
			{
				pPath = new NodePath;

				ERRORIF(pPath == NULL, _R(IDE_NOMORE_MEMORY), FALSE);
			}

			pPath->InkPath.Initialise();
			pPath->InkPath.CloneFrom(m_pPathList[i]);
			pPath->InkPath.IsFilled = TRUE;
			pPath->InkPath.InitialiseFlags(0, pPath->InkPath.GetNumCoords());
			
			// insert the node
			if (!bPassback)
			{
				pPath->ApplyAttributes(pAttrMapStart, TRUE);

				if (pOp)
					pOp->DoInsertNewNode(pPath, pInsertContext, NEXT, TRUE, FALSE, TRUE, TRUE);
				else
				{
					pPath->AttachNode(pInsertContext, NEXT, FALSE, FALSE);
					pPath->NormaliseAttributes();
				}
				pInsertContext = pPath;
				pPath->SetSelected(TRUE);

				pBecomeA->PassBack(pPath, this, pAttrMapStart->Copy());
			}
			else
			{
				pBecomeA->PassBack(pPath, this, pAttrMapStart->Copy());
			}

			// Now all the steps
			for (i = m_NumPaths-2; i>=0; i--)
			{
				// blend the attributes at this point
				Ratio = (StepSize * ((double)i)) + StepSize;
				
				// calculate the correct ratio given the profile for attributes
				CProfileBiasGain Profile = pController->GetAttrProfile();
				Profile.SetBias(-Profile.GetBias());
				Ratio = (double)Profile.MapZeroToOne((AFp)Ratio);
				
				Ratio = 1.0 - Ratio;
				
				if (!BlendAttributes(NULL, pAttrMapStart, pAttrMapEnd, &BlendAttrMap, 
					Ratio))
				{
					ERROR3("Blend attributes failed");
					return FALSE;
				}
				
				if (pOp)
				{
					ALLOC_WITH_FAIL(pPath, new NodePath, pOp);
				}
				else
				{
					pPath = new NodePath;

					ERRORIF(pPath == NULL, _R(IDE_NOMORE_MEMORY), FALSE);
				}

				pPath->InkPath.Initialise();
				pPath->InkPath.CloneFrom(m_pPathList[i]);

				pPath->InkPath.IsFilled = TRUE;
				pPath->InkPath.InitialiseFlags(0, pPath->InkPath.GetNumCoords());
				
				// insert the node
				if (!bPassback)
				{
					// copy the blended attributes to the new node
					// CopyAttributes(pPath, &BlendAttrMap);
					pPath->ApplyAttributes(&BlendAttrMap, TRUE);						

					if (pOp)
						pOp->DoInsertNewNode(pPath, pInsertContext, NEXT, TRUE, FALSE, TRUE, TRUE);
					else
					{
						pPath->AttachNode(pInsertContext, NEXT, FALSE, FALSE);
						pPath->NormaliseAttributes();
					}

					pInsertContext = pPath;
					pPath->SetSelected(TRUE);

					pBecomeA->PassBack(pPath, this, BlendAttrMap.Copy());
				}
				else
				{
					pBecomeA->PassBack(pPath, this, BlendAttrMap.Copy());
				}
			}
		}
		else
		{
			// inner contour !
			INT32 i = 0;
			for (i = m_NumPaths-1; i!=0 ; i--)
			{
				// Check if the path has any points before outputing it
				// This stops degenerate paths being added to the document
				// which will cause errors in the saving code
				Path* pStepPath = &(m_pPathList[(m_NumPaths - 1) - i]);
				if (pStepPath && pStepPath->GetNumCoords() > 0)
				{
					// calculate the correct ratio given the profile for attributes
					Ratio = (StepSize * ((double)i));
					
					// calculate the correct ratio given the profile for attributes
					CProfileBiasGain Profile = pController->GetAttrProfile();
					Profile.SetBias(-Profile.GetBias());
					Ratio = (double)Profile.MapZeroToOne((AFp)Ratio);
					
					if (!BlendAttributes(NULL, pAttrMapStart, pAttrMapEnd, &BlendAttrMap, 
						Ratio))
					{
						ERROR3("Blend attributes failed");
						return FALSE;
					}
					
					pPath = new NodePath;

					pPath->InkPath.Initialise();
					pPath->InkPath.CloneFrom(*pStepPath);
					pPath->InkPath.IsFilled = TRUE;
					pPath->InkPath.InitialiseFlags(0, pPath->InkPath.GetNumCoords());
					
					if (!bPassback)
					{	
						pPath->ApplyAttributes(&BlendAttrMap, TRUE);
						if (pOp)
							pOp->DoInsertNewNode(pPath, pInsertContext, NEXT, TRUE, FALSE, TRUE, TRUE);
						else
						{
							pPath->AttachNode(pInsertContext, NEXT, FALSE, FALSE);
							pPath->NormaliseAttributes();
						}

						pInsertContext = pPath;
						pBecomeA->PassBack(pPath, pPath, BlendAttrMap.Copy());
					}
					else
					{
						pBecomeA->PassBack(pPath, pPath, BlendAttrMap.Copy());
					}
				}
			}

			// Finally, the end node
			Path* pStepPath = &(m_pPathList[m_NumPaths - 1]);
			if (pStepPath && pStepPath->GetNumCoords() > 0)
			{
				pPath = new NodePath;
				pPath->InkPath.Initialise();

				pPath->InkPath.CloneFrom(*pStepPath);
				pPath->InkPath.IsFilled = TRUE;
				pPath->InkPath.InitialiseFlags(0, pPath->InkPath.GetNumCoords());
				
				// insert the node
				if (!bPassback)
				{
					// CopyAttributes(pPath, pAttrMapStart);
					pPath->ApplyAttributes(pAttrMapStart, TRUE);
					if (pOp)
						pOp->DoInsertNewNode(pPath, pInsertContext, NEXT, TRUE, FALSE, TRUE, TRUE);
					else
					{
						pPath->AttachNode(pInsertContext, NEXT, FALSE, FALSE);
						pPath->NormaliseAttributes();
					}

					pInsertContext = pPath;

					pBecomeA->PassBack(pPath, pPath, pAttrMapStart->Copy());
				}
				else
				{
					pBecomeA->PassBack(pPath, pPath, pAttrMapStart->Copy());
				}
			}
		}

		BlendAttrMap.DeleteAttributes();

		delete pAttrMapStart;
		delete pAttrMapEnd;
		
		// hide the contour begin node
		if (!bPassback)
		{
			if (pOp)
			{
				NodeHidden * pHidden = NULL;
				pOp->DoHideNode(this, TRUE, &pHidden, TRUE);
			}
			else
			{
				CascadeDelete();
				delete this;		// Scary!
			}
		}
		
		return TRUE;
	}

	return FALSE;
}

/***********************************************************************************************

>	BOOL NodeContour::CopyAttributes(NodeRenderableInk * pNode, CCAttrMap * pAttrMap)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/8/99
	Inputs:		See base class
	Purpose:	Copies all the attributes in the attribute map onto the given node

***********************************************************************************************/
BOOL NodeContour::CopyAttributes(NodeRenderableInk * pNode, CCAttrMap * pAttrMap)
{
	ERROR2IF(pNode == NULL, FALSE, "Node is null");
	ERROR2IF(pAttrMap == NULL, FALSE, "Attribute map is null");
	
	CCAttrMap::iterator end = pAttrMap->GetEndPosition();
	CCAttrMap::iterator PosStart = pAttrMap->GetStartPosition();

	while( PosStart != end )
	{
		CCRuntimeClass *pTypeStart = NULL; 
		void		   *pValStart = NULL; 

		pAttrMap->GetNextAssoc( PosStart, pTypeStart, pValStart );
		NodeAttribute* pNodeAttr = (NodeAttribute *)pValStart;

		// copy the attribute onto the node
		if (pNodeAttr)
		{
			if (pNodeAttr->CanBeAppliedToObject())
			{
				if (!pNodeAttr->CopyNode(pNode, FIRSTCHILD))
				{
					ERROR3("Copy node failed");
					return FALSE;
				}
			}
		}
	}

	return TRUE;
}









/***********************************************************************************************

>	BOOL NodeContour::CanBecomeA(BecomeA* pBecomeA)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/8/99
	Inputs:		See base class
	Purpose:	Can this node turn itself into a node of a particular type ?

***********************************************************************************************/
BOOL NodeContour::CanBecomeA(BecomeA* pBecomeA)
{
	if (pBecomeA->BAPath())
	{
		if (pBecomeA->IsCounting())
		{
			NodeContourController * pControl = (NodeContourController *)FindParent();
	
			if (pControl->GetInsetPathFlag())
			{
				pBecomeA->AddCount(1);
			}
			else
			{
				pBecomeA->AddCount(pControl->GetNumberOfSteps());
			}
		}

		return TRUE;
	}

	return FALSE;
}

/***********************************************************************************************

>	virtual BOOL NodeContour::NeedsParent(Node* pClassNode) const;

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/8/99
	Inputs:		See base class
	Purpose:	Does this node need its parent ?

***********************************************************************************************/
BOOL NodeContour::NeedsParent(Node* pClassNode) const
{
	if (!pClassNode)
		return TRUE;

	if (pClassNode->IsKindOf(CC_RUNTIME_CLASS(NodeContourController)))
		return TRUE;

	return FALSE;
}

/***********************************************************************************************

>	String NodeContour::Describe(BOOL Plural, BOOL Verbose)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/8/99
	Inputs:		See base class
	Purpose:	Gets the string descriptor for this object

***********************************************************************************************/
String NodeContour::Describe(BOOL Plural, BOOL Verbose)
{
	String Name;
	Name.Load(_R(IDS_CONTOUR_NODE_NAME));

	if (Plural)
	{
		Name += _T("s");
	}

	return Name;
}



/********************************************************************************************

>	BOOL NodeContour::GenerateContourPathForNode(	Path* pDestPath,
													const Node* pSourceNode,
													const List* pSourceList,
													const MILLIPOINT nWidth,
													const BOOL fOuterContour	= TRUE,
													const JointType jt			= RoundJoin,
													const MILLIPOINT nFlatness	= -1,
													const BOOL fUseLineWidths	= TRUE,
													const BOOL fIncludeShadows	= FALSE	)
	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12 September 2000

	Inputs:		pDestPath		ptr to a Path, which will receive the contoured result.
				pSourceNode		ptr to a node to contour.
				pSourceList		ptr to a list of paths + attrs to contour.
				nWidth			the width of the required contour, in millipoints.
				fOuterContour	TRUE => do an outer contour; FALSE => do an inner contour.
				jt				the required join type.
				nFlatness		the flatness to use, in millipoints. if <= 0, then this
								routine will use a default value which takes into account
								the current view's zoom factor. If there is no current
								view, then a default value of 200 will be used.
				fUseLineWidths	whether or not to include line width attributes.
				fIncludeShadows	whether or not to include the outlines of shadows
								in our calculations; required by feathering.

	Outputs:	if successful, pDestPath is filled with the required contour path.

	Returns:	TRUE if successful, FALSE otherwise.

	Purpose:	Generate a contour for the given node or list of paths + attrs.

	Errors:		ERROR2 if pDestPath is NULL or both pSourceNode and pSourceList are NULL.

	Notes:		Karim 18/11/2000
				Major rewrite - previously this method used ContourBecomeA. We no longer
				rely on that procedure, but use PathBecomeA instead, and contour the result.
				See Camelot2000 SourceSafe db for changes.
	See also:	

********************************************************************************************/
BOOL NodeContour::GenerateContourPathForNode(	Path* pDestPath,
												Node* pSourceNode,
												const List* pSourceList,
												const MILLIPOINT nWidth,
												const BOOL fOuterContour,
												const JointType jt,
												const MILLIPOINT nFlatness,
												const BOOL fUseLineWidths,
												const BOOL fIncludeShadows	)
{
	// pDestPath *must* be valid, as must one of pSourceNode or pSourceList.
	ERROR2IF(pDestPath == NULL ||
			(pSourceNode == NULL && pSourceList == NULL), FALSE,
			"NodeContour::GenerateContourPathForNode; Invalid input parameters!");

	pDestPath->ClearPath();
	pDestPath->Initialise();

/* Proposed implementation of this method: 
	ContourBecomeA2 baContour(NULL, nWidth, pDestPath);
	BOOL fSuccess = pSourceNode->DoBecomeA(&baContour);
*/
/* Current implementation of this method: */
	PathBecomeA baOutline(	BECOMEA_PASSBACK,
							CC_RUNTIME_CLASS(NodePath),
							NULL,
							FALSE,
							pDestPath,
							PathBecomeA::ADD_OUTLINES
						);

	baOutline.SetDoSilhouette(TRUE);
	baOutline.SetDoShadowSilhouettes(TRUE);

	BOOL fSuccess = pSourceNode->DoBecomeA(&baOutline);

	if (fSuccess && pDestPath->GetNumCoords() > 2)
	{
		Path WorkPath;
		WorkPath.Initialise();
		WorkPath.CloneFrom(*pDestPath);
		WorkPath.InitializeContourValues(2 * nWidth);
		fSuccess = (WorkPath.GetContourForStep(pDestPath) > 0);
	}
/**/

	return fSuccess;
}



/***********************************************************************************************

>	BOOL NodeContour::GenerateContour(List * pList)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/8/99
	Inputs:		If we're blending we pass in the path processor's list
	Purpose:	Generates the contour paths in this node

***********************************************************************************************/
BOOL NodeContour::GenerateContour(List * pList, CCAttrMap * pAttrMap)
{	
	// build up the source path
	NodeContourController * pControl = (NodeContourController *)FindParent();
	Node * pNode = pControl;

	// check for a valid set of children
	if (!pNode)
		return FALSE;

	// if we're printing, don't bother - probably called by mistake !
	if (pControl->IsPrinting())
	{
		ERROR3("Generate contour called when printing");
		return FALSE;
	}

	// an inset path is the same as a 1 step contour
	if (pControl->GetInsetPathFlag())
		m_NumPaths = 0;
	
	if (pControl->GetInsideBoundingRect().IsEmpty())
		return FALSE;

	DeleteCache();

	pNode = pNode->FindFirstChild();

	if (!pNode)
		return FALSE;

	m_SourcePath.ClearPath();

	if (m_pPathList)
	{
		delete [] m_pPathList;
		m_pPathList = NULL;
	}

	INT32 Width = 0;
	INT32 NumSteps = 0;
	BOOL bOuter = FALSE;

	if (pControl)
	{
		NumSteps = pControl->GetNumberOfSteps();
		Width = pControl->GetWidth();

		if (Width < 0)
		{
			Width = -Width;
			bOuter = TRUE;
		}

		NumSteps ++;

		if (pControl->GetInsetPathFlag())
			NumSteps = 1;
	}	

	ERROR3IF (NumSteps <= 0, "I'm not going to work with that!");
	
	// create and initialise my contour path list
	m_pPathList = new Path[NumSteps];

	if (!m_pPathList)
	{
		ERROR3("Can't create path list");
		return FALSE;
	}
	
	for (INT32 i = 0; i < NumSteps; i++)
		m_pPathList[i].Initialise();

	// CGS - we need to generate the bevel with respect to the JoinType applied to the path
	// that we are generating it with; BUT if the original bevel had a jointype; then we need
	// to generate it with respect to this!
	// get the current join type attribute
	AttrJoinType * pJT = NULL;
	AttrStartCap * pCT = NULL;
	JointType generateJoinType = BevelledJoin;//RoundJoin;
	LineCapType ct = LineCapRound;

	if (!pList)
	{
		if (FindAppliedAttribute(CC_RUNTIME_CLASS(AttrJoinType), (NodeAttribute **)&pJT))
		{
			// found an attribute so use this
			if (pJT)
				generateJoinType = pJT->Value.JoinType;
		}

		if (FindAppliedAttribute(CC_RUNTIME_CLASS(AttrStartCap), (NodeAttribute **)&pCT))
		{
			// found an attribute so use this
			if (pCT)
				ct = pCT->Value.StartCap;
		}
	}

	double Flatness = 200.0;

/// < ---------------
	// Mark Howitt 31/10/00
	// This function call now replaces the passed in value when the BecomeA function is created.
	// This has been done to elliminate zoom dependancies which change the flatness values
	// depending on the current zoom value. See the function header for more details
	Node* pNodeToBevel = pControl->FindFirstChildInk();

	while(pNodeToBevel == this && pNodeToBevel)
		pNodeToBevel = pNodeToBevel->FindNextInk();

	if(pNodeToBevel)
	{
		DocRect DR = ((NodeRenderableInk*)pNodeToBevel)->GetBoundingRect();
		
		if(!DR.IsEmpty())
		{
			// Get the smallest dimesion, Width or Height.
			double Smallest = DR.Height() < DR.Width() ? (double)DR.Height() : (double)DR.Width();

			// now make the flatness value equal to the smallest dimesion divided by 375
			Flatness = Smallest / 375.0;
		}

		// Check to see if we`re within the specified limits
		if(1.0 > Flatness) Flatness = 1.0;
		if(375.0 < Flatness) Flatness = 375.0;
	}
/// < ----------------

	// use the BecomeA passback mechanism to contour everything
	CProfileBiasGain	ObjectProfile( pControl->GetObjectProfile() );
	ContourBecomeA MyBecomeA( BECOMEA_PASSBACK, CC_RUNTIME_CLASS(NodePath),
		NULL, FALSE, m_pPathList, &m_SourcePath, Width, NumSteps, bOuter, ObjectProfile,
		generateJoinType, Flatness, FALSE );

	// tell the becomeA structure who's using it (i.e. Me !)
	MyBecomeA.SetCallNode(this);

	Path BecomeAStrokedPath;
	BecomeAStrokedPath.Initialise();

	Path CopyPath;
	CopyPath.Initialise();

	Path TempPath;
	TempPath.Initialise();
	NodePath * pPathNode = NULL;

	// run through all nodes, contouring them
	if (!pList)
	{
		while (pNode)
		{
			// is the node a blend path ?
			if (!pNode->IS_KIND_OF(NodeBlendPath))
			{	
				MyBecomeA.ResetCount();

				if (pNode->IsNodePath())
				{
					// make the node out of this path node, first we'll try to see if we have a stroke
					// attribute applied, if so then just make a copy of the path, as the passback function
					// deals with getting the right path out of the stroke
					BOOL IsStroked = (((NodeRenderableInk*)pNode)->GetActiveStroke() != NULL);

					if(IsStroked)
						pPathNode = (NodePath*)pNode;
					else
						pPathNode = ((NodePath *)pNode)->MakeNodePathFromAttributes(Flatness,NULL,FALSE,FALSE);

					pPathNode->DoBecomeA(&MyBecomeA);

					if(!IsStroked)
					{
						delete pPathNode;
						pPathNode = NULL;
					}
				}
				else if (pNode->CanBecomeA(&MyBecomeA) && pNode != this)
				{
					if (MyBecomeA.GetCount() > 0)
					{
						if (pNode->IsABaseTextClass())
						{
							// reformat the text first
							if (pNode->IS_KIND_OF(TextStory))
							{
								((TextStory *)pNode)->FormatAndChildren();
							}
							else
							{
								if (((BaseTextClass *)pNode)->FindParentStory())
								{
									((BaseTextClass *)pNode)->FindParentStory()->FormatAndChildren();
								}
								else
								{
									ERROR3("Found a base text class with no parent story");
								}
							}
						}
						
						pNode->DoBecomeA(&MyBecomeA);

						if(MyBecomeA.GetCount() > 1)
						{
							// Need to clip!
							CopyPath.ClearPath();
							CopyPath.CloneFrom(*m_pPathList);
							m_pPathList->ClearPath();

							TempPath.ClipPathToPath(CopyPath,m_pPathList,7|(1<<4),50,Flatness,Flatness);
						}
					}
				}
			}
			
			pNode = pNode->FindNext();
		}
	}
	else
	{
		SumAllPathsElem *pElem = (SumAllPathsElem *)pList->GetHead();

		while (pElem)
		{
			Path BlankPath;
			BlankPath.Initialise();
			
			NodePath * pPathNode = new NodePath;
			ERRORIF(pPathNode == NULL, _R(IDE_NOMORE_MEMORY), FALSE);
			
			pPathNode->InkPath.Initialise();
			
			pElem->GetPath()->ClipPathToPath(BlankPath, &(pPathNode->InkPath), 6);
			
			AttrJoinType * pJoinType = NULL;		// whats in the blend step?
			AttrStartCap * pStartType = NULL;

			pElem->GetAttrMap ()->Lookup( CC_RUNTIME_CLASS(AttrJoinType), (void *&)pJoinType );
			pElem->GetAttrMap ()->Lookup( CC_RUNTIME_CLASS(AttrStartCap), (void *&)pStartType );

			if (pJoinType)
				generateJoinType = pJoinType->Value.JoinType;		// use the one in the blend step

			if (pStartType)
				ct = pStartType->Value.StartCap;		// use the one in the blend step

			NodePath * pStrokedPathNode = pPathNode->MakeNodePathFromAttributes(Flatness, pElem->GetAttrMap(),FALSE,FALSE);
			
			if (pStrokedPathNode)
			{	
				pStrokedPathNode->DoBecomeA(&MyBecomeA);
				delete pStrokedPathNode;
			}
			
			if(pPathNode)
			{
				delete pPathNode;
				pPathNode = NULL;
			}
			
			pElem = (SumAllPathsElem *)pList->GetNext(pElem);
		}
	}

	// set all the filled flags on the path list
	for( INT32 i = NumSteps-1; i >= 0; i-- )
		m_pPathList[i].IsStroked = TRUE;

	m_NumPaths = NumSteps;
	
	InvalidateBoundingRect();

	if (FindParent())
		((NodeRenderableInk *)FindParent())->InvalidateBoundingRect(TRUE);

	return TRUE;
}

/***********************************************************************************************

>	void NodeContour::DeleteCache()

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/8/99
	Inputs:		
	Purpose:	Deletes all the paths

***********************************************************************************************/
void NodeContour::DeleteCache()
{
	if (m_pPathList)
	{
		delete [] m_pPathList;
		m_pPathList = NULL;
		m_NumPaths = 0;
	}
}

/***********************************************************************************************

>	void NodeContour::Transform(TransformBase &Trans)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/8/99
	Inputs:		
	Purpose:	Transforms me & all my paths

***********************************************************************************************/
void NodeContour::Transform(TransformBase &Trans)
{
	DocRect dr ;

	for (INT32 i = 0 ; i < m_NumPaths; i++)
	{
		Trans.Transform(m_pPathList[i].GetCoordArray(), m_pPathList[i].GetNumCoords());
		dr = dr.Union(m_pPathList[i].GetBoundingRect());
	}

	InvalidateBoundingRect(TRUE);

	Node * pNode = NULL;
	NodeContourController * pControl = (NodeContourController *)FindParent();

	if (!pControl)
		return;
	
	if (pControl->GetWidth() > 0)
		pNode = FindPrevious(CC_RUNTIME_CLASS(NodeRenderableInk));
	else
		pNode = FindNext(CC_RUNTIME_CLASS(NodeRenderableInk));

	// reformat all text nodes if we're not just a translation
	Trans2DMatrix * pMat2D = NULL;
	
	if (Trans.IsInvertable())
		pMat2D = (Trans2DMatrix*)(&Trans);

	if (pMat2D)
	{
		if (!pMat2D->GetMatrix().IsTranslation())
		{
			while (pNode)
			{
				if (pNode->IsABaseTextClass())
				{
					// reformat the text first
					if (pNode->IS_KIND_OF(TextStory))
					{
						((TextStory *)pNode)->FormatAndChildren();
					}
					else
					{
						if (((BaseTextClass *)pNode)->FindParentStory())
						{
							((BaseTextClass *)pNode)->FindParentStory()->FormatAndChildren();
						}
						else
						{
							ERROR3("Found a base text class with no parent story");
						}
					}
				}
				
				if (pControl->GetWidth() > 0)
					pNode = pNode->FindPrevious(CC_RUNTIME_CLASS(NodeRenderableInk));
				else
					pNode = pNode->FindNext(CC_RUNTIME_CLASS(NodeRenderableInk));
			}
		}
	}

	TransformChildren(Trans);
}

/***********************************************************************************************

>	void NodeContour::RenderEorDrag(RenderRegion * pRender)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/8/99
	Inputs:		
	Purpose:	Renders my eor drag stuff

***********************************************************************************************/
void NodeContour::RenderEorDrag(RenderRegion * pRender)
{
	if (!m_pPathList || m_NumPaths == 0)
		return;
	
	if (m_pPathList[0].GetNumCoords()>0)
		pRender->DrawPath(&(m_pPathList[0]));				// render the first contour

	if (m_pPathList[m_NumPaths-1].GetNumCoords()>0)
		pRender->DrawPath(&(m_pPathList[m_NumPaths-1]));	// render the last contour
}

/***********************************************************************************************

>	BOOL NodeContour::WritePreChildrenWeb(BaseCamelotFilter* pFilter)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/8/99
	Inputs:		See base class
	Purpose:	Writes the node to a file

***********************************************************************************************/
BOOL NodeContour::WritePreChildrenWeb(BaseCamelotFilter* pFilter)
{
	CXaraFileRecord Rec(TAG_CONTOUR, TAG_CONTOUR_SIZE);

	BOOL ok = Rec.Init();
	if (ok)	ok = pFilter->Write(&Rec);

	return ok;
}

/***********************************************************************************************

>	BOOL NodeContour::WritePreChildrenNative(BaseCamelotFilter* pFilter)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/8/99
	Inputs:		See base class
	Purpose:	Writes the node to a file

***********************************************************************************************/
BOOL NodeContour::WritePreChildrenNative(BaseCamelotFilter* pFilter)
{
	return WritePreChildrenWeb(pFilter);
}

/***********************************************************************************************

>	Node* NodeContour::SimpleCopy()

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/8/99
	Inputs:		See base class
	Purpose:	Writes the node to a file

***********************************************************************************************/
Node* NodeContour::SimpleCopy()
{
	NodeContour * pNode = new NodeContour;

	if (!pNode)
	{
		ERROR3("Can't create node contour");
		return NULL;
	}

	CopyNodeContents(pNode);

	return pNode;
}

/********************************************************************************************
>	void NodeContour::CopyNodeContents(NodeContour* pNewNode)

 	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/01/2004
	Inputs:		-
	Returns:	
	Purpose:	Sort of backwards copy constructor

********************************************************************************************/
void NodeContour::CopyNodeContents(NodeContour* pNewNode)
{
	if (pNewNode->m_pPathList)
	{
		for (INT32 i = 0 ; i < pNewNode->m_NumPaths; i++)
		{
			pNewNode->m_pPathList[i].ClearPath();
		}
		delete[] pNewNode->m_pPathList;
	}

	pNewNode->m_NumPaths = m_NumPaths;
	pNewNode->m_pPathList = new Path[m_NumPaths];
	pNewNode->m_FirstRender = m_FirstRender;

	if (pNewNode->m_pPathList)
	{
		for (INT32 i = 0 ; i < m_NumPaths; i++)
		{
			pNewNode->m_pPathList[i].Initialise();
			pNewNode->m_pPathList[i].CloneFrom(m_pPathList[i]);
			pNewNode->m_pPathList[i].IsFilled = TRUE;
			pNewNode->m_pPathList[i].IsStroked = TRUE;
		}
	}

	pNewNode->m_SourcePath.CloneFrom(m_SourcePath);

	NodeRenderableInk::CopyNodeContents(pNewNode);
}




/***********************************************************************************************
>   void NodeContour::PolyCopyNodeContents(NodeRenderable* pNodeCopy)

    Author:     Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
    Created:    18/12/2003
    Outputs:    -
    Purpose:    Polymorphically copies the contents of this node to another
	Errors:     An assertion failure will occur if NodeCopy is NULL
    Scope:      protected
								     
***********************************************************************************************/

void NodeContour::PolyCopyNodeContents(NodeRenderable* pNodeCopy)
{
	ENSURE(pNodeCopy, "Trying to copy a node's contents into a NULL node");
	ENSURE(IS_A(pNodeCopy, NodeContour), "PolyCopyNodeContents given wrong dest node type");

	if (IS_A(pNodeCopy, NodeContour))
		CopyNodeContents((NodeContour*)pNodeCopy);
}



   
/***********************************************************************************************

>	BOOL NodeContour::BlendAttributes(RenderRegion * pRegion, CCAttrMap * pBlendedAttrMap, 
											double BlendRatio)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/8/99
	Inputs:		Render region to blend the attributes with, and the ratio (i.e. the
				position of the blend 0 <= BlendRatio <= 1.0
	Purpose:	Blends all the attributes to this particular position,

***********************************************************************************************/
BOOL NodeContour::BlendAttributes(RenderRegion * pRender, CCAttrMap * pAttrMapStart, 
								  CCAttrMap * pAttrMapEnd, CCAttrMap * pBlendedAttrMap, 
											double BlendRatio)
{
	// out of nodebldr.cpp - ln 1388
	// Process each attribute in turn

	// These vars are used as params to the CCAttrMap funcs
	CCRuntimeClass	   *pTypeStart = NULL; 
	void			   *pValStart = NULL; 
	void			   *pValEnd = NULL;

	NodeContourController * pController = (NodeContourController *)FindParent(CC_RUNTIME_CLASS(NodeContourController));

	if (!pController)
		return FALSE;

	BOOL useStartAttrMap = FALSE;

	CCAttrMap::iterator	PosStart = pAttrMapStart->GetStartPosition();
	CCAttrMap::iterator	end		 = pAttrMapStart->GetEndPosition();
	while ( PosStart != end )
	{
		// Get a ptr to the attr at position PosStart in the start node's attr map
		pAttrMapStart->GetNextAssoc(PosStart,pTypeStart,pValStart);
		NodeAttribute* pNodeAttrStart = (NodeAttribute *)pValStart;

		if (pNodeAttrStart->CanBeAppliedToObject())
		{
			// Get a blended attribute
			NodeAttribute* pBlendedNodeAttr = NULL;
			
			if (pAttrMapEnd)
			{
				// Find an attr of the same type in the end object's attr list,
				// and blend the two attrs together
				if (pAttrMapEnd->Lookup(pTypeStart,pValEnd))
				{
					// Set up the param object to pass to the start attr's blend method
					BlendAttrParam BlendParam;
					
					// Initialise the BlendParam with the end attr and blend ratio
					if (BlendParam.Init(pRender,
										(NodeAttribute *)pValEnd,BlendRatio,pController->GetColourBlendType(),
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
			}
			else
			{
				useStartAttrMap = TRUE;
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
	}

	if (useStartAttrMap)
	{
		pBlendedAttrMap = pAttrMapStart->Copy ();
	}

	return TRUE;
}



/***********************************************************************************************

>	BOOL NodeContour::AllowOp(ObjChangeParam *pParam, BOOL SetOpPermissionState = TRUE,
													  BOOL DoPreTriggerEdit = TRUE)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>; Karim 12/01/2000
	Created:	12/8/99
	Inputs:		pParam					describes the way an op wants to change the node
				SetOpPermissionState	if TRUE the OpPermission of nodes should be set
				DoPreTriggerEdit		if TRUE then NameGallery::PreTriggerEdit is called.
										*Must* be TRUE if the calling Op may make any nodes
										change their bounds, eg move, line width, cut.
										Use TRUE if unsure.
	Purpose:	Does the allow op mechanism

***********************************************************************************************/
BOOL NodeContour::AllowOp(ObjChangeParam *pParam, BOOL SetOpPermissionState,
												  BOOL DoPreTriggerEdit)
{
	ERROR2IF(pParam==NULL,FALSE,"NodeContourController::AllowOp() - pParam==NULL");

	// clean out the calling-child ptr, so it doesn't get passed around unintentionally.
	pParam->SetCallingChild(NULL);

	// Set up a flag to see if any of the child objects get changed
	BOOL allowed=TRUE;

	UndoableOperation* pOp = pParam->GetOpPointer();

	// see whether our parent allows the op.
	if (allowed && Parent != NULL && pParam->GetDirection() != OBJCHANGE_CALLEDBYPARENT)
	{
		ObjChangeDirection OldDirection=pParam->GetDirection();
		pParam->SetCallingChild(this);
		pParam->SetDirection(OBJCHANGE_CALLEDBYCHILD);
		allowed=Parent->AllowOp(pParam,SetOpPermissionState,DoPreTriggerEdit);
		pParam->SetDirection(OldDirection);
	}

	// if the parent allows this, then test the op
	if (allowed)
	{
		// ok, test for ops which we don't allow.
		if(pOp != NULL)
		{
PORTNOTE("other","Removed OpCreateBevel, OpCreateNewMould, OpPasteEnvelope, OpPastePerspective usage")
			if(
#ifndef EXCLUDE_FROM_XARALX
				pOp->IS_KIND_OF(OpCreateBevel) ||
				pOp->IS_KIND_OF(OpCreateNewMould) ||
				pOp->IS_KIND_OF(OpPasteEnvelope) ||
				pOp->IS_KIND_OF(OpPastePerspective) ||
#endif
				false )
			{
				allowed = FALSE;
			}
		}
	}

	// if necessary, set permissions for OnChildChange.
	if (SetOpPermissionState)
		SetOpPermission(allowed ? PERMISSION_ALLOWED : PERMISSION_DENIED, TRUE);

	if (allowed)
	{
		UndoableOperation* pChangeOp = pParam->GetOpPointer();

		// check for geometry linked attributes
		BOOL InformGeomLinkedAttrs = SetOpPermissionState && pChangeOp && pChangeOp->MayChangeNodeBounds();
		if (InformGeomLinkedAttrs)
		{
			NodeAttribute* pNA = FindFirstGeometryLinkedAttr();
			while(pNA)
			{
				pNA->LinkedNodeGeometryHasChanged(pChangeOp);
				pNA = pNA->FindNextGeometryLinkedAttr();
			}
		}		

		// if we're ok so far and were asked to do a PreTriggerEdit, then
		// determine whether the Op may change the bounds of some nodes.
		// If it may, then call NameGallery::PreTriggerEdit.
		if (DoPreTriggerEdit && pChangeOp && pChangeOp->MayChangeNodeBounds())
		{
PORTNOTE("other","Removed NameGallery usage")
#ifndef EXCLUDE_FROM_XARALX
			allowed = NameGallery::Instance()->PreTriggerEdit(pChangeOp, pParam, this);
#endif
		}
	}

	// return result (directly, or indirectly via a child AllowOp()) to op
	return allowed;
}



/***********************************************************************************************

>	void NodeContour::RenderTinyBlobs(RenderRegion * pRegion)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/8/99
	Inputs:		Render region to render the tiny blobs to
	Purpose:	Renders the tiny blobs for this object

***********************************************************************************************/
void NodeContour::RenderTinyBlobs(RenderRegion * pRender)
{
//#pragma message( __LOCMSG__ "NodeContour::RenderEorDrag - do nothing" )
//	TRACE( _T("Warning - NodeContour::RenderEorDrag called\n") );
	// Set the line colours etc as we need them
	pRender->SaveContext();
	pRender->SetLineColour(COLOUR_NONE);
	pRender->SetFillColour(COLOUR_UNSELECTEDBLOB);

	// Render the blobs on the path
	BlobManager* BlobMgr = GetApplication()->GetBlobManager();

	DocCoord Coord(BoundingRectangle.lo.x + (BlobMgr->GetBlobSize() / 2), 
		BoundingRectangle.hi.y - (BlobMgr->GetBlobSize() / 2));
	pRender->DrawBlob(Coord, BT_UNSELECTED);
	pRender->RestoreContext();
}

/********************************************************************************************

>	virtual NodeCompound* NodeContour::GetParentController() const

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	06/12/1999
	Returns:	Pointer to parent Contour controller (NULL if not found!)
	Purpose:	Returns a type correct pointer to the parent Contour controller

********************************************************************************************/
NodeCompound* NodeContour::GetParentController() const
{
	NodeContourController* pBob = (NodeContourController*)FindParent();
	if (pBob != NULL && !IS_A(pBob, NodeContourController))
	{
		ERROR3("Parent of Contour was not a NodeContourController!");
		pBob = NULL;
	}

	return pBob;
}



/********************************************************************************************

>	virtual DocRect NodeContour::ValidateExtend(const ExtendParams& ExtParams)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	06/12/1999
	Inputs:		ExtParams		description parameters for the extension.
	Outputs:	
	Returns:	TRUE if extending this Nodewill be a reversible operation,
				FALSE otherwise.
	Purpose:	Tests the reversibility of an Extend operation applied to this node.

				A NodeContour cannot itself extend, so instead it asks its parent controller
				to extend for it. Infinite recursion does not occur, as the controller node
				ignores its child node, this NodeContour.
	Errors:		In debug builds, ERROR3 if this Node has no NodeContourController,
				in release, we return TRUE.
	See also:	IsTypeExtendible(), Extend().

********************************************************************************************/
DocRect NodeContour::ValidateExtend(const ExtendParams& ExtParams)
{
	Node* pBob = GetParentController();
	if (pBob == NULL)
	{
		ERROR3("NodeContour::ValidateExtend- no controller Node found!");
		return DocRect(INT32_MAX, INT32_MAX, INT32_MAX, INT32_MAX);
	}
	else
	{
		return pBob->ValidateExtend(ExtParams);
	}
}



/********************************************************************************************

>	virtual void NodeContour::Extend(const ExtendParams& ExtParams)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	06/12/1999
	Inputs:		ExtParams		description parameters for the extension.
	Outputs:	Some of the child nodes of this ContourNode's NodeContourController may have
				their dimensions altered.
	Returns:	
	Purpose:	Perform an Extend operation on this Node, and its children if appropriate.

				A NodeContour cannot itself extend, so instead it asks its parent controller
				to extend for it. Infinite recursion does not occur, as the controller node
				ignores its child node, this NodeContour.
	Errors:		In debug builds, ERROR3 if this Node has no NodeContourController,
				in release, we do nothing.
	See also:	

********************************************************************************************/
void NodeContour::Extend(const ExtendParams& ExtParams)
{
	Node* pBob = GetParentController();
	if (pBob == NULL)
	{
		ERROR3("NodeContour::ValidateExtend- no controller Node found!");
	}
	else
	{
		pBob->Extend(ExtParams);
	}

	return;
}

/********************************************************************************************

>	virtual BOOL NodeContour::OnNodePopUp(Spread* pSpread, DocCoord PointerPos, ContextMenu* pMenu)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/2/2000
	Inputs:		pSpread		The spread in which things are happening
				PointerPos	The Location of the mouse pointer at the time of the click
				pMenu		The menu to which items should be added
	Returns:	BOOL - TRUE if the node claims the click as its own and FALSE if it is
				not interested in the click
	Purpose:	Allows the shadow to respond to the click

********************************************************************************************/

BOOL NodeContour::OnNodePopUp(Spread* pSpread, DocCoord PointerPos, ContextMenu* pMenu)
{
	PORTNOTETRACE("other","NodeShadow::OnNodePopUp - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	return pMenu->BuildCommand(TOOL_OPTOKEN_CONTOUR, TRUE);
#else
	return FALSE;
#endif
}






//////////////////////////////////////////////////////////////////
// ContourBecomeA Implementation

/***********************************************************************************************

>	BOOL ContourBecomeA::PassBack(NodeRenderableInk* pNewNode,NodeRenderableInk* pCreatedByNode,
							  CCAttrMap* pAttrMap)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/8/99
	Inputs:		See base class
	Purpose:	Sums all node paths' paths together into one path

***********************************************************************************************/
BOOL ContourBecomeA::PassBack(NodeRenderableInk* pNewNode,NodeRenderableInk* pCreatedByNode,
							  CCAttrMap* pAttrMap)
{
	// output the path to a file
	ERROR2IF(pNewNode == NULL || pCreatedByNode == NULL, FALSE, "Parameter null");
	
	// if the new node isn't a node path then exit
	if (!pNewNode->IsNodePath())
	{
		// is it a compound node ?
		if (!pNewNode->IsCompound())
			return FALSE;

		// get the path out of the compound node
		NodePath * pPath = (NodePath *)(((NodeCompound *)pNewNode)->GetNodeToBlend()->PublicCopy());

		// delete the new node
		pNewNode->CascadeDelete();
		delete pNewNode;
		pNewNode = pPath;

	}

	// if the number of coordinates in the path is less than two exit
	if (((NodePath *)pNewNode)->InkPath.GetNumCoords() < 0)
		return TRUE;

	// variable to tell delete pAttrMap commands after this whether to do a delete attributes
	// on the map or not
	BOOL bDeleteAttrMap = TRUE;
	
	if (!pAttrMap)
	{
		// ok, set up the attribute map from the created by node
		pAttrMap = CCAttrMap::MakeAppliedAttrMap(pCreatedByNode);
		
		// and tell all deletes not to remove the attributes in this map when deleting it
		bDeleteAttrMap = FALSE;
	}

	DocRect br;
//	double d = 0;
//	double dWidth = 0;

	BOOL bStroke = FALSE;
	AttrLineWidth * pLineWidth = NULL;
	AttrStrokeColour * pStrokeColour = NULL;
	NodePath* pStrokedPath = NULL;

	if(pAttrMap)
	{
		pAttrMap->Lookup( CC_RUNTIME_CLASS(AttrStrokeColour), (void *&)pStrokeColour );
		
		if(pStrokeColour && !pStrokeColour->Value.GetStartColour()->IsTransparent())
			pStrokedPath = pCreatedByNode->GetSmoothVariableWidthStrokePath();
	}
	else
		pStrokedPath = pCreatedByNode->GetSmoothVariableWidthStrokePath();

	if(pStrokedPath)
	{
		// replace the path we got with the stroked path
		delete pNewNode;
		pNewNode = pStrokedPath;
		pStrokedPath = NULL;
		bStroke = TRUE;
		m_bIncludeLineWidths = FALSE; // stroke already incorporates line width
	
	}

	Path CopyPath;
	CopyPath.Initialise();

	Path ClipPath;
	ClipPath.Initialise();

//	Node * pChild = NULL;
//	Node * pNextNode = NULL;

	BOOL IsClosed = ((NodePath *)pNewNode)->InkPath.IsClosed();

	// only clip unstroked closed paths
	if (IsClosed && !bStroke)
	{
		ClipPath.CloneFrom(((NodePath *)pNewNode)->InkPath);
		((NodePath *)pNewNode)->InkPath.ClearPath();

		if(CopyPath.ClipPathToPath(ClipPath, &(((NodePath *)pNewNode)->InkPath), 3, ClipPathToPath_Tolerance, m_Flatness, m_Flatness) < 0)
		{
			ERROR3("BAD PATH ALERT!");
		}
	}

	CopyPath.ClearPath();
	ClipPath.ClearPath();
	
//	UINT32 LineWidth = 0;

	if (pNewNode->IS_KIND_OF(NodeBlendPath))
	{
		// don't do blend paths
		if (pAttrMap)
		{
			if (bDeleteAttrMap)
				pAttrMap->DeleteAttributes();
			
			delete pAttrMap;
		}
		
		pNewNode->CascadeDelete();
		delete pNewNode;	
		return TRUE;		
	}

	if (pNewNode->IsNodePath())
	{
		BOOL bDontUseThisNodePath = FALSE;

		// Karim 29/08/2000
		// If the NodePath's path data contains less than two handles, then give up now.
		if (((NodePath*)pNewNode)->InkPath.GetNumCoords() < 2)
			bDontUseThisNodePath = TRUE;

		// do nothing for these nodes as well - indicates a 'fit text to curve'
		// object
		else if (pNewNode->FindFirstChild())
			bDontUseThisNodePath = TRUE;

		if (bDontUseThisNodePath)
		{
			if (pAttrMap)
			{
				if (bDeleteAttrMap)
					pAttrMap->DeleteAttributes();

				delete pAttrMap;
				pAttrMap = NULL;
			}

			pNewNode->CascadeDelete();
			delete pNewNode;
			pNewNode = NULL;

			return TRUE;
		}
	}

	JointType jt = m_Join;

	// get the line width & stroke colour from the attribute map
	// to decide if we need to stroke it first
	if (IsClosed)
	{
		if(m_bIncludeLineWidths && pAttrMap != NULL)
		{
			pAttrMap->Lookup( CC_RUNTIME_CLASS(AttrLineWidth), (void *&)pLineWidth );
			pAttrMap->Lookup( CC_RUNTIME_CLASS(AttrStrokeColour), (void *&)pStrokeColour );

			if (pStrokeColour && pLineWidth)
			{
				UINT32 Width = pLineWidth->Value.LineWidth;

				if (pStrokeColour->GetStartColour() && Width > 0)
				{
					if (!pStrokeColour->GetStartColour()->IsTransparent())
					{
						// stroke the path & add it to the original path node's path
						Path OrigStrokedPath;
						OrigStrokedPath.Initialise();

						JoinStyles JoinS = (jt==MitreJoin) ? JOIN_MITER : (jt==RoundJoin) ? JOIN_ROUND : JOIN_BEVEL;

						OrigStrokedPath.CloneFrom(((NodePath *)pNewNode)->InkPath);
						((NodePath *)pNewNode)->InkPath.ClearPath();
							
						OrigStrokedPath.InitializeContourValues(Width,JoinS,true,m_Flatness);
						if(OrigStrokedPath.GetContourForStep(&((NodePath *)pNewNode)->InkPath,1.0) < 0)
						{
							ERROR3("BAD PATH ALERT!");
						}
					}
				}
			}
		}

		((NodePath *)pNewNode)->InkPath.TryToClose();
		((NodePath *)pNewNode)->InkPath.IsFilled = TRUE;
		((NodePath *)pNewNode)->InkPath.IsStroked = FALSE;
	}
	else
	{
		// turn the path into a closed path
		NodePath * pWholePath = ((NodePath *)pNewNode)->MakeNodePathFromAttributes(m_Flatness, pAttrMap);

		if (pWholePath)
		{
			// ok, delete the new node pointer & all its children
			// does the new node have pointers to child nodes ? if so, delete the child nodes
			// first, then delete the top level node
			pNewNode->CascadeDelete();
			delete pNewNode;
			pNewNode = pWholePath;
		}
	}


	// are the lists ok ?
	if (m_pPathList && m_pSummedPath)
	{
		// is the node a path (you can get groups out of BecomeA with NodePath)
		if (pNewNode->IsNodePath() && ((NodePath *)pNewNode)->InkPath.GetNumCoords() > 2)
		{
			if(m_Width != 0)
				if(!GenerateContourPaths(&(((NodePath *)pNewNode)->InkPath), m_pPathList, m_Join, (m_bOuter == TRUE)))
					return FALSE;

			m_pSummedPath->MergeTwoPaths(((NodePath *)pNewNode)->InkPath);
		}
	}

	if (pAttrMap)
	{
		if (bDeleteAttrMap)
			pAttrMap->DeleteAttributes();
		
		delete pAttrMap;
	}
	
	// does the new node have pointers to child nodes ? if so, delete the child nodes
	// first, then delete the top level node
	pNewNode->CascadeDelete();
	delete pNewNode;
	
	return TRUE;
}

/***********************************************************************************************

>	BOOL ContourBecomeA::GenerateContourPaths(Path * pPathToContour, Path * pPathList
											  JointType jt, bool Outer)
	Author:		Mark_Howitt (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/05/00
	Inputs:		See base class
	Purpose:	Contours the path & sums the output with the path list passed in
	Notes:		This is a totally rewritten version of the David Mc code!
				It basically cuts out all the unnessasary clipping and uses the Gavin
				Contouring code found in Path.cpp!

***********************************************************************************************/
BOOL ContourBecomeA::GenerateContourPaths(Path* pPathToContour, Path* pPathList, JointType jt, bool Outer,
										  bool UseContourMode, LineCapType ct)
{
	// First create a join Style from the given JoinType!
	JoinStyles JoinS = (jt==MitreJoin) ? JOIN_MITER : (jt==RoundJoin) ? JOIN_ROUND : JOIN_BEVEL;
	CapStyles CapS = (ct==LineCapButt) ? CAPS_BUTT : (ct==LineCapRound) ? CAPS_ROUND : CAPS_SQUARE;

	// Now initialize the contour values for the path To process using the width & JoinStyle
	/*UINT32 Length =*/ pPathToContour->InitializeContourValues(m_Width * 2, JoinS, Outer, m_Flatness, true, UseContourMode, CapS);

	Path* pTempPath = NULL;
	double StepValue = 1.0;

	// Get the current profiler in order to alter the step values
	CProfileBiasGain Profile = m_Profile;
	Profile.SetBias(-Profile.GetBias());

	// Loop through all the steps getting the individual paths!
	for (INT32 i = m_NumSteps; i > 0; i--)
	{
		// Get the correct Step Value
		if (m_NumSteps >= 2)
			StepValue = (double)i / m_NumSteps;

		// Correct the step value with the current profile settings
		StepValue = (double)Profile.MapZeroToOne((AFp)StepValue);

		// Get the Next Stepped Path pointer
		pTempPath = &pPathList[i-1];

		// Now get the new contoured path using the current width and step value!
		if(pPathToContour->GetContourForStep(pTempPath, StepValue) < 0)
			ERROR2(FALSE,"Failed to Get Controur! NODECNTR");
	}

	return TRUE;
}



/********************************************************************************************

>	virtual INT32 NodeContour::EstimateNodeComplexity (OpParam* details)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/09/2000

	Inputs:		details		any data that should be used for the calculation

	Outputs:	-

	Returns:	an estimate of the nodes complexity

	Purpose:	This function estimates a complexity value for the node.  The complexity
				value is based upon the total length of all paths in the node.

	See Also:	OpBlendNodes::DeterminBlendObjectsProcessorHit ()

********************************************************************************************/

INT32 NodeContour::EstimateNodeComplexity (OpParam* details)
{
	INT32 total = 0;
	
	for (INT32 i = 0 ; i < m_NumPaths && m_pPathList; i++)
		total += m_pPathList[i].GetUsedSlots();

	return (total);
}
