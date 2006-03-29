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
/* $Log*/
// 

#ifndef IDTOHRESULT_H
#define IDTOHRESULT_H

#include "ralphint.h"
//#include "mario.h"
//#include "andy.h"
//#include "will.h"
//#include "ben.h"
//#include "justin.h"
//#include "tim.h"
//#include "rik.h"
//#include "filtrres.h"
//#include "nev.h"


#ifdef RALPH
// This associates a Error ID with an suitable HRESULT

struct ErrorMap
{
	UINT32 Err;				// Err ID
	HRESULT hr;				// mapped HRESULT
};
ErrorMap ErrorToHRESULT[] =
{

_R(IDE_BADMETAFILE),					RALPH_E_LOADFAILED,
_R(IDE_FILTERDLL_BADLOAD),				RALPH_E_LOADFAILED,
_R(IDE_FILTERDLL_NOIMPORT),				RALPH_E_LOADFAILED,
_R(IDE_MF_NOCREATE),					RALPH_E_LOADFAILED,
_R(IDE_MF_NOMEMLOCK),					RALPH_E_LOADFAILED,
_R(IDE_BADFORMAT),						RALPH_E_BADFILE,
_R(IDE_FILEREADERROR),					RALPH_E_LOADFAILED,
_R(IDE_FORMATNOTSUPPORTED),				RALPH_E_NONSUPPORTEDTYPE,
_R(IDE_CORELPALETTEFILTER_FILEERR),		RALPH_E_BADFILE,
_R(IDE_CORELPALETTEFILTER_BADPALFILE),	RALPH_E_BADFILE,
_R(IDE_CORELPALETTEFILTER_BADCPLFILE),	RALPH_E_BADFILE,
_R(IDE_CORELPALETTEFILTER_NOTCPLORPAL),	RALPH_E_BADFILE,
_R(IDE_CDRFILTER_FORMATERROR),			RALPH_E_BADFILE,
_R(IDE_CDRFILTER_NOTCDR),				RALPH_E_BADFILE,
_R(IDE_LOADCAMELOTEPSITHINKNOT),		RALPH_E_LOADFAILED,
_R(IDE_LOADCAMELOTEPSITHINKNOTNAMED),	RALPH_E_LOADFAILED,
_R(IDE_PALETTEIMPORT_ACOUNKNOWN),		RALPH_E_LOADFAILED,
_R(IDE_CREATE_VIEW_FAILED),    			RALPH_E_INTERNAL,
_R(IDE_DOC_ALREADY_OPEN),				RALPH_E_INTERNAL,
_R(IDE_NOMORE_MEMORY),					RALPH_E_OUTOFMEMORY,
_R(IDE_PATH_ERROR),						RALPH_E_LOADFAILED,
_R(IDE_BUFFER_ERROR),					RALPH_E_LOADFAILED,
_R(IDE_CLOSE_ERROR),					RALPH_E_LOADFAILED,
_R(IDE_OPEN_ERROR),						RALPH_E_LOADFAILED,
_R(IDE_EOF_ERROR),						RALPH_E_LOADFAILED,
_R(IDE_FILEPOS_ERROR),					RALPH_E_LOADFAILED,
_R(IDE_NOTOPEN_ERROR),					RALPH_E_LOADFAILED,
_R(IDE_NULL_FILE),						RALPH_E_LOADFAILED,
_R(IDE_FILE_SIZE_ZERO),					RALPH_E_LOADFAILED,
_R(IDE_NO_DOC_VIEW),					RALPH_E_INTERNAL,
_R(IDE_IO_ERROR),						RALPH_E_LOADFAILED,
_R(IDE_ZOPEN_ERROR),					RALPH_E_LOADFAILED,
_R(IDE_ZFILE_READ_ERROR),				RALPH_E_LOADFAILED,
_R(IDE_ZFILE_WRITE_ERROR),				RALPH_E_LOADFAILED,
_R(IDE_ZFILE_BASEEERROR),				RALPH_E_LOADFAILED,
_R(IDE_ZFILE_ERRORNO),					RALPH_E_LOADFAILED,
_R(IDE_ZFILE_STREAMERROR),				RALPH_E_LOADFAILED,
_R(IDE_ZFILE_DATAERROR),				RALPH_E_LOADFAILED,
_R(IDE_ZFILE_MEMORYERROR),				RALPH_E_OUTOFMEMORY,
_R(IDE_ZFILE_BUFFERERROR),				RALPH_E_LOADFAILED,
_R(IDE_FILE_READ_ERROR),				RALPH_E_LOADFAILED,
_R(IDE_FILE_PERM_DENIED),				RALPH_E_LOADFAILED,
_R(IDE_FILE_BAD_FILE),					RALPH_E_BADFILE,
_R(IDE_FILE_NO_SPACE),					RALPH_E_LOADFAILED,
_R(IDE_FILE_UNKNOWN),	 				RALPH_E_LOADFAILED,
_R(IDE_UNKNOWN_EPSOBJECT),				RALPH_E_LOADFAILED,
NULL, NULL
};

/********************************************************************************************

>	HRESULT GetHRESULTFromID(UINT32 Err)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/7/96				
	Purpose:	Linear search throuh ErrorMap lookup table to find a suitable
				HRESULT for a  given ID
	Input :		A Error resource ID
	Return :	Err->HRESULT
********************************************************************************************/

HRESULT GetHRESULTFromID(UINT32 Err)
{
	// Scan through the whole table.
	for (ErrorMap* pErrMap = ErrorToHRESULT;
		 pErrMap->Err != NULL;
		 pErrMap++)
	{
		// If the Error matches return the HRESULT.
		if ( Err == pErrMap->Err)
		{
			return pErrMap->hr;
		}
	}

	return RALPH_E_UNEXPECTED;
}

#endif
#endif
