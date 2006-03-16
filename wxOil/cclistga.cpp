// $Id: cclistga.cpp 662 2006-03-14 21:31:49Z alex $
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

//****************************************************************************************************
//  File contents: implementation of the Win95 list control
//  Created: 06/08/96 (Adrian)
//****************************************************************************************************

#include "camtypes.h" 

#include "bitmpinf.h"
#include "camelot.h"
#include "cclistga.h"
#include "gbrush.h"
#include "palman.h"
#include "wbitmap.h"


CC_IMPLEMENT_MEMDUMP(CCListGadget, CC_CLASS_MEMDUMP);

// This must go after all CC_IMPLEMENT macros.
#define new CAM_DEBUG_NEW



#define USE_CUSTOM_ENABLER // undef this if you want to stick with the Windows one

CTypedPtrArray<CObArray, CCListGadget::ListEntry*> CCListGadget::GadgetList;

CCListGadget* CCListGadget::Create(CWindowID parentID, CGadgetID gadgetID)
{
	CCListGadget* pListGadget = new CCListGadget(parentID, gadgetID);
	if (pListGadget)
	{
		if (IsWindows31()) // on Win32s we have to set the font to MS Sans Serif - the system font won't work properly 
		{
			pListGadget->gadgetFont.CreatePointFont(80, "MS Sans Serif");
			::SendDlgItemMessage((HWND) parentID, gadgetID, WM_SETFONT, (WPARAM) (HFONT)  pListGadget->gadgetFont, (LPARAM) NULL);
			HWND hGadget = GetDlgItem((HWND) parentID, gadgetID);
			DWORD dwStyle = GetWindowLong(hGadget, GWL_STYLE);
			dwStyle |= LVS_NOSCROLL;
			SetWindowLong(hGadget, GWL_STYLE, dwStyle);
		}
		return	(pListGadget->AddToList()) ? pListGadget : (CCListGadget*) NULL;
	}
	else return (CCListGadget*) NULL;
}


//private constructor
CCListGadget::CCListGadget(CWindowID parentID, CGadgetID gadgetID)
{
	listID = gadgetID;
	formID = parentID;
	hHeader = 0;
	hHeaderBitmap = 0;
	columns = 0; 
	switches = 0; // no switches by default
	switchColumnWidth = 0;
	hStateEnabled = 0;
	hStateDisabled = 0;
	hImageListEnabled = 0;
	hImageListDisabled = 0;
	isEnabled = TRUE;
	isColourList = FALSE;
	for (INT32 i = 0; i < SWITCHES_SUPPORTED; i++)
		SwitchLeftCoord[i] = 0;
}



/******************************************************************************************

>	CCListGadget::~CClistGadget()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/9/96

	Purpose:	Destructor. Makes sure any used resources are cleaned up

******************************************************************************************/

CCListGadget::~CCListGadget()
{
	ERROR3IF(hHeader != NULL || hHeaderBitmap != NULL ||
			hStateEnabled != NULL || hStateDisabled != NULL ||
			hImageListEnabled != NULL || hImageListDisabled != NULL,
			"CCListGadget object probably leaked");
}



//function to add this gadget to the global list
CCListGadget::AddToList()
{
	ListEntry* pListEntry = new ListEntry(formID, listID, this);
	if (pListEntry)	
	{
		GadgetList.Add(pListEntry);
		return TRUE;
	}
	else return FALSE;
}

void CCListGadget::OnDialogClose(CWindowID dialogID)
{
	for (INT32 i = 0; i < CCListGadget::GadgetList.GetSize(); i++)
	{
		ListEntry* listIterator = CCListGadget::GadgetList.GetAt(i);
		if (listIterator)
		{
			if ((listIterator->parentID == dialogID) || (::IsChild((HWND) dialogID, (HWND) listIterator->parentID)))
			// the list is either on the dialog's form or on one of its tabs, so we kill it
				listIterator->pListGadget->DestroyGadget();
		}
	}
}


CCListGadget* CCListGadget::GetGadget(CWindowID parentID, CGadgetID gadgetID)
{
	ListEntry* pListEntry = NULL;
	for (INT32 i = 0; i < CCListGadget::GadgetList.GetSize(); i++)
	{
		ListEntry* listIterator = CCListGadget::GadgetList.GetAt(i);
		if (listIterator)
		{
			if ((parentID == listIterator->parentID) && (gadgetID == listIterator->gadgetID))
				pListEntry = listIterator;
		}
	}
	if (pListEntry) return pListEntry->pListGadget;
	else return (CCListGadget*) (void*) NULL;
}

void CCListGadget::DestroyGadget()
{
	//first find this object's entry in the global list and delete it
	ListEntry* pListEntry = NULL;
	for (INT32 i = 0; i < CCListGadget::GadgetList.GetSize(); i++)
	{
		ListEntry* listIterator = CCListGadget::GadgetList.GetAt(i);
		if (listIterator)
		{
			if ((formID == listIterator->parentID) && (listID == listIterator->gadgetID))
				GadgetList.RemoveAt(i);
		}
	}
	
	// Destroy the image lists, if any
	if (hImageListEnabled && !ImageList_Destroy(hImageListEnabled))
		ERROR3("Error deleting image list");
	hImageListEnabled = NULL;

	if (hImageListDisabled && !ImageList_Destroy(hImageListDisabled))
		ERROR3("Error deleting image list");
	hImageListDisabled = NULL;

	if (hStateEnabled && !ImageList_Destroy(hStateEnabled))
		ERROR3("Error deleting image list");
	hStateEnabled = NULL;

	if (hStateDisabled && !ImageList_Destroy(hStateDisabled))
		ERROR3("Error deleting image list");
	hStateDisabled = NULL;

	// Destroy the header window
	if (hHeader != NULL)
	{
		::DestroyWindow(hHeader);
		hHeader = NULL;
	}

	// Destroy the header bitmap
	if (hHeaderBitmap != NULL)
	{
		::DeleteObject(hHeaderBitmap);
		hHeaderBitmap = NULL;
	}

	//now delete the object itself
	delete this;
}

BOOL CCListGadget::CreateCustomHeader(UINT32 bitmapID)
{
	//The listview control has a built-in header, but we will create our own as we want it to display bitmaps
	//First get control's coordinates, so that we know where to place the header

	RECT listviewRect;
	HWND hGadget = GetDlgItem((HWND) formID, listID);
	GetWindowRect(hGadget, &listviewRect);
	POINT listviewOrigin = { listviewRect.left, listviewRect.top };
	ScreenToClient((HWND) formID, &listviewOrigin);

	hHeaderBitmap = LoadBitmap(AfxGetResourceHandle(), MAKEINTRESOURCE(bitmapID)); 
	ERROR2IF(!hHeaderBitmap, FALSE, "Failed to load header bitmap");

	//Get the height of the bitmap so we can figure out the height of the header
	BITMAP bitmap;
	if (!GetObject(hHeaderBitmap, sizeof(bitmap), &bitmap))
	{
		ERROR2RAW("Failed to get header bitmap data");
		return(FALSE);
	}
	//Change the background colour of the bitmap to the one used by dialog backgrounds, in case the colour scheme used is not the default 
	SetBitmapBkgToSystem(hHeaderBitmap);
	//Create the header
	hHeader = CreateWindowEx(
				0L,							// No extended styles.
				WC_HEADER,					// A header class window.
				(LPCTSTR) NULL,				// No default text.
				WS_CHILD | /*WS_BORDER | */ HDS_BUTTONS | HDS_HORZ, 
				listviewOrigin.x, listviewOrigin.y - bitmap.bmHeight + 1,
				listviewRect.right - listviewRect.left,	bitmap.bmHeight,
				GetParent(hGadget),         // Handle to the parent window.
				(HMENU) _R(ID_HEADER),			// ID for the header window.
				AfxGetInstanceHandle(),		// Current instance.
				(LPVOID) NULL);

	if (hHeader)
	{
		ShowWindow(hHeader, SW_SHOW);
		::EnableWindow(hHeader, FALSE); // We disable it so that clicking on it has no effect
		//SetWindowPos(hGadget, HWND_TOPMOST, 0,0,0,0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
		HD_ITEM hdi;  // Header item.
		hdi.mask = HDI_FORMAT | HDI_WIDTH | HDI_BITMAP; // the header only contains a bitmap 
		hdi.fmt = HDF_LEFT | HDF_BITMAP;  // Left-justify the item.
		hdi.cxy = listviewRect.right - listviewRect.left;           // Make it as wide as the window.
		hdi.hbm = hHeaderBitmap;
		return (Header_InsertItem(hHeader, 0, &hdi) != -1);
	}

	return FALSE; // we failed
}

BOOL CCListGadget::OnSysColorChange(CWindowID dialogID)
{
	// search the global list of gadgets to find if there are any on the current dialog form or one of its tabs
	for (INT32 i = 0; i < CCListGadget::GadgetList.GetSize(); i++)
	{
		ListEntry* listIterator = CCListGadget::GadgetList.GetAt(i);
		if (listIterator)
		{
			if ((listIterator->parentID == dialogID) || (::IsChild((HWND) dialogID, (HWND) listIterator->parentID)))
			// the list is either on the dialog's form or on one of its tabs, so forward the message to it
			{
				CCListGadget* pList = listIterator->pListGadget;
				SendDlgItemMessage(pList->formID, pList->listID, WM_SYSCOLORCHANGE, NULL, NULL);
				// if the list is got a header, we need to send the message to it and additionally to change the background of the header bitmap to COLOR_3DFACE		
				if (pList->hHeader && pList->hHeaderBitmap)
				{
					SendDlgItemMessage(pList->formID, _R(ID_HEADER), WM_SYSCOLORCHANGE, NULL, NULL);
					HD_ITEM hdi;  // Header item.
					hdi.mask = HDI_BITMAP;
					if (!Header_GetItem(pList->hHeader, 0, &hdi))
					{
						ERROR2RAW("Failed to update list header colours");
						return(FALSE);
					}
					pList->SetBitmapBkgToSystem(hdi.hbm);
					InvalidateRect(pList->hHeader, NULL, TRUE);
					UpdateWindow(pList->hHeader);
				}
			}
		}
	}
	return TRUE;
}
		

BOOL CCListGadget::SetBitmapBkgToSystem(HBITMAP hBitmap)
{
	BITMAP bitmap;
	HDC hBitmapDC = CreateCompatibleDC(NULL);
	if (!GetObject(hBitmap, sizeof(bitmap), &bitmap) || !hBitmapDC)
	{
		ERROR2RAW("Non-fatal GDI error");
		return(FALSE);
	}
	SelectObject(hBitmapDC, hBitmap);
	// We make the assumption that the pixel in the lower right corner has the background colour 
	DWORD currentBkColour = (DWORD) GetPixel(hBitmapDC, bitmap.bmWidth - 1, bitmap.bmHeight -1); 
	DWORD sysBkColour = GetSysColor(COLOR_3DFACE);
	for (INT32 i = 0; i < bitmap.bmWidth; i++)
	{
		for (INT32 j = 0; j < bitmap.bmHeight; j++)
		{
			if ((DWORD) GetPixel(hBitmapDC, i, j) == currentBkColour)
				SetPixelV(hBitmapDC, i, j, (COLORREF) sysBkColour);
		}
	}
	DeleteDC(hBitmapDC);
	return TRUE;
}


BOOL CCListGadget::SetSwitches(UINT32 noOfSwitches, UINT32 idBitmapOn, UINT32 idBitmapOff, UINT32 idDisabledBitmapOn, UINT32 idDisabledBitmapOff)
{
	if (switches != 0)
	{
		ERROR3("Switches have already been set for this list gadget");
		return FALSE;
	}
	if ((noOfSwitches < 1) || (noOfSwitches > SWITCHES_SUPPORTED))
	{
		ERROR3("Incorrect number of switches");
		return FALSE;
	}
	switches = noOfSwitches;

	//Create two arrays of bitmaps, one for each possible state of the listview (enabled/disabled) 
	INT32 noOfBitmaps = 1 << noOfSwitches;
	HBITMAP bitmaps[1 << SWITCHES_SUPPORTED];
	HBITMAP disabledBitmaps[1 << SWITCHES_SUPPORTED];

	for (INT32 i = 0; i < (1 << SWITCHES_SUPPORTED); i++)
	{
		bitmaps[i] = NULL;
		disabledBitmaps[i] = NULL;
	}

	const INT32 bitmapHeight = SWITCH_HEIGHT + 2;

	// BLOCK
	{
		HBITMAP hBitmapOn =  LoadBitmap(AfxGetResourceHandle(), MAKEINTRESOURCE(idBitmapOn));
		HBITMAP hBitmapOff =  LoadBitmap(AfxGetResourceHandle(), MAKEINTRESOURCE(idBitmapOff));
		ERROR2IF(!((hBitmapOn) && (hBitmapOff)), FALSE, "Error loading bitmaps");

		HBITMAP hDisabledBitmapOn =  LoadBitmap(AfxGetResourceHandle(), MAKEINTRESOURCE(idDisabledBitmapOn));
		HBITMAP hDisabledBitmapOff =  LoadBitmap(AfxGetResourceHandle(), MAKEINTRESOURCE(idDisabledBitmapOff));
		ERROR2IF(!((hDisabledBitmapOn) && (hDisabledBitmapOff)), FALSE, "Error loading bitmaps");

		BOOL ok = TRUE;
		
		//Ensure that the bitmaps have correct dimmensions (we only check the enabled bitmaps) 
		// BLOCK
		{
			BITMAP bitmapOn;
			BITMAP bitmapOff;
			ok = GetObject(hBitmapOn, sizeof(bitmapOn), &bitmapOn);
			ERROR3IF(!ok, "Can't get 'ON' switch bitmap data");
			ERROR3IF(!((bitmapOn.bmHeight == SWITCH_HEIGHT) && (bitmapOn.bmWidth == SWITCH_WIDTH)),
				"'ON' switch bitmap is not the size defined in cclistga.h (SWITCH_WIDTH x SWITCH_HEIGHT)");
			ok = GetObject(hBitmapOff, sizeof(bitmapOff), &bitmapOff);
			ERROR3IF(!ok, "Can't get 'OFF' switch bitmap data");
			ERROR3IF(!((bitmapOff.bmHeight == SWITCH_HEIGHT) && (bitmapOff.bmWidth == SWITCH_WIDTH)),
				"'OFF' switch bitmap is not the size defined in cclistga.h (SWITCH_WIDTH x SWITCH_HEIGHT)");
		}

		//Make sure that this width is always even, or CreateBitmapIndirect() - which is used to create the bitmap inside CreateScreenCompatibleBitmap() - will fail !!!
		switchColumnWidth = SWITCH_MARGIN + noOfSwitches * (SWITCH_WIDTH + SWITCH_SEPARATOR);
		for (INT32 i = 0; i < noOfBitmaps; i++)
		{
			bitmaps[i] = CreateScreenCompatibleBitmap(switchColumnWidth,bitmapHeight);
			disabledBitmaps[i] = CreateScreenCompatibleBitmap(switchColumnWidth,bitmapHeight);
			HDC hSwitchBitmapDC = CreateCompatibleDC(NULL);
			HDC hDisabledBitmapDC = CreateCompatibleDC(NULL);

			HBITMAP OldSwitchBitmap = (HBITMAP) SelectObject(hSwitchBitmapDC, bitmaps[i]);
			HBITMAP OldDSwitchBitmap = (HBITMAP) SelectObject(hDisabledBitmapDC, disabledBitmaps[i]);

#ifdef USE_CUSTOM_ENABLER
			PatBlt(hDisabledBitmapDC, 0, 0, switchColumnWidth, bitmapHeight, WHITENESS);
#else
			HBRUSH OldBrush = SelectObject(hDisabledBitmapDC, (HBRUSH) GetStockObject(LTGRAY_BRUSH));
			PatBlt(hDisabledBitmapDC, 0, 0, switchColumnWidth, bitmapHeight, PATCOPY);// when disabled switches will be gray
#endif
			PatBlt(hSwitchBitmapDC, 0, 0, switchColumnWidth, bitmapHeight, WHITENESS);// blit a white background

			//Combine the on/off bitmaps to create a composite state bitmap
			HDC hBitmapOnDC = CreateCompatibleDC(NULL);
			HDC hBitmapOffDC = CreateCompatibleDC(NULL);

			HBITMAP OldBitmapOn = (HBITMAP) SelectObject(hBitmapOnDC, hBitmapOn);
			HBITMAP OldBitmapOff = (HBITMAP) SelectObject(hBitmapOffDC, hBitmapOff);

			HDC hDisabledBitmapOnDC = CreateCompatibleDC(NULL);
			HDC hDisabledBitmapOffDC = CreateCompatibleDC(NULL);

			HBITMAP OldDisBitmapOn = (HBITMAP) SelectObject(hDisabledBitmapOnDC, hDisabledBitmapOn);
			HBITMAP OldDisBitmapOff = (HBITMAP) SelectObject(hDisabledBitmapOffDC, hDisabledBitmapOff);

			for (UINT32 j = 0; j < noOfSwitches; j++)
			{
				// we blit the switches from right to left
				if ( i & (1 << j)) // so this bit is 1
				{
					ok = BitBlt(hSwitchBitmapDC,
					switchColumnWidth - (j + 1) * (SWITCH_WIDTH + SWITCH_SEPARATOR),  // x-coordinate of switch's upper-left corner
					1, // we leave a 1 pixel border
					SWITCH_WIDTH,
					SWITCH_HEIGHT,
					hBitmapOnDC,
					0,
					0,
					SRCCOPY);
					ERROR3IF(!ok, "GDI Error (most likely invalid bitmap handle)");
#ifdef USE_CUSTOM_ENABLER
					ok = BitBlt(hDisabledBitmapDC,
					switchColumnWidth - (j + 1) * (SWITCH_WIDTH + SWITCH_SEPARATOR),  // x-coordinate of switch's upper-left corner
					1, // we leave a 1 pixel border
					SWITCH_WIDTH,
					SWITCH_HEIGHT,
					hDisabledBitmapOnDC,
					0,
					0,
					SRCCOPY);
					ERROR3IF(!ok, "GDI Error (most likely invalid bitmap handle)");
#endif
				}
				else // so this bit is OFF
				{
					ok = BitBlt(hSwitchBitmapDC,
					switchColumnWidth - (j + 1) * (SWITCH_WIDTH + SWITCH_SEPARATOR),  // x-coordinate of switch's upper-left corner
					1, // we leave a 1 pixel border
					SWITCH_WIDTH,
					SWITCH_HEIGHT,
					hBitmapOffDC,
					0,
					0,
					SRCCOPY);
					ERROR3IF(!ok, "GDI Error (most likely invalid bitmap handle)");
#ifdef USE_CUSTOM_ENABLER
					ok = BitBlt(hDisabledBitmapDC,
					switchColumnWidth - (j + 1) * (SWITCH_WIDTH + SWITCH_SEPARATOR),  // x-coordinate of switch's upper-left corner
					1, // we leave a 1 pixel border
					SWITCH_WIDTH,
					SWITCH_HEIGHT,
					hDisabledBitmapOffDC,
					0,
					0,
					SRCCOPY);
					ERROR3IF(!ok, "GDI Error (most likely invalid bitmap handle)");
#endif
				}
				if (!SwitchLeftCoord[noOfSwitches - 1 - j])
					SwitchLeftCoord[noOfSwitches - 1 - j] = switchColumnWidth - (j + 1) * (SWITCH_WIDTH + SWITCH_SEPARATOR);
			}	// End for (j)

			// Restore the DCs
			SelectObject(hBitmapOnDC, OldBitmapOn);
			SelectObject(hBitmapOffDC, OldBitmapOff);

			SelectObject(hDisabledBitmapOnDC, OldDisBitmapOn);
			SelectObject(hDisabledBitmapOffDC, OldDisBitmapOff);

			SelectObject(hSwitchBitmapDC, OldSwitchBitmap);
			SelectObject(hDisabledBitmapDC, OldDSwitchBitmap);

#ifndef USE_CUSTOM_ENABLER
			SelectObject(hDisabledBitmapDC, OldBrush);
#endif
			// And then delete the DCs
			DeleteDC(hSwitchBitmapDC);
			DeleteDC(hBitmapOnDC);
			DeleteDC(hBitmapOffDC);
			DeleteDC(hDisabledBitmapDC);
			DeleteDC(hDisabledBitmapOnDC);
			DeleteDC(hDisabledBitmapOffDC);
		}	// End for (i)

		// And delete our temporary bitmaps
		::DeleteObject(hBitmapOn);
		::DeleteObject(hBitmapOff);

		::DeleteObject(hDisabledBitmapOn);
		::DeleteObject(hDisabledBitmapOff);
	} // End block


	//Create two state image list from our arrays and attach them to the control
	hStateEnabled = CreateImageList(switchColumnWidth, bitmapHeight);
	hStateDisabled = CreateImageList(switchColumnWidth, bitmapHeight);
	for (INT32 k = 0; k < noOfBitmaps; k++)
	{
		if (hStateEnabled != NULL)
			ImageList_Add(hStateEnabled, bitmaps[k], NULL);

		if (hStateDisabled != NULL)
			ImageList_Add(hStateDisabled, disabledBitmaps[k], NULL);
	
		DeleteObject(bitmaps[k]);
		DeleteObject(disabledBitmaps[k]);
	}

	HWND hListGadget = GetDlgItem((HWND) formID, listID);
	ERROR2IF(!hListGadget, FALSE, "Invalid control handle");

	if (IsWindowEnabled(hListGadget))
	{
		if (hStateEnabled != NULL)
			ListView_SetImageList(hListGadget, hStateEnabled, LVSIL_STATE);
	}
	else
	{
		if (hStateDisabled != NULL)
			ListView_SetImageList(hListGadget, hStateDisabled, LVSIL_STATE);
	}

	// If the columns have ben set, we need to resize them to accomodate the new switches
	if (columns)
	{
		RECT gadgetRect;
		GetWindowRect(hListGadget, &gadgetRect);
		// By default all columns will be the same width
		UINT32 columnWidth = (gadgetRect.right - gadgetRect.left - switchColumnWidth)/columns;
		for (UINT32 i = 0; i < columns; i++)
		{
			if (i)
				ListView_SetColumnWidth(hListGadget, i, columnWidth);
			else
				ListView_SetColumnWidth(hListGadget, i, columnWidth + switchColumnWidth);
		}
	}

	//Check that we have been successful - i.e. we have an image list
//	return (BOOL) ListView_GetImageList( hListGadget, LVSIL_STATE); 

	// we now gracefully degrade if the imagelist is unavailable, so return TRUE
	return TRUE;
}

BOOL CCListGadget::SetEnabled(BOOL enabled)
{
	//Currently we are using our own enabler
	HWND hListGadget = GetDlgItem((HWND) formID, listID);
	ERROR2IF(!hListGadget, FALSE, "Invalid control handle");
	if (!enabled && isEnabled) // disable the list then
	{
		//Change the following to: ListView_SetCallbackMask(hListGadget, NULL) if you want to use ::EnableWindow()
		ListView_SetCallbackMask(hListGadget, LVIS_FOCUSED | LVIS_SELECTED);
		isEnabled = FALSE;

		if (hStateDisabled)
			ListView_SetImageList(hListGadget, hStateDisabled, LVSIL_STATE);

		if (hImageListDisabled)
			ListView_SetImageList(hListGadget, hImageListDisabled, LVSIL_SMALL);

		ListView_SetTextColor(hListGadget, (COLORREF) GetSysColor(COLOR_GRAYTEXT));
		ListView_RedrawItems(hListGadget, 0, GetItemCount() - 1);
		UpdateWindow(hListGadget);
	}
	else if (enabled && !isEnabled) // OK to enable
	{
		ListView_SetCallbackMask(hListGadget, NULL);
		isEnabled = TRUE;

		if (hStateEnabled)
			ListView_SetImageList(hListGadget, hStateEnabled, LVSIL_STATE);	

		if (hImageListEnabled)
			ListView_SetImageList(hListGadget, hImageListEnabled, LVSIL_SMALL);

		ListView_SetTextColor(hListGadget, (COLORREF) GetSysColor(COLOR_WINDOWTEXT));
		ListView_RedrawItems(hListGadget, 0, GetItemCount() - 1);
		UpdateWindow(hListGadget);
	}
	return TRUE;
}

BOOL CCListGadget::SetColumnWidth(INT32 colIndex, INT32 width)
{
	HWND hListGadget = GetDlgItem((HWND) formID, listID);
	ERROR2IF(!hListGadget, FALSE, "Invalid control handle");
	return ListView_SetColumnWidth(hListGadget, colIndex, width);
}

INT32 CCListGadget::GetColumnWidth(INT32 colIndex)
{
	HWND hListGadget = GetDlgItem((HWND) formID, listID);
	ERROR2IF(!hListGadget, FALSE, "Invalid control handle");
	return ListView_GetColumnWidth(hListGadget, colIndex);
}

UINT32 CCListGadget::GetNumberOfSwitches()
{
	return switches;
}

BOOL CCListGadget::SetHeaderString(StringBase& string, UINT32 colIndex)
{
	if (!columns) 	columns = 1;// the columns have not been set, so we default to 1 column
	if (colIndex > (columns - 1))
	{
		ERROR3("Invalid column index");
		return FALSE;
	}
	LV_COLUMN lvc;
	lvc.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT;
	if (columns == 1) lvc.mask |= LVCF_WIDTH;
	lvc.fmt     = LVCFMT_LEFT; // text will be left-aligned
	lvc.cchTextMax = sizeof((TCHAR*) string);
	if (columns == 1)
	{
		RECT gadgetRect;
		GetWindowRect( GetDlgItem((HWND) formID, listID), &gadgetRect);
		lvc.cx = gadgetRect.right - gadgetRect.left;
	}
	lvc.iSubItem = colIndex;
	lvc.pszText = (TCHAR*) string;
	if (columns == 1) return (ListView_InsertColumn( GetDlgItem((HWND) formID, listID), colIndex, &lvc) != -1);
	else return	ListView_SetColumn( GetDlgItem((HWND) formID, listID), colIndex, &lvc);
}

BOOL CCListGadget::SetNumberOfColumns(UINT32 nColumns)
{
	HWND hListGadget = GetDlgItem((HWND) formID, listID);
	ERROR2IF(!hListGadget, FALSE, "Invalid gadget handle");
	if (!nColumns) 
	{
		ERROR3("Invalid number of columns");
		return FALSE;
	}
	BOOL success = TRUE;
	if (columns) 
	{
		ERROR3("Columns already set for list gadget");
		return FALSE;
	}
	//All columns will have the same width
	RECT gadgetRect;
	GetWindowRect(hListGadget, &gadgetRect);
	UINT32 columnWidth = (gadgetRect.right - gadgetRect.left - switchColumnWidth)/nColumns;
	for (UINT32 i = 0; i < nColumns; i++)
	{
		LV_COLUMN lvc;
		lvc.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_WIDTH;
		lvc.fmt     = LVCFMT_LEFT; // text will be left-aligned
		lvc.iSubItem = i;
		if (i)
			lvc.cx = columnWidth;
		else
			lvc.cx = columnWidth + switchColumnWidth;
		if  (ListView_InsertColumn( hListGadget, i, &lvc) == -1)
			success = FALSE;
	}
	if (success)
		columns = nColumns;// if we're fine, update the columns variable  
	return success;
}

UINT32 CCListGadget::GetNumberOfColumns()
{
	//easy one...
	return columns;
}

BOOL CCListGadget::SetSwitchState(BOOL state, UINT32 itemIndex, UINT32 switchIndex)
{
	HWND hwndLV = GetDlgItem((HWND) formID, listID);
	if (itemIndex >= (UINT32) ListView_GetItemCount(hwndLV))
	{
		ERROR3("Invalid item index");
		return FALSE;
	}
	if (switchIndex >= switches)
	{
		ERROR3("Invalid switch index");
		return FALSE;
	}
	//If we get this far the index is correct, so let's get the state bits for the item
	//The state bits are actually a 1-based index into the state image list, shifted 12 places to the right (!!!!!!!)  
	UINT32 itemState = ListView_GetItemState(hwndLV, itemIndex,  LVIS_STATEIMAGEMASK);
	itemState >>= 12;
	itemState--;
	if (state) itemState |= (1 << (switches - switchIndex -1));
	else itemState &= ~(1 << (switches - switchIndex -1));
	itemState++;
	itemState <<= 12;
    ListView_SetItemState(hwndLV, itemIndex, itemState, LVIS_STATEIMAGEMASK);
	return TRUE;
}

BOOL CCListGadget::GetSwitchState(UINT32 itemIndex, UINT32 switchIndex)
{
	HWND hwndLV = GetDlgItem((HWND) formID, listID);
	if (itemIndex >= (UINT32) ListView_GetItemCount(hwndLV))
	{
		ERROR3("Invalid item index");
		return FALSE;
	}
	if (switchIndex >= switches)
	{
		ERROR3("Invalid switch index");
		return FALSE;
	}
	UINT32 itemState = ListView_GetItemState(hwndLV, itemIndex,  LVIS_STATEIMAGEMASK);
	itemState >>= 12;
	itemState--;
	return (itemState & (1 << (switches - switchIndex -1)));
}

BOOL CCListGadget::AddItem(StringBase& itemString, KernelBitmap* pItemImage)
{
	ERROR3IF(isColourList, "This is a colour list - use AddColourListItem()");
	if (!columns)
	{
		ERROR3("Columns not set for list gadget");
		return FALSE;
	}
		
	HWND hwndLV = GetDlgItem((HWND) formID, listID);
	//Check if the insertion will cause the list to scroll  - if yes, we will have to resize the last column
	BOOL resize = (ListView_GetItemCount(hwndLV) == ListView_GetCountPerPage(hwndLV)) ? TRUE : FALSE;
	if (pItemImage) //first set the item image, if any 
	{
		//Check if we have an imagelist
		HIMAGELIST hImgList = ListView_GetImageList(hwndLV, LVSIL_SMALL);
		WinBitmap* pWinBmp = (WinBitmap*) pItemImage->ActualBitmap;
		ERROR2IF(!pWinBmp, FALSE, "Illegal NULL bitmap");
		if (!ListView_GetItemCount(hwndLV)) //this is the first item so we have to create an image list
		{
			//Get the colour depth of our bitmap
			BitmapInfo Info;
			pWinBmp->GetInfo(&Info);
			UINT32 colorFlag = 0;
			switch (Info.PixelDepth)
			{
			case 4:
				colorFlag = ILC_COLOR4;
				break;
			case 8:
				colorFlag = ILC_COLOR8;
				break;
			case 16:
				colorFlag = ILC_COLOR16;
				break;
			case 24:
				colorFlag = ILC_COLOR24;
				break;
			case 32:
				colorFlag = ILC_COLOR32;
				break;
			default:
				ERROR3("Unsupported bitmap colour depth");
				return FALSE;
			}
			//Create the image list and attach it to the control
			hImgList = ImageList_Create(
				pWinBmp->GetWidth(),
				pWinBmp->GetHeight(),	
				colorFlag,
				16,
				MAX_NO_ITEMS);
		}
//		ERROR2IF(!hImgList, FALSE, "Cannot find image list"); //if we still have a null handle, something is badly wrong...

		if (hImgList)
		{
			ListView_SetImageList(hwndLV, hImgList, LVSIL_SMALL);

			//Create a temporary bitmap from the data in WinBmp
			HBITMAP hTempBmp = CreateBitmap(
				pWinBmp->GetWidth(), 
				pWinBmp->GetHeight(),
				pWinBmp->GetBPP(),
				1,
				pWinBmp->BMBytes);
			if (ImageList_Add(hImgList, hTempBmp, NULL) == -1)
			{
				ERROR3("Failed to update image list");
			}
			DeleteObject(hTempBmp);
		}
	}
	//Set the item text
	LV_ITEM itemData;
	INT32 itemIndex = ListView_GetItemCount(hwndLV);
    itemData.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_STATE;

	//The state value is a 1-based index into the state image list shifted 12 bits to the left 
	//All switches will be OFF initially
	itemData.state = 0x01;
	itemData.state <<= 12; 
	itemData.stateMask   = LVIS_STATEIMAGEMASK;
    itemData.iItem = itemIndex;
    itemData.iSubItem = 0;
    itemData.pszText = (TCHAR*) itemString;
    itemData.cchTextMax  = sizeof(itemString);
    itemData.iImage = itemIndex;
   	if (ListView_InsertItem(hwndLV, &itemData) == -1)
	{
		ERROR3("Failed to add list item");
		return FALSE;
	}
	// Check if the insertion will cause the list to scroll  - if yes, we have to resize the last column
	if (resize) // so we need to resize the last column
	{
		ListView_SetColumnWidth(
			hwndLV,
			columns - 1,
			ListView_GetColumnWidth(hwndLV, columns - 1) - GetSystemMetrics(SM_CXVSCROLL));
	}
	return TRUE;
}




BOOL CCListGadget::AddItem(StringBase& itemString, UINT32 bitmapEnabledID, UINT32 bitmapDisabledID)
{
	ERROR3IF(isColourList, "This is a colour list - use AddColourListItem()");
	if (!columns)
	{
		ERROR3("Columns not set for list gadget");
		return FALSE;
	}
	
	HWND hwndLV = GetDlgItem((HWND) formID, listID);
	//Check if the insertion will cause the list to scroll  - if yes, we will have to resize the last column
	BOOL resize = (ListView_GetItemCount(hwndLV) == ListView_GetCountPerPage(hwndLV)) ? TRUE : FALSE;
	HBITMAP hItemBitmapEnabled = LoadBitmap (AfxGetResourceHandle(),MAKEINTRESOURCE(bitmapEnabledID));
	HBITMAP hItemBitmapDisabled = LoadBitmap (AfxGetResourceHandle(),MAKEINTRESOURCE(bitmapDisabledID));
	if (!(hItemBitmapEnabled && hItemBitmapDisabled))
	{
		ERROR3("Failed to load item bitmaps");
		return FALSE;
	}
	if (!ListView_GetItemCount(hwndLV)) //this is the first item so we have to create the image lists
	{
		//Get  a BITMAP struct for our bitmap
		BITMAP bitmapData;
		GetObject(hItemBitmapEnabled, sizeof(BITMAP), &bitmapData); // the two bitmaps, obviously, have to be the same size
		//Create the image lists and attach it to the control
		hImageListEnabled = CreateImageList(bitmapData.bmWidth,	bitmapData.bmHeight);
		hImageListDisabled = CreateImageList(bitmapData.bmWidth, bitmapData.bmHeight);
	}
//	ERROR2IF(!(hImageListEnabled && hImageListDisabled), FALSE, "Cannot find image list"); //if we still have a null handle, something is badly wrong...
	if (isEnabled)
	{
		if (hImageListEnabled)
			ListView_SetImageList(hwndLV, hImageListEnabled, LVSIL_SMALL);
	}
	else
	{
		if (hImageListDisabled)
			ListView_SetImageList(hwndLV, hImageListDisabled, LVSIL_SMALL);
	}

	BOOL Failed = FALSE;

	if (hImageListEnabled && ImageList_Add(hImageListEnabled, hItemBitmapEnabled, NULL) == -1)
	{
		ERROR3("Failed to update image list");
		Failed = TRUE;
	}


	if (hImageListDisabled && ImageList_Add(hImageListDisabled, hItemBitmapDisabled, NULL) == -1)
	{
		ERROR3("Failed to update image list");
		Failed = TRUE;
	}

	DeleteObject(hItemBitmapEnabled);
	DeleteObject(hItemBitmapDisabled);

	if (Failed)
		return(FALSE);

	//Set the item text
	LV_ITEM itemData;
	INT32 itemIndex = ListView_GetItemCount(hwndLV);
    itemData.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_STATE;
	//The state value is a 1-based index into the state image list shifted 12 bits to the left 
	//All switches will be OFF initially
	itemData.state = 0x01;
	itemData.state <<= 12; 
	itemData.stateMask   = LVIS_STATEIMAGEMASK;
    itemData.iItem = itemIndex;
    itemData.iSubItem = 0;
    itemData.pszText = (TCHAR*) itemString;
    itemData.cchTextMax  = sizeof((TCHAR*) itemString);
    itemData.iImage = itemIndex;
   	if (ListView_InsertItem(hwndLV, &itemData) == -1)
	{
		ERROR3("Failed to add list item");
		return FALSE;
	}
	if (resize) // so we need to resize the last column
	{
		ListView_SetColumnWidth(
			hwndLV,
			columns - 1,
			ListView_GetColumnWidth(hwndLV, columns - 1) - GetSystemMetrics(SM_CXVSCROLL));
	}
	return TRUE;
}


HBITMAP CCListGadget::CreateScreenCompatibleBitmap( INT32 width, INT32 height)
{
	HDC hProbeDC = ::CreateCompatibleDC(NULL); // device context used to probe the current screen mode
	ERROR3IF(hProbeDC == NULL, "Couldn't create probe DC");
	const BITMAP bitmapData = 
	{ 
		0,
		width,
		height,
		width * GetDeviceCaps(hProbeDC, BITSPIXEL) * GetDeviceCaps(hProbeDC, PLANES) / 8 ,
		GetDeviceCaps(hProbeDC, PLANES),
		GetDeviceCaps(hProbeDC, BITSPIXEL),
		0L
	};
	DeleteDC(hProbeDC);
	return CreateBitmapIndirect(&bitmapData);
}
	

HIMAGELIST CCListGadget::CreateImageList(INT32 imageWidth, INT32 imageHeight)
{
	UINT32 colourFlag = ILC_COLOR;

	if (IsWindows31())
	{
		// Under Win32s, setting an explicit colour depth means it uses a DIB, and many
		// Win3.1 machines fail to create the bitmaps. However, using ILC_COLOR uses a DDB
		// which works, at least in 8bpp scren modes.
		colourFlag = ILC_COLOR;
	}
	else
	{
		HDC hProbeDC = ::CreateCompatibleDC(NULL); // device context used to probe the current screen mode
		ERROR3IF(hProbeDC == NULL, "Couldn't create probe DC");

		switch (GetDeviceCaps(hProbeDC, BITSPIXEL))
		{
			case 4:
				colourFlag = ILC_COLOR4;
				break;
			case 8:
				colourFlag = ILC_COLOR8;
				break;
			case 16:
				colourFlag = ILC_COLOR16;
				break;
			case 24:
				colourFlag = ILC_COLOR24;
				break;
			case 32:
				colourFlag = ILC_COLOR32;
				break;
			default:
				colourFlag = ILC_COLOR;
				break;
		}

		DeleteDC(hProbeDC);
	}
	HIMAGELIST hImageList = NULL;
	if (hImageList = ImageList_Create(imageWidth, imageHeight, colourFlag, 16, 1)) 
		return hImageList;
	else if (hImageList = ImageList_Create(imageWidth, imageHeight, ILC_COLORDDB, 16, 1)) // try a DDB
		return hImageList;
	else if (hImageList = ImageList_Create(imageWidth, imageHeight, ILC_PALETTE, 16, 1)) // try a palette
		return hImageList;
	else return (HIMAGELIST) NULL; // it failed, there's nothing we can do
} 




/******************************************************************************************

>	BOOL CCListGadget::AddColourListItem(StringBase& colourName,
										INT32 red, INT32 green, INT32 blue,
										BOOL IsSpotColour = FALSE)
	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com> (patch rendering upgraded to use GBrush by Jason)
	Created:	6/8/96?

	Inputs:		colourName - The colour plate name
				red, green, blue - The RGB definition (0..255) of the colour patch
				IsSpotColour - TRUE if the colour is a spot colour (should be drawn
						as a circle), FALSE to draw it as a normal colour square.

	Returns:	TRUE for success

	Purpose:	Adds a colour plate list item to the list - this is a special case for
				the separations dialogue tab. Adds a colour plate name and colour patch
				to the appropriate column of the control

******************************************************************************************/

BOOL CCListGadget::AddColourListItem(StringBase& colourName, INT32 red, INT32 green, INT32 blue, BOOL IsSpotColour)
{
	//ERROR3("Entering AddColourListItem()"); //Win32s "breakpoint" 
	HWND hwndLV = GetDlgItem((HWND) formID, listID);
	if (ListView_GetItemCount(hwndLV) && !isColourList)
	{
		ERROR3("This is not a colour list - use AddItem()");
	}

	if (!columns)
	{
		ERROR3("Columns not set for list gadget");
		return FALSE;
	}

	//Check if the insertion will cause the list to scroll  - if yes, we will have to resize the last column
	BOOL resize;
	if (IsWindows31())
		 resize = (ListView_GetItemCount(hwndLV) == (ListView_GetCountPerPage(hwndLV) - 1)) ? TRUE : FALSE;
	else
		resize = (ListView_GetItemCount(hwndLV) == ListView_GetCountPerPage(hwndLV)) ? TRUE : FALSE;


	if (!ListView_GetItemCount(hwndLV)) //this is the first item so we have to create the image lists
	{
		hImageListEnabled = CreateImageList(COLOUR_PATCH_WIDTH, COLOUR_PATCH_HEIGHT); 
		hImageListDisabled = CreateImageList(COLOUR_PATCH_WIDTH, COLOUR_PATCH_HEIGHT); 
	}
//	ERROR2IF(!(hImageListEnabled && hImageListDisabled), FALSE, "Cannot find image list"); //if we still have a null handle, something is badly wrong...
	if (isEnabled)
	{
		if (hImageListEnabled)
			ListView_SetImageList(hwndLV, hImageListEnabled, LVSIL_SMALL);
	}
	else
	{
		if (hImageListDisabled)
			ListView_SetImageList(hwndLV, hImageListDisabled, LVSIL_SMALL);
	}

	//First create and insert the colour patch associated with the item
	//Create a memory DC and 2 bitmaps (enbled/disabled) compatible with the screen 
	HDC hDC = ::CreateCompatibleDC(NULL);
	ERROR3IF(hDC == NULL, "Couldn't create rendering DC");

	HBITMAP bitmaps[2];
	bitmaps[0] = CreateScreenCompatibleBitmap(COLOUR_PATCH_WIDTH,	COLOUR_PATCH_HEIGHT);
	bitmaps[1] = CreateScreenCompatibleBitmap(COLOUR_PATCH_WIDTH,	COLOUR_PATCH_HEIGHT);

	ERROR2IF(!(bitmaps[0] && bitmaps[1]), FALSE, "GDI Error");

	// And draw the colour patch into the bitmap, using GBrush to get semi-decent dithering
	for (INT32 i = 0; i <= 1; i++)
	{
		if (hDC != NULL)
		{
			HBITMAP OldBitmap = (HBITMAP) ::SelectObject(hDC, bitmaps[i]);

	/*	--- GBrush doesn't work into this bitmap - there are palette problems
			which need to be sorted out.

			// Set up palette and GBrush for Spiffy redraw
			HPALETTE OldPal = NULL;
			if (PaletteManager::UsePalette())
				OldPal = PaletteManager::StartPaintPalette(hDC, (HPALETTE *)NULL, TRUE);

			GBrush GDrawBrush;
			if (GDrawBrush.Init(hDC))
				GDrawBrush.Start();
	*/
			HBRUSH Brush;

			COLORREF Colour = RGB(red, green, blue);
			if (i != 0)
			{
				// Convert the colour to a greyscale
				BYTE Grey = BYTE((red * 0.305) + (green * 0.586) + (blue * 0.109));
				Colour = RGB(Grey, Grey, Grey);
			}

	/*
			if (GDrawBrush.Available())
			{
				LOGBRUSH LogBrush;
				GDrawBrush.GetLogBrush(Colour, &LogBrush);
				Brush = ::CreateBrushIndirect(&LogBrush);
			}
			else	*/
			{
				Brush = ::CreateSolidBrush(Colour);
			}

			ERROR3IF(Brush == NULL, "Couldn't create brush");

			HPEN BlackPen = (HPEN) ::GetStockObject(BLACK_PEN);

			HPEN OldPen = NULL;
			HBRUSH OldBrush = NULL;

			if (IsSpotColour)
			{
				// Spot colours are drawn as circles - but first, fill the bitmap with white,
				// so that the un-covered corners of the square are a sensible colour.
				HBRUSH WhiteBrush = (HBRUSH) ::GetStockObject(WHITE_BRUSH);
				HPEN NullPen = (HPEN) ::GetStockObject(NULL_PEN);
				OldPen = (HPEN) ::SelectObject(hDC, NullPen);
				OldBrush = (HBRUSH) ::SelectObject(hDC, WhiteBrush);
				::Rectangle(hDC, 0, 0, COLOUR_PATCH_WIDTH+1, COLOUR_PATCH_HEIGHT+1);

				::SelectObject(hDC, BlackPen);
				::SelectObject(hDC, Brush);
				::Ellipse(hDC, 0, 0, COLOUR_PATCH_WIDTH, COLOUR_PATCH_HEIGHT);
			}
			else
			{
				// Process colours are shown as rectangles
				OldPen = (HPEN) ::SelectObject(hDC, BlackPen);
				OldBrush = (HBRUSH) ::SelectObject(hDC, Brush);
				::Rectangle(hDC, 0, 0, COLOUR_PATCH_WIDTH, COLOUR_PATCH_HEIGHT);
			}

			::SelectObject(hDC, OldPen);
			::SelectObject(hDC, OldBrush);
			::SelectObject(hDC, OldBitmap);

			::DeleteObject(Brush);

			// Finish with GBrush
	/*
			GDrawBrush.Stop();
			if (OldPal)
				PaletteManager::StopPaintPalette(hDC, OldPal);
	*/
		}
	}

	if (hImageListEnabled && ImageList_Add(hImageListEnabled, bitmaps[0], NULL) == -1)
	{
		ERROR3("Failed to update image list for listview control");
	}
	if (hImageListDisabled && ImageList_Add(hImageListDisabled, bitmaps[1], NULL) == -1)
	{
		ERROR3("Failed to update image list for SysListView32 control");
	}

	DeleteObject(bitmaps[0]);// the bitmap has been copied to an internal buffer, so we can safely delete the original 
	DeleteObject(bitmaps[1]);
	DeleteDC(hDC);

	//Set the item text
	LV_ITEM itemData;
	INT32 itemIndex = ListView_GetItemCount(hwndLV);
    itemData.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_STATE;
	//The state value is a 1-based index into the state image list shifted 12 bits to the left 
	//All switches will be OFF initially
	itemData.state = 0x01;
	itemData.state <<= 12; 
	itemData.stateMask   = LVIS_STATEIMAGEMASK;
    itemData.iItem = itemIndex;
    itemData.iSubItem = 0;
    itemData.pszText = (TCHAR*) colourName;
    itemData.cchTextMax  = sizeof(colourName);
    itemData.iImage = itemIndex;
   	if (ListView_InsertItem(hwndLV, &itemData) == -1)
	{
		ERROR3("Failed to add list item");
		return FALSE;
	}
	isColourList = TRUE;
	if (resize) // so we need to resize the last column
	{
		if (IsWindows31())
		{
			DWORD dwStyle = GetWindowLong(hwndLV, GWL_STYLE);
			dwStyle &= ~LVS_NOSCROLL;
			SetWindowLong(hwndLV, GWL_STYLE, dwStyle);
		}
		else
		{
			ListView_SetColumnWidth(
				hwndLV,
				columns - 1,
				ListView_GetColumnWidth(hwndLV, columns - 1) - GetSystemMetrics(SM_CXVSCROLL));
		}
	}
	//ERROR3("Exiting AddColourListItem()"); // Win32s "breakpoint" 
	return TRUE;
}


BOOL CCListGadget::SetItemString(StringBase& itemString, UINT32 itemIndex, UINT32 columnIndex)
{
	HWND hLV = GetDlgItem((HWND) formID, listID);
	if (columnIndex >= columns)
	{
		ERROR3("Invalid column index");
		return FALSE;
	}
	if (itemIndex >= (UINT32) ListView_GetItemCount(hLV))
	{
		ERROR3("Invalid item index");
		return FALSE;
	}
	ListView_SetItemText(hLV, itemIndex, columnIndex, (TCHAR*)  itemString);	
    return TRUE;
}

BOOL CCListGadget::GetItemString(StringBase& itemString, UINT32 itemIndex, UINT32 columnIndex)
{
	HWND hLV = GetDlgItem((HWND) formID, listID);
	if (columnIndex >= columns)
	{
		ERROR3("Invalid column index");
		return FALSE;
	}
	if (itemIndex >= (UINT32) ListView_GetItemCount(hLV))
	{
		ERROR3("Invalid item index");
		return FALSE;
	}
	TCHAR buffer[64];
	ListView_GetItemText(hLV, itemIndex, columnIndex, buffer, sizeof(buffer));	
    itemString = buffer;
	return TRUE;
}


BOOL CCListGadget::DeleteAllItems()
{
	HWND hLV = GetDlgItem((HWND) formID, listID);
	//This will only delete the item strings, we will have to destroy the image list ourselves
	ListView_DeleteAllItems(hLV);
	//ERROR3("About to delete image lists"); // Win32s "breakpoint"
	if ((hImageListEnabled && (!ImageList_Destroy(hImageListEnabled)) && ImageList_GetImageCount(hImageListEnabled)) ||
		(hImageListDisabled && (!ImageList_Destroy(hImageListDisabled)) && ImageList_GetImageCount(hImageListDisabled)))
	{
		ERROR3("Error deleting image list");
		return FALSE;
	}
	//ERROR3("Image lists deleted now"); // Win32s "breakpoint"

	hImageListEnabled = NULL;
	hImageListDisabled = NULL;

	ListView_SetImageList(hLV, (HIMAGELIST) NULL, LVSIL_SMALL);
	return TRUE;
}


INT32 CCListGadget::GetSelectedItemIndex()
{
	HWND hLV = GetDlgItem((HWND) formID, listID);
	for (INT32 i = 0; i < ListView_GetItemCount(hLV);i++)
	{
//		if (ListView_GetItemState(hLV, i, 0x000F) & LVIS_SELECTED)
		if (ListView_GetItemState(hLV, i, LVIS_SELECTED) != 0)
			return(i);
	}
	return -1;
}



/******************************************************************************************

>	void CCListGadget::SetSelectedItemIndex(INT32 NewSel)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/96 (er.. actually, it's 24/8/96 now...
						Maybe I should go to bed soon...)

	Inputs:		NewSel - The index of the row to select (0 .. NumItems-1)

	Purpose:	Selects (and scrolls to show) the given item (row) in the list.

	Notes:		** IMPORTANT **
				Your ListView MUST have the LVS_SHOWSELALWAYS style set, or else
				the selection will only be/become visible when the control has the
				keyboard input focus. Absence of this style can make it _appear_
				that the selection process has failed, but it has not!

******************************************************************************************/

void CCListGadget::SetSelectedItemIndex(INT32 NewSel)
{
	// Find the window
	HWND hLV = GetDlgItem((HWND) formID, listID);

	// Setting the selected item only has a visible effect when the control has
	// the input focus, unless LVS_SHOWSELALWAYS is set on the control.
	// It is preferable that we set that style rather than grabbing the input 
	// focus, so we expect that our control has this style set, and we don't
	// force the focus into our window.
//	SetFocus(hLV);

	// Set the selection, and make this the active line (put the keyboard focus
	// box around this item, if the control has the focus)
	UINT32 Mask = LVIS_SELECTED | LVIS_FOCUSED;

	// Set the states of all items to ensure only the one item is selected
	INT32 ItemCount = ListView_GetItemCount(hLV);
	for (INT32 i = 0; i < ItemCount; i++)
	{
		UINT32 State = (i == NewSel) ? Mask : 0;		// Select or Deselect
		ListView_SetItemState(hLV, i, State, Mask);
	}

	// Scroll the view to show the selected item
	ListView_EnsureVisible(hLV, NewSel, FALSE);
}



INT32 CCListGadget::GetItemCount()
{
	HWND hLV = GetDlgItem((HWND) formID, listID);
	return ListView_GetItemCount(hLV);
}



/******************************************************************************************

>	BOOL CCListGadget::OnGadgetClicked()

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com>, upgraded to auto-select by Jason
	Created:	6/8/96? (24/8/96)

	Returns:	TRUE for success

	Purpose:	Handles clicks on the ListView control.
				The behaviour is as follows:
					1) Any click anywhere on an item's row selects that item
					2) A click on a switch at the left end of the row will toggle
					   that switches state.

******************************************************************************************/

BOOL CCListGadget::OnGadgetClicked()
{
	//So the user has clicked inside the control, let's see if they have actually clicked a checkbox
	HWND hwndLV = GetDlgItem((HWND) formID, listID);
	// On Win32s we get sometimes a spurious horizontal scroll bar, so we'll turn off WS_HSCROLL  just in case
	/*if (IsWindows31()) 
	{
		DWORD dwStyle = GetWindowLong(hwndLV, GWL_STYLE);
		if (dwStyle & WS_HSCROLL)
			SetWindowLong(hwndLV, GWL_STYLE, (dwStyle & ~WS_HSCROLL));
	}*/
	UINT32        state;
	LV_HITTESTINFO lvhti; // "hit test" structure
    DWORD dwpos = GetMessagePos(); //get the position clicked  
	lvhti.pt.x = LOWORD(dwpos);
	lvhti.pt.y = HIWORD(dwpos);
	MapWindowPoints(HWND_DESKTOP, hwndLV, &lvhti.pt, 1);
	INT32 index = ListView_HitTest(hwndLV, &lvhti); // do a hit test to see if the point lies within the control

	// Added by Jason - Always make the last item clicked-on selected - this means the user
	// can click anywhere and it still selects
	if (index < 0)
	{
		// We didn't click on the important bits of the item (ticks or name text). Helpfully
		// the control doesn't think that clicks to the right are important. So we will
		// do a second hit test, using xpos == 5, to determine which line to select
		lvhti.pt.x = 5;
		index = ListView_HitTest(hwndLV, &lvhti);

		if (index >= 0)
			SetSelectedItemIndex(index);
	
		return(FALSE);		// But don't bother checking the switches
	}

	SetSelectedItemIndex(index);

	//Loop through the switches to find the one that has been clicked
	for (UINT32 i = 0; i < switches; i++)
	{
		if ((lvhti.flags & LVHT_ONITEM) && ((lvhti.pt.x >= SwitchLeftCoord[i] ) && (lvhti.pt.x <= (SwitchLeftCoord[i] + SWITCH_WIDTH))))
		{
			// Yes, a switch has been clicked so get the state from the item and toggle it
			// The state retrieved with the LVIS_STATEIMAGEMASK is a 1-based index into the state image list shifted 12 bits to the left,
			// so we shift it back and substract 1 to turn it into a two-bit value showing the state of the switches
			state = ListView_GetItemState(hwndLV, index, LVIS_STATEIMAGEMASK);
			state >>= 12;
			state --;
			state ^= (1 << (switches - 1 - i));
			state++;
			state <<= 12;
	        ListView_SetItemState(hwndLV, index, state, LVIS_STATEIMAGEMASK);
			return TRUE;		
		 }
	}
	return FALSE;
}

//Unimplemented as yet

BOOL InsertItem(UINT32 index, StringBase& itemString, UINT32 bitmapID)
{
	ERROR3("This function is unimplemented, use AddItem()");
	return FALSE;
}
	

BOOL CCListGadget::InsertItem(UINT32 index, StringBase& itemString, KernelBitmap* pItemImage)
{
	ERROR3("This function is unimplemented, use AddItem()");
	return FALSE;
}

