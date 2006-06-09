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
/**************************************************************************************
	

	Definition of the string class, as declared in StringBase.h.
***************************************************************************************/

#include "camtypes.h"
#include <iostream>
#include <stdarg.h>

//#include "basestr.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "camelot.h"
//#include "fixmem.h"				// for CCMalloc, etc... - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "reshelp.h"
//#include "oilmods.h"			// for OILModule::xx
#if !defined(EXCLUDE_FROM_XARLIB)
//#include "convert.h"			// for MakePercent - in camtypes.h [AUTOMATICALLY REMOVED]
#endif

//#include "Res/UKEnglish/stringtbl.h"

//#include "richard3.h"			// for MakePercent
//#include "mario.h"

DECLARE_SOURCE("$Revision$");

BOOL StringBase::ThrowExceptionStack[str_MAXEXCEPTIONS];
INT32 StringBase::CurrEx = -1;

	
/**************************************************************************************
>	virtual BOOL StringBase::Alloc(INT32 size)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22nd April 1993
	Inputs:		The number of characters the string is to hold (EXCLUDING the
				terminating 0).
	Returns:	TRUE if the allocation was successful.
	Purpose:	Allocates space on the heap for a 0-terminated string of the
				passed size.  This function should only be called for the StringBase
				base class, NOT for any of its fixed-length derivatives.
	Errors:		_R(IDE_NOMORE_MEMORY)
	Scope:		Public
***************************************************************************************/
BOOL StringBase::Alloc(INT32 size)
{
	ERROR3IF(size < 0, "Attempt to ALLOCate a String of negative length");
	if (text) delete[] text;
#ifdef _UNICODE
	text = new TCHAR[length = size + 1];
#else
	text = new TCHAR[length = (size + 1) * 2];		// MBCS
#endif
	ERRORIF(text == 0, _R(IDE_NOMORE_MEMORY), FALSE);
	*text = 0;
	return TRUE;
}


	

/**************************************************************************************
>	BOOL StringBase::Load(UINT32 resourceID, UINT32 moduleID = 0)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22nd April 1993
	Inputs:		A string resource identifier and an optional module handle.  Specify
				a module handle only if the resource is not located in the default
				place (CAMELOT.EXE).  Most resources will be.
				If a moduleID is given, the string will try to be loaded from that module.
				If it fails, it will fall-back to the .exe file. The string will be truncated
				if required to fit the available buffer size.
	Returns:	TRUE of the string is successfully loaded, FALSE otherwise.
	Purpose:	Extracts a string from the application's .EXE file, by default,
				or another module (eg. a dedicated DLL), overwriting the old contents
				of this StringBase object.
	SeeAlso:	SmartLoadString
***************************************************************************************/
BOOL StringBase::Load(UINT32 resID, UINT32 moduleID)
{
	ERROR3IF(text == 0, "Attempt to LOAD a String which hasn't been ALLOCated");

	// Load the string resource. Get OIL layer to truncate to suit the destination.
	if (!text) return FALSE;
	return SmartLoadString(moduleID, resID, text, length*sizeof(TCHAR));
}


	

/**************************************************************************************
>	StringBase& StringBase::toTitle()

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/10/97
	Purpose:	Converts all the first letters of words to uppercase and
				the rest to lower case.
***************************************************************************************/
StringBase& StringBase::toTitle()
{
	ERROR3IF(text == 0, "Call to TOLOWER for an unALLOCated String");

	//First set up a pointer with which to go through the string
	TCHAR* pText=text;

	//And set up a buffer, which we will use to convert the case of single
	//characters

	//GTODO: Put this in a separate function?
	TCHAR bufToConvert[2];
	bufToConvert[0]='\0';
	bufToConvert[1]='\0';

	//Now, until we reach the end of the string
	while (pText && *pText!='\0')
	{
		//Move pText through the string until it points
		//to a letter
		while (pText && !IsAlpha(*pText) && *pText!='\0')
		{
			pText++;
		}

		//And, if we are not at the end of the string
		if (pText && *pText!='\0')
		{
			//Convert that letter to upper case
			bufToConvert[0]=*pText;

			CharUpper(bufToConvert);

			*pText=bufToConvert[0];

			//Now advance to the next character
			pText++;
		}

		//And convert any following letters to lower case, until
		//we reach the end of the string of letters. We also
		//count apostrophes as letters, since they may be part of
		//a word
		while (pText && (IsAlpha(*pText) || *pText=='\''))
		{
			// Convert that letter to lower case
			bufToConvert[0]=*pText;
			CharLower(bufToConvert);
			*pText=bufToConvert[0];
			
			// And advance to the next letter
			pText++;
		}
	}

	return *this;
}



/**************************************************************************************

>	INT32 StringBase::CompareTo(const StringBase& other, BOOL CaseSensitive = TRUE) const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/5/94
	Inputs:		other - A string against which this one will be compared
	Returns:	The same as the standard C strcmp, i.e:

				A negative value if this is considered 'less than' other

				Zero, if the strings are considered equal

				A positive value if this is considered 'greater than' other

	Purpose:	Compare this StringBase object to another string. This is mainly
				intended for use internally by the comparison operators, but
				may also come in handy in other places.

	Notes:		This function is VERY SLOW, but does proper comparisons for the
				locale to determine relative positioning of strings. Use 
				StringBase::IsIdentical to see if 2 strings are exactly the same.

	Errors:		If either of the strings is 0, an ENSURE failure is generated

	SeeAlso:	StringBase::IsIdentical

**************************************************************************************/

INT32 StringBase::CompareTo(const StringBase& other, BOOL CaseSensitive) const
{
	ERROR3IF(text == 0 || other.text == 0, "StringBase::CompareTo: not ALLOCed");
PORTNOTE("other", "StringBase::CompareTo should be locale specific");
#if FALSE
	DWORD dw = NORM_IGNOREKANATYPE | NORM_IGNOREWIDTH;
	if (!CaseSensitive) dw |= NORM_IGNORECASE;
	return CompareString(LOCALE_USER_DEFAULT, dw, text, -1, other.text, -1) - 2;
#endif

	if( CaseSensitive )
	{
		return camStrcmp( text, other.text );
	}
	else
	{
		return camStricmp( text, other.text );
	}
}



/**************************************************************************************

>	INT32 StringBase::CompareTo(const TCHAR *other, BOOL CaseSensitive) const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/5/94
	Inputs:		other - A string against which this one will be compared
	Returns:	The same as the standard C strcmp, i.e:

				A negative value if this is considered 'less than' other

				Zero, if the strings are considered equal

				A positive value if this is considered 'greater than' other

	Purpose:	Compare this StringBase object to another string. This is mainly
				intended for use internally by the comparison operators, but
				may also come in handy in other places.

	Notes:		This function is VERY SLOW, but does proper comparisons for the
				locale to determine relative positioning of strings. Use 
				StringBase::IsIdentical to see if 2 strings are exactly the same.

	Errors:		If either of the strings is 0, an ENSURE failure is generated

	SeeAlso:	StringBase::IsIdentical

**************************************************************************************/

INT32 StringBase::CompareTo(const TCHAR* other, BOOL CaseSensitive) const
{
	ERROR3IF(text == 0 || other == 0, "StringBase::CompareTo: null buffers");
PORTNOTE("other", "StringBase::CompareTo should be locale specific");
#if FALSE
	DWORD dw = NORM_IGNOREKANATYPE | NORM_IGNOREWIDTH;
	if (!CaseSensitive) dw |= NORM_IGNORECASE;
	return CompareString(LOCALE_USER_DEFAULT, dw, text, -1, other, -1) - 2;
#endif

	if( CaseSensitive )
	{
		return camStrcmp( text, other );
	}
	else
	{
		return camStricmp( text, other );
	}
}



/**************************************************************************************
>	StringBase& StringBase::operator=(const StringBase& rhs)


	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22nd April 1993
	Inputs:		A reference to a constant String.
	Returns:	A reference to this.
	Purpose:	Assignment (set equal to) operator, eg.
					StringBase x("rhubarb");
					x = "custard";
***************************************************************************************/
StringBase& StringBase::operator=(const StringBase& other)
{
	ERROR3IF(!text || !other.text, "Call to String::operator= for an unALLOCated String");
//	ERROR3IF(camStrlen(other.text) >= length, "Call to String::operator= will result in overflow");

	if (text && other.text)
	{
		if(camStrlen(other.text) >= length)
			camStrncpy(text, other.text, (INT32)length - 1);
		else
			camStrcpy(text, other.text);
	}
	return *this;
}


	
/**************************************************************************************
>   StringBase& StringBase::operator=(const TCHAR* pcsz)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22nd April 1993
	Inputs:		A pointer to a constant C-style (possibly wide-character) string.
	Returns:	A reference to this.
	Purpose:	Assignment (set equal to) operator, eg.
					StringBase x("rhubarb");
					x = "custard";
***************************************************************************************/
StringBase& StringBase::operator=(const TCHAR* s)
{
	ERROR3IF(!text || !s, "Call to String::operator= for an unALLOCated String or 0 char*");
//	ERROR3IF(camStrlen(s) >= length, "Call to String::operator= will result in overflow");

	if (text && s)
	{
		if(camStrlen(s) >= length)
			camStrncpy(text, s, (INT32)length-1);
		else
			camStrcpy(text, s);
	}
	return *this;
}

	
	
/**************************************************************************************
>	StringBase& StringBase::operator+=(const TCHAR* s)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22nd April 1993
	Inputs:		The C-style character "string" to add to the end of this String.
	Returns:	A reference to the lhs, ie. this StringBase as an lvalue.
	Purpose:	Append string y onto the end of string x, eg.
					x = TEXT("hello");
					cout << (x += TEXT(" world!")) << endl;
***************************************************************************************/
StringBase& StringBase::operator+=(const TCHAR* s)
{
	ERROR3IF(!text || !s, "Call to String::operator+= for an unALLOCated String");
	ERROR3IF(camStrlen(text) + camStrlen(s) >= length,
			"Call to String::operator+= will result in overflow");
	if (text && s) SafeCat(text, s, length-1);
	return *this;
}


/**************************************************************************************
>	StringBase& StringBase::operator+=(const TCHAR ch)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/12/95
	Inputs:		A character to add to the end of this String.
	Returns:	A reference to the lhs, ie. this StringBase as an lvalue.
	Purpose:	Appends this character to the end of this string
***************************************************************************************/
StringBase& StringBase::operator+=(const TCHAR ch)
{
	ERROR3IF(text == 0, "Call to String::operator+= for an unALLOCated String");
	INT32 CurrentLength = camStrlen(text);
	ERROR3IF(CurrentLength == MaxLength() - 1, "StringBase::operator+= will overflow");

	text[CurrentLength] = ch;
	text[CurrentLength + 1] = 0;
	return *this;
}



/**************************************************************************************

>	static TCHAR *StringBase::SafeCat(TCHAR *string1, const TCHAR *string2, INT32 Max )

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26th February 1996

	Inputs:		string1		0-terminated destination string
				string2		0-terminated source string
				Max			Maximum number of characters string1 is allowed to contain
	
	Outputs:	string1		Resulting string
	Returns:	string1

	Purpose:	Appends string2 to the end of string1 and returns string1 as the result.
				The difference between this and camStrcat is the Max parameter which helps
				alleviate the buffer overwrite problems. Setting max to the length member
				of a stringbase will ERROR3IF that we don't go overwriting ends of buffers
				and suchlike... In such cases the result will be trucated.

				eg, TCHAR String1[256] = "Fish";
					SafeCat(String1, "cake", 7);
				will result in "Fishcak" being placed in String1...

***************************************************************************************/

TCHAR *StringBase::SafeCat(TCHAR *string1, const TCHAR *string2, UINT32 Max )
{
	ERROR3IF(string1 == 0 || string2 == 0, "StringBase::SafeCat given 0 params");

	UINT32 len1 = camStrlen(string1);

	ERROR3IF(len1 > Max, "First parameter to StringBase::SafeCat already larger than safe");

	// No point trying if all space used up
	if (len1 <= Max)
	{
		// Are we going to overflow ?
		if ((len1 + camStrlen(string2)) > Max)
		{
			ERROR3("Call to StringBase::SafeCat will give truncated result");

			// Yeah, so we need to chop some of the second string off before concatenation
			INT32 Required = Max - len1;
			if (Required > 0)
			{
				camStrncpy( &(string1[len1]), string2, Required);
				string1[Max] = 0;
			}
		}
		else
		{
			// No, just do a normal strcat
			camStrcat(string1, string2);
		}
	}
	
	return string1;
}
	

	
/**************************************************************************************
>	const StringBase& StringBase::Left(StringBase* out, INT32 count) const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22nd April 1993
	Inputs:		The number of characters in the prefix.
	Outputs:	The leftmost [count] characters of the StringBase.  If count is greater than
				the StringBase's length, the the whole StringBase is returned.  If count is equal
				to zero, the 0 StringBase is output.
	Purpose:	Similar to the BASIC Left$() string function.
	Errors:		ERROR3IF failure if (count) is negative, or if the out StringBase is not
				large enough to hold the result.
	SeeAlso:	StringBase::Right
				StringBase::Mid
***************************************************************************************/
const StringBase& StringBase::Left(StringBase* out, UINT32 count) const
{
	ERROR3IF(out == 0, "0 output parameter in StringBase::Left");
	ERROR3IF(!text || !out->text, "Call to String::Left for unALLOCated String");
	ERROR3IF(count < 0, "Call to String::Left with a meaningless character count");
	ERROR3IF(out->length <= count, "Call to String::Left will overflow");

	if (text && out && out->text)
	{
		if (count == 0)
			*(out->text) = 0;
		else if (count >= camStrlen(text))
			camStrcpy(out->text, text);
		else
		{
			camStrncpy(out->text, text, (INT32)count);
			out->text[count] = 0;
		}
	}

	return *this;
}


	
	
/**************************************************************************************
>	const StringBase& StringBase::Mid(StringBase* out, INT32 first, INT32 count) const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22nd April 1993
	Inputs:		The position of the first character to extract and the total number of
				characters to include in the sub-string.  Note that	the first character
				of a string is at position 0, like C, not 1, like BASIC.
	Outputs:	Places the n characters starting at position m in [out], which must be
				large enough.
	Purpose:	Similar to the BASIC Mid$() function.
	SeeAlso:	StringBase::Left
				StringBase::Right
***************************************************************************************/
const StringBase& StringBase::Mid(StringBase* out, UINT32 first, UINT32 count) const
{
	ERROR3IF(out == 0, "0 output parameter in StringBase::Mid");
	ERROR3IF(!text || !out->text, "Call to String::Mid for unALLOCated String");
	ERROR3IF(count < 0, "Call to String::Mid with a meaningless character count");
	ERROR3IF(out->length <= count, "Call to String::Mid will overflow");

	if (text && out && out->text)
	{	
		UINT32 L = camStrlen(text);
		if ((first + count) > L) count = L - first;
		if (count <= 0)
			*(out->text) = 0;
		else
		{
			camStrncpy(out->text, text + first, (INT32)count);
			out->text[count] = 0;
		}
	}

	return *this;
}


	
	
/**************************************************************************************
>	const StringBase& StringBase::Right(StringBase* out, INT32 count) const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22nd April 1993
	Inputs:		The number of characters in the postfix.
	Returns:	The rightmost n characters of the StringBase.  If n is greater than the
				StringBase's length, the value of the whole StringBase is returned.  If n is
				less than or equal to zero, the 0 StringBase is returned.
	Purpose:	Similar to the BASIC Right$() function.					
	SeeAlso:	StringBase::Left
				StringBase::Mid
***************************************************************************************/
const StringBase& StringBase::Right(StringBase* out, UINT32 count) const
{
	ERROR3IF(out == 0, "0 output parameter in StringBase::Right");
    ERROR3IF(!text || !out->text, "Call to StringBase::Right for unALLOCated String");
    ERROR3IF(count < 0, "Call to StringBase::Right with a meaningless character count");
    ERROR3IF(out->length <= count, "Call to StringBase::Right will overflow");

	if (text && out && out->text)
	{    
	    UINT32 L = camStrlen(text);
	    if (count == 0)
	    	*(out->text) = 0;
	    else if (count >= L)
	    	camStrcpy(out->text, text);
	    else
	    {
	    	camStrncpy(out->text, text + L - count, (INT32)count);
	    	out->text[count] = 0;
	    }
	}

	return *this;
}


	
/**************************************************************************************
>	void StringBase::Split(StringBase* pstrStart, StringBase* pstrEnd, 
			INT32 iSplit, BOOL fIncludeSplitChar)

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/5/97
	Inputs:		iSplit		Index of the splitting character
				fIncludeSplitChar
							TRUE	Put the splitting character at the end of strStart
							FALSE	Put the splitting character at the start of strEnd
	Outputs:	pstrStart	The string into which to put everything before
							the splitting character
				pstrEnd		The string into which to put everything before
							the splitting character

	Purpose:	Splits a string into two separate strings at a given character.

				Note that pstrStart or pstrEnd may either be 0 or may point
				at this string.

				Useful in parsing URLs.

	Errors:		If any parameters are invalid
	SeeAlso:	WebAddress::Parse
***************************************************************************************/
void StringBase::Split(StringBase* pstrStart, StringBase* pstrEnd, INT32 iSplit, BOOL fIncludeSplitChar)
{
	//First check our parameters
	if (iSplit<0)
	{
		ERROR2RAW("StringBase::Split - 0 parameter");
		return;
	}

	//If iSplit is greater than the length of the string, then
	//put the entire string into pstrStart
	if (iSplit>=Length())
	{
		if (pstrStart)
			*pstrStart=*this;

		if (pstrEnd)
			pstrEnd->Empty();

		return;
	}
		
	//This variable is 1 if we're including the splitting character
	//and 0 otherwise
	INT32 iIncludeSplitChar=(fIncludeSplitChar? 1 : 0);

	//And get the length
	INT32 iLength=Length();

	//Now, let's get the start string
	String_256 strStart;

	//This is the number of characters to include in the start string
	INT32 iStartChars=iSplit+iIncludeSplitChar;

	//And now actually create the start string
	Left(&strStart, iStartChars);

	//And the end string
	String_256 strEnd;

	Right(&strEnd, iLength-iStartChars);

	//Now, if we have been passed an output start string
	if (pstrStart)
		//Copy our start string into it
		*pstrStart=strStart;

	//And if we have been passed an output end string
	if (pstrEnd)
		//Copy our end string into it
		*pstrEnd=strEnd;

}

	
/**************************************************************************************
>	INT32 StringBase::Sub(const StringBase& mask, INT32 from = 0, TCHAR wildcard = 0) const	
	
	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22nd April 1993
	Inputs:		A reference to the substring (mask) to search for; optionally the first
				character position to search from (default is first character in the
				string); optionally the character in the mask which is acting as a
				wildcard (if not specified, there is no wildcard in the mask).
	Returns:	Returns the starting position of the substring, or -1 if the
				substring does not appear within "this".
	Purpose:	Similar to the (non-standard) BASIC Instr$() function.
	Scope:		Public
***************************************************************************************/
INT32 StringBase::Sub(const StringBase& mask, UINT32 from, TCHAR fuzz) const
{
	ERROR3IF(!text || !mask.text, "Call to String::Sub for an unALLOCated String");
	ERROR3IF(from < 0, "Call to String::Sub with a negative \"from\" character position");
	
	if (text && mask.text)
	{
		if (from >= camStrlen(text) || camStrlen(mask.text) == 0)
			return -1;

		BOOL matched = FALSE;						// 2 state DFA
		const TCHAR* pSub = mask.text;
	 	for (const TCHAR* pThis = text + from; *pThis; pThis++)
			if (*pSub != fuzz && *pThis != *pSub) 	// current pair match?
			{
				if (matched)                      	// no, so if we had a match previously
				{
					pThis--;						// then we must go back one character
					matched = FALSE;
				}
				pSub = mask.text;               	// and also match from beginning of mask
			}
			else
			{
				matched = TRUE;						// yes, we have a match
				if (*++pSub == 0)					// matched all of mask?
					return (pThis - text) - (pSub - mask.text) + 1;
			}
	}

	// return NOT FOUND
	return -1;									
}

/**************************************************************************************
>	INT32 StringBase::SubWithoutCase(const StringBase& strToFind) const	
	
	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/6/97
	Inputs:		mask	The string to search for
				from	(optional) the character to search from
	Returns:	Returns the starting position of the substring, or -1 if the
				substring does not appear within "this".

				The comparison is done without regard to case. For example, trying
				to find the string "fILe" in the following string:

				"The Rockford Files"

				would give a result of 13.
	Purpose:	Similar to the (non-standard) BASIC Instr$() function.
	Scope:		Public
***************************************************************************************/
INT32 StringBase::SubWithoutCase(const StringBase& strToFind) const
{
	ERROR3IF(!text || !strToFind.text, "Call to String::Sub for an unALLOCated String");

	if (IsEmpty() || strToFind.IsEmpty())
		return -1;
	
#if !defined(__WXMAC__)
	//First set up a pointer to search this string with
	TCHAR* pcThisChar=text;
	INT32 iThisChar=0;
	
	//Now, while pcThisChar is valid
	while (pcThisChar!=0 && *pcThisChar!='\0')
	{
		//Is strToSearch from pcThisChar onwards the same
		//as strToFind, doing a case insensitive comparision?
		if (camStrnicmp(pcThisChar, strToFind, camStrlen(strToFind))==0)
		{
			//Yes. So return the index of the character we found
			return iThisChar;
		}

		pcThisChar=camStrinc(pcThisChar);
		iThisChar++;
	}
#else
PORTNOTE("macport", "StringBase::SubWithoutCase not yet implemented");
ERROR3("StringBase::SubWithoutCase not yet implemented");
#endif
	//Otherwise return -1
	return -1;

}




/**************************************************************************************
>	void StringBase::Insert(StringBase *InsertString, INT32 Position);	
	
	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26th August 1994
	Inputs:		InsertString - the string to inserted.
				Position - the number of the character before which InsertString is
						   to be inserted.  Counting starts at 0.
	Returns:	-
	Purpose:	Inserting a string into another one.
	Scope:		Public
***************************************************************************************/
void StringBase::Insert( const StringBase &InsertString, INT32 Position)
{
	ERROR3IF(Position < 0,"Negative position passed to StringBase::Insert");
	ERROR3IF(!text || !InsertString.text, "Not ALLOCed in StringBase::Insert");

	if (text && InsertString && InsertString.text)
	{
		INT32	LeftL = Position;
		INT32	RightL = Length()-Position;
		StringBase	LeftS;
		StringBase	RightS;

		if (LeftL<=0)
		{
			RightS.Alloc(this->Length());
			RightS=*this;
			*this=*InsertString;
			*this+=RightS;
		}
		else
		{
			if (RightL<=0)
			{
				*this+=*InsertString;
			}
			else
			{
				LeftS.Alloc(LeftL);
				Left(&LeftS, LeftL);
				RightS.Alloc(RightL);
				Right(&RightS, RightL);

				*this=LeftS;
				*this+=*InsertString;
				*this+=RightS;
			}
		}
	}
}



/**************************************************************************************
>	void StringBase::Remove(INT32 Position, INT32 Number);	
	
	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26th August 1994
				Position - the position of the first character to be removed.  Counting
				           starts at 0.
				Number - the number of characters to be removed.
	Returns:	-
	Purpose:	Inserting a string into another one.
	Scope:		Public
***************************************************************************************/
void StringBase::Remove(INT32 nchStartPos, INT32 nchRemove)
{
	ERROR3IF(nchStartPos < 0,"Bad start position passed to StringBase::Remove");
	if (!text)
	{
		ERROR3("Not ALLOCed in StringBase::Remove");
		return;
	}

	StringBase strLeftSeg;
	StringBase strRightSeg;
	INT32	nchRightSeg = Length() - nchStartPos - nchRemove;

	if (nchStartPos <= 0)
	{
		if (nchRightSeg <= 0)
			Empty();
		else
			Right(this, nchRightSeg);
	}
	else
	{
		if (nchRightSeg <= 0)
		{
			Left(this, nchStartPos);
		}
		else
		{
			if (!strLeftSeg.Alloc(nchStartPos) || !strRightSeg.Alloc(nchRightSeg)) return;
			Left(&strLeftSeg, nchStartPos);
			Right(&strRightSeg, nchRightSeg);
			*this = strLeftSeg;
			*this += strRightSeg;
		}
	}
}




/**************************************************************************************

>	void StringBase::SwapChar(const TCHAR schar, const TCHAR rchar)
	
	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/04/95
	Inputs:		schar - the search character
				rchar - the replacement character
	Returns:	-
	Purpose:	Exchange all schar characters in the string with rchar characters
	Scope:		Public

***************************************************************************************/

void StringBase::SwapChar(const TCHAR schar, const TCHAR rchar)
{
	ERROR3IF(!text ,"Call to String::SwapChar for an unALLOCated String");
	if (text)
	{
		TCHAR* pThis = text;
		while( *pThis != wxT('\0') )
		{
			if (*pThis == schar)
				*pThis = rchar;
			pThis = camStrinc(pThis);
		}
	}
}


/**************************************************************************************

>	INT32 StringBase::CountChar(const TCHAR tchar)
	
	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/11/95
	Inputs:		tchar - the search character
	Returns:	Number of times 'tchar' appears in the string
	Purpose:	Count all tchar characters in the string
	Scope:		Public

***************************************************************************************/

INT32 StringBase::CountChar(const TCHAR tchar)
{
	ERROR3IF(!text, "Call to String::CountChar for an unALLOCated String");
	INT32 Count = 0;
	if (text)
	{
		TCHAR* pThis = text;
		while (*pThis != 0)
		{
			if (*pThis == tchar)
				Count ++;
			pThis = camStrinc(pThis);
		}
	}
	return Count;
}		


#if !defined(EXCLUDE_FROM_XARLIB)
/**************************************************************************************
>	void StringBase::MakePercent(INT32 Value);
	void StringBase::MakePercent(double Value);
	void StringBase::MakePercent(TCHAR *Value);

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22nd November 1995
	Inputs:		Takes a number / string
	Purpose:	Set the string equal to the given argument + a % character, or however
				the current locale wants it
	Example:	String_32 Str;
				Str.MakePercent(23);
				For UK, Str now equals "23%"
**************************************************************************************/

void StringBase::MakePercent(INT32 Value)
{	
	if (text)
	{
		String_32 Format(_R(IDS_PERCENT_LONG));
		Format.FixFormat();
		camSnprintf( text, 32, (TCHAR *)Format, Value );
	}
}

void StringBase::MakePercent(double Value)
{
	if (text)
	{
		String_32 TmpStr;
		if(Convert::DoubleToString(Value, &TmpStr))
			MakePercent((TCHAR *)TmpStr);
	}
}

void StringBase::MakePercent(TCHAR *Value)
{
	if (text)
	{
		// Some callers seem to call this with Value=Text ... Grrr
		String_32 tmp(Value);
		String_32 Format(_R(IDS_PERCENT_STRING));
		Format.FixFormat();
		camSnprintf( text, 32, (TCHAR *)Format, tmp.text );
	}
}

// Make formats into Unicode compatible variants
void StringBase::FixFormat()
{
#if FALSE
#if 0 != wxUSE_UNICODE
	INT32 i=0;
	while ((i+1)<Length()) // Don't look at the last character
	{
		if ( (text[i]==_T('%')) && ( (text[i+1]==_T('s')) || (text[i+1]==_T('c')) ) )
		{
			String_8 l(_T("l"));
			Insert(l, i+1);
			i++; // Skip over the inserted char too
		}
		i++;
	}
#endif
#endif
}

#endif


/***********************************************************************************************
>	INT32 SmartLoadString(UINT32 modID, UINT32 resID, LPTCHAR buf, INT32 size)

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/8/93
	Inputs:		modID	(portable) module ID, or 0 for Kernel
				resID	ID of required string
				size	byte size of return buffer
	Outputs:	buf		result buffer address
	Returns:	0 if failed to load, else number of bytes read
	Purpose:	Loading a string resource, possibly from a .DLL. If modID is non-zero then
				that module is tried first, falling back to the main program if not found.
				Also has to work-around a nasty bug in Win32s LoadStringA. Also recognises
				Tool resource IDs and works out the module automatically (if modID is zero).
				
				Actually when we say 'falling back to the main program' we really mean
				'checking if it's in an external resources dll before falling back to the
				main program'. If no modID is given, we also check the external resource dll
				first. Well, actually that's not TRUE, since if it's a tool resource it's going
				to be in the tools dll rather than the resource only one, so we jump the queue
				a little there...

	Errors:		Returns 0 if not found anywhere.

***********************************************************************************************/

INT32 SmartLoadString(UINT32 modID, UINT32 resID, LPTCHAR buf, INT32 size)
{
	PORTNOTE("other","SmartLoadString - needs to be converted to GetText")
	ERROR3IF(resID == 0, "Zero string resource ID in SmartLoadString");
	ERROR3IF(buf == 0, "No output buffer in SmartLoadString");
	INT32 numchars = size/sizeof(TCHAR);
	ERROR3IF(numchars < 2, "Buffer too small in SmartLoadString");

	camStrncpy( buf, CamResource::GetText(resID), numchars ); // NB GetText cannot fail
	buf[numchars-1]=0; // ensure zero terminated
	return (camStrlen(buf)+1)*sizeof(TCHAR); // return number of bytes copied
}

#if 0
	const HINSTANCE hMain = AfxGetResourceHandle();	// This is the main program
	HINSTANCE hLib  = ExtraDLLs[Resources_DLL];	// This is the resources dll, and will
												//	be 0 (or HINSTANCE_ERROR) if it's not loaded
	if(hLib <= (HINSTANCE)HINSTANCE_ERROR)
		hLib = 0;
	
	HINSTANCE hInst = hMain;

	// if module ID given (and tools.dll present) then use it
	if (modID && DLLs[0] != 0 && DLLs[0] != (HINSTANCE)HINSTANCE_ERROR)
		hInst = OILModule::GetInstance(modID);
	else
	{
		// if no ID given, first have a rummage through the external resource dll (if loaded)
		if(hLib && !(((resID >= EXTERNAL_RESOURCES_START) && (resID < EXTERNAL_RESOURCES_END) )
			|| ( (resID >= VIEWRC_RESOURCES_START) && (resID < VIEWRC_RESOURCES_END))))
		{
			// Note that when using FindResource on strings, the number has to be
			// altered - see PSS Q20011 for further details
			HRSRC hRes = FindResource(ExtraDLLs[Resources_DLL], MAKEINTRESOURCE((resID/16)+1), RT_STRING);

			// if cannot find it then fall back to the built-in resources
			if (hRes != 0)
			{
				INT32 External = ::LoadString(ExtraDLLs[Resources_DLL], resID, buf, size );
				if(External)
					return External;
			}
			else
			{
				TRACEUSER( "Richard", _T("Resource (0x%x) NOT found in external dll\n"), resID);
			}
		}

		// Resource isn't in the external resources DLL...
		// this will return the toolsID or the mainID, or 0 if it's completely confused
		hInst = ResHelp::GetInstanceFromID(resID);
	}

	// We're ok doing this because hLib will be 0 if not loaded
	if((hInst == hMain || hInst == 0) && hLib != 0)
		hInst = hLib;
	if(hInst == 0 && hLib == 0)
		hInst = hMain;

	hInst = hMain;
	INT32 LoadStringResult = ::LoadString( hInst, resID, buf, size );
	if(!LoadStringResult)
	{
		// Temporary bodge until we can fix the string-resource bug.  I'm preventing ERROR3IF's from
		// going off, and filling buffers with safe crud so that calling code won't stiff.
//		ERROR3("Failed to load string resource!");
		if (size > 8) size = 8;
		LoadStringResult = size - 1;
		buf[--size] = TEXT('\0');
		while (size) buf[--size] = TEXT('X');

#if !defined(EXCLUDE_FROM_RALPH) && !defined(EXCLUDE_FROM_XARALX)
		// (Richard) A Trace by popular request
		TRACE( _T("String resource (0x%x, %d) failed to load (searching 0x%x) - GetLastError is 0x%lX\n"),
			resID, resID, hInst, (DWORD)::GetLastError());
#endif
	}
	else
	{
		TRACEUSER( "JustinF", _T("LoadString (#%u, %d chrs): \"%s\"\n"),
					(UINT32) resID, (INT32) nRetVal, (LPCTSTR) buf);
	}

	return LoadStringResult;
#endif



#ifdef _DEBUG
/**************************************************************************************
>	void StringBase::AssertValid() const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22nd April 1993
	Purpose:	MFC required debugging facilities - this checks if the string is valid
				(or rather, doesn't have really stupid data)
	Errors:		ERROR3IF failure if the string's length is negative or if its text
				pointer doesn't point to readable memory, and isn't 0.
***************************************************************************************/
void StringBase::AssertValid() const
{
	CCObject::AssertValid();
	ERROR3IF(length < 0, "StringBase has a negative length!\n");
#if defined( __WXMSW__ )
	ERROR3IF(!text || ::IsBadReadPtr(text, length), "StringBase has bad text pointer!");
#else
	ERROR3IF(!text , "StringBase has bad text pointer!");
#endif	
}
	


	
/**************************************************************************************
>	void StringBase::Dump(CDumpContext& dc) const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22nd April 1993
	Inputs:		A reference to an MFC dump context (a little like an ostream).
	Purpose:	MFC required debugging facilities - this prints on the dump context
				the string and how many references there are to it.
***************************************************************************************/
PORTNOTE("other","Removed StringBase::Dump")
#ifndef EXCLUDE_FROM_XARALX
void StringBase::Dump(CDumpContext& dc) const
{
	CCObject::Dump(dc);
	dc << text;
}
#endif
#endif	// _DEBUG


	
	
/**************************************************************************************
>	istream& operator>>(istream& is, StringBase& s)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22nd April 1993
	Inputs:		A reference to an input stream and a reference to the string whose
				value is to be read.
	Returns:	A reference to the input stream, which enables the chaining of stream
				operations.
	Purpose:	Read a string from a (non-persistent) stream.  The input is	terminated
				by a newline character (NOT whitespace, cf. reading C-style character
				arrays).  If the string is longer than MAX_STRING_RES_LENGTH get() will
				read as many characters as possible and put a newline at the end.
	SeeAlso:	ostream& operator<<(ostream& os, const StringBase& s)
***************************************************************************************/
std::istream &operator>>( std::istream& is, StringBase& s )
{
	ERROR3IF(!s.text, "Call to istream >> for an unALLOCated String");

	char				temp[MAX_STRING_RES_LENGTH];
	is >> std::ws;								// skip any leading whitespace
	is.get(temp, MAX_STRING_RES_LENGTH);	// grab a chunk of input, up to a newline
	
	ERROR3IF( size_t(s.length) <= strlen(temp), "Call to istream >> will overflow String" );
#if wxUSE_UNICODE
PORTNOTE("other","This is not very efficient needs revisiting")
	TCHAR				ptsz[MAX_STRING_RES_LENGTH];
	mbstowcs( ptsz, temp, MAX_STRING_RES_LENGTH );
#else
	s = temp;								// NB. s.operator=(const TCHAR*)
#endif
	return is;
}




/**************************************************************************************
>	ostream& operator<<(ostream& os, const StringBase& s)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22nd April 1993
	Inputs:		A reference to an output stream) and a reference to the constant string
				to write to it.
	Returns:	Reference to the output stream.
	Purpose:	Write a string to a (non-persistent) stream.  If the string is 0 then
				"[0]" is printed.
	SeeAlso:	istream& operator>>(istream& is, StringBase& s)
***************************************************************************************/
std::ostream& operator<<(std::ostream& os, const StringBase& s)
{
	ERROR3IF(!s.text, "Call to ostream << for an unALLOCated String");	
	os << (s.Length() ? s.text : TEXT("[0]"));
	return os;
}


/**************************************************************************************

>	BOOL StringBase::EnableExceptionHandler(BOOL PushNewHandler=TRUE)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/07/96
	Inputs:		PushNewHandler = a default parameter, determining whether a new
								 exception handler state is pushed on the handler stack
								 or not. 
	Returns:	TRUE if throwing exceptions is enabled
				FALSE if a new exception state could not be pushed.
	Purpose:	This function adds a new exception handler to the string exception
				handler stack (if PushNewHandler is TRUE). Otherwise it will simply
				change the state of the most recently pushed handler. If there are no
				handlers and Push is FALSE, an ERROR3IF will be generated.
	Errors:		if an attempt is made to push a new handler when the stack is full
				if an attempt is made to set the current exception handler to 'on' when
				there's no exception handlers on the stack

***************************************************************************************/

BOOL StringBase::EnableExceptionHandler(BOOL PushNewHandler)
{
	if (PushNewHandler) CurrEx++;

	ERROR2IF(CurrEx >= str_MAXEXCEPTIONS, FALSE, "StringBase::EnableExceptions overflow!");
	ERROR2IF(CurrEx < 0, FALSE, "StringBase::EnableExceptions - handler does not exist");

	ThrowExceptionStack[CurrEx]=TRUE;
	return TRUE;
}


/**************************************************************************************

>	void StringBase::DisableExceptionHandler(BOOL PopHandler=TRUE)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/07/96
	Inputs:		PopHandler = a default parameter, determining whether the current
							 handler state is poped or not
	Returns:	TRUE if all goes well
				FALSE if there are no exception states to pop!
	Purpose:	This function disables or completely removes the most previously
				enabled exception state. FALSE will be returned if the exception stack
				is empty. The default parameter allows you to temporarily disable
				the exception state without poping the handler.
	Errors:

***************************************************************************************/

BOOL StringBase::DisableExceptionHandler(BOOL PopHandler)
{
	ERROR2IF(CurrEx < 0, FALSE, "StringBase::DisableExceptions; Nothing on the exception stack!");
	ThrowExceptionStack[CurrEx]=FALSE;
	if (PopHandler) CurrEx--;
	return TRUE;
}



/**************************************************************************************

>	BOOL StringBase::WillThrowException()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/07/96
	Inputs:		-
	Returns:	TRUE if exceptions will currently be thrown
				FALSE if functions will return errors (ie out of range indexes)
	Purpose:	This function interogates the exceptions state stack and determins
				whether exceptions will be thrown or not.

***************************************************************************************/

BOOL StringBase::WillThrowException()
{
	return (CurrEx < 0) ? FALSE : ThrowExceptionStack[CurrEx];
}



/**************************************************************************************

>	void StringBase::ThrowStringException()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/07/96
	Purpose:	Throws a 'User' exception if the string exception stack says 'Yes'

***************************************************************************************/

void StringBase::ThrowStringException()
{
	PORTNOTETRACE("other","StringBase::ThrowStringException - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	if (StringBase::WillThrowException()) AfxThrowUserException();
#endif
}



/**************************************************************************************

>	INT32 StringBase::SkipComma(INT32 pos=0) const

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/07/96
	Inputs:		pos = the character index from which to start
	Returns:	the index of the next none space character after the comma, 
				-1 if none, OR an exception is thrown
				Throws an exception if string exceptions are enabled
	Purpose:	Find the next comma (starting from pos) and skip on to the next
				none space character beyond it.
	Notes:		** Throws an exception if string exceptions are enabled **

***************************************************************************************/

INT32 StringBase::SkipComma(INT32 pos) const
{
	if ((pos = FindNextChar(TEXT(','), pos)) >= 0) pos = FindNextChar(pos + 1);
	return pos;
}


/**************************************************************************************

>	INT32 StringBase::FindNextChar(INT32 position=0) const

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/07/96
	Inputs:		position = the character index from which to start
	Returns:	the index of the next none space character 
				-1 if none, OR an exception is thrown
	Purpose:	Find the next none space character starting from pos. If pos is a
				none space character, the function will simply return with the same
				index.
	Notes:		** Throws an exception if string exceptions are enabled **

***************************************************************************************/

INT32 StringBase::FindNextChar(INT32 position) const
{
	ERROR3IF(!text, "Call to StringBase::FindNextChar for unALLOCated String");

	// get the length of this string in characters	
	INT32 nchars = Length();
	if (!text || position<0 || position>=nchars)
		return IllegalIndex();

	TCHAR c = TEXT(' ');
	while ((text[position]==c) && position<nchars)
		position=NextIndex(position);

	return ((position==nchars) ? (IllegalIndex()) : (position));
}


/**************************************************************************************

>	INT32 StringBase::FindNextChar(TCHAR c, INT32 position=0) const

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/07/96
	Inputs:		c = the character whose index we need
				position = the character index from which to start
	Returns:	the index of the next none space character, 
				-1 if none, OR an exception is thrown
	Purpose:	Find the next character to match c starting from position. 
	Notes:		** Throws an exception if string exceptions are enabled **

***************************************************************************************/

INT32 StringBase::FindNextChar(TCHAR c, INT32 position) const
{
	ERROR3IF(!text, "Call to StringBase::FindNextChar for unALLOCated String");
	INT32 nchars = Length();
	if (!text || position<0 || position>=nchars)
		return IllegalIndex();

	while ((text[position]!=c) && position<nchars)
		position=NextIndex(position);

	return ((position==nchars) ? (IllegalIndex()) : (position));
}

/**************************************************************************************

>	INT32 StringBase::ReverseFind(TCHAR cToFind) const

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>, from Mike
	Created:	2/5/97
	Inputs:		c = the character whose index we need
	Returns:	The index of the last occurrence of character cToFind
				in the string.
	Purpose:	As above
	
***************************************************************************************/

INT32 StringBase::ReverseFind(TCHAR cToFind) const
{
	ERROR2IF(text==0, IllegalIndex(), "Call to StringBase::ReverseFind for unALLOCated String");
	
	INT32 iPosition=Length()-1;

	while ((text[iPosition]!=cToFind) && iPosition>=0)
		iPosition--;

	if (iPosition<0)
		return IllegalIndex();
	else
		return iPosition;
}





/**************************************************************************************

>	INT32 StringBase::ConvertToInteger(INT32& position) const

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/07/96
	Inputs:		position = the character index
	Outputs:	position = the new character index
	Returns:	A signed numeric value for the sequence of characters starting
				at position in this string.
	Purpose:	Convert the sequence of characters starting from position into a numeric
				value. 32 bits of precision are returned and hence approx 10 digit
				numbers can be represented. If the numeric string is longer, the value
				0x7FFFFFFF is returned (ie represting an overflow). If string exceptions
				are enabled, an exception will be thrown in this instance.
				All valid numeric digits will be read, no matter whether an overflow is
				immanent or not, hence you can safely continue to parse what is expected
				to be the next token along the string.
				The types of numbers allowed are
				+123686
				-12394
				124802398
				but not
				12439127397123

	Notes:		** Throws an exception if string exceptions are enabled **

***************************************************************************************/

INT32 StringBase::ConvertToInteger(INT32& position) const
{
	ERROR3IF(!text, "Call to StringBase::ConvertToInteger for unALLOCated String");

	const TCHAR pos = TEXT('+');
	const TCHAR neg = TEXT('-');

	INT32 value   = 0;
	BOOL isneg   = FALSE;
	INT32 maxdigs = 10;
	INT32 nchars  = Length();

	position = SkipSpace(position);
	if (position>-1 && position<nchars)
	{
		isneg = (text[position]==neg);
		if (isneg || text[position]==pos)
			position=NextIndex(position);

		while (position<nchars && IsNumeric(text[position]))
		{
			value = value*10 + (StringBase::TCharToNum(text[position]));
			position=NextIndex(position);
			maxdigs--;
		}
		
		if (maxdigs<0)
		{
			TRACEUSER("Mike", wxT("Overflow during ConvertToInteger") );
			StringBase::ThrowStringException();
			return ((isneg) ? 0x80000000 : 0x7FFFFFFF);
		}

		value = (isneg) ? (-value) : (value);
	}

	return value;
}



//////////////////////////////////////////////////////////////////////////////// TEMP

/**************************************************************************************
>	void StringBase::Serialize(CArchive& arc)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22nd April 1993
	Inputs:		A reference to an MFC archive object.
	Purpose:	Read/write a string to/from a persistent stream, a.k.a. an archive,
				depending upon the value of ar.IsStoring().
***************************************************************************************/
/*
void StringBase::Serialize(CArchive& archive)
{
	ERROR3IF(!text, "Call to String::Serialize for an unALLOCated String");
	CCObject::Serialize(archive);

	INT32 L;
	if (archive.IsStoring())
	{
		L = camStrlen(text);
		archive.Write(&L, sizeof(L));
		archive.Write(text, L * sizeof(TCHAR));
	}
	else
	{
		archive.Read(&L, sizeof(L));
		ERROR3IF(L >= length, "Call to String::Serialize will overflow");
		archive.Read(text, L * sizeof(TCHAR));
	}
}


	
	
// And the rest . . .
IMPLEMENT_SERIAL(StringBase, CCObject, 1);
*/

//////////////////////////////////////////////////////////////////////////////// TEMP


/**************************************************************************************

>	TCHAR* cc_lstrstr(TCHAR *String1, TCHAR *String2)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21st November 1995

	Inputs:		String 1 - String to search in
				String 2 - String to search for

	Returns:	Returns a pointer to the first occurrence of string2 in
				string1, or 0 if string2 does not occur in string1

	Purpose:	Finds the first occurrence of string2 in string1
				This version is case sensitive.
	Notes:		Replaces the Windows API version which seems to be missing. Uses TCHAR's
				instead of char's, and also uses CompareString for multinational support

***************************************************************************************/

const TCHAR *cc_lstrstr(const TCHAR* String1, const TCHAR* String2)
{
	ERROR3IF(String1 == 0 || String2 == 0,
				"0 (zero) parameters passed to cc_lstrstr");

	if (String1 == 0 || String2 == 0) return 0;
	if (!*String2) return String1;
	return camStrstr(String1, String2);

//	TCHAR *cp = String1;
//	TCHAR *s1;
//	TCHAR *s2;

//	while (*cp)
//	{
//		s1 = cp;
//		s2 = String2;

//		// DCBS compatible case sensitive string compare
//		while(*s1 && *s2 && (CompareString(LOCALE_USER_DEFAULT,
//			(NORM_IGNOREKANATYPE | NORM_IGNOREWIDTH),
//			s1, 1, s2, 1) == 2))
//			s1++, s2++;
//		if(!*s2)
//			return(cp);
//		cp++;
//	}
//
//	return(0);
}

/**************************************************************************************

>	TCHAR* cc_lstristr(LPCTSTR String1, LPCTSTR *String2)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21st November 1995

	Inputs:		String 1 - String to search in
				String 2 - String to search for

	Returns:	Returns a pointer to the first occurrence of string2 in
				string1, or 0 if string2 does not occur in string1

	Purpose:	Finds the first occurrence of string2 in string1.
				This version is case insensitive.
	Notes:		Replaces the Windows API version which seems to be missing. Uses TCHAR's
				instead of char's, and also uses CompareString for multinational support

***************************************************************************************/

/*TCHAR* cc_lstristr(LPCTSTR String1, LPCTSTR String2)
{
#if !defined(__WXMSW__)
	return wcsstr( String1, String2 );
#else
	ERROR3IF(String1 == 0 || String2 == 0,
				"0 (zero) parameters passed to cc_lstristr");

	if (String1 == 0 || String2 == 0) return 0;
	if (!*String2) return String1;

	TCHAR *cp = String1;
    TCHAR *s1;
    TCHAR *s2;

	while (*cp)
	{
		s1 = cp;
		s2 = String2;

		// DCBS compatible case insensitive string compare
		while(*s1 && *s2 && (CompareString(LOCALE_USER_DEFAULT,
			(NORM_IGNORECASE | NORM_IGNOREKANATYPE | NORM_IGNOREWIDTH),
			s1, 1, s2, 1) == 2))
			s1++, s2++;

		if(!*s2)
			return(cp);
		cp++;
	}

	return(0);
#endif
}
*/


/**************************************************************************************

>	TCHAR* cc_lstrichr(TCHAR *Str, TCHAR c)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21st November 1995

	Inputs:		Str	- String to search in
				c	- Character to search for

	Returns:	returns pointer to the first occurence of c in string
				returns 0 if c does not occur in string

	Purpose:	Searches a string for a given character, which may be the
				0 character '\0'.
				This version is case insensitive.
	Notes:		Replaces the Windows API version which seems to be missing. Uses TCHAR's
				instead of char's, and also uses CompareString for multinational support

***************************************************************************************/

/*TCHAR* cc_lstrichr(TCHAR *Str, TCHAR c)
{
#if !defined(__WXMSW__)
	return wcschr( Str, c );
#else	
	ERROR3IF(Str == 0, "0 (zero) parameters passed to cc_lstrichr");

	if(Str == 0)
		return 0;

	while(*Str && (CompareString(LOCALE_USER_DEFAULT,
			(NORM_IGNORECASE | NORM_IGNOREKANATYPE | NORM_IGNOREWIDTH),
			Str, 1, &c, 1) != 2))
		Str++;

	if(CompareString(LOCALE_USER_DEFAULT,
			(NORM_IGNORECASE | NORM_IGNOREKANATYPE | NORM_IGNOREWIDTH),
			Str, 1, &c, 1) == 2)
		return(Str);
    
    return(0);
#endif
}
*/
