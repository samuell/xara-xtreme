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

// The preview filters. Basically the same as the normal filter but never puts
// a dialog up for the export options - always provides sensible defaults for
// the preview.


#ifndef INC_PREVIEWFILTERS
#define INC_PREVIEWFILTERS

#include "bmpfiltr.h"	// for BMP preview
//#include "giffiltr.h"	// for GIF preview
//#include "exjpeg.h"		// for JPEG preview
#include "pngfiltr.h"	// for, you guessed it.. PNG preview

// Forward definitions.
class AIEPSRenderRegion;

/********************************************************************************************

>	class PreviewFilter : public Filter

	Author:		Martin_Bell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	01/01/97
	Purpose:	The filter's sole purpose is to hold the PreviewBitmapSize static. It can
				not import, export or appear on a menu.
	Notes:		20/01/97
				This class used to be derived from AccusoftFilters (but prob never should 
				have been). All other preview filters were derived from this, but are now
				derived from their respectively similar file filter i.e. PreviewFilterBMP
				is derived from BMPFilter. This causes a problem with FILTERIDs cos
				FILTERID_PREVIEW_TIFF is defined to be the same as FILTERID_PREVIEW obviously
				this won't work now that the filters are not all derived from PreviewFilter

********************************************************************************************/

class PreviewFilter : public Filter
{
	CC_DECLARE_DYNAMIC(PreviewFilter);
	
public:
	PreviewFilter();
	BOOL Init();

	// we will always be incompatible
	virtual INT32 HowCompatible( PathName&, ADDR, UINT32, UINT32 );

	// we will never import
	virtual BOOL DoImport(SelOperation*, CCLexFile*, Document*, 
						  BOOL AutoChosen = FALSE, ImportPosition *Pos = NULL,
						   KernelBitmap** ppImportedBitmap = NULL, 
						   DocCoord* pPosTranslate = NULL, String_256* = NULL);

	// we will never export
	virtual BOOL DoExport(Operation*, CCLexFile*, PathName*, Document*, BOOL);

public:
	// The Size of the Preview Bitmap Preference
	static MILLIPOINT PreviewBitmapSize;

};

/********************************************************************************************

>	class PreviewFilterBMP : public BMPFilter

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/6/95
	Purpose:	The filter will save out the BMP Preview Bitmap. Overrides GetExportOptions
				so dialogs are not put up but returns preview export options.

********************************************************************************************/

class PreviewFilterBMP : public BMPFilter
{
	CC_DECLARE_DYNAMIC(PreviewFilterBMP);
	
public:
	PreviewFilterBMP();
	BOOL Init();
	virtual BOOL DoExport(Operation*, CCLexFile*, PathName*, Document*, BOOL);

protected:
	virtual BOOL GetExportOptions(BitmapExportOptions* pOptions);
};

PORTNOTE ("filters", "Removed GIF & JPEG preview filters")
#ifndef EXCLUDE_FROM_XARALX
/********************************************************************************************

>	class PreviewFilterGIF : public TI_GIFFilter

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/6/95
	Purpose:	The filter will save out the GIF Preview Bitmap. Overrides GetExportOptions
				so dialogs are not put up but returns preview export options.

********************************************************************************************/

class PreviewFilterGIF : public TI_GIFFilter
{
	CC_DECLARE_DYNAMIC(PreviewFilterGIF);
	
public:
	PreviewFilterGIF();
	BOOL Init();
	virtual BOOL DoExport(Operation*, CCLexFile*, PathName*, Document*, BOOL);

protected:
	virtual BOOL GetExportOptions(BitmapExportOptions* pOptions);
};

/********************************************************************************************

>	class PreviewFilterJPEG : public JPEGExportFilter

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/6/95
	Purpose:	The filter will save out the JPEG Preview Bitmap. Overrides GetExportOptions
				so dialogs are not put up but returns preview export options.
	Note:		22/01/97	Preview filters can generally import as well now (if their base 
				class can) but obviously this one can't - use JPEGImportFilter instead.

********************************************************************************************/

class PreviewFilterJPEG : public JPEGExportFilter
{
	CC_DECLARE_MEMDUMP(PreviewFilterJPEG);
	
public:
	PreviewFilterJPEG();
	BOOL Init();
	virtual BOOL DoExport(Operation*, CCLexFile*, PathName*, Document*, BOOL);

protected:
	virtual BOOL GetExportOptions(BitmapExportOptions* pOptions);
};
#endif

/********************************************************************************************

>	class PreviewFilterPNG : public PNGFilter

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/6/95
	Purpose:	The filter will save out the PNG Preview Bitmap. Overrides GetExportOptions
				so dialogs are not put up but returns preview export options.

********************************************************************************************/

class PreviewFilterPNG : public PNGFilter
{
	CC_DECLARE_DYNAMIC(PreviewFilterPNG);
	
public:
	PreviewFilterPNG();
	BOOL Init();
	virtual BOOL DoExport(Operation*, CCLexFile*, PathName*, Document*, BOOL);

protected:
	virtual BOOL GetExportOptions(BitmapExportOptions* pOptions);
};

/********************************************************************************************

>	class ThumbnailFilterPNG : public PreviewFilterPNG

	Author:		Stefan_Stoykov (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/6/97
	Purpose:	The filter will save out the PNG Thumbnail Bitmap. Overrides DoExport
				so dialogs are not put up but returns preview export options.

********************************************************************************************/

class ThumbnailFilterPNG : public PreviewFilterPNG
{
	CC_DECLARE_DYNAMIC(ThumbnailFilterPNG);
	
public:
	ThumbnailFilterPNG() : PreviewFilterPNG() {};

	virtual BOOL DoExport(Operation* pOp, CCLexFile* pFile, PathName* pPath, 
								Document* TheDocument, BOOL ShowOptions);
};

#endif  // INC_PREVIEWFILTERS
