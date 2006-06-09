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

// Diagnostic output routines to help in remote end-user debugging



#include "camtypes.h"
#include "diagnost.h"
//#include "app.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "camelot.h"

static INT32 OutputDiagnostics = 0;

/********************************************************************************************

>	BOOL InitDiagnosticPrefs()

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/93
	Returns:	TRUE if preferences retrieved correctly, FALSE otherwise
	Purpose:	To obtain the diagnostics preference from Camelot's ini file.
	SeeAlso:	Main.cpp

********************************************************************************************/

BOOL InitDiagnosticPrefs()
{
	Camelot.DeclarePref(TEXT("DebugFlags"), TEXT("OutputDiagnostics"), 
						&OutputDiagnostics, FALSE, TRUE);

	return TRUE;
}


/********************************************************************************************

>	void CDECL DiagnosticFn(LPCSTR FunctionStr, LPCSTR pszFormat, ...)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/11/93
	Inputs:		FunctionStr: (no space required at end, a space is inserted before being 
							  concatenated)
	`			pszFormat: Format string (no \n required at end)
				arguments
	Outputs:	-
	Returns:	-
	Purpose:	This function is the same as Diagnostic except that it takes an additional 
				function parameter which is concatenated with the format string. 

				*The idea of this function is to save space. The compiler settings will 
				recognise multiple  uses of the same constant string. 

	Errors:		-
	SeeAlso:	Diagnostic

********************************************************************************************/

void CDECL DiagnosticFn(LPCTSTR FunctionStr, LPCTSTR pszFormat, ...)
{
	UINT32					nBuf;
	TCHAR				szBuffer[512];
	const TCHAR		   *pszLocalFormat;

	pszLocalFormat = pszFormat;

	va_list args;
	va_start(args, pszFormat);

	nBuf = camVsnprintf( szBuffer, 512, pszLocalFormat, args );
	
	ASSERT(nBuf < sizeof(szBuffer));

#if 0
	if ((OutputDiagnostics == 1) && (AfxGetApp() != NULL))
		afxDump << AfxGetApp()->m_pszExeName << ": ";

	afxDump << FunctionStr << ": " << szBuffer << "\n";
#else
	String_256 DiagOutput;

	if( OutputDiagnostics == 1 )
	{
		DiagOutput = AfxGetApp().GetAppName().c_str();
		DiagOutput += wxT(": ");
	}

	DiagOutput += FunctionStr;
	DiagOutput += wxT(": ");
	DiagOutput += szBuffer;
	DiagOutput += wxT("\n");

#if defined(__WXMSW__)
	OutputDebugString(DiagOutput);
#elif defined(__WXMSW__)
#pragma error( "OutputDebugString equiv. for Linux" )
#endif

#endif

	va_end(args);
	
}

/********************************************************************************************

>	void CDECL Diagnostic(LPCSTR pszFormat, ...)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/11/93
	Inputs:		pszFormat: Format string (no \n required at end)
				arguments
	Outputs:	-
	Returns:	-
	Purpose:	This function is used in a similar way to TRACE, but exists in retail builds 
				also. It is designed to aid in technical support. It should be used when 
				resources get low, for example, when it might say: "CreatePen failed" or 
				"BitBlt failed", eg,

				if (!SelectPen(blobby))
					Diagnostic("SelectPen %d failed", PenIndex); 

				The strings used will always be in English, must be short & sweet, and must 
				never contain any words you would not say to your mother. 

	Errors:		-
	SeeAlso:	DiagnosticFn

********************************************************************************************/

void CDECL Diagnostic(LPCTSTR pszFormat, ...)
{
	UINT32					nBuf;
	TCHAR				szBuffer[512];
	const TCHAR		   *pszLocalFormat;

	pszLocalFormat = pszFormat;

	va_list args;
	va_start(args, pszFormat);

	nBuf = camVsnprintf( szBuffer, 512, pszLocalFormat, args );
	ASSERT(nBuf < sizeof(szBuffer));

#if 0

	if ((OutputDiagnostics == 1) && (AfxGetApp() != NULL))
		afxDump << AfxGetApp()->m_pszExeName << ": ";

	afxDump << szBuffer << "\n";
#else
	String_256 DiagOutput;

	if( OutputDiagnostics == 1 )
	{
		DiagOutput = AfxGetApp().GetAppName().c_str();
		DiagOutput += wxT(": ");
	}

	DiagOutput += szBuffer;
	DiagOutput += wxT("\n");

#if defined(__WXMSW__)
	OutputDebugString(DiagOutput);
#elif defined(__WXGTK__)
	camPrintf( DiagOutput );
#endif

#endif

	va_end(args);

}

