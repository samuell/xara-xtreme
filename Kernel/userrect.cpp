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


#include "camtypes.h"
#include "userrect.h"
#include "ensure.h"

DECLARE_SOURCE("$Revision$");

/*
  $Log: rect.ctt $
Revision 1.20  1995/07/07  11:35:00  Jason
Fixed ABS macro definitions to be safe

Revision 1.19  1994/06/28  16:57:47  Rik
Added some code that shows how the hell the IsAdjacent function works

Revision 1.18  1994/03/03  11:38:12  Tim
Added a MakeEmpty function.

Revision 1.17  1994/01/20  17:09:23  Simon
Default constructor now creates an empty recatngle

Revision 1.16  1994/01/12  17:30:17  Rik
Improved region merging and region joining

Revision 1.15  1993/11/05  14:56:03  Rik
Blobs work, fixed DocRect probs, selecting blobs work

Revision 1.14  1993/08/12  11:17:48  Will
Global CheckIn for new UserCoord system.

Revision 1.13  1993/07/29  11:10:02  Will
Fixed problem with fuzzy adjacent test.

Revision 1.12  1993/07/21  21:08:29  Will
Split rect function was splitting the wrong rect.

Revision 1.11  1993/07/19  17:02:42  Will
Added some comments.
Added Fuzzy limit passed during IsAdjacent().

Revision 1.10  1993/07/15  17:04:10  Will
Added SplitRect function for BG rendering.

Revision 1.9  1993/07/14  17:34:25  Will
Added IsAdjacent function for region merging.

Revision 1.8  1993/07/12  12:58:11  Tim
Added a ContainsRect function.

Revision 1.7  1993/06/18  11:06:28  Tim
Made rectangle classes usable by tools by using the CAMAPI macro in the
class declaration.

Revision 1.6  1993/06/15  16:49:28  Tim
Altered Intersection and Union to return objects rather than object refs.

Revision 1.5  1993/06/10  10:29:35  Tim
Improved links for ADC generated help file.

Revision 1.4  1993/06/09  16:40:02  Tim
Moved class comment from .cpp file to .h file to conform to style guide.

Revision 1.3  1993/06/09  13:52:40  Tim
Revised use and meaning of empty/invalid rectangles.
Expanded comment headers to explain this.

Revision 1.2  1993/06/08  14:12:06  Tim
Added comments about non-re-entrancy of Union and Intersection.

*/


/**********************************************************************************************

>    BOOL UserRect::IsAdjacent() const

     Author: 	Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
     Created:   14/7/93
     Inputs:	The Fuzzy limit in MILLIPOINTS.  
     Outputs:   - 
     
     Returns:   TRUE if the rectangles are adjacent within a Fuzzy limit.
     			
     Purpose:	To check for adjacent rectangles.

     Errors:	

**********************************************************************************************/  

BOOL UserRect::IsAdjacent(const UserRect& Rect, MILLIPOINT Fuzzy) const
{
	// Check for an an empty rectangle
	if (IsEmpty())
		return FALSE;

    // Detect an invalid rectangle
	ENSURE(IsValid(), "UserRect::IsAdjacent() was called on\nan invalid rectangle.");  

#if 0
	// This is the expansion of the rampant return statement below

	// Near top or bottom
	BOOL NearBot = (ABS(Rect.hi.y - lo.y)) <= Fuzzy;
	BOOL NearTop = (ABS(Rect.lo.y - hi.y)) <= Fuzzy;

	// Near left or right
	BOOL NearLeft  = (ABS(Rect.hi.x - lo.x)) <= Fuzzy;
	BOOL NearRight = (ABS(Rect.lo.x - hi.x)) <= Fuzzy;

	// Overlaps
	BOOL OverLeft  = (ABS(Rect.lo.x - lo.x)) <= Fuzzy;
	BOOL OverRight = (ABS(Rect.hi.x - hi.x)) <= Fuzzy;
	BOOL OverTop   = (ABS(Rect.hi.y - hi.y)) <= Fuzzy;
	BOOL OverBot   = (ABS(Rect.lo.y - lo.y)) <= Fuzzy;

	// Adjacent to the top or bottom?
	BOOL TopOrBot    = (NearTop || NearBot) && OverLeft && OverRight;
	BOOL LeftOrRight = (NearLeft || NearRight) && OverTop && OverBot;

	return (TopOrBot || LeftOrRight);
#endif

	// This is explained more carefully in the section above that is excluded from the build
	return (( ((ABS(Rect.lo.x - lo.x))<=Fuzzy) && ((ABS(Rect.hi.x - hi.x))<=Fuzzy) &&
			 (((ABS(Rect.hi.y - lo.y))<=Fuzzy) || ((ABS(Rect.lo.y - hi.y))<=Fuzzy)) ) ||
	        ( ((ABS(Rect.lo.y - lo.y))<=Fuzzy) && ((ABS(Rect.hi.y - hi.y))<=Fuzzy) &&
			 (((ABS(Rect.hi.x - lo.x))<=Fuzzy) || ((ABS(Rect.lo.x - hi.x))<=Fuzzy)) ));
}



/*********************************************************************************************

>    UserRect UserRect::Union(const UserRect& R)

     Author: 	Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
     Created:   17/5/93
     Inputs:	R - the rectangle to join with the object.
     Outputs:   - 
     
     Returns:   The simple union (bounding box).
     			
     Purpose:	Compute union of two rectangles.
     			
     			If one of the rectangles involved is invalid, the other is returned as the
     			result.  In the case of both being invalid, one of the invalid rectangles
     			is returned as the result.  It is undefined which rectangle is returned in
     			this case.

     Errors:	None.

**********************************************************************************************/  

UserRect UserRect::Union(const UserRect& R) const
{
	// Special cases for invalid rectangles...
	if (!IsValid()  ||  IsEmpty())
		return R;
	if (!R.IsValid()  ||  R.IsEmpty())
		return *this;
		
	// Return the result
	UserRect U;
	U.lo.x = min(lo.x, R.lo.x);
	U.lo.y = min(lo.y, R.lo.y);
	U.hi.x = max(hi.x, R.hi.x);
	U.hi.y = max(hi.y, R.hi.y);
	return U;
}



/*********************************************************************************************

>    UserRect UserRect::Intersection(const UserRect& R)

     Author: 	Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
     Created:   17/5/93
     Inputs:	R - the rectangle to intersect with.
     Outputs:   - 
     
     Returns:   The intersection.
     			
     Purpose:	Compute intersection of two rectangles.

     			If one of the rectangles involved is invalid, the other is returned as the
     			result.  In the case of both being invalid, one of the invalid rectangles
     			is returned as the result.  It is undefined which rectangle is returned in
     			this case.

     Errors:	None.

**********************************************************************************************/  

UserRect UserRect::Intersection(const UserRect& R) const
{
	// Special cases for invalid rectangles...
	if (!IsValid())
		return R;
	if (!R.IsValid())
		return *this;
	
	// Special cases when empty rectangles
	if (IsEmpty())
		return *this;
	if (R.IsEmpty())
		return R;

	// Return the result
	UserRect I;
	I.lo.x = max(lo.x, R.lo.x);
	I.lo.y = max(lo.y, R.lo.y);
	I.hi.x = min(hi.x, R.hi.x);
	I.hi.y = min(hi.y, R.hi.y);
	return I;
}



/*********************************************************************************************

>    INT32 UserRect::SplitRect(const UserRect& R, UserRect* SubRects)

     Author: 	Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
     Created:   14/7/93
     Inputs:	R is the rectangle to intersect with, SubRects is an array of UserRect to fill.  
     Outputs:   Puts any sub-rects into SubRects. 
     
     Returns:   The number of subrectangles found. 0 for no intersection.
     			
     Purpose:	Splits a rectangle into sub-rectangles which all exclude the interecting
     			rectangle R.  The number of sub-rects generated depends on how the rectangles
     			intersect.
     				 _______________________       	 ___________________
	            	|                       |      	|                   |
     				| *this    A            |      	| *this    A        |
     				|_ _ _  ________  _ _ _ |      	|_ _ _ _ _  ________|__________
     				|      |        |       |      	|          |        |          |
     				|  B   |   R    |   C   |      	|    B     |        |          |
     				|_ _ _ |________| _ _ _ |      	|__________|________| R        |
     				|                       |      	           |                   |
                    |          D    Index=4 |      	           |           Index=2 |
     				|_______________________|      	           |___________________|
     Errors:	

**********************************************************************************************/  

INT32 UserRect::SplitRect(const UserRect& R, UserRect* SubRects)
{
	if (!this->IsIntersectedWith(R))
		return 0;	// No intersection  

	INT32 Index = 0;

	if (R.hi.y < hi.y)
		SubRects[Index++] = UserRect(lo.x, R.hi.y, hi.x, hi.y);	// Sub-Rect A
		
	if (R.lo.y > lo.y)
		SubRects[Index++] = UserRect(lo.x, lo.y, hi.x, R.lo.y);	// Sub-Rect D

	INT32 Ry0 = max(R.lo.y, lo.y);
	INT32 Ry1 = min(R.hi.y, hi.y);
	
	if (R.lo.x > lo.x)
		SubRects[Index++] = UserRect(lo.x, Ry0, R.lo.x, Ry1);			// Sub-Rect B		

	if (R.hi.x < hi.x)
		SubRects[Index++] = UserRect(R.hi.x, Ry0, hi.x, Ry1);			// Sub-Rect C		

	return Index;
}



/***********************************************************************************************

>	void UserRect::IncludePoint(const UserCoord& Point)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/5/93
	Inputs:		Point - coordinate to include into this rectangle.
	Outputs:	-
	Returns:	-
	Purpose:	Expand a rectangle so that it includes the given point.
	Errors:		An assertion failure if the rectangle is invalid.
	SeeAlso:	-

***********************************************************************************************/

void UserRect::IncludePoint(const UserCoord& Point)
{
    // Detect an invalid rectangle
	ENSURE(IsValid(), "UserRect::IncludePoint() was called on an \ninvalid rectangle.");  

	// Extend lower corner to include point if necessary
	lo.x = min(lo.x, Point.x);
	lo.y = min(lo.y, Point.y);
	
	// Extend upper corner to include point if necessary
	// (remember upper corner is exclusive)
	hi.x = max(hi.x, (INT32)(Point.x + 1L));
	hi.y = max(hi.y, (INT32)(Point.y + 1L));
}

// extra functions specific to the DocRect class

/*
$Log: userrect.def $
Revision 1.1  1995/09/12  17:11:49  Ed
Initial revision

*/

DECLARE_SOURCE("$Revision$");

#include "docrect.h"

/********************************************************************************************
>	DocRect UserRect::ToSpread(Spread* pSpread)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/9/95
	Inputs:		pSpread - 
	Returns:	SpreadRect
	Purpose:	Convert a UserRect to a SpreadRect (AKA DocRect)
********************************************************************************************/

DocRect UserRect::ToSpread(Spread* pSpread)
{
	return DocRect( this->lo.ToSpread(pSpread), this->hi.ToSpread(pSpread) );
}


