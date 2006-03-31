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
#include "app.h"
#include "node.h"
#include "nodeliveeffect.h"
//#include "xpehost.h"
#include "dibutil.h"
#include "cxftags.h"
#include "cxfrec.h"
#include "blobs.h"
#include "bitmap.h"
#include "bmpcomp.h"	
#include "oilbitmap.h"
#include "bshadow.h"
#include "fillval.h"
#include "becomea.h"
#include "nodepath.h"
#include "lineattr.h"
#include "blndtool.h"
#include "nodebmp.h"

// Resource headers
//#include "resource.h"
//#include "mario.h"
//#include "phil.h"
//#include "tim.h"
#include "transop.h"
//#include "opfeathr.h"

//#include "pmaskrgn.h"
#include "scanrr.h"

//#include "pmaskrgn.h"
#include "scanrr.h"
#include "ophist.h"

DECLARE_SOURCE( "$Revision$" );

CC_IMPLEMENT_DYNCREATE(NodeBitmapEffect, NodeEffect)
CC_IMPLEMENT_DYNCREATE(NodeLiveEffect, NodeBitmapEffect)
CC_IMPLEMENT_DYNCREATE(NodeLockedEffect, NodeBitmapEffect)
#ifdef FEATHER_EFFECT
CC_IMPLEMENT_DYNCREATE(NodeFeatherEffect, NodeLiveEffect)
#endif
CC_IMPLEMENT_DYNAMIC(LiveEffectRecordHandler, CamelotRecordHandler)

// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW


#ifdef DEBUG
#define DEBUGATTACH(STATE, LABEL) {STATE.AttachToDoc(LABEL);}
#else
#define DEBUGATTACH(STATE, LABEL) {}
#endif

// Define this if you think captured shapes need a 1 pixel border to allow for anti-aliasing
#define CAPTURE_BORDER 1

// Define this if you want to test the Atomic skipping of live, locked and feather tags
// during XAR file loading
#ifdef DEBUG
//	#define TEST_NO_EFFECT_LOADING
#endif



/********************************************************************************************

	Preference:	DefaultLivePixelsPerInch
	Section:	Effects
	Range:		0-72000
	Purpose:	

********************************************************************************************/

UINT32 NodeBitmapEffect::DefaultLivePixelsPerInch = 96;		// default is 96 dpi




/********************************************************************************************

	Preference:	DefaultLockedPixelsPerInch
	Section:	Effects
	Range:		1-72000
	Purpose:	

********************************************************************************************/

UINT32 NodeBitmapEffect::DefaultLockedPixelsPerInch = 150;		// default is 150 dpi




/********************************************************************************************

	Preference:	GroupPixelsPerInch
	Section:	Effects
	Range:		0-72000
	Purpose:	

********************************************************************************************/

UINT32 NodeBitmapEffect::GroupPixelsPerInch = 0;				// default is "Automatic" dpi




/********************************************************************************************

	Preference:	ShadowPixelsPerInch
	Section:	Effects
	Range:		0-72000
	Purpose:	

********************************************************************************************/

UINT32 NodeBitmapEffect::ShadowPixelsPerInch = 0;				// default is "Automatic" dpi




/********************************************************************************************

	Preference:	DefaultLocked
	Section:	Effects
	Range:		FALSE-TRUE
	Purpose:	

********************************************************************************************/

BOOL NodeBitmapEffect::DefaultLocked = FALSE;				// Default is to create live effects




/********************************************************************************************

>	static BOOL NodeBitmapEffect::Init()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/09/2004
	Inputs:		-
	Purpose:	Initialise the NodeBitmapEffect class

********************************************************************************************/

BOOL NodeBitmapEffect::Init()
{
	if (Camelot.DeclareSection( _T("Effects"), 10))
	{
		Camelot.DeclarePref(NULL, _T("DefaultLivePixelsPerInch"), (INT32*)&DefaultLivePixelsPerInch, 0, 72000);
		Camelot.DeclarePref(NULL, _T("DefaultLockedPixelsPerInch"), (INT32*)&DefaultLockedPixelsPerInch, 1, 72000);
		Camelot.DeclarePref(NULL, _T("DefaultLocked"), (INT32*)&DefaultLocked, FALSE, TRUE);
		Camelot.DeclarePref(NULL, _T("GroupPixelsPerInch"), (INT32*)&GroupPixelsPerInch, 0, 72000);
		Camelot.DeclarePref(NULL, _T("ShadowPixelsPerInch"), (INT32*)&ShadowPixelsPerInch, 0, 72000);
	}

	return TRUE;
}




/***********************************************************************************************

>	NodeBitmapEffect::NodeBitmapEffect(Node* 	ContextNode,
						AttachNodeDirection Direction,
						const DocRect& 		BoundingRect,
						BOOL				Locked = FALSE,
						BOOL 				Mangled = FALSE,
						BOOL 				Marked = FALSE,
						BOOL 				Selected = FALSE,
						BOOL 				Renderable = FALSE
						)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/09/2004
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
NodeBitmapEffect::NodeBitmapEffect(Node* ContextNode,  
					AttachNodeDirection Direction,  
					BOOL Locked, 
					BOOL Mangled,  
					BOOL Marked, 
					BOOL Selected    
			  ) : NodeEffect(ContextNode, Direction, Locked, Mangled, Marked, Selected )
{                         
	m_pEditsDoc = NULL;
	m_bHasChangedRecently = FALSE;
	m_strPostProID = String(_T(""));
	m_strDisplayName = String(_T(""));
	m_rectDirectBitmap = DocRect(0,0,0,0);
}




/*********************************************************************************************

>	NodeBitmapEffect::NodeBitmapEffect() 

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/09/2004
	Purpose:	
	Note:		
	SeeAlso:	

**********************************************************************************************/
NodeBitmapEffect::NodeBitmapEffect() : NodeEffect()
{
	m_pEditsDoc = NULL;
	m_bHasChangedRecently = FALSE;
	m_strPostProID = String( _T("") );
	m_strDisplayName = String( _T("") );
}




/*********************************************************************************************

>	NodeBitmapEffect::~NodeBitmapEffect() 

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/09/2004
	Purpose:	Destructor
	Note:		
	SeeAlso:	

**********************************************************************************************/
NodeBitmapEffect::~NodeBitmapEffect()
{
	m_pEditsDoc = NULL;				// m_pEditsDoc is a smart pointer so NULLing it Releases the COM interface
									// and releases any memory
	// Note that NodeBitmapEffect stores original bitmap data in the cache
	// but this destructor relies no derived classes clareing cached data when they
	// are destroyed for efficiency.
}




/********************************************************************************************

>	virtual double NodeBitmapEffect::GetPixelsPerInch()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/09/2004
	Inputs:		- 
	Outputs:	Pixel resolution for this live effect bitmap in pixels per inch (double)
	Returns:		Description of the group node 
	Purpose:	To get the required/stored pixel res of this node
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

double NodeBitmapEffect::GetPixelsPerInch()
{
	double dPPI = 0.0;

	if (m_dPixelsPerInch==0)
	{
		// 0 means automatic
		// Automatic means get resolution in context
		// Context means either:
		//	* Respond to a direct bitmap below us
		//	* or respond to a fixed resolution above us in the tree
		//	* or get resolution from current view
		BOOL bDirectResolution = GetChildDirectBitmap(NULL, NULL, NULL, NULL, NULL, &dPPI);
		if (bDirectResolution && dPPI!=0)
			return dPPI;

		Node* pParent = this->FindParent();
//		while (pParent && (pParent->IsBitmapEffect() || pParent->IsNodeHidden()))
		while (pParent && !pParent->IsLayer())
		{
			if (pParent->IsBitmapEffect())
			{
				dPPI = ((NodeBitmapEffect*)pParent)->GetPixelsPerInchValue();
				if (dPPI!=0)
					break;
			}

			pParent = pParent->FindParent();
		}

		if (dPPI==0)
		{
			// See also OpEffectLock::DoEffectNodeOp
			View* pView = View::GetCurrent();
			if (pView) dPPI = 72000.0 / pView->GetPixelWidth().MakeDouble();
		}
	}

	if (dPPI==0)
		dPPI = m_dPixelsPerInch;

	if (dPPI==0)
	{
		// Shouldn't ever reach this clause but just in case...
		ERROR3("GetPixelsPerInch can't get sensible PPI so defaulting to 96");
		dPPI = 96.0;
	}

	return dPPI;
};




/********************************************************************************************

>	virtual void NodeBitmapEffect::SetPixelsPerInch(double dPixelsPerInch)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/09/2004
	Inputs:		- 
	Outputs:	Pixel resolution for this live effect bitmap in pixels per inch (double)
	Retuns:		Description of the group node 
	Purpose:	To get the required/stored pixel res of this node
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void NodeBitmapEffect::SetPixelsPerInch(double dPixelsPerInch)
{
	m_dPixelsPerInch = dPixelsPerInch;
};

/********************************************************************************************

>	virtual double NodeBitmapEffect::GetPixelWidth()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/09/2004
	Inputs:		- 
	Outputs:	Pixel width for live effect bitmap in millipoints (double)
	Retuns:		Description of the group node 
	Purpose:	To get the required/stored pixel width of this node
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

/*double NodeBitmapEffect::GetPixelWidth()
{
	return 72000.0 / GetPixelsPerInch();
};*/




/********************************************************************************************

>	BOOL NodeBitmapEffect::GetOriginalBitmap(LPBITMAPINFO* plpInfo, LPBYTE* plpBits, DocRect* pRect = NULL)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/09/2004
	Inputs:		-
	Outputs:	lpInfo - reference to LPBITMAPINFO pointer to be written to for bitmap info
				lpBits - reference to LPBYTE pointer to be written to for bitmap data
	Purpose:	-

********************************************************************************************/

BOOL NodeBitmapEffect::GetOriginalBitmap(LPBITMAPINFO* plpInfo, LPBYTE* plpBits, DocRect* pRect)
{
	// ---------------------------------------------------------
	// New!
	// Discover whether our child can supply a DirectBitmap for
	// us to use as the original for processing...
	//
	LPBITMAPINFO lpLocalInfo = NULL;
	LPBYTE lpLocalBits = NULL;
	DocRect rectLocal;
	BOOL bFound = GetChildDirectBitmap(NULL, &lpLocalInfo, &lpLocalBits, &rectLocal);
	if (bFound)
	{
		if (plpInfo)	*plpInfo = lpLocalInfo;
		if (plpBits)	*plpBits = lpLocalBits;
		if (pRect)		*pRect = rectLocal;
		return (lpLocalInfo!=NULL && lpLocalBits!=NULL);		// Only return TRUE if we got good bitmap data
	}
	// ---------------------------------------------------------

	CBitmapCache* pBitmapCache = Camelot.GetBitmapCache();
	if (pBitmapCache)
	{
		// Look for a cached bitmap at the appropriate pixel size...
		CBitmapCacheKey inky(this, GetPixelWidth());					// Get cached BMP for this node at our dpi
		CCachedBitmap cbmp;
		BOOL bFound = pBitmapCache->Lookup(inky, cbmp);
		if (bFound)
		{
			if (plpInfo)	*plpInfo = cbmp.pbmpInfo;
			if (plpBits)	*plpBits = cbmp.pbmpBits;
			if (pRect)		*pRect = cbmp.GetCachedRect();
			return TRUE;
		}
	}

	if (plpInfo)	*plpInfo = NULL;
	if (plpBits)	*plpBits = NULL;

	return FALSE;
}




/********************************************************************************************

>	Void NodeBitmapEffect::SetOriginalBitmap(LPBITMAPINFO lpInfo, LPBYTE lpBits, DocRect rect)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/09/2004
	Inputs:		-
	Outputs:	lpInfo - reference to LPBITMAPINFO pointer for bitmap info
				lpBits - reference to LPBYTE pointer for bitmap data
	Purpose:	-

********************************************************************************************/

void NodeBitmapEffect::SetOriginalBitmap(LPBITMAPINFO lpInfo, LPBYTE lpBits, DocRect rect)
{
	CBitmapCache* pBitmapCache = Camelot.GetBitmapCache();
	if (pBitmapCache==NULL)
		return;

	if (rect.IsEmpty())
	{
		rect = GetChildBoundingRect();
		rect.Inflate(CAPTURE_BORDER*(INT32)GetPixelWidth());
	}

	CBitmapCacheKey inky(this, GetPixelWidth());						// Get cached BMP for this node at our dpi

	// Deallocate any cached DIB here
	CCachedBitmap cbmp;
	BOOL bFound = pBitmapCache->Lookup(inky, cbmp);
	if (bFound)
	{
		pBitmapCache->RemoveBitmap(inky);
		cbmp.Release();
	}

	if (lpInfo && lpBits)
	{
		cbmp.pbmpInfo = lpInfo;
		cbmp.pbmpBits = lpBits;
		cbmp.SetCachedRect(rect);
		cbmp.nPriority = CACHEPRIORITY_TEMPBITMAP_HIGH;

		pBitmapCache->StoreBitmap(inky, cbmp);
	}
}

/********************************************************************************************

>	virtual BOOL NodeBitmapEffect::GetChildDirectBitmap(RenderRegion* pRender,
														LPBITMAPINFO* plpInfo = NULL,
														LPBYTE* plpBits = NULL,
														DocRect* pRect = NULL,
														Matrix* pMat = NULL,
														double* pdRes = NULL
														)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/07/2005
	Inputs:		-
	Outputs:	lpInfo - reference to LPBITMAPINFO pointer to be written to for bitmap info
				lpBits - reference to LPBYTE pointer to be written to for bitmap data
	Purpose:	-

********************************************************************************************/

BOOL NodeBitmapEffect::GetChildDirectBitmap(RenderRegion* pRender,
											LPBITMAPINFO* plpInfo,
											LPBYTE* plpBits,
											DocRect* pRect,
											Matrix* pMat,
											double* pdRes
											)
{
	NodeRenderableInk* pInkNode = GetInkNodeFromController();
	if (pInkNode==NULL)
		return FALSE;

	// This is the point to choke off all DirectBitmap handling if required
	// by returning FALSE;

// TEMP BODGE! DO NOT RELEASE!
//return FALSE;
	if (!EnableDirectCapture())
		return FALSE;

	return pInkNode->GetDirectBitmap(pRender, plpInfo, plpBits, pRect, pMat, pdRes);
}




/********************************************************************************************

>	BOOL NodeBitmapEffect::GetDirectBitmap(RenderRegion* pRender, LPBITMAPINFO* plpInfo, LPBYTE* plpBits, DocRect* pRect, Matrix* pMat, double* pdRes)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/07/2005
	Inputs:		pMat	- pointer to matrix awaiting update
	Outputs:	plpInfo - LPBITMAPINFO pointer describing bitmap
				plpBits - LPBYTE pointer to bitmap data
				pRect	- rect of Original bitmap data
				pMat	- matrix describing transform from pRect to this instance on the page
				pdRes	- resolution of this bitmap (dpi)
	Returns:	TRUE if this node can supply direct bitmap data to the caller
				FALSE otherwise
	Purpose:	Return details of direct bitmap to caller (caller is usually a NodeBitmapEffect)

********************************************************************************************/

BOOL NodeBitmapEffect::GetDirectBitmap(RenderRegion* pRender, LPBITMAPINFO* plpInfo, LPBYTE* plpBits, DocRect* pRect, Matrix* pMat, double* pdRes)
{
	DocRect rectDirect;
	Matrix matLocalTransform;
	double dResolution = 0;

	// We don't change our child's instance transformation or resolution
	// So just get them directly from the child
	BOOL bChildDirect = GetChildDirectBitmap(pRender, NULL, NULL, NULL, &matLocalTransform, &dResolution);
	if (pMat)	*pMat = matLocalTransform;
	if (pdRes)	*pdRes = dResolution;

	// Initial response is to simply get the cached results of our processing
	// (See also HasCachedDirectBitmap())
	GetProcessedBitmap(bChildDirect && !matLocalTransform.IsIdentity(), plpInfo, plpBits, &rectDirect);	// Rect is in DirectBitmap domain
	if (pRect)	*pRect = rectDirect;

	// We should return our native res if our child didn't supply one
	// So that callers can check whether they have the same res and optionally make a direct connection
	// to our bitmap.
	if (!bChildDirect && pdRes)
				*pdRes = GetPixelsPerInch();

	// If we have effect attrs applied then we must make a subtree bitmap to capture them
	if (plpInfo && plpBits && HasEffectAttrs())
	{
		AttrFillGeometry* pTranspAttr = NULL;
		double dPixelWidth = GetPixelWidth();

		// Lookup processed bitmap in cache 
		CBitmapCache* pBitmapCache = Camelot.GetBitmapCache();
		if (pBitmapCache==NULL)
			return FALSE;

		CBitmapCacheKey inky2(this, dPixelWidth, 2);			// Option 2 is processed/rendered bitmap (see below)
		CCachedBitmap cbmp2;
		BOOL bFound = pBitmapCache->Lookup(inky2, cbmp2);
		if (bFound)
		{
			if (plpInfo)	*plpInfo = cbmp2.pbmpInfo;
			if (plpBits)	*plpBits = cbmp2.pbmpBits;
			return TRUE;
		}

		// TODO: Implement NodeFeatherEffect::GetDirectBitmap
		ERROR2IF(IsFeatherEffect(), FALSE, "TODO! Give NodeFeatherEffect its own implementation of GetDirectBitmap!");

		// We need to create a new bitmap here that captures the effects of the attributes
		//
		// If we don't have a RenderRegion, we can't build it, so return NULLs to indicate
		// that we need to be called again with a RenderRegion (allows quick test)
		// (See EnsureLiveEffectOriginalBitmaps)
		if (pRender == NULL)
		{
			if (plpInfo)	*plpInfo	= NULL;
			if (plpBits)	*plpBits	= NULL;
			return TRUE;
		}

		// We must "render" this bitmap to capture the attributes that effect its appearance
		// By default, we can't do arbitrarily transformed bitmaps - use a bitmap fill.
//		CBitmapCacheKey inky(this, dPixelWidth, 1);				// Option 1 is processed bitmap not including rendering
//		CCachedBitmap cbmp;
//		BOOL bFoundCached = pBitmapCache->Lookup(inky, cbmp);
//		ERROR3IF(!bFoundCached, "Effect:GetDirectBitmap failed to find processed bitmap!");
//		LPBITMAPINFO lpInfo = cbmp.pbmpInfo;
//		LPBYTE lpBits = cbmp.pbmpBits;

		// In the bitmap domain, we need to upright version of the bitmap, not the 
		// transformed parallelogram and bounds stored with the cbmp, the processed bitmap
//		DocRect CaptureRect(0, 0, lpInfo->bmiHeader.biWidth*dPixelWidth, lpInfo->bmiHeader.biHeight*dPixelWidth);

		ERROR3IF(*plpInfo==NULL || *plpBits==NULL, "How come we got here with no processed bitmap?");
		DocRect CaptureRect = rectDirect;

		CaptureFlags caFlags = CaptureFlags(cfLOCKEDTRANSPARENT | cfFULLCOVERAGE);
		pRender->StartCapture(this, CaptureRect, CAPTUREINFO(ctNESTABLE, caFlags), TRUE, FALSE, dResolution);
		pRender->SaveContext();

		{	// ------------------------------------------------------
			// Setup coords for rendering in DirectBitmap domain
			DocCoord coords[3];
			coords[0] = CaptureRect.lo;
			coords[1] = DocCoord(CaptureRect.hi.x, CaptureRect.lo.y);
			coords[2] = DocCoord(CaptureRect.lo.x, CaptureRect.hi.y);

			// NOTE! We can only deal with effect transparency at the moment!
			// Render the transparency geometry using inverse Direct-Screen transform
			// (This bitmap is being rendered in "Direct space" whereas the transparency attribute
			// was applied in "Screen space")
			NodeAttribute* pAttr;
			if (FindAppliedAttribute(CC_RUNTIME_CLASS(AttrTranspFillGeometry), &pAttr))
			{
				if (pAttr &&
					!pAttr->IsADefaultAttr() &&
					!pAttr->HasEquivalentDefaultValue(TRUE) &&
					pAttr->IsEffectAttribute())
				{
					pTranspAttr = (AttrFillGeometry*) ((AttrFillGeometry*)pAttr)->SimpleCopy();
					DocCoord tcoords[4];
					DocCoord* pCoord = NULL;
					pCoord = pTranspAttr->GetStartPoint();
					tcoords[0] = pCoord ? *pCoord : DocCoord(0,0);
					pCoord = pTranspAttr->GetEndPoint();
					tcoords[1] = pCoord ? *pCoord : DocCoord(0,0);
					pCoord = pTranspAttr->GetEndPoint2();
					tcoords[2] = pCoord ? *pCoord : DocCoord(0,0);
					pCoord = pTranspAttr->GetEndPoint3();
					tcoords[3] = pCoord ? *pCoord : DocCoord(0,0);

					Matrix matInverse = matLocalTransform.Inverse();
					matInverse.transform(tcoords, 4);

					pTranspAttr->SetStartPoint(&tcoords[0]);
					pTranspAttr->SetEndPoint(&tcoords[1]);
					pTranspAttr->SetEndPoint2(&tcoords[2]);
					pTranspAttr->SetEndPoint3(&tcoords[3]);

					pTranspAttr->Render(pRender);
				}
			}

			pRender->RenderBits(*plpInfo, *plpBits, coords, 3, FALSE, NULL);
		}	// ------------------------------------------------------

		pRender->RestoreContext();
		if (pTranspAttr)
		{
			delete pTranspAttr;
			pTranspAttr = NULL;
		}

		LPBITMAPINFO lpInfo = NULL;
		LPBYTE lpBits = NULL;
		pRender->StopCapture(this, FALSE, FALSE, &lpInfo, &lpBits, &CaptureRect);

		// We should now have a bitmap containing an upright version of the NodeBitmap
		// with transparency and contoning applied as per the attributes in the tree
		if (lpInfo && lpBits)
		{
			*plpInfo = lpInfo;
			*plpBits = lpBits;

			// Cache the PROCESSED/RENDERED bitmap as Option 2
			cbmp2.pbmpBits = lpBits;
			cbmp2.pbmpInfo = lpInfo;
			cbmp2.SetCachedRect(CaptureRect);
			cbmp2.nPriority = CACHEPRIORITY_TEMPBITMAP_HIGH;
			if (cbmp2.IsValid())
				pBitmapCache->StoreBitmap(inky2, cbmp2);
		}
	}

	return TRUE;
}




/********************************************************************************************

>	virtual BOOL NodeBitmapEffect::CanGetChildDirectBitmap()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/07/2005
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if we can get a bitmap dircetly from our controlled child node
				FALSE otherwise
	Purpose:	FInd out whether our controlled child can give us a DirectBitmap

********************************************************************************************/

BOOL NodeBitmapEffect::CanGetChildDirectBitmap()
{
	NodeRenderableInk* pInkNode = GetInkNodeFromController();
	if (pInkNode==NULL)
		return FALSE;

	BOOL bOK = EnableDirectCapture() && pInkNode->CanSupplyDirectBitmap();

	return bOK;
}




/********************************************************************************************

>	BOOL NodeBitmapEffect::RenderDirectBitmapState(RenderRegion* pRender)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/07/2005
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Render the current state into the renderregion if we haven't yet done so.
				Only used in background rendering timeslice screen update

********************************************************************************************/

BOOL NodeBitmapEffect::RenderDirectBitmapState(RenderRegion* pRender)
{
	CBitmapCache* pBitmapCache = Camelot.GetBitmapCache();
	if (pBitmapCache!=NULL && pRender->RRQuality.GetFillQuality() > Quality::Solid)
	{
		RenderCachedEffect(pBitmapCache, pRender, TRUE);
	}

	return TRUE;
}




/********************************************************************************************

>	BOOL NodeBitmapEffect::HasCachedDirectBitmap()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/07/2005
	Inputs:		-
	Outputs:	-
	Returns:	- 
	Purpose:	-

********************************************************************************************/

BOOL NodeBitmapEffect::HasCachedDirectBitmap()
{
	Matrix matTrans;
	BOOL bDirect = GetChildDirectBitmap(NULL, NULL, NULL, NULL, &matTrans, NULL);
	return (bDirect && !matTrans.IsIdentity());			// ??? Non-ident used to indicate that a direct bitmap was found under the effect...
}




/********************************************************************************************

>	BOOL NodeBitmapEffect::EnableDirectCapture()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	04/08/2005
	Inputs:		-
	Outputs:	-
	Returns:	- 
	Purpose:	Determine whether this effect can do a direct capture of its child node(s)
				or whether it needs to do a normal, rendering capture.
				At the moment, this depends solely on whether the DirectBitmap supplier
				can supply a bitmap at the same resolution as this effect requires

********************************************************************************************/

BOOL NodeBitmapEffect::EnableDirectCapture()
{
	NodeRenderableInk* pInkNode = GetInkNodeFromController();
	if (pInkNode==NULL)
		return FALSE;

PORTNOTE("effects", "EnableDirectCapture returns FALSE until effects work. Forces base objects to render themselves.");
#if !defined(EXCLUDE_FROM_XARALX)
	double dChildPPI = 0;
	BOOL bDirectResolution = pInkNode->GetDirectBitmap(NULL, NULL, NULL, NULL, NULL, &dChildPPI);
	if (!bDirectResolution || dChildPPI==0)
		return FALSE;

	// Enable Direct Capture if we are "automatic" PPI or we are the same PPI as the child direct bitmap
	if (m_dPixelsPerInch==0 || dChildPPI==m_dPixelsPerInch)
		return TRUE;
#endif 

	return FALSE;
}




/********************************************************************************************

>	DocRect NodeBitmapEffect::AdjustPixelOffsets(DocRect inRect, INT32 width, INT32 height, INT32 xOffset, INT32 yOffset, double dPixelWidth = 0)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/09/2004
	Inputs:		-
	Outputs:	-
	Purpose:	-
	Notes:		Width, height, xoffset, yoffset refer to bitmap in standard Windows
				fashion - Y coords increase downwards, offsets refer to top-left corner
				This routine must move from one coord system to the other while it updates the
				supplied rectangle

********************************************************************************************/

DocRect NodeBitmapEffect::AdjustPixelOffsets(DocRect inRect, INT32 width, INT32 height, double xOffset, double yOffset, double dPixelWidth)
{
	if (dPixelWidth==0)
		dPixelWidth = GetPixelWidth();

	DocRect outRect(inRect.lo.x, inRect.hi.y, inRect.lo.x, inRect.hi.y);
															// Set new bitmap origin to old bitmap origin
	outRect.lo.x += INT32( xOffset * dPixelWidth );			// Set new bitmap origin X (-ve offset means "move left")
	outRect.hi.y -= INT32( yOffset * dPixelWidth );			// Set new bitmap origin Y (-ve yoffset means "move up")

	if (width==0)
		outRect.hi.x = outRect.lo.x + inRect.Width();
	else
		outRect.hi.x = outRect.lo.x + INT32( width * dPixelWidth );	// Set width of new bitmap

	if (height==0)
		outRect.lo.y = outRect.hi.y - inRect.Height();
	else
		outRect.lo.y = outRect.hi.y - INT32( height * dPixelWidth );	// Set height of new bitmap

	return outRect;
}




/********************************************************************************************

>	IXMLDOMDocumentPtr NodeBitmapEffect::GetEditList()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/09/2004
	Inputs:		-
	Outputs:	-
	Purpose:	-

********************************************************************************************/

IXMLDOMDocumentPtr NodeBitmapEffect::GetEditList()
{
	return m_pEditsDoc;
}




/********************************************************************************************

>	void NodeBitmapEffect::SetEditList(IXMLDOMDocumentPtr pNewEditsDoc)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/09/2004
	Inputs:		-
	Outputs:	-
	Purpose:	-

********************************************************************************************/

void NodeBitmapEffect::SetEditList(IXMLDOMDocumentPtr pNewEditsDoc)
{
	m_pEditsDoc = pNewEditsDoc;
}




/********************************************************************************************

>	BOOL NodeBitmapEffect::SetDefaultEditList()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/07/2005
	Inputs:		-
	Outputs:	-
	Purpose:	-

********************************************************************************************/

BOOL NodeBitmapEffect::SetDefaultEditList()
{
	PORTNOTETRACE("other","NodeBitmapEffect::SetDefaultEditList - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	// Ensure that we have an edit list
	if (m_pEditsDoc==NULL)
	{
		m_pEditsDoc = CXMLUtils::NewDocument();
		ERROR3IF(m_pEditsDoc==NULL, "Can't make default edit document");
		return (m_pEditsDoc!=NULL);
	}
#endif
	return TRUE;
}




/********************************************************************************************

>	void NodeBitmapEffect::RenderAfterSubtree(RenderRegion* pRender)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/09/2004
	Inputs:		pRender - The region to render into
	Purpose:	Capture the group as a cached bitmap

********************************************************************************************/

void NodeBitmapEffect::RenderAfterSubtree(RenderRegion* pRender)
{
	Matrix matTrans;
	double dResolution = 0;

	CBitmapCache* pBitmapCache = Camelot.GetBitmapCache();
	if (pBitmapCache==NULL)
	{
		return;
	}

	// --------------------------------------------------------------------------------------
	// Note that we use the Capture system to store the state of rendering
	// in this node in the scope of the rendering system rather than storing
	// it temporarilly in the document tree (e.g. in this node). This means
	// the tree is stateless and can be rendered by several different "threads"
	// with different states at the same time.
	// In this context a "thread" is a background, rendering RenderRegion but
	// it could be a real thread one day.
	//
	Capture* pCapture = pRender->GetTopCapture();
	if (pCapture==NULL)											// If nothing was captured
		return;													// Then do nothing

	// Only stop the capture if we started it
	// (means we only capture whole subtrees at the mo.)
	if (pCapture->GetOwner()==this)
	{
		// End this capture:
		// Blit capture to screen
		// Retain bitmap because we will release it ourselves only if we fail to cache it
		BOOL bCached = FALSE;
		BOOL bCaptureIsDirect = pCapture->IsDirect();
		LPBITMAPINFO lpInfo = NULL;
		LPBYTE lpBits = NULL;
		DocRect CaptureRect = GetChildBoundingRect();				// Set maximum size we allow
		CaptureRect.Inflate(CAPTURE_BORDER*(INT32)GetPixelWidth());
		pRender->StopCapture(this, FALSE, FALSE, &lpInfo, &lpBits, &CaptureRect, &matTrans, &dResolution);
																	// pCapture is deleted now...

		// Is this capture direct?
		if (bCaptureIsDirect)
		{
			ERROR3IF(dResolution==0, "Direct Capture returned 0 pixels per inch\n");
			// If we are supplied with a direct bitmap, try to process it and cache
			// the results.
			// 
			// Note that CaptureRect is in DirectBitmap space, not drawing space
			// and that matTransform holds the matrix to map from DirectBitmap space
			// to drawing space...
			//
			if (lpInfo && lpBits && CaptureRect.IsValid())
			{
				// Process the bitmap using appropriate virtual functions from derived class
				LPBITMAPINFO lpProcessedInfo = NULL;
				LPBYTE lpProcessedBits = NULL;
				double xOffset = 0;
				double yOffset = 0;
				ProcessBitmap(pRender, pBitmapCache, lpInfo, lpBits, CaptureRect, lpProcessedInfo, lpProcessedBits, &xOffset, &yOffset);

				if (lpProcessedInfo && lpProcessedBits)
				{
					// Cache the PROCESSED bitmap, as Option 1
					//
					// Note that SetProcessedBitmap stores the bitmap with coords transformed into Drawing space
					// So that rendering into drawing space will be quick and simple
					// The rectangle in DirectBitmap space is stored in a member variable m_rectDirectBitmap
					SetProcessedBitmap(lpProcessedInfo, lpProcessedBits, CaptureRect, lpProcessedInfo->bmiHeader.biWidth, lpProcessedInfo->bmiHeader.biHeight, xOffset, yOffset, 72000.0/dResolution, &matTrans);

					// Note RenderCachedEffect only does anything if the current capture
					// doesn't consume this bitmap directly
					RenderCachedEffect(pBitmapCache, pRender);
				}
			}
		}
		else
		{
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

				// Process the bitmap using appropriate virtual processor for derived class
				LPBITMAPINFO lpProcessedInfo = NULL;
				LPBYTE lpProcessedBits = NULL;
				double xOffset = 0;
				double yOffset = 0;

				//BOOL bOK =
				ProcessBitmap(pRender, pBitmapCache, lpInfo, lpBits, CaptureRect, lpProcessedInfo, lpProcessedBits, &xOffset, &yOffset);
//				ERROR3IF(!bOK, "NodeBitmapEffect::RenderAfterSubtree ProcessBitmap failed\n");

				if (lpProcessedInfo && lpProcessedBits)
				{
					// Cache the PROCESSED bitmap as well, as Option 1
					CaptureRect = SetProcessedBitmap(lpProcessedInfo, lpProcessedBits, CaptureRect, lpProcessedInfo->bmiHeader.biWidth, lpProcessedInfo->bmiHeader.biHeight, xOffset, yOffset);

					// Render it
					RenderCachedEffect(pBitmapCache, pRender);

					// If we are rendering during printing, there may be multiple passes
					// through the tree to do different aspects of printing. The
					// PrintingMaskedRenderRegion produces a monochrome mask so we
					// definitely don't want to cache that!
PORTNOTE("printing", "NodeBitmapEffect::RenderAfterSubtree - removed use of PrintingMaskedRenderRegion")
#if !defined(EXCLUDE_FROM_XARALX)
					if (pRender->IsKindOf(CC_RUNTIME_CLASS(PrintingMaskedRenderRegion)))
					{
						double PixelWidth = GetPixelWidth();
						CBitmapCacheKey inky(this, PixelWidth, 0);
						pBitmapCache->RemoveBitmap(inky);

						CBitmapCacheKey inky2(this, PixelWidth, 1);
						pBitmapCache->RemoveBitmap(inky2);

						bCached = FALSE;
					}
#endif

					// If we failed to cache the captured bitmap then release it
					if (lpProcessedInfo!=NULL && lpProcessedBits!=NULL && !bCached)
					{
						FreeDIB(lpProcessedInfo, lpProcessedBits, NULL, FALSE);
					}
				}
				else
				{
					// If we failed to process the captured bitmap for some reason
					// Then just plot it back to the screen so the user sees something
//					ERROR3("NodeBitmapEffect::RenderAfterSubtree ProcessBitmap failed\n");
					pRender->RenderBits(lpInfo, lpBits, CaptureRect);
				}
			}

			// If we failed to cache the captured bitmap then release it
			if (lpInfo!=NULL && lpBits!=NULL && !bCached)
			{
				FreeDIB(lpInfo, lpBits, NULL, FALSE);
			}
		}
	}
}




/***********************************************************************************************

>	virtual void NodeBitmapEffect::Render(RenderRegion* pRRegion)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/2004
	Inputs:		Pointer to a render region
	Purpose:	Will render the liveeffect iff we have it cached - for hit detection reasons

***********************************************************************************************/

void NodeBitmapEffect::Render(RenderRegion* pRender)
{
	CBitmapCache* pBitmapCache = Camelot.GetBitmapCache();
	if (pBitmapCache!=NULL && pRender->IsHitDetect() && pRender->RRQuality.GetFillQuality() > Quality::Solid)
	{
		RenderCachedEffect(pBitmapCache, pRender, TRUE);
	}
}



/********************************************************************************************

>	virtual BOOL NodeBitmapEffect::FindCachedEffect(CBitmapCache* pBitmapCache)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/11/2004
	Inputs:		-
	Outputs:	-
	Purpose:	Find all cached items this node needs to call RenderCachedEffect
				succesfully

********************************************************************************************/

BOOL NodeBitmapEffect::FindCachedEffect(CBitmapCache* pBitmapCache)
{
//	BOOL bRendered = FALSE;

	CBitmapCacheKey inky(this, GetPixelWidth(), 1);						// Get cached BMP for this PROCESSED node at our dpi
	CCachedBitmap cbmp;
	BOOL bFoundCached = pBitmapCache->Lookup(inky, cbmp);

	return bFoundCached;
}




/********************************************************************************************

>	BOOL NodeBitmapEffect::RenderCachedEffect(CBitmapCache* pBitmapCache, RenderRegion* pRender, BOOL bIgnoreCapture = FALSE)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/11/2004
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the cached effect was rendered or passed on to a consumer capture
				FALSE if the cached effect was not present or could not be consumed or rendered
	Purpose:	Render the cached version of this node

********************************************************************************************/

BOOL NodeBitmapEffect::RenderCachedEffect(CBitmapCache* pBitmapCache, RenderRegion* pRender, BOOL bIgnoreCapture)
{
	BOOL bRendered = FALSE;

	CBitmapCacheKey inky(this, GetPixelWidth(), 1);						// Get cached BMP for this PROCESSED node at our dpi
	CCachedBitmap cbmp;
	BOOL bFoundCached = pBitmapCache->Lookup(inky, cbmp);

	if (bFoundCached)
	{
		// No need to render if we are going to supply to a capture
		Capture* pCapture = pRender->GetTopCapture();
		if (!bIgnoreCapture && pCapture && pCapture->ConsumeDirectBitmap(this))
		{
			// Tell the caller that the effect has been "rendered"
			bRendered = TRUE;
		}
		else
		{
			// Allow RenderBits to render any effect attributes that are attached to us
			// before rendering the bitmap so that they affect its appearance
			// by passing it a pointer to this node to search for effect attributes
			bRendered = pRender->RenderBits(cbmp.pbmpInfo, cbmp.pbmpBits, &cbmp.coord0, 3, TRUE, this);
		}
	} 

	return bRendered;
}




/********************************************************************************************
>	virtual BOOL NodeBitmapEffect::ProcessBitmap(RenderRegion* pRender,
												  CBitmapCache* pBitmapCache,
												  LPBITMAPINFO pBMPInfo,
												  LPBYTE pBMPBits,
												  DocRect BMPRect,
												  LPBITMAPINFO& pOutputInfo,
												  LPBYTE& pOutputBits,
												  double* pXOffset,
												  double* pYOffset)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/11/2004
	Returns:	
	Purpose:	Processes the bitmap produced by the children of this node.
				Can be overridden by derived classes to perofrm special processing

********************************************************************************************/

BOOL NodeBitmapEffect::ProcessBitmap(RenderRegion* pRender,
									  CBitmapCache* pBitmapCache,
									  LPBITMAPINFO pBMPInfo,
									  LPBYTE pBMPBits,
									  DocRect BMPRect,
									  LPBITMAPINFO& pOutputInfo,
									  LPBYTE& pOutputBits,
									  double* pXOffset,
									  double* pYOffset)
{
	BOOL				bOK = TRUE;

	PORTNOTETRACE("other","NodeBitmapEffect::ProcessBitmap - do nothing, CXMLUtils, XPEEditOp");
#ifndef EXCLUDE_FROM_XARALX
	// Apply the effect to the newly captured bitmap
	BOOL bValidEditList = TRUE;
	if (m_pEditsDoc==NULL)
	{
		HRESULT hr = E_FAIL;
		m_pEditsDoc = CXMLUtils::NewDocument();
		bValidEditList = FALSE;
		ERROR3IF(m_pEditsDoc==NULL, "Can't make default edit document");
	}

	if (bValidEditList)
	{
//TRACEUSER( "Phil", _T("NodeLiveEffect calling ProcessBitmap %x %s\n"), this, this->GetRuntimeClass()->m_lpszClassName);
		INT32 iXOffset = (INT32)*pXOffset;
		INT32 iYOffset = (INT32)*pYOffset;
		bOK = XPEEditOp::ProcessBitmap(pBMPInfo, pBMPBits, pOutputInfo, pOutputBits, m_pEditsDoc, &iXOffset, &iYOffset);
		*pXOffset = iXOffset;
		*pYOffset = iYOffset;
//TRACEUSER( "Phil", _T("NodeLiveEffect back from ProcessBitmap\n"));
	} 
#endif
	return bOK;
}

			

			
/********************************************************************************************
>	DocRect NodeBitmapEffect::GetBlobBoundingRect()

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com> (re-written)
	Created:	29/6/95
	Returns:	DocRect - The bounding rect of the group and its blobs
	Purpose:	Finds the bounding rect of the group along with its blobs. Its main use is
				to determine which areas of the document to invalidate
********************************************************************************************/

DocRect NodeBitmapEffect::GetBlobBoundingRect()
{
	DocRect NewRect = DocRect(0,0,0,0);

#if !defined(EXCLUDE_FROM_RALPH)
	// Find the Blob manager
	BlobManager* pBlobMgr = GetApplication()->GetBlobManager();
	if (pBlobMgr!= NULL)
	{
//		BlobStyle VisibleBlobs = pBlobMgr->GetCurrentInterest();
//		if (VisibleBlobs.Object)
//		{
			DocRect BoundingRect = GetBoundingRect();

			// Object blobs are visible, so include blob in each
			// corner of the group bounds
			DocRect BlobSize;

			// Find out where the blobs	will be drawn
			DocCoord Low  = BoundingRect.LowCorner();
			DocCoord High = BoundingRect.HighCorner();

			// Include the object blob in each corner
			pBlobMgr->GetBlobRect(Low, &BlobSize);
			NewRect = NewRect.Union(BlobSize);
			pBlobMgr->GetBlobRect(High, &BlobSize);
			NewRect = NewRect.Union(BlobSize);
			pBlobMgr->GetBlobRect(DocCoord(Low.x, High.y), &BlobSize);
			NewRect = NewRect.Union(BlobSize);
			pBlobMgr->GetBlobRect(DocCoord(High.x, Low.y), &BlobSize);
			NewRect = NewRect.Union(BlobSize);
//		}
	}

	Node* pNode=FindFirstChild();
	while (pNode!=NULL)
	{
		if (pNode->IsBounded())
			NewRect = NewRect.Union(((NodeRenderableBounded*)pNode)->GetBlobBoundingRect());
		pNode = pNode->FindNext();
	}

	IncludeChildrensBoundingRects(&NewRect);
#endif
	return NewRect;
}




/********************************************************************************************
>	virtual BOOL NodeBitmapEffect::CompareState(NodeEffect* pPPNode)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/11/2004
	Returns:	TRUE if this state matches the state (and type) of the supplied node
	Purpose:	-
	Notes:		Would be more efficient to pass in level xml instead of getting it inside
				this function

********************************************************************************************/

BOOL NodeBitmapEffect::CompareState(NodeEffect* pPPNode)
{
	if (!pPPNode->IsBitmapEffect())
		return FALSE;

	PORTNOTETRACE("other","NodeBitmapEffect::CompareState - do nothing, XML and BSTR");
#ifndef EXCLUDE_FROM_XARALX
	BSTR temp;
	HRESULT hr;

	_bstr_t bstrThisEditDoc;
	IXMLDOMDocumentPtr pThisEditDoc = GetEditList();
	hr = pThisEditDoc->get_xml(&temp);
	bstrThisEditDoc = temp;

	_bstr_t bstrTestEditDoc;
	IXMLDOMDocumentPtr pTestEditDoc = ((NodeBitmapEffect*)pPPNode)->GetEditList();
	hr = pTestEditDoc->get_xml(&temp);
	bstrTestEditDoc = temp;

	return (bstrTestEditDoc == bstrThisEditDoc); */
#else
	return false;
#endif
}




/********************************************************************************************

>	BOOL NodeBitmapEffect::DoBecomeA(BecomeA* pBecomeA);

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/02/2005
	Inputs:		BecomeA struct
	Purpose:	Converts me to a path
	Notes:		Note that when this routine creates paths it does NOT call DoBecomeA on its
				children because the paths and the bitmap fills represetn everything the
				child objects have rendered.
	SeeAlso:	

********************************************************************************************/
BOOL NodeBitmapEffect::DoBecomeA(BecomeA* pBecomeA)
{
	// Check for a NULL entry param
	ERROR2IF(pBecomeA == NULL, FALSE, "pBecomeA is NULL");

	if (!pBecomeA->BAPath())
		return FALSE;

	// This lump checks that the Reason is one that we understand
	// It also makes sure that we don't have a NULL UndoOp ptr
	BOOL ValidReason = (pBecomeA->GetReason() == BECOMEA_REPLACE || pBecomeA->GetReason() == BECOMEA_PASSBACK);
	ERROR2IF_PF(!ValidReason,FALSE,("Unkown BecomeA reason %d",pBecomeA->GetReason()));

	BOOL bOK						= FALSE;
	NodePath* pNewNodePath			= NULL;
	NodeAttribute* pAttr			= NULL;
	NodeHidden* pHiddenNode			= NULL;
//	AttrLineWidth* pLineWidth		= NULL;
	UndoableOperation* pOp			= pBecomeA->GetUndoOp();

	// Don't make a dumb square bitmap if the Op is a blend op because in that case
	// it's more useful to have the original shape with the effect
	// Or if this node is in the top surface of those being considered
	//		and the op tells us that the results of dobecomea will be left in place (i.e. under
	//		and effect stack)
	if (!IsLockedEffect())
	{
		if (pBecomeA->IsBlendBecomeA() || pBecomeA->DoSilhouette() || pBecomeA->OnlyNeedPaths() || pBecomeA->ResultsStayInPlace())
			return NodeEffect::DoBecomeA(pBecomeA);
	}
	else
	{
		if (pBecomeA->OnlyNeedPaths() || (pBecomeA->IsBlendBecomeA() && pBecomeA->GetReason() == BECOMEA_PASSBACK))
			return NodeEffect::DoBecomeA(pBecomeA);
	}

	// We only do conversions if we are the top effect in the stack
	if (!IsLockedEffect() && FindParent() && FindParent()->IsEffect())
		return TRUE;

	// Get a permanent kernel bitmap to fill the path
	DocCoord coords[3];
	KernelBitmap* kBitmap = GetKernelBitmap(coords, TRUE);	// Retain the cached data because we may be called again (blends with multiple paths)
	// Note: kBitmap may be NULL

	// Create a NodePath containing the parallelogram from the cache
	ALLOC_WITH_FAIL(pNewNodePath, new NodePath, pOp); 
	if (pNewNodePath)
	{
		Path InkPath;
		InkPath.Initialise();
		InkPath.AddMoveTo(coords[0]);
		InkPath.AddLineTo(coords[1]);
		InkPath.AddLineTo(DocCoord(coords[2].x+coords[1].x-coords[0].x, coords[2].y+coords[1].y-coords[0].y));
		InkPath.AddLineTo(coords[2]);
		InkPath.AddLineTo(coords[0]);
		InkPath.CloseSubPath();

		pNewNodePath->InkPath.Initialise();
		pNewNodePath->InkPath.CloneFrom(InkPath);
		pNewNodePath->InkPath.IsFilled = TRUE;
		pNewNodePath->InkPath.TryToClose();
		pNewNodePath->InkPath.InitialiseFlags();

		switch (pBecomeA->GetReason())
		{
		case BECOMEA_REPLACE:
			{
				// Note! In many cases we are just being asked to become a path to get
				// the path data ignoring the attributes.
				// So if we can't make the attribute, it's better tp just go ahead
				// and return the path data without them.
				//
				// Make a new bitmap fill attribute referring to the newly created Kernel bitmap
				BitmapFillAttribute* pBitmapAttr = NULL;
				ALLOC_WITH_FAIL(pBitmapAttr, new BitmapFillAttribute, pOp);
				if (pBitmapAttr!=NULL && kBitmap)
				{
					pBitmapAttr->GetBitmapRef()->SetBitmap(kBitmap);
					pBitmapAttr->StartPoint = coords[0];
					pBitmapAttr->EndPoint   = coords[1];
					pBitmapAttr->EndPoint2 	= coords[2];

					// Make sure the Fill attributes point to the new bitmap!
					pAttr = pBitmapAttr->MakeNode();
					pAttr->AttachNode(pNewNodePath, FIRSTCHILD);
					delete pBitmapAttr;
					pBitmapAttr = NULL;
				}

				AttrStrokeColour* pStrokeColour = NULL;
				ALLOC_WITH_FAIL(pStrokeColour, new AttrStrokeColour, pOp);
				if (pStrokeColour!=NULL)
				{
					// Also make the line colour NONE
					DocColour colNone(COLOUR_NONE);
					pStrokeColour->SetStartColour(&colNone);
					pStrokeColour->AttachNode(pNewNodePath, FIRSTCHILD);
				}

				bOK = TRUE;

				// Copy any effect attributes over to the new node
				// where they can become normal attributes now that we have captured
				// the effect as a bitmap
				NodeAttribute* pEffectAttr = NodeAttribute::FindFirstAppliedAttr(this);
				while (bOK && pEffectAttr && pEffectAttr->IsEffectAttribute())
				{
					CALL_WITH_FAIL(pEffectAttr->CopyNode(pNewNodePath, FIRSTCHILD), pOp, bOK);

					pEffectAttr = NodeAttribute::FindPrevAppliedAttr(pEffectAttr);
				}

				// Now insert the new node
				if (bOK)
				{
					if (pOp)
					{
						bOK = pOp->DoInsertNewNode(pNewNodePath, this, NEXT, TRUE, FALSE, this->IsSelected() || this->IsParentOfSelected(), TRUE);
					}
					else
					{
						pNewNodePath->AttachNode(this, NEXT);
						pNewNodePath->Select(this->IsSelected() || this->IsParentOfSelected());
					}
				}

				// Handle the passback bits
				if (bOK)
				{
					CCAttrMap ThisMap;
					CCAttrMap* pThisMap = NULL;
					if (pNewNodePath->FindAppliedAttributes(&ThisMap))
						pThisMap = ThisMap.Copy();
					pBecomeA->PassBack(pNewNodePath, this, pThisMap);
				}

				// Now hide (or delete) this node
				if (bOK)
				{
					if (pOp)
					{
						bOK = pOp->DoHideNode(this, TRUE, &pHiddenNode, FALSE);
					}
					else
					{
						CascadeDelete();
						delete this;	// SCARY!
					}
				}
			}
			break;

		case BECOMEA_PASSBACK:
			{
				// if we're passing back, just do the first path
				CCAttrMap ThisMap;
				if (FindAppliedAttributes(&ThisMap))
				{
					// make a copy of this attribute map
					CCAttrMap* pNewAttrMap = ThisMap.Copy();
					AttrStrokeColour* pStrokeColour = NULL;
					// get the stroke colour attribute out of this map and set it to COLOUR_NONE
					if( pNewAttrMap->Lookup( CC_RUNTIME_CLASS(AttrStrokeColour), (void*&)(pStrokeColour) ) )
					{
						if (pStrokeColour && pStrokeColour->GetStartColour())
						{
							DocColour colNone(COLOUR_NONE);
							pStrokeColour->SetStartColour(&colNone);
						}
					}

					// Replace the Fill attribute in the Attr Map copy
					// with a bitmap fill
					AttrFillGeometry* pBitmapFill = new AttrBitmapColourFill();
					if (pBitmapFill && kBitmap)
					{
						// Make a Bitmap Fill that is equivalent to this
						// node bitmap
						pBitmapFill->AttachBitmap(kBitmap);

						// Set any contone colours that we have
//						pBitmapFill->SetStartColour(ContoneStart);
//						pBitmapFill->SetEndColour(ContoneEnd);

						// Set the control points from the NodeBitmap Parallelogram
						pBitmapFill->SetStartPoint(&coords[0]);
						pBitmapFill->SetEndPoint(&coords[1]);
						pBitmapFill->SetEndPoint2(&coords[2]);

						void* pOldFill;
						if (pNewAttrMap->Lookup(CC_RUNTIME_CLASS(AttrFillGeometry), pOldFill))
						{
							// We need to Remove and Delete the existing Fill Attr
							pNewAttrMap->RemoveKey(CC_RUNTIME_CLASS(AttrFillGeometry));
							delete (AttrFillGeometry*)pOldFill;
						}

						// Add the Bitmap fill into the Attr Map
						pNewAttrMap->SetAt(CC_RUNTIME_CLASS(AttrFillGeometry),(void*)pBitmapFill);
					}

					// ok, now lets set up the attribute maps
					pBecomeA->PassBack(pNewNodePath, this, pNewAttrMap);
					bOK = TRUE;
				}
			}
			break;

		case BECOMEA_TEST:
			ERROR3("Unhandled BECOMEA type\n");
			break;
		}
	}

	if (!bOK)
	{
		if (pNewNodePath != NULL)
		{
			// Delete all the NodePath's children (if it has any) and unlink it from the tree (if it's linked)
			// This is all done by CascadeDelete()
			pNewNodePath->CascadeDelete(); 
			delete pNewNodePath;
			pNewNodePath = NULL;
		}
	}

	return bOK;
}




/********************************************************************************************

>	BOOL NodeBitmapEffect::CanBecomeA(BecomeA* pBecomeA);

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/02/2005
	Inputs:		Ink class to test
	Purpose:	Can become a path !
	SeeAlso:	

********************************************************************************************/

BOOL NodeBitmapEffect::CanBecomeA(BecomeA* pBecomeA)
{
	// Don't make a dumb square bitmap if the Op is a blend op because in that case
	// it's more useful to have the original shape with the effect
	// Or if this node is in the top surface of those being considered
	//		and the op tells us that the results of dobecomea will be left in place (i.e. under
	//		and effect stack)
	if (!IsLockedEffect())
	{
		if (pBecomeA->IsBlendBecomeA() || pBecomeA->ResultsStayInPlace())
		{
			return NodeEffect::CanBecomeA(pBecomeA);
		}
	}

	// We only do conversions if we are the top effect in the stack
	if (!IsLockedEffect() && FindParent() && FindParent()->IsEffect())
		return FALSE;

	if (pBecomeA->BAPath())
	{
		pBecomeA->AddCount(1);

		return TRUE;
	}

	return FALSE;
}

/********************************************************************************************

>	virtual BOOL NodeBitmapEffect::CanGenerateBitmap()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	01/07/2005
	Inputs:		-
	Returns:	TRUE if this effect node can generate an original bitmap
				FALSE otherwise
	Purpose:	Test whether node is big enough to bother with
	SeeAlso:	

********************************************************************************************/

BOOL NodeBitmapEffect::CanGenerateBitmap()
{
	double PixelWidth = GetPixelWidth();
	DocRect CaptureRect = GetChildBoundingRect();
	return (CaptureRect.Width()>=PixelWidth && CaptureRect.Height()>=PixelWidth);
}




/********************************************************************************************

>	virtual BOOL NodeBitmapEffect::WriteBoundsRecord(BaseCamelotFilter* pFilter)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/09/2005
	Inputs:		pFilter = ptr to filter to write to
	Returns:	TRUE if ok, FALSE otherwise
	Purpose:	Begin to write out your child records, in the native format

				The base class will write out a TAG_DOWN record, but only if it has a child ptr

	SeeAlso:	WritePostChildrenWeb(), WritePreChildrenWeb()

********************************************************************************************/

BOOL NodeBitmapEffect::WriteBoundsRecord(BaseCamelotFilter* pFilter)
{
	BOOL ok = TRUE;

	// Add a description of the TAG_COMPOUNDRENDER record, for older importers that don't understand this record
	pFilter->AddTagDescription(TAG_COMPOUNDRENDER, _R(IDS_TAG_COMPOUNDRENDER));

	// Write out the record
	DocRect Rect = GetChildBoundingRect();

	CamelotFileRecord Rec(pFilter, TAG_COMPOUNDRENDER, TAG_COMPOUNDRENDER_SIZE);
	ok = Rec.Init();
	if (ok) ok = Rec.WriteUINT32(0);			// Reserved
	if (ok) ok = Rec.WriteCoord(Rect.lo);
	if (ok) ok = Rec.WriteCoord(Rect.hi);
	if (ok) ok = pFilter->Write(&Rec);

	return ok;
}





/********************************************************************************************
	Class:		NodeLiveEffect

********************************************************************************************/

/***********************************************************************************************

>	NodeLiveEffect::NodeLiveEffect(Node* 	ContextNode,
						AttachNodeDirection Direction,
						const DocRect& 		BoundingRect,
						BOOL				Locked = FALSE,
						BOOL 				Mangled = FALSE,
						BOOL 				Marked = FALSE,
						BOOL 				Selected = FALSE,
						BOOL 				Renderable = FALSE
						)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/09/2004
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
NodeLiveEffect::NodeLiveEffect(Node* ContextNode,  
					AttachNodeDirection Direction,  
					BOOL Locked, 
					BOOL Mangled,  
					BOOL Marked, 
					BOOL Selected    
			  ) : NodeBitmapEffect(ContextNode, Direction, Locked, Mangled, Marked, Selected )
{                         
	m_rectEstimatedBounds = DocRect(0,0,0,0);
	m_lMaxBorder = 0;
	m_dPixelsPerInch = (double)NodeBitmapEffect::DefaultLivePixelsPerInch;
}                        
 



/*********************************************************************************************

>	NodeLiveEffect::NodeLiveEffect() 

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/09/2004
	Purpose:	
	Note:		
	SeeAlso:	

**********************************************************************************************/
NodeLiveEffect::NodeLiveEffect() : NodeBitmapEffect()
{
	m_rectEstimatedBounds = DocRect(0,0,0,0);
	m_lMaxBorder = 0;
	m_dPixelsPerInch = (double)NodeBitmapEffect::DefaultLivePixelsPerInch;
}




/*********************************************************************************************

>	NodeLiveEffect::~NodeLiveEffect() 

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/09/2004
	Purpose:	Destructor
	Note:		
	SeeAlso:	

**********************************************************************************************/
NodeLiveEffect::~NodeLiveEffect()
{
	m_pEditsDoc = NULL;				// m_pEditsDoc is a smart pointer so NULLing it Releases the COM interface
									// and releases any memory
	CBitmapCache* pBitmapCache = Camelot.GetBitmapCache();
	if (pBitmapCache!=NULL)
	{
		CBitmapCacheKey inky(this, 42);
		pBitmapCache->RemoveAllOwnedBitmaps(inky, FALSE, CACHEPRIORITY_PERMANENT);
	}
}




/********************************************************************************************

>	DocRect NodeLiveEffect::GetBoundingRect(BOOL DontUseAttrs, BOOL HitTest)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/09/2004
	Returns:	The bounding box of the live effect
	Purpose:	Get the bounding rect of the cached processed bitmap
				or the original bitmap if the processed isn't set yet...
				or the children if neither is set yet...

********************************************************************************************/

DocRect NodeLiveEffect::GetBoundingRect(BOOL DontUseAttrs, BOOL HitTest)
{
	// Let sanity prevail!
	BOOL bIncludeAttrs = !DontUseAttrs;

	// Optimise this function by returning fast cached info if we know it's valid.
	if (IsBoundingRectValid && bIncludeAttrs)
		return BoundingRectangle;

	// Else work it out by looking up the cached bitmaps
	// and failing that by scanning our children
	if (bIncludeAttrs)
	{
		// We can check the bounds of our whacky effects bitmap
		CBitmapCache* pBitmapCache = Camelot.GetBitmapCache();
		if (pBitmapCache!=NULL)
		{
			// Look for cached processed bitmap
			CBitmapCacheKey inky(this, GetPixelWidth(), 1);				// Get cached BMP for this node at our dpi
			CCachedBitmap cbmp;
			BOOL bFound = pBitmapCache->Lookup(inky, cbmp);
			if (bFound)
			{
				BoundingRectangle = cbmp.GetCachedRect();
				IsBoundingRectValid = TRUE;
				return BoundingRectangle;
			}
		}
	}

	// If we get here, we don't know what the bounds of the effect bitmap
	// or what they might be...
	// Make a best guess using previous knowledge if we have it
	//		m_rectEstimatedBounds
	//		m_lMaxBorder
	// and factor in an arbitrary border to allow the effect to create
	// a bigger bitmap than the original data.
	//
	DocRect brect = GetChildBoundingRect(bIncludeAttrs);				// Note - don't set IsBoundingRectValid yet

	if (bIncludeAttrs)
	{
		brect.Inflate(CAPTURE_BORDER*(INT32)GetPixelWidth());

		// Inflate by the previously known border width
		brect.Inflate((INT32)m_lMaxBorder);

		// NOTE! There's a possible optimisation to be had here:
		// If the border is known (not just default value) and is zero then we know that
		// the effect will not spread the bounds and so we don't need to expand the bounds
		// by 50 pixels to cope with it!
		//
		// Expand bounds to account for as yet unknown size increases that effects might apply
		brect.Inflate((INT32)GetPixelWidth()*50);

		// Include transformed bounds of last known bounding rect
		if (!m_rectEstimatedBounds.IsEmpty())
			brect = brect.Union(m_rectEstimatedBounds);
	}

	return brect;
}




/********************************************************************************************

>	SubtreeRenderState NodeLiveEffect::RenderSubtree(RenderRegion* pRender, Node** ppNextNode, BOOL bClip)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/09/2004
	Inputs:		pRender - The region render into (or use as context for decisions about subtree)
				ppNextNode - Address of node pointer for next node to render or run to after this
				bClip - flag indicating whether to clip or not
	Purpose:	Do clever stuff on the way into a subtree, possibly modifying rendering
				behaviour.

********************************************************************************************/

SubtreeRenderState NodeLiveEffect::RenderSubtree(RenderRegion* pRender, Node** ppNextNode, BOOL bClip)
{
	BOOL bRendered = FALSE;

	if (pRender == NULL)							// If no render region supplied, assume we need to be rendered
		return SUBTREE_ROOTANDCHILDREN;

	// Let children render themselves directly if we're not showing filled paths so the user can
	// see their outlines
	if (pRender->RRQuality.GetFillQuality() <= Quality::Solid)
		return SUBTREE_ROOTANDCHILDREN;

	// Live effects need to be rendered in the complex bitmap stages of printing
	if (pRender->IsKindOf(CC_RUNTIME_CLASS(ScanningRenderRegion)))
	{
		ScanningRenderRegion* pScanner = (ScanningRenderRegion*)pRender;
		pScanner->ComplexShapeFoundWrapper();
		return SUBTREE_NORENDER;	// Don't render any of our children
	}

	CBitmapCache* pBitmapCache = Camelot.GetBitmapCache();
	if (pBitmapCache==NULL)
		return SUBTREE_NORENDER;

	// Find out whether we have a cached processed bitmap for this node...
	BOOL bFoundCached = FindCachedEffect(pBitmapCache);

	// Go find out about my bounding rectangle
	// If we don't have a cached bitmap then expand the bounding rect to ensure it will cover an expansion caused by the effect
	// If we do have a cached bitmap then use the bounding rect stored with the processed bitmap
	DocRect BoundingRect = GetBoundingRect();
	if (!BoundingRect.IsValid())
		return SUBTREE_ROOTANDCHILDREN;

	if (!bFoundCached)
		BoundingRect.Inflate((INT32)(GetPixelWidth()*150));		// Bodge to ensure LE is rendered before it grows
	DocRect ClipRect = pRender->GetClipRect();

	if (bClip && !ClipRect.IsIntersectedWith(BoundingRect))		// If not within the clipping rect then
		return SUBTREE_NORENDER;								// Don't render us or our children

	// No need to check BackmostChanged node here because we never capture
	// our background in a 24BPP bitmap we always capture at 32BPP using cfLOCKEDTRANSPARENT

	// Now ask our derived classes to render what they need to render
	bRendered = RenderCachedEffect(pBitmapCache, pRender);

	if (bRendered)
		return SUBTREE_NORENDER;

	// If this region is "immediate rendering" then we should always render
	// Else try to avoid rendering to keep the user experience smooth...
	if (!pRender->GetImmediateRender())
	{
		// Any sort of drag prevents new cacheing (to keep drags running smoothly)
		if (GetQuickRender())
			return SUBTREE_ROOTANDCHILDREN;

		// XPE editing op can prevent new cacheing (to keep XPE edits running smoothly)
PORTNOTE("xpe", "NodeLiveEffect::RenderSubtree - removed use of XPE")
#if !defined(EXCLUDE_FROM_XARALX)
		if (!XPEHost::EnableLERendering(this))
			return SUBTREE_ROOTANDCHILDREN;							// But allow children to render
#endif
	}

	// If we couldn't find or render a cached bitmap then try to cache a new one
	// Only cache if it's worth it!
	if (CanGenerateBitmap())
	{
		DocRect CaptureRect = GetChildBoundingRect();			// Make sure we get our child bound not result of processing!
		CaptureRect.Inflate(CAPTURE_BORDER*(INT32)GetPixelWidth());

		// If child node can supply a bitmap directly then we'll have it!
		// Note that RenderAfterSubtree will only do anything if it finds this capture
		// (This is crucial because RenderCallback filtering might prevent RenderSubtree being
		//  called but it does not then also prevent RenderAfterSubtree being called for the
		//  same node)
		CaptureFlags caFlags = CaptureFlags(cfLOCKEDTRANSPARENT | cfFULLCOVERAGE | cfUNCLIP | (EnableDirectCapture() ? cfALLOWDIRECT : cfNONE));
		double dResolution = GetPixelsPerInch();
		//Capture* pCapture =
		pRender->StartCapture(this, CaptureRect, CAPTUREINFO(ctNESTABLE, caFlags), TRUE, FALSE, dResolution, GetInkNodeFromController());
//		if (pCapture && pCapture->IsDirect())
//			return SUBTREE_ROOTONLY;							// Capture gets bitmap directly from child so child does not need to be rendered
	}

	return SUBTREE_ROOTANDCHILDREN;								// We must render ourselves and our children
}

/********************************************************************************************

>	BOOL NodeLiveEffect::GetProcessedBitmap(BOOL bDirect, LPBITMAPINFO* plpInfo, LPBYTE* plpBits, DocRect* pRect)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/09/2004
	Inputs:		bDirect	- TRUE to get bounding rect in DirectBitmap space
						  FALSE to get bounding rect in Drawing space
	Outputs:	plpInfo - pointer to LPBITMAPINFO pointer to be written to for bitmap info
				plpBits - pointer to LPBYTE pointer to be written to for bitmap data
				pRect	- pointer to DocRect describing bitmap bounds
	Purpose:	-

********************************************************************************************/

BOOL NodeLiveEffect::GetProcessedBitmap(BOOL bDirect, LPBITMAPINFO* plpInfo, LPBYTE* plpBits, DocRect* pRect)
{
	CBitmapCache* pBitmapCache = Camelot.GetBitmapCache();
	if (pBitmapCache==NULL)
		return FALSE;

	CBitmapCacheKey inky(this, GetPixelWidth(), 1);						// Get cached BMP for this node at our dpi

	// Look for a cached bitmap at the appropriate pixel size...
	CCachedBitmap		cbmp;
	BOOL				bFound = pBitmapCache->Lookup(inky, cbmp);
	if (bFound)
	{
		if (plpInfo)
			*plpInfo = cbmp.pbmpInfo;
		if (plpBits)
			*plpBits = cbmp.pbmpBits;
		if (pRect)
		{
			if (bDirect)
				*pRect = m_rectDirectBitmap;
			else
				*pRect = cbmp.GetCachedRect();
		}
		return TRUE;
	}

	if (plpInfo)
		*plpInfo = NULL;
	if (plpBits)
		*plpBits = NULL;

	return FALSE;
}




/********************************************************************************************

>	DocRect NodeLiveEffect::SetProcessedBitmap(LPBITMAPINFO lpInfo, LPBYTE lpBits, DocRect rect, double xOffset, double yOffset, double dPixelWidth = 0, Matrix* pmatTransform = NULL)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/09/2004
	Inputs:		-
	Outputs:	lpInfo - reference to LPBITMAPINFO pointer for bitmap info
				lpBits - reference to LPBYTE pointer for bitmap data
	Purpose:	-

********************************************************************************************/

DocRect NodeLiveEffect::SetProcessedBitmap(LPBITMAPINFO lpInfo, LPBYTE lpBits, DocRect rect, INT32 width, INT32 height, double xOffset, double yOffset, double dPixelWidth, Matrix* pmatTransform)
{
	BOOL bWasInvalidBounds = !IsBoundingRectValid;
	CBitmapCache* pBitmapCache = Camelot.GetBitmapCache();
	if (pBitmapCache==NULL)
		return rect;

	if (dPixelWidth==0)
		dPixelWidth = GetPixelWidth();

	if (rect.IsEmpty())
	{
		rect = GetChildBoundingRect();
		rect.Inflate(CAPTURE_BORDER*(INT32)dPixelWidth);
	}

	CBitmapCacheKey inky(this, dPixelWidth, 1);						// Get cached BMP for this node at our dpi
	DocRect crect = AdjustPixelOffsets(rect, width, height, xOffset, yOffset, dPixelWidth);
	DocRect uprect = crect;

	// If we have a transform, then we must transform our coords before rendering
	DocCoord coords[3];
	coords[0] = crect.lo;
	coords[1] = DocCoord(crect.hi.x, crect.lo.y);
	coords[2] = DocCoord(crect.lo.x, crect.hi.y);
	if (pmatTransform)
		pmatTransform->transform(coords, 3);

	// Deallocate any cached DIB here
	CCachedBitmap cbmp;
	BOOL bFound = pBitmapCache->Lookup(inky, cbmp);
	if (bFound)
	{
		pBitmapCache->RemoveBitmap(inky);
		cbmp.Release();
		m_rectDirectBitmap = DocRect(0,0,0,0);
	}

	if (lpInfo!=NULL && lpBits!=NULL)
	{
		cbmp.pbmpInfo = lpInfo;
		cbmp.pbmpBits = lpBits;
//		cbmp.SetCachedRect(crect);
		cbmp.SetCachedParallelogram(coords, 3);
		cbmp.nPriority = CACHEPRIORITY_PERMANENT;

		pBitmapCache->StoreBitmap(inky, cbmp);

		m_rectDirectBitmap = crect;			// "Cache" rectangle in DirectBitmap space for GetProcessedBitmap

		uprect = cbmp.GetCachedRect();

		IsBoundingRectValid = TRUE;			// URGH! Force InvalidateBoundingRect to go up parent links!
		InvalidateBoundingRect();			// Tell our parents that our bounds have changed
		BoundingRectangle = uprect;			// But we know our bounding rectangle now
		IsBoundingRectValid = TRUE;			// And we know that it's valid

		// Store some data about the bounds while we know it
		m_rectEstimatedBounds = uprect;

		// Store max border created by this effect so we can redraw correctly after scaling
		// rect is in DirectBitmap space, crect is in Drawing space, so we must get a version of rect
		// in Drawing space before we can compare the two rects to get m_lMaxBorder measured in
		// Drawing space dimensions...
		//
		// Transform rect into Drawing space...
		DocCoord OrigCoords[4];
		OrigCoords[0] = rect.lo;
		OrigCoords[1] = DocCoord(rect.lo.x, rect.hi.y);
		OrigCoords[2] = DocCoord(rect.hi.x, rect.lo.y);
		OrigCoords[3] = rect.hi;
		if (pmatTransform)
			pmatTransform->transform(OrigCoords, 4);
		// Form upright rect enclosing transformed coords...
		DocRect OrigRect(OrigCoords[0], OrigCoords[0]);
		OrigRect.IncludePoint(OrigCoords[1]);
		OrigRect.IncludePoint(OrigCoords[2]);
		OrigRect.IncludePoint(OrigCoords[3]);
		// Compare upright original rect against new upright rect to find max offset in Drawing space...
		m_lMaxBorder = max(OrigRect.lo.x-uprect.lo.x, OrigRect.lo.y-uprect.lo.y);
		m_lMaxBorder = max(uprect.hi.x-OrigRect.hi.x, m_lMaxBorder);
		m_lMaxBorder = max(uprect.hi.y-OrigRect.hi.y, m_lMaxBorder);
		// Safety check
		if ( !Error::IsUserName("Gavin") )
			ERROR3IF(m_lMaxBorder<0, "m_lMaxBorder has become negative!");
		m_lMaxBorder = 0;

		// If the processed bitmap is bigger than the original we must ensure the bounds are updated
		// (Compare two rects in DirectBitmap space)
		if (rect!=crect || bWasInvalidBounds)
		{
			DocView* pView = DocView::GetCurrent();
			if (pView)
				pView->NotifyBoundsChanged();
		}
	}

	// Note! Leave bounds alone if setting processed bitmap to NULL in the hope they will persist...
	return uprect;
}




/********************************************************************************************

>	virtual String NodeLiveEffect::Describe(BOOL Plural, BOOL Verbose = TRUE)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/09/2004
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

String NodeLiveEffect::Describe(BOOL Plural, BOOL Verbose)
{
	if (Plural)
		return(String(_R(IDS_LIVEEFFECT_DESCRP)));
	else
		return(String(_R(IDS_LIVEEFFECT_DESCRS)));
};




/***********************************************************************************************
> Node* NodeLiveEffect::SimpleCopy()  

    Author:     Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
    Created:    09/09/2004
	Inputs:     -  
    Outputs:    
    Returns:    A copy of the node, or NULL if memory has run out 
		 
    Purpose:    This method returns a shallow copy of the node with all Node pointers NULL. 
				The function is virtual, and must be defined for all derived classes.  
	            
	Errors:     If memory runs out when trying to copy, then ERROR is called with an out of memory
				error and the function returns NULL.                                                                      
                                                                                 
**********************************************************************************************/

Node* NodeLiveEffect::SimpleCopy()
{
	NodeLiveEffect* NodeCopy; 
	NodeCopy = new NodeLiveEffect();
	ERRORIF(NodeCopy == NULL, _R(IDE_NOMORE_MEMORY), NULL); 
	CopyNodeContents(NodeCopy);   		
	return (NodeCopy);
}   



   
/***********************************************************************************************

>	void NodeLiveEffect::CopyNodeContents(Node* pCopyOfNode)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/09/2004
	Inputs:		pCopyOfNode - The node to copy data to
	Outputs:	-
	Returns:	-
	Purpose:	Copies the data from this node to pCopyOfNode by first calling the base class to get it to
				copy its stuff, and then copying its own stuff
	Scope:		protected
	SeeAlso:	NodeGroup::CopyNodeContents

***********************************************************************************************/

void NodeLiveEffect::CopyNodeContents(NodeLiveEffect* pCopyOfNode)
{
	NodeBitmapEffect::CopyNodeContents(pCopyOfNode);

	// Copy member vars here
	if (m_pEditsDoc)
	{
PORTNOTETRACE("other","NodeLiveEffect::CopyNodeContents - not copying XML data");
#ifndef EXCLUDE_FROM_XARALX
		pCopyOfNode->m_pEditsDoc = CXMLUtils::NewDocument(m_pEditsDoc);
#endif
	}
	else
		pCopyOfNode->m_pEditsDoc = NULL;
	pCopyOfNode->m_dPixelsPerInch = m_dPixelsPerInch;
	pCopyOfNode->m_strDisplayName = m_strDisplayName;
	pCopyOfNode->m_rectEstimatedBounds = m_rectEstimatedBounds;
	pCopyOfNode->m_lMaxBorder = m_lMaxBorder;
	pCopyOfNode->m_rectDirectBitmap = m_rectDirectBitmap;

	// If this node cannot remake it's associated bitmap
	// Then we need to copy the cached bitmaps into the new node
	CopyCached(pCopyOfNode, GetPixelWidth(), 1);	// Copy cached bitmaps, options 0 and 1 (original and processed)
}




/***********************************************************************************************
>   void NodeLiveEffect::PolyCopyNodeContents(NodeRenderable* pNodeCopy)

    Author:     Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
    Created:    09/09/2004
    Outputs:    -
    Purpose:    Polymorphically copies the contents of this node to another
	Errors:     An assertion failure will occur if NodeCopy is NULL
    Scope:      protected
								     
***********************************************************************************************/

void NodeLiveEffect::PolyCopyNodeContents(NodeRenderable* pNodeCopy)
{
	ENSURE(pNodeCopy, "Trying to copy a node's contents into a NULL node");
	ENSURE(IS_A(pNodeCopy, NodeLiveEffect), "PolyCopyNodeContents given wrong dest node type");

	if (IS_A(pNodeCopy, NodeLiveEffect))
		CopyNodeContents((NodeLiveEffect*)pNodeCopy);
}




/********************************************************************************************

>	virtual UINT32 NodeLiveEffect::GetNodeSize() const

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/09/2004
	Inputs:		-
	Outputs:	-
	Returns:	The size of the node in bytes
	Purpose:	For finding the size of the node 
				
	SeeAlso:	Node::GetSubtreeSize

********************************************************************************************/

UINT32 NodeLiveEffect::GetNodeSize() const 
{     
	return sizeof(NodeLiveEffect);
}  




/********************************************************************************************

>	virtual void NodeLiveEffect::Transform( TransformBase& Trans )

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/09/2004
	Inputs:		Trans - The transform Object
	Purpose:	Transforms the LiveEffect node.
	SeeAlso:	NodeRenderableInk::Transform()

********************************************************************************************/

void NodeLiveEffect::Transform( TransformBase& Trans )
{
	CBitmapCache* pBitmapCache = Camelot.GetBitmapCache();
	ERROR3IF(pBitmapCache==NULL, "Can't find BitmapCache");

	// Transform the cached bitmaps
	CCachedBitmap cbmp;
	CBitmapCacheKey inky(this, GetPixelWidth(), 0);						// Get cached BMP for this ORIGINAL node at our dpi
	BOOL bFound = pBitmapCache->Lookup(inky, cbmp);
	if (bFound)
	{
		cbmp.Transform(Trans);
		pBitmapCache->StoreBitmap(inky, cbmp);
	}

	CBitmapCacheKey inky1(this, GetPixelWidth(), 1);					// Get cached BMP for this PROCESSED node at our dpi
	bFound = pBitmapCache->Lookup(inky1, cbmp);
	if (bFound)
	{
		cbmp.Transform(Trans);
		pBitmapCache->StoreBitmap(inky1, cbmp);
	}
	else
	{
		Trans.bHaveTransformedCached = FALSE;
		Trans.bTransformYourChildren = TRUE;
	}

	// No need to transform Option 2, processed/rendered bitmap because it is only
	// an intermediate stage for producing other cached bitmaps
	// and exists outside the drawing geometry domain anyway

	// Transform the estimated bounds
	DocCoord t[3];
	t[0] = m_rectEstimatedBounds.lo;
	t[1] = DocCoord(m_rectEstimatedBounds.hi.x, m_rectEstimatedBounds.lo.y);
	t[2] = DocCoord(m_rectEstimatedBounds.lo.x, m_rectEstimatedBounds.hi.y);

	Trans.Transform(t, 3);

	m_rectEstimatedBounds = DocRect(t[0], t[0]);
	m_rectEstimatedBounds.IncludePoint(t[1]);
	m_rectEstimatedBounds.IncludePoint(t[2]);
	m_rectEstimatedBounds.IncludePoint(DocCoord(t[2].x+t[1].x-t[0].x, t[2].y+t[1].y-t[0].y));

	// Note that we don't scale the m_lMaxBorder field here so that, if m_rectEstimatedBounds
	// gets smaller, m_lMaxBorder won't and will generate a better guess at the resulting
	// bounds in GetBoundingRect

	InvalidateBoundingRect();

	// Preserve current state of transformed cache flag
	BOOL bTransCache = Trans.bHaveTransformedCached;

	// Transform all the children...
	// No need to transfom children if we are being dragged and we have successfully transformed our cached bitmap
//	if (!(IsDragged() && bFound))
	if (Trans.bTransformYourChildren)
		TransformChildren(Trans);
	else
	{
		TransformEffectAttrs(Trans);
		Trans.bHaveTransformedChildren = FALSE;
	}

	// If the current transform is a translation then we have successfully
	// transformed our cached info so don't allow our children to modify the state of
	// bHaveTransformedCached
	Trans.bHaveTransformedCached = bTransCache;
//	if (bTransCache && Trans.IsTranslation())
//	{
//		Trans.bHaveTransformedCached = TRUE;
//	} 
}




/********************************************************************************************

>	virtual BOOL NodeLiveEffect::ReleaseCached(BOOL bAndParents = TRUE,
													 BOOL bAndChildren = TRUE,
													 BOOL bSelf = TRUE,
													 BOOL bAndDerived = TRUE)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/06/2004
	Returns:	TRUE if captured data was cached
	Purpose:	Protected Helper function
				Use the CaptureManager to capture the results of rendering, cache them
				and associate them with this Ink node
	SeeAlso:	NodeRenderableInk::RenderCached, CaptureCached

********************************************************************************************/

BOOL NodeLiveEffect::ReleaseCached(BOOL bAndParents, BOOL bAndChildren, BOOL bSelf, BOOL bAndDerived)
{
	// Don't release cached data if the release request is the result of transformation
	// re-rendering because we can transform our cached data!
	if (!IsDragged())
	{
		CBitmapCache* pBitmapCache = Camelot.GetBitmapCache();
		if (pBitmapCache!=NULL && bSelf)
		{
			CBitmapCacheKey inky(this, 42);
			pBitmapCache->RemoveAllOwnedBitmaps(inky, FALSE, CACHEPRIORITY_PERMANENT);
			m_rectDirectBitmap = DocRect(0,0,0,0);
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

>	virtual BOOL NodeLiveEffect::WritePreChildrenWeb(BaseCamelotFilter* pFilter)

 	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/09/2004
	Inputs:		pFilter = ptr to the filter
	Returns:	TRUE if record is written, FALSE if not
	Purpose:	Writes the path record to the filter
	SeeAlso:	-

********************************************************************************************/

BOOL NodeLiveEffect::WritePreChildrenWeb(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT

	BOOL ok = TRUE;

	ok = WriteLiveEffect(pFilter);

	return ok;

#else
	return FALSE;
#endif
}

/********************************************************************************************

>	virtual BOOL NodeLiveEffect::WritePreChildrenNative(BaseCamelotFilter* pFilter)

 	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/09/2004
	Inputs:		pFilter = ptr to the filter
	Returns:	TRUE if record is written, FALSE if not
	Purpose:	Writes the path record to the filter
	SeeAlso:	-

********************************************************************************************/

BOOL NodeLiveEffect::WritePreChildrenNative(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT

	BOOL ok = TRUE;

	ok = WriteLiveEffect(pFilter);

	return ok;

#else
	return FALSE;
#endif
}




/********************************************************************************************

>	virtual BOOL NodeLiveEffect::WriteLiveEffect(BaseCamelotFilter* pFilter)

 	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/09/2004
	Inputs:		pFilter = ptr to the filter
	Returns:	TRUE if record is written, FALSE if not
	Purpose:	Writes the path record to the filter
	SeeAlso:	-

********************************************************************************************/

BOOL NodeLiveEffect::WriteLiveEffect(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter == NULL,FALSE,"NULL filter param");

	BOOL ok = TRUE;
	BYTE Flags = 0;

	UINT32 Tag = TAG_LIVE_EFFECT;

	CXaraFileRecord Rec(Tag);

	BSTR bstrValue;
	HRESULT hr;
	hr = m_pEditsDoc->get_xml(&bstrValue);
	ok = (SUCCEEDED(hr));
	if (ok) ok = Rec.Init();
	if (ok) ok = Rec.WriteBYTE(Flags);					// flags
	if (ok) ok = Rec.WriteDOUBLE(m_dPixelsPerInch);		// Resolution
	if (ok) ok = Rec.WriteUnicode(m_strPostProID);		// Effect ID
	if (ok) ok = Rec.WriteUnicode(m_strDisplayName);	// Display Name
	if (ok) ok = Rec.WriteBSTR(bstrValue);				// UNICODE xml string edits list

	// Write the record
	if (ok) ok = pFilter->Write(&Rec);

	return ok;
#else
	return FALSE;
#endif
}




/********************************************************************************************

>	virtual NodeRenderableInk* NodeLiveEffect::FindNodeAtPointHelper(const Spread* pSpread, const DocCoord dcPoint)

 	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/09/2004
	Inputs:		pSpread - pointer to relevant spread (should be our ancestor somewhere)
				dcPoint - point on spread under consideration
	Returns:	Pointer to node that should become selected as result of hit detection at
				this point
	Purpose:	Allows a node class to control hit detection and selection of itself
	SeeAlso:	-

********************************************************************************************/

NodeRenderableInk* NodeLiveEffect::FindNodeAtPointHelper(const Spread* pSpread, const DocCoord dcPoint)
{
	// Non-destructive LiveEffects should be invisible to selection...
	// But they do render into the hit-detection bitmap so that the user can click
	// them where they are bigger than the original object
	// So skip down the tree until we hit a "real" object
	// NOTE! We assume that a non-destructive LiveEffect only contains one renderablechild
	NodeRenderableInk* pNode = this;
	while (pNode!=NULL && pNode->IsCompoundClass() && !(pNode->CanSelectAsCompoundParent() && pNode->PromoteHitTestOnChildrenToMe()))
	{
		pNode = ((NodeCompound*)pNode)->GetInkNodeFromController();
	}

	return pNode;
}




/********************************************************************************************

>	virtual KernelBitmap* NodeLiveEffect::GetKernelBitmap(DocCoord* pCoords, BOOL bRetainCached = FALSE)

 	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/02/2005
	Inputs:		bRetainCached - TRUE to retain the cached version when making the permanent bitmap
	Outputs:	pCoords - Array of coords filled in with 3 points defining parallelogram
	Returns:	Pointer to new KernelBitmap
	Purpose:	Allows a bitmap effect to create a permanent kernel bitmap version of itself
	SeeAlso:	-

********************************************************************************************/

KernelBitmap* NodeLiveEffect::GetKernelBitmap(DocCoord* pCoords, BOOL bRetainCached)
{
	// Get bitmap out of cache (hope it's still in there!)
	CBitmapCache* pBitmapCache = Camelot.GetBitmapCache();
	CBitmapCacheKey inky(this, GetPixelWidth(), 1);						// Get cached BMP for this PROCESSED node at our dpi
	CCachedBitmap cbmp;
	BOOL bFoundCached = pBitmapCache->Lookup(inky, cbmp);
	if (!bFoundCached)
		return NULL;

	CWxBitmap		   *wBitmap		= new CWxBitmap(cbmp.pbmpInfo, cbmp.pbmpBits);
	if (wBitmap==NULL)
		return NULL;

	wBitmap->SetHidden(TRUE);											// We don't want the user to see this bitmap in the gallery
	KernelBitmap* kBitmap	= new KernelBitmap(wBitmap);
	if (kBitmap==NULL)
	{
		delete wBitmap;
		return NULL;
	}
	Document* pDoc = Document::GetCurrent();
	BitmapList* pBmpList = pDoc->GetBitmapList();
	kBitmap->Attach(pBmpList);
PORTNOTE("xpe", "NodeLiveEffect::GetKernelBitmap - removed use of XPE")
#if !defined(EXCLUDE_FROM_XARALX)
	XPEHost::GetEffectDetails(m_strPostProID, &m_strDisplayName);
#endif
	String_256 name(m_strDisplayName);
	kBitmap->SetName(name);

	if (pCoords)
	{
		pCoords[0] = cbmp.coord0;
		pCoords[1] = cbmp.coord1;
		pCoords[2] = cbmp.coord2;
	}

	if (!bRetainCached)
		pBitmapCache->RemoveBitmap(inky);

	return kBitmap;
}




/********************************************************************************************

>	void NodeLiveEffect::RemoveBitmapFromCache()

 	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/06/2005
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Remove the bitmap from the cache but don't vape the bitmap itself from memory
	SeeAlso:	-

********************************************************************************************/

void NodeLiveEffect::RemoveBitmapFromCache()
{
	CBitmapCache* pBitmapCache = Camelot.GetBitmapCache();
	CBitmapCacheKey inky(this, GetPixelWidth(), 1);			// Get cached BMP for this PROCESSED node at our dpi
	pBitmapCache->RemoveBitmap(inky);
}




/********************************************************************************************
	Class:		NodeLockedEffect

********************************************************************************************/

/***********************************************************************************************

>	NodeLockedEffect::NodeLockedEffect(Node* 	ContextNode,
						AttachNodeDirection Direction,
						const DocRect& 		BoundingRect,
						BOOL				Locked = FALSE,
						BOOL 				Mangled = FALSE,
						BOOL 				Marked = FALSE,
						BOOL 				Selected = FALSE,
						BOOL 				Renderable = FALSE
						)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/09/2004
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
NodeLockedEffect::NodeLockedEffect(Node* ContextNode,  
					AttachNodeDirection Direction,  
					BOOL Locked, 
					BOOL Mangled,  
					BOOL Marked, 
					BOOL Selected    
			  ) : NodeBitmapEffect(ContextNode, Direction, Locked, Mangled, Marked, Selected )
{
	m_PGram[0] = DocCoord(0,0);
	m_PGram[1] = DocCoord(0,0);
	m_PGram[2] = DocCoord(0,0);

	m_dPixelsPerInch = (double)NodeBitmapEffect::DefaultLockedPixelsPerInch;
	m_bIsDestructive	= TRUE;
}                        




/*********************************************************************************************

>	NodeLockedEffect::NodeLockedEffect() 

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/09/2004
	Purpose:	
	Note:		
	SeeAlso:	

**********************************************************************************************/
NodeLockedEffect::NodeLockedEffect() : NodeBitmapEffect()
{
	m_PGram[0] = DocCoord(0,0);
	m_PGram[1] = DocCoord(0,0);
	m_PGram[2] = DocCoord(0,0);

	m_dPixelsPerInch = (double)NodeBitmapEffect::DefaultLockedPixelsPerInch;
	m_bIsDestructive	= TRUE;
}




/*********************************************************************************************

>	NodeLockedEffect::~NodeLockedEffect() 

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/11/2004
	Purpose:	Destructor
	Note:		
	SeeAlso:	

**********************************************************************************************/
NodeLockedEffect::~NodeLockedEffect()
{
	m_BitmapRef.Detach();			// Detach bitmap ref
	m_pEditsDoc = NULL;				// m_pEditsDoc is a smart pointer so NULLing it Releases the COM interface
									// and releases any memory

	// Get rid of our original bitmap(s)
	CBitmapCache* pBitmapCache = Camelot.GetBitmapCache();
	if (pBitmapCache!=NULL)
	{
		CBitmapCacheKey inky(this, 42);
		pBitmapCache->RemoveAllOwnedBitmaps(inky, FALSE, CACHEPRIORITY_PERMANENT);
	}
}




/*********************************************************************************************

>	virtual void NodeLockedEffect::UnlinkNodeFromTree(BaseDocument* pOwnerDoc)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/09/2005
	Purpose:	Pre-Destructor
				Do smart tidy up of bitmap usage if we are being deleted from the tree
				rather than simply unlinked temporarilly (signalled by pOwnerDoc being
				non-null)
	Note:		
	SeeAlso:	

**********************************************************************************************/
void NodeLockedEffect::UnlinkNodeFromTree(BaseDocument* pOwnerDoc)
{
	KernelBitmap* pOldBitmap = m_BitmapRef.GetBitmap();
	if (pOldBitmap && pOwnerDoc)
	{
		m_BitmapRef.Detach();		// Detach so that Enumerate function below returns NULL and OilBitmap::IsUsedInDocument operates correctly

		BitmapList* pBmpList = pOldBitmap->GetParentBitmapList();
		if (pBmpList)
		{
			if (pOwnerDoc && pOwnerDoc->IsKindOf(CC_RUNTIME_CLASS(Document)))
			{
				Document* pBitmapDoc = (Document*)pOwnerDoc;
				if (pBitmapDoc && !pOldBitmap->IsUsedInDocument(pBitmapDoc, TRUE))
				{
					pOldBitmap->Detach();
					BROADCAST_TO_ALL(BitmapListChangedMsg(pBmpList, pOldBitmap));
					delete pOldBitmap;

					// Free up any unused bitmaps in the global list
					// (just deleting the KernelBitmaps doesn't seem to do it)
					Camelot.GetGlobalBitmapList()->DeleteAllUnusedBitmaps();
				}
			}
		}
	}

	// Call base class
	NodeBitmapEffect::UnlinkNodeFromTree(pOwnerDoc);
}




/********************************************************************************************

>	KernelBitmap* NodeLockedEffect::EnumerateBitmaps(UINT32 Count)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/06/2005
	Inputs:		Count - the bitmap to get (see Purpose).
	Returns:	The KernelBitmap in use by the node, or NULL if no more are used.
	Purpose:	Find out what bitmaps, if any, are used by this node.

				The base class returns NULL always, so you over-ride this in any node classes
				that use bitmaps.

				This function supports nodes that use more than one bitmap - you call this
				function repeatedly and keep incrementing the Count parameter that you pass
				in each time by 1.  You should stop calling it when it returns NULL, as this
				indicates that no more bitmaps are used by this node.
				Count should start off as 0 for the first call.  Note that this function
				can (and often will) return NULL for the first call, as many nodes don't
				use bitmaps, obviously.

	SeeAlso:	KernelBitmap, NodeBitmap

********************************************************************************************/

KernelBitmap* NodeLockedEffect::EnumerateBitmaps(UINT32 Count)
{
	if (Count == 0) return m_BitmapRef.GetBitmap();

	return NULL;
}



/********************************************************************************************

>	DocRect NodeLockedEffect::GetBoundingRect(BOOL DontUseAttrs, BOOL HitTest)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/09/2004
	Returns:	The bounding box of the live effect
	Purpose:	Get the bounding rect of the cached processed bitmap
				or the original bitmap if the processed isn't set yet...
				or the children if neither is set yet...

********************************************************************************************/

DocRect NodeLockedEffect::GetBoundingRect(BOOL DontUseAttrs, BOOL HitTest)
{
	// Let sanity prevail!
	BOOL bIncludeAttrs = !DontUseAttrs;

	// Optimise this function by returning fast cached info if we know it's valid.
	if (IsBoundingRectValid && bIncludeAttrs)
		return BoundingRectangle;

	// Else work it out by looking up the cached bitmaps
	// and failing that by scanning our children
	if (bIncludeAttrs)
	{
		// We can check the bounds of our whacky effects bitmap
		if (m_BitmapRef.GetBitmap()!=NULL)
		{
			DocRect r(m_PGram[0], m_PGram[0]);
			r.IncludePoint(m_PGram[1]);
			r.IncludePoint(m_PGram[2]);
			r.IncludePoint(DocCoord(m_PGram[2].x+m_PGram[1].x-m_PGram[0].x, m_PGram[2].y+m_PGram[1].y-m_PGram[0].y));
			IsBoundingRectValid = TRUE;
			BoundingRectangle = r;
			return r;
		}
	}

	DocRect brect = GetChildBoundingRect(bIncludeAttrs);				// Note - don't set IsBoundingRectValid yet

	// Expand bounds to account for as yet unknown size increases that effects might apply
	if (bIncludeAttrs)
		brect.Inflate((INT32)(GetPixelWidth()*50+0.5));

	return brect;
}




/********************************************************************************************

>	SubtreeRenderState NodeLockedEffect::RenderSubtree(RenderRegion* pRender, Node** ppNextNode, BOOL bClip)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/09/2004
	Inputs:		pRender - The region render into (or use as context for decisions about subtree)
				ppNextNode - Address of node pointer for next node to render or run to after this
				bClip - flag indicating whether to clip or not
	Purpose:	Do clever stuff on the way into a subtree, possibly modifying rendering
				behaviour.

********************************************************************************************/

SubtreeRenderState NodeLockedEffect::RenderSubtree(RenderRegion* pRender, Node** ppNextNode, BOOL bClip)
{
//	BOOL bRendered = FALSE;

	if (pRender == NULL)							// If no render region supplied, assume we need to be rendered
		return SUBTREE_ROOTANDCHILDREN;

	DocRect BoundingRect = GetBoundingRect();
	if (!BoundingRect.IsValid())
		return SUBTREE_ROOTANDCHILDREN;

	DocRect ClipRect = pRender->GetClipRect();
	if (m_BitmapRef.GetBitmap()!=NULL)
	{
		// Go find out about my bounding rectangle
		// If we don't have a cached bitmap then expand the bounding rect to ensure it will cover an expansion caused by the effect
		// If we do have a cached bitmap then use the bounding rect stored with the processed bitmap

		if (bClip && !ClipRect.IsIntersectedWith(BoundingRect))	// If not within the clipping rect then
			return SUBTREE_NORENDER;					// Don't render us or our children

		KernelBitmap* pkBitmap = m_BitmapRef.GetBitmap();
		if (pkBitmap==NULL || pkBitmap->HasBeenDeleted())
			return SUBTREE_ROOTANDCHILDREN;

#ifdef _DEBUG
		BOOL bRendered = RenderCachedEffect(NULL, pRender);
		ERROR3IF(!bRendered, "How can something so simple fail to render?");
#else
		RenderCachedEffect(NULL, pRender);
#endif

		return SUBTREE_NORENDER;
	}

	BoundingRect.Inflate((INT32)(GetPixelWidth()*150));						// Bodge to ensure LE is rendered before it grows
	if (bClip && !ClipRect.IsIntersectedWith(BoundingRect))	// If not within the clipping rect then
		return SUBTREE_NORENDER;					// Don't render us or our children

	// If we couldn't find or render a cached bitmap then try to cache a new one
//	double PixelWidth = GetPixelWidth();

	// Work out how much of the object we propose to capture
	// (This may change during the Capture if we have to fall back to 24BPP opaque capture)
	DocRect viewrect = ClipRect;
	DocRect CaptureRect = GetChildBoundingRect();			// Make sure we get our child bound not result of processing!

	// Only cache if it's worth it!
	if (CanGenerateBitmap())
	{
		// It's quicker to scan ahead looking for non-mix transparency than to find it
		// half way through rendering a bitmap...
		//
		// Note that RenderAfterSubtree will only do anything if it finds this capture
		// (This is crucial because RenderCallback filtering might prevent RenderSubtree being
		//  called but it does not then also prevent RenderAfterSubtree being called for the
		//  same node)
		DocRect CaptureRect = GetChildBoundingRect();	// Make sure we get our child bound not result of processing!
		CaptureRect.Inflate(CAPTURE_BORDER*(INT32)GetPixelWidth());
		CaptureFlags caFlags = CaptureFlags(cfLOCKEDTRANSPARENT | cfFULLCOVERAGE | cfUNCLIP | (EnableDirectCapture() ? cfALLOWDIRECT : cfNONE));
		double dResolution = GetPixelsPerInch();
		//Capture* pCapture =
		pRender->StartCapture(this, CaptureRect, CAPTUREINFO(ctNESTABLE, caFlags), TRUE, FALSE, dResolution, GetInkNodeFromController());
//		if (pCapture && pCapture->IsDirect())
//			return SUBTREE_ROOTONLY;				// Capture gets bitmap directly from child so child does not need to be rendered
	} 

	return SUBTREE_ROOTANDCHILDREN;					// We must render ourselves and our children
}




/***********************************************************************************************

>	virtual void NodeLockedEffect::Render(RenderRegion* pRRegion)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/12/2004
	Inputs:		Pointer to a render region
	Purpose:	Will render the liveeffect iff we have it cached - for hit detection reasons

***********************************************************************************************/

void NodeLockedEffect::Render(RenderRegion* pRender)
{
	if (m_BitmapRef.GetBitmap()!=NULL && pRender->IsHitDetect())
	{
		CWxBitmap* pWBitmap = (CWxBitmap*)m_BitmapRef.GetBitmap()->GetActualBitmap();
		if (pWBitmap==NULL || pWBitmap->HasBeenDeleted())
			return;

		LPBITMAPINFO lpInfo = pWBitmap->BMInfo;
		LPBYTE lpBits = pWBitmap->BMBytes;
#ifdef _DEBUG
		BOOL bRendered = pRender->RenderBits(lpInfo, lpBits, m_PGram, 3, TRUE, this);
		ERROR3IF(!bRendered, "How can something so simple fail to render?");
#else
		pRender->RenderBits(lpInfo, lpBits, m_PGram, 3, TRUE, this);
#endif
	}
}



/********************************************************************************************

>	BOOL NodeLockedEffect::GetProcessedBitmap(BOOL bDirect, LPBITMAPINFO* plpInfo, LPBYTE* plpBits, DocRect* pRect)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/09/2004
	Inputs:		-
	Outputs:	plpInfo - pointer to LPBITMAPINFO pointer to be written to for bitmap info
				plpBits - pointer to LPBYTE pointer to be written to for bitmap data
	Purpose:	-

********************************************************************************************/

BOOL NodeLockedEffect::GetProcessedBitmap(BOOL bDirect, LPBITMAPINFO* plpInfo, LPBYTE* plpBits, DocRect* pRect)
{
	if (plpInfo)	*plpInfo = NULL;
	if (plpBits)	*plpBits = NULL;

	if (m_BitmapRef.GetBitmap()!=NULL)
	{
		CWxBitmap* pWBitmap = (CWxBitmap*)m_BitmapRef.GetBitmap()->GetActualBitmap();
		if (pWBitmap!=NULL && !pWBitmap->HasBeenDeleted())
		{
			if (plpInfo)	*plpInfo = pWBitmap->BMInfo;
			if (plpBits)	*plpBits = pWBitmap->BMBytes;
			if (pRect)
			{
				if (bDirect)
					*pRect = m_rectDirectBitmap;
				else
				{
					DocRect uprect = DocRect(m_PGram[0], m_PGram[0]);
					uprect.IncludePoint(m_PGram[1]);
					uprect.IncludePoint(m_PGram[2]);
					uprect.IncludePoint(DocCoord(m_PGram[2].x+m_PGram[1].x-m_PGram[0].x, m_PGram[2].y+m_PGram[1].y-m_PGram[0].y));
					*pRect = uprect;
				}
			}

			return TRUE;
		}
	}

	return FALSE;
}

/********************************************************************************************

>	DocRect NodeLockedEffect::SetProcessedBitmap(LPBITMAPINFO lpInfo, LPBYTE lpBits, DocRect rect, double xOffset, double yOffset, double dPixelWidth = 0, Matrix* pmatTransform = NULL)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/09/2004
	Inputs:		-
	Outputs:	lpInfo - reference to LPBITMAPINFO pointer for bitmap info
				lpBits - reference to LPBYTE pointer for bitmap data
	Purpose:	-

********************************************************************************************/

DocRect NodeLockedEffect::SetProcessedBitmap(LPBITMAPINFO lpInfo, LPBYTE lpBits, DocRect rect, INT32 width, INT32 height, double xOffset, double yOffset, double dPixelWidth, Matrix* pmatTransform)
{
	BOOL bWasInvalidBounds = !IsBoundingRectValid;

	if (dPixelWidth==0)
		dPixelWidth = GetPixelWidth();

	if (rect.IsEmpty())
	{
		rect = GetChildBoundingRect();
		rect.Inflate(CAPTURE_BORDER*(INT32)dPixelWidth);
	}

	DocRect crect = AdjustPixelOffsets(rect, width, height, xOffset, yOffset, dPixelWidth);
	DocRect uprect = crect;

	// If we have a transform, then we must transform our coords before rendering
	DocCoord coords[3];
	coords[0] = crect.lo;
	coords[1] = DocCoord(crect.hi.x, crect.lo.y);
	coords[2] = DocCoord(crect.lo.x, crect.hi.y);

	if (pmatTransform)
	{
		pmatTransform->transform(coords, 3);

		// Get new upright bounding rect for transformed coords
		uprect = DocRect(coords[0], coords[0]);
		uprect.IncludePoint(coords[1]);
		uprect.IncludePoint(coords[2]);
		uprect.IncludePoint(DocCoord(coords[2].x+coords[1].x-coords[0].x, coords[2].y+coords[1].y-coords[0].y));
	}

	// Store locked bitmaps using the old style Kernel/OilBitmap technology
	// so that they are permanent and can be loaded and saved more easily
	m_BitmapRef.Detach();
	m_rectDirectBitmap = DocRect(0,0,0,0);

	if (lpInfo && lpBits)
	{
		// Create a Kernel bitmap from the bmp data.
		CWxBitmap* wBitmap		= new CWxBitmap(lpInfo, lpBits);
		if (wBitmap==NULL)
			return uprect;
		wBitmap->SetHidden(TRUE);			// We don't want the user to see this bitmap in the gallery
		KernelBitmap* kBitmap	= new KernelBitmap(wBitmap);
		if (kBitmap==NULL)
		{
			delete wBitmap;
			return uprect;
		}

		m_rectDirectBitmap = crect;

		Document* pDoc = Document::GetCurrent();
		BitmapList* pBmpList = pDoc->GetBitmapList();
		kBitmap->Attach(pBmpList);
PORTNOTE("xpe", "NodeLockedEffect::SetProcessedBitmap - removed use of XPE")
#if !defined(EXCLUDE_FROM_XARALX)
		XPEHost::GetEffectDetails(m_strPostProID, &m_strDisplayName);
#endif
		String_256 strName(m_strDisplayName);
		kBitmap->SetName(strName);
		m_BitmapRef.Attach(kBitmap);

		IsBoundingRectValid = TRUE;			// URGH! Force InvalidateBoundingRect to go up parent links!
		InvalidateBoundingRect();			// Tell our parents that our bounds have changed
		BoundingRectangle = uprect;			// But we know our bounding rectangle now
		m_PGram[0] = coords[0];
		m_PGram[1] = coords[1];
		m_PGram[2] = coords[2];
		IsBoundingRectValid = TRUE;			// And we know that it's valid

		// If the processed bitmap is bigger than the original we must ensure the bounds are updated
		if (rect!=uprect || bWasInvalidBounds)
		{
			DocView* pView = DocView::GetCurrent();
			if (pView)
				pView->NotifyBoundsChanged();
		}
	}

	// Note! Leave bounds alone if not setting processed bitmap
	return uprect;
}




/********************************************************************************************

>	BOOL NodeLockedEffect::RenderCachedEffect(CBitmapCache* pBitmapCache, RenderRegion* pRender, BOOL bIgnoreCapture = FALSE)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	01/08/2005
	Inputs:		-
	Outputs:	DocRect - rectangle of Processed bitmap in DirectBitmap space
	Returns:	TRUE if a rectangle was obtained successfully
				FALSE otherwise
	Purpose:	-

********************************************************************************************/

BOOL NodeLockedEffect::RenderCachedEffect(CBitmapCache* pBitmapCache, RenderRegion* pRender, BOOL bIgnoreCapture)
{
	BOOL bRendered = FALSE;

	// No need to render if we are going to supply to a capture
	Capture* pCapture = pRender->GetTopCapture();
	if (!bIgnoreCapture && pCapture && pCapture->ConsumeDirectBitmap(this))
	{
		// Tell the caller that the effect has been "rendered"
		bRendered = TRUE;
	}
	else
	{
		CWxBitmap* pWBitmap = (CWxBitmap*)m_BitmapRef.GetBitmap()->GetActualBitmap();
		if (pWBitmap==NULL || pWBitmap->HasBeenDeleted())
			return FALSE;

		LPBITMAPINFO lpInfo = pWBitmap->BMInfo;
		LPBYTE lpBits = pWBitmap->BMBytes;
		bRendered = pRender->RenderBits(lpInfo, lpBits, m_PGram, 3, TRUE, this);
		ENSURE(bRendered, "How can something so simple fail to render?");
	}

	return bRendered;
}




/********************************************************************************************

>	virtual String NodeLockedEffect::Describe(BOOL Plural, BOOL Verbose = TRUE)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/11/2004
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

String NodeLockedEffect::Describe(BOOL Plural, BOOL Verbose)
{
	if (Plural)
		return(String(_R(IDS_DLIVEEFFECT_DESCRP)));
	else
		return(String(_R(IDS_DLIVEEFFECT_DESCRS)));
};




/***********************************************************************************************
> Node* NodeLockedEffect::SimpleCopy()  

    Author:     Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
    Created:    09/09/2004
	Inputs:     -  
    Outputs:    
    Returns:    A copy of the node, or NULL if memory has run out 
		 
    Purpose:    This method returns a shallow copy of the node with all Node pointers NULL. 
				The function is virtual, and must be defined for all derived classes.  
	            
	Errors:     If memory runs out when trying to copy, then ERROR is called with an out of memory
				error and the function returns NULL.                                                                      
                                                                                 
**********************************************************************************************/

Node* NodeLockedEffect::SimpleCopy()
{
	NodeLockedEffect* NodeCopy; 
	NodeCopy = new NodeLockedEffect();
	ERRORIF(NodeCopy == NULL, _R(IDE_NOMORE_MEMORY), NULL); 
	CopyNodeContents(NodeCopy);   		
	return (NodeCopy);
}   



   
/***********************************************************************************************

>	void NodeLockedEffect::CopyNodeContents(Node* pCopyOfNode)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/09/2004
	Inputs:		pCopyOfNode - The node to copy data to
	Outputs:	-
	Returns:	-
	Purpose:	Copies the data from this node to pCopyOfNode by first calling the base class to get it to
				copy its stuff, and then copying its own stuff
	Scope:		protected
	SeeAlso:	NodeGroup::CopyNodeContents

***********************************************************************************************/

void NodeLockedEffect::CopyNodeContents(NodeLockedEffect* pCopyOfNode)
{
	NodeBitmapEffect::CopyNodeContents(pCopyOfNode);

	// Copy member vars here
PORTNOTE("other","NodeLockedEffect::CopyNodeContents - Removed CXMLUtils usage")
#ifndef EXCLUDE_FROM_XARALX
	pCopyOfNode->m_pEditsDoc = CXMLUtils::NewDocument(m_pEditsDoc);
#endif
	pCopyOfNode->m_dPixelsPerInch = m_dPixelsPerInch;
	pCopyOfNode->m_strDisplayName = m_strDisplayName;
	pCopyOfNode->m_rectDirectBitmap = m_rectDirectBitmap;

	pCopyOfNode->m_BitmapRef = m_BitmapRef;
	pCopyOfNode->m_PGram[0] = m_PGram[0];
	pCopyOfNode->m_PGram[1] = m_PGram[1];
	pCopyOfNode->m_PGram[2] = m_PGram[2];
	pCopyOfNode->m_bIsDestructive = m_bIsDestructive;
}




/***********************************************************************************************
>   void NodeLiveEffect::PolyCopyNodeContents(NodeRenderable* pNodeCopy)

    Author:     Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
    Created:    09/09/2004
    Outputs:    -
    Purpose:    Polymorphically copies the contents of this node to another
	Errors:     An assertion failure will occur if NodeCopy is NULL
    Scope:      protected
								     
***********************************************************************************************/

void NodeLockedEffect::PolyCopyNodeContents(NodeRenderable* pNodeCopy)
{
	ENSURE(pNodeCopy, "Trying to copy a node's contents into a NULL node");
	ENSURE(IS_A(pNodeCopy, NodeLockedEffect), "PolyCopyNodeContents given wrong dest node type");

	if (IS_A(pNodeCopy, NodeLockedEffect))
		CopyNodeContents((NodeLockedEffect*)pNodeCopy);
}




/********************************************************************************************

>	virtual UINT32 NodeLockedEffect::GetNodeSize() const

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/09/2004
	Inputs:		-
	Outputs:	-
	Returns:	The size of the node in bytes
	Purpose:	For finding the size of the node 
				
	SeeAlso:	Node::GetSubtreeSize

********************************************************************************************/

UINT32 NodeLockedEffect::GetNodeSize() const 
{     
	UINT32 Size = sizeof(NodeLockedEffect);

	// Nasty cast here to avoid const-ness of this pointer
	KernelBitmap* pBitmap = ((NodeLockedEffect*)this)->m_BitmapRef.GetBitmap();
	if (pBitmap && pBitmap->ActualBitmap)
	{
		Size += pBitmap->ActualBitmap->GetBitmapSize();
	}

	return Size;
}  




/********************************************************************************************

>	virtual void NodeLockedEffect::Transform( TransformBase& Trans )

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/09/2004
	Inputs:		Trans - The transform Object
	Purpose:	Transforms the LiveEffect node.
	SeeAlso:	NodeRenderableInk::Transform()

********************************************************************************************/

void NodeLockedEffect::Transform( TransformBase& Trans )
{
	// Transform the DocCoords of the parallelogram
	Trans.Transform(m_PGram, 3);

	InvalidateBoundingRect();

	// Transform all the children...
	if (Trans.bTransformYourChildren)
		TransformChildren(Trans);
	else
		TransformEffectAttrs(Trans);
}




/********************************************************************************************

>	virtual BOOL NodeLockedEffect::WritePreChildrenWeb(BaseCamelotFilter* pFilter)

 	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/09/2004
	Inputs:		pFilter = ptr to the filter
	Returns:	TRUE if record is written, FALSE if not
	Purpose:	Writes the path record to the filter
	SeeAlso:	-

********************************************************************************************/

BOOL NodeLockedEffect::WritePreChildrenWeb(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT

	BOOL ok = TRUE;

	ok = WriteLockedEffect(pFilter);

	return ok;

#else
	return FALSE;
#endif
}

/********************************************************************************************

>	virtual BOOL NodeLockedEffect::WritePreChildrenNative(BaseCamelotFilter* pFilter)

 	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/09/2004
	Inputs:		pFilter = ptr to the filter
	Returns:	TRUE if record is written, FALSE if not
	Purpose:	Writes the path record to the filter
	SeeAlso:	-

********************************************************************************************/

BOOL NodeLockedEffect::WritePreChildrenNative(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT

	BOOL ok = TRUE;

	ok = WriteLockedEffect(pFilter);

	return ok;

#else
	return FALSE;
#endif
}




/********************************************************************************************

>	virtual BOOL NodeLockedEffect::WriteLockedEffect(BaseCamelotFilter* pFilter)

 	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/09/2004
	Inputs:		pFilter = ptr to the filter
	Returns:	TRUE if record is written, FALSE if not
	Purpose:	Writes the path record to the filter
	SeeAlso:	-

********************************************************************************************/

BOOL NodeLockedEffect::WriteLockedEffect(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter == NULL,FALSE,"NULL filter param");

	BOOL ok = TRUE;
	BYTE Flags = 0;
	if (!m_bIsDestructive)		Flags |= 0x02;			// Flag set when NOT destructive for back-compatibility

	// Find the kernel bmp
	KernelBitmap* pBmp = m_BitmapRef.GetBitmap();
	ERROR2IF(pBmp == NULL, FALSE, "NULL kernel bitmap in WriteLockedEffect");

	// Must write out the bitmap first
	INT32 BitmapRecordRef = pFilter->WriteRecord(pBmp);
	ERROR2IF(BitmapRecordRef == 0, FALSE, "Bitmap ref is zero in WriteLockedEffect");

	// Is the bmp reference ok?
	if (ok) ok = (BitmapRecordRef != 0);
		
	UINT32 Tag = TAG_LOCKED_EFFECT;

	CXaraFileRecord Rec(Tag);

	BSTR bstrValue;
	HRESULT hr;
	hr = m_pEditsDoc->get_xml(&bstrValue);
	ok = (SUCCEEDED(hr));
	if (ok) ok = Rec.Init();
	if (ok) ok = Rec.WriteBYTE(Flags);					// flags
	if (ok) ok = Rec.WriteDOUBLE(m_dPixelsPerInch);		// Resolution
	if (ok) ok = Rec.WriteReference(BitmapRecordRef);	// Bitmap Record Reference
	if (ok) ok = Rec.WriteCoordTrans(m_PGram[0], 0, 0);	// Coords
	if (ok) ok = Rec.WriteCoordTrans(m_PGram[1], 0, 0);
	if (ok) ok = Rec.WriteCoordTrans(m_PGram[2], 0, 0);
	if (ok) ok = Rec.WriteUnicode(m_strPostProID);		// Effect ID
	if (ok) ok = Rec.WriteUnicode(m_strDisplayName);	// Display Name
	if (ok) ok = Rec.WriteBSTR(bstrValue);				// UNICODE xml string edits list

	// Write the record
	if (ok) ok = pFilter->Write(&Rec);

	return ok;
#else
	return FALSE;
#endif
}




/********************************************************************************************

>	virtual KernelBitmap* NodeLockedEffect::GetKernelBitmap(DocCoord* pCoords, BOOL bRetainCached = FALSE)

 	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/02/2005
	Inputs:		bRetainCached - TRUE to retain the cached version when making the permanent bitmap
	Outputs:	pCoords - Array of coords filled in with 3 points defining parallelogram
	Returns:	Pointer to new KernelBitmap
	Purpose:	Allows a bitmap effect to create a permanent kernel bitmap version of itself
	SeeAlso:	-

********************************************************************************************/

KernelBitmap* NodeLockedEffect::GetKernelBitmap(DocCoord* pCoords, BOOL bRetainCached)
{
	KernelBitmap* kBitmap = m_BitmapRef.GetBitmap();

	if (pCoords)
	{
		pCoords[0] = m_PGram[0];
		pCoords[1] = m_PGram[1];
		pCoords[2] = m_PGram[2];
	}

	return kBitmap;
}




/********************************************************************************************

>	void NodeLockedEffect::RenderObjectBlobs(RenderRegion* pRender)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	??/1/2005
	Inputs:		pRender - the region to render the blobs to
	Purpose:	Renders the Object blobs for a Node Shape
	SeeAlso:	BlobManager

********************************************************************************************/

void NodeLockedEffect::RenderObjectBlobs(RenderRegion* pRender)
{
#if !defined(EXCLUDE_FROM_RALPH)
	ERROR3IF(!IsSelected(), "Who's asking us to render blobs when we're not selected?");
	// Set the line colours etc as we need them
	DocRect r = GetBoundingRect();
	pRender->SetLineColour(COLOUR_NONE);
	pRender->SetFillColour(COLOUR_UNSELECTEDBLOB);

	pRender->DrawBlob(r.lo, BT_UNSELECTED);
	pRender->DrawBlob(DocCoord(r.hi.x, r.lo.y), BT_UNSELECTED);
	pRender->DrawBlob(r.hi, BT_UNSELECTED);
	pRender->DrawBlob(DocCoord(r.lo.x, r.hi.y), BT_UNSELECTED);
#endif
}



/********************************************************************************************

>	void NodeLockedEffect::RenderTinyBlobs(RenderRegion* pRender)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	??/1/2005
	Inputs:		pRender - the region to render the blobs to
	Purpose:	Renders the Tiny blobs for a Node Shape
	SeeAlso:	BlobManager

********************************************************************************************/

void NodeLockedEffect::RenderTinyBlobs(RenderRegion* pRender)
{
#if !defined(EXCLUDE_FROM_RALPH)
//	ERROR3IF(!IsSelected(), "Who's asking us to render blobs when we're not selected?");
	// Set the line colours etc as we need them
	DocRect r = GetBoundingRect();
	pRender->SetLineColour(COLOUR_NONE);
	pRender->SetFillColour(COLOUR_UNSELECTEDBLOB);

	pRender->DrawBlob(r.hi, BT_UNSELECTED);
#endif
}




/********************************************************************************************

>	virtual NodeRenderableInk* NodeLockedEffect::GetInkNodeFromController() const

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	06/08/2005
	Inputs:		-
	Outputs:	-
	Returns:	NodeRenderableInk* - Pointer to the single child ink node that this effect
				derives all its input from (if there is just one child ink node)
				Otherise NULL
	Purpose:	Find next node down in the effects stack

********************************************************************************************/

NodeRenderableInk* NodeLockedEffect::GetInkNodeFromController() const
{
	NodeRenderableInk* pNode = FindFirstChildInk();
	if (pNode == FindLastChildInk())
		return pNode;

	return NULL;
}








#ifdef FEATHER_EFFECT
/********************************************************************************************
	Class:		NodeFeatherEffect

********************************************************************************************/

/***********************************************************************************************

>	NodeFeatherEffect::NodeFeatherEffect(Node* 	ContextNode,
						AttachNodeDirection Direction,
						const DocRect& 		BoundingRect,
						BOOL				Locked = FALSE,
						BOOL 				Mangled = FALSE,
						BOOL 				Marked = FALSE,
						BOOL 				Selected = FALSE,
						BOOL 				Renderable = FALSE
						)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/11/2004
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
NodeFeatherEffect::NodeFeatherEffect(Node* ContextNode,  
					AttachNodeDirection Direction,  
					BOOL Locked, 
					BOOL Mangled,  
					BOOL Marked, 
					BOOL Selected    
			  ) : NodeLiveEffect(ContextNode, Direction, Locked, Mangled, Marked, Selected )
{                         
//	m_Profile = CProfileBiasGain()				// Defaults to this
	m_FeatherSize = 0;							// Document base Feather has 0 feather size.
												// The doc base feather is created when camelot
												// initialises a new document.
												// The default feather applied to each new object
												// is also setup by this constructor.
}                        
 



/*********************************************************************************************

>	NodeFeatherEffect::NodeFeatherEffect() 

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/11/2004
	Purpose:	
	Note:		
	SeeAlso:	

**********************************************************************************************/
NodeFeatherEffect::NodeFeatherEffect() : NodeLiveEffect()
{
//	m_Profile = CProfileBiasGain()				// Defaults to this
	m_FeatherSize = 0;							// Document base Feather has 0 feather size.
												// The doc base feather is created when camelot
												// initialises a new document.
												// The default feather applied to each new object
												// is also setup by this constructor.
}




/*********************************************************************************************

>	NodeFeatherEffect::~NodeFeatherEffect() 

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/11/2004
	Purpose:	Destructor
	Note:		
	SeeAlso:	

**********************************************************************************************/
NodeFeatherEffect::~NodeFeatherEffect()
{
	CBitmapCache* pBitmapCache = Camelot.GetBitmapCache();
	if (pBitmapCache!=NULL)
	{
		CBitmapCacheKey inky(this, 42);
		pBitmapCache->RemoveAllOwnedBitmaps(inky, FALSE, CACHEPRIORITY_PERMANENT);
	}
}




/********************************************************************************************

>	virtual String NodeFeatherEffect::Describe(BOOL Plural, BOOL Verbose = TRUE)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/11/2004
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

String NodeFeatherEffect::Describe(BOOL Plural, BOOL Verbose)
{
	if (Plural)
		return(String(_R(IDS_FEATHEREFFECT_DESCRP)));
	else
		return(String(_R(IDS_FEATHEREFFECT_DESCRS)));
};




/***********************************************************************************************
> Node* NodeFeatherEffect::SimpleCopy()  

    Author:     Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
    Created:    24/11/2004
	Inputs:     -  
    Outputs:    
    Returns:    A copy of the node, or NULL if memory has run out 
		 
    Purpose:    This method returns a shallow copy of the node with all Node pointers NULL. 
				The function is virtual, and must be defined for all derived classes.  
	            
	Errors:     If memory runs out when trying to copy, then ERROR is called with an out of memory
				error and the function returns NULL.                                                                      
                                                                                 
**********************************************************************************************/

Node* NodeFeatherEffect::SimpleCopy()
{
	NodeFeatherEffect* NodeCopy; 
	NodeCopy = new NodeFeatherEffect();
	ERRORIF(NodeCopy == NULL, _R(IDE_NOMORE_MEMORY), NULL); 
	CopyNodeContents(NodeCopy);   		
	return (NodeCopy);
}   



   
/***********************************************************************************************

>	void NodeFeatherEffect::CopyNodeContents(Node* pCopyOfNode)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/11/2004
	Inputs:		pCopyOfNode - The node to copy data to
	Outputs:	-
	Returns:	-
	Purpose:	Copies the data from this node to pCopyOfNode by first calling the base class to get it to
				copy its stuff, and then copying its own stuff
	Scope:		protected
	SeeAlso:	NodeGroup::CopyNodeContents

***********************************************************************************************/

void NodeFeatherEffect::CopyNodeContents(NodeFeatherEffect* pCopyOfNode)
{
	NodeLiveEffect::CopyNodeContents(pCopyOfNode);

	// Copy member vars here
	pCopyOfNode->m_FeatherSize = m_FeatherSize;
	pCopyOfNode->m_Profile = m_Profile;
}




/***********************************************************************************************
>   void NodeLiveEffect::PolyCopyNodeContents(NodeRenderable* pNodeCopy)

    Author:     Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
    Created:    09/09/2004
    Outputs:    -
    Purpose:    Polymorphically copies the contents of this node to another
	Errors:     An assertion failure will occur if NodeCopy is NULL
    Scope:      protected
								     
***********************************************************************************************/

void NodeFeatherEffect::PolyCopyNodeContents(NodeRenderable* pNodeCopy)
{
	ENSURE(pNodeCopy, "Trying to copy a node's contents into a NULL node");
	ENSURE(IS_A(pNodeCopy, NodeFeatherEffect), "PolyCopyNodeContents given wrong dest node type");

	if (IS_A(pNodeCopy, NodeFeatherEffect))
		CopyNodeContents((NodeFeatherEffect*)pNodeCopy);
}




/********************************************************************************************

>	virtual UINT32 NodeFeatherEffect::GetNodeSize() const

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/11/2004
	Inputs:		-
	Outputs:	-
	Returns:	The size of the node in bytes
	Purpose:	For finding the size of the node 
				
	SeeAlso:	Node::GetSubtreeSize

********************************************************************************************/

UINT32 NodeFeatherEffect::GetNodeSize() const 
{     
	return sizeof(NodeFeatherEffect);
}  




/********************************************************************************************

>	virtual BOOL NodeFeatherEffect::WritePreChildrenWeb(BaseCamelotFilter* pFilter)

 	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/09/2004
	Inputs:		pFilter = ptr to the filter
	Returns:	TRUE if record is written, FALSE if not
	Purpose:	Writes the path record to the filter
	SeeAlso:	-

********************************************************************************************/

BOOL NodeFeatherEffect::WritePreChildrenWeb(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT

	BOOL ok = TRUE;

	ok = WriteFeatherEffect(pFilter);

	return ok;

#else
	return FALSE;
#endif
}

/********************************************************************************************

>	virtual BOOL NodeFeatherEffect::WritePreChildrenNative(BaseCamelotFilter* pFilter)

 	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/09/2004
	Inputs:		pFilter = ptr to the filter
	Returns:	TRUE if record is written, FALSE if not
	Purpose:	Writes the path record to the filter
	SeeAlso:	-

********************************************************************************************/

BOOL NodeFeatherEffect::WritePreChildrenNative(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT

	BOOL ok = TRUE;

	ok = WriteFeatherEffect(pFilter);

	return ok;

#else
	return FALSE;
#endif
}




/********************************************************************************************

>	virtual BOOL NodeFeatherEffect::WriteFeatherEffect(BaseCamelotFilter* pFilter)

 	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/11/2004
	Inputs:		pFilter = ptr to the filter
	Returns:	TRUE if record is written, FALSE if not
	Purpose:	Writes the path record to the filter
	SeeAlso:	-

********************************************************************************************/

BOOL NodeFeatherEffect::WriteFeatherEffect(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter == NULL,FALSE,"NULL filter param");

	BOOL ok = TRUE;
	BYTE Flags = 0;

	UINT32 Tag = TAG_FEATHER_EFFECT;

	CXaraFileRecord Rec(Tag);

	if (ok) ok = Rec.Init();
	if (ok) ok = Rec.WriteBYTE(Flags);					// flags
	if (ok) ok = Rec.WriteDOUBLE(m_dPixelsPerInch);		// Resolution
	if (ok) ok = Rec.WriteUnicode(m_strPostProID);		// Effect ID
	if (ok) ok = Rec.WriteUnicode(m_strDisplayName);	// Display Name
	if (ok) ok = Rec.WriteINT32(m_FeatherSize);			// MILLIPOINT Feather size
	if (ok) ok = Rec.WriteDOUBLE((double)m_Profile.GetBias());	// Profile Bias
	if (ok) ok = Rec.WriteDOUBLE((double)m_Profile.GetGain());	// Profile Gain

	// Write the record
	if (ok) ok = pFilter->Write(&Rec);

	return ok;
#else
	return FALSE;
#endif
}




/********************************************************************************************
>	virtual BOOL NodeFeatherEffect::ProcessBitmap(RenderRegion* pRender,
												  CBitmapCache* pBitmapCache,
												  LPBITMAPINFO pBMPInfo,
												  LPBYTE pBMPBits,
												  DocRect BMPRect,
												  LPBITMAPINFO& pOutputInfo,
												  LPBYTE& pOutputBits,
												  double* pXOffset,
												  double* pYOffset)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/11/2004
	Returns:	
	Purpose:	Processes the bitmap produced by the children of this node.
				Can be overridden by derived classes to perform special processing

********************************************************************************************/

BOOL NodeFeatherEffect::ProcessBitmap(RenderRegion* pRender,
									  CBitmapCache* pBitmapCache,
									  LPBITMAPINFO pBMPInfo,
									  LPBYTE pBMPBits,
									  DocRect BMPRect,
									  LPBITMAPINFO& pOutputInfo,
									  LPBYTE& pOutputBits,
									  double* pXOffset,
									  double* pYOffset)
{
	pOutputInfo = NULL;
	pOutputBits = NULL;
	*pXOffset = 0;
	*pYOffset = 0;

	/////////////////////////////////////////////////////////////////////////////////////////
	//	Create the feather bitmap transparency mask.
	/////////////////////////////////////////////////////////////////////////////////////////

	// 1. Extract a mask from the RGBT input bitmap
	// 2. Contour inwards by half the blur diameter
	// 3. Blur by blur diameter

	double dPixBlurDiameter = (double)m_FeatherSize/GetPixelWidth();
	double dRemainingContour = dPixBlurDiameter/2;
	double dContourStep = 20;

	if (dPixBlurDiameter<1)
		return TRUE;

	// Initialise the processing pipeline state
	ProcessBitmapState bmpstate(pBMPInfo, pBMPBits, 0, 0, FALSE);
//											DEBUGATTACH(bmpstate, "Feather Capture");

	// ----------------------------------
	// This is the processing pipeline
	// Note that contouring is sped up by doing it in steps of 20 pixels or less
	//
	// At each step:
	//	* Transform one state to the next (inside the function),
	//  * Record the new state (assign result to bmpstate)
	//  * Delete previous temp state (inside ProcessBitmapState assignment operator)
	//
	bmpstate = bmpstate.Create8BPPMask();
//											DEBUGATTACH(bmpstate, "Feather Mask");

//	bmpstate = bmpstate.Contour8BPP(-dPixBlurDiameter/2);
	while (dRemainingContour>0)
	{
		if (dContourStep>dRemainingContour) dContourStep = dRemainingContour;
		bmpstate = bmpstate.Contour8BPP(-dContourStep);
//											DEBUGATTACH(bmpstate, "Feather Contour");
		dRemainingContour -= dContourStep;
	}
//	bmpstate = bmpstate.Expand8BPP((INT32)(dPixBlurDiameter+1), 0xFF);
	UINT32 uBlurDiameter = UINT32(dPixBlurDiameter+0.5)-1 ;
	bmpstate = bmpstate.Expand8BPP(uBlurDiameter, 0xFF);
//											DEBUGATTACH(bmpstate, "Feather Expand");
	bmpstate = bmpstate.Blur8BPP(dPixBlurDiameter);
//											DEBUGATTACH(bmpstate, "Feather Blur");
	// ----------------------------------

	// If the final state is OK Extract the results from the pipeline
	if (bmpstate.IsOK())
	{
		bmpstate.GetPixelOffsets(pXOffset, pYOffset);
		bmpstate.GetBitmap(pOutputInfo, pOutputBits);		// (Makes final bitmap "permanent")
	}
	else
		bmpstate.DeInit();

	return bmpstate.IsOK();
}

			

			
/********************************************************************************************

>	virtual BOOL NodeFeatherEffect::FindCachedEffect(CBitmapCache* pBitmapCache)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/11/2004
	Inputs:		-
	Outputs:	-
	Purpose:	Find all cached items this node needs to call RenderCachedEffect
				succesfully
				Feathers need two bitmaps - captured objects and feather mask

********************************************************************************************/

BOOL NodeFeatherEffect::FindCachedEffect(CBitmapCache* pBitmapCache)
{
	BOOL bFoundCached = FALSE;
	CCachedBitmap cbmp;

	// TODO: Call GetChildDirectBitmap here?
	CBitmapCacheKey inky(this, GetPixelWidth(), 0);						// Get cached BMP for this ORIGINAL node at our dpi
	bFoundCached = pBitmapCache->Lookup(inky, cbmp);
//	bFoundCached = GetOriginalBitmap();

	CBitmapCacheKey inky2(this, GetPixelWidth(), 1);					// Get cached BMP for this PROCESSED FEATHER node at our dpi
	bFoundCached = bFoundCached & pBitmapCache->Lookup(inky2, cbmp);

	return bFoundCached;
}




/********************************************************************************************

>	BOOL NodeFeatherEffect::RenderCachedEffect(CBitmapCache* pBitmapCache, RenderRegion* pRender, BOOL bIgnoreCapture = FALSE)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/11/2004
	Inputs:		-
	Outputs:	-
	Purpose:	Render the cached version of this node
				Feathers need two bitmaps - captured objects and feather mask

********************************************************************************************/

BOOL NodeFeatherEffect::RenderCachedEffect(CBitmapCache* pBitmapCache, RenderRegion* pRender, BOOL bIgnoreCapture)
{
	BOOL bRendered = FALSE;
	BOOL bFoundCached = FALSE;

	// TODO: Call GetChildDirectBitmap here?
	CBitmapCacheKey inky(this, GetPixelWidth(), 0);						// Get cached BMP for this ORIGINAL node at our dpi
	CCachedBitmap cbmp;
	bFoundCached = pBitmapCache->Lookup(inky, cbmp);
//	LPBITMAPINFO lpOriginalInfo = NULL;
//	LPBYTE lpOriginalBits = NULL;
//	bFoundCached = GetOriginalBitmap();

	CBitmapCacheKey inky2(this, GetPixelWidth(), 1);					// Get cached BMP for this PROCESSED FEATHER node at our dpi
	CCachedBitmap cbmpFeather;
	bFoundCached = bFoundCached & pBitmapCache->Lookup(inky2, cbmpFeather);

	if (bFoundCached)
	{
		// No need to render if we are going to supply to a capture
		Capture* pCapture = pRender->GetTopCapture();
		if (!bIgnoreCapture && pCapture && pCapture->ConsumeDirectBitmap(this))
		{
			// Tell the caller that the effect has been "rendered"
			bRendered = TRUE;
		}
		else
		{
			pRender->SaveContext();

//#ifdef DEBUG
//		bRendered = pRender->RenderBits(cbmpFeather.pbmpInfo, cbmpFeather.pbmpBits, &cbmpFeather.coord0, 3);
//#endif

			CWxBitmap* wFeatherBitmap		= new CWxBitmap(cbmpFeather.pbmpInfo, cbmpFeather.pbmpBits);
			KernelBitmap* kFeatherBitmap	= new KernelBitmap(wFeatherBitmap,TRUE);
			if (kFeatherBitmap->GetBPP()==8)
			{
				LPRGBQUAD pPalette = kFeatherBitmap->GetPaletteForBitmap();
				for ( INT32 i=0 ; i<0x100 ; i++ )
					(UINT32&)pPalette[i] = i*0x010101 ;
					
				kFeatherBitmap->SetAsGreyscale();
			}

			BitmapTranspFillAttribute* pBmpTranspFill = new BitmapTranspFillAttribute;
			TranspFillMappingLinearAttribute* pNoRepeatAttr = new TranspFillMappingLinearAttribute;
			if (kFeatherBitmap != NULL)
			{
				pNoRepeatAttr->Repeat = 0;
				pRender->SetTranspFillMapping(pNoRepeatAttr, TRUE);		// Temp, attribute will be deleted when unused

				CreateBitmapTranspFill(kFeatherBitmap, &cbmpFeather.coord0, pBmpTranspFill);
				pRender->SetTranspFillGeometry(pBmpTranspFill, TRUE);	// Temp, attribute will be deleted when unused
			}

			// URGH!	We're using transparency attr to implement feather
			// BUT		The user might want to apply an effect transparency attribute as well...
			// SO		We must capture the results of our feather transp, then apply the effect attrs
			if (!this->HasEffectAttrs())
				// Simple, just render it as we always intended...
				bRendered = pRender->RenderBits(cbmp.pbmpInfo, cbmp.pbmpBits, &cbmp.coord0, 3, FALSE, NULL);
			else
			{
				// Tricky, capture feather transp, then apply effect attrs...
				pRender->SaveContext();

				LPBITMAPINFO lpCapturedInfo = NULL;
				LPBYTE lpCapturedBits = NULL;
				CaptureFlags caFlags = CaptureFlags(cfLOCKEDTRANSPARENT | cfUNCLIP | cfPIXWIDTHSCALE);
				DocRect CaptureRect = GetBoundingRect();
				pRender->StartCapture(this, CaptureRect, CAPTUREINFO(ctNESTABLE, caFlags), TRUE, FALSE, 0);
				bRendered = pRender->RenderBits(cbmp.pbmpInfo, cbmp.pbmpBits, &cbmp.coord0, 3, FALSE, NULL);
				pRender->StopCapture(this, FALSE, FALSE, &lpCapturedInfo, &lpCapturedBits, &CaptureRect);

				pRender->RestoreContext();

				if (bRendered && lpCapturedInfo && lpCapturedBits)
				{
					// Now render the bitmap we just captured using the effect attrs that are applied   vvvv
					bRendered = pRender->RenderBits(lpCapturedInfo, lpCapturedBits, CaptureRect, FALSE, this);
					FreeDIB(lpCapturedInfo, lpCapturedBits);
				}
			}

			if (wFeatherBitmap)
				wFeatherBitmap->BMBytes = ((CWxBitmap*)OILBitmap::Default)->BMBytes;
			if (kFeatherBitmap)
				delete kFeatherBitmap;

			pRender->RestoreContext();
		}
	}

	return bRendered;
}



/********************************************************************************************

>	BOOL NodeFeatherEffect::CreateBitmapTranspFill(KernelBitmap* pFeather, DocCoord* pCoords, BitmapTranspFillAttribute* BmpTranspFill)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/01/2005
	Inputs:		-
	Outputs:	-
	Purpose:	Render the cached version of this node
				Feathers need two bitmaps - captured objects and feather mask

********************************************************************************************/

BOOL NodeFeatherEffect::CreateBitmapTranspFill(KernelBitmap* pFeather, DocCoord* pCoords, BitmapTranspFillAttribute* BmpTranspFill)
{
	// Transp fill is same size as viewable part of the bitmap 
	BmpTranspFill->StartPoint = pCoords[0];
	BmpTranspFill->EndPoint   = pCoords[1];
	BmpTranspFill->EndPoint2 	= pCoords[2];
	BmpTranspFill->EndPoint3 	= DocCoord(pCoords[1].x, pCoords[2].y);
	BmpTranspFill->GetBitmapRef()->SetBitmap(pFeather);
	BmpTranspFill->Transp = 0;
	BmpTranspFill->EndTransp = 255;
	m_Profile.SetIsAFeatherProfile(TRUE);		// enable extra processing on the profile
	BmpTranspFill->SetProfile(m_Profile);

	return TRUE;
}




/********************************************************************************************
>	static BOOL NodeFeatherEffect::ProcessBitmap( LPBITMAPINFO pBMPInfo,
												  LPBYTE pBMPBits,
												  double dPixBlurDiameter,
												  CProfileBiasGain profileFeather)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/11/2004
	Returns:	
	Purpose:	Processes the bitmap produced by the children of this node.
				Can be overridden by derived classes to perform special processing

********************************************************************************************/

BOOL NodeFeatherEffect::ProcessBitmap(LPBITMAPINFO pBMPInfo,
									  LPBYTE pBMPBits,
									  double dPixBlurDiameter,
									  CProfileBiasGain profileFeather)
{
	LPBITMAPINFO pOutputInfo = NULL;
	LPBYTE pOutputBits = NULL;
//	double XOffset = 0;
//	double YOffset = 0;

	/////////////////////////////////////////////////////////////////////////////////////////
	//	Create the feather bitmap transparency mask.
	/////////////////////////////////////////////////////////////////////////////////////////

	// 1. Extract a mask from the RGBT input bitmap
	// 2. Contour inwards by half the blur diameter
	// 3. Blur by blur diameter

	dPixBlurDiameter = floor(dPixBlurDiameter+0.5);
	double dRemainingContour = dPixBlurDiameter/2;
	double dContourStep = 20;

	if (dPixBlurDiameter<1)
		return TRUE;

	// Initialise the processing pipeline state
	ProcessBitmapState bmpstate(pBMPInfo, pBMPBits, 0, 0, FALSE);
//											DEBUGATTACH(bmpstate, "Feather Capture");

	// ----------------------------------
	// This is the processing pipeline
	// Note that contouring is sped up by doing it in steps of 20 pixels or less
	//
	// At each step:
	//	* Transform one state to the next (inside the function),
	//  * Record the new state (assign result to bmpstate)
	//  * Delete previous temp state (inside ProcessBitmapState assignment operator)
	//
	bmpstate = bmpstate.Create8BPPMask();
//											DEBUGATTACH(bmpstate, "Feather Mask");

//	bmpstate = bmpstate.Contour8BPP(-dPixBlurDiameter/2);
	while (dRemainingContour>0)
	{
		if (dContourStep>dRemainingContour) dContourStep = dRemainingContour;
		bmpstate = bmpstate.Contour8BPP(-dContourStep);
//											DEBUGATTACH(bmpstate, "Feather Contour");
		dRemainingContour -= dContourStep;
	}
//	bmpstate = bmpstate.Expand8BPP((INT32)(dPixBlurDiameter+1), 0xFF);
	UINT32 uBlurDiameter = UINT32(dPixBlurDiameter+0.5)-1 ;
	bmpstate = bmpstate.Expand8BPP(uBlurDiameter, 0xFF);
//											DEBUGATTACH(bmpstate, "Feather Expand");
	bmpstate = bmpstate.Blur8BPP(dPixBlurDiameter);
//											DEBUGATTACH(bmpstate, "Feather Blur");
	bmpstate = bmpstate.AddMaskTo(pBMPInfo, pBMPBits);
//											DEBUGATTACH(bmpstate, "Feathered Bitmap");
	// ----------------------------------

	// If the final state is OK Extract the results from the pipeline
	if (bmpstate.IsOK())
	{
//		bmpstate.GetPixelOffsets(pXOffset, pYOffset);
		bmpstate.GetBitmap(pOutputInfo, pOutputBits);		// (Makes final bitmap "permanent")
	}
	else
		bmpstate.DeInit();

	return bmpstate.IsOK();
}

			

			
/********************************************************************************************

>	BOOL NodeFeatherEffect::GetDirectBitmap(RenderRegion* pRender, LPBITMAPINFO* plpInfo, LPBYTE* plpBits, DocRect* pRect, Matrix* pMat, double* pdRes)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/08/2005
	Inputs:		pMat	- pointer to matrix awaiting update
	Outputs:	plpInfo - LPBITMAPINFO pointer describing bitmap
				plpBits - LPBYTE pointer to bitmap data
				pRect	- rect of Original bitmap data
				pMat	- matrix describing transform from pRect to this instance on the page
				pdRes	- resolution of this bitmap (dpi)
	Returns:	TRUE if this node can supply direct bitmap data to the caller
				FALSE otherwise
	Purpose:	Return details of direct bitmap to caller (caller is usually a NodeBitmapEffect)

********************************************************************************************/

/*BOOL NodeFeatherEffect::GetDirectBitmap(RenderRegion* pRender, LPBITMAPINFO* plpInfo, LPBYTE* plpBits, DocRect* pRect, Matrix* pMat, double* pdRes)
{
	DocRect rectDirect;
	Matrix matLocalTransform;
	double dResolution = 0;

	GetDirectBitmapDetails(&rectDirect, &dResolution);
	if (pRect)	*pRect = rectDirect;
	if (pdRes)	*pdRes = dResolution;

	// We don't change our child's instance transformation
	// So just get that directly from the child
	BOOL bChildDirect = GetChildDirectBitmap(pRender, NULL, NULL, NULL, &matLocalTransform);
	if (pMat)	*pMat = matLocalTransform;

	// If we have effect attrs applied then we must make a subtree bitmap to capture them
	if (plpInfo && plpBits)
	{
		AttrFillGeometry* pTranspAttr = NULL;
		double dPixelWidth = 72000.0/dResolution;

		// Lookup processed bitmap in cache 
		CBitmapCache* pBitmapCache = Camelot.GetBitmapCache();
		if (pBitmapCache==NULL)
			return FALSE;

		CBitmapCacheKey inky2(this, dPixelWidth, 2);			// Option 2 is processed/rendered bitmap (see below)
		CCachedBitmap cbmp2;
		BOOL bFound = pBitmapCache->Lookup(inky2, cbmp2);
		if (bFound)
		{
			if (plpInfo)	*plpInfo = cbmp2.pbmpInfo;
			if (plpBits)	*plpBits = cbmp2.pbmpBits;
			return TRUE;
		}

		// We need to create a new bitmap here that captures the effects of the attributes
		//
		// If we don't have a RenderRegion, we can't build it, so return NULLs to indicate
		// that we need to be called again with a RenderRegion (allows quick test)
		// (See EnsureLiveEffectOriginalBitmaps)
		if (pRender == NULL)
		{
			if (plpInfo)	*plpInfo	= NULL;
			if (plpBits)	*plpBits	= NULL;
			return TRUE;
		}

		// We must "render" this bitmap to capture the attributes that effect its appearance
		// By default, we can't do arbitrarily transformed bitmaps - use a bitmap fill.
		DocRect CaptureRect = rectDirect;

		CaptureFlags caFlags = CaptureFlags(cfLOCKEDTRANSPARENT | cfFULLCOVERAGE);
		pRender->StartCapture(this, CaptureRect, CAPTUREINFO(ctNESTABLE, caFlags), TRUE, FALSE, dResolution);
		pRender->SaveContext();

		{	// ------------------------------------------------------
			// Setup coords for rendering in DirectBitmap domain
			DocCoord coords[3];
			coords[0] = CaptureRect.lo;
			coords[1] = DocCoord(CaptureRect.hix, CaptureRect.loy);
			coords[2] = DocCoord(CaptureRect.lox, CaptureRect.hiy);

			// NOTE! We can only deal with effect transparency at the moment!
			// Render the transparency geometry using inverse Direct-Screen transform
			// (This bitmap is being rendered in "Direct space" whereas the transparency attribute
			// was applied in "Screen space")
			NodeAttribute* pAttr;
			if (FindAppliedAttribute(CC_RUNTIME_CLASS(AttrTranspFillGeometry), &pAttr))
			{
				if (pAttr &&
					!pAttr->IsADefaultAttr() &&
					!pAttr->HasEquivalentDefaultValue(TRUE) &&
					pAttr->IsEffectAttribute())
				{
					pTranspAttr = (AttrFillGeometry*) ((AttrFillGeometry*)pAttr)->SimpleCopy();
					DocCoord tcoords[4];
					tcoords[0] = *pTranspAttr->GetStartPoint();
					tcoords[1] = *pTranspAttr->GetEndPoint();
					tcoords[2] = *pTranspAttr->GetEndPoint2();
					tcoords[3] = *pTranspAttr->GetEndPoint3();

					Matrix matInverse = matLocalTransform.Inverse();
					matInverse.transform(tcoords, 4);

					pTranspAttr->SetStartPoint(&tcoords[0]);
					pTranspAttr->SetEndPoint(&tcoords[1]);
					pTranspAttr->SetEndPoint2(&tcoords[2]);
					pTranspAttr->SetEndPoint3(&tcoords[3]);

					pTranspAttr->Render(pRender);
				}
			}

			// TODO: This probably won't work because the geometry will need to be inverted into DirectBitmap space
			RenderCachedEffect(pBitmapCache, pRender, TRUE);
		}	// ------------------------------------------------------

		pRender->RestoreContext();
		if (pTranspAttr)
		{
			delete pTranspAttr;
			pTranspAttr = NULL;
		}

		LPBITMAPINFO lpInfo = NULL;
		LPBYTE lpBits = NULL;
		pRender->StopCapture(this, FALSE, FALSE, &lpInfo, &lpBits, &CaptureRect);

		// We should now have a bitmap containing an upright version of the NodeBitmap
		// with transparency and contoning applied as per the attributes in the tree
		if (lpInfo && lpBits)
		{
			*plpInfo = lpInfo;
			*plpBits = lpBits;

			// Cache the PROCESSED/RENDERED bitmap as Option 2
			cbmp2.pbmpBits = lpBits;
			cbmp2.pbmpInfo = lpInfo;
			cbmp2.SetCachedRect(CaptureRect);
			cbmp2.nPriority = CACHEPRIORITY_TEMPBITMAP_HIGH;
			if (cbmp2.IsValid())
				pBitmapCache->StoreBitmap(inky2, cbmp2);
		}
	}

	return TRUE;
}*/




/********************************************************************************************
>	virtual BOOL NodeFeatherEffect::CompareState(NodeEffect* pPPNode)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/03/2005
	Returns:	TRUE if this state matches the state (and type) of the supplied node
	Purpose:	-

********************************************************************************************/

BOOL NodeFeatherEffect::CompareState(NodeEffect* pPPNode)
{
	if (!pPPNode->IsFeatherEffect())
		return FALSE;

	NodeFeatherEffect* pTest = (NodeFeatherEffect*)pPPNode;

	BOOL bSame = m_FeatherSize == pTest->m_FeatherSize;
	bSame = bSame && m_Profile == pTest->m_Profile;

	return bSame;
}




/********************************************************************************************
>	virtual BOOL NodeFeatherEffect::GetQuickRender()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/03/2005
	Returns:	TRUE if this feather effect should not start new captures at the minute
	Purpose:	-

********************************************************************************************/

BOOL NodeFeatherEffect::GetQuickRender()
{
	if (Operation::GetQuickRender(this)==FALSE)
		return FALSE;

	// Operation wants us to be quick but if it's a feather editing op
	// then we've got to re-render anyway!
	// Urgh! This is getting tortuous...
PORTNOTE("effects", "NodeFeatherEffect::GetQuickRender - removed OpChangeFeatherSize usage.");
#if defined(EXCLUDE_FROM_XARALX)
	return FALSE;
#else
	Operation* pOp = Operation::GetCurrentDragOp();
	return (pOp!=NULL && !pOp->IsKindOf(CC_RUNTIME_CLASS(OpChangeFeatherSize)));
#endif
}
#endif




/********************************************************************************************
	Class:		LiveEffectRecordHandler

********************************************************************************************/

/********************************************************************************************

>	virtual UINT32* LiveEffectRecordHandler::GetTagList()

 	Author:		Andy_Hills (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/10/00
	Inputs:		-
	Returns:	Ptr to a list of tag values, terminated by CXFRH_TAG_LIST_END
	Purpose:	Provides the record handler system with a list of records handled by this
				handler
	SeeAlso:	-

********************************************************************************************/

UINT32* LiveEffectRecordHandler::GetTagList()
{
	static UINT32 TagList[] = {	TAG_LIVE_EFFECT,
								TAG_LOCKED_EFFECT,
								TAG_FEATHER_EFFECT,

								CXFRH_TAG_LIST_END};

	return (UINT32*)&TagList;
}


/********************************************************************************************

>	virtual BOOL LiveEffectRecordHandler::IsStreamed(UINT32 Tag)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/01/2004
	Inputs:		The tag of the record
	Returns:	TRUE if this is a streamed record
				FALSE otherwise
	Purpose:	Function to find out if the record is streamed or not.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL LiveEffectRecordHandler::IsStreamed(UINT32 Tag)
{
	// We handle both types so check what we need to do by the tag we have been given
	BOOL Streamed = FALSE;
	switch (Tag)
	{
		case TAG_LIVE_EFFECT:
		case TAG_LOCKED_EFFECT:
		case TAG_FEATHER_EFFECT:
			Streamed = FALSE;

			break;
		default:
			Streamed = FALSE;
			ERROR3_PF(("LiveEffectRecordHandler::IsStreamed I don't handle records with the tag (%d)\n", Tag));
			break;
	}

	return Streamed;
}

/********************************************************************************************

>	virtual BOOL LiveEffectRecordHandler::HandleRecord(CXaraFileRecord* pCXaraFileRecord)

 	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	??/10/2004
	Inputs:		pCXaraFileRecord = ptr to record to handle
	Returns:	TRUE if handled successfuly
				FALSE otherwise
	Purpose:	Handles the given record.
	SeeAlso:	-

********************************************************************************************/

BOOL LiveEffectRecordHandler::HandleRecord(CXaraFileRecord* pCXaraFileRecord)
{
	ERROR2IF(pCXaraFileRecord == NULL,FALSE,"LiveEffectRecordHandler::HandleRecord pCXaraFileRecord is NULL");

	BOOL ok = TRUE;
	INT32 tag = pCXaraFileRecord->GetTag();
	switch (tag)
	{
	case TAG_LIVE_EFFECT:
		{
			// read in the record ---------------------------------------------------------
			// read flags
			BYTE Flags;
			if (ok) ok = pCXaraFileRecord->ReadBYTE(&Flags);

			// Read resolution
			double dPixelsPerInch;
			if (ok) ok = pCXaraFileRecord->ReadDOUBLE(&dPixelsPerInch);

			// Read Effect ID
			String_256 strPostProID;
			if (ok) ok = pCXaraFileRecord->ReadUnicode(&strPostProID);

			// Read Display Name
			String_256 strDisplayName;
			if (ok) ok = pCXaraFileRecord->ReadUnicode(&strDisplayName);

			// Read edits list
//			_bstr_t bstrXML;
//			if (ok) ok = pCXaraFileRecord->ReadBSTR(&bstrXML, pCXaraFileRecord->GetSize());
			StringVar strXML;
			if (ok) ok = pCXaraFileRecord->ReadUTF16STR(&strXML, pCXaraFileRecord->GetSize());

			// process the record ---------------------------------------------------------
			if (ok)
			{
				NodeLiveEffect* pLE = new NodeLiveEffect();

				pLE->m_dPixelsPerInch = dPixelsPerInch;

				pLE->m_strPostProID = strPostProID;

				pLE->m_strDisplayName = strDisplayName;

PORTNOTETRACE("other","LiveEffectRecordHandler::HandleRecord - removed use of XML");
#ifndef EXCLUDE_FROM_XARALX
				VARIANT_BOOL varResult;
				IXMLDOMDocumentPtr pxmlDoc = CXMLUtils::NewDocument();
				HRESULT hr = pxmlDoc->loadXML(bstrXML, &varResult);
				ok =  (SUCCEEDED(hr) && VARIANT_TRUE == varResult);
				if (ok) pLE->m_pEditsDoc = pxmlDoc;
#else
	pLE->m_pEditsDoc = NULL;
	pLE->m_vstrEdits = strXML;
#endif

				if (ok)	ok = InsertNode(pLE);
			}
			return ok;
		}
		break;

	case TAG_LOCKED_EFFECT:
		{
			// read in the record ---------------------------------------------------------
			// read flags
			BYTE Flags;
			if (ok) ok = pCXaraFileRecord->ReadBYTE(&Flags);

			// Read resolution
			double dPixelsPerInch;
			if (ok) ok = pCXaraFileRecord->ReadDOUBLE(&dPixelsPerInch);

			// Read bitmap ref
			INT32 BitmapRef;
			if (ok) ok = pCXaraFileRecord->ReadINT32(&BitmapRef);

			// Read coords
			DocCoord tc[3];
			if (ok) ok = pCXaraFileRecord->ReadCoordTrans(&tc[0], 0, 0);
			if (ok) ok = pCXaraFileRecord->ReadCoordTrans(&tc[1], 0, 0);
			if (ok) ok = pCXaraFileRecord->ReadCoordTrans(&tc[2], 0, 0);

			// Read Effect ID
			String_256 strPostProID;
			if (ok) ok = pCXaraFileRecord->ReadUnicode(&strPostProID);

			// Read Display Name
			String_256 strDisplayName;
			if (ok) ok = pCXaraFileRecord->ReadUnicode(&strDisplayName);

			// Read edits list
//			_bstr_t bstrXML;
//			if (ok) ok = pCXaraFileRecord->ReadBSTR(&bstrXML, pCXaraFileRecord->GetSize());
			StringVar strXML;
			if (ok) ok = pCXaraFileRecord->ReadUTF16STR(&strXML, pCXaraFileRecord->GetSize());

			// process the record ---------------------------------------------------------
			if (ok)
			{
				NodeLockedEffect* pLE = new NodeLockedEffect();

				pLE->m_bIsDestructive = !((Flags & 0x02) == 0x02);		// Flag set in file when NOT destructive for back-compatibility

				pLE->m_dPixelsPerInch = dPixelsPerInch;
				pLE->m_strPostProID = strPostProID;
				pLE->m_strDisplayName = strDisplayName;
				pLE->m_PGram[0] = tc[0];
				pLE->m_PGram[1] = tc[1];
				pLE->m_PGram[2] = tc[2];

PORTNOTETRACE("other","LiveEffectRecordHandler::HandleRecord - removed use of XML");
#ifndef EXCLUDE_FROM_XARALX
				VARIANT_BOOL varResult;
				IXMLDOMDocumentPtr pxmlDoc = CXMLUtils::NewDocument();
				HRESULT hr = pxmlDoc->loadXML(bstrXML, &varResult);
				ok =  (SUCCEEDED(hr) && VARIANT_TRUE == varResult);
				if (ok) pLE->m_pEditsDoc = pxmlDoc;
#else
	pLE->m_pEditsDoc = NULL;
	pLE->m_vstrEdits = strXML;
#endif

				// Convert the bmp reference into a kernel bmp
				if (ok)
				{
					KernelBitmap* pBitmap = NULL;
					pBitmap = GetReadBitmapReference(BitmapRef);
					pBitmap->GetActualBitmap()->SetHidden(TRUE);		// We don't want the user to see this bitmap in the gallery
					ok = (pBitmap != NULL);
					if (ok) pLE->m_BitmapRef.Attach(pBitmap);
				}

				if (ok)	ok = InsertNode(pLE);
			}

			return ok;
		}
		break;

#ifdef FEATHER_EFFECT
	case TAG_FEATHER_EFFECT:
		{
			// read in the record ---------------------------------------------------------
			// read flags
			BYTE Flags;
			if (ok) ok = pCXaraFileRecord->ReadBYTE(&Flags);

			// Read resolution
			double dPixelsPerInch;
			if (ok) ok = pCXaraFileRecord->ReadDOUBLE(&dPixelsPerInch);

			// Read Effect ID
			String_256 strPostProID;
			if (ok) ok = pCXaraFileRecord->ReadUnicode(&strPostProID);

			// Read Display Name
			String_256 strDisplayName;
			if (ok) ok = pCXaraFileRecord->ReadUnicode(&strDisplayName);

			// Read feather details
			MILLIPOINT mpFeatherSize = 0;
			double dBias = 0;
			double dGain = 0;
			if (ok) ok = pCXaraFileRecord->ReadINT32(&mpFeatherSize);
			if (ok) ok = pCXaraFileRecord->ReadDOUBLE(&dBias);
			if (ok) ok = pCXaraFileRecord->ReadDOUBLE(&dGain);

			// process the record ---------------------------------------------------------
			if (ok)
			{
				NodeFeatherEffect* pFE = new NodeFeatherEffect();

				pFE->m_dPixelsPerInch = dPixelsPerInch;

				pFE->m_strPostProID = strPostProID;

				pFE->m_strDisplayName = strDisplayName;

				pFE->SetFeatherSize(mpFeatherSize);

				CProfileBiasGain Profile((AFp)dBias, (AFp)dGain);
				pFE->SetProfile(Profile);

				if (ok)	ok = InsertNode(pFE);
			}

			return ok;
		}
		break;
#endif

	default:
		ERROR3_PF(("I don't handle records with the tag (%d)\n", pCXaraFileRecord->GetTag()));
		break;
	}

	return FALSE;
}




/********************************************************************************************

>	virtual BOOL LiveEffectRecordHandler::HandleStreamedRecord(CXaraFile * pCXFile, UINT32 Tag,UINT32 Size,UINT32 RecordNumber)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/01/2004
	Inputs:		pCXFile = access to the CXaraFile class
				Tag	 = the tag value
				Size = size of record
				RecordNumber = the record number in the file
	Returns:	TRUE if handled ok
				FALSE otherwise
	Purpose:	This is the bitmap streamed record handler. It handles the loading of bitmap
				definitions.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL LiveEffectRecordHandler::HandleStreamedRecord(CXaraFile * pCXFile, UINT32 Tag,UINT32 Size,UINT32 RecordNumber)
{
	ERROR2IF(pCXFile == NULL,FALSE,"BitmapRecordHandler::HandleStreamedRecord pCXFile is NULL");

	ERROR3_PF(("Unimplemented!", Tag));

	return TRUE;
}




/********************************************************************************************

>	virtual void LiveEffectRecordHandler::GetRecordDescriptionText(CXaraFileRecord* pRecord,StringBase* pStr)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/09/2005
	Inputs:		pRecord = ptr to a record
				pStr = ptr to string to update
	Returns:	-
	Purpose:	Produce a textual description of the specified record
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

#if XAR_TREE_DIALOG
void LiveEffectRecordHandler::GetRecordDescriptionText(CXaraFileRecord* pCXaraFileRecord, StringBase* pStr)
{
	if (pStr == NULL || pCXaraFileRecord == NULL)
		return;

	char s[256];
	BOOL ok = TRUE;

	// Call base class first
	CamelotRecordHandler::GetRecordDescriptionText(pCXaraFileRecord, pStr);

	switch (pCXaraFileRecord->GetTag())
	{
	case TAG_LIVE_EFFECT:
		{
			// read in the record ---------------------------------------------------------
			// read flags
			BYTE Flags;
			if (ok) ok = pCXaraFileRecord->ReadBYTE(&Flags);

			// Read resolution
			double dPixelsPerInch;
			if (ok) ok = pCXaraFileRecord->ReadDOUBLE(&dPixelsPerInch);

			// Read Effect ID
			String_256 strPostProID;
			if (ok) ok = pCXaraFileRecord->ReadUnicode(&strPostProID);

			// Read Display Name
			String_256 strDisplayName;
			if (ok) ok = pCXaraFileRecord->ReadUnicode(&strDisplayName);

			// Read edits list
//			_bstr_t bstrXML;
//			if (ok) ok = pCXaraFileRecord->ReadBSTR(&bstrXML, pCXaraFileRecord->GetSize());
			StringVar strXML;
			if (ok) ok = pCXaraFileRecord->ReadUTF16STR(&strXML, pCXaraFileRecord->GetSize());

			// --------------------------------------------------------------
			_stprintf(s,"Flags\t\t= %d\r\n", (INT32)Flags);
			(*pStr) += s;
		}
		break;

	case TAG_LOCKED_EFFECT:
		{
			// read in the record ---------------------------------------------------------
			// read flags
			BYTE Flags;
			if (ok) ok = pCXaraFileRecord->ReadBYTE(&Flags);

			// Read resolution
			double dPixelsPerInch;
			if (ok) ok = pCXaraFileRecord->ReadDOUBLE(&dPixelsPerInch);

			// Read bitmap ref
			INT32 BitmapRef;
			if (ok) ok = pCXaraFileRecord->ReadINT32(&BitmapRef);

			// Read coords
			DocCoord tc[3];
			if (ok) ok = pCXaraFileRecord->ReadCoordTrans(&tc[0], 0, 0);
			if (ok) ok = pCXaraFileRecord->ReadCoordTrans(&tc[1], 0, 0);
			if (ok) ok = pCXaraFileRecord->ReadCoordTrans(&tc[2], 0, 0);

			// Read Effect ID
			String_256 strPostProID;
			if (ok) ok = pCXaraFileRecord->ReadUnicode(&strPostProID);

			// Read Display Name
			String_256 strDisplayName;
			if (ok) ok = pCXaraFileRecord->ReadUnicode(&strDisplayName);

			// Read edits list
//			_bstr_t bstrXML;
//			if (ok) ok = pCXaraFileRecord->ReadBSTR(&bstrXML, pCXaraFileRecord->GetSize());
			StringVar strXML;
			if (ok) ok = pCXaraFileRecord->ReadUTF16STR(&strXML, pCXaraFileRecord->GetSize());

			// --------------------------------------------------------------
			_stprintf(s,"Flags\t\t= %d\r\n", (INT32)Flags);
			(*pStr) += s;
		}
		break;

#ifdef FEATHER_EFFECT
	case TAG_FEATHER_EFFECT:
		{
			// read in the record ---------------------------------------------------------
			// read flags
			BYTE Flags;
			if (ok) ok = pCXaraFileRecord->ReadBYTE(&Flags);

			// Read resolution
			double dPixelsPerInch;
			if (ok) ok = pCXaraFileRecord->ReadDOUBLE(&dPixelsPerInch);

			// Read Effect ID
			String_256 strPostProID;
			if (ok) ok = pCXaraFileRecord->ReadUnicode(&strPostProID);

			// Read Display Name
			String_256 strDisplayName;
			if (ok) ok = pCXaraFileRecord->ReadUnicode(&strDisplayName);

			// Read feather details
			MILLIPOINT mpFeatherSize = 0;
			double dBias = 0;
			double dGain = 0;
			if (ok) ok = pCXaraFileRecord->ReadINT32(&mpFeatherSize);
			if (ok) ok = pCXaraFileRecord->ReadDOUBLE(&dBias);
			if (ok) ok = pCXaraFileRecord->ReadDOUBLE(&dGain);

			// --------------------------------------------------------------
			_stprintf(s,"Flags\t\t= %d\r\n", (INT32)Flags);
			(*pStr) += s;
		}
		break;
#endif

/*		case TAG_GROUP:
			break;

		case TAG_COMPOUNDRENDER:
			INT32 reserved = 0;
			pRecord->ReadINT32(&reserved);
			_stprintf(s,"Reserved\t\t= %d\r\n", reserved);
			(*pStr) += s;

			DocRect rbounds;
			pRecord->ReadCoord(&rbounds.lo);
			pRecord->ReadCoord(&rbounds.hi);
			_stprintf(s,"Bounds\t\t= %d, %d, %d, %d\r\n", rbounds.lox, rbounds.loy, rbounds.hix, rbounds.hiy);
			(*pStr) += s;
			break;
*/
	}
}
#endif




/********************************************************************************************

>	ProcessBitmapState::ProcessBitmapState(LPBITMAPINFO lpInitialInfo,
						LPBYTE lpInitialBits,
						double InitialXOffset = 0,
						double InitialYOffset = 0,
						BOOL bInitialTempState = FALSE)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/04/2005
	Inputs:		lpInitialInfo - pointer to bitmap info structure
				lpInitialBits - pointer to bitmap data
				InitialXOffset - Start position of bitmap in pixel space 
				InitialYOffset -
				bInitialTempState - TRUE if this state represents a temporary bitmap
									that can be deallocated when the state dies
	Outputs		-
	Returns:	-
	Purpose:	Building constructor

********************************************************************************************/

#ifdef FEATHER_EFFECT
ProcessBitmapState::ProcessBitmapState(LPBITMAPINFO lpInitialInfo,
						LPBYTE lpInitialBits,
						double InitialXOffset,
						double InitialYOffset,
						BOOL bInitialTempState)
{
	m_lpInfo = lpInitialInfo;
	m_lpBits = lpInitialBits;
	m_XOffset = InitialXOffset;
	m_YOffset = InitialYOffset;
	m_XPelsPerMeter = 0;
	m_YPelsPerMeter = 0;
	m_bTemp = bInitialTempState;
	m_bOK = TRUE;
}




/********************************************************************************************

>	ProcessBitmapState::ProcessBitmapState(const ProcessBitmapState& copystate)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/04/2005
	Inputs:		
	Outputs		-
	Returns:	-
	Purpose:	Copy constructor

********************************************************************************************/

ProcessBitmapState::ProcessBitmapState(const ProcessBitmapState& copystate)
{
	m_lpInfo = copystate.m_lpInfo;
	m_lpBits = copystate.m_lpBits;
	m_XOffset = copystate.m_XOffset;
	m_YOffset = copystate.m_YOffset;
	m_XPelsPerMeter = copystate.m_XPelsPerMeter;
	m_YPelsPerMeter = copystate.m_YPelsPerMeter;
	m_bTemp = copystate.m_bTemp;
	m_bOK = copystate.m_bOK;
}




/********************************************************************************************

>	void ProcessBitmapState::InitPipelineSettings(ProcessBitmapState* const prevstate)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/04/2005
	Inputs:		
	Outputs		-
	Returns:	-
	Purpose:	Copy member vars that should propagate from one state to the next
				in a pipeline

********************************************************************************************/

void ProcessBitmapState::InitPipelineSettings(ProcessBitmapState* const prevstate)
{
	m_XOffset = prevstate->m_XOffset;
	m_YOffset = prevstate->m_YOffset;

	if (m_lpInfo)
	{
		m_lpInfo->bmiHeader.biXPelsPerMeter = prevstate->GetXPelsPerMeter();
		m_lpInfo->bmiHeader.biYPelsPerMeter = prevstate->GetYPelsPerMeter();
	}
	else
	{
		m_XPelsPerMeter = prevstate->GetXPelsPerMeter();
		m_YPelsPerMeter = prevstate->GetYPelsPerMeter();
	}
}




/********************************************************************************************

>	void ProcessBitmapState::DeInit()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/04/2005
	Inputs:		
	Outputs		-
	Returns:	-
	Purpose:	Copy member vars that should propagate from one state to the next
				in a pipeline

********************************************************************************************/

void ProcessBitmapState::DeInit()
{
	if (m_lpInfo && m_lpBits && m_bTemp)
	{
		::FreeDIB(m_lpInfo, m_lpBits);
		m_lpInfo = NULL;
		m_lpBits = NULL;
	}
}




/********************************************************************************************

>	ProcessBitmap& ProcessBitmapState::operator=(const ProcessBitmapState& rhsstate)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/04/2005
	Inputs:		lpInitialInfo - pointer to bitmap info structure
				lpInitialBits - pointer to bitmap data
				InitialXOffset - Start position of bitmap in pixel space 
				InitialYOffset -
				bInitialTempState - TRUE if this state represents a temporary bitmap
									that can be deallocated when the state dies
	Outputs		-
	Returns:	-
	Purpose:	Assignment operator

********************************************************************************************/

ProcessBitmapState& ProcessBitmapState::operator=(const ProcessBitmapState& rhsstate)
{
	if (m_lpInfo && m_lpBits && m_bTemp && m_lpInfo!=rhsstate.m_lpInfo && m_lpBits!=rhsstate.m_lpBits)
	{
		::FreeDIB(m_lpInfo, m_lpBits);
		m_lpInfo = NULL;
		m_lpBits = NULL;
	}

	m_lpInfo = rhsstate.m_lpInfo;
	m_lpBits = rhsstate.m_lpBits;
	m_XOffset = rhsstate.m_XOffset;
	m_YOffset = rhsstate.m_YOffset;
	m_bTemp = rhsstate.m_bTemp;
	m_bOK = rhsstate.m_bOK;

	if (m_lpInfo)
	{
		m_lpInfo->bmiHeader.biXPelsPerMeter = rhsstate.GetXPelsPerMeter();
		m_lpInfo->bmiHeader.biYPelsPerMeter = rhsstate.GetYPelsPerMeter();
		m_XPelsPerMeter = 0;
		m_YPelsPerMeter = 0;
	}
	else
	{
		m_XPelsPerMeter = rhsstate.GetXPelsPerMeter();
		m_YPelsPerMeter = rhsstate.GetYPelsPerMeter();
	}

	return *this;
}




/********************************************************************************************

>	void		ProcessBitmapState::GetBitmap(LPBITMAPINFO& pOutputInfo, LPBYTE& pOutputBits, BOOL bTemp = FALSE)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/04/2005
	Inputs:		bTemp - TRUE if the bitmap state should be marked temporary
						FALSE if the bitmap state should be marked permanent (default)
	Outputs		pOutputInfo - Pointer to bitmap info structure
				pOutputBits - Pointer to bitmap data
	Returns:	-
	Purpose:	Extract the bitmap from the current pipeline state
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void ProcessBitmapState::GetBitmap(LPBITMAPINFO& pOutputInfo, LPBYTE& pOutputBits, BOOL bTemp)
{
	if (m_lpInfo && m_XPelsPerMeter!=0 && m_YPelsPerMeter!=0)
	{
		m_lpInfo->bmiHeader.biXPelsPerMeter = m_XPelsPerMeter;
		m_lpInfo->bmiHeader.biYPelsPerMeter = m_YPelsPerMeter;
		m_XPelsPerMeter = 0;
		m_YPelsPerMeter = 0;
	}

	pOutputInfo = m_lpInfo;
	pOutputBits = m_lpBits;
	m_bTemp = bTemp;
}




/********************************************************************************************

>	BOOL ProcessBitmapState::AllocDIB(UINT32 Width, UINT32 Height, UINT32 Depth)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/04/2005
	Inputs:		bTemp - TRUE if the bitmap state should be marked temporary
						FALSE if the bitmap state should be marked permanent (default)
	Outputs		pOutputInfo - Pointer to bitmap info structure
				pOutputBits - Pointer to bitmap data
	Returns:	-
	Purpose:	Extract the bitmap from the current pipeline state
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL ProcessBitmapState::AllocDIB(UINT32 Width, UINT32 Height, UINT32 Depth)
{
	m_bOK = FALSE;
	m_lpInfo = ::AllocDIB(Width, Height, Depth, &m_lpBits);
	if (m_lpInfo)
	{
		m_bOK = TRUE;
		if (m_XPelsPerMeter!=0 && m_YPelsPerMeter!=0)
		{
			m_lpInfo->bmiHeader.biXPelsPerMeter = m_XPelsPerMeter;
			m_lpInfo->bmiHeader.biYPelsPerMeter = m_YPelsPerMeter;
			m_XPelsPerMeter = 0;
			m_YPelsPerMeter = 0;
		}
	}

	return m_bOK;
}




/********************************************************************************************

>	ProcessBitmapState ProcessBitmapState::Create8BPPMask()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/04/2005
	Inputs:		-
	Outputs		-
	Returns:	New ProcessBitmapState (may be invalid)
	Purpose:	Extract the 8bpp T channel from this bitmap state
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

ProcessBitmapState ProcessBitmapState::Create8BPPMask()
{
	// Create the next state
	ProcessBitmapState mask;
	mask.InitPipelineSettings(this);

	// Don't process if the current state is invalid
	if (!m_bOK)
		return mask;		// IsOK = FALSE

//	ERROR3IF(GetDepth()!=32, "Create8BPPMask only copes with 32BPP inputs at the moment");

	// Get the alpha channel bitmap out of the capture bitmap and invert it
	// because we want ContourBitmap to generate an INNER contour...
	mask.AllocDIB(GetWidth(), GetHeight(), 8);
	if (!mask.IsOK())
		return mask;		// IsOK = FALSE

	if (GetDepth()==32)
	{
		UINT32 uWidth  = mask.GetWidth();
		UINT32 uHeight = mask.GetHeight();
		UINT32 uByteWidth8 = DIBUtil::ScanlineSize(uWidth, 8);
		UINT32 uByteWidth32 = DIBUtil::ScanlineSize(GetWidth(), GetDepth())/sizeof(UINT32);	// width in words

		LPBYTE pDst = mask.m_lpBits;
		UINT32* pSrc = (UINT32*)m_lpBits;
		UINT32 i;
		UINT32 j;
		for ( j=0 ; j<uHeight ; j++ )
		{
			for ( i=0 ; i<uWidth ; i++ )
			{
				// Combine alpha channel info with colour info to get silhouette
				// Unrendered areas are BGRT = 0x000000FF (T=0xFF for fully transparent)
				// Wherever transp is FE or less
	//			pDst[i] = (pSrc[i]==0xFF000000) ? 0 : 0xFF;		// Invert
	//			pDst[i] = (pSrc[i]==0xFF000000) ? 0xFF : 0;
				pDst[i] = (pSrc[i]&0xFF000000) >> 24;
			}
			pDst += uByteWidth8;
			pSrc += uByteWidth32;
		}
	}
	else
	{
		// TODO!
		// Should try to get mask data from other formats here!
		// At the moment just create a solid rectangle...
		UINT32 uWidth  = mask.GetWidth();
		UINT32 uHeight = mask.GetHeight();
		UINT32 uByteWidth8 = DIBUtil::ScanlineSize(uWidth, 8);

		LPBYTE pDst = mask.m_lpBits;
		UINT32 i;
		UINT32 j;
		for ( j=0 ; j<uHeight ; j++ )
		{
			for ( i=0 ; i<uWidth ; i++ )
			{
				pDst[i] = 0;
			}
			pDst += uByteWidth8;
		}
	}

	return mask;
}




/********************************************************************************************

>	ProcessBitmapState ProcessBitmapState::AddMaskTo(LPBITMAPINFO lpDeepInfo, LPBYTE lpDeepBits)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/08/2005
	Inputs:		-
	Outputs		-
	Returns:	New ProcessBitmapState (may be invalid)
	Purpose:	Put this bitmap state into another DIB as the transparency channel
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

ProcessBitmapState ProcessBitmapState::AddMaskTo(LPBITMAPINFO lpDeepInfo, LPBYTE lpDeepBits)
{
	// Create the next state
	ProcessBitmapState deep;
	deep.InitPipelineSettings(this);

	// Don't process if the current state is invalid
	if (!m_bOK)
		return deep;		// IsOK = FALSE

	ERROR3IF(GetDepth()!=8, "Error");
	ERROR3IF(lpDeepInfo->bmiHeader.biBitCount != 32, "Error");
	ERROR3IF(lpDeepInfo->bmiHeader.biWidth != (INT32)GetWidth(), "Error");
	ERROR3IF(lpDeepInfo->bmiHeader.biHeight != (INT32)GetHeight(), "Error");
	if (GetDepth()!=8) return deep;
	if (lpDeepInfo->bmiHeader.biBitCount != 32) return deep;
	if (lpDeepInfo->bmiHeader.biWidth != (INT32)GetWidth()) return deep;
	if (lpDeepInfo->bmiHeader.biHeight != (INT32)GetHeight()) return deep;

	deep.m_lpInfo = lpDeepInfo;
	deep.m_lpBits = lpDeepBits;
	if (!deep.IsOK())
		return deep;		// IsOK = FALSE

	UINT32 uWidth  = GetWidth();
	UINT32 uHeight = GetHeight();
	UINT32 uByteWidth8 = DIBUtil::ScanlineSize(uWidth, 8);
	UINT32 uByteWidth32 = DIBUtil::ScanlineSize(deep.GetWidth(), deep.GetDepth())/sizeof(UINT32);	// width in words

	UINT32* pDst = (UINT32*)lpDeepBits;
	LPBYTE pSrc = m_lpBits;
	UINT32 i;
	UINT32 j;
	for ( j=0 ; j<uHeight ; j++ )
	{
		for ( i=0 ; i<uWidth ; i++ )
		{
			pDst[i] = (pDst[i]&0xFF000000) | pSrc[i] << 24;
		}
		pDst += uByteWidth32;
		pSrc += uByteWidth8;
	}

	return deep;
}




/********************************************************************************************

>	ProcessBitmapState ProcessBitmapState::Contour8BPP(double dContourWidth)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/04/2005
	Inputs:		-
	Outputs		-
	Returns:	-
	Purpose:	Contour this state return resulting state
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

ProcessBitmapState ProcessBitmapState::Contour8BPP(double dContourWidth)
{
	// Create the next state
	double OffsetX = 0;
	double OffsetY = 0;
	ProcessBitmapState contour;
	contour.InitPipelineSettings(this);

	// Don't process if the current state is invalid
	if (!m_bOK)
		return contour;		// IsOK = FALSE

	ERROR3IF(GetDepth()!=8, "Contour8BPP only copes with 8BPP inputs at the moment");
	ERROR3IF(dContourWidth>=0, "Contour8BPP only copes with inner contours at the moment");

	// Now generate an Inner contour of half the feather width
	contour.m_bOK = CBitmapShadow::ContourBitmap(m_lpInfo,
										   m_lpBits,
										   fabs(dContourWidth),
										   (dContourWidth<0),
										   0xFE,			// Threshold
										   &contour.m_lpInfo,
										   &contour.m_lpBits,
										   &OffsetX,
										   &OffsetY
										   );
	ERROR3IF(!contour.IsOK(), "ContourBitmap failed in RenderShadowedNodeOffscreen");
	if (!contour.IsOK())
		return contour;

	// If the contour produced a NULL bitmap
	// Just return a copy of the current state...
	if (contour.m_lpInfo==NULL)
		return *this;

	contour.m_XOffset += OffsetX;	// Assume ContourBitmap has set sense correctly for inside/outside contouring
	contour.m_YOffset += OffsetY;

	return contour;
}




/********************************************************************************************

>	ProcessBitmapState ProcessBitmapState::Blur8BPP(double dBlurWidth)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/04/2005
	Inputs:		-
	Outputs		-
	Returns:	-
	Purpose:	Contour the source bitmap
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

ProcessBitmapState ProcessBitmapState::Blur8BPP(double dBlurWidth)
{
	// Create the next state
	ProcessBitmapState blur;
	blur.InitPipelineSettings(this);

	// Don't process if the current state is invalid
	if (!m_bOK)
		return blur;		// IsOK = FALSE

	ERROR3IF(GetDepth()!=8, "Blur8BPP only copes with 8BPP inputs at the moment");

	// Now blur
	UINT32 DWordWidth = DIBUtil::ScanlineSize(GetWidth(), GetDepth());
	// Alloc a destination DIB for the convolution.
	UINT32 uBlurDiameter = UINT32(dBlurWidth+0.5)-1 ;
	blur.AllocDIB(GetWidth()-uBlurDiameter, GetHeight()-uBlurDiameter, 8);
	if (!blur.IsOK())
		return blur;		// IsOK = FALSE

	CBitmapShadow::Blur8BppBitmap(&m_lpInfo->bmiHeader, m_lpBits, &blur.m_lpInfo->bmiHeader, blur.m_lpBits, DWordWidth, dBlurWidth/2);

	// Blur result is offset by blur radius
	// So to position blur bitmap correctly we must move it down and left by blur radius
	blur.m_XOffset += dBlurWidth/2;
	blur.m_YOffset += dBlurWidth/2;

	return blur;
}




/********************************************************************************************

>	ProcessBitmapState ProcessBitmapState::Expand8BPP(INT32 nPixels, BYTE fill)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/04/2005
	Inputs:		-
	Outputs		-
	Returns:	-
	Purpose:	Expand the source bitmap, filling pixels with specified value
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

ProcessBitmapState ProcessBitmapState::Expand8BPP(INT32 nPixels, BYTE fill)
{
	ProcessBitmapState expand;
	expand.InitPipelineSettings(this);

	// Don't process if the current state is invalid
	if (!m_bOK)
		return expand;		// IsOK = FALSE

	ERROR3IF(GetDepth()!=8, "Expand8BPP only copes with 8BPP inputs at the moment");

	expand.AllocDIB(GetWidth()+nPixels*2, GetHeight()+nPixels*2, 8);
	if (!expand.IsOK())
		return expand;		// IsOK = FALSE

	UINT32 uSrcWidth8  = GetWidth();
	UINT32 uSrcHeight8 = GetHeight();
	UINT32 uSrcByteWidth8 = DIBUtil::ScanlineSize(uSrcWidth8, 8);

	UINT32 uExWidth8  = expand.GetWidth();
	UINT32 uExHeight8 = expand.GetHeight();
	UINT32 uExByteWidth8 = DIBUtil::ScanlineSize(uExWidth8, 8);

	LPBYTE pSrc = m_lpBits;
	LPBYTE pDst = expand.m_lpBits + uExByteWidth8*nPixels + nPixels;
	UINT32 i,j;

	memset(expand.m_lpBits, fill, uExByteWidth8*uExHeight8);

	for ( j=0 ; j<uSrcHeight8 ; j++ )
	{
		for ( i=0 ; i<uSrcWidth8 ; i++ )
			pDst[i] = pSrc[i];

		pSrc += uSrcByteWidth8;
		pDst +=  uExByteWidth8;
	}

	// Resulting bitmap is bigger than original by nPixels all round so corner moves down and left
	expand.m_XOffset -= (double)nPixels;
	expand.m_YOffset -= (double)nPixels;

	return expand;
}




/********************************************************************************************

>	ProcessBitmapState ProcessBitmapState::AttachToDoc()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/04/2005
	Inputs:		-
	Outputs		-
	Returns:	-
	Purpose:	Expand the source bitmap, filling pixels with specified value
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

#ifdef DEBUG
void ProcessBitmapState::AttachToDoc(String_256 strName)
{
	if (m_lpInfo != NULL && m_lpBits != NULL)
	{
		CWxBitmap*		wbmp = new CWxBitmap(m_lpInfo, m_lpBits);
		KernelBitmap*	kbmp = new KernelBitmap(wbmp, TRUE);
		if (kbmp->GetBPP()==8)
		{
			LPRGBQUAD pPalette = kbmp->GetPaletteForBitmap();
			for ( INT32 i=0 ; i<0x100 ; i++ )
				(UINT32&)pPalette[i] = i*0x010101 ;
				
			kbmp->SetAsGreyscale();
		}
		kbmp->AttachDebugCopyToCurrentDocument(strName);
		wbmp->BMBytes = ((CWxBitmap*)OILBitmap::Default)->BMBytes;
		delete kbmp;
	}
}
#endif

#endif
