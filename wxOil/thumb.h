// $Id: thumb.h 662 2006-03-14 21:31:49Z alex $
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

// Thumbnail cache class for the library system
// Cheap 'n' Cheesy version knocked up by Richard
/***********************************************************************/

#ifndef INC_THUMB
#define INC_THUMB

#include "bitmap.h"
//#include "pathname.h"

typedef void * LP_SGTHUMBS_MOREINFO;

/***********************************************************************/

class Library;
class PathName;

// All the types of library
enum SGLibType
{
	SGLib_Blank = 0,	// Unknown library type
	SGLib_ClipArt,		// Clipart libraries
	SGLib_Font,			// Font libraries
	SGLib_Bitmap,		// Bitmap libs
	SGLib_Texture,		// Texture libs
	SGLib_Colour,		// Colour libs
	SGLib_Fractal,		// Fractal fill libs
	SGLib_ClipArt_WebThemes
};

// All thumbnail sizes
enum SGThumbSize
{
	SGThumb_Small = 0,	// Small thumbnails
	SGThumb_Medium,
	SGThumb_Large
};	


/***********************************************************************/

class SGThumbs : public CC_CLASS_MEMDUMP
{
	CC_DECLARE_MEMDUMP(SGThumbs)

public:

	SGThumbs();

	// Destructor - calls DeleteThumbnails() - returns all memory
	~SGThumbs();

	// Constructor which sets the Size of thumbnails as well
	SGThumbs(PathName *Dir, SGLibType Type, SGThumbSize Size);
									
	// Get a thumbnail from the cache, disk, or if Directory == NULL try and generate it.
	BOOL GetThumbnail(UINT32 ID, BOOL Urgent, KernelBitmap **Result, PathName *File = NULL, LP_SGTHUMBS_MOREINFO MoreInfo = NULL);
	
	// Set thumbnail size - all returned thumbnails will be this size after this call
	BOOL SetSize(SGThumbSize Size);

	// Get current thumbnail size setting
	BOOL GetSize(SGThumbSize *Size);

	// Delete all thumbnail cache entries and return the memory
	BOOL DeleteThumbnails(void);

	// Debugging stuff - do not call
	void DumpCache(char *DumpFile);

	// Called when we want to change MaxThumbnails;
	static BOOL ResizeAllThumbCaches(INT32 Entries);

	// Access to the protected members
	inline PathName *GetDirectory() {return Directory;}
	inline SGLibType GetType() {return ThumbnailsLibType;}

protected:

	// Return encrypted thumbnail filename
	BOOL GetThumbnailName(UINT32 ID, String_8 *Result);

	// Sets the internal structures to sensible values
	// Assumes pointers not pointing to NULL are not pointing to memory
	BOOL InitThumbnails(SGLibType Type, SGThumbSize Size);

	// Return TRUE and an offset if ID is in cache
	BOOL OffsetInCache(UINT32 ID, UINT32 *Offset);
	
	// Find an unused slot, if there are non, use the one with the least Usage
	BOOL FindNewSlotInBuffer(UINT32 *Slot);

	// Load a thumbnail into a given slot
	BOOL LoadThumbnailIntoSlot(UINT32 Slot, UINT32 ID, PathName *ActualFile = NULL);

private:

	// Implementation helper function to check file existence
	BOOL OpenThumbnailFile( CCDiskFile* pFile, const PathName& ThumbFileName ) const;

protected:

	// Use a more fabby data-structure when we've got CCList access
	struct TN {
		UINT32 ID;				// ID for thumbnail held below
		UINT32 Usage;			// Usage count - used for finding new slots
		BOOL Valid;				// Are buffer and ID valid ?
		SGThumbSize Size;		// Size of thumbnail
		KernelBitmap *buffer;	// Pointer to actual data
	};

	// Array of thumbnails
	TN *Thumbnails;

	// Number of thumbnails in array
	INT32 Entries;

	// Place where all the thumbnails are being stored
	PathName *Directory;
	
	// Have we initialised the data-structures yet ?
	BOOL ThumbnailsInitialised;

	// Size of thumbnail to load, cache and return
	SGThumbSize ThumbnailsSize;

	// Type of library we're dealing with here
	SGLibType ThumbnailsLibType;

public:
	// Maximum number of entries in thumbnail cache
	static INT32 MaxThumbnails;
};

/***********************************************************************/

#endif
