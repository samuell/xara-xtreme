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
/********************************************************************************************

>	fontcomp.cpp

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	06/08/96
	Purpose:	Implementation of the document font component

********************************************************************************************/

// Just a brief summary of the horrors of the way in which Camelot handles fonts.
// 
// There seem to be three independent systems handling fonts. The font
// manager looks after installed fonts, specifically those used in
// documents. The font gallery deals with both installed fonts and those
// available (i.e. on the CD) and the text tool infobar deals with the
// installed font families.
// 
// Each of these systems enumerates the fonts seperately, independently and
// stores different information about the fonts. The biggedt gripe is with
// the font gallery. When a font gets dragged form the font gallery to a
// text story (to apply the font to the text story) the following steps are
// done:
// 
// From the font gallery we know the LOGFONT structure for the font. We
// remember the typeface name and whether its bold or italic (we store
// these as booleans, even though the weight has at least 10 different
// values). Then we ask the font manager for cache a font with the same
// typeface name (doesn't care which font, just any font with the same
// typeface name) and return a font handle. We then create a typeface
// attribute with this font handle and the bold, italic booleans we
// remembered.
// 
// The problem is that the font handle just points to any font with the
// same typeface name, _not_ the font that was originally dragged from the
// font manager. i.e. we could end up with the following situations.
// 
// Arial Bold Italic dragged from the font gallery
// Typeface attribute:
// 	font handle	-> Arial Regular
// 	bold		-> True
// 	italic		-> True
// 
// Arial Regular dragged from the font gallery
// Typeface attribute:
// 	font handle	-> Arial Bold Italic
// 	bold		-> False
// 	italic		-> False
// 
// (These do actually happen. The font found by the font manager when asked
// for a font with a particular typeface is the first font installed with
// that typeface. Try deinstalled the arial font family, install Arial Bold
// Italic and then the other members of that family. Then see what happens
// in the debug tree.)
// 
// Another point of evidence:
// create four text stories, drag Arial to the first, Arial Bold to the
// second, Arial Italic to the third and Arial Bold Italic to the fourth.
// Then look at the debug tree - the text stories all have typeface
// attributes, all with the same font handle but different bold and italic
// flags.
// 
// Summary - after dragging a font from the font gallery to the document
// there's absolutly no way to determine the name of the font originally
// dragged. 
// 
// Shame really, since this information is required by the new file format.
// The old .xar format gets around this by saving the typeface na,e and the
// two flags and assuming that there are never more than 4 fonts with the
// same typeface; one regular, one bold, one italic and one bold italic
// (great - we'll assume the problem doesn't exist).
// 
// So, how can we correct this, since for the new file format we require
// the full font name (we can't get this, remember?), the typeface name and
// the PANOSE number for the original font (which we don't know, since we
// don't know what the font originally dragged was).
// 
// Solution
// 	* ignore the full font name, just save out an empty string
// 	* save out the typeface name (at least we have something)
// 	* get the PANOSE number for the font we do have, alter the Weight
// 	  and Letterform members to take into account the bold and italic
// 	  flags.
// 
// On import we'll have to get a list of all the fonts with the typeface
// and choose the closest using a metric on the PANOSE numbers - this may
// work, but no guarentees. :(
// 
// In order to manage all this information and to provide a method of
// retained font information should we load a file with contains font
// definitions to fonts we don't have, I offer another method of handling
// fonts the FontComponent document component.
// 
// When importing a new format file font defintions get matched immediatly
// to one of the installed fonts using the font name, face name and PANOSE
// number. All this information is then added to the font list maintained
// by the FontComponent object attached to the document. Whe we reach a
// font typeface attribute we can just retrieve the necessary information
// and put it into the tree.
// 
// Whe exporting a new format file font definitions get requested by the
// font typeface attribute code. We first search the list for a matching
// item in the list, so we can immediatly get the relevant font information
// (in case we're dealiing with a font not installed on this machine).
// Otherwise we'll ask the font manager nicely for the typeface name and
// PANOSE number (We can't get the font name because its all so screwed
// up). Then we'll have to bodge the PANOSE nubmer using the IsBold and
// IsItalic flags in the typeface attribute. Finally we'll put this
// information into our list, write out a font definition record, store the
// record number along side the rest of the information about this font and
// return the record number.
// 
// Of course subsequent times we get asked for a font definition record we
// can search the list and return the record number of the original font
// definition record.
// 
// Before import and export we need to set all the record numbers to NULL
// to show that they're not valid (i.e. to allow multiple imports and
// exports to and from the same document).
// 
// Some of this information could be merged with the font manager (font
// name, face name, PANOSE number), some of this is document specific
// (record number).
// 
// There's bound to be some information I've missed out - so go and read
// the code and work out for yourself how its all held together. 
// 
// Ach.

#include "camtypes.h"

//#include "app.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "ccpanose.h"
//#include "fontbase.h"
#include "fontman.h"

#include "cxftags.h"
//#include "cxfrec.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "camfiltr.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "filtrres.h"	// _R(IDS_NATIVE_FONTWARNING)

//#include "doccomp.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "fontcomp.h"

#include "ttfonts.h"
//#include "atmfonts.h"

CC_IMPLEMENT_DYNAMIC(FontComponentClass, DocComponentClass)
CC_IMPLEMENT_DYNAMIC(FontComponentListItem, ListItem)
CC_IMPLEMENT_DYNAMIC(FontComponentList, List)
CC_IMPLEMENT_DYNAMIC(FontComponent, DocComponent)

// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW

/********************************************************************************************

>	BOOL FontComponentClass::Init()

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	06/08/96
	Inputs:		-
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	Registers the font component with the main application
	See Also:	class DocComponentClass

********************************************************************************************/

BOOL FontComponentClass::Init()
{
	// Instantiate a component class to register with the application
	FontComponentClass *pClass = new FontComponentClass;
	if (pClass==NULL)
	{
		return FALSE;
	}
	else
	{
		GetApplication()->RegisterDocComponent(pClass);
		return TRUE;
	}
}

/********************************************************************************************

>	BOOL FontComponentClass::AddComponent(BaseDocument *pDoc)

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	06/08/96
	Inputs:		pDoc	- pointer to the document to which we add a component to
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	Adds a font component to the specified document
	See Also:	class FontComponent

********************************************************************************************/

BOOL FontComponentClass::AddComponent(BaseDocument *pDoc)
{
	// check to see if this document already has a font component, if so we leave it alone
	if (pDoc->GetDocComponent(CC_RUNTIME_CLASS(FontComponent)) != NULL)
	{
		return TRUE;
	}
	else
	{
		// no document component - try to create one for this document
		FontComponent *pFontComponent = new FontComponent;

		// now add this to the document
		pDoc->AddDocComponent(pFontComponent);

		// success!
		return TRUE;
	}
}

/********************************************************************************************

>	FontComponentListItem::FontComponentListItem()

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/08/96
	Inputs:		-
	Returns:	-
	Purpose:	Constructor
	See Also:	class FontComponentList, class FontComponent

********************************************************************************************/

FontComponentListItem::FontComponentListItem()
{
	mIsBold = FALSE;
	mIsItalic = FALSE;
	mRecentlyCreated = TRUE;		// set to FALSE before any imports or exports
	mFontHandle = ILLEGALFHANDLE;
	mRecordNumber = 0;
	mFullFontName = _T("");
	mTypeFaceName = _T("");
	mFontClass = FC_UNDEFINED;
	mPanoseNumber.SetAllToAny();
}

/********************************************************************************************

>	FontComponentListItem::~FontComponentListItem()

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/08/96
	Inputs:		-
	Returns:	-
	Purpose:	Destructor
	See Also:	class FontComponentList, class FontComponent

********************************************************************************************/

FontComponentListItem::~FontComponentListItem()
{
}

/********************************************************************************************

>	FontComponentList::FontComponentList()

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/08/96
	Inputs:		-
	Returns:	-
	Purpose:	Constructor
	See Also:	class FontComponent

********************************************************************************************/

FontComponentList::FontComponentList()
{
	ExportInitialised=FALSE;
	ImportInitialised=FALSE;
}

/********************************************************************************************

>	FontComponentList::~FontComponentList()

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/08/96
	Inputs:		-
	Returns:	-
	Purpose:	Destructor
	See Also:	class FontComponent

********************************************************************************************/

FontComponentList::~FontComponentList()
{
	ERROR3IF(ExportInitialised==TRUE, "Deconstructor called when ExportInitialised==TRUE");
	ERROR3IF(ImportInitialised==TRUE, "Deconstructor called when ImportInitialised==TRUE");
	
	DeleteAll();
}

/********************************************************************************************

>	BOOL FontComponentList::ExportStart()

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/08/96
	Inputs:		-
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	Gives objects of this type a chance to prepare for an export operation.
	See Also:	class FontComponent

********************************************************************************************/

BOOL FontComponentList::ExportStart()
{
	ERROR2IF(ExportInitialised!=FALSE, FALSE, "ExportStart called when ExportInitialised!=FALSE.");

	FontComponentListItem *pItem = (FontComponentListItem *) GetHead();

	// need to set the mRecentlyCreated and mRecordNumber member of all the elements in the list to NULL
	while (pItem!=NULL)
	{
		pItem->mRecentlyCreated = FALSE;
		pItem->mRecordNumber = 0;
		pItem = (FontComponentListItem *) GetNext(pItem);
	}

	ExportInitialised = TRUE;

	return TRUE;
}

/********************************************************************************************

>	BOOL FontComponentList::ExportEnd(BOOL Success)

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/08/96
	Inputs:		Success - succcess status of the export operation
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	Gives objects if this class a chance to recover from an export operation,
				tidying up if neccessary.
	See Also:	class FontComponent

********************************************************************************************/

BOOL FontComponentList::ExportEnd(BOOL Success)
{
	//ERROR2IF(ExportInitialised==FALSE, FALSE, "ExportEnd called when ExportInitialised==FALSE.");
	// If we error about the ExportInitialised then we assume that the StartExport has been called.
	// This may not be the case if we are in clean up mode. So just handle it!

	if (Success==FALSE && ExportInitialised)
	{
		// export operation was unsuccessful, so need to delete all the entries in the list
		// made during the export. this is easy; since the mRecentlyCreated member of all the
		// elements in the list was set to FALSE before the export any element with an non-FALSE
		// mRecentlyCreated member.

		FontComponentListItem *pItem = (FontComponentListItem *) GetHead();
		FontComponentListItem *qItem;

		while (pItem!=NULL)
		{
			qItem = pItem;
			pItem = (FontComponentListItem *) GetNext(qItem);

			if (qItem->mRecentlyCreated==TRUE)
			{
				RemoveItem(qItem);
				delete qItem;
			}
		}
	}
	//else // Removed due to the change to the if statement above
	//{
	//	// oh well, the export operation was successful, so do nothing
	//}

	ExportInitialised=FALSE;
	return TRUE;
}

/********************************************************************************************

>	FontComponentListItem *FontComponentList::ExportFind(WORD FontHandle, BOOL IsBold, BOOL IsItalic)

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/08/96
	Inputs:		FontHandle	: font handle to search for
				IsBold		: bold flag to search for
				IsItalic	: italic flag to search for
	Returns:	Pointer to the matching FontComponentListItem if found, or NULL
	Purpose:	Searches the list of FontComponentListItem's for a match
	See Also:	class FontComponentListItem

********************************************************************************************/

FontComponentListItem *FontComponentList::ExportFind(WORD FontHandle, BOOL IsBold, BOOL IsItalic)
{
	ERROR2IF(ExportInitialised==FALSE, FALSE, "ExportFind called when ExportInitialised==FALSE.");

	// auxillary functions, searches using tail recursion. the msvc compiler should
	// optimise this quite well - at least, it should
	return ExportFindAux((FontComponentListItem *) GetHead(), FontHandle, IsBold, IsItalic);
}

/********************************************************************************************

>	FontComponentListItem *FontComponentList::ExportFindAux(FontComponentListItem *pItem, WORD FontHandle, BOOL IsBold, BOOL IsItalic)

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/08/96
	Inputs:		pItem		: current FontComponentListItem of interest
				FontHandle	: font handle to search for
				IsBold		: bold flag to search for
				IsItalic	: italic flag to search for
	Returns:	Pointer to a matching FontComponentListItem, or null if not found
	Purpose:	Searches the list of FontComponentListItems for match
	See Also:	class FontComponentListItem, class FontComponent

********************************************************************************************/

FontComponentListItem *FontComponentList::ExportFindAux(FontComponentListItem *pItem, WORD FontHandle, BOOL IsBold, BOOL IsItalic)
{
	ERROR2IF(ExportInitialised==FALSE, FALSE, "ExportFindAux called when ExportInitialised==FALSE.");
	ERROR2IF(FontHandle==ILLEGALFHANDLE, FALSE, "ExportFindAux called with an illegal font handle.");

	if (pItem==NULL)
	{
		return NULL;
	}
	else if (pItem->mFontHandle==FontHandle && pItem->mIsBold==IsBold && pItem->mIsItalic==IsItalic)
	{
		return pItem;
	}
	else
	{
		// tail recursion ;)
		return ExportFindAux((FontComponentListItem *) GetNext(pItem), FontHandle, IsBold, IsItalic);
	}
}

/********************************************************************************************

>	BOOL FontComponentList::ImportStart()

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/08/96
	Inputs:		-
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	Allows objects of this class a chance to initialise before an import operation
	See Also:	class FontComponent

********************************************************************************************/

BOOL FontComponentList::ImportStart()
{
	ERROR2IF(ImportInitialised!=FALSE, FALSE, "ImportStart called when ImportInitialised!=FALSE");

	FontComponentListItem *pItem = (FontComponentListItem *) GetHead();

	// need to set the mRecentlyCreated and mRecordNumber members of all the elements in the list
	// to FALSE and NULL respectively.
	while (pItem!=NULL)
	{
		pItem->mRecentlyCreated=FALSE;
		pItem->mRecordNumber = 0;
		pItem = (FontComponentListItem *) GetNext(pItem);
	}

	ImportInitialised=TRUE;

	return TRUE;
}

/********************************************************************************************

>	BOOL FontComponentList::ImportEnd(BOOL Success)

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/08/96
	Inputs:		Success		- success status of the import operation
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	Allows objects of this class a chance to recover from an import operation
	See Also:	class FontComponent

********************************************************************************************/

BOOL FontComponentList::ImportEnd(BOOL Success)
{
	//ERROR2IF(ImportInitialised==FALSE, FALSE, "ImportEnd called when ImportInitialised==FALSE.");
	// If we error about the ImportInitialised then we assume that the StartExport has been called.
	// This may not be the case if we are in clean up mode. So just handle it!

	if (Success==FALSE && ImportInitialised)
	{
		// import operation was not successful, so we need to delete any elements in the list
		// created during the import. this is easy since all the elements in the list had
		// their mRecentlyCreated member set to FALSE before the import - any new elements will
		// have mRecentlyCreated==TRUE (thanks to the FontComponentListItem constructor).

		FontComponentListItem *pItem = (FontComponentListItem *) GetHead();
		FontComponentListItem *qItem = NULL;

		while (pItem!=NULL)
		{
			qItem = pItem;
			pItem = (FontComponentListItem *) GetNext(qItem);

			if (qItem->mRecentlyCreated==TRUE)
			{
				RemoveItem(qItem);
				delete qItem;
			}
		}
	}
	//else // Removed due to the change to the if statement above
	//{
	//	// import operation was successful, so just return.
	//}

	ImportInitialised=FALSE;
	return TRUE;
}

/********************************************************************************************

>	FontComponentListItem *FontComponentList::ImportFind(INT32 RecordNumber)

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/08/96
	Inputs:		RecordNumber	- record number to search for
	Returns:	pointer to the matching FontComponentListItem, or NULL if not found.
	Purpose:	Searches the list for a matching FontComponentListItem.
	See Also:	class FontComponentListItem, class FontComponent

********************************************************************************************/

FontComponentListItem *FontComponentList::ImportFind(INT32 RecordNumber)
{
	ERROR2IF(ImportInitialised==FALSE, FALSE, "ImportFind callled when ImportInitialied==FALSE.");

	return ImportFindAux((FontComponentListItem *) GetHead(), RecordNumber);
}

/********************************************************************************************

>	FontComponentListItem *FontComponentList::ImportFindAux(FontComponentListItem *pItem, INT32 RecordNumber)

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/08/96
	Inputs:		pItem			- current FontComponentListItem of interest
				RecordNumber	- record number to search for
	Returns:	pointer to the matching FontComponentListItem or NULL if not found
	Purpose:	Searches the list for a matching FontComponentListItem
	See Also:	class FontComponentListItem, class FontComponent

********************************************************************************************/

FontComponentListItem *FontComponentList::ImportFindAux(FontComponentListItem *pItem, INT32 RecordNumber)
{
	ERROR2IF(ImportInitialised==FALSE, FALSE, "ImportFindAux called when ImportInitialised==FALSE.");

	if (pItem==NULL)
	{
		return NULL;
	}
	else if (pItem->mRecordNumber==RecordNumber)
	{
		return pItem;
	}
	else
	{
		// tail recursion ;)
		return ImportFindAux((FontComponentListItem *) GetNext(pItem), RecordNumber);
	}
}

/********************************************************************************************

>	FontComponent::FontComponent()

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/08/96
	Inputs:		-
	Returns:	-
	Purpose:	Constructor
	See Also:	class FontComponent

********************************************************************************************/

FontComponent::FontComponent()
{
	// constructor for this class, ho hum,
	ExportInitialised = FALSE;
	ImportInitialised = FALSE;

	pFontList = new FontComponentList;
	pFontManager = GetApplication()->GetFontManager();

	// Our link to the BaseCamelotFilter
	pCamFilter = NULL;
	// Set our variable so that we warn about a possible replacement only once
	WarnedBefore = FALSE;
}

/********************************************************************************************

>	FontComponent::~FontComponent()

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/08/96
	Inputs:		-
	Returns:	-
	Purpose:	Destructor
	See Also:	class FontComponent

********************************************************************************************/

FontComponent::~FontComponent()
{
	ERROR3IF(ImportInitialised!=FALSE, "Deconstructor called when ImportInitialised!=FALSE");
	ERROR3IF(ExportInitialised!=FALSE, "Deconstructor called when ExportInitialised!=FALSE");

	if (pFontList)
	{
		delete pFontList;
		pFontList = NULL;
	}
}

/********************************************************************************************

>	BOOL FontComponent::StartExport(BaseCamelotFilter *pFilter)

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/08/96
	Inputs:		-
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	Allows the font component a chance to prepare for an export operation
	See Also:	class FontComponent

********************************************************************************************/

BOOL FontComponent::StartExport(BaseCamelotFilter *pFilter)
{
	ERROR2IF(ExportInitialised!=FALSE, FALSE, "StartExport called when ExportInitialised!=FALSE");
	ERROR2IF(pFontList==NULL, FALSE, "StartExport called when pFontList==NULL.");

	BOOL ok = TRUE;

	if (ok) ok = pFontList->ExportStart();
	if (ok) ExportInitialised=TRUE;

	return ok;
}

/********************************************************************************************

>	BOOL FontComponent::EndExport(BaseCamelotFilter *pFilter, BOOL Success)

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/08/96
	Inputs:		-
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	Allows the font component a chance to recover from an export operation
	See Also:	class FontComponent

********************************************************************************************/

BOOL FontComponent::EndExport(BaseCamelotFilter *pFilter, BOOL Success)
{
	//ERROR2IF(ExportInitialised==FALSE, FALSE, "EndExport called when ExportInitialised==FALSE.");
	//ERROR2IF(pFontList==NULL, FALSE, "EndExport called when pFontList==NULL.");
	// If we error about the pFontList and ExportInitialised then we assume that the StartExport has been called.
	// This may not be the case if we are in clean up mode. So just handle it!

	BOOL ok = TRUE;

	if (ok && pFontList) ok = pFontList->ExportEnd(Success);
	if (ok) ExportInitialised=FALSE;

	return ok;
}

/********************************************************************************************

>	INT32 FontComponent::WriteFontDefinition(BaseCamelotFilter *pFilter, WORD FontHandle, BOOL IsBold, BOOL IsItalic)

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/08/96
	Inputs:		pFilter		- filter to write font defintion record to
				FontHandle	- font handle of font definition to write
				IsBold		- bold flag of font definition
				IsItalic	- italic flag of font definition
	Returns:	RecordNumber of font definition, or NULL if unsuccessful
	Purpose:	Writes a font definition record to the filter, and returns with the record number
				of this font definition record.
	See Also:	class FontComponent, class FontComponentList, class FontComponentListItem

********************************************************************************************/

INT32 FontComponent::WriteFontDefinition(BaseCamelotFilter *pFilter, WORD FontHandle, BOOL IsBold, BOOL IsItalic)
{
	ERROR2IF(ExportInitialised==FALSE, FALSE, "EndExport called when ExportInitialised==FALSE.");
	ERROR2IF(pFilter==NULL, FALSE, "Parameter pFilter==NULL.");
	ERROR2IF(FontHandle==ILLEGALFHANDLE, FALSE, "Parameter FontHandle==ILLEGALFHANDLE.");
	ERROR2IF(pFontList==NULL, FALSE, "member variable pFontList==NULL.");

	INT32 RecordNumber = 0;
	FontComponentListItem *pItem;
	
	// is this item already cached?
	pItem = pFontList->ExportFind(FontHandle, IsBold, IsItalic);

	if (pItem==NULL)
	{
		// first time we've come across this font, so cache some information about it.
		pItem = new FontComponentListItem;
		pItem->mFontHandle = FontHandle;
		pItem->mIsBold = IsBold;
		pItem->mIsItalic = IsItalic;

		(void) ExportCacheFont(pItem);		// cache the rest of the information about this font

		pFontList->AddTail(pItem);
	}

	// by now, pItem!=NULL, otherwise we've got serious problems

	if (pItem!=NULL)
	{
		// have we already written a font defintion record for this font? i.e. is
		// mRecordNumber!=NULL?
		if (pItem->mRecordNumber != 0)
		{
			RecordNumber = pItem->mRecordNumber;
		}
		else
		{
			RecordNumber = WriteFontRecord(pFilter, pItem);
			pItem->mRecordNumber=RecordNumber;
		}
	}
	else
	{
		ERROR3("pItem==NULL, and it shouldn't!");
		RecordNumber=0;
	}

	return RecordNumber;
}

/********************************************************************************************

>	INT32 FontComponent::WriteFontRecord(BaseCamelotFilter *pFilter, FontComponentListItem *pItem)
	INT32 FontComponent::WriteTrueTypeRecord(BaseCamelotFilter *pFilter, FontComponentListItem *pItem)
	INT32 FontComponent::WriteATMRecord(BaseCamelotFilter *pFilter, FontComponentListItem *pItem)
	INT32 FontComponent::WriteUnknownRecord(BaseCamelotFilter *pFilter, FontComponentListItem *pItem)

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/08/96
	Inputs:		pFilter		- filter to write
				pItem		- fontto write a definition for
	Returns:	Record number of font definition record, or 0 if unsuccessful
	Purpose:	Writes a font definition record to the filter, and returns with the record number
	See Also:	class FontComponent, class FontComponentListItem

********************************************************************************************/

INT32 FontComponent::WriteFontRecord(BaseCamelotFilter *pFilter, FontComponentListItem *pItem)
{
	ERROR2IF(ExportInitialised==FALSE, FALSE, "EndExport called when ExportInitialised==FALSE.");
	ERROR2IF(pFilter==NULL, 0, "Parameter pFilter==NULL.");
	ERROR2IF(pItem==NULL, 0, "Parameter pItem==NULL.");

	INT32 RecordNumber;
	
	switch (pItem->mFontClass)
	{
		case FC_TRUETYPE		: RecordNumber = WriteTrueTypeRecord(pFilter, pItem); break;
		case FC_ATM				: RecordNumber = WriteATMRecord(pFilter, pItem); break;
		default					: RecordNumber = WriteUnknownRecord(pFilter, pItem); break;
	}

	return RecordNumber;
}

INT32 FontComponent::WriteTrueTypeRecord(BaseCamelotFilter *pFilter, FontComponentListItem *pItem)
{
	ERROR2IF(ExportInitialised==FALSE, FALSE, "EndExport called when ExportInitialised==FALSE.");
	ERROR2IF(pFilter==NULL, 0, "Parameter pFilter==NULL.");
	ERROR2IF(pItem==NULL, 0, "Parameter pItem==NULL.");

	BOOL ok = TRUE;
	INT32 RecordNumber = 0;

	CamelotFileRecord Rec(pFilter, TAG_FONT_DEF_TRUETYPE, TAG_FONT_DEF_TRUETYPE_SIZE);

	ok = Rec.Init();

	if (ok) ok = Rec.WriteUnicode((TCHAR*)pItem->mFullFontName);
	if (ok) ok = Rec.WriteUnicode((TCHAR*)pItem->mTypeFaceName);
	if (ok) ok = Rec.WriteCCPanose(pItem->mPanoseNumber);

	if (ok) ok = (0 != (RecordNumber = pFilter->WriteDefinitionRecord(&Rec)));
	
	// if ok==TRUE, then by the above line RecordNumber!=NULL. if ok==FALSE then
	// RecordNumber==NULL. so we don't need to check ok at this point.
	return RecordNumber;
}

INT32 FontComponent::WriteATMRecord(BaseCamelotFilter *pFilter, FontComponentListItem *pItem)
{
	ERROR2IF(ExportInitialised==FALSE, FALSE, "EndExport called when ExportInitialised==FALSE.");
	ERROR2IF(pFilter==NULL, 0, "Parameter pFilter==NULL.");
	ERROR2IF(pItem==NULL, 0, "Parameter pItem==NULL.");

	BOOL ok = TRUE;
	INT32 RecordNumber = 0;

	CamelotFileRecord Rec(pFilter, TAG_FONT_DEF_ATM, TAG_FONT_DEF_ATM_SIZE);

	ok = Rec.Init();

	if (ok) ok = Rec.WriteUnicode((TCHAR*)pItem->mFullFontName);
	if (ok) ok = Rec.WriteUnicode((TCHAR*)pItem->mTypeFaceName);
	if (ok) ok = Rec.WriteCCPanose(pItem->mPanoseNumber);

	if (ok) ok = (0 != (RecordNumber = pFilter->WriteDefinitionRecord(&Rec)));

	// if ok==TRUE, then by the above line RecordNumber!=NULL. if ok==FALSE then
	// RecordNumber==NULL. so we don't need to check ok at this point.
	return RecordNumber;
}

INT32 FontComponent::WriteUnknownRecord(BaseCamelotFilter *pFilter, FontComponentListItem *pItem)
{
	ERROR2IF(ExportInitialised==FALSE, FALSE, "EndExport called when ExportInitialised==FALSE.");
	ERROR2IF(pFilter==NULL, 0, "Parameter pFilter==NULL.");
	ERROR2IF(pItem==NULL, 0, "Parameter pItem==NULL.");

	ERROR3("Attempt to write an font definition for an unknown file type.");
	
	return 0;
}

/********************************************************************************************

>	BOOL FontComponent::StartImport(BaseCamelotFilter *pFilter)

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/08/96
	Inputs:		pFilter	- filter to use
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	Gives objects of type class FontComponent a chance to prepare themselves for
				an import operation
	See Also:	class FontComponent

********************************************************************************************/

BOOL FontComponent::StartImport(BaseCamelotFilter *pFilter)
{
	ERROR2IF(ImportInitialised!=FALSE, FALSE, "StartImport called when ImportInitialised!=FALSE.");
	ERROR2IF(pFontList==NULL, FALSE, "StartImport called when pFontList==NULL.");

	// Save this in our link to the BaseCamelotFilter
	pCamFilter = pFilter;
	// Set our variable so that we warn about a possible replacement only once
	WarnedBefore = FALSE;

	BOOL ok = TRUE;

	if (ok) ok = pFontList->ImportStart();
	if (ok) ImportInitialised=TRUE;

	return ok;
}

/********************************************************************************************

>	BOOL FontComponent::EndImport(BaseCamelotFilter *pFilter, BOOL Success)

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/08/96
	Inputs:		pFilter - filter to use
				Success	- TRUE if import operation was successful, FALSE otherwise
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	Gives this class a chance to recover from an import operation
	See Also:	class FontComponent

********************************************************************************************/

BOOL FontComponent::EndImport(BaseCamelotFilter *pFilter, BOOL Success)
{
	//ERROR2IF(ImportInitialised==FALSE, FALSE, "EndImport called when ImportInitialised==FALSE.");
	//ERROR2IF(pFontList==NULL, FALSE, "EndImport called when pFontList==NULL.");
	// If we error about the pFontList and ImportInitialised then we assume that the EndExport has been called.
	// This may not be the case if we are in clean up mode. So just handle it!

	BOOL ok = TRUE;

	if (ok && pFontList) ok = pFontList->ImportEnd(Success);
	if (ok) ImportInitialised=FALSE;

	// Null our link to the BaseCamelotFilter
	pCamFilter = NULL;
	// Set our variable so that we warn about a possible replacement only once
	WarnedBefore = FALSE;

	return ok;
}

/********************************************************************************************

>	BOOL FontComponent::ReadFontDefinition(CXaraFileRecord *pRec)
	BOOL FontComponent::ReadFontTrueType(CXaraFileRecord *pRec)
	BOOL FontComponent::ReadFontATM(CXaraFileRecord *pRec)
	BOOL FontComponent::ReadFontDefault(CXaraFileRecord *pRec)

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/08/96
	Inputs:		pRec	: pointer to a CXaraFileRecord to read
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	Reads a font component record from the filter
	See Also:	class FontComponent, class TextObjRecordHandler

********************************************************************************************/

BOOL FontComponent::ReadFontDefinition(CXaraFileRecord *pRec)
{
	ERROR2IF(pRec==NULL, FALSE, "Parameter pRec==NULL.");
	ERROR2IF(ImportInitialised==FALSE, FALSE, "ReadFontDefinition called when ImportInitialised==FALSE.");

	BOOL ok;

	switch (pRec->GetTag())
	{
		case TAG_FONT_DEF_TRUETYPE	: ok = ReadFontTrueType(pRec); break;
		case TAG_FONT_DEF_ATM		: ok = ReadFontATM(pRec); break;
		default						: ok = ReadFontUnknown(pRec); break;
	}

	return ok;
}

BOOL FontComponent::ReadFontTrueType(CXaraFileRecord *pRec)
{
	ERROR2IF(ImportInitialised==FALSE, FALSE, "ReadFontTrueType called when ImportInitialised==FALSE.");
	ERROR2IF(pRec==NULL, FALSE, "Parameter pRec==NULL.");
	ERROR2IF(pFontList==NULL, FALSE, "ReadFontTrueType called when pFontList==NULL.");

	BOOL ok = TRUE;

	TCHAR Buffer[256];
	FontComponentListItem *pItem = new FontComponentListItem;

	pItem->mRecordNumber = pRec->GetRecordNumber();

	pItem->mFontClass = FC_TRUETYPE;

	if (ok) ok = pRec->ReadUnicode(Buffer, 256);
	pItem->mFullFontName = Buffer;

	if (ok) ok = pRec->ReadUnicode(Buffer, 256);
	pItem->mTypeFaceName = Buffer;

	if (ok) ok = pRec->ReadCCPanose( &(pItem->mPanoseNumber) );

	PORTNOTE("text","FontComponent::ReadFontTrueType - removed ImportCacheFont call");
#ifndef DISABLE_TEXT_RENDERING
	if (ok) ok = ImportCacheFont(pItem);	// do all our clever matching

	if (ok) pFontList->AddTail(pItem);
#endif

	return ok;
}

BOOL FontComponent::ReadFontATM(CXaraFileRecord *pRec)
{
	ERROR2IF(ImportInitialised==FALSE, FALSE, "ReadFontATM called when ImportInitialised==FALSE.");
	ERROR2IF(pRec==NULL, FALSE, "Parameter pRec==NULL.");
	ERROR2IF(pFontList==NULL, FALSE, "ReadFontATM called when pFontList==NULL.");

	BOOL ok = TRUE;

	TCHAR Buffer[256];
	FontComponentListItem *pItem = new FontComponentListItem;

	pItem->mRecordNumber = pRec->GetRecordNumber();

	pItem->mFontClass = FC_ATM;

	if (ok) ok = pRec->ReadUnicode(Buffer, 256);
	pItem->mFullFontName = Buffer;

	if (ok) ok = pRec->ReadUnicode(Buffer, 256);
	pItem->mTypeFaceName = Buffer;

	if (ok) ok = pRec->ReadCCPanose( &(pItem->mPanoseNumber) );

	PORTNOTE("text","FontComponent::ReadFontATM - removed ImportCacheFont call");
#ifndef DISABLE_TEXT_RENDERING
	if (ok) ok = ImportCacheFont(pItem);	// do all our clever matching

	if (ok) pFontList->AddTail(pItem);
#endif
	return ok;
}

BOOL FontComponent::ReadFontUnknown(CXaraFileRecord *pRec)
{
	ERROR2IF(ImportInitialised==FALSE, FALSE, "ReadFontTrueType called when ImportInitialised==FALSE.");
	ERROR2IF(pRec==NULL, FALSE, "Parameter pRec==NULL.");
	ERROR2IF(pFontList==NULL, FALSE, "ReadFontTrueType called when pFontList==NULL.");

	ERROR3("Attempt to read an unrecognised font definition record.");

	return FALSE;
}

/********************************************************************************************

>	BOOL FontComponent::GetFontInformation(INT32 RecordNumber, WORD *pFontHandle, BOOL *pIsBold, BOOL *pIsItalic)

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/08/96
	Inputs:		RecordNumber	- Record number of font definition record
	Outputs:	pFontHandle		- pointer to a WORD to return the font maangers font handle in
				pIsBold			- pointer to a BOOL to return the bold flag in
				pIsItalic		- pointer to a BOOL to return the italic flag in
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	Retrives information from the font definition record with the corresponding record number
	See Also:	class FontComponent, class TextAttrRecordHandler

********************************************************************************************/

BOOL FontComponent::GetFontInformation(INT32 RecordNumber, WORD *pFontHandle, BOOL *pIsBold, BOOL *pIsItalic)
{
	ERROR2IF(ImportInitialised==FALSE, FALSE, "GetFontInformation called when ImportInitialised==FALSE.");
	ERROR2IF(pFontList==NULL, FALSE, "GetFontInformation called when pFontList==NULL.");
	ERROR2IF(pFontHandle==NULL, FALSE, "GetFontInformation called when pFontHandle==NULL.");
	ERROR2IF(pIsBold==NULL, FALSE, "GetFontInformation called when pIsBold==NULL.");
	ERROR2IF(pIsItalic==NULL, FALSE, "GetFontInformation called when pIsItalic==NULL.");

	BOOL ok;

	FontComponentListItem *pItem = pFontList->ImportFind(RecordNumber);

	if (pItem==NULL)
	{
		// oh dear, we can't find the font definition record - possibly an error in the file
		ok = TRUE;

		// We will warn the user and use a replacement font instead
		// If we have a pFilter then ask the default warning handle to append our message
		if (pCamFilter)
		{
			if (!WarnedBefore)
			{
				pCamFilter->AppendWarning(_R(IDS_NATIVE_FONTWARNING));
				WarnedBefore = TRUE;
			}
		}
		else
			ERROR3("FontComponent::GetFontInformation unkown font reference - using default");

		*pFontHandle = DEFAULTHANDLE;
		*pIsBold = FALSE;
		*pIsItalic = FALSE;
	}
	else
	{
		ok = TRUE;
		*pFontHandle = pItem->mFontHandle;
		*pIsBold = pItem->mIsBold;
		*pIsItalic = pItem->mIsItalic;
	}

	return ok;	
}

/********************************************************************************************

>	BOOL FontComponent::ExportCacheFont(FontComponentListItem *pItem)
	BOOL FontComponent::ExportCacheFullFontName(FontComponentListItem *pItem)
	BOOL FontComponent::ExportCacheTypeFaceName(FontComponentListItem *pItem)
	BOOL FontComponent::ExportCacheFontClass(FontComponentListItem *pItem)
	BOOL FontComponent::ExportCacheCCPanose(FontComponentListItem *pItem)

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/08/96
	Inputs:		pItem	: FontComponentListItem to complete
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	Caches information about a font in a FontComponentListItem object for export.
	See Also:	class FontComponentListItem, class FontComponent

********************************************************************************************/

BOOL FontComponent::ExportCacheFont(FontComponentListItem *pItem)
{
	ERROR2IF(pItem==NULL, FALSE, "Parameter pItem==NULL.");
	ERROR2IF(ExportInitialised==FALSE, FALSE, "ExportCacheFont called when ExportInitialised==FALSE.");
	ERROR2IF(pFontManager==NULL, FALSE, "ExportCacheFont called when pFontManager==NULL.");

	BOOL ok = TRUE;
	
	// members of class FontComponentListItem are:
	//		BOOL mIsBold					: should be already filled in	
	//		BOOL mIsItalic					: should be already filled in
	//		BOOL mRecentlyCreated			: leave alone
	//		WORD mFontHandle				: should be already filled in
	//		INT32 mRecordNumber				: leave alone
	//		String_64 mFullFontName			: set to a null string
	//		String_64 mTypeFaceName			: get from the font manager
	//		FontClass mFontClass			: get from the font manager
	//		CCPanose mPanoseNumber			: get from the font manager

	// the following order is important - don't change unless you need to. 		ach - 16/08/96
	if (ok) ok = ExportCacheFontClass(pItem);
	if (ok) ok = ExportCacheTypeFaceName(pItem);
	if (ok) ok = ExportCacheFullFontName(pItem);
	if (ok) ok = ExportCacheCCPanose(pItem);

	return ok;
}

BOOL FontComponent::ExportCacheFullFontName(FontComponentListItem *pItem)
{
	PORTNOTETRACE("text","FontComponent::ExportCacheFullFontName - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	ERROR2IF(pItem==NULL, FALSE, "ExportCacheFullFontName called with pItem==NULL.");

	OUTLINETEXTMETRIC *pOutlineTextMetric;

	if (NULL!=(pOutlineTextMetric=ExportGetOutlineTextMetricStage1(pItem)))
	{
		// successful!
		pItem->mFullFontName = (char *)(((INT32) pOutlineTextMetric) + ((INT32) pOutlineTextMetric->otmpFaceName));
	}
	else
	{
		if (NULL!=(pOutlineTextMetric=ExportGetOutlineTextMetricStage2(pItem)))
		{
			// successful!
			pItem->mFullFontName = (char *)(((INT32) pOutlineTextMetric) + ((INT32) pOutlineTextMetric->otmpFaceName));
			free(pOutlineTextMetric);	// remember to free this result
		}
		else
		{
			// unsuccessful. :(
			pItem->mFullFontName="";
		}
	}
#endif
	return TRUE;
}


BOOL FontComponent::ExportCacheTypeFaceName(FontComponentListItem *pItem)
{
	ERROR2IF(pItem==NULL, FALSE, "ExportCacheFaceName called with pItem==NULL.");
	ERROR2IF(pFontManager==NULL, FALSE, "pFontManager==NULL");
	ERROR2IF(pItem->mFontHandle==ILLEGALFHANDLE, FALSE, "pItem->mFontHandle==ILLEGALFHANDLE.");

	BOOL ok = TRUE;

	if (ok) ok = pFontManager->GetFontName(pItem->mFontHandle, pItem->mTypeFaceName);

	return ok;
}

BOOL FontComponent::ExportCacheFontClass(FontComponentListItem *pItem)
{
	ERROR2IF(pItem==NULL, FALSE, "ExportCacheFaceName called with pItem==NULL.");
	ERROR2IF(pFontManager==NULL, FALSE, "pFontManager==NULL");
	ERROR2IF(pItem->mFontHandle==ILLEGALFHANDLE, FALSE, "pItem->mFontHandle==ILLEGALFHANDLE.");

	BOOL ok = TRUE;

	pItem->mFontClass = pFontManager->GetFontClass(pItem->mFontHandle);

	return ok;
}

BOOL FontComponent::ExportCacheCCPanose(FontComponentListItem *pItem)
{
	ERROR2IF(pItem==NULL, FALSE, "ExportCacheFullFontName called with pItem==NULL.");

	OUTLINETEXTMETRIC *pOutlineTextMetric;

	if (NULL!=(pOutlineTextMetric=ExportGetOutlineTextMetricStage1(pItem)))
	{
		// successful!
		pItem->mPanoseNumber = pOutlineTextMetric->otmPanoseNumber;
	}
	else
	{
		if (NULL!=(pOutlineTextMetric=ExportGetOutlineTextMetricStage2(pItem)))
		{
			// successful!
			pItem->mPanoseNumber = pOutlineTextMetric->otmPanoseNumber;
			free(pOutlineTextMetric);	// remember to free this result
		}
		else
		{
			// unsuccessful. :(
			pItem->mPanoseNumber.SetAllToAny();
		}
	}

	pItem->mPanoseNumber.BodgePanoseNumber(pItem->mIsBold, pItem->mIsItalic);

	return TRUE;
}

/********************************************************************************************

>	OUTLINETEXTMETRIC *FontComponent::ExportGetOutlineTextMetricStage1(FontComponentListItem *pItem);
	OUTLINETEXTMETRIC *FontComponent::ExportGetOutlineTextMetricStage2(FontComponentListItem *pItem);

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/08/96
	Inputs:		pItem	- pointer to a FontComponentListItem to get the OUTLINETEXTMETRIC for
	Returns:	Pointer to an OUTLINETEXTMETRIC, or NULL if not found
	Purpose:	Tries to retrieve the OUTLINETEXTMETRIC structure for a font

	Be careful, since the pointer returned by these functions is (and should be) deleted by the
	caller.

********************************************************************************************/

OUTLINETEXTMETRIC *FontComponent::ExportGetOutlineTextMetricStage1(FontComponentListItem *pItem)
{
	PORTNOTETRACE("text","FontComponent::ExportGetOutlineTextMetricStage1 - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	ERROR2IF(pItem==NULL, NULL, "ExportGetOutlineTextMetricStage1 with pItem==NULL.");

	// stage 1 - does the font manager have the OUTLINETEXTMETRIC structure for this font?
	//
	// We'll ask it nicely and see what result it comes backwith. Because the font manager can
	// oly cope with one font per typeface (Doh!) we'll ask it for the OUTLINETEXTMETRIC for
	// the typeface of the font we're trying to cache, and see if it bears any resemblance to the
	// font we originally asked for.

	OUTLINETEXTMETRIC *pOutlineTextMetric = pFontManager->GetOutlineTextMetric(pItem->mFontHandle);

	if (pOutlineTextMetric!=NULL)
	{
		// see if it bears any resemblance to the one we want
		//
		// Here we notice Microsoft's documentation and code don't quite agree. The OUTLINETEXTMETRIC
		// structure has 4 strings at the end, the 'family name', 'type name', 'style name'
		// and 'full name'. However for, say, Arial Bold:
		//
		// family name = "Arial"			: what we call the typeface name
		// face name = "Arial Bold"			: what we call the full name
		// style name = "Bold"				: style name
		// full name = "blah blah blah" (not important).
		
		const String_64 TypeFaceName = (char *) (((INT32)pOutlineTextMetric) + ((INT32)pOutlineTextMetric->otmpFamilyName));
		const BOOL IsBold = pOutlineTextMetric->otmTextMetrics.tmWeight > FW_NORMAL;
		const BOOL IsItalic = pOutlineTextMetric->otmTextMetrics.tmItalic != 0;

		if (TypeFaceName==pItem->mTypeFaceName && IsBold==pItem->mIsBold && IsItalic==pItem->mIsItalic)
		{
			// hurray, success - so return this result.
		}
		else
		{
			// doesn't match, so return NULL;
			pOutlineTextMetric = NULL;
		}
	}

	return pOutlineTextMetric;
#endif
	return NULL;
}
	
OUTLINETEXTMETRIC *FontComponent::ExportGetOutlineTextMetricStage2(FontComponentListItem *pItem)
{
	PORTNOTETRACE("text","FontComponent::ExportGetOutlineTextMetricStage2 - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	ERROR2IF(pItem==NULL, NULL, "ExportGetOutlineTextMetricStage2 called with pItem==NULL.");
	ERROR2IF(pFontManager==NULL, NULL, "pFontManager==NULL.");
	ERROR2IF(pItem->mFontHandle==ILLEGALFHANDLE, NULL, "pItem->mFontHandle==ILLEGALFHANDLE.");

	OUTLINETEXTMETRIC *pOutlineTextMetric = NULL;

	// stage 2 - can we get the OUTLINETEXTMETRIC from one of the oil font managers
	//
	// Again, we'll ask nicely and see what result we get back. We'll get the LOGFONT from the font
	// manager, fill in the weight and italic entries like OSRenderRegion does and see what it comes back
	// with. And again we'll check to see if its anything like what we asked for.

	ENUMLOGFONT *pEnumLogFont;
	LOGFONT MyLogFont;

	pEnumLogFont = pFontManager->GetEnumLogFont(pItem->mFontHandle);

	if (pEnumLogFont!=NULL)
	{
		MyLogFont = pEnumLogFont->elfLogFont;

		// straight from OSRenderRegion.
		MyLogFont.lfWeight = (pItem->mIsBold) ? FW_BOLD : FW_NORMAL;
		MyLogFont.lfItalic = (pItem->mIsItalic) ? 1 : 0;

		pOutlineTextMetric=OILFontMan::GetOutlineTextMetric(pItem->mFontClass, &MyLogFont);

		if (pOutlineTextMetric!=NULL)
		{
			// we've got an OUTLINETEXTMETRIC, now check to see if its the right one
			//
			// Here we notice Microsoft's documentation and code don't quite agree. The OUTLINETEXTMETRIC
			// structure has 4 strings at the end, the 'family name', 'type name', 'style name'
			// and 'full name'. However for, say, Arial Bold:
			//
			// family name = "Arial"
			// face name = "Arial Bold"
			// style name = "Bold"
			// full name = "blah blah blah" (not important).

			const String_64 TypeFaceName = (char *) (((INT32)pOutlineTextMetric) + ((INT32)pOutlineTextMetric->otmpFamilyName));
			const BOOL IsBold = pOutlineTextMetric->otmTextMetrics.tmWeight > FW_NORMAL;
			const BOOL IsItalic = pOutlineTextMetric->otmTextMetrics.tmItalic != 0;
			
			if (TypeFaceName==pItem->mTypeFaceName && IsBold==pItem->mIsBold && IsItalic==pItem->mIsItalic)
			{
				// hurray it is, so do nothing
			}
			else
			{
				// No, its not. Free the memory and return NULL.
				free(pOutlineTextMetric);
				pOutlineTextMetric=NULL;
			}
		}
		else
		{
			// pOutlineTextMetric==NULL, so we have to return NULL
		}
	}
	else
	{
		// unable to get the LOGFONT structure, so we'll return NULL
		pOutlineTextMetric=NULL;
	}

	return pOutlineTextMetric;
#endif
	return NULL;
}

/********************************************************************************************

>	BOOL FontComponent::ImportCacheFont(FontComponentListItem *pItem)

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/08/96
	Inputs:		pItem	: FontComponentListItem to complete
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	Caches information about a font in a FontComponentListItem for import
	See Also:	class FontComponentListItem, class FontComponent

	Finally, the guts of the clever font matching system for the new file format. From
	the font definition record we get the font name (null string), face name and panose
	number for a font. We need to find the closest matching font out of all those installed
	on this system.

********************************************************************************************/

BOOL FontComponent::ImportCacheFont(FontComponentListItem *pItem)
{
	ERROR2IF(pItem==NULL, FALSE, "Parameter pItem==NULL.");
	ERROR2IF(ImportInitialised==FALSE, FALSE, "ImportCacheFont called when ImportInitialised==FALSE.");
	ERROR2IF(pFontManager==NULL, FALSE, "ImportCacheFont called when pFontManager==NULL.");
	
	// members of class FontComponentListItem are:
	//		BOOL mIsBold					: bodge from the panose number
	//		BOOL mIsItalic					: bodge from the panose number
	//		BOOL mRecentlyCreated			: leave alone
	//		WORD mFontHandle				: from the font manager
	//		INT32 mRecordNumber				: leave alone
	//		String_64 mFullFontName			: should already be filled in
	//		String_64 mTypeFaceName			: should already be filled in
	//		FontClass mFontClass			: should already be filled in
	//		CCPanose mPanoseNumber			: should already be filled in

	WORD FontHandle;

	// Ask the kernel font manager to find this typeface name and class
	FontHandle = pFontManager->FindTypeface(pItem->mTypeFaceName, pItem->mFontClass);

	if (FontHandle==ILLEGALFHANDLE)
	{
		// Failed, we ask the kernel font manager just to find this typeface name, ignoring the class
		FontHandle = pFontManager->FindTypeface(pItem->mTypeFaceName, FC_UNDEFINED);
	}
	else if (FALSE==(pFontManager->IsFontValid(FontHandle)))
	{
		// although the font is in the font cache it's not fully cached. i.e.
		// the font has been deinstalled during this session of Camelot. So
		// we find a replacement font now...

		PORTNOTE("text","FontComponent::ImportCacheFont - Removed FindClosestFont usage");
#ifndef DISABLE_TEXT_RENDERING
		CachedFontItem *pCachedFontItem = pFontManager->GetFont(FontHandle);

		WORD BestFontHandle = pFontManager->FindClosestFont(pItem->mPanoseNumber);
		ENUMLOGFONT MyEnumLogFont = *(pFontManager->GetEnumLogFont(BestFontHandle));

		// if necessary, get rid of the old ENUMLOGFONT structure
		pCachedFontItem->DeleteMetrics();

		pCachedFontItem->SetCacheData(&MyEnumLogFont);
		pCachedFontItem->SetIsReplaced(TRUE);
#endif
	}

	if (FontHandle==ILLEGALFHANDLE)
	{
		PORTNOTE("text","FontComponent::ImportCacheFont - Removed FindClosestFont usage");
#ifndef DISABLE_TEXT_RENDERING

			// Failed again, so we try to find the closest font using PANOSE font matching.
		WORD BestFontHandle = pFontManager->FindClosestFont(pItem->mPanoseNumber);

		// Get the correct fontclass for the subsituted font
		FontClass BestFontClass = pFontManager->GetFontClass(BestFontHandle);

		// add our font to the font manager's list
		CachedFontItem *pCachedFontItem = pFontManager->AddFont(&(pItem->mTypeFaceName), BestFontClass, FontHandle);

		// fix in the ENUMLOGFONT from the font found above.
		ENUMLOGFONT MyEnumLogFont = *(pFontManager->GetEnumLogFont(BestFontHandle));
		pCachedFontItem->SetCacheData(&MyEnumLogFont);
		pCachedFontItem->SetIsReplaced(TRUE);
#endif
	}
	
	if (FontHandle==ILLEGALFHANDLE)
	{
		// something has gone wrong. complain, and fix the error
		ERROR2(FALSE, "Unable to find a font!.");
		FontHandle = DEFAULTHANDLE;
	}

	pItem->mFontHandle = FontHandle;

	// fill in the mIsBold and mIsItalic entries for the FontComponentListItem from the panose number.
	pItem->mIsBold	 = pItem->mPanoseNumber.GetWeight() > PAN_WEIGHT_MEDIUM;				// !!HACK!!
	pItem->mIsItalic = pItem->mPanoseNumber.GetLetterform() > PAN_LETT_NORMAL_SQUARE;		// !!HACK!!

	return TRUE;		// we succeed - we always succeed!
}
