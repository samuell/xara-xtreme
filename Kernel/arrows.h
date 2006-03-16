// $Id: arrows.h 662 2006-03-14 21:31:49Z alex $
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
// Defines ArrowRec class (a Line ArrowHead Record)

#ifndef INC_ARROW
#define INC_ARROW

#include "fixed16.h"
#include "doccoord.h"
#include "paths.h"

class Trans2DMatrix;

const INT32 DEFAULT_ARROW_LINEWIDTH = (72000/2);
const INT32 DEFAULT_ARROW_SIZE = 3;

/********************************************************************************************

< StockArrow

Comment:
	MonoOn
	enum StockArrow
	{
		SA_NULLARROW,
		SA_STRAIGHTARROW,
		SA_ANGLEDARROW,
		SA_ROUNDEDARROW,
		SA_SPOT,
		SA_DIAMOND,
		SA_ARROWFEATHER
		SA_ARROWFEATHER2,
		SA_HOLLOWDIAMOND
	};
	MonoOff
	
********************************************************************************************/

enum StockArrow
{
	SA_NULLARROW,
	SA_STRAIGHTARROW,
	SA_ANGLEDARROW,
	SA_ROUNDEDARROW,
	SA_SPOT,
	SA_DIAMOND,
	SA_ARROWFEATHER,
	SA_ARROWFEATHER2,
	SA_HOLLOWDIAMOND,

	NUM_STOCK_ARROWS
};

/********************************************************************************************

>	class ArrowRec : public CCObject

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/4/95
	Purpose:	A class defining a Line ArrowHead Record.

********************************************************************************************/

class ArrowRec : public CCObject
{
	CC_DECLARE_DYNCREATE(ArrowRec);

public:

// Construction
	ArrowRec();
	ArrowRec(Path* pArrowPath, const DocCoord& Centre, 
			 INT32 LineWidth = DEFAULT_ARROW_LINEWIDTH, 
			 FIXED16 WidthScale  = DEFAULT_ARROW_SIZE, 
			 FIXED16 HeightScale = DEFAULT_ARROW_SIZE,
			 BOOL Start = FALSE);

// Copy constructor
	ArrowRec(const ArrowRec&);

// Destruction
	~ArrowRec();

//Initialisation
	BOOL CreateStockArrow(StockArrow);

	void SetArrowSize(FIXED16 Width, FIXED16 Height = 0);

// Copying and equality
	INT32 operator==(const ArrowRec&);
	ArrowRec& operator=(const ArrowRec&);

	INT32 operator==(StockArrow);
	ArrowRec& operator=(StockArrow);

// Arrow Data access
	Path*	 	GetArrowPath();
	DocCoord 	GetArrowCentre();
	INT32	 	GetArrowLineWidth();
	FIXED16 	GetArrowWidth();
	FIXED16 	GetArrowHeight();

	BOOL 		IsStartArrow();
	BOOL 		ShouldScaleWithLineWidth();

	INT32 		GetArrowID();

// Position and bounds calculations
	DocRect GetArrowBoundingRect(Path* pParent, INT32 LineWidth, BOOL StartArrow);

	BOOL GetArrowMatrix(const DocCoord& Centre, const DocCoord& Direction, 
		 				INT32 LineWidth, Trans2DMatrix* pMatrix);

// Statics for getting arrow positions within paths
	static BOOL DoesPathNeedArrowHeads(DocCoord* Coords, PathVerb* Verbs, INT32 NumCoords);

	static BOOL GetFirstArrowPos(BOOL StartArrow,
								 DocCoord* Coords, PathVerb* Verbs, INT32 NumCoords, 
								 INT32* Index, DocCoord* Centre, DocCoord* Direction);

	static BOOL GetNextArrowPos(BOOL StartArrow,
							 	DocCoord* Coords, PathVerb* Verbs, INT32 NumCoords, 
								INT32* Index, DocCoord* Centre, DocCoord* Direction);

// Check for a dummy ArrowHead
	inline BOOL IsNullArrow() { return (ArrowShape == NULL || IsNull); }

// Number of Stock Arrows available
	static INT32 GetNumStockArrows() { return NUM_STOCK_ARROWS; }
	static String_256 GetStockArrowName(StockArrow);

// Angle calculation
	static ANGLE CalcAngle(const DocCoord& Centre, const DocCoord& Direction);

private:
	DocRect FindBoundsAt(const DocCoord& Centre, const DocCoord& Direction, INT32 LineWidth);

public:
	Path* ArrowShape;
	DocCoord Centre;
	INT32 LineWidth;
	FIXED16 ArrowWidth;
	FIXED16 ArrowHeight;

	BOOL StartArrow;
	BOOL ScaleWithLineWidth;
	INT32 ArrowID;

	// For backwards compatability
	BOOL IsNull;

	// gets the base of the arrow (in arrow coordinates i.e. before it's been transformed
	// by the matrix delivered in GetArrowMatrix)
	void GetArrowBase(DocCoord * pPoint);

	// flag to indicate that the arrow is outside of the path
	BOOL m_bExtendPath;
};

#endif  // INC_ARROW
