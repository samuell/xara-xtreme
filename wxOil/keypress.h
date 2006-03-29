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
// Definition of the KeyPress class

#ifndef INC_KEYPRESS
#define INC_KEYPRESS

#include "listitem.h"

enum KeyPressSysMsgHandle
{
	KM_KEYUP,
	KM_KEYDOWN,
	KM_CHAR
};


// This is the first in the range of privat Unicodes that Camelot will use to encode keys that do
// not have direct Unicode equivalents
#define CAMELOT_UNICODE_BASE (0xf800)
#define CAMELOT_UNICODE_LAST (0xf94f)




/********************************************************************************************
>	class AdditionalVirtKey: public ListItem

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/10/95
	Purpose:	Used to store 'additional' known virtual key codes so the range of handled
				keys can be extended at runtime
********************************************************************************************/

class AdditionalVirtKey: public ListItem
{
public:                
	UINT32 VirtualKeyCode;
	WCHAR UnicodeValue;
};	       


/********************************************************************************************

>	class KeyPressSysMsg: public CC_CLASS_MEMDUMP

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/8/94
	Purpose:	Represents a key press message that is slightly more abstract than the raw
				Windows message.
				This will hopefully aid the porting of this code to other platforms

********************************************************************************************/

class KeyPressSysMsg : public CC_CLASS_MEMDUMP
{
	CC_DECLARE_MEMDUMP(KeyPressSysMsg);

public:
	KeyPressSysMsg( wxKeyEvent *pMsg );
	BOOL IsValid();

	KeyPressSysMsgHandle Message;		// The message type, KM_ something

	UINT32				VirtKey;		// Virtual key code describing the actual key that's been pressed
	UINT32				RepeatCount;	// The repeat count for the key before message was posted
	UINT32				ScanCode;		// Keyboard scan code. Depends on keyboard manufacturer
#if FALSE != wxUSE_UNICODE
	wxChar				m_Char;			// The character code
#endif

	BOOL Extended 	: 1;			// Set when an extended key was press (e.g. right-hand Alt or Ctrl)
	BOOL PrevDown	: 1;			// Set when the key was down before the key press message was sent

private:
	BOOL Valid : 1;

	// VirtKey of last keypress, used to detect repeated keys
	static UINT32 m_LastVirtKey;
};

/********************************************************************************************

>	class KeyPress: public CC_CLASS_MEMDUMP

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/8/94
	Purpose:	Encapsulates key presses in camelot

********************************************************************************************/

class KeyPress : public CC_CLASS_MEMDUMP
{
	CC_DECLARE_MEMDUMP(KeyPress);

public:
	static BOOL Init();
	static BOOL DispatchKeyEvent(UINT32 nMsgID, UINT32 nChar, UINT32 nRepCnt, UINT32 nFlags);
	static BOOL	TranslateMessage( wxKeyEvent *pMsg );
	static void ResetModifierKeys();
	static BOOL GenerateCharMessage( wxKeyEvent *pMsg );

	// These all allow key states to be tested asynchronously, ie. without a key event happening.
	static BOOL IsAdjustPressed();
	static BOOL IsConstrainPressed();
	static BOOL IsAlternativePressed();
	static BOOL IsEscapePressed();
	static BOOL IsKeyPressed(UINT32 VirtKey);		// General async keypress func call.

	// ModifierChange() will return TRUE if the last keypress message was due to a modifier key state change
	static BOOL ModifierChanged() { return fModifierChanged; }

	// These are special methods for galleries, as they must emulate Windows list boxes
	// They should not be used outside the gallery system unless you're really really sure
	static BOOL IsGalleryShiftPressed(void);
	static BOOL IsGalleryCtrlPressed(void);

	// The constructors
	KeyPress(KeyPressSysMsg* pKeySysMsg,WCHAR UnicodeChar);
	KeyPress(UINT32 VirtKey, BOOL PlusAdjust = FALSE, BOOL PlusConstrain = FALSE, BOOL PlusAlternative = FALSE,
						   BOOL ExtendedKey = FALSE, BOOL WorksInDragsKey = FALSE);
	KeyPress(const KeyPress &c): // Copy constructor as CCObject's is private which prevents synthesis working
		VirtKey(c.VirtKey), Unicode(c.Unicode), Adjust(c.Adjust), Constrain(c.Constrain),
		Alternative(c.Alternative), Option(c.Option), Extended(c.Extended), Repeat(c.Repeat),
		RightHand(c.RightHand), Valid(c.Valid) {}

	static KeyPress* ConstructVK(TCHAR ch);

	// Lets you find out if the key press is a valid one
	BOOL IsValid();

	// Discover the key-related bits with these lovely public funcs
	UINT32 	GetVirtKey()	{ return VirtKey; }	// Returns the virtual key code
	WCHAR	GetUnicode()	{ return Unicode; }	// Returns the unicode char generated by the virtual key

	// Public access functions to the various flags of a key press (see later for flag descriptions)
	BOOL IsAdjust()			{ return Adjust; }
	BOOL IsConstrain()		{ return Constrain; }
	BOOL IsAlternative()	{ return Alternative; }
	BOOL IsOption()			{ return Option; }
	BOOL IsModified()		{ return (IsAdjust() || IsConstrain() || IsAlternative() || IsOption()); }

	BOOL IsExtended()		{ return Extended; }
	BOOL IsRepeat()			{ return Repeat; }
	BOOL IsRightHand()		{ return RightHand; }
	BOOL IsRelease() 		{ return Release; }
	BOOL IsOkInDrags()		{ return WorksInDrag; }

	// The all-important equivalence operator
	// Main users of this are HotKey and ToolSwitch, which need to do key combination comparisons
	// Note: this does NOT compare the Unicode element of KeyPress objects.
	//		 To do this you'll have to do pKeyPress1->GetUnicode() == pKeyPress2->GetUnicode()
	//		 This is because the same unicode char could theoretically be produced by different
	//		 key combinations, so a full comparison of two key presses may produce "not equal" when
	//		 they have the same unicode char.
	INT32 operator==(const KeyPress& ); 

#if defined(_DEBUG) && defined(__WXMSW__)
	static void DumpKeyMessage(wxKeyEvent* pMsg);
#endif

	static BOOL AddVirtualKeyCode(UINT32 VCode, WCHAR Unicode);
	static void RemoveVirtualKeyCode(UINT32 VCode, WCHAR Unicode);

private:
	// The default constuctor is private in order to force others to use one of the public constructors
	// or static member functions to create an instance of KeyPress
	KeyPress();
	void Initialise();

	static BOOL 		GenerateKeyPress( wxKeyEvent *pMsg,KeyPress **ppKeyPress );
	static INT32  		GenerateUnicode(UINT32 VirtKey,UINT32 ScanCode,BYTE* pKeyState,WCHAR* pWideCharBuf,UINT32 WideCharBufSize);
	static BOOL			IgnoreKeyMessage(KeyPressSysMsg* pKeySysMsg);
	static BOOL 		IsModifier(UINT32 VirtKey);
	static void 		UpdateModifierStatus(KeyPressSysMsg* pKeySysMsg);
	static KeyPress* 	MakeKeyPress(KeyPressSysMsg* pKeySysMsg);
	static BOOL 		EscapePressed(KeyPress* pKeyPress);

	UINT32  VirtKey;			// The virtual key code for the key
	WCHAR Unicode;			// The Unicode char that the virtual key generates

	BOOL Adjust 	 : 1;	// Key press modified by the Adjust modifier (usu. shift)
	BOOL Constrain 	 : 1;	// Key press modified by the Constrain modifier (usu. ctrl)
	BOOL Alternative : 1;	// Key press modified by the Alternative modifier (usu. alt)
	BOOL Option 	 : 1;	// Key press modified by the Option modifier (usu. Mac option key)

	BOOL Extended 	 : 1;	// The key was one of the extended keys (e.g. right-hand ctrl key)

	BOOL Repeat 	 : 1;	// The key press was generated by an auto-repeat (i.e. not the original keypress)
	BOOL RightHand	 : 1;	// The key press was generated by a right-hand equivelent (e.g. 'Enter' and not 'Return')
	BOOL Release	 : 1;	// The key has just been released (e.g. Windows KEYUP event)

	BOOL Valid		 : 1;	// Validity bit. Set means object is ok, clear means DON'T TOUCH IT!
	BOOL WorksInDrag : 1;	// TRUE if this keypress will work in the middle of a drag

	// These next vars hold the current state of the various modifier keys.
	// TRUE means the key is down, FALSE means the key is up
	static BOOL AdjustStateLeft;
	static BOOL AdjustStateRight;
	static BOOL ConstrainStateLeft;
	static BOOL ConstrainStateRight;
	static BOOL AlternativeStateLeft;
	static BOOL AlternativeStateRight;
	static BOOL OptionStateLeft;
	static BOOL OptionStateRight;

	// TRUE if the one or more of the modifier keys changed on the last keypress
	static BOOL fModifierChanged;

	// This info is used to generate WM_CHAR messages via Alt and the numerical key pad
	static INT32 AsciiVal;
	static BOOL ValidAsciiVal;

	// The last Lead Byte recived
	static BYTE LastLeadByte;

	// One list of additional runtime virtual key codes
	static List AdditionalVirtKeys;

};

#endif  // INC_KEYPRESS


