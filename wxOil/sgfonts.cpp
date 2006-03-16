// $Id: sgfonts.cpp 662 2006-03-14 21:31:49Z alex $
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
// SGFonts.cpp - Fonts SuperGallery classes - FontsSGallery and SGDisplayFonts

/* The below code is riddled with DC's and OIL-specifc Font calls... Hence the location */

#include "camtypes.h"

#ifndef EXCLUDE_GALS
#include "sgfonts.h"

#include "app.h"		// For GetApplication()
//#include "galstr.h"
#include "convert.h"	// For Millipoint convs
#include "ccdc.h"		// For render-into-dialogue support
#include "fillval.h"
#include "grnddib.h"
#include "bitmap.h"		// For bitmap stuff !
#include "bitmpinf.h"	// For more bitmap stuff
#include "nodebmp.h"	// For bitmap drawing stuff
#include "progress.h"	// For hourglass stuff
//#include "galres.h"
#include "sgallery.h"
#include "fixmem.h"
#include "wbitmap.h"
//#include "richard.h"	
#include "sgscan.h"
#include "sgscanf.h"
#include "sglib.h"
#include "thumb.h"
#include "thumbmsg.h"
#include "fontpgen.h"	// For Font thumbnail generation
#include "sginit.h"
#include "basestr.h"
//#include "resource.h"
#include "sgliboil.h"	// For various stuff
#include "dlgmgr.h"
#include "camelot.h"
#include "sgindgen.h"
#include "product.h"
#include "fontman.h"
#include "sgmenu.h"
#include "fontbase.h"
#include "stockcol.h"
#include "atmfonts.h"
#include "atminst.h"
#include "txtattr.h"	// For apply
#include "attrmgr.h"	// For apply
#include "textinfo.h"	// For apply
#include "nodetext.h"	// For status line stuff
#include "dragmgr.h"	// For status line stuff
#include "sglbase.h"	// For LibraryGallery
#include "sgdfonts.h"	// Font dragging stuff
#include "fontlist.h"	// For checking font usage on deinstall
//#include "richard2.h"	// New reosurce strings
#include "dibutil.h"
#include "keypress.h"
#include "camnet.h"
#include "inetop.h"
//#include "webster.h"
//#include "resimmap.h"	// For _R(IDS_LARGEFONTDISPLAYSTRING)

#include "helpuser.h"	//For the help button
//#include "xshelpid.h"	//For the help button
//#include "helppath.h"
#include "resdll.h"

#include "ndoptmz.h"

using namespace InetUtils;

// Implement the dynamic class bits...
CC_IMPLEMENT_DYNCREATE(FontsSGallery, LibraryGallery)
CC_IMPLEMENT_DYNAMIC(SGDisplayPreviewFonts, SGDisplayItem)
CC_IMPLEMENT_DYNAMIC(SGTTFItem, SGDisplayPreviewFonts)
CC_IMPLEMENT_DYNAMIC(SGATMItem, SGDisplayPreviewFonts)
CC_IMPLEMENT_DYNCREATE(OpDisplayFontsGallery, Operation)
CC_IMPLEMENT_DYNAMIC(SGFontsGroup, SGDisplayGroup)
CC_IMPLEMENT_DYNAMIC(SGLibFontItem, SGLibDisplayItem)

// This line mustn't go before any CC_IMPLEMENT_... macros
#define new CAM_DEBUG_NEW

// If you want more info on the installed fonts, uncomment this next line
//#define SGFONTS_FULLINFO_FOR_INSTALLED_FONTS

// Windows 95 does things slightly differently to the rest... use the next line to disable it
//#define STOP_WINDOWS95_FONT_INSTALLS

/**************************************************************************/

// NB: The default text for the installed font previews is in 'richard.rc'

// The size of bitmaps (in pixels) to generate for the installed fonts

// New larger size normal font
const INT32 SGF_NORMAL_X = 180;
const INT32 SGF_NORMAL_Y = 26;

const INT32 SGF_FULL_X = 160;
const INT32 SGF_FULL_Y = 12;

const INT32 SGF_SMALL_X = 28;
const INT32 SGF_SMALL_Y = 16;

// Space for bmp to the left of each item
const INT32 SGF_TYPE_WIDTH = 14;

// BPP of thumbnails generated 'on the fly' (only 8 and 32 are known to work)
const INT32 SGF_BPP = 8;

// Number of pixels around thumbnail bitmap for selection rectangle
// This has only be tested with a value of 3, others may (should) work.
const INT32 SGF_SURROUND = 3;

/**************************************************************************/

// Initialise the statics

// Default location of the fonts library index
#ifdef _DEBUG
//	String_256 FontsSGallery::DefaultLibraryPath = TEXT("\\\\deepthought\\camelotcd\\notneeded\\fonts");
	String_256 FontsSGallery::DefaultLibraryPath = TEXT("\\\\jimpc\\corelxra\\fonts");
#else
	String_256 FontsSGallery::DefaultLibraryPath = TEXT("D:\\fonts");
#endif

// Display mode - required by callback function (possibly)
INT32 SGDisplayPreviewFonts::DMode = 0;

// Pointer to the gallery being described - required by callback function
FontsSGallery *FontsSGallery::ThisGallery = NULL;

// For enumeration of non-family fonts
BOOL FontsSGallery::AddedFonts = FALSE;

// Are we installing or deinstalling a font ?
BOOL FontsSGallery::DontUpdate = FALSE;

// For keeping the display modes constant when next loaded
INT32 FontsSGallery::DefaultDisplayMode = 0;

// For keeping the sort keys constant when next loaded (default == sort alphabetically)
// 1st sort key = DefaultSortKeys & 0x7f
// 2nd sort key = ((DefaultSortKeys>>8) & 0x7f)
// 1st sort key reversed = ((DefaultSortKeys>>7) & 0x01)==1
// 2nd sort key reversed = ((DefaultSortKeys>>15) & 0x01)==1
// So 0 means no sorting at all
// and 1 means sort the gallery alphabetically
UINT32 FontsSGallery::DefaultSortKeys = 1;

// The installed fonts group
SGFontsGroup *FontsSGallery::InsGroup = NULL;

// Deinstalling fonts option
BOOL FontsSGallery::DeleteTTFandFOTfiles = TRUE;

// Use this as the large display mode string (for the installed fonts)
//Changed by Graham 1/7/97 so we use "AaBbCc" instead of the product name
String_256 FontsSGallery::LargeDisplayString=String_256();

// Used so that we don't add enummed fonts that we don't mean to
FontClass FontsSGallery::EnumFontClass = FC_UNDEFINED;

// Quiet status of gallery
BOOL FontsSGallery::QuietStatus = FALSE;

// Flag for stopping multiple warnings about dragging between sections...
BOOL FontsSGallery::WarnAboutSectionDragging = FALSE;

// Flag for stopping multiple warnings when ATM isn't installed for dragging between sections...
BOOL FontsSGallery::WarnAboutMultipleATMDragging = FALSE;

/***********************************************************************************************

>	SGFontsGroup::SGFontsGroup(SuperGallery *ParentGal,
								Document *ParentDoc = NULL, Library *ParentLib = NULL,
								String_64 *Text)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/2/94

	Inputs:		ParentGal - points to the SuperGallery object which 'owns' this node
				ParentDoc - NULL, or a pointer to the document this group references
				ParentLib - NULL, or a pointer to the library this group references
				TText - Text to use as the group's title

	Purpose:	SGFontsGroup constructor. Initialises the Group's parent pointers to
				point at its parent(s). Note that generally speaking, one of ParentDoc,
				ParentLib will be NULL, and the other will be non-NULL.

	Notes:		This constructor does nothing - just passes the call on to the baseclass
				constructor of identical proportions.

	SeeAlso:	SGDisplayGroup::SGDisplayGroup

***********************************************************************************************/

SGFontsGroup::SGFontsGroup(SuperGallery *ParentGal,
							Document *ParentDoc, Library *ParentLib, String_64 *TText)
				: SGDisplayGroup(ParentGal, ParentDoc, ParentLib)
{
	if(TText != NULL)
		TitleText = *TText;
	else
		TitleText.MakeMsg(_R(IDS_FONTS_GROUP));

	SetVirtualisedState(FALSE);
}

/***********************************************************************************************

>	void SGDisplayGroup::ReadGroupTitle(void)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/2/94
	Inputs:		-
	Outputs:	-
	Purpose:	Since we now store the group text with the group, we have to overwrite
				the normal ReadGroupTitle member. This new one simply does nothing.
				
***********************************************************************************************/

void SGFontsGroup::ReadGroupTitle(void)
{
}


/***********************************************************************************************

>	virtual BOOL SGFontsGroup::HandleEvent(SGEventType EventType, void *EventInfo, SGMiscInfo *MiscInfo)

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

	Purpose:	Handles an Installed font group event - thumbnail resizing, etc...
	SeeAlso:	SGDisplayNode::HandleEvent

***********************************************************************************************/

BOOL SGFontsGroup::HandleEvent(SGEventType EventType, void *EventInfo, SGMiscInfo *MiscInfo)
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
						// Installed fonts cache needs resizing ?
						case ThumbMessage::ThumbState::CACHESIZECHANGED:
							{
								// We know our parent is the fonts gallery....
								FontsSGallery *TheGal = (FontsSGallery *)GetParentGallery();

								if(TheGal == NULL)
								{
									ERROR3("SGFontsGroup::HandleEvent Library == NULL for installed fonts group - bad !!!");
								}
								else
								{
									// Thumbnail cache associated with group
									SGThumbs *Thumbs = TheGal->InsCache;

									if(Thumbs != NULL)
									{
										// Rip the details out of the old cache
										SGThumbSize OldSize;
										Thumbs->GetSize(&OldSize);

										// Just check the new cache will know it's maximum size
										SGThumbs::MaxThumbnails = Msg->NewSize;
										
										// Kill off the old cache and reclaim all the memory
										delete Thumbs;

										// Allocate a new thumbnail cache for installed fonts
										// NULL directory is a special case...
										Thumbs = new SGThumbs(NULL, SGLib_Font, OldSize);

										// Assign the new cache to the library
										TheGal->InsCache = Thumbs;
									}
								}
							}

					 		break;

						case ThumbMessage::ThumbState::KILLCACHE:
							{
								// We know our parent is the fonts gallery....
								FontsSGallery *TheGal = (FontsSGallery *)GetParentGallery();

								if(TheGal == NULL)
								{
									ERROR3("SGFontsGroup::HandleEvent Library == NULL for installed fonts group - bad !!!");
								}
								else
								{
									// Thumbnail cache associated with group
									SGThumbs *Thumbs = TheGal->InsCache;

									if(Thumbs != NULL)
									{
										if(Msg->Type == SGLib_Font)
										{
											// Reclaim installed fonts thumbnail cache memory
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

>	virtual BOOL SGFontsGroup::GetBubbleHelp(DocCoord *MousePos, String_256 *Result)

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

BOOL SGFontsGroup::GetBubbleHelp(DocCoord *MousePos, String_256 *Result)
{
	ERROR3IF(MousePos == NULL || Result == NULL, "Invalid NULL params");

	return FALSE;
}


	
/********************************************************************************************

>	virtual BOOL SGFontsGroup::GetStatusLineHelp(DocCoord *MousePos, String_256 *Result)

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

BOOL SGFontsGroup::GetStatusLineHelp(DocCoord *MousePos, String_256 *Result)
{
	ERROR3IF(MousePos == NULL || Result == NULL, "Invalid NULL params");

	if(MousePos->x <= 16500)
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

>	virtual BOOL SGFontsGroup::CanVirtualise(void);

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/1/95

	Returns:	FALSE, since the installed fonts group can't be virtualised...

********************************************************************************************/

BOOL SGFontsGroup::CanVirtualise(void)
{
	return FALSE;
}






/***********************************************************************************************

>	SGDisplayPreviewFonts::SGDisplayPreviewFonts()

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/1/95 (base generated in sgbase.cpp)
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	SGDisplayPreviewFonts constructor
				DON'T call this constructor. It ERROR3's. Call the other constructor

***********************************************************************************************/

SGDisplayPreviewFonts::SGDisplayPreviewFonts()
{
	FontBitmap = NULL;
	FontDescription = "";
	CachedLogFont = NULL;
	ID = 0;
	IntLeading = 0;
	Type = FC_UNDEFINED;
	Invalid = FALSE;
}

/***********************************************************************************************

>	SGDisplayPreviewFonts::~SGDisplayPreviewFonts()

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/1/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	SGDisplayPreviewFonts destructor
				General memory and object tidy up - all thumbnails are vaped...

***********************************************************************************************/

SGDisplayPreviewFonts::~SGDisplayPreviewFonts()
{
	// The FontBitmap is also referenced in the thumbnail cache, and that's where its
	// memory is free'd
	FontBitmap = NULL;
	FontDescription = "";

	if(CachedLogFont != NULL)
	{
		delete CachedLogFont;
		CachedLogFont = NULL;
	}

	ID = 0;
	IntLeading = 0;
}



/********************************************************************************************

>	KernelBitmap *SGDisplayPreviewFonts::GetDisplayedKernelBitmap(SGMiscInfo *MiscInfo, BOOL Background)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/04/95

 	Inputs:		MiscInfo - misc info given to the gallery redraw code
				Background - if false then redrawing in foreground - only return bmp if in cache
	Outputs:
	
	Returns:	A pointer to the KernelBitmap which this Display Item is used to display,
				or NULL if there is no thumbnail (e.g. a colour library has no thumbs),
				or the thumbnail failed to load/cache
			
	Purpose:	To find out the KernelBitmap this object is responsible for displaying

********************************************************************************************/

KernelBitmap *SGDisplayPreviewFonts::GetDisplayedKernelBitmap(SGMiscInfo *MiscInfo, BOOL Background)
{
	// Invalid font - don't display thumbnail, or allow it to be dragged...
	if(Invalid)
		return NULL;

	LibDisplayType DType = GetDisplayType(MiscInfo);

	if(DType == LibDisplay_JustText)
		return NULL;

	FontsSGallery *FSGallery = FontsSGallery::ThisGallery;
	ERROR3IF(FSGallery == NULL, "SGDisplayPreviewFonts::GetDisplayedKernelBitmap, FontsSGallery::ThisGallery is NULL");

	// We haven't got the bmp from the cache yet
	FontBitmap = NULL;

	if(FSGallery->InsCache != NULL)
	{
		// Depending on 'Background' return the thumbnail from the cache (or generate a new one and add
		// it to the cache) or return NULL if we're in a hurry...
		FSGallery->InsCache->SetSize((SGThumbSize)MiscInfo->DisplayMode);
		if(!FSGallery->InsCache->GetThumbnail(ID, Background, &FontBitmap, NULL, (LP_SGTHUMBS_MOREINFO)this))
			FontBitmap = NULL;
	}

	return FontBitmap;
}
					 
/***********************************************************************************************

>	virtual void SGDisplayPreviewFonts::CalculateMyRect(SGFormatInfo *FormatInfo,
												SGMiscInfo *MiscInfo)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/1/95 (base generated in sgbase.cpp)

	Inputs:		FormatInfo - The formatting info from which to calculate my position/size
				MiscInfo - As usual, the useful misc info struct

	Outputs:	member variable FormatRect - is returned filled in with the size/position of
				this PreviewFonts item's display area. This is dependent upon the current display
				mode and format state

				FormatInfo will be updated as a result of the formatting operation

	Purpose:	Shared code for PreviewFonts items to calculate where they will appear in the
				grand scheme of things

	Notes:
	Scope:		private (for use of SGDisplayPreviewFonts class only)

***********************************************************************************************/

void SGDisplayPreviewFonts::CalculateMyRect(SGFormatInfo *FormatInfo, SGMiscInfo *MiscInfo)
{
	UINT32 XSize = SG_InfiniteWidth;
	UINT32 YSize = SG_DefaultLargeIcon;
	
	DMode =	MiscInfo->DisplayMode;

	// Get the relevant size details for the current mode (pixels)
	GetThumbnailDetails(DMode, &XSize, &YSize, NULL);

	INT32 OnePixel  = (INT32) DevicePixels(MiscInfo, 1);

	// Convert pixel values into millipoint measurements
	switch(DMode)
	{
		case 1:
			// Full info mode requires more text description space
			//XSize = SG_InfiniteWidth;
			XSize = (XSize * OnePixel) + (SG_DefaultNameText * 2);
			break;

		case 3:
			// No description requires no text description space
			XSize = (XSize * OnePixel) + (SGF_SURROUND * OnePixel * 2) + (SGF_TYPE_WIDTH * OnePixel);
			break;

		case 4:
			// Just text
			YSize = GridLock(MiscInfo, 18 * OnePixel);
			XSize = GridLock(MiscInfo, SG_DefaultNameText);
			CalculateFormatRect(FormatInfo, MiscInfo, XSize, YSize);
			return;

		default:
			// Normal and Small modes just have the font name at present
			XSize = (XSize * OnePixel) + (INT32)((float) SG_DefaultNameText * 1.1);
			break;
	}

	YSize = (YSize + (SGF_SURROUND * 2)) * OnePixel;

	// Snap to gridpoints
	XSize = GridLock(MiscInfo, XSize);
	YSize = GridLock(MiscInfo, YSize);

	CalculateFormatRect(FormatInfo, MiscInfo, XSize, YSize);
}

/***********************************************************************************************

>	static LibDisplayType SGDisplayPreviewFonts::GetDisplayType(SGMiscInfo *MiscInfo)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/5/95

	Inputs:		MiscInfo - Contains a few useful bits of info that may be
				needed for all event types.
	Outputs:	
	Returns:	The display mode type to use (position of text, and size of thumb)

	Purpose:	Return the display type to use - similar to the library item ones
	Notes:
	SeeAlso:

***********************************************************************************************/

LibDisplayType SGDisplayPreviewFonts::GetDisplayType(SGMiscInfo *MiscInfo)
{
	switch(MiscInfo->DisplayMode)
	{
		case 4:
			return LibDisplay_JustText;
			break;
		case 3:
			return LibDisplay_MediumThumb;
			break;
		case 2:
			return LibDisplay_SmallThumbText;
			break;
		case 1:
			// return LibDisplay_FullInfo;
			return LibDisplay_SingleLineFullInfo;
			break;
		case 0:
		default:
			return LibDisplay_LargeThumbText;
			break;
	}

	return LibDisplay_LargeThumbTextUnder;
}


/***********************************************************************************************

>	virtual void SGDisplayPreviewFonts::DrawItemText(RenderRegion *Renderer, SGRedrawInfo *RedrawInfo,
						SGMiscInfo *MiscInfo, DocRect *Rectangle, DocRect *BmpRect, BOOL Selected)	
	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/5/95
	
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

void SGDisplayPreviewFonts::DrawItemText(RenderRegion *Renderer, SGRedrawInfo *RedrawInfo,
	SGMiscInfo *MiscInfo, DocRect *Rectangle, DocRect *BmpRect, BOOL Selected)
{
	LibDisplayType DType = GetDisplayType(MiscInfo);

	switch(DType)
	{
		case LibDisplay_SmallThumb:
		case LibDisplay_MediumThumb:
		case LibDisplay_LargeThumb:
			return; // No text
	}

	INT32 OnePixel  = (INT32) DevicePixels(MiscInfo, 1);

	// Work out the text rectangle and stick it here
	DocRect TextRect(*Rectangle);
	TextRect.lo.x = BmpRect->hi.x + SG_GapBeforeText;

	GridLockRect(MiscInfo, &TextRect);

	Renderer->SetLineWidth(0);
	Renderer->SetLineColour(RedrawInfo->Transparent);

	// Set up the colours for rendering our text, and fill the background if selected
	if (Selected)
	{	 
		Renderer->SetFillColour(RedrawInfo->SelBackground);
			
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

		Renderer->SetFixedSystemTextColours(&RedrawInfo->SelForeground, &RedrawInfo->SelBackground);
	}
	else
		Renderer->SetFixedSystemTextColours(&RedrawInfo->Foreground, &RedrawInfo->Background);

	Renderer->SetFillColour(RedrawInfo->Foreground);	

	// No point drawing the text, etc...
	if(TextRect.lo.x + (OnePixel * 4) > Rectangle->hi.x)
		return;

	// Work out proper text string to display and display it
	String_256 DisplayText;

	if(DMode == 1)
		GetFullInfoText(&DisplayText);
	else
		GetNameText(&DisplayText);
	
	// Left justified text
	TextRect.lo.x += (OnePixel * 4);
	Renderer->DrawFixedSystemText(&DisplayText, TextRect);
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

BOOL SGDisplayPreviewFonts::DrawThumb(RenderRegion *Renderer, SGRedrawInfo *RedrawInfo, 
									SGMiscInfo *MiscInfo, DocRect *Rectangle, BOOL Background)
{
	LibDisplayType DType = GetDisplayType(MiscInfo);

	if(DType == LibDisplay_JustText)
		return TRUE;

	FontBitmap = GetDisplayedKernelBitmap(MiscInfo, Background);
						  
	// Bitmap not there...
	if(FontBitmap == NULL)
		return FALSE;

	if(FontBitmap->ActualBitmap == NULL)
		return FALSE;

	DMode = MiscInfo->DisplayMode;
	FontsSGallery *FSGallery = FontsSGallery::ThisGallery;
	
	DocRect BmpRect(*Rectangle);

	INT32 OnePixel  = (INT32) DevicePixels(MiscInfo, 1);

	UINT32 XSize = 0;
	UINT32 YSize = 0;
	
	Renderer->SetLineWidth(0);
	Renderer->SetLineColour(DocColour(COLOUR_TRANS));

	BOOL ThumbnailOK = TRUE;

	BitmapInfo Info;

  	BOOL InfoOK = FontBitmap->ActualBitmap->GetInfo( &Info );

	if(InfoOK)
	{
		// Get the details for the bitmap 
		XSize = (UINT32)Info.PixelWidth;
		YSize = (UINT32)Info.PixelHeight;				
		ThumbnailOK = TRUE;
	}
	else
	{
		// Get the default details for this mode
		GetThumbnailDetails(DMode, &XSize, &YSize, NULL);		
		ThumbnailOK = FALSE;
	}

	XSize *= OnePixel;
//	YSize *= OnePixel;

	// Thumbnail rectangle... space around edges but don't scale thumbnail
	BmpRect.lo.x += (3 * OnePixel);
	BmpRect.hi.x  = BmpRect.lo.x + XSize ;
	BmpRect.Inflate(0, -(3 * OnePixel));
	GridLockRect(MiscInfo, &BmpRect);			// Ensure it maps exactly to specific pixels

	NodeBitmap* DummyBmp = NULL;

	if(Invalid)
		return FALSE;

	if(ThumbnailOK)
	{
		// Setup the bitmap ready for plotting
		DummyBmp = new NodeBitmap();
		if (DummyBmp == NULL)
		{
			ThumbnailOK = FALSE;
		}
		else
		{
			// Need to clip to MyRect.hi.x at some point;
			DummyBmp->SetUpPath();
			DummyBmp->CreateShape(BmpRect);
			DummyBmp->GetBitmapRef()->SetBitmap(FontBitmap);

			// Now render the bitmap
			DummyBmp->Render(Renderer);

			// Unscaled version - problems with win32s
			// Renderer->DrawBitmap(IconRect.LowCorner(), FontBitmap);

			delete DummyBmp;
		}
	}

	// This could happen in the last block. Basically, we haven't got a bitmap, or had
	// problems creating one...
	if(!ThumbnailOK)
	{
		// NULL bitmap, just draw rectangle											  
		DocColour NullBitmapColour(230L, 230L, 230L);

		// Draw the 'blank' bitmap
		Renderer->SetFillColour(NullBitmapColour);	// RedrawInfo->Background//);

		GridLockRect(MiscInfo, &BmpRect);		// Ensure we're on a pixel
		Renderer->DrawRect(&BmpRect);
	}

	// Drawn bitmap preview ok
	return TRUE;
}

/***********************************************************************************************

>	virtual void SGDisplayPreviewFonts::HandleRedraw(SGRedrawInfo *RedrawInfo,
												SGFormatInfo *FormatInfo)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/1/95 (base generated in sgbase.cpp)

	Inputs:		RedrawInfo	- The information on the kernel-rendered redraw area
				FormatInfo	- The formatting information structure

				member variable FormatRect should be set up (before calling this method)
				to be the rectangle in which to draw this item

	Purpose:	SGDisplayPreviewFonts item redraw method - removed from the main HandleEvent
				method merely to make the code tidier.
				
				If the item has a null preview bitmap, but all the info required to generate
				a bitmap, then we go ahead and generate the bitmap ourselves and cache it with
				the item.

	Scope:		private

***********************************************************************************************/

void SGDisplayPreviewFonts::HandleRedraw(SGRedrawInfo *RedrawInfo, SGMiscInfo *MiscInfo)
{
	// First, inform the system that we are about to start rendering this item
	StartRendering(RedrawInfo, MiscInfo);

	RenderRegion *Renderer = RedrawInfo->Renderer;
	INT32 OnePixel = (INT32) DevicePixels(MiscInfo, 1);
	Renderer->SetLineWidth(0);
	Renderer->SetLineColour(RedrawInfo->Transparent);

	LibDisplayType DType = GetDisplayType(MiscInfo);

	// Exclude the 'TTF' / 'ATM' bmp space from the Format Rect, so as not to confuse things
	DocRect NewFormatRect(FormatRect);

	// Space for icon
	NewFormatRect.lox += (SGF_TYPE_WIDTH * OnePixel);

	GridLockRect(MiscInfo, &NewFormatRect);

	DocRect TypeRect(FormatRect);
	TypeRect.hix = NewFormatRect.lox;

	DrawTypeIcon(RedrawInfo, MiscInfo, &TypeRect, Type);

	// Use NewFormatRect from here on...
	DocRect BmpRect(NewFormatRect);
	DocRect UnscaledRect(NewFormatRect);

	if(DType == LibDisplay_JustText)
	{
		// No bitmap, so don't reserve much space for it...
		BmpRect.hi.x = BmpRect.lo.x + OnePixel;
	}
	else
	{

		UINT32 XSize = 0;
		UINT32 YSize = 0;
		DMode = MiscInfo->DisplayMode;

		// Get the relevant size details for the current mode (pixels)
		GetThumbnailDetails(DMode, &XSize, &YSize, NULL);
	
		// Convert pixels to millipoints
		XSize *= OnePixel;

		// Thumbnail rectangle... space around edges but don't scale thumbnail
		BmpRect.hi.x = BmpRect.lo.x + XSize + (6 * OnePixel);

	  	// Space for selection rectangle
	  	BmpRect.Inflate(-(OnePixel * 3));

		// Ensure it maps exactly to specific pixels
		GridLockRect(MiscInfo, &BmpRect);

		// Draw bmp outline it if selected
		if (Flags.Selected)
		{	
			if(BmpRect.hi.x > UnscaledRect.hi.x) BmpRect.hi.x = UnscaledRect.hi.x;
  
			BmpRect.Inflate(OnePixel * 3);
			GridLockRect(MiscInfo, &BmpRect);		// Ensure we're on a pixel
			DrawSelectionOutline(RedrawInfo, MiscInfo, &BmpRect);

			BmpRect.Inflate(-(OnePixel * 3));
		}

		// Not drawn a bitmap yet
		BOOL DrawnBitmap = FALSE;

		// The thumbnail drawing section...
		if(Library::BackgroundRedraw && DType != LibDisplay_JustText)
		{
			// Try to draw the thumb from cache. If it's not available in the cache, then
			// we'll register for background redraw and draw the blank box. Note that
			// ShouldIDraw... will also force us to always draw the thumb for selected items etc
			DrawnBitmap = DrawThumb(Renderer, RedrawInfo, MiscInfo, &UnscaledRect, FALSE);

			if (ShouldIDrawForeground(DrawnBitmap))
			{
				if (!DrawnBitmap)
				{
					// We failed to draw anything before, so we must try again, forcing the thumb to
					// be cached if it is necessary - if this fails we draw	a crossed box
					if (!DrawThumb(Renderer, RedrawInfo, MiscInfo, &UnscaledRect, TRUE))
						SGLibDisplayItem::DrawNullBitmapRect(Renderer, MiscInfo, &BmpRect, FALSE);
				}
			}
			else
			{
				if (!DrawnBitmap)
				{
					// We should background render, so we just draw a blank grey box
					SGLibDisplayItem::DrawNullBitmapRect(Renderer, MiscInfo, &BmpRect, TRUE);
				}
			}
		}
		else
		{
			// We're not doing this background stuff... Force a thumbnail to be drawn right this second !
			if (!DrawThumb(Renderer, RedrawInfo, MiscInfo, &UnscaledRect, TRUE))
				SGLibDisplayItem::DrawNullBitmapRect(Renderer, MiscInfo, &BmpRect, FALSE);
		}
	}

	// Draw the text
	DrawItemText(Renderer, RedrawInfo, MiscInfo, &UnscaledRect, &BmpRect, Flags.Selected);

	// Finally, inform the system that we have completed rendering this item
	StopRendering(RedrawInfo, MiscInfo);

}

/***********************************************************************************************

>	static void SGDisplayPreviewFonts::DrawTypeIcon(SGRedrawInfo *RedrawInfo, SGMiscInfo *MiscInfo,
						 DocRect *TypeRect, FontClass Type);

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/10/95

	Inputs:		RedrawInfo	- The information on the kernel-rendered redraw area
				FormatInfo	- The formatting information structure
				TypeRect	- A small rectangle to the left of the main display item
				Type		- The FontClass for the item

	Purpose:	Draws / Plots the 'ATM' or 'TTF' icon itself into the gallery

***********************************************************************************************/

void SGDisplayPreviewFonts::DrawTypeIcon(SGRedrawInfo *RedrawInfo, SGMiscInfo *MiscInfo, DocRect *TypeRect, FontClass Type)
{
	INT32 BitmapID = 0;					// Work out the bmp ID, if not known, simply return...
	if(Type == FC_ATM)
		BitmapID = _R(IDB_ATM_SYMBOL);
	else if(Type == FC_TRUETYPE)
		BitmapID = _R(IDB_TTF_SYMBOL);
	else return;

	DocRect GlyphRect(*TypeRect);

	INT32 OnePixel = (INT32) MiscInfo->PixelSize;
	INT32 Centre = GlyphRect.loy + ((GlyphRect.hiy - GlyphRect.loy) / 2);
	GlyphRect.loy = Centre - ((SGF_TYPE_WIDTH / 2) * OnePixel);
	GlyphRect.hiy = Centre + (((SGF_TYPE_WIDTH / 2) + 1) * OnePixel);		// Always round up...

	// Bodge so 'text only' mode has space to left of icon...
	LibDisplayType DType = SGDisplayPreviewFonts::GetDisplayType(MiscInfo);

	if(DType == LibDisplay_JustText)
		GlyphRect.lox += (6 * OnePixel);

	RedrawInfo->Renderer->DrawBitmap(GlyphRect.lo, BitmapID);
}

/***********************************************************************************************

>	virtual void SGDisplayPreviewFonts::GetNameText(String_256 *Result)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/4/95

	Inputs:
	Outputs:	Result - String to place resulting text in
	Returns:

	Purpose:	Returns the name text for this item, to support simple searching
				and sorting operations, with redraw methods for font items.
	Notes:
	SeeAlso:

***********************************************************************************************/

void SGDisplayPreviewFonts::GetNameText(String_256 *Result)
{
	ERROR3IF(Result == NULL, "SGDisplayPreviewFonts::GetNameText given null ptr");

	*Result = (String_256)FontDescription;
}

/***********************************************************************************************

>	virtual void SGDisplayPreviewFonts::GetFullInfoText(String_256 *Result)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/3/95

	Inputs:
	Outputs:	Result - String to place resulting text in
	Returns:	The display mode type to use (position of text, and size of thumb)

	Purpose:	Returns the full-info text for this item, to support simple searching
				operations, and redraw methods for font items.
	Notes:
	SeeAlso:

***********************************************************************************************/

void SGDisplayPreviewFonts::GetFullInfoText(String_256 *Result)
{
	ERROR3IF(Result == NULL, "SGDisplayPreviewFonts::GetFullInfoText given null ptr");

	// This is usually overridden to give better descriptions...
	*Result = (String_256)FontDescription;
}

/***********************************************************************************************

>	virtual INT32 SGDisplayPreviewFonts::CompareTo(SGDisplayNode *Other, INT32 SortKey)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/4/95

	Inputs:		Other - the node to compare this node to
				SortKey - An integer identifying how to compare the items
					0 = No sorting (always returns 0)
					1 = Sort-by-name
					2 = Sort-by-size
					3 = Sort-by-namelength
					4 = Sort-by-type (ATM / TTF)
					Other values will return 0, unless the derived class overrides this
					method in order to provide other sort modes.

	Returns:	negative (I am lesser), 0 (we are equal), or positive (I am greater)

	Purpose:	Compares this node to the 'other' node, to determine their relative positions
				in the display tree. Returns a value which usually indicates that the other
				node should be inserted before (-1, or 0) or after (+1) this item.

	SeeAlso:	SGDisplayNode::AddItem

***********************************************************************************************/

INT32 SGDisplayPreviewFonts::CompareTo(SGDisplayNode *Other, INT32 SortKey)
{
	switch(SortKey)
	{
		case 1:
			// Sort by name - override default and make 'FRED' and 'fred' equal...
			{
				String_256 MyName;
				String_256 ItsName;

				GetNameText(&MyName);
				MyName.toLower();
				Other->GetNameText(&ItsName);
				ItsName.toLower();

				return(MyName.CompareTo(ItsName, FALSE));
			}

		case 2:
			// Search by size
			// We could hunt for the ttf file and find it's size, maybe in future versions
			return 0;

		case 3:
			{
				String_256 MyName;
				String_256 ItsName;

				GetNameText(&MyName);
				Other->GetNameText(&ItsName);

				return (MyName.Length() - ItsName.Length());
			}
			break;
	
		case 4:
		{
			SGDisplayPreviewFonts *OtherFont = (SGDisplayPreviewFonts *)Other;

			if(Type == OtherFont->Type)
				return 0;
			if(Type == FC_ATM && OtherFont->Type == FC_TRUETYPE)
				return -1;
			return 1;
			break;
		}

	}

	return (SGDisplayNode::CompareTo(Other, SortKey));
}


/***********************************************************************************************

>	BOOL SGDisplayPreviewFonts::CreateThumbnail(KernelBitmap **Bitmap)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/2/95
	Inputs:		-
	Outputs:	*Bitmap contains the bitmap on exit
	Returns:	TRUE if the bmp was created successfully
				FALSE if it was not
	Purpose:	Creates a bitmap for this font
				We assume the item contains a valid logfont structure.
	
	Notes:		Only works with 8 and 32 bpp bmps at the mo.

***********************************************************************************************/

BOOL SGDisplayPreviewFonts::CreateThumbnail(KernelBitmap **Bitmap)
{
	ERROR3("SGDisplayPreviewFonts::CreateThumbnail should be overridden");
 
	// we've created a thumbnail - if ok is false then it's not got much in, but it's there...
	return TRUE;
}


/***********************************************************************************************

>	virtual BOOL SGDisplayPreviewFonts::HandleEvent(SGEventType EventType, void *EventInfo,
											 SGMiscInfo *MiscInfo)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/1/95 (base generated in sgbase.cpp)

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

BOOL SGDisplayPreviewFonts::HandleEvent(SGEventType EventType, void *EventInfo, SGMiscInfo *MiscInfo)
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

				RedrawInfo->Renderer->SaveContext();

				if (IMustRedraw(RedrawInfo))	// only redraw if we intersect the clip rect
					HandleRedraw(RedrawInfo, MiscInfo);

				RedrawInfo->Renderer->RestoreContext();
			}
			break;		// exit and return FALSE to pass the redraw event on


		case SGEVENT_MOUSECLICK:
			{
				SGMouseInfo *Mouse = GetMouseInfo(EventType, EventInfo);

				if (FormatRect.ContainsCoord(Mouse->Position))
				{
					if(Mouse->DoubleClick)
						DefaultClickHandler(Mouse, MiscInfo);
					else
					{
						DefaultPreDragHandler(Mouse, MiscInfo);

						// Grab a copy of the bitmap required for dragging
						GalleryFontsDragInfo *DragFont;
						KernelBitmap *DispBmp = GetDisplayedKernelBitmap(MiscInfo, TRUE);
						if(DispBmp)
						{
							LibraryGallery::TmpDraggingBitmap = DIBUtil::CopyKernelBitmap(DispBmp, TRUE);
							DragFont = new GalleryFontsDragInfo(this, Mouse, MiscInfo,
																Mouse->MenuClick, 0, 0);
						}
						else
						{
							// Null rectangle - give a specific size
							LibraryGallery::TmpDraggingBitmap = NULL;

							// Get the relevant size details for the current mode (pixels)
							UINT32 XSize = SG_InfiniteWidth;
							UINT32 YSize = SG_DefaultLargeIcon;
							INT32 DisplayMode = MiscInfo->DisplayMode;
							SGDisplayPreviewFonts::GetThumbnailDetails(DisplayMode, &XSize, &YSize, NULL);

							if(XSize == 0 || YSize == 0)
							{
								XSize = SGF_SMALL_X;
								YSize = SGF_SMALL_Y;
							}

							// Create the drag object...
							DragFont = new GalleryFontsDragInfo(this, Mouse, MiscInfo,
																Mouse->MenuClick, XSize, YSize);
						}

						if (DragFont != NULL)
							DragManagerOp::StartDrag(DragFont);
					}
					return(TRUE);		// Claim this event - nobody else can own this click
				}
			}
			break;

		default:
			return SGDisplayItem::HandleEvent(EventType, EventInfo, MiscInfo);
	}

	// Default return value: We do not claim this event, so it will be passed on to others
	return FALSE;
}


/***********************************************************************************************

>	virtual void SGDisplayPreviewFonts::DragWasReallyAClick(SGMouseInfo *Mouse, SGMiscInfo *MiscInfo)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/4/95

	Inputs:		Mouse - The mouse info passed to the original click handler
				MiscInfo - The misc info passed to the original click handler

	Purpose:	Handles a mouse click event. This is a callback function - drags of
				bitmaps from galleries will call this function back if the drag turns
				out to just be a click.

	SeeAlso:	SGDisplayPreviewFonts::HandleEvent; GalleryFontsDragInfo::OnClick

***********************************************************************************************/

void SGDisplayPreviewFonts::DragWasReallyAClick(SGMouseInfo *Mouse, SGMiscInfo *MiscInfo)
{
	// Just get default selection action to be applied for this click
	DefaultClickHandler(Mouse, MiscInfo, TRUE);
}

/********************************************************************************************

>	static BOOL GetThumbnailDetails(INT32 Mode, UINT32 *X, UINT32 *Y, String_256 *Text)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/2/95
	Inputs:		Mode - Mode of display (Small, medium, large -> 0-2)
				X - Location for return of X size (pixels)
				Y - Location for return of Y size (pixels)
				Text - Pointer to font name (used in full info mode)
	Returns:	Text - Text to use as for thumbnail
	Outputs:	Returns TRUE if things went OK.
	Purpose:	Quick way of returning size and information details regarding the font
				preview thumbnails.

********************************************************************************************/
													 
BOOL SGDisplayPreviewFonts::GetThumbnailDetails(INT32 Mode, UINT32 *X, UINT32 *Y, String_256 *Text)
{
	if(X == NULL || Y == NULL) return FALSE;

	switch(Mode)
	{
		case 0:						// Large (Normal)
			*X = SGF_NORMAL_X;
			*Y = SGF_NORMAL_Y;
			if(Text != NULL)
				*Text = _R(IDS_LARGEFONTDISPLAYSTRING);
			break;

		case 1:						// Full info
			*X = SGF_FULL_X;
			*Y = SGF_FULL_Y;
			// Text inserted beforehand
			//if(Text != NULL) *Text = _R(IDS_FONTS_GALLERY_LARGE_TEXT);
			break;

		case 2:						// Small (with description)
			*X = SGF_SMALL_X;
			*Y = SGF_SMALL_Y;
			if(Text != NULL) *Text = _R(IDS_FONTS_GALLERY_SMALL_TEXT);
			break;

		case 3:						// Small (with no description)
			*X = SGF_FULL_X;
			*Y = SGF_FULL_Y;
			//if(Text != NULL) *Text = _R(IDS_FONTS_GALLERY_SMALL_TEXT);
			break;

		case 4:						// No icon mode...
			*X = 0;
			*Y = 0;
			break;

	}

	return TRUE;
}		


/***********************************************************************************************

>	virtual void SGDisplayPreviewFonts::GetKeyWords(String_256 *Result)

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

void SGDisplayPreviewFonts::GetKeyWords(String_256 *Result)
{
	ERROR3IF(Result == NULL, "SGDisplayPreviewFonts::GetKeywords given a NULL param");
	if(Result == NULL)
		return;	

	// We don't support keyword searching on the installed fonts !
	*Result = "";
}




/***********************************************************************************************

>	virtual void SGDisplayPreviewFonts::MoveAfter(SGDisplayNode *NodeToMove)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/3/95

	Inputs:		NodeToMove - the node to move

	Purpose:	MOVES the given node (to a different position in the DisplayTree) as the
				previous (left) sibling of this node. If the node is not linked into
				a tree, it is effectively just inserted.

	Notes:		This base class method simply delinks the item and relinks it elsewhere
				in the display tree. However, derived classes will override this method
				so that moving display items can have a further effect of also rearranging
				the displayed "real" items. Before/After moving the real item, the
				derived class can then call this baseclass method to complete the action.
		
				Take care when moving items between groups (e.g. if an item is "moved"
				from one docuemnt to another, it could be a bad thing, so be very
				careful in derived classes to take appropriate action)

				Any attempt to move an item after *itself* is quietly ignored

	Errors:		ERROR3 and quiet exit if NodeToMove == NULL

	SeeAlso:	SuperGallery; SGDisplayColour::InsertAfter; SGDisplayColour::AddItem

***********************************************************************************************/

void SGDisplayPreviewFonts::MoveAfter(SGDisplayNode *NodeToMove)
{
	SGDisplayNode::MoveAfter(NodeToMove);
}



/***********************************************************************************************

>	virtual void SGDisplayPreviewFonts::MoveBefore(SGDisplayNode *NodeToMove)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/3/95

	Inputs:		NodeToMove - the node to move

	Purpose:	MOVES the given node (to a different position in the DisplayTree) as the
				previous (left) sibling of this node. If the node is not linked into
				a tree, it is effectively just inserted.

	Notes:		This base class method simply delinks the item and relinks it elsewhere
				in the display tree. However, derived classes will override this method
				so that moving display items can have a further effect of also rearranging
				the displayed "real" items. Before/After moving the real item, the
				derived class can then call this baseclass method to complete the action.
		
				Take care when moving items between groups (e.g. if an item is "moved"
				from one docuemnt to another, it could be a bad thing, so be very
				careful in derived classes to take appropriate action)

				Any attempt to move an item before *itself* is queitly ignored

	Errors:		ERROR3 and quiet exit if NodeToMove == NULL

	SeeAlso:	SuperGallery; SGDisplayColour::InsertBefore; SGDisplayColour::AddItem

***********************************************************************************************/

void SGDisplayPreviewFonts::MoveBefore(SGDisplayNode *NodeToMove)
{
	SGDisplayNode::MoveBefore(NodeToMove);
}



/********************************************************************************************

>	virtual BOOL SGDisplayPreviewFonts::GetBubbleHelp(DocCoord *MousePos, String_256 *Result)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/4/95

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

BOOL SGDisplayPreviewFonts::GetBubbleHelp(DocCoord *MousePos, String_256 *Result)
{
	ERROR3IF(MousePos == NULL || Result == NULL, "Invalid NULL params");

	*Result = *GetDisplayedTextDescription();

	return(TRUE);
}


	
/********************************************************************************************

>	virtual BOOL SGDisplayPreviewFonts::GetStatusLineHelp(DocCoord *MousePos, String_256 *Result)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/4/95

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

BOOL SGDisplayPreviewFonts::GetStatusLineHelp(DocCoord *MousePos, String_256 *Result)
{
	ERROR3IF(MousePos == NULL || Result == NULL, "Invalid NULL params");

	// "'<FontName>'; Click, then use the Deinstall button to remove font"
	Result->MakeMsg(_R(IDS_FONTS_INSTALLED_STATUS), ((TCHAR *)(*GetDisplayedTextDescription())));

	return(TRUE);
}



/***********************************************************************************************

>	SGTTFItem::SGTTFItem()

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/10/95
	Notes:		DON'T call this constructor. It ERROR3's. Call the other constructor

***********************************************************************************************/

SGTTFItem::SGTTFItem()
{
	ERROR3("Illegal call on default SGTTFItem constructor - call the other one!");
	CachedLogFont = NULL;
	IntLeading = 0;
	Type = FC_TRUETYPE;
}

/***********************************************************************************************

>	SGTTFItem::~SGTTFItem()

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/10/95

***********************************************************************************************/

SGTTFItem::~SGTTFItem()
{
 	if(CachedLogFont != NULL)
	{
		delete CachedLogFont;
		CachedLogFont = NULL;
	}
}

/***********************************************************************************************

>	SGTTFItem::SGTTFItem(KernelBitmap *PreviewFontsToDisplay, String_64 *FontName, INT32 IL, PLOGFONT lplf, UINT32 TheID)
 
	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/1/95
	Inputs:		PreviewFontsToDisplay - A pointer to a kernel bitmap which will be used
					by this item on redraws.
				FontName - Pointer to a description to place alongside the preview in the gallery		
				IL - Internal leading value required by preview generation code - saves passing
					an entire newtextmetric about
				lplf - a pointer to a log font structure which corresponds to this item. We
					need to allocate memory and store the entire structure for each item at
					the moment so we can generate thumbnails later on. This needs sorting out
					at some point !
				TheID - ID to associate item with... This is only really used for the filenames
					for saving at present. 				
 
 	Purpose:	Constructor for installed truetype font gallery item
 
 ***********************************************************************************************/

SGTTFItem::SGTTFItem(KernelBitmap *PreviewFontsToDisplay, String_64 *FontName, INT32 IL, PLOGFONT lplf, UINT32 TheID)
{
	// This should now be NULL since the font bitmaps are handled by the thumb cache
	ERROR3IF(PreviewFontsToDisplay != NULL, "SGTTFItem::SGTTFItem PreviewFontsToDisplay - This should now be NULL since the font bitmaps are handled by the thumb cache");
	
	FontBitmap = NULL; //PreviewFontsToDisplay;
	
  	if(FontName != NULL)
  		FontDescription = *FontName;

	CachedLogFont = NULL;
	if(lplf != NULL)
		CachedLogFont = new LOGFONT;
	if(CachedLogFont != NULL)
		*CachedLogFont = *lplf;

	ID = TheID;
	IntLeading = IL;
	Type = FC_TRUETYPE;
	Invalid = FALSE;
}

							
/***********************************************************************************************

>	virtual void SGTTFItem::GetFullInfoText(String_256 *Result)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/3/95

	Inputs:
	Outputs:	Result - String to place resulting text in

	Purpose:	Returns the full-info text for this item, to support simple searching
				operations, and redraw methods for font items.
	Notes:
	SeeAlso:

***********************************************************************************************/

void SGTTFItem::GetFullInfoText(String_256 *Result)
{
	ERROR3IF(Result == NULL, "SGTTFItem::GetFullInfoText given null ptr");

	*Result = (String_256)FontDescription;

#ifdef SGFONTS_FULLINFO_FOR_INSTALLED_FONTS

	PathName FOTFile;
	GetFOTNameFromRegistry(FontDescription, &FOTFile);

	*Result += " '"; // Debug
	*Result += TEXT(FOTFile.GetPath());
	*Result += "' - "; // Debug

	switch((CachedLogFont->lfPitchAndFamily & 0xf0))
	{
		case FF_DONTCARE:
			*Result += String_64(_R(IDS_FONTS_FF_DONTCARE)); // "Not classified"
			break;
		case FF_MODERN:
			*Result += String_64(_R(IDS_FONTS_FF_MODERN)); // "Modern"
			break;
		case FF_ROMAN:
			*Result += String_64(_R(IDS_FONTS_FF_ROMAN)); // "Roman"
			break;
		case FF_SCRIPT:
			*Result += String_64(_R(IDS_FONTS_FF_SCRIPT)); // "Script"
			break;
		case FF_SWISS:
			*Result += String_64(_R(IDS_FONTS_FF_SWISS)); // "Swiss"
			break;
		case FF_DECORATIVE:
			*Result += String_64(_R(IDS_FONTS_FF_DECORATIVE)); // "Decorative"
			break;
		default:
			*Result += String_64(_R(IDS_FONTS_UNRECOGNISED)); // "Unrecognised"
			break;
	}

	*Result += " - "; // Debug

	switch((CachedLogFont->lfPitchAndFamily & 0x0f))
	{
		case DEFAULT_PITCH:
			*Result += String_64(_R(IDS_FONTS_DEFAULT_PITCH)); // "Default pitch"
			break;
		case FIXED_PITCH:
			*Result += String_64(_R(IDS_FONTS_FIXED_PITCH)); // "Fixed pitch"
			break;
		case VARIABLE_PITCH:
			*Result += String_64(_R(IDS_FONTS_VARIABLE_PITCH)); // "Variable pitch"
			break;
	}

	*Result += " - "; // Debug

	switch(CachedLogFont->lfWeight)
	{
		case FW_DONTCARE:
			*Result += String_64(_R(IDS_FONTS_FW_DONTCARE)); // "No Weight"
			break;
		case FW_THIN:
			*Result += String_64(_R(IDS_FONTS_FW_THIN)); // "Thin"
			break;
		case FW_EXTRALIGHT:
			*Result += String_64(_R(IDS_FONTS_FW_EXTRALIGHT)); // "Extra Light"
			break;
		case FW_LIGHT:
			*Result += String_64(_R(IDS_FONTS_FW_LIGHT)); // "Light"
			break;
		case FW_NORMAL:
			*Result += String_64(_R(IDS_FONTS_FW_NORMAL)); // "Normal"
			break;
		case FW_MEDIUM:
			*Result += String_64(_R(IDS_FONTS_FW_MEDIUM)); // "Medium"
			break;
		case FW_SEMIBOLD:
			*Result += String_64(_R(IDS_FONTS_FW_SEMIBOLD)); // "Semi Bold"
			break;
		case FW_BOLD:
			*Result += String_64(_R(IDS_FONTS_FW_BOLD)); // "Bold"
			break;
		case FW_EXTRABOLD:
			*Result += String_64(_R(IDS_FONTS_FW_EXTRABOLD)); // "Extra Bold"
			break;
		case FW_HEAVY:
			*Result += String_64(_R(IDS_FONTS_FW_HEAVY)); // "Heavy"
			break;
	}

	*Result += " - ";// Debug

	switch((CachedLogFont->lfCharSet))
	{
		case ANSI_CHARSET:
			*Result += String_64(_R(IDS_FONTS_ANSI_CHARSET)); // "ANSI"
			break;
		case DEFAULT_CHARSET:
			*Result += String_64(_R(IDS_FONTS_DEFAULT_CHARSET)); // "Default"
			break;
		case SYMBOL_CHARSET:
			*Result += String_64(_R(IDS_FONTS_SYMBOL_CHARSET)); // "Symbol"
			break;
		case SHIFTJIS_CHARSET:
			*Result += String_64(_R(IDS_FONTS_SHIFTJIS_CHARSET)); // "ShiftJIS"
			break;
		case HANGEUL_CHARSET:
			*Result += String_64(_R(IDS_FONTS_HANGEUL_CHARSET)); // "Hangeul"
			break;
		case GB2312_CHARSET:
			*Result += String_64(_R(IDS_FONTS_GB2312_CHARSET)); // "GB2312"
			break;
		case CHINESEBIG5_CHARSET:
			*Result += String_64(_R(IDS_FONTS_CHINESEBIG5_CHARSET)); // "Chinese"
			break;
		case OEM_CHARSET:
			*Result += String_64(_R(IDS_FONTS_OEM_CHARSET)); // "OEM"
			break;
	}
#endif

}



/***********************************************************************************************

>	BOOL SGTTFItem::CreateThumbnail(KernelBitmap **Bitmap)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/2/95
	Inputs:		-
	Outputs:	*Bitmap contains the bitmap on exit
	Returns:	TRUE if the bmp was created successfully
				FALSE if it was not
	Purpose:	Creates a bitmap for this font
				We assume the item contains a valid logfont structure.

				If you are I (Richard) then this function can be told to save the
				thumbnail to disk... This is for generating the library thumbs...
	
	Notes:		Only works with 8 and 32 bpp bmps at the mo.

***********************************************************************************************/

BOOL SGTTFItem::CreateThumbnail(KernelBitmap** Bitmap)
{
	if(Bitmap == NULL)
	{
		ERROR3("SGTTFItem::CreateThumbnail given a NULL bitmap ptr");
		return FALSE;
	}

	// Create a bitmap, and actually stick the text string into it
	StringToBitmap S2BMP;
	
	// Don't save thumbnails
	PathName* ThePath = NULL;

	// Only save bitmaps if you are Richard and you are in Debug mode
	#ifdef _DEBUG

		// Only save out thumbnails if you've got a name like mine...	
	  	//if (IsUserName("Richard"))
		if (FALSE)
	  	{
			INT32 LibID = -1;
				
			// Filename to save thumbnail with (can be NULL, or left out)
			String_256 tmp;

			/****************************************/

		 	SuperGallery  *SGal = GetParentGallery();
			if(SGal != NULL)
			{
				SGDisplayRoot *DispTree = SGal->GetDisplayTree();
				if(DispTree != NULL)
				{
					// Get first item... bit scarry this...
					SGDisplayNode *Item = DispTree->FindNextSelectedItem(NULL);					

					while (Item != NULL)
					{
						if(Item->Flags.Selected)
						{		
							if(Item->IsKindOf(CC_RUNTIME_CLASS(SGLibFontItem)))
							{
								SGLibFontItem *FontItem = (SGLibFontItem *) Item;
								if(FontItem->GetType() == FC_TRUETYPE)
								{
									String_256 Result;
	 								((SGLibFontItem *)FontItem)->GetNameText(&Result);

									String_32 Result32;
									String_32 FontDesc32;
									Result.Left(&Result32, 31);
									FontDescription.Left(&FontDesc32, 31);

									if(Result32 == FontDesc32)
									{
										if(LibID != -1)
										{
											ERROR3("Bad... Two identical text strings in the fonts...");
										}
										else
										{	
											LibID = ((SGLibFontItem *)FontItem)->GetFontID();
										}
									}
								}
							}
						}

						Item = SGDisplayRoot::FindNextItemInTree(Item);
					}
				}
			}

			/**************************************/

			if (LibID != -1)
			{

				if (Library::MaxFieldCacheEntries <= 25)
				{
					switch(DMode)
					{
						case 0:
							wsprintf(tmp, TEXT("c:\\fonts\\TrueType\\XaraInfo\\F%05dL.bmp"), LibID);
							break;
						case 2:
							wsprintf(tmp, TEXT("c:\\fonts\\TrueType\\XaraInfo\\F%05dS.bmp"), LibID);
							break;
						case 1:
						case 3:
						default:
							wsprintf(tmp, TEXT("c:\\fonts\\TrueType\\XaraInfo\\F%05dM.bmp"), LibID);
							break;
					}
					ThePath = new PathName(tmp);	

					if(ThePath != NULL)
					{
						if(!ThePath->IsValid())
						{
				  			delete ThePath;	  			 
							ThePath = NULL;
						}
					}
				}
				else
				{
					switch(DMode)
					{
						case 0:
							wsprintf(tmp, TEXT("d:\\fonts\\TrueType\\XaraInfo\\F%05dL.bmp"), LibID);
							break;
						case 2:
							wsprintf(tmp, TEXT("d:\\fonts\\TrueType\\XaraInfo\\F%05dS.bmp"), LibID);
							break;
						case 1:
						case 3:
						default:
							wsprintf(tmp, TEXT("d:\\fonts\\TrueType\\XaraInfo\\F%05dM.bmp"), LibID);
							break;
					}
					ThePath = new PathName(tmp);	

					if(ThePath != NULL)
					{
						if(!ThePath->IsValid())
						{
				  			delete ThePath;	  			 
							ThePath = NULL;
						}
					}
				}
			}
		}

	#endif

	// Don't save bitmap
	//ThePath = NULL;
							
   	// Get the screen DPI
	UINT32 XDPI = 90;
	CDC Screen;
	if(Screen.CreateIC(TEXT("DISPLAY"), 0, 0, 0))
	{
		XDPI = GetDeviceCaps(Screen.m_hDC, LOGPIXELSX);			
		Screen.DeleteDC();
	}
	else
	{
		ERROR3("SGTTFItem::CreateThumbnail Unable to create screen DC");
		return FALSE;
	}

	BOOL ok = TRUE;						
	UINT32 Xsize = 128;
	UINT32 Ysize = 32;
	String_256 text(FontDescription);

	// Get the proper default sizes and text for this mode
	GetThumbnailDetails(DMode, &Xsize, &Ysize, &text);

	// Actually create the bitmap		
	ok = S2BMP.MakeBitmap(&text, Xsize, Ysize, SGF_BPP, XDPI, CachedLogFont, IntLeading, Bitmap, FC_TRUETYPE, ThePath);

	if(!ok)
		Invalid = TRUE;
	
#if _DEBUG

	if(!ok)	TRACEUSER( "Richard", _T("SGTTFItem::CreateBitmap: failed to create the bitmap\n"));

	if(ThePath != NULL) 
	{
		delete ThePath;	  			 
		ThePath = NULL;
	}

#endif

	// we've created a thumbnail - if ok is false then it's not got much in, but it's there...
	return TRUE;
}




/***********************************************************************************************

>	SGATMItem::SGATMItem()

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/10/95
	Notes:		DON'T call this constructor. It ERROR3's. Call the other constructor

***********************************************************************************************/

SGATMItem::SGATMItem()
{
	ERROR3("Illegal call on default SGATMItem constructor - call the other one!");
	CachedLogFont = NULL;
	IntLeading = 0;
 	Type = FC_ATM;
}

/***********************************************************************************************

>	SGATMItem::~SGATMItem()

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/10/95

***********************************************************************************************/

SGATMItem::~SGATMItem()
{
 	if(CachedLogFont != NULL)
	{
		delete CachedLogFont;
		CachedLogFont = NULL;
	}
}

/***********************************************************************************************

>	SGATMItem::SGATMItem(KernelBitmap *PreviewFontsToDisplay, String_64 *FontName, INT32 IL, PLOGFONT lplf, UINT32 TheID)
 
	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/10/95
	Inputs:		PreviewFontsToDisplay - A pointer to a kernel bitmap which will be used
					by this item on redraws.
				FontName - Pointer to a description to place alongside the preview in the gallery		
				IL - Internal leading value required by preview generation code - saves passing
					an entire newtextmetric about
				lplf - a pointer to a log font structure which corresponds to this item. We
					need to allocate memory and store the entire structure for each item at
					the moment so we can generate thumbnails later on. This needs sorting out
					at some point !
				TheID - ID to associate item with... This is only really used for the filenames
					for saving at present. 				
 
 	Purpose:	Constructor for installed ATM font gallery item
 
 ***********************************************************************************************/

SGATMItem::SGATMItem(KernelBitmap *PreviewFontsToDisplay, String_64 *FontName, INT32 IL, PLOGFONT lplf, UINT32 TheID)
{
	// This should now be NULL since the font bitmaps are handled by the thumb cache
	ERROR3IF(PreviewFontsToDisplay != NULL, "SGATMItem::SGATMItem PreviewFontsToDisplay - This should now be NULL since the font bitmaps are handled by the thumb cache");
	
	FontBitmap = NULL; //PreviewFontsToDisplay;
	
  	if(FontName != NULL)
  		FontDescription = *FontName;

	CachedLogFont = NULL;
	if(lplf != NULL)
		CachedLogFont = new LOGFONT;
	if(CachedLogFont != NULL)
		*CachedLogFont = *lplf;

	ID = TheID;
	IntLeading = IL;
 	Type = FC_ATM;
	Invalid = FALSE;
}
							
/***********************************************************************************************

>	virtual void SGATMItem::GetFullInfoText(String_256 *Result)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/10/95

	Inputs:
	Outputs:	Result - String to place resulting text in

	Purpose:	Returns the full-info text for this item, to support simple searching
				operations, and redraw methods for font items.
	Notes:
	SeeAlso:

***********************************************************************************************/

void SGATMItem::GetFullInfoText(String_256 *Result)
{
	ERROR3IF(Result == NULL, "SGATMItem::GetFullInfoText given null ptr");

	*Result = (String_256)FontDescription;

#ifdef SGFONTS_FULLINFO_FOR_INSTALLED_FONTS
	// Stick full info text in here...
#endif
}


/***********************************************************************************************

>	BOOL SGATMItem::CreateThumbnail(KernelBitmap **Bitmap)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/2/95
	Inputs:		-
	Outputs:	*Bitmap contains the bitmap on exit
	Returns:	TRUE if the bmp was created successfully
				FALSE if it was not
	Purpose:	Creates a bitmap for this font
				We assume the item contains a valid logfont structure.

				If you are I (Richard) then this function can be told to save the
				thumbnail to disk... This is for generating the library thumbs...
	
	Notes:		Only works with 8 and 32 bpp bmps at the mo.

***********************************************************************************************/

BOOL SGATMItem::CreateThumbnail(KernelBitmap **Bitmap)
{
	if(Bitmap == NULL)
	{
		ERROR3("SGATMItem::CreateThumbnail given a NULL bitmap ptr");
		return FALSE;
	}

	// Create a bitmap, and actually stick the text string into it
	StringToBitmap S2BMP;
	
	// Don't save thumbnails
	PathName *ThePath = NULL;

	// Only save bitmaps if you are Richard and you are in Debug mode
	#ifdef _DEBUG

		// Only save out thumbnails if you've got a name like mine...	
	  	//if (IsUserName("Richard"))
		if (FALSE)
	  	{
			INT32 LibID = -1;
				
			// Filename to save thumbnail with (can be NULL, or left out)
			String_256 tmp;

			/****************************************/

		 	SuperGallery  *SGal = GetParentGallery();
			if(SGal != NULL)
			{
				SGDisplayRoot *DispTree = SGal->GetDisplayTree();
				if(DispTree != NULL)
				{
					// Get first item... bit scarry this...
					SGDisplayNode *Item = DispTree->FindNextSelectedItem(NULL);					

					while (Item != NULL)
					{
						if(Item->Flags.Selected)
						{		
							if(Item->IsKindOf(CC_RUNTIME_CLASS(SGLibFontItem)))
							{
								SGLibFontItem *FontItem = (SGLibFontItem *) Item;
								if(FontItem->GetType() == FC_ATM)
								{
									String_256 Result;
	 								((SGLibFontItem *)FontItem)->GetNameText(&Result);

									String_32 Result32;
									String_32 FontDesc32;
									Result.Left(&Result32, 31);
									FontDescription.Left(&FontDesc32, 31);

									if(Result32 == FontDesc32)
									{
										if(LibID != -1)
										{
											ERROR3("Bad... Two identical text strings in the fonts...");
										}
										else
										{	
											LibID = ((SGLibFontItem *)FontItem)->GetFontID();
										}
									}
								}
							}
						}

						Item = SGDisplayRoot::FindNextItemInTree(Item);
					}
				}
			}

			/**************************************/

			if(LibID != -1)
			{

				if(Library::MaxFieldCacheEntries <= 25)
				{
					switch(DMode)
					{
						case 0:
							_stprintf(tmp, "c:\\fonts\\ATM\\XaraInfo\\F%05dL.bmp", LibID);
							break;
						case 2:
							_stprintf(tmp, "c:\\fonts\\ATM\\XaraInfo\\F%05dS.bmp", LibID);
							break;
						case 1:
						case 3:
						default:
							_stprintf(tmp, "c:\\fonts\\ATM\\XaraInfo\\F%05dM.bmp", LibID);
							break;
					}
					ThePath = new PathName(tmp);	

					if(ThePath != NULL)
					{
						if(!ThePath->IsValid())
						{
				  			delete ThePath;	  			 
							ThePath = NULL;
						}
					}
				}
				else
				{
					switch(DMode)
					{
						case 0:
							_stprintf(tmp, "d:\\fonts\\ATM\\XaraInfo\\F%05dL.bmp", LibID);
							break;
						case 2:
							_stprintf(tmp, "d:\\fonts\\ATM\\XaraInfo\\F%05dS.bmp", LibID);
							break;
						case 1:
						case 3:
						default:
							_stprintf(tmp, "d:\\fonts\\ATM\\XaraInfo\\F%05dM.bmp", LibID);
							break;
					}
					ThePath = new PathName(tmp);	

					if(ThePath != NULL)
					{
						if(!ThePath->IsValid())
						{
				  			delete ThePath;	  			 
							ThePath = NULL;
						}
					}
				}
			}
		}

	#endif

   	// Get the screen DPI
	UINT32 XDPI = 90;
	CDC Screen;
	if(Screen.CreateIC(TEXT("DISPLAY"), 0, 0, 0))
	{
		XDPI = GetDeviceCaps(Screen.m_hDC, LOGPIXELSX);			
		Screen.DeleteDC();
	}
	else
	{
		ERROR3("SGATMItem::CreateThumbnail Unable to create screen DC");
		return FALSE;
	}

	BOOL ok = TRUE;						
	UINT32 Xsize = 128;
	UINT32 Ysize = 32;
	String_256 text(FontDescription);

	// Get the proper default sizes and text for this mode
	GetThumbnailDetails(DMode, &Xsize, &Ysize, &text);

	// Actually create the bitmap		
	ok = S2BMP.MakeBitmap(&text, Xsize, Ysize, SGF_BPP, XDPI, CachedLogFont, IntLeading, Bitmap, FC_ATM, ThePath);

	if(!ok)
		Invalid = TRUE;

//	ERROR3IF(!ok, "Problems creating ATM bmp");

	// we've created a thumbnail - if ok is false then it's not got much in, but it's there...
	return TRUE;
}



/********************************************************************************************

>	FontsSGallery::FontsSGallery()
												 
	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/1/95 (base generated in sgbase.cpp)
	Purpose:	FontsSGallery default constructor
				A FontsSGallery is the main Fonts Gallery class... 

********************************************************************************************/

FontsSGallery::FontsSGallery()
{
	// The installed fonts group
	InsGroup = NULL;

	// Thumbnail cache for installed fonts - should be OK if null, we check later
	InsCache = new SGThumbs(NULL, SGLib_Font, SGThumb_Large);

	ThisGallery = this;

	IDCount = 0;

	// Default gallery size
	CSize Size(380, 256);
	SetGallerySize(Size);

	WarnAboutSectionDragging = FALSE;
	WarnAboutMultipleATMDragging = FALSE;
	if (!OpAsynchFontInstall::Init())
	{
		ERROR3("Failed to init operation");
	}
} 



/********************************************************************************************

>	FontsSGallery::~FontsSGallery()

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/1/95 (base generated in sgbase.cpp)
	Purpose:	FontsSGallery destructor.

********************************************************************************************/

FontsSGallery::~FontsSGallery()
{
	if(InsGroup != NULL) InsGroup = NULL;

	if(InsCache != NULL)
	{
		delete InsCache;
		InsCache = NULL;
	}

	ThisGallery = NULL;
	IDCount = 0;

	OpenLibFiles.DeleteAll();		// Ensure all open libraries are closed

	WarnAboutSectionDragging = FALSE;
	WarnAboutMultipleATMDragging = FALSE;
}

/********************************************************************************************

>	INT32 CALLBACK FontsSGallery::EnumInstalledFamily(ENUMLOGFONT FAR *lplf,
			 NEWTEXTMETRIC FAR *lpntm, INT32 FontType, LPARAM handle)
												 
	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/2/95
	Inputs:		lplf - address of logical-font data
				lpntm - address of physical-font data 
    			FontType - type of font 
    			handle - pointer to an 'SGDisplayGroup'
				(All the above are generated by Windows except 'handle' which is
				 passed in when we register the function)

	Returns:	Should return TRUE all the time, so that EnumFontFamilies can carry on doing its
				stuff... Returning FALSE will terminate any further calls.

	Purpose:	Callback function which creates a new item to display in the fonts
				preview gallery each time it's called.

				This function is called by another callback function which is also enumerating
				the fonts, so it's kind of nested really...
				
********************************************************************************************/

INT32 CALLBACK FontsSGallery::EnumInstalledFamily(ENUMLOGFONT FAR *lplf,
			 NEWTEXTMETRIC FAR *lpntm, INT32 FontType, LPARAM handle)
{
	FontsSGallery::AddedFonts = TRUE;
	
	ERROR3IF(ThisGallery == NULL, "There's no gallery in EnumInstalledFonts...");
	if(ThisGallery == NULL)	return 0;

	// Only allow the fonts which we know we're adding here...
	if((FontsSGallery::EnumFontClass == FC_ATM) && !(FontType & DEVICE_FONTTYPE))
		return TRUE;

	if((FontsSGallery::EnumFontClass == FC_TRUETYPE) && !(FontType & TRUETYPE_FONTTYPE))
		return TRUE;

	// Don't add ATM fonts if the ATM dll's messed up...
	if((FontType & DEVICE_FONTTYPE) && !OILFontMan::IsOkToCall(FC_ATM))
		return TRUE;
	
	// Can be TRUETYPE_FONTTYPE, RASTER_FONTTYPE, or DEVICE_FONTTYPE
	if((FontType & TRUETYPE_FONTTYPE) || (FontType & DEVICE_FONTTYPE)) {

		BOOL ok = TRUE;			
						
		// Next ID number...
		ThisGallery->IDCount++;

		// Don't create a bitmap just yet
		KernelBitmap *pBitmap = NULL;

		// Defer the preview generation until we actually want to redraw it
		pBitmap = NULL;

		// Bodge to fix weird restriction on font name length in non-95 windows... Hmmm...
/*		if(FontName.Length() >= 32)
			FontName = (TCHAR *)lplf->elfLogFont.lfFaceName;*/

		SGFontsGroup* DisplayGroup = (SGFontsGroup*) handle;

		// It's a TrueType or OpenType font ...
		if (FontType & TRUETYPE_FONTTYPE)
		{
			SGTTFItem* NewItem;					  
			String_64 FontName = (TCHAR*) lplf->elfFullName; // elfLogFont.lfFaceName; elfFullName;
			
			// HORROR BODGE: use the longest (ie. most descriptive) of the fullname or facename.
			String_64 strFaceName = (LPCTSTR) lplf->elfLogFont.lfFaceName;
			if (FontName.Length() < strFaceName.Length()) FontName = strFaceName;

			// Ignore fonts whose name begins with '@'
			if (((LPCTSTR) FontName)[0] != TEXT('@'))
			{
				// Look up the given name in the group's items.
				BOOL fAlreadyExists = FALSE;
				for (SGDisplayPreviewFonts* pItem = (SGDisplayPreviewFonts*) DisplayGroup->GetChild();
					 pItem != 0;
					 pItem = (SGDisplayPreviewFonts*) pItem->GetNext())
						if (pItem->FontDescription == FontName)
						{
							fAlreadyExists = TRUE;
							break;
						}

				// If it doesn't already exist then add it to the group.
				if (!fAlreadyExists)
				{
					// Create the font display item itself 
					NewItem = new SGTTFItem(pBitmap, &FontName, lpntm->tmInternalLeading,
											&lplf->elfLogFont, ThisGallery->IDCount);

					// Add it to the group														
					if (NewItem != 0) DisplayGroup->AddItem(NewItem);
				}
			}
		}
		else
		{
			// It's a Device font ...
			if (FontType & DEVICE_FONTTYPE)
			{
				SGATMItem* NewItem;

				// Of course, it would be just too sodding much to expect ATM to give
				// us the unique FullNames wouldn't it...
				// It's 11.30 at night, I'm really past caring... Tidy this up sometime someone...
				String_64 FontName = (TCHAR*) lplf->elfLogFont.lfFaceName;
				if (lplf->elfLogFont.lfWeight >= FW_BOLD) FontName += String_16(_R(IDS_FONTS_SPACE_BOLD));
				if (lplf->elfLogFont.lfItalic) FontName += String_16(_R(IDS_FONTS_SPACE_ITALIC));

				// Create the font item itself 
				NewItem = new SGATMItem(pBitmap, &FontName, lpntm->tmInternalLeading,
										&lplf->elfLogFont, ThisGallery->IDCount);

				// Add it to the Group														
				if (NewItem != 0) DisplayGroup->AddItem(NewItem);
			}
		}
	}
	return TRUE;
}

/********************************************************************************************

>	INT32 CALLBACK FontsSGallery::EnumInstalledFonts(ENUMLOGFONT FAR *lplf, NEWTEXTMETRIC FAR *lpntm,
										INT32 FontType, LPARAM handle)
												 
	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/2/95
	Inputs:		lplf - address of logical-font data
				lpntm - address of physical-font data 
    			FontType - type of font 
    			handle - pointer to an 'SGDisplayGroup'
				(All the above are generated by Windows except 'handle' which is
				passed in when we register the function)

	Outputs:	Returns TRUE if things went OK.
				Note - returning 0 will terminate any further calls.

	Purpose:	EnumFontFamilies callback function which is called for each font family found installed.
	
				Each time we are called, we actually use another callback function to generate
				items for all the members of the family.

********************************************************************************************/

INT32 CALLBACK FontsSGallery::EnumInstalledFonts(ENUMLOGFONT FAR *lplf,
			 NEWTEXTMETRIC FAR *lpntm, INT32 FontType, LPARAM handle)
{
	ERROR3IF(ThisGallery == NULL, "There's no gallery in EnumInstalledFonts...");
	if(ThisGallery == NULL)	return 0;
	
	// Don't add ATM fonts if the ATM dll's messed up...
	if((FontType & DEVICE_FONTTYPE) && !OILFontMan::IsOkToCall(FC_ATM))
		return TRUE;

	// Can be TRUETYPE_FONTTYPE, RASTER_FONTTYPE, or DEVICE_FONTTYPE
	if((FontType & TRUETYPE_FONTTYPE) || (FontType & DEVICE_FONTTYPE))
	{
		BOOL ok = TRUE;			
		FontsSGallery::AddedFonts = FALSE;

		// Hide aliases...
		if(!((FontType & DEVICE_FONTTYPE) && ATMInstall::IsFontAlias(lplf->elfLogFont.lfFaceName)))
		{	
			HDC ScreenDC = CreateCompatibleDC(NULL);
			if (ScreenDC == NULL)
			{
				ERROR3("FontsSGallery::EnumInstalledFontsSGDisplay: Unable to create screen DC");
				return TRUE; // keep enumming though
			}
			else
			{
				FontsSGallery::EnumFontClass = FC_UNDEFINED;

				if(FontType & TRUETYPE_FONTTYPE)
					FontsSGallery::EnumFontClass = FC_TRUETYPE;

				if(FontType & DEVICE_FONTTYPE)
					FontsSGallery::EnumFontClass = FC_ATM;

				ok = EnumFontFamilies(ScreenDC, (LPCSTR) lplf->elfLogFont.lfFaceName,
									  (FONTENUMPROC) EnumInstalledFamily, handle);

				// OK, so the font we're given wasn't a font family, it was an actual font...
				// Add it on its own...
				if (!AddedFonts) EnumInstalledFamily(lplf, lpntm, FontType, handle);
			}

			DeleteDC(ScreenDC);
		}
	}

	return TRUE;
}

/********************************************************************************************

>	virtual SGDisplayItem *FontsSGallery::AddLibraryItem(SGDisplayGroup *LibraryGroup,
												Library *ParentLib,
												LibraryIndex ItemIndex)
	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/3/95

	Inputs:		LibraryGroup - The group to add the item into
				ParentLib - (For cross checking inputs) the library you allege the above
				group is for.
				ItemIndex - The Library generated index for this item

	Returns:	NULL, or a pointer to the created item

	Purpose:	Called by the Library class to create a display item for every item in 
				a newly-scanned library file. It is essentially a callback to the gallery
				which requested that the library be scanned.
				
	Notes:		This method MUST BE OVERRIDDEN by the derived gallery that opens the library,
				in order to create appropriate SGDisplayItem-derived nodes for the things
				in the library (e.g. a clipart library gallery will have to create items
				that display clipart thumbnails)

	SeeAlso:	SuperGallery::AddLibraryGroup; SuperGallery::RemoveLibraryGroup

********************************************************************************************/

SGDisplayItem *FontsSGallery::AddLibraryItem(SGDisplayGroup *LibraryGroup,
													Library *ParentLib,
													LibraryIndex ItemIndex, BOOL bNew)
{
	ERROR3IF(LibraryGroup == NULL || ParentLib == NULL,
				"SuperGallery::AddLibraryItem - NULL params are illegal");

	ERROR3IF(LibraryGroup->GetParentLibrary() != ParentLib,
				"SuperGallery::AddLibraryitem - The DisplayGroup is not for the same library!");

	// Create a font library item
	SGLibFontItem *NewItem = new SGLibFontItem(ItemIndex, bNew);

#if 0
	// Alphabetic add...
	SGSortKey SortKeys[MaxSGSortKeys];
	for (INT32 i = 0; i < MaxSGSortKeys; i++)
	{
		SortKeys[i].SortKey  = 0;
		SortKeys[i].Reversed = FALSE;
	}

	SortKeys[0].SortKey  = 1;
#endif

	if (NewItem != NULL)
		LibraryGroup->AddItem(NewItem/*, (SGSortKey *)SortKeys*/);

	return(NewItem);
}





/********************************************************************************************

>	void FontsSGallery::CreateNewSubtreeForInstalledFonts(SGDisplayGroup *ExistingGroup)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/1/95 (base generated in sgbase.cpp)

	Inputs:		ExistingGroup - NULL (creates a new group for this document), or
					a pointer to the existing group-node for this document (in which case
					it clears all displayitems from the group and rebuilds it in place - this
					stops the display group moving around the tree at random!)

	Purpose:	This call will create a group containing all the installed fonts
				on your computer.
	Notes:		
	SeeAlso:	FontsSGallery::CreateNewSubtreeForLibraryFonts

********************************************************************************************/

void FontsSGallery::CreateNewSubtreeForInstalledFonts(SGFontsGroup *ExistingGroup)
{
	ERROR3IF(DisplayTree==NULL, "No display tree!");

	if(DisplayTree == NULL) return;
	if(ThisGallery == NULL)	return;

	ERROR3IF(ExistingGroup == NULL, "FontsSGallery::CreateNewSubtreeForInstalledFonts - NULL parameter given");
	if(ExistingGroup == NULL) return;

	if(InsCache != NULL)
		InsCache->DeleteThumbnails();

	// Wipe any existing PreviewFonts display items
	ExistingGroup->DestroySubtree(FALSE);

	String_64 Description(_R(IDS_FONTS_SCANNING_FONTS));
	BeginSlowJob(-1, FALSE, &Description);

	// Add the installed fonts to the gallery	
 	BOOL ok=TRUE;

	HDC ScreenDC = CreateCompatibleDC(NULL);
	if (ScreenDC == NULL)
	{
		ERROR3("FontSGallery::CreateNewSubtreeForInstalledFonts Unable to create screen DC");
		EndSlowJob();
		return;
	}
	else
	{
		IDCount = 0;
		ok=EnumFontFamilies(ScreenDC, NULL, (FONTENUMPROC)EnumInstalledFonts, (UINT32)ExistingGroup);
	}
	DeleteDC(ScreenDC);

	EndSlowJob();
}


/********************************************************************************************

>	BOOL FontsSGallery::CreateNewSubtreeForLibraryFonts(PathName *LibPath, BOOL WipeOld)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/4/95

	Inputs:		LibPath should be set to the new library location
				WipeOld will wipe the old library fonts from the gallery if TRUE
	Returns:	TRUE if things went OK

	Purpose:	This call will optionally kill all the old library groups and then recreate
				library groups for the given libpath
	Notes:		
	SeeAlso:	FontsSGallery::CreateNewSubtreeForInstalledFonts
				Erm... This is the 'AddNewLibrary' shared function in sglbase...

********************************************************************************************/

BOOL FontsSGallery::CreateNewSubtreeForLibraryFonts(PathName *LibPath, BOOL WipeOld)
{
	// Common library - gallery function which adds new groups to the gallery...
	return AddNewLibrary(LibPath, WipeOld, SGLib_Font, FALSE);
}



/********************************************************************************************

>	virtual BOOL FontsSGallery::InitMenuCommands(void)
												 
	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/9/95

	Returns:	TRUE for success

	Purpose:	Initialises any menu commands that this gallery needs.

	Notes:		Will only create the menu commands once - further calls in the future
				will return TRUE immediately wihtout doing anything.

********************************************************************************************/

BOOL FontsSGallery::InitMenuCommands(void)
{
	static BOOL MenusInitialised = FALSE;

	BOOL ok = TRUE;

	if (!MenusInitialised)
	{
		// Initialise menu command Ops

		// "Standard" entries for options menu
		ok = ok && InitMenuCommand((StringBase *) &SGCmd_Find, _R(IDS_SGMENU_FIND));
		ok = ok && InitMenuCommand((StringBase *) &SGCmd_Sort, _R(IDS_SGMENU_SORT));
		ok = ok && InitMenuCommand((StringBase *) &SGCmd_Properties, _R(IDS_SGMENU_PROPERTIES));

		// "Special" entries for over-list menu
		ok = ok && InitMenuCommand((StringBase *) &SGCmd_Add, _R(IDS_SGMENU_ADDFONT));
		ok = ok && InitMenuCommand((StringBase *) &SGCmd_EmptyFontsCache, _R(IDS_SGMENU_EMPTYFONTCACHE));

		// "Special" entries for over-list menu
		ok = ok && InitMenuCommand((StringBase *) &SGCmd_Apply, _R(IDS_SGMENU_APPLY));
		ok = ok && InitMenuCommand((StringBase *) &SGCmd_Install, _R(IDS_SGMENU_INSTALL));
		ok = ok && InitMenuCommand((StringBase *) &SGCmd_Deinstall, _R(IDS_SGMENU_DEINSTALL));
		ok = ok && InitMenuCommand((StringBase *) &SGCmd_Remove, _R(IDS_SGMENU_REMOVE));

		// "Standard" commands for over-list menu
		ok = ok && InitMenuCommand((StringBase *) &SGCmd_FoldGroup, _R(IDS_SGMENU_FOLD));
		ok = ok && InitMenuCommand((StringBase *) &SGCmd_UnfoldGroup, _R(IDS_SGMENU_UNFOLD));

		ok = ok && InitMenuCommand((StringBase *) &SGCmd_NextGroup, _R(IDS_SGMENU_NEXTGROUP));
		ok = ok && InitMenuCommand((StringBase *) &SGCmd_PrevGroup, _R(IDS_SGMENU_PREVGROUP));

		MenusInitialised = TRUE;
	}

	return(ok);
}



/********************************************************************************************

>	virtual BOOL FontsSGallery::BuildCommandMenu(GalleryContextMenu *TheMenu, SGMenuID MenuID)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/9/95

	Inputs:		TheMenu - The menu to add commands to
				MenuID  - The type of menu (over-list or from-options-button) to create

	Returns:	TRUE if it succeeded

	Purpose:	To build a menu of commands to be popped up over the gallery.
	
	Notes:		Override this method to stop the default menus being built

********************************************************************************************/

BOOL FontsSGallery::BuildCommandMenu(GalleryContextMenu *TheMenu, SGMenuID MenuID)
{
 	BOOL ok = TRUE;

	if (MenuID == SGMENU_OPTIONS)
	{
		// Options menu
		ok = ok && AddCommand(TheMenu, (StringBase *) &SGCmd_Add);
		ok = ok && AddCommand(TheMenu, (StringBase *) &SGCmd_EmptyFontsCache, TRUE);		// With separator
		ok = ok && AddCommand(TheMenu, (StringBase *) &SGCmd_Find);
		ok = ok && AddCommand(TheMenu, (StringBase *) &SGCmd_Sort);
		ok = ok && AddCommand(TheMenu, (StringBase *) &SGCmd_Properties);
	}
	else
	{
		// Over-list menu
		ok = ok && AddCommand(TheMenu, (StringBase *) &SGCmd_Apply);
		ok = ok && AddCommand(TheMenu, (StringBase *) &SGCmd_Install);
		ok = ok && AddCommand(TheMenu, (StringBase *) &SGCmd_Deinstall);
		ok = ok && AddCommand(TheMenu, (StringBase *) &SGCmd_Remove, TRUE);		// With separator

		SGDisplayGroup *TheGroup = FindCommandGroup();		// Fold or unfold as appropriate
		if (TheGroup == NULL || !TheGroup->Flags.Folded)
			ok = ok && AddCommand(TheMenu, (StringBase *) &SGCmd_FoldGroup);
		else
			ok = ok && AddCommand(TheMenu, (StringBase *) &SGCmd_UnfoldGroup);

		ok = ok && AddCommand(TheMenu, (StringBase *) &SGCmd_PrevGroup);
		ok = ok && AddCommand(TheMenu, (StringBase *) &SGCmd_NextGroup);
	}

	return(ok);
}



/********************************************************************************************

>	virtual OpState FontsSGallery::GetCommandState(StringBase *CommandID, String_256 *ShadeReason)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/9/95

	Inputs:		CommandID - TheString ID of the command
	Outputs:	ShadeReason - If you return (OpState.Greyed == TRUE) then this should be filled
				ion with the reason that the item is shaded/greyed.

	Returns:	An OpState indicating the current menu item state.

	Purpose:	To determine the state of a given menu item. This method is an exact
				parallel to an Op's GetState method (in fact, it is called by an Op's GetState)
	
	Notes:		Override this method to provide state info for your special commands
				Call the base class for unknown commands to allow it to handle them for you

				The base class handles all of these (maybe more - see the base class help)
					Properties, Sort, Find;
					New, Edit, Delete, Redefine;
					NextGroup, PrevGroup, FoldGroup, UnfoldGroup;

********************************************************************************************/

OpState FontsSGallery::GetCommandState(StringBase *CommandID, String_256 *ShadeReason)
{
	OpState State;

	if (*CommandID == SGCmd_Add)											// --- Add (always available)
		return(State);

	if (*CommandID == SGCmd_EmptyFontsCache)
	{
		if (m_bDiscardWebFolders)
			State.Greyed = TRUE;
		return (State);
	}


	if (*CommandID == SGCmd_Apply)											// --- Apply
	{
		if (GetSelectedItemCount() != 1)
		{
			State.Greyed = TRUE;
			ShadeReason->MakeMsg(_R(IDS_SGSHADE_NOSEL));
		}
		else
		{
			SGLibFontItem* pItem = (SGLibFontItem*) DisplayTree->FindNextSelectedItem(NULL);
			if (pItem)
			{
				String_256 Desc256;
				pItem->GetNameText(&Desc256);
				// Disable "Apply" for web fonts which are not already installed 
				if (!IsFontAlreadyInstalled(&Desc256, pItem->GetType()) && pItem->GetParentLibrary() && pItem->GetParentLibrary()->IsWebLibrary())
				{
					State.Greyed = TRUE;
					ShadeReason->MakeMsg(_R(IDS_SGSHADE_WEBFONT));
				}
			}
		}
	}
	else if (*CommandID == SGCmd_Install)									// --- Install
	{
#ifdef STOP_WINDOWS95_FONT_INSTALLS
		// Install if > one selected library item && not windows 95
		if (LibraryFontsSelected() < 1 || IsWin32c())
#else
		// Install if > one selected library item
		if (LibraryFontsSelected() < 1)
#endif
		{
			State.Greyed = TRUE;
			ShadeReason->MakeMsg(_R(IDS_SGSHADE_NOSEL));
		}
	}
	else if (*CommandID == SGCmd_Deinstall)									// --- Deinstall
	{
#ifdef STOP_WINDOWS95_FONT_INSTALLS
		// Deinstall if > one selected installed item && not windows 95
		if (!InstalledFontsSelected() || IsWin32c())
#else
		// Deinstall if > one selected installed item
		if (!InstalledFontsSelected())
#endif
		{
			State.Greyed = TRUE;
			ShadeReason->MakeMsg(_R(IDS_SGSHADE_NOSEL));
		}
	}
	else if (*CommandID == SGCmd_Remove)									// --- Remove
	{
		SGDisplayGroup *TheGroup = FindCommandGroup();
		if (TheGroup == NULL || (GetSelectedGroupCount() == 1 && TheGroup == InsGroup))
		{
			// Shaded if no groups are selected, or if the only selected group is the installed fonts group
			State.Greyed = TRUE;
			ShadeReason->MakeMsg(_R(IDS_SGSHADE_NOSELGROUP));
		}
	}
	else
		return(SuperGallery::GetCommandState(CommandID, ShadeReason));		// Unknown command- pass to baseclass

	return(State);
}



/********************************************************************************************

>	virtual void FontsSGallery::DoCommand(StringBase *CommandID)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/9/95

	Inputs:		CommandID - The String ID of the command

	Purpose:	To apply a given command when it is chosen from the menu.
	
	Notes:		Override this method to provide handling for your special commands.
				Call the base class if you don't recognise the command, so that it can
				handle standard commands.

				The base class handles all of these (maybe more - see the base class help)
					Properties, Sort, Find;
					New, Edit, Delete, Redefine; (it calls ApplyAction as appropriate)
					NextGroup, PrevGroup, FoldGroup, UnfoldGroup;

********************************************************************************************/

void FontsSGallery::DoCommand(StringBase *CommandID)
{
	if (*CommandID == SGCmd_Add)											// --- Add (always available)
		BrowseClicked();
	else if (*CommandID == SGCmd_EmptyFontsCache)
		RemoveWebFolders(SGLib_Font);
	else if (*CommandID == SGCmd_Apply)										// --- Apply
	{
		ApplyAction(SGACTION_APPLY);
		SelectionHasChanged();
	}
	else if (*CommandID == SGCmd_Install)									// --- Install
	{
		InstallFonts(TRUE);
		SelectionHasChanged();
	}
	else if (*CommandID == SGCmd_Deinstall)									// --- Deinstall
	{
		DeinstallFonts(FontsSGallery::DeleteTTFandFOTfiles);
		SelectionHasChanged();
	}
	else if (*CommandID == SGCmd_Remove)									// --- Remove
	{
		RemoveSelectedLibraries(TRUE);
		UpdateGRMFile();
	}
	else
		SuperGallery::DoCommand(CommandID);		// Unknown command- pass to the base class (Apply, etc)
}



/********************************************************************************************

>	void FontsSGallery::SortInstalledFonts(void)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/4/95

	Inputs:

	Purpose:	Applies the default gallery sort mode (last sort applied) to the
				installed group.

	SeeAlso:	SuperGallery::SortComparator; ::qsort

********************************************************************************************/

void FontsSGallery::SortInstalledFonts(void)
{
	CurrentSortGallery = this;

	// Fairly specialised sort for just the installed group
	SGDisplayNode *CurrentGroup = InsGroup;
	if (CurrentGroup == NULL)	// Nothing to sort
		return;

	SGDisplayNode *Ptr = CurrentGroup;

	// Count the number of available items to sort
	INT32 NumItemsToSort = 0;
	SGDisplayNode *Item = Ptr->GetChild();	// Count number of items to sort in this group
	while (Item != NULL)
	{
		NumItemsToSort++;
		Item = Item->GetNext();
	}

	// If there is no point in trying to sort, abort now
	if (NumItemsToSort < 2 || (DefaultSortKeys & 0x7f == 0))
		return;

	// Keep a copy of the current sort keys
  	SGSortKey TmpSortKeys[MaxSGSortKeys];

	// We want a straight alphabetic sort
	for(INT32 j=0; j<MaxSGSortKeys; j++)
		TmpSortKeys[j] = SortKeys[j]; 

	// Extract the SortKey array from a UINT32
	SortKeys[0].SortKey  = DefaultSortKeys & 0x7f;
	SortKeys[0].Reversed = (((DefaultSortKeys>>7) & 0x1) == 1);
	SortKeys[1].SortKey  = (DefaultSortKeys>>8) & 0x7f;
	SortKeys[1].Reversed = (((DefaultSortKeys>>15) & 0x1) == 1);

	// Start progress indicators, with a percentage based upon the number of items.
	// We will update twice for each group (after qsort and shuffle-items stages)
	String_64 Description(_R(IDS_SGOPTS_SORTING));
	BeginSlowJob(NumItemsToSort * 2, FALSE, &Description);

	INT32 NumItems = NumItemsToSort;
	NumItemsToSort = 0;
	INT32 i = 0;
	BOOL GroupHasChanged = FALSE;

	Ptr = CurrentGroup->GetChild();
	ERROR3IF(!Ptr->IsKindOf(CC_RUNTIME_CLASS(SGDisplayItem)),
				"Sort hasn't found items! Hideous failure imminent!" );

	// Get memory for an array of pointers to these items
	SGDisplayNode **SortArray = (SGDisplayNode **)CCMalloc(NumItems * sizeof(SGDisplayNode *));
	if (SortArray == NULL)
	{
		CurrentSortGallery = NULL;
		EndSlowJob();
		InformError();
		return;
	}

	// Fill in the array with pointers to display items to sort
	i = 0;
	Ptr = CurrentGroup->GetChild();
	while (Ptr != NULL)
	{
		SortArray[i++] = Ptr;
		Ptr = Ptr->GetNext();
	}

	// Sort the array of pointers
	qsort(SortArray, NumItems, sizeof(SGDisplayNode *), SuperGallery::SortComparator);

	NumItemsToSort += NumItems;
	ContinueSlowJob(NumItemsToSort);	// Update percentage complete for the number of items processed

	// Now, take the sorted array, and rearrange the display items to be in that order		
	// Special case the first item
	if (SortArray[0]->GetPrevious() != NULL)
	{
		SortArray[1]->MoveBefore(SortArray[0]);
	}

	// Then whip through the rest of the items
	for (i = 1; i < NumItems; i++)
	{
		if (SortArray[i]->GetPrevious() != SortArray[i-1])
		{
			SortArray[i-1]->MoveAfter(SortArray[i]);
		}
	}

	// Free our temporary workspace
	CCFree(SortArray);

	// Tell the derived gallery that the items in this group have been moved about
	AllItemsCopied((SGDisplayGroup *)CurrentGroup);

	NumItemsToSort += NumItems;
	ContinueSlowJob(NumItemsToSort);	// Update percentage complete for the number of items processed
		
	CurrentSortGallery = NULL;

	// Retain the original sort key status
	for(j=0; j<MaxSGSortKeys; j++)
	{
		SortKeys[j] = TmpSortKeys[j]; 
	}

	EndSlowJob();
	
	InvalidateCachedFormat();
	ReformatAndRedrawIfNecessary();
}


/********************************************************************************************

>	BOOL FontsSGallery::PreCreate(void)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/1/95 (base generated in sgbase.cpp)

	Inputs:		-
	Returns:	TRUE if the Gallery initialised successfully
				FALSE if it should not be opened due to a failure to initialise

	Purpose:	The FontsSGallery PreCreate handler. This overrides the base class
				PreCreate function. It is called in the very SuperGallery::Create
				method, just before the redraw stuff...

********************************************************************************************/

BOOL FontsSGallery::PreCreate(void)
{
	// If there isn't already one, create a DisplayTree
	if (DisplayTree == NULL)
	{
		DisplayTree = new SGDisplayRootScroll(this);	// New root node, with a scrollbar
		if (DisplayTree == NULL)
			return FALSE;
	}

	BOOL Sort = FALSE;

	// The installed fonts group
	if (InsGroup == NULL)
	{
		// New root node
		String_64 tstr(_R(IDS_FONTS_GALLERY_INSTALLED_FONTS));
		InsGroup = new SGFontsGroup(this, NULL, NULL, &tstr);
		if (InsGroup != NULL)
		{
			InsGroup->Flags.CanSelect = TRUE;		// Make all groups selectable
			
			// Add our new group to the display tree
			DisplayTree->AddItem(InsGroup);

			// Add items into the group, TRUE for the installed fonts
			CreateNewSubtreeForInstalledFonts(InsGroup);

			// Fold the installed group by default
			ForceGroupFolded(InsGroup, TRUE);

			Sort = TRUE;
		}
	}

	// Add the library groups to the gallery if they're not there already
	if(OpenLibFiles.IsEmpty())
	{
		String_256 sLoc = DefaultLibraryPath;
		// Set DefaultLibraryPath to <ExeLocation>\Fills - the user might have installed
		// the fills to his hard disk:
		if(CResDll::GetExecutablePath((TCHAR*)DefaultLibraryPath))
		{
			// Look for the localised help file in the HelpAndSupport folder now!
			String_256 LibDirName;
			GetLibraryDirectoryName(&LibDirName);
			DefaultLibraryPath += "\\";
			DefaultLibraryPath += LibDirName;
			PathName ThisPath(DefaultLibraryPath);
			if(!SGLibOil::FileExists(&ThisPath))	// will also work for paths (not just paths with a filename on the end)
			{
				// Fills not on hard disk. Try the CD location
				if(!ScanForLocation(SGLib_Font, NULL))
				{
					// hard drive and CD location not found
					// put the original path back
					DefaultLibraryPath = sLoc;
				}
			}
		}

		LibraryGallery::AddLibraryGroups(SGLib_Font, &DefaultLibraryPath);
	}

	// Use last time's display mode
	DisplayMode = FontsSGallery::DefaultDisplayMode;

	// Sort the new gallery contents
	if(Sort)
		SortGallery();

	return TRUE;
}

/********************************************************************************************

>	void FontsSGallery::SortGallery(void)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/5/95

	Inputs:		
	Returns:

	Purpose:	Sorts the contents of the gallery in an alphabetical fashion, whilst keeping
				the old sort key status...
	Notes:		
	SeeAlso:

********************************************************************************************/

void FontsSGallery::SortGallery(void)
{
	// Automatic sorting of library fonts now disabled
	// Installed fonts are still sorted
	SortInstalledFonts();
	return;

	// Sort alphabetically (and keep old sort keys)...
	SGSortKey TmpSortKeys[MaxSGSortKeys];
	for (INT32 i = 0; i < MaxSGSortKeys; i++)
		TmpSortKeys[i] = SortKeys[i];

	// Extract the SortKey array from a UINT32
	SortKeys[0].SortKey  = DefaultSortKeys & 0x7f;
	SortKeys[0].Reversed = (((DefaultSortKeys>>7) & 0x1) == 1);
	SortKeys[1].SortKey  = (DefaultSortKeys>>8) & 0x7f;
	SortKeys[1].Reversed = (((DefaultSortKeys>>15) & 0x1) == 1);

	ApplySortNow(TRUE);

	for (i = 0; i < MaxSGSortKeys; i++)
		SortKeys[i] = TmpSortKeys[i];
}

/********************************************************************************************

>	BOOL FontsSGallery::BrowseClicked(void)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/5/95

	Inputs:		
	Returns:	TRUE if the gallery has new stuff in it (FALSE if cancel clicked, etc)...

	Purpose:	Pops up the browse box and lets a new location be set for the data

	Notes:		The fonts gallery browse is slightly different to the base...

********************************************************************************************/

BOOL FontsSGallery::BrowseClicked(void)
{
	return (LibraryGallery::BrowseClicked(&DefaultLibraryPath, SGLib_Font, _R(IDS_REMOVE_OLD_GOUPS_FONTS)));
#if 0
	PathName ThePath(DefaultLibraryPath);

	// This returns FALSE if Cancel was hit, or an error occurred.
	if(SGLibOil::GetLibPath(&ThePath, GenerateIndexFile::CreateFontIndexes, SGLib_Font))
	{
		if(ThePath.IsValid())
		{							
			// Remove by default
			INT32 ButtonPressed = 2;

			if(LibraryGroupsInGallery() > 0 && LibraryGallery::AskAboutRemoving)
			{
				// Find out whether we should kill all the other groups
				ButtonPressed = InformMessage(_R(IDS_REMOVE_OLD_GOUPS_FONTS), _R(IDS_REMOVE), _R(IDS_KEEP), _R(IDS_CANCEL)/*, _R(IDS_HELP)*/);
				Error::ClearError();

				if(ButtonPressed == 3)
					return FALSE;
			}

			// Remove clicked if button pressed == 1
			if(CreateNewSubtreeForLibraryFonts(&ThePath, ButtonPressed == 1))
			{							
				// Remember the new path for the next time
				DefaultLibraryPath = ThePath.GetPath();
			}

			// Sort the entire gallery alphabetically
			SortGallery();

			ReformatAndRedrawIfNecessary();

			return TRUE;
		}
	}
	return FALSE;
#endif
}



/********************************************************************************************

>	virtual BOOL FontsSGallery::CanCreateIndexes(void)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/12/95

	Returns:	TRUE to if index generation is possible

	Purpose:	To determine if this gallery can generate indexes or not

********************************************************************************************/

BOOL FontsSGallery::CanCreateIndexes(void)
{
	return GenerateIndexFile::CreateFontIndexes;
}



/********************************************************************************************

>	virtual BOOL FontsSGallery::GetDefaults(String_256 *DefaultIndex, String_256 *IndexDesc, SGLibType *Type)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/12/95

	Outputs:	DefaultIndex	- The filename for the default index file (Xaraclip.txt)
				IndexDesc		- Description of the index / gallery (Clipart)
				Type			- Default library type associated with this gallery

	Returns:	TRUE if this was overridden successffuullyy (bloody spelling)

	Purpose:	To determine various library gallery default properties

********************************************************************************************/

BOOL FontsSGallery::GetDefaults(String_256 *DefaultIndex, String_256 *IndexDesc, SGLibType *Type)
{
	if(DefaultIndex != NULL)
		*DefaultIndex = _R(IDS_LIBRARIES_FONTS_FILENAME); 	// "XaraFont.txt";

	if(IndexDesc != NULL)
		*IndexDesc = _R(IDS_LIBRARIES_FONTS_DESC);			// "Font";

	if(Type != NULL)
		*Type = SGLib_Font;

	return TRUE;
}

/********************************************************************************************

>	virtual BOOL FontsSGallery::GetLibraryDirectoryName(String_256 *LibDirName)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/12/95

	Outputs:	LibDirName	- Returns the Default directory (on the clipart CD) for the gallery
	Returns:	TRUE if overridden and directory obtained...

	Purpose:	Get the default CD directory name for the gallery

********************************************************************************************/

BOOL FontsSGallery::GetLibraryDirectoryName(String_256 *LibDirName)
{
	LibDirName->MakeMsg(_R(IDS_LIBRARIES_FONTS_DIRNAME));	
	return TRUE;
}

/********************************************************************************************

>	virtual BOOL FontsSGallery::CheckForIndexMatch(StringBase *Txt)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/12/95

	Inputs:		Kind	- Last column entry in the index.txt file (" F")
	Returns:	TRUE if this signifies the gallery in question...

	Purpose:	To see whether we should add this line of the index.txt file to this gallery

********************************************************************************************/

BOOL FontsSGallery::CheckForIndexMatch(StringBase *Txt)
{
	BOOL Match = FALSE;

	// Textures (fills)
	if(((Txt->Sub(String_8("F"))!=-1) || (Txt->Sub(String_8("f"))!=-1)) ) Match = TRUE;

	return Match;
}

/********************************************************************************************

>	virtual BOOL FontsSGallery::GetQuietStatus(void)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/12/95
	Returns:	TRUE if Quiet has been pressed (SetQuiet status called with TRUE)
	Purpose:	Get the Quiet status of the gallery

********************************************************************************************/

BOOL FontsSGallery::GetQuietStatus(void)
{
	return FontsSGallery::QuietStatus;
}

/********************************************************************************************

>	virtual void FontsSGallery::SetQuietStatus(BOOL Status)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/12/95
	Inputs:		Status - Set to TRUE and call if Quiet has just been pressed on an 'adding
						 indexes' dialog
	Purpose:	Set the Quiet status of the gallery

********************************************************************************************/
		
void FontsSGallery::SetQuietStatus(BOOL Status)
{
	FontsSGallery::QuietStatus = Status;
}


/***********************************************************************************************

>	virtual BOOL FontsSGallery::ScanForLocation(SGLibType Type, StringBase *Result = NULL);

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/12/95

	Inputs:		Type of library to scan for
	Outputs:	(We assume the library static strings exist and use them - bit yucky)
				If a Result pointer if given then we copy this into there as well..
	Returns:	FALSE if it fails; TRUE if we got a path

	Purpose:	Searches all the drives for a CDROM drive. If it finds the Camelot CD
				mount here at Xara HQ, we point to that instead.
	Notes:

***********************************************************************************************/

BOOL FontsSGallery::ScanForLocation(SGLibType Type, StringBase *Result)
{
#ifndef STANDALONE
	// Search for a CD ROM drive
	String_256 DriveName;
	BOOL AreWeXara = FALSE;
	String_256 XaraDrive;

	BOOL Adjust = KeyPress::IsAdjustPressed();
#ifndef _DEBUG
	Adjust = FALSE;
#endif

	if(SGLibOil::LocateCDROMDrive(this, Type, &DriveName, &AreWeXara, &XaraDrive, Adjust))
	{
		if(AreWeXara)
			DriveName = XaraDrive;

		switch(Type)
		{
			case SGLib_Font:
			{
				FontsSGallery::DefaultLibraryPath = DriveName;
#ifdef _DEBUG
				if(Adjust)
				{
					FontsSGallery::DefaultLibraryPath += TEXT("Fonts");
				}
				else
#endif
				{
					String_256 LibDirName;
					GetLibraryDirectoryName(&LibDirName);
					FontsSGallery::DefaultLibraryPath += LibDirName;
				}

				if(Result)
					*Result = FontsSGallery::DefaultLibraryPath;

				return TRUE;
			}
		}
	}
#endif
	return FALSE;
}


								 
/********************************************************************************************

>	virtual BOOL FontsSGallery::ApplyAction(SGActionType Action)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/1/95 (base generated in sgbase.cpp)

	Inputs:		Action - Indicates what action to apply

	Returns:	TRUE to indicate successful handling of the action, or
				FALSE to indicate failure

	Purpose:	Applies certain conventional gallery actions (usually associated with
				gallery buttons, for new, edit, delete, etc)

	Notes:		There's a whole lot of mess in here at the moment, as it's been given
				the job of 'test launcher' for the library and thumbnail code...

	SeeAlso:	SGActionType

********************************************************************************************/

BOOL FontsSGallery::ApplyAction(SGActionType Action)
{
	// No display tree? Better forget about it then!
	if (DisplayTree == NULL)
		return FALSE;

	switch(Action)
	{
		case SGACTION_APPLY:
#ifndef STANDALONE
			// Apply the first selected fontone.
			ApplyFont(FALSE, DisplayTree->FindNextSelectedItem(NULL));
#endif
			break;

		case SGACTION_DISPLAYMODECHANGED:

			FlushBackgroundRedraws();
			
			if(InsGroup != NULL)
			{
				// Kill all the previews and recreate them at their new size
				// CreateNewSubtreeForInstalledFonts(InsGroup);

				// And force a redraw of the entire list
				ForceRedrawOfList();
			}
			FontsSGallery::DefaultDisplayMode = DisplayMode;
			
			return OK;

		case SGACTION_SETOPTIONS:	// Set values in the options dialogue as it is opened
			{
				if (CurrentOptionsDlg == NULL)
					return(FALSE);

				CurrentOptionsDlg->AddDisplayModeName(_R(IDS_GALLERYDM_LARGE));		// 0
				CurrentOptionsDlg->AddDisplayModeName(_R(IDS_GALLERYDM_FULLINFO));	// 1
				CurrentOptionsDlg->AddDisplayModeName(_R(IDS_GALLERYDM_SMALL));		// 2
				CurrentOptionsDlg->AddDisplayModeName(_R(IDS_GALLERYDM_ICONONLY));	// 3
				CurrentOptionsDlg->AddDisplayModeName(_R(IDS_GALLERYDM_TEXTONLY));	// 4
			}
			break;

		case SGACTION_SETSORTMODE:
			{
				if (CurrentSortDlg == NULL)
					return(FALSE);

				CurrentSortDlg->AddSortKeyName(_R(IDS_SORTBY_NAME));		// 1
				CurrentSortDlg->AddSortKeyName(_R(IDS_SORTBY_MEMORY));		// 2
				CurrentSortDlg->AddSortKeyName(_R(IDS_SORTBY_NAMELENGTH));	// 3
				CurrentSortDlg->AddSortKeyName(_R(IDS_SORTBY_FONTTYPE));	// 4
			}
			break;

		default:
			return(SuperGallery::ApplyAction(Action));
			break;
	}

	return(TRUE);
}

/********************************************************************************************

>	virtual void FontsSGallery::ApplySortNow(BOOL ApplyToEntireList)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/5/95

	Inputs:		ApplyToEntireList - TRUE to sort the entire DisplayList,
				FALSE to sort only groups which contain a selection

	Purpose:	Override which sets the default sort mode variable before calling the base
				class to actually do the sort.

	SeeAlso:	SuperGallery::SortComparator; ::qsort

********************************************************************************************/

void FontsSGallery::ApplySortNow(BOOL ApplyToEntireList)
{
	// Encode the SortKey array info into a UINT32
	DefaultSortKeys =  ((SortKeys[0].SortKey & 0x7f))
					 + ((SortKeys[0].Reversed & 0x1) << 7)
					 + ((SortKeys[1].SortKey & 0x7f) << 8)
					 + ((SortKeys[1].Reversed & 0x1) << 15);

	SuperGallery::ApplySortNow(ApplyToEntireList);
}

/********************************************************************************************

>	virtual MsgResult FontsSGallery::Message(Msg* Message)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/1/95 (base generated in sgbase.cpp)

	Inputs:		Message - The message to handle

	Purpose:	A standard message handler, really.

				It's needed to add various font gallery specific things to the base handler.
				Currently we handle the bar buttons, so they press down and come up properly,
				also various other buttons in the gallery are done here.

	Notes:		Any messages that this does not handle must be passed down to the
				SuperGallery base class message handler.

				NOTE WELL that the SuperGallery base class handler does some funky things
				for us - see SuperGallery::Message - such as deleting our display subtree
				for any document which dies (which, uncannily, would explain why they go
				away like that when you close documents ;-), and shading the gallery when
				there are no documents present. [To override this behaviour in these cases,
				you should respond to the message, and return OK rather than calling the
				base class message handler]

	SeeAlso:	SuperGallery::Message

********************************************************************************************/

MsgResult FontsSGallery::Message(Msg* Message)
{
	// Added by Craig Hamilton 18/1/01.
	static HANDLE	handle = NULL;
	CString			mutexName = "autorunMutex";
	// End added.

	// If we have no displaytree, then we have not been shown, or something terrible has
	// happened, so we don't bother handling any of these messages.
	if (DisplayTree == NULL)
		return(LibraryGallery::Message(Message));

	if (IS_OUR_DIALOG_MSG(Message))
	{
		DialogMsg* Msg = (DialogMsg*)Message;

		switch (Msg->DlgMsg)
		{
			case DIM_CREATE:
				// Added by Craig Hamilton 18/1/01.
				// This and the section of code of the same date in the DIM_CANCEL handler below
				// deal with the creation and destruction of a kernel object that is recognised by
				// the autorun. If this object exists then the autorun does not run. This is so
				// that the user can enter their resources cd while the gallery is open and not be
				// annoyed by the autorun appearing.
				handle = CreateMutex(NULL,TRUE,mutexName);
				// End added.

				SGInit::UpdateGalleryButton(OPTOKEN_DISPLAYFONTSGALLERY, TRUE);
				GalleryAboutToReOpen();
				break;

			case DIM_CANCEL:
				// Added by Craig Hamilton 18/1/01.
				if(handle != NULL)
				{
					CloseHandle(handle);
				}
				// End added.

				SGInit::UpdateGalleryButton(OPTOKEN_DISPLAYFONTSGALLERY, FALSE);
				BROADCAST_TO_CLASS(ThumbMessage(ThumbMessage::ThumbState::KILLCACHE, SGLib_Font), DialogOp);
				// Free memory, etc...
				GalleryAboutToClose();
				break;

			case DIM_LFT_BN_CLICKED:
				switch (Msg->GadgetID)
				{
					case _R(IDC_LIBGAL_BROWSE):
					case _R(IDC_LIBGAL_ADD_FONTS):
						BrowseClicked();
						return OK;

					case _R(IDC_GALLERY_INSTALL):
						InstallFonts(TRUE);
						SelectionHasChanged();
						return OK;

					case _R(IDC_GALLERY_DEINSTALL):
						DeinstallFonts(FontsSGallery::DeleteTTFandFOTfiles);
						SelectionHasChanged();
						return OK;

					case _R(IDC_GALLERY_HELP):		// Show help page
						HelpUserTopic(_R(IDS_HELPPATH_Gallery_Font));
						break;



					case _R(IDC_BMPGAL_SAVE):
#ifdef _DEBUG		
						{
							SGDisplayNode *Item = DisplayTree->FindNextSelectedItem(NULL);
							if(Item != NULL && Item->IsKindOf(CC_RUNTIME_CLASS(SGLibDisplayItem)))
							{
								SGLibDisplayItem *LibItem = (SGLibDisplayItem *)Item;

								Library *Parent = LibItem->GetParentLibrary();
								if(Parent != NULL)
								{
									PathName *Source = Parent->ReturnIndexLocation();
									PathName Dest(*Source);
									Dest.SetType((String_256)"BAK");
									SGLibOil::FileCopy(Source, &Dest);

									Parent->SaveIndexInDisplayedOrder(Source, FALSE);
								}
							}
						}
#endif
						break;
					

				}
				break;

			case DIM_FONTCHANGE:
				FontsSGallery::DoFontChange();
				break;

		}
	}		 	
	else if (MESSAGE_IS_A(Message, DocChangingMsg))
	{
		DocChangingMsg *Msg = (DocChangingMsg *) Message;
		switch (Msg->State)
		{
			case DocChangingMsg::DocState::SELCHANGED:
				if (Msg->pNewDoc == NULL)
				{
					// There is no selected doc - this can only mean there are no docs
					// at all, so we had better shade the gallery
					SuperGallery::ShadeGallery(TRUE);
				}
				else
				{
					// Possibly a new document
					SuperGallery::ShadeGallery(FALSE);
					SelectionHasChanged();
				}
				break;
		}
	}
	else if (MESSAGE_IS_A(Message, ThumbMessage) && DisplayTree != NULL)
	{
		ThumbMessage *Msg = (ThumbMessage *) Message;

		// If a library Thumb message comes around, flush the redraw stuff, etc
		if(Msg->State == ThumbMessage::ThumbState::CACHESIZECHANGED
		   || Msg->State == ThumbMessage::ThumbState::KILLCACHE)
		{
			FlushBackgroundRedraws();
			ForceRedrawOfList();
		}
	}

	return(LibraryGallery::Message(Message));
}    
				   
/********************************************************************************************

>	static void FontsSGallery::DoFontChange(void)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/10/95

	Purpose:	Called on WM_FONTCHANGE / DIM_FONTCHANGE messages.
				Rebuilds the installed section of the gallery.

********************************************************************************************/

void FontsSGallery::DoFontChange(void)
{
	// Don't rebuild a gallery if it isn't there, or explicitly flagging not to...
	if(ThisGallery == NULL || DontUpdate)
		return;
	
	// We have a gallery, but no display tree - eek !
	if(ThisGallery->DisplayTree == NULL)
		return;

	// This now deletes all the thumbs too
	ThisGallery->CreateNewSubtreeForInstalledFonts(InsGroup);

	// Sort the installed fonts alphabetically
	ThisGallery->SortInstalledFonts();

	// And force a redraw of the entire list
	ThisGallery->ForceRedrawOfList();
}


/********************************************************************************************

>	virtual void FontsSGallery::HandleDragStart(DragMessage *DragMsg)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/3/95

	Inputs:		DragMsg - The DRAGSTARTED message that we've just recieved, indicating
				the type of drag being started

	Purpose:	Checks a DragMessage to see if it is a colour drag.
				If it is, then it creates a drag target for this gallerys listbox.

	Notes:		Overrides the default base-class action. Calls down to the base class
				if it is not a colour drag, so that dragging of gallery groups is allowed

********************************************************************************************/

void FontsSGallery::HandleDragStart(DragMessage *DragMsg)
{
	// If it's a font drag, add a target for our window. If not, let the base class
	// have a look at it (to see if it is a gallery item being dragged)
	if (DragMsg->pInfo->IsKindOf(CC_RUNTIME_CLASS(GalleryFontsDragInfo)) ||
		DragMsg->pInfo->IsKindOf(CC_RUNTIME_CLASS(GalleryLibFontsDragInfo)) )
	{
		SGFontsDragTarget *NewTarget = new SGFontsDragTarget(this, GetListGadgetID());
	}
	else
		SuperGallery::HandleDragStart(DragMsg);
}

/***********************************************************************************************

>  	static BOOL FontsSGallery::ApplyFont(BOOL Dropping, SGDisplayNode *TheNode, NodeRenderableInk* pObjectHit = NULL);

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/4/95

	Inputs:		Dropping - set to true if it's a drag and drop apply. If this is true, pObjectHit must
							not be NULL.
				TheNode - specifies a font item to apply. It should be either an SGTTFItem, SGATMItem or
						 	item or a SGLibDisplayItem.
				pObjectHit - should be set up if Dropping is true.
	Outputs:

	Returns:	TRUE then success and joyous screams
				FALSE on failure

	Purpose:	Applies the selected font. This can be applied as a drag and drop. If so,
				Dropping should be true and the pObjectHit should point to the associated
				structure.
	Notes:

***********************************************************************************************/

BOOL FontsSGallery::ApplyFont(BOOL Dropping, SGDisplayNode *TheNode, NodeRenderableInk* pObjectHit)
{
	// If this is false after getting the name, we don't try applying it
	BOOL FoundFont = FALSE;
	
	// This will hold the facename for the font
	String_64 Desc;

	// Should the applied font be italic ?
	BOOL Italic = FALSE;

	// How bold should the applied font be ?
	INT32 Weight = FW_NORMAL;

	// Type of font - TTF or ATM
	FontClass Type = FC_UNDEFINED;

	if(TheNode == NULL)
	{
		// "Can't find a selected font to apply"
		InformError(_R(IDS_FONTS_CANT_FIND_SELECTED_FONT), _R(IDS_OK));
		return FALSE;
	}

	if(TheNode->IsKindOf(CC_RUNTIME_CLASS(SGTTFItem)))
	{
		// Check the font's valid before applying it !
		if(((SGTTFItem *)TheNode)->Invalid)
		{
			InformError(_R(IDS_FONTS_INVALID_APPLY));
			Error::ClearError();
			return FALSE;
		}

		// Few, it's already installed
		LOGFONT *lplf = ((SGTTFItem *)TheNode)->CachedLogFont;
		if(lplf != NULL)
		{
			Desc = (TCHAR *)lplf->lfFaceName;

			Italic = lplf->lfItalic;
			if(Italic)
				Italic = TRUE;

			Weight = lplf->lfWeight;

			FoundFont = TRUE;

			Type = FC_TRUETYPE;
		}
		else
		{
			FoundFont = FALSE;
		}
	}
	else
	if(TheNode->IsKindOf(CC_RUNTIME_CLASS(SGATMItem)))
	{
		// Few, it's already installed
		LOGFONT *lplf = ((SGATMItem *)TheNode)->CachedLogFont;
		if(lplf != NULL)
		{
			Desc = (TCHAR *)lplf->lfFaceName;

			Italic = lplf->lfItalic;
			if(Italic)
				Italic = TRUE;

			Weight = lplf->lfWeight;

			FoundFont = TRUE;

			Type = FC_ATM;
		}
		else
		{
			FoundFont = FALSE;
		}
	}
	else
	{
		if(TheNode->IsKindOf(CC_RUNTIME_CLASS(SGLibDisplayItem)))
		{
			if(ThisGallery == NULL)
			{
				ERROR3("Can't find the font gallery to install a font to");
				return FALSE;
			}	

			// Check ATM's running if this font is an ATM font. If not, error and stop...
			FontClass Class	= ((SGLibFontItem *)TheNode)->GetType();
			if(Class == FC_ATM)
			{
				if(!OILFontMan::IsOkToCall(FC_ATM))
				{
					InformWarning(_R(IDS_ATM_NOT_RUNNING), _R(IDS_OK));
					Error::ClearError();
					return FALSE;
				}			
			}

			// Check if the font's already installed
			String_256 Desc256;
			((SGLibFontItem *)TheNode)->GetNameText(&Desc256);
//			((SGLibDisplayItem *)TheNode)->GetDisplayedTextDescription(&Desc256);
			FoundFont = IsFontAlreadyInstalled(&Desc256, ((SGLibFontItem *)TheNode)->GetType());
			Desc = (String_64)Desc256;

			// If it's not already installed, install it...
			if(!FoundFont)
				FoundFont = InstallDraggedLibFont(ThisGallery, (SGLibDisplayItem *)TheNode, &Desc);

			if(FoundFont)
			{
				// Get the logfont for the library font which will now be installed
				LOGFONT *lplf = NULL;
				BOOL FoundLogFont = FindLogFont(&Desc256, &lplf, ((SGLibFontItem *)TheNode)->GetType());

				if(lplf != NULL)
				{
					// _tcscpy((TCHAR *)Desc, (TCHAR *)lplf->lfFaceName);
					Desc = (TCHAR *)lplf->lfFaceName;

					Italic = lplf->lfItalic;
					if(Italic)
						Italic = TRUE;

					Weight = lplf->lfWeight;

					Type = ((SGLibFontItem *)TheNode)->GetType();
				}
				else
				{
					FoundFont = FALSE;
				}
			}
		}
		else
		{
			ERROR3("It's not a library font and it's not an installed font ! What is it ???");
			FoundFont = FALSE;
		}
	}

#ifndef STANDALONE
	// Now let's go ahead and apply the font
	if(FoundFont)
	{
		// We've got to cache fonts before returning their handles...
		if ( FONTMANAGER->CacheNamedFont(&Desc, Type) != ILLEGALFHANDLE )
		{				
			// create a typeface attribute
			AttrTxtFontTypeface *TypeFaceAttrib = new AttrTxtFontTypeface;
			if(TypeFaceAttrib != NULL)
			{
				TypeFaceAttrib->Value.HTypeface = FONTMANAGER->GetFontHandle(&Desc, Type);
//			   	TypeFaceAttrib->SetBold(Weight > FW_MEDIUM);
//				TypeFaceAttrib->SetItalic(Italic);
			   	TypeFaceAttrib->SetBold(FALSE);
				TypeFaceAttrib->SetItalic(FALSE);
			}
			// Create a bold attribute
			AttrTxtBold* BoldAttrib = new AttrTxtBold;
			if (BoldAttrib)
				BoldAttrib->Value.BoldOn = (Weight > FW_MEDIUM);
			// Create an italic attribute
			AttrTxtItalic* ItalicAttrib = new AttrTxtItalic;
			if (ItalicAttrib)
				ItalicAttrib->Value.ItalicOn = Italic;
			// apply the attributes	
			if(Dropping)
			{
				// Dropping, if not dropped onto anything, pObjectHit will be NULL, and so will
				// Ask about applying the attribute as the default one
				if (TypeFaceAttrib != NULL)
					AttributeManager::ApplyAttribToNode(pObjectHit, (NodeAttribute *)TypeFaceAttrib);

				if (BoldAttrib != NULL)
					AttributeManager::ApplyAttribToNode(pObjectHit, (NodeAttribute *)BoldAttrib);

				if (ItalicAttrib != NULL)
					AttributeManager::ApplyAttribToNode(pObjectHit, (NodeAttribute *)ItalicAttrib);
			}
			else
			{
				// Not dropping, or didn't hit anything, so just set the current attributes
//				if(TypeFaceAttrib != NULL)
//					AttributeManager::AttributeSelected((NodeAttribute *)TypeFaceAttrib, NULL);
				// First we add the items to a List of NodeAttributePtrItem
				List AttrList;
				NodeAttributePtrItem* pItem;
				if (TypeFaceAttrib)
				{
					pItem = new NodeAttributePtrItem;
					if (pItem)
					{
						pItem->NodeAttribPtr = TypeFaceAttrib;
						AttrList.AddHead(pItem);
					}
				}
				if (BoldAttrib)
				{
					pItem = new NodeAttributePtrItem;
					if (pItem)
					{
						pItem->NodeAttribPtr = BoldAttrib;
						AttrList.AddHead(pItem);
					}
				}
				if (ItalicAttrib)
				{
					pItem = new NodeAttributePtrItem;
					if (pItem)
					{
						pItem->NodeAttribPtr = ItalicAttrib;
						AttrList.AddHead(pItem);
					}
				}

				if (!AttrList.IsEmpty())
					AttributeManager::AttributesSelected(AttrList, _R(IDS_FONTAPPLY_UNDO));

				// We don't need the list of attrs anymore
				NodeAttributePtrItem* pAttrItem = (NodeAttributePtrItem*)AttrList.GetHead();
				while (pAttrItem)
				{
					delete (pAttrItem->NodeAttribPtr);
					pAttrItem->NodeAttribPtr = NULL;
					pAttrItem = (NodeAttributePtrItem*)AttrList.GetNext(pAttrItem);
				}
				AttrList.DeleteAll();
			}

			// make sure the infobar reflects the current attributes
			TextInfoBarOp::Update();
		}
	}
#endif

	return FoundFont;
}

/***********************************************************************************************

>  	BOOL FontsSGallery::InstallFonts(BOOL Copy);

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/3/95

	Inputs:		Copy - If true then copy the TTF file to Windows\System

	Returns:	TRUE then success and joyous screams
				FALSE on failure

	Purpose:	Shoots through all the selected library font items and installs them.
	
	Notes:		By install, we mean create a .fot file and register it with the windows
				kernel and ourselves.

***********************************************************************************************/

BOOL FontsSGallery::InstallFonts(BOOL Copy)
{
	if (DisplayTree == NULL)
	{
		ERROR3("FontsSGallery::InstallFonts No display tree - bad !");
		return FALSE;
	}

	// Find the first selected item (if any) and get the next item to start searching from
	// If there is no selection, then get the first item
	SGDisplayNode *Item = DisplayTree->FindNextSelectedItem(NULL);
	BOOL InstalledFonts = FALSE;

	INT32 ButtonPressed = 0;

	// Don't update the gallery while we're installing
	DontUpdate = TRUE;

	INT32 SelectedFontCount = LibraryFontsSelected();
	BOOL WarnedThatATMIsntPresent = FALSE;
	BOOL ATMInstalled = OILFontMan::IsOkToCall(FC_ATM);

	while (Item != NULL && ButtonPressed != 4)
	{
		if(Item->IsKindOf(CC_RUNTIME_CLASS(SGLibFontItem)))
		{
			if(Item->Flags.Selected)
			{
				SGLibDisplayItem *FontItem = (SGLibDisplayItem *) Item;
				PathName FileName;		
				BOOL ok = FontItem->GetFileName(&FileName);

				if(ok)
				{
					String_256 Desc;
					ok = FontItem->GetDisplayedTextDescription(&Desc);
					
					if(ok)
					{				
						// Check if ATM font. If it is, check ATM is installed and running
						// If it isn't, warn and don't install the font
						String_8 Ending;
						Ending += FileName.GetType();
						Ending.toLower();
						BOOL ThisFontIsATM = (Ending.Sub((String_8)"pfb") != -1);

						if(ThisFontIsATM && !ATMInstalled && !WarnedThatATMIsntPresent)
						{
							InformWarning(_R(IDS_ATM_NOT_RUNNING), _R(IDS_OK));
							Error::ClearError();
							WarnedThatATMIsntPresent = TRUE;
						}

						if(!ThisFontIsATM || ATMInstalled)
						{
							if(ButtonPressed != 2)
							{
								// check if it's ok to install this font with the user
								String_256 WarnMsg;

								// NB the proper display name is different to the one which should be
								// passed into InstallFont (for ATM's only)
								String_256 ProperDisplayName;
								FontItem->GetNameText(&ProperDisplayName);
								
								WarnMsg.MakeMsg(_R(IDS_FONTS_GALLERY_SURE_INSTALL), (TCHAR *)ProperDisplayName);
								Error::SetError(0, WarnMsg, 0);
								if(SelectedFontCount > 1)
									ButtonPressed = InformWarning(0, _R(IDS_INSTALL), _R(IDS_ALL_SELECTED) /*_R(IDS_INSTALL_ALL)*/, _R(IDS_SKIP), _R(IDS_CANCEL));
								else
								{
									ButtonPressed = InformWarning(0, _R(IDS_INSTALL), _R(IDS_CANCEL));
									
									// Bodge so that ButtonPressed will be the same for both versions of the warning
									// IE: 1 for install, 2 for all, 3 for skip, 4 for cancel
									if(ButtonPressed == 2) ButtonPressed = 4;
								}
								Error::ClearError();
							}
	
							if (ButtonPressed == 1 || ButtonPressed == 2)
							{
								//("Installing 'FRED'");
								Library* pFontLib = FontItem->GetParentLibrary();
								if (!pFontLib)
								{
									ERROR3("Illegal NULL pointer");
									return FALSE;
								}
								if (pFontLib->IsWebLibrary()) // if so we have to download the font first (font files are never cached locally)
								{
									String_256 strFileURL = FileName.GetPath(FALSE);
									pFontLib->LocalPath2URL(&strFileURL);
									OpDescriptor* pOpDesc =	OpDescriptor::FindOpDescriptor(CC_RUNTIME_CLASS(OpAsynchFontInstall));
									if (pOpDesc != NULL)
									{
										WebFontInstallParam* Param = new WebFontInstallParam;
										ERROR2IF(!Param, FALSE, "Memory allocation error");
										// Set up the parameters which we need to download and install the font
										Param->type = TYPE_FONT;
										Param->priority = AsynchDownload::PRIORITY_HIGH;
										Param->pGallery = this;
										Param->file = FileName;
										FontItem->GetDisplayedTextDescription(&Param->strDescription);
										FontItem->GetNameText(&Param->strAltDescription);
										Param->strURL = strFileURL;
										Param->bIsTemp = FALSE;
										Param->Output = &ok;
										// Invoke the operation
										pOpDesc->Invoke((OpParam*) Param);
									}
									else
									{
										ERROR3("Can't find Op descriptor");
									}
								}
								else // do a normal install
								{
									String_64 SlowJob;
									SlowJob.MakeMsg(_R(IDS_FONTS_INSTALLING), (TCHAR *)Desc);
									BeginSlowJob(-1, FALSE, &SlowJob);
									ok = InstallFont(&FileName, &Desc, FALSE);
									InstalledFonts = TRUE;
									EndSlowJob();
								}
							}
						}
					}

					if(!ok)
					{
						// Problems installing a specified font
						String_256 ErrMsg;
						ErrMsg.MakeMsg(_R(IDS_PROBLEMS_INSTALLING), (TCHAR *)Desc);
						Error::SetError(0, ErrMsg, 0);
						InformError();
						Error::ClearError();
					}
				}
				else
				{
					DWORD f = GetLastError();
					ERROR3("FontsSGallery::InstallFonts Error getting library filename");
				}
			}
		}

		Item = SGDisplayRoot::FindNextItemInTree(Item);
	}

	// Safe to update the gallery when we get our fontchange message now
	DontUpdate = FALSE;

	if (InstalledFonts)
	{
		// Warn everyone that the fonts have changed
		SendMessage(HWND_BROADCAST, WM_FONTCHANGE, 0, 0);
	}
	return TRUE;
}

/***********************************************************************************************

>  	BOOL FontsSGallery::DeinstallFonts(BOOL Delete);

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/3/95

	Inputs:		Delete - If TRUE then delete the TTF file in Windows\System
						 Note, we delete the FOT file always... if we can...

	Returns:	TRUE then success and joyous screams
				FALSE if september

	Purpose:	Deinstalls a Library Font Item (Truetype font file).
	
	Notes:		By deinstall, we mean de-register the .fot file with the windows kernel and
				ourselves, then optionally delete the .FOT file and .TTF file in windows\
				system.

***********************************************************************************************/

BOOL FontsSGallery::DeinstallFonts(BOOL Delete)
{
	TRACEUSER( "Richard", _T("Deinstall Font\n"));

	if(InsGroup == NULL)
	{
		ERROR3("No installed fonts group detected");
		return FALSE;
	}
	//SGFontsGroup *InsGroup;		// The installed fonts group

	INT32 ButtonPressed = 0;
	BOOL DeinstalledFonts = FALSE;
	SGDisplayPreviewFonts *Selected = NULL;

	// Don't update the gallery while we're installing
	DontUpdate = TRUE;

	if (InsGroup != NULL)
		Selected = (SGDisplayPreviewFonts *) InsGroup->FindNextSelectedItem(NULL);

	INT32 SelectedFontCount = InsGroup->GetSelectedItemCount();
							 
	while (Selected != NULL && ButtonPressed != 4)
	{
		String_256 Desc(Selected->FontDescription);

		// It's a truetype
		if(Selected->Type == FC_TRUETYPE)
			Desc += String_16(_R(IDS_FONTS_SPACE_TRUETYPE)); // Add the TrueType -> "Gill Sans Bold Italic (TrueType)");

		// Check if the font is being used by a document, and let the user do something about
		// it if it is...
		if(Selected->IsFontBeingUsed())
		{
			String_256 WarnMsg;
			WarnMsg.MakeMsg(_R(IDS_DEINSTALL_FONT_IN_USE), (TCHAR *)Selected->FontDescription);
			Error::SetError(0, WarnMsg, 0);
			INT32 Button = InformWarning(0, _R(IDS_DEINSTALL), _R(IDS_SKIP), _R(IDS_CANCEL));
			Error::ClearError();

			// If Not doing a Deinstall all, deinstall this one
			if(Button == 1 && ButtonPressed != 2) ButtonPressed = 1;

			// Skip - skips just this one font
			if(Button == 2) ButtonPressed = 3;

			// Cancel - stops the deinstall at this point
			if(Button == 3) ButtonPressed = 4;
		}
		else
		{
			if(ButtonPressed != 2)
			{
				// tell the user that some objects couldn't be converted
				String_256 WarnMsg;
				WarnMsg.MakeMsg(_R(IDS_FONTS_GALLERY_SURE_DEINSTALL), (TCHAR *)Selected->FontDescription);
				Error::SetError(0, WarnMsg, 0);

				if(SelectedFontCount > 1)
					ButtonPressed = InformWarning(0, _R(IDS_DEINSTALL), _R(IDS_ALL_SELECTED) /*_R(IDS_DEINSTALL_ALL)*/, _R(IDS_SKIP), _R(IDS_CANCEL));
				else
				{
					ButtonPressed = InformWarning(0, _R(IDS_DEINSTALL), _R(IDS_CANCEL));
				
					// Bodge so that ButtonPressed will be the same for both versions of the warning
					// IE: 1 for deinstall, 2 for all, 3 for skip, 4 for cancel
					if(ButtonPressed == 2) ButtonPressed = 4;
				}

				Error::ClearError();
			}
		}
	
		if (ButtonPressed == 1 || ButtonPressed == 2)
		{
			String_64 SlowJob;
			SlowJob.MakeMsg(_R(IDS_FONTS_DEINSTALLING), (TCHAR *)Desc); 
			BeginSlowJob(-1, FALSE, &SlowJob);

			if(DeinstallFont(&Desc, Delete, Selected->CachedLogFont))
			{
				DeinstalledFonts = TRUE;

				// Pause for a given amount of time after each deinstall to let windows
				// sort itself out...
				if(SelectedFontCount > 1)
				{
					MonotonicTime PauseClock;
					while ( !PauseClock.Elapsed( 800 ) ) ;
				}
			}
			EndSlowJob();
		}
		Selected = (SGDisplayPreviewFonts *) InsGroup->FindNextSelectedItem(Selected);
	}

	// Safe to update the gallery when we get our fontchange message now
	DontUpdate = FALSE;

	if (DeinstalledFonts)
	{
		// Warn everyone that the fonts have changed
		SendMessage(HWND_BROADCAST, WM_FONTCHANGE, 0, 0);
	}
	return TRUE;
}	


/***********************************************************************************************

>	BOOL FontsSGallery::InstallFont(PathName *FontFile, String_256 *Description, BOOL Temp)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/10/95

	Inputs:		FontFile points to the TTF file on the CD, or wherever it is.
				Description is used by the ini file to identify the font
				   Should be something like "Gill Sans Bold Italic (TrueType)"	
				Temp specifies if the installation is only a temporary one.
	
	Returns:	TRUE if installation went OK, FALSE on fail.

	Purpose:	Decides what type of font this is from the filename ending, then calls
				the appropriate install / deinstall function

	Notes:		As with most of this fonts stuff it only works with ATM and TTF fonts...

				*** IMPORTANT ***
				You must set the static 'DontUpdate' to TRUE before calling this
				function, since it generates a font change message, which will update
				the gallery. You should set it to FALSE after calling it. There are 
				situations where setting it to FALSE directly afterwards is also bad
				(Install All), so it's not done in the function itself.

***********************************************************************************************/

BOOL FontsSGallery::InstallFont(PathName *FontFile, String_256 *Description, BOOL Temp)
{
	ERROR3IF(!DontUpdate, "If you're calling install font or deinstall font, you MUST set DontUpdate to TRUE beforehand !");
	if(!DontUpdate) return FALSE;

	String_8 Ending;
	Ending += FontFile->GetType();
	Ending.toLower();

	if(Ending.Sub((String_8)"ttf") != -1)
	{
		if(FontsSGallery::IsFontAlreadyInstalled(Description, FC_ATM))
		{
			InformError(_R(IDS_FONTS_INSTALL_TTF_SAME_NAME));
			Error::ClearError();
		}
		else
		{
			if (InstallTTFFont(FontFile, Description, Temp))
			{
				return TRUE;
			}
			else
			{
				return FALSE;
			}
		}
	}

	if(Ending.Sub((String_8)"pfb") != -1)
	{
		String_256 Result(*Description);

		// Change lines which end ;BOLDITALIC to ' Bold Italic' (ATM only really)
		FontsSGallery::MakeBoldItalicReadable(&Result);
		/*if(Result.Sub((String_8)";") != -1)
		{
			String_256 TmpResult(Result);
			TmpResult.Left(&Result, TmpResult.Sub((String_8)";"));

			if(TmpResult.Sub(String_16(_R(IDS_FONTS_CAPITAL_BOLD))) != -1)
				Result += String_16(_R(IDS_FONTS_SPACE_BOLD));
	
			if(TmpResult.Sub(String_16(_R(IDS_FONTS_CAPITAL_ITALIC))) != -1)
				Result += String_16(_R(IDS_FONTS_SPACE_ITALIC));
		} */

		if(FontsSGallery::IsFontAlreadyInstalled(&Result, FC_TRUETYPE))
		{
			InformError(_R(IDS_FONTS_INSTALL_ATM_SAME_NAME));
			Error::ClearError();
		}
		else
		{
			if (ATMInstall::InstallFont(FontFile, Description, Temp))
			{
				return TRUE;
			}
			else
			{
				return FALSE;
			}
		}
	}

	return FALSE;
}



/***********************************************************************************************

>	BOOL FontsSGallery::DeinstallFont(String_256 *FontDesc, BOOL Delete, LONGFONT *LogFnt = NULL)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/10/95

	Inputs:		FontDesc 	- used by the ini file to identify the font
				   			Should be something like "Gill Sans Bold Italic (TrueType)"	
				Delete		- Set to TRUE if you want the files deleted after deinstalling.
				LogFnt		- Pointer to logfont for item (only currently used with ATM's)
	
	Returns:	TRUE if installation went OK, FALSE on fail.

	Purpose:	Decides what type of font this is by checking for a (TrueType) in the string,
				then calls the appropriate install / deinstall

	Notes:		As with most of this fonts stuff it only works with ATM and TTF fonts...

				*** IMPORTANT ***
				You must set the static 'DontUpdate' to TRUE before calling this
				function, since it generates a font change message, which will update
				the gallery. You should set it to FALSE after calling it. There are 
				situations where setting it to FALSE directly afterwards is also bad
				(Install All), so it's not done in the function itself.

				Due to this, it's a good idea to send one of these after calling this
				function to update the gallery and generally make sure everything is
				absolutely certain that the font table has changed:

					SendMessage(HWND_BROADCAST, WM_FONTCHANGE, 0, 0);

***********************************************************************************************/

BOOL FontsSGallery::DeinstallFont(String_256 *FontDesc, BOOL Delete, LOGFONT *LogFnt)
{
	ERROR3IF(!DontUpdate, "If you're calling install font or deinstall font, you MUST set DontUpdate to TRUE beforehand !");
	if(!DontUpdate) return FALSE;

	if(FontDesc->Sub(String_16(_R(IDS_FONTS_SPACE_TRUETYPE))) != -1)
	{
		if (DeinstallTTFFont(FontDesc, Delete))
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
	else
	{
		if (ATMInstall::DeinstallFont(FontDesc, TRUE/*Delete*/, LogFnt))
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
	return FALSE;
}

/***********************************************************************************************

>	BOOL FontsSGallery::UseWin95FontInstallMethod()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/6/97
	Inputs:		-
	Returns:	TRUE if we should use Win 95 font installation method.
				FALSE if we should use the WinNT 3.51 method using FOT files

	Purpose:	Centralised place for working out how to install a font on the current OS
***********************************************************************************************/

BOOL FontsSGallery::UseWin95FontInstallMethod()
{
	return CCamApp::IsNewWindowsUI();
}

/***********************************************************************************************

>	BOOL FontsSGallery::InstallTTFFont(PathName *FontFile, String_256 *Description, BOOL Temp)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/3/95

	Inputs:		FontFile points to the TTF file on the CD, or wherever it is.
				Description is used by the ini file to identify the font
				   Should be something like "Gill Sans Bold Italic (TrueType)"	
				Temp specifies if the installation is only a temporary one.
	
	Returns:	TRUE if installation went OK, FALSE on fail.

	Purpose:	Installs a truetype font file for use by the system, and Camelot.

	Notes:		The Temp flag doesn't do anything special yet.
				We shouldn't really install fonts in windows\system if it's set.
				Also, they don't need registering properly, etc...

				*** IMPORTANT ***
				You must set the static 'DontUpdate' to TRUE before calling this
				function, since it generates a font change message, which will update
				the gallery. You should set it to FALSE after calling it. There are 
				situations where setting it to FALSE directly afterwards is also bad
				(Install All), so it's not done in the function itself.

				Due to this, it's a good idea to send one of these after calling this
				function to update the gallery and generally make sure everything is
				absolutely certain that the font table has changed:

					SendMessage(HWND_BROADCAST, WM_FONTCHANGE, 0, 0);

***********************************************************************************************/

BOOL FontsSGallery::InstallTTFFont(PathName *FontFile, String_256 *Description, BOOL Temp)
{
	ERROR3IF(FontFile == NULL || Description == NULL, "FontsSGallery::InstallTTFFont null params are bad");
	ERROR3IF(!FontFile->IsValid(), "FontsSGallery::InstallTTFFont FontFile is invalid");
	ERROR3IF(!DontUpdate, "If you're calling install font or deinstall font, you MUST set DontUpdate to TRUE beforehand !");
	if(FontFile == NULL || Description == NULL || !FontFile->IsValid() || !DontUpdate)
		return FALSE;

	// Install the ttf file 'FontFile'

	// Work out where the system fot files live
	String_256 FontDir;

	if(!GetWindowsFontDirectory(&FontDir))
	{
		DWORD f = GetLastError();
		ERROR3("FontsSGallery::InstallTTFFont Problems getting the system dir");
		return FALSE;
	}

	BOOL ok = TRUE;
	PathName LibTTFPath(*FontFile);

 	String_256 SysTTF(FontDir);
	SGLibOil::AppendSlashIfNotPresent(&SysTTF);
	SysTTF += LibTTFPath.GetFileName();
	PathName TTFPath(SysTTF);

	PathName FOTPath(TTFPath);
	FOTPath.SetType((String_256)"FOT");

	String_256 KeyName(*Description);
	KeyName += String_16(_R(IDS_FONTS_SPACE_TRUETYPE));
	
	// Right, so:
	// FontDir = "C:\windows\system"
	// LibTTFPath = "D:\fonts\truetype\fish.ttf"
	// TTFPath = "C:\windows\system\fish.ttf"
	// FOTPath = "C:\windows\system\fish.fot"

	// Copy TTF file into windows\system (if we need to)
	if(!SGLibOil::FileExists(&TTFPath))
	{
		if(!SGLibOil::FileCopy(&LibTTFPath, &TTFPath))
		{
			ERROR3("FontsSGallery::InstallTTFFont Failed to copy TTF file into system directory");
			return FALSE;
		}
	}

	// Windows 95 is slightly different from the other two...
	// Since windows 95 doesn't require the concept of FOT files, we don't bother creating
	// a scalable font resource...
	if(UseWin95FontInstallMethod()) // IsWin32c())
	{
		// Add TTF file to windows font table
		//if(!AddFontResource((TCHAR *)TTFPath.GetPath()))
		if(!AddFontResource((const TCHAR *)TTFPath.GetFileName()))
		{
			DWORD f = GetLastError();
			ERROR3_PF(("FontsSGallery::InstllTTFFont Couldn't AddFontResource (W95) - %d", f));
			return FALSE;
		}

		// Add TTF file to windows registry / ini for use next time
		ok = SetFOTNameInRegistry(&KeyName, &TTFPath);

		if(!ok)
			ERROR3("FontsSGallery::InstallTTFFont setting win95 registry failed");
	}
	else
	{
		// do we already have a .FOT file 
		if(!SGLibOil::FileExists(&FOTPath))
		{
			// Create FOT file in windows\system
			String_256 TheFontDir(FontDir);
			SGLibOil::AppendSlashIfNotPresent(&TheFontDir);

			// To fix stack problems with pathname stuff
			String_256 FOTStr(FOTPath.GetPath());
			String_256 TTFStr(TTFPath.GetFileName());

			if(!CreateScalableFontResource(0,
					(TCHAR *)FOTStr,
					(TCHAR *)TTFStr,
					(TCHAR *)TheFontDir))
			{
				DWORD f = GetLastError();
				ERROR3_PF(("FontsSGallery::InstallTTFFont Couldn't create scalablefontresource - %d", f));
				return FALSE;
			}
		}

		// Add FOT file to windows font table
//		if(!AddFontResource((TCHAR *)FOTPath.GetPath()))	
		if(!AddFontResource((const TCHAR *)FOTPath.GetFileName()))	
		{
			DWORD f = GetLastError();
			ERROR3("FontsSGallery::InstallTTFFont Couldn't AddFontResource");
			return FALSE;
		}

		// Add FOT file to windows registry / ini for use next time
		//WriteProfileString("Fonts", (TCHAR *)KeyName, (TCHAR *)FOTPath.GetFileName());

		ok = SetFOTNameInRegistry(&KeyName, &FOTPath);
	}

	if(ok)
	{
		// Warn that the ini file has changed
		SendMessage(HWND_BROADCAST, WM_WININICHANGE, 0, (LPARAM) (LPCTSTR)"Fonts");

		// Warn everyone that a new font is in town
//		if(!IsWin32c())
			SendMessage(HWND_BROADCAST, WM_FONTCHANGE, 0, 0);
	}
	else
	{
		ERROR3("FontsSGallery::InstallTTFFont problems setting registry entries");
		return FALSE;
	}

	// Everything went ok...
	return TRUE;
}

/***********************************************************************************************

>	BOOL FontsSGallery::DeinstallTTFFont(String_256 *FontDesc, BOOL Delete);

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/3/95

	Inputs:		FontDesc - Description of font name to deinstall
						   Should be something like "Gill Sans Bold Italic (TrueType)"	
				Delete - If TRUE then delete the TTF file in Windows\System
						 Note, we delete the FOT file always... if we can...
	Returns:	TRUE then success and joyous screams
				FALSE if september
	Purpose:	Deinstalls a Truetype font file.	
	Notes:		By deinstall, we mean de-register the .fot file with the windows kernel and
				ourselves, delete the .FOT file and optionally delete the .TTF file in windows/
				system.

				*** IMPORTANT ***
				You must set the static 'DontUpdate' to TRUE before calling this
				function, since it generates a font change message, which will update
				the gallery. You should set it to FALSE after calling it. There are 
				situations where setting it to FALSE directly afterwards is also bad
				(Deinstall All), so it's not done in the function itself.

				Due to this, it's a good idea to send one of these after calling this
				function to update the gallery and generally make sure everything is
				absolutely certain that the font table has changed:

					SendMessage(HWND_BROADCAST, WM_FONTCHANGE, 0, 0);

***********************************************************************************************/

BOOL FontsSGallery::DeinstallTTFFont(String_256 *FontDesc, BOOL Delete)
{
	ERROR3IF(FontDesc == NULL, "FontsSGallery::DeinstallTTFFont FontDesc == NULL");
	ERROR3IF(DontUpdate == FALSE, "FontsSGallery::DeinstallTTFFont called with TRUE DontUpdate");

	if(!DontUpdate || FontDesc == NULL) return FALSE;
 
	PathName FOTFile;
	BOOL ok = GetFOTNameFromRegistry(FontDesc, &FOTFile);

	if(ok)
	{
		// Now this is totally bizarre...
		// Since there was an ambiguity in Microsoft's documentation it would seem
		// that it's ok to pass AddFontResource filenames only or filenames with paths.
		// Internally AddFontResource seems to have a limit of around 32 characters. If
		// the pathname is passed in and it's bigger than this then it chops the filename
		// off and uses that to reference the fonts in the font table (which we have no
		// way of seeing of course).
		//
		// Anyhow all of this triviality means we can't just pass RemoveFontResource a filename
		// or a pathname, as it screws up if it doesn't match the internally cached one, etc...
		//
		// To solve this, the below code passes does it twice, firstly with a filename then with
		// the full-length pathname... If BOTH return errors then something's gone wrong..
		//
		// This seems to fix the problems I was having, whether it's the correct way to go
		// about things is anyone's guess.. If Microsoft fancied documenting something useful,
		// instead of comparative reviews of debuggers, maybe we'd of had this whole project
		// finished several months ago...		

		// Remove the FOT file from the windows font table
		ok = RemoveFontResource((const TCHAR *)FOTFile.GetFileName());

#ifdef _DEBUG
		String_256 GLErrMsgF;

		if(!ok)
		{
			DWORD LastErr = GetLastError();
			wsprintf(GLErrMsgF,
					 TEXT("Problems with remove font resource (file) - GetLastError returned %d '%s'"),
					 LastErr, (const TCHAR *)FOTFile.GetPath());
		}
#endif

		BOOL ok2 = RemoveFontResource((const TCHAR *)FOTFile.GetPath());

		if(!ok && !ok2)
		{
#ifdef _DEBUG
			// Why does this sometimes fail ???
			DWORD LastErr = GetLastError();
			String_256 GLErrMsgP;
			wsprintf(GLErrMsgP,
					 TEXT("Problems with remove font resource (path) - GetLastError returned %d '%s'"),
					 LastErr, (const TCHAR *)FOTFile.GetPath());
			ERROR3((TCHAR*) GLErrMsgF);
			ERROR3((TCHAR*) GLErrMsgP);
#endif
		}

		// By one means or another we removed it from the font table
		if(!ok && ok2)
			ok = TRUE;

		if(ok)
		{	
			// Remove FOT file from windows registry / ini file
			ok = SetFOTNameInRegistry(FontDesc, NULL);
		
			if(ok && Delete)
			{
				// Since Windows 95 doesn't need FOT files, don't try deleting them...
				if(!UseWin95FontInstallMethod()) // !IsWin32c())
					SGLibOil::FileDelete(&FOTFile);			// Delete the FOT file

				if(FOTFile.SetType("TTF"))
					SGLibOil::FileDelete(&FOTFile);		// Delete the TTF file
			}

			// Warn that the ini file has changed
			SendMessage(HWND_BROADCAST, WM_WININICHANGE, 0, (LPARAM) (LPCTSTR)"Fonts");
		}

		// Warn that the windows font table is now different
		SendMessage(HWND_BROADCAST, WM_FONTCHANGE, 0, 0);
	}

	if(!ok)
	{
		// Problems deinstalling a specified font - tell the user
		String_256 ErrMsg;
		ErrMsg.MakeMsg(_R(IDS_PROBLEMS_DEINSTALLING), (TCHAR *)*FontDesc);
		Error::SetError(0, ErrMsg, 0);
		InformError();
		Error::ClearError();
	}

	return ok;
}


/***********************************************************************************************

>	static BOOL FontsSGallery::InstallDraggedLibFont(SuperGallery *FontGallery,
								SGLibDisplayItem *FontItem, String_64 *Desc);

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/4/95

	Inputs:		FontGallery points to the fonts gallery
				FontItem is the library font item to install
	Outputs:	Desc will contain the textual description for the font:
					"Gill Sans Bold Italic (TrueType)"
	
	Returns:	TRUE if installation went OK, FALSE on fail.

	Purpose:	Installs a truetype font file from a drag (ie it checks that it's ok
				to install it with the user first).
	Notes:

***********************************************************************************************/

BOOL FontsSGallery::InstallDraggedLibFont(FontsSGallery *FontGallery, SGLibDisplayItem *FontItem, String_64 *Desc)
{
	BOOL ok = TRUE;

	if(ThisGallery == NULL || FontItem == NULL || Desc == NULL)
	{
		ERROR3("FontsSGallery::InstallDraggedLibFont - null params are BAD !!!");
		return FALSE;
	}

	// Check ATM's running if this font is an ATM font. If not, error and stop...
	FontClass Class	= ((SGLibFontItem *)FontItem)->GetType();
	if(Class == FC_ATM)
	{
		if(!OILFontMan::IsOkToCall(FC_ATM))
		{
			if(!WarnAboutMultipleATMDragging)
			{
				InformWarning(_R(IDS_ATM_NOT_RUNNING), _R(IDS_OK));
				Error::ClearError();
				WarnAboutMultipleATMDragging = TRUE;
			}
			return FALSE;
		}			
	}

	*Desc = (String_64)"";

#ifdef STOP_WINDOWS95_FONT_INSTALLS
	if(IsWin32c())
	{
		ERROR3("Still problems installing on win95");
		return FALSE;
	}
#endif

	String_256 OurDesc;

	// This one does the extra Bold / Italic Biz...
	if(FontItem->IsKindOf(CC_RUNTIME_CLASS(SGLibFontItem)))
		((SGLibFontItem *)FontItem)->GetNameText(&OurDesc);
	else
	{
		if(!FontItem->GetDisplayedTextDescription(&OurDesc))
			return FALSE;
	}

	if(OurDesc.Length() == 0)
		return FALSE;

	if(IsFontAlreadyInstalled(&OurDesc, ((SGLibFontItem *)FontItem)->GetType()))
	{
		//ERROR3("This font is already installed");
		*Desc = (String_64)OurDesc;
		return TRUE;
	}
	
	PathName FileName;		
	ok = (FontItem->GetFileName(&FileName));
	
	if(ok)
	{
		// check if it's ok to install this font with the user
		String_256 WarnMsg;
		WarnMsg.MakeMsg(_R(IDS_FONTS_GALLERY_SURE_INSTALL), (TCHAR *)OurDesc);
		Error::SetError(0, WarnMsg, 0);
		INT32 ButtonPressed = InformWarning(0, _R(IDS_INSTALL), _R(IDS_CANCEL));
		Error::ClearError();

		// install the library font
		if (ButtonPressed == 1)
		{
			//("Installing 'FRED'");
			Library* pFontLib = FontItem->GetParentLibrary();
			if (!pFontLib)
			{
				ERROR3("Illegal NULL pointer");
				return FALSE;
			}
			if (pFontLib->IsWebLibrary()) // if so we have to download the font first (font files are never cached locally)
			{
				String_256 strFileURL = FileName.GetPath(FALSE);
				pFontLib->LocalPath2URL(&strFileURL);
				OpDescriptor* pOpDesc =	OpDescriptor::FindOpDescriptor(CC_RUNTIME_CLASS(OpAsynchFontInstall));
				if (pOpDesc != NULL)
				{
					WebFontInstallParam* Param = new WebFontInstallParam;
					ERROR2IF(!Param, FALSE, "Memory allocation error");
					// Set up the parameters which we need to download and install the font
					Param->type = TYPE_FONT;
					Param->priority = AsynchDownload::PRIORITY_HIGH;
					Param->pGallery = FontGallery;
					Param->file = FileName;
					FontItem->GetDisplayedTextDescription(&Param->strDescription);
					FontItem->GetNameText(&Param->strAltDescription);
					Param->strURL = strFileURL;
					Param->bIsTemp = FALSE;
					Param->Output = &ok;
					// Invoke the operation
					pOpDesc->Invoke((OpParam*) Param);
				}
				else
				{
					ERROR3("Can't find Op descriptor");
				}
			}
			else // do a normal install
			{
				//("Installing 'FRED'");
				String_64 SlowJob;
				SlowJob.MakeMsg(_R(IDS_FONTS_INSTALLING), (TCHAR *)OurDesc);
				BeginSlowJob(-1, FALSE, &SlowJob);
				DontUpdate = TRUE;
				// Added this to sort out dragging ATM install problems...			
				String_256 ProperName;
				FontItem->GetDisplayedTextDescription(&ProperName);
				ok = ThisGallery->InstallFont(&FileName, &ProperName, FALSE);
				//ok = ThisGallery->InstallFont(&FileName, &OurDesc, FALSE);
				DontUpdate = FALSE;
				EndSlowJob();
				if (ok)	// Warn everyone that the fonts have changed - and update our gallery
					SendMessage(HWND_BROADCAST, WM_FONTCHANGE, 0, 0);
			}
			if(!ok)
			{
				// Problems installing a specified font
				String_256 ErrMsg;
				ErrMsg.MakeMsg(_R(IDS_PROBLEMS_INSTALLING), (TCHAR *)OurDesc);
				Error::SetError(0, ErrMsg, 0);
				InformError();
				Error::ClearError();
			}
		}
	}

	// Set the description
	*Desc = (String_64)OurDesc;

	return ok;
}

/***********************************************************************************************

>	static BOOL FontsSGallery::FindLogFont(String_256 *FontDesc, LOGFONT **lplplf, FontClass Class = FC_UNDEFINED)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/5/95

	Inputs:		FontDesc string "Autumn Regular", "Fred Bold Oblique", etc...
				FontClass - Class of font to look for	
	Outputs:	lplplf - points the variable we're pointing at to the logfont
	Returns:	TRUE if font found in installed group

	Purpose:	Check whether a font is already installed, if it is, return it's logfont

	Notes:		We check the whole string, so fonts of the same typeface but with differing
				attributes will return FALSE

	SeeAlso:	FontsSGallery::IsFontAlreadyInstalled

***********************************************************************************************/

BOOL FontsSGallery::FindLogFont(String_256 *FontDesc, LOGFONT **lplplf, FontClass Class)
{
	if(FontDesc == NULL || lplplf == NULL)
	{
		ERROR3("FontsSGallery::FindLogFont given null params");
		return FALSE;
	}

	if(InsGroup == NULL)
	{
		ERROR3("You don't seem to have an installed font group. This is rather bad !");
		return FALSE;
	}

	*lplplf = NULL;

	// Get the first item in the group...
	SGDisplayNode *Item = InsGroup->GetChild();

	// Name of item currently examining
	String_256 ItemName;
	
	// Lower case version of the font we're looking for
	String_256 FontDescLower(*FontDesc);
	FontDescLower.toLower();

	// Loop through all the group items
	while (Item != NULL)
	{
		// Check we're an installed font
		if(Item->IsKindOf(CC_RUNTIME_CLASS(SGDisplayPreviewFonts)))
		{
			SGDisplayPreviewFonts *FontItem = (SGDisplayPreviewFonts *) Item;

			if((Class == FontItem->Type) || (Class == FC_UNDEFINED))
			{
				// Get the name of the font item
				FontItem->GetNameText(&ItemName);
				ItemName.toLower();

				// Do the two names match ?
				if(ItemName == FontDescLower)
				{
					*lplplf = FontItem->CachedLogFont;
					return TRUE;
				}
			}
		}

		// If we've reached the end, this will be NULL
		Item = Item->GetNext();
	}

	return FALSE;
}

/***********************************************************************************************

>	static BOOL FontsSGallery::IsFontAlreadyInstalled(String_256 *FontDesc, FontClass Class = FC_UNDEFINED);

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/5/95

	Inputs:		FontDesc string "Autumn Regular", "Fred Bold Oblique", etc...
				Class - the type of font we're interested in finding
	Outputs:
	Returns:	TRUE if font found in installed group

	Purpose:	Check whether a font is already installed, to stop various irritatingly
				pointless warning messages.

	Notes:		We check the whole string, so fonts of the same typeface but with differing
				attributes will return FALSE

***********************************************************************************************/

BOOL FontsSGallery::IsFontAlreadyInstalled(String_256 *FontDesc, FontClass Class)
{
	LOGFONT *lplf = NULL;
	return FindLogFont(FontDesc, &lplf, Class);
}


/********************************************************************************************

>	virtual void FontsSGallery::WorkOutSectionName(String_256 *Section);

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/5/95

	Outputs:	Section	- String used to return the section name

	Purpose:	Returns the section name to use in the grm file

********************************************************************************************/

void FontsSGallery::WorkOutSectionName(String_256 *Section)
{
	if(Section)
		Section->MakeMsg(_R(IDS_FONTS_GALLERY));
}





/***********************************************************************************************

>	static BOOL GetWindowsFontDirectory(String_256 *Result);

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/3/95

	Inputs:		
	Outputs:	String pointing to location that windows will look for its fot font files
	Returns:	TRUE if a recognised OS, and valid path

	Purpose:	Since various versions of windows do things is various different ways,
				we need to work out where the fot files actually live, because there isn't
				a system call to do this...

	Notes:		THIS WILL NOT WORK ON OS/2 (probably)...

***********************************************************************************************/

BOOL FontsSGallery::GetWindowsFontDirectory(String_256* Result)
{
	if (Result == NULL)
	{
		ERROR3("FontsSGallery::GetWindowsFontDirectory given a NULL string to return to");
		return FALSE;
	}

	String_256 WindowsDir;
	String_256 FontDir;
	
	BOOL RecognisedWindows = FALSE;

	*Result = TEXT("");

	if (GetWindowsDirectory((TCHAR*) WindowsDir, 255) != 0)
	{
		if (IsWindows31()) // IsWin32s() && !IsWin32c())
		{
			wsprintf((TCHAR*) FontDir, TEXT("%s\\System"), (LPCTSTR) WindowsDir);
			RecognisedWindows = TRUE;
		}

		if (UseWin95FontInstallMethod()) // IsWin32c())
		{
			wsprintf((TCHAR*) FontDir, TEXT("%s\\Fonts"), (LPCTSTR) WindowsDir);
			RecognisedWindows = TRUE;
		}

		if (!UseWin95FontInstallMethod() && IsWindowsNT()) // IsWin32NT())
		{
			wsprintf((TCHAR*) FontDir, TEXT("%s\\System"), (LPCTSTR) WindowsDir);
			RecognisedWindows = TRUE;
		}
	}

	if (RecognisedWindows)
	{
		*Result = FontDir;
		return TRUE;
	}

	return FALSE;
}

/********************************************************************************************

>	BOOL FontsSGallery::GetFOTNameFromRegistry(String_256 *Desc, PathName *RetBuf)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/4/95

	Inputs:		Desc - Font description - "FancyDress (TrueType)"
				FOTFile - returns the pathname of the fot file associated with 'Desc'
	Outputs:
	Returns:	TRUE if we got the string ok

	Purpose:	Return the filename (and path) of the fot file associated with the given font
				description.

	Notes:		Windows 3.1, NT and W95 all do this in different ways !

				Windows 3.1 info is in c:\windows\win.ini
				Windows 3.11 info is the same as 3.1
				Windows NT info is in the registry BUT calls to the ini file get redirected
				Windows 95 info is in the registry, but doesn't redirect registry calls.
				OS/2 - not supported...

				Win32 functions that work in Windows 95
					n	RegCreateKeyEx
					n	RegDeleteValue
					n	RegEnumKeyEx
					n	RegEnumValue
					n	RegFlushKey
					n	RegOpenKeyEx
					n	RegQueryInfoKey
					n	RegQueryValueEx
					n	RegSetValueEx
			
********************************************************************************************/

BOOL FontsSGallery::GetFOTNameFromRegistry(String_256 *Desc, PathName *FOTFile)
{
	if (Desc == NULL || FOTFile == NULL)
	{
		ERROR3("FontsSGallery::GetFOTNameFromRegistry given NULL params - bad !");
		return FALSE;
	}

	String_256 Description(*Desc);
	String_256 FOTBuf(TEXT(""));

	BOOL ok = FALSE;

	// win.ini functions for windows 3.1
	if (IsWindows31()) // IsWin32s() && !IsWin32c())
	{
		String_8 Fonts(TEXT("Fonts"));
		String_32 Default(TEXT(""));
		ok = GetProfileString((TCHAR *)Fonts, (TCHAR *)Description, (TCHAR *)Default, (TCHAR *)FOTBuf, 256);
	}

	// Registry functions for NT and beyond
	if (IsWindows95() || IsWindowsNT()) // IsWin32c() || IsWin32NT())
	{
		String_256 Value(TEXT(""));
		DWORD Length = 255;
		HKEY KeyHandle = 0;	
		DWORD dwType = 0;
		INT32 lResult = ERROR_SUCCESS;

		String_256 FontKey;
		ok = GetFontsKey(&FontKey);

		if (ok)
		{
			// Open the fonts key for reading
			lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, (TCHAR *)FontKey, 0, KEY_READ, &KeyHandle);
			if (lResult != ERROR_SUCCESS)
			{
				ERROR3("FontsSGallery::GetFOTNameFromRegistry had a problem with the registry - 1");
				return FALSE;
			}

			// Now get the value from the registry
			lResult = RegQueryValueEx(KeyHandle, (TCHAR *)Description, NULL,
									&dwType, (unsigned char *)((TCHAR *)Value), &Length);
			if (lResult != ERROR_SUCCESS)
			{
				ERROR3("FontsSGallery::GetFOTNameFromRegistry had a problem with the registry - 3");
				return FALSE;
			}

			FOTBuf = Value;
			ok = TRUE;
		}
	}

	if (ok)
	{
		// Check whether we can find the file with what was in the registry
		PathName FOTBufPath(FOTBuf);
		ok = FOTBufPath.IsValid();

//		if(SGLibOil::FileExists(&FOTBufPath))
		String_256 Location(FOTBufPath.GetLocation());
		if(Location.Length() != 0)
		{
			TRACEUSER( "Richard", _T("Registry contained entire path for fot file -> %s\n"), (LPCTSTR) FOTBuf);
			ok = FOTFile->SetPathName(FOTBuf);
			if (ok) ok = FOTFile->IsValid();
			return ok;
		}

		// Work out where the system fot files live and add this to the fot's returned path
		String_256 FontDir;
		ok = GetWindowsFontDirectory(&FontDir);

		if (ok)
		{
		 	String_256 SysTTF;
			wsprintf((TCHAR*) SysTTF, TEXT("%s\\%s"),
					 (LPCTSTR) FontDir, (LPCTSTR) FOTBufPath.GetFileName());
			ok = FOTFile->SetPathName(SysTTF);
			if (ok) ok = FOTFile->IsValid();
		}
	}
	return ok;
}


/********************************************************************************************

>	BOOL FontsSGallery::SetFOTNameInRegistry(String_256 *Desc, PathName *FOTFile)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/4/95

	Inputs:		Desc - Font description - "FancyDress (TrueType)"
				FOTFile - Filename of the fot file associated with 'Desc'
						  Set to NULL to delete the current entry
	Outputs:
	Returns:	TRUE if we wrote the string ok

	Purpose:	Return the filename of the fot file associated with the given font
				description.

	Notes:		Windows 3.1, NT and W95 all do this in different ways !
			
********************************************************************************************/

BOOL FontsSGallery::SetFOTNameInRegistry(String_256 *Desc, PathName *FOTFile)
{
	if(Desc == NULL)
	{
		ERROR3("FontsSGallery::GetFOTNameFromRegistry given NULL params - bad !");
		return FALSE;
	}

	BOOL ok = FALSE;

	String_256 Description(*Desc);

	// win.ini functions
	if(IsWindows31()) // IsWin32s() && !IsWin32c())
	{
		if(FOTFile == NULL)
			ok = WriteProfileString((TCHAR *)"Fonts", (TCHAR *)Description, NULL);
		else
			ok = WriteProfileString((TCHAR *)"Fonts", (TCHAR *)Description, (const TCHAR *)FOTFile->GetFileName(TRUE));
	}

	// Registry functions
	if(IsWindows95() || IsWindowsNT()) // IsWin32c() || IsWin32NT())
	{
		HKEY KeyHandle = 0;	
		INT32 lResult = ERROR_SUCCESS;

		String_256 FontKey;
		ok = GetFontsKey(&FontKey);

		if(ok)
		{
			// Open the fonts key, ready for action
			lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, (TCHAR *)FontKey, 0, KEY_ALL_ACCESS, &KeyHandle);
			if(lResult != ERROR_SUCCESS)
			{
				ERROR3("FontsSGallery::SetFOTNameInRegistry had a problem with the registry - 1");
				if(FOTFile != NULL)
				{
					ERROR3_PF(("Show these to Richard: (Ins) FontKey = %s, lResult = %d, Fotpath = %s",(TCHAR *)FontKey, lResult, (const TCHAR *)FOTFile->GetPath()));
				}
				else
				{
					ERROR3_PF(("Show these to Richard: (Del) FontKey = %s, lResult = %d, Desc = %s",(TCHAR *)FontKey, lResult, (TCHAR *)Description));
				}
				return FALSE;
			}

			if(FOTFile == NULL)
			{
				// Delete the appropriate entry in the subkey
				lResult = RegDeleteValue(KeyHandle, (TCHAR *)Description);

				if(lResult != ERROR_SUCCESS)
				{
					ERROR3("FontsSGallery::SetFOTNameInRegistry had a problem deleting the font key in the registry");
					return FALSE;
				}

				ok = TRUE;
			}
			else
			{
				// Write the new value to the subkey			
				String_256 Value;
				
				// Windows 95 seems to like the whole path in its registry
				if(IsWindows95()) // IsWin32c())
					Value = (const TCHAR *)FOTFile->GetPath();
				else
					Value = (const TCHAR *)FOTFile->GetFileName(TRUE);

				// Set the value to our FOT file name (without the path)
				lResult = RegSetValueEx(
				    KeyHandle,							// handle of key to set value for  
				    (TCHAR *)Description,				// font description
				    NULL,			 					// reserved 
				    REG_SZ,								// it's a zero terminated string
				    (CONST BYTE *) ((TCHAR *)Value),	// FOT filename - value data 
				    (DWORD) Value.Length()+1			// size of value data 
				   );	

				if(lResult != ERROR_SUCCESS)
				{
					ERROR3("FontsSGallery::SetFOTNameInRegistry had a problem writing to the registry");
					return FALSE;
				}

				// Commit the data to the registry right now...
				ok = (RegFlushKey(KeyHandle) == ERROR_SUCCESS);
			}
		}
	}
	return ok;
}

/********************************************************************************************

>	BOOL FontsSGallery::GetFontsKey(String_256 *Key)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/4/95

	Inputs:
	Outputs:	Key returns the registry keyname of the fonts key under this OS
	Returns:	TRUE if a recognised OS, etc...

	Purpose:	Return the registry keyname of the fonts key under this OS

	Notes:		Windows NT and W95 have different font key names...
			
********************************************************************************************/

BOOL FontsSGallery::GetFontsKey(String_256 *Key)
{
	*Key = (TCHAR *)"";

	if(IsWindowsNT()) // IsWin32NT())
	{
		*Key = TEXT("Software\\Microsoft\\Windows NT\\CurrentVersion\\Fonts");
		return TRUE;
	}
	
	if(IsWindows95()) // IsWin32c())
	{
		*Key = TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Fonts");
		return TRUE;
	}

	return FALSE;
}

/********************************************************************************************

>	void FontsSGallery::SelectionHasChanged(void);

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/3/95 (from sgallery.cpp)

	Purpose:	To inform the gallery that the selection has changed in some way.
				We need this for greying install / uninstall / apply, etc...
	Notes:		This is an override for the base class function which can't do custom
				buttons.
			
********************************************************************************************/

void FontsSGallery::SelectionHasChanged(void)
{
	if (DisplayTree == NULL)
		return;

	// As we now allow the gallery to be closed and all the groups to remain, we must now
	// also check to see if the gallery is visible i.e. open or not. If not then get out now.
	// Required as SetStringGadgetValue will ERROR and memory leak if called when dialog closed.
	if (!IsVisible())
		return;

#ifdef STOP_WINDOWS95_FONT_INSTALLS
	// Install if > one selected library item && not windows 95
	EnableGadget(_R(IDC_GALLERY_INSTALL), (LibraryFontsSelected() > 0) && !IsWin32c());

	// Deinstall if > one selected installed item && not windows 95
	EnableGadget(_R(IDC_GALLERY_DEINSTALL),	InstalledFontsSelected() && !IsWin32c());
#else
	// Install if > one selected library item
	EnableGadget(_R(IDC_GALLERY_INSTALL), (LibraryFontsSelected() > 0));

	// Deinstall if > one selected installed item
	EnableGadget(_R(IDC_GALLERY_DEINSTALL),	InstalledFontsSelected());
#endif

#ifdef STANDALONE	
	// Can't apply in StandAlone version
	EnableGadget(_R(IDC_GALLERY_APPLY),	FALSE);
#else
	// Apply needs exactly 1 item
	SGLibFontItem* pItem = (SGLibFontItem*) DisplayTree->FindNextSelectedItem(NULL);
	if (pItem)
	{
		String_256 Desc256;
		pItem->GetNameText(&Desc256);
		if (!IsFontAlreadyInstalled(&Desc256, pItem->GetType()) && pItem->GetParentLibrary() && pItem->GetParentLibrary()->IsWebLibrary())
			SetStringGadgetValue(_R(IDC_GALLERY_APPLY), _R(IDS_SGMENU_DOWNLOAD));
		else
			SetStringGadgetValue(_R(IDC_GALLERY_APPLY), _R(IDS_SGMENU_APPLY));
	}
	EnableGadget(_R(IDC_GALLERY_APPLY),	(DisplayTree->GetSelectedItemCount() == 1) && Document::GetSelected() != NULL);
#endif

	EnableGadget(_R(IDC_LIBGAL_ADD_FONTS), TRUE);

	// Does the standard library buttons
	LibraryGallery::SelectionHasChanged();
}

/********************************************************************************************

>	void FontsSGallery::DoShadeGallery(BOOL ShadeIt)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com> (based on Jason code)
	Created:	28/3/95

	Inputs:		ShadeIt - True if we want to actually shade the gallery buttons
	Outputs:
	Returns:

	Purpose:	To un-grey the options... button when there are no docs
	Notes:

********************************************************************************************/

void FontsSGallery::DoShadeGallery(BOOL ShadeIt)
{
	if (DisplayTree == NULL)
		return;

	// Sort the normal buttons out
	SelectionHasChanged();

	if(ShadeIt)
	{
		EnableGadget(_R(IDC_GALLERY_APPLY),	FALSE);
	}

	EnableGadget(_R(IDC_LIBGAL_ADD_FONTS), TRUE);

	// Does the standard library buttons
	LibraryGallery::DoShadeGallery(ShadeIt);
}
										  
/********************************************************************************************

>	BOOL FontsSGallery::InstalledFontsSelected(void)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/3/95
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if any installed fonts are selected
	Purpose:	For button greying out and the like
	SeeAlso:	SelectionHasChanged

********************************************************************************************/

BOOL FontsSGallery::InstalledFontsSelected(void)
{
	if(InsGroup == NULL)
	{
		ERROR3("No installed fonts group detected");
		return FALSE;
	}

	SGDisplayPreviewFonts *Selected = NULL;

	if (InsGroup != NULL)
		Selected = (SGDisplayPreviewFonts *) InsGroup->FindNextSelectedItem(NULL);
								 
	// Found one
	return (Selected != NULL);
}	


/********************************************************************************************

>	INT32 FontsSGallery::LibraryFontsSelected(void)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/3/95
	Inputs:		-
	Outputs:	-
	Returns:	Number of fonts selected in the library section
	Purpose:	For button greying out and the like
	SeeAlso:	SelectionHasChanged

********************************************************************************************/

INT32 FontsSGallery::LibraryFontsSelected(void)
{
	if (DisplayTree == NULL)
	{
		ERROR3("FontsSGallery::LibraryFontsSelected No display tree - bad !");
		return 0;
	}

	INT32 Count = 0;

	SGDisplayNode *Item = DisplayTree->FindNextSelectedItem(NULL);

	while (Item != NULL)
	{
		if(Item->IsKindOf(CC_RUNTIME_CLASS(SGLibFontItem)))
		{
			// Found one
			if(Item->Flags.Selected)
				Count ++;
		}

		Item = SGDisplayRoot::FindNextItemInTree(Item);
	}
			   
	return Count;
}

/********************************************************************************************

>	virtual BOOL FontsSGallery::CanSearchKeywords(void)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/3/95

	Returns:	FALSE

	Purpose:	Used to determine if this type of gallery supports keyword searching.
				This one doesn't (yet).
				
********************************************************************************************/

BOOL FontsSGallery::CanSearchKeywords(void)
{
	return(FALSE);
}


/***********************************************************************************************

>	static INT32 FontsSGallery::GetDragFontOffset(KernelBitmap *Bmp)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/t/95

	Inputs:		Bmp	- KernelBitmap which we'll be using to drag
	Returns:	Centre of pixel data in the bmp

	Purpose:	Font gallery previews often only have a small bmp coverage, with a large
				blank area over to the right hand side.
				
				In order to 'pick them up' in the centre, we need to work out the centre of
				the actual data in the bitmap.
				
				This routine loops through the bmp pixels, and works out the largest X pixel
				which has non-background data in. It then returns this divided by 2...

	Notes:		It only works for 8bpp bitmaps and assumes values lower than 250 are foreground
				pixels, since the font gallery uses 8bpp greyscaled bmps...

***********************************************************************************************/

INT32 FontsSGallery::GetDragFontOffset(KernelBitmap *Bmp)
{
	if(Bmp == NULL || Bmp->ActualBitmap == NULL)
		return 0;

	BitmapInfo TmpInfo;
	Bmp->ActualBitmap->GetInfo(&TmpInfo);

	INT32 XSize = TmpInfo.PixelWidth;
	INT32 YSize = TmpInfo.PixelHeight;
	INT32 BPP = TmpInfo.PixelDepth;
	INT32 LargestXFound = 0;

	if(BPP == 8 && XSize > 0 && YSize > 0)
	{
		for(INT32 y = 0; y < YSize; y++)
		{
			for(INT32 x = LargestXFound; x < XSize; x++)
			{
				PixelGreyscale Pixel = Bmp->ReadPixelGreyscale(x, y);
	
				if(Pixel < 250)
					LargestXFound = x;
			}
		}
	}

	if(LargestXFound == 0)
		return 0;
	else
	{
		// We need to return an offset from the centre of the bitmap...
		return ((XSize / 2) - (LargestXFound / 2));
	}
}













/***********************************************************************************************

>	virtual SGDisplayGroup *FontsSGallery::AddLibraryGroup(Library *LibraryToDisplay, INT32 NumItems)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/4/95

	Inputs:		LibraryToDisplay - Pointer to the library associated with the group
				NumItems - Number of items in the group
	Returns:	The newly created library group, or NULL if there were problems
	Purpose:	Create a library group, as opposed to a display group
	Notes:

***********************************************************************************************/

SGDisplayGroup *FontsSGallery::AddLibraryGroup(Library *LibraryToDisplay, INT32 NumItems)
{
	ERROR3IF(LibraryToDisplay == NULL, "FontsSGallery::AddLibraryGroup - NULL parameter is illegal");

	if (DisplayTree == NULL)
	{
		ERROR3("FontsSGallery::AddLibraryGroup called before the DisplayTree was initialised!");
		return(NULL);
	}

	SGLibGroup *TheGroup = (SGLibGroup *)DisplayTree->FindSubtree(this, NULL, LibraryToDisplay);

	if (TheGroup == NULL)
	{
		// No existing group for that library, so create a new one
		TheGroup = new SGLibGroup(this, NULL, LibraryToDisplay);

		if (TheGroup == NULL)				// Failed!
			return(NULL);

		// And add it to our display tree
		DisplayTree->AddItem(TheGroup);
	}
	else
		TheGroup->DestroySubtree(FALSE);	// Delete all items in the group

	return(TheGroup);
}


/********************************************************************************************

>	virtual SGDisplayItem *FontsSGallery::CopyDisplayItem(SGDisplayItem *SourceItem, 
													SGDisplayGroup *DestGroup,
													SGDisplayItem *TargetPosition = NULL)
	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/4/95

	Inputs:		SourceItem - The item to copy elsewhere in the tree (see below)

				DestGroup - The group into which the item should be inserted

				TargetPosition - NULL (to add the copied item to the end of the sibling
				list), or points to an item BEFORE which the copied item will be inserted.

	Returns:	NULL (failed) or a pointer to the new (copied) display item

	Purpose:	"Copies" the existing node in the tree in an appropriate fashion.
				
				This method is normally called when a gallery-organising drag completes,
				and it is discovered that the dragged item(s) have been dragged to a 
				different display group.

	Notes:		This method should be overridden by derived galleries to provide
				appropriate behaviour (some galleries (e.g colour) will copy the real-item
				that the given display-item references to the new group (document), while
				other galleries (layer) may just move the item after all).

				Note the handy InsertCopiedItem and MoveBefore/After methods which
				are available to take all of the hard work out of copying/moving items!

				See the body of this method in the source code for example pseudocode.
				For real code, see the Colour Gallery (sgcolour.cpp)

	SeeAlso:	SuperGallery::InsertCopiedItem; SGDisplayItem::MoveBefore;
				SGDisplayItem::MoveAfter; ColourSGallery::CopyDisplayItem

********************************************************************************************/

SGDisplayItem *FontsSGallery::CopyDisplayItem(SGDisplayItem *SourceItem, 
								SGDisplayGroup *DestGroup, SGDisplayItem *TargetPosition)
{
	if(SourceItem == NULL || DestGroup == NULL)
	{
		ERROR3("FontsSGallery::CopyDisplayItem -> Illegal NULL param");
		return NULL;
	}

	if(DestGroup->IsKindOf(CC_RUNTIME_CLASS(SGFontsGroup)))
	{
		if(SourceItem->IsKindOf(CC_RUNTIME_CLASS(SGLibFontItem)))
		{
			// A library font has been dragged into the installed group... install it
			String_64 Desc;
			FontsSGallery::InstallDraggedLibFont(FontsSGallery::ThisGallery, (SGLibDisplayItem *)SourceItem, &Desc);
			TRACEUSER( "Richard", _T("%s installed\n"), (TCHAR *)Desc);
		}
		else
		{
			// ERROR3("Drag of installed font into installed group");
		}
	}

	if(DestGroup->IsKindOf(CC_RUNTIME_CLASS(SGLibGroup)))
	{
		if(SourceItem->IsKindOf(CC_RUNTIME_CLASS(SGLibFontItem)))
		{
			// ERROR3("Drag of lib font into lib group");
			SGDisplayNode *SourceGroup = SourceItem->GetParent();
			if((SGDisplayGroup *)SourceGroup != DestGroup)
			{
				// Can't move fonts between sections
				FontsSGallery::WarnAboutSectionDragging = TRUE;
				//InformWarning(_R(IDS_FONTS_BETWEEN_GROUPS));
				//Error::ClearError();
			}
		}
		else
		{
			// ERROR3("Drag of installed font into lib group");
		}
	}

	return(NULL);
}

/********************************************************************************************

>	virtual void SuperGallery::AllItemsCopied(SGDisplayGroup *DestGroup)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/2/96

	Inputs:		DestGroup - Indicates where the copy/move took place.

	Purpose:	This is used to warn about multiple items being dragged into bad places, once only
				It's called AFTER releasing the mouse button with the drag
	  
********************************************************************************************/

void FontsSGallery::AllItemsCopied(SGDisplayGroup *DestGroup)
{
	// Can't move fonts between sections
	if(WarnAboutSectionDragging)
		InformWarning(_R(IDS_FONTS_BETWEEN_GROUPS));

	WarnAboutMultipleATMDragging = FALSE;
	WarnAboutSectionDragging = FALSE;
}



/***********************************************************************************************

>	static void FontsSGallery::MakeBoldItalicReadable(String_256 *Desc)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/11/95

	Inputs:		Desc (String ending with ";BOLDITALIC", etc)
	Outputs:	Desc (String ending with " Bold Italic", etc)

	Purpose:	To make an internal / ini file version of the string, readable
	
***********************************************************************************************/

void FontsSGallery::MakeBoldItalicReadable(String_256 *Desc)
{
	// Change lines which end ;BOLDITALIC to ' Bold Italic' (ATM only really)
	if(Desc->Sub(String_8(_R(IDS_FONTS_ATM_EFFECT_SEPARATOR))) != -1)
	{
		String_256 TmpResult(*Desc);
		TmpResult.Left(Desc, TmpResult.Sub(String_8(_R(IDS_FONTS_ATM_EFFECT_SEPARATOR))));

		if(TmpResult.Sub(String_16(_R(IDS_FONTS_CAPITAL_BOLD))) != -1)
			*Desc += String_16(_R(IDS_FONTS_SPACE_BOLD));

		if(TmpResult.Sub(String_16(_R(IDS_FONTS_CAPITAL_ITALIC))) != -1)
			*Desc += String_16(_R(IDS_FONTS_SPACE_ITALIC));
	}
}


//-----------------------------------------------------------------------------------------------

// OpDisplayFontsGallery - the operation that is used to display the fonts gallery

/********************************************************************************************

>	BOOL OpDisplayFontsGallery::Init()

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/1/95
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the operation could be successfully initialised 
				FALSE if no more memory could be allocated 
				
	Purpose:	OpDisplayFontsGallery initialiser method
	Errors:		ERROR will be called if there was insufficient memory to allocate the 
				operation.
	SeeAlso:	-

********************************************************************************************/

BOOL OpDisplayFontsGallery::Init()
{

	return (RegisterOpDescriptor(
	 							0,
	 							_R(IDS_DISPLAY_FONTS_GALLERY),
								CC_RUNTIME_CLASS(OpDisplayFontsGallery),
	 							OPTOKEN_DISPLAYFONTSGALLERY,
	 							OpDisplayFontsGallery::GetState,
	 							0,	/* help ID */
	 							_R(IDBBL_DISPLAY_FONTS_GALLERY),
	 							0	/* fonts ID */));

	

}               
    
/********************************************************************************************

>	OpState	OpDisplayFontsGallery::GetState(String_256* UIDescription, OpDescriptor*)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/1/95
	Inputs:		-
	Outputs:	-
	Returns:	The state of the OpDisplayFontsGallery operation
	Purpose:	For finding the OpDisplayFontsGallery's state. 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

OpState	OpDisplayFontsGallery::GetState(String_256* UIDescription, OpDescriptor*)
{
	OpState OpSt;  

	// If the gallery is currenty open, then the menu item should be ticked
	String_32 Name(_R(IDS_FONTS_GALLERY_OP));
	DialogBarOp* pDialogBarOp = DialogBarOp::FindDialogBarOp(Name);

	if (pDialogBarOp != NULL)
	{
		if (pDialogBarOp->GetRuntimeClass() == CC_RUNTIME_CLASS(FontsSGallery))
			OpSt.Ticked = pDialogBarOp->IsVisible();
	}

 	return(OpSt);   
}

/********************************************************************************************

>	void OpDisplayFontsGallery::Do(OpDescriptor*)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/1/95
	Inputs:		A pointer to an OpDescriptor
	Outputs:	-
	Returns:	-
	Purpose:	Displays the fonts gallery
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void OpDisplayFontsGallery::Do(OpDescriptor*)
{

	String_32 Name(_R(IDS_FONTS_GALLERY_OP));
	DialogBarOp* pDialogBarOp = DialogBarOp::FindDialogBarOp(Name);
  
	if (pDialogBarOp != NULL)
	{
		// Toggle the visible state of the gallery window
		pDialogBarOp->SetVisibility( !pDialogBarOp->IsVisible() );

		// And update our button state
		SGInit::UpdateGalleryButton(OPTOKEN_DISPLAYFONTSGALLERY, pDialogBarOp->IsVisible());

		// If we're closing the gallery, flush the thumbnail caches
		if(pDialogBarOp->IsVisible() == FALSE)
		{
			BROADCAST_TO_CLASS(ThumbMessage(ThumbMessage::ThumbState::KILLCACHE, SGLib_Font), DialogOp);

			// Reclaim open library group memory when gallery closed...
			if(FontsSGallery::ThisGallery != NULL)
			{
				FontsSGallery::ThisGallery->GalleryAboutToClose();
			}
		}
	}

	End();
}

/***********************************************************************************************

>	SGLibFontItem::SGLibFontItem(LibraryIndex LibraryIndexToDisplay);

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/3/95

	Inputs:		LibraryIndex - Offset into index file which describes this item
	Outputs:	
	Returns:
	Purpose:	Creates and initialises a new item for the clipart gallery
	Notes:
	SeeAlso:

***********************************************************************************************/

SGLibFontItem::SGLibFontItem(LibraryIndex LibraryIndexToDisplay, BOOL bNew) :
	SGLibDisplayItem(LibraryIndexToDisplay, bNew)
{
}

/* default constructor and destructor */

SGLibFontItem::SGLibFontItem()
{
}

SGLibFontItem::~SGLibFontItem()
{
}

/***********************************************************************************************

>	virtual LibDisplayType SGLibFontItem::GetDisplayType(SGMiscInfo *MiscInfo)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/3/95

	Inputs:		MiscInfo - Contains a few useful bits of info that may be
				needed for all event types.
	Outputs:	
	Returns:	The display mode type to use (position of text, and size of thumb)

	Purpose:	Return the display type to use - clipart gallery override
	Notes:
	SeeAlso:

***********************************************************************************************/

LibDisplayType SGLibFontItem::GetDisplayType(SGMiscInfo *MiscInfo)
{
	// So the same function is used for library fonts and installed fonts...
	return SGDisplayPreviewFonts::GetDisplayType(MiscInfo);
}

/***********************************************************************************************

>	virtual void SGLibFontItem::GetNameText(String_256 *Result)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/3/95

	Inputs:
	Outputs:	Result - String to place resulting text in
	Returns:	The display mode type to use (position of text, and size of thumb)

	Purpose:	Returns the name text for this item, to support simple searching
				operations, and redraw methods for font library items.
	Notes:
	SeeAlso:

***********************************************************************************************/

void SGLibFontItem::GetNameText(String_256 *Result)
{
	GetDisplayedTextDescription(Result);

	// Change lines which end ;BOLDITALIC to ' Bold Italic' (ATM only really)
	FontsSGallery::MakeBoldItalicReadable(Result);
/*	if(Result->Sub((String_8)";") != -1)
	{
		String_256 TmpResult(*Result);
		TmpResult.Left(Result, TmpResult.Sub((String_8)";"));

		if(TmpResult.Sub(String_16(_R(IDS_FONTS_CAPITAL_BOLD))) != -1)
			*Result += String_16(_R(IDS_FONTS_SPACE_BOLD));

		if(TmpResult.Sub(String_16(_R(IDS_FONTS_CAPITAL_ITALIC))) != -1)
			*Result += String_16(_R(IDS_FONTS_SPACE_ITALIC));
	}*/
}

/***********************************************************************************************

>	virtual BOOL SGLibFontItem::GetNameTextPtr(TCHAR **Result)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/5/95

	Inputs:
	Outputs:	Result - points to name in memory if the index file is cached
	Returns:	TRUE if index was cached and name found ok, etc...

	Purpose:	Returns a pointer to the filename for this item... The main use of this
				call is to speed sorting up, since passing pointers about is much quicker
				than constructing strings....
	
	Notes:		BEWARE: If the index file is not in memory this call will probably fail !

***********************************************************************************************/

BOOL SGLibFontItem::GetNameTextPtr(TCHAR **Result)
{
	ERROR3IF(Result == NULL, "SGLibDisplayItem::GetNameText given a NULL param");

	//	If we can get a Library from an item, we're sorted...
	Library *Lib = GetParentLibrary();

	if (Lib != NULL)
		return (Lib->GetTextname(TheLibraryIndex, Result));

	return FALSE;
}



/***********************************************************************************************

>	virtual void SGLibFontItem::GetFullInfoText(String_256 *Result)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/3/95

	Inputs:
	Outputs:	Result - String to place resulting text in
	Returns:	The display mode type to use (position of text, and size of thumb)

	Purpose:	Returns the full-info text for this item, to support simple searching
				operations, and redraw methods for font items.
	Notes:
	SeeAlso:

***********************************************************************************************/

void SGLibFontItem::GetFullInfoText(String_256* Result)
{
	*Result = TEXT("");

	//	If we can get a Library from an item, we're sorted...
	Library* Lib = GetParentLibrary();
														  
	if (Lib != NULL)
	{
		String_256 ItemPath;
		Lib->GetFilename(TheLibraryIndex, &ItemPath, FALSE);
		INT32 FileSize = GetFileSize(NULL);

		String_256 Desc;
		GetDisplayedTextDescription(&Desc);

		// Change lines which end ;BOLDITALIC to ' Bold Italic' (ATM only really)
		FontsSGallery::MakeBoldItalicReadable(&Desc);
		/*if(Desc.Sub((String_8)";") != -1)
		{
			String_256 TmpResult(Desc);
			TmpResult.Left(&Desc, TmpResult.Sub((String_8)";"));

			if(TmpResult.Sub(String_16(_R(IDS_FONTS_CAPITAL_BOLD))) != -1)
				Desc += String_16(_R(IDS_FONTS_SPACE_BOLD));

			if(TmpResult.Sub(String_16(_R(IDS_FONTS_CAPITAL_ITALIC))) != -1)
				Desc += String_16(_R(IDS_FONTS_SPACE_ITALIC));
		} */

		String_32 FileLength;
		BOOL FileSizeOK = FALSE;
		if(FileSize != 0)
		{
			FileSizeOK = Convert::BytesToString(&FileLength, (UINT32)FileSize);
		}

		if(FileSizeOK)
		{
			// "%s, '%s', %s"
			Result->MakeMsg(_R(IDS_SGFONTS_FULLINFO1), (LPCTSTR) Desc, (LPCTSTR) ItemPath, (LPCTSTR) FileLength);
		}
		else
		{
			// "%s, '%s'"
			Result->MakeMsg(_R(IDS_SGFONTS_FULLINFO2), (LPCTSTR) Desc, (LPCTSTR) ItemPath);
		}
	}
}



/***********************************************************************************************

>	UINT32 SGLibFontItem::GetFontID(void)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/3/95

	Inputs:		
	Outputs:
	Returns:	The ID for this font item

	Purpose:	To return the id for this font item
	
***********************************************************************************************/

UINT32 SGLibFontItem::GetFontID(void)
{
	//	If we can get a Library from an item, we're sorted...
	Library *Lib = GetParentLibrary();

	if (Lib != NULL)
		return(Lib->GetID(TheLibraryIndex));

	return 0;
}

/***********************************************************************************************

>	virtual INT32 SGLibFontItem::GetTextWidth(SGFormatInfo *FormatInfo,
												SGMiscInfo *MiscInfo)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/3/95

	Inputs:		FormatInfo - The formatting info from which to calculate my position/size
				MiscInfo - As usual, the useful misc info struct
	Outputs:	Width (in millipoints) required by the descrive text for this mode

	Purpose:	To give the library fonts the same text width as the displayable ones
	
***********************************************************************************************/

INT32 SGLibFontItem::GetTextWidth(SGFormatInfo *FormatInfo, SGMiscInfo *MiscInfo)
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
			XSize = (SGF_SURROUND * OnePixel * 2);
			break;			

		case LibDisplay_SmallThumb:
		case LibDisplay_MediumThumb:
		case LibDisplay_LargeThumb:
			// No extra space for text
			XSize = (SGF_SURROUND * OnePixel * 2);
			break;

		case LibDisplay_SmallThumbText:
		case LibDisplay_MediumThumbText:
		case LibDisplay_LargeThumbText:
			XSize = (INT32)((float) SG_DefaultNameText * 1.1);
			break;

		case LibDisplay_JustText:
			// No description requires no text description space
			XSize = GridLock(MiscInfo, SG_DefaultNameText);
			break;

		case LibDisplay_SingleLineFullInfo:
		default:
			// Text is to the right
			XSize = (INT32)((float) SG_DefaultNameText * 2);
			// XSize = SG_InfiniteWidth; // (XSize * OnePixel) + (SG_DefaultNameText * 3);
			break;
	}

	// Snap to gridpoints
	XSize = GridLock(MiscInfo, XSize);

	return XSize;
}

/***********************************************************************************************

>	virtual BOOL SGLibFontItem::HandleEvent(SGEventType EventType, void *EventInfo,
											 SGMiscInfo *MiscInfo)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/1/95 (base generated in sgbase.cpp)

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

BOOL SGLibFontItem::HandleEvent(SGEventType EventType, void *EventInfo,
								  SGMiscInfo *MiscInfo)
{
	switch (EventType)
	{
		case SGEVENT_MOUSECLICK:
			{
				SGMouseInfo *Mouse = GetMouseInfo(EventType, EventInfo);

				if (FormatRect.ContainsCoord(Mouse->Position))
				{
					if(Mouse->DoubleClick)
						DefaultClickHandler(Mouse, MiscInfo);
					else
					{
						DefaultPreDragHandler(Mouse, MiscInfo);
						GalleryLibFontsDragInfo *DragFont;

						// Grab a copy of the bitmap required for dragging
						KernelBitmap *DispBmp = GetDisplayedKernelBitmap(MiscInfo, TRUE);
						if(DispBmp)
						{
							LibraryGallery::TmpDraggingBitmap = DIBUtil::CopyKernelBitmap(DispBmp, TRUE);
							DragFont = new GalleryLibFontsDragInfo(this, Mouse, MiscInfo,
																Mouse->MenuClick, 0, 0);
						}
						else
						{
		 					// Null rectangle - give a specific size
							LibraryGallery::TmpDraggingBitmap = NULL;

							// Get the relevant size details for the current mode (pixels)
							UINT32 XSize = SG_InfiniteWidth;
							UINT32 YSize = SG_DefaultLargeIcon;
							INT32 DisplayMode = MiscInfo->DisplayMode;
							SGDisplayPreviewFonts::GetThumbnailDetails(DisplayMode, &XSize, &YSize, NULL);

							if(XSize == 0 || YSize == 0)
							{
								XSize = SGF_SMALL_X;
								YSize = SGF_SMALL_Y;
							}

							// Create the drag object...
							DragFont = new GalleryLibFontsDragInfo(this, Mouse, MiscInfo,
																Mouse->MenuClick, XSize, YSize);
						}
						
						if (DragFont != NULL)
							DragManagerOp::StartDrag(DragFont);
					}
					return TRUE;		// Claim this event - nobody else can own this click
				}
			}
			break;

		default:
			return(SGLibDisplayItem::HandleEvent(EventType, EventInfo, MiscInfo));
			break;
	}

	// Default return value: We do not claim this event, so it will be passed on to others
	return FALSE;
}

/***********************************************************************************************

>	virtual void SGLibFontItem::MoveAfter(SGDisplayNode *NodeToMove)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/3/95

	Inputs:		NodeToMove - the node to move

	Purpose:	MOVES the given node (to a different position in the DisplayTree) as the
				previous (left) sibling of this node. If the node is not linked into
				a tree, it is effectively just inserted.

	Notes:		This base class method simply delinks the item and relinks it elsewhere
				in the display tree. However, derived classes will override this method
				so that moving display items can have a further effect of also rearranging
				the displayed "real" items. Before/After moving the real item, the
				derived class can then call this baseclass method to complete the action.
		
				Take care when moving items between groups (e.g. if an item is "moved"
				from one docuemnt to another, it could be a bad thing, so be very
				careful in derived classes to take appropriate action)

				Any attempt to move an item after *itself* is quietly ignored

	Errors:		ERROR3 and quiet exit if NodeToMove == NULL

	SeeAlso:	SuperGallery; SGDisplayColour::InsertAfter; SGDisplayColour::AddItem

***********************************************************************************************/

void SGLibFontItem::MoveAfter(SGDisplayNode *NodeToMove)
{
	SGDisplayNode::MoveAfter(NodeToMove);
}



/***********************************************************************************************

>	virtual void SGLibFontItem::MoveBefore(SGDisplayNode *NodeToMove)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/3/95

	Inputs:		NodeToMove - the node to move

	Purpose:	MOVES the given node (to a different position in the DisplayTree) as the
				previous (left) sibling of this node. If the node is not linked into
				a tree, it is effectively just inserted.

	Notes:		This base class method simply delinks the item and relinks it elsewhere
				in the display tree. However, derived classes will override this method
				so that moving display items can have a further effect of also rearranging
				the displayed "real" items. Before/After moving the real item, the
				derived class can then call this baseclass method to complete the action.
		
				Take care when moving items between groups (e.g. if an item is "moved"
				from one docuemnt to another, it could be a bad thing, so be very
				careful in derived classes to take appropriate action)

				Any attempt to move an item before *itself* is queitly ignored

	Errors:		ERROR3 and quiet exit if NodeToMove == NULL

	SeeAlso:	SuperGallery; SGDisplayColour::InsertBefore; SGDisplayColour::AddItem

***********************************************************************************************/

void SGLibFontItem::MoveBefore(SGDisplayNode *NodeToMove)
{
	SGDisplayNode::MoveBefore(NodeToMove);
}



/********************************************************************************************

>	virtual BOOL SGLibFontItem::GetBubbleHelp(DocCoord *MousePos, String_256 *Result)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/4/95

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

BOOL SGLibFontItem::GetBubbleHelp(DocCoord *MousePos, String_256 *Result)
{
	ERROR3IF(MousePos == NULL || Result == NULL, "Invalid NULL params");

	GetNameText(Result);

	return(TRUE);
}


	
/********************************************************************************************

>	virtual BOOL SGLibFontItem::GetStatusLineHelp(DocCoord *MousePos, String_256 *Result)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/4/95

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

BOOL SGLibFontItem::GetStatusLineHelp(DocCoord *MousePos, String_256 *Result)
{
	ERROR3IF(MousePos == NULL || Result == NULL, "Invalid NULL params");

	String_256 FontName;
	GetNameText(&FontName);

	// "'<FontName>'; Click, then use the Install button to add font; Or drag and drop"
	Result->MakeMsg(_R(IDS_FONTS_DEINSTALLED_STATUS), (TCHAR *)FontName);

	return(TRUE);
}


/***********************************************************************************************

>	virtual void SGLibFontItem::CalculateMyRect(SGFormatInfo *FormatInfo,
												SGMiscInfo *MiscInfo)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/10/95

	Inputs:		FormatInfo - The formatting info from which to calculate my position/size
				MiscInfo - As usual, the useful misc info struct

	Outputs:	member variable FormatRect - is returned filled in with the size/position of
				this LibraryIndex item's display area. This is dependent upon the current display
				mode and format state

				FormatInfo will be updated as a result of the formatting operation

	Purpose:	Slight alteration required to formatrects for the font gallery 'type' icon...

***********************************************************************************************/

void SGLibFontItem::CalculateMyRect(SGFormatInfo *FormatInfo, SGMiscInfo *MiscInfo)
{
	// If we're displaying text underneath the thumbnail, add some space for it
	LibDisplayType DType = GetDisplayType(MiscInfo);
	INT32 OnePixel  = (INT32) DevicePixels(MiscInfo, 1);
	Library *ParentLib = GetParentLibrary();
	
	INT32 XSize = GetTextWidth(FormatInfo, MiscInfo);
	INT32 YSize = 0;

	// Lock the current sizes to the grid
	YSize = GridLock(MiscInfo, YSize);

	// Extra Space required if selected
	YSize += (3 * 2 * OnePixel);

	// Add in space required by the bitmap itself
	switch(DType)
	{
		case LibDisplay_MediumThumb:
			YSize += GridLock(MiscInfo, ParentLib->PreviewBMPHeight(SGThumb_Medium) * OnePixel);
			XSize += GridLock(MiscInfo, (ParentLib->PreviewBMPWidth(SGThumb_Medium) * OnePixel) + (SGF_TYPE_WIDTH * OnePixel));
			break;

		default:
			SGLibDisplayItem::CalculateMyRect(FormatInfo, MiscInfo);
			return;

	}

	// Calculate a proper rectangle for the item
	CalculateFormatRect(FormatInfo, MiscInfo, XSize, YSize);
}


/***********************************************************************************************

>	virtual void SGLibFontItem::HandleRedraw(SGRedrawInfo *RedrawInfo,
												SGMiscInfo *MiscInfo)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/10/95

	Inputs:		RedrawInfo	- The information on the kernel-rendered redraw area
				MiscInfo - always provided. Contains a few useful bits of info that may be
				needed for all event types.

				[Member variable FormatRect should be set up (before calling this method)
				to be the rectangle in which to draw this item]

	Purpose:	Override default redraw so we can pop an icon to the left of the main redraw stuff

***********************************************************************************************/

void SGLibFontItem::HandleRedraw(SGRedrawInfo *RedrawInfo, SGMiscInfo *MiscInfo)
{
	// First, inform the system that we are about to start rendering this item
	StartRendering(RedrawInfo, MiscInfo);

	// Determine the SGSubLib library object to ask for info on this item
	Library *ParentLib = GetParentLibrary();

	// First, inform the system that we are about to start rendering this item
	StartRendering(RedrawInfo, MiscInfo);

	RenderRegion *Renderer = RedrawInfo->Renderer;
	INT32 OnePixel = (INT32) DevicePixels(MiscInfo, 1);
	INT32 TwoPixels = (INT32) DevicePixels(MiscInfo, 2);

	Renderer->SetLineWidth(0);
	Renderer->SetLineColour(RedrawInfo->Transparent);

	// Exclude the 'TTF' / 'ATM' bmp space from the Format Rect, so as not to confuse things
	DocRect NewFormatRect(FormatRect);

	// Draw the icon here...
	NewFormatRect.lox += (SGF_TYPE_WIDTH * OnePixel);
	GridLockRect(MiscInfo, &NewFormatRect);

	DocRect TypeRect(FormatRect);
	TypeRect.hix = NewFormatRect.lox;

	SGDisplayPreviewFonts::DrawTypeIcon(RedrawInfo, MiscInfo, &TypeRect, GetType());

	// Use NewFormatRect from here on...
	DocRect BmpRect(NewFormatRect);
	DocRect UnscaledRect(NewFormatRect);

	LibDisplayType DType = GetDisplayType(MiscInfo);

	if(DType == LibDisplay_JustText)
		BmpRect.hi.x = BmpRect.lo.x + OnePixel;
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

>	virtual INT32 SGLibFontItem::CompareTo(SGDisplayNode *Other, INT32 SortKey)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/12/95

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

INT32 SGLibFontItem::CompareTo(SGDisplayNode *Other, INT32 SortKey)
{
	return(SGLibDisplayItem::CompareTo(Other, SortKey));
}


/********************************************************************************************

>	BOOL SGLibFontItem::GetThumbFileName(PathName *Result)  (WEBSTER)

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/12/96

	Outputs:	Result - will be filled in with the full filename of the object's thumb file
			
	Returns:	FALSE if it failed, else TRUE

	Purpose:	To find the corresponding filename for this object's thumb

********************************************************************************************/


BOOL SGLibFontItem::GetThumbFileName(String_256* path)
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
	String_32 strFilename;
	TCHAR tchType = _T('F');
	TCHAR tchSize;
	LibDisplayType DType;
	SGThumbSize Size = SGThumb_Large;
	SuperGallery* pGallery = GetParentGallery();
	if (!pGallery)
	{
		ERROR3("Ilegal NULL pointer!");
		return FALSE;
	}
	switch (pGallery->GetDisplayMode())
	{
		case 4:
			DType = LibDisplay_JustText;
			break;
		case 3:
			DType = LibDisplay_MediumThumb;
			break;
		case 2:
			DType = LibDisplay_SmallThumbText;
			break;
		case 1:
			DType = LibDisplay_SingleLineFullInfo;
			break;
		case 0:
		default:
			DType = LibDisplay_LargeThumbText;
	}
	switch(DType)
	{
		case LibDisplay_MediumThumbText:
		case LibDisplay_MediumThumb:
		case LibDisplay_SingleLineFullInfo:
			Size = SGThumb_Medium;
			break;
	
		case LibDisplay_SmallThumbText:
			Size = SGThumb_Small;
			break;

		case LibDisplay_JustText:
			return FALSE; // the function should not have been called in this case
	
		default:
			Size = SGThumb_Large;
	}
	switch (Size)
	{
		case SGThumb_Small:
			tchSize = _T('S');
			break;
		case SGThumb_Medium:
			tchSize = _T('M');
			break;
		case SGThumb_Large:
			tchSize = _T('L');
			break;
		default:
			tchSize = _T(' ');
	}
	_sntprintf(strFilename, _MAX_PATH, "%c%05d%c", tchType, pLibrary->GetID(TheLibraryIndex), tchSize); 
	thumbPath += strFilename;
	thumbPath += _T(".png"); // thumbnails for web files will always be PNGs
	*path = thumbPath;
	return TRUE;
}




/***********************************************************************************************

>	FontClass SGLibFontItem::GetType(void)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/10/95

	Purpose:	To return the FontClass type for this item. Can be FC_ATM or FC_TRUETYPE

***********************************************************************************************/

FontClass SGLibFontItem::GetType(void)
{
	Library *Lib = GetParentLibrary();

	if (Lib != NULL)
	{
		TCHAR *pFName;
		BOOL ok = Lib->GetFilename(TheLibraryIndex, &pFName);
		if(ok)
		{
			if(_tcsstr(pFName, ".ttf") != NULL || strstr(pFName, ".TTF") != NULL)
				return FC_TRUETYPE;

			if(_tcsstr(pFName, ".pfb") != NULL || strstr(pFName, ".PFB") != NULL)
				return FC_ATM;
		}
		else
		{
			String_256 FName;
			ok = Lib->GetFilename(TheLibraryIndex, &FName, FALSE);
			if(ok)
			{
				FName.toLower();
				if(FName.Sub((String_8)".ttf") != -1)
					return FC_TRUETYPE;

				if(FName.Sub((String_8)".pfb") != -1)
					return FC_ATM;
			}
		}
	}

	return FC_UNDEFINED;
}


/********************************************************************************************

>	BOOL SGDisplayPreviewFonts::IsFontBeingUsed(void)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/10/95
 	Inputs:
	Outputs:
	Returns:	TRUE if the font is found in any of the currently loaded documents	
	Purpose:	To find out whether we need to warn that this font is being used when deinstalling

********************************************************************************************/

BOOL SGDisplayPreviewFonts::IsFontBeingUsed(void)
{
//	Document *WorkDoc = Document::GetSelected();
	Document *WorkDoc = (Document *)GetApplication()->Documents.GetHead();
	if(WorkDoc == NULL)
		return FALSE;

	if(CachedLogFont == NULL)
		return FALSE;

	String_64 SlowJob(_R(IDS_FONTS_SCANNING_DOCUMENTS));
	BeginSlowJob(-1, TRUE, &SlowJob);

	// Extract info on this item
	String_64 TypeFace(CachedLogFont->lfFaceName);
	TypeFace.toUpper();
	INT32 Style = 0;
	if(CachedLogFont->lfWeight > FW_MEDIUM)
		Style |= 1;
	if(CachedLogFont->lfItalic)
		Style |= 2;

	while(WorkDoc != NULL)
	{
		// build the font list
		FontList DocFonts;
		DocFonts.Build(WorkDoc);

		FontListItem* FontItem = DocFonts.GetFirstItem();
		if(FontItem!=NULL)
		{
			// go through each item checking
			while(FontItem)
			{
				// Check the style first
				// if either of the style bits match, then it's the same style...
				if((FontItem->GetFontStyle() & 3) == Style)
				{
					// get the name
					String_64 Name = FontItem->GetFontName();
					Name.toUpper();
		
					// Crickey... It's the same font !
					if(Name == TypeFace)
					{
						EndSlowJob();
						return TRUE;
					}
				}
		 		FontItem = DocFonts.GetNextItem(FontItem);
			}
		}
		
		// Try the font with the next document
		Document *LastWorkDoc = WorkDoc;
		WorkDoc = (Document *)GetApplication()->Documents.GetNext(LastWorkDoc);
	}

	EndSlowJob();

	// Didn't find font used in any docs
	return FALSE;
}
#endif
