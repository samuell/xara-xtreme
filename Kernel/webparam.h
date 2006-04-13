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
#ifndef _INC_WEB_PARAM_H
#define _INC_WEB_PARAM_H

#include "bmpprefs.h"

/********************************************************************************************

>	class WebPrefsDlgParam : public OpParam

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>	
	Created:	1/5/96
	Purpose:	Allows data to be passed into the export web preference dialog box and 
				then returned to the caller.
	
********************************************************************************************/

class WebPrefsDlgParam : public OpParam
{

CC_DECLARE_MEMDUMP(WebPrefsDlgParam);

public:
    // Default constructor, set up some useful defaults
	WebPrefsDlgParam();
	
// WEBSTER - markn 28/1/97
// Added access functions, and the Convert blends to outlines flag

	void SetBmpCompression(INT32 BmpComp)		{ BmpCompression = BmpComp; };		// Bitmap compression flag 
	void SetCompression(BOOL Comp)			{ Compression = Comp; };			// File compression flag 
	void SetExportPreviewBitmap(BOOL b)		{ ExportPreviewBitmap = b; };		// Export preview bitmap
	void SetConvertTextToOutlines(BOOL b)	{ ConvertTextToOutlines = b; };		// Convert text to outlines
	void SetConvertBlendsToOutlines(BOOL b) { ConvertBlendsToOutlines = b; };	// Convert blends to outlines
	void SetRemoveInvisibleLayers(BOOL b)	{ RemoveInvisibleLayers = b; };		// whether invisible layers should be removed or not
	void SetRemoveUnusedColours(BOOL b)		{ RemoveUnusedColours = b; };		// whether unused colours should be removed or not
	void SetMinimalWebFormat(BOOL b)		{ MinimalWebFormat = b; };			// whether we use our minimal web format or not
	void SetExportSel(SelectionType Type)	{ ExportSel = Type; };				// Export spread, drawing or selection 
	void SetViewportSel(SelectionType Type)	{ ViewportSel = Type; };			// Viewport as spread, drawing or selection 
	void SetHTMLToClipboard(BOOL Ok)		{ HTMLToClipboard = Ok; };			// whether an HTML tag should be exported to the clipboard
	void SetWebOk(BOOL Ok)					{ WebOk = Ok; };					// Ok or cancel selected 
	

	INT32				GetBmpCompression()				{ return BmpCompression; };				// Bitmap compression flag 
	BOOL			GetCompression()				{ return Compression; };				// File compression flag 
	BOOL			GetExportPreviewBitmap()		{ return ExportPreviewBitmap; };		// Export preview bitmap
	BOOL			GetConvertTextToOutlines()		{ return ConvertTextToOutlines; };		// Convert text to outlines
	BOOL			GetConvertBlendsToOutlines()	{ return ConvertBlendsToOutlines; };	// Convert blends to outlines
	BOOL			GetRemoveInvisibleLayers()		{ return RemoveInvisibleLayers; };		// whether invisible layers should be removed or not
	BOOL			GetRemoveUnusedColours()		{ return RemoveUnusedColours; };		// whether unused colours should be removed or not
	BOOL			GetMinimalWebFormat()			{ return MinimalWebFormat; };			// whether we use our minimal web format or not
	SelectionType	GetExportSel()					{ return ExportSel; };					// Export spread, drawing or selection 
	SelectionType	GetViewportSel()				{ return ViewportSel; };				// Viewport spread, drawing or selection 
	BOOL			GetHTMLToClipboard()			{ return HTMLToClipboard; };			// Whether an HTML tag should be exported to the clipboard
	BOOL			GetWebOk()						{ return WebOk; };						// Ok or cancel selected 
	
private:
	// Parameters that the dialog can set
	INT32 BmpCompression;				// Bitmap compression flag 
	BOOL Compression;				// File compression flag 
	BOOL ExportPreviewBitmap;		// Export preview bitmap
	BOOL ConvertTextToOutlines;		// Convert text to outlines
	BOOL ConvertBlendsToOutlines;	// Convert blends to outlines
	BOOL RemoveInvisibleLayers;		// whether invisible layers should be removed or not
	BOOL RemoveUnusedColours;		// whether unused colours should be removed or not
	BOOL MinimalWebFormat;			// whether we use our minimal web format or not
	BOOL HTMLToClipboard;			// whether we should export an HTML tag to the clipboard
	
	SelectionType ExportSel;		// Export spread, drawing or selection 
	SelectionType ViewportSel;		// Export spread, drawing or selection 

	BOOL WebOk;						// Ok or cancel selected 
};


#endif // _INC_WEB_PARAM_H
