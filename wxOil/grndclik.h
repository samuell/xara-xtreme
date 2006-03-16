// $Id: grndclik.h 662 2006-03-14 21:31:49Z alex $
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

// header file for GRenderClick and GRenderClickColour classes

#ifndef INC_GRNDCLIK
#define	INC_GRNDCLIK

#include "grnddib.h"
#include "rendbits.h"

/********************************************************************************************

>	class GRenderClick : public GRenderDIB

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/11/94
	Purpose:	Special version of a GRenderRegion that renders into a mono bitmap. It
				is used for hit-detection only. It can also be asked if it is 'blank'.

********************************************************************************************/

class GRenderClick : public GRenderDIB
{
	friend class OSRenderBitmap;

	CC_DECLARE_DYNAMIC(GRenderClick)

public:
	GRenderClick(DocRect ClipRegion, Matrix ConvertMatrix, FIXED16 ViewScale, UINT32 Depth, double dpi);
	~GRenderClick();

	virtual void DebugMe(INT32 Stretch);
	virtual BOOL ImmediateRender();

	// banding functions
	virtual BOOL SetFirstBand();
	virtual BOOL GetNextBand();

protected:
	LPBITMAPINFO GetLPBits( INT32 Width, INT32 Height, INT32 Depth, LPBYTE*);
	void FreeLPBits( LPBITMAPINFO, LPBYTE );

protected:		// private:
	virtual BitmapContents GetContents( BOOL );
	BOOL DisplayBits(LPBITMAPINFO lpDisplayBitmapInfo = NULL, LPBYTE lpDisplayBits = NULL);
	virtual void SetClean(BOOL bResetChangedBounds, BOOL FillWhite);
	BOOL AttachDevice(View*, wxDC*, Spread* SpreadToAttach = NULL, bool fOwned = false );
	BOOL StartRender();

	// member variables
	LPBYTE pMiddle;							// pointer to byte of centre pixel
	BYTE MiddleMask;						// mask of centre pixel

	// Var to ask the bitmap to be cleared the next time Start Render is called
	BOOL ClearOnStart;
};

/********************************************************************************************

>	class GRenderClickColour : public GRenderClick

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/10/99
	Purpose:	Special version of a GRenderClick that renders into a colour bitmap. It
				is used for colour picker hit-detection/pixel colour detection only.

********************************************************************************************/

class GRenderClickColour : public GRenderClick
{
	friend class OSRenderBitmap;

	CC_DECLARE_DYNAMIC(GRenderClickColour)

public:
	GRenderClickColour(DocRect ClipRegion, Matrix ConvertMatrix, FIXED16 ViewScale, UINT32 Depth, double dpi);
	~GRenderClickColour();

	virtual void DebugMe(INT32 Stretch);

	Pixel32bpp GetCentrePixel32BPP();
	Pixel32bpp GetAveragePixel32BPP();

private:
	BOOL AttachDevice(View*, wxDC*, Spread* SpreadToAttach = NULL, bool fOwned = false );
	BOOL StartRender();

protected:
	virtual BitmapContents GetContents(BOOL JustMiddle);

};

/********************************************************************************************

>	class GRenderClickColourNoPaper : public GRenderClickColour

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/03/2004
	Purpose:	Special version of a GRenderClick that renders into a colour bitmap. It
				is used for hit-detection.
				The only difference between this and GRenderClickColour is that when
				it attaches to a CCDC that CCDC's NeedsPaper function will return FALSE

********************************************************************************************/

class GRenderClickColourNoPaper : public GRenderClickColour
{
	friend class OSRenderBitmap;

	CC_DECLARE_DYNAMIC(GRenderClickColourNoPaper)

public:
	GRenderClickColourNoPaper(DocRect ClipRegion, Matrix ConvertMatrix, FIXED16 ViewScale, UINT32 Depth, double dpi);
	~GRenderClickColourNoPaper();

private:
	BOOL AttachDevice(View*, wxDC*, Spread* SpreadToAttach = NULL, bool fOwned = false );
};

#endif
