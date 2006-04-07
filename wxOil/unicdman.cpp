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
// 

#include "camtypes.h"
#include "unicdman.h"

#include "camelot.h"

// We want better memory tracking
// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW


/********************************************************************************************
>	static inline BOOL UnicodeManager::IsDBCSLeadByte(BYTE TestChar)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/7/96
	Inputs:		TestChar - the character to test
	Outputs:	-
	Returns:	TRUE if the character is a DBCS lead character, FALSE if not
	Purpose:	Platform independent way of testing for lead bytes.
	Note:		It's inline, in the header
********************************************************************************************/

/********************************************************************************************
>	static inline UINT32 UnicodeManager::ComposeMultiBytes(BYTE LeadChar, BYTE TrailChar)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/7/96
	Inputs:		LeadChar	- Lead multichar byte
				TrailChar	- Trail multichar byte
	Outputs:	-
	Returns:	A composed UINT32 reperesenting the multibyte character
	Purpose:	One standard (correct!) way for composing the lead and trail bytes
	Note:		It's inline, in the header
********************************************************************************************/

/********************************************************************************************
>	static void UnicodeManager::DecomposeMultiBytes(UINT32 MBChar, BYTE* LeadChar, BYTE* TrailChar);

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/7/96
	Inputs:		MBChar	- the composed multibyte character
	Outputs:	LeadChar	- Lead multichar byte
				TrailChar	- Trail multichar byte
	Returns:	-
	Purpose:	One standard (correct!) way for decomposing the lead and trail bytes
********************************************************************************************/
void UnicodeManager::DecomposeMultiBytes(UINT32 MBChar, BYTE* LeadChar, BYTE* TrailChar)
{
	ERROR3IF(LeadChar==NULL || TrailChar==NULL, "Output pointers were NULL");

	if (LeadChar!=NULL || TrailChar!=NULL)
	{
		*LeadChar = (MBChar >> 8) & 0xFF;
		*TrailChar = MBChar & 0xFF;
	}
}



/********************************************************************************************
>	static UINT32 UnicodeManager::UnicodeToMultiByte(WCHAR UnicodeChar)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/10/95
	Inputs:		UnicodeChar - a single unicode character
	Ouptuts:	-
	Returns:	A single multi-byte character value that represents UnicodeChar
	Purpose:	Converts a single unicode character into it's multi-byte equivelent.
				BUT what about two composed characters? Oh...
	Errors:		If the conversion fails then character 128 (box), or the system defualt is
				returned.
********************************************************************************************/
UINT32 UnicodeManager::UnicodeToMultiByte(WCHAR UnicodeChar)
{
#if defined(__WXMSW__)
	if (UnicodeChar < 128)
		return UnicodeChar;
		
	CHAR Return[20];
	LPSTR lpReturn = Return;
	INT32 sizeReturn = sizeof lpReturn;

	// Get the active code page.
	// This is for Win95/98 systems which do not support unicode.
	UINT32 nCodePage = GetACP();
	ERROR3IF( (!IsValidCodePage(nCodePage)), "UnicodeManager::UnicodeToMultiByte - not a valid codepage" );

	// COnvert from Unicode
	if ( ::WideCharToMultiByte(nCodePage, WC_COMPOSITECHECK | WC_DEFAULTCHAR, &UnicodeChar, 1,
								lpReturn, sizeReturn, NULL, NULL) )
	{
		if (IsDBCSLeadByte(Return[0]))
			return ((unsigned char)Return[0] << 8) | (unsigned char)Return[1];
		else
			return (unsigned char)Return[0];
	}
	else
	{
		ERROR3("MultiByteToWideChar failed");
		return 128;
	}
#else
	char				psz[MB_CUR_MAX];
	switch( wctomb( psz, UnicodeChar ) )
	{
	case 1:
		return (unsigned char)psz[0];

	case 2:
		return ( (unsigned char)psz[0] << 8 ) | (unsigned char)psz[1];
	
	case 4:
		printf( "UniCh = %08x\n", ( (unsigned char)psz[0] << 24 ) | 
			  ( (unsigned char)psz[1] << 16 ) | ( (unsigned char)psz[2] << 8 )  | 
				(unsigned char)psz[3] ); 
		return( (unsigned char)psz[0] << 24 ) | ( (unsigned char)psz[1] << 16 ) | 
			  ( (unsigned char)psz[2] << 8 )  |   (unsigned char)psz[3];
	
	default:
		ERROR3("MultiByteToWideChar failed");
		return 128;		
	}
#endif
}


/********************************************************************************************
>	static WCHAR UnicodeManager::MultiByteToUnicode(UINT32 MBChar)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/10/95
	Inputs:		MBChar - a single multi-byte character
	Ouptuts:	-
	Returns:	The unicode value that represents MBChar
	Purpose:	Converts a single multi-byte character into it's unicode equivelent.
				BUT what about two composed characters? Oh...  Unimplemented as no one will
				cope with two chars being returned...
	Errors:		If the conversion fails then character 128 (box) is returned.
********************************************************************************************/
WCHAR UnicodeManager::MultiByteToUnicode( UINT32 MBChar )
{
	CHAR				MBArray[3];
	WCHAR				ReturnArray[4];
	size_t				cch;

	// Convert UINT32 to DBCS
	if( IsDBCSLeadByte( ( MBChar >> 8 ) & 0xFF ) )
	{
		MBArray[0] = (MBChar>>8) & 0xFF;
		MBArray[1] = MBChar & 0xFF;
		MBArray[2] = 0;
		cch = 2;
	}
	else
	{
		MBArray[0] = MBChar & 0xFF;
		MBArray[1] = 0;
		cch = 1;
	}
	
#if defined(__WXMSW__)
	// Get the active code page.
	// This is for Win95/98 systems which do not support unicode.
	UINT32 nCodePage = GetACP();
	ERROR3IF( (!IsValidCodePage(nCodePage)), "UnicodeManager::MultiByteToUnicode - not a valid codepage" );

	// Convert to Unicode
	if ( ::MultiByteToWideChar(nCodePage, MB_PRECOMPOSED, MBArray, -1, ReturnArray, 4) )
	{
		return ReturnArray[0];	// Composed chars then?
	}
	else
	{
		ERROR3("MultiByteToWideChar failed");
		/*DWORD dwError = GetLastError();
		//ERROR3_PF ("Error=%lx", dwError);
		char sError[20];
		sprintf (sError, "Error=%lx", dwError);
		ERROR3 (sError);*/
		return 128;
	}
#else
	mbstate_t			state;
	memset(&state, 0, sizeof(mbstate_t));
	mbrtowc( ReturnArray, MBArray, cch, &state );
	return ReturnArray[0];
#endif
}



/********************************************************************************************
>	BOOL UnicodeManager::IsUnicodeCompleteOS()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/10/95
	Returns:	TRUE if running on a Unicode Complete OS
********************************************************************************************/
BOOL UnicodeManager::IsUnicodeCompleteOS()
{
#if defined(__WXMSW__)
	return IsWin32NT();
#elif defined(__WXGTK__)
	return true;
#endif
}



/********************************************************************************************
>	BOOL UnicodeManager::IsDBCSOS()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/6/99
	Returns:	TRUE if running on a DBCS enabled OS
********************************************************************************************/
BOOL UnicodeManager::IsDBCSOS()
{
#if defined(__WXMSW__)
	return ::GetSystemMetrics(SM_DBCSENABLED);
#elif defined(__WXGTK__)
	return true;
#endif
}



/********************************************************************************************
>	static UINT32 UnicodeManager::GetFontCharSet()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/7/96
	Inputs:		-
	Ouptuts:	-
	Returns:	UINT32 representing the font character set of the system
	Purpose:	Reads what value the system is using for the character sets of its fonts.
********************************************************************************************/
UINT32 UnicodeManager::GetFontCharSet()
{
	if( IsDBCSOS() )
		return SHIFTJIS_CHARSET;
	else
		return ANSI_CHARSET;

//	UINT32 CharSet = 0;
//	
//	// This appears to be the only way of doing this.  It's ugly as we have to create a DC.
//	if (!IsWindows31())
//	{
//		CWnd* pWnd = CWnd::GetDesktopWindow();
//		if (pWnd!=NULL && pWnd->GetDC()!=NULL)
//			CharSet = ::GetTextCharset(pWnd->GetDC()->GetSafeHdc());
//	}
//
//	return CharSet;
}
