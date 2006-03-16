// $Id: sgscanf.h 662 2006-03-14 21:31:49Z alex $
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
// Super Gallery Library File class

#ifndef INC_SGSCANF
#define INC_SGSCANF

#include "thumb.h"

class PathName;
class SuperGallery;
class LibraryGallery;
class Library;


/***********************************************************************************************

>	class LibraryFile : public ListItem

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/2/95
	Purpose:	This is the main super gallery library creation class...
				Creating one of these will give your gallery a set of new sub-lib groups
				containing DisplayItems for the entries in the sub-group.
	SeeAlso:	Library; LibraryFileGroup;

***********************************************************************************************/

class LibraryFile : public ListItem
{
	CC_DECLARE_DYNCREATE(LibraryFile)

public:		// Construction/destruction
	LibraryFile();
	~LibraryFile();


public:		// public interface

	// Ask a newly created LibraryFile to scan and create Gallery groups
	// Returns number of groups created, or 0 in case of error
	// If the main or default subindex files don't exits we give the option
	// of generating a default sublib index to the user.
	INT32 Init(SuperGallery *ParentGal, PathName *Path, SGLibType Type, BOOL Updated = FALSE, BOOL DoScroll = TRUE);

	// Add a specific index to a newly created LibraryFile (This can be called instead
	// of the above Init call, which is really designed to hunt out it's own index files) 
	BOOL AddSpecificIndex(SuperGallery *ParentGal, PathName *Path, SGLibType Type, StringBase *Description = NULL, UINT32 bModified = 0, 
		BOOL bIsWebLib = FALSE);

	// Find the list of Libraries found by Init
	inline List *GetLibraryList(void);

	// Kill leading spaces from a string
	static BOOL KillLeadingSpaces(StringBase *Str);

	// Returns the pathname of this library, as passed in to the Init method
	PathName *GetLibraryPathName(void);

	// Returns the type of this library, as passed into the Init method
	SGLibType GetLibraryType(void);

	// Generate a default index for a given directory
	static BOOL GenerateDefaultIndex(PathName *IndexFile, PathName *LibDirPath,
								String_64 *Author, SGLibType Type, BOOL Thumbnails);

	// Check directory for subindex files. If we find any, add them to the gal.
	// If we don't, give the option to generate a subindex if the type can cope with it.
	BOOL CheckForSubIndexes(SuperGallery *ParentGal, PathName *Path, SGLibType Type, BOOL Updated = FALSE);

	// Get pathname of default sublib index and find out whether we can generate indexes, etc
	static BOOL GetSubIndexDetails(LibraryGallery *LG, String_256 *DefaultIndex, String_256 *IndexDesc, BOOL *CanGenerate);

	// Remove XaraInfo's and trailing backslashes from a string
	static BOOL TidyUpSubPath(String_256 *Str);

	// Count number of valid lines in a file
	static INT32 CountLines(PathName *IndexFilePath);

	// Actually do the scroll / redraw...
	void DoScrollRedraw(void);

protected:

	// Init the scroll/redraw system
	void InitScrollRedrawSystem(void);

	// Add a new folder to the scroll/redraw system
	void AddNewFolderToScrollRedrawSystem(Library *NewFolder);
	
protected:
	PathName MyPath;						// The pathname of this library file
	SGLibType MyType;						// The type of library we represent

public:
	List Libraries;							// A list of all contained Libraries
	SuperGallery *ParentGallery;			// A pointer to the gallery who owns us
	LibraryGallery *ParentLibraryGallery;	// A pointer to the library gallery who owns us (if either
											// of these are NULL, we're in deep water...)
	static Library *FirstLibraryAdded;		// Two statics for redraw/scroll purposes...
	static Library *LastLibraryAdded;
};



/***********************************************************************************************

>	inline List *LibraryFile::GetLibraryList(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/2/95
	Purpose:	Returns a pointer to the LibraryFile's list of available libraries.
				You should treat this list as READ ONLY.

***********************************************************************************************/

List *LibraryFile::GetLibraryList(void)
{
	return(&Libraries);
}


#endif
