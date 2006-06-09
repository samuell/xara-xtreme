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

#ifndef NODECNTR_H
#define NODECNTR_H

//#include "becomea.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "biasgain.h" - in camtypes.h [AUTOMATICALLY REMOVED]

class ExtendParams;
class NodeContourController;
class AttrBrushType;
class SumAllPathsElem;
class NodeCompound;

/***********************************************************************************************

>	class NodeContour : public NodeRenderableInk

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/8/99
	Purpose:	Contour node

***********************************************************************************************/

class NodeContour : public NodeRenderableInk
{
	// Give my name out in memory dumps
	CC_DECLARE_DYNCREATE(NodeContour);

public:
	NodeContour();

	NodeContour(Node* ContextNode,  
				AttachNodeDirection Direction,  
				BOOL Locked = FALSE, 
				BOOL Mangled = FALSE,  
				BOOL Marked = FALSE, 
				BOOL Selected = FALSE    
				) ;

	~NodeContour();

	// Copy functions
	virtual Node* SimpleCopy(); 
	void CopyNodeContents(NodeContour* pNewNode);
	virtual void PolyCopyNodeContents(NodeRenderable* pNodeCopy);

	// get the bounding rect of me
	virtual DocRect GetBoundingRect(BOOL DontUseAttrs = FALSE, BOOL HitTest = FALSE);
	virtual DocRect GetBlobBoundingRect();

	virtual void RenderTinyBlobs(RenderRegion * pRegion);

	virtual void Render(RenderRegion* pRender);

	virtual BOOL DoBecomeA(BecomeA* pBecomeA);
	virtual BOOL CanBecomeA(BecomeA* pBecomeA); 

	virtual BOOL NeedsParent(Node* pClassNode) const;
	virtual NodeCompound* GetParentController() const;

	virtual String Describe(BOOL Plural, BOOL Verbose); 

	virtual BOOL IsAContour() const { return TRUE;}
	BOOL GenerateContour(List * pList = NULL, CCAttrMap * pAttrMap = NULL);
	
	void DeleteCache();

	void Transform(TransformBase &Trans);

	void RenderEorDrag(RenderRegion * pRender);

	// Version 2 file format functions
	virtual BOOL WritePreChildrenWeb(BaseCamelotFilter* pFilter);
	virtual BOOL WritePreChildrenNative(BaseCamelotFilter* pFilter);

	virtual BOOL AllowOp(ObjChangeParam *pParam, BOOL SetOpPermissionState = TRUE,
												 BOOL DoPreTriggerEdit = TRUE);

	// Karim MacDonald 25/11/1999
	// virtual Extend functions; see declaration/implementation in Node.
public:
	virtual BOOL IsTypeExtendible() const { return TRUE; }
	virtual DocRect ValidateExtend(const ExtendParams& ExtParams);
	virtual void Extend(const ExtendParams& ExtParams);

	// smooths the path
	static BOOL SmoothPath(Path * pPath);

	virtual BOOL OnNodePopUp(Spread* pSpread, DocCoord PointerPos, ContextMenu* pMenu);

	static BOOL GenerateContourPathForNode(	Path* pDestPath,
											Node* pSourceNode,
											const List* pSourceList,
											const MILLIPOINT nWidth,
											const BOOL fOuterContour	= TRUE,
											const JointType jt			= RoundJoin,
											const MILLIPOINT nFlatness	= -1,
											const BOOL fUseLineWidths	= TRUE,
											const BOOL fIncludeShadows	= FALSE	);

	virtual INT32 EstimateNodeComplexity (OpParam* details);

protected:
	// blend attributes function
	BOOL BlendAttributes(RenderRegion * pRender, CCAttrMap * pAttrMapStart, 
						 CCAttrMap * pEndAttrMap, CCAttrMap * pBlendedAttrMap, 
						 double BlendRatio);

	// copies all the attributes in the attribute map onto the node
	BOOL CopyAttributes(NodeRenderableInk * pNode, CCAttrMap * pAttrMap);

	// smooths all paths
	BOOL SmoothPaths();

	// finds the node I'm to use to blend colours from
	NodeRenderableInk * FindNodeToContour(NodeRenderableInk * pInk);

protected:
	// the source path generated by the path processor
	Path m_SourcePath;

	// the path list generated by the contour
	Path * m_pPathList;
	INT32 m_NumPaths;

	BOOL m_FirstRender;

} ;

/***********************************************************************************************

>	class ContourBecomeA: public BecomeA

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com> Mc
	Created:	18/10/94
	Purpose:	This is used to turn all nodes which are to be contoured into a single path

***********************************************************************************************/

class ContourBecomeA : public BecomeA
{
public:
	ContourBecomeA(	BecomeAReason Reason,
					CCRuntimeClass* pClass, 
					UndoableOperation* pOp, 
					BOOL Sel,
					Path * pPathList,
					Path * pSummedPath, 
					INT32 Width,
					INT32 NumSteps,
					BOOL bOuter,
					CProfileBiasGain &Profile,
					JointType jt = RoundJoin,
					double Flatness = 200.0,
					BOOL bIncludeLineWidths = TRUE) : BecomeA(Reason, pClass, pOp, Sel)
	{
		m_pPathList = pPathList;
		m_bNodeIsPath = FALSE;
		m_Width = Width;
		m_NumSteps = NumSteps;
		m_pSummedPath = pSummedPath;
		m_bOuter = bOuter;
		m_Profile = Profile;
		m_Join	  = jt;
		m_Flatness = Flatness;
		m_bIncludeLineWidths = bIncludeLineWidths;

		// we're only interested in the silhouettes of shapes.
		fSilhouette = TRUE;

		m_bContourBrush = FALSE;
	}

	// this sets the variable which tells the passback function that the next node
	// passed in is a path node
	void SetNodeIsPath(BOOL b) { m_bNodeIsPath = b; }
	
	// This function should be called when Reason == BECOMEA_PASSBACK 
	virtual BOOL PassBack(NodeRenderableInk* pNewNode,NodeRenderableInk* pCreatedByNode,CCAttrMap* pAttrMap);

	// MRH New Gavin Version!
	BOOL GenerateContourPaths(Path* pPathToContour, Path* pPathList, JointType jt, bool Outer = true,
							  bool UseContourMode = true, LineCapType ct = LineCapRound);
	
	void SetContourBrush(BOOL Set);
protected:
	// DY we want to contour nodes with brushes applied to them, so heres a yucky special case function
	BOOL PassbackBrush(NodeRenderableInk* pCreatedByNode, NodeRenderableInk** ppNewNode);

private:
	Path * m_pPathList;
	
	// the summed path of all nodes' paths which have been passed through the passback mecahnism
	Path * m_pSummedPath;
	INT32 m_NumSteps;
	INT32 m_Width;
	BOOL m_bNodeIsPath;
	BOOL m_bOuter;
	BOOL m_bIncludeLineWidths;

	CProfileBiasGain m_Profile;

	JointType m_Join;

	double m_Flatness;

	BOOL m_bContourBrush;

};

#endif // NODECNTR_H



	


