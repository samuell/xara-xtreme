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
// SGBitmap.cpp - Bitmap SuperGallery classes - BitmapSGallery and SGDisplayBitmap

#include "camtypes.h"

#include "app.h"		// For GetApplication()
//nclude "gallery.h"	// For _R(IDC_GALLERY_LISTBOX)
//#include "galstr.h"
//#include "galres.h"
#include "sgbitmap.h"	
#include "sginit.h"	
#include "sgmenu.h"

//nclude "ccdc.h"		// For render-into-dialogue support
//nclude "fillval.h"
//nclude "grnddib.h"
//#include "will.h"		// For OpShowBitmapGallery ID's
//nclude "bitmap.h"	
#include "bitmpinf.h"	
#include "fillattr.h"	
//nclude "attrmgr.h"	
#include "nodebmp.h"	
//nclude "convert.h"	
//#include "resource.h"	// For _R(IDS_CANCEL)

#include "dragmgr.h"
//nclude "dragbmp.h"
//nclude "viewrc.h"		// FOR _R(IDC_CANDROPONPAGE)

#include "docview.h"
#include "spread.h"
#include "tracedlg.h"
#include "bfxdlg.h"
//nclude "bfxalu.h"
//nclude "bitmapfx.h"
#include "impexpop.h"	// Import/Export ops
//#include "scrcamvw.h"
#include "fixmem.h"

//nclude "sglib.h"

//#include "richard2.h"	// extra resource strings
#include "bmpcomp.h"	
//#include "will3.h"	
//#include "phil.h"

#include "comattrmsg.h"
#include "bmpsdlg.h"
//#include "bmpdlgr.h"
//#include "bmpsdlgr.h"
#include "giffiltr.h"
#include "bmpsrc.h"

#include "plugop.h"		// PlugInsContextMenu
#include "backgrnd.h"	// OpBackground
#include "keypress.h"	// KeyPress
#include "impexpop.h"	// BitmapExportParam
//#include "prevwdlg.h"	// PreviewDialog::Init()
#include "bmpexprw.h"	// BitmapExportPreviewDialog::Init()

#include "helpuser.h"		//For the help button
//#include "xshelpid.h"		//For the help button
//#include "helppath.h"
//#include "xpehost.h"		// For OPTOKEN_XPE_EDIT
#include "qualattr.h"
#include "bfxop.h"			// useful BFX plug-in related operations

#ifdef PHOTOSHOPPLUGINS
#include "plugmngr.h"	// CheckHaveDetailsOnPlugIns
#endif //PHOTOSHOPPLUGINS

// Implement the dynamic class bits...
CC_IMPLEMENT_DYNCREATE(BitmapSGallery, SuperGallery)
CC_IMPLEMENT_DYNAMIC(SGDisplayKernelBitmap, SGDisplayItem)
CC_IMPLEMENT_DYNCREATE(OpDisplayBitmapGallery,Operation);
CC_IMPLEMENT_DYNAMIC(SGBitmapDragTarget, SGListDragTarget);

PORTNOTE("other", "Disabled GalleryBitmapDragInfo")
#ifndef EXCLUDE_FROM_XARALX
CC_IMPLEMENT_DYNCREATE(GalleryBitmapDragInfo, BitmapDragInformation)
#endif

// Enable Background redraw in the bitmap gallery...
//#define SGBITMAP_BACKGROUND_REDRAW

// This line mustn't go before any CC_IMPLEMENT_... macros
#define new CAM_DEBUG_NEW


BOOL BitmapSGallery::IgnoreBmpListChange = FALSE;


/********************************************************************************************

>	SGBitmapDragTarget::SGBitmapDragTarget(DialogOp *TheDialog, CGadgetID TheGadget = NULL)
	 
	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/3/95
	Inputs:		TheDialog - The kernel dialog in which the target exists
				TheGadget - The gadget within that dialogue which is the target

	Purpose:	Constructor

********************************************************************************************/

SGBitmapDragTarget::SGBitmapDragTarget(DialogOp *TheDialog, CGadgetID TheGadget)
					: SGListDragTarget(TheDialog, TheGadget)
{
	ERROR3IF(!TheDialog->IsKindOf(CC_RUNTIME_CLASS(BitmapSGallery)),
			"You can only use SGBitmapDragTargets with BitmapSGallery dialogues!");
}



/********************************************************************************************

	BOOL SGBitmapDragTarget::ProcessEvent(DragEventType Event, DragInformation *pDragInfo,
											OilCoord *pMousePos, KeyPress* pKeyPress)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/3/95
	Inputs:		Event - Indicates what has happened
				pDragInfo - points to drag information describing this drag. This
				should be a BitmapDragInformation or derivation thereof
				pMousePos - points to information on the current mouse position, in OIL coords
				pKeyPress - NULL, or if for a keypress event, keypress information

	Returns:	TRUE to claim the event, FALSE to let it through to other targets

	Purpose:	Event Handler for SuperGallery listitem drag events. Overrides the
				base class handler to enable it to sort out the node being dragged
				for Bitmap drags.

********************************************************************************************/

BOOL SGBitmapDragTarget::ProcessEvent(DragEventType Event, DragInformation *pDragInfo,
										OilCoord *pMousePos, KeyPress* pKeyPress)
{
PORTNOTE("other", "Disabled use of BitmapDragInformation");
#ifndef EXCLUDE_FROM_XARALX
	if (!pDragInfo->IsKindOf(CC_RUNTIME_CLASS(BitmapDragInformation)))
		return(FALSE);

	SGDisplayNode *DraggedNode = NULL;
	BOOL IsSimpleBitmapDrag = TRUE;			// Only one bitmap is being dragged

	if (IS_A(pDragInfo, GalleryBitmapDragInfo))
	{
		DraggedNode = ((GalleryBitmapDragInfo *)pDragInfo)->GetDraggedBitmap();

		IsSimpleBitmapDrag = FALSE;			// We started the drag, so we will accept multiple
											// bitmaps being dragged in a single operation
	}

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
				// Call a subroutine to work out and set our current cursor shape
				return(DetermineCursorShape((SuperGallery *) TargetDialog,
											DraggedNode, pMousePos));
			default:
				break;
		}
	}
#endif
	// Otherwise, we aren't interested in the event, so we don't claim it
	return(FALSE);
}

PORTNOTE ("other", "Removed GalleryBitmapDragInfo class")
#ifndef EXCLUDE_FROM_XARALX
/********************************************************************************************

>	void GalleryBitmapDragInfo::GalleryBitmapDragInfo() 
	 
	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/3/95		  

	Purpose:	Default constructor - do not call this constructor

********************************************************************************************/

GalleryBitmapDragInfo::GalleryBitmapDragInfo()
{
	ERROR3("Default GalleryBitmapDragInfo constructor called");	
}



/********************************************************************************************

>	GalleryBitmapDragInfo::GalleryBitmapDragInfo(SGDisplayKernelBitmap *pSourceItem,
											SGMouseInfo *pMouseInfo, SGMiscInfo *pMiscInfo,
 											BOOL IsAdjust = FALSE)
	 
	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/3/95		  

	Inputs:		pSourceItem - The gallery item from which the drag originated
				pMouseInfo	- The mouse info which made the item start the drag
				pMiscInfo	- The MiscInfo which accompanied the mouse event
				IsAdjust	- TRUE if this is an adjust (line-Bitmap) drag

	Purpose:	Constructor

********************************************************************************************/

GalleryBitmapDragInfo::GalleryBitmapDragInfo(SGDisplayKernelBitmap *pSourceItem,
											SGMouseInfo *pMouseInfo, SGMiscInfo *pMiscInfo,
 											BOOL IsAdjust)
					: BitmapDragInformation(m_pDragBmp = new KernelBitmap(pSourceItem->GetDisplayedKernelBitmap()->ActualBitmap, TRUE), 
											120,0,0,0, IsAdjust)
{
	SourceItem	= pSourceItem;	// Copy the source item pointer

	MouseInfo	= *pMouseInfo;	// Duplicate the structures (they may cease to exist
	MiscInfo	= *pMiscInfo;	// soon after the drag is started)
}

/********************************************************************************************

>	GalleryBitmapDragInfo::~GalleryBitmapDragInfo()
 
	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/3/95		  

	Purpose:	Destructor

********************************************************************************************/

GalleryBitmapDragInfo::~GalleryBitmapDragInfo()
{
	if (m_pDragBmp)
		delete m_pDragBmp;
}


/********************************************************************************************

>	void GalleryBitmapDragInfo::OnClick(INT32 Flags,POINT Point) 
	 
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

void GalleryBitmapDragInfo::OnClick(INT32 Flags ,POINT Point)
{
	if (SourceItem != NULL)
		SourceItem->DragWasReallyAClick(&MouseInfo, &MiscInfo);
}

/********************************************************************************************

>	void GalleryBitmapDragInfo::GetCursorID(DragTarget* pDragTarget)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/3/95
	Purpose:	Set cursor over this target


********************************************************************************************/

UINT32 GalleryBitmapDragInfo::GetCursorID(DragTarget* pDragTarget)
{
	if (pDragTarget && pDragTarget->IS_KIND_OF(ViewDragTarget))
	{
		PageDropInfo PageDropInfo;
		((ViewDragTarget*)pDragTarget)->GetDropInfo(&PageDropInfo);

		NodeRenderableInk* pObjectHit 	= PageDropInfo.pObjectHit;
		ObjectDragTarget TargetHit 		= PageDropInfo.TargetHit;

		if (pObjectHit && pObjectHit->IS_KIND_OF(NodeBitmap))
			TargetHit = NO_TARGET;				// Ignore NodeBitmap objects

		ClickModifiers ClickMods = ClickModifiers::GetClickModifiers();
		BOOL IsInside = ClickMods.Constrain;

		if (!IsInside && pObjectHit && pObjectHit->IsCompound())
		{
			TargetHit = MANY_TARGET;
		}

		UINT32 CanFill = IsInside ? _R(IDC_DROPINSIDEONFILL) : _R(IDC_CANDROPONFILL);

		switch (TargetHit)
		{
			case FILL_TARGET:
			case LINE_TARGET:
			case STARTCOL_TARGET:
			case ENDCOL_TARGET:
			case ENDCOL2_TARGET:
			case ENDCOL3_TARGET:
			case MANY_TARGET:
				return CanFill;

			case NO_TARGET:
				// bitmap gallery - drop = add bitmap, CTRL + drop = page fill
				return IsInside ? _R(IDC_CANFILLPAGE) : _R(IDC_CANDROPONPAGE);
				//return _R(IDC_CANDROPONPAGE);
		};

		return _R(IDC_CANDROPONPAGE);
	}

	return _R(IDC_CANTDROP);
}

/********************************************************************************************

>	virtual BOOL GalleryBitmapDragInfo::GetStatusLineText(String_256 * TheText, DragTarget* pDragTarget)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/3/95
	Returns:	Whether String is valid
	Purpose:	provide status line text for this target
   
********************************************************************************************/

BOOL GalleryBitmapDragInfo::GetStatusLineText(String_256 * TheText, DragTarget* pDragTarget)
{
	ERROR2IF(TheText==NULL,FALSE,"NULL string in GetStatusLineText()");

	if (TheBitmap == NULL || TheBitmap->ActualBitmap == NULL)
		return FALSE;

 	String_256 DragString;
 	String_256 BmpName(TheBitmap->ActualBitmap->GetName());
 	DragString.MakeMsg(_R(IDS_SGBITMAP_DRAGGING_BITMAP), (TCHAR *)BmpName); // "Dragging Bitmap 'Bitmap' : ");

	if (pDragTarget && pDragTarget->IS_KIND_OF(ViewDragTarget))
	{
		PageDropInfo PageDropInfo;
		((ViewDragTarget*)pDragTarget)->GetDropInfo(&PageDropInfo);

		NodeRenderableInk* pObjectHit 	= PageDropInfo.pObjectHit;
		ObjectDragTarget TargetHit 		= PageDropInfo.TargetHit;

		String_256 ObjectDesc = String_32(_R(IDS_SGBITMAP_THIS_OBJECT)); // " this object";

		if (pObjectHit)
		{
			ObjectDesc = pObjectHit->Describe(FALSE);
		}

		if (pObjectHit && pObjectHit->IS_KIND_OF(NodeBitmap))
			TargetHit = NO_TARGET;				// Ignore NodeBitmap objects

		ClickModifiers ClickMods = ClickModifiers::GetClickModifiers();
		BOOL IsInside = ClickMods.Constrain;

		if (!IsInside && pObjectHit && pObjectHit->IsCompound())
		{
			TargetHit = MANY_TARGET;
		}

		switch (TargetHit)
		{
			case FILL_TARGET:
			case LINE_TARGET:
			case STARTCOL_TARGET:
			case ENDCOL_TARGET:
			case ENDCOL2_TARGET:
			case ENDCOL3_TARGET:
				DragString += String_256(_R(IDS_SGBITMAP_DROP_APPLY_FILL)); // "Drop to apply a Bitmap fill to this ";
				DragString += ObjectDesc;
				if (IsInside)
					DragString += String_16(_R(IDS_SGBITMAP_ALONE)); // " alone";
				break;
			case MANY_TARGET:
				DragString += String_256(_R(IDS_SGBITMAP_DROP_APPLY_FILL)); // "Drop to apply a Bitmap fill to this ";
				DragString += ObjectDesc;
				DragString += String_256(_R(IDS_SGBITMAP_CONTROL_DROP_INSIDE)); // "; Press 'Control' to drop 'Inside'";
				break;
			case NO_TARGET:
				if (IsInside)
				{
					DragString += String_256(_R(IDS_DRAGBMP_DROP_SETPAGEBACKGROUND)); // "Drop to set the Bitmap fill of the page background";
				}
				else
				{
					DragString += String_256(_R(IDS_SGBITMAP_DROP_INSERT_BITMAP)); // "Drop to Insert a new Bitmap object on the Page";
				}
				break;
		};

	 	*TheText = DragString;
		return TRUE;
	}
 	
	return FALSE;
}

/********************************************************************************************

>	BOOL GalleryBitmapDragInfo::OnPageDrop(ViewDragTarget* pDragTarget)
 
	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/3/95		  
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Called when a bitmap is dropped onto the page.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL GalleryBitmapDragInfo::OnPageDrop(ViewDragTarget* pDragTarget)
{
	PageDropInfo ThePageDropInfo;
	((ViewDragTarget*)pDragTarget)->GetDropInfo(&ThePageDropInfo);
	NodeRenderableInk* pObjectHit = ThePageDropInfo.pObjectHit;

	// Find the bitmap to apply
	KernelBitmap* BitmapToApply = TheBitmap;
	TheBitmap = NULL;
	m_pDragBmp = NULL;

	if(BitmapToApply == NULL)
		return FALSE;

	// See if the user dropped the bitmap onto an object(s) on the page
	// If so then try and apply that bitmap as a fill to that object(s).
	if (pObjectHit && !pObjectHit->IS_KIND_OF(NodeBitmap)) 
	{
		// Fill the object(s) with the dropped bitmap
		AttrBitmapColourFill* Attrib = new AttrBitmapColourFill;
		if (Attrib == NULL)
			return FALSE;
						
		Attrib->AttachBitmap(BitmapToApply);
		if (Attrib->GetBitmap() != BitmapToApply)
		{
			// It didn't use the bitmap we gave it, so we can delete it
			delete BitmapToApply;
		}

		AttributeManager::ApplyAttribToNode(pObjectHit, Attrib);
	}
	else
	{
		// Not dropped onto an object on the page
		// See if the control key is pressed
		BOOL Constrain = KeyPress::IsConstrainPressed();
		if (Constrain)
		{
			// If the control key is pressed then apply the bitmap as a new background
			// We should use the document given to us by the page info class
			Document * pDocument = ThePageDropInfo.pDoc;
			if (pDocument == NULL)
				return FALSE;
			OpBackgroundParam Param;
			Param.pBitmap = BitmapToApply;
			Param.pDoc = pDocument;			
			Param.pSpread = ThePageDropInfo.pSpread;
			
			// Obtain a pointer to the op descriptor for the create operation 
			OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_BACKGROUND);

			// Invoke the operation, passing in our parameters
			pOpDesc->Invoke(&Param);		 
		}
		else
		{
			// Create a new bitmap on the page at the drop point

			// Obtain a pointer to the op descriptor for the create operation 
			OpDescriptor* OpDesc = OpDescriptor::FindOpDescriptor(CC_RUNTIME_CLASS(OpCreateNodeBitmap));

			// Invoke the operation, passing DocView and Pos as parameters
			OpParam param((void *)BitmapToApply,(void *)&ThePageDropInfo);
			OpDesc->Invoke(&param);		 
		}
	}

	return TRUE;
}

#endif // EXCLUDE_FROM_XARALX

/***********************************************************************************************

>	SGDisplayKernelBitmap::SGDisplayKernelBitmap()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/1/95 (base generated in sgbase.cpp)

	Purpose:	SGDisplayKernelBitmap constructor
				DON'T call this constructor. It ERROR3's. Call the other constructor

***********************************************************************************************/

SGDisplayKernelBitmap::SGDisplayKernelBitmap()
{
	ERROR3("Illegal call on default SGDisplayKernelBitmap constructor - call the other one!");
	TheKernelBitmap = NULL;
}


/***********************************************************************************************

>	SGDisplayKernelBitmap::SGDisplayKernelBitmap(KernelBitmap *KernelBitmapToDisplay)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/1/95 (base generated in sgbase.cpp)

	Inputs:		KernelBitmapToDisplay - The KernelBitmap this item will display

	Purpose:	SGDisplayKernelBitmap constructor

***********************************************************************************************/

SGDisplayKernelBitmap::SGDisplayKernelBitmap(KernelBitmap *KernelBitmapToDisplay)
{
	ERROR3IF(KernelBitmapToDisplay == NULL,
				"SGDisplayKernelBitmap - trying to construct me with a NULL parameter is bad");

	TheKernelBitmap = KernelBitmapToDisplay;
}



/***********************************************************************************************

>	virtual void SGDisplayKernelBitmap::CalculateMyRect(SGFormatInfo *FormatInfo,
												SGMiscInfo *MiscInfo)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/1/95 (base generated in sgbase.cpp)

	Inputs:		FormatInfo - The formatting info from which to calculate my position/size
				MiscInfo - As usual, the useful misc info struct

	Outputs:	member variable FormatRect - is returned filled in with the size/position of
				this KernelBitmap item's display area. This is dependent upon the current display
				mode and format state

				FormatInfo will be updated as a result of the formatting operation

	Purpose:	Shared code for KernelBitmap items to calculate where they will appear in the
				grand scheme of things

	Notes:		KernelBitmaps supply only one display mode ("full info")

	Scope:		private (for use of SGDisplayKernelBitmap class only)

***********************************************************************************************/

void SGDisplayKernelBitmap::CalculateMyRect(SGFormatInfo *FormatInfo, SGMiscInfo *MiscInfo)
{
	INT32 XSize;
	INT32 YSize;

	switch (MiscInfo->DisplayMode)
	{
		case 1:						// 1 - Small full info
			XSize = GridLock(MiscInfo, SG_InfiniteWidth);
			YSize = GridLock(MiscInfo, SG_DefaultSmallBmp);
			break;

		case 2:						// 2 - Large icon with name
			XSize = GridLock(MiscInfo, SG_DefaultLargeBmpText);
			YSize = GridLock(MiscInfo, SG_DefaultLargeBmp);
			break;

		default:	  				// 0 - Full Info
			XSize = GridLock(MiscInfo, SG_InfiniteWidth);
			YSize = GridLock(MiscInfo, SG_DefaultLargeBmp);
			break;
	}

	BitmapInfo Info;
	INT32 MaxYSize = YSize;

	if (TheKernelBitmap->ActualBitmap &&
		TheKernelBitmap->ActualBitmap->GetInfo( &Info )
		)
	{
		// We will adjust the Height of the format rect
		// according to the Height of the bitmap.

		double BitmapWidth  = Info.RecommendedWidth;
		double BitmapHeight = Info.RecommendedHeight;

		// Calculate the spect ratio of the bitmap
		double Ratio = BitmapHeight/BitmapWidth;

		YSize = INT32(YSize * Ratio);

		// ... set a minimum height for each item
		INT32 SelHeight = 16000;

		if (YSize < SelHeight*3)
			YSize = SelHeight*3;

		if (YSize > MaxYSize)
			YSize = MaxYSize;
	}

	CalculateFormatRect(FormatInfo, MiscInfo, XSize, YSize);
}



/***********************************************************************************************

>	virtual void SGDisplayKernelBitmap::HandleRedraw(SGRedrawInfo *RedrawInfo,
												SGFormatInfo *FormatInfo)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/1/95 (base generated in sgbase.cpp)

	Inputs:		RedrawInfo	- The information on the kernel-rendered redraw area
				FormatInfo	- The formatting information structure

				member variable FormatRect should be set up (before calling this method)
				to be the rectangle in which to draw this item

	Purpose:	SGDisplayKernelBitmap item redraw method - removed from the main HandleEvent
				method merely to make the code tidier.

	Scope:		private

***********************************************************************************************/

void SGDisplayKernelBitmap::HandleRedraw(SGRedrawInfo *RedrawInfo, SGMiscInfo *MiscInfo)
{
	if (TheKernelBitmap == NULL || TheKernelBitmap->ActualBitmap == NULL)
		return;

	StartRendering(RedrawInfo, MiscInfo);

	DocRect MyRect(FormatRect);		// Get my redraw position from the cached FormatRect


	// Is this a valid format rectangle ?
	if (MyRect.IsEmpty() || !MyRect.IsValid())
	{
		// No, so just ignore it
		// Erroring here is not good, as we are in the middle of a redraw loop
		StopRendering(RedrawInfo, MiscInfo);
		return;
	}

	// Check the rectangle is at least our minimum height,
	// and adjust it if not
	//INT32 SelHeight = 16000; 
	// Changed to allow a third line of text.
	INT32 SelHeight = 23000; 

	if (MyRect.Height() < SelHeight*3)
	{
		INT32 diff = SelHeight*3 - MyRect.Height();
	 	MyRect.Inflate(0, diff);
	}

	RenderRegion *Renderer = RedrawInfo->Renderer;

	INT32 OnePixel  		= (INT32) DevicePixels(MiscInfo, 1);
	INT32 TwoPixels 		= (INT32) DevicePixels(MiscInfo, 2);
	INT32 ThreePixels 	= (INT32) DevicePixels(MiscInfo, 3);

	Renderer->SetLineWidth(0);
	Renderer->SetLineColour(RedrawInfo->Transparent);

	// Now calculate the rectangle to plot the bitmap into
	DocRect IconRect(MyRect);

	BitmapInfo Info;

	BOOL bScaleDown = FALSE;

	if (TheKernelBitmap->ActualBitmap &&
		TheKernelBitmap->ActualBitmap->GetInfo( &Info ))
	{
		// We will try and make all the bitmaps the same width on screen.
		double BitmapWidth  = Info.RecommendedWidth;
		double BitmapHeight = Info.RecommendedHeight;

		// Calculate the spect ratio of the bitmap
		double Ratio = BitmapWidth/BitmapHeight;

		INT32 IconWidth = INT32(IconRect.Height() * Ratio);

		INT32 YSize;
		switch (MiscInfo->DisplayMode)
		{
			case 1:						// 1 - Small full info
				YSize = GridLock(MiscInfo, SG_DefaultSmallBmp);
				break;

			case 2:						// 2 - Large icon with name
				YSize = GridLock(MiscInfo, SG_DefaultLargeBmp);
				break;

			default:	  				// 0 - Full Info
				YSize = GridLock(MiscInfo, SG_DefaultLargeBmp);
				break;
		}
		
		// If the bitmap is very Wide and Thin, then we need to scale it
		// so it fits into out maximum icon width
		if (IconWidth > YSize)
		{
			Matrix Scale;
			Ratio = double(IconWidth)/double(YSize);

			Scale = Matrix(FIXED16(1)/Ratio, FIXED16(1)/Ratio);

			DocCoord Centre;
			Centre.x = IconRect.lo.x;
			Centre.y = IconRect.lo.y + IconRect.Height()/2;

			// Scale the bitmap rectangle about the Left Middle so
			// that it is always 'YSize' in Width
			IconRect.Translate(-Centre.x, -Centre.y);
			Scale.transform(&IconRect.lo);
			Scale.transform(&IconRect.hi);
			IconRect.Translate(Centre.x, Centre.y);

			IconWidth = YSize;
			bScaleDown = TRUE;
		}

		INT32 RightSpace = 0;

		if (IconWidth < YSize)
		{
			INT32 LeftSpace = TwoPixels * 4;
		 	RightSpace = YSize - IconWidth - LeftSpace;
			IconRect.Translate(LeftSpace, 0);	// Move it over a bit
		}

		// Now setup the IconRect
		IconRect.hi.x = IconRect.lo.x + IconWidth;

		MyRect.lo.x = IconRect.hi.x + RightSpace + TwoPixels;	// And exclude it from 'MyRect'
		MyRect.lo.x += TwoPixels;

		// Now check the Icon isn't too tall

		INT32 MaxYSize = YSize;
		INT32 IconHeight = IconRect.Height();
		if (IconHeight > MaxYSize)
		{
			Matrix Scale;
			Ratio = double(IconHeight)/double(YSize);

			Scale = Matrix(FIXED16(1)/Ratio, FIXED16(1)/Ratio);

			DocCoord Centre;
			Centre.x = IconRect.lo.x + IconRect.Width()/2;
			Centre.y = IconRect.hi.y;

			// Scale the bitmap rectangle about the Middle so
			// that it is always 'YSize' in Height
			IconRect.Translate(-Centre.x, -Centre.y);
			Scale.transform(&IconRect.lo);
			Scale.transform(&IconRect.hi);
			IconRect.Translate(Centre.x, Centre.y);

			MyRect.lo.y = IconRect.lo.y;
			MyRect.hi.y = IconRect.hi.y;
			bScaleDown = TRUE;
		}
	}

	// This section doesn't currently do anything special as we 
	// have gone back to using an OSRenderRegion
	// If the display quality is too bad then we should modify 
	// this code to render a high quality thumbnail and store it 
	// for future use, rendering the thumbnail into an OSRenderRegion
/*	Renderer->SetUsingSmoothedBitmaps(bScaleDown);
	if (bScaleDown)
	{
		Quality qual;
		qual.SetQuality(QUALITY_MAX);
		QualityAttribute *pQuality = new QualityAttribute(qual);
		Renderer->SetQuality(pQuality, TRUE);			// Temp
	}
	else
	{
		Quality qual;	// QUALITY_DEFAULT is implied
		QualityAttribute *pQuality = new QualityAttribute(qual);
		Renderer->SetQuality(pQuality, TRUE);			// Temp
	}*/

	GridLockRect(MiscInfo, &MyRect);	// Ensure the new 'MyRect' is pixel-grid-aligned

	// If we can, we need to leave a little space around the
	// edges of the bitmap rect, so we can see the 'Selected' rectangle
	// behind it.  We only try this if the rectangle is big enough
	// otherwise the Inflate routine will create an invalid rectangle.

	if (IconRect.Height() > ThreePixels * 2)
		IconRect.Inflate(0, -ThreePixels);		// Leave a bit of space around the edge

	if (IconRect.Width() > ThreePixels * 2)
		IconRect.Inflate(-ThreePixels, 0);		// Leave a bit of space around the edge
	
	GridLockRect(MiscInfo, &IconRect);	// Ensure it maps exactly to specific pixels

	// Need to remember the icon rect size before the selection rectangle takes over...
	DocRect ThumbnailRect(IconRect);

	// In the bitmap gallery, it's better to show something than nothing
	// So if the thumbnail has become so thin as to be invisible
	// We will bodge it back into shape even if that distorts things a bit
	if (ThumbnailRect.Width()==0)
		ThumbnailRect.hi.x = ThumbnailRect.lo.x + OnePixel;
	if (ThumbnailRect.Height()==0)
		ThumbnailRect.hi.y = ThumbnailRect.lo.y + OnePixel;

	// Set up the colours for rendering our text, and fill the background if selected
	if (Flags.Selected)
	{
		// This Item is Selected

		// Fill the entire background with the 'selected' colour, so we don't
		// get gaps between bits of text or uneven rectangles in multiple selections
		Renderer->SetFillColour(RedrawInfo->SelBackground);

		Renderer->SetFillColour(COLOUR_BLACK);
		IconRect.Inflate(ThreePixels);

		// Draw a Three Pixel wide rectangle around the edge of the bitmap
		// (Actually we're plotting a filled rectangle which is three pixels
		//  bigger than the bitmap rectangle)
		Renderer->DrawRect(&IconRect);

		// If the icon rect is wider than the gallery, then MyRect could have become invalid,
		// so we only try to draw the selection background if any of it is actually visible
		if (MyRect.IsValid())
		{
			DocRect SelRect(MyRect);
			if (SelHeight > MyRect.Height()/2)
				SelHeight = MyRect.Height()/2;

			SelRect.hi.y = MyRect.lo.y + MyRect.Height()/2 + SelHeight;
			SelRect.lo.y = MyRect.lo.y + MyRect.Height()/2 - SelHeight;
			SelRect.lo.x += ThreePixels;
			GridLockRect(MiscInfo, &SelRect);	// Ensure it maps exactly to specific pixels

			Renderer->SetFillColour(RedrawInfo->SelBackground);
			Renderer->DrawRect(&SelRect);
		}

		Renderer->SetFixedSystemTextColours(&RedrawInfo->SelForeground, &RedrawInfo->SelBackground);
	}
	else
	{
		// This Item is Not Selected
		Renderer->SetFixedSystemTextColours(&RedrawInfo->Foreground, &RedrawInfo->Background);
	}


#ifdef SGBITMAP_BACKGROUND_REDRAW

	BOOL DrawnBitmap = FALSE;

	if (ShouldIDrawForeground(DrawnBitmap))
	{
		if(!DrawnBitmap)
		{
			// Now we need to create a temporary NodeBitmap, which we will
			// use to render the bitmap preview.
			NodeBitmap* DummyBmp = new NodeBitmap();
			if (DummyBmp == NULL)
			{
				// Just exit if we're out of memory
				SGLibDisplayItem::DrawNullBitmapRect(Renderer, MiscInfo, &ThumbnailRect, FALSE);
				StopRendering(RedrawInfo, MiscInfo);
				return;
			}

			// Set the NodeBitmap path to be our IconRect, 
			// and attach the Bitmap to it.
			DummyBmp->SetUpPath();
			DummyBmp->CreateShape(ThumbnailRect);
			DummyBmp->GetBitmapRef()->SetBitmap(TheKernelBitmap);

			// Now render the bitmap preview
		  	DummyBmp->Render(Renderer);

			delete DummyBmp;
		}
	}
	else
	{
		// We should background render, so we just draw a blank grey box
		if(!DrawnBitmap)
			SGLibDisplayItem::DrawNullBitmapRect(Renderer, MiscInfo, &ThumbnailRect, TRUE);
	}

#else

	// Now we need to create a temporary NodeBitmap, which we will
	// use to render the bitmap preview.
	NodeBitmap* DummyBmp = new NodeBitmap();
	if (DummyBmp == NULL)
	{
		// Just exit if we're out of memory
		StopRendering(RedrawInfo, MiscInfo);
		return;
	}

	// Set the NodeBitmap path to be our IconRect, 
	// and attach the Bitmap to it.
	DummyBmp->SetUpPath();
	DummyBmp->CreateShape(ThumbnailRect);
	DummyBmp->GetBitmapRef()->SetBitmap(TheKernelBitmap);

	// Now render the bitmap preview
  	DummyBmp->Render(Renderer);

	delete DummyBmp;

#endif

	// Leave a small gap before text begins
	MyRect.lo.x += SG_GapBeforeText*2;

	// And now render the text describing the bitmap
	// ...but only if there is any space left (i.e. MyRect is still valid)
	if (MyRect.IsValid())
	{
		// First get the name of the bitmap
		String_256 BitmapName = TheKernelBitmap->ActualBitmap->GetName();

		// Now find how many colours it has
		String_256 BitmapCols;
		INT32 bpp = TheKernelBitmap->GetBPP();

		BOOL HasTransparency = FALSE;
		
		if (bpp <= 8)
		{
			// Check to see if this is a masked bitmap
			INT32 Index;

			if (TheKernelBitmap->GetTransparencyIndex(&Index))
				HasTransparency = TRUE;
		}

		switch (bpp)
		{
			case 1:
				if (HasTransparency)
					BitmapCols = _R(IDS_SGBITMAP_MONOCHROME_T); // "Monochrome, transparent";
				else
					BitmapCols = _R(IDS_SGBITMAP_MONOCHROME); 	// "Monochrome";
				break;

			case 2:
				if (HasTransparency)
					BitmapCols = _R(IDS_SGBITMAP_4COLOURS_T); 	// "3 colours, transparent";
				else
					BitmapCols = _R(IDS_SGBITMAP_4COLOURS); 	// "4 colours";
				break;

			case 4:
				if (HasTransparency)
					BitmapCols = _R(IDS_SGBITMAP_16COLOURS_T); 	// "15 colours, transparent";
				else
					BitmapCols = _R(IDS_SGBITMAP_16COLOURS); 	// "16 colours";
				break;

			case 8:
				if (HasTransparency)
					BitmapCols = _R(IDS_SGBITMAP_256COLOURS_T); // "255 colours, transparent";
				else
					BitmapCols = _R(IDS_SGBITMAP_256COLOURS); 	// "256 colours";
				break;

			case 16:
				BitmapCols = _R(IDS_SGBITMAP_65COLOURS); 		// "65K colours";
				break;

			case 24:
				BitmapCols = _R(IDS_SGBITMAP_MILLIONS_COLOURS); // "Millions of colours";
				break;

			case 32:
				BitmapCols = _R(IDS_SGBITMAP_32_BIT); // "Millions of colours";
				break;

			default:
				BitmapCols = "";
				break;
		}

		// Now get its pixel Width and Height
		String_256 BitmapWidth;
		Convert::LongToString(Info.PixelWidth, &BitmapWidth);
		
		String_256 BitmapHeight;
		Convert::LongToString(Info.PixelHeight, &BitmapHeight);

		// Find the delay value;
		UINT32 BitmapDelay = TheKernelBitmap->ActualBitmap->GetBitmapAnimDelay();
		String_256 Delay;
		Convert::LongToString(BitmapDelay, &Delay);

		// And finally, work out how much memory it is using
		// The memory size depends on whether this is a bitmap or JPEG.

		INT32 Bytes = 0;
		BitmapSource* pSource = NULL;
		BaseBitmapFilter* pDummyFilter;

PORTNOTE("other", "Removed XPE hook")
#ifndef EXCLUDE_FROM_XARALX
		KernelBitmap* pMaster = NULL;
		IXMLDOMDocumentPtr pEdits = NULL;
		TheKernelBitmap->GetXPEInfo(pMaster, pEdits);
		BOOL bIsXPE = (pMaster!=NULL && pEdits!=NULL);
#else
		BOOL bIsXPE = FALSE;
#endif

		BOOL OriginalSourcePresent = TheKernelBitmap->GetOriginalSource(&pSource, &pDummyFilter);
		String_256 BitmapFormat;
		if(OriginalSourcePresent)
		{
			if (pSource != NULL)
			{
				if (pSource->IsJPEG())
				{
					Bytes = pSource->GetSize();
					BitmapFormat = _R(IDS_FORMAT_JPEG);
				}
			}
		}
		else if (bIsXPE)
		{
			BitmapFormat = _R(IDS_FORMAT_XPE);
		}
		else
		{
			Bytes = (Info.PixelWidth * Info.PixelHeight * bpp)/8;
			BitmapFormat = _R(IDS_FORMAT_BITMAP);
		}

		String_256 MemoryUsed;
		Convert::BytesToString(&MemoryUsed, Bytes);

		if (!BaseCamelotFilter::ShouldSaveXPEBitmaps() && bIsXPE)
		{
			// If the bitmap is generated from a Master and edit list
			// Then don't show size in bytes 'cos it's meaningless
			MemoryUsed.Load(_R(IDS_SGBITMAP_XPE));
		}

		// Now we are going to plot all that information
		// split into two lines of text
		DocRect TopRect(MyRect);
		DocRect MiddleRect(MyRect);
		DocRect BottomRect(MyRect);

		//TopRect.lo.y 	+= 11000;
		TopRect.lo.y 	+= 18000;
		MiddleRect.hi.y 	-= 5000;
		//MiddleRect.hi.y 	-= 16000; Down
		BottomRect.hi.y	-= 26000; 

		// Plot the Name and Memory used on the Top Line
		String_256 TopLine; // = "'"+BitmapName+"', "+BitmapFormat", "+MemoryUsed;
		TopLine.MakeMsg(_R(IDS_SGBITMAP_FULLINFO_TOPLINE),(TCHAR *)BitmapName,(TCHAR*)BitmapFormat,(TCHAR *)MemoryUsed);
		Renderer->DrawFixedSystemText(&TopLine, TopRect);				


		// Plot the Dimensions and Colours on the Bottom Line
		String_256 MiddleLine; //  = BitmapWidth+" by "+BitmapHeight+" pixels,  "+BitmapCols;

		// Calculate the dpi of the original winbitmap...
		UINT32 hdpi = TheKernelBitmap->ActualBitmap->GetHorizontalDPI();
		UINT32 vdpi = TheKernelBitmap->ActualBitmap->GetVerticalDPI();

		if (hdpi == vdpi)
		{
			String_256 dpitext;
			Convert::LongToString(hdpi, &dpitext);
			MiddleLine.MakeMsg(_R(IDS_SGBITMAP_FULLINFO_BOTTOMLINE), (TCHAR *)BitmapWidth, (TCHAR *)BitmapHeight, (TCHAR *)BitmapCols, (TCHAR *)dpitext);
		}
		else
		{
			String_256 dpitext;
			camSnprintf(dpitext, 256, _T("%u x %u"), hdpi, vdpi);
			MiddleLine.MakeMsg(_R(IDS_SGBITMAP_FULLINFO_BOTTOMLINE), (TCHAR *)BitmapWidth, (TCHAR *)BitmapHeight, (TCHAR *)BitmapCols, (TCHAR *)dpitext);
		}
		Renderer->DrawFixedSystemText(&MiddleLine, MiddleRect);				


		// Find the Restore type 
		GIFDisposalMethod RestoreType = TheKernelBitmap->ActualBitmap->GetAnimationRestoreType();
		
		switch(RestoreType)
		{
			case GDM_NONE:
			{
				String_256 BottomLine;
				String_256 Nothing(_R(IDS_NOTHING));
				BottomLine.MakeMsg(_R(IDS_SGBITMAP_DELAY_RESTORE_BOTTOMLINE), (TCHAR*)Delay, (TCHAR*)Nothing);
				Renderer->DrawFixedSystemText(&BottomLine, BottomRect);	
			}
			break;
			case  GDM_LEAVE:
			{
				String_256 BottomLine;
				String_256 Leave(_R(IDS_LEAVE));
				BottomLine.MakeMsg(_R(IDS_SGBITMAP_DELAY_RESTORE_BOTTOMLINE), (TCHAR*)Delay, (TCHAR*)Leave);
				Renderer->DrawFixedSystemText(&BottomLine, BottomRect);				
			}
			break;
			case GDM_BACKTOBACK:
			{
				String_256 BottomLine;
				String_256 Back(_R(IDS_BACKGROUND));
				BottomLine.MakeMsg(_R(IDS_SGBITMAP_DELAY_RESTORE_BOTTOMLINE), (TCHAR*)Delay, (TCHAR*)Back);
				Renderer->DrawFixedSystemText(&BottomLine, BottomRect);				
			}
			break;
			case GDM_PREVIOUS:
			{
				String_256 BottomLine;
				String_256 Previous(_R(IDS_PREVIOUS));
				BottomLine.MakeMsg(_R(IDS_SGBITMAP_DELAY_RESTORE_BOTTOMLINE), (TCHAR*)Delay, (TCHAR*)Previous);
				Renderer->DrawFixedSystemText(&BottomLine, BottomRect);				
			}
			break;

			default:
				break;
		}
	}

	StopRendering(RedrawInfo, MiscInfo);
}


/***********************************************************************************************

>	virtual void SGDisplayKernelBitmap::DragWasReallyAClick(SGMouseInfo *Mouse, SGMiscInfo *MiscInfo)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/3/95

	Inputs:		Mouse - The mouse info passed to the original click handler
				MiscInfo - The misc info passed to the original click handler

	Purpose:	Handles a mouse click event. This is a callback function - drags of
				bitmaps from galleries will call this function back if the drag turns
				out to just be a click.

	SeeAlso:	SGDisplayKernelBitmap::HandleEvent; GalleryBitmapDragInfo::OnClick

***********************************************************************************************/

void SGDisplayKernelBitmap::DragWasReallyAClick(SGMouseInfo *Mouse, SGMiscInfo *MiscInfo)
{
	// Just get default selection action to be applied for this click
	DefaultClickHandler(Mouse, MiscInfo, TRUE);
}

/***********************************************************************************************

>	virtual BOOL SGDisplayKernelBitmap::HandleEvent(SGEventType EventType, void *EventInfo,
											 SGMiscInfo *MiscInfo)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
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

BOOL SGDisplayKernelBitmap::HandleEvent(SGEventType EventType, void *EventInfo,
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
					// If the colour is in the selected document, then it is safe to
					// do a colour drag - for now, it will only allow drags for the
					// selected doc.
					// Otherwise, the normal click action takes place.
					// If the drag fails (turns into a click) then the normal click action
					// takes place, passed on by the GalleryColourDragInfo::OnClick handler
//					SGDisplayGroup *Parent = (SGDisplayGroup *) GetParent();

					if (Mouse->DoubleClick) // || Parent->GetParentDocument() != Document::GetSelected())
					{
						// Send bitmap to Xara Photo Editor
/*						OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_XPE_EDIT);
						if (pOpDesc)
						{
							pOpDesc->Invoke();
							return TRUE;
						}
						else
*/
							DefaultClickHandler(Mouse, MiscInfo);
					}
					else
					{
PORTNOTE("other", "Disabled GalleryBitmapDragInfo")
#ifndef EXCLUDE_FROM_XARALX
						DefaultPreDragHandler(Mouse, MiscInfo);

						GalleryBitmapDragInfo *DragBmp;
						DragBmp = new GalleryBitmapDragInfo(this, Mouse, MiscInfo,
															Mouse->MenuClick);
						if (DragBmp != NULL)
							DragManagerOp::StartDrag(DragBmp);
#endif
					}

					return(TRUE);		// Claim this event - nobody else can own this click
				}
			}
			break;

		default:
			return(SGDisplayItem::HandleEvent(EventType, EventInfo, MiscInfo));
	}

	// Default return value: We do not claim this event, so it will be passed on to others
	return(FALSE);
}

/********************************************************************************************

>	virtual void SGDisplayKernelBitmap::GetNameText(String_256 *Result)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/3/95

	Outputs:	On exit, the string pointed at by Result will contain either a blank
				string, or the name text associated with this item (if any)

	Purpose:	To determine a name string for this node. Generally, this is used for
				a simple mechanism which searches for display items whose names match
				given search parameters in some way. It is also used in libraries to
				provide default redraw methods.

	SeeAlso:	SGDisplayKernelBitmap::GetFullInfoText

********************************************************************************************/

void SGDisplayKernelBitmap::GetNameText(String_256 *Result)
{
	ERROR3IF(Result == NULL, "Illegal NULL param");

	*Result = TheKernelBitmap->ActualBitmap->GetName();
}



/********************************************************************************************

>	virtual void SGDisplayKernelBitmap::GetFullInfoText(String_256 *Result)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/3/95

	Outputs:	On exit, the string pointed at by Result will contain either a blank
				string, or the full-information text associated with this item (if any)

	Purpose:	To determine a full-info string for this node. Generally, this is used for
				a simple mechanism which searches for display items whose info matches
				given search parameters in some way. It is also used in libraries to
				provide default redraw methods.

	SeeAlso:	SGDisplayKernelBitmap::GetNameText

********************************************************************************************/

void SGDisplayKernelBitmap::GetFullInfoText(String_256 *Result)
{
	ERROR3IF(Result == NULL, "Illegal NULL param");

	*Result = TheKernelBitmap->ActualBitmap->GetName();
}

/***********************************************************************************************

>	virtual void SGDisplayKernelBitmap::MoveAfter(SGDisplayNode *NodeToMove)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/3/95

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

				Any attempt to move an item after *itself* is queitly ignored

	Errors:		ERROR3 and quiet exit if NodeToMove == NULL

	SeeAlso:	SGDisplayKernelBitmap::MoveBefore

***********************************************************************************************/

void SGDisplayKernelBitmap::MoveAfter(SGDisplayNode *NodeToMove)
{
	ERROR3IF(NodeToMove == NULL, "Illegal NULL param");

	if (NodeToMove == this)
		return;

	KernelBitmap *BitmapToMove = ((SGDisplayKernelBitmap *) NodeToMove)->GetDisplayedKernelBitmap();
	ERROR3IF(BitmapToMove == NULL, "NULL displayed bitmap?!");

	Document *ScopeDoc = ((SGDisplayGroup *) GetParent())->GetParentDocument();
	ERROR3IF(ScopeDoc == NULL, "No parent document?!");

	if ( ((SGDisplayGroup *) ( ((SGDisplayKernelBitmap *)NodeToMove)->GetParent() ) )->GetParentDocument() !=
			ScopeDoc)
	{
		ERROR2RAW("Attempt to MOVE a bitmap between documents!");
		return;
	}

	BitmapSGallery::IgnoreBmpListChange = TRUE;	

	// And get the bitmap list
	BitmapList* BmpList = ScopeDoc->GetBitmapList();
	ERROR3IF(BmpList == NULL, "A document with no bitmap list?!");
	
	BmpList->MoveAfter(GetDisplayedKernelBitmap(), BitmapToMove);
	SGDisplayNode::MoveAfter(NodeToMove);

	BitmapSGallery::IgnoreBmpListChange = FALSE;	
}

/***********************************************************************************************

>	virtual void SGDisplayKernelBitmap::MoveBefore(SGDisplayNode *NodeToMove)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/3/95

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

				Any attempt to move an item Before *itself* is queitly ignored

	Errors:		ERROR3 and quiet exit if NodeToMove == NULL

	SeeAlso:	SGDisplayKernelBitmap::MoveBefore

***********************************************************************************************/

void SGDisplayKernelBitmap::MoveBefore(SGDisplayNode *NodeToMove)
{
	ERROR3IF(NodeToMove == NULL, "Illegal NULL param");

	if (NodeToMove == this)
		return;

	KernelBitmap *BitmapToMove = ((SGDisplayKernelBitmap *) NodeToMove)->GetDisplayedKernelBitmap();
	ERROR3IF(BitmapToMove == NULL, "NULL displayed bitmap?!");

	Document *ScopeDoc = ((SGDisplayGroup *) GetParent())->GetParentDocument();
	ERROR3IF(ScopeDoc == NULL, "No parent document?!");

	if ( ((SGDisplayGroup *) ( ((SGDisplayKernelBitmap *)NodeToMove)->GetParent() ) )->GetParentDocument() !=
			ScopeDoc)
	{
		ERROR2RAW("Attempt to MOVE a bitmap between documents!");
		return;
	}

	BitmapSGallery::IgnoreBmpListChange = TRUE;	

	// And get the bitmap list
	BitmapList* BmpList = ScopeDoc->GetBitmapList();
	ERROR3IF(BmpList == NULL, "A document with no bitmap list?!");
	
	BmpList->MoveBefore(GetDisplayedKernelBitmap(), BitmapToMove);

	SGDisplayNode::MoveBefore(NodeToMove);

	BitmapSGallery::IgnoreBmpListChange = FALSE;	
}

/********************************************************************************************

>	virtual BOOL SGDisplayKernelBitmap::GetStatusLineHelp(DocCoord *MousePos, String_256 *Result)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/7/95

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

********************************************************************************************/

BOOL SGDisplayKernelBitmap::GetStatusLineHelp(DocCoord *MousePos, String_256 *Result)
{
	ERROR3IF(MousePos == NULL || Result == NULL, "Invalid NULL params");

	String_256 Name;
	GetNameText(&Name);

	// "'Bitmapname'"
	Result->MakeMsg(_R(IDS_SGBITMAP_STATLINE), (TCHAR *)Name);

	/* Double click, or drag and drop to apply as a fill*/
	*Result += String_256(_R(IDS_SGBITMAP_DOUBLE_CLICK)); //"; Double click, or drag and drop to apply this bitmap");

	return(TRUE);
}











/********************************************************************************************

>	BitmapSGallery::BitmapSGallery()
												 
	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/1/95 (base generated in sgbase.cpp)
	Purpose:	BitmapSGallery default constructor

********************************************************************************************/

BitmapSGallery::BitmapSGallery()
{
	DlgResID = _R(IDD_BITMAPSGALLERY);

	// WEBSTER - markn 9/12/96
	// Default gallery size
#ifdef WEBSTER
	CSize Size(333, 256);
	SetGallerySize(Size);
#endif
} 



/********************************************************************************************

>	BitmapSGallery::~BitmapSGallery()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/1/95 (base generated in sgbase.cpp)
	Purpose:	BitmapSGallery destructor.

********************************************************************************************/

BitmapSGallery::~BitmapSGallery()
{
}



/********************************************************************************************

>	void BitmapSGallery::CreateNewSubtree(Document *ParentDoc, SGDisplayGroup *ExistingGroup)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/1/95 (base generated in sgbase.cpp)

	Inputs:		ParentDoc - The document to create a display subtree for
				ExistingGroup - NULL (creates a new group for this document), or
				a pointer to the existing group-node for this document (in which case
				it clears all displayitems from the group and rebuilds it in place - this
				stops the display group moving around the tree at random!)

	Purpose:	Internal call. This takes the KernelBitmap list of the given document and 
				creates a DisplayTree subtree from it. This subtree is then added to
				the DisplayTree. Note that this does not force a redraw of the list - 
				after making this call, you should also call ForceRedrawOfList

	Notes:		Passing in a NULL parent document pointer results in an ERROR3 -
				the function returns without doing anything in retail builds

	SeeAlso:	SuperGallery::ForceRedrawOfList

********************************************************************************************/

void BitmapSGallery::CreateNewSubtree(Document *ParentDoc, SGDisplayGroup *ExistingGroup)
{
	ERROR3IF(ParentDoc == NULL, "BitmapSGallery::CreateNewSubtree - NULL parameter passed in");
	if (ParentDoc == NULL || DisplayTree == NULL)
		return;

	SGDisplayGroup  *DisplayDocument;
	SGDisplayKernelBitmap *DisplayKernelBitmap;

	if (ExistingGroup != NULL)
	{
		ERROR3IF(ExistingGroup->GetParentDocument() != ParentDoc, 
				 "This group is not for that document! What's going down, dude?");
		DisplayDocument = ExistingGroup;							// Use existing group
		DisplayDocument->DestroySubtree(FALSE);	// Wipe any existing KernelBitmap display items
	}
	else
	{
		DisplayDocument = new SGDisplayGroup(this, ParentDoc,NULL);	// Create new Group

		if (DisplayDocument == NULL)								// Total failure - abort
			return;

		DisplayDocument->Flags.CanSelect = TRUE;					// Groups are selectable
		DisplayTree->AddItem(DisplayDocument);						// Add new group to tree
	}

	// Lets generate some SGDisplayKernelBitmaps to be displayed ...

	// Get the bitmap list
	BitmapList* Bitmaps = ParentDoc->GetBitmapList();

	// now iterate though the list, creating display items for each bitmap
	KernelBitmap* Ptr = (KernelBitmap*)Bitmaps->GetHead();
	while (Ptr != NULL)
	{
		// Ignore bitmaps that have been deleted
		if (!Ptr->GetActualBitmap()->IsHiddenInGallery())
		{
			DisplayKernelBitmap = new SGDisplayKernelBitmap(Ptr);
	   		if (DisplayKernelBitmap != NULL)
	      		DisplayDocument->AddItem(DisplayKernelBitmap);
		}

		Ptr = (KernelBitmap*)Bitmaps->GetNext(Ptr);
	}
}



/********************************************************************************************

>	BOOL BitmapSGallery::PreCreate(void)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/1/95 (base generated in sgbase.cpp)

	Returns:	TRUE if the Gallery initialised successfully
				FALSE if it should not be opened due to a failure to initialise

	Purpose:	The BitmapSGallery PreCreate handler. This overrides the base class
				PreCreate function. It is called at the very end of the
				SuperGallery::Create method, after the window has been created.

********************************************************************************************/

BOOL BitmapSGallery::PreCreate(void)
{
	// If there isn't already one, create a DisplayTree
	if (DisplayTree == NULL)
	{
		DisplayTree = new SGDisplayRootScroll(this);	// New root node, with a scrollbar
		if (DisplayTree == NULL)
			return(FALSE);
	}

	// Next, call CreateNewSubtree to scan all the available documents and build a
	// display list for each of them.
	Document *ParentDoc = (Document *) GetApplication()->Documents.GetTail();
	while (ParentDoc != NULL)
	{
		SGDisplayNode *Ptr = DisplayTree->FindSubtree(this, ParentDoc, NULL);
		CreateNewSubtree(ParentDoc, (SGDisplayGroup *) Ptr);
		ParentDoc = (Document *) GetApplication()->Documents.GetPrev(ParentDoc);
	}

	// We're opening the window, so we fold all groups except that for the Selected document
	SGDisplayGroup *Group = (SGDisplayGroup *) DisplayTree->GetChild();
	while (Group != NULL)
	{
		// Set the group's folded state, but don't bother trying to redraw, because we know
		// that the window is not currently visible, and that we are doing this to multiple groups.
		Group->SetFoldedState((Group->GetParentDocument() != Document::GetSelected()), FALSE);		
		Group = (SGDisplayGroup *) Group->GetNext();
	}

	return(TRUE);
}

/********************************************************************************************

>	void BitmapSGallery::SetSelectionFromDocument(BOOL AlwaysScroll = FALSE)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/8/96 (copied from ColourGallery)

	Inputs:		AlwaysScroll - TRUE if you always want to autoscroll, regardless of the
				preference setting. (This is used when opening the gallery, as there is no
				point in not auto-scrolling somewhere useful)

	Purpose:	Sets the bitmap gallery listbox selection state based upon the currently
				selected object(s) in the document.

	Scope:		private

********************************************************************************************/

void BitmapSGallery::SetSelectionFromDocument(BOOL AlwaysScroll)
{
	if (DisplayTree == NULL || !IsVisible())
		return;
	
	SGDisplayGroup *DocGroup = DisplayTree->FindSubtree(this, Document::GetSelected(), NULL);
	SelectItems(FALSE);					// Clear the current selection

	// This needs to be a preference if we ever make this gallery show the
	// currently selected bitmaps.
	BOOL AutoScrollSelection = FALSE;

	DocRect ScrollToRect;
	BOOL HaveSelected = FALSE;
	BOOL ScrollToBottom = FALSE;	// TRUE if it is more important that the bottom of
									// ScrollToRect, rather than the top, is visible
	if (DocGroup != NULL)
		ForceGroupFolded(DocGroup, FALSE);	// Ensure group is unfolded

	ReformatNow(TRUE);				// Ensure the tree format information is up to date

	if (AutoScrollSelection || AlwaysScroll)
	{
		if (AlwaysScroll && !HaveSelected && DocGroup != NULL)
		{
			// We didn't select anything in the gallery. In that case, scroll to the top
			// of the document display
			DocRect TempRect;
			//DocGroup->GetFormatRect(&TempRect);
			DocGroup->GetChildArea(&ScrollToRect);
			//ScrollToRect = ScrollToRect.Union(TempRect);
			HaveSelected = TRUE;
			ScrollToBottom = FALSE;
		}

		if (HaveSelected)
			ScrollToShow(&ScrollToRect, ScrollToBottom);
	}
												// scroll to make seln visible on screen

	SelectionHasChanged();						// Update for the new selection
}

/********************************************************************************************

>	virtual SGDisplayItem *BitmapSGallery::CopyDisplayItem(SGDisplayItem *SourceItem, 
									SGDisplayGroup *DestGroup,
									SGDisplayItem *TargetPosition = NULL)
	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/3/95

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
				SGDisplayItem::MoveAfter; SGColour::CopyDisplayItem

********************************************************************************************/

SGDisplayItem *BitmapSGallery::CopyDisplayItem(SGDisplayItem *SourceItem, 
						SGDisplayGroup *DestGroup, SGDisplayItem *TargetPosition)
{
	ERROR3IF(SourceItem == NULL || DestGroup == NULL, "Illegal NULL param");

	return(NULL);

/**** Example psuedocode ****

	// Find the real item (Thingy) to be moved/copied
	Thingy *ThingyToCopy = ((SGDisplayThingy *)SourceItem)->GetDisplayedThingy();

	if (we really want to copy the items, rather than just move them)
	{
		// Just copy the items...
		
		// First, copy the real-item associated with the display item
		ThingyType *NewThingy = new ThingyType(ThingyToCopy);

		// Add the copied item to your 'real item' list or whatever.
		DocThingyList = DestGroup->GetParentDocument()->FindThingyList();

		if (TargetPosition == NULL)
			DocThingyList->AddTail(NewThingy);
		else
		{
			Thingy *TargetPos = ((SGDisplayThingy *)TargetPosition)->GetDisplayedThingy();
			if (TargetPos == NULL)
				DocThingyList->AddTail(NewThingy);
			else
				DocThingyList->InsertBefore(TargetPos, NewThingy);
		}

		// Now, create a new display item for the copied Thingy, and call the
		// base class helper method to insert it as appropriate.
		SGDisplayThingy *NewThingyDisplayItem = new SGDisplayThingy(NewThingy);
		if (NewThingyDisplayItem != NULL)
			SuperGallery::InsertCopiedItem(NewThingyDisplayItem, DestGroup, TargetPosition);

		return(NewThingyDisplayItem);
	}
	else
	{
		// Just move the items
		
		// This code will be much the same as the above, but instead of copying the
		// real-item, you will have to move it as appropriate. Instead of making
		// a new DisplayItem to pass on to InsertCopiedItem, simply delink it from the
		// DisplayTree, and then pass it to InsertCopiedItem as follows:

		SourceItem->RemoveFromTree();		// Delink from current displaytree position
		SuperGallery::InsertCopiedItem(SourceItem, DestGroup, TargetPosition);

		return(SourceItem);
	}

****/

}



KernelBitmap* BitmapSGallery::GetSelectedBitmap()
{
	// Determine useful info - this is usually needed for most actions, so always get it
	Document		*SelectedDoc = Document::GetSelected();
	/*SGDisplayGroup	*DocumentGroup =*/ DisplayTree->FindSubtree(this, SelectedDoc, NULL);
	SGDisplayKernelBitmap	*FirstSelected = NULL;
	
	// Lets see if any of our Items are selected
	Document *ParentDoc = (Document *) GetApplication()->Documents.GetHead();
	while (ParentDoc != NULL)
	{
		SGDisplayGroup *DocGroup = DisplayTree->FindSubtree(this, ParentDoc, NULL);
		if (DocGroup != NULL)
			FirstSelected = (SGDisplayKernelBitmap *) DocGroup->FindNextSelectedItem(NULL);

		if (FirstSelected)
			break;

		ParentDoc = (Document *) GetApplication()->Documents.GetNext(ParentDoc);
	}

	KernelBitmap *SelectedBitmap = NULL;
	if (FirstSelected != NULL)
	{
		SelectedBitmap = FirstSelected->GetDisplayedKernelBitmap();
	}

	return SelectedBitmap;
}

/********************************************************************************************

>	virtual BOOL BitmapSGallery::ApplyAction(SGActionType Action)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/1/95 (base generated in sgbase.cpp)

	Inputs:		Action - Indicates what action to apply

	Returns:	TRUE to indicate successful handling of the action, or
				FALSE to indicate failure (NOTE that this is not necessarily an error, but
				may simply be that we don't support the given action. e.g. if we don't
				handle APPLYADJUST, we'll be called again with a normal APPLY call)

	Purpose:	Applies certain conventional gallery actions (usually associated with
				gallery buttons, for new, edit, delete, etc)

	SeeAlso:	SGActionType

********************************************************************************************/

BOOL BitmapSGallery::ApplyAction(SGActionType Action)
{
	// No display tree? Better forget about it then!
	if (DisplayTree == NULL)
		return(FALSE);

	// Determine useful info - this is usually needed for most actions, so always get it
	Document		*SelectedDoc = Document::GetSelected();
	/*SGDisplayGroup	*DocumentGroup =*/ DisplayTree->FindSubtree(this, SelectedDoc, NULL);
	SGDisplayKernelBitmap	*FirstSelected = NULL;
	
	// Lets see if any of our Items are selected
	Document *ParentDoc = (Document *) GetApplication()->Documents.GetHead();
	while (ParentDoc != NULL)
	{
		SGDisplayGroup *DocGroup = DisplayTree->FindSubtree(this, ParentDoc, NULL);
		if (DocGroup != NULL)
			FirstSelected = (SGDisplayKernelBitmap *) DocGroup->FindNextSelectedItem(NULL);

		if (FirstSelected)
			break;

		ParentDoc = (Document *) GetApplication()->Documents.GetNext(ParentDoc);
	}

	KernelBitmap *SelectedBitmap = NULL;
	if (FirstSelected != NULL)
	{
		SelectedBitmap = FirstSelected->GetDisplayedKernelBitmap();
	}

	// Now, process the action	TO DO! - see Colour gallery for examples
	switch(Action)
	{
		case SGACTION_APPLY:
			if (SelectedBitmap != NULL)
			{
				// Fill colour selected	so create a fill colour attribute
				NodeAttribute *Attrib = new AttrBitmapColourFill;
				if (Attrib == NULL)
					return(FALSE);

				((AttrBitmapColourFill *)Attrib)->AttachBitmap(SelectedBitmap);

				// AttributeSelected knows what to do with a selected attribute
				AttributeManager::AttributeSelected(NULL, Attrib);
			}
			break;

		case SGACTION_DELETE:
			return DeleteSelection();
			break;

		case SGACTION_SETOPTIONS:	// Set values in the options dialogue as it is opened
			if (CurrentOptionsDlg == NULL)
				return(FALSE);
																		// Display Modes
			CurrentOptionsDlg->AddDisplayModeName(_R(IDS_GALLERYDM_FULLINFO));	// 0
			break;

		case SGACTION_DISPLAYMODECHANGED:
			if (DisplayMode < 0 || DisplayMode > 1)
				DisplayMode = 0;
			break;

		default:		// Unknown/unsupported command (e.g. CREATE, APPLYADJUST), so we failed to do it
			return(FALSE);
	}

	return(TRUE);
}

/********************************************************************************************

>	BOOL BitmapSGallery::DeleteSelection()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/2/95
	Purpose:	Deletes the selected bitmaps.

********************************************************************************************/

BOOL BitmapSGallery::DeleteSelection()
{
	INT32 ButtonPressed = 2;

	ButtonPressed = AskQuestion(_R(IDS_BMPGAL_BMPINUSE),
								_R(IDS_BMPGAL_KILL), _R(IDS_CANCEL), 0,	0,
								2, 2);	// CANCEL=default, CANCEL=cancel

	if (ButtonPressed == 2)		// Delete was cancelled - return FALSE
		return(FALSE);

	if (ButtonPressed == 1)		// Force-delete all items, so go ahead and delete
	{
		BOOL DeletedBitmaps = FALSE;
		IgnoreBmpListChange = TRUE;

		GlobalBitmapList* pGlobalBmpList = GetApplication()->GetGlobalBitmapList();

		Document *ParentDoc = (Document *) GetApplication()->Documents.GetHead();
		while (ParentDoc != NULL)
		{
			// Get the bitmap list
			BitmapList* pDocBitmaps = ParentDoc->GetBitmapList();

			SGDisplayKernelBitmap* Selected = NULL;

			SGDisplayGroup *DocGroup = DisplayTree->FindSubtree(this, ParentDoc, NULL);
			if (DocGroup != NULL)
				Selected = (SGDisplayKernelBitmap *) DocGroup->FindNextSelectedItem(NULL);

			while (Selected != NULL)
			{
				KernelBitmap* SelectedBitmap = Selected->GetDisplayedKernelBitmap();

				// Get the next one before we delete this one
				Selected = (SGDisplayKernelBitmap*)DocGroup->FindNextSelectedItem(Selected);

				// Is this Kernel bitmap pointer still valid ?
				if (!pDocBitmaps->IsEmpty() &&
					pDocBitmaps->FindPosition(SelectedBitmap) != NOT_IN_LIST)
				{
					if (SelectedBitmap->ActualBitmap != OILBitmap::Default)
					{
						// Is this Actual bitmap pointer still valid ?
						if (!pGlobalBmpList->IsEmpty() &&
							pGlobalBmpList->FindPosition(SelectedBitmap->ActualBitmap) != NOT_IN_LIST)
						{
							OpDeleteBitmap* pOp = new OpDeleteBitmap();
							if (pOp)
							{
								pOp->DoDelete(SelectedBitmap->ActualBitmap);
								DeletedBitmaps = TRUE;
							}
						}
					}
					else
					{
						// Shouldn't we be using an error from the resource file here??
						Error::SetError(0, _T("You cannot delete the Default Bitmap"),0);
						InformError();
					}
				}
			}

			ParentDoc = (Document *) GetApplication()->Documents.GetNext(ParentDoc);
		}
		
		IgnoreBmpListChange = FALSE;

		if (DeletedBitmaps)
		{
			Document *ParentDoc = (Document *) GetApplication()->Documents.GetHead();
			while (ParentDoc != NULL)
			{
				SGDisplayNode *Ptr = DisplayTree->FindSubtree(this, ParentDoc, NULL);
				CreateNewSubtree(ParentDoc, (SGDisplayGroup *) Ptr);

				ParentDoc = (Document *) GetApplication()->Documents.GetNext(ParentDoc);
			}

			// And force a redraw of the entire list
			ForceRedrawOfList();
			SelectionHasChanged();
		}
	}

	return TRUE;
}

/********************************************************************************************

>	KernelBitmap* BitmapSGallery::CheckTextureBitmap(KernelBitmap* pBitmap)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/2/95
	Purpose:	Checks a bitmap to see if can be applied as a texture,
				and gives the option to create a grey level version if not.

********************************************************************************************/

KernelBitmap* BitmapSGallery::CheckTextureBitmap(KernelBitmap* pBitmap)
{
	return NodeBitmap::CheckGreyscaleBitmap(pBitmap, _R(IDS_BMPGAL_MAKEGREY),
											 		 _R(IDS_BMPGAL_DOGREY));

/*
	if (!BfxALU::IsGreyscaleBitmap(pBitmap))	// Should do proper Grey Level check here
	{
		INT32 ButtonPressed = 1;

		ButtonPressed = AskQuestion(_R(IDS_BMPGAL_MAKEGREY),
									_R(IDS_BMPGAL_DOGREY), _R(IDS_CANCEL), 0,	0,
									1, 1);	// CANCEL=default, CANCEL=cancel

		if (ButtonPressed == 2)		// Apply was cancelled - return NULL
			return NULL;

		if (ButtonPressed == 1)
		{
			// Make a new grey level version of the bitmap, and apply that instead
			ABFXMakeGreyscale GSBfx;
			KernelBitmap* pNewBitmap = GSBfx.GetProcessedBitmap(pBitmap);

			if (pNewBitmap == NULL)
			{
				InformError();
				return NULL;
			}

			// We need to check the Document bitmap list to see if
			// there is already a grey-level version.

			BitmapList* pBmpList = pBitmap->GetParentBitmapList();
			ListItem* pItem = pBmpList->GetHead();

			while (pItem != NULL)
			{
				KernelBitmap* pBmp = (KernelBitmap*)pItem;

				if (pBmp->ActualBitmap != pNewBitmap->ActualBitmap &&
					*pBmp->ActualBitmap == *pNewBitmap->ActualBitmap)
				{
				 	pNewBitmap->ActualBitmap->Detach();
					delete pNewBitmap;

					return pBmp;
				}

				pItem = pBmpList->GetNext(pItem);
			}

			// There is no grey level version in this document,
			// but there might be one in another document somewhere
			// so we'll have a look in the global list.

			GlobalBitmapList* pGlobalBmpList = GetApplication()->GetGlobalBitmapList();
			pItem = pGlobalBmpList->GetHead();

			while (pItem != NULL)
			{
				OILBitmap* pBmp = (OILBitmap*)pItem;

				if (pBmp != pNewBitmap->ActualBitmap &&
					*pBmp == *pNewBitmap->ActualBitmap)
				{
				 	pNewBitmap->ActualBitmap->Detach();
					delete pNewBitmap;

					return new KernelBitmap(pBmp);
				}

				pItem = pGlobalBmpList->GetNext(pItem);
			}

			String_256 NewName = pBitmap->ActualBitmap->GetName();
			NewName += " (Greyscale)";
			pNewBitmap->ActualBitmap->SetName(NewName);

			return pNewBitmap;
		}
	}

	return pBitmap;
*/
}

/********************************************************************************************

>	virtual void BitmapSGallery::SelectionHasChanged(void);

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/1/95

	Purpose:	To inform the gallery that the selection has changed in some way.
				The base gallery class will do the following things:

				MonoOn
				- If the selection is not exactly one item, the following standard
				buttons are shaded:
					Apply, Redefine, Edit

				- If the selection is greater than 1 item, the New button will shade

				- If the selection is zero items, then the Delete button will shade
				MonoOff

	Notes:		If this default action is unsuitable, derived galleries may override it.
				If they add their own special buttons, they may need to override this
				method to ensure that their buttons also behave correctly on sel change.

				This method is called after creation of the window (immediately after
				the PostCreate upcall) to ensure the window state is correct

********************************************************************************************/

void BitmapSGallery::SelectionHasChanged(void)
{
	if (DisplayTree == NULL || AmShaded || !IsVisible())
		return;

	INT32 Count = DisplayTree->GetSelectedItemCount();

	EnableGadget(_R(IDC_BMPGAL_CREATE),		(Count == 1));

	EnableGadget(_R(IDC_BMPGAL_FILL),		(Count == 1));

	EnableGadget(_R(IDC_BMPGAL_TEXTURE),	(Count == 1));

	EnableGadget(_R(IDC_BMPGAL_XPE_EDIT),	(Count == 1));
	EnableGadget(_R(IDC_BMPGAL_TRACE),		(Count == 1));
//	EnableGadget(_R(IDC_BMPGAL_EFFECTS),	(Count == 1));

#ifdef PHOTOSHOPPLUGINS
// Only add in if required - general plug-in removal at present
	EnableGadget(_R(IDC_BMPGAL_PLUGINS),	(Count == 1));
#endif

	EnableGadget(_R(IDC_BMPGAL_BACKGROUND),	(Count == 1));

	// Now that we handle multiple images we enable as long as there is a selection
	EnableGadget(_R(IDC_BMPGAL_SAVE), (Count > 0));

	EnableGadget(_R(IDC_BMPGAL_PROPS),(Count > 0));

//#if _DEBUG
	// Now that we handle multiple images we enable as long as there is a selection
	EnableGadget(_R(IDC_BMPGAL_PREVIEW), (Count > 0));
//#endif

	if (Count == 1 && GetSelectedBitmap()->ActualBitmap == OILBitmap::Default)
	{
		EnableGadget(_R(IDC_GALLERY_DELETE),	FALSE);
	}
	else
	{
		EnableGadget(_R(IDC_GALLERY_DELETE),	(Count > 0));
	}

	EnableGadget(_R(IDC_GALLERY_MENU),		TRUE);
}

/***********************************************************************************************

>	void BitmapSGallery::DoShadeGallery(BOOL ShadeIt)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/7/97
	Inputs:		ShadeIt -	TRUE if the gallery is being shaded
							FALSE if the gallery is being unshaded
	Purpose:	Called by the base class whenever the shaded status of the gallery
				is changed, to allow derived galleries to shade/unshade any extra
				controls that they provide in the window. ONLY called if the gallery
				is actually open/visible.

	Notes:		Need not be overridden - the default action is to do nothing extra
				to the normal shade/unshade operation.
	SeeAlso:	Gallery::ShadeGallery; Gallery::UnshadeGallery

***********************************************************************************************/

void BitmapSGallery::DoShadeGallery(BOOL ShadeIt)
{
	// Shade any non-standard buttons that we have on the gallery
	EnableGadget(_R(IDC_BMPGAL_SAVE),	!ShadeIt);

	EnableGadget(_R(IDC_BMPGAL_PROPS),	!ShadeIt);
	
	EnableGadget(_R(IDC_BMPGAL_BACKGROUND),	!ShadeIt);
#ifdef PHOTOSHOPPLUGINS
// Only add in if required - general plug-in removal at present
	EnableGadget(_R(IDC_BMPGAL_PLUGINS),	!ShadeIt);
#endif
//#if _DEBUG
	EnableGadget(_R(IDC_BMPGAL_PREVIEW),	!ShadeIt);
//#endif
	EnableGadget(_R(IDC_BMPGAL_CREATE),	!ShadeIt);

	EnableGadget(_R(IDC_BMPGAL_FILL),	!ShadeIt);
	EnableGadget(_R(IDC_BMPGAL_TEXTURE),	!ShadeIt);
	EnableGadget(_R(IDC_BMPGAL_TRACE),	!ShadeIt);
	EnableGadget(_R(IDC_BMPGAL_XPE_EDIT),	!ShadeIt);
	//EnableGadget(_R(IDC_BMPGAL_EFFECTS),	!ShadeIt);
}

/********************************************************************************************

>	virtual MsgResult BitmapSGallery::Message(Msg* Message)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/1/95 (base generated in sgbase.cpp)
	Inputs:		Message - The message to handle

	Purpose:	A standard message handler, really.

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

MsgResult BitmapSGallery::Message(Msg* Message)
{
	if (IS_OUR_DIALOG_MSG(Message))
	{
		DialogMsg* Msg = (DialogMsg*)Message;
		KernelBitmap* SelectedBitmap;

		switch (Msg->DlgMsg)
		{
			case DIM_CREATE:
				SGInit::UpdateGalleryButton(_R(OPTOKEN_DISPLAYBITMAPGALLERY), TRUE);
				SetSelectionFromDocument(TRUE);
				break;

			case DIM_CANCEL:
				SGInit::UpdateGalleryButton(_R(OPTOKEN_DISPLAYBITMAPGALLERY), FALSE);
				break;

			case DIM_LFT_BN_CLICKED:
				if (FALSE) {}
				else if (Msg->GadgetID == _R(IDC_BMPGAL_CREATE))
				{	
					SelectedBitmap = GetSelectedBitmap();

					if (SelectedBitmap != NULL)
					{
						PageDropInfo DropInfo;
						DropInfo.pDocView 	= DocView::GetSelected();
						DropInfo.pDoc 		= Document::GetSelected();
						DropInfo.pSpread 	= Document::GetSelectedSpread();

						FindCentreInsertionPosition(&DropInfo.pSpread, &DropInfo.DropPos);

						// Obtain a pointer to the op descriptor for the create operation 
						OpDescriptor* OpDesc = OpDescriptor::FindOpDescriptor(CC_RUNTIME_CLASS(OpCreateNodeBitmap));

						// Invoke the operation, passing DocView and Pos as parameters
						OpParam param((void *)SelectedBitmap,(void *)&DropInfo);
						OpDesc->Invoke(&param);		 
					}
				}
				else if (Msg->GadgetID == _R(IDC_BMPGAL_FILL))
				{	
					SelectedBitmap = GetSelectedBitmap();

					if (SelectedBitmap != NULL)
					{
						// Fill colour selected	so create a fill colour attribute
						NodeAttribute *Attrib = new AttrBitmapColourFill;
						if (Attrib == NULL)
							return(SuperGallery::Message(Message));

						((AttrBitmapColourFill *)Attrib)->AttachBitmap(SelectedBitmap);

						// AttributeSelected knows what to do with a selected attribute
						AttributeManager::AttributeSelected(NULL, Attrib);
					}
				}
				else if (Msg->GadgetID == _R(IDC_BMPGAL_TEXTURE))
				{	
					SelectedBitmap = GetSelectedBitmap();

					if (SelectedBitmap != NULL)
					{
						SelectedBitmap = CheckTextureBitmap(SelectedBitmap);

						AttributeManager::HaveAskedAboutContoneColours = FALSE;
						AttributeManager::UserCancelledContoneColours = FALSE;

						if (SelectedBitmap == NULL)
							return(SuperGallery::Message(Message));

						// Fill colour selected	so create a fill colour attribute
						NodeAttribute *Attrib = new AttrBitmapTranspFill;
						if (Attrib == NULL)
							return(SuperGallery::Message(Message));

						((AttrBitmapTranspFill *)Attrib)->AttachBitmap(SelectedBitmap);

						// AttributeSelected knows what to do with a selected attribute
						AttributeManager::AttributeSelected(NULL, Attrib);
					}
				}
PORTNOTE("other", "Remove XPE hooks")
#ifndef EXCLUDE_FROM_XARALX
				else if (Msg->GadgetID == _R(IDC_BMPGAL_XPE_EDIT))
				{	
					SelectedBitmap = GetSelectedBitmap();
					if (SelectedBitmap != NULL)
					{
						// Obtain a pointer to the op descriptor for the create operation 
						OpDescriptor* OpDesc = OpDescriptor::FindOpDescriptor(CC_RUNTIME_CLASS(XPEEditItemOp));

						// Invoke the operation, passing DocView and Pos as parameters
						OpParam param((void *)SelectedBitmap, NULL);
						OpDesc->Invoke(&param);
					}
				}
#endif
PORTNOTE("other", "Disabled tracer")
#ifndef EXCLUDE_FROM_XARALX
				else if (Msg->GadgetID == _R(IDC_BMPGAL_TRACE))
				{	
					SelectedBitmap = GetSelectedBitmap();
				// WEBSTER-Martin-09/01/97 we don't trace stuff
				#ifndef WEBSTER
					if (SelectedBitmap != NULL)
					{
						TraceMsg::OpenOrUse(SelectedBitmap);
					}
				#endif //webster
				}
#endif
				/* else if (Msg->GadgetID == _R(IDC_BMPGAL_EFFECTS))
				{	
					SelectedBitmap = GetSelectedBitmap();
				// WEBSTER-Martin-09/01/97
				#ifndef WEBSTER
					if (SelectedBitmap != NULL)
					{
						BfxMsg::OpenOrUse(SelectedBitmap);
					}
				#endif //webster
					break;
				} */
				else if (Msg->GadgetID == _R(IDC_BMPGAL_SAVE))
				{	
					// Generate a list of the selected bitmaps
					UINT32 ListSize = 0;
					KernelBitmap** pList = NULL;
					if (!GetSelectedBitmaps(&pList, &ListSize))
						InformError();
					else
					{
						if (ListSize != 0)
						{
							BmpDlgParam BmpDlgParam;

							GetBmpInfo(&BmpDlgParam);
							// Save the Bitmap deatils for use in the bitmap export code.#
							BmpDlgParam.SetBitmapList(pList);
							BmpDlgParam.SetListSize(ListSize);
							
							BitmapExportParam ExportList(ListSize, pList, &BmpDlgParam);
							// Invoke the export operation
							OpDescriptor* OpDesc = OpDescriptor::FindOpDescriptor(CC_RUNTIME_CLASS(OpMenuExport));
							OpDesc->Invoke(&ExportList);		 
							SetBmpInfo(&BmpDlgParam);
							ForceRedrawOfList();
						}
						else
							ERROR3("No bitmaps were selected - how come the option wasn't greyed");
					}
					if (pList != NULL)
						CCFree(pList);
				}
				else if (Msg->GadgetID == _R(IDC_BMPGAL_HELP))
				{	
					// HelpUserTopic(_R(IDH_Gallery_Bitmap));
					HelpUserTopic(_R(IDS_HELPPATH_Gallery_Bitmap));
					break;
				}
PORTNOTE("other", "Removed preview dialog")
#ifndef EXCLUDE_FROM_XARALX
				else if (Msg->GadgetID == _R(IDC_BMPGAL_PREVIEW))
				{
					// Generate a list of the selected bitmaps
					UINT32 ListSize = 0;
					KernelBitmap** pList = NULL;
					if (!GetSelectedBitmaps(&pList, &ListSize))
						InformError();
					else
					{
						if (ListSize != 0)
						{
							BmpDlgParam BmpDlgParam;
							GetBmpInfo(&BmpDlgParam);
							// Save the Bitmap deatils for use in the bitmap export code.#
							BmpDlgParam.SetBitmapList(pList);
							BmpDlgParam.SetListSize(ListSize);
							BitmapExportParam ExportList(ListSize, pList, &BmpDlgParam);

							// Invoke the preview operation
															// Check if there is a Preview Dialog present.
							PreviewDialog* pPreviewDialog = PreviewDialog::GetPreviewDialog();
							if (!pPreviewDialog)
							{
								OpDescriptor* OpDesc;
								//if (KeyPress::IsAdjustPressed())
								//	OpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_BITMAPPREVIEWDIALOG);
								//else
								OpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_PREVIEWDIALOG);

								if (OpDesc)
								{
									PreviewDialog::ToggleViaBitmapGallery (TRUE);
									OpDesc->Invoke(&ExportList);
									pPreviewDialog->SetDelay (100);
									pPreviewDialog = PreviewDialog::GetPreviewDialog();
								//	pPreviewDialog->ToggleViaBitmapGallery (TRUE);
								}
							}
							else
							{
								// If the animation is playing, stop the animation before we pass
								// it our new list of bitmaps.
								if(pPreviewDialog->GetPlayAnimation())
									pPreviewDialog->SetPlayAnimation(FALSE);

								PreviewDialog::ToggleViaBitmapGallery (TRUE);

								pPreviewDialog->SetBitmapList(&ExportList);
							//	pPreviewDialog->ToggleViaBitmapGallery (TRUE);
								pPreviewDialog->SetDelay (100);

								// Call the dialog box so that it sets up the necessary states
								pPreviewDialog->ReInitDialog();
							}

							SetBmpInfo(&BmpDlgParam);
							ForceRedrawOfList();
						}
						else
							ERROR3("No bitmaps were selected - how come the option wasn't greyed");
					}
					if (pList != NULL)
						CCFree(pList);
				}
#endif
#ifdef PHOTOSHOPPLUGINS
// Only add in if required - general plug-in removal at present
				else if (Msg->GadgetID == _R(IDC_BMPGAL_PLUGINS))
				{	
					// Need to say which document and more importantly which bitmap we
					// want the menu to apply to.
					SelectedBitmap = GetSelectedBitmap();
					// Now find out what the selected group is, which will be the document group
					SGDisplayNode * FirstSelected = DisplayTree->FindNextSelectedItem(NULL);
					Document *pDocument	= NULL;
					if (FirstSelected == NULL)
						pDocument = Document::GetSelected();
					else
						pDocument = ((SGDisplayGroup *) FirstSelected->GetParent())->GetParentDocument();
					ERROR3IF(pDocument == NULL, "No parent document?!");
					if (SelectedBitmap != NULL && pDocument != NULL)
					{
						// replaced menu with direct invocation of Bfx:Special Effects operation since this was 
						// the only option left on menu - all the other plug-in options have been superseded
						// by the new Live Effects implementation. Fixes #11376.

						OpDescriptor *pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_BFX_SPECIALEFFECTS);
						if(pOpDesc)
						{
							// Inform the Bfx operation what bitmap and document it will be working on
							BfxPlugInOp::SetBitmapAndDocument(SelectedBitmap, pDocument);
							// and invoke it immediately
							pOpDesc->Invoke();
						}
						else
						{
							ERROR2RAW("Unable to find BFX:Special Effects operation");
						}
					}
				}
#endif
				else if (Msg->GadgetID == _R(IDC_BMPGAL_BACKGROUND))
				{	
					// Need to say which document and more importantly which bitmap we
					// want the menu to apply to.
					SelectedBitmap = GetSelectedBitmap();
					// Now find out what the selected group is, which will be the document group
					SGDisplayNode * FirstSelected = DisplayTree->FindNextSelectedItem(NULL);
					Document *pDocument	= NULL;
					if (FirstSelected == NULL)
						pDocument = Document::GetSelected();
					else
						pDocument = ((SGDisplayGroup *) FirstSelected->GetParent())->GetParentDocument();
					ERROR3IF(pDocument == NULL, "No parent document?!");
					if (SelectedBitmap != NULL && pDocument != NULL)
					{
						// Apply the bitmap as the new background
						OpBackgroundParam Param;
						Param.pBitmap = SelectedBitmap;
						Param.pDoc = pDocument;			
						
						// Obtain a pointer to the op descriptor for the create operation 
						OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_BACKGROUND);

						// Invoke the operation, passing in our parameters
						pOpDesc->Invoke(&Param);		 
					}
				}
				else if (Msg->GadgetID == _R(IDC_BMPGAL_PROPS))
				{	
					// Generate a list of the selected bitmaps
					UINT32 ListSize = 0;
					KernelBitmap** pList = NULL;
					if (!GetSelectedBitmaps(&pList, &ListSize))
						InformError();
					else
					{
						if (ListSize != 0)
						{
							//  Opens the Bitmap Properties Dialog, from the Bitmap Gallery.
							BmpDlgParam BmpDlgParam;
							GetBmpInfo(&BmpDlgParam);
							BmpDlg::InvokeBmpDlg(&BmpDlgParam);
							SetBmpInfo(&BmpDlgParam);
							ForceRedrawOfList();
						}
						else
							ERROR3("No bitmaps were selected - how come the option wasn't greyed");
					}
					if (pList != NULL)
						CCFree(pList);
				}
				break;
			default:
				break;
		}

		return(SuperGallery::Message(Message));
	}

	// If we have no displaytree, then we have not been shown, or something terrible has
	// happened, so we don't bother handling any of these messages.
	if (DisplayTree == NULL || !IsVisible())
		return(SuperGallery::Message(Message));

	if (IsVisible() && MESSAGE_IS_A(Message, DocChangingMsg))
	{
		DocChangingMsg *TheMsg = (DocChangingMsg *) Message;

		switch ( TheMsg->State )
		{
			case DocChangingMsg::BORN:						// New document - add to display tree
				{
					INT32 Extent = GetDisplayExtent();		// Remember previous list extent

					CreateNewSubtree(TheMsg->pChangingDoc);	// Add a subtree for this doc
					ShadeGallery(FALSE);					// Unshade the gallery

					InvalidateCachedFormat();				// And redraw what is necessary
					RedrawEverythingBelow(-Extent);
				}
				break;		// Pass this message on to the base class as well
			default:
				break;
		}
	}

	if (MESSAGE_IS_A(Message, BitmapListChangedMsg) && !IgnoreBmpListChange)
	{
		BitmapListChangedMsg *TheMsg = (BitmapListChangedMsg *) Message;

		// First, try to find the document which owns the changed list
		Document *ScopeDoc = (Document *)TheMsg->pChangedBmpList->GetParentDocument();

		// Just check that the bi-directional link is sane!
		ERROR3IF(ScopeDoc == NULL, "A Document bitmap list is invalid");

		// If we found it, recreate the display subtree for it
		if (ScopeDoc != NULL)
		{
			SGDisplayNode *Ptr = DisplayTree->FindSubtree(this, ScopeDoc, NULL);

			if (Ptr != NULL)
			{
				CreateNewSubtree(ScopeDoc, (SGDisplayGroup *) Ptr);

				// And force a redraw of the entire list
				ForceRedrawOfList();
				
				// The above will loose the selection, so we can listen out for the special
				// form of the message which says which bitmap to reselect.
				if ((TheMsg->State == BitmapListChangedMsg::SELECTNEWBITMAP) &&
					(TheMsg->pNewBitmap != NULL))
				{
					// We have been sent a message that a new bitmap has been added and
					// this is the one we should go and select.
					SelectItems(FALSE);					// Clear the current selection

					SGDisplayGroup *DocGroup = DisplayTree->FindSubtree(this, ScopeDoc, NULL);
					SGDisplayKernelBitmap* pCurrentItem = NULL;
					if (DocGroup != NULL)
						pCurrentItem = (SGDisplayKernelBitmap *) DocGroup->GetChild();
						//Selected = (SGDisplayKernelBitmap *) DocGroup->FindNextSelectedItem(NULL);

					// Ensure the tree format information is up to date, ready for when we sample it
					ReformatNow(TRUE);
					BOOL HaveSelected = FALSE;
					DocRect ScrollToRect;

					while (pCurrentItem != NULL)
					{
						KernelBitmap* CurrentBitmap = pCurrentItem->GetDisplayedKernelBitmap();
						if (CurrentBitmap == TheMsg->pNewBitmap)
						{
							pCurrentItem->SetSelected(TRUE);
							HaveSelected = TRUE;
							pCurrentItem->GetFormatRect(&ScrollToRect);
							break;
						}

						// Get the next one in the list
						pCurrentItem = (SGDisplayKernelBitmap *) pCurrentItem->GetNext();
					}

						
					// Try and scroll to the selected item, if we have one
					if (HaveSelected)
					{
						BOOL ScrollToBottom = TRUE;
						ScrollToShow(&ScrollToRect, ScrollToBottom);
					}
				}
			
				// Was not present before 20/12/96, safest to have it present as this action
				// will lose the selection.
				SelectionHasChanged();
			}
		}
	}

	if (MESSAGE_IS_A(Message, CommonAttrsChangedMsg))
	{
		if (!AmShaded)			// If we're open & active, set listbox selection
			SetSelectionFromDocument();
	}

	return(SuperGallery::Message(Message));
}    


/********************************************************************************************

>	virtual void BitmapSGallery::HandleDragStart(DragMessage *DragMsg)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/3/95

	Inputs:		DragMsg - The DRAGSTARTED message that we've just recieved, indicating
				the type of drag being started

	Purpose:	Checks a DragMessage to see if it is a bitmap drag.
				If it is, then it creates a drag target for this gallerys listbox.

	Notes:		Overrides the default base-class action. Calls down to the base class
				if it is not a colour drag, so that dragging of gallery groups is allowed

********************************************************************************************/

void BitmapSGallery::HandleDragStart(DragMessage *DragMsg)
{
PORTNOTE("other", "Disable GalleryBitmapDragInfo")
#ifndef EXCLUDE_FROM_XARALX
	// If it's a bitmap drag, add a target for our window. If not, let the base class
	// have a look at it (to see if it is a gallery item being dragged)
	if (DragMsg->pInfo->IsKindOf(CC_RUNTIME_CLASS(GalleryBitmapDragInfo)))
		/* SGBitmapDragTarget *NewTarget =*/ new SGBitmapDragTarget(this, GetListGadgetID());
	else
#endif
		SuperGallery::HandleDragStart(DragMsg);
}



/********************************************************************************************

>	BOOL BitmapSGallery::FindCentreInsertionPosition(Spread** Spread, DocCoord* Position)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/7/94
	Inputs:		-
	Outputs:	Spread:  The spread to place the clipboard objects on
				Position:The centre of the view (Spread coords)
	Returns:	-
	Purpose:	Finds the centre insertion position for clipboard objects
	Errors:		-
	Scope:		private
	SeeAlso:	-

********************************************************************************************/

BOOL BitmapSGallery::FindCentreInsertionPosition(Spread** Spread, DocCoord* Position)
{
	// ---------------------------------------------------------------------------------
	// Find out which spread is in the centre of the view 
	// this is the spread that the pasted objects will be placed on

	// Obtain the current DocView
	DocView* CurDocView = DocView::GetCurrent();

	ENSURE(CurDocView != NULL, "The current DocView is NULL"); 
	if (CurDocView == NULL)
	{
		return FALSE; // No DocView
	}

	// Get the view rect
	WorkRect WrkViewRect = CurDocView->GetViewRect();

	if (WrkViewRect.IsEmpty() || (!WrkViewRect.IsValid()) )
	{
		return FALSE; // Defensive
	}
	
	// Determine the centre of the view
	WorkCoord WrkCentreOfView; 
	WrkCentreOfView.x = WrkViewRect.lo.x	+ (WrkViewRect.Width()/2); 
	WrkCentreOfView.y = WrkViewRect.lo.y	+ (WrkViewRect.Height()/2);
	
	// FindEnclosing spread requires an OilCoord
	OilCoord OilCentreOfView = WrkCentreOfView.ToOil(CurDocView->GetScrollOffsets()); 

	// Find out which spread to insert the pasteboard objects onto
	(*Spread) = CurDocView->FindEnclosingSpread(OilCentreOfView);
	if ((*Spread) == NULL)
	{
		// There is no spread
		return FALSE; 
	}

	// Phew
	// ---------------------------------------------------------------------------------
	// Now lets find the spread coordinate of the centre of the view
	DocRect DocViewRect = CurDocView->GetDocViewRect(*Spread);
	
	if ( DocViewRect.IsEmpty() || (!DocViewRect.IsValid()) )
	{
		return FALSE; // Defensive
	}

	ENSURE( ( (!DocViewRect.IsEmpty()) && DocViewRect.IsValid()), 
			"DocViewRect is invalid" );

	// Find the centre of the DocViewRect
   	DocCoord DocCentreOfView; 
	DocCentreOfView.x = DocViewRect.lo.x	+ (DocViewRect.Width()/2); 
	DocCentreOfView.y = DocViewRect.lo.y	+ (DocViewRect.Height()/2);

	// Now convert from DocCoords to spread coords
	(*Spread)->DocCoordToSpreadCoord(&DocCentreOfView);

	// Finally, fill in the return value
	*Position = DocCentreOfView;
	
	return TRUE;  
}


/********************************************************************************************

>	virtual RenderRegion *BitmapSGallery::CreateRenderRegion(DocRect *VirtualSize,
															ReDrawInfoType *DlgRedrawInfo)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/3/95

	Purpose:	An overridable veneer to the DialogOp CreateOSRenderRegion method.
				This can be overriden to use a different type of renderregion for
				all of your rendering.

				If you override this, you MUST also override DestroyRenderRegion!

	SeeAlso:	SuperGallery::DestroyRenderRegion

********************************************************************************************/

RenderRegion *BitmapSGallery::CreateRenderRegion(DocRect *VirtualSize, ReDrawInfoType *DlgRedrawInfo)
{
	return(CreateOSRenderRegion(VirtualSize, DlgRedrawInfo));
}



/********************************************************************************************

>	virtual void BitmapSGallery::DestroyRenderRegion(RenderRegion *pRender)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/3/95

	Purpose:	An overridable veneer to the DialogOp DestroyOSRenderRegion method.
				This can be overriden to use a different type of renderregion for
				all of your rendering.

				If you override this, you MUST also override CreateRenderRegion!

	SeeAlso:	SuperGallery::CreateRenderRegion

********************************************************************************************/

void BitmapSGallery::DestroyRenderRegion(RenderRegion *pRender)
{
	DestroyOSRenderRegion(pRender);
}


/********************************************************************************************

>	virtual BOOL BitmapSGallery::PreContextMenu(void)
												 
	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/10/97
	Returns:	TRUE for success
	Purpose:	Give the gallery a chance to do something before popping up the context menu.
				Called before the context menu is popped up on a right click.
				At present, used by the bitmap gallery to ensure that the plug-ins have all
				been parsed.

********************************************************************************************/

BOOL BitmapSGallery::PreContextMenu(void)
{
#ifdef PHOTOSHOPPLUGINS
// Only add in if required - general plug-in removal at present
	// As we are just about to invoke UI, check that the plug-in manager has its list
	// of available plug-ins otherwise, go and get it now as we are just about to need it.
	// Must do it now before even the hint of a context menu is started as the context
	// menus stack the automatic menu id and then restore it after they are destroyed.
	// This is bad if our main menu ids are in this range!
	PlugInManager* pManager = GetApplication()->GetPlugInManager();
	if (pManager)
		pManager->CheckHaveDetailsOnPlugIns();
#endif // PHOTOSHOPPLUGINS

	return(TRUE);
}

/********************************************************************************************

>	virtual BOOL BitmapSGallery::InitMenuCommands(void)
												 
	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/9/95

	Returns:	TRUE for success

	Purpose:	Initialises any menu commands that this gallery needs.

	Notes:		Will only create the menu commands once - further calls in the future
				will return TRUE immediately without doing anything.

********************************************************************************************/

BOOL BitmapSGallery::InitMenuCommands(void)
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
		ok = ok && InitMenuCommand((StringBase *) &SGCmd_Insert, _R(IDS_SGMENU_INSERT));
		ok = ok && InitMenuCommand((StringBase *) &SGCmd_Fill, _R(IDS_SGMENU_FILL));
		ok = ok && InitMenuCommand((StringBase *) &SGCmd_Transp, _R(IDS_SGMENU_TRANSP));
		ok = ok && InitMenuCommand((StringBase *) &SGCmd_XPE_Edit, _R(IDS_SGMENU_XPE_EDIT));
		ok = ok && InitMenuCommand((StringBase *) &SGCmd_Delete, _R(IDS_SGMENU_DELETE));
		ok = ok && InitMenuCommand((StringBase *) &SGCmd_Save, _R(IDS_SGMENU_SAVE));
//#if _DEBUG
		ok = ok && InitMenuCommand((StringBase *) &SGCmd_Preview, _R(IDS_SGMENU_PREVIEW));
//#endif
//	WEBSTER-ranbirr-22/11/96
#ifndef WEBSTER
		ok = ok && InitMenuCommand((StringBase *) &SGCmd_Trace, _R(IDS_SGMENU_TRACE));
		ok = ok && InitMenuCommand((StringBase *) &SGCmd_Effects, _R(IDS_SGMENU_EFFECTS));
#endif //webster
#ifdef PHOTOSHOPPLUGINS
// Only add in if required - general plug-in removal at present
		ok = ok && InitMenuCommand((StringBase *) &SGCmd_PlugIns, _R(IDS_SGMENU_PLUGINS));
#endif
		ok = ok && InitMenuCommand((StringBase *) &SGCmd_SetBackground, _R(IDS_SGMENU_SETBACKGROUND));

		//	WEBSTER-ranbirr-22/11/96
		ok = ok && InitMenuCommand((StringBase *) &SGCmd_Props, _R(IDS_SGMENU_PROPS));

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

>	virtual BOOL BitmapSGallery::BuildCommandMenu(GalleryContextMenu *TheMenu, SGMenuID MenuID)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/9/95

	Inputs:		TheMenu - The menu to add commands to
				MenuID  - The type of menu (over-list or from-options-button) to create

	Returns:	TRUE if it succeeded

	Purpose:	To build a menu of commands to be popped up over the gallery.
	
	Notes:		Override this method to stop the default menus being built

********************************************************************************************/

BOOL BitmapSGallery::BuildCommandMenu(GalleryContextMenu *TheMenu, SGMenuID MenuID)
{
 	BOOL ok = TRUE;

	if (MenuID == SGMENU_OPTIONS)
	{
		// Options menu
		ok = ok && AddCommand(TheMenu, (StringBase *) &SGCmd_Find);
//		ok = ok && AddCommand(TheMenu, (StringBase *) &SGCmd_Sort);
//		ok = ok && AddCommand(TheMenu, (StringBase *) &SGCmd_Properties);
	}
	else
	{
		// Over-list menu
		ok = ok && AddCommand(TheMenu, (StringBase *) &SGCmd_Insert);
		ok = ok && AddCommand(TheMenu, (StringBase *) &SGCmd_Fill);
		ok = ok && AddCommand(TheMenu, (StringBase *) &SGCmd_Transp,TRUE);

		ok = ok && AddCommand(TheMenu, (StringBase *) &SGCmd_Delete);
		ok = ok && AddCommand(TheMenu, (StringBase *) &SGCmd_Save);
//#if _DEBUG
		ok = ok && AddCommand(TheMenu, (StringBase *) &SGCmd_Preview);
//#endif
//	WEBSTER-ranbirr-22/11/96
#ifndef WEBSTER
		ok = ok && AddCommand(TheMenu, (StringBase *) &SGCmd_XPE_Edit);
		ok = ok && AddCommand(TheMenu, (StringBase *) &SGCmd_Trace);
//		ok = ok && AddCommand(TheMenu, (StringBase *) &SGCmd_Effects);		
#endif //webster

#ifdef PHOTOSHOPPLUGINS
// Only add in if required - general plug-in removal at present
		// Need to say which document and more importantly which bitmap we
		// want the menu to apply to.
		KernelBitmap * pSelectedBitmap = GetSelectedBitmap();
		// Now find out what the selected group is, which will be the document group
		SGDisplayNode * FirstSelected = DisplayTree->FindNextSelectedItem(NULL);
		Document *pDocument	= NULL;
		if (FirstSelected == NULL)
			pDocument = Document::GetSelected();
		else
			pDocument = ((SGDisplayGroup *) FirstSelected->GetParent())->GetParentDocument();
		ERROR3IF(pDocument == NULL, "No parent document?!");

		// Add plug-in ("Special Effects" is the only one still used since Live Effects) to the 
		// main menu, not a submenu. Fixes #11376
		// Well, in fact we will ask the PlugInsContextMenu to do it for us!
		ok = ok && PlugInsContextMenu::BuildMenu(pSelectedBitmap, pDocument, TheMenu, NULL, TRUE);

#endif
		//	WEBSTER-ranbirr-22/11/96
		ok = ok && AddCommand(TheMenu, (StringBase *) &SGCmd_Props,TRUE);

		// This is the set background menu option
		ok = ok && AddCommand(TheMenu, (StringBase *) &SGCmd_SetBackground, TRUE);	// With separator

		// Add the fold and unfold group items
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

>	virtual OpState BitmapSGallery::GetCommandState(StringBase *CommandID, String_256 *ShadeReason)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/9/95

	Inputs:		CommandID - TheString ID of the command
	Outputs:	ShadeReason - If you return (OpState.Greyed == TRUE) then this should be filled
				in with the reason that the item is shaded/greyed.

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

OpState BitmapSGallery::GetCommandState(StringBase *CommandID, String_256 *ShadeReason)
{
	OpState State;

	if (*CommandID == SGCmd_Properties || *CommandID == SGCmd_Sort)
	{
		State.Greyed = TRUE;
		ShadeReason->MakeMsg(_R(IDS_SGSHADE_NOTSUPPORTED));
	}
	else if (*CommandID == SGCmd_Delete)
	{
		INT32 Count = GetSelectedItemCount();
		if (Count == 0)
		{
			State.Greyed = TRUE;
			ShadeReason->MakeMsg(_R(IDS_SGSHADE_NOSEL));
		}
		else if (Count == 1 && GetSelectedBitmap()->ActualBitmap == OILBitmap::Default)
		{
			State.Greyed = TRUE;
			ShadeReason->MakeMsg(_R(IDS_SGSHADE_NODELBITMAP));
		}
	}
		else if (*CommandID == SGCmd_Insert	|| *CommandID == SGCmd_Fill
//	WEBSTER-ranbirr-22/11/96
#ifndef WEBSTER
				 || *CommandID == SGCmd_Transp
				 || *CommandID == SGCmd_Trace || *CommandID == SGCmd_XPE_Edit || *CommandID == SGCmd_Effects
				 || *CommandID == SGCmd_SetBackground
#endif //Webster
#ifdef PHOTOSHOPPLUGINS
// Only add in if required - general plug-in removal at present
				 || *CommandID == SGCmd_PlugIns
#endif
				 )
	{
		if (GetSelectedItemCount() != 1)
		{
			State.Greyed = TRUE;
			ShadeReason->MakeMsg(_R(IDS_SGSHADE_SINGLE));
		}
	}
	else if (*CommandID == SGCmd_Save)
	{
		if (GetSelectedItemCount() == 0)
		{
			State.Greyed = TRUE;
			ShadeReason->MakeMsg(_R(IDS_SGSHADE_NOSEL));
		}
	}
	else if (*CommandID == SGCmd_Preview)
	{
		if (GetSelectedItemCount() == 0)
		{
			State.Greyed = TRUE;
			ShadeReason->MakeMsg(_R(IDS_SGSHADE_NOSEL));
		}
	}
	else if (*CommandID == SGCmd_Props)
	{
		if (GetSelectedItemCount() == 0)
		{
			State.Greyed = TRUE;
			ShadeReason->MakeMsg(_R(IDS_SGSHADE_NOSEL));
		}
	}
	else
		return(SuperGallery::GetCommandState(CommandID, ShadeReason));		// Unknown command- pass to baseclass

	return(State);
}



/********************************************************************************************

>	virtual void BitmapSGallery::DoCommand(StringBase *CommandID)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/9/95

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

void BitmapSGallery::DoCommand(StringBase *CommandID)
{
	if (*CommandID == SGCmd_Insert)
	{
		// Fake a button click message to ourself
		DialogMsg Bob(WindowID, DIM_LFT_BN_CLICKED, _R(IDC_BMPGAL_CREATE));
		Message(&Bob);
	}

	else if (*CommandID == SGCmd_Fill)
	{
		// Fake a button click message to ourself
		DialogMsg Bob(WindowID, DIM_LFT_BN_CLICKED, _R(IDC_BMPGAL_FILL));
		Message(&Bob);
	}
//	WEBSTER-ranbirr-22/11/96
#ifndef WEBSTER
		else if (*CommandID == SGCmd_Transp)
	{
		// Fake a button click message to ourself
		DialogMsg Bob(WindowID, DIM_LFT_BN_CLICKED, _R(IDC_BMPGAL_TEXTURE));
		Message(&Bob);
	}
#endif //webster
	else if (*CommandID == SGCmd_Save)
	{
		// Fake a button click message to ourself
		DialogMsg Bob(WindowID, DIM_LFT_BN_CLICKED, _R(IDC_BMPGAL_SAVE));
		Message(&Bob);
	}
	else if (*CommandID == SGCmd_Preview)
	{
		// Fake a button click message to ourself
		DialogMsg Bob(WindowID, DIM_LFT_BN_CLICKED, _R(IDC_BMPGAL_PREVIEW));
		Message(&Bob);
	}
	else if (*CommandID == SGCmd_XPE_Edit)
	{
		// Fake a button click message to ourself
		DialogMsg Bob(WindowID, DIM_LFT_BN_CLICKED, _R(IDC_BMPGAL_XPE_EDIT));
		Message(&Bob);
	}
//	WEBSTER-ranbirr-22/11/96
#ifndef WEBSTER
	else if (*CommandID == SGCmd_Trace)
	{
		// Fake a button click message to ourself
		DialogMsg Bob(WindowID, DIM_LFT_BN_CLICKED, _R(IDC_BMPGAL_TRACE));
		Message(&Bob);
	}
/*	else if (*CommandID == SGCmd_Effects)
	{
		// Fake a button click message to ourself
		DialogMsg Bob(WindowID, DIM_LFT_BN_CLICKED, _R(IDC_BMPGAL_EFFECTS));
		Message(&Bob);
	} */
#endif //webster
#ifdef PHOTOSHOPPLUGINS
// Only add in if required - general plug-in removal at present
	else if (*CommandID == SGCmd_PlugIns)
	{
		// Fake a button click message to ourself
		DialogMsg Bob(WindowID, DIM_LFT_BN_CLICKED, _R(IDC_BMPGAL_PLUGINS));
		Message(&Bob);
	}
#endif
	else if (*CommandID == SGCmd_SetBackground)
	{
		// Fake a button click message to ourself
		DialogMsg Bob(WindowID, DIM_LFT_BN_CLICKED, _R(IDC_BMPGAL_BACKGROUND));
		Message(&Bob);
	}

	// WEBSTER-ranbirr-22/11/96
	else if (*CommandID == SGCmd_Props)
	{
		// Fake a button click message to ourself
		DialogMsg Bob(WindowID, DIM_LFT_BN_CLICKED, _R(IDC_BMPGAL_PROPS));
		Message(&Bob);
	}
	else
		SuperGallery::DoCommand(CommandID);		// Unknown command- pass to the base class
}







//-----------------------------------------------------------------------------------------------

// OpDisplayBitmapGallery - the operation that is used to display the bitmap gallery

/********************************************************************************************

>	BOOL OpDisplayBitmapGallery::Init()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/1/95
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the operation could be successfully initialised 
				FALSE if no more memory could be allocated 
				
	Purpose:	OpDisplayBitmapGallery initialiser method
	Errors:		ERROR will be called if there was insufficient memory to allocate the 
				operation.
	SeeAlso:	-

********************************************************************************************/

BOOL OpDisplayBitmapGallery::Init()
{
	return (
			RegisterOpDescriptor(
	 							0,
	 							_R(IDS_DISPLAY_BITMAP_GALLERY),
								CC_RUNTIME_CLASS(OpDisplayBitmapGallery),
	 							OPTOKEN_DISPLAYBITMAPGALLERY,
	 							OpDisplayBitmapGallery::GetState,
	 							0,	/* help ID */
	 							_R(IDBBL_DISPLAY_BITMAP_GALLERY),
				 				_R(IDC_BTN_SGBITMAP), // UINT32 resourceID = 0,	// resource ID
								_R(IDC_BTN_SGBITMAP), // UINT32 controlID = 0,	// control ID
								SYSTEMBAR_ILLEGAL,	  // SystemBarType GroupBarID = SYSTEMBAR_ILLEGAL,	// group bar ID
				 				TRUE,	  // BOOL ReceiveMessages = TRUE,	// BODGE
				 				FALSE,	  // BOOL Smart = FALSE,
				 				TRUE,	  // BOOL Clean = TRUE,   
								NULL,	  // OpDescriptor *pVertOpDesc = NULL,
								0,	  // UINT32 OneOpenInstID = 0,		
								0,	  // UINT32 AutoStateFlags = 0,
								TRUE	  // BOOL fCheckable = FALSE
								)
			
PORTNOTE("other", "Removed Preview Dialog")
#ifndef EXCLUDE_FROM_XARALX
//#if _DEBUG
			&& BitmapExportPreviewDialog::Init()
//#endif
			&& PreviewDialog::Init()
#endif
			);
}               
    
/********************************************************************************************

>	OpState	OpDisplayBitmapGallery::GetState(String_256*, OpDescriptor*)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/1/95
	Inputs:		-
	Outputs:	-
	Returns:	The state of the OpDisplayBitmapGallery operation
	Purpose:	For finding the OpDisplayBitmapGallery's state. 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

OpState	OpDisplayBitmapGallery::GetState(String_256* UIDescription, OpDescriptor*)
{
	OpState OpSt;

	// If the gallery is currenty open, then the menu item should be ticked
	SuperGallery* pSuperGallery = SuperGallery::FindSuperGallery(_R(IDD_BITMAPSGALLERY));

	if (pSuperGallery != NULL)
	{
		if (pSuperGallery->GetRuntimeClass() == CC_RUNTIME_CLASS(BitmapSGallery))
			OpSt.Ticked = pSuperGallery->IsVisible();
	}

	// If there are no open documents, you can't toggle the gallery
	OpSt.Greyed = (Document::GetSelected() == NULL);

 	return(OpSt);
}

/********************************************************************************************

>	void OpDisplayBitmapGallery::Do(OpDescriptor*)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/1/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Displays the bitmap gallery
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void OpDisplayBitmapGallery::Do(OpDescriptor*)
{
	SuperGallery* pSuperGallery = SuperGallery::FindSuperGallery(_R(IDD_BITMAPSGALLERY));

	if (!pSuperGallery)
		pSuperGallery = new BitmapSGallery();

	if (pSuperGallery != NULL)
	{
		if (pSuperGallery->GetRuntimeClass() == CC_RUNTIME_CLASS(BitmapSGallery))
		{
			// Toggle the visibility of the gallery window
			pSuperGallery->SetVisibility( !pSuperGallery->IsVisible() );

			// And update the gallery button state
			SGInit::UpdateGalleryButton(_R(OPTOKEN_DISPLAYBITMAPGALLERY),
										pSuperGallery->IsVisible());
		}
#if _DEBUG
		else
			ERROR3("The Bitmap Gallery isn't a BitmapSGallery! Woss goin' on?");
#endif
	}

	ERROR3IF(pSuperGallery == NULL,"Couldn't find the bitmap gallery bar");

	End();
}



/********************************************************************************************
>	BOOL BitmapSGallery::GetSelectedBitmaps(KernelBitmap** pOutputArray, UINT32* pOutputCount)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/6/96
	Inputs:		see Outputs
	Outputs:	pOutputArray - list of selected KernelBitmaps
				pOutputCount - the length of the list
	Returns:	TRUE/FALSE for success/error
	Purpose:	Generates a list of all the selected bitmaps in the gallery.  Note that the
				generated array becomes the callers responsibility if TRUE is returned; the
				caller must CCFree it.
	SeeAlso:	BitmapSGallery::GetSelectedBitmap
********************************************************************************************/
BOOL BitmapSGallery::GetSelectedBitmaps(KernelBitmap*** pOutputArray, UINT32* pOutputCount)
{
	// Init vars
	ERROR2IF(pOutputArray==NULL || pOutputCount==NULL, FALSE, "NULL output param");
	KernelBitmap** pBuildingArray = NULL;
//	UINT32 BuildingCount = 0;
	
	// Lets see if any of our items are selected
	Document* ParentDoc = (Document*) GetApplication()->Documents.GetHead();
	BOOL ok = TRUE;
	while (ParentDoc!=NULL && ok)
	{
		SGDisplayGroup *DocGroup = DisplayTree->FindSubtree(this, ParentDoc, NULL);
		if (DocGroup != NULL)
		{
			// Note the selected items in this group
			SGDisplayKernelBitmap* pCurrent = (SGDisplayKernelBitmap*)DocGroup->FindNextSelectedItem(NULL);
			while (pCurrent!=NULL && ok)
			{
				if (pBuildingArray==NULL)
				{
					// Claim selected space
					pBuildingArray = (KernelBitmap**) CCMalloc(sizeof(KernelBitmap*));
					if (pBuildingArray == NULL)
						ok = FALSE;
					else
						pBuildingArray[0] = pCurrent->GetDisplayedKernelBitmap();
					*pOutputCount = 1;
				}
				else
				{
					// Add to end of selection
					KernelBitmap** pTemp = (KernelBitmap**) CCRealloc(pBuildingArray, ((*pOutputCount)+1)*sizeof(KernelBitmap*));
					if (pTemp == NULL)
						ok = FALSE;
					else
					{
						pBuildingArray = pTemp;
						pBuildingArray[*pOutputCount] = pCurrent->GetDisplayedKernelBitmap();
					}
					(*pOutputCount)++;
				}
	
				pCurrent = (SGDisplayKernelBitmap*)DocGroup->FindNextSelectedItem(pCurrent);
			}
		}

		ParentDoc = (Document *) GetApplication()->Documents.GetNext(ParentDoc);
	}

	if (!ok)
	{
		CCFree(pBuildingArray);
		*pOutputArray = NULL;
		*pOutputCount = 0;
	}
	else
		*pOutputArray = pBuildingArray;

	return ok;
}




/********************************************************************************************

	BOOL BitmapSGallery::GetBmpInfo(BmpDlgPara Param)

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/01/96
	Inputs:		Param - Allows the transfer of data to and from the bitmap poperties dialog.
	Outputs:	-
	Returns:	TRUE if Successful, else FALSE. 
	Purpose:	Retrieves inforamtion from Bitmaps selected in the Bitmaps Gallery.
	SeeAlso:	-

********************************************************************************************/

BOOL BitmapSGallery::GetBmpInfo(BmpDlgParam* Param)
{
	ERROR2IF(Param == NULL,FALSE,"Param is NULL");

	UINT32 ListSize = 0;				
	KernelBitmap** pList = NULL;	
//	BOOL Done = FALSE;
	
	if (!GetSelectedBitmaps(&pList, &ListSize))
		return FALSE;

	//Pass these values to our Param class, for use in the dialog code
	Param->SetBitmapList(pList);
	Param->SetListSize(ListSize);

	if (ListSize>1)
	{
		Param->SetMany(TRUE);
	}
	
		String_256* BitmapCols = new String_256[ListSize];
		String_256* Width = new String_256[ListSize];
		String_256* Height = new String_256[ListSize];
		String_256* BitmapFormat = new String_256[ListSize];
		UINT32* Delay	= new UINT32[ListSize];
		BOOL* Interpolation	= new BOOL[ListSize];
		GIFDisposalMethod* DisposalMethod = new GIFDisposalMethod[ListSize];
		UINT32* TranspType = new UINT32[ListSize];

		if ( !BitmapCols || !Width || !Height ||
			 !BitmapFormat  || !Delay  || !DisposalMethod || !TranspType ) 
			return FALSE;

			UINT32 i=0;					// Loop index    
			UINT32 TempListSize=ListSize; // Keep a record of our list size.
			INT32 Bytes =0;				// Holds the bitmap Memory info.	

		while (ListSize>0)
		{
			ListSize--;

			KernelBitmap* pKernelBitmap = pList[ListSize];
			if (pKernelBitmap == NULL)
				return FALSE;
							
			OILBitmap* pOILBitmap = pKernelBitmap->GetActualBitmap();
			if (pOILBitmap == NULL)
				return FALSE;

PORTNOTE("other", "Removed XPE hooks")
#ifndef EXCLUDE_FROM_XARALX
			IXMLDOMDocumentPtr pEditsList = NULL;
			KernelBitmap* pMaster = NULL;
			pKernelBitmap->GetXPEInfo(pMaster, pEditsList);
			BOOL bIsXPE = (pMaster!=NULL && pEditsList!=NULL);
#else
			BOOL bIsXPE = FALSE;
#endif

			//  Get the bitmap name details.
			Param->SetBitmapName(pOILBitmap->GetName());

			// Is this bitmap a compressed JPEG?
			IsJPEG(pList,ListSize,Param,&Bytes);

			// Set the format field - JPEG\Bitmap.
			if(Param->GetIsJPEG())
			{
				Param->SetBitmapFormat(_R(IDS_FORMAT_JPEG));
			}
			else if (bIsXPE)
			{
				Param->SetBitmapFormat(_R(IDS_FORMAT_XPE));
			}
			else
			{
				Param->SetBitmapFormat(_R(IDS_FORMAT_BITMAP));
			}

			// Get the Delay for the selected bitmap.
			Param->SetAnimDelay(pKernelBitmap->GetDelay());

			// Get the 'smooth when scaled up?' setting
			Param->SetInterpolation(pKernelBitmap->GetInterpolation());

			// Now find how many colours it has?
			INT32 bpp = pKernelBitmap->GetBPP();
			Param->SetBitmapTranspType(_R(IDS_BITMAPTRANSPTYPE_NONE));
			UINT32 ttype = TT_NoTranspType;

			if (bpp <= 8)
			{
				// Check to see if this is a masked bitmap.
				INT32 Index=0;

				if(pKernelBitmap->GetTransparencyIndex(&Index)) 
				{
					Param->SetBitmapTranspType(_R(IDS_BITMAPTRANSPTYPE_MASKED));
					ttype = TT_Mix;
				}
			}
			else if (bpp<32)
			{
				Param->SetBitmapTranspType(_R(IDS_BITMAPTRANSPTYPE_NONE));
			}
			else
			{
				Param->SetBitmapTranspType(_R(IDS_BITMAPTRANSPTYPE_ALPHA));
				ttype = TT_StainGlass;
			}

			switch (bpp)
			{
				case 1:
						Param->SetBitmapCols(_R(IDS_SGBITMAP_MONOCHROME)); 	// "Monochrome";
				break;

				case 2:
						Param->SetBitmapCols(_R(IDS_SGBITMAP_4COLOURS)); 	// "4 colours";
				break;

				case 4:
						Param->SetBitmapCols(_R(IDS_SGBITMAP_16COLOURS)); 	// "16 colours";
				break;

				case 8:
						Param->SetBitmapCols(_R(IDS_SGBITMAP_256COLOURS)); 	// "256 colours";
				break;
	
				case 16:
						Param->SetBitmapCols(_R(IDS_SGBITMAP_65COLOURS)); 		// "65K colours";
				break;

				case 24:
						Param->SetBitmapCols(_R(IDS_SGBITMAP_MILLIONS_COLOURS)); // "Millions of colours";
				break;

				case 32:
						Param->SetBitmapCols(_R(IDS_SGBITMAP_32_BIT)); // "Millions of colours";
				break;

				default:
						Param->SetBitmapCols ("");
					break;
			}
		
			// Get the bitmap Dimensions.
			BitmapInfo Info;

			if(pOILBitmap->GetInfo(&Info))
			{
				// Get its pixel Width and Height.
				Convert::LongToString(Info.PixelWidth, &Param->GetBitmapWidth());
				Convert::LongToString(Info.PixelHeight, &Param->GetBitmapHeight());
			
				// If this bitmap is not have a compressed JPEG, then calculate the memeory used by the bitmap.
				if(!Param->GetIsJPEG() && !bIsXPE)
				{
					// Work out how much memory it is using.
					Bytes += (Info.PixelWidth * Info.PixelHeight * bpp)/8;
				}
			}

			// Get the bitmap Animation Restore details.
			Param->SetRestoreType(pOILBitmap->GetAnimationRestoreType());

			// Keep a record of the details.
			BitmapCols[i]	= Param->GetBitmapCols();
			Width[i]		= Param->GetBitmapWidth();
			Height[i]		= Param->GetBitmapHeight();
			BitmapFormat[i]	= Param->GetBitmapFormat();
			Delay[i]		= Param->GetAnimDelay();
			Interpolation[i]= Param->GetInterpolation();
			DisposalMethod[i] = Param->GetRestoreType();
			TranspType[i]	= ttype;
			i++;
		}	

		// ok, we have no more bitmaps\JPEGS, so convert the memory (Bytes), into a string.
		Convert::BytesToString(&Param->GetMemoryUsed(),(INT32)Bytes);
			
		// Compare the bitmap colors,Dimensions,Transparency and Format fields and set their flags appropriately.
		// i.e. If for any one field, all the bitmaps are not the same, then we output "-", instead of the details.
		// Only do this if more than one bitmap is selected..

		if (TempListSize>1)
		{
			for(i=0; i<TempListSize; i++)
			{
				for(UINT32 e=i+1; e<TempListSize; e++)
				{
					if (BitmapCols[i] != BitmapCols[e])
							Param->SetSameColors(FALSE);
						
					if (Width[i] != Width[e])
							Param->SetSameDimensions(FALSE);
						
					if (Height[i] != Height[e])
							Param->SetSameDimensions(FALSE);

					if (BitmapFormat[i] != BitmapFormat[e])
							Param->SetSameFormat(FALSE);

					if  (DisposalMethod[i] != DisposalMethod[e])
							Param->SetSameRestoreType(FALSE);

					if  (Delay[i] != Delay[e])
							Param->SetAreDelayValuesSame(FALSE);

					if  (Interpolation[i] != Interpolation[e])
							Param->SetAreInterpolationValuesSame(FALSE);

					if (TranspType[i] != TranspType[e])
							Param->SetSameTranspType(FALSE);
				}
			}
		}
							
		if (pList != NULL)
		CCFree(pList);
		delete [] BitmapCols;
		delete [] Width;
		delete [] Height;
		delete [] DisposalMethod;
		delete [] BitmapFormat;
		delete [] Delay;
		delete [] Interpolation;
		delete [] TranspType;

		return TRUE;
}

/*************************************************************************************************

>	BOOL BitmapSGallery::SetBmpInfo(BmpDlgPara* Param)

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/01/96
	Inputs:		Param - Allows the transfer of data to and from the bitmap poperties dialog.   
	Outputs:	-
	Returns:	TRUE if Successful, FALSE otherwise.
	Purpose:	Stores info, retrieved from the Bitmap Properties Dialog, in an OILBitmap.
	SeeAlso:	-

**************************************************************************************************/

BOOL BitmapSGallery::SetBmpInfo(BmpDlgParam* Param)
{
		ERROR2IF(Param == NULL,FALSE,"Param is NULL");

		// The user has entrered OK, so go through the list of selected bitmaps and set their Delay\Restore fields.
		UINT32 ListSize = 0;
		KernelBitmap** pList = NULL;
		Document* WorkDoc = Document::GetSelected();


		if (!GetSelectedBitmaps(&pList, &ListSize))
		return FALSE;

		if(Param->GetAreDelayValuesSame())
		{
			UINT32 ListCounter = ListSize;

			while (ListCounter>0)
			{
				KernelBitmap* pKernelBitmap = pList[--ListCounter];
				if (pKernelBitmap != NULL)
				{
					pKernelBitmap->SetDelay(Param->GetAnimDelay());
				}
			}
		}
		if (Param->GetSameRestoreType())
		{
			UINT32 ListCounter = ListSize;

			while (ListCounter>0)
			{
				KernelBitmap* pKernelBitmap = pList[--ListCounter];
				if(pKernelBitmap != NULL)
				{
					OILBitmap* pOILBitmap = pKernelBitmap->GetActualBitmap();
					if (pOILBitmap != NULL)
					{
						pOILBitmap->SetAnimationRestoreType(Param->GetRestoreType());
					}
				}
			}
		}
		// Andy Hills, 09-10-00
		// Record changes to the 'smooth when scaled up' tickbox
		if (Param->GetAreInterpolationValuesSame())
		{
			UINT32 ListCounter = ListSize;

			while (ListCounter>0)
			{
				KernelBitmap* pKernelBitmap = pList[--ListCounter];
				if(pKernelBitmap != NULL)
				{
					OILBitmap* pOILBitmap = pKernelBitmap->GetActualBitmap();
					if (pOILBitmap != NULL)
					{
						// change the interpolation setting of each selected bitmap
						pOILBitmap->SetInterpolation(Param->GetInterpolation());

						// invalidate all uses of this bitmap, so that the screen is updated correctly
						if (WorkDoc != NULL)
						{
							pOILBitmap->InvalidateAllReferences(WorkDoc);
						}
					}
				}
			}
		}
		if (pList != NULL)
			CCFree(pList);
		
	return TRUE;
}


/************************************************************************************************************

>	BOOL BitmapSGallery::IsJPEG()

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/01/96
	Inputs:		pList	 - A ptr to a Kernel bitmap. 
				ListSize - The number of bitmaps selected in the bitmap gallery. 
				Param	 - Allows data to be passed to and from the bitmap properties dialog.
				pBytes	 - If it is a JPEG, the memory (bytes), is returned to the caller. 
	Outputs:	-
	Returns:	True if the selected bitmap is a compressed JPEG, false otherwise
	Purpose:	To determine whether an imported bitmap is a compressed JPEG.
	SeeAlso:	-

***************************************************************************************************************/

BOOL BitmapSGallery::IsJPEG(KernelBitmap** pList,UINT32 ListSize, BmpDlgParam* Param, INT32* pBytes)
{
	ERROR2IF(pList == NULL || Param == NULL || pBytes == NULL, FALSE, "NULL Parameters Passed");

	BitmapSource* pSource = NULL;
	BaseBitmapFilter* pDummyFilter;

	KernelBitmap* pKernelBitmap =  pList[ListSize];

	if (pKernelBitmap == NULL)
		return FALSE;
	
	BOOL OriginalSourcePresent = pKernelBitmap->GetOriginalSource(&pSource, &pDummyFilter);
	
	if(OriginalSourcePresent)
	{
		if (pSource != NULL)
		{
			if (pSource->IsJPEG())
			{
				Param->SetIsJPEG(TRUE);
				*pBytes += pSource->GetSize();
			}
		}
	}
	else
	{
		Param->SetIsJPEG(FALSE);
	}
	return Param->GetIsJPEG();
}

/******************************************************************************************

	BOOL BitmapSGallery::AreDelayValuesSame ()

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/01/96
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if values are the same , FALSE otherwise.
	Purpose:	To find if the Animation delay values of selected Bitmaps are the same.
	SeeAlso:	-

********************************************************************************************/
/*
BOOL BitmapSGallery::AreDelayValuesSame()
{

	
	UINT32 ListSize = 0;				
	KernelBitmap** pList = NULL;	
	BOOL DelayValuesDiffer = FALSE;
	UINT32 i =0;
	UINT32 e=0;
	UINT32 Value =0;
	
	if (!GetSelectedBitmaps(&pList, &ListSize))
		return FALSE;

	UINT32 TempListSize = ListSize;
	UINT32* Delay = new UINT32[ListSize];

	if (!Delay)
		return FALSE;

	// Get each bitmaps delay value
	while (ListSize>0)
	{
		ListSize--;
		Value = pList[ListSize]->GetDelay();
		Delay[i] = Value;
		i++;
	}

	// Compare the values.		
	for(i=0; i<TempListSize; i++)
	{
		for(UINT32 e=0; e<TempListSize; e++)
		{
			if (Delay[i] != Delay[e])
				return FALSE;
		}
	}

	if (ListSize != NULL)
		CCFree(pList);
		delete [] Delay;

	return TRUE;
}
*/
/********************************************************************************************
>	UINT32 BitmapSGallery::GetBitmapAnimDelay()

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/01/96
	Inputs:		 
	Outputs:	-
	Returns:	The Delay value stored in a Bitmap.
	Purpose:	
	SeeAlso:	-
********************************************************************************************/
/*
UINT32 BitmapSGallery::GetBitmapDelay()
{
	UINT32 ListSize = 0;
	KernelBitmap** pList = NULL;	
	UINT32 i =0;
	
	if (!GetSelectedBitmaps(&pList, &ListSize))
		InformError();

	return	pList[i]->GetActualBitmap()->GetBitmapAnimDelay();
}
*/



