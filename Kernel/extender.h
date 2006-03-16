// $Id: extender.h 662 2006-03-14 21:31:49Z alex $
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
//

#ifndef INC_EXTENDER
#define INC_EXTENDER

#include "doccoord.h"	// for DocCoord member variables

// types of extend-action.
static const BYTE X_EXTEND	= 1;
static const BYTE Y_EXTEND	= 2;
static const BYTE X_STRETCH	= 4;
static const BYTE Y_STRETCH	= 8;

// an extend buffer-zone constant, in DocCoord units (millipoints).
// points on or within this distance from the centre-point must not extend out.
static const INT32		EXT_EXPAND_BUFFER = 10;

// an extend shrinking buffer-zone constant. this must be 1mp bigger than the expanding
// buffer-zone, so that coords are never shrunk down to be next to that zone. this is
// because rounding errors may arbitrarily shift coords by 1mp, thus making previously
// valid points invariant under expansion.
static const INT32		EXT_SHRINK_BUFFER = EXT_EXPAND_BUFFER + 1;

/********************************************************************************************

>	class ExtendParams : public CC_CLASS_MEMDUMP

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/09/1999
	Purpose:	Parameter container for extension operation.
	See also:	

********************************************************************************************/

class ExtendParams : public CC_CLASS_MEMDUMP
{
	// Declare the class for memory tracking
	CC_DECLARE_MEMDUMP(ExtendParams);

public:
	// default and copy constructor.
	ExtendParams() {}
	ExtendParams(const ExtendParams& other)
	{
		fExtendFlags	= other.fExtendFlags;
		xinc			= other.xinc;
		xdec			= other.xdec;
		yinc			= other.yinc;
		ydec			= other.ydec;
		doccEndCentre	= other.doccEndCentre;
		doccStartCentre	= other.doccStartCentre;
		doccOffset		= other.doccOffset;
		xscale			= other.xscale;
		yscale			= other.yscale;
		doccScaleStart	= other.doccScaleStart;
		doccScaleEnd	= other.doccScaleEnd;
		xincExtendBuffer= other.xincExtendBuffer;
		xdecExtendBuffer= other.xdecExtendBuffer;
		yincExtendBuffer= other.yincExtendBuffer;
		ydecExtendBuffer= other.ydecExtendBuffer;
		pOp				= other.pOp;
	}

	// data.
	BYTE fExtendFlags;			// the type of extending required.
	INT32 xinc;					// displacement of points in the positive x-direction
	INT32 xdec;					// displacement of points in the negative x-direction
	INT32 yinc;					// displacement of points in the positive y-direction
	INT32 ydec;					// displacement of points in the negative y-direction
	DocCoord doccEndCentre;		// end-centre of extension; defines extension limits.
	DocCoord doccStartCentre;	// start-centre of extension; defines extension behaviour.
	DocCoord doccOffset;		// the displacement vector from start- to end- centres.
	double xscale;				// x-direction scaling factor
	double yscale;				// y-direction scaling factor
	DocCoord doccScaleStart;	// the scaling origin of the Node.
	DocCoord doccScaleEnd;		// the result of transforming doccScaleStart.
	INT32 xincExtendBuffer;		// the size of the dead zone in the +ve x-direction.
	INT32 xdecExtendBuffer;		// the size of the dead zone in the -ve x-direction.
	INT32 yincExtendBuffer;		// the size of the dead zone in the +ve y-direction.
	INT32 ydecExtendBuffer;		// the size of the dead zone in the -ve y-direction.
	UndoableOperation * pOp;
};



/********************************************************************************************

>	class Extender : public CC_CLASS_MEMDUMP

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/09/1999
	Purpose:	Preliminary container class for Extend() function.
				This performs automatic extension and stretching of nodes.

				Stretching is defined as application of a matrix transformation to control
				points, causing the points to scale and translate. The distance a point is
				moved depends on its own distance from some centre - think 'x -> ax+b'.

				Extension is defined as translation of control points, depending on their
				initial position. All points are moved by the same fixed amount, although the
				*direction* follows the same rules as for stretching - 'x -> x +/- delta'.
	See also:	

********************************************************************************************/

class Extender : public CC_CLASS_MEMDUMP
{

// Declare the class for memory tracking
CC_DECLARE_MEMDUMP(Extender);

public:
	static DocRect CheckValidExtend(NodeRenderableInk* pNode, BYTE fExtendFlags,
									const DocRect& drTriggerRelation,
									const DocRect& drTargetRelation,
									const DocRect& drThisTrigger,
									const DocRect& drLastTrigger,
									const DocRect& drLastTarget,
									BOOL * pOK = NULL,
									const BOOL ExtendAroundTarget = FALSE);

	static void Extend(	NodeRenderableInk* pNode, BYTE fExtendFlags,
						const DocRect& drTriggerRelation, const DocRect& drTargetRelation,
						const DocRect& drNewTrigger, const DocRect& drLastTrigger,
						const DocRect& drLastTarget, const DocRect* pdrDifference = NULL,
						const BOOL ExtendAroundTarget = FALSE,
						UndoableOperation * pOp = NULL);

	static BOOL ConvertQuickShapesInSelRangeToPaths(	UndoableOperation* pUndoOp,
														Range* pRange );

	static BOOL CheckValidExtend(	NodeRenderableInk* pNode,
									DocRect* pStartRect,
									DocRect* pEndRect,
									BYTE fStretchType,
									DocRect* pOldStartRect = NULL);

	static void Extend(	NodeRenderableInk* pNode,
						DocRect* pStartRect,
						DocRect* pEndRect,
						BYTE fStretchType,
						DocRect* pOldStartRect = NULL );

	static DocRect ValidateControlPoints(INT32 numPoints, const DocCoord* doccArray, const ExtendParams& ExtParams);

	static void CalculateExtendParams(ExtendParams* pEPS, BYTE fExtendFlags,
						const DocRect& drTriggerRelation, const DocRect& drTargetRelation,
						const DocRect& drNewTrigger, const DocRect& drLastTrigger,
						const DocRect& drLastTarget, const DocRect* pdrDifference = NULL,
						const BOOL ExtendAroundTarget = FALSE);

private:
	static INT32 CheckInvalidShrinkingPoints(INT32 nCentre, INT32 nDelta, INT32 nOffset,
											INT32 nNumPoints, const DocCoord* doccArray,
											BOOL bExamineX);
	static INT32 CheckInvalidExpandingPoints(INT32 nCentre, INT32 nDelta, INT32 nOffset,
											INT32 nNumPoints, const DocCoord* doccArray,
											BOOL bExamineX);
	static INT32 CheckLimits(const INT32 limit, const INT32 delta, const INT32 value);
};

#endif	// !INC_EXTENDER
