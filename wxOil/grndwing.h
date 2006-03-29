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

#include "grndrgn.h"

/********************************************************************************************

>	class GRenderWinG : public GRenderRegion

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/6/94
	Purpose:	A GDraw render region that uses normal DIBs for the bitmap and WinG to blit them

********************************************************************************************/

class GRenderWinG : public GRenderRegion
{
	CC_DECLARE_DYNAMIC(GRenderWinG)

public:
	GRenderWinG(DocRect ClipRegion, Matrix ConvertMatrix, FIXED16 ViewScale, UINT32 Depth, double dpi);
	~GRenderWinG();

	static BOOL Init(BOOL);
	static void Deinit();
	static BOOL CanWinG( UINT32 Depth );

	// here are the function pointer types to the dynamically linked DLL
	// if wing.h ever changes, so will these. They are not Win16-clean, but as we never
	// use them in Win16 I don't care
	// these are public becuase other people might use them
	// (the stdcalls are required to use Pascal conventions)

	typedef HDC (PASCAL *WING_CREATEDC)( void );
	typedef BOOL (PASCAL *WING_RECOMMENDED)( BITMAPINFO FAR *pFormat );
	typedef HBITMAP (PASCAL *WING_CREATEBITMAP)( HDC WinGDC, BITMAPINFO const FAR *pHeader,
        void FAR *FAR *ppBits );
	typedef BOOL (PASCAL *WING_BITBLT)( HDC hdcDest, INT32 nXOriginDest,
        INT32 nYOriginDest, INT32 nWidthDest, INT32 nHeightDest, HDC hdcSrc,
        INT32 nXOriginSrc, INT32 nYOriginSrc );

	// and here's huw users can get to them. The names of the fn pointers are the same
	// as the real fns, except with a 'p' on the front. Don't use these fn pointers if
	// WinGDLL is NULL, as that would be bad

	static HINSTANCE WinGDLL;								// NULL if not loaded

	static WING_CREATEDC		pWinGCreateDC;
	static WING_RECOMMENDED		pWinGRecommendedDIBFormat;
	static WING_CREATEBITMAP	pWinGCreateBitmap;
	static WING_BITBLT			pWinGBitBlt;


private:
	LPBITMAPINFO GetLPBits( INT32 Width, INT32 Height, INT32 Depth, LPBYTE*);
	void FreeLPBits( LPBITMAPINFO, LPBYTE );
	BOOL DisplayBits(LPBITMAPINFO lpDisplayBitmapInfo = NULL, LPBYTE lpDisplayBits = NULL);
	BOOL StartRenderAfter(GMATRIX *GMat);

	// member variables
	HBITMAP WinGBitmap;
	BOOL Inverted;										// FALSE for old fashioned DIB,
														// TRUE for upside-down DIB

	// static variables
	static HDC OffScreenDC;								// a WinGDC really
};
