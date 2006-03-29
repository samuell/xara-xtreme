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
// Super Gallery Library DisplayItem stuff

#ifndef INC_SGLIB
#define INC_SGLIB

#include "bitmap.h"
#include "sgallery.h"
#include "sgtree.h"
#include "sgscan.h"
//#include "camnet.h"

/************************************************************************************/

// The various valid display modes. Note that FullInfo defaults to using the medium
// thumbnail. 
enum LibDisplayType
{
	LibDisplay_Default = 0,
	LibDisplay_SmallThumb,
	LibDisplay_MediumThumb,
	LibDisplay_LargeThumb,
	LibDisplay_SmallThumbText,
	LibDisplay_MediumThumbText,
	LibDisplay_LargeThumbText,
	LibDisplay_SmallThumbTextUnder,
	LibDisplay_MediumThumbTextUnder,
	LibDisplay_LargeThumbTextUnder,
	LibDisplay_FullInfo,
	LibDisplay_JustText,
	LibDisplay_SingleLineFullInfo
};
	
/***********************************************************************************************

>	class SGLibDisplayItem : public SGDisplayItem

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/01/95
	Purpose:	This DisplayTree node is used by Library SuperGalleries
				It is responsible for providing the ability to handle/redraw one Library item
				displayed in said gallery.
				It is a base class to be used by all library display items, though many
				gallery types may nmot need to derive a new class from it - it's generic
				handling of display may be adequate.

	SeeAlso:	LibClipartSGallery; SuperGallery; SGDisplayItem

***********************************************************************************************/

class OpThumbDownload;

class CCAPI SGLibDisplayItem : public SGDisplayItem
{
	CC_DECLARE_DYNAMIC(SGLibDisplayItem);

public:
	SGLibDisplayItem();
	SGLibDisplayItem(LibraryIndex LibraryIndexToDisplay, BOOL bIsNew = FALSE);
	~SGLibDisplayItem();

protected:
	// Event handler
	virtual	BOOL HandleEvent(SGEventType EventType, void *EventInfo, SGMiscInfo *MiscInfo);

	// Shared code to use before starting an item drag (sets drag bmp up, and returns size)
	virtual BOOL StartDrag(SGEventType EventType, void *EventInfo, SGMiscInfo *MiscInfo,
		INT32 *XSize, INT32 *YSize);

	// Determines the width of the text to display for this item
	virtual INT32 GetTextWidth(SGFormatInfo *FormatInfo, SGMiscInfo *MiscInfo);

	// Determines item size for the current DisplayMode and calculates FormatRect
	virtual void CalculateMyRect(SGFormatInfo *FormatInfo, SGMiscInfo *MiscInfo);

	// Redraws the item into the current FormatRect
	virtual void HandleRedraw(SGRedrawInfo *RedrawInfo, SGMiscInfo *MiscInfo);


	// Specialisations of this class

	// Draw a thumbnail for this item
	virtual BOOL DrawThumb(RenderRegion *Renderer, SGRedrawInfo *RedrawInfo,
			SGMiscInfo *MiscInfo, DocRect *Rectangle, BOOL Background = FALSE);

public:
	// Draw a null bitmap rectangle if we couldn't draw a proper one
	static void DrawNullBitmapRect(RenderRegion *Renderer,
			SGMiscInfo *MiscInfo, DocRect *Rectangle, BOOL Background);

	virtual void DragWasReallyAClick(SGMouseInfo *Mouse, SGMiscInfo *MiscInfo);

protected:

	// Draw the text for this item
	virtual void DrawItemText(RenderRegion *Renderer, SGRedrawInfo *RedrawInfo,
			SGMiscInfo *MiscInfo, DocRect *Rectangle, DocRect *BmpRect, BOOL Selected);

	virtual void DrawPlaceholder(RenderRegion *Renderer, SGMiscInfo *MiscInfo, DocRect *Rectangle, BOOL Background);

	virtual void DrawItemThumbnail(SGRedrawInfo* pRedrawInfo, SGMiscInfo* pMiscInfo, DocRect* pRect);
	
	// return the display type to use - this should really be overridden
	virtual LibDisplayType GetDisplayType(SGMiscInfo *MiscInfo);

	// generic searching and redraw function (which works properly !)
	virtual void GetFullInfoText(String_256 *Result);

	// generic searching and redraw function
	virtual void GetFullInfoText(String_256 *Result, INT32 Line = 0);

	// generic searching function
	virtual void GetKeyWords(String_256 *Result);

	// for sorting
	virtual INT32 CompareTo(SGDisplayNode *Other, INT32 SortKey);


public:

	// Find the LibraryIndex which indexes the item we are displaying
	inline LibraryIndex GetDisplayedLibraryIndex(void);

	// Returns size, scale, aspectm, translation details of a thumbnail and a rectangle
	BOOL GetThumbSize(SGMiscInfo *MiscInfo, DocRect *Rectangle,
		KernelBitmap *Bitmap, INT32 *XSize, INT32 *YSize, double *Scale = NULL,
		INT32 *XTrans = NULL, INT32 *YTrans = NULL, BOOL *Outline = NULL);

	// Finds the parent SGSubLib library (by asking our parent group)
	Library *GetParentLibrary(void);

	// generic searching and redraw function
	virtual void GetFileName(String_256 *Result);

	// Get item's thumb filename (WEBSTER)
	virtual BOOL GetThumbFileName(String_256* path);

	virtual BOOL ShouldDownloadThumb();

	virtual BOOL DownloadThumbnail();

	virtual void OnThumbDownloadComplete();

	virtual void OnThumbDownloadProgress(const INT32 nPercentageCompleted);

	virtual BOOL ShouldIDrawForeground(BOOL ForceForeground);

	inline BOOL IsDownloadingThumb() { return bIsDownloadingThumb;}

	// generic searching and redraw function
	virtual BOOL GetFileNamePtr(TCHAR **Result);

	// generic searching and redraw function
	virtual void GetNameText(String_256 *Result);

	// Return a pointer to the filename for this item (wont work with diskfile indexes)
	virtual BOOL GetNameTextPtr(TCHAR **Result);
	
	// Finds the text description for the displayed item
	virtual BOOL GetDisplayedTextDescription(String_256 *Result);

	// Returns pointer to text description (only if index cached)
	virtual BOOL GetDisplayedTextDescription(TCHAR **Result);

	// Return the actual text which will be displayed for this item in the current mode
	// New Line default can be used for multi-line full info strings. The first line is
	// always 1, and if no info can be found for the line, we return FALSE.
	// 0 is a special value for Line which returns all the text, as the default used to
	// do.
	virtual BOOL GetActualDisplayedText(String_256 *String, INT32 Line = 0);

	// Return a pointer to the actual text which will be displayed for this item
	virtual BOOL GetActualDisplayedTextPtr(TCHAR **String, INT32 Line = 0);

	// Get the item's full filename
	BOOL GetFileName(PathName *Result);

	// Finds the thumbnail bitmap (if any) for the displayed item
	KernelBitmap *GetDisplayedKernelBitmap(SGMiscInfo *MiscInfo, BOOL Background = FALSE);

	// Get the item's file size (Lib can be null, but it will be quicker if it's not)
	INT32 GetFileSize(Library *Lib = NULL);

	// Bubble help for library item
	virtual BOOL GetBubbleHelp(DocCoord *MousePos, String_256 *Result);

	// Status line help for library item
	virtual BOOL GetStatusLineHelp(DocCoord *MousePos, String_256 *Result);
														 										
protected:
	// Special member variables
	BOOL bIsDownloadingThumb;
	INT32 nPercentageDownloaded;
	BOOL bIsNew;
	OpThumbDownload* pDownloadOp;
	LibraryIndex TheLibraryIndex;

	friend class OpThumbDownload;
};



/********************************************************************************************

>	inline LibraryIndex SGLibDisplayItem::GetDisplayedLibraryIndex(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com> (Based on template code by Jason)
	Created:	27/1/95

	Returns:	A pointer to the LibraryIndex which this Display Item is used to display.
			
	Purpose:	To find out the LibraryIndex this object is responsible for displaying

********************************************************************************************/

LibraryIndex SGLibDisplayItem::GetDisplayedLibraryIndex(void)
{
	return(TheLibraryIndex);
}

/***********************************************************************************************

>	class SGLibGroup : public SGDisplayGroup

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/4/95
	Purpose:	This is a display tree group specifically for library galleries
				The main reason for implementing it was to receive library group messages
	SeeAlso:	SuperGallery; SGDisplayGroup;

***********************************************************************************************/

class CCAPI SGLibGroup : public SGDisplayGroup
{
	CC_DECLARE_DYNAMIC(SGLibGroup);
public:
	SGLibGroup(SuperGallery *ParentGal, Document *ParentDoc, Library *ParentLib);
	virtual void ReadGroupTitle(void);
	virtual BOOL HandleEvent(SGEventType EventType, void *EventInfo, SGMiscInfo *MiscInfo);

	// Bubble help for library groups
	virtual BOOL GetBubbleHelp(DocCoord *MousePos, String_256 *Result);

	// Status line help for library groups
	virtual BOOL GetStatusLineHelp(DocCoord *MousePos, String_256 *Result);

	// Can this group virtualise out to disk ?
	virtual BOOL CanVirtualise(void);

	// Virtualise back in
	virtual BOOL DeVirtualise(void);

public:
	// ini file option for libraries
	static BOOL LibraryVirtualisingEnabled;

};
			

#endif  // INC_SGLIB
