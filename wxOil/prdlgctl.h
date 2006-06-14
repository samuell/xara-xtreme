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
// The Print Dialog Class

#ifndef INC_PRDLGCTL
#define INC_PRDLGCTL

//#include <afxdlgs.h>	// Contains the def of CPrintDialog
#include "printctl.h"	// Defines PrintControl
#include "printprg.h"	// The print progress dialogue

class Document;
//	WEBSTER-ranbirr-12/11/96
#ifndef WEBSTER
class CCPrintDialog;
#endif //webster
class PrintPatchInfo;
//	WEBSTER-ranbirr-12/11/96
#ifndef WEBSTER
class PrintProgressDlg;
#endif //webster
class CDC;
class CCamView;

/********************************************************************************************

>	class CCPrintInfo : public CPrintInfo

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/3/95
	Purpose:	To provide a custom Print Info class

********************************************************************************************/
//	WEBSTER-ranbirr-12/11/96
#ifndef WEBSTER

class CCPrintInfo : public CPrintInfo
{
public:
	CCPrintInfo();	// Does nothing - do not use this constuctor
	CCPrintInfo(Document* pDoc,CCamView* pCCamView);

	~CCPrintInfo();

	BOOL StartPrinting();
	BOOL MorePaper() const;
	BOOL GetNextPaper();
	BOOL GetNextPatch(PrintPatchInfo* pPatchInfo);
	BOOL ReprintPaper();
	BOOL EndPrinting();

	BOOL StartPlatePrinting(PrintView *pPrintView, UINT32 *const pErrID);
	BOOL MorePlates();
	BOOL SetNextPlate(PrintView *pPrintView);
	void EndPlatePrinting(PrintView *pPrintView);

	void SetSliderSubRangeMax(INT32 Max);
	void SetSliderSubRangePos(INT32 Pos);

	void SetAnalysing();
	void SetPrinting();

	INT32  SetAbortProc(CDC* pCDC);			// Calls pCDC->SetAbortProc() with the appropriate abort proc
	BOOL Abort();							// TRUE if user has aborted the print job, FALSE otherwise
	BOOL SetUpDocInfo(DOCINFO** pDocInfo);	// Sets up the DOCINFO ready for a call to CDC::StartDoc()

	PrintControl *GetPrintControl()		{ return pPrCtrl;  }

	static CCPrintInfo* GetCurrent()	{ return pCurrent; }

	// Locks/Unlocks the print progress dialogue (if any) to make sure it doesn't update
	// the progress slider etc during some critical operation.
	void LockProgressUpdate(BOOL Locked) { if (pPrgDlg != NULL) pPrgDlg->LockProgressUpdate(Locked); }

private:
	BOOL 			Initialised;	// TRUE if Init() has been successfully called
	BOOL			Printing;		// TRUE after StartPrinting() called - FALSE after EndPrinting()
	CCPrintDialog*	pOurPD;			// Ptr to one of our own print dialogs (derived from CPrintDialog)
	CPrintDialog*	pOriginalPD;	// Original dlg created by base class constructor
	Document*		pDocument;		// Ptr to associated document
	CString			DocTitle;		// The title of the assciated document
	PrintControl*	pPrCtrl;		// Ptr to print control that will control print layout
	PrintProgressDlg* pPrgDlg;		// Ptr to the print progress dlg, created by StartPrinting()
	CCamView*		pCCamView;		// Ptr to the CCamView that is doing the print job

	static CCPrintInfo*	pCurrent;	// Ptr to the last constructed CCPrintInfo
};

#endif //webster

/********************************************************************************************

>	class CCPrintDialog : public CPrintDialog

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/3/95
	Purpose:	To provide a custom Print Dialog

********************************************************************************************/
//	WEBSTER-ranbirr-12/11/96
#ifndef WEBSTER

class CCPrintDialog : public CPrintDialog
{
// There is no memory stuff (Declare_Memdump etc) as this is an MFC derived class
public:
	CCPrintDialog(Document* pDoc,BOOL PrintSetUpOnly = FALSE);
	~CCPrintDialog();

	// Called by MFC to initialise the dlg's controls
	virtual BOOL OnInitDialog();

	// Called by MFC to open the print dialog
	virtual INT32 DoModal();

	// Called by MFC when user clicks on OK
	// Takes the values from the dlg, and commits them to the doc's print control
	virtual void OnOK();

	// Called by MFC when user clicks on Cancel
	// Copies the original print control back into doc's print control
	virtual void OnCancel();

	// Functions for retrieving info on the default printer
	static BOOL GetPaperSize(SIZEL* pPaperSize, BOOL RedrawPrintBorders = TRUE);
	static BOOL GetPrintableArea(DocRect* pPrintableArea);
	static BOOL GetPrintMargins(INT32* pLeft = NULL,INT32* pTop = NULL,INT32* pRight = NULL,INT32* pBottom = NULL);
	static BOOL GetResolution(INT32* pDPI,INT32* pXDPI = NULL,INT32* pYDPI = NULL);
	static BOOL GetScale(INT32* pScale);
	static BOOL CanMultiCopy(BOOL* pState);
	static BOOL IsPostscript();

	// Funcs to call when the printer settings change
	static BOOL UpdatePrinterSettings(DEVMODE* pDevMode,HDC hdc,Document* pDocument = NULL,BOOL RedrawPrintBorders = TRUE);
	static BOOL UpdatePrinterSettings(CPrintDialog* pPrDlg,Document* pDocument = NULL,BOOL RedrawPrintBorders = TRUE);
  	BOOL UpdatePrinterSettings(BOOL RedrawPrintBorders = TRUE);
  	static BOOL UpdateAppPrinterSettings(BOOL RedrawPrintBorders = TRUE);

	// Creates a DC using given global memory handles for DEVNAMES and DEVMODE structures
	static HDC CreatePrinterDC(HGLOBAL hDevNames,HGLOBAL hDevMode);

	// Func that retrieves the print control func associated with attached doc
	PrintControl* GetPrintControl();

	// This checks for the Problems help button being clicked in the Print dialog.
	static UINT32 CALLBACK PrintHookProc(HWND hwnd, UINT32 nMsg, WPARAM wParam, LPARAM lParam);

	// This checks for the help button being clicked in the Print Setup dialog.
	static UINT32 CALLBACK SetupHookProc(HWND hwnd, UINT32 nMsg, WPARAM wParam, LPARAM lParam);

	// This group of functions are used to cope with the selected printer disappearing
	// For example, when the user closes a connection, or renames a printer via the printer manager
	static void		ResetToDefaultPrinter();
	static void		ClosePrintDialogs();
	static void		InformResetToDefaultPrinter(BOOL ClosingDlgs);
	static CWnd*	GetPrintCWnd();
	static BOOL		IgnorePrintData()		{ return IgnorePrntData; }
	static void		ResetIgnorePrintData()	{ IgnorePrntData = FALSE; }
	static void		FreeGlobalHandle(HGLOBAL* pHandle);
	static void		GetSelectedPrinterName(LPTSTR pName,INT32 BufferSize);

private:
	BOOL InitCustomDlg();	// Initialises the dlg to use our custom dlg. Called from DoModal()

	void 			Enable(UINT32 GadgetID,BOOL State);	// Func for enabling/disabling dlg controls
	BOOL 			ShowDetails();						// General func for showing print details in the dlg
	void			GetDetails(PrintControl *TargetPC);	// General func for getting print detail settings from the dlg

	Document* 		pDocument;				// The doc this dlg is associated with
	PrintControl	LocalPrintControl;		// Local print control settings, committed to doc if user clicks OK

	// Functions for collecting printer settings
	static BOOL GetPrinterSettings(BOOL RedrawPrintBorders = TRUE);
	static BOOL LookUpPaperSize(UINT32 PaperSizeID,SIZEL* pPaperSize);

	// Current printer setting
	// All dimensions (paper size, margins, etc) are in MILLIPOINTS
	static BOOL		GotPrinterSettings;			// TRUE when we have a set of valid printer settings
	static SIZEL 	PrPaperSize;				// Size (in MILLIPOINTS) of the printer's paper
	static BOOL  	PrPortrait;					// TRUE when the printer is set for portrait, FALSE for landscape
	static INT32		PrScale;					// Printer scale factor percentage (i.e. 100 = 100%)
	static BOOL		PrMultiCopies;				// TRUE when printer can do multiple copies itself
	static INT32		PrResolutionX;				// Printer X resolution
	static INT32		PrResolutionY;				// Printer Y resolution
	static INT32		PrResolution;				// Printer resolution (can be -ve) (min(xres,yres))
	static INT32		PrLeftMargin;				// Left margin of printable area
	static INT32		PrTopMargin;				// Top margin of printable area
	static INT32		PrRightMargin;				// Right margin of printable area
	static INT32		PrBottomMargin;				// Bottom margin of printable area
	static BOOL		PrPostscript;				// TRUE if the printer is a Postscript printer

//	static DEVMODE* GetSystemDevMode();
	static DEVMODE* GetSystemDevMode(HWND hWnd,LPCTSTR pDriverName,LPCTSTR pDeviceName,LPCTSTR pPortName);

	BOOL PrintPrefsOpen;
	BOOL PrintPrefsChanged;

	static void DumpLastError(char* pStr1,char* pStr2);		// Dumps the last error returned by GetLastError()

	static BOOL ReopenMainDlg;

	static CPrintDialog*  pDlgSetup;
	static CCPrintDialog* pCurrentCCPrintDialog;
	static BOOL InformPrinterReset;
	static BOOL IgnorePrntData;

protected:
	//{{AFX_MSG(CCPrintDialog)
	afx_msg BOOL OnCommand(UINT32 GadgetID);
	afx_msg void OnPrintSetup();				// Called when user clicks on the Setup... button
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	DECLARE_DYNAMIC(CCPrintDialog)
};
#endif //webster

/********************************************************************************************

>	class CCPrintToFileDialog : public CFileDialog

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/5/95
	Purpose:	To provide a custom PrintToFile Dialog

********************************************************************************************/

#define FILENAMEBUFSIZE 256

class CCPrintToFileDialog : public CFileDialog
{
// There is no memory stuff (Declare_Memdump etc) as this is an MFC derived class
public:
	CCPrintToFileDialog(TCHAR *DefExt, TCHAR *FilterString);
	~CCPrintToFileDialog();

	virtual INT32 DoModal();
	LPSTR GetFullPathNamePtr();

private:
	static TCHAR FileName[FILENAMEBUFSIZE];
	String_64 Title;
};

#endif  // INC_PRDLGCTL


