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

#ifndef NODELIVEFX_H
#define NODELIVEFX_H

//#include "ink.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "nodepostpro.h"
//#include "cxfrech.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "biasgain.h" - in camtypes.h [AUTOMATICALLY REMOVED]

// Define this if you want to pick up the development of the Feather Effect
#define FEATHER_EFFECT
//#include "opliveeffects.h"

PORTNOTE("other","Define dummy IXMLDOMDocumentPtr")
#ifdef EXCLUDE_FROM_XARALX
typedef void		   *IXMLDOMDocumentPtr;
#endif

class CCAttrMap;
class BitmapTranspFillAttribute;

/***********************************************************************************************

>	class LiveEffectRecordHandler : public CamelotRecordHandler

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/09/2004
	Purpose:	Handles the reading of live effects records
	SeeAlso:	-

***********************************************************************************************/

class LiveEffectRecordHandler : public CamelotRecordHandler
{
	// Give my name in memory dumps
	CC_DECLARE_DYNAMIC(LiveEffectRecordHandler);

public:
	LiveEffectRecordHandler() : CamelotRecordHandler() {}
	~LiveEffectRecordHandler() {}

	virtual UINT32*	GetTagList();
	virtual BOOL	HandleRecord(CXaraFileRecord* pCXaraFileRecord);
	virtual BOOL	IsStreamed(UINT32 Tag);
	virtual BOOL	HandleStreamedRecord(CXaraFile* pCXFile, UINT32 Tag, UINT32 Size, UINT32 RecordNumber);

#ifdef XAR_TREE_DIALOG
	virtual void	GetRecordDescriptionText(CXaraFileRecord* pRecord, StringBase* pStr);
#endif
};




/***********************************************************************************************

>	class NodeBitmapEffect : public NodeEffect

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/09/2004
	Purpose:	Bitmap effects nodes are type of Post-Processors. They process the bitmap
				output of their child nodes.

***********************************************************************************************/

class NodeBitmapEffect : public NodeEffect
{
// Give my name out in memory dumps
CC_DECLARE_DYNCREATE(NodeBitmapEffect);

friend class LiveEffectRecordHandler;

public:
	static BOOL Init();

// Prefs
public:
	// Default resolution of live effect bitmaps in pixels per inch
	static UINT32 DefaultLivePixelsPerInch;
	static UINT32 DefaultLockedPixelsPerInch;
	static UINT32 GroupPixelsPerInch;
	static UINT32 ShadowPixelsPerInch;
	static BOOL DefaultLocked;

public:
	NodeBitmapEffect();
	~NodeBitmapEffect();

	NodeBitmapEffect(Node* ContextNode,
				AttachNodeDirection Direction,
				BOOL Locked = FALSE,
				BOOL Mangled = FALSE,
				BOOL Marked = FALSE,
				BOOL Selected = FALSE
			  );

// Overrides
public:
	virtual BOOL				IsBitmapEffect() const {return TRUE;}
	virtual DocRect				GetBlobBoundingRect();
	virtual NodeRenderableInk*	GetInkNodeFromController() const {return FindFirstChildInk();}
	virtual void				Render(RenderRegion* pRender);
	virtual BOOL				DoBecomeA(BecomeA* pBecomeA);
	virtual BOOL				CanBecomeA(BecomeA* pBecomeA);
	virtual BOOL				HasCachedDirectBitmap();
//	virtual BOOL				WriteBeginChildRecordsWeb(BaseCamelotFilter* pFilter);
//	virtual BOOL				WriteBeginChildRecordsNative(BaseCamelotFilter* pFilter);
	virtual BOOL				WriteBoundsRecord(BaseCamelotFilter* pFilter);
	virtual BOOL				WillWriteBounds(BaseCamelotFilter* pFilter) { return(TRUE); }

// Specialist functions for this class
public:
	virtual void				RenderAfterSubtree(RenderRegion* pRender);
	virtual BOOL				GetOriginalBitmap(LPBITMAPINFO* plpInfo, LPBYTE* plpBits, DocRect* pRect);
	virtual void				SetOriginalBitmap(LPBITMAPINFO lpInfo, LPBYTE lpBits, DocRect rect);
	virtual BOOL				GetProcessedBitmap(BOOL bDirect, LPBITMAPINFO* plpInfo, LPBYTE* plpBits, DocRect* pRect) {return FALSE;}
	virtual DocRect				SetProcessedBitmap( LPBITMAPINFO lpInfo,
													LPBYTE lpBits,
													DocRect rect,
													INT32 width,
													INT32 height,
													double xOffset,
													double yOffset,
													double dPixelWidth = 0,
													Matrix* pmatTransform = NULL)
													{return DocRect(0,0,0,0);}
	virtual IXMLDOMDocumentPtr	GetEditList();
	virtual void				SetEditList(IXMLDOMDocumentPtr pNewEditsDoc);
	virtual BOOL				SetDefaultEditList();
	virtual BOOL				IsLockedEffect() {return FALSE;}
	virtual BOOL				HasChanged() const {return m_bHasChangedRecently;}
	virtual BOOL				SetChanged(BOOL bNewFlag = TRUE) {m_bHasChangedRecently = bNewFlag; return TRUE;}
	virtual BOOL				CompareState(NodeEffect* pPPNode);
	virtual BOOL				ProcessBitmap(RenderRegion* pRender,
											  CBitmapCache* pBitmapCache,
											  LPBITMAPINFO pBMPInfo,
											  LPBYTE pBMPBits,
											  DocRect BMPRect,
											  LPBITMAPINFO& pOutputInfo,
											  LPBYTE& pOutputBits,
											  double* pXOffset,
											  double* pYOffset);
	virtual String_64&			GetDisplayName() {return m_strDisplayName;}
	virtual void				SetDisplayName(String_64& strDisplayName) {m_strDisplayName = strDisplayName;}
	virtual KernelBitmap*		GetKernelBitmap(DocCoord* pCoords, BOOL bRetainCached = FALSE) {return NULL;}
	virtual double				GetPixelsPerInch();
	virtual double				GetPixelsPerInchValue() const {return m_dPixelsPerInch;}
	virtual void				SetPixelsPerInch(double dPixelsPerInch);
	virtual BOOL				CanGenerateBitmap();
	virtual BOOL				GetChildDirectBitmap(RenderRegion* pRender,
													LPBITMAPINFO* plpInfo = NULL,
													LPBYTE* plpBits = NULL,
													DocRect* pRect = NULL,
													Matrix* pMat = NULL,
													double* pdRes = NULL
													);
	virtual BOOL				CanSupplyDirectBitmap() {return TRUE;}
	virtual BOOL				GetDirectBitmap(RenderRegion* pRender, LPBITMAPINFO* plpInfo, LPBYTE* plpBits, DocRect* pRect, Matrix* pMat, double* pdRes);
	virtual BOOL				RenderDirectBitmapState(RenderRegion* pRender);
	virtual BOOL				EnableDirectCapture();

protected:
	virtual double				GetPixelWidth() {return 72000.0 / GetPixelsPerInch();}
	virtual DocRect				AdjustPixelOffsets(DocRect inRect, INT32 width, INT32 height, double xOffset, double yOffset, double dPixelWidth = 0);
	virtual BOOL				FindCachedEffect(CBitmapCache* pBitmapCache);
	virtual BOOL				RenderCachedEffect(CBitmapCache* pBitmapCache, RenderRegion* pRenderRegion, BOOL bIgnoreCapture = FALSE);
	virtual BOOL				GetQuickRender() {return Operation::GetQuickRender(this);}
	virtual BOOL				CanGetChildDirectBitmap();

protected:
	IXMLDOMDocumentPtr			m_pEditsDoc;				// The parameter list for XPE processing
		StringVar				m_vstrEdits;				// Hold xml string in memory while we don't have an XMl document class
	double						m_dPixelsPerInch;			// effect resolution of this effect
	BOOL						m_bHasChangedRecently;		// TRUE if this effect has been changed by XaraPhotoEditorHost::UpdateEdit
	String_64					m_strDisplayName;			// The display name of this effect (for use when we can't find it by ID)
	DocRect						m_rectDirectBitmap;			// The "cached" bitmap rect in DirectBitmap space

};




/***********************************************************************************************

>	class NodeLiveEffect : public NodeBitmapEffect

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/09/2004
	Purpose:	Bitmap effects nodes are type of Post-Processors. They process the bitmap
				output of their child nodes.
				They are "Live" in that as soon as their children change they re-process
				the output.

***********************************************************************************************/

class NodeLiveEffect : public NodeBitmapEffect
{
// Give my name out in memory dumps
CC_DECLARE_DYNCREATE(NodeLiveEffect);

friend class LiveEffectRecordHandler;

public:
	NodeLiveEffect();
	~NodeLiveEffect();

	NodeLiveEffect(Node* ContextNode,
				AttachNodeDirection Direction,
				BOOL Locked = FALSE,
				BOOL Mangled = FALSE,
				BOOL Marked = FALSE,
				BOOL Selected = FALSE
			  );

// Overrides
public:
	virtual SubtreeRenderState	RenderSubtree(RenderRegion* pRender, Node** ppNextNode = NULL, BOOL bClip = TRUE);
	virtual String				Describe(BOOL Plural, BOOL Verbose);
	virtual Node*				SimpleCopy();  
    virtual UINT32				GetNodeSize() const;
	virtual void				PolyCopyNodeContents(NodeRenderable* pNodeCopy);
	virtual DocRect				GetBoundingRect(BOOL DontUseAttrs=FALSE, BOOL HitTest=FALSE);
	virtual void				Transform(TransformBase& Trans);
	virtual BOOL				ReleaseCached(BOOL bAndParents = TRUE,
											  BOOL bAndChildren = TRUE,
											  BOOL bSelf = TRUE,
											  BOOL bAndDerived = TRUE);
	virtual BOOL				WritePreChildrenWeb(BaseCamelotFilter* pFilter);
	virtual BOOL				WritePreChildrenNative(BaseCamelotFilter* pFilter);
	virtual NodeRenderableInk*	FindNodeAtPointHelper(const Spread* pSpread, const DocCoord dcPoint);
	virtual BOOL				CanSelectAsCompoundParent() {return FALSE;}					// FALSE if normal effect
	virtual BOOL				PromoteHitTestOnChildrenToMe() const {return FALSE;}		// FALSE if normal effect
	virtual BOOL				ShouldITransformWithChildren() const {return TRUE;}
	virtual BOOL				AllowSelectInside() const {return TRUE;}					// TRUE if normal effect

// Specialist functions for this class
public:
	virtual BOOL				GetProcessedBitmap(BOOL bDirect, LPBITMAPINFO* plpInfo, LPBYTE* plpBits, DocRect* pRect);
	virtual DocRect				SetProcessedBitmap( LPBITMAPINFO lpInfo,
													LPBYTE lpBits,
													DocRect rect,
													INT32 width,
													INT32 height,
													double xOffset,
													double yOffset,
													double dPixelWidth = 0,
													Matrix* pmatTransform = NULL);
	virtual BOOL				IsLockedEffect() {return FALSE;}
	virtual KernelBitmap*		GetKernelBitmap(DocCoord* pCoords, BOOL bRetainCached = FALSE);
	void						RemoveBitmapFromCache();

protected:
	virtual BOOL				WriteLiveEffect(BaseCamelotFilter* pFilter);

protected:
	void						CopyNodeContents(NodeLiveEffect* pCopyOfNode);

private:
	DocRect						m_rectEstimatedBounds;		// Hold on to known bounds of processed bitmap even when we don't have a processed bitmap
	MILLIPOINT					m_lMaxBorder;
};




/***********************************************************************************************

>	class NodeLockedEffect : public NodeBitmapEffect

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/09/2004
	Purpose:	Bitmap effects nodes are type of Post-Processors. They process the bitmap
				output of their child nodes.
				They are "Locked" in that they capture the output once then ignore changes
				to their children (because they cannot reproduce the processing).

***********************************************************************************************/

class NodeLockedEffect : public NodeBitmapEffect
{
// Give my name out in memory dumps
CC_DECLARE_DYNCREATE(NodeLockedEffect);

friend class LiveEffectRecordHandler;

public:
	NodeLockedEffect();
	~NodeLockedEffect();

	NodeLockedEffect(Node* ContextNode,
				AttachNodeDirection Direction,
				BOOL Locked = FALSE,
				BOOL Mangled = FALSE,
				BOOL Marked = FALSE,
				BOOL Selected = FALSE
			  );

// Overrides
public:
	virtual SubtreeRenderState	RenderSubtree(RenderRegion* pRender, Node** ppNextNode = NULL, BOOL bClip = TRUE);
	virtual String				Describe(BOOL Plural, BOOL Verbose);
	virtual Node*				SimpleCopy();  
    virtual UINT32				GetNodeSize() const;
	virtual void				PolyCopyNodeContents(NodeRenderable* pNodeCopy);
	virtual DocRect				GetBoundingRect(BOOL DontUseAttrs=FALSE, BOOL HitTest=FALSE);
	virtual void				Transform(TransformBase& Trans);
	virtual BOOL				WritePreChildrenWeb(BaseCamelotFilter* pFilter);
	virtual BOOL				WritePreChildrenNative(BaseCamelotFilter* pFilter);
	virtual BOOL				CanSelectAsCompoundParent() {return TRUE;}				// FALSE if normal effect
	virtual BOOL				PromoteHitTestOnChildrenToMe() const {return TRUE;}		// FALSE if normal effect
	virtual BOOL				AllowSelectInside() const {return FALSE;}				// TRUE if normal effect
	virtual void				RenderObjectBlobs(RenderRegion* pRender);
	virtual void				RenderTinyBlobs(RenderRegion* pRender);
	virtual void				Render(RenderRegion* pRender);
	virtual KernelBitmap*		EnumerateBitmaps(UINT32 Count);
	virtual NodeRenderableInk*	GetInkNodeFromController() const;
	virtual void				UnlinkNodeFromTree(BaseDocument* pOwnerDoc = NULL);
	virtual BOOL				CanSupplyDirectBitmap() {return HasCachedDirectBitmap();}	// Only supply DirectBitmap if applied to DirectBitmap

protected:
	virtual BOOL				RenderCachedEffect(CBitmapCache* pBitmapCache, RenderRegion* pRender, BOOL bIgnoreCapture = FALSE);

// Specialist functions for this class
public:
	virtual BOOL				GetProcessedBitmap(BOOL bDirect, LPBITMAPINFO* plpInfo, LPBYTE* plpBits, DocRect* pRect);
	virtual DocRect				SetProcessedBitmap( LPBITMAPINFO lpInfo,
													LPBYTE lpBits,
													DocRect rect,
													INT32 width,
													INT32 height,
													double xOffset,
													double yOffset,
													double dPixelWidth = 0,
													Matrix* pmatTransform = NULL);
	virtual BOOL				IsLockedEffect() {return TRUE;}
	virtual BOOL				CanBeUnlocked() const {return !m_bIsDestructive;}
	virtual void				SetLockPermanence(BOOL bDestructive) {m_bIsDestructive = bDestructive;}

	virtual KernelBitmap*		GetKernelBitmap(DocCoord* pCoords, BOOL bRetainCached = FALSE);

protected:
	virtual BOOL				WriteLockedEffect(BaseCamelotFilter* pFilter);

protected:
	void						CopyNodeContents(NodeLockedEffect* pCopyOfNode);

private:
	KernelBitmapRef				m_BitmapRef;				// Use a KernelBitmap to store a 
	DocCoord					m_PGram[3];					// Parallelogram
	BOOL						m_bIsDestructive;			// Is this one permanently locked?
};



/***********************************************************************************************

>	class NodeFeatherEffect : public NodeLiveEffect

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/11/2004
	Purpose:	Feather effects are specialised types of LiveEffect

***********************************************************************************************/

#ifdef FEATHER_EFFECT
class NodeFeatherEffect : public NodeLiveEffect
{
// Give my name out in memory dumps
CC_DECLARE_DYNCREATE(NodeFeatherEffect);

friend class LiveEffectRecordHandler;
friend class OpLiveEffect;

static BOOL ProcessBitmap(LPBITMAPINFO pBMPInfo,
						  LPBYTE pBMPBits,
						  double dPixBlurDiameter,
						  CProfileBiasGain profileFeather);

public:
	NodeFeatherEffect();
	~NodeFeatherEffect();

	NodeFeatherEffect(Node* ContextNode,
				AttachNodeDirection Direction,
				BOOL Locked = FALSE,
				BOOL Mangled = FALSE,
				BOOL Marked = FALSE,
				BOOL Selected = FALSE
			  );

// Overrides
public:
	virtual String				Describe(BOOL Plural, BOOL Verbose);
	virtual Node*				SimpleCopy();  
    virtual UINT32				GetNodeSize() const;
	virtual void				PolyCopyNodeContents(NodeRenderable* pNodeCopy);
	virtual BOOL				WritePreChildrenWeb(BaseCamelotFilter* pFilter);
	virtual BOOL				WritePreChildrenNative(BaseCamelotFilter* pFilter);
	virtual BOOL				ProcessBitmap(RenderRegion* pRender,
											  CBitmapCache* pBitmapCache,
											  LPBITMAPINFO pBMPInfo,
											  LPBYTE pBMPBits,
											  DocRect BMPRect,
											  LPBITMAPINFO& pOutputInfo,
											  LPBYTE& pOutputBits,
											  double* pXOffset,
											  double* pYOffset);
	virtual BOOL				FindCachedEffect(CBitmapCache* pBitmapCache);
	virtual BOOL				RenderCachedEffect(CBitmapCache* pBitmapCache, RenderRegion* pRenderRegion, BOOL bIgnoreCapture = FALSE);
	virtual BOOL				CompareState(NodeEffect* pPPNode);
	virtual BOOL				IsFeatherEffect() {return TRUE;}
	virtual BOOL				CanSupplyDirectBitmap() {return FALSE;}
	virtual BOOL				EnableDirectCapture() {return FALSE;}
//	virtual BOOL				GetDirectBitmap(RenderRegion* pRender, LPBITMAPINFO* plpInfo, LPBYTE* plpBits, DocRect* pRect, Matrix* pMat, double* pdRes);

// Specialist functions for this class
public:
	virtual void				SetFeatherSize(MILLIPOINT newFeatherSize) {m_FeatherSize = newFeatherSize;}
	virtual MILLIPOINT			GetFeatherSize() const {return m_FeatherSize;}
	virtual void				SetProfile(CProfileBiasGain newProfile) {m_Profile = newProfile;}
	virtual CProfileBiasGain	GetProfile() const {return m_Profile;}

protected:
	virtual BOOL				WriteFeatherEffect(BaseCamelotFilter* pFilter);
	BOOL						CreateBitmapTranspFill(KernelBitmap* pFeather,
														DocCoord* pCoords,
														BitmapTranspFillAttribute* BmpTranspFill);
	void						CopyNodeContents(NodeFeatherEffect* pCopyOfNode);
	virtual BOOL				GetQuickRender();

private:
	CProfileBiasGain			m_Profile;
	MILLIPOINT					m_FeatherSize;
};
#endif


/***********************************************************************************************

>	class ProcessBitmapPipeline : public CCObject

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/04/2005
	Purpose:	Holds the state of a bitmap during processing and implements functions to push
				the bitmap through the processing pipeline.

***********************************************************************************************/

#ifdef FEATHER_EFFECT
class ProcessBitmapState : public CCObject
{
public:
	// Default constructor
	ProcessBitmapState()
	{
		m_lpInfo = NULL;
		m_lpBits = NULL;
		m_XOffset = 0;
		m_YOffset = 0;
		m_XPelsPerMeter = 0;
		m_YPelsPerMeter = 0;
		m_bTemp = TRUE;
		m_bOK = FALSE;
	}

	// Build constructor
	ProcessBitmapState(LPBITMAPINFO lpInitialInfo,
						LPBYTE lpInitialBits,
						double InitialXOffset = 0,
						double InitialYOffset = 0,
						BOOL bInitialTempState = FALSE);
	// Copy constructor
	ProcessBitmapState(const ProcessBitmapState& copystate);
	// assignment operator
	ProcessBitmapState& operator=(const ProcessBitmapState& rhsstate);

	// Destructor
	~ProcessBitmapState()
	{
	};

// Methods
public:
	ProcessBitmapState	Create8BPPMask();
	ProcessBitmapState	Contour8BPP(double dContourWidth);
	ProcessBitmapState	Blur8BPP(double dBlurWidth);
	ProcessBitmapState	Expand8BPP(INT32 nPixels, BYTE fill);
	ProcessBitmapState	AddMaskTo(LPBITMAPINFO lpDeepInfo, LPBYTE lpDeepBits);

	void				GetPixelOffsets(double* pXOffset, double* pYOffset) const {*pXOffset = m_XOffset; *pYOffset = m_YOffset;}
	void				GetBitmap(LPBITMAPINFO& pOutputInfo, LPBYTE& pOutputBits, BOOL bTemp = FALSE);
	BOOL				IsOK() const {return m_bOK;}
	void				DeInit();

#ifdef DEBUG
	void				AttachToDoc(String_256 strName);
#endif

// Implementation
protected:
	void				InitPipelineSettings(ProcessBitmapState* const prevstate);
	UINT32				GetWidth() const {return m_lpInfo->bmiHeader.biWidth;}
	UINT32				GetHeight() const {return m_lpInfo->bmiHeader.biHeight;}
	UINT32				GetDepth() const {return m_lpInfo->bmiHeader.biBitCount;}
	UINT32				GetXPelsPerMeter() const {return m_XPelsPerMeter==0 ? m_lpInfo->bmiHeader.biXPelsPerMeter : m_XPelsPerMeter;}
	UINT32				GetYPelsPerMeter() const {return m_YPelsPerMeter==0 ? m_lpInfo->bmiHeader.biYPelsPerMeter : m_YPelsPerMeter;}
	void				SetPelsPerMeter(UINT32 x, UINT32 y);
	BOOL				AllocDIB(UINT32 Width, UINT32 Height, UINT32 Depth);

// Members
private:
	LPBITMAPINFO		m_lpInfo;				// Pointer to bitmap info for this pipeline state
	LPBYTE				m_lpBits;				// Pointer to bitmap data
	double				m_XOffset;				// XOffset from original bottom left corner to bottom left of this state
	double				m_YOffset;				// YOffset...
	BOOL				m_bTemp;				// TRUE if this bitmap can be deleted when another state is assigned to this one
	BOOL				m_bOK;					// TRUE if processing was successful in this state
	UINT32				m_XPelsPerMeter;		// Transport pels per meter through the pipeline when no bitmap is allocated
	UINT32				m_YPelsPerMeter;		// ...

};
#endif


#endif	// NODELIVEFX_H
