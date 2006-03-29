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

// Device context used for printing to PostScript devices.

#ifndef INC_PSDC
#define INC_PSDC

#include "kerneldc.h"
#include "wincoord.h"

// Size of buffer used to xfer PostScript data to device.
#define MAX_PSBUF (512)

class PrintPSRenderRegion;

/********************************************************************************************

>	class PassThruBuffer

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	04/24/95
	Purpose:	A simple structure for passing data to PostScript devices under Windows.
				It holds the number of bytes in the buffer followed by the data.
				(See SDK/MFC docs for Escape() function)
	SeeAlso:	PSPrintDC

********************************************************************************************/

class PassThruBuffer
{
public:
	short nCount;
	char  Data[MAX_PSBUF + 1];
};

// Used for rendering text via GDI
// (It is a simple cache system).
class PSDCFontInfo
{
public:
	PSDCFontInfo();

public:
	WORD Typeface;
	BOOL Bold;
	BOOL Italic;
	MILLIPOINT Width;
	MILLIPOINT Height;
	ANGLE Rotation;
	CFont *pRenderFont;
	CFont *pOldFont;
};

/********************************************************************************************

>	class PSPrintDC : public KernelDC

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/03/94
	Purpose:	Provide a device context for printing to a PostScript printer.
	SeeAlso:	KernelDC; PSPrintRenderRegion

********************************************************************************************/

class PSPrintDC : public KernelDC
{
public:
	PSPrintDC(CDC *pDC);
	~PSPrintDC();

	// Functions to do the actual output to the PostScript printer.
	virtual BOOL OutputNewLine();
	virtual BOOL OutputToken(TCHAR*);
	virtual BOOL OutputDirect(BYTE *, INT32);

	// Handles the change of co-ordinate system that GDI introduces to PS devices.
	virtual BOOL OutputCoord(DocCoord&, EPSAccuracy Accuracy = ACCURACY_NORMAL);
	virtual BOOL OutputUserSpaceValue(MILLIPOINT, EPSAccuracy Accuracy = ACCURACY_NORMAL);

	void SetDCTransforms(Matrix RenderMatrix, View *pView);
	WinCoord TransformCoord(const DocCoord&);

	void AttachRenderRegion(PrintPSRenderRegion *pRegion)
		{	pPSRegion = pRegion;   }
	void DetachRenderRegion() { pPSRegion = NULL; }

	// Function to call when using OS functions to render.
	BOOL StartOSOutput();
	BOOL EndOSOutput();

	// Function to call when printing is finished
	BOOL FlushDC();

	// Flushes pending PostScript output to device.
	BOOL FlushPSBuffer();

	BOOL SelectNewFont(WORD Typeface, BOOL Bold, BOOL Italic, 
					   MILLIPOINT Width, MILLIPOINT Height, ANGLE Rotation);

	// Functions to setup the use of a Postscript clipping rectangle
	BOOL SetClipping(BOOL NewState);
	BOOL OutputPSClipping();

protected:

	BOOL MakeRoomInBuffer(INT32);

	// Used for pending output.
	PassThruBuffer Buffer;

	// Escape code for passing data through depends on platform.
	WORD PassThruEscape;

	// The transforms to use when outputting user space values.
	// The matrix is used for co-ordinates; the FIXED16 is used to convert distances
	// (e.g. linewidths).
	Matrix RenderMatrix;
	MILLIPOINT PixelSize;
	View *pView;

	// If zero, then it is not safe to do GDI output; if non-zero then it is.
	INT32 SafeToUseGDI;

	// Initialised to true on contruction of this DC. It will be set to FALSE as
	// soon as we attempt to output a token. We can use this to control once only
	// startofpage output data such as DSC comments.
	BOOL StartOfPage;

	// If TRUE, then the current PS context is the one set up by GDI, otherwise it is the
	// context used by Camelot.
	BOOL GDIContextIsCurrent;

	// Specifies whether to output the current clipping region to Postscript in order to
	// clip our PostScript graphics output. This defaults to TRUE.
	BOOL UsePSLevelClipping;

	// Used to cache GDI fonts.
	PSDCFontInfo FontInfo;

	// Used for outputing postscript header.
	PrintPSRenderRegion *pPSRegion;
};



#endif  // INC_PSDC


