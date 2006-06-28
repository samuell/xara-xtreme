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

/*
*/

#include "camtypes.h"
#include "xpfrgn.h"

//#include "spread.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "xpfilter.h"
#include "xpfcaps.h"
//#include "rrcaps.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "insertnd.h"
//#include "becomea.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "opcntr.h"
#include "lineattr.h"
#include "strkattr.h"
#include "brshattr.h"
#include "vstate.h"
#include "nodepath.h"
#include "nodetxts.h"
#include "nodetext.h"
#include "nodetxtl.h"
#include "grndbmp.h"

// The implements to match the declares in the .h file.

CC_IMPLEMENT_DYNAMIC(SpanListItem, ListItem);
CC_IMPLEMENT_DYNAMIC(NodeThreeBoolListItem, ListItem);
CC_IMPLEMENT_DYNAMIC(XPFRenderRegion, RenderRegion);
CC_IMPLEMENT_DYNAMIC(XPFView, View);

// Proper memory dumps
#define new CAM_DEBUG_NEW




/********************************************************************************************

>	XPFRenderRegion::XPFRenderRegion(PluginNativeFilter* pFilter, 
										CapabilityTree* pPlugCaps, 
										CommonTransInfo* pTransInfo)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/4/95
	Purpose:	The Scanning Render Regions constructor. This sets all the member vars of
				the class to default values (usually 0 or NULL).

********************************************************************************************/

XPFRenderRegion::XPFRenderRegion(PluginNativeFilter* pFilter, CapabilityTree* pPlugCaps, CommonTransInfo* pTransInfo)
{
	m_pFilter = pFilter;
	m_PixelsPerInch = pPlugCaps ? pPlugCaps->GetRasteriseDPI() : 96.0;
	m_pTransInfo = pTransInfo;
}



/********************************************************************************************

>	XPFRenderRegion::~XPFRenderRegion()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/4/95
	Purpose:	Destructor - It will do something if it needs to but at present this function
				is empty

********************************************************************************************/

XPFRenderRegion::~XPFRenderRegion()
{
}


BOOL XPFRenderRegion::AttachDevice(View* ViewToAttach, CNativeDC* DCToAttach,
								Spread* SpreadToAttach)
{
	BOOL ok = RenderRegion::AttachDevice(ViewToAttach, DCToAttach, SpreadToAttach);
	if (ok)
	{
		

	}

	return(ok);
}



/********************************************************************************************

>	virtual BOOL XPFRenderRegion::StartRender()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/4/95
	Returns:	TRUE if everything went OK, FALSE if there was a problem
	Purpose:	Called to start the rendering process. Allows the render region to get
				ready to start rendering
	SeeAlso:	XPFRenderRegion::StopRender

********************************************************************************************/

BOOL XPFRenderRegion::StartRender()
{
	// Call base class first
	if (!RenderRegion::StartRender())
		return FALSE;

	// All worked
	return TRUE;
}


/********************************************************************************************

>	virtual BOOL XPFRenderRegion::StopRender()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/4/95
	Purpose:	Called to stop rendering.

********************************************************************************************/

BOOL XPFRenderRegion::StopRender()
{
	return(TRUE);
}



/********************************************************************************************

>	virtual void XPFRenderRegion::DrawPathToOutputDevice(Path* pPathToRender)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/4/95
	Inputs:		pPathToRender - The path that we want to try and render
	Purpose:	There is no actual rendering, as this class does not actually render anything
				to a device.

********************************************************************************************/

void XPFRenderRegion::DrawPathToOutputDevice(Path* pPathToRender, PathShape)
{
	if (m_pTransInfo && m_pTransInfo->IsCommonType())
	{
		// Path rendering happens in two parts, the fill and the stroke
		// so these must be checked separately

		if (pPathToRender->IsFilled)
		{
			// Get the fill attribute
			FillGeometryAttribute* pFillAttr = (FillGeometryAttribute*) CurrentAttrs[ATTR_FILLGEOMETRY].pAttr;
			
			// If it is not a no-colour flat fill then

			if (!IS_A(pFillAttr, FlatFillAttribute) || !(RR_FILLCOLOUR().IsTransparent()))
			{
				// Get the current Transparency Fill Geometry
				TranspFillAttribute* pTransAttr = RR_FILLTRANSP();

				// And update the common type object
				m_pTransInfo->UpdateCommonType(pTransAttr->GetTranspType());
			}
		}

		if (pPathToRender->IsStroked)
		{
			// If the stroke colour is not transparent
			if (!(RR_STROKECOLOUR().IsTransparent()))
			{
				// Get the current Transparency Fill Geometry
				StrokeTranspAttribute* pTransAttr = RR_STROKETRANSP();

				// And update the common type object
				m_pTransInfo->UpdateCommonType(pTransAttr->GetTranspType());
			}
		}
	}
}


/********************************************************************************************

>	virtual void XPFRenderRegion::DrawRect(DocRect* pRectToRender)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/4/95
	Inputs:		pRectToRender - The Rectangle that we need to render
	Purpose:	There is no actual rendering, as this class does not actually render anything
				to a device.

********************************************************************************************/

void XPFRenderRegion::DrawRect(DocRect* pRectToRender)
{
}


/********************************************************************************************

>	virtual void XPFRenderRegion::DrawLine(const DocCoord &StartPoint,
												const DocCoord &EndPoint)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/4/95
	Inputs:		StartPoint, EndPoint - the coords to draw a line between
	Purpose:	There is no actual rendering, as this class does not actually render anything
				to a device.

********************************************************************************************/

void XPFRenderRegion::DrawLine(const DocCoord &StartPoint, const DocCoord &EndPoint)
{
}


/********************************************************************************************

>	virtual void XPFRenderRegion::DrawPixel(const DocCoord &Point)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/4/95
	Inputs:		Point - the coord to draw the pixel in
	Purpose:	There is no actual rendering, as this class does not actually render anything
				to a device.

********************************************************************************************/

void XPFRenderRegion::DrawPixel(const DocCoord &Point)
{
}


/********************************************************************************************

>	virtual void XPFRenderRegion::DrawBitmap(const DocCoord &Point, KernelBitmap* pBitmap)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/4/95
	Inputs:		Point - The position to draw the bitmap at
				pBitmap - the bitmap that needs drawing
	Purpose:	There is no actual rendering, as this class does not actually render anything
				to a device.

********************************************************************************************/

void XPFRenderRegion::DrawBitmap(const DocCoord &Point, KernelBitmap* pBitmap)
{
}


/********************************************************************************************

>	virtual void XPFRenderRegion::DrawBitmap(const DocCoord &Point, UINT32 BitmapID,
												UINT32 ToolID)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/4/95
	Inputs:		Point - the position to draw the bitmap at
				BitmapID - The ID of the bitmap to render
				ToolID - the ID of the tool that the bitmap belongs to
	Purpose:	There is no actual rendering, as this class does not actually render anything
				to a device.

********************************************************************************************/

void XPFRenderRegion::DrawBitmap(const DocCoord &Point, UINT32 BitmapID, UINT32 ToolID)
{
}


/********************************************************************************************

>	BOOL XPFRenderRegion::DrawTransformedBitmap(NodeBitmap *pNodeBitmap)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/7/95
	Inputs:		pNodeBitmap - The bitmap to render
	Returns:	TRUE if it worked, FLASE if not
	Purpose:	Works out if it can draw a Transformed bitmap.

********************************************************************************************/

BOOL XPFRenderRegion::DrawTransformedBitmap(NodeBitmap *pNodeBitmap)
{
	return TRUE;
}


/********************************************************************************************

>	virtual void XPFRenderRegion::DrawDragRect(DocRect *RectToRender)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/4/95
	Inputs:		RectToRender - The Rectangle
	Purpose:	None - I just had to overide this function as it is pure virtual. Does nothing

********************************************************************************************/

void XPFRenderRegion::DrawDragRect(DocRect *RectToRender)
{
}


/********************************************************************************************

>	virtual void XPFRenderRegion::DrawBlob(DocCoord p, BlobType type)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/4/95
	Inputs:		p - A coord
				type - The type of blob
	Purpose:	None - I just had to overide this function as it is pure virtual. Does nothing

********************************************************************************************/

void XPFRenderRegion::DrawBlob(DocCoord p, BlobType type)
{
}


/********************************************************************************************

>	virtual void XPFRenderRegion::DrawCross(const DocCoord &Point, const UINT32 Size)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/4/95
	Inputs:		Point - a Coord
				Size - the size of the cross
	Purpose:	None - I just had to overide this function as it is pure virtual. Does nothing

********************************************************************************************/

void XPFRenderRegion::DrawCross(const DocCoord &Point, const UINT32 Size)
{
}


/********************************************************************************************

>	virtual void XPFRenderRegion::DrawBitmapBlob(const DocCoord &Point,
													KernelBitmap* BlobShape)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/4/95
	Inputs:		Point - A Coord
				BlobShape - the bitmap to use
	Purpose:	None - I just had to overide this function as it is pure virtual. Does nothing

********************************************************************************************/

void XPFRenderRegion::DrawBitmapBlob(const DocCoord &Point, KernelBitmap* BlobShape)
{
}


/********************************************************************************************

>	virtual void XPFRenderRegion::DrawBitmapBlob(const DocCoord &Point, ResourceID resID)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/4/95
	Inputs:		Point - A coord
				ResourceID - the ID of a bitmap
	Purpose:	None - I just had to overide this function as it is pure virtual. Does nothing

********************************************************************************************/

void XPFRenderRegion::DrawBitmapBlob(const DocCoord &Point, ResourceID resID)
{
}



/********************************************************************************************

>	virtual void XPFRenderRegion::GetRenderRegionCaps(RRCaps* pCaps)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/4/95
	Outputs:	pCaps - The details about what types of thing this render region can render
	Purpose:	This function allows render regions to admit to what they can and can not
				render. This allows other areas of the program to come in and help render
				regions out in some situations, if they are unable to render everything.
				eg. an OSRenderRegion can not render transparancy.

********************************************************************************************/

void XPFRenderRegion::GetRenderRegionCaps(RRCaps* pCaps)
{
	pCaps->CanDoAll();
}



/********************************************************************************************

>	virtual void XPFRenderRegion::InitClipping()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/4/95
	Purpose:	Sets up the clipping - does nothing in this class

********************************************************************************************/

void XPFRenderRegion::InitClipping()
{
}


/********************************************************************************************

>	virtual void XPFRenderRegion::InitAttributes()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/4/95
	Purpose:	Sets up the attributes. Does nothing in this class

********************************************************************************************/

void XPFRenderRegion::InitAttributes()
{
}


/********************************************************************************************

>	virtual void XPFRenderRegion::SetLineAttributes()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/4/95
	Purpose:	Does nothing in this class

********************************************************************************************/

void XPFRenderRegion::SetLineAttributes()
{
}


/********************************************************************************************

>	virtual void XPFRenderRegion::SetOSDrawingMode()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/4/95
	Purpose:	Does nothing in this class

********************************************************************************************/

void XPFRenderRegion::SetOSDrawingMode()
{
}


/********************************************************************************************

>	virtual void XPFRenderRegion::SetQualityLevel()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/4/95
	Purpose:	Does nothing in this class

********************************************************************************************/

void XPFRenderRegion::SetQualityLevel()
{
}


/********************************************************************************************

>	virtual MILLIPOINT XPFRenderRegion::CalcPixelWidth()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/4/95
	Purpose:	Does nothing in this class

********************************************************************************************/

MILLIPOINT XPFRenderRegion::CalcPixelWidth()
{
	return MILLIPOINT( 72000.0 / m_PixelsPerInch);
}


/********************************************************************************************

>	virtual MILLIPOINT XPFRenderRegion::CalcScaledPixelWidth()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/4/95
	Purpose:	Does nothing in this class

********************************************************************************************/

MILLIPOINT XPFRenderRegion::CalcScaledPixelWidth()
{
	const double dpi = 72000.0 / m_PixelsPerInch;

	return (MILLIPOINT)( (dpi / ScaleFactor.MakeDouble()) + 0.5 );
}





/****************************************************************************

>	XPFRenderCallback::XPFRenderCallback(PluginNativeFilter* pFilter, XPFRenderRegion* pXPFRegion, CapabilityTree* pCapTree, INT32 ConvertPass)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/10/2005

	Inputs:		pFilter		- pointer to a PluginNativeFilter
				pXPFRegion	- pointer to a XPFRenderRegion
				pCapTree	- pointer to a CapabilityTree
				ConvertPass	- conversion pass
	Purpose:	Constructs an XPFRenderCallback object to control the 
				rendering loop for each phase of the conversion mechanism

****************************************************************************/

XPFRenderCallback::XPFRenderCallback(PluginNativeFilter* pFilter, XPFRenderRegion* pXPFRegion, CapabilityTree* pCapTree, INT32 ConvertPass)
{
	m_pFilter = pFilter;
	m_pXPFRegion = pXPFRegion;
	m_pCapTree = pCapTree;
	m_ConvertPass = ConvertPass;
	m_pNewSpread = NULL;
	m_pContextNode = NULL;
	m_Direction = FIRSTCHILD;
	m_pSpanParent = NULL;
}



/****************************************************************************

>	XPFRenderCallback::~XPFRenderCallback()

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/10/2005

	Purpose:	Destructor

****************************************************************************/

XPFRenderCallback::~XPFRenderCallback()
{
	m_ConvertList.DeleteAll();
	m_ParentList.DeleteAll();
}



/****************************************************************************

>	BOOL XPFRenderCallback::BeforeNode(RenderRegion* pRegion, Node* pNode)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/10/2005

	Inputs:		pRegion		- pointer to a RenderRegion
				pNode		- pointer to a Node
	Returns:	TRUE if ok, FALSE if bother
	Purpose:	

****************************************************************************/

BOOL XPFRenderCallback::BeforeNode(RenderRegion* pRegion, Node* pNode)
{
//	TRACEUSER( "Gerry", _T("XPFRC# BeforeNode    0x%08x - %s\n"), pNode, pNode->GetRuntimeClass()->m_lpszClassName);

	if (pNode->IsNodeHidden() || pNode->IsAnInsertionNode())
		return(FALSE);

	// Move code for each pass into separate functions
	switch (m_ConvertPass)
	{
		case 1:
		{
			// All of the children of pNode have been passed to BeforeSubtree and BeforeNode
			if (m_Direction == FIRSTCHILD)
			{
//				TRACEUSER( "Gerry", _T("XPFRC# Was child, attach next node as sibling of %s\n"), m_pContextNode->GetRuntimeClass()->m_lpszClassName);
				// If the next node will be attached as first child then set it to attach as
				// as next sibling instead
				m_Direction = NEXT;
			}
			else if (m_Direction == NEXT)
			{
				// If already set to next sibling then set the context node to its parent
				m_pContextNode = m_pContextNode->FindParent();
//				TRACEUSER( "Gerry", _T("XPFRC# Was next, attach next node as sibling of %s\n"), m_pContextNode->GetRuntimeClass()->m_lpszClassName);
			}
		}
		break;

		case 2:
		{
			// If the node is a renderable object but not a compound then get the conversion type for pass 2
			if (pNode->IsAnObject() && !pNode->IsCompound())
			{
				// Get the convert type of this node
				XPFConvertType ConvertType = m_pCapTree->GetConvertTypePass2(pNode, m_pXPFRegion);
				
				// If it specifies stroked then set a pending stroked conversion for this node
				if (ConvertType == XPFCONVTYPE_STROKED)
				{
//					TRACEUSER( "Gerry", _T("XPFRC# Setting pending stroked conversion for %s\n"), pNode->GetRuntimeClass()->m_lpszClassName);
					NodeListItem* pItem = new NodeListItem(pNode);
					m_ConvertList.AddTail(pItem);
				}
			}
		}
		break;

		case 3:
		{
			// If the node is a renderable object then get the conversion type for pass 2
			// This used to also check for non-compund nodes but that breaks group transparency
			// It may be necessary to modify GetConvertTypePass3 to correctly distinguish
			// compound objects that can and can't support group transparency
			if (pNode->IsAnObject())
			{
				// Get the convert type of this node
				BOOL bFill = FALSE;
				BOOL bTrans = FALSE;
				BOOL bFillTrans = FALSE;
				m_pCapTree->GetConvertTypePass3(pNode, m_pXPFRegion, &bFill, &bTrans, &bFillTrans);
				
				// If it specifies bitmapfill then set a pending bitmapfill conversion for this node
				if (bFill || bTrans || bFillTrans)
				{
//					TRACEUSER( "Gerry", _T("XPFRC# Setting pending bitmapfill conversion for %s\n"), pNode->GetRuntimeClass()->m_lpszClassName);
					NodeThreeBoolListItem* pItem = new NodeThreeBoolListItem(pNode, bFill, bTrans, bFillTrans);
					m_ConvertList.AddTail(pItem);
				}
			}
		}
		break;

		case 4:
		{
			// If the node is a renderable object but not a compound then get the conversion type for pass 4
			if (pNode->IsAnObject())
			{
				// Get the convert type of this node
				XPFConvertType ConvertType = m_pCapTree->GetConvertTypePass4(pNode, m_pXPFRegion);
				
				// If it specifies bitmap then add the node to the convert list
				if (ConvertType == XPFCONVTYPE_BITMAP)
				{
//					TRACEUSER( "Gerry", _T("XPFRC# Setting pending bitmap conversion for %s\n"), pNode->GetRuntimeClass()->m_lpszClassName);
					NodeListItem* pItem = new NodeListItem(pNode);
					m_ConvertList.AddTail(pItem);
				}
			}
		}
		break;

		case 5:
		{
			// If the node is a renderable object then get the conversion type for pass 5
			if (pNode->IsAnObject())
			{
				// Get the parent
				Node* pParent = pNode->FindParent();

				// Get the convert type of this node and
				// update the span list appropriately
				XPFConvertType ConvertType = m_pCapTree->GetConvertTypePass5(pNode, m_pXPFRegion);

				// If this node has children
				if (pNode->FindFirstChild())
				{
					// If this node should be converted
					if (ConvertType == XPFCONVTYPE_BITMAPSPAN)
					{
						// Remove all items from ConvertList that refer to this as the parent
						while (TRUE)
						{
							SpanListItem* pChildSpanItem = (SpanListItem*)m_ConvertList.GetTail();
							if (pChildSpanItem && pChildSpanItem->m_pFirstNode && 
								pChildSpanItem->m_pFirstNode->FindParent() == pNode)
							{
//								TRACEUSER( "Gerry", _T("XPFRC# Removing span item for 0x%08x (%s)\n"), pChildSpanItem->m_pFirstNode, pChildSpanItem->m_pFirstNode->GetRuntimeClass()->m_lpszClassName);
								delete m_ConvertList.RemoveTail();
							}
							else
							{
								break;
							}
						}
					}
					
					// Get the last item from the parent list
					NodeThreeBoolListItem* pItem = (NodeThreeBoolListItem*)m_ParentList.RemoveTail();
//					TRACEUSER( "Gerry", _T("XPFRC# Removed parent list item for 0x%08x (%s)\n"), pItem->m_pNode, pItem->m_pNode->GetRuntimeClass()->m_lpszClassName);
					if (pItem)
					{
						// Check to make sure it is the correct one
						if (pItem->m_pNode == pNode)
						{
							// If all the children were complex then
							// we delete the last complex span of the children 
							// and force this node to be treated as complex
							if (pItem->m_bSecond && !(pItem->m_bFirst))
							{
								SpanListItem* pChildSpanItem = (SpanListItem*)m_ConvertList.GetTail();
								if (pChildSpanItem && pChildSpanItem->m_pFirstNode &&
									pChildSpanItem->m_pFirstNode->FindParent() == pNode)
								{
//									TRACEUSER( "Gerry", _T("XPFRC# Removing span item for 0x%08x (%s)\n"), pChildSpanItem->m_pFirstNode, pChildSpanItem->m_pFirstNode->GetRuntimeClass()->m_lpszClassName);
									delete m_ConvertList.RemoveTail();
								}
								// We must reset the parent of the current span here
								SpanListItem* pCurItem = (SpanListItem*)m_ConvertList.GetTail();
								// If the last node in the last span was the previous one
								if (pCurItem && pCurItem->m_pLastNode == pNode->FindPrevious())
								{
									// Then we are still in the span at the moment
									m_pSpanParent = pCurItem->m_pLastNode->FindParent();
								}
								else
								{
									// Otherwise we aren't in a span at the moment
									m_pSpanParent = NULL;
								}
//								TRACEUSER( "Gerry", _T("XPFRC# Current span parent set to 0x%08x (%s)\n", m_pSpanParent, m_pSpanParent?m_pSpanParent->GetRuntimeClass()->m_lpszClassName:"NULL"));
								ConvertType = XPFCONVTYPE_BITMAPSPAN;
							}
						}
						delete pItem;
					}
				}

				// If this node should be in a span
				if (ConvertType == XPFCONVTYPE_BITMAPSPAN)
				{
					SpanListItem* pItem = NULL;
					// If we are currently in a span and the parent is the same
					if (m_pSpanParent == pParent)
					{
//						TRACEUSER( "Gerry", _T("XPFRC# Adding to current span 0x%08x (%s)\n"), pNode, pNode->GetRuntimeClass()->m_lpszClassName);
						// Make this node the last node in the current span
						pItem = (SpanListItem*)m_ConvertList.GetTail();
						if (pItem)
							pItem->m_pLastNode = pNode;
					}
					else
					{
//						TRACEUSER( "Gerry", _T("XPFRC# Starting new span with 0x%08x (%s)\n"), pNode, pNode->GetRuntimeClass()->m_lpszClassName);
						// Make this node the first node in a new span
						pItem = new SpanListItem(pNode);
						m_ConvertList.AddTail(pItem);
						m_pSpanParent = pParent;
					}

					// If this span doesn't have non-alpha transparency yet
					if (!pItem->m_bNonAlphaTrans)
					{
						// Check the current attributes in the render region for
						// non-alpha transparency types
						StrokeTranspAttribute* pStroke = (StrokeTranspAttribute*)(pRegion->GetCurrentAttribute(ATTR_STROKETRANSP));
						UINT32 Type = pStroke->GetTranspType();
						if (Type != TT_NoTranspType &&
							Type != TT_Mix &&
							Type != TT_DARKEN &&
							Type != TT_LIGHTEN &&
							Type != TT_BRIGHTNESS &&
							Type != TT_BEVEL)
						{
//							TRACEUSER( "Gerry", _T("XPFRC# Found non-alpha stroke transparency\n"));
							pItem->m_bNonAlphaTrans = TRUE;
						}
						
						if (!pItem->m_bNonAlphaTrans)
						{
							TranspFillAttribute* pFill = (TranspFillAttribute*)(pRegion->GetCurrentAttribute(ATTR_TRANSPFILLGEOMETRY));
							UINT32 Type = pFill->GetTranspType();
							if (Type != TT_NoTranspType &&
								Type != TT_Mix &&
								Type != TT_DARKEN &&
								Type != TT_LIGHTEN &&
								Type != TT_BRIGHTNESS &&
								Type != TT_BEVEL)
							{
//								TRACEUSER( "Gerry", _T("XPFRC# Found non-alpha fill transparency\n"));
								pItem->m_bNonAlphaTrans = TRUE;
							}
						}
					}
				}
				else
				{
//					TRACEUSER( "Gerry", _T("XPFRC# Stopping current span\n"));
					// Stop the current span
					m_pSpanParent = NULL;
				}

				NodeThreeBoolListItem* pParentItem = (NodeThreeBoolListItem*)m_ParentList.GetTail();
				if (pParentItem)
				{
					if (ConvertType == XPFCONVTYPE_BITMAPSPAN)
						pParentItem->m_bSecond = TRUE;
					else
						pParentItem->m_bFirst = TRUE;
				}
			}
		}
		break;

		default:
		{
			TRACEUSER( "Gerry", _T("Unimplemented Pass (%d)\n"), m_ConvertPass);
		}
		break;
	}
	
	return(TRUE);
}



/****************************************************************************

>	BOOL XPFRenderCallback::BeforeSubtree(RenderRegion* pRegion, Node* pNode, Node** ppNextNode, BOOL bClip, SubtreeRenderState* pState)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/10/2005

	Inputs:		pRegion		- pointer to a RenderRegion
				pNode		- pointer to a Node
				ppNextNode	- pointer to a pointer to a Node
				bClip		- 
				pState		- pointer to a SubtreeRenderState
	Returns:	TRUE if ok, FALSE if bother
	Purpose:	

****************************************************************************/

BOOL XPFRenderCallback::BeforeSubtree(RenderRegion* pRegion, Node* pNode, Node** ppNextNode, BOOL bClip, SubtreeRenderState* pState)
{
//	if (pNode->FindFirstChild())
//	{
//		TRACEUSER( "Gerry", _T("XPFRC# BeforeSubtree 0x%08x - %s\n"), pNode, pNode->GetRuntimeClass()->m_lpszClassName);
//	}

	if (pNode->IsNodeHidden() || pNode->IsAnInsertionNode())
		return(FALSE);

	BOOL ok;
	BOOL bSetState = TRUE;
	SubtreeRenderState RenderState = SUBTREE_ROOTANDCHILDREN;

	// Move code for each pass into separate functions
	switch (m_ConvertPass)
	{
		case 1:
		{
			// If we are doing selection only, this node is an object 
			// and this node is not selected and doesn't have a selected 
			// parent or child then skip it
			if (m_pCapTree->GetSelection() && pNode->IsAnObject() && 
				!(pNode->IsChildOfSelected() || pNode->HasSelectedChildren()))
			{
				RenderState = SUBTREE_NORENDER;
				bSetState = TRUE;
			}
			else
			{
				// This pass can handle native, simple, bitmap, reformat and remove conversions
				// Create a copy of the tree doing the relevant conversions on-the-fly
				
				// Get the convert type of this node
				XPFConvertType ConvertType = m_pCapTree->GetConvertTypePass1(pNode, m_pXPFRegion);

				if (ConvertType == XPFCONVTYPE_NATIVE)
				{
					// If it is native then:
					//	create a shallow copy of the node
					//	attach it at the current context position
					//	update the context to the first child of this node
					//	continue the render into the subtree

					Node* pNodeToAttach = pNode->PublicCopy();
					// TODOG: Check for NULL

					// If we have a context node then attach to it otherwise set m_pNewSpread
					if (m_pContextNode)
					{
	/*					if (m_Direction == FIRSTCHILD)
						{
							TRACEUSER( "Gerry", _T("XPFRC# Attaching %s as child of %s\n"), pNodeToAttach->GetRuntimeClass()->m_lpszClassName, m_pContextNode->GetRuntimeClass()->m_lpszClassName);
						}
						else if (m_Direction == LASTCHILD)
						{
							TRACEUSER( "Gerry", _T("XPFRC# Attaching %s as last child of %s\n"), pNodeToAttach->GetRuntimeClass()->m_lpszClassName, m_pContextNode->GetRuntimeClass()->m_lpszClassName);
						}
						else if (m_Direction == NEXT)
						{
							TRACEUSER( "Gerry", _T("XPFRC# Attaching %s as sibling of %s\n"), pNodeToAttach->GetRuntimeClass()->m_lpszClassName, m_pContextNode->GetRuntimeClass()->m_lpszClassName);
						}*/
						pNodeToAttach->AttachNode(m_pContextNode, m_Direction);
					}

					// Update context node
					m_pContextNode = pNodeToAttach;
					m_Direction = FIRSTCHILD;
				}
				else if (ConvertType == XPFCONVTYPE_SIMPLE)
				{
					// If it is simple then:
					//	create a deep copy of the node
					//	attach it at the current context position
					//	convert it to editable shapes (become a path)
					//	update the context to the next sibling
					//  skip the render of this subtree

					TRACEUSER( "Gerry", _T("XPFRC# Converting %s to simple\n"), pNode->GetRuntimeClass()->m_lpszClassName);
					// Create a deep copy of this subtree
					Node* pNodeToAttach = NULL;
					ok = pNode->NodeCopy(&pNodeToAttach);
					if (ok && pNodeToAttach)
					{
						// Attach the copy into the output tree
	/*					if (m_Direction == FIRSTCHILD)
						{
							TRACEUSER( "Gerry", _T("XPFRC# Attaching %s as child of %s\n"), pNodeToAttach->GetRuntimeClass()->m_lpszClassName, m_pContextNode->GetRuntimeClass()->m_lpszClassName);
						}
						else if (m_Direction == LASTCHILD)
						{
							TRACEUSER( "Gerry", _T("XPFRC# Attaching %s as last child of %s\n"), pNodeToAttach->GetRuntimeClass()->m_lpszClassName, m_pContextNode->GetRuntimeClass()->m_lpszClassName);
						}
						else if (m_Direction == NEXT)
						{
							TRACEUSER( "Gerry", _T("XPFRC# Attaching %s as sibling of %s\n"), pNodeToAttach->GetRuntimeClass()->m_lpszClassName, m_pContextNode->GetRuntimeClass()->m_lpszClassName);
						}*/
						pNodeToAttach->AttachNode(m_pContextNode, m_Direction);

						// We must convert this subtree to simple shapes
						BecomeA BecomeAPath(BECOMEA_REPLACE, CC_RUNTIME_CLASS(NodePath), NULL);
						BecomeAPath.SetResultsStayInPlace(TRUE);
						ok = pNodeToAttach->DoBecomeA(&BecomeAPath);
	//					TRACEUSER( "Gerry", _T("BecomeAPath returned %s\n", ok ? "true" : "false"));

						if (m_Direction == NEXT)
							m_pContextNode = m_pContextNode->FindNext();
						else
							m_pContextNode = m_pContextNode->FindFirstChild();
						m_Direction = NEXT;

						// And we must skip this subtree
						RenderState = SUBTREE_NORENDER;
						bSetState = TRUE;
					}
				}
				else if (ConvertType == XPFCONVTYPE_BITMAP)
				{
					// If it is bitmap then:
					//	render the node into a new bitmap node
					//	attach it at the current context position
					//	update the context to the next sibling
					//  skip the render of this subtree

					BOOL bNonAlphaTrans = DoesNodeUseNonAlphaTrans(pNode);
					TRACEUSER("Gerry", _T("XPFRC# Converting %s to bitmap (%s)\n"), pNode->GetRuntimeClass()->m_lpszClassName, bNonAlphaTrans ? _T("NonAlpha") : _T("Alpha"));

					Node* pNodeToAttach = RenderNodesToBitmap(pNode, pNode, bNonAlphaTrans);
					if (pNodeToAttach)
					{
						// Attach the new node into the output tree
	/*					if (m_Direction == FIRSTCHILD)
						{
							TRACEUSER( "Gerry", _T("XPFRC# Attaching %s as child of %s\n"), pNodeToAttach->GetRuntimeClass()->m_lpszClassName, m_pContextNode->GetRuntimeClass()->m_lpszClassName);
						}
						else if (m_Direction == LASTCHILD)
						{
							TRACEUSER( "Gerry", _T("XPFRC# Attaching %s as last child of %s\n"), pNodeToAttach->GetRuntimeClass()->m_lpszClassName, m_pContextNode->GetRuntimeClass()->m_lpszClassName);
						}
						else if (m_Direction == NEXT)
						{
							TRACEUSER( "Gerry", _T("XPFRC# Attaching %s as sibling of %s\n"), pNodeToAttach->GetRuntimeClass()->m_lpszClassName, m_pContextNode->GetRuntimeClass()->m_lpszClassName);
						}*/
						pNodeToAttach->AttachNode(m_pContextNode, m_Direction);

						// The next node must be the NEXT sibling of this one
						m_pContextNode = pNodeToAttach;
						m_Direction = NEXT;
					}		
					
					RenderState = SUBTREE_NORENDER;
					bSetState = TRUE;
				}
				else if (ConvertType == XPFCONVTYPE_REFORMAT)
				{
					// If it is reformat then:
					//	create a reformatted copy of the text story
					//	attach it at the current context position
					//	update the context to the next sibling
					//  skip the render of this subtree

					TRACEUSER( "Gerry", _T("XPFRC# Reformatting %s\n"), pNode->GetRuntimeClass()->m_lpszClassName);

					Node* pNodeToAttach = NULL;
					if (IS_A(pNode, TextStory))
					{
						// Reformat the copy of the story
						TextStory* pStory = (TextStory*)pNode;
						pNodeToAttach = ReformatTextStory(pStory);
						if (pNodeToAttach)
						{
							pNodeToAttach->AttachNode(m_pContextNode, m_Direction);

							// Update context node
							m_pContextNode = pNodeToAttach;
							m_Direction = NEXT;
						}
					}

					RenderState = SUBTREE_NORENDER;
					bSetState = TRUE;
				}
				else if (ConvertType == XPFCONVTYPE_REMOVE)
				{
					// If it is remove then:
					//  skip the render of this subtree

//					TRACEUSER( "Gerry", _T("XPFRC# Ignoring %s\n"), pNode->GetRuntimeClass()->m_lpszClassName);
					RenderState = SUBTREE_NORENDER;
					bSetState = TRUE;
				}
			}
		}
		break;

		case 2:
		{
			// Don't do anything in here for now
		}
		break;

		case 3:
		{
			// Don't do anything in here for now
		}
		break;

		case 4:
		{
			// Get the convert type of this node
			XPFConvertType ConvertType = m_pCapTree->GetConvertTypePass4(pNode, m_pXPFRegion);
			// If it specifies bitmap then add the node to the convert list
			if (ConvertType == XPFCONVTYPE_BITMAP)
			{
				TRACEUSER( "Gerry", _T("XPFRC# Setting pending bitmap conversion for %s\n"), pNode->GetRuntimeClass()->m_lpszClassName);
				NodeListItem* pItem = new NodeListItem(pNode);
				m_ConvertList.AddTail(pItem);
				RenderState = SUBTREE_NORENDER;
				bSetState = TRUE;
			}
		}
		break;

		case 5:
		{
			// If the node is a renderable object and has children
			if (pNode->IsAnObject() && pNode->FindFirstChild())
			{
//				TRACEUSER( "Gerry", _T("XPFRC# Creating parent list item for 0x%08x (%s)\n"), pNode, pNode->GetRuntimeClass()->m_lpszClassName);
				// Create a new NodeThreeBoolListItem for this node
				NodeThreeBoolListItem* pItem = new NodeThreeBoolListItem(pNode);
				m_ParentList.AddTail(pItem);
			}
		}
		break;
		
		default:
		{
			TRACEUSER( "Gerry", _T("Unimplemented Pass (%d)\n"), m_ConvertPass);
		}
		break;
	}

	if (bSetState)
		*pState = RenderState;

	return(bSetState);
}



/****************************************************************************

>	TextStory* XPFRenderCallback::ReformatTextStory(TextStory* pStory)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/10/2005

	Inputs:		pStory		- pointer to a TextStory
	Returns:	NULL in times of grief
	Purpose:	Creates a copy of a TextStory where the copy is manually 
				kerned and only uses left justification

****************************************************************************/

TextStory* XPFRenderCallback::ReformatTextStory(TextStory* pStory)
{
	// Create a FormatRegion to track text attributes
	FormatRegion FormattingRegion;
	if (FormattingRegion.Init(pStory) == FALSE)
		return(NULL);

	// Create a copy of the TextStory node
	TextStory* pNewStory = (TextStory*)(pStory->PublicCopy());
	if (pNewStory == NULL)
		return(NULL);

	// Keep track of where we are attaching new nodes
	Node* pContextNode = pNewStory;
	AttachNodeDirection Direction = FIRSTCHILD;

	// Loop through all the children of the story
	Node* pNode = pStory->FindFirstChild();
	while (pNode != NULL)
	{
		Node* pNewNode = NULL;
		if (IS_A(pNode, TextLine))				// If it is a TextLine
		{
			// Then create a reformatted copy
			pNewNode = ReformatTextLine((TextLine*)pNode, &FormattingRegion);
		}
		else if (!pNode->IsNodeHidden())		// Otherwise if it isn't a hidden node
		{
			// If it is a text attribute then render it into the FormatRegion
			if (pNode->IsKindOfTextAttribute())
				pNode->Render(&FormattingRegion);

			// If it isn't a justification or tracking attribute then copy it
			if (!IS_A(pNode, AttrTxtJustification) && !IS_A(pNode, AttrTxtTracking))
				pNewNode = pNode->PublicCopy();
		}

		// If we created a new node then attach it
		if (pNewNode)
		{
			pNewNode->AttachNode(pContextNode, Direction, FALSE, FALSE);

			// Next node will always be attached as the next sibling of this one
			pContextNode = pNewNode;
			Direction = NEXT;
		}
		
		pNode = pNode->FindNext();
	}

	// Make sure the auto-kerning is turned off
	pNewStory->SetAutoKerning(FALSE);

	// May have to cause the new story to reformat itself here

	return(pNewStory);
}



/****************************************************************************

>	Node* XPFRenderCallback::ReformatTextLine(TextLine* pLineNode, FormatRegion* pFormatRegion)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/10/2005

	Inputs:		pLineNode	- pointer to a TextLine
				pFormatRegion- pointer to a FormatRegion
	Returns:	NULL in times of grief
	Purpose:	Creates a left justified and manually kerned copy of a text 
				line by inserting manual kern codes where necessary

****************************************************************************/

Node* XPFRenderCallback::ReformatTextLine(TextLine* pLineNode, FormatRegion* pFormatRegion)
{
//	TRACE( _T("ReformatTextLine 0x%08x\n"), pLineNode);
	
	BOOL ok = TRUE;

	// Copy the line
	Node* pNewRoot = pLineNode->PublicCopy();

	// Kepp track of where we are attaching copies
	Node* pContextNode = pNewRoot;
	AttachNodeDirection Direction = FIRSTCHILD;

	pFormatRegion->SaveContext();

	// Keep track of the current pos on the line
	MILLIPOINT CurrentPosMP = 0;

	// scan immediate children rendering any text attrs, and copying any text nodes
	Node* pNode = pLineNode->FindFirstChild();
	while (pNode != NULL)
	{
//		TRACE( _T("Node at 0x%08x (%s)\n"), pNode, pNode->GetRuntimeClass()->m_lpszClassName);
		Node* pNewNode = NULL;
		if (pNode->IsAnAbstractTextChar())
		{
			VisibleTextNode* pVTN=(VisibleTextNode*)pNode;
			AbstractTextChar* pATC=(AbstractTextChar*)pNode;

			// Save the context and render all the child text attributes
			// into the FormatRegion
			pFormatRegion->SaveContext();

			Node* pChildNode = pNode->FindFirstChild();
			while (pChildNode != NULL)
			{
				if (pChildNode->IsKindOfTextAttribute())
					pChildNode->Render(pFormatRegion);

				pChildNode = pChildNode->FindNext();
			}

			// Get the metrics of the character
//			MILLIPOINT EMWidth = 0;
			CharMetrics CharMet;
			ok = pFormatRegion->GetCharMetrics(&CharMet, pATC->GetUnicodeValue());

			// If this character isn't a space
			// (have to do this or we get problems at the ends of lines)
			if (!pVTN->IsASpace())
			{
				// Check the line position and insert a KernCode
				MILLIPOINT PosMP = pVTN->GetPosInLine();
//				TRACE( _T("PosInLine = %d\n"), PosMP);
//				TRACE( _T("Current   = %d\n"), CurrentPosMP);
				if (PosMP != CurrentPosMP)					// If we aren't at the correct position in the line
				{
					// Calculate the required kern width in em/1000
					INT32 PosEM1000 = MulDiv(PosMP - CurrentPosMP, 1000, CharMet.FontEmWidth);

					// Convert the em/1000 size back to millipoints
					INT32 KernSizeMP = MulDiv(PosEM1000, CharMet.FontEmWidth, 1000);
//					TRACE( _T("Inserting justify kern of %d\n"), PosMP - CurrentPosMP);
//					TRACE( _T("Real size %d\n"), KernSizeMP);
					// If the millipoint size is larger than it should be then
					// subtract 1 from the em/1000 size
					if (KernSizeMP > (PosMP - CurrentPosMP))
						PosEM1000 -= 1;

					// Create and insert a KernCode with the same attributes as pNode
					DocCoord TempCoord(PosEM1000, 0);
					KernCode* pKern = new KernCode(TempCoord);
					if (pKern)
					{
						ok = CopyAttributesFromNode(pKern, pNode);
						pKern->AttachNode(pContextNode, Direction, FALSE, FALSE);
						pContextNode = pKern;
						Direction = NEXT;
					}
					CurrentPosMP = PosMP;
				}
			}

//			TRACE( _T("Inserting node width %d\n"), pATC->GetCharWidth());
			// Advance the current pos by the character width (note, not the CharAdvance)
			CurrentPosMP += pATC->GetCharWidth();

			// Create a deep copy of this Node
			pNewNode = pNode->PublicCopy();
			if (pNewNode)
			{
				ok = CopyAttributesFromNode(pNewNode, pNode);
				pNewNode->AttachNode(pContextNode, Direction, FALSE, FALSE);
				pContextNode = pNewNode;
				Direction = NEXT;
				pNewNode = NULL;
			}

			pFormatRegion->RestoreContext();
		}
		else if(!pNode->IsNodeHidden())
		{
			if (pNode->IsKindOfTextAttribute())
				pNode->Render(pFormatRegion);

			if (!IS_A(pNode, AttrTxtJustification) && !IS_A(pNode, AttrTxtTracking))
				pNewNode = pNode->PublicCopy();

			if (pNewNode)
			{
				ok = CopyAttributesFromNode(pNewNode, pNode);
				pNewNode->AttachNode(pContextNode, Direction, FALSE, FALSE);
				pContextNode = pNewNode;
				Direction = NEXT;
			}
		}

		pNode = pNode->FindNext();
	}

	pFormatRegion->RestoreContext();

	return(pNewRoot);
}



/****************************************************************************

>	BOOL XPFRenderCallback::CopyAttributesFromNode(Node* pDestNode, Node* pSrcNode)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/10/2005

	Inputs:		pDestNode	- pointer to a Node
				pSrcNode	- pointer to a Node
	Returns:	TRUE if ok, FALSE if bother
	Purpose:	Copies all attributes except justification and tracking 
				from one node to another

****************************************************************************/

BOOL XPFRenderCallback::CopyAttributesFromNode(Node* pDestNode, Node* pSrcNode)
{
	Node* pChildNode = pSrcNode->FindFirstChild();
	Node* pContextNode = pDestNode;
	AttachNodeDirection Direction = FIRSTCHILD;
	while (pChildNode)
	{
		if (!pChildNode->IsNodeHidden() && !IS_A(pChildNode, AttrTxtJustification) && !IS_A(pChildNode, AttrTxtTracking))
		{
			Node* pNewChild = pChildNode->PublicCopy();		
			if (pNewChild)
			{
				pNewChild->AttachNode(pContextNode, Direction, FALSE, FALSE);
				pContextNode = pNewChild;
				Direction = NEXT;
			}
		}

		pChildNode = pChildNode->FindNext();
	}

	return(TRUE);
}



/****************************************************************************

>	void XPFRenderCallback::RemoveChildAttrs(Node* pNode, CCRuntimeClass* pClass)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/10/2005

	Inputs:		pNode		- pointer to a Node
				pClass		- pointer to a CCRuntimeClass
	Purpose:	Removes all child attributes of a particular attribute type

****************************************************************************/

void XPFRenderCallback::RemoveChildAttrs(Node* pNode, CCRuntimeClass* pClass)
{
	Node* pChild = pNode->FindFirstChild();
	while (pChild)
	{
		Node* pNextChild = pChild->FindNext();
		if (pChild->IsAnAttribute())
		{
			NodeAttribute* pAttr = (NodeAttribute*)pChild;
			if (pAttr->GetAttributeType() == pClass)
			{
				pChild->CascadeDelete();
				delete pChild;
			}
		}
		pChild = pNextChild;
	}
}



/****************************************************************************

>	NodeAttribute* XPFRenderCallback::FindChildAttr(Node* pNode, CCRuntimeClass* pClass)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/10/2005

	Inputs:		pNode		- pointer to a Node
				pClass		- pointer to a CCRuntimeClass
	Returns:	NULL in times of grief
	Purpose:	Finds the last child attr of the specified attribute type

****************************************************************************/

NodeAttribute* XPFRenderCallback::FindChildAttr(Node* pNode, CCRuntimeClass* pClass)
{
	Node* pChild = pNode->FindLastChild();
	while (pChild)
	{
		if (pChild->IsAnAttribute())
		{
			NodeAttribute* pAttr = (NodeAttribute*)pChild;
			if (pAttr->GetAttributeType() == pClass)
			{
				return(pAttr);
			}
		}
		pChild = pChild->FindPrevious();
	}
	return(NULL);
}



/****************************************************************************

>	BOOL XPFRenderCallback::DoesNodeUseNonAlphaTrans(Node* pRootNode)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/10/2005

	Inputs:		pRootNode	- pointer to a Node
	Returns:	TRUE if ok, FALSE if bother
	Purpose:	Determines if a node has any attributes that use non-alpha 
				transparency

****************************************************************************/

BOOL XPFRenderCallback::DoesNodeUseNonAlphaTrans(Node* pRootNode)
{
	// This function determines if the specified node uses any non-alpha compatible 
	// transparency types (anywhere in its subtree)

	// If this isn't a renderable ink node then get out
	// It might be a layer or spread which are paper nodes!
//	if (!pRootNode->IS_KIND_OF(NodeRenderableInk))
//		return(FALSE);

	if (pRootNode->IsAnObject())
	{
		NodeRenderableInk* pInkNode = (NodeRenderableInk*)pRootNode;
		// Basically, it just needs to check any transparency attributes
		// First it needs to check the attributes applied above this node in the tree

		AttrStrokeTransp* pStrkAttr = (AttrStrokeTransp*)(pInkNode->FindAppliedAttribute(CC_RUNTIME_CLASS(AttrStrokeTransp), TRUE));
		if (pStrkAttr)
		{
			UINT32 Type = pStrkAttr->GetTranspType();
			if (Type != TT_NoTranspType &&
				Type != TT_Mix &&
				Type != TT_DARKEN &&
				Type != TT_LIGHTEN &&
				Type != TT_BRIGHTNESS &&
				Type != TT_BEVEL)
			{
				return(TRUE);
			}
		}

		AttrFillGeometry* pFillAttr = (AttrFillGeometry*)(pInkNode->FindAppliedAttribute(CC_RUNTIME_CLASS(AttrTranspFillGeometry), TRUE));
		if (pFillAttr)
		{
			UINT32 Type = pFillAttr->GetTranspType();
			if (Type != TT_NoTranspType &&
				Type != TT_Mix &&
				Type != TT_DARKEN &&
				Type != TT_LIGHTEN &&
				Type != TT_BRIGHTNESS &&
				Type != TT_BEVEL)
			{
				return(TRUE);
			}
		}
	}

	// Now we scan through the tree looking for all AttrFillGeometry nodes
	Node* pNode = pRootNode->FindFirstDepthFirst();
	while (pNode)
	{
		// Check this node
		if (pNode->IsAnAttribute() && pNode->IS_KIND_OF(AttrFillGeometry))
		{
			AttrFillGeometry* pAttr = (AttrFillGeometry*)pNode;
			UINT32 Type = pAttr->GetTranspType();
			if (Type != TT_NoTranspType &&
				Type != TT_Mix &&
				Type != TT_DARKEN &&
				Type != TT_LIGHTEN &&
				Type != TT_BRIGHTNESS &&
				Type != TT_BEVEL)
			{
				return(TRUE);
			}
		}

		// Move on to the next node
		pNode = pNode->FindNextDepthFirst(pRootNode);
	}

	return(FALSE);
}



/****************************************************************************

>	BOOL XPFRenderCallback::ConvertNodes()

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/10/2005

	Returns:	TRUE if ok, FALSE if bother
	Purpose:	Performs the required conversion on the stored list of nodes

****************************************************************************/

BOOL XPFRenderCallback::ConvertNodes()
{
	BOOL ok = TRUE;
	switch (m_ConvertPass)
	{
		case 2:
			ok = ConvertNodesForPass2();
			break;

		case 3:
			ok = ConvertNodesForPass3();
			break;

		case 4:
			ok = ConvertNodesForPass4();
			break;

		case 5:
			ok = ConvertNodesForPass5();
			break;

		default:
			break;
	}

	// And empty the list
	m_ConvertList.DeleteAll();

	return(ok);
}



/****************************************************************************

>	BOOL XPFRenderCallback::ConvertNodesForPass2()

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/10/2005

	Returns:	TRUE if ok, FALSE if bother
	Purpose:	Performs the "stroked" conversion on the convert list

****************************************************************************/

BOOL XPFRenderCallback::ConvertNodesForPass2()
{
	TRACEUSER( "Gerry", _T("ConvertNodesForPass2 (%d)\n"), m_ConvertList.GetCount());

	double dProgress = 0.0;
	double dInc = 100.0 / (double)m_ConvertList.GetCount();

	BOOL ok = TRUE;
	NodeListItem* pItem = (NodeListItem*)m_ConvertList.GetHead();
	while (pItem)
	{
		Node* pNode = pItem->pNode;
		NodeRenderableInk* pInkNode = (NodeRenderableInk*)pNode;

		TRACEUSER( "Gerry", _T("ConvertStroked 0x%08x (%s) \n"), pNode, pNode->GetRuntimeClass()->m_lpszClassName);

		// First we need to check if there is any line colour on the object
		// If there isn't then we don't do anything
		NodeAttribute* pStrokeCol = NULL;
		pInkNode->FindAppliedAttribute(CC_RUNTIME_CLASS(AttrStrokeColour), &pStrokeCol);
		if (pStrokeCol && !((AttrStrokeColour*)pStrokeCol)->Value.GetStartColour()->IsTransparent())
		{
			// Now we need to check if there is any fill on this object.
			// If there is then we need to create a group to represent this node
			// This will hold this node with the stroke attributes removed and 
			// a copy of this node to represent the stroke which will have the 
			// conversion done on it
			// Otherwise we can just do the conversion on the original node

			BOOL bHasFill = TRUE;
			if (pNode->IsKindOf(CC_RUNTIME_CLASS(NodePath)))
			{
				NodePath* pPath = (NodePath*)pNode;
				if (!pPath->InkPath.IsFilled)
					bHasFill = FALSE;
			}

			if (bHasFill)
			{
				NodeAttribute* pAppliedAttr = NULL;
				pInkNode->FindAppliedAttribute(CC_RUNTIME_CLASS(AttrFillGeometry), &pAppliedAttr);
				if (pAppliedAttr != NULL && IS_A(pAppliedAttr, AttrFlatColourFill))
				{
					DocColour* pLineColour = ((AttrFlatColourFill*)pAppliedAttr)->GetStartColour();
					if (pLineColour != NULL)
						if (pLineColour->IsTransparent())
							bHasFill = FALSE;
				}
			}

			if (bHasFill)
			{
				// Create a group node and insert it into the tree as next
				NodeGroup* pGroup = new NodeGroup(pNode, NEXT);
				if (pGroup == NULL)
				{
					// Set the error
					return(FALSE);
				}

				Node* pFillNode = NULL;

				// Create a copy of the node and attach it as first child of the group
				ok = pNode->NodeCopy((Node**)&pFillNode);
				if (!ok)
				{
					// Set the error
					return(ok);
				}

				pFillNode->AttachNode(pGroup, FIRSTCHILD);

				// Move the node to convert to be next sibling of the copy
				pNode->MoveNode(pFillNode, NEXT);

				// Convert the node
				ok = OpConvertPathToShapes::ConvertPathToShapes(NULL, (NodeRenderableInk*)pNode);
				TRACEUSER( "Gerry", _T("ConvertPathToShapes returned %s\n"), ok ? _T("true") : _T("false"));
				if (!ok)
				{
					// Report error
					return(ok);
				}

				// Remove the stroke attributes from the fill object
				pGroup->LocaliseCommonAttributes(FALSE, TRUE, NULL);

				// Remove all the stroke based attributes
				RemoveChildAttrs(pFillNode, CC_RUNTIME_CLASS(AttrStrokeColour));
				RemoveChildAttrs(pFillNode, CC_RUNTIME_CLASS(AttrStrokeTransp));
				RemoveChildAttrs(pFillNode, CC_RUNTIME_CLASS(AttrDashPattern));
				RemoveChildAttrs(pFillNode, CC_RUNTIME_CLASS(AttrStartArrow));
				RemoveChildAttrs(pFillNode, CC_RUNTIME_CLASS(AttrEndArrow));
				RemoveChildAttrs(pFillNode, CC_RUNTIME_CLASS(AttrStartCap));
				RemoveChildAttrs(pFillNode, CC_RUNTIME_CLASS(AttrLineWidth));
				RemoveChildAttrs(pFillNode, CC_RUNTIME_CLASS(AttrVariableWidth));
				RemoveChildAttrs(pFillNode, CC_RUNTIME_CLASS(AttrBrushType));
				RemoveChildAttrs(pFillNode, CC_RUNTIME_CLASS(AttrStrokeType));

				// Apply a no stroke colour attribute
				AttrStrokeColour* pTranspAttr = new AttrStrokeColour(pFillNode, FIRSTCHILD);
				if (pTranspAttr)
				{
					StrokeColourAttribute* pTranspVal = (StrokeColourAttribute*)(pTranspAttr->GetAttributeValue());
					pTranspVal->Colour = DocColour(COLOUR_NONE);
				}

				pGroup->FactorOutCommonChildAttributes(TRUE, (AttrTypeSet*)NULL);
			}
			else
			{
				ok = OpConvertPathToShapes::ConvertPathToShapes(NULL, (NodeRenderableInk*)pNode);
				TRACEUSER( "Gerry", _T("ConvertPathToShapes returned %s\n"), ok ? _T("true") : _T("false"));
				if (!ok)
				{
					return(ok);
				}
			}
		}
		else
		{
			TRACEUSER( "Gerry", _T("No line colour\n"));
		}

		// Get the next span item
		pItem = (NodeListItem*)m_ConvertList.GetNext(pItem);

		dProgress += dInc;
		if (!m_pFilter->SetProgressBarCount((UINT32)dProgress))
		{
			Error::SetError(_R(IDN_USER_CANCELLED),0);			// Expects error set
			return(FALSE);
		}
	}

	return(TRUE);
}



/****************************************************************************

>	BOOL XPFRenderCallback::ConvertNodesForPass3()

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/10/2005

	Returns:	TRUE if ok, FALSE if bother
	Purpose:	Performs the bitmapfill, bitmaptrans and bitmapfilltrans 
				conversions on the convert list

****************************************************************************/

BOOL XPFRenderCallback::ConvertNodesForPass3()
{
	TRACEUSER( "Gerry", _T("ConvertNodesForPass3 (%d)\n"), m_ConvertList.GetCount());

	double dProgress = 0.0;
	double dInc = 100.0 / (double)m_ConvertList.GetCount();

	NodeCompound* pLastParent = NULL;
	NodeThreeBoolListItem* pItem = (NodeThreeBoolListItem*)m_ConvertList.GetHead();
	while (pItem)
	{
		TRACEUSER( "Gerry", _T("ConvertFillAttrs 0x%08x (%s) %s%s\n"), pItem->m_pNode, pItem->m_pNode->GetRuntimeClass()->m_lpszClassName, pItem->m_bFirst ? _T("fill ") : _T(""), pItem->m_bSecond ? _T("trans") : _T(""));
		Node* pNode = pItem->m_pNode;
		Node* pParentNode = pNode->FindParent();
		ERROR2IF(!pParentNode, FALSE, "Node has no parent in ConvertNodesForPass3");

		NodeCompound* pParent = NULL;
		if (pParentNode->IsCompound())
		{
			pParent = (NodeCompound*)pParentNode;
			if (pParent != pLastParent)
			{
				if (pLastParent)
					pLastParent->FactorOutCommonChildAttributes(TRUE, (AttrTypeSet*)NULL);

				pParent->LocaliseCommonAttributes(TRUE, TRUE, NULL);
				pLastParent = pParent;
			}
		}

		NodeRenderableBounded* pTheNode = (NodeRenderableBounded*)pNode;
		DocRect BoundsRect = pTheNode->GetBoundingRect(TRUE, FALSE);

		if (pItem->m_bThird)
		{
			// We need to combine the fill and transparency into a bitmap fill
			// and then create a bitmap fill attribute to replace the existing fill with
			// All of the fill attributes must be set to sensible values
			KernelBitmap* pBmp = RenderFillAndTransToBitmap(pNode, BoundsRect);
			if (!pBmp)
				return(FALSE);

			// Remove any existing fill and transparency attributes
			RemoveChildAttrs(pNode, CC_RUNTIME_CLASS(AttrFillGeometry));
			RemoveChildAttrs(pNode, CC_RUNTIME_CLASS(AttrFillMapping));
			RemoveChildAttrs(pNode, CC_RUNTIME_CLASS(AttrFillEffect));
			RemoveChildAttrs(pNode, CC_RUNTIME_CLASS(AttrTranspFillMapping));
			RemoveChildAttrs(pNode, CC_RUNTIME_CLASS(AttrTranspFillGeometry));

			// Apply a bitmap fill
			AttrBitmapColourFill* pBmpFill = new AttrBitmapColourFill(pNode, FIRSTCHILD);
			if (!pBmpFill)
			{
				delete pBmp;
				return(FALSE);
			}

			BitmapFillAttribute* pBmpVal = (BitmapFillAttribute*)(pBmpFill->GetAttributeValue());
			pBmpVal->BitmapRef.Attach(pBmp);
			DocCoord BottomLeft(BoundsRect.lo);
			DocCoord BottomRight(BoundsRect.hi.x, BoundsRect.lo.y);
			DocCoord TopLeft(BoundsRect.lo.x, BoundsRect.hi.y);
			pBmpVal->SetStartPoint(&BottomLeft);
			pBmpVal->SetEndPoint(&BottomRight);
			pBmpVal->SetEndPoint2(&TopLeft);

			// Apply a simple fill mapping
			AttrFillMappingLinear* pFillMap = new AttrFillMappingLinear(pNode, FIRSTCHILD);
			if (!pFillMap)
			{
				delete pBmpFill;
				return(FALSE);
			}

			FillMappingLinearAttribute* pMapVal = (FillMappingLinearAttribute*)(pFillMap->GetAttributeValue());
			pMapVal->Repeat = 1;	// Set no repeat
		}
		else
		{
			if (pItem->m_bFirst)
			{
				// We need to render the fill of the object into a bitmap ignoring any transparency
				// and then create a bitmap fill attribute to replace the existing fill with
				// All of the fill attributes must be set to sensible values
				KernelBitmap* pBmp = RenderFillToBitmap(pNode, BoundsRect);
				if (!pBmp)
					return(FALSE);

				// Remove any existing fill attributes
				RemoveChildAttrs(pNode, CC_RUNTIME_CLASS(AttrFillGeometry));
				RemoveChildAttrs(pNode, CC_RUNTIME_CLASS(AttrFillMapping));
				RemoveChildAttrs(pNode, CC_RUNTIME_CLASS(AttrFillEffect));

				// Apply a bitmap fill
				AttrBitmapColourFill* pBmpFill = new AttrBitmapColourFill(pNode, FIRSTCHILD);
				if (!pBmpFill)
				{
					delete pBmp;
					return(FALSE);
				}

				BitmapFillAttribute* pBmpVal = (BitmapFillAttribute*)(pBmpFill->GetAttributeValue());
				pBmpVal->BitmapRef.Attach(pBmp);
				DocCoord BottomLeft(BoundsRect.lo);
				DocCoord BottomRight(BoundsRect.hi.x, BoundsRect.lo.y);
				DocCoord TopLeft(BoundsRect.lo.x, BoundsRect.hi.y);
				pBmpVal->SetStartPoint(&BottomLeft);
				pBmpVal->SetEndPoint(&BottomRight);
				pBmpVal->SetEndPoint2(&TopLeft);

				// Apply a simple fill mapping
				AttrFillMappingLinear* pFillMap = new AttrFillMappingLinear(pNode, FIRSTCHILD);
				if (!pFillMap)
				{
					delete pBmpFill;
					return(FALSE);
				}

				FillMappingLinearAttribute* pMapVal = (FillMappingLinearAttribute*)(pFillMap->GetAttributeValue());
				pMapVal->Repeat = 1;	// Set no repeat
			}

			if (pItem->m_bSecond)
			{
				// We need to render the transparency of the object as a greyscale graduated fill 
				// and then create a bitmap transparency attribute to replace the existing one with
				// All of the transparency attributes must be set to sensible values

				UINT32 TransType = 0;
				KernelBitmap* pBmp = RenderTransToBitmap(pNode, BoundsRect, &TransType);
				if (!pBmp)
					return(FALSE);

				// Find the last child AttrTranspFillGeometry
				// Apply a bitmap transparency as the next node to the one found
				NodeAttribute* pOldGeometry = FindChildAttr(pNode, CC_RUNTIME_CLASS(AttrTranspFillGeometry));

				// Apply a bitmap transparency
				AttrBitmapTranspFill* pBmpTrans = NULL;
				if (pOldGeometry)
					pBmpTrans = new AttrBitmapTranspFill(pOldGeometry, NEXT);
				else
					pBmpTrans = new AttrBitmapTranspFill(pNode, FIRSTCHILD);
				if (!pBmpTrans)
				{
					delete pBmp;
					return(FALSE);
				}

				BitmapTranspFillAttribute* pBmpVal = (BitmapTranspFillAttribute*)(pBmpTrans->GetAttributeValue());
				pBmpVal->BitmapRef.Attach(pBmp);
				DocCoord BottomLeft(BoundsRect.lo);
				DocCoord BottomRight(BoundsRect.hi.x, BoundsRect.lo.y);
				DocCoord TopLeft(BoundsRect.lo.x, BoundsRect.hi.y);
				pBmpVal->SetStartPoint(&BottomLeft);
				pBmpVal->SetEndPoint(&BottomRight);
				pBmpVal->SetEndPoint2(&TopLeft);
				UINT32 Val = 0;
				pBmpVal->SetStartTransp(&Val);
				Val = 255;
				pBmpVal->SetEndTransp(&Val);
				pBmpVal->SetTranspType(TransType);

				// Find the last child AttrTranspFillMappingLinear
				NodeAttribute* pOldMapping = FindChildAttr(pNode, CC_RUNTIME_CLASS(AttrTranspFillMapping));

				// If we have found an old geometry attr
				if (pOldGeometry)
				{
					// And we have found an old mapping attr and they don't match Effects
					if (pOldMapping && pOldMapping->IsEffectAttribute() != pOldGeometry->IsEffectAttribute())
						pOldMapping = NULL;		// Flag that we haven't found an old mapping
				}

				// Apply a simple fill mapping
				AttrTranspFillMappingLinear* pTransMap = NULL;
				if (pOldMapping)
					pTransMap = new AttrTranspFillMappingLinear(pOldMapping, NEXT);
				else if (pOldGeometry)
					pTransMap = new AttrTranspFillMappingLinear(pOldGeometry, NEXT);
				else
					pTransMap = new AttrTranspFillMappingLinear(pNode, FIRSTCHILD);
				if (!pTransMap)
				{
					delete pBmpTrans;
					return(FALSE);
				}

				TranspFillMappingLinearAttribute* pMapVal = (TranspFillMappingLinearAttribute*)(pTransMap->GetAttributeValue());
				pMapVal->Repeat = 1;	// Set no repeat

				// Delete the old attributes
				if (pOldGeometry)
				{
					pOldGeometry->CascadeDelete();
					delete pOldGeometry;
				}
				if (pOldMapping)
				{
					pOldMapping->CascadeDelete();
					delete pOldMapping;
				}
			}
		}

		dProgress += dInc;
		if (!m_pFilter->SetProgressBarCount((UINT32)dProgress))
		{
			Error::SetError(_R(IDN_USER_CANCELLED),0);			// Expects error set
			return(FALSE);
		}

		pItem = (NodeThreeBoolListItem*)m_ConvertList.GetNext(pItem);
	}

	if (pLastParent)
		pLastParent->FactorOutCommonChildAttributes(TRUE, (AttrTypeSet*)NULL);

	return(TRUE);
}



/****************************************************************************

>	BOOL XPFRenderCallback::ConvertNodesForPass4()

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/10/2005

	Returns:	TRUE if ok, FALSE if bother
	Purpose:	Performs the bitmap conversion on the convert list

****************************************************************************/

BOOL XPFRenderCallback::ConvertNodesForPass4()
{
	TRACEUSER( "Gerry", _T("ConvertNodesForPass4 (%d)\n"), m_ConvertList.GetCount());

	double dProgress = 0.0;
	double dInc = 100.0 / (double)m_ConvertList.GetCount();

	NodeListItem* pItem = (NodeListItem*)m_ConvertList.GetHead();
	while (pItem)
	{
		Node* pNode = pItem->pNode;
		if (pNode)
		{
			TRACEUSER( "Gerry", _T("NodeListItem 0x%08x (%s)\n"), pNode, pNode->GetRuntimeClass()->m_lpszClassName);

			// Render the node span to a bitmap
			BOOL bNonAlphaTrans = DoesNodeUseNonAlphaTrans(pNode);
			Node* pNewNode = RenderNodesToBitmap(pNode, pNode, bNonAlphaTrans);
			if (!pNewNode)
				return(FALSE);

			// Attach the new node as the previous of the first in the span
			pNewNode->AttachNode(pNode, PREV);

			// Delete the node we have just replaced
			pNode->CascadeDelete();
			delete pNode;
		}

		dProgress += dInc;
		if (!m_pFilter->SetProgressBarCount((UINT32)dProgress))
		{
			Error::SetError(_R(IDN_USER_CANCELLED),0);			// Expects error set
			return(FALSE);
		}

		// Get the next item
		pItem = (NodeListItem*)m_ConvertList.GetNext(pItem);
	}

	return(TRUE);
}



/****************************************************************************

>	BOOL XPFRenderCallback::ConvertNodesForPass5()

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/10/2005

	Returns:	TRUE if ok, FALSE if bother
	Purpose:	Performs the bitmapspan conversion on the convert list

****************************************************************************/

BOOL XPFRenderCallback::ConvertNodesForPass5()
{
	TRACEUSER( "Gerry", _T("ConvertNodesForPass5 (%d)\n"), m_ConvertList.GetCount());

	double dProgress = 0.0;
	double dInc = 100.0 / (double)m_ConvertList.GetCount();
	
	SpanListItem* pItem = (SpanListItem*)m_ConvertList.GetHead();
	while (pItem)
	{
		TRACEUSER( "Gerry", _T("SpanListItem 0x%08x (%s) to 0x%08x (%s)\n"), pItem->m_pFirstNode, pItem->m_pFirstNode->GetRuntimeClass()->m_lpszClassName, pItem->m_pLastNode, pItem->m_pLastNode->GetRuntimeClass()->m_lpszClassName);

		// Render the node span to a bitmap
		Node* pNewNode = RenderNodesToBitmap(pItem->m_pFirstNode, pItem->m_pLastNode, pItem->m_bNonAlphaTrans);
		if (!pNewNode)
			return(FALSE);

		// Attach the new node as the previous of the first in the span
		pNewNode->AttachNode(pItem->m_pFirstNode, PREV);

		// Delete all the nodes in the span
		Node* pNode = pItem->m_pFirstNode;
		while (pNode)
		{
			Node* pNextNode = (pNode == pItem->m_pLastNode) ? NULL : pNode->FindNext();

			pNode->CascadeDelete();
			delete pNode;

			pNode = pNextNode;
		}

		dProgress += dInc;
		if (!m_pFilter->SetProgressBarCount((UINT32)dProgress))
		{
			Error::SetError(_R(IDN_USER_CANCELLED),0);			// Expects error set
			return(FALSE);
		}

		// Get the next span item
		pItem = (SpanListItem*)m_ConvertList.GetNext(pItem);
	}

	return(TRUE);
}


/****************************************************************************

>	BOOL XPFRenderCallback::FindCommonTransTypeToApply(Node* pFirstNode, Node* pLastNode, UINT32* pCommonType)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/06/2006

	Inputs:		pFirstNode	- pointer to a Node
				pLastNode	- pointer to a Node
				pCommonType	- pointer to a UINT32
	Returns:	TRUE if ok, FALSE if bother
	Purpose:	

****************************************************************************/

BOOL XPFRenderCallback::FindCommonTransTypeToApply(Node* pFirstNode, Node* pLastNode, UINT32* pCommonType)
{
	// If CommonTrans isn't set then return false
	if (!m_pCapTree->HasRasteriseCommonTrans())
		return(FALSE);
	
	// Render the node span using an XPFRenderRegion and XPFSpanRenderCallback
	// to track the transparency used
	View *pView = View::GetCurrent();
	Spread* pSpread = pFirstNode->FindParentSpread();;

	CommonTransInfo TransInfo;
	
	// Create and set up a new XPFRenderRegion
	XPFRenderRegion XPFRegion(NULL, NULL, &TransInfo);

	// Attach a device to the scanning render region
	// Since this rr does no real rendering, it does not need a Device context
	XPFRegion.AttachDevice(pView, NULL, pSpread);

	// Start the render region and return if it fails
	if (XPFRegion.StartRender())
	{			
		TRACEUSER( "Gerry", _T("Rendering nodes from 0x%08x to 0x%08x\n"), pFirstNode, pLastNode);
		XPFSpanRenderCallback SpanCallback(pFirstNode, pLastNode, FALSE);
		// Call RenderTree to do the rendering
		XPFRegion.RenderTree(pSpread, FALSE, FALSE, &SpanCallback);

		// Thats all the nodes rendered, so stop rendering
		XPFRegion.StopRender();

		// Check the CommonTransInfo
		if (TransInfo.IsCommonType())
		{
			UINT32 CommonType = TransInfo.GetCommonType();
			if (CommonType != TT_Mix)
			{
				*pCommonType = CommonType;
				return(TRUE);
			}
		}
		else
		{
			if (TransInfo.UsesNonAlpha())
			{
				if (pFirstNode == pLastNode && IS_A(pFirstNode, Layer))
				{
					Layer* pLayer = (Layer*)pFirstNode;
					String_256 LayerName = pLayer->GetLayerID();
					String_256 WarningMsg;
					WarningMsg.MakeMsg(_R(IDS_XPF_MIXEDTRANSLAYER), &LayerName);
					Error::SetError(_R(IDS_XPF_MIXEDTRANSLAYER),WarningMsg,0);
					InformWarning();		
				}
			}
		}
	}
	else
	{
		ERROR2(FALSE, "StartRender failed");
	}

	return(FALSE);
}


/****************************************************************************

>	Node* XPFRenderCallback::RenderNodesToBitmap(Node* pFirstNode, Node* pLastNode, BOOL bNonAlphaTrans)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/10/2005

	Inputs:		pFirstNode	- pointer to a Node
				pLastNode	- pointer to a Node
				bNonAlphaTrans- if TRUE then node span uses non-alpha trans
	Returns:	NULL in times of grief
	Purpose:	Renders a range of nodes into a bitmap.  If it uses non-alpha 
				trans then a 24bpp bitmap is generated including all the 
				objects before the span.  If it doesn't use non-alpha trans 
				then a 32bpp RGBA bitmap of just the node span is generated

****************************************************************************/

Node* XPFRenderCallback::RenderNodesToBitmap(Node* pFirstNode, Node* pLastNode, BOOL bNonAlphaTrans)
{
	// First we detect the single layer case for various bits of special handling
	BOOL bOldLayerVisibility = FALSE;
	Layer* pSingleLayer = NULL;
	Spread* pSingleSpread = NULL;
	if (pFirstNode == pLastNode)
	{
		if (pFirstNode->IsLayer())
		{
			pSingleLayer = (Layer*)pFirstNode;
			bOldLayerVisibility = pSingleLayer->GetVisibleFlagState();
			pSingleLayer->SetVisible(TRUE);
		}
		if (pFirstNode->IsSpread())
		{
			pSingleSpread = (Spread*)pFirstNode;
		}
	}
	
	// Find the bounding rect of the nodes and determine if the background needs
	// to be rendered
	DocRect SpanBounds;
	BOOL bBackground = FALSE;
	BOOL bForceMix = FALSE;
	BOOL bAlpha = m_pCapTree->GetRasteriseAlpha();
	UINT32 TransToApply = TT_NoTranspType;
	if (!bAlpha)
	{
		bBackground = TRUE;
	}
	else
	{
		if (FindCommonTransTypeToApply(pFirstNode, pLastNode, &TransToApply))
		{
			bBackground = FALSE;
			bForceMix = TRUE;
		}
		else
		{
			bBackground = bNonAlphaTrans;
		}
	}

	Node* pNode = pFirstNode;
	while (pNode)
	{
		if (pNode->IsBounded())
		{
			DocRect ObjectRect = ((NodeRenderableBounded*)pNode)->GetBoundingRect(FALSE, FALSE);
			SpanBounds = SpanBounds.Union(ObjectRect);
		}
		
		if (pNode == pLastNode)
			break;

		pNode = pNode->FindNext();
	}

	View* pView = View::GetCurrent();
	Spread* pSpread = pFirstNode->FindParentSpread();

	Matrix ViewTrans;
	FIXED16 TempScale(1.0);
	double Dpi = m_pCapTree->GetRasteriseDPI();

	// Make sure that SpanBounds is an exact multiple of pixels and is at 
	// pixel multiples and is not zero-sized
	double MPPerPix = 72000.0 / Dpi;

	INT32 IntVal = (INT32)floor((double)SpanBounds.lo.x / MPPerPix);
	SpanBounds.lo.x = (INT32)floor((double)IntVal * MPPerPix);
	IntVal = (INT32)floor((double)SpanBounds.lo.y / MPPerPix);
	SpanBounds.lo.y = (INT32)floor((double)IntVal * MPPerPix);
	IntVal = (INT32)ceil((double)SpanBounds.hi.x / MPPerPix);
	SpanBounds.hi.x = (INT32)ceil((double)IntVal * MPPerPix);
	IntVal = (INT32)ceil((double)SpanBounds.hi.y / MPPerPix);
	SpanBounds.hi.y = (INT32)ceil((double)IntVal * MPPerPix);

	if (SpanBounds.Width() < MPPerPix)
		SpanBounds.hi.x = (INT32)ceil((double)SpanBounds.lo.x + MPPerPix);
	if (SpanBounds.Height() < MPPerPix)
		SpanBounds.hi.y = (INT32)ceil((double)SpanBounds.lo.y + MPPerPix);

	// Create a full 32bpp RGBA for the mask
	// This is so that the antialiased pixels are handled correctly in the mask
	// Rendering into a 1bpp mask only sets half of the edge pixels that an 
	// anti-aliased render does and the mask spreading feature doesn't correctly 
	// account for the difference
	GRenderBitmap MaskBitmap(SpanBounds, ViewTrans, TempScale, 32, Dpi);
	if (bBackground && bAlpha)
	{
		MaskBitmap.m_DoCompression = TRUE;
		MaskBitmap.AttachDevice(pView, NULL, pSpread);

		if (MaskBitmap.StartRender())
		{
			// Save the context here so we can clear everything up later
			MaskBitmap.SaveContext();

			// Best quality please
			QualityAttribute *pQualAttr = new QualityAttribute();
			pQualAttr->QualityValue.SetQuality(QUALITY_MAX);
			MaskBitmap.SetQuality(pQualAttr, TRUE);

			XPFSpanRenderCallback MaskCallback(pFirstNode, pLastNode, FALSE);
			MaskBitmap.RenderTree(pSpread, FALSE, FALSE, &MaskCallback);

			// Save the context here so we can clear everything up later
			MaskBitmap.RestoreContext();

			// Tell the render region we are done rendering
			MaskBitmap.StopRender();
		}
	}

	GRenderBitmap BitmapRR(SpanBounds, ViewTrans, TempScale, 32, Dpi);
	if (!bBackground)
		BitmapRR.m_DoCompression = TRUE;
	BitmapRR.SetForceMixTransparency(bForceMix);
	BitmapRR.SetUsingSmoothedBitmaps(TRUE);		// Make sure we do high quality
	BitmapRR.AttachDevice(pView, NULL, pSpread);

	// Start rendering into the bitmap
	if (BitmapRR.StartRender())
	{
		// Save the context here so we can clear everything up later
		BitmapRR.SaveContext();

		if (bBackground)
		{
			// Draw required background
			DocRect DrawRect = SpanBounds;
			// Inflate the rect by 2 pixels
			DrawRect.Inflate( (INT32)(2*72000.0/Dpi + 0.5) );

			BitmapRR.SaveContext();
			DocColour White(255,255,255);
			BitmapRR.SetFillColour(White);
			BitmapRR.DrawRect(&DrawRect);
			BitmapRR.RestoreContext();
		}

		// Best quality please
		QualityAttribute *pQualAttr = new QualityAttribute();
		pQualAttr->QualityValue.SetQuality(QUALITY_MAX);
		BitmapRR.SetQuality(pQualAttr, TRUE);

		// Render the relevant span of the tree
		TRACEUSER("Gerry", _T("Rendering nodes from 0x%08x to 0x%08x%s\n"), pFirstNode, pLastNode, bBackground ? _T(" with background") : _T(""));
		XPFSpanRenderCallback SpanCallback(pFirstNode, pLastNode, bBackground);
		BitmapRR.RenderTree(pSpread, FALSE, FALSE, &SpanCallback);

		// This should stop any captures
		BitmapRR.RestoreContext();

		// Stop rendering
		BitmapRR.StopRender();

		// Reset the layer visiblity to the correct value
		if (pSingleLayer)
			pSingleLayer->SetVisible(bOldLayerVisibility);
	}
	else
	{
		ERROR2(NULL, "StartRender failed in RenderNodesToBitmap");
	}

	// Get the rendered OILBitmap
	OILBitmap* pFullBitmap = BitmapRR.ExtractBitmap();
	String_256 BmpName(m_pFilter->GetNewBitmapName());
	pFullBitmap->SetName(BmpName);

	if (bBackground && bAlpha)
	{
		// Merge in the mask info to knock out the surrounding areas
		OILBitmap* pMaskBitmap = MaskBitmap.ExtractBitmap();
		pFullBitmap->CopyFullyTransparentFrom(pMaskBitmap);

		// We can't delete an OILBitmap directly so we create a 
		// KernelBitmap and then delete that
		KernelBitmap* pTempBmp = KernelBitmap::MakeKernelBitmap(pMaskBitmap);
		delete pTempBmp;
	}
	
	KernelBitmap* pRealBmp = KernelBitmap::MakeKernelBitmap(pFullBitmap);

	// Attach the bitmap to this document or a copy will be created when 
	// it is used in the NodeBitmap
	BitmapList* pBmpList = NULL;
	Document* pCurDoc = Document::GetCurrent();
	if (pCurDoc)
		pBmpList = pCurDoc->GetBitmapList();

	// and then attach the bitmap (doesn't matter if BmpList is NULL)
	pRealBmp->Attach(pBmpList);

	// Create a NodeBitmap not in the tree
	NodeBitmap* pNodeBmp = new NodeBitmap();
	ERROR2IF(!pNodeBmp, NULL, "Failed to create NodeBitmap");

	pNodeBmp->SetUpPath();
	pNodeBmp->CreateShape(SpanBounds);
	pNodeBmp->BitmapRef.Attach(pRealBmp);		// Attach the correct bitmap
	pNodeBmp->ApplyDefaultBitmapAttrs(NULL);	// Apply the correct attrs

	// If we should be applying a non-mix transparency then do so
	if (TransToApply != TT_NoTranspType && TransToApply != TT_Mix)
	{
		AttrFlatTranspFill* pTrans = new AttrFlatTranspFill(pNodeBmp, FIRSTCHILD);
		if (pTrans)
		{
			pTrans->SetTranspType(TransToApply);
			UINT32 TransVal = 0;
			pTrans->SetStartTransp(&TransVal);
		}
	}

	if (pSingleSpread)
	{
		// Copy the spread and create a default layer
		Spread* pNewSpread = (Spread*)(pSingleSpread->PublicCopy());
		ERROR2IF(!pNewSpread, NULL, "Failed to create Spread");

		Layer* pNewLayer = new Layer(pNewSpread, FIRSTCHILD, String_256("Layer 1"));
		ERROR2IF(!pNewLayer, NULL, "Failed to create Layer");

		// Attach the NodeBitmap as the first child of the new layer
		pNodeBmp->AttachNode(pNewLayer, FIRSTCHILD);

		// Return the new spread node
		return(pNewSpread);
	}

	if (pSingleLayer)
	{
		// Create a shallow copy of the layer
		Layer* pNewLayer = (Layer*)(pSingleLayer->PublicCopy());
		ERROR2IF(!pNewLayer, NULL, "Failed to create Layer");

		// Attach the NodeBitmap as the first child of the new layer
		pNodeBmp->AttachNode(pNewLayer, FIRSTCHILD);

		// Return the new layer node
		return(pNewLayer);
	}

	// Just return the NodeBitmap
	return(pNodeBmp);
}



/****************************************************************************

>	KernelBitmap* XPFRenderCallback::RenderFillToBitmap(Node* pNode, DocRect& BoundsRect)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/10/2005

	Inputs:		pNode		- pointer to a Node
				BoundsRect	- 
	Returns:	NULL in times of grief
	Purpose:	Renders the fill applied to the object into a KernelBitmap 
				so that the fill can be replaced with a simple bitmap fill

****************************************************************************/

KernelBitmap* XPFRenderCallback::RenderFillToBitmap(Node* pNode, DocRect& BoundsRect)
{
	View* pView = View::GetCurrent();
	Spread* pSpread = pNode->FindParentSpread();

	Matrix ViewTrans;
	FIXED16 TempScale(1.0);
	double Dpi = m_pCapTree->GetRasteriseDPI();

	// Make sure that BoundsRect wont end up as a zero-sized rectangle
	double MPPerPix = 72000.0 / Dpi;
	if (BoundsRect.Width() < MPPerPix)
		BoundsRect.hi.x = BoundsRect.lo.x + (INT32)(MPPerPix + 0.5);
	if (BoundsRect.Height() < MPPerPix)
		BoundsRect.hi.y = BoundsRect.lo.y + (INT32)(MPPerPix + 0.5);

	GRenderBitmap BitmapRR(BoundsRect, ViewTrans, TempScale, 32, Dpi);
	BitmapRR.m_DoCompression = TRUE;
	BitmapRR.AttachDevice(pView, NULL, pSpread);

	TRACEUSER( "Gerry", _T("Rendering fill as bitmap (%d, %d)\n"), (INT32)((double)BoundsRect.Width() / MPPerPix), (INT32)((double)BoundsRect.Height() / MPPerPix));

	// Start rendering into the bitmap
	if (!BitmapRR.StartRender())
	{
		ERROR2(NULL, "StartRender failed in RenderFillToBitmap");
	}

	BitmapRR.SaveContext();

	// Best quality please
	QualityAttribute *pQualAttr = new QualityAttribute();
	pQualAttr->QualityValue.SetQuality(QUALITY_MAX);
	BitmapRR.SetQuality(pQualAttr, TRUE);

	NodeAttribute* pAttr;
	pAttr = FindChildAttr(pNode, CC_RUNTIME_CLASS(AttrFillGeometry));
	if (pAttr)
		pAttr->Render(&BitmapRR);
	pAttr = FindChildAttr(pNode, CC_RUNTIME_CLASS(AttrFillMapping));
	if (pAttr)
		pAttr->Render(&BitmapRR);
	pAttr = FindChildAttr(pNode, CC_RUNTIME_CLASS(AttrFillEffect));
	if (pAttr)
		pAttr->Render(&BitmapRR);

	BitmapRR.SetLineColour(COLOUR_NONE);

	Path RectPath;
	if (RectPath.Initialise())
	{
		// Start at bottom left corner
		RectPath.InsertMoveTo(BoundsRect.lo);
		RectPath.InsertLineTo(DocCoord(BoundsRect.hi.x, BoundsRect.lo.y));
		RectPath.InsertLineTo(BoundsRect.hi);
		RectPath.InsertLineTo(DocCoord(BoundsRect.lo.x, BoundsRect.hi.y));
		RectPath.InsertLineTo(BoundsRect.lo);

		// Close the path properly
		RectPath.CloseSubPath();
		RectPath.IsFilled = TRUE;
		RectPath.IsStroked = FALSE;
		BitmapRR.DrawPath(&RectPath);
	}
	else
	{
		pNode->Render(&BitmapRR);
	}

	BitmapRR.RestoreContext();

	// Stop rendering
	BitmapRR.StopRender();

	OILBitmap* pFullBitmap = BitmapRR.ExtractBitmap();
	String_256 BmpName = m_pFilter->GetNewBitmapName();
	pFullBitmap->SetName(BmpName);
	KernelBitmap* pRealBmp = KernelBitmap::MakeKernelBitmap(pFullBitmap);

	// Attach the bitmap to this document or a copy will be created when 
	// it is used in the attribute
	BitmapList* pBmpList = NULL;
	Document* pCurDoc = Document::GetCurrent();
	if (pCurDoc)
		pBmpList = pCurDoc->GetBitmapList();

	// and then attach the bitmap (doesn't matter if BmpList is NULL)
	pRealBmp->Attach(pBmpList);

	return(pRealBmp);
}



/****************************************************************************

>	KernelBitmap* XPFRenderCallback::RenderTransToBitmap(Node* pNode, DocRect& BoundsRect, UINT32* pTransType)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/10/2005

	Inputs:		pNode		- pointer to a Node
				BoundsRect	- 
				pTransType	- pointer to a UINT32
	Returns:	NULL in times of grief
	Purpose:	Renders the transparency applied to the object as a greyscale 
				bitmap so that the transparency can be replaced by a simple 
				bitmap transparency

****************************************************************************/

KernelBitmap* XPFRenderCallback::RenderTransToBitmap(Node* pNode, DocRect& BoundsRect, UINT32* pTransType)
{
	View* pView = View::GetCurrent();
	Spread* pSpread = pNode->FindParentSpread();

	Matrix ViewTrans;
	FIXED16 TempScale(1.0);
	double Dpi = m_pCapTree->GetRasteriseDPI();

	// Make sure that BoundsRect wont end up as a zero-sized rectangle
	double MPPerPix = 72000.0 / Dpi;
	if (BoundsRect.Width() < MPPerPix)
		BoundsRect.hi.x = BoundsRect.lo.x + (INT32)(MPPerPix + 0.5);
	if (BoundsRect.Height() < MPPerPix)
		BoundsRect.hi.y = BoundsRect.lo.y + (INT32)(MPPerPix + 0.5);

	GRenderBitmap BitmapRR(BoundsRect, ViewTrans, TempScale, 32, Dpi);
	BitmapRR.AttachDevice(pView, NULL, pSpread);

	TRACEUSER( "Gerry", _T("Rendering trans as bitmap (%d, %d)\n"), (INT32)((double)BoundsRect.Width() / MPPerPix), (INT32)((double)BoundsRect.Height() / MPPerPix));

	// Start rendering into the bitmap
	if (!BitmapRR.StartRender())
	{
		ERROR2(NULL, "StartRender failed in RenderTransToBitmap");
	}

	// Should draw a big white rectangle here, into the bitmap render region or
	// transparent objects will not fade to white where you can see the paper under them
	DocRect DrawRect = BoundsRect;
	// Inflate the rect by 2 pixels
	DrawRect.Inflate( (INT32)(2*72000.0/Dpi + 0.5) );

	// Draw it into the real bitmap
	BitmapRR.SaveContext();
	BitmapRR.SetFillColour(COLOUR_WHITE);
	BitmapRR.DrawRect(&DrawRect);

	// Best quality please
	QualityAttribute *pQualAttr = new QualityAttribute();
	pQualAttr->QualityValue.SetQuality(QUALITY_MAX);
	BitmapRR.SetQuality(pQualAttr, TRUE);

	NodeAttribute* pAttr;
	pAttr = FindChildAttr(pNode, CC_RUNTIME_CLASS(AttrTranspFillGeometry));
	if (pAttr)
	{
		// It might be safer to create a copy of the attribute so that 
		// the transparency type can be changed without affecting the existing 
		// attribute but the attribute is going to get deleted anyway
		TranspFillAttribute* pVal = (TranspFillAttribute*)pAttr->GetAttributeValue();
		if (pTransType)
			*pTransType = pVal->GetTranspType();
		pVal->SetTranspType(TT_Mix);
		pAttr->Render(&BitmapRR);
	}
	pAttr = FindChildAttr(pNode, CC_RUNTIME_CLASS(AttrTranspFillMapping));
	if (pAttr)
		pAttr->Render(&BitmapRR);

	BitmapRR.SetFillColour(COLOUR_BLACK);
	BitmapRR.SetLineColour(COLOUR_NONE);

	Path RectPath;
	if (RectPath.Initialise())
	{
		// Start at bottom left corner
		RectPath.InsertMoveTo(BoundsRect.lo);
		RectPath.InsertLineTo(DocCoord(BoundsRect.hi.x, BoundsRect.lo.y));
		RectPath.InsertLineTo(BoundsRect.hi);
		RectPath.InsertLineTo(DocCoord(BoundsRect.lo.x, BoundsRect.hi.y));
		RectPath.InsertLineTo(BoundsRect.lo);

		// Close the path properly
		RectPath.CloseSubPath();
		RectPath.IsFilled = TRUE;
		RectPath.IsStroked = FALSE;
		BitmapRR.DrawPath(&RectPath);
	}
	else
	{
		pNode->Render(&BitmapRR);
	}

	BitmapRR.RestoreContext();

	// Stop rendering
	BitmapRR.StopRender();

	OILBitmap* pFullBitmap = BitmapRR.ExtractBitmap();
	String_256 BmpName = m_pFilter->GetNewBitmapName();
	pFullBitmap->SetName(BmpName);
	KernelBitmap* pRealBmp = KernelBitmap::MakeKernelBitmap(pFullBitmap);

	// Attach the bitmap to this document or a copy will be created when 
	// it is used in the attribute
	BitmapList* pBmpList = NULL;
	Document* pCurDoc = Document::GetCurrent();
	if (pCurDoc)
		pBmpList = pCurDoc->GetBitmapList();

	// and then attach the bitmap (doesn't matter if BmpList is NULL)
	pRealBmp->Attach(pBmpList);

	return(pRealBmp);
}



/****************************************************************************

>	KernelBitmap* XPFRenderCallback::RenderFillAndTransToBitmap(Node* pNode, DocRect& BoundsRect)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/10/2005

	Inputs:		pNode		- pointer to a Node
				BoundsRect	- 
	Returns:	NULL in times of grief
	Purpose:	Renders the applied fill and transparency to a bitmap so 
				both the fill and trans can be replaced by a simple bitmap 
				fill.  If the transparency is non-alpha compatible then 
				we also have to render all the previous objects into it

****************************************************************************/

KernelBitmap* XPFRenderCallback::RenderFillAndTransToBitmap(Node* pNode, DocRect& BoundsRect)
{
	BOOL bBackground = FALSE;
	NodeAttribute* pTransAttr = FindChildAttr(pNode, CC_RUNTIME_CLASS(AttrTranspFillGeometry));
	if (pTransAttr)
	{
		TranspFillAttribute* pTrans = (TranspFillAttribute*)(pTransAttr->GetAttributeValue());
		UINT32 Type = pTrans->GetTranspType();
		if (Type != TT_NoTranspType &&
			Type != TT_Mix &&
			Type != TT_DARKEN &&
			Type != TT_LIGHTEN &&
			Type != TT_BRIGHTNESS &&
			Type != TT_BEVEL)
		{
			bBackground = TRUE;
		}
	}
	
	if (!m_pCapTree->GetRasteriseAlpha())
		bBackground = TRUE;

	View* pView = View::GetCurrent();
	Spread* pSpread = pNode->FindParentSpread();

	Matrix ViewTrans;
	FIXED16 TempScale(1.0);
	double Dpi = m_pCapTree->GetRasteriseDPI();

	// Make sure that BoundsRect wont end up as a zero-sized rectangle
	double MPPerPix = 72000.0 / Dpi;
	if (BoundsRect.Width() < MPPerPix)
		BoundsRect.hi.x = BoundsRect.lo.x + (INT32)(MPPerPix + 0.5);
	if (BoundsRect.Height() < MPPerPix)
		BoundsRect.hi.y = BoundsRect.lo.y + (INT32)(MPPerPix + 0.5);

	GRenderBitmap BitmapRR(BoundsRect, ViewTrans, TempScale, 32, Dpi);
	if (!bBackground)
		BitmapRR.m_DoCompression = TRUE;
	BitmapRR.AttachDevice(pView, NULL, pSpread);

	TRACEUSER( "Gerry", _T("Rendering fill and trans as bitmap (%d, %d)\n"), (INT32)((double)BoundsRect.Width() / MPPerPix), (INT32)((double)BoundsRect.Height() / MPPerPix));

	// Start rendering into the bitmap
	if (!BitmapRR.StartRender())
	{
		ERROR2(NULL, "StartRender failed in RenderFillAndTransToBitmap");
	}

	if (bBackground)
	{
		// Should draw a big white rectangle here, into the bitmap render region or
		// transparent objects will not fade to white where you can see the paper under them
		DocRect DrawRect = BoundsRect;
		// Inflate the rect by 2 pixels
		DrawRect.Inflate( (INT32)(2*72000.0/Dpi + 0.5) );

		// Draw it into the real bitmap
		BitmapRR.SaveContext();
		BitmapRR.SetFillColour(COLOUR_WHITE);
		BitmapRR.DrawRect(&DrawRect);
		BitmapRR.RestoreContext();
	}

	BitmapRR.SaveContext();
	
	// Best quality please
	QualityAttribute *pQualAttr = new QualityAttribute();
	pQualAttr->QualityValue.SetQuality(QUALITY_MAX);
	BitmapRR.SetQuality(pQualAttr, TRUE);

	if (bBackground)
	{
		// We need to render the background objects first
		// Reuse existing code by finding the previous object and 
		// asking for it to be rendered with its background
		Node* pBackNode = pNode->FindPrevious();
		Node* pParent = pNode->FindParent();
		while (pBackNode == NULL)
		{
			if (pParent == NULL)
				break;
			pBackNode = pParent->FindPrevious();
			pParent = pParent->FindParent();
		}			
		
		if (pBackNode)
		{
			XPFSpanRenderCallback SpanCallback(pBackNode, pBackNode, TRUE);
			BitmapRR.RenderTree(pSpread, FALSE, FALSE, &SpanCallback);
		}
	}

	NodeAttribute* pAttr;
	pAttr = FindChildAttr(pNode, CC_RUNTIME_CLASS(AttrFillGeometry));
	if (pAttr)
		pAttr->Render(&BitmapRR);
	pAttr = FindChildAttr(pNode, CC_RUNTIME_CLASS(AttrFillMapping));
	if (pAttr)
		pAttr->Render(&BitmapRR);
	pAttr = FindChildAttr(pNode, CC_RUNTIME_CLASS(AttrFillEffect));
	if (pAttr)
		pAttr->Render(&BitmapRR);
	pAttr = FindChildAttr(pNode, CC_RUNTIME_CLASS(AttrTranspFillGeometry));
	if (pAttr)
		pAttr->Render(&BitmapRR);
	pAttr = FindChildAttr(pNode, CC_RUNTIME_CLASS(AttrTranspFillMapping));
	if (pAttr)
		pAttr->Render(&BitmapRR);

	BitmapRR.SetLineColour(COLOUR_NONE);

	Path RectPath;
	if (RectPath.Initialise())
	{
		// Start at bottom left corner
		RectPath.InsertMoveTo(BoundsRect.lo);
		RectPath.InsertLineTo(DocCoord(BoundsRect.hi.x, BoundsRect.lo.y));
		RectPath.InsertLineTo(BoundsRect.hi);
		RectPath.InsertLineTo(DocCoord(BoundsRect.lo.x, BoundsRect.hi.y));
		RectPath.InsertLineTo(BoundsRect.lo);

		// Close the path properly
		RectPath.CloseSubPath();
		RectPath.IsFilled = TRUE;
		RectPath.IsStroked = FALSE;
		BitmapRR.DrawPath(&RectPath);
	}
	else
	{
		pNode->Render(&BitmapRR);
	}

	BitmapRR.RestoreContext();

	// Stop rendering
	BitmapRR.StopRender();

	OILBitmap* pFullBitmap = BitmapRR.ExtractBitmap();
	String_256 BmpName = m_pFilter->GetNewBitmapName();
	pFullBitmap->SetName(BmpName);
	KernelBitmap* pRealBmp = KernelBitmap::MakeKernelBitmap(pFullBitmap);

	// Attach the bitmap to this document or a copy will be created when 
	// it is used in the attribute
	BitmapList* pBmpList = NULL;
	Document* pCurDoc = Document::GetCurrent();
	if (pCurDoc)
		pBmpList = pCurDoc->GetBitmapList();

	// and then attach the bitmap (doesn't matter if BmpList is NULL)
	pRealBmp->Attach(pBmpList);

	return(pRealBmp);
}


/****************************************************************************

>	BOOL XPFSpanRenderCallback::BeforeNode(RenderRegion* pRegion, Node* pNode)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/06/2006

	Inputs:		pRegion		- pointer to a RenderRegion
				pNode		- pointer to a Node
	Returns:	TRUE if ok, FALSE if bother
	Purpose:	

****************************************************************************/

BOOL XPFSpanRenderCallback::BeforeNode(RenderRegion* pRegion, Node* pNode)
{
//	char* pStateStr = (m_RenderState == RS_INSPAN) ? "in" : (m_RenderState == RS_AFTERSPAN) ? "after" : "before";
//	TRACE( _T("XPFSpanRC# BeforeNode    0x%08x - %s  %s\n"), pNode, pNode->GetRuntimeClass()->m_lpszClassName, pStateStr);
	
	BOOL bRender = FALSE;
	switch (m_RenderState)
	{
		case RS_BEFORESPAN:
			if (m_bBackground || pNode->IsAnAttribute() || pNode->IsANodeClipView())
			{
				// Let it render normally
				bRender = TRUE;
			}
			break;

		case RS_INSPAN:
			bRender = TRUE;
			break;
		
		case RS_AFTERSPAN:
			// Must skip everything until the end
			bRender = FALSE;
			break;

		default:
			TRACE( _T("XPFSpanRC# Bad RenderState in BeforeNode\n"));
			break;
	}			

//	TRACE( _T("XPFSpanRC# BeforeNode    0x%08x - %s	returning %s\n", pNode, pNode->GetRuntimeClass()->m_lpszClassName, bRender ? "true" : "false"));
	return(bRender);
}


/****************************************************************************

>	BOOL XPFSpanRenderCallback::BeforeSubtree(RenderRegion* pRegion, Node* pNode, Node** ppNextNode, BOOL bClip, SubtreeRenderState* pState)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/06/2006

	Inputs:		pRegion		- pointer to a RenderRegion
				pNode		- pointer to a Node
				ppNextNode	- pointer to a pointer to a Node
				bClip		- 
				pState		- pointer to a SubtreeRenderState
	Returns:	TRUE if ok, FALSE if bother
	Purpose:	

****************************************************************************/

BOOL XPFSpanRenderCallback::BeforeSubtree(RenderRegion* pRegion, Node* pNode, Node** ppNextNode, BOOL bClip, SubtreeRenderState* pState)
{
//	char* pStateStr = (m_RenderState == RS_INSPAN) ? "in" : (m_RenderState == RS_AFTERSPAN) ? "after" : "before";
//	TRACE( _T("XPFSpanRC# BeforeSubtree 0x%08x - %s  %s\n"), pNode, pNode->GetRuntimeClass()->m_lpszClassName, pStateStr);

	switch (m_RenderState)
	{
		case RS_BEFORESPAN:
			if (pNode == m_pFirstNode)
			{
//				TRACE( _T("XPFSpanRC# Start of span\n"));
				// Change state to be in the span
				m_RenderState = RS_INSPAN;
			}
			else if (!pNode->IsAnAttribute() && !m_bBackground && !pNode->IsNodeInSubtree(m_pFirstNode))
			{
				// The first node isn't in this subtree so don't bother rendering it
				*pState = SUBTREE_NORENDER;
				return(TRUE);
			}
			break;

		case RS_INSPAN:
			break;

		case RS_AFTERSPAN:
			// Don't render this subtree
			*pState = SUBTREE_NORENDER;
			return(TRUE);
			break;

		default:
			TRACE( _T("XPFSpanRC# Bad RenderState in BeforeSubtree\n"));
			break;
	}			
	
	return(FALSE);
}
		

/****************************************************************************

>	BOOL XPFSpanRenderCallback::AfterSubtree(RenderRegion* pRegion, Node* pNode)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/06/2006

	Inputs:		pRegion		- pointer to a RenderRegion
				pNode		- pointer to a Node
	Returns:	TRUE if ok, FALSE if bother
	Purpose:	

****************************************************************************/

BOOL XPFSpanRenderCallback::AfterSubtree(RenderRegion* pRegion, Node* pNode)
{
//	char* pStateStr = (m_RenderState == RS_INSPAN) ? "in" : (m_RenderState == RS_AFTERSPAN) ? "after" : "before";
//	TRACE( _T("XPFSpanRC# AfterSubtree    0x%08x - %s  %s\n"), pNode, pNode->GetRuntimeClass()->m_lpszClassName, pStateStr);

	// By default we do want RenderAfterSubtree to be called
	BOOL bStopRender = FALSE;
	switch (m_RenderState)
	{
		case RS_BEFORESPAN:
			if (!m_bBackground && !pNode->IsNodeInSubtree(m_pFirstNode))
			{
				// The first node isn't in this subtree so don't bother rendering it
				bStopRender = TRUE;
			}
			break;

		case RS_INSPAN:
			if (pNode == m_pLastNode)
			{
//				TRACE( _T("XPFSpanRC# End of span\n"));
				// Change state to be after the span
				m_RenderState = RS_AFTERSPAN;
			}
			break;
		
		case RS_AFTERSPAN:
			// Must skip everything until the end
			bStopRender = TRUE;
			break;

		default:
			TRACE( _T("XPFSpanRC# Bad RenderState in AfterSubtree\n"));
			break;
	}			
//	TRACE( _T("XPFSpanRC# AfterSubtree  0x%08x - %s	returning %s\n", pNode, pNode->GetRuntimeClass()->m_lpszClassName, bStopRender ? "true" : "false"));
	return(bStopRender);
}



CommonTransInfo::CommonTransInfo()
{
	m_bCommonType = TRUE;
	m_CommonType = TT_NoTranspType;
}

void CommonTransInfo::UpdateCommonType(UINT32 Type)
{
	if (m_bCommonType)
	{
		if (Type == TT_NoTranspType ||
			Type == TT_DARKEN ||
			Type == TT_LIGHTEN ||
			Type == TT_BRIGHTNESS ||
			Type == TT_BEVEL)
		{
			Type = TT_Mix;		// These are all mix
		}

		if (m_CommonType == TT_NoTranspType)
			m_CommonType = Type;
		else if (m_CommonType != Type)
			m_bCommonType = FALSE;
	}
}

UINT32 CommonTransInfo::GetCommonType()
{
	return(m_bCommonType ? m_CommonType : TT_NoTranspType);
}



/****************************************************************************

>	XPFView::~XPFView()

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/10/2005
	Purpose:	Destroys a XPFView, deallocating it's associated ViewState.

****************************************************************************/

XPFView::~XPFView()
{
	delete pVState;
	pVState = 0;
}



/********************************************************************************************

>	BOOL XPFView::Init(double DPI)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com> (copying Tim code)
	Created:	022/4/95

	Returns:	TRUE if initialised ok;
				FALSE if not

	Purpose:	Initialise the XPFView for rendering.

********************************************************************************************/

BOOL XPFView::Init(double DPI)
{
	// Store the DPI away
	m_DPI = DPI;
	
	// Set up our pixel size
	PixelWidth = FIXED16(72000.0 / DPI);
	PixelHeight = FIXED16(72000.0 / DPI);
	ScaledPixelWidth = PixelWidth;
	ScaledPixelHeight = PixelHeight;

	// Get ourselves a ViewState object
	pVState = new ViewState;
	if (pVState == NULL) 
		return FALSE;

	// Connect this view state to this view
	pVState->pView = this;

	// All ok
	return TRUE;
}



/********************************************************************************************

>	BOOL XPFView::ViewStateChanged()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com> (copying Tim code)
	Created:	22/4/95
	Returns:	TRUE (always).
	Purpose:	

********************************************************************************************/

BOOL XPFView::ViewStateChanged()
{
	// Local variables...
	DocCoord	ExtentLo;						// coord of bottom-left of logical extent
	DocCoord	ExtentHi;						// coord of top-right of logical extent
	
	// Read DocCoords extent of document & set extent in platform-indy ViewState struct.
	pDoc->GetExtents(&ExtentLo, &ExtentHi, &PhysExtent, this);
	SetExtent(ExtentLo, ExtentHi);				

	return TRUE;
}



/********************************************************************************************

>	void XPFView::SetViewPixelSize()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com> (copying Tim code)
	Created:	22/4/95
	Purpose:	Sets up the pixel size for this view object.
	SeeAlso:	XPFView

********************************************************************************************/

void XPFView::SetViewPixelSize()
{
	// Set the scaled pixel size
	ScaledPixelWidth = PixelWidth / Scale;
	ScaledPixelHeight = PixelHeight / Scale;
}



/********************************************************************************************

>	void XPFView::ContinueRenderView(RenderRegion*, Spread*, BOOL = TRUE, BOOL = TRUE,
									 BOOL bForceImmediate = FALSE)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com> (copying Tim code)
	Created:	22/4/95
	Inputs:		N/A
	Purpose:	None - XPFView is a skeleton class that is only used to access screen
				characteristics.
	Errors:		Always => ERROR3

********************************************************************************************/

void XPFView::ContinueRenderView(RenderRegion*, Spread*, BOOL, BOOL,
									 BOOL bForceImmediate)
{
	ERROR3("XPFView::ContinueRenderView() should not be called!");
}



/********************************************************************************************

>	CDC *XPFView::GetRenderDC()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com> (copying Tim code)
	Created:	22/4/95
	Returns:	NULL (always)
	Purpose:	None - XPFView is a skeleton class that is only used to access screen
				characteristics.

********************************************************************************************/

CNativeDC *XPFView::GetRenderDC()
{
	return NULL;
}



/********************************************************************************************

>	BOOL XPFView::GetForeBackMode()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com> (copying Tim code)
	Created:	22/4/95
	Returns:	FALSE - dialogs don't do background rendering.
	Purpose:	None - XPFView is a skeleton class that is only used to access screen
				characteristics.

********************************************************************************************/

BOOL XPFView::GetForeBackMode()
{
	// XPFViews don't background render.
	return FALSE;
}



/********************************************************************************************

>	void XPFView::SetForeBackMode(BOOL)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com> (copying Tim code)
	Created:	22/4/95
	Inputs:		N/A
	Purpose:	None - XPFView is a skeleton class that is only used to access screen
				characteristics.
	Errors:		ERROR3 always - dialogs cannot background render!
	SeeAlso:	XPFView

********************************************************************************************/

void XPFView::SetForeBackMode(BOOL)
{
	ERROR3("XPFView::SetForeBackMode() should not be called!");
}



/********************************************************************************************

>	DocRect XPFView::GetDocViewRect(Spread*)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com> (copying Tim code)
	Created:	22/4/95
	Inputs:		N/A
	Returns:	The bounds of the objects in the InternalClipboard
	Purpose:	To determine the docrect area that this view is viewing

********************************************************************************************/

DocRect XPFView::GetDocViewRect(Spread* pSpread)
{
	return(pSpread->GetBoundingRect());
}



/********************************************************************************************

>	void XPFView::SetExtent(DocCoord, DocCoord)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com> (copying Tim code)
	Created:	22/4/95
	Inputs:		Two DocCoords describing the low and high corners of a rectangle in logical
				space describing the full extent of the document. I.E. the bottom-left corner
				of the last chapter and the top-right corner of the first.
	Purpose:	Informs the DocView that the extent of the document has changed in some way and 
				that the view must be altered to take this into account (namely the 
				scrollbars).
	SeeAlso:	XPFView

********************************************************************************************/

void XPFView::SetExtent(DocCoord lolog, DocCoord hilog)
{
	// Convert the extent given in DocCoord to WorkCoords...
	pVState->WorkAreaExtent.lo = lolog.ToWork(pDoc, this);
	pVState->WorkAreaExtent.hi = hilog.ToWork(pDoc, this);
}



/********************************************************************************************

>	WorkRect XPFView::GetViewRect()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com> (copying Tim code)
	Created:	22/4/95
	Returns:	An empty rectangle (always).
	Purpose:	None - XPFView is a skeleton class that is only used to access screen
				characteristics.

********************************************************************************************/

WorkRect XPFView::GetViewRect()
{
	WorkRect Empty;
	return Empty;
}



/****************************************************************************

>	double XPFView::GetConvertToEditableShapesDPI()

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/08/2005

	Returns:	The resolution to use for convert to editable shapes 
				operations while this view is current
	Purpose:	Allows derived view classes to override the reolution

****************************************************************************/

double XPFView::GetConvertToEditableShapesDPI()
{
	return(m_DPI);
}
