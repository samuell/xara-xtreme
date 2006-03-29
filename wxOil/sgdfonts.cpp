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
// SGDFonts.cpp - Fonts SuperGallery Dragging classes

/* The below code is riddled with DC's and OIL-specifc Font calls... Hence the location */

#include "camtypes.h"
#include "sgdfonts.h"
#include "sgfonts.h"
#include "sgfonts.h"

#ifndef EXCLUDE_GALS

#include "app.h"		// For GetApplication()
//#include "galstr.h"
#include "convert.h"	// For Millipoint convs
#include "fillval.h"
#include "grnddib.h"
#include "bitmap.h"		// For bitmap stuff !
#include "bitmpinf.h"	// For more bitmap stuff
#include "nodebmp.h"	// For bitmap drawing stuff
//#include "galres.h"
#include "sgallery.h"
#include "fixmem.h"
#include "wbitmap.h"
//#include "richard.h"	
#include "sgscan.h"
#include "sgscanf.h"
#include "sglib.h"
#include "thumb.h"
#include "fontpgen.h"	// For Font thumbnail generation
#include "basestr.h"
//#include "resource.h"
#include "sgliboil.h"	// For various stuff
#include "dlgmgr.h"
#include "camelot.h"
#include "keypress.h"
//#include "sgindgen.h"

// For apply
//#include "txtattr.h"
//#include "attrmgr.h"	
//#include "textinfo.h"
#include "nodetext.h"	// For status line stuff

#include "dragmgr.h"
#include "scrcamvw.h"
//#include "viewrc.h"		// FOR _R(IDC_CANDROPONPAGE)
#include "docview.h"
#include "spread.h"

// For drag masks
#include "grndbmp.h"

// For LibraryGallery
#include "sglbase.h"

//#include "richard2.h"
//#include "richard3.h"

// Implement the dynamic class bits...
CC_IMPLEMENT_DYNCREATE(GalleryFontsDragInfo, BitmapDragInformation)
CC_IMPLEMENT_DYNCREATE(GalleryLibFontsDragInfo, BitmapDragInformation)
CC_IMPLEMENT_DYNAMIC(SGFontsDragTarget, SGListDragTarget)

// This line mustn't go before any CC_IMPLEMENT_... macros
#define new CAM_DEBUG_NEW


/********************************************************************************************

>	SGFontsDragTarget::SGFontsDragTarget(DialogOp *TheDialog, CGadgetID TheGadget = NULL)
	 
	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/3/95
	Inputs:		TheDialog - The kernel dialog in which the target exists
				TheGadget - The gadget within that dialogue which is the target

	Purpose:	Constructor

********************************************************************************************/

SGFontsDragTarget::SGFontsDragTarget(DialogOp *TheDialog, CGadgetID TheGadget)
					: SGListDragTarget(TheDialog, TheGadget)
{
	ERROR3IF(!TheDialog->IsKindOf(CC_RUNTIME_CLASS(FontsSGallery)),
			"You can only use SGFontsDragTargets with LibFontsSGallery dialogues!");
}



/********************************************************************************************

	BOOL SGFontsDragTarget::ProcessEvent(DragEventType Event, DragInformation *pDragInfo,
											OilCoord *pMousePos, KeyPress* pKeyPress)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/3/95
	Inputs:		Event - Indicates what has happened
				pDragInfo - points to drag information describing this drag. This
				should be a FontsDragInformation or derivation thereof
				pMousePos - points to information on the current mouse position, in OIL coords
				pKeyPress - NULL, or if for a keypress event, keypress information

	Returns:	TRUE to claim the event, FALSE to let it through to other targets

	Purpose:	Event Handler for SuperGallery listitem drag events. Overrides the
				base class handler to enable it to sort out the node being dragged
				for Fonts drags.

********************************************************************************************/

BOOL SGFontsDragTarget::ProcessEvent(DragEventType Event, DragInformation *pDragInfo,
										OilCoord *pMousePos, KeyPress* pKeyPress)
{
	SGDisplayNode *DraggedNode = NULL;
	BOOL IsSimpleBitmapDrag = TRUE;

	if (IS_A(pDragInfo, GalleryFontsDragInfo))
	{
		// We must be dragging an Installed Font
		DraggedNode = ((GalleryFontsDragInfo *)pDragInfo)->GetDraggedFont();
		TRACEUSER( "Richard", _T("We're Dragging an installed font\n"));
		IsSimpleBitmapDrag = FALSE;
	}
	else if (IS_A(pDragInfo, GalleryLibFontsDragInfo))
	{
		// We must be dragging an Un-installed Font
		DraggedNode = ((GalleryLibFontsDragInfo *)pDragInfo)->GetDraggedFont();
		TRACEUSER( "Richard", _T("We're Dragging a library font\n"));
		IsSimpleBitmapDrag = FALSE;
	}
	else
		return(FALSE);		// We only accept font drags

	if (DraggedNode != NULL)
	{
		switch(Event)
		{
			case DRAGEVENT_COMPLETED:
				HandleDragCompleted((SuperGallery *) TargetDialog,
									DraggedNode, pMousePos, IsSimpleBitmapDrag);
				return(TRUE);


			case DRAGEVENT_MOUSESTOPPED:
			case DRAGEVENT_MOUSEMOVED:
			case DRAGEVENT_MOUSEIDLE:
				{
					// Specialised cursor changing code

					SuperGallery *ParentGallery = (SuperGallery *) TargetDialog;
					if (ParentGallery != NULL && pMousePos != NULL && DraggedNode != NULL)
					{
						// Convert the OilCoords into DocCoords
						DocCoord MousePos(pMousePos->x, pMousePos->y);
						SGDisplayNode *DestNode = ParentGallery->FindNodeUnderPointer(&MousePos);

						if(DestNode != NULL)
						{
							SGDisplayNode *DestGroup = DestNode->GetParent();
							SGDisplayNode *SourceGroup = DraggedNode->GetParent();

							if(DestGroup != NULL && SourceGroup != NULL)
							{	
								// We're dragging within a group - that's ok
								if(SourceGroup == DestGroup)
									return(DetermineCursorShape(ParentGallery, DraggedNode, pMousePos));

								// We're dragging a library font onto the installed section - that's ok
								if(SourceGroup->IS_KIND_OF(SGLibGroup) && DestGroup->IS_KIND_OF(SGFontsGroup))
									return(DetermineCursorShape(ParentGallery, DraggedNode, pMousePos));
							}
						}		
					}
				}

				// If we can't drag a font item into this group, set the default cursor...
				CurrentCursorID = _R(IDC_DRAGGING_COLOUR);		// No-can-drop cursor shape
				return TRUE;
		}
	}

	// Otherwise, we aren't interested in the event, so we don't claim it
	return(FALSE);
}

/********************************************************************************************

>	void GalleryFontsDragInfo::GalleryFontsDragInfo() 
	 
	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/3/95		  

	Purpose:	Default constructor - do not call this constructor

********************************************************************************************/

GalleryFontsDragInfo::GalleryFontsDragInfo()
{
	ERROR3("Default GalleryFontsDragInfo constructor called");	
}

/********************************************************************************************

>	void GalleryFontsDragInfo::~GalleryFontsDragInfo() 
	 
	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/4/95		  

	Purpose:	Destructor - reclaim temporary bitmap memory

********************************************************************************************/

GalleryFontsDragInfo::~GalleryFontsDragInfo()
{
	if(LibraryGallery::TmpDraggingBitmap != NULL)
	{
		delete LibraryGallery::TmpDraggingBitmap;
		LibraryGallery::TmpDraggingBitmap = NULL;
	}
}



/********************************************************************************************

>	GalleryFontsDragInfo::GalleryFontsDragInfo(SGDisplayPreviewFonts *pSourceItem,
											SGMouseInfo *pMouseInfo, SGMiscInfo *pMiscInfo,
 											BOOL IsAdjust = FALSE,  INT32 XSize, INT32 YSize)
	 
	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/3/95		  

	Inputs:		pSourceItem - The gallery item from which the drag originated
				pMouseInfo	- The mouse info which made the item start the drag
				pMiscInfo	- The MiscInfo which accompanied the mouse event
				IsAdjust	- TRUE if this is an adjust (line-Bitmap) drag
				XSize 		- Width in pixels of bmp to drag
				YSize		- Height in pixels of bmp to drag

	Purpose:	Constructor

********************************************************************************************/

GalleryFontsDragInfo::GalleryFontsDragInfo(SGDisplayPreviewFonts *pSourceItem,
											SGMouseInfo *pMouseInfo, SGMiscInfo *pMiscInfo,
 											BOOL IsAdjust, INT32 XSize, INT32 YSize)
					  : BitmapDragInformation(LibraryGallery::TmpDraggingBitmap, XSize, YSize,
	  						FontsSGallery::GetDragFontOffset(LibraryGallery::TmpDraggingBitmap), 0,
  							IsAdjust)
{
	SourceItem	= pSourceItem;	// Copy the source item pointer

	MouseInfo	= *pMouseInfo;	// Duplicate the structures (they may cease to exist
	MiscInfo	= *pMiscInfo;	// soon after the drag is started)
}



/********************************************************************************************

>	void GalleryFontsDragInfo::OnClick(INT32 Flags,POINT Point) 
	 
	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/3/95		  
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	This is called if a drag was attempted but never started because it was a 
				click all along. It calls back the SourceItem SGDisplayBitmap, to get it
				to handle the click.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void GalleryFontsDragInfo::OnClick(INT32 Flags ,POINT Point)
{
	if (SourceItem != NULL)
		SourceItem->DragWasReallyAClick(&MouseInfo, &MiscInfo);
}

/********************************************************************************************

>	void GalleryFontsDragInfo::GetCursorID(DragTarget* pDragTarget)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/3/95
	Purpose:	Set cursor over this target


********************************************************************************************/

UINT32 GalleryFontsDragInfo::GetCursorID(DragTarget* pDragTarget)
{
	if (pDragTarget && pDragTarget->IS_KIND_OF(ViewDragTarget))
	{
		PageDropInfo PageDropInfo;
		((ViewDragTarget*)pDragTarget)->GetDropInfo(&PageDropInfo);

		NodeRenderableInk* pObjectHit 	= PageDropInfo.pObjectHit;
		ObjectDragTarget TargetHit 		= PageDropInfo.TargetHit;

		// if (pObjectHit && pObjectHit->IS_KIND_OF(TextChar))
		if (pObjectHit && pObjectHit->RequiresAttrib(CC_RUNTIME_CLASS(AttrTxtFontTypeface)))
			TargetHit = FILL_TARGET;
		else
			TargetHit = NO_TARGET;

		switch (TargetHit)
		{
			case FILL_TARGET:
				return _R(IDC_CANDROPONFILL);

			case NO_TARGET:
				return _R(IDC_CANDROPONPAGE);
		};

		return _R(IDC_CANDROPONPAGE);
	}

	return _R(IDC_CANTDROP);
}

/********************************************************************************************

>	virtual BOOL GalleryFontsDragInfo::GetStatusLineText(String_256 * TheText, DragTarget* pDragTarget)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/3/95
	Returns:	Whether String is valid
	Purpose:	provide status line text for this target
   
********************************************************************************************/

BOOL GalleryFontsDragInfo::GetStatusLineText(String_256 * TheText, DragTarget* pDragTarget)
{
#ifdef STANDALONE
	return FALSE;
#else
	ERROR2IF(TheText==NULL,FALSE,"NULL string in GetStatusLineText()");

	if (TheBitmap == NULL || TheBitmap->ActualBitmap == NULL)
		return FALSE;

	// "Dragging font '#1%s'"
 	String_256 DragString;
	String_256 ItemName;
	SourceItem->GetNameText(&ItemName);
//	DragString += *SourceItem->GetDisplayedTextDescription();

 	DragString.MakeMsg(_R(IDS_FONTS_DRAGGING), (TCHAR *)ItemName);


	if (pDragTarget && pDragTarget->IS_KIND_OF(ViewDragTarget))
	{
		DragString += String_8(_R(IDS_SGDFONTS_STAT_COLON_SEP)); //" : ";
	
		PageDropInfo PageDropInfo;
		((ViewDragTarget*)pDragTarget)->GetDropInfo(&PageDropInfo);

		NodeRenderableInk* pObjectHit 	= PageDropInfo.pObjectHit;
		ObjectDragTarget TargetHit 		= PageDropInfo.TargetHit;

		// if (pObjectHit && pObjectHit->IS_KIND_OF(TextChar))
		if (pObjectHit && pObjectHit->RequiresAttrib(CC_RUNTIME_CLASS(AttrTxtFontTypeface)))
			TargetHit = FILL_TARGET;
		else
			TargetHit = NO_TARGET;

		switch (TargetHit)
		{
			case FILL_TARGET:
				if (IS_A(pObjectHit,CaretNode))
				{
					// + "Drop to apply this font to the caret";
					String_64 DropToCaret(_R(IDS_FONTS_DROP_TO_APPLY_CARET));
					DragString += DropToCaret;
				}
				else
				{
					if(pObjectHit->IsSelected())
					{
						// + "Drop to apply this font to the text selection";
						String_64 DropToText(_R(IDS_FONTS_DROP_TO_APPLY_SEL));
						DragString += DropToText;
					}
					else
					if (KeyPress::IsConstrainPressed())
					{
						// + "Drop to apply this font to this character";
						String_64 DropToText(_R(IDS_FONTS_DROP_TO_APPLY_CHAR));
						DragString += DropToText;
					}
					else
					{
						// + "Drop to apply this font to the text story";
						String_64 DropToText(_R(IDS_FONTS_DROP_TO_APPLY_TEXT));
						DragString += DropToText;
					}
				}
				break;

			case NO_TARGET:
				{
					// + "Drop to set the current font style";
					String_64 DropToCurrent(_R(IDS_FONTS_DROP_TO_APPLY_CURRENT));
					DragString += DropToCurrent;
				}
				break;
		};

	 	*TheText = DragString;
		return TRUE;
	}

 	*TheText = DragString;
	return TRUE;
#endif
}

/********************************************************************************************

>	BOOL GalleryFontsDragInfo::OnPageDrop(ViewDragTarget* pDragTarget)
 
	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/3/95		  
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Called when a Fill is dropped onto the page.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL GalleryFontsDragInfo::OnPageDrop(ViewDragTarget* pDragTarget)
{
#ifdef STANDALONE
	return FALSE;
#else
	PageDropInfo PageDropInfo;
	((ViewDragTarget*)pDragTarget)->GetDropInfo(&PageDropInfo);
	NodeRenderableInk* pObjectHit = PageDropInfo.pObjectHit;
	SGDisplayPreviewFonts* SelectedNode = SourceItem;

	return(FontsSGallery::ApplyFont(TRUE, (SGDisplayNode *)SourceItem, pObjectHit));
#endif
}



/********************************************************************************************

>	INT32 GalleryFontsDragInfo::GetDragTransparency()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/4/95		
	Returns:	The drag tranparency to use for a Font Drag.
	Purpose:	Determines how transparent the dragged font will be.

********************************************************************************************/

INT32 GalleryFontsDragInfo::GetDragTransparency()
{
	// If there's no preview, it looks best if the rectangle is transparent
	if(TheBitmap == NULL)
		return 50;

	// Font drags look better as solid masked bmps
	return 0;
}

/********************************************************************************************

>	KernelBitmap* GalleryFontsDragInfo::GetSolidDragMask()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/4/95		
	Returns:	A pointer to the 1bpp Mask bitmap.
	Purpose:	Makes a 1bpp KernelBitmap mask for the Font preview solid drag.

				This version makes use of the fact that it knows the Font Bitmaps are
				8bpp, and so it can alter the palette (temporarily) to make the
				Anti-Aliased pixels render as solid black.

	SeeAlso:	-

********************************************************************************************/

KernelBitmap* GalleryFontsDragInfo::GetSolidDragMask()
{
	if(TheBitmap == NULL)
	{
		DragMask = NULL;
		return NULL;
	}

	if (DragMask == NULL)
	{
		// First set up a 1bpp Gavin Bitmap to render into
		Matrix ConvertMatrix;
		FIXED16 ViewScale = 1;

		CDC* pDisplayDC = new CDC();
		if (pDisplayDC == NULL)
			return NULL;

		pDisplayDC->CreateDC("DISPLAY", NULL, NULL, NULL);

		double dpi   = (double) GetDeviceCaps( pDisplayDC->m_hDC, LOGPIXELSX );

		// Make life easy by making the Redraw Rectangle start at 0,0
		INT32 PixelSize = (INT32)(72000.0/dpi); //.MakeLong();
		DocRect ClipRegion(0,0, SolidDragSize.cx*PixelSize, SolidDragSize.cy*PixelSize);

		GRenderBitmap* pMaskRegion 	= new GRenderBitmap(ClipRegion, ConvertMatrix, ViewScale,
														1, dpi);
		if (pMaskRegion == NULL)
		{
			delete pDisplayDC;
			return NULL;
		}

		// Make a Dummy view and spread to attach, just to keep it happy
		DocView *View = DocView::GetCurrent();
		if (View == NULL)
		{
			delete pMaskRegion;
			delete pDisplayDC;
			return NULL;	// Must have a DocView
		}

		Spread *pSpread = View->FindEnclosingSpread(OilCoord(0,0));
		if (pSpread == NULL)
		{
			delete pMaskRegion;
			delete pDisplayDC;
			return NULL;	// Must have a Spread
		}

		pMaskRegion->AttachDevice(View, pDisplayDC, pSpread);

// ****** Start of Bodge *******

// To make the font mask look correct, we'll make use of the fact
// that we know it's an 8bpp grey scale bitmap.
// First we'll copy the existing palette, then temporarily set the
// palette so all the colours are Black.
// This should mean that everything except the White background of the
// font bitmap will be rendered as Black.

// This hopefully won't be needed when 'Proper' mask render regions are done

		WinBitmap* WinBM = (WinBitmap*)TheBitmap->ActualBitmap;

		RGBQUAD* Palette = WinBM->BMInfo->bmiColors;
		RGBQUAD OldPalette[256];
		
		// Make a copy of the existing palette
		for (INT32 i=0; i<256; i++)
		{
			OldPalette[i] = Palette[i];
		}

		// Now set all but the last few palette entries to 0
		// (The font bitmaps seems to use 253 for the background colour)
		for (i=0; i<256; i++)
		{
			Palette[i].rgbBlue  = ((i < 253) ? 0 : 255);
			Palette[i].rgbGreen = ((i < 253) ? 0 : 255);
			Palette[i].rgbRed   = ((i < 253) ? 0 : 255);
		}

// ****** End of Bodge *******

		// Render the bitmap into the MaskRegion to create the 1bpp mask
		pMaskRegion->StartRender();
		pMaskRegion->DrawBitmap(DocCoord(0,0), TheBitmap);
		pMaskRegion->StopRender();

		// Restore the Proper Palatte values, so the bitmap can be
		// rendered properly
		for (i=0; i<256; i++)
		{
			Palette[i] = OldPalette[i];
		}

		// Get the 1bpp Mask bitmap out of the Render Region
		OILBitmap* pOilMaskBmp = pMaskRegion->ExtractBitmap();
		DragMask = new KernelBitmap(pOilMaskBmp, TRUE);	

		// Tidy up
		delete pMaskRegion;
		delete pDisplayDC;
	}

	return DragMask;
}






/********************************************************************************************

>	void GalleryLibFontsDragInfo::GalleryLibFontsDragInfo() 
	 
	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/3/95		  

	Purpose:	Default constructor - do not call this constructor

********************************************************************************************/

GalleryLibFontsDragInfo::GalleryLibFontsDragInfo()
{
	ERROR3("Default GalleryLibFontsDragInfo constructor called");	
}

/********************************************************************************************

>	void GalleryLibFontsDragInfo::~GalleryLibFontsDragInfo() 
	 
	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/4/95		  

	Purpose:	Destructor - reclaim temporary bitmap memory

********************************************************************************************/

GalleryLibFontsDragInfo::~GalleryLibFontsDragInfo()
{
	if(LibraryGallery::TmpDraggingBitmap != NULL)
	{
		delete LibraryGallery::TmpDraggingBitmap;
		LibraryGallery::TmpDraggingBitmap = NULL;
	}
}



/********************************************************************************************

>	GalleryLibFontsDragInfo::GalleryLibFontsDragInfo(SGLibFontItem *pSourceItem,
											SGMouseInfo *pMouseInfo, SGMiscInfo *pMiscInfo,
 											BOOL IsAdjust = FALSE, INT32 XSize = 0, INT32 YSize = 0)
	 
	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/3/95		  

	Inputs:		pSourceItem - The gallery item from which the drag originated
				pMouseInfo	- The mouse info which made the item start the drag
				pMiscInfo	- The MiscInfo which accompanied the mouse event
				IsAdjust	- TRUE if this is an adjust (line-Bitmap) drag
				XSize 		- Width of dragging bmp
				YSize		- Height of dragging bmp

	Purpose:	Constructor

********************************************************************************************/

GalleryLibFontsDragInfo::GalleryLibFontsDragInfo(SGLibFontItem *pSourceItem,
											SGMouseInfo *pMouseInfo, SGMiscInfo *pMiscInfo,
 											BOOL IsAdjust, INT32 XSize, INT32 YSize)
					  : BitmapDragInformation(LibraryGallery::TmpDraggingBitmap, XSize, YSize,
  							FontsSGallery::GetDragFontOffset(LibraryGallery::TmpDraggingBitmap), 0,
  							IsAdjust)
{
	SourceItem	= pSourceItem;	// Copy the source item pointer

	MouseInfo	= *pMouseInfo;	// Duplicate the structures (they may cease to exist
	MiscInfo	= *pMiscInfo;	// soon after the drag is started)
}

/********************************************************************************************

>	void GalleryLibFontsDragInfo::OnClick(INT32 Flags,POINT Point) 
	 
	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/3/95		  
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	This is called if a drag was attempted but never started because it was a 
				click all along. It calls back the SourceItem SGDisplayBitmap, to get it
				to handle the click.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void GalleryLibFontsDragInfo::OnClick(INT32 Flags ,POINT Point)
{
	if (SourceItem != NULL)
		SourceItem->DragWasReallyAClick(&MouseInfo, &MiscInfo);
}

/********************************************************************************************

>	void GalleryLibFontsDragInfo::GetCursorID(DragTarget* pDragTarget)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/3/95
	Purpose:	Set cursor over this target


********************************************************************************************/

UINT32 GalleryLibFontsDragInfo::GetCursorID(DragTarget* pDragTarget)
{
	if (pDragTarget && pDragTarget->IS_KIND_OF(ViewDragTarget))
	{
		PageDropInfo PageDropInfo;
		((ViewDragTarget*)pDragTarget)->GetDropInfo(&PageDropInfo);

		NodeRenderableInk* pObjectHit 	= PageDropInfo.pObjectHit;
		ObjectDragTarget TargetHit 		= PageDropInfo.TargetHit;
		
		String_256 Desc256;
		SourceItem->GetNameText(&Desc256);
		if (pObjectHit && !FontsSGallery::IsFontAlreadyInstalled(&Desc256, SourceItem->GetType()) && SourceItem->GetParentLibrary() && SourceItem->GetParentLibrary()->IsWebLibrary())
			return _R(IDC_CANTDROP);


		// if (pObjectHit && pObjectHit->IS_KIND_OF(TextChar))
		if (pObjectHit && pObjectHit->RequiresAttrib(CC_RUNTIME_CLASS(AttrTxtFontTypeface)))
			TargetHit = FILL_TARGET;
		else
			TargetHit = NO_TARGET;

		switch (TargetHit)
		{
			case FILL_TARGET:
				return _R(IDC_CANDROPONFILL);

			case NO_TARGET:
				return _R(IDC_CANDROPONPAGE);
		};

		return _R(IDC_CANDROPONPAGE);
	}

	return _R(IDC_CANTDROP);
}

/********************************************************************************************

>	virtual BOOL GalleryLibFontsDragInfo::GetStatusLineText(String_256 * TheText, DragTarget* pDragTarget)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/3/95
	Returns:	Whether String is valid
	Purpose:	provide status line text for this target
   
********************************************************************************************/

BOOL GalleryLibFontsDragInfo::GetStatusLineText(String_256 * TheText, DragTarget* pDragTarget)
{
#ifdef STANDALONE
	return FALSE;
#else
	ERROR2IF(TheText==NULL,FALSE,"NULL string in GetStatusLineText()");

	if (TheBitmap == NULL || TheBitmap->ActualBitmap == NULL)
		return FALSE;

 	String_256 DragString;
	String_256 ItemName;
	SourceItem->GetNameText(&ItemName);
	NodeRenderableInk* pObjectHit = NULL;
	PageDropInfo PageDropInfo;
	ObjectDragTarget TargetHit;
	if (pDragTarget && pDragTarget->IS_KIND_OF(ViewDragTarget))
	{
		((ViewDragTarget*)pDragTarget)->GetDropInfo(&PageDropInfo);
		pObjectHit 	= PageDropInfo.pObjectHit;
		TargetHit 		= PageDropInfo.TargetHit;
	}
	if (pObjectHit && !FontsSGallery::IsFontAlreadyInstalled(&ItemName, SourceItem->GetType()) && SourceItem->GetParentLibrary() && SourceItem->GetParentLibrary()->IsWebLibrary())
		DragString = String_256(_R(IDS_MUST_DOWNLOAD));
	else
	{
		// "Dragging font '#1%s'"
 		DragString.MakeMsg(_R(IDS_FONTS_DRAGGING), (TCHAR *)ItemName);

		if (pDragTarget && pDragTarget->IS_KIND_OF(SGFontsDragTarget))
		{
			DragString += String_8(_R(IDS_SGDFONTS_STAT_COLON_SEP)); //" : ";
		
			// + "Drop to Install this font"
			String_64 DropToInstall(_R(IDS_FONTS_DROP_TO_INSTALL));
			DragString += DropToInstall;

	 		*TheText = DragString;
			return TRUE;
		}

		if (pDragTarget && pDragTarget->IS_KIND_OF(ViewDragTarget))
		{
			DragString += String_8(_R(IDS_SGDFONTS_STAT_COLON_SEP)); //" : ";
		

			if (pObjectHit && pObjectHit->RequiresAttrib(CC_RUNTIME_CLASS(AttrTxtFontTypeface)))
				TargetHit = FILL_TARGET;
			else
				TargetHit = NO_TARGET;

			switch (TargetHit)
			{
				case FILL_TARGET:

					if (IS_A(pObjectHit,CaretNode))
					{
						// + "Drop to apply this font to the caret";
						String_64 DropToCaret(_R(IDS_FONTS_DROP_TO_APPLY_CARET));
						DragString += DropToCaret;
					}
					else
					{
						if(pObjectHit->IsSelected())
						{
							// + "Drop to apply this font to the text selection";
							String_64 DropToText(_R(IDS_FONTS_DROP_TO_APPLY_SEL));
							DragString += DropToText;
						}
						else
						if (KeyPress::IsConstrainPressed())
						{
							// + "Drop to apply this font to this character";
							String_64 DropToText(_R(IDS_FONTS_DROP_TO_APPLY_CHAR));
							DragString += DropToText;
						}
						else
						{
							// + "Drop to apply this font to the text story";
							String_64 DropToText(_R(IDS_FONTS_DROP_TO_APPLY_TEXT));
							DragString += DropToText;
						}
					}
					break;

				case NO_TARGET:
					{
						// + "Drop to set the current font style";
						String_64 DropToCurrent(_R(IDS_FONTS_DROP_TO_APPLY_CURRENT));
						DragString += DropToCurrent;
					}
					break;
			};

	 		*TheText = DragString;
			return TRUE;
		}
	}

 	*TheText = DragString;
	return TRUE;
#endif
}


/********************************************************************************************

>	BOOL GalleryLibFontsDragInfo::OnPageDrop(ViewDragTarget* pDragTarget)
 
	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/4/95		  
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Called when a library font is dropped onto the page.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL GalleryLibFontsDragInfo::OnPageDrop(ViewDragTarget* pDragTarget)
{
#ifdef STANDALONE
	return FALSE;
#else
	PageDropInfo PageDropInfo;
	((ViewDragTarget*)pDragTarget)->GetDropInfo(&PageDropInfo);
	NodeRenderableInk* pObjectHit = PageDropInfo.pObjectHit;
	SGLibFontItem *FontItem = SourceItem;

	// Install the font then apply it
	return(FontsSGallery::ApplyFont(TRUE, (SGDisplayNode *)FontItem, pObjectHit));
#endif
}



/********************************************************************************************

>	INT32 GalleryLibFontsDragInfo::GetDragTransparency()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/4/95		
	Returns:	The drag tranparency to use for a Font Drag.
	Purpose:	Determines how transparent the dragged font will be.

********************************************************************************************/

INT32 GalleryLibFontsDragInfo::GetDragTransparency()
{
	// If there's no preview, it looks best if the rectangle is transparent
	if(TheBitmap == NULL)
		return 50;

	// Font drags look better as solid masked bmps
	return 0;
}

/********************************************************************************************

>	KernelBitmap* GalleryLibFontsDragInfo::GetSolidDragMask()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/4/95		
	Returns:	A pointer to the 1bpp Mask bitmap.
	Purpose:	Makes a 1bpp KernelBitmap mask for the Font preview solid drag.

				This version makes use of the fact that it knows the Font Bitmaps are
				8bpp, and so it can alter the palette (temporarily) to make the
				Anti-Aliased pixels render as solid black.

	SeeAlso:	-

********************************************************************************************/

KernelBitmap* GalleryLibFontsDragInfo::GetSolidDragMask()
{
	if(TheBitmap == NULL)
	{
		DragMask = NULL;
		return NULL;
	}

	if (DragMask == NULL)
	{
		// First set up a 1bpp Gavin Bitmap to render into
		Matrix ConvertMatrix;
		FIXED16 ViewScale = 1;

		CDC* pDisplayDC = new CDC();
		if (pDisplayDC == NULL)
			return NULL;

		pDisplayDC->CreateDC("DISPLAY", NULL, NULL, NULL);

		INT32 dpi   = GetDeviceCaps( pDisplayDC->m_hDC, LOGPIXELSX );

		// Make life easy by making the Redraw Rectangle start at 0,0
		INT32 PixelSize = 72000/dpi;
		DocRect ClipRegion(0,0, SolidDragSize.cx*PixelSize, SolidDragSize.cy*PixelSize);

		GRenderBitmap* pMaskRegion 	= new GRenderBitmap(ClipRegion, ConvertMatrix, ViewScale,
														1, dpi);
		if (pMaskRegion == NULL)
		{
			delete pDisplayDC;
			return NULL;
		}

		// Make a Dummy view and spread to attach, just to keep it happy
		DocView *View = DocView::GetCurrent();
		if (View == NULL)
		{
			delete pMaskRegion;
			delete pDisplayDC;
			return NULL;	// Must have a DocView
		}

		Spread *pSpread = View->FindEnclosingSpread(OilCoord(0,0));
		if (pSpread == NULL)
		{
			delete pMaskRegion;
			delete pDisplayDC;
			return NULL;	// Must have a Spread
		}

		pMaskRegion->AttachDevice(View, pDisplayDC, pSpread);

// ****** Start of Bodge *******

// To make the font mask look correct, we'll make use of the fact
// that we know it's an 8bpp grey scale bitmap.
// First we'll copy the existing palette, then temporarily set the
// palette so all the colours are Black.
// This should mean that everything except the White background of the
// font bitmap will be rendered as Black.

// This hopefully won't be needed when 'Proper' mask render regions are done
		WinBitmap* WinBM = (WinBitmap*)TheBitmap->ActualBitmap;

		RGBQUAD* Palette = WinBM->BMInfo->bmiColors;
		RGBQUAD OldPalette[256];
		
		// Make a copy of the existing palette
		for (INT32 i=0; i<256; i++)
		{
			OldPalette[i] = Palette[i];
		}

		// Now set all but the last few palette entries to 0
		// (The font bitmaps seems to use 253 for the background colour)
		for (i=0; i<256; i++)
		{
			Palette[i].rgbBlue  = ((i < 253) ? 0 : 255);
			Palette[i].rgbGreen = ((i < 253) ? 0 : 255);
			Palette[i].rgbRed   = ((i < 253) ? 0 : 255);
		}

// ****** End of Bodge *******

		// Render the bitmap into the MaskRegion to create the 1bpp mask
		pMaskRegion->StartRender();
		pMaskRegion->DrawBitmap(DocCoord(0,0), TheBitmap);
		pMaskRegion->StopRender();

		// Restore the Proper Palatte values, so the bitmap can be
		// rendered properly
		for (i=0; i<256; i++)
		{
			Palette[i] = OldPalette[i];
		}

		// Get the 1bpp Mask bitmap out of the Render Region
		OILBitmap* pOilMaskBmp = pMaskRegion->ExtractBitmap();
		DragMask = new KernelBitmap(pOilMaskBmp, TRUE);	

		// Tidy up
		delete pMaskRegion;
		delete pDisplayDC;
	}

	return DragMask;
}
#endif
