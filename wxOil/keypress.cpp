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
// Implementation of the KeyPress class that encapsulates key presses in camelot.

/*
*/

#include "camtypes.h"
#include "keypress.h"
#include "vkextra.h"
#include "camelot.h"
#include "app.h"
#include "hotkeys.h"
#include "keymsg.h"
#include "docview.h"
#include "document.h"
//#include "ctrlhelp.h"
//#include "basebar.h"
//#include "dragmgr.h"
#include "unicdman.h"

DECLARE_SOURCE("$Revision$");

CC_IMPLEMENT_MEMDUMP(KeyPressSysMsg,CC_CLASS_MEMDUMP)
CC_IMPLEMENT_MEMDUMP(KeyPress,		CC_CLASS_MEMDUMP)

CC_IMPLEMENT_DYNAMIC(KeyMsg,Msg)

// This will get Camelot to display the filename and linenumber of any memory allocations
// that are not released at program exit
#define new CAM_DEBUG_NEW

struct ExtraUnicode
{
	UINT32	VirtKey;
	WCHAR	Unicode;
} ExtraUnicodes[] = 
{
	{CAMKEY(F1),  		CAMELOT_UNICODE_BASE + 0},
	{CAMKEY(F2),  		CAMELOT_UNICODE_BASE + 1},
	{CAMKEY(F3),  		CAMELOT_UNICODE_BASE + 2},
	{CAMKEY(F4),  		CAMELOT_UNICODE_BASE + 3},
	{CAMKEY(F5),  		CAMELOT_UNICODE_BASE + 4},
	{CAMKEY(F6),  		CAMELOT_UNICODE_BASE + 5},
	{CAMKEY(F7),  		CAMELOT_UNICODE_BASE + 6},
	{CAMKEY(F8),  		CAMELOT_UNICODE_BASE + 7},
	{CAMKEY(F9),  		CAMELOT_UNICODE_BASE + 8},
	{CAMKEY(F10), 		CAMELOT_UNICODE_BASE + 9},
	{CAMKEY(F11), 		CAMELOT_UNICODE_BASE + 10},
	{CAMKEY(F12), 		CAMELOT_UNICODE_BASE + 11},
	{CAMKEY(F13), 		CAMELOT_UNICODE_BASE + 12},
	{CAMKEY(F14), 		CAMELOT_UNICODE_BASE + 13},
	{CAMKEY(F15), 		CAMELOT_UNICODE_BASE + 14},
	{CAMKEY(F16), 		CAMELOT_UNICODE_BASE + 15},
	{CAMKEY(F17), 		CAMELOT_UNICODE_BASE + 16},
	{CAMKEY(F18), 		CAMELOT_UNICODE_BASE + 17},
	{CAMKEY(F19), 		CAMELOT_UNICODE_BASE + 18},
	{CAMKEY(F20), 		CAMELOT_UNICODE_BASE + 19},
	{CAMKEY(F21), 		CAMELOT_UNICODE_BASE + 20},
	{CAMKEY(F22), 		CAMELOT_UNICODE_BASE + 21},
	{CAMKEY(F23), 		CAMELOT_UNICODE_BASE + 22},
	{CAMKEY(F24), 		CAMELOT_UNICODE_BASE + 23},

	{CAMKEY(SHIFT),   	CAMELOT_UNICODE_BASE + 24},
	{CAMKEY(CONTROL), 	CAMELOT_UNICODE_BASE + 25},
	{CAMKEY(MENU),		CAMELOT_UNICODE_BASE + 26},

	{CAMKEY(DELETE), 	CAMELOT_UNICODE_BASE + 28},
	{CAMKEY(INSERT), 	CAMELOT_UNICODE_BASE + 27},
	{CAMKEY(HOME),   	CAMELOT_UNICODE_BASE + 29},
	{CAMKEY(END),    	CAMELOT_UNICODE_BASE + 30},
	{CAMKEY(PRIOR),  	CAMELOT_UNICODE_BASE + 31},		// Page up
	{CAMKEY(NEXT),   	CAMELOT_UNICODE_BASE + 32},		// Page down

	{CAMKEY(LEFT),   	CAMELOT_UNICODE_BASE + 33},		// Cursor left
	{CAMKEY(UP),   		CAMELOT_UNICODE_BASE + 34},		// Cursor up
	{CAMKEY(RIGHT),   	CAMELOT_UNICODE_BASE + 35},		// Cursor right
	{CAMKEY(DOWN),   	CAMELOT_UNICODE_BASE + 36},		// Cursor down

	{CAMKEY(NUMLOCK),   	CAMELOT_UNICODE_BASE + 37},
	{CAMKEY(SCROLL),   	CAMELOT_UNICODE_BASE + 38},
	{CAMKEY(CAPITAL),   	CAMELOT_UNICODE_BASE + 39},

	{CAMKEY(ESCAPE),   	CAMELOT_UNICODE_BASE + 40},
	{CAMKEY(CANCEL),   	CAMELOT_UNICODE_BASE + 41},		// Ctrl-break

	{CAMKEY(PAUSE), 		CAMELOT_UNICODE_BASE + 42},
	{CAMKEY(SELECT), 	CAMELOT_UNICODE_BASE + 43},
	{CAMKEY(EXECUTE), 	CAMELOT_UNICODE_BASE + 44},
	{CAMKEY(SNAPSHOT), 	CAMELOT_UNICODE_BASE + 45},		// Print screen
	{CAMKEY(HELP), 		CAMELOT_UNICODE_BASE + 46},

	{CAMKEY(MINUS),		CAMELOT_UNICODE_BASE + 47},		// These two are placed in hear so that when you do ctrl+<key>
	{CAMKEY(EQUALS),		CAMELOT_UNICODE_BASE + 48},		// a unicode value will be generated, allowing the keypress to be made

	{CAMKEY(1),  		CAMELOT_UNICODE_BASE + 49},
	{CAMKEY(2),  		CAMELOT_UNICODE_BASE + 50},
	{CAMKEY(3),  		CAMELOT_UNICODE_BASE + 51},
	{CAMKEY(4),  		CAMELOT_UNICODE_BASE + 52},
	{CAMKEY(5),  		CAMELOT_UNICODE_BASE + 53},

	{CAMKEY(ADD),  		CAMELOT_UNICODE_BASE + 54},
	{CAMKEY(SUBTRACT),	CAMELOT_UNICODE_BASE + 55},

	{CAMKEY(0),			CAMELOT_UNICODE_BASE + 56},
	{CAMKEY(9),			CAMELOT_UNICODE_BASE + 57},
	{CAMKEY(8),			CAMELOT_UNICODE_BASE + 58},
	{CAMKEY(7),			CAMELOT_UNICODE_BASE + 59},

	{CAMKEY(NUMPAD_ADD), _T('+')},						// These are need so we can get unicode version, and 
	{CAMKEY(NUMPAD_SUBTRACT), _T('-')},					// hence get the hotkey handled

	{CAMKEY(CC_NONE), 0}	// Marks the end of the list
};

// The static member vars of KeyPress that will hold the current state of the modifier keys
BOOL KeyPress::AdjustStateLeft			= FALSE;
BOOL KeyPress::AdjustStateRight			= FALSE;
BOOL KeyPress::ConstrainStateLeft		= FALSE;
BOOL KeyPress::ConstrainStateRight		= FALSE;
BOOL KeyPress::AlternativeStateLeft 	= FALSE;
BOOL KeyPress::AlternativeStateRight	= FALSE;
BOOL KeyPress::OptionStateLeft			= FALSE;
BOOL KeyPress::OptionStateRight			= FALSE;

// TRUE if one or more modifier keys changed on the last key event
BOOL KeyPress::fModifierChanged = FALSE;

// Ascii generating vars used with Alt and numerical key presses
INT32 KeyPress::AsciiVal = -1;
BOOL KeyPress::ValidAsciiVal = FALSE;

BYTE KeyPress::LastLeadByte = 0;

// This was done by Tim
// this macro returns TRUE if the machine does not support every key, which means
// WinNT or not, basically
#if WIN32
#define	INCOMPLETE_KEYS	IsWin32s()
#else
#define	INCOMPLETE_KEYS	TRUE
#endif
		   	
extern void Beep();

List					KeyPress::AdditionalVirtKeys;
UINT32					KeyPressSysMsg::m_LastVirtKey = 0;

/********************************************************************************************

> KeyPressSysMsg::KeyPressSysMsg(MSG* pMsg)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/8/94
	Inputs:		pMsg = Ptr to a Windows message block
	Outputs:	-
	Returns:	-
	Purpose:	The constructor.
				Creates a key press message object from a raw Windows message block.
				A valid KeyPressSysMsg is constructed if the message was one of these:
					WM_KEYDOWN
					WM_KEYUP
					WM_SYSKEYDOWN
					WM_SYSKEYUP
					WM_CHAR

	Errors:		If the Windows message is not key-related, then the member function IsValid()
				will return FALSE
	SeeAlso:	KeyPressSysMsg::IsValid

********************************************************************************************/

KeyPressSysMsg::KeyPressSysMsg(wxKeyEvent* pMsg)
{
	// Assume the message is a key-related one that we want
	Valid = TRUE;

	// There doesn't seem to be a need to distinguish between WM_KEY and WM_SYSKEY message
	// so map them to the same KM message
	if( wxEVT_KEY_DOWN == pMsg->GetEventType() ) 
	{
		Message = KM_KEYDOWN;
	}
	else
	if( wxEVT_CHAR == pMsg->GetEventType() ) 
	{
		Message = KM_CHAR;
	}
	else
	if( wxEVT_KEY_UP == pMsg->GetEventType() ) 
	{
		Message = KM_KEYUP;
	}
#pragma message( __LOCMSG__ _T("Removed Windows IME usage") )
/*	case WM_IME_CHAR:
		TRACE( _T("IME\n"));
		break; */
	else
	{
		// Message was not one we're interested in, so the KeyPressSysMsg object is invalid
		Valid = FALSE;
	}

	if (Valid)
	{
		// Now we have a valid Windows message to play with.
		// Extract the data and store in a more platform-indy way

		// Get the virtual key code for the key pressed
		VirtKey = pMsg->GetKeyCode();

		// Get the key data flags word
//		INT32 KeyData = pMsg->lParam;
		
		// We NEVER want to intercept Alt-Tab events, so filter them out here
		// (N.B. This was only ever a problem on Win16 - NT doesn't send us
		// Alt-Tab keydown events, only key-ups, which are ignored.)
		if ((VirtKey == CAMKEY(TAB)) && KeyPress::IsAlternativePressed())
		{
			Valid=FALSE;
			return;
		}
		
#pragma message( __LOCMSG__ _T("Removed NameGallery usage") )
#if 0
		RepeatCount = KeyData & 0xffff;
		ScanCode	= (KeyData >> 16) & 0xff;
		Extended 	= (KeyData & (1<<24)) != 0;
		PrevDown 	= (KeyData & (1<<30)) != 0;
#else
		RepeatCount = 1;
		ScanCode	= VirtKey;
		Extended 	= false;
		PrevDown 	= m_LastVirtKey == VirtKey;
#if FALSE != wxUSE_UNICODE
		m_Char		= pMsg->GetRawKeyCode();
#endif
#endif
		// Update the last virtual keycode if not a modifier
		switch( m_LastVirtKey )
		{
		case WXK_SHIFT:
		case WXK_ALT:
		case WXK_CONTROL:
		case WXK_MENU:
			// Skip
			break;

		default:
			if( KM_KEYUP == Message )
				m_LastVirtKey = 0;
			else
				m_LastVirtKey = VirtKey;
			break;
		}
	}
}

/********************************************************************************************

> BOOL KeyPressSysMsg::IsValid()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/8/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the KeyPressSysMsg object is a valid one. FALSE otherwise
	Purpose:	Allows you to see if the KeyPressSysMsg was legally constructed.
	Errors:		-
	SeeAlso:	KeyPressSysMsg::KeyPressSysMsg

********************************************************************************************/

BOOL KeyPressSysMsg::IsValid()
{
	return Valid;
}

//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------

/********************************************************************************************

> static BOOL KeyPress::Init()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/8/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if initialised correctly
	Purpose:	Initialises the KeyPress class
	Errors:		-
	SeeAlso:	KeyPress::IsValid, KeyPress::GenerateKeyPress

********************************************************************************************/

// remove after testing
#include "hotkeys.h"

BOOL KeyPress::Init()
{
	return TRUE;
}



/********************************************************************************************
>	static BOOL KeyPress::DispatchKeyEvent(UINT32 nMsgID, UINT32 nChar, UINT32 nRepCnt, UINT32 nFlags)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/10/94
	Inputs:		nMsgID		the Windows ID of the key message, eg. WM_KEYDOWN
				nChar		the virtual key code / char value (depending on you believe)
				nRepCnt		repeat count
				nFlags		keyboard state flags, eg. key down or up
	Outputs:	-
	Returns:	TRUE if the app processed the event.
	Purpose:	Packages an MFC key-stroke message into a form suitable for the kernel
				and calls the keyboard system with the event.
	Errors:		-
	SeeAlso:	KeyPress::TranslateMessage
********************************************************************************************/

BOOL KeyPress::DispatchKeyEvent(UINT32 nMsgID, UINT32 nChar, UINT32 nRepCnt, UINT32 nFlags)
{
#pragma message( __LOCMSG__ _T("BaseTextClass::PreOpProcessing - do nothing") )
	TRACE( _T("Warning - BaseTextClass::PreOpProcessing called") );
/*	// For compatibility with existing code we must repackage the unpacked message.
	// NB. the RHS of the lParam expression will work for Win16 as well.
	MSG msg;
	msg.message = nMsgID;
	msg.wParam  = nChar;
	msg.lParam  = (nRepCnt & 0xFFFF) | ((((LPARAM) nFlags) & 0xFFFF) << 16);

	// Make sure the kernel knows which view/doc the event applies to, if any.
	if (Document::GetSelected() != NULL) Document::GetSelected()->SetCurrent();
	if (DocView::GetSelected() != NULL)  DocView::GetSelected()->SetCurrent();
	
	// Call the key-press system.
	return TranslateMessage(&msg); */

	return true;
}



/********************************************************************************************

> KeyPress::KeyPress()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/8/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Default constructor.
				Creates an invalid keypress object.  The KeyPress class will have one or more public
				constructors or static member functions for creating valid KeyPress objects.
	Errors:		-
	SeeAlso:	KeyPress::IsValid, KeyPress::GenerateKeyPress

********************************************************************************************/

KeyPress::KeyPress()
{
	Initialise();
}

/********************************************************************************************

> KeyPress::KeyPress(KeyPressSysMsg* pKeySysMsg,WCHAR UnicodeChar)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/8/94
	Inputs:		pKeySysMsg  = Ptr to a platform indepenent version of a Windows key press message
				UnicodeChar	= The Unicode char this key press should have
	Outputs:	-
	Returns:	-
	Purpose:	Constructs a KeyPress object from the given parameters
	Errors:		-
	SeeAlso:	KeyPress::IsValid, KeyPress::GenerateKeyPress

********************************************************************************************/

KeyPress::KeyPress(KeyPressSysMsg* pKeySysMsg,WCHAR UnicodeChar)
{
	VirtKey		= pKeySysMsg->VirtKey;
	Unicode 	= UnicodeChar;

	Adjust 		= KeyPress::AdjustStateLeft 	 | KeyPress::AdjustStateRight;
	Constrain 	= KeyPress::ConstrainStateLeft 	 | KeyPress::ConstrainStateRight;
	Alternative = KeyPress::AlternativeStateLeft | KeyPress::AlternativeStateRight;
	Option 		= KeyPress::OptionStateLeft  	 | KeyPress::OptionStateRight;

	Extended	= pKeySysMsg->Extended;

	// Interpret Alt+Ctrl as Extended Alt, because AltGr (UK right hand Alt) produces an additional 
	// Ctrl key press that is indistinguishable from a left hand Ctrl.
	// This means that left hand Alt+Ctrl will appear the same as right hand AltGr to Camelot.
	// E.g. Alt+Ctrl+Spacebar == AltGr+Spacebar
	if (Alternative && Constrain)
	{
		Extended  = TRUE;
		Constrain = FALSE;
	}

	Repeat		= pKeySysMsg->PrevDown;
	RightHand	= pKeySysMsg->Extended;
	Release		= (pKeySysMsg->Message == KM_KEYUP);

	Valid 		= TRUE;		// This is now a valid KeyPress object.
}

/********************************************************************************************

> KeyPress::KeyPress(	UINT32 ThisVirtKey,
						BOOL PlusAdjust 	 = FALSE, 
						BOOL PlusConstrain 	 = FALSE, 
						BOOL PlusAlternative = FALSE,
						BOOL ExtendedKey	 = FALSE)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/8/94
	Inputs:		ThisVirtKey 	 = The virtual key code of the key you want to use
				PlusAdjust  	 = TRUE if the key should be modified by the Adjust key
				PlusConstrain  	 = TRUE if the key should be modified by the Constrain key
				PlusAlternative  = TRUE if the key should be modified by the Alternative key
				ExtendedKey		 = TRUE if you are specifying an extended key (e.g. "Enter" key instead of "Return")
				WorksInDragsKey	 = TRUE if you want this keypress to work in the middle of drags.
	Outputs:	-
	Returns:	-
	Purpose:	Constructs a KeyPress object from the given parameters
				This constructor is for use by systems that deal with "hot keys", i.e. ones that are only
				interested in the physical key that is being pressed
	Errors:		-
	SeeAlso:	KeyPress::IsValid, KeyPress::GenerateKeyPress

********************************************************************************************/

KeyPress::KeyPress(UINT32 ThisVirtKey, BOOL PlusAdjust, BOOL PlusConstrain, BOOL PlusAlternative, BOOL ExtendedKey,
					BOOL WorksInDragsKey)
{
	Initialise();

	VirtKey		= ThisVirtKey;
	Adjust 		= PlusAdjust;
	Constrain 	= PlusConstrain;
	Alternative = PlusAlternative;
	Extended	= ExtendedKey;
	WorksInDrag = WorksInDragsKey;
	Valid 		= TRUE;
}



/********************************************************************************************

>	static KeyPress* ConstructVK(TCHAR ch);

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/10/95
	Inputs:		ch - ASCII/Unicode value of required keypress
	Outputs:	-
	Returns:	-
	Purpose:	Constructs a keypress that represents the keypress required to generate the
				specified character.  The Virtual Code will be CAMKEY(CC_NONE) if the key cannot
				be generated
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
KeyPress* KeyPress::ConstructVK(TCHAR ch)
{
//	PORTNOTETRACE( "other", "KeyPress::ConstructVK - do nothing" );
	KeyPress* pNew = new KeyPress();

#if !defined(EXCLUDE_FROM_XARALX)
	if (pNew != NULL)
	{
		INT8 Result = ::VkKeyScan(ch);
		BYTE LowByte = Result & 0xFF;
		BYTE HighByte = (Result >> 8) & 0xFF;

		if ((HighByte == 0xFF) && (LowByte == 0xFF))
		{
			pNew->VirtKey = CAMKEY(CC_NONE);
		}
		else
		{
			pNew->VirtKey = LowByte;
			pNew->Adjust = HighByte & 1;
			pNew->Constrain = HighByte & 2;
			pNew->Alternative	= HighByte & 4;
		}
	}
#endif
	
	return pNew;
}



/********************************************************************************************

>	KeyPress::Initialise()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/10/96
	Inputs:		-
	Outputs:	See below
	Returns:	-
	Purpose:	Initialises all member variables of the KeyPress object to sensible values
	Errors:		-
	SeeAlso:	KeyPress constructors

********************************************************************************************/
void KeyPress::Initialise()
{
	// Some highly unlikely Unicode char
	Unicode 	= 0xffff;

	// A highly unlikely virtual key
	VirtKey 	= CAMKEY(CC_NONE);

	// Set all the flags to FALSE
	Adjust 		= FALSE;
	Constrain	= FALSE;
	Alternative	= FALSE;
	Option		= FALSE;

	Extended	= FALSE;

	Repeat		= FALSE;
	RightHand	= FALSE;
	Release		= FALSE;

	// This is an invalid KeyPress object at the moment
	Valid 		= FALSE;
}



/********************************************************************************************

> BOOL KeyPress::IsValid()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/8/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the KeyPress object is a valid one. FALSE otherwise
	Purpose:	Allows you to see if the KeyPress is legal.
				The default KeyPress constuctor will always create invalid instances. It is up
				to other constructors, or member functions, to generate valid ones
	Errors:		-
	SeeAlso:	KeyPress::KeyPress, KeyPress::GenerateKeyPress

********************************************************************************************/

BOOL KeyPress::IsValid()
{
	return Valid;
}

/********************************************************************************************

> static BOOL KeyPress::IsModifier(UINT32 VirtKey)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/8/94
	Inputs:		VirtKey = A Windows virtual key code
	Outputs:	-
	Returns:	TRUE  if the key is one of the modifier keys
				FALSE otherwise
	Purpose:	Central modifier key identifying routine.
				The modifiers are as follows :
					Both Alt   keys
					Both Shift keys
					Both Ctrl  keys
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL KeyPress::IsModifier(UINT32 VirtKey)
{
	return (VirtKey == CAMKEY(CC_MOD_ADJUST) || VirtKey == WXK_CC_MOD_CONSTRAIN || VirtKey == WXK_CC_MOD_ALTERNATIVE);
}

/********************************************************************************************

 > static INT32 KeyPress::GenerateUnicode(UINT32 VirtKey,UINT32 ScanCode,BYTE* pKeyState,WCHAR* pWideCharBuf,INT32 WideCharBufSize)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/8/94
	Inputs:		VirtKey 		= a Windows virtual key code
				ScanCode 		= the scan code generated by the keyboard
				pKeyState 		= 256 byte array filled with the current keyboard state via GetKeyboardState
				pWideCharBuf 	= buffer to stick the Unicode char(s) into
				WideCharBufSize = the size of the wide char buffer (min value of 2)
	Outputs:	pWideCharBuf will contain 1 WCHAR at index [0]
	Returns:	The number of Unicode chars generated (see the Windows ToUnicode() function)
	Purpose:	Converts a virtual key code into a Unicode char.
				This is a layer on top of the Windows ToUnicode() function that will generate Unicodes in the
				corporate user zone for keys that do not have a standard Unicode evivalent (e.g. function keys)

				If the internal call to ToUnicode() fails, but the VirtKey is an alphabetic key, a Unicode code is 
				generated that is equivalent to pressing Ctrl+<alphabetic key>, e.g. Ctrl+A = 1, Ctrl+B = 2.
				This happens when you press Ctrl+Alt+<alphabetic key>, which is only really used by the hot key system,
				so therefore the actual unicode code is not significant (correct at time of writing - Markn 11/1/95)
	Errors:		-
	SeeAlso:	KeyPress::IsValid

********************************************************************************************/

INT32 KeyPress::GenerateUnicode(UINT32 VirtKey,UINT32 ScanCode,BYTE* pKeyState,WCHAR* pWideCharBuf,UINT32 WideCharBufSize)
{
#if defined(__WXMSW__)
	// Make sure we have a buffer to use
	ENSURE(WideCharBufSize >= 2,"The wide char buffer size should be at least 2");
	if (WideCharBufSize < 2) return 0;

	INT32 NumChars = ToUnicode(VirtKey,ScanCode,pKeyState,pWideCharBuf,WideCharBufSize,0);

	// NumChars == 0 if ToUnicode() failed to generate a code for the keypress

	// If no direct eqivalent Unicode char, see if we can generate one with the ASCII value for the VirtKey
	// or look one up in our extra Unicode table
	if (NumChars == 0)
	{
		WORD pWordBuf[10];

		// See if ToAscii() can make a Latin I ASCII char out of it
		INT32 NumASCIIChars = ToAscii(VirtKey,ScanCode,pKeyState,pWordBuf,0);

		if (NumASCIIChars == 1)
		{
			// Map the lower byte ASCII value onto the equivalent Unicode char
			pWideCharBuf[0] = pWordBuf[0] & 0xff;
			NumChars = 1;
		}
	}

	if (NumChars == 0)
	{
		// ToAscii() failed, so lets have a look in our table

		INT32 i=0;
		BOOL finished = FALSE;

		do
		{
			// We've finished if we've reached the ungeneratable VK code
			finished = (ExtraUnicodes[i].VirtKey == CAMKEY(CC_NONE));

			if (!finished && ExtraUnicodes[i].VirtKey == VirtKey)
			{
				// We have found an entry in our table for the given virtual key

				// Stuff the Unicode value into the buffer and set the num chars generated to 1
				pWideCharBuf[0] = ExtraUnicodes[i].Unicode;
				NumChars = 1;
				finished = TRUE;
			}
			i++;
		} while (!finished);

		// Check the runtime list too
		if (NumChars != 1)
		{
			finished = FALSE;
			AdditionalVirtKey* pKey = (AdditionalVirtKey*)AdditionalVirtKeys.GetHead();
			while (!finished && (pKey != NULL))
			{
				if (pKey->VirtualKeyCode == VirtKey)
				{
					pWideCharBuf[0] = pKey->UnicodeValue;
					NumChars = 1;
					finished = TRUE;
				}

				pKey = (AdditionalVirtKey*)AdditionalVirtKeys.GetNext(pKey);
			}
		}
	}

	if (NumChars == 0)
	{
		// If the virt key is obviously one that should be handled (i.e. an alphabetical key),
		// we automatically generate a code.
		// This Unicode code will be the same as the one generated by Ctrl+<alphabetical key>
		//
		// "Why do we need this?" I hear you scream. This is because ToUnicode() fails when you do a 
		// Ctrl+Alt+<key>.  As this is an important modifier combo used by the hot key system, we need to make it work.
		//
		// Markn - 11/1/95

		if (VirtKey >= 'A' && VirtKey <= 'Z')
		{
			pWideCharBuf[0] = VirtKey - 'A';
			NumChars = 1;
		}
	}

#ifdef _DEBUG
	/*
	if (IsUserName("MarkN") && (NumChars == 0))
	{ 
		Beep();
		TRACE( _T("\n***\nVirtKey %lx has no Unicode equivalent!\n***\n"),VirtKey); 
	}
	*/
#endif

	// Return the number of Unicode chars that have been dumped into the buffer
	return NumChars;
#else
	return 0;
#endif
}
		

/********************************************************************************************

> static BOOL KeyPress::IgnoreKeyMessage(KeyPressSysMsg* pKeySysMsg)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/8/94
	Inputs:		pKeySysMsg = Ptr to a platform indepenent version of a Windows key press message
	Outputs:	-
	Returns:	TRUE  if the key press message should be ignored
				FALSE if the key press message should be processed
	Purpose:	This filters out any invalid key messages that we may get from Windows, such as 
				auto-repeated modifier (shift, ctrl, alt) keys.
	Errors:		-
	SeeAlso:	KeyPress::GenerateKeyPress

********************************************************************************************/

BOOL KeyPress::IgnoreKeyMessage(KeyPressSysMsg* pKeySysMsg)
{
//	PORTNOTETRACE( "other", "KeyPress::IgnoreKeyMessage - do nothing" );

	ENSURE(pKeySysMsg->IsValid(),"Invalid KeyPressSysMsg given");

	// Ignore if it's a key down msg, and the key is a modifier, and it's an auto-repeat message
	BOOL Ignore = (pKeySysMsg->Message == KM_KEYDOWN && KeyPress::IsModifier(pKeySysMsg->VirtKey) && pKeySysMsg->PrevDown);

#if !defined(EXCLUDE_FROM_XARALX)
	KeyPress* pNew = new KeyPress();
	if (UnicodeManager::IsDBCSOS())
	{
		// handling for DBCS characters; they roll up as two sepearte WM_CHAR messages
		if (!Ignore && pKeySysMsg->Message==KM_CHAR)
		{
			if (LastLeadByte==0 && IsDBCSLeadByte(pKeySysMsg->VirtKey))
			{
				// It's a lead byte so store it away for the next message and ignore this one
				TRACE( _T("Lead byte %d\n"), pKeySysMsg->VirtKey);
				LastLeadByte = pKeySysMsg->VirtKey;
				Ignore = TRUE;
			}
		}
	}
#endif
	
	return Ignore;
}

/********************************************************************************************

> static void KeyPress::UpdateModifierStatus(KeyPressSysMsg* pKeySysMsg)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/8/94
	Inputs:		pKeySysMsg = Ptr to a platform indepenent version of a Windows key press message
	Outputs:	-
	Returns:	-
	Purpose:	If the message is a modifier key press, the static status vars for the modifier keys
				in the KeyPress class are updated.
	Errors:		-
	SeeAlso:	KeyPress::GenerateKeyPress

********************************************************************************************/

void KeyPress::UpdateModifierStatus(KeyPressSysMsg* pKeySysMsg)
{
	ENSURE(pKeySysMsg->IsValid(),"Invalid KeyPressSysMsg given");

	// pKeySysMsg is not used at the moment
	// The modifier key state is read via GetKeyState()

	BOOL AdjL = KeyPress::AdjustStateLeft;		BOOL AdjR = KeyPress::AdjustStateRight;
	BOOL ConL = KeyPress::ConstrainStateLeft;	BOOL ConR = KeyPress::ConstrainStateRight;
	BOOL AltL = KeyPress::AlternativeStateLeft;	BOOL AltR = KeyPress::AlternativeStateRight;
	BOOL OptL = KeyPress::OptionStateLeft;		BOOL OptR = KeyPress::OptionStateRight;

	KeyPress::AdjustStateLeft  		= wxGetKeyState(CAMKEY(CC_MOD_ADJUST));
	KeyPress::AdjustStateRight 		= KeyPress::AdjustStateLeft;

	KeyPress::ConstrainStateLeft 	= wxGetKeyState(CAMKEY(CC_MOD_CONSTRAIN));
	KeyPress::ConstrainStateRight 	= KeyPress::ConstrainStateLeft;

	KeyPress::AlternativeStateLeft  = wxGetKeyState(CAMKEY(CC_MOD_ALTERNATIVE));
	KeyPress::AlternativeStateRight = KeyPress::AlternativeStateLeft;

	KeyPress::OptionStateLeft  		= FALSE;
	KeyPress::OptionStateRight 		= FALSE;

	KeyPress::fModifierChanged =(AdjL != KeyPress::AdjustStateLeft) 	|| (AdjR != KeyPress::AdjustStateRight) 	||
								(ConL != KeyPress::ConstrainStateLeft) 	|| (ConR != KeyPress::ConstrainStateRight) 	||
								(AltL != KeyPress::AlternativeStateLeft)|| (AltR != KeyPress::AlternativeStateRight)||
								(OptL != KeyPress::OptionStateLeft)		|| (OptR != KeyPress::OptionStateRight);
}

/********************************************************************************************

> static KeyPress* KeyPress::MakeKeyPress(KeyPressSysMsg* pKeySysMsg)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/8/94
	Inputs:		pKeySysMsg = Ptr to a platform indepenent version of a Windows key press message
	Outputs:	-
	Returns:	Ptr to a valid KeyPress object
				OR NULL if not enough memory
	Purpose:	Makes a KeyPress from a KeyPressSysMsg.
	Errors:		-
	SeeAlso:	KeyPress::GenerateKeyPress,KeyPress::GenerateUnicode

********************************************************************************************/

KeyPress* KeyPress::MakeKeyPress(KeyPressSysMsg* pKeySysMsg)
{
//	PORTNOTETRACE( "other", "KeyPress::MakeKeyPress - do nothing" );
	KeyPress*			pKeyPress = NULL;		// This will point to the key press object

	// Try and get a Unicode char to represent the key press
	WCHAR pWideChar[10];
	INT32 NumChars = 0;
	
	if (pKeySysMsg->Message == KM_CHAR)
	{
		// If the message is a CHAR message, then we need to generate a Unicode value ourselves
		// using the char code (which is stored in the VirtKey field of the KeyPressSysMsg class)
		//
		// These codes are generated by holding down Alt and typing the ASCII value of a character
		// on the numerical key pad. The Virt key codes generated when doing this fail to produce
		// Uncode chars, so the key presses never get passed on.  However, if after this sequence an
		// ASCII char is specified (e.g. 'A' via Alt+ '6' '5') a WM_CHAR message is posted, so we need
		// to trap this and pass the Unicode equivalent of the ASCII char onto Camelot.
		//
		// The fake VirtKey code (CAMKEY(CC_NONE)) is specified so that the receiver of this key message
		// will not process it on the VirtKey code.  However, anyone interested in just the Unicode
		// value (e.g. the text tool) will process it successfully.

		TRACE( _T("Trail byte %d\n"), pKeySysMsg->VirtKey);

		char pants[3];

		if (UnicodeManager::IsDBCSOS())
		{
			// See if we have a lead byte 
			if (LastLeadByte != 0)
			{
				pants[0] = LastLeadByte;
				pants[1] = pKeySysMsg->VirtKey;
				pants[2] = 0;
				pWideChar[0] = UnicodeManager::MultiByteToUnicode(UnicodeManager::ComposeMultiBytes(LastLeadByte, pKeySysMsg->VirtKey));
			}
			else
			{
				pants[0] = pKeySysMsg->VirtKey;
				pants[1] = 0;
				pWideChar[0] = pKeySysMsg->VirtKey;
			}
			LastLeadByte = 0;
		}
		else
		{
			pants[0] = pKeySysMsg->VirtKey;
			pants[1] = 0;
			pWideChar[0] = pKeySysMsg->VirtKey;
		}

		TRACE( _T("Entered character %s\n"),pants);
		TRACE( _T("Unicode value = %X\n"),pWideChar[0]);

		pKeySysMsg->VirtKey = CAMKEY(CC_NONE);
		NumChars = 1;
	}
	else
	{
#if FALSE != wxUSE_UNICODE
		NumChars = 1;
		pWideChar[0] = pKeySysMsg->m_Char;
		
		// We still have to try our custom translations (for VirtKey >= WXK_START
		if( pKeySysMsg->VirtKey >= WXK_START )
		{
			INT32 i;
			for( i = 0; ExtraUnicodes[i].VirtKey != CAMKEY(CC_NONE); ++i )
			{
				// We have found an entry in our table for the given virtual key
				if( ExtraUnicodes[i].VirtKey == pKeySysMsg->VirtKey )
				{
					// Stuff the Unicode value into the buffer and set the num chars generated to 1
					pWideChar[0] = ExtraUnicodes[i].Unicode;
					break;
				}
			}
			
			// Don't pass on unknown function keys
			if( ExtraUnicodes[i].VirtKey == CAMKEY(CC_NONE) )
				NumChars = 0;
		}
		
#elif defined(__WXMSW__)
		BYTE pKeyState[256];			// Array to hold the current state of the keyboard
		if (GetKeyboardState(pKeyState))	// Scan the current keyboard state
		{
			NumChars = GenerateUnicode( pKeySysMsg->VirtKey, pKeySysMsg->ScanCode, pKeyState, pWideChar, 10 );
		}
#else
		// This should never happen, but just in case we'll use a dumb implementation
		NumChars = 1;
		mbtowc( pWideChar, pKeySysMsg->VirtKey ); 
#endif
	}

	if (NumChars == 1) // Make a KeyPress object if we have a valid associated Unicode character
	{
		// Construct a KeyPress object
	 	pKeyPress = new KeyPress(pKeySysMsg,pWideChar[0]);

		// If we make an invalid one, delete it and make sure we return NULL
	 	if (pKeyPress != NULL && !pKeyPress->IsValid())
		{
			delete pKeyPress;
			pKeyPress = NULL;
		}
	}

	return pKeyPress;
}

/********************************************************************************************

> static BOOL KeyPress::GenerateKeyPress(MSG* pMsg,KeyPress** ppKeyPress)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/8/94
	Inputs:		pMsg = A Windows message
	Outputs:	*ppKeyPress = ptr to the generated KeyPress object, or NULL if not enough memory
	Returns:	TRUE  if the message was to do with a key press.
				FALSE if the message wasn't to do with the keyboard at all & *ppKeyPress is unspecified
	Purpose:	Generates a legal, platform-independent key press object from the raw platform-dependent
				data provided.
	Errors:		If the virtual key code doesn't have an equivalent Unicode value (either standard, or one that
				we have allocated to it), then KeyPress::IsValid() will return FALSE.
	SeeAlso:	KeyPress::IsValid

********************************************************************************************/

BOOL KeyPress::GenerateKeyPress(wxKeyEvent* pMsg,KeyPress** ppKeyPress)
{
	// Ensure *ppKeyPress is valid, just in case we have to exit the function early
	*ppKeyPress = NULL;

	// Create a platform-indy key press message object
	KeyPressSysMsg KeySysMsg(pMsg);

	if (KeySysMsg.IsValid())
	{
		// If it's a key press message that we can ignore, then ignore it, pretending we have processed it

		if (!IgnoreKeyMessage(&KeySysMsg))
		{
			// We like the look of this key press event

			// Ensure the modifier key states are up to date
			UpdateModifierStatus(&KeySysMsg);

			// Make a KeyPress object
			*ppKeyPress = MakeKeyPress(&KeySysMsg);

			// If the mods have changed, and we were able to make a keypress object, send
			// a "modifiers have changed message"
			if (KeyPress::fModifierChanged && *ppKeyPress != NULL)
			{
				BROADCAST_TO_ALL(KeyMsg(KeyMsg::MODIFIERCHANGED,*ppKeyPress));
			}
		}

#ifdef _DEBUG
/*
		if (*ppKeyPress != NULL)
		{
			char s[1000];
			_stprintf(s,"Mess = %d, Virtkey = 0x%lx, Ext = %d, PrevD = %d, Shift = %d, Ctrl = %d, Alt = %d, AShift = 0x%x, ACtrl = 0x%x, AAlt = 0x%x, IsAd = %d, IsCon = %d, IsAlt = %d",
			KeySysMsg.Message,
			KeySysMsg.VirtKey,
			KeySysMsg.Extended,
			KeySysMsg.PrevDown,
			KeyPress::AdjustStateLeft | KeyPress::AdjustStateRight,
			KeyPress::ConstrainStateLeft | KeyPress::ConstrainStateRight,
			KeyPress::AlternativeStateLeft | KeyPress::AlternativeStateRight,
			AsyncShift,
			AsyncCtrl,
			AsyncAlt,
			(*ppKeyPress)->IsAdjust(),
			(*ppKeyPress)->IsConstrain(),
			(*ppKeyPress)->IsAlternative()
			);
			ENSURE(FALSE,s);
		}
  */
		//DumpKeyMessage(pMsg);

#endif // _DEBUG

		// Return TRUE to say that it really was a keyboard message
		return TRUE;
	}
	else
		return FALSE;	// Not a keybaord type event that we're interested in
}


/********************************************************************************************

>	static void KeyPress::ResetModifiers()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com> (Equivalent to Tim's ResetModifierKeys() routine)
	Created:	26/08/94
	Purpose:	Maintain integrity of our key modifier status.  Win32S doesn't always
				tell us what key events have occured - notably when the user uses Alt-TAB
				to switch tasks.  Hence this gets called whenever our main window is
				deactivated or activated.  The temporary tool is updated if required.
	SeeAlso:	UpdateTemporaryTool

********************************************************************************************/

void KeyPress::ResetModifierKeys()
{
	//ENSURE(FALSE,"KeyPress::ResetModifierKeys() called");

	// We can't trust Win32S to tell us about all keypreses we need to know about,
	// so set all modifiers to ff to avoid weird effects.
	if (INCOMPLETE_KEYS)
	{
		KeyPress::AdjustStateLeft		= FALSE;
		KeyPress::AdjustStateRight		= FALSE;
		KeyPress::ConstrainStateLeft	= FALSE;
		KeyPress::ConstrainStateRight	= FALSE;
		KeyPress::AlternativeStateLeft 	= FALSE;
		KeyPress::AlternativeStateRight	= FALSE;
		KeyPress::OptionStateLeft		= FALSE;
		KeyPress::OptionStateRight		= FALSE;
	}

	// Temporary tool may have changed - update it.
	//UpdateTemporaryTool();
}



/********************************************************************************************
>	static BOOL KeyPress::IsAdjustPressed()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/12/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the "Adjust" key is currently depressed.
	Purpose:	Tests status of the "Adjust" key.
	Errors:		-
	SeeAlso:	-
********************************************************************************************/

BOOL KeyPress::IsAdjustPressed()
{
	return ::wxGetKeyState(CAMKEY(CC_MOD_ADJUST));
}



/********************************************************************************************
>	static BOOL KeyPress::IsConstrainPressed()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/12/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the "Constrain" key is down.
	Purpose:	Tests status of the "Constrain" key.
	Errors:		-
	SeeAlso:	-
********************************************************************************************/

BOOL KeyPress::IsConstrainPressed()
{
	return ::wxGetKeyState(CAMKEY(CC_MOD_CONSTRAIN));
}



/********************************************************************************************
>	static BOOL KeyPress::IsAlternativePressed()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/12/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the "Alternative" key is down.
	Purpose:	Tests status of the "Alternative" key.
	Errors:		-
	SeeAlso:	-
********************************************************************************************/

BOOL KeyPress::IsAlternativePressed()
{
	return ::wxGetKeyState(CAMKEY(CC_MOD_ALTERNATIVE));
}

/********************************************************************************************
>	static BOOL KeyPress::IsKeyPressed(UINT32 VirtKey)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/7/94
	Inputs:		VirtKey = key to check
	Outputs:	-
	Returns:	TRUE if the VirtKey key is down.
	Purpose:	General async keypress func call.
	Errors:		-
	SeeAlso:	-
********************************************************************************************/

BOOL KeyPress::IsKeyPressed(UINT32 VirtKey)
{
	return ::wxGetKeyState( wxKeyCode(VirtKey) );
}


/********************************************************************************************

>	static BOOL KeyPress::IsGalleryShiftPressed(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/2/95

	Returns:	TRUE if the "Gallery Shift" key is down.

	Purpose:	Tests status of the "Gallery Shift" key
				This is a special method for use by galleries, which must emulate the
				action of windows list boxes. "Gallery Shift" is the key used to select
				multiple items at once.

	Notes:		SHOULD NOT be used outside the gallery system

********************************************************************************************/

BOOL KeyPress::IsGalleryShiftPressed(void)
{
	return ::wxGetKeyState(CAMKEY(SHIFT));
}



/********************************************************************************************

>	static BOOL KeyPress::IsGalleryCtrlPressed(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/2/95

	Returns:	TRUE if the "Gallery Ctrl" key is down.

	Purpose:	Tests status of the "Gallery Ctrl" key
				This is a special method for use by galleries, which must emulate the
				action of windows list boxes. "Gallery Ctrl" is the key used to add/remove
				single items to/from a selection

	Notes:		SHOULD NOT be used outside the gallery system

********************************************************************************************/

BOOL KeyPress::IsGalleryCtrlPressed(void)
{
	return ::wxGetKeyState(CAMKEY(CONTROL));
}


	   


/********************************************************************************************

>	static BOOL KeyPress::IsEscapePressed()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/9/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE  - if an escape key is being pressed, or was pressed since the last call
						to this routine
				FALSE - no escape key has been pressed
	Purpose:	Lets you see if an escape key combination is currently being pressed, or has been pressed
				since the last call to this routine

				It does a GetAsyncKeyState() on certain key combinations, currently -
					CAMKEY(ESCAPE)
					CAMKEY(CANCEL)	(Ctrl-Pause, or Break)

				GetAsyncKeyState() returns the current key state AND whether the key was pressed since
				the last call to GetAsyncKeyState().

				Note: 	If TRUE is returned, all pending key messages will have been removed before hand.
						using PeekMessage(...WM_KEYFIRST,WM_KEYLAST,PM_REMOVE)
	SeeAlso:	

********************************************************************************************/

BOOL KeyPress::IsEscapePressed()
{
#pragma message( __LOCMSG__ _T("KeyPress::IsEscapePressed - do nothing") )
//	TRACE( _T("Warning - KeyPress::IsEscapePressed called") );
/*	BOOL EscapePressed = (GetAsyncKeyState(CAMKEY(ESCAPE)) != 0);
	BOOL Pressed       = EscapePressed || (GetAsyncKeyState(CAMKEY(CANCEL)) != 0);

	if (Pressed)
	{
		BOOL EscapeKeyDown = FALSE;

		// Throw away all pending key messages
		MSG msg;
		while(PeekMessage( &msg, NULL, WM_KEYFIRST, WM_KEYLAST, PM_REMOVE ))
		{
			// If we the actual Escape key has been pressed, see if we got an Escape Key Down event.
			if (EscapePressed)
			{
				KeyPressSysMsg KeyMsg(&msg);
				if (KeyMsg.IsValid() && (KeyMsg.Message == KM_KEYDOWN) && (KeyMsg.VirtKey == CAMKEY(ESCAPE)))
					EscapeKeyDown = TRUE;
			}
		}

		// Alt+Tab Esc fix - i.e. Aborting a task switch via Alt+Tab using the Escape key.
		//
 		// If the actual Escape key has been pressed, only allow it if we have been posted a KeyDown event
		// for the Escape key.
		// 
		// When you do Alt+Tab while in Studio, then hit Escape, unfortunately we get posted an Escape Key Up event
		// which also means that the line above that reads "GetAsyncKeyState(CAMKEY(ESCAPE)) != 0" will return TRUE.
		// Obviously we want to ignore the Escape key being pressed in this circumstance, but it seems the only way 
		// to identify the Escape key press as one we don't want is to see if we *don't* have a corresponding
		// key down event.
		//
		// if (GetAsyncKeyState(CAMKEY(ESCAPE)) != 0)  // i.e. the Escape key has been pressed
		// {
		//     if (App received a Escape KeyDown message)
		//	      Legitimate Escape key press, so Pressed = TRUE
		//	   else
		//        Escape hit outside our app, so ignore it
		// }

		// If the ActualEscape key has been pressed, then Pressed should only be TRUE if we have received a
		// Escape Key Down event.
		if (EscapePressed)
			Pressed = EscapeKeyDown;
	}

	return (Pressed); */

	return false;
}

/********************************************************************************************

>	static BOOL KeyPress::EscapePressed(KeyPress* pKeyPress)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/9/94
	Inputs:		pKeyPress = ptr to a key press
	Outputs:	-
	Returns:	TRUE  - the key was an escape event (e.g. Escape, Break, etc)
				FALSE - the key was not an escape key, nothing happened
	Purpose:	Deals with an Escape key press

				If the key press was NOT an escape key, (i.e. not Escape or Ctrl+Pause (Break)),
				nothing happens, and FALSE is returned

				if it was an escape key press, it is processed in a relevent way
				At the moment, this just means that the current drag is terminated, if there is one.

				If it's an escape key press, and it was acted upon (e.g. a drag was terminated), TRUE is returned.
				Otherwise FALSE is returned
	SeeAlso:	

********************************************************************************************/

BOOL KeyPress::EscapePressed(KeyPress* pKeyPress)
{
	ENSURE(pKeyPress->IsValid(),"Invalid KeyPress given");

	BOOL Processed = FALSE;
	UINT32 VirtKey = pKeyPress->GetVirtKey();

	// Only check non-auto-repeat "key down" key presses
	if (!pKeyPress->IsRelease() && !pKeyPress->IsRepeat())
	{
		if (VirtKey == CAMKEY(ESCAPE) || VirtKey == WXK_CANCEL)
		{
#if !defined(EXCLUDE_FROM_RALPH) && !defined(EXCLUDE_FROM_XARALX)						
			// MarkN I've moved this code in an attempt to stop the selection being
			// cleared after a bar drag cancel - Chris.
			if (!Processed)
				Processed = BaseBar::EscPressed(TRUE);

			// cancel any drag manager drags (and say that we have done so!!!)
			// stops the selection being cleared when you cancel a
			// colour or sprite (or any) DragManagerOp drag
			if(!Processed && DragManagerOp::GetCurrentManager() != NULL)
			{
				DragManagerOp::AbortDrag();
				Processed = TRUE;
			}
#endif

			if (!Processed && Operation::GetCurrentDragOp() != NULL)
			{
				OilCoord PointerPos(0,0);
				ClickModifiers ClickMods;
//				Spread* pSpread = NULL;

				DocView* pDocView = DocView::GetSelected();
				if (pDocView)
					pDocView->DragFinished(Operation::GetCurrentDragOp(), PointerPos, ClickMods, FALSE);

				// We've done something with this escape key press so set Processed to TRUE
				// to prevent it from being passed on
				Processed = (Operation::GetCurrentDragOp() == NULL);
			}

			if (!Processed && Operation::GetCurrentDragOp() != NULL)
			{
				DocCoord PointerPos(0,0);
				ClickModifiers ClickMods;
				Spread* pSpread = NULL;

				Operation::GetCurrentDragOp()->DragFinished(PointerPos, ClickMods, pSpread, FALSE, FALSE);

				// We've done something with this escape key press so set Processed to TRUE
				// to prevent it from being passed on
				Processed = TRUE;
			}

			// This gives Chris a chance to terminate his bar dragging.  This was necessary as his former
			// way of detecting Escape (via KeyMsg broadcasts) no longer works because Escape is now a hot key, and
			// as such gets in there before a KeyMsg is broadcast.
			// (MarkN 27/4/95)
			/*if (!Processed)
				Processed = BaseBar::EscPressed(TRUE);
			  */
			// Throw away all pending key messages
PORTNOTE( "other", "Remove PeekMessage" )
#if !defined(EXCLUDE_FROM_XARALX)						
			MSG msg;
			while(PeekMessage( &msg, NULL, WM_KEYFIRST, WM_KEYLAST, PM_REMOVE ))
			;
#endif
		}
	}

	return Processed;
}

/********************************************************************************************

>	static BOOL KeyPress::GenerateCharMessage(MSG* pMsg)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com> 
	Created:	10/3/95
	Inputs:		pMsg = A Windows keypress message
	Outputs:	-
	Returns:	TRUE  if the message was processed
				FALSE if the message wasn't processed
	Purpose:	This tries to see if the keypress message can generate a WM_CHAR message.

				It does this by trapping numerical key pad entry of the ASCII value of the char while
				the Alt ky is held down. (e.g. hold down ALT, type '6' then '5' on the num key pad, then release
				Alt - you should get 'A' appearing at the input focus).  If, on a Alt key up event we have a
				valid ASCII value that's been entered, we post ourselves a WM_CHAR message containing the ASCII
				char.

				We have to do this ourselves because we get less messages generated in Win 3.1 than we do in
				NT, so the NT solution (act upon the auto generated WM_CHAR message) won't work an all target
				platforms (pain in the butt or what?). The ideal solution is to find out exactly why we don't
				get this extra WM_CHAR message in Win3.1, but I've traced paths of key press messages under NT
				and have found no obvious reason why the Win3.1 should behave differently. As we don't have single
				stepping technology under Win3.1, it's almost impossible to determine what's going on.

				So, the time honoured CC tradition, if the OS doesn't do it, we'll do it ourselves.


	Errors:		-
	SeeAlso:	KeyPress::TranslateMessage

********************************************************************************************/

BOOL KeyPress::GenerateCharMessage(wxKeyEvent* pMsg)
{
	BOOL Processed = FALSE;

	// Get the virtual key code for the key pressed
	UINT32 VirtKey = pMsg->GetKeyCode();

	//DumpKeyMessage(pMsg);

	if( wxEVT_KEY_DOWN == pMsg->GetEventType() ) 
	{
		if (VirtKey == CAMKEY(MENU))
		{
			// Trap ALL Alt key down messages

	 		if (!KeyPress::ValidAsciiVal)
			{
				// Initialise a ASCII value entry
		 		//TRACE( _T("-*-*-*-- Starting valid ascii val\n"));
				KeyPress::ValidAsciiVal = TRUE;
				KeyPress::AsciiVal 	    = -1;
			}
		}
		else if (KeyPress::ValidAsciiVal && ((VirtKey < CAMKEY(NUMPAD0)) || (VirtKey > WXK_NUMPAD9)))
		{
			// If the key down message is not a num pad key between '0' and '9'
			// invalidate the ASCII value
			//TRACE( _T("-*-*-*-- (keydown) Invalidating ascii val\n"));
			KeyPress::ValidAsciiVal = FALSE;
		}
	}
	else
	if( wxEVT_KEY_UP == pMsg->GetEventType() ) 
	{
		if (KeyPress::ValidAsciiVal)
		{
			// Only try to produce an ASCII value if the last set of key presses has generated
			// a valid value

			if (VirtKey == CAMKEY(MENU))
			{
				// We've received a Alt key up message, so post ourselves a WM_CHAR message that
				// contains the generated ASCII value

		 		if (KeyPress::AsciiVal >= 0)
				{
					// Only post the ASCII value if one's been specified via the Num key pad
					// (i.e. the value >= 0)
			 		//TRACE( _T("-*-*-*-- Posting WM_CHAR. Val = %ld\n"),KeyPress::AsciiVal);
#pragma message( __LOCMSG__ _T("Removed PostMessage usage - WM_CHAR from ascii code") )
//					Processed = ::PostMessage(pMsg->hwnd,WM_CHAR,KeyPress::AsciiVal % 256,1);
				}
				//TRACE( _T("-*-*-*-- (keyup) Invalidating ascii val. Val = %ld\n"),KeyPress::AsciiVal);
				KeyPress::ValidAsciiVal = FALSE;
			}
			else if ((VirtKey >= CAMKEY(NUMPAD0)) && (VirtKey <= WXK_NUMPAD9))
			{
				// We've got a num key pad key up event
				// We have to do this bit on key up events, because for some reason under NT (and possibly
				// other OSs) we don't get a key down event for the first key that's pressed while the
				// Alt key is auto-repeating (wierd - any ideas welcome)

				// If this is the first one (i.e. ASCII val is currently < 0), reset the value to 0
				if (KeyPress::AsciiVal < 0)
					KeyPress::AsciiVal = 0;

				// Shift the current value up, and add in the digit of the num key pad key.
				KeyPress::AsciiVal = ((KeyPress::AsciiVal*10)+(VirtKey-CAMKEY(NUMPAD0)));

				// We've processed this key press
				Processed = TRUE;

				//TRACE( _T("-*-*-*-- ascii val digit received. Val = %ld\n"),KeyPress::AsciiVal);
			}
		}
	}

	return Processed;
}

/********************************************************************************************

>	static BOOL KeyPress::TranslateMessage(MSG* pMsg)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com> 
	Created:	26/08/94
	Inputs:		pMsg = A Windows message
	Outputs:	-
	Returns:	TRUE  if the message was to do with a key press AND was processed
				FALSE if the message wasn't to do with the keyboard at all OR key press was not processed
	Purpose:	Called from a low-level PreTranslateMessage() routine to allow key presses to be processed
				If it is a key press message, it generates a KeyPress object and sends it off to
				Application::OnKeyPress()
	Errors:		-
	SeeAlso:	Application::OnKeyPress

********************************************************************************************/

BOOL KeyPress::TranslateMessage(wxKeyEvent* pMsg)
{
#if !defined(EXCLUDE_FROM_RALPH) && !defined(EXCLUDE_FROM_XARALX)
	// What kind of message is it?
	BOOL KeyDown = wxEVT_KEY_DOWN == pMsg->GetEventType();
	BOOL KeyUp   = wxEVT_KEY_UP == pMsg->GetEventType();

	// If it's a key event, lose the bubble help.
	if (KeyDown || KeyUp)
		ControlHelper::BubbleHelpDisable();
#endif

	// Normal key processing.
	KeyPress* pKeyPress;
	BOOL Processed = FALSE;

	// See if we can generate an WM_CHAR message from the key press
	if (!Processed)
		Processed = GenerateCharMessage(pMsg);

	// Can we make a key press event from this message?
	if (!Processed && KeyPress::GenerateKeyPress(pMsg,&pKeyPress))
	{
		if (pKeyPress != NULL)
		{
			// The message was a key press and we have a key press object at pKeyPress

	 		// check for an escape key press
			if (!Processed)
				Processed = EscapePressed(pKeyPress);

			// Now see if Camelot wants to use it
			if (!Processed)
				Processed = GetApplication()->OnKeyPress(pKeyPress);

			// Don't process Alt key presses.  By always returning FALSE on Alt key presses, we get
			// normal functionality from single press/releases of Alt, even in the Selector tool
			if (Processed && pKeyPress->VirtKey == CAMKEY(MENU))
				Processed = FALSE;

			//TRACE( _T("Processed = %d\n"),Processed);

			// Must delete the key press after use
			delete pKeyPress;
		}
	}

	if (!Processed)
	{
		// MFC ISSUES...
		// If we have had a key press we wish to ignore totally (i.e. repeated modifier key presses)
		// then return TRUE so that it doesn't get passed onto the rest of MFC.
		//
		// This is mainly for performance improvements during a drag AND modifier key press op, e.g. Alt held down
		// in the freehand tool for drawing straight lines.
		//
		// This fix makes modified drags usable in retail builds.  In debug builds, you get an extra hit with
		// AfxValidateObject (or what ever it's called), which takes about 50% of the time.
		//
		// During Drags:
		// We want to prevent key presses getting to MFC during drags.  In particular things like 
		// Ctrl-Tab (i.e. switch to new doc)

		KeyPressSysMsg KeySysMsg(pMsg);
		if (KeySysMsg.IsValid())
		{
			// We have a keypress message
			// Prevent it getting to MFC if we are dragging or it's one we wish to ignore
			Processed = ((Operation::GetCurrentDragOp()!=NULL) || IgnoreKeyMessage(&KeySysMsg));
		}
	}

	return Processed;
}
 
/********************************************************************************************

>	INT32 KeyPress::operator==(const KeyPress& other)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/8/94
	Inputs:		other = the key press you wish to check for equivalence with this key press
	Outputs:	-
	Returns:	TRUE if equal, FALSE otherwise
	Purpose:	KeyPress == operator
				Two KeyPress objects are equal if
					1) They have the same virtual key code
					2) Have the same modifier settings
					3) Have the same extended flag settings
					4) Have the same validity (i.e. both valid OR both invalid)

				THIS DOES NOT COMPARE THE UNICODE COMPONENTS OF THE TWO KeyPress OBJECTS.
				To compare Unicode values, use the KeyPress::GetUnicode() func.
	Errors:		-
	SeeAlso:	KeyPress::GetUnicode()

********************************************************************************************/

INT32 KeyPress::operator==(const KeyPress& other)
{
	return ( 
			VirtKey 	== other.VirtKey 	 &&
			Adjust 		== other.Adjust  	 &&
			Constrain 	== other.Constrain 	 &&
			Alternative	== other.Alternative &&
			Option		== other.Option 	 && 
			Extended	== other.Extended	 &&
			Valid		== other.Valid
		   );
}



/********************************************************************************************

>	static BOOL KeyPress::AddVirtualKeyCode(UINT32 VCode, WCHAR Unicode)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/10/95
	Inputs:		VCode - the virtual key code to add
				Unicode - the unicode value generated when this key is pressed
	Outputs:	-
	Returns:	TRUE/FALSE for success/failure
	Purpose:	Adds the given virtual key code to the list that Camelot knows about and
				send around internally (rather than ignoring)
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
BOOL KeyPress::AddVirtualKeyCode(UINT32 VCode, WCHAR Unicode)
{
	// Construct a list item
	AdditionalVirtKey* pNew = new AdditionalVirtKey;

	if (pNew != NULL)
	{
		pNew->VirtualKeyCode = VCode;
		pNew->UnicodeValue = Unicode;

		AdditionalVirtKeys.AddHead(pNew);
	}

	return (pNew != NULL);
}



/********************************************************************************************

>	void KeyPress::RemoveVirtualKeyCode(UINT32 VCode, WCHAR Unicode)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/10/95
	Inputs:		VCode - the virtual key code to remove
				Unicode - the unicode value generated when this key is pressed
	Outputs:	-
	Returns:	-
	Purpose:	Removes the given virtual key code that generates the givne unicode value
				from the list of additional virtual key codes.  Only the first occurace is
				removed.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
void KeyPress::RemoveVirtualKeyCode(UINT32 VCode, WCHAR Unicode)
{
	// Loop through the list until we find the list item
	BOOL finished = FALSE;
	AdditionalVirtKey* pKey = (AdditionalVirtKey*)AdditionalVirtKeys.GetHead();

	while (!finished && (pKey != NULL))
	{
		AdditionalVirtKey* pNext = (AdditionalVirtKey*)AdditionalVirtKeys.GetNext(pKey);
		
		if ((pKey->VirtualKeyCode == VCode) && (pKey->UnicodeValue == Unicode))
		{
			AdditionalVirtKeys.RemoveItem(pKey);
			delete pKey;
			finished = TRUE;
		}

		pKey = pNext;
	}
}



//----------------------------------------------------------------------------------------------------

#if defined(_DEBUG) && defined(__WXMSW__)

void KeyPress::DumpKeyMessage(wxKeyEvent* pMsg)
{
#pragma message( __LOCMSG__ _T("KeyPress::DumpKeyMessage - do nothing") )
	TRACE( _T("Warning - KeyPress::DumpKeyMessage called") );
/*	//if (!IsUserName("Markn")) return;

	KeyPressSysMsg KeySysMsg(pMsg);
	WCHAR c = 1;
	KeyPress k1(&KeySysMsg,c);
	KeyPress k2(&KeySysMsg,c);

	BOOL tits = (k1 == k2);

	BOOL KeyMsg = ((pMsg->message >= WM_KEYFIRST) && (pMsg->message <= WM_KEYLAST));

	if (KeyMsg)
	{
		if (pMsg->message == WM_KEYDOWN) 		TRACE( _T("*** WM_KEYDOWN"));
		if (pMsg->message == WM_KEYUP)   		TRACE( _T("*** WM_KEYUP"));
		if (pMsg->message == WM_SYSKEYDOWN) 	TRACE( _T("*** WM_SYSKEYDOWN"));
		if (pMsg->message == WM_SYSKEYUP) 		TRACE( _T("*** WM_SYSKEYUP"));
		if (pMsg->message == WM_CHAR)			TRACE( _T("*** WM_CHAR"));
		if (pMsg->message == WM_DEADCHAR)		TRACE( _T("*** WM_DEADCHAR"));
		if (pMsg->message == WM_SYSCHAR)		TRACE( _T("*** WM_SYSCHAR"));
		if (pMsg->message == WM_SYSDEADCHAR)	TRACE( _T("*** WM_SYSDEADCHAR"));

		UINT32 VirtKey  = pMsg->wParam;
		INT32 KeyData  = pMsg->lParam;

		if (TRUE)
		{
			TRACE( _T(" VK=%lx\n"),VirtKey);
		}
		else
		{
			TRACE( _T("\n"));
			TRACE( _T("KeyData = %lx\n"),KeyData);
			TRACE( _T("VirtKey = %lx\n"),VirtKey);
			TRACE( _T("Repeat  = %lx\n"),KeyData & 0xffff);
			TRACE( _T("ScanCode= %lx\n\n"),(KeyData >> 16) & 0xff);

			TRACE( _T("Bit 24 (extended)   = %lx\n"),KeyData & 1<<24);
			TRACE( _T("Bit 29 (context)    = %lx\n"),KeyData & 1<<29);
			TRACE( _T("Bit 30 (prev state) = %lx\n"),KeyData & 1<<30);
			TRACE( _T("Bit 31 (trans stat) = %lx\n"),KeyData & 1<<31);

			//TRACE( _T("\nBit 25 (reserved)   = %lx\n"),KeyData & 1<<25);
			//TRACE( _T("Bit 26 (reserved)   = %lx\n"),KeyData & 1<<26);
			//TRACE( _T("Bit 27 (reserved)   = %lx\n"),KeyData & 1<<27);
			//TRACE( _T("Bit 28 (reserved)   = %lx\n"),KeyData & 1<<28);

			TRACE( _T("KeyPress::AdjustStateLeft       = %lx\n"),KeyPress::AdjustStateLeft);
			TRACE( _T("KeyPress::AdjustStateRight      = %lx\n"),KeyPress::AdjustStateRight);
			TRACE( _T("KeyPress::ConstrainStateLeft    = %lx\n"),KeyPress::ConstrainStateLeft);
			TRACE( _T("KeyPress::ConstrainStateRight   = %lx\n"),KeyPress::ConstrainStateRight);
			TRACE( _T("KeyPress::AlternativeStateLeft  = %lx\n"),KeyPress::AlternativeStateLeft);
			TRACE( _T("KeyPress::AlternativeStateRight = %lx\n"),KeyPress::AlternativeStateRight);
		}
	} */
}
#endif	// _DEBUG



