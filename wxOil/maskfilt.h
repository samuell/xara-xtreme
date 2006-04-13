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
//

#ifndef INC_MASKEDFILTER
#define INC_MASKEDFILTER

#include "bitfilt.h"
//#include "bmpprefs.h"  	// SelectionType
#include "exphint.h"

class CXaraFileRecord;
class KernelBitmap;
class OutputDIB;

/********************************************************************************************

>	class MaskedFilterExportOptions : public BitmapExportOptions

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/10/96
	Purpose:   	Base class for storing information about the bitmap we're exporting.

********************************************************************************************/

class MaskedFilterExportOptions : public BitmapExportOptions
{
	CC_DECLARE_DYNAMIC(MaskedFilterExportOptions)

public:

	static BOOL Declare();

	MaskedFilterExportOptions();
    MaskedFilterExportOptions(const CDlgResID& Dialog, 
							  const FilterType FilterID, const StringBase* pFilterName);

	virtual BOOL	RetrieveDefaults();
	virtual BOOL	SetAsDefaults() const;

	// Save and load from named export set properties.
	virtual BOOL Write(CXaraFileRecord* pRec);
	virtual BOOL Read(CXaraFileRecord* pRec);

	virtual DITHER	GetDither() const { return m_Dither; }
	virtual BOOL	SetDither(const DITHER& Dither);

	BOOL	WantTransparent() const;
	BOOL	WantInterlaced() const { return m_bInterlaced; }
	BOOL	SetMakeInterlaced(BOOL bInterlaced);

	void	SetFilterType(FilterType ID)	{	m_FilterID = ID;	}

// WEBSTER - markn 21/1/97
	virtual BOOL UseSpecificNumColsInPalette();

	// for object copying and comparing
	virtual BOOL CopyFrom(BitmapExportOptions *pOther);

	//  Same as above, except it is used to copy information when changing file types
	virtual BOOL FileTypeChangeCopyFrom(BitmapExportOptions *pOther);
//	virtual BOOL Equal(BitmapExportOptions *pOther);

	// needed for the make bitmap copy options preview dialog
	BOOL CopyFromMasked(MaskedFilterExportOptions *pOther);

protected:

	// Parameters that the dialog can set
	DITHER	m_Dither;					// Dither type
	BOOL	m_bInterlaced;

	// Persistant buffer
	static DITHER	g_Dither;
	static PALETTE	g_Palette;
	static BOOL		g_bTransparent;
	static BOOL		g_bInterlaced;

};

/********************************************************************************************

>	class MaskedFilter : public BitmapFilter

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/4/96
	Purpose:	Encapsulates a masked filter which provides funcitonality for doing masked
				bitmap export and import.

********************************************************************************************/

class MaskedFilter : public BaseBitmapFilter
{
	CC_DECLARE_DYNAMIC(MaskedFilter);
	
public:

	MaskedFilter();
	BOOL Init();

	INT32	SetTransColour(INT32 NewColour);
	INT32	GetTransColour() { return TransColour; }

	// SMFIX - Sets All the appropriate colour to being Transparent!
	BOOL ApplyTransparentColoursToBitmap(KernelBitmap* pExportBitmap = NULL, BitmapExportOptions* pExportOptions = NULL);
	BOOL ApplyTransparentColoursToBitmap(BITMAPINFOHEADER* pInfoHeader = NULL, BYTE* pBMPBits = NULL,
										 BitmapExportOptions* pExportOptions = NULL, UINT32 YOffset = 0,
										 UINT32 StripHeight = 0);

	// Virtual overrides
	virtual UINT32 GetNumReservedColours();
	virtual BOOL ExportSelectionOnly(BOOL MaskedRender = FALSE); 

	virtual BOOL CheckSingleBitmapsOnSpread ( Spread	*pSpread,
											  BOOL		*pSamePalettes,
											  BOOL		*pOurBrowserPalette,
											  BOOL		*pAllHaveBitmaps,
											  BOOL		*pAllWithinCount,
											  UINT32		*pPaletteEntries = NULL );

protected:

	// Export a bitmap.
	virtual BOOL WriteToFile ( BOOL End );

	// Get a pointer to the OutputDIB.
	virtual OutputDIB* GetOutputDIB(void) { return NULL; }

	// Base Class version - See PNG Filter
	virtual void InvertAlpha(LPBITMAPINFO lpBitmapInfo, LPBYTE lpBits) { return; }

	INT32 FindUnusedColour(LPBITMAPINFO pBMInfo, LPBYTE pBMBits);

	UINT32 Compression;			// Compression to use (JPEG = %, others = TRUE, FALSE)

	BOOL SecondPass;			// flag for whether we are rendering second pass on export
	BOOL DoingMask;				// flag for whether we are rendering the mask or not

	// Storage for the 8bpp mask that is made up in the first pass rendering on export
	LPBITMAPINFO pDestBMInfo;	
	LPBYTE pDestBMBytes;
	BYTE* pTempBitmapMask;				// WhiteArea mask generated during 1st pass render
	UINT32 CurrentScanline;				// Current scanline in the WhiteArea mask.

	INT32 TransColour;					// -1 == none, or the transparent colour.

	virtual BOOL SetExportHint(Document* pDoc);
	
	// Override this to provide the correct hint type
	virtual UINT32 GetHintType(void) { return(HINTTYPE_BAD); }

	INT32 m_BandNumber;	//  Tells you which band of the bitmap we are working on.
	//  Tells you if the user made a selection before opening the export dialog.
	BOOL m_bSelection;	
};

#endif // INC_MASKEDFILTER


