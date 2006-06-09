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
// FileUtil - WinOil level file manipulation library (implementation code)

#include "camtypes.h"
#include "fileutil.h"

#include "camelot.h"
//#include "pathname.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "sgliboil.h"

// Place any IMPLEMENT type statements here
CC_IMPLEMENT_MEMDUMP(FindFiles, CC_CLASS_MEMDUMP);

#define new CAM_DEBUG_NEW

BOOL			FileUtil::SearchActive	= FALSE;
bool			FileUtil::s_fStarted	= false;
String_256		FileUtil::SearchPath	= TEXT("");
wxDir			FileUtil::s_dirSearch;

PORTNOTE("other", "Removed lots of Windows'isms" )
#if !defined(EXCLUDE_FROM_XARALX)

/******************************************************************************

>	static BOOL FileUtil::FindResourceFile(TCHAR* SearchFile, TCHAR* FoundFile)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/5/96
	Inputs:		SearchFile = the name of the file to search for
	Outputs:	FoundFile = the pathname of the first file to match SearchFile.
	Returns:	TRUE if the file has been found
				FALSE if not
	Purpose:	The function searches for a matching file in the following directories
				in the following sequence: 
				 1.	The directory from which the application loaded. 
				 2.	The current directory. 
				 3.	Windows 95: The Windows system directory. 
					Windows NT: The 32-bit Windows system directory.
				 4.	Windows NT only: The 16-bit Windows system directory.
				 5.	The Windows directory. 
				 6.	The directories that are listed in the PATH environment variable. 

******************************************************************************/

BOOL FileUtil::FindResourceFile(TCHAR* SearchFile, TCHAR* FoundFile)
{
	// Try to find the printer.cms file
	BOOL Found = FALSE;
	
	#if WIN16

		// 16 bit Windows - look in the PATH variable and current
		// directory using _searchenv.
		_tsearchenv((char*)SearchFile, "PATH", (char*)FoundFile);
		Found = (FoundFile[0] != '\0');

	#else
		
		// 32-bit Windows - use Win32 API SearchPath, which will look in all kinds 
		// of places, and hopefully should find the 16-bit Windows directory if it's
		// on the user's path, and use the ini file in there.
		LPTSTR FirstChar;
		DWORD BytesRead = ::SearchPath(NULL,          // Use default search paths (see help file)
			   						 SearchFile, 
			   						 NULL,          // Filename already has an extension
			   						 _MAX_PATH - 1, // Destination buffer size
			   						 FoundFile,     // Destination buffer
			   						 &FirstChar);
		
		Found = (BytesRead > 0) && (BytesRead < _MAX_PATH);

	#endif

	return Found;
}

#endif

/********************************************************************************************

>	static BOOL FileUtil::StartFindingFiles(String_256 *FileSpecifier)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/9/96

	Inputs:		FileSpecifier - A full pathname specifying the file(s) to search for.
				You may include ? and * wildcards

				For example,
					c:\Files\Drawings\Blobby.xar	one specific file
					c:\Files\Drawings\*.txt			all .txt files
					c:\Files\Drawings\*.*			all files

	Returns:	TRUE if the scan was successfully started, FALSE otherwise

	Purpose:	Scans a given directory for files matching a particular name.
	
				To find files, do something like the following

				PathName SearchPath("c:\\Directory\\*.*")
				if (FileUtil::StartFindingFiles(&SearchPath))
				{
					String_256 LeafName;
					while (TRUE)
					{
						if (!FileUtil::FindNextFile(&LeafName))
							break;

						// Do something with file "LeafName"
						String_256 FullPathName = "c:\\Directory\";
						FullPathName += LeafName;
						TRACE( _T("Full file path name is %s\n"), (TCHAR *)FullPathName);
					}
					FileUtil::StopFindingFiles();
				}

	Notes:		This search is non-"re-entrant". Between calls to Start & Stop, any calls
				to start another search will give an Error3 and return failure.

	SeeAlso:	FileUtil::FindNextFile; FileUtil::StopFindingFiles

********************************************************************************************/

BOOL FileUtil::StartFindingFiles(String_256 *FileSpecifier)
{
	ERROR3IF(FileSpecifier == NULL, "Duff FileSpecifier param");

	if (SearchActive)
	{
		ERROR3("StartFindingFiles called while already finding files! Call returns failure");
		return(FALSE);
	}

	SearchActive = TRUE;
	s_fStarted	 = false;
	SearchPath = *FileSpecifier;

	return(TRUE);
}



/********************************************************************************************

>	static BOOL FileUtil::FindNextFile(String_256 *FoundFile)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/9/96

	Outputs:	If return value is TRUE, FoundFile will be filled in with
				the LEAF filename ("bob.xar") of a found file - else, it will be
				set to an empty string.

				(I was going to return a PathName - it seemed logical - but....
				An attempt to set
				the location/path ("c:\directory") followed by the leaf-name ("bob.xar")
				resulted in a PathName of "bob.xar.xar" as opposed to "c:\directory\bob.xar".

	Returns:	TRUE if it found a(nother) file matching the filespec.
				FALSE if there are no (more) matching files

	Purpose:	Finds the next file, if any.
				See StartFindingFiles for example search code	

	Notes:		** Note that this will not return system/hidden files or directories **

	SeeAlso:	FileUtil::StartFindingFiles; FileUtil::StopFindingFiles

********************************************************************************************/

BOOL FileUtil::FindNextFile(String_256 *FoundFile)
{
	ERROR3IF(FoundFile == NULL, "Illegal NULL param");

	if (!SearchActive)
	{
		ERROR3("FindNextFile called when StartFindingFiles not called/failed");
		return(FALSE);
	}

	BOOL result = TRUE;

	while (result)
	{
		wxString	strFileName;
		
		if( !s_fStarted )
		{
			// find first
			PathName	path( SearchPath );
			result = wxDir::Exists( path.GetLocation() );
			result = result && s_dirSearch.Open( path.GetLocation() );
			result = result && s_dirSearch.GetFirst( &strFileName, (PCTSTR)path.GetFileName(), wxDIR_FILES );
			s_fStarted = result;
		}
		else
		{
			// find next
			result = s_dirSearch.GetNext( &strFileName );
		}

		if (result)
		{
PORTNOTE("other", "This is part of initial GetFirst" )
#if !defined(EXCLUDE_FROM_XARALX)
			// Only return "files" which are not system or hidden, and which aren't directories!
			const DWORD AttrMask = FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM;

			if ((SearchData.dwFileAttributes & AttrMask) == 0)
#endif
			{
				// OK, it's a safe file to return, so return it
				*FoundFile = (PCTSTR)strFileName;
				return(TRUE);
			}
		}
	}

	*FoundFile = TEXT("");
	return(FALSE);
}



/********************************************************************************************

>	static void FileUtil::StopFindingFiles(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/9/96

	Purpose:	Stops a directory scan
	
				To find files, do something like the following
				See StartFindingFiles for example search code

	Notes:		This code is non-re-entrant. Between calls to Start & Stop, any calls
				to start another search will give an Error3 and return failure.

				DO NOT CALL THIS FUNCTION if StartFindingFiles failed.
				Call this function exactly once for each call to StartFindingFiles.

	SeeAlso:	FileUtil::StartFindingFiles; FileUtil::FindNextFile

********************************************************************************************/

void FileUtil::StopFindingFiles(void)
{
	ERROR3IF(!SearchActive, "StopFindingFiles called when StartFindingFiles not called/failed");

	s_fStarted	 = false;
	SearchActive = FALSE;
}

PORTNOTE("other", "Removed lots of Windows'isms" )
#if !defined(EXCLUDE_FROM_XARALX)


/********************************************************************************************
>	static BOOL FileUtil::GetLongFileName(LPTSTR lpszPath, LPTSTR lpszOutput, size_t cchMaxOutLen)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/8/96
	Inputs:		lpszPath		the full pathname of the file (short, or long)
				cchMaxLen		the size of the output buffer (if not big enough, we WILL 
								fail and return FALSE)
	Outputs:	lpszOutput		contains the full, long, filename of the file
	Returns:	TRUE if successful, FALSE otherwise.
	Purpose:	Given a full 8.3 pathname, extract the full filename.
	Errors:		-
	SeeAlso:	MakeShortPath (which is currently in helpuser.cpp for some bizarre reason...)
********************************************************************************************/

BOOL FileUtil::GetLongFileName(LPTSTR lpszPath, LPTSTR lpszOutput, size_t cchMaxOutLen)
{
	if(lpszPath == NULL || lpszOutput == NULL || cchMaxOutLen < 1)
	{
		ERROR3("FileUtil::GetLongFileName given dodgy params - come on, play the game...");
		return FALSE;
	}

	WIN32_FIND_DATA FileData;

	// Perform a search for the specified file
	HANDLE hSearch = ::FindFirstFile(lpszPath, &FileData);
	if (hSearch == INVALID_HANDLE_VALUE)
	{
		//ERROR3_PF(("FileUtil::GetLongFileName Corresponding long filename version of %s not found", lpszPath));
		return FALSE;
	}

	// The find_data structure will hopefully now contain the long filename for the file
	if(camStrlen(FileData.cFileName) < (INT32)cchMaxOutLen)
	{
		camStrcpy(lpszOutput, FileData.cFileName);
		return TRUE;
	}

	return FALSE;
}


/********************************************************************************************
>	static PathName FileUtil::GetTemporaryFile()

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/5/97
	Inputs:		-
	Outputs:	-
	Returns:	A path to a new temporary file

	Purpose:	Simplified interface to GetTempFileName().

				Call this function when you want to create a new temporary file
				and want a pathname for it.

				The pathname that this function returns will be unique (i.e. will
				not point to an existing file), will begin with 'xar' and will
				be in the system's temporary directory.
								
	Errors:		-
	SeeAlso:	-
********************************************************************************************/
PathName FileUtil::GetTemporaryPathName()
{
	//First get the directory in which to put our temporary files
	TCHAR pcPath[MAX_PATH];

	FileUtil::GetTemporaryPath(MAX_PATH, pcPath);

	//Now get a temporary file name
	TCHAR pcFileName[MAX_PATH];

	FileUtil::GetTemporaryFileName(pcPath, "xar", 0, pcFileName);

	//And convert the file name into a PathName
	String_256 strFileName=pcFileName;
	PathName pthFileName(strFileName);

	//And return it
	return pthFileName;

}


/********************************************************************************************
>	BOOL FileUtil::GetTemporaryPathName(const TCHAR *pExt, PathName *pTempPath)

	Author:		Stefan_Stoykov (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/6/97
	Inputs:		pExt - pointer to the extension for the new file
	Outputs:	pTempPath - a path for a new temporary file
	Returns:	TRUE, if successful, FALSE otherwise
	Purpose:	Simplified interface to GetTempFileName().
				Call this function when you want to create a new temporary file
				with a particular extension.
	Errors:		-
	SeeAlso:	- GetTemporaryPathName()
********************************************************************************************/
BOOL FileUtil::GetTemporaryPathName(const TCHAR *pExt, PathName *pTempPath)
{
	ERROR3IF((pTempPath == NULL) || (pExt == NULL), "NULL param passed to FileUtil::GetTemporaryPathName");
	if ((pTempPath == NULL) || (pExt == NULL))
		return FALSE;

	// create a new temp file 
	*pTempPath = FileUtil::GetTemporaryPathName();

	// if tmp extension is asked - call the relevant function
	if (camStricmp(pExt, (const TCHAR *)"tmp") == 0)
		return TRUE;

	// remember the current name
	String_256 pInFile = pTempPath->GetPath();

	// change the extension
	pTempPath->SetType(pExt);

	// check if the file exists
	if (SGLibOil::FileExists(pTempPath))
	{
		// make a recursive call
		BOOL ok = GetTemporaryPathName(pExt, pTempPath);

		// remove our .tmp file
		_tremove((TCHAR *)pInFile);

		return ok;
	}
	else
	{
		// try to rename the file
		String_256 path = pTempPath->GetPath();
		if (_trename(pInFile,path) != 0)
			return FALSE;
		else
			return TRUE;
	}
}


/********************************************************************************************
>	static UINT32 FileUtil::GetTempFileName(TCHAR *PathName, const TCHAR *Prefix, UINT32 Unique,	
											TCHAR *FileName)

	Author:		Stefan_Stoykov (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/5/97
	Inputs:		pPathName	address of directory name for temporary file 
				Prefix		address of filename prefix 
				Unique		number used to create temporary filename 
	Outputs:	FileName	address of buffer that receives the new filename 
	Returns:	The unique numeric value used in the temporary filenamem, or 0 if unsuccessful.
	Purpose:	Creates a unique file name to be used as temporary file. The format of the file 
				name is   path\preuuuu.TMP, where 'path' is specified by PathName, 'pre' are the 
				first three characters pointed by Prefix, 'uuuu' is the hexadecimal Unique 
				parameter. Pass 0 for Unique to be certain the temp file name is really unique.
	Errors:		-
	SeeAlso:	FileUtil::GetTemporaryDir
********************************************************************************************/
UINT32 FileUtil::GetTemporaryFileName(const TCHAR *PathName, const TCHAR *Prefix, UINT32 Unique, TCHAR *FileName)
{
	if(PathName == NULL || Prefix == NULL || FileName == NULL)
	{
		ERROR3("FileUtil::GetTempFileName given dodgy params - come on, play the game...");
		return FALSE;
	}

	return ::GetTempFileName(PathName,Prefix,Unique,FileName);
}



/********************************************************************************************
>	static INT32 FileUtil::GetTempPath(UINT32 BufferLength,	TCHAR *Buffer)

	Author:		Stefan_Stoykov (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/5/97
	Inputs:		BufferLength	the lenght of the buffer addressed by the second parameter
	Outputs:	Buffer			points to a character buffer, to receive the directory name
	Returns:	The actual length of the returned string
	Purpose:	Gets the temporary directory
	Errors:		-
	SeeAlso:	FileUtil::GetTemporaryFileName
********************************************************************************************/
DWORD FileUtil::GetTemporaryPath(UINT32 BufferLength,	TCHAR *Buffer)
{
	if(Buffer == NULL || BufferLength == 0)
	{
		ERROR3("FileUtil::GetTempPath given dodgy params - come on, play the game...");
		return FALSE;
	}

	return ::GetTempPath(BufferLength, Buffer);
}

#endif

/*******************************************************************************************

>	static BOOL FileUtil::DeleteFile(PathName *FileToRemove)

	Author:		Stefan_Stoykov (Xara Group Ltd) <camelotdev@xara.com> (based on code by Neville Humphrys)
	Created:	11/4/97
	Inputs:		FileToRemove - the path name of the file to be removed.
	Returns:	TRUE if succesful, FALSE otherwise
	Purpose:	Removes a file

*******************************************************************************************/

BOOL FileUtil::DeleteFile(PathName *FileToRemove)
{
	if (FileToRemove == NULL)
		return FALSE;
	
	BOOL Exists = TRUE;
	BOOL status = TRUE;
	
	// check if the file exists
	Exists = SGLibOil::FileExists(FileToRemove);

	// remove it
	if (Exists)
		status = wxRemoveFile( FileToRemove->GetPath() );

	return !status;
}

PORTNOTE("other", "Removed lots of Windows'isms" )
#if !defined(EXCLUDE_FROM_XARALX)

/*******************************************************************************************

>	static BOOL FileUtil::SetCurrentDirectory(const PathName& NewCurrentPath)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/10/97
	Inputs:		NewCurrentPath - the new current path to be used.
	Returns:	TRUE if succesful, FALSE otherwise
	Purpose:	Set up the operating system current directory

*******************************************************************************************/

BOOL FileUtil::GetCurrentDirectory(String_256 * pCurrentPath)
{
	ERROR2IF(pCurrentPath == NULL,FALSE,"FileUtil::GetCurrentDirectory Bad params");

	// Call the OS to get the new current directory
	// DWORD nBufferLength	size, in characters, of directory buffer 
	// LPTSTR lpBuffer		address of buffer for current directory 
	// If the function succeeds, the return value specifies the number of characters
	// written to the buffer, not including the terminating null character. 
	// If the function fails, the return value is zero.
	const INT32 MaxSize = 256;
	TCHAR Buffer[MaxSize];
	DWORD chars = ::GetCurrentDirectory( MaxSize, Buffer);
	if (chars > 0)
	{
		*pCurrentPath = Buffer;
	}
	else
		pCurrentPath->Empty();

	return (chars > 0);
}

/*******************************************************************************************

>	static BOOL FileUtil::SetCurrentDirectory(const PathName& NewCurrentPath)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/10/97
	Inputs:		NewCurrentPath - the new current path to be used.
	Returns:	TRUE if succesful, FALSE otherwise
	Purpose:	Set up the operating system current directory

*******************************************************************************************/

BOOL FileUtil::SetCurrentDirectory(const PathName& NewCurrentPath)
{
	String_256 CurrentPath = NewCurrentPath.GetPath();

	// Call the OS to set the new current directory
	BOOL ok = ::SetCurrentDirectory( (TCHAR *)CurrentPath );
	return ok;
}

/*******************************************************************************************

>	static BOOL FileUtil::SetCurrentDirectory(const String_256& NewCurrentPath)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/10/97
	Inputs:		NewCurrentPath - the new current path to be used.
	Returns:	TRUE if succesful, FALSE otherwise
	Purpose:	Set up the operating system current directory

*******************************************************************************************/

BOOL FileUtil::SetCurrentDirectory(const String_256& NewCurrentPath)
{
	// Call the OS to set the new current directory
	String_256 CurrentPath = NewCurrentPath;
	BOOL ok = ::SetCurrentDirectory( (TCHAR *)CurrentPath );
	return ok;
}


/********************************************************************************************
// FindFiles class
********************************************************************************************/

/********************************************************************************************

>	BOOL FindFiles::StartFindingFiles(String_256 *FileSpecifier)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com> based on Jason code.
	Created:	9/1/97
	Inputs:		FileSpecifier - A full pathname specifying the file(s) to search for.
				You may include ? and * wildcards

				For example,
					c:\Files\Drawings\Blobby.xar	one specific file
					c:\Files\Drawings\*.txt			all .txt files
					c:\Files\Drawings\*.*			all files

	Returns:	TRUE if the scan was successfully started, FALSE otherwise
	Purpose:	Scans a given directory for files matching a particular name.
				Same as FileUtil::StartFindingFiles but being a class is re-entrant
				and returns directories so that you can do recursive searches. 

	SeeAlso:	FindFiles::FindNextFile; FindFiles::StopFindingFiles
	SeeAlso:	FileUtil::StartFindingFiles FileUtil::FindNextFile; FileUtil::StopFindingFiles

********************************************************************************************/

BOOL FindFiles::StartFindingFiles(String_256 *FileSpecifier)
{
	ERROR3IF(FileSpecifier == NULL, "Duff FileSpecifier param");

	m_SearchHandle = INVALID_HANDLE_VALUE;
	m_SearchPath = *FileSpecifier;

	return(TRUE);
}

/********************************************************************************************

>	BOOL FindFiles::FindNextFile(String_256 *FoundFile)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com> based on Jason code.
	Created:	12/9/96
	Outputs:	if return value is TRUE
					FoundFile	will be filled in with the LEAF filename ("bob.xar") of a found
					IsDirectory	will be TRUE if the file is a directory, else FALSE,
				if return value is FALSE
					FoundFile	will be	set to an empty string.
					IsDirectory	will be FALSE.
	Returns:	TRUE if it found a(nother) file matching the filespec.
				FALSE if there are no (more) matching files
	Purpose:	Finds the next file, if any.
				See StartFindingFiles for example search code	
	Notes:		** Note that this will not return system/hidden files **

	SeeAlso:	FindFiles::StartFindingFiles; FindFiles::StopFindingFiles
	SeeAlso:	FileUtil::StartFindingFiles FileUtil::FindNextFile; FileUtil::StopFindingFiles

********************************************************************************************/

BOOL FindFiles::FindNextFile(String_256 *FoundFile, BOOL *IsDirectory)
{
	ERROR3IF(FoundFile == NULL || IsDirectory == NULL, "Illegal NULL param");

	BOOL result = TRUE;

	while (result)
	{
		if (m_SearchHandle == INVALID_HANDLE_VALUE)
		{
			// find first
			m_SearchHandle = ::FindFirstFile((TCHAR *)m_SearchPath, &m_SearchData);
			result = (m_SearchHandle != INVALID_HANDLE_VALUE);
		}
		else
		{
			// find next
			result = ::FindNextFile(m_SearchHandle, &m_SearchData);
		}

		if (result)
		{
			// Only return "files" which are not system or hidden
			const DWORD AttrMask = FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM;
			const DWORD AttrDirMask = FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM;
			TCHAR * ThisDir = ".";
			TCHAR * ParentDir = "..";
			INT32 len = camStrclen(m_SearchData.cFileName);
			if ((m_SearchData.dwFileAttributes & AttrMask) == 0)
			{
				// OK, it's a safe file to return, so return it
				*FoundFile = m_SearchData.cFileName;
				*IsDirectory = FALSE;
				return TRUE;
			}
			else if (((m_SearchData.dwFileAttributes & AttrDirMask) == 0) &&
//					  (cc_lstrncmp(m_SearchData.cFileName, ThisDir, len) != 0) &&
//					  (cc_lstrncmp(m_SearchData.cFileName, ParentDir, len) != 0))
					  (camStrncmp(m_SearchData.cFileName, ThisDir, len) != 0) &&
					  (camStrncmp(m_SearchData.cFileName, ParentDir, len) != 0))
			{
				// OK, it's a safe directory to return, so return it
				*FoundFile = m_SearchData.cFileName;
				*IsDirectory = TRUE;
				return TRUE;
			}
		}
	}

	*FoundFile = TEXT("");
	*IsDirectory = FALSE;
	return FALSE;
}

/********************************************************************************************

>	BOOL FindFiles::StopFindingFiles()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com> based on Jason code.
	Created:	12/9/96
	Returns:	True if completed ok, False otherwise.
	Purpose:	Stops a directory scan
				To find files, do something like the following
				See StartFindingFiles for example search code
				DO NOT CALL THIS FUNCTION if StartFindingFiles failed.
				Call this function exactly once for each call to StartFindingFiles.

	SeeAlso:	FindFiles::StartFindingFiles; FindFiles::FindNextFile
	SeeAlso:	FileUtil::StartFindingFiles FileUtil::FindNextFile; FileUtil::StopFindingFiles

********************************************************************************************/

BOOL FindFiles::StopFindingFiles()
{
	if (m_SearchHandle != NULL)
		::FindClose(m_SearchHandle);

	m_SearchHandle = INVALID_HANDLE_VALUE;

	return TRUE;
}

#endif
