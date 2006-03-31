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
#include "fthrattr.h"

// Algorithms and node classes used for creating feather transp mask
#include "nodecntr.h"		// NodeContour::GenerateContourPathForNode
//#include "doccolor.h"		// DocColour and COLOUR_BLACK
#include "lineattr.h"		// AttrLineWidth
#include "fillval.h"		// BitmapFillAttribute, TransparencyFillAttribute
#include "offscrn.h"		// GRenderRegionWrapper

// Tags, messages and other UI stuff
#include "attrmgr.h"		// enum AttrIndex
//#include "feather.h"		// _R(IDS_FEATHER_ATTR_ID)
//#include "mario.h"			// _R(IDE_NOMORE_MEMORY)

// RenderRegions
#include "grndrgn.h"		// GRenderRegion

// Fiddling with bitmaps - offscreen and feather transp bmps
#include "bitmap.h"
#include "oilbitmap.h"

// Caching contour path and updating on pseudo AllowOps
//#include "opfeathr.h"		// RegenerateFeatherContourAction
#include "objchge.h"		// ObjChange and AllowOp stuff
#include "transop.h"		// TransOperation - optimise which operations trigger recontour

// Blending (creating dynamic attributes)
#include "blendatt.h"		// BlendAttrParam
#include "nodeblnd.h"		// NodeBlend
#include "attrmap.h"		// CCAttrMap

// Special cases
#include "nodedoc.h"		// NodeDocument
//#include "grndclik.h"		// GRenderClick
//#include "grndbrsh.h"		// GRenderBrush

// Saving and loading
#include "cxftags.h"
#include "cxfdefs.h"
#include "cxfrec.h"

// Printing
//#include "saveeps.h"

// Quality settings
#include "qualattr.h"

#include "nodemold.h"
//#include "bitmapcache.h"
#include "pathndge.h"

//DECLARE_SOURCE("$Revision$")

CC_IMPLEMENT_DYNCREATE( FeatherAttrValue, OffscreenAttrValue )
CC_IMPLEMENT_DYNAMIC( AttrFeather, NodeAttribute )
CC_IMPLEMENT_DYNAMIC( FeatherRecordHandler,CamelotRecordHandler )

// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW   

// Uncomment any of the following to attach the associated bitmap into the bitmap gallery
//#define DEBUG_ATTACH_PRECONV_OFFSCREEN_BMP
//#define DEBUG_ATTACH_OFFSCREEN_BMP
//#define DEBUG_ATTACH_SILHOUETTE_BMP
//#define DEBUG_ATTACH_FEATHER_BMP
//#define DEBUG_RENDER_OFFSCREEN_BMP_WITHOUT_MASK

// The maximum feather size which we allow the user to play with.
// If you're thinking of changing this, PLEASE BE AWARE that other/older versions
// of the app will cap feathers loaded/pasted into them at THEIR maximum value.
// As of 30/01/2001, GDraw's maximum blur size is a 200 pixel diameter.
// I'm choosing a user limit of 50 pixels diameter at 96dpi (100% zoom on most systems)
// which gives 50 * (72000 / 96) = 37500 millipoints.
//
const MILLIPOINT FeatherAttrValue::MaxUserFeatherSize = 37500;


/////////////////////////////////////////////////////////////////////////////////////////////
//
//								Helper functions
//
/////////////////////////////////////////////////////////////////////////////////////////////
// TRUE if map contains all necessary line attrs for NodePath::MakeNodePathFromAttributes to succeed
// FALSE otherwise
BOOL CheckLineAttrs(CCAttrMap* pAttrMap)
{
	void* ptr = NULL;
	return (
		pAttrMap->Lookup( CC_RUNTIME_CLASS(AttrLineWidth),(void *&)ptr) &&
		pAttrMap->Lookup( CC_RUNTIME_CLASS(AttrStartArrow),(void *&)ptr) &&
		pAttrMap->Lookup( CC_RUNTIME_CLASS(AttrEndArrow),(void *&)ptr) &&
		pAttrMap->Lookup( CC_RUNTIME_CLASS(AttrJoinType),(void *&)ptr) &&
		pAttrMap->Lookup( CC_RUNTIME_CLASS(AttrStartCap),(void *&)ptr) &&
		pAttrMap->Lookup( CC_RUNTIME_CLASS(AttrDashPattern),(void *&)ptr)
		);
}

BOOL MakeLineAttributeCompleteForContouring(Node* Context, Node* NodeToModify, CCAttrMap* pAttrMap)
{
	AttrLineWidth * pAttrLineWidth = NULL;
	AttrStartArrow * pAttrStartArrow = NULL;
	AttrEndArrow * pAttrEndArrow = NULL;
	AttrJoinType * pAttrJoinType = NULL;
	AttrStartCap * pAttrStartCap = NULL;
	AttrDashPattern * pAttrDashPattern = NULL;
	AttrStrokeColour* pAttrStrokeColour = NULL;

	// get all the attributes out of the attribute map
	pAttrMap->Lookup( CC_RUNTIME_CLASS(AttrLineWidth),(void *&)pAttrLineWidth);
	if(pAttrLineWidth)
		pAttrLineWidth->AttachNode(Context,FIRSTCHILD,FALSE,FALSE);
		
	pAttrMap->Lookup( CC_RUNTIME_CLASS(AttrStartArrow),(void *&)pAttrStartArrow);
	if (pAttrStartArrow)
		pAttrStartArrow->AttachNode(Context,FIRSTCHILD,FALSE,FALSE);

	pAttrMap->Lookup( CC_RUNTIME_CLASS(AttrEndArrow),(void *&)pAttrEndArrow);
	if (pAttrEndArrow)
		pAttrEndArrow->AttachNode(Context,FIRSTCHILD,FALSE,FALSE);

	pAttrMap->Lookup( CC_RUNTIME_CLASS(AttrJoinType),(void *&)pAttrJoinType);
	if(pAttrJoinType)
		pAttrJoinType->AttachNode(Context,FIRSTCHILD,FALSE,FALSE);
		
	pAttrMap->Lookup( CC_RUNTIME_CLASS(AttrStartCap),(void *&)pAttrStartCap);
	if(pAttrStartCap)
		pAttrStartCap->AttachNode(Context,FIRSTCHILD,FALSE,FALSE);
		
	pAttrMap->Lookup( CC_RUNTIME_CLASS(AttrDashPattern),(void *&)pAttrDashPattern);
	if(pAttrDashPattern)
		pAttrDashPattern->AttachNode(Context,FIRSTCHILD,FALSE,FALSE);

	pAttrMap->Lookup( CC_RUNTIME_CLASS(AttrStrokeColour),(void *&)pAttrStrokeColour);
	if(pAttrStrokeColour)
		pAttrStrokeColour->AttachNode(Context,FIRSTCHILD,FALSE,FALSE);

	if( pAttrLineWidth && pAttrStartArrow && pAttrEndArrow && pAttrJoinType && 
		pAttrStartCap && pAttrDashPattern && pAttrStrokeColour)
	{
		Node* pLastChild = Context->FindLastChild();
		NodeToModify->AttachNode(pLastChild,NEXT,FALSE,FALSE);	// dynamic attrs are copies, so we are free to modify them
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


/////////////////////////////////////////////////////////////////////////////////////////////
//
//								AttributeValue classes
//
/////////////////////////////////////////////////////////////////////////////////////////////

/********************************************************************************************
>	FeatherAttrValue::
	Author:		Ilan_Copelyn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/01/2000
	Purpose:	This constructor is called in three different scenarios:-
				1 - to initialise attrvalue tables
				2 - to create a new feather to apply to a selected node by an OpFeather operation
				3 - in the process of creating a simple copy of an exisiting Feather attr
					ie. either a default attr from the attr table (to attach to a new node)
					or  a clone of an AttrFeather __node__ (ie as part of OpClone)
				The initialisation process will always create 0 width default feather attributes
				and hence will always attach 0 width feathers as the documents base feather attr.

	SeeAlso:	
 ********************************************************************************************/
FeatherAttrValue::FeatherAttrValue()
{
//	m_Profile = CProfileBiasGain()				// Defaults to this
	m_GapSize = DEFAULT_GAP_TOLERANCE_MP;
	m_FeatherSize = 0;							// Document base Feather has 0 feather size.
												// The doc base feather is created when camelot
												// initialises a new document.
												// The default feather applied to each new object
												// is also setup by this constructor.
	m_pOuterContour = NULL;
	m_bCached = FALSE;
}

/********************************************************************************************
>	FeatherAttrValue::
	Author:		Ilan_Copelyn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/01/2000
	Purpose:	Free outer contour path if we have one
	SeeAlso:	
 ********************************************************************************************/
FeatherAttrValue::~FeatherAttrValue()
{
	// only m_pOuterContour may be left because it is cached
	if (m_pOuterContour != NULL)
	{
		delete m_pOuterContour;
		m_pOuterContour = NULL;
	}
}

/********************************************************************************************
>	FeatherAttrValue::
	Author:		Ilan_Copelyn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/01/2000
	Purpose:	
	SeeAlso:	
 ********************************************************************************************/
BOOL FeatherAttrValue::Init()
{
	// Default to no feathering
	FeatherAttrValue *pAttr = new FeatherAttrValue;
	if (pAttr==NULL)
		// error message has already been set by new
		return FALSE;

	pAttr->SetDefault();

	UINT32 AttrID = AttributeManager::RegisterDefaultAttribute(CC_RUNTIME_CLASS(NodeRenderableInk),
														 pAttr);

	ERROR2IF(AttrID == ATTR_BAD_ID, FALSE, "Bad ID when Initialising FeatherAttrValue");
	ERROR2IF(AttrID != ATTR_FEATHER, FALSE, "Incorrect ID for FeatherAttrValue");

	return TRUE;
}

/********************************************************************************************
>	FeatherAttrValue::
	Author:		Ilan_Copelyn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/01/2000
	Purpose:	
	SeeAlso:	
	TODO:		Determine whether to copy bitmap or just pointer to it
 ********************************************************************************************/
void FeatherAttrValue::SimpleCopy(AttributeValue *pVal)
{
	ERROR3IF(!pVal->IS_KIND_OF(FeatherAttrValue), "Incorrect runtime class passed to FeatherAttrValue::SimpleCopy");
	FeatherAttrValue* pFthr = (FeatherAttrValue*) pVal;

	m_Profile = pFthr->m_Profile;
	SetFeatherSize(pFthr->GetFeatherSize());
	m_GapSize = pFthr->m_GapSize;

// NOTE! Don't copy m_outerContour because callers assume that this will
// not be copied and rely on it being regenerated on demand. Urgh!
	// Remember to copy or blank cached path data without leaking memory...
	if (m_pOuterContour)
		delete m_pOuterContour;
	m_pOuterContour = NULL;
/*
	Path* pSrcPath = pFthr->GetOuterContour();
	if (pSrcPath)
	{
		m_pOuterContour = new Path();
		m_pOuterContour->Initialise(pSrcPath->GetNumCoords());
		m_pOuterContour->CopyPathDataFrom(pSrcPath);
	}
*/
	// preserves default flag to avoid confusion when copying CCAttrMaps
	GeometryLinkedAttrValue::SimpleCopy(pVal);
}
	
/********************************************************************************************
>	FeatherAttrValue::
	Author:		Ilan_Copelyn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/01/2000
	Purpose:	
	SeeAlso:	
 ********************************************************************************************/
NodeAttribute* FeatherAttrValue::MakeNode()
{
	AttrFeather* pNode = new AttrFeather();
	pNode->Value.SimpleCopy(this);
	return pNode;
}

/********************************************************************************************
>	FeatherAttrValue::
	Author:		Ilan_Copelyn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/01/2000
	Purpose:	
	SeeAlso:	
 ********************************************************************************************/
BOOL FeatherAttrValue::Blend(BlendAttrParam* pBlendParam)
{
	// First get the fill that we are blending to
	FeatherAttrValue* OtherFthr = (FeatherAttrValue*)pBlendParam->GetOtherAttrVal();

	if(IsDefaultFlagSet() && OtherFthr->IsDefaultFlagSet())
	{
		// no blending required
//		pBlendParam->SetBlendedAttrVal(NULL);
		return FALSE;
	}


	FeatherAttrValue* pNewAttr = new FeatherAttrValue;

	if (pNewAttr == NULL)
	{
		// Fail if we couldn't create the new fill
		pBlendParam->SetBlendedAttrVal(NULL);
		return FALSE;
	}

	// and what point along the blend we are at
	double Ratio = pBlendParam->GetBlendRatio();

	// blend m_pFeatherWidth
	MILLIPOINT Start = GetFeatherSize();
	MILLIPOINT End = OtherFthr->GetFeatherSize();
	pNewAttr->SetFeatherSize( (MILLIPOINT) ( Start + Ratio*(End - Start) ) );

	// blend m_Profile
	CProfileBiasGain BiasGain = m_Profile;						// the first fills profile
	CProfileBiasGain OtherBiasGain = OtherFthr->m_Profile;		// the other fills profile
	if (!(BiasGain == OtherBiasGain))
	{
/*		From other blend Profiles code
		double InvRatio = 1 - Ratio;
		double BlendedBias = (BiasGain.GetBias () * InvRatio) + (OtherBiasGain.GetBias () * Ratio);
		double BlendedGain = (BiasGain.GetGain () * InvRatio) + (OtherBiasGain.GetGain () * Ratio);
*/
		double StartBias = m_Profile.GetBias(); double EndBias = OtherBiasGain.GetBias();
		double StartGain = m_Profile.GetGain(); double EndGain = OtherBiasGain.GetGain();
		double BlendedBias = StartBias + ((EndBias - StartBias) * Ratio);
		double BlendedGain = StartGain + ((EndGain - StartGain) * Ratio);

		pNewAttr->m_Profile.SetBias (BlendedBias);
		pNewAttr->m_Profile.SetGain (BlendedGain);
	}

	// Set the new fill as the blended attribute
	pBlendParam->SetBlendedAttrVal(pNewAttr);

	return TRUE;
}

/********************************************************************************************
>	FeatherAttrValue::
	Author:		Ilan_Copelyn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/01/2000
	Purpose:	Put the GRenderRegion offscreen rendering system into play (provided we're
				dealing with a real feather attribute and not just the base doc attr).
	SeeAlso:	GRenderRegion::SetFeathering, FeatherAttrValue::OffscreenRenderingCompleted()
 ********************************************************************************************/
void FeatherAttrValue::Render(RenderRegion *pRender, BOOL Temp)
{
//	TRACEUSER( "Gerry", _T("Rendering feather attribute of %d (0x%08x)\n"), GetFeatherSize(), this);
	
	// Karim 20/07/2000
	// currently, we only generate an offscreen bitmap for GRenderRegions, although we are
	// 'rendered' into all RR's. therefore we must quit quietly if pRender is not a GRR.
	// Gerry 12/01/2005
	// This is no longer true.  We need to render this into all types of render region
	// so the code has been removed
	
//	if (!pRender->IS_KIND_OF(GRenderRegion))
//		return;

	// if the feather size is zero, then quit now - no feathering is necessary.
	MILLIPOINT mpFeather = GetFeatherSize();
	if (mpFeather == 0)
		return;

	// ... and shout out if the feather size is negative - something is WRONG!
	else if (mpFeather < 0)
	{
		ERROR3("Negative feather size! What's going on?");
		return;
	}

	// you would place code here if you need to affect currentAttrs table
	// also alloc any mem you need for the duration of the rendering cycle
	OffscreenAttrValue::Render(pRender, Temp);
}

/********************************************************************************************
>	FeatherAttrValue::
	Author:		Ilan_Copelyn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/01/2000
	Purpose:	
	SeeAlso:	GRenderRegion::RestoreGDrawBitmap
 ********************************************************************************************/
void FeatherAttrValue::Restore(RenderRegion *pRegion, BOOL Temp)
{
//TRACEUSER( "Phil", _T("Restoring feather attribute of %d (0x%08x)\n"), GetFeatherSize(), this);
	// NB this function doesn't need to be implemented - the base class handles everything
	// Just showing you where to put extra code

	OffscreenAttrValue::Restore(pRegion, Temp);
}

/********************************************************************************************
>	FeatherAttrValue::
	Author:		Ilan_Copelyn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/01/2000
	Purpose:	The random nature of generation of redraw cliprect coordinates means that we
				may sometimes be asked to redraw a really thin sliver of the feathered object.
				This is a problem for the convolution code if the sliver is thinner (in height
				or width) than 2 pixels.
	SeeAlso:	GRenderRegion::RestoreGDrawBitmap
 ********************************************************************************************/
void FeatherAttrValue::GetMinimumOffscreenBmpDimensions(UINT32* RequiredWidth, UINT32* RequiredHeight)
{
	*RequiredWidth = 2;
	*RequiredHeight = 2;
}

/********************************************************************************************
>	FeatherAttrValue::
	Author:		Ilan_Copelyn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/01/2000
	Purpose:	To allow the offscreen attribute to perform whatever processing it requires on
				the bitmap copy of the subtree rooted at m_pLinkedNode and then draw into the partially
				rendered bitmap of the predecessors of m_pLinkedNode

	Notes:		1)	This function gets called when the RR exits the context under which the
				attribute was Render()ed (ie after completeing a DrawPath("this->m_pLinkedNode->Path").
				2)	The offscreen rendering system (implemented in GRenderRegion) has already
				restored the clip rect and GDraw bitmap and matrix state of the previous offscreen
				attribute.
					This function is actually being called by the RenderRegion in the process of
				Restore()ing this previous offscreen attribute (ie the process of taking the attribute
				which is sitting on top of the attribute context stack, and restoring it to the
				CurrentAttrs table).
					So at this point the GRenderRegion and GDraw context are back the way they
				were before "this" offscreen attribute was Render()ed.
				3)	m_pOffscreenBmpBits points to the buffer holding the fully rendered bitmap
				of the subtree rooted at m_pLinkedNode
				4)	Any transparency attribute existing at the same level as this offscreen attr
				(ie attached to "this->m_pLinkedNode") will already have affected the bitmap in
				"this->m_pOffscreenBmpBits".
				5)	You don't need to delete the bitmap pointers UNLESS you installed "this" attribute
				as a non-temporary attribute in the Render() function (ie SetFeathering(this, FALSE..)).
				If it is a temporary attribute (which is the default) then the deconstructor will handle
				the deallocation of the memory pointed to by m_pOffscreenBmpBits & m_pOffscreenBmpInfo
	SeeAlso:	GRenderRegion::RestoreFeathering(); ~FeatherAttrValue(); RenderRegion::RestoreAttribute()
				FeatherAttrValue::Render(), GRenderRegion::SetFeathering()
 ********************************************************************************************/
void FeatherAttrValue::OffscreenRenderingCompleted(RenderRegion *pRender, LPBITMAPINFO lpBitmapInfo, LPBYTE lpBits, DocRect BitmapRect)
{
//#pragma message( __LOCMSG__ "FeatherAttrValue::OffscreenRenderingCompleted - do nothing" )
//	TRACE( _T("Warning - FeatherAttrValue::OffscreenRenderingCompleted called\n") );
//	TRACEUSER( "Gerry", _T("Feather attribute OffscreenRenderingCompleted (0x%08x)\n"), this);
//	TRACEUSER( "Phil", _T("Feather attribute OffscreenRenderingCompleted (0x%08x)\n"), this);

// TEMP BODGE
	m_OffBB = BitmapRect;

	// Karim 20/07/2000
	// currently, we only generate an offscreen bitmap for GRenderRegions, although we are
	// 'rendered' into all RR's. therefore we must quit quietly if pRender is not a GRR.
	if (!pRender->IS_KIND_OF(GRenderRegion))
		return;

	GRenderRegion* pGRR = (GRenderRegion*)pRender;
	RRCaps Caps;
	pGRR->GetRenderRegionCaps(&Caps);
	if (!Caps.DoesRegionDoAll())
		return;

	// Karim 19/10/2000
	// The offscreen rendering may possibly fail,
	// in which case all we can do now is quit.
	if (lpBitmapInfo == NULL || lpBits == NULL)
	{
//		ERROR3("FeatherAttrValue::OffscreenRenderingCompleted; don't have any bitmap info!");
		return;
	}

	double fScaledPixelWidth = pGRR->GetScaledPixelWidthDouble();
//	TRACEUSER( "Gerry", _T("ScaledPixelWidth = %f\n"), fScaledPixelWidth);
	double pixBlurDiameter = GetFeatherPixelSize(fScaledPixelWidth);
//	TRACEUSER( "Gerry", _T("BlurDiameter = %f\n"), pixBlurDiameter);

	// Setup path to draw
	Path* pPathToDraw = GetVisibleBoundingPath();
	if (pPathToDraw == NULL)
	{
		ERROR3("FeatherAttrValue::OffscreenRenderingCompleted; Couldn't create path!");
		return;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	//	Create the bitmap fill containing the offscreen rendition of the objects to feather.
	/////////////////////////////////////////////////////////////////////////////////////////

	CWxBitmap		   *wBitmap = NULL;
	KernelBitmap	   *OffscreenBitmap = NULL;

//	TRACEUSER( "Gerry", _T("Offscreen size = (%d, %d)\n"), pOffBmpHdr->biWidth, pOffBmpHdr->biHeight);

	// Create a Kernel bitmap from the bmp data.
	wBitmap			= new CWxBitmap(lpBitmapInfo, lpBits);
	OffscreenBitmap	= new KernelBitmap(wBitmap,TRUE);

	// Setup the bitmap fill.
	// Since the offscreen bitmap has been plotted using the full rendering matrix
	// which may include a rotation during printing, we must now force GDraw to
	// render it using the same rotation (see GRenderRegion::AllocateOffScreenState)
	BitmapFillAttribute BmpFill;
	ANGLE rot;
	Matrix m = pGRR->GetMatrix();
	m.Decompose(NULL, NULL, &rot, NULL, NULL);
	if (rot==0)
//		CreateBitmapFill(OffscreenBitmap, &m_OffBB, &BmpFill);
		CreateBitmapFill(OffscreenBitmap, &BitmapRect, &BmpFill);
	else
//		CreateBitmapFillRot90(OffscreenBitmap, &m_OffBB, &BmpFill);
		CreateBitmapFillRot90(OffscreenBitmap, &BitmapRect, &BmpFill);
	// No other rotations are currently possible...

// DEBUG: add offscreen bmp to bitmap gallery
#ifdef DEBUG_ATTACH_OFFSCREEN_BMP
	OffscreenBitmap->AttachDebugCopyToCurrentDocument("Offscreen Bitmap");
#endif


	/////////////////////////////////////////////////////////////////////////////////////////
	//	Create the feather bitmap transparency mask.
	/////////////////////////////////////////////////////////////////////////////////////////

	KernelBitmap* FeatherBitmap = CreateFeatherBitmap(pGRR, pixBlurDiameter);

	BitmapTranspFillAttribute BmpTranspFill;
	if (FeatherBitmap != NULL)
	{
		CreateBitmapTranspFill(FeatherBitmap, &m_drFeatherBB, &BmpTranspFill);
	}


	/////////////////////////////////////////////////////////////////////////////////////////
	//	Render the bitmaps together for the final feathered result.
	//
	//	If we had any problems creating the feather bitmap transparency, or if our debug
	//	rendering is enabled, then we try to fail gracefully by rendering just the
	//	offscreen bitmap. Note that this bitmap will only have an alpha channel if some of
	//	the feathered objects require it, and will have a white background otherwise.
	/////////////////////////////////////////////////////////////////////////////////////////
	pGRR->SaveContext();

// DEBUG: see note above.
#ifndef DEBUG_RENDER_OFFSCREEN_BMP_WITHOUT_MASK
	if (FeatherBitmap != NULL)
		pGRR->RenderBitmapWithTransparency(pPathToDraw, &BmpFill, &BmpTranspFill);
#else
	pGRR->RenderBitmapWithTransparency(pPathToDraw, &BmpFill, NULL);
#endif

	pGRR->RestoreContext();

	/////////////////////////////////////////////////////////////////////////////////////////
	//	Clean up all our data structures.
	/////////////////////////////////////////////////////////////////////////////////////////
	if (FeatherBitmap != NULL)
	{
		delete FeatherBitmap;
		FeatherBitmap = NULL;
	}

	if (pPathToDraw != NULL)
	{
		delete pPathToDraw;
		pPathToDraw = NULL;
	}

	FreeDIB(lpBitmapInfo, lpBits, NULL, FALSE);

	// makes ~CWxBitmap() think that bmp HasBeenDeleted().
	wBitmap->BMBytes = ( (CWxBitmap *)OILBitmap::Default )->BMBytes;
	delete OffscreenBitmap;
}




/********************************************************************************************

>	virtual BOOL FeatherAttrValue::CreateFeatherTransp(	GRenderRegion* pGRR,
														Path* pPath,
														double dPixelWidth,
														BitmapTranspFillAttribute** ppFeatherTransp,
														KernelBitmap** ppFeatherBitmap
														)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/08/2005
	Inputs:		pPath			- Pointer to path to be feathered
				dPixelWidth		- Pixel width in millipoints
	Outputs:	pFeatherTransp	- Pointer to transparency attribute for feathering
				pFeatherBitmap	- POinter to feather bitmap used in transparency attribute
	Returns:	TRUE if it worked
				FALSE otherwise
	Purpose:	Create the feather transparency attribute using the supplied path /without/
				reference to any surrounding nodes in the tree
				Used by NodeBitmap::GetDirectBitmap

********************************************************************************************/
BOOL FeatherAttrValue::CreateFeatherTransp(	GRenderRegion* pGRR,
										    Path* pPath,
											double dPixelWidth,
											BitmapTranspFillAttribute** ppFeatherTransp,
											KernelBitmap** ppFeatherBitmap
											)
{
	double dpixBlurDiameter = GetFeatherPixelSize(dPixelWidth);
	LPBYTE			pFeatherBits	= NULL;
	LPBITMAPINFO	pFeatherInfo	= NULL;
	KernelBitmap*	kbmpFeather		= NULL;
	DocRect			rectBounds		= pPath->GetBoundingRect();
//	UINT32			pixWidth		= UINT32(floor( rectBounds.Width() / dPixelWidth + 0.5 ));
//	UINT32			pixHeight		= UINT32(floor( rectBounds.Height() / dPixelWidth + 0.5 ));

	// -------------------------------------------------------------------
	// First make the silhouette bitmap
	// create the silhouette bitmap.
	m_OffBB = rectBounds;
	m_GapSize = 0;
	LPBYTE			pSilhouetteBits;
	LPBITMAPINFO	pSilhouetteInfo = CreateSilhouetteBitmap(pGRR, &pSilhouetteBits, pPath);

	if (pSilhouetteInfo==NULL || pSilhouetteBits==NULL)
		return FALSE;

	// -------------------------------------------------------------------
	// Next blur the silhouette bitmap to obtain the feather bitmap.
	pFeatherInfo = CBitmapShadow::Feather8BppBitmap(
													dpixBlurDiameter,
													pSilhouetteInfo,
													pSilhouetteBits,
													&pFeatherBits
												);

	// If Feather8BPP returned the same bitmap, clear silhouette pointers so we don't
	// deallocate the bitmap at the end of this function...
	if (pFeatherInfo == pSilhouetteInfo)
	{
		pSilhouetteInfo = NULL;
		pSilhouetteBits = NULL;
	}

	if (pFeatherInfo != NULL && pFeatherBits != NULL)
	{
		CWxBitmap*		wbmpFeather = new CWxBitmap(pFeatherInfo, pFeatherBits);
						kbmpFeather = new KernelBitmap(wbmpFeather, TRUE);

		SetupFeatherBitmapPalette(kbmpFeather);

		*ppFeatherBitmap = kbmpFeather;
	}

	// We don't need the silhouette bitmap any more
	if (pSilhouetteInfo != NULL && pSilhouetteBits != NULL)
		FreeDIB(pSilhouetteInfo, pSilhouetteBits);

	// If we failed to create the blurred bitmap then exit now
	if (*ppFeatherBitmap==NULL)
		return FALSE;

	// ----------------------------------------------------------------------
	// Now make the transparency attribute to carry the feather bitmap
	*ppFeatherTransp  = new BitmapTranspFillAttribute;
	if (*ppFeatherTransp==NULL)
	{
		delete *ppFeatherBitmap;
		*ppFeatherBitmap = NULL;
		return FALSE;
	}

	CreateBitmapTranspFill(*ppFeatherBitmap, &rectBounds, *ppFeatherTransp);

	return TRUE;
}




/********************************************************************************************

>	virtual BOOL FeatherAttrValue::DoesOffscreenBmpRequireTransp(GRenderRegion* pGRR)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/11/2000

	Inputs:		pGRR	the RR which the feather will be rendered into.

	Purpose:	Override of OffscreenAttrValue virtual function.
				Feathers always require a transparent bitmap if their feather size is less
				than a certain value, otherwise they use the default implementation.

				This prevents white artifacts on small feather sizes.

	See also:	CreateSilhouetteBitmap, OffscreenAttrValue::DoesOffscreenBmpRequireTransp.

********************************************************************************************/
BOOL FeatherAttrValue::DoesOffscreenBmpRequireTransp(GRenderRegion* pGRR)
{
// In conjunction with DoesOffscreenBMPCaptureBackground this function should
// now return FALSE always so that the offscreen bitmap does not attempt to
// capture transparency info but rather captures the RESULTS of transparency...
	if (DoesOffscreenBMPCaptureBackground())
		return FALSE;

//#pragma message( __LOCMSG__ "Removed GRenderRegion usage" )
//		return false;
	if (pGRR == NULL)
	{
		ERROR3("FeatherAttrValue::DoesOffscreenBmpRequireTransp; Invalid params!");
		return FALSE;
	}

	const double ScaledPixelWidth = pGRR->GetScaledPixelWidthDouble();
	const double FeatherPixelSize = GetFeatherPixelSize(ScaledPixelWidth);

	return (FeatherPixelSize < 2*MIN_BLUR_DIAMETER) ? TRUE :
			OffscreenAttrValue::DoesOffscreenBmpRequireTransp(pGRR) ;
}



/********************************************************************************************

>	KernelBitmap* FeatherAttrValue::CreateFeatherBitmap(GRenderRegion* pGRR,
														double pixBlurDiameter)
	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19 September 2000
	Inputs:		pGRR				the GRenderRegion to base the silhouette bitmap on.
				pixBlurDiameter		feather blur width, in pixels (can be non-integer).

	Returns:	A ptr to a KernelBitmap containing the transparency mask part of the feather.

	Purpose:	Create the feather bitmap-transparency mask.

	See Also:	CreateSilhouetteBitmap().

********************************************************************************************/
KernelBitmap* FeatherAttrValue::CreateFeatherBitmap(GRenderRegion* pGRR,
													double pixBlurDiameter)
{
	// will contain the feather bitmap transparency mask.
	LPBYTE			pFeatherBits	= NULL;
	LPBITMAPINFO	pFeatherInfo	= NULL;
	KernelBitmap*	kbmpFeather		= NULL;

	// create the silhouette bitmap.
	LPBYTE			pSilhouetteBits;
	LPBITMAPINFO	pSilhouetteInfo = CreateSilhouetteBitmap(pGRR, &pSilhouetteBits);

	if (pSilhouetteInfo != NULL && pSilhouetteBits != NULL)
	{
//		TRACEUSER( "Gerry", _T("Silhouette size = (%d, %d)\n"), pSilhouetteInfo->bmiHeader.biWidth, pSilhouetteInfo->bmiHeader.biHeight);

		// blur the silhouette bitmap to obtain the feather bitmap.
		pFeatherInfo = CBitmapShadow::Feather8BppBitmap(
														pixBlurDiameter,
														pSilhouetteInfo,
														pSilhouetteBits,
														&pFeatherBits
													);

		// If Feather8BPP returned the same bitmap, clear silhouette pointers so we don't
		// deallocate the bitmap at the end of this function...
		if (pFeatherInfo == pSilhouetteInfo)
		{
			pSilhouetteInfo = NULL;
			pSilhouetteBits = NULL;
		}

		if (pFeatherInfo != NULL && pFeatherBits != NULL)
		{
//			TRACEUSER( "Gerry", _T("Feather size = (%d, %d)\n"), pFeatherInfo->bmiHeader.biWidth, pFeatherInfo->bmiHeader.biHeight);
			CWxBitmap*		wbmpFeather = new CWxBitmap(pFeatherInfo, pFeatherBits);
							kbmpFeather = new KernelBitmap(wbmpFeather, TRUE);

			SetupFeatherBitmapPalette(kbmpFeather);
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////
//
// DEBUG: add copies of the silhouette and feather bitmaps to the bitmap gallery.
//
#ifdef DEBUG_ATTACH_SILHOUETTE_BMP
	if (pSilhouetteInfo != NULL && pSilhouetteBits != NULL)
	{
		CWxBitmap*		wbmpSilhouette = new CWxBitmap(pSilhouetteInfo, pSilhouetteBits);
		KernelBitmap*	kbmpSilhouette = new KernelBitmap(wbmpSilhouette, TRUE);

		SetupFeatherBitmapPalette(kbmpSilhouette);
		kbmpSilhouette->AttachDebugCopyToCurrentDocument("Silhouette Bitmap");

		wbmpSilhouette->BMBytes = ((CWxBitmap*)OILBitmap::Default)->BMBytes;
		delete kbmpSilhouette;
	}
#endif

#ifdef DEBUG_ATTACH_FEATHER_BMP
	if (kbmpFeather != NULL)
	{
		kbmpFeather->AttachDebugCopyToCurrentDocument("Feather Mask Bitmap");
	}
#endif
//
/////////////////////////////////////////////////////////////////////////////////////////////

	// Silhouette bitmap does not use limited mem manager, so free its DIB ourself.
	if (pSilhouetteInfo != NULL && pSilhouetteBits != NULL)
		FreeDIB(pSilhouetteInfo, pSilhouetteBits, NULL, FALSE);

	return kbmpFeather;
}



/********************************************************************************************
>	FeatherAttrValue::
	Author:		Ilan_Copelyn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/04/2000
	Inputs:		ppBmpBits = pointer to a LPBYTE pointer which we should allocate memory for
	Outputs:	ppBmpBits = monochrome bitmap of the outline of the feathered object(s)
	Returns:	BitmapInfo structure describing the mono bitmap @ **ppBmpBits
	Purpose:	Create a monochrome image of the accumulated path, using a white outline so the
				resulting image matches the inner contour of the path. The thickness of the line
				is determined by the feather width.
				NB line width is centred about the path outline. Hence if we require the inner
				contour to be featherwidth pixels inside the outline, then need to use a line
				width of 2*FeatherSize
				NB2 Feathersize is moderated to max 40 pix
	SeeAlso:	FeatherAttrValue::GetFeatherSize,OffscreenRenderingCompleted,Render
				PathAccumulatorPathProcessor
				OffscreenRenderer
	TechnicalNotes: leverages the offscreen rendering system to get the active renderegion to
					render the path into a new bitmap and then restore the renderregion to the
					way it was.
					
					I order to avoid DWORD aligning overhead in the convolution code we always
					render into bitmaps which are DWORD aligned. This means we need to be careful
					when creating a transparency attribute to hold the mask as we must use
					the aligned co-ords, not m_OffBB otherwise we will introduce scaling.
					NB we do this by drawing more pixels on the right hand border to account
					for the alignment.
	NB NB NB
	Non-technical notes: these notes are out of date so ignore! will fix later.
 **********************************************************************************************/
LPBITMAPINFO FeatherAttrValue::CreateSilhouetteBitmap(GRenderRegion* GRR,
													  LPBYTE* ppBmpBits,
													  Path* pOutline)
{
//#pragma message( __LOCMSG__ "FeatherAttrValue::CreateSilhouetteBitmap - do nothing" )
//	TRACE( _T("Warning - FeatherAttrValue::CreateSilhouetteBitmap called\n") );
//	return NULL;

//	ERROR3IF(m_pOffscreenBmpInfo == NULL,
//			"FeatherAttrValue::CreateSilhouetteBitmap; No offscreen bitmap info.");

	// generate a contour of m_GapSize around the path of our linked node, to
	// eradicate any small gaps which would otherwise be feathered.
	if (pOutline==NULL)
	{
		if (m_pOuterContour == NULL)
			GenerateOuterContour();

		pOutline = m_pOuterContour;
	}

	// if we couldn't generate our outer contour, or it had no points in it,
	// don't try to produce a feather bitmap.
	if (pOutline == NULL || pOutline->GetNumCoords() == 0)
	{
		*ppBmpBits = NULL;
		return NULL;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	//	-	Work out the appropriate size silhouette bitmap by inflating the invalid
	//		offscreen rectangle by the blur *radius* (half the feather size).
	//	-	If the blur radius is above the maximum we can do, then we work out *how*
	//		much bigger it is and render the silhouette bitmap appropriately scaled down.
	//	-	The final feather bitmap will then scale up to fill the offscreen rect,
	//		making it look like we actually used the requested blur radius.
	const	double	ScaledPixelWidth	= GRR->GetScaledPixelWidthDouble();
	const	double	FeatherMPSize		= GetFeatherSize();
	const	double	FeatherPixelSize	= GetFeatherPixelSize(ScaledPixelWidth);
			double	FeatherScaleFactor	= 1;

	DocRect drSilhouetteBounds = m_OffBB;
	m_drFeatherBB = drSilhouetteBounds;
	if (FeatherPixelSize >= MIN_BLUR_DIAMETER)
	{
		// requested blur diameter	= BlurPixDiam pixels.
		// actual blur we can do	= FeatherPixelSize pixels.
		double BlurPixDiam = (ScaledPixelWidth > 0) ? (FeatherMPSize / ScaledPixelWidth) : 0;
		if (BlurPixDiam > FeatherPixelSize)
		{
			FeatherScaleFactor = BlurPixDiam / FeatherPixelSize;

			// Scale the silhouette around its centre by the feather scale factor.
			// This is so that the rendered silhouette bitmap is has a width of
			// at least (2 * FeatherRadius + 1) pixels.
			double BoundsSF = (FeatherScaleFactor - 1.0) / 2.0;
			UINT32	nWidth	= (UINT32)(0.5 + (double)drSilhouetteBounds.Width() * BoundsSF);
			UINT32	nHeight	= (UINT32)(0.5 + (double)drSilhouetteBounds.Height() * BoundsSF);
			drSilhouetteBounds.Inflate(nWidth, nHeight);
			m_drFeatherBB = drSilhouetteBounds;
		}

		// Inflate the silhouette bounds by FeatherRadius pixels around each edge.
		// This will shrink down to the correct bounds when blurred.
		const UINT32 nBlur = (UINT32)(BlurPixDiam +  0.5);

		UINT32 nHiInflate = (nBlur - 1) / 2;
		UINT32 nLoInflate = (nBlur - 1) - nHiInflate;

		nHiInflate = (UINT32)(ScaledPixelWidth * (double)nHiInflate);
		nLoInflate = (UINT32)(ScaledPixelWidth * (double)nLoInflate);

		drSilhouetteBounds.hi.x += nHiInflate;
		drSilhouetteBounds.hi.y += nHiInflate;
		drSilhouetteBounds.lo.x -= nLoInflate;
		drSilhouetteBounds.lo.y -= nLoInflate;

		// Don't allow further code to try to make a zero pixel silhouette bitmap
		if (drSilhouetteBounds.Width()<ScaledPixelWidth || drSilhouetteBounds.Height()<ScaledPixelWidth)
		{
			*ppBmpBits = NULL;
			return NULL;
		}
	}
	/////////////////////////////////////////////////////////////////////////////////////////

	// create our offscreen render-region.
	ConcurrentRenderer* pRendWrap = NULL;
	pRendWrap = GRenderRegionWrapper::GetConcurrentRenderer(GRR, 1.0 / FeatherScaleFactor,
															drSilhouetteBounds, 8, FALSE);
	if (pRendWrap == NULL)
	{
		*ppBmpBits = NULL;
		return NULL;
	}

	GRenderRegion* pLocalGRR = NULL;
	pLocalGRR = pRendWrap->GetRenderRegion();

	pLocalGRR->GetDrawContext()->SetHintingFlag(FALSE);

	// ask the bitmap shadower if it the final feather bitmap will be offset diagonally
	// by half a pixel. if this is the case then we must compensate, by rendering
	// the silhouette bitmap at a half-pixel offset in the other direction.
	if (CBitmapShadow::BlurringWillOffsetBitmap(FeatherPixelSize))
		pLocalGRR->OffsetByHalfPixel();

	////////////////////////////////////////////////////////////////////
	// Draw into new RenderRegion which GRenderRegionWrapper has created
	////////////////////////////////////////////////////////////////////
	pLocalGRR->SaveContext();

	// Initialise our new render-region:
	//	1.	Render in solid black with solid white line-widths.
	//	1a.	Line colour now graduates from black -> off-white over 0->2*MIN_BLUR_DIAMETER.
	//	2.	Line width is half feather size + contour gap size, on either side.
	//	3.	No arrowheads/dashed lines/fancy join types.
	//	4.	High quality rendering, so we get anti-aliased lines.

	pLocalGRR->SetFillColour(COLOUR_BLACK);
	if (FeatherPixelSize < (2*MIN_BLUR_DIAMETER))
	{
		double RampValue = FeatherPixelSize / (2*MIN_BLUR_DIAMETER);
		ColourValue GreyVal(0.8 * RampValue);
		DocColour FeatherGrey(GreyVal, GreyVal, GreyVal);
		pLocalGRR->SetLineColour(FeatherGrey);
	}
	else
		pLocalGRR->SetLineColour(COLOUR_WHITE);

	pLocalGRR->SetJoinType(RoundJoin);
	pLocalGRR->SetStartArrow(SA_NULLARROW);
	pLocalGRR->SetEndArrow(SA_NULLARROW);
	pLocalGRR->SetDashPattern(SD_SOLID);

	MILLIPOINT RequiredLineWidth = (MILLIPOINT)(FeatherMPSize + 2*m_GapSize);
	pLocalGRR->SetLineWidth(RequiredLineWidth);

	FlatTranspFillAttribute NoFillTransp(TT_NoTranspType);
	pLocalGRR->SetTranspFillGeometry(&NoFillTransp, FALSE);

	StrokeTranspAttribute NoLineTransp(TT_NoTranspType);
	pLocalGRR->SetLineTransp(&NoLineTransp, FALSE);

	Quality AntiAliasQuality;
	AntiAliasQuality.SetQuality(Quality::QualityMax);
	QualityAttribute AntiAliasQualityAttr(AntiAliasQuality);
	pLocalGRR->SetQuality(&AntiAliasQualityAttr, FALSE);

	// render our contoured silhouette path into the new RR.
	pOutline->IsFilled	= TRUE;
	pOutline->IsStroked	= TRUE;
	pLocalGRR->DrawPath(pOutline);
	pLocalGRR->RestoreContext();

	////////////////////////////////////////////////////////////////////
	// Extract bitmap from GRenderRegionWrapper and restore GDraw state
	////////////////////////////////////////////////////////////////////
	LPBITMAPINFO tmpBI;
	pRendWrap->GetBitmapPointers(&tmpBI, ppBmpBits);
	pRendWrap->RestorePreviousRendererState();

	return tmpBI;
}



/********************************************************************************************
>	FeatherAttrValue::
	Author:		Ilan_Copelyn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/01/2000
	Purpose:	Setup the feather bitmap's palette using the bias/gain profile
	SeeAlso:	
 **********************************************************************************************/
void FeatherAttrValue::SetupFeatherBitmapPalette(KernelBitmap* pFeatherBMP)
{
	LPRGBQUAD pPalette = pFeatherBMP->GetPaletteForBitmap();

	// Using linear palette
	// For some reason we have to initialise the palette even though the rendering
	// code sets up the palette using the profile which is attached to the bitmap
	// transparency attribute which contains the feather bitmap mask.
	for (INT32 i = 0; i < 256; i ++)
	{
		pPalette[i].rgbRed = pPalette[i].rgbBlue = pPalette[i].rgbGreen = (BYTE)i;
		pPalette[i].rgbReserved = 0x00;
	}

	// Mark this bitmap as being greyscale to avoid re-conversion by GetGreyscaleVersion
	pFeatherBMP->SetAsGreyscale();
}

/********************************************************************************************
>	FeatherAttrValue::
	Author:		Ilan_Copelyn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/01/2000
	Purpose:	
	SeeAlso:	
 ********************************************************************************************/
void FeatherAttrValue::CreateBitmapFill(KernelBitmap* pBitmap, DocRect* dr, BitmapFillAttribute* BmpFill)
{
	// Bitmap fill is the size of the current offscreen bitmap
	DocCoord StartPoint(dr->lo);
	DocCoord EndPoint(dr->hi.x,dr->lo.y);
	DocCoord EndPoint2(dr->lo.x,dr->hi.y);
	DocCoord EndPoint3(dr->hi);

	BmpFill->SetStartPoint(&StartPoint);
	BmpFill->SetEndPoint(&EndPoint);
	BmpFill->SetEndPoint2(&EndPoint2);
	BmpFill->SetEndPoint3(&EndPoint3);
	BmpFill->GetBitmapRef()->SetBitmap(pBitmap);
}

/********************************************************************************************
>	FeatherAttrValue::
	Author:		Ilan_Copelyn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/01/2000
	Purpose:	
	SeeAlso:	
 ********************************************************************************************/
void FeatherAttrValue::CreateBitmapFillRot90(KernelBitmap* pBitmap, DocRect* dr, BitmapFillAttribute* BmpFill)
{
	// Bitmap fill is the size of the current offscreen bitmap
	DocCoord StartPoint(dr->hi.x, dr->lo.y);
	DocCoord EndPoint(dr->hi);
	DocCoord EndPoint2(dr->lo);
	DocCoord EndPoint3(dr->lo.x,dr->hi.y);

	BmpFill->SetStartPoint(&StartPoint);
	BmpFill->SetEndPoint(&EndPoint);
	BmpFill->SetEndPoint2(&EndPoint2);
	BmpFill->SetEndPoint3(&EndPoint3);
	BmpFill->GetBitmapRef()->SetBitmap(pBitmap);
}

void FeatherAttrValue::CreateBitmapTranspFill(KernelBitmap* pFeather, DocRect* dr, BitmapTranspFillAttribute* BmpTranspFill)
{
	// Transp fill is same size as viewable part of the bitmap 
	DocCoord StartPoint(dr->lo);
	DocCoord EndPoint(dr->hi.x,dr->lo.y);
	DocCoord EndPoint2(dr->lo.x,dr->hi.y);
	DocCoord EndPoint3(dr->hi);

	BmpTranspFill->SetStartPoint(&StartPoint);
	BmpTranspFill->SetEndPoint(&EndPoint);
	BmpTranspFill->SetEndPoint2(&EndPoint2);
	BmpTranspFill->SetEndPoint3(&EndPoint3);
	BmpTranspFill->GetBitmapRef()->SetBitmap(pFeather);
	BmpTranspFill->Transp = 0;
	BmpTranspFill->EndTransp = 255;
	m_Profile.SetIsAFeatherProfile (TRUE);		// enable extra processing on the profile
	BmpTranspFill->SetProfile(m_Profile);
}

Path* FeatherAttrValue::GetVisibleBoundingPath()
{
	DocRect dr = GetOffscreenBoundingRect();
	Path* p = new Path();
	if (p != NULL)
	{
		p->Initialise(5);
		p->CreatePathFromDocRect(&dr);
	}
	return p;
}

/********************************************************************************************
>	FeatherAttrValue::
	Author:		Ilan_Copelyn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/01/2000
	Inputs:		pNode	- node to feather (possibly compound node)
				size	- of feather region (NB millipoint = INT32, pixels = INT32)
						- "INT32" rather than "MILLIPOINT" Width used to emphasize that
						   millipoints are expected but not only measure excepted

				type	- which measurment used for size
	Purpose:	Setup feather member variables so that it is created properly on rendering
	SeeAlso:	OpFeather::Do
 ********************************************************************************************/
BOOL FeatherAttrValue::SetupFeather(Node* pNode, INT32 Width, UnitType type)
{
	// Get required feather size in Millipoints, taking current zoom into account
	// if width specified in Pixels
	MILLIPOINT BlurSize = ConvertMeasurementToMillipointsAtCurrentViewScale(Width,type);
	if (BlurSize == -1)
		BlurSize = DEFAULT_FEATHERSIZE_MP;
	SetFeatherSize(BlurSize);

	// Save pointer to node so we can get at it in the rendering code.
	SetLinkedNode((NodeRenderableBounded*) pNode);

#ifdef _DEBUG_OFFSCREEN_PIXEL_WIDTH
	RenderDebuggingBitmaps(NULL, pNode, 1.0, 32, FALSE);
#endif

	return TRUE;
}

/********************************************************************************************
>	FeatherAttrValue::
	Author:		Ilan_Copelyn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/01/2000
	Purpose:	
	SeeAlso:	
 ********************************************************************************************/
BOOL FeatherAttrValue::ChangeFeatherProfile(CProfileBiasGain &biasgain)
{
	AFp BiasValue = biasgain.GetBias();
	AFp GainValue = biasgain.GetGain();

	m_Profile.SetBias(BiasValue);
	m_Profile.SetGain(GainValue);

	return TRUE;
}

/********************************************************************************************
>	FeatherAttrValue::
	Author:		Ilan_Copelyn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/01/2000
	Purpose:	
	SeeAlso:	
 ********************************************************************************************/
BOOL FeatherAttrValue::ChangeFeatherSize(INT32 Width, UnitType type)
{
	BOOL res=SetupFeather(GetLinkedNode(), Width, type);

	return res;
}



/********************************************************************************************

>	MILLIPOINT FeatherAttrValue::GetFeatherSize()

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/01/2001

	Returns:	Our feather size, in millipoints.

	Purpose:	Accessor method for our feather size.

	Notes:		This value is always capped off to MaxUserFeatherSize millipoints,
				to prevent anybody ever using a feather size which we cannot handle.

********************************************************************************************/
MILLIPOINT FeatherAttrValue::GetFeatherSize()
{
	return (m_FeatherSize > MaxUserFeatherSize) ? MaxUserFeatherSize : m_FeatherSize;
}



/********************************************************************************************

>	MILLIPOINT FeatherAttrValue::SetFeatherSize(MILLIPOINT mpWidth)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/01/2001

	Inputs:		Our new feather size, in millipoints.

	Returns:	The feather size we actually set, taking cap values into account.

	Purpose:	Mutator method for our feather size.

	Notes:		This method will not set a value <0 mp or >MaxUserFeatherSize mp,
				to prevent us dealing in feather sizes that we cannot handle.

********************************************************************************************/
MILLIPOINT FeatherAttrValue::SetFeatherSize(MILLIPOINT mpWidth)
{
	m_FeatherSize =	mpWidth < 0						? 0 :
					mpWidth > MaxUserFeatherSize	? MaxUserFeatherSize :
					mpWidth;

	return m_FeatherSize;
}



/********************************************************************************************

>	inline double FeatherAttrValue::GetFeatherPixelSize(double fPixelWidth)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/08/2000
	Inputs:		fPixelWidth		width of a pixel in millipoints - accepts non-integer values.

	Returns:	The feather size, in pixels which are fPixelWidth millipoints wide,
				capped off to a maximum of (2 * MAX_SHADOW_BLUR) pixels.

	Purpose:	Return the size of this feather, in pixels of the specified width.
				Different from GetFeatherSize(PIXELS) mainly because:
					1. we return a double, so eg you can get back a value of 2.5 pix.
					2. we're inline.

	Notes:		inline, as it's small and fast.

********************************************************************************************/
inline double FeatherAttrValue::GetFeatherPixelSize(double fPixelWidth)
{
	const double MaxFeatherBlurDiameter = 2.0 * MAX_SHADOW_BLUR;
	double fFeatherSize = (fPixelWidth <= 0) ? 0 : (double)m_FeatherSize / fPixelWidth;
	return (fFeatherSize > MaxFeatherBlurDiameter) ? MaxFeatherBlurDiameter : fFeatherSize;
}



/********************************************************************************************
>	FeatherAttrValue::
	Author:		Ilan_Copelyn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/01/2000
	Purpose:	
	SeeAlso:	
 ********************************************************************************************/
void FeatherAttrValue::SetFeatherProfile(CProfileBiasGain &biasgain)
{
	AFp BiasValue = biasgain.GetBias();
	AFp GainValue = biasgain.GetGain();

	m_Profile.SetBias(BiasValue);
	m_Profile.SetGain(GainValue);
}



/********************************************************************************************

>	virtual void FeatherAttrValue::SetLinkedNode(NodeRenderableBounded* pNode)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/10/2000
	Purpose:	Our outer contour gap size depends on what our linked node is, so we must
				recalculate our gap size whenever our linked node is set.

********************************************************************************************/
void FeatherAttrValue::SetLinkedNode(NodeRenderableBounded* pNode)
{
	m_pLinkedNode = pNode;

	if (pNode != NULL)
	{
		if (IS_A(GetLinkedNode(), NodeGroup) ||
			IS_A(GetLinkedNode(), NodeMould) ||
			IS_A(GetLinkedNode(), NodeBlend) ||
			GetLinkedNode()->IsANodeClipViewController())
		{
			m_GapSize = DEFAULT_GAP_TOLERANCE_MP;
		}
		else
		{
			m_GapSize = 1;
		}
	}
}



/********************************************************************************************
>	FeatherAttrValue::
	Author:		Ilan_Copelyn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/01/2000
	Purpose:	
	SeeAlso:	
 ********************************************************************************************/
BOOL FeatherAttrValue::GenerateOuterContour()
{
	// fail now if we have no linked node.
	if (GetLinkedNode() == NULL)
	{
		ERROR3("FeatherAttrValue::GenerateOuterContour; Aarrgh!! Where's our linked node?!!");
		return FALSE;
	}

	// We know we are about to overwrite this value so if it's still set up
	// we shuold delete it now to prevent memory leaks
	if (m_pOuterContour)
	{
		delete m_pOuterContour;
		m_pOuterContour = NULL;
	}

	// Get outer contour
	// NB m_pOuterContour is a contour which is m_GapSize outside m_pPath's outline
	// Outer contour with m_GapSize is used so that small gaps are forced out. This is
	// especially desireable when there are a large number of tiny gaps within the object
	// we are trying feather (eg the gaps between the 'front windscreen' shape and the 
	// 'windscreen frame')

	m_pOuterContour = new Path;
	BOOL fSuccess =
		NodeContour::GenerateContourPathForNode(m_pOuterContour, GetLinkedNode(), NULL,
												m_GapSize, TRUE, RoundJoin, -1, TRUE, TRUE);

	if (!fSuccess)
	{
		delete m_pOuterContour;
		m_pOuterContour = NULL;
	}

	return fSuccess;
}

/********************************************************************************************
>	FeatherAttrValue::
	Author:		Ilan_Copelyn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/5/2000
 ********************************************************************************************/
BOOL FeatherAttrValue::CanBeRenderedDirectly()
{
	if(!GetLinkedNode())
		return FALSE;
	if(GetLinkedNode()->IS_KIND_OF(NodeDocument))
		return FALSE;
	if(GetFeatherSize()==0)
		return FALSE;

	return TRUE;
}



/********************************************************************************************

>	BOOL FeatherAttrValue::RegenerateOuterContourOnNextRedraw(UndoableOperation* pOp = NULL)

	Author:		Ilan_Copelyn (Xara Group Ltd) <camelotdev@xara.com>;		Karim MacDonald
	Created:	24/01/2000;	01/03/2001
	Inputs:		pOp		optional ptr to an UndoOp, letting us store undo information.

	Returns:	TRUE if we were successful,
				FALSE otherwise.

	Purpose:	Clear the cache of our outer-contour path.

	See also:	AttrFeather::Transform(), AttrFeather::LinkedNodeGeometryHasChanged().

********************************************************************************************/
BOOL FeatherAttrValue::RegenerateOuterContourOnNextRedraw(UndoableOperation* pOp)
{
	if (m_pOuterContour != NULL)
	{
		if (pOp != NULL)
		{
PORTNOTE("other","Removed RegenerateFeatherContourAction usage")
#ifndef EXCLUDE_FROM_XARALX
			// make this undoable
			RegenerateFeatherContourAction* pAct = NULL;
			if (RegenerateFeatherContourAction::Init(pOp, pOp->GetUndoActionList(), this, m_pOuterContour, &pAct) == AC_FAIL)
			{
				TRACEALL( _T("FeatherAttrValue::RegenerateOuterContourOnNextRedraw; Could not record undo information.\n"));
				return FALSE;
			}
#endif
		}
		// don't actually delete the old outer contour
		// if it's been saved via the undoable operation.
		if (pOp == NULL)
			delete m_pOuterContour;

		m_pOuterContour = NULL;
	}

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////////////////////
//
//								NodeAttribute classes
//
/////////////////////////////////////////////////////////////////////////////////////////////

/********************************************************************************************

>	void AttrFeather::Render( RenderRegion* pRender )

	Author:		Ilan_Copelyn (Xara Group Ltd) <camelotdev@xara.com>		, Karim MacDonald
	Created:	24/01/2000	, 21/07/2000

	Inputs:		pRender		the render-region to render into.

	Purpose:	Render this feather attribute.
				We do one or two checks for validity of this attr, and then render its
				attribute-value into the RR.

	Notes:		If you want to customise for different RRs, then override
				RenderRegion::SetOffscreen and FeatherAttrValue::OffscreenRenderingCompleted.

	Errors:		ERROR3 if we have no linked node at the mo'.

	See also:	FeatherAttrValue::Render().

********************************************************************************************/
void AttrFeather::Render( RenderRegion* pRender )
{
	// no feathering for hit-testing, please.
	if (pRender->IsHitDetect())
		return;

	// Karim 21/07/2000
	// Feathers can render into most render regions.
	// If a render region can't deal with feathering, then the attribute will only be
	// 'rendered' in the RenderRegion sense - ie plonked on the context stack - and
	// no attempt will be made to generate offscreen bitmaps and what-not.

	// only bother rendering us if our feather size is non-zero.
	if (Value.GetFeatherSize() != 0)
	{
		// ensure internal pointer is setup for rendering.
		if (Value.GetLinkedNode() == NULL)
		{
			Node* pParent = FindParent();
			if (pParent != NULL && pParent->IsAnObject())
				Value.SetLinkedNode((NodeRenderableBounded*) FindParent());

			else
			{
				if (!Error::IsUserName("Gavin"))
					ERROR3("AttrFeather::Render; No linked node and parent is not an ink-node!");
				return;
			}
		}
		Value.Render(pRender);
	}
}



/********************************************************************************************
>	void* AttrFeather::GetDebugDetails(StringBase* Str) 
	Author:		Ilan_Copelyn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	
	Outputs:	Str: String giving debug info about the node
	Purpose:	For obtaining debug information about the Node
 ********************************************************************************************/
#ifdef _DEBUG
void AttrFeather::GetDebugDetails(StringBase* Str) 
{
	// Output base class debug info.
	NodeRenderable::GetDebugDetails(Str);

	// Output our feather size, in millipoints.
	MILLIPOINT mp = Value.GetFeatherSize();

	String_256 TempStr; 
	TempStr._MakeMsg(TEXT("\r\nFeather size (millipoints)    #1%ld"), mp);
	(*Str) += TempStr;
}
#endif

/********************************************************************************************
>	AttrFeather::
	Author:		Ilan_Copelyn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/01/2000
	Purpose:	
	SeeAlso:	Node::CopyChildren(OpClone); attrmgr::applycurrentattribstonode (opnewregshape)
	TODO:		Code which uses this to localise attributes must be changed. Inconsistencies
				arise if bitmap preserved and subsequently generated from ex-member node only,
				and time delays occur if feather bmp regenerated for each member node on
				localisation.
 ********************************************************************************************/
Node* AttrFeather::SimpleCopy()
{
	AttrFeather* pAttr =  new AttrFeather();
	ERRORIF(pAttr==NULL, _R(IDE_NOMORE_MEMORY),NULL);

	CopyNodeContents(pAttr);
	return pAttr;
}

/********************************************************************************************
>	AttrFeather::
	Author:		Ilan_Copelyn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/01/2000
	Purpose:	
 ********************************************************************************************/
void AttrFeather::CopyNodeContents(NodeAttribute *pCopy)
{
	NodeAttribute::CopyNodeContents(pCopy);
	((AttrFeather*)pCopy)->Value.SimpleCopy( &Value  );
}

/***********************************************************************************************
>   void AttrFeather::PolyCopyNodeContents(NodeRenderable* pNodeCopy)

    Author:     Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
    Created:    18/12/2003
    Outputs:    -
    Purpose:    Polymorphically copies the contents of this node to another
	Errors:     An assertion failure will occur if NodeCopy is NULL
    Scope:      protected
								     
***********************************************************************************************/

void AttrFeather::PolyCopyNodeContents(NodeRenderable* pNodeCopy)
{
	ENSURE(pNodeCopy, "Trying to copy a node's contents into a NULL node");
	ENSURE(IS_A(pNodeCopy, AttrFeather), "PolyCopyNodeContents given wrong dest node type");

	if (IS_A(pNodeCopy, AttrFeather))
		CopyNodeContents((AttrFeather*)pNodeCopy);
}



/********************************************************************************************
>	AttrFeather::
	Author:		Ilan_Copelyn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/01/2000
	Purpose:	Closely tied in with the attr optimisation code. The ShouldBeOptimized()
				function will only allow a AttrFeather (ie a NodeAttribute) to be factored
				out if the contained value's feather size is 0. This will happen when new
				nodes are created using a current feather attribute with 0 feather size.
				When this occurs we want to let the optimisation code (NormaliseAttributes())
				remove the AttrFeather from the node on insertion into the tree.
				NB the check in the optim code is
					"if (AttrAleadyAppliedHigherUpTheTree == attrToOptimize)"
				so we want to return TRUE if attrToOptimize->Value.feathersize is 0 so that it
				gets deleted.
				NB As long as the optimisation method is always used to insert nodes into the tree,
				and also the order of comparison (ie a==b but not b==a) is preserved, we can always
				expect  0 width feathers to be optimised out.
	TODOCHECK:	Assuming here that == is not required anywhere other than in the attribute
				optimisation code. May not be correct behaviour if comparison required in other
				circumstance.
 ********************************************************************************************/
INT32 AttrFeather::operator==(const NodeAttribute& NodeAttrib)
{
	ERROR3IF(!NodeAttrib.IsAFeatherAttr(), "AttrFeather::operator==; Compared with a non-feather attr.");
	AttrFeather *PotentialAttrToOptimize = (AttrFeather*) &NodeAttrib;

	return (Value.GetFeatherSize() == PotentialAttrToOptimize->Value.GetFeatherSize());
}

/********************************************************************************************
>	AttrFeather::
	Author:		Ilan_Copelyn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/01/2000
	Purpose:	
	SeeAlso:	
 ********************************************************************************************/
UINT32 AttrFeather::GetAttrNameID()
{
	return (_R(IDS_FEATHER_ATTR_ID)); 
}                          

/********************************************************************************************
>	AttrFeather::
	Author:		Ilan_Copelyn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/01/2000
	Purpose:	
	SeeAlso:	
 ********************************************************************************************/
UINT32 AttrFeather::GetNodeSize()
{
	return sizeof(AttrFeather);
}

/********************************************************************************************
>	AttrFeather::
	Author:		Ilan_Copelyn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/01/2000
	Purpose:	
	SeeAlso:	
 ********************************************************************************************/
BOOL AttrFeather::IsAFeatherAttr() const
{
	return TRUE;
}

/********************************************************************************************
>	BOOL AttrFeather::DoFeather(Node* pNode, INT32 size = DEFAULT_FEATHERSIZE_MP, UnitType type = MILLIPOINTS)
	Author:		Ilan_Copelyn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/01/2000
	Purpose:	Setup new FeatherAttrValue to apply feathering or change existing feather size
	SeeAlso:	OpFeather::Do(),OpChangeFeather::DoWithParam(...)
 ********************************************************************************************/
BOOL AttrFeather::DoFeather(Node* pNode, INT32 size, UnitType type)
{
	return Value.SetupFeather(pNode, size, type);
}

/********************************************************************************************
>	AttrFeather::
	Author:		Ilan_Copelyn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/01/2000
	Purpose:	
	SeeAlso:	
 ********************************************************************************************/
BOOL AttrFeather::DoSetProfile(CProfileBiasGain bg)
{
	return Value.ChangeFeatherProfile(bg);
}

/********************************************************************************************

						Layering and redundant feather removal

	Purpose:	Feathers are layered attributes. Feathering individual members in a 
				group will look visibly different to feathering the entire group (if
				any of the children overlap)
				Hence once a feather attribute has been inserted in to the tree it must
				not be moved around by the optimisation code.
				However, we don't want zero size feather attributes to be stuck into the
				tree ever. This function determines when an attribute can be optimised
				so determined whether we can get rid of rdundant attributes.
				Hence we allow optimisation to occur in the case where we have a zero width
				feather.

 ********************************************************************************************/
	
/********************************************************************************************
>	AttrFeather::
	Author:		Ilan_Copelyn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/01/2000
	Purpose:	Ensure that this node is not optimised (ie factored out when groups created
				or new / cloned nodes inserted into the tree).

				Called when :-
				1) a new node is inserted into the tree
				2) a clone is inserted into the tree
				3) a node with feathering applied is being grouped with other nodes
				4) when a group with feathering applied is being ungrouped
				
				Feathers should never be optimised. This means that they will always be
				inserted into the tree with each new node created, and not factored out.
				The default feather after camelot initialises itself (ie on startup) has zero
				feather width. To prevent the tree becomming filled with lots of superfluous
				0 width feather attrs we have an extra check in AttributeManager::ApplyCurrentAttribsToNode.

	SeeAlso:	AttributeManager::ApplyCurrentAttribsToNode
 ********************************************************************************************/
BOOL AttrFeather::ShouldBeOptimized()
{
	// don't want zero width feathers in the tree
	if (Value.GetFeatherSize() == 0)
		return TRUE;

	return FALSE;
}

/********************************************************************************************

							Feather prevention ;)

	Purpose:	Feathering is not supported in certain circumstances listed below
				Rather than hacking other code to prevent these situations arising
				I am using this method to prevent feathers causing damage to other
				nodes.

				This way feathering can be added simply once the individual
				incompatibilities have been resolved

	Notes:		Attributes aren't always rendered by the standard render loop
				(eg DocView::RenderView). Hence this function doesn't catch all cases
				where feathers will get rendered, and this test needs to be repeated
				in the corresdponding AttrValue::Render()
				See RenderRegion::InitDevice() and CCAttrMap::Render() for egs of 
				where these functions will be skipped.

	More Notes:	Karim 21/07/2000
				I have now annihilated all of the render-region checks - rather than
				do checks on the type of render-region, we'll just let the RR's
				feather-rendering methods do the job for us.
				This makes it a *lot* easier to customise for different RR's ;o)

 ********************************************************************************************/
SubtreeRenderState AttrFeather::RenderSubtree(RenderRegion *pRender, Node** ppNextNode, BOOL bClip)
{
	// Can't feather unless attribute is attached to an object (ie something
	// with an inkpath)
	if (Value.GetLinkedNode() == NULL && FindParent() == NULL)
		return SUBTREE_NORENDER;

	// don't bother rendering if feather size is zero or we're the default attr.
	if (Value.GetFeatherSize() == 0 || Value.IsDefaultFlagSet())
		return SUBTREE_NORENDER;

	return SUBTREE_ROOTONLY;
}


/********************************************************************************************

							Dynamic Attribute creation

	Purpose:	Support dynamic creation of feather attibutes by other complex nodes
				(just Blends and Contours so far)
				
				Dynamic attrs are created by CCAttrMap::MakeAppliedAttrMap
				which calls NodeRenderableInk::FindAppliedAttributes
				
				However the problem is that some attributes require additional info
				inorder to apply themselves to a given path or node. These attributes
				are specific to the node that they are attached to, so need an additonal
				step to set themselves up after being dynamically created.
				
				Feather attributes need their internal node pointer to be setup. This
				pointer is required in order to work out the dimensions of the offscreen
				bitmap, to place this bitmap onscreen once it has been created, and to
				get the inkpath so that the convolved transparency mask can be created

 ********************************************************************************************/
BOOL AttrFeather::PostDynCreateInit(CCAttrMap* pBlendMap, Path* pInkPath, CCRuntimeClass* CreatorClass)
{
	// all defaults will be out by this stage
	// blending default to default will return false - so no feather should be in blend attr map
	// blending from default to non-def will dyn create a new feather and blend it's state members
	//		its m_bDefault will be FALSE as it's set in the constructor
	ENSURE(!Value.IsDefaultFlagSet(),"How did this attribute get copied from the default attr?"); 

	if (Value.GetFeatherSize() == 0)
		return FALSE;			// Won't render and doesn't alloc mem

	// Karim 17/08/2000
	// Feather attrs usually decide for themselves whether to use a transparency-capable
	// bitmap when rendering. However, within contours and blends, we must force them to,
	// as they are now detached from the tree, and therefore cannot examine the surroundings
	// to make a transparency decision.
	Value.SetAlwaysUseTransparency();

	// fix internal node pointer so that we can setup offscreen bitmap
	// and create outer contour for generating feather mask

	// if we fail (eg out of mem) then put this feather attr into a state so that it won't NeedsToRender()

	NodePath* pNodePath = new NodePath();
	if(!pNodePath)
	{
		return FALSE;	// nb don't report errors. simply render without feathering if this fails
	}

	pNodePath->SetUpPath(pInkPath->GetNumCoords(),12);
	pNodePath->InkPath.CopyPathDataFrom(pInkPath);

	if(!MakeLineAttributeCompleteForContouring(pNodePath, this, pBlendMap))
	{
		// Feather is unrenderable
		// Need to de-alloc mem because we will be returning FALSE and
		// hence won't be added to the attrmap - therefore no PostDynCreateDeinit
		// Set Child pointer to null in case we had partial sucess
		// *** Change here if copy attrs from blendmap inside MakeLineAttributeCompleteForContouring
		pNodePath->SetChildDangerous(NULL);
		delete pNodePath;
		return FALSE;
	}

	Value.SetLinkedNode(pNodePath);
	return TRUE;
}



/********************************************************************************************

>	void AttrFeather::PostDynCreateDeInit()

	Author:		Ilan_Copelyn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	Mid-2000

	Purpose:	Free memory allocated by PostDynCreateInit(..) functions
	Errors:		No memory needs to be allocated, so cannot fail.

********************************************************************************************/
void AttrFeather::PostDynCreateDeInit()
{
	if (Value.GetFeatherSize() == 0)
	{
		ERROR3("Failure to PostDynCreateInit should have removed this attr from the attrmap");
		return;		// nothing to de-alloc
	}


	// Karim 17/08/2000
	// This call is for completeness - probably isn't 100% necessary, but
	// better safe than sorry. See PostDynCreateInit() above for more info.
	Value.UnsetAlwaysUseTransparency();

	// NB this must only be called if you have called PostDynCreateInit(..)

	// dealloc the memory that we allocated in order to perform dynamic feathering
	// check parent node
	Node* pParent = FindParent();
	ENSURE(pParent->FindParent()==NULL && pParent->FindPrevious()==NULL && pParent->FindNext()==NULL,"Parent not created dynamically");

	// delete line attrs which we added
	// *** Change here if copy attrs from blendmap inside MakeLineAttributeCompleteForContouring
/*	Node* pPrev = FindPrevious();
	if(pPrev)
	{
		ENSURE(pPrev->FindParent()==pParent,"Deleting dynamic child attrs which we didn't alloc.");
		pPrev->SetNextDangerous(NULL);
		pParent->CascadeDelete();
	}
*/	
	pParent->SetChildDangerous(NULL);
	delete pParent;
	return;
}



/********************************************************************************************

>	BOOL AttrFeather::IsLinkedToThisNode(Node* pNode)

	Author:		Ilan_Copelyn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	Mid-2000
	Inputs:		pNode	ptr to the node to check for linkage.

	Returns:	TRUE if we're linked to pNode,
				FALSE otherwise.

	See Also:	LinkToGeometry().

********************************************************************************************/
BOOL AttrFeather::IsLinkedToThisNode(Node* pNode)
{
	return (Value.GetLinkedNode() == pNode);
}



/********************************************************************************************

>	virtual void AttrFeather::Transform(TransformBase& Trans)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/10/2000
	Inputs:		Trans	the applied transformation.
	Outputs:	The feather size of our FeatherAttrValue will scale.
	Purpose:	Scales the size of the feather as the feathered object is transformed.

********************************************************************************************/
void AttrFeather::Transform(TransformBase& Trans)
{
	static const FIXED16 One(1.0);

	FIXED16 ScaleFactor = Trans.GetScalar();
	if (ScaleFactor != One)
	{
		// Only transform feather width of the transform allows widths to be affected
// NEW FEATURE?
//		if (Trans.TransLines)
//		{
			MILLIPOINT FeatherSize = Value.GetFeatherSize();
			MILLIPOINT NewSize = INT32(0.5 + fabs(ScaleFactor.MakeDouble() * (double)FeatherSize) );
			FeatherSize = (NewSize == 0) ? 1 : NewSize;

			Value.SetFeatherSize(FeatherSize);
//		}
	}

	// Always transform the outer path so that the shape appears correctly during solid drags
	Path* pOuterContour = Value.GetOuterContour();
	if (pOuterContour != NULL)
	{
		Trans.Transform(pOuterContour->GetCoordArray(), pOuterContour->GetNumCoords());
	}
}



/********************************************************************************************

>	BOOL AttrFeather::Blend(BlendAttrParam* pBlendParam)

	Author:		Ilan_Copelyn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	Mid-2000

	Inputs:		pBlendParam		contains the blending info.
	Returns:	TRUE if success,
				FALSE otherwise.

	Purpose:	Blend us.
	
	Notes:		Always returns FALSE if blending from default to default.
				NB	in this case will never add an attr of this type to the blended attr map
					hence PostDynCreateInit will not be called ('cause it doesn't get dyn created!)

	Errors:		ERROR3 if NULL input param; ENSURE if pBlendParam has no blend-attr.

********************************************************************************************/
BOOL AttrFeather::Blend(BlendAttrParam* pBlendParam)
{
	// Check entry param isn't NULL
	ERROR3IF(pBlendParam == NULL, "NULL entry param");
	if (pBlendParam == NULL)
		return FALSE;

	BOOL Success = FALSE;

	// We have a valid feather to blend, so do it (in the convoluted style copied from other attrs...)
	// Get the Value member to blend to the Value member of the other NodeAttribute.
	// If it succeeds, ask the blended Value to make a NodeAttribute out of itself.
	if (Value.Blend(pBlendParam))
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
				delete pBlendedAttrVal;
				Success = TRUE;
			}
			else
			{
				// Couldn't make a blended NodeAttribute, so give the blended attr val back
				// and return FALSE
				pBlendParam->SetBlendedAttrVal(pBlendedAttrVal);
				Success = FALSE;
			}
		}
		else
		{
			ENSURE(FALSE,"Couldn't get blended attr val from BlendAttrParam.");
			Success = FALSE;
		}
	}
	
	return Success;
}



/********************************************************************************************
>	virtual BOOL AttrFeather::PostDynCreateInit(	CCAttrMap* pMap,
											   Node* pNode,
											   CCRuntimeClass* pCreatorClass)
	Author:		Ilan_Copelyn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/01/2000
	Purpose:	
	SeeAlso:	
 ********************************************************************************************/
BOOL AttrFeather::PostDynCreateInit(CCAttrMap* pMap,
									   Node* pNode,
									   CCRuntimeClass* pCreatorClass)
{
	return TRUE;
}



/********************************************************************************************

>	BOOL AttrFeather::LinkedNodeGeometryHasChanged(UndoableOperation* pOp)

	Author:		Ilan_Copelyn (Xara Group Ltd) <camelotdev@xara.com>;		Karim MacDonald
	Created:	27/4/00;	01/03/2001

	Inputs:		pOp		the operation causing our linked node's geometry to change.
	Outputs:	Our cache of our outer-contour path may be cleared.
	Returns:	TRUE if successful,
				FALSE otherwise.

	Purpose:	Inform this attribute that the geometry of its linked node may be changing.

	Errors:		ERROR2 if pOp is NULL - should *never* happen.

	See also:	Node::AllowOp() + overrides - main place from which this fn is called.

********************************************************************************************/
BOOL AttrFeather::LinkedNodeGeometryHasChanged(UndoableOperation* pOp)
{
// DEBUG:
//	static UINT32 ctr = 0;
//	TRACEUSER( "Karim", _T("%d. LinkedNodeGeometryHasChanged() for 0x%x.\n"), ++ctr, &Value);

	ERROR2IF(pOp == NULL, FALSE,
			"AttrFeather::LinkedNodeGeometryHasChanged; Should never call with NULL pOp!");

	// Karim 28/02/2000
	// If we're undergoing translation, rotation or reflection, then our transform
	// method will automatically update our outer-contour.
	// In all other cases, we should clear out our outer-contour,
	// so it is refreshed at the next render.
	//
	// The only way to detect whether we're being transformed as above, is to test on the Op.

PORTNOTE("other","Removed NameGallery usage")
	if (pOp->IS_KIND_OF(TransOperation) 
		&& !pOp->IsKindOf( CC_RUNTIME_CLASS(OpPathNudge) )
		)
	{
		// arbitrarily chosen error threshold.
		static const double epsilon = 0.000016;
		Matrix Mat = ((TransOperation*)pOp)->GetCurrentMatrix();
		if (Mat.IsTranslation() ||
			Mat.IsReflection() ||
			Mat.IsRotation(epsilon))
		{
			// can only know for sure that we were transformed,
			// if our linked node, or one of its parents, is the op's target.
			Node* pLinkedNode	= Value.GetLinkedNode();
//			Node* pTransNode	= ((TransOperation*)pOp)->GetNode();
//			if( pLinkedNode == pTransNode ||
//				(pTransNode != NULL && pTransNode->IsNodeInSubtree(pLinkedNode)) )
//				return TRUE;
			Range* pRange = ((TransOperation*)pOp)->GetTransformRange();
			if (pRange && pRange->Contains(pLinkedNode, TRUE))
				return TRUE;
		}
	}

	return Value.RegenerateOuterContourOnNextRedraw(pOp);
}

/********************************************************************************************
>	virtual void AttrFeather::NewlyCreatedDefaultAttr(NodeDocument* pNode)
	Author:		Ilan_Copelyn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/4/00
	Inputs:		
	Outputs:	
	Returns:	
	Purpose:	Attributes which are linked to the geometry of the objects which they affect,
				may not be able to perform a useful function if attached directly to the 
				NodeDocument node (ie a default attribute). This function allows a default
				attribute of this type to perform an extra initialisation step so that the
				Render() function can handle 
	Errors:		
	SeeAlso:	-
 ********************************************************************************************/
void AttrFeather::NewlyCreatedDefaultAttr(NodeDocument* pNode)
{
	Value.SetLinkedNode((NodeRenderableBounded*)pNode);
	Value.SetDefault();
}

/********************************************************************************************
>	AttrFeather::
	Author:		Ilan_Copelyn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/5/2000
	Purpose:	connect attribute values internal node pointer
	SeeAlso:	
 ********************************************************************************************/
BOOL AttrFeather::LinkToGeometry(Node* pContext)
{
	Value.SetLinkedNode((NodeRenderableBounded*) pContext);
	return TRUE;
}

/********************************************************************************************
>	virtual BOOL AttrFeather::ContainsAttributeValue(AttributeValue* pVal)
	Author:		Ilan_Copelyn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/6/00
	Inputs:		
	Outputs:	
	Returns:	
	Purpose:	To check if this NodeAttribute contains a the AttributeValue
				NB needs to be implemented in a manner specific to the way in which
				the derived NodeAttribute class stores the AttributeValue
	Errors:		
	SeeAlso:	OffscreenAttrValue::DoesOffscreenBmpRequireTransp
				AttrFeather::ContainsAttributeValue
 ********************************************************************************************/
BOOL AttrFeather::ContainsAttributeValue(AttributeValue* pVal)
{
	if(!pVal->IS_KIND_OF(FeatherAttrValue))
	{
		ENSURE(FALSE,"Strange attr value comparison test requested");
		return FALSE;
	}

	return (pVal == &Value);
}

/********************************************************************************************
>	AttrFeather::
	Author:		Ilan_Copelyn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/01/2000
	Purpose:	
	SeeAlso:	
 ********************************************************************************************/

/////////////////////////////////////////////////////////////////////////////////////////////
//
//								Saving and loading
//
/////////////////////////////////////////////////////////////////////////////////////////////
BOOL AttrFeather::WritePreChildrenWeb(BaseCamelotFilter* pFilter)
{
	ERROR2IF(pFilter == NULL,FALSE,"NULL filter param");
	CXaraFileRecord Rec(TAG_FEATHER,TAG_FEATHER_SIZE);
	BOOL ok = TRUE;
	
	MILLIPOINT sz = Value.GetFeatherSize();
	CProfileBiasGain pProfile(Value.GetProfile());

	ok = Rec.Init();
	if (ok) ok = Rec.WriteINT32(sz);
	if (ok) ok = Rec.WriteDOUBLE ((double) pProfile.GetBias());
	if (ok) ok = Rec.WriteDOUBLE ((double) pProfile.GetGain());
	if (ok) ok = pFilter->Write(&Rec);

	return ok;
}

BOOL AttrFeather::WritePreChildrenNative(BaseCamelotFilter* pFilter)
{
	return WritePreChildrenWeb(pFilter);
}

UINT32* FeatherRecordHandler::GetTagList()
{
	static UINT32 TagList[] = {TAG_FEATHER,CXFRH_TAG_LIST_END};
	return (UINT32*)&TagList;
}

BOOL FeatherRecordHandler::HandleRecord(CXaraFileRecord* pCXaraFileRecord)
{
	ERROR2IF(pCXaraFileRecord == NULL,FALSE,"pCXaraFileRecord is NULL");
	BOOL ok = TRUE;

	switch (pCXaraFileRecord->GetTag())
	{
		case TAG_FEATHER:
			ok = HandleFeatherRecord(pCXaraFileRecord);
			break;

		default:
			ok = FALSE;
			ERROR3_PF(("Incorrect tag supplied to HandleRecord. Tag = (%d)\n",pCXaraFileRecord->GetTag()));
			break;
	}

	return ok;
}

BOOL FeatherRecordHandler::HandleFeatherRecord(CXaraFileRecord* pCXaraFileRecord)
{
	BOOL ok = TRUE;

	MILLIPOINT FeatherSz;
	double Bias = 0, Gain = 0;
	double* ptrBias = &Bias, *ptrGain = &Gain;

	// Read in the feather data
	if (ok) ok = pCXaraFileRecord->ReadINT32(&FeatherSz);
	if (ok) ok = pCXaraFileRecord->ReadDOUBLEnoError (ptrBias);
	if (ok) ok = pCXaraFileRecord->ReadDOUBLEnoError (ptrGain);

	CProfileBiasGain Profile;

	if ((ptrBias != NULL) && (ptrGain != NULL))
	{
		Profile.SetBias((AFp) Bias);
		Profile.SetGain((AFp) Gain);
	}

	if (ok)
	{
		AttrFeather* pAttr = new AttrFeather;
		if (pAttr != NULL)
		{
			// Get a ptr to the attr value object
			FeatherAttrValue* pValue = (FeatherAttrValue*)&pAttr->Value;

			if (pValue != NULL)
			{
				// Set the feather size
				pValue->SetFeatherSize(FeatherSz);

				// Set the profile
				pValue->SetFeatherProfile(Profile);
				
				if (ok) ok = InsertNode(pAttr);
				// Set the m_Node pointer (TODO remove)
				if (ok && pBaseCamelotFilter->GetInsertMode()==INSERTMODE_ATTACHTOTREE)
					pValue->SetLinkedNode((NodeRenderableBounded*)pAttr->FindParent());
			}
			else
				ok = FALSE;
		}
		else
			ok = FALSE;
	}

	return ok;
}
