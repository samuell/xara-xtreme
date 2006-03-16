// $Id: ngitem.cpp 662 2006-03-14 21:31:49Z alex $
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
/*
	$Header: /Camelot/kernel/ngitem.cpp 17    22/11/04 18:55 Phil $
	Attribute gallery display items
*/

#include "camtypes.h"

#include "ngcore.h"
#include "ngdrag.h"
#include "ngitem.h"
#include "ngprop.h"
#include "ngscan.h"
#include "ngsentry.h"
#include "ngsetop.h"

#include "app.h"
#include "docrect.h"
#include "ink.h"
#include "rndrgn.h"
#include "dragmgr.h"
#include "keypress.h"

#include "bitmap.h"				// bitmaps
#include "nodebmp.h"
#include "fillattr.h"

#include "cxfrech.h"			// names
#include "attrval.h"
#include "userattr.h"
#include "tmpltatr.h"

#include "fontman.h"			// fonts
#include "txtattr.h"
#include "nodetxts.h"
#include "nodetxtl.h"
#include "nodetext.h"

#include "doccolor.h"			// colours
#include "colourix.h"

//#include "galres.h"
//#include "mario.h"
//#include "justin3.h"
#include "slicehelper.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

DECLARE_SOURCE("$Revision: 662 $");

// Implement the dynamic class bits...
CC_IMPLEMENT_DYNAMIC(SGNameItem,	SGDisplayItem);
CC_IMPLEMENT_DYNAMIC(SGNameGroup,	SGDisplayGroup);

// This line mustn't go before any CC_IMPLEMENT_... macros
#define new CAM_DEBUG_NEW


/***********************************************************************************************
>   SGNameItem::SGNameItem(const StringBase& strName)

	Author:     Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:    27/6/99
	Inputs:     strName - the name ('attribute value') of this set item.
	Purpose:    Construct an SGNameItem.
***********************************************************************************************/

SGNameItem::SGNameItem(const StringBase& strName)
  : m_strName(strName),
	m_nNodes(0),
	m_nObjects(0),
	m_nSelected(0),
	m_pCachedPropertyNode(0)
{
//  TRACEUSER( "JustinF", _T("SGNameItem::SGNameItem(%s)\n"), (LPCTSTR) m_strName);
}



/***********************************************************************************************
>   virtual void SGNameItem::GetNameText(String_256* pResult)

	Author:     Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:    9/3/99
	Outputs:    On exit, the string pointed at by Result will contain either a blank
				string, or the name text associated with this item (if any)
	Purpose:    To determine a name string for this node. Generally, this is used for
				a simple mechanism which searches for display items whose names match
				given search parameters in some way. It is also used in libraries to
				provide default redraw methods.
	Notes:      **** TO DO ****
				Modify this method to return the correct text. You may also want to
				add an override for the GetFullInfoText() method if you can provide
				a full-info display mode.
	SeeAlso:    SGDisplayNode::GetNameText
***********************************************************************************************/

void SGNameItem::GetNameText(String_256* pResult)
{
	ERROR3IF(pResult == 0, "SGNameItem::GetNameText: illegal null param");
	*pResult = m_strName;
}



/***********************************************************************************************
>   SGNameProp* SGNameItem::GetProperty(INT32 nIndex = -1) const

	Author:     Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:    27/6/99
	Inputs:     nIndex    ---     the index of the property to retrieve (default is the
								  current property)
	Returns:	The property associated with this item of the given type (index).  NB. the
				item remains responsible for deallocation of the property.
	SeeAlso:	SGNameProp; NodeSetProperty; NodeSetSentinel
***********************************************************************************************/

SGNameProp* SGNameItem::GetProperty(INT32 nIndex) const
{
	// No document means no properties.
	Document* pDoc = Document::GetSelected();
	if (pDoc == 0) return 0;

	// Only 'Used Names' have properties (for now).
	if (GetParent() != NameGallery::Instance()->GetUsedNames())
		return 0;

	// Fetch the current property?
	if (nIndex == -1) nIndex = NameGallery::Instance()->GetPropertyIndex();
	ERROR3IF(nIndex < 0 || nIndex >= SGNameProp::nPropertyCount,
				"SGNameItem::GetProperty: index out of range");

	// Retrieve the SGNameProp* from the associated NodeSetProperty.
	NodeSetProperty* pSetProp = pDoc->GetSetSentinel()->FindPropertyNode(m_strName);
	if (pSetProp == 0) return 0;
	SGNameProp* pProp = pSetProp->GetProperty(nIndex);
	ERROR3IF(pProp == 0, "SGNameItem::GetProperty: no property");
	return pProp;
}



/***********************************************************************************************
>   void SGNameItem::Reset(BOOL fPropagateChanges)

	Author:     Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:    27/6/99
	Inputs:		fPropagateChanges  ---	TRUE if all bounds records should be sync'ed (?)
	Purpose:    Resets the state of the object in preparation for a refresh
***********************************************************************************************/

void SGNameItem::Reset(BOOL fPropagateChanges)
{
	// Remember the old bounds of the set and reset all running totals.
	if (fPropagateChanges) m_rOldSetBounds = m_rSetBounds;
	m_rSetBounds.MakeEmpty();
	m_rSelectBounds.MakeEmpty();
	m_nNodes = m_nObjects = m_nSelected = 0;
	m_pCachedPropertyNode = 0;
}


/***********************************************************************************************
>   NodeSetProperty* SGNameItem::GetPropertyNode()

	Author:     Simon_Knight (Xara Group Ltd) <camelotdev@xara.com>
	Created:    17/3/00
	Purpose:    returns a cached ptr to this sets property node.
				This is NULLed in Reset() and filled with a search on demand
***********************************************************************************************/
NodeSetProperty* SGNameItem::GetPropertyNode()
{
	// scan for the property node and cache this ptr
	return (m_pCachedPropertyNode != 0)
				? m_pCachedPropertyNode
				: m_pCachedPropertyNode =
						Document::GetCurrent()->GetSetSentinel()->FindPropertyNode(m_strName);
}	



/***********************************************************************************************
>   void SGNameItem::Include(Node* pNode)

	Author:     Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:    27/6/99
	Inputs:     pNode    ---     the node to "register"
	Purpose:    Called by the CreateScan::Do when a node is discovered that is a member
				of this set item.  Updates the running totals of member objects and
				selected member objects.
***********************************************************************************************/

void SGNameItem::Include(Node* pNode)
{
	// Count nodes, including the sentinel.
	++m_nNodes;
	if (pNode->IsNodeRenderableClass())
	{
		// Count selectable (renderable) nodes.
		++m_nObjects;
		BOOL fSelect = (pNode->IsSelected() || pNode->IsChildOfSelected()) || (pNode->IsCompound() && pNode->IsParentOfSelected());
		if (fSelect) ++m_nSelected;

		// Accumulate bounds of objects, and just those selected.
		if (pNode->IsBounded())
		{
			DocRect rBounds = SliceHelper::BoundingNodeSize(pNode);
			m_rSetBounds = m_rSetBounds.Union(rBounds);
			if (fSelect) m_rSelectBounds = m_rSelectBounds.Union(rBounds);
		}
	}
}



/***********************************************************************************************
>   virtual INT32 SGNameItem::CompareTo(SGDisplayNode* pOther, INT32 nKey)

	Author:     Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:    27/6/99
	Inputs:     (see SGDisplayNode::CompareTo)
	Returns:	-1 if comparison is <
				0 if =
				+1 if >
	Purpose:	Overrides the default comparator so that it takes into accounted 
				appended numbers.
***********************************************************************************************/

INT32 SGNameItem::CompareTo(SGDisplayNode* pItem, INT32 nKey)
{
	ERROR3IF(pItem == 0, "SGNameItem::CompareTo: null input");
	ERROR3IF(!pItem->IS_KIND_OF(SGNameItem), "SGNameItem::CompareTo: not an SGNameItem");
	
	switch (nKey)
	{
	case SGSORTKEY_BYNAME:
		{
			// Compare the strings character by character until we can determine their
			// lexical order.  If the characters to be compared are numeric, then parse
			// and compare them as ordinals.
			LPCTSTR pchThis = m_strName, pchOther = ((SGNameItem*) pItem)->m_strName;
			for (;;)
			{
				// Eat up any whitespace.
				while (StringBase::IsSpace(*pchThis)) pchThis = _tcsinc(pchThis);
				while (StringBase::IsSpace(*pchOther)) pchOther = _tcsinc(pchOther);
			
				// Check if we've reached the end of the strings.  If we have reached
				// the end of both, they are equal.  Otherwise the one that is shortest
				// is 'lower'.
				if (*pchThis == TEXT('\0'))
					return (*pchOther == TEXT('\0')) ? 0 : -1;
				else if (*pchOther == TEXT('\0'))
					return 1;

				// Are the next two characters to be compared numeric?
				if (StringBase::IsNumeric(*pchThis) && StringBase::IsNumeric(*pchOther))
				{
					// Span and copy the integers embedded in each string.
					String_256 strThisNum, strOtherNum;
					LPTSTR pchThisNum = strThisNum, pchOtherNum = strOtherNum;
					
					do {
						*pchThisNum = *pchThis;
						pchThisNum = _tcsinc(pchThisNum);
						pchThis = _tcsinc(pchThis);
					} while (StringBase::IsNumeric(*pchThis));

					do {
						*pchOtherNum = *pchOther;
						pchOtherNum = _tcsinc(pchOtherNum);
						pchOther = _tcsinc(pchOther);
					} while (StringBase::IsNumeric(*pchOther));

					*pchThisNum = *pchOtherNum = TEXT('\0');

					// Convert and compare the numbers.
					INT32 nThis, nOther;
					Convert::StringToLong(&strThisNum, &nThis);
					Convert::StringToLong(&strOtherNum, &nOther);
					if (nThis < nOther)
						return -1;
					else if (nThis > nOther)
						return 1;

					// Restart all tests as we may now be pointing at a null or a space.
					continue;
				}

				// The characters are not both numeric, so perform a normal comparison.
				INT32 nTest = ::CompareString(LOCALE_USER_DEFAULT,
											NORM_IGNOREKANATYPE |
											NORM_IGNOREWIDTH |
											NORM_IGNORECASE,
											pchThis, 1, pchOther, 1) - 2;

				if (nTest != 0) return nTest;

				// The strings are still equal so compare the next characters.
				pchThis = _tcsinc(pchThis);
				pchOther = _tcsinc(pchOther);
			}
		}
		break;

	default:
		// Let the base class handle all other types of comparison.
		return SGDisplayItem::CompareTo(pItem, nKey);
	}

	return 0;
}



/***********************************************************************************************
>   virtual void SGNameItem::DragWasReallyAClick(SGMouseInfo* pMouseInfo, SGMiscInfo* pMiscInfo)

	Author:     Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:    27/6/99
	Inputs:     (see SGDisplayNode::DragWasReallyACick)
	Purpose:	Called by the dragging code when a drag turns out to be a
				mundane single click.
***********************************************************************************************/

void SGNameItem::DragWasReallyAClick(SGMouseInfo* pMouseInfo, SGMiscInfo* pMiscInfo)
{
	// NB. called post-drag.
	DefaultClickHandler(pMouseInfo, pMiscInfo, TRUE);
}



/********************************************************************************************
>   virtual BOOL SGNameItem::GetBubbleHelp(DocCoord *pMousePos, String_256 *pResult)

	Author:     Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:    16/4/99
	Inputs:     pMousePos - The current mouse position. This will generally be expected
				to lie inside this item's FormatRect. With it, this item can provide
				help on specific areas of an item.
	Outputs:    On exit, if the return value is TRUE, the string pointed at by Result
				will contain a bubble help string for this item
	Returns:    TRUE if it filled in the string, FALSE if it did not
	Purpose:    Called by the parent gallery when bubble help is needed. The parent
				gallery will do a hit test to determine which node contains the pointer,
				and will then ask that node to supply bubble/status-line help.
	Notes:      The base class returns FALSE (i.e. provides no help)
				If you can provide help, then override the base class method to do so.
	SeeAlso:    SGDisplayNode::GetStatusLineHelp
********************************************************************************************/

BOOL SGNameItem::GetBubbleHelp(DocCoord* pMousePos, String_256* pResult)
{
	ERROR3IF(pResult == 0, "SGNameItem::GetBubbleHelp: invalid null params");

	// Matt - 16/09/2000 - Added for tooltip help over properties
	// Work out which UI item is hovered over, if any...
	if (m_nObjects > 0 && m_rProp.ContainsCoord(*pMousePos))
	{
		//Then we're hovering over the properties section...
		SGNameProp* pProp = GetProperty();
		if (pProp->GetIndex() == 0)
		{
			//Then it's an EXPORT property
			TRACEUSER( "matt", _T("...EXPORTY...\n"));
		}
		else if (pProp->GetIndex() == 1)
		{
			//Then it's a SLICE tickbox
			TRACEUSER( "matt", _T("SLICEY...\n"));
		}
		else if (pProp->GetIndex() == 2)
		{
			//Then it's a STRETCH tickbox
			TRACEUSER( "matt", _T("...STRETCHY...\n"));
		}

		UINT32 id;
		id = _R(IDBBL_NAMEGAL_NONE);
		return pResult->MakeMsg(_R(IDBBL_NAMEGAL_ITEM), &String(id), &m_strName);
	}
	else
	{
		// Work out the intersection with the user selection.
		UINT32 id;
		if (IsNoneSelected())
			id = _R(IDBBL_NAMEGAL_NONE);
		else if (IsAllSelected())
			id = _R(IDBBL_NAMEGAL_ALL);
		else
			id = _R(IDBBL_NAMEGAL_SOME);

		// Create the bubble help text.
		return pResult->MakeMsg(_R(IDBBL_NAMEGAL_ITEM), &String(id), &m_strName);
	}
}


	
/**********************************************************************************************
>   virtual BOOL SGNameItem::GetStatusLineHelp(DocCoord* pMousePos, String_256* pResult)

	Author:     Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:    16/4/99
	Inputs:     pMousePos - The current mouse position. This will generally be expected
				to lie inside this item's FormatRect. With it, this item can provide
				help on specific areas of an item.
	Outputs:    On exit, if the return value is TRUE, the string pointed at by Result
				will contain a status line help string for this item
	Returns:    TRUE if it filled in the string, FALSE if it did not            
	Purpose:    Called by the parent gallery when status line help is needed. The parent
				gallery will do a hit test to determine which node contains the pointer,
				and will then ask that node to supply bubble/status-line help.          
	Notes:      The base class returns FALSE (i.e. provides no help)
				If you can provide help, then override the base class method to do so.
	SeeAlso:    SGDisplayNode::GetBubbleHelp
**********************************************************************************************/

BOOL SGNameItem::GetStatusLineHelp(DocCoord* pMousePos, String_256* pResult)
{
	// Matt - 16/09/2000 - Added for statusbar help over properties
	// Work out which UI item is hovered over, if any...
	if (m_nObjects > 0 && m_rProp.ContainsCoord(*pMousePos))
	{
		//Then we're hovering over the properties section...
		SGNameProp* pProp = GetProperty();
		if (pProp->GetIndex() == 0)
		{
			//Then it's an EXPORT property
			TRACEUSER( "matt", _T("...EXPORTY...\n"));
			return pResult->MakeMsg(_R(IDBBL_NAMEGAL_PROPEXPORTOPT));
		}
		else if (pProp->GetIndex() == 1)
		{
			//Then it's a SLICE tickbox
			TRACEUSER( "matt", _T("SLICEY...\n"));
			return pResult->MakeMsg(_R(IDBBL_NAMEGAL_PROPSLICE));
		}
		else if (pProp->GetIndex() == 2)
		{
			//Then it's a STRETCH tickbox
			TRACEUSER( "matt", _T("...STRETCHY...\n"));
			return pResult->MakeMsg(_R(IDBBL_NAMEGAL_PROPSTRETCH));
		}
	}
	else
	{
		String strSel;

		// Intersect this item's set with the selected objects.
		if (IsNoneSelected())
			strSel = _R(IDBBL_NAMEGAL_NONE);
		else if (IsAllSelected())
			strSel = _R(IDBBL_NAMEGAL_ALL);
		else
			strSel = _R(IDBBL_NAMEGAL_SOME);

		strSel.toLower();

		// What is the mouse pointing at?
		UINT32 idMask;
		SGNameGroup* pGroup = (SGNameGroup*) GetParent();
		if (m_rToggle.ContainsCoord(*pMousePos))
			// Over the selection toggle gadget.
			idMask = _R(IDST_NAMEGAL_ITEM_SEL_GADGET);
		else if (pGroup == NameGallery::Instance()->GetUsedNames())
			// Over a 'Used Name'
			idMask = _R(IDST_NAMEGAL_NAME_ITEM);
		else
			// Over 'Used' something else.
			idMask = _R(IDST_NAMEGAL_OTHER_ITEM);
	
		// Build up a specific message.
		String strType(pGroup->GetTypeID());
		return pResult->MakeMsg(idMask, &strSel, &m_strName, &strType, &strType);
	}

	// It failed, so return FALSE.
	return FALSE;
}



/***********************************************************************************************
>   virtual void SGNameItem::CalcUiBounds(SGFormatInfo* pFormatInfo, SGMiscInfo* pMiscInfo)

	Author:     Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:    27/6/99
	Inputs:     pFormatInfo ---		formatting info from which to calculate my position/size
				pMiscInfo	---		the window dimensions, pixel size etc
	Purpose:    Calculates the bounds of the distinct UI items that make up a line in
				the Attribute gallery.
	Notes:		Member variable FormatRect should be set up (before calling this method)
				to be the rectangle in which to draw this item.
	SeeAlso:	SGNameItem::HandleRedraw; SGNameItem::HandleEvent
***********************************************************************************************/

void SGNameItem::CalcUiBounds(SGFormatInfo* pFormatInfo, SGMiscInfo* pMiscInfo)
{
	// pMiscInfo can't be null (pFormatInfo can be as it's not very useful).
	ERROR3IF(pMiscInfo == 0, "SGNameItem::CalcUiBounds: null input/output argument");

	// Work out the maximum bounds of the toggle gadget, label and property.
	m_rText = FormatRect;
	m_rText.hi.x -= SG_GapBeforeText / 2;
	m_rToggle = m_rText;
	m_rToggle.hi.x = m_rToggle.lo.x + SG_DefaultSmallIcon;
	m_rText.lo.x = m_rToggle.hi.x + SG_GapBeforeText;

	// Split the label bounds with the property.
	SGNameProp* pProp = GetProperty();
	if (pProp == 0)
		m_rProp.MakeEmpty();
	else
	{
		m_rProp = m_rText;
		pProp->CalcUiBounds(this, pFormatInfo, pMiscInfo, &m_rProp);
		m_rText.hi.x = m_rProp.lo.x - SG_GapBeforeText;
	}

	// Pixel-align.
	GridLockRect(pMiscInfo, &m_rToggle);
	GridLockRect(pMiscInfo, &m_rText);
	GridLockRect(pMiscInfo, &m_rProp);
}



/***********************************************************************************************
>   virtual void SGNameItem::CalculateMyRect(SGFormatInfo* pFormatInfo, SGMiscInfo* pMiscInfo)

	Author:     Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:    27/6/99
	Inputs:     pFormatInfo - The formatting info from which to calculate my position/size
	Outputs:    pMiscInfo - As usual, the useful misc info struct
				member variable FormatRect - is returned filled in with the size/position of
				this NameItem item's display area. This is dependent upon the current display
				mode and format state. FormatInfo will be updated as a result of the formatting
				operation.
	Purpose:    Shared code for NameItem items to calculate where they will appear in the
				grand scheme of things
***********************************************************************************************/

void SGNameItem::CalculateMyRect(SGFormatInfo* pFormatInfo, SGMiscInfo* pMiscInfo)
{
	CalculateFormatRect(pFormatInfo, pMiscInfo, SG_InfiniteWidth, SG_DefaultSmallIcon);
	CalcUiBounds(pFormatInfo, pMiscInfo);
}



/***********************************************************************************************
>   virtual void SGNameItem::DrawLabel(SGRedrawInfo* pRedrawInfo, SGMiscInfo* pMiscInfo)

	Author:     Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:    27/6/99
	Inputs:     pRedrawInfo		---		the information on the kernel-rendered redraw area
				pMiscInfo		---		the drawing context information structure
	Purpose:    Default implementation for drawing the label of an item, the renderer
				has already been set up with default attributes.
	Notes:		Member variable FormatRect should be set up (before calling this method)
				to be the rectangle in which to draw this item.
	SeeAlso:	SGNameItem::CalcUiBounds
***********************************************************************************************/

void SGNameItem::DrawLabel(SGRedrawInfo* pRedrawInfo, SGMiscInfo* pMiscInfo)
{
	pRedrawInfo->Renderer->DrawFixedSystemText(&m_strName, m_rText);
}



/***********************************************************************************************
>   virtual void SGNameItem::HandleRedraw(SGRedrawInfo* pRedrawInfo,
										  SGFormatInfo* pFormatInfo)

	Author:     Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:    27/6/99
	Inputs:     pRedrawInfo		---		the information on the kernel-rendered redraw area
				pFormatInfo		---		the formatting information structure
	Purpose:    SGNameItem item redraw method - removed from the main HandleEvent
				method merely to make the code tidier.
	Notes:		Member variable FormatRect should be set up (before calling this method)
				to be the rectangle in which to draw this item.
	SeeAlso:	SGNameItem::CalcUiBounds
***********************************************************************************************/

void SGNameItem::HandleRedraw(SGRedrawInfo* pRedrawInfo, SGMiscInfo* pMiscInfo)
{
	// Set the foreground and background colours according to whether this item is
	// gallery selected or not.
	DocColour dcolForegnd, dcolBackgnd;
	if (Flags.Selected)
	{
		dcolForegnd = pRedrawInfo->SelForeground;
		dcolBackgnd = pRedrawInfo->SelBackground;
	}
	else
	{
		// Unselected items are in the 'selected' colour if the user selection includes
		// any of their members.
		dcolBackgnd = pRedrawInfo->Background;
		dcolForegnd = pRedrawInfo->Foreground;
	}

	// Render the unhighlighted background.
	pRedrawInfo->Renderer->SetFillColour(pRedrawInfo->Background);
	pRedrawInfo->Renderer->SetLineColour(pRedrawInfo->Transparent);
	pRedrawInfo->Renderer->SetLineWidth(0);
	pRedrawInfo->Renderer->DrawRect(&m_rToggle);
	pRedrawInfo->Renderer->DrawRect(&m_rProp);

	// Render highlighted background.
	pRedrawInfo->Renderer->SetFillColour(dcolBackgnd);
	pRedrawInfo->Renderer->DrawRect(&m_rText);

	// Render the selection toggle gadget, on the left, if the set isn't empty.
	if (!IsEmpty())
	{
		UINT32 idBmp;
		if (IsNoneSelected())
			idBmp = _R(IDB_NAMEGAL_NONESEL);
		else if (IsAllSelected())
			idBmp = _R(IDB_NAMEGAL_ALLSEL);
		else
			idBmp = _R(IDB_NAMEGAL_SOMESEL);

		pRedrawInfo->Renderer->DrawBitmap(m_rToggle.lo, idBmp);
	}

	// Render the label of the item, in the middle.
	pRedrawInfo->Renderer->SetFixedSystemTextColours(&dcolForegnd, &dcolBackgnd);
	DrawLabel(pRedrawInfo, pMiscInfo);

	// Render the property, on the right.
	if (m_nObjects > 0)
	{
		SGNameProp* pProp = GetProperty();
		if (pProp != 0) pProp->HandleRedraw(this, pRedrawInfo, pMiscInfo, m_rProp);
	}
}



/***********************************************************************************************
>   virtual BOOL SGNameItem::HandleEvent(SGEventType nEventType, void* pEventInfo,
										 SGMiscInfo* pMiscInfo)
	Author:     Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:    17/4/99
	Inputs:     nEventType  - an enumerated value describing what type of event is to be processed
				pEventInfo  - a structure describing the event (may be 0). The exact thing
							  pointed at by this pointer depends upon the event type:
					
					SGEVENT_FORMAT      0
					SGEVENT_REDRAW      (SGRedrawInfo*)
					SGEVENT_BGREDRAW    0
					SGEVENT_BGFLUSH     0                   - May have 0 MiscInfo
					SGEVENT_MOUSECLICK  (SGMouseInfo*)
					SGEVENT_DRAGSTARTED (DragMessage*)
					SGEVENT_CLAIMPOINT  (SGMouseInfo*)
					SGEVENT_THUMBMSG    (ThumbMessage*)     - May have 0 MiscInfo

				Use the provided SGDisplayNode::Get[Format]Info() inlines to retrieve this
				information - they provide useful error/type checking, and hide the cast

				pMiscInfo - almost always provided. Contains a few useful bits of info that
				may be needed for all event types. This may be 0 for special event types
				(see sgtree.h for the enum and information on which ones may pass 0 MiscInfo
				- currently this is _THUMBMSG and _BGFLUSH, neither of which should concern
				you - so as long as you only reference MiscInfo once you know the event type,
				you will be safe)

	Outputs:    *pFormatInfo is updated as appropriate
	Returns:    TRUE if the event was handled successfully, FALSE if it was not.
	Purpose:    Handles a SuperGallery display event.
	Notes:      VERY IMPORTANT: The rendering must be enclosed by calls to StartRendering
				and StopRendering, to ensure that rendering works properly (in the future
				we may change the redraw system to use a GRenderRegion for each individual
				item, rather than one global one for the window, for which these calls will
				be essential)
	SeeAlso:    SGDisplayItem::HandleEvent; SGNameItem::CalcUiBounds
***********************************************************************************************/

BOOL SGNameItem::HandleEvent(SGEventType nEventType, void* pEventInfo, SGMiscInfo* pMiscInfo)
{
	switch (nEventType)
	{
		case SGEVENT_FORMAT:
		{
			SGFormatInfo* pFormatInfo = GetFormatInfo(nEventType, pEventInfo);
			CalculateMyRect(pFormatInfo, pMiscInfo);
			CalcUiBounds(0, pMiscInfo);
			break;
		}

		case SGEVENT_REDRAW:
		{
			DocRect MyRect(FormatRect);     // Rely on FormatRect being cached from above
			SGRedrawInfo* pRedrawInfo = GetRedrawInfo(nEventType, pEventInfo);
			if (IMustRedraw(pRedrawInfo))
			{
				StartRendering(pRedrawInfo, pMiscInfo);
				pRedrawInfo->Renderer->SaveContext();
				HandleRedraw(pRedrawInfo, pMiscInfo);
				pRedrawInfo->Renderer->RestoreContext();
				StopRendering(pRedrawInfo, pMiscInfo);
			}
			break;
		}

		case SGEVENT_MOUSECLICK:
		{
			// Work out which UI item has been clicked on, if any.
			SGMouseInfo* pMouseInfo = GetMouseInfo(nEventType, pEventInfo);
			if (m_rToggle.ContainsCoord(pMouseInfo->Position))
			{
				TRACEUSER( "matt", _T("m_rToggle Contains the Mouse Coords\n"));
				if (m_nObjects > 0)
				{
					TRACEUSER( "matt", _T("Num Objects > 0\n"));
					// It's a click on the selection toggle gadget for some objects.
					SelectScan::Change eChange;
					if (KeyPress::IsGalleryCtrlPressed())
						eChange = SelectScan::TOGGLE;
					else if (IsAllSelected())
						eChange = SelectScan::DESELECT;
					else
						eChange = SelectScan::SELECT;

					// Change the selection state of the objects within this set item and
					// claim the event.
					OpDescriptor* pDesc =
							OpDescriptor::FindOpDescriptor(OPTOKEN_SELECT_SET);
					ERROR3IF(pDesc == 0, "SGNameItem::HandleEvent: no descriptor");
					pDesc->Invoke(&OpParam((INT32) this, (INT32) eChange));
					return TRUE;
				}
			}
			
			else if (m_nObjects > 0 && m_rProp.ContainsCoord(pMouseInfo->Position))
			{
				TRACEUSER( "matt", _T("m_rToggle DOES NOT contain MouseCoords, m_rProp DOES\n"));

				// It's a click on the property UI.  Pass it on and claim it.
				SGNameProp* pProp = GetProperty();
				if (pProp != 0 && !pProp->HandleMouse(this, pMouseInfo, pMiscInfo))
				{
					InformError();
					return FALSE;
				}

				return TRUE;
			}

			else if (m_rText.ContainsCoord(pMouseInfo->Position) ||
					 (m_nObjects == 0 && m_rProp.ContainsCoord(pMouseInfo->Position)))
			{
				TRACEUSER( "matt", _T("m_rText contains MouseCoords OR m_rProp DOES\n"));

				// It's a click on the text label.  If a name is clicked then (always)
				// start dragging it.  If the click isn't really a drag then
				// DragWasReallyAClick will be called when the drag is cancelled.
				DefaultPreDragHandler(pMouseInfo, pMiscInfo);
				if (GetParent() == NameGallery::Instance()->GetUsedNames())
				{           
					// It's a name (ie. a child of the 'Used Names' group), so create and run
					// a drag operation.
					SGNameDrag* pDragInfo = new SGNameDrag(this, pMouseInfo, pMiscInfo);
					ERRORIF(pDragInfo == 0, _R(IDE_NOMORE_MEMORY), FALSE);
					DragManagerOp::StartDrag(pDragInfo);
				}
				else
					// Not a name so just do the default, but don't close on
					// adjust-double-click.
					DefaultClickHandler(pMouseInfo, pMiscInfo, FALSE);

				TRACEUSER("JustinF",
						  "%s \"%s\" at 0x%p --- %d nodes, %d objects, %d selected\n",
						  (LPCTSTR) String(((SGNameGroup*) GetParent())->GetTypeID()),
						  (LPCTSTR) m_strName, (LPVOID) this,
						  m_nNodes, m_nObjects, m_nSelected);

				// Claim this click.
				return TRUE;
			}

			TRACEUSER( "matt", _T("Click outside bounds of item's UI\n"));
			// The click was outside the bounds of the item's UI gadgets.
			break;
		}
/*
		// Matt - 15/09/2000 - Added for tooltip help over properties
		case SGEVENT_CLAIMPOINT:
		{
			// Work out which UI item is hovered over, if any...
			SGClaimPointInfo *pMouseInfo = GetClaimPointInfo(nEventType, pEventInfo);
			if (!m_rToggle.ContainsCoord(pMouseInfo->Position) && m_nObjects > 0 && m_rProp.ContainsCoord(pMouseInfo->Position))
			{
				//Then we're hovering over the properties section...
				SGNameProp* pProp = GetProperty();

				if (pProp->GetIndex() == 0)
				{
					//Then it's an EXPORT property
					TRACEUSER( "matt", _T("...EXPORTY...\n"));
				}
				else if (pProp->GetIndex() == 1)
				{
					//Then it's a SLICE tickbox
					TRACEUSER( "matt", _T("SLICEY...\n"));
				}
				else if (pProp->GetIndex() == 2)
				{
					//Then it's a STRETCH tickbox
					TRACEUSER( "matt", _T("...STRETCHY...\n"));
				}

//				if (pProp != 0 && !pProp->HandleMouse(this, pMouseInfo, pMiscInfo))
//				{
//					InformError();
//					return FALSE;
//				}
				return true;
			}
			break;
		}
*/
		default:
			// Let the base class handle any events we don't know about.
			// This includes things like hit testing (CLAIMPOINT) etc
			return SGDisplayItem::HandleEvent(nEventType, pEventInfo, pMiscInfo);
	}

	// Default return value: we do not claim this event, so it will be passed on to others.
	return FALSE;
}

// is this name group item a backbar?
BOOL SGNameItem::IsABackBar()
{
	String_256 SubName = m_strName;
	*(SubName + 7) = 0;
	return (SubName.CompareTo("BackBar") == 0);
}




////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////




/********************************************************************************************
>	SGNameGroup::SGNameGroup(UINT32 idType, Document* pDoc = 0)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/6/99
	Inputs:		pDoc	---		the document associated with this group (by default, 
								always whatever is the currently 'selected' document)
	Purpose:	Creates an abstract Name Gallery group.
	Notes:		Uses the lowest bit of SGDisplayFlags.Reserved to track whether a group
				has been automatically folded because it has no items (and hence should
				automatically unfold if it later has new items to show).
********************************************************************************************/

SGNameGroup::SGNameGroup(UINT32 idType, UINT32 idTitle, Document* pDoc)
  :	SGDisplayGroup(NameGallery::Instance(), pDoc),
	m_idType(idType),
	m_idTitle(idTitle)
{
	Flags.CanSelect = TRUE;
	Flags.Reserved &= ~1;
}



/********************************************************************************************
>	virtual UINT32 SGNameGroup::GetTypeID() const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/6/99
	Returns:	The ID of a string that describes the type of items in this group.
********************************************************************************************/

UINT32 SGNameGroup::GetTypeID() const
{
	return m_idType;
}



/********************************************************************************************
>	SGNameItem* SGNameGroup::FindItem(const StringBase& strName) const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/6/99
	Returns:	The ID of a string that describes the type of items in this group.
********************************************************************************************/

SGNameItem* SGNameGroup::FindItem(const StringBase& strName) const
{
	// Iterate over all the items looking a matching name.
	for (SGNameItem* pItem = (SGNameItem*) GetChild();
		 pItem != 0;
		 pItem = (SGNameItem*) pItem->GetNext())
	{
		if (pItem->GetNameTextPtr()->CompareTo(strName) == 0)
			return pItem;
	}

	// Can't find an item with that name.
	return 0;
}



/********************************************************************************************
>	SGNameItem* SGNameGroup::RegisterMember(Node* pNode, const StringBase& strName)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/6/99
	Inputs:		pNode		---		the node to register as a member
				strName		---		the name of the new item to make if one doesn't
									exist already
	Purpose:	Creates and inserts a SGNameItem with the given name if one doesn't
				exist already.  Includes the given node in the set's bounds etc.
	Errors:		Out of memory.
********************************************************************************************/

SGNameItem* SGNameGroup::RegisterMember(Node* pNode, const StringBase& strName)
{
	// Only make a new item if there isn't one already.
	for (SGNameItem* pItem = (SGNameItem*) GetChild();
		 pItem != 0 && !pItem->IsEqual(strName);
		 pItem = (SGNameItem*) pItem->GetNext())
			/* empty */ ;

	if (pItem == 0)
	{
		// If we previously had no children and folded because of that then
		// automatically unfold.
		if (GetChild() == 0 && (Flags.Reserved & 1))
		{
			Flags.Reserved &= ~1;
			Flags.Folded = FALSE;
		}
		
		// We must create a new set item for this 'attribute value'.
		pItem = MakeItem(strName);
		ERRORIF(pItem == 0, _R(IDE_NOMORE_MEMORY), 0);
		
		// Insert in alphabetical order.
		SGSortKey sk[2] = { { SGSORTKEY_BYNAME } };
		AddItem(pItem, sk);
	}
/*
	TRACEUSER( "JustinF", _T("Registering %s at 0x%p as a member of %s\n"),
				(LPCTSTR) pNode->GetRuntimeClass()->m_lpszClassName,
				(LPVOID) pNode, (LPCTSTR) strName);
*/
	// 'Register' the node with the set item which will note its selection
	// status, bounds etc.
	pItem->Include(pNode);
	return pItem;
}



/********************************************************************************************
>	virtual SGNameItem* SGNameGroup::MakeItem(const StringBase& strName) const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/6/99
	Purpose:	Seize control of the group's title from the base class.
********************************************************************************************/

SGNameItem* SGNameGroup::MakeItem(const StringBase& strName) const
{
	return new SGNameItem(strName);
}



/********************************************************************************************
>	virtual void SGNameGroup::ReadGroupTitle()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/6/99
	Purpose:	Seize control of the group's title from the base class.
********************************************************************************************/

void SGNameGroup::ReadGroupTitle()
{
	TitleText.Load(m_idTitle);
}



/********************************************************************************************
>	virtual BOOL SGUsedNames::IsMember(Node* pNode, const StringBase& strName) const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/6/99
	Inputs:		pNode		---		the node to test for membership
				strName		---		the set value (name) to test for equality
	Returns:	TRUE if the given node is a member of a set within this group.
	SeeAlso:	SGNameGroup::RegisterMember; SGNameItem::IsMember;
				CreateDisplayScan::CreateItems::Do
********************************************************************************************/

BOOL SGUsedNames::IsMember(Node* pNode, const StringBase& strName) const
{
	TemplateAttribute* pAttr;
	for (pAttr = (TemplateAttribute*) pNode->FindFirstAttr(Node::IsAnObjectName);
		 pAttr != 0;
		 pAttr = (TemplateAttribute*) pAttr->FindNextAttr(Node::IsAnObjectName))
			if (strName == pAttr->GetParam())
				return TRUE;

	return FALSE;
}



/********************************************************************************************
>	virtual BOOL SGUsedNames::CreateItems(Node* pNode)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/6/99
	Inputs:		pNode			---		the node to test/register for membership
	Purpose:	Create 'Used Names' set items for the node, as appropriate.
	Returns:	FALSE for an error, eg. out of memory.
	SeeAlso:	SGNameGroup::RegisterMember; SGUsedNames::IsMember;
				CreateDisplayScan::CreateItems::Do
********************************************************************************************/

BOOL SGUsedNames::CreateItems(Node* pNode)
{
	TemplateAttribute* pAttr;
	for (pAttr = (TemplateAttribute*) pNode->FindFirstAttr(Node::IsAnObjectName);
		 pAttr != 0;
		 pAttr = (TemplateAttribute*) pAttr->FindNextAttr(Node::IsAnObjectName))
			if (RegisterMember(pNode, pAttr->GetParam()) == 0)
				return FALSE;

	return TRUE;
}



/********************************************************************************************
>	virtual BOOL SGUsedBitmaps::IsMember(Node* pNode, const StringBase& strName) const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/6/99
	Inputs:		pNode		---		the node to test for membership
				strName		---		the set value (name) to test for equality
	Returns:	TRUE if the given node is a member of the given set within this group.
	SeeAlso:	SGNameGroup::RegisterMember; SGNameItem::IsMember;
				CreateDisplayScan::CreateItems::Do

  NB. the value returned as pBmp can sometimes be set to 1 rather than NULL.
		so for the meantime test for this too
********************************************************************************************/
BOOL SGUsedBitmaps::IsMember(Node* pNode, const StringBase& strName) const
{
	// Is it a bitmap with that value?
	KernelBitmap* pBmp;
	String_256 str;
	if (pNode->IsABitmap())
	{
		pBmp = ((NodeBitmap*) pNode)->BitmapRef.GetBitmap();
		if (pBmp != 0 && pBmp != (class KernelBitmap *)1 && pBmp->GetName() == strName) return TRUE;
	}

	// Test against bitmap transparency fills.
	NodeAttribute* pAttr = pNode->FindFirstAttr(Node::IsABitmapTranspFill);
	if (pAttr != 0)
	{
		pBmp = ((BitmapTranspFillAttribute*) pAttr->GetAttributeValue())
							->BitmapRef.GetBitmap();
		if (pBmp != 0 && pBmp != (class KernelBitmap *)1 && pBmp->GetName() == strName) return TRUE;
	}

	// Test against bitmap colour fills.
	pAttr = pNode->FindFirstAttr(Node::IsABitmapColourFill);
	if (pAttr != 0)
	{
		pBmp = ((BitmapFillAttribute*) pAttr->GetAttributeValue())
							->BitmapRef.GetBitmap();
		if (pBmp != 0 && pBmp != (class KernelBitmap *)1 && pBmp->GetName() == strName) return TRUE;
	}

	return FALSE;
}



/********************************************************************************************
>	virtual BOOL SGUsedBitmaps::CreateItems(Node* pNode)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/6/99
	Inputs:		pNode			---		the node to test/register for membership
	Purpose:	Create 'Used Bitmaps' set items for the node, as appropriate.
	Returns:	FALSE for an error, eg. out of memory.
	SeeAlso:	SGNameGroup::RegisterMember; SGUsedBitmaps::IsMember;
				CreateDisplayScan::CreateItems::Do
********************************************************************************************/

BOOL SGUsedBitmaps::CreateItems(Node* pNode)
{
	// Is the object itself a kind of bitmap?
	KernelBitmap* pBmp;
	if (pNode->IsABitmap())
	{
		pBmp = ((NodeBitmap*) pNode)->BitmapRef.GetBitmap();
		if (pBmp != 0 && RegisterMember(pNode, pBmp->GetName()) == 0)
			return FALSE;
		else if (pBmp != 0)
			return (TRUE);
	}

	// Does it have a bitmap transparency fill attribute?
	NodeAttribute* pAttr = pNode->FindFirstAttr(Node::IsABitmapTranspFill);
	if (pAttr != 0)
	{
		pBmp = ((BitmapTranspFillAttribute*) pAttr->GetAttributeValue())
						->BitmapRef.GetBitmap();
		
		if (pBmp != 0 && RegisterMember(pNode, pBmp->GetName()) == 0)
			return FALSE;
		else if (pBmp != 0)
			return (TRUE);
	}

	// Does a bitmap colour or transparency fill attribute apply to it?
	pAttr = pNode->FindFirstAttr(Node::IsABitmapFill);
	if (pAttr != 0 && !pAttr->IsAFractalFill()) // fractals and plasmas don't have a bitmap!!! sjk
	{
		pBmp = ((BitmapFillAttribute*) pAttr->GetAttributeValue())
						->BitmapRef.GetBitmap();
		if (pBmp != 0 && RegisterMember(pNode, pBmp->GetName()) == 0)
			return FALSE;
		else if (pBmp != 0)
			return (TRUE);
	}

	return TRUE;
}



/********************************************************************************************
>	virtual BOOL SGUsedFonts::IsMember(Node* pNode, const StringBase& strName) const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/6/99
	Inputs:		pNode			---		the node to test for membership
				strName			---		the set value (name) to test for equality
	Returns:	TRUE if the given node is a member of the given set within this group.
	SeeAlso:	SGNameGroup::RegisterMember; SGNameItem::IsMember;
				CreateDisplayScan::CreateItems::Do
********************************************************************************************/

BOOL SGUsedFonts::IsMember(Node* pNode, const StringBase& strName) const
{
	// Only consider genuine text objects.
	if (!pNode->IsATextChar()) return FALSE;
//	if (!pNode->IsABaseTextClass()) return FALSE;

	// Does a typeface attribute apply?
	AttrTxtFontTypeface* pAttr =
				(AttrTxtFontTypeface*) pNode->FindFirstAttr(Node::IsATypeface);
	if (pAttr == 0) return FALSE;

	// The same value?
	String_64 strFont;
	FONTMANAGER->GetFontName(pAttr->Value.HTypeface, strFont);
	return strFont == strName;
}



/********************************************************************************************
>	virtual BOOL SGUsedFonts::CreateItems(Node* pNode)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/6/99
	Inputs:		pNode			---		the node to test/register for membership
	Purpose:	Create 'Used Fonts' set items for the node, as appropriate.
	Returns:	FALSE for an error, eg. out of memory.
	SeeAlso:	SGNameGroup::RegisterMember; SGUsedFonts::IsMember;
				CreateDisplayScan::CreateItems::Do
********************************************************************************************/

BOOL SGUsedFonts::CreateItems(Node* pNode)
{
	// Only consider genuine text objects.
	if (!pNode->IsATextChar()) return TRUE;
//	if (!pNode->IsABaseTextClass()) return TRUE;

	// Does a typeface attribute apply?
	AttrTxtFontTypeface* pAttr =
				(AttrTxtFontTypeface*) pNode->FindFirstAttr(Node::IsATypeface);
	if (pAttr == 0) return TRUE;

	// Yes, make an item for it.
	String_64 strFont;
	FONTMANAGER->GetFontName(pAttr->Value.HTypeface, strFont);
	return RegisterMember(pNode, strFont) != 0;
}



/********************************************************************************************
>	virtual void SGUsedColourItem::CalcUiBounds(SGFormatInfo*, SGMiscInfo*)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/6/99
	Inputs:		(see SGNameGroup::CalcUiBounds)
	Returns:	The width of a colour icon plus a little gap.
	SeeAlso:	SGNameGroup::RegisterMember; SGUsedColours::IsMember;
				CreateDisplayScan::CreateItems::Do
********************************************************************************************/

void SGUsedColourItem::CalcUiBounds(SGFormatInfo* pFormatInfo, SGMiscInfo* pMiscInfo)
{
	SGNameItem::CalcUiBounds(pFormatInfo, pMiscInfo);

	// Work out the new maximum bounds of the patch and the text.
	m_rPatch = m_rText;
	MILLIPOINT w = m_rPatch.Width(), h = m_rPatch.Height();
	m_rPatch.hi.x = m_rPatch.lo.x + (h <= w ? h : w);
	m_rText.lo.x = m_rPatch.hi.x + SG_GapBeforeText / 2;

	// Deflate the patch a little and pixel-align.
	m_rPatch.Inflate(4 * -pMiscInfo->PixelSize, 4 * -pMiscInfo->PixelSize);
	m_rPatch.Translate(-4, 0);
	GridLockRect(pMiscInfo, &m_rPatch);
	GridLockRect(pMiscInfo, &m_rText);
}



/********************************************************************************************
>	void SGUsedColourItem::DrawLabel(SGRedrawInfo* pRedrawInfo, SGMiscInfo* pMiscInfo)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/6/99
	Inputs:		(see SGNameItem::HandleRedraw)
	Purpose:	Renders the colour icon for this display item.
	SeeAlso:	SGNameItem::DrawLabel
********************************************************************************************/

void SGUsedColourItem::DrawLabel(SGRedrawInfo* pRedrawInfo, SGMiscInfo* pMiscInfo)
{
	SGNameItem::DrawLabel(pRedrawInfo, pMiscInfo);

	// Save the context, or we disturb the colour system.  Set the outline.
	pRedrawInfo->Renderer->SaveContext();
	pRedrawInfo->Renderer->SetLineWidth(0);
	pRedrawInfo->Renderer->SetLineColour(COLOUR_BLACK);

	// Make a short-lived reference to the indexed colour.
	DocColour dcFill;
	ERROR3IF(m_picSample == 0, "SGUsedColourItem::DrawLabel: no indexed colour");
	dcFill.MakeRefToIndexedColour(m_picSample);
	pRedrawInfo->Renderer->SetFillColour(dcFill);

	// Render a circle for a spot colour, rectangle for others.
	extern void DrawCircle(RenderRegion*, DocRect&);
	if (m_picSample->IsSpotOrTintOfSpot())
		DrawCircle(pRedrawInfo->Renderer, m_rPatch);
	else
		pRedrawInfo->Renderer->DrawRect(&m_rPatch);

	// Restore context as per SGDisplayColour::HandleRedraw.
	pRedrawInfo->Renderer->RestoreContext();
}



/********************************************************************************************
>	SGUsedColourItem* SGUsedColours::RegisterMember(Node* pNode, const StringBase& strName,
													DocColour* pCol)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/6/99
	Purpose:	Register a 'Used Colours' set item for the node.
	Returns:	0 for out of memory, pointer to the SGUsedColourItem if successful.
	SeeAlso:	SGNameGroup::RegisterMember
********************************************************************************************/

SGUsedColourItem* SGUsedColours::RegisterMember(Node* pNode, const StringBase& strName,
												DocColour* pCol)
{
	SGUsedColourItem* pItem =
				(SGUsedColourItem*) SGNameGroup::RegisterMember(pNode, strName);
	ERROR3IF(pCol == 0, "SGUsedColours::RegisterMember: null input");
	if (pItem != 0) pItem->SetPatchColour(pCol->FindParentIndexedColour());
	return pItem;
}



/********************************************************************************************
>	static const StringBase* SGUsedColours::IsNamedColour(DocColour* pCol)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/6/99
	Inputs:		pCol		---		the colour to test for nomination
	Outputs:	strName		---		name of the colour, if any
	Purpose:	Helper to the SGUsedColours display group.
	Returns:	Null if the colour isn't named, the address of its name if it is.
	SeeAlso:	DocColour::FindParentIndexedColour; IndexedColour::GetName
				SGUsedColours::IsMember; SGUsedColours::CreateItems
********************************************************************************************/

const StringBase* SGUsedColours::IsNamedColour(DocColour* pCol)
{
	ERROR3IF(pCol == 0, "SGUsedColours::IsNamedColour: null input");
	IndexedColour* pIndex = pCol->FindParentIndexedColour();
	return pIndex != 0 && pIndex->IsNamed() ? pIndex->GetName() : 0;
}



/********************************************************************************************
>	virtual BOOL SGUsedColours::IsMember(Node* pNode, const StringBase& strName) const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/6/99
	Inputs:		pNode		---		the node to test for membership
				strName		---		the set value (name) to test for equality
	Returns:	TRUE if the given node is a member of the given set within this group.
	SeeAlso:	SGNameGroup::RegisterMember; SGNameItem::IsMember;
				CreateDisplayScan::CreateItems::Do
********************************************************************************************/

BOOL SGUsedColours::IsMember(Node* pNode, const StringBase& strName) const
{
	const StringBase* pstrCol;
	DocColour* pCol;
	AttrFillGeometry* pAttr;

	for (pAttr = (AttrFillGeometry*) pNode->FindFirstAttr(Node::IsAFillAttr);
		 pAttr != 0;
		 pAttr = (AttrFillGeometry*) pAttr->FindNextAttr(Node::IsAFillAttr))
	{
		pCol = pAttr->GetStartColour();
		if (pCol != 0)
		{
			pstrCol = IsNamedColour(pCol);
			if (pstrCol != 0 && *pstrCol == strName)
				return TRUE;
		}

		pCol = pAttr->GetEndColour();
		if (pCol != 0)
		{
			pstrCol = IsNamedColour(pCol);
			if (pstrCol != 0 && *pstrCol == strName)
				return TRUE;
		}

		pCol = pAttr->GetEndColour2();
		if (pCol != 0)
		{
			pstrCol = IsNamedColour(pCol);
			if (pstrCol != 0 && *pstrCol == strName)
				return TRUE;
		}

		pCol = pAttr->GetEndColour3();
		if (pCol != 0)
		{
			pstrCol = IsNamedColour(pCol);
			if (pstrCol != 0 && *pstrCol == strName)
				return TRUE;
		}
	}

	return FALSE;
}



/********************************************************************************************
>	virtual BOOL SGUsedColours::CreateItems(Node* pNode)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/6/99
	Inputs:		pNode			---		the node to test/register for membership
	Purpose:	Create 'Used Names' set items for the node, as appropriate.
	Returns:	FALSE for an error, eg. out of memory.
	SeeAlso:	SGNameGroup::RegisterMember; SGUsedColours::IsMember;
				CreateDisplayScan::CreateItems::Do
********************************************************************************************/

BOOL SGUsedColours::CreateItems(Node* pNode)
{
	const StringBase* pstrCol;
	DocColour* pCol;
	AttrFillGeometry* pAttr;

	for (pAttr = (AttrFillGeometry*) pNode->FindFirstAttr(Node::IsAFillAttr);
		 pAttr != 0;
		 pAttr = (AttrFillGeometry*) pAttr->FindNextAttr(Node::IsAFillAttr))
	{
		pCol = pAttr->GetStartColour();
		if (pCol != 0)
		{
			pstrCol = IsNamedColour(pCol);
			if (pstrCol != 0 && RegisterMember(pNode, *pstrCol, pCol) == 0)
				return FALSE;
		}

		pCol = pAttr->GetEndColour();
		if (pCol != 0)
		{
			pstrCol = IsNamedColour(pCol);
			if (pstrCol != 0 && RegisterMember(pNode, *pstrCol, pCol) == 0)
				return FALSE;
		}

		pCol = pAttr->GetEndColour2();
		if (pCol != 0)
		{
			pstrCol = IsNamedColour(pCol);
			if (pstrCol != 0 && RegisterMember(pNode, *pstrCol, pCol) == 0)
				return FALSE;
		}

		pCol = pAttr->GetEndColour3();
		if (pCol != 0)
		{
			pstrCol = IsNamedColour(pCol);
			if (pstrCol != 0 && RegisterMember(pNode, *pstrCol, pCol) == 0)
				return FALSE;
		}
	}

	return TRUE;
}


