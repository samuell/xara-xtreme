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

// Metafile filter code

#include "camtypes.h"
#include "metafilt.h"

//#include "fixmem.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "tim.h"
//#include "andy.h"
//#include "rik.h"
#include "gdimagic.h"
//#include "group.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "camelot.h"
#include "nodepath.h"
//#include "pathname.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "oilfiles.h"
#include "csrstack.h"
#include "progress.h"        
#include "page.h"
//#include "trans2d.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "fillattr.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "attrmgr.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "resource.h"
//#include "spread.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "accures.h"
//#include "docview.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "rndrgn.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "ccfile.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "layer.h"
#include "osrndrgn.h"
#include "camvw.h"
#include "wbitmap.h"
//#include "dibutil.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "nodebmp.h"
#include "nodetxts.h"
#include "nodeelip.h"

#include "clipext.h"	// External clipboard
//#include "selop.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "metaview.h"
//#include "richard2.h"

CC_IMPLEMENT_DYNCREATE(MetaFileClipMap, ClipboardMapping)


// ------------------------------------------------------------------------------------------

// There now follows a certain amount of unpleasantness, which allows us to access
// 16-bit Windows' GDI records, which is what we get when reading 16-bit metafiles.

typedef short INT16;
typedef WORD  UINT16;

#ifdef _MSC_VER
	// Make sure structures correspond to Win16 equivalents.
	// (pack to 1 byte boundaries)
	#pragma pack(push, 1)

	typedef struct
	{
		INT16			lbStyle;
		COLORREF 		lbColor;
		INT16			lbHatch;
	} LOGBRUSH_16;

	typedef struct
	{
		INT16 x;
		INT16 y;
	} POINT_16;

	typedef struct
	{
		INT16			lopnStyle;
		POINT_16   		lopnWidth;
		COLORREF 		lopnColor;
	} LOGPEN_16;

	typedef struct
	{
		BYTE        peRed;
		BYTE        peGreen;
		BYTE        peBlue;
		BYTE        peFlags;
	} PALETTEENTRY_16;
	
	typedef struct
	{
		WORD        palVersion;
		WORD        palNumEntries;
		PALETTEENTRY_16        palPalEntry[1];
	} LOGPALETTE_16;

	#pragma pack(pop)
#else
	#error Dont know how to pack structures with this compiler!
#endif // _MSC_VER


/********************************************************************************************

<	HandleType

	Comment:	Used to identify the type of a GDI handle in a HandleTable entry.
	SeeAlso:	HandleRecord; HandleTable; MetaFileFilter

********************************************************************************************/

typedef enum
{
	HANDLE_NONE,
	HANDLE_PEN,
	HANDLE_BRUSH,
	HANDLE_PALETTE,
	HANDLE_PATTERNBRUSH,
	HANDLE_FONTINDIRECT,
	HANDLE_REGION
} HandleType;


/********************************************************************************************

>	class HandleRecord

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/03/94
	Purpose:	Table entries used by the HandleTable class.
				This class currently supports pen handles and brush handles.  Other
				handles (e.g. Fonts) will be added as necessary.
	SeeAlso:	HandleTable; MetaFileFilter

********************************************************************************************/

class HandleRecord
{
public:
	HandleType	Type;
	DocColour 	Colour;
	MILLIPOINT	PenWidth;
	LOGFONT_16*	pFont;
};
	


/********************************************************************************************

>	class HandleTable

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/03/94
	Purpose:	Provide a mirror of the table of handles maintained by GDI when importing
				a MetaFile.  This is necessary because, given a GDI handle, it is not
				possible to ask GDI what kind of object it is, and hence work out e.g. what
				kind of pen/brush it is.
				This class therefore creates, selects, and deletes handle objects in the
				same way that GDI would (see EnumMetafile help in SDK documentation for
				further details...possibly).
				At the moment, this HandleTable only supports pens and brushes.
	SeeAlso:	HandleRecord; MetaFileFilter

********************************************************************************************/

class HandleTable
{
public:
	HandleTable(MetaFileFilter *);
	~HandleTable();

	// Function to create objects
	BOOL CreatePen(LOGPEN_16 *);
	BOOL CreateBrush(LOGBRUSH_16 *);
	BOOL CreatePalette(LOGPALETTE_16 *);
	BOOL CreatePatternBrush(WORD*);
	BOOL CreateFontIndirect(LOGFONT_16*);
	BOOL CreateRegion(WORD*);

	// Select and delete them
	BOOL SelectObject(INT32 Index);
	BOOL DeleteObject(INT32 Index);

private:
	INT32 FindFreeSlot();

	// The filter associated with this HandleTable.
	MetaFileFilter *Context;

	// Pointer to the handle table.
	HandleRecord *Handles;

	// Number of entries in the handle table.
	INT32 TableSize;
};

/********************************************************************************************

>	HandleTable::HandleTable(MetaFileFilter *pFilter)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/03/94
	Purpose:	Initialises the HandleTable.  The table initially contains no entries.
	SeeAlso:	HandleTable

********************************************************************************************/

HandleTable::HandleTable(MetaFileFilter *pFilter)
{
	Handles = NULL;
	TableSize = 0;
	Context = pFilter;
}


/********************************************************************************************

>	HandleTable::~HandleTable()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/03/94
	Purpose:	Deinitialises a handle table; deallocates the handle table.
	SeeAlso:	HandleTable

********************************************************************************************/
HandleTable::~HandleTable()
{
	CCFree(Handles);
	Handles = NULL;
	TableSize = 0;
}


// Constants used in HandleTable member functions:

// Number of entries to increase handle table by when it runs out of space.
static const INT32 TableGranularity = 10;

// Value that implies an error occurred while getting a slot index.
static const INT32 BAD_SLOT = -1;



/********************************************************************************************

>	INT32 HandleTable::FindFreeSlot()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/03/94
	Returns:	Index of first free slot in the handle table, or BAD_SLOT if there is no
				free slot.
	Purpose:	Find the index of the first free slot available in the handle table.  If no
				free handles are found, the table is expanded and the index of the first
				new slot is returned. If it is not possible to expand the table (out of
				memerory) then BAD_SLOT is returned.
	Errors:		Out of memory.
	SeeAlso:	HandleTable

********************************************************************************************/
INT32 HandleTable::FindFreeSlot()
{
	INT32 Slot;

	// Search the existing table for blanks
	for (Slot = 0; Slot < TableSize; Slot++)
	{
		if (Handles[Slot].Type == HANDLE_NONE)
			return Slot;
	}

	// Didn't find a free slot - get some more memory and return the first slot we get.

	INT32 NewTableSize = TableSize + TableGranularity;

	// If no slots yet, then allocate a table, otherwise expand the existing table
	HandleRecord *NewHandles;
	if (Handles == NULL)
		NewHandles = (HandleRecord *) CCMalloc(NewTableSize * sizeof(HandleRecord));
	else
		NewHandles = (HandleRecord *) CCRealloc(Handles, NewTableSize * sizeof(HandleRecord));

	// Did that work?
	if (NewHandles == NULL)
	{
		// No more memory available, but error state set by ccmalloc
		return BAD_SLOT;
	}

	// Initialise the new slots
	for (Slot = TableSize; Slot < NewTableSize; Slot++)
	{
		NewHandles[Slot].Type = HANDLE_NONE;

		// This bizarre syntax of 'new' allows us to call the constructor for an object
		// that has already been allocated.  We need to do this otherwise all the 
		// DocColour objects in the handle table are not initialised.
		// The constructors are not called because we just CCMalloc() the block of memory,
		// because we want to be able to resize the block (using CCRealloc).
		new(&NewHandles[Slot].Colour) DocColour;
	}

	// Remember the first slot
	Slot = TableSize;

	// Update handle table variables
	Handles   = NewHandles;
	TableSize = NewTableSize;

	// Return the first new slot
	return Slot;
}

/********************************************************************************************

>	BOOL HandleTable::CreatePen(LOGPEN_16 *pPen)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/03/94
	Inputs:		pPen - the logical pen information for this pen. 
					(See LOGPEN in Win16 SDK Help)
	Returns:	TRUE if handle was created ok, FALSE if not.
	Purpose:	Create a handle for the specified pen in the HandleTable, and store
				the pen information in this.
	Errors:		Out of memory.
	SeeAlso:	HandleTable::CreateBrush

********************************************************************************************/
BOOL HandleTable::CreatePen(LOGPEN_16 *pPen)
{
	INT32 Style = pPen->lopnStyle;
	COLORREF Col = (COLORREF) pPen->lopnColor;

	// Get a new slot to put this pen in
	INT32 Slot = FindFreeSlot();

	if (Slot == BAD_SLOT)
		// Could not create this pen
		return FALSE;

	Handles[Slot].Type = HANDLE_PEN;

	switch (Style)
	{
		case PS_SOLID:
		case PS_INSIDEFRAME:
			Handles[Slot].Colour = DocColour(GetRValue(Col), GetGValue(Col), GetBValue(Col));
			break;

		case PS_NULL:
			Handles[Slot].Colour = DocColour(COLOUR_TRANS);
			break;

		default:
			ENSURE(FALSE, "Unknown pen style in metafile!");
			ERROR(_R(IDT_BAD_METAFILE), FALSE);
	}

	// Scale pen width to document coordinates.
	DocCoord PenSize(pPen->lopnWidth.x, 0);
	Context->ScaleCoord(&PenSize);
	Handles[Slot].PenWidth = PenSize.x;

	// All ok
	return TRUE;
}


/********************************************************************************************

>	BOOL HandleTable::CreateBrush(LOGBRUSH_16 *pBrush)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/03/94
	Inputs:		pBrush - the logical brush information for this brush.
					(See LOGBRUSH in Win16 SDK Help)
	Returns:	TRUE if handle was created ok, FALSE if not.
	Purpose:	Create a handle for the specified brush in the HandleTable, and store
				the brush information in this.
	Errors:		Out of memory.
	SeeAlso:	HandleTable::CreatePen

********************************************************************************************/
BOOL HandleTable::CreateBrush(LOGBRUSH_16 *pBrush)
{
	INT32 Style = pBrush->lbStyle;
	COLORREF Col = (COLORREF) pBrush->lbColor;

	// Get a new slot to put this pen in
	INT32 Slot = FindFreeSlot();

	if (Slot == BAD_SLOT)
		// Could not create this pen
		return FALSE;

	Handles[Slot].Type = HANDLE_BRUSH;

	switch (Style)
	{
		case BS_SOLID:
			Handles[Slot].Colour = DocColour(GetRValue(Col), GetGValue(Col), GetBValue(Col));
			break;

		case BS_NULL:
			Handles[Slot].Colour = DocColour(COLOUR_TRANS);
			break;

		default:
			ENSURE(FALSE, "Unknown brush style in metafile!");
			ERROR(_R(IDT_BAD_METAFILE), FALSE);
	}

	// All ok
	return TRUE;
}


/********************************************************************************************

>	BOOL HandleTable::CreatePalette(LOGPALETTE_16 *)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/5/95
	Returns:	TRUE if it worked, FALSE if there was a problem
	Purpose:	Creates a Palette object ready for use

********************************************************************************************/
BOOL HandleTable::CreatePalette(LOGPALETTE_16 *)
{
	// Get a new slot to put this pen in
	INT32 Slot = FindFreeSlot();

	// Could not create this Palette
	if (Slot == BAD_SLOT)
		return FALSE;

	// Say what this is
	Handles[Slot].Type = HANDLE_PALETTE;

	// return Happy
	return TRUE;
}


/********************************************************************************************

>	BOOL HandleTable::CreatePatternBrush(WORD*)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/5/95
	Returns:	TRUE if it worked, FALSE if there was a problem
	Purpose:	Creates a PatternBrush object ready for use

********************************************************************************************/
BOOL HandleTable::CreatePatternBrush(WORD*)
{
	// Get a new slot to put this pen in
	INT32 Slot = FindFreeSlot();

	// Could not create this Palette
	if (Slot == BAD_SLOT)
		return FALSE;

	// Say what this is
	Handles[Slot].Type = HANDLE_PATTERNBRUSH;

	// return Happy
	return TRUE;
}


/********************************************************************************************

>	BOOL HandleTable::CreateFontIndirect(LOGFONT_16* pNewFont)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/5/95
	Inputs:		pNewFont - the font we are creating
	Returns:	TRUE if it worked, FALSE if there was a problem
	Purpose:	Creates a Font object ready for use

********************************************************************************************/
BOOL HandleTable::CreateFontIndirect(LOGFONT_16* pNewFont)
{
	// Get a new slot to put this pen in
	INT32 Slot = FindFreeSlot();

	// Could not create this Palette
	if (Slot == BAD_SLOT)
		return FALSE;

	// Say what this is
	Handles[Slot].Type = HANDLE_FONTINDIRECT;
	Handles[Slot].pFont = pNewFont;

	// return Happy
	return TRUE;
}


/********************************************************************************************

>	BOOL HandleTable::CreateRegion(WORD*)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/5/95
	Returns:	TRUE if it worked, FALSE if there was a problem
	Purpose:	Creates a Region object ready for use

********************************************************************************************/
BOOL HandleTable::CreateRegion(WORD*)
{
	// Get a new slot to put this pen in
	INT32 Slot = FindFreeSlot();

	// Could not create this Palette
	if (Slot == BAD_SLOT)
		return FALSE;

	// Say what this is
	Handles[Slot].Type = HANDLE_REGION;

	// return Happy
	return TRUE;
}


/********************************************************************************************

>	BOOL HandleTable::SelectObject(INT32 Index)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/03/94
	Inputs:		Index - the index of the handle to select.
	Returns:	TRUE if the handle was selected ok; FALSE if not.
	Purpose:	Mimic the selection of a GDI object.  This function will update the relevant
				status variables in the associated MetaFileFilter object.  
	Errors:		Bad index handle (i.e. specified handle does not exist)
	SeeAlso:	HandleTable::DeleteObject

********************************************************************************************/
BOOL HandleTable::SelectObject(INT32 Index)
{
	// Sanity check
	if ((Index < 0) || (Index >= TableSize))
	{
		// Out of range index
		if (IsUserName("Rik"))
			TRACE( _T("Tried to select out of range object\n"));
		ERROR(_R(IDT_BAD_METAFILE), FALSE);
	}

	// Try to select the object.
	switch (Handles[Index].Type)
	{
		case HANDLE_NONE:
			// No such object
			if (IsUserName("Rik"))
				TRACE( _T("Tried to select non-existent object\n"));
			return FALSE;

		case HANDLE_PEN:
			Context->SetLineColour(Handles[Index].Colour);
			Context->SetLineWidth(Handles[Index].PenWidth);
			break;

		case HANDLE_BRUSH:
			Context->SetFillColour(Handles[Index].Colour);
			break;

		case HANDLE_PALETTE:
			break;

		case HANDLE_PATTERNBRUSH:
			break;

		case HANDLE_FONTINDIRECT:
			Context->SetLogicalFont(Handles[Index].pFont);
			break;
	
		case HANDLE_REGION:
			break;

		default:
			ENSURE(FALSE, "Bad metafile handle type!");
			ERROR(_R(IDT_BAD_METAFILE), FALSE);
	}

	// To get here we must have selected the object ok
	return TRUE;
}


/********************************************************************************************

>	BOOL HandleTable::DeleteObject(INT32 Index)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/03/94
	Inputs:		Index - index of the handle object to delete from the table/
	Returns:	TRUE if handle deleted ok; FALSE if not.
	Purpose:	Deletes a handle from the Handle table.  The index of this handle should
				no longer be used (unless, of course, it is reallocated to a new handle).
	Errors:		Bad index handle (i.e. specified handle does not exist)
	SeeAlso:	HandleTable::SelectObject

********************************************************************************************/
BOOL HandleTable::DeleteObject(INT32 Index)
{
	// Sanity check
	if ((Index < 0) || (Index >= TableSize))
	{
		// Out of range index
		if (IsUserName("Rik"))
			TRACE( _T("Tried to delete out of range object\n"));
		return FALSE;
	}

	// Try to select the object.
	switch (Handles[Index].Type)
	{
		case HANDLE_NONE:
			// No such object
			if (IsUserName("Rik"))
				TRACE( _T("Tried to delete non-existent object\n"));
			return FALSE;

		case HANDLE_PEN:
		case HANDLE_BRUSH:
		case HANDLE_PALETTE:
		case HANDLE_PATTERNBRUSH:
		case HANDLE_FONTINDIRECT:
		case HANDLE_REGION:
			Handles[Index].Type = HANDLE_NONE;
			break;

		default:
			ENSURE(FALSE, "Bad metafile handle type!");
			ERROR(_R(IDT_BAD_METAFILE), FALSE);
	}

	// To get here we must have deleted the object ok
	return TRUE;
}


/********************************************************************************************

>	MetaFileClipMap::MetaFileClipMap(ClipboardMappingType TheType, Filter *TheFilter,
										InternalClipboardFormat *TheInternalDataType,
										UINT32 TheExternalDataType,
										UINT32 ThePriority)
	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/4/95

	Inputs:		TheType - 1 (import), 2 (export), or 3 (import and export)

				TheFilter - The filter which can apply this conversion

				TheInternalDatatType - An object defining the internal data type
				(see cliptype.h)

				TheExternalDataType - A Windows CF_ constant defining the external
				clipboard data type which will be imported/exported.

				ThePriority - An integer indicating the priority of this mapping.
				The highest available priority mapping will be used in order to
				retain as much information in the data as possible. See
				docs\howtouse\ExtClip.doc for details of the existing mappings.

	Purpose:	Constructs a clipboard mapping with the ExternalClipboard
				manager. This mapping info describes a filter which is able to import
				data from or export data to a windows clipboard in some way.

	Notes:		DON'T call the constructor - call CreateAndRegister

	SeeAlso:	MetaFileClipMap::CreateAndRegister

********************************************************************************************/
MetaFileClipMap::MetaFileClipMap(ClipboardMappingType TheType, Filter *TheFilter,
									InternalClipboardFormat &TheInternalDataType,
									UINT32 TheExternalDataType,
									UINT32 ThePriority)
				: ClipboardMapping(TheType, TheFilter, TheInternalDataType,
									TheExternalDataType, ThePriority)
{
}



/********************************************************************************************

>	static void MetaFileClipMap::CreateAndRegister(ClipboardMappingType TheType, Filter *TheFilter,
												InternalClipboardFormat &TheInternalDataType,
												UINT32 TheExternalDataType,
												UINT32 ThePriority)
	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/4/95

	Inputs:		ImportOrExport - TRUE == Import, FALSE == Export

				TheFilter - The filter which can apply this conversion

				TheInternalDatatType - An object defining the internal data type
				(see cliptype.h)

				TheExternalDataType - A Windows CF_ constant defining the external
				clipboard data type which will be imported/exported.

				ThePriority - An integer indicating the priority of this mapping.
				The highest available priority mapping will be used in order to
				retain as much information in the data as possible. See
				docs\howtouse\ExtClip.doc for details of the existing mappings.

	Purpose:	Constructs and registers a clipboard mapping with the ExternalClipboard
				manager. This mapping info describes a filter which is able to import
				data from or export data to a windows clipboard in some way.

********************************************************************************************/
void MetaFileClipMap::CreateAndRegister(ClipboardMappingType TheType, Filter *TheFilter,
										InternalClipboardFormat &TheInternalDataType,
										UINT32 TheExternalDataType,
										UINT32 ThePriority)
{
	MetaFileClipMap *Mapping = new MetaFileClipMap(TheType, TheFilter,
													TheInternalDataType,
													TheExternalDataType,
													ThePriority);
	if (Mapping == NULL)
		InformError();
	else
		ExternalClipboard::RegisterDataType(Mapping);
}



/********************************************************************************************

>	virtual BOOL MetaFileClipMap::HandleImport(SelOperation *Caller,
												HANDLE ClipboardData,
												InternalClipboard *Dest)
	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/4/95

	Inputs:		Caller - The operation within which this method is being called
				ClipboardData - The result of calling GetClipboardData for your datatype
				Dest - The InternalClipboard (document) to import the data into.

	Returns:	TRUE for success

	Purpose:	Calls the parent filter as appropriate to import the given data from
				the external clipboard.

********************************************************************************************/
BOOL MetaFileClipMap::HandleImport(SelOperation *Caller,
									HANDLE ClipboardData, InternalClipboard *Dest)
{
	BOOL ok = FALSE;

	// Get a scratch file - if TMP isn't set, this will try for c:\temp.
	// The filename will have XS as a prefix
	char *tempname = GetTempFileName();	//_ttempnam("C:\temp", "XS");
	if (tempname == NULL)
	{
		ERROR3("Couldn't get a temp filename");
		return(FALSE);
	}

	// ---
	// Get the header data out of the clipboard, and copy the referenced metafile to disc
	METAFILEPICT *Info = (METAFILEPICT *) GlobalLock(ClipboardData);

	if (Info == NULL)
	{
		RemoveTempFile();
		return(FALSE);
	}

	HMETAFILE hCopy = CopyMetaFile(Info->hMF, tempname);

	GlobalUnlock(ClipboardData);

	if (hCopy != NULL)
	{
		ok = ImportFromTempFile(tempname, Caller, Dest);
		DeleteMetaFile(hCopy);
	}

	RemoveTempFile();

	return(ok);
}


		
/********************************************************************************************

>	virtual BOOL MetaFileClipMap::HandleExport(Operation *Caller,
												InternalClipboard *Source)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/4/95

	Inputs:		Caller - the operation within which this method is being called
				Source - the internal clipboard (document) to be exported

	Returns:	NULL (if it failed), or a windows handle of the data to be placed on
				the clipboard.

	Purpose:	Invokes this mapping for exporting
				This takes the document tree of Source, and exports it to the external
				(windows) clipboard. Usually this just involves calling Filter::DoExport
				for the parent filter, and then returning the handle to the global memory
				block to be placed onto the external clipboard.

	Notes:		The returned handle should be the thing you'd pass to SetClipboardData
				if you were dealing with it directly. You must adhere to all the Windows
				rules for this - i.e. a global data block, unlocked, etc etc.

********************************************************************************************/
HANDLE MetaFileClipMap::HandleExport(Operation *Caller, InternalClipboard *Source)
{
#if (_OLE_VER >= 0x200)
	
	// Check if we need to allocate some memory.
	HANDLE hGlobalMem;
	if ((hGlobalMem = m_hMem) != 0)
	{
		// We have some already.  Is it big enough?
		if (m_cbMemSize < sizeof(METAFILEPICT)) return 0;
	}
	else
	{
		// We must allocate some.
		hGlobalMem = m_hMem = GlobalAlloc(GHND, sizeof(METAFILEPICT));
		if (!hGlobalMem) return 0;
	}

#else

	HANDLE hGlobalMem = GlobalAlloc(GHND, sizeof(METAFILEPICT));
	if (!hGlobalMem) return 0;

#endif

	METAFILEPICT* Info = (METAFILEPICT*) GlobalLock(hGlobalMem);

	// Initialise to suitable defaults, just in case
	Info->mm	= MM_ANISOTROPIC;
	Info->xExt	= 0;
	Info->yExt	= 0;
	Info->hMF	= NULL;	

	// --- Now, export the source clipboard to a memory metafile
	// Create a dummy MemFile to keep the exporter happy
	char *temp[1024];
	CCMemFile DummyFile(temp, 1000);

	((MetaFileFilter *)pFilter)->DoExport(Caller, &DummyFile, (Document *)Source, Info);

	if (DummyFile.isOpen()) DummyFile.close();

	// -- Finally, lob the metafile on the clipboard (we can forget about it now, as
	// it is now owned (and will be deleted) by the OS)
	// We must unlock the block before giving it to the clipboard
	GlobalUnlock(hGlobalMem);
	return(hGlobalMem);
}



/********************************************************************************************

>	MetaFileFilter::MetaFileFilter()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/93
	Purpose:	MetaFile filter constructor - this initialises the default attributes.

********************************************************************************************/
MetaFileFilter::MetaFileFilter()
{
	// Set up filter descriptions.
	FilterName.Load(_R(IDT_METAFILE_FILTERNAME));
	FilterInfo.Load(_R(IDT_METAFILE_FILTERINFO));
	FilterID = FILTERID_METAFILE;

	// We can import and export metafiles
	Flags.CanImport = TRUE ;
	Flags.CanExport = TRUE;

	OutputFile = NULL;
	ExportRegion = NULL;
	ExportMsgID = _R(IDT_EXPORTMSG_METAFILE1);

	// Default values
	FlipYCoords = FALSE;
	YShift = 0;
	IsYExtentNegative = FALSE;
	Dpi = 96;

	// Default to Black text
	TextColour = DocColour(0,0,0);

	// Set up the font
	SelectedFont.lfHeight = -96;
	SelectedFont.lfWidth = 0;
	SelectedFont.lfEscapement = 0;
	SelectedFont.lfOrientation = 0;
	SelectedFont.lfWeight = 0;
	SelectedFont.lfItalic = FALSE;
	SelectedFont.lfUnderline = FALSE;
	SelectedFont.lfStrikeOut = FALSE;
	SelectedFont.lfCharSet = 0;
	SelectedFont.lfOutPrecision = 3;
	SelectedFont.lfClipPrecision = 2;
	SelectedFont.lfQuality = 1;
	SelectedFont.lfPitchAndFamily = 18;
	
	camStrcpy(SelectedFont.lfFaceName, (TCHAR *)String_64(_R(IDS_METAFILT_FONT))/*"Times New Roman"*/);

	// Clear out the view
	pMetaView = NULL;
};

/********************************************************************************************

>	void MetaFileFilter::AddNodeToMetaFileGroup(NodeRenderableBounded *pNewNode)

	Author:		Jonathan_Payne (Xara Group Ltd) <camelotdev@xara.com>
	Created:	06/11/2000
	Inputs:		pNewNode
	Returns:	True if all is OK
	Purpose:	Inserts a new node into the group the meta file is being imported into

********************************************************************************************/
void MetaFileFilter::AddNodeToMetaFileGroup(NodeRenderableBounded *pNewNode)
{
	if (pLastInsertedNode)
		// The group we are adding to has some objects in it se we just need
		// to instert the new node as NEXT after pLastInsertedNode
		pNewNode->AttachNode(pLastInsertedNode, NEXT);
	else
		// pLastInsertedNode == 0 so this is the fist object to be inserted into the
		// group so the object is inserted as LASTCHILD of the group
		pNewNode->AttachNode(pNode, LASTCHILD);

	pNewNode->GetBoundingRect();

	// Update pLastInsertedNode pointer
	pLastInsertedNode = pNewNode;
}

/********************************************************************************************

>	MetaFileFilter::~MetaFileFilter()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/93
	Purpose:	Destructor for the metafile filter.
				Does nothing at present.

********************************************************************************************/
MetaFileFilter::~MetaFileFilter()
{
}

/********************************************************************************************

>	BOOL MetaFileFilter::Init()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/93
	Returns:	TRUE if it worked, FLASE if it failed
	Purpose:	Initialise the filter.

********************************************************************************************/
BOOL MetaFileFilter::Init()
{
	// Get the OILFilter object
	// This is deleted in the base class destructor
	pOILFilter = new MetaFileOILFilter(this);
	if (pOILFilter==NULL)
		// Error state already set by new
		return FALSE;		

	MetaFile = NULL;
	MetaFileDC = NULL;
	Handles = NULL;

	// Register ourselves with the clipboard: We can provide import and export mappings
	// between internal "vector" format and windows metafiles (CF_METAFILEPICT).
	// Our mapping has a priority level of 100.
	InternalClipboardFormat Format(CLIPTYPE_VECTOR);
	MetaFileClipMap::CreateAndRegister(CLIPMAP_IMPORTONLY, this, Format, CF_METAFILEPICT, 100);

	// All ok
	return TRUE;
}


/********************************************************************************************

>	virtual BOOL MetaFileFilter::IsDefaultDocRequired(const TCHAR* pcszPathName)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/10/95
	Inputs:		pcszPathName	pointer to the pathname to check
	Returns:	TRUE if the filter requires a default document, FALSE if not.
	Purpose:	Works out if opening a file of this type requires a default document to be
				loaded. If the file format supplies the document then return FALSE otherwise
				return TRUE. An example would be opening a bitmap file. This has no document
				defined in the file format and so we need to laod the default document before
				importing the bitmap into this file.
				In this baseclass version return FALSE and hence assume that the filters that
				need to will override this function to return TRUE.
	SeeAlso:	Filter::IsDefaultDocRequired; FilterFamily::IsDefaultDocRequired;
				CCamDoc::OnOpenDocument;
				FilterFamily::DoImport; Filter::DoImport; 

********************************************************************************************/
BOOL MetaFileFilter::IsDefaultDocRequired(const TCHAR* pcszPathName)
{
	// No need to check the pathname, just return TRUE as all this current filter has no
	// concept of page size, layout etc and hence will require the default document.
	return TRUE;
}	

/********************************************************************************************

>	BOOL MetaFileFilter::PrepareToImport()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/02/94
	Returns:	TRUE if it succeeds, FALSE if not.
	Purpose:	Sets up the MetaFile filter so it can read in a metafile.
	Errors:		None

********************************************************************************************/
BOOL MetaFileFilter::PrepareToImport()
{
	// Not attached to a metafile
	ENSURE(MetaFile == NULL, "PrepareToImport: still attached to a metafile!");
	MetaFile = NULL;
	ENSURE(MetaFileDC == NULL, "PrepareToImport: still attached to a metafile DC!");
	MetaFileDC = NULL;

	// Get some attributes
	if (!SetUpCurrentAttrs())
		return FALSE;

	// Get a handle table
	ENSURE(Handles == NULL, "PrepareToImport: still got a handle table!");
	Handles = new HandleTable(this);
	if (Handles==NULL)
		// Error state already set by new
		return FALSE;

	// For progress cursor update.
	FileSize = 0;
	BytesRead = 0;

	// Prepare pseudo-graphics mode
	CurrentMappingMode = MM_TEXT;
	MetaFileOrigin.x = 0;
	MetaFileOrigin.y = 0;

	// Default to Black text
	TextColour = DocColour(0,0,0);

	return TRUE;
}

/********************************************************************************************

>	void MetaFileFilter::CleanUpAfterImport()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/02/94
	Purpose:	Delete dynamic objects used in the import process.
	SeeAlso:	MetaFileFilter::PrepareToImport; MetaFileFilter::DoImport

********************************************************************************************/
void MetaFileFilter::CleanUpAfterImport()
{
	DeleteCurrentAttrs();

	// Release our handle to the metafile
	if (MetaFile != NULL)
	{
		if (!::DeleteMetaFile(MetaFile))
			ENSURE(FALSE, "Error occured while closing metafile");
		MetaFile = NULL;
	}

	// Release our handle to the metafile's DC
	if (MetaFileDC != NULL)
	{
		if (!::CloseMetaFile(MetaFileDC))
		{
			ENSURE(FALSE, "Error occured while closing metafile");
		}
		MetaFileDC = NULL;
	}

	// Get rid of our handle table
	delete Handles;
	Handles = NULL;
}


/********************************************************************************************

>	INT32 MetaFileFilter::DecodeMetaFile(HDC hdc, HANDLETABLE FAR* pHandleTable, 
				            	   METARECORD FAR* pMetaRec, INT32 cObj, LPARAM lParam)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/03/94
	Inputs:		hdc - the DC of the metafile.
				pHandleTable - pointer to the handle table that GDI maintains while
							   enumerating a metafile (NB this is not the same thing as the
							   Camelot 'HandleTable' class).
				pMetaRec - pointer to the metafile record to be decoded.
				cObj - number of objects that have handles in the handle table (ignored).
				lParam - user data; actually a pointer to the relevant MetaFileFilter object.
	Returns:	1 => Keep enumerating the metafile
				0 => An error occurred - stop enumerating the metafile now.
	Purpose:	Callback function for decoding metafiles.  This is called by the SDK function
				EnumMetaFile, and decodes each record that it is passed into the relevant
				Camelot object.
	Errors:		Out of memory; Syntax error/corruption in metafile (e.g. trying to select
				a non-existent object).
	SeeAlso:	MetaFileFilter

********************************************************************************************/
INT32 CALLBACK MetaFileFilter::DecodeMetaFile(HDC hdc, HANDLETABLE FAR* pHandleTable, 
		        			  		   METARECORD FAR* pMetaRec, INT32 cObj, LPARAM lParam)
{
	MetaFileFilter *Filter = (MetaFileFilter*)lParam;
	INT32 Result = Filter->DecodeMetafileRecord(pMetaRec);

	// If there was an error, return an error to the caller
	if (Result!=1)
		ERROR1(FALSE, _R(IDE_BADMETAFILE));

	// if there was no error, return asking for the next record
	return 1;
}

/********************************************************************************************

>	BOOL MetaFileFilter::OpenAndIterateMetafile( CCLexFile* pDiskFile )

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/3/95
	Purpose:	Virtual fn which opens and enumerates the metafile. This can be overriden
				for other metafile-based readers (e.g. AldusFormatfilter).
	Returns:	TRUE if OK, FALSE if not (and sets error code).
	SeeAlso:	MetaFileFilter::DoImport

********************************************************************************************/
BOOL MetaFileFilter::OpenAndIterateMetafile(CCLexFile *pDiskFile)
{
	// Try to open the file.
	if (!pDiskFile->IS_KIND_OF(CCDiskFile))
	{
		TRACEUSER( "JustinF", _T("Non-CCDiskFile in MetaFileFilter::OpenAndIterateMetafile\n"));
		return FALSE;
	}

	METADATA MetaData;
	PathName Path = ((CCDiskFile*) pDiskFile)->GetPathName();
	String_256 PathString = Path.GetPath();
	MetaFile = GetMetaFileHandle((TCHAR*) PathString, &MetaData);

	if (MetaFile == NULL)
	{
		// MetaFile failed to open
		TRACEUSER( "Jonathan", _T("Could not open metafile\n"));
		ERROR(_R(IDT_IMPORT_NOTFOUND), FALSE);
	}

	// Set the placeable flag so the callback knows what kind of file this is.
	Placeable = MetaData.Placeable;

	// If this is a placeable, get the size and origin of this file.
	if (Placeable)
	{
		// Origin is bottom left of bounding box
		MetaFileOrigin.x = MetaData.Header.bbox.left;
		MetaFileOrigin.y = MetaData.Header.bbox.bottom;

		// Find out the width and height of the bbox
		FlipYCoords = FALSE;
		IsYExtentNegative = FALSE;

		// See which way up it all is
		INT32 YExtent = MetaData.Header.bbox.top-MetaFileOrigin.y;
		if (YExtent < 0)
			IsYExtentNegative = TRUE;

		// adjust the origin if needed
		if (MetaFileOrigin.y > MetaData.Header.bbox.top)
			YShift = -YExtent;

		// Scaling factor is defined in units per inch
		// Convert to millipoints: (n / units per inch) * 72000
		Dpi = MetaData.Header.inch;
		ScaleFactor = (72000 / MetaData.Header.inch);

		TRACEUSER( "Jonathan", _T("Placable metafile -\n  (%ld, %ld) %ld\n"), MetaFileOrigin.x, MetaFileOrigin.y, ScaleFactor);
		TRACEUSER( "Jonathan", _T("Bbox (%ld, %ld)\n     (%ld, %ld)\n"), MetaData.Header.bbox.left, MetaData.Header.bbox.bottom, MetaData.Header.bbox.right, MetaData.Header.bbox.top);
		TRACEUSER( "Jonathan", _T("Is Negative %d\n"), IsYExtentNegative);
	}
	else
	{
		// Sensible default values for non-placeable metafiles.
		MetaFileOrigin.x = 0;
		MetaFileOrigin.y = 0;
		ScaleFactor = 72000 / 96;
		FlipYCoords = FALSE;
		YShift = 0;
		
		// By default windows has the 0,0 at the top left, we have it at the bottom left
		IsYExtentNegative = TRUE;
		Dpi = 96;
		TRACEUSER( "Jonathan", _T("Non-Placable metafile -\n  Origin (%ld, %ld) ScaleFactor %ld\n"), MetaFileOrigin.x, MetaFileOrigin.y, ScaleFactor);
		TRACEUSER( "Jonathan", _T("Is Negative %d\n"), IsYExtentNegative);
	}

	// Find out the size of the file, in bytes.
	FileSize = GetMetaFileBitsEx(MetaFile, 0, NULL);
	if (FileSize == 0)
	{
		if (IsUserName("Rik"))
			TRACE( _T("Couldn't take size of the MetaFile\n"));
		ERROR(_R(IDT_IMPORT_NOTFOUND), FALSE);
	}

	// Create a device context for our metafile
	MetaFileDC = CreateMetaFile(NULL);
	if (MetaFileDC == NULL)
	{
		return FALSE;
	}

	// Set the progress indicator, this next bit might take a while.
	String_64 ImportMessage(_R(IDT_IMPORTMSG_METAFILE));
	ImportMessage = GetImportProgressString(pDiskFile, _R(IDT_IMPORTMSG_METAFILE));
	BeginSlowJob(FileSize, TRUE, &ImportMessage);

	// Process the metafile by enumerating it via GDI
	BOOL Worked = ::EnumMetaFile(MetaFileDC, MetaFile, DecodeMetaFile, (LPARAM) this);

	EndSlowJob();

	return Worked;
}

/********************************************************************************************
>	BOOL MetaFileFilter::DoImport(SelOperation *Op, CCLexFile* pDiskFile, 
							  Document *DestDoc, BOOL AutoChosen, ImportPosition *Pos,
							  KernelBitmap** ppImportedBitmap,DocCoord* pPosTranslate, String_256* URL)
	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/93
	Inputs:		Op - pointer to the operation that this read process is associated with.
				pDiskFile - The file that we want to import
				DestDoc - pointer to the document to insert the MetaFile data into.
				AutoChosen - TRUE if the filter system selected this filter automatically,
							 and not the user.
				Pos -
				ppImportedBitmap - this is used mainly in the bitfltr.cpp for the HTML
				import filter. HTMLFilter::DoImport() needs a pointer to a kernel bitmap
				to set the background bitmap up into Camelot.
				pPosTranslate - This is used too by the HTMLFilter in order to do a formatting.
				URL - original URL of the imported file
	Returns:	TRUE if the read was successful, FALSE if not.
	Purpose:	Import the named Windows metafile into the specified document.
				Currently, the data is always positioned on the first page of the first
				spread in the first chapter.
	Errors:		Unable to open file, corrupted document tree found.
********************************************************************************************/
BOOL MetaFileFilter::DoImport(SelOperation *Op, CCLexFile* pDiskFile, 
							  Document *DestDoc, BOOL AutoChosen, ImportPosition *pPos,
							  KernelBitmap** ppImportedBitmap,DocCoord* pPosTranslate, String_256* URL)
{
	// Set up value for each import
	InFile = 0;
	
	// We need to know which document later on
	TheDocument = DestDoc;

	// Let's get ready
	if (!PrepareToImport())
		return FALSE;

	// Make sure that there is a layer to put the bitmap onto
	if (!MakeSureLayerExists(DestDoc))
		// There is no layer and one could not be made, so we will have to fail
		return FALSE;

	// Find the layer on the first page of this document...
	pLayer = GetActiveLayer(DestDoc);
	ENSURE(pLayer != NULL, "Spread has no active layer"); 

	NodeGroup* pGroup = new NodeGroup;
	if (!pGroup)
		return FALSE;
	pNode = pGroup;

	// Put the group in the tree
	AttachNodeDirection dir;

	Node *pInsertPoint = pLayer->FindLastChild();
	if (pInsertPoint)
	{
		if (pInsertPoint->IsAnInsertionNode())
			dir = PREV;
		else
			dir = NEXT;
	}
	else
	{
		pInsertPoint = pLayer;
		dir = LASTCHILD;
	}

	Op->DoInsertNewNode(pGroup,			// Node to insert
						pInsertPoint,	// Context
	 					dir,			// AttachDirection
						TRUE,			// InvalidateRegion
						TRUE,			// ClearSelection
						TRUE,			// SelectNewObject
						FALSE);			// NormaliseAttributes


	// Set last inserted node to zero so the next node is inserted as a child of pGroup
	pLastInsertedNode = 0;

	ImportInfo.pOp = Op;

	if (pPos == NULL)
	{		
		ImportInfo.Pos.pSpread = GetFirstSpread(DestDoc);
		DocCoord centre = ImportInfo.Pos.pSpread->FindFirstPageInSpread()->GetPageRect().Centre();
		ImportInfo.Pos.Position.x = centre.x;
		ImportInfo.Pos.Position.y = centre.y;
	}
	else
		ImportInfo.Pos		= *pPos;

	// now read the file etc using virtual function so does different things on different
	// sub-classes
	AllUnderstood = TRUE;
	BOOL Worked = OpenAndIterateMetafile( pDiskFile );

	CleanUpAfterImport();

	if (InFile)
	{
		_lclose(InFile);
		InFile = 0;
	}

	if (!Worked)
	{
//		pGroup->CascadeDelete();
//		delete pGroup;
		return FALSE;
	}

	// Get the final group under the mouse cursor (or coords
	// set above if this is not a drag'n'drop)
	pGroup->InvalidateBoundingRect();

	DocCoord GroupCentre = pGroup->GetBoundingRect().Centre();
	DocCoord GroupTrans;

	GroupTrans.x = ImportInfo.Pos.Position.x - GroupCentre.x;
	GroupTrans.y = ImportInfo.Pos.Position.y - GroupCentre.y;
	
	Trans2DMatrix trans(GroupTrans.x, GroupTrans.y);
	pGroup->Transform(trans);

	pGroup->InvalidateBoundingRect();
	Op->DoInvalidateNodeRegion(pGroup, FALSE, FALSE, FALSE);

	// Post import
	if ((TheDocument!=NULL) && (pNode!=NULL))
		TheDocument->PostImport();

	// If it worked, but there was something we did not understand, explain to the user
	if ((Worked) && (!AllUnderstood))
	{
		Error::SetError(_R(IDS_MEATFILE_WARN), 0);
		InformWarning();
		Error::ClearError();
	}

	return Worked;
}


/********************************************************************************************

>	BOOL MetaFileFilter::MetaFileHeaderIsOk(METAHEADER *pHeader)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/03/94
	Inputs:		pHeader - the header to check.
	Returns:	TRUE if the header looks ok;
				FALSE if not.
	Purpose:	Evaluate a metafile header structure, to see if it looks like a real
				metafile.  This is done as part of the auto-recognition process.
	SeeAlso:	MetaFileFilter::HowCompatible

********************************************************************************************/
BOOL MetaFileFilter::MetaFileHeaderIsOk(METAHEADER *pHeader)
{
	return( (pHeader->mtType == 1) 				&&	// disk-based

			(pHeader->mtHeaderSize >= 9)		&&
			(pHeader->mtHeaderSize < 100)		&&	// assumes header wont get huge

			(pHeader->mtSize >= 9)				&&	// minimum file size
			(pHeader->mtSize < 0xFF000000L)   	&&  // assume <24 bit file length

			(pHeader->mtVersion >= 0x200));			// allow Windows 2 MFs

}


/********************************************************************************************

>	INT32 MetaFileFilter::HowCompatible(PathName& Filename, ADDR HeaderStart, 
									  UINT32 HeaderSize, UINT32 FileSize)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/03/94
	Inputs:		Filename - name of the file.
				HeaderStart - Address of the first few bytes of the file.
				HeaderSize - the number of bytes in the header pointed to by FileStart.
				FileSize - the size of the whole file, in bytes.
	Returns:	A number between 0 and 10 - see base class version for details
	Purpose:	Find out how compatible a file is with this import filter, i.e. does it look
				like a metafile?  It evaluates the header structure, and checks the file
				extension.
	SeeAlso:	Filter::HowCompatible; MetaFilefilter::MetaFileHeaderIsOk

********************************************************************************************/
INT32 MetaFileFilter::HowCompatible(PathName& Filename, ADDR HeaderStart, UINT32 HeaderSize,
					   			  UINT32 FileSize)
{
	// Check to see if this is a placeable metafile.
	METAFILEHEADER *pMetaFileHeader = (METAFILEHEADER *) HeaderStart;

	if (pMetaFileHeader->key == METAFILEHEADERKEY)
		// It's a metafile
		return 10;

	// Not a placeable metafile - examine header for sane METAHEADER structure.
	METAHEADER *pMetaHeader = (METAHEADER *) HeaderStart;

	if (MetaFileHeaderIsOk(pMetaHeader))
	{
		// Could be a metafile - check extension
		String Extension = Filename.GetType();
		if (_tcsicmp((TCHAR *) Extension, "wmf") == 0)
			// Likely to be a metafile
			return 9;
		else
			// Could still be a metafile, but less likely
			return 5;
	}
	else
		// We can't handle this file
		return 0;
}


/********************************************************************************************

>	HMETAFILE MetaFileFilter::GetMetaFileHandle(LPSTR Filename, METADATA *pMetaData)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/03/94
	Inputs:		Filename - name of the file to attempt to load as a metafile.
	Outputs:	pMetaData - various data about the metafile, if it was loaded successfully.
	Returns:	Handle to the loaded metafile, or NULL if an error occured.
	Purpose:	Given a filename, obtain a handle to the metafile, and provide basic info
				about the metafile. (Based on code written by Andy for MetaView).
	Errors:		Unable to open file; Unable to read enough data from file; Header looks
				invalid; Out of memory.
	SeeAlso:	METADATA

********************************************************************************************/
HMETAFILE MetaFileFilter::GetMetaFileHandle(LPSTR Filename, METADATA *pMetaData)
{
	OFSTRUCT OpenBuf;

	// Try to open the file
	OpenBuf.cBytes = sizeof(OpenBuf);
	InFile = OpenFile(Filename, &OpenBuf, OF_READ);

	// Did it work ok?
	if (InFile == HFILE_ERROR)
		return NULL;

	// Read in the first dword of the file to see if this is a placeable metafile
	INT32 BytesRead = _lread(InFile, &pMetaData->Header, sizeof(DWORD));

	if (BytesRead != sizeof(DWORD))
	{
		// Unable to read from the file - return error.
		_lclose(InFile);
		InFile = 0;
		return NULL;
	}

	METAHEADER MetaHeader;

	// Is this a placeable metafile?
	if (pMetaData->Header.key != METAFILEHEADERKEY)
	{

		// Not placable - if it looks OK, let the system read it
		// Go to the start of the file, read the header in, and close the file.
		_llseek(InFile, 0L, 0);
		_lread(InFile, &MetaHeader, sizeof(MetaHeader));
		_lclose(InFile);
		InFile = 0;

		if (!MetaFileHeaderIsOk(&MetaHeader))
			// Can't handle this file - return ERROR
			return NULL;

		// Looks ok - get the system to try and read it
		pMetaData->Placeable = FALSE;
		return ::GetMetaFile(Filename);
	}

	// Read the placeable header in.
	BytesRead = _lread(InFile, (LPVOID) &(pMetaData->Header.hmf), sizeof(METAFILEHEADER)-sizeof(DWORD));

	// Did it work?
	if (BytesRead!=(sizeof(METAFILEHEADER)-sizeof(DWORD)))
	{
		// No - return error
		_lclose(InFile);
		InFile = 0;
		return NULL;
	}

	// Read standard metafile header in.
	BytesRead = _lread(InFile, (LPSTR) &MetaHeader, sizeof(METAHEADER));

	// Did it work?
	if (BytesRead != sizeof(METAHEADER))
	{
		// No - return error.
		_lclose(InFile);
		InFile = 0;
		return NULL;
	}

	// Allocate memory for the metafile bits.
	GLOBALHANDLE MemHandle = GlobalAlloc(GHND, (MetaHeader.mtSize * 2L));

	if (MemHandle == NULL)
	{
		// Didn't get the memory - return error
		_lclose(InFile);
		InFile = 0;
		return NULL;
	}

	// Lock the memory.
	LPSTR MemPtr = (LPSTR) GlobalLock(MemHandle);

	if (MemPtr == NULL)
	{
		// Unable to lock memory - return error
      	GlobalFree(MemHandle);
		_lclose(InFile);
		InFile = 0;
		return NULL;
	}

	// Seek past the header
	_llseek(InFile, sizeof(METAFILEHEADER), 0);

	// Read metafile bits.
	BytesRead = _lread(InFile, MemPtr, (UINT32)(MetaHeader.mtSize * 2));


	// Did we read the correct number of bytes?
	if ((UINT32) BytesRead == (MetaHeader.mtSize * 2))
	{
		// Read the data ok - set the metafile bits.
		HMETAFILE MetaFileHandle;
		MetaFileHandle = SetMetaFileBitsEx(BytesRead, (LPBYTE) MemPtr);

		if (MetaFileHandle != NULL)
		{
			// At last - it's all worked; return the metafile handle
			pMetaData->Placeable = TRUE;
			return MetaFileHandle;
		}
	}

	// Failure - clean up and return error.
   	GlobalUnlock(MemHandle);
   	GlobalFree(MemHandle);
	_lclose(InFile);
	InFile = 0;

	return NULL;
}

/********************************************************************************************

<	POLYPOLYGON_INFO

	Comment:	Used to decode metafile records when importing metafiles.
	SeeAlso:	MetaFileFilter; MetaFileFilter::DecodePolyPolygon

********************************************************************************************/
typedef struct
{
	INT16 PolyCount;
	INT16 VertCounts[1];
} POLYPOLYGON_INFO16;


/********************************************************************************************

>	BOOL MetaFileFilter::DecodeBitBlt(METARECORD* pMetaRec)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/5/95
	Inputs:		pMetaRec - the metafile records that holds the info about the bitmap
	Returns:	TRUE if the bitmap was decoded, FALSE if there was a problem
	Purpose:	Decodes the bitmap from the metafile and creates a NodeBitmap in the
				tree of the correct size etc.

********************************************************************************************/
BOOL MetaFileFilter::DecodeBitBlt(METARECORD* pMetaRec)
{
	TRACEUSER( "Jonathan", _T("Decoding Bit Blt\n"));
	AllUnderstood = FALSE;
	return TRUE;
}


/********************************************************************************************

>	BOOL MetaFileFilter::DecodeStretchBlt(METARECORD* pMetaRec)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/5/95
	Inputs:		pMetaRec - the metafile records that holds the info about the bitmap
	Returns:	TRUE if the bitmap was decoded, FALSE if there was a problem
	Purpose:	Decodes the bitmap from the metafile and creates a NodeBitmap in the
				tree of the correct size etc.

********************************************************************************************/

BOOL MetaFileFilter::DecodeStretchBlt(METARECORD* pMetaRec)
{
	TRACEUSER( "Jonathan", _T("Decoding Stretch Blt\n"));
	AllUnderstood = FALSE;
	return TRUE;
}


/********************************************************************************************

>	BOOL MetaFileFilter::DecodeDIBToDevice(METARECORD* pMetaRec)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/5/95
	Inputs:		pMetaRec - the metafile records that holds the info about the bitmap
	Returns:	TRUE if the bitmap was decoded, FALSE if there was a problem
	Purpose:	Decodes the bitmap from the metafile and creates a NodeBitmap in the
				tree of the correct size etc.

********************************************************************************************/
BOOL MetaFileFilter::DecodeDIBToDevice(METARECORD* pMetaRec)
{
	TRACEUSER( "Jonathan", _T("Decoding Set DIB To Device\n"));
	AllUnderstood = FALSE;
	return TRUE;
}


/********************************************************************************************

>	BOOL MetaFileFilter::DecodeDIBBitBlt(METARECORD* pMetaRec)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/5/95
	Inputs:		pMetaRec - the metafile records that holds the info about the bitmap
	Returns:	TRUE if the bitmap was decoded, FALSE if there was a problem
	Purpose:	Decodes the bitmap from the metafile and creates a NodeBitmap in the
				tree of the correct size etc.

********************************************************************************************/
BOOL MetaFileFilter::DecodeDIBBitBlt(METARECORD* pMetaRec)
{
	TRACEUSER( "Jonathan", _T("Decoding DIB Bit Blt\n"));
	AllUnderstood = FALSE;
	return TRUE;
}


/********************************************************************************************

>	BOOL MetaFileFilter::DecodeDIBStretchBlt(METARECORD* pMetaRec)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/5/95
	Inputs:		pMetaRec - the 16bit metafile record
	Returns:	TRUE if all went well, FALSE if there was a problem
	Purpose:	Decodes the StretchBlt function in a Metafile. It builds a NodeBitmap and
				adds it to the tree.

********************************************************************************************/
BOOL MetaFileFilter::DecodeDIBStretchBlt(METARECORD* pRec)
{
	TRACEUSER( "Jonathan", _T("Decoding DIBSTRETCHBLT\n"));
	AllUnderstood = FALSE;
/*
	// Get the bitmap info block
	LPWORD pWords = (LPWORD) pRec->rdParm;
	
	// Step over the Rop field and all the src coord fields
	// There are 6 words worth of this in a StrecthBlt
	pWords+=6;
	
	// Read in all the coords for the bitmap
	INT16 DestYExt = (UINT16) *pWords++;
	INT16 DestXExt = (UINT16) *pWords++;
	INT16 DestY = (UINT16) *pWords++;
	INT16 DestX = (UINT16) *pWords++;

	// Get at the bitmap info structure, and the bitmap info header in that
	BITMAPINFO* pBmpInfo = (BITMAPINFO*) pWords;
	KernelBitmap* pBitmap = CreateBitmap(pBmpInfo);

	// There was an error, so send it up
	if (pBitmap==NULL)
		return FALSE;

	// First, set the rectangle to the right size for the bitmap...
	DocCoord Position((INT32)DestX, (INT32)DestY);
	DocCoord Size(Position.x + (INT32)DestXExt, Position.y + (INT32)DestYExt);

	// Get them in DocCoords
	TransformCoord(&Position);
	TransformCoord(&Size);

	// Build a rectangle
	DocRect BoundsRect;
	BoundsRect.lo.x = min(Position.x, Size.x);
	BoundsRect.lo.y = min(Position.y, Size.y);
	BoundsRect.hi.x = max(Position.x, Size.x);
	BoundsRect.hi.y = max(Position.y, Size.y);
				 Poly
	// Create a NodeBitmap to put it in
	NodeBitmap* pNodeBitmap = BuildNodeBitmap(pBitmap, BoundsRect);
*/
	// return Happy
	return TRUE;
}


/********************************************************************************************

>	BOOL MetaFileFilter::DecodeStretchDIB(METARECORD* pRec)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/5/95
	Inputs:		pRec - the Metafile record
	Returns:	TRUE if it decoded the record ok, FALSE if there was a problem
	Purpose:	Decodes the StretchDIB record from a metafile and makes a NodeBitmap object
				out of it.

********************************************************************************************/
BOOL MetaFileFilter::DecodeStretchDIB(METARECORD* pRec)
{
	TRACEUSER( "Jonathan", _T("Decoding Stretch DIB\n"));

	// Get the bitmap info block
	LPWORD pWords = (LPWORD) pRec->rdParm;
	
	// Step over the Rop field and all the src coord fields
	// there are 7 words worth in a StretchDIBits function
	pWords+=7;
	
	// Read in all the coords for the bitmap
	INT16 DestYExt = (UINT16) *pWords++;
	INT16 DestXExt = (UINT16) *pWords++;
	INT16 DestY = (UINT16) *pWords++;
	INT16 DestX = (UINT16) *pWords++;

	// Get at the bitmap info structure, and the bitmap info header in that
	BITMAPINFO* pBmpInfo = (BITMAPINFO*) pWords;
	KernelBitmap* pBitmap = CreateBitmap(pBmpInfo);

	// There was an error, so send it up
	if (pBitmap==NULL)
		return FALSE;

	// First, set the rectangle to the right size for the bitmap...
	DocCoord Position((INT32)DestX, (INT32)DestY);
	DocCoord Size(Position.x + (INT32)DestXExt, Position.y + (INT32)DestYExt);

	// Get them in DocCoords
	TransformCoord(&Position);
	TransformCoord(&Size);

	// Build a rectangle
	DocRect BoundsRect;
	BoundsRect.lo.x = min(Position.x, Size.x);
	BoundsRect.lo.y = min(Position.y, Size.y);
	BoundsRect.hi.x = max(Position.x, Size.x);
	BoundsRect.hi.y = max(Position.y, Size.y);

	// Create a NodeBitmap to put it in
	NodeBitmap* pNodeBitmap = BuildNodeBitmap(pBitmap, BoundsRect);

	// return Happy
	return TRUE;
}


/********************************************************************************************

>	KernelBitmap* MetaFileFilter::CreateBitmap(BITMAPINFO* pBitmapStart)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/5/95
	Inputs:		pBitmapStart - the start of the bitmap info.
	Returns:	A pointer to a KernelBitmap, or NULL if one could not be created
	Purpose:	This function expects to find the BITMAPINFO structure at the address passed
				in, immediatly followed by the Bitmap Bits (ie that actual bitmap data).
				This function will try to extract the bitmap info and the bitmap bits. It
				will also try to allocate memory to put a copy of the bitmap info into, and
				a seperate block of memory to put the bits in. It is assumed that the pointer
				passed in was found from inside a metafile so the memory is not owned by us.
				We need to make a copy so that we can keep the bitmap after the metafile is
				closed and thrown away by windows.
	Errors:		Error _R(IDS_OUT_OF_MEMORY) if there is not enough memory for the new bitmap

********************************************************************************************/
KernelBitmap* MetaFileFilter::CreateBitmap(BITMAPINFO* pBitmapStart)
{
	// Find out about the bitmap header
	BITMAPINFOHEADER Header = pBitmapStart->bmiHeader;
	LPBYTE pData = NULL;

	// Allocate some ram for the bitmap
	LPBITMAPINFO pInfo = AllocDIB(Header.biWidth, Header.biHeight, Header.biBitCount, &pData);

	// See if it worked
	if ((pInfo==NULL) || (pData==NULL))
		return NULL;

	// work out if there is a palette etc and if so, how big it is.
	INT32 OldPalSize = Header.biClrUsed * sizeof(RGBQUAD);
	INT32 NewPalSize = pInfo->bmiHeader.biClrUsed * sizeof(RGBQUAD);
	ERROR3IF(OldPalSize!=NewPalSize, "Palettes are of a different size");

	// Work out where the real bits start
	LPBYTE pBitsStart = (LPBYTE) pBitmapStart;
	pBitsStart += sizeof(BITMAPINFOHEADER) + OldPalSize;

	// copy the bits into our new bitmap
	memcpy(pData, pBitsStart, Header.biSizeImage);

	// Copy the palette
	memcpy(pInfo->bmiColors, pBitmapStart->bmiColors, NewPalSize);

	// Finally create the WinBitmap with all this in it
	WinBitmap* pWinBitmap = new WinBitmap(pInfo, pData);

	// Check for out of memory
	if (pWinBitmap==NULL)
		return NULL;
	
	// Create a new KernelBitmap to put the WinBitmap into
	KernelBitmap* pKernelBmp = new KernelBitmap(pWinBitmap, FALSE);
	if (pKernelBmp==NULL)
	{
		delete pWinBitmap;
		return NULL;
	}

	// return the bitmap
	return pKernelBmp;
}


/********************************************************************************************

>	NodeBitmap* MetaFileFilter::BuildNodeBitmap(KernelBitmap* pBitmap, const DocRect& Position)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/5/95
	Inputs:		pBitmap - the bitmap to put in the NodeBitmap
				Position - the rect that the bitmap will fill.
	Returns:	A pointer to a NodeBitmap, or NULL if there was a problem
	Purpose:	This function creates a NodeBitmap and attaches the Kernel bitmap to it.
				When it creates the Node, it attaches it to the correct place in the sub
				tree that we are building.

********************************************************************************************/
NodeBitmap* MetaFileFilter::BuildNodeBitmap(KernelBitmap* pBitmap, const DocRect& Position)
{
	// Create a NodeBitmap to put it in
	NodeBitmap *pNodeBitmap = new NodeBitmap;

	if (pNodeBitmap==NULL)
		return NULL;

	// try and set up the bitmap
	if (!pNodeBitmap->SetUpPath(12,12))
	{
		// Destroy the unwanted nodes
		pNodeBitmap->CascadeDelete();
		delete pNodeBitmap;
		return NULL;
	}

	// Make the node bitmap use the bitmap we have found
	pNodeBitmap->GetBitmapRef()->Attach(pBitmap, GetDocument());
	if (pNodeBitmap->GetBitmap() != pBitmap)
	{
		// It didn't use the bitmap we gave it, so we can delete it
		delete pBitmap;
	}

	// And set this in our bitmap node
	pNodeBitmap->CreateShape(Position);

	// And apply the default Bitmap attributes
	pNodeBitmap->ApplyDefaultBitmapAttrs(NULL);

	// return the bitmap we have made
	AddNodeToMetaFileGroup(pNodeBitmap);

	return pNodeBitmap;
}


/********************************************************************************************

>	BOOL MetaFileFilter::DecodePolyPolygon(METARECORD *pRec)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/03/94
	Inputs:		pRec - the poly-polygon record to decode.
	Returns:	TRUE if the poly-polygon was decoded successfuly;
				FALSE otherwise.
	Purpose:	Given a Metafile record for a 'poly-polygon', decode it and convert it into
				a Camelot path.
	Errors:		Out of memory.
	SeeAlso:	MetaFileFilter; MetaFileFilter::DecodeMetaFile

********************************************************************************************/
BOOL MetaFileFilter::DecodePolyPolygon(METARECORD *pRec)
{
	BOOL retvalue;

	// Get friendly pointer to the polypolygon data.
	POLYPOLYGON_INFO16 *pPolyInfo = (POLYPOLYGON_INFO16 *) pRec->rdParm;

	// Used to read coordinates from the data structure
	DocCoord Coord;

	// Used to set the bound rectangle of the path.
	DocRect BoundingRect;

	// Find out how many polygons there are
	UINT32 NumPolys = (UINT32) pPolyInfo->PolyCount;

 	// Find out how big this path is so we can give the path class a hint about how big
 	// to make it's initial arrays.
 
 	// Get pointer to start of polygon vertex counts
 	INT16 *pVertCount = pPolyInfo->VertCounts;
 
 	UINT32 NumSlots = 0;
 
 	// Iterate through all the polygons to find the number of slots for this path.
 	while (NumPolys > 0)
 	{
 		// Find out how many points there are
 		NumSlots += (UINT32) *pVertCount;
 
 		// Try next polygon
 		pVertCount++;
 		NumPolys--;
 	}

	// Create a new NodePath object
	NodePath *pPath = new NodePath;
	if (pPath==NULL)
		return FALSE;

	// try and set up the path
	if (!pPath->SetUpPath(NumSlots))
	{
		// Couldn't initialise path - delete path (if created) and return error.
		// Destroy any half-created paths
		if (pPath != NULL)
			pPath->CascadeDelete();
		
		// delete the unwanted path
		delete pPath;

		// Set the error for the caller of EnumMetaFile() to report
		return FALSE;
	}

	// Position tag at start of path.
	pPath->InkPath.FindStartOfPath();

 	// Reposition the vertex count pointer, and polygon count
 	pVertCount = pPolyInfo->VertCounts;
 	NumPolys = (UINT32) pPolyInfo->PolyCount;

	// Get pointer to start of polygon points
	POINT_16 *pCoord = (POINT_16 *) (pPolyInfo->VertCounts + NumPolys);

	// Iterate through all the polygons.
	while (NumPolys > 0)
	{
		// Find out how many points there are
		UINT32 NumPoints = (UINT32) *pVertCount;

		// Add the moveto to the path
		Coord.x = pCoord->x;
		Coord.y = pCoord->y;
		TransformCoord(&Coord);
		if (!pPath->InkPath.InsertMoveTo(Coord))
			// Not enough dynamic heap to insert the moveto command
			goto NoMemory;

		// Move past the moveto command
		pCoord++;
		NumPoints--;

		while (NumPoints > 0)
		{
			Coord.x = pCoord->x;
			Coord.y = pCoord->y;
			TransformCoord(&Coord);
			if (!pPath->InkPath.InsertLineTo(Coord))
				// Not enough dynamic heap to insert the moveto command
				goto NoMemory;

			// Try next coordinate
			pCoord++;
			NumPoints--;
		}

		// Try next polygon
		pVertCount++;
		NumPolys--;
	}

	// Remember current point
	pCoord--;
	CurrentPoint.x = pCoord->x;
	CurrentPoint.y = pCoord->y;

	// Terminate path properly.
	if (!pPath->InkPath.CloseSubPath())
		// Not enough dynamic heap to insert the closepath command
		goto NoMemory;

	// Make sure that we have a vald path here...
	BOOL ChangesMade;
	if (!pPath->InkPath.EnsureValid(&ChangesMade))
	{
		// We could not fix this path, so scrap it
		TRACE( _T("dpp Found a path that was so badly damaged we had to throw it away\n"));
		Error::ClearError();

		// Destroy the path
		if (pPath != NULL)
			pPath->CascadeDelete();

		// delete the unwanted path
		delete pPath;

		// and return without error
		return TRUE;
	}

	#ifdef _DEBUG
		// See if we had to make any changes to the path
		if (ChangesMade)
		{
			// yep, we sure did - tell Rik all about it
			TRACE( _T("We had to change this path as it was corrupt\n"));
		}
	#endif

	// Find out what fill colour we have (safe to do this as we only have flat fills
	// in metafiles), and use this to set the 'filled' flag on the path.
	FlatFillAttribute *pAttr;
	pAttr = (FlatFillAttribute *) CurrentAttrs[ATTR_FILLGEOMETRY].pAttr;
	ENSURE(pAttr->IsKindOf(CC_RUNTIME_CLASS(FlatFillAttribute)),
			"Bad fill type in metafile filter");

	pPath->InkPath.IsFilled = !(pAttr->Colour.IsTransparent());

	// Add attributes
	retvalue = AddAttributes(pPath);

	AddNodeToMetaFileGroup(pPath);

	return retvalue;

NoMemory:
	// Error - Out of memory while reading metafile
	
	// Destroy any half-created paths
	if (pPath != NULL)
		pPath->CascadeDelete();
		
	// delete the unwanted path
	delete pPath;

	// Set the error for the caller of EnumMetaFile() to report
	return FALSE;
}

/********************************************************************************************

>	BOOL MetaFileFilter::DecodePolygon(METARECORD *pRec, BOOL FillPolygon)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/03/94
	Inputs:		pRec - the polygon record to decode.
				FillPolygon - TRUE if the polygon should be filled with the current fill
				colour, FALSE if it should be just an outline
	Returns:	TRUE if the polygon was decoded successfuly;
				FALSE otherwise.
	Purpose:	Given a Metafile record for a polygon, decode it and convert it into
				a Camelot path. This function is used to decode the Polygon and PolyLine
				records in a metafile. The only difference between the two is that polygons
				are filled and polylines are not, hence the FillPolygon flag that gets passed
				in
	Errors:		Out of memory.
	SeeAlso:	MetaFileFilter; MetaFileFilter::DecodeMetaFile

********************************************************************************************/

BOOL MetaFileFilter::DecodePolygon(METARECORD *pRec, BOOL FillPolygon)
{
	DocRect BoundingRect;
	BOOL retvalue;

	// Get pointer to the polygon structure
	INT16 *pNumPoints;
	pNumPoints = (INT16 *) (pRec->rdParm);

	// Find out how many points there are
	UINT32 NumPoints;
	NumPoints = (UINT32) *pNumPoints;

	// Ignore anything with less than 2 points
	if (NumPoints < 2)
		return TRUE;

	// Create a new NodePath pbject
	NodePath *pPath = new NodePath;
	if (pPath==NULL)
		return FALSE;

	// Try and set it up
	if (!pPath->SetUpPath(NumPoints))
	{
		// Couldn't initialise path - delete path (if created) and return error.
		// Destroy any half-created paths
		if (pPath != NULL)
			pPath->CascadeDelete();
		
		// delete the unwanted path
		delete pPath;

		// Set the error for the caller of EnumMetaFile() to report
		return FALSE;
	}

	// Position tag at start of path.
	pPath->InkPath.FindStartOfPath();

	// Get pointer to the first coord
	POINT_16 *pCoord = (POINT_16 *) (pNumPoints + 1);
				
	// Add the moveto to the path
	DocCoord Coord;
	Coord.x = pCoord->x;
	Coord.y = pCoord->y;
	TransformCoord(&Coord);
	if (!pPath->InkPath.InsertMoveTo(Coord))
		// Not enough dynamic heap to insert the moveto command
		goto NoMemory;

	// Move past the moveto command
	pCoord++;
	NumPoints--;

	while (NumPoints > 0)
	{
		Coord.x = pCoord->x;
		Coord.y = pCoord->y;
		TransformCoord(&Coord);
		if (!pPath->InkPath.InsertLineTo(Coord))
			// Not enough dynamic heap to insert the moveto command
			goto NoMemory;

		// Try next coordinate
		pCoord++;
		NumPoints--;
	}

	// Remember current point
	pCoord--;
	CurrentPoint.x = pCoord->x;
	CurrentPoint.y = pCoord->y;

	// Terminate path properly.
	if ((FillPolygon) && (!pPath->InkPath.CloseSubPath()))
		// Not enough dynamic heap to insert the closepath command
		goto NoMemory;

	// Make sure that we have a vald path here...
	BOOL ChangesMade;
	if (!pPath->InkPath.EnsureValid(&ChangesMade))
	{
		// We could not fix this path, so scrap it
		TRACE( _T("dp Found a path that was so badly damaged we had to throw it away\n"));
		Error::ClearError();

		// Destroy the path
		if (pPath != NULL)
			pPath->CascadeDelete();

		// delete the unwanted path
		delete pPath;

		// and return without error
		return TRUE;
	}

	#ifdef _DEBUG
		// See if we had to make any changes to the path
		if (ChangesMade)
		{
			// yep, we sure did - tell Rik all about it
			TRACE( _T("We had to change this path as it was corrupt\n"));
		}
	#endif

	if (FillPolygon)
	{
		// Find out what fill colour we have (safe to do this as we only have flat fills
		// in metafiles), and use this to set the 'filled' flag on the path.
		FlatFillAttribute *pAttr;
		pAttr = (FlatFillAttribute *) CurrentAttrs[ATTR_FILLGEOMETRY].pAttr;
		ENSURE(pAttr->IsKindOf(CC_RUNTIME_CLASS(FlatFillAttribute)),
				"Bad fill type in metafile filter");

		pPath->InkPath.IsFilled = !(pAttr->Colour.IsTransparent());
	}
	else
	{
		// Make sure that the path is not filled
		pPath->InkPath.IsFilled = FALSE;
	}

	// Add attributes
	retvalue = AddAttributes(pPath);

	AddNodeToMetaFileGroup(pPath);

	return retvalue;
		
NoMemory:
	// Error - Out of memory while decoding polygon
	
	// Destroy any half-created paths
	if (pPath != NULL)
		pPath->CascadeDelete();
		
	// delete the unwanted path
	delete pPath;

	// Set the error for the caller of EnumMetaFile() to report
	return FALSE;
}



/********************************************************************************************

>	BOOL MetaFileFilter::DecodeLineTo(METARECORD* pMetaRec)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/9/95
	Inputs:		pMetaRec - the metafile record
	Returns:	TRUE if it decoded the LineTo ok, FALSE if not
	Purpose:	Decodes the metafile LineTo function and updates the current position

********************************************************************************************/

BOOL MetaFileFilter::DecodeLineTo(METARECORD* pMetaRec)
{
	// The current position should hold the coord for the last point drawn (eg, after a
	// moveto or a previous line to.

	// Create a new NodePath object to put the line it
	NodePath *pPath = new NodePath;
	if (pPath==NULL)
		return FALSE;

	// Try and set up the path
	if (!pPath->SetUpPath(2))
	{
		// Couldn't initialise path - delete path (if created) and return error.
		pPath->CascadeDelete();
		delete pPath;
		return FALSE;
	}

	// Position tag at start of path.
	pPath->InkPath.FindStartOfPath();
	pPath->InkPath.IsFilled = FALSE;

	// Insert a move to to the current point
	if (!pPath->InkPath.InsertMoveTo(CurrentPoint))
	{
		// Clean it up
		pPath->CascadeDelete();
		delete pPath;
		return FALSE;
	}

	// Decode the coord for the line to and update the current point
	CurrentPoint.y = (INT16) pMetaRec->rdParm[0];
	CurrentPoint.x = (INT16) pMetaRec->rdParm[1];
	TransformCoord(&CurrentPoint);

	// Insert a lineto to the new current point
	if (!pPath->InkPath.InsertLineTo(CurrentPoint))
	{
		// Clean it up
		pPath->CascadeDelete();
		delete pPath;
		return FALSE;
	}

	// Make sure that we have a vald path here...
	BOOL ChangesMade;
	if (!pPath->InkPath.EnsureValid(&ChangesMade))
	{
		// We could not fix this path, so scrap it
		TRACE( _T("dl Found a path that was so badly damaged we had to throw it away\n"));
		Error::ClearError();
		pPath->CascadeDelete();
		delete pPath;
		
		// This path had nothing useful in it, so just return
		return TRUE;
	}

	#ifdef _DEBUG
		// See if we had to make any changes to the path
		if (ChangesMade)
		{
			// yep, we sure did - tell Rik all about it
			TRACE( _T("We had to change this path as it was corrupt\n"));
		}
	#endif

	// Add attributes
	BOOL retvalue = AddAttributes(pPath);

	AddNodeToMetaFileGroup(pPath);

	return retvalue;
}


/********************************************************************************************

>	BOOL MetaFileFilter::DecodeEllipse(METARECORD* pMetaRec)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/5/95
	Inputs:		pMetaRec - the metafile record
	Returns:	TRUE if it decoded the ellipse ok, FALSE if not
	Purpose:	Decodes the metafile ellipse function.

********************************************************************************************/

BOOL MetaFileFilter::DecodeEllipse(METARECORD* pMetaRec)
{
	TRACEUSER( "Jonathan", _T("Decoding Ellipse\n"));

	// Create a new NodePath pbject
	NodeEllipse *pElip = new NodeEllipse;
	if (pElip==NULL)
		return FALSE;

	// try and set up the ellipse
	if (!pElip->SetUpPath(12,12))
	{
		// Couldn't initialise path - delete path (if created) and return error.
		pElip->CascadeDelete();
		delete pElip;
		return FALSE;
	}

	// Get pointer to the first coord
	INT16 *pCoord = (INT16 *) (pMetaRec->rdParm);
				
	// Add the moveto to the path
	DocCoord TopRight((INT32) pCoord[1], (INT32) pCoord[2]);
	DocCoord BotLeft((INT32) pCoord[3], (INT32) pCoord[0]);
	TransformCoord(&TopRight);
	TransformCoord(&BotLeft);

	DocRect Rect(min(TopRight.x, BotLeft.x),
				 min(TopRight.y, BotLeft.y),
				 max(TopRight.x, BotLeft.x),
				 max(TopRight.y, BotLeft.y));

	// Create the ellipse
	pElip->CreateShape(Rect);
	pElip->InkPath.IsFilled = TRUE;

	// Add attributes
	BOOL retvalue = AddAttributes(pElip);

	AddNodeToMetaFileGroup(pElip);

	return retvalue;
}


/********************************************************************************************

>	BOOL MetaFileFilter::DecodeRectangle(METARECORD* pMetaRec)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/5/95
	Inputs:		pMetaRec - The metafile record
	Returns:	TRUE if we decoded it ok, FALSE if there was a problem
	Purpose:	Decodes the metafile rectangle command. We insert a Path into the tree of
				the correct size.

********************************************************************************************/

BOOL MetaFileFilter::DecodeRectangle(METARECORD* pMetaRec)
{
	BOOL retvalue;
	TRACEUSER( "Jonathan", _T("Decoding Rectangle\n"));

	// Create a new NodePath pbject
	NodePath *pPath = new NodePath;
	if (pPath==NULL)
		return FALSE;

	// try and set up the path
	if (!pPath->SetUpPath(12,12))
	{
		// Destroy any half-created paths
		if (pPath != NULL)
			pPath->CascadeDelete();
		
		// delete the unwanted path
		delete pPath;

		// Set the error for the caller of EnumMetaFile() to report
		return FALSE;
	}

	// Position tag at start of path.
	pPath->InkPath.FindStartOfPath();

	// Get pointer to the first coord
	INT16 *pCoord = (INT16 *) (pMetaRec->rdParm);
				
	// Add the moveto to the path
	DocCoord TopRight((INT32) pCoord[1], (INT32) pCoord[2]);
	DocCoord BotLeft((INT32) pCoord[3], (INT32) pCoord[0]);
	TransformCoord(&TopRight);
	TransformCoord(&BotLeft);

	DocRect Rect(min(TopRight.x, BotLeft.x),
				 min(TopRight.y, BotLeft.y),
				 max(TopRight.x, BotLeft.x),
				 max(TopRight.y, BotLeft.y));

	// Insert the move to
	DocCoord Coord;
	Coord.x = Rect.lo.x;
	Coord.y = Rect.lo.y;
	if (!pPath->InkPath.InsertMoveTo(Coord))
		// Not enough dynamic heap to insert the moveto command
		goto NoMemory;

	// Insert each of the line sections
	Coord.y = Rect.hi.y;
	if (!pPath->InkPath.InsertLineTo(Coord))
		// Not enough dynamic heap to insert the moveto command
		goto NoMemory;

	Coord.x = Rect.hi.x;
	if (!pPath->InkPath.InsertLineTo(Coord))
		// Not enough dynamic heap to insert the moveto command
		goto NoMemory;

	Coord.y = Rect.lo.y;
	if (!pPath->InkPath.InsertLineTo(Coord))
		// Not enough dynamic heap to insert the moveto command
		goto NoMemory;

	Coord.x = Rect.lo.x;
	if (!pPath->InkPath.InsertLineTo(Coord))
		// Not enough dynamic heap to insert the moveto command
		goto NoMemory;

	// Terminate path properly.
	if (!pPath->InkPath.CloseSubPath())
		// Not enough dynamic heap to insert the closepath command
		goto NoMemory;

	// Make sure that we have a vald path here...
	BOOL ChangesMade;
	if (!pPath->InkPath.EnsureValid(&ChangesMade))
	{
		// We could not fix this path, so scrap it
		TRACE( _T("Found a path that was so badly damaged we had to throw it away\n"));
		Error::ClearError();

		// Destroy the path
		if (pPath != NULL)
			pPath->CascadeDelete();

		// delete the unwanted path
		delete pPath;

		// and return without error
		return TRUE;
	}

	#ifdef _DEBUG
		// See if we had to make any changes to the path
		if (ChangesMade)
		{
			// yep, we sure did - tell Rik all about it
			TRACE( _T("We had to change this path as it was corrupt\n"));
		}
	#endif

	// Make sure that the path is not filled
	pPath->InkPath.IsFilled = TRUE;

	// Add attributes
	retvalue = AddAttributes(pPath);

	AddNodeToMetaFileGroup(pPath);

	return retvalue;

NoMemory:
	// Error - Out of memory while decoding polygon
	// Destroy any half-created paths
	if (pPath != NULL)
		pPath->CascadeDelete();

	// delete the unwanted path
	delete pPath;
		
	// Set the error for the caller of EnumMetaFile() to report
	return FALSE;
}

/********************************************************************************************

>	BOOL MetaFileFilter::DecodeTextStory(METARECORD* pMetaRec)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/5/95
	Inputs:		pMetaRec - the record to decode
	Returns:	TRUE if it worked, FALSE if there was a problem
	Purpose:	Decodes the TextOut function and makes a text story.

********************************************************************************************/

BOOL MetaFileFilter::DecodeTextStory(METARECORD* pMetaRec)
{
	// Get the bitmap info block
	LPWORD pWords = (LPWORD) pMetaRec->rdParm;
	
	// Read in the number of chars in the string
	INT16 Count = (UINT16) *pWords++;

	// Get a string
	char* pString = new char[Count+1];
	if (pString==NULL)
		return FALSE;
	
	// Copy the chars into our new string
	camStrncpy(pString, (char*)pWords, Count);
	pString[Count] = 0;
	TRACE( _T("- %s\n"), pString);

	// Find the coords
	while (Count>0)
	{
		pWords++;
		Count-=2;
	}

	// Get a coord to house the string
	DocCoord Position(0,0);

	// Get the coordinates
	Position.y = (INT16) *pWords++;
	Position.x = (INT16) *pWords++;

	// transform them
	TransformCoord(&Position);

	// Build the text story
	TRACEUSER( "Jonathan", _T("Text Story : %s\n"), pString);
	TextStory* pNewStory = TextStory::CreateFromChars(Position, pString, NULL, GetDocument(),
													  &SelectedFont, FALSE, &TextColour);
	// Clean up the string
	delete pString;

	// See if we have a new text story
	if (pNewStory==NULL)
		return FALSE;

	// Yes, put it in the tree
	AddNodeToMetaFileGroup(pNewStory);

	// Return
	return TRUE;
}



/********************************************************************************************

>	BOOL MetaFileFilter::DecodeExtTextStory(METARECORD* pMetaRec)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/7/95
	Inputs:		pMetaRec - the metafile record for the ExtTextOut
	Returns:	TRUE if all went well, FALSE if there was a problem
	Purpose:	Decodes the ExtTextOut function.

********************************************************************************************/

BOOL MetaFileFilter::DecodeExtTextStory(METARECORD* pMetaRec)
{
	// Get the info block
	LPWORD pWords = (LPWORD) pMetaRec->rdParm;
	
	// Get a coord to house the string
	DocCoord Position(0,0);

	// Get the coordinates
	Position.y = (INT16) *pWords++;
	Position.y -= SelectedFont.lfHeight;
	Position.x = (INT16) *pWords++;

	// transform the Coord
	TransformCoord(&Position);

	// Read in the number of chars in the string
	INT16 Count = (UINT16) *pWords++;

	// Read in the options flag
	INT16 Option = (UINT16) *pWords++;
	if (Option!=0)
	{
		// Read in the rectangle if its there
		pWords++;		// left
		pWords++;		// top
		pWords++;		// right
		pWords++;		// bottom
	}

	// Get a string
	char* pString = new char[Count+1];
	if (pString==NULL)
		return FALSE;
	
	// Copy the chars into our new string
	camStrncpy(pString, (char*)pWords, Count);
	pString[Count] = 0;
	TRACE( _T("%s\n"), pString);

	// Find the coords
	while (Count>0)
	{
		pWords++;
		Count-=2;
	}

	// Build the text story
	TextStory* pNewStory = TextStory::CreateFromChars(Position, pString, NULL, GetDocument(),
													  &SelectedFont, FALSE, &TextColour);
	// Clean up the string
	delete pString;

	// See if we have a new text story
	if (pNewStory==NULL)
		return FALSE;

	// Yes, put it in the tree
	AddNodeToMetaFileGroup(pNewStory);

	// Return
	return TRUE;
}




/********************************************************************************************

>	void MetaFileFilter::SetLogicalFont(LOGFONT* pNewFont)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/5/95
	Inputs:		pNewFont - the details about the newly selected font
	Purpose:	Makes a note of the selected font ready for when it is used.

********************************************************************************************/

void MetaFileFilter::SetLogicalFont(LOGFONT_16* pNewFont)
{
	// record the font
	// Copy the values from the 16 bit version to the 32 bit version
	SelectedFont.lfHeight = pNewFont->lfHeight;
	SelectedFont.lfWidth = pNewFont->lfWidth;
	SelectedFont.lfEscapement = pNewFont->lfEscapement;
	SelectedFont.lfOrientation = pNewFont->lfOrientation;
	SelectedFont.lfWeight = pNewFont->lfWeight;
	SelectedFont.lfItalic = pNewFont->lfItalic;
	SelectedFont.lfUnderline = pNewFont->lfUnderline;
	SelectedFont.lfStrikeOut = pNewFont->lfStrikeOut;
	SelectedFont.lfCharSet = pNewFont->lfCharSet;
	SelectedFont.lfOutPrecision = pNewFont->lfOutPrecision;
	SelectedFont.lfClipPrecision = pNewFont->lfClipPrecision;
	SelectedFont.lfQuality = pNewFont->lfQuality;
	SelectedFont.lfPitchAndFamily = pNewFont->lfPitchAndFamily;
	camStrcpy(SelectedFont.lfFaceName, pNewFont->lfFaceName);

	// Scale the text size according to the current mapping mode.
	switch (CurrentMappingMode)
	{
		case MM_TEXT:
			// Scale is in pixels
			SelectedFont.lfHeight = (SelectedFont.lfHeight * 96) / Dpi;
			SelectedFont.lfWidth = (SelectedFont.lfWidth * 96) / Dpi;
			break;

		case MM_LOMETRIC:
			// Scale of 0.1mm per unit
			// Convert to 96 dpi
			SelectedFont.lfHeight = (SelectedFont.lfHeight * 96) / 254;
			SelectedFont.lfWidth = (SelectedFont.lfWidth * 96) / 254;
			break;

		case MM_HIMETRIC:
			// Scale of 0.01mm per unit
			// Convert to millipoints: n * (72000 / (100 * 25.4))
			SelectedFont.lfHeight = (SelectedFont.lfHeight * 96) / 2540;
			SelectedFont.lfWidth = (SelectedFont.lfWidth * 96) / 2540;
			break;

		case MM_LOENGLISH:
			// Scale of 0.01 inches per unit
			// Convert to millipoints: (n / 100) * 72000
			SelectedFont.lfHeight = (SelectedFont.lfHeight * 96) / 100;
			SelectedFont.lfWidth = (SelectedFont.lfWidth * 96) / 100;
			break;

		case MM_HIENGLISH:
			// Scale of 0.001 inches per unit
			// Convert to millipoints: (n / 1000) * 72000
			SelectedFont.lfHeight = (SelectedFont.lfHeight * 96) / 1000;
			SelectedFont.lfWidth = (SelectedFont.lfWidth * 96) / 1000;
			break;

		case MM_TWIPS:
			// Scale of 1/1440th of an inch per unit
			// Convert to millipoints: (n / 1440) * 72000
			SelectedFont.lfHeight = (SelectedFont.lfHeight * 96) / 1440;
			SelectedFont.lfWidth = (SelectedFont.lfWidth * 96) / 1440;
			break;

		default:
			SelectedFont.lfHeight = (SelectedFont.lfHeight * 96) / Dpi;
			SelectedFont.lfWidth = (SelectedFont.lfWidth * 96) / Dpi;
			break;
	}
}


/********************************************************************************************

>	INT32 MetaFileFilter::DecodeMetafileRecord( METARECORD FAR* pMetaRec )

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/03/94
	Inputs:
				pMetaRec - pointer to the metafile record to be decoded.
	Returns:	1 => Keep enumerating the metafile
				0 => An error occurred - stop enumerating the metafile now.
	Purpose:	Callback function for decoding metafiles.  This is called indirectly by the SDK function
				EnumMetaFile, and decodes each record that it is passed into the relevant
				Camelot object.
	Errors:		Out of memory; Syntax error/corruption in metafile (e.g. trying to select
				a non-existent object).
	SeeAlso:	MetaFileFilter

********************************************************************************************/

INT32 MetaFileFilter::DecodeMetafileRecord( METARECORD FAR* pMetaRec )
{
	NodePath    *pPath = NULL;
	DocCoord     Coord;
	LOGPEN_16 	*pPen;
	LOGBRUSH_16	*pBrush;
	DocRect		 BoundingRect;
	LOGFONT_16*	 pFont;
	LOGPALETTE_16* pPalette;

	// The rdSize field is the size of this record in words.
	BytesRead += pMetaRec->rdSize * 2;

	// Keep it less than the filesize
	if (BytesRead>FileSize)
		BytesRead = FileSize;

	// Update progress cursor every 2k
	#if WIN32
	if (BytesRead > (LastProgressUpdate + 2048))
	{
		if (!ContinueSlowJob(BytesRead))
		{
			// Abort operation - make sure nodes are deleted and not added to the tree.
			ERROR(_R(IDT_IMPORT_USERABORT), FALSE);
		}
		else
		{
			LastProgressUpdate = BytesRead;
		}
	}
	#endif

	// Deal with all the functions that can appear in a windows metafile
	switch (pMetaRec->rdFunction)
	{
        case META_SETTEXTCHAREXTRA:
			AllUnderstood = FALSE;
			TRACEUSER( "Jonathan", _T("SETTEXTCHAREXTRA\n"));
			break;

        case META_SETTEXTCOLOR:
		{
			// Find out what colour we are setting text to
			COLORREF* pColour = (COLORREF*) pMetaRec->rdParm;
			DocColour NewColour(INT32(GetRValue(*pColour)), INT32(GetGValue(*pColour)),
								INT32(GetBValue(*pColour)));

			// Set the text colour
			TextColour = NewColour;
			break;
		}

        case META_SETTEXTJUSTIFICATION:
			AllUnderstood = FALSE;
			TRACEUSER( "Jonathan", _T("SETTEXTJUSTIFICATION\n"));
			break;

        case META_SETTEXTALIGN:
			//AllUnderstood = FALSE;
			break;

        case META_EXTTEXTOUT:
			if (!DecodeExtTextStory(pMetaRec))
				return FALSE;
			break;

        case META_TEXTOUT:
			if (!DecodeTextStory(pMetaRec))
				return FALSE;
			break;



        case META_SETSTRETCHBLTMODE:
			//This has no meaning in Camelot
			break;

        case META_DIBBITBLT:
			if (!DecodeDIBBitBlt(pMetaRec))
				return FALSE;
			break;

        case META_DIBSTRETCHBLT:
			if (!DecodeDIBStretchBlt(pMetaRec))
				return FALSE;
			break;

        case META_STRETCHDIB:
			if (!DecodeStretchDIB(pMetaRec))
				return FALSE;
			break;

        case META_SETDIBTODEV:
			if (!DecodeDIBToDevice(pMetaRec))
				return FALSE;
			break;

        case META_BITBLT:
			if (!DecodeBitBlt(pMetaRec))
				return FALSE;
			break;

        case META_STRETCHBLT:
			if (!DecodeStretchBlt(pMetaRec))
				return FALSE;
			break;

        case META_PATBLT:
			AllUnderstood = FALSE;
			TRACEUSER( "Jonathan", _T("PATBLT\n"));
			break;

        case META_DIBCREATEPATTERNBRUSH:
			AllUnderstood = FALSE;
			TRACEUSER( "Jonathan", _T("DIBCREATEPATTERNBRUSH\n"));
			break;



		case META_SETWINDOWORG:
		{
			// If metafile is not placeable, use this information
			if (!Placeable)
			{
				// Remember params are in reverse order
				MetaFileOrigin.y = (INT16) pMetaRec->rdParm[0];
				MetaFileOrigin.x = (INT16) pMetaRec->rdParm[1];
				YShift = 0;
			}
			break;
		}

		case META_SETWINDOWEXT:
		{
			// If metafile is not placeable, use this information
			//if (Placeable)
			{
				INT16 YExtent = (INT16) pMetaRec->rdParm[0];
				TRACEUSER( "Jonathan", _T("Extent - x=%d, y=%d\n"), pMetaRec->rdParm[1], pMetaRec->rdParm[0]);
				if (YExtent<0 && !IsYExtentNegative)
					FlipYCoords = TRUE;

				if (YExtent>=0 && IsYExtentNegative)
					FlipYCoords = TRUE;

				if (FlipYCoords)
					YShift = 0;
			}
			break;
		}

        case META_OFFSETWINDOWORG:
			AllUnderstood = FALSE;
			TRACEUSER( "Jonathan", _T("OFFSETWINDOWORG\n"));
			break;

        case META_SCALEWINDOWEXT:
			AllUnderstood = FALSE;
			TRACEUSER( "Jonathan", _T("SCALEWINDOWEXT\n"));
			break;

        case META_OFFSETVIEWPORTORG:
			AllUnderstood = FALSE;
			TRACEUSER( "Jonathan", _T("OFFSETVIEWPORTORG\n"));
			break;

        case META_SCALEVIEWPORTEXT:
			AllUnderstood = FALSE;
			TRACEUSER( "Jonathan", _T("SCALEVIEWPORTEXT\n"));
			break;

		case META_SETVIEWPORTORG:
			AllUnderstood = FALSE;
			TRACEUSER( "Jonathan", _T("SetViewportOrg\n"));
			break;

		case META_SETVIEWPORTEXT:
			AllUnderstood = FALSE;
			TRACEUSER( "Jonathan", _T("SetViewportExt\n"));
			break;



        case META_SETPOLYFILLMODE:
			if (((INT32) pMetaRec->rdParm[0]) == WINDING)
			{
				// Non-zero winding rule
				SetWindingRule(NonZeroWinding);
			}
			else
			{
				// Even-odd winding
				SetWindingRule(EvenOddWinding);
			}
			break;

        case META_POLYGON:
			if (!DecodePolygon(pMetaRec, TRUE))
				// An error occured
				return FALSE;
			break;

        case META_POLYPOLYGON:
			if (!DecodePolyPolygon(pMetaRec))
				// An error occured
				return FALSE;
			break;

        case META_POLYLINE:
			// Try and decode the polyline
			if (!DecodePolygon(pMetaRec, FALSE))
				return FALSE;
			break;



        case META_ESCAPE:
			// Comment. Ignore this
			break;



        case META_SELECTOBJECT:
			if (!Handles->SelectObject(pMetaRec->rdParm[0]))
				return FALSE;
			break;

        case META_DELETEOBJECT:
			if (!Handles->DeleteObject(pMetaRec->rdParm[0]))
				return FALSE;
			break;

        case META_CREATEPALETTE:
			pPalette = (LOGPALETTE_16 *) pMetaRec->rdParm;
			if (!Handles->CreatePalette(pPalette))
				// Error occured while creating this pen
				return FALSE;
			break;

        case META_CREATEPATTERNBRUSH:
			if (!Handles->CreatePatternBrush(pMetaRec->rdParm))
				// Error occured while creating this pen
				return FALSE;
			break;

        case META_CREATEFONTINDIRECT:
			pFont = (LOGFONT_16 *) pMetaRec->rdParm;
			if (!Handles->CreateFontIndirect(pFont))
				// Error occured while creating this pen
				return FALSE;
			break;

        case META_CREATEREGION:
			if (!Handles->CreateRegion(pMetaRec->rdParm))
				// Error occured while creating this pen
				return FALSE;
			break;

        case META_CREATEPENINDIRECT:
			pPen = (LOGPEN_16 *) pMetaRec->rdParm;
			if (!Handles->CreatePen(pPen))
				// Error occured while creating this pen
				return FALSE;
			break;

        case META_CREATEBRUSHINDIRECT:
			pBrush = (LOGBRUSH_16 *) pMetaRec->rdParm;
			if (!Handles->CreateBrush(pBrush))
				// Error occured while creating this brush
				return FALSE;
			break;



        case META_LINETO:
			// Add a single straight line to the drawing
			if (!DecodeLineTo(pMetaRec))
				return FALSE;
			break;

        case META_MOVETO:
			// Remember params are in reverse order
			CurrentPoint.y = (INT16) pMetaRec->rdParm[0];
			CurrentPoint.x = (INT16) pMetaRec->rdParm[1];
			TransformCoord(&CurrentPoint);
			break;

        case META_FLOODFILL:
			// This has no meaning in Camelot
			break;

        case META_EXTFLOODFILL:
			// This has no meaning in Camelot
			break;

        case META_ARC:
			AllUnderstood = FALSE;
			TRACEUSER( "Jonathan", _T("Arc\n"));
			break;

        case META_ELLIPSE:
			if (!DecodeEllipse(pMetaRec))
				return FALSE;
			break;

        case META_PIE:
			AllUnderstood = FALSE;
			TRACEUSER( "Jonathan", _T("Pie\n"));
			break;

        case META_RECTANGLE:
			if (!DecodeRectangle(pMetaRec))
				return FALSE;
			break;

        case META_ROUNDRECT:
			if (!DecodeRectangle(pMetaRec))
				return FALSE;
			break;

        case META_CHORD:
			AllUnderstood = FALSE;
			TRACEUSER( "Jonathan", _T("Chord\n"));
			break;

        case META_SETPIXEL:
			// This has no meaning in Camelot
			break;



        case META_SELECTPALETTE:
			// This has no meaning in Camelot
			break;

        case META_REALIZEPALETTE:
			// This has no meaning in Camelot
			break;

        case META_ANIMATEPALETTE:
			// This has no meaning in Camelot
			break;

        case META_SETPALENTRIES:
			// This has no meaning in Camelot
			break;

        case META_RESIZEPALETTE:
			// This has no meaning in Camelot
			break;



        case META_FILLREGION:
			AllUnderstood = FALSE;
			TRACEUSER( "Jonathan", _T("FILLREGION\n"));
			break;

        case META_FRAMEREGION:
			AllUnderstood = FALSE;
			TRACEUSER( "Jonathan", _T("FRAMEREGION\n"));
			break;

        case META_INVERTREGION:
			AllUnderstood = FALSE;
			TRACEUSER( "Jonathan", _T("INVERTREGION\n"));
			break;

        case META_PAINTREGION:
			AllUnderstood = FALSE;
			TRACEUSER( "Jonathan", _T("PAINTREGION\n"));
			break;

        case META_SELECTCLIPREGION:
			AllUnderstood = FALSE;
			TRACEUSER( "Jonathan", _T("SELECTCLIPREGION\n"));
			break;



		case META_SAVEDC:
			// This has no meaning in Camelot
			break;

		case META_RESTOREDC:
			// This has no meaning in Camelot
			break;

        case META_SETROP2:
			// Find out what they want to set the pen to
			if ( ((INT32)pMetaRec->rdParm[0]) != R2_COPYPEN)
				TRACEUSER( "Jonathan", _T("SetROP2 - Not Copy Pen!\n"));
			break;



        case META_EXCLUDECLIPRECT:
			//This has no meaning in Camelot
			break;

        case META_INTERSECTCLIPRECT:
			//This has no meaning in Camelot
			break;

        case META_OFFSETCLIPRGN:
			//This has no meaning in Camelot
			break;



        case META_SETMAPPERFLAGS:
			AllUnderstood = FALSE;
			TRACEUSER( "Jonathan", _T("SETMAPPERFLAGS\n"));
			break;



        case META_SETBKCOLOR:
			//This has no meaning in Camelot
			break;

        case META_SETBKMODE:
			// This has no meaning in Camelot
			break;

        case META_SETMAPMODE:
			CurrentMappingMode = (INT32) ((INT16) pMetaRec->rdParm[0]);
			TRACEUSER( "Jonathan", _T("SetMapMode: %d\n"), CurrentMappingMode);
			break;

        case META_SETRELABS:
			// This record appears in a lot of WMF files but does not seem
			// to break anything if it is ignored so I am stopping this
			// causing a warning to the the user
//			AllUnderstood = FALSE;
			TRACEUSER( "Jonathan", _T("SETRELABS\n"));
			break;

		default:
			TRACEUSER( "Jonathan", _T("Unknown Function %x\n"), pMetaRec->rdFunction);
			return FALSE;
			break;
	}

	// Get the next metafile record;
	return 1;
}

/********************************************************************************************
>	void MetaFileFilter::TransformCoord(DocCoord *C)
	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/03/94
	Inputs:		C - the coordinate to transform
	Outputs:	C - the transformed coordinate.
	Purpose:	Transform a metafile coordinate into a Camelot coordinate, by compensating
				for the origin, and scaling the units.
				Use MetaFileFilter::ScaleCoord() to scale without performing an origin
				translation (useful for scaling dimensions).
	Errors:		ENSURE failure if a MM_ISOTROPIC or MM_ANISOTROPIC mapping mode is used.
	SeeAlso:	MetaFileFilter::ScaleCoord
********************************************************************************************/
void MetaFileFilter::TransformCoord(DocCoord *C)
{
	// Convert from metafile origin
	C->x -= MetaFileOrigin.x;
	C->y -= MetaFileOrigin.y;

	// Add in any offset that may be needed
	C->y += YShift;

 	// Scale it
	ScaleCoord(C);
}

/********************************************************************************************
>	void MetaFileFilter::ScaleCoord(DocCoord *C)
	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/03/94
	Inputs:		C - the coordinate to transform
	Outputs:	C - the transformed coordinate.
	Purpose:	Transform a metafile coordinate into a Camelot coordinate, by scaling the 
				units.  NB.  NO origin translation is performed.
	Errors:		ENSURE failure if a MM_ISOTROPIC or MM_ANISOTROPIC mapping mode is used.
	SeeAlso:	MetaFileFilter::TransformCoord
********************************************************************************************/

void MetaFileFilter::ScaleCoord(DocCoord *C)
{
	// Scale the coordinates
	if (Placeable)
	{
		// Scale the X Coord
		C->x *= ScaleFactor;
		C->y *= ScaleFactor;
	}
	else
	{
		switch (CurrentMappingMode)
		{
			case MM_TEXT:
				// Scale is in pixels
				// Convert to millipoints
				C->x *= ScaleFactor;
				C->y *= ScaleFactor;
				break;

			case MM_LOMETRIC:
				// Scale of 0.1mm per unit
				// Convert to millipoints: n * (72000 / (10 * 25.4))
				C->x *= (72000 / 254);
				C->y *= (72000 / 254);
				break;

			case MM_HIMETRIC:
				// Scale of 0.01mm per unit
				// Convert to millipoints: n * (72000 / (100 * 25.4))
				C->x *= (72000 / 2540);
				C->y *= (72000 / 2540);
				break;

			case MM_LOENGLISH:
				// Scale of 0.01 inches per unit
				// Convert to millipoints: (n / 100) * 72000
				C->x *= (72000 / 100);
				C->y *= (72000 / 100);
				break;

			case MM_HIENGLISH:
				// Scale of 0.001 inches per unit
				// Convert to millipoints: (n / 1000) * 72000
				C->x *= (72000 / 1000);
				C->y *= (72000 / 1000);
				break;

			case MM_TWIPS:
				// Scale of 1/1440th of an inch per unit
				// Convert to millipoints: (n / 1440) * 72000
				C->x *= (72000 / 1440);
				C->y *= (72000 / 1440);
				break;

			//case MM_ISOTROPIC:
			//	ENSURE(FALSE, "Can't cope with ISOTROPIC mapping in metafiles!");
			//	break;

			//case MM_ANISOTROPIC:
			//	ENSURE(FALSE, "Can't cope with ANISOTROPIC mapping in metafiles!");
			//	break;

			default:
				C->x *= ScaleFactor;
				C->y *= ScaleFactor;
				break;
		}
	}

	// Scale the Y Coord
	if (FlipYCoords)
		C->y = -C->y;
}

/********************************************************************************************
>	BOOL MetaFileFilter::AddAttributes(NodePath *pPath)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/93
	Inputs:		pPath - pointer to the NodePath object to add attributes to.
				pHeader - pointer to the Draw file object header which describes the path
				attributes.
	Returns:	TRUE if the attributes were added ok, FALSE if not.
	Purpose:	Add attributes to the path object specified.  The attributes are optimised
				so that if they are the same as the document's default attributes, they are
				not applied.
				The attributes applied are line and fill colour, and line style.
	Errors:		Fails (returns FALSE) if not enough memory to add attributes.
	Scope:		Private

********************************************************************************************/
BOOL MetaFileFilter::AddAttributes(Node* pNewNode)
{
	// If not filled, then set the ignore bit on the fill attribute.
	if (pNewNode->IS_KIND_OF(NodePath))
	{
		NodePath* pPath = (NodePath*) pNewNode;
		if (!pPath->InkPath.IsFilled)
			CurrentAttrs[ATTR_FILLGEOMETRY].Ignore = TRUE;
	}
		
	// Add attributes to the path, if they are different from the default...
	BOOL Result = AttributeManager::ApplyBasedOnDefaults(pNewNode, CurrentAttrs);

	// Enable the fill attribute again
	CurrentAttrs[ATTR_FILLGEOMETRY].Ignore = FALSE;

	// Return success or failure
	return Result;
}


/********************************************************************************************
>	BOOL MetaFileFilter::DoExport(Operation*, CCLexFile* pFile, PathName* pPath, Document*,
								  BOOL ShowOptions)
	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/3/95
	Inputs:		pOp - the operation that started the export off
				pFile - the file to put the exported data into
				pPath - the pathname of the file to be exported to
				pDoc - the document to export
				ShowOptions - Show the export options dialogue.
	Returns:	TRUE if it worked, FALSE if there was a problem
	Purpose:	To actually export into a metafile
	SeeAlso:	Filter::DoExport
********************************************************************************************/
BOOL MetaFileFilter::DoExport(Operation*, CCLexFile* pFile, PathName* pPath, Document* pDoc,
							  BOOL ShowOptions)
{
	// Used to open the file up before starting DoExport. But this meant a cancel on the export
	// options dialog had filled the file, if it was already present. So now up up here if
	// not open already. In the PreviewBitmap case the file will already be open.
	if (!pFile->isOpen())
	{
		if (pFile->IsKindOf(CC_RUNTIME_CLASS(CCDiskFile)))
		{
			BOOL ok = OpenExportFile((CCDiskFile*) pFile, pPath);
			if (!ok) return FALSE;
		}
		else
		{
			TRACEUSER( "JustinF", _T("Tried to open non-CCDiskFile in MetaFileFilter::DoExport\n"));
			return FALSE;
		}
	}

	// Get pointer to the spread to export.
	Spread* pSpread = GetFirstSpread(pDoc);
	ExportClipRect.MakeEmpty();

	// Set up device context and render region for this export.
	// This will show a progress hourglass for the objects being rendered
	if (!PrepareToExport(pFile, pSpread))
	{
		CleanUpAfterExport();
		return FALSE;
	}

	// put out some sensible stuff on the front
	MetafileDC.SetMapMode(pMetaView->GetMetafileMapMode());

	// Export the data to the file
	BOOL ok = ExportRender(ExportRegion);
	HMETAFILE hMetafile = NULL;

	// If there have been no errors so far, find out about the metafile
	if (ok)
	{
		// get a metafile handle from the metafile DC
		hMetafile = MetafileDC.Close();
		if (hMetafile == NULL)
		{
			ERROR1RAW( _R(IDE_MF_NOCLOSE) );
			ok = FALSE;
		}
	}

	// and clean up
	if (!ok)
	{
		if (hMetafile) DeleteMetaFile(hMetafile);
		CleanUpAfterExport();
		return FALSE;
	}

	// Put up a progress display/hourglass
	// (0 - 100, FALSE => no delay, show it NOW!, use the filters export message)
	String_64 ProgressString(_R(IDT_EXPORTMSG_METAFILE2));
	ProgressString = GetExportProgressString(pFile, _R(IDT_EXPORTMSG_METAFILE2));
	BeginSlowJob(100, FALSE, &ProgressString);

	// build standard placeable metafile header
	METAFILEHEADER Header;
	Header.key 		= METAFILEHEADERKEY;
	Header.hmf 		= 0;
	Header.inch 	= pMetaView->GetMetafileDPI();
	Header.reserved = 0L;
	Header.checksum = 0;

	// And fill in the details about the Bounding rect
	FIXED16 PixelSize;
	pMetaView->GetPixelSize(&PixelSize, &PixelSize);
	INT32 Size = PixelSize.MakeLong();
	Header.bbox.left   = (short) (ExportClipRect.lo.x / Size);
	Header.bbox.top    = (short) (ExportClipRect.lo.y / Size);
	Header.bbox.right  = (short) (ExportClipRect.hi.x / Size);
	Header.bbox.bottom = (short) (ExportClipRect.hi.y / Size);

	// calculate checksum as Word checks this
	LPWORD p = (LPWORD) &Header;
	INT32 i = 10;
	while (i--)
	{
		// Just OR together all the WORDs in the Header
		Header.checksum ^= *p++;
	}

	// actually write to the file
	ok = WriteToFile( hMetafile, &Header );
	DeleteMetaFile( hMetafile );

	// All done - deallocate dynamic objects, stop the progress display/hourglass
	// and return success. (Also closes file).
	CleanUpAfterExport();
	EndSlowJob();		

	// return, saying if it worked or not
	return ok;
}


/********************************************************************************************
>	BOOL MetaFileFilter::DoExport(Operation*, CCLexFile* pFile, Document* pDoc,
								  METAFILEPICT* pMetaInfo)
	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/7/95
	Inputs:		pFile - Dummy param. Need to pass in something, but we try not to use it
				pDoc - the document to render
				pMetaInfo - the info about the metafile we will create.
	Outputs:	pMetaInfo - the info about the metafile we will create.
	Returns:	TRUE if it worked, FALSE if not
	Purpose:	This is a special version of the DoExport function that is used to render in
				a memory metafile (not a real file, like normal).
********************************************************************************************/
BOOL MetaFileFilter::DoExport(Operation*, CCLexFile* pFile, Document* pDoc, METAFILEPICT* pMetaInfo)
{
	// Get pointer to the spread to export.
	Spread *pSpread = GetFirstSpread(pDoc);
	ExportClipRect.MakeEmpty();

	// Set up device context and render region for this export.
	// This will show a progress hourglass for the objects being rendered
	if (!PrepareToExport(NULL, pSpread))
	{
		CleanUpAfterExport();
		return FALSE;
	}

	// put out some sensible stuff on the front
	MetafileDC.SetMapMode(pMetaView->GetMetafileMapMode());
	pMetaInfo->mm = pMetaView->GetMetafileMapMode();

	// Export the data to the file
	BOOL ok = ExportRender(ExportRegion);
	HMETAFILE hMetafile = NULL;

	if (ok)
	{
		// get a metafile handle from the metafile DC
		hMetafile = MetafileDC.Close();
		if (hMetafile==NULL)
		{
			ERROR1RAW(_R(IDE_MF_NOCLOSE));
			ok = FALSE;
		}
	}

	// If at this point, something has gone wrong, tidy up
	if (!ok)
	{
		// Get rid of the metafile
		if (hMetafile)
			DeleteMetaFile(hMetafile);

		// Tidy up and fail
		CleanUpAfterExport();
		return FALSE;
	}

	// Put up a progress display/hourglass
	String_64 ProgressString(_R(IDT_EXPORTMSG_METAFILE2));
	ProgressString = GetExportProgressString(pFile, _R(IDT_EXPORTMSG_METAFILE2));
	BeginSlowJob(100, FALSE, &ProgressString);

	// Fill in the info about the metafile that we know
	// make it bigger as the coords we use are smaller than most.
	FIXED16 PixelSize;
	pMetaView->GetPixelSize(&PixelSize, &PixelSize);
	INT32 Size = PixelSize.MakeLong();
	pMetaInfo->xExt = ExportClipRect.Width() / Size;
	pMetaInfo->yExt = ExportClipRect.Height() / Size;
	pMetaInfo->hMF = hMetafile;

	// All done - deallocate dynamic objects, stop the progress display/hourglass
	// and return success. (Also closes file).
	CleanUpAfterExport();
	EndSlowJob();		

	return ok;
}


/********************************************************************************************
>	BOOL MetaFileFilter::PrepareToExport(CCLexFile* pFile, Spread* pSpread)
	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/3/95
	Inputs:		See base class version.
	Returns:	See base class version.
	Purpose:	To get ready to export a metafile. Most importantly creates an
				ExportRegion, but has to create other support items first.
	SeeAlso:	Filter::DoExport
********************************************************************************************/
BOOL MetaFileFilter::PrepareToExport(CCLexFile* pFile, Spread* pSpread)
{
	// the member variable OutputFile is deliberately a vanilla CCLexFile because
	// metafile output must be made to work to all file types, especially memory files
	OutputFile = pFile;

	// work out the clip rect by exporting the whole drawing
	// Start out with an empty clip rect
	DocRect SpreadRect;
	SpreadRect.MakeEmpty();

	// We need to take into account the fact that some layers may not be visible
	Node* pNode = pSpread->FindFirstChild();
	while (pNode!=NULL)
	{
		// See if this is a layer
		if (IS_A(pNode, Layer))
		{
			// It sure is... lets see if it is visible
			Layer* pLayer = (Layer*) pNode;
			if (pLayer->IsVisible())
			{
				// This one is visible, so union it in
				DocRect LayerRect = pLayer->GetBoundingRect();
				SpreadRect = SpreadRect.Union(LayerRect);
			}
		}

		// Get the next sibling
		pNode = pNode->FindNext();
	}

	// Set the clip rect
	DocRect ClipRect = SpreadRect;
	ExportClipRect = SpreadRect;

	// we need a View to Attach to, so that:
	//		default Quality setting
	//		CalcPixelWidth etc get called
	//View *pView = View::GetCurrent();
	pMetaView = new MetafileView();
	if (pMetaView==NULL)
		return FALSE;

	// Init the view
	if (!pMetaView->Init())
	{
		delete pMetaView;
		pMetaView = NULL;
		return FALSE;
	}

	// now lets get a render region suitably contructed
	FIXED16 XScale, YScale;
	pMetaView->GetScaleFactor(&XScale, &YScale);
	Matrix Identity(XScale, YScale);

	// Don't use view scale; set to 1
	FIXED16 Scale(1);

	// Create the region
	ExportRegion = OSRenderRegion::Create(ClipRect, Identity, Scale, RENDERTYPE_METAFILE16, pMetaView, TRUE );
	if (ExportRegion == NULL)
		// Error already set by new, so just return
		return FALSE;

	// lets create a metafile DC now
	if (!MetafileDC.Create( NULL ))									// create it in memory
		ERROR1( FALSE, _R(IDE_MF_NOCREATE) );

	// need a refernce DC - don't just use the view window because it will get messed up
	// We use a fresh new screen DC
	if (!ReferenceDC.CreateIC( "DISPLAY", NULL, NULL, NULL ))
		ERROR1( FALSE, _R(IDE_MF_NOCREATE) );

	// if we don't do this then a load of Getxxx calls will fail
	MetafileDC.SetAttribDC( ReferenceDC.m_hDC );

	// this (i) stops paper etc from being rendered, and (ii) stops background rendering screwing us up
	MetafileDC.m_bPrinting = TRUE;

	// Attach to the right device
	return ExportRegion->AttachDevice(pMetaView, &MetafileDC, pSpread);
}

/********************************************************************************************
>	void MetaFileFilter::CleanUpAfterExport()
	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/3/95
	Inputs:		See base class version.
	Returns:	See base class version.
	Purpose:	Tidys up outstanding items created in PrepareToExport
********************************************************************************************/
void MetaFileFilter::CleanUpAfterExport()
{
	// Clean up the view
	if (pMetaView!=NULL)
	{
		delete pMetaView;
		pMetaView = NULL;
	}

	// Clean up the metafile DC
	if (MetafileDC.m_hDC)
	{
		MetafileDC.DeleteDC();
	}

	// and the attribute DC
	if (ReferenceDC.m_hDC)
	{
		ReferenceDC.DeleteDC();
	}

	// and make sure the file is closed
	if (OutputFile)
	{
		OutputFile->close();
		// *dont* delete OutputFile because it turns out to be a static object...
		OutputFile = NULL;
	}
}

/********************************************************************************************
>	BOOL MetaFileFilter::WriteToFile( HMETAFILE hMetafile, METAFILEHEADER *PlaceableHeader )
	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/3/95
	Inputs:		A metafile handle to write out the data from and a placeable header to
				write out as well.
	Returns:	TRUE worked, FALSE failed (error set)
	Purpose:	Get the metafile onto the disk. Inherited classes could override this for
				further nefarious purposes e.g. output via Aldus filters
********************************************************************************************/
BOOL MetaFileFilter::WriteToFile( HMETAFILE hMetafile, METAFILEHEADER *PlaceableHeader )
{
	// write header first
	OutputFile->write( PlaceableHeader, sizeof(METAFILEHEADER) );

	// now the actual data
	UINT32 size = GetMetaFileBitsEx( hMetafile, 0, NULL );			// does NOT invalidate hMetafile
	ERROR1IF( size==0, FALSE, _R(IDE_MF_NOMEMLOCK) );

	LPVOID block = CCMalloc( size );
	ERROR1IF( block==NULL, FALSE, _R(IDS_OUT_OF_MEMORY) );

	size = GetMetaFileBitsEx( hMetafile, size, block );
	if (size)
	{
		OutputFile->write( block, size );
		CCFree( block );
		return TRUE;
	}
	else
	{
		CCFree( block );
		ERROR1( FALSE, _R(IDE_MF_NOMEMLOCK) );
	}
}

//////////////////////////////////////////////////////////
// Aldus format filter class was here.  As none of it	//
// was being used I have deleted all of it.  See		//
// SourceSafe for Camelot2000 for the code.  There are	//
// also some docs in the docs folder (AldusFilter.doc	//
// or similar).  I think the idea of the filter was that//
// we would be able to open lots of vector file formats	//
// by emulating the Aldus filter DLL format.			//
//							-- Jonthan 12/10/2000		//
//////////////////////////////////////////////////////////
