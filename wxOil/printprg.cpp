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
//
// PrintProgressDlg - implements the print progress dialog displayed during printing

/*
*/

#include "camtypes.h"

//	WEBSTER-ranbirr-12/11/96
#ifndef WEBSTER

#include "printprg.h"
//#include "printdlg.h"
//#include "errors.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "custmsg.h"
//#include "markn.h"
//#include "simon.h"
//#include "andy.h"
//#include "fixst256.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "camelot.h"
//#include "justin2.h"

DECLARE_SOURCE("$Revision$");

#define new CAM_DEBUG_NEW

PrintProgressDlg* PrintProgressDlg::pPrintProgressDlg = NULL;

const INT32 SLIDER_MAX = 0x08000000;

// Fix for MFC4 - names are missing or slightly changed so we replace them as for MFC3.
#ifndef AfxGetWinState
#define AfxGetWinState() _afxWinState
#define AFX_WIN_STATE _AFX_WIN_STATE
#endif


//--------------------------------------------

PrintProgressDlg::PrintProgressDlg(CWnd* pParent)
{
	// Init variables to semi-sensible defaults
	SliderMax = SLIDER_MAX;
	SliderCurPercent = -1;
	SliderSubRangeMax = SLIDER_MAX;
	SliderSubRangeBase = 0;
	SliderSubRangeStep = 1;


	Create(_R(IDD_PRINTPROGRESS), pParent);      // modeless !
#ifndef RALPH
	AfxGetWinState()->m_bUserAbort = FALSE;
#endif

	ERROR3IF(pPrintProgressDlg != NULL,"Static pPrintProgressDlg is NOT NULL");
	pPrintProgressDlg = this;

	SetSliderMax(SLIDER_MAX);
	SetSliderPos(0);
	Printing = FALSE;
	IgnoreUpdates = FALSE;
}

//--------------------------------------------

BOOL PrintProgressDlg::OnInitDialog()
{
//	SetWindowText(AfxGetAppName());		-- no, "Printing" is a better title than "Camelot", thanks... 
	CenterWindow();
	IgnoreUpdates = FALSE;
	return CDialog::OnInitDialog();
}

//--------------------------------------------

void PrintProgressDlg::OnCancel()
{
	if (CCamApp::DisableSys)
		return;

#ifndef RALPH
	if (!AfxGetWinState()->m_bUserAbort)
	{
		INT32 b = InformWarning(_R(IDS_PRINT_SUSPEND),_R(IDS_ABORT),_R(IDS_CONTINUE));

		if (b == 1)
		{
			AfxGetWinState()->m_bUserAbort = TRUE;  // flag that user aborted print
			CDialog::OnCancel();
		}
	}
	else
#endif
	{
		CDialog::OnCancel();
	}
}

//--------------------------------------------

void PrintProgressDlg::SetSliderMax(INT32 max)
{
	SliderMax 	  	 = max;
	SliderCurPercent = -1;

	CWnd* pGadget = GetDlgItem(_R(IDC_PRINTPROGSLIDER));
	
	if (pGadget != NULL)
	{
		::SendMessage(pGadget->GetSafeHwnd(),WM_SET_MIN,0,LPARAM(0));
		::SendMessage(pGadget->GetSafeHwnd(),WM_SET_MAX,0,LPARAM(max));
	}
}

//--------------------------------------------

void PrintProgressDlg::SetSliderPos(INT32 pos)
{
	if (pos == -1)
		pos = SliderMax;

	if (pos < 0) pos = 0;
	if (pos > SliderMax) pos = SliderMax;

	INT32 p;
	double ratio = double(SliderMax) / 100.0;
	if (ratio != 0.0)
		p = INT32 ((double(pos) / ratio)+0.5);
	else
		p = 0;

	if (p != SliderCurPercent)
	{
		SliderCurPercent = p;

		CWnd* pGadget = GetDlgItem(_R(IDC_PRINTPROGSLIDER));
	
		if (pGadget != NULL)
		{
			::SendMessage(pGadget->GetSafeHwnd(),WM_SET_POSITION,0,LPARAM(pos));
			::SendMessage(pGadget->GetSafeHwnd(),WM_PAINT,0,0);
		}

		String_32 Percent;
		Percent.MakeMsg(_R(IDS_PRINT_PROG_PERCENT), (INT32) p);
		SetDlgItemText(_R(IDC_CURRENTPERCENTAGE), (TCHAR *)Percent);
	}
}

//--------------------------------------------

void PrintProgressDlg::SetSliderSubRangeMax(INT32 max)
{
	if (max < 1) max = 1;
	SliderSubRangeMax = max;
}

//--------------------------------------------

void PrintProgressDlg::SetSliderSubRangePos(INT32 pos)
{
	if (pos > SliderSubRangeMax) pos = SliderSubRangeMax;
	if (pos < 0) 				 pos = 0;

	double ratio = double(pos)/double(SliderSubRangeMax);

	SetSliderPos(SliderSubRangeBase+INT32(double(SliderSubRangeStep)*ratio));
}

//--------------------------------------------

void PrintProgressDlg::Show()
{
	ShowWindow(SW_SHOW);
	UpdateWindow();
}

//--------------------------------------------

void PrintProgressDlg::SetDocName(LPCTSTR pDocName)
{
	if (pDocName != NULL)
		SetDlgItemText(_R(IDC_DOCNAME),  (TCHAR*)pDocName);
}

//--------------------------------------------

void PrintProgressDlg::SetPrinterName(LPCTSTR pPrinterName)
{
	if (pPrinterName != NULL)
		SetDlgItemText(_R(IDC_PRINTERNAME), pPrinterName);
}

//--------------------------------------------

void PrintProgressDlg::SetPortName(LPCTSTR pPortName)
{
// Phil has decided that the port name is a bit too tecchie and not
// really very useful info, so we've removed it to simplify the dialog
//
//	if (pPortName != NULL)
//		SetDlgItemText(_R(IDC_PORTNAME), pPortName);
}

//--------------------------------------------

/********************************************************************************************

>	void PrintProgressDlg::SetPageNumber(INT32 PageNumber,  INT32 MaxPageNumber,
									 INT32 PlateNumber, INT32 MaxPlateNumber, LPCTSTR pPlateName,
									 INT32 TileNumber,  INT32 MaxTileNumber)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>/Jason
	Created:	? (93 or 94 ish)
	Inputs:		PageNumber		- The page being printed in the range 1..MaxPageNumber
				MaxPageNumber	- Total number of pages being printed (minimum of 1)
				PlateNumber		- If separating, the current plate number in range 1..MaxPlateNumber
									else ignored
				MaxPlateNumber	- If separating, the number of plates being printed (minimum of 1)
								  If not separating, this MUST be set to 0.
				pPlateName		- If separating, the name of the plate,
									else NULL
				TileNumber		- The current tile in the range 1..MaxTileNumber
				MaxTileNumber	- The number of tiles being printed (minimum of 1)
	Returns:	-
	Purpose:	Dumps the last error returned by GetLastError() using the FormatMessage() command
	SeeAlso:	CPrintDialog::DoModal

********************************************************************************************/

void PrintProgressDlg::SetPageNumber(INT32 PageNumber,  INT32 MaxPageNumber,
									 INT32 PlateNumber, INT32 MaxPlateNumber, LPCTSTR pPlateName,
									 INT32 TileNumber,  INT32 MaxTileNumber)
{
	// If the dlg is tem,porarily locked to stop screen updates, just return immediately.
	if (IgnoreUpdates)
		return;

	String_256 Str;

	// Set the page info
	Str.MakeMsg(_R(IDS_PRINT_PROG_PAGE), PageNumber, MaxPageNumber);
	SetDlgItemText(_R(IDC_PAGENUMBER), (TCHAR*)Str);


	// Set the plate info
	if (MaxPlateNumber > 0 && pPlateName != NULL)
		Str.MakeMsg(_R(IDS_PRINT_PROG_PLATE), PlateNumber, MaxPlateNumber, pPlateName);
	else
		Str.MakeMsg(_R(IDS_PRINT_PROG_COMPOSITE));

	SetDlgItemText(_R(IDC_PLATENUMBER), (TCHAR *)Str);


	// Set the tile info
	if (MaxTileNumber > 1)
	{
		Str.MakeMsg(_R(IDS_PRINT_PROG_TILE), TileNumber, MaxTileNumber);
		SetDlgItemText(_R(IDC_TILENUMBER), (TCHAR *)Str);
	}

	// If there is no tile info, we hide both the tile gadgets
	CWnd *Gadget = GetDlgItem(_R(IDC_TILETITLE));
	if (Gadget)
		Gadget->ShowWindow((MaxTileNumber <= 1) ? SW_HIDE : SW_SHOWNORMAL);
	Gadget = GetDlgItem(_R(IDC_TILENUMBER));
	if (Gadget)
		Gadget->ShowWindow((MaxTileNumber <= 1) ? SW_HIDE : SW_SHOWNORMAL);

	// Make sure we don't get a div by zero or a nasty negative in the calculations below
	if (MaxPageNumber < 1)		MaxPageNumber = 1;
	if (PageNumber < 1)			PageNumber = 1;
	if (MaxPlateNumber < 1)		MaxPlateNumber = 1;
	if (PlateNumber < 1)		PlateNumber = 1;
	if (MaxTileNumber < 1)		MaxTileNumber = 1;
	if (TileNumber < 1)			TileNumber = 1;

	// And set the slider position accordingly - divide the 100% up into ranges for each page,
	// subdivided into ranges for each plate, subdivided into ranges for each tile.
	INT32 SliderPerPage  = SliderMax / MaxPageNumber;
	INT32 SliderPerPlate = SliderPerPage / MaxPlateNumber;
	INT32 SliderPerTile  = SliderPerPlate / MaxTileNumber;

	SliderSubRangeBase = (SliderPerPage  * (PageNumber-1))  +
						 (SliderPerPlate * (PlateNumber-1)) +
						 (SliderPerTile  * (TileNumber-1));
	SliderSubRangeStep = SliderPerTile;

	SetSliderPos(SliderSubRangeBase);
}



//--------------------------------------------
//--------------------------------------------

BOOL CALLBACK PrintProgressDlg::AbortProc(HDC,INT32 code)
{
	if (CCamApp::DisableSys)
		// Ignore
		return TRUE;

#ifndef RALPH
	AFX_WIN_STATE* pWinState = AfxGetWinState();
	MSG msg;

   	while (!pWinState->m_bUserAbort &&
   		::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) 
   	{
		// Do we have a printing dialog?
		if (pPrintProgressDlg != NULL)
		{
	        // Yes - process any messages for the Cancel dialog box.
	        if (!pPrintProgressDlg->IsDialogMessage(&msg)) 
    	    {
        	    TranslateMessage((LPMSG) &msg);
            	DispatchMessage((LPMSG) &msg);
	        }
		}
		else
		{
			TranslateMessage((LPMSG) &msg);
			DispatchMessage((LPMSG) &msg);
		}
    }

	// If the code is 0, there's no error, otherwise an error has occurred.
	// The error could be a SP_USERABORT generated from the print manager
	if (code != NULL && !pWinState->m_bUserAbort)
	{
		pWinState->m_bUserAbort = TRUE;
		if (pPrintProgressDlg != NULL)
			pPrintProgressDlg->EndDialog(IDCANCEL);
	}		

	return !pWinState->m_bUserAbort;
#else
	return TRUE;
#endif
}

#endif //webster
