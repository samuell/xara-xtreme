// $Id$
////////////////////////////////////////////
// Bitmap merging routines
// DMc 17-3-98
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
//#include "app.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "bmanip.h"

/******************************************************************************************

>	BOOL CBitmapManip::ApplyGaussianFilter(BYTE * bits, INT32 wid, INT32 hei, INT32 matrix[3][3],
			INT32 index)


	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1-10-98

	Inputs:		The source bitmap bits, its width and height and the matrix to apply

	Returns:	-

	Purpose:	Applies a 3x3 matrix to the bitmap's alpha channel

	Notes:		Remember DIB's are held upside-down in memory

******************************************************************************************/
BOOL CBitmapManip::ApplyGaussianFilter(BYTE * bits, INT32 wid, INT32 hei, INT32 matrix[3][3], 
									   BOOL bIgnoreBlankPixels)
{
	// we need a temporary bitmap
	INT32 ksize = (wid) * (hei) * 4;

	BYTE * tmpBits = new BYTE[ksize];

	if (!tmpBits)
		return FALSE;

	memcpy(tmpBits, bits, ksize);

	INT32 size = wid * hei;
	size -= wid * 2;
	size -= 2;

	COLORREF *srcPtr = ((COLORREF *)bits);
	COLORREF *destPtr = ((COLORREF *)tmpBits) + wid + 1;

	UINT32 srcAlpha = 0;
	UINT32 destAlpha = 0;

	INT32 incBy1 = wid*4;

	INT32 sum = 0;

	INT32 posX = 0;
	INT32 posY = 1;

	COLORREF Area[3][3];

	INT32 i = 0;
	INT32 j = 0;

	BYTE R = 0;
	BYTE A = 0;

	INT32 SumR;
	INT32 SumA;

	while (posY < hei - 1)
	{
		// transfer colour
		for (i = 0 ; i < 3; i++)
		{
			for (j = 0 ; j < 3; j++)
			{
				Area[i][j] = *(srcPtr + i * wid + j);
			}
		}

		// now, ignore transparent areas
		if (((Area[1][1] & 0xff000000) >> 24) != 0xff || !bIgnoreBlankPixels)
		{
			SumR = 0;
			SumA = 0;
			sum = 0;

			for (i = 0 ; i < 3; i++)
			{
				for (j = 0 ; j < 3; j++)
				{
					A = (BYTE)(Area[i][j] & 0xff000000) >> 24;
					R = (BYTE)(Area[i][j] & 0xff);
					
					if (A != 0xff || !bIgnoreBlankPixels)
					{
						SumR += ((INT32)R) * matrix[i][j];
						SumA += ((INT32)A) * matrix[i][j];
						sum += matrix[i][j];
					}
				}
			}

			SumR /= sum;
			SumA /= sum;
			
			*destPtr = SumR + (SumR << 8) + (SumR << 16) + (SumA << 24);
		}

		// increment of pointers
		posX ++;
		
		destPtr ++;
		srcPtr ++;

		if (posX >= wid - 3)
		{
			destPtr += 3;
			srcPtr += 3;
			posX = 0;
			posY ++;
		}	
	}

	memcpy(bits, tmpBits, wid * hei * 4);

	delete [] tmpBits;

	return TRUE;
}
