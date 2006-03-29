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
// Handling RIFF files (used by CDR importer)

#ifndef INC_RIFFFILE
#define INC_RIFFFILE

#include "riffform.h"
#include "list.h"
#include "ccfile.h"

typedef enum {
	RIFFOBJECTTYPE_NONE,			// initial condition
	RIFFOBJECTTYPE_CHUNK,
	RIFFOBJECTTYPE_LISTSTART,
	RIFFOBJECTTYPE_LISTEND,
	RIFFOBJECTTYPE_FILEEND
} RIFFObjectType;

// a class for storeing level information
class RIFFFileLevel : public ListItem
{
friend class RIFFFile;

protected:
	INT32 Start;		// the offset in the file at which this object starts
	INT32 End;		// where it ends
	FOURCC Type;	// and the type of this list
};

/********************************************************************************************

>	class RIFFFile : public CC_CLASS_MEMDUMP

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>	
	Created:	21 03 95	
	Purpose:	Nice class for reading RIFF files, takes a CCFile in the Init() routine
				to get it's data from.
				This doesn't view a RIFF file quite how they're actually stored on the disc.
				It's viewed as a series of Objects. These objects can be
					Chunks
					Start of lists
					End of lists
					End of file
				An object inside a list has a higher level than the list and chunks just
				before the list.
				You ask for the next object with NextObject(). This reads in the details
				about the next object in the file, which you can then read with the Get
				functions.
				You can get at the data with GetChunkData - this will error if the last
				object is not a chunk. This function reads the data into a block you give it.
				Alternatively, use AquireChunkData which will read it into a block maintained
				by this class.
				Data is not read unless it has to be.
				RepeatCurrent() will repeat the current object, giving one level of rewind.
				Useful for reading files with naff structures.
				SkipToListEnd() will skip to the end of the list of the level given.
				
				It's recommended that you have a quite shufty at the documentation in the
				MSDN about RIFF files.  

********************************************************************************************/

class RIFFFile : public CC_CLASS_MEMDUMP
{
	CC_DECLARE_MEMDUMP(RIFFFile);

public:
	RIFFFile();
	~RIFFFile();

	FOURCC Init(CCLexFile *File, BOOL NoHeader = FALSE);
	BOOL NextObject(void);
	BOOL GetChunkData(ADDR Block, INT32 Size);
	BOOL GetChunkData(ADDR Block, INT32 Size, INT32 Offset);
	BOOL AquireChunkData(void);
	BOOL SkipToListEnd(UINT32 Level);
	BOOL RepeatCurrent(void);
	BOOL GetListContents(ADDR *Block, INT32 *Size);
	BOOL AquireListContents(INT32 *ListSize);

	// checks to see if a start of a file is a RIFF file. Returns the form type if it's
	// a valid RIFF header, or 0 if not
	static FOURCC CheckRIFFHeader(ADDR Header, DWORD FileSize);

	static BOOL FindChunkWithinListContents(ADDR ListContents, INT32 ListSize, FOURCC Type, INT32 Entry, ADDR *Chunk, INT32 *ChunkSize);

	RIFFObjectType GetObjType(void) {return ObjType;}
	FOURCC GetObjChunkType(void) {return ObjChunkType;}
	INT32 GetObjSize(void) {return ObjSize;}
	UINT32 GetObjLevel(void) {return ObjLevel;}
	ADDR GetAquiredData(void) {return AquiredData;}

	INT32 ProgressLimit(void) {return Size;}			// the last value progress will return
	INT32 ProgressNow(void) {return Location;}		// the current position for the progress bar

private:
	CCLexFile *File;			// the file the RIFF file comes from

	RIFFObjectType ObjType;		// object type of current object
	FOURCC ObjChunkType;		// type of current chunk (or type of list)
	INT32 ObjSize;				// size in bytes of current chunk
	UINT32 ObjLevel;				// level of object
	INT32 ObjLocation;			// location of start of the current object

	UINT32 CurrentLevel;			// level we're currently at

	INT32 Location;				// current location within the file
	INT32 Size;					// size of the file

	BOOL GotData;				// Have we already got the data for this object?
								// this is set even if it's only partially got

	INT32 AlignedObjectSize;		// the size of this object aligned to the actual
								// size it takes up in the file

	List Levels;				// list of information about the levels we're on
								// the tail tells us about the current level...
								// items are deleted and created as we process the file

	BOOL RepeatObject;			// repeat the current object on the next NextObject()
	BOOL Error;					// had an error

	BYTE *AquiredData;			// pointer to aquired data
	INT32 AquiredDataSize;		// size of aquired data block

private:
	BOOL EnsureAquiredSizeIsAtLeast(INT32 Size);
};

#endif  // INC_RIFFFILE

