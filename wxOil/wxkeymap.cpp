// $Id: wxOil/wxkeymap.cpp, 1, 01-Jan-2006, Anonymous $
#include "wxkeymap.h"
#include "vkextra.h"
#include <wx/defs.h>

/********************************************************************************************

	std::map<String_32, UINT32> wxKeyMap::keymap;
	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com> M'Sadoques
	Created:	5/30/2007
	Purpose:	Holds the string to enum translations for WXK_* keys.

********************************************************************************************/
std::map<String_32, UINT32> wxKeyMap::keymap;

/********************************************************************************************

	UINT32 wxKeyMap::GetKeyVal(String_32 key_str)
	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com> M'Sadoques
	Created:	5/30/2007
	Inputs:		The String representation of a WXK_* key, for example "WXK_ADD"
	Returns:	The enum value.
	Purpose:	This function ensures that it is unnecessary to hard-code key values into
				resource or other configuration files. The enumeration name can be kept
				the same, and the actual enumeration value can change without changing this
				code.

				The first time through, the keymap map is populated with the string to enum
				translations.
********************************************************************************************/
UINT32 wxKeyMap::GetKeyVal(String_32 key_str)
{
	if(wxKeyMap::keymap.empty())
		wxKeyMap::GenerateMap();
	ENSURE(!wxKeyMap::keymap.empty(), "keymap is still empty");
	ENSURE(wxKeyMap::keymap[key_str] != 0, "key_str is invalid");
	return(wxKeyMap::keymap[key_str]);
}

/********************************************************************************************

	void wxKeyMap::GenerateMap()
	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com> M'Sadoques
	Created:	5/30/2007
	Purpose:	Generate the string to enum translations in the keymap map.

********************************************************************************************/
void wxKeyMap::GenerateMap()
{
	/* Defined in vkextra.h */
	wxKeyMap::keymap[TEXT("WXK_0")]=WXK_0;
	wxKeyMap::keymap[TEXT("WXK_1")]=WXK_1;
	wxKeyMap::keymap[TEXT("WXK_2")]=WXK_2;
	wxKeyMap::keymap[TEXT("WXK_3")]=WXK_3;
	wxKeyMap::keymap[TEXT("WXK_4")]=WXK_4;
	wxKeyMap::keymap[TEXT("WXK_5")]=WXK_5;
	wxKeyMap::keymap[TEXT("WXK_6")]=WXK_6;
	wxKeyMap::keymap[TEXT("WXK_7")]=WXK_7;
	wxKeyMap::keymap[TEXT("WXK_8")]=WXK_8;
	wxKeyMap::keymap[TEXT("WXK_9")]=WXK_9;
	wxKeyMap::keymap[TEXT("WXK_A")]=WXK_A;
	wxKeyMap::keymap[TEXT("WXK_B")]=WXK_B;
	wxKeyMap::keymap[TEXT("WXK_C")]=WXK_C;
	wxKeyMap::keymap[TEXT("WXK_D")]=WXK_D;
	wxKeyMap::keymap[TEXT("WXK_E")]=WXK_E;
	wxKeyMap::keymap[TEXT("WXK_F")]=WXK_F;
	wxKeyMap::keymap[TEXT("WXK_G")]=WXK_G;
	wxKeyMap::keymap[TEXT("WXK_H")]=WXK_H;
	wxKeyMap::keymap[TEXT("WXK_I")]=WXK_I;
	wxKeyMap::keymap[TEXT("WXK_J")]=WXK_J;
	wxKeyMap::keymap[TEXT("WXK_K")]=WXK_K;
	wxKeyMap::keymap[TEXT("WXK_L")]=WXK_L;
	wxKeyMap::keymap[TEXT("WXK_M")]=WXK_M;
	wxKeyMap::keymap[TEXT("WXK_N")]=WXK_N;
	wxKeyMap::keymap[TEXT("WXK_O")]=WXK_O;
	wxKeyMap::keymap[TEXT("WXK_P")]=WXK_P;
	wxKeyMap::keymap[TEXT("WXK_Q")]=WXK_Q;
	wxKeyMap::keymap[TEXT("WXK_R")]=WXK_R;
	wxKeyMap::keymap[TEXT("WXK_S")]=WXK_S;
	wxKeyMap::keymap[TEXT("WXK_T")]=WXK_T;
	wxKeyMap::keymap[TEXT("WXK_U")]=WXK_U;
	wxKeyMap::keymap[TEXT("WXK_V")]=WXK_V;
	wxKeyMap::keymap[TEXT("WXK_W")]=WXK_W;
	wxKeyMap::keymap[TEXT("WXK_X")]=WXK_X;
	wxKeyMap::keymap[TEXT("WXK_Y")]=WXK_Y;
	wxKeyMap::keymap[TEXT("WXK_Z")]=WXK_Z;
	wxKeyMap::keymap[TEXT("WXK_LESS")]=WXK_LESS;
	wxKeyMap::keymap[TEXT("WXK_GREATER")]=WXK_GREATER;
	wxKeyMap::keymap[TEXT("WXK_ADD")]=WXK_ADD;
	wxKeyMap::keymap[TEXT("WXK_EQUALS")]=WXK_EQUALS;
	wxKeyMap::keymap[TEXT("WXK_COMMA")]=WXK_COMMA;
	wxKeyMap::keymap[TEXT("WXK_MINUS")]=WXK_MINUS;
	wxKeyMap::keymap[TEXT("WXK_PERIOD")]=WXK_PERIOD;
	wxKeyMap::keymap[TEXT("WXK_HASH")]=WXK_HASH;

	/* Defined in wx/defs.h */
	wxKeyMap::keymap[TEXT("WXK_BACK")]=WXK_BACK;
	wxKeyMap::keymap[TEXT("WXK_TAB")]=WXK_TAB;
	wxKeyMap::keymap[TEXT("WXK_RETURN")]=WXK_RETURN;
	wxKeyMap::keymap[TEXT("WXK_ESCAPE")]=WXK_ESCAPE;
	wxKeyMap::keymap[TEXT("WXK_SPACE")]=WXK_SPACE;
	wxKeyMap::keymap[TEXT("WXK_DELETE")]=WXK_DELETE;
	wxKeyMap::keymap[TEXT("WXK_START")]=WXK_START;
	wxKeyMap::keymap[TEXT("WXK_LBUTTON")]=WXK_LBUTTON;
	wxKeyMap::keymap[TEXT("WXK_RBUTTON")]=WXK_RBUTTON;
	wxKeyMap::keymap[TEXT("WXK_CANCEL")]=WXK_CANCEL;
	wxKeyMap::keymap[TEXT("WXK_MBUTTON")]=WXK_MBUTTON;
	wxKeyMap::keymap[TEXT("WXK_CLEAR")]=WXK_CLEAR;
	wxKeyMap::keymap[TEXT("WXK_SHIFT")]=WXK_SHIFT;
	wxKeyMap::keymap[TEXT("WXK_ALT")]=WXK_ALT;
	wxKeyMap::keymap[TEXT("WXK_CONTROL")]=WXK_CONTROL;
	wxKeyMap::keymap[TEXT("WXK_MENU")]=WXK_MENU;
	wxKeyMap::keymap[TEXT("WXK_PAUSE")]=WXK_PAUSE;
	wxKeyMap::keymap[TEXT("WXK_CAPITAL")]=WXK_CAPITAL;
	wxKeyMap::keymap[TEXT("WXK_PRIOR")]=WXK_PRIOR;
	wxKeyMap::keymap[TEXT("WXK_NEXT")]=WXK_NEXT;
	wxKeyMap::keymap[TEXT("WXK_END")]=WXK_END;
	wxKeyMap::keymap[TEXT("WXK_HOME")]=WXK_HOME;
	wxKeyMap::keymap[TEXT("WXK_LEFT")]=WXK_LEFT;
	wxKeyMap::keymap[TEXT("WXK_UP")]=WXK_UP;
	wxKeyMap::keymap[TEXT("WXK_RIGHT")]=WXK_RIGHT;
	wxKeyMap::keymap[TEXT("WXK_DOWN")]=WXK_DOWN;
	wxKeyMap::keymap[TEXT("WXK_SELECT")]=WXK_SELECT;
	wxKeyMap::keymap[TEXT("WXK_PRINT")]=WXK_PRINT;
	wxKeyMap::keymap[TEXT("WXK_EXECUTE")]=WXK_EXECUTE;
	wxKeyMap::keymap[TEXT("WXK_SNAPSHOT")]=WXK_SNAPSHOT;
	wxKeyMap::keymap[TEXT("WXK_INSERT")]=WXK_INSERT;
	wxKeyMap::keymap[TEXT("WXK_HELP")]=WXK_HELP;
	wxKeyMap::keymap[TEXT("WXK_NUMPAD0")]=WXK_NUMPAD0;
	wxKeyMap::keymap[TEXT("WXK_NUMPAD1")]=WXK_NUMPAD1;
	wxKeyMap::keymap[TEXT("WXK_NUMPAD2")]=WXK_NUMPAD2;
	wxKeyMap::keymap[TEXT("WXK_NUMPAD3")]=WXK_NUMPAD3;
	wxKeyMap::keymap[TEXT("WXK_NUMPAD4")]=WXK_NUMPAD4;
	wxKeyMap::keymap[TEXT("WXK_NUMPAD5")]=WXK_NUMPAD5;
	wxKeyMap::keymap[TEXT("WXK_NUMPAD6")]=WXK_NUMPAD6;
	wxKeyMap::keymap[TEXT("WXK_NUMPAD7")]=WXK_NUMPAD7;
	wxKeyMap::keymap[TEXT("WXK_NUMPAD8")]=WXK_NUMPAD8;
	wxKeyMap::keymap[TEXT("WXK_NUMPAD9")]=WXK_NUMPAD9;
	wxKeyMap::keymap[TEXT("WXK_MULTIPLY")]=WXK_MULTIPLY;
	wxKeyMap::keymap[TEXT("WXK_SEPARATOR")]=WXK_SEPARATOR;
	wxKeyMap::keymap[TEXT("WXK_SUBTRACT")]=WXK_SUBTRACT;
	wxKeyMap::keymap[TEXT("WXK_DECIMAL")]=WXK_DECIMAL;
	wxKeyMap::keymap[TEXT("WXK_F1")]=WXK_F1;
	wxKeyMap::keymap[TEXT("WXK_F2")]=WXK_F2;
	wxKeyMap::keymap[TEXT("WXK_F3")]=WXK_F3;
	wxKeyMap::keymap[TEXT("WXK_F4")]=WXK_F4;
	wxKeyMap::keymap[TEXT("WXK_F5")]=WXK_F5;
	wxKeyMap::keymap[TEXT("WXK_F6")]=WXK_F6;
	wxKeyMap::keymap[TEXT("WXK_F7")]=WXK_F7;
	wxKeyMap::keymap[TEXT("WXK_F8")]=WXK_F8;
	wxKeyMap::keymap[TEXT("WXK_F9")]=WXK_F9;
	wxKeyMap::keymap[TEXT("WXK_F10")]=WXK_F10;
	wxKeyMap::keymap[TEXT("WXK_F11")]=WXK_F11;
	wxKeyMap::keymap[TEXT("WXK_F12")]=WXK_F12;
	wxKeyMap::keymap[TEXT("WXK_F13")]=WXK_F13;
	wxKeyMap::keymap[TEXT("WXK_F14")]=WXK_F14;
	wxKeyMap::keymap[TEXT("WXK_F15")]=WXK_F15;
	wxKeyMap::keymap[TEXT("WXK_F16")]=WXK_F16;
	wxKeyMap::keymap[TEXT("WXK_F17")]=WXK_F17;
	wxKeyMap::keymap[TEXT("WXK_F18")]=WXK_F18;
	wxKeyMap::keymap[TEXT("WXK_F19")]=WXK_F19;
	wxKeyMap::keymap[TEXT("WXK_F20")]=WXK_F20;
	wxKeyMap::keymap[TEXT("WXK_F21")]=WXK_F21;
	wxKeyMap::keymap[TEXT("WXK_F22")]=WXK_F22;
	wxKeyMap::keymap[TEXT("WXK_F23")]=WXK_F23;
	wxKeyMap::keymap[TEXT("WXK_F24")]=WXK_F24;
	wxKeyMap::keymap[TEXT("WXK_NUMLOCK")]=WXK_NUMLOCK;
	wxKeyMap::keymap[TEXT("WXK_SCROLL")]=WXK_SCROLL;
	wxKeyMap::keymap[TEXT("WXK_PAGEUP")]=WXK_PAGEUP;
	wxKeyMap::keymap[TEXT("WXK_PAGEDOWN")]=WXK_PAGEDOWN;
	wxKeyMap::keymap[TEXT("WXK_NUMPAD_SPACE")]=WXK_NUMPAD_SPACE;
	wxKeyMap::keymap[TEXT("WXK_NUMPAD_TAB")]=WXK_NUMPAD_TAB;
	wxKeyMap::keymap[TEXT("WXK_NUMPAD_ENTER")]=WXK_NUMPAD_ENTER;
	wxKeyMap::keymap[TEXT("WXK_NUMPAD_F1")]=WXK_NUMPAD_F1;
	wxKeyMap::keymap[TEXT("WXK_NUMPAD_F2")]=WXK_NUMPAD_F2;
	wxKeyMap::keymap[TEXT("WXK_NUMPAD_F3")]=WXK_NUMPAD_F3;
	wxKeyMap::keymap[TEXT("WXK_NUMPAD_F4")]=WXK_NUMPAD_F4;
	wxKeyMap::keymap[TEXT("WXK_NUMPAD_HOME")]=WXK_NUMPAD_HOME;
	wxKeyMap::keymap[TEXT("WXK_NUMPAD_LEFT")]=WXK_NUMPAD_LEFT;
	wxKeyMap::keymap[TEXT("WXK_NUMPAD_UP")]=WXK_NUMPAD_UP;
	wxKeyMap::keymap[TEXT("WXK_NUMPAD_RIGHT")]=WXK_NUMPAD_RIGHT;
	wxKeyMap::keymap[TEXT("WXK_NUMPAD_DOWN")]=WXK_NUMPAD_DOWN;
	wxKeyMap::keymap[TEXT("WXK_NUMPAD_PRIOR")]=WXK_NUMPAD_PRIOR;
	wxKeyMap::keymap[TEXT("WXK_NUMPAD_PAGEUP")]=WXK_NUMPAD_PAGEUP;
	wxKeyMap::keymap[TEXT("WXK_NUMPAD_NEXT")]=WXK_NUMPAD_NEXT;
	wxKeyMap::keymap[TEXT("WXK_NUMPAD_PAGEDOWN")]=WXK_NUMPAD_PAGEDOWN;
	wxKeyMap::keymap[TEXT("WXK_NUMPAD_END")]=WXK_NUMPAD_END;
	wxKeyMap::keymap[TEXT("WXK_NUMPAD_BEGIN")]=WXK_NUMPAD_BEGIN;
	wxKeyMap::keymap[TEXT("WXK_NUMPAD_INSERT")]=WXK_NUMPAD_INSERT;
	wxKeyMap::keymap[TEXT("WXK_NUMPAD_DELETE")]=WXK_NUMPAD_DELETE;
	wxKeyMap::keymap[TEXT("WXK_NUMPAD_EQUAL")]=WXK_NUMPAD_EQUAL;
	wxKeyMap::keymap[TEXT("WXK_NUMPAD_MULTIPLY")]=WXK_NUMPAD_MULTIPLY;
	wxKeyMap::keymap[TEXT("WXK_NUMPAD_ADD")]=WXK_NUMPAD_ADD;
	wxKeyMap::keymap[TEXT("WXK_NUMPAD_SEPARATOR")]=WXK_NUMPAD_SEPARATOR;
	wxKeyMap::keymap[TEXT("WXK_NUMPAD_SUBTRACT")]=WXK_NUMPAD_SUBTRACT;
	wxKeyMap::keymap[TEXT("WXK_NUMPAD_DECIMAL")]=WXK_NUMPAD_DECIMAL;
	wxKeyMap::keymap[TEXT("WXK_NUMPAD_DIVIDE")]=WXK_NUMPAD_DIVIDE;
	wxKeyMap::keymap[TEXT("WXK_WINDOWS_LEFT")]=WXK_WINDOWS_LEFT;
	wxKeyMap::keymap[TEXT("WXK_WINDOWS_RIGHT")]=WXK_WINDOWS_RIGHT;
	wxKeyMap::keymap[TEXT("WXK_WINDOWS_MENU ")]=WXK_WINDOWS_MENU ;
	wxKeyMap::keymap[TEXT("WXK_COMMAND")]=WXK_COMMAND;
	wxKeyMap::keymap[TEXT("WXK_SPECIAL1")]=WXK_SPECIAL1;
	wxKeyMap::keymap[TEXT("WXK_SPECIAL2")]=WXK_SPECIAL2;
	wxKeyMap::keymap[TEXT("WXK_SPECIAL3")]=WXK_SPECIAL3;
	wxKeyMap::keymap[TEXT("WXK_SPECIAL4")]=WXK_SPECIAL4;
	wxKeyMap::keymap[TEXT("WXK_SPECIAL5")]=WXK_SPECIAL5;
	wxKeyMap::keymap[TEXT("WXK_SPECIAL6")]=WXK_SPECIAL6;
	wxKeyMap::keymap[TEXT("WXK_SPECIAL7")]=WXK_SPECIAL7;
	wxKeyMap::keymap[TEXT("WXK_SPECIAL8")]=WXK_SPECIAL8;
	wxKeyMap::keymap[TEXT("WXK_SPECIAL9")]=WXK_SPECIAL9;
	wxKeyMap::keymap[TEXT("WXK_SPECIAL10")]=WXK_SPECIAL10;
	wxKeyMap::keymap[TEXT("WXK_SPECIAL11")]=WXK_SPECIAL11;
	wxKeyMap::keymap[TEXT("WXK_SPECIAL12")]=WXK_SPECIAL12;
	wxKeyMap::keymap[TEXT("WXK_SPECIAL13")]=WXK_SPECIAL13;
	wxKeyMap::keymap[TEXT("WXK_SPECIAL14")]=WXK_SPECIAL14;
	wxKeyMap::keymap[TEXT("WXK_SPECIAL15")]=WXK_SPECIAL15;
	wxKeyMap::keymap[TEXT("WXK_SPECIAL16")]=WXK_SPECIAL16;
	wxKeyMap::keymap[TEXT("WXK_SPECIAL17")]=WXK_SPECIAL17;
	wxKeyMap::keymap[TEXT("WXK_SPECIAL18")]=WXK_SPECIAL18;
	wxKeyMap::keymap[TEXT("WXK_SPECIAL19")]=WXK_SPECIAL19;
	wxKeyMap::keymap[TEXT("WXK_SPECIAL20")]=WXK_SPECIAL20;
}
