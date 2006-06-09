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
// SGRMfile.h - Gallery Resource Management file routines

#ifndef INC_SGRMFILE
#define INC_SGRMFILE

//#include "ccobject.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "ccfile.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "list.h" - in camtypes.h [AUTOMATICALLY REMOVED]

class PathName;
class String_256;

/********************************************************************************************

>	class LibSettingsListItem : public ListItem

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/01/96
	Purpose:	Memory list of section lines to speed GRMFile reading up somewhat
	SeeAlso:	LibSettings::BuildList, which builds the list from a section name
	
********************************************************************************************/

class LibSettingsListItem : public ListItem
{
	CC_DECLARE_DYNCREATE( LibSettingsListItem )

public:
	LibSettingsListItem();
	~LibSettingsListItem();

	LibSettingsListItem(String_256 *Entry);
	String_256 *Line;
};



/********************************************************************************************

>	LibSettings

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/5/95
	Purpose:	The Gallery Resource Management class...

	Notes:		Currently this code uses the GRM file to remember which groups were in which
				libraries and in which order when quitting the program so than on loading next
				time we can reflect the old status in the library...
				
				As this is fairly general read and write ini file code, then this has been
				expanded so that an option other ini file name can be specified. If this is
				specified then this will be used instead of the GRM file name.
				Added by Neville 14/1/97 for saving/loading paths for plug-ins as the main 
				ini file code cannot cope with the concept of sections filled with an arbitrary
				number of items which need cleaning out when resaving to remove old items no
				longer required.

********************************************************************************************/

class LibSettings: public CCObject
{
	CC_DECLARE_DYNCREATE( LibSettings )

public:
  	LibSettings();
	~LibSettings();
	static BOOL Init(void);
	static BOOL GetEntry(String_256 *Section, INT32 Line, String_256 *Entry,
						 FilePos *Pos = NULL, String_256 * pIniFileString = NULL);
	static BOOL AddLine(String_256 *Line, String_256 * pIniFileString = NULL);
	static BOOL AddSection(String_256 *Section, String_256 * pIniFileString = NULL);
	static BOOL IsThisANewSection(String_256 *Line, String_256 *Section);
	static BOOL StartOutput(String_256 *Section, String_256 * pIniFileString = NULL);
	static BOOL NukeSettings(String_256 * pIniFileString = NULL);
	static BOOL GetPathName(PathName *PathName, String_256 * pIniFileString = NULL);

	static BOOL GetEntryStart(CCDiskFile *DiskFile, String_256 * pIniFileString = NULL);
	static BOOL GetEntryFromFile(CCLexFile *TheFile, String_256 *Section, INT32 Line, String_256 *Entry,
						FilePos *Pos = NULL);
	static BOOL GetEntryFinish(CCDiskFile *DiskFile);

	static BOOL BuildList(String_256 *Section, List *OutputList);
	static BOOL GetNextLine(CCDiskFile *TheFile, String_256 *Line);
	static BOOL SeekToSection(CCDiskFile *TheFile, String_256 *Section);

	static CCDiskFile *DiskFile;
};  

#endif
