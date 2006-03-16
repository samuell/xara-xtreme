// $Id: sgindgen.h 662 2006-03-14 21:31:49Z alex $
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
// SGindgen.h - Index generation code

#ifndef INC_SGINDGEN
#define INC_SGINDGEN

#include "pathname.h"
#include "thumb.h"
#include "ccfile.h"

/********************************************************************************************

>	class IndGenFileBuffer : public CCObject

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/05/95
	Purpose:	Since MSDOS seems happy returning filenames of files in a directory in a
				completely random fashion, we need to have some way of storing all the ones
				it returns us and sorting them alphabetically, so we can generate index files
				in a nice alphabetical order.

				The data structure used below is basically an array of TCHAR *'s pointed to
				by Buffer. These TCHAR *'s are CCMalloced and given variable length C style
				strings by AddItem. The destructor handles the memory tidying up.

********************************************************************************************/

class IndGenFileBuffer : public CCObject
{
	CC_DECLARE_DYNCREATE( IndGenFileBuffer )

public:
	IndGenFileBuffer();
	~IndGenFileBuffer();
	BOOL Init(INT32 Count);
	BOOL AddItem(TCHAR *String);
	BOOL Sort(void);
	TCHAR *GetItem(INT32 Count);
	INT32 ItemCount(void)	{return Items;}

protected:
	static INT32 __cdecl SortComparator(const void *Item1, const void *Item2);

protected:
	TCHAR **Buffer;
	INT32 Items;
	INT32 Position;
};


/********************************************************************************************

>	class GenerateIndexFile : public CCObject

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/04/95
	Purpose:	For generating of index files

********************************************************************************************/

class GenerateIndexFile : public CCObject
{
	CC_DECLARE_DYNCREATE( GenerateIndexFile )

public:

	// Constructor
	GenerateIndexFile();

	// Destructor
	~GenerateIndexFile() {}

	// Actually generate the index
	BOOL Generate(PathName *IndexFile, PathName *LibDirPath, String_64 *Author,
								SGLibType Type, BOOL Thumbnails);

protected:

	// Close the search handle
	BOOL CloseSearchHandle(void);
	
	// Close the file handle
	BOOL CloseIndexFileHandle(void);

	// Create the index file and keywords file then add headers to them													   
	BOOL CreateIndexFile(void);

	// Count the number of files that will be added to the index file in the directory
	INT32 CountValidFiles(void);

	// If the normal place for the index is read only, sir might consider a more remote location, oui ?
	BOOL RemoteIndexSetup(PathName *FilesPath);

	// Check if the given index has a remote pointer, and return the location if it has
	BOOL CheckExistingIndexForRemoteness(PathName *IndexFile, PathName *FilesPath);

public:

	// Returns true if the directory is read only
	static BOOL IsDirectoryReadOnly(PathName *Directory);

	// Check to see if we've already got a remote directory for a FilesDirectory, or at
	// least return the location for the remote directory
	static BOOL CheckForRemote(PathName *FilesDirectory, String_256 *RemoteDirectory);

	// Given a random collection of garbage as an input string, return a valid directory path on output
	static BOOL GiveMeAValidDirectoryName(String_256 *Input, String_256 *Output);

protected:

	// Add all the header data to an index file
	BOOL DoIndexHeader(void);

	// Scan the directory for files and add them to the index file
	BOOL AddToSubLibrary(void);

	// Poke the count field with the correct value
	BOOL FixSubLibraryNumber(void);

protected:

	// Add a file to the index
	BOOL AddFile(PathName *FileName);

	// Return true if a file should be associated with the index
	static BOOL IsFileType(PathName *FileName, const SGLibType Type);

protected:
	// Get a 16bit value from the file
	INT16 GetShort(FILE *fp);

	// Return the integer part of a 16.16 fixed only
	INT16 GetFixed(FILE *fp);

	// Return a full 32 bit number from a file
	INT32 GetLONG(FILE *fp);

	// Rip out the full TTF typeface name from a font file
	BOOL RipTrueTypeNameFromFile(PathName *FileName, String_256 *Name);

	// Rip out the full ATM typeface and styles from a font file
	BOOL RipATMNameFromFile(PathName *FileName, String_256 *RetName, BOOL *Bold, BOOL *Italic);

protected:
	// This will check that creating the thumbnail won't cause us to go BANG !
	static BOOL PreCreateThumbnail(PathName *InFile);

	// Given an input filename and output filename, generate a thumbnail of specified dimensions
	BOOL CreateThumbnail(PathName *InFile, PathName *OutFile, UINT16 XSize, UINT16 YSize);

	/* Create the thumbnails for the specified file */
	BOOL CreateThumbnails(PathName *FileName, BOOL NewSystem = TRUE);

protected:

	// Scan the given file and try and work out a description and keywords for it
	BOOL RipDescriptionFromFile(PathName *FileName,
					String_256 *Text, String_256 *Keywords = NULL, String_256 *Title = NULL);

	// Try and work out a description and keywords given an EPS/ART filename (uses comments in file)
	BOOL RipDescriptionFromEPSFile(PathName *FileName,
					String_256 *Text, String_256 *Keywords = NULL, String_256 *Title = NULL);

	// Try and work out a description and keywords given a CDR filename
	BOOL RipDescriptionFromCDRFile(PathName *FileName,
					String_256 *Text, String_256 *Keywords = NULL, String_256 *Title = NULL);

	// Try and work out a description and keywords given a version 2 native/web filename
	BOOL RipDescriptionFromNativeV2File(PathName *FileName,
					String_256 *Text, String_256 *Keywords = NULL, String_256 *Title = NULL,
					BOOL *FileIsNewNative = NULL);


	// Scan an old index file looking for the description of the given filename
	BOOL RipDescriptionFromIndex(PathName *IndexFile, PathName *FileName,
					FilePos Start, INT32 DescriptionField, INT32 KeywordField, INT32 TitleField,
					String_256 *Text, String_256 *Keywords = NULL, String_256 *Title = NULL);

	BOOL PrePassIndex(PathName *IndexFile, FilePos *Start,
		INT32 *DescriptionField, INT32 *KeywordField, INT32 *TitleField, String_256 *Description);

public:
	// General utilities for the ripping functions
	static BOOL ReplaceCharacters(StringBase *Source, TCHAR Search, TCHAR Replace);
	static INT32 CountCommas(StringBase *Source, StringBase &SearchFor);
	static BOOL GetToken(StringBase *Source, INT32 Count, String_256 *Result);

protected:
	// Adds the given text to the index file
	BOOL AddLineToIndex(String_256 *Text);

protected:
	PathName Index;
	PathName LibPath;
	SGLibType LibType;
	BOOL DoThumbnails;
	String_64 Author;

	// Disk file for index file
	CCDiskFile *IndexFile;

	// Offset into index for the 'Number of files' data
	FilePos IndexCountOff;

	// Search handle for directory scan		
	HANDLE hSearch;

	// ID of current item and number of items in file
	UINT32 ID;

	// Pre-counted number of files to add to index
	INT32 TotalFiles;

	// Location of old index file (if there was one)
	PathName *TmpIndexFile;

	// Sorted buffer containing all the filenames in the directory
	// ( Initialised by CountValidFiles() )
	IndGenFileBuffer FileNameBuffer;

	// Vars to speed up ripping old data from existing indexes
	FilePos OldIndexStart;
	INT32 OldIndexDescriptionField;
	INT32 OldIndexKeywordField;
	INT32 OldIndexTitleField;
	BOOL OldIndexOK;
	String_256 OldIndexDescription;

	// Remote indexes
	BOOL IndexIsRemote;
	String_256 RemoteLocationOfIndex;
	String_256 RemoteLocationOfFiles;


	// Automatic index update functions
public:
	static UINT32 GetDirectoryChecksum(String_256 *Directory, SGLibType Type = SGLib_Blank);
	static BOOL UseAutomaticUpdate;

public:
	// Preference setting saying whether thumbnail generation is required	
	static BOOL CreateFontIndexes;

	// Preferences to use old index files / rip doc comments out
	static BOOL UseOldIndexes;
	static BOOL UseDocComments;

};

#endif
