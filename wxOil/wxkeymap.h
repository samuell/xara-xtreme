// $Id: wxOil/wxkeymap.h, 1, 01-Jan-2006, Anonymous $
#ifndef WXOIL_WXKEYMAP
#define WXOIL_WXKEYMAP

#include "camtypes.h"
#include <map>
#include "fixstr32.h"

/********************************************************************************************

	class: wxKeyMap
	Author: Jason_Williams (Xara Group Ltd) <camelotdev@xara.com> M'Sadoques
	Created: 28/05/2007
	Purpose: Maps the string representation of a wx key (i.e. "WXK_F1"), into it's enum value.

********************************************************************************************/
class wxKeyMap
{
	static std::map<String_32, UINT32> keymap;
	static void GenerateMap();
	public:
		static UINT32 GetKeyVal(String_32);
};

#endif
