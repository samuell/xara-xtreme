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

// Device Contexts that are kernel-safe.

#ifndef INC_KERNELDC
#define INC_KERNELDC

#include "ccdc.h"
//#include "ccfile.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "doccolor.h" - in camtypes.h [AUTOMATICALLY REMOVED]


// Used to control the output of 'user space values' (See KernelDC::OutputUserSpaceValue).
typedef enum
{
	ACCURACY_NORMAL,	// Output to 2dp
	ACCURACY_ROUNDUP,	// Output to 0dp, rounding up
	ACCURACY_ROUNDDOWN  // Output to 0dp, rounding down
} EPSAccuracy;

class Filter;


/********************************************************************************************

>	class KernelDC : public CwxDC

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/04/95
	Purpose:	Provide a base device context for rendering to something other than the
				screen.  This includes printers, files, and PostScript devices.

				The main purpose of this class is to provide a repository for our bespoke
				PostScript procedures.  This base class cannot use them as the centralised
				output routines are unimplemented and will error if called.  Derived
				classes such as EPSExportDC and PrintPSDC will override these output
				functions, so the PostScript output routines can be used.  This class
				provides basic primitives for writing out EPS/PS tokens such as commands,
				colours, and user space values (line-widths, co-ordinates etc).
				The DC takes care of handling formatting such as separating tokens with
				a space or newline, and limits line widths to a reasonable size.

	SeeAlso:	EPSExportDC; ExportDC; PrintPSDC

********************************************************************************************/

class KernelDC : public CCDummyDC
{
	CC_DECLARE_DYNAMIC(KernelDC)
public:
	KernelDC(RenderType Type);
	KernelDC(CNativeDC * pDC, RenderType Type);

public:
	// Functions to output in PostScript format.

	// These three functions must be over-ridden in base classes in order that the other
	// OutputXXX() functions can actually be used.  (The base class versions of these three
	// functions will error if called).
	virtual BOOL OutputNewLine();
	virtual BOOL OutputToken(TCHAR*);
	virtual BOOL OutputDirect(BYTE *, INT32);
	virtual BOOL OutputTCHARAsChar(TCHAR *Buf, INT32 nBytes);

	void SetOrigin(DocCoord&);
	DocCoord GetOrigin() { return Origin; }
	virtual BOOL OutputCoord(DocCoord&, EPSAccuracy Accuracy = ACCURACY_NORMAL);
	virtual BOOL OutputUserSpaceValue(MILLIPOINT, EPSAccuracy Accuracy = ACCURACY_NORMAL);
	BOOL OutputColour(PColourCMYK*);
	BOOL OutputNamedColour(DocColour*, ColourContext* pContext=NULL);
	BOOL OutputColourValue(UINT32 n);
	BOOL OutputString(TCHAR *);
	BOOL OutputValue(INT32);
	BOOL OutputValue(UINT32);
	BOOL OutputReal(double);
	BOOL OutputFloat(const double Value, const UINT32 DecPl);
	BOOL OutputMatrix(Matrix * M);
	BOOL OutputArray(INT32*, INT32);

	// (Chris G 29/11/00) Function to Output a colour's name ONLY.
	BOOL OutputColourName (DocColour* pCol);

	virtual INT32 OutputRawBinary(BYTE *Data, UINT32 Length, UINT32 Alignment = 1);

	BOOL StartASCII85Output(BOOL RunLengthEncode = FALSE);
	INT32 OutputASCII85(BYTE *Data, UINT32 Length);
	INT32 EndASCII85Output();

	void SetFullAccuracy(BOOL);

	virtual Filter* GetParentFilter() { return NULL; }

protected:
	void ConvertToHex(BYTE *Data, UINT32 Length, TCHAR *Buf);

	BYTE *RunLengthEncode(BYTE *Data, INT32 *Length);
	BYTE *RunLengthDecode(BYTE *Data, INT32 *Length);

	// Routines for ASCII85 encoding.
	BOOL QueueASCII85Data(BYTE *Data, UINT32 Length);
	INT32 FlushASCII85Buffer();
	INT32 ConvertToASCII85(BYTE *Src, UINT32 Length, BYTE *Dest);
	INT32 ConvertFromASCII85(BYTE *Src, UINT32 Length, BYTE *Dest);

	// The width of the line currently being output (to control line wrap).
	UINT32 LineWidth;

	// The origin (as set by SetOrigin) of the EPS file.
	DocCoord Origin;

	// Whether to output user space values as 2dp or 3dp.
	BOOL FullAccuracy;

	// Converts millipoints to an ASCII representation of the value, in points.
	void ConvertToPoints(MILLIPOINT, TCHAR *);

	// Buffer for converting to ASCII85.
	LPBYTE RawBuf, A85Buf;
	UINT32 RawBufSize;
	BOOL RLEtheASCII85Data;
};


/********************************************************************************************

>	class ExportDC : public CwxDC

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/03/94
	Purpose:	Provide a device context for rendering to a file rather than the screen.
				This base class handles opening the file.
	SeeAlso:	EPSExportDC

********************************************************************************************/

class ExportDC : public KernelDC
{
public:
	ExportDC(Filter *Parent);
	~ExportDC();

	BOOL Init(CCLexFile*);
	virtual Filter* GetParentFilter();

	CCLexFile* ExportFile;

protected:
	Filter* ParentFilter;
};


#endif  // INC_KERNELDC
