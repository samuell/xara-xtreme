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
// ppbevel - Definition of Blend helper class

#ifndef INC_BLENDHELP
#define INC_BLENDHELP

//#include "becomea.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "pathpcs.h"

class BlendNodeParam;
class NodeCompound;

/********************************************************************************************

>	class BlendHelpers : public CCObject

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com> Mc
	Created:	23/2/2000

	Purpose:	This encapsulates the blend functions which are needed to blend compound
				nodes (e.g. shadows etc)

	SeeAlso:	NodeRenderableInk::Blend

********************************************************************************************/
class BlendHelpers : public CCObject
{
	CC_DECLARE_DYNAMIC(BlendHelpers);

public:
	BlendHelpers();
	~BlendHelpers();

	// the blending code (stripped from NodeBlender)
	// get the attribute map at a particular blend step
	BOOL BlendAttributes(BlendNodeParam * pParam, CCAttrMap * pAttrMap);

	// gives the path at a particular blend step
	BOOL BlendPaths(BlendNodeParam * pParam, Path * pPath);

private:
	Trans2DMatrix* GetRotateMatrix(NodeRenderableBounded* pNode,double Angle);

	/////////////////////////////////////////////////////////////////
	// Access funcs for the path arrays
	// Straight out of the blender
	DocCoord*  	GetCoordArray(UINT32 MinSize);
	PathVerb*  	GetVerbArray(UINT32 MinSize);
	PathFlags* 	GetFlagArray(UINT32 MinSize);
	UINT32*     	GetGBlendBuff(UINT32 MinSize);

	BOOL 		ReallocTempBuffers(UINT32 Size);
	void 		DeallocTempBuffers();

	// The following set of vars deal with arrays that are set up by the PreBlend() routine
	// Also, these arrays can also change by calls to the access funcs GetCoordArray(), GetVerbArray() and GetFlagArray()
	UINT32 		m_TempArraySize;
	DocCoord*	m_pTempCoords;
	PathVerb*	m_pTempVerbs;
	PathFlags* 	m_pTempFlags;
	UINT32		m_GBlendBuffSize;
	UINT32*		m_pGBlendBuff;
	UINT32		m_ArrayLength;
} ;

/********************************************************************************************

>	class CompoundNodeBlendBecomeA : public BecomeA

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com> Mc
	Created:	23/2/2000

	Purpose:	Gets the compound node to wrap up any nodes passed to the passback in a
				copy of its tree structure

	SeeAlso:	NodeRenderableInk::Blend

********************************************************************************************/
class CompoundNodeBlendBecomeA : public BecomeA
{
public:
	// inputs -
	// pCompound	-	the compound node
	//
	CompoundNodeBlendBecomeA(BecomeAReason Reason,
					CCRuntimeClass* pClass, 
					UndoableOperation* pOp, 
					BOOL Sel,
					NodeCompound * pCompound,
					BecomeA * pBecomeA) : BecomeA(Reason, pClass, pOp, Sel)
	{
		m_pCompound  = pCompound;
		m_pBecomeA   = pBecomeA;
//		m_Count = 0;
		m_NumPathNodes = 0;
	}					

	BOOL IsCompoundBlendBecomeA() { return TRUE; }

	BOOL PassBack(NodeRenderableInk* pNewNode,
		NodeRenderableInk* pCreatedByNode,
		CCAttrMap* pAttrMap=NULL);

//	void ResetCount() { m_Count = 0; }
	void SetNumPathNodes(UINT32 NumPaths) { m_NumPathNodes = NumPaths; }


private:
	NodeCompound	  * m_pCompound;
	BecomeA			  * m_pBecomeA;
//	UINT32				m_Count; // how many nodes we have converted
	UINT32				m_NumPathNodes; // how many nodes we are going to convert on this pass
} ;

/********************************************************************************************

>	class SumAllPathsElem : public ListItem

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/2/2000
	Purpose:	Element for the list in SumAllPathsPathProcessor
********************************************************************************************/
class SumAllPathsElem : public ListItem
{
	CC_DECLARE_DYNCREATE(SumAllPathsElem);

public:
	SumAllPathsElem()
	{
		m_pAttrMap = NULL;
		m_Path.Initialise();
	}

	SumAllPathsElem(Path * pPath, CCAttrMap * pAttrMap);
	~SumAllPathsElem();

	Path * GetPath() { return &m_Path; }
	CCAttrMap * GetAttrMap() { return m_pAttrMap; }
private:
	Path m_Path;
	CCAttrMap* m_pAttrMap;
} ;

/********************************************************************************************

>	class SumAllPathsPathProcessor : public PathProcessor

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/2/2000
	Purpose:	All paths are placed in the linked list, with their associated attribute
				maps
********************************************************************************************/
class SumAllPathsPathProcessor : public PathProcessor
{
	CC_DECLARE_DYNAMIC(SumAllPathsPathProcessor);
public:
	// inputs - whether we should draw the path passed into the ProcessPath or not
	SumAllPathsPathProcessor(BOOL bDrawOriginalPaths = FALSE);
	
	~SumAllPathsPathProcessor();

	virtual void ProcessPath(Path *pPath,
							 RenderRegion *pRender,
							 PathShape ShapePath = PATHSHAPE_PATH);

	List * GetList() { return &PathList; }

	void SetEnabled(BOOL bEnabled) { m_bEnabled = bEnabled; }

	// CGS:  we need to be to insert paths into the list when doing convert to editable shapes
	// (this is due to the fact that we don't have a RenderRegion - and therefore cannot push
	// a path processor!)
	void InsertSumAllPathsElem (SumAllPathsElem* newElem);

private:
	List PathList;
	BOOL m_bDrawPaths;
	BOOL m_bEnabled;
} ;



#endif

