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

#ifndef INC_NODEBEVEL
#define INC_NODEBEVEL

#include "nodepath.h"
#include "beveler.h"

// file input & output stuff
#include "camfiltr.h"
#include "cxftags.h"
#include "cxfile.h"
#include "cxfrech.h"
#include "cxfrec.h"
#include "attr.h"
#include "ink.h"
//#include "grndbmp.h"

//#ifdef BUILDSHADOWS

class NodeBevelController;
class ExtendParams;
class SumAllPathsElem;
class NodeCompound;
class BitmapTranspFillAttribute;

/***********************************************************************************************

>	class NodeBevel : public NodePath

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/98
	Purpose:	Does the bevelling

***********************************************************************************************/

class NodeBevel : public NodeRenderableInk
{
	// Give my name out in memory dumps
	CC_DECLARE_DYNCREATE(NodeBevel);

public:
	NodeBevel();
	~NodeBevel();

	NodeBevel(Node* ContextNode,  
				AttachNodeDirection Direction,  
				BOOL Locked = FALSE, 
				BOOL Mangled = FALSE,  
				BOOL Marked = FALSE, 
				BOOL Selected = FALSE    
				) ;

	// copy this node
	virtual Node* SimpleCopy(); 
	void CopyNodeContents(NodeBevel* pNewNode);
	virtual void PolyCopyNodeContents(NodeRenderable* pNodeCopy);

	// just does bevelling (generation of paths & the bitmap)!
	BOOL GenerateBevel();
	
	// generates the bevel from a particular node list
	BOOL GenerateBevelForBlendStep(SumAllPathsElem* pElem, CCAttrMap * pMap);

	// set up all the variables on myself & my parent by finding the appropriate
	// attributes
	BOOL SetupVariables(CCAttrMap * pMap = NULL);

	// get the bounding rect of me
	virtual DocRect GetBoundingRect(BOOL DontUseAttrs = FALSE, BOOL HitTest = FALSE);
	virtual DocRect GetBlobBoundingRect();
	virtual void SetBoundingRect(DocRect rect) { m_SelectedRect = rect; }

	// rendering functions
	virtual void RenderBitmap(RenderRegion* pRender);
	virtual void Render(RenderRegion* pRender);

	virtual void Transform( TransformBase& Trans );
	
	// we don't want to have brush attrs applied to us.
	virtual BOOL RequiresAttrib(CCRuntimeClass* pAttribClass, BOOL Search = FALSE);
	virtual BOOL RequiresAttrib(NodeAttribute* pAttr, BOOL Search = FALSE);
	virtual BOOL CanAttrBeAppliedToMe(CCRuntimeClass* pAttribClass);

	virtual NodeCompound* GetParentController() const;

	// no, I'm actually a bevel node, even though I'm derived from the NodePath class
	virtual BOOL IsNodePath() const;

	virtual BOOL IsABevel() const { return TRUE; } 

	// deletes all data & bitmaps
	void DeleteCache();
	void DeleteCachedBitmap();

	// calculate all the bevel paths
	BOOL CreateBevelPaths();

	virtual void RenderEorDrag( RenderRegion* rr );
	
	// makes sure that on the next render, the bevel is regenerated
	void InvalidateMe() {};

	virtual BOOL NeedsTransparency() const;

	void DisableRendering() { m_bStopRender = TRUE; }
	void EnableRendering()  { m_bStopRender = FALSE; }
	BOOL IsRenderingStopped() { return m_bStopRender; }

	virtual BOOL DoBecomeA(BecomeA* pBecomeA);
	virtual BOOL CanBecomeA(BecomeA* pBecomeA);

	virtual String Describe(BOOL Plural, BOOL Verbose = TRUE);

	virtual BOOL NeedsParent(Node* pNode) const;

	BOOL PostDuplicate(UndoableOperation* pOp);

	BOOL AllowOp(ObjChangeParam *pParam, BOOL SetOpPermissionState,
										 BOOL DoPreTriggerEdit = TRUE);

	DocRect GetPathBoundingRect() { return m_Path.GetBoundingRect(); }

	virtual BOOL WritePreChildrenWeb(BaseCamelotFilter* pFilter);
	virtual BOOL WritePreChildrenNative(BaseCamelotFilter* pFilter);

	virtual BOOL ExportRender ( RenderRegion *pRegion );
	virtual void PreExportRender ( RenderRegion *pRegion );

	virtual void RenderTinyBlobs(RenderRegion* pRender);

	BitmapFillAttribute* GetBitmapFillAttribute ( void );

	virtual BOOL OnNodePopUp(Spread* pSpread, DocCoord PointerPos, ContextMenu* pMenu);

	BOOL AmICached() { return m_bCached; }

	// Karim MacDonald 25/11/1999
	// virtual Extend functions; see declaration/implementation in Node.
	virtual BOOL IsTypeExtendible() const { return TRUE; }
	virtual DocRect ValidateExtend(const ExtendParams& ExtParams);
	virtual void Extend(const ExtendParams& ExtParams);

	// used in the shadowing code to ensure that when the bitmap is created it's
	// just black-on-white
	void SetRenderBitmaps(BOOL b) { m_bRenderBitmaps = b; }
	BOOL GetRenderBitmaps() { return m_bRenderBitmaps; }

	// used by the glow shadow, to tell the node whether to zero the line width or not
	void SetZeroLineWidths(BOOL b) { m_bZeroLineWidth = b; }

	// gets the bounding rect of my original objects
	DocRect GetInsideBoundingRect();

	// re-renders the bevel bitmap given a particular light angle
	// - the beveler MUST be initialised !
	// the flag is to indicate whether to resample the bitmap i.e. to
	// generate the antialiased bitmap or not
	BOOL ReRenderBevelBitmap(BOOL bAntialias);

	virtual INT32 EstimateNodeComplexity (OpParam* details);

	// This returns the outer path for the bevel as a whole shape, NOT with the cut though!
	Path* GetOuterBevelPath() { return &m_OuterBevelPath; } 
	Path* GetBevelPath() { return &m_Path; }

	void DoRender(BOOL DoRender = TRUE) { m_DoRender = DoRender; }

	KernelBitmap* GetBevelBitmap() const { return m_pBevelBitmap; }

	KernelBitmap* CreateBitmapCopy(double DPI = -1.0, BOOL CheckFillFlags = TRUE);

	void SetConvertingFlag(BOOL Set = FALSE) { m_AmConvertingToShapes = Set; }

	void GetDebugDetails(StringBase* Str);

public:
	// the data members for the bevel
	// public for easy access
	Path InkPath;

	BOOL m_MustRegenOnChildChange;

	INT32 m_BevelType;
	INT32 m_Indent;
	
	double m_LightAngle;
	BOOL m_bOuter;
	INT32 m_Contrast;
	double m_Tilt;

	BOOL m_IsABlendStepBevel;

	// points determining the fill control points
	DocCoord m_BMPCentre;
	DocCoord m_BMPPoint1;
	DocCoord m_BMPPoint2;

	// the rect of the (original) selection
	DocRect m_SelectedRect;
	JointType m_JointType;

protected:

	KernelBitmap* CreateCombinedBMPAndTranspFill(RenderRegion* pRegion = NULL);

	// calculates the bitmap size out of m_Path etc
	void CalculateBitmapSize(INT32 * retnWid, INT32 * retnHei, RenderRegion * pRegion=NULL);

	// renders all the original nodes again
	void RenderNodesForPrinting(RenderRegion * pRender);
	
	// renders all the child nodes of the given node, and then renders the node
	void RenderNodeRecursively(Node * pNode, RenderRegion * pRender);

	// sets up the bitmap palette depending on contrast
	void SetUpBitmapPalette(RenderRegion* pRegion = NULL, DocColour FlatColour = COLOUR_BLACK);
	
	// final stage of bevel generation - creation of the bitmap
	// this is public so external processes can use it
	// e.g. contrast changes and light angle changes don't require
	// regeneration of the paths, just the bitmap
	BOOL CreateBevelBitmap();

	// re-renders the bitmap to the correct resolution for the zoom (dynamic)
	BOOL RenderBitmapForDisplay(RenderRegion * pRegion);

	// Reverses the bevel path and adds it to the original to create the new bevel path
	// Also shrinks/Expands the inner/outer path edge by a pixel to get rid of antialiasing
	// effects when overlapping the object
	BOOL ReverseAndShrinkBevelPathIntoInkPath(Path* pBevelPath = NULL, double ZoomValue = 1.0, BOOL IsAnOuter = TRUE);

protected:
	// the bevel path
	Path  m_Path;
	BOOL m_DoRender;

	KernelBitmap * m_pBevelBitmap;
	KernelBitmap * m_pCombiBitmap;

	BitmapFillAttribute* m_pBMPFill;
	BitmapTranspFillAttribute* m_pTranspFill;

	INT32 m_BitmapWidth;
	INT32 m_BitmapHeight;

	DocCoord m_SubPixelOffset;
	DocRect m_PixelAllignedRect;
	BOOL m_IsFlatFilledOnly;
	BOOL m_IsAnAlphaBMP;
	BOOL m_HasTransparencyApplied;
	BOOL m_AmConvertingToShapes;

	double m_LastCachedPrintBMPDPI;
	double m_LastExportedDPI;

	// lock on rendering
	BOOL m_bStopRender;
	BOOL m_DoingRotation;

	// this is the bevel path generated by the path processor for outer bevels (without
	// the hole in it)
	Path m_OuterBevelPath;

	// the pixel sizes used to create the bevel bitmap
	UINT32 m_ViewScale;

	// the shrunk path used in rendering
	Path m_ShrunkPath;
	BOOL m_bLock;
	BOOL m_bRenderBitmaps;

	// indicates we should zero the line width of the path, or not (as the case might be)
	BOOL m_bZeroLineWidth;

	CBeveler * m_pBeveler;

	// whether the original path is closed or not
	// if not, don't do any path shrinking
	BOOL m_bPathIsClosed;

	// flag to indicate that when this node is inside of a blend there's
	// no need to regenerate it - it's already been cached
	BOOL m_bCached;

	// Reverses the bevel path and adds it to the original to create the new bevel path
	// Also shrinks/Expands the inner/outer path edge by a pixel to get rid of antialiasing
	// effects when overlapping the object
	double m_LastZoomFactor;

	// the last pixel size for the bitmap
	UINT32 m_LastPixelSize;
} ;

/***********************************************************************************************

>	class BevelRecordHandler: public CamelotRecordHandler

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/10/98
	Purpose:	Handles the reading of all bevel attribute records in the v2 file format
	SeeAlso:	-

***********************************************************************************************/

class BevelRecordHandler : public CamelotRecordHandler
{
	// Give my name out in memory dumps
	CC_DECLARE_DYNCREATE( BevelRecordHandler );

public:
	// functions that must be implemented.
	BevelRecordHandler();
	~BevelRecordHandler();

	virtual UINT32*	GetTagList();
	virtual BOOL	HandleRecord(CXaraFileRecord* pCXaraFileRecord);

private:
	NodeBevelController * m_pBevelController;
	NodeBevel			* m_pBevelInk;
} ;

/***********************************************************************************************

>	class NodeBevelBegin : public NodeRenderableBounded

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/10/98
	Purpose:	Denotes the beginning of a bevel
	SeeAlso:	-

***********************************************************************************************/

class NodeBevelBegin : public NodeRenderableBounded
{
	// Give my name out in memory dumps
	CC_DECLARE_DYNCREATE(NodeBevelBegin)

public:
	// functions that must be implemented.
	NodeBevelBegin();
	NodeBevelBegin(Node* ContextNode,  
		 AttachNodeDirection Direction, 
		 BOOL Locked=FALSE, 
		 BOOL Mangled=FALSE,  
		 BOOL Marked=FALSE, 
		 BOOL Selected=FALSE
		 );

	void Render(RenderRegion * region);

	DocRect GetBoundingRect(BOOL DontUseAttrs, BOOL HitTest);
	DocRect GetBlobBoundingRect();

	// the bounding rect of me - set at creation !
	DocRect m_Rect;

	virtual void RenderEorDrag( RenderRegion* rr ) { };
	virtual void RenderObjectBlobs(RenderRegion* pRender) { };
	virtual void RenderTinyBlobs(RenderRegion* pRender) { } ;
	virtual BOOL ExportRender ( RenderRegion *pRegion );

	virtual BOOL NeedsParent(Node* pNode) const;

	// can never be selected
	void SetSelected(BOOL Status);

	// don't need to render if there's a bevel path processor already on the stack
	virtual SubtreeRenderState RenderSubtree(RenderRegion* pRender, Node** ppNextNode = NULL, BOOL bClip = TRUE);

protected:
	// copy this node
	virtual Node* SimpleCopy(); 

	// the clip rect of the render region passed to Render() before
	// it was altered to include all children
	DocRect m_RegionClipRect;
} ;


//#endif
#endif
