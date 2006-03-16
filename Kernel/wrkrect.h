// $Id: wrkrect.h 662 2006-03-14 21:31:49Z alex $
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

      
#ifndef INC_WRKRECT
#define INC_WRKRECT

#include "wrkcoord.h"


/***********************************************************************************************

>    Class WorkRect

     Author:       Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
     Date:    	   11/5/93
     
     Purpose:      This class is used to represent a rectangular region.  It uses a union to
     			   enable the client to access the rectangle as two WorkCoords (lo and hi) or as
     			   four XLONGs (lox, loy, hix, hiy).  An anonymous union is used, which is a
     			   defined C++ feature and so portable.

     			   The lo coordinates are inclusive, whilst the hi coordinates are exclusive.
     			   In the document space, lo.y <= hi.y and lo.x <= hi.x (the inequalities are 
     			   not strict because rectangles can have zero width and/or height - such
     			   rectangles are considered perfectly valid). 
     			   
     			   The inclusivity can be explained as follows:
     			   
     			   A point at (lo.x, lo.y) is inside the rectangle
     			   A point at (hi.x, hi.y) is outside the rectangle
     			   
     			   Any rectangle that does not obey the inequalites lo.y <= hi.y and 
     			   lo.x <= hi.x is deemed to be 'invalid'.  Invalid rectangles have special
     			   properties.  They will cause errors when used in some operations, but not
     			   in others.  The general rule is that if the rectangle is used as an entity,
     			   then the operation will succeed (taking account of the 'invalidity' of
     			   the rectangle).  If, however, the actual coordinates of the invalid
     			   rectangle must be used/changed in the operation, then an error (usually an
     			   assertion failure) will occur.
     			   
     			   For example, using invalid rectangles in Unions and Intersections is ok, 
     			   because the invalid rectangle is ignored, and the 'other' rectangle is
     			   returned as the result.  This means that if both rectangles are invalid,
     			   then an invalid rectangle is returned.
     			   
     			   Conversely, trying to use operations like Translate(), Inflate(), Width()
     			   or Height() on invalid rectangles is considered to be an error, and will
     			   cause an assertion failure.
     			   
     			   See the individual function descriptions for more details.
     			   
     			   
     Errors:	   -

     SeeAlso:	   WorkCoord

     SeeAlso:	   Rect
     SeeAlso:	   DocRect
     SeeAlso:	   WorkRect
     SeeAlso:	   OSRect
     			    
***********************************************************************************************/

class CAMAPI WorkRect
{   
public:

	WorkCoord			lo;
	WorkCoord			hi;

	                        
	// WorkRect constructors
	                    
	WorkRect();              

	WorkRect(XLONG LowX, XLONG LowY, XLONG HighX, XLONG HighY); 

	WorkRect(const WorkCoord& Low, XLONG Width, XLONG Height); 

	WorkRect(const WorkCoord& Low, const WorkCoord& High);   
			 		 
	WorkRect(const WorkRect& WorkRect); // Copy constructor

	WorkRect& operator=(const WorkRect& WorkRect);// Assignment operator
			 		 
	XLONG Width() const;
	XLONG Height() const; 
	
	WorkCoord LowCorner() const;
	WorkCoord HighCorner() const;  

	WorkCoord Centre() const;
	
	// General operations type stuff
	BOOL IsIntersectedWith(const WorkRect&) const;
	WorkRect Intersection(const WorkRect&) const;
	WorkRect Union(const WorkRect&) const;
	INT32 SplitRect(const WorkRect& R, WorkRect* SubRects);

	BOOL ContainsCoord(const WorkCoord&) const;
	BOOL ContainsRectCoord(const WorkCoord&) const;
	BOOL ContainsRect(const WorkRect&) const;
	BOOL IsAdjacent(const WorkRect&, MILLIPOINT Fuzzy) const;

	void MakeEmpty();
		
	BOOL IsEmpty() const;	
	BOOL IsValid() const;	

	void Inflate(XLONG XInc, XLONG YInc);
	void Inflate(XLONG XInc);
	void Translate(XLONG XOfs, XLONG YOfs);
	void IncludePoint(const WorkCoord&);
	
	// Overloaded operators
	INT32 operator==(const WorkRect&) const;
	INT32 operator!=(const WorkRect&) const;


};                                            


  
/*********************************************************************************************

>    void WorkRect::Translate(XLONG XOfs, XLONG YOfs)

     Author: 	Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
     Created:   17/5/93
     Inputs:	(XOfs, YOfs) - the offset to translate the rectangle by.
     Outputs:   - 
     
     Returns:   N/A.
     			
     Purpose:	Translate a rectangle by given offset.

     Errors:	An assertion failure if the rectangle is invalid.

**********************************************************************************************/  

inline void WorkRect::Translate(XLONG XOfs, XLONG YOfs)
{
    // Detect an invalid rectangle
	ENSURE(IsValid(), "WorkRect::Translate() was called on an \ninvalid rectangle.");  

	lo.x += XOfs;
	lo.y += YOfs;
    
	hi.x += XOfs;
	hi.y += YOfs;
}



/*********************************************************************************************

>    INT32 operator==(const WorkRect& R) const

     Author: 	Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
     Created:   17/5/93
     Inputs:	R - the rectangle to compare against.
     Outputs:   - 
     
     Returns:   TRUE if R is describes the same rectangle as the object.
     			
     Purpose:	Test for equality of two rectangles.  As all invalid rectangles have the
     			same results when used for Union/Intersection, any two invalid rectangles
     			are considered equal.

	 Friend:    WorkRect
	 
     Errors:	None.

**********************************************************************************************/  

inline INT32 WorkRect::operator==(const WorkRect& R) const
{
	// Invalid rectangles are equal
	if ((!IsValid()) && (!R.IsValid()))
		return TRUE;
		
	// Could use structure compare? Would it be portable? Probably not...
	
 	return ((lo.x == R.lo.x) && (lo.y == R.lo.y) &&
 	        (hi.x == R.hi.x) && (hi.y == R.hi.y));
}



/*********************************************************************************************

>    INT32 operator!=(const WorkRect& R) const

     Author: 	Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
     Created:   17/5/93
     Inputs:	R - the rectangle to compare against.
     Outputs:   - 
     
     Returns:   TRUE if R does not describe the same rectangle as the object.
     			
     Purpose:	Test for inequality of two rectangles.  As all invalid rectangles have the
     			same results when used for Union/Intersection, any two invalid rectangles
     			are considered equal.

	 Friend:    WorkRect
	 
     Errors:	None.

**********************************************************************************************/  

inline INT32 WorkRect::operator!=(const WorkRect& R) const
{
	// Invalid rectangles are equal
	if ((!IsValid()) && (!R.IsValid()))
		return FALSE;
		
 	return ((lo.x != R.lo.x) || (lo.y != R.lo.y) ||
 	        (hi.x != R.hi.x) || (hi.y != R.hi.y));
}



/*********************************************************************************************

>    void WorkRect::Inflate(XLONG XInc, XLONG YInc)

     Author: 	Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
     Created:   17/5/93
     Inputs:	XInc, YInc - the amount to inflate (or deflate) the rectangle by.
     Outputs:   - 
     
     Returns:   N/A.
     			
     Purpose:	Inflate a rectangle by given amounts.  Negative values will deflate the
     			rectangle.

     Errors:	An assertion failure if the rectangle is invalid.

**********************************************************************************************/  

inline void WorkRect::Inflate(XLONG XInc, XLONG YInc)
{
    // Detect an invalid rectangle
	ENSURE(IsValid(), "WorkRect::Inflate(XLONG, XLONG) was called on an \ninvalid rectangle.");  

	lo.x -= XInc;
	lo.y -= YInc;
    
	hi.x += XInc;
	hi.y += YInc;
}



/*********************************************************************************************

>    void WorkRect::Inflate(XLONG Inc)

     Author: 	Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
     Created:   17/5/93
     Inputs:	Inc - the amount to inflate (or deflate) the rectangle by.
     Outputs:   - 
     
     Returns:   N/A.
     			
     Purpose:	Inflate a rectangle by given amount.  Negative values will deflate the
     			rectangle.

     Errors:	An assertion failure if the rectangle is invalid.

**********************************************************************************************/  

inline void WorkRect::Inflate(XLONG Inc)
{
    // Detect an invalid rectangle
	ENSURE(IsValid(), "WorkRect::Inflate(XLONG) was called on an \ninvalid rectangle.");  

	lo.x -= Inc;
	lo.y -= Inc;
    
	hi.x += Inc;
	hi.y += Inc;
}



/*********************************************************************************************

>    WorkRect::WorkRect() 

     Author: 	Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
     Created:   13/5/93
     Inputs:	None
     Outputs:   -
     Returns:   - 
     			
     Purpose:	To construct an empty WorkRect.
     			

     Errors:	

**********************************************************************************************/  

inline WorkRect::WorkRect()
{
	// An empty rectangle
	hi.x = hi.y = lo.x = lo.y = 0;
}
     
	            

/*********************************************************************************************
           
>    WorkRect::WorkRect(XLONG LowX, XLONG LowY, XLONG HighX, XLONG HighY)

     Author: 	Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
     Created:   13/5/93
     Inputs:	LowX : Lower X coord of rectangle (inclusive)
     			HighX: Higher X coord of rectangle (exclusive) 
     			LowY : Lower Y coord of rectangle (inclusive)
     			HighY: Higher Y coord of rectangle (exclusive)
     			
     Outputs:   -
     Returns:   - 
     			
     Purpose:	To construct a WorkRect with an inclusive lower left hand corner 
     			position of (Left, Lower) and an exclusive upper right hand corner 
     			position of (Right, Upper).  

     Errors:	An assertion failure will occur if the lower left hand coordinates
     			are not lower than and to the left of the upper right coordinate. 

**********************************************************************************************/  
                      
inline WorkRect::WorkRect(XLONG LowX, XLONG LowY, XLONG HighX, XLONG HighY)
{                               
    // Defensive programming, detect an invalid rectangle
	ENSURE((LowX <= HighX) && (LowY <= HighY),
	       "WorkRect::WorkRect(XLONG, XLONG, XLONG, XLONG) was\n passed invalid coordinates");  
	
	lo.x = LowX;
	lo.y = LowY; 
	
	hi.x = HighX; 
	hi.y = HighY; 	
} 



/*********************************************************************************************
           
>    WorkRect::WorkRect(const WorkCoord& Low, const WorkCoord& High) 

     Author: 	Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
     Created:   13/5/93
     Inputs:	Low : WorkCoordinates of the lower left hand corner (inclusive) 
     			High: WorkCoordinates of the upper right hand corner (exclusive)
     Outputs:   -
     Returns:   - 
     			
     Purpose:	To construct a rectangle with an inclusive lower left hand corner 
     			position of Low and an exclusive upper right hand corner
     			position of High.

     Errors:	An assertion failure will occur if the lower left hand coordinates
     			are not lower than and to the left of the upper right coordinates. 

**********************************************************************************************/  

inline WorkRect::WorkRect(const WorkCoord& Low, const WorkCoord& High)
{         
    // Defensive programming, detect an invalid rectangle
	ENSURE((Low.x <= High.x) && (Low.y <= High.y),
	       "WorkRect::WorkRect(WorkCoord, WorkCoord) was\n passed invalid coordinates");  
    
	lo = Low;
	hi = High;
}       
    


/*********************************************************************************************
           
>    WorkRect::WorkRect(const WorkCoord& Low, XLONG Width, XLONG Height) 

     Author: 	Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
     Created:   17/5/93
     Inputs:	Low: WorkCoordinates of the inclusive lower left hand corner.
     			Width, Height  : Desired dimensions of the rectangle.
     Outputs:   -
     Returns:   - 
     			
     Purpose:	To construct a rectangle with an inclusive lower left hand corner 
     			position of Low and a width and height as specified.

     Errors:	None.
     
**********************************************************************************************/  

inline WorkRect::WorkRect(const WorkCoord& Low, XLONG Width, XLONG Height) 
{         
	lo = Low;
	
	hi.x = lo.x + Width;
	hi.y = lo.y + Height;
}       
   
   
    
       
/*********************************************************************************************
           
>    WorkRect::WorkRect(const WorkRect& R) 

     Author: 	Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
     Created:   13/5/93
     Inputs:	R: The copy of the WorkRect 
     Outputs:   -
     Returns:   - 
     			
     Purpose:	Copy constructor 

**********************************************************************************************/  

inline WorkRect::WorkRect(const WorkRect& R)
{
	lo = R.lo;
	hi = R.hi;
}



/*********************************************************************************************
           
>    WorkRect& WorkRect::operator=(const WorkRect& WorkRect)

     Author: 	Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
     Created:   13/5/93
     Inputs:	WorkRect: WorkRect to copy 
     Outputs:   -
     Returns:   Reference to this WorkRect 
     			
     Purpose:	Equals operator 

**********************************************************************************************/  

inline WorkRect& WorkRect::operator=(const WorkRect& WorkRect)
{                                                    
    lo = WorkRect.lo; 
    hi = WorkRect.hi;
    
	return *this; 
}



/*********************************************************************************************
           
>    XLONG WorkRect::Width() const

     Author: 	Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
     Created:   13/5/93
     Inputs:	- 
     Outputs:   -
     Returns:   The width of the WorkRect 
     			
     Purpose:	To find the width of the WorkRect 

**********************************************************************************************/  
			 		 
inline XLONG WorkRect::Width() const
{
    // Detect an invalid rectangle
	ENSURE(IsValid(), "WorkRect::Width() was called on\nan invalid rectangle.");  

	return (hi.x - lo.x); 	
}       



/*********************************************************************************************
           
>    XLONG WorkRect::Height() const

     Author: 	Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
     Created:   13/5/93
     Inputs:	- 
     Outputs:   -
     Returns:   The height of the WorkRect 
     			
     Purpose:	To find the height of the WorkRect 

**********************************************************************************************/  

inline XLONG WorkRect::Height() const 
{                      
    // Detect an invalid rectangle
	ENSURE(IsValid(), "WorkRect::Height() was called on\nan invalid rectangle.");  

	return(hi.y - lo.y); 
}
	                 


/*********************************************************************************************
           
>    WorkCoord WorkRect::LowCorner() const
     Author: 	Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
     Created:   13/5/93
     Inputs:	- 
     Outputs:   -
     Returns:   The inclusive lower left hand coordinates of the WorkRect
     			
     Purpose:	To find the lower left hand coordinates of the WorkRect
     
     Errors:	Assertion failure if the rectangle is invalid.

**********************************************************************************************/  
	                 
inline WorkCoord WorkRect::LowCorner() const
{     
    // Detect an invalid rectangle
	ENSURE(IsValid(), "WorkRect::LowCorner() was called on\nan invalid rectangle.");  

	return(lo); 
}        



/*********************************************************************************************
           
>    WorkCoord WorkRect::HighCorner() const  

     Author: 	Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
     Created:   13/5/93
     Inputs:	- 
     Outputs:   -
     Returns:   The exclusive upper right hand coordinates of the WorkRect 
     			
     Purpose:	To find the upper right hand coordinates of the WorkRect

     Errors:	Assertion failure if the rectangle is invalid.

**********************************************************************************************/  

inline WorkCoord WorkRect::HighCorner() const  
{      
    // Detect an invalid rectangle
	ENSURE(IsValid(), "WorkRect::HighCorner() was called on\nan invalid rectangle.");  

	return(hi); 
}                  



/*********************************************************************************************
           
>    WorkCoord WorkRect::Centre() const

     Author: 	Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
     Created:   19/5/99
     Inputs:	- 
     Outputs:   -
     Returns:   The centre coord of this WorkRect
     			
     Purpose:	To find the centre of the WorkRect
				It calculates WorkCoord(lox+(width/2),loy+(height/2))
     
     Errors:	Assertion failure if the rectangle is invalid.

**********************************************************************************************/  
	                 
inline WorkCoord WorkRect::Centre() const
{     
    // Detect an invalid rectangle
	ENSURE(IsValid(), "WorkRect::Centre() was called on\nan invalid rectangle.");  

	return WorkCoord(lo.x + Width() / 2, lo.y + Height() / 2); 
}        



/*********************************************************************************************

>    BOOL WorkRect::IsIntersectedWith(const WorkRect& WorkRect) const

     Author: 	Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
     Created:   17/5/93
     Inputs:	-  
     Outputs:   - 
     
     Returns:   TRUE if the rectangles intersect, FALSE otherwise.
     			
     Purpose:	To check for rectangle intersection 

     Errors:	

**********************************************************************************************/  

inline BOOL WorkRect::IsIntersectedWith(const WorkRect& R) const
{
    // Detect an invalid rectangle
    if ((!IsValid()) || (!R.IsValid()))
    	return FALSE;
    	
	return ((hi.x > R.lo.x) && (lo.x < R.hi.x) &&
			(hi.y > R.lo.y) && (lo.y < R.hi.y));
}



/*********************************************************************************************

>    BOOL WorkRect::ContainsCoord(const WorkCoord& Point)

     Author: 	Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
     Created:   17/5/93
     Inputs:	-  
     Outputs:   - 
     
     Returns:   TRUE if the coordinate is within the rectangle, FALSE otherwise.
     			
     Purpose:	To check for coordinate containment.

     Errors:	Assertion failure if the rectangle is invalid.

     SeeAlso:	ContainsRectCoord; ContainsRect

**********************************************************************************************/  

inline BOOL WorkRect::ContainsCoord(const WorkCoord& Point) const
{
	// Check for an an empty rectangle
	if (IsEmpty())
		return FALSE;

    // Detect an invalid rectangle
	ENSURE(IsValid(), "WorkRect::ContainsCoord() was called on\nan invalid rectangle.");  

	return ((Point.x >= lo.x) && (Point.x < hi.x) &&
	        (Point.y >= lo.y) && (Point.y < hi.y));
}



/*********************************************************************************************

>    BOOL WorkRect::ContainsRectCoord(const WorkCoord& Point)

     Author: 	Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
     Created:   17/5/93
     Inputs:	-  
     Outputs:   - 
     
     Returns:   TRUE if the coordinate is within the rectangle, FALSE otherwise.
     			
     Purpose:	To check for coordinate containment.  This will work for coordinates which
     			have been extracted from other rectangles (i.e. the top right corner is
     			considered inclusive for this operation, not exclusive).

     Errors:	Assertion failure if the rectangle is invalid.

     SeeAlso:	ContainsCoord; ContainsRect

**********************************************************************************************/  

inline BOOL WorkRect::ContainsRectCoord(const WorkCoord& Point) const
{
	// Check for an an empty rectangle
	if (IsEmpty())
		return FALSE;

    // Detect an invalid rectangle
	ENSURE(IsValid(), "WorkRect::ContainsRectCoord() was called on\nan invalid rectangle.");  

	return ((Point.x >= lo.x) && (Point.x <= hi.x) &&
	        (Point.y >= lo.y) && (Point.y <= hi.y));
}



/*********************************************************************************************

>    BOOL WorkRect::ContainsRect(const WorkRect& Rect)

     Author: 	Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
     Created:   17/5/93
     Inputs:	-  
     Outputs:   - 
     
     Returns:   TRUE if the rectangle 'Rect' is within the rectangle, FALSE otherwise.
     			
     Purpose:	To check for rectangle containment.

     Errors:	Assertion failure if the rectangle is invalid.

     SeeAlso:	ContainsRectCoord; ContainsCoord

**********************************************************************************************/  

inline BOOL WorkRect::ContainsRect(const WorkRect& Rect) const
{
	// Check for an an empty rectangle
	if (IsEmpty())
		return FALSE;

    // Detect an invalid rectangle
	ENSURE(IsValid(), "WorkRect::ContainsRect() was called on\nan invalid rectangle.");  

	return ((Rect.lo.x >= lo.x) && (Rect.hi.x <= hi.x) &&
	        (Rect.lo.y >= lo.y) && (Rect.hi.y <= hi.y));
}



/********************************************************************************************

>	void WorkRect::MakeEmpty()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/03/94
	Purpose:	Make the rectangle an empty one (all coordinates are set to 0).
	SeeAlso:	WorkRect::MakeEmpty

********************************************************************************************/

inline void WorkRect::MakeEmpty()
{
  lo.x = lo.y = hi.x = hi.y = 0;
}



/*********************************************************************************************

>    BOOL WorkRect::IsEmpty() const

     Author: 	Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
     Created:   17/5/93
     Inputs:	-  
     Outputs:   - 
     
     Returns:   TRUE if the rectangle is empty.
     			
     Purpose:	To check for empty rectangle.

     Errors:	

**********************************************************************************************/  

inline BOOL WorkRect::IsEmpty() const
{
	return ((lo.x == hi.x) || (lo.y == hi.y));
}



/*********************************************************************************************

>    BOOL WorkRect::IsValid() const

     Author: 	Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
     Created:   9/6/93
     Inputs:	-  
     Outputs:   - 
     
     Returns:   TRUE if the rectangle is valid.
     			
     Purpose:	To check for a valid rectangle.

     Errors:	

**********************************************************************************************/  

inline BOOL WorkRect::IsValid() const
{
	return ((lo.x <= hi.x) && (lo.y <= hi.y));
}

#endif

