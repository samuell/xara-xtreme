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
// The Cursor class (platform-dependent implementation).

/*
*/


#include "camtypes.h"

//#include "cursor.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "ensure.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "oiltool.h"
//#include "resource.h"
//#include "tool.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "errors.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "camelot.h"
#include "csrstack.h"
#include "cartprov.h"
//#include "docview.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "camframe.h"

//#include "hotlink.h"  //For the pointing hand cursor

DECLARE_SOURCE("$Revision$");

// Memory dump info
CC_IMPLEMENT_MEMDUMP(Cursor, CC_CLASS_MEMDUMP)
#define new CAM_DEBUG_NEW


// Pointers to our "built-in" cursors.
Cursor* Cursor::Arrow;								// the bog-standard pointer
Cursor* Cursor::Busy;								// the hour-glass
Cursor* Cursor::Insert;								// the I-beam
Cursor* Cursor::CrossHair;							// the crosshair
Cursor* Cursor::Progress;							// special animated percentage counter
Cursor* Cursor::SizeNWSE;							// resize north-west south-east

#ifdef RALPH
	Cursor* Cursor::PointingHand;					// Netscape-style pointing hand
#endif



/********************************************************************************************

>	static BOOL Cursor::Init()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/11/93
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if initialisation successful.
	Purpose:	Loads some standard GUI cursors from the operating system resources.  Only
				three are found on most platforms - the arrow, the hour-glass, and the
				insertion beam.
	Errors:		-
	SeeAlso:	Cursor::DeInit; CursorStack::Init

********************************************************************************************/

BOOL Cursor::Init()
{
	Arrow	  = new Cursor( wxCURSOR_ARROW );
	Busy	  = new Cursor( wxCURSOR_WAIT );
	Insert	  = new Cursor( wxCURSOR_IBEAM );
	CrossHair = new Cursor( wxCURSOR_CROSS );
	SizeNWSE  = new Cursor( wxCURSOR_SIZENWSE );
	Progress  = 0;

#ifdef RALPH
	//Graham 6/9/96: This is used in Ralph's No Tool mode. The No Tool mode is
	//implemented in DocView, but we can't initiate a cursor there, because we end up
	//creating a new cursor every time a new view is created.
	PointingHand = new Cursor(_R(IDC_POINTINGHANDCURSOR));
#endif //RALPH

	return Arrow != 0
		&& Busy != 0
		&& Insert != 0
		&& CrossHair != 0
		&& SizeNWSE != 0
		&& Arrow->IsValid()
		&& Busy->IsValid()
		&& Insert->IsValid()
		&& CrossHair->IsValid()
		&& SizeNWSE->IsValid();
}




/********************************************************************************************

>	static void Cursor::DeInit()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/11/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Deallocates the Cursor objects created by Init().
	Errors:		-
	SeeAlso:	Cursor::Init; CursorStack::DeInit

********************************************************************************************/

void Cursor::DeInit()
{
	delete Arrow;
	delete Busy;
	delete Insert;
	delete CrossHair;
	delete SizeNWSE;

	Arrow = Busy = Insert = CrossHair = SizeNWSE = 0;

#ifdef RALPH
	delete PointingHand;
	PointingHand = 0;
#endif
	
}




/********************************************************************************************

>	Cursor::Cursor(LPCSTR lpStdCsrName)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/11/93
	Inputs:		The (string) identifier of a standard Windows cursor, eg. _R(IDC_WAIT).
	Outputs:	-
	Returns:	-
	Purpose:	Loads a standard cursor from the operting system's resources.
	Errors:		Will trap in debugging version if the load fails.
	SeeAlso:	-

********************************************************************************************/

Cursor::Cursor(LPCSTR lpStdCsrName)
  :	hCursor(0), bMustDestroy(FALSE)				// we must not try to destroy standard cursors!
{
	PORTNOTETRACE("other","Cursor::Cursor(LPCSTR) - do nothing");
	// Try to load the cursor (it is likely to be loaded already).
//	hCursor = wxCursor(lpStdCsrName);

/*	hCursor = AfxGetApp()->LoadStandardCursor(lpStdCsrName);
	if (hCursor == 0)
	{
		if (IsWin32c() && lpStdCsrName == _R(IDC_SIZE))
		{
			// ChicagoBodge
			// when marked as a 4.0 .exe it is unable to load _R(IDC_SIZE) or _R(IDC_ICON) resources, so we fall
			// back to _R(IDC_SIZEALL)
			hCursor = AfxGetApp()->LoadStandardCursor( _R(IDC_SIZEALL) );
		}

	#ifdef RALPH
		if(hCursor == 0)
		{
			TRACEUSER( "JustinF", _T("Cursor '%s' not found\n"), lpStdCsrName);
			hCursor = AfxGetApp()->LoadStandardCursor(_R(IDC_CROSS));
		}
	#endif
		
		if (hCursor == 0)
			ERROR3_PF(("Failed to load standard cursor %u", (INT32) LOWORD(lpStdCsrName)));
	}
*/
}




/********************************************************************************************

>	Cursor::Cursor(UINT32 wCsrID)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/11/93
	Inputs:		The numeric identifier of the cursor in the kernel's resources.
	Outputs:	-
	Returns:	-
	Purpose:	Loads the specified cursor from the kernel's application module.
	Errors:		ENSURE failure if the cursor can't be loaded.
	SeeAlso:	-

********************************************************************************************/

Cursor::Cursor(UINT32 wCsrID)
  : bMustDestroy(FALSE)
{
    if (wCsrID > 0 && wCsrID <= wxCURSOR_MAX)
	{
		// Simply set the standard wxCursor
		hCursor = wxCursor(wCsrID);
	}
	else
	{
		wxImage		   *pImage = (CamArtProvider::Get())->FindImage( wCsrID );
		ENSURE( NULL != pImage, "Failed to find cursor in Cursor::Cursor!\n");

		if (NULL == pImage)
		{
			hCursor = wxCursor(wxCURSOR_CROSS);
			return;
		}

		// Try to load the specified cursor from the OILTool's module.
		hCursor = wxCursor(*pImage);
	}

//	// Check that the resource ID is valid for a kernel resource.
//
// We no longer do this, as we are now using some non-system cursors which have
// IDs outside this range.
//
//	ENSURE(wCsrID >= KID_FIRST && wCsrID <= KID_LAST,
//		   "Kernel cursor has out-of-range resource ID!\n");

	// Load from the application's global (ie. kernel) resources.
//	hCursor = wxCursor(wCsrID);
/*
	hCursor = ::LoadCursor(AfxGetResourceHandle(), MAKEINTRESOURCE(wCsrID));

#ifdef RALPH
	if(hCursor == 0)
	{
		TRACEUSER( "JustinF", _T("Cursor '%d' not found 2\n"), wCsrID);
		hCursor = AfxGetApp()->LoadStandardCursor(_R(IDC_CROSS));
	}
#endif
*/
//	ENSURE(hCursor != 0, "Failed to load a kernel cursor in Cursor::Cursor!\n");
}




/********************************************************************************************

>	Cursor::Cursor(Tool* pTool, UINT32 wCsrID)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/11/93
	Inputs:		A pointer to the tool associated with the desired cursor and the numeric
				resource identifier of the particular cursor.
	Outputs:	-
	Returns:	-
	Purpose:	Loads the specified cursor from the specified tool.
	Errors:		In the debugging version traps a failure to load.
	SeeAlso:	-

********************************************************************************************/

Cursor::Cursor(Tool_v1*, UINT32 wCsrID)
  :	bMustDestroy(FALSE)
{
	wxImage		   *pImage = (CamArtProvider::Get())->FindImage( wCsrID );
	ENSURE( NULL != pImage, "Failed to find cursor in Cursor::Cursor!\n");

	if( NULL == pImage )
	{
		hCursor = wxCursor( wxCURSOR_CROSS );
		return;
	}

	// Try to load the specified cursor from the OILTool's module.
	hCursor = wxCursor( *pImage );
}




/********************************************************************************************

>	Cursor::Cursor(UINT32 wToolID, UINT32 wCsrID)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/11/93
	Inputs:		The ID of the tool associated with the desired cursor and the numeric
				resource identifier of the particular cursor.
	Outputs:	-
	Returns:	-
	Purpose:	Loads the specified cursor from the specified tool.
	Errors:		In the debugging version traps a failure to load.
	SeeAlso:	-

********************************************************************************************/

Cursor::Cursor(UINT32 wToolID, UINT32 wCsrID)
  :	hCursor(wxCURSOR_ARROW), bMustDestroy(FALSE)
{
	wxImage		   *pImage = (CamArtProvider::Get())->FindImage( wCsrID );
	ENSURE( NULL != pImage, "Failed to find cursor in Cursor::Cursor!\n");

	if( NULL == pImage )
	{
		hCursor = wxCursor( wxCURSOR_CROSS );
		return;
	}

	// Try to load the specified cursor from the OILTool's module.
	hCursor = wxCursor( *pImage );
	
//	PORTNOTETRACE("other","Cursor::Cursor - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	// Check that the resource ID is in the valid range for tools.
/*	ENSURE(wCsrID < KID_FIRST || wCsrID > KID_LAST,
		   "Tool cursor has out-of-range resource ID!\n");*/
	// Find the OILTool object associated with this tool.
	OILTool* ptOil = Tool::GetOILTool(wToolID);
	ENSURE(ptOil != 0, "Failed to find OIL tool in Cursor::Cursor!\n");
	
	// Try to load the specified cursor from the OILTool's module.
	hCursor = ptOil->LoadCursor(wCsrID);

#ifdef RALPH
	if(hCursor == 0)
	{
		TRACEUSER( "Richard", _T("Cursor '%d' not found 4\n"), wCsrID);
		hCursor = AfxGetApp()->LoadStandardCursor(_R(IDC_CROSS));
	}
#endif

	ENSURE(hCursor != 0, "Failed to load a tool's cursor in Cursor::Cursor!\n");
#endif
}




/********************************************************************************************

>	Cursor::Cursor(CBitmap* pbmpAND, CBitmap* pbmpXOR)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/11/93
	Inputs:		Two pointers to the bitmaps which are used as masks when creating the
				cursor.
	Outputs:	-
	Returns:	-
	Purpose:	Creates a cursor from two bitmap masks (AND & XOR).  See the Windows
				SDK documentation for the CreateCursor() function to understand how
				this is done.
	Errors:		Checks that the bitmaps are of the same size as cursors.
	SeeAlso:	-

********************************************************************************************/

Cursor::Cursor(wxBitmap* pbmpAND, wxBitmap* pbmpXOR)
  :	hCursor(0), bMustDestroy(TRUE)								// destroy any cursor we create
{
	PORTNOTETRACE("other","Cursor::Cursor(wxBitmap*, wxBitmap*) - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	// Make the cursor from the bitmaps.  The bitmaps must be of the correct size.
	BITMAP andInfo;
	pbmpAND->GetObject(sizeof(BITMAP), &andInfo);

    // In the debug version make this check for both parameters.
	INT32 w = Width();
	INT32 h = Height();

#ifdef	_DEBUG
    BITMAP xorInfo;
    pbmpXOR->GetObject(sizeof(BITMAP), &xorInfo);
	ENSURE((andInfo.bmWidth == w) && (andInfo.bmHeight == h) &&
		   (xorInfo.bmWidth == w) && (xorInfo.bmHeight == h),
		   "Mask bitmaps are not cursor-sized in Cursor::Cursor!\n");
#endif	// _DEBUG
    
    // Copy the pixel data of the two masks into buffers allocated on the heap.
    size_t dwSize = andInfo.bmWidthBytes * andInfo.bmHeight;
	LPWORD lpwAndBits = new WORD[dwSize / 2 + 1];
	LPWORD lpwXorBits = new WORD[dwSize / 2 + 1];
	if (!lpwAndBits ||
		!lpwXorBits ||
		!pbmpAND->GetBitmapBits(dwSize, lpwAndBits) ||
		!pbmpXOR->GetBitmapBits(dwSize, lpwXorBits))
	{
	#ifdef	_DEBUG
		if (IsUserName("JustinF"))
			TRACE( _T("Failed to allocate heap space for mask buffers in Cursor::Cursor!\n"));
	#endif	// _DEBUG

		delete[] lpwAndBits;
		delete[] lpwXorBits;
		return;
	}

	// Now make the cursor.
    hCursor = ::CreateCursor(AfxGetApp()->m_hInstance,
    						 w / 2, h / 2, w, h,
    						 lpwAndBits, lpwXorBits);
	#ifdef	_DEBUG
		if (hCursor == 0 && IsUserName("JustinF"))
			TRACE( _T("Failed to create cursor in Cursor::Cursor!\n"));
	#endif	// _DEBUG

	// Finally, deallocate all resources.
	delete[] lpwAndBits;
	delete[] lpwXorBits;
#endif
}




/********************************************************************************************

>	virtual Cursor::~Cursor()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/11/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Destroys a cursor (if necessary).
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

Cursor::~Cursor()
{
//	if (hCursor != 0 && bMustDestroy) ::DestroyCursor(hCursor);
}




/********************************************************************************************

>	BOOL Cursor::IsValid() const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/11/93
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the Cursor object was successfully loaded/created.
	Purpose:	Tests that this object was constructed without any errors.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL Cursor::IsValid() const
{
	PORTNOTETRACE( "other", "Cursor::IsValid - pretend cursor is valid even though not supported" );
#ifndef EXCLUDE_FROM_XARALX
	return !hCursor.IsNull();
#else
	return true;
#endif
}




/********************************************************************************************

>	void Cursor::SetActive() const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/11/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Makes this Cursor object the active operating system cursor.
	Errors:		If the cursor could not be loaded or created, this call does nothing.
	SeeAlso:	Cursor::Show; Cursor::Hide

********************************************************************************************/

void Cursor::SetActive() const
{
	// If we have captured the mouse then release it
	wxWindow* pCaptureWnd = wxWindow::GetCapture();
	if (pCaptureWnd)
		pCaptureWnd->ReleaseMouse();

	// Set the global cursor (but only if we have a Render window to
	// control its scope)
	wxWindow* pRenderWnd = DocView::GetCurrentRenderWindow();
	if( NULL != pRenderWnd )
		wxSetCursor(hCursor);

	// If we have a RenderWindow and it doesn't have the capture then set its cursor
	if( pRenderWnd != NULL && pCaptureWnd != pRenderWnd)
		pRenderWnd->SetCursor(hCursor);

	// If we did have the capture
	if (pCaptureWnd)
	{
		// Set the cursor on the relevant window and recapture the mouse
		pCaptureWnd->SetCursor(hCursor);
		pCaptureWnd->CaptureMouse();
	}

#if FALSE	// Phil's version
	// This works around various problems concerning cursor shape changing while the mouse is captured by gdk_pointer_grab
	// Note1: At the time of writing wxWindow::DoCaptureMouse won't use the global cursor if the local one is unset so we need to set both versions
	// Note2: At the time of writing gdk_pointer_grab seems to prevent cursor changes
	CNativeWnd* pRenderWnd = DocView::GetCurrentRenderWindow();
	if (pRenderWnd && pRenderWnd->HasCapture())
	{
		pRenderWnd->ReleaseMouse();
		wxSetCursor(hCursor);
		pRenderWnd->SetCursor(hCursor);
		pRenderWnd->CaptureMouse();
	}
	else
	{
		wxSetCursor(hCursor);
		if (pRenderWnd)
			pRenderWnd->SetCursor(hCursor);
	}
#endif
}




/********************************************************************************************

>	static BOOL Cursor::Show()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/11/93
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the cursor is visible after this function call.
	Purpose:	Increments the cursor's semaphore, generally making the cursor visible.
	Errors:		-
	SeeAlso:	Cursor::Hide

********************************************************************************************/

BOOL Cursor::Show()
{
//	return ::ShowCursor(TRUE) >= 0;
	SetActive();
	return true;
}




/********************************************************************************************

>	static BOOL Cursor::Hide()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/11/93
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the cursor is no longer visible.
	Purpose:	Decrements the cursor's semaphore, generally making the cursor invisible.
	Errors:		-
	SeeAlso:	Cursor::Show

********************************************************************************************/

BOOL Cursor::Hide()
{
//	return ::ShowCursor(FALSE) < 0;
	::wxSetCursor(wxNullCursor);
	return true;
}




/********************************************************************************************

>	static INT32 Cursor::Width()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/11/93
	Inputs:		-
	Outputs:	-
	Returns:	The width of a cursor, in pixels, for this system & video mode.
	Purpose:	(see above)
	Errors:		-
	SeeAlso:	Cursor::Height

********************************************************************************************/

INT32 Cursor::Width()
{
	return wxSystemSettings::GetMetric( wxSYS_CURSOR_X );
}




/********************************************************************************************

>	static INT32 Cursor::Height()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/11/93
	Inputs:		-
	Outputs:	-
	Returns:	The height of a cursor, in pixels, for this system & video mode.
	Purpose:	(see above)
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

INT32 Cursor::Height()
{
	return wxSystemSettings::GetMetric( wxSYS_CURSOR_Y );
}
