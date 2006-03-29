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
// This file implements the fractal filling code itself

/*
*/

#include "camtypes.h"
#include "bitmap.h"
#include "bitmpinf.h"
#include "fracfill.h"
//WEBSTER-Martin-14/01/97
#ifndef EXCLUDE_BFX
//#include "bfxasm.h"
#endif
#include "errors.h"

// This is not compulsory, but you may as well put it in so that the correct version
// of your file can be registered in the .exe
DECLARE_SOURCE("$Revision: 662 $");

// An implement to match the Declare in the .h file.
// If you have many classes, it is recommended to place them all together, here at the start of the file
CC_IMPLEMENT_DYNCREATE(PlasmaFractalFill, ListItem)

// This will get Camelot to display the filename and linenumber of any memory allocations
// that are not released at program exit
// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW


const INT32 MAX_FRACTAL_COORD = (1<<28); // All fractals are plotted in a coordinate space
										// going 0..(1<<31)-1

/********************************************************************************************

>	void PlasmaFractalFill::SetSeed(INT32 NewSeed);
					
	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/9/94
	Inputs:		NewSeed - the new seed
	Outputs:	(changes seed)
	Returns:	Nothing
	Purpose:	Sets the random number seed of our fabby platform independent random
				number generator. Actually the seed is 33 bits long and we only allow
				half those values to be set. I don't think anyone will notice though :-).
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void PlasmaFractalFill::SetSeed(INT32 NewSeed)
{
	CurrentSeed = (UINT32) NewSeed;
	CurrentSeed2 = 1; // Don't use 0 here as (0,0) has sequ length of zero
}

/********************************************************************************************

>	void PlasmaFractalFill::GetRandom()
					
	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/9/94
	Inputs:		NewSeed - the new seed
	Outputs:	a random number & changes seed
	Returns:	Nothing
	Purpose:	Find the next random number in a sequence as detailed ARM data manual p 2-51
	Errors:		-
	SeeAlso:	-

This is a Roger/Sophie Wilson routine port and thus incomprehensible

ARM Code:
; Enter with seed in R0 (32 bits) R1 (1 bit in b0 only)
	TST		R1, R1, LSR #1			; b0 into carry
	MOVS	R2, R0, RRX				; 33 bit rotate
	ADC		R1, R1, R1				; Carry into lsb of R1 (rest of it junked)
	EOR		R2, R2, R0, LSL #12		; dual
	EOR		R0, R2, R2, LSR #20		; tap
; exit as before

********************************************************************************************/

// All must be INT32, t is temporary, the routine sorts out types
#define DoRand(Seed1, Seed2, t) \
	t = ((UINT32)(Seed1) >> 1 ) ^ ((UINT32)(Seed2)<<31); \
	Seed2 = ((UINT32)(Seed1) & 1); \
	t ^= (UINT32)(Seed1) << 12; \
	Seed1 = ((UINT32)(t)) ^ ((UINT32)(t>>20));

inline INT32 PlasmaFractalFill::GetRandom()
{
	UINT32 t;
	DoRand(CurrentSeed, CurrentSeed2, t);
	return((INT32)(CurrentSeed));	
}


/********************************************************************************************

>	INT32 PlasmaFractalFill::Adjust(INT32 pa,INT32 pb,INT32 x, INT32 y
								   INT32 aGraininess, INT32 aGravity)
					
	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/9/94
	Inputs:		(xa, ya) - (xb, yb)	line to adjust
				pa, pb				potentials at these two coordinates
				x, y				coordinate we're interested in
	Outputs:	Fills bitmap
	Returns:	Potential at (x,y)
	Purpose:	Linterpolates between potentials at (xa, ya, xb, yb) adding a random peturbation
	Errors:		-
	SeeAlso:	-

********************************************************************************************/


inline INT32 PlasmaFractalFill::Adjust(INT32 pa,INT32 pb,INT32 x, INT32 y,
								      INT32 aGraininess, INT32 aGravity)
#ifdef FASTFRACTALADJUST
{
// The squash term only needs to be compared against zero and this disgustingness assumes that the
// representation of (fixed16)0 is hex 0 on all architectures
	return FastFractalAdjust(pa, pb, x, y, aGraininess, aGravity, Seed, *((INT32 *)(&Squash)), RecursionLevel);
}
#else
{
	// Bodge to pseudorandomate psuedorandomiser from coords alone!
	UINT32 topseed=1;
	INT32 potential=(pa ^ (pb<<16) ^ _lrotl((UINT32)x,16)^((UINT32) y) ^ Seed ^ 0xABCD1234);
	UINT32 t;
	DoRand(potential,topseed,t);	// Do an initial spin
	potential = _lrotl(potential,(x^(x>>5)^(x>>10)^(x>>15)^(x>>20)^(x>>25)^(x>>30)^y^(y>>5)^(y>>10)^(y>>15)^(y>>20)^(y>>25)^(y>>30)) & 31);
	DoRand(potential,topseed,t);	// random number to potential

	// When squashing we only peturb along the edges and the rest is pure linterp.
	// (effect of gravity unknown!). At the edges (because of CoordinateMask) one of
	// x & y is zero. We can cunningly use the logical operators to make this a quick test.
	if ((Squash!=0) && !(x || y)) potential = 0;
	
	// The adjustment is mostly linear interpolation (linterp) (i.e. (pa+pb)/2) but we add:
	//
	// Peterbation effects: We use a random number scaled by graininess in proportion to the (y1-y2) which
	// simply means shifting it right by the recursion level.				
	//
	// Gravitational effects:
	// We linterp between y1 and y2. But we want this to look like a parabola. So if xh is the midpoint:
	//
	//          2			2			2			   2
	// p1 = a y1   p2 = a y2   ph = a yh  = a (y1 + y2)	/4
	//
	//										2
	// ph = (p1+p2)/2 - (p1+p2)/2 + a(y1+y2) /4
	//
	//								2
	// ph = (p1+p2)/2 - (a/4)(y1-y2)
	//                  ~~~~~~~~~~~~~
	//
	// Thus we must subtract the underlined term to make it look like a parabola. However a is negative
	// and we operate using a constant offset as our potential is high in the middle and zero at the edges
	// (the same annoyance happens all over potential theory!) so we acually add the term. We get the squared
	// term by shifting right by 2xrecursion level.
	//

	potential = ( ( ((INT32)(aGraininess)) * ((INT32)(potential>>17)) ) >>RecursionLevel) /*peterb*/
				- (aGravity>>(RecursionLevel*2)) /*gravity*/
				+ (( pa + pb + 1 ) >> 1 ); /* linterp */

	// Clip potential to max & min
	if ( potential > MaxPotential ) potential = MaxPotential;
	if ( potential < 0 ) potential = 0;

	//if (p!=potential) {TRACEUSER( "Alex", _T("Awooga awooga maths error\n"));}
	return(potential);
}
#endif

/********************************************************************************************

>	BOOL PlasmaFractalFill::SubDivide(INT32 x1, INT32 y1, INT32 x2, INT32 y2,
										 INT32 p11, INT32 p12, INT32 p21, INT32 p22, BOOL PlotNow)
	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/9/94
	Inputs:		(x1, y1) - (x2, y2) rectangle to subdivide
				pxx = potential at a corner (e.g. p21 = potential at (x2,y1))
				PlotNow = TRUE to plot pixel at x1,y1 with potential p11
	Outputs:	Recursive (adjusts bitmap)
	Returns:	TRUE if succeeded, FALSE if failed
	Purpose:	Fills the 8 bit bitmap passed with the fractal requested
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL PlasmaFractalFill::SubDivide(INT32 x1, INT32 y1, INT32 x2, INT32 y2,
	   						 	  INT32 p11, INT32 p12, INT32 p22, INT32 p21, BOOL PlotNow)
{
	INT32 xh, yh;
	INT32 ph1, ph2, p1h, p2h, phh;

#ifdef CRUMMYOLDCODE
	// First check this square is at least partially within the clip rectangle and thus is worth
	// subdividing. It only is NOT worth subdiving if the highest coordinate is too low, or the
	// lowest coordinate too high.

	if (  (((x2>>EigenValue)-Ox)<0) || (((x1>>EigenValue)-Ox)>Width)
	   || (((y2>>EigenValue)-Oy)<0) || (((y1>>EigenValue)-Oy)>Height) ) return (TRUE);

	// We only ever plot the left hand corner. This implies the right most pixel never gets plotted
	// which is fine as the pixels go 0..n-1 and we pass n in as the first coordinate.
	// We use UINT32 casting to check the value is both smaller than Width and non-negative.
	if ( (PlotNow) && (((UINT32)((x1>>EigenValue)-Ox))<(UINT32)Width)
	  	           && (((UINT32)((y1>>EigenValue)-Oy))<(UINT32)Height) )
		pBitmap->PlotPixel((x1>>EigenValue)-Ox, (y1>>EigenValue)-Oy, (PixelGreyscale) ~(p11>>8));

	if ( ( ((x2>>EigenValue)-(x1>>EigenValue)) <2 ) && ( ((y2>>EigenValue)-(y1>>EigenValue)) <2 ) ) return(TRUE);
#else
	// See how stupid the VCC optimiser is. OK, I'll do its job for it then. Soon no doubt I'll get round to writing
	// this whole thing in assembler
	{
		INT32 x1e=((x1>>EigenValue)-Ox); // one would hope the optimiser leaves the shift in CX. Oh no it doesn't...
		INT32 x2e=((x2>>EigenValue)-Ox);
		INT32 y1e=((y1>>EigenValue)-Oy);
		INT32 y2e=((y2>>EigenValue)-Oy);

		// First check this square is at least partially within the clip rectangle and thus is worth
		// subdividing. It only is NOT worth subdiving if the highest coordinate is too low, or the
		// lowest coordinate too high.

		if (  (x2e<0) || (x1e>Width)
		   || (y2e<0) || (y1e>Height) ) return (TRUE);

		// We only ever plot the left hand corner. This implies the right most pixel never gets plotted
		// which is fine as the pixels go 0..n-1 and we pass n in as the first coordinate.
		// We use UINT32 casting to check the value is both smaller than Width and non-negative.
		if ( (PlotNow) && (((UINT32)x1e)<(UINT32)Width)
		  	           && (((UINT32)y1e)<(UINT32)Height) )
			pBitmap->PlotPixel(x1e, y1e, (PixelGreyscale) ~(p11>>8));

		// we cunningly note that ((x2>>EigenValue)-(x1>>EigenValue)) = x2e-x1e
		if ( ( (x2e-x1e) <2 ) && ( (y2e-y1e) <2 ) ) return(TRUE);
	}
#endif

	// Mark the fact we are one recursion level deeper.	
	RecursionLevel++;

	// Calculate the midpoints
	xh = (x1+x2)>>1;
	yh = (y1+y2)>>1;

	/*
	This is the way adjust is going to calculate the potential at the midpoints:

	   x1		 xh		  x2
	 y2__________|_________y2
	   |p12     C|ph2	p22|
	   |		 |		   |
	   |    4    |    3    |
	   |		 |		   |
	   |		 |		   |
	 yh|D_______E|________B|yh
	   |p1h      |phh	p2h|
	   |		 |		   |
	   |		 |		   |
	   |	1	 |    2    |
	   |		 |		   |
	 y1|p11_____A|ph1___p21|y1
	   x1        xh       x2

	Adjust splits each line (in order A, B, C, D) to find the potential at its midpoint adding a random peturbation.
	The point E is then calculated as the average of A, B, C & D. As we calculate the potentials at A, B, C, D, E we
	plot them. We can then subdivide each smaller square in the order 1, 2, 3, 4.
	*/

	ph1=Adjust( p11, p21, xh & CoordinateMask, y1 & CoordinateMask, xGraininess, xGravity);
	p2h=Adjust( p21, p22, x2 & CoordinateMask, yh & CoordinateMask, yGraininess, yGravity);
	ph2=Adjust( p12, p22, xh & CoordinateMask, y2 & CoordinateMask, xGraininess, xGravity);
	p1h=Adjust( p11, p12, x1 & CoordinateMask, yh & CoordinateMask, yGraininess, yGravity);
	phh = (ph1+p2h+ph2+p1h+2)>>2;

	// When we're at the top level under gravity we make the midpoint full potential	
	if (RecursionLevel==1)
	{
		if (ForceCornersToZero )
		{
			phh=MaxPotential;
		}
		else
		{
			// if it's tileable we set it to the opposite extreme.
			if (Tileable) phh = MaxPotential - p11; // p11==p12==p21==p22 as tileable recursion1
		}
	}

	// Now subdivide our square into four
	if (   SubDivide( x1, y1, xh, yh, p11, p1h, phh, ph1, FALSE ) // This pixel's just been plotted
	 	&& SubDivide( xh, y1, x2, yh, ph1, phh, p2h, p21, TRUE  ) // These ones haven't
		&& SubDivide( xh, yh, x2, y2, phh, ph2, p22, p2h, TRUE  )
		&& SubDivide( x1, yh, xh, y2, p1h, p12, ph2, phh, TRUE ))
	{
		RecursionLevel--;
		return(TRUE);
	}
	else
	{
		RecursionLevel--;
		return(FALSE);
	}
}


/********************************************************************************************

>	PlasmaFractalFill::PlasmaFractalFill(
					INT32 tSeed = 0,
					BOOL tTileable = 1
					fixed16 tSquash = 0,
					fixed16 tGraininess = 1,
					fixed16 tGravity = 0,
					)

	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/9/94
	Inputs:		INT32 Seed (seed of the random number generator)
				BOOL Tileable (true if the fractal can be tiled)
				fixed16 XSquash (amount of Squashing, 1.0 = none)
				fixed16 Graininess (fractal's graininess)
				fixed16 Gravity (gravitational pull towards graininess of the centre)
	Outputs:	Constructs a PlasmaFractalFill
	Returns:	TRUE if succeeded, FALSE if failed
	Purpose:	Constructor for a PlasmaFractalFill
	Errors:		-
	SeeAlso:	-

Tileable fractals are just as fabby as normal fractals except they can be tiled. It's in here
as a flag partle as you might want one that wasn't tileable (why?) but tiled fractals in
general exhibit less assymetry than non-tiled fractals, especially with zero gravity.

If Squash is zero, no squashing is done. If it is negative, the image is XSquashed, and if it
is positive, the image is YSquashed. Squashing only works if Tileable is set (unless you want
the special grey morass effect).

********************************************************************************************/

PlasmaFractalFill::PlasmaFractalFill(
					INT32 tSeed,
					BOOL tTileable,
					fixed16 tSquash,
					fixed16 tGraininess,			//0..about 32
					fixed16 tGravity				//0..about 255
					)
{
	MaxPotential = (1<<16)-1;  						// *** DON'T CHANGE WITHOUT CHANGING THE
													// EQUATE IN FastFractalAdjust

	pBitmap = NULL;
	Seed = tSeed;
	ForceCornersToZero = (tGravity != 0);
	Tileable = tTileable;
	Squash = tSquash;

	if (Tileable)
	{
		CoordinateMask = MAX_FRACTAL_COORD - 1;
	}
	else
	{
		CoordinateMask = 0xffffffff;
	}
	
	xGravity = yGravity = (UINT32) (tGravity.MakeDouble() * MaxPotential);
		xGraininess = yGraininess = (UINT32) (tGraininess.MakeDouble() * 8 + 0.5);

	if (Squash > 0)
	{
		xGravity = (UINT32) ( ((tGravity.MakeDouble() * MaxPotential) / Squash.MakeDouble()));
		xGraininess = (UINT32) ( ((tGraininess * 8 + 0.5) / Squash).MakeLong());
	}
	else if (Squash<0)
	{
		yGravity = (UINT32) ( ((tGravity.MakeDouble() * MaxPotential) / (-Squash.MakeDouble())));
		yGraininess = (UINT32) ( ((tGraininess * 8 + 0.5) / (-Squash)).MakeLong());
	}

	// Limit Graininess to prevent overflow in Adjust
	if ( xGraininess < 0 )   xGraininess = 0;
	if ( xGraininess > 0xffff ) xGraininess = 0xffff;
	if ( yGraininess < 0 )   yGraininess = 0;
	if ( yGraininess > 0xffff ) yGraininess = 0xffff;
	
}

void PlasmaFractalFill::Test(KernelBitmap * pB,
					INT32 tSeed,
					BOOL tTileable,
					fixed16 tSquash,
					fixed16 tGraininess,
					fixed16 tGravity)

{
	PlasmaFractalFill pf(tSeed?tSeed:rand(), tTileable, tSquash, tGraininess, tGravity);
	pf.DoFill(pB);
}

/********************************************************************************************

>	static INT32 PlasmaFractalFill::GetDimension(INT32 x, INT32 y)

	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/9/94
	Inputs:		x and y are pixel width/height of the (detransformed) bounding box you have to
				fill.
	Outputs:	None
	Returns:	Dimension of virtual fractal needed
	Purpose:	Returns required dimension of virtual fractal
	Errors:		-
	SeeAlso:	PlasmaFractalFill::DoFill

The fractal bitmap must be power of 2 in width and it must be square (i.e. width = height) for
resolution independent bitmaps to be created reliably. Given that most fractals will not be
exactly this size, the expected use is that they will be generated at one such size and a scaled
sprite plot be done.

This routine returns a suitable set of 'virtual fractal coordinate bounds' (i.e. a size of the
theoretical fractal bitmap in pixels if it were plotted unclipped). This is a power of two. This
should be used to work out the clipping calculations as the only coordinates the routines know how
to deal with are of this sort.

If you ask for an inordinately large fractal the return value may be smaller than the input values.
Presently this value squared is larger than the physical memory capacity of the platform though

You can, of course, use a smaller dimension if (for instance) memory requirements mean you must,
but it must be a power of 2.

********************************************************************************************/

INT32 PlasmaFractalFill::GetDimension(INT32 x, INT32 y)
{
	INT32 Dimension=4; // Minimum
	while ((Dimension < MAX_FRACTAL_COORD) && (( Dimension < x) || (Dimension < y)))
		Dimension=Dimension<<1;
	return(Dimension);
}

/********************************************************************************************

>	BOOL PlasmaFractalFill::DoFill(KernelBitmap * pBitmap, INT32 Dimension, INT32 tOx, INT32 tOy)

	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/9/94
	Inputs:		pBitmap = ^KernelBitmap to fill,
				Dimension=dimension (see notes)
				(Ox, Oy) = origin of bitmap passed inside vitual bitmap
	Outputs:	Fills bitmap
	Returns:	TRUE if succeeded, FALSE if failed
	Purpose:	Fills the 8 bit bitmap passed with the fractal requested
	Errors:		-
	SeeAlso:	PlasmaFractalFill::GetDimension

Dimension must be power of 2 for resolution independent bitmaps to be created reliably. It
describes the length of the sides of the square bitmap into which the unclipped fractal would
be plotted. You may pass this routine another bitmap (need not be square) into which the the
fractal is plotted (clipped). Pixels outside the fractal will not be filled. i.e.:


   0____________________D
  D|					|D
   |                    |
   |        ____________|_____
   |        |           |	 |
   |        | + + + + + |	 |
   |        |           |	 |
   |        | + + + + + |	 | <--- The bitmap you pass (dimensions obtained from the bitmap)
   |      Oy|___________|____|
   |         Ox         |
   |                    |
  0|____________________|0 <--- Virtual fractal bitmap dimension D
   0					D


The area marked with plus signs is plotted.

********************************************************************************************/


BOOL PlasmaFractalFill::DoFill(KernelBitmap * theBitmap, INT32 Dimension, INT32 tOx, INT32 tOy )
{
	INT32 i;
	INT32 rnd[4];
	BitmapInfo Info;

	if ((pBitmap = theBitmap) == NULL)
	{
		ENSURE(FALSE,"PlasmaFractalFill::DoFill pBitmap is NULL");
		return(FALSE);
	}

	pBitmap->ActualBitmap->GetInfo(&Info);
	Width = Info.PixelWidth;
	Height = Info.PixelHeight;
	Ox = tOx;
	Oy = tOy;

	// Dimension zero means 1:1
	if (Dimension==0) Dimension=GetDimension(Width,Height);

	// Bump width and height up
	EigenValue = 0;
	while ((Dimension<<EigenValue) < MAX_FRACTAL_COORD)
	{
		EigenValue++;
	}

	// This checks Width=Height and they are both MAX_FRACTAL_COORD. If all kernel bitmaps passed in
	// fulful this condition then on each subdivision the coordinates will be the same, so the random
	// peterurbation will be the same, so the bitmap will be the same. Wow! And thats the fabbiness of
	// psuedorandom numbers for you.	
	if (Dimension<<EigenValue != MAX_FRACTAL_COORD)
		TRACE( _T("PlasmaFractalFill::DoFill only generates resolution independent bitmaps if Width==Height==2^n \n"));

	// seed random number generator
	SetSeed(Seed);

	// set corner potentials randomly
	for (i=0; i<4; i++) rnd[i]=(INT32)(((((UINT32)GetRandom())>>16)*(UINT32)MaxPotential+(1<<15))>>16);

	// When we are not tiling we want to make quite sure we get enough contrast. We take the contrast range across one
	// diagonal and make the other diagonal have at least one minus that range. When we are tiling we make the value
	// either +1 or -1

	if (Tileable && !(ForceCornersToZero))
	{
		rnd[0]=0;// (rnd[0]>(MaxPotential/2))?MaxPotential:0; this makes them predictable
	}
	else
	{
		INT32 diagdiff1;
		INT32 diagdiff2;
		short a,b;
		// Calculate contrast across diagonals
		diagdiff1=Abs(rnd[0]-rnd[2]);
		diagdiff2=Abs(rnd[1]-rnd[3]);
		// Find smallest one		
		if (diagdiff1<diagdiff2)
		{
			a=(rnd[0]<rnd[2])?0:2;
			b=2-a; // ab diagonal is 0-2 or 2-0
		}
		else
		{
			a=(rnd[1]<rnd[3])?1:3;
			b=4-a; // ab diagonal is 1-3
		}
		// set to how much we want to increase diagdiff by.
		diagdiff2=(MaxPotential-diagdiff1) /*new*/ - diagdiff2 /*current*/;
		if (diagdiff2>0)
		{
			rnd[a]-=diagdiff2/2;
			rnd[b]+=diagdiff2/2;
		}		
	}

	RecursionLevel = 0;
	SubDivide(0,0, MAX_FRACTAL_COORD, MAX_FRACTAL_COORD,
		ForceCornersToZero?0:rnd[Tileable?0:0], ForceCornersToZero?0:rnd[Tileable?0:1],
		ForceCornersToZero?0:rnd[Tileable?0:2], ForceCornersToZero?0:rnd[Tileable?0:3], TRUE );

	return(TRUE);
}


