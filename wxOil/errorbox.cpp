// $Id: errorbox.cpp 662 2006-03-14 21:31:49Z alex $
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

// implementation of CInformErrorDialog, used for error reporting
// (taken from errors.cpp 1.27)


#include "camtypes.h"

#include "errorbox.h"
#include "errors.h"
//#include "andy.h"
//#include "tim.h"
//#include "resource.h"
#include "splash.h"
#include "mainfrm.h"
//#include "richard2.h"
#include "prdlgctl.h"

#include "helpuser.h"


CInformErrorDialog::CInformErrorDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CInformErrorDialog::IDD, pParent ? pParent : GetSafeParent() )
{
	//{{AFX_DATA_INIT(CInformErrorDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	// remember the HWND to use in case of emergency
	ParentHwnd = GetSafeParent()->GetSafeHwnd();
  
	// Make sure all the string variables are NULL.
	m_StaticTextStr = 0;
	m_TitleStr		= 0;					// by default NOT a custom title
	m_ErrorBoxType  = ERRORTYPE_ERROR;		// This is an error box by default
	m_OwnerModule	= 0L;
	m_OK			= 1;
	m_Cancel		= 2;
	m_Help			= 0;					// by default there is no help button

	// Set reasonable defaults for the button captions.
	m_ButtonStr[0]  = _R(IDS_OK);
	for (INT32 i = 1; i < ERRORDLG_MAXBUTTONS; i++) m_ButtonStr[i] = 0;
}



void CInformErrorDialog::DoDataExchange(CDataExchange* pDX)
{
	// IF YOU CHANGE THE VALUE OF ERRORDLG_MAXBUTTONS YOU MUST CHANGE THIS AS WELL!
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInformErrorDialog)
	DDX_Control(pDX, _R(ID_ERRORBOX_BUTTON5), m_Button[4]);
	DDX_Control(pDX, _R(ID_ERRORBOX_BUTTON4), m_Button[3]);
	DDX_Control(pDX, _R(ID_ERRORBOX_BUTTON3), m_Button[2]);
	DDX_Control(pDX, _R(ID_ERRORBOX_BUTTON2), m_Button[1]);
	DDX_Control(pDX, _R(ID_ERRORBOX_BUTTON1), m_Button[0]);
	//}}AFX_DATA_MAP
}


// IF YOU CHANGE THE VALUE OF ERRORDLG_MAXBUTTONS YOU MUST CHANGE THIS AS WELL!
BEGIN_MESSAGE_MAP(CInformErrorDialog, CDialog)
	//{{AFX_MSG_MAP(CInformErrorDialog)
	ON_WM_PAINT()
	ON_BN_CLICKED(_R(ID_ERRORBOX_BUTTON1), OnClickedButton1)
	ON_BN_CLICKED(_R(ID_ERRORBOX_BUTTON2), OnClickedButton2)
	ON_BN_CLICKED(_R(ID_ERRORBOX_BUTTON3), OnClickedButton3)
	ON_BN_CLICKED(_R(ID_ERRORBOX_BUTTON4), OnClickedButton4)
	ON_BN_CLICKED(_R(ID_ERRORBOX_BUTTON5), OnClickedButton5)
	ON_COMMAND(IDCANCEL, OnCancelCmd)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


// Static variables used to hold info about the error box template.
SIZE		CInformErrorDialog::DefButtonSize;
INT32			CInformErrorDialog::ButtonSpacing;
SIZE		CInformErrorDialog::DialogSize;
INT32			CInformErrorDialog::EdgeSpacing;
INT32			CInformErrorDialog::DefTopOfButton;
POINT		CInformErrorDialog::DefIconPos;
SIZE  		CInformErrorDialog::DefMsgSize;
BOOL  		CInformErrorDialog::ValidInfo = FALSE;
String_64 	CInformErrorDialog::ButtonText[ERRORDLG_MAXBUTTONS];


// The ID's of the error box buttons.
// IF YOU CHANGE THE VALUE OF ERRORDLG_MAXBUTTONS YOU MUST CHANGE THIS AS WELL!
UINT32 CInformErrorDialog::ButtonID[ERRORDLG_MAXBUTTONS] =
{
	_R(ID_ERRORBOX_BUTTON1),
	_R(ID_ERRORBOX_BUTTON2),
	_R(ID_ERRORBOX_BUTTON3),
	_R(ID_ERRORBOX_BUTTON4),
	_R(ID_ERRORBOX_BUTTON5)
};



/********************************************************************************************

>	void CInformErrorDialog::GetDialogInfo()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/05/94
	Purpose:	Finds out various properties of the error box dialog.  This is done once
				only - the first time an error box is displayed.
				These properties are used when moving/resizing buttons, the icon, and
				the static control used to display the message.
	SeeAlso:	CInformErrorDialog::SetupButtons; CInformErrorDialog::SetupMessage

********************************************************************************************/

void CInformErrorDialog::GetDialogInfo()
{
	// Skip this if we've already done it.
	if (ValidInfo)
		return;

	// Find out how bug the dialog is by default.
	CRect DlgRect;
	GetClientRect(&DlgRect);
	DialogSize.cx = DlgRect.Width();
	DialogSize.cy = DlgRect.Height();

	// Find out the button spacing/sizes etc.
	CWnd *pCtrl1 = GetDlgItem(ButtonID[0]);
	CWnd *pCtrl2 = GetDlgItem(ButtonID[1]);
	ENSURE((pCtrl1 != NULL) && (pCtrl2 != NULL), 
		   "Can't find control in CInformErrorDialog::OnInitDialog()");

	// Safety check.
	if ((pCtrl1 == NULL) || (pCtrl2 == NULL))
		return;

	// Get width of buttons, and the spacing between the buttons and the edge of the dialog.
	WINDOWPLACEMENT Placement;
	Placement.length = sizeof(WINDOWPLACEMENT);
	pCtrl1->GetWindowPlacement(&Placement);

	DefTopOfButton = Placement.rcNormalPosition.top;
	DefButtonSize.cx = Placement.rcNormalPosition.right - Placement.rcNormalPosition.left;
	DefButtonSize.cy = Placement.rcNormalPosition.bottom - Placement.rcNormalPosition.top;
	EdgeSpacing = Placement.rcNormalPosition.left;

	// Get space between adjacent buttons.
	Placement.length = sizeof(WINDOWPLACEMENT);
	pCtrl2->GetWindowPlacement(&Placement);

	ButtonSpacing = Placement.rcNormalPosition.left - (EdgeSpacing + DefButtonSize.cx);

	// Find the position of the icon.
	CWnd *pIconCtrl = GetDlgItem(_R(IDC_ERRORBOX_ICON));
	ENSURE(pIconCtrl != NULL, "Can't find Icon control in CInformErrorDialog::GetDialogInfo()");

	// Safety check.
	if (pIconCtrl == NULL)
		return;

	Placement.length = sizeof(WINDOWPLACEMENT);
	pIconCtrl->GetWindowPlacement(&Placement);
	
	DefIconPos.x = Placement.rcNormalPosition.left;
	DefIconPos.y = Placement.rcNormalPosition.top;

	// Find the position of the message text area.
	CWnd *pMsgCtrl = GetDlgItem(_R(IDC_ERRORBOX_TEXT));
	ENSURE(pMsgCtrl != NULL, "Can't find Text control in CInformErrorDialog::GetDialogInfo()");

	// Safety check.
	if (pMsgCtrl == NULL)
		return;

	Placement.length = sizeof(WINDOWPLACEMENT);
	pMsgCtrl->GetWindowPlacement(&Placement);

	DefMsgSize.cx = Placement.rcNormalPosition.right - Placement.rcNormalPosition.left;
	DefMsgSize.cy = Placement.rcNormalPosition.bottom - Placement.rcNormalPosition.top;

	// The static variables now contain valid information.
	ValidInfo = TRUE;
}



/********************************************************************************************

>	BOOL CInformErrorDialog::SetupButtons(HDC hDC, INT32 NumButtons)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/05/94
	Inputs:		hDC - device context for the dialog (used to access the font).
				NumButtons - how many buttons the caller has asked for.
	Returns: 	TRUE if the buttons were set up ok;
				FALSE if not, and hence caller should fall back on MessageBox().
	Purpose:	Initialises the buttons in the dialog ready to be displayed.
				This includes setting the text, resizing the buttons if the text is too
				big to fit, moveing the buttons so they are always centred no matter how
				big they are or how many there are.
				If the buttons get very wide, then the dialog is made wider to accomadate
				them.
	SeeAlso:	CInformErrorDialog::GetDialogInfo; CInformErrorDialog::SetupMessage

********************************************************************************************/

BOOL CInformErrorDialog::SetupButtons(HDC hDC, INT32 NumButtons)
{
	// Set the default button in the dialog.
	CWnd *pDefCtrl = GetDlgItem(ButtonID[m_OK - 1]);
	ENSURE(pDefCtrl != NULL, "Can't get handle to default control in CInformErrorDialog");

	// If we can't get at this button then ooer...bit of a fatal error
	if (pDefCtrl == NULL)
	{
		ENSURE(FALSE, "Can't get default button in error box!");
		return FALSE;
	}

	// Set the keyboard focus to the default button, and give it a 'default' border.
	pDefCtrl->SetFocus();
	SendMessage(DM_SETDEFID, ButtonID[m_OK - 1], 0);

	// Read in the button texts, and find which is the widest string.
	INT32 ButtonWidth = DefButtonSize.cx;
	INT32 i;
	for (i = 0; i < NumButtons; i++)
	{
		// Try to load text for this button
		if (!ButtonText[i].Load(m_ButtonStr[i], m_OwnerModule))
		{
			ENSURE(FALSE, "Unable to load button text for error box!");
			return FALSE;
		}

		// Try to read the size of this button text.
		SIZE TextSize;
		if (!GetTextExtentPoint(hDC, (TCHAR *) ButtonText[i], ButtonText[i].Length(),
						   	    &TextSize))
		{
			// Error reading text size
			ENSURE(FALSE, "Unable to read button text size for error box!");
			return FALSE;
		}

		if (TextSize.cx > ButtonWidth)
			ButtonWidth = TextSize.cx + 8;
	}

	// Allow for space on either side in the button
	ButtonWidth += 8;

	// Find out how big the buttons can be at the most.
	INT32 MaxWidth = DialogSize.cx - 
				   (2 * EdgeSpacing) - 
				   ((NumButtons - 1) * ButtonSpacing);

	// NumButtons cannot be 0 if we get to here...but just in case :-)
	if (NumButtons == 0)
	{
		ENSURE(FALSE, "NumButtons is zero in error box!");
		return FALSE;
	}

	// Safe to do a divide now!
	MaxWidth /= NumButtons;

	// The width of the dialog may change.
	INT32 NewWidth = DialogSize.cx;

	// Find out if we need to make the dialog bigger to accomodate the buttons.
	if (ButtonWidth > MaxWidth)
	{
		// Yes - find out if the buttons actually fit on screen - if not, make them
		// smaller and truncate the button text (this shouldn't happen too often!)

		// Get required dialog width
		NewWidth = (EdgeSpacing * 2) +
				   (NumButtons * ButtonWidth) +
				   ((NumButtons - 1) * ButtonSpacing);

		// Does this actually fit on screen?
		INT32 ScreenWidth = GetSystemMetrics(SM_CXSCREEN);

		if (ScreenWidth < NewWidth)
		{
			// They don't fit - downsize the buttons to fit.
			ButtonWidth = ScreenWidth - 
					      (2 * EdgeSpacing) - 
					      ((NumButtons - 1) * ButtonSpacing);
			ButtonWidth /= NumButtons;

			NewWidth = ScreenWidth;
		}

		// Ok - buttons are now correct size - resize the dialog.
		SIZE BorderSize;
		BorderSize.cx = 2 * ::GetSystemMetrics(SM_CXDLGFRAME);
		BorderSize.cy = ::GetSystemMetrics(SM_CYDLGFRAME) + 
						::GetSystemMetrics(SM_CYCAPTION);

		if (!SetWindowPos(NULL, 0, 0, 
					 	  NewWidth + BorderSize.cx, DialogSize.cy + BorderSize.cy,
					      SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOMOVE | SWP_NOREDRAW))
		{
			ENSURE(FALSE, "Unable to resize the error box!");
			return FALSE;
		}

		// Resize the error message control so it doesn't look silly.
		INT32 Diff = NewWidth - DialogSize.cx;
		
		CWnd *pMsgCtrl = GetDlgItem(_R(IDC_ERRORBOX_TEXT));
		ENSURE(pMsgCtrl != NULL, "Can't get handle to text control in CInformErrorDialog");

		if (pMsgCtrl != NULL)
		{
			// Ok - resize it.
			if (!pMsgCtrl->SetWindowPos(NULL, 0, 0, 
								   		DefMsgSize.cx + Diff, DefMsgSize.cy,
			  					   		SWP_NOACTIVATE | SWP_NOZORDER | 
			  					   		SWP_NOMOVE | SWP_NOREDRAW))
			{
				ENSURE(FALSE, "Unable to adjust error message control in error box!");
				return FALSE;
			}
		}
	}

	// Position buttons accordingly...

	// How much space is left on either side of the row of buttons?
	INT32 SpareSpace = NewWidth - 
					 (NumButtons * ButtonWidth) - 
					 ((NumButtons - 1) * ButtonSpacing);

	// Work out where the first button should be.
	INT32 FarLeft = SpareSpace / 2;

	INT32 ButtonStep = ButtonSpacing + ButtonWidth;

	// For each button:
	//  * Set the button text.
	//  * Move the button to the correct position.
	//  * Show the button.
	for (i = 0; i < NumButtons; i++)
	{
		SetDlgItemText(ButtonID[i], ButtonText[i]);

		if (!m_Button[i].SetWindowPos(NULL, 
								 	  FarLeft + (i * ButtonStep), DefTopOfButton, 
								 	  ButtonWidth, DefButtonSize.cy,
								 	  SWP_NOACTIVATE | SWP_NOZORDER | 
								 	  SWP_NOREDRAW | SWP_SHOWWINDOW))
		{
			ENSURE(FALSE, "Unable to move button in the error box!");
			return FALSE;
		}
	}

	// Tell the caller that we managed to do all this successfully!
	return TRUE;
}




/********************************************************************************************

>	BOOL CInformErrorDialog::SetupMessage(HDC hDC)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/05/94
	Inputs:		hDC - the device context of the dialog (used to access the dialog's font).
	Purpose:	Puts the error message into the dialog, and checks to see if this message
				will fit. If it doesn't, the dialog is expanded vertically so that it does
				fit, and the buttons are moved down, and the icon is moved down so it's
				halfway down the message text so it looks all neat and luvverly.
	SeeAlso:	CInformErrorDialog::GetDialogInfo; CInformErrorDialog::SetupButtons

********************************************************************************************/

BOOL CInformErrorDialog::SetupMessage(HDC hDC)
{
	// Set the error message in the text control, and find out the size of the message.
	SIZE TextSize;
	RECT TextRect;
	INT32 Offset = 0;

	CWnd *pMsgCtrl = GetDlgItem(_R(IDC_ERRORBOX_TEXT));
	ENSURE(pMsgCtrl != NULL, "Can't get handle to text control in CInformErrorDialog");

	
	if (pMsgCtrl == NULL)
	{
		// Failed to get at the message control - flag an error
		ENSURE(FALSE, "Unable to access error message control");
		return FALSE;
	}

	// Get rectangle of message control.
	RECT MsgCtrlRect;
	pMsgCtrl->GetWindowRect(&MsgCtrlRect);
	ScreenToClient(&MsgCtrlRect);

	// Use the current control's rectangle as the basis for the formatting rectangle
	// of the text (Bottom will extend in preference to Right)
	TextRect = MsgCtrlRect;

	if (m_StaticTextStr != 0)
	{
		String_256 MsgStr(m_StaticTextStr, m_OwnerModule);
		SetDlgItemText(_R(IDC_ERRORBOX_TEXT), MsgStr);
		DrawText(hDC, (TCHAR *) MsgStr, -1, &TextRect, DT_CALCRECT | DT_TOP | DT_LEFT | DT_WORDBREAK);
	}
	else
	{
		TCHAR *pMsg = Error::GetErrorString();
		SetDlgItemText(_R(IDC_ERRORBOX_TEXT), pMsg);
		DrawText(hDC, pMsg, -1, &TextRect, DT_CALCRECT | DT_TOP | DT_LEFT | DT_WORDBREAK);
	}

	TextSize.cx = TextRect.right - TextRect.left;
	TextSize.cy = TextRect.bottom - TextRect.top;

	if ((TextSize.cx == 0) || (TextSize.cy == 0))
	{
		// Failed to get the size of the text - flag an error
		ENSURE(FALSE, "Unable to read size of error message");
		return FALSE;
	}

	// Get dimensions of message control.
	SIZE MsgCtrlSize;
	MsgCtrlSize.cx = MsgCtrlRect.right - MsgCtrlRect.left;
	MsgCtrlSize.cy = MsgCtrlRect.bottom - MsgCtrlRect.top;

	if ((MsgCtrlSize.cx == 0) || (MsgCtrlSize.cy == 0))
	{
		// Message control has a bad size - flag an error
		ENSURE(FALSE, "Bad message control size in error dialog box!");
		return FALSE;
	}

	// If formatted text is bigger than current control rect
	// we need to resize it...
	if (MsgCtrlSize.cx < TextSize.cx || MsgCtrlSize.cy < TextSize.cy)
	{
		if (MsgCtrlSize.cx < TextSize.cx)
			MsgCtrlSize.cx = TextSize.cx + 8;

		if (MsgCtrlSize.cy < TextSize.cy)
		{
			Offset = TextSize.cy - MsgCtrlSize.cy + 8;
			MsgCtrlSize.cy = TextSize.cy + 8;
		}

		// Resize message.
		if (!pMsgCtrl->SetWindowPos(NULL, 0, 0, MsgCtrlSize.cx, MsgCtrlSize.cy,
		  					   		SWP_NOACTIVATE | SWP_NOZORDER | 
		  					   		SWP_NOMOVE | SWP_NOREDRAW))
		{
			ENSURE(FALSE, "Unable to resize the message control in error box!");
			return FALSE;
		}

		// Move icon (just change IconPos - we ignore the real icon from now on)
		IconPos.y += (Offset / 2);

		// Move buttons
		for (INT32 i = 0; i < ERRORDLG_MAXBUTTONS; i++)
		{
			RECT ButtonRect;
			m_Button[i].GetWindowRect(&ButtonRect);
			ScreenToClient(&ButtonRect);
			if (!m_Button[i].SetWindowPos(NULL, ButtonRect.left, ButtonRect.top + Offset,
										  0, 0,
		  					   		 	  SWP_NOACTIVATE | SWP_NOZORDER | 
		  					   		 	  SWP_NOSIZE | SWP_NOREDRAW))
			{
				ENSURE(FALSE, "Unable to resize the button control in error box!");
				return FALSE;
			}
		}

		// Resize dialog.
		RECT DialogRect;
		GetWindowRect(&DialogRect);
		INT32 DialogWidth  = DialogRect.right  - DialogRect.left;
		INT32 DialogHeight = DialogRect.bottom - DialogRect.top;

		if (!SetWindowPos(NULL, 0, 0, DialogWidth, DialogHeight + Offset,
		  			 	  SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOMOVE | SWP_NOREDRAW))
		{
			ENSURE(FALSE, "Unable to resize the error box dialog!");
			return FALSE;
		}
	}

	// All ok!
	return TRUE;
}


///////////////////////////////////////
// CInformErrorDialog message handlers


/********************************************************************************************

>	BOOL CInformErrorDialog::OnInitDialog()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/7/93
	Returns:	TRUE if all went OK, FALSE if the focus was set to a control
	Purpose:	This function sets up the dialog accoding to the data supplied by the
				program. First the various fields in the dialog are filled in and the 
				relevant buttons activated, and then the dialog is centred.

********************************************************************************************/

BOOL CInformErrorDialog::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	String_64 BoxTitle;
	BoxTitle = _R(IDS_ERROR_BOX_SERIOUS_ERROR);	// "Serious error"

	String_256 VerySeriousError;
	VerySeriousError = _R(IDS_ERROR_BOX_VERY_SERIOUS_ERROR); // "A very serious error has occured - please consult your technical support."

	// Andy Hills, 22-11-00
	// Store the help context.
	// We need to do this here, because the global help context variable
	// nNextMessageHelpContext may change before the user clicks the 'Help'
	// button. This fixes bug 6359.
	m_nHelpContext = Error::GetErrorNumber();
	if (! m_nHelpContext) m_nHelpContext = GetNextMsgHelpContext();

	// Find out how many buttons there are.
	for (INT32 NumButtons = ERRORDLG_MAXBUTTONS; NumButtons > 0; NumButtons--)
	{
		if (m_ButtonStr[NumButtons - 1] != 0) break;
	}

	// Adjust the OK and Cancel fields if necessary
	if (m_OK > (UINT32) NumButtons)
	{
		if (IsUserName("Tim"))
		{
			TRACE( _T("OK out of range, OK=%u, NumButtons=%d\n"), m_OK, NumButtons);
		}
		// Default to first button
		m_OK = 1;
	}

	if (m_Cancel > (UINT32) NumButtons)
	{
		if (IsUserName("Tim"))
		{
			TRACE( _T("Cancel out of range, Cancel=%u, NumButtons=%d\n"), m_Cancel, NumButtons);
		}

		// Default to be the same as OK (this means a box with a single OK box will
		// respond to Enter and Esc without the user having to specify a Cancel ID).
		m_Cancel = m_OK;
	}

	if (m_Help > (UINT32) NumButtons)
	{
		TRACEUSER( "JustinF", _T("Help button (%d) out of range (%d)\n"),
									(INT32) m_Help, (INT32) NumButtons);
		
		// The only really safe thing we can do is drop the help button.
		m_Help = 0;
	}
		
	// Make sure we have correct dialog information
	GetDialogInfo();
	if (!ValidInfo)
	{
		// Serious error - fall back to to MessageBox().
		goto SevereError;
	}

	// Get icon position
	IconPos = DefIconPos;


	// Get a DC for this dialog, so we can find out the size of text strings.
	// We'll also need to select in our font or else it'll base the width upon the
	// System font rather than the font we're using (MS Sans Serif at last check)
	CDC *pDC;
	CFont *OldFont;

	pDC = GetDC();
	ENSURE(pDC != NULL, "Can't get DC for error box dialog");

	// Check the DC
	if (pDC == NULL)
		goto SevereError;

	OldFont = pDC->SelectObject(GetFont());

	// Set buttons text and move/resize buttons according to the number of them,
	// and their contents.
	BOOL Success;
	Success = SetupButtons(pDC->m_hDC, NumButtons);

	// Size the error message control, and put the message in it.
	Success = Success && SetupMessage(pDC->m_hDC);

	if (OldFont != NULL)
		pDC->SelectObject(OldFont);

	// We've finished with this DC now.
	ReleaseDC(pDC);


	// Check for failure in button/message setup.
	if (!Success)
		goto SevereError;

	// Play the appropriate sound and set the appropriate title bar text ID.
	UINT32 TitleID;
	TitleID = m_TitleStr;
	switch (m_ErrorBoxType)
	{
		case ERRORTYPE_NORMAL:
			// No sound for this one - it's just a message; nothing to shout about.
			if (TitleID == 0) TitleID = _R(IDS_ERRORBOX_NORMAL);
			break;

		case ERRORTYPE_QUESTION:
			MessageBeep(MB_ICONQUESTION);
			if (TitleID == 0) TitleID = _R(IDS_ERRORBOX_NORMAL);
			break;

		case ERRORTYPE_ERROR:
			MessageBeep(MB_ICONEXCLAMATION);
			if (TitleID == 0) TitleID = _R(IDS_ERRORBOX_ERROR);
			break;

		case ERRORTYPE_WARNING:
			MessageBeep(MB_ICONASTERISK);
			if (TitleID == 0) TitleID = _R(IDS_ERRORBOX_WARNING);
			break;

		case ERRORTYPE_SERIOUS:
			MessageBeep(MB_ICONHAND);
			if (TitleID == 0) TitleID = _R(IDS_ERRORBOX_SERIOUS);
			break;

		case ERRORTYPE_ENSURE:
			MessageBeep(MB_ICONHAND);
			if (TitleID == 0) TitleID = _R(IDS_ERRORBOX_ENSURE);
			break;

		default:
			ENSURE(FALSE, "Bad errortype in CInformErrorDialog::OnInitDialog()");
			goto SevereError;
			break;
	}

	// Set the title bar text if necessary.
	if (TitleID != 0)
	{
		String_64 Title(TitleID);
		SetWindowText((TCHAR*) Title);
	}

	// Centre the dialog on the screen (Code stolen from splash.cpp)	
	// Get the size of the screen
	INT32 ScreenWidth, ScreenHeight;
	ScreenWidth = GetSystemMetrics(SM_CXSCREEN);
	ScreenHeight = GetSystemMetrics(SM_CYSCREEN);
	
	// Get the size of the dialog box
	RECT DialogRect;
	GetWindowRect(&DialogRect);
	INT32 DialogWidth, DialogHeight;
	DialogWidth  = DialogRect.right  - DialogRect.left;
	DialogHeight = DialogRect.bottom - DialogRect.top;
	
	// Chicago M7 gives us crap values at this point if we are minimized so try and make them sane
	if (DialogWidth<=0)
		DialogWidth = ScreenWidth / 2;
	else if (DialogWidth > ScreenWidth)
		DialogWidth = ScreenWidth;

	if (DialogHeight<=0)
		DialogHeight = ScreenHeight / 2;
	else if (DialogHeight > ScreenHeight)
		DialogHeight = ScreenHeight;

	// Centre the dialog box and give it the 'top' style.
	INT32 Left, Top;
	Left = (ScreenWidth - DialogWidth) / 2;
	Top  = (ScreenHeight - DialogHeight) / 2;
	SetWindowPos(&wndTop, Left, Top, DialogWidth, DialogHeight, SWP_SHOWWINDOW);
	
	// If we got this far, then we changed the keyboard focus, so return FALSE.
	return FALSE;

SevereError:
	// Ooer - deeply catastrophic error...report to user and exit.
	String_256 Message;

	// First - check that this isn't just because of an empty error message.
	if (m_StaticTextStr == 0)
	{
		// Get the error message
		TCHAR *pMsg = Error::GetErrorString();

		if ((pMsg == NULL) || (pMsg[0] == 0))
		{
			//
			// There is no error message!
			//

			// In debug builds, give developer a chance to go into debugger to see who is not
			// setting an error message.
			#ifdef _DEBUG
			if (::MessageBox(ParentHwnd, 
							 "Somebody reported an error without an error message being set.\r"
							 "Click OK to continue, or Cancel to go into debugger",
							 "DEBUG Warning from Camelot",
							 MB_OKCANCEL| MB_SYSTEMMODAL | MB_ICONHAND) == IDCANCEL)
			{
				// User wants to go into debugger
				DebugBreak();
			}
			#endif

			// Tell the user a spurious error has occured,
			if (!Message.Load(_R(IDS_ERRORBOX_SPURIOUS)))
				// Can't load error message - panic.
				goto VerySevereError;

			// Try to get the string that says "Warning from Camelot"
			String_64 Title;
			if (!Title.Load(_R(IDS_ERRORBOX_WARNING)))
				goto VerySevereError;

			if (::MessageBox(ParentHwnd, (TCHAR *) Message, (TCHAR *) Title,
							 MB_OK | MB_SYSTEMMODAL | MB_ICONHAND) == 0)
				// Could not create the message box - try our fallback one (probably won't
				// work but what the hell).
				goto VerySevereError;

			// Simulate user hitting default button.
			EndDialog((INT32) m_OK);

			return TRUE; // We haven't set the keyboard focus.
		}
	}


	// Inform the user that we've got a bit of a bad karma situation, and that the error
	// box might be not be completely accurate.
	if (!Message.Load(_R(IDS_ERRORBOX_SEVERE)))
		goto VerySevereError;

	if (::MessageBox(ParentHwnd, (TCHAR *) Message, (TCHAR *)BoxTitle, /*"Serious Error", */
					 MB_OK | MB_SYSTEMMODAL | MB_ICONHAND) == 0)
		// Could not create the message box - try our fallback one (probably won't
		// work but what the hell).
		goto VerySevereError;

	// Ok - try to report the error as best we can...

	// Work out what buttons to put on the dialog...

	// We use SYSTEMMODAL with ICONHAND because the SDK docs recommend this for low
	// memory/severe error situations.
	UINT32 MBFlags;
	MBFlags = MB_SYSTEMMODAL | MB_ICONHAND;

	// Do we need a cancel icon?
	if (m_Cancel != m_OK)
		MBFlags |= MB_OKCANCEL;
	else
		MBFlags |= MB_OK;

	if (m_StaticTextStr != 0)
	{
		if (Message.Load(m_StaticTextStr, m_OwnerModule))
		{
			// Error message loaded ok - display it
			INT32 Result =  ::MessageBox(ParentHwnd, (TCHAR *) Message, (TCHAR *)BoxTitle, MBFlags);

			if (Result == 0)
				// Could not create the message box - try our fallback one (probably won't
				// work but what the hell).
				goto VerySevereError;

			if (Result == IDCANCEL)
			{
				// Simulate user hitting cancel button.
				EndDialog((INT32) m_Cancel);
			}
			else
			{
				// Simulate user hitting default button.
				EndDialog((INT32) m_OK);
			}

			return TRUE; // We haven't set the keyboard focus.
		}
		else
		{
			// Can't load error message - bail out
			goto VerySevereError;
		}
	}
	else
	{
		// Get the error message
		TCHAR *pMsg = Error::GetErrorString();

		if (pMsg == NULL)
			// No error message!
			goto VerySevereError;

		// Found error message ok - display it
		INT32 Result =  ::MessageBox(ParentHwnd, pMsg, (TCHAR *)BoxTitle, MBFlags);

		if (Result == 0)
			// Could not create the message box - try our fallback one (probably won't
			// work but what the hell).
			goto VerySevereError;

		if (Result == IDCANCEL)
		{
			// Simulate user hitting cancel button.
			EndDialog((INT32) m_Cancel);
		}
		else
		{
			// Simulate user hitting default button.
			EndDialog((INT32) m_OK);
		}

		return TRUE; // We haven't set the keyboard focus.
	}

VerySevereError:
	// Very bad - we can't even report the error - just let the user that something deeply
	// sad has happened, and pretend that the OK button was pressed.
	::MessageBox(ParentHwnd, (TCHAR *)VerySeriousError,	(TCHAR *)BoxTitle, MB_OK | MB_SYSTEMMODAL | MB_ICONHAND);

	// Simulate user hitting default button.
	EndDialog((INT32) m_OK);

	return TRUE; // We haven't set the keyboard focus.
}



/********************************************************************************************

>	void CInformErrorDialog::OnPaint()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/05/94
	Purpose:	Special painting of the dialog.  This involves drawing the correct icon
				onto the dialog, according to the type/degree of error.
				The icon position is based on the position of the hidden icon control in
				the dialog, but may be moved down if the message area needs to grow to
				accomodate a long message.
	SeeAlso:	CInformErrorDialog::SetupMessage

********************************************************************************************/

void CInformErrorDialog::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	
	HICON hIcon;
	CWinApp *pApp = AfxGetApp();

	ENSURE(pApp != NULL, "Could not get application object in CInformErrorDialog::OnPaint()");
	if (pApp == NULL)
		return;

	switch (m_ErrorBoxType)
	{
		case ERRORTYPE_NORMAL:
			hIcon = pApp->LoadIcon(_R(IDR_MAINFRAME));
			break;

		case ERRORTYPE_QUESTION:
// The line below is commented out (by Phil, 12/8/96) because the latest UI guidelines
// advise against using this icon.
// See "The Windows Interface Guidelines for Software Design" P.211.
//			hIcon = pApp->LoadStandardIcon(_R(IDI_QUESTION));
			hIcon = pApp->LoadStandardIcon(_R(IDI_EXCLAMATION));
			break;

		case ERRORTYPE_ERROR:
			hIcon = pApp->LoadStandardIcon(_R(IDI_EXCLAMATION));
			break;

		case ERRORTYPE_WARNING:
			hIcon = pApp->LoadStandardIcon(_R(IDI_ASTERISK));
			break;

		case ERRORTYPE_SERIOUS:
		case ERRORTYPE_ENSURE:
			hIcon = pApp->LoadStandardIcon(_R(IDI_HAND));
			break;

		default:
			ENSURE(FALSE, "Bad errortype in CInformErrorDialog::OnPaint()");
			return;
	}


	ENSURE(hIcon != NULL, "Could not load icon in CInformErrorDialog::OnPaint()");
	if (hIcon != NULL)
	{
		// Got an icon - let's draw it on the dialog.
		dc.DrawIcon(IconPos.x, IconPos.y, hIcon);
	}

	// Do not call CWnd::OnPaint() for painting messages
}



/********************************************************************************************
>	void CInformErrorDialog::OnClickedButton1()
	void CInformErrorDialog::OnClickedButton2()
	void CInformErrorDialog::OnClickedButton3()
	void CInformErrorDialog::OnClickedButton4()
	void CInformErrorDialog::OnClickedButton5()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/7/93
	Purpose:	Responds to the five buttons at the bottom of the dialog.  Calls the
				HandleButton function in each case, passing the ID of the button.
	SeeAlso:	CInformErrorDialog::HandleButton
********************************************************************************************/

void CInformErrorDialog::OnClickedButton1()
{
	HandleButton(1);
}


void CInformErrorDialog::OnClickedButton2()
{
	HandleButton(2);
}


void CInformErrorDialog::OnClickedButton3()
{
	HandleButton(3);
}


void CInformErrorDialog::OnClickedButton4()
{
	HandleButton(4);
}


void CInformErrorDialog::OnClickedButton5()
{
	HandleButton(5);
}



/********************************************************************************************
>	virtual void CInformErrorDialog::HandleButton(UINT32 idButton)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/5/95
	Inputs:		idButton			which button was clicked, ie. 1-5
	Purpose:	Handles a button being clicked in a Inform... dialog box.  If the button
				is the "help" button it runs help for the dialog, otherwise it ends the
				dialog and returns which button was clicked.
	SeeAlso:	CInformErrorDialog::OnClickedButton1
********************************************************************************************/

void CInformErrorDialog::HandleButton(UINT32 idButton)
{
	// Check if the clicked button is a "Help" button.
	if (idButton == m_Help)
	{
#if !defined(EXCLUDE_FROM_RALPH) && !defined(EXCLUDE_FROM_XARALX)
		// It is, so run the help topic associated with the message ID.
		HelpUser(m_nHelpContext);
#endif
	}
	else
	{
		// It isn't, so end the dialog, returning which button was clicked.
		EndDialog((INT32) idButton);
	}
}



/********************************************************************************************
>	void CInformErrorDialog::OnCancelCmd()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/05/94
	Purpose:	Responds to an Esc keypress from the user - it simulates pressing the
				button defined to be the 'cancel' button in the dialog (although the
				button may not actually be labelled 'cancel' - it may say 'No' or
				whatever).
********************************************************************************************/

void CInformErrorDialog::OnCancelCmd()
{
	// User has hit Esc - return with the ID of the 'Cancel' button.
	EndDialog((INT32) m_Cancel);
}



/********************************************************************************************

>	CWnd* CInformErrorDialog::GetSafeParent()

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/11/94
	Returns:	Pointer to a CWnd for use as a parent window in error dialogs, or NULL
				if we don't have one.
	Purpose:	Used by the error reporter and ensure handler to get a suitable parent
				window.

********************************************************************************************/

CWnd* CInformErrorDialog::GetSafeParent()
{
	CWnd *Parent = NULL;

#if !defined(EXCLUDE_FROM_RALPH) && !defined(EXCLUDE_FROM_XARALX)
	// if the splash box is up, use that as the parent
	Parent = CSplashDialog::GetpWnd();

	if (Parent)
		return Parent;
#endif

//	WEBSTER-ranbirr-12/11/96
#ifndef WEBSTER
#ifndef STANDALONE
	// If there is a print-related dlg up, use it as the parent
	Parent = CCPrintDialog::GetPrintCWnd();
	if (Parent)
		return Parent;
#endif
#endif //webster

	// see if mainframe is up - use it if visible
	Parent = GetMainFrame();
	if (Parent && Parent->IsWindowVisible())
		return Parent;
	return NULL;
}
