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

#ifndef INC_PathName
#define INC_PathName
 
/*********************************************************************************************

>	class PathName : public CCObject

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/5/1993          
	Base Class:	CCObject
	Purpose:	Represents a simple path and filename.	
	Errors:		None.

*********************************************************************************************/

class PathName : public CCObject
{
protected:
	BOOL PathNameValid;									// If we like that pathname
	String_256 location;								// Where file resides in file system
	String_256 filename;    	                		// Name of file
	String_256 filetype;        	            		// Type of file - Extension
	String_256 drivename;								// Drive location
	virtual BOOL IsValidAndReturnInfo(const String_256& ConstPath, String_256& tempFilename,
		String_256& tempLocation, String_256& tempFiletype,	String_256& tempDrivename, BOOL SetErrors = TRUE) const;
														// Checks if a pathname is valid or not
														// and if so returns filename, location
														// and file extension found to caller.
	void TruncateName(String_256& FullName, String_256* pShortName, INT32 MaxSize) const;

public:
	PathName();
	PathName(const PathName&);	
	PathName(const String_256&);						// Initialise PathName with string
	
 	const String_256 GetPath(BOOL TrailingSlash=FALSE) const; 	// return full path of file
 	const String_256 GetLocation(BOOL KeepSep=TRUE) const; // return just the location
 	const String_256 GetFileName(BOOL FullName=TRUE) const;// return just the filename.ext
	const String_256 GetType() const;                      // return the file type = .ext

	String_256 GetWebAddress() const;

	const String_256 GetTruncatedPath(UINT32 MaxSize = 0) const; // return truncated form of path
	const String_256 GetTruncatedLocation(UINT32 MaxSize = 0) const; // return truncated form of location
	
	BOOL SetType(const String_256& NewType);			// Allows you to change the file extension
	BOOL SetFileName(const String_256& NewFileName);	// Modifies the filename (if extension present then ignores)
	BOOL SetFileNameAndType(const String_256& NewFileName);// Modifies the filename and type
	BOOL SetPathName(const String_256&, BOOL SetErrors = TRUE);				// Builds a PathName from a string


	virtual BOOL IsValid(const String_256&) const;					// Checks if a pathname is valid or not
	virtual BOOL IsValid() const;										// Checks if the current pathname is valid or not
	
	PathName& operator=(const PathName&);				// Assigns one PathName onto another

};



/*********************************************************************************************

>	class DirPath : public PathName

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/2/1996
	Purpose:	Represents and validates a directory pathname, rather than a file pathname
	Errors:		None.

*********************************************************************************************/

class DirPath : public PathName
{
public:
	DirPath();
	DirPath(const DirPath&);
	DirPath(const String_256&);

	BOOL SetDirPath(const String_256&);				// Builds a DirPath from a string

	virtual BOOL IsValid(const String_256&)const;		// Checks if a pathname is valid or not
	virtual BOOL IsValid()const;							// Checks if the current pathname is valid or not

protected:
	virtual BOOL IsValidAndReturnInfo(const String_256&, String_256&, String_256&, String_256&, 
							  String_256&, BOOL=TRUE)const;
		// Checks if a pathname is valid or not and if so returns filename, location
		// and file extension (which should be empty) found to caller.
		// Note that the filename is the last directory name in the path, so "C:\fred\bob" will
		// return "bob" as its filename, "C:\fred" as its location. (GetPath() will return the
		// lot.
};



/*********************************************************************************************

>	class FilePath : public PathName

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/2/1996
	Purpose:	Represents and validates a file pathname, rather than a directory pathname
	Errors:		None.

*********************************************************************************************/

class FilePath : public PathName
{
public:
	FilePath();
	FilePath(const FilePath&);
	FilePath(const String_256&);

	BOOL SetFilePath(const String_256&);			// Builds a FilePath from a string

	virtual BOOL IsValid(const String_256&)const;		// Checks if a file pathname is valid or not
	virtual BOOL IsValid()const;							// Checks if the current pathname is valid or not

protected:
	virtual BOOL IsValidAndReturnInfo(const String_256&, String_256&, String_256&, String_256&, 
							  String_256&, BOOL=TRUE)const;
		// Checks if a file pathname is valid or not and if so returns filename, location
		// and file extension found to caller.
};


#endif

