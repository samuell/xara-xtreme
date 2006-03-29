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
#include "pathname.h"

#include "errors.h"
//#include "mario.h"
//#include "hotlink.h"
//#include "unicdman.h"

//#include "resimmap.h"	

#define MAXDRIVE 3
#define MAXNAME 8
#define MAXNETNAME 256
#define MAXEXT 16

#if defined(__WXMSW__)
	#define SEPARATOR 		_T('\\')
#elif defined(__WXGTK__)
	#define SEPARATOR 		_T('/')
#elif defined(__WXMAC__)
	#define SEPARATOR 		_T('/')
#else
    #error "You will need to setup the path separator for your platform"
#endif

#define FULLSTOP		'.'
#define COLON			':'
#define END_OF_PATH		'\0'
#define END_OF_STRING	'\0'

#define SEPARATOR_SYM 	"\\"
#define FULLSTOP_SYM	"."
#define COLON_SYM		":"
#define END_OF_PATH_SYM	"\0"
#define NET_DRIVE_SYM	"\\\\"

DECLARE_SOURCE("$Revision: 662 $");

extern void AbbreviateName(LPTSTR lpszCanon, INT32 cchMax, BOOL bAtLeastName);


// Forward Declarations

BOOL getDrive(const TCHAR**, TCHAR* drive);
BOOL getNetDrive(const TCHAR**, TCHAR* drive, BOOL *BadCharacter);
BOOL getIdentifier(const TCHAR**, TCHAR* identifier, UINT32 MaxSize, BOOL *BadCharacter);
BOOL getExtension(const TCHAR**, TCHAR* extension, UINT32 MaxSize, const TCHAR* );
BOOL getParentDirectory(const TCHAR **fn, TCHAR *directory, UINT32 MaxSize, const TCHAR * StartOfPath);
BOOL IsReservedChar(const TCHAR **fn);
BOOL IsDeviceName(const String_256& path);


/**********************************************************************************************

>	PathName::PathName()

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/5/93
	Inputs:		None
	Outputs:	None
	Returns:	None
	Purpose:	Default PathName class constructor
	Errors:		None

**********************************************************************************************/

PathName::PathName()
  : PathNameValid(FALSE)
{
	// Empty.
}

/**********************************************************************************************

>	PathName::PathName(const PathName& newPath)

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/5/93
	Inputs:		newPath
	Outputs:	None
	Returns:	None
	Purpose:	Copy Constructor for the PathName class
	Errors:		None

**********************************************************************************************/

PathName::PathName(const PathName& other)
  :	PathNameValid(other.PathNameValid),
	location(other.location),
	filename(other.filename),
	filetype(other.filetype),
	drivename(other.drivename)
{
	// Empty.
}

/**********************************************************************************************

>	PathName::PathName(const String_256&)

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/5/93
	Inputs:		String representing the path.
	Outputs:	None
	Returns:	None
	Purpose:	Is to parse the path string passed in as a parameter.
				Also, it sets up the internal variables for location, filename and filetype.
	Errors:		If a problem is found it will call set error with _R(IDE_PATH_ERROR) but will
				not call InformError as this is a contructor!

**********************************************************************************************/

PathName::PathName(const String_256& path)
{
	// Set up the protected variables at our disposal
	PathNameValid = FALSE;

	// Check if supplied path is valid or not, if so then set up our internal variables
	// Use the internal form of the checking code which returns the values back to us,
	// blanked if a problem has happened.
	BOOL ok;
	String_256 tempFilename;			//temporary string to hold the filename
	String_256 tempLocation;			//temporary string to hold the location
	String_256 tempFiletype;			//temporary string to hold the filetype
	String_256 tempDrivename;			//temporary string to hold the drive name

	// Use the internal form of the routine which returns us information which
	// we do not want and so just throw away  
	ok = IsValidAndReturnInfo(path, tempFilename, tempLocation, tempFiletype, tempDrivename);

	// We could return the result to the user but we are a constructor and so cannot.
	// but we will set up our variables to be the returned variables 
	filename = tempFilename;
	location = tempLocation;
	filetype = tempFiletype;
	drivename = tempDrivename;
	PathNameValid = ok;
}

/**********************************************************************************************

>	const String_256 PathName::GetPath(BOOL TrailingSlash=FALSE) const

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>	(modified for new Strings by JCF 13/8/93)
								(modified by Neville 25/8/94 to cope with null file types)
								(modified by Richard 15/2/96 for TrailingSlash biz)
	Created:	12/5/93
	Inputs:		TrailingSlash	- If TRUE, returned string will have a trailing '\' assigned to it...
	Outputs:	None
	Returns:	A Path = Location + filename + filetype
	Purpose:	To reconstruct path name and return it.
	Errors:		None

**********************************************************************************************/

const String_256 PathName::GetPath(BOOL TrailingSlash) const
{            
	String_256 path;
	path.Empty();          
	
	// Concatenation of Location, FileName and FileType
	path += location;   
	path += filename;

	// Changed by Neville 25/8/94 so that if no filetype is present then a full stop
	// is not added to the filename.
	if ( !filetype.IsEmpty() )
	{
		path += String( wxT(FULLSTOP_SYM) );
		path += filetype;
	}

	// Though it might not make much sense for filenames, it's invaluable for directory paths in the
	// library gallery system...
	if(TrailingSlash)
	{
		// Add a trailing slash if it hasn't got one (usually, roots have 'em, but sub-dirs don't)
		INT32 ByteLength = path.Length();
PORTNOTE("other","Removed UnicodeManager usage")
		if( path[ByteLength-1]!=SEPARATOR )
#ifndef EXCLUDE_FROM_XARALX
			 //|| ((path[ByteLength-1]==SEPARATOR) && UnicodeManager::IsDBCSLeadByte(path[ByteLength-2])))
#endif
			path += SEPARATOR;
	}
	
	return path;
}

/**********************************************************************************************

>	const String_256 PathName::GetWebAddress() const

	Author: 	Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/4/97
	Inputs:		-
	Outputs:	-
	Returns:	The path name as a URL
	Purpose:	This function returns the path name in URL form.

				The resulting URL will be recognised by browsers
				as pointing to a local file.

  				The way we do this is:
				a. Convert all backslashes to forward slashes
				b. Add "file:///" to the start to the string

				Remarkably, this works both with UNC pathnames and ordinary ones. So:

				\\earth\progtemp\imagemaps/test.htm
				becomes
				file://///earth/progtemp/imagemaps/test.htm

				d:\imagemaps\test.htm
				becomes
				file:///d:/imagemaps/test.htm

				bugfix #10747 (Marc 20/9/04) - 	some browsers (notably Opera) choked on the | character 
				when used to replace the colon in a path, so this behaviour was removed. Also, the host
				was not correctly specified, so it is now specified as the local machine with "file:///".

				Notes: it was not possible to rewrite a UNC as "file://<host>/<path>" because this fails in 
				Mozilla & Netscape. So we are left with the "file://///<host>/<path>" solution which 
				unfortunately does not work in Opera but at least works in IE,Netscape & Mozilla!

	Errors:		None

**********************************************************************************************/

String_256 PathName::GetWebAddress() const
{            
	//First let's get this path as a string
	String_256 strPath=GetPath(FALSE);

	//Convert all the backslashes to forward slashes
	strPath.SwapChar('\\', '/');

	//Add "file:///" to the start of the sting
	String_256 strToReturn( wxT("file:///") );

	strToReturn+=strPath;

	//And return
	return strToReturn;

}


/**********************************************************************************************

>	const String_256 PathName::GetTruncatedPath(UINT32 MaxSize = 0) const

	Author: 	Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/3/95
	Inputs:		MaxSize determines the maximum length of the string required.
				Defaults to 0 = return full path
	Outputs:	None
	Returns:	A Path = Location + filename + filetype which may be truncated to the specified
				size or less so that it can be displayed in the space required.  
	Purpose:	Display fields which require a pathname to be shown in a fixed sized space will
				need the path name truncated if it is too long to fit into the space. This
				routine should do this in the standard Microsoft way.
				We use an OS/MFC routine, AbbreviateName, to do the work for for us.
	Errors:		None

**********************************************************************************************/

const String_256 PathName::GetTruncatedPath(UINT32 MaxSize) const
{            
	// Use the normal routine to get the pathname
	String_256 path;
	path = GetPath();

	// Shorten the filename down a bit.
	String_256 ShortPath;
	ShortPath.Empty();
	TruncateName(path, &ShortPath, (INT32)MaxSize);
	return ShortPath;
}




/********************************************************************************************

>	void PathName::TruncateName(String_256& FullName, String_256* pShortName, UINT32 MaxSize)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/5/95
	Inputs:		FullName - The Full Path name of the file we need to shrink
				MaxSize - The maximum number of characters that the path name is allowed to
				take up. A value of zero means infinite (ie, the whole path will be returned)
	Outputs:	pShortName - the shortened version of the file name
	Purpose:	This function shortens a file name, replacing the middle sections of the
				name with ... if needed. eg if the input path is
				m:\materials\graphics\artwork\myfile.art
				Resulting Filename                            MaxSize
				myfile.art										0-16
				c:\...\myfile.art                   		   17-24
				c:\...\artwork\myfile.art		   			   25-33
				c:\...\graphics\artwork\myfile.art		   	   34-etc
				if the filename is longer than MaxSize then the filename will still be
				returned. In other words, it is possible for a string longer than MaxSize
				to be returned, so if this is important, check the length after you get
				your string back.

				NOTE: Made DBCS compatiable by Peter 9/8/96

********************************************************************************************/

void PathName::TruncateName(String_256& FullName, String_256* pShortName, INT32 MaxSize) const
{
	// If maxsize is zero, then give back the whole path name
	if (MaxSize==0)
	{
		*pShortName = FullName;
		return;
	}

	// Find out how long the full path is
	INT32 FullLength = FullName.Length();

	// if we can fit it into MaxSize, then return it
	if (FullLength<=MaxSize)
	{
		*pShortName = FullName;
		return;
	}

	// OK, we will need to compress it, so look for the actual filename part of the path
	// the filename goes from the end of the string to the last \ in the string
	const TCHAR* pFullName = (const TCHAR*)FullName;
	const TCHAR* pLastSlash = _tcsrchr(pFullName, SEPARATOR);

	// if we fell off the end of the string, we only had a filename, so return it
	if (pLastSlash == NULL)
	{
		*pShortName = FullName;
		return;
	}

	// Find out the length of the filename
	INT32 FileNameLen = FullLength - (pLastSlash - pFullName);
	INT32 FileNameStart = pLastSlash - pFullName;

	// Now, starting from the front of the string, we have to try and add
	// in more and more of the path, until it will fit no more.
	// We will skip the first 2 chars as they will be either c: or \\.
	const TCHAR* pCurrent = pFullName;
	pCurrent = _tcsinc(pCurrent);
	pCurrent = _tcsinc(pCurrent);		// Now pointing at the third character

	// see if this is a UNC filename
	if (*pCurrent != SEPARATOR)
	{
		// yes, it's UNC, so walk though the server name
		do
		{
			pCurrent = _tcsinc(pCurrent);
		} while (*pCurrent != SEPARATOR);
	}

	// Up to here is the volume name really
	INT32 VolumeNameLen = pCurrent-pFullName;

	// and we need at least the first directory
	do
	{
		pCurrent = _tcsinc(pCurrent);
	} while (*pCurrent != SEPARATOR);

	// See if what we have will fit
	INT32 CurrentLength = pCurrent - pFullName;
	if ((CurrentLength+FileNameLen+4)>MaxSize)
	{
		// maybe we could have just volume name\...\filename
		if ((VolumeNameLen+4+FileNameLen)>MaxSize)
		{
			// Nope, it won't fit, so just return the filename
			*pShortName = String_256(pFullName+FileNameStart+1);
			return;
		}
		else
		{
			// Yep, we can fit it in...
			// Build the resulting short path
			FullName.Left(pShortName, VolumeNameLen);
			*pShortName+=TEXT(SEPARATOR_SYM "...");
			*pShortName+=pFullName+FileNameStart;
			return;
		}
	}

	// Since we can fit the first directory, keep it as part of the volume name
	VolumeNameLen = pCurrent-pFullName;

	// put i back to the start of the filename and start working backwards
	pCurrent = pFullName + FileNameStart;
	INT32 DirectoryStart = FileNameStart;
	while ((VolumeNameLen+4+(FullLength-(pCurrent-pFullName))) < MaxSize)
	{
		DirectoryStart = pCurrent-pFullName;
		do
		{
			pCurrent = _tcsdec(pFullName, pCurrent);
		} while ((*pCurrent!=SEPARATOR) && ((pCurrent-pFullName)>VolumeNameLen));
	}

	// We got all the way back to the start of the volume name
	if (DirectoryStart==VolumeNameLen)
	{
		*pShortName = FullName;
		return;
	}

	// Build the resulting short path
	FullName.Left(pShortName, VolumeNameLen);
	*pShortName+=TEXT(SEPARATOR_SYM "...");
	*pShortName+=pFullName+DirectoryStart;
}



/**********************************************************************************************

>	const String_256 PathName::GetTruncatedLocation(UINT32 MaxSize = 0) const

	Author: 	Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/3/95
	Inputs:		MaxSize determines the maximum length of the string required.
				Defaults to 0 = return full path
	Outputs:	None
	Returns:	A Path = Location, which may be truncated to the specified size or less so that
				it can be displayed in the space required.  
	Purpose:	Display fields which require a location to be shown in a fixed sized space will
				need the location truncated if it is too long to fit into the space. This
				routine should do this in the standard Microsoft way.
				We use an OS/MFC routine, AbbreviateName, to do the work for for us.
	Errors:		None

**********************************************************************************************/

const String_256 PathName::GetTruncatedLocation(UINT32 MaxSize) const
{            
	// Use the normal routine to get the pathname
	static String_256 path;
	path = GetLocation(FALSE);

	// Shorten the filename down a bit.
	String_256 ShortPath;
	TruncateName(path, &ShortPath, (INT32)MaxSize);
	return ShortPath;
}

/**********************************************************************************************

>	const String_256 PathName::GetLocation(BOOL KeepSep = TRUE) const

	Author: 	Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/3/94
	Inputs:		KeepSep - if TRUE, it leaves the terminating backslash on the end of
				the location name; if FALSE, it removes it.	 Defaults to TRUE.
	Returns:	The location.
	Purpose:	To obtain the location from a pathname.

**********************************************************************************************/

const String_256 PathName::GetLocation(BOOL KeepSep) const
{            
	String_256 path;

	path = location;
	if (!KeepSep)
	{
		// Get the last character and remove it if it is a backslash.
		INT32 Length = path.Length() - 1;
		TCHAR *pLocation = (TCHAR *) path;
PORTNOTE("other","Removed UnicodeManager usage")
		if( pLocation[Length] == SEPARATOR )
#ifndef EXCLUDE_FROM_XARALX
 	//&& !UnicodeManager::IsDBCSLeadByte(pLocation[Length-1]))
#endif
			pLocation[Length] = 0;
	}

	return path;
}


/**********************************************************************************************

>	const String_256 PathName::GetFileName(BOOL FullName=TRUE) const

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com> (modified by Neville 25/8/94 to cope with null file types)
	Created:	12/5/93
	Inputs:		FullName TRUE means that we are interested in the name plus extension
						 FALSE means that we are showing the user and so the extension is not
						 important, especially on Windows 95 (Chicago). 
	Outputs:	None
	Returns:	FileName and FileType
	Purpose:	To Construct the FileName out of the FileName and FileType
	Errors:		None

**********************************************************************************************/

const String_256 PathName::GetFileName(BOOL FullName) const
{            
	String_256 fname;
	fname.Empty();
	
	//Concatenation of FileName and FileType
	fname += filename;

	// Changed by Neville 25/8/94 so that if no filetype is present then a full stop
	// is not added to the filename.
	// FullName parameter added	21/3/95 so that can just specify the filename itself,
	// without the extension.
	// Use IsWin32c() to test for Windows 95/Chicago 
	if ( !filetype.IsEmpty() && FullName)
	{
		fname += String( wxT(FULLSTOP_SYM) );
		fname += filetype;
	}

	return fname;
}

/**********************************************************************************************

>	const String_256 PathName::GetType() const

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/5/93
	Inputs:		None
	Outputs:	None
	Returns:	FileType
	Purpose:	Extracts the file type.
	Errors:		None

**********************************************************************************************/

const String_256 PathName::GetType() const
{
	return filetype;
}



/********************************************************************************************

>	BOOL PathName::SetType(const String_256& NewType)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/2/95
	Inputs:		NewType - The new extension for the path name
	Returns:	TRUE if it changed the extension OK
	Purpose:	Changes the Pathnames type (extension). 

********************************************************************************************/

BOOL PathName::SetType(const String_256& NewType)
{
	// set the filetype to the new one
	filetype = NewType;

	// all worked
	return TRUE;
}


/********************************************************************************************

>	BOOL PathName::SetFileName(const String_256& NewFileName)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/06/96
	Inputs:		NewFileName - The new filename for the path name
	Returns:	TRUE if NewFileName is a valid file name consisting solely of the name
				of a file with no path delimiters, drive specification, etc.
				FALSE otherwise
	Purpose:	Allow alteration of the filename component of the path

********************************************************************************************/

BOOL PathName::SetFileName(const String_256& NewFileName)
{
	const TCHAR* fn = NewFileName;
	String_256 newfilename;
	BOOL BadCharacter = FALSE;
	BOOL ok = FALSE;
	ok = getIdentifier(&fn, newfilename, NewFileName.Length(), &BadCharacter);
	if (!ok || BadCharacter || newfilename.Length() != NewFileName.Length())
	{
		// don't allow any bad characters & ensure given NewFileName is the whole thing
		return FALSE;
	}

	String_32 temp;
	ok = getExtension(&fn, temp, MAXEXT, fn);
	if (ok)
	{
		// an extnesion was found - don't allow it
		return FALSE;
	}

	// set the filename to the new one
	filename = newfilename;

	return TRUE;
}

/**********************************************************************************************

>	BOOL PathName::SetFileNameAndType(const String_256& NewFileName)

	Author: 	Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/12/96
	Inputs:		NewFileName - String representing a filename and type
	Outputs:	None
	Returns:	TRUE if the filename is parsed successfully or FALSE otherwise
	Purpose:	Parses a string representing a filename type and sets up the class
	 			variables filetype and filename if the path is valid
	 			otherwise it leaves them sets them to null strings.
				Always sets the filename and filetype regardless of whether there is a file
				type present or not. SetFileName will not set the name if there is a type present.
	SeeAlso:	SetFileName();
	Errors:		None

**********************************************************************************************/

BOOL PathName::SetFileNameAndType(const String_256& NewFileName)
{
	// Check if supplied path is valid or not, if so then set up our internal variables
	// Use the internal form of the checking code which returns the values back to us,
	// blanked and with an error flag set if a problem has happened.
	BOOL ok;
	String_256 tempFilename;			//temporary string to hold the filename
	String_256 tempLocation;			//temporary string to hold the location
	String_256 tempFiletype;			//temporary string to hold the filetype
	String_256 tempDrivename;			//temporary string to hold the drive name

	// Use the internal form of the routine which returns us information which
	// we do not want and so just throw away  
	ok = IsValidAndReturnInfo(NewFileName, tempFilename, tempLocation, tempFiletype, tempDrivename);

	// Set up our class variables to be the returned variables 
	// Only set the ones that we are interested in though.
	filename = tempFilename;
	filetype = tempFiletype;

	// Set up our class variable which holds this validation information
	PathNameValid = ok;

	// return the result of the parsing of the pathname to the caller.  
	return ok;
}
 
/**********************************************************************************************

>	BOOL PathName::SetPathName(const String_256&, BOOL SetErrors)

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/5/93
	Inputs:		String representing a pathname and SetErrors - enables us to ignore errors.
				This should ONLY be false when we call this routine with an empty path
				(thereby ensuring that any subsequent calls to this routine (that fail) will
				result in an empty (but valid!) path).  For an example of this, look at
				SGLibDisplayItem::GetFileName(PathName *Result).
	Outputs:	None
	Returns:	TRUE if the pathname is parsed successfully or FALSE otherwise
	Purpose:	Parses a string representing a path	and sets up the class
	 			variables filetype, filename and location if the path is valid
	 			otherwise it sets them to null strings. 
	Errors:		None

**********************************************************************************************/

BOOL PathName::SetPathName(const String_256& path, BOOL SetErrors)
{
	// Check if supplied path is valid or not, if so then set up our internal variables
	// Use the internal form of the checking code which returns the values back to us,
	// blanked and with an error flag set if a problem has happened.
	BOOL ok;
	String_256 tempFilename;			//temporary string to hold the filename
	String_256 tempLocation;			//temporary string to hold the location
	String_256 tempFiletype;			//temporary string to hold the filetype
	String_256 tempDrivename;			//temporary string to hold the drive name

	// Use the internal form of the routine which returns us information which
	// we do not want and so just throw away  
	ok = IsValidAndReturnInfo(path, tempFilename, tempLocation, tempFiletype, tempDrivename,
							  SetErrors);

	// Set up our class variables to be the returned variables 
	filename = tempFilename;
	location = tempLocation;
	filetype = tempFiletype;
	drivename = tempDrivename;

	// Set up our class variable which holds this validation information
	PathNameValid = ok;

	// return the result of the parsing of the pathname to the caller.  
	return ok;
}

/**********************************************************************************************

>	BOOL PathName::IsValid(const String_256&) const

	Author: 	Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/5/93
	Inputs:		String representing a pathname
	Outputs:	None
	Returns:	TRUE if the pathname is parsed successfully or FALSE otherwise
	Purpose:	Parses a string representing a path	and sets up the class
	 			variables filetype, filename and location if the path is valid.
				Cannot do the usual 8.3 checking of things like filenames as this is no longer
				valid given Chicargo and NT non-FAT filing systems. 
	Errors:		If it fails then it will call set error with the error found.

**********************************************************************************************/
/*
Technical notes:
We used to do explicit checking of 8.3 filenames, correct letters being used in all filenames
and directory names but unfortunately we did not cope with things like long directory names,
spaces in filenames and directory names, relative paths. Unfortunately, there is no nice
operating system routine to do this checking for us so all we can do is some basic checking of
validity like a null path being specified, bad names such as d:\ and \\blobby. A lot of the
time the data being passed to us has been validated by the Common Dialogs for open and save.  

**********************************************************************************************/

BOOL PathName::IsValid(const String_256& path) const
{
	BOOL ok;
	String_256 tempFilename;			//temporary string to hold the filename
	String_256 tempLocation;			//temporary string to hold the location
	String_256 tempFiletype;			//temporary string to hold the filetype
	String_256 tempDrivename;			//temporary string to hold the drive name

	// Use the internal form of the routine which returns us information which
	// we do not want and so just throw away  
	ok = IsValidAndReturnInfo(path, tempFilename, tempLocation, tempFiletype, tempDrivename);

	// Set up our class variable which holds this validation information
	//PathNameValid = ok; // don't be so stupid

	// return result to the user
	return ok;
}

/**********************************************************************************************

>	BOOL PathName::IsValid() const

	Author: 	Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/3/95
	Inputs:		None
	Outputs:	None
	Returns:	TRUE if the present pathname has been parsed successfully or FALSE otherwise
	Purpose:	Says whether the pathname currently in the class has been parsed correctly.
				This should only be used if the non-blank contructors have been called and more
				specifically the contructor which takes a pathname as a parameter has been used.
				This function then returns the result of the validation that happens in the 
				constructor as of course constructors cannot return results!
	SeeAlso:	PathName::IsValid(const String_256& path);	
	Errors:		If it fails then it will call set error with the error found.

**********************************************************************************************/

BOOL PathName::IsValid() const
{
	// Just return the result that we have found earlier to the caller
	// If the wrong constructor has been called then should be automatically FALSE.
	return PathNameValid;
}




/**********************************************************************************************

>	virtual BOOL PathName::IsValidAndReturnInfo(const String_256& ConstPath, String_256& tempFilename,
		String_256& tempLocation, String_256& tempFiletype,	String_256& tempDrivename, BOOL SetErrors = TRUE) const

	Author: 	Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com> (with updated function protocol comments by Richard !)
	Created:	12/5/93

	Inputs:		ConstPath		- String representing the pathname to check
				SetErrors		- Flag to say whether we should set errors or not (defaults to TRUE)

	Outputs:	tempFilename	- String representing the filename
				tempLocation	- String representing the location
				tempFiletype	- String representing the filetype
				tempDrivename	- String representing the drive name
	
	Returns:	TRUE if the pathname is parsed successfully or FALSE otherwise
	Purpose:	Parses a string representing a path	and returns the filetype, filename
				and location to the caller if the path is valid.
				Cannot do the usual 8.3 checking of things like filenames as this is no longer
				valid given Chicago and NT non-FAT filing systems.
				Routine called by IsValid to actually go and do the work involved with
				checking the pathname. Done this way so that internal calls do not have to
				parse the data again to get at the variables.  
	Errors:		If it fails then it will call set error with the error found.
				This can now be supressed if required. 

**********************************************************************************************/

BOOL PathName::IsValidAndReturnInfo(const String_256& ConstPath,
									String_256& tempFilename,
									String_256& tempLocation,
									String_256& tempFiletype,
									String_256& tempDrivename,
									BOOL SetErrors) const
{
	// First ensure the strings passed in are blanked 
	tempFilename.Empty();
	tempLocation.Empty();
	tempFiletype.Empty();
	tempDrivename.Empty();
	
	// Get a non const version, as this function tends to party a bit too much
	String_256 path = ConstPath;

	// Then check for an empty path being passed to us
	// and also that a device name is not being specified 
	if ( !path.IsEmpty() && !IsDeviceName(path))
	{
		BOOL ok 		= TRUE;			// General flag for returning results
		BOOL driveFound = FALSE;		// Flag for valid drive found
		BOOL fnameFound = FALSE;		// Flag for valid filename found
		BOOL BadCharacter = FALSE;		// Flag for bad character found

		String_256 temp;				// temporary string buffer
		String_256 tempDrive;			// temporary string to hold drive

		const TCHAR *fn;				// File Name Pointer 
    
    	fn = path;
		temp.Empty();

		// First check if there is a valid drive name or net drive name present
		// If the first character of the path is an alpha then assume drive and check if valid
		// and so of the form D:/
		// Otherwise, check for a UNC (Universal naming convention) form of drive of the form
		// \\Deepthought 
	    if (*fn == SEPARATOR)			
			driveFound = getNetDrive(&fn, temp, &BadCharacter);	//Parse as UNC or network drive
		else 
   			driveFound = getDrive(&fn, temp);	// Parse the Drive 
		
		// If parsed that ok then add the drive to our location store
		// otherwise reset back to the entire path string and look for a relative style path 
		if (driveFound)
		{
			tempLocation = temp;		// Add drive to location string
			tempDrivename = temp;		// Note drive name found for future use
		}
		else
	    	fn = path;					// Start again at first character

		// If no drive or net drive (UNC) present then this may be a relative path so do not
		// assume that everything fails because of this.
		// Now move along the path to see if we have a correct filename with possible multiple
		// directories being specified before it.
		while (ok && !fnameFound) 
		{
			temp.Empty();						// Clear temporary
			// The size of the file/directory name is really max string size - current size
			// of the location string i.e. the space remaining in our fixed length strings.
			UINT32 MaxSize = temp.MaxLength() - tempLocation.Length(); 
			ok = getIdentifier(&fn, temp, MaxSize, &BadCharacter);	//Get the Identifier
			if (ok)
			{
				if (*fn == SEPARATOR)       // If it is a directory location\path 
				{
					tempLocation += temp;   // add to file location\path temp buffer
					fn++;					// and move to after separator
				}
				else                        // If it is a filename 
				{
					tempFilename = temp;    // add to file name temp buffer
					fnameFound = TRUE;		// and flag found to terminate search
				}
			}
		}

		// If everything went well then we should now have a filename with an optional
  		// drive/net drive and optional directory list. Now check for a file extension
		// on the end of the filename.
		// fn is pointing at the end of the found filename if everything ok 

		if (ok)			                    		// If ok so far 
		{
			// Note current position i.e. end of supplied path
			// We will scan from the end backwards to locate the extension.
//			const TCHAR* CurrentPos = fn;
			const TCHAR* StartOfPath = path;
				
		    if (*fn == END_OF_PATH || *fn == END_OF_STRING)
			{
				fn = _tcsdec(path, fn);		// move to last valid character
				// Get File Extension
				// If we don't find a vlaid extension then do not complain as this should
				// not be a problem.
				BOOL ExtOk = getExtension(&fn, temp, MAXEXT, StartOfPath);

				if (ExtOk)
				{
					// We parsed it ok, so make a note of it.
					tempFiletype = temp;			// FileType = File Extension
					// tempFileName should already be = leafname found
					// tempLocation should already be = directory path found
					// And remove it from the filename
					UINT32 LenFilename = tempFilename.Length();
					UINT32 LenExtensionName = tempFiletype.Length();
					// Strip the extension name plus the full stop from the filename.
					if (LenFilename > LenExtensionName)
					{
						temp.Empty();
						tempFilename.Left(&temp, (LenFilename - LenExtensionName - 1));
						tempFilename = temp;
					}
				}
				else
				{
					// We have not found what we consider to be a valid extension so say
					// we have a blank filetype.
					tempFiletype.Empty();				// FileType = File Extension
					// The filename is the valid leafname that has been found, possibly
					// including a dodgy extension name that we did not like at all.
				}
			}
			else
			{
				tempFiletype.Empty();				// FileType = File Extension = none
			}
		}

		// Ok is the flag that says whether we have found a valid filename or not so return
		// this to the caller.
		// Now we also have a flag BadCharacter. If set we have found what we consider to be
		// an illegal/reserved character in the filename and so should return false but may
		// have parsed the filename ok so that this can be shown to the user.
//		if (BadCharacter)
//		{
//			// If set errors has been specified then set up the required error
//			if (SetErrors)
//				Error::SetError(_R(IDE_PATH_ERROR), 0);	// Set up the correct error
//			return FALSE;
//		}
//		else
//			return ok;								// Return ok flag   
		
		// Always return the result, if we reach this point. Ignore bad characters as these
		// are ok under NT.
		if (!ok)
		{
			if (SetErrors)
			{
				Error::SetError(_R(IDE_PATH_ERROR), 0);
			}
		}
		return ok;
	}
	else
	{
		tempFiletype.Empty();					//Return blank fileType 
		tempFilename.Empty();					//Return blank fileName
		tempLocation.Empty();					//Return blank location

		// If set errors has been specified then set up the required error
		if (SetErrors)
			Error::SetError(_R(IDE_PATH_ERROR), 0);		// Set up the correct error
		return FALSE;							// Null path passed to us   
	}

	// If we reach here then things must have gone terribly wrong so return False
	return FALSE;
} 


/**********************************************************************************************

>	PathName& PathName::operator=(const PathName& newPath)

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/8/93
	Inputs:		newPath
	Outputs:	None
	Returns:	PathName
	Purpose:	Assignment operator for pathnames.
	Errors:		None

**********************************************************************************************/

PathName& PathName::operator=(const PathName& newPath)
{
	filename = newPath.filename;
	location = newPath.location;
	filetype = newPath.filetype;
	drivename = newPath.drivename;

	PathNameValid = newPath.PathNameValid;

	return *this;
}


/**********************************************************************************************
	Win16 Path String Parser Functions

**********************************************************************************************/

/**********************************************************************************************

>	BOOL getDrive(TCHAR **fn, TCHAR *drive)

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/5/93
	Inputs:		A pointer to the pointer to the path string
	Outputs:	drive - the parsed Drive Letter 
	Returns:	TRUE if parsed ok and false otherwise
	Purpose:	Parses the Drive location of file.
	Scope:		Private
	SeeAlso:	PathName();
	SeeAlso:	SetPath();
	Errors:		None

**********************************************************************************************/

BOOL getDrive(const TCHAR **fn, TCHAR *drive)
{
	UINT32 i = 0;	
	                   
	if  (String::IsAlpha(**fn))
	{
		drive[i++] = **fn;						// Parse Drive Letter
		(*fn)++;
		if (**fn == COLON)    					// Parse colon
		{
			drive[i++] = **fn;
			(*fn)++;
			if (**fn == SEPARATOR)              // Parse slash
	        {
				drive[i++] = **fn;
				(*fn)++;
	        }
	        else 
	        	return FALSE;                    // Else Syntax error
		}
		else 
			return FALSE;	
	}
	else 
		return FALSE; 
	
	drive[i++] = END_OF_STRING;

	return TRUE;
}

/**********************************************************************************************

>	BOOL getNetDrive(TCHAR **fn, TCHAR *drive, BOOL *BadCharacter)

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/9/93
	Inputs:		A pointer to the pointer to the path string
				A pointer to the current bad character flag state
	Outputs:	A network drive Letter
				A pointer to the potentially new bad character flag state 
	Returns:	TRUE if parsed ok FALSE otherwise
	Purpose:	Parses the network Drive location of file and returns whether this was
				accomplished or not.
				The network name will consist of the server name followed by a mount point name.
	Scope:		Private
	SeeAlso:	PathName();
	SeeAlso:	SetPath();
	Errors:		None

**********************************************************************************************/

BOOL getNetDrive(const TCHAR **fn, TCHAR *drive, BOOL *pBadCharacter)
{                                               
	// Used for storing either <ServerName> or <DirectoryName> 
	String_256 identifier;
	TCHAR* pId;
		
	UINT32 i = 0;
	                   
	if  (**fn == SEPARATOR)							// Parse '\\' bit
	{
		drive[i++] = **fn;

		(*fn)++;

		if (**fn == SEPARATOR) 					
		{

			drive[i++] = **fn;

			(*fn)++;						

			identifier.Empty();						
													
			// We have a valid network name start
			// Parse Network drive name(s)
			if ( getIdentifier(&(*fn), identifier, MAXNETNAME, pBadCharacter) )
			{
				pId = identifier;					// Copy identifier into drive

				while (*pId != END_OF_STRING)
				{
					drive[i++] = *pId;
					pId++;
				}

				(*fn)++;						

				identifier.Empty();

				// We have a valid network drive name and so parse the mount point name
				if ( getIdentifier(&(*fn), identifier, MAXNETNAME, pBadCharacter) )
				{
					pId = identifier;				// Copy identifier into drive

					while (*pId != END_OF_STRING)
					{
						drive[i++] = *pId;
						pId++;
					}

					(*fn)++;						
				
					drive[i++] = END_OF_STRING;

					// Everything has gone ok so now exit
					return TRUE;
				}
				else 
					return FALSE;
			}
			else 
				return FALSE;
		}
		else 
			return FALSE;	
	}
	else 
		return FALSE; 
}

// Forward Declaration

//BOOL IsSpecialChar(TCHAR **fn);

/**********************************************************************************************

>	BOOL getIdentifier(TCHAR **fn, TCHAR *identifier, UINT32 MaxSize, BOOL *BadCharacter)

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/5/93
	Inputs:		A pointer to the pointer to the path string
				MaxSize - the maximum size of a network drive name
				A pointer to the current bad character flag
	Outputs:	A file or directory identifier
				A pointer to a potentially new state of the bad character flag which signals
				a bad/reserved character was found in the string  
	Returns:	TRUE if parsed ok FALSE otherwise
	Purpose:	Parses file or directory identifier
	Scope:		Private
	SeeAlso:	PathName();
	SeeAlso:	SetPath();
	Errors:		None

**********************************************************************************************/

BOOL getIdentifier(const TCHAR **fn, TCHAR *identifier, UINT32 MaxSize, BOOL *pBadCharacter)
{	 
    UINT32 i = 0;

    // While not a Separator or not EOF or Maximum size
	// Used to check for (String::IsAlphaNumeric(**fn) || IsSpecialChar(fn))
	// to see if it was a valid character or not but this would not allow things like
	// spaces which are allowed under NT and Chicago. Reduce check to known problem
	// characters such as colons in the wrong place and '/'.
	// but must only flag this as an error and continue as we must have a filename at the
	// end so that a m4essage such as 'cannot open file: xxxxx.xx' can be shown by using
	// the GetFileName call.
	// Removed the FULLSTOP test as this was stopping directory names having full stops in

	while (	**fn != SEPARATOR &&		// If not a separator for next directory/filename
			**fn != END_OF_PATH &&		// and not terminator 
			i < (MaxSize))				// is less than maximum name size
	{ 
		// Check if this is an illegal character for the filing system and if so
		// make a note and continue.
		if ( IsReservedChar(fn) )
		{
			*pBadCharacter = TRUE;		// Flag bad character found
		}
					
		identifier[i++] = **fn;			// Parse the Identifier
		(*fn)++;
	}

	// if a valid identifier terminator found then return True
	// otherwise return False.
	// Do not stop on full stops as these are valid in  directory names.
	if (**fn == SEPARATOR ||
		**fn == END_OF_PATH)
	{	 
		if (**fn == SEPARATOR)
		{ 
			identifier[i++] = **fn;         // Parse the Separator 
   		}                   

		identifier[i++] = END_OF_STRING;   	// Set end of string 

   		return TRUE;
    }
    else 
    	return FALSE;
}

/**********************************************************************************************

>	BOOL getExtension(TCHAR **fn, TCHAR *ext, UINT32 MaxSize, const TCHAR * StartOfPath)

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/5/93
	Inputs:		A pointer to the pointer to the path string
				MaxSize - the maximum size of a network drive name
				StartOfPath - a pointer to the start of the path string
	Outputs:	A file extension 
	Returns:	TRUE if parsed ok FALSE otherwise
	Purpose:	Parses from the specified position, which should be the last valid character on
				the pathname, backwards until we hit either:-
				- a full stop, in which case we have a valid filename extension, which we return
				- a path separator, in which case there is no extension
				- the start of the pathname, in which case there is no extension. 
	Scope:		Private
	SeeAlso:	PathName(); IsValidAndReturnInfo(); SetPath();
	Errors:		None

**********************************************************************************************/

BOOL getExtension(const TCHAR **fn, TCHAR *ext, UINT32 MaxSize, const TCHAR * StartOfPath)
{
	UINT32 i = 0; 

	// Search from the specified string end backwards until we find either:-
	// - a full stop indicating a valid extension
	// - a path separator indicating no file extension is present
	// - the start of the file indicating no file extension is present
	while (	
			(**fn != SEPARATOR) &&				// is not at a path separating character
			(**fn != FULLSTOP) &&				// is not at a full stop character
			(*fn != StartOfPath) &&				// is not at the start of the path string
			(i < MaxSize)						// is not maximum size 
		  )
	{ 
		i++;
		(*fn)--;
	}
		
	if (**fn != FULLSTOP)						// if we are not at a full stop Then Syntax error
		return FALSE;
		
	// Make up the string that we are going to return
	// Do it this way as otherwise we would be adding the characters in backwards in the above
	// routine as we are scanning backwards!
	UINT32 position = 0;	// Start at first character in the string
	(*fn)++;			// Move to character after the full stop
	while (( i >= 0 ) && (**fn != END_OF_PATH))
	{
		ext[position++] = **fn;					// Add current character to File Extension
		(*fn)++;								// Move to next character
		i--;									// decrement extension letters count
	}

	ext[position++] = END_OF_STRING;			// Set end of string 
	
	return TRUE;
}

/**********************************************************************************************

>	BOOL getParentDirectory(TCHAR **fn, TCHAR *ext, UINT32 MaxSize)

	Author: 	Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/3/95
	Inputs:		A pointer to the pointer to the path string
				MaxSize - the maximum size of a network drive name
				StartOfPath - a pointer to the start of the path string
	Outputs:	The parent directory name 
	Returns:	TRUE if parsed ok FALSE otherwise
	Purpose:	Parses from the specified position, which should be something like the first valid
				character of the filename on the pathname, backwards until we hit either:-
				- a path separator, in which case we return the found name as a possible directory
				- the start of the pathname, in which case there is no directory name. 
	Scope:		Private
	SeeAlso:	GetTruncatedPath();
	Errors:		None

**********************************************************************************************/

BOOL getParentDirectory(const TCHAR **fn, TCHAR *directory, UINT32 MaxSize, const TCHAR * StartOfPath)
{
	UINT32 i = 0; 

	// Note where we are in the path
	const TCHAR* CurrentPosition = *fn;

	// If we are tsarting on a separator then skip it 
	if (**fn == SEPARATOR)
		(*fn)--;

	// Search from the specified string end backwards until we find either:-
	// - a path separator indicating we have found a potential parent directory.
	// - the start of the file indicating no parent directory is present
	while (	
			(**fn != SEPARATOR) &&				// is not at a path separating character
			(*fn != StartOfPath) &&				// is not at the start of the path string
			(i < MaxSize)						// is not maximum size 
		  )
	{ 
		i++;
		(*fn)--;
	}
		
	if (**fn != SEPARATOR)						// if we are not at a directory separator
		return FALSE;							//  Then Syntax error
		
	// Make up the string that we are going to return
	// Do it this way as otherwise we would be adding the characters in backwards in the above
	// routine as we are scanning backwards!
	UINT32 position = 0;	// Start at first character in the string
	while (( i >= 0 ) && (*fn != CurrentPosition))
	{
		directory[position++] = **fn;			// Add current character to directory name
		(*fn)++;								// Move to next character
		i--;									// decrement directory name letters count
	}

	directory[position++] = END_OF_STRING;		// Set end of string 
	
	return TRUE;
}


/**********************************************************************************************

>	BOOL IsSpecialChar(const TCHAR **fn)

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/10/93
	Inputs:		A pointer to the pointer to the path string
	Outputs:	None
	Returns:	True if character is a special char allowed in filenames and False otherwise
	Purpose:	Checks if character passed in is a special character allowed in file names.
				No longer used.
	Scope:		Private
	SeeAlso:	PathName();
	Errors:		None

**********************************************************************************************/

// Function no longer required
//BOOL IsSpecialChar(TCHAR **fn)
//{
//	const TCHAR UNDERSCORE 	= '_';
//	const TCHAR HAT			= '^';
//	const TCHAR DOLLAR		= '$';
//	const TCHAR EXCLAMATION = '!';
//	const TCHAR HASH		= '#';
//    const TCHAR PERCENT     = '%';
//    const TCHAR AMPESAND    = '&';
//    const TCHAR MINUS    	= '-';
//    const TCHAR OPENCURLY   = '{';
//    const TCHAR CLOSEDCURLY = '}';
//    const TCHAR OPENBRAC	= '(';
//    const TCHAR CLOSEDBRAC	= ')';
//    const TCHAR ATSIGN		= '@';
//    const TCHAR INVERTCOMMA = '`';
//	
//	if ((**fn != UNDERSCORE) &&
//		(**fn != HAT) &&
//		(**fn != DOLLAR) &&
//		(**fn != EXCLAMATION) &&
//		(**fn != HASH) &&
//	    (**fn != PERCENT) &&
//	    (**fn != AMPESAND) &&
//	    (**fn != MINUS) &&
//	    (**fn != OPENCURLY) &&
//	    (**fn != CLOSEDCURLY) &&
//	    (**fn != OPENBRAC) &&
//	    (**fn != CLOSEDBRAC) &&
//	    (**fn != ATSIGN) &&
//	    (**fn != INVERTCOMMA))
//	    return FALSE;
//	else
//	    return TRUE;
//}

/**********************************************************************************************

>	BOOL IsReservedChar(const TCHAR **fn)

	Author: 	Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/08/94
	Inputs:		A pointer to the pointer to the path string
	Outputs:	None
	Returns:	True if the character is a reserved character not allowed in filenames
				and False otherwise
	Purpose:	Checks if character passed in is a reserved character allowed in file names.
				Does not check for terminators which are allowed by the callers such as .
				and backslash. These characters are considered illegal when converting long to
				short filenames and so it would seem sensible to limit their use.
	Scope:		Private
	SeeAlso:	PathName();
	Errors:		None

**********************************************************************************************/

BOOL IsReservedChar(const TCHAR **fn)
{
	const TCHAR SLASH 		= '/';
	const TCHAR OPENSQUARE	= '[';
	const TCHAR CLOSESQUARE	= ']';
	const TCHAR QUOTES		= '"';
	const TCHAR COLON_TCHAR	= ':';
	const TCHAR SEMICOLON	= ';';
	const TCHAR COMMA		= ',';
	const TCHAR EQUALS		= '=';
	
	if ((**fn != SLASH) &&
		(**fn != OPENSQUARE) &&
		(**fn != CLOSESQUARE) &&
		(**fn != QUOTES) &&
		(**fn != COLON_TCHAR) &&
		(**fn != SEMICOLON) &&
		(**fn != COMMA) &&
		(**fn != EQUALS))
	    return FALSE;
	else
	    return TRUE;
}

			
/**********************************************************************************************

>	BOOL IsDeviceName(const String_256& path)

	Author: 	Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/5/93
	Inputs:		String representing a pathname
	Outputs:	None
	Returns:	TRUE if the pathname is a device name or FALSE otherwise.
	Purpose:	Parses a string representing a path	and sees if it refers to
				a device name which would be illegal.
	Errors:		None 

**********************************************************************************************/

BOOL IsDeviceName(const String_256& path)
{
#if defined(__WXMSW__)	
	const INT32			buffer_size = 256;
	char				buffer[buffer_size];			// create buffer of characters
	// First use a Windows call to try and see if the pathname supplied is a
	// known device name. Use a 256 string as a buffer as not particular bothered
	// by what is returned, only if something is returned  
	DWORD i;
	i = QueryDosDevice(path, buffer, buffer_size);	// See if known Windows device name

	// returns i = number of characters supplied in pBuffer, if 0 then failed
	//TRACE( _T("QueryDosDevice returns %d\n"),i);
	if (i>0)
		return TRUE;						// valid device name, therefore passes
#endif
	
	// Check our pathname against some known basic names as a final check
	// Device names will in general have three letters with a possible number
	// and possible colon on the end.
	String_256 temp;						// general workspace string
	
	// work out in temp the left most three characters and then compare this with
	// some known bad values
	path.Left(&temp,4);						// temp = left most 4 characters of path

	if ( temp.IsIdentical(TEXT(SEPARATOR_SYM SEPARATOR_SYM "." SEPARATOR_SYM )))		// NT device name starts with this
		return TRUE;						// if start this then no more checks required

	path.Left(&temp,3);						// temp = left most 3 characters of path
	temp.toUpper();							// Now convert to upper case

	if ( (temp.IsIdentical(TEXT("LPT"))) ||		// basic DOS print device
	     (temp.IsIdentical(TEXT("COM"))) ||		// basic DOS communication port
	     (temp.IsIdentical(TEXT("NUL"))) ||		// basic DOS null device
	     (temp.IsIdentical(TEXT("AUX"))))		// basic DOS auxiliary port
	{
		// Check that the length of the name is correct i.e. 3, 4 or 5 long
		const TCHAR	*dn;						// device name pointer
		dn = path;							// set to point to the current path
		
		switch (path.Length())
		{
			case 3:
				// correct length and passed the test above so its a device name
				// drop through to after case and return True
			break;
	
			case 4:
				// check for number or a colon on the end
				// position is from 0.. and so want character number 3
				dn += 3;						// point to third character in name 					
				if ( ( !String::IsNumeric(*dn) ) && ( *dn != COLON ) )
					return FALSE;				// not colon or number
				// drop through to after case and return True
			break;

			case 5:
				// check for number and colon on the end 
				// position is from 0.. and so want character number 3
				dn += 3;						// point to third character in name 					
				// first check that the second to last character is a number
				if ( !String::IsNumeric(*dn) )
					return FALSE;				// not a number so return False
				// now check that the last character is a colon
				dn += 1;						// move to last character
				if ( *dn != COLON )
					return FALSE;				// not a colon so return False
				// drop through to after case and return True
			break;
		
			default:
				// In all other cases can return False as wrong length
				return FALSE;
			break;
		} // end switch
		return TRUE;	// passed above tests so return True
	}
	else
		return FALSE;	// failed in three letter comparisons so not a recognised device
}




/**********************************************************************************************

>	DirPath::DirPath()
	DirPath::DirPath(const DirPath& newPath)
	DirPath::DirPath(const String_256& path)

	Author: 	Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/2/96
	Purpose:	DirPath class constructors

**********************************************************************************************/

DirPath::DirPath() : PathName()
{
}
DirPath::DirPath(const DirPath& newPath) : PathName(newPath)
{
}
DirPath::DirPath(const String_256& path) : PathName(path)
{
}

/**********************************************************************************************

>	BOOL DirPath::SetDirPath(const String_256& path)

	Author: 	Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/2/96
	Inputs:		path	- String representing a pathname
	Returns:	TRUE if the directory pathname is parsed successfully or FALSE otherwise
	Purpose:	Parses a string representing a path	and sets up the class
	 			variables filetype, filename and location if the path is valid
	 			otherwise it sets them to null strings. 
	Errors:		None

**********************************************************************************************/

BOOL DirPath::SetDirPath(const String_256& path)
{
	return PathName::SetPathName(path);
}

/**********************************************************************************************

>	virtual BOOL DirPath::IsValid(const String_256& path) const

	Author: 	Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/2/96
	Inputs:		path	- String representing a pathname
	Returns:	TRUE if the directory pathname is parsed successfully or FALSE otherwise
	Purpose:	Parses a string representing a directory path and sets up the class
	 			variables filetype, filename and location if the path is valid.
				Cannot do the usual 8.3 checking of things like filenames as this is no longer
				valid given Chicago and NT non-FAT filing systems. 
	Errors:		If it fails then it will call set error with the error found.

**********************************************************************************************/

BOOL DirPath::IsValid(const String_256& path) const
{
	return PathName::IsValid(path);
}

/**********************************************************************************************

>	virtual BOOL DirPath::IsValid() const

	Author: 	Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/2/96
	Returns:	TRUE if the present pathname has been parsed successfully or FALSE otherwise
	Purpose:	Says whether the pathname currently in the class has been parsed correctly.
				This should only be used if the non-blank contructors have been called and more
				specifically the contructor which takes a pathname as a parameter has been used.
				This function then returns the result of the validation that happens in the 
				constructor as of course constructors cannot return results!
	SeeAlso:	DirPath::IsValid(const String_256& path);	
	Errors:		If it fails then it will call set error with the error found.

**********************************************************************************************/

BOOL DirPath::IsValid() const
{
	return PathName::IsValid();
}

/**********************************************************************************************

>	virtual BOOL DirPath::IsValidAndReturnInfo(const String_256& ConstPath,	String_256& tempFilename,
		String_256& tempLocation, String_256& tempFiletype,	String_256& tempDrivename,
		BOOL SetErrors = TRUE) const

	Author: 	Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/2/96

	Inputs:		ConstPath		- String representing the pathname to check
				SetErrors		- Flag to say whether we should set errors or not (defaults to TRUE)

	Outputs:	tempFilename	- String representing the filename (last entry in the path list)
				tempLocation	- String representing the location (all entries before the last)
				tempFiletype	- String representing the filetype (probably empty with dir names)
				tempDrivename	- String representing the drive name
	
	Returns:	TRUE if the dirpath is parsed successfully or FALSE otherwise

	Purpose:	Parses a string representing a directory path and returns if it's valid or not. It
				also splits the string up into drivename, location and last directory name in the
				list. (it does this because it's overriding the pathname version, and can be
				useful if you know what it's doing !)

				Routine called by IsValid to actually go and do the work involved with
				checking the pathname. Done this way so that internal calls do not have to
				parse the data again to get at the variables.

	Errors:		If it fails then it will call set error with the error found.
				This can now be supressed if required. 

**********************************************************************************************/

BOOL DirPath::IsValidAndReturnInfo(const String_256& ConstPath,	String_256& tempFilename,
		String_256& tempLocation, String_256& tempFiletype,	String_256& tempDrivename, BOOL SetErrors) const
{
	// This routine doesn't actually do very much because it was found that the pathname class
	// returns ok for "C:\"... Possibly the FilePath class should detect this and return FALSE ?
	
	return PathName::IsValidAndReturnInfo(ConstPath, tempFilename, tempLocation, tempFiletype,
		tempDrivename, SetErrors);
}




/**********************************************************************************************

>	FilePath::FilePath()
	FilePath::FilePath(const FilePath& newPath)
	FilePath::FilePath(const String_256& path)

	Author: 	Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/2/96
	Purpose:	FilePath class constructors

**********************************************************************************************/

FilePath::FilePath() : PathName()
{
}
FilePath::FilePath(const FilePath& newPath) : PathName(newPath)
{
}
FilePath::FilePath(const String_256& path) : PathName(path)
{
}

/**********************************************************************************************

>	BOOL DirPath::SetFilePath(const String_256& path)

	Author: 	Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/2/96
	Inputs:		path	- String representing a file + its pathname
	Returns:	TRUE if the file pathname is parsed successfully or FALSE otherwise
	Purpose:	Parses a string representing a path	and sets up the class
	 			variables filetype, filename and location if the path is valid
	 			otherwise it sets them to null strings. 
	Errors:		None

**********************************************************************************************/

BOOL FilePath::SetFilePath(const String_256& path)
{
	return PathName::SetPathName(path);
}

/**********************************************************************************************

>	virtual BOOL FilePath::IsValid(const String_256& path) const

	Author: 	Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/2/96
	Inputs:		path	- String representing a pathname
	Returns:	TRUE if the file pathname is parsed successfully or FALSE otherwise
	Purpose:	Parses a string representing a file path and sets up the class
	 			variables filetype, filename and location if the path is valid.
				Cannot do the usual 8.3 checking of things like filenames as this is no longer
				valid given Chicago and NT non-FAT filing systems. 
	Errors:		If it fails then it will call set error with the error found.

**********************************************************************************************/

BOOL FilePath::IsValid(const String_256& path) const
{
	return PathName::IsValid(path);
}

/**********************************************************************************************

>	virtual BOOL FilePath::IsValid() const

	Author: 	Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/2/96
	Returns:	TRUE if the present pathname has been parsed successfully or FALSE otherwise
	Purpose:	Says whether the pathname currently in the class has been parsed correctly.
				This should only be used if the non-blank contructors have been called and more
				specifically the contructor which takes a pathname as a parameter has been used.
				This function then returns the result of the validation that happens in the 
				constructor as of course constructors cannot return results!
	SeeAlso:	FilePath::IsValid(const String_256& path);	
	Errors:		If it fails then it will call set error with the error found.

**********************************************************************************************/

BOOL FilePath::IsValid() const
{
	return PathName::IsValid();
}

/**********************************************************************************************

>	virtual BOOL FilePath::IsValidAndReturnInfo(const String_256& ConstPath, String_256& tempFilename,
		String_256& tempLocation, String_256& tempFiletype,	String_256& tempDrivename, BOOL SetErrors = TRUE) const

	Author: 	Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/2/96

	Inputs:		ConstPath		- String representing the pathname to check
				SetErrors		- Flag to say whether we should set errors or not (defaults to TRUE)

	Outputs:	tempFilename	- String representing the filename
				tempLocation	- String representing the location
				tempFiletype	- String representing the filetype
				tempDrivename	- String representing the drive name
	
	Returns:	TRUE if the pathname is parsed successfully or FALSE otherwise
	Purpose:	Parses a string representing a path	and returns the filetype, filename
				and location to the caller if the path is valid.
				Cannot do the usual 8.3 checking of things like filenames as this is no longer
				valid given Chicago and NT non-FAT filing systems.
				Routine called by IsValid to actually go and do the work involved with
				checking the pathname. Done this way so that internal calls do not have to
				parse the data again to get at the variables.  
	Errors:		If it fails then it will call set error with the error found.
				This can now be supressed if required. 

**********************************************************************************************/

BOOL FilePath::IsValidAndReturnInfo(const String_256& ConstPath, String_256& tempFilename,
		String_256& tempLocation, String_256& tempFiletype,	String_256& tempDrivename, BOOL SetErrors) const
{
	return PathName::IsValidAndReturnInfo(ConstPath, tempFilename, tempLocation, tempFiletype,
		tempDrivename, SetErrors);
}
