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
// Thumb.cpp - Thumbnail handling class for the library system

#include "camtypes.h"
#include "bitmap.h"
#include "oilbitmap.h"
#include "dibutil.h"
//#include "pngutil.h"
#include "thumb.h"
//#include "sgscan.h"
#include "bitfilt.h"
#ifndef WEBSTER
//#include "extfilts.h"
#endif //WEBSTER
#include "errors.h"
#include "fixmem.h"
#include "app.h"
//#include "sgfonts.h"
#include "ccfile.h"
#include "thumbmsg.h"
#include "sgliboil.h"
//#include "fontpgen.h"
#include "camfiltr.h"	// BaseCamelotFilter
//#include "cmxifltr.h"	// CMXImportFilter
#include "dialogop.h"

// Implement the dynamic class bits...
CC_IMPLEMENT_MEMDUMP(SGThumbs, CC_CLASS_MEMDUMP)

// The thumb messages
CC_IMPLEMENT_DYNAMIC(ThumbMessage, Msg)

// This line mustn't go before any CC_IMPLEMENT_... macros
#define new CAM_DEBUG_NEW

/***********************************************************************/

// MaxThumbnails is a static... it's read from the .ini file at some point
INT32 SGThumbs::MaxThumbnails = 25;

// Files larger than this won't be loaded and scaled for the bmp gallery
const INT32 ThumbFileSizeThreshold = 50*1024;

/***********************************************************************/

// Which files should we be checking to load in the thumbnails
//#define THUMB_CHECK_FILENAME_WITH_TIF
//#define THUMB_CHECKTIFS

// Check for reversed type endings -> .pmb instead of .bmp (in the thumbnails directory)
#define THUMB_CHECK_REVERSED_FILENAME

// Default thumbnails generated by the galleries use this naming convention
#define THUMB_CHECK_FILENAME_WITH_BMP

// Thumbnails generated for download use this naming convention
#define THUMB_CHECK_FILENAME_WITH_PNG

// Needed for font previews
//#ifndef WEBSTER
#define THUMB_CHECKBMPS
//#endif //WEBSTER
#define THUMB_CHECKPNGS

/***********************************************************************************************

>	SGThumbs::SGThumbs()

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/2/95
	Inputs:		
	Purpose:	Default constructor - DO NOT USE !
	Notes:
	SeeAlso:	SGThumbs::SGThumbs

***********************************************************************************************/

SGThumbs::SGThumbs()
{
	ERROR3("WRONG SGThumb constructor called");
}


/***********************************************************************************************

>	SGThumbs::~SGThumbs()

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/2/95
	Inputs:		
	Purpose:	Destroys a thumbnail cache, freeing memory as it goes
	Notes:
	SeeAlso:	SGThumbs::SGThumbs

***********************************************************************************************/
			   
SGThumbs::~SGThumbs()
{
	DeleteThumbnails();

	if(Thumbnails != NULL)
	{
		CCFree(Thumbnails);
		Thumbnails = NULL;
	}

	if(Directory != NULL)
	{
		delete Directory;
		Directory = NULL;
	}
}

/***********************************************************************************************

>	SGThumbs::SGThumbs(PathName *Dir, SGLibType Type, SGThumbSize Size)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/2/95
	Inputs:		Dir - Pathname for thumbnail directory (should end in \XaraInfo) (see notes)
				Type - Type of library we're dealing with
				Size - Size of thumbnails to display (SGThumb_Small, SGThumb_Medium, or SGThumb_Large)
	Purpose:	Creates a new thumbnail cache for use by the SG library system
	Notes:		If the Dir is NULL then we assume the thumbnails need generating rather than
				loading.
	SeeAlso:	SGThumbs::~SGThumbs

***********************************************************************************************/

SGThumbs::SGThumbs(PathName *Dir, SGLibType Type, SGThumbSize Size)
{
/*	if(Type == SGLib_ClipArt)
	{
		String_256 D(Dir->GetPath());
		ERROR3_PF(("SGThumbs initialised with %s", (TCHAR *)D));
	}*/
	
	// Search directory for thumbnails
	if(Dir != NULL)
		Directory = new PathName(*Dir);
	else
		Directory = NULL;
	
	InitThumbnails(Type, Size);
}

/***********************************************************************************************

>	BOOL SGThumbs::GetThumbnailName(UINT32 ID, String_8 *Filename)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/2/95
	Inputs:		ID - ID for the thumbnail
				Path - pointer to a string_8 to return the thumbnail name
	Purpose:	Builds an encrypted thumbnail filename for an ID, size and type...
	Notes:		Size and type information are held internally in the SGThumbs class.
				The extension isn't added here.
	SeeAlso:	SGThumbs::SGThumbs

***********************************************************************************************/

BOOL SGThumbs::GetThumbnailName(UINT32 ID, String_8 *Filename)
{
	ERROR3IF(Filename == NULL, "SGThumbs::GetThumbnailName passed a null ptr");

	TCHAR type;
	TCHAR size;

	switch(ThumbnailsLibType) {
		case SGLib_ClipArt:
		case SGLib_ClipArt_WebThemes:
			type = 'A';
			break;
		case SGLib_Bitmap:
			type = 'B';
			break;
		case SGLib_Colour:
			type = 'C';
			break;
		case SGLib_Font:
			type = 'F';
			break;
		case SGLib_Texture:
			type = 'T';
			break;
		default:
			type = ' ';
			break;
	}

	switch(ThumbnailsSize) {
		case SGThumb_Small:
			size = 'S';
			break;
		case SGThumb_Medium:
			size = 'M';
			break;
		case SGThumb_Large:
			size = 'L';
			break;
		default:
			size = ' ';
			break;
	}

	Filename->_MakeMsg(_T("%c%05d%c"), type, ID, size); 

	return TRUE;
}

/***********************************************************************************************

>	BOOL SGThumbs::InitThumbnails(SGLibType Type, SGThumbSize Size)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/2/95
	Inputs:		Type - Type of library
				Size - Initial size of thumbnail to use
	Purpose:	Sets the internal structures to sensible values
				Assumes pointers not pointing to NULL are not pointing to memory
				Allocates memory for thumbnail cache
	Notes:

***********************************************************************************************/

BOOL SGThumbs::InitThumbnails(SGLibType Type, SGThumbSize Size)
{	
//  Read in value for MaxThumbnails here
//	MaxThumbnails = 10;
//	GetApplication()->DeclareSection(TEXT("Libraries"), 6);
//	GetApplication()->DeclarePref(TEXT("Libraries"), TEXT("Thumbnails"), &MaxThumbnails, 1, 100);

	Entries = MaxThumbnails;

	Thumbnails = (struct TN*)CCMalloc(sizeof(struct TN) * Entries);
	if(Thumbnails != NULL)
	{
		INT32 i;
		ThumbnailsLibType = Type;
		ThumbnailsSize = Size;

		for(i=0; i<Entries; i++)
		{
			Thumbnails[i].ID = 0;
			Thumbnails[i].Usage = 0;
			Thumbnails[i].Valid = FALSE;
			Thumbnails[i].Size = ThumbnailsSize;
			Thumbnails[i].buffer = NULL;
		}
		ThumbnailsInitialised = TRUE;
		return TRUE;
	}
	else
	{
		ThumbnailsInitialised = TRUE;
		return FALSE;
	}
}

/***********************************************************************************************

>	BOOL SGThumbs::DeleteThumbnails(void)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/2/95
	Inputs:
	Purpose:	Delete all thumbnail cache entries and return the memory
	Notes:

***********************************************************************************************/

BOOL SGThumbs::DeleteThumbnails(void)
{
	INT32 i;
	if(!ThumbnailsInitialised) return FALSE;
	for(i=0; i<Entries; i++)
	{
		Thumbnails[i].ID = 0;
		Thumbnails[i].Usage = 0;
		Thumbnails[i].Valid = FALSE;
		Thumbnails[i].Size = ThumbnailsSize;
		if(Thumbnails[i].buffer)
		{
		 	//delete (CWxBitmap *)(Thumbnails[i].buffer->ActualBitmap);
			//Thumbnails[i].buffer->ActualBitmap = NULL;
			delete Thumbnails[i].buffer;
			Thumbnails[i].buffer = NULL;
		}
	}

	return TRUE;
}

/***********************************************************************************************

>	BOOL SGThumbs::OffsetInCache(UINT32 ID, UINT32 *Offset)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/2/95
	Inputs:		ID - Thumbnail ID
				Offset - Offset in cache which is used
	Purpose:	Return TRUE and an offset if ID is in cache
	Notes:

***********************************************************************************************/
				   
BOOL SGThumbs::OffsetInCache(UINT32 ID, UINT32 *Offset)
{
	if(Offset == NULL)
	{
		ERROR3("SGThumbs::OffsetInCache given a null offset");
		return FALSE;
	}

	INT32 i;
	// Check if we're already in the buffer 
	for(i=0; i<Entries; i++)
	{
		if(Thumbnails[i].ID == ID && Thumbnails[i].Valid && Thumbnails[i].Size == ThumbnailsSize)
		{
			*Offset = i;
			return TRUE;
		}
	}

	return FALSE;
}

/***********************************************************************************************

>	BOOL SGThumbs::FindNewSlotInBuffer(UINT32 *Slot)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/2/95
	Inputs:		Slot - Pointer to slot to use
	Purpose:	Find an unused slot, if there are non, use the one with the least Usage
	Notes:

***********************************************************************************************/

BOOL SGThumbs::FindNewSlotInBuffer(UINT32 *Slot)
{
	if(Slot == NULL)
	{
		ERROR3("SGThumbs::FindNewSlotInBuffer given a null slot");
		return FALSE;
	}

//	BOOL found = FALSE;
	INT32 i;

	for(i=0; i<Entries; i++)
	{
		if(!Thumbnails[i].Valid || (Thumbnails[i].Size != ThumbnailsSize))
		{
			*Slot = i;
			return TRUE;
		}
	}

	UINT32 SmallestOffset = 0;
	UINT32 Smallest = Thumbnails[0].Usage;

	for(i=1; i<Entries; i++)
	{
		if(Thumbnails[i].Usage < Smallest)
		{
			SmallestOffset = i;
			Smallest = Thumbnails[i].Usage;
		}
	}

	*Slot = SmallestOffset;
	return TRUE;
}


/***********************************************************************************************

>	BOOL SGThumbs::LoadThumbnailIntoSlot(UINT32 Slot, UINT32 ID, PathName *ActualFile = NULL)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/2/95
	Inputs:		Slot - Pointer to slot to use
				ID - ID of thumbnail being put in slot
				ActualFile - Filename of actual art or font file being referenced
	Purpose:	Load a thumbnail into a given slot
	Notes:		The way we find the thumbnail is currently a little non-obvious.
				...and changed on 02/04/97 Martin
				First we check to see if the original file is a type that contains an internal
				preview. art files start with a tiff, xar and web files have a tagged preview
				of any type or possibly none, cmx and photoCD files have their own internal
				formats. If the file does not exist (may not be downloaded yet) or it doesn't
				have an internal preview or it is a font file we look for an external preview
				testing for the most likely file extensions. All new previews have straight
				forward extensions like png for png files, unlike the old system which did file
				type reversal and who knows what else. Generally we try to use native Xara code
				but we always give Accusoft the last chance if we failed. We still don't use the
				filter system everywhere - this should be implemented sometime (instead of
				calling DIBUtil or Accusoft etc. directly).

***********************************************************************************************/

BOOL SGThumbs::LoadThumbnailIntoSlot(UINT32 Slot, UINT32 ID, PathName *ActualFile)
{
/*	String_256 P(ActualFile->GetPath());
	ERROR3_PF(("Actual file: %s", (TCHAR *)P));*/

	PORTNOTETRACE("dialog","SGThumbs::LoadThumbnailIntoSlot - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	if(Directory == NULL || ActualFile == NULL)
	{
		ERROR3("SGThumbs::LoadThumbnailIntoSlot Directory == NULL || ActualFile == NULL");
		return FALSE;
	}

	// Return any memory that would otherwise go down the toilet, and block the drains
	if(Thumbnails[Slot].Valid && Thumbnails[Slot].buffer)
	{
		delete Thumbnails[Slot].buffer;
		Thumbnails[Slot].buffer=NULL;
	}

	// Just in case we jump out in a state of unhappy rampantness
	Thumbnails[Slot].Valid = FALSE;

	BOOL CheckFilenameBmp = TRUE;
	BOOL CheckFilenamePng = TRUE;
	BOOL CheckFilenameReversed = TRUE;
	BOOL CheckFilenameTif = TRUE;
	BOOL CheckEncodedBmp = TRUE;
	BOOL CheckEncodedPng = TRUE;
	BOOL CheckEncodedTif = TRUE;

	BOOL UseActualFile = FALSE;

	BOOL IsRIFFTypeFile = FALSE;
	BOOL IsPCDTypeFile = FALSE;

	// Get lowercase filetype
	String_256 Type(ActualFile->GetType());
	Type.toLower();

	// Bodge to speed galleries up for art files - doesn't check or use external previews
	if( (Type.Sub((String_8)"art") != -1) ||
		(Type.Sub((String_8)"xar") != -1) )
	{
		UseActualFile = TRUE;
		CheckFilenameReversed = FALSE;
	}
	else if ( (Type.Sub((String_8)"web") != -1) ||
			  (Type.Sub((String_8)"cxw") != -1) ||
			  (Type.Sub((String_8)"cxn") != -1) )
	{
		// Check for our new version 2 native and web files
		//WEBSTER-Martin-23/01/97
		//force us to look for previews
		UseActualFile = TRUE;
		CheckFilenameReversed = FALSE;
	}
	else if ( (Type.Sub((String_8)"cdr") != -1) ||
			  (Type.Sub((String_8)"cmx") != -1) )
	{
		IsRIFFTypeFile = TRUE;
		UseActualFile = TRUE;
		CheckFilenameReversed = FALSE;
	}
	else if( Type.Sub((String_8)"ttf") != -1 ||
			 Type.Sub((String_8)"pfb") != -1)
	{
		// Bodge to speed font gallery up a bit - only checks encoded bmp file thumbnails
		CheckFilenamePng = 
			CheckFilenameBmp = 
			CheckFilenameReversed = 
			CheckFilenameTif = 
			CheckEncodedTif = FALSE;

		CheckEncodedBmp = TRUE;
		CheckEncodedPng = TRUE;

		UseActualFile = FALSE;
	}
	else if( Type.Sub((String_8)"pcd") != -1 )
	{
		IsPCDTypeFile = TRUE;
		UseActualFile = TRUE;
	}

	// Read the file into the bm yet ?
	BOOL Found = FALSE;
	CWxBitmap *pCWxBitmap = NULL;
	BOOL ReadOK = FALSE;

	// Don't error, just set exceptions... (changed 2/10/2000 by Jonathan to use
	// errors rather than exceptions to avoid a pile of messy first-change exceptions
	// when fills were not being found)
    CCDiskFile File(1024, FALSE, FALSE);

	// internal preview
	if(UseActualFile)
	{
		// Open file and check if it exists at the same time
		Found = OpenThumbnailFile( &File, *ActualFile );

		if(Found)
		{

			// Create an OILy bitmap object
			pCWxBitmap = new CWxBitmap;
			if (pCWxBitmap == NULL)
			{
				ERROR3("Problem creating thumbnail bitmap");
				return FALSE;
			}
		
			// files with internal preveiws
			// WEBSTER-Martin-08/01/97
			if(IsRIFFTypeFile)
			{
#ifndef WEBSTER //WEBSTER-Martin-21/01/97
				if ( AccusoftFilters::GetVersionNumber() > 0)
				{
					// search to see if we can find a thumbnail in this file
					if ( CMXImportFilter::SkipToPreviewBitmap(&File) )
					{
						TRACEUSER( "Martin", _T("Accusoft read RIFF preview\n"));
						ReadOK = AccusoftFilters::ReadFromFile( &File, &pCWxBitmap->BMInfo, &pCWxBitmap->BMBytes, TRUE, NULL, NULL, TRUE );
					}
				}
				else
				{
					TRACEUSER( "Martin", _T("Accusoft filter not loaded\n"));
					ReadOK = FALSE;
				}
#endif //WEBSTER
			}
			else if(IsPCDTypeFile)
			{
#ifndef WEBSTER //WEBSTER-Martin-21/01/97
				if ( AccusoftFilters::GetVersionNumber() > 0)
				{
					TRACEUSER( "Martin", _T("Accusoft read PCD preview\n"));
					// Use the tiny preview... (we're a friend of AccusoftFilters by the way)
					AccusoftFilters::ImagePageToRead = 2;
					ReadOK = AccusoftFilters::ReadFromFile( &File, &pCWxBitmap->BMInfo, &pCWxBitmap->BMBytes, TRUE, NULL, NULL, TRUE );
				}
				else
				{
					TRACEUSER( "Martin", _T("Accusoft filter not loaded\n"));
					ReadOK = FALSE;
				}
#endif //WEBSTER
			}
			else // new native file
			{
				// First, check that we have a CX2/XaraX format file, and
				// try to extract its preview bitmap at the same time.
				BOOL IsNewFormat = FALSE;
				UINT32 FilterId = FILTERID_NONE;
				if (BaseCamelotFilter::SkipToPreviewBitmap(&File, &IsNewFormat, &FilterId))
				{
					// Get the preview!
					if (FilterId != FILTERID_NONE)
					{
						Filter* pFilter = Filter::FindFilterFromID(FilterId);
						if (pFilter != NULL)
						{
							ERROR3IF( !pFilter->IS_KIND_OF(BaseBitmapFilter), "Preview filter is not a BaseBitmapFilter\n" );
							ReadOK = ((BaseBitmapFilter *)pFilter)->ReadFromFile( pCWxBitmap, NULL, &File, FALSE );
						}
					}
					
					// Couldn't find a preview bitmap, so we'll make a note to
					// check for an external preview file instead.
					else
					{
						UseActualFile	= FALSE;
						Found			= FALSE;
						File.close();

						delete pCWxBitmap;
						pCWxBitmap = NULL;
					}
				}

				// SkipToPreviewBitmap failed, so we'll assume that it's an old-format file.
				// In that case, we must try to load in its TIFF preview.
				else
				{
					if (AccusoftFilters::GetVersionNumber() > 0)
						ReadOK = AccusoftFilters::ReadFromFile( &File, &pCWxBitmap->BMInfo, &pCWxBitmap->BMBytes, TRUE );
					else
						ReadOK = FALSE;
				}
			}
		}
		else
		{
			TRACEUSER( "Martin", _T("Failed to find original file %s\n"), (const TCHAR *)ActualFile->GetFileName(TRUE));
		 	Error::ClearError();
			// didn't find original so look for external preview
			UseActualFile = FALSE;
		}
	}

	//we test this again cos we can change our mind about UseActualFile
	if(!UseActualFile)
	{

#ifdef THUMB_CHECK_FILENAME_WITH_PNG
		if(CheckFilenamePng && !Found)
		{
			// Check for c:\clipart\sublib\xarainfo\picture.png files (from picture.tif or whatever)
			String_256 FAP;
		
			FAP = Directory->GetPath(TRUE);
			FAP += ActualFile->GetFileName(FALSE);
			FAP += TEXT(".png");

// 			ERROR3_PF(("Looking for: %s", (TCHAR *)FAP));

			PathName ThumbPath(FAP);

			// <Filename>.png
			Found = OpenThumbnailFile( &File, ThumbPath );

			if(Found)
			{
				// Create an OILy bitmap object
				pCWxBitmap = new CWxBitmap;
				if (pCWxBitmap == NULL)
				{
					ERROR3("Problem creating bitmap");
					return FALSE;
				}

				// PNGs
				INT32 TransColour;	// ignore value returned into this
				ReadOK = PNGUtil::ReadFromFile( &File, &pCWxBitmap->BMInfo, &pCWxBitmap->BMBytes, &TransColour );
			}
		}
#endif

#ifdef THUMB_CHECK_FILENAME_WITH_BMP
		if(CheckFilenameBmp && !Found)
		{
			// Check for c:\clipart\sublib\xarainfo\picture.bmp files (from picture.tif or whatever)
			String_256 FAP;
		
			FAP = Directory->GetPath(TRUE);
			FAP += ActualFile->GetFileName(FALSE);
			FAP += TEXT(".bmp");

			PathName ThumbPath(FAP);

			// <Filename>.bmp
			Found = OpenThumbnailFile( &File, ThumbPath );

			if(Found)
			{
				// Create an OILy bitmap object
				pCWxBitmap = new CWxBitmap;
				if (pCWxBitmap == NULL)
				{
					ERROR3("Problem creating bitmap");
					return FALSE;
				}

				// BMPs
				ReadOK = DIBUtil::ReadFromFile( &File, &pCWxBitmap->BMInfo, &pCWxBitmap->BMBytes, TRUE );

				if ( !ReadOK )
				{
					delete pCWxBitmap;
					pCWxBitmap = NULL;
					Found = FALSE;
					Error::ClearError();
		
					// Er, well, it might be monochrome, so use accusoft then...
					// <Filename>.bmp
					File.close();
					Found = OpenThumbnailFile( &File, ThumbPath );
				}
			}
		}
#endif

#ifdef THUMB_CHECK_REVERSED_FILENAME
		if( CheckFilenameReversed && !Found )
		{
			// Check for c:\clipart\sublib\xarainfo\picture.fit file from picture.tif, etc...
			String_256 FAP;
		
			FAP = Directory->GetPath(TRUE);
			FAP += ActualFile->GetFileName(FALSE);
			String_256 Ending(ActualFile->GetType());
			_tcsrev((TCHAR *)Ending);		// reverse the bmp to pmb
			FAP += TEXT(".");
			FAP += (TCHAR *)Ending;

			//ERROR3_PF(("Looking for: %s", (TCHAR *)FAP));

			PathName ThumbPath(FAP);

			// <Filename>.fit <Filename>.pmb etc.
			Found = OpenThumbnailFile( &File, ThumbPath );

			if(Found)
			{
				// Create an OILy bitmap object
				pCWxBitmap = new CWxBitmap;
				if (pCWxBitmap == NULL)
				{
					return FALSE;
				}

				// BMPs
				ReadOK = DIBUtil::ReadFromFile( &File, &pCWxBitmap->BMInfo, &pCWxBitmap->BMBytes, TRUE );

				if ( ReadOK )
				{
					// Set the name to the filename (without the .bmp)	
					String_256 Name(ActualFile->GetFileName(FALSE));
					pCWxBitmap->SetName(Name);
				}
				else
				{
					delete pCWxBitmap;
					pCWxBitmap = NULL;
					Found = FALSE;
					Error::ClearError();

					// Er, well, it might be monochrome, so use accusoft then...
					// <Filename>.pmb
					File.close();
					Found = OpenThumbnailFile( &File, ThumbPath );
				}
			}
		}

#endif

#ifdef THUMB_CHECK_FILENAME_WITH_TIF
		if( CheckFilenameTif && !Found )
		{
			// Check for normal tiff thumbnail file of the form <Filename>.TIF
			// Construct a full pathname for actual filename with a tif extension
			String_256 FullThumbnailPath(Directory->GetPath(TRUE));
			FullThumbnailPath += ActualFile->GetFileName(FALSE);
			FullThumbnailPath += TEXT(".tif");

			TRACEUSER( "Richard", _T("Filename: %s\n"), (TCHAR *)FullThumbnailPath);

			PathName FileAndPath(FullThumbnailPath);

			// <Filename>.tif
			Found = OpenThumbnailFile( &File, FileAndPath );
		}
#endif

		// Work out the encoded thumbnail name for use in multiple places below
		String_256 FullThumbnailPathWithoutExtension;
		if(!Found)
		{
			String_8 EncodedThumbnail;
	
			// Find encoded thumbnail path
			if(!GetThumbnailName(ID, &EncodedThumbnail))
				return FALSE;

			// Construct a full pathname for the encoded thumbnail file
			FullThumbnailPathWithoutExtension =  Directory->GetPath(TRUE);
			FullThumbnailPathWithoutExtension += EncodedThumbnail;
		}


#ifdef THUMB_CHECKBMPS
		if(CheckEncodedBmp && !Found)
		{
			// Check for standard bitmap thumbnail file of the form X00000X.BMP
			// And load it using internal Xara code rather than Accusoft

			// Construct a full pathname for the encoded thumbnail file
			String_256 FullThumbnailPath(FullThumbnailPathWithoutExtension);
			FullThumbnailPath += TEXT(".bmp");
			TRACEUSER( "Richard", _T("Filename: %s\n"), (TCHAR *)FullThumbnailPath);

			PathName FileAndPath(FullThumbnailPath);

			// <Filename>.bmp
			Found = OpenThumbnailFile( &File, FileAndPath );

			if(Found)
			{
				// Create an OILy bitmap object
				pCWxBitmap = new CWxBitmap;
				if (pCWxBitmap == NULL)
				{
					ERROR3("Problem creating bitmap");
					return FALSE;
				}

				// Set the name to the thumbID name (without the .bmp)	
				String_256 Name(FileAndPath.GetFileName(FALSE));
				pCWxBitmap->SetName(Name);

				// BMPs
				ReadOK = DIBUtil::ReadFromFile( &File, &pCWxBitmap->BMInfo, &pCWxBitmap->BMBytes, TRUE );

				if(!ReadOK)
				{
					delete pCWxBitmap;
					pCWxBitmap = NULL;
					Found = FALSE;
					Error::ClearError();

					// Er, well, it might be monochrome, so use accusoft then...
					// <Filename>.bmp
					File.close();
					Found = OpenThumbnailFile( &File, FileAndPath );
				}
			}
		}
#endif

#ifdef THUMB_CHECKPNGS
		if(CheckEncodedPng && !Found)
		{
			// Check for downloaded thumbnail file of the form X00000X.PNG
			// And load it using internal Xara code rather than Accusoft

			// Construct a full pathname for the encoded thumbnail file
			String_256 FullThumbnailPath(FullThumbnailPathWithoutExtension);
			FullThumbnailPath += TEXT(".png");
			TRACEUSER( "Richard", _T("Filename: %s\n"), (TCHAR *)FullThumbnailPath);

			PathName FileAndPath(FullThumbnailPath);

			// <Filename>.png
			Found = OpenThumbnailFile( &File, FileAndPath );

			if(Found)
			{
				// Create an OILy bitmap object
				pCWxBitmap = new CWxBitmap;
				if (pCWxBitmap == NULL)
				{
					ERROR3("Problem creating bitmap");
					return FALSE;
				}

				// Set the name to the thumbID name (without the .png)	
				String_256 Name(FileAndPath.GetFileName(FALSE));
				pCWxBitmap->SetName(Name);

				// PNGs
				INT32 TransColour;	// ignore value returned into this
				ReadOK = PNGUtil::ReadFromFile( &File, &pCWxBitmap->BMInfo, &pCWxBitmap->BMBytes, &TransColour );
			}
		}
#endif

#ifdef THUMB_CHECKTIFS
		if(CheckEncodedTif && !Found)
		{
			// Check for encoded tiff thumbnail file of the form X00000X.TIF

			// Construct a full pathname for the encoded thumbnail file
			String_256 FullThumbnailPath(FullThumbnailPathWithoutExtension);
			FullThumbnailPath += TEXT(".tif");
			TRACEUSER( "Richard", _T("Filename: %s\n"), (TCHAR *)FullThumbnailPath);

			PathName FileAndPath(FullThumbnailPath);

			// X00000X.tif
			Found = OpenThumbnailFile( &File, FileAndPath );
		}
#endif

#ifndef WEBSTER //WEBSTER-Martin-21/01/97
		if ( Found && !ReadOK )
		{
			// this is Accusoft's last chance
			if ( AccusoftFilters::GetVersionNumber() > 0)
			{
				ReadOK = AccusoftFilters::ReadFromFile( &File, &pCWxBitmap->BMInfo, &pCWxBitmap->BMBytes, TRUE, NULL, NULL, TRUE );
			}
			else
			{
				TRACEUSER( "Martin", _T("Accusoft filter not loaded\n"));
				ReadOK = FALSE;
			}
		}
#endif //WEBSTER
	}

	if(!ReadOK || pCWxBitmap == NULL)
	{
		// Problems somewhere above
		TRACEUSER( "Richard", _T("Can't find/load thumbnail"));

		// We don't want error boxes popping up every time we can't find a thumbnail
		Error::ClearError();

		if(pCWxBitmap != NULL)
		{
			delete pCWxBitmap;
			pCWxBitmap = NULL;
		}
		return FALSE;
	}

	// Set the name to the file's name (without the .art .xar etc)	
	String_256 Name(ActualFile->GetFileName(FALSE));
	pCWxBitmap->SetName(Name);

	// create the kernel bitmap, attaching the OILy bitmap to it
	Thumbnails[Slot].buffer = new KernelBitmap(pCWxBitmap, TRUE);
	if (Thumbnails[Slot].buffer == NULL)
	{
		ERROR3("Problem creating kernel bitmap for thumbnail");
		delete pCWxBitmap;
		pCWxBitmap = NULL;
		return FALSE;
	}

	// Set the rest of the entries to sensible values
	Thumbnails[Slot].Valid = TRUE;
	Thumbnails[Slot].ID = ID;
	Thumbnails[Slot].Usage = 0;
	Thumbnails[Slot].Size = ThumbnailsSize;

	Error::ClearError();
#endif
	// Well, looks like we got ourselves a thumbnail...
	return TRUE;
}

/**********************************************************************************************

>	BOOL SGThumbs::OpenThumbnailFile( CCDiskFile* pFile, const PathName& ThumbFileName ) const

	Author:		Martin_Bell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/01/97

	Inputs:		pFile - pointer to the file to try to open
				ThumbFileName - the filename of the file to try

	Returns:	TRUE - if the file exists and was opened successfully
				FALSE otherwise

	Scope:		private

	Purpose:	Check to see if the thumbnail file exists and open the file. The function simply
				checks the filename given which may be a genuine thumbnail or a xara file with a
				preview

***********************************************************************************************/

BOOL SGThumbs::OpenThumbnailFile( CCDiskFile* pFile, const PathName& ThumbFileName ) const
{
	ERROR3IF( pFile==NULL, "SGThumbs::OpenThumbnailFile passed a null ptr" );
	
	TRACEUSER( "Martin", _T("Open thumb file: %s\n"), (const TCHAR *)ThumbFileName.GetPath() );

	BOOL Found = FALSE;

	if(!ThumbFileName.IsValid())
	{
		// either xarainfo\\<filename> or the actual clipart file
		ERROR3("SGThumbs::OpenThumbnailFile received an invalid xarainfo\\<filename> file");
		Error::ClearError();
		return FALSE;
	}

	// Open file and check if it exists at the same time
	if( !( Found = pFile->open( ThumbFileName, ios::in | ios::binary ) ) )
	{
		Found = FALSE;
		Error::ClearError();
	}

	return Found;
}

			    
/**********************************************************************************************

>	BOOL SGThumbs::GetThumbnail(UINT32 ID, BOOL Urgent, KernelBitmap **Result, PathName *File = NULL,
														LP_SGTHUMBS_MOREINFO MoreInfo = NULL)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/2/95
	Inputs:		ID - ID of thumbnail to get
				Urgent - set to TRUE if we definitely want the thumbnail now, setting it
						 to FALSE will only return a bitmap if one's in the cache (This is
						 to support background redraw where only thumbs which can be 
						 retrieved fast will be drawn)
				File - Filename of actual art or font file being referenced (can be null)
				MoreInfo - usually a pointer to an object which will have a member capable of
						 generating a thumbnail if (Directory == NULL). Handy for installed fonts.

	Outputs:	Result - The value pointed to by Result will be filled in with
				a pointer to a created thumbnail. This may be NULL if a thumb could
				not be found/created (if Urgent == FALSE, then this will be NULL if the
				thumb is not cached in memory)

	Purpose:	Get a thumbnail from disk, or hopefully from the cache 

***********************************************************************************************/

BOOL SGThumbs::GetThumbnail(UINT32 ID, BOOL Urgent, KernelBitmap **Result, PathName *File, LP_SGTHUMBS_MOREINFO MoreInfo)
{
	PORTNOTETRACE("dialog","SGThumbs::GetThumbnail - do nothing");
#ifndef EXCLUDE_FROM_XARALX
#ifndef EXCLUDE_GALS
	// File can be NULL, and signifies that we need to generate the thumbnail - fonts
	if(Result == NULL)
	{
		ERROR3("SGThumbs::GetThumbnail - NULL result parameters are illegal");
		return FALSE;
	}																				 
	
	*Result = NULL;

	if(!ThumbnailsInitialised) return FALSE;

	BOOL ok = TRUE;
	UINT32 Slot = 0;

	ok = OffsetInCache(ID, &Slot);

	// Not in cache and not urgent
	if(!ok && !Urgent)
		return FALSE;

	MonotonicTime TempTime;

	if(!ok) 
	{
		ok = FindNewSlotInBuffer(&Slot);
		if(!ok) return FALSE;

		if(Directory == NULL)
		{
			if(MoreInfo == NULL)
			{
				ERROR3("SGThumbs::GetThumbnail MoreInfo is NULL as is Directory. Can't generate the thumbnail");
				return FALSE;
			}

			if(Thumbnails[Slot].Valid && Thumbnails[Slot].buffer)
			{
				delete Thumbnails[Slot].buffer;
				Thumbnails[Slot].buffer=NULL;
			}

			// Just in case we jump out in a state of unhappy rampantness
			Thumbnails[Slot].Valid = FALSE;

			switch(ThumbnailsLibType)
			{
				case SGLib_Font:
					ok = ((SGDisplayPreviewFonts *)MoreInfo)->CreateThumbnail(&Thumbnails[Slot].buffer);					
					break;
					
				default:
					TRACEUSER( "Richard", _T("Cannot create thumbnails of this type"));
					return FALSE;
			}

			if(ok)
			{
				// Set the cache entries so the thing works
				Thumbnails[Slot].Valid = TRUE;
				Thumbnails[Slot].ID = ID;
				Thumbnails[Slot].Usage = 0;
				Thumbnails[Slot].Size = ThumbnailsSize;
			}
		}
		else
		{
			if(File != NULL)
			{
				ok = LoadThumbnailIntoSlot(Slot, ID, File);
				if(!ok)
				{
					TRACEUSER( "Richard", _T("PROBLEMS loading thumbnail"));
					return FALSE;
				}

// WEBSTER-Martin-09/01/97
#ifndef WEBSTER
#ifndef STANDALONE
				if(ThumbnailsLibType == SGLib_Font)
				{
					StringToBitmap::ContoneFontBitmap(Thumbnails[Slot].buffer);
				}
#endif
#endif //WEBSTER
			}
			else
			{
				ERROR3("SGThumbs::GetThumbnail - No filename given for the thumbnail");
				return FALSE;
			}
		}
	}

	// Adds the monotonic time to the cached item
	Thumbnails[Slot].Usage = TempTime.Sample();
	*Result = Thumbnails[Slot].buffer;	
#endif
#endif
	return TRUE;
}

/***********************************************************************************************

>	BOOL SGThumbs::SetSize(SGThumbSize Size)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/2/95
	Inputs:		Size - Size to set... If this is the current size we just return straight away
									  If not, we delete the entire cache and set this to the current
	Purpose:	Set thumbnail size - all returned thumbnails will be this size after this call
				Note that this will also trash the contents of the thumbnail cache
	Notes:

***********************************************************************************************/

BOOL SGThumbs::SetSize(SGThumbSize Size)
{
	if(ThumbnailsSize == Size) return TRUE;
	ThumbnailsSize = Size;
	return(DeleteThumbnails());
}

/***********************************************************************************************

>	BOOL SGThumbs::GetSize(SGThumbSize *Size)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/2/95
	Inputs:		Size - Pointer to a SGThumbSize for returning the current size
	Purpose:	Get current thumbnail size setting
	Notes:

***********************************************************************************************/

BOOL SGThumbs::GetSize(SGThumbSize *Size)
{
	if(Size == NULL)
	{
		ERROR3("SGThumbs::GetSize passed a null size");
		return FALSE;
	}

	*Size = ThumbnailsSize;
	return TRUE;
}

/***********************************************************************************************

>	void SGThumbs::DumpCache(char *DumpFile)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/2/95
	Inputs:		DumpFile - Filename to use for a dumpfile
	Purpose:	Debugging stuff
	Notes:

***********************************************************************************************/
			   
void SGThumbs::DumpCache(char *DumpFile)
{
#if 0
	UINT32 i;
	if(!ThumbnailsInitialised)
	{
		_tprintf("Thumbnails not initialised\n");
		return;
	}
		 
	FILE *fp;
	fp = _tfopen(DumpFile, "w");

	if(!fp)
	{
		_tprintf("Error opening %s\n", DumpFile);
		return;
	}
		  
	_tprintf("Dumping's going OK\n");

	
	_ftprintf(fp, "Directory %s\n", Directory->GetPath());
	_ftprintf(fp, "Initialised %d\n", ThumbnailsInitialised);
	_ftprintf(fp, "Size %d\n", ThumbnailsSize);
	_ftprintf(fp, "LibType %d\n\n", ThumbnailsLibType);

	for(i=0; i<Entries; i++)
	{
		_ftprintf(fp, "Entry %d - Valid %d, ID %d, Usage %d, Size %d, buffer %d\n", i, Thumbnails[i].Valid, Thumbnails[i].ID, Thumbnails[i].Usage, Thumbnails[i].Size, (INT32)Thumbnails[i].buffer);
	}

	fclose(fp);
#endif
}

/***********************************************************************************************

>	static BOOL SGThumbs::ResizeAllThumbCaches(INT32 Entries)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/2/95
	Inputs:		Size - Pointer to a SGThumbSize for returning the current size
	Returns:	FALSE if there was a problem
	Purpose:	To change the number of entries in all the thumbnail caches
	Notes:

***********************************************************************************************/

BOOL SGThumbs::ResizeAllThumbCaches(INT32 Entries)
{
	//TRACE( _T("Dynamic changing of thumbnail cache entries not yet implemented"));
	BROADCAST_TO_CLASS(ThumbMessage(ThumbMessage::CACHESIZECHANGED, Entries), DialogOp);
	return TRUE;
}
