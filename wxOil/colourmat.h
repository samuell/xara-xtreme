// $Id: colourmat.h 264 2006-01-19 12:28:28Z alex $
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

#ifndef INC_COLOURMAT
#define	INC_COLOURMAT

class ColourMatrix;


class ColourVector : public SimpleCCObject
{
public:
	double elements[5];

	enum ColourVectorElement
	{
		Red		= 0,
		Green 	= 1,
		Blue 	= 2,
		Alpha	= 3,
		Spare	= 4
	};

	ColourVector(double r=0.0, double g=0.0, double b=0.0, double a=1.0, double s=1.0)
	{	
		elements[Red]=r;
		elements[Green]=g;
		elements[Blue]=b;
		elements[Alpha]=a;
		elements[Spare]=s;
	}

	void SetRGBA(UINT32 r, UINT32 g, UINT32 b, UINT32 a=255)
	{
		elements[Red]   = ((double)r)/255.0;
		elements[Green] = ((double)g)/255.0;
		elements[Blue]  = ((double)b)/255.0;
		elements[Alpha] = ((double)a)/255.0;
	}

	static UINT32 Get(double v)
	{
		if (v<0) return 0;
		UINT32 x = (UINT32)(v*255.0+0.5);
		return (x>255)?255:x;
	}

	UINT32 GetR() {return Get(elements[Red]);}
	UINT32 GetG() {return Get(elements[Green]);}
	UINT32 GetB() {return Get(elements[Blue]);}
	UINT32 GetA (){return Get(elements[Alpha]);}

	ColourVector Apply(ColourMatrix &cm);

	ColourVector ComposeOnTop(ColourVector &v)
	{
		// Check if either is transparent
		if (elements[Alpha]==0.0) return v;
		if (v.elements[Alpha]==0.0) return *this;

		// Generate the new alpha value
		double newalpha = v.elements[Alpha] + elements[Alpha] - v.elements[Alpha]*elements[Alpha];
		if (newalpha == 0.0) return ColourVector(elements[Red], elements[Green], elements[Blue], 0.0);

		/* Here's how this works:
			on a normal composition, of (x1,a1), x= x0 * (1-a1) + x1 * a1
			So where (x2,a2) are imposed on top, x = ( x1 * a1 + x0 * (1-a1) ) * (1-a2) + x2 * a2;
			Which can be rewritten as
				x = x0 ( 1- (a1+a2-a1a2) ) + ( a1+a2-a1a2 )/(a1+a2-a1a2)*((1-a2)(x1a1)+x2a2)
			Substituting A = (a1+a2-a2a2), X=((1-a2)(x1a1)+x2a2)/A we get
				x = x0 (1-A) + X A
			Which is the formula for transparency with alpha A, colour X
		*/

		return ColourVector(
				((1-v.elements[Alpha])*(elements[Alpha]*elements[Red]  ) + (v.elements[Red]   * v.elements[Alpha])) / newalpha,
				((1-v.elements[Alpha])*(elements[Alpha]*elements[Green]) + (v.elements[Green] * v.elements[Alpha])) / newalpha,
				((1-v.elements[Alpha])*(elements[Alpha]*elements[Blue] ) + (v.elements[Blue]  * v.elements[Alpha])) / newalpha,
				newalpha);
	}
};

class ColourMatrix : public SimpleCCObject
{
public:
	typedef struct {double el[5*5];} elements_t;

	elements_t elements;

	ColourMatrix()
	{
		INT32 i,j;
		for (i = 0; i<5; i++) for (j=0 ; j<5; j++) elements.el[i+j*5]=(i==j)?1.0:0.0;
	}

	static ColourMatrix Boost(double d=1.2);
	static ColourMatrix Whiten();
	static ColourMatrix Grey();
};



#endif

