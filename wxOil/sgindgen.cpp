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
// Gallery index generation classes

/*
*/

#include "camtypes.h"
#include "sgindgen.h"

/*#include "ccfile.h"
//#include "pathname.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "thumb.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "sgliboil.h"
//#include "richard.h"
#include "progress.h"
//#include "resource.h"
//#include "errors.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "fixmem.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#ifndef WEBSTER
#include "extfilts.h"
#include "imglib.h"
#include "img_err.h"
#endif	//WEBSTER
//#include "accures.h"
//#include "oilfltrs.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "resource.h"
#include "sgscanf.h"
//#include "simon.h"		// for _R(IDS_CONTINUE)
//#include "sgscan.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "richard2.h"
//#include "camfiltr.h"	// BaseCamelotFilter - new version 2 native/web filter - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "cxfdefs.h"		// CXF_IDWORD1, signature of new file format - in camtypes.h [AUTOMATICALLY REMOVED]

#include "bmpexdoc.h"
#include "bmpfiltr.h"
#include "prvwflt.h"
//#include "docview.h" - in camtypes.h [AUTOMATICALLY REMOVED]
*/

extern BOOL MakeShortPath(LPTSTR lpszPath, size_t cchMaxLen);

DECLARE_SOURCE("$Revision$");

CC_IMPLEMENT_DYNCREATE(GenerateIndexFile, CCObject)
CC_IMPLEMENT_DYNCREATE(IndGenFileBuffer, CCObject)

#pragma message( __LOCMSG__ "sgindgen.cpp STRIPPED" )
#if 0

#define new CAM_DEBUG_NEW

// Static initialisation

// Preference setting saying whether thumbnail generation is required	
#ifdef _DEBUG
#ifdef _BATCHING
BOOL GenerateIndexFile::CreateFontIndexes = TRUE;
#else
BOOL GenerateIndexFile::CreateFontIndexes = FALSE;
#endif
#else
BOOL GenerateIndexFile::CreateFontIndexes = FALSE;
#endif

// Use old indexes and doc comments for index generation
BOOL GenerateIndexFile::UseOldIndexes = TRUE;
BOOL GenerateIndexFile::UseDocComments = TRUE;

// Use automatic index update technology ?
BOOL GenerateIndexFile::UseAutomaticUpdate = FALSE;

// define this to add filenames without their extensions as default descriptions
//#define FILENAME_AS_DEFAULT_DESCRIPTION

// Display warnings unless told not to
static BOOL QuietThumbnail = FALSE;

// Keep doing index until told not to
static BOOL StopIndGen = FALSE;

static FilePos LastFoundItemInIndex = 0;

// Tmp disk file for index ripping
//CCDiskFile *TmpDiskFile = NULL;

#endif

/***********************************************************************************************

>	GenerateIndexFile::GenerateIndexFile()

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/05/95

	Inputs:		
	Outputs:
	Returns:

	Purpose:	Variable initalising constructor
	Notes:

***********************************************************************************************/

GenerateIndexFile::GenerateIndexFile()
{
	LibType = SGLib_Blank;
	DoThumbnails = FALSE;
	Author = _T("");
	IndexFile = NULL;
	IndexCountOff = 0;
	hSearch = NULL;
	ID = 0;
	TotalFiles = 0;
	TmpIndexFile = NULL;
	OldIndexStart = 0;
	OldIndexDescriptionField = OldIndexKeywordField = OldIndexTitleField = 0;
	OldIndexOK = FALSE;
	OldIndexDescription = "";
//	TmpDiskFile = NULL;
}

#if 0

/***********************************************************************************************

>	BOOL GenerateIndexFile::CloseSearchHandle(void);

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/04/95

	Inputs:		
	Outputs:
	Returns:	TRUE if things went ok.

	Purpose:	Close the search handle
	Notes:

***********************************************************************************************/

BOOL GenerateIndexFile::CloseSearchHandle(void)
{
  	// Close the search handle
	if(hSearch != NULL) {
		if (!FindClose(hSearch)) {
	    	ERROR3("Couldn't close search handle.");
			return FALSE;
		}
		else
		{
			hSearch = NULL;
		}
	}
	return TRUE;
}
	
/***********************************************************************************************

>	BOOL GenerateIndexFile::CloseIndexFileHandle(void);

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/04/95

	Inputs:		
	Outputs:
	Returns:	TRUE if things went ok.

	Purpose:	Close the file handle
	Notes:

***********************************************************************************************/

BOOL GenerateIndexFile::CloseIndexFileHandle(void)
{
	if(IndexFile != NULL)
	{
		IndexFile->close();
		delete IndexFile;
		IndexFile = NULL;
	}
	return TRUE;
}


/***********************************************************************************************

>	static BOOL GenerateIndexFile::IsDirectoryReadOnly(PathName *Directory);

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/04/95

	Inputs:		Directory	- location of directory we're interested in (without trailing \)
	Outputs:
	Returns:	TRUE if things the specified directory is read only

	Purpose:	See Returns...
	Notes:

***********************************************************************************************/

BOOL GenerateIndexFile::IsDirectoryReadOnly(PathName *Directory)
{
	ERROR3IF(Directory == NULL, "GenerateIndexFile::IsDirectoryReadOnly given a NULL dir");

	String_256 ROPathStr(Directory->GetPath(TRUE));
	ROPathStr += TEXT("XaraInf");				// Read only check - not displayed, etc...
	PathName ROPath(ROPathStr);
	Error::ClearError();

	CCDiskFile ROFile(1024, FALSE, TRUE);
	BOOL ReadOnly = TRUE;
	BOOL CreatedFile = FALSE;

	// Exceptions here mean the directory is RO (or there was some other problem)
	TRY
	{
		CreatedFile = ROFile.open(ROPath, ios::out);
	}
		
	CATCH(CFileException, e)
	{
		// The directory with the files in is READ ONLY...
		ReadOnly = TRUE;
		Error::ClearError();
	}
	END_CATCH

	// Make sure that the files are closed and the memory is reclaimed properly...
	if (ROFile.isOpen())
		ROFile.close();

	if(CreatedFile)
	{
		SGLibOil::FileDelete(&ROPath);
		return FALSE;
	}

	return TRUE;
}

/***********************************************************************************************

>	static BOOL GenerateIndexFile::CheckForRemote(PathName *FilesDirectory, String_256 *RemoteDirectory);

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/04/95

	Inputs:		FilesDirectory	- location of directory with files in, on read only media
	Outputs:	RemoteDirectory	- location of existing indexed remote directory
	Returns:	TRUE if we found a remote index directory

	Purpose:	Given a directory on a CD (for example), check whether we've already done
				a remote index for it or not... And return its location.
	Notes:

***********************************************************************************************/

BOOL GenerateIndexFile::CheckForRemote(PathName *FilesDirectory, String_256 *RemoteDirectory)
{
	ERROR3IF(FilesDirectory == NULL || RemoteDirectory == NULL, "GenerateIndexFile::CheckForRemote given NULL args");

	*RemoteDirectory = Library::RemoteIndexLocation;
	BOOL UseTmp = FALSE;

	if(RemoteDirectory->Length()==0)
		UseTmp = TRUE;
	else
	{
		PathName RemLocPath(*RemoteDirectory);
		if(!RemLocPath.IsValid() || !SGLibOil::FileExists(&RemLocPath) || IsDirectoryReadOnly(&RemLocPath))
			UseTmp = TRUE;
	}

	if(UseTmp)
	{
		TCHAR *OSTMP = _tgetenv("TEMP");
		if(OSTMP == NULL)
		{
			//PathName RemLocPath(*RemoteDirectory);

			// Check the specified remote directory isn't read only
			//if(IsDirectoryReadOnly(&RemLocPath) || RemLocPath)
			//{
			*RemoteDirectory = TEXT("");
			return FALSE;
			//}
		}

		camStrcpy((TCHAR *)*RemoteDirectory, OSTMP);
	}

	String_256 TmpDir(FilesDirectory->GetFileName(FALSE));
	String_256 FPath(FilesDirectory->GetPath());

	if(SGLibOil::IsRootDirectory(&FPath))
	{
/*		String_256 Test("\\\\deepfried\\chicken");
		GiveMeAValidDirectoryName(&Test, &TmpDir);
		ERROR3_PF(("%s -> %s", (TCHAR *)Test, (TCHAR *)TmpDir));

		Test = "a:\\";
		GiveMeAValidDirectoryName(&Test, &TmpDir);
		ERROR3_PF(("%s -> %s", (TCHAR *)Test, (TCHAR *)TmpDir));

		Test = "\\fred\\fish";
		GiveMeAValidDirectoryName(&Test, &TmpDir);
		ERROR3_PF(("%s -> %s", (TCHAR *)Test, (TCHAR *)TmpDir));

		Test = "\\\\fred\\fish";
		GiveMeAValidDirectoryName(&Test, &TmpDir);
		ERROR3_PF(("%s -> %s", (TCHAR *)Test, (TCHAR *)TmpDir));

		Test = "\\\\fred\\fish\\fosh";
		GiveMeAValidDirectoryName(&Test, &TmpDir);
		ERROR3_PF(("%s -> %s", (TCHAR *)Test, (TCHAR *)TmpDir));

		Test = "x\\\\fred\\fish\\fosh";
		GiveMeAValidDirectoryName(&Test, &TmpDir);
		ERROR3_PF(("%s -> %s", (TCHAR *)Test, (TCHAR *)TmpDir));

		Test = "\\\\wibble";
		GiveMeAValidDirectoryName(&Test, &TmpDir);
		ERROR3_PF(("%s -> %s", (TCHAR *)Test, (TCHAR *)TmpDir));

		Test = "\\\\wibble\\";
		GiveMeAValidDirectoryName(&Test, &TmpDir);
		ERROR3_PF(("%s -> %s", (TCHAR *)Test, (TCHAR *)TmpDir));

		Test = "\\\\wibble\\\\";
		GiveMeAValidDirectoryName(&Test, &TmpDir);
		ERROR3_PF(("%s -> %s", (TCHAR *)Test, (TCHAR *)TmpDir));

		Test = "\\\\wibble\\ ";
		GiveMeAValidDirectoryName(&Test, &TmpDir);
		ERROR3_PF(("%s -> %s", (TCHAR *)Test, (TCHAR *)TmpDir));*/

		if(!GiveMeAValidDirectoryName(&FPath, &TmpDir))
			return FALSE;
	}

	// Well, we could do it, but it's a little nasty using temp\xarainfo aot temp\something\xarainfo
	if(TmpDir.Length() == 0)
		return FALSE;

	SGLibOil::AppendSlashIfNotPresent(RemoteDirectory);
	*RemoteDirectory += TmpDir;

	PathName RemLocPath(*RemoteDirectory);
	if(RemLocPath.IsValid() && SGLibOil::FileExists(&RemLocPath) && !IsDirectoryReadOnly(&RemLocPath))
		return TRUE;
	else
		return FALSE;
}


/***********************************************************************************************

>	static BOOL GenerateIndexFile::GiveMeAValidDirectoryName(String_256 *Input, String_256 *Output)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/02/96

	Inputs:		Input	- String of random characters -> "\\deepthought\fishcake", "a:\", "hg23~';e#y", etc
	Outputs:	Output	- String which could be used as a directory name -> "deepthou", "a", "hg23ey"
	Returns:	TRUE if things went ok, FALSE if no valid chars were found...

	Purpose:	Given a random string, construct a valid directory name from the string. This is
				smashing if we want a directory name from a root directory, etc...

***********************************************************************************************/

BOOL GenerateIndexFile::GiveMeAValidDirectoryName(String_256 *Input, String_256 *Output)
{
	ERROR3IF(Input == NULL || Output == NULL, "GenerateIndexFile::GiveMeAValidDirectoryName given NULL params");

	if(Input == NULL || Output == NULL)
		return FALSE;

	INT32 Length = Input->Length();
	Output->Empty();
	INT32 StartIndex = 0;

	if(Input->Sub(String_8("\\\\")) == 0 && Input->CountChar('\\') > 2)
	{
		String_256 Tmp;
		Input->Right(&Tmp, Length - 2);

		// Tmp now holds string minus the \\ at the start, so we can get an index to the third \ in the string
		StartIndex = Tmp.Sub(String_8("\\")) + 2;

		if((Length - StartIndex) < 2)
			StartIndex = 0;
	}

	for(INT32 i = StartIndex; i <= Length; i++)
	{
		if(Output->Length() < 8)
		{
			if(iswalnum((*Input)[i])) // != '\\' && (*Input)[i] != ':')
				*Output += TEXT((*Input)[i]); 
		}
	}

	if(Output->Length() > 0)
		return TRUE;

	return FALSE;
}

/***********************************************************************************************

>	BOOL GenerateIndexFile::RemoteIndexSetup(PathName *FilesPath);

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/04/95

	Inputs:		
	Outputs:
	Returns:	TRUE if things went ok and we're using a remote index.

	Purpose:	Check with the user that it's ok to use a remote index, and find a sensible
				location to put it.
	Notes:

***********************************************************************************************/

BOOL GenerateIndexFile::RemoteIndexSetup(PathName *FilesPath)
{
	ERROR3IF(FilesPath == NULL, "GenerateIndexFile::RemoteIndexSetup passed a null path");

	IndexIsRemote = FALSE;
	RemoteLocationOfIndex = "";
	RemoteLocationOfFiles = "";

	if(!Library::RemoteIndexes)
		return FALSE;

	BOOL Existing = CheckForRemote(FilesPath, &RemoteLocationOfIndex);

	if(RemoteLocationOfIndex.Length() == 0)
		return FALSE;

	// Only report this if the user hasn't already found the index for us...
	if(!SGLibOil::FileExists(&Index))
	{
		// "The chosen folder appears to be read only. Would you like to use '#1%s' for the index and thumbnail data? etc..."
	 	String_256 Msg;
		String_256 SmallLocation;
		RemoteLocationOfIndex.Left(&SmallLocation, 100);
	 	Msg.MakeMsg(_R(IDS_LIBRARY_CREATE_REMOTE), (TCHAR *)SmallLocation);
		Error::SetError(0, Msg, 0);
		INT32 ButtonPressed = InformWarning(0, _R(IDS_CREATE), _R(IDS_CANCEL));
		Error::ClearError();

		if(ButtonPressed != 1)
			return FALSE;
	}

	// Pathname for the index file -> c:\clipart\jobby\xarainfo
	PathName TmpPath(RemoteLocationOfIndex);

	if(!SGLibOil::FileExists(&TmpPath))
	{
		// Create the Xarainfo\.. directory
		if (!CreateDirectory((TCHAR *)RemoteLocationOfIndex, NULL))
		{
			DWORD err = GetLastError();		
			ERROR3("Can't create Tmp Index directory");
			return FALSE;
		}
	}

	IndexIsRemote = TRUE;
	SGLibOil::AppendSlashIfNotPresent(&RemoteLocationOfIndex);
	/*if(RemoteLocationOfIndex[RemoteLocationOfIndex.Length()-1] != '\\')
		RemoteLocationOfIndex += TEXT("\\");*/
	RemoteLocationOfIndex += String_16(_R(IDS_LIBRARIES_XARAINFO_DIRNAME));
	RemoteLocationOfFiles = FilesPath->GetPath(FALSE);

	return TRUE;
}


/***********************************************************************************************

>	BOOL GenerateIndexFile::CreateIndexFile(void);

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/04/95

	Inputs:		
	Outputs:
	Returns:	TRUE if things went ok.

	Purpose:	Create the index file and keywords file then add headers to them													   
	Notes:

***********************************************************************************************/

BOOL GenerateIndexFile::CreateIndexFile(void)
{
	if(!GenerateIndexFile::CreateFontIndexes && LibType == SGLib_Font)
		return FALSE;

	String_256 XIPathStr;

	// Check that we have write access to the directory in question...
	if(IsDirectoryReadOnly(&LibPath))
	{
		// The directory with the files in is READ ONLY...
		Error::ClearError();

		if(RemoteIndexSetup(&LibPath))
		{
			XIPathStr = RemoteLocationOfIndex;
			IndexIsRemote = TRUE;

			String_256 NewIndex(RemoteLocationOfIndex);
			SGLibOil::AppendSlashIfNotPresent(&NewIndex);
			/*if(NewIndex[NewIndex.Length()-1] != '\\')
				NewIndex += TEXT("\\");*/
			NewIndex += Index.GetFileName(TRUE);

			Index = NewIndex;
		}
		else
		{
			// Can't find a sensible temp directory, or remote indexes aren't enabled...
			// Tell the user that their media is read only...
			InformWarning(_R(IDS_READONLY_MEDIA), _R(IDS_OK));
			XIPathStr = LibPath.GetPath(TRUE);
			XIPathStr += String_16(_R(IDS_LIBRARIES_XARAINFO_DIRNAME));
			IndexIsRemote = FALSE;
		}
	}
	else
	{
		XIPathStr = LibPath.GetPath(TRUE);
		XIPathStr += String_16(_R(IDS_LIBRARIES_XARAINFO_DIRNAME));
		IndexIsRemote = FALSE;
	}

	/***********************************************************************************/

	// Pathname for the index file -> c:\clipart\jobby\xarainfo
	PathName XIPath(XIPathStr);

	if(!SGLibOil::FileExists(&XIPath))
	{
		// Create the Xarainfo directory
		if (!CreateDirectory((TCHAR *)XIPathStr, NULL))
		{
			DWORD err = GetLastError();		
			ERROR3("Can't create XaraInfo directory");
			return FALSE;
		}
	}

	// This should only be true if we've succesfully found an old index, scanned its header
	// and copied it to and index.bak file
	OldIndexOK = FALSE;

	// If there's already an index file there, make a backup...
	if(SGLibOil::FileExists(&Index) && GenerateIndexFile::UseOldIndexes)
	{
		TmpIndexFile = new PathName(Index); /*(String_256)"C:\\Xaraclip.BAK");*/
		if(TmpIndexFile != NULL)
		{
			TmpIndexFile->SetType("BAK");
			SGLibOil::FileCopy(&Index, TmpIndexFile);

			OldIndexOK = PrePassIndex(TmpIndexFile, &OldIndexStart, &OldIndexDescriptionField,
							&OldIndexKeywordField, &OldIndexTitleField, &OldIndexDescription);
		}
	}

	IndexFile = new CCDiskFile(1024, TRUE, FALSE);
	if(IndexFile == NULL)
	{
		ERROR3("Null index file allocated");
		return FALSE;
	}

	if(!IndexFile->open(Index, ios::out))
	{
		TRACEUSER( "Richard", _T("Can't open index file"));
		delete IndexFile;
		IndexFile = NULL;
		return FALSE;
	}

	return TRUE;
}

/***********************************************************************************************

>	BOOL GenerateIndexFile::DoIndexHeader(void);

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/04/95

	Inputs:		
	Outputs:
	Returns:	TRUE if things went ok.

	Purpose:	Add all the header data to an index file
	Notes:

***********************************************************************************************/

BOOL GenerateIndexFile::DoIndexHeader(void)
{
	String_256 Line(_R(IDS_LIBRARIES_INDEX_FILE_FOR));
	Line += LibPath.GetFileName();
	Line += TEXT("\r\n");
	AddLineToIndex(&Line);

//	_stprintf(Line, "#\r\n# Entry count:\r\n");
//	AddLineToIndex(&Line);

	IndexCountOff = IndexFile->tell();

	ID = 0;
	wsprintf(Line, "%05d", TotalFiles);
	Line += TEXT("\r\n");
	AddLineToIndex(&Line);

//	_stprintf(Line, "# Synonym file:\r\nSynonyms.txt\r\n");
	Line = _R(IDS_LIBRARIES_SYNONYM_FILENAME);
	Line += TEXT("\r\n");
	AddLineToIndex(&Line);

//	_stprintf(Line, "# Thumbnail sizes (Small, Medium, Large):\r\n");
//	AddLineToIndex(&Line);

	// Fonts have weird thumbnail sizes
	if(LibType == SGLib_Font)
		Line = TEXT("28,16, 160,12, 180,26\r\n");
	else
		Line = TEXT("45,45, 130,64, 130,130\r\n");

//		_stprintf(Line, "32,32, 64,64, 128,128\r\n");

	AddLineToIndex(&Line);
	
	// "Filename, Description, ID, Title, Key, Size\r\n"
	Line = String_16(_R(IDS_LIBRARIES_INDEX_ITEM_FILENAME));
	Line += TEXT(", ");
	Line += String_16(_R(IDS_LIBRARIES_INDEX_ITEM_DESC));
	Line += TEXT(", ");
	Line += String_16(_R(IDS_LIBRARIES_INDEX_ITEM_ID));
	Line += TEXT(", ");
	Line += String_16(_R(IDS_LIBRARIES_INDEX_ITEM_TITLE));
	Line += TEXT(", ");
	Line += String_16(_R(IDS_LIBRARIES_INDEX_ITEM_KEY));
	Line += TEXT(", ");
	Line += String_16(_R(IDS_LIBRARIES_INDEX_ITEM_SIZE));
	Line += TEXT("\r\n");
	AddLineToIndex(&Line);

	String_256 Description;
	if(OldIndexDescription.Length() == 0 || !GenerateIndexFile::UseOldIndexes)
	{
		String_256 TmpPath(LibPath.GetPath());
		LibraryFile::TidyUpSubPath(&TmpPath);
		TmpPath.Left(&Description, 60);
	}
	else
		Description = OldIndexDescription;	

	Line = TEXT("#");
	Line += String_16(_R(IDS_LIBRARIES_INDEX_DESCRITION)); // "#DESCRIPTION:"
	Line += TEXT(" ") + Description;
	Line += TEXT("\r\n");
	AddLineToIndex(&Line);

	if(IndexIsRemote)
	{
		Line = _R(IDS_LIBRARIES_INDEX_FILES);
		Line += TEXT(" ") + RemoteLocationOfFiles;
		Line += TEXT("\r\n");
		AddLineToIndex(&Line);
	}

	Line = _R(IDS_LIBRARIES_INDEX_START);
	Line += TEXT("\r\n");
	AddLineToIndex(&Line);

	return TRUE;
}

/***********************************************************************************************

>	BOOL GenerateIndexFile::AddToSubLibrary(void);

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/04/95

	Inputs:		
	Outputs:
	Returns:	TRUE if things went ok.

	Purpose:	This little baby scans a directory for all the files we're expecting and adds
   				sensible dummy entries into the Index file...
	Notes:

***********************************************************************************************/

BOOL GenerateIndexFile::AddToSubLibrary(void)
{
	BOOL Problems = FALSE;
	INT32 Count = FileNameBuffer.ItemCount();
	INT32 i = 0;

	while(i < Count)
	{
		TCHAR *Item = FileNameBuffer.GetItem(i);
		i++;

		if(Item != NULL)
		{
			PathName FileToAdd(Item);

		    if(!AddFile(&FileToAdd))
		    {
				ERROR3("Problems adding file to index or keyword file - or ESC pressed");
				Problems = TRUE;
				i = Count;
		    }
		}
	}

	return !Problems;
}

/***********************************************************************************************

>	INT32 GenerateIndexFile::CountValidFiles(void);

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/04/95

	Inputs:		
	Outputs:
	Returns:	Number of files that have 'add to index' potential for this type of library

	Purpose:	Counts what it returns
	Notes:

***********************************************************************************************/

INT32 GenerateIndexFile::CountValidFiles(void)
{
	// Progress bar for generation status
	String_64 Status(_R(IDS_LIBRARY_CREATION_SCANNING));
	BeginSlowJob(-1, FALSE, &Status);

	INT32 Count = 0;
	BOOL Counting = TRUE;
	BOOL Creating = FALSE;
	INT32 Pass = 0;
	BOOL ok = TRUE;

	String_256 FileSearch((const TCHAR *)LibPath.GetPath(TRUE));	
	FileSearch += TEXT("*.*");

	WIN32_FIND_DATA FileData;
	BOOL fFinished;

	// We go through the below code twice.
	// The first time we just count the number of files, so we know how big to make the
	// array for the second pass...
	do
	{
		Pass ++;
		Counting = (Pass == 1);
		Creating = (Pass == 2);

		fFinished = FALSE;
		hSearch = 0;

		// Start searching for files
		hSearch = FindFirstFile(FileSearch, &FileData);
		if (hSearch == INVALID_HANDLE_VALUE)
		{
		    ERROR3("No valid files found in this directory");
			fFinished = TRUE;
		}

		while (!fFinished)
		{
			// Add full pathname to file
			String_256 FileToAddStr((const TCHAR *)LibPath.GetPath(TRUE));
			FileToAddStr += (TCHAR *)FileData.cFileName;

			// Only use normal files - not directories, hidden files, etc...
			DWORD FileAttributes = GetFileAttributes(FileToAddStr);

			if( ((FileAttributes & FILE_ATTRIBUTE_DIRECTORY) ||
			 	(FileAttributes & FILE_ATTRIBUTE_HIDDEN) ||
			 	(FileAttributes & FILE_ATTRIBUTE_SYSTEM)) == 0)
			{
				PathName FileToAdd(FileToAddStr);

				// Check if the file is addable to the index
				if(IsFileType(&FileToAdd, LibType))
				{
					// First or second pass (first == counting files, second == adding files)
					if(Counting)
						Count ++;

					// Second pass, allocate memory for string and add it into the buffer
					if(Creating)
						FileNameBuffer.AddItem((TCHAR *)FileToAddStr);
				}
			}
		
			// Find the next file
			if (!FindNextFile(hSearch, &FileData))
		    	fFinished = TRUE;
		}

		// Close the search handle
		if(!CloseSearchHandle())
		    ERROR3("Couldn't close search handle.");

		if(Count != 0 && Counting)
		{
			ok = FileNameBuffer.Init(Count);
		}

		if(Count != 0 && Creating)
		{
			ok = FileNameBuffer.Sort();
		}


	} while(Count != 0 && Pass < 2 && ok);

	EndSlowJob();
	return Count;
}

#endif

/***********************************************************************************************

>	IndGenFileBuffer::IndGenFileBuffer()

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/05/95

	Inputs:
	Outputs:
	Returns:

	Purpose:	Constructor
	Notes:		Since MSDOS seems happy returning filenames of files in a directory in a
				completely random fashion, we need to have some way of storing all the ones
				it returns us and sorting them alphabetically, so we can generate index files
				in a nice alphabetical order.

				The data structure used below is basically an array of TCHAR *'s pointed to
				by Buffer. These TCHAR *'s are CCMalloced and given variable length C style
				strings by AddItem. The destructor handles the memory tidying up.

***********************************************************************************************/

IndGenFileBuffer::IndGenFileBuffer()
{
	Buffer = NULL;
	Items = 0;
	Position = 0;
}

/***********************************************************************************************

>	IndGenFileBuffer::~IndGenFileBuffer()

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/05/95

	Inputs:
	Outputs:
	Returns:

	Purpose:	Destructor - frees all the memory
	Notes:

***********************************************************************************************/

IndGenFileBuffer::~IndGenFileBuffer()
{	
	if(Buffer != NULL)
	{
		for(INT32 i = 0; i<Items; i++)
		{
			if(Buffer[i] != NULL)
				CCFree(Buffer[i]);
		}

		CCFree(Buffer);
		Buffer = NULL;
	}
}

#if 0

/***********************************************************************************************

>	BOOL IndGenFileBuffer::Init(INT32 Count)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/05/95

	Inputs:		Count 	- Number of files we're going to be adding
	Outputs:
	Returns:	TRUE if things went ok.

	Purpose:	Initialise the FileName Buffer system with a given number of entries
	Notes:

***********************************************************************************************/

BOOL IndGenFileBuffer::Init(INT32 Count)
{
	ERROR3IF(Buffer != NULL, "IndGenFileBuffer::Init called twice for same object");

	Items = Count;
	Buffer = (TCHAR **)CCMalloc(sizeof(TCHAR *) * Count);

	if(Buffer == NULL)
	{
		ERROR3("IndGenFileBuffer::Init failed to allocate buffer memory");
		Items = 0;
		return FALSE;
	}

	for(INT32 i = 0; i<Items; i++)
	{
		Buffer[i] = NULL;
	}	

	return TRUE;
}

/***********************************************************************************************

>	BOOL IndGenFileBuffer::AddItem(INT32 Count)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/05/95

	Inputs:		String	- pointer to null terminated string of TCHARS which we want to add
	Outputs:
	Returns:	TRUE if things went ok.

	Purpose:	Add a filename to the FileName buffer
	Notes:

***********************************************************************************************/

BOOL IndGenFileBuffer::AddItem(TCHAR *String)
{
	ERROR3IF(Items == 0, "IndGenFileBuffer::AddItem called without initialising first");

	if(Position >= Items)
	{
		ERROR3("IndGenFileBuffer::AddItem called too many times");
		return FALSE;
	}

	BOOL ok = FALSE;

	Buffer[Position] = (TCHAR *)CCMalloc((camStrlen(String) + 1) * sizeof(TCHAR));
	if(Buffer[Position] != NULL)
	{
		camStrcpy(Buffer[Position], String);
		ok = TRUE;
	}

	Position++;

	return ok;
}

/***********************************************************************************************

>	BOOL IndGenFileBuffer::Sort(void)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/05/95

	Inputs:
	Outputs:	
	Returns:	TRUE if the buffer was sorted ok

	Purpose:	Sort the strings pointed to by the buffer alphabetically
	Notes:

***********************************************************************************************/

BOOL IndGenFileBuffer::Sort(void)
{
	qsort(Buffer, Items, sizeof(TCHAR *), IndGenFileBuffer::SortComparator);

	return FALSE;
}

/********************************************************************************************

>	static INT32 __cdecl IndGenFileBuffer::SortComparator(const void *Item1, const void *Item2)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/5/95

	Inputs:		Item1, Item2 - the strings to be compared

	Returns:	a negative, zero, or positive result of comparing the strings

	Purpose:	'qsort' comparator function, used when sorting the file name buffer

	Notes:

********************************************************************************************/

INT32 __cdecl IndGenFileBuffer::SortComparator(const void *Item1, const void *Item2)
{
	if(Item1 == NULL || Item2 == NULL)
	{
		ERROR3("IndGenFileBuffer::SortComparator given null params");
		return 0;
	}

	TCHAR *String1 = *((TCHAR **)Item1);
	TCHAR *String2 = *((TCHAR **)Item2);

	// Sort by name - case insensitive - 'FRED' and 'fred' are equal...
	INT32 Value = CompareString(LOCALE_USER_DEFAULT,
		(NORM_IGNORECASE | NORM_IGNOREKANATYPE | NORM_IGNOREWIDTH),
		String1, -1, String2, -1);	// multi-locale supported compare routine
	Value -= 2;

	return(Value);
}

/***********************************************************************************************

>	TCHAR *IndGenFileBuffer::GetItem(INT32 Count)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/05/95

	Inputs:		Count	- position of item in buffer
	Outputs:	
	Returns:	Pointer to the string at offset 'Count'

	Purpose:	Return pointer to item at index 'Count'
	Notes:		This can return NULL, and will if the item hasn't been allocated properly...

***********************************************************************************************/

TCHAR *IndGenFileBuffer::GetItem(INT32 Count)
{
	ERROR3IF(Count > Position, "IndGenFileBuffer::GetItem given an index which hasn't been added yet");

	return Buffer[Count];
}

/***********************************************************************************************

>	BOOL GenerateIndexFile::FixSubLibraryNumber(void);

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/04/95

	Inputs:
	Outputs:
	Returns:	TRUE if things went ok.

	Purpose:	Poke the count field with the correct value
	Notes:

***********************************************************************************************/

BOOL GenerateIndexFile::FixSubLibraryNumber(void)
{
	if(IndexFile && IndexCountOff)
	{
		IndexFile->seek(IndexCountOff);
		String_256 Line;
		wsprintf(Line, "%05d", ID);
		AddLineToIndex(&Line);
	}
	else
		return FALSE;

	return TRUE;
}


/***********************************************************************************************

>	BOOL GenerateIndexFile::AddFile(PathName *FileName);

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/04/95

	Inputs:		FileName - name of file to add to index
	Outputs:
	Returns:	TRUE if things went ok.

	Purpose:	Add a file to the index
	Notes:

***********************************************************************************************/

BOOL GenerateIndexFile::AddFile(PathName *FileName)
{
	// Update progress bar & Line count
	if(!ContinueSlowJob(ID))
		return FALSE;
	ID++;

	TRACEUSER( "Richard", _T("Adding %s - ID %d to Index file\n"), (const TCHAR *)FileName->GetPath(), ID);

	// Get the file's size
	INT32 Size = (DWORD)SGLibOil::FileSize(FileName);

	// Default filename - force to lower case
	String_256 FName(FileName->GetFileName(TRUE));
	FName.toLower();

	// And capitalise the first character
	TCHAR First = camToupper(((TCHAR *)FName)[0]);
	((TCHAR *)FName)[0] = First;

	// Default description - lowercase filename without file extension
	String_256 Description;

#ifdef FILENAME_AS_DEFAULT_DESCRIPTION
	Description = FileName->GetFileName(FALSE);
	Description.toLower();

	// And capitalise the first character
	First = camToupper(((TCHAR *)Description)[0]);
	((TCHAR *)Description)[0] = First;
#endif

	// Default keywords - none
	String_256 Keywords("");

	// Default title - none
	String_256 Title("");

	BOOL FoundProperDescription = FALSE;
	
	// If it's a ttf font, rip out the name from the file
	if(LibType == SGLib_Font) {

		String_8 Ending(".");
		Ending += FileName->GetType();
		Ending.toLower();
		Ending += ";";

		if(Ending.Sub((String_8)".ttf;") != -1)
		{
			String_256 MenuName;
			if(RipTrueTypeNameFromFile(FileName, &MenuName))
			{
			 	Description = MenuName;
				FoundProperDescription = TRUE;
			}
		}

		if(Ending.Sub((String_8)".pfb;") != -1)
		{
			String_256 MenuName;
			BOOL Bold = FALSE;
			BOOL Italic = FALSE;
			if(RipATMNameFromFile(FileName, &MenuName, &Bold, &Italic))
			{
				// Since this is going in the index, it would be silly to have a comma now wouldn't it ?
				if(Bold || Italic) MenuName += ";";
				if(Bold) MenuName += String_16(_R(IDS_FONTS_CAPITAL_BOLD));
				if(Italic) MenuName += String_16(_R(IDS_FONTS_CAPITAL_ITALIC));

			 	Description = MenuName;
				FoundProperDescription = TRUE;
			}
		}
	}

	// If it's an art / eps file, rip out the comment block
	// NB: The function checks to see if it can handle the file type, no need to do it here...
	// We could also rip authors and keywords out possibly in the future...
	if((LibType == SGLib_ClipArt || LibType == SGLib_ClipArt_WebThemes) && !FoundProperDescription && GenerateIndexFile::UseDocComments)
	{
		String_256 OldDesc(Description);
		String_256 OldKeywords(Keywords);
		String_256 OldTitle(Title);
		if(RipDescriptionFromFile(FileName, &OldDesc, &OldKeywords, &OldTitle))
		{
			Description = OldDesc;
			Keywords = OldKeywords;
			Title = OldTitle;
			FoundProperDescription = TRUE;
		}
	}

	// If we haven't found a name yet, scan the tmp copy of the old index (if there was one)
	// Also get any old keywords found in the index for the item
	if(TmpIndexFile != NULL)
	{
/*	if(!FoundProperDescription && TmpIndexFile != NULL && OldIndexOK && GenerateIndexFile::UseOldIndexes)
	{ */ 
		String_256 OldDesc(Description);
		String_256 OldKeywords(Keywords);
		String_256 OldTitle(Title);

/*		if(((LastFoundItemInIndex - 128) >  	OldIndexStart) && RipDescriptionFromIndex(TmpIndexFile, FileName, LastFoundItemInIndex - 128, 
				OldIndexDescriptionField, OldIndexKeywordField, OldIndexTitleField,
				&OldDesc, &OldKeywords, &OldTitle))
		{
			Description = OldDesc;
			Keywords = OldKeywords;
			Title = OldTitle;
			FoundProperDescription = TRUE;
		}
		else
		{*/
			String_256 FN;
			FN = FileName->GetFileName();
	
			if(RipDescriptionFromIndex(TmpIndexFile, FileName, OldIndexStart, 
					OldIndexDescriptionField, OldIndexKeywordField, OldIndexTitleField,
					&OldDesc, &OldKeywords, &OldTitle))
			{
				Description = OldDesc;
				Keywords = OldKeywords;
				Title = OldTitle;
				FoundProperDescription = TRUE;

				if(Description == Title)
					Description = (String_256)"";

			}
			else
			{
#ifdef _BATCHING
				// If the entry for a .xar file isn't found in the index, check for a .art with
				// the same name, and use that if found...
				String_8 Ending(FileName->GetType());
				Ending.toLower();
				if(Ending.Sub((String_8)"xar") != -1)
				{
					PathName NewFileName(*FileName);
					NewFileName.SetType("ART");
				
					if(RipDescriptionFromIndex(TmpIndexFile, &NewFileName, OldIndexStart, 
							OldIndexDescriptionField, OldIndexKeywordField, OldIndexTitleField,
							&OldDesc, &OldKeywords, &OldTitle))
					{
						Description = OldDesc;
						Keywords = OldKeywords;
						Title = OldTitle;
						FoundProperDescription = TRUE;

						if(Description == Title)
							Description = (String_256)"";
					}
				}
#endif
				//ERROR3("Couldn't find title\keywords for '%s'", (TCHAR *)FN);
			}
	//	}
/*		if(Keywords.Length() == 0)
		{
			String_256 WarnMsg;
			_stprintf(WarnMsg, "Couldn't find keywords for '%s'", (TCHAR *)FN);
			ERROR3(WarnMsg);
		}*/
	}



	// Matt 04/01/2001 - Removing the filename shortening for clipart /web themes gallery index creation code...
	// When it shortens a filename to the 8.3 format, the file can no longer be found as it was left with the original INT32 name
	// The comment below appears unfounded, 'CreateProcess' is not called within Xara... Also, checked (with Derek's help!) for instances of
	// ShellExecute(), thread spawning calls and system calls which may use the INT32 filename incorrectly - couldn't find any...
	/*

  //String_256 ShortPath(FName);
	String_256 ShortPath(FileName->GetPath());

	// Make the path into its "short form", ie. 8.3 MS-DOS compatible.  This is necessary
	// as Windows NT doesn't like paths with spaces, even though it allows them (eg.
	// CreateProcess will fail when parsing the image path / command line).
#ifdef _DEBUG
	ERROR3IF(!MakeShortPath((TCHAR *)ShortPath, _MAX_PATH), "MakeShortPath failed in SGIndGen");
#else
	MakeShortPath((TCHAR *)ShortPath, _MAX_PATH);
#endif


	// Get a filename from a full path string...
	PathName SrtPath(ShortPath);
	ShortPath = SrtPath.GetFileName(TRUE);

	// If we're using a short filename for the filename, and haven't got a title yet,
	// use the full filename as the title 
	if(Title.Length() == 0)
	{
		String_256 ShortPathU(ShortPath);
		String_256 LongPathU(FName);
		ShortPathU.toUpper();
		LongPathU.toUpper();

		if(ShortPathU != LongPathU)
		{
			Title = FName;
		}
	}
*/

	// Just so we don't get any spurious commas...
	if(Description.Length() != 0)
		ReplaceCharacters(&Description, ',', ';');

	if(Title.Length() != 0)
		ReplaceCharacters(&Title, ',', ';');

	if(Keywords.Length() != 0)
		ReplaceCharacters(&Keywords, ',', ';');
	
	String_256 Line;
	wsprintf(Line, "%s, %s, %d, %s, %s, %d\r\n", (TCHAR *)/*ShortPath*/FName, (TCHAR *)Description,
										ID, (TCHAR *)Title, (TCHAR *)Keywords, Size/*, (TCHAR *)Author*/);
	AddLineToIndex(&Line);

	if( DoThumbnails )
		CreateThumbnails(FileName, TRUE);

	if(StopIndGen)
		return FALSE;

	return TRUE;
}

/***********************************************************************************************

>	static BOOL GenerateIndexFile::IsFileType(PathName *FileName, const SGLibType Type);

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/04/95

	Inputs:		FileName - filename and path of file
				Type - type of library
	Outputs:
	Returns:	TRUE or FALSE

	Purpose:	Return true if a file should be associated with the index
	Notes:

***********************************************************************************************/

BOOL GenerateIndexFile::IsFileType(PathName *FileName, const SGLibType Type)
{

	String_8 Ending(".");
	Ending += FileName->GetType();
	Ending.toLower();
	Ending += ";";

	// Ending now has the format '.art;' or whatever

	switch(Type) {
		case SGLib_Font:
			// Include straight truetype fonts
			if(Ending.Sub((String_8)".ttf;") != -1) return TRUE;
			
			// Also include ATM fonts, but check all the required files are present first
			if(Ending.Sub((String_8)".pfb;") != -1)
			{	
				PathName TmpFile(*FileName);
				TmpFile.SetType((String_8)"pfm");
				BOOL PFM = SGLibOil::FileExists(&TmpFile);
				TmpFile.SetType((String_8)"afm");
				BOOL AFM = SGLibOil::FileExists(&TmpFile);
			 	if(PFM && AFM)
			 		return TRUE;
			}
			break;

		case SGLib_ClipArt:
		case SGLib_ClipArt_WebThemes:
			{
				// Speed optimisation for native files
				if( (Ending.Sub((String_8)".art;") != -1)
					|| (Ending.Sub((String_8)".xar;") != -1)
					|| (Ending.Sub((String_8)".cxn;") != -1)
					|| (Ending.Sub((String_8)".cxw;") != -1)
					|| (Ending.Sub((String_8)".web;") != -1) )
					return TRUE;

				// Whizz through all the filters checking if the given file has a standard filter extension
				Filter *pFilter = Filter::GetFirst();

				while (pFilter != NULL)
				{
					FilterFlags Flags = pFilter->GetFlags();

					if ( Flags.CanImport && Flags.ShowFilter )
					{
						// Get the filter string... and compare it with the ending						
						// Note this has the format 'xara studio document|*.art;*.eps'
						// Adding a ';' to the end means we can check an ending of the form '.art;' with the
						// line and it won't return matches for stuff like 'o' files...
						String_256 FilterString(pFilter->pOILFilter->ConstructFilterString());
						FilterString.toLower();
						FilterString += ";";

						if(FilterString.Sub(Ending) != -1)
							return TRUE;
					}

					// Try the next filter
					pFilter = Filter::GetNext(pFilter);
				}

				// This causes random problems later on !
				/*	PathName TmpPath(*FileName);
					INT32 f = AccusoftFilters::GetTypeOfFile(TmpPath);			
					if (f > -1 )
						return TRUE;
				*/

			}
			break;

		case SGLib_Texture:
 		case SGLib_Bitmap:
			{
				Filter *pFilter = Filter::GetFirst();
				while (pFilter != NULL)
				{
					FilterFlags Flags = pFilter->GetFlags();

					if ( Flags.CanImport && Flags.ShowFilter && pFilter->IS_KIND_OF(BitmapFilter))
					{
						// Get the filter string... and compare it with the ending						
						// Note this has the format 'xara studio document|*.art;*.eps'
						// Adding a ';' to the end means we can check an ending of the form '.art;' with the
						// line and it won't return matches for stuff like 'o' files...
						String_256 FilterString(pFilter->pOILFilter->ConstructFilterString());
						FilterString.toLower();
						FilterString += ";";

						if(FilterString.Sub(Ending) != -1)
							return TRUE;
					}

					// Try the next filter
					pFilter = Filter::GetNext(pFilter);
				}
			}
			break;

		case SGLib_Blank:
			return TRUE;

		case SGLib_Fractal:
		case SGLib_Colour:
		default:
			break;
	}

	return FALSE;
}

/***********************************************************************************************

>	INT8 GenerateIndexFile::GetShort(FILE *fp);

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/04/95

	Inputs:		Pointer to file location
	Outputs:
	Returns:	16bit value

	Purpose:	Get a 16bit value from the file
	Notes:

***********************************************************************************************/

INT8 GenerateIndexFile::GetShort(FILE *fp)
{
	INT8 ret;
	ret = (_gettc(fp) << 8);
	ret += _gettc(fp);
	return ret;
}

/***********************************************************************************************

>	INT8 GenerateIndexFile::GetFixed(FILE *fp);

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/04/95

	Inputs:		File pointer for file
	Outputs:
	Returns:	The short

	Purpose:	Return the integer part of a 16.16 fixed only
	Notes:

***********************************************************************************************/

INT8 GenerateIndexFile::GetFixed(FILE *fp)
{
	INT8 ret;
	ret = (_gettc(fp) << 8);
	ret += _gettc(fp);
	_gettc(fp);
	_gettc(fp);
	return ret;
}

/***********************************************************************************************

>	INT32 GenerateIndexFile::GetLONG(FILE *fp);

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/04/95

	Inputs:		A file pointer
	Outputs:
	Returns:	The 32bit INT32 value

	Purpose:	Return a full 32 bit number from a file
	Notes:

***********************************************************************************************/

INT32 GenerateIndexFile::GetLONG(FILE *fp)
{
	INT32 ret;
	ret = (_gettc(fp) << 24);
	ret |= (_gettc(fp) << 16);
	ret |= (_gettc(fp) << 8);
	ret |= (_gettc(fp));
	return ret;
}		


/***********************************************************************************************

>	BOOL GenerateIndexFile::RipTrueTypeNameFromFile(PathName *FileName, String_256 *RetName);

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/04/95

	Inputs:		FileName - filename and path of ttf file
				RetName - returns the truetype name, as found in the file
	Outputs:
	Returns:	TRUE if things went ok.

	Purpose:	Rip out the full typeface name from a font file

	Notes:		This is a bit of a desperate hack, and I'm not totally sure it will work
				with all flavours of TTF file. However, it's worked on all the ones I've
				tried it with.

				As an aside... There's a call 'GetTypeFaceNameFromTTF(FileName, Name, 64)'
				documented in MSDN as in the MS setup suite. Indeed it even appears in the
				header file, but it seems to be missing from their DLL. If this ever reappears
				somewhere, probably best to replace the code below with a proper call !

***********************************************************************************************/

BOOL GenerateIndexFile::RipTrueTypeNameFromFile(PathName *FileName, String_256 *RetName)
{
#ifdef _DEBUG

	if(FileName == NULL || RetName == NULL)
	{
		ERROR3("GenerateIndexFile::RipTrueTypeNameFromFile given null params - bad !");
		return FALSE;
	}

 	BOOL ok=TRUE;
	BOOL found = FALSE;
		
	FILE *file = NULL;
						   
	file = _tfopen((const TCHAR *)FileName->GetPath(), "rb");
	if(file) {

		/**********************/

		INT8 version =  GetFixed(file);
		UINT8 NumTables = GetShort(file);

		GetShort(file);
		GetShort(file);
		GetShort(file);	 

		/**********************/

		TCHAR tag[8] = "";
		static INT32 checksum = 0;
		static INT32 Noffset = 0;
		static INT32 length = 0;

		static INT32 NameOffset = 0;
		
		INT32 i;
					 
		for(i=1; i<=NumTables; i++) {

			//fread(tag, sizeof(char), 4, file);

			tag[0]		= _gettc(file);
			tag[1]		= _gettc(file);
			tag[2]		= _gettc(file);
			tag[3]		= _gettc(file);
			tag[4]		= 0;
			
			checksum 	= GetLONG(file);
			Noffset 	= GetLONG(file);
			length 		= GetLONG(file);

			if(feof(file)) {
				DWORD Z = GetLastError();
				ERROR3("eof returned in ttf file");
			}

			// name
			if(!(camStrcmp(tag,"name"))) {
				NameOffset = Noffset;
			} 
			
		}

		/**********************/

		// OK, so we've looked through all the tags, now decode the name block
		if(NameOffset != 0) {

			if(fseek(file, NameOffset, SEEK_SET)) {
				ERROR3("Can't find TTF name table");
				fclose(file);
				return FALSE;
			}

			UINT8 FormatSelect = GetShort(file);
			UINT8 NumRecords   = GetShort(file);
			UINT8 StringStore  = GetShort(file);

			UINT8 PlatformID;
			UINT8 PlatformSpecID;
			UINT8 LangID;
			UINT8 NameID;
			UINT8 StrLen;
			UINT8 StrOff;

			UINT8 family_strlen = 0;
			UINT8 family_stroff = 0;

			for(i=1; i<=NumRecords; i++) {
					
				PlatformID      = GetShort(file);
				PlatformSpecID  = GetShort(file);
				LangID          = GetShort(file);
				NameID          = GetShort(file);
				StrLen          = GetShort(file);
				StrOff          = GetShort(file);

				// Currently we only bother with the Macintosh name, as it's in ASCII not UNICODE !
				if(NameID == 4 && PlatformID == 1) {

				//	printf("We've got a family name\n");

					family_strlen = StrLen;
					family_stroff = StrOff;
				
					fpos_t pos;

					fgetpos(file, &pos);
					if(fseek(file, family_stroff + StringStore + NameOffset, SEEK_SET)) {
					
						ERROR3("String in name table past end of file in TTF file.");
					
					} else { 

						if(family_strlen >= 255) family_strlen = 255-1;

						TCHAR Name[255];

						// Get typeface name from file - +1 for the terminator...
						_fgetts(Name, family_strlen+1, file);
						found = TRUE;

						camStrcpy((TCHAR *)*RetName, Name);
				
						TRACEUSER( "Richard", _T("Font name = %s\n"), Name);
						TRACEUSER( "Richard", _T("PlatID %d, SpecID %d, LangID %d\n"), PlatformID, PlatformSpecID, LangID);
					}
					fsetpos(file, &pos);						 
				}
			}

			if(!found)
				ERROR3("Can't find TTF family name");

		}

		/**********************/

		fclose(file);
	}
				   
	return found;

#endif

	return TRUE;
}

/***********************************************************************************************

>	BOOL GenerateIndexFile::RipATMNameFromFile(PathName *FileName, String_256 *RetName, BOOL *Bold, BOOL *Italic);

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/10/95

	Inputs:		FileName 	- filename and path of pfm/pfb files
	Outputs:	RetName 	- returns the typeface name, as should be displayed in menus...
				Bold		- true if the file is bold
				Italic		- true if the file is italic
	Returns:	TRUE if things went ok.

	Purpose:	Rip out the ATM name to display in a menu for a given set of pfb/pfm files. Also
				aquire the bold / italic status of the files...

	Notes:		As with the TTF function, this is a bit of a desperate hack, and I'm not totally
				sure it will work with all flavours of ATM file. However, it's worked on all the
				ones I've tried it with.

***********************************************************************************************/

BOOL GenerateIndexFile::RipATMNameFromFile(PathName *FileName, String_256 *RetName, BOOL *Bold, BOOL *Italic)
{
#ifdef _DEBUG

	ERROR3IF(FileName == NULL || RetName == NULL || Bold == NULL || Italic == NULL, "Null params given to ATM ripper");

	// The basic idea behind doing this is to scan the pfm file for 'PostScript<0>', use the next
	// bit as the menu name, then check the last bit for flags...
	//			PostScript<0>Aldine401 BT<0>Aldine401BT-BoldA<0>

	BOOL ok = TRUE;

	*RetName = TEXT("");
	*Bold = FALSE;
	*Italic = FALSE;

	PathName PFMPath(*FileName);
	PFMPath.SetType((String_8)"PFM");

	if(!SGLibOil::FileExists(&PFMPath))
	{
		ERROR3("Can't find PFM file");
		return FALSE;
	}

	CCDiskFile PFMFile(1024, FALSE, TRUE);

	TRY
	{
		// Open the xxx.pfm file
		ok = PFMFile.open(PFMPath, ios::in | ios::nocreate | ios::binary);
	}	
			
	CATCH(CFileException, e)
	{
		Error::ClearError();
		return FALSE;
	}
	END_CATCH

	FilePos Position = 100; //199;
	BOOL FoundPostScript = FALSE;
	PFMFile.seekIn(Position);
		
	TRY
	{
		// Keep looping through the file until we hit the end...
		while(!PFMFile.eof() && !FoundPostScript)
		{

			DWORD Buffer[256];
			memset(Buffer, 0, 256 * 4);
			PFMFile.seekIn(Position);

			if(!FoundPostScript)
			{
				PFMFile.read((void *)Buffer, 11);

				if(!camStrcmp(((TCHAR *)Buffer) , "PostScript"))	// Debug
				{
//					ERROR3("Found postscript");
					FoundPostScript = TRUE;
					
					TCHAR Typeface[256];
					INT32 Count = 0;

					do {
						PFMFile.read((void *)Buffer, 1);
						Typeface[Count] = ((TCHAR *)Buffer)[0];
					} while(Typeface[Count++] != 0 && !PFMFile.eof());

					TCHAR Styles[256];
					Count = 0;

					do {
						PFMFile.read((void *)Buffer, 1);
						Styles[Count] = ((TCHAR *)Buffer)[0];
					} while(Styles[Count++] != 0 && !PFMFile.eof());

					String_256 StyleStr;
					StyleStr = (TCHAR *)Styles;

					// ALL BELOW STRINGS ARE DEBUG ONLY...

					// Major bodge here... If light, but medium, then we're bold... Erm... got that ? No ? Me neither !
					if(StyleStr.Sub((String_8)"-Medium") != -1)
					{
						String_256 TypeString(Typeface);
						if(TypeString.Sub((String_8)"Lt") != -1)
							*Bold = TRUE;
					}

					if( StyleStr.Sub((String_8)"-Bold") != -1
						|| StyleStr.Sub((String_8)"-Semi") != -1
						|| StyleStr.Sub((String_8)"-Black") != -1
						|| StyleStr.Sub((String_8)"-Demi") != -1
						|| StyleStr.Sub((String_8)"-Heavy") != -1)
						*Bold = TRUE;

					// Some names seem to get truncated randomly if BOLDITALIC...
					if( StyleStr.Sub((String_8)"Ital") != -1
						|| StyleStr.Sub((String_8)"BoldIt") != -1
						|| StyleStr.Sub((String_8)"DemiIt") != -1
						|| StyleStr.Sub((String_8)"SemiIt") != -1
						|| StyleStr.Sub((String_8)"HeavyIt") != -1
						|| StyleStr.Sub((String_8)"BlackIt") != -1)
						*Italic = TRUE;

					StyleStr = (TCHAR *)Typeface;
					*RetName = StyleStr;

					// Now hack out the rest
				}
			}

			Position ++;
		}
	}

	// See if there was a file io error
	CATCH(CFileException, e)
	{
		// Something rather unhelpful has taken place, or we've reached the end of the file......
		ok = FALSE;

		// remove any errors
		Error::ClearError();
	}
	END_CATCH

	// Make sure that the files are closed and the memory is reclaimed properly...
	TRY
	{
		if (PFMFile.isOpen())
			PFMFile.close();
	}
	CATCH(CFileException, e)
	{
		// Failed to close the files - not much we can do about it really
		Error::ClearError();
	}
	END_CATCH


#endif

	return TRUE;
}

/***********************************************************************************************

>	static BOOL GenerateIndexFile::PreCreateThumbnail(PathName *InFile);

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	06/09/96

	Inputs:		InFile	- Input path and filename
	Outputs:
	Returns:	TRUE if we can generate a thumbnail without exploding.

	Purpose:	This will check that creating the thumbnail won't cause us to go BANG !
		
				Currently if you give Accusoft a 32BPP thumbnail, all hell breaks loose on win95

***********************************************************************************************/

BOOL GenerateIndexFile::PreCreateThumbnail(PathName *InFile)
{
	if(InFile == NULL)
		return FALSE;

	String_8 Ending(InFile->GetType());
	Ending.toLower();

	// Check for nonstandard bmp bpps
	if(Ending.Sub((String_8)"bmp") != -1)
	{
		BOOL NiceHappyBpp = FALSE;

		FILE *pFile = fopen((const TCHAR *)InFile->GetPath(), "rb");
		if(pFile)
		{
			INT32 b = fgetc(pFile);
			INT32 m = fgetc(pFile);

			// Check file header really is a bmp file
			if(b == 'B' && m == 'M')
			{
				DWORD BppOffset = sizeof(BITMAPFILEHEADER) + 4 + 4 + 4 + 2;
				
				if(!fseek(pFile, BppOffset, SEEK_SET))
				{
					INT32 bpp = fgetc(pFile);
					bpp += (fgetc(pFile) << 8);

					// These are the formats Accusoft support...
					if(bpp == 1 || bpp == 4 || bpp == 8 || bpp == 24)
						NiceHappyBpp = TRUE;
				}
			}
			fclose(pFile);
		}
		return NiceHappyBpp;
	}

	return TRUE;
}


/***********************************************************************************************

>	BOOL GenerateIndexFile::CreateThumbnail(PathName *InFile, PathName *OutFile, UINT8 XSize, UINT8 YSize);

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/04/95

	Inputs:		InFile	- Input path and filename
				OutFile	- Output path and filename
				XSize	- Width of rectangle to scale image to
				YSize	- Height of rectangle to scale image to
	Outputs:
	Returns:	TRUE if things went ok.

	Purpose:	Given an input filename and output filename, generate a thumbnail to fit in the
				rectangle described.
				
	Notes:		Aspect ratio is maintained, and the output thumbnail is guaranteed not to exeed the
				dimensions given...

***********************************************************************************************/

BOOL GenerateIndexFile::CreateThumbnail(PathName *InFile, PathName *OutFile, UINT8 XSize, UINT8 YSize)
{
	// We can't generate font thumbnails yet
	if(LibType == SGLib_Font)
		return FALSE;

	// Now you're just being silly...
	if(XSize == 0 || YSize == 0)
		return FALSE;

	// Pre explosion check
	if(!PreCreateThumbnail(InFile))
		return FALSE;

//#ifdef WEBSTER

	// create a disk file
	CCDiskFile TempDiskFile(1024, FALSE, TRUE);
	
	// open it for reading
	BOOL ok = TempDiskFile.open(*InFile, ios::in | ios::binary);

	// Make sure we have a generic filter!
	GenericFilter *pGenericFilter = Filter::GetGenericFilter();
	if (ok)	ok = (pGenericFilter != NULL);
	if (ok)
	{
		// import the file into a kernel bitmap

		// remember the file size
		UINT32 FileSize = TempDiskFile.Size();

		// find the best filter for the import
		Filter *pImportFilter = pGenericFilter->GetBestFilter(&TempDiskFile);
	
		if (pImportFilter == NULL) 
			return FALSE;  // no filter capable of importing the file
		
		// Find the png thumbnail filter for export
		Filter *pFilter = Filter::GetFirst();
		while (pFilter != NULL)
		{
			if (IS_A(pFilter,ThumbnailFilterPNG))
				// This is the filter!
				break;

			// Try the next filter
			pFilter = Filter::GetNext(pFilter);
		}

		if (pFilter == NULL)
			return FALSE;  // filter not found

		// get the bitmap filter
		ThumbnailFilterPNG *pPNGFilter = (ThumbnailFilterPNG *)pFilter;
		
		INT32 PixelSize = 72000 / 96;	// Size of output pixel in millipoints

		// remember the preview size
		INT32 OldPreviewSize = PreviewFilter::PreviewBitmapSize;
		
		// Set the Preview to be just over an inch accross
		PreviewFilter::PreviewBitmapSize = (XSize > YSize ? XSize : YSize) * PixelSize;

		// remember the current and the selected documents
		Document *pOldCurrent = Document::GetCurrent();
		Document *pOldSelected = Document::GetSelected();
		
		// create a new document, needed for the bitmap import
		Document *pDoc = new Document(TRUE);
		if (pDoc == NULL)
			ok = FALSE;
		else
		{
			pDoc->Init(0);

			// create an operation (for the export)
			SelOperation *pOp = new SelOperation;
			if (pOp == NULL)
				ok = FALSE;
			else
			{
				// set our document as selected
				Document::SetSelectedViewAndSpread(pDoc, NULL, pDoc->FindFirstSpread());

				if (pImportFilter->IS_KIND_OF(BaseBitmapFilter))
				{
					KernelBitmap *pKernelBitmap = NULL;

					// bitmap filter, so simply import the bitmap
					ok = pImportFilter->ImportBitmap(&TempDiskFile, &pKernelBitmap);

					// close the file 
					if (TempDiskFile.isOpen())
						TempDiskFile.close();

					//create a new bitmap export document
					BitmapExportDocument *pBmpDoc = new BitmapExportDocument();
					if (pBmpDoc != NULL)
					{
						// set the bitmap size
						DocRect RectToExport;
						RectToExport.lo.x = 0;
						RectToExport.lo.y = 0;
						RectToExport.hi.x = XSize * PixelSize;
						RectToExport.hi.y = YSize * PixelSize;

						// pass the bitmap to the document
						pBmpDoc->Init(pKernelBitmap, RectToExport);

						if (ok) ok = pPNGFilter->DoExport(pOp, &TempDiskFile, OutFile, pBmpDoc, TRUE);

						// get rid of the document
						delete pBmpDoc;
					}
					else
						ok = FALSE;

					// get rid of the bitmap
		//			if (pKernelBitmap != NULL)
		//				delete pKernelBitmap;
				}
				else
				{
					// web/xar file, so try to import the file

					// remember the current view
					View *pOldView = View::GetCurrent();

					// create a new view 
					DocView *pView = new DocView(pDoc);
					if (pView && pView->Init())
					{
						// set the view as current (the import function requires a current view)
						pView->SetCurrent();

						// import the file into the document
						ok = pImportFilter->DoImport(pOp,&TempDiskFile,pDoc);

						delete pView;
					}

					// restore the last view
					if (pOldView != NULL)
						pOldView->SetCurrent();
					else
						View::SetNoCurrent();

					// close the file 
					if (TempDiskFile.isOpen())
						TempDiskFile.close();

					// export the document into the OutFile
					if (ok) ok = pPNGFilter->DoExport(pOp, &TempDiskFile, OutFile, pDoc, TRUE);
				}

				// delete the created operation
				delete pOp;
			}

			// restore the previous current document ...
			if (pOldCurrent != NULL)
				pOldCurrent->SetCurrent();
			else
				Document::SetNoCurrent();

			//... and the selected one together with its view and spread
			if (pOldSelected != NULL)
				Document::SetSelectedViewAndSpread(pOldSelected);
			else
				Document::SetNoSelectedViewAndSpread();

			// delete our document
			delete pDoc;
			pDoc = NULL;
		}

		// Set the Preview back to its default setting
		PreviewFilter::PreviewBitmapSize = OldPreviewSize;
	}

	// close the file 
	if (TempDiskFile.isOpen())
		TempDiskFile.close();
//	return FALSE;


//#else	//WEBSTER

#if FALSE

	// Check Accusoft dll is loaded...
	static WarnAboutAccusoft = TRUE;
	if (AccusoftFilters::GetVersionNumber() <= 0)
	{
#ifdef _DEBUG
		if(WarnAboutAccusoft)
		{
			// tell the user there was a problem, and give them a skip button so we don't
			// whinge in the future...
		 	String_256 Msg("Accusoft DLL not found, can't generate thumbnails"); // debug only
			Error::SetError(0, Msg, 0);
			INT32 ButtonPressed = InformWarning(0, _R(IDS_SKIP), _R(IDS_SILENT));
			Error::ClearError();

			if(ButtonPressed == 2)
				WarnAboutAccusoft = FALSE;
		}
#endif
		return FALSE;
	}

	TRACEUSER( "Richard", _T("About to do %s (%d)"), (const TCHAR *)InFile->GetPath(), SGLibOil::FileSize(InFile));

	BOOL ok = FALSE;

	// Read the header info from the input file
	void *Fish = CCMalloc(sizeof(BITMAPINFOHEADER) + 256 * 4);
	BITMAPINFOHEADER *header = (BITMAPINFOHEADER *)Fish;
	
	char FileWithPath[256];
	camStrcpy(FileWithPath, (const TCHAR *)InFile->GetPath());
	
	INT32 z = AccusoftFilters::pfnIMGLOW_get_fileinfo(FileWithPath, header);
	if(z<0)
	{	
#ifdef _DEBUG
		// This is only really a 'can't get the thumbnail for this file' error...
		if(!QuietThumbnail)
		{
			String_256 Msg;
			wsprintf(Msg, "GenIdxFl::CreateThumbnail GetFileInfo - '%s' (0x%x '", (const TCHAR *)InFile->GetPath(), z);
			String_256 bodge(_R(IDE_ACCUSOFT_ERROR) - z);
			Msg += bodge;
			Msg += TEXT("')");
			ERROR3(Msg);
		}
#endif
		CCFree(Fish);
	}
	else
	{
		// Using the header info, work out what dimensions the output bmp needs to be
		INT32 BmpX = 0;
		INT32 BmpY = 0;

		BmpX = header->biWidth;
		BmpY = header->biHeight;

		CCFree(Fish);

		double XScale = (double)BmpX / (double)XSize;
		double YScale = (double)BmpY / (double)YSize;

		double Scale = (double)1;
		if(XScale > YScale)
			Scale = XScale;
		else
			Scale = YScale;
		if(Scale <= (double)0)
			Scale = (double)0.1;

		XScale = (double)BmpX / (double)Scale;
		YScale = (double)BmpY / (double)Scale;

		INT32 Xscale = (INT32)XScale;
		INT32 Yscale = (INT32)YScale;

		// Set the output size
		INT32 x = AccusoftFilters::pfnIMGLOW_set_decompsize(Xscale, Yscale);
		if(x<0)
		{
#ifdef _DEBUG
			String_256 Msg;
			_stprintf(Msg, "GenIdxFl::CreateThumbnail SetDecompSize - '%s' (0x%x '", (const TCHAR *)InFile->GetPath(), x);
			String_256 bodge(_R(IDE_ACCUSOFT_ERROR) - x);
			Msg += bodge;
			Msg += TEXT("')");
			ERROR3(Msg);
#endif
		}
		else
		{
			// Decompress the actual bitmap
			INT32 imghandle = AccusoftFilters::pfnIMG_decompress_bitmap(FileWithPath);
			if(imghandle<0)
			{
#ifdef _DEBUG
				String_256 Msg;
				_stprintf(Msg, "GenIdxFl::CreateThumbnail DecompBitmap - '%s' (0x%x '", (const TCHAR *)InFile->GetPath(), imghandle);
				String_256 bodge(_R(IDE_ACCUSOFT_ERROR) - imghandle);
				Msg += bodge;
				Msg += TEXT("')");
				ERROR3(Msg);
#endif
			} else {	
		
				TCHAR OutFileAndPath[256];
				camStrcpy(OutFileAndPath, (const TCHAR *)OutFile->GetPath());

				// Save the newly decompressed / scaled bitmap
				INT32 Problem = AccusoftFilters::pfnIMG_save_bitmap(imghandle, OutFileAndPath, BMP_UNCOMPRESSED);

				if(Problem == 0)
				{
					// We loaded, scaled and saved ok...
					ok = TRUE;					
				}
				else
				{
#ifdef _DEBUG
					String_256 Msg;
					_stprintf(Msg, "GenIdxFl::CreateThumbnail SaveBitmap - '%s' (0x%x '", (const TCHAR *)InFile->GetPath(), Problem);
					String_256 bodge(_R(IDE_ACCUSOFT_ERROR) - Problem);
					Msg += bodge;
					Msg += TEXT("')");
					ERROR3(Msg);
#endif
				}
			}
		}
	}


#endif  // FALSE

	// Problems...
	if(!ok)
	{
		if(!QuietThumbnail)
		{
			// tell the user there was a problem
		 	String_256 Msg;
		 	Msg.MakeMsg(_R(IDS_LIBRARY_THUMBNAIL_PROBLEMS), (const TCHAR *)InFile->GetFileName(TRUE));
			Error::SetError(0, Msg, 0);
			INT32 ButtonPressed = InformWarning(0, _R(IDS_OK), _R(IDS_SILENT), _R(IDS_STOP), NULL);
			Error::ClearError();

			if(ButtonPressed == 2)
				QuietThumbnail = TRUE;

			if(ButtonPressed == 3)
				StopIndGen = TRUE;
		}

		// If there was a problem, delete the resulting mess -> seems to like creating 0 length files, etc...
		SGLibOil::FileDelete(OutFile);
	}

//	// Possibly reset the dll properly...
//	INT32 x = AccusoftFilters::pfnIMGLOW_set_decompsize(0, 0);

	return ok;

//#endif	//WEBSTER
}

/***********************************************************************************************

>	BOOL GenerateIndexFile::CreateThumbnails(PathName *FileName, BOOL NewSystem = TRUE);

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/04/95

	Inputs:		FileName  - file to get thumbnail of
	 			NewSystem - set to TRUE and we'll just generate 1 (large) thumbnail with the same
							name as the file itself (but in the XI dir and with a .bmp ext).
	Outputs:
	Returns:	TRUE if we want to carry on after this file...
	
	Purpose:	Create the thumbnail(s) for the specified file
	Notes:		OldSystem == generate three bitmaps for the file - Small, Medium and Large
																 - filenames == A00002L.bmp, etc
				NewSystem == generate one (large) bitmap for the file - same filename as file

				(With the NewSystem) we also check if the thumbnail has already been generated
				and save ourselves a bit (well, quite a lot) of time if this is the case.

	SeeAlso:	GenerateIndexFile::CreateThumbnail

***********************************************************************************************/

BOOL GenerateIndexFile::CreateThumbnails(PathName *FileName, BOOL NewSystem)
{
	if(FileName == NULL)
	{
		ERROR3("GenerateIndexFile::CreateThumbnails given a null filename - bad");
		return FALSE;
	}

	if(LibType == SGLib_Font || LibType == SGLib_Colour)
	{
		// ERROR3("Thumbnail cannot be generated for this library.");
		return FALSE;
	}

	// Don't generate thumbnails for certain files...
	String_8 Ending;
	Ending = FileName->GetType();
	Ending.toLower();

	// These are the new native and web formats, these may change to be different
	// file extensions
	if(Ending == (String_8)"cxn")
		return TRUE;
	// Web files may not have preview bitmaps but below we use Accusoft to generate a preview bitmap
	if(Ending == (String_8)"cxw")
		return TRUE;
	if((Ending == (String_8)"web") || (Ending == (String_8)"xar"))
	{
		// Don't error, just set exceptions...
	    CCDiskFile File(1024, FALSE, TRUE);

		BOOL ok;

		// Open file and check if it exists at the same time
		TRY
		{
			ok = File.open(*FileName, ios::in | ios::binary | ios::nocreate);
		}		
		CATCH( CFileException, e)
		{
			ok = FALSE;
			Error::ClearError();
		}
		END_CATCH

		// First check that the file is what we think it is.
		BOOL IsNewFormat = FALSE;
		UINT32 FilterId = FILTERID_NONE;

		// In the process it will position the file at the start of the preview bitmap image
		// and get the image type
		if (ok)
		{
			// This method looks for a preview image at the start of the file, and returns
			// the format of that image in FilterId.
			if (BaseCamelotFilter::SkipToPreviewBitmap(&File, &IsNewFormat, &FilterId))
			{
				if (FilterId != FILTERID_NONE)
					return TRUE;
			}

			// If the above test failed, then we're not dealing with a CX2/XaraX file.
			// We now need to see whether it's a CX1.1 file, ie binary EPS with TIFF header.
			else
			{
				GenericFilter* pGenericFilter = Filter::GetGenericFilter();
				if (pGenericFilter != NULL)
				{
					File.seek(0);
					Filter* pOldXarFilter = pGenericFilter->GetBestFilter(&File);
					if (pOldXarFilter != NULL &&
						pOldXarFilter->FilterID == FILTERID_NATIVE_EPS)
						return TRUE;
				}
			}
		}
	}
	if(Ending == (String_8)"art")
		return TRUE;
	if(Ending == (String_8)"cdr")
		return TRUE;
	if(Ending == (String_8)"cdt")
		return TRUE;
	if(Ending == (String_8)"cdx")
		return TRUE;
	if(Ending == (String_8)"cmx")
		return TRUE;
	if(Ending == (String_8)"eps")
		return TRUE;
//	if(Ending == (String_8)"ai")
//		return TRUE;
	if(Ending == (String_8)"wmf")
		return TRUE;
	if(Ending == (String_8)"pcd")
		return TRUE;


	BOOL okl = TRUE, okm = TRUE, oks = TRUE;
	PathName InFile(*FileName);
	PathName OutFile;

	// Defaults
	TCHAR LibraryTypeCharacter = 'A';
	UINT8 XsizeL = 128, YsizeL = 128;
	UINT8 XsizeM = 128,  YsizeM = 64;
	UINT8 XsizeS = 45,  YsizeS = 45;

	String_256 OutFileStr;

	if(NewSystem)
	{
		if(IndexIsRemote)
		{
			// Use the temp directory, or whereever the index is...
			OutFileStr = RemoteLocationOfIndex;
			SGLibOil::AppendSlashIfNotPresent(&OutFileStr);
			/*if(OutFileStr[OutFileStr.Length()-1] != '\\')
				OutFileStr += TEXT("\\");*/
		}
		else
		{
			// Use the subdirectory of the files themselves
			OutFileStr = LibPath.GetPath(TRUE);
			OutFileStr += String_16(_R(IDS_LIBRARIES_XARAINFO_DIRNAME));
			OutFileStr += TEXT("\\");
		}

		OutFileStr += FileName->GetFileName(FALSE);
//#ifdef WEBSTER
		// webster uses png 
		OutFileStr += TEXT(".png");
//#else
//		OutFileStr += TEXT(".bmp");
//#endif
		OutFile.SetPathName(OutFileStr);
		if((SGLibOil::FileExists(&OutFile)) && (SGLibOil::FileSize(&OutFile) >= 0) )
		{
			// Thumbnail already exists and not 0-length
			UINT32 InModified = SGLibOil::FileModified(&InFile);
			UINT32 OutModified = SGLibOil::FileModified(&OutFile);

			// Don't bother updating the thumbnail if it's got a later timestamp than the
			// file itself...
			if(InModified <= OutModified)
				return TRUE;
		}

		// Create a thumbnail for 'InFile'
		return (CreateThumbnail(&InFile, &OutFile, XsizeL, YsizeL));
	}
	else
	{
		switch(LibType)
		{
			// Fills, Texture, same thing...
			case SGLib_Texture:
				LibraryTypeCharacter = 'T';
				break;

			// Clipart and bitmaps live in the same gallery
			case SGLib_ClipArt:
			case SGLib_ClipArt_WebThemes:
			case SGLib_Bitmap:
				LibraryTypeCharacter = 'A';
				break;

			// Font previews are a completely different shape to the rest
			case SGLib_Font:
				LibraryTypeCharacter = 'F';
				XsizeL = 180;
				YsizeL = 26;
				XsizeM = 160;
				YsizeM = 12;
				XsizeS = 28;
				YsizeS = 32;
				break;
		}

		String_256 Dir(LibPath.GetPath(TRUE)); 		// Directory + \Xarainfo\                 .
		Dir += String_16(_R(IDS_LIBRARIES_XARAINFO_DIRNAME));
		Dir += TEXT("\\");

		// Go ahead and create all three thumbnails for the file...
		_stprintf(OutFileStr, "%s%C%05dL.bmp", (TCHAR *)Dir, LibraryTypeCharacter, ID);
		OutFile.SetPathName(OutFileStr);
		okl = CreateThumbnail(&InFile, &OutFile, XsizeL, YsizeL);

		_stprintf(OutFileStr, "%s%C%05dM.bmp", (TCHAR *)Dir, LibraryTypeCharacter, ID);
		OutFile.SetPathName(OutFileStr);
		okm = CreateThumbnail(&InFile, &OutFile, XsizeM, YsizeM);
	
		_stprintf(OutFileStr, "%s%C%05dS.bmp", (TCHAR *)Dir, LibraryTypeCharacter, ID);
		OutFile.SetPathName(OutFileStr);
		oks = CreateThumbnail(&InFile, &OutFile, XsizeS, YsizeS);

		return (okl && okm && oks);
	}
	return FALSE;										  	
}

/***********************************************************************************************

>	BOOL GenerateIndexFile::RipDescriptionFromFile(PathName *FileName, String_256 *Text, 
			String_256 *Keywords = NULL, String_256 *Title = NULL)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/05/95

	Inputs:		Filename - Filename of file
	Outputs:	Text - textual description to use for the file in the index
				Keywords
				Title

	Returns:	TRUE if we found a description for the file

	Purpose:	Try and work out a description and keywords given a file.
				Currently we only check ART/EPS files and CDR files.

***********************************************************************************************/

BOOL GenerateIndexFile::RipDescriptionFromFile(PathName *FileName, String_256 *Text, String_256 *Keywords, String_256 *Title)
{
	ERROR3IF(FileName == NULL, "GenerateIndexFile::RipDescriptionFromIndex given null filename");
	ERROR3IF(Text == NULL, "GenerateIndexFile::RipDescriptionFromIndex given null text");

	String_256 Type(FileName->GetType());
	Type.toLower();


#ifdef _BATCHING  
	if( (Type.Sub((String_8)"xar") != -1) || (Type.Sub((String_8)"art") != -1) )
	{
		PathName TmpPath(*FileName);
		TmpPath.SetType("CDR");
		if(SGLibOil::FileExists(&TmpPath))
		{
			return RipDescriptionFromCDRFile(&TmpPath, Text, Keywords, Title);
		}
	}
#endif

//#if NEW_NATIVE_FILTER	// New native filters, only available to those who need them at present
	// Rip the indexing strings from the version 2 native/web format files
	if( (Type.Sub((String_8)"cxn") != -1) || (Type.Sub((String_8)"cxw") != -1) || (Type.Sub((String_8)"web") != -1) )
	{
		return RipDescriptionFromNativeV2File(FileName, Text, Keywords, Title);
	}
//#endif

	if( (Type.Sub((String_8)"xar") != -1) || (Type.Sub((String_8)"art") != -1) || (Type.Sub((String_8)"eps") != -1) )
	{
		// Now that we have had the decision to make .cxns become .xars, we must check to see if its a
		// new native file before doing the EPS parsing. This will have the added side effect of hiding 
		// the binary file from the EPS parser, which seems to toast memory if this is not done!
		BOOL IsNewNativeFile = FALSE;
		BOOL ok = RipDescriptionFromNativeV2File(FileName, Text, Keywords, Title, &IsNewNativeFile); 
		// If we found it was a new native file then return the result to the caller
		if (IsNewNativeFile)
			return ok;

		// Otherwise, assume it is old format and so let the EPS parser have a go at ripping out
		// the comments.
		return RipDescriptionFromEPSFile(FileName, Text, Keywords, Title);
	}

#ifdef _BATCHING
	if( (Type.Sub((String_8)"cdr") != -1) )
	{
		return RipDescriptionFromCDRFile(FileName, Text, Keywords, Title);
	}
#endif

	return FALSE;
}

/***********************************************************************************************

>	BOOL GenerateIndexFile::RipDescriptionFromCDRFile(PathName *FileName, String_256 *Text, 
			String_256 *Keywords = NULL, String_256 *Title = NULL)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/05/95

	Inputs:		Filename - Filename of file
	Outputs:	Text - textual description to use for the file in the index
				Keywords
				Title

	Returns:	TRUE if we found a description for the file

	Purpose:	Try and work out a description and keywords given a cdr 5 file

	Notes:		There might be problems if the preview bitmaps are ever changed to be something
				other than tiffs, at the moment we check the first four bytes of the file to see
				if it's a tiff file and jump over the tiffity bit if it is...

				CDR files seem to contain the text "INFOIKEY" near the start, then four 0's then a bunch
				of ';' separated keywords (with spaces)...


***********************************************************************************************/

BOOL GenerateIndexFile::RipDescriptionFromCDRFile(PathName *FileName, String_256 *Text, String_256 *Keywords, String_256 *Title)
{
#ifdef _BATCHING   

//	ERROR3IF(FileName == NULL || RetName == NULL || Bold == NULL || Italic == NULL, "Null params given to ATM ripper");

	BOOL ok = TRUE;

	PathName CDRPath(*FileName);

	if(!SGLibOil::FileExists(&CDRPath))
	{
		ERROR3("Can't find CDR file");
		return FALSE;
	}

	CCDiskFile CDRFile(1024, FALSE, TRUE);

	TRY
	{
		// Open the xxx.cdr file
		ok = CDRFile.open(CDRPath, ios::in | ios::nocreate | ios::binary);
	}	
			
	CATCH(CFileException, e)
	{
		Error::ClearError();
		return FALSE;
	}
	END_CATCH

	FilePos Position = 0x400; //199;
	BOOL FoundInfoIKey = FALSE;
	CDRFile.seekIn(Position);
		
	TRY
	{
		// Keep looping through the file until we hit the end...
		while(!CDRFile.eof() && !FoundInfoIKey)
		{
			DWORD Buffer[256];
			memset(Buffer, 0, 256 * 4);
			CDRFile.seekIn(Position);

			if(!FoundInfoIKey)
			{
				CDRFile.read((void *)Buffer, 8);

				// Debug
				if(!camStrcmp(((TCHAR *)Buffer) , "INFOIKEY"))
				{
					FoundInfoIKey = TRUE;
					
					// Loop past the four '0's (possibly strings in some files ?)
					INT32 Count = 3;
					while(Count > 0 && !CDRFile.eof())
					{
						CDRFile.read((void *)Buffer, 1);
						if(((TCHAR *)Buffer)[0] == 0)
							Count --;
					};

					TCHAR Keys[256];
					Count = -1;

					do {
						//Count ++;
						CDRFile.read((void *)((char *)Keys + (++Count)), 1);
						//Keys[Count] = ((TCHAR *)Buffer)[0];
						//if(Keys[Count] == ',' || Keys[Count] == ' ')
						//	Keys[Count] = ';';
					} while(Keys[Count] != 0 && !CDRFile.eof() && Count < 255);

					*Keywords = (String_256)Keys;

					// Some chap at corel decided to stick loads of spaces in some of
					// the clipart groups (space and graphics...)
					// This will rip trailing spaces off...
					if(Keywords->Sub((String_8)"   ") != -1)
					{
						String_256 Tmp(*Keywords);
						Tmp.Left(Keywords, Tmp.Sub((String_8)"   "));					
					}

					// Make the last keyword a title
					String_256 Tmp(*Keywords);
					String_256 Tmp2;
					INT32 Offset = Tmp.Sub((String_8)";");				
					while(Offset != -1 && Tmp.Length()>0)
					{
						Tmp2 = Tmp;
						Tmp2.Right(&Tmp, ((Tmp2.Length() - Offset) - 1));
						Offset = Tmp.Sub((String_8)";");
					}
					*Title = Tmp;			

					ReplaceCharacters(Title, ',', ' ');

					ReplaceCharacters(Keywords, ';', '|');
					ReplaceCharacters(Keywords, ',', '|');
					ReplaceCharacters(Keywords, ' ', '|');

					CDRFile.close();
					return TRUE;
				}
			}
			Position ++;
		}
	}

	// See if there was a file io error
	CATCH(CFileException, e)
	{
		// Something rather unhelpful has taken place, or we've reached the end of the file......
		ok = FALSE;

		// remove any errors
		Error::ClearError();
	}
	END_CATCH

	// Make sure that the files are closed and the memory is reclaimed properly...
	TRY
	{
		if (CDRFile.isOpen())
			CDRFile.close();
	}
	CATCH(CFileException, e)
	{
		// Failed to close the files - not much we can do about it really
		Error::ClearError();
	}
	END_CATCH


#endif

	return FALSE;
}

/***********************************************************************************************

>	BOOL GenerateIndexFile::RipDescriptionFromEPSFile(PathName *FileName, String_256 *Text, 
			String_256 *Keywords = NULL, String_256 *Title = NULL)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/05/95

	Inputs:		Filename - Filename of file
	Outputs:	Text - textual description to use for the file in the index
				Keywords
				Title

	Returns:	TRUE if we found a description for the file

	Purpose:	Try and work out a description given an art file (using comments in file)
				This might work for the various eps files knocking about too, so it gives them
				a chance.

	Notes:		There might be problems if the preview bitmaps are ever changed to be something
				other than tiffs, at the moment we check the first four bytes of the file to see
				if it's a tiff file and jump over the tiffity bit if it is...

***********************************************************************************************/

BOOL GenerateIndexFile::RipDescriptionFromEPSFile(PathName *FileName, String_256 *Text, String_256 *Keywords, String_256 *Title)
{
	CCDiskFile *EPSDiskFile = new CCDiskFile(1024, TRUE, FALSE);
	if (EPSDiskFile == NULL)
	{
		ERROR3("Problems allocating CCDiskFile");
		return FALSE;
	}

	if (!EPSDiskFile->InitLexer(TRUE))
	{
		ERROR3("Problems with InitLexer");
		delete EPSDiskFile;
		return FALSE;
	}

	if (!EPSDiskFile->open(*FileName, ios::in))
	{
		ERROR3("Can't open ART/EPS file");
		EPSDiskFile->DeinitLexer();
		delete EPSDiskFile;
		return FALSE;
	}

	// ART (+ XStudio EPS) files optionally contain document comments near their start.
	// The basic format of these in the file is:
	//
	// <tiff file and random general info>
	// %%DocumentInfo
	// %%+p .....
	// %%+c This is the document comment. If it's a large comment then it
	// %%+c will overwrap two lines like this.^
	// %%+c Uphats signify returns in the info... We don't take any notice
	// %%+c of the lines after the uphat...
	// %%+v
	// %%NextSection
	//
	// Since we don't have much room to display INT32 descriptions in the gallery we only currently
	// use the first line of comments in the code below.

	String_256 TokenBuffer("");
	LexTokenType TokType = TOKEN_LINE;

	BOOL FoundDocumentInfo = FALSE;
	BOOL FoundDescription  = FALSE;
	BOOL FileHasNoComments = FALSE;
	BOOL UpHat = FALSE;

	// OK, we will have to check for a preview bmp and skip to the Postscript if there is one
	// read the first 4 bytes and see if they match the magic PostScript word
	char Buf[4];
		
	// clear the buffer
	memset(Buf, 0, 4);

	// read the bytes in
	EPSDiskFile->read(Buf, 4);

	UINT32* pUINT32 = (UINT32*)Buf;
	// see if its a match
	if ((Buf[0]=='\xC5') && (Buf[1]=='\xD0') && (Buf[2]=='\xD3') && (Buf[3]=='\xC6'))
	{
		// Yes, this is a binary EPS file that has a TIFF attached, so find the EPS
		FilePos StartOfEPS = 0;
		EPSDiskFile->read(&StartOfEPS, 4);
		TRACEUSER( "Richard", _T("EPS Starts at %ld\n"), StartOfEPS);
	
		// Seek to the start of the EPS ready for the importer to read it.
		EPSDiskFile->seekIn(StartOfEPS, ios::beg);
	}
	else if (pUINT32[0] == CXF_IDWORD1)
	{
		// the first eight bytes are our magic identifier for the new native/web file format
		TRACEUSER( "Neville", _T(".xar file in new format\n"));
		return FALSE;
	}
	else
	{
		// Nope, this must be any ordinary EPS file, so go back to the start of the file
		EPSDiskFile->seekIn(0, ios::beg);
		// +++++ WARNING +++++
		// Uses to cope with eps files without a preview bitmap in them but this is very bad news these
		// days. If we have .xar files which are really .cxn i.e. the new native format then we will start
		// lexing in a binary file. This is bad!!!!! Memory is toast is the more than likely outcome!
		// The check for new format files above should stop this but left in this warning just in case
		// somebody tries it with a different binary .xar file.
		// Neville 9/8/96
		TRACE( _T(".xar file without binary TIFF header! Dangerous! Assume no indexing data\n"));
		//return FALSE;
	}

	BOOL TimeToFinish = FALSE;

	// Make sure that the flags have priority over the GetLineToken call as if you find the %%Compression
	// line then reading the next line without turning compression on could prove to be slightly fatal
	// in that the EPS parser might try walking into the compressed region due to the buffering that it
	// does. If it does hit the %%Compression token then next time round the loop TimeToFinish will be
	// set and so use this to sop very quickly.
	while(!TimeToFinish && !FileHasNoComments && EPSDiskFile->GetLineToken() && TokType != TOKEN_EOF)
	{
		TokType = EPSDiskFile->GetTokenType();
		if(TokType == TOKEN_LINE)
		{
			TokenBuffer = EPSDiskFile->GetTokenBuf();

			if(FoundDocumentInfo)
			{
				// We've reached the end of the section - finish looping...
				if(TokenBuffer.Sub((String_8)"%%+") == -1)
				{					
					FileHasNoComments = !FoundDescription;
					TimeToFinish = TRUE;
				}

				// Check if this line holds our comment...
				if(TokenBuffer.Sub((String_8)"%%+c ") == 0)
				{
					// Our string looks like: %%+c This bit of text here is a comment
					// Guess which bit we need...
					String_256 TheDesc( ((TCHAR *)TokenBuffer) + 5 );
					String_256 TheDescU(TheDesc);		
					TheDescU.toUpper();

					// Unfortunately, fields are limited to 64 bytes (since lines shouldn't be over 256 bytes)
					// If our description beats back these boundaries we give it a good beating forward...
					// Maybe if life was easy then this wouldn't be a problem. Maybe if we had String_1024s this
					// wouldn't be a problem, ah well...
					if(TheDesc.Length() > 64)
					{
						TokenBuffer.Mid(&TheDesc, 5, 64);
					}
					else
					{
						// Check if we've got an uphat on our string (and strip it off if so...)
						if(TheDesc.Sub((String_8)"^") != -1)
						{
							TokenBuffer.Mid(&TheDesc, 5, TheDesc.Sub((String_8)"^"));
							UpHat = TRUE;
						}
					}

					if(Keywords && TheDescU.Sub(String_16(_R(IDS_LIBRARIES_RIP_XAR_KEYWORDS))) == 0)
					{
						// this line is a keyword entry...

						INT32 exclude = 4;

						// strip leading spaces
						while ( (((TCHAR *)TheDesc)[exclude] == ' ') && (exclude < TheDesc.Length()))
							exclude ++;

						TheDesc.Right(Keywords, TheDesc.Length() - exclude);

						ReplaceCharacters(Keywords, ',', '|');
					}
					else
					{
						if(Title && TheDescU.Sub(String_16(_R(IDS_LIBRARIES_RIP_XAR_TITLE))) == 0)
						{
							// this line is a title entry...
							INT32 exclude = 6;

							// strip leading spaces
							while ( (((TCHAR *)TheDesc)[exclude] == ' ') && (exclude < TheDesc.Length()))
								exclude ++;

							TheDesc.Right(Title, TheDesc.Length() - exclude);

							ReplaceCharacters(Title, ',', '|');
						}
						else
						{
							// default the line to be a description - only if it was the first
							if(!FoundDescription)
							{
								ReplaceCharacters(&TheDesc, ',', '|');
								*Text = TheDesc;
							}
						}
					}

					FoundDescription = TRUE;
				}
			}
			else
			{
				// Convert line to upper case for case insensitive match
				String_256 TokenBufferU(TokenBuffer);
				TokenBufferU.toUpper();

				// Check line for start of document info section
				if(TokenBufferU.Sub((String_16)"%%DOCUMENTINFO") == 0)
				{
					FoundDocumentInfo = TRUE;
				}
	
				// Check to see if we've overshot, and are wasting time...
				if( (TokenBufferU.Sub((String_16)"%%AWCOLOURTABLE") == 0)
					|| (TokenBufferU.Sub((String_16)"%%COMPRESSION") == 0)
					|| (TokenBuffer.Sub((String_8)"%") != 0) )
				{
					FileHasNoComments = TRUE;
					TimeToFinish = TRUE;
				}
			}
		}
	}
	
	// reclaim lexer-buffer memory
	EPSDiskFile->DeinitLexer();

	if(EPSDiskFile->isOpen())
		EPSDiskFile->close();

	delete EPSDiskFile;

	return FoundDescription;
}

/***********************************************************************************************

>	BOOL GenerateIndexFile::RipDescriptionFromNativeV2File(PathName *FileName, String_256 *Text, 
			String_256 *Keywords = NULL, String_256 *Title = NULL, BOOL *FileIsNewNative = NULL)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/08/96

	Inputs:		Filename - Filename of file
	Outputs:	Text - textual description to use for the file in the index
				Keywords
				Title

	Returns:	TRUE if we found a description for the file

	Purpose:	Try and work out a description given a new version 2 format native, cxn, file
				(using a document comment record stored in the file). This is not stored in
				Web format files as this is meant to be the compact form of the file. 
				This might work for the various eps files knocking about too, so it gives them
				a chance.

***********************************************************************************************/

BOOL GenerateIndexFile::RipDescriptionFromNativeV2File(PathName *FileName,
													   String_256 *Text, String_256 *Keywords, String_256 *Title,
													   BOOL *FileIsNewNative)
{
	CCDiskFile *NativeDiskFile = new CCDiskFile(1024, TRUE, FALSE);
	if (NativeDiskFile == NULL)
	{
		ERROR3("Problems allocating CCDiskFile");
		return FALSE;
	}

	if (!NativeDiskFile->open(*FileName, ios::in))
	{
		ERROR3("Can't open CXN/CXW file");
		delete NativeDiskFile;
		return FALSE;
	}

	BOOL FoundDescription  = FALSE;

	// Ask the base camelot filter to rip out the document comment from the file, returns FALSE
	// if not a version 2 native or web file or the comment has not been found. Only returns TRUE
	// if a document comment has succesfully been found.
	// May not be found as only saved if there text has been entered into the document information
	// field.
	String_256 DocumentComment;
	BOOL IsNewNativeFile = FALSE;
	BOOL ok = BaseCamelotFilter::SkipToIndexInformation(NativeDiskFile, &DocumentComment, &IsNewNativeFile);
	// Return whether it was a new native/web file to the caller
	if (FileIsNewNative)
		*FileIsNewNative = IsNewNativeFile;
	if (!ok)
	{
		if(NativeDiskFile->isOpen())
			NativeDiskFile->close();

		delete NativeDiskFile;
		return FALSE;
	}

	// We should now have a document comment in DocumentComment.
	// If its emptry then there is no point in trying to rip the comments out
	if (!DocumentComment.IsEmpty())
	{
		// Rip the data we require from the comment.
		// We need to read a line at a time from the string and check if it starts with one of our
		// standard indexing strings. The first line, if it doesn't start with a standard indexing
		// string can be considered as a description string

		const TCHAR LineEnd = TEXT('\n');
		INT32 pos = 0;
		INT32 LastPos = 0;
		String_256 TheDesc;
		String_256 TheDescU;
		BOOL TimeToEnd = FALSE;
		while (pos >= 0 && !TimeToEnd)
		{
			// This will return -1 if not found or the position in the string of the line end
			pos = DocumentComment.FindNextChar(LineEnd, pos);
			// If a line end is not found then try parsing the entire string
			// We need to do this on the last pass as otherwise the last line in the string will
			// not be looked at
			if (pos == -1)
			{
				// One passed the end of the string as the pos - LastPos - 1 will then be correct
				pos = DocumentComment.Length() + 1;
				// Make sure we only do this once
				TimeToEnd = TRUE;
			}	

			// See if it is a blank line, if so then skip it
			if ((pos >= 0) && (pos - LastPos <= 1))
			{
				// shift onto after the found line end character
				pos += 1;
				// remember this as the last search position
				LastPos = pos;
			}
			else if ((pos >= 0) && (pos - LastPos > 1))
			{
				// Get the line that we have just found, make sure we don't include the line end
				// as otherwise is screws up the text line in the index file
				DocumentComment.Mid(&TheDesc, LastPos , pos - LastPos - 1);
				// shift onto after the found line end character
				pos += 1;
				// remember this as the last search position
				LastPos = pos;

				// Get an upper case version of the string, ready to do do our comparing on
				TheDescU = TheDesc;
				TheDescU.toUpper();
				// check for KEY:
				if(Keywords && TheDescU.Sub(String_16(_R(IDS_LIBRARIES_RIP_XAR_KEYWORDS))) == 0)
				{
					// this line is a keyword entry...

					INT32 exclude = 4; // Length of KEY: 

					// strip leading spaces
					while ( (((TCHAR *)TheDesc)[exclude] == ' ') && (exclude < TheDesc.Length()))
						exclude ++;

					TheDesc.Right(Keywords, TheDesc.Length() - exclude);

					ReplaceCharacters(Keywords, ',', '|');
				}
				else
				{
					// Check for TITLE
					if(Title && TheDescU.Sub(String_16(_R(IDS_LIBRARIES_RIP_XAR_TITLE))) == 0)
					{
						// this line is a title entry...
						INT32 exclude = 6; // Length of TITLE: 

						// strip leading spaces
						while ( (((TCHAR *)TheDesc)[exclude] == ' ') && (exclude < TheDesc.Length()))
							exclude ++;

						TheDesc.Right(Title, TheDesc.Length() - exclude);

						ReplaceCharacters(Title, ',', '|');
					}
					else
					{
						// default the line to be a description - only if it was the first
						if(!FoundDescription)
						{
							ReplaceCharacters(&TheDesc, ',', '|');
							*Text = TheDesc;
						}
					}
				}

				// Flag we have found a comment
				FoundDescription = TRUE;
			}
		}
	}

	if(NativeDiskFile->isOpen())
		NativeDiskFile->close();

	delete NativeDiskFile;

	return FoundDescription;
}

#endif

/***********************************************************************************************

>	static BOOL GenerateIndexFile::ReplaceCharacters(StringBase *Source, TCHAR Search, TCHAR Replace)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/05/95

	Inputs:		Source		- Source string
				Search		- Character to search for
				Replace		- Character to replace found 'search' characters with
	Outputs:	Updates the buffer in 'Source'
	Returns:	TRUE if it all worked...

	Purpose:	To find and replace specific characters in a string

***********************************************************************************************/

BOOL GenerateIndexFile::ReplaceCharacters(StringBase *Source, TCHAR Search, TCHAR Replace)
{
	ERROR3IF(Source == NULL, "GenerateIndexFile::ReplaceCharacters - source == null");

	INT32 Len = Source->Length();
	if (Len == 0)
		return FALSE;

#ifdef _DEBUG
	INT32 Count = 0;
#endif

	TCHAR *Buf = (TCHAR *)*Source;
	for(INT32 i = 0; i < Len; i++)
	{
		if(*Buf == Search)
		{
			*Buf = Replace;
#ifdef _DEBUG
		 	Count ++;
#endif
		}
		Buf ++;
	}

#ifdef _DEBUG
	TRACEUSER("Richard", _T("%d %c chars found in %s and replaced with %c\n"), Count, Search, (TCHAR *)*Source, Replace);
#endif
	return TRUE;
}

#if 0

/***********************************************************************************************

>	static INT32 GenerateIndexFile::CountCommas(StringBase *Source, StringBase &SearchFor)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/05/95

	Inputs:		Source		- Source string with ,'s in
				SearchFor	- Substring to search for in source string
	Outputs:
	Returns:	Count of commas to the left of 'searchfor' in 'source'

	Purpose:	To count the commas towards the left of a substring in a source string.
		
				For example, passing it:
				
				"This, is, a string" and "stri" would return 2

***********************************************************************************************/

INT32 GenerateIndexFile::CountCommas(StringBase *Source, StringBase &SearchFor)
{
	ERROR3IF(Source == NULL, "GenerateIndexFile::CountCommas - source == null");

	INT32 Len = Source->Length();
	if(Len == 0)
		return 0;

	INT32 Count = 0;
	INT32 Pos = Source->Sub(SearchFor);

	if(Pos != -1)
	{
		// Find out which field it was
		String_256 Left;
		Source->Left(&Left, Pos);
		Len = Left.Length();

		// This bit could go in basestr...		
		TCHAR *Buf = (TCHAR *)Left;
		for(INT32 i = 0; i < Len; i++)
			if(*(Buf++) == ',')
			 	Count ++;
	}
	return Count;
}


/***********************************************************************************************

>	static BOOL GenerateIndexFile::GetToken(StringBase *Source, INT32 Count, String_256 *Result)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/05/95

	Inputs:		Source		- Source string
				Count		- Number of commas to skip in source string
	Outputs:	Result 		- Resulting token found in string (with leading and trailing
							  spaces stripped)
	Returns:	TRUE if things went ok

	Purpose:	To return the space stripped token Count commas into the given string

***********************************************************************************************/

BOOL GenerateIndexFile::GetToken(StringBase *Source, INT32 Count, String_256 *Result)
{
	ERROR3IF(Source == NULL, "GenerateIndexFile::GetToken - source == null");

	TCHAR *Comma = NULL;
	
	if(Count == 0)
	{
		Comma = ((TCHAR *)*Source);
	}
	else
	{
		Comma = camStrchr((TCHAR *)*Source, _T(','));

		if(Count > 2)
		{
			for(INT32 i = Count - 1; i > 0; i--)
			{
				Comma = camStrchr(Comma + 1, _T(','));
				
				// Couldn't find token in string - not enough commas
				if(Comma == NULL)
					return FALSE;		
			}
		}
	}

	INT32 first = (Comma - (TCHAR *)*Source) + 1;	
	INT32 second = 0;
	if(camStrchr(Comma + 1, _T(',')))
		second = (camStrchr(Comma + 1, _T(',')) - (TCHAR *)*Source);
	else
		second = camStrlen((TCHAR *)*Source);

	// strip leading spaces
	while ( (((TCHAR *)*Source)[first] == ' ') && (first < Source->Length()))
		first ++;

	// strip trailing spaces
	if(second > 0)
		while ( (((TCHAR *)*Source)[second-1] == ' ') && (second > first))
			second --;

	// Looks like we've got a valid string
	if(first != 0 && second != 0 && first < second)
	{
		Source->Mid(Result, first, second - first);
		return TRUE;
	}

	// Couldn't find token in string		
	return FALSE;
}


/***********************************************************************************************

>	BOOL GenerateIndexFile::PrePassIndex(PathName *IndexFile, FilePos *Start,
		INT32 *DescriptionField, INT32 *KeywordField, INT32 *TitleField, String_256 *Description)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/05/95

	Inputs:		IndexFile 			- Filename of index file to scan
	Outputs:	Start				- File offset to 'START' text
				DescriptionField	- Number of commas to left of 'description' field
				KeywordField		- Number of commas to left of 'keyword' field
				TitleField			- Number of commas to left of 'title' field
				Description			- Description for group in gallery

	Returns:	TRUE if the index file looks ok to use
	
	Purpose:	Scan the old index header and return useful information

***********************************************************************************************/

BOOL GenerateIndexFile::PrePassIndex(PathName *IndexFile, FilePos *Start,
		INT32 *DescriptionField, INT32 *KeywordField, INT32 *TitleField, String_256 *Description)
{
	ERROR3IF(IndexFile == NULL, "GenerateIndexFile::PrePassIndex given null index");

	CCDiskFile *TmpDiskFile = new CCDiskFile(1024, TRUE, FALSE);
	if (TmpDiskFile == NULL)
	{
		ERROR3("Null index file allocated");
		return FALSE;
	}

	if (!TmpDiskFile->InitLexer(TRUE))
	{
		ERROR3("Problems with InitLexer");
		delete TmpDiskFile;
		TmpDiskFile = NULL;
		return FALSE;
	}

	if (!TmpDiskFile->open(*IndexFile, ios::in))
	{
		ERROR3("Can't open index file for pre pass");
		TmpDiskFile->DeinitLexer();
		delete TmpDiskFile;
		TmpDiskFile = NULL;
		return FALSE;
	}

	LexTokenType TokType = TOKEN_LINE;
	BOOL FoundFieldTitles = FALSE;
	BOOL FoundStart = FALSE;

	// Loop through header ripping the interesting stuff out
	while(TmpDiskFile->GetLineToken() && TokType != TOKEN_EOF && !FoundStart)
	{
		TokType = TmpDiskFile->GetTokenType();
		if(TokType == TOKEN_LINE || TokType == TOKEN_COMMENT)
		{
			String_256 TokenBufferU(TmpDiskFile->GetTokenBuf());
			TokenBufferU.toUpper();

			String_16 FilenameStr(_R(IDS_LIBRARIES_INDEX_ITEM_FILENAME));
			String_16 DescStr(_R(IDS_LIBRARIES_INDEX_ITEM_DESC));
			FilenameStr.toUpper();
			DescStr.toUpper();

			// Rip out the field positions from the field description line in the header
  			if(TokType == TOKEN_LINE && !FoundFieldTitles && TokenBufferU.Sub(FilenameStr) == 0 && TokenBufferU.Sub(DescStr) > 0)
			{
				if(DescriptionField)
				{
					String_16 Str(_R(IDS_LIBRARIES_INDEX_ITEM_DESC));
					Str.toUpper();
					*DescriptionField = CountCommas(&TokenBufferU, Str);
				}

				if(KeywordField)
				{
					String_16 Str(_R(IDS_LIBRARIES_INDEX_ITEM_KEY));
					Str.toUpper();
					*KeywordField = CountCommas(&TokenBufferU, Str);
				}
				
				if(TitleField)
				{
					String_16 Str(_R(IDS_LIBRARIES_INDEX_ITEM_TITLE));
					Str.toUpper();
					*TitleField = CountCommas(&TokenBufferU, Str);
				}

				FoundFieldTitles = TRUE;
			}
			else if(TokType == TOKEN_LINE && TokenBufferU.Sub(String_16(_R(IDS_LIBRARIES_INDEX_START))) == 0)
			{
				// We've found 'START', finish looping and return the results
				if(Start)
					*Start = TmpDiskFile->tellIn();

				FoundStart = TRUE;
			}
			else
			{
				// Check for the group description line. This could be in comment
				// form: #DESC... or just normal form DESC... at the start of a line
				INT32 Offset = TokenBufferU.Sub(String_16(_R(IDS_LIBRARIES_INDEX_DESCRITION)));

	  			if(Description && Offset > -1 && Offset < 2)
				{
					String_256 TokenBuffer(TmpDiskFile->GetTokenBuf());
					TokenBuffer.Right(Description, TokenBuffer.Length() - (12 + Offset));				
					LibraryFile::KillLeadingSpaces(Description);
				}
			}
		}
	}
	
	// reclaim lexer-buffer memory
	TmpDiskFile->DeinitLexer();

	if(TmpDiskFile->isOpen())
		TmpDiskFile->close();

	delete TmpDiskFile;

	return (FoundFieldTitles && FoundStart);
}

/***********************************************************************************************

>	BOOL GenerateIndexFile::RipDescriptionFromIndex(PathName *IndexFile, PathName *FileName,
					FilePos Start, INT32 DescriptionField, INT32 KeywordField, INT32 TitleField,
					String_256 *Text, String_256 *Keywords = NULL, String_256 *Title = NULL)
	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/05/95

	Inputs:		IndexFile 			- Filename of index file to scan
				FileName 			- Filename of file to look for in index file
				Start				- Offset from start of file to start scanning from
				DescriptionField	- Number of commas to left of description field
				KeywordField		- Number of commas to left of keyword field
				TitleField			- Number of commas to left of title field

	Outputs:	Text 				- Textual description to use for the file in the index
				Keywords 			- Keywords associated with the file in the index
				Title	 			- Alternative title to display instead of filename in certain modes

	Returns:	TRUE if we found a description for the file

	Purpose:	Scan an old index file looking for the description of the given filename

	Notes:		PreScan does most of the parameter work for use...

***********************************************************************************************/

BOOL GenerateIndexFile::RipDescriptionFromIndex(PathName *IndexFile, PathName *FileName,
					FilePos Start, INT32 DescriptionField, INT32 KeywordField, INT32 TitleField,
					String_256 *Text, String_256 *Keywords, String_256 *Title)
{
	ERROR3IF(IndexFile == NULL, "GenerateIndexFile::RipDescriptionFromIndex given null index");
	ERROR3IF(FileName == NULL, "GenerateIndexFile::RipDescriptionFromIndex given null filename");
	ERROR3IF(Text == NULL, "GenerateIndexFile::RipDescriptionFromIndex given null text");

	if(IndexFile == NULL || FileName == NULL || Text == NULL)
		return FALSE;

	// We must always have 'filename' and 'description' as the first two...
	if(KeywordField < 2 || TitleField < 2 || (DescriptionField == TitleField))
		return FALSE;

	CCDiskFile *TmpDiskFile = new CCDiskFile(1024, TRUE, FALSE);
	if (TmpDiskFile == NULL)
	{
		ERROR3("Null index file allocated");
		return FALSE;
	}

	if (!TmpDiskFile->InitLexer(TRUE))
	{
		ERROR3("Problems with InitLexer");
		delete TmpDiskFile;
		return FALSE;
	}

	if (!TmpDiskFile->open(*IndexFile, ios::in))
	{
		ERROR3("Can't open backup index file");
		TmpDiskFile->DeinitLexer();
		delete TmpDiskFile;
		return FALSE;
	}

	// Jump over possibly useless lines..
	TmpDiskFile->seekIn(Start);

	String_256 TokenBuffer;
	LexTokenType TokType = TOKEN_LINE;
	BOOL FoundDescription = FALSE;
	String_256 TheFile(FileName->GetFileName());
	//String_256 ShortPath(TheFile);
	String_256 ShortPath(FileName->GetPath());
	TheFile.toUpper();

	// Make the path into its "short form", ie. 8.3 MS-DOS compatible.  This is necessary
	// as Windows NT doesn't like paths with spaces, even though it allows them (eg.
	// CreateProcess will fail when parsing the image path / command line).
#ifdef _DEBUG
	ERROR3IF(!MakeShortPath((TCHAR *)ShortPath, _MAX_PATH), "MakeShortPath failed in SGIndGen");
#else
	MakeShortPath((TCHAR *)ShortPath, _MAX_PATH);
#endif

	// Get a filename from a full path string...
	PathName SrtPath(ShortPath);
	ShortPath = SrtPath.GetFileName(TRUE);


	while(TmpDiskFile->GetLineToken() && TokType != TOKEN_EOF && !FoundDescription)
	{
		TokType = TmpDiskFile->GetTokenType();
		if(TokType == TOKEN_LINE)
		{
			TokenBuffer = TmpDiskFile->GetTokenBuf();
			String_256 TokenBufferU(TokenBuffer);
			TokenBufferU.toUpper();

			// Check both uppercase versions for sub...
			// Note the filename is assumed to be the first field in each line, and the
			// description the second...
			if(TokenBufferU.Sub(TheFile) == 0 || TokenBufferU.Sub(ShortPath) == 0)
			{
				TRACEUSER( "Richard", _T("Matched %s\n"), (TCHAR *)TheFile);

				// Rip out the Description
				// -----------------------
		
				if(GetToken(&TokenBuffer, DescriptionField, Text))
				{
					FoundDescription = TRUE;
					TRACEUSER( "Richard", _T("Description - '%s'\n"), (TCHAR *)*Text);
				}

				if(Title && TitleField != 0)
				{
					// Rip out the Title
					// -----------------

					if(GetToken(&TokenBuffer, TitleField, Title))
					{
						FoundDescription = TRUE;
						TRACEUSER( "Richard", _T("Title - '%s'\n"), (TCHAR *)*Title);
					}
				}

				if(Keywords && KeywordField != 0)
				{

					// Rip out the Keywords
					// --------------------

					if(GetToken(&TokenBuffer, KeywordField, Keywords))
					{
						FoundDescription = TRUE;
						TRACEUSER( "Richard", _T("Keywords - '%s'\n"), (TCHAR *)*Keywords);
					}
				}
			}
		}
	}


/*	if(FoundDescription)
	{
		LastFoundItemInIndex = TmpDiskFile->tell();
	}*/
	
	// reclaim lexer-buffer memory
	TmpDiskFile->DeinitLexer();

	if(TmpDiskFile->isOpen())
		TmpDiskFile->close();

	delete TmpDiskFile;

	// If no title present in index, but there was a description, or keywords, and the
	// file is an art file, use the art filename as the title...
	if(Title->Length() == 0 && FoundDescription)
	{
		String_256 Type(FileName->GetType());
		Type.toLower();
		if( (Type.Sub((String_8)"art") == 0) || (Type.Sub((String_8)"xar") == 0))
		{
			*Title = FileName->GetFileName(FALSE);

			// Default filename - force to lower case
			Title->toLower();

			// And capitalise the first character
			TCHAR First = camToupper(((TCHAR *)*Title)[0]);
			((TCHAR *)*Title)[0] = First;
		}
	}

	return FoundDescription;
}


/***********************************************************************************************

>	BOOL GenerateIndexFile::AddLineToIndex(String_256 *Text);

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/04/95

	Inputs:		Text - the text to add
	Outputs:
	Returns:	TRUE if things went ok.

	Purpose:	Adds the given text to the index file
	Notes:

***********************************************************************************************/

BOOL GenerateIndexFile::AddLineToIndex(String_256 *Text)
{
	if(IndexFile == NULL)
	{
		ERROR3("GenerateIndexFile::AddLineToIndex called with NULL index file");
		return FALSE;
	}

	if(Text == NULL)
	{
		ERROR3("GenerateIndexFile::AddLineToIndex called with NULL text ptr");
		return FALSE;
	}

	IndexFile->write(*Text, (UINT32)Text->Length());

	return TRUE;
}

/***********************************************************************************************

>	BOOL GenerateIndexFile::CheckExistingIndexForRemoteness(PathName *IndexFile, PathName *FilesPath);

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/04/95

	Inputs:		IndexFile		- location and name of the index file to check
	Outputs:	FilesPath		- ptr to path of files in library to alter
	Returns:	TRUE if there's an index, and it's remote

	Purpose:	Check an index file to see if it's remote... If it is, return the remote location
				to the caller...
	Notes:

***********************************************************************************************/

BOOL GenerateIndexFile::CheckExistingIndexForRemoteness(PathName *IndexFile, PathName *FilesPath)
{
	ERROR3IF(IndexFile == NULL || FilesPath == NULL, "GenerateIndexFile::CheckExistingIndexForRemoteness given null params");
	if(IndexFile == NULL || FilesPath == NULL)
		return FALSE;

	if(!SGLibOil::FileExists(IndexFile))
		return FALSE;

	CCDiskFile *TmpDiskFile = new CCDiskFile(1024, FALSE, TRUE);
	if (TmpDiskFile == NULL)
	{
		ERROR3("Null index file allocated");
		return FALSE;
	}

	if (!TmpDiskFile->InitLexer(TRUE))
	{
		ERROR3("Problems with InitLexer");
		delete TmpDiskFile;
		TmpDiskFile = NULL;
		return FALSE;
	}

	TRY
	{
		if (!TmpDiskFile->open(*IndexFile, ios::in))
		{
			ERROR3("Can't open index file for pre pass");
			TmpDiskFile->DeinitLexer();
			delete TmpDiskFile;
			TmpDiskFile = NULL;
			return FALSE;
		}
	}

	CATCH(CFileException, e)
	{
		// Some sort of problem with the open thingie...
		Error::ClearError();
		return FALSE;
	}
	END_CATCH

	LexTokenType TokType = TOKEN_LINE;
	String_256 RemotePath;
	BOOL FoundRemotePath = FALSE;
	BOOL FinishLooping = FALSE;

	// Loop through header ripping the interesting stuff out
	while(TmpDiskFile->GetLineToken() && TokType != TOKEN_EOF && !FinishLooping)
	{
		TokType = TmpDiskFile->GetTokenType();
		if(TokType == TOKEN_LINE || TokType == TOKEN_COMMENT)
		{
			String_256 TokenBufferU(TmpDiskFile->GetTokenBuf());
			TokenBufferU.toUpper();

			if(TokType == TOKEN_LINE && TokenBufferU.Sub(String_16(_R(IDS_LIBRARIES_INDEX_START))) == 0)
				FinishLooping = TRUE;
			else
			{
				// Check for the remote files line.
				INT32 Offset = TokenBufferU.Sub(String_16(_R(IDS_LIBRARIES_INDEX_FILES)));
	  			if(Offset == 0)
				{
					String_256 TokenBuffer(TmpDiskFile->GetTokenBuf());
					TokenBuffer.Right(&RemotePath, TokenBuffer.Length() - (6 + Offset));				
					LibraryFile::KillLeadingSpaces(&RemotePath);
					FinishLooping = TRUE;
					FoundRemotePath = TRUE;
					FilesPath->SetPathName(RemotePath);
				}
			}
		}
	}
	
	// reclaim lexer-buffer memory
	TmpDiskFile->DeinitLexer();

	if(TmpDiskFile->isOpen())
		TmpDiskFile->close();

	delete TmpDiskFile;

	return (FoundRemotePath);
}



/***********************************************************************************************

>	BOOL GenerateIndexFile::Generate(PathName *IFile, PathName *LibDirPath, String_64 *Author
								SGLibType Type, BOOL Thumbnails);

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/4/95

	Inputs:		IFile - Full path of index file to generate - C:\testlib\animals\xarainfo\xaraclip.txt
				LibDirPath - Full path of sublibrary - C:\testlib\animals
				Author - Default origin of these files
				Type - Type of library index required 
				Thumbnails - Generate thumbnails if we can ?
	Outputs:
	Returns:	TRUE if things went ok.

	Purpose:	Generates a dummy index file for a directory of say .art files.

***********************************************************************************************/

BOOL GenerateIndexFile::Generate(PathName *IFile, PathName *LibDirPath, String_64 *Auth,
								 SGLibType Type, BOOL Thumbnails)
{
/*	String_256 IF(IFile->GetPath());
	String_256 LDP(LibDirPath->GetPath());
	ERROR3_PF(("Generate called with '%s' and '%s'", (TCHAR *)IF, (TCHAR *)LDP));*/
	
	if(IFile == NULL || LibDirPath == NULL)
	{
		ERROR3("GenerateIndexFile::Generate null params are bad");
		return FALSE;
	}

	if(SGLibOil::FileExists(IFile) && IsDirectoryReadOnly(LibDirPath))
	{
		ERROR3("Chosen directory is read only and already has an index... (We shouldn't really get here...)");
		return FALSE;
	}

	// Check if there's an existing index, and if so, check if it's a remote one...
	IndexIsRemote = CheckExistingIndexForRemoteness(IFile, LibDirPath);

	Index = *IFile;
	LibPath = *LibDirPath;
	LibType = Type;
	DoThumbnails = Thumbnails;

#ifdef _DEBUG
	String_256 ChecksumDir(LibPath.GetPath());
	UINT32 Checksum = GetDirectoryChecksum(&ChecksumDir, Type);
	//ERROR3_PF(("Checksum for '%s' = %d", (TCHAR *)ChecksumDir, Checksum));
#endif

	if(Auth != NULL)
		Author = *Auth;
	else
		Author = "";

	// DiskFile for index
	IndexFile = NULL;

	// Offset into index for the 'Number of files' data
	IndexCountOff = 0;

	// Search handle for directory scan		
	hSearch = NULL;

	// ID of current item and number of items in file
	ID = 0;

	// If we're overwriting an old index file, copy the old file to a tmp file and
	// use this variable to access it...
	TmpIndexFile = NULL;

	// Remote index stuff...
	IndexIsRemote = FALSE;
	RemoteLocationOfIndex = "";
	RemoteLocationOfFiles = "";

	BOOL ok = FALSE;

	// Display warnings unless told not to
	QuietThumbnail = FALSE;

	// Keep doing index until told not to
	StopIndGen = FALSE;

	// Total number of files that will be added to the index
	TotalFiles = CountValidFiles();

	if(TotalFiles == 0)
	{
		// "Can't find any files to add to index in specified folder";
		INT32 ButtonPressed = InformWarning(_R(IDS_LIBRARY_NO_FILES), _R(IDS_OK), NULL);
		Error::ClearError();
		ok = FALSE;
	}
	else
	{
		// Progress bar for generation status
		String_64 *Status = new String_64(_R(IDS_LIBRARY_GENERATING_INDEX));
		BeginSlowJob(TotalFiles, FALSE, Status);

		// Create the index file and keywords file then add headers to them													   
		if(CreateIndexFile())
		{
			// Add all the header data to an index file
			if(DoIndexHeader())
			{
				// scan the directory for files and add them to the index and keyword files
				ok = AddToSubLibrary();

				// add in the number of IDs in the file
				if(!FixSubLibraryNumber())
					ok = FALSE;
			}
		}
	
		// Close the search handle
		CloseSearchHandle();
	
		// Close the file handle
		CloseIndexFileHandle();

		// If we created a temp index file, delete it
		if(TmpIndexFile != NULL)
		{
			SGLibOil::FileDelete(TmpIndexFile);
			delete TmpIndexFile;
			TmpIndexFile = NULL;
		}

		if(Status != NULL)
		{
			delete Status;
			Status = NULL;
		}

		EndSlowJob();
	}

	if(!ok)
	{
		TRACEUSER( "Richard", _T("All finished and hunky-dorey...\n"));
	}

	// Sort the index location out for the 'add' code...
	if(IndexIsRemote)
		*IFile = Index;

	// Just in case some stupid pathname problem occured...
	Error::ClearError();

	return ok;
}


/***********************************************************************************************

>	static UINT32 GenerateIndexFile::GetDirectoryChecksum(String_256 *Directory, SGLibType Type = SGLib_Blank)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/8/95

	Inputs:		Directory 	- Path of directory to calculate check on
				Type 		- Type of library we require the checksum for (SGLib_Blank will include all
								the files in the directory, whereas SGLib_Texture will only include
								bitmapped images...)
	Outputs:
	Returns:	Checksum of files in directory

	Purpose:	Generates a dummy index file for a directory of say .art files.

***********************************************************************************************/

UINT32 GenerateIndexFile::GetDirectoryChecksum(String_256 *Directory, SGLibType Type)
{
	UINT32 Checksum = 0;

	if(GenerateIndexFile::UseAutomaticUpdate)
	{
		// Progress bar for checksum
		String_64 Status(_R(IDS_LIBRARIES_CALCULATING_CHECKSUM));
		BeginSlowJob(-1, FALSE, &Status);

		WIN32_FIND_DATA FileData;
		BOOL fFinished = FALSE;
		HANDLE hSearch = 0;

		String_256 FileSearch(*Directory);
		SGLibOil::AppendSlashIfNotPresent(&FileSearch);
		/*if(FileSearch[FileSearch.Length()-1] != '\\')
			FileSearch += TEXT("\\");*/
		FileSearch += TEXT("*.*");

		// Start searching for files
		hSearch = FindFirstFile(FileSearch, &FileData);
		if (hSearch == INVALID_HANDLE_VALUE)
		{
		    ERROR3("No valid files found in this directory");
			fFinished = TRUE;
		}

		// Loop through those files...
		while (!fFinished)
		{
			// Add full pathname to file
			String_256 FileToAddStr(*Directory);
			SGLibOil::AppendSlashIfNotPresent(&FileToAddStr);
			/*if(FileToAddStr[FileToAddStr.Length()-1] != '\\')
				FileToAddStr += TEXT("\\");*/
			FileToAddStr += (TCHAR *)FileData.cFileName;

			// Only use normal files - not directories, hidden files, etc...
			DWORD FileAttributes = GetFileAttributes(FileToAddStr);

			if( ((FileAttributes & FILE_ATTRIBUTE_DIRECTORY) ||
			 	(FileAttributes & FILE_ATTRIBUTE_HIDDEN) ||
			 	(FileAttributes & FILE_ATTRIBUTE_SYSTEM)) == 0)
			{
				PathName FileToAdd(FileToAddStr);

				// Check if the file is addable to the index
				if(IsFileType(&FileToAdd, Type))
				{
					UINT32 Modified = SGLibOil::FileModified(&FileToAdd);
					INT32 Size = SGLibOil::FileSize(&FileToAdd);

					Checksum ++;
					Checksum += Modified;
					Checksum += Size;
				}
			}
	
			// Find the next file
			if (!FindNextFile(hSearch, &FileData))
		    	fFinished = TRUE;
		}

		// Close the search handle
		if(hSearch != NULL)
			if (!FindClose(hSearch))
		    	ERROR3("Couldn't close search handle.");
		hSearch = NULL;

		EndSlowJob();
	}

	return Checksum;
}

#endif
