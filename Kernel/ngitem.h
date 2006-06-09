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
/*
	$Header: /Camelot/kernel/ngitem.h 10    15/10/04 9:37 Gerry $
	Attribute gallery display items ans groups.
*/

#ifndef NGITEM_H
#define NGITEM_H

//#include "sgtree.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "justin3.h"

class Node;
class NodeRenderableInk;
class DocRect;
class IndexedColour;
class SGNameItem;
class SGNameProp;
class NodeSetProperty;


/***********************************************************************************************
>	class CCAPI SGNameGroup : public SGDisplayGroup

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/6/99
	SeeAlso:	SGNameItem; NameGallery; SGDisplayItem
***********************************************************************************************/

class CCAPI SGNameGroup : public SGDisplayGroup
{
	CC_DECLARE_DYNAMIC(SGNameGroup);
public:
	// Creation & destruction.
	SGNameGroup(UINT32 idType, UINT32 idTitle, Document* pDoc = 0);

	// Returns a string ID that describes the type of items in this group.
	UINT32 GetTypeID() const;

	// Returns TRUE if the given node is a member of the named set within this group.
	virtual BOOL IsMember(Node* pNode, const StringBase& strName) const = 0;

	// Register the node and all the values (names) of the sets it is a member of
	// (ie. create display items for them, if necessary).  Returns FALSE for error.
	virtual BOOL CreateItems(Node* pNode) = 0;

	// Returns the address of the item with the given name, or null if there isn't one.
	SGNameItem* FindItem(const StringBase& strName) const;

	// If necessary, creates a new display item with the given name for the set that
	// the node is a member of.  Includes the bounds of the node in the bounds of the
	// set etc.
	SGNameItem* RegisterMember(Node* pNode, const StringBase& strName);

protected:

	// Return a newly-created display item.
	virtual SGNameItem* MakeItem(const StringBase& strName) const;

	//  Seize back control of the group's title.
	virtual void ReadGroupTitle();

private:
	// Data.
	const UINT32 m_idType;		// eg. 'name', 'font', 'bitmap' ...
	const UINT32 m_idTitle;		// eg. 'Used names', 'Used fonts', 'Used bitmaps' ...
};



/***********************************************************************************************
>	class CCAPI SGNameItem : public SGDisplayItem

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/6/99
	Purpose:	Represents an item in the Name gallery's display.
	SeeAlso:	NameGallery; SuperGallery; SGDisplayItem
***********************************************************************************************/

class CCAPI SGNameItem : public SGDisplayItem
{
public:
	// Creation & destruction.
	SGNameItem(const StringBase& strName);

	// return a cached ptr to this sets property node
	NodeSetProperty* GetPropertyNode();
	void ResetCachedPropertyNode()						{ m_pCachedPropertyNode = 0; }

	// Set contents and selection status etc
	BOOL IsABackBar();
	BOOL IsEqual(const StringBase& strOther) const;
	BOOL IsEmpty() const;
	BOOL IsAllSelected() const;
	BOOL IsNoneSelected() const;
	BOOL IsExclusivelySelected() const;
	BOOL IsMember(Node* pNode) const
			{ return ((SGNameGroup*) GetParent())->IsMember(pNode, m_strName); }

	// Low level contents status.
	INT32 GetNodeCount() const;
	INT32 GetObjectCount() const;
	INT32 GetSelectedCount() const;

	// Bounding boxes.
	const DocRect& GetSetBounds() const;
	const DocRect& GetOldSetBounds() const;
	const DocRect& GetSetSelectedBounds() const;
	BOOL HasChangedBounds() const
			{ return m_rSetBounds != m_rOldSetBounds; }

	// sjk added this nasty backdoor for the meantime
	void SetSetBounds(const DocRect& r)						{ m_rSetBounds = r; }

	// Property management.
	SGNameProp* GetProperty(INT32 nIndex = -1) const;

	// Characteristics of this display item.
	virtual void GetNameText(String_256* pResult);
	virtual const String_256* GetNameTextPtr() const	{ return &m_strName; };

	// Refresh and calculation of bounds and selection state.
	void Reset(BOOL fPropagateChanges = TRUE);
	void Include(Node* pNode);

	// Modify sorting comparisons so they take account of appended numbers.
	virtual INT32 CompareTo(SGDisplayNode* pOther, INT32 nKey);

	// Called when a drag operation turns out to be a mundane button click.
	virtual void DragWasReallyAClick(SGMouseInfo* pMouseInfo, SGMiscInfo* pMiscInfo);
	
protected:
	// Specialised UI.
	virtual void CalcUiBounds(SGFormatInfo* pFormatInfo, SGMiscInfo* pMiscInfo);
	virtual void DrawLabel(SGRedrawInfo* pRedrawInfo, SGMiscInfo* pMiscInfo);

	// UI state handlers.
	virtual BOOL GetBubbleHelp(DocCoord* pMousePos, String_256* pResult);
	virtual BOOL GetStatusLineHelp(DocCoord* pMousePos, String_256* pResult);
	
	// Gallery system event & redraw handlers.
	virtual void CalculateMyRect(SGFormatInfo* pFormatInfo, SGMiscInfo* pMiscInfo);
	virtual void HandleRedraw(SGRedrawInfo* pRedrawInfo, SGMiscInfo* pMiscInfo);
	virtual	BOOL HandleEvent(SGEventType EventType, void* pEventInfo,
							 SGMiscInfo* pMiscInfo);

	// Data for UI specialisation.
	String_256	m_strName;					// displayed name of this
	DocRect		m_rToggle;					// UI bounds of the selection toggle gadget
	DocRect		m_rText;					// UI bounds of the item's name label
	DocRect		m_rProp;					// UI bounds of currently displayed property

	NodeSetProperty* m_pCachedPropertyNode; // a cached ptr to this sets property node
											// this is NULLed in Reset() and filled on demand
private:
	// Data for set creation & properties.
	DocRect		m_rSetBounds;				// accumulated bounds of objects in the set
	DocRect		m_rOldSetBounds;			// value of m_rSetBounds before last operation
	DocRect		m_rSelectBounds;			// accumulated bounds of selected objects in set
	INT32			m_nNodes;					// count of nodes in the set of this
	INT32			m_nObjects;					// count of objects in the set of this
	INT32			m_nSelected;				// count of selected objects in the set

	CC_DECLARE_DYNAMIC(SGNameItem);

public:
	// cached items of data stored in the name item
	// and used in the extending code
	BOOL	 m_SetIsAffectedTrigger;	// used in FastApplyStretchScan to store sets that have changed size
	BOOL	 m_IsATrigger;				// used in FastApplyStretchScan to store sets that appear in a trigger list
	BOOL	 m_IsPartOfThisStretch;		// used in FastApplyStretchScan to store sets that have been extended
	INT32		 m_BarNumber;				// set in FastUpdateNamedSetSizes and should be up-to-date if the bar data doesn't change in your op
	DocCoord m_Translation;				// set in the ShuffleBar function

};



/***********************************************************************************************
>	class CCAPI SGUsedNames : public SGNameGroup
	class CCAPI SGUsedFonts : public SGNameGroup
	class CCAPI SGUsedBitmaps : public SGNameGroup
	class CCAPI SGUsedColours : public SGNameGroup
	class CCAPI SGUsedColourItem : public SGNameItem

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/6/99
	Purpose:	NameGallery groups and items to manage the various types of attribute sets.
	SeeAlso:	NameGallery; SGNameGroup; SGNameItem
***********************************************************************************************/

class CCAPI SGUsedNames : public SGNameGroup
{
public:
	SGUsedNames()
	  :	SGNameGroup(_R(IDS_NAMEGAL_NAMES_TYPE), _R(IDS_NAMEGAL_NAMES_TITLE))
			{ /* empty */ }

	virtual BOOL IsMember(Node*, const StringBase&) const;
	virtual BOOL CreateItems(Node*);
};



class CCAPI SGUsedFonts : public SGNameGroup
{
public:
	SGUsedFonts()
	  :	SGNameGroup(_R(IDS_NAMEGAL_TYPEFACES_TYPE), _R(IDS_NAMEGAL_TYPEFACES_TITLE))
			{ /* empty */ }

	virtual BOOL IsMember(Node*, const StringBase&) const;
	virtual BOOL CreateItems(Node*);
};



class CCAPI SGUsedBitmaps : public SGNameGroup
{
public:
	SGUsedBitmaps()
	  :	SGNameGroup(_R(IDS_NAMEGAL_BITMAPS_TYPE), _R(IDS_NAMEGAL_BITMAPS_TITLE))
			{ /* empty */ }

	virtual BOOL IsMember(Node*, const StringBase&) const;
	virtual BOOL CreateItems(Node*);
};



class CCAPI SGUsedColourItem : public SGNameItem
{
public:
	// Creation & destruction.
	SGUsedColourItem(const StringBase& strName)
	  :	SGNameItem(strName), m_picSample(0)
			{ /* empty */ }

	// Set the colour associated with the given item.
	void SetPatchColour(IndexedColour* pCol)
			{ m_picSample = pCol; }

protected:
	// Override default rendering to draw a colour patch beside the label.
	virtual void CalcUiBounds(SGFormatInfo*, SGMiscInfo*);
	virtual void DrawLabel(SGRedrawInfo*, SGMiscInfo*);

	DocRect			m_rPatch;					// bounds of the coloured patch by the label
	IndexedColour*  m_picSample;				// reference to an named indexed colour
};



class CCAPI SGUsedColours : public SGNameGroup
{
public:
	SGUsedColours()
	  :	SGNameGroup(_R(IDS_NAMEGAL_COLOURS_TYPE), _R(IDS_NAMEGAL_COLOURS_TITLE))
			{ /* empty */ }

	virtual BOOL IsMember(Node*, const StringBase&) const;
	virtual BOOL CreateItems(Node*);

protected:
	// SGUsedColours groups make special SGUsedColourItems which can render a
	// DocColour patch by their label.
	virtual SGNameItem* MakeItem(const StringBase& strName) const
			{ return new SGUsedColourItem(strName); }

	static const StringBase* IsNamedColour(DocColour* pCol);
	SGUsedColourItem* RegisterMember(Node*, const StringBase&, DocColour* pCol);
};



/***********************************************************************************************
>   const DocRect& SGNameItem::GetSetBounds() const

	Author:     Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:    27/6/99
	Returns:    The accumulated bounds of the objects with the set.
***********************************************************************************************/

inline const DocRect& SGNameItem::GetSetBounds() const
{
	return m_rSetBounds;
}



/***********************************************************************************************
>   const DocRect& SGNameItem::GetOldSetBounds() const

	Author:     Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:    27/6/99
	Returns:    The accumulated bounds of the objects with the set prior to the last operation.
***********************************************************************************************/

inline const DocRect& SGNameItem::GetOldSetBounds() const
{
	return m_rOldSetBounds;
}



/***********************************************************************************************
>   const DocRect& SGNameItem::GetSetSelectedBounds() const

	Author:     Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:    27/6/99
	Returns:    The accumulated bounds of the objects with the set.
***********************************************************************************************/

inline const DocRect& SGNameItem::GetSetSelectedBounds() const
{
	return m_rSelectBounds;
}



/***********************************************************************************************
>   INT32 SGNameItem::GetNodeCount() const

	Author:     Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:    27/6/99
	Returns:    The number of selected objects included in this item's set.
***********************************************************************************************/

inline INT32 SGNameItem::GetNodeCount() const
{
	return m_nNodes;
}



/***********************************************************************************************
>   INT32 SGNameItem::GetObjectCount() const

	Author:     Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:    27/6/99
	Returns:    The total number of objects included in this item's set.
***********************************************************************************************/

inline INT32 SGNameItem::GetObjectCount() const
{
	return m_nObjects;
}



/***********************************************************************************************
>   INT32 SGNameItem::GetSelectedCount() const

	Author:     Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:    27/6/99
	Returns:    The number of selected objects included in this item's set.
***********************************************************************************************/

inline INT32 SGNameItem::GetSelectedCount() const
{
	return m_nSelected;
}



/***********************************************************************************************
>   BOOL SGNameItem::IsEmpty() const

	Author:     Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:    27/6/99
	Returns:    TRUE if the item has no members (ie. total counter is zero).
***********************************************************************************************/

inline BOOL SGNameItem::IsEmpty() const
{
	return m_nObjects == 0;
}



/***********************************************************************************************
>   BOOL SGNameItem::IsAllSelected() const

	Author:     Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:    27/6/99
	Returns:    TRUE if all the items in the set are selected.
***********************************************************************************************/

inline BOOL SGNameItem::IsAllSelected() const
{
	return m_nObjects > 0 && m_nSelected == m_nObjects;
}



/***********************************************************************************************
>   BOOL SGNameItem::IsNoneSelected() const

	Author:     Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:    27/6/99
	Returns:    TRUE if no object within this item's set is selected.
***********************************************************************************************/

inline BOOL SGNameItem::IsNoneSelected() const
{
	return m_nSelected == 0;
}



/***********************************************************************************************
>   BOOL SGNameItem::IsExclusivelySelected() const

	Author:     Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:    27/6/99
	Returns:    TRUE if this item's set and the current selection are the same.
***********************************************************************************************/

inline BOOL SGNameItem::IsExclusivelySelected() const
{
	return IsAllSelected() && m_nObjects == (INT32) GetApplication()->FindSelection()->Count();
}



/***********************************************************************************************
>   BOOL SGNameItem::IsEqual(const StringBase& strOther) const

	Author:     Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:    27/6/99
	Inputs:     strOther    ---     'attribute value' to compare this item to
	Returns:    TRUE if this item has the same 'attribute value' as the input.
***********************************************************************************************/

inline BOOL SGNameItem::IsEqual(const StringBase& strOther) const
{
	return m_strName == strOther;
}

#endif	/* !NGITEM_H */
