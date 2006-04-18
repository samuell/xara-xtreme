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

// Gallery library handling classes

#include "camtypes.h"
#include "sglib.h"
#include <io.h>

#include "ccdc.h"		// For render-into-dialogue support
#include "fillval.h"
#include "grnddib.h"
#include "bitmap.h"
#include "bitmpinf.h"
#include "nodebmp.h"
#include "sgscan.h"
#include "sgliboil.h"
#include "thumb.h"
#include "thumbmsg.h"
#include "errors.h"
//#include "richard.h"
//#include "richard2.h"
//#include "richard3.h"
#include "sglbase.h"
#include "progress.h"
#include "sglcart.h"
#include "inetop.h"
//#include "webster.h"
//#include "simon.h"
//#include "resource.h"

// Implement the dynamic class bits...
CC_IMPLEMENT_DYNAMIC(SGLibDisplayItem, SGDisplayItem)
CC_IMPLEMENT_DYNAMIC(SGLibGroup, SGDisplayGroup)

// This line mustn't go before any CC_IMPLEMENT_... macros
#define new CAM_DEBUG_NEW

using namespace InetUtils;

// Blank space below text when in 'text underneath' modes
const INT32 SG_SPACE_UNDER = SG_GapAboveText / 2;

// Bodge for status line text
#define GROUP_BAR_ICON_WIDTH 16500

// Should we use the virtualising system if we can ?
BOOL SGLibGroup::LibraryVirtualisingEnabled = TRUE;

//#if (USERNUMBER==22)
//#ifdef _DEBUG
// If Debug, the default is YES
//BOOL SGLibGroup::LibraryVirtualisingEnabled = TRUE;
//#else
// Otherwise it's NO
//BOOL SGLibGroup::LibraryVirtualisingEnabled = FALSE;
//#endif

/***********************************************************************************************

>	SGLibDisplayItem::SGLibDisplayItem()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/1/95 (base generated in sgbase.cpp)

	Purpose:	SGLibDisplayItem constructor
				DON'T call this constructor. It ERROR3's. Call the other constructor

***********************************************************************************************/

SGLibDisplayItem::SGLibDisplayItem()
{
	ERROR3("Illegal call on default SGLibDisplayItem constructor - call the other one!");
	TheLibraryIndex = NULL;

}



/***********************************************************************************************

>	SGLibDisplayItem::SGLibDisplayItem(LibraryIndex LibraryIndexToDisplay)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/1/95 (base generated in sgbase.cpp)

	Inputs:		LibraryIndexToDisplay - A LibraryIndex indicating which library item
				this DisplayItem is displaying

	Purpose:	SGLibDisplayItem constructor

***********************************************************************************************/

SGLibDisplayItem::SGLibDisplayItem(LibraryIndex LibraryIndexToDisplay, BOOL bNew)
{
	TheLibraryIndex = LibraryIndexToDisplay;
	bIsDownloadingThumb = FALSE;
	pDownloadOp = NULL;
	nPercentageDownloaded = 0;
	bIsNew = bNew;
}


/***********************************************************************************************

>	SGLibDisplayItem::~SGLibDisplayItem()

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/1/97 

	Purpose:	SGLibDisplayItem destructor

***********************************************************************************************/

SGLibDisplayItem::~SGLibDisplayItem()
{
	if (IsDownloadingThumb() && pDownloadOp)
		pDownloadOp->Abort();
}


/***********************************************************************************************

>	virtual INT32 SGLibDisplayItem::GetTextWidth(SGFormatInfo *FormatInfo,
												SGMiscInfo *MiscInfo)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/3/95

	Inputs:		FormatInfo - The formatting info from which to calculate my position/size
				MiscInfo - As usual, the useful misc info struct
	Outputs:	Width (in millipoints) required by the description text for this mode

	Purpose:	An overridable way of returning the width of the text description
	
***********************************************************************************************/

INT32 SGLibDisplayItem::GetTextWidth(SGFormatInfo *FormatInfo, SGMiscInfo *MiscInfo)
{
	INT32 XSize = SG_InfiniteWidth;
	INT32 OnePixel  = (INT32) DevicePixels(MiscInfo, 1);

	// If we're displaying text underneath the thumbnail, add some space for it
	LibDisplayType DType = GetDisplayType(MiscInfo);

	// Extra space required by the text
	switch(DType)
	{
		case LibDisplay_SmallThumbTextUnder:
		case LibDisplay_MediumThumbTextUnder:
		case LibDisplay_LargeThumbTextUnder:
			// Text is underneath
			XSize = 0;
			break;			

		case LibDisplay_SmallThumb:
		case LibDisplay_MediumThumb:
		case LibDisplay_LargeThumb:
			// No text, so no space
			XSize = 0;
			break;

		case LibDisplay_FullInfo:
		case LibDisplay_SingleLineFullInfo:
//			XSize = SG_InfiniteWidth;
			XSize = SG_DefaultNameText * 2;
			break;

		case LibDisplay_JustText:
			// Text only
			XSize = (SG_DefaultNameText * 4) / 3;
			break;

		default:
			// Text is to the right
			XSize = (SG_DefaultNameText * 2) / 3;
			break;
	}

	// Lock the current sizes to the grid
	XSize = GridLock(MiscInfo, XSize);

	// Extra Space required if selected
	XSize += (3 * 2 * OnePixel);

	return XSize;
}

/***********************************************************************************************

>	virtual void SGLibDisplayItem::CalculateMyRect(SGFormatInfo *FormatInfo,
												SGMiscInfo *MiscInfo)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/3/95 (base generated in sgbase.cpp)

	Inputs:		FormatInfo - The formatting info from which to calculate my position/size
				MiscInfo - As usual, the useful misc info struct

	Outputs:	member variable FormatRect - is returned filled in with the size/position of
				this LibraryIndex item's display area. This is dependent upon the current display
				mode and format state

				FormatInfo will be updated as a result of the formatting operation

	Purpose:	Shared code for LibraryIndex items to calculate where they will appear in the
				grand scheme of things

	Notes:		LibraryIndexs supply only one display mode ("full info")
																	  
	Scope:		private (for use of SGLibDisplayItem class only)

***********************************************************************************************/

void SGLibDisplayItem::CalculateMyRect(SGFormatInfo *FormatInfo, SGMiscInfo *MiscInfo)
{
	INT32 XSize = SG_InfiniteWidth;
	INT32 YSize = SG_DefaultLargeIcon;
	INT32 OnePixel  = (INT32) DevicePixels(MiscInfo, 1);
	Library *ParentLib = GetParentLibrary();

	// If we're displaying text underneath the thumbnail, add some space for it
	LibDisplayType DType = GetDisplayType(MiscInfo);

	// Get width of text
	XSize = GetTextWidth(FormatInfo, MiscInfo);

	// Extra space required by the text
	switch(DType)
	{
		case LibDisplay_SmallThumbTextUnder:
		case LibDisplay_MediumThumbTextUnder:
		case LibDisplay_LargeThumbTextUnder:
			// Text is underneath
			YSize = SG_GapAboveText + SG_SPACE_UNDER;
			break;			

		default:
			// Text is to the right
			YSize = 0;
			break;
	}

	// Lock the current sizes to the grid
	YSize = GridLock(MiscInfo, YSize);

	// Extra Space required if selected
	YSize += (3 * 2 * OnePixel);

	// Add in space required by the bitmap itself
	switch(DType)
	{
		case LibDisplay_SmallThumbTextUnder:
		case LibDisplay_SmallThumbText:
		case LibDisplay_SmallThumb:
			YSize += GridLock(MiscInfo, ParentLib->PreviewBMPHeight(SGThumb_Small) * OnePixel);
			XSize += GridLock(MiscInfo, ParentLib->PreviewBMPWidth(SGThumb_Small) * OnePixel); 
			break;

		case LibDisplay_LargeThumbTextUnder:
		case LibDisplay_Default:
		case LibDisplay_LargeThumbText:
		case LibDisplay_LargeThumb:
			YSize += GridLock(MiscInfo, ParentLib->PreviewBMPHeight(SGThumb_Large) * OnePixel);
			XSize += GridLock(MiscInfo, ParentLib->PreviewBMPWidth(SGThumb_Large) * OnePixel);
			break;

		case LibDisplay_MediumThumbTextUnder:
		case LibDisplay_MediumThumbText:
		case LibDisplay_MediumThumb:
			YSize += GridLock(MiscInfo, ParentLib->PreviewBMPHeight(SGThumb_Medium) * OnePixel);
			XSize += GridLock(MiscInfo, ParentLib->PreviewBMPWidth(SGThumb_Medium) * OnePixel);
			break;

		case LibDisplay_FullInfo:
		case LibDisplay_SingleLineFullInfo:
			YSize += GridLock(MiscInfo, ParentLib->PreviewBMPHeight(SGThumb_Medium) * OnePixel);
			XSize += GridLock(MiscInfo, ParentLib->PreviewBMPWidth(SGThumb_Medium) * OnePixel);
			//XSize = SG_InfiniteWidth;
			break;

		case LibDisplay_JustText:
			// No description requires no text description space
			YSize = GridLock(MiscInfo, 18 * OnePixel);
			XSize = GridLock(MiscInfo, SG_DefaultNameText);
			break;
	}

	// Calculate a proper rectangle for the item
	CalculateFormatRect(FormatInfo, MiscInfo, XSize, YSize);
}



/***********************************************************************************************

>	virtual void SGLibDisplayItem::HandleRedraw(SGRedrawInfo *RedrawInfo,
												SGMiscInfo *MiscInfo)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/3/95 (base generated in sgbase.cpp)

	Inputs:		RedrawInfo	- The information on the kernel-rendered redraw area
				MiscInfo - always provided. Contains a few useful bits of info that may be
				needed for all event types.

				[Member variable FormatRect should be set up (before calling this method)
				to be the rectangle in which to draw this item]

	Purpose:	SGLibDisplayItem item redraw method - removed from the main HandleEvent
				method merely to make the code tidier.
	Notes:		
	Scope:		private

***********************************************************************************************/

void SGLibDisplayItem::HandleRedraw(SGRedrawInfo *RedrawInfo, SGMiscInfo *MiscInfo)
{
	// First, inform the system that we are about to start rendering this item
	StartRendering(RedrawInfo, MiscInfo);

	// Determine the SGSubLib library object to ask for info on this item
	Library *ParentLib = GetParentLibrary();

	DocRect BmpRect(FormatRect);		// Get my redraw position from the cached FormatRect
	DocRect UnscaledRect(FormatRect);

	RenderRegion *Renderer = RedrawInfo->Renderer;

	INT32 OnePixel  = (INT32) DevicePixels(MiscInfo, 1);
	INT32 TwoPixels = (INT32) DevicePixels(MiscInfo, 2);

	LibDisplayType DType = GetDisplayType(MiscInfo);

//#ifdef _DEBUG
//  Show format rect
//	Renderer->SetFillColour(DocColour(0x40, 0x40, 0x40));	
//	Renderer->DrawRect(&UnscaledRect);
//#endif

	Renderer->SetLineWidth(0);
	Renderer->SetLineColour(RedrawInfo->Transparent);

	if(DType == LibDisplay_JustText)
	{
		BmpRect.hi.x = BmpRect.lo.x + OnePixel;
	}
	else
	{
		INT32 XSize = 0;

		switch(DType)
		{
			case LibDisplay_MediumThumbTextUnder:
			case LibDisplay_MediumThumbText:
			case LibDisplay_MediumThumb:
			case LibDisplay_FullInfo:
			case LibDisplay_SingleLineFullInfo:
				XSize = ParentLib->PreviewBMPWidth(SGThumb_Medium);
				break;
	
			case LibDisplay_SmallThumbText:
			case LibDisplay_SmallThumbTextUnder:
			case LibDisplay_SmallThumb:
				XSize = ParentLib->PreviewBMPWidth(SGThumb_Small);
				break;
	
			case LibDisplay_Default:
			case LibDisplay_LargeThumbText:
			case LibDisplay_LargeThumb:
			case LibDisplay_LargeThumbTextUnder:
			default:
				// OnePixel bodge to get the 1:1 bitmaps inside the rectangle
				XSize = ParentLib->PreviewBMPWidth(SGThumb_Large);
				break;
		}
	
		// Convert pixels to millipoints
		XSize *= OnePixel;

		// Thumbnail rectangle... space around edges but don't scale thumbnail
		BmpRect.hi.x  = BmpRect.lo.x + XSize + (6 * OnePixel);

	  	// Space for selection rectangle
	  	BmpRect.Inflate(-(TwoPixels+OnePixel));

		// Exclude the space needed below the bmp rect from the bmp rect
		switch(DType)
		{
			case LibDisplay_SmallThumbTextUnder:
			case LibDisplay_MediumThumbTextUnder:
			case LibDisplay_LargeThumbTextUnder:
				BmpRect.lo.y += SG_GapAboveText + SG_SPACE_UNDER;
				break;
			default:
				break;
		}

		// Ensure it maps exactly to specific pixels
		GridLockRect(MiscInfo, &BmpRect);

		// Set up the colours for rendering our text, and outline it if selected
		if (Flags.Selected)
		{	
			if(BmpRect.hi.x > UnscaledRect.hi.x) BmpRect.hi.x = UnscaledRect.hi.x;
  
			BmpRect.Inflate(TwoPixels+OnePixel);
			GridLockRect(MiscInfo, &BmpRect);		// Ensure we're on a pixel
			DrawSelectionOutline(RedrawInfo, MiscInfo, &BmpRect);

			BmpRect.Inflate(-(TwoPixels+OnePixel));
		}
		// Draw the thumbnail
		DrawItemThumbnail(RedrawInfo, MiscInfo, &BmpRect);
	}

	// Draw the text
	DrawItemText(Renderer, RedrawInfo, MiscInfo, &UnscaledRect, &BmpRect, Flags.Selected);


	// Finally, inform the system that we have completed rendering this item
	StopRendering(RedrawInfo, MiscInfo);
}



/***********************************************************************************************

>	virtual void SGLibDisplayItem::DrawItemTrumbnail(SGRedrawInfo* pRedrawInfo,	SGMiscInfo* pMiscInfo, DocRect* pDocRect)

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com> 
	Created:	10/3/97 

	Inputs:		pRedrawInfo	- pointer to SGRedrawInfo struct as passes to the HandleRedraw() 
					function from whithin which this function should be called
					pRedrawInfo	- pointer to SGMiscInfo struct as passes to the HandleRedraw() 
					function from whithin which this function should be called
					pDocRect - pointer to the rect where the thumbnail should be drawn

	Purpose:	draws the item's thumbnail
	Scope:		protected

***********************************************************************************************/

void SGLibDisplayItem::DrawItemThumbnail(SGRedrawInfo* pRedrawInfo, SGMiscInfo* pMiscInfo, DocRect* pDocRect)
{
	RenderRegion* pRenderer = pRedrawInfo->Renderer;
	if	(Library::BackgroundRedraw)
	{
		BOOL DrawnBitmap = DrawThumb(pRenderer, pRedrawInfo, pMiscInfo, pDocRect, FALSE);
		if (ShouldDownloadThumb())
			DownloadThumbnail();
		if (ShouldIDrawForeground(DrawnBitmap))
		{
			if (!DrawnBitmap)
			{
				if (!DrawThumb(pRenderer, pRedrawInfo, pMiscInfo, pDocRect, TRUE))
				{
					if (IsDownloadingThumb()) // draw grey rectangle if we're still waiting for the thumbnail
						DrawPlaceholder(pRenderer, pMiscInfo, pDocRect, TRUE);
					else // draw a crossed box 
						DrawPlaceholder(pRenderer, pMiscInfo, pDocRect, FALSE);
				}
			}
		}
		else if (!DrawnBitmap)
			DrawPlaceholder(pRenderer, pMiscInfo, pDocRect, TRUE);
	}
	else if (!DrawThumb(pRenderer, pRedrawInfo, pMiscInfo, pDocRect, TRUE))
		DrawPlaceholder(pRenderer, pMiscInfo, pDocRect, FALSE);
}



/***********************************************************************************************

>	BOOL SGLibDisplayItem::ShouldIDrawForeground(BOOL ForceForeground)

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/5/97

	Inputs:		ForceForeground - TRUE if you want to force foreground redrawing (e.g. if you
				know your thumbnail is cached you are better off just drawing it immediately),
				or FALSE to allow the operation to be backgrounded.

	Returns:	TRUE if you should draw in the foreground (i.e. draw the item to completion),
				FALSE if you should draw the background stuff (i.e. draw a grey box instead
				of a thumbnail) - if FALSE, you'll be called back to complete the redraw later.

	Purpose:	overrides the base class version to force foreground mode if the item is downloading
						its thumbnail. This is necessary to avoid flickering.

  SeeAlso:	SGDisplayNode::ShouldIDrawForeground(BOOL ForceForeground)

***********************************************************************************************/

BOOL SGLibDisplayItem::ShouldIDrawForeground(BOOL ForceForeground)
{
	if (pDownloadOp)
		return SGDisplayNode::ShouldIDrawForeground(TRUE);
	else
		return SGDisplayNode::ShouldIDrawForeground(ForceForeground);
}









/***********************************************************************************************

>	virtual void SGLibDisplayItem::DrawItemText(RenderRegion *Renderer,	SGRedrawInfo *RedrawInfo,
						SGMiscInfo *MiscInfo, DocRect *Rectangle, DocRect *BmpRect, BOOL Selected)	
	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/3/95
	
	Inputs:		Renderer  - Pointer to RenderRegion in which to plot the bitmap rect
				RedrawInfo - Various bits of Redraw information	such as colours
				MiscInfo  -	Miscellaneous info as passed to the redraw handler by the sgallery code
				Rectangle - Pointer to a DocRect the size and position of which the selected
							rectangle will use, so we need to draw our thumbnail inside this.
				BmpRect   - Bitmap rectangle
				Selected  - Should the text be drawn in a selected state ?
	Outputs:
	Returns:	
	Purpose:	Plots the item text in the position as required by the GetDisplayType.
	Notes:
	SeeAlso:	SGLibDisplayItem::HandleRedraw

***********************************************************************************************/

void SGLibDisplayItem::DrawItemText(RenderRegion *Renderer, SGRedrawInfo *RedrawInfo,
	SGMiscInfo *MiscInfo, DocRect *Rectangle, DocRect *BmpRect, BOOL Selected)
{
	LibDisplayType DType = GetDisplayType(MiscInfo);

	// Work out the text rectangle and stick it here
	DocRect TextRect(*Rectangle);

	INT32 OnePixel  = (INT32) DevicePixels(MiscInfo, 1);

	switch(DType)
	{
		case LibDisplay_SmallThumbTextUnder:
		case LibDisplay_MediumThumbTextUnder:
		case LibDisplay_LargeThumbTextUnder:
			TextRect = *Rectangle;
			
			// Give some extra space above and below the clipart
			TextRect.lo.y = TextRect.lo.y + SG_SPACE_UNDER;

			TextRect.hi.y = TextRect.lo.y + SG_GapAboveText;
			
			// give the text a little space below the piccy frame, and more below to the next
			TextRect.Inflate(0, -OnePixel); 
			break;			

		case LibDisplay_SmallThumb:
		case LibDisplay_MediumThumb:
		case LibDisplay_LargeThumb:
			// No text
			return;
			break;

		default:
			TextRect = *Rectangle;
			TextRect.lo.x = BmpRect->hi.x + SG_GapBeforeText;
			break;			
	}

	GridLockRect(MiscInfo, &TextRect);

	Renderer->SetLineWidth(0);
	Renderer->SetLineColour(RedrawInfo->Transparent);
	DocColour red(0xFF, 0, 0); // for new items

	// Set up the colours for rendering our text, and fill the background if selected
	if (Selected)
	{	 
		Renderer->SetFillColour(RedrawInfo->SelBackground);
			
		switch(DType)
		{
			case LibDisplay_SmallThumbTextUnder:
			case LibDisplay_MediumThumbTextUnder:
			case LibDisplay_LargeThumbTextUnder:
				Renderer->DrawRect(&TextRect);
				break;			

			case LibDisplay_FullInfo:
				{
					// Two line full info selection rectangle		
					DocRect SelRect(TextRect);

					// Text not in gallery, don't redraw
					if(TextRect.lo.x > Rectangle->hi.x) return;

					SelRect.hi.y = TextRect.lo.y + TextRect.Height()/2 - (OnePixel * 2);
					SelRect.lo.y = TextRect.lo.y + TextRect.Height()/2 + (OnePixel * 2);
					SelRect.hi.y += SG_DefaultLargeIcon + OnePixel*4;
					SelRect.lo.y -= (SG_DefaultLargeIcon + OnePixel*5);
					SelRect.hi.x = TextRect.hi.x;

					GridLockRect(MiscInfo, &SelRect);
		
					Renderer->DrawRect(&SelRect);
				}
				break;

			default:
				{
					DocRect SelRect(TextRect);

					// Text not in gallery, don't redraw
					if(TextRect.lo.x > Rectangle->hi.x) return;

					SelRect.hi.y = TextRect.lo.y + TextRect.Height()/2 - (OnePixel * 2);
					SelRect.lo.y = TextRect.lo.y + TextRect.Height()/2 + (OnePixel * 2);
					SelRect.hi.y += SG_DefaultLargeIcon/2;
					SelRect.lo.y -= (SG_DefaultLargeIcon/2 + OnePixel);
					SelRect.hi.x = TextRect.hi.x;
				
					GridLockRect(MiscInfo, &SelRect);
		
					Renderer->DrawRect(&SelRect);
				}
				break;			
		}
		Renderer->SetFixedSystemTextColours(&RedrawInfo->SelForeground, &RedrawInfo->SelBackground);
	}
	else
		Renderer->SetFixedSystemTextColours(&RedrawInfo->Foreground, &RedrawInfo->Background);


	// No point drawing the text, etc...
	if(TextRect.lo.x + (OnePixel * 4) > Rectangle->hi.x)
		return;

	// Get the text into the text strings...
	String_256 DisplayText;
	String_256 DisplayText2;

	switch(DType)
	{
		case LibDisplay_FullInfo:
			// Work out proper text strings to display and display it
			// This string is also used by the sorting code, so use a common function to get it...
			if(!GetActualDisplayedText(&DisplayText, 1))
				DisplayText = TEXT("");

			if(!GetActualDisplayedText(&DisplayText2, 2))
				DisplayText2 = TEXT("");
			break;

		default:
			if(!GetActualDisplayedText(&DisplayText, 0))
				DisplayText = TEXT("");
			break;
	}

	if (bIsNew)
	{
		String_256 strTemp(_R(IDS_FOLDERNEW));
		strTemp += _T(" ");
		strTemp += DisplayText;
		DisplayText = strTemp;
	}


	// And finally plot the text itself
	switch(DType)
	{
		case LibDisplay_SmallThumbTextUnder:
		case LibDisplay_MediumThumbTextUnder:
		case LibDisplay_LargeThumbTextUnder:
			{
				// Centred plot

				// Get the size of the text string
				DocRect TextBoundRect;
				Renderer->GetFixedSystemTextSize(&DisplayText, &TextBoundRect);

	 			// Centre the TextBoundRect about the TextRect
				TextBoundRect.Translate(TextRect.lo.x + (TextRect.Width()-TextBoundRect.Width())/2, 
									TextRect.lo.y + (TextRect.Height()-TextBoundRect.Height())/2);

				// Ooops, too big... Use initial rectangle			
				if(TextBoundRect.Width() > TextRect.Width() || TextBoundRect.Height() > TextRect.Height())
					TextBoundRect = TextRect;

				// Draw the text
				Renderer->DrawFixedSystemText(&DisplayText, TextBoundRect);
				if (bIsNew)
				{
					*camStrchr(DisplayText, _T(' ')) = 0x00;
					Renderer->SetFixedSystemTextColours(&red, &RedrawInfo->Background);
					Renderer->DrawFixedSystemText(&DisplayText, TextBoundRect);
					Renderer->SetFixedSystemTextColours(&RedrawInfo->Foreground, &RedrawInfo->Background);
				}
			}
			break;

		case LibDisplay_FullInfo:
			{
				// Two line full info plot
				
				// Get the size of the text string
				DocRect TextBoundRect;
				Renderer->GetFixedSystemTextSize(&DisplayText, &TextBoundRect);

				// Work out the two bounding rectangles
				TextRect.lo.x += (OnePixel * 4);
				DocRect TopTextRect(TextRect);
				DocRect BottomTextRect(TextRect);
				INT32 Shift = (TextBoundRect.Height()/2) + (OnePixel * 2);

				TopTextRect.lo.y += Shift;
				TopTextRect.hi.y += Shift;
				BottomTextRect.lo.y -= Shift;
				BottomTextRect.hi.y -= Shift;

				// Draw the text
				Renderer->DrawFixedSystemText(&DisplayText, TopTextRect);
				if (bIsNew)
				{
					*camStrchr(DisplayText, _T(' ')) = 0x00;
					Renderer->SetFixedSystemTextColours(&red, &RedrawInfo->Background);
					Renderer->DrawFixedSystemText(&DisplayText, TopTextRect);
					Renderer->SetFixedSystemTextColours(&RedrawInfo->Foreground, &RedrawInfo->Background);
				}
				Renderer->DrawFixedSystemText(&DisplayText2, BottomTextRect);
			}
			break;

		default:
			// Left justified plot
			TextRect.lo.x += (OnePixel * 4); //SG_GapBeforeText;
			Renderer->DrawFixedSystemText(&DisplayText, TextRect);
			if (bIsNew)
			{
				*camStrchr(DisplayText, _T(' ')) = 0x00;
				Renderer->SetFixedSystemTextColours(&red, &RedrawInfo->Background);
				Renderer->DrawFixedSystemText(&DisplayText, TextRect);
				Renderer->SetFixedSystemTextColours(&RedrawInfo->Foreground, &RedrawInfo->Background);
			}

			break;
	}
}

/***********************************************************************************************

>	BOOL SGLibDisplayItem::GetThumbSize(SGMiscInfo *MiscInfo, DocRect *Rectangle,
		KernelBitmap *Bitmap, INT32 *XSize, INT32 *YSize, double *Scale = NULL,
		INT32 *XTrans = NULL, INT32 *YTrans = NULL, BOOL *Outline = NULL)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/4/95
	
	Inputs:		MiscInfo  -	Miscellaneous info as passed to the redraw handler by the sgallery code
				Rectangle - Pointer to a DocRect the size and position of which the selected
							rectangle will use, so we need to draw our thumbnail inside this.
				Background - TRUE if redrawing in backgrounding - return FALSE if not in buffer

	Outputs:	XSize - Returned size of thumbnail to use in millipoints
				YSize - Returned size of thumbnail to use in millipoints
				Scale - Returns the Scale factor used
				XTrans - X translation required to centre image in rectangle
				YTrans - Y translation required to centre image in rectangle
				Outline - TRUE if the aspect ratio is such that we should put a rectangle round the item
	
	Returns:	TRUE if things went OK, FALSE otherwise

	Purpose:	Returns the size of thumbnail to render. 
				For thumbnails, drawing them at a 1:1 aspect ratio looks much better than
				simply squashing them into an arbitrary rectangle. This code handles that...

	Notes:		Going for the record number of parameters to a function !

	SeeAlso:	SGLibDisplayItem::HandleRedraw

***********************************************************************************************/

BOOL SGLibDisplayItem::GetThumbSize(SGMiscInfo *MiscInfo, DocRect *Rectangle, KernelBitmap *Bitmap,
		 INT32 *XSize, INT32 *YSize, double *Scale, INT32 *XTrans, INT32 *YTrans, BOOL *Outline)
{
	if( MiscInfo == NULL || Bitmap == NULL || Rectangle == NULL || Bitmap->ActualBitmap == NULL
		|| XSize == NULL || YSize == NULL)
	{
		ERROR3("SGLibDisplayItem::GetThumbSize given null params");
		return FALSE;
	}

	// Get the bitmap's dimensions
	BitmapInfo Info;	
	BOOL InfoOK = Bitmap->ActualBitmap->GetInfo( &Info );

	if(InfoOK == FALSE)
	{
		ERROR3("SGLibDisplayItem::GetThumbSize can't get bitmap info - corrupt bitmap ?");
		return FALSE;
	}

	MILLIPOINT BmpWidth = 0;
	MILLIPOINT BmpHeight = 0;

	INT32 OnePixel = DevicePixels(MiscInfo, 1);

	BmpWidth = Info.PixelWidth * OnePixel;
	BmpHeight = Info.PixelHeight * OnePixel;

	MILLIPOINT BoundWidth = 0;
	MILLIPOINT BoundHeight = 0;

	// Get the details of the bounding rectangle
	// Slight bodge so we round up slightly in the division... Gets the pixel plan right
	BoundWidth = Rectangle->Width();// + 400;
	BoundHeight = Rectangle->Height();// + 400;

	double XScale = (double)BmpWidth / (double)BoundWidth;
	double YScale = (double)BmpHeight / (double)BoundHeight;

	double TheScale = 1.0;

	// Which is the greatest ?
	if(XScale > YScale)
		TheScale = XScale;
	else
		TheScale = YScale;

	// If we're nearly at the correct size for the thumb, don't rescale it - looks better
	if(TheScale > 0.8 && TheScale < 1.0)
		TheScale = (double)1.0;

	// Arg... don't want divide by zero show-stoppers, thank you very much !
	if(TheScale == 0)
		return FALSE;

	*XSize = (INT32)(BmpWidth / TheScale);
	*YSize = (INT32)(BmpHeight / TheScale);

	if(Scale != NULL)
		*Scale = TheScale;

	if(XTrans != NULL)
		*XTrans = Rectangle->lo.x + ((BoundWidth - ((INT32)(BmpWidth / TheScale))) / 2);

	if(YTrans != NULL)
		*YTrans = Rectangle->lo.y + ((BoundHeight - ((INT32)(BmpHeight / TheScale))) / 2);

	if(Outline != NULL && (XScale/YScale) > 1.6)
		*Outline = TRUE;

	return TRUE;
}

/***********************************************************************************************

>	virtual BOOL SGLibDisplayItem::DrawThumb(RenderRegion *Renderer, SGRedrawInfo *RedrawInfo,
											SGMiscInfo *MiscInfo, DocRect *Rectangle, BOOL Background)	
	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/3/95
	
	Inputs:		Renderer  - Pointer to RenderRegion in which to plot the bitmap rect
				RedrawInfo - Various bits of Redraw information	such as colours
				MiscInfo  -	Miscellaneous info as passed to the redraw handler by the sgallery code
				Rectangle - Pointer to a DocRect the size and position of which the selected
							rectangle will use, so we need to draw our thumbnail inside this.
				Background - TRUE if redrawing in backgrounding - return FALSE if not in buffer
	Outputs:
	Returns:	TRUE if things went OK, FALSE otherwise
	Purpose:	Draws a thumbnail for the library item. This can be overridden, so the colour
				library for instance would plot a rectangle instead of a bitmap.

				For bitmaps, drawing them at a 1:1 aspect ratio looks much better than
				simply squashing them into an arbitrary rectangle.
	Notes:
	SeeAlso:	SGLibDisplayItem::HandleRedraw

***********************************************************************************************/

BOOL SGLibDisplayItem::DrawThumb(RenderRegion *Renderer, SGRedrawInfo *RedrawInfo, 
									SGMiscInfo *MiscInfo, DocRect *Rectangle, BOOL Background)
{
	KernelBitmap *Bitmap = GetDisplayedKernelBitmap(MiscInfo, Background);
						  
	// Bitmap not there...
	if(Bitmap == NULL)
		return FALSE;

	INT32 XSize = 0;
	INT32 YSize = 0;
	double Scale = (double)1;
	INT32 XTrans = 0;
	INT32 YTrans = 0;
	BOOL Outline = FALSE;

	if(!GetThumbSize(MiscInfo, Rectangle, Bitmap, &XSize, &YSize, &Scale, &XTrans, &YTrans, &Outline))
		return FALSE;

	DocRect BmpRect(0, 0, XSize, YSize);														
	BmpRect.Translate(XTrans, YTrans);

#if 0
	// Setup the bitmap ready for plotting
	UINT32 InputBPP = Bitmap->GetBPP();
	if(InputBPP >= 24)
	{
		CDC *pDC = Renderer->GetRenderDC();
		INT32 OutputBPP = pDC->GetDeviceCaps(BITSPIXEL);
	
		if(OutputBPP == 8)
		{
			//ERROR3("Given a 32\\24bpp bitmap to plot on an 8bpp device");
		}
	}
#endif
	
	// Create a path with the bitmap in, ready for plotting...
	NodeBitmap *DummyBmp = new NodeBitmap();

	if(DummyBmp == NULL)
	{
		ERROR3("SGLibDisplayItem::DrawThumb can't get node bitmap - not enough memory ?");
		return FALSE;
	}

	DummyBmp->SetUpPath();
	DummyBmp->CreateShape(BmpRect);
	DummyBmp->GetBitmapRef()->SetBitmap(Bitmap);

	// Plot white background rectangle...
	//Renderer->SetFillColour(RedrawInfo->Background);
	//Renderer->SetLineColour(COLOUR_TRANS);	//RedrawInfo->Background);
	//Renderer->DrawRect(Rectangle);

	// Now render the bitmap
	DummyBmp->Render(Renderer);

	delete DummyBmp;

	// If we're displaying text underneath the thumbnail, add some space for it
	LibDisplayType DType = GetDisplayType(MiscInfo);

	// Extra space required by the text
	switch(DType)
	{
		case LibDisplay_SmallThumbTextUnder:
		case LibDisplay_MediumThumbTextUnder:
		case LibDisplay_LargeThumbTextUnder:
			// Text is underneath
			// Border around small objects
			if(Outline)
			{
				INT32 OnePixel = DevicePixels(MiscInfo, 1);

				Renderer->SetLineWidth(0);
				Renderer->SetLineColour(DocColour(COLOUR_TRANS));
				Renderer->SetFillColour(DocColour(COLOUR_GREY));

				DocRect TempRect(*Rectangle);					// Left
				TempRect.Inflate(OnePixel);
				TempRect.hi.x = TempRect.lo.x + OnePixel;
				Renderer->DrawRect(&TempRect);

				TempRect = *Rectangle;							// Top
				TempRect.Inflate(OnePixel);
				TempRect.lo.y = TempRect.hi.y - OnePixel;
				Renderer->DrawRect(&TempRect);

				TempRect = *Rectangle;							// Right
				TempRect.Inflate(OnePixel);
				TempRect.lo.x = TempRect.hi.x - OnePixel;
				Renderer->DrawRect(&TempRect);

				TempRect = *Rectangle;							// Bottom
				TempRect.Inflate(OnePixel);
				TempRect.hi.y = TempRect.lo.y + OnePixel;
				Renderer->DrawRect(&TempRect);
			}
			break;			

		default:
			// don't add a rectangle unless the text is below - hence a long way away
			break;
	}
									   	
	// Drawn bitmap preview ok
	return TRUE;
}

/***********************************************************************************************

>	static void SGLibDisplayItem::DrawNullBitmapRect(RenderRegion *Renderer,
								SGMiscInfo *MiscInfo, DocRect *Rectangle, BOOL Background)	
	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/3/95
	
	Inputs:		Renderer  - Pointer to RenderRegion in which to plot the bitmap rect
				MiscInfo  -	Miscellaneous info as passed to the redraw handler by the sgallery code
				Rectangle - Pointer to a DocRect the size and position of where the bitmap would
							be positioned normally
				Background - If true, draw a background redrawing, not plotted proper thumbnail rectangle
	Outputs:

	Returns:

	Purpose:	Draws a rectangle with two lines crossing it, to signify that the thumbnail
				could not be found, or could not be generated.
				
	Notes:

	SeeAlso:	SGLibDisplayItem::HandleRedraw

***********************************************************************************************/

void SGLibDisplayItem::DrawNullBitmapRect(RenderRegion *Renderer, SGMiscInfo *MiscInfo,
						DocRect *Rectangle, BOOL Background)
{
	
	if(Renderer == NULL || MiscInfo == NULL || Rectangle == NULL)
	{
		ERROR3("SGLibDisplayItem::DrawNullBitmapRect null params are BAD");
		return;
	}
	
	

	// Draw the 'blank' bitmap
	
	if(!Background)
		Renderer->SetFillColour(COLOUR_GREY);
	else
	{
		DocColour NullBitmapColour(230L, 230L, 230L);
		Renderer->SetFillColour(NullBitmapColour);
	}

	GridLockRect(MiscInfo, Rectangle);
	Renderer->DrawRect(Rectangle);

	// Yes I know it's horrible, but since we're now a static, DevicePixels doesn't seem to work
	INT32 OnePixel = MiscInfo->PixelSize;

	Renderer->SetLineWidth(0);
	Renderer->SetLineColour(DocColour(COLOUR_TRANS));
	Renderer->SetFillColour(DocColour(COLOUR_BLACK));

	DocRect TempRect(*Rectangle);					// Left
	TempRect.hi.x = TempRect.lo.x + OnePixel;
	Renderer->DrawRect(&TempRect);

	TempRect = *Rectangle;							// Top
	TempRect.lo.y = TempRect.hi.y - OnePixel;
	Renderer->DrawRect(&TempRect);

	TempRect = *Rectangle;							// Right
	TempRect.lo.x = TempRect.hi.x - OnePixel;
	Renderer->DrawRect(&TempRect);

	TempRect = *Rectangle;							// Bottom
	TempRect.hi.y = TempRect.lo.y + OnePixel;
	Renderer->DrawRect(&TempRect);

	if(!Background)
	{
		// Pop two intersecting lines through the centre of the rectangle to signify
		// that the item doesn't have a thumbnail
		TempRect = *Rectangle;						// Middle X
		TempRect.hi.y = TempRect.lo.y + (TempRect.Height() / 2);
		TempRect.lo.y = TempRect.hi.y;
		GridLockRect(MiscInfo, &TempRect);
		TempRect.hi.y += OnePixel;
		Renderer->DrawRect(&TempRect);

		TempRect = *Rectangle;						// Middle Y
		TempRect.hi.x = TempRect.lo.x + (TempRect.Width() / 2);
		TempRect.lo.x = TempRect.hi.x;
		GridLockRect(MiscInfo, &TempRect);
		TempRect.hi.x += OnePixel;
		Renderer->DrawRect(&TempRect);
	}
}


/***********************************************************************************************

>	virtual LibDisplayType SGLibDisplayItem::GetDisplayType(SGMiscInfo *MiscInfo)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/3/95

	Inputs:		MiscInfo - Contains a few useful bits of info that may be
				needed for all event types.
	Outputs:	
	Returns:	The display mode type to use (position of text, and size of thumb)

	Purpose:	Return the display type to use - this should be overridden by the
				library gallery using the code.

	Notes:		There is the possibility that the MiscInfo passed only has a valid
				DisplayMode setup - the rest might be garbage

***********************************************************************************************/

LibDisplayType SGLibDisplayItem::GetDisplayType(SGMiscInfo *MiscInfo)
{
	switch(MiscInfo->DisplayMode)
	{
		case 1:
			return LibDisplay_SmallThumbText;
			break;
		case 2:
			return LibDisplay_FullInfo;
			break;
		case 0:
		default:
			return LibDisplay_LargeThumbTextUnder;
			break;
	}

	return LibDisplay_LargeThumbTextUnder;
}

/***********************************************************************************************

>	virtual void SGLibDisplayItem::GetFileName(String_256 *Result);

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/10/95

	Inputs:
	Outputs:	Result - String to place resulting filename in

	Purpose:	Returns the filename (without path) for the given item
				This is needed by the searching code

***********************************************************************************************/

void SGLibDisplayItem::GetFileName(String_256 *Result)
{
	ERROR3IF(Result == NULL, "SGLibDisplayItem::GetFileName given a NULL param");

	//	If we can get a Library from an item, we're sorted...
	Library *Lib = GetParentLibrary();

	if (Lib != NULL)
	{
		Lib->GetFilename(TheLibraryIndex, Result, FALSE);
	}
	else
	{
		*Result = "";
	}
}

/***********************************************************************************************

>	virtual BOOL SGLibDisplayItem::GetFileName(String_256 *Result);

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/10/95

	Inputs:
	Outputs:	Result - String to place resulting filename in

	Purpose:	Returns the filename (without path) for the given item
				This is needed by the searching code

	Notes:		Quicker than previous String_256 version...

***********************************************************************************************/

BOOL SGLibDisplayItem::GetFileNamePtr(TCHAR **Result)
{
	ERROR3IF(Result == NULL, "SGLibDisplayItem::GetFileName given a NULL param");

	BOOL ok = FALSE;

	//	If we can get a Library from an item, we're sorted...
	Library *Lib = GetParentLibrary();

	if (Lib != NULL)
		ok = Lib->GetFilename(TheLibraryIndex, Result);
	else
		ok = FALSE;

	return (ok);
}

/***********************************************************************************************

>	virtual void SGLibDisplayItem::GetNameText(String_256 *Result)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/3/95

	Inputs:
	Outputs:	Result - String to place resulting text in
	Returns:	The display mode type to use (position of text, and size of thumb)

	Purpose:	Returns the name text for this item, to support simple searching
				and sorting operations, and generic redraw methods for library items.

***********************************************************************************************/

void SGLibDisplayItem::GetNameText(String_256 *Result)
{
	ERROR3IF(Result == NULL, "SGLibDisplayItem::GetNameText given a NULL param");

	//	If we can get a Library from an item, we're sorted...
	Library *Lib = GetParentLibrary();

	if (Lib != NULL)
	{
		if(!Lib->GetTitle(TheLibraryIndex, Result))
			Lib->GetFilename(TheLibraryIndex, Result, FALSE);
	}
	else
	{
		*Result = "";
	}
}

/***********************************************************************************************

>	BOOL SGLibDisplayItem::GetNameTextPtr(TCHAR **Result)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/5/95

	Inputs:
	Outputs:	Result - points to name in memory if the index file is cached
	Returns:	TRUE if index was cached and name found ok, etc...

	Purpose:	Returns a pointer to the filename for this item... The main use of this
				call is to speed sorting up, since passing pointers about is much quicker
				than constructing strings....
	
	Notes:		BEWARE: If the index file is not in memory this call will probably fail !

***********************************************************************************************/

BOOL SGLibDisplayItem::GetNameTextPtr(TCHAR **Result)
{
	ERROR3IF(Result == NULL, "SGLibDisplayItem::GetNameText given a NULL param");

	//	If we can get a Library from an item, we're sorted...
	Library *Lib = GetParentLibrary();

	BOOL ok = FALSE;

	if (Lib != NULL)
	{
		ok = Lib->GetTitle(TheLibraryIndex, Result);
		if(!ok)
			ok = Lib->GetFilename(TheLibraryIndex, Result);
	}

	return ok;
}

/***********************************************************************************************

>	virtual void SGLibDisplayItem::GetFullInfoText(String_256 *Result)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/3/95

	Outputs:	Result 	- String to place resulting text in

	Purpose:	Returns the full-info text for this item, to support simple searching
				operations, and generic redraw methods for library items.
	Notes:		This overrides the sgdisplaynode version to give full information...
	SeeAlso:

***********************************************************************************************/

void SGLibDisplayItem::GetFullInfoText(String_256 *Result)
{
	// Call the below function to get a full line of text...
	GetFullInfoText(Result, 0);
}

/***********************************************************************************************

>	virtual void SGLibDisplayItem::GetFullInfoText(String_256 *Result, INT32 Line = 0)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/3/95

	Inputs:		Line 	- Line number for multi-line strings. All multi-liners start from 1,
						  and return FALSE if an invalid line is supplied.
						  Setting Line to 0 will return all the text information, String_256
						  permitting of course...
	Outputs:	Result 	- String to place resulting text in

	Purpose:	Returns the full-info text for this item, to support simple searching
				operations, and generic redraw methods for library items.
	Notes:
	SeeAlso:

***********************************************************************************************/

void SGLibDisplayItem::GetFullInfoText(String_256 *Result, INT32 Line)
{
	ERROR3IF(Result == NULL, "SGLibDisplayItem::GetFullInfoText given a NULL param");

	//	If we can get a Library from an item, we're sorted...
	Library *Lib = GetParentLibrary();
														  
	if (Lib == NULL)
	{
		ERROR3("Lib == NULL in GetFullInfoText");
		*Result = "";
	}
	else
	{
		// No text if Line is set to something weird...
		*Result = TEXT("");

		String_256 Text;

		// Full description and title, as given in the index
		if(Line < 2 && Line >= 0)
		{
			GetDisplayedTextDescription(&Text);
			BOOL FullDescription = (Text.Length() > 0);
			*Result = Text;

			// Title used in normal display modes
			if(Lib->GetTitle(TheLibraryIndex, &Text))
			{
				if(FullDescription)
				{
					// " (Text)"
					String_256 TmpResult;
					TmpResult.MakeMsg(_R(IDS_SGLIB_FULLDESCRIPTION), (TCHAR *)Text);
					*Result += TmpResult;
				}
				else
					*Result += Text;
			}
		}

		// Trivial bit of logic made tricky here...
		// If the first line was blank, give it the Filename, else only give the second
		// line the filename

		BOOL GiveLineFileName = FALSE;

		if((Line == 0) || (Line == 1 && Result->Length() == 0))
			GiveLineFileName = TRUE;
		
		if(Line == 2)
		{
			GiveLineFileName = FALSE;

			GetDisplayedTextDescription(&Text);
			if(Text.Length() > 0)
				GiveLineFileName = TRUE;

			// Title used in normal display modes
			if(Lib->GetTitle(TheLibraryIndex, &Text))
			if(Text.Length() > 0)
				GiveLineFileName = TRUE;
		}

		// The line needs the filename...
		if(GiveLineFileName)
		{
			// Filename of file without the path
			Lib->GetFilename(TheLibraryIndex, &Text, FALSE);

/*			if(Line == 0)
				*Result += TEXT(", ");

			*Result += TEXT("'");
			*Result += Text;
			*Result += TEXT("'");

			if(Line != 1)
				*Result += TEXT(", ");*/

			String_256 FileNameBit;

			switch(Line)
			{
				case 0:
					FileNameBit.MakeMsg(_R(IDS_SGLIB_FNAMEBIT_LINE0), (TCHAR *)Text); //", '#1%s', "
					break;

				case 1:
					FileNameBit.MakeMsg(_R(IDS_SGLIB_FNAMEBIT_LINE1), (TCHAR *)Text); //"'#1%s'"
					break;

				default:
					FileNameBit.MakeMsg(_R(IDS_SGLIB_FNAMEBIT_DEFAULT), (TCHAR *)Text); //"'#1%s', "
					break;
			}

			*Result += FileNameBit;
		}

		// Filesize of file, expressed in a pleasant textual format
		if((Line == 0) || (Line == 2))
		{
	  		INT32 FileSize = GetFileSize(Lib);
			if(Convert::BytesToString(&Text, (UINT32)FileSize))
			{
				*Result += Text;
			}
		}
	}
}

/***********************************************************************************************

>	virtual void SGLibDisplayItem::GetKeyWords(String_256 *Result)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/3/95

	Outputs:	On exit, the string pointed at by Result will contain either a blank
				string, or a list of | seperated keywords associated with the item
				
	Purpose:	To determine the keywords for this node. Generally, this is used for
				a simple searching mechanism.
				
	Notes:		The base class returns a blank string.
				If you can provide a better name string, then override the base class
				method to do so.

	SeeAlso:	SGDisplayNode::GetFullInfoText

***********************************************************************************************/

void SGLibDisplayItem::GetKeyWords(String_256 *Result)
{
	ERROR3IF(Result == NULL, "SGLibDisplayItem::GetKeyWords given a NULL param");
	if(Result == NULL)
		return;	

	*Result = "";

	//SGDisplayGroup *SGGroup = (SGDisplayGroup *) GetParent();
	SuperGallery *Parent = (SuperGallery *)GetParentGallery();

	if(Parent != NULL)
	{
		if(Parent->CanSearchKeywords())
		{	
			Library *Lib = GetParentLibrary();
		 	if (Lib != NULL)
			{
				String_64 Key(_R(IDS_LIBRARIES_INDEX_ITEM_KEY));
				Lib->GetSingleField(TheLibraryIndex, &Key, Result);
			}
		}
	}
}

/***********************************************************************************************

>	INT32 SGLibDisplayItem::GetFileSize(Library *Lib == NULL)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/4/95

	Inputs:		Lib can optionally point to the library to use - we can get it if it
				doesn't.
	Outputs:	On exit, the string pointed at by Result will contain either a blank
				string, or a list of | seperated keywords associated with the item
				
	Purpose:	To determine the keywords for this node. Generally, this is used for
				a simple searching mechanism.
				
	Notes:		The base class returns a blank string.
				If you can provide a better name string, then override the base class
				method to do so.

	SeeAlso:	SGDisplayNode::GetFullInfoText

***********************************************************************************************/

INT32 SGLibDisplayItem::GetFileSize(Library *Lib)
{
	// Check the index file first...
 	if (Lib == NULL)
		Lib = GetParentLibrary();

 	if (Lib != NULL)
	{
		String_64 SizeTxt(_R(IDS_LIBRARIES_INDEX_ITEM_SIZE));
		String_64 Result;
		if(Lib->GetSingleField(TheLibraryIndex, &SizeTxt, &Result))
		{
			INT32 size = _ttoi((TCHAR *)Result);
			if(size > 0)
				return size;
		}
	}

	// If the index info is not available, or the size was 0 in the index,
	// resort to using the OS on the file
	PathName ItemPath;					
	GetFileName(&ItemPath);
	return SGLibOil::FileSize(&ItemPath);
}

/********************************************************************************************

>	virtual BOOL SGLibDisplayItem::GetBubbleHelp(DocCoord *MousePos, String_256 *Result)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/6/95

	Inputs:		MousePos - The current mouse position. This will generally be expected
				to lie inside this item's FormatRect. With it, this item can provide
				help on specific areas of an item.

	Outputs:	On exit, if the return value is TRUE, the string pointed at by Result
				will contain a bubble help string for this item

	Returns:	TRUE if it filled in the string, FALSE if it did not
				
	Purpose:	Called by the parent gallery when bubble help is needed. The parent
				gallery will do a hit test to determine which node contains the pointer,
				and will then ask that node to supply bubble/status-line help.
				
	Notes:		The base class returns FALSE (i.e. provides no help)
				If you can provide help, then override the base class method to do so.

	SeeAlso:	SGDisplayNode::GetStatusLineHelp

********************************************************************************************/

BOOL SGLibDisplayItem::GetBubbleHelp(DocCoord *MousePos, String_256 *Result)
{
	ERROR3IF(MousePos == NULL || Result == NULL, "Invalid NULL params");

	return FALSE;
}


	
/********************************************************************************************

>	virtual BOOL SGLibDisplayItem::GetStatusLineHelp(DocCoord *MousePos, String_256 *Result)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/1/95

	Inputs:		MousePos - The current mouse position. This will generally be expected
				to lie inside this item's FormatRect. With it, this item can provide
				help on specific areas of an item.

	Outputs:	On exit, if the return value is TRUE, the string pointed at by Result
				will contain a status line help string for this item

	Returns:	TRUE if it filled in the string, FALSE if it did not
				
	Purpose:	Called by the parent gallery when status line help is needed. The parent
				gallery will do a hit test to determine which node contains the pointer,
				and will then ask that node to supply bubble/status-line help.
				
	Notes:		The base class returns FALSE (i.e. provides no help)
				If you can provide help, then override the base class method to do so.

	SeeAlso:	SGDisplayNode::GetBubbleHelp

********************************************************************************************/

BOOL SGLibDisplayItem::GetStatusLineHelp(DocCoord *MousePos, String_256 *Result)
{
	ERROR3IF(MousePos == NULL || Result == NULL, "Invalid NULL params");

	GetNameText(Result);

	// Filesize of file, expressed in a pleasant textual format
	String_32 SizeString;
	INT32 FileSize = GetFileSize(NULL);
	if(Convert::BytesToString(&SizeString, (UINT32)GetFileSize(NULL)))
	{
		String_32 TmpStr;
		TmpStr.MakeMsg(_R(IDS_SGLIB_STAT_LINE_SIZE), (TCHAR *)SizeString);
		*Result += TmpStr; // " (Size)"
	}

	/* Double click to open this file, or drag and drop */
	String_256 DClick(_R(IDS_LIBRARY_DOUBLE_CLICK_OPEN));

	*Result += String_8(_R(IDS_SGFONTS_STATUS_LINE_SEP)); // "; "
	*Result += DClick;

	return(TRUE);
}






/***********************************************************************************************

>	virtual BOOL SGLibDisplayItem::HandleEvent(SGEventType EventType, void *EventInfo,
											 SGMiscInfo *MiscInfo)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/1/95

	Inputs:		EventType - An enumerated value describing what type of event is to be processed

				EventInfo - A structure describing the event (may be NULL). The exact thing
							pointed at by this pointer depends upon the event type:

							MonoOn
							Event				Thing EventInfo points at
							SGEVENT_FORMAT		(SGFormatInfo *)
							SGEVENT_REDRAW		(SGRedrawInfo *)
							SGEVENT_MOUSECLICK	(SGMouseInfo *)
							MonoOff
				Use the provided SGDisplayNode::Get[Format]Info() inlines to retrieve this
				information - they provide useful error/type checking, and hide the cast

				MiscInfo - always provided. Contains a few useful bits of info that may be
				needed for all event types.

	Outputs:	FormatInfo is updated as appropriate

	Returns:	TRUE if the event was handled successfully
				FALSE if it was not

	Purpose:	Handles a SuperGallery DisplayTree event

	Notes:		This overrides the pure virtual SGDisplayNode::HandleEvent method

				A node need not handle a specific event - if it does not handle it, it
				should return FALSE.

				Redraw and Formatting handlers should never return TRUE, as this will
				prevent the event from continuing through the tree.

				Non-leaf-nodes must call SGDisplayNode::GiveEventToMyChildren in order
				to pass the event dow the tree. THIS node is a leaf-node, so it doesn't.

	SeeAlso:	SGDisplayNode::HandleEvent

***********************************************************************************************/

BOOL SGLibDisplayItem::HandleEvent(SGEventType EventType, void *EventInfo,
								  SGMiscInfo *MiscInfo)
{
	switch (EventType)
	{
		case SGEVENT_FORMAT:
			{
				SGFormatInfo *FormatInfo = GetFormatInfo(EventType, EventInfo);
				CalculateMyRect(FormatInfo, MiscInfo);		// Cache our FormatRect for later use
			}
			break;


		case SGEVENT_REDRAW:
			{
				DocRect MyRect(FormatRect);		// Rely on FormatRect being cached from above
				SGRedrawInfo *RedrawInfo = GetRedrawInfo(EventType, EventInfo);

				if (IMustRedraw(RedrawInfo))	// only redraw if we intersect the clip rect
					HandleRedraw(RedrawInfo, MiscInfo);
			}
			break;		// exit and return FALSE to pass the redraw event on


		case SGEVENT_MOUSECLICK:
			{
				SGMouseInfo *Mouse = GetMouseInfo(EventType, EventInfo);

				if (FormatRect.ContainsCoord(Mouse->Position))
				{
					// No drag, so move on to normal selection click handling
					DefaultClickHandler(Mouse, MiscInfo);
					return(TRUE);
				}
			}
			break;

		default:
			return(SGDisplayItem::HandleEvent(EventType, EventInfo, MiscInfo));
			break;
	}

	// Default return value: We do not claim this event, so it will be passed on to others
	return(FALSE);
}




/***********************************************************************************************

>	virtual BOOL SGLibDisplayItem::StartDrag(SGEventType EventType, void *EventInfo,
												SGMiscInfo *MiscInfo, INT32 *XSize, INT32 *YSize)
	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/12/95

	Inputs:		EventType - An enumerated value describing what type of event is to be processed
				EventInfo - A structure describing the event
				MiscInfo - always provided. Contains a few useful bits of info that may be
				needed for all event types.

	Outputs:	XSize, YSize - Dimensions of object to drag

	Returns:	TRUE if it was a drag, false if double-click

	Purpose:	Sets the TmpDraggingBitmap up, and returns the sizes to the caller.

	Notes:		If this returns FALSE, don't try to create the drag object because something
				bad has happened. If it returns TRUE though, you should create the DragInfo
				object and call DragManagerOp::StartDrag(DragClipart);

***********************************************************************************************/

BOOL SGLibDisplayItem::StartDrag(SGEventType EventType, void *EventInfo, SGMiscInfo *MiscInfo, INT32 *XSize, INT32 *YSize)
{
	SGMouseInfo *Mouse = GetMouseInfo(EventType, EventInfo);

	if (Mouse->DoubleClick)
	{
		DefaultClickHandler(Mouse, MiscInfo);
		return FALSE;
	}
	else
	{
		DefaultPreDragHandler(Mouse, MiscInfo);

		// Grab a copy of the bitmap required for dragging
		KernelBitmap *DispBmp = GetDisplayedKernelBitmap(MiscInfo, TRUE);
		if(DispBmp)
			LibraryGallery::TmpDraggingBitmap = DIBUtil::CopyKernelBitmap(DispBmp, TRUE);
		else
			LibraryGallery::TmpDraggingBitmap = NULL;

		// Find the displayed pixel sizes of the bitmap to drag
		Library *ParentLib = GetParentLibrary();
		if(ParentLib != 0)
		{
			// Thumbnail cache for library
			SGThumbs *Thumbs = ParentLib->Thumbnails;
			if(Thumbs != NULL)
			{
				// Get current size from thumbnail cache
				SGThumbSize ThumbSize = SGThumb_Large;
				if(Thumbs->GetSize(&ThumbSize))
				{
					*XSize = ParentLib->PreviewBMPWidth(ThumbSize);
					*YSize = ParentLib->PreviewBMPHeight(ThumbSize);
				
					if(LibraryGallery::TmpDraggingBitmap != NULL)
					{
						INT32 OnePixel  = (INT32) DevicePixels(MiscInfo, 1);									
						DocRect Rectangle(0, 0, (*XSize) * OnePixel, (*YSize) * OnePixel);
			
						// Get the proper aspect ratio'd clipped XSize and YSize (MILLIPOINTS)					
						if(!GetThumbSize(MiscInfo, &Rectangle, LibraryGallery::TmpDraggingBitmap, XSize, YSize))
						{
							*YSize = 0;
							*XSize = 0;
						}
						else
						{
							// Convert Millipoint values into pixel values
							*XSize = (*XSize) / OnePixel;
							*YSize = (*YSize) / OnePixel;
						}
					}
				}
			}
		}
	}
	return TRUE;
}



/***********************************************************************************************

>	Library *SGLibDisplayItem::GetParentLibrary(void)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/2/95

	Returns:	NULL if a serious error occurred (ERROR2 reported)
				else a pointer to the parent SGSubLib library object

	Purpose:	Finds the library for which this DisplayItem displays an item.

***********************************************************************************************/

Library *SGLibDisplayItem::GetParentLibrary(void)
{
	SGDisplayGroup *Parent = (SGDisplayGroup *) GetParent();
	if (Parent == NULL)
	{
		ERROR2RAW("Infeasible gallery display tree structure detected");
		return(NULL);
	}

	Library *ParentLib = Parent->GetParentLibrary();
	/*if (ParentLib == NULL) 
	{
		ERROR2RAW("Supergallery library display item has no attached library!");
		return(NULL);
	}*/ 

	return(ParentLib);
}



/********************************************************************************************

>	KernelBitmap *SGLibDisplayItem::GetDisplayedKernelBitmap(SGMiscInfo *MiscInfo, BOOL Background)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/02/95

 	Inputs:		MiscInfo - misc info given to the gallery redraw code
				Background - if false then redrawing in foreground - only return bmp if in cache
	Outputs:
	
	Returns:	A pointer to the KernelBitmap which this Display Item is used to display,
				or NULL if there is no thumbnail (e.g. a colour library has no thumbs),
				or the thumbnail failed to load/cache
			
	Purpose:	To find out the KernelBitmap this object is responsible for displaying

********************************************************************************************/

KernelBitmap *SGLibDisplayItem::GetDisplayedKernelBitmap(SGMiscInfo *MiscInfo, BOOL Background)
{
	KernelBitmap *BM = NULL;
	SGThumbSize Size = SGThumb_Large;

	// Get the current display mode type 
	LibDisplayType DType = GetDisplayType(MiscInfo);

	// Which thumbnail should we be asking the thumb cache for ?
	switch(DType)
	{
		case LibDisplay_MediumThumbText:
		case LibDisplay_MediumThumbTextUnder:
		case LibDisplay_MediumThumb:
		case LibDisplay_FullInfo:
		case LibDisplay_SingleLineFullInfo:
			Size = SGThumb_Medium;
			break;
	
		case LibDisplay_SmallThumbText:
		case LibDisplay_SmallThumbTextUnder:
		case LibDisplay_SmallThumb:
			Size = SGThumb_Small;
			break;

		case LibDisplay_JustText:
			return NULL;
	
		default:
			Size = SGThumb_Large;
			break;
	}
																													 
	//	If we can get a Library from an item, we're sorted...
	Library *Lib = GetParentLibrary();
		
	// For background redraws we want the bitmap, but for foreground ones we only want
	// it if it's in the cache...
	BOOL GotIt = FALSE;		
									  
	if (Lib != NULL)
		GotIt = Lib->GetThumbnail(TheLibraryIndex, Size, Background, &BM);

	if(GotIt)
		return(BM);
	else
		return NULL;
}


/********************************************************************************************

>	BOOL SGLibDisplayItem::GetDisplayedTextDescription(String_256 *Result)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/02/95

	Outputs:	Result - will be filled in with the Description string which this
				Display Item uses.
			
	Returns:	FALSE if it failed, else TRUE

	Purpose:	To find the Text description for this object

********************************************************************************************/

BOOL SGLibDisplayItem::GetDisplayedTextDescription(String_256 *Result)
{
	ERROR3IF(Result == NULL,
			"SGLibDisplayItem::GetDisplayedTextDescription - NULL param is illegal");

	BOOL ok = FALSE;

	Library *Lib = GetParentLibrary();
					 
	// Get the name using our library code				 	
	if (Lib != NULL)
		ok = Lib->GetTextname(TheLibraryIndex, Result);

	// Ensure String wiped if any problems
	if(!ok)
		*Result = TEXT("");

	return ok;
}

/********************************************************************************************

>	BOOL SGLibDisplayItem::GetDisplayedTextDescription(TCHAR **Result)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/05/95

	Outputs:	Result - will be filled in with the Description string which this
				Display Item uses.
			
	Returns:	FALSE if it failed, else TRUE

	Purpose:	To find the Text description for this object

	Notes:		Will return false if the index is not in memory...

********************************************************************************************/

BOOL SGLibDisplayItem::GetDisplayedTextDescription(TCHAR **Result)
{
	ERROR3IF(Result == NULL,
			"SGLibDisplayItem::GetDisplayedTextDescription - NULL param is illegal");
	Library *Lib = GetParentLibrary();

	// Get the name using our library code				 	
	if (Lib != NULL)
		return(Lib->GetTextname(TheLibraryIndex, Result));

	return(FALSE);
}

/********************************************************************************************

>	BOOL SGLibDisplayItem::GetFileName(PathName *Result)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/03/95

	Outputs:	Result - will be filled in with the full filename of the object
			
	Returns:	FALSE if it failed, else TRUE

	Purpose:	To find the corresponding filename for this object

********************************************************************************************/

BOOL SGLibDisplayItem::GetFileName(PathName *Result)
{
	ERROR3IF(Result == NULL,
			"SGLibDisplayItem::GetFileName - NULL param is illegal");
	Library *Lib = GetParentLibrary();

	Result->SetPathName(String_8(""), FALSE);	// Ensure path wiped if we fail
	//Error::ClearError();
				 
	// Get the name using our library code				 	
	if (Lib != NULL)
	{
		String_256 TmpPath;
		BOOL ok = Lib->GetFilename(TheLibraryIndex, &TmpPath, TRUE);
		Result->SetPathName(TmpPath);
		
		// Dodgy pathname
		if(!ok && !Result->IsValid())
			ok = FALSE;
		return ok;
	}

	return FALSE;
}




/***********************************************************************************************

>	virtual BOOL SGLibDisplayItem::GetActualDisplayedText(String_256 *String, INT32 Line = 0)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/5/95

	Inputs:		Line 	- Line number for multi-line strings. All multi-liners start from 1,
						  and return FALSE if an invalid line is supplied.
						  Setting Line to 0 will return all the text information, String_256
						  permitting of course...

	Outputs:	String	- Returns the string that will actually be displayed in the current mode

	Returns:	TRUE if all went ok

	Purpose:	Returns the mode-sensitive string that will actually be displayed. This is
				handy for sorting methods, etc...

***********************************************************************************************/

BOOL SGLibDisplayItem::GetActualDisplayedText(String_256 *String, INT32 Line)
{
	ERROR3IF(String == NULL, "SGLibDisplayItem::GetActualDisplayedText with params is a real recipe for disaster");

	// We're going to do something horrible here...
	SGMiscInfo MiscInfo;
	SuperGallery *ParentGal = GetParentGallery();
	ERROR3IF(ParentGal == NULL, "SGLibDisplayItem::GetActualDisplayedText with null parent gallery");
	MiscInfo.DisplayMode = ParentGal->GetDisplayMode();
	LibDisplayType DType = GetDisplayType(&MiscInfo);

	// Work out proper text string to display and display it
	switch(DType)
	{
		case LibDisplay_FullInfo:
		case LibDisplay_SingleLineFullInfo:
			// Use the full info description
			GetFullInfoText(String, Line);
			break;

		default:
			// Just use the normal text description
			GetNameText(String);
			break;
	}

	return TRUE;
}

/***********************************************************************************************

>	virtual BOOL SGLibDisplayItem::GetActualDisplayedTextPtr(TCHAR **String, INT32 Line = 0)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/5/95

	Inputs:		Line 	- Line number for multi-line strings. All multi-liners start from 1,
						  and return FALSE if an invalid line is supplied.
						  Setting Line to 0 will return all the text information, String_256
						  permitting of course...

	Outputs:	String	- Returns a ptr to the string that will actually be displayed in the current mode

	Returns:	TRUE if all went ok

	Purpose:	Returns the mode-sensitive string ptr that will actually be displayed. This is
				handy for sorting methods, etc...

***********************************************************************************************/

BOOL SGLibDisplayItem::GetActualDisplayedTextPtr(TCHAR **String, INT32 Line)
{
	ERROR3IF(String == NULL, "SGLibDisplayItem::GetActualDisplayedText with params is a real recipe for disaster");

	// We're going to do something horrible here...
	SGMiscInfo MiscInfo;
	SuperGallery *ParentGal = GetParentGallery();
	ERROR3IF(ParentGal == NULL, "SGLibDisplayItem::GetActualDisplayedText with null parent gallery");
	MiscInfo.DisplayMode = ParentGal->GetDisplayMode();
	LibDisplayType DType = GetDisplayType(&MiscInfo);

	// Work out proper text string to display and display it
	switch(DType)
	{
		case LibDisplay_FullInfo:
		case LibDisplay_SingleLineFullInfo:
			// Use the full info description - can't do this in memory
			return FALSE;

		default:
			// Just use the normal text description
			return GetNameTextPtr(String);
	}

	return FALSE;
}


/***********************************************************************************************

>	virtual INT32 SGLibDisplayItem::CompareTo(SGDisplayNode *Other, INT32 SortKey)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/4/95

	Inputs:		Other - the node to compare this node to
				SortKey - An integer identifying how to compare the items
					0 = No sorting (always returns 0)
					1 = Sort-by-name
					2 = sort by size (memory size)
					3 = sort by name length
					4 = sort by type
					Other values will return 0, unless the derived class overrides this
					method in order to provide other sort modes.

	Returns:	negative (I am lesser), 0 (we are equal), or positive (I am greater)

	Purpose:	Compares this node to the 'other' node, to determine their relative positions
				in the display tree. Returns a value which usually indicates that the other
				node should be inserted before (-1, or 0) or after (+1) this item.

	SeeAlso:	SGDisplayNode::AddItem

***********************************************************************************************/

INT32 SGLibDisplayItem::CompareTo(SGDisplayNode *Other, INT32 SortKey)
{
	switch(SortKey)
	{
		case 1:
			// Sort by name - override default and make 'FRED' and 'fred' equal...
			// Note - we should really make "'Fred'" and "Fred'" equal, since full info sorting
			// can be a bit rampant if you don't know what's going on...
			{
				BOOL FoundPtr = FALSE;

#ifndef _BATCHING
				// Normal, quick alphabetic sorting

				TCHAR *pOurName = NULL;
				TCHAR *pTheirName = NULL;
				FoundPtr = GetActualDisplayedTextPtr(&pOurName);
				if(FoundPtr)
				{
					FoundPtr = ((SGLibDisplayItem *)Other)->GetActualDisplayedTextPtr(&pTheirName);
					if(FoundPtr)
					{
						// Two TCHAR *'s...
						INT32 Value = 0;
						 
						// This little beastie (below) is the DBCS string comparison flob which we should
						// be using for the Jap's, and so forth...
						// 1 if pOurName < pTheirName
						// 2 if pOutName == pTheirName
						// 3 if pOutName > pTheirName
						Value = CompareString(LOCALE_USER_DEFAULT,
							(NORM_IGNORECASE | NORM_IGNOREKANATYPE | NORM_IGNOREWIDTH),
							pOurName, -1, pTheirName, -1);	// 1.42
						Value -= 2;

						return (Value);
					}
				}

				// Fields not found, or index file not cached in memory		
				String_256 MyName;
				String_256 ItsName;

				GetActualDisplayedText(&MyName);
				MyName.toLower();
				((SGLibDisplayItem *)Other)->GetActualDisplayedText(&ItsName);
				ItsName.toLower();

				return(MyName.CompareTo(ItsName));
#else
				// Not so quick, special alphabetic sorting (fred20 > fred10 > fred5 > fred)
				String_256 MyName;
				String_256 ItsName;

				GetActualDisplayedText(&MyName);
				MyName.toLower();
				((SGLibDisplayItem *)Other)->GetActualDisplayedText(&ItsName);
				ItsName.toLower();

				if(Library::MaxFieldCacheEntries == 50 && MyName == ItsName)
				{
					ERROR3_PF(("Identical sort entries found: '%s'", (TCHAR *)MyName));
				}

				INT32 FSize = GetFileSize(NULL);
				if(FSize == 0)
				{
					ERROR3_PF(("Filesize <= 0 found: '%s'", (TCHAR *)MyName));
				}

				INT32 MRCount = MyName.Length() - 1;
				BOOL MFound = FALSE;		
				TCHAR TheChar = ((TCHAR *)MyName)[MRCount];

				while(((TheChar == ' ') || (TheChar >= '0' && TheChar <= '9')) && MRCount > 0)
				{
					if(TheChar != ' ')
						MFound = TRUE;
					TheChar = ((TCHAR *)MyName)[--MRCount];
				}
				
				INT32 IRCount = ItsName.Length() - 1;
				BOOL IFound = FALSE;
				TheChar = ((TCHAR *)ItsName)[IRCount];

				while(((TheChar == ' ') || (TheChar >= '0' && TheChar <= '9')) && IRCount > 0)
				{
					if(TheChar != ' ')
						IFound = TRUE;
					TheChar = ((TCHAR *)ItsName)[--IRCount];
				}

				if(IFound && MFound)
				{
					String_256 MyLeft;
					String_256 ItsLeft;

					MyName.Left(&MyLeft, MRCount + 1);
					ItsName.Left(&ItsLeft, IRCount + 1);
				
					if(MyLeft == ItsLeft)
					{
						String_256 MyRight;
						String_256 ItsRight;

						MyName.Right(&MyRight, MyName.Length() - MRCount - 1);
						ItsName.Right(&ItsRight, ItsName.Length() - IRCount - 1);
				
						INT32 Me = _ttoi((TCHAR *)MyRight);
						INT32 It = _ttoi((TCHAR *)ItsRight);
					
						if(Me > It)
							return 1;
						else if(Me < It)
							return -1;
					}
				}

				return(MyName.CompareTo(ItsName, FALSE));
#endif
			}
		case 2:
			// Sort by size
			{
				Library *Lib = GetParentLibrary();
				return(GetFileSize(Lib) - ((SGLibDisplayItem *)Other)->GetFileSize(Lib));
			}

		case 3:
			// Sort by name length
			{
				String_256 MyName;
				String_256 ItsName;

				GetActualDisplayedText(&MyName);
				((SGLibDisplayItem *)Other)->GetActualDisplayedText(&ItsName);

				return (MyName.Length() - ItsName.Length());
			}
			break;

		case 4:
			// Sort by file type
			{
				PathName MyPath;
				PathName ItsPath;

				GetFileName(&MyPath);
				((SGLibDisplayItem *)Other)->GetFileName(&ItsPath);
	
				Error::ClearError();

				if(MyPath.IsValid() && ItsPath.IsValid())
				{
					String_8 MyType(MyPath.GetType());
					String_8 ItsType(ItsPath.GetType());

					return(MyType.CompareTo(ItsType, FALSE));
				}
			}
			break;
	}
	return (SGDisplayNode::CompareTo(Other, SortKey));
}



/***********************************************************************************************

>	virtual void SGLibDisplayItem::DragWasReallyAClick(SGMouseInfo *Mouse, SGMiscInfo *MiscInfo)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/4/95

	Inputs:		Mouse - The mouse info passed to the original click handler
				MiscInfo - The misc info passed to the original click handler

	Purpose:	Handles a mouse click event. This is a callback function - drags of
				bitmaps from galleries will call this function back if the drag turns
				out to just be a click.

	SeeAlso:	SGFillsItem::HandleEvent; GalleryFillsDragInfo::OnClick

***********************************************************************************************/

void SGLibDisplayItem::DragWasReallyAClick(SGMouseInfo *Mouse, SGMiscInfo *MiscInfo)
{
	// Just get default selection action to be applied for this click
	DefaultClickHandler(Mouse, MiscInfo, TRUE);
}


/***********************************************************************************************

>	SGLibGroup::SGLibGroup(SuperGallery *ParentGal,
								Document *ParentDoc = NULL, Library *ParentLib = NULL,
								String_64 *Text)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/4/95

	Inputs:		ParentGal - points to the SuperGallery object which 'owns' this node
				ParentDoc - NULL, or a pointer to the document this group references
				ParentLib - NULL, or a pointer to the library this group references

	Purpose:	SGLibGroup constructor. Initialises the Group's parent pointers to
				point at its parent(s). Note that generally speaking, one of ParentDoc,
				ParentLib will be NULL, and the other will be non-NULL.

	Notes:		This constructor does nothing - jusat passes the call on to the baseclass
				constructor of identical proportions.

	SeeAlso:	SGDisplayGroup::SGDisplayGroup

***********************************************************************************************/

SGLibGroup::SGLibGroup(SuperGallery *ParentGal,
							Document *ParentDoc, Library *ParentLib)
				: SGDisplayGroup(ParentGal, ParentDoc, ParentLib)
{
	// Library groups can be selected
	Flags.CanSelect = TRUE;

	SetVirtualisedState(FALSE);
}

/***********************************************************************************************

>	void SGLibGroup::ReadGroupTitle(void)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/4/95
	Inputs:		-
	Outputs:	-
	Purpose:	Since we now store the group text with the group, we have to overwrite
				the normal ReadGroupTitle member. This new one simply does nothing.
				
***********************************************************************************************/

void SGLibGroup::ReadGroupTitle(void)
{
}


/***********************************************************************************************

>	virtual BOOL SGLibGroup::HandleEvent(SGEventType EventType, void *EventInfo, SGMiscInfo *MiscInfo)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/4/95

	Inputs:		EventType - An enumerated value describing what type of event is to be processed

				EventInfo - A structure describing the event (may be NULL). The exact thing
							pointed at by this pointer depends upon the event type:

							MonoOn
							Event				Thing EventInfo points at
							SGEVENT_FORMAT		(SGFormatInfo *)
							SGEVENT_REDRAW		(SGRedrawInfo *)
							SGEVENT_MOUSECLICK	(SGMouseInfo *)
							MonoOff
				Use the provided SGDisplayNode::Get[Format]Info() inlines to retrieve this
				information - they provide useful error/type checking, and hide the cast

				MiscInfo - always provided. Contains a few useful bits of info that may be
				needed for all event types.
	Outputs:	

	Returns:	TRUE if the event was handled successfully
				FALSE if it was not

	Purpose:	Handles a library group event - thumbnail resizing, etc...
	SeeAlso:	SGDisplayNode::HandleEvent

***********************************************************************************************/

BOOL SGLibGroup::HandleEvent(SGEventType EventType, void *EventInfo, SGMiscInfo *MiscInfo)
{
	switch (EventType)
	{
		case SGEVENT_THUMBMSG:
			{
				ThumbMessage *Msg = (ThumbMessage *) EventInfo;
				if(Msg != NULL)
				{
					switch (Msg->State)
					{
						// Resize the library thumbnail cache
						case ThumbMessage::ThumbState::CACHESIZECHANGED:
							{
								Library *TheLib = GetParentLibrary();

								if(TheLib == NULL)
								{
									ERROR3("SGLibGroup::HandleEvent Library == NULL for library group - bad !!!");
								}
								else
								{
									// Thumbnail cache associated with library
									SGThumbs *Thumbs = TheLib->Thumbnails;

									if(Thumbs != NULL)
									{
										// Rip the details out of the old cache
										PathName OldPath(*(Thumbs->GetDirectory()));
										SGLibType OldType = Thumbs->GetType();
										SGThumbSize OldSize;
										Thumbs->GetSize(&OldSize);

										// Just check the new cache will know it's maximum size
										SGThumbs::MaxThumbnails = Msg->NewSize;
										
										// Kill off the old cache and reclaim all the memory
										delete Thumbs;

										// Create the new cache										
										Thumbs = new SGThumbs(&OldPath, OldType, OldSize);

										// Assign the new cache to the library
										TheLib->Thumbnails = Thumbs;
									}
								}
							}
					 		break;

							case ThumbMessage::ThumbState::KILLCACHE:
							{
								Library *TheLib = GetParentLibrary();

								if(TheLib == NULL)
								{
									ERROR3("SGLibGroup::HandleEvent Library == NULL for library group - bad !!!");
								}
								else
								{
									// Thumbnail cache associated with library
									SGThumbs *Thumbs = TheLib->Thumbnails;

									if(Thumbs != NULL)
									{
										if(Thumbs->GetType() == Msg->Type)
										{
											// Reclaim thumbnail cache memory
											Thumbs->DeleteThumbnails();
										}
									}
								}
							}
							break;
					}
				}
			}
			break;
	}

	// Pass back the event
	return SGDisplayGroup::HandleEvent(EventType, EventInfo, MiscInfo);
}



/********************************************************************************************

>	virtual BOOL SGLibGroup::GetBubbleHelp(DocCoord *MousePos, String_256 *Result)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/6/95

	Inputs:		MousePos - The current mouse position. This will generally be expected
				to lie inside this item's FormatRect. With it, this item can provide
				help on specific areas of an item.

	Outputs:	On exit, if the return value is TRUE, the string pointed at by Result
				will contain a bubble help string for this item

	Returns:	TRUE if it filled in the string, FALSE if it did not
				
	Purpose:	Called by the parent gallery when bubble help is needed. The parent
				gallery will do a hit test to determine which node contains the pointer,
				and will then ask that node to supply bubble/status-line help.
				
	Notes:		The base class returns FALSE (i.e. provides no help)
				If you can provide help, then override the base class method to do so.

	SeeAlso:	SGDisplayNode::GetStatusLineHelp

********************************************************************************************/

BOOL SGLibGroup::GetBubbleHelp(DocCoord *MousePos, String_256 *Result)
{
	ERROR3IF(MousePos == NULL || Result == NULL, "Invalid NULL params");

	return FALSE;
}


	
/********************************************************************************************

>	virtual BOOL SGLibGroup::GetStatusLineHelp(DocCoord *MousePos, String_256 *Result)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/1/95

	Inputs:		MousePos - The current mouse position. This will generally be expected
				to lie inside this item's FormatRect. With it, this item can provide
				help on specific areas of an item.

	Outputs:	On exit, if the return value is TRUE, the string pointed at by Result
				will contain a status line help string for this item

	Returns:	TRUE if it filled in the string, FALSE if it did not
				
	Purpose:	Called by the parent gallery when status line help is needed. The parent
				gallery will do a hit test to determine which node contains the pointer,
				and will then ask that node to supply bubble/status-line help.
				
	Notes:		The base class returns FALSE (i.e. provides no help)
				If you can provide help, then override the base class method to do so.

	SeeAlso:	SGDisplayNode::GetBubbleHelp

********************************************************************************************/

BOOL SGLibGroup::GetStatusLineHelp(DocCoord *MousePos, String_256 *Result)
{
	ERROR3IF(MousePos == NULL || Result == NULL, "Invalid NULL params");

	if(MousePos->x <= GROUP_BAR_ICON_WIDTH)
	{
		if(Flags.Folded)
			/* "Click to open this section of the gallery"; */
			Result->MakeMsg(_R(IDS_LIBRARY_FOLDER_CLICK_TO_OPEN));
		else
			/* "Click to close this section of the gallery"; */
			Result->MakeMsg(_R(IDS_LIBRARY_FOLDER_CLICK_TO_CLOSE));
	}
	else
	{
		if(Flags.Selected)
		{
			/* "Ctrl-Click to deselect" */
			Result->MakeMsg(_R(IDS_LIBRARY_SECTION_CLICK_SELECTED));
			*Result += String_8(_R(IDS_SGFONTS_STATUS_LINE_SEP)); // "; "
		}
		else if(Flags.CanSelect)
		{
			/* "Click to select" */
			Result->MakeMsg(_R(IDS_LIBRARY_SECTION_CLICK_DESELECTED));
			*Result += String_8(_R(IDS_SGFONTS_STATUS_LINE_SEP)); // "; "
		}

		String_256 DClick;
		if(Flags.Folded)
			/* "Double click to open this section of the gallery"; */
			DClick.MakeMsg(_R(IDS_LIBRARY_SECTION_DCLICK_TO_OPEN));
		else
			/* "Double click to close this section of the gallery"; */
			DClick.MakeMsg(_R(IDS_LIBRARY_SECTION_DCLICK_TO_CLOSE));

		*Result += DClick;
	}
	return(TRUE);
}


/********************************************************************************************

>	virtual BOOL SGLibGroup::CanVirtualise(void);

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/1/95

	Returns:	TRUE if it can
				
	Purpose:	Some groups shouldn't virtualise out to disk (the installed font group for
				example). Such groups should override this and return FALSE.

********************************************************************************************/

BOOL SGLibGroup::CanVirtualise(void)
{
	return SGLibGroup::LibraryVirtualisingEnabled;
}

/********************************************************************************************

>	virtual BOOL SGLibGroup::DeVirtualise(void);

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/1/95

	Returns:	TRUE if it devirtualised back in OK
				
	Purpose:	Virtualise a group back into memory.

********************************************************************************************/

BOOL SGLibGroup::DeVirtualise(void)
{
	if(!IsVirtualised())
		return TRUE;

	// On older machines this can take a couple of seconds, so we need some feedback...
	Progress ProgMsg(_R(IDS_GALLERY_PREPARE_FOR_UNFOLD), -1/*, FALSE*/);
	ParentLibrary->ProgressBar = &ProgMsg; // ::Init will call update if this is non-null

	INT32 Count = ParentLibrary->CreateItems();

#ifdef _DEBUG
	ForceRedrawOfMyselfAndChildren();
#endif

	ParentLibrary->ProgressBar = NULL;

	return (Count > 0);
}


/***********************************************************************************************

>	virtual BOOL SGLibDisplayItem::ShouldDownloadThumb()

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/02/97

	Inputs:		-
	Outputs:	-
	Returns:	TRUE if we should download the thumbnail for this item, false otherwise

	Purpose:	Used to decide whether or not to (re)start a thumbnail download

***********************************************************************************************/

	BOOL SGLibDisplayItem::ShouldDownloadThumb()
	{
		Library* pLibrary = GetParentLibrary();
		ERROR2IF(!pLibrary, FALSE, "Illegal NULL pointer");
		// Check if we belong to a web folder
		String_256 thumbPath;
		if (pLibrary->IsWebLibrary())
		{
			GetThumbFileName(&thumbPath);
			return (_access((TCHAR*) thumbPath, 0) == -1 && !IsDownloadingThumb() && 
				(!((LibraryGallery*) GetParentGallery())->IsThumbDownloadingSuspended()) &&
				!OpThumbDownload::GetLastError(this));
		}
		else
		{
			return FALSE;
		}
	}
		

	/***********************************************************************************************

>	virtual BOOL SGLibDisplayItem::DownloadThumbnail()

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/02/97

	Inputs:		-
	Outputs:	-
	Returns:	TRUE the download was started succesfully, false otherwise

	Purpose:	Starts to download the item's thumbnail

***********************************************************************************************/

BOOL SGLibDisplayItem::DownloadThumbnail()
{
	nPercentageDownloaded = 0;
	OpDescriptor* pOpDesc =	OpDescriptor::FindOpDescriptor(CC_RUNTIME_CLASS(OpThumbDownload));
	if (pOpDesc != NULL)
	{
		ThumbDownloadParam* Param = new ThumbDownloadParam;
		ERROR3IF(!Param, "Memory allocation error");
		Param->pItem = this;
		Param->type = GetParentLibrary()->GetType();
		// Invoke the operation
		pOpDesc->Invoke((OpParam*) Param);
		if (Param->bSuccess)
		{
			pDownloadOp = Param->pOp;
			bIsDownloadingThumb = TRUE;
			return TRUE;
		}
	}
	return FALSE;
}





/***********************************************************************************************

>	virtual void SGLibDisplayItem::OnThumbDownloadComplete()

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/02/97

	Inputs:		-
	Outputs:	-
	Returns:	-

	Purpose:	Should be called when the thumbnail download is over  - it will force
					a redraw among other things

***********************************************************************************************/

void SGLibDisplayItem::OnThumbDownloadComplete()
{
	bIsDownloadingThumb = FALSE;
	ForceRedrawOfMyself();
	GetParentGallery()->PaintListNow();
	pDownloadOp = NULL;
}


/***********************************************************************************************

>	virtual void SGLibDisplayItem::OnThumbDownloadProgress(const INT32 nPercentageCompleted)

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/02/97

	Inputs:		-
	Outputs:	-
	Returns:	-

	Purpose:	Called on thumbnail download progress - it will update the thumbnail's 
				placeholder. NB: the background is not erased to prevent flickering

***********************************************************************************************/

void SGLibDisplayItem::OnThumbDownloadProgress(const INT32 nPercentageCompleted)
{
	nPercentageDownloaded = nPercentageCompleted;
	ForceRedrawOfMyself(FALSE);
	GetParentGallery()->PaintListNow();
}



/***********************************************************************************************

>	virtual void SGLibDisplayItem::DrawPlaceholder(RenderRegion *Renderer,
								SGMiscInfo *MiscInfo, DocRect *Rectangle, BOOL Background)	
	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com>
	Created:	128/4/97
	
	Inputs:		Renderer  - Pointer to RenderRegion in which to plot the bitmap rect
				MiscInfo  -	Miscellaneous info as passed to the redraw handler by the sgallery code
				Rectangle - Pointer to a DocRect the size and position of where the bitmap would
							be positioned normally
				Background - If true, draw a background redrawing, not plotted proper thumbnail rectangle
	Outputs:

	Returns:

	Purpose:	Draws a thumbnail placeholder if the actual thumbnail is not available. If the thumb is 
					downloading, it displays the percentage downloaded so far
				
	Notes:		If the OS is not Windows this function will simply call DrawNullBitmapRect

	SeeAlso:	SGLibDisplayItem::HandleRedraw

***********************************************************************************************/

void SGLibDisplayItem::DrawPlaceholder(RenderRegion *Renderer, SGMiscInfo *MiscInfo,
						DocRect *Rectangle, BOOL Background)
{
	if (Renderer == NULL || MiscInfo == NULL || Rectangle == NULL)
	{
		ERROR3("Illegal NULL pointer");
		return;
	}
#ifdef _WIN32
	if (Background)
	{		
		DocColour NullBitmapColour(230L, 230L, 230L);
		Renderer->SetFillColour(NullBitmapColour);
		GridLockRect(MiscInfo, Rectangle);
		Renderer->DrawRect(Rectangle);
		INT32 OnePixel = MiscInfo->PixelSize;
		Renderer->SetLineWidth(0);
		Renderer->SetLineColour(DocColour(COLOUR_TRANS));
		Renderer->SetFillColour(DocColour(COLOUR_BLACK));
		DocRect TempRect(*Rectangle);					// Left
		TempRect.hi.x = TempRect.lo.x + OnePixel;
		Renderer->DrawRect(&TempRect);
		TempRect = *Rectangle;							// Top
		TempRect.lo.y = TempRect.hi.y - OnePixel;
		Renderer->DrawRect(&TempRect);
		TempRect = *Rectangle;							// Right
		TempRect.lo.x = TempRect.hi.x - OnePixel;
		Renderer->DrawRect(&TempRect);
		TempRect = *Rectangle;							// Bottom
		TempRect.hi.y = TempRect.lo.y + OnePixel;
		Renderer->DrawRect(&TempRect);
		if (IsDownloadingThumb())
		{
			String_32 strLabel(GetStringField(0, _R(IDS_LOADINGTHUMB)));
			DocRect rcText; 
			Renderer->DrawFixedSystemText(&strLabel, rcText, FORMAT_NOPREFIX | FORMAT_CALCRECT | FORMAT_SINGLELINE | FORMAT_CENTER | FORMAT_VCENTER);
			if (rcText.Width() >= Rectangle->Width()) // we have to draw multiline
			{
				camSprintf((TCHAR*) strLabel, _T("%s%d%%"), (TCHAR*) (String_256) GetStringField(1, _R(IDS_LOADINGTHUMB)), nPercentageDownloaded);
				// DrawFixedSystemText can't center multiline text, so we'll have to adjust the rect
				Renderer->DrawFixedSystemText(&strLabel, rcText, FORMAT_NOPREFIX | FORMAT_CALCRECT);
				DocRect rcAdjusted = *Rectangle;
				rcAdjusted.hi.y -= ((rcAdjusted.hi.y - rcAdjusted.lo.y) - (rcText.hi.y - rcText.lo.y))/2;
				Renderer->DrawFixedSystemText(&strLabel, rcAdjusted, FORMAT_NOPREFIX | FORMAT_CENTER);
			}
			else
			{
				camSprintf((TCHAR*) strLabel, _T("%s%d%%"), (TCHAR*) (String_256) GetStringField(2, _R(IDS_LOADINGTHUMB)), nPercentageDownloaded);
				Renderer->DrawFixedSystemText(&strLabel, *Rectangle, FORMAT_NOPREFIX | FORMAT_SINGLELINE | FORMAT_CENTER | FORMAT_VCENTER);
			}
		}
	}
	else
	{
		DrawNullBitmapRect(Renderer, MiscInfo, Rectangle, Background);
	}
#else
	DrawNullBitmapRect(Renderer, MiscInfo, Rectangle, Background);
#endif
}



/********************************************************************************************

>	BOOL SGLibDisplayItem::GetThumbFileName(PathName *Result)  (WEBSTER)

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/12/96

	Outputs:	Result - will be filled in with the full filename of the object's thumb file
			
	Returns:	FALSE if it failed, else TRUE

	Purpose:	To find the corresponding filename for this object's thumb

********************************************************************************************/


BOOL SGLibDisplayItem::GetThumbFileName(String_256* path)
{
	PathName filePath;
	GetFileName(&filePath);
	Library* pLibrary = GetParentLibrary();
	if (!pLibrary)
	{
		ERROR3("Ilegal NULL pointer!");
		return FALSE;
	}
	if (!pLibrary->IsWebLibrary()) 
	{
		ERROR3("This function should only be called for web folders");
		return FALSE;
	}
	PathName indexPath(*(pLibrary->ReturnIndexLocation()));
	String_256 thumbPath(indexPath.GetLocation());
	thumbPath += filePath.GetFileName(FALSE);
	thumbPath += _T(".png"); // thumbnails for web files will always be PNGs
	*path = thumbPath;
	return TRUE;
}





