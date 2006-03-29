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

// An input filter for importing RISC OS Draw format files.


#include "camtypes.h"
#include "drawfltr.h"

#include <ctype.h>

#include <afxdlgs.h>

#include "pathname.h"
#include "document.h"
#include "docview.h"
#include "errors.h"
#include "ensure.h"
#include "paper.h"
#include "nodepath.h"
#include "paths.h"
//#include "tim.h"
#include "lineattr.h"
#include "progress.h"			// for the progress indicator
//#include "tim.h"
#include "fixmem.h"
#include "oilfltrs.h"
#include "group.h"
#include "spread.h"
#include "page.h"
#include "trans2d.h"
#include "attrmgr.h"


DECLARE_SOURCE("$Revision: 662 $");


CC_IMPLEMENT_DYNAMIC(AcornDrawFilter, VectorFilter)



#define DRAWOBJECT_FONTTABLE		   (0) 
#define DRAWOBJECT_TEXT				   (1) 
#define DRAWOBJECT_PATH				   (2) 
#define DRAWOBJECT_SPRITE 			   (5)
#define DRAWOBJECT_GROUP			   (6)
#define DRAWOBJECT_TAGGED			   (7)
#define DRAWOBJECT_TEXTAREA			   (9)
#define DRAWOBJECT_TEXTCOLUMN		  (10)
#define DRAWOBJECT_OPTIONS			  (11)
#define DRAWOBJECT_TRANSFORMEDTEXT    (12)
#define DRAWOBJECT_TRANSFORMEDSPRITE  (13)

#define DRAWOBJECT_ERROR			  (-1)  // => Error reading object header




#define DRAWTAG_ENDOFPATH			   (0)
#define DRAWTAG_MOVETO				   (2)
#define DRAWTAG_CLOSESUBPATH		   (5)
#define DRAWTAG_LINETO				   (8)
#define DRAWTAG_CURVETO				   (6)



#define SIZEOF_MOVETO				  (12)
#define SIZEOF_CLOSESUBPATH			   (4)
#define SIZEOF_LINETO				  (12)
#define SIZEOF_CURVETO				  (28)





#define DRAW_TRANSPARENT	  (0xFFFFFFFF)



/********************************************************************************************

>	class DrawColour

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/12/93
	Purpose:	Mirrors the representation of a colour as used in Acorn Draw files.
	SeeAlso:	DrawPathStyle; DashPatternHeader

********************************************************************************************/

class DrawColour
{
public:
	union
	{
		struct
		{
			BYTE Reserved : 8;
			BYTE Red      : 8;
			BYTE Green    : 8;
			BYTE Blue     : 8;
		} Component;
		UINT32 ColourValue;
	} Data;
};


/********************************************************************************************

	typedef DrawPathStyle

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/12/93
	Purpose:	Holds the style information for a path as used in Acorn Draw files.
	SeeAlso:	DrawColour; DashPatternHeader

********************************************************************************************/

typedef union
{
	UINT32 Value;
	struct
	{
		UINT32 Join        : 2;
		UINT32 EndCap      : 2;
		UINT32 StartCap    : 2;
		UINT32 WindingRule : 1;
		UINT32 DashPresent : 1;
		UINT32 Reserved    : 8;
		UINT32 CapWidth    : 8;
		UINT32 CapLength   : 8;
	} Data;
} DrawPathStyle;


/********************************************************************************************

>	class DashPatternHeader

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/12/93
	Purpose:	The header for the dash pattern structure contained in Acorn Draw files.
				It is usually followed by a series of dash and gap lengths in the file.
	SeeAlso:	DrawColourl; DrawPathStyle

********************************************************************************************/

class DashPatternHeader
{
public:
	UINT32 Offset;
	UINT32 NumElements;
};


/********************************************************************************************

>	class DrawPathElement

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/12/93
	Purpose:	Used to load in a path element from a Draw file.  Not all of the 'Coords'
				array is always used - it depends on the path element.
	SeeAlso:	DrawPathHeader

********************************************************************************************/

class DrawPathElement
{
public:
	UINT32 Tag;
	DocCoord Coords[3]; 
};


/********************************************************************************************

>	class DrawPathHeader

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/12/93
	Purpose:	Used to load in the header information for a path object in a Draw file.
				It holds details on the fill and line colours, the line width, and the
				style of the path.
	SeeAlso:	DrawPathElement; DrawColour; DrawPathStyle

********************************************************************************************/

class DrawPathHeader
{
public:
	DrawColour		FillColour;
	DrawColour		LineColour;
	UINT32			LineWidth;
	DrawPathStyle	Style;
};


/********************************************************************************************

>	AcornDrawFilter::AcornDrawFilter()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/93
	Purpose:	Filter constructor - this initialises the default attributes and the
				input buffer.

********************************************************************************************/

AcornDrawFilter::AcornDrawFilter()
{
	// Set up filter descriptions.
	FilterName.Load(_R(IDT_DRAW_FILTERNAME));
	FilterInfo.Load(_R(IDT_DRAW_FILTERINFO));
	FilterID = FILTERID_ACORN_DRAW;

#ifndef STANDALONE
	Flags.CanImport = TRUE;
	Flags.CanExport = FALSE;
#else
	Flags.CanImport = FALSE;
	Flags.CanExport = FALSE;
#endif

#if 0
	// No attributes yet
	DefLineWidthAttr = NULL;
	DefLineColAttr = NULL;
	DefFillColAttr = NULL;
#endif

	// No buffer yet
	DataBuf = NULL;
	DataBufSize = 0;
};


/********************************************************************************************

>	AcornDrawFilter::~AcornDrawFilter()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/93
	Purpose:	Destructor for the filter - this deallocates the input buffer.

********************************************************************************************/

AcornDrawFilter::~AcornDrawFilter()
{
	// Clean up our input buffer
	CCFree(DataBuf);
}


/********************************************************************************************

>	BOOL AcornDrawFilter::Init()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/93
	Returns:	Always TRUE at the moment.
	Purpose:	Initialise the filter (attaches a DrawOILFilter object).
	SeeAlso:	DrawOILFilter

********************************************************************************************/

BOOL AcornDrawFilter::Init()
{
	// Get the OILFilter object
	pOILFilter = new DrawOILFilter(this);
	if (pOILFilter == NULL)
		return FALSE;

	// All ok
	return TRUE;
}

/********************************************************************************************

>	virtual BOOL AcornDrawFilter::IsDefaultDocRequired(const TCHAR* pcszPathName)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/10/95
	Inputs:		pcszPathName	pointer to the pathname to check
	Returns:	TRUE if the filter requires a default document, FALSE if not.
	Purpose:	Works out if opening a file of this type requires a default document to be
				loaded. If the file format supplies the document then return FALSE otherwise
				return TRUE. An example would be opening a bitmap file. This has no document
				defined in the file format and so we need to laod the default document before
				importing the bitmap into this file.
				In this version we return TRUE as either this format has no page size or
				nobody has implemented it yet.
	SeeAlso:	Filter; Filter::IsDefaultDocRequired; CCamDoc::OnOpenDocument;
	SeeAlso:	FilterFamily::DoImport; Filter::DoImport;

********************************************************************************************/

BOOL AcornDrawFilter::IsDefaultDocRequired(const TCHAR* pcszPathName)
{
	// No need to check the pathname, just return TRUE as all currently known bitmap filters
	// will require the default document.
	return TRUE;
}	


/********************************************************************************************

>	INT32 AcornDrawFilter::HowCompatible(PathName& Filename, ADDR HeaderStart, 
								   	   UINT32 HeaderSize, UINT32 FileSize)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/03/94
	Inputs:		Filename - name of the file.
				HeaderStart - Address of the first few bytes of the file.
				HeaderSize - the number of bytes in the header pointed to by FileStart.
				FileSize - the size of the whole file, in bytes.
	Returns:	Compatibility: 0 => Not a Draw file; 10 => It is a Draw file
	Purpose:	Analyse a file to see if it is an Acorn Draw file - at present this
				is done by checking to see if the first four bytes are "Draw".
	SeeAlso:	AcornDrawFilter

********************************************************************************************/

INT32 AcornDrawFilter::HowCompatible(PathName& Filename, ADDR HeaderStart, 
								   UINT32 HeaderSize, UINT32 FileSize)

{
PORTNOTE("byteorder", "TODO: Check byte ordering")
	if (_tcsncmp((char *) HeaderStart, "Draw", 4) == 0)
	{
		// It's a Draw file.
		return 10;
	}
	
	// It's not a Draw file.
	return 0;
}


/********************************************************************************************

>	BOOL AcornDrawFilter::PrepareToImport()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/02/94
	Returns:	TRUE if it succeeds, FALSE if not.
	Purpose:	Sets up the Draw filter so it can read in a Draw file.

********************************************************************************************/

BOOL AcornDrawFilter::PrepareToImport()
{
	// Get a default set of attributes for ink objects.
	if (!SetUpCurrentAttrs())
		return FALSE;

	// Shouldn't have a data buffer at this point
	ENSURE(DataBuf == NULL, "DataBuf is non-NULL in PrepareToImport()");

	// No objects read yet
	HeaderSize = 0;
	DataSize = 0;

	// Get a Diskfile object
	DrawFile = NULL;

	// Ok so far
	return TRUE;
}


/********************************************************************************************

>	void AcornDrawFilter::CleanUpAfterImport()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/02/94
	Purpose:	Delete dynamic objects used in the import process.
	SeeAlso:	AcornDrawFilter::PrepareToImport; AcornDrawFilter::DoImport

********************************************************************************************/

void AcornDrawFilter::CleanUpAfterImport()
{
	DeleteCurrentAttrs();
	DrawFile = NULL;
	CCFree(DataBuf);
	DataBuf = NULL;
	DataBufSize = 0;
}


/********************************************************************************************

>	BOOL AcornDrawFilter::DoImport(SelOperation *Op, CCLexFile* pFile, 
							   Document *DestDoc, BOOL AutoChosen, ImportPosition* Pos,
							   KernelBitmap** ppImportedBitmap, DocCoord* pPosTranslate, String_256* URL)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/93
	Inputs:		Op - pointer to the operation that this read process is associated with.
				pFile - The file that we are trying to import
				DestDoc - pointer to the document to insert the Draw file data into.
				AutoChosen - 
				Pos -
				ppImportedBitmap - this is used mainly in the bitfltr.cpp for the HTML
				import filter. HTMLFilter::DoImport() needs a pointer to a kernel bitmap
				to set the background bitmap up into Camelot.
				pPosTranslate - This is used too by the HTMLFilter in order to do a formatting.
				URL - original URL of the imported file
	Returns:	TRUE if the read was successful, FALSE if not.
	Purpose:	Import the named Acorn draw file into the specified document.
				As yet, only path and group objects are handled.  Tagged objects are
				handled correctly, so, e.g. TableMate files can be loaded by Camelot
				(although the results are not very interesting).
				Currently, the data is always positioned on the first page of the first
				spread in the first chapter.
				If the read is successful, the End() function of the 'Op' operation is
				called, otherwise it is not called.
	Errors:		Unable to open file, corrupted document tree found.

********************************************************************************************/

BOOL AcornDrawFilter::DoImport(SelOperation *Op, CCLexFile* pFile, 
							   Document *DestDoc, BOOL AutoChosen, ImportPosition* Pos,
							   KernelBitmap** ppImportedBitmap, DocCoord* pPosTranslate, String_256* URL)
{
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
	
	// pNode is where new objects will go - they all go into a group so the user
	// can easily drag the imported Draw file around.  They can then ungroup it afterwards
	// if they want.
	NodeGroup *pDrawGroup = new NodeGroup;
	ERRORIF(pDrawGroup == NULL, _R(IDT_DRAW_NOMEMORY), FALSE);
	pNode = pDrawGroup;
		
	Spread *pSpread;

	if (Pos == NULL)
	{
		// For now, position Draw objects on 1st page of spread 1
		pSpread = GetFirstSpread(DestDoc);
		pPage = (Page *) pSpread->FindFirstPageInSpread();
		ENSURE(pPage->IsKindOf(CC_RUNTIME_CLASS(Page)),
			   "AcornDrawFilter::Read(): Could not find first Page");
		
		// Use bottom left of page as origin
		DocRect PageRect = pPage->GetPageRect();
		Origin = PageRect.lo;
	}
	else
	{
		pSpread = Pos->pSpread;
		Origin = Pos->Position;
	}
	
	// No complex path initially
	ComplexPath = FALSE;

	// Make a note of the disk file
	DrawFile = pFile;

	// Disable exceptions and error reporting for this file - we'll do it ourselves
	DrawFile->SetReportErrors(FALSE);
	DrawFile->SetThrowExceptions(FALSE);

	// Find out the size of the file, in bytes.
	INT32 filesize = DrawFile->Size();
		
	// Initialise
	eof = FALSE;
		
	// Set the progress indicator, this next bit might take a while.
	String_64 ImportMessage(_R(IDT_IMPORTMSG_ACORNDRAW));
	ImportMessage = GetImportProgressString(pFile, _R(IDT_IMPORTMSG_ACORNDRAW));
	BeginSlowJob(filesize, TRUE, &ImportMessage);

	BytesRead = 0;
	LastProgressUpdate = 0;

	// Find out details on the file
	ReadFileHeader();

	// Check that this is an Acorn Draw file...the first four characters in the file
	// should be "Draw".  If we've been chosen automatically, then don't bother as we've
	// already checked this.
	if (DrawFile->eof() || (!AutoChosen && (_tcsncmp(FileHeader.Ident, "Draw", 4) != 0)))
	{
		// Could not find the ident header - close the file, and return error.
		EndSlowJob();
		CleanUpAfterImport();
		ERROR(_R(IDT_DRAW_NOTADRAWFILE), FALSE);
	}
	
	// Remember file errors
	BOOL Error = FALSE;

	// Process each object in the file
	do 
	{
		// Read and process each object
		if (ReadObjectHeader())
		{
			// Header read ok so there is another object to process
			if (!ProcessObject())
				// Unable to process object - set error flag
				Error = TRUE;
		}
	}
	while ((BytesRead < filesize) && !Error);
	
	// All work has been completed . . .
	EndSlowJob();
	
	// All done
	BOOL Failed = Error && (BytesRead < filesize);

	// Free up dynamic objects used for import
	CleanUpAfterImport();

	// If the load failed for any reason, delete the subtree we have created; otherwise
	// graft it into the tree.
	if (Failed)
	{
		// Failed - delete the sub-tree we just created, free the buffer, and return error.
		pDrawGroup->CascadeDelete();
		delete pDrawGroup;
		return FALSE;
	}
	else
	{
		// It worked - add the node into the tree
		
		// Remember current bounding rectangle of the group
		DocRect BoundsRect = pDrawGroup->GetBoundingRect(TRUE);
		
		if (Pos != NULL)
		{
			// It should be centred on the given position...
			DocCoord Centre;
			Centre.x = (BoundsRect.lo.x + BoundsRect.hi.x) / 2;
			Centre.y = (BoundsRect.lo.y + BoundsRect.hi.y) / 2;

			Trans2DMatrix Xlate(Pos->Position.x - Centre.x, Pos->Position.y - Centre.y);
			pDrawGroup->Transform(Xlate);
		}
				
		// Insert the node and invalidate its region
		if (!Op->DoInsertNewNode(pDrawGroup, pSpread, TRUE))
		{
			// It didn't work - delete the sub-tree we just created
			pDrawGroup->CascadeDelete();
			delete pDrawGroup;
			pDrawGroup = NULL;
		}
	
	}

	// All ok
	return TRUE;
}

/********************************************************************************************

>	BOOL AcornDrawFilter::ReadFileHeader()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/93
	Returns:	TRUE if the read is successful, FALSE if an error was encountered.
	Purpose:	Read the header block of an Acorn Draw file.
	SeeAlso:	AcornDrawFilter::ReadObjectHeader
	Scope:		Private

********************************************************************************************/

BOOL AcornDrawFilter::ReadFileHeader()
{
	DrawFile->read(&FileHeader, sizeof(FileHeader));
	BytesRead += sizeof(FileHeader);
	return TRUE;
}

/********************************************************************************************

>	BOOL AcornDrawFilter::ReadObjectHeader()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/93
	Returns:	TRUE if the header was read ok, FALSE if not.
	Purpose:	This function reads in the header of a Draw object from the input file,
				and stores the result in the ObjectHeader member variable.
				Using the data in the header it then sets up the HeaderSize and DataSize
				member variables correctly.  It is not ok just to use sizeof(ObjectHeader)
				as this is not always accurate because some objects in Draw files has
				stupid headers which are smaller than the standard size, so we have to
				compensate for this.  In such cases we also have to rewind the file so we
				are in the correct position to read in the data following the header.
	Errors:		If the EOF is found.
	SeeAlso:	AcornDrawFilter::ReadObjectData; AcornDrawFilter::ReadFileHeader
	Scope:		Private

********************************************************************************************/

BOOL AcornDrawFilter::ReadObjectHeader()
{
	// Set default header size
	FilePos BeforePos = DrawFile->tell();

	HeaderSize = sizeof(ObjectHeader);

	DrawFile->read(&ObjectHeader, sizeof(ObjectHeader));

	BytesRead += sizeof(ObjectHeader);

	FilePos AfterPos = DrawFile->tell();

	if ((UINT32) (AfterPos-BeforePos) != sizeof(ObjectHeader))
	{
		ERROR3_PF(("Header bytes read - actual: %d, planned: %d", (AfterPos-BeforePos), sizeof(ObjectHeader)));
		// If EOF found, then an error has occured
		ERROR1(FALSE, _R(IDT_DRAW_BADSYNTAX));
	}

	// Check for silly objects that don't have a bounding box, and move the input pointer
	// back to cope with this.
	switch (ObjectHeader.Type)
	{
		case DRAWOBJECT_FONTTABLE:
			// Move back over the bounding box fields (which don't really exist)
			FilePos Pos = DrawFile->tellIn();

			// Bounding box fields take up 16 bytes in Acorn Draw files.
			DrawFile->seekIn(Pos - 16);
			BytesRead -= 16;

			// Adjust header size
			HeaderSize -= 16;
			break;
	}

	DataSize = ObjectHeader.Size - HeaderSize;

	// If eof not found, then no error
	return TRUE;
}

/********************************************************************************************

>	BOOL AcornDrawFilter::ProcessObject()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/11/93
	Returns:	TRUE if the object was processed with no errors, FALSE if not.
	Purpose:	Routes the object data to the appropriate function to be processed,
				according to the tag in the object's header data.
				If the object is not understood, then the data is simply discarded.
	Errors:		-
	SeeAlso:	AcornDrawFilter::ProcessPath; AcornDrawFilter::ProcessGroup;
				AcornDrawFilter::ProcessTaggedObject; AcornDrawFilter::SkipObject
	Scope:		Private

********************************************************************************************/

BOOL AcornDrawFilter::ProcessObject()
{
	if (BytesRead > (LastProgressUpdate + 2048))
	{
		LastProgressUpdate = BytesRead;
		if (!ContinueSlowJob(BytesRead))
			return FALSE;
	}

	switch (ObjectHeader.Type)
	{
		case DRAWOBJECT_PATH:		return ProcessPath();
		case DRAWOBJECT_GROUP:		return ProcessGroup();
		case DRAWOBJECT_TAGGED:		return ProcessTaggedObject();
		default:					return SkipObject();
	}
}

/********************************************************************************************

>	BOOL AcornDrawFilter::ProcessPath()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/93
	Returns:	TRUE if the path object is read and converted ok, FALSE if not.
	Purpose:	Read in the data for a Draw path object, and convert it to an equivalent
				Camelot	NodePath object, and add it to the node pointed to by the pNode
				member variable.
	Errors:		File reading error, out of memory.
	SeeAlso:	AcornDrawFilter::ProcessObject
	Scope:		Private

********************************************************************************************/

BOOL AcornDrawFilter::ProcessPath()
{
	// Attempt to read the data for this object
	if (!ReadObjectData())
		return FALSE;

	// Construct the path and add it to our group node...

	// Create a new NodePath pbject
	NodePath *pPath = new NodePath(pNode, LASTCHILD);
	if ((pPath == NULL) || (!pPath->SetUpPath()))
	{
		// Couldn't initialise path - delete path (if created) and return error.
		delete pPath;
		ERROR(_R(IDT_DRAW_NOMEMORY), FALSE);
	}

	// Position tag at start of path.
	pPath->InkPath.FindStartOfPath();

	// Examine path header info
	DrawPathHeader *pHeader = (DrawPathHeader *) DataBuf;

	// Find first path element
	DrawPathElement *pElement = (DrawPathElement *) (pHeader + 1);

	// Skip dash pattern if present
	if (pHeader->Style.Data.DashPresent)
	{
		DashPatternHeader *pDashPattern = (DashPatternHeader *) pElement;
		pElement = (DrawPathElement *) 
		 ( ((ADDR) pElement) + sizeof(DashPatternHeader) + (4 * pDashPattern->NumElements) );
	}

	// Process path elements until no more data is left.
	UINT32 BytesUsed = 0;

	INT32 Tag = (pElement->Tag) & 0xFF;

	while (Tag != DRAWTAG_ENDOFPATH)
	{
		switch (Tag)
		{
			case DRAWTAG_MOVETO:
				BytesUsed = SIZEOF_MOVETO;

				TransformCoord(pElement->Coords[0]);

				// Insert a moveto into the path
				if (!pPath->InkPath.InsertMoveTo(pElement->Coords[0]))
					// Not enough dynamic heap to insert the moveto command
					goto NoMemory;
				break;

			case DRAWTAG_LINETO:
				BytesUsed = SIZEOF_LINETO;

				TransformCoord(pElement->Coords[0]);

				// Insert a lineto into the path
				if (!pPath->InkPath.InsertLineTo(pElement->Coords[0]))
					// Not enough dynamic heap to insert the lineto command
					goto NoMemory;
				break;

			case DRAWTAG_CURVETO:
				BytesUsed = SIZEOF_CURVETO;

				TransformCoord(pElement->Coords[0]);
				TransformCoord(pElement->Coords[1]);
				TransformCoord(pElement->Coords[2]);

				// Insert a curveto into the path
				if (!pPath->InkPath.InsertCurveTo(pElement->Coords[0], 
												  pElement->Coords[1], pElement->Coords[2]))
					// Not enough dynamic heap to insert the curveto command
					goto NoMemory;
				break;

			case DRAWTAG_CLOSESUBPATH:
				BytesUsed = SIZEOF_CLOSESUBPATH;

				// Terminate sub-path
				if (!pPath->InkPath.CloseSubPath())
					// Not enough dynamic heap to insert the final element of the path
					goto NoMemory;
							
				break;

			default:
				// Don't understand this element!
				ERROR1(FALSE, _R(IDT_DRAW_BADSYNTAX));
		}

		// Move pointer onto next path element
		pElement = (DrawPathElement *) (((ADDR) pElement) + BytesUsed);

		Tag = (pElement->Tag) & 0xFF;
	}

	// check that the path is OK
	if(!pPath->InkPath.EnsureValid())
	{
		// no, it's completely knackered
		pPath->UnlinkNodeFromTree();
		delete pPath;
		// don't error about it.
		return TRUE;
	}

	// Use the path header to set the attributes on this path
	AddAttributes(pPath, pHeader);

	// Terminate path properly.
	pPath->InvalidateBoundingRect();
				
	// All ok
	return TRUE;

NoMemory:
	// Error - Out of memory while reading Draw file
	
	// Destroy any half-created paths
	if (pPath != NULL)
	{
		pPath->CascadeDelete();
		delete pPath;
	}	
	// Set the error for the caller to report
	ERROR(_R(IDT_DRAW_NOMEMORY), FALSE);
}

/********************************************************************************************

>	BOOL AcornDrawFilter::ProcessGroup()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/93
	Returns:	-
	Purpose:	Read in a Draw group object, which can be an arbitrary group of Draw objects,
				possibly including other groups.  A NodeGroup object is constructed from
				the group's data and added to the node pointed to by the member variable
				pNode.
	Errors:		Out of memory, error processing objects in the group, unexpected EOF.
	SeeAlso:	AcornDrawFilter::ProcessObject
	Scope:		Private

********************************************************************************************/

#define DRAW_GROUPNAMESIZE (12)

BOOL AcornDrawFilter::ProcessGroup()
{
	// Remembers file read errors
	BOOL Error = FALSE;

	// Skip past the group name
	DrawFile->seekIn(DRAW_GROUPNAMESIZE, ios::cur);
	BytesRead += DRAW_GROUPNAMESIZE;

	// Create a group node, and make pNode point to it, so that all objects in the group
	// are attached to it.
	NodeGroup *pGroup = new NodeGroup(pNode, LASTCHILD);
	if (pGroup == NULL)
	{
		// Not enough heap to create group
		ERROR(_R(IDT_DRAW_NOMEMORY), FALSE);
	}

	Node *pOldNode = pNode;
	pNode = pGroup;

	// Work out where the end of the group is
	FilePos EndPos = DrawFile->tellIn();
	DataSize -= DRAW_GROUPNAMESIZE;
	EndPos += DataSize;

	// Read (and process) each object in the group, until the end of group or end of file
	// is found.
	do 
	{
		// Read in and process the next object
		if (!(ReadObjectHeader() && ProcessObject()))
			Error = TRUE;

	} while (!DrawFile->eof() && (DrawFile->tellIn() < EndPos));

		
	// Restore the node pointer to the parent of this group
	pNode = pOldNode;

	// If EOF found, then the group was truncated, so return error.
	// Also return error if object reading failed in any way.
	if (Error)
		// Error has already been set
		return FALSE;

	if (DrawFile->eof())
		ERROR1(FALSE, _R(IDT_DRAW_BADSYNTAX));
		
	// Must have worked
	return TRUE;
}

#define DRAW_TAGSIZE (4)

/********************************************************************************************

>	BOOL AcornDrawFilter::ProcessTaggedObject()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/11/93
	Returns:	TRUE if the tagged object is read and process ok, FALSE if not.
	Purpose:	Read in and process a tagged object from a Draw file.
				A tagged object is a normal object with some arbitrary data following it.
				This function can handle the tagged object being a group which itself
				contains tagged objects, and so on.
	Errors:		Out of memory, error processing the tagged object.
	SeeAlso:	AcornDrawFilter::ProcessObject; AcornDrawFilter::ReadObjectHeader
	Scope:		Private

********************************************************************************************/

BOOL AcornDrawFilter::ProcessTaggedObject()
{
	// Remembers file read errors
	BOOL Error = FALSE;

	// Skip past the object tag
	DrawFile->seekIn(DRAW_TAGSIZE, ios::cur);
	BytesRead += DRAW_TAGSIZE;

	// Work out where the end of the tagged object is
	FilePos EndPos = DrawFile->tellIn();
	DataSize -= DRAW_TAGSIZE;
	EndPos += DataSize;

	// Read (and process) the object
	if (!(ReadObjectHeader() && ProcessObject()))
		// Something went wrong so return error
		return FALSE;

	// Skip the word-aligned data following the object
	DrawFile->seekIn(EndPos);
	BytesRead = EndPos;

	// All ok
	return TRUE;
}

/********************************************************************************************

>	BOOL AcornDrawFilter::SkipObject()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/93
	Returns:	TRUE always at present.
	Purpose:	Skips past the data for this object, usually because the object is not
				supported by this filter and/or Camelot.
	SeeAlso:	AcornDrawFilter::ProcessObject
	Scope:		Private

********************************************************************************************/

BOOL AcornDrawFilter::SkipObject()
{
	if (IsUserName("Tim"))
	{
		TRACE( _T("Skipping unsupported object (type %d, size %d)\n"),
				ObjectHeader.Type, 
				ObjectHeader.Size);
	}

	// Skip the number of bytes specified in the current object's header
	FilePos Pos = DrawFile->tellIn();
	Pos += DataSize;
	DrawFile->seekIn(Pos);
	BytesRead = Pos;

	// No error checking yet
	return TRUE;
}

/********************************************************************************************

>	BOOL AcornDrawFilter::ReadObjectData()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/93
	Returns:	TRUE if the data was read ok, FALSE if not.
	Purpose:	Read in the data for a Draw object.  The data is stored in the buffer
				pointed to by the member variable DataBuf.  If the buffer is not big enough,
				it is automagically expanded.
	Errors:		Out of memory.
	SeeAlso:	AcornDrawFilter::ReadObjectHeader
	Scope:		Private

********************************************************************************************/

BOOL AcornDrawFilter::ReadObjectData()
{
	// Obtain a buffer to read the data into, if we haven't got one already
	if (DataSize > DataBufSize)
	{
		// Buffer is not big enough - try to expand it.
		// Don't overwrite DataBuf pointer unless we know the realloc succeeds, otherwise
		// we won't be able to free the old buffer if it fails.
		ADDR NewBuf = (ADDR) CCRealloc(DataBuf, DataSize);

		// Failed to expand buffer - return error.
		ERRORIF(NewBuf == NULL, _R(IDT_DRAW_NOMEMORY), FALSE);

		// Buffer was expanded ok - update the buffer pointer/size variables.
		DataBufSize = DataSize;
		DataBuf = NewBuf;
	}

	// To get this far, we must have a buffer big enough to hold the object, so read it in...
	// NB we check that we read the correct number of bytes.
	FilePos BeforePos = DrawFile->tell();
	DrawFile->read(DataBuf, DataSize);
	BytesRead += DataSize;
	FilePos AfterPos = DrawFile->tell();

	if ((UINT32) (AfterPos-BeforePos) != DataSize)
	{
		ERROR3_PF(("Bytes read - actual: %d, planned: %d", (AfterPos-BeforePos), DataSize));
		// If EOF found, then an error has occured
		ERROR1(FALSE, _R(IDT_DRAW_BADSYNTAX));
	}

	// Ok if we got here.
	return TRUE;
}


/********************************************************************************************

>	BOOL AcornDrawFilter::AddAttributes(NodePath *pPath, DrawPathHeader *pHeader)

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

BOOL AcornDrawFilter::AddAttributes(NodePath *pPath, DrawPathHeader *pHeader)
{
	// Add attributes to the path, if they are different from the default...

	// First set the current attributes.

	// Fill colour
	if (pHeader->FillColour.Data.ColourValue == DRAW_TRANSPARENT)
	{
		pPath->InkPath.IsFilled = FALSE;
	}
	else
	{
		pPath->InkPath.IsFilled = TRUE;

		DocColour FillColour(pHeader->FillColour.Data.Component.Red, 
							 pHeader->FillColour.Data.Component.Green,
							 pHeader->FillColour.Data.Component.Blue);
		
		if (!SetFillColour(FillColour))
			return FALSE;
	}

	// Line colour
	if (pHeader->LineColour.Data.ColourValue == DRAW_TRANSPARENT)
	{
		if (!SetLineColour(DocColour(COLOUR_TRANS)))
			return FALSE;
	}
	else
	{
		DocColour LineColour(pHeader->LineColour.Data.Component.Red, 
							 pHeader->LineColour.Data.Component.Green,
							 pHeader->LineColour.Data.Component.Blue);

		if (!SetLineColour(LineColour))
			return FALSE;
	}
				
	// Line width

	// Convert from Draw units (1/640 of a point) to Camelot units (millipoints_
	// Hence it involves multiplying by 1000/640, which equates to 25/16.
	pHeader->LineWidth *= 25;
	pHeader->LineWidth /= 16;

	if (!SetLineWidth(pHeader->LineWidth))
		return FALSE;

	// Now add attributes if they are different from the defaults.

	// If not filled, then set the ignore bit on the fill attribute.
	if (!pPath->InkPath.IsFilled)
		CurrentAttrs[ATTR_FILLGEOMETRY].Ignore = TRUE;

	// Add attributes to the path, if they are different from the default...
	BOOL Result = AttributeManager::ApplyBasedOnDefaults(pPath, CurrentAttrs);

	// Enable the fill attribute again
	CurrentAttrs[ATTR_FILLGEOMETRY].Ignore = FALSE;

	// Return success or failure
	return Result;
}

/********************************************************************************************

>	BOOL AcornDrawFilter::DoExport(Operation*, CCLexFile*, PathName*, Document*, BOOL)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/03/94
	Inputs:		As base class version.
	Returns:	FALSE.
	Purpose:	None; this is a dummy function as we don't export in Acorn Draw format (yet).
	SeeAlso:	Filter::DoExport; Filter

********************************************************************************************/

BOOL AcornDrawFilter::DoExport(Operation*, CCLexFile*, PathName*, Document*, BOOL)
{
	// We don't export in Draw format
	return FALSE;
}

