// $Id: pagesize.cpp 662 2006-03-14 21:31:49Z alex $
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

/*
*/

#include "camtypes.h"
#include "pagesize.h"
#include "errors.h"
#include "units.h"		// units
#include "convert.h"	// conversion utilities

//#include "will2.h"

#include "ccfile.h"
#include "oilfiles.h"
//#include "nev.h"
#include <limits.h>		// INT32_MAX 
//#include "pagesres.h"	// page related error messages and resources

// This is not compulsory, but you may as well put it in so that the correct version
// of your file can be registered in the .exe
DECLARE_SOURCE("$Revision: 662 $");

// An implement to match the Declare in the .h file.
// If you have many classes, it is recommended to place them all together, here at the start of the file
CC_IMPLEMENT_DYNAMIC(PageSize, ListItem)
CC_IMPLEMENT_DYNAMIC(PageSizesList, List)

// This will get Camelot to display the filename and linenumber of any memory allocations
// that are not released at program exit
#define new CAM_DEBUG_NEW

//List PageSize::PageSizeList;			// The global list of installed page sizes.


enum PsTokenIndex
{
	PsTOKEN_NONE = -1,
	TOKEN_PAGESIZE,
	TOKEN_PAGESIZEEND,
	TOKEN_MILLIMETRES,
	TOKEN_INCHES,
	// Add new token indexs BEFORE NUM_TOKENS
	PsNUM_TOKENS
};

static struct
{
	TCHAR*  Token;
} TokenTable[] = 
{
	"PageSize",
	"PageSizeEnd",
	"MILLIMETRES",
	"INCHES",
};


static PsTokenIndex FindToken(const TCHAR* Token)
{
	for (INT32 i = 0; i < PsNUM_TOKENS; i++)
		if (lstrcmp(TokenTable[i].Token,Token) == 0) return  (PsTokenIndex) i;

	return PsTOKEN_NONE;
}

/********************************************************************************************

>	PageSize::PageSize()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/94
	Purpose:	Constructor.
	Errors:
	SeeAlso:	AppPrefsDlg;

********************************************************************************************/

PageSize::PageSize()
{
	// Do nothing
}

/********************************************************************************************

>	BOOL PageSize::Init(PageId NewId, String_32 *Name, double Width, double Height,
					UnitType Units)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/94
	Inputs:		NewId	unique page id for this page size
				Name	textual name for this page (for menus etc.)
				Width	width in the specified units e.g. millimetres
				Height	height in the specified units
				Units	the specified units
	Purpose:	Initialiser.
	Errors:
	SeeAlso:	PageSize; AppPrefsDlg;

********************************************************************************************/

BOOL PageSize::Init(PageId NewId, String_32 *Name, double Width, double Height,
					UnitType Units)
{
	// Guard against floating point exceptions by checking the input values
	// 0.1" or 0.1mm should be a good enough minimim
	ERROR2IF(Width < 0.1 || Width > (double)INT32_MAX, FALSE, "PageSize::Init bad width")
	ERROR2IF(Height < 0.1 || Height > (double)INT32_MAX, FALSE, "PageSize::Init bad width")

	Id = NewId;					// Copy the page id across
	PageName = *Name;			// Copy the page name across 
 	PageWidthInMM = Width;		// Copy the width across
 	PageHeightInMM = Height;	// Copy the height across
	
	// Now work out the equivalent millipoint values
	// Cannot use the ConvertToNewUnits functions as these require a current doc
	// and we have not got one yet! Do it the hard way but use the conversion values
	// specified in units.h
	switch (Units)
	{
		case MILLIMETRES:
			PageWidth = (INT32)(Width * MM_MP_VAL);
			PageHeight = (INT32)(Height * MM_MP_VAL);
			break;
		case INCHES:
			PageWidth = (INT32)(Width * IN_MP_VAL);
			PageHeight = (INT32)(Height * IN_MP_VAL);
			break;
		default:
			ERROR2(FALSE,"Bad units used in PageSize::Init");
			break;
	}
	//PageWidth = ConvertToNewUnits(Width, MILLIMETRES, MILLIPOINTS);
	//PageHeight = ConvertToNewUnits(Height, MILLIMETRES, MILLIPOINTS);
	
	return TRUE;
}


/********************************************************************************************

>	PageId PageSize::GetPageId()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/94
	Returns:	The id for this page
	Purpose:	Returns page id
	SeeAlso:	PageSize; AppPrefsDlg;

********************************************************************************************/
PageId PageSize::GetPageId()
{
	return Id;
}

/********************************************************************************************

>	String_32 *PageSize::GetPageName()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/94
	Returns:	pointer to the name of this page
	Purpose:	Way of finding the name for this page.
	SeeAlso:	PageSize; AppPrefsDlg;

********************************************************************************************/
String_32 *PageSize::GetPageName()
{
	return &PageName;
}


/********************************************************************************************

>	PageSize::GetPageWidth()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/94
	Returns:	Width of this page in millipoints.
	Purpose:	Way of of finding out the current page width.
	SeeAlso:	PageSize; AppPrefsDlg;

********************************************************************************************/
INT32 PageSize::GetPageWidth()
{
	return PageWidth;
}

/********************************************************************************************

>	INT32 PageSize::GetPageHeight()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/94
	Returns:	Height of this page in millipoints.
	Purpose:	Way of of finding out the current page height.
	SeeAlso:	PageSize; AppPrefsDlg;

********************************************************************************************/
INT32 PageSize::GetPageHeight()
{
	return PageHeight;
}



/******************************************************************************************
********************************************************************************************/


/********************************************************************************************

>	PageSizesList::PageSizesList()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/94
	Purpose:	Constructor.
	Errors:
	SeeAlso:	AppPrefsDlg;

********************************************************************************************/

PageSizesList::PageSizesList()
{
	// Set up useful defaults

	// This is the next page id which will be used when the next page is added
	MaxPageId = (INT32)LASTPAGE;
}

/********************************************************************************************

>	PageSizesList::~PageSizesList()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/94
	Purpose:	Destructor.
	Errors:
	SeeAlso:	AppPrefsDlg;

********************************************************************************************/


PageSizesList::~PageSizesList()
{
	// Remove all page sizes from the list
	ListItem* pItem;

	while ((pItem = this->RemoveHead()) != NULL)
		delete pItem;
}

/********************************************************************************************

>	BOOL PageSizesList::InitPageSizes()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/94
	Returns:	TRUE if initialised ok, FALSE if not.
	Purpose:	Scans all PageSize to find out which PageSize are available for use, and
				instantiates one of each, and holds them in a list.
	Errors:		Out of memory (if a PageSize fails to initialise for other reasons then an 
				error is reported, but Camelot will still start up ok, i.e. TRUE is 
				returned).
	SeeAlso:	PageSize

********************************************************************************************/

BOOL PageSizesList::InitPageSizes()
{
	// Reset our store for error message identifiers to the 'no error' state.
	ErrorId = 0;
	
	// First, try and read hot keys from resource file
	BOOL Ok = ReadPageSizes();

	// Failed to read in the pages from the resource, tell the user this and what we think
	// the problem is, then use the bound in pages.
	if (!Ok)
	{
		// For now if we fail, error 3 to tell the user and then use the bound in version. 
		if (ErrorId == 0)
			ErrorId = _R(IDW_PAGES_UNKNOWNERROR);
	
		String_256 ErrorReason;
		ErrorReason.MakeMsg(ErrorId);
		
		// Combine the above error with the main error message
		String_256 ErrorMsg;
		ErrorMsg.MakeMsg(_R(IDW_PAGES_FAILEDREADING), (TCHAR*) ErrorReason);
		Error::SetError(_R(IDW_PAGES_FAILEDREADING), (TCHAR *) ErrorMsg, 0);
		InformError();															\
		TRACEUSER( "Neville", _T("PageSizesList::InitPageSizes failed to read in list from resource, using bound in one"));

		// Vape any that may have been created
		this->DeleteAll();
		
		// Create, initialise and install the built in PageSize list
		PageSize *pPageSize;

		// First size is implied to be custom i.e. if unknown size.
		DeclarePageSize(A0, &String_32(_R(IDS_K_PAGESIZE_A0)), 840.0, 1188.0, MILLIMETRES, &pPageSize);
		DeclarePageSize(A1, &String_32(_R(IDS_K_PAGESIZE_A1)), 594.0, 840.0, MILLIMETRES, &pPageSize);
		DeclarePageSize(A2, &String_32(_R(IDS_K_PAGESIZE_A2)), 420.0, 594.0, MILLIMETRES, &pPageSize);
		DeclarePageSize(A3, &String_32(_R(IDS_K_PAGESIZE_A3)), 297.0, 420.0, MILLIMETRES, &pPageSize);
		DeclarePageSize(A4, &String_32(_R(IDS_K_PAGESIZE_A4)), 210.0, 297.0, MILLIMETRES, &pPageSize);
		DeclarePageSize(A5, &String_32(_R(IDS_K_PAGESIZE_A5)), 148.5, 210.0, MILLIMETRES, &pPageSize);
		DeclarePageSize(A6, &String_32(_R(IDS_K_PAGESIZE_A6)), 105.0, 148.5, MILLIMETRES, &pPageSize);
		DeclarePageSize(USLEGAL, &String_32(_R(IDS_K_PAGESIZE_USLEGAL)), 8.5, 14, INCHES, &pPageSize);
		DeclarePageSize(USLETTER, &String_32(_R(IDS_K_PAGESIZE_USLETTER)), 8.5, 11, INCHES, &pPageSize);
		DeclarePageSize(FANFOLD, &String_32(_R(IDS_K_PAGESIZE_FANFOLD)), 8.25, 12, INCHES, &pPageSize);
	}
	
	// All ok
	return TRUE;
}

/********************************************************************************************

>	void PageSizesList::DeinitPageSizes()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/94
	Purpose:	Destroy all the PageSize objects.
	SeeAlso:	PageSize

********************************************************************************************/

BOOL PageSizesList::DeinitPageSizes()
{
	// Get rid of our PageSize list
	this->DeleteAll();

	// All ok
	return TRUE;
}

/********************************************************************************************

>	BOOL PageSizesList::DeclarePageSize(PageId NewId, String_32 *Name, double Width, double Height,
									UnitType Units, PageSize **ppPageSize)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/94
	Inputs:		NewId		unique pageid.
				Name		textual desciption of page size
				Width		width of page in mm.
				Height		height of page in mm.
				ppPageSize	pointer to a page size pointer to be updated
	Returns:	True if page size declared, False if something went wrong. 
	Purpose:	A function to declare a new page size to the system.
	SeeAlso:	PageSize

********************************************************************************************/

BOOL PageSizesList::DeclarePageSize(PageId NewId, String_32 *Name, double Width, double Height,
									UnitType Units, PageSize **ppPageSize)
{
	// First check to see if somebody has actually declared the pointer to be non null.
	PageSize *pPageSize = new PageSize;
	if (pPageSize == NULL) return FALSE;

	if (!pPageSize->Init(NewId, Name, Width, Height, Units))
	{
		/* Error occured - report it and stop trying to initialise the options tab. */
		delete pPageSize;		// remove the declared page size
		pPageSize = NULL;		// remove the item from the system
		InformError();			// tell the user about the problem
		return TRUE;
	}

	// Add this new item at the end of the list
	this->AddTail(pPageSize);

	*ppPageSize = pPageSize;	// update the new end pointer 
	
	// All ok
	return TRUE;
}

/********************************************************************************************

>	PageSize *PageSizesList::GetFirstPageSize()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/94
	Returns:	Pointer to the first page size NULL if none.
	Purpose:	Iterating through the list of PageSize known to Camelot.  This function
				returns the first PageSize.
	SeeAlso:	PageSizesList::GetNext()
	Scope:		Static

********************************************************************************************/

PageSize *PageSizesList::GetFirstPageSize()
{
	return (PageSize *) this->GetHead();
}

/********************************************************************************************

>	PageSize *PageSizesList::GetNextPageSize(PageSize *pPageSize)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/94
	Inputs:		pPageSize - pointer to the PageSize to be used to get the following
				PageSize.
	Returns:	Pointer to the next PageSize, or NULL if none.
	Purpose:	Iterating through the list of PageSize known to Camelot.  This function
				returns the next PageSize.
	SeeAlso:	PageSizesList::GetFirst

********************************************************************************************/

PageSize *PageSizesList::GetNextPageSize(PageSize *pPageSize)
{
	return (PageSize *) this->GetNext(pPageSize);
}

/********************************************************************************************

>	String_32* PageSizesList::GetTextualDescription(PageId ThisId)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/94
	Inputs:		PageId - id of the page that we are searching for
	Returns:	Textual name for the specified page.
	Purpose:	Return the name for the page size specified.
	SeeAlso:	PageSize::GetFirst

********************************************************************************************/

String_32* PageSizesList::GetTextualDescription(PageId ThisId)
{
	PageSize* pPageSize = (PageSize*) this->GetHead();

	while (pPageSize != NULL)
	{
		if (pPageSize->GetPageId() == ThisId)
			return pPageSize->GetPageName();

		pPageSize = (PageSize*) this->GetNext(pPageSize);
	}

	ERROR3("PageSizesList::GetTextualDescription Specified page id not found");
	return NULL;
}


/********************************************************************************************

>	PageId PageSizesList::GetPageIdFromSize(INT32 Width INT32 Height)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/94
	Inputs:		Height 	height of page in millipoints.
				Width 	width of page in millipoints.
	Returns:	PageId	the Id for the specified page.
	Purpose:	Returns the name for the page size specified or CUSTOM if not found.
	SeeAlso:	PageSize::GetSizeFromPageId()

********************************************************************************************/

PageId PageSizesList::GetPageIdFromSize(INT32 Width, INT32 Height)
{
	PageSize* pPageSize = (PageSize*) this->GetHead();

	while (pPageSize != NULL)
	{
		// Must give a little leeway either side as otherwise rounding errors may
		// raise their ugly heads and mean that 841891 != 841892 and hence miss the page size
		INT32 bodge = (INT32)(0.5 * MM_MP_VAL);	// extra leeway on comparison required
		INT32 FoundWidth = pPageSize->GetPageWidth();
		INT32 FoundHeight = pPageSize->GetPageHeight(); 
		if ( (FoundWidth >= (Width - bodge)) && (FoundWidth <= (Width + bodge)) &&
			 (FoundHeight  >= (Height - bodge)) && (FoundHeight  <= (Height + bodge)) )
			return pPageSize->GetPageId();

		pPageSize = (PageSize*) this->GetNext(pPageSize);
	}

	return CUSTOM;
}

/********************************************************************************************

>	BOOL PageSizesList::GetSizeFromPageId(PageId ThisId, INT32 *Width, INT32 *Height)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/94
	Inputs:		PageId - id of the page that we are searching for
	Returns:	Width	the width for the specified page in millipoints.
				Height	the height for the specified page in millipoints.
	Purpose:	Returns the width and height the page size specified.
	SeeAlso:	PageSize::GetPageIdFromSize()

********************************************************************************************/

BOOL PageSizesList::GetSizeFromPageId(PageId ThisId, INT32 *Width, INT32 *Height)
{
	// Set up default values in case of early exit
	*Width = 0;
	*Height = 0;

	PageSize* pPageSize = (PageSize*) this->GetHead();

	while (pPageSize != NULL)
	{
		if (pPageSize->GetPageId() == ThisId)
		{
			*Width = pPageSize->GetPageWidth();			// return found page width
			*Height = pPageSize->GetPageHeight();		// return found page height
			return TRUE;
		}
		pPageSize = (PageSize*) this->GetNext(pPageSize);
	}

	// Did not find the specified page 
	return FALSE;
}



/********************************************************************************************
*** Reading page sizes from the resource or from an ini file							*****
********************************************************************************************/




/********************************************************************************************

> BOOL PageSizesList::ReadPageSizes()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/11/95
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if OK, FALSE otherwise
	Purpose:	Reads the page size definitions from somewhere (either file or bound-in resource file).
	SeeAlso:	HotKey::ReadHotKeys;

********************************************************************************************/

BOOL PageSizesList::ReadPageSizes()
{
	BOOL ok = PageSizesList::ReadPageSizesFromDisk();

	if (!ok) ok = PageSizesList::ReadPageSizesFromRes();

	return (ok);
}

/********************************************************************************************

> BOOL PageSizesList::ReadPageSizesFromDisk()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/11/95
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if OK, FALSE otherwise
	Purpose:	Reads the page size definitions from disk (i.e. writable media)
	SeeAlso:	HotKey::ReadHotKeysFromDisk;

********************************************************************************************/

BOOL PageSizesList::ReadPageSizesFromDisk()
{	
	// Not yet implemented

	return FALSE;
}

/********************************************************************************************

> BOOL PageSizesList::ReadPageSizesFromRes()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/11/95
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if OK, FALSE otherwise
	Purpose:	Reads the hot key definitions from the bound in resource.
	SeeAlso:	HotKey::ReadHotKeysFromRes;

********************************************************************************************/

BOOL PageSizesList::ReadPageSizesFromRes()
{
	// Resource File
	CCResTextFile file;				

	// Open resource
	BOOL ok = file.open(_R(IDH_DEFAULT_PAGESIZES), _R(IDT_CAM_PAGESIZES_RES));

	if (ok)
	{
		ok = PageSizesList::ReadPageSizesFromFile(file);
		file.close();
	}

	return (ok);
}

/********************************************************************************************

> BOOL PageSizesList::ReadPageSizesFromFile(CCLexFile& file)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/11/95
	Inputs:		file = file that contains the page size definitions
	Outputs:	-
	Returns:	TRUE if OK, FALSE otherwise
	Purpose:	Reads the page size definitions from the given file
	SeeAlso:	HotKey::ReadHotKeysFromFile;

********************************************************************************************/

BOOL PageSizesList::ReadPageSizesFromFile(CCLexFile& file)
{
	BOOL finished = FALSE;
	BOOL ok;;	

	// Initialise lexing routines, and aspects of the lexer
	ok = file.InitLexer();
	file.SetDelimiters("\r\n");			// Set token delimiting characters
	file.SetCommentMarker(';');			// Set comment marker char
	file.SetWhitespace(" \t");			// Set whitespace chars
	file.SetStringDelimiters("\"\"");	// Set string delimiters

	PsTokenIndex Token;
	const TCHAR* TokenBuf = file.GetTokenBuf();	// Token buffer remains constant until lexer deinitialisation
	
	while (ok && !finished)
	{
		// Grab a token
		ok = file.GetSimpleToken();

		if (ok)
		{
			// Look the token up in our table
			Token = FindToken(TokenBuf);

			switch (Token)
			{
				case TOKEN_PAGESIZE:
					// Correst line start found so read in the page size info from that line
					ok = PageSizesList::ReadPageSizesList(file);
					break;

				case TOKEN_PAGESIZEEND:
					// End of table marker found so our job is over
					finished = TRUE;
					break;

				default:
					TRACEUSER( "Neville, _T(""PageSizesList: Unexpected token - %s\n"),TokenBuf);
					ErrorId = _R(IDW_PAGES_UNEXPECTEDMAINTOKEN);
					ok = FALSE;
					break;
			}
		}
		else
			ErrorId = _R(IDW_PAGES_FAILEDTOGETTOKEN);
	}

	if (!ok)
	{
		TRACEUSER( "Neville", _T("\nPageSizesList: Offending line - %s\n"),file.GetLineBuf());
		//ERROR3("Error reading page sizes.  See TRACE output for details");
	}

	// We are now finished with the lexer
	file.DeinitLexer();

	return (ok);
}

/********************************************************************************************

> BOOL PageSizesList::ReadPageSizesList(CCLexFile& file)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/11/95
	Inputs:		file = file that contains the page size definitions
	Outputs:	-
	Returns:	TRUE if OK, FALSE otherwise
	Purpose:	Read one page size definition from the given file
	SeeAlso:	-

********************************************************************************************/

BOOL PageSizesList::ReadPageSizesList(CCLexFile& file)
{
	PageSize* 	pPageSize	= NULL;
	String_32 	PageName;
	double		Width		= 0.0;
	double 		Height		= 0.0; 
	UnitType	Units 		= NOTYPE;

	BOOL ok = PageSizesList::ReadPageSizeDef(file, &PageName, &Width, &Height, &Units);

	if (ok)
	{
		// First size is implied to be custom i.e. if unknown size.
		DeclarePageSize((PageId)MaxPageId, &PageName, Width, Height, Units, &pPageSize);

		// Now increment are next new page id
		MaxPageId++;

		if (!ok)
		{
			ErrorId = _R(IDW_PAGES_FAILEDTOADDPAGE);
			TRACEUSER( "Neville", _T("PageSizesList: Unable to add page size to the system\n"));
		}
	}

	return (ok);
}

/********************************************************************************************

> BOOL HotKey::ReadPageSizeDef(	CCLexFile& file,
										String_32* pPageName,
										double* pWidth, 
										double* pHeight,
										UnitType* pUnits)


	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/11/95
	Inputs:		file 			= file that contains the key definitions
				ppKeyPress 		= Place to put the ptr to a key press object
				pOpToken   		= ptr to string to place the op token string
				ppTextDesc		= ptr to a ptr to a String_32 object that describes the key def (e.g. "Ctrl+X")
				pCheckUnicode	= Flag to determine how the hot key is compared with key press objects

	Outputs:	*ppKeyPress 	= a ptr to a key press object OR NULL
				*pOpToken   	= op token of op associated with the key press object OR empty string
				*ppTextDesc		= ptr to a String_32 describing the key combination.
							  	  If this is NULL, then the key combo has no text description
				*pCheckUnicode	= TRUE if Unicode checking should be done, FALSE otherwise

	Returns:	TRUE if OK
				FALSE otherwise (syntax error, not enough memory, etc)
	Purpose:	Generates a key press object and op token from the given file
	SeeAlso:	HotKey::ReadKeyDef

********************************************************************************************/

BOOL PageSizesList::ReadPageSizeDef(CCLexFile& file,
									String_32* pPageName,
									double* pWidth, 
									double* pHeight,
									UnitType* pUnits)
{
	// Check the parameters
	ERROR2IF(pPageName == NULL,	FALSE,"PageSizesList::ReadPageSizeDef pPageName is NULL");
	ERROR2IF(pWidth == NULL,	FALSE,"PageSizesList::ReadPageSizeDef pWidth is NULL");
	ERROR2IF(pHeight == NULL,	FALSE,"PageSizesList::ReadPageSizeDef pHeight is NULL");
	ERROR2IF(pUnits == NULL,	FALSE,"PageSizesList::ReadPageSizeDef pUnits is NULL");

	// Fill in some useful default values
	*pPageName 				= "";
	*pWidth 				= 0.0;
	*pHeight 				= 0.0;
	*pUnits 				= MILLIMETRES;	// default to reading in measurements in millimetres
	
	PsTokenIndex Token;
	const TCHAR* TokenBuf 	= file.GetTokenBuf();

	// This set of parameters will combine to form a key press object that represents
	// the hot key combination
	String_32* pTextDesc 	= NULL;

	// This is FALSE until we have read the text that describes the page size
	BOOL TextDescRead 		= FALSE;
	// This is FALSE until we have read the first number in, which should be the width
	BOOL ReadWidth 			= FALSE;

	// We haven't finsihed, but we're OK at the mo
	BOOL finished 			= FALSE;
	BOOL ok 	  			= TRUE;

	while (ok && !finished)
	{
		// Get the next token
		ok = file.GetSimpleToken();

		if (ok)
		{
			// Find out the type of the token
			LexTokenType TokenType = file.GetTokenType();

			switch (TokenType)
			{
				case TOKEN_STRING:
					{
						// Read that text into the name of the page as this is the only 
						// string that is expected
						// Make sure the desccription is not too long and there is one
						INT32 length = lstrlen(TokenBuf); 
						ok = ((length <= pPageName->MaxLength()) && (length > 0) );

						if (ok)
						{
							// Only take as much as we can take
							//TokenBuf.Left(pPageName, pPageName->MaxLength());					
							*pPageName = TokenBuf;
							
							// We have read the text desc string
							TextDescRead = TRUE;
						}
						else
						{
							TRACEUSER( "Neville", _T("PageSizesList::ReadPageSizeDef blank or too long a page size name!"));
							ErrorId = _R(IDW_PAGES_BADPAGENAME);
						}
						
					}
					break;

				case TOKEN_NORMAL:
					{
						Token = FindToken(TokenBuf);
						switch (Token)
						{
							// if we find either of the units then this is the last item
							// so this page size is all read in
							case TOKEN_MILLIMETRES:
								*pUnits = MILLIMETRES;
								finished = TRUE;
								break;
							case TOKEN_INCHES:
								*pUnits = INCHES;
								finished = TRUE;
								break;

							case TOKEN_NONE:
								{
									double Number;
									// Could use this but the routine requires a valid doc!
									//String_256 Value;
									//Value = TokenBuf;
									//ok = Convert::StringToDouble( &Value, &Number);
									// Could use this but is not unicode compliant!
									//Number = atof((char *)TokenBuf);
									ok = (_stscanf(TokenBuf,"%lg",&Number) == 1);
									if (ok)
									{
										if (!ReadWidth)
										{
											ReadWidth = TRUE;
											*pWidth = Number;
										}
										else
										{
											//ok = (_stscanf(TokenBuf,"%g",pHeight) == 1);
											*pHeight = Number;
										}
									}
									else
									{
										ErrorId = _R(IDW_PAGES_BADPAGESIZE);
										TRACEUSER( "Neville", _T("PageSize: Expected a size (width/height) but got : '%s'\n"),TokenBuf);
									}
								}
								break;

							default:
								// Flag a bad token so that we stop parsing this line
								ok = FALSE;
								TRACEUSER( "Neville", _T("PageSize: Didn't expect to get this token ('%s') in the middle of a hot key def\n"),TokenBuf);
								ErrorId = _R(IDW_PAGES_BADTOKEN);
								break;
						}
					}
					break;

				default:
					// bad token so stop parsing this line
					TRACEUSER( "Neville", _T("PageSize: Unexpected token - %s\n"),TokenBuf);
					ok = FALSE;
					ErrorId = _R(IDW_PAGES_UNEXPECTEDTOKEN);
					break;
			}
		}
	}

	return (ok);
}
