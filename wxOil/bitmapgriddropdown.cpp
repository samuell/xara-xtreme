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


/******************************************************************************
Function  : CBGDDWxBitmapItem::DrawItem
Author    : Mikhail Tatarnikov
Purpose   : Draws this item into dc
Returns   : void
Exceptions: 
Parameters: [in] wxDC&		   dc	  - device context to draw to;
            [in] const wxRect& rect	  - area occupied by the item;
            [in] INT32		   iFlags - drawing parameters.
Notes     : 
******************************************************************************/
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



CBGDDCachedItem::CBGDDCachedItem(String_256 strLabel)
	: CBGDDItemInfo(strLabel)
{
}

CBGDDCachedItem::~CBGDDCachedItem()
{
	TDCacheCollection::const_iterator citCur;
	for (citCur = m_colCache.begin(); citCur != m_colCache.end(); ++citCur)
	{
		delete (*citCur)->second;
		delete *citCur;
	}
	
	m_colCache.clear();
}

/******************************************************************************
Function  : CBGDDCachedItem::DoesCacheItemSizeMatch
Author    : Mikhail Tatarnikov
Purpose   : Checks whether a cache item corresponds to the specific size
Returns   : BOOL -
Exceptions: 
Parameters: [in] const pair<wxSize, xBitmap*>* poItem - 
            [in] wxSize szBitmap - 
Notes     : 
******************************************************************************/
BOOL CBGDDCachedItem::DoesCacheItemSizeMatch(const pair<wxSize, wxBitmap*>* poItem, wxSize szBitmap)
{
	return poItem->first == szBitmap;
}


/******************************************************************************
Function  : CBGDDCachedItem::GetWxBitmap
Author    : Mikhail Tatarnikov
Purpose   : Retrieves a representation from the cache
Returns   : wxBitmap* - the representation with the required size.
Exceptions: 
Parameters: [in] wxSize szBitmap - cached image size to look for.
Notes     : Creates a new representation if it doesn't exist.
******************************************************************************/
wxBitmap* CBGDDCachedItem::GetWxBitmap(wxSize szBitmap) const
{
	// Try to locate the cache.
	TDCacheCollection::const_iterator citFound = find_if(m_colCache.begin(), m_colCache.end(),
		bind2nd(ptr_fun(DoesCacheItemSizeMatch), szBitmap));

	if (citFound != m_colCache.end())
		return (*citFound)->second;

	// No bitmap found in the cache. Ask the derivide class to render its content.
	wxBitmap* pBitmap = RenderItemToBitmap(szBitmap);

	// Now we have a resulting bitmap. We need to cache and return it.
	m_colCache.push_back(new pair<wxSize, wxBitmap*>(szBitmap, pBitmap));

	return pBitmap;
}

void CBGDDCachedItem::DrawItem(wxDC& dc, const wxRect& rcDraw, INT32 iFlags) const
{
	wxBitmap* pBitmap = GetWxBitmap(rcDraw.GetSize());

	// We don't need to scale or do anything since we already have a bitmap of the right size.
	dc.DrawBitmap(*pBitmap, rcDraw.x, rcDraw.y, FALSE);
}


/******************************************************************************
Function  : CBGDDCachedItem::RenderItemToBitmap
Author    : Mikhail Tatarnikov
Purpose   : Renders an item to bitmap.
Returns   : wxBitmap* - bitmap with the rendered item.
Exceptions: 
Parameters: [in] wxSize szBitmap - size of the required bitmap.
Notes     : The function prepares a GRenderRegion and asks the derived class to
			draw itself into it.
******************************************************************************/
wxBitmap* CBGDDCachedItem::RenderItemToBitmap(wxSize szBitmap) const
{
	wxMemoryDC dcMem;
	wxBitmap* pBitmap = new wxBitmap(szBitmap.x, szBitmap.y);
	dcMem.SelectObject(*pBitmap);


	INT32 iDPI = 96;

	// Since we don't have a view, we need a fake one.
	DialogView *pDialogView = new DialogView;
	pDialogView->Init();
	FIXED16 Scale(1);

	Matrix oMatrix(1, 0, 0, 1, 0, 0);

	// Convert the item area to millipoints
	DocRect drcDocClip;
	drcDocClip.lo.x = 0;
	drcDocClip.lo.y = 0;

	drcDocClip.hi.x = (szBitmap.x * 1000);
	drcDocClip.hi.y = (szBitmap.y * 1000);


	// Create a bitmap render region.
	GRenderBitmap* pRenderRegion = new GRenderBitmap(drcDocClip, oMatrix, Scale, 24, iDPI, FALSE, 0, NULL, TRUE);
	pRenderRegion->AttachDevice(pDialogView, &dcMem, NULL, false);
	
	// Prepare for the rendering.
	pRenderRegion->InitDevice();
	pRenderRegion->SaveContext();
	pRenderRegion->StartRender();


	// Ask the derived class to render itself into GRenderRegion.
	RenderItemToGRenderRegion(pRenderRegion, drcDocClip);

	// Finalize the rendering.
	pRenderRegion->StopRender();
	pRenderRegion->RestoreContext();

	// Extract bitmap from the render region. 
	KernelBitmap* pKernelBitmap = new KernelBitmap(pRenderRegion->ExtractBitmap(), FALSE);

	BitmapDragInformation oDragBitmap(pKernelBitmap, szBitmap.x, szBitmap.y, 0, 0);
	oDragBitmap.OnDrawSolidDrag(wxPoint(0, 0), &dcMem);

	// Free resourced and return the resulting bitmap.
	delete pKernelBitmap;
	delete pRenderRegion;
	delete pDialogView;

	dcMem.SelectObject(wxNullBitmap);

	return pBitmap;
}


/******************************************************************************
Function  : CBGDDCachedItem::RenderItemToGRenderRegion
Author    : Mikhail Tatarnikov
Purpose   : Renders the item into GRenderRegion.
Returns   : void
Exceptions: 
Parameters: [in] GRenderRegion* pRenderRegion - Render region to draw to;
            [in] DocRect		drcItem		  - item size.
Notes     : The derived classes should override this function if they don't
			override RenderItemToBitmap.
******************************************************************************/
void CBGDDCachedItem::RenderItemToGRenderRegion(GRenderRegion* pRenderRegion, DocRect drcItem) const
{
	ASSERT(FALSE);	// The derived classes should override this method
					// Or the parent RenderItemToBitmap method.
}






CBGDDKernelBitmapItem::CBGDDKernelBitmapItem(KernelBitmap* pKernelBitmap, BOOL bAutodelete,
											 String_256 strLabel, BOOL bStretch)
	: CBGDDCachedItem(strLabel)
{
	m_pKernelBitmap		  = pKernelBitmap;
	m_bDeleteKernelBitmap = bAutodelete;
	m_bStretch			  = bStretch;
}

CBGDDKernelBitmapItem::~CBGDDKernelBitmapItem()
{
	if (m_bDeleteKernelBitmap)
		delete m_pKernelBitmap;
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
wxBitmap* CBGDDKernelBitmapItem::RenderItemToBitmap(wxSize szBitmap) const
{
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

	return pBitmap;
}








CBGDDStrokeItem::CBGDDStrokeItem(LineAttrItem* plaiStroke, BOOL bAutodelete, String_256 strLabel)
	: CBGDDCachedItem(strLabel)
{
	m_plaiStroke = plaiStroke;
	m_bDelete	 = bAutodelete;
}

CBGDDStrokeItem::~CBGDDStrokeItem()
{
	if (m_bDelete)
		delete m_plaiStroke;
}


/******************************************************************************
Function  : CBGDDStrokeItem::RenderItemToGRenderRegion
Author    : Mikhail Tatarnikov
Purpose   : Renders the stroke into GRenderRegion.
Returns   : void
Exceptions: 
Parameters: [in] GRenderRegion* pRenderRegion - Render region to draw to;
            [in] DocRect		drcItem		  - item size.
Notes     : 
******************************************************************************/
void CBGDDStrokeItem::RenderItemToGRenderRegion(GRenderRegion* pRenderRegion, DocRect drcItem) const
{
	// Render the stroke.
	m_plaiStroke->Render(pRenderRegion, drcItem, LineAttrItem::NO_LABEL);
}





CBGDDBrushItem::CBGDDBrushItem(AttrBrushType* pabtBrush, BOOL bAutodelete, String_256 strLabel)
	: CBGDDCachedItem(strLabel)
{
	m_pabtBrush  = pabtBrush;
	m_bDelete	 = bAutodelete;
}

CBGDDBrushItem::~CBGDDBrushItem()
{
	if (m_bDelete)
		delete m_pabtBrush;
}


/******************************************************************************
Function  : CBGDDBrushItem::RenderItemToGRenderRegion
Author    : Mikhail Tatarnikov
Purpose   : Renders the brush into GRenderRegion.
Returns   : void
Exceptions: 
Parameters: [in] GRenderRegion* pRenderRegion - Render region to draw to;
            [in] DocRect		drcItem		  - item size.
Notes     : 
******************************************************************************/
void CBGDDBrushItem::RenderItemToGRenderRegion(GRenderRegion* pRenderRegion, DocRect drcItem) const
{
	Path  pthBrush;

	pthBrush.Initialise(8);
	pthBrush.IsFilled  =  FALSE;
	pthBrush.IsStroked = TRUE;
	pthBrush.FindStartOfPath();

	// avoid overhead of allocation each time ....
	INT32 iWidth	  = drcItem.Width();
	INT32 iHeight	  = drcItem.Height();
	INT32 iBorderX	  = (INT32)(0.025 * iWidth);
//	INT32 iBorderY	  = (INT32)(0.025 * iHeight);
//	INT32 iHalfWidth  = iWidth  / 2;
	INT32 iHalfHeight = iHeight / 2;

	DocCoord dcrdStartPoint(iBorderX, iHalfHeight);
	DocCoord dcrdEndPoint(iWidth - iBorderX, iHalfHeight);

	pthBrush.InsertMoveTo(dcrdStartPoint);
	pthBrush.InsertLineTo(dcrdEndPoint);


	pRenderRegion->SetLineWidth(iHeight / 3);

	PathProcessorBrush* pPathProc = m_pabtBrush->GetPathProcessor();
	pPathProc->ScaleToValue(1);
	pRenderRegion->SetLineColour(COLOUR_BLACK);

	m_pabtBrush->Render(pRenderRegion);
	pRenderRegion->DrawPath (&pthBrush);
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
	CBGDDBrushItem* pBrushItem = new CBGDDBrushItem(pabtBrush, bDelete, strLabel);
	CGridDropDown::AddItem(pBrushItem);
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

	if (pItemData)
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
