// $Id: errorbox.h 662 2006-03-14 21:31:49Z alex $
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

// class used to report errors
// (taken from errors.h 1.26)

#ifndef INC_ERRORBOX
#define	INC_ERRORBOX

// #include "errordlg.h"

class ErrorInfo;

/////////////////////////////////////////////////////////////////////////////
// CInformErrorDialog dialog

class CInformErrorDialog : public wxDialog
{
// Construction
public:
	CInformErrorDialog(wxWindow* pParent = NULL);	// standard constructor

	static wxWindow* GetSafeParent();

// Dialog Data
	//{{AFX_DATA(CInformErrorDialog)
	enum { IDD = _R(IDD_ERRORBOX) };
	wxButton		   *m_Button[ERRORDLG_MAXBUTTONS];
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	virtual BOOL OnInitDialog();
	void OnPaint();
	void OnClickedButton1();
	void OnClickedButton2();
	void OnClickedButton3();
	void OnClickedButton4();
	void OnClickedButton5();
	void OnCancelCmd();
    
	// Man Made Section
	// The strings that are to be displayed in the buttons and the static text box
	UINT32		m_StaticTextStr;
	UINT32		m_ButtonStr[ERRORDLG_MAXBUTTONS];
	UINT32		m_ErrorBoxType;		// Holds a ERRORTYPE_ description of whether the error is warning etc.
	UINT32		m_TitleStr;			// optionally holds ID of string to use for dialogue title
	UINT32		m_OwnerModule;
	UINT32		m_OK;				// which button is the "accept" button
	UINT32		m_Cancel;			// which button, if any, is the "reject" button
	UINT32		m_Help;				// which button, if any, is the "give me help" button

	POINT		IconPos;
	HWND		ParentHwnd;					// parent window, or NULL

	void GetDialogInfo();
	BOOL SetupButtons(HDC hDC, INT32 NumButtons);
	BOOL SetupMessage(HDC hDC);

	// Handles a button being clicked, ending the dialog, running help etc.
	virtual void HandleButton(UINT32 idButton);

	// The ID's of the error box buttons.
	static UINT32 ButtonID[ERRORDLG_MAXBUTTONS];

	// The default button size for the error box.
	static SIZE DefButtonSize;

	// How far apart the buttons should be.
	static INT32 ButtonSpacing;

	// Default size of the dialog
	static SIZE DialogSize;

	// Space between buttons and the edge of the dialog
	static INT32 EdgeSpacing;

	// Position of the buttons' top edges.
	static INT32 DefTopOfButton;

	// Position of the icon
	static POINT DefIconPos;

	// Default size of the area used to contain the error message.
	static SIZE DefMsgSize;

	// TRUE if we've initialised the above variables already; FALSE if not.
	static BOOL ValidInfo;

	// The text for each of the buttons.
	static String_64 ButtonText[ERRORDLG_MAXBUTTONS];

	// Andy Hills, 22-11-00
	// We need to store the help context as the dialogue is created.
	// This is because various On<Event> handlers can cause the help context
	// to be changed in the background, causing bug 6359 (Help button doesn't
	// work second time)
	UINT32 m_nHelpContext;

	friend INT32 InformGeneral(UINT32 Error, UINT32 modID, UINT32 ErrorMsg, 
				  			 UINT32 Butt1, UINT32 Butt2, UINT32 Butt3, UINT32 Butt4,
				  			 UINT32 OK, UINT32 Cancel);
	friend INT32 InformGeneral(UINT32 Error, ErrorInfo *pInfo, UINT32 ModID);
	friend void InformLastError(UINT32);
};

#endif
