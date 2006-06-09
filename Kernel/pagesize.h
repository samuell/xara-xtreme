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
//
// Encapsulates the page sizes available in Camelot.

#ifndef INC_PAGESIZES
#define INC_PAGESIZES

//#include "listitem.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "list.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "units.h" - in camtypes.h [AUTOMATICALLY REMOVED]

// Page sizes that are allowed in the sizes list
// The ordering of the ids must correspond to the ordering of the page size
// definitions in PageSizesList::InitPageSizes()
enum PageId {CUSTOM, A0, A1, A2, A3, A4, A5, A6, USLEGAL, USLETTER, FANFOLD}; 

// These are now loaded in and will be calculated as the page size is defined
// just need the new last ID = the next ID to be used when the neex page is defined 
// Above still required as if no ini files is found or problems happen with the ini
// file then we use the built in defaults
#define LASTPAGE A0; 

class CCLexFile;

/********************************************************************************************

>	class PageSize : public ListItem

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>		
	Created:	12/12/94	
	Purpose:	Encapsulates all the page size information and functionality in one class.
				The class knows what size an A4 page is and what its textual name is.

********************************************************************************************/

class CCAPI PageSize : public ListItem
{
	CC_DECLARE_DYNAMIC(PageSize);

public:
	PageSize();								// Constructor

	// Initialisation
	virtual BOOL Init(PageId NewId, String_32 *Name, double PageWidth, double PageHeight,
					  UnitType Units);

	PageId GetPageId();		 				// Returns page id
	String_32 *GetPageName();				// returns name of page
	INT32 GetPageWidth();					// returns width of page
	INT32 GetPageHeight();					// returns height of page

protected:
	PageId Id;								// id assigned to this page e.g. A4
	String_32 PageName;						// Textual page name for use in lists etc.
	INT32 PageWidth;							// Width of this page in millipoints
	INT32 PageHeight;						// Height of this page in millipoints
	double PageWidthInMM;					// Width of this page in mm
	double PageHeightInMM;					// Height of this page in mm
};

/********************************************************************************************

>	class PageSizesList : public List

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>		
	Created:	12/12/94	
	Purpose:	Encapsulates all the page size information and functionality in one class.
				The class knows what size an A4 page is and what its textual name is.

********************************************************************************************/

class PageSizesList : public List
{
	CC_DECLARE_DYNAMIC(PageSizesList);

public:
	PageSizesList();						// Constructor
	~PageSizesList();						// Destructor

	BOOL InitPageSizes();					// set up all page sizes available
	BOOL DeinitPageSizes();					// remove all page sizes available

	PageSize *GetFirstPageSize();
	PageSize *GetNextPageSize(PageSize*);

	// Function to declare a page size to the PageSizes system
	BOOL DeclarePageSize(PageId NewId, String_32 *Name, double Width, double Height,
						 UnitType Units, PageSize **pPageSize);

	BOOL DeclarePageSize(PageId NewId, ResourceID r, double Width, double Height,
						UnitType Units, PageSize **pPageSize)
						{ String_32 Name(r); return DeclarePageSize(NewId, &Name, Width, Height, Units, pPageSize); }

	// Useful functions which are supplied by each page size in the list
	String_32* GetTextualDescription(PageId ThisId);
	PageId GetPageIdFromSize(INT32 Width, INT32 Height);
	BOOL GetSizeFromPageId(PageId ThisId, INT32 *Width, INT32 *Height);

private:
	// Functions for reading page sizes from either the resource or from
	// an ini file
	BOOL ReadPageSizes();

	BOOL ReadPageSizesFromDisk();
	BOOL ReadPageSizesFromRes();
	BOOL ReadPageSizesFromFile(CCLexFile& file);

	BOOL ReadPageSizesList(CCLexFile& file);

	// Main function that actually does the reading in of the data
	BOOL ReadPageSizeDef(CCLexFile& file, String_32* pPageName, double* pWidth, 
						 double* pHeight, UnitType* pUnits);

	// This stores the next page id that is going to be allocated when a new page
	// is created. 
	INT32	MaxPageId;

	// This stores the error id for later use. 
	UINT32 ErrorId;
};

#endif  // INC_PAGESIZE


