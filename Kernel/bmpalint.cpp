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
// Bitmap export palette interface class that provides an interface
// between the real palette and the sorted version of the palette

// All references to 'the palette' in this file refer to the sorted palette
// that this class represents.  All references to the palette that is used
// for exporting a bitmap will state they are using the 'real palette'.

#include "camtypes.h"
#include "bmpalint.h"
#include "bmpprefs.h"	// For ExtendedPalette
#include "bmapprev.h"	// For BmapPrevDlg

CC_IMPLEMENT_MEMDUMP(BitmapExportPaletteInterface, CC_CLASS_MEMDUMP)

#define new CAM_DEBUG_NEW

bool BitmapExportPaletteInterface::m_SortedPaletteValid;

BitmapExportPaletteInterface::BitmapExportPaletteInterface()
{
	// Make sure the sorted palette is generated before it is used
	InvalidateSortedPalette();

	m_CurrentSortType = SORT_USE;	// Default sort type
}

/********************************************************************************************
>	INT32	BitmapExportPaletteInterface::SortedValueToRealValue(INT32 index)
	Author:		Jonathan_Payne (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/12/2000
	Inputs:		An index in the sorted palette (eg from the palette control)
	Returns:	An index in the real palette (eg that used for bitmap export)
	Purpose:	To convert between the sorted and real palettes
********************************************************************************************/
INT32	BitmapExportPaletteInterface::SortedValueToRealValue(INT32 index)
{
	if (!m_SortedPaletteValid) ValidateSortedPalette();
	if (index == BitmapExportPaletteControl::INVALID_COLOUR_VALUE)
		return BitmapExportPaletteControl::INVALID_COLOUR_VALUE;
	else
		return m_PaletteSortedToReal[index];
}

/********************************************************************************************
>	INT32	BitmapExportPaletteInterface::RealValueToSortedValue(INT32 index)
	Author:		Jonathan_Payne (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/12/2000
	Inputs:		An index in the real palette (eg that used for bitmap export)
	Returns:	An index in the sorted palette (eg from the palette control)
	Purpose:	To convert between the real and sorted palettes
********************************************************************************************/
INT32	BitmapExportPaletteInterface::RealValueToSortedValue(INT32 index)
{
	if (!m_SortedPaletteValid) ValidateSortedPalette();
	if (index == BitmapExportPaletteControl::INVALID_COLOUR_VALUE)
		return BitmapExportPaletteControl::INVALID_COLOUR_VALUE;
	else
		return m_PaletteSortedToReal[index];
}

void BitmapExportPaletteInterface::InvalidateSortedPalette()
{
	m_SortedPaletteValid = false;
}

INT32 BitmapExportPaletteInterface::GetNumberOfColours()
{
	if (BmapPrevDlg::m_pExportOptions->GetSupportsPalette())
	{
		ExtendedPalette	*palette = BmapPrevDlg::m_pExportOptions->GetExtendedPalette();
		ERROR3IF(!palette, "There is no palette - This should never happen");
		return palette->NumberOfColours;
	}
	else
		return 0;
}

BYTE BitmapExportPaletteInterface::GetRed(INT32 index)
{
	if (!m_SortedPaletteValid) ValidateSortedPalette();

	ERROR3IF(index == -1, "Function called with an invalid palette index");

	ExtendedPalette	*palette = BmapPrevDlg::m_pExportOptions->GetExtendedPalette();
	ERROR3IF(!palette, "There is no palette - This should never happen");
	return palette->Data[m_PaletteSortedToReal[index]].Red;
}

BYTE BitmapExportPaletteInterface::GetGreen(INT32 index)
{
	if (!m_SortedPaletteValid) ValidateSortedPalette();

	ERROR3IF(index == -1, "Function called with an invalid palette index");

	ExtendedPalette	*palette = BmapPrevDlg::m_pExportOptions->GetExtendedPalette();
	ERROR3IF(!palette, "There is no palette - This should never happen");
	return palette->Data[m_PaletteSortedToReal[index]].Green;
}

BYTE BitmapExportPaletteInterface::GetBlue(INT32 index)
{
	if (!m_SortedPaletteValid) ValidateSortedPalette();

	ERROR3IF(index == -1, "Function called with an invalid palette index");

	ExtendedPalette	*palette = BmapPrevDlg::m_pExportOptions->GetExtendedPalette();
	ERROR3IF(!palette, "There is no palette - This should never happen");
	return palette->Data[m_PaletteSortedToReal[index]].Blue;
}

INT32 BitmapExportPaletteInterface::GetFlags(INT32 index)
{
	if (!m_SortedPaletteValid) ValidateSortedPalette();

	ERROR3IF(index == -1, "Function called with an invalid palette index");

	ExtendedPalette	*palette = BmapPrevDlg::m_pExportOptions->GetExtendedPalette();
	ERROR3IF(!palette, "There is no palette - This should never happen");
	return palette->Data[m_PaletteSortedToReal[index]].Flags;
}

void BitmapExportPaletteInterface::SetRed(INT32 index, BYTE red)
{
	if (!m_SortedPaletteValid) ValidateSortedPalette();

	ERROR3IF(index == -1, "Function called with an invalid palette index");

	ExtendedPalette	*palette = BmapPrevDlg::m_pExportOptions->GetExtendedPalette();
	ERROR3IF(!palette, "There is no palette - This should never happen");
	palette->Data[m_PaletteSortedToReal[index]].Red = red;
	palette->Data[m_PaletteSortedToReal[index]].Flags &= LOCKED_COLOUR;
}

void BitmapExportPaletteInterface::SetGreen(INT32 index, BYTE green)
{
	if (!m_SortedPaletteValid) ValidateSortedPalette();

	ERROR3IF(index == -1, "Function called with an invalid palette index");

	ExtendedPalette	*palette = BmapPrevDlg::m_pExportOptions->GetExtendedPalette();
	ERROR3IF(!palette, "There is no palette - This should never happen");
	palette->Data[m_PaletteSortedToReal[index]].Green = green;
	palette->Data[m_PaletteSortedToReal[index]].Flags &= LOCKED_COLOUR;
}

void BitmapExportPaletteInterface::SetBlue(INT32 index, BYTE blue)
{
	if (!m_SortedPaletteValid) ValidateSortedPalette();

	ERROR3IF(index == -1, "Function called with an invalid palette index");

	ExtendedPalette	*palette = BmapPrevDlg::m_pExportOptions->GetExtendedPalette();
	ERROR3IF(!palette, "There is no palette - This should never happen");
	palette->Data[m_PaletteSortedToReal[index]].Blue = blue;
	palette->Data[m_PaletteSortedToReal[index]].Flags &= LOCKED_COLOUR;
}

void BitmapExportPaletteInterface::SetFlags(INT32 index, INT32 flags)
{
	if (!m_SortedPaletteValid) ValidateSortedPalette();

	ERROR3IF(index == -1, "Function called with an invalid palette index");

	ExtendedPalette	*palette = BmapPrevDlg::m_pExportOptions->GetExtendedPalette();
	ERROR3IF(!palette, "There is no palette - This should never happen");
	palette->Data[m_PaletteSortedToReal[index]].Flags = flags;
}

void BitmapExportPaletteInterface::ValidateSortedPalette()
{
	// Initalize the palette to not sorted - This is done what ever the sort type
	// so all the colours are in the palette.  A possible optimisation would be to
	// only do this step on first use and when the number of colours in the palette
	// changes
	INT32 i;

	for (i = 0; i < GetNumberOfColours(); ++i)
		m_PaletteSortedToReal[i] = i;

	switch (m_CurrentSortType)
	{
		case SORT_USE:
		case SORT_NONE:
		{
			// The default palette order (as it comes from Gavin/SimonK code) is by
			// use so there is nothing to do in this case (the for loops above and
			// below this switch sort out the palette indexes).
			break;
		}
		case SORT_HUE:
		{
			qsort(m_PaletteSortedToReal, GetNumberOfColours(), sizeof(INT32),
				(INT32 (*)(const void*,const void*))HueComparisonFn);
			break;
		}

		case SORT_LUMINANCE:
		{
			qsort(m_PaletteSortedToReal, GetNumberOfColours(), sizeof(INT32),
				(INT32 (*)(const void*,const void*))LuminanceComparisonFn);
			break;
		}
		default:
		{
			// Don't use ERROR3's here as they upset the rendering code
			TRACE( _T("Unknown sort type used\n"));
			break;
		}
	}

	// Copy the changes into the other array
	for (i = 0; i < GetNumberOfColours(); ++i)
		m_PaletteRealToSorted[m_PaletteSortedToReal[i]] = i;

	m_SortedPaletteValid = true;
}

void BitmapExportPaletteInterface::SetPaletteSortType(PaletteSortType newSortType)
{
	m_CurrentSortType = newSortType;
	InvalidateSortedPalette();		// make sure palette is regenerated on next use
}

BitmapExportPaletteInterface::PaletteSortType BitmapExportPaletteInterface::GetPaletteSortType()
{
	return m_CurrentSortType;
}

/******************************************************************************************
>	INT32 BitmapExportPaletteInterface::LuminanceComparisonFn(const INT32 *arg1, const INT32 *arg2)
	Author:		Jonathan_Payne (Xara Group Ltd) <camelotdev@xara.com> (based on code by Alex Price)
	Created:	30/11/2000
	Inputs:		arg1	Pointer to a value in the m_PaletteSortedToReal array ie an index in
						the real palette
				arg2	Pointer to a value in the m_PaletteSortedToReal array ie an index in
						the real palette
	Purpose:	For use with qsort'ing the palette by Luminance
******************************************************************************************/
INT32 BitmapExportPaletteInterface::LuminanceComparisonFn(const INT32 *arg1, const INT32 *arg2)
{
	INT32 paletteIndex1	= *arg1;
	INT32 paletteIndex2	= *arg2;

	ExtendedPalette	*palette = BmapPrevDlg::m_pExportOptions->GetExtendedPalette();
	ERROR3IF(!palette, "There is no palette - This should never happen");

	//  Extract the red, green, and blue values for the first index
	INT32 red1	= palette->Data[paletteIndex1].Red;
	INT32 green1	= palette->Data[paletteIndex1].Green;
	INT32 blue1	= palette->Data[paletteIndex1].Blue;

	//  Extract the red, green, and blue values for the first index
	INT32 red2	= palette->Data[paletteIndex2].Red;
	INT32 green2	= palette->Data[paletteIndex2].Green;
	INT32 blue2	= palette->Data[paletteIndex2].Blue;

	//  Get the Luminance value corresponding to the first RGB triplet.
	double luminance1 = ( 0.299 * red1 ) + ( 0.587 * green1 ) + ( 0.114 * blue1 );

	//  Get the Luminance value corresponding to the second RGB triplet.
	double luminance2 = ( 0.299 * red2 ) + ( 0.587 * green2 ) + ( 0.114 * blue1 );

	//  This will sort in descending Hue values.
	if		(luminance1 < luminance2)
		return 1;
	else if (luminance1 > luminance2)
		return -1;

	ERROR3IF(luminance1 != luminance2, "This is not possible!");

	if		(red1 < red2 )
		return 1;
	else if (red1 > red2)
		return -1;

	ERROR3IF(red1 != red2, "This is not possible!");

	if		(green1 < green2 )
		return 1;
	else if (green1 > green2)
		return -1;

	ERROR3IF(green1 != green2, "This is not possible!");

	if		(blue1 < blue2 )
		return 1;
	else if (blue1 > blue2)
		return -1;

	ERROR3IF(blue1 != blue2, "This is not possible!");

	return 0;	// The red, green and blue are equal	
}

/******************************************************************************************
>	INT32 BitmapExportPaletteInterface::HueComparisonFn(const INT32 *arg1, const INT32 *arg2)
	Author:		Jonathan_Payne (Xara Group Ltd) <camelotdev@xara.com> (based on code by Alex Price)
	Created:	30/11/2000
	Inputs:		arg1	Pointer to a value in the m_PaletteSortedToReal array ie an index in
						the real palette
				arg2	Pointer to a value in the m_PaletteSortedToReal array ie an index in
						the real palette
	Purpose:	For use with qsort'ing the palette by hue
******************************************************************************************/
INT32 BitmapExportPaletteInterface::HueComparisonFn(const INT32 *arg1, const INT32 *arg2)
{
	INT32 paletteIndex1	= *arg1;
	INT32 paletteIndex2	= *arg2;

	ExtendedPalette	*palette = BmapPrevDlg::m_pExportOptions->GetExtendedPalette();
	ERROR3IF(!palette, "There is no palette - This should never happen");

	//  Extract the red, green, and blue values for the first index
	INT32 red1	= palette->Data[paletteIndex1].Red;
	INT32 green1	= palette->Data[paletteIndex1].Green;
	INT32 blue1	= palette->Data[paletteIndex1].Blue;

	//  Extract the red, green, and blue values for the first index
	INT32 red2	= palette->Data[paletteIndex2].Red;
	INT32 green2	= palette->Data[paletteIndex2].Green;
	INT32 blue2	= palette->Data[paletteIndex2].Blue;

	//  Get the Hue value corresponding to the first RGB triplet.
	DocColour colour1(red1, green1, blue1);
	INT32 hue1, saturation1, value1;
	colour1.GetHSVValue(&hue1, &saturation1, &value1);

	//  Get the Hue value corresponding to the first RGB triplet.
	DocColour colour2(red2, green2, blue2);
	INT32 hue2, saturation2, value2;
	colour2.GetHSVValue(&hue2, &saturation2, &value2);

	if		(hue1 > hue2)
		return 1;
	else if	(hue1 < hue2)
		return -1;

	ERROR3IF(hue1 != hue2, "This is not possible!");

	//  If the Hue values are the same, then put in order of Saturation
	if		(saturation1 < saturation2)
		return 1;
	else if (saturation1 > saturation2)
		return -1;

	ERROR3IF(saturation1 != saturation2, "This is not possible!");

	//  If the saturation values are the same, then the final step is
	//  to put them in order of Value
	if		(value1 < value2)
		return 1;
	else if	(value1 > value2)
		return -1;

	ERROR3IF(value1 != value2, "This is not possible!");

	
	return 0;	// Everything is eqaul
}
