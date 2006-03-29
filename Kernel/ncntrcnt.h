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

#ifndef NCNTRCNT_H
#define NCNTRCNT_H

#include "blendatt.h"
#include "pathpcs.h"
#include "cxfrech.h"
#include "biasgain.h"
#include "quality.h"
#include "qualattr.h"
#include "group.h"
//#include "control.h"

class NodeContour;
class CCAttrMap;
class ExtendParams;
class BlendNodeParam;

/***********************************************************************************************

>	class InsetPathPathProcessor : public PathProcessor

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/3/2000
	Purpose:	Makes sure the original nodes aren't rendered, if the active flag is set

***********************************************************************************************/
class InsetPathPathProcessor : public PathProcessor
{
	CC_DECLARE_DYNAMIC(InsetPathPathProcessor);
public:
	InsetPathPathProcessor();
	~InsetPathPathProcessor();

	virtual void ProcessPath(Path *pPath,
							 RenderRegion *pRender,
							 PathShape ShapePath = PATHSHAPE_PATH);

	void SetActive(BOOL b) { m_bActive = b; }
private:
	BOOL m_bActive;
} ;



/***********************************************************************************************

>	class NodeContourController : public NodeGroup

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/8/99
	Purpose:	Contour node controller class

***********************************************************************************************/

class NodeContourController : public NodeGroup
{
	// Give my name out in memory dumps
	CC_DECLARE_DYNCREATE(NodeContourController);

public:
	NodeContourController();

	NodeContourController(Node* ContextNode,  
				AttachNodeDirection Direction,  
				BOOL Locked = FALSE, 
				BOOL Mangled = FALSE,  
				BOOL Marked = FALSE, 
				BOOL Selected = FALSE    
				) ;

	~NodeContourController();

	// Copy functions
	virtual Node* SimpleCopy(); 
	void CopyNodeContents(NodeContourController* pNewNode);
	virtual void PolyCopyNodeContents(NodeRenderable* pNodeCopy);

	// get the bounding rect of me
	virtual DocRect GetBoundingRect(BOOL DontUseAttrs = FALSE, BOOL HitTest = FALSE);
	virtual DocRect GetBlobBoundingRect();

	// Karim 14/04/2000 - helper method for marquee selection.
	virtual void SelectInRect(const DocRect& Rect, SelStateAction st);

	virtual void PreExportRender( RenderRegion* pRender ); 
	virtual BOOL ExportRender( RenderRegion* pRender ); 
	virtual void Render(RenderRegion* pRender);

	virtual void RenderTinyBlobs(RenderRegion* pRender);
	
	virtual BOOL DoBecomeA(BecomeA* pBecomeA);
	virtual BOOL CanBecomeA(BecomeA* pBecomeA);

	virtual NodeRenderableInk * CreateTreeFromNodeToBlend(NodeRenderableInk * pNode, 
		CCAttrMap * pAttrMap);

	// compound nodes overrides
	virtual BOOL RegenerateNode(UndoableOperation * pOp = NULL,
		BOOL bCacheRender = FALSE, BOOL bInformParents = TRUE);

	virtual ChangeCode OnChildChange(ObjChangeParam* pParam);
	
	// when the user clicks on a child node, this tells the hit testing code
	// whether to promote the hit test to this node
	virtual BOOL PromoteHitTestOnChildrenToMe() const { return FALSE; }

	// this indicates whether node should transform with its children
	virtual BOOL ShouldITransformWithChildren() const { return TRUE; }

	// retrieves this node's contour child
	NodeContour * GetContourNode();

	void SetNumberOfSteps(UINT32 Steps) { m_Steps = Steps; }
	void SetWidth(INT32 Width)		  { m_Width = Width; }

	void SetColourBlendType(ColourBlendType Type) { m_BlendType = Type; }
	ColourBlendType GetColourBlendType()		  { return m_BlendType; }

	UINT32 GetNumberOfSteps() { return m_Steps; }
	INT32 GetWidth()			{ return m_Width; }

	virtual String Describe(BOOL Plural, BOOL Verbose); 

	void Transform(TransformBase &Trans);

	// Version 2 file format functions
	virtual BOOL WritePreChildrenWeb(BaseCamelotFilter* pFilter);
	virtual BOOL WritePreChildrenNative(BaseCamelotFilter* pFilter);

	virtual BOOL PostImport();			// function called after import

	// are there any open paths in my child list
	BOOL AreOpenPathsInChildren();

	virtual BOOL AllowOp(ObjChangeParam *pParam, BOOL SetOpPermissionState = TRUE,
												 BOOL DoPreTriggerEdit = TRUE);

	virtual BOOL RegenerateForPrinting() { return FALSE; }

	virtual void RenderEorDrag( RenderRegion* pRender );
	virtual BOOL ChildrenAreEorDragRenderedByMe() { return TRUE; }

	INT32 ComplexHide(UndoableOperation* pOp, Node* pNextInRange);

	void SetObjectProfile( const CProfileBiasGain &Profile )	{ m_ObjBiasGain = Profile; }
	void SetAttrProfile( const CProfileBiasGain &Profile )		{ m_AttrBiasGain = Profile; } 
	CProfileBiasGain GetObjectProfile()	{ return m_ObjBiasGain; }
	CProfileBiasGain GetAttrProfile()	{ return m_AttrBiasGain; }
	CProfileBiasGain* GetObjectProfilePtr()	{ return &m_ObjBiasGain; }
	CProfileBiasGain* GetAttrProfilePtr()	{ return &m_AttrBiasGain; }

	virtual BOOL IsController() { return TRUE;}
	virtual BOOL IsAContourController() const { return TRUE;}

	virtual BOOL EndBlendStep(BlendNodeParam * pParam);

	BOOL IsContourAnOuterContour() { return (m_Width < 0); }
	// Karim MacDonald 25/11/1999
	// virtual Extend functions; see declaration/implementation in Node.

	BOOL GetInsetPathFlag() { return m_bInsetPath; }
	void SetInsetPathFlag(BOOL b) { m_bInsetPath = b; }
	
public:
	virtual BOOL IsTypeExtendible() const { return TRUE; }
	virtual DocRect ValidateExtend(const ExtendParams& ExtParams);
	virtual void Extend(const ExtendParams& ExtParams);

public:
	// the pre render children function for inset path stuff
	virtual SubtreeRenderState RenderSubtree(RenderRegion* pRender, Node** ppNextNode = NULL, BOOL bClip = TRUE);

	// disables and enables the inset path path processor
	void DisableInsetPathPathProcessor();
	void EnableInsetPathPathProcessor();	

	virtual BOOL PromoteAttributeApplicationToMe(CCRuntimeClass *pAttrClass) const;

	NodeContour* GetContour ();

		// retrieves the node that we end up contouring
	virtual NodeRenderableInk* GetInkNodeFromController() const;
	virtual BOOL GroupCanTransformCached(TransformBase& Trans) const {return Trans.IsTranslation();} //{return FALSE;}

protected:
	BOOL DealWithJoinTypeChange(UndoableOperation * pOp);


private:
	BOOL BlendParameters (NodeContourController * pStart, NodeContourController * pEnd, double Scale);
	BOOL BlendParameters(NodeContourController * pStart, double Scale);

protected:
	UINT32 m_Steps;
	INT32 m_Width;
	ColourBlendType m_BlendType;

	CProfileBiasGain m_AttrBiasGain;
	CProfileBiasGain m_ObjBiasGain;

	BOOL m_bInsetPath;
	BOOL m_PerformedExtend;

	InsetPathPathProcessor* m_pPathProc;
} ;

/***********************************************************************************************

>	class ContourNodeTreeFactory : public CompoundNodeTreeFactory

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/8/99
	Purpose:	Creates a new contour node with the given parameters

***********************************************************************************************/
class ContourNodeTreeFactory : public CompoundNodeTreeFactory
{
	CC_DECLARE_DYNCREATE(ContourNodeTreeFactory);

public:
	ContourNodeTreeFactory();
	~ContourNodeTreeFactory();

	NodeCompound * CreateNode(List * pList, UndoableOperation * pOp = NULL);

	void SetNumberOfSteps(UINT32 Steps) { m_Steps = Steps; }
	void SetWidth(UINT32 Width)		  { m_Width = Width; }
	UINT32 GetNumberOfSteps()			  { return m_Steps; }
	UINT32 GetWidth()					  { return m_Width; }
	void SetAttributeMap(CCAttrMap * pMap)			  { m_pMap = pMap; }
	void SetInsetPathFlag(BOOL b)	  { m_bInsetPath = b; }
protected:
	UINT32 m_Steps;
	UINT32 m_Width;
	CCAttrMap * m_pMap;
	BOOL m_bInsetPath;
} ;

/***********************************************************************************************

>	class ContourNodePathProcessor : public PathProcessor

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/8/99
	Purpose:	The path processor which is used to sum all paths to be contoured

***********************************************************************************************/
class ContourNodePathProcessor : public PathProcessor
{
	CC_DECLARE_DYNCREATE(ContourNodePathProcessor);
public:
	ContourNodePathProcessor();
	~ContourNodePathProcessor();

	virtual void ProcessPath(Path * pPath,
							 RenderRegion * pRegion,
							 PathShape ShapePath = PATHSHAPE_PATH);

	void SetActive(BOOL bActive) { m_bActive = bActive; }

	void SetIsPrinting(BOOL bPrinting) { m_bPrinting = bPrinting; }
	
	// whether to test the line width to decide whether to extend the path or not
	// if false, processor ALWAYS extends the line width
	void TestLineWidth(BOOL bLineWidth) { m_bLineWidthTest = bLineWidth; }

	// calculates the maximum inner contour width possible with the given controller node
	static MILLIPOINT GetMaxInnerContourWidth(NodeCompound * pNode);

private:
	BOOL m_bActive;
	BOOL m_bLineWidthTest;
	BOOL m_bPrinting;
	BOOL m_bRespectJoinType;
} ;

/***********************************************************************************************

>	class ContourRecordHandler : public CamelotRecordHandler

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/10/98
	Purpose:	Handles the reading of all contour records
	SeeAlso:	-

***********************************************************************************************/

class ContourRecordHandler : public CamelotRecordHandler
{
	// Give my name out in memory dumps
	CC_DECLARE_DYNCREATE(ContourRecordHandler)

public:
	// functions that must be implemented.
	ContourRecordHandler();
	~ContourRecordHandler();
	virtual UINT32*	GetTagList();
	virtual BOOL	HandleRecord(CXaraFileRecord* pCXaraFileRecord);
} ;


#endif



	


