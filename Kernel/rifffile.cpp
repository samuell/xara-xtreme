// $Id: rifffile.cpp 662 2006-03-14 21:31:49Z alex $
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
// *********RIFF file handling class

/*
*/

#include "camtypes.h"
#include "rifffile.h"
#include "errors.h"
#include "fixmem.h"
#include "riffdata.h"

DECLARE_SOURCE("$Revision: 662 $");

CC_IMPLEMENT_MEMDUMP(RIFFFile, CC_CLASS_MEMDUMP)

#define new CAM_DEBUG_NEW

#define RIFFFILE_RETURNERR {Error = TRUE; return FALSE;}

#define ALIGNSIZE(x) ((((x) & 1) == 0)?(x):((x) + 1))


/********************************************************************************************

>	RIFFFile::RIFFFile(void)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21 03 95
	Inputs:		
	Returns:	
	Purpose:	Constructor for the RIFFFile class
	SeeAlso:	

********************************************************************************************/

RIFFFile::RIFFFile(void)
{
	File = NULL;
	Error = FALSE;
	AquiredData = NULL;
}


/********************************************************************************************

>	RIFFFile::~RIFFFile(void)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21 03 95
	Inputs:		
	Returns:	
	Purpose:	Destructor for the RIFFFile class
	SeeAlso:	

********************************************************************************************/

RIFFFile::~RIFFFile(void)
{
	// get rid of all the list entries in the file
	Levels.DeleteAll();

	// Get rid of the aquired object, if there is one
	if(AquiredData != 0)
		CCFree(AquiredData);
}


/********************************************************************************************

>	FOURCC RIFFFile::Init(CCLexFile *pFile, BOOL NoHeader = FALSE)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/03/95
	Inputs:		File to read from, whether it has a header
	Returns:	0 = not a RIFF file
				1 = error occured
				other = RIFF form type
	Purpose:	Initialises the class for reading in a RIFF file. If NoHeader is TRUE then
				the file has no header and a RIFF chunk is expected to start immediately.
	SeeAlso:	

********************************************************************************************/

FOURCC RIFFFile::Init(CCLexFile *pFile, BOOL NoHeader)
{
	// store Mr. NiceCCFile
	File = pFile;

	FOURCC FileType = 0;

	// if this file has a header, check it and get the type
	if(!NoHeader)
	{
		// read in the header of the file
		RIFFFile_Header Header;

		if(File->read(&Header, sizeof(Header)).bad())
			return FALSE;

	
		// and check it for being a valid form
		FileType = CheckRIFFHeader((ADDR)&Header, File->Size());

		// and was it?
		if(FileType == 0)
		  return 0;		// not a RIFF form
	}
		
	// OK, set up lots of nice variables for the funky business of RIFF reading
	ObjType = RIFFOBJECTTYPE_NONE;
	RepeatObject = FALSE;
	Error = FALSE;
	Location = NoHeader?0:sizeof(RIFFFile_Header);
	Size = File->Size();
	GotData = FALSE;
	CurrentLevel = 0;
	AquiredData = 0;
	AlignedObjectSize = 0;

	return FileType;
}

/********************************************************************************************

>	FOURCC RIFFFile::CheckRIFFHeader(ADDR Header, DWORD Size)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21 03 95
	Inputs:		None
	Returns:	0 = not a RIFF file
				other = RIFF form type
	Purpose:	Checks to see if the header supplied is that of a valid RIFF form. Useful for
				HowCompatable functions in filter code.
	SeeAlso:	

********************************************************************************************/

FOURCC RIFFFile::CheckRIFFHeader(ADDR Header, DWORD Size)
{
PORTNOTE("byteorder", "TODO: Check byte ordering")
	RIFFFile_Header *Hdr = (RIFFFile_Header *)Header;

	if(RIFFDATA_FOURCC(Hdr->CK.ckID) != RIFFTYPE_RIFF)
		return 0;		// file doesn't start "RIFF"

	if(RIFFDATA_DWORD(Hdr->CK.ckSize) > (Size - sizeof(RIFFFile_Header)) &&
				RIFFDATA_DWORD(Hdr->CK.ckSize) < (Size - sizeof(RIFFFile_Header)))
		return 0;		// Size doesn't match that in header. Be a bit relaxed about exact lengths

	return Hdr->FormType;
}


/********************************************************************************************

>	BOOL RIFFFile::NextObject(void)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21 03 95
	Inputs:		None
	Returns:	whether an error occured
	Purpose:	Initialises the class for reading in a RIFF file
	SeeAlso:	

********************************************************************************************/

BOOL RIFFFile::NextObject(void)
{
	if(Error)
		return FALSE;

	// do we need to return the current object again?
	if(RepeatObject)
	{
		RepeatObject = FALSE;
		return TRUE;			// all data nicely set up already
	}

	// skip over any data it it hasn't already been got
	if(ObjType == RIFFOBJECTTYPE_CHUNK && (GotData == FALSE || Location != (ObjLocation + AlignedObjectSize)))
	{
		if(File->seek(ObjLocation + AlignedObjectSize).bad()) {
TRACEUSER( "Ben", _T("RIFFFile: Error on seek when jumping object data\n"));
			RIFFFILE_RETURNERR;
		}

		Location = ObjLocation + AlignedObjectSize;

		// as we may come this way again...
		GotData = TRUE;
	}

	// have we gone past the end of the file?
	if(Location >= Size)
	//if (File->eof())
	{
		// Yes. Firstly, ensure that all the lists got the end list thingy - is the list empty?
		if(!Levels.IsEmpty())
		{
TRACEUSER( "Ben", _T("RIFFFile: Ending... terminating list\n"));
			RIFFFileLevel *Level = (RIFFFileLevel *)Levels.GetTail();

			ERROR3IF(Level == NULL, "List class returned a null tail for a non empty list! Help!");
			
			// set up a nice list end object
			ObjType = RIFFOBJECTTYPE_LISTEND;
			ObjChunkType = Level->Type;
			ObjLevel = CurrentLevel;
			ObjLocation = Location;
			GotData = TRUE;

			// level down one
			CurrentLevel--;

			// get rid of the last level on the list
			delete Levels.RemoveTail();

			return TRUE;
		}

		// And if they all have, say that the file ended
		ObjType = RIFFOBJECTTYPE_FILEEND;
		ObjChunkType = 0;
		ObjSize = 0;
		ObjLevel = 0;
		ObjLocation = Location;
		GotData = TRUE;
TRACEUSER( "Ben", _T("returning file end\n"));
		return TRUE;
	}

	// check to see if the current list has ended
	if(!Levels.IsEmpty())
	{
		RIFFFileLevel *Level = (RIFFFileLevel *)Levels.GetTail();
		
		ERROR3IF(Level == NULL, "List class returned a null tail for a non empty list! Help!");

		if(Location >= Level->End)
		{
			// RIFF list has ended
			ObjType = RIFFOBJECTTYPE_LISTEND;
			ObjChunkType = Level->Type;
			ObjLevel = CurrentLevel;
			ObjSize	= 0;
			ObjLocation = Location;
			GotData = TRUE;
			
			// down a level
			CurrentLevel--;

			// delete the last entry in the list
			delete Levels.RemoveTail();

			return TRUE;
		}
	}

	// right then, let's see what the next object in the file is.
	RIFFck ChunkHeader;

	if(File->read(&ChunkHeader, sizeof(ChunkHeader)).bad())
	{
TRACEUSER( "Ben", _T("RIFFFile: file error on reading chunk header\n"));
 		RIFFFILE_RETURNERR;
	}

	Location += sizeof(ChunkHeader);

	// is it a list chunk?
	if(RIFFDATA_FOURCC(ChunkHeader.ckID) == RIFFTYPE_LIST)
	{
		// yep... process it
		RIFFFile_List ListHdr;

		// read in a list header
		if(File->read(&ListHdr, sizeof(ListHdr)).bad())
		{
TRACEUSER( "Ben", _T("RIFFFile: file error on reading list id\n"));
			RIFFFILE_RETURNERR;
		}

		Location += sizeof(ListHdr);

		// make a new entry in the list
		RIFFFileLevel *NewLevel;

		if((NewLevel = new RIFFFileLevel) == 0)
			RIFFFILE_RETURNERR;

	 	NewLevel->Start = Location;
		NewLevel->End = Location + ALIGNSIZE(RIFFDATA_DWORD(ChunkHeader.ckSize)) - sizeof(ListHdr);
		NewLevel->Type = RIFFDATA_FOURCC(ListHdr.ListType);

		// and add it to the list
		Levels.AddTail(NewLevel);

		// up a level
		CurrentLevel++;
		
		// return a nice list start object
		ObjType = RIFFOBJECTTYPE_LISTSTART;
		ObjChunkType = RIFFDATA_FOURCC(ListHdr.ListType);
		ObjSize = 0;
		ObjLevel = CurrentLevel;
		ObjLocation = Location;
		GotData = TRUE;
		return TRUE;
	}

	// and after all that excitement, if it was just a normal, boring chunk, return stuff about that
	ObjType = RIFFOBJECTTYPE_CHUNK;
	ObjChunkType = RIFFDATA_FOURCC(ChunkHeader.ckID);
	ObjSize = RIFFDATA_DWORD(ChunkHeader.ckSize);
	ObjLevel = CurrentLevel;
	ObjLocation = Location;

	// set up some useful data
	GotData = FALSE;
	AlignedObjectSize = ALIGNSIZE(ObjSize);

	// and that's all folks
	return TRUE;
}


/********************************************************************************************

>	BOOL RIFFFile::GetChunkData(ADDR Block, INT32 Size)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21 03 95
	Inputs:		Pointer to the block where you want the data, and the size of the block
	Returns:	whether an error occured
	Purpose:	Gets the all data from a RIFF chunk
	SeeAlso:	

********************************************************************************************/

BOOL RIFFFile::GetChunkData(ADDR Block, INT32 BSize)
{
	// first of all, check to see if the caller has been a little bit silly
	if(ObjType != RIFFOBJECTTYPE_CHUNK)
	{
		Error = TRUE;		// only chunks have getable data
		ERROR2(FALSE, "RIFFFile::GetChunkData called for a non-chunk object\n");
	}

	if(BSize < ObjSize)
	{
		Error = TRUE;		// wrong size...
		ERROR2(FALSE, "RIFFFile::GetChunkData called with a block which is just *too* small\n");
	}

	// has the data already been got for this object?
	if(GotData)
	{
		// yep, go back and get it again
TRACEUSER( "Ben", _T("Chunk data asked for more than once\n"));
		if(File->seek(ObjLocation).bad())
			RIFFFILE_RETURNERR;

		Location = ObjLocation;
	}

	// and get that data!
	if(File->read(Block, ObjSize).bad())
	{
TRACEUSER( "Ben", _T("RIFFFile: file error when reading chunk data\n"));
		RIFFFILE_RETURNERR;
	}

	// ensure alignment is maintained within the file
	if(ObjSize != AlignedObjectSize)
	{
		// read a byte to align this to the correct position
		BYTE junk;

		if(File->read(&junk, sizeof(junk)).bad())
			RIFFFILE_RETURNERR;
	}

	// update locations
	Location += AlignedObjectSize;

	// and set an essential flag!
	GotData = TRUE;

	return TRUE;
}


/********************************************************************************************

>	BOOL RIFFFile::GetChunkData(ADDR Block, INT32 Size, INT32 Offset)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21 03 95
	Inputs:		Pointer to the block where you want the data, and the size of the block
				and the offset within the block of the data you want
	Returns:	whether an error occured
	Purpose:	Gets a certain amount of the data from a RIFF chunk. ATM it's a bit
				inefficient
	SeeAlso:	

********************************************************************************************/

BOOL RIFFFile::GetChunkData(ADDR Block, INT32 BSize, INT32 Offset)
{
	// first of all, check to see if the caller has been a little bit silly
	if(ObjType != RIFFOBJECTTYPE_CHUNK)
	{
		Error = TRUE;		// only chunks have getable data
		ERROR2(FALSE, "RIFFFile::GetChunkData called for a non-chunk object\n");
	}

	// check offset is OK
	if(Offset >= ObjSize)
		Offset = ObjSize - 1;

	// check that we'll be grabbing a nice amount of data
	if((Offset + BSize) > ObjSize)
		BSize = ObjSize - Offset;
	
	// don't do anything if there's no data to get
	if(BSize <= 0)
		return TRUE;
	
	// where in the file do we wish to get data from?
	INT32 TargetLocation = ObjLocation + Offset;

	// seek to the data we want, but only if it's necessary
	if(TargetLocation != Location)
	{
		if(File->seek(TargetLocation).bad())
			RIFFFILE_RETURNERR;
	
		Location = TargetLocation;
	}

	// and get that data!
	if(File->read(Block, BSize).bad())
	{
TRACEUSER( "Ben", _T("RIFFFile: file error when reading chunk data\n"));
		RIFFFILE_RETURNERR;
	}

	// update locations
	Location += BSize;

	// and set an essential flag!
	GotData = TRUE;

	return TRUE;
}


/********************************************************************************************

>	BOOL RIFFFile::RIFFFile::SkipToListEnd(UINT32 Level)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21 03 95
	Inputs:		The level number of the list to skip to the end of
	Returns:	whether an error occured
	Purpose:	Skips to the end of a given level. ListEnd objects are not returned for the
				lists you skip
	SeeAlso:	

********************************************************************************************/

BOOL RIFFFile::SkipToListEnd(UINT32 LevelNumber)
{
	RIFFFileLevel *Level;
	RIFFFileLevel *Next;

TRACEUSER( "Ben", _T("SkipToListEnd called to level %d\n"), LevelNumber);
	Level = (RIFFFileLevel *)Levels.FindItem(LevelNumber - 1);
				// - 1 because level 0 doesn't have an entry as
				// it's the root level of the file

	ERROR2IF(Level == 0, FALSE, "Couldn't find given level to jump to in list");

	// seek to the end of the level
	if(File->seek(Level->End).bad())
	{
TRACEUSER( "Ben", _T("RIFFFile: file error when seeking to end of level\n"));
		RIFFFILE_RETURNERR;
	}

	// update the location
	Location = Level->End;

	// update the level we're at
	CurrentLevel = LevelNumber - 1;
	
	// remove the last few entries from the list, including this one
	while(Level != 0)
	{
		Next = (RIFFFileLevel *)Levels.GetNext(Level);
		delete Levels.RemoveItem(Level);
		Level = Next;
	}	

	return TRUE;
}


/********************************************************************************************

>	BOOL RIFFFile::EnsureAquiredSizeIsAtLeast(INT32 Size)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	06/06/95
	Inputs:		minimum size required
	Returns:	whether an error occured
	Purpose:	Ensures that the aquired block is at least a certain size
	SeeAlso:	

********************************************************************************************/

BOOL RIFFFile::EnsureAquiredSizeIsAtLeast(INT32 Size)
{
	if(AquiredData == 0 || AquiredDataSize < Size)
	{
		if(AquiredData != 0) 
		{
			CCFree(AquiredData);
			AquiredData = 0;
		}

		if((AquiredData = (BYTE *)CCMalloc(Size)) == 0)
			RIFFFILE_RETURNERR;

		AquiredDataSize = Size;
	}

	return TRUE;
}


/********************************************************************************************

>	BOOL RIFFFile::AquireChunkData()

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21 03 95
	Inputs:		none
	Returns:	whether an error occured
	Purpose:	Gets the data from a RIFF chunk	and puts it into a buffer maintained by the
				class. Any previous data is lost.
	SeeAlso:	

********************************************************************************************/

BOOL RIFFFile::AquireChunkData()
{
	// check to see if the current object is a chunk
	ERROR2IF(ObjType != RIFFOBJECTTYPE_CHUNK, FALSE, "Tried an AquiredChunkData on a non-chunk RIFF object\n");
	
	// ensure that AquiredData points at enough space for this to be hoopy
	if(!EnsureAquiredSizeIsAtLeast(ObjSize))
		return FALSE;

	// there is at enough space in AquiredData to store the current object
	// use GetChunkData on it

	return GetChunkData(AquiredData, AquiredDataSize);
}


/********************************************************************************************

>	BOOL RIFFFile::AquireListContents(INT32 *ListSize)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	06/06/95
	Inputs:		none
	Returns:	whether an error occured
	Purpose:	Aquires the data of an entire list. Fills in ListSize to be the size of the
				data.
	SeeAlso:	

********************************************************************************************/

BOOL RIFFFile::AquireListContents(INT32 *ListSize)
{
	ERROR2IF(ObjType != RIFFOBJECTTYPE_LISTSTART, FALSE, "Tried an GetListContents on a non-list start RIFF object\n");

	// OK, first thing we need to do is to get the list's header... where is it?
	INT32 StartLocation = Location - sizeof(RIFFck) - sizeof(RIFFFile_List);

	// seek there
	if(File->seek(StartLocation).bad())
	{
		RIFFFILE_RETURNERR;
	}

	// OK, get the chunk header (not the list header which gives it's type
 	RIFFck ChunkHeader;

	if(File->read(&ChunkHeader, sizeof(ChunkHeader)).bad())
	{
 		RIFFFILE_RETURNERR;
	}

	// right then, we now know how much data we need to extract from the file...
	INT32 ContentsSize =	RIFFDATA_DWORD(ChunkHeader.ckSize) + sizeof(RIFFck);

	// now we need a block of data to get it into
	if(!EnsureAquiredSizeIsAtLeast(ContentsSize))
		return FALSE;

	// and getting it would be a cunning plan! seek to the beginning
	if(File->seek(StartLocation).bad())
	{
		RIFFFILE_RETURNERR;
	}

	// grab the data
	if(File->read(AquiredData, ContentsSize).bad())
	{
		RIFFFILE_RETURNERR;
	}
	
	// seek back to the location we were at the beginning
	if(File->seek(Location).bad())
	{
		RIFFFILE_RETURNERR;
	}

	// and set up the return values
	*ListSize = ContentsSize;
	
	return TRUE;
}


/********************************************************************************************

>	BOOL RIFFFile::RepeatCurrent()

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21 03 95
	Inputs:		none
	Returns:	whether an error occured
	Purpose:	Makes the next NextObject return the current object
	SeeAlso:	

********************************************************************************************/

BOOL RIFFFile::RepeatCurrent()
{
	if(RepeatObject == TRUE)
	{
TRACEUSER( "Ben", _T("RepeatObject already set for this object...\n"));
	}

	RepeatObject = TRUE;

	return TRUE;
}


/********************************************************************************************

>	BOOL RIFFFile::GetListContents(ADDR *Block, INT32 *Size)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/04/95
	Inputs:		pointer to a pointer for the block address, pointer to size to return size
	Returns:	whether an error occured
	Purpose:	gets the contents from a list. This is in the raw form from the RIFF file
	SeeAlso:	

********************************************************************************************/

BOOL RIFFFile::GetListContents(ADDR *Block, INT32 *Size)
{
	ERROR2IF(ObjType != RIFFOBJECTTYPE_LISTSTART, FALSE, "Tried an GetListContents on a non-list start RIFF object\n");

	// OK, first thing we need to do is to get the list's header... where is it?
	INT32 StartLocation = Location - sizeof(RIFFck) - sizeof(RIFFFile_List);

	// seek there
	if(File->seek(StartLocation).bad())
	{
		RIFFFILE_RETURNERR;
	}

	// OK, get the chunk header (not the list header which gives it's type
 	RIFFck ChunkHeader;

	if(File->read(&ChunkHeader, sizeof(ChunkHeader)).bad())
	{
 		RIFFFILE_RETURNERR;
	}

	// right then, we now know how much data we need to extract from the file...
	INT32 ContentsSize =	RIFFDATA_DWORD(ChunkHeader.ckSize) + sizeof(RIFFck);

	// now we need a block of data to get it into
	ADDR Contents = (ADDR)CCMalloc(ContentsSize);
	
	if(Contents == 0)
		return FALSE;

	// and getting it would be a cunning plan! seek to the beginning
	if(File->seek(StartLocation).bad())
	{
		RIFFFILE_RETURNERR;
	}

	// grab the data
	if(File->read(Contents, ContentsSize).bad())
	{
		RIFFFILE_RETURNERR;
	}
	
	// seek back to the location we were at the beginning
	if(File->seek(Location).bad())
	{
		RIFFFILE_RETURNERR;
	}

	// and set up the return values
	*Block = Contents;
	*Size = ContentsSize;
	
	return TRUE;
}


/********************************************************************************************

>	BOOL RIFFFile::FindChunkWithinListContents(ADDR ListContents, INT32 ListSize, FOURCC Type,
				INT32 Entry, ADDR *Chunk, INT32 *ChunkSize);

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	06/06/95
	Inputs:		ListContents	Pointer to list contents
				ListSize		Size of the list
				Type			Type of chunk required
				Entry			The number of that type of chunk to skip
				Chunk			Pointer to a pointer to a chunk to be filled in
				ChunkSize		Pointer to a value to be filled in with the chunk length.
	Returns:	whether the chunk could be found
	Purpose:	Finds a chunk within a list

********************************************************************************************/

BOOL RIFFFile::FindChunkWithinListContents(ADDR ListContents, INT32 ListSize, FOURCC Type,
		INT32 Entry, ADDR *Chunk, INT32 *ChunkSize)
{
	RIFFck *Header;
	INT32 OffsetWithin = sizeof(RIFFck) + sizeof(RIFFFile_List);

	while(OffsetWithin < ListSize)
	{
		Header = (RIFFck *)(ListContents + OffsetWithin);

		INT32 Size = RIFFDATA_DWORD(Header->ckSize);
		if(RIFFDATA_FOURCC(Header->ckID) == Type)
		{
			if(Entry <= 0)
			{
				// return this one...
				if(Chunk != 0)
					(*Chunk) = ListContents + OffsetWithin + sizeof(RIFFck);

				if(ChunkSize != 0)
					(*ChunkSize) = Size;

				return TRUE;
			}
			else
			{
				Entry--;
			}
		}
		OffsetWithin += Size + sizeof(RIFFck);
	}

	return FALSE;
}

