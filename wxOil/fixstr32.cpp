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

#include "camtypes.h"
#include "fixstr32.h"


/**************************************************************************************
>	String_32::String_32(const TCHAR* psz = 0)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12th August 1993
	Inputs:		An optional pointer to a (constant) Unicode character array - if
				omitted then the String becomes empty (NOT the null pointer).
	Purpose:	Constructs a string of the appropriate maximum length.
	Errors:		ASSERTion failure if the passed character array is too long for
				this length of string, or if psz is NULL.
	Scope:		Public
***************************************************************************************/

String_32::String_32(const TCHAR* psz)
{
	*(text = fixedbuf) = 0;
	length = FIX_LEN_BUFSIZE;
	if (psz != 0)
	{
		ERROR3IF(camStrlen(psz) >= length, "String_32::String_32 overflow");
		camStrcpy(text, psz);
	}
}



/**************************************************************************************
>	String_32::String_32(const StringBase& other)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12th August 1993
	Inputs:		A constant reference to another string (can be of any length)
	Purpose:	Duplicates the text of the other string.
	Errors:		ASSERTion failure if this string is not long enough to hold a copy
				of the other string's text, or if the copied String has not been
				allocated.
	Scope:		Public
***************************************************************************************/

void String_32::CopyConstruct( const StringBase &other )
{
	*(text = fixedbuf) = 0;
	length = FIX_LEN_BUFSIZE;
	ERROR3IF((const TCHAR*) other == 0, "StringBase to be copied has not been ALLOCated");
	ERROR3IF(camStrlen((const TCHAR*) other) >= length,
				"Constructed String_32 not large enough to hold StringBase copy");
	camStrcpy(text, (const TCHAR*) other);
}




/**************************************************************************************
>	String_32::String_32(UINT32 resID, UINT32 hinst = 0)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12th August 1993
	Inputs:		A string resource numeric identifier, an optional resource module
				handle (if not supplied, the default resource module is used).
	Purpose:	Loads the string resource into this String.
	Errors:		ENSURE failure if this string is not long enough to hold the
				resource, or if the resource cannot be found in the module. If it cannot
				be found, a very short temp string is built of the form "<%X:%X>" containing
				the ID and the module number, or if that is too long then simply "!".
	Scope:		Public
	SeeAlso:	SmartLoadString
***************************************************************************************/

String_32::String_32(UINT32 resID, UINT32 hinst)
{
	*(text = fixedbuf) = 0;
	length = FIX_LEN_BUFSIZE;
	if (!Load(resID, hinst))
	{
		ERROR3("String resource failed to load");
		TCHAR buf[128];
		camSnprintf(buf, 128, TEXT("<%X:%X>"), (INT32) resID, (INT32) hinst);
		camStrcpy(text, (camStrlen(buf) <= 32) ? buf : TEXT("!"));
	}
}



/**************************************************************************************
>	String_32::String_32(const wxString& wxs)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28th January 2006
	Inputs:		A wxString
	Purpose:	Constructs a string of the appropriate maximum length.
	Errors:		ASSERTion failure if the passed character array is too long for
				this length of string, or if psz is NULL.
	Scope:		Public
***************************************************************************************/

String_32::String_32(const wxString& wxs)
{
	*(text = fixedbuf) = 0;
	length = FIX_LEN_BUFSIZE;
	ERROR3IF(wxs.Len() >= length, "String_32::String_32 overflow");
	camStrcpy(text, (LPCTSTR)wxs);
}



/**************************************************************************************
>	virtual BOOL String_32::Alloc(INT32 nSize)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com> & Andy
	Created:	12th August 1993
	Purpose:	Fixes Alloc() for fixed-length strings, which CAN be safely changed#
				to use a buffer on the heap.
	Scope:		Public
	SeeAlso:	StringBase::Alloc
***************************************************************************************/

BOOL String_32::Alloc(INT32 nSize)
{
	if (text == fixedbuf) text = 0;
	return StringBase::Alloc(nSize);
}

