// $Id: tracergn.cpp 662 2006-03-14 21:31:49Z alex $
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
// This file implements region tracing

/*
*/

#include "camtypes.h"
#include "errors.h"
#include "tracergn.h"
#include "fixmem.h"
#include "nodepath.h"
#include "trans2d.h"

// for test
#include "app.h"
#include "range.h"
#include "node.h"
#include "nodebmp.h"
#include "bitmpinf.h"
#include "chapter.h"
#include "spread.h"
#include "page.h"
#include "bfxtest.h"
#include "bfxalu.h"
#include "bfxpixop.h"
#include "ccmaths.h"

// This is not compulsory, but you may as well put it in so that the correct version
// of your file can be registered in the .exe
DECLARE_SOURCE("$Revision: 662 $");

// An implement to match the Declare in the .h file.
// If you have many classes, it is recommended to place them all together, here at the start of the file
CC_IMPLEMENT_DYNCREATE(TraceRegion, CCObject)

// This will get Camelot to display the filename and linenumber of any memory allocations
// that are not released at program exit
#define new CAM_DEBUG_NEW

#define MAXTANGENTPOINTS	10

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

// Statics
/*
  	{-1, 1}	{ 0, 1}	{ 1, 1}
	{-1, 0}			{ 1, 0}
	{-1,-1}	{ 0,-1} { 1,-1}

*/

/********************************************************************************************

>	TraceRegion::TraceRegion()
					
	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/11/94
	Inputs:		none
	Outputs:	(constructs object)
	Returns:	none
	Purpose:	Sets intial values up
	Errors:		None
	Scope:		Public
	SeeAlso:	-

********************************************************************************************/

TraceRegion::TraceRegion()
{
	pBfxPixelOp = NULL;
	BoundaryBuffer = NULL;
	BoundaryRingSize = 0;
	BoundaryRingMask =0;
	Bitmap = NULL;
	xsize = 1;
	ysize = 1;
	VirginBuffer = TRUE;
	AtCusp = FALSE;
	TailCusp = FALSE;
	TailDirection = -1;
	ThePath = NULL;

	double TheError = 1.00;
	SetParams(&TheError);

/*
	In each direction of travel we always leave with a particular corner of the pixel set. In some directinos
	of travel, we have to set 2 edges. Here goes:

	 ->1-X
	|    |  This is how an east going trace is represented. The top edge is the one we need to fill in.
	|    |
	 ----

	 ->1- 
	|    V  This is how a SE going trace is represented. We have 2 edges to fill in this time.
	|    2
	 ----
	
	When there is a change in direction, things become more complex. Let us assume that the current
	direction is east, then note (for the following new directions) how will fill be buffer in:

	Current direction East									 Current direction south east:           
															                                         
	 ->1-X													  ->1-X			                        
	|    |  E												 |    |  E                               
	|    |													 |    |                                  
	 ----													  ----                                   
															                                         
	 ->1- 													  ->1-                                   
	|    V  SE												 |    V  SE                              
	|    2													 |    2                                  
	 ----X													  ----X                                  
															                                         
	 ->1-C 													  ->1-                                   
	|    V  S												 |    V  S                               
	|    2													 |    2                                  
	 ----X													  ----X                                  
															                                         
	 ->1-C													  ->1-                                   
	|    V  SW												 |    V  SW                              
	|    2													 |    2                                  
	X-<3-													 X-<3-C                                  
															                                         
	 ->1-C													  ->1-                                   
	|    V  W												 |    V  W                               
	|    2													 |    2                                  
	X-<3-(C)				 								 X-<3-C                                   
															                                         
	X---- 													  ->1-                                   
	|    |  NW	(theoretically impossible - no edges)		 4    V  NW                              
	|    |													 ^    2                                  
	 ----													  -<3-C                                   
															                                         
	C---- 													 X----                                   
	|    |  N	(no edges (correct place))					 |    |  N   (theoretically impossible)  
	|    |													 |    |                                  
	 ----													  ----                                   
															                                         
	 ->1-													  ->1-X                                  
	|    |  NE												 |    |  NE                              
	|    |													 |    |                                  
	 ----													  ----                                   


 Note how strikingly similar these are! We use the RH column one as that's generic for both sides.

*/
	



	// Grrr... This should be static but lovely C++ wont let me compile it. Thanks a lot C++
	// Heh what's 128 bytes when you have a bitmap to trace
	Directions[0].Init( (1<<8), (0<<8));	// East
	Directions[1].Init( (1<<8),-(1<<8));	// South East
	Directions[2].Init( (0<<8),-(1<<8));	// South
	Directions[3].Init(-(1<<8),-(1<<8));	// South West
	Directions[4].Init(-(1<<8), (0<<8));	// West
	Directions[5].Init(-(1<<8), (1<<8));	// North West
	Directions[6].Init( (0<<8), (1<<8));	// North
	Directions[7].Init( (1<<8), (1<<8));	// North East

	// First sort out the non-diagonal table
	DirectionPoint[0][0][0].Init( (1<<7), (1<<7));				// East to East
	DirectionPoint[0][0][1].Init( -1, -1);						// No point 2
	DirectionPoint[0][0][2].Init( -1, -1);						// No point 3
	DirectionPoint[0][0][3].Init( -1, -1);						// No point 4

	DirectionPoint[0][1][0].Init( (1<<7), (1<<7));				// East to SE
	DirectionPoint[0][1][1].Init( (1<<7),-(1<<7));				// point 2
	DirectionPoint[0][1][2].Init( -1, -1);						// No point 3
	DirectionPoint[0][1][3].Init( -1, -1);						// No point 4
	
	DirectionPoint[0][2][0].Init( (1<<7), (1<<7));				// East to S
	DirectionPoint[0][2][1].Init( -1, 2);						// No point 2, go south
	DirectionPoint[0][2][2].Init( -1, -1);						// No point 3
	DirectionPoint[0][2][3].Init( -1, -1);						// No point 4
	
	DirectionPoint[0][3][0].Init( (1<<7), (1<<7));				// East to SW
	DirectionPoint[0][3][1].Init( -1, 3);						// No point 2, go SW
	DirectionPoint[0][3][2].Init( -1, -1);						// point 3
	DirectionPoint[0][3][3].Init( -1, -1);						// No point 4

	DirectionPoint[0][4][0].Init( (1<<7), (1<<7));				// East to W
	DirectionPoint[0][4][1].Init( -1, 2);						// No point 2, go S
	DirectionPoint[0][4][2].Init( -1, -1);						// point 3
	DirectionPoint[0][4][3].Init( -1, -1);						// No point 4

	DirectionPoint[0][5][0].Init( (1<<7), (1<<7));				// South East to NW
	DirectionPoint[0][5][1].Init( (1<<7),-(1<<7));				// point 2
	DirectionPoint[0][5][2].Init( -1, 5);						// No point 3, go NW
	DirectionPoint[0][5][3].Init( -1, -1);						// No point 4

	DirectionPoint[0][6][0].Init( -1, 6);						// No point 1, go N
	DirectionPoint[0][6][1].Init( -1, -1);						// No point 2
	DirectionPoint[0][6][2].Init( -1, -1);						// No point 3
	DirectionPoint[0][6][3].Init( -1, -1);						// No point 4

	DirectionPoint[0][7][0].Init( (1<<7), (1<<7));				// East to NE
	DirectionPoint[0][7][1].Init( -1, -1);						// No point 2
	DirectionPoint[0][7][2].Init( -1, -1);						// No point 3
	DirectionPoint[0][7][3].Init( -1, -1);						// No point 4



	DirectionPoint[1][0][0].Init( (1<<7), (1<<7));				// South East to East
	DirectionPoint[1][0][1].Init( -1, -1);						// No point 2
	DirectionPoint[1][0][2].Init( -1, -1);						// No point 3
	DirectionPoint[1][0][3].Init( -1, -1);						// No point 4

	DirectionPoint[1][1][0].Init( (1<<7), (1<<7));				// South East to SE
	DirectionPoint[1][1][1].Init( (1<<7),-(1<<7));				// point 2
	DirectionPoint[1][1][2].Init( -1, -1);						// No point 3
	DirectionPoint[1][1][3].Init( -1, -1);						// No point 4
	
	DirectionPoint[1][2][0].Init( (1<<7), (1<<7));				// South East to S
	DirectionPoint[1][2][1].Init( (1<<7),-(1<<7));				// point 2
	DirectionPoint[1][2][2].Init( -1, -1);						// No point 3
	DirectionPoint[1][2][3].Init( -1, -1);						// No point 4
				    
	DirectionPoint[1][3][0].Init( (1<<7), (1<<7));				// South East to SW
	DirectionPoint[1][3][1].Init( (1<<7), (0<<7));				// point 3
	DirectionPoint[1][3][2].Init( -1, 3);						// No point 4, go SW
	DirectionPoint[1][3][3].Init( -1, -1);						// No point 4

	DirectionPoint[1][4][0].Init( (1<<7), (1<<7));				// South East to W
	DirectionPoint[1][4][1].Init( (1<<7),-(1<<7));				// point 2
	DirectionPoint[1][4][2].Init( -1, 4);          				// No point 3, go W
	DirectionPoint[1][4][3].Init( -1, -1);						// No point 4
	
	DirectionPoint[1][5][0].Init( (1<<7), (1<<7));				// South East to NW
	DirectionPoint[1][5][1].Init( (1<<7),-(1<<7));				// point 2
	DirectionPoint[1][5][2].Init( -1, 5);						// No point 3, go NW
	DirectionPoint[1][5][3].Init( -1, -1);						// No point 4

	DirectionPoint[1][6][0].Init( -1, 6);						// No point 1, go N
	DirectionPoint[1][6][1].Init( -1, -1);						// No point 2
	DirectionPoint[1][6][2].Init( -1, -1);						// No point 3
	DirectionPoint[1][6][3].Init( -1, -1);						// No point 4
			    
	DirectionPoint[1][7][0].Init( (0<<7), (1<<7));				// South East to NE
	DirectionPoint[1][7][1].Init( -1, 7);						// No point 2, go NE
	DirectionPoint[1][7][2].Init( -1, -1);						// No point 3
	DirectionPoint[1][7][3].Init( -1, -1);						// No point 4
	
	INT32 i;
	INT32 j;
	INT32 k;
	for (i=2; i<=7; i+=1) for (j=0; j<=7; j++) for (k=0; k<=3; k++)
	{
		// Go from direction i to direction j, i even
		INT32 j2 = (j-(i & 0x6)) & TR_NUMDIRECTIONMASK;
		DirectionPoint[i][j][k]=DirectionPoint[i & 0x1][j2][k];
		
		if (DirectionPoint[i][j][k].x != -1)
		{
			DirectionPoint[i][j][k].RotateDirection(Directions[(i & 0x6)]);
		}
		else
		{
			if (DirectionPoint[i][j][k].y != -1)
				DirectionPoint[i][j][k].y = (((INT32)(DirectionPoint[(i & 0x1)][j2][k].y) + (i & 0x6)) & TR_NUMDIRECTIONMASK);
		}
	}
	
	TraceBoundaryPoint Centre;
	Centre.Init((1<<7),(1<<7));
	for (i=0; i<=7; i+=1) for (j=0; j<=7; j++) for (k=0; k<=3; k++)
	{
		if (DirectionPoint[i][j][k].x != -1)
		{
			DirectionPoint[i][j][k].translate(Centre);
		}
	}
	
}


/********************************************************************************************

>	BOOL TraceRegion::SetParams(double * pPixelError)

	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/1/95
	Inputs:		^double values of parameters to set. May be set to NULL to not change them
	Outputs:	None
	Returns:	TRUE on success, FALSE (& error set) on failure
	Purpose:	Sets the tracing parameters
	Errors:		mainly memory.
	Scope:		Public
	SeeAlso:	-

********************************************************************************************/

BOOL TraceRegion::SetParams(double * pPixelError)
{
	if (pPixelError) PixelError = *pPixelError;
	Error = PixelError;
	Error *= 256.0; // For shift
	Error *= Error; // Squared

	return TRUE;
}

/********************************************************************************************

>	BOOL TraceRegion::GetParams(double * pPixelError)

	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/1/95
	Inputs:		None
	Outputs:	^double values of parameters to read May be set to NULL to not read them
	Returns:	TRUE on success, FALSE (& error set) on failure
	Purpose:	Sets the tracing parameters
	Errors:		mainly memory.
	Scope:		Public
	SeeAlso:	-

********************************************************************************************/

BOOL TraceRegion::GetParams(double * pPixelError)
{
	if (pPixelError) *pPixelError = PixelError;
	return TRUE;
}

/********************************************************************************************

>	TraceBoundaryPoint::RotateDirection(const TraceBoundaryPoint & rotation)
					
	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/11/94
	Inputs:		rotation = amount to rotate by
	Outputs:	point is rotated
	Returns:	none
	Purpose:	urmm.. rotation of trace boundary points to conform to a particular direction
	Errors:		None
	Scope:		Public
	SeeAlso:	-

This rotates an trace boundary point which is assumed to have an eastern direction to that
specified

********************************************************************************************/

void TraceBoundaryPoint::RotateDirection(const TraceBoundaryPoint & rotation)
{


	TraceBoundaryPoint temp;
	temp = *this;
//
// Parallelogram is (r.x, r.y) & (-r.y, r.x)
//
// (  r.x -r.y ) ( t.x )
// (  r.y  r.x ) ( t.y )

	x = (temp.x * ((INT32)(rotation.x)>>8)) - (temp.y * ((INT32)(rotation.y)>>8));
	y = (temp.x * ((INT32)(rotation.y)>>8)) + (temp.y * ((INT32)(rotation.x)>>8));
}

/********************************************************************************************

>	TraceBoundaryPoint TraceBoundaryPoint::SetLength( double NewLen )

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/03/94
	Inputs:		NewLen - The length you want the vector to be
	Returns:	TraceBoundaryPoint - the new vector that points in the same direction as this vector,
				only of magnitude NewLen
	Purpose:	Scales the TraceBoundaryPoint vector to by the specified length

********************************************************************************************/

TraceBoundaryPoint TraceBoundaryPoint::SetLength( double NewLen )
{
	TraceBoundaryPoint Result(x, y);

	double Len = Length();
	if (Len != 0.0)
	{
		Result.x *= (NewLen / Len);
		Result.y *= (NewLen / Len);
	}

	return Result;
}

/********************************************************************************************

>	TraceRegion::~TraceRegion()
					
	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/11/94
	Inputs:		none
	Outputs:	(destructs object)
	Returns:	none
	Purpose:	Destructs object
	Errors:		None
	Scope:		Public
	SeeAlso:	-

This frees the boundary buffer it is is present.

********************************************************************************************/

TraceRegion::~TraceRegion()
{
	if (BoundaryBuffer) CCFree(BoundaryBuffer);
	// Clear some hanging pointers
	BoundaryBuffer = NULL;
	Bitmap = NULL;
	ThePath = NULL;
}

/********************************************************************************************

>	BOOL TraceRegion::UseBitmap(KernelBitmap * pBitmap)
					
	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/11/94
	Inputs:		pBitmap = pointer to bitmap object to use or NULL for none
	Outputs:	(sets object up)
	Returns:	TRUE if succeeded else FALSE and error set
	Purpose:	Points the TraceRegion at a particular bitmap and claims the relevant size
				buffers.
	Errors:		None
	Scope:		Public
	SeeAlso:	-

The TraceRegion is set up with a new boundary buffer of an appropriate size. If NULL is passed
in the buffers are removed.

Note (surprisingly, perhaps) we don't actually ever refer to pixels within this bitmap. Only
its size is used. The BfxPixelOp does all the poking around and it may be referencing a
different (but same sized) bitmap elsewhere.

********************************************************************************************/

BOOL TraceRegion::UseBitmap(KernelBitmap * pBitmap)
{
	if (BoundaryBuffer)
	{
		CCFree(BoundaryBuffer);
		BoundaryBuffer = NULL;
	}
                                                           
	xsize=1;
	ysize=1;
	Bitmap = NULL;
	
	if (pBitmap)
	{
		INT32 MaxDimension;
		BitmapInfo BMInfo;
		pBitmap->ActualBitmap->GetInfo(&BMInfo);
		
		// Max dimension is 8 times the largest of the width and height (enough to fit a bezier
		// curve of maximal usefulness into half the buffer)
		MaxDimension = ((BMInfo.PixelWidth>BMInfo.PixelHeight)?BMInfo.PixelWidth:BMInfo.PixelHeight)*8;
		ERROR2IF(MaxDimension<=0, FALSE, "Bitmap info structure is dead");
		BoundaryRingSize = 1;
		
		if (MaxDimension<80) MaxDimension=80;

		// Get next highest power of 2 & mask
		while (BoundaryRingSize < MaxDimension) BoundaryRingSize = BoundaryRingSize<<1;
		BoundaryRingMask = BoundaryRingSize -1;
		
		// Allocate memory (CCMalloc sets error)
		if (NULL== (BoundaryBuffer=(TraceBoundaryPoint *)CCMalloc(sizeof(TraceBoundaryPoint) * BoundaryRingSize)) )
			return FALSE;
//		// fill the memory up with +S-NAN
//		for (INT32 i=0; i<BoundaryRingSize; i++)
//		{
//			INT32* p = (INT32*)&(BoundaryBuffer[i].x);
//			*p++ = 0xFFFFFFFF;
//			*p++ = 0x7FF7FFFF;
//			p = (INT32*)&(BoundaryBuffer[i].y);
//			*p++ = 0xFFFFFFFF;
//			*p++ = 0x7FF7FFFF;
//		}

		xsize = BMInfo.PixelWidth;
		ysize = BMInfo.PixelHeight;
		Bitmap=pBitmap;
		VirginBuffer = TRUE;
		TailDirection = -1;
		BoundaryHead =0;
		BoundaryTail =0;
		TailDirection = 0;
		HeadDirection = 0;
		InitialDirection = 0;
		AtCusp = FALSE;
		TailCusp = FALSE;
	}
	return (TRUE);
}

/********************************************************************************************

>	BOOL TraceRegion::UsePath(Path * pPath)
					
	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/11/94
	Inputs:		pPath = pointer to Path object to use or NULL for none
	Outputs:	(sets object up)
	Returns:	TRUE if succeeded else FALSE and error set
	Purpose:	Specifies we should use a particular path. The path is initialised, cleared etc.
	Errors:		None
	Scope:		Public
	SeeAlso:	-


********************************************************************************************/

BOOL TraceRegion::UsePath(Path * pPath)
{
	ThePath = NULL;
	VirginPath = TRUE;
	
	if (pPath)
	{
//		if (!pPath->Initialise()) return FALSE;
		if (!pPath->ClearPath()) return FALSE;
		ThePath = pPath;
	}
	return (TRUE);
}

/********************************************************************************************

>	BOOL TraceRegion::FindInitialPoint(BOOL * End)
					
	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/11/94
	Inputs:		None
	Outputs:	End set if no points
	Returns:	TRUE if succeeded else FALSE and error set
	Purpose:	Finds an initial point to start at.
	Errors:		Various
	Scope:		Protected
	SeeAlso:	-

This routine is currently a horrible bodge creature.

The boundary buffer is cleared. An initial point will be found if possible and End cleared. If
one is not found, End will be set.

********************************************************************************************/

BOOL TraceRegion::FindInitialPoint(BOOL * End)
{
	INT32 x;
	INT32 y;

	ERROR2IF((!Bitmap),FALSE,"How about giving us a bitmap first then?");

	InitialDirection = 0; // East
	VirginBuffer = TRUE;
	BoundaryHead =0;
	BoundaryTail =0;
	TailDirection = -1;
	HeadDirection = 0;
	AtCusp=FALSE;
	TailCusp = FALSE;
	*End = FALSE;

	for (y = ysize-1; y>=0; y--) for (x = 0; x < xsize; x++) if (pBfxPixelOp->IsInRegion(x,y))
	{
		InitialPoint.x = x<<8;
		InitialPoint.y = y<<8;
		HeadPoint = InitialPoint;
		return (TRUE);
	}

	*End = TRUE;	// Oh dear, no initial point
		
	return(TRUE);
}

/********************************************************************************************

>	BOOL TraceRegion::TraceBoundary(DocCoord Origin,DocCoord Point1,DocCoord Point2)
					
	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/11/94
	Inputs:		3 points describing parallelogram

	Outputs:	None
	Returns:	TRUE if succeeded else FALSE and error set
	Purpose:	Does a single boundary trace
	Errors:		Various
	Scope:		Protected
	SeeAlso:	-

This function is now defunct and is used to test porpoises only

********************************************************************************************/

BOOL TraceRegion::TraceBoundary(DocCoord Origin,DocCoord Point1,DocCoord Point2)
{
	BOOL Done;
	ERROR2IF((!Bitmap),FALSE,"How about giving us a bitmap first then?");

	// First find the initial point
	if (!(FindInitialPoint(&Done))) return (FALSE);
	if (Done) return (TRUE);

	do
	{
		if (!FillBoundaryBuffer(&Done)) return (FALSE);
		if (!ProcessBoundaryBuffer(Done)) return (FALSE);
	} while (!Done);

	ThePath->CloseSubPath();
	ThePath->IsFilled = TRUE; // What lovely encapsulation & abstraction...

	Matrix tMatrix(Div32By32(Point1.x-Origin.x,xsize<<8),Div32By32(Point2.x-Origin.x,xsize<<8),
				   Div32By32(Point1.y-Origin.y,ysize<<8),Div32By32(Point2.y-Origin.y,ysize<<8),
				   Origin.x,Origin.y);
	Trans2DMatrix Trans(tMatrix);
	Trans.Transform( (DocCoord*)ThePath->GetCoordArray(), ThePath->GetNumCoords() );

	return (TRUE);
}

/********************************************************************************************

>	BOOL TraceRegion::Trace(INT32 InitialX, INT32 InitialY, BfxPixelOp * thepBfxPixelOp)

					
	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/11/94
	Inputs:		Initial coordinates and pixel contour to trace
	Outputs:	None
	Returns:	TRUE if succeeded else FALSE and error set
	Purpose:	Does a single boundary trace
	Errors:		Various
	Scope:		Protected
	SeeAlso:	-

********************************************************************************************/

BOOL TraceRegion::Trace(INT32 InitialX, INT32 InitialY, BfxPixelOp * thepBfxPixelOp)
{
	BOOL Done=FALSE;
	ERROR2IF((!Bitmap),FALSE,"How about giving us a bitmap first then?");

	pBfxPixelOp = thepBfxPixelOp;
	
	ERROR2IF( ((!pBfxPixelOp->IsInRegion(InitialX,InitialY))), FALSE, "Initial point not in region" )

	BOOL Bottom=FALSE;
	while (!Bottom)
	{
		Bottom=TRUE;
		if (pBfxPixelOp->IsInRegion(InitialX-1,InitialY-1))
		{
			InitialY--;
			InitialX--;
			Bottom=FALSE;
		}
		if (pBfxPixelOp->IsInRegion(InitialX,InitialY-1))
		{
			InitialY--;
			Bottom=FALSE;
		}
		if (pBfxPixelOp->IsInRegion(InitialX-1,InitialY))
		{
			InitialX--;
			Bottom=FALSE;
		}
	}

	InitialDirection = 4; // West
	VirginBuffer = TRUE;
	BoundaryHead =0;
	BoundaryTail =0;
	TailDirection = -1;
	HeadDirection = 4;
	AtCusp=FALSE;
	TailCusp = FALSE;

	InitialPoint.x = InitialX<<8;
	InitialPoint.y = InitialY<<8;
	HeadPoint = FirstBufferPoint = InitialPoint;

	do
	{
		if (!FillBoundaryBuffer(&Done)) return (FALSE);
		if (!VirginBuffer) { if (!ProcessBoundaryBuffer(Done)) return (FALSE); } else BoundaryHead=0;
	} while (!Done);

	ERROR2IF(VirginPath, FALSE, "We've just traced a path that wasn't a path. Very odd");

	ThePath->CloseSubPath();
	ThePath->IsFilled = TRUE; // What lovely encapsulation & abstraction...

	return (TRUE);
}


/********************************************************************************************

>	BOOL TraceRegion::FillBoundaryBuffer(BOOL * End)
					
	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/11/94
	Inputs:		End ptr to BOOL to return in
	Outputs:	End set TRUE if last point else set FALSE
	Returns:	TRUE if worked, FALSE & error set if failed
	Purpose:	Fills up the boundary buffer
	Errors:		Error2 & Error3 (various)
	Scope:		Protected
	SeeAlso:	TraceRegion::ProcessBoundaryBuffer

This routine fills the boundary buffer up with some more points. It stops if the buffer
becomes nearly full.

It also stops if it meets a cusp, or the intial point in the initial direction.

********************************************************************************************/


BOOL TraceRegion::FillBoundaryBuffer(BOOL * End)
{
	INT32 NewDirection;
	INT32 Turns;

	ERROR2IF((!Bitmap),FALSE,"TraceRegion incorrectly initialised");

	*End=FALSE;	
	while (BoundaryHead<BoundaryRingSize - 6)	// there is enough room in buffer
	{
	 	// Calculate the new direction. We start looking at direction-2 but as we add one before we start
		// we have -3. If we're at a cusp we know the NewDirection set to HeadDirection is the correct place,
		// i.e. we want +0, but because we add one before we start, we want -1.
		NewDirection = (HeadDirection-(AtCusp?1:3)) & TR_NUMDIRECTIONMASK;
		//NewDirection = (HeadDirection-3) & TR_NUMDIRECTIONMASK;
		Turns=0;
		TraceBoundaryPoint NewPoint;
		
		
		do
		{
			// This should only return(TRUE) if asked to trace a
			// one pixel shape - else we must have got to this pixel some how.
			if ((Turns++)==8)
			{
				*End=TRUE;
				// We want to force it to be the 4 corner case
				NewDirection=((HeadDirection & 0x6) + 6) & TR_NUMDIRECTIONMASK;
				ERROR3("One pixel regions NYI");
				// The above won't work properly because it's not properly entered. I think we need a horrible
				// goto here.
				break;
			}
			// Find next direction
			NewDirection = (NewDirection + 1) & TR_NUMDIRECTIONMASK;			
			NewPoint = HeadPoint;		// Calculate the new point
			NewPoint.translate(Directions[NewDirection]);
		} while (!pBfxPixelOp->IsInRegion(((INT32)(NewPoint.x))>>8, ((INT32)(NewPoint.y))>>8)); 

		if (VirginBuffer) InitialDirection = TailDirection = NewDirection;


		// stick point in ring buffer
		// Limit is such that we know we can fit the maximum of 4 points in
		INT32 HintPoint=0;
		if (!VirginBuffer)
		{
			if (AtCusp)
			{
				// Only put the last point in if at a cusp
				while ((DirectionPoint[HeadDirection][NewDirection][HintPoint].x != -1) // sufficient check
					  && (HintPoint<4)) {HintPoint++;}	// Don't inc at final point
				BoundaryBuffer[BoundaryHead] = HeadPoint;
				BoundaryBuffer[BoundaryHead++].translate(DirectionPoint[HeadDirection][NewDirection][HintPoint-1]);
				// HintPoint still points at terminator
			}
			else
			{
				// Put all points in
				while ((DirectionPoint[HeadDirection][NewDirection][HintPoint].x != -1) // sufficient check
					  && (HintPoint<4))
				{
					BoundaryBuffer[BoundaryHead] = HeadPoint;
					BoundaryBuffer[BoundaryHead++].translate(DirectionPoint[HeadDirection][NewDirection][HintPoint++]);
				}
			}
		} else while ((DirectionPoint[HeadDirection][NewDirection][HintPoint].x != -1) // sufficient check
					  && (HintPoint<4)) {HintPoint++;}	// Don't inc at final point


		if (DirectionPoint[HeadDirection][NewDirection][HintPoint].y !=-1)
		{
			HeadDirection = (INT32)(DirectionPoint[HeadDirection][NewDirection][HintPoint].y);
//			HeadPoint = NewPoint;
			AtCusp = TRUE;
			if (TailDirection == -1) TailDirection = HeadDirection;
//			VirginBuffer = FALSE;
			return (TRUE);
		}

		// check for cusp a with dirty unsigned arithmetic and return NOW if we are there.
		// AtCusp =  ( ( ((unsigned)(Turns-2)) >=3) && (!AtCusp));
		AtCusp = FALSE;

		//if (AtCusp) return (TRUE); // cusp return	
	

	 	if ( (HeadPoint == InitialPoint) && (NewDirection == InitialDirection) && !VirginBuffer )
		{
			*End = TRUE;
			VirginBuffer = FALSE;
			return (TRUE);
		}

		VirginBuffer = FALSE;
 
		if (TailDirection == -1) TailDirection = NewDirection;

		// Set up member vars
		HeadPoint = NewPoint;
		HeadDirection = NewDirection;

	}

	return(TRUE); // Out of buffer space, go process some & come back
}


/********************************************************************************************

>	BOOL TraceRegion::ProcessBoundaryBuffer(BOOL Done = FALSE)
					
	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/11/94
	Inputs:		Done TRUE to set last point to FirstBufferPoint
	Outputs:	none
	Returns:	TRUE if worked, FALSE & error set if failed
	Purpose:	Empties the boundary buffer
	Errors:		Error2 & Error3 (various)
	Scope:		Protected
	SeeAlso:	TraceRegion::ProcessBoundaryBuffer

This routine empties the boundary buffer. It leaves exactly one coordinate in it. The buffer
must be non-empty.

********************************************************************************************/

BOOL TraceRegion::ProcessBoundaryBuffer(BOOL Done)
{
	BoundaryTail = 0;
//	ERROR3IF( (((BoundaryHead - BoundaryTail) & BoundaryRingMask) <=0 ),
//			"Boundary ring buffer is empty");
	if (((BoundaryHead - BoundaryTail) & BoundaryRingMask) <=0 ) return TRUE;

	if (VirginPath)
	{
		ThePath->FindStartOfPath();
		ThePath->InsertMoveTo(DocCoord((INT32)(BoundaryBuffer[BoundaryTail].x), (INT32)(BoundaryBuffer[BoundaryTail].y)));
		VirginPath = FALSE;
		FirstBufferPoint=BoundaryBuffer[BoundaryTail];
	}

	// Patch up initial hinting (done on inadequate information)
	if (Done)
	{
		BoundaryBuffer[BoundaryHead-1]=FirstBufferPoint;
		if (BoundaryBuffer[BoundaryHead-2]==FirstBufferPoint) BoundaryHead=(BoundaryHead-1)&BoundaryRingMask;
		if (((BoundaryHead - BoundaryTail) & BoundaryRingMask) <=0 ) return TRUE;
	}

#if 0	
	while (BoundaryTail != (BoundaryHead-1))
	{
		InsertLine(DocCoord((INT32)(BoundaryBuffer[BoundaryTail].x),(INT32)(BoundaryBuffer[BoundaryTail].y)),
				   DocCoord((INT32)(BoundaryBuffer[BoundaryTail=(BoundaryTail + 1) & BoundaryRingMask].x),
				   	        (INT32)(BoundaryBuffer[BoundaryTail].y)), TRUE, TRUE);
	}
#else
	// This case should never occur, but does very very occasionally - not sure why (Alex)
	// we ignore buffers with only one point in
	if (((BoundaryHead-1) & BoundaryRingMask)==BoundaryTail) return TRUE;

	// Just have to fit a curve from the last cusp to the end of the path
	TraceBoundaryPoint Tangent1 = LeftTangent(BoundaryTail,BoundaryHead-BoundaryTail);
	TraceBoundaryPoint Tangent2 = RightTangent(BoundaryHead-1,BoundaryHead-BoundaryTail);
    
    // and do a load of maths that will hopefully fit a nice curve on it
	FitCubic(BoundaryTail, BoundaryHead-1, Tangent1, Tangent2, TailCusp, AtCusp);
#endif
	
	BoundaryBuffer[0]=BoundaryBuffer[BoundaryHead-1];
	BoundaryTail = 0;
	BoundaryHead = 1;

	TailDirection = -1;
	TailCusp = AtCusp;
	return (TRUE);
}


/********************************************************************************************

>	void TraceRegion::FitCubic( INT32 FirstPoint, INT32 LastPoint,
								   TraceBoundaryPoint Tangent1, TraceBoundaryPoint Tangent2,
								   BOOL IsStartCusp = TRUE, BOOL IsEndCusp = TRUE);

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com> & Alex
	Created:	7/3/94
	Inputs:		FirstPoint - the index of the coordinate in the path to start fitting from
				LastPoint - the index of the coordinate in the path to stop fitting at
				Tangent1 - The tangent of the curve at the first point
				Tangent2 - the tangent of the curve at the last point
				IsStartCusp - TRUE if FirstPoint is next to a cusp
				IsEndCusp - TRUE of EndPoint is next to a cusp
	Purpose:	This function is recursive. It tries to fit a cubic function to the
				coordinates in the path between FirstPoint and LastPoint. It then compares
				this function with the actual coordinates to determine how good a fit it
				has found. If the fit is good it is added to the path object. If the fit
				is bad then it is split at the point where the fit is at its worst and
				FitCubic is called again for the left and right halves.
	Scope:		private
	SeeAlso:	TraceRegion::GenerateBezier; TraceRegion::CalcMaxError

********************************************************************************************/

void TraceRegion::FitCubic(INT32 FirstPoint, INT32 LastPoint, TraceBoundaryPoint Tangent1, TraceBoundaryPoint Tangent2,
							  BOOL IsStartCusp, BOOL IsEndCusp)
{
	// Will need space for a Bezier curve
	TraceBoundaryPoint Bezier[4];
	TraceBoundaryPoint CentTangent;
	INT32 NumPoints = LastPoint - FirstPoint + 1;
	INT32 SplitPoint, Split;


	// If we have the flexibility, recalculate the tangents to the left and right
	if (IsStartCusp) Tangent1 = LeftTangent(FirstPoint, NumPoints);
	if (IsEndCusp) Tangent2 = RightTangent(LastPoint, NumPoints);

// The following is actually legal if we start on a cusp
//	ERROR3IF(NumPoints<2,"Too few points to fit a cubic");
	if (NumPoints<2) return;
#if 0
	// if this segment only has 2 points in it then do the special case
	if ( NumPoints == 2 )
	{
		InsertLine( DocCoord((INT32)(BoundaryBuffer[FirstPoint].x), (INT32)(BoundaryBuffer[FirstPoint].y)),
					DocCoord((INT32)(BoundaryBuffer[LastPoint].x), (INT32)(BoundaryBuffer[FirstPoint].y)),
					IsStartCusp, IsEndCusp);
		return;
	}
#endif
	/* Due to a bug in the algorithm we also have to consider 3 points a special case.

	Here's what Mathematica reckons is the formula for a bezier A B C D that passes through a point Q
	with tangents tx & ty

	              4 t1x (Ay t2x + Dy t2x - 2 Qy t2x - Ax t2y - Dx t2y + 2 Qx t2y)
	 {{Bx -> Ax - ---------------------------------------------------------------, 
	                                   3 (t1y t2x - t1x t2y)
	              4 t1y (Ay t2x + Dy t2x - 2 Qy t2x - Ax t2y - Dx t2y + 2 Qx t2y)
	   By -> Ay - ---------------------------------------------------------------, 
	                                   3 (t1y t2x - t1x t2y)
	              (12 (Ay + Dy - 2 Qy) t1x - 12 (Ax + Dx - 2 Qx) t1y) t2x
	   Cx -> Dx - -------------------------------------------------------, 
	                             9 (-(t1y t2x) + t1x t2y)
	              4 (Ay t1x + Dy t1x - 2 Qy t1x - Ax t1y - Dx t1y + 2 Qx t1y) t2y
	   Cy -> Dy + ---------------------------------------------------------------, 
	                                   3 (t1y t2x - t1x t2y)

	I reckon I can rewrite this better:


	   F  -> (4/3) (A + D - 2 Q) / (t2x t1y - t2y t1x)

	   B  -> Fy t2x - Fx t2y
         
	   C  -> Fx t1y - Fy t1x
              
	   Bx -> Ax - B t1x
   
	   By -> Ay - B t1y

	   Cx -> Dx - C t2x
              
	   Cy -> Dy - C t2y

	This is all very well and good, but when does it occur? Here's an example:


	A	  B
	 ....
		 ....
			 ...T1

	T2....C

  
	Here A is is the FistPoint with tangent T1, and C is the last point with tangent T2. The only bezier that
	fits through B as well with tangents specified is a loop that goes left from C, back under it and up through T1.
	Though all the maths is fine, and we can even detect this case, it's just not worth it. We just miss out point B.

	Oh well, so much for the beaury of mathematics. (PS - the real soln is to introduce 1 or more extra points. But that's
	getting silly).

	*/
	
	#if 0
	// Never appears to run to completion anyway :-(
	if (NumPoints == 3)
	{
		INT32 MidPoint = FirstPoint+1;
		double Bottom = 0.75 * (Tangent2.x * Tangent1.y - Tangent2.y * Tangent1.x);
		if (Abs(Bottom)>0.001) // Else ignore middle point - this happens if the tangents are nearly parallel
		{
			TraceBoundaryPoint F = (BoundaryBuffer[FirstPoint]+BoundaryBuffer[LastPoint]-BoundaryBuffer[MidPoint]*2.0)/Bottom;
			double B = F.y * Tangent2.x - F.x * Tangent2.y;
			double C = F.x * Tangent1.y - F.y * Tangent1.x;
			if ((B<0.0) && (C<0.0)) // Not generating a loop
			{
				Tangent1 = -(Tangent1 * B);
				Tangent2 = -(Tangent2 * C); // Don't worry - we just need their directions for the fallback case
				// Max distance is ((1<<8)/2)^2 = (1<<14)
				if ((Tangent1.SquaredLength()<(1<<14)) && (Tangent2.SquaredLength()<(1<<14)))
				{
					Bezier[0] = BoundaryBuffer[FirstPoint];
					Bezier[3] = BoundaryBuffer[LastPoint];
					Bezier[1] = BoundaryBuffer[FirstPoint] + Tangent1;
					Bezier[2] = BoundaryBuffer[LastPoint] + Tangent2;
					InsertBezier(Bezier, IsStartCusp, IsEndCusp);
					return;
				}
			}
		}	
	}
	#endif

	// We must consider 2 points (or unhandled 3 point cases) as a special case
	if ( NumPoints <=2)
	{
		// With 3 points the distance is always 2.
		INT32 Distance = (1<<8)*2/3;
		
		// store the end points
		Bezier[0] = BoundaryBuffer[FirstPoint];
		Bezier[3] = BoundaryBuffer[LastPoint];
		
		// calc the control points
		Bezier[1] = Bezier[0] + Tangent1.SetLength(Distance);
		Bezier[2] = Bezier[3] + Tangent2.SetLength(Distance);

		// add it to the path
		InsertBezier(Bezier, IsStartCusp, IsEndCusp);
		return;
	}

	
	if (NumPoints ==3)
	{
		Split = FirstPoint + 1; // Midpoint	
	}
	else
	{
		// Create a Bezier curve from the segemnt and see if it is a good fit
		Parameterize(FirstPoint, LastPoint);

		GenerateBezier(FirstPoint, LastPoint, Tangent1, Tangent2, Bezier);
	
		SplitPoint=NumTPoints-1;
		double MaxError = 0;	
		if (!CalcMaxError(0, NumTPoints, Bezier, &SplitPoint, &MaxError, 0))
		{
			// The mapping was good, so output the curve segment
			//TRACEUSER( "Alex", _T("  OK FP=%d, LP=%d, ME=%f\n"),FirstPoint,LastPoint,MaxError);
			InsertBezier(Bezier, IsStartCusp, IsEndCusp);
			return;
		}

		Split = TPoints[SplitPoint];
	}	
	
	//TRACEUSER( "Alex", _T("Split FP=%d, SP=%d, LP=%d\n"),FirstPoint,Split,LastPoint);
	// fitting failed -- split at max error point and fit recursively
	CentTangent = CentreTangent(Split, MIN(Split-FirstPoint+1,LastPoint-Split+1));
	//Tangent1=LeftTangent(FirstPoint,SplitPoint-FirstPoint+1);
	FitCubic(FirstPoint, Split, Tangent1, CentTangent, IsStartCusp, FALSE);

	CentTangent = -CentTangent;
	//Tangent2=RightTangent(LastPoint,LastPoint-SplitPoint+1);
	FitCubic(Split, LastPoint, CentTangent, Tangent2, FALSE, IsEndCusp);
	
}



/********************************************************************************************

>	void TraceRegion::Parameterize(INT32 FirstPoint, INT32 LastPoint)

	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/94
	Inputs:		FirstPoint - the index of the coordinate in the path to start fitting from
				LastPoint - the index of the coordinate in the path to stop fitting at
	Outputs:	Fills up TValues & TPoints array
	Purpose:	This function determines the TPoints which will be fitted (i.e. the trace
				boundary points we use) and the TValues which will be assigned to them
				(i.e. the t values along the chord length to which the points correspond).
	Scope:		private

********************************************************************************************/

void TraceRegion::Parameterize(INT32 FirstPoint, INT32 LastPoint)
{
	INT32 pos, i;
	
	INT32 NumPoints = LastPoint-FirstPoint;
	// Chord length parameterisation

	NumTPoints = MIN(NumPoints, FIT_STEPS);

	for (pos=0; pos<=NumTPoints; pos++)
	{
		i = (INT32)(((double)pos/(double)NumTPoints*(double)(NumPoints))+0.5);
		// In this loop, 
		TValues[pos] = ((double)i)/((double)NumPoints);
		TPoints[pos] = (i+FirstPoint);
	}
	
	// These should algorithmically never happen
	ERROR3IF(TPoints[0]!=FirstPoint,"TraceRegion::Parameterize fouled up first point");
	ERROR3IF(TPoints[NumTPoints]!=LastPoint,"TraceRegion::Parameterize fouled up last point");
	return;
}

/********************************************************************************************

>	void TraceRegion::GenerateBezier(INT32 FirstPoint, INT32 LastPoint, TraceBoundaryPoint Tangent1, 
										TraceBoundaryPoint Tangent2, TraceBoundaryPoint* Bezier)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com> & Alex
	Created:	7/3/94
	Inputs:		FirstPoint - the index of the coordinate in the path to start fitting from
				LastPoint - the index of the coordinate in the path to stop fitting at
				Tangent1 - The tangent of the curve at the first point
				Tangent2 - the tangent of the curve at the last point
	Outputs:	Bezier - A pointer to a Bezier Curve
	Purpose:	This function supplies the maths to try and fit a cubic function to a 
				set of points. It spends its time trying to come up with good lengths for
				the two tangents passed in to maximise the closeness of the fit. If it
				fails to come up with realistic results it simply sets the tangent lengths
				to be 1/3 of the distance between the start point and the end point.
	Scope:		private

Note the differential & 2nd differential are also generated

********************************************************************************************/

void TraceRegion::GenerateBezier(INT32 FirstPoint, INT32 LastPoint, TraceBoundaryPoint Tangent1, 
									TraceBoundaryPoint Tangent2, TraceBoundaryPoint* Bezier)
{
	INT32 pos;
	INT32 i;
	INT32 NumPoints = LastPoint - FirstPoint + 1;
	
	// Build a matrix type of thing that contains the tangents scaled by the offsets
	TraceBoundaryPoint A[FIT_STEPS+1][2];			//	Vector2 (*A)[2] = new Vector2[NumPoints+1][2];
	
	// Chord length parameterisation
	for (pos=0; pos<=NumTPoints; pos++)
	{
		// Fill the matrix A
		A[pos][0] = Tangent1.SetLength( Bezier1(TValues[pos]) );
		A[pos][1] = Tangent2.SetLength( Bezier2(TValues[pos]) );
	}

	// For a detailed description of the maths used here, please see Graphics Gems I
	// I have made some changes to the basic algorithm used there - the main one is
	// that this block of maths is only performed on a small selection of the points
	// in the data set, where-as the one in the book uses all the points
	double  C[2][2];
	double  X[2];
	
	C[0][0] = 0.0;
	C[0][1] = 0.0;
	C[1][0] = 0.0;
	C[1][1] = 0.0;
	X[0]    = 0.0;
	X[1]    = 0.0;
	
	TraceBoundaryPoint FirstCoord = BoundaryBuffer[FirstPoint];
	TraceBoundaryPoint LastCoord  = BoundaryBuffer[LastPoint];
	TraceBoundaryPoint ThisCoord, Combo;

	for (pos=0; pos<=NumTPoints; pos++)
	{
		C[0][0] += A[pos][0].SquaredLength();
		C[0][1] += A[pos][0].Dot(A[pos][1]);
		// Point C[1][0] is the same as C[0][1] and is set outside the loop below
		C[1][1] += A[pos][1].SquaredLength();
		
		// Go ahead and build a vector based on the bezier functions
		ThisCoord = BoundaryBuffer[TPoints[pos]];
		Combo = ThisCoord - ((FirstCoord * Bezier0(TValues[pos]))
							+ (FirstCoord * Bezier1(TValues[pos]))
							+ (LastCoord  * Bezier2(TValues[pos]))
							+ (LastCoord  * Bezier3(TValues[pos])));

		// Combine it with the other points
		X[0] += A[pos][0].Dot( Combo );
		X[1] += A[pos][1].Dot( Combo );

	}

	// This point in the matrix is the same, so we do not need to do it in the loop
	C[1][0] = C[0][1];
	
	// calc the determinants of C and X
	double det_C0_C1 = C[0][0] * C[1][1] - C[1][0] * C[0][1];
	double det_C0_X  = C[0][0] * X[1]    - C[0][1] * X[0];
	double det_X_C1  = X[0]    * C[1][1] - X[1]    * C[0][1];
	
	// finally, derive the length of the ideal tangents
	if (det_C0_C1 == 0.0)
		det_C0_C1 = (C[0][0] * C[1][1]) * 10e-12;	// oh err, whats it up to here then!
	
	double AlphaLeft  = det_X_C1 / det_C0_C1;
	double AlphaRight = det_C0_X / det_C0_C1;
	
	Bezier[0] = BoundaryBuffer[FirstPoint];
	Bezier[3] = BoundaryBuffer[LastPoint];

	// if alpha negative, the set the tangent length to a third of the distance between
	// the start and the end points of the curve segment	
	if ( AlphaLeft < 0.0 || AlphaRight < 0.0)
	{
		INT32 Distance = ((LastPoint - FirstPoint)<<8) / 3;
		
		Bezier[1] = Bezier[0] + Tangent1.SetLength(Distance);
		Bezier[2] = Bezier[3] + Tangent2.SetLength(Distance);
	}
	else
	{	
		Bezier[1] = Bezier[0] + Tangent1.SetLength(AlphaLeft);
		Bezier[2] = Bezier[3] + Tangent2.SetLength(AlphaRight);
	}

	// Calculate Q1 (differential)
	for (i = 0; i<=2; i++)
	{
		Q1[i]= (Bezier[i+1]-Bezier[i]) * 3.0;
	}

	// Calculate Q2 (2nd differential)
	for (i = 0; i<=1; i++)
	{
		Q2[i] = (Q1[i+1]-Q1[i]) * 2.0;
	}

}


/********************************************************************************************

>	TraceBoundaryPoint TraceRegion::BezierPoint( TraceBoundaryPoint* Bez, double u)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com> & Alex
	Created:	7/3/94
	Inputs:		Bez - The control points of a bezier curve
				u - the normalised distance along the bezier that we are interested in
	Returns:	The coord of the point that is a distance u into the bezier. for example,
				if u = 0.5 then the coord of the point half way along the bezier will be
				returned
	Purpose:	This function simply evaluates the bezier function for a given position and
				is used to help when determining how good a fit we have obtained
	Scope:		private
	SeeAlso:	TraceRegion::CalcMaxError

********************************************************************************************/

TraceBoundaryPoint TraceRegion::BezierPoint( TraceBoundaryPoint* Bez, double u)
{
	double OneMinus = 1.0-u;
	double uSquared = u*u;
	double OneMinusSquared = OneMinus*OneMinus;

	TraceBoundaryPoint Coord;
	Coord = Bez[0]*(OneMinusSquared*OneMinus);
	Coord = Coord + Bez[1]*(3.0*u*OneMinusSquared);
	Coord = Coord + Bez[2]*(3.0*uSquared*OneMinus);
	Coord = Coord + Bez[3]*(uSquared*u);

	return Coord;
}



/********************************************************************************************

>	BOOL TraceRegion::CalcMaxError(INT32 LeftPoint, INT32 RightPoint,
								   TraceBoundaryPoint* Bezier, INT32* SplitPoint, double * MaxDist, INT32 Level)

	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/94
	Inputs:		LeftPoint, RightPoint - the index of the start and send of the section - note these are indexes
				into the T Array
				Bezier - the control points of the bezier we have fitted to the points
				Level - a recursion level counter
	Outputs:	SplitPoint - the point to split the curve at if the error is too great
				MaxDist - The biggest distance
	Returns:	TRUE if the bezier should be split
	Purpose:	Finds out how good a fit the bezier curve we have created is when compared
				with the data points
	Scope:		private

This is done by recursive splitting of the line (as the largest error terms are likely to be
in the middle we are extremely likely to only do one split). However, in order to find the
maximum error point we force 3 levels of recursion.

Cunningly we move the TValues about if we think they are salvageable.

********************************************************************************************/

BOOL TraceRegion::CalcMaxError(INT32 LeftPoint, INT32 RightPoint, TraceBoundaryPoint* Bezier,
								 INT32* SplitPoint, double * MaxDist, INT32 Level)
{
	TraceBoundaryPoint Q_t, Q1_t, Q2_t;	// t evaluated at Q, Q' & Q''
	INT32 MidPoint = (LeftPoint + RightPoint+1)/2;  // where we are going to evaluate it
	double Distance;
	
	if (!((MidPoint == LeftPoint) || (MidPoint==RightPoint)))
	{
	
		for (INT32 iterations=1; iterations<=3; iterations++)
		{		
			double t = TValues[MidPoint];
			TraceBoundaryPoint pt = BoundaryBuffer[TPoints[MidPoint]];

			// Calculate where the curve actually is and find the distance from where we want it
			Distance = (BezierPoint(Bezier, t) - pt).SquaredLength();

			if ((Distance > *MaxDist) && (Distance < Error * 2.0))
			// If this is would be the new winner, and it's salvageable (less than twice as far away as we
			// we want), recalculate a better TValue
			{

				// Evaluate Q, Q' & Q''
				double OneMinus = 1.0-t;
				double tSquared = t*t;
				double OneMinusSquared = OneMinus*OneMinus;

				Q_t = Bezier[0] *(OneMinusSquared*OneMinus) + Bezier[1]*(3.0*t*OneMinusSquared)
					 +Bezier[2] *(3.0*tSquared*OneMinus) + Bezier[3]*(tSquared*t);
		        Q1_t = Q1[0]*(OneMinusSquared) + Q1[1]*(2.0*t*OneMinus) + Q1[2]*(tSquared);
				Q2_t = Q2[0]*(OneMinus) + Q2[1]*(t);

				// Now Newton Raphson tprime = t - f(t)/f'(t)
				TraceBoundaryPoint QtMinuspt = Q_t - pt;

//	changed by Ed Cornes 31/10/95 in an attempt to remove an itermittent floating point exception in the tracer
//				t = t - (  (QtMinuspt.x)*(Q1_t.x) + (QtMinuspt.y) * (Q1_t.y) ) /
//						(  Q1_t.SquaredLength() + ((QtMinuspt.x) * (Q2_t.x) + (QtMinuspt.y) * (Q2_t.y)) );
				double denom = Q1_t.SquaredLength() + ((QtMinuspt.x) * (Q2_t.x) + (QtMinuspt.y) * (Q2_t.y));
				if (fabs(denom)<1e-50)
					denom = (denom<0) ? -1e-50 : 1e-50;

				t = t - (  (QtMinuspt.x)*(Q1_t.x) + (QtMinuspt.y) * (Q1_t.y) ) / denom;
				if (t<0) t=0;
				if (t>1) t=1;
				//TRACEUSER( "Alex", _T("Point %f moves to %f\n"),TValues[MidPoint],t);
				TValues[MidPoint] = t;
			}
			else break; // Leave 'for' loop - point OK or point not salvageable
		}

		if ( Distance >= *MaxDist)
		{
			*MaxDist = Distance;
			*SplitPoint = MidPoint;
		}

		// If we haven't got a point to split at yet, or if we're in the 4 first points,
		// recurse down. This ensures we do 8 points.
		if ((*MaxDist<=Error) || (Level<3)) CalcMaxError(LeftPoint, MidPoint,
														 Bezier, SplitPoint, MaxDist, Level+1);	
		if ((*MaxDist<=Error) || (Level<3)) CalcMaxError(MidPoint, RightPoint,
														 Bezier, SplitPoint, MaxDist, Level+1);	
		
	}	
	return(*MaxDist>Error);
}






/********************************************************************************************

>	TraceBoundaryPoint TraceRegion::LeftTangent(INT32 Start, INT32 Points)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com> & Alex
	Created:	7/3/94
	Inputs:		Start - the index of the point at the start of the segment to fit
				Points - the max number of points that can be scanned
	Returns:	The tangent at the point Start
	Purpose:	Finds the Left tangent at the given point in the path
	Scope:		private

********************************************************************************************/

TraceBoundaryPoint TraceRegion::LeftTangent(INT32 Start, INT32 Points)
{
	TraceBoundaryPoint Tangent(0,0);

	Points = 1+MIN (Points/2,MAXTANGENTPOINTS);
	if (Points<2) Points=2;

	// Make sure that is not of zero length
	while ((Tangent.x==0) && (Tangent.y==0))
	{
		if ((--Points)>0)
		{
			Tangent.x = BoundaryBuffer[Start+Points].x - BoundaryBuffer[Start].x;
			Tangent.y = BoundaryBuffer[Start+Points].y - BoundaryBuffer[Start].y;
		}
		else
		{
			ERROR3("Tangent was a zero length vector in the curve fitter (Left)");
			Tangent.x = -1;
		}
	}

	return Tangent;
}


/********************************************************************************************

>	TraceBoundaryPoint TraceRegion::RightTangent(INT32 End, INT32 Points)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com> & Alex
	Created:	7/3/94
	Inputs:		End - the index of the point at the end of the segment to fit
				Points - the max number of points that can be scanned
	Returns:	The tangent at the point End
	Purpose:	Finds the Right tangent at the given point in the path
	Scope:		private

********************************************************************************************/

TraceBoundaryPoint TraceRegion::RightTangent(INT32 End, INT32 Points)
{
	TraceBoundaryPoint Tangent(0,0);

	Points = 1+MIN (Points/2,MAXTANGENTPOINTS);
	if (Points<2) Points=2;

	// Make sure that is not of zero length
	while ((Tangent.x==0) && (Tangent.y==0))
	{
		if ((--Points)>0)
		{
			Tangent.x = BoundaryBuffer[End-Points].x - BoundaryBuffer[End].x;
			Tangent.y = BoundaryBuffer[End-Points].y - BoundaryBuffer[End].y;
		}
		else
		{
			ERROR3("Tangent was a zero length vector in the curve fitter (Right)");
			Tangent.x = -1;
		}
	}

	return Tangent;
}



/********************************************************************************************

>	TraceBoundaryPoint TraceRegion::CentreTangent(INT32 Centre, INT32 Points)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com> & Alex
	Created:	7/3/94
	Inputs:		Centre - the index of the split point in the path
				Points - the max number of points that can be scanned
	Returns:	The tangent at the point Centre
	Purpose:	Finds the tangent at the centre of the path
	Scope:		private

********************************************************************************************/

TraceBoundaryPoint TraceRegion::CentreTangent(INT32 Centre, INT32 Points)
{
	TraceBoundaryPoint Left, Right;
	TraceBoundaryPoint Tangent(0,0);

	Points = 1+MIN (Points/2,MAXTANGENTPOINTS);
	if (Points<2) Points=2;

	// Make sure that is not of zero length
	while ((Tangent.x==0) && (Tangent.y==0))
	{
		if ((--Points)>0)
		{
			// Calc right tangent
			Left.x = BoundaryBuffer[Centre-Points].x - BoundaryBuffer[Centre].x;
			Left.y = BoundaryBuffer[Centre-Points].y - BoundaryBuffer[Centre].y;

			// Calc left tangent
			Right.x = BoundaryBuffer[Centre].x - BoundaryBuffer[Centre+Points].x;
			Right.y = BoundaryBuffer[Centre].y - BoundaryBuffer[Centre+Points].y;

			// Average to get the centre tangent
			Tangent.x = (Left.x + Right.x) / 2;
			Tangent.y = (Left.y + Right.y) / 2;

		}
		else
		{
			ERROR3("Tangent was a zero length vector in the curve fitter (Centre)");
			Tangent.x = -1;
		}
	}

	return Tangent;
}



/********************************************************************************************

>	void TraceRegion::InsertBezier(TraceBoundaryPoint* Bezier, BOOL IsStartCusp, BOOL IsEndCusp)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com> & Alex
	Created:	7/3/94
	Inputs:		Bezier - the control points of the bezier curve to add to the path
				IsStartCusp - TRUE if the start of this bezier is at a cusp
				IsEndCusp - TRUE if the End of this bezier is at a cusp
	Purpose:	Adds the bezier curve to the path. If it is that last curve in the fitting
				(ie Depth is 0) then the rotate flag will not be set
	Scope:		private

********************************************************************************************/

void TraceRegion::InsertBezier(TraceBoundaryPoint* Bezier, BOOL IsStartCusp, BOOL IsEndCusp)
{
	// Prepare some flags
	PathFlags Flags;
	Flags.IsSelected = FALSE;
	Flags.IsSmooth = FALSE;
	Flags.IsRotate = TRUE;

	// Add this Bezier curve into the path
	ThePath->InsertCurveTo( DocCoord( (INT32)Bezier[1].x, (INT32)Bezier[1].y),
							 DocCoord( (INT32)Bezier[2].x, (INT32)Bezier[2].y),
							 DocCoord( (INT32)Bezier[3].x, (INT32)Bezier[3].y), &Flags);

	// Deal with cusps
	if (IsStartCusp || IsEndCusp)
	{
		// Go find out about the path
		PathFlags* AllFlags = ThePath->GetFlagArray();
		INT32 NumCoords = ThePath->GetNumCoords();

		if (IsStartCusp)
		{
			// Patch up the flags of the bits near that start
			AllFlags[NumCoords-3].IsRotate = FALSE;
		}
	
		if (IsEndCusp)
		{
			// Patch up the flags of the bits near that end
			AllFlags[NumCoords-2].IsRotate = FALSE;
			AllFlags[NumCoords-1].IsRotate = FALSE;
		}
	}
}



/********************************************************************************************

>	void TraceRegion::InsertLine(const TraceBoundaryPoint& Start, const TraceBoundaryPoint& End, 
									TraceBoundaryPoint Tangent1, TraceBoundaryPoint Tangent2, BOOL IsStartCusp, BOOL IsEndCusp)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com> & Alex
	Created:	7/3/94
	Inputs:		Start - the coord of the start point of the line
				End - the coord of the end point of the line
				IsStartCusp - TRUE if the start of this bezier is at a cusp
				IsEndCusp - TRUE if the End of this bezier is at a cusp
	Purpose:	Inserts a straight curve into the path. It keeps it as a curve so that the
				path will continue to look smooth after it is edited. If this is the last
				Path element in the fit, (ie Depth is zero) then the rotate flag will not
				be set.
	Scope:		private

I've simplified the maths in this somewhat since over the FitCurve implementation.

********************************************************************************************/

void TraceRegion::InsertLine(const DocCoord& Start, const DocCoord& End,
							 BOOL IsStartCusp, BOOL IsEndCusp)
{
	// Prepare some flags
	PathFlags Flags;
	Flags.IsSelected = FALSE;
	Flags.IsSmooth = FALSE;
	Flags.IsRotate = TRUE;

	// Find out a third of the distance between the two points
	DocCoord StartPos;
	DocCoord EndPos;

	StartPos.x = (2 * Start.x + End.x) /3;
	StartPos.y = (2 * Start.y + End.y) /3;
	EndPos.x = (Start.x + 2 * End.x) /3;
	EndPos.y = (Start.y + 2 * End.y) /3;

	// Add the line to the curve
	ThePath->InsertCurveTo( (DocCoord) StartPos, (DocCoord) EndPos, (DocCoord) End, &Flags);	
	
	// Patch up the path
	PathVerb * AllVerbs = ThePath->GetVerbArray();
	PathFlags* AllFlags = ThePath->GetFlagArray();
	INT32 NumCoords = ThePath->GetNumCoords();
	ERROR3IF(NumCoords<4,"No initial moveto ?!");

	AllFlags[NumCoords-1].IsRotate = !IsEndCusp;	// Endpoint
	AllFlags[NumCoords-2].IsRotate = !IsEndCusp; // 2nd ctl

	if (AllVerbs[NumCoords-4] == PT_MOVETO)
	{
		AllFlags[NumCoords-3].IsRotate = FALSE;
		AllFlags[NumCoords-4].IsRotate = FALSE;
	}
	else
	{
		//BOOL IsStartCusp = AllFlags[NumCoords-5].IsRotate;

		AllFlags[NumCoords-3].IsRotate = !IsStartCusp;
		AllFlags[NumCoords-4].IsRotate = !IsStartCusp;

		
	}

}




/********************************************************************************************

>	void TraceRegion::InsertStraightLine(const TraceBoundaryPoint& End)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com> & Alex	& Alex
	Created:	25/4/94
	Inputs:		End - the coord of the end point of the line
	Purpose:	Inserts a straight Line into the path. This is used in cases where the
				Line segment must be kept as a straight line (ie by using Straight Line
				Mode of the FreeHand tool)
	Scope:		private

********************************************************************************************/

void TraceRegion::InsertStraightLine(const DocCoord& End)
{
	// Prepare some flags
	PathFlags Flags;
	Flags.IsSelected = FALSE;
	Flags.IsRotate = FALSE;
	Flags.IsSmooth = FALSE;

	// Insert the line
	ThePath->InsertLineTo((DocCoord) End, &Flags);	
}



/********************************************************************************************

>	void TraceRegion::Test(UndoableOperation * Op)
					
	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/11/94
	Inputs:		^Op
	Outputs:	none
	Returns:	nothing
	Purpose:	Internal test routine
	Errors:		none
	SeeAlso:	-

A test routine

********************************************************************************************/


void TraceRegion::Test(UndoableOperation * Op)
{
	 
	BOOL CarryOn=TRUE;
	Range Sel(*(GetApplication()->FindSelection()));

	Node* FirstSelectedNode = Sel.FindFirst(); 
	if (FirstSelectedNode != NULL) // No nodes selected so End
	{
	    Node* CurrentNode = FirstSelectedNode;       
	    Node* NextCurrent; 
	    
	    // Do all bitmaps
		while ((CurrentNode != NULL) && CarryOn)
		{
			NextCurrent = Sel.FindNext(CurrentNode);
	       	if  ( (CurrentNode->IsSelected()) && (CurrentNode->GetRuntimeClass() == CC_RUNTIME_CLASS(NodeBitmap)) ) 
	       	{         
				KernelBitmap * pBitmap = ((NodeBitmap *)(CurrentNode))->GetBitmap();
				BitmapInfo BMInfo;
				UINT32 bpp;
				pBitmap->ActualBitmap->GetInfo(&BMInfo);
				bpp=BMInfo.PixelDepth;
		   		
  				TRACEUSER( "Alex", _T("Bitmap found %d bpp\n"),bpp);

				if ((bpp==32) || TRUE)
				{
					CarryOn = FALSE;
//					NodeBitmap *pNodeBitmap = new NodeBitmap;
//					if ((pNodeBitmap == NULL) || (!pNodeBitmap->SetUpPath(12,12)))
//						return;

					Spread *pSpread;
					DocCoord Origin;

					// For now, position Draw objects on 1st page of spread 1
					Node *pNode = (Document::GetSelected())->GetFirstNode()->FindNext()->FindFirstChild();
					while ((pNode != NULL) && (!pNode->IsKindOf(CC_RUNTIME_CLASS(Chapter))))
						pNode = pNode->FindNext();
		
					ENSURE(pNode->IsKindOf(CC_RUNTIME_CLASS(Chapter)), 
						   "Filter::GetFirstSpread(): Could not find Chapter");
					Chapter *pChapter = (Chapter *) pNode;
	
					// pSpread is a child of pChapter
					pSpread = (Spread *) pChapter->FindFirstChild();
					ENSURE(pSpread->IsKindOf(CC_RUNTIME_CLASS(Spread)),
						   "Filter::GetFirstSpread(): Could not find Spread");

					Page *pPage = (Page *) pSpread->FindFirstPageInSpread();
					ENSURE(pPage->IsKindOf(CC_RUNTIME_CLASS(Page)),
						   "BaseBitmapFilter::DoImport(): Could not find first Page");
		
					// Use bottom left of page as origin
					DocRect PageRect = pPage->GetPageRect();
					Origin = PageRect.lo;
	
					NodePath * pNewNode;
					pNewNode = new NodePath;

					TraceRegion TR;

//					if ((pNode = TR->Trace(pBitmap,((NodeBitmap *)(CurrentNode))->Parallel)) == NULL) {return;}

					TR.UseBitmap(pBitmap);
					TR.UsePath(&(pNewNode->InkPath));
					TR.TraceBoundary(((NodeBitmap * )CurrentNode)->Parallel[3],
									 ((NodeBitmap * )CurrentNode)->Parallel[2],
									 ((NodeBitmap * )CurrentNode)->Parallel[0]);

					// Insert the node, but don't invalidate its region
					if (!Op->DoInsertNewNode(pNewNode, pSpread, FALSE))
					{
						// It didn't work - delete the sub-tree we just created, and report error.
						delete pNewNode;
						return;
					}
														
					// Invalidate the region
					Op->DoInvalidateNodeRegion(pNewNode, TRUE, FALSE);
				
				}
	       	}
			CurrentNode = NextCurrent; 
		}

	} 

//	if (CarryOn) BitmapEffectSILT::RunA();

	return;
}
