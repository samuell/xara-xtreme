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
// SGRMfile.cpp - Gallery Resource Management file routines

#include "camtypes.h"
#include "sgrmfile.h"

#include "product.h"	// for PRODUCT_BASENAME
#include "sgliboil.h"
//#include "pathname.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "sgscan.h"		// Generate code with hourglass... - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "nev.h"
#include "progress.h"
//#include "list.h" - in camtypes.h [AUTOMATICALLY REMOVED]

  
// Implement the dynamic class bits...
CC_IMPLEMENT_DYNCREATE(LibSettingsListItem, ListItem)
CC_IMPLEMENT_DYNCREATE(LibSettings, CCObject)

// This line mustn't go before any CC_IMPLEMENT_... macros
#define new CAM_DEBUG_NEW

/********************************************************************************************

>	LibSettingsListItem::LibSettingsListItem(String_256 *Entry)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/1/96
	Inputs:		Entry	- The Entry
	Purpose:	A GRM file line list entry

********************************************************************************************/

LibSettingsListItem::LibSettingsListItem(String_256 *Entry)
{
	if(Entry != NULL)
		Line = new String_256(*Entry);
	else
		Line = NULL;
}

/********************************************************************************************

>	LibSettingsListItem::LibSettingsListItem()

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/1/96
	Purpose:	GRM file line list entry default contructor

********************************************************************************************/

LibSettingsListItem::LibSettingsListItem()
{
	ERROR3("LibSettingsListItem::LibSettingsListItem - call the other constructor");
	Line = NULL;
}

/********************************************************************************************

>	LibSettingsListItem::~LibSettingsListItem()

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/1/96
	Purpose:	GRM file line list entry destructor

********************************************************************************************/

LibSettingsListItem::~LibSettingsListItem()
{
	if(Line != NULL)
	{
		delete Line;
		Line = NULL;
	}
}




/********************************************************************************************

>	LibSettings::LibrarySettings()

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/5/95
	Inputs:
	Outputs:
	Returns:
	Purpose:	The Gallery Resource Management code...

	Notes:		Currently this code uses the GRM file to remember which groups were in which
				libraries and in which order when quitting the program so than on loading next
				time we can reflect the old status in the library...

********************************************************************************************/

LibSettings::LibSettings()
{
}

/********************************************************************************************

>	LibSettings::~LibrarySettings()

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/5/95
	Inputs:
	Outputs:
	Returns:
	Purpose:	Destructor

********************************************************************************************/

LibSettings::~LibSettings()
{
}


/********************************************************************************************

>	static BOOL LibSettings::Init(void)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/5/95
	Inputs:
	Outputs:
	Returns:
	Purpose:	Value returning initialisation routine...

********************************************************************************************/

BOOL LibSettings::Init(void)
{
	return TRUE;
}


/********************************************************************************************

>	static BOOL LibSettings::GetEntry(String_256 *Section, INT32 Line, String_256 *Entry,
			FilePos *Pos = NULL, String_256 * pIniFileString = NULL)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/5/95
	Inputs:		Section	- Text to match with the section names
				Line	- Line offset in the section to return
				pIniFileString	set to NULL if using default gallery ini file (Default)
								or to the leafname of the ini file to use
	Outputs:	Entry	- The line associated with the two input params
				Pos		- The FilePos after the item was found
	Returns:	TRUE if this was successful. FALSE if we hit the next section, or EOF
				or if the file doesn't exist...

	Purpose:	Returns a single line from the grm file.

	Notes:		As this is fairly general read and write ini file code, then this has been
				expanded so that an option other ini file name can be specified. If this is
				specified then this will be used instead of the GRM file name.
				Added by Neville 14/1/97 for saving/loading paths for plug-ins as the main 
				ini file code cannot cope with the concept of sections filled with an arbitrary
				number of items which need cleaning out when resaving to remove old items no
				longer required.

********************************************************************************************/

BOOL LibSettings::GetEntry(String_256 *Section, INT32 Line, String_256 *Entry, FilePos *Pos,
						   String_256 * pIniFileString)
{
	ERROR3IF(Entry == NULL, "LibSettings::GetEntry - not happy with null params");

	BOOL ok = FALSE;

	CCDiskFile GetEntryFile(1024, FALSE, TRUE);

	if(GetEntryStart(&GetEntryFile))
	{
		ok = GetEntryFromFile(&GetEntryFile, Section, Line, Entry, Pos);
	}

	GetEntryFinish(&GetEntryFile);
	
	return ok;	
}


/********************************************************************************************

>	static BOOL LibSettings::GetEntryStart(CCDiskFile *DiskFile, String_256 * pIniFileString = NULL)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/1/96
	Inputs:		DiskFile	- The DiskFile to use in following GetEntryFromFile calls
				pIniFileString	set to NULL if using default gallery ini file (Default)
								or to the leafname of the ini file to use
	Outputs:	DiskFile	- This will be initialised and opened withe the correct grm file
	Returns:	TRUE if this was successful

	Purpose:	Call with a pre-created, but not initialised CCDiskFile. After calling this,
				calls to GetEntryFromFile can be passed this diskfile. When finished, call
				GetEntryFinish with the DiskFile to De-do all the stuff we done here, like...

	Notes:		As this is fairly general read and write ini file code, then this has been
				expanded so that an option other ini file name can be specified. If this is
				specified then this will be used instead of the GRM file name.
				Added by Neville 14/1/97 for saving/loading paths for plug-ins as the main 
				ini file code cannot cope with the concept of sections filled with an arbitrary
				number of items which need cleaning out when resaving to remove old items no
				longer required.

********************************************************************************************/

BOOL LibSettings::GetEntryStart(CCDiskFile *DiskFile, String_256 * pIniFileString)
{
	if(DiskFile == NULL)
	{
		ERROR3("LibSettings::GetEntryStart given null diskfile");
		return FALSE;
	}
	
	// Get the pathname of the grm file
	PathName GRMPath;
	if(!GetPathName(&GRMPath, pIniFileString))
	{
		ERROR3("LibSettings::GetEntryStart couldn't even get the pathname without going wrong...");
		return FALSE;
	}

	if(!DiskFile->InitLexer(TRUE))
	{
		ERROR3("LibSettings::GetEntryStart - Problems with InitLexer");
		return FALSE;
	}

	TRY
	{
		// Open the GRM file
		if(!DiskFile->open(GRMPath, ios::in | ios::nocreate))
			return FALSE;
	}	
	CATCH(CFileException, e)
	{
		// Something bad happened with the GRM file. It possibly doesn't exist...
		Error::ClearError();
		return FALSE;
	}
	END_CATCH

	return TRUE;
}

/********************************************************************************************

>	static BOOL LibSettings::GetEntryFinish(CCDiskFile *DiskFile)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/1/96
	Inputs:		DiskFile	- The DiskFile to tidy up
	Outputs:
	Returns:	TRUE if this was successful

	Purpose:	Call after GetEntryStart, and after any GetEntryFromFile calls, but before
				killing the DiskFile off.
				This will close the file, and de-initialise the lexer, etc...

********************************************************************************************/

BOOL LibSettings::GetEntryFinish(CCDiskFile *DiskFile)
{
	if(DiskFile == NULL)
	{
		ERROR3("LibSettings::GetEntryFinish given null diskfile");
		return FALSE;
	}

	if(DiskFile == NULL)
		return TRUE;

	BOOL ok = TRUE;

	// Make sure that the files are closed and the memory is reclaimed properly...
	TRY
	{
		// Tidy up...
		DiskFile->DeinitLexer();
		if (DiskFile->isOpen()) DiskFile->close();
	}
	CATCH(CFileException, e)
	{
		// Failed to close the files - not much we can do about it really
		Error::ClearError();
		ok = FALSE;
	}
	END_CATCH

	return ok;
}


/********************************************************************************************

>	static BOOL LibSettings::GetEntryFromFile(CCLexFile *TheFile, String_256 *Section, INT32 Line,
		String_256 *Entry)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/1/96
	Inputs:		TheFile	- An open and initialised LexFile which we should be using
				Section	- Text to match with the section names
				Line	- Line offset in the section to return
	Outputs:	Entry	- The line associated with the two input params
				Pos		- The FilePos after the item was found

	Returns:	TRUE if this was successful. FALSE if we hit the next section, or EOF
				or if the file doesn't exist...

	Purpose:	Returns a single line from the grm file.

********************************************************************************************/

BOOL LibSettings::GetEntryFromFile(CCLexFile *TheFile, String_256 *Section, INT32 Line, String_256 *Entry,
								   FilePos *Pos)
{
	BOOL FoundLine = FALSE;
	BOOL ok = TRUE;

	TRY
	{
		// When we're doing lines in the section specified this will be true
		BOOL InOurSection = FALSE;

		// This will count the number of the lines since the last section header
		INT32 LineCount = 0;

		String_256 TokenBuffer;
		LexTokenType TokType = TOKEN_LINE;

		// Loop through file checking for sections and incrementing the line count
		// Finish at EOF, if we find a token, or if there was a problem...
		while(TokType != TOKEN_EOF && ok && !FoundLine && TheFile->GetLineToken())
		{
			TokType = TheFile->GetTokenType();

			if(TokType == TOKEN_LINE)
			{
				TokenBuffer = TheFile->GetTokenBuf();
				LineCount ++;

				// Check if we've hit a new section name, if so check it and stop the output...
				String_256 PossibleSectionName;				
				if(IsThisANewSection(&TokenBuffer, &PossibleSectionName))
				{
					LineCount = 0;
					if(InOurSection)
					{
						// ERROR3("LibSettings::GetEntry - Line outside section");
						ok = FALSE;
					}
					InOurSection = (PossibleSectionName == *Section);
				}

				// Check if it's the line we're interested in or not
				if(InOurSection && Line == LineCount)
				{
					*Entry = TokenBuffer;
					FoundLine = TRUE;
				}
			}
		}
	}

	// See if there was a file io error
	CATCH(CFileException, e)
	{
		// Something rather unhelpful has taken place...
		ok = FALSE;

		// Report the error if no one else did
		if (Error::GetErrorNumber() != _R(IDN_USER_CANCELLED))
			InformError();
		else
			Error::ClearError();	// otherwise remove the error so it won't get reported
	}
	END_CATCH

	TRY
	{
		// Tidy up...
		if(Pos != NULL)
			*Pos = TheFile->tellIn();
	}
	CATCH(CFileException, e)
	{
		// Failed to close the files - not much we can do about it really
		Error::ClearError();
	}
	END_CATCH

#ifdef _DEBUG
	// Something went wrong...
	if(!ok && FoundLine)
	{
		PathName GRMPath;
		if(GetPathName(&GRMPath))
			if(SGLibOil::FileExists(&GRMPath))
				ERROR3("LibSettings::GetEntry - Something went wrong...");
	}
#endif

	return (FoundLine && ok);
}


/********************************************************************************************

>	static BOOL LibSettings::AddLine(String_256 *Line, String_256 * pIniFileString = NULL)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/5/95
	Inputs:		Line	- Line of text to add to file
				pIniFileString	set to NULL if using default gallery ini file (Default)
								or to the leafname of the ini file to use
	Outputs:
	Returns:	TRUE if this was successful.

	Purpose:	Adds a single line to the end of the grm file.

	Notes:		As this is fairly general read and write ini file code, then this has been
				expanded so that an option other ini file name can be specified. If this is
				specified then this will be used instead of the GRM file name.
				Added by Neville 14/1/97 for saving/loading paths for plug-ins as the main 
				ini file code cannot cope with the concept of sections filled with an arbitrary
				number of items which need cleaning out when resaving to remove old items no
				longer required.

********************************************************************************************/

BOOL LibSettings::AddLine(String_256 *Line, String_256 * pIniFileString)
{
	ERROR3IF(Line == NULL, "LibSettings::AddLine - not happy with null params");

	BOOL ok = TRUE;

	PathName GRMPath;
	if(!GetPathName(&GRMPath, pIniFileString))
	{
		ERROR3("LibSettings::AddLine - Problems getting pathname for grm file");
		return FALSE;
	}

	CCDiskFile GRMFile(1024, FALSE, TRUE);

	TRY
	{
		// Initialise the lexer memory for the token routines...
		if (!GRMFile.InitLexer(FALSE))
		{
			ERROR3("LibSettings::AddLine - Problems with InitLexer");
			ok = FALSE;
		}

		// Open file for appending to...
		if (ok && !GRMFile.open(GRMPath, ios::app))
		{
			ERROR3("LibSettings::AddLine - Failed to open the grm file for appending");
			ok = FALSE;
		}

		// Pop the token at the end of the file...
		if(ok) ok = GRMFile.PutToken(*Line, (String_8)"\r\n");
	}

	// See if there was a file io error
	CATCH(CFileException, e)
	{
		// Report the error if no one else did
		if (Error::GetErrorNumber() != _R(IDN_USER_CANCELLED))
			InformError();
		else
			Error::ClearError();	// otherwise remove the error so it won't get reported

		ok = FALSE;
	}
	END_CATCH

	// Make sure that the file is closed
	TRY
	{
		// Tidy up...
		GRMFile.DeinitLexer();
		if (GRMFile.isOpen()) GRMFile.close();
	}
	CATCH(CFileException, e)
	{
		// Failed to close the file - not much we can do about it really
		Error::ClearError();
	}
	END_CATCH

	if(!ok) ERROR3("LibSettings::AddLine - Something went wrong");
	return ok;
}


/********************************************************************************************

>	static BOOL LibSettings::AddSection(String_256 *Section, String_256 * pIniFileString = NULL)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/5/95
	Inputs:		Section	- Section name to add to file
				pIniFileString	set to NULL if using default gallery ini file (Default)
								or to the leafname of the ini file to use
	Outputs:
	Returns:	TRUE if this was successful.

	Purpose:	Adds a new section to the end of the grm file.

	Notes:		This is essentially the same as AddLine but it gives us control of what
				sections look like.

	Notes:		As this is fairly general read and write ini file code, then this has been
				expanded so that an option other ini file name can be specified. If this is
				specified then this will be used instead of the GRM file name.
				Added by Neville 14/1/97 for saving/loading paths for plug-ins as the main 
				ini file code cannot cope with the concept of sections filled with an arbitrary
				number of items which need cleaning out when resaving to remove old items no
				longer required.

********************************************************************************************/

BOOL LibSettings::AddSection(String_256 *Section, String_256 * pIniFileString)
{
	ERROR3IF(Section == NULL, "LibSettings::AddSection - not happy with null params");

	String_256 Line("[");
	Line += *Section;
	Line += "]";
	return AddLine(&Line, pIniFileString);
}


/********************************************************************************************

>	static BOOL LibSettings::IsThisANewSection(String_256 *Line, String_256 *Section)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/5/95
	Inputs:		Line	- Line of text to check
	Outputs:	Section - The actual section name itself
	Returns:	TRUE if this line is a new section name

	Purpose:	Checks if a given line is a new section or not and returns the section name
				if it is...

********************************************************************************************/

BOOL LibSettings::IsThisANewSection(String_256 *Line, String_256 *Section)
{
	ERROR3IF(Section == NULL, "LibSettings::IsThisANewSection - not happy with null params");
	ERROR3IF(Line == NULL, "LibSettings::IsThisANewSection - not happy with null params");

	if((Line->Sub((String_8)"[") != -1) && (Line->Sub((String_8)"]") != -1))
	{
		INT32 first = Line->Sub((String_8)"[");
		INT32 second = Line->Sub((String_8)"]");

		if(first < second-1)
		{
			Line->Mid(Section, first+1, (second - first)-1);
			return TRUE;
		}
	}
	return FALSE;
}


/********************************************************************************************

>	static BOOL LibSettings::StartOutput(String_256 Section, String_256 * pIniFileString = NULL)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/5/95
	Inputs:		Section - The section we're about to add
				pIniFileString	set to NULL if using default gallery ini file (Default)
								or to the leafname of the ini file to use
	Outputs:
	Returns:	TRUE if this was successful.

	Purpose:	Checks if there's already a grm file on the disk. If so, create a new .bak file
				holding copies of all the sections (except the one specified), then copy this
				new file over the old .grm file.

				Doing this means that future calls to AddLine will append the information to
				the end of the grm file, and there won't be any duplication.

	Notes:		This avoids the old system of sending a message around all the galleries, and
				also means that unopened galleries don't need to store a list of their groups.

				Also, if there's a crisis, the old grm file is still on the disk...

	Notes:		As this is fairly general read and write ini file code, then this has been
				expanded so that an option other ini file name can be specified. If this is
				specified then this will be used instead of the GRM file name.
				Added by Neville 14/1/97 for saving/loading paths for plug-ins as the main 
				ini file code cannot cope with the concept of sections filled with an arbitrary
				number of items which need cleaning out when resaving to remove old items no
				longer required.

********************************************************************************************/

BOOL LibSettings::StartOutput(String_256 *Section, String_256 * pIniFileString)
{
	ERROR3IF(Section == NULL, "LibSettings::StartOutput - not happy with null params");

	// Get the pathname of the grm file
	PathName GRMPath;
	BOOL ok = GetPathName(&GRMPath, pIniFileString);
	if(!ok)
	{
		ERROR3("LibSettings::StartOutput couldn't even get the pathname without going wrong...");
		return FALSE;
	}

	if(!SGLibOil::FileExists(&GRMPath))
	{
		// No existing GRM file, don't bother keeping its contents...
		return TRUE;
	}

	PathName BAKPath(GRMPath);
	BAKPath.SetType("bak");

	CCDiskFile GRMFile(1024, FALSE, TRUE);
	CCDiskFile BAKFile(1024, FALSE, TRUE);

	TRY
	{
		if (!GRMFile.InitLexer(FALSE) || !BAKFile.InitLexer(FALSE))
		{
			ERROR3("LibSettings::StartOutput - Problems with InitLexer");
			ok = FALSE;
		}

		if(ok) ok = GRMFile.open(GRMPath, ios::in | ios::nocreate);
		if(ok) ok = BAKFile.open(BAKPath, ios::out);
		
		if(ok)
		{
			// When we're doing lines in the section specified this will be true
			BOOL InOurSection = FALSE;
			BOOL Finished = FALSE;

			String_256 TokenBuffer;
			LexTokenType TokType = TOKEN_LINE;

			// Read and write the lines...
			while(TokType != TOKEN_EOF && ok && GRMFile.GetLineToken())
			{
				TokType = GRMFile.GetTokenType();

				if(TokType == TOKEN_LINE)
				{
					TokenBuffer = GRMFile.GetTokenBuf();

					// Check if we've hit a new section name, if so check it and stop the output...
					String_256 PossibleSectionName;				
					if(IsThisANewSection(&TokenBuffer, &PossibleSectionName))
						InOurSection = (PossibleSectionName == *Section);

					// If it's not the specified section, write the line straight out to the bak file
					if(!InOurSection)
					{
						if(ok) ok = BAKFile.PutToken(TokenBuffer, (String_8)"\r\n");
					}
				}
			}
		}
	}

	// See if there was a file io error
	CATCH(CFileException, e)
	{
		// Something rather unhelpful has taken place...
		ok = FALSE;

		// Report the error if no one else did
		if (Error::GetErrorNumber() != _R(IDN_USER_CANCELLED))
			InformError();
		else
			Error::ClearError();	// otherwise remove the error so it won't get reported
	}
	END_CATCH

	// Make sure that the files are closed and the memory is reclaimed properly...
	TRY
	{
		// Tidy up...
		GRMFile.DeinitLexer();
		BAKFile.DeinitLexer();
		if (GRMFile.isOpen()) GRMFile.close();
		if (BAKFile.isOpen()) BAKFile.close();
	}
	CATCH(CFileException, e)
	{
		// Failed to close the files - not much we can do about it really
		Error::ClearError();
	}
	END_CATCH

	// Copy the BAK file over the GRM file
	if(ok) SGLibOil::FileDelete(&GRMPath);
	if(ok) ok = SGLibOil::FileCopy(&BAKPath, &GRMPath);
	
	// Always delete the bak file, whatever happened
	SGLibOil::FileDelete(&BAKPath);

	// Something went wrong...
	if(!ok) ERROR3("LibSettings::StartOutput - Something went wrong...");
	return ok;
}

/********************************************************************************************

>	static BOOL LibSettings::NukeSettings(String_256 * pIniFileString = NULL)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/6/95
	Inputs:		
	Outputs:
	Returns:	TRUE if this was successful.

	Purpose:	Delete all the settings from the gallery resource file.

				This will be called when the user hits 'default' at startup, so the galleries
				will have to find some other way of working out where their groups are - the
				ini file for instance, but since this will also have been destroyed, there only
				remains 1 place - the default static setting - eek !

	Notes:		As this is fairly general read and write ini file code, then this has been
				expanded so that an option other ini file name can be specified. If this is
				specified then this will be used instead of the GRM file name.
				Added by Neville 14/1/97 for saving/loading paths for plug-ins as the main 
				ini file code cannot cope with the concept of sections filled with an arbitrary
				number of items which need cleaning out when resaving to remove old items no
				longer required.

********************************************************************************************/

BOOL LibSettings::NukeSettings(String_256 * pIniFileString)
{
	PathName GRMFile;
	if(LibSettings::GetPathName(&GRMFile, pIniFileString))
	{
		SGLibOil::FileDelete(&GRMFile);
		return TRUE;
	}
	return FALSE;
}

/********************************************************************************************

>	static BOOL LibSettings::GetPathName(PathName *PathName, String_256 * pIniFileString = NULL)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/5/95
	Inputs:		pIniFileString	set to NULL if using default gallery ini file (Default)
								or to the leafname of the ini file to use
	Outputs:	PathName	- returns the pathname of the settings file in c:\windows, or wherever
	Returns:	TRUE if successful
	Purpose:	Returns the path and filename for the gallery resource management file...
				Currently this file holds the groups in the gallery for use when it next
				starts up... 
	
	Notes:		As this is fairly general read and write ini file code, then this has been
				expanded so that an option other ini file name can be specified. If this is
				specified then this will be used instead of the GRM file name.
				Added by Neville 14/1/97 for saving/loading paths for plug-ins as the main 
				ini file code cannot cope with the concept of sections filled with an arbitrary
				number of items which need cleaning out when resaving to remove old items no
				longer required.

********************************************************************************************/

BOOL LibSettings::GetPathName(PathName *PathName, String_256 * pIniFileString)
{
	ERROR3IF(PathName == NULL, "LibSettings::GetPathName - not happy with null params");

	String_256 GRMFile;

	// Have a look in the windows directory for our GRM file...
	UINT32 Len = GetWindowsDirectory((TCHAR *)GRMFile, 255);
	if ((Len > 0) && (Len < _MAX_PATH))
	{
		GRMFile += "\\";
		// File was called XStudio.grm or XViewer.grm
		// We will now call them all CorelX##.ini where ## in this case is GM
		String_256 File(PRODUCT_GALLERY_INIFILE);
		// If somebody has asked us to use a different ini file name then use this instead
		if (pIniFileString != NULL)
			File = *pIniFileString;

		GRMFile += File;

		//TRACEUSER( "Richard", _T("Path Gallery Resource Management file - %s\n"), (TCHAR *)GRMFile);

		PathName->SetPathName(GRMFile);
		if(!PathName->IsValid())
		{
			Error::ClearError();
			ERROR3("LibSettings::GetPathName - invalid pathname");
			return FALSE;
		}
		
		return TRUE;
	}

	ERROR3("LibSettings::GetPathName - Problems getting the windows directory name for the Gallery Resource Management file");

	// oops .. Problems getting the windows directory name
	return FALSE;
}


/********************************************************************************************

>	static BOOL LibSettings::BuildList(String_256 *Section, List *OutputList);

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/1/96
	Inputs:		Section		- Section title for who contents should be popped in the list
	Outputs:	OutputList	- Appends each line it finds to this list
	Returns:	TRUE if this was successful.

	Purpose:	Given a section, build a list of entries in the section. This should speed
				reading lots of things from a section up by quite a bit...

********************************************************************************************/

BOOL LibSettings::BuildList(String_256 *Section, List *OutputList)
{
	ERROR3IF(Section == NULL || OutputList == NULL, "LibSettings::BuildList given NULL jobbies...");

	BOOL AddedItems = FALSE;
	String_256 Entry;
	INT32 Line = 1;

	CCDiskFile GetEntryFile(1024, FALSE, TRUE);

	if(GetEntryStart(&GetEntryFile))
	{
		if(SeekToSection(&GetEntryFile, Section))
		{
			while(GetNextLine(&GetEntryFile, &Entry))
			{
				LibSettingsListItem *LibSettingsItem = new LibSettingsListItem(&Entry);
				OutputList->AddTail((ListItem *)LibSettingsItem);
				AddedItems = TRUE;
			}
		}

		GetEntryFinish(&GetEntryFile);
	}

	return AddedItems;
}


/********************************************************************************************

>	static BOOL LibSettings::GetNextLine(CCDiskFile *TheFile, String_256 *Line)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/1/96
	Inputs:		TheFile	- Open and seeked lex/disk file
	Outputs:	Line	- Will contain the next line (if we return TRUE)
	Returns:	TRUE if this was successful and a line was found and placed in Line.

	Purpose:	Output the next line of the file passed in. Return FALSE if problems.

	Notes:		If we hit a section title, we also return FALSE

********************************************************************************************/

BOOL LibSettings::GetNextLine(CCDiskFile *TheFile, String_256 *Line)
{
	BOOL FoundLine = FALSE;
	BOOL ok = TRUE;

	TRY
	{
		String_256 TokenBuffer;
		LexTokenType TokType = TOKEN_LINE;

		while(TokType != TOKEN_EOF && !FoundLine && TheFile->GetLineToken())
		{
			TokType = TheFile->GetTokenType();
			if(TokType == TOKEN_LINE)
			{
				TokenBuffer = TheFile->GetTokenBuf();

				// Check if we've hit a new section name, if so check it and stop the output...
				String_256 PossibleSectionName;				
				if(IsThisANewSection(&TokenBuffer, &PossibleSectionName))
					return FALSE;

				*Line = TokenBuffer;
				FoundLine = TRUE;
			}
		}
	}

	// See if there was a file io error
	CATCH(CFileException, e)
	{
		// Something rather unhelpful has taken place...
		ok = FALSE;

		// Report the error if no one else did
		if (Error::GetErrorNumber() != _R(IDN_USER_CANCELLED))
			InformError();
		else
			Error::ClearError();	// otherwise remove the error so it won't get reported
	}
	END_CATCH

	return (FoundLine && ok);
}

/********************************************************************************************

>	static BOOL LibSettings::SeekToSection(CCDiskFile *TheFile, String_256 *Section)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/1/96
	Inputs:		TheFile	- Open lex/disk file (preferably not seeked half way through)
				Section	- The section we're a'looking for
	Returns:	TRUE if this was successful and we're now seeked to the EOL after the section
				name
	Purpose:	Hunt down our section in the grm file

********************************************************************************************/

BOOL LibSettings::SeekToSection(CCDiskFile *TheFile, String_256 *Section)
{
	TRY
	{
		String_256 TokenBuffer;
		LexTokenType TokType = TOKEN_LINE;

		while(TokType != TOKEN_EOF && TheFile->GetLineToken())
		{
			TokType = TheFile->GetTokenType();
			if(TokType == TOKEN_LINE)
			{
				TokenBuffer = TheFile->GetTokenBuf();

				// Check if we've hit a new section name, if so check it and stop the output...
				String_256 PossibleSectionName;				
				if(IsThisANewSection(&TokenBuffer, &PossibleSectionName))
				{
					if(PossibleSectionName == *Section)
						return TRUE;
				}
			}
		}
	}

	// See if there was a file io error
	CATCH(CFileException, e)
	{
		// Report the error if no one else did
		if (Error::GetErrorNumber() != _R(IDN_USER_CANCELLED))
			InformError();
		else
			Error::ClearError();	// otherwise remove the error so it won't get reported
	}
	END_CATCH

	return FALSE;
}
