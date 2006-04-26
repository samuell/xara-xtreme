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
/*******************************************************************************************
	
 ****************************************************************************************/ 

/*******************************************************************************************
>	class StringBase : public CCObject

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	
	Created:	22nd April 1993; ammended 6th August 1993
	
	Purpose:	General-purpose string class.  Overloads the C++ arithmetic operator +=
				which for strings perform concatenation, together with the
				comparison operators <, >,	!=, ==, etc, which will perform
				case-sensitive comparisons	according to the local custom.
	
				Other functions provided include Left(), Mid(), and Right(), for
				decomposing strings, plus Sub(), which returns the position of a string
				within another string.  A new sprintf()-type member function has been
				provided, MakeMsg(), which performs internationally portable argument
				substitution according to a formatting string read from the application's
				resources.
				
				The primary use of this class is to serve as a base for the derivation
				of fixed length StringBases.  Derived String_N's, unlike their StringBase
				base classes, do not allocate a buffer for text on the heap, but within
				their class instance.
				
				Note that these kind of base-class strings CAN be of an arbitrary length,
				but the user must call Alloc() to change the length - all member functions
				assume that the StringBase is long enough to cope with the operation of the
				function.  Attempts to access past the end of the string will be trapped
				with ASSERTions.
	
	Errors:		Most functions validate their parameters, eg. asking Left() to extract
				the leftmost 0 characters of a string will cause the function to return
				a 0 string.
	
	Notes:		As of 4th May 1993, all serialisation / archive support has been commented
				out, as this does not work and nobody seems to know why.  This will be
				rectified when serialisation is needed (some time in the future).
********************************************************************************************/				


#ifndef INC_STRINGBASE
#define INC_STRINGBASE

#include "ccobject.h"
#include "errors.h"
// #include <stdarg.h>

// Maximum number of characters in a string resource.
const size_t MAX_STRING_RES_LENGTH = 255;
const INT32 str_MAXEXCEPTIONS = 30;

class CCAPI StringBase : public CCObject
{
	// Error class needs direct access
	friend class Error;
	
public:
	// Constructors & destructor
	StringBase();
	virtual ~StringBase();
	
	// Utility functions
	virtual BOOL Alloc(INT32 size);				// get heap space for string
	BOOL Load(UINT32 resID, UINT32 inst = 0);		// Load StringBase from resources
	
	// Attributes.
	INT32 Length() const;                   		// returns no. of characters
	INT32 MaxLength() const;						// returns maximum allowed length
	BOOL IsEmpty() const;						// returns TRUE if string is zero-length
	
	INT32 NextIndex(INT32 index) const;				// given a char index, return the next (-1 if none)
	INT32 PrevIndex(INT32 index) const;				// given a char index, return the prev (-1 if none)
	BOOL IsLegal(INT32 index) const;				// is the char index legal?
	BOOL IsIllegal(INT32 index) const;			// is the char index illegal?
	
	static BOOL IsAlpha(TCHAR c);				// tests a Unicode character.
	static BOOL IsAlphaNumeric(TCHAR c);
	static BOOL IsNumeric(TCHAR c);
	static BOOL IsLower(TCHAR c);
	static BOOL IsUpper(TCHAR c);
	static BOOL IsSpace(TCHAR c);
	
	// Operations.
	StringBase& Empty();						// make this a 0 string
	StringBase& toUpper();						// convert string to upper case
	StringBase& toLower();						// convert to lower case
	StringBase& toTitle();						// Graham 30/10/97: Converts first character to upper, all others to lower

	INT32 __cdecl MakeMsg(UINT32 resID ...);		// Like sprintf(...)
	INT32 __cdecl _MakeMsg(const TCHAR* fmt ...);	// Like the above without resources

	
	// Casting operators
	operator TCHAR*();                          // convert to a standard C (Unicode) string
	operator const TCHAR*() const;              // same, but for constant instances

	operator wxString();
	operator const wxString() const;

	// Assignment & concatenation operators
	StringBase& operator+=(const StringBase& rhs);
	StringBase& operator+=(const TCHAR* s);
	StringBase& operator+=(const TCHAR ch);
	StringBase& operator=(const StringBase& other);     
	StringBase& operator=(const TCHAR* s);			
	StringBase& operator=(const TCHAR ch);

	// non memory toasting += support function
	static TCHAR* SafeCat(TCHAR* pstr1, const TCHAR* pcstr2, UINT32 nchMax);

	// Functions to turn on exceptions in 'some' string functions
	static BOOL EnableExceptionHandler(BOOL PushNewHandler=TRUE);
	static BOOL DisableExceptionHandler(BOOL PopHandler=TRUE);
	static BOOL WillThrowException();

	// Character/substring manipulation functions
	const StringBase& Left(StringBase* out, UINT32 count) const;
	const StringBase& Mid(StringBase* out, UINT32 first, UINT32 count) const;
	const StringBase& Right(StringBase* out, UINT32 count) const;

	void Split(StringBase* pstrStart, StringBase* pstrEnd, INT32 iSplit, BOOL fIncludeSplitChar);
	INT32 Sub(const StringBase& mask, UINT32 from = 0, TCHAR fuzz = 0) const;
	INT32 SubWithoutCase(const StringBase& strToFind) const;
	void Remove(INT32 Position, INT32 Number);
	void Insert( const StringBase &InsertString, INT32 Position);
	void SwapChar(const TCHAR schar, const TCHAR rchar);
	INT32 CountChar(const TCHAR tchar);
	TCHAR operator[](const INT32 Index);
	const WCHAR CharAt(const INT32 Index) const;
	
	INT32 SkipComma(INT32 pos=0) const;
	INT32 SkipSpace(INT32 pos=0) const;
	INT32 FindNextChar(INT32 position=0) const;
	INT32 FindNextChar(TCHAR c, INT32 position=0) const;
	INT32 ConvertToInteger(INT32& position) const;

	//Graham 6/5/97
	INT32 ReverseFind(TCHAR cToFind) const;

#if !defined(EXCLUDE_FROM_XARLIB)
	// Special internationalisation stuff...
	virtual void MakePercent(INT32 Value);
	virtual void MakePercent(double Value);
	virtual void MakePercent(TCHAR *Value);
	virtual void FixFormat();
#endif

	// Comparison operators.
	bool IsIdentical(const TCHAR *other) const;
	bool IsIdentical(const StringBase &other) const;
		// Fast way of determining if strings are exactly equal

	INT32 CompareTo(const TCHAR *other, BOOL CaseSensitive = TRUE) const;
	INT32 CompareTo(const StringBase &other, BOOL CaseSensitive = TRUE) const;
		// Slower comparison - returns the locale-defined relative ordering of the 2 strings

	friend bool operator==(const StringBase& x, const TCHAR* y);
	friend bool operator==(const StringBase& x, const StringBase& y);
	friend bool operator<(const StringBase& x, const TCHAR* y);		
	friend bool operator<(const StringBase& x, const StringBase& y);
	friend bool operator>(const StringBase& x, const TCHAR* y);
	friend bool operator>(const StringBase& x, const StringBase& y);
	friend bool operator!=(const StringBase& x, const TCHAR* y);
	friend bool operator!=(const StringBase& x, const StringBase& y);
	friend bool operator<=(const StringBase& x, const TCHAR* y);
	friend bool operator<=(const StringBase& x, const StringBase& y);
	friend bool operator>=(const StringBase& x, const TCHAR* y);
	friend bool operator>=(const StringBase& x, const StringBase& y);

	// I/O (non-persistent) stream functions
	friend std::istream& operator>>(std::istream& is, StringBase& s);
	friend std::ostream& operator<<(std::ostream& os, const StringBase& s);

	// Overridden CCObject functions
#ifdef _DEBUG
	virtual void AssertValid() const;			// checks if StringBase is valid
PORTNOTE("other","Removed MFC CDumpContext")
#ifndef EXCLUDE_FROM_XARALX
	virtual void Dump(CDumpContext& dc) const;	// dumps to debug terminal
#endif
#endif

private:
	INT32  IllegalIndex() const;			// return an illegal index for this string
	static INT32 TCharToNum(TCHAR c);			// convert a digit char to a number (0..9)

	static void ThrowStringException();
	static BOOL ThrowExceptionStack[str_MAXEXCEPTIONS];
	static INT32 CurrEx;

	INT32 BuildList(const TCHAR*);                // break format string into parts
	INT32 CCvsprintf(const TCHAR*, va_list);		// substitute parameters
	StringBase(const StringBase& other);		// StringBase x(StringBase ...)

protected:
	// Member data
	TCHAR* text;
	UINT32 length;
};


INT32 CCAPI SmartLoadString(UINT32 modID, UINT32 resID, TCHAR *buf, INT32 size);

// This is our very own version of the Windows string function camStrncpy(), which
// was mysteriously dropped from the Win32 API.  Using the standard C library function
// strncpy() doesn't give quite the same results, so we must provide our own.
TCHAR* cc_camStrncpy(TCHAR* dest, const TCHAR* src, size_t n);
const TCHAR* cc_lstrstr(const TCHAR* String1, const TCHAR *String2);
TCHAR* cc_lstristr(TCHAR* String1, TCHAR* String2);
TCHAR* cc_lstrchr(TCHAR* Src, TCHAR c);
TCHAR* cc_lstrichr(TCHAR* String1, TCHAR c);
TCHAR* cc_lstrrchr(TCHAR* Src, TCHAR c);
TCHAR* cc_lstrtok(TCHAR* String1, TCHAR* String2);
INT32 cc_lstrncmp(TCHAR* String1, TCHAR* String2, INT32 Count);
size_t cc_strlenCharacters( const TCHAR* string );
size_t cc_strlenBytes( const TCHAR* string );



/**************************************************************************************
>	StringBase::StringBase()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22nd April 1993
	Inputs:		-
	Purpose:	Default constructor for a StringBase.  Sets the character buffer to 0
				and the string length to zero.  The user must call Alloc() before
				trying to use the StringBase.
	Scope:		Public
***************************************************************************************/
inline StringBase::StringBase()
  : text(0), length(0)
{
	// Empty.
}


	
	
/**************************************************************************************
>	StringBase::StringBase(const StringBase&)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22nd April 1993
	Inputs:		Another StringBase object.
	Purpose:	Copy constructor - this is made private (and unimplemented) as it
				will not work for base class strings, which must call Alloc() before
				they are used.
	Scope:		Private
***************************************************************************************/
inline StringBase::StringBase(const StringBase&)
{
	// Empty.
}


	
	
/**************************************************************************************
>	virtual StringBase::~StringBase()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22nd April 1993
	Purpose:	Destroys & cleans up a string.
***************************************************************************************/
inline StringBase::~StringBase()
{
	if (text) delete[] text;
}



/**************************************************************************************
>	INT32 StringBase::Length() const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22nd April 1993
	Returns:	The number of characters in the string, excluding any terminating 0.
	Purpose:	Calculates the length of the string.
***************************************************************************************/
inline INT32 StringBase::Length() const
{
	ERROR3IF(!text, "Attempt to find the LENGTH of an unALLOCated String");
	return (text) ? camStrlen( text ) : 0;
}

	
	
/**************************************************************************************
>	INT32 StringBase::MaxLength() const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25th August 1993
	Returns:	The maximum number of characters the string can contain.
	Purpose:	Calculates the longest allowed string that can be stored in this.
***************************************************************************************/
inline INT32 StringBase::MaxLength() const
{
#ifdef _UNICODE
	return length - 1;
#else
	return (length / 2) - 1;		// MBCS - two chars per character
#endif
}

	
	
/**************************************************************************************
>	BOOL StringBase::IsEmpty() const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25th August 1993
	Returns:	TRUE if the string has no length.
	Purpose:	Tests for the 0 string.
***************************************************************************************/
inline BOOL StringBase::IsEmpty() const
{
	return text == 0 || *text == 0;
}



/**************************************************************************************

>	BOOL StringBase::IsIdentical(const StringBase &other) const

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/4/96

	Inputs:		other - A string against which this one will be compared

	Returns:	TRUE if the strings are identical
				FALSE if the strings are different in any way

	Purpose:	Compare this StringBase object to another string.

	Errors:		If either of the strings is 0, an ESNURE failure is generated

	Notes:		We define different versions of the function for UNICODE builds,
				as camStrcmp is very slow compared to vanilla strcmp.

				Note also that this is a very fast comparison compared to CompareTo.
				It should be used in preference where relative ordering of strings
				does not need to be determined.

	SeeAlso:	StringBase::CompareTo

**************************************************************************************/

inline bool StringBase::IsIdentical(const StringBase &other) const
{
	ERROR3IF(text == 0 || other.text == 0, "StringBase::IsIdentical: not ALLOCed");
	return wxStrcmp(text, other.text) == 0;
}



/**************************************************************************************

>	INT32 StringBase::IsIdentical(const TCHAR *other) const

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/4/96

	Inputs:		other - A string against which this one will be compared

	Returns:	TRUE if the strings are identical
				FALSE if the strings are different in any way

	Purpose:	Compare this StringBase object to another string.

	Errors:		If either of the strings is 0, an ESNURE failure is generated

	Notes:		We define different versions of the function for UNICODE builds,
				as camStrcmp is very slow compared to vanilla strcmp.

				Note also that this is a very fast comparison compared to CompareTo.
				It should be used in preference where relative ordering of strings
				does not need to be determined.

	SeeAlso:	StringBase::CompareTo

**************************************************************************************/

inline bool StringBase::IsIdentical(const TCHAR* other) const
{
	ERROR3IF(text == 0 || other == 0, "StringBase::IsIdentical: null params");
	return wxStrcmp( text, other ) == 0;
}



/**************************************************************************************

>	inline INT32 StringBase::NextIndex(INT32 index)	const

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/07/96
	Inputs:		index = the current character index
	Returns:	the next characater index after 'index'
				-1 if there is no next index, OR an exception is thrown
	Purpose:	Find the next character index
	Notes:		If string exceptions are enabled, an exception will be thrown if the
				index goes out of range.

***************************************************************************************/

inline INT32 StringBase::NextIndex(INT32 index)	const
{
	INT32 i = index + 1;
	if (IsLegal(i)) return i;
	TRACEUSER( "Mike", wxT("String index overflow in StringBase::NextIndex()") );
	return IllegalIndex();
}



/**************************************************************************************

>	inline INT32 StringBase::PrevIndex(INT32 index) const

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/07/96
	Inputs:		index = the current character index
	Returns:	the previous characater index before 'index'
				-1 if there is no previous index, OR an exception is thrown
	Purpose:	Find the previous character index
	Notes:		If string exceptions are enabled, an exception will be thrown if the
				index goes out of range.

***************************************************************************************/

inline INT32 StringBase::PrevIndex(INT32 index) const
{
	INT32 i = index-1;
	if (IsLegal(i)) return i;

	TRACEUSER( "Mike", wxT("String index underflow in StringBase::PrevIndex()") );
	return IllegalIndex();
}



/**************************************************************************************

>	inline INT32 StringBase::IllegalIndex() const

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/07/96
	Inputs:		-
	Returns:	An illegal index ie	-1 if exceptions are disabled, otherwise
				an exception is thrown
	Purpose:	Generate an illegal index
	Notes:		** Can potentially throw a user exception **

***************************************************************************************/

inline INT32 StringBase::IllegalIndex() const
{
	StringBase::ThrowStringException();
	return -1;
}



/**************************************************************************************

>	inline BOOL StringBase::IsIllegal(INT32 index) const

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/07/96
	Inputs:		index, a character index in this string
	Returns:	TRUE if index is illegal 
				FALSE if not
	Purpose:	Check index for its legality

***************************************************************************************/

inline BOOL StringBase::IsIllegal(INT32 index) const
{
	return ((index<0) || index>(Length()));
}



/**************************************************************************************

>	inline BOOL StringBase::IsLegal(INT32 index) const

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/07/96
	Inputs:		index, a character index in this string
	Returns:	TRUE if index is llegal 
				FALSE if its illegal
	Purpose:	Check index for its legality

***************************************************************************************/

inline BOOL StringBase::IsLegal(INT32 index) const
{
	return !IsIllegal(index);
}



/**************************************************************************************
>	static BOOL StringBase::IsAlpha(TCHAR c)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10th August 1993
	Inputs:		A Unicode character.
	Returns:	TRUE if the passed character is an alphabet letter.
	Purpose:	Tests if the character is a-z, A-Z (or local equivalent).
	
***************************************************************************************/
inline BOOL StringBase::IsAlpha(TCHAR c)
{
	return camIsalpha(c); // IsCharAlpha(c);
}



/**************************************************************************************
>	static BOOL StringBase::IsAlphaNumeric(TCHAR c)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10th August 1993
	Inputs:		A Unicode character
	Returns:	TRUE if the passed character is an alphabet letter or a digit.
	Purpose:	Tests if the character is a-z, A-Z, 0-9, or local equivalent.
***************************************************************************************/
inline BOOL StringBase::IsAlphaNumeric(TCHAR c)
{
	return camIsalnum(c);	//IsCharAlphaNumeric(c);
}



/**************************************************************************************
>	static BOOL StringBase::IsNumeric(TCHAR c)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10th August 1993
	Inputs:		A Unicode character
	Returns:	TRUE if the passed character is a digit.
	Purpose:	Tests if the character is 0-9, or local equivalent.
***************************************************************************************/
inline BOOL StringBase::IsNumeric(TCHAR c)
{
	return camIsalnum(c) && !camIsalpha(c);
}



/**************************************************************************************
>	static BOOL StringBase::IsLower(TCHAR c)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10th August 1993
	Inputs:		A Unicode character
	Returns:	TRUE if the passed character is a lower-case letter.
	Purpose:	Tests if the character is a-z (or local equivalent).
***************************************************************************************/
inline BOOL StringBase::IsLower(TCHAR c)
{
	return camIslower(c); // IsCharLower(c)
}



/**************************************************************************************
>	static BOOL StringBase::IsUpper(TCHAR c)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10th August 1993
	Inputs:		A Unicode character
	Returns:	TRUE if the passed character is an upper-case letter.
	Purpose:	Tests if the character is A-Z, or local equivalent.
***************************************************************************************/
inline BOOL StringBase::IsUpper(TCHAR c)
{
	return camIsupper(c); // IsCharUpper(c)
}



/**************************************************************************************
>	static BOOL StringBase::IsSpace(TCHAR c)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10th August 1993
	Inputs:		A Unicode character
	Returns:	TRUE if the passed character is whitespace.
***************************************************************************************/
inline BOOL StringBase::IsSpace(TCHAR c)
{
	return camIsspace(c);
}



/**************************************************************************************
>	StringBase& StringBase::Empty()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10th August 1993
	Purpose:	Sets a string to the 0 string (zero-terminated but no characters,
				NOT A 0 POINTER!)
***************************************************************************************/
inline StringBase& StringBase::Empty()
{
	ERROR3IF(!text, "Attempt to EMPTY an unALLOCated String");
	if (text) *text = 0;
	return *this;
}
	
	

/**************************************************************************************
>	StringBase& StringBase::toUpper()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22nd April 1993
	Purpose:	Converts all lower-case letters in the string to upper-case.
***************************************************************************************/
inline StringBase& StringBase::toUpper()
{
	ERROR3IF(!text, "Call to TOUPPER for an unALLOCated String");
   	if (text) CharUpper(text);							// get Windows API to convert
	return *this;
}

/**************************************************************************************
>	StringBase& StringBase::toLower()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22nd April 1993
	Purpose:	Converts all upper-case letters to lower-case.
***************************************************************************************/
inline StringBase& StringBase::toLower()
{
	ERROR3IF(!text, "Call to TOLOWER for an unALLOCated String");
	if (text) CharLower(text);
	return *this;
}


	
	
/**************************************************************************************
>	StringBase::operator TCHAR*()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22nd April 1993
	Returns:	Not applicable!
	Purpose:	Cast a string into a C-style pointer to array of characters.  If you
				intend to use the subscript operator [] with the results of the cast
				remember that no range checking is performed.
***************************************************************************************/
inline StringBase::operator TCHAR*()
{
	return text;
}


	
	
/**************************************************************************************
>	StringBase::operator const TCHAR*() const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22nd April 1993
	Returns:	N/A
	Purpose:	Cast a string into a C-style pointer to array of constant characters.
	SeeAlso:	StringBase::operator TCHAR*()
***************************************************************************************/
inline StringBase::operator const TCHAR*() const
{
	return text;
}


	
	
/**************************************************************************************
>	StringBase::operator wxString()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28th January 2006
	Returns:	Not applicable!
	Purpose:	Cast a string into a wxString
***************************************************************************************/
inline StringBase::operator wxString()
{
	return wxString(text);
}


	
	
/**************************************************************************************
>	StringBase::operator const wxString&() const

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28th January 2006
	Returns:	N/A
	Purpose:	Cast a string into a wxString
	SeeAlso:	StringBase::operator TCHAR*()
***************************************************************************************/
inline StringBase::operator const wxString() const
{
	return wxString(text);
}


	
	
/**************************************************************************************
>	StringBase& StringBase::operator+=(const StringBase& rhs)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22nd April 1993
	Inputs:		The string to add to the end of this string.
	Returns:	A reference to the lhs, ie. this StringBase as an lvalue.
	Purpose:	Append string y onto the end of string x, eg.
					x = TEXT("hello");
					cout << (x += TEXT(" world!")) << endl;
***************************************************************************************/
inline StringBase& StringBase::operator+=(const StringBase& rhs)
{
	ERROR3IF(!text || !rhs.text, "Call to String::operator+= for an unALLOCated String");
/*	ERROR3IF(camStrlen(text) + camStrlen(rhs.text) >= length,
			"Call to String::operator+= will result in overflow");*/

	if (text && rhs.text) SafeCat(text, rhs.text, length-1);
	return *this;
}



/**************************************************************************************
>   StringBase& StringBase::operator=(const TCHAR ch)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/12/1995
	Inputs:		ch - a single character
	Returns:	A reference to this.
	Purpose:	Assigns the single character to this string
***************************************************************************************/
inline StringBase& StringBase::operator=(const TCHAR ch)
{
	ERROR3IF(text == 0, "Call to String::operator= for an unALLOCated String");
	text[0] = ch;
	text[1] = 0;
	return *this;
}




/**************************************************************************************
>	TCHAR StringBase::operator[](const INT32 Index);

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22nd November 1995
	Returns		The character at offset Index into the string... Or 0 if there were
				problems (or if the character was 0 of course).
	Purpose:	Extract single characters from the string...
				0 is the first char, String.Length() is the terminating zero...
**************************************************************************************/
inline TCHAR StringBase::operator[](const INT32 Index)
{
	ERROR3IF(!text, "Call to String::operator[] for an unALLOCated String");
	return (text && (Index <= (INT32)camStrlen(text))) ? text[Index] : 0;
}



/**************************************************************************************
>	const WCHAR StringBase::CharAt(const INT32 Index);

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22nd November 1995
	Returns		The character at offset Index into the string... Or 0 if there were
				problems (or if the character was 0 of course).
	Purpose:	Extract single characters from the string...
				0 is the first char, String.Length() is the terminating zero...
**************************************************************************************/
inline const WCHAR StringBase::CharAt(const INT32 Index) const
{
#ifdef UNICODE
	ERROR3IF(!text, "Call to String::operator[] for an unALLOCated String");
	TCHAR ch = (text && (Index <= (INT32)camStrlen(text))) ? text[Index] : 0;

	return (WCHAR)ch;

#else
	PORTNOTETRACE("other", "WCHAR StringBase::CharAt does nothing in non-UNICODE builds!");
	ERROR3("This function is unimplemented in non-UNICODE builds!");
	return (WCHAR)0;
#endif
}



/**************************************************************************************
>   INT32 operator==(const StringBase& lhs, const StringBase& rhs)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>	(Jason)
	Created:	22nd April 1993 (24/5/94) (23/4/96)
	Returns		Return 0 (FALSE) or 1 (TRUE) according to the relative lexical order of
				two strings.
	Purpose:	Comparison (logical) operators, eg.
					x = "alpha";
					y = "beta";
					cout << "First is " << (x < y ? x : y) << endl;
				These functions will work according to local custom.

	Notes:		Now uses IsIdentical to compare the strings, which is 12 times faster
				than the new internationalised CompareTo.
**************************************************************************************/

inline bool operator==(const StringBase& x, const TCHAR* y)
{
	return x.IsIdentical(y);
}


inline bool operator==(const StringBase& x, const StringBase& y)
{
	return x.IsIdentical(y);
}

	
	
/**************************************************************************************
>	INT32 operator!=(const StringBase& lhs, const StringBase& rhs)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com> (Jason)
	Created:	22nd April 1993 (24/5/94) (23/4/96)
	Returns		Return 0 (FALSE) or 1 (TRUE) according to the relative lexical order of
				two strings.
	Purpose:	Comparison (logical) operators, eg.
					x = "alpha";
					y = "beta";
					cout << "First is " << (x < y ? x : y) << endl;
				These functions will work according to local custom.

	Notes:		Now uses IsIdentical to compare the strings, which is 12 times faster
				than the new internationalised CompareTo.
**************************************************************************************/
inline bool operator!=(const StringBase& x, const StringBase& y)
{
	return !x.IsIdentical(y);
}


inline bool operator!=(const StringBase& x, const TCHAR* y)
{
	return !x.IsIdentical(y);
}



/**************************************************************************************
<	INT32 operator<(const StringBase& lhs, const StringBase& rhs)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com> (Jason)
	Created:	22nd April 1993 (24/5/94)
	Returns		Return 0 (FALSE) or 1 (TRUE) according to the relative lexical order of
				two strings.
	Purpose:	Comparison (logical) operators, eg.
					x = "alpha";
					y = "beta";
					cout << "First is " << (x < y ? x : y) << endl;
				These functions will work according to local custom.
**************************************************************************************/
inline bool operator<(const StringBase& x, const StringBase& y)
{
	return x.CompareTo(y) < 0;
}


inline bool operator<(const StringBase& x, const TCHAR* y)
{
	return x.CompareTo(y) < 0;
}



	
/**************************************************************************************
>	INT32 operator>(const StringBase& lhs, const StringBase& rhs)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com> (Jason)
	Created:	22nd April 1993 (24/5/94)
	Returns		Return 0 (FALSE) or 1 (TRUE) according to the relative lexical order of
				two strings.
	Purpose:	Comparison (logical) operators, eg.
					x = "alpha";
					y = "beta";
					cout << "First is " << (x < y ? x : y) << endl;
				These functions will work according to local custom.
**************************************************************************************/
inline bool operator>(const StringBase& x, const StringBase& y)
{
	return x.CompareTo(y) > 0;
}


inline bool operator>(const StringBase& x, const TCHAR* y)
{
	return x.CompareTo(y) > 0;
}


	
	
/**************************************************************************************
<	INT32 operator<=(const StringBase& lhs, const StringBase& rhs)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com> (Jason)
	Created:	22nd April 1993 (24/5/94)
	Returns		Return 0 (FALSE) or 1 (TRUE) according to the relative lexical order of
				two strings.
	Purpose:	Comparison (logical) operators, eg.
					x = "alpha";
					y = "beta";
					cout << "First is " << (x < y ? x : y) << endl;
				These functions will work according to local custom.
**************************************************************************************/
inline bool operator<=(const StringBase& x, const StringBase& y)
{
	return x.CompareTo(y) <= 0;
}
	

inline bool operator<=(const StringBase& x, const TCHAR* y)
{
	return x.CompareTo(y) <= 0;
}

	


/**************************************************************************************
>	INT32 operator>=(const StringBase& lhs, const StringBase& rhs)
    		... etc.

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com> (Jason)
	Created:	22nd April 1993 (24/5/94)
	Returns		Return 0 (FALSE) or 1 (TRUE) according to the relative lexical order of
				two strings.
	Purpose:	Comparison (logical) operators, eg.
					x = "alpha";
					y = "beta";
					cout << "First is " << (x < y ? x : y) << endl;
				These functions will work according to local custom.
**************************************************************************************/
inline bool operator>=(const StringBase& x, const TCHAR* y)
{
	return x.CompareTo(y) >= 0;
}



inline bool operator>=(const StringBase& x, const StringBase& y)
{
	return x.CompareTo(y) >= 0;
}


/**************************************************************************************

>	static INT32 StringBase::TCharToNum(TCHAR c)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/07/96
	Inputs:		c, a character
	Returns:	If c turns out to be a numeric character, its numeric value 0..9
	Purpose:	Convert a TCHAR into a numeric value. The routine does not check for
				valid numeric characters, it just performs the conversion. Hence it
				is possibly for you to pass in the character 's' for instance and receive
				a >9 value back

***************************************************************************************/

inline INT32 StringBase::TCharToNum(TCHAR c)
{
	return c - TEXT('0');
}




/**************************************************************************************
>	TCHAR* cc_camStrncpy(TCHAR* dest, const TCHAR* src, size_t n)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9th Feb 1994

	Inputs:		destination, source, characters to copy
	Outputs:	-

	Returns:	destination

	Purpose:	Replaces the Windows API function camStrncpy(), whose existence has been
				rewritten out of history.  Does basically what the standard C library
				function memmove() does, only for TCHARs.
***************************************************************************************/

inline TCHAR* cc_camStrncpy(TCHAR* dest, const TCHAR* src, INT32 n)
{
	ERROR3IF(src == 0 || dest == 0 || n <= 0,
				"0 (zero) parameters passed to cc_camStrncpy!");
	return camStrncpy(dest, src, n + 1);		// +1 as camStrncpy includes the terminator.
}



/**************************************************************************************

>	TCHAR* cc_lstrrchr(TCHAR *Str, TCHAR c)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21st November 1995

	Inputs:		Str	- String to search in
				c	- Character to search for

	Returns:	returns a pointer to the last occurrence of c in the given string
				returns 0 if ch does not occurr in the string

	Purpose:	Finds the last occurrence of ch in string.  The terminating
				0 character is used as part of the search.
				This version is case sensitive.
	Notes:		Replaces the Windows API version which seems to be missing. Uses TCHAR's
				instead of char's

***************************************************************************************/

inline TCHAR* cc_lstrrchr(TCHAR *Str, TCHAR c)
{
	ERROR3IF(Str == 0, "0 (zero) parameters passed to cc_lstrrchr");
	return (Str != 0) ? camStrrchr(Str, c) : 0;

//	TCHAR *start = Str;
//
//	/* find end of string */
//	while (*Str++);
//
//	/* search towards front */
//	while (--Str != start && *Str != c);
//
//	/* char found ? */
//	if (*Str == c)
//		return(Str);
//
//	return(0);
}


/**************************************************************************************

>	TCHAR* cc_lstrtok(TCHAR *String1, TCHAR *String2)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21st November 1995

	Inputs:		String1	- string to tokenize, or 0 to get next token
				String2 - string of characters to use as delimiters
	Returns:	returns pointer to first token in string, or if string
				was 0, to next token
				returns 0 when no more tokens remain.

	Purpose:	strtok considers the string to consist of a sequence of zero or more
				text tokens separated by spans of one or more control chars. the first
				call, with string specified, returns a pointer to the first char of the
				first token, and will write a 0 char into string immediately
				following the returned token. subsequent calls with zero for the first
				argument (string) will work thru the string until no tokens remain. the
				control string may be different from call to call. when no tokens remain
				in string a 0 pointer is returned. remember the control chars with a
				bit map, one bit per ascii char. the 0 char is always a control char.
	Notes:		Replaces the Windows API version which seems to be missing. Uses TCHAR's
				instead of char's. I'm not totally convinced it's going to work with
				DBCS, but if everyone uses it, at least we've got a single fix...

***************************************************************************************/

// Only briefly tested...
inline TCHAR* cc_lstrtok(TCHAR *String1, TCHAR *String2)
{
	return NULL;
	
//	return _tcstok(String1, String2);

//	TCHAR *string = String1;
//	TCHAR *str;
//	TCHAR *ctrl = String2;
//
//	TCHAR map[32];
//	INT32 count;
//
//	static TCHAR nextoken[256];
//
//	/* Clear control map */
//	for (count = 0; count < 32; count++)
//		map[count] = 0;
//
//	/* Set bits in delimiter table */
//	do
//	{
//		map[*ctrl >> 3] |= (1 << (*ctrl & 7));
//	} while (*ctrl++);
//
//	/* Initialize str. If string is 0, set str to the saved
//	 * pointer (i.e., continue breaking tokens out of the string
//	 * from the last strtok call) */
//	if (string)
//		str = string;
//	else
//		str = (TCHAR *)nextoken;
//
//	/* Find beginning of token (skip over leading delimiters). Note that
//	 * there is no token iff this loop sets str to point to the terminal
//	 * 0 (*str == '\0') */
//	while ( (map[*str >> 3] & (1 << (*str & 7))) && *str )
//			str++;
//
//	string = str;
//
//	/* Find the end of the token. If it is not the end of the string,
//	 * put a 0 there. */
//	for ( ; *str ; str++ )
//		if ( map[*str >> 3] & (1 << (*str & 7)) )
//		{
//	    	*str++ = '\0';
//	        break;
//	    }
//
//	/* Update nextoken (or the corresponding field in the per-thread data
//	 * structure */
//	camStrcpy(nextoken, str);
//
//	/* Determine if a token has been found. */
//	if ( string == str )
//		return 0;
//	else
//		return string;
}

/**************************************************************************************

>	INT32 cc_lstrncmp(TCHAR *String1, TCHAR *String2, INT32 Count)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21st November 1995

	Inputs:		String1	- string to compare
				String2 - string to compare
				Count	- maximum number of characters to compare

	Returns:	returns <0 if first < last
				returns  0 if first == last
				returns >0 if first > last

	Purpose:	Compares two strings for lexical order.  The comparison stops
				after: (1) a difference between the strings is found, (2) the end
				of the strings is reached, or (3) count characters have been
				compared.
	Notes:		Replaces the Windows API version which seems to be missing. Uses TCHAR's
				instead of char's, and also uses CompareString for multinational support

***************************************************************************************/

inline INT32 cc_lstrncmp(TCHAR *String1, TCHAR *String2, INT32 Count)
{
	ERROR3IF(String1 == 0 || String2 == 0,
		"INT32 cc_lstrncmp(TCHAR *String1, TCHAR *String2, INT32 Count) given 0 params");
	return (Count != 0) ? camStrncmp(String1, String2, Count) : 0;
}



/**************************************************************************************
>	size_t cc_strlenCharacters( const TCHAR* string )

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/8/96
	Inputs:		string - pointer to a string
	Returns:	The number of *characters* in a string
	Purpose:	Returns the number of characters in a string - not necessaraily the 
				length of the string.
	SeeAlso:	cc_strlenBytes
***************************************************************************************/
inline size_t cc_strlenCharacters( const TCHAR* string )
{
	return camStrclen(string);
}



/**************************************************************************************
>	size_t cc_strlenBytes( const TCHAR* string )

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/8/96
	Inputs:		string - pointer to a string
	Returns:	The length of a string in bytes
	Purpose:	Returns the number of bytes needed to store the given string.  Dosen't
				include the terminator.  Unicode and DBCS aware.
	SeeAlso:	camStrclen
***************************************************************************************/
inline size_t cc_strlenBytes( const TCHAR* string )
{
	return camStrlen(string)*sizeof(TCHAR);
}



/**************************************************************************************

>	TCHAR* cc_lstrchr(TCHAR *Str, TCHAR c)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21st November 1995

	Inputs:		Str	- String to search in
				c	- Character to search for

	Returns:	returns pointer to the first occurence of c in string
				returns 0 if c does not occur in string

	Purpose:	Searches a string for a given character, which may be the
				0 character '\0'.
				This version is case sensitive.
	Notes:		Replaces the Windows API version which seems to be missing. Uses TCHAR's
				instead of char's

***************************************************************************************/

inline TCHAR* cc_lstrchr(TCHAR *Str, TCHAR c)
{
	ERROR3IF(Str == 0, "0 (zero) parameters passed to cc_lstrchr");
	return (Str != 0) ? camStrchr(Str, c) : 0;

//	while (*Str && *Str != c)
//		Str++;
//
//	if (*Str == c)
//		return(Str);
//	return(0);
}


/**************************************************************************************

>	INT32 StringBase::SkipSpace(INT32 pos=0) const

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/07/96
	Inputs:		pos = the character index from which to start
	Returns:	the index of the next none space character
				-1 if none, OR an exception is thrown
	Purpose:	Find the next non-space character starting from pos. If pos is a
				none space character, the function will simply return with the same
				index.
	Notes:		** Throws an exception if string exceptions are enabled **

***************************************************************************************/

inline INT32 StringBase::SkipSpace(INT32 pos) const
{
	return FindNextChar(pos);
}

#endif	// INC_STRINGBASE
