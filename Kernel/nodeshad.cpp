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
*/

#include "camtypes.h"

#ifdef BUILDSHADOWS

#include <math.h>
//#include "pathedit.h"

#include "nodeshad.h"
#include "nodecont.h"
#include "noderect.h"

// Code headers
#include "attrmgr.h"
#include "fillval.h"
#include "fillattr.h"
#include "gclips.h"
#include "paths.h"
#include "pathtrap.h"
#include "grndrgn.h"
#include "nodebmp.h"
#include "becomea.h"
#include "bmpcomp.h"

// New Bitmap Shadowing code
#include "app.h"
#include "bshadow.h"

// necessary for special case with bevelling
//#include "opbevel.h"
#include "nodebev.h"

// Save/load
#include "cxfdefs.h"	// TAG_SHADOW_SIZE
#include "cxftags.h"	// TAG_SHADOW
#include "cxfrec.h"		// CXaraFileRecord
#include "camfiltr.h"	// BaseCamelotFilter
//#include "swfrndr.h"	// FlashRenderRegion
#include "cmxrendr.h"	// CMXRenderRegion
//#include "ai_epsrr.h"	// AIEPSRenderRegion
#include "dibutil.h"
#include "lineattr.h"

// Resource headers
//#include "resource.h"
//#include "shadres.h"

//#include "moldtool.h"
//#include "mario.h"
#include "attrappl.h"
#include "view.h"
#include "docview.h"
#include "blobs.h"
#include "attrmap.h"
#include "extender.h"
//#include "ngcore.h"		// NameGallery, for stretching functionality

#include "gdraw.h"

//#include "contmenu.h"
#include "blndhelp.h"
#include "osrndrgn.h"
//#include "progress.h"
#include "spread.h"		// for class Spread
#include "qualattr.h"	// for class QualityAttribute
//#include "rendbits.h"	// for class OSRenderBitmap
#include "nodebldr.h"	// for class NodeCompoundBlendBecomeA
#include "offscrn.h"	// for GRenderRegionWrapper
#include "oilbitmap.h"	// for WinBitmap
#include "nodecntr.h"	// for ContourBecomeA
#include "nodecont.h"	// for default shadow settings
#include "fthrattr.h"	// for feather bodge.
#include "quality.h"	// for quality setting info - used for caching.
#include "brshattr.h"	// for AttrBrushType, which we don't want applied to us.
//#include "opfeathr.h"	// for OpChangeFeatherSize, which we don't want operating on us.
#include "nodeliveeffect.h"
#include "ophist.h"
#include "objchge.h"

//#include "prntview.h"

DECLARE_SOURCE( "$Revision: 662 $" );

CC_IMPLEMENT_DYNCREATE(NodeShadow, NodeRenderableInk)

// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW

// uncomment these to see the silhouette and final shadow bitmaps in the bitmap gallery.
//#define ATTACH_SILHOUETTE_BMP
//#define ATTACH_SHADOW_BMP

#define NEW_GLOW_SILHOUETTE

const UINT32 GlowWidthStrokeFlatness = 200;

const double MaxShadowBitmapSize = 1024;
const double MinShadowBitmapSize = 32;



/***********************************************************************************************

>	NodeShadow::NodeShadow(Node* 	ContextNode,
						AttachNodeDirection Direction,
						const DocRect& 		BoundingRect,
						BOOL				Locked = FALSE,
						BOOL 				Mangled = FALSE,
						BOOL 				Marked = FALSE,
						BOOL 				Selected = FALSE,
						BOOL 				Renderable = FALSE
						)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/11/94
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
	Note:		SetUpShape() must be called before the NodeRegularShape is in a state in which
				it can be used.
    SeeAlso:	NodeRegularShape::SetUpShape
    Errors:		An ENSURE will occur if ContextNode is NULL

***********************************************************************************************/
NodeShadow::NodeShadow(Node* ContextNode,  
					AttachNodeDirection Direction,  
					BOOL Locked, 
					BOOL Mangled,  
					BOOL Marked, 
					BOOL Selected    
			  ) : NodeRenderableInk(ContextNode, Direction, Locked, Mangled, Marked, Selected )
{                         
	InitialiseMemberVars();
}                        


 
/*********************************************************************************************

>	NodeShadow::NodeShadow() 

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/11/94
	Purpose:	This constructor creates a NodeRegularShape linked to no other.
	Note:		SetUpShape() should be called to change the NodeRegularShape before use!
	SeeAlso:	NodeRegularShape::SetUpShape                                                        

**********************************************************************************************/
NodeShadow::NodeShadow() : NodeRenderableInk()
{
	InitialiseMemberVars();
}



/*********************************************************************************************

>	void NodeShadow::InitialiseMemberVars()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/6/96
	Inputs:		-
	Outputs:	Initalised member variables
	Returns:	-
	Purpose:	One consistant place to initialise all the member variables of a NodeRegularShape

**********************************************************************************************/
void NodeShadow::InitialiseMemberVars()
{
	m_LastQualitySetting = (enum Quality::Fill)Quality::Solid;
	m_LastRequestedPixWidth = -1;
	m_LastActualPixWidth = -1;
	m_ShadowBitmap = NULL;
	m_RenderBitmaps = TRUE;
	m_bAmCopying = FALSE;
	m_bAmLoading = TRUE;
	m_Path.Initialise();
	m_NonTranslatedPath.Initialise();
	m_BitmapXOffset = 0;

	// m_CurrentBlur = 4 * 750;
	m_PreviousBlur = 0;
	m_ShadowWidth = 0;
	m_ShadowHeight = 0;
	// m_FloorShadowAngle = 20.0 * PI / 180.0;	
	// m_FloorShadowHeight = 1.0;
	m_dDarkness = 0.0;
	m_bHaveTransformed = TRUE;

	m_pBMPTransFill = NULL;
	
	// Get a pointer to the My new Render Selection functions
	m_pShadower = NULL;
}



/*********************************************************************************************

>	NodeShadow::~NodeShadow() 

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/1/95
	Purpose:	Distructor to delete the cached render path
	SeeAlso:	-

**********************************************************************************************/
NodeShadow::~NodeShadow()
{
	DeleteCache();
}



/*******************************************************************************************
>	void NodeShadow::DeleteCurrentShadowStuff()

	Author:		Mark_Howitt (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/9/98
	Purpose:	Deletes all Bitmaps and BitmapFillAttribute pointers
********************************************************************************************/
void NodeShadow::DeleteCurrentShadowStuff()
{
	
}



/***********************************************************************************************

>	virtual Node* NodeShadow::SimpleCopy()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/11/94
	Returns:	Pointer to a Node or NULL if there was not enough memory for the new node
	Purpose:	Makes a copy of all the data in the node

***********************************************************************************************/
Node* NodeShadow::SimpleCopy()
{
	NodeShadow* pNodeCopy = new NodeShadow();

	if (pNodeCopy != NULL)
    	CopyNodeContents(pNodeCopy);

	return pNodeCopy;
}            



/***********************************************************************************************

>	void NodeShadow::CopyNodeContents(NodeShadow* NodeCopy)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/11/94
	Inputs:		NodeCopy - The node to copy the data into
	Purpose:	Copies the data in the node by first calling the base class to get it to
				copy its stuff, and then copying its own stuff
				Note : Copies FROM this TO NodeCopy
	SeeAlso:	NodeRenderableInk::CopyNodeContents
	Errors:		ERROR3 if passed a NULL pointer.

***********************************************************************************************/
void NodeShadow::CopyNodeContents(NodeShadow* NodeCopy)
{
	ERROR3IF(NodeCopy == NULL,"NodeShadow::CopyNodeContents was asked to copy into a NULL pointer");

	// copy the bitmaps and them fill too
	// if they're defined !
	if (NodeCopy->m_ShadowBitmap)
		delete NodeCopy->m_ShadowBitmap;
	NodeCopy->m_ShadowBitmap = NULL;
	if (m_ShadowBitmap)
	{
		NodeCopy->m_ShadowBitmap = new KernelBitmap(m_ShadowBitmap->GetWidth(),
				m_ShadowBitmap->GetHeight(), 8, 0, TRUE);

		// ok, now copy the bitmap from me into the new bitmap on the copy node
		memcpy(NodeCopy->m_ShadowBitmap->GetBitmapBits(), m_ShadowBitmap->GetBitmapBits(), 
			DIBUtil::ScanlineSize(m_ShadowBitmap->GetWidth(), 8) * m_ShadowBitmap->GetHeight());

		// now, copy the palette
		LPRGBQUAD pTranspPalette = NodeCopy->m_ShadowBitmap->GetPaletteForBitmap();

		double dI = 0;
		
		for (INT32 i = 0 ; i < 256; i++)
		{
			dI = ((double)i / 255.0);

			dI = m_BiasGain.MapZeroToOne((AFp)dI);

			dI *= 255.0;

			if (dI > 255.0)
				dI = 255.0;

			pTranspPalette->rgbBlue = (BYTE)dI;
			pTranspPalette->rgbRed = (BYTE)dI;
			pTranspPalette->rgbGreen = (BYTE)dI;
			pTranspPalette->rgbReserved = 0x00;

			pTranspPalette ++;
		}

		if (NodeCopy->m_pBMPTransFill)
			delete NodeCopy->m_pBMPTransFill;
		NodeCopy->m_pBMPTransFill = new BitmapTranspFillAttribute;
		if (NodeCopy->m_pBMPTransFill)
		{
			// Now work out the control points for the given Shadow
			DocCoord StartP(m_SelectedRect.lo.x, m_SelectedRect.lo.y);
			DocCoord EndPA(m_SelectedRect.hi.x, m_SelectedRect.lo.y);
			DocCoord EndPB(m_SelectedRect.lo.x, m_SelectedRect.hi.y);

			// Set the individual control points with the worked out values
			NodeCopy->m_pBMPTransFill->SetStartPoint(&StartP);
			NodeCopy->m_pBMPTransFill->SetEndPoint(&EndPA);
			NodeCopy->m_pBMPTransFill->SetEndPoint2(&EndPB);	

			NodeCopy->m_pBMPTransFill->SetTesselation(RT_Simple);
			NodeCopy->m_pBMPTransFill->SetTranspType(TT_Mix);

			NodeCopy->m_pBMPTransFill->BitmapRef.SetBitmap(NodeCopy->m_ShadowBitmap);		

			UINT32 startTrans = GetTransp();
					
			NodeCopy->m_pBMPTransFill->SetStartTransp(&startTrans);

			UINT32 endTrans = 255;

			NodeCopy->m_pBMPTransFill->SetEndTransp(&endTrans);	
		}
	}

	// DocRect dr = NodeCopy->GetBoundingRect();

	// Make a copy of our internal state.
	NodeCopy->SetDarkness(GetDarkness());
	NodeCopy->SetBiasGain(GetBiasGain());

	// Remember to copy or blank cached path data without leaking memory...
	NodeCopy->m_Path.Initialise(m_Path.GetNumCoords());
	NodeCopy->m_Path.CopyPathDataFrom(&m_Path);

	NodeCopy->m_NonTranslatedPath.Initialise(m_NonTranslatedPath.GetNumCoords());
	NodeCopy->m_NonTranslatedPath.CopyPathDataFrom(&m_NonTranslatedPath);

	// Copy other member vars...
	NodeCopy->m_ShadowWidth = m_ShadowWidth;
	NodeCopy->m_ShadowHeight = m_ShadowHeight;

	NodeCopy->m_bHaveTransformed = m_bHaveTransformed;
	NodeCopy->m_RenderBitmaps = m_RenderBitmaps;
	NodeCopy->m_bAmCopying = m_bAmCopying;
	NodeCopy->m_bAmLoading = m_bAmLoading;
//	NodeCopy->m_Darkness;

	NodeCopy->m_SelectedRect = m_SelectedRect;
	NodeCopy->m_PreviousBlur = m_PreviousBlur;
	
	// DMc the bitmap shadower (not the COM shadower flat)
//	CBitmapShadow * m_pShadower;

	NodeCopy->m_BitmapXOffset = m_BitmapXOffset;
	NodeCopy->m_BiasGain = m_BiasGain;
	NodeCopy->m_LastRequestedPixWidth = m_LastRequestedPixWidth;
	NodeCopy->m_LastQualitySetting = m_LastQualitySetting;
	NodeCopy->m_LastActualPixWidth = m_LastActualPixWidth;

	// Copy from the base class
	NodeRenderableInk::CopyNodeContents(NodeCopy);
}



/***********************************************************************************************
>   void NodeShadow::PolyCopyNodeContents(NodeRenderable* pNodeCopy)

    Author:     Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
    Created:    18/12/2003
    Outputs:    -
    Purpose:    Polymorphically copies the contents of this node to another
	Errors:     An assertion failure will occur if NodeCopy is NULL
    Scope:      protected
								     
***********************************************************************************************/

void NodeShadow::PolyCopyNodeContents(NodeRenderable* pNodeCopy)
{
	ENSURE(pNodeCopy, "Trying to copy a node's contents into a NULL node");
	ENSURE(IS_A(pNodeCopy, NodeShadow), "PolyCopyNodeContents given wrong dest node type");

	if (IS_A(pNodeCopy, NodeShadow))
		CopyNodeContents((NodeShadow*)pNodeCopy);
}



/***********************************************************************************************

>	void NodeShadow::ShowDebugTreeDetails() const

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/11/94
	Purpose:	Displays debugging info of the tree
	SeeAlso:	NodeRenderableInk::ShowDebugTreeDetails

***********************************************************************************************/
void NodeShadow::ShowDebugTreeDetails() const
{                     
 	// Display a bit of debugging info
	// For now, we will just call the base class version
	TRACEALL( _T("NodeShadow  ") );
#ifdef _DEBUG
	NodeRenderableInk::ShowDebugTreeDetails();	
#endif
}



/********************************************************************************************

>	void NodeShadow::GetDebugDetails( StringBase* Str )

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/11/94
	Outputs:	Str: String giving debug info about the node
	Purpose:	For obtaining debug information about the Node (for

********************************************************************************************/
void NodeShadow::GetDebugDetails( StringBase* Str )
{
	// Call base class
#ifdef _DEBUG
	if (Str)
	{
		NodeRenderableInk::GetDebugDetails( Str );
		
		String_256 TempStr;
		String_256 TempStr2;
		
		(*Str) += TEXT( "\r\nNode Shadow Data Dump\r\n" );
		
		DocRect BlobRect = GetBlobBoundingRect();
		TempStr._MakeMsg( TEXT("Blob Bounding Rect :\r\n\t#1%ld,\t#2%ld\r\n\t#3%ld,\t#4%ld\r\n"),
			BlobRect.lo.x, BlobRect.lo.y, BlobRect.hi.x, BlobRect.hi.y );
		(*Str) += TempStr;
	}
#endif
}



/***********************************************************************************************

>	BOOL NodeShadow::SetUpShape()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/11/94
	Inputs:		- 
	Returns:	TRUE if the shape was init'ed ok, FALSE otherwise
	Purpose:	To initialise the paths used by the shape into a state that can be used,
				by allocating memory, setting up member variables properly.

***********************************************************************************************/
BOOL NodeShadow::SetUpShape()
{
	return TRUE;
}



/********************************************************************************************

>	virtual void NodeShadow::Transform( TransformBase& Trans )

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/11/94
	Inputs:		Trans - The transform Object
	Purpose:	Transforms the shape.
	SeeAlso:	NodeRenderableInk::Transform()

********************************************************************************************/
void NodeShadow::Transform( TransformBase& Trans )
{
	NodeShadowController* pControl = (NodeShadowController*)GetParentController();
	if (pControl && pControl->IsCapturingChildren())
	{
//		TransformChildren(Trans);
		return;
	}

	if (m_pBMPTransFill)
	{
		Trans.Transform(m_pBMPTransFill->GetStartPoint(), 1);
		Trans.Transform(m_pBMPTransFill->GetEndPoint(), 1);
		Trans.Transform(m_pBMPTransFill->GetEndPoint2(), 1);
		Trans.Transform(m_pBMPTransFill->GetEndPoint3(), 1);
	}

	// transform my shape
	Trans.Transform(m_Path.GetCoordArray(), m_Path.GetNumCoords());
	Trans.Transform(m_NonTranslatedPath.GetCoordArray(), m_NonTranslatedPath.GetNumCoords());

	InvalidateBoundingRect();

	// transform the selection rect

	// check for a change in aspect
	if (Trans.IsTranslation())
	{
		if (pControl != NULL)
		{
			if (pControl->GetShadowType() == SHADOWTYPE_GLOW)
			{
				Trans.Transform(&m_SelectedRect.lo, 1);
				Trans.Transform(&m_SelectedRect.hi, 1);
			}
			else
			{
//	Karim 19/10/2000
//	Chris' last changes to this method amount to commenting out this line.
//					pControl->RegenerateNode(NULL);
			}
		}
	}
	else
	{
		// force a redraw of myself to stop rendering glitches
		Document * pDoc = Document::GetCurrent();

		if (pDoc)
		{
			DocRect dr(0,0,0,0);
			m_Path.GetTrueBoundingRect(&dr);
			pDoc->ForceRedraw(FindParentSpread(), dr, FALSE, this, FALSE);
		}
	}

	// transform my children
	TransformChildren(Trans);
}



/***********************************************************************************************

>	void NodeShadow::Render(RenderRegion* pRender)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/11/94
	Inputs:		Pointer to a render region
	Purpose:	Will build a path for the shape and draw it to the given render region.

***********************************************************************************************/
void NodeShadow::Render(RenderRegion* pRender)
{
//#pragma message( __LOCMSG__ "NodeShadow::Render - do nothing" )
//	TRACE( _T("Warning - NodeShadow::Render called\n") );
	m_bHaveTransformed = FALSE;
	m_RenderBitmaps    = FALSE;

	// check for us having something to render or not
	NodeShadowController* pController = (NodeShadowController*)GetParentController();

	if (pController)
	{
#ifdef NEW_SHADOW_RENDER
		// If Controller has Effect Attributes applied
		// Then the Shadow Node must not render anything - it's RenderShadow
		// function will be called at the right time instead
		if (pController->IsCapturingChildren())
		{
//			TRACEUSER( "Gerry", _T("Render# Controller is capturing\n"));
			// Normally we allow the ShadowController to render us when it wants to
			// But for hit detection we must render ourselves (aswell)
			//		or when render quality is outline
			if (pRender->IsHitDetect() || pRender->RRQuality.GetFillQuality() <= Quality::Solid)
			{
//				TRACEUSER( "Gerry", _T("Render# Hit detect or outline\n"));
				RenderShadow(pRender);
			}
			return;
		}
#endif
		// if my inside bounding rect is empty (i.e. the bounding rect of all
		// the objects to be shadowed) then do nothing
		if (pController->GetInsideBoundingRect().IsEmpty())
		{
//			TRACEUSER( "Gerry", _T("Render# Inside bounding rect is empty\n"));
			return;
		}
	}

	if (!RenderBitmapForDisplay(pRender))
	{
//		TRACEUSER( "Gerry", _T("Render# RenderBitmapForDisplay failed\n"));
		return;
	}

	// check the quality setting - if it's too low, then we need to render a shadow outline.
	enum Quality::Fill CurrentQuality = pRender->RRQuality.GetFillQuality();
	if (CurrentQuality >= (enum Quality::Fill)Quality::Solid)
	{
//		TRACEUSER( "Gerry", _T("Render# Calling RenderBitmap\n"));
		RenderBitmap(pRender);
	}
	else
	{
		// outlines only - we'll just render the shadow's bounding rect.
//		TRACEUSER( "Gerry", _T("Render# Rendering bounding rect\n"));
		DocRect drBounds = GetBoundingRect();
		pRender->SaveContext();
		pRender->SetFillColour(COLOUR_NONE);
		pRender->SetLineColour(COLOUR_BLACK);
		pRender->DrawRect(&drBounds);
		pRender->RestoreContext();
	}

	// get the view out of the renderer
	// and reset its rendering clock
	View * pView = pRender->GetRenderView();

	if (pView)
	{
		if (pView->IS_KIND_OF(DocView))
		{
			((DocView *)pView)->ResetRenderingStartTime();
		}
	}
}  



/********************************************************************************************

>	void NodeShadow::RenderEorDrag( RenderRegion* pRender )

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/11/94
	Inputs:		pRender - A Pointer to the current RenderRegion
	Purpose:	Renders a version of the shape for EORed dragging.
	SeeAlso:	NodeRenderableInk::RenderEorDrag

********************************************************************************************/
void NodeShadow::RenderEorDrag( RenderRegion* pRender )
{
	// outlines only - we'll just render the shadow's bounding rect.
	DocRect drBounds = GetBoundingRect();
//	pRender->SaveContext();
//	pRender->SetFillColour(COLOUR_NONE);
//	pRender->SetLineColour(COLOUR_BLACK);
	pRender->DrawRect(&drBounds);
//	pRender->RestoreContext();
}



/********************************************************************************************

>	void NodeShadow::RenderObjectBlobs(RenderRegion* pRender)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/11/94
	Inputs:		pRender - the region to render the blobs to
	Purpose:	Renders the Object blobs for a NodeRegularShape
	SeeAlso:	BlobManager

********************************************************************************************/
void NodeShadow::RenderObjectBlobs(RenderRegion* pRender)
{
}



/********************************************************************************************

>	void NodeShadow::RenderTinyBlobs(RenderRegion* pRender)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/11/94
	Inputs:		pRender - the region to render the blobs to
	Purpose:	Renders the Tiny blobs for a NodeRegularShape
	SeeAlso:	BlobManager

********************************************************************************************/
void NodeShadow::RenderTinyBlobs(RenderRegion* pRender)
{
	pRender->SaveContext();
	pRender->SetLineColour(COLOUR_NONE);
	pRender->SetFillColour(COLOUR_UNSELECTEDBLOB);

	// Render the blobs on the path
	BlobManager* BlobMgr = GetApplication()->GetBlobManager();

//	DocRect dr = m_Path.GetBoundingRect(); MRH 19/5/00
	DocRect dr(0,0,0,0);
	m_Path.GetTrueBoundingRect(&dr);

	DocCoord Coord(dr.lo.x + (BlobMgr->GetBlobSize() / 2), 
		dr.hi.y - (BlobMgr->GetBlobSize() / 2));
	pRender->DrawBlob(Coord, BT_UNSELECTED);
	pRender->RestoreContext();
}



/********************************************************************************************

>	DocRect NodeShadow::GetBoundingRect(BOOL DontUseAttrs = FALSE, BOOL HitTest = FALSE)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/06/2000
	Inputs:		DontUseAttrs		TRUE if we should take node attributes into account.
				HitTest				TRUE if we're being called during a hit-test.

	Returns:	This node's bounding rect.

	Purpose:	If the bounding rect has been marked as invalid, it is recalculated,
				otherwise a cached bounding rectangle is returned.

	Errors:		ERROR3 if we find an unrecognised shadow type.

********************************************************************************************/
DocRect NodeShadow::GetBoundingRect(BOOL DontUseAttrs, BOOL HitTest)
{
	// is our cached bounding rect invalid?
	if (!IsBoundingRectValid)
	{
		DocRect drBounds(0, 0, 0, 0);

		// if our shadow path is valid, then just try to use its bounding rect.
		// This will give us a perfect bounds rect so cache it of appropriate
		if (m_Path.GetNumCoords() != 0)
		{
			if (m_Path.GetTrueBoundingRect(&drBounds) && !drBounds.IsEmpty())
			{
				// if we're taking attributes into account then cache it.
				if (!DontUseAttrs)
				{
					// update our cached bounding rect.
					BoundingRectangle	= drBounds;
					IsBoundingRectValid	= TRUE;
				}

				return drBounds;
			}
		}

		// if that didn't work, then we'll use the lengthier method of finding bounds
		// for our next-siblings, and then transforming them appropriately.
		// However, we wont cache this result as it can be a bit small 
		// which causes problems when printing

		// Get bounds of the ink node at the bottom of the shadow stack
		NodeShadowController* pControl = (NodeShadowController*)GetParentController();
		if (pControl)
		{
			NodeShadowController* pBottomControl = pControl->FindBottomShadowController();
			NodeRenderableInk* pSibling = pBottomControl->GetInkNodeFromController();
			if (pSibling)
				drBounds = drBounds.Union(pSibling->GetBoundingRect(DontUseAttrs, HitTest));
		}

		// a different transformation is required, according to our shadow type.
		if (pControl != NULL)
		{
			switch (pControl->GetShadowType())
			{
			case SHADOWTYPE_WALL:
				drBounds.Translate(pControl->GetOffsetX(), pControl->GetOffsetY());
				drBounds.Inflate(pControl->GetPenumbraWidth());
				break;

			case SHADOWTYPE_GLOW:
				drBounds.Inflate(pControl->GetGlowWidth());
				drBounds.Inflate(pControl->GetPenumbraWidth());
				break;

			case SHADOWTYPE_FLOOR:
				{
					DocCoord dcControlVector = pControl->GetFloorShadowVector();

					drBounds.hi.y = drBounds.lo.y + dcControlVector.y;
					drBounds.hi.y += pControl->GetPenumbraWidth();

					if (dcControlVector.x > 0)
					{
						drBounds.hi.x += dcControlVector.x;
						drBounds.hi.x += pControl->GetPenumbraWidth();
					}
					else
					{
						drBounds.lo.x += dcControlVector.x;
						drBounds.lo.x -= pControl->GetPenumbraWidth();
					}
				}
				break;

			case SHADOWTYPE_FEATHER:
				drBounds.Inflate(pControl->GetFeatherWidth());	// TODO: /2?
				drBounds.Inflate(pControl->GetPenumbraWidth());
				break;

			default:
				ERROR3("NodeShadow::GetBoundingRect; unknown parent shadow type!");
				break;
			}
		}

		return drBounds;
	}

	return BoundingRectangle;
}



/********************************************************************************************

>	DocRect NodeShadow::GetBlobBoundingRect()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/11/94
	Returns:	DocRect - Returns the bounding rect of the shape and its blobs
	Purpose:	This calculates the bounding box of the shape's path and adds in the
				influence of the selection blobs. It does not consider if the blobs are
				visible or not, it just gives the bounding box that they would occupy if
				they were visible

********************************************************************************************/
DocRect NodeShadow::GetBlobBoundingRect()
{
	// Find the Shapes bounding rectangle
	DocRect Rect = GetBoundingRect();

	BlobManager * pBlobMgr = GetApplication()->GetBlobManager();

	Rect.lo.x -= pBlobMgr->GetBlobSize();
	Rect.lo.y -= pBlobMgr->GetBlobSize();
	Rect.hi.x += pBlobMgr->GetBlobSize();
	Rect.hi.y += pBlobMgr->GetBlobSize();

	// Make sure we include the Bounds of our children
	IncludeChildrensBoundingRects(&Rect);

	return Rect;
}

/********************************************************************************************

>	virtual UINT32 NodeShadow::GetNodeSize() const

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/11/94
	Returns:	The size of the node in bytes 
	Purpose:	For finding the size of the node 
	SeeAlso:	Node::GetSubtreeSize

********************************************************************************************/
UINT32 NodeShadow::GetNodeSize() const 
{     
	return (sizeof(NodeShadow)); 
}  

/********************************************************************************************

>	BOOL NodeShadow::OnClick( DocCoord PointerPos, ClickType Click, ClickModifiers ClickMods,
												Spread* pSpread)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/11/94
	Inputs:		PointerPos - The Location of the mouse pointer at the time of the click
				Click - The type of click received (single, double, drag etc)
				ClickMods - The modifiers to the click (eg shift, control etc )
	Returns:	BOOL - TRUE if the node claims the click as its own and FALSE if it is
				not interested in the click
	Purpose:	Allows the QuickShape to respond to clicks by selecting its blobs or starting
				drags etc.

********************************************************************************************/

BOOL NodeShadow::OnClick( DocCoord PointerPos, ClickType Click, ClickModifiers ClickMods,
														Spread* pSpread)
{
	// did not use the click
	return FALSE;
}

/********************************************************************************************

>	virtual BOOL NodeShadow::CanBecomeA(BecomeA* pBecomeA)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/11/94
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
BOOL NodeShadow::CanBecomeA(BecomeA* pBecomeA)
{
	// The NodeRegularShape can become a NodePath
	if (pBecomeA->BAPath())
	{
		pBecomeA->AddCount(1);

		return TRUE;
	}

	return FALSE;
}



/********************************************************************************************

>	virtual BOOL NodeShadow::DoBecomeA(BecomeA* pBecomeA) 

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/11/94
	Inputs:		pBecomeA = 	ptr to a class that contains all the info needed to become a new
							type of node.
	Outputs:	-
	Returns:	TRUE if the object has been transformed, FALSE if we run out of memory
	Purpose:	Transforms the object into another type of object. 
	SeeAlso:	NodeRegularShape::CanBecomeA

********************************************************************************************/
BOOL NodeShadow::DoBecomeA(BecomeA* pBecomeA)
{
	// Check for a NULL entry param
	ERROR2IF(pBecomeA == NULL,FALSE,"pBecomeA is NULL");

	// This lump checks that the Reason is one that we understand
	// It also makes sure that we don't have a NULL UndoOp ptr
	BOOL ValidReason = (pBecomeA->GetReason() == BECOMEA_REPLACE || pBecomeA->GetReason() == BECOMEA_PASSBACK);
	ERROR2IF_PF(!ValidReason,FALSE,("Unkown BecomeA reason %d",pBecomeA->GetReason()));

	// New behaviour: Leave shadow in the tree if the BecomeA claims
	// it will leave the result node in position in the tree
	if (pBecomeA->ResultsStayInPlace() && pBecomeA->IsCombineBecomeA())
	{
		return TRUE;
	}

	Node * pAttrNode = NULL;
	BitmapTranspFillAttribute	* pTranspBitmapFill			= NULL;

	NodeAttribute			* pAttr = NULL;

	BOOL bInsertNew = TRUE;				// By default we will insert a new node
	if (RenderBitmapForDisplay())
		ApplyProfileToShadowBitmap();
	else
		bInsertNew = FALSE;				// Failed to generate the shadow bitmap

	// if we're replacing we need to create a normal node path
	if (pBecomeA->GetReason() == BECOMEA_REPLACE)
	{
		DocRect dr(0, 0, 0, 0);
		m_Path.GetTrueBoundingRect(&dr);
		DocCoord dc1(dr.lo.x, dr.lo.y);
		DocCoord dc2(dr.hi.x, dr.lo.y);
		DocCoord dc3(dr.lo.x, dr.hi.y);

		UINT32 startTrans = GetTransp();
		UINT32 endTrans = 255;

		DocColour TransparentColour(COLOUR_NONE);
		AttrStrokeColour * pAttrStrokeColour = NULL;

		CCAttrMap * pAttrMap = NULL;

		// Get the current Transparency value to use!
		UndoableOperation* pUndoOp = pBecomeA->GetUndoOp();
		
		// We need to make sure that all the groups we may have produced have their attrs correctly
		// factored out.  This is so the final factoring out done by the NodeBlend node will be able
		// to complete the job.
		// Without this step, leaf nodes can have common child attrs that should be factored up the
		// to a higher level.
		Node* pNode = this->FindNext();
		while (pNode != NULL)
		{
			if (IS_A(pNode,NodeGroup))
			{
				if (pUndoOp)
				{
					if (!pUndoOp->DoFactorOutCommonChildAttributes((NodeGroup*)pNode))
						return FALSE;
				}
				else
				{
					if (!((NodeGroup*)pNode)->FactorOutCommonChildAttributes())
						return FALSE;
				}
			}
			
			pNode = pNode->FindNext();
		}					
		
		if (bInsertNew)
		{
			// Make a new node to put the shadow bitmap into
			NodePath* pNodePath;
			ALLOC_WITH_FAIL(pNodePath, (new NodePath), pUndoOp); 
			if (pNodePath == NULL)
				return FALSE;
			
			// apply the attribute map
			pAttrMap = CCAttrMap::MakeAppliedAttrMap(this);
			pNodePath->ApplyAttributes(pAttrMap, TRUE);
			delete pAttrMap;
			
			// remove all the appropriate attributes from this node
			pAttrNode = pNodePath->FindFirstChild();
			
			while (pAttrNode)
			{
				if (pAttrNode->IsAnAttribute())
				{
					pAttr = (NodeAttribute *)pAttrNode;
					
					if (pAttr->IsAStrokeColour() ||
						pAttr->IsATranspFill() ||
						pAttr->IS_KIND_OF(AttrTranspFillMapping))
					{
						pAttrNode = pAttrNode->FindNext();
						pAttr->UnlinkNodeFromTree();
						delete pAttr;
					}
					else
					{
						pAttrNode = pAttrNode->FindNext();
					}
				}
				else
				{
					pAttrNode = pAttrNode->FindNext();
				}
			}
			
			// Setup the new Rect node that is to be replacing the shadow node.
			pNodePath->InkPath.Initialise();
			pNodePath->InkPath.CloneFrom(m_Path);
			pNodePath->InkPath.IsFilled = TRUE;
			pNodePath->InkPath.TryToClose();
			pNodePath->InkPath.InitialiseFlags(0, pNodePath->InkPath.GetNumCoords()-1);
			
			// set up the bitmap transparency fill
			AttrBitmapTranspFill * pAttrBitmapTranspFill = NULL;
			ALLOC_WITH_FAIL(pAttrBitmapTranspFill , (new AttrBitmapTranspFill), pUndoOp);
			
			pTranspBitmapFill = (BitmapTranspFillAttribute *)(pAttrBitmapTranspFill ->GetAttributeValue());
			
			pTranspBitmapFill->SetStartPoint(&dc1);
			pTranspBitmapFill->SetEndPoint(&dc2);
			pTranspBitmapFill->SetEndPoint2(&dc3);
			pTranspBitmapFill->SetTesselation(RT_Simple);
			pTranspBitmapFill->SetTranspType(TT_Mix);
			
			pTranspBitmapFill->SetStartTransp(&startTrans);
			pTranspBitmapFill->SetEndTransp(&endTrans);
			
			pTranspBitmapFill->AttachBitmap(m_ShadowBitmap);
			
			// attach it to the node
			pAttrBitmapTranspFill->AttachNode(pNodePath, FIRSTCHILD);
			
			// ensure the line colour is transparent
			ALLOC_WITH_FAIL(pAttrStrokeColour, (new AttrStrokeColour), pUndoOp);
			
			pAttrStrokeColour->Value.SetStartColour(&TransparentColour);
			
			pAttrStrokeColour->AttachNode(pNodePath, FIRSTCHILD);
			
			AttrTranspFillMappingLinear * pFillMapping = NULL;
			ALLOC_WITH_FAIL(pFillMapping, new AttrTranspFillMappingLinear, pUndoOp);
			pFillMapping->Value.Repeat = 1;
			
			pFillMapping->AttachNode(pNodePath, FIRSTCHILD);
			
			// Insert the NodeGroup where the NodeBlend used to be
			if (pUndoOp)
			{
				if (!pUndoOp->DoInsertNewNode(pNodePath,this,PREV,TRUE,FALSE,FALSE,TRUE))
					return FALSE;
			}
			else
			{
				pNodePath->AttachNode(this, PREV);
				pNodePath->NormaliseAttributes();
			}

			CCAttrMap ThisMap;
			CCAttrMap* pThisMap = NULL;
			if (pNodePath->FindAppliedAttributes(&ThisMap))
				pThisMap = ThisMap.Copy();
			pBecomeA->PassBack(pNodePath, this, pThisMap);
		}
			
		if (pUndoOp)
		{
			NodeHidden* pHiddenNodeShadower = NULL;
			
			// hide this node
			if (!pUndoOp->DoHideNode(this, TRUE, &pHiddenNodeShadower, TRUE))
				return FALSE;
		}
		else
		{
			CascadeDelete();
			delete this;
		}
	}
	else
	{
		return TRUE;
	}
	return TRUE;
}



/********************************************************************************************

>	NodePath * NodeShadow::CreateShadowPassbackNode();

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/11/96
	Inputs:		
	Returns:	NULL for failure, otherwise the new shadow passback node
	Purpose:	Creates the shadow passback node for the DoBeomeA - BECOMEA_PASSBACK 
				mechanism
	SeeAlso:	NodeShadow::DoBecomeA

********************************************************************************************/
NodePath * NodeShadow::CreateShadowPassbackNode()
{
	NodePath *pPassbackNode = new NodePath();

	ERRORIF(pPassbackNode == NULL, _R(IDE_NOMORE_MEMORY), NULL);

	pPassbackNode->InkPath.Initialise();

	NodeRenderableInk * pInk = (NodeRenderableInk *)FindNext(CC_RUNTIME_CLASS(NodeRenderableInk));

	NodeShadowBecomeA MyBecomeA(BECOMEA_PASSBACK, CC_RUNTIME_CLASS(NodePath), NULL,
		FALSE, &(pPassbackNode->InkPath));

	while (pInk)
	{
		if (pInk->CanBecomeA(&MyBecomeA))
		{
			pInk->DoBecomeA(&MyBecomeA);
		}

		pInk = (NodeRenderableInk *)pInk->FindNext(CC_RUNTIME_CLASS(NodeRenderableInk));
	}

	pPassbackNode->InkPath.InitialiseFlags();

	return pPassbackNode;
}



/***********************************************************************************************

>	BOOL NodeShadow::RenderBitmapForDisplay(RenderRegion* pRegion, List* pList = NULL)

    Author:     David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
    Created:    19/4/2000
	Inputs:     pRegion		the render region to take the values from
				pList		list of SumAllPathsElem's - if non-NULL, then we'll use
									this list as our source of paths for shadowing.
    Outputs:    
    Returns:    TRUE 	- success
    			FALSE	- failure
    			                                  		 
    Purpose:    Renders the shadow bitmap to the correct dpi

	Notes:		Karim 03/07/2000
				To use the old shadow rendering code, comment out the USE_NEW_SHADOW_CODE
				define up top. Note that as time progresses, the odds on something or other
				breaking the old code do grow.

				Also, sorry about lumping everything into one function - at some point,
				this function should probably be hacked apart into several smaller, more
				manageable pieces.

	Errors:     -   
	Scope:      protected
	       
**********************************************************************************************/
BOOL NodeShadow::RenderBitmapForDisplay(RenderRegion* pRegion, List* pList)
{
//#pragma message( __LOCMSG__ "NodeShadow::RenderBitmapForDisplay - do nothing" )
//	TRACE( _T("Warning - NodeShadow::RenderBitmapForDisplay called\n") );
//	return false;
	// the shadow bitmap is cached, using the last scaled pixel width and quality settings.
	MILLIPOINT CurrentPixWidth = pRegion->GetScaledPixelWidth();
	enum Quality::Fill CurrentQuality = pRegion->RRQuality.GetFillQuality();

	if (m_ShadowBitmap != NULL)
		if (m_LastRequestedPixWidth	== CurrentPixWidth &&
			m_LastQualitySetting	== CurrentQuality)
		{
//			TRACEUSER( "Gerry", _T("RenderBitmapForDisplay# Same as last\n"));
			return TRUE;
		}

	// no need to regenerate the bitmap if quality is below a certain level.
	if (CurrentQuality < (enum Quality::Fill)Quality::Solid)
	{
//		TRACEUSER( "Gerry", _T("RenderBitmapForDisplay# Quality too low\n"));
		return TRUE;
	}

	// we can only regenerate the bitmap if we're given a GRenderRegion.
	// don't return a failure code, as RRs such as ScanningRR may need to dummy-render us.
	if (!pRegion->IS_KIND_OF(GRenderRegion))
	{
//		TRACEUSER( "Gerry", _T("RenderBitmapForDisplay# Not a GRenderRegion\n"));
		return TRUE;
	}

	// Bodge for effects that might change their bounds...
	// Floor shadows position themselves based on the bounding rect of the shadowed node
	// But the shadowed node might be an Effect that hasn't been generated yet and so
	// still has expanded bounds...
	// So we must ensure that the effect is rendered and knows its bounds before
	// we create and render the shadow
	// Note that this is not a huge overhead because the effect processing would have
	// taken place inside GetSilhouette bitmap anyway. Instead that function will get
	// the cached results of this render.
	NodeShadowController* pController = (NodeShadowController*)GetParentController();
	if (pController && pController->GetShadowType()==SHADOWTYPE_FLOOR)
	{
		// Check whether Ink node is an effect without a processed bitmap
		NodeRenderableInk* pInk = pController->GetInkNodeFromController();
		if (pInk && pInk->IsEffect() && ((NodeEffect*)pInk)->IsBitmapEffect())
		{
			NodeBitmapEffect* pEffect = (NodeBitmapEffect*)pInk;
			BOOL bOK = pEffect->GetProcessedBitmap(FALSE, NULL, NULL, NULL);
			if (!bOK)
			{
				pRegion->SaveContext();

				// Direct capture and ignore the bitmap handed back to us
				// so that we avoid creating unused capture bitmap
				CaptureFlags caFlags = CaptureFlags(cfLOCKEDTRANSPARENT | cfUNCLIP | cfALLOWDIRECT | cfPIXWIDTHSCALE);
				DocRect CaptureRect = pEffect->GetBoundingRect();
				pRegion->StartCapture(this, CaptureRect, CAPTUREINFO(ctNESTABLE, caFlags), TRUE, FALSE, 0, pEffect);
				pRegion->RenderTree(pEffect, FALSE);
				pRegion->StopCapture(this, FALSE, TRUE);

				pRegion->RestoreContext();
			}
		}
	}

	// BitmapShadower does all the silhouette creation work, and exposes methods
	// which return useful shadow info, as well as the silhouette bitmap.
	BitmapShadower* pBmpShadower = BitmapShadower::CreateSilhouetteBitmap((NodeShadowController*)GetParentController(), (GRenderRegion*)pRegion, pList);
	if (pBmpShadower == NULL)
	{
//		TRACEUSER( "Gerry", _T("RenderBitmapForDisplay# CreateSilhouetteBitmap failed\n"));
		return FALSE;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	// Post-process the 32bpp 'silhouette' bitmap, to get the 8bpp greyscale shadow bitmap.
	/////////////////////////////////////////////////////////////////////////////////////////

	// initialise the transparency fill attribute and clear our caches.
	SetupTransFill();

	// get the silhouette bitmap out of the shadower.
	KernelBitmap* pSilhouetteBmp = pBmpShadower->GetSilhouetteBitmap();

	// generate the shadow bitmap for the required blur diameter.
	const double pixBlurDiameter = pBmpShadower->GetPixBlurDiameter();
	BOOL bOK = GenerateShadowBitmap(pSilhouetteBmp, pixBlurDiameter);
	if (bOK)
	{
	// if desired, attach a debug copy of the final shadow bitmap within the bitmap gallery.
#ifdef ATTACH_SHADOW_BMP
		m_ShadowBitmap->AttachDebugCopyToCurrentDocument("Shadow Final Bitmap");
#endif

//		TRACEUSER( "Gerry", _T("RenderBitmapForDisplay# GenerateShadowBitmap ok\n"));

		// ok, we're done! now remember the requested and actual pix-per-inch settings
		// at which we have just successfully created the new shadow bitmap.
		m_LastRequestedPixWidth	= CurrentPixWidth;
		m_LastQualitySetting	= CurrentQuality;
		m_LastActualPixWidth = (CurrentPixWidth > 0) ? pBmpShadower->GetActualPixelWidth() : -1;

		// one last thing - create the path which the shadow bitmap-transparency is drawn into.
		DocRect drBounds = pBmpShadower->GetBitmapBounds();
		CreateBitmapPath(drBounds, m_LastActualPixWidth, pixBlurDiameter);

		// free up the shadower, now we've finished using it.
		delete pBmpShadower;
		pBmpShadower = NULL;
	}
//	else
//	{
//		TRACEUSER( "Gerry", _T("RenderBitmapForDisplay# GenerateShadowBitmap failed\n"));
//	}

	if (pSilhouetteBmp)
	{
//		TRACE( _T("Warning - NodeShadow::RenderBitmapForDisplay not deleting bitmap\n") );
		delete pSilhouetteBmp;
		pSilhouetteBmp = NULL;
	}

	return bOK;
}



/********************************************************************************************

>	BOOL NodeShadow::RenderBitmapForDisplay()

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/10/2000

	Outputs:	Our shadow bitmap and path will be set up ready for rendering, if successful.

	Returns:	TRUE if successful, FALSE otherwise.

	Purpose:	A simpler overload of the complicated function above.
				Simpler because this function:
				*	Does not use a render-region - it queries the current view
					for render information instead.
				*	Does not do any caching - providing we don't fail, we _will_ render
					a new shadow whenever you call us, so do NOT use with abandon!

	Notes:		In order to call this function, we must be installed in the tree. Somewhere
				deep in the innards of this routine, we need to access the Spread, in order
				to get the rendering origin right.

	See also:	RenderBitmapForDisplay above, which is more fully featured and should be the
				function called in normal use.

********************************************************************************************/
BOOL NodeShadow::RenderBitmapForDisplay()
{
//#pragma message( __LOCMSG__ "NodeShadow::RenderBitmapForDisplay - do nothing" )
//	TRACE( _T("Warning - NodeShadow::RenderBitmapForDisplay called\n") );
//	return false;
	// Create a bitmap-shadower containing the shadow silhouette.
	BitmapShadower* pBmpShadower = BitmapShadower::CreateSilhouetteBitmap((NodeShadowController*)GetParentController());
	if (pBmpShadower == NULL)
		return FALSE;

	// initialise the transparency fill attribute and clear our caches.
	SetupTransFill();

	// get the silhouette bitmap out of the shadower.
	KernelBitmap* pSilhouetteBmp = pBmpShadower->GetSilhouetteBitmap();

	// Blur & grayscale the silhouette, to get the shadow bitmap.
	const double pixBlurDiameter = pBmpShadower->GetPixBlurDiameter();
	BOOL bOK = GenerateShadowBitmap(pSilhouetteBmp, pixBlurDiameter);
	if (bOK)
	{
		// Tell the bitmap what resolution people should render it at.
		// Doesn't affect rendering - just useful if it ends up in the bitmap gallery.
		LPBITMAPINFOHEADER pShadowHeader = m_ShadowBitmap->GetBitmapInfoHeader();
		const double PixPerMetre = MILLIPOINTS_PER_INCH * INCHES_PER_METRE / pBmpShadower->GetActualPixelWidth();
		pShadowHeader->biXPelsPerMeter = pShadowHeader->biYPelsPerMeter = (INT32)(PixPerMetre + 0.5);

// attach a debug copy to the bitmap gallery, if required.
#ifdef ATTACH_SHADOW_BMP
		m_ShadowBitmap->AttachDebugCopyToCurrentDocument("Shadow Final Bitmap");
#endif

		// generate the path to hold the shadow bitmap.
		DocRect drBounds = pBmpShadower->GetBitmapBounds();
		CreateBitmapPath(drBounds, pBmpShadower->GetActualPixelWidth(), pixBlurDiameter);

		// the cached bitmap is now invalid, so mark it so via the last pixel width.
		m_LastRequestedPixWidth = -1;

		// free up our used bitmap-shadower.
		delete pBmpShadower;
		pBmpShadower = NULL;
	}

	if (pSilhouetteBmp)
	{
		delete pSilhouetteBmp;
		pSilhouetteBmp = NULL;
	}

	return bOK;
}



/********************************************************************************************

>	BOOL NodeShadow::ApplyProfileToShadowBitmap()

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/10/2000

	Outputs:	The palette for m_ShadowBitmap will be set correctly, to use the information
				from the shadow's profile control.

	Returns:	TRUE if successful, FALSE otherwise.

	Errors:		ERROR3 if m_ShadowBitmap is NULL or our profile control gives a bad result.

********************************************************************************************/
BOOL NodeShadow::ApplyProfileToShadowBitmap()
{
	if (m_ShadowBitmap == NULL)
	{
		ERROR3("NodeShadow::ApplyProfileToShadowBitmap; NULL shadow bitmap!");
		return FALSE;
	}

	LPRGBQUAD pPalette = m_ShadowBitmap->GetPaletteForBitmap();

	AFp res;

	m_BiasGain.SetBias(-m_BiasGain.GetBias());

	for (INT32 i = 0 ; i < 256; i++)
	{
		double d = ((double)i) / 255;

		res = m_BiasGain.MapZeroToOne((AFp)(d));
		res *= 255.0;

		ERROR3IF(res < 0 || res > 255,
				"NodeShadow::ApplyProfileToShadowBitmap; Bad result from profile control!");

		pPalette[i].rgbRed = pPalette[i].rgbBlue = pPalette[i].rgbGreen = (BYTE)res;
	}

	m_BiasGain.SetBias(-m_BiasGain.GetBias());

	return TRUE;
}

	
	
/***********************************************************************************************

> BOOL NodeShadow::Snap(DocCoord* pDocCoord)

    Author:     Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
    Created:    21/12/94
	Inputs:     pDocCoord	= a coord in Spread coords
    Outputs:    If the point is snapped then pDocCoord will contain the point of attraction.
    Returns:    TRUE 	- the DocCoord has been snapped to the shapes path.
    			FALSE	- the DocCoord has not been processed.
    			                                  		 
    Purpose:    Snaps to given coord to the nearest point on the shapes render path.  If it is
    			not appropriate to snap the coord to the shape (at the moment this means the
    			coord is too far awawy), then FALSE is returned.
    Errors:     -   
	Scope:      public
	       
**********************************************************************************************/
BOOL NodeShadow::Snap(DocCoord* pDocCoord)
{
	return FALSE;
}



/***********************************************************************************************

> BOOL NodeShadow::Snap(DocRect* pDocRect,const DocCoord& PrevCoord,const DocCoord& CurCoord)

    Author:     Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
    Created:    21/9/94
	Inputs:     pDocCoord	- the rectangle to snap
				StartDrag	- Start coord of drag
				EndDrag		- End coord of drag
    Outputs:    
    Returns:    TRUE 	- the DocRect been snapped to the grid.
    			FALSE	- the DocRect has not been processed.
    			                                  		 
    Purpose:    Snaps the given rect to the nearest position on the grid, preserving its width
    			and height.
    			The coords of the rect used for the snapping are determined by the PrevCoord and
				CurCoord coords supplied.  This is done to allow the user to control how a
				selection rectangle is snapped to the grid by the direction of his/her last mouse 
				movement.
				To force the bottom left hand corner of the rect to be snapped, 
				supply PrevCoord=(0,0) and CurCoord(-1,-1).
	Scope:      public
	       
**********************************************************************************************/
BOOL NodeShadow::Snap(DocRect* pDocRect,const DocCoord& PrevCoord,const DocCoord& CurCoord)
{
	return FALSE;
}



/********************************************************************************************

>	BOOL NodeShadow::SnapToCoords(DocCoord* pDocCoord)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/3/95
	Inputs:		pDocCoord - the coord to try and snap to
	Outputs:	pDocCoord - The snapped coord, if it was close enough to any of the magnetic
				points for a rectangle.
	Returns:	TRUE if it was snapped, FALSE if not
	Purpose:	Snaps the point magnetically to the the significant points of the shape

********************************************************************************************/
BOOL NodeShadow::SnapToCoords(DocCoord* pDocCoord)
{
	return FALSE;
}



/********************************************************************************************

>	virtual String NodeShadow::Describe(BOOL Plural, BOOL Verbose = TRUE)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/11/94
	Inputs:		Plural: Flag indicating if the string description should be plural or
				singular. 
	Returns:	Description of the object                                    
	Purpose:	To return a description of the NodeRegularShape object in either the singular
				or the plural. This method is called by the DescribeRange method.
				The description will always begin with a lower case letter.   
	Errors:		A resource exception will be thrown if a problem occurs when loading the 
				string resource. 

********************************************************************************************/
String NodeShadow::Describe(BOOL Plural, BOOL Verbose) 
{     
	if (Plural)
	{
		String str(_R(IDS_SHADOWNODENAME));
		str += _T("s");

		return(str);
	}
	else
		return(String(_R(IDS_SHADOWNODENAME))); 
}



/********************************************************************************************

>	virtual BOOL NodeShadow::ExportRender ( RenderRegion *pRegion )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/12/99
	Inputs:		pRegion - A pointer to a RenderRegion.

	Outputs:	After this call, our cached shadow-rendering information may have changed.

	Returns:	TRUE if we successfully ExportRender()'ed the shadow,
				FALSE otherwise.

	Purpose:	Used to output shadows into various export filters.
				Shadows are usually exported as a bitmap or a transparent bitmap.
				For more info, see the appropriate implementation of ExportShadow()
				in the filter render-regions.

	Notes:		Rewritten by Karim 24/01/2000 for clarity and to stop unnecessary rendering.

********************************************************************************************/
BOOL NodeShadow::ExportRender( RenderRegion *pRegion )
{
	PORTNOTETRACE("other","NodeShadow::ExportRender - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	enum FilterType
	{
		NONE,
		FLASH,
		CMX,
		AIEPS
	};
	FilterType ftExportFilter	=	pRegion->IS_KIND_OF(FlashRenderRegion)	? FLASH	:
									pRegion->IS_KIND_OF(CMXRenderRegion)	? CMX	:
									pRegion->IS_KIND_OF(AIEPSRenderRegion)	? AIEPS	:
									NONE;

	// Render our shadow bitmap at the export resolution.
	// Note that we choose to continue if ApplyProfileToShadowBitmap() fails.
	if (ftExportFilter != NONE)
	{
		if (!RenderBitmapForDisplay())
			return FALSE;

		ApplyProfileToShadowBitmap();
	}

	// OK, export our shadow!
	BOOL bResult = FALSE;
	switch (ftExportFilter)
	{
	// Flash export.
	case FLASH:
	{
		FlashRenderRegion	*pFlash		= static_cast<FlashRenderRegion*> ( pRegion );
		bResult = pFlash->ExportShadow(	&m_Path,
										m_ShadowBitmap->GetActualBitmap (),
										GetTransp() );
	}
	break;

	// CMX export.
	case CMX:
	{
		CMXRenderRegion		*pCMX		= static_cast<CMXRenderRegion*> ( pRegion );
		bResult = pCMX->ExportShadow(	m_ShadowBitmap->GetActualBitmap (),
										GetTransp(),
										GetBoundingRect () );
	}
	break;

	// AI EPS export.
	case AIEPS:
	{
		AIEPSRenderRegion	*pAIEPS		= static_cast<AIEPSRenderRegion*> ( pRegion );
		bResult = pAIEPS->ExportShadow(	m_ShadowBitmap->GetActualBitmap (),
										GetTransp(),
										GetBoundingRect () );
	}
	break;

	// Unrecognised filter.
	default:
		bResult = FALSE;
		break;
	}

	return bResult;
#else
	return FALSE;
#endif
}



/********************************************************************************************

>	BOOL NodeShadow::WritePreChildrenNative(BaseCamelotFilter * pFilter)
	BOOL NodeShadow::WritePreChildrenWeb(BaseCamelotFilter * pFilter)

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/06/96
	Inputs:		pFilter - filter to use
	Returns:	Boolean value indicating success
	Purpose:	Writes this regular shape to the filter
				
				NodeShadows do NOT save anything out in the current version of Camelot.

				For now though, do save out just the place holder node

	Notes:		Karim 06/02/2001
				Added code to write out the shadow Darkness.

********************************************************************************************/
BOOL NodeShadow::WritePreChildrenNative(BaseCamelotFilter * pFilter)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter == NULL,FALSE,"NULL filter param");
	BOOL ok = TRUE;
	
	CXaraFileRecord Rec(TAG_SHADOW, TAG_SHADOW_SIZE);

	INT32 Blur = 0;

	Rec.Init();
	if (ok) ok = Rec.WriteDOUBLE((double)m_BiasGain.GetBias());
	if (ok) ok = Rec.WriteDOUBLE((double)m_BiasGain.GetGain());
	if (ok) ok = Rec.WriteDOUBLE(GetDarkness());
	if (ok) ok = pFilter->Write(&Rec);

	return ok;
#else
	return FALSE;
#endif
}



BOOL NodeShadow::WritePreChildrenWeb(BaseCamelotFilter * pFilter)
{
	return WritePreChildrenNative(pFilter);
}



/********************************************************************************************

>	virtual BOOL NodeShadow::CanWriteChildrenWeb(BaseCamelotFilter* pFilter)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/12/97
	Inputs:		pFilter = ptr to the filter to write to
	Returns:	TRUE if children can be written automatically, FALSE otherwise
	Purpose:	This allows a node to stop the filter from writing out the child nodes automatically.

				NodeShadows do NOT save anything out in the current version of Camelot.
				Therefore, none of it's children can be saved out either.

				For now though, do save the childern

********************************************************************************************/
BOOL NodeShadow::CanWriteChildrenWeb(BaseCamelotFilter* pFilter)
{
	return TRUE;
}



//------------------------------------------------------------------
// See comments for NodeMoulder::CanWriteChildrenNative(BaseCamelotFilter* pFilter)
BOOL NodeShadow::CanWriteChildrenNative(BaseCamelotFilter* pFilter)
{
	return CanWriteChildrenWeb(pFilter);
}



/*******************************************************************************************

>	void NodeShadow::PreExportRender(RenderRegion* pRegion)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/2/95
	Inputs:		pRegion - points to the export render region
	Outputs:	-
	Returns:	-
	Purpose:	This function is called just before the shape is exported.  It outputs the 
				shapes start token
	Errors:		-
	SeeAlso:	NodeRegularShape::ExportRender

********************************************************************************************/
void NodeShadow::PreExportRender(RenderRegion* pRegion)
{
}



/********************************************************************************************

>	virtual BOOL NodeShadow::ReleaseCached( BOOL bAndParents = TRUE,
											BOOL bAndChildren = TRUE,
											BOOL bSelf = TRUE,
											BOOL bAndDerived = TRUE)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/12/2004
	Returns:	TRUE if captured data was cached
	Purpose:	Protected Helper function
				Use the CaptureManager to capture the results of rendering, cache them
				and associate them with this Ink node
	SeeAlso:	NodeRenderableInk::RenderCached, CaptureCached

********************************************************************************************/

BOOL NodeShadow::ReleaseCached(BOOL bAndParents, BOOL bAndChildren, BOOL bSelf, BOOL bAndDerived)
{
	// Don't release cached data if the release request is the result of transformation
	// re-rendering because we can transform our cached data!
	if (!IsDragged() && bSelf)
	{
		DeleteCache();
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




/*******************************************************************************************
>	void NodeShadow::DeleteCache()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/12/96
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Deletes all cached shadow paths
********************************************************************************************/
void NodeShadow::DeleteCache()
{
//	TRACEUSER( "Gerry", _T("DeleteCache\n"));
	
	m_LastRequestedPixWidth = -1;

	if (m_ShadowBitmap)
	{
		delete m_ShadowBitmap;
		m_ShadowBitmap = NULL;
	}
	
	if (m_pBMPTransFill)
	{
		delete m_pBMPTransFill;
		m_pBMPTransFill = NULL;
	}

	if (m_pShadower)
	{
		delete m_pShadower;
		m_pShadower = NULL;
	}
}



/*******************************************************************************************
>	void NodeShadow::DeleteAndRefreshCache()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/12/96
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Deletes all cached shadow paths and regenerates them
********************************************************************************************/
void NodeShadow::DeleteAndRefreshCache()
{
	DeleteCache();	
}



/********************************************************************************************
>	virtual NodeCompound* NodeShadow::GetParentController() const

 	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/12/96
	Inputs:		-
	Returns:	Pointer to parent shadow controller (NULL if not found!)
	Purpose:	Returns a type correct pointer to the parent shadow controller
********************************************************************************************/
NodeCompound* NodeShadow::GetParentController() const
{
	NodeShadowController* pBob = (NodeShadowController*)FindParent();
	if (pBob == NULL || !IS_A(pBob, NodeShadowController))
	{
//		ERROR3("NodeShadow::GetParentController; cannot find NodeShadowController parent!");
		pBob = NULL;
	}

	return pBob;
}



/********************************************************************************************

>	void NodeShadow::RenderBitmap(RenderRegion* pRender)

 	Author:		Mark_Howitt (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/8/98
	Inputs:		pRender	- The render region to use for rendering
	Purpose:	Renders the shadow using the bitmap shadowing system.

********************************************************************************************/
void NodeShadow::RenderBitmap(RenderRegion* pRender)
{
	// If we don't have a trans fill then try to create one
	if (m_pBMPTransFill == NULL)
	{
//		TRACEUSER( "Gerry", _T("RenderBitmap# TransFill is NULL\n"));
		SetupTransFill();
	}

	// quit if we still have no bitmap transp fill.
	if (m_pBMPTransFill == NULL)
	{
//		TRACEUSER( "Gerry", _T("RenderBitmap# TransFill is still NULL!!! *********\n"));
		return;
	}

	// if we have no shadow bitmap, then create a dummy one.
	if (m_ShadowBitmap == NULL)
	{
//		TRACEUSER( "Gerry", _T("RenderBitmap# Creating dummy bitmap!!! ***********\n"));
		m_ShadowBitmap = new KernelBitmap(4, 4, 8, 0, TRUE);
		::memset(m_ShadowBitmap->GetBitmapBits(), 0, DIBUtil::ScanlineSize(4, 8) * 4);
		m_pBMPTransFill->BitmapRef.SetBitmap(m_ShadowBitmap);
	}

	ApplyProfileToShadowBitmap();

	// Get the current Transparency value to use!
	UINT32 startTrans = GetTransp();

	// prepare a non-repeating bitmap-transp-fill type.
	AttrTranspFillMappingLinear FillMapping;
	FillMapping.Value.Repeat = 1;

	// do the bitmap fitting.
	DocRect dr(0,0,0,0);
	m_Path.GetTrueBoundingRect(&dr);

	DocCoord Centre(dr.lo.x, dr.lo.y);
	DocCoord HiX(dr.hi.x, dr.lo.y);
	DocCoord HiY(dr.lo.x, dr.hi.y);

//	m_pBMPTransFill->SetStartPoint(&Centre);
//	m_pBMPTransFill->SetEndPoint(&HiX);
//	m_pBMPTransFill->SetEndPoint2(&HiY);
	DocCoord* pCoords = m_Path.GetCoordArray();		// These lines allow cached bitmap to be transformed
	m_pBMPTransFill->SetStartPoint(&pCoords[3]);
	m_pBMPTransFill->SetEndPoint(&pCoords[2]);
	m_pBMPTransFill->SetEndPoint2(&pCoords[0]);

	m_pBMPTransFill->SetStartTransp(&startTrans);

	// ok, do all the rendering.
	pRender->SaveContext();

	pRender->SetLineColour(COLOUR_NONE);
	m_pBMPTransFill->Render(pRender);
	FillMapping.Render(pRender);

//	DocRect BoundsRect = m_Path.GetBoundingRect();
//	TRACEUSER( "Gerry", _T("Rendering shadow = (%d, %d) - (%d, %d)\n"), BoundsRect.lox, BoundsRect.loy, BoundsRect.hix, BoundsRect.hiy);

	pRender->DrawPath(&m_Path);

	pRender->RestoreContext();
}



/********************************************************************************************

>	BOOL NodeShadow::GenerateShadow()

 	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/9/98
	Inputs:		
	Purpose:	Can be called externally to set up this node with its shadow in the correct
				place

********************************************************************************************/
BOOL NodeShadow::GenerateShadow()
{
	// find my parent & test to see if I've got anything to shadow
	NodeShadowController * pControl = (NodeShadowController *)FindParent();

	if (pControl)
	{
		if (pControl->GetInsideBoundingRect().IsEmpty())
		{
			return FALSE;
		}
	}
	else
	{
		return FALSE;
	}

	// ensure a render

	m_bHaveTransformed = TRUE;

	m_RenderBitmaps    = TRUE; 

	// regen the shape path & everything !!
	SetupTransFill();

	if (m_ShadowBitmap)
	{
		delete m_ShadowBitmap;
		m_ShadowBitmap = NULL;
	}

	ReleaseCached();

	// set these two to false, as we've already generated the shadow
	m_RenderBitmaps = FALSE;
	m_bHaveTransformed = FALSE;

	m_Path.ClearPath();

	if (pControl != NULL)
	{
		pControl->InvalidateBoundingRect(TRUE);
		InvalidateBoundingRect();
	}

	return TRUE;
}



/********************************************************************************************

>	BOOL NodeShadow::GenerateShadowForBlends(List * pList, RenderRegion * pRegion)

 	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/3/2000
	Inputs:		List of SumAllPathsElems (see blndhelp.h)
	Purpose:	Creates the shadow using the given list
				Can take place inside of a render loop

********************************************************************************************/
BOOL NodeShadow::GenerateShadowForBlends(List * pList, RenderRegion * pRegion)
{
	if (pList == NULL)
		return FALSE;
	else
		return RenderBitmapForDisplay(pRegion, pList);
}



/********************************************************************************************

>	BOOL NodeShadow::GenerateShadowBitmap(double pixBlurDiameter)

 	Author:		Mark_Howitt (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/9/98
	Inputs:		INT32 Blurr : Current value of the blurr setting
	Returns:	TRUE for Ok .. and FALSE for Not ok!
	Purpose:	Generates a Bitmap Shadow from the current Selection Bitmap with the current
				Blurr setting.

********************************************************************************************/
BOOL NodeShadow::GenerateShadowBitmap(KernelBitmap* pSilhouetteBmp, double pixBlurDiameter)
{
	if (pSilhouetteBmp == NULL)
		return FALSE;

	KernelBitmap * p8BitDIB = NULL;

	NodeShadowController * pController = (NodeShadowController*)GetParentController();
	if (pController == NULL)
		return FALSE;

	if(m_RenderBitmaps || !m_ShadowBitmap || !m_pBMPTransFill)
	{
		// If we already have a shadow bitmap, get rid of it as it`s outa data!
		if(m_ShadowBitmap != NULL)
		{
			delete m_ShadowBitmap;
			m_ShadowBitmap = NULL;
		}
		
		// Set up a new kernel shadow bitmap with the selection bitmap as the reference to shadow with
		if (!m_pShadower)
		{
			m_pShadower = new CBitmapShadow;

			if (!m_pShadower)
			{
				ERROR3("Can't create shadower");
				return FALSE;
			}
		}

		INT32 i = 0 ;
		
		CNativeSize		ShadowSize( pSilhouetteBmp->GetWidth(), pSilhouetteBmp->GetHeight() );

		// create the shadow
		switch (pController->GetShadowType())
		{
		case SHADOWTYPE_WALL:
			{
				m_pShadower->CreateShadow(TRUE,
											0,
											0,
											pixBlurDiameter,
											1.0,
											0,
											0,
											(UINT32*)pSilhouetteBmp->GetBitmapBits(),
											ShadowSize);
			}
			break;

		case SHADOWTYPE_FLOOR:
			{
				m_pShadower->CreateShadow(FALSE,
											pController->GetFloorShadowHeight(),
											pController->GetFloorShadowAngle() * 180.0 / PI,
											pixBlurDiameter,
											1.0,
											0,
											0,
											(UINT32*)pSilhouetteBmp->GetBitmapBits(),
											ShadowSize);
				
				INT32 YOffset = 0;
				m_pShadower->GetShadowOffset(&m_BitmapXOffset, &YOffset);
			}
			break;

		case SHADOWTYPE_GLOW:
			{
				m_pShadower->CreateShadow(TRUE,
											0,
											0,
											pixBlurDiameter,
											1.0,
											0,
											0,
											(UINT32*)pSilhouetteBmp->GetBitmapBits(),
											ShadowSize);
			}
			break;

		case SHADOWTYPE_FEATHER:
			{
				ERROR3("Unimplemented!");	// TODO:
			}
			break;

		default:
			ERROR3("Unknown Shadow type");
		}

		INT32 BWidth = m_pShadower->GetWidth();
		INT32 BHeight = m_pShadower->GetHeight();
		
		// floor shadows are generated as full 32 bit dibs, whereas wall shadows are
		// generated as 8 bit dibs
		if (m_pShadower->GetDepth() == 32)
		{
			m_ShadowBitmap = new KernelBitmap(BWidth, BHeight, 24, 96, TRUE);

			if (!m_ShadowBitmap)
			{
				ERROR(_R(IDE_NOMORE_MEMORY), FALSE);
	
			}
			
			// change the shadow bitmap from being a 32 bit-DIB to a 24 bit-DIB
			BYTE * SrcPtr = ((LPBYTE)m_pShadower->GetBytes()) + 3;
			BYTE * DestPtr = (LPBYTE)m_ShadowBitmap->GetBitmapBits();		
			
			INT32 XSize = DIBUtil::ScanlineSize(BWidth, 24);
			INT32 XInc  = XSize - (BWidth * 3);
			
			INT32 XPos = 0;
			
			INT32 size = BWidth * BHeight;
			for (i = 0 ; i < size; i++)
			{
				*(DestPtr + 0) = *(SrcPtr);
				*(DestPtr + 1) = *(SrcPtr);
				*(DestPtr + 2) = *(SrcPtr);
				
				XPos ++;
				if (XPos < BWidth)
				{
					DestPtr += 3;
				}
				else
				{
					XPos = 0;
					DestPtr += 3 + XInc;
				}
				
				SrcPtr += 4;
			}
			
			// change it into an 8-bit DIB, if not printing
			if (!pController->IsPrinting())
			{	
				if (Create8BitBitmap(m_ShadowBitmap, &p8BitDIB))
				{
					delete m_ShadowBitmap;
					m_ShadowBitmap = p8BitDIB;
				}
			}
		}
		else if (m_pShadower->GetDepth() == 8)
		{
			// the shadowed bitmap is already 8 bit, therefore just make it into a kernel bitmap
			m_ShadowBitmap = new KernelBitmap(BWidth, BHeight, 8, 96, TRUE);

			if (!m_ShadowBitmap)
			{
				ERROR3("Can't create shadow bitmap");
				return FALSE;
			}

			RGBQUAD * pQuad = NULL;

			pQuad = m_ShadowBitmap->GetPaletteForBitmap();

			// make the palette
			for (INT32 i = 0 ; i <= 255; i++)
			{
				pQuad[i].rgbRed = i;
				pQuad[i].rgbBlue = i;
				pQuad[i].rgbGreen = i;
			}

			// copy the bits over
			memcpy(m_ShadowBitmap->GetBitmapBits(), m_pShadower->GetBytes(),
				DIBUtil::ScanlineSize(BWidth, 8) * BHeight);
		}
		else
		{
			ERROR3("Shadower has an unrecognised bmp depth!");
		}

		// delete the shadower
		delete m_pShadower;
		m_pShadower = NULL;
	
		ERROR2IF(m_ShadowBitmap == NULL,FALSE,"Failed to create a shadow bitmap for shadowing!!!!");

		// DMc
		// New Trans Bitmap Fill Attributes
		// ah, but has the bitmap fill been initialised ?
		if (!m_pBMPTransFill)
		{
			SetupTransFill();
		}

		m_pBMPTransFill->BitmapRef.SetBitmap(m_ShadowBitmap);

		m_bHaveTransformed = TRUE;
	}

	return TRUE;
}



/********************************************************************************************

>	BOOL NodeShadow::SetupTransFill();

 	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/4/99
	Inputs:		
	Returns:	TRUE = success, FALSE = fail
	Purpose:	Sets up the transparency fill for this node

********************************************************************************************/
BOOL NodeShadow::SetupTransFill()
{
	DeleteCache();

	m_pBMPTransFill = new BitmapTranspFillAttribute;

	UINT32 EndTransp = 255;
	
	if(m_pBMPTransFill == NULL)
	{
		ERROR3("Arrhhh! Didn`t get an m_pBMPTransFill ptr for the shadow!");
	}
	else
	{
		// Always have an end transparency of 100%
		m_pBMPTransFill->SetEndTransp(&EndTransp);
		// Always make the shadow bitmap non repeating!
		m_pBMPTransFill->SetTesselation(RT_Simple);
	}

	return TRUE;
}



/********************************************************************************************

>	void NodeShadow::UpdateAfterWallOffset()

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/06/2000

	Outputs:	This NodeShadow may be translated so it is offset as defined by its parent
				NodeShdaowController. Its bounding box will also be invalidated and
				recalculated.

	Purpose:	Updates our bounds and shadow path after our NodeShadowController parent
				has been told to move its wall shadow to a new offset.

				After a wall-shadow offset, the shadow needn't be regenerated, but simply
				translated. This method performs any necessary translation, so that the
				wall shadow is where the parent NodeShadowController says it should be.

	SeeAlso:	NodeShadowController::SetWallShadowOffset();

********************************************************************************************/
void NodeShadow::UpdateAfterWallOffset()
{
	// update our path for the shadow bitmap-transparency.
	if (m_LastActualPixWidth > 0)
	{
		NodeShadowController* pControl = (NodeShadowController*)GetParentController();
		if (pControl != NULL)
		{
			m_Path.ClearPath();
			m_Path.CloneFrom(m_NonTranslatedPath);
			m_Path.IsFilled = TRUE;
			m_Path.Translate(pControl->GetWallShadowOffset());
		}
	}
	else
	{
// DEBUG:
//		TRACEUSER( "Karim", _T("NodeShadow::UpdateAfterWallOffset; Couldn't create shadow path!\n"));
	}

	// our bounds will likely be invalid after recalculating the shadow fill-path.
	InvalidateBoundingRect();
}



/********************************************************************************************

>	BOOL NodeShadow::PostImport()

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/02/2001

	Purpose:	Called immediately after this NodeShadow and all its children have been
				imported & installed in the tree, to ensure correct import of shadows
				from the old Xara X format:

				1. Overwrite our shadow transparency, from that of any non-default
					attr-transp applied to us.
				2. Delete all our attribute-children which we do not require.

********************************************************************************************/
BOOL NodeShadow::PostImport()
{
//#pragma message( __LOCMSG__ "NodeShadow::PostImport - do nothing" )
//	TRACE( _T("Warning - NodeShadow::PostImport called\n") );

	NodeAttribute* pAttrTransp;
	if (FindAppliedAttribute(CC_RUNTIME_CLASS(AttrTranspFillGeometry), &pAttrTransp) &&
		!pAttrTransp->IsADefaultAttr())
	{
		SetTransp( * ((AttrFillGeometry*)pAttrTransp)->GetStartTransp() );
	}

	Node* pThisKid = FindFirstChild();
	Node* pNextKid = NULL;
	while (pThisKid != NULL)
	{
		pNextKid = pThisKid->FindNext();

		if (pThisKid->IsAnAttribute() &&
			!RequiresAttrib((NodeAttribute*)pThisKid) )
		{
			pThisKid->CascadeDelete();
			delete pThisKid;
		}

		pThisKid = pNextKid;
	}

	return TRUE;
}



/********************************************************************************************

>	BOOL NodeShadow::PostDuplicate(UndoableOperation* pOp)


	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/4/99
	Inputs:		
	Returns:	True if successful
	Purpose:	Does post-import stuff

********************************************************************************************/
BOOL NodeShadow::PostDuplicate(UndoableOperation* pOp)
{
	// force a regeneration of the shadow
	m_bAmCopying = FALSE;

	// GenerateShadow();
	NodeRenderableInk::PostDuplicate(pOp);

	IsBoundingRectValid = FALSE;
	DocRect dr = GetBoundingRect(FALSE, FALSE);

	return TRUE;
}



/********************************************************************************************

>	BOOL NodeShadow::Create8BitBitmap(KernelBitmap * pSrcBitmap, KernelBitmap * pDestBitmap);


	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/4/99
	Inputs:		The source 24-bit greyscale bitmap
	Returns:	True if successful
	Purpose:	Returns an 8-bit bitmap out of the 24-bit greyscale bitmap

********************************************************************************************/
BOOL NodeShadow::Create8BitBitmap(KernelBitmap * pSrcBitmap, KernelBitmap ** ppDestBitmap)
{
	if (!pSrcBitmap || !ppDestBitmap)
	{
		ERROR3("No source or destination bitmap defined");
		return FALSE;
	}

	if (pSrcBitmap->GetBPP() != 24)
	{
		ERROR3("Source bitmap not 24 bit");
		return FALSE;
	}

	UINT32 Width = pSrcBitmap->GetWidth();
	UINT32 Height = pSrcBitmap->GetHeight();

	// make the width a factor of 4
	
	*ppDestBitmap = new KernelBitmap(Width, Height,	8, pSrcBitmap->GetHorizontalDPI(), TRUE);

	if (!ppDestBitmap)
		return FALSE;

	// ok, we've created both bitmaps now, so fill their bits !
	BYTE * pDest = (*ppDestBitmap)->GetActualBitmap()->GetBitmapBits();
	BYTE * pStartDest = pDest;
	
	BYTE * pSrc	 = pSrcBitmap->GetActualBitmap()->GetBitmapBits();
	BYTE * pStartSrc = pSrc;

	INT32 size = pSrcBitmap->GetWidth() * pSrcBitmap->GetHeight();

	UINT32 posX = 0;
	
	// get gavin's scanline size for 8-bit bitmaps
	UINT32 ScanlineSize24 = DIBUtil::ScanlineSize(Width, 24);
	UINT32 ScanlineSize8 = DIBUtil::ScanlineSize(Width, 8);

	// memset the destination so it's white
	memset(pDest, 0xff, ScanlineSize8 * Height);

	while (size > 0)
	{
		*pDest = *pSrc;

		pSrc += 3;
		pDest += 1;

		posX ++;

		if (posX >= Width)
		{
			pStartSrc += ScanlineSize24;
			pStartDest  += ScanlineSize8;

			pSrc = pStartSrc;
			pDest = pStartDest;

			posX = 0;
		}

		size --;
	}

	// and do their palettes
	LPRGBQUAD pTranspPalette = (*ppDestBitmap)->GetPaletteForBitmap();

	for (INT32 i = 0 ; i < 256; i++)
	{
		pTranspPalette->rgbBlue = (BYTE)i;
		pTranspPalette->rgbRed = (BYTE)i;
		pTranspPalette->rgbGreen = (BYTE)i;
		pTranspPalette->rgbReserved = 0x00;

		pTranspPalette ++;
	}

	return TRUE;
}



/********************************************************************************************

>	void NodeShadow::CreateBitmapPath(	const DocRect& drInkForShadowing,
										const double PixelWidth,
										const double pixBlurDiameter )
	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15 September 2000
	Inputs:		drInkForShadowing	the bounding rect of the object to be shadowed.
				PixelWidth			the width of an onscreen pixel, in millipoints.
				pixBlurDiameter		the exact penumbra width, in pixels.

	Outputs:	m_NonTranslatedPath	contains the path to render the shadow bmp into, but
									is centred on the object.
				m_Path				contains the true path to render the shadow bmp into,
									ie m_NonTranslatedPath with any wall offset applied.

	Purpose:	Create the path into which we will render our shadow bitmap:

				1.	Use PixelWidth together with the dimensions of our stored shadow bitmap,
					to work out the exact dimensions of the bitmap onscreen, in millipoints.
					If we get these dimensions wrong, then the bitmap will be slightly
					squashed or stretched when it is rendered.

				2.	Calculate the shadow path, using drInkForShadowing to determine its
					location onscreen, and the bounds calculated above to determine its size.

	Notes:	+	Can cope with fractional values for PixelWidth, but RenderBitmapForDisplay
				needs to be modified to send these more accurate values through. To see the
				problem this would solve, zoom to 25601%, create and examine a wall or glow
				shadow with no blur.

			+	This method could be made more robust, so that if an invalid PixelWidth is
				passed in, or our bitmap is NULL, it will estimate a shadow path, based only
				on drInkForShadowing.

********************************************************************************************/
void NodeShadow::CreateBitmapPath(	const DocRect& drInkForShadowing,
									const double PixelWidth,
									const double pixBlurDiameter )
{
	// clear our current shadow path.
	m_Path.ClearPath();
	m_NonTranslatedPath.ClearPath();

	// get a pointer to my controller node - if there is none, then exit.
	NodeShadowController* pControl = (NodeShadowController*)GetParentController();
	if (pControl == NULL)
	{
		ERROR3("NodeShadow::CreateBitmapPath; No parent controller!");
		return;
	}

	DocRect dr = drInkForShadowing;

	MILLIPOINT Width = 0;
	MILLIPOINT Height = 0;

//	double LowWidth = 0;
//	double LowHeight = 0;
//	double HighWidth = 0;
//	double HighHeight = 0;

	DocCoord dc1(0,0);
	DocCoord dc2(0,0);
	DocCoord dc3(0,0);
	DocCoord dc4(0,0);
	DocCoord Centre(0,0);

	// get the width and height of the bitmap in document coordinates.
	if (m_ShadowBitmap != NULL)
	{
		Width	= (MILLIPOINT)(0.5 + (PixelWidth * m_ShadowBitmap->GetWidth()));
		Height	= (MILLIPOINT)(0.5 + (PixelWidth * m_ShadowBitmap->GetHeight()));
	}
	else
	{
		Width	= dr.Width();
		Height	= dr.Height();

		if (pControl->GetShadowType() == SHADOWTYPE_GLOW)
		{
			MILLIPOINT ExtraGlow = pControl->GetPenumbraWidth() + pControl->GetGlowWidth();
			Width	+= ExtraGlow;
			Height	+= ExtraGlow;
		}
	}

	// different behaviours for different types of shadow.
	switch (pControl->GetShadowType())
	{
		case SHADOWTYPE_GLOW:
		case SHADOWTYPE_WALL:
		case SHADOWTYPE_FEATHER:
		{
/*
 *	Karim 04/10/2000
 *	Shadows are now applied from the bottom left, as it is much easier
 *	to understand pixel-alignment issues this way!
 *
			// Apply from the centre
			Centre = dr.Centre();

			// need to get the offset of the centre left & right in order to make
			// sure the pixel size of the bitmap in document coordinates is exactly right
			LowWidth = floor(0.5 * Width);
			LowHeight = floor(0.5 * Height);

			// calculate the rect for the bitmap
			dr.lo.x = Centre.x - (MILLIPOINT)LowWidth;
			dr.lo.y = Centre.y - (MILLIPOINT)LowHeight;
*/
			MILLIPOINT DeltaBmpSize = CBitmapShadow::GetWholePixelBlurDiameter(pixBlurDiameter) - 1;
			MILLIPOINT HalfBlurWidth = MILLIPOINT((DeltaBmpSize < 0) ? 0 : PixelWidth * (DeltaBmpSize / 2));

			dr.lo.x -= HalfBlurWidth;
			dr.lo.y -= HalfBlurWidth;

			dr.hi.x = dr.lo.x + Width;
			dr.hi.y = dr.lo.y + Height;
		}
		break;

		case SHADOWTYPE_FLOOR:
		{
			// slight bug fix
			dr.lo.x += (MILLIPOINT)(0.5 + (PixelWidth * m_BitmapXOffset));
			dr.lo.y += (MILLIPOINT)PixelWidth;

			dr.hi.x = dr.lo.x + Width;
			dr.hi.y = dr.lo.y + Height;
		}
		break;

		default:
			ERROR3("NodeShadow::CreateBitmapPath; Unrecognised shadow type!");
			break;
	}

	m_NonTranslatedPath.CreatePathFromDocRect(&dr);
	m_Path.CloneFrom(m_NonTranslatedPath);
	m_Path.IsFilled = TRUE;
	if (pControl->GetShadowType() == SHADOWTYPE_WALL)
		m_Path.Translate(pControl->GetWallShadowOffset());

	m_Path.GetTrueBoundingRect(&m_SelectedRect);
}



/********************************************************************************************

>	virtual BOOL NodeShadow::NeedsParent(Node* pClassNode) const


	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/5/99
	Inputs:		A node to do the class checking on
	Returns:	
	Purpose:	Do we need a parent of the same class as the given node ???
	See also:	NodeCompound::OnChildChange

********************************************************************************************/
BOOL NodeShadow::NeedsParent(Node* pClassNode) const
{
	if (!pClassNode)
		return TRUE;
	
	if (pClassNode->IsKindOf(CC_RUNTIME_CLASS(NodeShadowController)))
		return TRUE;

	return FALSE;
}



/********************************************************************************************

>	virtual BOOL NodeShadow::AllowOp(ObjChangeParam *pParam, BOOL SetOpPermissionState,
															 BOOL DoPreTriggerEdit)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>; Karim 20/01/2000
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
BOOL NodeShadow::AllowOp(ObjChangeParam *pParam, BOOL SetOpPermissionState,
												 BOOL DoPreTriggerEdit)
{
	TRACE( _T("Warning - NodeShadow::AllowOp called\n") );
	ERROR2IF(pParam == NULL, FALSE, "NodeShadowController::AllowOp(); NULL input(s) !!");

	// clean out the calling-child ptr, so it doesn't get passed around unintentionally.
	pParam->SetCallingChild(NULL);

	// A flag for whether or not we allow the Op.
	BOOL allowed = TRUE;

	UndoableOperation* pOp = pParam->GetOpPointer();

	// See if our parent allows this Op.
	if (allowed && Parent != NULL && pParam->GetDirection() != OBJCHANGE_CALLEDBYPARENT)
	{
		ObjChangeDirection OldDirection = pParam->GetDirection();
		pParam->SetCallingChild(this);
		pParam->SetDirection(OBJCHANGE_CALLEDBYCHILD);
		allowed = Parent->AllowOp(pParam, SetOpPermissionState, DoPreTriggerEdit);
		pParam->SetDirection(OldDirection);
	}

	// If our parent allowed the Op, then see what we think.
	if (allowed)
	{
		if (pOp)
		{
PORTNOTE("other", "Removed use of sundry Ops from NodeShadow::AllowOp")
#if !defined(EXCLUDE_FROM_XARALX)
			// Shadows can't be moulded.
			if ((pOp->IS_KIND_OF(OpCreateNewMould))				 ||
				 pOp->IsKindOf(CC_RUNTIME_CLASS(OpPasteEnvelope)) ||
				 pOp->IsKindOf(CC_RUNTIME_CLASS(OpPastePerspective)))
			{
				allowed = FALSE;
			}

			// Shadows can't be feathered.
			else if (pParam->GetChangeFlags().Attribute)
				if (pOp->IS_KIND_OF(OpChangeFeatherSize))
					allowed = FALSE;
#endif
		}
	}

	// if necessary, set permissions for OnChildChange.
	if (SetOpPermissionState)
		SetOpPermission(allowed ? PERMISSION_ALLOWED : PERMISSION_DENIED, TRUE);
		
	// If we're ok so far and were asked to do a PreTriggerEdit, then
	// determine whether the Op may change the bounds of some nodes.
	// If it may, then call NameGallery::PreTriggerEdit.
PORTNOTETRACE("NameGallery", "Use of NameGallery removed in NodeShadowController::AllowOp");
#if !defined(EXCLUDE_FROM_XARALX)
	if (allowed && DoPreTriggerEdit)
	{
		// if the Op is non-NULL then query its MayChangeNodeBounds() method.
		UndoableOperation* pChangeOp = pParam->GetOpPointer();
		if (pChangeOp != NULL && pChangeOp->MayChangeNodeBounds())
		{
			allowed = NameGallery::Instance()->PreTriggerEdit(pChangeOp, pParam, this);
		}
	}
#endif

	return allowed;
}



/********************************************************************************************

>	void NodeShadow::SetBiasGain(CProfileBiasGain &gain)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/11/96
	Inputs:		
	Purpose:	Sets the profile for the shadow node
	SeeAlso:	

********************************************************************************************/
void NodeShadow::SetBiasGain(CProfileBiasGain &gain)
{
	AFp BiasValue = gain.GetBias();
	AFp GainValue = gain.GetGain();

	m_BiasGain.SetBias(BiasValue);
	m_BiasGain.SetGain(GainValue);
}



/********************************************************************************************

>	CProfileBiasGain &NodeShadow::GetBiasGain()

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/11/96
	Inputs:		
	Purpose:	Gets the profile for the shadow node
	SeeAlso:	

********************************************************************************************/
CProfileBiasGain &NodeShadow::GetBiasGain()
{
	static CProfileBiasGain Profile;
	
	AFp BiasValue = m_BiasGain.GetBias();
	AFp GainValue = m_BiasGain.GetGain();

	Profile.SetBias(BiasValue);
	Profile.SetGain(GainValue);

	return Profile;
}

CProfileBiasGain* NodeShadow::GetBiasGainPtr()
{
	return (&m_BiasGain);
}



/********************************************************************************************

>	virtual DocRect NodeShadow::ValidateExtend(const ExtendParams& ExtParams)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	06/12/1999
	Inputs:		ExtParams		description parameters for the extension.
	Outputs:	
	Returns:	TRUE if extending this Nodewill be a reversible operation,
				FALSE otherwise.
	Purpose:	Tests the reversibility of an Extend operation applied to this node.

				A NodeShadow cannot itself extend, so instead it asks its parent controller
				to extend for it. Infinite recursion does not occur, as the controller node
				ignores its child node, this NodeShadow.
	Errors:		In debug builds, ERROR3 if this Node has no NodeShadowController,
				in release, we return TRUE.
	See also:	IsTypeExtendible(), Extend().

********************************************************************************************/
DocRect NodeShadow::ValidateExtend(const ExtendParams& ExtParams)
{
	Node* pBob = GetParentController();
	if (pBob == NULL)
	{
		ERROR3("NodeShadow::ValidateExtend- no controller Node found!");
		return DocRect(INT32_MAX, INT32_MAX, INT32_MAX, INT32_MAX);
	}
	else
	{
		return pBob->ValidateExtend(ExtParams);
	}
}



/********************************************************************************************

>	virtual void NodeShadow::Extend(const ExtendParams& ExtParams)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	06/12/1999
	Inputs:		ExtParams		description parameters for the extension.
	Outputs:	Some of the child nodes of this ShadowNode's NodeShadowController may have
				their dimensions altered.
	Returns:	
	Purpose:	Perform an Extend operation on this Node, and its children if appropriate.

				A NodeShadow cannot itself extend, so instead it asks its parent controller
				to extend for it. Infinite recursion does not occur, as the controller node
				ignores its child node, this NodeShadow.
	Errors:		In debug builds, ERROR3 if this Node has no NodeShadowController,
				in release, we do nothing.
	See also:	

********************************************************************************************/
void NodeShadow::Extend(const ExtendParams& ExtParams)
{
	Node* pBob = GetParentController();
	if (pBob == NULL)
	{
		ERROR3("NodeShadow::ValidateExtend- no controller Node found!");
	}
	else
	{
		pBob->Extend(ExtParams);
	}

	return;
}



/********************************************************************************************

>	virtual BOOL NodeShadow::OnNodePopUp(Spread* pSpread, DocCoord PointerPos, ContextMenu* pMenu)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/2/2000
	Inputs:		pSpread		The spread in which things are happening
				PointerPos	The Location of the mouse pointer at the time of the click
				pMenu		The menu to which items should be added
	Returns:	BOOL - TRUE if the node claims the click as its own and FALSE if it is
				not interested in the click
	Purpose:	Allows the shadow to respond to the click

********************************************************************************************/
BOOL NodeShadow::OnNodePopUp(Spread* pSpread, DocCoord PointerPos, ContextMenu* pMenu)
{
	PORTNOTE("other","NodeShadow::OnNodePopUp - do nothing")
#ifndef EXCLUDE_FROM_XARALX
	BOOL ok = TRUE;
#ifndef NO_ADVANCED_TOOLS	
	ok = ok && pMenu->BuildCommand(TOOL_OPTOKEN_SOFTSHADOW, TRUE);
#endif
	return ok;
#endif
	return FALSE;
}



/********************************************************************************************

>	virtual INT32 NodeShadow::EstimateNodeComplexity (OpParam* details)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/09/2000

	Inputs:		details		any data that should be used for the calculation

	Outputs:	-

	Returns:	an estimate of the nodes complexity

	Purpose:	This function estimates a complexity value for the node.  The complexity
				value is based upon the total length of all paths in the node.

	See Also:	OpBlendNodes::DeterminBlendObjectsProcessorHit ()

********************************************************************************************/
INT32 NodeShadow::EstimateNodeComplexity (OpParam* details)
{
	return (m_Path.GetUsedSlots ());
}



/********************************************************************************************

>	virtual BOOL NodeShadow::CanAttrBeAppliedToMe(CCRuntimeClass* pAttribClass)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/08/2000
	Inputs:		pAttribClass	the attr type to test.
	Returns:	FALSE if pAttribClass is a brush attr,
				TRUE otherwise.

	Purpose:	Determine whether the given attr type can be applied directly to us.
				Any of those attrs which we require can be directly applied to us.

	Errors:		returns FALSE if pAttribClass is NULL.

********************************************************************************************/
BOOL NodeShadow::CanAttrBeAppliedToMe(CCRuntimeClass* pAttribClass)
{
	// For NodeShadow, this fn does the same thing as RequiresAttrib().
	return RequiresAttrib(pAttribClass);
}



/********************************************************************************************

>	void NodeShadow::SetDarkness(double Darkness)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/02/2001
	Purpose:	Set shadow darkness, from 0.0 (transparent) to 1.0 (opaque).
	Errors:		ERROR3 if input value is out of range.

********************************************************************************************/
void NodeShadow::SetDarkness(double Darkness)
{
	ERROR3IF(Darkness < 0.0 || Darkness > 1.0, "NodeShadow::SetDarkness; invalid input!");
	m_dDarkness	=	Darkness < 0.0 ? 0.0 :
					Darkness > 1.0 ? 1.0 :
					Darkness ;
}




#ifdef NEW_SHADOW_RENDER
/********************************************************************************************

>	BOOL NodeShadow::MakeShadow(LPBITMAPINFO lpInfo, LPBYTE lpBits, DocRect orect)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/03/2005
	Purpose:	Create a shadow from the supplied 32BPP RGBA captured bitmap of the 
				original objects

********************************************************************************************/
BOOL NodeShadow::MakeShadow(LPBITMAPINFO lpInfo, LPBYTE lpBits, DocRect orect)
{
	if (m_ShadowBitmap)
		return TRUE;

	// Get details of this shadow from our controller
	NodeCompound* pCompound = GetParentController();
	NodeShadowController* pController = NULL;
	if (pCompound && pCompound->IsAShadowController())
		pController = (NodeShadowController*)pCompound;
	if (pController==NULL)
		return FALSE;

	double dPixelWidth = pController->GetPixelWidth();
	double dPixBlurDiameter = pController->GetPenumbraWidth()/dPixelWidth;

	// Use the RGBT bitmap provided to create a suitable shadow
	// and store it in the cache
//	LPBITMAPINFO lpShadowInfo = NULL;
//	LPBYTE lpShadowBits = NULL;
	DocRect ShadowRect;
	BOOL bOK = FALSE;


	// ------------------------------------------------
	// This code lifted from RenderBitmapForDisplay
	//
	// initialise the transparency fill attribute and clear our caches.
	SetupTransFill();

	// Get a kernel version of the silhouette bitmap
	CWxBitmap		   *pwBitmap	= new CWxBitmap(lpInfo, lpBits);
	KernelBitmap	   *pkBitmap	= new KernelBitmap(pwBitmap);

	// generate the shadow bitmap for the required blur diameter.
	bOK = GenerateShadowBitmap(pkBitmap, dPixBlurDiameter);
	if (!bOK)
		return FALSE;

// if desired, attach a debug copy of the final shadow bitmap within the bitmap gallery.
#ifdef ATTACH_SHADOW_BMP
	m_ShadowBitmap->AttachDebugCopyToCurrentDocument("MakeShadow Bitmap");
#endif

	// Get rid of temp kernel bitmap
	if (pwBitmap)
		pwBitmap->BMBytes = ((CWxBitmap*)OILBitmap::Default)->BMBytes;
	if (pkBitmap)
	{
		delete pkBitmap;
		pkBitmap = NULL;
	}

	m_LastRequestedPixWidth	= MILLIPOINT(dPixelWidth);
//	m_LastQualitySetting	= CurrentQuality;
	m_LastActualPixWidth = dPixelWidth;

	// one last thing - create the path which the shadow bitmap-transparency is drawn into.
	CreateBitmapPath(orect, dPixelWidth, dPixBlurDiameter);				// TODO: Set pixel width properly
	// ------------------------------------------------


	// No need to do this at the moment because GenerateShadowBitmap creates m_ShadowBitmap
/*	CBitmapCache* pBitmapCache = Camelot.GetBitmapCache();
	if (pBitmapCache==NULL)
		return FALSE;

	if (lpInfo==NULL || lpBits==NULL)
		return FALSE;

	// Cache the ORIGINAL bitmap as Option 0
	// See also, SetOriginalBitmap
	double PixelWidth = 42;						// TODO: Does this matter?
	CBitmapCacheKey inky(this, PixelWidth, 0);

	CCachedBitmap cbmp;

	cbmp.pbmpBits = lpShadowBits;
	cbmp.pbmpInfo = lpShadowInfo;
	cbmp.SetCachedRect(ShadowRect);
	cbmp.nPriority = CACHEPRIORITY_TEMPBITMAP_HIGH;

	if (cbmp.IsValid())
	{
		pBitmapCache->StoreBitmap(inky, cbmp);
		bCached = TRUE;
	}
*/
	return TRUE;
}




/********************************************************************************************

>	BOOL NodeShadow::RenderShadow(RenderRegion* pRender)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/03/2005
	Purpose:	Render a previously prepared shadow

********************************************************************************************/
BOOL NodeShadow::RenderShadow(RenderRegion* pRender)
{
//#pragma message( __LOCMSG__ "NodeShadow::RenderShadow - do nothing" )
//	TRACE( _T("Warning - NodeShadow::RenderShadow called\n") );
	enum Quality::Fill CurrentQuality = pRender->RRQuality.GetFillQuality();

	pRender->SaveContext();

	if (CurrentQuality >= (enum Quality::Fill)Quality::Solid)
	{
		pRender->SetFillColour(COLOUR_BLACK);	// make sure there's some shadow colour

		Node* pNode = FindFirstChild();
		while (pNode)
		{
			if (pNode->IsAnAttribute())
			{
				pNode->Render(pRender);
			}

			pNode = pNode->FindNext();
		}

		RenderBitmap(pRender);
	}
	else
	{
		// outlines only - we'll just render the shadow's bounding rect.
		DocRect drBounds = GetBoundingRect();
		pRender->SetFillColour(COLOUR_NONE);
		pRender->SetLineColour(COLOUR_BLACK);
		pRender->DrawRect(&drBounds);
	}

	pRender->RestoreContext();

	return FALSE;
}




/********************************************************************************************

>	BOOL NodeShadow::NeedsRemake()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/03/2005
	Purpose:	Create a shadow from the supplied 32BPP RGBA captured bitmap of the 
				original objects

********************************************************************************************/
BOOL NodeShadow::NeedsRemake()
{
	return (m_ShadowBitmap == NULL);
}




/********************************************************************************************

>	virtual BOOL NodeShadow::TransformShadow(TransformBase& Trans)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/04/2005
	Inputs:		Trans - The transform Object
	Returns:	TRUE if could transform the shadow
				FALSE otherwise
	Purpose:	Transforms the shadow when the controller is in IsCapturingChildren mode
				if we can
	SeeAlso:	NodeRenderableInk::Transform()

********************************************************************************************/
BOOL NodeShadow::TransformShadow(TransformBase& Trans)
{
	NodeShadowController* pControl = (NodeShadowController*)GetParentController();
	if (pControl==NULL)
		return FALSE;

	ERROR3IF(!pControl->IsCapturingChildren(), "TransformShadow should only be called when IsCapturingChildren");

	ShadowType shadtype = pControl->GetShadowType();

	// We can only translate floor shadows!
	if (!Trans.IsTranslation() && shadtype==SHADOWTYPE_FLOOR)
		return FALSE;

	// We can't transform shadows unless the transform has a usable matrix
	if (!Trans.IS_KIND_OF(Trans2DMatrix))
		return FALSE;

	// Transform the base path
	Trans.Transform(m_NonTranslatedPath.GetCoordArray(), m_NonTranslatedPath.GetNumCoords());

	switch (shadtype)
	{
	case SHADOWTYPE_WALL:
		// Transform the shadow path using the modified transform
		m_Path.ClearPath();
		m_Path.CloneFrom(m_NonTranslatedPath);
		m_Path.IsFilled = TRUE;
		m_Path.Translate(pControl->GetWallShadowOffset());
		break;

	case SHADOWTYPE_FLOOR:
		Trans.Transform(m_Path.GetCoordArray(), m_Path.GetNumCoords());
		break;

	case SHADOWTYPE_GLOW:
		Trans.Transform(m_Path.GetCoordArray(), m_Path.GetNumCoords());
		break;

	default:
		ERROR3("NodeShadow::TransformShadow; unknown parent shadow type!");
		break;
	}

	TransformChildren(Trans);

	return TRUE;
}
#endif



//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------



//////////////////////////////////////////////////////
// The passback function for the shadow become a
/********************************************************************************************

>	BOOL NodeShadowBecomeA::PassBack(NodeRenderableInk* pNewNode,
								 NodeRenderableInk* pCreatedByNode,CCAttrMap* pAttrMap)


	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/12/99
	Purpose:	Used by NodeShadow to produce the path for the NodeShadowPassback node

********************************************************************************************/
BOOL NodeShadowBecomeA::PassBack(NodeRenderableInk* pNewNode,
								 NodeRenderableInk* pCreatedByNode,CCAttrMap* pAttrMap)
{
	if (pNewNode)
	{
		if (pNewNode->IsNodePath())
		{
			m_pPath->MergeTwoPaths(((NodePath *)pNewNode)->InkPath);
		}

		pNewNode->CascadeDelete();
		delete pNewNode;
	}

	return TRUE;
}



//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------



/********************************************************************************************

>	BitmapShadower* BitmapShadower::CreateSilhouetteBitmap(	NodeShadowController* pControl,
															GRenderRegion* pGRR = NULL,
															List* pList = NULL )
	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/10/2000

	Inputs:		pControl	NodeShadowController of the shadow which we're creating.
				pGRR		render region for which to generate the shadow bitmap.
				pList		list of SumAllPathsElem's to shadow - usually NULL, in which
							case we shadow the node lying within the shadow tree structure.

	Outputs:	If successful, a 32bpp silhouette + other shadow information is generated
				and stored in a new instance of this class.

	Returns:	A new instance of this class, containing all relevant shadow information, or
				NULL if we failed.

	Purpose:	Generate a 32bpp silhouette bitmap ready for shadowing.
				All relevant shadow information is stored in the returned object.

				At a later date, I may move all non-bshadow.cpp/.h shadow bitmap
				generating code into this class.

	Errors:		ERROR1 with NULL if we cannot create a new BitmapShadower.
				ERROR2 with NULL if pControl or pGRR is NULL.
				ERROR3 with NULL if pList is NULL and the shadowed node is NULL too.

	See also:	NodeShadow::RenderBitmapForDisplay().

********************************************************************************************/
BitmapShadower* BitmapShadower::CreateSilhouetteBitmap(	NodeShadowController* pControl,
														GRenderRegion* pGRR, List* pList )
{
	// input validation.
	ERROR2IF(pControl == NULL, NULL, "BitmapShadower::CreateSilhouetteBitmap; NULL input params!");

	// create a new shadower.
	BitmapShadower* pBitmapShadower = new BitmapShadower;
	ERROR1IF(pBitmapShadower == NULL, NULL, _R(IDS_OUT_OF_MEMORY));

	// initialise it and tell it to do its stuff.
	BOOL	ok = pBitmapShadower->Initialise(pControl, pGRR, pList);
	if (ok) ok = pBitmapShadower->DoCreateSilhouetteBitmap();

	return ok ? pBitmapShadower : NULL;
}



/********************************************************************************************

>	BitmapShadower::BitmapShadower()

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/10/2000

	Purpose:	Private constructor - you may only create this class through public static
				interface.

	See also:	BitmapShadower::CreateSilhouetteBitmap().

********************************************************************************************/
BitmapShadower::BitmapShadower()
{
	m_pGRR			= NULL;

	m_pControl		= NULL;
	m_pShadow		= NULL;
	m_pShadowedNode	= NULL;
	m_pShadowedList	= NULL;

	m_ShadowType	= SHADOWTYPE_NONE;

	m_drShadowedBounds.MakeEmpty();
	m_ShadowScaleFactor	= 1.0;
	m_ActualPixelWidth	= 0;
	m_pixBlurDiameter	= 0;

	m_pRendWrap		= NULL;
	m_pOffscreenRR	= NULL;

	m_pSilhouetteBitmap	= NULL;
}



/********************************************************************************************

>	BitmapShadower::~BitmapShadower()

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/10/2000
	Purpose:	Destructor - frees the silhouette bitmap if it has not been extracted.

********************************************************************************************/
BitmapShadower::~BitmapShadower()
{
	if (m_pSilhouetteBitmap != NULL)
	{
		delete m_pSilhouetteBitmap;
		m_pSilhouetteBitmap = NULL;
	}
}



/********************************************************************************************

>	BOOL BitmapShadower::Initialise(NodeShadowController* pControl, GRenderRegion* pGRR,
																			 List* pList)
	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/10/2000
	Inputs:		pControl	NodeShadowController of the shadow which we're creating.
				pGRR		render region for which to generate the shadow bitmap.
				pList		list of SumAllPathsElem's to shadow - usually NULL, in which
							case we shadow the node lying within the shadow tree structure.

	Returns:	TRUE if successful, FALSE othewise.

	Purpose:	Initialises our state.

********************************************************************************************/
BOOL BitmapShadower::Initialise(NodeShadowController* pControl, GRenderRegion* pGRR,
																		List* pList)
{
	m_pControl			= pControl;
	m_pGRR				= pGRR;
	m_ShadowType		= m_pControl->GetShadowType();

	if (pList == NULL)
		m_pShadowedNode = pControl->GetShadowedNode();
	else
		m_pShadowedList = pList;

	if (m_pShadowedList == NULL && m_pShadowedNode == NULL)
	{
		ERROR3("BitmapShadower called with NULL shadowed node or NULL path-list to shadow!");
		return FALSE;
	}

	return TRUE;
}



/********************************************************************************************

>	BOOL BitmapShadower::DoCreateSilhouetteBitmap()

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/10/2000
	Returns:	TRUE if successful, FALSE otherwise.
	Purpose:	Creates the shadow silhouette bitmap.

********************************************************************************************/
BOOL BitmapShadower::DoCreateSilhouetteBitmap()
{
	BOOL	ok = CalculateBoundsAndScale1();
	if (ok)	ok = (m_pGRR == NULL) ? CalculateBoundsAndScale2NoGRR() :
									CalculateBoundsAndScale2WithGRR() ;
	if (ok)	ok = CreateOffscreenRR();
	if (ok)	ok = RenderShadowedNodeOffscreen();

	return ok;
}



/********************************************************************************************

>	BOOL BitmapShadower::CalculateBoundsAndScale()

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/10/2000

	Outputs:	Following member variables are initialised:

				m_drShadowedBounds
				m_ShadowType
				m_ShadowScaleFactor
				m_ActualPixelWidth

	Returns:	TRUE if successful, FALSE otherwise.

	Purpose:	Calculate the bounds and scaling factor for our shadow bitmap.

				This function is split into two parts, 1 and 2.
				This is because part 2 performs differently, depending on whether or not we
				have a ptr to a GRenderRegion. A GRenderRegion ptr ensures that our rendering
				matches exactly with the target output device, however if we have no GRR, we
				must query the current view, which is likely to produce just as good results,
				but 

********************************************************************************************/
BOOL BitmapShadower::CalculateBoundsAndScale1()
{
	// if we're shadowing a node in the tree (usual behaviour), use its bounds.
	if (m_pShadowedNode != NULL)
		m_drShadowedBounds = m_pShadowedNode->GetBoundingRect();

	// if we're shadowing a list of paths & attrs, use the union of their bounds,
	// taking into account any attributes attached.
	else if (m_pShadowedList != NULL)
	{
		SumAllPathsElem* pElem = (SumAllPathsElem*)m_pShadowedList->GetHead();
		while (pElem != NULL)
		{
			DocRect PathBounds(0,0,0,0);
			pElem->GetPath()->GetTrueBoundingRect(&PathBounds, 0, pElem->GetAttrMap());
			m_drShadowedBounds = m_drShadowedBounds.Union(PathBounds);

			//////////////////////////////////////////////////////////////////////////////////////////////
			//	This code block is a BODGE for feathers+blends, and does not belong here!
			//
			AttrFeather* appliedFeather = NULL;
			pElem->GetAttrMap()->Lookup( CC_RUNTIME_CLASS(AttrFeather), (void *&) appliedFeather);

			if (appliedFeather)
			{
				if (!(((FeatherAttrValue*) appliedFeather->GetAttributeValue ())->IsDefaultFlagSet ()))
				{
					pElem->GetAttrMap ()->PostBlendInit ((pElem->GetPath()), CC_RUNTIME_CLASS (NodePath));
				}
			}
			//////////////////////////////////////////////////////////////////////////////////////////////

			pElem = (SumAllPathsElem *)m_pShadowedList->GetNext(pElem);
		}
	}

	// glow shadows require a bitmap big enough to hold the glow contour.
	if (m_ShadowType == SHADOWTYPE_GLOW)
		m_drShadowedBounds.Inflate(m_pControl->GetGlowWidth());

	return TRUE;
}



BOOL BitmapShadower::CalculateBoundsAndScale2WithGRR()
{
//#pragma message( __LOCMSG__ "BitmapShadower::CalculateBoundsAndScale2WithGRR - do nothing" )
//	TRACE( _T("Warning - BitmapShadower::CalculateBoundsAndScale2WithGRR called\n") );
	// leave a pixel-width's safety-margin around the bitmap.
	// we don't use GetScaledPixelWidth(), as that is not always correct!
	FIXED16 ScaleFactor;
	m_pGRR->GetMatrix().Decompose(&ScaleFactor);
	double GRRPixelWidth = (double)m_pGRR->GetPixelWidth() / fabs(ScaleFactor.MakeDouble());
	m_drShadowedBounds.Inflate((INT32)(GRRPixelWidth + 0.5));

	// if the offscreen bitmap will be too big, then we'll render it as large
	// as we can, and remember how much further it needs to scale up.
	WinRect	wrBounds = m_pGRR->CalculateWinRect(m_drShadowedBounds);

	INT32 XSize = wrBounds.width;
	INT32 YSize = wrBounds.height;
	if (XSize > YSize)
	{
		if (XSize > MaxShadowBitmapSize)
			m_ShadowScaleFactor = MaxShadowBitmapSize / (double)XSize;
	}
	else
	{
		if (YSize > MaxShadowBitmapSize)
			m_ShadowScaleFactor = MaxShadowBitmapSize / (double)YSize;
	}

	// use the shadow scaling factor to calculate the actual pixel-width
	// for the shadow bitmap, so it renders at the correct size onscreen.
	m_ActualPixelWidth	= GRRPixelWidth / m_ShadowScaleFactor;

	return TRUE;
}



BOOL BitmapShadower::CalculateBoundsAndScale2NoGRR()
{
//#pragma message( __LOCMSG__ "BitmapShadower::CalculateBoundsAndScale2NoGRR - do nothing" )
//	TRACE( _T("Warning - BitmapShadower::CalculateBoundsAndScale2NoGRR called\n") );
	// we have no render region, but we need a render matrix,
	// so we must get the view to construct a render matrix for us.
	View* pView = View::GetCurrent();
	if (pView == NULL)
		return FALSE;

	Spread* pSpread = m_pControl->FindParentSpread();
	if (pSpread == NULL)
		return FALSE;

	double PixelsPerInch = pView->GetConvertToEditableShapesDPI();

	// leave a pixel-width's safety-margin around the bitmap.
	FIXED16 ViewPixelWidth = pView->GetScaledPixelWidth();
	m_drShadowedBounds.Inflate(ViewPixelWidth.round().MakeLong());

	// if the offscreen bitmap will be too big, then we'll render it as large
	// as we can, and remember how much further it needs to scale up.
	Matrix RenderMatrix = pView->ConstructRenderingMatrix(pSpread);
	WinRect	wrBounds = OSRenderRegion::DocRectToWin(RenderMatrix, m_drShadowedBounds, PixelsPerInch);

	INT32 XSize = wrBounds.width;
	INT32 YSize = wrBounds.height;
	if (XSize > YSize)
	{
		if (XSize > MaxShadowBitmapSize)
			m_ShadowScaleFactor = MaxShadowBitmapSize / (double)XSize;
	}
	else
	{
		if (YSize > MaxShadowBitmapSize)
			m_ShadowScaleFactor = MaxShadowBitmapSize / (double)YSize;
	}

	// use the shadow scaling factor to calculate the actual pixel-width
	// for the shadow bitmap, so it renders at the correct size onscreen.
	const double AppPixelWidth = MILLIPOINTS_PER_INCH / PixelsPerInch;
	m_ActualPixelWidth = AppPixelWidth / m_ShadowScaleFactor;

	// Work out the adjustment to the scale factor to account for 
	// not rendering at 96 dpi.  This must be done after the calculations 
	// above as the bitmap is always rendered at 96 dpi but with a scale factor 
	// to ensure it is rendered the correct size.  If we are rendering at 20 dpi 
	// then we need to render 20/96 of the size but this must NOT be included in 
	// the calculation of m_ActualPixelWidth or the bitmap will be rendered into 
	// the main render region at the wrong size.

// Phil, 21/02/2006
// This doesn't seme to be needed any more and in fact it causes bug BZ861
// Note that the ScaledPixelWidth resolution of captures is better controlled
// than when this bodge was added.
// So I'm tentatively removing it again
// Unfortunately, removing this causes problems with export of shadows to 
// plugin filters so it is going back in for now...
	
	if (PixelsPerInch != 96.0)
	{
		double NewScale = PixelsPerInch / 96.0;
		m_ShadowScaleFactor *= NewScale;
	}

	return TRUE;
}



/********************************************************************************************

>	BOOL BitmapShadower::CreateOffscreenRR()

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/10/2000

	Outputs:	Following member variables are initialised:

				m_pRendWrap
				m_pOffscreenRR
				m_pixBlurDiameter

	Returns:	TRUE if successful, FALSE otherwise.

	Purpose:	Creates the offscreen render-region, ready for rendering into.

********************************************************************************************/
BOOL BitmapShadower::CreateOffscreenRR()
{
//#pragma message( __LOCMSG__ "BitmapShadower::CreateOffscreenRR - do nothing" )
//	TRACE( _T("Warning - BitmapShadower::CreateOffscreenRR called\n") );
	// create a concurrent renderer please, scaling it up if necessary.
	// ConcurrentRenderer creates a new GRenderRegion, based on the given GRenderRegion.
	m_pRendWrap = GRenderRegionWrapper::GetConcurrentRenderer(	m_pGRR,
																m_ShadowScaleFactor,
																m_drShadowedBounds,
																32,
																TRUE );
	if (m_pRendWrap == NULL)
		return FALSE;

	m_pOffscreenRR = m_pRendWrap->GetRenderRegion();

	// work out the required blur diameter in pixels (allowed to be non-integer).
	m_pixBlurDiameter = m_pControl->GetPenumbraWidth() / m_ActualPixelWidth;

	// wall and glow shadows have alignment problems, which require that we:
	//	1. force the offscreen RR to always smooth bitmaps.
	//	2. turn off line hinting in the offscreen RR.
	//	3. offset the RR down and left by a half pixel if we have an even blur.
	//
	// Note: non-full-quality rendering can do this job properly,
	// so if we're not rendering at full quality then we can only do odd blurs.
	if (m_ShadowType == SHADOWTYPE_GLOW || m_ShadowType == SHADOWTYPE_WALL)
	{
		if (m_pOffscreenRR->RRQuality.GetAntialiasQuality() == (enum Quality::Antialias)Quality::FullAntialias)
		{
			m_pOffscreenRR->SetForceBitmapSmoothing(TRUE);
			m_pOffscreenRR->GetDrawContext()->SetHintingFlag(FALSE);
			if (CBitmapShadow::BlurringWillOffsetBitmap(m_pixBlurDiameter))
				m_pOffscreenRR->OffsetByHalfPixel();
		}
		else
		{
			// if blurring will offset the bitmap, then make the blur odd: +1,
			// and round to the nearest whole pixel: (UINT32)(+0.5).
			if (CBitmapShadow::BlurringWillOffsetBitmap(m_pixBlurDiameter))
				m_pixBlurDiameter = (UINT32)(m_pixBlurDiameter + 1.5);
		}
	}

	return TRUE;
}



#ifndef NEW_GLOW_SILHOUETTE
/********************************************************************************************

>	BOOL BitmapShadower::RenderShadowedNodeOffscreen()

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/10/2000

	Outputs:	Following member variables are initialised:

				m_pSilhouetteBitmap

				Following member variables are deinitialised:

				m_pRendWrap
				m_pOffscreenRR

	Returns:	TRUE if successful, FALSE otherwise.

	Purpose:	Render the shadowed node offscreen, to create the silhouette bitmap.

********************************************************************************************/
BOOL BitmapShadower::RenderShadowedNodeOffscreen()
{
//#pragma message( __LOCMSG__ "BitmapShadower::RenderShadowedNodeOffscreen - do nothing" )
//	TRACE( _T("Warning - BitmapShadower::RenderShadowedNodeOffscreen called\n") );
// if we're rendering our shadowed nodes, then we need to pre-render all
	// attributes which indirectly apply to it via the controller,
	// *except* for offscreen attributes, as they're meant to divert rendering offscreen
	// for our *controller* node, not us.
	if (m_pShadowedNode != NULL)
	{
		CCAttrMap* pControlMap = CCAttrMap::MakeAppliedAttrMap(m_pControl);
		if (pControlMap != NULL)
		{
			pControlMap->Render(m_pOffscreenRR, FALSE);
			delete pControlMap;
		}
	}

	// we save context here, as the rendered attributes are applied indirectly,
	// therefore normal rendering would have one or more SaveContext()'s before
	// the ink-node is rendered.
	m_pOffscreenRR->SaveContext();

	// Create some no-transp attrs, for possible use if we're a glow shadow.
	FlatTranspFillAttribute NoFillTransp(TT_NoTranspType);
	StrokeTranspAttribute NoLineTransp(TT_NoTranspType);
	
	// for glow shadows, we must contour out from what we're shadowing and render that.
	if (m_ShadowType == SHADOWTYPE_GLOW)
	{
		// do a glow-width contour out from the shadowed node.
		// TODO: test and act on failure of this function.
		Path ContouredPath;
		NodeContour::GenerateContourPathForNode(&ContouredPath, m_pShadowedNode,
												m_pShadowedList, m_pControl->GetGlowWidth());

		// set the render region to draw black silhouettes.
		m_pOffscreenRR->SetFillColour(COLOUR_BLACK);
		m_pOffscreenRR->SetLineColour(COLOUR_TRANS);

		// glow shadows do not pay attention to any transparency affecting the shadowed nodes,
		// so we must turn off any transparency attributes which were rendered by our attr-map.
		m_pOffscreenRR->SetTranspFillGeometry(&NoFillTransp, FALSE);
		m_pOffscreenRR->SetLineTransp(&NoLineTransp, FALSE);

		// that should do it - render the path into the render-region.
		m_pOffscreenRR->DrawPath(&ContouredPath);
	}

	// for other shadow types, we just render the node/path-list.
	// note that this is done *uninterruptibly*, unlike normal rendering.
	else
	{
		if (m_pShadowedNode != NULL)
		{
			BOOL SubRenderStateLockedByShadower = FALSE;
			if (!m_pOffscreenRR->IsSubRenderStateLocked())
			{
				m_pOffscreenRR->LockSubRenderState(TRUE);
				SubRenderStateLockedByShadower = TRUE;
			}
			m_pShadowedNode->RenderTreeAtomic(m_pOffscreenRR);
			if (SubRenderStateLockedByShadower)
			{
				m_pOffscreenRR->LockSubRenderState(FALSE);
				SubRenderStateLockedByShadower = FALSE;
			}
		}

		else if (m_pShadowedList != NULL)
		{
			SumAllPathsElem* pElem = (SumAllPathsElem *)m_pShadowedList->GetHead();
			while (pElem != NULL)
			{
				m_pOffscreenRR->SaveContext();
				pElem->GetAttrMap()->Render(m_pOffscreenRR);
				m_pOffscreenRR->DrawPath(pElem->GetPath());
				m_pOffscreenRR->RestoreContext();

				pElem = (SumAllPathsElem*)m_pShadowedList->GetNext(pElem);
			}
		}
	}

	// restore the context to match our SaveContext() above.
	m_pOffscreenRR->RestoreContext();

	// extract the bitmap from the offscreen RR before the RR is closed down.
	m_pSilhouetteBitmap = m_pRendWrap->GetKernelBitmap();
	if (m_pSilhouetteBitmap == NULL)
	{
		ERROR3("BitmapShadower::RenderShadowedNodeOffscreen; Couldn't extract KernelBitmap!");
		return FALSE;
	}

// if desired, insert a debug copy of the silhouette bitmap within the bitmap gallery.
#ifdef ATTACH_SILHOUETTE_BMP
	m_pSilhouetteBitmap->AttachDebugCopyToCurrentDocument("Shadow Silhouette Bitmap");
#endif;

	// close down the offscreen render-region and clear our ptrs to prevent future nastiness.
	m_pRendWrap->RestorePreviousRendererState();
	m_pRendWrap		= NULL;
	m_pOffscreenRR	= NULL;

	return TRUE;
}
#endif


#ifdef NEW_GLOW_SILHOUETTE
/********************************************************************************************

>	BOOL BitmapShadower::RenderShadowedNodeOffscreen()

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/10/2000

	Outputs:	Following member variables are initialised:

				m_pSilhouetteBitmap

				Following member variables are deinitialised:

				m_pRendWrap
				m_pOffscreenRR

	Returns:	TRUE if successful, FALSE otherwise.

	Purpose:	Render the shadowed node offscreen, to create the silhouette bitmap.

********************************************************************************************/
BOOL BitmapShadower::RenderShadowedNodeOffscreen()
{
//#pragma message( __LOCMSG__ "BitmapShadower::RenderShadowedNodeOffscreen - do nothing" )
//	TRACE( _T("Warning - BitmapShadower::RenderShadowedNodeOffscreen called\n") );
	// if we're rendering our shadowed nodes, then we need to pre-render all
	// attributes which indirectly apply to it via the controller,
	// *except* for offscreen attributes, as they're meant to divert rendering offscreen
	// for our *controller* node, not us.
	if (m_pShadowedNode != NULL)
	{
		CCAttrMap* pControlMap = CCAttrMap::MakeAppliedAttrMap(m_pControl, TRUE, FALSE);
		if (pControlMap != NULL)
		{
			pControlMap->Render(m_pOffscreenRR, FALSE);
			delete pControlMap;
		}
	}

	// we save context here, as the rendered attributes are applied indirectly,
	// therefore normal rendering would have one or more SaveContext()'s before
	// the ink-node is rendered.
	m_pOffscreenRR->SaveContext();

	// Create some no-transp attrs, for possible use if we're a glow shadow.
	FlatTranspFillAttribute NoFillTransp(TT_NoTranspType);
	StrokeTranspAttribute NoLineTransp(TT_NoTranspType);
	
	// NOTE: This render is not interruptible for normal background rendering
	// NOTE: Should use Phil's new Capture system to avoid this extra render
	{
		if (m_pShadowedNode != NULL)
		{
			BOOL SubRenderStateLockedByShadower = FALSE;
			if (!m_pOffscreenRR->IsSubRenderStateLocked())
			{
				m_pOffscreenRR->LockSubRenderState(TRUE);
				SubRenderStateLockedByShadower = TRUE;
			}
			m_pShadowedNode->RenderTreeAtomic(m_pOffscreenRR);
			if (SubRenderStateLockedByShadower)
			{
				m_pOffscreenRR->LockSubRenderState(FALSE);
				SubRenderStateLockedByShadower = FALSE;
			}
		}

		else if (m_pShadowedList != NULL)
		{
			SumAllPathsElem* pElem = (SumAllPathsElem *)m_pShadowedList->GetHead();
			while (pElem != NULL)
			{
				m_pOffscreenRR->SaveContext();
				pElem->GetAttrMap()->Render(m_pOffscreenRR);
				m_pOffscreenRR->DrawPath(pElem->GetPath());
				m_pOffscreenRR->RestoreContext();

				pElem = (SumAllPathsElem*)m_pShadowedList->GetNext(pElem);
			}
		}
	}

	// restore the context to match our SaveContext() above.
	m_pOffscreenRR->RestoreContext();

	// for glow shadows, we must contour out from what we're shadowing and render that.
	if (m_ShadowType == SHADOWTYPE_GLOW)
	{
		GRenderRegion* pRegion = NULL;
		LPBITMAPINFO lpInfo = NULL;
		LPBYTE lpBits = NULL;
		pRegion = m_pRendWrap->GetRenderRegion();
		m_pRendWrap->GetBitmapPointers(&lpInfo, &lpBits);
		ENSURE(pRegion && lpInfo && lpBits, "Failed to get offscreen region details in RenderShadowedNodeOffscreen");

		LPBITMAPINFO lpContourInfo = NULL;
		LPBYTE lpContourBits = NULL;
		double OffsetX = 0;
		double OffsetY = 0;
		double dPixelContour = (double)m_pControl->GetGlowWidth()/pRegion->GetScaledPixelWidthDouble();
//		if (dPixelContour>MAX_CONTOUR)
//			dPixelContour = MAX_CONTOUR;
//        BOOL ok = CBitmapShadow::ContourBitmap(lpInfo,
//											   lpBits,
//											   dPixelContour,
//											   0xFE,			// Threshold
//											   &lpContourInfo,
//											   &lpContourBits,
//											   &OffsetX,
//											   &OffsetY
//											   );
		// Call ContourBitmap in 20 pixel steps - Gavin says this will be faster than
		// doing the whole contour at once and it overcomes the 100 pixel, MAX_CONTOUR
		// limit.
		BOOL ok = TRUE;
		double dPixRemaining = dPixelContour;
		if (dPixelContour<=0)
			ok = FALSE;
		LPBITMAPINFO lpIterInfo = lpInfo;
		LPBYTE lpIterBits = lpBits;
		while (ok && dPixRemaining>0)
		{
			double dPixIteration = dPixRemaining>20 ? 20 : dPixRemaining;

			double dx = 0;
			double dy = 0;
			ok = CBitmapShadow::ContourBitmap(lpIterInfo,
											   lpIterBits,
											   dPixIteration,
											   0xFE,			// Threshold
											   &lpContourInfo,
											   &lpContourBits,
											   &dx,
											   &dy
											   );
			OffsetX += dx;
			OffsetY += dy;

			dPixRemaining = dPixRemaining - dPixIteration;
			if (ok && lpIterInfo!=lpInfo && lpIterBits!=lpBits)
			{
				FreeDIB(lpIterInfo, lpIterBits);
			}
			lpIterInfo = lpContourInfo;
			lpIterBits = lpContourBits;
		}

//		ERROR3IF(!ok, "ContourBitmap failed in RenderShadowedNodeOffscreen");

		if (ok && lpContourInfo && lpContourBits)
		{
			if (m_pSilhouetteBitmap)
				delete m_pSilhouetteBitmap;

			double PixelWidth = m_pOffscreenRR->GetScaledPixelWidthDouble();
			MILLIPOINT newWidth = MILLIPOINT(PixelWidth * lpContourInfo->bmiHeader.biWidth);
			MILLIPOINT newHeight = MILLIPOINT(PixelWidth * lpContourInfo->bmiHeader.biHeight);
			DocCoord newCorner = m_drShadowedBounds.lo;
			newCorner.x += INT32(OffsetX * PixelWidth);
			newCorner.y += INT32(OffsetY * PixelWidth);
			m_drShadowedBounds.lo = newCorner;
			m_drShadowedBounds.hi.x = newCorner.x + newWidth;
			m_drShadowedBounds.hi.y = newCorner.y + newHeight;

			CWxBitmap* wbmpSilhouette	= new CWxBitmap(lpContourInfo, lpContourBits);
			m_pSilhouetteBitmap			= new KernelBitmap(wbmpSilhouette, TRUE);


			// Make sure offscreen bitmap doesn't hang around
			KernelBitmap* pOffBitmap = m_pRendWrap->GetKernelBitmap();
			delete pOffBitmap;
		}
		else
		{
			// ContourBitmap failed!
			// extract the bitmap from the offscreen RR before the RR is closed down.
			m_pSilhouetteBitmap = m_pRendWrap->GetKernelBitmap();
		}
	}
	else
	{
		// extract the bitmap from the offscreen RR before the RR is closed down.
		m_pSilhouetteBitmap = m_pRendWrap->GetKernelBitmap();
	}

	// close down the offscreen render-region and clear our ptrs to prevent future nastiness.
	m_pRendWrap->RestorePreviousRendererState();
	m_pRendWrap		= NULL;
	m_pOffscreenRR	= NULL;

	if (m_pSilhouetteBitmap == NULL)
	{
		ERROR3("BitmapShadower::RenderShadowedNodeOffscreen; Couldn't extract KernelBitmap!");
		return FALSE;
	}

// if desired, insert a debug copy of the silhouette bitmap within the bitmap gallery.
#ifdef ATTACH_SILHOUETTE_BMP
	m_pSilhouetteBitmap->AttachDebugCopyToCurrentDocument("Shadow Silhouette Bitmap");
#endif

	return TRUE;
}
#endif



#endif	// BUILDSHADOWS
