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

#include "camelot.h"
#include "pathnmex.h"
//#include "webster.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#if defined(__WXMSW__)
#include <io.h>
#include <direct.h>
#include <process.h>

const TCHAR				chPathSep = _T('\\');
#else
const TCHAR				chPathSep = _T('/');
#endif

/********************************************************************************************

  >	BOOL PathNameEx::CreateLocation()

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/96
	Inputs:		none 
	Return:		TRUE if successful, FALSE otherwise
	Purpose:	Creates the full directory path (as returned by GetLocation()) 
					on the physical medium. This is useful when creating new
					directory structures.
			 
	
********************************************************************************************/

BOOL PathNameEx::CreateLocation()
{
	PORTNOTETRACE("other","PathNameEx::CreateLocation - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	if (!IsValid())
		return FALSE;
	
	// We'll walk the location string from left to right - if we come across non-existent directories,
	// we create them
	String_256			strLocation = GetLocation(FALSE);
	String_256			strDirPath = drivename;
	INT32					nPos =  drivename.Length(); // start after the drivename
	while( nPos < strLocation.Length() )
	{
		while( ( strLocation[nPos] != chPathSep ) && ( nPos < strLocation.Length() ) )
		{
			strDirPath += strLocation[nPos];
			nPos++;
		}
		// strDirPath has been added a directory, we check if it exists
		
		if (_access((TCHAR*) strDirPath, 0) == -1) // not found, try to create the directory
		{
			if (_mkdir((TCHAR*) strDirPath))
			{
#ifdef _DEBUG
				TCHAR szMsg[256];
				TCHAR szError[128];
				switch (errno)
				{
					case EACCES:
						lstrcpy(szError, "access denied (EACCES)");
						break;
					case ENOENT:
						lstrcpy(szError, "path not found (ENOENT)");
						break;
					default:
						wsprintf(szError, "errno = %d", errno);
				}
				wsprintf(szMsg, "Create directory %s failed, %s", strDirPath, szError);
				ERROR3(szMsg);
#endif
				return FALSE;
			}
		}
		strDirPath += chPathSep; // add a backslash in case there are further subdirectories  
		nPos++; // move to the next position
	}
#endif
	return TRUE;
}



/********************************************************************************************

  >	BOOL PathNameEx::RemoveRecursively()

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/96
	Inputs:		none 
	Return:		TRUE if the path is completely deleted, FALSE otherwise (access denied
					or invalid parameters). In case some files cannot be removed, the function will
					do its best to remove the all accessible ones without falling over, unlike the 
					NT system call RMDIR which stops at the first file it can't delete.
	Purpose:	Removes a file or a whole directory tree from the the physical medium (in
					which case, the object should be pointing to the root directory of the tree 
					you want deleted)
			 
	
********************************************************************************************/


BOOL PathNameEx::RemoveRecursively(const String_256& rPath)
{
	PORTNOTETRACE("other","PathNameEx::RemoveRecursively - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	String_256 strFilename(rPath);
	strFilename.toLower();
	// See if the path points to a file (the easy case) or a directory
	if (strFilename[strFilename.Length() - 1] == chPathSep)
	{
		strFilename.Remove(strFilename.Length() - 1, 1);
		goto DIRECTORY;
	}
	struct _stat fileData;
	if (_stat((TCHAR*) strFilename, &fileData))
	{
		if (errno == ENOENT)
		{
			ERROR3("Filename or path not found");
		}
		else
		{
			ERROR3("_stat() failed with an unknown error");
		}
		return FALSE;
	}
	if (fileData.st_mode & _S_IFDIR) // directory
	{
DIRECTORY:
		// Make sure the directory is not the current one
		TCHAR tchbuff[_MAX_PATH];
		if (_getcwd(tchbuff, _MAX_PATH) == NULL)
		{
			ERROR3("Can't get working directory");
			return FALSE;
		}
		if (strstr(_strlwr(tchbuff), (TCHAR*) strFilename))
		{
			// change to upper dir (we should never attempt to delete the root directory!)
			PathName path(strFilename);
			if (_chdir((TCHAR*) String_256(path.GetLocation(FALSE))))
			{
				ERROR3("Can't change directory");
				return FALSE;
			}
		}
		// Try to remove it in the hope that it's empty
		if (_rmdir((TCHAR*) strFilename) == -1)
		{
			if (errno == ENOTEMPTY || errno == EACCES)
			{
				_finddata_t	findData;
				String_256 strSearchPattern(strFilename);
				strSearchPattern += chPathSep;
				strSearchPattern += _T("*"); // add wildcard
				INT32 hSearch = _findfirst(strSearchPattern, &findData);
				if (hSearch == -1)
					return FALSE;
				do
				{
					if (!(strcmp(findData.name, _T(".")) &&  strcmp(findData.name, _T(".."))))
						continue; // skip this directory (.) or its parent (..)
					String_256 strFoundFile(strFilename);
					strFoundFile += chPathSep; 
					strFoundFile += findData.name;
					RemoveRecursively(strFoundFile);
				}
				while (_findnext(hSearch, &findData) == 0);
				_findclose(hSearch);
				return (_rmdir((TCHAR*) strFilename) != -1);
			}
			else
			{
				return FALSE; // probably invalid path
			}
		}
		else
			return TRUE; // succedded
	}
	else if (fileData.st_mode & _S_IFREG) // file
		return (remove((TCHAR*) strFilename) != -1);
	else
#endif
		return FALSE;
}



