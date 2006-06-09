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
//#include "doccoord.h" - in camtypes.h [AUTOMATICALLY REMOVED]

#if !defined(EXCLUDE_FROM_XARLIB)
#include "usercord.h"
//#include "node.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "paper.h"
//#include "xmatrix.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "spread.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "chapter.h"
//#include "oilcoord.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "view.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "document.h" - in camtypes.h [AUTOMATICALLY REMOVED]


/********************************************************************************************

>	Matrix ComposeDocToOilMat(	DocCoord& ChapterPos,
                         		XLONG& ChapterLogHiY,
                          		FIXED16& ViewScale,
                          		WorkCoord& Scroll,
                        	)
	Author: 	Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/8/93
	Inputs:		Chapter physical position
				Accumulated depth of previous chapters
				View scale factor (always 1:1 aspect ratio) 
				Individual millipoint to pixel scale factors
				Scroll offsets
				Final device-dependant mapping matrix
	Outputs:	None.
	Returns:	Document to Oil matrix for a specific chapter.
	Purpose:	Creates a Document to Oil matrix for a specific chapter.
	Errors:		None.

********************************************************************************************/
/*
Technical notes:
	Assumes that the top corner of logical space is at 0,0 and y gets more negative downwards
	Assumes that the top corner of workarea space is at 0,0...
	Assumes that scroll.x is positive and that scroll.y is negative.
	Assumes that Document space can never be rotated with respect to OS space!

********************************************************************************************/

Matrix ComposeDocToOilMat( const DocCoord& ChapterPos,	// Top left coord of chapter
                          const XLONG& ChapterDepth,		// Accumulated height of previous chaps
                          const FIXED16& ViewScale,		// User viewing scale
                          const WorkCoord& ScrollOffset	// Scroll offsets
                        )
{
// Scale ChapterDepth into device units so that it can be combined with the scroll offset
// BEFORE we build the scroll translation matrix. In this way we avoid storing 64-bit
// translation values in our final matrix...
//
// If we COULD store 64-bit E and F values temporarilly during this composition then
// we would not need to perform this trick at all. This would have the knock-on advantage
// that the OSMapping matrix supplied by the OIL layer could contain the scroll offsets!
//
// NOTE: This adjustment can be removed when testing the system!

    Matrix DocToOil;

//**********************************************************************
// Is it just me or is there gunna be a problem with this ?
// If ChapterDepth is big (atfer all it is an XLONG) and ViewScale
// is big too (Say we are zoomed to 1000%) won't this go VERY wrong ?
//
// - Will.

    Matrix Scroll(	-ScrollOffset.x,
    				-(ScrollOffset.y + (ChapterDepth * (ViewScale)))
    			 );

//**********************************************************************

    Matrix ScaleMat(ViewScale,ViewScale);
	Matrix Translate(-((Coord)ChapterPos));

// The following matrix compositions MUST be performed in this order. If you are tempted
// to optimise this code MAKE SURE THAT THEY ARE STILL IN THIS ORDER WHEN YOU'VE FINISHED!
//
// Apply translation to get coordinates into logical column...
    DocToOil *= Translate;

// Apply scale factors to convert from millipoint distances to pixel distances...
    DocToOil *= ScaleMat;

// Apply scroll-offset translation to move origin to viewing position...
    DocToOil *= Scroll;

    return DocToOil;
}


/********************************************************************************************

>	Matrix ComposeDocToWorkXMat(DocCoord ChapterPos,
                         		XLONG ChapterDepth,
                          		FIXED16 ViewScale
                        	)
	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/5/93
	Inputs:		Chapter physical position
				Accumulated depth of previous chapters
				View scale factor (always 1:1 aspect ratio) 
				Individual millipoint to pixel scale factors
	Outputs:	None.
	Returns:	Document to WorkArea matrix for a specific chapter.
	Purpose:	Creates a Document to WorkArea matrix for a specific chapter. 
	Errors:		None.

********************************************************************************************/
/*
Technical notes:
	Assumes that the top corner of logical space is at 0,0 and y gets more negative downwards
	Assumes that the top corner of workarea space is at 0,0...
	Assumes that scroll.x is positive and that scroll.y is negative.

********************************************************************************************/

XMatrix ComposeDocToWorkXMat(const DocCoord ChapterPos,	// Top left coord of chapter
                          const XLONG ChapterDepth,		// Accumulated height of previous chaps
                          const FIXED16 ViewScale		// User viewing scale
                        )
{

    XMatrix ScaleMat(ViewScale,ViewScale);

// Conversion of chapter position from DocCoords to WorkCoords
//    WorkCoord WrkChapPos(MakeXLong(ChapterPos.x), MakeXLong(ChapterPos.y));
    WorkCoord WrkChapPos(MakeXLong(ChapterPos.x), MakeXLong(ChapterPos.y)+ChapterDepth);

// DocToOS matrix is initialised as the Translate matrix: saving space and performance
// by the exclusion of an extra multiplication.
	XMatrix DocToWork(-(WrkChapPos));
    
// The following matrix compositions MUST be performed in this order. If you are tempted
// to optimise this code MAKE SURE THAT THEY ARE STILL IN THIS ORDER WHEN YOU'VE FINISHED!
//

// Apply scale factors to convert from millipoint distances to pixel distances...
    DocToWork *= ScaleMat;

    return DocToWork;
}

/********************************************************************************************

>	WorkCoord	DocCoord::ToWork(Document *pDocument, View *pView)

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com> (with modifications by Phil (and Tim))
	Created:	26/5/93
	Inputs:		The document we are converting within.
				The view we are converting within.
	Outputs:	None.
	Returns:	Returns a Workarea coordinate.
	Purpose:	Converts a document coordinate to a work area coordnate - slowly!
				Note that if you are performing this conversion often and you want to do it
				fast then you should use the approriate "Compose" function to build a matrix
				which does the job and repeatedly apply that matrix.
	Errors:		None.

********************************************************************************************/
/*
Technical notes:

********************************************************************************************/

WorkCoord DocCoord::ToWork(Document *pDocument, View *pView)
{
	WorkCoord 	WrkPoint(MakeXLong(this->x), MakeXLong(this->y));
	DocCoord	ChapterPos;				// To hold the top-left of the found chapter
	XLONG		ChapterDepth;			// To hold the accumulated depth of the chapter
	XMatrix		ConvertToWorkMat;		// Transformation matrix
	
	// Find chapter and its log depth
	this->FindEnclosingChapter(pDocument, &ChapterPos, &ChapterDepth, pView);
	
	// This builds the 64-bit conversion matrix needed for Document To WorkArea 
	// coordinate mappings.
	ConvertToWorkMat = ComposeDocToWorkXMat(ChapterPos, 
											ChapterDepth,
											pView->GetViewScale());
	
	// Apply the matrix to the DocCoord...
	ConvertToWorkMat.transform(&WrkPoint, 1);

	return (WrkPoint);
}

/********************************************************************************************

>	WorkCoord DocCoord::ToWork( Spread *pSpread, const FIXED16 ViewScale)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/12/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Convert a document coordinate to a Work Coordinate, given the spread
				that the coordinate is on.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

WorkCoord DocCoord::ToWork( Spread *pSpread, View *pView)
{
	WorkCoord 	WrkPoint(MakeXLong(this->x), MakeXLong(this->y));
	DocCoord	ChapterPos;				// To hold the top-left of the found chapter
	XLONG		ChapterDepth;			// To hold the accumulated depth of the chapter
	XMatrix		ConvertToWorkMat;		// Transformation matrix
	
	Chapter *pChapter = (Chapter *) pSpread->FindParent();
	ENSURE(pChapter->IsKindOf(CC_RUNTIME_CLASS(Chapter)), "Spread's parent is not a chapter");


	// Find top left of chapter pasteboard
	DocRect ChapterRect = pChapter->GetPasteboardRect(TRUE, pView);
	ChapterPos.x = ChapterRect.lo.x;
	ChapterPos.y = ChapterRect.hi.y;

	// Find chapter depth
	ChapterDepth = pChapter->GetChapterDepth();
	
	// This builds the 64-bit conversion matrix needed for Document To WorkArea 
	// coordinate mappings.

	ConvertToWorkMat = ComposeDocToWorkXMat(ChapterPos, 
											ChapterDepth,
											pView->GetViewScale());
	
	// Apply the matrix to the DocCoord...
	ConvertToWorkMat.transform(&WrkPoint, 1);

	return (WrkPoint);
}


WorkCoord DocCoord::ToLogical(Spread* pSpread, View *pView)
{
	// Check that we have not been fed garbage
	ENSURE( pSpread!=NULL, "DocCoord::ToLogical was passed a NULL spread" );

	// Find the chapter
	Chapter* pChapter = (Chapter*) pSpread->FindParent();
	ENSURE( pChapter!=NULL, "DocCoord::ToLogical. Spread did not have a parent chapter" );
	ENSURE( pChapter->IsKindOf(CC_RUNTIME_CLASS(Chapter)), "DocCoord::ToLogical. Chapter is not a chapter" );

	// Find out about the chapter
	DocRect ChapterRect = pChapter->GetPasteboardRect(TRUE, pView);
	XLONG   ChapterDepth = pChapter->GetChapterDepth();

	// Build the logical coord from the doccoord
	WorkCoord Result( MakeXLong(x), MakeXLong(y) );
	Result.x = Result.x - ChapterRect.lo.x;
	Result.y = Result.y - ChapterRect.hi.y;
	Result.y = Result.y - ChapterDepth;

	// return the coord
	return Result;
}


/***********************************************************************************************
>	DocCoord DocCoord::ToSpread(Spread* pSpread, View* pView)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/9/95
	Inputs:		pSpread  - 
				pView - 
	Returns:	SpreadCoord
	Purpose:    Convert DocCoord to a SpreadCoord

	SeeAlso:	Spread::SpreadCoordToDocCoord
*********************************************************************************************/

DocCoord DocCoord::ToSpread(Spread* pSpread, View* pView)
{
	ERROR3IF((pSpread==NULL || pView==NULL), "DocCoord::ToSpread() - pSpread==NULL || pView==NULL");

	DocCoord temp(x, y);

	if (pSpread != NULL)
		pSpread->DocCoordToSpreadCoord(&temp);

	return(temp);
}
 

/***********************************************************************************************
>	DocCoord DocCoord::ToDoc(Spread* pSpread, View* pView)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/9/95
	Inputs:		pSpread  - 
				pView - 
	Returns:	SpreadCoord
	Purpose:    Convert SpreadCoord to a DocCoord

	SeeAlso:	Spread::SpreadCoordToDocCoord
*********************************************************************************************/

DocCoord DocCoord::ToDoc(Spread* pSpread, View* pView)
{
	ERROR3IF(pSpread == NULL || pView == NULL, "DocCoord::ToDoc() - pSpread==NULL || pView==NULL");

	DocCoord temp(x, y);

	if (pSpread != NULL)
		pSpread->SpreadCoordToDocCoord(&temp);

	return(temp);
}
 

/***********************************************************************************************
>	UserCoord DocCoord::ToUser(Spread* pSpread)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/9/95
	Inputs:		pSpread  - 
	Returns:	UserCoord
	Purpose:    Convert SpreadCoord to a UserCoord
*********************************************************************************************/

UserCoord DocCoord::ToUser(Spread* pSpread)
{
	DocCoord UserOrigin(0,0);
	if (pSpread!=NULL)
		UserOrigin=pSpread->GetUserOrigin();
	else
		ERROR3("DocCoord::ToUser() - pSpread==NULL");

	return UserCoord(this->x-UserOrigin.x, this->y-UserOrigin.y);
}
 

/***********************************************************************************************
>	OilCoord DocCoord::ToOil(Spread* pSpread, View* pView)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/9/95
	Inputs:		pSpread  - 
				pView - 
	Returns:	OilCoord
	Purpose:    Convert SpreadCoord (AKA DocCoord) to an OilCoord
*********************************************************************************************/

OilCoord DocCoord::ToOil(Spread* pSpread, View* pView)
{
	WorkCoord ScrollOffsets(0,0);
	if (pView!=NULL)
		ScrollOffsets=pView->GetScrollOffsets();
	else
		ERROR3("DocCoord::ToOil() - pView==NULL");

	return ToDoc(pSpread,pView).ToWork(pSpread,pView).ToOil(ScrollOffsets);
}
 

/***********************************************************************************************

>	Chapter* DocCoord::FindEnclosingChapter(Document *pDocument, 
											DocCoord* ChapterPos, 
											XLONG* ChapterDepth);  

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/4/93
	Inputs:		pDocument - the document we are converting in.
				pView - the View to use for pixelising.
	Outputs:	ChapterPos:   The top left hand corner of the chapters pasteboard rectangle 
				ChapterDepth: The Sum of the lengths of all preceding chapters   
				
	Returns:	If this coord is enclosed within a chapter then the chapter is returned               
				else NULL is returned (In the non DEBUG version). 
				 
	Purpose:    For finding the enclosing chapter of a DocCoord, the position of the start 
				of the enclosing chapter, and the chapters depth.

	Scope:		Private

	Errors:		An ERROR3 will occur if the DocCoord is not in any chapter
				NOTE: The Spread Coordinate Origin for a spread MAY NOT reside INSIDE a
				Chapter's Pasteboard bounds! See Spread::GetSpreadCoordOrigin for full
				details of this.

	SeeAlso:	-

*********************************************************************************************/
 
Chapter* DocCoord::FindEnclosingChapter(Document *pDocument, 
										DocCoord* ChapterPos, 
										XLONG* ChapterDepth,
										View *pView)
{
	Chapter* CurrentChapter = Node::FindFirstChapter(pDocument);
	// Search all chapters
	*ChapterDepth = 0;
	DocRect CurrentChaptersPasteboardRect;

	while (CurrentChapter != NULL)
	{
		CurrentChaptersPasteboardRect = CurrentChapter->GetPasteboardRect(TRUE, pView);

		if (CurrentChaptersPasteboardRect.ContainsRectCoord(*this))
		{
			// Chapter position is the top left hand corner of the chapters pasteboard
			ChapterPos->x = CurrentChaptersPasteboardRect.LowCorner().x;
			ChapterPos->y = CurrentChaptersPasteboardRect.HighCorner().y;
			return (CurrentChapter);
		}

        (*ChapterDepth) += CurrentChaptersPasteboardRect.Height();
		CurrentChapter = CurrentChapter->FindNextChapter();
    }

	// The DocCoord was not found in any chapter
	ERROR3("DocCoord::FindEnclosingChapter: Coord wasn't in any chapter. See the function help for debugging tips");
	return(NULL);
}






/*********************************************************************************************
>	BOOL DocCoord::Pixelise(View* pView)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/11/94
	Inputs:		The view we are pixelising for.
	Purpose:	Modifies this DocCoord to lie on an exact pixel boundary.
	Scope:		Public
	SeeAlso:	DocCoord::SetScaledPixelSize()

**********************************************************************************************/
/*
	Technical notes:
	This routine ensures that the coordinate is aligned to a whole pixel boundary at all times
	by dividing and re-multiplying by the pixel size (YUK).

	Note that when a negative value is divided in C++ it is done so without regard for the
	sign of the value.
	E.g.
		21 / 4 =  5	Result has been truncated towards 0, moving Y coordinates down
		-21/ 4 = -5 Result has been truncated towards 0 again, moving Y coordinates UP!!!
	In other words when the values are coordinates the division truncates them in different
	directions depending which side of the origin they are on!
	This is obviously not required and so any negative values are adjusted before the division
	to ensure the end result is movement in the same direction as positive values.

	Range of scaled pixel width/height:
		Min:	ScaledPixelsize = 1(millipoint) / 40 (4000%) = 0.025
		Max:	ScaledPixelSize = 2000 (1/36th of an inch) / 0.05 (5%) = 40,000
	These values fit nicely into a Fixed16 so they should be correct on the way into the
	pixelising maths. However, Fixed16s cannot be combined with INT32s easily and they would not
	be accuarate enough to hold the possible pixel values. So, the pixelising maths is all done
	using floating point doubles.

********************************************************************************************/

BOOL DocCoord::Pixelise(View* pView)
{
	// See if the view is ok
	if (pView==NULL)
		return FALSE;

	FIXED16 pw,ph;
	pView->GetScaledPixelSize(&pw,&ph);
	x = (INT32) ( (floor(0.5 + (double)x / pw.MakeDouble())) * pw.MakeDouble() );
	y = (INT32) ( (floor(0.5 + (double)y / ph.MakeDouble())) * ph.MakeDouble() );

	return TRUE;
}


/*********************************************************************************************
>	BOOL DocCoord::IsPixelised(View* pView)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/11/94
	Inputs:		The view we are pixelising for.
	Outputs:	-
	Returns:	TRUE if this DocCoord is already pixelised
				FALSE otherwise
	Purpose:	Modifies this DocCoord to lie on an exact pixel boundary.
	Scope:		Public

**********************************************************************************************/

BOOL DocCoord::IsPixelised(View* pView)
{
	// See if the view is ok
	if (pView==NULL)
		return FALSE;

	// Now pixelise a temp copy of this coord and see if it changed at all...
	DocCoord temp(x,y);
	temp.Pixelise(pView);
	return (x==temp.x && y==temp.y);
}


/*********************************************************************************************

>	static BOOL DocCoord::Pixelise()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/11/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Modifies this DocCoord to lie on an exact pixel boundary.
	Errors:		-
	Scope:		Public
	SeeAlso:	DocCoord::SetScaledPixelSize()

**********************************************************************************************/

// NB. This is a bodge until I fix all the callers of Pixelise to pass in the View (Tim).

BOOL DocCoord::Pixelise()
{
	// BODGE: ...until I can be bothered to get callers to pass view in...  (Tim)
	return Pixelise(View::GetCurrent());
}

#endif	// EXCLUDE_FROM_XARLIB

/********************************************************************************************

>	DocCoord operator + (const DocCoord& One, const DocCoord& Two)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/12/94
	Inputs:		One - The first DocCoord
				Two - The second DocCoord
	Returns:	The result of adding the two DocCoords together.
	Purpose:	Adds two DocCoords together. ie One.x+Two.x, One.y+Two.y

********************************************************************************************/

DocCoord operator + (const DocCoord& One, const DocCoord& Two)
{
	DocCoord Result;

	// Add the two DocCoords together
	Result.x = One.x + Two.x;
	Result.y = One.y + Two.y;

	// return the result
	return Result;
}


/********************************************************************************************

>	DocCoord operator - (const DocCoord& One, const DocCoord& Two)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/12/94
	Inputs:		One - The first DocCoord
				Two - The second DocCoord
	Returns:	the DocCoord that is the first DocCoord minus the second DocCoord
	Purpose:	Subtracts the two DocCoords.  ie One.x-Two.x, One.y-Two.y

********************************************************************************************/

DocCoord operator - (const DocCoord& One, const DocCoord& Two)
{
	DocCoord Result;

	// Subtract the two DocCoords from each other
	Result.x = One.x - Two.x;
	Result.y = One.y - Two.y;

	// return the result
	return Result;
}

/********************************************************************************************

>	const DocCoord& DocCoord::operator += ( const DocCoord &Other )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/4/00
	Inputs:		Other	- The other DocCoord value, that's being added to the original.
	Returns:	A reference to the current object, for concatentation
	Purpose:	Adds a DocCoord value onto the current object.

********************************************************************************************/

const DocCoord& DocCoord::operator += ( const DocCoord &Other )
{
	// Add the other DocCoord's values onto this object's.
	x += Other.x;
	y += Other.y;

	return *this;
}

/********************************************************************************************

>	const DocCoord& DocCoord::operator -= ( const DocCoord &Other )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/4/00
	Inputs:		Other	- The other DocCoord value, that's being subtracted from this one.
	Returns:	A reference to the current object, for concatentation
	Purpose:	Subtracts a DocCoord value from the current object.

********************************************************************************************/

const DocCoord& DocCoord::operator -= ( const DocCoord &Other )
{
	// Add the other DocCoord's values onto this object's.
	x -= Other.x;
	y -= Other.y;

	return *this;
}

/*******************************************************************************************
>	static DocCoord DocCoord::PositionPointFromRatio(const DocCoord& StartPoint, const DocCoord& EndPoint, const double Ratio)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/95
	Inputs:		StartPoint and EndPoint define the start and end of a line
				Ratio is the ratio of the distance from start to end that you want the coordinate of
	Outputs:	-
	Returns:	The coordinate of the position of the ratio (err, does that make sense?)
	Purpose:	Gives the coordinate of a point specified by a ratio along a line defined by
				two other points.  A ratio of zero gives the same point as the start of the
				line, 0.5 gives the mid point of the line, etc.  Negative ratio give a point
				before the start, ratios greater than 1 give points beyond the end
********************************************************************************************/
DocCoord DocCoord::PositionPointFromRatio(const DocCoord& StartPoint, const DocCoord& EndPoint, const double Ratio)
{
	DocCoord result;
	
	result.x = StartPoint.x + (INT32)((EndPoint.x - StartPoint.x)*Ratio) ;
	result.y = StartPoint.y + (INT32)((EndPoint.y - StartPoint.y)*Ratio) ;

	return result;
} 


/*******************************************************************************************
>	static double DocCoord::DistanceFromLine(const DocCoord& Line1, const DocCoord& Line2, const DocCoord& Point)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/95
	Inputs:		Line1 and Line2 are two points defining a line that passes through those points
				Point is another point
	Outputs:	-
	Returns:	The perpendicular distance from the line to the point. 
	Purpose:	Gets the shortest distance from the line to the point.
	Errors:		Will return -1 if a division by zero case was detected
********************************************************************************************/
double DocCoord::DistanceFromLine(const DocCoord& Line1, const DocCoord& Line2, const DocCoord& Point)
{
	// Obtain perpendicular distance from the axes line (see A-Level maths page 24)
	const double X1 = Line1.x;
	const double Y1 = Line1.y;
	const double X2 = Line2.x;
	const double Y2 = Line2.y;

	const double a = -Y2+Y1;
	const double b =  X2-X1;
	const double c = -(Y1*X2) + (Y1*X1) + (X1*Y2) - (X1*Y1);

	const double divisor = sqrt( a*a + b*b );
	
	// We don't want any nasty floating point div 0, thank you
	double Distance = -1.0;
	if (divisor >= 0.0001)
		Distance = fabs(( a*Point.x + b*Point.y + c )/divisor);	   

	return Distance;
}



/*******************************************************************************************
>	static DocCoord DocCoord::OneThird(const DocCoord& Start, const DocCoord& End)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/95
	Inputs:		Start and End are two coordinates
	Outputs:	-
	Returns:	The point one third of the way from Start to End
	Purpose:	For getting a point one third of the way between two other points
********************************************************************************************/
DocCoord DocCoord::OneThird(const DocCoord& Start, const DocCoord& End)
{
	DocCoord result;

	result.x = Start.x + (INT32)((End.x - Start.x)/3) ;
	result.y = Start.y + (INT32)((End.y - Start.y)/3) ;

	return result;
}



/*******************************************************************************************
>	static DocCoord DocCoord::TwoThirds(const DocCoord& Start, const DocCoord& End)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/95
	Inputs:		Start and End are two coordinates
	Outputs:	-
	Returns:	The point two thirds of the way from Start to End
	Purpose:	For getting a point two thirds of the way between two other points
********************************************************************************************/
DocCoord DocCoord::TwoThirds(const DocCoord& Start, const DocCoord& End)
{
	DocCoord result;

	result.x = Start.x + (INT32)((End.x - Start.x)*(2.0/3.0)) ;
	result.y = Start.y + (INT32)((End.y - Start.y)*(2.0/3.0)) ;

	return result;
}



/*******************************************************************************************
>	static DocCoord DocCoord::OneHalf(const DocCoord& Start, const DocCoord& End)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/95
	Inputs:		Start and End are two coordinates
	Outputs:	-
	Returns:	The point one half of the way from Start to End
	Purpose:	For getting a point midway between two other points
********************************************************************************************/
DocCoord DocCoord::OneHalf(const DocCoord& Start, const DocCoord& End)
{
	DocCoord result;

	result.x = Start.x + (INT32)((End.x - Start.x)/2) ;
	result.y = Start.y + (INT32)((End.y - Start.y)/2) ;

	return result;
}



/********************************************************************************************
>	BOOL DocCoord::IsJoinAcute(const DocCoord* pJoin, const DocCoord* pOtherEndOfJoin, const DocCoord* pOtherPoint)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/95
	Inputs:		pJoin - the coordinate where two lines join
				pOtherEndOfJoin - another point that with pJoin defines a line
				pOtherPoint - another point that with pJoin defines another line
	Outputs:	-
	Returns:	TRUE if the angle between the two lines is an acute angle
	Purpose:	For seeing the perpendicual distance from pOtherPoint to the line
				pJoin-pOtherEndOfJoin end on the line or before pJoin.  Used when dragging
				along a line to see if the drag point has gon back beyond the start of the
				line.
********************************************************************************************/
BOOL DocCoord::IsJoinAcute(const DocCoord* pJoin, const DocCoord* pOtherEndOfJoin, const DocCoord* pOtherPoint)
{
	// Translate the points so the join is on the origin and the other points are relative to that
	DocCoord OriginLine = *pOtherEndOfJoin - *pJoin;
	DocCoord OriginOther = *pOtherPoint - *pJoin;
	double LineAngle = atan2((double)OriginLine.y, (double)OriginLine.x);
	double OtherAngle = atan2((double)OriginOther.y, (double)OriginOther.x);

	double diff = OtherAngle - LineAngle;
	if (diff < -PI)
		diff += 2*PI;
	if (diff > PI)
		diff -= 2*PI;

	if ( (diff < -(PI/2)) || (diff > (PI/2)) )
		return FALSE;
	else
		return TRUE;
}

/********************************************************************************************

 >	void DocCoord::Scale(const DocCoord dcOrigin, const double dDPI=96.0)
										   
	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/4/97
	Inputs:		dDPI		The DPI by which to scale the coord
				dcOrigin	The origin by which to scale the coord
  
  Purpose:		Scales the DocCoord by the specified DPI and origin.

				That is, this function does the following to this DocCoord:

				a. Subtracts the origin dcOrigin
				b. Inverts the y-axis
				c. Multiplies the coordinate to convert them to the number
				   of DPI specified.

				Used in imagemaps.

				Note that, technically, what this function produces should be
				a WorkCoord. But that causes problems, because this
				function is called to scale paths to bitmap coordinates - and
				you can't specify a path in terms of WorkCoords.
			
SeeAlso:		Path::Scale(), Imagemap::AddPolygon()
	
********************************************************************************************/

void DocCoord::Scale(const DocCoord dcOrigin, const double dDPI)
{
	//Subtract the origin from this coord
	x-=dcOrigin.x;
	y-=dcOrigin.y;

	//And scale by the appropriate DPI, inverting the y-axis
	//and rounding to the nearest pixel
	x=INT32(x*(dDPI/72000.0)+0.5);
	y=INT32(-y*(dDPI/72000.0)+0.5);
	
}


