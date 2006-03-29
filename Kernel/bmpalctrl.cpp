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
// Palette control for use in the export/create bitmap copy dialog

// Implementation notes:  The palette contol displays the palette in the
// bitmap export dialog.  This palette has various sort options so the user
// can view the palette in diffrent ways.  To avoid having to change the actual
// bitmap when the user changes the sort type, this controls comunicates with
// the actual palette throught an interface class, BitmapExportPaletteInterface,
// which sits between the palette from the bitmap and the palette that this
// control uses (throught the member variable m_Palette).  Because of this,
// when the palette is comunicating with other parts of camelot, it must take
// into account this sorting and provide indexs into the palette in terms that
// the caller expects (ie sorted or non sorted).

#include "camtypes.h"
#include "bmpalctrl.h"

#include "filedlgs.h"		// required CreateOSRenderRegion
#include "dlgcol.h"			// required for DialogColours
//#include "bmpreres.h"		// for _R(IDC_T2_PALETTE_CONTROL)
#include "bmpalint.h"		// bitmap palette interface class
#include "bmpprefs.h"		// for colour type bit fields
#include "bmapprev.h"		// for BmapPrevDlg (used in web safe code)
#include "grndrgn.h"		// for GRenderRegion::GetDefaultDPI()
#include "palmenu.h"		// for palette's context senesitve menu

CC_IMPLEMENT_MEMDUMP(BitmapExportPaletteControl, CC_CLASS_MEMDUMP)

#define new CAM_DEBUG_NEW

BitmapExportPaletteControl::BitmapExportPaletteControl()
 : m_MouseOverCell(INVALID_COLOUR_VALUE /* Set to none */)
 , m_SelectedCell(INVALID_COLOUR_VALUE /* Set to none */)
 , m_NumberOfColoursAtLastRedraw(0)
 , m_WindowID(0)
 , m_nPixelSize(72000 / GRenderRegion::GetDefaultDPI())
 , m_nCellWidthPixels(11)
 , m_nCellHeightPixels(11)
 , m_nCellWidth(m_nPixelSize * m_nCellHeightPixels)
 , m_nCellHeight(m_nPixelSize * m_nCellHeightPixels)
 , m_nCellsPerLine(30)
{
}

BitmapExportPaletteControl::~BitmapExportPaletteControl()
{
}

/********************************************************************************************
>	void BitmapExportPaletteControl::SetCurrentSortType(BitmapExportPaletteInterface::PaletteSortType newSortType)
	Author:		Jonathan_Payne (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/12/2000
	Purpose:	Sets the palette control to use a new sort method (hue, luminance etc) - 
				changes will be visible the next time the palette is drawn
********************************************************************************************/
void BitmapExportPaletteControl::SetCurrentSortType(BitmapExportPaletteInterface::PaletteSortType newSortType)
{
	m_Palette.SetPaletteSortType(newSortType);
}

/********************************************************************************************
>	BitmapExportPaletteInterface::PaletteSortType BitmapExportPaletteControl::GetCurrentSortType()
	Author:		Jonathan_Payne (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/12/2000
	Purpose:	Returns the current sorting order of the palette control
********************************************************************************************/
BitmapExportPaletteInterface::PaletteSortType BitmapExportPaletteControl::GetCurrentSortType()
{
	return m_Palette.GetPaletteSortType();
}

/********************************************************************************************
>	INT32 BitmapExportPaletteControl::GetCellFromPos(INT32 x, INT32 y)
	Author:		Jonathan_Payne (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/12/2000
	Inputs:		x and y coords in DocCoords (not win coords)
	Purpose:	Find the cell from a coord
********************************************************************************************/
INT32 BitmapExportPaletteControl::GetCellFromPos(INT32 x, INT32 y)
{
	INT32 cellX = x / m_nCellWidthPixels;		// zero based

	INT32 cellY = y / m_nCellHeightPixels;	// zero based

	INT32 cell = (m_nCellsPerLine * cellY) + cellX;

	if (cell >= m_Palette.GetNumberOfColours() || cellX >= m_nCellsPerLine)
		return INVALID_COLOUR_VALUE;
	else
		return cell;
}

void BitmapExportPaletteControl::MsgMouseMove(ReDrawInfoType *pInfo)
{
	INT32 cell = GetCellFromPos(pInfo->pMousePos->x / m_nPixelSize, (pInfo->dy - pInfo->pMousePos->y) / m_nPixelSize);

	SetHighlightedCellFromSortedIndex(pInfo, cell);
}

/********************************************************************************************
>	bool BitmapExportPaletteControl::MsgMouseLeftButtonDown(ReDrawInfoType *pInfo)
	Author:		Jonathan_Payne (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/12/2000
	Inputs:		pInfo struct passed to parent dlg as part of the mouse message
	Returns:	true if the message resulted in a change to the selection else false
	Purpose:	Processes a mouse message for the palette control
********************************************************************************************/
bool BitmapExportPaletteControl::MsgMouseLeftButtonDown(ReDrawInfoType *pInfo)
{
	ERROR3IF(!m_WindowID, "Window ID not set");

	INT32 cell = GetCellFromPos(pInfo->pMousePos->x / m_nPixelSize, (pInfo->dy - pInfo->pMousePos->y) / m_nPixelSize);

	return SetSelectedCellFromSortedIndex(pInfo, cell);
}

/********************************************************************************************
>	bool BitmapExportPaletteControl::MsgMouseRightButtonUp()
	Author:		Jonathan_Payne (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/12/2000
	Purpose:	Show palette's context sensitive menu
********************************************************************************************/
void BitmapExportPaletteControl::MsgMouseRightButtonUp(ReDrawInfoType *pInfo, BmapPrevDlg *pBmapPrevDlg)
{
	MsgMouseLeftButtonDown(pInfo);

	if (m_SelectedCell == INVALID_COLOUR_VALUE)
		return;

	//  User has right-clicked on the palette control. So, we have
	//  to construct and display the right-click palette menu.
	OpPalettePopupCommand::Init(this, pBmapPrevDlg);

	PaletteContextMenu* Menu = new PaletteContextMenu;

	//  If everything is OK, then show the menu.
	if( Menu != NULL )
		Menu->Show();
	else
		ERROR3( "Can't create PaletteContextMenu" );
}

/********************************************************************************************
>	void BitmapExportPaletteControl::RedrawSelectedCell(ReDrawInfoType *pInfo)
	Author:		Jonathan_Payne (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/12/2000
	Inputs:		pInfo struct
	Purpose:	Redraws the selected cell, to be used after SetSelectedCell[Toggle]*()
				functions
********************************************************************************************/
void BitmapExportPaletteControl::RedrawSelectedCell(ReDrawInfoType *pInfo)
{
	InvalidateCell(pInfo, m_SelectedCell);
}

void BitmapExportPaletteControl::InvalidateCell(ReDrawInfoType *pInfo, INT32 paletteIndex)
{
	DocRect cellRect;
	ERROR3IF(!m_WindowID, "Window ID not set");

	GetRectForCell(paletteIndex, &cellRect, pInfo->dy);

	// Add the bodge factor
	cellRect.loy	-= m_nPixelSize;
	cellRect.hiy	+= m_nPixelSize;
	cellRect.hix	+= m_nPixelSize * 2;

	DialogManager::InvalidateGadget(m_WindowID, _R(IDC_T2_PALETTE_CONTROL), pInfo, &cellRect);
}

/********************************************************************************************
>	bool BitmapExportPaletteControl::SetSelectedCell(ReDrawInfoType *pInfo, INT32 paletteIndex)
	Author:		Jonathan_Payne (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/12/2000
	Inputs:		pInfo			- ReDrawInfoType struct
				paletteIndex	- an index into the real palette
	Purpose:	Sets the selected cell in the palette
********************************************************************************************/
bool BitmapExportPaletteControl::SetSelectedCell(ReDrawInfoType *pInfo, INT32 paletteIndex)
{
	return SetSelectedCellFromSortedIndex(pInfo, m_Palette.RealValueToSortedValue(paletteIndex));
}

/********************************************************************************************
>	bool BitmapExportPaletteControl::SetSelectedCellFromSortedIndex(ReDrawInfoType *pInfo, INT32 paletteIndex)
	Author:		Jonathan_Payne (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/12/2000
	Author:		Jonathan_Payne (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/12/2000
	Inputs:		pInfo			- ReDrawInfoType struct
				paletteIndex	- an index into the sorted palette
	Purpose:	Sets the selected cell in the palette
********************************************************************************************/
bool BitmapExportPaletteControl::SetSelectedCellFromSortedIndex(ReDrawInfoType *pInfo, INT32 paletteIndex)
{
	if (paletteIndex == m_SelectedCell)
		return false;

	// Invalidate the old cell
	if (m_SelectedCell != INVALID_COLOUR_VALUE)
		InvalidateCell(pInfo, m_SelectedCell);

	m_SelectedCell = paletteIndex;

	// See if the mouse is in a valid cell
	if (m_SelectedCell == INVALID_COLOUR_VALUE)
		return true;

	// Invalidate the new cell so it will be redrawn
	InvalidateCell(pInfo, m_SelectedCell);

	return true;
}

bool BitmapExportPaletteControl::SetHighlightedCell(ReDrawInfoType *pInfo, INT32 paletteIndex)
{
	return SetHighlightedCellFromSortedIndex(pInfo, m_Palette.RealValueToSortedValue(paletteIndex));
}

bool BitmapExportPaletteControl::SetHighlightedCellFromSortedIndex(ReDrawInfoType *pInfo, INT32 paletteIndex)
{
	if (paletteIndex == m_MouseOverCell)
		return false;

	// Invalidate the old cell
	if (m_MouseOverCell != INVALID_COLOUR_VALUE)
		InvalidateCell(pInfo, m_MouseOverCell);

	m_MouseOverCell = paletteIndex;

	// See if the mouse is in a valid cell
	if (m_MouseOverCell == INVALID_COLOUR_VALUE)
		return true;

	// Invalidate the new cell so it will be redrawn
	InvalidateCell(pInfo, m_MouseOverCell);

	return true;
}

/********************************************************************************************
>	void BitmapExportPaletteControl::RenderSoon()
	Author:		Jonathan_Payne (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/12/2000
	Purpose:	This function redraws the palette control by invaldating it so it is drawn
				latter.  This is useful for functions that can't get a DC in the pInfo
				struct.
********************************************************************************************/
void BitmapExportPaletteControl::RenderSoon()
{
	// Check the selection and mouse over colour are valid as the palette has probably changed
	if (m_SelectedCell > m_Palette.GetNumberOfColours())
		m_SelectedCell = INVALID_COLOUR_VALUE;
	if (m_MouseOverCell > m_Palette.GetNumberOfColours())
		m_MouseOverCell = INVALID_COLOUR_VALUE;

	DialogManager::InvalidateGadget(m_WindowID, _R(IDC_T2_PALETTE_CONTROL));
}

/********************************************************************************************
>	void BitmapExportPaletteControl::Render(ReDrawInfoType *pInfo)
	Author:		Jonathan_Payne (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/12/2000
	Returns:	Draw the palette.  This assumes that the pInfo struct contains a DC to draw
				with.
********************************************************************************************/
void BitmapExportPaletteControl::Render(ReDrawInfoType *pInfo)
{
	DocRect PaletteSize(0, 0, pInfo->dx, pInfo->dy);

	//  Create a RenderRegion so that we can draw the control
	RenderRegion *pRender = CreateOSRenderRegion(&PaletteSize, pInfo);
	if (pRender == 0)
		return;	// Could not create a RenderRegion

	// Decide if we are going to render the palette
	if (!m_Palette.GetNumberOfColours())
	{
		// We are not rendering a palette so just draw a grey box
		RenderGrey(&PaletteSize, pRender);
	}
	else
	{
		// Check the selection and mouse over colour are valid
		if (m_SelectedCell > m_Palette.GetNumberOfColours())
			m_SelectedCell = INVALID_COLOUR_VALUE;
		if (m_MouseOverCell > m_Palette.GetNumberOfColours())
			m_MouseOverCell = INVALID_COLOUR_VALUE;

		// We are rendering a full palette
		RenderPalette(&PaletteSize, pRender, pInfo->dy, pInfo->pClipRect);
	}

	DestroyOSRenderRegion(pRender);
}

/********************************************************************************************
>	void BitmapExportPaletteControl::RenderGrey(DocRect *pPaletteSize, RenderRegion *pRender)
	Author:		Jonathan_Payne (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/12/2000
	Purpose:	Render the palette's background in dialog background colour.
********************************************************************************************/
void BitmapExportPaletteControl::RenderGrey(DocRect *pPaletteSize, RenderRegion *pRender)
{
	DialogColourInfo DialogColours;
	pRender->SetFillColour(DialogColours.DialogBack());
	pRender->SetLineColour(COLOUR_TRANS);
	pRender->DrawRect(pPaletteSize);
}

void BitmapExportPaletteControl::GetRectForCell(INT32 cell, DocRect *pRect, INT32 controlHeight)
{
	pRect->lox = (cell % m_nCellsPerLine) * m_nCellWidth;
	pRect->loy = controlHeight - (cell / m_nCellsPerLine) * m_nCellHeight - m_nCellHeight;
	pRect->hix = (cell % m_nCellsPerLine + 1) * m_nCellWidth - m_nPixelSize;
	pRect->hiy = controlHeight - (cell / m_nCellsPerLine) * m_nCellHeight - m_nPixelSize;
}

void BitmapExportPaletteControl::RenderPalette(DocRect *pPaletteSize, RenderRegion *pRender, INT32 controlHeight,
											   DocRect* pClipRect)
{
	INT32 nColours = m_Palette.GetNumberOfColours();

	// See if the background needs rendering
	if (nColours < m_NumberOfColoursAtLastRedraw)
		RenderGrey(pPaletteSize, pRender);

	m_NumberOfColoursAtLastRedraw = nColours;

	pRender->SetLineColour(COLOUR_BLACK);

	DocRect cell;

	// Draw each cell in turn
	for (INT32 i = 0; i < nColours; ++i)
	{
		GetRectForCell(i, &cell, controlHeight);
		if (cell.IsIntersectedWith(*pClipRect))
		{
			BYTE r, g, b;
			r = m_Palette.GetRed(i);
			g = m_Palette.GetGreen(i);
			b = m_Palette.GetBlue(i);
			DrawCell(&cell, DocColour(r, g, b), m_Palette.GetFlags(i), pRender,
				IsColourWebSafe(r, g, b), i == m_SelectedCell);
		}
	}

	// Draw the mouse over high light
	if (m_MouseOverCell != INVALID_COLOUR_VALUE)
	{
		GetRectForCell(m_MouseOverCell, &cell, controlHeight);
		pRender->SetFillColour(COLOUR_TRANS);
		pRender->SetLineColour(COLOUR_WHITE);
		pRender->DrawRect(&cell);
	}

}

void BitmapExportPaletteControl::SetSelectedColourToggleLocked()
{
	m_Palette.SetFlags(m_SelectedCell, m_Palette.GetFlags(m_SelectedCell) ^ LOCKED_COLOUR);
	if (!GetSelectedColourLocked())
		BmapPrevDlg::m_pExportOptions->InvalidatePalette();
}

/********************************************************************************************
>	void BitmapExportPaletteControl::MakeColourWebSafe(BYTE *pColour)
	Author:		Jonathan_Payne (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/2000
	Inputs:		A pointer to a colour value (in BYTE format)
	Purpose:	Makes a colour web safe by moving it to the closest multiple of 51
********************************************************************************************/
void BitmapExportPaletteControl::MakeColourWebSafe(BYTE *pColour)
{
	if (*pColour % 51 != 0)
	{
		if (*pColour % 51 > 20)
			// round up to nearest multiple of 51
			*pColour += (51 - *pColour % 51);
		else
			// round down to nearest multiple of 51
			*pColour -= *pColour % 51;
	}
}

/********************************************************************************************
>	void BitmapExportPaletteControl::SetSelectedColourWebSafe()
	Author:		Jonathan_Payne (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/2000
	Purpose:	Makes the currently selected colour web safe
********************************************************************************************/
void BitmapExportPaletteControl::SetSelectedColourWebSafe()
{
	INT32 realIndex = m_Palette.SortedValueToRealValue(m_SelectedCell);

	ExtendedPalette	*palette = BmapPrevDlg::m_pExportOptions->GetExtendedPalette();
	ERROR3IF(!palette, "There is no palette - This should never happen");

	if (!GetSelectedColourLocked())
		SetSelectedColourToggleLocked();

	palette->Data[realIndex].PreEditedRed	= palette->Data[realIndex].Red;
	palette->Data[realIndex].PreEditedGreen	= palette->Data[realIndex].Green;
	palette->Data[realIndex].PreEditedBlue	= palette->Data[realIndex].Blue;

	MakeColourWebSafe(&palette->Data[realIndex].Red);
	MakeColourWebSafe(&palette->Data[realIndex].Green);
	MakeColourWebSafe(&palette->Data[realIndex].Blue);
}

bool BitmapExportPaletteControl::SetSelectedColourToggleTransparent()
{
	m_Palette.SetFlags(m_SelectedCell, m_Palette.GetFlags(m_SelectedCell) ^ TRANSPARENT_COLOUR);
	// if we have turned off transp for the background return TRUE to let the caller know
	if (m_SelectedCell == 0 && ((m_Palette.GetFlags(m_SelectedCell) & TRANSPARENT_COLOUR) == 0))
		return true;
	else
		return false;
}
void BitmapExportPaletteControl::SetSelectedColourToggleDeleted()
{
	m_Palette.SetFlags(m_SelectedCell, m_Palette.GetFlags(m_SelectedCell) ^ DELETED_COLOUR);
	if (GetSelectedColourDeleted())
		BmapPrevDlg::m_pExportOptions->MakePaletteEntryUnreadable(m_Palette.SortedValueToRealValue(m_SelectedCell));
	else
		BmapPrevDlg::m_pExportOptions->InvalidatePalette();
}

INT32 BitmapExportPaletteControl::GetSelectedColour()
{
	return m_Palette.SortedValueToRealValue(m_SelectedCell);
}

INT32 BitmapExportPaletteControl::GetMouseOverColour()
{
	return m_Palette.SortedValueToRealValue(m_MouseOverCell);
}

bool BitmapExportPaletteControl::GetSelectedColourLocked()
{
	return ((m_Palette.GetFlags(m_SelectedCell) & LOCKED_COLOUR) != 0);
}

bool BitmapExportPaletteControl::IsColourWebSafe(BYTE r, BYTE g, BYTE b)
{
	return (r % 51 == 0 && g % 51 == 0 && b % 51 == 0);
}

bool BitmapExportPaletteControl::GetSelectedColourWebSafe()
{
	return IsColourWebSafe(	m_Palette.GetRed(m_SelectedCell),
							m_Palette.GetGreen(m_SelectedCell),
							m_Palette.GetBlue(m_SelectedCell));
}

bool BitmapExportPaletteControl::GetSelectedColourTransparent()
{
	return ((m_Palette.GetFlags(m_SelectedCell) & TRANSPARENT_COLOUR) != 0);
}

bool BitmapExportPaletteControl::GetSelectedColourDeleted()
{
	return ((m_Palette.GetFlags(m_SelectedCell) & DELETED_COLOUR) != 0);
}

/********************************************************************************************
>	void BitmapExportPaletteControl::DrawCell(DocRect *pCellRect, DocColour colour, INT32 flags,
					RenderRegion *pRender, bool webSafe, bool selected, bool highlight)
	Author:		Jonathan_Payne (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/2000
	Purpose:	Draw a palette cell with the given options
	Inputs:		pCellRect	- The rect to draw the palette cell in
				colour		- The colour of the palette entry to draw
				flags		- The flags of that palette entry (locked, transparency, etc)
				pRender		- The render region to render with (tested with an OS rr)
				webSafe		- Is the colour web safe (ie do we draw the web safe mark)
				selected	- Do we draw the cell as the selected cell?
********************************************************************************************/
void BitmapExportPaletteControl::DrawCell(DocRect *pCellRect, DocColour colour, INT32 flags, RenderRegion *pRender,
												bool webSafe, bool selected)
{
	pRender->SetLineColour(COLOUR_BLACK);
	pRender->SetFillColour(colour);
	pRender->DrawRect(pCellRect);

	if (selected)
	{
		pRender->SetFillColour(COLOUR_TRANS);

		DocRect cell = *pCellRect; // make temp copy

		// Draw the outer line in white
		pRender->SetLineColour(COLOUR_WHITE);
		cell.Inflate(-1 * m_nPixelSize);
		pRender->DrawRect(&cell);

		// Draw the inner line in black
		pRender->SetLineColour(COLOUR_BLACK);
		cell.Inflate(-1 * m_nPixelSize);
		pRender->DrawRect(&cell);
	}

	if (flags & DELETED_COLOUR)
	{
		//  Draw a mark to show that this palette entry has been deleted

		// Draw a black line from top left to bottom right
		pRender->SetLineColour(COLOUR_BLACK);
		pRender->DrawLine(	DocCoord(pCellRect->lox, pCellRect->hiy + m_nPixelSize),
							DocCoord(pCellRect->hix + m_nPixelSize, pCellRect->loy));

		// Draw a white line from top right to bottom left
		pRender->SetLineColour(COLOUR_WHITE);
		pRender->DrawLine(	DocCoord(pCellRect->hix + m_nPixelSize, pCellRect->hiy + m_nPixelSize),
							DocCoord(pCellRect->lox + m_nPixelSize, pCellRect->loy + m_nPixelSize));

		return; // Stop before rending any other marks
	}

	if (flags & LOCKED_COLOUR)
	{
		//  Draw a mark to show that this palette entry has been
		//  edited by the user.

		pRender->SetLineColour(COLOUR_BLACK);
		pRender->SetFillColour(COLOUR_WHITE);
		
		// Draw a rectangle in the bottom left corner of the cell
		DocRect markRect;
		markRect.hix	= pCellRect->lox + m_nPixelSize * 2;
		markRect.lox	= pCellRect->lox;
		markRect.hiy	= pCellRect->loy + m_nPixelSize * 2;
		markRect.loy	= pCellRect->loy;
		pRender->DrawRect(&markRect);
	}

	if (flags & TRANSPARENT_COLOUR)
	{
		//  Draw a mark to show that this palette entry is transparent

		pRender->SetLineColour(COLOUR_BLACK);
		pRender->SetFillColour(COLOUR_WHITE);
		
		// Draw a rectangle in the top left corner of the cell
		DocRect markRect;
		markRect.hix	= pCellRect->lox + m_nPixelSize * 2;
		markRect.lox	= pCellRect->lox;
		markRect.hiy	= pCellRect->hiy;
		markRect.loy	= pCellRect->hiy - m_nPixelSize * 2;
		pRender->DrawRect(&markRect);
	}
	
	if (webSafe)
	{
		//  Draw a mark to show that this palette entry is web-safe

		pRender->SetLineColour(COLOUR_BLACK);
		pRender->SetFillColour(COLOUR_WHITE);

		DocRect markRect = *pCellRect;
		markRect.Inflate(-4 * m_nPixelSize);
		pRender->DrawRect(&markRect);
	}
}
