// $Id: colourmat.cpp 264 2006-01-19 12:28:28Z alex $
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

#include "colourmat.h"

/********************************************************************************************

>	ColourVector ColourVector::Apply(ColourMatrix &cm)


	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	24/01/2005
	Inputs:		cm - Colour matrix to operate on
	Outputs:	
	Returns:	The matrix multiplied colour vector
	Purpose:	Multiplies a ColourVector by a ColourMatrix and returns the result
	Errors:		-
	SeeAlso:	-

					( a1 a2 a3 a4 a5 )
					( b1 b2 b3 b4 b5 )
	( a b c d e ) 	( c1 c2 c3 c4 c5 ) = (a*a1+b*b1+c*c1+d*d1+e*e1, a*a2+b*b2+c*c2+d*d2+e*e2 etc.)
					( d1 d2 d3 d4 d5 )
					( e1 e2 e3 e4 e5 )

********************************************************************************************/

ColourVector ColourVector::Apply(ColourMatrix &cm)
{
	ColourVector result;
	INT32 i, j;

	for (i=0; i<5; i++) // Column of matrix
	{
		result.elements[i] = 0.0;
		for (j=0 ; j<5 ; j++) // Row of matrix
		{
			result.elements[i]+=elements[j] * cm.elements.el[i+j*5];
		}
	}
	result.elements[Spare]=1.0; // Fix this up in case of rounding peculiarities
	return result;
}

/********************************************************************************************

>	static ColourMatrix ColourMatrix::Boost(double d=1.2)


	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	24/01/2005
	Inputs:		d - boost quantity
	Outputs:	
	Returns:	A matrix which boosts colour
	Purpose:	-
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

ColourMatrix ColourMatrix::Boost(double d)
{
	ColourMatrix r;
	if (d != 1.0)
	{
		double cc = (1.0-d)/2.0; // Add constant boost to centre colour change
		elements_t e = {
			d,   0.0, 0.0, 0.0, 0.0,
			0.0, d,   0.0, 0.0, 0.0,
			0.0, 0.0, d,   0.0, 0.0,
			0.0, 0.0, 0.0, 1.0, 0.0,
			cc,  cc,  cc,  0.0, 1.0
			};
		r.elements = e;
	}
	return r;
}

/********************************************************************************************

>	static ColourMatrix ColourMatrix::Whiten()


	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	24/01/2005
	Inputs:		None
	Outputs:	
	Returns:	A matrix which makes the entire image white
	Purpose:	-
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

ColourMatrix ColourMatrix::Whiten()
{
	ColourMatrix r;
	elements_t e = {
		0.0, 0.0, 0.0, 0.0, 0.0,
		0.0, 0.0, 0.0, 0.0, 0.0,
		0.0, 0.0, 0.0, 0.0, 0.0,
		0.0, 0.0, 0.0, 1.0, 0.0,
		1.0, 1.0, 1.0, 0.0, 1.0
		};
	r.elements=e;
	return r;
}

/********************************************************************************************

>	static ColourMatrix ColourMatrix::Grey()


	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	24/01/2005
	Inputs:		d - boost quantity
	Outputs:	
	Returns:	A matrix which greys the image
	Purpose:	-
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

ColourMatrix ColourMatrix::Grey()
{
	ColourMatrix r;
	elements_t e = {
		0.1, 0.1, 0.1, 0.0, 0.0,
		0.1, 0.1, 0.1, 0.0, 0.0,
		0.1, 0.1, 0.1, 0.0, 0.0,
		0.0, 0.0, 0.0, 1.0, 0.0,
		0.6, 0.6, 0.6, 0.0, 1.0
		};
	r.elements=e;
	return r;
}






#if 0
The control makes a Hot version of a button face from a Normal version at render time if a Hot version was not loaded from the resource system. And the control always makes grey versions using the same techniques:


double colourboost = 1.0;
// If glyph is highlighted but button should be normal, knock it back a bit
// Else if glyph is normal but button should be highlighted, boost it up
if ((BBX_HIGHLIGHT_ID(BMPIndex)) && !(ISSELECTED(hwnd) || (flags & BS_POINTEROVER)))
	colourboost = 0.8;
else if (!(BBX_HIGHLIGHT_ID(BMPIndex)) && (ISSELECTED(hwnd) || (flags & BS_POINTEROVER)))
	colourboost = 1.2;

PlotGdiPlusBitmap(hDC, pShared->pGPBitmap, SrcOffset,
xPlotOffset, yPlotOffset, XSize, YSize,
ISDISABLED(hwnd), colourboost, BGColour);


static void PlotGdiPlusBitmap	(HDC destDC, CGdiPlusBitmap* pBitmap, INT32 SrcOffset,
								INT32 xPlotOffset,INT32 yPlotOffset, INT32 width, INT32 height,
								BOOL Shaded, double colourboost, INT32 BGColour)
{
	// Always fill the entire background with BTNFACE (grey) to make sure we don't
	// miss any pixels - we don't need to be careful about redraw flicker etc
	// now that we're plotting into an offscreen bitmap.
	
	if (pBitmap == NULL)
		return;
	
	Graphics graphics(destDC);
	Status stat;
	
	Gdiplus::Rect faceRect(0, 0, MAXBITMAPSIZE, MAXBITMAPSIZE);
	
	Brush* pHighlightBrush = NULL;
	switch (BGColour)
	{
		case BGCOLOUR_LTGREY:
		{
			Color face;
			face.SetFromCOLORREF(GetSysColor(COLOR_BTNHIGHLIGHT));
			pHighlightBrush = new SolidBrush(face);
		}
		break;
		
		case BGCOLOUR_CREAM:
		{
			pHighlightBrush = new SolidBrush(Color(255, 255, 192));
			// Color face;
			// face.SetFromCOLORREF(GetSysColor(COLOR_BTNSHADOW));
			// pHighlightBrush = new LinearGradientBrush(faceRect, Color(255,255,192), face, LinearGradientModeVertical);
		}
		break;
		
		default:
		{
			Color face;
			face.SetFromCOLORREF(GetSysColor(COLOR_BTNFACE));
			pHighlightBrush = new SolidBrush(face);
		}
		break;
	}
	
	graphics.FillRectangle(pHighlightBrush, 0, 0, MAXBITMAPSIZE, MAXBITMAPSIZE);
	stat = graphics.GetLastStatus();
	
	if (Shaded)
	{
		// Button is shaded (greyed)
		// Make a colour transform here to set all pixels to white
		ImageAttributes effect;
		ColorMatrix whiteMatrix = {
		0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 1.0f, 0.0f, 1.0f
		};
	
		effect.SetColorMatrix(&whiteMatrix);
		
		// Plot white version of glyph first, with slight offset
		Gdiplus::Rect destRect(xPlotOffset+1, yPlotOffset+1, width, height);
		graphics.DrawImage(pBitmap->m_pBitmap, destRect, SrcOffset, 0, width, height, UnitPixel, &effect);
		stat = graphics.GetLastStatus();
		
		// Make a colour transform here to remove hue from the pixels
		ColorMatrix greyMatrix = {
		0.1f, 0.1f, 0.1f, 0.0f, 0.0f,
		0.1f, 0.1f, 0.1f, 0.0f, 0.0f,
		0.1f, 0.1f, 0.1f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		0.6f, 0.6f, 0.6f, 0.0f, 1.0f
		};
		
		effect.SetColorMatrix(&greyMatrix);
		
		// Plot greyed out glyph over the top
		Gdiplus::Rect destRect2(xPlotOffset, yPlotOffset, width, height);
		graphics.DrawImage(pBitmap->m_pBitmap, destRect2, SrcOffset, 0, width, height, UnitPixel, &effect);
		stat = graphics.GetLastStatus();
		
	}
	else
	{
		// Button is not shaded but may have it colour boosted or diminished
		// to indicate other things...
		// Make a colour transform here to apply colour boost
		ImageAttributes effect;
		if (colourboost!=1.0)
		{
			float cb = (float)colourboost;
			float cc = (1.0f-cb)/2.0f; // Add constant boost to centre colour change
			ColorMatrix boostMatrix = {
			cb, 0.0f, 0.0f, 0.0f, 0.0f,
			0.0f, cb, 0.0f, 0.0f, 0.0f,
			0.0f, 0.0f, cb, 0.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
			cc, cc, cc, 0.0f, 1.0f
			};
		
			effect.SetColorMatrix(&boostMatrix);
		}

		Gdiplus::Rect destRect(xPlotOffset, yPlotOffset, width, height);
		graphics.DrawImage(pBitmap->m_pBitmap, destRect, SrcOffset, 0, width, height, UnitPixel, &effect);
		stat = graphics.GetLastStatus();
	}
		
	delete pHighlightBrush;
	pHighlightBrush = NULL;
	
}

Info about GDI+ colour matrix here:
http://msdn.microsoft.com/library/default.asp?url=/library/en-us/gdicpp/GDIPlus/UsingGDIPlus/Recoloring.asp

Phil
#endif
