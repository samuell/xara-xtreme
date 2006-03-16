// $Id: nodecont.h 662 2006-03-14 21:31:49Z alex $
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
// NodeShadowController class

#ifndef	INC_NODESHADOWCONTROLLER
#define	INC_NODESHADOWCONTROLLER

#ifdef BUILDSHADOWS

// Define this variable to upgrade from X1-style shadow rendering
// to X2-Capture-based rendering
// Declare smart memory handling in Debug builds
//#define NEW_SHADOW_RENDER

// the bitmap shadow's type
enum ShadowType
{
	SHADOWTYPE_NONE = 0,
	SHADOWTYPE_WALL,
	SHADOWTYPE_FLOOR,
	SHADOWTYPE_GLOW,
	SHADOWTYPE_FEATHER
} ;

// The shadow field mask for our special version of CompareState
typedef enum
{
	smOFFSETX		= 0x0001,
	smOFFSETY		= 0x0002,
	smPENUMBRA		= 0x0004,
	smNAME			= 0x0008,
	smTYPE			= 0x0010,
	smFLOORANGLE	= 0x0020,
	smFLOORHEIGHT	= 0x0040,
	smSCALE			= 0x0080,
	smGLOWWIDTH		= 0x0100,
	smFEATHERWIDTH	= 0x0200,
	smBIASGAIN		= 0x0400,
	smDARKNESS		= 0x0800
} ShadowMaskBits;

const UINT32 SHADOWMASK_ALL = 0xFFF;

typedef struct tagShadowFieldMask
{
	tagShadowFieldMask()
	{
		OffsetX			= FALSE;
		OffsetY			= FALSE;
		Penumbra		= FALSE;
		Name			= FALSE;
		Type			= FALSE;
		FloorAngle		= FALSE;
		FloorHeight		= FALSE;
		Scale			= FALSE;
		GlowWidth		= FALSE;
		FeatherWidth	= FALSE;
		BiasGain		= FALSE;
		Darkness		= FALSE;
	}

	tagShadowFieldMask(UINT32 flagbits)
	{
		OffsetX			= ((flagbits & smOFFSETX) == smOFFSETX);
		OffsetY			= ((flagbits & smOFFSETY) == smOFFSETY);
		Penumbra		= ((flagbits & smPENUMBRA) == smPENUMBRA);
		Name			= ((flagbits & smNAME) == smNAME);
		Type			= ((flagbits & smTYPE) == smTYPE);
		FloorAngle		= ((flagbits & smFLOORANGLE) == smFLOORANGLE);
		FloorHeight		= ((flagbits & smFLOORHEIGHT) == smFLOORHEIGHT);
		Scale			= ((flagbits & smSCALE) == smSCALE);
		GlowWidth		= ((flagbits & smGLOWWIDTH) == smGLOWWIDTH);
		FeatherWidth	= ((flagbits & smFEATHERWIDTH) == smFEATHERWIDTH);
		BiasGain		= ((flagbits & smBIASGAIN) == smBIASGAIN);
		Darkness		= ((flagbits & smDARKNESS) == smDARKNESS);
	}

	BOOL OffsetX		: 1;
	BOOL OffsetY		: 1;
	BOOL Penumbra		: 1;
	BOOL Name			: 1;
	BOOL Type			: 1;
	BOOL FloorAngle		: 1;
	BOOL FloorHeight	: 1;
	BOOL Scale			: 1;
	BOOL GlowWidth		: 1;
	BOOL FeatherWidth	: 1;
	BOOL BiasGain		: 1;
	BOOL Darkness		: 1;
} ShadowFieldMask;

// constants for the maximum floor-shadow height and angle.
#define MAXSHADOWHEIGHT 1.5
#define MAXSHADOWANGLE (69 * (PI / 180.0))

//#include "control.h"
#include "pathpcs.h"
#include "nodepostpro.h"

class RenderRegion;
class String_32;
class NodeRect; 
class CCAttrMap;

class NodeShadow;

class ExtendParams;

class AttrFillGeometry;

/********************************************************************************************

>	class NodeShadowController : public NodeEffect

	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com> 
	Created:	20/11/96
	Purpose:	

********************************************************************************************/
class NodeShadowController : public NodeEffect
{
	CC_DECLARE_DYNAMIC( NodeShadowController )    
public: 
	
	NodeShadowController(); 
	~NodeShadowController();
	
	NodeShadowController(Node* ContextNode,  
			  AttachNodeDirection Direction,    
			  BOOL Locked=FALSE, 
			  BOOL Mangled=FALSE,  
			  BOOL Marked=FALSE, 
			  BOOL Selected=FALSE    
		     ); 
	
	// Copy functions
	virtual Node* SimpleCopy(); 
	void CopyNodeContents(NodeShadowController* pNewNode);
	virtual void PolyCopyNodeContents(NodeRenderable* pNodeCopy);

	// virtual Functions to deal with bounding boxes
	virtual DocRect GetBlobBoundingRect();
	virtual void Transform(TransformBase&);

	// Virtual rendering functions	     
	virtual void PreExportRender( RenderRegion* pRender ); 
	virtual BOOL ExportRender( RenderRegion* pRender ); 
	virtual void RenderEorDrag( RenderRegion* );
	virtual BOOL ChildrenAreEorDragRenderedByMe() { return TRUE; }

	// Gets the bounding rect to be used when eor dragging
	DocRect GetEorDragBoundingRect() { return GetInsideBoundingRect(); }
	// virtual void RenderObjectBlobs(RenderRegion* pRender);
	virtual void RenderTinyBlobs(RenderRegion* pRender);

	// Other virtual functions
	virtual String Describe(BOOL Plural, BOOL Verbose); 
    virtual UINT32 GetNodeSize() const; 			// Returns size of node in bytes

	// Fuctions that allows ops and other nodes to inform this node of events on it's children
	virtual ChangeCode OnChildChange(ObjChangeParam* pParam);
	virtual BOOL AllowOp(ObjChangeParam *pParam, BOOL SetOpPermissionState = TRUE,
												 BOOL DoPreTriggerEdit = TRUE);

	// Functions for converting nodes into other types
  	virtual BOOL CanBecomeA(BecomeA* pBecomeA);
	virtual BOOL DoBecomeA(BecomeA* pBecomeA);

	BOOL ApplyShadowTransform(Path* pTargetPath);

	// Functions to deal with messages
	virtual BOOL OnClick( DocCoord, ClickType, ClickModifiers, Spread* );
 
	// Version 2 file format functions
	virtual BOOL WritePreChildrenWeb(BaseCamelotFilter* pFilter);
	virtual BOOL WritePreChildrenNative(BaseCamelotFilter* pFilter);

	NodeShadow* GetShadow() const;
	NodeRenderableInk* GetShadowedNode() const;

	// alternative virtual fn. to get shadowed node
	virtual NodeRenderableInk* GetInkNodeFromController() const;

// Access functions for member variables
	MILLIPOINT GetPenumbraWidth() const	{return m_PenumbraWidth;}
	void SetPenumbraWidth(MILLIPOINT NewWidth)	{ if (NewWidth > 0) m_PenumbraWidth = NewWidth;
												  else m_PenumbraWidth = 0; }

	MILLIPOINT GetOffsetX() const { return m_OffsetX; }
	MILLIPOINT GetOffsetY() const { return m_OffsetY; }
	void SetOffsetX(MILLIPOINT xx) { m_OffsetX = xx; }
	void SetOffsetY(MILLIPOINT yy) { m_OffsetY = yy; }

	// The same idea at present, so return the same value
	// ShadowType	GetRenderMethod()	{ return m_ShadowType; } 
	
	void SetShadowType(ShadowType NewType)		{ m_ShadowType = NewType; }
	ShadowType GetShadowType() { return m_ShadowType; }

	String_256 GetShadowName() { return m_sShadowName; }
	void SetNewShadowName(String_256 sNewName) { m_sShadowName = sNewName; }

	BOOL HasShadowSelectionChanged() { return m_ShadowSelectionChanged; }
	void ResetShadowSelectionChange() { m_ShadowSelectionChanged = FALSE; }

	virtual BOOL PostImport();			// function called after import
	BOOL PostDuplicate(UndoableOperation* pOp);		// function called after node has been copied.

	BOOL RegenerateNode(UndoableOperation * pOp, BOOL bCacheRender = FALSE,
		BOOL bInformParents = TRUE);

	// hit testing on children isn't promoted
	virtual BOOL				PromoteHitTestOnChildrenToMe() const { return FALSE; }
	virtual BOOL				ShouldITransformWithChildren() const { return TRUE; }
	virtual BOOL				CanSelectAsCompoundParent() {return FALSE;}
	virtual BOOL				NeedsTransparency() const {return TRUE;}
	virtual BOOL				IsController() { return TRUE;}
	virtual BOOL				IsAShadowController() const { return TRUE;}
	virtual BOOL				AllowSelectInside() const {return TRUE;}
	virtual BOOL				IsPassThroughEffect() const {return TRUE;}	// User can see original object through this effect

	// Karim 14/04/2000 - helper method for marquee selection.
	virtual void SelectInRect(const DocRect& Rect, SelStateAction st);

	virtual BOOL				CompareState(NodeEffect* pPPNode);
	virtual BOOL				CompareState(NodeEffect* pPPNode, ShadowFieldMask mask);
	virtual DocRect				GetChildBoundingRect(BOOL bIncludeAttrs = TRUE);
	virtual void				InvalidateBoundingRect(BOOL InvalidateChildBounds = FALSE);

#ifdef NEW_SHADOW_RENDER
public:
	virtual SubtreeRenderState	RenderSubtree(RenderRegion* pRender, Node** ppNextNode = NULL, BOOL bClip = TRUE);
	virtual void				RenderAfterSubtree(RenderRegion* pRender);
//	virtual void				Render(RenderRegion* pRender);
	virtual BOOL				ReleaseCached(BOOL bAndParents = TRUE,
											  BOOL bAndChildren = TRUE,
											  BOOL bSelf = TRUE,
											  BOOL bAndDerived = TRUE);
	virtual BOOL				IsCapturingChildren();
	virtual void				ReleaseStackedShadows();
	virtual double				GetPixelsPerInch();
	virtual double				GetPixelWidth();
	NodeShadowController*		FindBottomShadowController();

protected:
	virtual BOOL				FindCachedEffect(CBitmapCache* pBitmapCache);
	virtual BOOL				RenderCachedEffect(CBitmapCache* pBitmapCache, RenderRegion* pRenderRegion);

private:
	BOOL						IsTopShadowController();
	NodeShadow*					FindBottomShadow();
	NodeShadow*					FindShadowAbove(NodeShadow* pCurrentShadow);
	BOOL						IsInShadowStack();

#endif


public:
	// static ShadowType RenderMethod;

// 	#ifdef _DEBUG
 	void ShowDebugTreeDetails() const;
	void GetDebugDetails( StringBase* Str );
//	#endif

	// a couple of conversion functions for storing/retrieving the double value
	// of the floor-shadow angle in a INT32 without loss of too much precision.
	INT32		FloorAngleToINT32(double Angle);
	double		FloorAngleFromINT32(INT32 Angle);

	double		GetFloorShadowAngle()	{ return m_FloorShadowAngle;	}
	float		GetFloorShadowHeight()	{ return m_FloorShadowHeight;	}
	DocCoord	GetFloorShadowVector();
	void		SetFloorShadowAngle(double Angle)	{ m_FloorShadowAngle = Angle;	}
	void		SetFloorShadowHeight(float Height)	{ m_FloorShadowHeight = Height;	}
	void		SetFloorShadowVector(const DocCoord& dcControlVector);

	DocCoord	GetWallShadowOffset();
	float		GetWallShadowScale()	{ return m_Scale;	}
	void		SetWallShadowOffset(const DocCoord& dcOffset, BOOL UpdateNodeShadow);
	void		SetWallShadowScale(float NewScale)	{ m_Scale = NewScale; }

	MILLIPOINT GetGlowWidth() { return m_GlowWidth; }
	void SetGlowWidth(MILLIPOINT NewWidth) { m_GlowWidth = NewWidth; }

	MILLIPOINT GetFeatherWidth() { return m_FeatherWidth; }
	void SetFeatherWidth(MILLIPOINT NewWidth) { m_FeatherWidth = NewWidth; }

	UINT32 GetShadowTransp();

	void ResetShadowSettings();

	INT32 ComplexHide(UndoableOperation* pOp, Node* pNextInRange);

//	virtual BOOL IsValidEffectAttr(CCRuntimeClass* pAttrClass) const {return FALSE;}
	virtual BOOL IsValidEffectAttr(NodeAttribute* pAttr) const {return FALSE;}
	virtual NodeRenderableInk* GetObjectToApplyTo(CCRuntimeClass* AttrType);

	// Karim MacDonald 25/11/1999
	// virtual Extend functions; see declaration/implementation in Node.
public:
	virtual BOOL IsTypeExtendible() const { return TRUE; }
	virtual DocRect ValidateExtend(const ExtendParams& ExtParams);
	virtual void Extend(const ExtendParams& ExtParams);

	// the blend stuff for shadows
	virtual BOOL EndBlendStep(BlendNodeParam* pParam);
	virtual NodeRenderableInk* CreateTreeFromNodeToBlend(NodeRenderableInk* pNode, CCAttrMap* pAttrMap);

protected:
	// blends the parameters, given the start & end nodes
	BOOL BlendParameters (NodeShadowController * pStart, NodeShadowController * pEnd, BlendNodeParam * pParam, CCAttrMap* pMap);
	// blends the parameters, given the start node (NOTE:  where possible assumes sensible values)
	BOOL BlendParameters(NodeShadowController * pStart, BlendNodeParam * pParam);

// Member variables
protected:
	MILLIPOINT  m_OffsetX;
	MILLIPOINT  m_OffsetY;
	MILLIPOINT	m_PenumbraWidth;
	
	BOOL		m_ShadowSelectionChanged;

	String_256	m_sShadowName;
	ShadowType	m_ShadowType;

	DocRect		m_PreviousRect;

	double		m_FloorShadowAngle;
	
	// this is relative to the size of the original object
	// (i.e. 1.0 = full height of the original image)
	float	m_FloorShadowHeight;
	float   m_Scale;						// the scale of the wall shadow
	MILLIPOINT m_GlowWidth;					// the width of the glow
	MILLIPOINT m_FeatherWidth;				// The width of the feather

	BOOL m_PerformedExtend;

	/////////////////////////////////
	// stuff to do with the rendering of eor dragged outlines for glows
	Path m_GlowDragPath;


public:
	// the default settings which should be used when creating a shadow with no extra info.

	static const ShadowType	DefaultType;
	static const MILLIPOINT	DefaultWallOffsetX;
	static const MILLIPOINT	DefaultWallOffsetY;
	static const MILLIPOINT	DefaultGlowWidth;
	static const MILLIPOINT	DefaultFeatherWidth;
	static const double		DefaultFloorAngle;
	static const float		DefaultFloorHeight;
	static const MILLIPOINT	DefaultBlurSize;
	static const double		DefaultDarkness;
	static const double		DefaultScale;

	static MILLIPOINT		DefaultPixelsPerInch;
};

/***********************************************************************************************

>	class ShadowNodeTreeFactory : public CompoundNodeTreeFactory

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/5/99
	Purpose:	Creates a new shadow node with the given parameters

***********************************************************************************************/
class ShadowNodeTreeFactory : public CompoundNodeTreeFactory
{
	CC_DECLARE_DYNCREATE(ShadowNodeTreeFactory);

public:
	ShadowNodeTreeFactory();
	~ShadowNodeTreeFactory();

	// given a list of NodeListItems, shadows them
	virtual NodeCompound * CreateNode(List *pList, UndoableOperation * pOp = NULL);

	// access functions
	MILLIPOINT GetPenumbraWidth() const	{return m_PenumbraWidth;}
	// CGS:  the penumbra should NEVER be able to go negative!
	void SetPenumbraWidth(MILLIPOINT NewWidth)	{ if (NewWidth > 0) m_PenumbraWidth = NewWidth;
												  else m_PenumbraWidth = 0; }

	MILLIPOINT GetOffsetX() const { return m_OffsetX; }
	MILLIPOINT GetOffsetY() const { return m_OffsetY; }
	void SetOffsetX(MILLIPOINT xx) { m_OffsetX = xx; }
	void SetOffsetY(MILLIPOINT yy) { m_OffsetY = yy; }

	void SetShadowType(ShadowType NewType)		{ m_ShadowType = NewType; }
	ShadowType GetShadowType() { return m_ShadowType; }

	void SetFloorShadowAngle(double Angle) { m_FloorShadowAngle = Angle; }
	void SetFloorShadowHeight(float Height) { m_FloorShadowHeight = Height; }
	double GetFloorShadowAngle() { return m_FloorShadowAngle; }
	float GetFloorShadowHeight() { return m_FloorShadowHeight; }

	float GetWallShadowScale() { return m_Scale; }
	void  SetWallShadowScale(float NewScale) { m_Scale = NewScale; }

	MILLIPOINT GetGlowWidth() { return m_GlowWidth; }
	void SetGlowWidth(MILLIPOINT GlowWidth) { m_GlowWidth = GlowWidth; }

	MILLIPOINT GetFeatherWidth() { return m_FeatherWidth; }
	void SetFeatherWidth(MILLIPOINT FeatherWidth) { m_FeatherWidth = FeatherWidth; }

	// sets up the attribute map for the shadow node
	void SetUpAttributeList(Node * pShadow);

private:
	ShadowType	m_ShadowType;

	MILLIPOINT  m_OffsetX;
	MILLIPOINT  m_OffsetY;
	MILLIPOINT	m_PenumbraWidth;
	double		m_FloorShadowAngle;
	float		m_FloorShadowHeight;
	float		m_Scale;
	MILLIPOINT	m_GlowWidth;
	MILLIPOINT	m_FeatherWidth;

	List m_AttributeList;
} ;

#endif	// BUILDSHADOWS
 
#endif	// include
