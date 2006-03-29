// $Id$
/////////////////////////////////////////////////
// the bitmap beveler (for inner bevels)
// DMc 2/6/98
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

#ifndef _BEVELER_H
#define _BEVELER_H

#include "pathtrap.h"
//#include "bevtrap.h"
//#include "wincoord.h"
//#include "doccoord.h"
//#include "bitmap.h"

#define BEVEL_FLATNESS  200

// macro to turn the system time into an INT32 value which can be used for timings
#define LONGSYSTEMTIME(X) { SYSTEMTIME s; \
						    GetSystemTime(&s); \
							X = s.wMilliseconds + s.wSecond * 1000 + \
								s.wMinute * 60000 + \
								s.wHour   * 60000 * 60 ; }

#define WRITETIME(X, Y) { FILE * kxp = fopen("timings.txt", "a"); \
					   fprintf(kxp, X); fprintf(kxp, " %d\n", Y); \
						  fclose(kxp); }

class NodeBevel;
struct BEVEL_FACE ;

#define BEVELBITMAPSCALE 4
#define BEVELBITMAPSHIFT 2

/******************************************************************************************

>	class CBeveler : public SimpleCCObject

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/10/98

	Purpose:	Does the bevelling

******************************************************************************************/

class CBeveler : public CCObject
{
	// Give my name out in memory dumps
	CC_DECLARE_DYNAMIC(CBeveler);

public:
	CBeveler();
	CBeveler(Path * pPath);

	~CBeveler();

	void SetBevelType(INT32 t) { m_BevelType = t; }

	BOOL SetUpBeveler(MILLIPOINT Indent, JointType jtype, NormCoord * lightVec, BOOL bOuter,
					  INT32 Contrast, RenderRegion * pRegion, MILLIPOINT Flatness);
/*
	BOOL SetUpBeveler(MILLIPOINT Indent, JointType jtype, NormCoord * lightVec, BOOL bOuter,
					INT32 width, INT32 height, INT32 Contrast, RenderRegion * pRegion,
					MILLIPOINT Flatness, double dpi, BOOL bFlat = TRUE);
*/
	BOOL SetBevelerSize(INT32 Width, INT32 Height);

	DocRect GetSelectionRect() { return m_SelectionBounds; }
	void SetBevelAngle(double LightAngle) { m_LightAngle = LightAngle; }
	void SetBevelTilt(double Tilt)	{	 m_Tilt = Tilt; }

	// gets the flatness to use depending on the document
	static INT32 GetBevelFlatness();

	// render the bevel to a given bitmap
	BOOL RenderToBitmap(KernelBitmap * pBitmap, BOOL bResample = FALSE, DocCoord* pSubPixOffSet = NULL);

private:
	MILLIPOINT m_Indent;
	INT32 m_Contrast;
	INT32 m_BevelType;
	double m_LightAngle;
	double m_Tilt;

	// pointer to the path to be used in bevelling
	Path * m_pBevelPath;
	POINT* m_pBevelPoints ;
	BYTE * m_pBevelTypes ;
	UINT32  m_nBevelLength ;
	BYTE m_Transparency;

	// the selection's bounding rect in document coordinates
	DocRect m_SelectionBounds;
	DocRect m_BevelBounds;

	// sets up the above two scale factors
	void CalcSelectionBounds() ;
	void CalcInnerPath() ;
	void SetUpMapping();

	// transforms a win coord to a doc coord based on currently active spread and view
	void ToWinCoord(const DocCoord * dc, WinCoord * wc);

	// the width and height of the bitmap that we are to return
	INT32 m_Width;
	INT32 m_Height;

	KernelBitmap* m_pStrip32;
	KernelBitmap* m_pMaskBitmap;
	KernelBitmap* m_pNewMask;

	JointType   m_JointType;
	BOOL m_bOuter;

	INT32 SetDIBitmap( BITMAPINFOHEADER* pHeader, BYTE* pBits ) ;

	BEVEL_FACE* m_pFaceList;
	UINT32 m_NumFaces;

	// resamples the bitmap given into destination
	void ResampleBitmap(BYTE * p32BitDIB, UINT32 Width, UINT32 Height, BYTE * pDestBits,
								BYTE * pMaskBitmap);

	BOOL EmboldenBitmap(KernelBitmap* pImgBitmap, KernelBitmap* pMaskBitmap, KernelBitmap* pNewMaskBitmap);

	double m_dBmpToWinX;
	double m_dBmpToWinY;

	// renders the points list, resampling
	BOOL RenderWithResampling(KernelBitmap * pBitmap, INT32 OffsetX, INT32 OffsetY, DocCoord* pSubPixOffSet = NULL);
	
	// copy bitmap fn
	void CopyBitmapIntoBitmap(const KernelBitmap * pSrcBitmap, KernelBitmap * pDestBitmap,
						INT32 DestX, INT32 DestY);

	BOOL RenderInStripsToBitmap(KernelBitmap * pBitmap, DocCoord* pSubPixOffSet = NULL);
} ;


#endif
