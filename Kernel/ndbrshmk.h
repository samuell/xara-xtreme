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

//Nodebrushmaker class

#ifndef INC_NDBRSHMK
#define INC_NDBRSHMK

#include "tranform.h"
#include "doccoord.h"
#include "docrect.h"
#include "ink.h"
#include "paths.h"
#include "blendatt.h"
#include "nodebldr.h"
#include "trans2d.h"
#include "brushref.h"
#include "brshcomp.h"

class RenderRegion;
class GRenderBrush;
class Spread;
class BecomeA;
class HandleBecomeA;
class CCAttrMap;
class Progress;
class BaseCamelotFilter;

class NodeBrush;
class NodeBrushPath;
class CPathPointInfo;

/*
enum PathOffset {OFFSET_NONE,
				 OFFSET_ALTERNATE,
				 OFFSET_LEFT,
				 OFFSET_RIGHT,
				 OFFSET_RANDOM};

const MILLIPOINT MIN_BRUSH_SPACING = 10;
const MILLIPOINT MAX_BRUSH_SPACING = 10000;
const UINT32       MAX_BRUSH_OBJECTS = 20;
*/





/********************************************************************************************

>	class NodeBrushMaker: public NodeRenderableInk

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/10/99
	Purpose:	The NodeBrushMaker class

********************************************************************************************/

class NodeBrushMaker : public NodeRenderableInk
{
	CC_DECLARE_DYNCREATE(NodeBrushMaker);

public:
	typedef std::vector< CPathPointInfo > CListOfPointsToRender;

public:
	NodeBrushMaker();
	NodeBrushMaker(Node* ContextNode,  
			  AttachNodeDirection Direction,    
			  BOOL Locked=FALSE, 
			  BOOL Mangled=FALSE,  
			  BOOL Marked=FALSE, 
			  BOOL Selected=FALSE    
		     ); 
	
	~NodeBrushMaker();
 
	BOOL InitialiseInk(NodeRenderableInk** pInkNode, const UINT32 NumInkNodes = 1, Progress* pProgress = NULL);
	BOOL InitialisePath(NodeBrushPath* pBrushPath);
	void InitialiseBrushArray(UINT32 NumObjects);
	// get the bounding rect 
	virtual DocRect GetBoundingRect(BOOL DontUseAttrs = FALSE, BOOL HitTest = FALSE);
	virtual DocRect GetBlobBoundingRect();

	// the main render function
	virtual void Render(RenderRegion* pRender);
	
	// blob rendering
//	void RenderObjectBlobs(RenderRegion* pRender);
	

	virtual void RenderEorDrag( RenderRegion* );
	//  require transparency ?
//	virtual BOOL NeedsTransparency() const;

//	virtual String Describe(BOOL Plural, BOOL Verbose = TRUE);

	// transformation function
	virtual void Transform( TransformBase& Trans );

	// Version 2 file format functions
//	virtual BOOL WritePreChildrenWeb(BaseCamelotFilter* pFilter);
//	virtual BOOL WritePreChildrenNative(BaseCamelotFilter* pFilter);

	// Information functions
	virtual UINT32 GetNodeSize() const; 			// Returns size of node in bytes
 


	// access functions
	BOOL SetPositionRandomness(double Value);
	BOOL SetAttributeRandomness(double Value);
	BOOL SetSizeRandomness(double Value);
	BOOL SetAlignmentRandomness(double Value);

	double GetPositionRandomness();
	double GetAttrRandomness();
	double GetSizeRandomness();
	double GetAlignmentRandomness();

	void SetGRenderBitmap(GRenderBrush* pGRenderBitmap);
	MILLIPOINT GetBrushSpacing();
	BOOL       SetBrushSpacing(MILLIPOINT Spacing);
	PathOffset GetPathOffsetType();
	MILLIPOINT GetPathOffsetValue();
	void	   SetPathOffsetType(PathOffset Offset);
	void	   SetPathOffsetValue(MILLIPOINT Value);
	void	   SetBlendRef(BlendRef* pBlendRef);
	BOOL	   GetRotated();
	void	   SetRotated(BOOL Rotate);
	void	   SetUsePathCache(BOOL Value);

	void SetNumberOfSteps(INT32 Steps);
	BOOL GetStartCoord(DocCoord* Coord);
	void SetStartCoord(DocCoord Coord);
	void RenderStep(INT32 StepNum, Spread* pSpread, CPathPointInfo Point);

	NodeRenderableInk* GetInkNode();
	BOOL PrepareToRender(Spread* pSpread);
	void FinishRender();
	void RecalculateMaxStep();
	void ClearListOfPointsToRender();

	virtual void PolyCopyNodeContents(NodeRenderable* pNodeCopy);
	
protected:
	// return a copy of this node
	virtual Node* SimpleCopy();
	void RenderAttributes(RenderRegion* pRender, CCAttrMap* pAttrMap);
	
	void CopyNodeContents(NodeBrushMaker* pCopyOfNode);
	void ResetMembers();
	void RandomizeShape(Trans2DMatrix* TransformMatrix);
	void RandomizeAttributes();
	
	DocCoord SetPosition( CPathPointInfo OriginalPoint, UINT32 AltValue, 
		CListOfPointsToRender::iterator *ListPosition );
	
	BOOL GetGRenderBitmap(DocRect* pRenderRect, GRenderBrush** ppGRender, Spread* pSpread);
	BOOL DrawViewToRenderRegion(Spread* pSpread, RenderRegion* pRender);
	
	void AddPointsInRectToList(DocRect Rect);
	void AddAllPointsToList();
	void RenderBrushObjectAtPoint(CPathPointInfo Point, RenderRegion* pRender);


	// Debugging function
	#ifdef _DEBUG
	void	ShowDebugTreeDetails() const;
	#endif
	void GetDebugDetails( StringBase* Str );
	void GetDebugDetails( StringBase* Str,BlendRef* pBlendRef );


	String_32		   m_BrushMakerName;
	
	NodeRenderableInk* m_pInkNode;			// the node used to create the brush
	BlendRef*		   m_pBlendRef;			// creates and stores the blendpath objects that are rendered
	NodeBrushPath*     m_pNodeBrushPath;     // the path along which to draw the brush
	GRenderBrush* 	   m_pGRender;           // the render region that we use to render the brush objects 
											// with as they are being drawn
	
	GRenderBrush		   *m_pGRenderDIB;
	CListOfPointsToRender	m_ListOfPointsToRender;	// used to store the list of points we need to render
	std::vector< BrushRef * > m_BrushRefPtrArray;		// holds multiple blendrefs used to create the brush objects 
														// (necessary to have an array in case we want to use it as an
														// object sprayer)
	
	MILLIPOINT		   m_BrushSpacing;  // the distance between each brush object


	// the amount of randomness in the brush objects
	double			   m_PositionRandomness;
	double			   m_AttrRandomness;
	double			   m_SizeRandomness;
	double             m_AlignmentRandomness;
	UINT32			   m_AltValue;
	PathOffset		   m_PathOffsetType;
	MILLIPOINT		   m_PathOffsetValue;
	CCAttrMap*         m_pAttrMap;
	
	INT32			   m_BrushStep;  // the current step that is rendering
	INT32               m_MaxStep;	 // the maximum step to render

	MILLIPOINT		   m_CurrentDistance; // the distance along the path that we have reached whilst rendering
	DocCoord           m_StartCoord;      // the starting coordinate of the nodebrushpath

	DocRect            m_BrushBoundingRect;
	

	BOOL			   m_bRandomizeShape;
	BOOL			   m_JustTransformed;  // has the node just been transformed?
	
	BOOL			   m_bFirstRender;
	BOOL			   m_bRotate;

	BOOL			   m_bTile;           // do we tile our fills?
	BOOL			   m_bUsePathCache;   // do we use the cached path processor of the nodebrushpath?

	// the list of transformations from the centre of the blendpath to the centre of 
	// the inknode.  Only really used when making brushes from groups.
	std::vector< BlendPathOffset >	m_OffsetList;
private:

};

#endif
