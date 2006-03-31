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

/*
	The progress indicator (OIL implementation).


*/


#include "camtypes.h"

#include <string.h>						// for memset()
#include "csrstack.h"
#include "oilprog.h"
#include "ensure.h"
#include "keypress.h"
#include "unicdman.h"

//#include "justin.h"						// for cursor bitmaps
//#include "justin2.h"


DECLARE_SOURCE("$Revision$");



// This structure defines a pair of bitmaps, which can be combined to make a cursor.

struct MaskPair
{
	UINT32 AndID;
	UINT32 XorID;
};


struct CursorMask
{
	CBitmap* pAndMask;
	CBitmap* pXorMask;
};



// Declare an array of CursorMasks, which together form the animation sequence.

const INT32 nFrameCount = 9;
INT32 CurrentFrame;

CursorMask CursorSeq[nFrameCount];



// This array stores the numeric resource identifiers of each mask bitmap.

const MaskPair CursorMaskID[nFrameCount] =
{
	{ _R(IDB_AND_PROGRESS_0), _R(IDB_XOR_PROGRESS_0) },
	{ _R(IDB_AND_PROGRESS_1), _R(IDB_XOR_PROGRESS_1) },
	{ _R(IDB_AND_PROGRESS_2), _R(IDB_XOR_PROGRESS_2) },
	{ _R(IDB_AND_PROGRESS_3), _R(IDB_XOR_PROGRESS_3) },
	{ _R(IDB_AND_PROGRESS_4), _R(IDB_XOR_PROGRESS_4) },
	{ _R(IDB_AND_PROGRESS_5), _R(IDB_XOR_PROGRESS_5) },
	{ _R(IDB_AND_PROGRESS_6), _R(IDB_XOR_PROGRESS_6) },				
	{ _R(IDB_AND_PROGRESS_7), _R(IDB_XOR_PROGRESS_7) },
	{ _R(IDB_AND_PROGRESS_8), _R(IDB_XOR_PROGRESS_8) }
};



// GDI objects we use etc.

CDC* pMemoryDC1;
CDC* pMemoryDC2;
CBitmap* pAndBmp;
CBitmap* pXorBmp;
CFont* pCursorFont;

CRect TextRect;
CRect MaskRect;




/********************************************************************************************
>	BOOL InitProgressCursor()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/11/93
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if initialisation was successful.
	Purpose:	Does initial calculations and creates all the GDI objects that
				UpdateProgressCursor() requires.
	Errors:		-
	SeeAlso:	UpdateProgressCursor; DeInitProgressCursor
********************************************************************************************/

BOOL InitProgressCursor()
{
	// Load in the mask bitmap pairs which make up the cursor animation sequence.
	for (INT32 i = 0; i < nFrameCount; i++)
	{
		// Allocate two new bitmap objects on the heap.
		CursorSeq[i].pAndMask = new CBitmap;
		CursorSeq[i].pXorMask = new CBitmap;
		if (!CursorSeq[i].pAndMask || !CursorSeq[i].pXorMask)
		{
			if (IsUserName("JustinF"))
				TRACE( _T("Couldn't allocate CBitmap objects in InitProgressCursor()!\n"));
			return FALSE;
		}

		// Load the appropriate bitmaps from resources.
		if (!CursorSeq[i].pAndMask->LoadBitmap(CursorMaskID[i].AndID) ||
			!CursorSeq[i].pXorMask->LoadBitmap(CursorMaskID[i].XorID))
		{
			if (IsUserName("JustinF"))
				TRACE( _T("Couldn't load bitmap resources in InitProgressCursor()!\n"));
			return FALSE;
		}

		// In the debugging version, check for AppStudio sabotage (it converts monochrome
		// bitmaps to colour - b*st*rd!)
		#ifdef	_DEBUG
			BITMAP andinfo, xorinfo;
			CursorSeq[i].pAndMask->GetObject(sizeof(andinfo), &andinfo);
			CursorSeq[i].pXorMask->GetObject(sizeof(xorinfo), &xorinfo);
			ENSURE(andinfo.bmBitsPixel == 1 && andinfo.bmPlanes == 1 &&
				   xorinfo.bmBitsPixel == 1 && xorinfo.bmPlanes == 1,
				   "Bitmaps in InitProgressCursor() are not monochrome!\n");
		#endif	// _DEBUG
	}

	// Make two scratch bitmaps with the same size as cursors.  These bitmaps are
	// monochrome, like standard Windows cursors.
	pAndBmp = new CBitmap;
	pXorBmp = new CBitmap;
	if (!pAndBmp || !pXorBmp ||
		!pAndBmp->CreateBitmap(Cursor::Width(), Cursor::Height(), 1, 1, 0) ||
		!pXorBmp->CreateBitmap(Cursor::Width(), Cursor::Height(), 1, 1, 0))
	{
		if (IsUserName("JustinF"))
			TRACE( _T("Couldn't create scratch bitmaps in InitProgressCursor()!\n"));
		return FALSE;
	}

	// Create a font to write the percentage text with.
	LOGFONT fontinfo;
	memset(&fontinfo, 0, sizeof(fontinfo));				// all fields zero means use default values
	fontinfo.lfCharSet = UnicodeManager::GetFontCharSet();
	fontinfo.lfHeight = -Cursor::Width() / 3;

	pCursorFont = new CFont;
	if (!pCursorFont || !pCursorFont->CreateFontIndirect(&fontinfo))
	{
		if (IsUserName("JustinF"))
			TRACE( _T("Couldn't create font object in InitProgressCursor()!\n"));
		return FALSE;
	}

	// Make a device context in memory which is compatible with the screen.
	pMemoryDC1 = new CDC;
	pMemoryDC2 = new CDC;
	if (!pMemoryDC1 || !pMemoryDC2 ||
		!pMemoryDC1->CreateCompatibleDC(0) ||
		!pMemoryDC2->CreateCompatibleDC(0))
	{
		if (IsUserName("JustinF"))
			TRACE( _T("Couldn't create memory DC's in InitProgressCursor()!\n"));
		return FALSE;
	}

	// Find out how large the font we created REALLY is, so we can calculate where within
	// the cursor we can write the text.
	TEXTMETRIC textinfo;
    CFont* pf = pMemoryDC1->SelectObject(pCursorFont);
	pMemoryDC1->GetTextMetrics(&textinfo);
	pMemoryDC1->SelectObject(pf);

	// The text appears at the bottom of the cursor.  Calculate its bounding rectangle, which
	// includes room for a two-pixel wide border
	TextRect.left	= 0;
	TextRect.top 	= Cursor::Height() - textinfo.tmAscent;
	TextRect.right	= Cursor::Width();
	TextRect.bottom = Cursor::Height();

	// The mask bitmaps are stretched into the remaining square space.
	MaskRect.top	= 0;
	MaskRect.bottom	= Cursor::Height() - textinfo.tmHeight;
	MaskRect.left	= Cursor::Width() / 2 - MaskRect.bottom / 2;
	MaskRect.right	= Cursor::Width() / 2 + MaskRect.bottom / 2;

	// Finally, set the animation to begin with the first frame and . . . success!
	CurrentFrame = 0;
	return TRUE;
}





/********************************************************************************************
>	Cursor* UpdateProgressCursor(INT32 percent)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com> (Messed around by Jason)
	Created:	19/11/93
	Inputs:		percent - 	A value between 0% and 100% to be displayed under the pointer, OR
							-1 to indicate that no percentage should be displayed.
	Outputs:	-
	Returns:	A pointer to the new progress cursor object if successful, NULL otherwise.
	Purpose:	(If percent >= 0 it draws the percentage text into the cursor masks and then)
				creates and sets a new cursor.
	Errors:		-
	SeeAlso:	InitProgressCursor; DeInitProgressCursor
********************************************************************************************/

Cursor* UpdateProgressCursor(INT32 percent)
{
	// Before we get too confused, the truth table for the combination of the 2 bitmaps is:
	//
	// 		ANDbmp	XORbmp	Result in cursor
	//		Black	Black	Black
	//		Black	White	White
	//		White	Black	Transparent
	//		White	White	Invert screen colour at this point
	//
	// First, we set AND/XOR to White/Black respectively so that the cursor is transparent...

    CBitmap* poldbmp1 = pMemoryDC1->SelectObject(pAndBmp);
	pMemoryDC1->PatBlt(0, 0, Cursor::Width(), Cursor::Height(), WHITENESS);

	CBitmap* poldbmp2 = pMemoryDC2->SelectObject(pXorBmp);
	pMemoryDC2->PatBlt(0, 0, Cursor::Width(), Cursor::Height(), BLACKNESS);

	// Find out how large the mask bitmaps are (they are assumed to all be of the same size).
	BITMAP bmpinfo;
	CursorSeq[CurrentFrame].pAndMask->GetObject(sizeof(bmpinfo), &bmpinfo);

	// Stretch the cursor masks for the current animation frame into the scratch bitmaps.
	// First the XOR mask, then the AND mask.
	pMemoryDC2->SelectObject(CursorSeq[CurrentFrame].pXorMask);			// source
	pMemoryDC1->SelectObject(pXorBmp);									// destination
	pMemoryDC1->StretchBlt(MaskRect.left, MaskRect.top, MaskRect.Width(), MaskRect.Height(),
						   pMemoryDC2, 0, 0, bmpinfo.bmWidth, bmpinfo.bmHeight, SRCCOPY);

	pMemoryDC2->SelectObject(CursorSeq[CurrentFrame].pAndMask);			// source
	pMemoryDC1->SelectObject(pAndBmp);									// destination
	pMemoryDC1->StretchBlt(MaskRect.left, MaskRect.top, MaskRect.Width(), MaskRect.Height(),
						   pMemoryDC2, 0, 0, bmpinfo.bmWidth, bmpinfo.bmHeight, SRCCOPY);

	if (percent >= 0)		// Only display percentage text if 'percent' >= 0
	{
		// Next, write the percentage text inside the text rectangle.
		// We want this to be black text on a white rub-out box, so we plot black-on-white text
		// into the XOR bitmap, and black-on-black into the AND bitmap (rather than plotting a
		// rectangle, because we want the rub-out box to be only slightly bigger than the text
		// sitting upon it...)
		pMemoryDC1->SelectObject(pXorBmp);

		pMemoryDC1->SetTextColor(RGB(0, 0, 0));
		pMemoryDC1->SetBkColor(RGB(255, 255, 255));

		CFont	*poldfont1 = pMemoryDC1->SelectObject(pCursorFont);
		TCHAR	txt[8];						// Cache - percentage text
		INT32		numchars;					// Cache - number of chars in the above %age text

		String_32 jcf(_R(IDS_PERCENT_FORMAT));
		numchars = ::wsprintf(txt, jcf, (INT32) percent);
		pMemoryDC1->DrawText(txt, numchars, &TextRect,
							 DT_SINGLELINE | DT_CENTER | DT_VCENTER | DT_NOPREFIX);

		pMemoryDC1->SelectObject(pAndBmp);
		pMemoryDC1->SetTextColor(RGB(0, 0, 0));
		pMemoryDC1->SetBkColor(RGB(0, 0, 0));

		pMemoryDC1->DrawText(txt, numchars, &TextRect,
							 DT_SINGLELINE | DT_CENTER | DT_VCENTER | DT_NOPREFIX);

		pMemoryDC1->SelectObject(poldfont1);
	}

	// Deselect the bitmaps.
	pMemoryDC1->SelectObject(poldbmp1);
	pMemoryDC2->SelectObject(poldbmp2);

	// Finally, create and set the new cursor.
	Cursor* pnc = new Cursor(pAndBmp, pXorBmp);
	if (!pnc || !pnc->IsValid())
	{
		if (IsUserName("JustinF"))
			TRACE( _T("Couldn't create new Cursor object in UpdateProgressCursor()!\n"));
		delete pnc;
		return 0;
	}

	// Update the animation frame counter.
	if (++CurrentFrame >= nFrameCount) CurrentFrame = 0;
	return pnc;
}





/********************************************************************************************
>	void DeInitProgressCursor()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/11/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Deallocates all objects created in InitProgressCursor()
	Errors:		-
	SeeAlso:	InitProgressCursor; UpdateProgressCursor
********************************************************************************************/

void DeInitProgressCursor()
{
	// Deallocate the mask bitmaps.
	for (INT32 i = 0; i < nFrameCount; i++)
	{
		CursorSeq[i].pAndMask->DeleteObject();
		CursorSeq[i].pXorMask->DeleteObject();
		delete CursorSeq[i].pAndMask;
		delete CursorSeq[i].pXorMask;
	}

	// Destroy the other GDI objects.
	pMemoryDC1->DeleteDC();
	delete pMemoryDC1;

	pMemoryDC2->DeleteDC();
	delete pMemoryDC2;

	pAndBmp->DeleteObject();
	delete pAndBmp;

	pXorBmp->DeleteObject();
	delete pXorBmp;

	delete pCursorFont;
}




/********************************************************************************************
>	BOOL BreakKeyHit()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/93
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the Escape key is pressed.
	Purpose:	Checks if the user is trying to interrupt or cancel a task.
	Errors:		-
	SeeAlso:	ContinueSlowJob
********************************************************************************************/

BOOL BreakKeyHit()
{
	return KeyPress::IsEscapePressed();
/*
	if (::GetAsyncKeyState(CAMKEY(ESCAPE)) <0 )
	{
		MSG msg;
		BOOL Again;
		// if Esc is pressed down, lets throw away all key messages otherwise the Esc is likely
		// to get passed on e.g. to the next dialog box
		do
		{
			Again = PeekMessage( &msg, NULL, WM_KEYFIRST, WM_KEYLAST, PM_REMOVE );
		} while (Again);

		return TRUE;
	}
	return FALSE;
*/
}




/********************************************************************************************

>	void Beep()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/11/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Makes a beep sound, stupid!
	Errors:		-
	SeeAlso:	BreakKeyHit; ContinueSlowJob

********************************************************************************************/

void Beep()
{
	::MessageBeep(MB_OK);
}
