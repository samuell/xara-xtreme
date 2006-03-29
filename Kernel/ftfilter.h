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
// The Flare Template format

#ifndef INC_FTFILTER
#define INC_FTFILTER

#include "webfiltr.h"	// 	CamelotWebFilter

/********************************************************************************************

>	class FlareTemplateFilter : public CamelotWebFilter

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/07/97
	Purpose:	A filter for the new flare template format (text based form of the the new
				version 2 native file filter.

				The CamelotWebFilter provides most of the implementation.

********************************************************************************************/

class FlareTemplateFilter : public CamelotWebFilter
{
	// Give my name in memory dumps
	CC_DECLARE_DYNAMIC(FlareTemplateFilter);

public:
	// Construction and initialisation
	FlareTemplateFilter();
	~FlareTemplateFilter();
	BOOL Init();

	virtual BOOL IsWebFilter()
		{ return BaseCamelotFilter::GetMinimalWebFormat(); } // FALSE by default
	virtual BOOL IsCompactNativeFilter()
		{ return !BaseCamelotFilter::GetMinimalWebFormat(); } // TRUE by default

	virtual INT32 HowCompatible(PathName& Filename, ADDR HeaderStart, UINT32 HeaderSize,
							   UINT32 FileSize);

protected:
	// Override this to not bother with any of that filesize rubbish
//	virtual BaseCamelotFilter* CreateNULLFilter(Document* pDocument) { return(NULL); };

	virtual CXaraFile* CreateCXaraFile();

	// We don't want to change the file length... (just yet 8-)
	virtual BOOL CorrectFileHeader(CCLexFile* pFile) { return(TRUE); };

	virtual BOOL SetTotalProgressBarCount(UINT32 n);

	// Resource IDs of the strings that describe this filter.
	UINT32 FilterNameID;
	UINT32 FilterInfoID;

	// The string to display when actually exporting rather than rendering.
	// 0 if we're not an export filter.
	// ExportMsgID is used in ExportRender when we are rendering the bitmap
	UINT32 ExportingMsgID;

protected:
	// Preferences for Flare template filter (separate from the base class)
	static INT32 FTBitmapCompression;
	static BOOL FTPreviewBitmapExport;
	static BOOL FTConvertTextToOutlines;
	static BOOL FTHTMLToClipboard;
	static BOOL FTConvertBlendsToOutlines;

public:
	// Find out how much compression is required for bitmaps
	virtual INT32 GetBitmapCompression(); 
	virtual INT32 SetBitmapCompression(INT32 NewBmpComp);

	// Find out whether the user has requested a preview bitmap
	virtual BOOL GetPreviewBitmapExport(); 
	virtual BOOL SetPreviewBitmapExport(BOOL NewExportPreviewBmp);

	// Find out whether the user has requested convert text to outlines
	virtual BOOL GetConvertTextToOutlines(); 
	virtual BOOL SetConvertTextToOutlines(BOOL NewConvertTextToOutlines);
	
	// WEBSTER - markn 28/1/97
	// Find out whether the user has requested convert blends to outlines
	virtual BOOL GetConvertBlendsToOutlines(); 
	virtual BOOL SetConvertBlendsToOutlines(BOOL NewConvertBlendsToOutlines);
	
	// Find out whether the user has requested remove invisible layers
	virtual BOOL GetRemoveInvisibleLayers(); 
	virtual BOOL SetRemoveInvisibleLayers(BOOL NewRemoveInvisibleLayers);

	// Find out whether the user has requested to remove unused colours
	virtual BOOL GetRemoveUnusedColours(); 
	virtual BOOL SetRemoveUnusedColours(BOOL NewRemoveUnusedColours);

	// Find out whether the user has requested to remove unused colours
	virtual BOOL GetMinimalWebFormat(); 
	virtual BOOL SetMinimalWebFormat(BOOL NewMinimalWebFormat);

	// Find out whether the user wants an HTML tag put on the clipboard
	virtual BOOL ShouldExportHTMLTag();
	virtual BOOL GetHTMLToClipboard();
	virtual BOOL SetHTMLToClipboard(BOOL NewHTMLToClipboard);
};

#endif  // INC_FTFILTER
