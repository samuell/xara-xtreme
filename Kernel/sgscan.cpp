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

// Gallery library FILE SCANNING classes

/*
*/

#include "camtypes.h"
#include "sgscan.h"
#include <string.h>
#include <io.h>
#include <fcntl.h>
#include <sys\stat.h>
#include "sgscanf.h"
#include "bitmap.h"
#include "ccfile.h"
#include "pathname.h"
#include "thumb.h"
//#include "resource.h"
//#include "richard.h"
#include "fixmem.h"
#include "app.h"		// for pref stuff...
#include "progress.h"
#include "sgallery.h"
#include "sglib.h"
#include "sgtree.h"
#include "sgfonts.h"
#include "sglcart.h"
#include "sglfills.h"
#include "sglbase.h"
#include "sgindgen.h"
#include "sgliboil.h"
//#include "simon.h"		// _R(IDS_CONTINUE)
#include "atminst.h"
//#include "richard2.h"
#include "camnet.h"
#include "pathnmex.h"

DECLARE_SOURCE("$Revision$");

CC_IMPLEMENT_DYNCREATE(Library, ListItem)

#define new CAM_DEBUG_NEW

using namespace InetUtils;

// Field cache static
INT32 Library::MaxFieldCacheEntries = 25;

// Background redraw the libraries ?
BOOL Library::BackgroundRedraw = TRUE;

// Should we eat loads of memory up by caching the index files ?
BOOL Library::CacheIndexFile = TRUE;

// Enable 'remote' library indexes if target media is read-only...
BOOL Library::RemoteIndexes = TRUE;

// Location for 'remote' library indexes (if empty, use temp)...
String_256 Library::RemoteIndexLocation = "";

// >>webster (adrian 17/12/96)
// Base URL for web folders
String_256 Library::URLBase = _T("");
// <<webster

// Quick index stuff - use same indexes in ..\xarainfo, as opposed to \xarainfo
BOOL Library::QuickIndex = TRUE;

// If non-null, ProgressBar->Update() will be called during bits which could take time
// such as de-virtualising a group
Progress *Library::ProgressBar = NULL;

// Only define this if you want field caching...
#define FIELDCACHE

// Should we report problems with indexes in debug builds
#define DONT_REPORT_KNOWN_CLIPART_PROBLEMS

// Only define if you want the 'SaveIndex' code for the button compiled...
#define SAVEINDEX

// Store for the cached settings
String_256 Library::g_ClipartLibraryPath	= TEXT("");
String_256 Library::g_WebThemeLibraryPath	= TEXT("");
String_256 Library::g_FontLibraryPath		= TEXT("");
String_256 Library::g_FillsLibraryPath		= TEXT("");

/***********************************************************************************************

>	Library::Library()

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/2/95

	Purpose:	Constructor

***********************************************************************************************/

Library::Library()
{
	ParentGallery = NULL;
	ParentGroup = NULL;

	Title = NULL;
	IndexFile = NULL;
	Thumbnails = NULL;
	SubLibPath = NULL;
	SynonymFile = NULL;
						
	ItemCount = 0;
	Type = SGLib_Blank;

	FirstLineOffset = LastLineOffset = 0;

	for(INT32 i=0; i<3; i++)
	{
		PreviewX[i] = 64;						// Widths of thumbnails
		PreviewY[i] = 64;						// Heights of thumbnails
	}

	// Add in a field cache for the library
#ifdef FIELDCACHE
	FieldCache = NULL;
	CacheInit(MaxFieldCacheEntries);
#endif
	
	// No sub index cached yet...
	//CachedSubIndex = NULL;
	CachedSubIndexBuf = NULL;
	CachedBufLength = 0;

	// Datestamp of last index cached
	IndexDateStamp = 0;

	// Quick Index stuff...
	QuickIndexEnabled = FALSE;
	QuickIndexFile = NULL;

	// Progress bar stuff
	ProgressBar = NULL;
	//>> webster (Adrian 02/01/97)
	m_nModified = 0;
	m_bIsWebLib = FALSE;
	m_lOldIndex = 0;
	//<< webster
}



/***********************************************************************************************

>	Library::~Library()

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/2/95

	Purpose:	Time to die...

***********************************************************************************************/

Library::~Library()
{
	TRACEUSER( "Richard", _T("~Library called\n"));

	// Ensure our parent gallery is no longer displaying us
	if (ParentGallery != NULL)
	{
		ParentGallery->RemoveLibraryGroup(this);
		ParentGallery = NULL;
	}

	if(Title != NULL)
	{
		delete Title;
		Title = NULL;
	}

	if(IndexFile != NULL)
	{
		delete IndexFile;
		IndexFile = NULL;
	}

	if(Thumbnails != NULL)
	{
		delete Thumbnails;
		Thumbnails = NULL;
	}
	
	if(SubLibPath != NULL)
	{
		delete SubLibPath;
		SubLibPath = NULL;
	}

	if(SynonymFile != NULL)
	{
		delete SynonymFile;
		SynonymFile = NULL;
	}

	// Reclaim field cache memory					 	
#ifdef FIELDCACHE
	CacheKill();
#endif

	// Free the CachedSubIndex file buffer
	if(CachedSubIndexBuf != NULL)
	{
		CCFree((void *)CachedSubIndexBuf);
		CachedSubIndexBuf = NULL;
	}

	CachedBufLength = 0;


	// And reclaim all memory used by the cached subindex itself
//	if(CachedSubIndex != NULL)
//	{
//		CachedSubIndex->DeinitLexer();	
//		if(CachedSubIndex->isOpen())
//			CachedSubIndex->close();
//		delete CachedSubIndex;
//	}


	if(QuickIndexFile != NULL)
		delete QuickIndexFile;
}

/***********************************************************************************************

>	BOOL Library::FreeCachedIndexes(void)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/6/95

	Returns:	TRUE if succesful and previously cached indexes are no more...

	Purpose:	Free some memory when we close a gallery by freeing all the cached index
				files...

***********************************************************************************************/

BOOL Library::FreeCachedIndexes(void)
{
	// Reclaim field cache memory
#ifdef FIELDCACHE
	CacheKill();
#endif

	// Free the CachedSubIndex file buffer
	if(CachedSubIndexBuf != NULL)
	{
		CCFree((void *)CachedSubIndexBuf);
		CachedSubIndexBuf = NULL;
		CachedBufLength = 0;
		return TRUE;
	}

	return FALSE;
}


/***********************************************************************************************

>	BOOL Library::ReCacheIndexes(void)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/6/95

	Returns:	TRUE if succesful and indexes have been (re-)cached...
				FALSE if problems
				
	Purpose:	Re-caches the indexes when opening a gallery...

***********************************************************************************************/

BOOL Library::ReCacheIndexes(void)
{
	// Don't recache indexes if group is v-d out...
	if(ParentGroup != NULL && ParentGroup->IsVirtualised())
		return TRUE;

	// Sort out the field cache
#ifdef FIELDCACHE
	CacheKill();
	CacheInit(MaxFieldCacheEntries);
#endif

	// Recache the indexes
	if(CachedSubIndexBuf == NULL && CacheIndexFile && IndexFile != NULL)
	{
		// Check the index file still exists
		//if(!SGLibOil::FileExists(IndexFile))
		//	return FALSE;

		// See if the index file has changed...
		//UINT32 NewIndexStamp = SGLibOil::FileModified(IndexFile);
		//if(IndexDateStamp != NewIndexStamp)
		//	return FALSE;

		if(CacheSubIndex(IndexFile))
		{
			// Whizz through index, inserting 0's for commas and sorting out spaces, etc...
			return ScanRestOfFile(CachedSubIndexBuf, FirstLineOffset, CachedBufLength, FALSE);
		}
		else
		{
			return FALSE;
		}
	}

	return TRUE;
}

/***********************************************************************************************

>	BOOL Library::CacheSubIndex(PathName *IndexFile)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/4/95

	Inputs:		IndexFile - Pathname of index file which we want to cache as a memfile
	Outputs:
	Returns:	TRUE if the file was successfully cached and opened

	Purpose:	Creates a CCMemTextFile with the given file in, and opens it. The idea
				being that reading from a file in memory will be quicker than off a
				very slow single speed CD ROM drive.

	Notes:		There are three ways of getting at the information in the library sub
				index at the moment, a field cache, this index file cache and then lexing
				over the diskfile itself. Obviously the first two are the most desirable,
				but take memory up. The field cache we use is fairly good, once the items
				have been read. This initial reading of the items is the slow bit, so that's
				why we've got the option to cache the entire index file in memory as well.

	SeeAlso:	Library::GetSingleField, Library::Init

***********************************************************************************************/

BOOL Library::CacheSubIndex(PathName *IndexFile)
{
	BOOL OpenedOK = FALSE;

	// Doesn't bother with modified calls anymore, uses file size...
	INT32 NewFileSize = 0;
	
	if(QuickIndexEnabled)
		NewFileSize = SGLibOil::FileSize(QuickIndexFile);
	else
		NewFileSize = SGLibOil::FileSize(IndexFile);

	if(NewFileSize == 0)
		return FALSE;

	if(IndexDateStamp != 0 && IndexDateStamp != (UINT32)NewFileSize)
		return FALSE;
	IndexDateStamp = (UINT32)NewFileSize;

	CachedBufLength = NewFileSize;
	CachedSubIndexBuf = (TCHAR *)CCMalloc(CachedBufLength + 4);

	if(CachedSubIndexBuf != NULL)
	{
		CCDiskFile TmpDiskFile;
		TRY
		{
			if(QuickIndexEnabled)
				OpenedOK = TmpDiskFile.open(*QuickIndexFile, ios::in);
			else
				OpenedOK = TmpDiskFile.open(*IndexFile, ios::in);

			if(!OpenedOK)
			{
				ERROR3("Couldn't open disk file for mem file");
				if(CachedSubIndexBuf != NULL)
					CCFree(CachedSubIndexBuf);
				CachedSubIndexBuf = NULL;
				CachedBufLength = 0;
				return FALSE;
			}

			TmpDiskFile.read(CachedSubIndexBuf, CachedBufLength);		
			TmpDiskFile.close();
		}
		CATCH(CFileException, e)
		{	
			if(CachedSubIndexBuf != NULL)
				CCFree(CachedSubIndexBuf);
			CachedSubIndexBuf = NULL;
			CachedBufLength = 0;
			if(TmpDiskFile.isOpen())
				TmpDiskFile.close();
			return FALSE;
		}
		END_CATCH
	}
	else
	{
		CachedBufLength = 0;
		ERROR3("Couldn't allocate space for cached subindex file");
		return FALSE;
	}
	return TRUE;
/* FIX */
}

/***********************************************************************************************

>	INT32 Library::Init(SuperGallery *ParentGal, PathName *Path, String_64 *SLTitle,
					StringBase *SubI, SGLibType SLType, BOOL Updated = FALSE,
					BOOL AllowedToCreateVirtualised = FALSE)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/2/95

	Inputs:		ParentGal	- The parent gallery into which the group should be added.
				Path		- The pathname of the library directory - C:\testlib\animals
				SLTitle		- The title text for this sublibrary
				SubI		- The SubIndex filename - XaraClip.txt
				SLType		- The expected type of the sublibrary
				Updated		- True if index has just been updated
				AllowedToCreateVirtualised - if set to FALSE we won't create a virtualised-out group

	Returns:	0 if an error occurred, -1 if the group was created in a virtualised out stage
				(in which case we'll need to be initialised again to create the items), else a
				count of the number of items in this library
				

	Purpose:	Given a specified sublibrary index, scan it and create a supergallery
				display group for it, containing display items for all of the items in
				the index.

				If the group already exists in the gallery, it will be replaced (recreated).

	Notes:		If you want the Quiet button code to work you need to call
					LibraryGallery->SetQuietStatus(FALSE);
				before calling this. Note that a large stream of new additions should call this
				just once for the first item. If you don't do this you won't get retry capability
				if the user's CD isn't in the drive...

				For the sake of progress bars and delayed hourglass popping up, we now call
				ProgressBar->Update(), if the static Library::ProgressBar is non-null... 

***********************************************************************************************/
				
INT32 Library::Init(SuperGallery *ParentGal, PathName *Path, String_64 *SLTitle,
					StringBase *SubI, SGLibType SLType, BOOL Updated, BOOL AllowedToCreateVirtualised)
{
	if(ParentGal == NULL || Path == NULL || SLTitle == NULL || SubI == NULL)
	{
		ERROR3("Library::Library - NULL parameters are illegal");
		return 0;
	}

//	String_256 P(Path->GetPath());
//	ERROR3_PF(("LibraryInitPath = %s", (TCHAR *)P));
//	ERROR3_PF(("LibraryInit SubI = %s", (TCHAR *)*SubI));
	if(ProgressBar != NULL) ProgressBar->Update();

	// Set up some of the member variables for this library object
	if(!SetUpMembers(ParentGal, Path, SLTitle, SubI, SLType))
		return 0;

	// Should we be creating items, etc at this point ?
	if(SGLibGroup::LibraryVirtualisingEnabled && ParentGroup == NULL && AllowedToCreateVirtualised)
	{
		ParentGroup = (SGLibGroup *)ParentGallery->AddLibraryGroup(this, -1);
		ParentGroup->SetVirtualisedState(TRUE);
		return -1;
	}

	// Sort the Quick Index members out. This doesn't need doing for virtualised out groups
	QuickIndexEnabled = SetupQuickIndexMembers(Path, TRUE);

	CCLexFile *SubIndex = NULL;
	BOOL CachedSubIndex = FALSE;
	BOOL OpenedOK = FALSE;
	BOOL Retry = FALSE;

	// 'Quiet' button handling when no indexes found...
	BOOL Quiet = FALSE;
	LibraryGallery *LG = NULL;
	if(ParentGal->IsKindOf(CC_RUNTIME_CLASS(LibraryGallery)))
		LG = (LibraryGallery *)ParentGal;
	else
	{
		ERROR3("Library::Init called with a non-library gallery gallery");
		return 0;
	}

	// Extra bit of code added to let the user 'retry' for their CD if we can't find it.
	do
	{
		Retry = FALSE;

		// Try to cache the index file for quicker access later on
		if(CacheIndexFile && !CachedSubIndex)
			CachedSubIndex = CacheSubIndex(IndexFile);
		
		TRY
		{
			// Use a disk file for this bit
			SubIndex = new CCDiskFile(1024, FALSE, TRUE);
			if(SubIndex != NULL)
			{
				if(QuickIndexEnabled)
					OpenedOK = ((CCDiskFile *)SubIndex)->open(*QuickIndexFile, ios::in);
				else
					OpenedOK = ((CCDiskFile *)SubIndex)->open(*IndexFile, ios::in);
			}
			else
			{
				ERROR3("Library::Init SubIndex not created properly");
				return(0);
			}
		}

		if(ProgressBar != NULL) ProgressBar->Update();

		CATCH(CFileException, e)
		{
			// Looks like the index file doesn't exist - or we can't get to it anyhow...
			// This only happens on first opening the gallery at the moment. Since this
			// is a pretty good candidate for not having the CD present, the below code was
			// added. Of course we don't know it's the CD that's not present, they could
			// have vaped their TEMP directory, or done something else obscenely stupid (well,
			// sensible really)... Who knows... Anything can and does happen...
			if(SubIndex != NULL)
			{
				INT32 LastButtonPressed = 1;

				Quiet = LG->GetQuietStatus();
			
				if(!Quiet)
				{
					String_256 WarnMsg;

					if(!SGLibGroup::LibraryVirtualisingEnabled)
					{
						// Old system where groups and items are all created when the gallery is opened
						WarnMsg.MakeMsg(_R(IDS_LIBRARY_CANT_FIND_INDEX), (const TCHAR *)Path->GetFileName(FALSE));
						Error::SetError(0, WarnMsg, 0);
						LastButtonPressed = InformWarning(0, _R(IDS_CONTINUE), _R(IDS_RETRY), _R(IDS_QUIET), NULL, 2, 1);
						Retry  = (LastButtonPressed == 2);
						Quiet  = (LastButtonPressed == 3);
					}
					else
					{
						// New system when this will only be called when de-virtualising all groups
						WarnMsg.MakeMsg(_R(IDS_LIBRARY_CANT_FIND_INDEX_SKIP), (const TCHAR *)Path->GetFileName(FALSE));
						Error::SetError(0, WarnMsg, 0);
						LastButtonPressed = InformWarning(0, _R(IDS_SKIP), _R(IDS_RETRY), _R(IDS_QUIET), NULL, 2, 1);
						Retry  = (LastButtonPressed == 2);
						Quiet  = (LastButtonPressed == 3);
					}

					LG->SetQuietStatus(Quiet);
				}
				if (SubIndex->isOpen())
					SubIndex->close();
				delete SubIndex;
				SubIndex = NULL;
			}
			Error::ClearError();
			if(!Retry)
				return 0;
		}
		END_CATCH

		// This can't happen, but just in case !
		if(Quiet && Retry)
			Retry = FALSE;
	} while(Retry);

	if(ProgressBar != NULL) ProgressBar->Update();

	// Subindex seeking required
	if (!SubIndex->InitLexer(TRUE))
	{
		ERROR3("Library::Init InitLexer failed");
		if(SubIndex->isOpen())
			SubIndex->close();
		delete SubIndex;
		return FALSE;
	}

	SubIndex->SetWhitespace("");			// Setting this to blank lets us read non-"'d strings
	SubIndex->SetDelimiters(",");		// ,s delimit our fields
	SubIndex->SetCommentMarker('#');		// #'d lines are commented out
	SubIndex->SetStringDelimiters("");	// No string delimiters

	if(OpenedOK)
	{
		// Read the header
		INT32 NumItems = ReadHeaderInfo(SubIndex);		

		if(ProgressBar != NULL) ProgressBar->Update();

		if (NumItems > 0)
		{
			// Remove any pre-existing groups with the same index file
			((LibraryGallery *)ParentGal)->RemoveSimilarLibraryGroups(SubLibPath, (String_256 *)SubI, TRUE, FALSE, this);

			// Create/find the required group in the parent gallery
			ParentGroup = (SGLibGroup *)ParentGallery->AddLibraryGroup(this, NumItems);
			ParentGroup->SetVirtualisedState(FALSE);

			if (ParentGroup == NULL)		// Couldn't get a display group! Argh!
			{
				// reclaim lexer-buffer memory
				SubIndex->DeinitLexer();	

				// and close the file...
				if(SubIndex->isOpen())
					SubIndex->close();

				// and reclaim the diskfile memory...
				delete SubIndex;
				return FALSE;
			}

			if(ProgressBar != NULL) ProgressBar->Update();

			// And construct all the items...
			if(CachedSubIndexBuf == NULL)
				ScanRestOfFile(SubIndex);
			else
				ScanRestOfFile(CachedSubIndexBuf, FirstLineOffset, CachedBufLength);

			if(ProgressBar != NULL) ProgressBar->Update();

#ifdef _DEBUG
			if(NumItems != ItemCount)
			{
				String_256 Warn;
				PathName Loc(IndexFile->GetLocation(FALSE));
				PathName Loc2(Loc.GetLocation(FALSE));
				String_256 FNameWarn(Loc2.GetFileName());

				BOOL DoWarning = TRUE;
				
#ifdef DONT_REPORT_KNOWN_CLIPART_PROBLEMS
				// Don't warn if doing Studio and only 1 off - The Infamous Rolex Bodge !
				if((ItemCount == (NumItems - 1)) && FNameWarn == (String_256)"Studio")
					DoWarning = FALSE;

				// Corel Xara CD (rel 1) bodges
				if(ItemCount == 50 && NumItems == 48 && FNameWarn == (String_256)"Xara")
					DoWarning = FALSE;
				if(ItemCount == 329 && NumItems == 330 && FNameWarn == (String_256)"ANIMALS")
					DoWarning = FALSE;
				if(ItemCount == 470 && NumItems == 474 && FNameWarn == (String_256)"PEOPLE")
					DoWarning = FALSE;
#endif
				if(DoWarning)
				{
					ERROR3_PF(("Dodgy checksum for '%s' index file. Header value should read %d.", (TCHAR *)FNameWarn, ItemCount));
				}
			}
#endif
		}
	}
	else
	{
		// Failed to read the sub index file...
		ERROR3("Library::Library couldn't open sub index file");
	}

	if(ProgressBar != NULL) ProgressBar->Update();

	// reclaim lexer-buffer memory
	SubIndex->DeinitLexer();	

	// and close the file...
	if(SubIndex->isOpen())
		SubIndex->close();

	// and reclaim the diskfile memory...
	delete SubIndex;

	// Thumbnail cache class
	Thumbnails = NULL;

//	String_256 IndexPath(IndexFile->GetPath());
//	ERROR3_PF(("IndexPath = %s", (TCHAR *)IndexPath));
		
	// Path for thumbnails - this is INCORRECT !!!
//	String_256 ThuF(Path->GetPath(TRUE));
	
//	PathName IndexP(IndexFile);
	String_256 ThuF(IndexFile->GetLocation(FALSE));
	LibraryFile::TidyUpSubPath(&ThuF);
	SGLibOil::AppendSlashIfNotPresent(&ThuF);

	ThuF += String_16(_R(IDS_LIBRARIES_XARAINFO_DIRNAME));
	PathName ThumbPath(ThuF);
	if(!ThumbPath.IsValid())
	{
		ERROR3("Library::Init ThumbPath is invalid");
		return 0;
	}

//	ERROR3_PF(("Path = %s, SubLibPath = %s, IndexFile = %s", Path->GetPath(), SubLibPath->GetPath(), IndexFile->GetPath()));

	// Allocate memory for new thumbnail cache and point to it
	Thumbnails = new SGThumbs(&ThumbPath, SLType, SGThumb_Small);

	return(ItemCount);
}


/***********************************************************************************************

>	INT32 Library::CreateItems(void)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/1/96

	Returns:	0 if an error occurred, else a count of the items added

	Purpose:	Mainly for use by the de-virtualise function... Init MUST have been called
				previously for this to work !

  	Notes:		If you want the Quiet button code to work you need to call
					LibraryGallery->SetQuietStatus(FALSE);
				before calling this. Note that a large stream of new additions should call this
				just once for the first item. If you don't do this you won't get retry capability
				if the user's CD isn't in the drive...

***********************************************************************************************/

INT32 Library::CreateItems(void)
{
	// Don't do this it the group isn't v'd out
	if(ParentGroup != NULL && ParentGroup->IsVirtualised())
	{
		String_256 SubI;
		SubI = IndexFile->GetFileName();
		PathName pathOldIndex(IndexFile->GetPath());
		pathOldIndex.SetType(_T("old"));
		INT32 hOldIndex = _open(pathOldIndex.GetPath(), _O_RDONLY | _O_BINARY, _S_IREAD);
		if (hOldIndex != -1)
		{
			m_lOldIndex = _filelength(hOldIndex);
			_close(hOldIndex);
		}
		INT32 ItemCount = Init(ParentGallery, SubLibPath, Title, &SubI, Type);
		return ItemCount;
	}

	return 0;
}

/***********************************************************************************************

>	BOOL Library::SetUpMembers(SuperGallery *ParentGal, PathName *Path, String_64 *SLTitle,
					StringBase *SubI, SGLibType SLType)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/5/95

	Inputs:		ParentGal	- The parent gallery into which the group should be added.
				Path		- The pathname of the library directory - C:\testlib\animals
				SLTitle		- The title text for this sublibrary
				SubI		- The SubIndex filename - XaraClip.txt
				SLType		- The expected type of the sublibrary

	Returns:	TRUE if things went OK...

	Purpose:	Fill in the member variables with the required paths, and things...
				Should only be called from ::Init

***********************************************************************************************/
				
BOOL Library::SetUpMembers(SuperGallery *ParentGal, PathName *Path, String_64 *SLTitle,
					StringBase *SubI, SGLibType SLType)
{
	ERROR3IF(ParentGal == NULL || Path == NULL || SLTitle == NULL || SubI == NULL, "Library::SetUpMembers - NULL parameters are illegal");

//	String_256 P(Path->GetPath());
//	ERROR3_PF(("Path = %s", (TCHAR *)P));

	ParentGallery = ParentGal;

	// Number of (active) items in sub-lib
	ItemCount = 0;

	// Field string found at the head of the sub-index 
	FieldString = "";

	// Type of Sub-Library
	Type = SLType;

	// Title text as given in main index file (used for the group name)
	// "A group containing pictures of animals" for example
	if(Title == NULL)
	{
		Title = new String_64(*SLTitle);
		if (Title == NULL)
			return FALSE;
	}

	// Add index path to index filename
	String_256 General(Path->GetPath(TRUE));
	General += String_16(_R(IDS_LIBRARIES_XARAINFO_DIRNAME));
	General += TEXT("\\");
	General += *SubI;

	// IndexFile now contains the full path to the index filename
	// "C:\testlib\animals\xarainfo\xaraclip.txt" for example
	if(IndexFile == NULL)
	{
		IndexFile = new PathName(General);
		if (IndexFile == NULL)
			return FALSE;
	}

	if(!IndexFile->IsValid())
	{
		ERROR3("Library::SetUpMembers indexfile is invalid");
		return FALSE;
	}

	// Full path of library directory -> C:\testlib\animals
	if(SubLibPath == NULL)
	{
		SubLibPath = new PathName(*Path);
		if(SubLibPath == NULL)
			return FALSE;
	}

	if(!SubLibPath->IsValid())
	{
		ERROR3("Library::SetUpMembers SubLibPath is invalid");
		return FALSE;
	}

	// For checking the Offsets...
	FirstLineOffset = 0;
	LastLineOffset = 0;

	return TRUE;
}



/***********************************************************************************************

>	BOOL Library::SetupQuickIndexMembers(PathName *Path, BOOL CheckTheyExist)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/1/96

	Inputs:		Path			- Path for normal index
				CheckTheyExist	- Checks with the filesystem for the file's existance

	Returns:	TRUE if paths set up and files found
				Note: We will ALWAYS return FALSE if CheckTheyExist is FALSE

	Purpose:	Given a normal index file path, set up members to a 'quick index file'. The
				idea is for groups of library directories to have copies of all their indexes
				in some common folder so that loading them in doesn't thrash the CD drive too
				much.

***********************************************************************************************/

BOOL Library::SetupQuickIndexMembers(PathName *Path, BOOL CheckTheyExist)
{
	QuickIndexEnabled = FALSE;

	// Quick indexes are just a slight bodge for speeding up the gallery opening times with
	// CD's... Basically instead of hunting all over the place for indexes, we stick all the
	// indexes down the XaraInfo directory (with the main index.txt), and open these instead
	// of the normal ones... If these don't exist, then obviously we can't use them...
	if(Library::QuickIndex)
	{
		// Work out Quick path
		PathName QuickPath(Path->GetPath());
		String_256 QuickPathLoc(QuickPath.GetLocation(FALSE));
		QuickPathLoc += TEXT("\\") + String_16(_R(IDS_LIBRARIES_XARAINFO_DIRNAME)) + TEXT("\\");
		QuickPathLoc += Path->GetFileName(FALSE);
		QuickPathLoc += TEXT(".txt");
		QuickPath.SetPathName(QuickPathLoc);
		
		if(CheckTheyExist && SGLibOil::FileExists(&QuickPath))
		{
			if(QuickIndexFile == NULL)
			{
				QuickIndexFile = new PathName(QuickPathLoc);
						
				if(QuickIndexFile != NULL)
					QuickIndexEnabled = TRUE;
			}
		}

		Error::ClearError();
	}

	return QuickIndexEnabled;
}


/***********************************************************************************************

>	INT32 Library::ReadHeaderInfo(CCLexFile *pSubIndex)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/2/95

	Inputs:		pSubIndex - the index file to read the header from

	Returns:	The number of items that should be found in this index file
				Returns 0 if there are no items, or if there was an error

	Purpose:	Read the no of items, field text and line pSubIndex up with the first
				proper index line

	Notes:		On Exit, the file pointer in pSubIndex will point at the first line
				of the index body

***********************************************************************************************/

INT32 Library::ReadHeaderInfo(CCLexFile *pSubIndex)
{	
	ERROR3IF(pSubIndex == NULL, "Library::ReadHeaderInfo - NULL params are illegal");

	BOOL ok = TRUE;
	BOOL retok = TRUE;
	INT32 Items = 0;

	LexTokenType TT = TOKEN_EOL;
	UINT32 NonGarbageLineNumber = 0;		
	BOOL HitStart = FALSE;
	while(ok && retok && !HitStart/*NonGarbageLineNumber < 5*/)
	{
		if (!pSubIndex->GetToken()) break;

		// Keep reading tokens until we hit a normal one... (skips line ends and
		// comments for us
		TT = pSubIndex->GetTokenType();		

		while (TT != TOKEN_NORMAL && ok)
		{
			// Bodge so that lines starting #DESCRIPTION: case us to jump out...
			if(TT == TOKEN_COMMENT)
			{
				// Optional header extras - Must have their name followed by a ':' then the item
				if(camStrchr((TCHAR *) pSubIndex->GetTokenBuf(), _T(':')))
				{
					// We assume these lines aren't going to be bigger than 256 chars
					String_256 TokenBufU(pSubIndex->GetTokenBuf());
					TokenBufU.toUpper();
	
					INT32 Offset = TokenBufU.Sub(String_16(_R(IDS_LIBRARIES_INDEX_DESCRITION)));

					if(Offset > -1 && Offset < 2)
					{
						// Need to set 'title'
						if(Title)
						{
							String_256 TokenBuf(pSubIndex->GetTokenBuf());
							TokenBuf.Right(Title, TokenBuf.Length() - (12 + Offset));				
							LibraryFile::KillLeadingSpaces(Title);
						}
					}
				}
			}

			ok = pSubIndex->GetToken();
			if(!ok) break;
			TT = pSubIndex->GetTokenType();		
			ok = (TT != TOKEN_EOF);
			if(!ok) break;
		}
		if(!ok) break;

		NonGarbageLineNumber++;		
		
		switch(NonGarbageLineNumber)
		{
			case 0: // Something nasty's happened
				ERROR3("Library ReadHeader - Something nasty has happened");
				break;

			case 1:	// Number of files described by index
				Items = _ttoi(pSubIndex->GetTokenBuf());
				break;

			case 2:	// Synonym filename
				{
					String_256 KPS;	// %s\\XaraInfo\\%s
					KPS = SubLibPath->GetPath(TRUE);
					KPS += String_16(_R(IDS_LIBRARIES_XARAINFO_DIRNAME));
					KPS += TEXT("\\") + String_64(pSubIndex->GetTokenBuf());
					SynonymFile = new PathName(KPS);
					
					// We can carry on from these two situations...
					if(SynonymFile == NULL)
					{
						ERROR3("Library::ReadHeaderInfo not enough memory to create a SynonymFile path");
						break;
					}

					if(!SynonymFile->IsValid())
					{
						ERROR3("Library::ReadHeaderInfo SynonymFile is invalid");
						break;
					}
				}
				break;

			case 3:	// Thumbnail sizes
				{
					for(INT32 i = 0; i < 3; i++)
					{
						PreviewX[i] = _ttoi((TCHAR *)pSubIndex->GetTokenBuf());
						
						ok = pSubIndex->GetToken();	// Skip ','
						if(!ok) break;
						TT = pSubIndex->GetTokenType();
						if(TT == TOKEN_EOL) break;

						ok = pSubIndex->GetToken();
						if(!ok) break;
						TT = pSubIndex->GetTokenType();
						if(TT == TOKEN_EOL) break;

						PreviewY[i] = _ttoi((TCHAR *)pSubIndex->GetTokenBuf());

						ok = pSubIndex->GetToken();	// Skip ','
						if(!ok) break;
						TT = pSubIndex->GetTokenType();
						if(TT == TOKEN_EOL) break;

						ok = pSubIndex->GetToken();
						if(!ok) break;
						TT = pSubIndex->GetTokenType();
						if(TT == TOKEN_EOL) break;

					}
				}
				break;
					
			case 4: // Field titles
				FieldString = pSubIndex->GetTokenBuf();

				// loop through rest of line adding the info to the buffer
				ok = pSubIndex->GetLineToken();
				if(!ok) break;

				TT = pSubIndex->GetTokenType();
				if(TT == TOKEN_LINE)
					FieldString += pSubIndex->GetTokenBuf();
				else
				{
					retok = FALSE;
					ERROR3("Library ReadHeader: Field line appears to be shorter than expected");
				}

				break;



			default:

				// Optional header extras - Must have their name followed by a ':' then the item
				if(camStrchr((TCHAR *) pSubIndex->GetTokenBuf(), _T(':')))
				{
					// We assume these lines aren't going to be bigger than 256 chars
					String_256 TokenBufU(pSubIndex->GetTokenBuf());
					TokenBufU.toUpper();
	
					// Description for group
					INT32 Offset = TokenBufU.Sub(String_16(_R(IDS_LIBRARIES_INDEX_DESCRITION)));
					if(Offset > -1 && Offset < 2)
					{
						// Need to set 'title'
						if(Title)
						{
							String_256 TokenBuf(pSubIndex->GetTokenBuf());
							TokenBuf.Right(Title, TokenBuf.Length() - (12 + Offset));				
							LibraryFile::KillLeadingSpaces(Title);
						}
					}

					// Remote files location
					Offset = TokenBufU.Sub(String_16(_R(IDS_LIBRARIES_INDEX_FILES)));
					if(Offset == 0)
					{
						TRACEUSER( "Richard", _T("Remote files in use\n"));

						// Need to set 'SubLibPath'
						if(!SubLibPath)
							SubLibPath = new PathName;
							
						if(SubLibPath != NULL)
						{
							PathName OldPath(*SubLibPath);
							String_256 ThePath;
							String_256 TokenBuf(pSubIndex->GetTokenBuf());
							TokenBuf.Right(&ThePath, TokenBuf.Length() - (6 + Offset));
							LibraryFile::KillLeadingSpaces(&ThePath);

							SubLibPath->SetPathName(ThePath);
							if(!SubLibPath->IsValid())
							{
								*SubLibPath = OldPath;
							}
						}
					}
				}

				// No further header entries, anything following the start line is a proper
				// line of library text...
				if(!camStrcmp((TCHAR *) pSubIndex->GetTokenBuf(), (TCHAR *)(String_16(_R(IDS_LIBRARIES_INDEX_START)))))
				{
					// Read until end of line
					do {
						ok = pSubIndex->GetToken();
						if(!ok) break;
						TT = pSubIndex->GetTokenType();		
					} while (TT != TOKEN_EOF && TT != TOKEN_EOL && ok);

					// We have reached the end of the header - exit immediately leaving
					// the file pointer at the start of the file "body"
					HitStart = TRUE;
				}
				break;
		}

		if(!ok) break;

		// Search for EOF or EOL after reading a line
		TT = pSubIndex->GetTokenType();		

		while (TT != TOKEN_EOF && TT != TOKEN_EOL && TT != TOKEN_LINE && TT != TOKEN_COMMENT && ok)
		{
			ok = pSubIndex->GetToken();
			if(!ok) break;
			TT = pSubIndex->GetTokenType();		
			ok = (TT != TOKEN_EOF);
			if(!ok) break;
		}
	}

	// Current file position is deemed the start of the first line of data
	FirstLineOffset = (LibraryIndex) pSubIndex->tellIn();

	// Hit EOF somehow... This is bad
	if(TT == TOKEN_EOF)
		return(0);

	if (retok)
		return(Items);

	return(0);
}



/***********************************************************************************************

>	BOOL Library::ScanRestOfFile(TCHAR *CachedIndex, INT32 Start, INT32 Finish, BOOL AddItems = TRUE)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/4/95

	Inputs:		CachedIndex	- points to the index file in memory
				Start		- Offset to start of first index line
				Finish		- Offset to end of file in memory
				AddItems	- Set to FALSE if you don't want the items to be added to the gallery

	Returns:	TRUE if it succeeded, FALSE if not. 
				If no items were found, it returns FALSE

	Purpose:	Scan the rest of the file (following the START), creating items
				for each entry...
				Also change ','s to 0's, so we can retrieve field strings more quickly later
				on.

				AddItems added so that recaching already created tree indexes wouldn't add new
				items every time the gallery opens !

***********************************************************************************************/

BOOL Library::ScanRestOfFile(TCHAR *CachedIndex, INT32 Start, INT32 Finish, BOOL AddItems)
{
	if(CachedIndex == NULL)
	{
		ERROR3("Library::ScanRestOfFile - no cached index");
		return FALSE;
	}

	if(Start >= Finish)
	{
		ERROR3("Library::ScanRestOfFile - nothing to scan");
		return FALSE;
	}

	INT32 Offset = Start;
	INT32 NewItemOffset = 0;
	
	INT32 LineCount = 0;
	ItemCount = 0;

	TCHAR Ch = ' ';
	TCHAR NCh = ' ';

	// chars to look for
	const TCHAR CR = '\n';
	const TCHAR LF = '\r';
	const TCHAR REM = '#';
	const TCHAR SEP = ',';
	const TCHAR SPC = ' ';

#ifdef _DEBUG
	// Count commas in FieldString
	INT32 CommaCount = 0;
	INT32 FieldCommaCount = 0;
	TCHAR *Buf = (TCHAR *)FieldString;
	INT32 Len = FieldString.Length();
	for(INT32 i = 0; i < Len; i++)
		if(Buf[i] == SEP)
		 	FieldCommaCount ++;
#endif

	BOOL ok = TRUE;

	do {
		// Offset should now point to the start of a line

#ifdef _DEBUG
		CommaCount=0;
#endif

		Ch = CachedIndex[Offset];
		if((Ch == REM) || (Ch == CR) || (Ch == LF) || (Ch == SPC) || (Ch == SEP))
			NewItemOffset = 0;
		else
			NewItemOffset = Offset;

		// Scan until eol
		do {
			Ch = CachedIndex[Offset];

#ifdef _DEBUG
			// Another separator to count
			if(Ch == SEP)
				CommaCount ++;
#endif
			
			// Zap separators into zeros
			if(Ch == SEP || Ch == LF || Ch == CR)
				CachedIndex[Offset] = 0;
			
			Offset ++;			

		} while ( (Offset < Finish) && (Ch != CR) && (Ch != LF));

		/* FIX */
#ifdef _DEBUG
		if(CommaCount != FieldCommaCount && CommaCount != 0)
		{
			String_256 WarnMsg;
			String_32 Line;
			
			// Bodge which might go wrong... Solves overflow problem...
			for(INT32 i=0; i<16; i++)
				((TCHAR *)Line)[i] = *(CachedIndex + NewItemOffset + (i * sizeof(TCHAR)));
			((TCHAR *)Line)[i] = 0;

			BOOL DoCommaWarning = TRUE;

#ifdef DONT_REPORT_KNOWN_CLIPART_PROBLEMS
			// Here are a couple of 'fixes' to stop the error3's in debug builds...
			if(NewItemOffset == 4205 && Start == 154 && Finish == 4271 && Offset == 4242 && CommaCount == 4 && FieldCommaCount == 5)
				DoCommaWarning = FALSE; // Apple.xar fix
			if(NewItemOffset == 4243 && Start == 154 && Finish == 4271 && Offset == 4270 && CommaCount == 4 && FieldCommaCount == 5)
				DoCommaWarning = FALSE; // Fish.xar fix
#endif
			// Another separator to count
			if(DoCommaWarning)
			{
				if(CommaCount > FieldCommaCount)	
					wsprintf(WarnMsg, "Too many commas in line '%s' of '%s'", (TCHAR *)Line, (const TCHAR *)SubLibPath->GetPath());
				else
					wsprintf(WarnMsg, "Not enough commas in line '%s' of '%s'", (TCHAR *)Line, (const TCHAR *)SubLibPath->GetPath());
				ERROR3(WarnMsg);
			}
		}
#endif
		/* FIX */

		// Since we need access to the fields whilst adding the item (for sorting) we
		// add the item after fixing up the index line
		if(NewItemOffset != 0)
		{
			// Create a supergallery Display Item representing this library item, and add it
			// to our parent group
			if(AddItems)
			{
				BOOL bIsNew = (m_lOldIndex != 0 && NewItemOffset >= m_lOldIndex) || (m_nModified == FOLDER_NEW);
				ok = (ParentGallery->AddLibraryItem(ParentGroup, this, NewItemOffset, bIsNew) != NULL);
			}

			if(ok)
			{
				ItemCount ++;
				LineCount ++;
			}
		}

		NCh = CachedIndex[Offset];

		// Skip possibly double eol chars
		if ( ((Ch==LF) || (Ch==CR)) && ((NCh==LF) || (NCh==CR)) && (Offset < Finish) )
			Offset ++;	

	} while(Offset < Finish);

	return (ItemCount > 0);
}


/***********************************************************************************************

>	BOOL Library::ScanRestOfFile(CCLexFile *pSubIndex)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/2/95

	Inputs:		pSubIndex - points to a CCLexFile to be scanned. The file pointer on
				entry is expected to be at the start of the index-file body.

	Returns:	TRUE if it succeeded, FALSE if not. 
				If no items were found, it returns FALSE

	Purpose:	Scan the rest of the file (following the START), creating items
				for each entry...

***********************************************************************************************/

BOOL Library::ScanRestOfFile(CCLexFile *pSubIndex)
{
	ERROR3IF(pSubIndex == NULL, "Library::ScanRestOfFile - NULL params are illegal");

	BOOL ok = TRUE;
	BOOL retok = TRUE;

	LexTokenType TT;
	UINT32 LineCount = 0;
	ItemCount = 0;
	LibraryIndex LineOffset = 0;

	// Current file position is deemed the start of the first line of data
	FirstLineOffset = (LibraryIndex) pSubIndex->tellIn();

	while(ok && retok)
	{
		// Remember line offset for the *start* of the line
		LineOffset = (LibraryIndex) pSubIndex->tellIn();

		if(!pSubIndex->GetToken()) break;

		// Keep reading tokens until we hit a normal one... (skips line ends and
		// comments for us - we could do this by skipping lines at a time, but there
		// are problems with comments...
		TT = pSubIndex->GetTokenType();		
		while (TT != TOKEN_NORMAL && ok)
		{
			// Remember line offset for the *start* of the line
			LineOffset = (LibraryIndex) pSubIndex->tellIn();

			ok = pSubIndex->GetToken();
			if(!ok) break;
			TT = pSubIndex->GetTokenType();		
			ok = (TT != TOKEN_EOF);
			if(!ok) break;
		}
		if(!ok) break;

		// Create a supergallery Display Item representing this library item, and add it
		// to our parent group

		ok = (ParentGallery->AddLibraryItem(ParentGroup, this, LineOffset) != NULL);

		if(ok) ItemCount ++;
		else break;

		// Search for EOF or EOL after reading a line
		TT = pSubIndex->GetTokenType();		

		while (TT != TOKEN_EOF && TT != TOKEN_EOL && TT != TOKEN_LINE && ok)
		{
			// Remember line offset for the *start* of the line
			LineOffset = (LibraryIndex) pSubIndex->tellIn();

			ok = pSubIndex->GetToken();
			if(!ok) break;
			TT = pSubIndex->GetTokenType();		
			ok = (TT != TOKEN_EOF);
			if(!ok) break;
		}
	}

	// Current file position is deemed the start of the first line of data
	LastLineOffset = (LibraryIndex) pSubIndex->tellIn();

	return (ItemCount > 0);
}



/***********************************************************************************************

>	void Library::GetLibraryTitle(StringBase *ReturnedTitle)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/2/95

	Outputs:	ReturnedTitle - will be filled in with the title (may NOT be NULL)

	Purpose:	To retrieve the title text for this library (this is the text
				that a linked SGDisplayGroup will display in the gallery list
				as in "Pictures of animals")

***********************************************************************************************/

void Library::GetLibraryTitle(StringBase *ReturnedTitle)
{
	ERROR3IF(ReturnedTitle == NULL, "Library::GetLIbraryTitle - NULL Params are illegal");

	if (Title == NULL)
		*ReturnedTitle = String_16(_R(IDS_LIBRARIES_UNAMED));
	else
		*ReturnedTitle = *Title;
}



/***********************************************************************************************

>	BOOL Library::GetFilename(LibraryIndex Offset, StringBase *File, BOOL FullPath = TRUE)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/2/95

	Inputs:		Offset 		- the index identifying the item you require
				FullPath 	- If TRUE, return the full path of the file, otherwise just the filename

	Outputs:	File 		- filled in path/file name

	Returns:	TRUE if it succeeded

	Purpose:	Return the path/filename for the indexed library item

	Notes:		File is now a stringbase for speed reasons. Constructing a full PathName was
				found to be quite slow, and hindered sorting quite a bit. The FullPath param
				is also included for this purpose.

***********************************************************************************************/

BOOL Library::GetFilename(LibraryIndex Offset, StringBase *File, BOOL FullPath)
{
	ERROR3IF(File == NULL, "Library::GetFilename - NULL params are illegal");

	if(!FullPath)
	{
		// Get filename from index file
		return (GetSingleField(Offset, 1, File));
	}

	String_256 Filename;

	// Get filename from index file
	if (!GetSingleField(Offset, 1, &Filename))
		return FALSE;

	// Path for normal files
	*File = SubLibPath->GetPath(TRUE);
	*File += Filename;
	
	return TRUE;
}


/***********************************************************************************************

>	BOOL Library::GetFilename(LibraryIndex Offset, TCHAR **File)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/5/95

	Inputs:		Offset - the index identifying the item you require
				File - A pointer to a path name to be filled in (May NOT be NULL)

	Returns:	TRUE if it succeeded

	Purpose:	Return a pointer to the filename for the indexed library item

	Notes:		If we're using diskfiles this will return FALSE


***********************************************************************************************/

BOOL Library::GetFilename(LibraryIndex Offset, TCHAR **File)
{
	ERROR3IF(File == NULL, "Library::GetFilename - NULL params are illegal");

	// Get filename from index file
	return (GetSingleField(Offset, 1, File));
}


/***********************************************************************************************

>	BOOL Library::GetTextname(LibraryIndex Offset, StringBase *Text)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/2/95

	Inputs:		Offset - the index identifying the item you require
				Text - A pointer to a string to recieve the text name (May NOT be NULL)

	Purpose:	Return the textual description for the indexed library item

***********************************************************************************************/

BOOL Library::GetTextname(LibraryIndex Offset, StringBase *Text)
{
	ERROR3IF(Text == NULL, "Library::GetTextname - NULL params are illegal");
	return(GetSingleField(Offset, 2, Text));
}

/***********************************************************************************************

>	BOOL Library::GetTextname(LibraryIndex Offset, TCHAR **Text)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/2/95

	Inputs:		Offset - the index identifying the item you require
	Outputs:	Text - A pointer to a string (May NOT be NULL)

	Purpose:	Return a pointer to the textual description for the indexed library item

***********************************************************************************************/

BOOL Library::GetTextname(LibraryIndex Offset, TCHAR **Text)
{
	ERROR3IF(Text == NULL, "Library::GetTextname - NULL params are illegal");
	return(GetSingleField(Offset, 2, Text));
}



/***********************************************************************************************

>	UINT32 Library::GetID(LibraryIndex Offset)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/2/95

	Inputs:		Offset - The library index identifying the item you want

	Purpose:	Return the ID for the indexed library item
			 	Returns 0 if there was a problem (0 is an invalid ID)

***********************************************************************************************/

UINT32 Library::GetID(LibraryIndex Offset)
{
	String_8 Tmp;
	if (!GetSingleField(Offset, 3, &Tmp))
		return(0);

	return((UINT32) _ttoi((TCHAR *)Tmp));
}


/***********************************************************************************************

>	BOOL Library::GetTitle(LibraryIndex Offset, StringBase *Title)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/5/95

	Inputs:		Offset - the index identifying the item you require
	Outputs:	Title - A pointer to a path name to be filled in (May NOT be NULL)

	Returns:	TRUE if it succeeded

	Purpose:	Return the 'title' for the indexed library item (if there is one)

***********************************************************************************************/

BOOL Library::GetTitle(LibraryIndex Offset, StringBase *Title)
{
	ERROR3IF(Title == NULL, "Library::GetTitle - NULL params are illegal");

	String_64 Tmp(_R(IDS_LIBRARIES_INDEX_ITEM_TITLE));
	if(!GotField(&Tmp))
		return FALSE;

	// Get title from index file
	BOOL ok = GetSingleField(Offset, &Tmp, Title);

//	BOOL ok = GetSingleField(Offset, 4, Title);

	// NULL titles are invalid - default to something else - probably filename
	if(Title->Length() == 0)
		ok = FALSE;

	return ok;
}


/***********************************************************************************************

>	BOOL Library::GetTitle(LibraryIndex Offset, TCHAR **Title)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/5/95

	Inputs:		Offset	- the index identifying the item you require
	Outputs:	Title	- ptr to title to return

	Returns:	TRUE if it succeeded

	Purpose:	Return a pointer to the title for the indexed library item

	Notes:		If we're using diskfiles this will return FALSE

***********************************************************************************************/

BOOL Library::GetTitle(LibraryIndex Offset, TCHAR **Title)
{
	ERROR3IF(Title == NULL, "Library::GetFilename - NULL params are illegal");

	String_64 Tmp(_R(IDS_LIBRARIES_INDEX_ITEM_TITLE));
	if(!GotField(&Tmp))
		return FALSE;

	// Get title from index file
	BOOL ok = GetSingleField(Offset, &Tmp, Title);
//	BOOL ok = GetSingleField(Offset, 4, Title);

	// NULL titles are invalid - default to something else - probably filename
	if(camStrlen(*Title)  == 0)
		ok = FALSE;

	return ok;
}

/***********************************************************************************************

>	BOOL Library::GetThumbnail(LibraryIndex Offset, SGThumbSize Size, BOOL Urgent,
								KernelBitmap **Thumbnail )

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>; modified by Adrian 1/12/96 
	Created:	22/2/95
	Inputs:		Offset into index file
				Size of thumbnail (Small, Medium, Large)
				Urgent - set to true if we want the thumbnail NOW ! (if false, only returns thumbnail if in cache)

	Outputs:	Thumbnail - The value pointed to by Result will be filled in with
				a pointer to a created thumbnail. This may be NULL if a thumb could
				not be found/created (if Urgent == FALSE, then this will be NULL if the
				thumb is not already cached in memory)

	Returns:	TRUE if it succeeds (though this may still return a NULL thumbnail ptr)
				FALSE if it failed miserably

	Purpose:	Return a pointer to the thumbnail for the given library index.

***********************************************************************************************/

BOOL Library::GetThumbnail(LibraryIndex Offset, SGThumbSize Size, BOOL Urgent,
							KernelBitmap **Thumbnail)
{
	if (Thumbnails == NULL)
		return FALSE;

	UINT32 ID = GetID(Offset);
	Thumbnails->SetSize(Size);

	// For ripping thumbnails out of art files, we need the name of the art file
	PathName ActualFile;
	PathName *pActualFile = &ActualFile;
	String_256 TheFile;

	if(!GetFilename(Offset, &TheFile, TRUE))
		pActualFile = NULL;
	else
		ActualFile.SetPathName(TheFile);
																												 
	return(Thumbnails->GetThumbnail(ID, Urgent, Thumbnail, pActualFile));
}



/***********************************************************************************************

>	void Library::KillAllThumbnails(void)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/2/95

	Purpose:	Deletes all thumbnails from cache

***********************************************************************************************/

void Library::KillAllThumbnails(void)
{
	if (Thumbnails != NULL)
		Thumbnails->DeleteThumbnails();
}								  




/***********************************************************************************************

>	BOOL Library::LocalPath2URL(String_256* pLocalPath)

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com>
	Created:	01/12/96

	Inputs:	   a pointer to a String_256 containing local path to convert to a URL

	Outputs: the expected location of the file on our web site. The input string is overwritten
				 with this location, but only if the function succeeds. The input string will not be
				 modified in case of failure.
				

	Returns: TRUE is successful, FALSE if failed

	Purpose:	converts a local path to a URL, using the base URL read from the library index file
					The URL can be used to download the file 
***********************************************************************************************/

BOOL Library::LocalPath2URL(String_256* pLocalPath)
{
	// The local path is made up of:
	// 1) a root path which is usually the exe's directory
	// 2) a leaf relative path which is assumed to be identical on the local drive and our web site
	// The URL is obtained by appending this leaf path to the root URL, and
	// converting all backslashes to normal ones. The returned URL is always lower case.

	String_256 strLocalPath(*pLocalPath);
	String_256 strURL(_T("http://xara.xaraonline.com/XaraX2/Resources/"));
	if (strURL.Length() == 0) 
	{
		ERROR3("URL base string is empty");
		return FALSE; // we don't have a base url, so can't convert
	}
	String_256 strWebFilesDir(GetStringField(Type, _R(IDS_CACHEDIR)));
	strWebFilesDir += _T("\\");
	strLocalPath.toLower();
	strWebFilesDir.toLower();
	strURL += camStrstr((TCHAR*) strLocalPath, (TCHAR*) strWebFilesDir + String_256(_R(IDS_CACHEPATH)).Length());
	strURL.toLower();
	// Convert backslashes to normal (UNIX) ones
	TCHAR* pBackSlash = NULL;
	while (pBackSlash = camStrchr((TCHAR*) strURL, _T('\\')))
			*pBackSlash = _T('/');
	// Check if the URL is valid
	WCHAR  wchURL[INTERNET_MAX_PATH_LENGTH];
	MultiByteToWideChar(CP_ACP, 0,  (TCHAR*) strURL, -1, wchURL, INTERNET_MAX_PATH_LENGTH);
	BOOL bIsValid = (IsValidURL(NULL, wchURL, 0) == S_OK) ? TRUE : FALSE;
	if (bIsValid)
	{
		*pLocalPath = strURL;
		return TRUE;
	}
	else
	{
		ERROR3("Conversion resulted in an invalid URL");
		return FALSE;
	}
}



/***********************************************************************************************

>	BOOL Library::GetSingleField(LibraryIndex Offset, UINT32 Count, StringBase *Field)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/2/95

	Inputs:		Offset - The index identifying the library item to get the field for
				Count - Identifies which field toi retrieve from the line of index
				information for this item.
	
	Outputs:	Field will be filled in with the field contents.

	Returns:	FALSE if it fails; Field will contain a blank string ("")

	Purpose:	Return a single field for a given library item
				Use this for extended info

***********************************************************************************************/

BOOL Library::GetSingleField(LibraryIndex Offset, UINT32 Count, StringBase *Field)
{
	ERROR3IF(Field == NULL, "Library::GetSingleField - NULL Params are illegal");

#ifdef FIELDCACHE
	// We've got a hit in our field cache
	if(CacheGet(Offset, Count, Field))
		return TRUE;
#endif

	// Extract the entry from the index file
	BOOL GotField = FALSE;

	// Check if we've got a cached version of the index
	if(CachedSubIndexBuf != NULL)
	{
		GotField = GetSingleFieldFromMemory(Offset, Count, Field);
	}
	
	// Memory file didn't have item, or not present... use a disk file
	if(!GotField)
	{
		GotField = GetSingleFieldFromDisk(Offset, Count, Field);
	}

	// If we got the field, whack it in the cache, otherwise return a blank
	if(!GotField)
		*Field = TEXT("");
#ifdef FIELDCACHE
	else
	{
		// Clean the string up a bit
		LibraryFile::KillLeadingSpaces(Field);
		CachePut(Offset, Count, Field);
	}
#endif
	
	return GotField;
}


/***********************************************************************************************

>	BOOL Library::GetSingleField(LibraryIndex Offset, UINT32 Count, TCHAR **Field)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/2/95

	Inputs:		Offset - The index identifying the library item to get the field for
				Count - Identifies which field toi retrieve from the line of index
				information for this item.
	
	Outputs:	Field will point to the field contents.

	Returns:	FALSE if it fails;

	Purpose:	Return a single field for a given library item
				Use this for extended info

	Notes: 		If using disk files we currently return false, this means you should always
				use the string copying one above if this returns false, etc...

***********************************************************************************************/

BOOL Library::GetSingleField(LibraryIndex Offset, UINT32 Count, TCHAR **Field)
{
	ERROR3IF(Field == NULL, "Library::GetSingleField - NULL Params are illegal");

	// Extract the entry from the index file
	BOOL GotField = FALSE;

	// Check if we've got a cached version of the index
	if(CachedSubIndexBuf != NULL)
	{
		// This will call the TCHAR ** version, not the stringbase version
		GotField = GetSingleFieldFromMemory(Offset, Count, Field);
	}
	
	return GotField;
}




/***********************************************************************************************

>	BOOL Library::GetSingleFieldFromMemory(LibraryIndex Offset, UINT32 Count, StringBase *Field)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/4/95

	Inputs:		Offset - The index identifying the library item to get the field for
				Count - Identifies which field toi retrieve from the line of index
				information for this item.
	
	Outputs:	Field will be filled in with the field contents.

	Returns:	FALSE if it fails

	Purpose:	Return a single field for a given library item using the 0 padded, cached
				index file in memory.

***********************************************************************************************/

BOOL Library::GetSingleFieldFromMemory(LibraryIndex Offset, UINT32 Count, StringBase *Field)
{
	ERROR3IF(Field == NULL, "Library::GetSingleField - NULL Params are illegal");
	ERROR3IF(Offset > CachedBufLength, "Library::GetSingleField - given out of range offset");
	ERROR3IF(CachedSubIndexBuf == NULL, "Library::GetSingleField - called with null index buffer");

#if 0
	// Equivalent to a memfile seek
	TCHAR *Start = CachedSubIndexBuf + Offset;
	TCHAR *Finish = CachedSubIndexBuf + CachedBufLength;

	if(Count == 1)
	{
		*Field = Start;
		return TRUE;
	}
	else
	{
		// chars to look for
		const TCHAR SPC = ' ';

		TCHAR Ch = *Start;
		UINT32 FieldCount = 1;

		// scan through fields until we hit our baby
		while(Count > FieldCount && Start < Finish)
		{
			if(Ch == 0)
				FieldCount ++;
			Start += sizeof(TCHAR);
			Ch = *Start;
		}

		if(Count == FieldCount)
		{
			// Quick, speedy kill leading spaces type thing
			Ch = *Start;
			while(Ch == SPC && Ch != 0 && Start < Finish)
			{
				Start += sizeof(TCHAR);
				Ch = *Start;
			}

			// COPY String into field (could pass back a pointer ?)		
			*Field = Start;
			return TRUE;
		}
		else
			return FALSE;
	}	

	return FALSE;
#endif

	TCHAR *Fld;
	BOOL ok = GetSingleFieldFromMemory(Offset, Count, &Fld);
	*Field = Fld;

	return ok;
}

/***********************************************************************************************

>	BOOL Library::GetSingleFieldFromMemory(LibraryIndex Offset, UINT32 Count, TCHAR **Field)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/5/95

	Inputs:		Offset - The index identifying the library item to get the field for
				Count - Identifies which field toi retrieve from the line of index
				information for this item.
	
	Outputs:	Field will point to the field string in memory

	Returns:	FALSE if it fails

	Purpose:	Return a pointer to a single field for a given library item using the 0
				padded, cached index file in memory.

***********************************************************************************************/

BOOL Library::GetSingleFieldFromMemory(LibraryIndex Offset, UINT32 Count, TCHAR **Field)
{
	ERROR3IF(Field == NULL, "Library::GetSingleField - NULL Params are illegal");
	ERROR3IF(Offset > CachedBufLength, "Library::GetSingleField - given out of range offset");
	ERROR3IF(CachedSubIndexBuf == NULL, "Library::GetSingleField - called with null index buffer");

	// Equivalent to a memfile seek
	TCHAR *Start = CachedSubIndexBuf + Offset;
	TCHAR *Finish = CachedSubIndexBuf + CachedBufLength;

	if(Count == 1)
	{
		*Field = Start;
		return TRUE;
	}
	else
	{
		// chars to look for
		const TCHAR SPC = ' ';

		TCHAR Ch = *Start;
		UINT32 FieldCount = 1;

		// scan through fields until we hit our baby
		while(Count > FieldCount && Start < Finish)
		{
			if(Ch == 0) FieldCount ++;
			Start += sizeof(TCHAR);
			Ch = *Start;
		}

		if(Count == FieldCount)
		{
			// Quick, speedy kill leading spaces type thing
			Ch = *Start;
			while(Ch == SPC && Ch != 0 && Start < Finish)
			{
				Start += sizeof(TCHAR);
				Ch = *Start;
			}

			// Pass back a pointer to the string data
			*Field = Start;
			return TRUE;
		}
		else
			return FALSE;
	}	

	return FALSE;
}
 
/***********************************************************************************************

>	BOOL Library::GetSingleFieldFromDisk(LibraryIndex Offset, UINT32 Count, StringBase *Field)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/4/95

	Inputs:		Offset - The index identifying the library item to get the field for
				Count - Identifies which field toi retrieve from the line of index
				information for this item.
	
	Outputs:	Field will be filled in with the field contents.

	Returns:	FALSE if it fails

	Purpose:	Return a single field for a given library item using a diskfile for the index

***********************************************************************************************/

BOOL Library::GetSingleFieldFromDisk(LibraryIndex Offset, UINT32 Count, StringBase *Field)
{
	ERROR3IF(Field == NULL, "Library::GetSingleField - NULL Params are illegal");

	BOOL ok = TRUE;
	BOOL OpenedOK = TRUE;
	CCLexFile *SubIndex = NULL;

	// If we've not got a cached version of the index, use a diskfile on the index itself
	SubIndex = new CCDiskFile;

	// Open the index file, seek to our position and rip out our token
	if(SubIndex != NULL)
		OpenedOK = ((CCDiskFile *)SubIndex)->open(*IndexFile, ios::in);

	if(SubIndex == NULL)
	{
		ERROR3("Library::Init Disk SubIndex not created properly");
		return FALSE;
	}

	if(!OpenedOK)
	{
		ERROR3("Library::Init Disk SubIndex not opened properly");
		delete SubIndex;
		return FALSE;
	}
	
	// Subindex seeking required
	if (!SubIndex->InitLexer(TRUE))
	{
		ERROR3("Library::GetSingleField InitLexer failed");
		if(SubIndex->isOpen())
			SubIndex->close();
		delete SubIndex;
		return FALSE;
	}

	SubIndex->SetWhitespace("");			// Setting this to blank lets us read non-"'d strings
	SubIndex->SetDelimiters(",");		// ,s delimit our fields
	SubIndex->SetCommentMarker('#');		// #'d lines are commented out
	SubIndex->SetStringDelimiters("");	// No string delimiters

	SubIndex->seekIn(Offset);
	
	String_256 Token;
	LexTokenType TT = TOKEN_EOL;

	LibraryIndex FOffset;
	ok = SubIndex->GetToken();
	TT = SubIndex->GetTokenType();		

	FOffset = (LibraryIndex) SubIndex->tellIn();

	if(FOffset != Offset)
		ERROR3("Lexer didn't seek properly");

	while(Offset > FOffset && ok && TT != EOF)
	{
		ok = SubIndex->GetToken();
		FOffset = (LibraryIndex) SubIndex->tellIn();
		TT = SubIndex->GetTokenType();		
		if (TT == TOKEN_EOF) ok = FALSE;
	}

	if(!ok)
	{
		ERROR3("Library::GetSingleField Line past end of file");
//		if(CachedSubIndex == NULL)
//		{
			if(SubIndex->isOpen())
				SubIndex->close();
			delete SubIndex;
//		}
		return FALSE;
	}

	// Get the first token in the line (since we did a seek, the first few
	// tries might return garbage...)
	while(ok && TT != TOKEN_NORMAL)
	{
		ok = SubIndex->GetToken();

		if (ok)
		{
			TT = SubIndex->GetTokenType();		
			if (TT == TOKEN_EOF)
				ok = FALSE;
		}
	}

	if (ok)
	{
		UINT32 i = 1;

		// Keep reading tokens until we hit the one we want
		while(Count > i && ok)
		{
			Token = SubIndex->GetTokenBuf();
			LibraryFile::KillLeadingSpaces(&Token);

			if (Token == String_8(",") )
				i++;						// Found a comma, move to next field
		
			ok = SubIndex->GetToken();		// Get next token
		}

		// Hopefully the token buffer now contains our item
		if (ok)						   
		{
			Token = SubIndex->GetTokenBuf();
			LibraryFile::KillLeadingSpaces(&Token);

			if (Token == String_8(",") )
				*Field = TEXT("");
			else
				*Field = Token;
		}
	}

	// reclaim lexer-buffer memory
	SubIndex->DeinitLexer();	

	// and close the file...
	if(SubIndex->isOpen())
		SubIndex->close();

	// and reclaim the diskfile memory...
	delete SubIndex;

	return ok;
}

/***********************************************************************************************

>	BOOL GetSingleField(LibraryIndex Offset, String_64 *FieldName, StringBase *Field);

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/3/95

	Inputs:		Offset		- The index identifying the library item to get the field for
				FieldName	- Name of field we require (must be in field header)
	
	Outputs:	Field 		- will be filled in with the field contents.

	Returns:	FALSE if it fails;

	Purpose:	Return a single field for a given library item and field description
				Use this for extended info

	Notes:		The above code assumes that the first three fields are always:
					Filename, Description, ID

				We've now got 'Title' as a fourth, but only if it's in the field description...
		
				After that we've got things like 'Key' and 'Size' which we check
				the positions of using this string...

***********************************************************************************************/

BOOL Library::GetSingleField(LibraryIndex Offset, String_64 *FieldName, StringBase *Field)
{
	ERROR3IF(FieldName == NULL || Field == NULL, "Library::GetSingleField given nulls");

	if(!GotField(FieldName))
	{
		*Field = TEXT("");
		return FALSE;
	}

	INT32 Count = FindField(FieldName);

	return (Count > 0 && GetSingleField(Offset, Count, Field));
}

/***********************************************************************************************

>	BOOL GetSingleField(LibraryIndex Offset, String_64 *FieldName, StringBase *Field);

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/3/95

	Inputs:		Offset		- The index identifying the library item to get the field for
				FieldName	- Name of field we require (must be in field header)
	
	Outputs:	Field 		- will be pointed to the field contents (if index cached).

	Returns:	FALSE if it fails;

	Purpose:	Get a pointer to a single field for a given library item and field description
				using the extended info

	Notes:		The above code assumes that the first three fields are always:
					Filename, Description, ID
		
				After that we've got things like 'Key', 'Size', 'Title' which we check
				the positions of using this string...

				If the index isn't in memory we can't return the pointer !

***********************************************************************************************/

BOOL Library::GetSingleField(LibraryIndex Offset, String_64 *FieldName, TCHAR **Field)
{
	ERROR3IF(FieldName == NULL || Field == NULL, "Library::GetSingleField given nulls");

	if(!GotField(FieldName))
		return FALSE;

	INT32 Count = FindField(FieldName);

	return (Count > 0 && GetSingleField(Offset, Count, Field));
}

/***********************************************************************************************

>	INT32 Library::FindField(String_64 *FieldName);

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/5/95

	Inputs:		FieldName	- Field to look for
	
	Returns:	Position of field - so for 'ID' we would return 3, etc...

	Purpose:	Works out the column for the given field string

***********************************************************************************************/

INT32 Library::FindField(String_64 *FieldName)
{
	ERROR3IF(FieldName == NULL, "Library::FindField given nulls");

	INT32 Pos = FieldString.Sub(*FieldName);
	
	if(Pos != -1)
	{
		// Find out which field it was
		String_256 Left;
		FieldString.Left(&Left, Pos);
		const TCHAR BrkChar = ',';

		// This bit could go in basestr...		
		INT32 Count = 0;
		TCHAR *Buf = (TCHAR *)Left;
		INT32 Len = Left.Length();
		for(INT32 i = 0; i < Len; i++)
			if(Buf[i] == BrkChar)
			 	Count ++;

		// Number of commas + 1 is our field
		Count ++;

		return Count;
	}

	return 0;
}


/***********************************************************************************************

>	BOOL Library::GotField(String_64 *FieldName);

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/5/95

	Inputs:		FieldName 	- Field to look for
	
	Returns:	TRUE if the field string has the specified field...

	Purpose:	Check if a string is in the field description string...

***********************************************************************************************/

BOOL Library::GotField(String_64 *FieldName)
{
	return (FieldString.Sub(*FieldName) != -1);
}
	  
/***********************************************************************************************

>	BOOL Library::CacheInit(INT32 Entries)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/4/95

	Inputs:		Entries - The maximum number of entries in the field cache	
	Outputs:	
	Returns:	FALSE if it fails;

	Purpose:	Initialises memory for a sub-index file field cache.
				Currently we use a single entry wide hash table with the specified number
				of buckets. Reading items from the cache is much quicker than from the
				index file itself, be it in memory or not ! (Well, that's the theory)

***********************************************************************************************/

BOOL Library::CacheInit(INT32 Entries)
{
	if(Entries == 0)
		FieldCache = NULL;
	else
		if(FieldCache == NULL)
			FieldCache = (SGFieldCache *)CCMalloc(sizeof(SGFieldCache) * Entries);
		
	if(FieldCache == NULL)
	{
		FieldCacheEntries = 0;
		ERROR3("Library::CacheInit - Not enough memory for a field cache - reduce the Fields in the ini file");
		return FALSE;
	}
	else
	{
		FieldCacheEntries = Entries;

		SGFieldCache Dummy;
		Dummy.Offset = 0;
		Dummy.Count = 0;
		Dummy.Field = NULL;

		for(INT32 i=0; i<Entries; i++)
		{
			FieldCache[i] = Dummy;
		}
	}
	return TRUE;
}


/***********************************************************************************************

>	BOOL Library::CacheKill(void)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/4/95

	Inputs:
	Outputs:	
	Returns:	FALSE if it fails;

	Purpose:	Reclaims all the memory used by the field cache.

***********************************************************************************************/

BOOL Library::CacheKill(void)
{
	TRACEUSER( "Richard", _T("Cache Kill\n"));

	if(FieldCache != NULL)
	{
		for(INT32 i=0; i<FieldCacheEntries; i++)
		{
			if(FieldCache[i].Field != NULL)
			{
				CCFree(FieldCache[i].Field);
				FieldCache[i].Field = NULL;
			}
		}
		FieldCacheEntries = 0;

		CCFree(FieldCache);
		FieldCache = NULL;
	}

	return TRUE;
}

/***********************************************************************************************

>	INT32 Library::CacheHashFunction(LibraryIndex Offset, UINT32 Count)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/4/95

	Inputs:		Offset - this is the file offset of the start of the relevant line
				Count - Field count
	Outputs:	
	Returns:	Index into the hash table

	Purpose:	Returns an index into the hash table for the given parameters.
				The simpler, the quicker, but probably the less efficient.

	Notes:		I've had a play around with this function, and due to the random nature
				of the index files, can't really improve on the below all that much. Since
				the below is so quick, this is where it's been left.

***********************************************************************************************/

INT32 Library::CacheHashFunction(LibraryIndex Offset, UINT32 Count)
{
	return ((Count + Offset) % FieldCacheEntries);
}

/***********************************************************************************************

>	BOOL Library::CachePut(LibraryIndex Offset, UINT32 Count, StringBase *Field)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/4/95

	Inputs:		Offset - this is the file offset of the start of the relevant line
				Count - Field count
				Field - The field string to put in the cache
	Outputs:	
	Returns:	TRUE if we allocated the memory, etc...

	Purpose:	Adds an item into the cache for returning later on with CacheGet

***********************************************************************************************/

BOOL Library::CachePut(LibraryIndex Offset, UINT32 Count, StringBase *Field)
{
	TRACEUSER( "Richard", _T("Cache Put\n"));

	// Put the field in the field buffer / line cache, etc...
	if(FieldCache != NULL)
	{		
		SGFieldCache OurEntry;
		OurEntry.Offset = Offset;
		OurEntry.Count = Count;
		OurEntry.Field = (TCHAR *)CCMalloc((Field->Length()+1) * sizeof(TCHAR));
		if(OurEntry.Field == NULL)
		{
			ERROR3("Library::GetSingleField can't allocate enough string space in field buffer");
			return FALSE;
		}
		else
		{
		 	camStrcpy(OurEntry.Field, (TCHAR *)*Field);
		}
	
		SGFieldCache *plc = &FieldCache[CacheHashFunction(Offset, Count)];
		
		if(plc->Field != NULL)
		{
			CCFree(plc->Field);
			plc->Field = NULL;
			//TRACEUSER( "Richard", _T("Hash Clash at %d\n"), CacheHashFunction(Offset, Count));
		}
	
		*plc = OurEntry;

		return TRUE;
	}
	return FALSE;
}

/***********************************************************************************************

>	BOOL Library::CacheGet(LibraryIndex Offset, UINT32 Count, StringBase *Field)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/4/95

	Inputs:		Offset - this is the file offset of the start of the relevant line
				Count - Field count
	Outputs:	Field - The resulting field
	Returns:	TRUE if the item was there

	Purpose:	Checks if an item is in the cache. If it is, return it.

***********************************************************************************************/

BOOL Library::CacheGet(LibraryIndex Offset, UINT32 Count, StringBase *Field)
{
#if 0
	// Check the FieldCache occupancy
	if(FieldCache != NULL)
	{		
		INT32 Occupancy = 0;
		for(INT32 i=0; i<FieldCacheEntries; i++)
		{
			if(FieldCache[i].Field != NULL) Occupancy++;
		}
		TRACEUSER( "Richard", _T("Hash usage %d out of %d - %d%%\n"), Occupancy, FieldCacheEntries, (100 * Occupancy / FieldCacheEntries));
	}
#endif
		
	// See if our entry is in the cache
	SGFieldCache OurEntry;
	if(FieldCache != NULL)
	{		
		OurEntry = FieldCache[CacheHashFunction(Offset, Count)];
	
		if(OurEntry.Offset == Offset && OurEntry.Count == Count && OurEntry.Field != NULL)
		{
			String_256 tmpstr(OurEntry.Field);
			*Field = tmpstr;
			return TRUE;
		}
	}

	//TRACEUSER( "Richard", _T("Field not in cache..."));
	
	return FALSE;
}


/***********************************************************************************************

>	BOOL Library::SaveIndexInDisplayedOrder(PathName *IndexPathName, BOOL NewIDs);

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/5/95

	Inputs:		IndexPathName - path to output index file to
				NewIDs - regenerate the IDs in numerical order
	Outputs:
	Returns:	FALSE if it fails; TRUE if we did it all ok

	Purpose:	Save out an index file in the order the group is displayed (possibly sorted !)
	Notes:		This is strictly a debug only function !

***********************************************************************************************/

BOOL Library::SaveIndexInDisplayedOrder(PathName *IndexPathName, BOOL NewIDs)
{
#ifdef SAVEINDEX
	// Create index file
	CCDiskFile *IndexPath = new CCDiskFile(1024, TRUE, FALSE);
	ERROR2IF(IndexPath == NULL, FALSE, "Null index file allocated");

	if(!IndexPath->open(*IndexPathName, ios::out))
	{
		ERROR3("Can't create new index file");
		delete IndexPath;
		return FALSE;
	}

	// Write the header to the index file
	IndexPath->write(CachedSubIndexBuf, (UINT32)FirstLineOffset);

	// Loop through each item in the group as they're displayed and save each one out...
	BOOL ok = TRUE;

	// For the NewID stuff...
	INT32 Count = 0;

	SGDisplayNode *Item = ParentGroup;

	// Get the first item in the group...
	if(Item != NULL)
		Item = Item->GetChild();

	// Loop through all the group items
	while (Item != NULL)
	{
		if(Item->IsKindOf(CC_RUNTIME_CLASS(SGLibDisplayItem)))
		{
			Count++;

			SGLibDisplayItem *LibItem = (SGLibDisplayItem *) Item;

			LibraryIndex Offset = LibItem->GetDisplayedLibraryIndex();
			String_64 Field;

			TCHAR FieldStringBuffer[256];
			TCHAR *FieldStringDone = (TCHAR *)FieldStringBuffer;
			camStrcpy(FieldStringDone, (TCHAR *)FieldString);

			BOOL MoreFields = TRUE;

			// Rip out each field in turn
			while(MoreFields)
			{			
				// find next comma in FieldStringDone and replace it with a 0
				// If there are none then we assume that the end has come...
				TCHAR *Comma = camStrchr(FieldStringDone, _T(','));
				if(Comma != NULL)
					*Comma = 0;
				else
					MoreFields = FALSE;

				// kill leading spaces from FieldStringDone
				while(FieldStringDone[0] == ' ')
					FieldStringDone += sizeof(TCHAR);

				// FieldStringDone now points to a field string... Get the data...
				String_64 FieldStringToUse(FieldStringDone);

				if(NewIDs && (FieldStringToUse == String_64(_R(IDS_LIBRARIES_INDEX_ITEM_ID))))
					wsprintf(Field, "%d", Count);
				else
					ok = GetSingleField(Offset, &FieldStringToUse, &Field);

				ERROR3IF(!ok, "Problems getting a field");
			
				// Write the data into the new index file
				IndexPath->write((TCHAR *)Field, (UINT32)Field.Length() * sizeof(TCHAR));

				// Ternimate the data, if it's the last field, use a return...
				if(Comma != NULL)
				{
					IndexPath->write((TCHAR *)",", sizeof(TCHAR) * 1);
					FieldStringDone += ((camStrlen(FieldStringDone)+1) * sizeof(TCHAR));
				}
				else
					IndexPath->write((TCHAR *)"\r\n", sizeof(TCHAR) * 2);		
			}
		}
		// If we've reached the end, this will be NULL
		Item = Item->GetNext();
	}

	// Close the index file and tidy up
	IndexPath->close();
	delete IndexPath;

	ERROR3IF(Count != ItemCount, "Wrong number of items in group -> bad !");

	return TRUE;
#endif
	return FALSE;
}


/***********************************************************************************************

>	static BOOL Library::InitLibPrefs(void);

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/4/95

	Inputs:		
	Outputs:		
	Returns:	FALSE if it fails; TRUE if we got our prefs from the ini file

	Purpose:	Initialises the preferences in the ini file and returns the values
				of the entries if they're there already

	Notes:

***********************************************************************************************/

BOOL Library::InitLibPrefs(void)
{
	// Only do this once
	static BOOL AlreadyCalled = FALSE;
	if(AlreadyCalled) return FALSE;
	AlreadyCalled = TRUE;

#ifndef EXCLUDE_GALS
	
	GetApplication()->DeclareSection(TEXT("Libraries"), 12);

	// The locations of the libraries (as found via browse or scan)
	GetApplication()->DeclarePref(TEXT("Libraries"), TEXT("ClipArt"), &LibClipartSGallery::ClipartPath);
	GetApplication()->DeclarePref(TEXT("Libraries"), TEXT("WebTheme"), &LibClipartSGallery::WebThemePath);

#ifndef STANDALONE
//WEBSTER-Martin-09/01/97	- Put back by Ranbir.
//#ifndef WEBSTER
	GetApplication()->DeclarePref(TEXT("Libraries"), TEXT("Font"), &FontsSGallery::DefaultLibraryPath);
//#endif //WEBSTER
	GetApplication()->DeclarePref(TEXT("Libraries"), TEXT("Texture"), &LibFillsSGallery::DefaultLibraryPath);
#endif

	// Remember the last display modes used
	GetApplication()->DeclarePref(TEXT("Libraries"), TEXT("ClipartDisplayMode"), &LibClipartSGallery::DefaultDisplayMode, 0, 2);
#ifndef STANDALONE
//WEBSTER-Martin-09/01/97
//#ifndef WEBSTER	- Put back by Ranbir.
	GetApplication()->DeclarePref(TEXT("Libraries"), TEXT("FontDisplayMode"), &FontsSGallery::DefaultDisplayMode, 0, 4);
//#endif //WEBSTER
	GetApplication()->DeclarePref(TEXT("Libraries"), TEXT("FillDisplayMode"), &LibFillsSGallery::DefaultDisplayMode, 0, 2);
#endif //WEBSTER

#ifdef _DEBUG
	// Cache the library index files in memory, so index accesses require NO disk accesses, except for thumbnails
	GetApplication()->DeclarePref(TEXT("Libraries"), TEXT("CacheIndexFiles"), &Library::CacheIndexFile, 0, 1);
#endif

	// Redraw the library galleries in two passes
	GetApplication()->DeclarePref(TEXT("Libraries"), TEXT("BackgroundRedraw"), &Library::BackgroundRedraw, 0, 1);

	// Number of thumbnails to cache in memory for EACH GROUP
	GetApplication()->DeclarePref(TEXT("Libraries"), TEXT("Thumbnails"), &SGThumbs::MaxThumbnails, 1, 100);

	// Number of fields to cache in the Field cache for EACH GROUP
	GetApplication()->DeclarePref(TEXT("Libraries"), TEXT("FieldCache"), &Library::MaxFieldCacheEntries, 1, 1000);

	// Rip info out of existing indexes when generating new ones
	GetApplication()->DeclarePref(TEXT("Libraries"), TEXT("UseOldIndexesWhenCreating"), &GenerateIndexFile::UseOldIndexes, 0, 1);

	// Rip info out of existing indexes when generating new ones
	GetApplication()->DeclarePref(TEXT("Libraries"), TEXT("UseDocCommentsWhenCreating"), &GenerateIndexFile::UseDocComments, 0, 1);

	// Rip info out of existing indexes when generating new ones
	GetApplication()->DeclarePref(TEXT("Libraries"), TEXT("AskAboutRemoving"), &LibraryGallery::AskAboutRemoving, 0, 1);

	// Enable 'remote' library indexes if target media is read-only...
	GetApplication()->DeclarePref(TEXT("Libraries"), TEXT("RemoteIndexes"), &Library::RemoteIndexes, 0, 1);

	// Location for 'remote' library indexes (if empty, use temp)...
	GetApplication()->DeclarePref(TEXT("Libraries"), TEXT("RemoteIndexLocation"), &Library::RemoteIndexLocation);

	// Should we make sounds if the index contains them ?
	GetApplication()->DeclarePref(TEXT("Libraries"), TEXT("Enhancements"), &LibClipartSGallery::DoSounds);

	// Should we use an explorer dialog on win95 if we can ?
	GetApplication()->DeclarePref(TEXT("Libraries"), TEXT("Explorer"), &SGLibOil::UseExplorerForAdd);

	// Enable 'quick indexes'
#ifdef _DEBUG
	GetApplication()->DeclarePref(TEXT("Libraries"), TEXT("QuickIndexes"), &Library::QuickIndex, 0, 1);
#endif

	// Should we just build the groups without items for speed ?
	GetApplication()->DeclarePref(TEXT("Libraries"), TEXT("Virtualising"), &SGLibGroup::LibraryVirtualisingEnabled, 0, 1);

//WEBSTER-Martin-09/01/97
//#ifndef WEBSTER
#ifndef STANDALONE
	// Preferences for the fonts gallery
	GetApplication()->DeclareSection(TEXT("FontGallery"), 3);

	// Preview string used in the installed section
	GetApplication()->DeclarePref(TEXT("FontGallery"), TEXT("LargeInstalledDisplayString"), &FontsSGallery::LargeDisplayString);

	// Delete the corresponding TTF and FOT files in windows\system (or wherever) when deinstalling the font
	GetApplication()->DeclarePref(TEXT("FontGallery"), TEXT("DeleteFilesOnDeinstall"), &FontsSGallery::DeleteTTFandFOTfiles, 0, 1);

	// Enable or Disable ATM Alias in menus / galleries...
	GetApplication()->DeclarePref(TEXT("FontGallery"), TEXT("DisableATMAliases"), &ATMInstall::DisableATMAliases, 0, 1);
#endif \\STANDALONE
//#endif \\WEBSTER
#endif
	return TRUE;
}

/***********************************************************************************************

>	static BOOL Library::CacheLibPrefs(void);

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/97
	Returns:	FALSE if it fails; TRUE if we everything went ok
	Purpose:	Called during preference wiping to allow us to squirrel away any settings that
				we desire before they are wiped. This is useful if we want to retain newly
				installed preferences that the installer has deliberately put there! E.g the paths
				to the clipart etc on the CD.

***********************************************************************************************/

BOOL Library::CacheLibPrefs(void)
{
	// Save away the current values stored in the preferences
	// Must read the values directly from the preferences as our preference variables will
	// not have been set up yet!
	Camelot.GetPrefDirect(TEXT("Libraries"), TEXT("ClipArt"),	&g_ClipartLibraryPath);
	Camelot.GetPrefDirect(TEXT("Libraries"), TEXT("WebTheme"),	&g_WebThemeLibraryPath);
	Camelot.GetPrefDirect(TEXT("Libraries"), TEXT("Font"),		&g_FontLibraryPath);
	Camelot.GetPrefDirect(TEXT("Libraries"), TEXT("Texture"),	&g_FillsLibraryPath);

	//g_ClipartLibraryPath	= LibClipartSGallery::DefaultLibraryPath;
	//g_FontLibraryPath		= FontsSGallery::DefaultLibraryPath;
	//g_FillsLibraryPath		= LibFillsSGallery::DefaultLibraryPath;
	
	return TRUE;
}

/***********************************************************************************************

>	static BOOL Library::RestoreLibPrefs(void);

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/97
	Returns:	FALSE if it fails; TRUE if we everything went ok
	Purpose:	Called during preference wiping to allow us to squirrel away any settings that
				we desire before they are wiped. This is useful if we want to retain newly
				installed preferences that the installer has deliberately put there! E.g the paths
				to the clipart etc on the CD.

***********************************************************************************************/

BOOL Library::RestoreLibPrefs(void)
{
	// Must save the values directly into the preferences as our preference variables will
	// not have been set up yet!
	const TCHAR * pClipPath = g_ClipartLibraryPath;
	const TCHAR * pWebPath = g_WebThemeLibraryPath;
	const TCHAR * pFontPath = g_FontLibraryPath;
	const TCHAR * pFillsPath = g_FillsLibraryPath;
	// Only put back the values if they contain something useful. Mustn't put back blank entires
	// otherwise the galleries will error about bad paths!
	if (!g_ClipartLibraryPath.IsEmpty())
		Camelot.SetPrefDirect(TEXT("Libraries"), TEXT("ClipArt"),	pClipPath);
	if (!g_WebThemeLibraryPath.IsEmpty())
		Camelot.SetPrefDirect(TEXT("Libraries"), TEXT("WebTheme"),	pWebPath);
	if (!g_FontLibraryPath.IsEmpty())
		Camelot.SetPrefDirect(TEXT("Libraries"), TEXT("Font"),		pFontPath);
	if (!g_FillsLibraryPath.IsEmpty())
		Camelot.SetPrefDirect(TEXT("Libraries"), TEXT("Texture"),	pFillsPath);

//	LibClipartSGallery::DefaultLibraryPath	= g_ClipartLibraryPath;
//	FontsSGallery::DefaultLibraryPath		= g_FontLibraryPath;
//	LibFillsSGallery::DefaultLibraryPath	= g_FillsLibraryPath;

	return TRUE;
}
