// $Id: brushref.h 662 2006-03-14 21:31:49Z alex $
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

// The brushref class

#ifndef BRUSHREF
#define BRUSHREF

#include "nodebldr.h"

#include "ink.h"

class BitmapFillAttribute;

/***********************************************************************************************

>	class BlendPathOffset

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/10/99
	Purpose:	Stores the translation of the blendpath to the centre of its parent object.
***********************************************************************************************/

class BlendPathOffset
{
public:
	BlendPathOffset() {m_XOffset = 0; m_YOffset = 0; m_Angle = 0; m_Magnitude = 0;}
	BlendPathOffset(MILLIPOINT X, MILLIPOINT Y) {m_XOffset = X; m_YOffset = Y; 
												CalculateAngleAndMagnitude();}

	void CalculateAngleAndMagnitude();

	// get the polar version
	void GetAngleAndMagnitude(double* pAngle, MILLIPOINT* pMagnitude);
	
	// work out what the offsets would be if it were rotated
	void RotateByAngle(double Angle, MILLIPOINT* pXoffset, MILLIPOINT* pYOffset);

public:  //data
	MILLIPOINT m_XOffset;
	MILLIPOINT m_YOffset;

protected: 
	// the polar representation
	double m_Angle;
	MILLIPOINT m_Magnitude;
};

/***********************************************************************************************

>	class BrushRef : public BlendRef

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/12/99
	Purpose:	'wrapper' for the blendref class for use with the brush.  Contains the information
				that the nodebrushmaker needs to draw the brush objects

***********************************************************************************************/

class BrushRef : public BlendRef
{
	CC_DECLARE_MEMDUMP(BrushRef);

public:
	typedef std::list<BlendPathOffset>	list;
	typedef list::iterator				iterator;

public:
	BrushRef();
	~BrushRef();

	BOOL Initialise(NodeRenderableInk* pInitNode);
	BlendPathOffset* GetFirstOffset( iterator *pHeadPosition );
	BlendPathOffset* GetNextOffset( iterator *pPosition );
	iterator GetEndPosition() { return m_OffsetList.end(); }
	
	DocRect GetBoundingRect();
	void DeleteBitmapFills();  // deletes bitmap fills, use only when camelot is closing
	void DeleteAttributesAndPath(); 
	void FreeAttributes();
	void TranslateTo(DocCoord Target);

	void TransformAttributes(TransformBase& Trans);

	// get the blendpaths to copy their data for rendering
	BOOL MakeCopiesForRendering();
	BOOL UpdateCopies();
	void DeleteRenderCopies();

	// returns the first and subsequent copies of the blendpaths
	Path* GetFirstCopyPath();
	Path* GetNextCopyPath();

	// returns the attribute map pertaining to the current blendpath
	CCAttrMap* GetCurrentAttributeCopy();

protected: //helper functions	
	BOOL SetBitmapFillFlags(); // tells any bitmap fills not to delete themselves normally as we will do it
	
protected:
	NodeRenderableInk*	m_pNode;
	List				m_BlendPathList;
	UINT32				m_NumBlendPaths;
	CMapPtrToPtr*		m_pBlendPathAttrMap;
	DocRect				m_CachedRect;
	// the list of transformations from the centre of the blendpath to the centre of 
	// the inknode.  Only really used when making brushes from groups.
	list				m_OffsetList;

	// our current blendpath for rendering
	BlendPath*			m_pCurrentBlendPath;
};


/***********************************************************************************************

>	class BrushRefBlender : public CC_CLASS_MEMDUMP

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/12/99
	Purpose:	An instance of this class is designed to take two BrushRef objects and blend together
				their paths and attributes, churning out a BlendedBrushRef object at the end.
				It essentially rips off a lot of what the NodeBlender does, except that instead of
				rendering anything or existing in the tree it just produces a blended object.

***********************************************************************************************/

class BrushRefBlender : public CC_CLASS_MEMDUMP
{
	CC_DECLARE_MEMDUMP(BrushRefBlender);
public:
	BrushRefBlender();
	~BrushRefBlender();

	void SetBrushRef(BrushRef* pBrushRef, BOOL First);
	BrushRef* Blend(BrushRef* pStartBrushRef, BrushRef* pOther, double Ratio);

protected: //helper functions

	BOOL BlendPaths(BlendPath* pBlendPathStart,BlendPath* pBlendPathEnd,double BlendRatio);
	BOOL BlendAttributes(BlendPath* pBlendPathStart, BlendPath* pBlendPathEnd,
						CCAttrMap* pBlendedAttribMap,double BlendRatio);

	BOOL 		ReallocTempBuffers(UINT32 Size);
	void 		DeallocTempBuffers();

	// Access funcs for the path arrays
	DocCoord*  	GetCoordArray(UINT32 MinSize);
	PathVerb*  	GetVerbArray(UINT32 MinSize);
	PathFlags* 	GetFlagArray(UINT32 MinSize);
	UINT32*     	GetGBlendBuff(UINT32 MinSize);


protected: //data
		// the two brushrefs to be blended
	BrushRef* m_pStartBrushRef;  
	BrushRef* m_pEndBrushRef;

	// The following set of vars deal with arrays that are set up by the PreBlend() routine
	// Also, these arrays can also change by calls to the access funcs GetCoordArray(), GetVerbArray() and GetFlagArray()
	UINT32 		m_TempArraySize;
	DocCoord*	m_pTempCoords;
	PathVerb*	m_pTempVerbs;
	PathFlags* 	m_pTempFlags;
	UINT32		m_GBlendBuffSize;
	UINT32*		m_pGBlendBuff;
	UINT32		m_ArrayLength;
	
};	
#endif
