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
// handles referenced descriptions for CMX import

#include "camtypes.h"
#include "riffform.h"
//#include "ccfile.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "document.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "colourix.h"
#include "collist.h"

#include "cmxifltr.h"
#include "cmxistut.h"
#include "cmxidata.h"
#include "cmxibitm.h"

#define CMX_COMPILE
#include "cdrfiltr.h"

#define new CAM_DEBUG_NEW

/********************************************************************************************

>	BOOL CMXImportFilter::Stage2_ReadMasterIndex(void)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/08/96
	Inputs:		None
	Returns:	error flag
	Purpose:	Processes the master index, placing offsets in the table so other routines
				can find their stuff

********************************************************************************************/

BOOL CMXImportFilter::Stage2_ReadMasterIndex(void)
{
	// here we find the master index, and get all the interesting objects from it
	Seek(Data->IndexFilePosition);		// go there

	// get the header of the section and have a peep at it
	RIFFck chdr;
	pFile->read(&chdr, sizeof(chdr));

	if(chdr.ckID != cmxRIFFCI_MasterIndex)
		CMXFORMATERROR(FALSE)

	// get the header of the thingy
	CI_READDATA(MasterIndexHeader, mih)

	if(mih.MasterID != cmxMASTERIN_MASTERIN)
		CMXFORMATERROR(FALSE)

	TRACEUSER( "Ben", _T("Reading master index, has %d records\n"),mih.RecordCount);

	for(INT32 l = 0; l < mih.RecordCount; l++)
	{
		CI_READDATA(MasterIndexEntry, mie)

		TRACEUSER( "Ben", _T("Entry: type = %d, value = %d\n"), mie.IndexRecordID, mie.Offset);

		// see what type the thing is
		if(mie.IndexRecordID >= 0 && mie.IndexRecordID < cmxMASTERIN_MAXENTRY)
		{
			// it's in the range we're interested it, store the offset
			Data->MasterIndexEntries[mie.IndexRecordID] = mie.Offset;
		}
	}

	TRACEUSER( "Ben", _T("finished reading master index\n"));

	return TRUE;
}


/********************************************************************************************

>	BOOL CMXImportFilter::Stage3_ConvertDescriptionSections(void)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/08/96
	Inputs:		None
	Returns:	error flag
	Purpose:	Goes through the description sections reading the data, and creates all the
				nice arrays of stuff.

********************************************************************************************/

BOOL CMXImportFilter::Stage3_ConvertDescriptionSections(void)
{
	if(!ConvertColours()
		|| !ConvertPens()
		|| !ConvertLineStyles()
		|| !ConvertDotDashes()
		|| !ConvertArrowShapes()
		|| !ConvertArrowheads()
		|| !ConvertOutlines()
		|| !ConvertFonts()
		|| !ReadProcedureIndex()
		|| !ReadBitmapIndex()
		|| !ReadFontIndex()
		|| !ReadEmbeddedFileIndex())

		return FALSE;

	return TRUE;
}


/********************************************************************************************

>	BOOL CMXImportFilter::ConvertColours(void)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/08/96
	Inputs:		None
	Returns:	error flag
	Purpose:	converts the colours, reading them into an array of bits and pieces

********************************************************************************************/

BOOL CMXImportFilter::ConvertColours(void)
{
	// go to the right place in the file
	if(!GotoSectionFromIndex(cmxMASTERIN_COLOUR, cmxRIFFCI_Desc_Colour, TRUE))
		return FALSE;

	// read the header
	CI_READDATA(SimpleIndexHeader, ih)

	// do the record count thing... allocate some room for the colours
	Data->Colours = new CMXImportColour[ih.RecordCount];
	TRACEUSER( "Ben", _T("file has %d colours\n"), ih.RecordCount);

	// and run through the file getting the thingys
	for(INT32 l = 0; l < ih.RecordCount; l++)
	{
		CI_READDATA(Colour, col)

		Data->Colours[l].Convert(&col);
	}

	// set number of colours
	Data->NumColours = ih.RecordCount;

	// done
	return TRUE;
}


/********************************************************************************************

>	BOOL CMXImportFilter::ConvertOutlines(void)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/08/96
	Inputs:		None
	Returns:	error flag
	Purpose:	converts the outlines, reading them into an array of bits and pieces

********************************************************************************************/

BOOL CMXImportFilter::ConvertOutlines(void)
{
	// go to the right place in the file
	BOOL Found;
	if(!GotoSectionFromIndex(cmxMASTERIN_OUTLINE, cmxRIFFCI_Desc_Outline, FALSE, &Found))
		return FALSE;

	if(!Found)
		return TRUE;		// not a problem if there isn't an outline list

	// check we have some pens
	ERROR2IF(Data->Pens == NULL, FALSE, "No pens when we tried to convert the outlines");
	// check we have some line styles
	ERROR2IF(Data->LineStyles == NULL, FALSE, "No pens when we tried to convert the outlines");

	// read the header
	CI_READDATA(SimpleIndexHeader, ih)

	// do the record count thing... allocate some room for the colours
	Data->Outlines = new CMXImportOutline[ih.RecordCount];
	TRACEUSER( "Ben", _T("file has %d outlines\n"), ih.RecordCount);

	// and run through the file getting the thingys
	for(INT32 l = 0; l < ih.RecordCount; l++)
	{
		CI_READDATA(Outline, out)

		Data->Outlines[l].Convert(&out, Data);
	}

	// set number of outlines
	Data->NumOutlines = ih.RecordCount;

	// we no longer need the pens
	delete [] Data->Pens;
	Data->Pens = NULL;

	// done
	return TRUE;
}


/********************************************************************************************

>	BOOL CMXImportFilter::ConvertPens(void)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/08/96
	Inputs:		None
	Returns:	error flag
	Purpose:	converts the outlines, reading them into an array of bits and pieces

********************************************************************************************/

BOOL CMXImportFilter::ConvertPens(void)
{
	// go to the right place in the file
	BOOL Found;
	if(!GotoSectionFromIndex(cmxMASTERIN_PEN, cmxRIFFCI_Desc_Pen, FALSE, &Found))
		return FALSE;

	if(!Found)
		return TRUE;		// not a problem if it doesn't exist

	// read the header
	CI_READDATA(SimpleIndexHeader, ih)

	// do the record count thing... allocate some room for the colours
	Data->Pens = new CMXImportPen[ih.RecordCount];
	TRACEUSER( "Ben", _T("file has %d pens\n"), ih.RecordCount);

	// and run through the file getting the thingys
	INT32 l;
	for(l = 0; l < ih.RecordCount; l++)
	{
		CI_READDATA(Pen, pen)

		Data->Pens[l].Convert(&pen);
	}

	// set number of pens
	Data->NumPens = ih.RecordCount;

	// done
	return TRUE;
}

/********************************************************************************************

>	BOOL CMXImportFilter::ConvertFonts(void)

	Author:		Claude_Quemerais (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/08/96
	Inputs:		None
	Returns:	error flag
	Purpose:	

********************************************************************************************/

BOOL CMXImportFilter::ConvertFonts(void)
{
	return TRUE;
}



/********************************************************************************************

>	BOOL CMXImportFilter::ConvertLineStyles(void)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/08/96
	Inputs:		None
	Returns:	error flag
	Purpose:	converts the outlines, reading them into an array of bits and pieces

********************************************************************************************/

BOOL CMXImportFilter::ConvertLineStyles(void)
{
	// go to the right place in the file
	BOOL Found;
	if(!GotoSectionFromIndex(cmxMASTERIN_LINESTYLE, cmxRIFFCI_Desc_LineStyle, FALSE, &Found))
		return FALSE;

	if(!Found)
		return TRUE;		// not a problem if it doesn't exist

	// read the header
	CI_READDATA(SimpleIndexHeader, ih)

	// do the record count thing... allocate some room for the line styles
	Data->LineStyles = new CMXImportLineStyle[ih.RecordCount];
	TRACEUSER( "Ben", _T("file has %d line styles\n"), ih.RecordCount);

	// and run through the file getting the thingys
	INT32 l;
	for(l = 0; l < ih.RecordCount; l++)
	{
		CI_READDATA(LineStyle, style)

		Data->LineStyles[l].Convert(&style);
	}

	// set number of lines styles
	Data->NumLineStyles = ih.RecordCount;

	// done
	return TRUE;
}


/********************************************************************************************

>	BOOL CMXImportFilter::ConvertArrowShapes(void)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/08/96
	Inputs:		None
	Returns:	error flag
	Purpose:	converts the arrow shapes, reading them into an array of bits and pieces

********************************************************************************************/

BOOL CMXImportFilter::ConvertArrowShapes(void)
{
	// go to the right place in the file
	BOOL Found;
	INT32 Size = 0;
	if(!GotoSectionFromIndex(cmxMASTERIN_ARROW, cmxRIFFCI_ArrowIndex, FALSE, &Found, &Size))
		return FALSE;

	if(!Found)
		return TRUE;		// not a problem if it doesn't exist

	// get the index header
	CI_READDATA(ArrowShapeIndexHeader, ih)
	
	// do the record count thing... allocate some room for the arrow shapes
	Data->ArrowShapes = new CMXImportArrowShape[ih.RecordCount];
	TRACEUSER( "Ben", _T("file has %d arrow shapes\n"), ih.RecordCount);

	INT32 SizeInFile = (Is32Bit)?(ih.SizeInFile):(sizeof(DWORD));

	// and run through the file getting the thingys
	INT32 l;
	for(l = 0; l < ih.RecordCount; l++)
	{
		// get the index entry
		INT32 EndEntryFilePos = Tell() + SizeInFile;
		CI_READDATA(ArrowShapeIndexEntry, en)
		Seek(en.Offset);

		// read the arrow shape, and convert it
		CI_READDATA(ArrowShape, as)

		Data->ArrowShapes[l].Convert(&as, Is32Bit);

		// go back for the next entry
		Seek(EndEntryFilePos);
	}

	// set number of arrow shapes
	Data->NumArrowShapes = ih.RecordCount;

	// done
	return TRUE;
}


/********************************************************************************************

>	BOOL CMXImportFilter::ConvertArrowheads(void)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/08/96
	Inputs:		None
	Returns:	error flag
	Purpose:	converts the arrowheads, reading them into an array of bits and pieces

********************************************************************************************/

BOOL CMXImportFilter::ConvertArrowheads(void)
{
	// go to the right place in the file
	BOOL Found;
	if(!GotoSectionFromIndex(cmxMASTERIN_ARROWHEADS, cmxRIFFCI_Desc_Arrowheads, FALSE, &Found))
		return FALSE;

	if(!Found)
		return TRUE;		// not a problem if it doesn't exist

	// read the header
	CI_READDATA(SimpleIndexHeader, ih)

	// do the record count thing... allocate some room for the line styles
	Data->Arrowheads = new CMXImportArrowheads[ih.RecordCount];
	TRACEUSER( "Ben", _T("file has %d arrowheads\n"), ih.RecordCount);

	// and run through the file getting the thingys
	INT32 l;
	for(l = 0; l < ih.RecordCount; l++)
	{
		CI_READDATA(Arrowheads, ah)

		Data->Arrowheads[l].Convert(&ah, Data);
	}

	// set number of lines styles
	Data->NumArrowheads = ih.RecordCount;

	// done
	return TRUE;
}


/********************************************************************************************

>	BOOL CMXImportFilter::ConvertDotDashes(void)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/08/96
	Inputs:		None
	Returns:	error flag
	Purpose:	converts the dot dashes, reading them into an array of bits and pieces

********************************************************************************************/

BOOL CMXImportFilter::ConvertDotDashes(void)
{
	// go to the right place in the file
	BOOL Found;
	if(!GotoSectionFromIndex(cmxMASTERIN_DOTDASH, cmxRIFFCI_Desc_DotDash, FALSE, &Found))
		return FALSE;

	if(!Found)
		return TRUE;		// not a problem if it doesn't exist

	// read the header
	CI_READDATA(SimpleIndexHeader, ih)

	// do the record count thing... allocate some room for the dot dashes
	Data->DotDashes = new CMXImportDotDash[ih.RecordCount];
	TRACEUSER( "Ben", _T("file has %d dot dashes\n"), ih.RecordCount);

	// and run through the file getting the thingys
	INT32 l;
	for(l = 0; l < ih.RecordCount; l++)
	{
		CI_READDATA(DotDash, dot)

		Data->DotDashes[l].Convert(&dot);
	}

	// set number of lines styles
	Data->NumDotDashes = ih.RecordCount;

	// done
	return TRUE;
}


/********************************************************************************************

>	BOOL CMXImportFilter::ReadEmbeddedFileIndex(void)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/08/96
	Inputs:		None
	Returns:	error flag
	Purpose:	reads the embedded file index, creating the RImage array

********************************************************************************************/

BOOL CMXImportFilter::ReadEmbeddedFileIndex(void)
{
	// go to the right place in the file
	BOOL Found;
	if(!GotoSectionFromIndex(cmxMASTERIN_EMBEDFILE, cmxRIFFCI_EmbeddedFileIndex, FALSE, &Found))
		return FALSE;

	if(!Found)
		return TRUE;		// not a problem if it doesn't exist

	// read the header
	CI_READDATA(SimpleIndexHeader, ih)

	// do the record count thing... allocate some room for the colours
	Data->RImages = new CMXImportRImage[ih.RecordCount];
	TRACEUSER( "Ben", _T("file has %d embedded files\n"), ih.RecordCount);

	// and run through the file getting the thingys
	INT32 l;
	for(l = 0; l < ih.RecordCount; l++)
	{
		CI_READDATA(EmbeddedFileIndexEntry, en)

		switch(en.Type)
		{
		case cmxEMBEDFILETYPE_RIMAGE:
			// ok, mark an RImage at this position...
			Data->RImages[l].Mark(en.Offset);
			break;

		default:
			// don't need to do anything special
			break;
		}
	}

	// set number of lines styles
	Data->NumRImages = ih.RecordCount;

	return TRUE;
}


/********************************************************************************************

>	BOOL CMXImportFilter::ReadBitmapIndex(void)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/08/96
	Inputs:		None
	Returns:	error flag
	Purpose:	reads the embedded file index, creating the RImage array

********************************************************************************************/

BOOL CMXImportFilter::ReadBitmapIndex(void)
{
	// go to the right place in the file
	BOOL Found;
	if(!GotoSectionFromIndex(cmxMASTERIN_BITMAP, cmxRIFFCI_BitmapIndex, FALSE, &Found))
		return FALSE;

	if(!Found)
		return TRUE;		// not a problem if it doesn't exist

	// read the header
	CI_READDATA(BitmapIndexHeader, ih)

	// do the record count thing... allocate some room for the bitmaps
	Data->Bitmaps = new CMXImportBitmap[ih.RecordCount];
	TRACEUSER( "Ben", _T("file has %d bitmaps\n"), ih.RecordCount);

	INT32 SizeInFile = (Is32Bit)?(ih.SizeInFile):(sizeof(DWORD));

	// and run through the file getting the thingys
	INT32 l;
	for(l = 0; l < ih.RecordCount; l++)
	{
		INT32 Start = Tell();

		CI_READDATA(BitmapIndexEntry, en)

		Data->Bitmaps[l].Set(en.Offset);

		if(Tell() != (Start + SizeInFile))
			Seek(Start + SizeInFile);
	}

	// set number of bitmaps
	Data->NumBitmaps = ih.RecordCount;

	return TRUE;
}

/********************************************************************************************

>	BOOL CMXImportFilter::ReadFontIndex(void)

	Author:		Claude_Quemerais (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/08/96
	Inputs:		None
	Returns:	error flag
	Purpose:	reads the embedded font index

********************************************************************************************/

BOOL CMXImportFilter::ReadFontIndex(void)
{
	// go to the right place in the file
	BOOL Found;
	if(!GotoSectionFromIndex(cmxMASTERIN_FONT, cmxRIFFCI_FontIndex, FALSE, &Found))
		return FALSE;

	if(!Found)
		return TRUE;		// not a problem if it doesn't exist

	// read the header
	CI_READDATA(FontIndexHeader, ih)

	// do the record count thing... allocate some room for the fonts
	Data->Fonts = new CMXImportFont[ih.RecordCount];
	TRACEUSER( "Claude", _T("file has %d fonts\n"), ih.RecordCount);

	INT32 SizeInFile = (Is32Bit)?(ih.SizeInFile):(sizeof(DWORD));

	// and run through the file getting the thingys	
	for(INT32 l = 0 ; l < ih.RecordCount ; l++)
	{
		INT32 Start = Tell();

		CI_READDATA(FontIndexEntry, en)

		Seek(en.Offset);

		CI_READDATA(FontRecord, rec)

		Data->Fonts[l].Convert(&rec);
		
		
		
		Seek(Start + SizeInFile);
	}

	// set number of bitmaps
	Data->NumFonts = ih.RecordCount;

	return TRUE;
}


/********************************************************************************************

>	BOOL CMXImportFilter::ReadProcedureIndex(void)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/08/96
	Inputs:		None
	Returns:	error flag
	Purpose:	reads the embedded file index, creating the RImage array

********************************************************************************************/

BOOL CMXImportFilter::ReadProcedureIndex(void)
{
	// go to the right place in the file
	BOOL Found;
	if(!GotoSectionFromIndex(cmxMASTERIN_PROCEDURE, cmxRIFFCI_ProcIndex, FALSE, &Found))
		return FALSE;

	if(!Found)
		return TRUE;		// not a problem if it doesn't exist

	// read the header
	CI_READDATA(SimpleIndexHeader, ih)

	// do the record count thing... allocate some room for the colours
	Data->Procedures = new CMXImportProcedure[ih.RecordCount];
	TRACEUSER( "Ben", _T("file has %d procedures\n"), ih.RecordCount);

	// and run through the file getting the thingys
	INT32 l;
	for(l = 0; l < ih.RecordCount; l++)
	{
		CI_READDATA(ProcedureIndexEntry, en)

		Data->Procedures[l].Read(&en);
	}

	// set number of lines styles
	Data->NumProcedures = ih.RecordCount;

	return TRUE;
}



/********************************************************************************************

>	DocColour *CMXImportFilter::GetColourFromReference(INT32 Ref)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/08/96
	Inputs:		None
	Returns:	pointer to colour to use
	Purpose:	gets a pointer to a colour we can use given a reference number

********************************************************************************************/

DocColour *CMXImportFilter::GetColourFromReference(INT32 Ref)
{
	if(Ref <= 0 || Ref > Data->NumColours)
		CMXFORMATERROR(0);

	return Data->Colours[Ref-1].GetColour();
}


/********************************************************************************************

>	BOOL CMXImportFilter::AddColoursToDocument(void)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/08/96
	Inputs:		None
	Returns:	error flag
	Purpose:	adds the colours (only the ones which have been used) to the document
				to keep everything happy

********************************************************************************************/

BOOL CMXImportFilter::AddColoursToDocument(void)
{
	// sort out a colour list
	ColourList *ColList = TheDocument->GetIndexedColours();

	for(INT32 l = 0; l < Data->NumColours; l++)
	{
		// if the colour is used, add it's indexed colour to the list
		if(Data->Colours[l].IsUsed())
		{
			ColList->GetUnnamedColours()->AddTail(Data->Colours[l].GetIndexedColour());
		}
	}

	return TRUE; //CDRFilter::AddStandardColourSet(TheDocument);
}


