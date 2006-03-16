// $Id: bmpalint.h 662 2006-03-14 21:31:49Z alex $
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
// Bitmap export palette interface class that provides an interface
// between the real palette and the sorted version of the palette

#ifndef INC_BMPALINT
#define INC_BMPALINT

/**************************************************************************************
>	class BitmapExportPaletteInterface : public CC_CLASS_MEMDUMP
	Author:		Jonathan_Payne (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/11/2000
	Purpose:	Provides an interface between the real palette and the sorted version
				of the palette
**************************************************************************************/
class BitmapExportPaletteInterface : public CC_CLASS_MEMDUMP
{
	// Declare the class for memory tracking
	CC_DECLARE_MEMDUMP(BitmapExportPaletteInterface);

private:	// Private constants
	enum {
		MAX_PALETTE_ENTRYS		= 256
	};

public:		// Public constants
	enum PaletteSortType{SORT_NONE, SORT_USE, SORT_LUMINANCE, SORT_HUE};

private:	// Data
	// Array index == value in real palette, value == value in sorted palette
	INT32 m_PaletteRealToSorted[MAX_PALETTE_ENTRYS];

	// Array index == value in sorted palette, value == value in real palette
	INT32 m_PaletteSortedToReal[MAX_PALETTE_ENTRYS];

	PaletteSortType m_CurrentSortType;

private:	// Statics
	// If this is false, we need to regenerate the arrays before using them again
	static bool m_SortedPaletteValid;

public:		// Constructors
	BitmapExportPaletteInterface();

public:		// Interface
	INT32		GetNumberOfColours();

	void			SetPaletteSortType(PaletteSortType newSortType);
	PaletteSortType	GetPaletteSortType();

	BYTE	GetRed	(INT32 index);
	BYTE	GetGreen(INT32 index);
	BYTE	GetBlue	(INT32 index);
	INT32		GetFlags(INT32 index);

	void	SetRed	(INT32 index, BYTE red);
	void	SetGreen(INT32 index, BYTE green);
	void	SetBlue	(INT32 index, BYTE blue);
	void	SetFlags(INT32 index, INT32 flags);

	// Function to convert from a index in the sorted palette to an index in the real palette
	INT32		SortedValueToRealValue(INT32 index);
	INT32		RealValueToSortedValue(INT32 index);

public:		// Static interface
	static void InvalidateSortedPalette();

private:	// Implementation
	void ValidateSortedPalette();

private:	// Static functions (so that they can be passed to qsort)
	static INT32 LuminanceComparisonFn(const INT32 *arg1, const INT32 *arg2);
	static INT32 HueComparisonFn(const INT32 *arg1, const INT32 *arg2);
};

#endif	// INC_BMPALINT
