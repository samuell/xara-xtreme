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
/*************************************************************************************
	
	
	Author: Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	
	Definition of the member function StringBase::MakeMsg(), which extends the traditional
	sprintf() function to cope with arbitrarily ordered format specifications.
**************************************************************************************/

#include "camtypes.h"
//#include "basestr.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "ensure.h" - in camtypes.h [AUTOMATICALLY REMOVED]


/**************************************************************************************
>	INT32 __cdecl StringBase::MakeMsg(UINT32 resourceID ...)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22nd April 1993
	Inputs:		A string resource identifier, and a variable number of other parameters,
				which must match the format specifiers embedded in the string resource.
				Note that floating point parameters are NOT supported.
	Outputs:	Changes "this" to become the formatted string, with the passed parameters
				substituted into the string resource in the correct order.
	Returns:	The number of characters in the formatted string.  This will be zero if
				a problem occurs, such as not being able to load the format string
				resource.
	Purpose:	Internationally portable version of sprintf(...), eg.
					StringBase s;
					INT32 x, y, z;
					s.MakeMsg(_R(IDS_COORDFORMAT), x, y, z);
					TextOut(hdc, 20, 20, s, s.Length());
	Errors:		ENSURE failure if called for a String that hasn't been allocated.
***************************************************************************************/
INT32 __cdecl StringBase::MakeMsg(UINT32 resID ...)
{
/*	if (IsUserName("JustinF"))
		TRACE( _T("MakeMsg called with resource ID %u\n"), resID);
*/
	ENSURE(text, "Call to StringBase::MakeMsg for unallocated String");

	va_list ap;
	va_start(ap, resID);
	
	INT32 n = 0;
	StringBase s;
	if (s.Alloc(MAX_STRING_RES_LENGTH) && s.Load(resID))
	{
		n = CCvsprintf(s.text, ap);
	}
#ifdef _DEBUG
/*	else
	{
		if (IsUserName("JustinF"))
			TRACE( _T("Failed to allocate or load the format string in MakeMsg()\n"));
	} */
#endif
	
	va_end(ap);
	
#if !defined(EXCLUDE_FROM_RALPH) && !defined(EXCLUDE_FROM_XARALX)
	// Remember the resource ID, in case the help system can't work out the message ID.
	SetNextMsgHelpContext(resID);
#endif
	
	// Return the number of characters in the formatted string.
	return n;
}


	
	
/**************************************************************************************
>	INT32 __cdecl StringBase::_MakeMsg(const TCHAR* fmt ...)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22nd April 1993
	Inputs:		A constant character pointer, and a variable number of other parameters,
				which must match the format specifiers embedded in the character pointer.
				**** Note that floating point parameters are NOT supported ****
	Outputs:	Changes "this" to become the formatted string, with the passed parameters
				substituted into the string resource in the correct order.
	Returns:	The number of characters in the formatted string.
	Purpose:	Internationally portable version of sprintf(...), eg.					
					UINT32 x;
					s._MakeMsg("#1%lu", x);
					TextOut(hdc, 20, 20, s, s.Length());
***************************************************************************************/
INT32 __cdecl StringBase::_MakeMsg(const TCHAR* fmt ...)
{
/*	if (IsUserName("JustinF"))
		TRACE( _T("_MakeMsg called with format string %s\n"), fmt);
*/
	ENSURE(text, "Call to StringBase::_MakeMsg for an unallocated String");
	ENSURE(fmt, "Call to StringBase::_MakeMsg with a null format parameter");
	
	va_list ap;
	va_start(ap, fmt);
	
	INT32 n = CCvsprintf(fmt, ap);
	
	va_end(ap);
	return n;	
}



/////////////////////////////////////////////////////////////////////////////////////////
//
// I M P L E M E N T A T I O N
//
// The types that can appear in a formatting string.  These have been documented,
// but it's worth pointing out that the wsprintf(...) function will accept other
// %specifiers, eg. %x, which are not documented.

enum ArgType
{
	LITERAL, CHAR_ARG, SIGNED_INT_ARG, SIGNED_INT32_ARG, UNSIGNED_INT_ARG,
	UNSIGNED_INT32_ARG, CHAR_POINTER_ARG, STRING_POINTER_ARG
};



// Private helper data structure - a linked list of strings & associated
// type information.
struct Item
{
	TCHAR* str;			// pointer to literal / format specifier
	ArgType type;		// type of associated parameter
	INT32 pos;			// relative position in format string
	Item* next;			// next in list
		
	static Item* head;
	static Item* tail;
	
	Item(const TCHAR* s, INT32 ps, ArgType t);
	~Item();
};
	
	
Item* Item::head = 0;
Item* Item::tail = 0;


// Create & join an Item.
Item::Item(const TCHAR* s, INT32 ps, ArgType t)
{
	camStrcpy(str = new TCHAR[camStrlen(s) + 1], s);
	pos = ps;
	type = t;
	next = 0;
	if (tail) tail = tail->next = this;
		else head = tail = this;
}


	
	
// Destroy an Item - tail recursively deletes every Item, if called by
// delete Item::head;
Item::~Item()
{
	delete[] str;
	if (next) delete next;
		else head = tail = 0;
}


	
	
/**************************************************************************************
>	INT32 StringBase::BuildList(const TCHAR* format)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22nd April 1993
	Inputs:		A pointer to the format string to be analysed.
	Returns:	The number of format specifiers in the string, which should be the same
				as the number of arguments to be formatted.
	Purpose:	This private helper function scans the layout string, breaking it up into
				literal text and parameters, which are placed on an private list in the
				order they are encountered, together with their type and which argument
				passed to Format that they refer to.  The formatting must be done in two
				passes as the order of the parameters is not known until the whole format
				string has been scanned.
	Scope:		Private
***************************************************************************************/
INT32 StringBase::BuildList(const TCHAR* format)
{
/*	if (IsUserName("JustinF"))
		TRACE( _T("\tBuildList called with format string %s\n"), format);
*/
	ENSURE(format, "Null parameter passed to StringBase::BuildList");
    const TCHAR* start = format;
    INT32 nArgs = 0;    
    while (*start)
    {
    	const TCHAR* next = start;    	
    	// Look for a format specifier in the layout string
    	if (*next == TEXT('#'))
    		if (IsNumeric(*(++next)))
    		{
    			INT32 ArgPos = *next - TEXT('0');		// MS approved method - yuck!!!
				ENSURE(ArgPos >= 1 && ArgPos <= 9,
						"Illegal format string passed to MakeMsg!");

    			if (*(++next) == TEXT('%'))
    			{
    				// Seem to have found the beginning of a specifier,
    				// so try parsing it, to extract the type information
    				if (*(++next) == TEXT('-')) next++;
    				if (*next == TEXT('#')) next++;
    				if (*next == TEXT('0')) next++;
    				while (IsNumeric(*next)) next++;
    				if (*next == TEXT('.'))
    					while (IsNumeric(*++next));
    				
    				// Ok, we have skipped the width, precision etc. and
					// "next" should now be pointing at the type character(s)
					ArgType kind;
					if (*next == TEXT('l'))		// an INT32 something or another?
						switch (*++next)
						{
							case TEXT('d'):  case TEXT('i'):
								kind = SIGNED_INT32_ARG;
								break;
							case TEXT('u'):  case TEXT('x'):  case TEXT('X'):
								kind = UNSIGNED_INT32_ARG;
								break;
							default:			// false alarm, can't be a specifier
								next = start + 1;
								goto not_format;
						}
					else
						switch (*next)			// single type character
						{    
							case TEXT('c'):
								kind = CHAR_ARG;
								break;
							case TEXT('d'):  case TEXT('i'):
								kind = SIGNED_INT_ARG;
								break;
							case TEXT('u'): case TEXT('x'): case TEXT('X'):
								kind = UNSIGNED_INT_ARG;
								break;
							case TEXT('s'):
								kind = CHAR_POINTER_ARG;
								break;
							case TEXT('S'):
								kind = STRING_POINTER_ARG;								
								break;
							default:
								next = start + 1;
								goto not_format;			// boo hiss hooray!
						}
					
					// Successfully parsed the format specifier, so add
					// it to the Item list.  Skip the leading #n of the specifier,
					// so (next) now points to the character following the '%X'
					// format specifier.
					next++;

					// Extract the specifier.
					TCHAR temp[64];
					INT32 len = next - start - 2;
					camStrncpy(temp, start + 2, len);
					temp[len] = 0;
					new Item(temp, ArgPos, kind);
					start = next;
					nArgs++;
					
/*					if (IsUserName("JustinF"))
					{
						TRACE( _T("\tItem: (%d) at #%d  %s\n"), kind, ArgPos, temp);
						TRACE( _T("\t\tRemaining: %s\n"), start);
					}
*/
					continue;
    			}
    			else
    				// Put back the previous character
    				next--;
    		}
    	
not_format:    	
    	// Scan a literal up to the next '#' or null, whichever is sooner
    	while (*next && *next != TEXT('#')) next++;

		TCHAR temp[256];
		INT32 len = next - start;

		camStrncpy(temp, start, len);
		temp[len] = 0;
		new Item(temp, 0, LITERAL);
		start = next;

/*		if (IsUserName("JustinF"))
		{
			TRACE( _T("\tLiteral: %s\n"), temp);
		}
*/
    }

	return nArgs;    	
}


	

/**************************************************************************************
>	INT32 StringBase::CCvsprintf(const TCHAR* layout, va_list va)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22nd April 1993
	Inputs:		A pointer to the format string, a variable-parameter pointer to the
				arguments to be formatted.
	Returns:	The length of the StringBase once the arguments have been substituted into
				it.
	Purpose:	This private helper function takes the list generated by
				StringBase::BuildList and scans through it, formatting stack-frame
				parameters according to their list entry.  This produces another list,
				this time of formatted parameters, which is then concatenated to
				produce the final output.
	Scope:		Private
***************************************************************************************/
INT32 StringBase::CCvsprintf(const TCHAR* layout, va_list va)
{
/*	if (IsUserName("JustinF"))
		TRACE( _T("CCvsprintf called with format string %s\n"), layout);
*/
	INT32 n = BuildList(layout);
	for (INT32 i = 1; i <= n; i++)
	{
		for (Item* p = Item::head; p; p = p->next)
			if (p->pos != i)
				continue;
			else
			{  	// Found specifier i in the list, so grab from the stack
				// and call wsprintf() to convert to text.  The converted
				// text is put in the temp[] buffer - note the enormous
				// size of this, in accordance with the C standard.
				TCHAR temp[512];
				switch (p->type)
				{
					// type 'c' - single character
					case CHAR_ARG:
#if defined(__WXMSW__)
						camSnprintf( temp, 512, p->str, va_arg(va, TCHAR) );
#else					// TCHARs are promoted to INT32 when passed via ... under GCC
						camSnprintf( temp, 512, p->str, va_arg(va, INT32) );
#endif
						break;
					// type 'd' / 'i' - signed decimal integer
					case SIGNED_INT_ARG:
						camSnprintf(temp, 512, p->str, va_arg(va, INT32));
						break;
					// type 'ld' / 'li' - signed decimal INT32
					case SIGNED_INT32_ARG:
						camSnprintf(temp, 512, p->str, va_arg(va, INT32));
						break;
					// type 'u' - unsigned decimal integer
					case UNSIGNED_INT_ARG:
						camSnprintf(temp, 512, p->str, va_arg(va, UINT32));
						break;
					// type 'lu' / 'lx' / 'lX' - unsigned decimal INT32
					case UNSIGNED_INT32_ARG:
						camSnprintf(temp, 512, p->str, va_arg(va, UINT32));
						break;
					// type 's' - long pointer to array of (constant) char
					case CHAR_POINTER_ARG:
						camSnprintf(temp, 512, p->str, va_arg(va, LPCTSTR));
						break;
					// type 'S' - a pointer to a StringBase.  First change the
					// %S format specifier, which is our own, into a sprintf()
					// compatible %s.  Note that the 'S' is always the last
					// character of the format specifier.
					case STRING_POINTER_ARG:
						(p->str)[camStrlen(p->str) - 1] = TEXT('s');
						camSnprintf(temp, 512, p->str, LPCTSTR(va_arg(va, const StringBase*)->text));
						break;
					default:
						break;
				}
				// Replace the format specifier with the formatted text
				delete[] p->str;
				camStrcpy(p->str = new TCHAR[camStrlen(temp) + 1], temp);
				break;
			}
	}
	
	// Concatenate every string in the list, tidy up, and return in "this" string
	*text = 0;
	UINT32 NextChar = 0;
	INT32 Index = 0;
	for (Item* p = Item::head; (p != NULL && NextChar < length) ; p = p->next)
	{
/*		if (camStrlen(text) + camStrlen(p->str) < length)
			camStrcat(text, p->str);
		else
		{
			ENSURE(FALSE, "Call to String::MakeMsg will overflow - text has been truncated");
		}
*/
		for (Index = 0; p->str[Index] != 0 && NextChar < length; Index++)
			text[NextChar++] = p->str[Index];
	}

	if (NextChar > length)
		NextChar = length;
	text[NextChar] = TCHAR('\0');

	delete Item::head;
	return camStrlen(text);
}
