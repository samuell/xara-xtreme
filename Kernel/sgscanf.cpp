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
// Gallery library FILE SCANNING class

/*
*/

#include "camtypes.h"
#include "sgscanf.h"

#include "sgscan.h"
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
#include "sgindgen.h"
#include "sgliboil.h"
#include "sglbase.h"
#include "product.h"
//#include "richard2.h"

#include "mfccopy.h"

DECLARE_SOURCE("$Revision: 662 $");

CC_IMPLEMENT_DYNCREATE(LibraryFile, ListItem)

#define new CAM_DEBUG_NEW

Library *LibraryFile::FirstLibraryAdded = NULL;		// Two statics for redraw/scroll purposes...
Library *LibraryFile::LastLibraryAdded = NULL;

/***********************************************************************************************

>	LibraryFile::LibraryFile()

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/2/95

	Purpose:	Constructor

***********************************************************************************************/

LibraryFile::LibraryFile()
{
	ParentGallery = NULL;
	ParentLibraryGallery = NULL;
	MyType = SGLib_Blank;
	InitScrollRedrawSystem();
}



/***********************************************************************************************

>	LibraryFile::~LibraryFile()

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/2/95

	Purpose:	Destructor

***********************************************************************************************/

LibraryFile::~LibraryFile()
{
	// Delete any live libraries. They will inform the gallery as they die
	if (!Libraries.IsEmpty())
		Libraries.DeleteAll();

	TRACEUSER( "Richard", _T("~LibraryFile called\n"));
}



/***********************************************************************************************

>	INT32 LibraryFile::Init(SuperGallery *ParentGal, PathName *Path, SGLibType Type,
													 BOOL Updated = FALSE, BOOL DoScroll = TRUE)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/2/95

	Inputs:		ParentGal 	- points to the parent super gallery to whack the new groups in.
				Path 		- should point to the library itself, "C:\Testlib", etc...
				Type 		- the type of library we're interested in creating
				Updated 	- true if the group has just been updated prior to calling this init function
				DoScroll	- true if you want this function to do the scroll / redraw... Otherwise it
							  is up to the caller to do it - note that you can call the DoScrollRedraw
							  function after calling the Init function, so if groups are to be shuffled
							  about afterwards, that would be the way to go...

	Returns:	The number of Libraries (display groups) created (not items created!)

	Purpose:	The main entry point for the galleries...
				Locates and runs through each entry in the main index file, if the
				entry matches the given type, we go and create a group for the sub-lib,
				and add all the items in the sub-lib to it...
				Returns number of groups created (0 indicates a possible problem)

	Notes:		This builds a list of available libraries, which is stored for future
				reference.

				This function needs splitting and tidying up sometime...

	SeeAlso:	LibraryFile::GetLibraryList

***********************************************************************************************/

INT32 LibraryFile::Init(SuperGallery *ParentGal, PathName *APath, SGLibType Type, BOOL Updated, BOOL DoScroll)
{
#ifndef EXCLUDE_GALS
	if(ParentGal == NULL || APath == NULL || !Libraries.IsEmpty())
	{
		ERROR3("LibraryFile::Init - NULL parameters are illegal OR Init called > 1 times");
		if(!Libraries.IsEmpty())
			return(Libraries.GetCount());
		else
			return 0;
	}

	BOOL ok = TRUE;

	// Tidy up Path a bit
	String_256 OurPath(APath->GetPath());
	LibraryFile::TidyUpSubPath(&OurPath);

	// Now point Path to the new pathname
	PathName ModifiedPath(OurPath);
	PathName *Path = &ModifiedPath;

	if(!ModifiedPath.IsValid())
	{
		ERROR3("LibraryFile::Init -> Modified library path is invalid");
		return 0;
	}

	// Remember the pathname and type
	MyPath = *Path;
	MyType = Type;

	ParentGallery = ParentGal;
	if(ParentGallery->IsKindOf(CC_RUNTIME_CLASS(LibraryGallery)))
		ParentLibraryGallery = (LibraryGallery *)ParentGal;
	else
	{
		ERROR3("LibraryFile::Init passed a non-library gallery - yikes...");
		return 0;
	}

	// Need to reset the Quiet status before a stream of Library::Init calls
	ParentLibraryGallery->SetQuietStatus(FALSE);

	BOOL Retry = TRUE;
	while(Retry)
	{
		Retry = FALSE;
	
		// Would be nice to have a way of adding a file to a path in PathName... Is there one ?
		if(!SGLibOil::FileExists(Path))
		{
			// We're opening the font gallery, but can't find the font library path - don't warn
			if(Type == SGLib_Font)
				return 0;

			// tell the user that the directory doesn't exist
			String_256 WarnMsg;
			String_256 DefaultIndex;
			String_256 IndexDesc;
			BOOL CanGenerate;
		
			ok = LibraryFile::GetSubIndexDetails(ParentLibraryGallery, &DefaultIndex, &IndexDesc, &CanGenerate);

			String_256 TmpPath(Path->GetLocation(FALSE));
			LibraryFile::TidyUpSubPath(&TmpPath);

			// Taken out by Graham 30/10/97: If the gallery had no directory specified,
			//we used to throw a warning which said "do you want to specify another folder?"
			//We don't do this any more, because the default is to open all galleries empty and
			//then download stuff from the Xara web site
#if 0 
			WarnMsg.MakeMsg(_R(IDS_BROWSE_OR_SCAN), (TCHAR *)IndexDesc, (TCHAR *)TmpPath);
			Error::SetError(0, WarnMsg, 0);
			INT32 ButtonPressed = InformWarning(0, _R(IDS_BROWSE), _R(IDS_RETRY), _R(IDS_CANCEL)/*, _R(IDS_HELP)*/);
#else	// WEBSTER
			INT32 ButtonPressed = 3;
#endif  // WEBSTER
			TRACEUSER( "Richard", _T("ButtonPressed: %d\n"), ButtonPressed);
			Error::ClearError();
			switch(ButtonPressed)
			{
				case 1:
				{
					// Open the Browse dialog (or the Add.. dialog as it seems to be called now)
					PathName ThePath(*Path);
				
					// This returns FALSE if Cancel was hit, or an error occurred.
 					if(!SGLibOil::GetLibPath(ParentLibraryGallery, &ThePath, CanGenerate, Type))
					{
						ERROR3("GetLibPath returned FALSE in LF::Init");
						return 0;
					}
					else
					{
						ModifiedPath = ThePath;
						if(!ModifiedPath.IsValid())
						{
							ERROR3("LibraryFile::Init -> scanned library path is invalid");
							return 0;
						}

						// Remember the pathname
						MyPath = ThePath;

						switch(Type)
						{
							case SGLib_ClipArt:
							case SGLib_Bitmap:
								LibClipartSGallery::DefaultLibraryPath = MyPath.GetPath();
								LibClipartSGallery::ClipartPath = LibClipartSGallery::DefaultLibraryPath;
								break;

							case SGLib_ClipArt_WebThemes:
								LibClipartSGallery::DefaultLibraryPath = MyPath.GetPath();
								LibClipartSGallery::WebThemePath = LibClipartSGallery::DefaultLibraryPath;
								break;

#ifndef STANDALONE
							case SGLib_Texture:
							case SGLib_Fractal:
								LibFillsSGallery::DefaultLibraryPath = MyPath.GetPath();
								break;

							case SGLib_Font:
								// WEBSTER-Martin-09/01/97 - Put back by Ranbir.
								//#ifndef WEBSTER
								FontsSGallery::DefaultLibraryPath = MyPath.GetPath();
								break; // Not in webster so we get the error below
								//#endif // WEBSTER
#endif
							default:
								ERROR2(FALSE,"Library::ScanForLocation Type not present!");
								break;
						}
					}
					break;
				}						

				case 2:
					Retry = TRUE;
#if 0
					{
						// Scan
						String_256 Result;
						if(!Library::ScanForLocation(Type, &Result))
						{
							ERROR3("No libraries found...");
							return 0;
						}

						if(!ModifiedPath.SetPathName(Result))
						{
							ERROR3("LibraryFile::Init -> scanned library path is invalid");
							return 0;
						}

						// Remember the pathname and type
						MyPath = *Path;
					}
#endif
					break;

				case 3:
					// Cancel
					return 0;
			}
		}
	}

	// Wipe libraries added to gallery for scroll / redraw purposes...
	InitScrollRedrawSystem();

	// Check the actual path exists
   	if(SGLibOil::FileExists(Path))
	{
		// Would be nice to have a way of adding a file to a path in PathName... Is there one ?
		String_256 IndexFile((const TCHAR *)Path->GetPath(TRUE)); // "%s\\XaraInfo\\index.txt"
		IndexFile += String_16(_R(IDS_LIBRARIES_XARAINFO_DIRNAME));
		IndexFile += TEXT("\\") + String_16(_R(IDS_LIBRARIES_INDEX_FILENAME));

		PathName IndexFilePath(IndexFile);
		if(!IndexFilePath.IsValid())
		{
			ERROR3("LibraryFile::Init indexfilepath is invalid");
			return 0;
		}

		CCDiskFile MainIndex;
		if (!MainIndex.InitLexer(FALSE))
		{
			// SetError!
			ERROR3("LibraryFile::LibraryFile InitLexer failed");
			return(0);
		}

	   	if(SGLibOil::FileExists(&IndexFilePath))
		{
			// Count lines in index file
			INT32 Count = CountLines(&IndexFilePath);

			TRACEUSER( "Richard", _T("%d lines in index file\n"), Count);

			// Used for the percentage display
			INT32 CurrentGroupNumber = 0;

			// Just in case there's a slow job already going on...
			SmashSlowJob();
			String_64 SlowJob(_R(IDS_LIBRARY_SCANNING));
			BeginSlowJob(Count, FALSE, &SlowJob);
 		
			// Now use the index file to create each group in turn
			if (MainIndex.open(IndexFilePath, ios::in))
			{
				MainIndex.SetWhitespace("");		// Setting this to blank lets us read non-"'d strings
				MainIndex.SetDelimiters(",");		// ,s delimit our fields
				MainIndex.SetCommentMarker('#');	// #'d lines are commented out
				MainIndex.SetStringDelimiters("");	// No string delimiters

				String_64 Directory;
				String_64 Description;
				String_64 SubIndex;
				String_64 Kind;
				LexTokenType TT;
	
				BOOL EscapePressed = FALSE;

				while(ok && !EscapePressed)
				{
					if(!MainIndex.GetToken()) break;		// Get SubLib directory name

					// Keep reading tokens until we hit a normal one... (skips line ends and
					// comments for us
					TT = MainIndex.GetTokenType();		
					while (TT != TOKEN_NORMAL && ok)
					{
						ok = MainIndex.GetToken();
						if(!ok) break;
						TT = MainIndex.GetTokenType();		
						ok = (TT != TOKEN_EOF);
						if(!ok) break;
					}
					if(!ok) break;
	
					Directory = MainIndex.GetTokenBuf();
					KillLeadingSpaces(&Directory);

					if(!MainIndex.GetToken()) break;		// Get ','
					if(!MainIndex.GetToken()) break;		// Get Description
					String_256 Description256;
					Description256 = MainIndex.GetTokenBuf();
					KillLeadingSpaces(&Description256);
					Description256.Left(&Description, 60);

					if(!MainIndex.GetToken()) break;		// Get ','
					if(!MainIndex.GetToken()) break;		// Get Sub Library Index name
					SubIndex = MainIndex.GetTokenBuf();
					KillLeadingSpaces(&SubIndex);

					if(!MainIndex.GetToken()) break;		// Get ','
					if(!MainIndex.GetToken()) break;		// Get type of files in sublib
					Kind = MainIndex.GetTokenBuf();
					KillLeadingSpaces(&Kind);
	
					BOOL Match = FALSE;
					Match = ParentLibraryGallery->CheckForIndexMatch(&Kind);

					if(Match)
					{				
						// Show status of additions
						EscapePressed = !ContinueSlowJob(CurrentGroupNumber++);
				
						// Sort pathname of sublib directory out	
						String_256 SubP(Path->GetPath(TRUE));
						SubP += Directory;
						PathName SubPath(SubP);
						if(!SubPath.IsValid())
						{
							ERROR3("LibraryFile::Init - invalid subpath");
							if(MainIndex.isOpen())
								MainIndex.close();
							EndSlowJob();								
							return 0;
						}
																   
						// Go ahead and add the new group
						if(ok)
						{
							// Create the sub lib
							Library *NewSubLib = new Library;

							if (NewSubLib != NULL)
							{
								// Create the new group in the gallery (note the TRUE for create a virtualised one if
								// we can to save time / memory)
								if(NewSubLib->Init(ParentGal, &SubPath, &Description, &SubIndex, Type, Updated, TRUE))
								{
									Libraries.AddTail(NewSubLib);

									// Keep track of libraries added for redraw purposes...
									AddNewFolderToScrollRedrawSystem(NewSubLib);
								}
								else
								{
									// This check is new, should be ok...
									delete NewSubLib;
									NewSubLib = NULL;
									ERROR3("Library::Init failed in LibraryFile::Init");
									ok = FALSE;
								}
							}
						}
					}
				}

			} else {
				// Failed to open the index file...

				// SetError?!
				ERROR3("LibraryFile::LibraryFile couldn't open index file");
				ok = FALSE;
			}

			EndSlowJob();								

		} else {
			// The directory given had no XaraInfo\index.txt file, maybe it's a sublib, check
			// For defaults...
			ok = CheckForSubIndexes(ParentGal, Path, Type, Updated);
		}

		// reclaim lexer-buffer memory
		MainIndex.DeinitLexer();

		// And close the file
		if(MainIndex.isOpen())
			MainIndex.close();

		// Scroll / redraw the newly added groups...
		if(DoScroll)
			DoScrollRedraw();
	}
	else
	{
		TRACEUSER( "Richard", _T("Path doesn't exist\n"));
	}

	// And return the number of items created
	return(Libraries.GetCount());
#endif
	return 0;
}



/***********************************************************************************************

>	static BOOL LibraryFile::GetSubIndexDetails(LibraryGallery *LG,
						 String_256 *DefaultIndex, String_256 *IndexDesc, BOOL *CanGenerate)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/4/95

	Inputs:		LG - LibraryGallery for the LibraryFile
	Outputs:	DefaultIndex - default index file name - "XaraClip.txt"
				IndexDesc - Description of gallery - "Clipart"
				CanGenerate - Do we have index generation facilities for this type of library
	Returns:	TRUE if things went ok, otherwise FALSE.
	Purpose:	Return various library specific details.

***********************************************************************************************/

BOOL LibraryFile::GetSubIndexDetails(LibraryGallery *LG, String_256 *DefaultIndex,
														String_256 *IndexDesc, BOOL *CanGenerate)
{
	if(DefaultIndex == NULL || IndexDesc == NULL || CanGenerate == NULL || LG == NULL)
	{
		ERROR3("LibraryFile::GetSubIndexDetails given null params");
		return FALSE;
	}

	if(LG == NULL)
	{
		ERROR3("LibraryFile::GetSubIndexDetails called when there was a NULL ParentLibraryGallery");
		*CanGenerate = FALSE;
	}
	else
	{
		// Get pathname of default sublib index and find out whether we can generate indexes, etc
		*CanGenerate = LG->CanCreateIndexes();
		return(LG->GetDefaults(DefaultIndex, IndexDesc, NULL));
	}

	return FALSE;
}



/***********************************************************************************************

>	BOOL LibraryFile::CheckForSubIndexes(SuperGallery *ParentGal, PathName *Path, SGLibType Type);

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/2/95

	Inputs:		ParentGal - points to the parent super gallery to whack the new groups in.
				Path - should point to the sublibrary, "C:\Testlib\NewABC", etc...
				Type - the type of library we're interested in creating

	Returns:	TRUE if we did our stuff...

	Purpose:	Checks for default sublibrary indexes. If there are non, and we have
				the technology, give the user the option to create one, and then use that.

				After all that, add the stuff in the index to the gallery as a group with a
				pathname in its title bar.
	Notes:
	SeeAlso:

***********************************************************************************************/

BOOL LibraryFile::CheckForSubIndexes(SuperGallery *ParentGal, PathName *Path, SGLibType Type, BOOL Updated)
{
	// The directory given had no XaraInfo\index.txt file, maybe it's a sublib, check
	// For defaults...

	String_256 DefaultIndex;
	String_256 IndexDesc;
	BOOL CanGenerate;
	BOOL ok = TRUE;

	// Need to reset the Quiet status before a stream of Library::Init calls
	ParentLibraryGallery->SetQuietStatus(FALSE);

	if(GetSubIndexDetails(ParentLibraryGallery, &DefaultIndex, &IndexDesc, &CanGenerate))
	{
		String_256 SubP(Path->GetPath(TRUE)); // "%s\\XaraInfo\\%s"
		SubP += String_16(_R(IDS_LIBRARIES_XARAINFO_DIRNAME));
		SubP += TEXT("\\") + DefaultIndex;
		PathName SubPath(SubP);
		if(!SubPath.IsValid())
		{
			ERROR3("LibraryFile::CheckForSubIndexes invalid subpath");
			return FALSE;
		}

		BOOL Generate = FALSE;
		BOOL Found = FALSE;

		// Is there a default sub index ?
	   	Found = SGLibOil::FileExists(&SubPath);
	   	
		if(!Found && CanGenerate)
		{
			if(Library::RemoteIndexes && GenerateIndexFile::IsDirectoryReadOnly(Path))
			{
				// Check whether there's a 'temporary' index for this directory, and 
				// possibly use that instead of the read only directory...

				String_256 RemoteLocationOfIndex;
				BOOL Existing = GenerateIndexFile::CheckForRemote(Path, &RemoteLocationOfIndex);

				if(Existing)
				{
					String_256 SubP(RemoteLocationOfIndex); // %s\\XaraInfo\\%s
					SGLibOil::AppendSlashIfNotPresent(&SubP);
					SubP += String_16(_R(IDS_LIBRARIES_XARAINFO_DIRNAME));
					SubP += TEXT("\\") + DefaultIndex;
					PathName TmpSubPath(SubP);
					
				   	Found = SGLibOil::FileExists(&TmpSubPath);

					// OK, so there's a remote index sitting pretty in the user's temporary
					// location... Use that and don't bother generating a new one...
					if(Found)
					{
						SubPath.SetPathName(SubP);
						Path->SetPathName(RemoteLocationOfIndex);
					}
				}
			}

			if(!Found)
			{
				// tell the user that there is no index file, and ask if they want one generating
				String_256 WarnMsg;
				String_256 TmpPath(SubPath.GetLocation(FALSE));
				LibraryFile::TidyUpSubPath(&TmpPath);
				WarnMsg.MakeMsg(_R(IDS_LIBRARY_NO_INDEX_FILE_GEN), (TCHAR *)IndexDesc, (TCHAR *)TmpPath);
				Error::SetError(0, WarnMsg, 0);
				//INT32 ButtonPressed = InformMessage(0, _R(IDS_NOTHUMBNAILS), _R(IDS_THUMBNAILS), _R(IDS_CANCEL)/*, _R(IDS_HELP)*/);
				INT32 ButtonPressed = InformMessage(0, _R(IDS_CREATE), _R(IDS_CANCEL));
				Error::ClearError();

				if(ButtonPressed < 2)
				{
					// Generate an index...
					String_64 Author(PRODUCT_NAME);
					Generate = GenerateDefaultIndex(&SubPath, Path, &Author, Type, TRUE);
					ok = TRUE;
				}
				else
				{
					// Cancel or help clicked
					ok = FALSE;
				}
			}
		}

		if(!Found && !CanGenerate)
		{
			// tell the user that there is no index file, and give them a cancel...
			String_256 WarnMsg;
			String_256 TmpPath(SubPath.GetLocation(FALSE));
			LibraryFile::TidyUpSubPath(&TmpPath);
			WarnMsg.MakeMsg(_R(IDS_LIBRARY_NO_INDEX_FILE), (TCHAR *)IndexDesc, (TCHAR *)TmpPath);
			Error::SetError(0, WarnMsg, 0);
			INT32 ButtonPressed = InformWarning(0, _R(IDS_CANCEL), NULL);
			Error::ClearError();
			ok = FALSE;
		}

		// Check again...
	   	Found = SGLibOil::FileExists(&SubPath);

		if((Found && ok) || (!Found && Generate && ok))
		{
			String_256 Description256 = Path->GetPath();
			AbbreviateName(Description256, 60, TRUE);

			String_64 Description(Description256);
			BOOL DoAgain = TRUE;
			while (DoAgain)
			{
				DoAgain = FALSE;

				// Create the sub lib
				Library *NewSubLib = new Library;

				if (NewSubLib != NULL)
				{
					String_64 DefIndex64(DefaultIndex);
	 				String_256 PathToAdd256 = SubPath.GetLocation(FALSE);
	 				TidyUpSubPath(&PathToAdd256);
					PathName PathToAdd(PathToAdd256);
	 				Error::ClearError();

					// Create the actual group itself
	 				if(NewSubLib->Init(ParentGal, &PathToAdd, &Description, &DefIndex64, Type, Updated))
					{
						Libraries.AddTail(NewSubLib);

						ok = TRUE;

						// Keep track of libraries added for redraw purposes...
						AddNewFolderToScrollRedrawSystem(NewSubLib);
					}
					else
					{
						delete NewSubLib;
						NewSubLib = NULL;

						String_256 WarnMsg;
						String_256 SmallPath;					
						PathToAdd256.Left(&SmallPath, 150);
						
						// "The index for '%s' seems to be invalid and requires updating."						
						WarnMsg.MakeMsg(_R(IDS_LIBRARY_DODGY_INDEX), (TCHAR *)SmallPath);
						Error::SetError(0, WarnMsg, 0);
						INT32 Button = InformWarning(0, _R(IDS_GENERATE), _R(IDS_CANCEL));
						Error::ClearError();

		 				String_256 IPathToAdd256(PathToAdd256);
						SGLibOil::AppendSlashIfNotPresent(&IPathToAdd256);
		 				IPathToAdd256 += TEXT("indexfle.txt");	// This is just to check we can write ok...
						PathName IPathToAdd(IPathToAdd256);

						if(Button == 1)
							DoAgain = SGLibOil::GenerateClicked(ParentLibraryGallery, &IPathToAdd);
					
						if(!DoAgain)
							ok = FALSE;

						Error::ClearError();
					}
				}
			}
		}
	}
	return ok;
}

/***********************************************************************************************

>	BOOL LibraryFile::AddSpecificIndex(SuperGallery *ParentGal, PathName *Path, SGLibType Type,
		StringBase *Description = NULL);

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/5/95

	Inputs:		ParentGal	- points to the parent super gallery to whack the new groups in.
				Path		- should point to the index file, "C:\Testlib\NewABC", etc...
				Type		- the type of library we're interested in creating
				Description	- Description to whack in the group title (NULL pops the path in)

	Returns:	TRUE if we found and added the index

	Purpose:	Adds a specific index to the gallery

	Notes:		This will not give the option of retrying or relocating if the index wasn't
				found, it'll simply return FALSE;

				This function can be used in preference to Init

***********************************************************************************************/

BOOL LibraryFile::AddSpecificIndex(SuperGallery *ParentGal, PathName *Path, SGLibType Type,
	StringBase *Description, UINT32 bModified, BOOL bIsWebLib)
{
	if(ParentGal == NULL || Path == NULL || !Libraries.IsEmpty() || !Path->IsValid())
	{
		ERROR3("LibraryFile::AddSpecificIndex - Problems on entry");
		return FALSE;
	}

	// Check the path for the library exists
//	if(!SGLibOil::FileExists(Path))
//		return FALSE;

	// Remember the pathname and type
	MyPath = *Path;
	MyType = Type;
	ParentGallery = ParentGal;

	// Create the sub lib
	Library *NewSubLib = new Library;

	if (NewSubLib != NULL)
	{
		// Path = "C:\testlib\animals\xarainfo\animals.txt"
		// Path of files themselves... FilesPath = "C:\testlib\animals"
		NewSubLib->m_bIsWebLib = bIsWebLib;
		PathName TmpPath(Path->GetLocation(FALSE));
		NewSubLib->SetModified(bModified);

		// If it's a root directory we need the slash...
		PathName FilesPath;
		String_256 TmpRootCheck(TmpPath.GetLocation(TRUE));
		if(SGLibOil::IsRootDirectory(&TmpRootCheck))
			FilesPath.SetPathName(TmpPath.GetLocation(TRUE));
		else
			FilesPath.SetPathName(TmpPath.GetLocation(FALSE));

		// Default title for the new group
		String_256 Desc256;
		String_64 Desc64;
		if(Description == NULL)
			Desc256 = Path->GetTruncatedPath(60);
		else
			Desc256 = *Description;
		Desc256.Left(&Desc64, 64);
											
		// The filename of the index
		String_64 IndexFilename(Path->GetFileName(TRUE));

		// Add this new index to the library file and scan it in...
		if(NewSubLib->Init(ParentGal, &FilesPath, &Desc64, &IndexFilename, Type, FALSE, (Description != NULL)))
		{
			Libraries.AddTail(NewSubLib);

			// Keep track of libraries added for redraw purposes...
			AddNewFolderToScrollRedrawSystem(NewSubLib);

			return TRUE;
		}
		else
		{
			// if there was a problem, don't go round leaving memory everywhere...
			delete NewSubLib;
			NewSubLib = NULL;
		}
	}

	// Nothing added
	return FALSE;
}



/***********************************************************************************************

>	static BOOL LibraryFile::TidyUpSubPath(String_256 *Str)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/4/95

	Inputs:		Str - The input string
	Outputs:	Str - The output string
	Returns:	TRUE if things went ok

	Purpose:	Removes trailing backslashes and trailing xarainfo's from the string.
				This means we can browse to a xarainfo directory then jump back a place
				to get the correct library subpath, etc...

				There's also a 'special case' for root directories... Instead of returning
				"C:" we return "C:\". This will not work with unc stuff, but then neither
				will most of the gallery code :-(

***********************************************************************************************/

BOOL LibraryFile::TidyUpSubPath(String_256 *Str)
{
	if(Str == NULL)
	{	
		ERROR3("LibraryFile::TidyUpSubPath given a null string");
		return FALSE;
	}

	String_256 OurPath(*Str);

	// Remove a trailing '\' if there is one
	if(((TCHAR *)OurPath)[OurPath.Length() - 1] == '\\')
	{
		Str->Left(&OurPath, Str->Length() - 1);
	}

	if(OurPath.Length() > 10)
	{
		// Remove a 'XaraInfo' if there is one on the end
		String_256 OurPathTmp(OurPath);	
		String_16 RighthandSection;
		OurPathTmp.Right(&RighthandSection, 9);
		RighthandSection.toUpper();

		// \\XARAINFO
		String_16 XaraInfo("\\" + String_16(_R(IDS_LIBRARIES_XARAINFO_DIRNAME_CAPS)));
		if(lstrcmp((TCHAR *)RighthandSection, (TCHAR *)XaraInfo) == 0)
		{
			// Use the un-uppered string to return
			OurPathTmp.Left(&OurPath, OurPath.Length() - 9);
		}
	}

	// Check if root directory, if so, add in a trailing '\' (fixes root directory problems)
	if(SGLibOil::IsRootDirectory(&OurPath))
	{
		//ERROR3("Drive name given - appending a slash");
		OurPath += TEXT("\\");
	}

	*Str = OurPath;
 	return TRUE;
}

/***********************************************************************************************

>	static INT32 LibraryFile::CountLines(PathName *IndexFilePath)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/4/95

	Inputs:		IndexFilePath - pathname of file we're interested in
	Outputs:
	Returns:	Number of lines found

	Purpose:	Count number of valid lines in a file

***********************************************************************************************/

INT32 LibraryFile::CountLines(PathName *IndexFilePath)
{
	INT32 Count = 0;

	CCDiskFile TmpDiskFile;
	if (!TmpDiskFile.InitLexer(FALSE))
	{
		ERROR3("Library::CountLines - Problems with InitLexer");
		return 0;
	}

	if (TmpDiskFile.open(*IndexFilePath, ios::in))
	{
		BOOL Finished = FALSE;;
		LexTokenType TokType = TOKEN_LINE;
		while(TmpDiskFile.GetLineToken() && TokType != TOKEN_EOF)
		{
			TokType = TmpDiskFile.GetTokenType();
				if(TokType == TOKEN_LINE)
					Count ++;
		}
	}
	
	// reclaim lexer-buffer memory
	TmpDiskFile.DeinitLexer();

	if(TmpDiskFile.isOpen())
		TmpDiskFile.close();

	return Count;
}
	
/***********************************************************************************************

>	BOOL LibraryFile::KillLeadingSpaces(StringBase *Str)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/2/95

	Inputs:		Str - The string to have leading spaces killed
	Outputs:	Str - modified to have leading spaces stripped off

	Returns:	FALSE in a major crisis situation, which, looking at the code is pretty unlikely
				since it only ever returns TRUE.

	Purpose:	Strip any leading AND NOW TRAILING spaces from the given string.

***********************************************************************************************/

BOOL LibraryFile::KillLeadingSpaces(StringBase *Str)
{
	ERROR3IF(Str == NULL, "LibraryFile::KillLeadingSpaces - NULL params are illegal");

	if( ((TCHAR *)(*Str))[0] != ' ' && ((TCHAR *)(*Str))[Str->Length()-1] != ' ')
		return TRUE;

	INT32 Length = Str->Length();
	INT32 FirstNonSpaceChar = 0;
	while (((TCHAR *)(*Str))[FirstNonSpaceChar] == ' ' && FirstNonSpaceChar < Length)
		FirstNonSpaceChar++;

	if(FirstNonSpaceChar >= Length)
		*Str = "";
	else
	{
		// Strip off trailing spaces...
		INT32 LastNonSpaceChar = Length;
		while (((TCHAR *)(*Str))[LastNonSpaceChar-1] == ' ' && LastNonSpaceChar > 0)
			LastNonSpaceChar--;

		String_256 Str256;
		Str->Mid(&Str256, FirstNonSpaceChar, LastNonSpaceChar - FirstNonSpaceChar);
		*Str = Str256;
	}

	return TRUE;
}


/***********************************************************************************************

>	PathName *LibraryFile::GetLibraryPathName(void)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/2/95

	Returns:	The disc pathname of this library file

	Purpose:	To determine the pathname with which this library was initialised

	SeeAlso:	LibraryFile::Init

***********************************************************************************************/

PathName *LibraryFile::GetLibraryPathName(void)
{
	// Did the client forget to call Init() before calling this method?
	ERROR3IF(MyType == SGLib_Blank, "This LibraryFile has not been properly initialised!");

	return(&MyPath);
}



/***********************************************************************************************

>	SGLibType LibraryFile::GetLibraryType(void)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/2/95

	Returns:	The library type of this library file

	Purpose:	To determine the librry type of this library file and its Libraries

	SeeAlso:	LibraryFile::Init

***********************************************************************************************/

SGLibType LibraryFile::GetLibraryType(void)
{
	// Did the client forget to call Init() before calling this method?
	ERROR3IF(MyType == SGLib_Blank, "This LibraryFile has not been properly initialised!");

	return(MyType);
}


/***********************************************************************************************

>	static BOOL LibraryFile::GenerateDefaultIndex(PathName *IndexFile, PathName *LibDirPath,
														SGLibType Type, BOOL Thumbnails)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/4/95

	Inputs:		IndexFile - Full path of index file to generate - C:\testlib\animals\xarainfo\xaraclip.txt
				LibDirPath - Full path of sublibrary - C:\testlib\animals
				Author - Default author for index entries
				Type - Type of library index required 
				Thumbnails - Generate thumbnails if we can ?
	Outputs:
	Returns:	TRUE if things went ok.

	Purpose:	Generates a dummy index file for a directory of say .art files.

***********************************************************************************************/

BOOL LibraryFile::GenerateDefaultIndex(PathName *IndexFile, PathName *LibDirPath, String_64 *Author,
														SGLibType Type, BOOL Thumbnails)
{
	if(IndexFile == NULL || LibDirPath == NULL)
	{
		ERROR3("LibraryFile::GenerateDefaultIndex null params are bad");
		return FALSE;
	}

	GenerateIndexFile TmpGen;
	BOOL ok = TmpGen.Generate(IndexFile, LibDirPath, Author, Type, Thumbnails);

	return ok;
}







/***********************************************************************************************

>	void LibraryFile::InitScrollRedrawSystem(void)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/9/95

	Inputs:
	Outputs:
	Returns:
	Purpose:	Resets the static vars...
	Notes:
	SeeAlso:

***********************************************************************************************/

void LibraryFile::InitScrollRedrawSystem(void)
{
	FirstLibraryAdded = NULL;
	LastLibraryAdded = NULL;
}

/***********************************************************************************************

>	void LibraryFile::AddNewFolderToScrollRedrawSystem(Library *NewFolder)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/9/95

	Inputs:
	Outputs:
	Returns:
	Purpose:	Adds a new library to the Scroll / redraw system
	Notes:
	SeeAlso:

***********************************************************************************************/

void LibraryFile::AddNewFolderToScrollRedrawSystem(Library *NewFolder)
{
	// Keep track of libraries added for redraw purposes...
	if(FirstLibraryAdded == NULL)
		FirstLibraryAdded = NewFolder;
	LastLibraryAdded = NewFolder;
}

/***********************************************************************************************

>	void LibraryFile::DoScrollRedraw(void)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/9/95
	Scope:		Public
	Purpose:	Scrolls to newly added groups...

***********************************************************************************************/

void LibraryFile::DoScrollRedraw(void)
{
	// Scroll / redraw the newly added groups...
	if(FirstLibraryAdded != NULL && LastLibraryAdded != NULL)
	{
		SGLibGroup *FirstGroup = FirstLibraryAdded->ParentGroup;
		SGLibGroup *SecondGroup = LastLibraryAdded->ParentGroup;
	
		if(FirstGroup != NULL && SecondGroup != NULL)
		{
			// Redraw whatever we need to and reformat the gallery ready for poking...
			ParentGallery->ReformatAndRedrawIfNecessary();

			// Calculate where the new folders are...
			DocRect ScrollToRect;
			FirstGroup->GetFormatRect(&ScrollToRect);
			if(SecondGroup != FirstGroup)
			{
				DocRect TempRect;
				FirstGroup->GetFormatRect(&TempRect);				
				ScrollToRect = ScrollToRect.Union(TempRect);
			}

			// Actually do the scroll...
			ParentGallery->ScrollToShow(&ScrollToRect, TRUE);
		}
	}
}
