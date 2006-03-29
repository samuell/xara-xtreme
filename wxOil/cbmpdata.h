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

// A dialog box for previwing bitmaps

#ifndef INC_BitmapPreviewData
#define INC_BitmapPreviewData

#include "filtimop.h"	//class ImagemapFilterOptions

class DocColour;
class KernelBitmap;
class PathName;
class BitmapExportOptions;
class BmapPrevDlg;
class Spread;

// define the different ways to specify the background
typedef enum {BROWSER_BGR_NONE, BROWSER_BGR_DOC, BROWSER_BGR_BITMAP, BROWSER_BGR_CHECKER} 
	BrowserBackground;


// class encapsulating the different options for displaying a browser preview of a bitmap
class BrowserPreviewOptions : public CCObject
{
	// Declare the class for memory tracking
	CC_DECLARE_MEMDUMP(BrowserPreviewOptions);

	friend class BitmapPreviewData;
public:
	//constructors
	BrowserPreviewOptions();
	BrowserPreviewOptions(BrowserBackground Bgr, BOOL bInfo, BOOL bImagemap, 
		ImagemapFilterOptions ifoToSet=ImagemapFilterOptions());
	BrowserPreviewOptions(const BrowserPreviewOptions& obj);

	// get/set functions
	void Get(BrowserBackground *pBgr, BOOL *pbInfo, BOOL *pbImagemap, 
		ImagemapFilterOptions* pifoToGet=NULL);

	void Set(BrowserBackground Bgr, BOOL bInfo, BOOL bImagemap,
		ImagemapFilterOptions ifoToSet=ImagemapFilterOptions());

	Spread * GetSpread() { return m_pSpread; }
	void  SetSpread(Spread * pSpread) { m_pSpread = pSpread; }

protected:
	BrowserBackground	m_Background;	// the background
	BOOL				m_bInfo;		// display additional bitmap info
	BOOL				m_bImagemap;	// include the imagemap as well
	Spread *			m_pSpread;		// the spread which the background is defined on 
	ImagemapFilterOptions m_ifoImagemapOptions;	//Graham 25/7/97: Options for the imagemap
};

// a class which just keeps data for each of the two bitmaps being previewed - kernel bitmap, file name, etc.
class BitmapPreviewData : public CCObject
{
	// Give my name in memory dumps
	CC_DECLARE_DYNAMIC(BitmapPreviewData);

friend class BitmapExportPreviewDialog;
friend class BmapPrevDlg;

public:
	BitmapPreviewData();
	~BitmapPreviewData();

	void DeleteBitmapData();

	void SetNewBitmap(KernelBitmap *pKernelBitmap, UINT32 FileSize, BitmapExportOptions *pOptions);

	void GenerateBitmapInfoStrings(String_64 &ImageSize, String_64 &FileSize, BOOL bIncludeBpp = FALSE);
	void CalculateTime(String_64 &timestring,double);
	BOOL GenerateHTMLStub(BrowserPreviewOptions BrowserOptions);

	static BOOL ComposeHTMLColour(DocColour *pColour, String_32 &OutColour);
	static BOOL ExportBrowserTypeBitmap(KernelBitmap *pBmp,PathName *pOutFile);
	static BOOL ExportImagemap(CCDiskFile &DiskFile, PathName *pPath, SelectionType Sel, DPI dpi,
		ImagemapFilterOptions ifoOptionsToUse);
	static BOOL SetBackgroundFromPage(CCDiskFile &DiskFile, Spread * pSpread = NULL);

private:
	KernelBitmap *m_pBitmap;
	UINT32 m_FileSize;
	BitmapExportOptions *m_pOptions;
	PathName *m_pTempHTMLPath;

	BOOL m_bIsSameBitmap; // indicate whether the data should be deleted when replaced
	static PathName *pPagePath; // pointer to the page background bitmap
};


#endif // INC_BitmapPreviewData
