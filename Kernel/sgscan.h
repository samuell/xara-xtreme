// $Id: sgscan.h 662 2006-03-14 21:31:49Z alex $
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
// Super Gallery Library scanning stuff

#ifndef INC_SGSCAN
#define INC_SGSCAN

#include "ccfile.h"
#include "thumb.h"

class PathName;
class KernelBitmap;
class SuperGallery;
class SGLibGroup;
class LibraryGallery;
class Progress;

// A library index is a unique identifier for a given entry in a library. The library
// generates it, and it is used as a handle to that entry by outside clients. The index
// is NOT guaranteed to be a sequential number (in fact at present it is a file seek pos!)
typedef INT32 LibraryIndex;

struct SGFieldCache
{
	LibraryIndex Offset;
	UINT32 Count;
	TCHAR *Field;
};


/***********************************************************************************************

>	class Library : public ListItem

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/2/95
	Purpose:	This is the main super gallery library group creation class...
				Creating one of these will create a single group in your gallery. It will
				contain items for each entry in the sub-lib.
	SeeAlso:	LibraryFile;

***********************************************************************************************/

class Library : public ListItem
{
	friend class LibClipartSGallery;
	friend class LibraryGallery;

	CC_DECLARE_DYNCREATE(Library)

public:		// Construction/destruction
	Library();
	~Library();


public:		// Public interface
	// Given a path, scan the {sub}library, and create a supergallery display group
	// with a DisplayItem for each item in the library. Returns number of items found
	INT32 Init(SuperGallery *ParentGal, PathName *Path, String_64 *Title,
				StringBase *IndexFile, SGLibType Type, BOOL Updated = FALSE,
				BOOL AllowedToCreateVirtualised = FALSE);

	// This initialised all the library preference values - and should be called after the above
	static BOOL InitLibPrefs(void);

	// Called during preference wiping to allow us to squirrel away any settings that we desire
	// before they are wiped. Useful, if we want to retain newly installed preferences!
	static BOOL CacheLibPrefs(void);
	static BOOL RestoreLibPrefs(void);

	// Use for creating the items of a virtual library
	INT32 CreateItems(void);

private:
	// Store for the cached settings
	static String_256 g_ClipartLibraryPath;
	static String_256 g_WebThemeLibraryPath;
	static String_256 g_FontLibraryPath;
	static String_256 g_FillsLibraryPath;

	// Setup various member variables - used by Init, and only Init
	BOOL SetUpMembers(SuperGallery *ParentGal, PathName *Path, String_64 *SLTitle,
			StringBase *SubI, SGLibType SLType);

	// Setup the QuickIndex member variables
	BOOL SetupQuickIndexMembers(PathName *Path, BOOL CheckTheyExist);

	// Read the no of items, field text and first line values
	INT32 ReadHeaderInfo(CCLexFile *pSubIndex);

	// Scan the rest of the file, creating items for each entry
	BOOL ScanRestOfFile(CCLexFile *pSubIndex);

public:			// Public interface for getting at library information
	// Returns the title (as displayed in the gallery group) of this library
	void GetLibraryTitle(StringBase *ReturnedTitle);

	//>> webster (Adrian 2/01/97)
	// Returns the status of this library, can be one of the following values defined in inetop.h:
	// FOLDER_UNMODIFIED
	// FOLDER_UPDATED
	// FOLDER_NEW
	// (Meaningful only for web folders)
	inline UINT32 GetModified() { return m_nModified;}

	inline void SetModified(UINT32 nModified) { m_nModified = nModified;};

	// Return the type of this library
	inline SGLibType GetType() { return Type;}
	//<<webster

	// Return the Filename for line at offset
	BOOL GetFilename(LibraryIndex Offset, StringBase *File, BOOL FullPath = TRUE);

	// Return ptr to Filename (only works for memory cached indexes)
	BOOL GetFilename(LibraryIndex Offset, TCHAR **File);
	
	// Return the textual description for line at offset
	BOOL GetTextname(LibraryIndex Offset, StringBase *Text);

	// Return ptr to textual description (only works for memory cached indexes)
	BOOL GetTextname(LibraryIndex Offset, TCHAR **Text);

	// Return the ID for line at offset
	UINT32 GetID(LibraryIndex Offset);

	// Get the title from the index
	BOOL GetTitle(LibraryIndex Offset, StringBase *Title);

	// Get the title from the index (just ptr)
	BOOL GetTitle(LibraryIndex Offset, TCHAR **Title);

	// Return a pointer to the thumbnail for line at offset
	BOOL GetThumbnail(LibraryIndex Offset, SGThumbSize Size, BOOL Urgent,
						KernelBitmap **Thumbnail);

	// Wipe clean the thumbnail cache and return all memory
	void KillAllThumbnails(void);

	// Return the default bitmap width (pixels)
	inline INT32 PreviewBMPWidth(SGThumbSize Size)
	{
		if((INT32)Size < 3) return PreviewX[(INT32)Size];
		else return 64;
	}

	// Return the default bitmap width (pixels)
	inline INT32 PreviewBMPHeight(SGThumbSize Size)
	{
		if((INT32)Size < 3) return PreviewY[(INT32)Size];
		else return 64;
	}

	// Return the full pathname of the index file
	inline PathName *ReturnIndexLocation(void) {return IndexFile;}

	// Return the full pathname of the sub index data directory
	inline PathName *ReturnSubLibraryLocation(void) {return SubLibPath;}

	// Return the full pathname of the index file
	inline PathName *ReturnSynonymFile(void) {return SynonymFile;}

	// Function to convert a PathName to an URL
	// The conversion is based on the base URL stored in URLBase
	BOOL LocalPath2URL(String_256* pLocalPath);

	// Return a single field from a line of text (start from field 1)
	// Use this for extracting the extended info fields...
	BOOL GetSingleField(LibraryIndex Offset, UINT32 Count, StringBase *Field);

	// Return a ptr to a single field from a line of text (start from field 1)
	BOOL GetSingleField(LibraryIndex Offset, UINT32 Count, TCHAR **Field);

	// Return a single field from a line of text
	// Field is referenced by field header text at start
	BOOL GetSingleField(LibraryIndex Offset, String_64 *FieldName, StringBase *Field);

	// Return a single field from a line of text
	// Field is referenced by field header text at start
	// Quicker, pointer version...
	BOOL GetSingleField(LibraryIndex Offset, String_64 *FieldName, TCHAR **Field);

	// Return field number for given field...
	INT32 FindField(String_64 *FieldName);

	// Check if a string is in the field description string...
	BOOL GotField(String_64 *FieldName);

	inline BOOL IsWebLibrary() {return m_bIsWebLib;} 


public:
	// Save out an index file in the order the group is displayed
	// This is strictly a debug only function !
	BOOL SaveIndexInDisplayedOrder(PathName *IndexFile, BOOL NewIDs);
	// >>webster (adrian 17/12/96)
	static String_256 URLBase;
	
	BOOL m_bIsWebLib;
	// <<webster
protected:		// Private data

	SGLibType Type;							// Type of Sub-Library

	INT32 ItemCount;							// Count of items in the sub-lib

	String_256 FieldString;					// Field string found at the head of the sub-index

	
	LibraryIndex FirstLineOffset;			// For checking the Offsets...
	LibraryIndex LastLineOffset;

	INT32 PreviewX[3];						// Widths of thumbnails
	INT32 PreviewY[3];						// Heights of thumbnails

	UINT32 IndexDateStamp;					// For recache checking

	//>> webster (Adrian 2/01/96)
	UINT32 m_nModified;
	INT32 m_lOldIndex;
	//<< webster

public:

	// These need to be public for the GRM code...
	String_64 *Title;						// Title text as given in main index file

	PathName *IndexFile;					// Full Path of index file itself
	PathName *SubLibPath;					// Full Path of the sub library
	PathName *SynonymFile;					// Full Path of the synonym file

	// 'Remove' needs these public
	SuperGallery	*ParentGallery;			// Gallery we're attached to
	SGLibGroup		*ParentGroup;			// Group (within that gallery) we're displayed in
	
	// Resizing and deleting messages require access to this...
	SGThumbs *Thumbnails;					// Thumbnail cache class

	// **** Speedup routines follow

public:

	// Gallery open / close memory routines...
	BOOL FreeCachedIndexes();
	BOOL ReCacheIndexes();

protected:	// Sub index file caching stuff

	// Cache for the subindex file
	BOOL CacheSubIndex(PathName *IndexFile);

	// After reading the header info out of the file, scan the rest (optionally) creating
	// items for each valid new line, and replacing all the commas with 0's...
	BOOL ScanRestOfFile(TCHAR *CachedIndex, INT32 Start, INT32 Finish, BOOL AddItems = TRUE);

	// Return a field from the cached version of the index file
	BOOL GetSingleFieldFromMemory(LibraryIndex Offset, UINT32 Count, StringBase *Field);

	// Return a pointer to a field from the cached version of the index file
	BOOL GetSingleFieldFromMemory(LibraryIndex Offset, UINT32 Count, TCHAR **Field);

	// Return a field using a diskfile for the index
	BOOL GetSingleFieldFromDisk(LibraryIndex Offset, UINT32 Count, StringBase *Field);

	// Buffer for the file itself
	TCHAR *CachedSubIndexBuf;

	// Size of Cached index file in buffer
	INT32 CachedBufLength;

public:
	// If non-null, ProgressBar->Update() will be called during bits which could take time
	// such as de-virtualising a group
	static Progress *ProgressBar;

protected:	// Field caching stuff

	BOOL CacheInit(INT32 Entries);
	BOOL CacheKill(void);
	INT32 CacheHashFunction(LibraryIndex Offset, UINT32 Count);
	BOOL CachePut(LibraryIndex Offset, UINT32 Count, StringBase *Field);
	BOOL CacheGet(LibraryIndex Offset, UINT32 Count, StringBase *Field);

	INT32	FieldCacheEntries;
	SGFieldCache *FieldCache;

protected:	// Quick Index stuff

	BOOL QuickIndexEnabled;
	PathName *QuickIndexFile;

public:
	// Maximum number of entries in the field cache
	static INT32 MaxFieldCacheEntries;

	// Should we background redraw the library galleries ?
	static BOOL BackgroundRedraw;

	// Should we cache sub index files ? Makes things go quite a bit quicker...
	static BOOL CacheIndexFile;

	// Enable 'remote' library indexes if target media is read-only...
	static BOOL RemoteIndexes;

	// Location for 'remote' library indexes (if empty, use temp)...
	static String_256 RemoteIndexLocation;

	// Quick index preference - use same indexes in ..\xarainfo, as opposed to \xarainfo
	static BOOL QuickIndex;
};

#endif
