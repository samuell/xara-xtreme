// $Id: coldrop.cpp 1052 2006-05-15 14:19:14Z alex $
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

// coldrop.cpp - Colour drop-down list support

/*
*/


//-----------------------------------------------------------------------------------------
// Include files

#include "camtypes.h"
#include "camelot.h"
#include "errors.h"
#include "palman.h"
#include "cartprov.h"
#include "bitmapgriddropdown.h"
#include "dragbmp.h"
//#include <wx/dcmemory.h>
#include "grndbmp.h"
#include "colcontx.h"

#include "sgline.h"
#include "ccdc.h"
#include "dlgview.h"
#include "grnddib.h"
#include "ppbrush.h"
#include "qualattr.h"




//-----------------------------------------------------------------------------------------
// This must be defined AFTER all CC_IMPLEMENT_DYNCREATE calls
#define new CAM_DEBUG_NEW


CBGDDItemInfo::CBGDDItemInfo()
{
	m_uiBitmapResID		= (UINT32)-1;
	m_pBitmap			= NULL;
	m_bBitmapAutodelete = FALSE;
}


/******************************************************************************
Function  : CBGDDItemInfo::CBGDDItemInfo
Author    : Mikhail Tatarnikov
Purpose   : Construct an item with image stored in resources
Returns   : void
Exceptions: 
Parameters: [in] UINT32		uiBitmapResID - resource id with the image;
            [in] String_256 strLabel	  - label for the item.
Notes     : 
******************************************************************************/
CBGDDItemInfo::CBGDDItemInfo(UINT32 uiBitmapResID, String_256 strLabel)
{
	m_uiBitmapResID		= uiBitmapResID;
	m_pBitmap			= NULL;
	m_bBitmapAutodelete = FALSE;
	m_strLabel			= strLabel;
}

/******************************************************************************
Function  : CBGDDItemInfo::CBGDDItemInfo
Author    : Mikhail Tatarnikov
Purpose   : Constructs an item with bitmap
Returns   : void
Exceptions: 
Parameters: [in] wxBitmap*	pBitmap			  - bitmap to show for the image;
            [in] BOOL		bBitmapAutodelete - whether we own the bitmap or not;
            [in] String_256 strLabel		  - label for the item.
Notes     : 
******************************************************************************/
CBGDDItemInfo::CBGDDItemInfo(wxBitmap* pBitmap, BOOL bBitmapAutodelete, String_256 strLabel)
{
	m_pBitmap			= pBitmap;
	m_uiBitmapResID		= (UINT32)-1;
	m_bBitmapAutodelete = bBitmapAutodelete;
	m_strLabel			= strLabel;
}

CBGDDItemInfo::~CBGDDItemInfo()
{
	if (m_pBitmap && m_bBitmapAutodelete)
		delete m_pBitmap;
}


/******************************************************************************
Function  : CBGDDItemInfo::GetBitmap
Author    : Mikhail Tatarnikov
Purpose   : Gets the bitmap associated with the item
Returns   : wxBitmap* - the item's bitmap.
Exceptions: 
Parameters: None
Notes     : Loads bitmap from resources if necessary.
******************************************************************************/
wxBitmap* CBGDDItemInfo::GetBitmap()
{
	// If we are given a bitmap, just return it.
	if (m_pBitmap)
		return m_pBitmap;

	//  Try to load the resource otherwise.
	return CamArtProvider::Get()->FindBitmap(m_uiBitmapResID, CAF_DEFAULT);
}


/******************************************************************************
Function  : CBGDDItemInfo::GetLabel
Author    : Mikhail Tatarnikov
Purpose   : Gets the text assocciated with the item.
Returns   : String_256 - the item label.
Exceptions: 
Parameters: None
Notes     : 
******************************************************************************/
String_256 CBGDDItemInfo::GetLabel()
{
	return m_strLabel;
}










CBitmapGridDropDown::CBitmapGridDropDown()
{
	m_poUnselectedItem	= NULL;
}



CBitmapGridDropDown::~CBitmapGridDropDown()
{
}


/******************************************************************************
Function  : CBitmapGridDropDown::AddItem
Author    : Mikhail Tatarnikov
Purpose   : Adds an item with an image from a resource
Returns   : void
Exceptions: 
Parameters: [in] UINT32		uiBitmapResID - resource id with the image;
            [in] String_256 strLabel	  - label for the item.
Notes     : 
******************************************************************************/
void CBitmapGridDropDown::AddItem(UINT32 uiBitmapResID, String_256 strLabel)
{
	CBGDDItemInfo* pData = new CBGDDItemInfo(uiBitmapResID, strLabel);
	CGridDropDown::AddItem(pData);
}

/******************************************************************************
Function  : CBitmapGridDropDown::AddItem
Author    : Mikhail Tatarnikov
Purpose   : Adds an item with an image
Returns   : void
Exceptions: 
Parameters: [in] wxBitmap*	pBitmap	 - the bitmap to be shown for the item;
            [in] String_256 strLabel - the item label.
Notes     : 
******************************************************************************/
void CBitmapGridDropDown::AddItem(wxBitmap* pBitmap, String_256 strLabel)
{
	CBGDDItemInfo* pData = new CBGDDItemInfo(pBitmap, TRUE, strLabel);
	CGridDropDown::AddItem(pData);
}

/******************************************************************************
Function  : CBitmapGridDropDown::AddItem
Author    : Mikhail Tatarnikov
Purpose   : Adds an item with a kernel bitmap
Returns   : void
Exceptions: 
Parameters: [in] KernelBitmap* pKernelBitmap - the bitmap to be shown for the item;
            [in] String_256	   strLabel		 - the item label.
Notes     : 
******************************************************************************/
void CBitmapGridDropDown::AddItem(KernelBitmap* pKernelBitmap, String_256 strLabel)
{
	wxSize szItem =  GetItemSize();
	wxBitmap* pBitmap = new wxBitmap(szItem.x, szItem.y);
	wxMemoryDC dcMem;

	dcMem.SelectObject(*pBitmap);

	BitmapDragInformation oDragBitmap(pKernelBitmap, szItem.x, szItem.y, 0, 0);
	oDragBitmap.OnDrawSolidDrag(wxPoint(0, 0), &dcMem);


	dcMem.SelectObject(wxNullBitmap);

	AddItem(pBitmap, strLabel);
}


/******************************************************************************
Function  : CBitmapGridDropDown::AddItem
Author    : Mikhail Tatarnikov
Purpose   : Adds an item and display a stroke in it.
Returns   : void
Exceptions: 
Parameters: [in] LineAttrItem* plaiStroke - the stroke to display in the item;
            [in] String_256	   strLabel	  - the item label.
Notes     : 
******************************************************************************/
void CBitmapGridDropDown::AddItem(LineAttrItem* plaiStroke, String_256 strLabel)
{
	KernelBitmap* pBitmap = PreviewStroke(plaiStroke);
	AddItem(pBitmap, strLabel);

	delete pBitmap;
	delete plaiStroke;
}


/******************************************************************************
Function  : CBitmapGridDropDown::AddItem
Author    : Mikhail Tatarnikov
Purpose   : Adds an item and display a brush in it.
Returns   : void
Exceptions: 
Parameters: [in] AttrBrushType* pabtBrush - the brush to be previewed in the item;
            [in] String_256		strLabel  - the item label.
Notes     : 
******************************************************************************/
void CBitmapGridDropDown::AddItem(AttrBrushType* pabtBrush, String_256 strLabel)
{
	wxBitmap* pBitmap = PreviewBrush(pabtBrush);
	AddItem(pBitmap, strLabel);
}


/******************************************************************************
Function  : CBitmapGridDropDown::SetUnselectedItem
Author    : Mikhail Tatarnikov
Purpose   : Sets an image to be used as "unselected" item
Returns   : void
Exceptions: 
Parameters: [in] UINT32		uiBitmapResID - resource id with the image;
            [in] String_256 strLabel	  - label for the item.
Notes     : 
******************************************************************************/
void CBitmapGridDropDown::SetUnselectedItem(UINT32 uiBitmapResID, String_256 strLabel)
{
	m_poUnselectedItem = new CBGDDItemInfo(uiBitmapResID, strLabel);
}


/******************************************************************************
Function  : CBitmapGridDropDown::SetUnselectedItem
Author    : Mikhail Tatarnikov
Purpose   : Sets an image to be used as "unselected" item
Returns   : void
Exceptions: 
Parameters: [in] wxBitmap*	pBitmap	 - the bitmap to be shown for the "unselected" item;
            [in] String_256 strLabel - the item label.
Notes     : 
******************************************************************************/
void CBitmapGridDropDown::SetUnselectedItem(wxBitmap* pBitmap, String_256 strLabel)
{
	m_poUnselectedItem = new CBGDDItemInfo(pBitmap, TRUE, strLabel);
}


/******************************************************************************
Function  : CBitmapGridDropDown::DrawItemCore
Author    : Mikhail Tatarnikov
Purpose   : Draws the item core - the image itself
Returns   : void
Exceptions: 
Parameters: [in] wxDC&	 dc		- the device context to draw to;
            [in] wxRect& rect	- the area of the item;
            [in] int	 iItem	- item index;
            [in] int	 iFlags - additional flags (selected, highlighted, ...)
Notes     : 
******************************************************************************/
void CBitmapGridDropDown::DrawItemCore(wxDC& dc, const wxRect& rect, int iItem, INT32 iFlags)
{
	// If the item isn't enabled just don't draw it
	// TODO: grey out the current bitmap instead.
	if (iFlags & wxGridComboPopup::keDisabled)
		return;

	CBGDDItemInfo* pData = (CBGDDItemInfo*)GetItemData(iItem);
	if (!pData)
		return;

	// Obtain the bitmap to draw.
	wxBitmap* pBitmap = pData->GetBitmap();
	if (!pBitmap)
		return;

	// Draw to the memory bitmap first (so we can be sure the borders
	// isn't eated by the scaling.
	wxMemoryDC dcMem;
//	wxSize szItem =  GetItemSize();
	wxBitmap oMemBitmap(rect.width, rect.height);
	dcMem.SelectObject(oMemBitmap);


	static wxPen   penBackground(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));
	static wxBrush brBackground(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));

	// Erase with background (by default it's grey).
	dcMem.SetPen(penBackground);
	dcMem.SetBrush(brBackground);

	dcMem.DrawRectangle(0, 0, rect.width, rect.height);


	double dXScale = (double)rect.width  / pBitmap->GetWidth();
	double dYScale = (double)rect.height / pBitmap->GetHeight();

	dcMem.SetUserScale(dXScale, dYScale);

	dcMem.DrawBitmap(*pBitmap, 0, 0, TRUE);
	

	dcMem.SetBrush(wxNullBrush);
	dcMem.SetPen(wxNullPen);
	dcMem.SelectObject(wxNullBitmap);
	
	// Now we can draw bitmap without scaling - it has been scaled while
	// drawing to the memory DC.
	dc.DrawBitmap(oMemBitmap, rect.x, rect.y, false);

}


/******************************************************************************
Function  : CBitmapGridDropDown::GetItemData
Author    : Mikhail Tatarnikov
Purpose   : Gets the item data
Returns   : CBGDDItemInfo* - the user data associated with the item;
Exceptions: 
Parameters: [in] INT32 iItem - the item index;
Notes     : 
******************************************************************************/
CBGDDItemInfo* CBitmapGridDropDown::GetItemData(INT32 iItem)
{
	if (iItem == -1)
		return m_poUnselectedItem;

	return dynamic_cast<CBGDDItemInfo*>(CGridDropDown::GetItemData(iItem));
}

/******************************************************************************
Function  : CBitmapGridDropDown::SelectByLabel
Author    : Mikhail Tatarnikov
Purpose   : Selects the element by label.
Returns   : void
Exceptions: 
Parameters: [in] String_256 strLabel - the label of the existing item to select;
Notes     : If several items with the same label exist, the first one is to be selected.
			If no item's label match, the combobox is unselected.
******************************************************************************/
void CBitmapGridDropDown::SelectByLabel(String_256 strLabel)
{
	// Walk over all items, looking for the first exact match.
	INT32 iSelectedIndex = -1;
	INT32 iItemsNum = GetItemsNum();
	for (INT32 i = 0; i < iItemsNum; ++i)
	{
		CBGDDItemInfo* pData = GetItemData(i);
		
		if (pData->GetLabel() == strLabel)
		{
			iSelectedIndex = i;
			break;
		}
		
	}
	
	SetSelected(iSelectedIndex);
}



/******************************************************************************
Function  : CBitmapGridDropDown::PreviewBrush
Author    : Mikhail Tatarnikov
Purpose   : Preview a brush into a bitmap
Returns   : wxBitmap* - the brush preview
Exceptions: 
Parameters: [in] AttrBrushType* pabtBrush - brush to preview.
Notes     : 
******************************************************************************/
wxBitmap* CBitmapGridDropDown::PreviewBrush(AttrBrushType* pabtBrush)
{
	ERROR2IF(TRUE, FALSE, "CBitmapGridDropDown::PreviewBrush Not implemented yet");
/*
	wxMemoryDC dcMem;

	// Setup a memory DC to draw into a bitmap.
	wxSize szItem =  GetItemSize();
	wxBitmap* pBitmap = new wxBitmap(szItem.x, szItem.y);
	dcMem.SelectObject(*pBitmap);



//	ReDrawInfoType ExtraInfo;
//	ExtraInfo.pMousePos = NULL;		// No mouse position info for redraw events

	// Build a CC dc out of it for rendering to the screen
	// Get a MFC CDC to put the DC in
	CCDC MyDc(&dcMem);

	// The devices DPI
//	ExtraInfo.Dpi = OSRenderRegion::GetFixedDCPPI(MyDc).GetHeight();

	// How big the window is
//	ExtraInfo.dx = (((INT32)szItem.GetWidth())*72000) / ExtraInfo.Dpi;
//	ExtraInfo.dy = (((INT32)szItem.GetHeight())*72000) / ExtraInfo.Dpi;

	// Alternatively we can do this:
	wxRect ClipRect(wxPoint(0, 0), szItem);



	// paths should not be made static (for safety) ....
	Path  CurvePath;

	CurvePath.Initialise(8);
	CurvePath.IsFilled  =  FALSE;
	CurvePath.IsStroked = TRUE;
	CurvePath.FindStartOfPath();

	// avoid overhead of allocation each time ....
	INT32 iWidth	  = szItem.x;
	INT32 iHeight	  = szItem.y;
	INT32 iBorderX	  = (INT32)(0.05 * iWidth);
	INT32 iBorderY	  = (INT32)(0.05 * iHeight);
	INT32 iHalfWidth  = szItem.x / 2;
	INT32 iHalfHeight = szItem.y / 2;

	DocCoord StartPoint2	(iBorderX, iBorderY);
	DocCoord MiddleCP1		(iBorderX, iHalfHeight);
	DocCoord MiddlePoint	(iHalfWidth, iHalfHeight);
	DocCoord MiddleCP2		(iWidth-iBorderX, iHalfHeight);
	DocCoord EndPoint2		(iWidth-iBorderX, iHeight-iBorderY);

	CurvePath.InsertMoveTo	(StartPoint2);
	CurvePath.InsertCurveTo	(StartPoint2, MiddleCP1, MiddlePoint);
	CurvePath.InsertCurveTo	(MiddlePoint, MiddleCP2, EndPoint2);
	CurvePath.InsertLineTo	(EndPoint2);

	// render a curved line ....
	DocRect RenderRect(0, 0, iWidth, iHeight);

	DialogView *pDialogView = new DialogView;
	pDialogView->Init();
	FIXED16 Scale(1);

	Matrix oMatrix(0, 0);


	GRenderRegion* pRenderRegion = new GRenderDIB(RenderRect, oMatrix, Scale, 32, 1.0);

	static const StockColour  kBackgroundOutline  =  COLOUR_NONE;
	static const StockColour  kBackground         =  COLOUR_WHITE;
	

	PathProcessorBrush* pPathProc = pabtBrush->GetPathProcessor();
	BrushAttrValue* pVal = (BrushAttrValue*)pabtBrush->GetAttributeValue();

	if( pRenderRegion != 0 )
	{
		pRenderRegion->SaveContext();

		/// set drawing quality
		Quality           QualityThing( Quality::QualityMax );
		QualityAttribute  AntiAliasQualityAttr( QualityThing );
		pRenderRegion->SetQuality( &AntiAliasQualityAttr, FALSE );

		pRenderRegion->SetFillColour( kBackground );
		pRenderRegion->DrawRect( &RenderRect );

		pRenderRegion->SetLineWidth (1);
		pPathProc->ScaleToValue(1);
		pRenderRegion->SetLineColour( kBackgroundOutline );

		pVal->FlushCache  ();
		pabtBrush->Render(pRenderRegion);
		pRenderRegion->DrawPath (&CurvePath);

		pRenderRegion->RestoreContext();
//		DestroyGRenderRegion( pRenderRegion );
		pRenderRegion->StopRender();
		delete pRenderRegion;
	}
		
	delete pDialogView;


	MyDc.GetDC()->EndDrawing();		

	dcMem.SelectObject(wxNullBitmap);
	
	return pBitmap;
*/
	return NULL;
}



/******************************************************************************
Function  : CBitmapGridDropDown::PreviewStroke
Author    : Mikhail Tatarnikov
Purpose   : Preview a stroke into a bitmap
Returns   : KernelBitmap* - the stroke preview.
Exceptions: 
Parameters: [in] LineAttrItem* plaiStroke - stroke to render.
Notes     : 
******************************************************************************/
KernelBitmap* CBitmapGridDropDown::PreviewStroke(LineAttrItem* plaiStroke)
{
	wxMemoryDC dcMem;

	// Setup a memory DC to draw into a bitmap.
	wxSize szItem =  GetItemSize();
	wxBitmap* pBitmap = new wxBitmap(szItem.x, szItem.y);
	dcMem.SelectObject(*pBitmap);

	dcMem.SetTextForeground(wxColour(0, 0, 0));

	INT32 iDPI = 96;

	DialogView *pDialogView = new DialogView;
	pDialogView->Init();
	FIXED16 Scale(1);

	Matrix oMatrix(1, 0, 0, 1, 0, 0);




	DocRect rcDocClip;
	// Convert the item area to millipoints
	rcDocClip.lo.x = 0;
	rcDocClip.lo.y = 0;

	rcDocClip.hi.x = (szItem.x * 1000);
	rcDocClip.hi.y = (szItem.y * 1000);


	// Create a bitmap render region.
	GRenderBitmap* pRenderRegion = new GRenderBitmap(rcDocClip, oMatrix, Scale, 24, iDPI, FALSE, 0, NULL, TRUE);
	pRenderRegion->AttachDevice(pDialogView, &dcMem, NULL, false);

	// Prepare for the rendering.
	pRenderRegion->InitDevice();
	pRenderRegion->SaveContext();
	pRenderRegion->StartRender();

	// Render the stroke.
	plaiStroke->Render(pRenderRegion, rcDocClip, LineAttrItem::NO_LABEL);

	pRenderRegion->StopRender();
	pRenderRegion->RestoreContext();

	// WinBM Now needs to be setup with the newly created bitmap
	KernelBitmap* pKernelBitmap = new KernelBitmap(pRenderRegion->ExtractBitmapCopy(), TRUE);


	delete pRenderRegion;
	delete pDialogView;

	dcMem.SelectObject(wxNullBitmap);
	delete pBitmap;

	return pKernelBitmap;
}
