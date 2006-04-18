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
// Custom Print Dialog

/*
*/

#include "camtypes.h"

#include "errors.h"
#include "prdlgctl.h"
//#include "printdlg.h"
#include "printprg.h"
#include "princomp.h"
#include "optsprin.h"
#include <winspool.h>
#include "fixmem.h"
#include "printmsg.h"
#include "camvw.h"
#include "prnprefs.h"	// PrintPrefsDlg;;InvokeDialog
#include "helpuser.h"
#include "camelot.h"
#include "app.h"
#include "ccdc.h"
#include "ctrlhelp.h"
//#include "richard2.h"
#include "progress.h"
//#include "markn.h"
#include "unicdman.h"
#include "fonts.h"
#include "printctl.h"
#include "prnmks.h"
//#include "jason.h"		// Error messages
//#include "resource.h"	// _R(IDS_OK)/CANCEL
//#include "simon.h"		// _R(IDS_HELP)

#include <dlgs.h>       // for standard control IDs for commdlg

#ifdef _DEBUG
#include "optsprin.h"
#include "keypress.h"
#endif

#define Swap(a,b)       { (a)^=(b), (b)^=(a), (a)^=(b); }

//---------------------------------

//	WEBSTER-ranbirr-12/11/96
#ifndef WEBSTER
BEGIN_MESSAGE_MAP(CCPrintDialog, CPrintDialog)
	//{{AFX_MSG_MAP(CPrintDialog)
	ON_COMMAND_EX(_R(IDC_ALLOBJECTS), 		OnCommand)
	ON_COMMAND_EX(_R(IDC_SELECTEDOBJECTS), 	OnCommand)
	ON_COMMAND_EX(_R(IDC_DPSBOTH), 			OnCommand)
	ON_COMMAND_EX(_R(IDC_DPSLEFTPAGES), 	OnCommand)
	ON_COMMAND_EX(_R(IDC_DPSRIGHTPAGES), 	OnCommand)
	ON_COMMAND_EX(_R(IDC_PRINTOPTIONS), 	OnCommand)
	ON_COMMAND_EX(_R(IDC_PRINTHELP), 		OnCommand)
	ON_COMMAND(psh1, OnPrintSetup) // print setup button when print is displayed
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//---------------------------------

IMPLEMENT_DYNAMIC(CCPrintDialog, CPrintDialog)
CC_IMPLEMENT_DYNAMIC(PrintMsg,Msg)
#endif //webster

//---------------------------------
// This is the new Win95-compatible DOCINFO structure that has two extra fields, lpszDatatype & fwType.
// We don't have the new header files yet, so we need to implement this struct ourselves.
// This new struct is utilised by the func CCPrintInfo::SetUpDocInfo(DOCINFO* pDocInfo)

typedef struct _NEW_DOCINFOA {
    INT32     cbSize;
    LPCSTR   lpszDocName;
    LPCSTR   lpszOutput;
    LPCSTR   lpszDatetype;
	DWORD    fwType;
} NEW_DOCINFOA;
typedef struct _NEW_DOCINFOW {
    INT32     cbSize;
    LPCWSTR  lpszDocName;
    LPCWSTR  lpszOutput;
    LPCWSTR  lpszDatetype;
	DWORD    fwType;
} NEW_DOCINFOW;
#ifdef UNICODE
typedef NEW_DOCINFOW NEW_DOCINFO;
#else
typedef NEW_DOCINFOA NEW_DOCINFO;
#endif // UNICODE

#ifndef DI_APPBANDING
#define DI_APPBANDING   0x0001
#endif

//---------------------------------

// CCPrintDialog static vars
//CCPrintDialog* CCPrintDialog::pInfoPrintDlg = NULL;		// Used to get settings for the default printer
//	WEBSTER-ranbirr-12/11/96
#ifndef WEBSTER

BOOL	CCPrintDialog::GotPrinterSettings	= FALSE;
SIZEL 	CCPrintDialog::PrPaperSize;
BOOL  	CCPrintDialog::PrPortrait			= TRUE;
INT32	CCPrintDialog::PrScale				= 100;
BOOL	CCPrintDialog::PrMultiCopies 		= FALSE;

INT32	CCPrintDialog::PrResolution			= 300;
INT32	CCPrintDialog::PrResolutionX		= 300;
INT32	CCPrintDialog::PrResolutionY		= 300;

INT32	CCPrintDialog::PrLeftMargin			= 0;	//0x32a0;
INT32	CCPrintDialog::PrTopMargin			= 0;	//0x2ee0;
INT32	CCPrintDialog::PrRightMargin		= 0;	//0x3750;
INT32	CCPrintDialog::PrBottomMargin		= 0;	//0x2ee0;

BOOL	CCPrintDialog::PrPostscript			= FALSE;

BOOL 	CCPrintDialog::ReopenMainDlg		= FALSE;
BOOL	CCPrintDialog::InformPrinterReset	= FALSE;
BOOL	CCPrintDialog::IgnorePrntData		= FALSE;

CCPrintInfo*   CCPrintInfo::pCurrent			= NULL;	// Ptr to the last constructed CCPrintInfo
#endif //webster

char	CCPrintToFileDialog::FileName[FILENAMEBUFSIZE]	= {'\0'};
//	WEBSTER-ranbirr-12/11/96
#ifndef WEBSTER
CCPrintDialog* CCPrintDialog::pCurrentCCPrintDialog = NULL;
CPrintDialog*  CCPrintDialog::pDlgSetup				= NULL;
#endif //webster
void FixFPControlRegister();

//---------------------------------

// Paper selections
// This data is taken from wingdi.h and defines the sizes of all the default paper size values
// that appear in the dmPaperSize member of DEVMODE
//
// If dmPaperSize is zero, dmPaperLength and dmPaperWidth define the paper size (in tenths of a millimetre)
//
// NOTE: The code must be tested on a dot matrix printer because the dmPaperLength can be arbitrary.
//
// NOTE: I have not found an automatic way of finding out the dimensions of a particular paper size.
// The closest I got was to use OpenPrinter() to get a handle of a printer, then use GetForm() to retrieve
// the dimensions of a paper size given the name of the paper (e.g. "A4").  However MSDN says that neither of
// these functions are supported under Win32s.

#define IN_TO_MP(n) (MILLIPOINT((double(IN_MP_VAL)*n)+0.5))
#define MM_TO_MP(n) (MILLIPOINT((double(MM_MP_VAL)*n)+0.5))

#define DMPAPER_LISTEND (DMPAPER_LAST+1)

struct PaperSizesType
{
	UINT32		ID;
	MILLIPOINT	Width;
	MILLIPOINT	Height;
} pPaperSizes[] = 
{
	{ DMPAPER_LETTER, 		IN_TO_MP(8.5), 		IN_TO_MP(11)  	}, 	// Letter 8 1/2 x 11 in               
	{ DMPAPER_LETTERSMALL,	IN_TO_MP(8.5), 		IN_TO_MP(11)  	},	// Letter Small 8 1/2 x 11 in         
	{ DMPAPER_TABLOID,		IN_TO_MP(11),		IN_TO_MP(17)  	},	// Tabloid 11 x 17 in                 
	{ DMPAPER_LEDGER,		IN_TO_MP(17),		IN_TO_MP(11)  	},	// Ledger 17 x 11 in                  
	{ DMPAPER_LEGAL,		IN_TO_MP(8.5),		IN_TO_MP(14)  	},	// Legal 8 1/2 x 14 in                
	{ DMPAPER_STATEMENT,	IN_TO_MP(5.5),		IN_TO_MP(8.5) 	},	// Statement 5 1/2 x 8 1/2 in         
	{ DMPAPER_EXECUTIVE,	IN_TO_MP(7.25),		IN_TO_MP(10.5)	},	// Executive 7 1/4 x 10 1/2 in        

	{ DMPAPER_A3,			MM_TO_MP(297),		MM_TO_MP(420)	},	// A3 297 x 420 mm                    
	{ DMPAPER_A4, 			MM_TO_MP(210),		MM_TO_MP(297)	},	// A4 210 x 297 mm                    
	{ DMPAPER_A4SMALL,		MM_TO_MP(210),		MM_TO_MP(297)	},	// A4 Small 210 x 297 mm              
	{ DMPAPER_A5,			MM_TO_MP(148),		MM_TO_MP(210)	},	// A5 148 x 210 mm                    
	{ DMPAPER_B4,			MM_TO_MP(250),		MM_TO_MP(354)	},	// B4 250 x 354                       
	{ DMPAPER_B5,			MM_TO_MP(182),		MM_TO_MP(257)	},	// B5 182 x 257 mm                    

	{ DMPAPER_FOLIO,		IN_TO_MP(8.5),		IN_TO_MP(13)	},	// Folio 8 1/2 x 13 in                
	{ DMPAPER_QUARTO,		MM_TO_MP(215),		MM_TO_MP(275)	},	// Quarto 215 x 275 mm                
	{ DMPAPER_10X14,		IN_TO_MP(10),		IN_TO_MP(14)	},	// 10x14 in                           
	{ DMPAPER_11X17,		IN_TO_MP(11),		IN_TO_MP(17)	},	// 11x17 in                           
	{ DMPAPER_NOTE,			IN_TO_MP(8.5),		IN_TO_MP(11)	},	// Note 8 1/2 x 11 in                 

	{ DMPAPER_ENV_9,		IN_TO_MP(3.875),	IN_TO_MP(8.875)	},	// Envelope #9 3 7/8 x 8 7/8 in
	{ DMPAPER_ENV_10,		IN_TO_MP(4.125),	IN_TO_MP(9.5)	},	// Envelope #10 4 1/8 x 9 1/2 in        
	{ DMPAPER_ENV_11,		IN_TO_MP(4.5),		IN_TO_MP(10.375)},	// Envelope #11 4 1/2 x 10 3/8 in       
	{ DMPAPER_ENV_12,		IN_TO_MP(4.75),		IN_TO_MP(11)	},	// Envelope #12 4 3/4 x 11 in          
	{ DMPAPER_ENV_14,		IN_TO_MP(5),		IN_TO_MP(11.5)	},	// Envelope #14 5 x 11 1/2 in           

	{ DMPAPER_CSHEET,		IN_TO_MP(17),		IN_TO_MP(22)	},	// C size sheet 17 x 22 in
	{ DMPAPER_DSHEET,		IN_TO_MP(22),		IN_TO_MP(34)	},	// D size sheet 22 x 34 in
	{ DMPAPER_ESHEET,		IN_TO_MP(34),		IN_TO_MP(44)	},	// E size sheet 34 x 44 in

	{ DMPAPER_ENV_DL,		MM_TO_MP(110),		MM_TO_MP(220)	},	// Envelope DL 110 x 220mm            
	{ DMPAPER_ENV_C5,		MM_TO_MP(162),		MM_TO_MP(229)	},	// Envelope C5 162 x 229 mm           
	{ DMPAPER_ENV_C3,		MM_TO_MP(324),		MM_TO_MP(458)	},	// Envelope C3  324 x 458 mm          
	{ DMPAPER_ENV_C4,		MM_TO_MP(229),		MM_TO_MP(324)	},	// Envelope C4  229 x 324 mm          
	{ DMPAPER_ENV_C6,		MM_TO_MP(114),		MM_TO_MP(162)	},	// Envelope C6  114 x 162 mm          
	{ DMPAPER_ENV_C65,		MM_TO_MP(114),		MM_TO_MP(229)	},	// Envelope C65 114 x 229 mm          

	{ DMPAPER_ENV_B4,		MM_TO_MP(250),		MM_TO_MP(353)	},	// Envelope B4  250 x 353 mm          
	{ DMPAPER_ENV_B5,		MM_TO_MP(176),		MM_TO_MP(250)	},	// Envelope B5  176 x 250 mm          
	{ DMPAPER_ENV_B6,		MM_TO_MP(176),		MM_TO_MP(125)	},	// Envelope B6  176 x 125 mm          
	{ DMPAPER_ENV_ITALY,	MM_TO_MP(110),		MM_TO_MP(230)	},	// Envelope 110 x 230 mm              

	{ DMPAPER_ENV_MONARCH,	IN_TO_MP(3.875),	IN_TO_MP(7.5)	},	// Envelope Monarch 3.875 x 7.5 in    
	{ DMPAPER_ENV_PERSONAL,	IN_TO_MP(3.625),	IN_TO_MP(6.5)	},	// 6 3/4 Envelope 3 5/8 x 6 1/2 in    

	{ DMPAPER_FANFOLD_US,			IN_TO_MP(14.875),	IN_TO_MP(11)	},	// US Std Fanfold 14 7/8 x 11 in      
	{ DMPAPER_FANFOLD_STD_GERMAN,	IN_TO_MP(8.5),		IN_TO_MP(12)	},	// German Std Fanfold 8 1/2 x 12 in   
	{ DMPAPER_FANFOLD_LGL_GERMAN,	IN_TO_MP(8.5),		IN_TO_MP(13)	},	// German Legal Fanfold 8 1/2 x 13 in 

#if(WINVER >= 0x0400)
	{ DMPAPER_ISO_B4,				MM_TO_MP(250),		MM_TO_MP(353)	},	// B4 (ISO) 250 x 353 mm
	{ DMPAPER_JAPANESE_POSTCARD,	MM_TO_MP(100),		MM_TO_MP(148)	},	// Japanese Postcard 100 x 148 mm     
	{ DMPAPER_9X11,					IN_TO_MP(9),		IN_TO_MP(11)	},	// 9 x 11 in                          
	{ DMPAPER_10X11,				IN_TO_MP(10),		IN_TO_MP(11)	},	// 10 x 11 in                         
	{ DMPAPER_15X11,				IN_TO_MP(15),		IN_TO_MP(11)	},	// 15 x 11 in                         
	{ DMPAPER_ENV_INVITE,			MM_TO_MP(220),		MM_TO_MP(220)	},	// Envelope Invite 220 x 220 mm       
//	{ DMPAPER_RESERVED_48,			MM_TO_MP(250),		MM_TO_MP(353)	},	// RESERVED--DO NOT USE               
//	{ DMPAPER_RESERVED_49,			MM_TO_MP(250),		MM_TO_MP(353)	},	// RESERVED--DO NOT USE               
	{ DMPAPER_LETTER_EXTRA,			IN_TO_MP(9.5),		IN_TO_MP(12)	},	// Letter Extra 9 \275 x 12 in        
	{ DMPAPER_LEGAL_EXTRA,			IN_TO_MP(9.5),		IN_TO_MP(15)	},	// Legal Extra 9 \275 x 15 in         
	{ DMPAPER_TABLOID_EXTRA,		IN_TO_MP(11.69),	IN_TO_MP(18)	},	// Tabloid Extra 11.69 x 18 in        
	{ DMPAPER_A4_EXTRA,				IN_TO_MP(9.27),		IN_TO_MP(12.69)	},	// A4 Extra 9.27 x 12.69 in           
	{ DMPAPER_LETTER_TRANSVERSE,	IN_TO_MP(8.5),		IN_TO_MP(11)	},	// Letter Transverse 8 \275 x 11 in   
	{ DMPAPER_A4_TRANSVERSE,		MM_TO_MP(210),		MM_TO_MP(297)	},	// A4 Transverse 210 x 297 mm         
	{ DMPAPER_LETTER_EXTRA_TRANSVERSE,IN_TO_MP(9.5),	IN_TO_MP(12)	},	// Letter Extra Transverse 9\275 x 12 in 
	{ DMPAPER_A_PLUS,				MM_TO_MP(227),		MM_TO_MP(356)	},	// SuperA/SuperA/A4 227 x 356 mm      
	{ DMPAPER_B_PLUS,				MM_TO_MP(305),		MM_TO_MP(487)	},	// SuperB/SuperB/A3 305 x 487 mm      
	{ DMPAPER_LETTER_PLUS,			IN_TO_MP(8.5),		IN_TO_MP(12.69)	},	// Letter Plus 8.5 x 12.69 in         
	{ DMPAPER_A4_PLUS,				MM_TO_MP(210),		MM_TO_MP(330)	},	// A4 Plus 210 x 330 mm               
	{ DMPAPER_A5_TRANSVERSE,		MM_TO_MP(148),		MM_TO_MP(210)	},	// A5 Transverse 148 x 210 mm         
	{ DMPAPER_B5_TRANSVERSE,		MM_TO_MP(182),		MM_TO_MP(257)	},	// B5 (JIS) Transverse 182 x 257 mm   
	{ DMPAPER_A3_EXTRA,				MM_TO_MP(322),		MM_TO_MP(445)	},	// A3 Extra 322 x 445 mm              
	{ DMPAPER_A5_EXTRA,				MM_TO_MP(174),		MM_TO_MP(235)	},	// A5 Extra 174 x 235 mm              
	{ DMPAPER_B5_EXTRA,				MM_TO_MP(201),		MM_TO_MP(276)	},	// B5 (ISO) Extra 201 x 276 mm        
	{ DMPAPER_A2,					MM_TO_MP(420),		MM_TO_MP(594)	},	// A2 420 x 594 mm                    
	{ DMPAPER_A3_TRANSVERSE,		MM_TO_MP(297),		MM_TO_MP(420)	},	// A3 Transverse 297 x 420 mm         
	{ DMPAPER_A3_EXTRA_TRANSVERSE,	MM_TO_MP(322),		MM_TO_MP(445)	},	// A3 Extra Transverse 322 x 445 mm   
#endif 

	{ DMPAPER_LISTEND,0,0 }
};

//---------------------------------
void Beep();
//---------------------------------

//	WEBSTER-ranbirr-12/11/96
#ifndef WEBSTER

/********************************************************************************************

>	CCPrintDialog::CCPrintDialog(Document* pDoc,BOOL PrintSetUpOnly = FALSE)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/3/95
	Inputs:		pDoc           = ptr to the document this print dialog is associated with
				PrintSetUpOnly = TRUE if you only want the print setup dialog only
	Purpose:	Creates a camelot print dialog, based on CPrintDialog
	SeeAlso:	CCPrintInfo

********************************************************************************************/

CCPrintDialog::CCPrintDialog(Document* pDoc,BOOL PrintSetUpOnly) : CPrintDialog(PrintSetUpOnly)
{
	pDocument = pDoc;
	PrintPrefsOpen 		= FALSE;
	PrintPrefsChanged 	= FALSE;
}

/********************************************************************************************

>	CCPrintDialog::~CCPrintDialog()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/3/95
	Inputs:		-
	Purpose:	Default destructor
				This deletes the member DC using ::DeleteDC() as instructed by the help on 
				CPrintDialog::GetPrinterDC() (only if the DC is not NULL of course)

				Why CPrintDialog doesn't have a destructor to do this is a mystery.  I think it relies
				on you using CPrintInfo who's destructor DOES delete the DC if need be.

	SeeAlso:	CCPrintInfo

********************************************************************************************/

CCPrintDialog::~CCPrintDialog()
{
	if (m_pd.hDC != NULL)
	{
		::DeleteDC(m_pd.hDC);
		m_pd.hDC = NULL;		// NULL it in case base class destructors get ideas above their station
	}

	// NB: We cannot free m_pd.hDevNames & m_pd.hDevMode here because CCPrintDialogs are used
	// with CWinApp functions (e.g. CWinApp::DoPrintDialog()), and these functions to copy the
	// handles into CWinApp::m_hDevNames and CWinApp::m_hDevMode member vars.
	// Freeing the handles here could make the app's handles invalid, hence making it impossible
	// to print anything.
}

/********************************************************************************************

>	PrintControl* CCPrintDialog::GetPrintControl()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/4/95
	Inputs:		-
	Returns:	Ptr to the print control
	Purpose:	Returns the ptr to the print control attached to the doc associated with this dialog
	SeeAlso:	CCPrintInfo

********************************************************************************************/

PrintControl* CCPrintDialog::GetPrintControl()
{
	PrintControl* pPrCtrl = NULL;

	if (pDocument != NULL)
	{
		PrintComponent* pComp = (PrintComponent*)pDocument->GetDocComponent(CC_RUNTIME_CLASS(PrintComponent));

		if (pComp != NULL)
			pPrCtrl = pComp->GetPrintControl();
	}

	ERROR3IF(pPrCtrl == NULL,"Unable to find the print control object");

	return pPrCtrl;
}



/********************************************************************************************
>	static UINT32 CALLBACK CCPrintDialog::PrintHookProc(HWND hwnd, UINT32 nMsg,
													  WPARAM wParam, LPARAM lParam)
	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/5/95
	Inputs:		(the usual message proc parameters)
	Returns:	FALSE if the common dialog code should process this message, TRUE if it
				should ignore the message.
	Purpose:	Checks messages destined for the print common dialog to see if they
				mean the user is invoking help on Printing Problems
	SeeAlso:	CCamApp::OnFilePrintSetup; _HelpUser
********************************************************************************************/

UINT32 CALLBACK CCPrintDialog::PrintHookProc(HWND hwnd, UINT32 nMsg, WPARAM wParam, LPARAM lParam)
{
	// Has the user clicked the 'Problems?' help button in the print dialog?
	if (nMsg == WM_COMMAND && wParam == _R(IDC_PROBLEMS_HELP_BUTTON))
	{
		// Yes, run the help system and don't process this message any more.  As we don't
		// have any C++ object associated with the Print Problems page we must use the
		// bodgey raw help function here.
		_HelpUser(TEXT("PrintProblems"));
		return TRUE;
	}

	if (nMsg == WM_INITDIALOG && pCurrentCCPrintDialog != NULL)
		return pCurrentCCPrintDialog->OnInitDialog();

	// Do the default for everything else.
	return FALSE;
}



/********************************************************************************************
>	static UINT32 CALLBACK CCPrintDialog::SetupHookProc(HWND hwnd, UINT32 nMsg,
													  WPARAM wParam, LPARAM lParam)
	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/5/95
	Inputs:		(the usual message proc parameters)
	Returns:	FALSE if the common dialog code should process this message, TRUE if it
				should ignore the message.
	Purpose:	Checks messages destined for the print-setup common dialog to see if they
				mean the user is invoking help on the dialog.
	SeeAlso:	CCamApp::OnFilePrintSetup; _HelpUser
********************************************************************************************/

UINT32 CALLBACK CCPrintDialog::SetupHookProc(HWND hwnd, UINT32 nMsg, WPARAM wParam, LPARAM lParam)
{
	// Has the user clicked the help button in the print-setup dialog?
	if (nMsg == WM_COMMAND && wParam == pshHelp)
	{
		// Yes, run the help system and don't process this message any more.  As we don't
		// have any C++ object associated with the print-setup dialog we must use the
		// bodgey raw help function here.
		_HelpUser(TEXT("CCPrintSetupDialog"));
		return TRUE;
	}

	// Do the default for everything else.
	return FALSE;
}



/********************************************************************************************

>	BOOL CCPrintDialog::InitCustomDlg()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/3/95
	Inputs:		-
	Returns:	TRUE if it initialised OK, FALSE if not.
	Purpose:	This initialises the underlying CPrintDialog to use our customised verison of the print
				dialog.

				It will error if it can't find our replacement dialog resource
	SeeAlso:	CCPrintInfo

********************************************************************************************/

BOOL CCPrintDialog::InitCustomDlg()
{
	BOOL ok = FALSE;

	HRSRC hrs = ::FindResource(AfxGetResourceHandle(),MAKEINTRESOURCE(_R(IDD_PRINTTPLATE)),RT_DIALOG);
	if (hrs != NULL)
	{
		HGLOBAL hTemplate = ::LoadResource(AfxGetResourceHandle(),hrs);
		if (hTemplate != NULL)
		{
			LPVOID p = ::LockResource(hTemplate);
			if (p != NULL)
			{
				m_pd.hPrintTemplate = hTemplate;
				m_pd.Flags |= PD_ENABLEPRINTTEMPLATEHANDLE;
				m_pd.Flags |= PD_USEDEVMODECOPIES;
				m_pd.Flags &= ~PD_DISABLEPRINTTOFILE;
				m_pd.Flags &= ~PD_HIDEPRINTTOFILE;

				// Make sure a help button is displayed etc.
				m_pd.Flags |= PD_ENABLEPRINTHOOK | PD_ENABLESETUPHOOK;
				m_pd.lpfnPrintHook = (LPPRINTHOOKPROC) PrintHookProc;
				m_pd.lpfnSetupHook = (LPSETUPHOOKPROC) SetupHookProc;
				m_pd.hwndOwner = GetMainFrame()->m_hWnd;

				// If not running on Chicago then show a help button.
				if (!IsWindows95())	//!IsWin32c())
				{
					m_pd.Flags |= PD_SHOWHELP;
				}

				BOOL MultiCopy = FALSE;
				CanMultiCopy(&MultiCopy);

				if (MultiCopy)
					m_pd.Flags |= PD_USEDEVMODECOPIES;
				else
					m_pd.Flags &= ~PD_USEDEVMODECOPIES;

				if (LocalPrintControl.IsCollated())
					m_pd.Flags |= PD_COLLATE;

				if (LocalPrintControl.GetPrintToFile())
					m_pd.Flags |= PD_PRINTTOFILE;

				ok = TRUE;
			}
		}
	}

	if (!ok)
		DumpLastError("CCPrintDialog::InitCustomDlg()","it failed");

	return (ok);
}

/********************************************************************************************

>	static void CCPrintDialog::DumpLastError(char* pStr1,char* pStr2)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/7/95
	Inputs:		-
	Returns:	-
	Purpose:	Dumps the last error returned by GetLastError() using the FormatMessage() command
	SeeAlso:	CPrintDialog::DoModal

********************************************************************************************/

void CCPrintDialog::DumpLastError(char* pStr1,char* pStr2)
{
	#ifdef _DEBUG

	DWORD ResourceErr = GetLastError();
	if (ResourceErr != NULL)
	{
		TRACE( _T("%s : %s\n"),pStr1,pStr2);
		TRACE( _T("Resource err code = %ld (0x%lx)\n"),ResourceErr,ResourceErr);

		char ErrMess[1000];

		::FormatMessage(
		  FORMAT_MESSAGE_FROM_SYSTEM,
		  NULL,
		  ResourceErr,
		  MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
		  ErrMess,
		  999,
		  NULL );

		TRACE( _T("Error message : %s\n"),(LPTSTR)ErrMess);
	}

	#endif
}

/********************************************************************************************

>	INT32 CCPrintDialog::DoModal()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/3/95
	Inputs:		-
	Returns:	See CPrintDialog::DoModal() for return values
	Purpose:	This calls CPrintDialog::DoModal() and returns the value that it returns

				This func allows us to detect when the print dialog has been displayed, and which
				button the user clicked on to close it (i.e. OK or Cancel)

	SeeAlso:	CPrintDialog::DoModal

********************************************************************************************/

INT32 CCPrintDialog::DoModal()
{
	ERROR2IF(pDocument == NULL,IDCANCEL,"NULL doc ptr");

	ERROR3IF(pCurrentCCPrintDialog != NULL,"pCurrentCCPrintDialog is not NULL");
	pCurrentCCPrintDialog = this;

	// Get a copy of the print control in the associated document
	PrintControl* pPrCtrl = GetPrintControl();
	ERROR2IF(pPrCtrl == NULL,IDCANCEL,"Unable to find the print control");
	LocalPrintControl = *pPrCtrl;

	// Inform the user if we have switched to using the default printer on the system
	InformResetToDefaultPrinter(FALSE);

	// Initialise the custom dlg template
	ERROR2IF(!InitCustomDlg(),IDCANCEL,"Unable to init the print dlg with our custom template");

	// Update our details about this printer
	UpdatePrinterSettings(FALSE);

	// Set the print control to print the selected spread
	// Do this after UpdatePrinterSettings() to make sure it uses the right values!
	LocalPrintControl.SetUp(Document::GetSelectedSpread());

	// Display the dlg to the user, and return the result
	BOOL ok = FALSE;
	BOOL Reopen = FALSE;
	do
	{
		// If we are reopening the main print dlg, update the printer settings first
		// (NB: This should only happen in Win3.1 after the print setup dlg has been used. 
		//  See CCPrintDialog::OnPrintSetup())
		if (Reopen)
			UpdatePrinterSettings();

		// Keep Control Helper system informed
		ControlHelper::InformModalDialogOpened();

		// Open the main print dlg (uses ::PrintDlg() in the depths of MFC)
		ok = CPrintDialog::DoModal();
		FixFPControlRegister();

		// Keep Control Helper system informed
		ControlHelper::InformModalDialogClosed();

		Reopen = ReopenMainDlg;		// Do we need to reopen the main print dlg?
		ReopenMainDlg = FALSE;		// Make sure this flag is cleared, or we'll be here all day

	} while (Reopen);

	// If we are to ignore the printer data (because the selected printer has been deleted, renamed, etc)
	// then we must reset the global memory handles that hold the data, as the settings they contain
	// no longer refer to a printer known by the host system
	if (IgnorePrintData())
	{
		FreeGlobalHandle(&m_pd.hDevMode);
		FreeGlobalHandle(&m_pd.hDevNames);
		ResetIgnorePrintData();
	}

	ERROR3IF(pCurrentCCPrintDialog == NULL,"pCurrentCCPrintDialog is NULL");
	pCurrentCCPrintDialog = NULL;

	return ok;
}

//-------------------

/********************************************************************************************

>	static BOOL CCPrintDialog::GetPrinterSettings(BOOL RedrawPrintBorders = TRUE)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/4/95
	Inputs:		RedrawPrintBorders if a force redraw should be issued (Defaults to TRUE)
	Returns:	TRUE if OK, FALSE otherwise
	Purpose:	This function ensures that we have a valid set of printer settings.

				If this is the first time this function has been called, it uses a CCPrintDialog
				to get the default settings from the printer driver.

				If we already have a set of valid settings, this func does nothing

	SeeAlso:	-

********************************************************************************************/

BOOL CCPrintDialog::GetPrinterSettings(BOOL RedrawPrintBorders)
{
	if (!GotPrinterSettings)
	{
		Progress Hourglass(_R(IDS_UPDATE_PRINTER_SETTINGS),-1,FALSE);
		Hourglass.Update();

		CCPrintDialog PrintDlg(NULL);		// Use CCPrintDialog to get hold of printer's DEVMODE settings
		PrintDlg.m_pd.Flags &= ~PD_RETURNDC;
		if (PrintDlg.GetDefaults())
		{
			FixFPControlRegister();
			PrintDlg.UpdatePrinterSettings(RedrawPrintBorders);
		}

		// We are reponsible for freeing the hDevNames and hDevMode handles if ::PrintDlg() assigned them
		// for us.
		//
		// NB: We cannot do this as a matter of course in the deconstructor because CCPrintDialogs are used
		// with CWinApp functions (e.g. CWinApp::DoPrintDialog()), and these functions copy the
		// handles into CWinApp::m_hDevNames and CWinApp::m_hDevMode member vars.
		// Freeing the handles in the deconstructor could make the app's handles invalid, hence making it impossible
		// to print anything.

		FreeGlobalHandle(&PrintDlg.m_pd.hDevNames);
		FreeGlobalHandle(&PrintDlg.m_pd.hDevMode);
	}

	return (GotPrinterSettings);
}

/********************************************************************************************

>	static void CCPrintDialog::FreeGlobalHandle(HGLOBAL* pHandle)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/2/96
	Inputs:		pHandle = ptr to the handle to free
	Outputs:	Handle at *pHandle is freed (if there is one), and *pHandle is set to NULL
	Returns:	-
	Purpose:	Helper function to make the code easier to read

				if *pHandle is NULL, nothing happens

				Otherwise *pHandle is freed using ::GlobalFree(*pHandle), and *pHandle is set to NULL.

	SeeAlso:	

********************************************************************************************/

void CCPrintDialog::FreeGlobalHandle(HGLOBAL* pHandle)
{
	ERROR3IF(pHandle == NULL,"pHandle is NULL");
	if (pHandle == NULL)
		return;

	if (*pHandle != NULL)
	{
		::GlobalFree(*pHandle);
		*pHandle = NULL;
	}
}

/********************************************************************************************

>	static void CCPrintDialog::ResetToDefaultPrinter()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/2/96
	Inputs:		-
	Returns:	-
	Purpose:	Use this function to tell the printing system that the printer data has been reset
				so that it will use the default printer next time you print.

				If there are any print-related dialogs open, they will be closed.

				If a dialog is closed, the function will display a warning to the user.

				If a dialog is not closed, a flag is set so that the warning will be displayed when 
				the user next tries to print something.

	SeeAlso:	CCamApp::ResetToDefaultPrinter()

********************************************************************************************/

void CCPrintDialog::ResetToDefaultPrinter()
{
	if (GotPrinterSettings)
	{
		GotPrinterSettings = FALSE;			// We need new printer settings
		InformPrinterReset = TRUE;			// We need to inform the use about what's happening

		// Close any print dlgs that may be open
		ClosePrintDialogs();

		// Get the default printer settings
		GetPrinterSettings();
	}
}

/********************************************************************************************

>	static void CCPrintDialog::ClosePrintDialogs()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/2/96
	Inputs:		-
	Returns:	-
	Purpose:	Closes any print-related dlgs that may be open.
				If any dlgs are closed, and it is due to resetting to the default printer,
				a warning will be issued to the user informing them of this fact.

	SeeAlso:	static BOOL ResetToDefaultPrinter()

********************************************************************************************/

void CCPrintDialog::ClosePrintDialogs()
{
	// Close the print prefs tabbed dlg
	PrintPrefsDlg::CloseCurrentDlg();

	// Close the print setup dlg that's opened from the main print dlg
	if (pDlgSetup != NULL)
	{
		IgnorePrntData = TRUE;	// We are forcing the closure of the print dlgs
		// Inform the user if we have switched to using the default printer on the system
		// before closing dialog
		InformResetToDefaultPrinter(TRUE);
		pDlgSetup->PostMessage(WM_CLOSE);
	}

	// Close the main print dlg
	if (pCurrentCCPrintDialog != NULL)
	{
		IgnorePrntData = TRUE;	// We are forcing the closure of the print dlgs
		// Inform the user if we have switched to using the default printer on the system
		// before closing dialog
		InformResetToDefaultPrinter(TRUE);
		pCurrentCCPrintDialog->PostMessage(WM_CLOSE);
	}

	CCamApp *pCamApp = (CCamApp *)AfxGetApp();

	// Close the print setup dlg opened from the File menu
	if (pCamApp != NULL && pCamApp->GetFilePrintSetupDlg() != NULL)
	{
		IgnorePrntData = TRUE;	// We are forcing the closure of the print dlgs
		// Inform the user if we have switched to using the default printer on the system
		// before closing dialog
		InformResetToDefaultPrinter(TRUE);
		pCamApp->GetFilePrintSetupDlg()->PostMessage(WM_CLOSE);
	}
}

/********************************************************************************************

>	static void CCPrintDialog::InformResetToDefaultPrinter(BOOL ClosingDlgs)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/2/96
	Inputs:		ClosingDlgs - TRUE if the caller will also close any print dlgs too.
	Returns:	-
	Purpose:	Displays a warning to the user saying that we have switched to the default printer.

				It only does this if the flag InformPrinterReset is TRUE.
				
				After displaying the warning, InformPrinterReset is set to FALSE, so you can call this
				func as often as you like - it will only get displayed once until the next time we
				switch to the default printer.

				If this is called just before closing any print dlgs, call with ClosingDlgs=TRUE.
				This will display a slightly different warning explaining the dlgs will be closed.

	SeeAlso:	static BOOL ResetToDefaultPrinter()

********************************************************************************************/

void CCPrintDialog::InformResetToDefaultPrinter(BOOL ClosingDlgs)
{
	if (InformPrinterReset)
	{
		if (ClosingDlgs)
			InformWarning(_R(IDS_RESET_PRINTER_REOPEN));
		else
			InformWarning(_R(IDS_RESET_PRINTER));

		InformPrinterReset = FALSE;
	}
}

/********************************************************************************************

>	static void CCPrintDialog::GetSelectedPrinterName(LPTSTR pName,INT32 BufferSize)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/2/96
	Inputs:		pName = ptr to buffer to place name in
				BufferSize = size of buffer pointed to by BufferSize
	Returns:	-
	Purpose:	This will return what is considered to be the name of the app's selected printer

				If the main print dlg or the print setup dlg (opened via the File menu) are open,
				the name is taken from them.

				Otherwise, the app's cached printer data is used, and this is gained vai CCamApp::GetDevNames()

				If no printer is found, pName == "" (i.e. pName[0] == 0).

	SeeAlso:	static BOOL ResetToDefaultPrinter()

********************************************************************************************/

void CCPrintDialog::GetSelectedPrinterName(LPTSTR pName,INT32 BufferSize)
{
	// Enough of these silly values
	ERROR3IF(pName == NULL || BufferSize < 1,"How about some senisble input params eh?");
	if (pName == NULL || BufferSize < 1)
		return;

	CCamApp *pCamApp = (CCamApp *)AfxGetApp();
	
	// Default to the app's DEVMODE structure
	HGLOBAL hDevNames = NULL;
	if(pCamApp != NULL)
		hDevNames = pCamApp->GetDevNames();

	// If we have a main print dlg up, look at the selected printer within the dlg
	if (pCurrentCCPrintDialog != NULL)
		hDevNames = pCurrentCCPrintDialog->m_pd.hDevNames;

	// If we have the print setup dlg open from the file menu, look at the selected printer within it
	if (pCamApp != NULL && pCamApp->GetFilePrintSetupDlg() != NULL)
		hDevNames = pCamApp->GetFilePrintSetupDlg()->m_pd.hDevNames;

	// NULL terminate now, just in case things go wrong
	pName[0] = 0;

	if (hDevNames != NULL)
	{
		// Get a ptr to the memory block defined by hDevNames
		DEVNAMES* pDevNames = (DEVNAMES*)::GlobalLock(hDevNames);
		if (pDevNames != NULL)
		{
			// Copy the name from the DEVNAMES field into the buffer provided
			camStrncpy(pName,LPTSTR(pDevNames) + pDevNames->wDeviceOffset,BufferSize);
			::GlobalUnlock(hDevNames);
		}
	}
}

/********************************************************************************************

>	static CWnd* CCPrintDialog::GetPrintCWnd()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/2/96
	Inputs:		-
	Returns:	Ptr CWnd of open print dlg, or NULL if non open
	Purpose:	Returns the CWnd of the top-most print related dlg that's currently open.

				If none are open, NULL is returned.

				This allows other modal dialogs, such as the error box, open on top of the
				print dialogs, and remain on top.

	SeeAlso:	CInformErrorDialog::GetSafeParent()

********************************************************************************************/

CWnd* CCPrintDialog::GetPrintCWnd()
{
	// Close the print setup dlg that's opened from the main print dlg
	if (pDlgSetup != NULL)
		return pDlgSetup;

	// Close the main print dlg
	if (pCurrentCCPrintDialog != NULL)
		return pCurrentCCPrintDialog;

	// Close the print setup dlg opened from the File menu
	CCamApp *pCamApp = (CCamApp *)AfxGetApp();
	if (pCamApp != NULL && pCamApp->GetFilePrintSetupDlg() != NULL)
		return pCamApp->GetFilePrintSetupDlg();

	return NULL;
}

/********************************************************************************************

>	BOOL CCPrintDialog::UpdateAppPrinterSettings(BOOL RedrawPrintBorders = TRUE)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/2/96
	Inputs:		RedrawPrintBorders = TRUE if you want any visible print borders redrawing
	Returns:	TRUE if ok, FALSE otherwise
	Purpose:	This updates the printer settings using the currently selected printer.

				The printer info it taken from the CCamApp.  If no printer has been selected yet,
				the system defualt printer is used.

	SeeAlso:	BOOL UpdatePrinterSettings(CPrintDialog*)

********************************************************************************************/

BOOL CCPrintDialog::UpdateAppPrinterSettings(BOOL RedrawPrintBorders)
{
	if (GotPrinterSettings)
	{
		BOOL ok = FALSE;

		CCPrintDialog pd(NULL);

		CCamApp *pCamApp = (CCamApp *)AfxGetApp();

		if(pCamApp != NULL)
		{
			pd.m_pd.hDevNames = pCamApp->GetDevNames();
			pd.m_pd.hDevMode  = pCamApp->GetDevMode();
		}

		ok = UpdatePrinterSettings(&pd);

		return ok;
	}

	return TRUE;
}

/********************************************************************************************

>	BOOL CCPrintDialog::UpdatePrinterSettings(BOOL RedrawPrintBorders = TRUE)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/4/95
	Inputs:		RedrawPrintBorders = TRUE if you want any visible print borders redrawing
	Returns:	TRUE if ok, FALSE otherwise
	Purpose:	Updates the static members that represent the current printer settings Camelot
				is interested in, such a paper size, orientation, etc.
				It uses the DEVMODE structure returned by GetDevMode(), plus member funcs for
				driver, device and port names.

				This sets the static member GotPrinterSettings to TRUE if successful

	SeeAlso:	static BOOL UpdatePrinterSettings()

********************************************************************************************/

BOOL CCPrintDialog::UpdatePrinterSettings(BOOL RedrawPrintBorders)
{
	return (CCPrintDialog::UpdatePrinterSettings(this,pDocument,RedrawPrintBorders));
}

/********************************************************************************************

>	static BOOL CCPrintDialog::UpdatePrinterSettings(CPrintDialog* pPrDlg,Document* pDocument = NULL,BOOL RedrawPrintBorders = TRUE)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/4/95
	Inputs:		pPrDlg 	  = ptr to a CPrintDialog class
				pDocument = ptr to associated doc (NULL means use current doc)
				RedrawPrintBorders = TRUE if you want any visible print borders redrawing
	Returns:	TRUE if ok, FALSE otherwise
	Purpose:	Calls the (DEVMODE*,HDC,Document*) version of this func, extracting the params from the supplied 
				CPrintDialog object

				If a HDC is created in the supplied pPrDlg, it is deleted before the function returns.

	SeeAlso:	-

********************************************************************************************/

BOOL CCPrintDialog::UpdatePrinterSettings(CPrintDialog* pPrDlg,Document* pDocument,BOOL RedrawPrintBorders)
{
	ERROR2IF(pPrDlg == NULL,FALSE,"pPrDlg is NULL");

	// If we have a DC, recreate it as it may no longer be valid.
	if (pPrDlg->m_pd.hDC != NULL)
	{
		::DeleteDC(pPrDlg->m_pd.hDC);
		pPrDlg->m_pd.hDC = NULL;
	}

	Progress Hourglass(_R(IDS_UPDATE_PRINTER_SETTINGS),-1,FALSE);
	Hourglass.Update();

	HDC hdc = pPrDlg->CreatePrinterDC();
	DEVMODE* pDevMode = pPrDlg->GetDevMode();
	BOOL ok = UpdatePrinterSettings(pDevMode,hdc,pDocument,RedrawPrintBorders);

	if (pPrDlg->m_pd.hDC != NULL)
	{
		::DeleteDC(pPrDlg->m_pd.hDC);
		pPrDlg->m_pd.hDC = NULL;
	}

	return ok;
}

/********************************************************************************************

>	static BOOL CCPrintDialog::UpdatePrinterSettings(DEVMODE* pDevMode, HDC hdc, Document* pDocument = NULL,BOOL RedrawPrintBorders = TRUE)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/4/95
	Inputs:		pDevMode = ptr to a DEVMODE used to extract printer settings from
				hdc		 = printer device context (can be NULL)
				pDocument= ptr to associated doc (NULL means use current doc)
				RedrawPrintBorders = TRUE if you want any visible print borders redrawing
	Returns:	TRUE if ok, FALSE otherwise
	Purpose:	Updates the static members that represent the current printer settings Camelot
				is interested in, such a paper size, orientation, etc.

				This sets the static member GotPrinterSettings to TRUE if successful

				It will also redraw all views in the given doc that are showing print borders

	SeeAlso:	-

********************************************************************************************/

BOOL CCPrintDialog::UpdatePrinterSettings(DEVMODE* pDevMode,HDC hdc,Document* pDocument,BOOL RedrawPrintBorders)
{
	if (pDevMode == NULL)
	{
		TRACE( _T("CCPrintDialog::UpdatePrinterSettings() - pDevMode is NULL"));
		GotPrinterSettings = FALSE;
		return FALSE;
	}

	INT32 Fields = pDevMode->dmFields;		// Find out which fields have been initialised

	// ---------------------------------
	// Extract the paper size
	//

	BOOL InitPaperSize = FALSE;

	// First, look-up the paper size if we can, as this is more accurate than using the DC to get the
	// physical width & height plus X & Y dpi
	if (!InitPaperSize && (Fields & DM_PAPERSIZE))
	{
		// The dmPaperSize field has been initialised, so look up the paper dimensions
		// if this value is not 0
		// Note: I discovered a page in MSDN on 7/2/96 that states that a paper size value
		// of DMPAPER_USER means "get size from dmPaperWidth & dmPaperLength"
		if (pDevMode->dmPaperSize != 0 && pDevMode->dmPaperSize != DMPAPER_USER)
			InitPaperSize = LookUpPaperSize(pDevMode->dmPaperSize,&PrPaperSize);
	}

	// Use the DC to find out the paper size
	if (!InitPaperSize && hdc != NULL)
	{
		double xdpi = double(::GetDeviceCaps(hdc,LOGPIXELSX));		// Get X & Y dots (pixels) per inch
		double ydpi = double(::GetDeviceCaps(hdc,LOGPIXELSY));

		ERROR3IF(xdpi == 0,"(papersize) xdpi == 0");
		ERROR3IF(ydpi == 0,"(papersize) ydpi == 0");

		// Find the width and height in inches
		double width  =double(::GetDeviceCaps(hdc,PHYSICALWIDTH))  / xdpi; // 2479.0 
		double height =double(::GetDeviceCaps(hdc,PHYSICALHEIGHT)) / ydpi; // 3504.0 

		PrPaperSize.cx = IN_TO_MP(width);
		PrPaperSize.cy = IN_TO_MP(height);
		InitPaperSize = TRUE;
	}

	if (!InitPaperSize && (Fields & DM_PAPERWIDTH) && (Fields & DM_PAPERLENGTH))
	{
		// OK, we still haven't got the paper size yet AND the width and length fields have been
		// initialised

		// Paper size is defined in dmPaperWidth and dmPaperLength when dmPaperSize == 0 OR dmPaperSize == DMPAPER_USER
		// Each dimension is defined in tenths of a millimeter
		PrPaperSize.cx = MM_TO_MP(double(pDevMode->dmPaperWidth)/10);
		PrPaperSize.cy = MM_TO_MP(double(pDevMode->dmPaperLength)/10);
		InitPaperSize = TRUE;
	}

	if (!InitPaperSize)
	{
		// If we still haven't managed to get the paper size, then error in debug builds
		// and set the paper size to a sensible default
		ERROR3("Unable to extract paper size from given DEVMODE");
		LookUpPaperSize(DMPAPER_A4,&PrPaperSize);
	}

	// ---------------------------------
	// Extract paper orientation
	//

	// Has the orientation field been initialised?
	if (Fields & DM_ORIENTATION)
		PrPortrait = (pDevMode->dmOrientation == DMORIENT_PORTRAIT);
	else
	{
		// We haven't managed to get the paper orientation, so error in debug builds
		// and set the paper orientationto a sensible default
		ERROR3("Unable to extract paper orientation from given DEVMODE");
		PrPortrait = TRUE;
	}

	// ---------------------------------
	// Extract print quality (i.e. resolution in dots per inch)
	//
	// It turns out that the printer quality setting isn't always set in the DEVMODE structure. If this is the
	// case, the hdc param is used to extract the device x & y resolution

	// Set PrResolution to a value that means "yet to find printer resolution"
	PrResolution = -1;

	// Use the DC to find the resolution, in preference to using the DEVMODE structure
	// (Overall DPI is the smallest between the X and Y resolution of the device)
	if (PrResolution <= 0 && hdc != NULL)
	{
		PrResolutionX = ::GetDeviceCaps(hdc,LOGPIXELSX);
		PrResolutionY = ::GetDeviceCaps(hdc,LOGPIXELSY);

		PrResolution = min(PrResolutionX,PrResolutionY);
	}

	// If we failed to get the printer resolution using the DC, try the DEVMODE structure
	if (PrResolution <= 0 && (Fields & (DM_PRINTQUALITY | DM_YRESOLUTION)))
	{
		PrResolution = pDevMode->dmPrintQuality;
		PrResolutionX = PrResolutionY = PrResolution;	// The X & Y res are the same

		// Ensure that PrResolution is a value we understand
		if ((PrResolution < 0) 				&& 
			(PrResolution != DMRES_HIGH) 	&& 
			(PrResolution != DMRES_MEDIUM) 	&& 
			(PrResolution != DMRES_LOW) 	&& 
			(PrResolution != DMRES_DRAFT))
		{
			ERROR3_PF(("Print res is -ve yet not a defined value (%d)",PrResolution));
			PrResolution = DMRES_HIGH;		// Set it to the highest quality device-independent setting
		}
	}

	// If all else fails, fall back on a default value
	if (PrResolution <= 0)
	{
		ERROR3IF(hdc == NULL,"hdc is NULL. Try supplying one in order to get printer resolution.");
		ERROR3("Unable to find the printer resolution");
		PrResolution = 300;
		PrResolutionX = PrResolutionY = PrResolution;	// The X & Y res are the same
	}

	// ---------------------------------
	// Extract the margins of the printable area on the paper
	// (we can only do this via the DC)

	if (hdc != NULL)
	{
		// Find out the various printer bits and bobs, all measured in pixels
		double left,top,width,height;

		if (IsWindows31()) //IsWin32s() && !IsWin32c())
		{
			POINT offsets,size;
			::Escape(hdc,GETPRINTINGOFFSET,NULL,NULL,&offsets);
			::Escape(hdc,GETPHYSPAGESIZE,NULL,NULL,&size);

			left 	= double(offsets.x); // 54.0
			top		= double(offsets.y); // 50.0
			width	= double(size.x);    // 2479.0
			height	= double(size.y);    // 3504.0

			//TRACE( _T("GETPRINTINGOFFSET.x = %ld\n"),offsets.x);
			//TRACE( _T("GETPRINTINGOFFSET.y = %ld\n"),offsets.y);
			//TRACE( _T("GETPHYSPAGESIZE.x   = %ld\n"),size.x);
			//TRACE( _T("GETPHYSPAGESIZE.y   = %ld\n"),size.y);
			//TRACE( _T("\n"));
		}
		else
		{
			left 	= double(::GetDeviceCaps(hdc,PHYSICALOFFSETX));	// left margin		// 54.0  
			top 	= double(::GetDeviceCaps(hdc,PHYSICALOFFSETY));	// top margin		// 50.0  
			width  	= double(::GetDeviceCaps(hdc,PHYSICALWIDTH));	// width of paper 	// 2479.0
			height 	= double(::GetDeviceCaps(hdc,PHYSICALHEIGHT));	// height of paper	// 3504.0

			//TRACE( _T("PHYSICALOFFSETX = %ld\n"),INT32(left));
			//TRACE( _T("PHYSICALOFFSETY = %ld\n"),INT32(top));	
			//TRACE( _T("PHYSICALWIDTH   = %ld\n"),INT32(width));	
			//TRACE( _T("PHYSICALHEIGHT  = %ld\n"),INT32(height));
		}

		double hres	= double(::GetDeviceCaps(hdc,HORZRES));			// width of printable area				2366.0
		double yres	= double(::GetDeviceCaps(hdc,VERTRES));			// height of printable area				3404.0
		double xdpi = double(::GetDeviceCaps(hdc,LOGPIXELSX));		// Get X & Y dots (pixels) per inch		300.0
		double ydpi = double(::GetDeviceCaps(hdc,LOGPIXELSY));		// 										300.0

		ERROR3IF(xdpi == 0,"xdpi == 0");
		ERROR3IF(ydpi == 0,"ydpi == 0");

		//TRACE( _T("HORZRES         = %ld\n"),INT32(hres));			
		//TRACE( _T("VERTRES         = %ld\n"),INT32(yres));			
		//TRACE( _T("LOGPIXELSX      = %ld\n"),INT32(xdpi));
		//TRACE( _T("LOGPIXELSY      = %ld\n"),INT32(ydpi));

		PrLeftMargin 	= IN_TO_MP(left / xdpi);
		PrTopMargin 	= IN_TO_MP(top  / ydpi);
		PrRightMargin 	= IN_TO_MP((width-hres-left) / xdpi);
		PrBottomMargin 	= IN_TO_MP((height-yres-top) / ydpi);
	}
	else
	{
		PrLeftMargin 	= 0;
		PrTopMargin 	= 0;
		PrRightMargin 	= 0;
		PrBottomMargin 	= 0;
	}

	// ---------------------------------
	// Is printer a Postscript printer?
	//
	PrPostscript = FALSE;
	if (hdc != NULL)
	{
		CDC cdc;
		if (cdc.Attach(hdc))
		{
			PrPostscript = (CCDC::GetType(&cdc,TRUE) == RENDERTYPE_PRINTER_PS);
			cdc.Detach();
		}
	}

	// ---------------------------------
	// Extract the scale factor
	//
	if (Fields & DM_SCALE)
	{
		PrScale = pDevMode->dmScale;

		// is the value sensible?
		if (PrScale <= 0)
		{
			ERROR3_PF(("Bad printer scale factor (%d)",PrScale));
			PrScale = 100;
		}
	}
	else
	{
		// We haven't managed to get the print scale, so error in debug builds
		// and set the scale to a sensible default
		//ERROR3("Unable to extract print scale from given DEVMODE");
		PrScale = 100;
	}

	// ---------------------------------
	// Extract the printer's ability to do multiple copies.
	//
	PrMultiCopies = ((Fields & DM_COPIES) != 0);

	// ---------------------------------
	// Final processing on extracted data 
	//

	if (!PrPortrait)
	{
		// If landscape printing, swap the width and height of the paper to which we will print to
		Swap(PrPaperSize.cx,PrPaperSize.cy);
	}

	if (PrScale != 100)
	{
		// If the printer driver is applying a scale factor, scale the real paper size to the apparent 
		// size of the paper after scaling

		double w = PrPaperSize.cx;			// w = width of paper
		double h = PrPaperSize.cy;			// h = height of paper
		double f = 100/double(PrScale);		// f = scale factor applied by printer driver

		PrPaperSize.cx = MILLIPOINT((w*f)+0.5);
		PrPaperSize.cy = MILLIPOINT((h*f)+0.5);
	}

	GotPrinterSettings = TRUE;	// Note that we have a set of valid printer settings now

	// Tell everyone that the print settings have changed
	BROADCAST_TO_ALL(PrintMsg(PrintMsg::SETTINGSCHANGED));

	// Update the document's print control to make sure it recalculates scale factors etc, and
	// redraw any views that are showing their print borders
	if (pDocument == NULL)
		pDocument = Document::GetSelected();

	if (pDocument != NULL)
	{
		PrintControl* pPrCtrl = NULL;
		PrintComponent* pComp = (PrintComponent*)pDocument->GetDocComponent(CC_RUNTIME_CLASS(PrintComponent));
		if (pComp != NULL)
			pPrCtrl = pComp->GetPrintControl();

		if (pPrCtrl != NULL)
			pPrCtrl->SetUp(pDocument->GetSelectedSpread());

		if (RedrawPrintBorders)
		{
			CBitmapCache* pBC = Camelot.GetBitmapCache();
			pBC->DeInitialise();							// Brute force cache clear

			// Redraw all views that are showing their print borders
			FRDocViewFunc pFunc = PrintControl::RedrawPrintableArea;
			pDocument->ForceRedraw(pFunc);
		}
	}

	return (TRUE);
}


/********************************************************************************************

>	static HDC CCPrintDialog::CreatePrinterDC(HGLOBAL hDevNames,HGLOBAL hDevMode)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/12/95
	Inputs:		hDevNames = global memory handle containing a DEVNAMES structure
				hFevMode  = global memory handle containing a DEVMODE  structure
	Returns:	Print DC, or NULL if it fails
	Purpose:	General printer DC generator.
	SeeAlso:	-

********************************************************************************************/

HDC CCPrintDialog::CreatePrinterDC(HGLOBAL hDevNames,HGLOBAL hDevMode)
{
	if (hDevNames == NULL)
		return NULL;

	LPDEVNAMES lpDevNames = (LPDEVNAMES)::GlobalLock(hDevNames);
	LPDEVMODE  lpDevMode = (hDevMode != NULL) ?
						(LPDEVMODE)::GlobalLock(hDevMode) : NULL;

	if (lpDevNames == NULL)
		return NULL;

	HDC hDC = ::CreateDC((LPCTSTR)lpDevNames + lpDevNames->wDriverOffset,
					  (LPCTSTR)lpDevNames + lpDevNames->wDeviceOffset,
					  (LPCTSTR)lpDevNames + lpDevNames->wOutputOffset,
					  lpDevMode);

	::GlobalUnlock(hDevNames);
	if (hDevMode != NULL)
		::GlobalUnlock(hDevMode);

	return hDC;
}

/********************************************************************************************

>	static BOOL CCPrintDialog::IsPostscript()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/6/95
	Inputs:		-
	Returns:	TRUE means the printer is a Postscript printer, FALSE otherwise
	Purpose:	Use this to find out if the printer is a Postscript one or not.
	SeeAlso:	-

********************************************************************************************/

BOOL CCPrintDialog::IsPostscript()
{
	GetPrinterSettings();
	return PrPostscript;
}

/********************************************************************************************

>	static BOOL CCPrintDialog::LookUpPaperSize(UINT32 PaperSizeID,SIZEL* pPaperSize)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/4/95
	Inputs:		PaperSizeID = ID of the paper, i.e. DMPAPER_?
				pPaperSize	= ptr to place to put dimensions in (defined in MILLIPOINTS)
	Returns:	TRUE means found, FALSE otherwise
	Purpose:	This looks up the paper size in our table of standard paper sizes, and puts the dimensions
				in pPaperSize.

				If PaperSizeID > 0, and it's not found, then we're in trouble!

	SeeAlso:	-

********************************************************************************************/

BOOL CCPrintDialog::LookUpPaperSize(UINT32 PaperSizeID,SIZEL* pPaperSize)
{
	ERROR2IF(pPaperSize == NULL,FALSE,"pPaperSize is NULL");

	for(INT32 i=0;pPaperSizes[i].ID != DMPAPER_LISTEND;i++)
	{
		if (pPaperSizes[i].ID == PaperSizeID)
		{
			pPaperSize->cx = pPaperSizes[i].Width;
			pPaperSize->cy = pPaperSizes[i].Height;
			return TRUE;
		}
	}

	ERROR3_PF(("Couldn't find paper size for paper ID %d",PaperSizeID));

	return FALSE;
}

/********************************************************************************************

>	static BOOL CCPrintDialog::GetPaperSize(SIZEL* pPaperSize, BOOL RedrawPrintBorders = TRUE)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/4/95
	Inputs:		pPaperSize = ptr to a SIZEL struct to place the size of the paper into
				RedrawPrintBorders	- True (Default) to force a redraw of docs
	Returns:	TRUE means OK, FALSE otherwise
	Purpose:	Retrieves the physical size of the printer's paper, in MILLIPOINTS

				This size takes into account any processing that is done by the printer driver
				independantly of the application.  This processing is in the form of scaling and 
				rotation (for landscape printing).

				E.g. Say the physical paper is A4 (210mm x 297mm), and the user has set Landscape
				with a scale factor of 50% via the Print Setup... dlg.

				The resultant paper size will be 594mm x 420mm, i.e. because of the setup of the printer
				it is as if we are printing on a landscape piece of paper twice the size of a landscape
				piece of A4.

	SeeAlso:	-

********************************************************************************************/

BOOL CCPrintDialog::GetPaperSize(SIZEL* pPaperSize, BOOL RedrawPrintBorders)
{
	ERROR2IF(pPaperSize == NULL,FALSE,"NULL paper size param");

	if (GetPrinterSettings(RedrawPrintBorders))
	{
		*pPaperSize = PrPaperSize;
		return TRUE;
	}

	return FALSE;
}


/********************************************************************************************

>	static BOOL CCPrintDialog::GetResolution(INT32* pDPI,INT32* pXDPI = NULL,INT32* pYDPI = NULL)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/4/95
	Inputs:		pDPI  = ptr to where to place overall printer resolution
				pXDPI = ptr to X resolution (can be NULL)
				pYDPI = ptr to Y resolution (can be NULL)
	Returns:	TRUE means OK, FALSE otherwise
	Purpose:	Retrieves the printer resolution

				If +ve, *pDPI contains the printer resolution in dots per inch (DPI)

				If -ve, *pDPI specifies a device-independent print quality setting that can be one of the
				following values:

						DMRES_HIGH
						DMRES_MEDIUM
						DMRES_LOW
						DMRES_DRAFT

				*pDPI contains the minimum of the X & Y DPI if these values are deferent.  To find the
				actual X & Y dpi, suppy ptrs in the last two params of this function.
	SeeAlso:	-

********************************************************************************************/

BOOL CCPrintDialog::GetResolution(INT32* pDPI,INT32* pXDPI,INT32* pYDPI)
{
	ERROR2IF(pDPI == NULL,FALSE,"NULL pDPI given");

	if (GetPrinterSettings())
	{
		*pDPI = PrResolution;
		if (pXDPI != NULL) *pXDPI = PrResolutionX;
		if (pYDPI != NULL) *pYDPI = PrResolutionY;
		return TRUE;
	}

	return FALSE;
}

/********************************************************************************************

>	static BOOL CCPrintDialog::GetScale(INT32* pScale)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/4/95
	Inputs:		pScale = ptr to where to place printer scale factor percentage
	Returns:	TRUE means OK, FALSE otherwise
	Purpose:	Retrieves the printer scale factor percentage

				E.g. 100 means 100% (no scaling)
				      50 means 50%
				      etc
	SeeAlso:	-

********************************************************************************************/

BOOL CCPrintDialog::GetScale(INT32* pScale)
{
	ERROR2IF(pScale == NULL,FALSE,"NULL pScale param");

	if (GetPrinterSettings())
	{
		*pScale = PrScale;
		return TRUE;
	}

	return FALSE;
}

/********************************************************************************************

>	static BOOL CCPrintDialog::GetPrintableArea(DocRect* pPrintableArea)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/5/95
	Inputs:		pPrintableArea = ptr to the printable area rect
	Returns:	TRUE means OK, FALSE otherwise
	Purpose:	Retrieves the printable area rect

				This printable area is relative to a piece of paper that has it's origin (0,0)
				at the bottom left.

	SeeAlso:	-

********************************************************************************************/

BOOL CCPrintDialog::GetPrintableArea(DocRect* pPrintableArea)
{
	ERROR2IF(pPrintableArea== NULL,FALSE,"NULL pPrintableArea param");

	if (GetPrinterSettings())
	{
		pPrintableArea->lox = PrLeftMargin;
		pPrintableArea->hix = PrPaperSize.cx - PrRightMargin;

		pPrintableArea->loy = PrBottomMargin;
		pPrintableArea->hiy = PrPaperSize.cy - PrTopMargin;

		return TRUE;
	}

	return FALSE;
}

/********************************************************************************************

>	static BOOL CCPrintDialog::GetPrintMargins(INT32* pLeft = NULL,INT32* pTop = NULL,INT32* pRight = NULL,INT32* pBottom = NULL)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/5/95
	Inputs:		pLeft 	= ptr to left margin value
				pTop 	= ptr to top margin value
				pRight 	= ptr to right margin value
				pBottom = ptr to bottom margin value
	Returns:	TRUE means OK, FALSE otherwise
	Purpose:	Retrieves the print margins

				Any of the params can be NULL, to allow selective use.

	SeeAlso:	-

********************************************************************************************/

BOOL CCPrintDialog::GetPrintMargins(INT32* pLeft,INT32* pTop,INT32* pRight,INT32* pBottom)
{
	if (GetPrinterSettings())
	{
		if (pLeft 	!= NULL) *pLeft 	= PrLeftMargin;
		if (pTop	!= NULL) *pTop		= PrTopMargin;
		if (pRight	!= NULL) *pRight	= PrRightMargin;
		if (pBottom != NULL) *pBottom	= PrBottomMargin;
		return TRUE;
	}

	return FALSE;
}

/********************************************************************************************

>	static BOOL CCPrintDialog::CanMultiCopy(BOOL* pState)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/4/95
	Inputs:		pState = ptr to where to place flag
	Returns:	TRUE means OK, FALSE otherwise
	Purpose:	Retrieves the printer's capability for printing multiple copies

				If TRUE is returned, we should only need to print a page once and get the printer to
				do multiple copies, otherwise we'll have to do each copy ourselves.
	SeeAlso:	-

********************************************************************************************/

BOOL CCPrintDialog::CanMultiCopy(BOOL* pState)
{
	ERROR2IF(pState == NULL,FALSE,"NULL pState param");

	if (GetPrinterSettings())
	{
		*pState = PrMultiCopies;
		return TRUE;
	}

	return FALSE;
}


//---------------------------------------------------
/*****************
// Paper size retrieving code that won't run on Win32s (bugger eh?)

		HANDLE PrHandle;

		if (::OpenPrinter((char*)pDevMode->dmDeviceName,&PrHandle,NULL))
		{
			UINT32 size = sizeof(FORM_INFO_1) + 100;
			BYTE* buffer = (BYTE*)CCMalloc(size);

			if (buffer != NULL)
			{
				DWORD count;
				if (::GetForm(PrHandle,(char*)pDevMode->dmFormName,1,buffer,size,&count))
				{
					FORM_INFO_1* pFormInfo1 = (FORM_INFO_1*)buffer;
					*pPaperSize = pFormInfo1->Size;
					ok = TRUE;
				}
				TRACEUSER( "Markn", _T("GetForm() count = %d\n"),count);
				CCFree(buffer);
			}
		}

		DWORD Err = GetLastError();
*************************/
//---------------------------------------------------

/********************************************************************************************

>	void CCPrintDialog::GetDetails(PrintControl *TargetPC)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/8/95
	Outputs:	TargetPC will be updated with the current settings from the dialogue

	Purpose:	Gets the settings from the various gadgets in the dlg and puts them in 
				LocalPrintControl.
	SeeAlso:	-

********************************************************************************************/

void CCPrintDialog::GetDetails(PrintControl *TargetPC)
{
	// The print range controls
	if (IsDlgButtonChecked(_R(IDC_ALLOBJECTS)))			TargetPC->SetObjPrintRange(PRINTRANGEOBJ_ALL);
	if (IsDlgButtonChecked(_R(IDC_SELECTEDOBJECTS)))	TargetPC->SetObjPrintRange(PRINTRANGEOBJ_SELECTED);

 	// The print pages controls (to do with double page spreads)
	if (IsDlgButtonChecked(_R(IDC_DPSBOTH)))			TargetPC->SetDPSPrintRange(PRINTRANGEDPS_BOTH);
	if (IsDlgButtonChecked(_R(IDC_DPSLEFTPAGES)))		TargetPC->SetDPSPrintRange(PRINTRANGEDPS_LEFTPAGES);
	if (IsDlgButtonChecked(_R(IDC_DPSRIGHTPAGES)))		TargetPC->SetDPSPrintRange(PRINTRANGEDPS_RIGHTPAGES);

	// Print to file check box
	TargetPC->SetPrintToFile(IsDlgButtonChecked(chx1));

	// Num copies ed field
	BOOL TranslatedOK;
	UINT32 NumCopies = GetDlgItemInt(edt3,&TranslatedOK,FALSE);
	if (!TranslatedOK || NumCopies == 0)
		NumCopies = 1;

	TargetPC->SetNumCopies(INT32(NumCopies));

	// Collated check box
	TargetPC->SetCollated(IsDlgButtonChecked(chx2));
}



/********************************************************************************************

>	void CCPrintDialog::OnOK()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/4/95
	Inputs:		-
	Returns:	-
	Purpose:	This takes the values specified in the dlg, and places them in the local
				print control object.

				Then it copies this complete local print control into the doc's print control
				object.

				This is called when the user clicks OK on the main print dialog
	SeeAlso:	-

********************************************************************************************/

void CCPrintDialog::OnOK()
{
	if (PrintPrefsOpen)
	{
		Beep();
		return;
	}

	// Get ptr to document's print control
	PrintControl* pPrCtrl = GetPrintControl();
	ERROR3IF(pPrCtrl == NULL,"Unable to find the print control");

 	// Get the details from the dlg controls
	if (pPrCtrl != NULL)
	{
		// Update the document settings to reflect those just committed in the dialogue
		GetDetails(pPrCtrl);

		// And refresh our local print control from the document one we've just updated
		// just to make sure that we're in step with all the document options
		LocalPrintControl = *pPrCtrl;
	}

	#ifdef _DEBUG
	if (KeyPress::IsConstrainPressed())
	{
		PrintLayoutTab::TestPrinting(pPrCtrl);
		return;
	}
	#endif

	// Before we allow the print to proceed, let's just check for total stupidity...
	TypesetInfo *TInfo = pPrCtrl->GetTypesetInfo();
	if (TInfo->AreSeparating())
	{
		// Are they trying to print colour seps in Bitmap mode? This is fruitless!
		if (pPrCtrl->GetPrintMethod() != PRINTMETHOD_NORMAL)
		{
			if (InformError(_R(IDE_PRINTSEPSASBMP), _R(IDS_PLATEPROCEED), _R(IDS_CANCEL)) != 1)
				return;
		}

		// Are they trying to print a silly number of plates?
		DWORD NumPlates = TInfo->GetNumPrintPlates();
		if (NumPlates < 1)
		{
			// No plates to print - don't allow them to print at all
			InformError(_R(IDE_NOPLATESTOPRINT), _R(IDS_OK));
			return;
		}

		if (NumPlates > 6)
		{
			// There are more than 6 plates set to print - this could be a mistake,
			// and it could be a very costly mistake! Check if the user really
			// meant to do this.
			if (InformError(_R(IDE_MANYPLATESTOPRINT), _R(IDS_PLATEPROCEED), _R(IDS_CANCEL)) != 1)
				return;
		}
	}

	// Finally, call base class function
	CPrintDialog::OnOK();
}

/********************************************************************************************

>	void CCPrintDialog::OnCancel()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/4/95
	Inputs:		-
	Returns:	-
	Purpose:	It copies the original print control into the doc's print control
				object.

				This is called when the user clicks Cancel on the main print dialog
	SeeAlso:	-

********************************************************************************************/

void CCPrintDialog::OnCancel()
{
	if (PrintPrefsOpen)
	{
		Beep();
		return;
	}

/*
	-- Jason here - I reckon if the user cancels, we shouldn't commit their
	-- changes to the document...

	// Get the dlg settings
	GetDetails();

	// Get ptr to document's print control
	PrintControl* pPrCtrl = GetPrintControl();
	ERROR3IF(pPrCtrl == NULL,"Unable to find the print control");

	if (pPrCtrl != NULL)
		// Copy the local print control to the doc's print control
		*pPrCtrl = LocalPrintControl;
*/

	// Finally, call base class function
	CPrintDialog::OnCancel();
}	

/********************************************************************************************

>	BOOL CCPrintDialog::OnInitDialog()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/3/95
	Inputs:		-
	Returns:	TRUE if all ok, FALSE if an error occurs
	Purpose:	Called just before the print dlg is displayed.

				This calls the base class version, then ShowDetails() to reflect the local print
				control in the dialog.

	SeeAlso:	ShowDetails()

********************************************************************************************/

BOOL CCPrintDialog::OnInitDialog()
{
	// Call the base class version first
	CPrintDialog::OnInitDialog();

	// Set the font in the dialog
	if (UnicodeManager::IsDBCSOS())
		FontFactory::ApplyFontToWindow(m_hWnd, STOCKFONT_DIALOG);

	return ShowDetails();
}

/********************************************************************************************

>	BOOL CCPrintDialog::ShowDetails()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/4/95
	Inputs:		-
	Returns:	TRUE if all ok, FALSE if an error occurs
	Purpose:	This sets up all the print dlg controls to reflect the current state of printing
				for the associated document.

	SeeAlso:	OnInitDialog(); OnPrintSetup()

********************************************************************************************/

BOOL CCPrintDialog::ShowDetails()
{
	// Set the name of the device in the control
	String_256 Str = GetDeviceName();
	String_256 Ps  = _R(IDS_PRDLGCTL_CAP_POSTSCRIPT); //"Postscript";
	String_256 ps  = _R(IDS_PRDLGCTL_SML_POSTSCRIPT); //"postscript";
	if (IsPostscript() && (Str.Sub(Ps) == -1) && (Str.Sub(ps) == -1))
		Str += String_32(_R(IDS_PRDLGCTL_BRACKET_POSTSCRIPT)); //" (Postscript)";
	SetDlgItemText(_R(IDC_PRINTERNAME),Str);

	// The print range controls
	PrintRangeObj ObjRange = LocalPrintControl.GetObjPrintRange();
	CheckDlgButton(_R(IDC_ALLOBJECTS), 		ObjRange == PRINTRANGEOBJ_ALL);
	CheckDlgButton(_R(IDC_SELECTEDOBJECTS),	ObjRange == PRINTRANGEOBJ_SELECTED);

	SelRange* pSelRange = GetApplication()->FindSelection();
	BOOL SelectionPresent= (pSelRange->FindFirst() != NULL);
 	Enable(grp1, 				SelectionPresent);
 	Enable(_R(IDC_ALLOBJECTS), 		SelectionPresent);
 	Enable(_R(IDC_SELECTEDOBJECTS),	SelectionPresent);

 	// The print pages controls (to do with double page spreads)
 	BOOL DPS 		 = LocalPrintControl.IsDoublePageSpread();
	BOOL WholeSpread = LocalPrintControl.IsWholeSpread();
 	Enable(_R(IDC_GROUP_PRINTPAGES),DPS && !WholeSpread);
 	Enable(_R(IDC_DPSBOTH), 	 	DPS && !WholeSpread);
 	Enable(_R(IDC_DPSLEFTPAGES), 	DPS && !WholeSpread);
 	Enable(_R(IDC_DPSRIGHTPAGES),	DPS && !WholeSpread);

	PrintRangeDPS DPSRange = LocalPrintControl.GetDPSPrintRange();
	CheckDlgButton(_R(IDC_DPSBOTH),      DPSRange == PRINTRANGEDPS_BOTH);
	CheckDlgButton(_R(IDC_DPSLEFTPAGES), DPSRange == PRINTRANGEDPS_LEFTPAGES);
	CheckDlgButton(_R(IDC_DPSRIGHTPAGES),DPSRange == PRINTRANGEDPS_RIGHTPAGES);

	// Print to file check box
	CheckDlgButton(chx1,LocalPrintControl.GetPrintToFile());

	// Num copies ed field
	SetDlgItemInt(edt3,LocalPrintControl.GetNumCopies());
	Enable(edt3,TRUE);

	// Collated check box
	CheckDlgButton(chx2,LocalPrintControl.IsCollated());
	INT32 NumPages = LocalPrintControl.GetNumPrintablePages();
	Enable(chx2,NumPages > 1);

	// The print info text
	Str = LocalPrintControl.BuildPrintInfoStr();
	SetDlgItemText(_R(IDC_PRINTLAYOUTINFO),Str);

// Code to comment out non-functional aspects of the print dlg (18/4/95)
/*
Enable(chx1,				FALSE);
Enable(_R(IDC_ALLOBJECTS),		FALSE);
Enable(_R(IDC_SELECTEDOBJECTS),	FALSE);
Enable(_R(IDC_GROUP_PRINTPAGES),FALSE);
Enable(_R(IDC_DPSBOTH),			FALSE);
Enable(_R(IDC_DPSLEFTPAGES),	FALSE);
Enable(_R(IDC_DPSRIGHTPAGES),	FALSE);
Enable(chx2,				FALSE);
Enable(grp1,				FALSE);
*/
	return TRUE;
}

/********************************************************************************************

>	void CCPrintDialog::Enable(UINT32 GadgetID, BOOL State)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/3/95
	Inputs:		GadgetID = ID of the dlg item
				State	 = TRUE = enable, FALSE = disable
	Returns:	-
	Purpose:	Enables (ungreys) or disables (greys) contols within the print dlg

	SeeAlso:	-

********************************************************************************************/

void CCPrintDialog::Enable(UINT32 GadgetID,BOOL State)
{
	CWnd* pGadget =  GetDlgItem(GadgetID);
	if (pGadget != NULL)
		pGadget->EnableWindow(State);
}

/********************************************************************************************

>	BOOL CCPrintDialog::OnCommand()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/3/95
	Inputs:		-
	Returns:	TRUE if all ok, FALSE if an error occurs
	Purpose:	Called when something happends to one of our controls

				It processes the UI change, and reflects this change in LocalPrintControl

	SeeAlso:	-

********************************************************************************************/

BOOL CCPrintDialog::OnCommand(UINT32 GadgetID)
{
	switch (GadgetID)
	{
		// Print range radio buttons
		case _R(IDC_ALLOBJECTS): 		LocalPrintControl.SetObjPrintRange(PRINTRANGEOBJ_ALL);		break;
		case _R(IDC_SELECTEDOBJECTS):	LocalPrintControl.SetObjPrintRange(PRINTRANGEOBJ_SELECTED);	break;

		// Print pages radio buttons
		case _R(IDC_DPSBOTH):		LocalPrintControl.SetDPSPrintRange(PRINTRANGEDPS_BOTH);			break;
		case _R(IDC_DPSLEFTPAGES):	LocalPrintControl.SetDPSPrintRange(PRINTRANGEDPS_LEFTPAGES);	break;
		case _R(IDC_DPSRIGHTPAGES):	LocalPrintControl.SetDPSPrintRange(PRINTRANGEDPS_RIGHTPAGES);	break;

		case _R(IDC_PRINTOPTIONS):
		{
			if (PrintPrefsOpen)
				break;

			// Get ptr to document's print control
			PrintControl* pPrCtrl = GetPrintControl();
			ERROR3IF(pPrCtrl == NULL,"Unable to find the print control");

			if (pPrCtrl != NULL)
			{
				// Get the dlg settings
				// We have to commit the settings to the document now, or else they won't be copied through to
				// the options dialogue. Thus, the Options button effectively soft-commits the values.
				GetDetails(pPrCtrl);

				PrintPrefsOpen = TRUE;

				ControlHelper::InformModalDialogOpened();

				// Open print prefs dlg
				if (PrintPrefsDlg::InvokeDialog())
					PrintPrefsChanged = TRUE;

				ControlHelper::InformModalDialogClosed();

				PrintPrefsOpen = FALSE;
				SetFocus();
			}

			// Now copy the document options (which may have been changed by the options dialogue)
			// back into our local print control, and make sure we update the dialogue as well.
			// Just to be really safe, I find the print control from scratch
			pPrCtrl = GetPrintControl();
			ERROR3IF(pPrCtrl == NULL,"Unable to find the print control");
			if (pPrCtrl != NULL)
				LocalPrintControl = *pPrCtrl;
			ShowDetails();
			break;
		}

		case _R(IDC_PRINTHELP):
		{
			// Get the dlg settings
//			GetDetails();

			// We need help!
			HelpUser(*this);
			break;
		}

		default:
			ERROR3("Unknown custom print dialog command");
			break;
	}

	// If a gadget that could effect the appearence of print borders has been clicked,
	// better redraw the print borders
	if ((GadgetID == _R(IDC_DPSBOTH)) || (GadgetID == _R(IDC_DPSLEFTPAGES)) || (GadgetID == _R(IDC_DPSRIGHTPAGES)))
	{
		PrintControl* pPrCtrl = GetPrintControl();
		if (pPrCtrl != NULL)
		{
			pPrCtrl->SetDPSPrintRange(LocalPrintControl.GetDPSPrintRange());
			
			Document* pDoc = pDocument;
			// Redraw all views that are showing their print borders
			if (pDoc == NULL)
				pDoc = Document::GetSelected();
			if (pDoc != NULL)
			{
				FRDocViewFunc pFunc = PrintControl::RedrawPrintableArea;
				pDoc->ForceRedraw(pFunc);
			}
		}
	}

	// Update the dlg controls in case something has changed.
	ShowDetails();

	return TRUE;
}

/********************************************************************************************

>	void CCPrintDialog::OnPrintSetup()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/4/95
	Inputs:		-
	Returns:	-
	Purpose:	This is called when the user clicks on Print setup...

				It calls the base class OnPrintSetup(), and when it returns, updates the
				printer settings static members.

				It also calls ShowDetails() to keep the dlg up to date.
	SeeAlso:	-

********************************************************************************************/

void CCPrintDialog::OnPrintSetup()
{
	// Get the dlg control settings
	PrintControl* pPrCtrl = GetPrintControl();
	ERROR3IF(pPrCtrl == NULL,"Unable to find the print control");

	if (pPrCtrl != NULL)
	{
		GetDetails(pPrCtrl);			// Commit all the current settings into the document
		LocalPrintControl = *pPrCtrl;	// And re-synchronise our local printcontrol with the doc settings
	}

	// Bring up the print setup dlg
//	CPrintDialog::OnPrintSetup();
	{
  		ASSERT_VALID(this);

		ERROR3IF(pDlgSetup != NULL,"Er, why is pDlgSetup not NULL?");

		pDlgSetup = AttachOnSetup();

		ERROR3IF(pDlgSetup == NULL,"AttachOnSetup() returned NULL");
		
		if (pDlgSetup != NULL)
		{
			AfxHookWindowCreate(pDlgSetup);
			Default();
			AfxUnhookWindowCreate();

			delete pDlgSetup;
			pDlgSetup = NULL;
		}
	}


	FixFPControlRegister();

	// In Win3.1 after the print setup dlg has been used, we have to close the main print dlg in order 
	// to get the up-to-date hDevNames and hDevMode handles in the m_pd structure (these are up-to-date 
	// at point which ::PrintDlg() returns).
	//
	// This means we must flag that we have prematurely closed the print dlg, so that it can
	// be reopened again (this is handled in CCPrintDialog::DoModal()).

	ReopenMainDlg = IsWindows31(); //(IsWin32s() && !IsWin32c());

	if (ReopenMainDlg)
		PostMessage(WM_CLOSE);
	else
	{
		if (!IgnorePrintData())
		{
			UpdatePrinterSettings();

			// Set up print control again so that it takes the new print setup on board.
			if (pPrCtrl != NULL)
				LocalPrintControl = *pPrCtrl;	// Re-synchronise our local printcontrol with the doc settings

			LocalPrintControl.SetUp(Document::GetSelectedSpread());

			// Update the dlg controls in case something has changed.
			ShowDetails();
		}
	}
}


//-----------------------------------------------------------------------------------------

/*************************************************************
// This code will retrieve the system-wide DEVMODE settings of printer driver
// It is not too useful to us, as we want the application-wide settings that override the
// system-wide settings, and hence can be different
*********************************************/

typedef INT32 (FAR WINAPI *EXTDEVICEMODEPROC)(HWND,HANDLE,LPDEVMODE,LPSTR,LPSTR,LPDEVMODE,LPSTR,WORD);

/********************************************************************************************

>	DEVMODE* CCPrintDialog::GetSystemDevMode(HWND hWnd,LPCTSTR pDriverName,LPCTSTR pDeviceName,LPCTSTR pPortName)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/4/95
	Inputs:		hWnd = window handle
				pDriverName = ptr to driver name (e.g. "winspool")
				pDeviceName = ptr to device name (e.g. "\\DEEPTHOUGHT\Hobbes")
				pPortName	= ptr to port name (e.g. "Ne00:")
	Returns:	ptr to DEVMODE defining system-wide print settings for the currenly selected printer
	Purpose:	******* COMMENTS ***********
	SeeAlso:	-

********************************************************************************************/

DEVMODE* CCPrintDialog::GetSystemDevMode(HWND hWnd,LPCTSTR pDriverName,LPCTSTR pDeviceName,LPCTSTR pPortName)
{
	if (pDriverName == NULL || pDeviceName == NULL || pPortName == NULL)
	{
		ERROR3("Given NULL entry params");
		return NULL;
	}

	DEVMODE* pDevMode = NULL;

	CString DriverName = pDriverName;	// GetDriverName()
	DriverName += ".drv";

	HMODULE hMod = GetModuleHandle(DriverName);
	if (hMod != NULL)
	{
		EXTDEVICEMODEPROC ExtDeviceMode;

		ExtDeviceMode = (EXTDEVICEMODEPROC) GetProcAddress(hMod,"ExtDeviceMode");
		if (ExtDeviceMode != NULL)
		{
			LPSTR pDevice = (LPSTR)CCMalloc(100);
			LPSTR pPort   = (LPSTR)CCMalloc(100);

			if (pDeviceName != NULL && pPortName != NULL)
			{
				camStrncpy(pDevice,pDeviceName,98);
				camStrncpy(pPort,pPortName,98);
				pDevice[99] = 0;
				pPort[99] = 0;

				//camStrncpy(pDeviceName,PrDlg.GetDeviceName(),200);
				//camStrncpy(pPortName,PrDlg.GetPortName(),200);

				INT32 size = ExtDeviceMode(hWnd,hMod,NULL, pDevice,pPort, 0, 0, 0);
				if (size > 0)
				{
					pDevMode = (DEVMODE*)CCMalloc(size);

					if (pDevMode != NULL)
					{
						pDevMode->dmFields = 0xffffffff;

						INT32 r = ExtDeviceMode(hWnd,hMod,pDevMode, pDevice,pPort, 0,0, DM_COPY);
						if (r != IDOK)
						{
					 		CCFree(pDevMode);
							pDevMode = NULL;
						}
					}					
				}
			}
			if (pDevice != NULL) CCFree(pDevice);
			if (pPort != NULL) CCFree(pPort);
		}
	}			

	return pDevMode;
}

//-----------------------------------------------------------------------------------------

/********************************************************************************************

>	CCPrintInfo::CCPrintInfo()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/3/95
	Inputs:		-
	Returns:	-
	Purpose:	Default constructor
	SeeAlso:	CCPrintInfo::CCPrintInfo(Document* pDoc)

********************************************************************************************/

CCPrintInfo::CCPrintInfo()
{
	ERROR3("Default constructor called.  Use other constructor");
	pOurPD 		= NULL;
	pOriginalPD = NULL;
	Initialised = FALSE;
	pDocument	= NULL;
}


/********************************************************************************************

>	CCPrintInfo::CCPrintInfo(Document* pDoc,CCamView* pCCamVw)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/3/95
	Inputs:		pDoc 	= ptr to doc associated with the print operation
				pCCamVw	= ptr to the cam view that's doing the print job
	Returns:	-
	Purpose:	Main constructor

				This replaces the m_pPD ptr created by the base CPrintInfo class with a ptr to
				a CCPrintDialog

	SeeAlso:	-

********************************************************************************************/

CCPrintInfo::CCPrintInfo(Document* pDoc,CCamView* pCCamVw)
{
	pOurPD 		= NULL;
	pOriginalPD = NULL;
	Initialised = FALSE;
	Printing	= FALSE;
	pDocument	= pDoc;
	pPrCtrl		= NULL;
	pPrgDlg		= NULL;
	pCCamView	= pCCamVw;

	if (pDocument != NULL && pCCamView != NULL)
	{
		pOurPD = new CCPrintDialog(pDocument);

		if (pOurPD != NULL)
		{
			pOriginalPD = m_pPD;
			m_pPD = pOurPD;
			Initialised = TRUE;
		}
		else
		{	ERROR3("Unable to create a CCPrintDialog object"); }
	}

	ERROR3IF(pDocument == NULL,"CCPrintInfo has NULL document ptr");
	ERROR3IF(pCCamView == NULL,"CCPrintInfo has NULL CCamView ptr");

	ERROR3IF(CCPrintInfo::pCurrent != NULL,"Constructing a new CCPrintInfo before destructing the old one");
	CCPrintInfo::pCurrent = this;
}

/********************************************************************************************

>	CCPrintInfo::~CCPrintInfo()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/3/95
	Inputs:		-
	Returns:	-
	Purpose:	Default destructor

				This tidies up by restoring the m_pPD member back to how the base class constructed it
				and destroys the CCPrintDialog it made on initialisation

	SeeAlso:	-

********************************************************************************************/

CCPrintInfo::~CCPrintInfo()
{
	ERROR3IF(!Initialised,"CCPrintInfo being destructed without successful construction");

	if (Initialised)
	{
		// restore base classes original CPrintDialog ptr
		m_pPD = pOriginalPD;

		// Delete our CCPrintDialog
		if (pOurPD != NULL)
			delete pOurPD;

		// Delete our print progress dialog
		if (pPrgDlg != NULL)
			delete pPrgDlg;
	}

	CCPrintInfo::pCurrent = NULL;
}


/********************************************************************************************

>	BOOL CCPrintInfo::StartPrinting()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/4/95
	Inputs:		-
	Returns:	TRUE if all OK, FALSE otherwise
	Purpose:	This informs the print info object that you are about to do some printing.

				It also creates and displays the print progress dialog.

				This is purely to set up the print layout mechanism.  It will work out how many 
				pieces of paper will be printed to, and how many times the page will be printed
				on each piece of of paper (for multiple fit printing).

				CCPrintInfo just provides easy access to the functions provided by the PrintControl class.
				It means you don't have to worry about getting hold of the right PrintControl object from
				the right document - this is all taken care of for you.

				The mechanism works like this:

				if (StartPrinting())
				{
					while (GetNextPaper())
					{
						while (GetNextPatch())
						{
							Use access funcs to get hold of data set up by GetNextPath(), e.g. scale, translation, etc
						
							if (SelectedOnly)
								print all selected objects that lie within the ClipRect area in the spread
							else
								print all objects that lie within the ClipRect area in the spread
						}
					}

					EndPrinting()
				}

	SeeAlso:	GetNextPaper(), GetNextPath()

********************************************************************************************/

BOOL CCPrintInfo::StartPrinting()
{
	ERROR2IF(Printing,FALSE,"StartPrinting() called before EndPrinting()");
	Printing = TRUE;

	// If not initialised properly, don't start printing
	ERROR2IF(!Initialised,FALSE,"Not initialised");
	ERROR2IF(pOurPD == NULL,FALSE,"NULL CCPrintDialog ptr");
	ERROR2IF(pDocument == NULL,FALSE,"Document ptr is NULL");
	ERROR2IF(pCCamView == NULL,FALSE,"CCamView ptr is NULL");

	// Get ptr to the PrintControl object
	pPrCtrl = pOurPD->GetPrintControl();
	ERROR2IF(pPrCtrl == NULL,FALSE,"The print dialog can't find attached print control object");
	if (!pPrCtrl->StartPrinting())
	{
		pPrCtrl = NULL;
		return FALSE;
	}

	// Start up the print marks manager too
	PrintMarksMan* pMarksMan = GetApplication()->GetMarksManager();
	if (pMarksMan)
		pMarksMan->StartPrinting();

	ERROR3IF(pPrgDlg != NULL,"StartPrinting() called with non-NULL pPrgDlg. Did you call EndPrinting() last time?");

	ControlHelper::InformModalDialogOpened();

	// Create and initialise the print progress dialog
	pPrgDlg = new PrintProgressDlg(pCCamView);
	if (pPrgDlg == NULL)
	{
		pPrCtrl->EndPrinting();
		pPrCtrl = NULL;
		return FALSE;
	}

	//pPrgDlg->SetDocName(pDocument->GetTitle());
	pPrgDlg->SetPrinterName(pOurPD->GetDeviceName());
	pPrgDlg->SetPortName(pOurPD->GetPortName());
	pPrgDlg->SetSliderPos(0);

	pPrgDlg->Show();


	return TRUE;
}

/********************************************************************************************

>	BOOL CCPrintInfo::EndPrinting()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/4/95
	Inputs:		-
	Returns:	TRUE if all OK, FALSE otherwise
	Purpose:	This informs the print info object that you have finished printing.
				It also closes the print progress dialog (if there is one)
	SeeAlso:	StartPrinting();

********************************************************************************************/

BOOL CCPrintInfo::EndPrinting()
{
	ERROR2IF(!Printing,FALSE,"EndPrinting() called before StartPrinting()");
	Printing = FALSE;

	// End the print control structure
	ERROR2IF(pPrCtrl == NULL,FALSE,"Called with NULL PrintControl ptr.  Have you called StartPrinting()?");
	BOOL ok = pPrCtrl->EndPrinting();
	pPrCtrl = NULL;

	// End the print marks manager too
	PrintMarksMan* pMarksMan = GetApplication()->GetMarksManager();
	if (pMarksMan)
		pMarksMan->EndPrinting();

	// now destroy the print dialogue
	if (pPrgDlg != NULL)
	{
		pPrgDlg->DestroyWindow();
		delete pPrgDlg;
		pPrgDlg = NULL;
	}		

	ControlHelper::InformModalDialogClosed();

	return (ok);
}

/********************************************************************************************

>	BOOL CCPrintInfo::GetNextPaper()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/4/95
	Inputs:		-
	Returns:	TRUE if you should print on another piece of paper
				FALSE if the print job should end
	Purpose:	Call this for each piece of paper to be printed to.
				NB: You must call StartPrinting() before calling this func as part of your print loop
	SeeAlso:	StartPrinting();

********************************************************************************************/

BOOL CCPrintInfo::GetNextPaper()
{
	ERROR2IF(!Printing,FALSE,"GetNextPaper() called before StartPrinting()");
	ERROR2IF(pPrCtrl == NULL,FALSE,"Called with NULL PrintControl ptr.  Have you called StartPrinting()?");
	return (pPrCtrl->GetNextPaper());
}


/********************************************************************************************

>	BOOL CCPrintInfo::MorePaper() const

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/6/96
	Inputs:		-
	Returns:	TRUE if there is another piece of paper to come
				FALSE if the print job should end
	Purpose:	Call this to determin when to finish the print run of paper
				NB: You must call StartPrinting() before calling this func as part of your print loop
	SeeAlso:	StartPrinting();

********************************************************************************************/

BOOL CCPrintInfo::MorePaper() const
{
	ERROR2IF(!Printing,FALSE,"MorePaper() called before StartPrinting()");
	ERROR2IF(pPrCtrl == NULL,FALSE,"Called with NULL PrintControl ptr.  Have you called StartPrinting()?");
	return (pPrCtrl->MorePaper());
}


/********************************************************************************************

>	BOOL CCPrintInfo::ReprintPaper()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/4/95
	Inputs:		-
	Returns:	TRUE if all OK, FALSE otherwise
	Purpose:	This allows you to print on the same piece of paper again.

				If you call this func after a call to GetNextPaper(), then the next time you
				call GetNextPaper() you will actually be starting the print on the same piece of
				paper.

	SeeAlso:	GetNextPaper()

********************************************************************************************/

BOOL CCPrintInfo::ReprintPaper()
{
	ERROR2IF(!Printing,FALSE,"ReprintPaper() called before StartPrinting()");
	ERROR2IF(pPrCtrl == NULL,FALSE,"Called with NULL PrintControl ptr.  Have you called StartPrinting()?");
	return (pPrCtrl->ReprintPaper());
}


/********************************************************************************************

>	BOOL CCPrintInfo::GetNextPatch(PrintPatchInfo* pPatchInfo)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/4/95
	Inputs:		pPatchInfo = ptr to object to store patch info in
	Outputs:	-
	Returns:	TRUE if you should print another page on the paper
				FALSE if the piece of paper printed to is finished.

	Purpose:	If this call returns TRUE, then the given PrintPatchInfo has been set up with
				all the data needed to print the given patch.

				NB: You must call GetNextPaper() before calling this func as part of your print loop

				It automatically updates any relevent fields within the print progress dialog, if there
				is one.

	SeeAlso:	StartPrinting(), GetNextPaper()

********************************************************************************************/

BOOL CCPrintInfo::GetNextPatch(PrintPatchInfo* pPatchInfo)
{
	ERROR2IF(!Printing,FALSE,"GetNextPatch() called before StartPrinting()");
	ERROR2IF(pPrCtrl == NULL,FALSE,"Called with NULL PrintControl ptr.  Have you called StartPrinting()?");

	BOOL ok = pPrCtrl->GetNextPatch(pPatchInfo);

	if (ok && pPrgDlg != NULL)
	{
		// Determine how many printing plates we're doing, and which one we're up to...
		INT32 MaxPlates = 0;		// If doing a composite print, we pass in 0
		INT32 CurrentPlate = 1;
		String_128 PlateName(TEXT("-"));

		if (GetPrintControl() != NULL)
		{
			TypesetInfo *TSInfo = GetPrintControl()->GetTypesetInfo();
			if (TSInfo != NULL && TSInfo->AreSeparating())
			{
				MaxPlates = (INT32) TSInfo->GetNumPrintPlates();
				CurrentPlate = MaxPlates - TSInfo->NumPrintPlatesRemaining();

				ColourPlate *ThisPlate = TSInfo->GetCurrentPrintPlate();
				if (ThisPlate != NULL)
					ThisPlate->GetDescription((StringBase *) &PlateName);
			}
		}

		// And set the page, plate, and tile numbers
		pPrgDlg->SetPageNumber(	pPatchInfo->PaperNumber, pPatchInfo->MaxPaperNumber,
								CurrentPlate, MaxPlates, (TCHAR *) PlateName,
								pPatchInfo->PatchNumber, pPatchInfo->MaxPatchNumber);
	}

	return ok;
}

/********************************************************************************************

>	BOOL CCPrintInfo::StartPlatePrinting(PrintView *pPrintView, UINT32 *const pErrID)
	BOOL CCPrintInfo::SetNextPlate(PrintView *pPrintView)
	BOOL CCPrintInfo::MorePlates()
	void CCPrintInfo::EndPlatePrinting(PrintView *pPrintView)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/6/96
	Inputs:		-
	Returns:	-
	Purpose:	Fast error checking connection functions to the PrintControl associated
				class functions. See the headers for these functions for details.

********************************************************************************************/

BOOL CCPrintInfo::StartPlatePrinting(PrintView *pPrintView, UINT32 *const pErrID)
{
	ERROR2IF(!Printing,FALSE,"StartPlatePrinting() called before StartPrinting()");
	ERROR2IF(pPrCtrl == NULL,FALSE,"Called with NULL PrintControl ptr.  Have you called StartPrinting()?");
	return (pPrCtrl->StartPlatePrinting(pPrintView, pErrID));
}

BOOL CCPrintInfo::MorePlates()
{
	ERROR2IF(!Printing,FALSE,"MorePlates() called before StartPrinting()");
	ERROR2IF(pPrCtrl == NULL,FALSE,"Called with NULL PrintControl ptr.  Have you called StartPrinting()?");
	return (pPrCtrl->MorePlates());
}

BOOL CCPrintInfo::SetNextPlate(PrintView *pPrintView)
{
	ERROR2IF(!Printing,FALSE,"SetNextPlate() called before StartPrinting()");
	ERROR2IF(pPrCtrl == NULL,FALSE,"Called with NULL PrintControl ptr.  Have you called StartPrinting()?");
	return (pPrCtrl->SetNextPlate(pPrintView));
}

void CCPrintInfo::EndPlatePrinting(PrintView *pPrintView)
{
	ERROR3IF(!Printing,"EndPlatePrinting() called before StartPrinting()");
	ERROR3IF(pPrCtrl == NULL,"Called with NULL PrintControl ptr.  Have you called StartPrinting()?");
	pPrCtrl->EndPlatePrinting(pPrintView);
}




/********************************************************************************************

>	void CCPrintInfo::SetSliderSubRangeMax(INT32 Max)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/4/95
	Inputs:		Max = The maximum value the sub-range will reach
	Outputs:	-
	Returns:	-
	Purpose:	Defines the maximum value you will call SetSliderSubRange() with.
				The slider is split up into sub-ranges, each range represents a patch (or tile) that
				will be printed.  The slider is automatically set to the start of a sub-range when
				GetNextPatch() is called.  
				
				After GetNextPatch() is called, call this func to define the range of values for 
				the slider sub-range to represent.  Then call SetSliderSubRanngePos() each time the 
				progress of the patch changes.

				This will only have an effect if StartPrinting() has been successfully called.
				If there isn't a progress bar (for what ever reason) this function does nothing (not
				even error).

	SeeAlso:	StartPrinting(), SetSliderSubRangePos()

********************************************************************************************/

void CCPrintInfo::SetSliderSubRangeMax(INT32 Max)
{
	if (pPrgDlg != NULL)
		pPrgDlg->SetSliderSubRangeMax(Max);
}

/********************************************************************************************

>	void CCPrintInfo::SetSliderSubRangePos(INT32 Pos)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/4/95
	Inputs:		Pos = value for the current sub-range
	Outputs:	-
	Returns:	-
	Purpose:	Sets the slider so that it represents this value within the current sub-range.
				Before Pos is used, it is brought into range such that 0 <= Pos <= MaxSubRange
				('MaxSubRange' is set via SetSliderSubRangeMax())

				This will only have an effect if StartPrinting() has been successfully called.
				If there isn't a progress bar (for what ever reason) this function does nothing (not
				even error).

	SeeAlso:	StartPrinting(), SetSliderSubRangeMax()

********************************************************************************************/

void CCPrintInfo::SetSliderSubRangePos(INT32 Pos)
{
	if (pPrgDlg != NULL)
		pPrgDlg->SetSliderSubRangePos(Pos);
}

/********************************************************************************************

>	void CCPrintInfo::SetAnalysing()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/5/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	This tells the associated print progress dlg that we are not yet printing the doc,
				we are currently running some analysis on it.  This will display "Analysing <doc name>"
				instead of "Printing <doc name>"

	SeeAlso:	-

********************************************************************************************/

void CCPrintInfo::SetAnalysing()
{
	if (pPrgDlg != NULL)
	{
		pPrgDlg->SetAnalysing();
		if (pDocument != NULL)
			pPrgDlg->SetDocName(pDocument->GetTitle());
	}
}

/********************************************************************************************

>	void CCPrintInfo::SetPrinting()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/5/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	This tells the associated print progress dlg that we have started printing the doc.  
				This will display "Printing <doc name>" instead of "Analysing <doc name>"

	SeeAlso:	-

********************************************************************************************/

void CCPrintInfo::SetPrinting()
{
	if (pPrgDlg != NULL)
	{
		pPrgDlg->SetPrinting();
		if (pDocument != NULL)
			pPrgDlg->SetDocName(pDocument->GetTitle());
	}
}

/********************************************************************************************

>	INT32 CCPrintInfo::SetAbortProc(CDC* pCDC)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/4/95
	Inputs:		pCDC = ptr to CDC used for printing
	Outputs:	-
	Returns:	0 means OK, non-zero means ERROR
	Purpose:	Calls pCDC->SetAbortProc() with the appropriate abort proc
	SeeAlso:	CDC::SetAbortProc()

********************************************************************************************/

INT32 CCPrintInfo::SetAbortProc(CDC* pCDC)
{
	ERROR2IF(pCDC == NULL,SP_ERROR,"Given NULL CDC ptr");

	return (pCDC->SetAbortProc(PrintProgressDlg::AbortProc));
}

/********************************************************************************************

>	BOOL CCPrintInfo::Abort()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/4/95
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the user has aborted the print job, FALSE otherwise

	Purpose:	Calls the print progress dlg's abort proc to see if the user has aborted the print job
		
				NB: The return value is the reverse of the origin logic used in the first working
				print code
	SeeAlso:	-

********************************************************************************************/

BOOL CCPrintInfo::Abort()
{
	return (!PrintProgressDlg::AbortProc(0,0));
}

/********************************************************************************************

>	BOOL CCPrintInfo::SetUpDocInfo(DOCINFO* pDocInfo)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/5/95
	Inputs:		ppDocInfo = ptr to place to put a ptr to a DOCINFO structure
	Outputs:	ppDocInfo = contains ptr to an initialised DOCINFO struct.
	Returns:	TRUE if ok, FALSE otherwise

	Purpose:	Creates and sets up a DOCINFO structure, ready for a call to StartDoc().
				If printing to file, this func will ensure that the lpszOutput member points to
				a valid file name.

				pDocInfo contains the ptr to the DOCINFO on exit.  It is up to the caller to delete
				this object when no longer needed

	SeeAlso:	-

********************************************************************************************/

BOOL CCPrintInfo::SetUpDocInfo(DOCINFO** ppDocInfo)
{
	ERROR2IF(ppDocInfo == NULL,FALSE,"ppDocInfo param is NULL");
	ERROR2IF(pDocument == NULL,FALSE,"pDocument is NULL");

	// Create a new DOCINFO struct so that we are compatible across all OSs
	NEW_DOCINFO* pNewDocInfo = new NEW_DOCINFO;

	BOOL ok = (pNewDocInfo != NULL);

	if (ok)
	{
		// Zero all struct members
		memset(pNewDocInfo,0,sizeof(NEW_DOCINFO));

		// Are we in Win95?
		if (IsWindows95())	//IsWin32c())
		{
			// Win95, so make this look like the new DOCINFO struct by making cbSize the size of
			// the new struct.
			// Also set fwType to be DI_APPBANDING.
			pNewDocInfo->cbSize = sizeof(NEW_DOCINFO);
			pNewDocInfo->fwType = DI_APPBANDING;
		}
		else
			pNewDocInfo->cbSize = sizeof(DOCINFO);		// Make it Win3.1 & WinNT compatible

		// set up document title
		DocTitle = pDocument->GetTitle();
		if (DocTitle.GetLength() > 31)
			DocTitle.ReleaseBuffer(31);

		pNewDocInfo->lpszDocName = DocTitle;
		pNewDocInfo->lpszOutput = NULL;

		ERROR2IF(pOurPD == NULL,FALSE,"NULL CCPrintDialog ptr");

		// Get ptr to the PrintControl object
		if (pPrCtrl == NULL)
			pPrCtrl = pOurPD->GetPrintControl();

		ERROR3IF(pPrCtrl == NULL,"Unable to get ptr to the PrintControl object");

		if (pPrCtrl != NULL && pPrCtrl->GetPrintToFile())
		{
			// Keep Control Helper system informed
			ControlHelper::InformModalDialogOpened();

			String_256 Filt(_R(IDS_PRDLGCTL_PRINT_FILES_FILT_DLG));

			CCPrintToFileDialog PrintToFileDlg("prn", (TCHAR *)Filt);
			if (PrintToFileDlg.DoModal() == IDOK)
				pNewDocInfo->lpszOutput = PrintToFileDlg.GetFullPathNamePtr();
			else
				ok = FALSE;		// Abort printing if Cancel is clicked

			// Keep Control Helper system informed
			ControlHelper::InformModalDialogClosed();
		}
	}

	if (!ok && pNewDocInfo != NULL)
	{
		delete pNewDocInfo;
		pNewDocInfo = NULL;
	}

	ERROR2IF(ok && pNewDocInfo == NULL,FALSE,"Returning TRUE but we have a NULL DOCINFO ptr");

	*ppDocInfo = (DOCINFO*)pNewDocInfo;

	return ok;
}

//-----------------------------------------------------------------------------------------

#endif //webster

/********************************************************************************************

>	CCPrintToFileDialog::CCPrintToFileDialog(TCHAR *DefExt, TCHAR *FilterString)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/5/95
	Inputs:		DefExt			- Default filter extension
				FilterString	- String to pass to the CFileDialog constructor dictating
																	the filters for the dialog
	Outputs:	-
	Returns:	-
	Purpose:	Default constructor for the PrintToFile dialog
	SeeAlso:	-

********************************************************************************************/

#if 0
CCPrintToFileDialog::CCPrintToFileDialog() : CFileDialog(	FALSE,
															"prn",
															FileName,
															OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
															"Print files (*.prn) | *.prn | All files (*.*) | *.* ||",
															NULL)
{
	m_ofn.lpstrTitle = (TCHAR *)(String_64(_R(IDS_PRDLGCTL_PRINT_TO_FILE))); //"Print To File";
}
#endif

CCPrintToFileDialog::CCPrintToFileDialog(TCHAR *DefExt, TCHAR *FilterString) :
	 CFileDialog(FALSE,	DefExt, FileName, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, FilterString, NULL)
{
//	String_256 Filt(_R(IDS_PRDLGCTL_PRINT_FILES_FILT_DLG)); // "Print files (*.prn) | *.prn | All files (*.*) | *.* ||"
//	CFileDialog::CFileDialog(FALSE, "prn", FileName,	OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, (TCHAR *)Filt, NULL)
	
	Title = String_64(_R(IDS_PRDLGCTL_PRINT_TO_FILE));
	m_ofn.lpstrTitle = (TCHAR *)(Title);

  	// Translate filter into commdlg format (lots of \0)
//	m_strFilter = (TCHAR *)Filt;
//	TCHAR *pch = (TCHAR *)Filt; // modify the buffer in place

	// MFC delimits with '|' not '\0'
//	while ((pch = camStrchr(pch, '|')) != NULL)
//		*pch++ = '\0';

//	m_ofn.lpstrFilter = m_strFilter;

}

/********************************************************************************************

>	CCPrintToFileDialog::~CCPrintToFileDialog()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/5/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Default destructor for the PrintToFile dialog
	SeeAlso:	-

********************************************************************************************/

CCPrintToFileDialog::~CCPrintToFileDialog()
{
}


/********************************************************************************************

>	INT32 CCPrintToFileDialog::DoModal()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/5/95
	Inputs:		-
	Outputs:	-
	Returns:	IDOK or IDCANCEL
	Purpose:	Displays the PrintToFile dlg modally, and returns the user's result
	SeeAlso:	-

********************************************************************************************/

INT32 CCPrintToFileDialog::DoModal()
{
	INT32 Result = CFileDialog::DoModal();

	if (Result == IDOK)
	{
		camStrncpy(FileName,GetPathName(),FILENAMEBUFSIZE);
		FileName[FILENAMEBUFSIZE-1] = '\0';
	}

	return Result;
}

/********************************************************************************************

>	LPSTR CCPrintToFileDialog::GetFullPathNamePtr()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/5/95
	Inputs:		-
	Outputs:	-
	Returns:	Ptr to the full path name entered by the user
	Purpose:	Access to the static buffer that contains the full path name for the file to print to
	SeeAlso:	-

********************************************************************************************/

LPSTR CCPrintToFileDialog::GetFullPathNamePtr()
{
	return FileName;
}


