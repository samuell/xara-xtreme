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


CBGDDItemInfo::CBGDDItemInfo(String_256 strLabel)
{
	m_strLabel = strLabel;
}


CBGDDItemInfo::~CBGDDItemInfo()
{
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




CBGDDWxBitmapItem::CBGDDWxBitmapItem(wxBitmap* pBitmap, BOOL bAutodelete,
									 String_256 strLabel,
									 BOOL bStretch)
	: CBGDDItemInfo(strLabel)
{
	m_pBitmap  = pBitmap;
	m_bDelete  = bAutodelete;
	m_bStretch = bStretch;
}

CBGDDWxBitmapItem::~CBGDDWxBitmapItem()
{
	if (m_bDelete)
		delete m_pBitmap;
}


void CBGDDWxBitmapItem::DrawItem(wxDC& dc, const wxRect& rect, INT32 iFlags) const
{
	// If the item isn't enabled just don't draw it
	// TODO: grey out the current bitmap instead.
	if (iFlags & wxGridComboPopup::keDisabled)
		return;


	// Draw to the memory bitmap first (so we can be sure the borders
	// isn't eated by the scaling.
	wxMemoryDC dcMem;
	wxBitmap oMemBitmap(rect.width, rect.height);
	dcMem.SelectObject(oMemBitmap);


	static wxPen   penBackground(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));
	static wxBrush brBackground(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));

	// Erase with background (by default it's grey).
	dcMem.SetPen(penBackground);
	dcMem.SetBrush(brBackground);

	dcMem.DrawRectangle(0, 0, rect.width, rect.height);

	double dXScale = 1.0;
	double dYScale = 1.0;

	if (m_bStretch)
	{
		dXScale = (double)rect.width  / (double)m_pBitmap->GetWidth();
		dYScale = (double)rect.height / (double)m_pBitmap->GetHeight();
	}

	dcMem.SetUserScale(dXScale, dYScale);

	dcMem.DrawBitmap(*m_pBitmap, 0, 0, TRUE);


	dcMem.SetBrush(wxNullBrush);
	dcMem.SetPen(wxNullPen);
	dcMem.SelectObject(wxNullBitmap);
	
	// Now we can draw bitmap without scaling - it has been scaled while
	// drawing to the memory DC.
	dc.SetUserScale(1.0, 1.0);
	dc.DrawBitmap(oMemBitmap, rect.x, rect.y, false);
}


CBGDDResourceItem::CBGDDResourceItem(UINT32 uiBitmapResID, String_256 strLabel, BOOL bStretch)
	: CBGDDWxBitmapItem(NULL, FALSE, strLabel, bStretch)
{
	m_pBitmap = CamArtProvider::Get()->FindBitmap(uiBitmapResID, CAF_DEFAULT);
	m_bDelete = FALSE;
}

CBGDDResourceItem::~CBGDDResourceItem()
{
}









CBGDDKernelBitmapItem::CBGDDKernelBitmapItem(KernelBitmap* pKernelBitmap, BOOL bAutodelete,
											 String_256 strLabel, BOOL bStretch)
	: CBGDDItemInfo(strLabel)
{
	m_pKernelBitmap		  = pKernelBitmap;
	m_bDeleteKernelBitmap = bAutodelete;
	m_bStretch			  = bStretch;
}

CBGDDKernelBitmapItem::~CBGDDKernelBitmapItem()
{
	if (m_bDeleteKernelBitmap)
		delete m_pKernelBitmap;

	TDCacheCollection::const_iterator citCur;
	for (citCur = m_colCache.begin(); citCur != m_colCache.end(); ++citCur)
	{
		delete (*citCur)->second;
		delete *citCur;
	}
	
	m_colCache.clear();
}


BOOL CBGDDKernelBitmapItem::DoesCacheItemSizeMatch(const pair<wxSize, wxBitmap*>* poItem, wxSize szBitmap)
{
	return poItem->first == szBitmap;
}

/******************************************************************************
Function  : CBGDDKernelBitmapItem::GetWxBitmap
Author    : Mikhail Tatarnikov
Purpose   : Gets the wxBitmap representation of the currently stored KernelBitmap
Returns   : wxBitmap* - the required version of the KernelBitmap.
Exceptions: 
Parameters: [in] wxSize szBitmap - the size of representation.
Notes     : Since the KernelBitmap stored in the item can be of considerable size,
			we have to speed up the drawing (and scaling). It's done by caching
			the bitmap representations.
******************************************************************************/
wxBitmap* CBGDDKernelBitmapItem::GetWxBitmap(wxSize szBitmap) const
{
	// Try to locate the cache.
	TDCacheCollection::const_iterator citFound = find_if(m_colCache.begin(), m_colCache.end(),
		bind2nd(ptr_fun(DoesCacheItemSizeMatch), szBitmap));

	if (citFound != m_colCache.end())
		return (*citFound)->second;

	// Here we have a cache miss. We need to rebuild the bitmap.
	// NOTE: the maximum size of bitmap allowed by BitmapDragInformation is 180*??? pixels!!
	wxMemoryDC dcMem;
	wxBitmap* pBitmap = new wxBitmap(szBitmap.x, szBitmap.y);
	dcMem.SelectObject(*pBitmap);
	dcMem.SetUserScale(1.0, 1.0);

	wxSize szDragBitmap = szBitmap;
	if (!m_bStretch)
		szDragBitmap = wxSize(m_pKernelBitmap->GetWidth(), m_pKernelBitmap->GetHeight());

	BitmapDragInformation oDragBitmap(m_pKernelBitmap, szDragBitmap.x, szDragBitmap.y, 0, 0);
	oDragBitmap.OnDrawSolidDrag(wxPoint(0, 0), &dcMem);

	dcMem.SelectObject(wxNullBitmap);

	// Now we have a resulting bitmap. We need to cache and return it.
	m_colCache.push_back(new pair<wxSize, wxBitmap*>(szBitmap, pBitmap));

	return pBitmap;
}


void CBGDDKernelBitmapItem::DrawItem(wxDC& dc, const wxRect& rcDraw, INT32 iFlags) const
{
	wxBitmap* pBitmap = GetWxBitmap(rcDraw.GetSize());

	// We don't need to scale or do anything since we already have a bitmap of the right size.
	dc.DrawBitmap(*pBitmap, rcDraw.x, rcDraw.y, FALSE);
}







CBGDDStrokeItem::CBGDDStrokeItem(LineAttrItem* plaiStroke, BOOL bAutodelete, String_256 strLabel)
	: CBGDDItemInfo(strLabel)
{
	m_plaiStroke = plaiStroke;
	m_bDelete	 = bAutodelete;
}

CBGDDStrokeItem::~CBGDDStrokeItem()
{
	if (m_bDelete)
		delete m_plaiStroke;
}

void CBGDDStrokeItem::DrawItem(wxDC& dc, const wxRect& rect, INT32 iFlags) const
{
	INT32 iDPI = 96;

	// Since we don't have a view, we need a fake one.
	DialogView *pDialogView = new DialogView;
	pDialogView->Init();
	FIXED16 Scale(1);

	Matrix oMatrix(1, 0, 0, 1, 0, 0);

	wxSize szItem = rect.GetSize();

	// Convert the item area to millipoints
	DocRect rcDocClip;
	rcDocClip.lo.x = 0;
	rcDocClip.lo.y = 0;

	rcDocClip.hi.x = (szItem.x * 1000);
	rcDocClip.hi.y = (szItem.y * 1000);


	// Create a bitmap render region.
	GRenderBitmap* pRenderRegion = new GRenderBitmap(rcDocClip, oMatrix, Scale, 24, iDPI, FALSE, 0, NULL, TRUE);
	pRenderRegion->AttachDevice(pDialogView, &dc, NULL, false);
	
	// Prepare for the rendering.
	pRenderRegion->InitDevice();
	pRenderRegion->SaveContext();
	pRenderRegion->StartRender();

	// Render the stroke.
	m_plaiStroke->Render(pRenderRegion, rcDocClip, LineAttrItem::NO_LABEL);

	pRenderRegion->StopRender();
	pRenderRegion->RestoreContext();

	KernelBitmap* pKernelBitmap = new KernelBitmap(pRenderRegion->ExtractBitmap(), FALSE);

	BitmapDragInformation oDragBitmap(pKernelBitmap, szItem.x, szItem.y, 0, 0);
	oDragBitmap.OnDrawSolidDrag(rect.GetTopLeft(), &dc);

	delete pKernelBitmap;
	delete pRenderRegion;
	delete pDialogView;

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
void CBitmapGridDropDown::AddItem(UINT32 uiBitmapResID, String_256 strLabel, BOOL bStretch)
{
	CBGDDItemInfo* pData = new CBGDDResourceItem(uiBitmapResID, strLabel, bStretch);
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
void CBitmapGridDropDown::AddItem(wxBitmap* pBitmap, BOOL bDelete, String_256 strLabel, BOOL bStretch)
{
	CBGDDItemInfo* pData = new CBGDDWxBitmapItem(pBitmap, bDelete, strLabel, bStretch);
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
void CBitmapGridDropDown::AddItem(KernelBitmap* pKernelBitmap, BOOL bDelete, String_256 strLabel, BOOL bStretch)
{
	CBGDDItemInfo* pData = new CBGDDKernelBitmapItem(pKernelBitmap, bDelete, strLabel, bStretch);
	CGridDropDown::AddItem(pData);
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
void CBitmapGridDropDown::AddItem(LineAttrItem* plaiStroke, BOOL bDelete, String_256 strLabel)
{
	CBGDDStrokeItem* pStrokeItem = new CBGDDStrokeItem(plaiStroke, bDelete, strLabel);
	CGridDropDown::AddItem(pStrokeItem);
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
void CBitmapGridDropDown::AddItem(AttrBrushType* pabtBrush, BOOL bDelete, String_256 strLabel)
{
	wxBitmap* pBitmap = PreviewBrush(pabtBrush);
	AddItem(pBitmap, bDelete, strLabel);
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
	m_poUnselectedItem = new CBGDDResourceItem(uiBitmapResID, strLabel);
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
	m_poUnselectedItem = new CBGDDWxBitmapItem(pBitmap, TRUE, strLabel);
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
	CBGDDItemInfo* pItemData = GetItemData(iItem);

	pItemData->DrawItem(dc, rect, iFlags);
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

	return (CBGDDItemInfo*)CGridDropDown::GetItemData(iItem);
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
