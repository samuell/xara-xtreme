// $Id: doccoord.h 662 2006-03-14 21:31:49Z alex $
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

/*
 *  */

/*********************************************************************************************

>	class DocCoord

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/5/1993          
	Base Class: Coord
	Purpose:	Structure holding a 32-bit cartesian coordinate.
				Its main use is to hold the physical coordinates of objects in document space.
	Errors:		None.

*********************************************************************************************/

#ifndef INC_DOCCOORD
#define INC_DOCCOORD

#ifndef INC_CAMTYPES
#include "camtypes.h"
#endif

#ifndef INC_COORD
#include "coord.h"
#endif

#include "wrkcoord.h"
#include "ccmaths.h"
#include "matrix.h"

// Declare various classes to prevent bogus stuff...
class Chapter;
class CCAPI WorkCoord;
class CCAPI OSCoord;
class Matrix;
class XMatrix;
class Spread;
class Document;
class View;
class UserCoord;

class CCAPI DocCoord : public Coord
{
public:

	DocCoord( INT32 x, INT32 y ) : Coord(x,y) {};
	DocCoord() : Coord() {};

	// Build the matrix which transforms DocCoords into OilCoords...
	friend Matrix ComposeDocToOilMat( const DocCoord& ChapterPos,	// Top left coord of chapter
                          const XLONG& ChapterDepth,			// Accumulated height of previous chaps
                          const FIXED16& ViewScale,			// User viewing scale
                          const WorkCoord& ScrollOffset );	// Scroll offsets

	// Build the matrix which transforms DocCoords into WorkCoords...
	friend XMatrix ComposeDocToWorkXMat( const DocCoord ChapterPos,	// Top left coord of chapter
                          const XLONG ChapterDepth,		// Accumulated height of previous chaps
                          const FIXED16 ViewScale);		// User viewing scale

	// Convert between coords
	WorkCoord ToWork( Document *pDocument, View *pView);
	WorkCoord ToWork( Spread *pSpread, View *pView);
	DocCoord  ToDoc(Spread* pSpread, View* pView);		// ie from SpreadCoords to    DocCoords
	DocCoord  ToSpread(Spread* pSpread, View* pView);	// ie from    DocCoords to SpreadCoords
	UserCoord ToUser(Spread* pSpread);					// ie from SpreadCoords to   UserCoords
	OilCoord  ToOil(Spread* pSpread, View* pView);		// ie from SpreadCoords to   OilCoords

	// Convert a DocCoord into a Logical coord (ie in millipoints, but 64 bits high)
	// The result will be returned in a WorkCoord (normally used for coords measured
	// in pixels)
	WorkCoord ToLogical(Spread* pSpread, View *pView);

	// Given a DocCoords, find the chapter its in and return other details...
	Chapter* FindEnclosingChapter(Document *pDocument, 
								  DocCoord* ChapterPos, 
								  XLONG* ChapterDepth,
								  View *pView);  

	// Functions dealing with pixel size taking scaling into account...
	// (See View class for actual scaled pixel size values/access functions)
	BOOL Pixelise();
	BOOL Pixelise(View* pView);
	BOOL IsPixelised(View* pView);

	// Some operator functions
	friend DocCoord operator + (const DocCoord& , const DocCoord& );
	friend DocCoord operator - (const DocCoord& , const DocCoord& );

	// Graeme (29-3-00) - Added the += and -= operators.
	const DocCoord& operator += ( const DocCoord &Other );
	const DocCoord& operator -= ( const DocCoord &Other );

	//A scaling function
	void Scale(const DocCoord dcOrigin, const double dDPI=96.0);

	// Karim 13/06/2000
	// Imagining a vector from the origin, return our angle clockwise from the vertical.
	// The angle returned is in radians, for ease-of-use with trig fns,
	// and is zero at the origin.
	// To convert to degrees, use (result * 180.0 / PI) :o)
	double AngleFromVertical() const { return atan2( (double)x, (double)y ); }

	// Jolly useful functions for working out coordinate positions
	static BOOL IsJoinAcute(const DocCoord* pJoin, const DocCoord* pOtherEndOfJoin, const DocCoord* pOtherPoint);
	static DocCoord PositionPointFromRatio(const DocCoord& StartPoint, const DocCoord& EndPoint, const double Ratio);
	static double DistanceFromLine(const DocCoord& Line1, const DocCoord& Line2, const DocCoord& Point);
	static DocCoord OneThird(const DocCoord& Start, const DocCoord& End);
	static DocCoord OneHalf(const DocCoord& Start, const DocCoord& End);
	static DocCoord TwoThirds(const DocCoord& Start, const DocCoord& End);
};

#endif

