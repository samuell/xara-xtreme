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
// helper functions that relate to things slice like
// such as image slicing, bar creation, bar duplication and text synchronising

#include "camtypes.h"

#include "layer.h"		// knowing about layers
#include "spread.h"

// for the use of wix temple attribs
#include "cxfrech.h"
#include "userattr.h"
#include "tmpltatr.h"

// named set stuff
//#include "ngcore.h"
//#include "ngitem.h"

//#include "simon.h" // for the _R(IDS_ROLLOVER_DEFAULT) etc
//#include "sliceres.h"		// more resources

#include "slicehelper.h"
#include "ngprop.h"
#include "ngsentry.h"

#include <algorithm> // for "find" of lists

#include "nodetxts.h"
#include "nodetxtl.h"
//#include "nodetext.h"
#include "nodecntr.h"

//#include "layergal.h" // for the vis layer action

// need to know about shadows and bevels since they size funny for the GetNodeBounding()
#include "nodeshad.h"
#include "nbevcont.h"
#include "ncntrcnt.h"

#include "lineattr.h" // for the line width inclusion part of the GetNodeBounding()

// global that is set when a bar property tag is imported to say how many bars there were beforehand
// used by SliceHelper::MeshImportedLayersWithExistingButtonBars() and is defined in rechdoc.cpp
extern INT32 g_NoOfBarsBeforeImport;

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

DECLARE_SOURCE("$Revision$");

// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW

#define MAX_IMPORTED_BARS	255

List * SliceHelper::m_pSelNodeList = 0;

/********************************************************************************************

>	static Layer * SliceHelper::FindLayerCalled(const StringBase & LayerName)

	Author:		Simon_Knight (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/9/99
	Params:		LayerName	-	The name of the layer to find
	Returns:	returns the layer node for the Layer name passed in.
				Returns NULL if there is no layer of that name
	Purpose:	-
	Errors:		-

********************************************************************************************/
Layer * SliceHelper::FindLayerCalled(const StringBase & LayerName)
{
	// find a spread?
	Spread* pSpread = Document::GetSelectedSpread();
	if (pSpread == NULL)
		return FALSE;

	// scan for the layer to delete
	Layer * pLayer = pSpread->FindFirstLayer();

	while (pLayer)
	{
		if (!pLayer->IsNodeHidden() && pLayer->GetLayerID().CompareTo(LayerName) == 0)
			return pLayer;

		pLayer = pLayer->FindNextLayer();
	}

	return NULL;
}

/********************************************************************************************

>	static INT32 SliceHelper::CountButtonsInBar(const StringBase & BarName)

	Author:		Simon_Knight (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/9/99
	Params:		BarName	-	The name of the bar we refer to
	Returns:	The number of buttons in the bar.
				Zero if it doesn't think the bar exists.
	Purpose:	-
	Errors:		-

********************************************************************************************/
INT32 SliceHelper::CountButtonsInBar(const StringBase & BarName)
{
	String_256 DefaultLayerName;
	DefaultLayerName.Load(_R(IDS_ROLLOVER_DEFAULT));

	// count the buttons on the default layer
	// every button bar requires a member to be on the default layer
	// so there is no point in counting any other layer
	Layer * pDef = FindLayerCalled(DefaultLayerName);

	INT32 NumberOfButtons = 0;

	if (pDef)
	{
		// store of example attribs of each button in this bar
		// but we don't need this data so we can throw it away afterwards
		TemplateAttribute ** ppFoundButton[MAX_BUTTONS_IN_A_BAR];

		// scan down from the default layer counting the different button names
		// that all have the same bar name
		CountButtonsInBarScan(pDef, (TemplateAttribute **) ppFoundButton, &NumberOfButtons, BarName);
	}

	return NumberOfButtons;
}


/********************************************************************************************

>	static void SliceHelper::CountButtonsInBarScan(Node * pNode, TemplateAttribute ** ppFoundButton,
								 INT32 * pNumberOfButtons, const StringBase &BarName)

	Author:		Simon_Knight (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/9/99
	Params:		pNode			-	The node to scan from.
									Looks at everything down from here.
				ppFoundButton	-	Pointer to array of Template Attribute nodes.
									Each one holds the name of the bar 
									and the name of the button it is.
				pNumberOfButtons-	The number of buttons found so far
				BarName			-	The bar we are looking for
	Returns:	-
	Purpose:	Finds the number of buttons in the bar and the names of each button,
				by filling in the ppFoundButton array and pNumberOfButtons.
	Errors:		Limitation of 20 buttons in a bar, which is all the program will let you have.
				Caution this function is RECURSIVE.

********************************************************************************************/
void SliceHelper::CountButtonsInBarScan(Node * pNode, TemplateAttribute ** ppFoundButton, INT32 * pNumberOfButtons, const StringBase & BarName)
{
	// is this a node we are looking for?
	if (pNode->IsAnAttribute()) // is an attrib
	{
		if (IS_A(pNode,TemplateAttribute)) // is a wix attrib
			{
				// does it have the correct bar name hidden in the question?
				if (BarName.CompareTo(((TemplateAttribute *)pNode)->GetQuestion()) == 0)
				{
					BOOL ok = TRUE;
					// get this buttons name
					String_256 ThisButton = ((TemplateAttribute *)pNode)->GetParam();

					for (INT32 i = 0; ok && i < *pNumberOfButtons; i++)
					{
						// has this button name been seen before?
						if (ThisButton.CompareTo(ppFoundButton[i]->GetParam()) == 0)
							ok = FALSE;
					}

					// found a new button name
					if (ok && *pNumberOfButtons < MAX_BUTTONS_IN_A_BAR) 
					{
						// add this button to the array
						ppFoundButton[*pNumberOfButtons] = (TemplateAttribute *)pNode;
						TRACE( _T("Found new button called "));
						TRACE(ppFoundButton[*pNumberOfButtons]->GetParam());
						TRACE( _T("\n"));
						// increase the number of buttons
						*pNumberOfButtons = *pNumberOfButtons + 1;
					}

				}
			}
	}
	else // look at this nodes children
	{
		Node * pChildNode = pNode->FindFirstChild();

		while (pChildNode)
		{
			// ***recursive call***
			if (!pChildNode->IsNodeHidden())
				CountButtonsInBarScan(pChildNode, ppFoundButton, pNumberOfButtons, BarName);

			pChildNode = pChildNode->FindNext();
		}
	}
}


/********************************************************************************************

>	static void SliceHelper::BuildListOfNodesInBar(List * pList, Node * pNode,
													 const StringBase & BarName)

	Author:		Simon_Knight (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/9/99
	Params:		pList			-	List that is filled in of pointers to all the nodes
				pNode			-	The node to scan from.
									Looks at everything down from here.
				BarName			-	The bar we are looking for
	Returns:	-
	Purpose:	Builds a list (or extends an existing list) of ALL the wix attribute nodes
				that have this bar name hidden in the question.
	Errors:		Caution is RECURSIVE

********************************************************************************************/
void SliceHelper::BuildListOfNodesInBar(List * pList, Node * pNode, const StringBase & BarName)
{
	if (pNode->IsAnAttribute()) // looking for an attrib
	{
		if (IS_A(pNode,TemplateAttribute)) // that is a wix attrib
			{
				// that has the bar name in the question
				// the same as the one we are looking for
				if (BarName.CompareTo(((TemplateAttribute *)pNode)->GetQuestion()) == 0)
				{
					//add it to the list then
					NodeListItem * pItem = new NodeListItem(pNode);
					pList->AddTail(pItem);
				}
			}
	}
	else // find anything else interesting?
	{
		Node * pChildNode = pNode->FindFirstChild();

		while (pChildNode)
		{
			// ***recursive call***
			if (!pChildNode->IsNodeHidden())
				BuildListOfNodesInBar(pList, pChildNode, BarName);
			pChildNode = pChildNode->FindNext();
		}
	}
}

/********************************************************************************************

>	static SGNameItem* SliceHelper::LookupNameGalleryItem(const StringBase& strName)

	Author:		Simon_Knight (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/9/99
	Params:		strName	-	Name of the set from the name gallery
							that we are looking for
	Returns:	a ptr to the name gallery item if it exists
				or NULL if it doesn't exist.
	Purpose:	Used to find out if a button name has been used before.
				Or find the dimensions of a button.
	Errors:		-

********************************************************************************************/
SGNameItem* SliceHelper::LookupNameGalleryItem(const StringBase & strName)
{
	PORTNOTETRACE("dialog","SliceHelper::LookupNameGalleryItem - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	NameGallery * pNameGallery = NameGallery::Instance();
	SGUsedNames* pNames = pNameGallery ? pNameGallery->GetUsedNames() : NULL;
	SGNameItem* pNameGalleryItem = pNames ? (SGNameItem*) pNames->GetChild() : NULL;

	String_256 str;

	// check all the name gallery items
	while (pNameGalleryItem)
	{
		pNameGalleryItem->GetNameText(&str);
		// if the name matches its your man
		if (strName.CompareTo(str) == 0)
			return pNameGalleryItem;

		// no then try the next one?
		pNameGalleryItem = (SGNameItem *) pNameGalleryItem->GetNext();
	}
#endif
	return NULL;
}

/********************************************************************************************

>	static void SliceHelper::GetNextFreeButtonName(INT32 &butno, StringBase * pStr)

	Author:		Simon_Knight (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/10/99
	Params:		butno	-	The number of the button to start looking from
							use zero to look for button1 first etc..
							Gets filled in to the number of the free button found.
				pStr	-	This is set to the string of the button name if you
							bother to pass it a buffer in.
	Returns:	Doesn't bother
	Purpose:	Used to find the next button name that can be reused or created,
				that isn't going to interfere with what we have.
	Errors:		-

********************************************************************************************/
void SliceHelper::GetNextFreeButtonName(INT32 &butno, StringBase * pStr)
{
	PORTNOTETRACE("dialog","SliceHelper::GetNextFreeButtonName - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	String_256 TempButtonName;
	SGNameItem* pNGItem = NULL;
	SGNameItem* pNGItemExtender = NULL;

	// Matt 20/12/2000
	// We can't just assume that because "Button1" doesn't exist that "Button1 Extender" doesn't
	// because we might have renamed either one !!!
	String_256 Postfix; // holds the "Extender" postfix
	Postfix.Load(_R(IDS_EXTENDER_POSTFIX));
	String_256 TempButtonExtender;

	do
	{
		butno++;
		TempButtonName.MakeMsg(_R(IDS_BUTTONNAME), butno);
		pNGItem = SliceHelper::LookupNameGalleryItem(TempButtonName);

		// Construct the appropriate default extender name for this button
		TempButtonExtender = TempButtonName;
		TempButtonExtender += Postfix;

		pNGItemExtender = SliceHelper::LookupNameGalleryItem(TempButtonExtender);
	} while ((pNGItem && !pNGItem->IsEmpty()) || (pNGItemExtender && !pNGItemExtender->IsEmpty()));

	if (pStr)
		*pStr = TempButtonName;
#endif
}

/********************************************************************************************

>	static Node * SliceHelper::FindNextOfClass(	Node *pNode,
												Node * pLidNode, 
												const class CCRuntimeClass * pClass,
												BOOL CheckThis = FALSE)

	Author:		Simon_Knight (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/9/99
	Params:		pNode	-	The node to continue the search from
				pLidNode-	Marks the end of the search, doesn't search beyond
							this node
				pClass	-	The class we are looking for e.g. CC_RUNTIME_CLASS(TextLine)
				CheckThis-	Include or exclude the pNode in the search.
							Useful if it has just returned this node and you then
							want to find the next, not be presented with the same one
							again!
	Returns:	a ptr to the next node of the desired type in the tree as it traverses it
				depth first.
				NULL if the search is complete.
	Purpose:	Used to walk a tree repeatedly from any given branch,
				searching for nodes of a particular type.

	Use:		To find all textlines in a layer pLayer and process them
				Node * pNode = SliceHelper::FindNextOfClass(pLayer, pLayer, CC_RUNTIME_CLASS(TextLine));
				while (pNode)
					{
					pNode-> whatever you like;
					pNode = SliceHelper::FindNextOfClass(pNode, pLayer, CC_RUNTIME_CLASS(TextLine));
					}
				
	Errors:		-

********************************************************************************************/
Node * SliceHelper::FindNextOfClass(Node *pNode, Node * pLidNode, const class CCRuntimeClass * pClass, BOOL CheckThis)
{
	Node * pNextNode = NULL;
	BOOL tested = FALSE;
	Node * pPassedNode = CheckThis ? NULL : pNode;

	BOOL scanAttributes = FALSE;	// Attribute nodes do NOT ever return CC_RUNTIME_CLASS(NodeAttribute).
									// if were scanning for these, then we need to do things slightly differently ....

	if (pClass == CC_RUNTIME_CLASS(NodeAttribute))
	{
		scanAttributes = TRUE;
	}

	while (pNode != pLidNode || !tested)
	{
		tested = TRUE;

		// check this node as we move across
		if (!scanAttributes)
		{
			if (!pNode->IsNodeHidden() && pNode != pPassedNode && pNode->GetRuntimeClass() == pClass)
				return pNode;
		}
		else
		{
			if (!pNode->IsNodeHidden() && pNode != pPassedNode && pNode->IsAnAttribute ())
				return pNode;
		}

		// while can go down do so
		pNextNode = pNode->FindFirstChild();

		if (pNextNode && pNextNode->IsNodeHidden())
			pNextNode = pNextNode->FindNextNonHidden();
		
		while(pNextNode)
		{
			pNode = pNextNode;
			
			if (!scanAttributes)
			{
				// check this node as we move down
				if (!pNode->IsNodeHidden() && pNode != pPassedNode && pNode->GetRuntimeClass() == pClass)
					return pNode;
			}
			else
			{
				// check this node as we move down
				if (!pNode->IsNodeHidden() && pNode != pPassedNode && pNode->IsAnAttribute ())
					return pNode;
			}

			pNextNode = pNode->FindFirstChild();

			if (pNextNode && pNextNode->IsNodeHidden())
				pNextNode = pNextNode->FindNextNonHidden();
		}

		// cant go down and we are already at the top
		if (pNode == pLidNode)
			return NULL;

		// find next none hidden brother
		pNextNode = pNode->FindNextNonHidden();

		if (pNextNode)
			pNode = pNextNode;
		else // no brothers so find an uncle
		{
			BOOL FoundUncle = FALSE;

			while (!FoundUncle)
			{
				pNextNode = pNode->FindParent();
				if (pNextNode == pLidNode || !pNextNode)
					return NULL;

				pNode = pNextNode;
				pNextNode = pNode->FindNextNonHidden();

				// found uncle
				if (pNextNode)
				{
					pNode = pNextNode;
					FoundUncle = TRUE;
				}
			}
		}
	}

	return NULL;
}


/********************************************************************************************

>	BOOL SliceHelper::SelectObjectsInSet(const StringBase& strName,
											SelectScan::Change eNewState)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/10/99
	Returns:	TRUE if it succeeded
	Params:		strName		-	The set name to select
				nNewState	-	1 selects, 0 deselects, -1 toggles
	Purpose:	Wraps the scan functionality.
				Selects all items from the named set.
********************************************************************************************/
BOOL SliceHelper::SelectObjectsInSet(const StringBase& strName,
										SelectScan::Change eNewState)
{
	PORTNOTETRACE("dialog","SliceHelper::SelectObjectsInSet - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	SGNameItem* pItem = SliceHelper::LookupNameGalleryItem(strName);
	if (pItem == 0) return FALSE;
	SelectScan scanner(pItem, eNewState);
	scanner.Scan();
	return TRUE;
#else
	return FALSE;
#endif
}



/********************************************************************************************

>	void SliceHelper::SelectAllSetsOfThisNode(	NodeRenderableInk* pInk,
												String_256 & SetName, BOOL bShift )

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/12/99
	Returns:	
	Params:		pInk		-	The node to check
				SetName		-	The name of the set we are looking for.
				bShift		-	TRUE if the shift key is down (so it toggles)
								FALSE to shift -> select
	Purpose:	Calls RecurseSelectAllSetsOfThisNode after looking up the tree for any
				non-selectable nodes. These are tested for by looking for a FALSE return from
				PromoteHitTestOnChildrenToMe. We do this because the user may try to select
				eg a NodeBevelController with a name on it, the selection slips through to
				its children who don't have the name directly applied. We must therefore find
				the node the user thought they were clicking on, and work on from there.

				See RecurseSelectAllSetsOfThisNode for more.

********************************************************************************************/
void SliceHelper::SelectAllSetsOfThisNode(	NodeRenderableInk* pInk,
											String_256 & SetName, BOOL bShift )
{
	// go up the tree to find the highest non-hittest node above us.
	Node* pSelect = pInk;
	Node* pNext = pInk->FindParent();
	while (	pNext != NULL && !pNext->IsLayer() &&
			!pNext->PromoteHitTestOnChildrenToMe() )
	{
		pSelect = pNext;
		pNext = pNext->FindParent();
	}

	// ok, now make the *recursive* call to RecurseSelectAllSetsOfThisNode :->
	RecurseSelectAllSetsOfThisNode(pSelect, SetName, bShift);
}



/********************************************************************************************

>	void SliceHelper::RecurseSelectAllSetsOfThisNode(Node * pAttrs, String_256 & SetName, BOOL bShift)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com> (renamed by Karim from SelectAllSetsOfThisNode, 14/12/1999)
	Created:	4/10/99
	Returns:	TRUE if this bar name is used from this node down
	Params:		pAttrs		-	The node to check from
				SetName		-	The name of the set we are looking for.
				bShift		-	TRUE if the shift key is down (so it toggles)
								FALSE to shift -> select
	Purpose:	Looks at all template attributes applied to this node and its children.
				Sets the selection state of the any referred name sets according to the
				shift setting.
				ie. you pick a member of a set and it selects all members of that set.
				ie2. If it is a member of more than one set all those sets are selected.
********************************************************************************************/
void SliceHelper::RecurseSelectAllSetsOfThisNode(Node * pAttrs, String_256 & SetName, BOOL bShift)
{
	if (pAttrs->IsAnAttribute() && IS_A(pAttrs,TemplateAttribute))
	{
		if (SetName.CompareTo(((TemplateAttribute *)pAttrs)->GetParam()) != 0)
		{
			// this line tests if the name is a bar name rather than any old named thing that
			// the user may have added
			if (!((TemplateAttribute *)pAttrs)->GetQuestion().IsEmpty())
			{
				SetName = ((TemplateAttribute *)pAttrs)->GetParam();
				SliceHelper::SelectObjectsInSet(SetName, bShift ? SelectScan::TOGGLE
											   : SelectScan::SELECT/*_EXCLUSIVE*/);
			}
		}
	}
	else
	{
		pAttrs = pAttrs->FindFirstChild();

		while (pAttrs)
		{
			// recursive call!!!
			if (!pAttrs->IsNodeHidden())
				RecurseSelectAllSetsOfThisNode(pAttrs, SetName, bShift);
			pAttrs = pAttrs->FindNext();
		}
	}
}



/********************************************************************************************
>	void SliceHelper::SelectAllSetsInRect(DocRect Rect, Spread* pSpread,
										SliceHelper::SelStateAction st = SelStateAction::SET)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>, from Simon / JustinF's code
	Created:	12/10/1999
	Inputs:		Rect	the Rectangle to act as a bounding box for the selection.
				Spread	the spread on which to select the objects.
				st		either CLEAR, SET, or TOGGLE, which is what should be done to
						the selection state of each object within the rectangle. 
	Purpose:	For each object wholly inside the supplied rectangle, selects that object
				and all other objects sharing any names belonging to that object.
				It will draw in all the EORed blobs of the objects that it selects/deselects.
				rewritten by Simon so that objects on locked layers do not get selected
				also more optimal.
				(st parameter etc added by JCF 1.11.94)
	SeeAlso:	NodeRenderableInk::SelectAllInRect
********************************************************************************************/
void SliceHelper::SelectAllSetsInRect(const DocRect Rect, Spread* pSpread,
										SliceHelper::SelStateAction st)
{
	// Make sure we are not being told garbage
	ERROR3IF(pSpread == NULL, "SliceHelper::SelectAllSetsInRect- NULL pSpread");
	ERROR3IF(pSpread->FindFirstLayer() == NULL, "SliceHelper::SelectAllSetsInRect- no layer");

	// process all objects on modifiable layers within the given bounding box.
	String_256 SetName;
	Layer* pLayer = pSpread->FindFirstLayer();
	while (pLayer != NULL)
	{
		// proceed if the current layer is unlocked, visible and overlaps the given rect.
		if (!pLayer->IsLocked() && pLayer->IsVisible() &&
			pLayer->GetBoundingRect().IsIntersectedWith(Rect)) 
		{
			Range rng(pLayer->FindFirstChild(), NULL, RangeControl(TRUE, TRUE));
			Node* pNode = rng.FindFirst();
			while (pNode != NULL)
			{
				// make sure it is a Renderable Ink Node
				if (pNode->IsAnObject())
				{
					// Ok, this object is unselected and renderable,  
					// so now we can check if it is in the rect
					NodeRenderableInk* pInk = (NodeRenderableInk*) pNode;
					if (Rect.ContainsRect(pInk->GetBoundingRect()))
					{
						switch (st)
						{
						case CLEAR:
							// Karim MacDonald 22/10/1999
							// TODO: replace this with code for Named-Set deselection.
							//		 the only current reason for this is completeness.
							pInk->DeSelect(TRUE);
							break;

						case SET:
							// although our Range object should only return unselected nodes,
							// we're busy selecting sets of nodes within the loop => what was
							// unselected at the start of the loop will often be selected by
							// this point - hence the double-check for selection.
							if (!pInk->IsSelected())
							{
								SetName.Empty();
								SliceHelper::SelectAllSetsOfThisNode(pInk, SetName, FALSE);
								if (SetName.IsEmpty())
									pInk->Select(TRUE);
							}
							break;

						case TOGGLE:
							if (pInk->IsSelected())
							{
								// Karim MacDonald 22/10/1999
								// TODO: replace this with code for Named-Set deselection.
								//		 the only current reason for this is completeness.
								pInk->DeSelect(TRUE);
							}
							else
							{
								SetName.Empty();
								SliceHelper::SelectAllSetsOfThisNode(pInk, SetName, FALSE);
								if (SetName.IsEmpty())
									pInk->Select(TRUE);
							}
							break;

						default:
							ERROR3("SliceHelper::SelectAllSetsInRect- unknown SelStateAction");
							return;
						}
					}
				}

				// get the next Node to consider.
				pNode = rng.FindNext(pNode);
			}
		}

		// check the next layer.
		pLayer = pLayer->FindNextLayer();
	}

	// Update the selection cache
	GetApplication()->UpdateSelection();
}

/********************************************************************************************

>	String_256 SliceHelper::GetBarName(TemplateAttribute * pTemplAttrib)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com> JK
	Created:	4/10/99
	Returns:	The name of the bar
	Params:		pTemplAttrib-	The attrib node.
	Purpose:	The bar name is stored as the question in the wix attrib.
				This fn extracts the bar name from the attrib, checking that it
				is a bar name and not any old string in there.
				If it isn't a bar identifier it returns an empty string.
********************************************************************************************/
String_256 SliceHelper::GetBarName(TemplateAttribute * pTemplAttrib)
{
	String_256 BarName = pTemplAttrib->GetQuestion();

	// must start with the word "Bar"
	if (BarName[0] != 'B' || BarName[1] != 'a' || BarName[2] != 'r')
		BarName.Empty();

	return BarName;
}

/********************************************************************************************

>	Node * SliceHelper::ReplaceAttrsInTree(UndoableOperation * pOp,
									   List * pImportedAttrList,
									   const String_256 & OldButtonName,
									   const String_256 & OldBarName,
									   const String_256 & NewButtonName,
									   const String_256 & NewBarName,
									   Node ** ppNodeFound)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com> JK
	Created:	21/9/00
	Purpose:	Scans the pImportedAttrList looking for OldButtonName and OldBarName.
				Replaces it with new versions of the template attribute.
				Returns a ptr to an example of a changed attr node
				Fills in an example of an old node into ppNodeFound if required

********************************************************************************************/
Node * SliceHelper::ReplaceAttrsInTree(UndoableOperation * pOp,
									   List * pImportedAttrList,
									   const String_256 & OldButtonName,
									   const String_256 & OldBarName,
									   const String_256 & NewButtonName,
									   const String_256 & NewBarName,
									   Node ** ppNodeFound)
{

	Node * pExampleNode = NULL;
	NodeListItem * pItem = (NodeListItem *)pImportedAttrList->GetHead();
	NodeListItem * pNextItem = NULL;

	while (pItem)
	{
		pNextItem = (NodeListItem *)pImportedAttrList->GetNext(pItem);

		if (OldBarName.CompareTo(((TemplateAttribute *)(pItem->pNode))->GetQuestion()) == 0
			&& OldButtonName.CompareTo(((TemplateAttribute *)(pItem->pNode))->GetParam()) == 0)
		{
			// found a matching attr

			// set the found node if required
			if (ppNodeFound && !*ppNodeFound)
				*ppNodeFound = pItem->pNode;

			if (NewBarName != OldBarName || NewButtonName != OldButtonName)
			{

				// add an attrib of the new type next to this

				if (!NewButtonName.IsEmpty()) // dont add empty named names
				{
					// define the new attrib with the required button/extender name and bar name if relevent
					TemplateAttribute* pAttr = new TemplateAttribute(String_256(TEXT("ObjectName")),
														NewBarName,
														NewButtonName);

					if (!pExampleNode)
						pExampleNode = pAttr;

					// add the new attrib to the tree
					if (pAttr) 
					{
						pAttr->AttachNode(pItem->pNode, NEXT);
						// add undo info to hide this node?
					}	
				}

				// remove the old attrib
				// it pointed to the out of date information
				pOp->DoHideNode(pItem->pNode, FALSE);
			}
			else
				if (!pExampleNode)
					pExampleNode = pItem->pNode;


			// remove this entry from this list
			pImportedAttrList->RemoveItem((NodeListItem *)pItem);
			delete pItem;
		}

		// look at the next item in the list
		pItem = pNextItem;
	}

	return pExampleNode;
}

// scans the list to see if name is used by a template attrib in the list
BOOL SliceHelper::IsUniqueName(const String_256 & Name, List * pList)
{
	NodeListItem * pExistingListItem = (NodeListItem *)pList->GetHead();
	while (pExistingListItem)
	{
		if (Name.CompareTo(((TemplateAttribute *)(pExistingListItem->pNode))->GetParam()) == 0)
			return TRUE;

		pExistingListItem = (NodeListItem *)pList->GetNext(pExistingListItem);
	}

	return FALSE;
}


// extension to the NodeListItem to store other stuff needed in this MeshImportedLayersWithExistingButtonBars() fn
class MeshNodeListItem : public NodeListItem
{
public:
	MeshNodeListItem(	Node* WhichNode,
						Node* pNode2,
						INT32 type,
						INT32 level,
						BYTE flags,
						DocRect target,
						DocRect extender)
	{
		pNode = WhichNode;
		pNewNode = pNode2;
		ButtonClassification = type;
		StretchLevel = level;
		ExtendFlags = flags;
		rTarget = target;
		rExtender = extender;
		pExtender = NULL;
		pOldPropNode = NULL;
	};// initialise vars

	Node * pNewNode;
	Node * pExtender;
	Node * pOldPropNode;
	INT32 ButtonClassification;
	INT32 StretchLevel;
	BYTE ExtendFlags;
	DocRect rTarget;
	DocRect rExtender;
};

/********************************************************************************************

>	void SliceHelper::MeshImportedLayersWithExistingButtonBars(Node * pImportedLayer[5], UndoableOperation * pUndoableOp, BOOL Imported)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com> JK
	Created:	28/10/99
	Params:		pImportedLayer[5]	-	ptrs to the 5 (or less) nodes just imported
										in that represent the DEFAULT, MOUSE, CLICKED
										and SELECTED states (and now BACKBAR) in the imported document.
										These have to be merged with whatever is already
										in these layers proper. Imported nodes are added
										AFTER the existing nodes and each ptr points to
										the first node on the tree in each layer.
										All 5 ptrs must be supplied, they can individually
										be left NULL.
				pUndoableOp		-	This function must hide and move nodes so it must be part
									of an undoable op.
				Imported		-	Bool to tell if the bar is coming from an external file (TRUE)
									or copied/cloned from within this document (FALSE) so that the bar data
									can be scavinged.
									
	Purpose:	Used when a drawing (.XAR, .WEB) is imported or dragged into an existing
				drawing, or cloned. Button names and bar names are rationalised and made unique, after
				the nodes themselves have already been merged onto the existing special layers.
				The function also has to mesh the stretching properties of any buttons that it
				tampers with providing new extenders to go with each new button defined. The 
				extenders have to be uniquely named too and match with the correct button and
				the correct nodes of that button.
	Errors:		This function Only works with a single item stretching in a button as it stores
				it as a string "Extender" which it recieves from the SliceHelper function
				FindTargetAndExtender().
********************************************************************************************/
void SliceHelper::MeshImportedLayersWithExistingButtonBars(Node * pImportedLayer[5], UndoableOperation * pUndoableOp, BOOL Imported)
{
	PORTNOTETRACE("dialog","SliceHelper::MeshImportedLayersWithExistingButtonBars - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	INT32 BarReplacedWithBar[MAX_IMPORTED_BARS]; // 255 bars the max to import into a document
	memset (BarReplacedWithBar, -1, sizeof(BarReplacedWithBar));

	NameGallery* pNameGallery = NameGallery::Instance();
	pNameGallery->FastUpdateNamedSetSizes(); // update the existing names in the gallery

	String_256 Postfix;							// holds the "Extender" postfix
	Postfix.Load(_R(IDS_EXTENDER_POSTFIX));

	NodeSetSentinel * pNodeSetSentinel = Document::GetSelected()->GetSetSentinel();

	INT32 i; // general perpose use

	// the all important lists of Template attribute nodes in the tree
	List ExistingAttrList;
	List ImportedAttrList;
	List MeshList; // this one add lots more data too to rebuild the properties

	Node * pNode;
	Node * pParent;
	INT32 BarNo = 0; // used to store the unique bar number we have generated up to so far
	INT32 ButtonNo = 0; // used to store the unique button number we have generated up to so far

	//*** fill in the ExistingAttrList & the ImportedAttrList ***

	// fill the list of attrs that were in the tree before the import
	// and those that were in the tree after the import
	// look on each possible special rollover layers
	for (i = 0; i < 5; i++)
		if (pImportedLayer[i])
		{
			// test to see if the first item imported is in fact the first item in the layer
			// implying that the whole lot has been imported
			pParent = pImportedLayer[i]->FindParent();
			Node * pFirstNodeOnLayer = pParent->FindFirstChild(); 
			if ( pFirstNodeOnLayer && pFirstNodeOnLayer != pImportedLayer[i])
			{
				// there was stuff on this layer before
				pNode = SliceHelper::FindNextNameNode(pFirstNodeOnLayer, pImportedLayer[i]);
				while (pNode)
				{
					// add this node to the list
					NodeListItem * pItem = new NodeListItem(pNode);
					ExistingAttrList.AddTail(pItem);
					pNode = SliceHelper::FindNextNameNode(pNode, pImportedLayer[i]);
				}
			}

			// Add the attrs that have just been added to this layer
			pNode = SliceHelper::FindNextNameNode(pImportedLayer[i], pParent);
			while (pNode)
			{
				// add this node to the list
				NodeListItem * pItem = new NodeListItem(pNode);
				ImportedAttrList.AddTail(pItem);
				pNode = SliceHelper::FindNextNameNode(pNode, pParent);
			}
		}

	//*** Scan the imported attrs list ***

	NodeListItem * pNodeListItem = (NodeListItem *)ImportedAttrList.GetHead();

	// walk the imported list to 
	while (pNodeListItem)
	{
		String_256 OldButtonName = ((TemplateAttribute *)(pNodeListItem->pNode))->GetParam();
		String_256 OldBarName = ((TemplateAttribute *)(pNodeListItem->pNode))->GetQuestion();
		String_256 NewButtonName = OldButtonName;
		String_256 NewBarName = OldBarName;
		String_256 OldExtender = "";
		String_256 NewExtender = "";

		TRACEUSER( "Matt", _T("Found "));
		TRACEUSER("Matt", OldButtonName);
		TRACEUSER( "Matt", _T("\n"));


		// is it a button or a backbar or an extender?

		INT32 type = 0; // a regular button / target

		if (OldBarName.IsEmpty())
			type = 1; // an extender

		// look to the layer it is on is it layer[4]
		if (pImportedLayer[4])
		{
			pParent = pImportedLayer[4]->FindParent();
			pNode = pNodeListItem->pNode;
			while (!pNode->IsLayer())
				pNode = pNode->FindParent();

			if (pNode == pParent)
				type = 2; // a back bar
		}

		//*** generate a unique name for the button and the bar ***

		// make the bar name unique
		if (type != 1) // it is not an extender
		{
			INT32 ExistingBarno = SliceHelper::GetBarNumberFromBarName(OldBarName);
			ASSERT(ExistingBarno >=0);

			if (BarReplacedWithBar[ExistingBarno] == -1)
			{
				if (Imported) // if imported it has a known number to become!
				{
					BarNo = ExistingBarno + g_NoOfBarsBeforeImport + 1;

					NewBarName.MakeMsg(_R(IDS_BARNAME), BarNo);
				}
				else // find a bar name that hasn't been used in the existing drawing
				{
					BOOL AlreadyUsed = TRUE;
					do
					{
						BarNo++;
						NewBarName.MakeMsg(_R(IDS_BARNAME), BarNo);

						AlreadyUsed = FALSE;
						NodeListItem * pExistingListItem = (NodeListItem *)ExistingAttrList.GetHead();
						while (pExistingListItem && !AlreadyUsed)
						{
							if (NewBarName.CompareTo(((TemplateAttribute *)(pExistingListItem->pNode))->GetQuestion()) == 0)
								AlreadyUsed = TRUE;

							pExistingListItem = (NodeListItem *)ExistingAttrList.GetNext(pExistingListItem);
						}
					} while (AlreadyUsed);
				}

				BarReplacedWithBar[ExistingBarno] = BarNo - 1; // remember this bar decision next time we find a bar of that bar number
															  // the -1 is so we dont waste index zero of the array
															  // Bar1 = location zero etc. like in the bar properties node
			}
			else
				NewBarName.MakeMsg(_R(IDS_BARNAME), BarReplacedWithBar[ExistingBarno] + 1);

			// scan the items selected in the name gallery
			// if we have selected only part of a button do not try to duplicate the whole thing
			BOOL PartOfButtonSelected = FALSE;
			if (!Imported)
			{
				SGNameItem* pNameGalleryItem = (SGNameItem*) pNameGallery->GetUsedNames()->GetChild();
				while (pNameGalleryItem && !PartOfButtonSelected)
				{
					INT32 count = pNameGalleryItem->GetSelectedCount();
					if ( count > 0 && count < pNameGalleryItem->GetObjectCount())
					{
						// this named object is only partly selected
						String_256 TempExtenderName = "";
						pNameGalleryItem->GetNameText(&TempExtenderName);
						if (OldButtonName.CompareTo(TempExtenderName) == 0)
							PartOfButtonSelected = TRUE; // has part of this set
					}

					pNameGalleryItem = (SGNameItem *) pNameGalleryItem->GetNext();
				}
			}

			if (PartOfButtonSelected) // only if we are not importing 
			{

				String_256 Target = "";
				if ( SliceHelper::FindTargetAndExtender(	pNodeListItem->pNode->FindParent(),
													Target,
													OldExtender,
													0) == 2) // ie we found an extneder
				{
					// delete these attrs from the tree
					ReplaceAttrsInTree(pUndoableOp,
										&ImportedAttrList,
										OldExtender,
										"",
										"",
										"");
				}

				// delete these attrs from the tree
				ReplaceAttrsInTree(pUndoableOp,
									&ImportedAttrList,
									OldButtonName,
									OldBarName,
									"",
									"");
			}
			else
			{
				// create a unique button name
				DocRect r;
				r.MakeEmpty();

				MeshNodeListItem * pMeshData = new MeshNodeListItem(pNodeListItem->pNode,
							NULL,
							type,
							0,
							0,
							r,
							r);

				if (type == 2)
				{
					// back bars are just called BackBarX where X is the bar number
					NewButtonName.MakeMsg(_R(IDS_BACKBARNAME), BarReplacedWithBar[ExistingBarno]);

					BOOL AlreadyUsed = TRUE;
					do
					{
						AlreadyUsed = IsUniqueName(NewButtonName, &ExistingAttrList);

						if (AlreadyUsed)
							NewButtonName += "x";

					}while (AlreadyUsed);
				}
				else
				{
					// buttons / targets need to find unique names not used before in the existing button list
					BOOL AlreadyUsed = TRUE;
					do
					{
						AlreadyUsed = IsUniqueName(NewButtonName, &ExistingAttrList);

						if (AlreadyUsed)
						{
							ButtonNo++;
							NewButtonName.MakeMsg(_R(IDS_BUTTONNAME), ButtonNo);
						}

					} while (AlreadyUsed);
				}

				//*** Read the property data for the button and store this data in the Mesh List ***

				// find out what the extender for this used to be

				String_256 Target = "";
				pMeshData->StretchLevel = SliceHelper::FindTargetAndExtender(	pNodeListItem->pNode->FindParent(),
																	Target,
																	OldExtender,
																	0,
																	&(pMeshData->ExtendFlags),
																	&(pMeshData->rTarget),
																	&(pMeshData->rExtender) );

				pMeshData->pOldPropNode = pNodeSetSentinel->FindPropertyNode(OldButtonName);

				// replace all the instances of attrs marking the old button in the tree with
				// attrs of the new button and bar.
				// Returns an example node into pMeshData->NewNode
				pMeshData->pNewNode = ReplaceAttrsInTree(pUndoableOp, &ImportedAttrList, OldButtonName, OldBarName, NewButtonName, NewBarName);

				// add the data about the button to the mesh list
				MeshList.AddTail(pMeshData);

				// add this new node data to the "existing" now that it has been placed in the tree
				// so that any new items will not think they can reuse this name
				NodeListItem * pNewItem = new NodeListItem(pMeshData->pNewNode);
				ExistingAttrList.AddTail(pNewItem);


				// found an extender for the button?
				if (type == 0 && !OldExtender.IsEmpty())
				{
					Node ** ppButtonExtenderNode = &(pMeshData->pExtender);
					pMeshData = new MeshNodeListItem(	NULL,
														NULL,
														1, // its an extender
														0,
														0,
														r,
														r);

					// generate a unique extender name
					NewExtender = NewButtonName;
					NewExtender += Postfix;

					BOOL AlreadyUsed = TRUE;
					do
					{
						AlreadyUsed = IsUniqueName(NewExtender, &ExistingAttrList);

						if (AlreadyUsed)
							NewExtender += "x";

					}while (AlreadyUsed);

					pMeshData->pOldPropNode = pNodeSetSentinel->FindPropertyNode(OldExtender);

					pMeshData->pNewNode = ReplaceAttrsInTree(pUndoableOp,
															&ImportedAttrList,
															OldExtender,
															"",
															NewExtender,
															"",
															&(pMeshData->pNode) );

					if (pMeshData->pNewNode)
					{
						// add the data about the extender to the mesh list
						MeshList.AddTail(pMeshData);

						// let the button have a ptr to an example of an extender for it
						*ppButtonExtenderNode = pMeshData->pNewNode; 

						// add this new node data to the "existing" now that it has been placed in the tree
						// so that any new items will not think they can reuse this name
						pNewItem = new NodeListItem(pMeshData->pNewNode);
						ExistingAttrList.AddTail(pNewItem);
					}
					else
					{
						// failled to find the extenders attr in the tree
						// we could be copying just part of a button so treat the last as a bit of a button
						// and treat this as though it had no extender
						delete pMeshData;
					}
				}
			}

			// we have deleted items from the list so start at the begining again
			pNodeListItem = (NodeListItem *)ImportedAttrList.GetHead();
		}
		else // skipped an extender node so look at the next in the list
			pNodeListItem = (NodeListItem *)ImportedAttrList.GetNext(pNodeListItem);
	}

	
	//*** add the button properties to the sentinel from the data stored in the MeshDataList ***

	MeshNodeListItem * pMeshData = (MeshNodeListItem *)MeshList.GetHead();
	while (pMeshData)
	{
		// delete the property it is based on as this is now longer used
		// since we created an updated version above
		if (pMeshData->pOldPropNode && ((NodeSetProperty*) pMeshData->pOldPropNode)->m_Imported)
		{
			// delete the attr in the sentinel that comes with the property
			Node * pAttr = pNodeSetSentinel->FindLastChild(); 
			while (pAttr)
			{
				if (IS_A(pAttr, TemplateAttribute) && 
					((TemplateAttribute *)(pMeshData->pNode))->GetParam() == ((TemplateAttribute *)pAttr)->GetParam())
				{
					pUndoableOp->DoHideNode(pAttr, FALSE);
					break;
				}

				pAttr = pAttr->FindPrevious();
			}

			// delete the unused imported property
			pUndoableOp->DoHideNode(pMeshData->pOldPropNode, FALSE);
		}

		// set the new property
		switch (pMeshData->ButtonClassification)
		{
		case 0: // a normal button
			if (pMeshData->pExtender) // the level that says there is a extender for this
			{
				CreatePropertiesForSet( ((TemplateAttribute *)(pMeshData->pNewNode))->GetParam(),
										((TemplateAttribute *)(pMeshData->pNewNode))->GetQuestion(),
										TRUE, TRUE, FALSE,
										pMeshData->ExtendFlags,
										((TemplateAttribute *)(pMeshData->pExtender))->GetParam(),
										FALSE,
										&(pMeshData->rTarget),
										&(pMeshData->rExtender),
										pUndoableOp,
										((NodeSetProperty *)pMeshData->pOldPropNode));
			}
			else // create none stretching attribs for the button
				CreatePropertiesForSet(	((TemplateAttribute *)(pMeshData->pNewNode))->GetParam(),
										((TemplateAttribute *)(pMeshData->pNewNode))->GetQuestion(),
										TRUE, FALSE, FALSE, 0,
										TEXT(""), FALSE, NULL, NULL, pUndoableOp,
										((NodeSetProperty *)pMeshData->pOldPropNode));

			break;

		case 1: // an extender
			CreatePropertiesForSet(	((TemplateAttribute *)(pMeshData->pNewNode))->GetParam(),
									TEXT(""),
									FALSE, FALSE, FALSE, 0,
									TEXT(""), FALSE, NULL, NULL, pUndoableOp, 
									((NodeSetProperty *)pMeshData->pOldPropNode));
			break;

		case 2: // a back bar
			CreatePropertiesForSet(	((TemplateAttribute *)(pMeshData->pNewNode))->GetParam(),
									((TemplateAttribute *)(pMeshData->pNewNode))->GetQuestion(),
									 FALSE,
									 pMeshData->ExtendFlags != 0,
									 TRUE,
									 pMeshData->ExtendFlags,
									 TEXT(""), FALSE, NULL, NULL, pUndoableOp, 
									 ((NodeSetProperty *)pMeshData->pOldPropNode));
			break;

		}

		pMeshData = (MeshNodeListItem *)MeshList.GetNext(pMeshData);
	}

	//*** Sort out the sentinel node which contains node properties and template attribs of any buttons ***
	
	// remove the import tags from the properties that the import added to the tree
	// we should have already taken all the data we need from them
	// this should only be stuff that doesn't extend left in this set

	if (Imported)
	{
		pNode = pNodeSetSentinel->FindLastChild();
		Node * pNextNode = NULL;
		while (pNode)
		{
			// scan the sentinel for imported properties that are marked "imported"
			// NB scans are done from bottom to top to find the last added items
			// which will be where the import left them
			pNextNode = pNode->FindPrevious();
			if (IS_A(pNode, NodeSetProperty) && ((NodeSetProperty*) pNode)->m_Imported)
			{
				TRACE(((NodeSetProperty*) pNode)->GetName());
				TRACE( _T(" imported and not deleted\n"));

				((NodeSetProperty*) pNode)->m_Imported = FALSE;

			}

			pNode = pNextNode;
		}
	}

	//*** Sort out the bar properties ***

	// fill up the bar property data in the sentinel
	// we dont have to do this on an import as the import pushed this data in for us
	// and we made sure that the bar names / numbers used by the import matched the 
	// unique new ones we generated.
	// however if we are cloning or duplicating we need to expand the bar props
 	if (!Imported)
	{
		// get the ptr to the bar properties node
		NodeBarProperty * pNodeBarProperty = (NodeBarProperty*) ((NodeSetSentinel *)pNodeSetSentinel)->FindBarProperty();
		NodeBarProperty* pBarPropertyCopy = NULL; // our local copy of the props

		for (i = 0; i < MAX_IMPORTED_BARS; i++)
		{
			if (BarReplacedWithBar[i] != -1)
			{
				// found a new bar that we have put in

				// copy the old bar data into the new bar data as the new is a copy of the old
				if (pBarPropertyCopy == NULL)
					ALLOC_WITH_FAIL(pBarPropertyCopy, ((NodeBarProperty*) pNodeBarProperty->SimpleCopy()), pUndoableOp);

				if (pBarPropertyCopy)
				{
					if (pBarPropertyCopy->HowMany() <= BarReplacedWithBar[i])
					{
						UINT32 tempuint = 0;
						while ((pBarPropertyCopy->HowMany() <= BarReplacedWithBar[i]) && tempuint != UINT_MAX)
							tempuint = pBarPropertyCopy->Add(pBarPropertyCopy->Bar(i));
					}
					else
						pBarPropertyCopy->Bar(BarReplacedWithBar[i]) = pBarPropertyCopy->Bar(i);
				}
			}
		}

		// tidy up the bar properties hiding the old properties and replacing with the new
		// if the new exists
		if (pBarPropertyCopy)
		{
			pBarPropertyCopy->AttachNode(pNodeSetSentinel, LASTCHILD);
			pUndoableOp->DoHideNode(pNodeBarProperty, FALSE);
		}
	}

	//*** Tidy up and exit ***

	// the work is done tidy up the three lists
	NodeListItem * pListItem = (NodeListItem *)ExistingAttrList.GetHead();
	NodeListItem * pNextListItem = NULL;
	while (pListItem)
	{
		pNextListItem = (NodeListItem *)ExistingAttrList.GetNext(pListItem);

		// remove this entry from this list
		ExistingAttrList.RemoveItem((NodeListItem *)pListItem);
		delete pListItem;

		pListItem = pNextListItem;
	}

	pListItem = (NodeListItem *)ImportedAttrList.GetHead();
	pNextListItem = NULL;
	while (pListItem)
	{
		pNextListItem = (NodeListItem *)ImportedAttrList.GetNext(pListItem);

		// remove this entry from this list
		ImportedAttrList.RemoveItem((NodeListItem *)pListItem);
		delete pListItem;

		pListItem = pNextListItem;
	}

	pListItem = (NodeListItem *)MeshList.GetHead();
	pNextListItem = NULL;
	while (pListItem)
	{
		pNextListItem = (NodeListItem *)MeshList.GetNext(pListItem);

		// remove this entry from this list
		MeshList.RemoveItem((NodeListItem *)pListItem);
		delete pListItem;

		pListItem = pNextListItem;
	}

	SliceHelper::ValidateNodeSetSentinel();

	// update the name gallery when we are finished
	pNameGallery->FastUpdateNamedSetSizes();

	// Ensure that the Trigger sets are correctly flagged...
	SliceHelper::EnsureTriggerInfo();
#endif
}


/********************************************************************************************

>	BOOL SliceHelper::BarNameExists(Node * pNode, String_256 &BarName, Node * pLid = NULL)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com> JK
	Created:	4/10/99
	Returns:	TRUE if this bar name is used from this node down
	Params:		pNode		-	The node to check from
				BarName		-	The name of the bar we are looking for.
				pButtonName	-	If provided fills in the button name found in a bar.
	Purpose:	Just a quick check to see if this bar name is used anywhere here.
				Used to find unused bar names. 

// Modified non recursive version written 17/9/00 (sjk)
// pnode is the node to start looking from and pLid is the node to stop looking when we reach it
// if pnode == plid or plid == 0 then it scans the entire branch defined by pnode
********************************************************************************************/
BOOL SliceHelper::BarNameExists(Node * pNode, String_256 &BarName, Node * pLid)
{
	if (pLid == NULL)
		pLid = pNode;

	Node * pNextNode = FindNextNameNode(pNode, pLid);

	while (pNextNode)
	{
		if (BarName.CompareTo(((TemplateAttribute *)pNextNode)->GetQuestion()) == 0)
			return TRUE;

		pNextNode = FindNextNameNode(pNextNode, pLid);
	}

	return FALSE;
}
/********************************************************************************************

>	INT32 SliceHelper::FindTargetAndExtender(Node * pStartNode, String_256 & Target, String_256 & Extender, INT32 RequiredLevel, BYTE *pExtenderFlags)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com> JK
	Created:	11/11/99
	Returns:	The level of compliance found of target and extender so far
	Params:		pStartNode	-	The node to check from
				Target		-	Fills this in with the button that gets moved ( levels 1 & 2)
				Extender	-	The set that triggers the target to extend	(level 2)
				RequiredLevel-	Don't return for levels less than this
				pExtenderFlags-	If passed in and an extender is found this is filled in with the
								way the button extends.

	Purpose:	When you want to find out what extends what from a button level (ie you have a node).
********************************************************************************************/
INT32 SliceHelper::FindTargetAndExtender(Node * pStartNode,
										String_256 & Target,
										String_256 & Extender, 
										INT32 RequiredLevel, 
										BYTE *pExtenderFlags,
										DocRect *pTargetRect,
										DocRect *pExtenderRect)
{
	DocRect rTarget, rExtender;
	INT32 ret = 0;
	Node * pNode = SliceHelper::FindNextOfClass(pStartNode, pStartNode, CC_RUNTIME_CLASS(TemplateAttribute));
	while (pNode && ret < 2)
		{
		INT32 FoundLevel = 0;
		// if pNode extends
		NodeSetProperty* pSetProp =
				Document::GetSelected()->GetSetSentinel()->FindPropertyNode(((TemplateAttribute *)pNode)->GetParam());
		NamedStretchProp* pProp = NULL;
		if (pSetProp) 
			pProp = (NamedStretchProp*) pSetProp->GetProperty(NamedStretchProp::nIndex);

		if (pProp && !pProp->GetTriggers().empty())
		{
			FoundLevel = 2;
			// get this extender
			//Extender = first name in the list of sets that extends target;
			Extender = pProp->GetTriggers().front().m_strSet;

			// record the exteding flags
			if (pExtenderFlags)
				*pExtenderFlags = pProp->GetStretchType();
				
			rExtender = pProp->GetRefUnionTriggerBounds();
		}
		else if (!GetBarName((TemplateAttribute *)pNode).IsEmpty())
		{
			// found a bar entry
			FoundLevel = 1;
		}

		if (FoundLevel > RequiredLevel && FoundLevel > ret)
		{
			Target = ((TemplateAttribute *)pNode)->GetParam();
			ret = FoundLevel;
			if (pProp)
				rTarget = pProp->GetRefTargetBounds();
		}

		pNode = SliceHelper::FindNextOfClass(pNode, pStartNode, CC_RUNTIME_CLASS(TemplateAttribute));
		}
		
	if (pTargetRect)
		*pTargetRect = rTarget;
	if (pExtenderRect)
		*pExtenderRect = rExtender;

	return ret;
}


/********************************************************************************************

>	BOOL SliceHelper::CreatePropertiesForSet(	const String_256& SetName,
											const String_256& BarName,
											BOOL isSlice,
											BOOL Stretches,
											BOOL IsBackBar,
											BYTE StretchFlags,
											const String_256& StretchedBy,
											BOOL DontOverWrite,
											const String_256& strRefTriggerSet,
											UndoableOperation* pOp)

	Author:		Simon_Knight (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/11/99
	Params:		SetName		-	The name of the set that you wish to create properties for
				BarName		-	The name of the bar if it is to belong to one, every thing but a button
								should ahve this empty
				isSlice		-	Is it a slice
				Stretches	-	Does it stretch
				IsBackBar	-	Is it a back bar - if so it will find its own buttons and set its triggers
				StretchFlags-	If it stretches how does it stretch
				StretchedBy	-	What stretches it. This is currently limited to just one set name for
								the purposes of this function. It could be extended to be a list later in the day.
				DontOverWrite-	If this is TRUE it cuts out if the property already exists
				strRefBoundsSet - the set, if not empty, whose reference bounds for triggers/targets
								   should be copied into the newly created property.
				pOp			-	Ptr to the undoable op
	Returns:	TRUE if it succeeded
	Purpose:	If you create a named set these days you need to create properties in the node
				set sentinel too so that they are stored somewhere that can be saved out, undone, etc.
				So if you have made any in your code this is a useful little number.

********************************************************************************************/	
BOOL SliceHelper::CreatePropertiesForSet(	const String_256& SetName,
											const String_256& BarName,
											BOOL isSlice,
											BOOL Stretches,
											BOOL IsBackBar,
											BYTE StretchFlags,
											const String_256& StretchedBy,
											BOOL DontOverWrite,
											DocRect *pTargetRect,
											DocRect *pExtenderRect,
											UndoableOperation* pOp,
											NodeSetProperty * pExampleProp)
{
	PORTNOTETRACE("dialog","SliceHelper::CreatePropertiesForSet - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	NodeSetSentinel * pSentinel = Document::GetSelected()->GetSetSentinel();

	if (!pSentinel)
		return FALSE;

	// find the property node for the setname
	NodeSetProperty* pOldSetProp =pSentinel->FindPropertyNode(SetName);

	// hide the old properties if they existed
	if (pOldSetProp)
	{
		if (DontOverWrite)
			return TRUE; // it exists and that is good enough for us

		// need a ptr to an op to hide a node
		ASSERT(pOp);
		pOp->DoHideNode(pOldSetProp, FALSE);
		TRACE( _T("Deleting property set of "));
		TRACE(SetName);
		TRACE( _T("\n"));
	}

	// create the new properties
	NodeSetProperty* pNewSetProp = pSentinel->CreatePropertyNode(SetName);
	if (pNewSetProp == 0 || !pNewSetProp->CreateDefaults()) return FALSE;

	// debug info
	TRACE( _T("Creating property set of "));
	TRACE(SetName);
	TRACE( _T(" "));

	NamedSliceProp* pNamedSliceProp = !isSlice ? new NamedSliceProp(SetName, isSlice) : 0;
	if (!pNamedSliceProp && !isSlice) return FALSE;

	NamedStretchProp* pNamedStretchProp = new NamedStretchProp(SetName, StretchFlags, Stretches);
	if (!pNamedStretchProp)
	{
		if (pNamedSliceProp) delete pNamedSliceProp;
		return FALSE;
	}

	// add the export props from the example if passed one
	// this will remember that the user wanted it as a jpeg and saved as whatever
	if (pExampleProp)
	{
		NamedExportProp * pExampleExportProp = (NamedExportProp *) pExampleProp->GetProperty(NamedExportProp::nIndex);
		if (pExampleExportProp)
		{
			NamedExportProp * pNamedExportProp = new NamedExportProp(*pExampleExportProp);
			if (pNamedExportProp)
			{
				SGNameProp* pOld = pNewSetProp->SetProperty(pNamedExportProp);
				if (pOld)
					delete pOld;
			}
		}
	}

	// debugging info
	if (pNamedSliceProp) TRACE( _T("with slices "));
	if (Stretches) TRACE( _T("with stretching "));

	// add an extend trigger
	if (pNamedStretchProp)
	{
		if (!StretchedBy.IsEmpty() && !IsBackBar)
		{
			pNamedStretchProp->AddTrigger(StretchedBy);

			// Fix up the trigger and target reference rectangles.
			if (pTargetRect && pExtenderRect)
			{
				TRACE( _T("button"));
				pNamedStretchProp->SetRefTargetBounds(*pTargetRect);
				pNamedStretchProp->SetRefUnionTriggerBounds(*pExtenderRect);
			}
		}
		else
		if (IsBackBar) // build a list of all the members of the bar to make triggers for the back bar
		{
			NameGallery * pNameGallery = NameGallery::Instance();
			SGUsedNames* pNames = pNameGallery ? pNameGallery->GetUsedNames() : NULL;

			if (pNameGallery && pNames)
			{
				TRACE( _T("BackBar"));
				// scan to get data about everything
				pNameGallery->FastUpdateNamedSetSizes(FALSE);

				INT32 BarNo = GetBarNumberFromBarName(BarName);
				DocRect RefUnionTriggers;
				DocRect BackBarBounds;

				RefUnionTriggers.MakeEmpty();
				BackBarBounds.MakeEmpty();

				String_256 FoundSetName;

				// reset all the names
				SGNameItem * pNameGalleryItem = (SGNameItem*) pNames->GetChild();

				while (pNameGalleryItem)
				{
					if (pNameGalleryItem->m_BarNumber == BarNo )//&& !pNameGalleryItem->IsABackBar())
					{
						pNameGalleryItem->GetNameText(&FoundSetName); 

						if (FoundSetName.CompareTo(SetName) == 0)
						{
							// found yourself the back bar
							BackBarBounds = pNameGalleryItem->GetSetBounds();
						}
						else
						{
							// found a button of this bar
							RefUnionTriggers = RefUnionTriggers.Union(pNameGalleryItem->GetSetBounds());
							pNamedStretchProp->AddTrigger(FoundSetName);
						}
					}

					pNameGalleryItem = (SGNameItem *) pNameGalleryItem->GetNext();
				}

				pNamedStretchProp->SetRefTargetBounds(pTargetRect ? *pTargetRect : BackBarBounds);
				pNamedStretchProp->SetRefUnionTriggerBounds(pExtenderRect ? *pExtenderRect : RefUnionTriggers);
			}
		}
	}

	// replace the slice and stretch properties
	if (pNamedSliceProp)
	{
		SGNameProp* pOld = pNewSetProp->SetProperty(pNamedSliceProp);
		if (pOld)
			delete pOld;
	}

	if (pNamedStretchProp)
	{
		SGNameProp* pOld = pNewSetProp->SetProperty(pNamedStretchProp);
		if (pOld)
			delete pOld;
	}

	TRACE( _T("\n"));

	// Create a hide node action to hide the node when we undo
	// this node was attached by the fn "NodeSetProperty* pNewSetProp = pSentinel->CreatePropertyNode(SetName);"
	// which is 107 lines above
	HideNodeAction* UndoHideNodeAction;
	HideNodeAction::Init(pOp,                    
					  	 pOp->GetUndoActions(), //&UndoActions,
						 pNewSetProp, 
						 TRUE, 		 // Include subtree size 
					  	 ( Action**)(&UndoHideNodeAction));

	// now we have added the property we should also add the standard attrib
	// to the node set sentinel too
	Node * pSentinelChild = pSentinel->FindFirstChild();

	BOOL Found = FALSE;
	while (pSentinelChild && !Found)
	{
		if (pSentinelChild->IsAnAttribute())
		{
			if (SetName.CompareTo(((TemplateAttribute *)pSentinelChild)->GetParam()) == 0)
			{
				if ((BarName.CompareTo(((TemplateAttribute *)pSentinelChild)->GetQuestion()) == 0))
					Found = TRUE;
				else
				{
					pOp->DoHideNode(pSentinelChild, TRUE); // hide the near duplicate
					pSentinelChild = NULL; // end the search
				}
			}
		}

		if (pSentinelChild)
			pSentinelChild = pSentinelChild->FindNext();
	}

	if (!Found)
	{
		TemplateAttribute* pExtraAttr = new TemplateAttribute(String_256(TEXT("ObjectName")),
															BarName,
															SetName);

		if (!pExtraAttr) return TRUE;

		// add the attrib into the tree
		pExtraAttr->AttachNode(pSentinel, FIRSTCHILD);

		// Create a hide node action to hide the node when we undo 
		HideNodeAction* UndoHideNodeAction;
		HideNodeAction::Init(pOp,                    
					  		 pOp->GetUndoActions(), //&UndoActions,
							 pExtraAttr, 
							 TRUE, 		 // Include subtree size 
					  		 ( Action**)(&UndoHideNodeAction));

	}
#endif
	return TRUE;
}


/********************************************************************************************

>	BOOL SliceHelper::MakeTriggerLikeExample(const String_256& NewButtonName,
											 const String_256& ButtonName,
											 const String_256* pExclude)

	Author:		Simon_Knight (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/11/99
	Params:		NewButtonName	-	The button name that you wish to extend the things
				ButtonName		-	The name of button as an example of what you wish this would
									stretch things like
				pExclude		-	ptr to button to exclude looking at
	Returns:	TRUE if it succeeded, FALSE if we had a problem
	Purpose:	Adds NewButtonName to the lists of triggers that ButtonName belongs to.
				This is used by the bars thatwant all the buttons to stretch the same background bar.

********************************************************************************************/	
BOOL SliceHelper::MakeTriggerLikeExample(const String_256& NewButtonName,
										 const String_256& ButtonName,
										 const String_256* pExclude)
{
	NodeSetSentinel* pSentinel = Document::GetSelected()->GetSetSentinel();
	if (!pSentinel) return FALSE;

	// find the property node for the setname
	NodeSetProperty* pSetProp =
		(NodeSetProperty*) FindNextOfClass(pSentinel, pSentinel, CC_RUNTIME_CLASS(NodeSetProperty));
	NamedStretchProp* pProp = NULL;

	// walk the list of node set properties
	while (pSetProp)
	{
		pProp = (NamedStretchProp*) pSetProp->GetProperty(NamedStretchProp::nIndex);

		BOOL ok = TRUE;

		if (pExclude)
		{
			// exclude a particular name
			ok = pSetProp->GetName().CompareTo(*pExclude);
		}

		if (ok && pProp && !pProp->GetTriggers().empty())
		{			
			// walk the list of triggers looking for the button name
			// if we find it add the new trigger to the list
			std::list<TriggerSet>::iterator p =
				std::find(pProp->GetTriggers().begin(), pProp->GetTriggers().end(), ButtonName);
			// found ButtonName?
			if (p != pProp->GetTriggers().end())
			{
				// make sure we haven't added this before
				p = std::find(pProp->GetTriggers().begin(), pProp->GetTriggers().end(), NewButtonName);
				if (p == pProp->GetTriggers().end())
					pProp->AddTrigger(NewButtonName); // add this new trigger
			}
		}
		// find the next node set property
		pSetProp = (NodeSetProperty*) FindNextOfClass(pSetProp, pSentinel,
													  CC_RUNTIME_CLASS(NodeSetProperty));
	}

	return TRUE;

}

/********************************************************************************************

>	DocRect SliceHelper::BoundingNodeSize(Node * pNode)

	Author:		Simon_Knight (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/3/00
	Params:		pNode	- the node that you want to know how big it is
	Returns:	its size
	Purpose:	Get bounds was returning the acturate "how big is it"
				This fn treats text that is of a size and the same font as being
				the same size ignoring decsenders and ascenders.

  * WARNING *	It does this by examining the char size of the first char in each line
				and getting a min and max height. It uses the width from getbounds().
				So if fonts change mid way then descenders become a problem again.
				But this is faster than checking each char.

********************************************************************************************/	
DocRect SliceHelper::BoundingNodeSize(Node * pNode)
{
	if (!pNode->IsBounded())
		return DocRect(0,0,0,0);

	DocRect rBounds = ((NodeRenderableBounded*) pNode)->GetBoundingRect();

	if (IS_A(pNode,TextStory))
	{
		DocRect rTextBounds;
		Node * pLid = pNode; 
		pNode = SliceHelper::FindNextOfClass(pLid, pLid, CC_RUNTIME_CLASS(TextChar));

		if (pNode)
		{
			if (((TextChar*) pNode)->GetMetricsRectBounds(&rTextBounds))
			{
				rBounds.hi.y = rTextBounds.hi.y;
				rBounds.lo.y = rTextBounds.lo.y;
			}
		}
		else
			return DocRect(0,0,0,0); // no text chars should return empty

		while (pNode)
		{
			pNode = SliceHelper::FindNextOfClass(pNode, pLid, CC_RUNTIME_CLASS(TextLine));

			if (pNode)
			{
				pNode = SliceHelper::FindNextOfClass(pNode, pLid, CC_RUNTIME_CLASS(TextChar));
				if (pNode)
				{
					if (((TextChar*) pNode)->GetMetricsRectBounds(&rTextBounds))
					{
						rBounds.hi.y = max(rTextBounds.hi.y, rBounds.hi.y);
						rBounds.lo.y = min(rTextBounds.lo.y, rBounds.lo.y);
					}
				}
			}
		}
	}
	else
	if (IS_A(pNode,NodeShadowController)) // ignore the size of the shadow and just get me the size of the other bits
	{
		rBounds.MakeEmpty();

		pNode = pNode->FindFirstChild();

		while (pNode)
		{
			if (!pNode->IsAnAttribute() && !pNode->IsNodeHidden() && !IS_A(pNode, NodeShadow))
				rBounds = rBounds.Union(BoundingNodeSize(pNode));

			pNode = pNode->FindNext();
		}
		
	}
	else
	if (IS_A(pNode,NodeBevelController)) // ignore the size of the bevel and just get me the size of the other bits
	{
		rBounds.MakeEmpty();

		INT32 BevWidth = ((NodeBevelController *)pNode)->m_bOuter ? ((NodeBevelController *)pNode)->m_Indent : 0;
		pNode = pNode->FindFirstChild();

		while (pNode)
		{
			if (!pNode->IsAnAttribute() && !pNode->IsNodeHidden()
				&& !IS_A(pNode, NodeBevel) && !IS_A(pNode, NodeBevelBegin))
			{
				rBounds = rBounds.Union(BoundingNodeSize(pNode));
			}

			pNode = pNode->FindNext();
		}

		if (!rBounds.IsEmpty())
			rBounds.Inflate(BevWidth); // inflate the other bits' size by the bevel width
		
	}
	else
	if (IS_A(pNode,NodeContourController)) // ignore the size of the contour and just get me the size of the other bits
	{
		rBounds.MakeEmpty();

		// -ve width means outer contour!!?!
		// this is crazy but ignore inners so negate it to get the actual width increase
		INT32 Width = -((NodeContourController *)pNode)->GetWidth(); 
		pNode = pNode->FindFirstChild();

		while (pNode)
		{
			if (!pNode->IsAnAttribute() && !pNode->IsNodeHidden()
				&& !IS_A(pNode, NodeContour))
			{
				rBounds = rBounds.Union(BoundingNodeSize(pNode));
			}

			pNode = pNode->FindNext();
		}

		if (Width > 0)
			rBounds.Inflate(Width);	// inflate the size of the other bits' by the width of the contour
	}

	return rBounds;
}


/********************************************************************************************

>	INT32 SliceHelper::GetBarNumberFromBarName(const String_256 & BarName)

	Author:		Simon_Knight (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/4/00
	Returns:	the bar number (-1 for not in a bar)
	Purpose:	Converts the bar name into a bar number

********************************************************************************************/	
INT32 SliceHelper::GetBarNumberFromBarName(const String_256 & BarName)
{
	PTSTR pszMark = NULL;
	if (BarName.Length() > 3)
		return camStrtol( ((const TCHAR*)BarName) + 3, &pszMark, 10 ) - 1;

	return -1;
}

/***************************************/




/********************************************************************************************

>	TemplateAttribute * SliceHelper::FindFirstSetNodeBelongsTo(Node * pNode)

	Author:		Simon_Knight (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/5/00
	Returns:	
	Purpose:	returns the fist attrib that may apply to a node

********************************************************************************************/	
TemplateAttribute * SliceHelper::FindFirstSetNodeBelongsTo(Node * pNode)
{
	while (!pNode->IsLayer())
	{
		// test children for wix attrib
		Node * pChild = pNode->FindFirstChild();

		while (pChild)
		{
			if (pChild->IsAnAttribute() && IS_A(pChild,TemplateAttribute))
				return (TemplateAttribute *) pChild;

			pChild = pChild->FindNext();
		}

		pNode = pNode->FindParent();
	}

	return NULL;
}

/********************************************************************************************

>	String_256 SliceHelper::GetSetNameFromAttrib(Node * pNode)

	Author:		Simon_Knight (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/5/00
	Returns:	The set name from the attrib empty if it is not an attrib
	Purpose:	returns the fist attrib that may apply to a node

********************************************************************************************/	
String_256 SliceHelper::GetSetNameFromAttrib(Node * pNode)
{
	if (IS_A(pNode,TemplateAttribute))
		return ((TemplateAttribute *)pNode)->GetParam();
	else
		return "";
}

/********************************************************************************************

>	TextStory * SliceHelper::FindNextTextStoryToSync( TextStory * pLastStory, Node * pLid, TextStory * pMasterStory, String_256 & ButtonName)

	Author:		Simon_Knight (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/5/00
	Returns:	The next text story that should be edited if the master has been edited
	Purpose:	scans for text stories that are of the named set and match the criteria to edit
				when the master is edited.

********************************************************************************************/	
TextStory * SliceHelper::FindNextTextStoryToSync( TextStory * pLastStory, Node * pLid, TextStory * pMasterStory,
												 const String_256 & ButtonName, const String_256 & MasterText,
												 BOOL AnyText)
{
	Node * pStart = pLastStory;

	if (!pStart)
		pStart = pLid;

	Node * pNode = NULL;

	BOOL ok;
	TemplateAttribute * pAttrib = NULL;

	do
	{
		// find the next text story
		pNode = FindNextOfClass(pStart, pLid, CC_RUNTIME_CLASS(TextStory));

		ok = TRUE;

		if (!pNode)
			ok = FALSE;

		if (ok && pNode == pMasterStory)
			ok = FALSE;


		if (ok)
		{
			// is it in the right set?
			pAttrib = FindFirstSetNodeBelongsTo(pNode);
			if (!pAttrib)
				ok = FALSE; // no set
		}

		if (ok)
		{
			// the name of the set must match and the question should have a string that implies it is a bar
			// not just any old user named set
			if (ButtonName.CompareTo(pAttrib->GetParam()) != 0 || pAttrib->GetQuestion().IsEmpty())
			{
				ok = FALSE; // wrong set

				// check other set names for this text story
				while (!ok && pAttrib)
				{
					// look at brothers of this template attrib
					pAttrib = (TemplateAttribute *) pAttrib->FindNext(CC_RUNTIME_CLASS(TemplateAttribute));
					if (pAttrib && ButtonName.CompareTo(pAttrib->GetParam()) == 0 && !pAttrib->GetQuestion().IsEmpty())
						ok = TRUE;
				}
			}
		}

		// does its text match?
		if (ok && !AnyText)
		{
			if (MasterText != ((TextStory *)pNode)->GetStoryAsString())
				ok = FALSE;
		}

		pStart = pNode;

	} while (!ok && pNode);

	return (TextStory *) pNode;
}


BOOL SliceHelper::TextStoriesHaveSameText (TextStory * pStory1, TextStory * pStory2)
{
	Node * pTextLine1 = pStory1->FindFirstChild(CC_RUNTIME_CLASS(TextLine));
	Node * pTextLine2 = pStory2->FindFirstChild(CC_RUNTIME_CLASS(TextLine));

	while (pTextLine1 && pTextLine2)
	{
		if (!TextLinesHaveSameText((TextLine *)pTextLine1, (TextLine *)pTextLine2))
			return FALSE;

		pTextLine1 = pTextLine1->FindNext(CC_RUNTIME_CLASS(TextLine));
		pTextLine2 = pTextLine2->FindNext(CC_RUNTIME_CLASS(TextLine));
	}

	if (!pTextLine1 && !pTextLine2)
		return TRUE; // both empty!

	return FALSE; // one empty
}

BOOL SliceHelper::TextLinesHaveSameText (TextLine * pLine1, TextLine * pLine2)
{
//	TextNode * pVTN1 = pLine1->FindFirstVTN();
//	TextNode * pVTN2 = pLine2->FindFirstVTN();
	Node * pChar1 = pLine1->FindFirstChild(CC_RUNTIME_CLASS(TextChar));
	Node * pChar2 = pLine2->FindFirstChild(CC_RUNTIME_CLASS(TextChar));

	while (pChar1 && pChar2)
	{
		if (((TextChar *)pChar1)->GetUnicodeValue() != ((TextChar *)pChar2)->GetUnicodeValue())
			return FALSE;

		pChar1 = pChar1->FindNext(CC_RUNTIME_CLASS(TextChar));
		pChar2 = pChar2->FindNext(CC_RUNTIME_CLASS(TextChar));
	}

	if (!pChar1 && !pChar2)
		return TRUE; // both empty!

	return FALSE; // one empty
}


/********************************************************************************************

>	BOOL SliceHelper::SyncTextStories (TextStory * pStory, TextStory * pMaster, UndoableOperation * pOp)

	Author:		Simon_Knight (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/5/00
	Returns:	TRUE if it changes pStory
	Purpose:	Changes the characters in the story pStory to match those in pMaster.
				In this process it attempts to minimise the amount of undo information required.
				Calls the fn bellow SyncTextLines()

********************************************************************************************/	
BOOL SliceHelper::SyncTextStories (TextStory * pStory, TextStory * pMaster, UndoableOperation * pOp)
{
	Node * pTextLine = pStory->FindFirstChild(CC_RUNTIME_CLASS(TextLine));
	Node * pMasterTextLine = pMaster->FindFirstChild(CC_RUNTIME_CLASS(TextLine));

	BOOL ret = FALSE;

	while (pTextLine || pMasterTextLine)
	{
		pStory->ReleaseCached();
		pOp->DoInvalidateRegion(Document::GetSelectedSpread(), pStory->GetBoundingRect());

		// only existing line
		if (pTextLine && pMasterTextLine)
		{
			SyncTextLines((TextLine*)pTextLine, (TextLine*)pMasterTextLine, pOp);
			ret = TRUE;
			pTextLine = pTextLine->FindNext(CC_RUNTIME_CLASS(TextLine));
			pMasterTextLine = pMasterTextLine->FindNext(CC_RUNTIME_CLASS(TextLine));
		}
		else
		if (!pTextLine)
		{
			// master has added a line do so and then sync
			pTextLine = TextLine::CreateEmptyTextLine(pStory, LASTCHILD);

			SyncTextLines((TextLine*)pTextLine, (TextLine*)pMasterTextLine, pOp);
			ret = TRUE;

			pTextLine = NULL;
			pMasterTextLine = pMasterTextLine->FindNext(CC_RUNTIME_CLASS(TextLine));
		}
		else
		if (!pMasterTextLine)
		{
			// master has deleted a line

			// get the caret onto the top story to make sure it is not deleted out of the story
			if (pStory->GetCaret()->FindParent() == pTextLine)
				pStory->MoveCaretToCharacter(pStory->FindFirstVTN(), PREV);

			TextLine * pTemp = (TextLine *)pTextLine;
			pTextLine = pTextLine->FindNext(CC_RUNTIME_CLASS(TextLine));
			pOp->DoHideNode(pTemp, TRUE);
			ret = TRUE;
		}

	}

	if (ret)
	{
		pStory->FormatAndChildren(pOp);
		// We also need to invalidate the region of the story
		//pOp->DoInvalidateNodeRegion((NodeRenderableBounded*) pStory, TRUE, FALSE);
		pStory->ReleaseCached();
		pOp->DoInvalidateRegion(Document::GetSelectedSpread(), pStory->GetBoundingRect());
	}

	return ret;
}



BOOL SliceHelper::SyncTextLines (TextLine * pLine1, TextLine * pLine2, UndoableOperation * pOp)
{
	PORTNOTETRACE("dialog","SliceHelper::SyncTextLines - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	Node * pChar1 = pLine1->FindFirstChild(CC_RUNTIME_CLASS(TextChar));
	Node * pChar2 = pLine2->FindFirstChild(CC_RUNTIME_CLASS(TextChar));

	BOOL ok = TRUE;

	TRACEUSER( "Matt", _T("\n#############################################\nStart: "));

	// find the first 2 different chars
	while (ok && pChar1 && pChar2)
	{
		if (((TextChar *)pChar1)->GetUnicodeValue() != ((TextChar *)pChar2)->GetUnicodeValue())
			ok = FALSE;

		if (ok)
		{
			TRACEUSER( "Matt", _T("%c="), ((TextChar *)pChar1)->GetUnicodeValue());
			pChar1 = pChar1->FindNext(CC_RUNTIME_CLASS(TextChar));
			pChar2 = pChar2->FindNext(CC_RUNTIME_CLASS(TextChar));
		}
	}

	// Check if the two strings were the same - no point continuing!
	if (ok && !pChar1 && !pChar2)
		return TRUE; // they were the same!


	// Matt 15/12/2000
	// If the master has run out of next chars before the slave, then we should just remove everything from the end of the slave line and return
	if (ok && pChar1 && !pChar2)
	{
		while (pChar1)
		{
			Node * pNodeNext = pChar1->FindNext(CC_RUNTIME_CLASS(TextChar));
			TRACEUSER( "Matt", _T("Deleting From Slave %c\n"), ((TextChar *)pChar1)->GetUnicodeValue());
			pOp->DoHideNode(pChar1, TRUE);
			pChar1 = pNodeNext;
		}

		return TRUE;
	}


	Node * pChar1e = pLine1->FindLastChild(CC_RUNTIME_CLASS(TextChar));
	Node * pChar2e = pLine2->FindLastChild(CC_RUNTIME_CLASS(TextChar));

	ok = TRUE;

	BOOL RequireDel = FALSE;
	BOOL RequireAdd = FALSE;


	TRACEUSER( "Matt", _T("\nEnd:   "));

	// find the last 2 different chars
	while (ok && pChar1e && pChar2e)
	{
		if (((TextChar *)pChar1e)->GetUnicodeValue() != ((TextChar *)pChar2e)->GetUnicodeValue())
			ok = FALSE;

		if (ok && pChar1e == pChar1)
		{
			ok = FALSE;
			RequireAdd = TRUE;
		}

		if (ok && pChar2e == pChar2)
		{
			ok = FALSE;
			RequireDel = TRUE;
		}

		if (ok)
		{
			TRACEUSER( "Matt", _T("%c="), ((TextChar *)pChar1e)->GetUnicodeValue());
			pChar1e = pChar1e->FindPrevious(CC_RUNTIME_CLASS(TextChar));
			pChar2e = pChar2e->FindPrevious(CC_RUNTIME_CLASS(TextChar));
		}
	}
	TRACEUSER( "Matt", _T("\n"));


	BOOL AddChar2e = FALSE;

	// Matt 15/12/2000
	// Bodge-Case Flagging to make it add the correct no. of chars after concatenating two lines
	if (!ok && !pChar1 && pChar1e)
	{
		if (pChar1e->FindNext(CC_RUNTIME_CLASS(TextChar)))
		{
			TRACEUSER( "Matt", _T("\nFlagging AddChar2e\n"));
			AddChar2e = TRUE;
		}
	}

	// add chars just before pchar1 of the range of text chars between pchar2 and pchar2e
	Node * pNode = pChar2;
	Node * pDest = pChar1;
	AttachNodeDirection TailAttachDirection = PREV;

	if (!pDest)
	{
		// add the chars onto the end

		// put it before the EOFL node
		pDest = pLine1->FindLastChild(CC_RUNTIME_CLASS(EOLNode));
		if (!pDest)
		{
			// failling that slap it on the end
			pDest = pLine1;
			TailAttachDirection = LASTCHILD;
		}
	}

	if (!RequireDel) RequireDel = pChar1e && pChar1 && pChar1e->FindNext(CC_RUNTIME_CLASS(TextChar)) != pChar1;
	if (!RequireAdd)  RequireAdd = pChar2e && pChar2e->FindNext(CC_RUNTIME_CLASS(TextChar)) != pChar2;

	if (RequireDel) TRACEUSER( "Matt", _T("Requires a del\n"));
	if (RequireAdd)	TRACEUSER( "Matt", _T("Requires an add\n"));

	ok = TRUE;

	while (RequireAdd && pNode && ok)
	{
		// this it the last time?
		ok =  pNode != pChar2e;

		// Make a new text char node
		TextChar * pTheCopy = NULL;

		ALLOC_WITH_FAIL(pTheCopy, (new TextChar()) , pOp);
		pTheCopy->SetUnicodeValue(((TextChar *)pNode)->GetUnicodeValue());

		TRACEUSER( "Matt", _T("Adding %c\n"), ((TextChar *)pNode)->GetUnicodeValue());

		// insert the node under the destination
		pOp->DoInsertNewNode((NodeRenderableBounded *)pTheCopy, pDest, TailAttachDirection,
					 TRUE, 		// Do Invalidate region 
					 FALSE,		// Dont Clear the selections
					 FALSE,		// Dont select this object
					 FALSE); 	// Dont normalise attribs

		// attach the next node after this one
		TailAttachDirection = NEXT;
		pDest = pTheCopy;

		pNode = pNode->FindNext(CC_RUNTIME_CLASS(TextChar));
	}

	// delete the text chars between pchar1 & pchar1e
	pNode = pChar1;
	BOOL hiddenall = FALSE;

	if (RequireDel)
		while (!hiddenall)
		{
			if (pNode == pChar1e)
				hiddenall = TRUE;

			if (pNode)
			{
				Node * pNodeNext = pNode->FindNext(CC_RUNTIME_CLASS(TextChar));
				TRACEUSER( "Matt", _T("Deleting %c\n"), ((TextChar *)pNode)->GetUnicodeValue());
				pOp->DoHideNode(pNode, TRUE);
				pNode = pNodeNext;
			}
			else
				hiddenall = TRUE;
		}

	// Matt 15/12/2000
	// If it was necessary to flag above that pChar2e and all following chars should be added, then do so...
	if (AddChar2e && pChar2e)
	{
		AttachNodeDirection TailAttachDirection = PREV;
		Node * pDestination = pLine1->FindLastChild(CC_RUNTIME_CLASS(EOLNode));
		Node * pNode = pChar2e->FindNext(CC_RUNTIME_CLASS(TextChar));

		if (!pDestination)
		{
			pDestination = pLine1;
			TailAttachDirection = LASTCHILD;
		}

		while (pNode)
		{
			// Make a new text char node
			TextChar * pTheCopy = NULL;

			ALLOC_WITH_FAIL(pTheCopy, (new TextChar()) , pOp);
			pTheCopy->SetUnicodeValue(((TextChar *)pNode)->GetUnicodeValue());

			TRACEUSER( "Matt", _T("Extra-Adding %c\n"), ((TextChar *)pTheCopy)->GetUnicodeValue());

			// insert the node under the destination
			pOp->DoInsertNewNode((NodeRenderableBounded *)pTheCopy, pDestination, TailAttachDirection,
						 TRUE, 		// Do Invalidate region 
						 FALSE,		// Dont Clear the selections
						 FALSE,		// Dont select this object
						 FALSE); 	// Dont normalise attribs

			// attach the next node after this one
			TailAttachDirection = NEXT;
			pDestination = pTheCopy;

			pNode = pNode->FindNext(CC_RUNTIME_CLASS(TextChar));
		}
	}

	TRACEUSER( "Matt", _T("Done\n"));
#endif
	return TRUE;
}

/********************************************************************************************

>	BOOL SliceHelper::OnTextStoryChanged(TextStory * pMasterStory, UndoableOperation * pOp, ObjChangeParam * pObjChange)

	Author:		Simon_Knight (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/5/00
	Returns:	TRUE if any changes were made
	Purpose:	Call this if a master text may have been edited.
				It tests to see if the text is a master text.
				Finds slave text stories and syncs these with the master text.

********************************************************************************************/	
BOOL SliceHelper::OnTextStoryChanged(TextStory * pMasterStory, UndoableOperation * pOp, ObjChangeParam * pObjChange, const String_256 & MasterText)
{
	TemplateAttribute * pAttrib = SliceHelper::FindFirstSetNodeBelongsTo(pMasterStory);

	// the attrib is not a bar attrib? 
	while (pAttrib && pAttrib->GetQuestion().IsEmpty())
	{
		// look at brothers of this template attrib
		pAttrib = (TemplateAttribute *) pAttrib->FindNext(CC_RUNTIME_CLASS(TemplateAttribute));
	}
	
	if (!pAttrib)
		return FALSE;

	String_256 MasterSetName = SliceHelper::GetSetNameFromAttrib((Node *)pAttrib);

	if (MasterSetName.IsEmpty())
		return FALSE;

	Spread * pSpread =  Document::GetSelectedSpread();

	CaretNode * pCaret = pMasterStory->GetCaret();
	BOOL CaretSelected = pCaret->IsSelected();

	Node * pStory = SliceHelper::FindNextTextStoryToSync(NULL, pSpread, pMasterStory, MasterSetName, MasterText);
	
	BOOL ret = FALSE;

	while (pStory)
	{
		if (pStory != pMasterStory)
		{
			if (!pObjChange || ((TextStory *)pStory)->GetCaret()->AllowOp(pObjChange))
			{
				SliceHelper::SyncTextStories((TextStory *)pStory, pMasterStory, pOp);
				ret = TRUE;
			}
		}

		pStory = SliceHelper::FindNextTextStoryToSync((TextStory *)pStory, pSpread, pMasterStory, MasterSetName, MasterText);
	}

	pCaret->SetSelected(CaretSelected);

	return ret;
}


/********************************************************************************************

>	BOOL PurgeUseOfSetName(const StringBase & SetName, UndoableOperation* pOp)

	Author:		Simon_Knight (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/5/00
	Returns:	TRUE if any changes were made
	Purpose:	"SetName" should be a new clean set
				there should be no old references to this "SetName" in any extend lists
				remove them if there are any.
				find the property node for the setname

********************************************************************************************/	
BOOL SliceHelper::PurgeUseOfSetName(const StringBase & SetName, UndoableOperation* pOp, const String_256 * pReplacementName)
{
	NodeSetSentinel* pSentinel = Document::GetSelected()->GetSetSentinel();
	if (!pSentinel) return FALSE;

	NodeSetProperty* pSetProp =
		(NodeSetProperty*) FindNextOfClass(pSentinel, pSentinel, CC_RUNTIME_CLASS(NodeSetProperty));
	NamedStretchProp* pProp = NULL;

	BOOL ret = FALSE;

	// walk the list of node set properties
	while (pSetProp)
	{
		pProp = (NamedStretchProp*) pSetProp->GetProperty(NamedStretchProp::nIndex);

		BOOL ok = TRUE;

		if (ok && pProp && !pProp->GetTriggers().empty())
		{			
			// walk the list of triggers looking for the set name
			// if we find it remove this trigger from the list
			std::list<TriggerSet>::iterator p =
				std::find(pProp->GetTriggers().begin(), pProp->GetTriggers().end(), SetName);
			// found SetName?
			if (p != pProp->GetTriggers().end())
			{
				NodeSetProperty* pNewSetProp = (NodeSetProperty*) pSetProp->SimpleCopy();
				if (pNewSetProp)
				{
					((NamedStretchProp*) pNewSetProp->GetProperty(NamedStretchProp::nIndex))->RemoveTrigger(SetName);
					if (pReplacementName)
						((NamedStretchProp*) pNewSetProp->GetProperty(NamedStretchProp::nIndex))->AddTrigger(*pReplacementName);

					// add the prop into the tree
					pNewSetProp->AttachNode(pSetProp, NEXT);

					// Create a hide node action to hide the node when we undo 
					HideNodeAction* UndoHideNodeAction;
					HideNodeAction::Init(pOp,                    
					  					 pOp->GetUndoActions(), //&UndoActions,
										 pNewSetProp, 
										 TRUE, 		 // Include subtree size 
					  					 ( Action**)(&UndoHideNodeAction));

					// delete the original
					pOp->DoHideNode(pSetProp, TRUE); // hide the near duplicate

					// carry on the search from here
					pSetProp = pNewSetProp;

					ret = TRUE;
				}
			}
		}
		// find the next node set property
		pSetProp = (NodeSetProperty*) FindNextOfClass(pSetProp, pSentinel,
													  CC_RUNTIME_CLASS(NodeSetProperty));
	}

	return ret;
}


/********************************************************************************************

>	DocRect SliceHelper::ScanForSetSizeExcluding (const String_256 & IncludeSet, const String_256 & ExcludeSet)

	Author:		Simon_Knight (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/6/00
	Returns:	The bounding rect of the set "IncludeSet" taking all members of "IncludeSet"
				that do NOT belong in "ExcludeSet" aswell.
	Purpose:	Caculating clean set sizes for extending buttons

********************************************************************************************/	
DocRect SliceHelper::ScanForSetSizeExcluding (const String_256 & IncludeSet, const String_256 & ExcludeSet)
{
	DocRect r;
	r.MakeEmpty();

	// scan the tree looking for name attribs
	// find one and the size of the parent should be added to the bounds of the set
	Spread * pSpread =  Document::GetSelectedSpread();
	if (!pSpread)
		return r;

	Node * pNode = NULL;
	Node * pTop = pSpread;
	Node * pNewNode = NULL;

	DocRect TempRect;
	TempRect.MakeEmpty();
	BOOL CleanNode = TRUE;

	// scan from the first layer all through the layers since they are brothers of this layer
	pNode = SliceHelper::FindNextOfClass(pTop, pTop, CC_RUNTIME_CLASS(TemplateAttribute));

	// for each marker in the tree
	while (pNode)
	{
		BOOL Siblings = TRUE;
		do
		{
			if (IncludeSet.CompareTo(((TemplateAttribute *)pNode)->GetParam()) == 0)
				TempRect = SliceHelper::BoundingNodeSize(pNode->FindParent());

			if (ExcludeSet.CompareTo(((TemplateAttribute *)pNode)->GetParam()) == 0)
				CleanNode = FALSE;

			pNewNode = SliceHelper::FindNextOfClass(pNode, pTop, CC_RUNTIME_CLASS(TemplateAttribute));

			Siblings = pNewNode && pNewNode->FindParent() == pNode->FindParent();

			pNode = pNewNode;

		} while (Siblings);

		if (CleanNode && !TempRect.IsEmpty())
			r = r.Union(TempRect);

		CleanNode = TRUE;
		TempRect.MakeEmpty();
	}

	return r;
}


/********************************************************************************************

>	BOOL SliceHelper::BarExistsOnLayer(const String_256 &BarName, const String_256 &Layer)

	Author:		Simon_Knight (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/6/00
	Returns:	TRUE if an element of the bar is found on the layer, FALSE if not found
	Purpose:	Fast scan for existance

********************************************************************************************/	
BOOL SliceHelper::BarExistsOnLayer(const String_256 &BarName, const String_256 &Layer)
{
	Node * pLayer = SliceHelper::FindLayerCalled(Layer);

	if (!pLayer) return FALSE;

	Node * pNode = pLayer;
	do
	{
		pNode = FindNextOfClass(pNode, pLayer, CC_RUNTIME_CLASS(TemplateAttribute));
		if (pNode && BarName.CompareTo(((TemplateAttribute *)pNode)->GetQuestion()) == 0)
			return TRUE;
	} while (pNode);

	return FALSE;
}

// find out if the rect r intersects with any bars (sjk 27/6/00)
void SliceHelper::BarNameInRect(DocRect r, String_256 *pBarName)
{
	PORTNOTETRACE("dialog","SliceHelper::BarNameInRect - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	NameGallery * pNameGallery = NameGallery::Instance();
	if (!pNameGallery)
		return;

	SGUsedNames* pNames = pNameGallery->GetUsedNames();
	if (!pNames)
		return;

	SGNameItem * pNameGalleryItem = (SGNameItem*) pNames->GetChild();

	while (pNameGalleryItem)
	{
		if (pNameGalleryItem->m_BarNumber >= 0 && r.IsIntersectedWith(pNameGalleryItem->GetSetBounds()))
		{
			pBarName->MakeMsg(_R(IDS_BARNAME), pNameGalleryItem->m_BarNumber + 1);
			return;
		}
		pNameGalleryItem = (SGNameItem *) pNameGalleryItem->GetNext();
	}
#endif
}

// add an action to show this Layer undoably (sjk 6/7/00)
void SliceHelper::ShowLayer(BOOL Visible, Layer * pLayer, Spread * pSpread, UndoableOperation * pUndoOp)
{
	PORTNOTETRACE("dialog","SliceHelper::ShowLayer - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	// dont put in the undo action if we have done nothing
	if (pLayer->IsVisible() == Visible)
		return;

	// Make layer visible/invisible undoably
	OpLayerGalParam Param(LAYER_VISIBLE, pSpread);
	Param.NewState = Visible;
	Param.pLayer = pLayer;
	LayerStateAction::Init(pUndoOp, pUndoOp->GetUndoActions(), Param);
#endif
}

// uses the name gallery data to work out if the selection
// consists of whole sets or not. We need to know if we have a partial set in the selection
// returns	0 = contains a parts of a set
//			1 = contains all of the set
//			2 = selection not part of a set at all
// sjk 7/7/00
INT32 SliceHelper::DoesSelectionOnlyContainCompleteSets()
{
	PORTNOTETRACE("dialog","SliceHelper::DoesSelectionOnlyContainCompleteSets - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	NameGallery * pNameGallery = NameGallery::Instance();
	if (!pNameGallery)
		return 2;

	pNameGallery->FastUpdateNamedSetSizes(); // Ensure timely info in NameGallery
	SGUsedNames* pNames = pNameGallery->GetUsedNames();
	if (!pNames)
		return 2;

	SGNameItem * pNameGalleryItem = (SGNameItem*) pNames->GetChild();

	INT32 ret = 2; // default as none of any set
	INT32 count = 0;

	while (pNameGalleryItem)
	{
		count = pNameGalleryItem->GetSelectedCount();
		if ( count > 0 && count < pNameGalleryItem->GetObjectCount())
		{
			return 0; // has part of this set
		}
		else if (count > 0)
			ret = 1; // has all of this set then

		pNameGalleryItem = (SGNameItem *) pNameGalleryItem->GetNext();
	}

	return ret;
#else
	return 2;
#endif
}

// Small function to scan the tree looking for a named object
// pNode -	the start of the scan
// pLid	-	the node that defines the branch to look down
// SetName - the name of the set we are looking for
// return TRUE if it is found, false otherwise
// sjk (18/9/00 or there abouts)
BOOL SliceHelper::SetUsedInTree(Node * pNode, const String_256 &SetName, Node * pLid)
{
	if (pLid == NULL)
		pLid = pNode;

	pNode = SliceHelper::FindNextNameNode(pNode, pLid);
	while (pNode)
	{
		if (SetName.CompareTo(((TemplateAttribute *)pNode)->GetParam()) == 0)
			return TRUE;
		pNode = SliceHelper::FindNextNameNode(pNode, pLid);
	}

	return FALSE;
}

// deletes the attribute and the properties in the node set sentinel
void SliceHelper::DeleteUnusedReferences(const String_256 &SetName, UndoableOperation * pOp)
{
	NodeSetSentinel * pNodeSetSentinel = Document::GetSelected()->GetSetSentinel();
	if (!pNodeSetSentinel)
		return;

	// remove all properties of the deleted node
	Node * pNode = pNodeSetSentinel->FindPropertyNode(SetName);
	if (pNode)
		pOp->DoHideNode(pNode, FALSE);

	// deleted bar members should be got rid of completely
	// remove the attribs from the sentinel
	pNode = pNodeSetSentinel->GetNameAttr(SetName);
	if (pNode)
		pOp->DoHideNode(pNode, FALSE);
}

// a controller has been added to the tree (such as a bevel controller)
// move any names from normal nodes onto the controller
BOOL SliceHelper::AddNamesToController (UndoableOperation * pOp, Node * pCtrlr)
{
	Node * pNode = pCtrlr->FindFirstChild();

	while (pNode)
	{
		if (!pNode->IsAnAttribute() && !pNode->IsNodeHidden())
		{
			Node * pAttr = NULL;

			do
			{
				// find any name attrs on this node
				pAttr = SliceHelper::FindNextNameNode(pNode, pNode);

				// does this already exist on the controller?
				Node * pExistingAttr = NULL;
				Node * pTemp = NULL;
				if (pAttr)
				{
					for (pTemp = pCtrlr->FindFirstChild();
						pTemp != NULL && pExistingAttr == NULL;
						pTemp = pTemp->FindNext() )
						{
							if (pTemp->IsAnObjectName())
							{
								if (((TemplateAttribute *) pTemp)->GetParam() == ((TemplateAttribute *) pAttr)->GetParam())
									pExistingAttr = pTemp;
							}
						}

					pTemp = SliceHelper::FindNextNameNode(pAttr, pNode);
				
					// didn't exist on controller so add it
					if (!pExistingAttr)
					{
						TemplateAttribute* pNewAttr = new TemplateAttribute(String_256(TEXT("ObjectName")),
															((TemplateAttribute *) pAttr)->GetQuestion(),
															((TemplateAttribute *) pAttr)->GetParam());

						// add the new attrib to the tree
						if (pNewAttr) 
						{
							pNewAttr->AttachNode(pCtrlr, FIRSTCHILD);
							// Create a hide node action to hide the node when we undo 
							HideNodeAction* UndoHideNodeAction;
							HideNodeAction::Init(pOp,                    
					  							 pOp->GetUndoActions(), //&UndoActions,
												 pNewAttr, 
												 TRUE, 		 // Include subtree size 
					  							 ( Action**)(&UndoHideNodeAction));
						}	
					}

					// did exist so just delete the duplicate
					pOp->DoHideNode(pAttr, FALSE);

					pAttr = pTemp;
				}

			}while (pAttr != NULL);
			
		}

		pNode = pNode->FindNext();
	}

	return TRUE;
}

// reverse of the above operation
// factorises the name attribs down from the controller
// controller MUST be deleted after this function is called
BOOL SliceHelper::RemoveNamesFromController (UndoableOperation * pOp, Node * pCtrlr)
{
	Node * pNode = pCtrlr->FindFirstChild();

	while (pNode)
	{
		// found a name attribute
		if (pNode->IsAnObjectName())
		{
			// add this to all proper nodes that are children of the controller
			Node * pTemp = NULL;
			for (pTemp = pCtrlr->FindFirstChild();
				pTemp != NULL;
				pTemp = pTemp->FindNext() )
				{
					// add it to all nodes excluding attributes, bevels and contours
					if (!pTemp->IsNodeHidden() && !pTemp->IsAnAttribute() && !pTemp->IsABevel() && !pTemp->IsAContour()
						&& !IS_A(pTemp, NodeBevelBegin))
					{
						// add the attr node to this child
						TemplateAttribute* pAttr = new TemplateAttribute(String_256(TEXT("ObjectName")),
															((TemplateAttribute *) pNode)->GetQuestion(),
															((TemplateAttribute *) pNode)->GetParam());

						// add the new attrib to the tree
						if (pAttr) 
						{
							pAttr->AttachNode(pTemp, FIRSTCHILD);
							// Create a hide node action to hide the node when we undo 
							HideNodeAction* UndoHideNodeAction;
							HideNodeAction::Init(pOp,                    
					  							 pOp->GetUndoActions(), //&UndoActions,
												 pAttr, 
												 TRUE, 		 // Include subtree size 
					  							 ( Action**)(&UndoHideNodeAction));

						}	
					}
				}

			// remove this from the controller
			pTemp = pNode;
			pNode = pNode->FindNext();
			pOp->DoHideNode(pTemp, FALSE);
		}
		else
			pNode = pNode->FindNext();
	}

	return TRUE;
}



/********************************************************************************************

>	BOOL SliceHelper::ModifySelectionToContainWholeButtonElements()

	Author:		Priestley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/11/00
	Returns:	TRUE all the time...

********************************************************************************************/	

BOOL SliceHelper::ModifySelectionToContainWholeButtonElements()
{
	PORTNOTETRACE("dialog","SliceHelper::ModifySelectionToContainWholeButtonElements - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	// Get a pointer to the NameGallery, and make sure it is up-to-date
	NameGallery *pNameGallery = NameGallery::Instance();
	pNameGallery->FastUpdateNamedSetSizes();

	//Get the first entry in the 'UsedNames' field of the NameGallery
	SGNameItem *pNameGalleryItem = (SGNameItem*) pNameGallery->GetUsedNames()->GetChild();

	// Build up a list of all the nodes used in the current selection. We will prune this list at every opportunity
	// in order to minimise the search space...

	SelRange* pSelRng = GetApplication()->FindSelection();
	RangeControl rangecontrol = pSelRng->GetRangeControlFlags();
	rangecontrol.PromoteToParent = TRUE;
	pSelRng->Range::SetRangeControl(rangecontrol);
	pSelRng->Update();

	List *pSelList = pSelRng->MakeListOfNodes(FALSE);
	bool unchanged = true; // Assume that no changes will be made...

	while (pNameGalleryItem)
	{
		INT32 CurrentSelection = pNameGalleryItem->GetSelectedCount();

		// If the named set is fully selected or not selected at all - there's no need to consider it further, otherwise...
		if ((CurrentSelection > 0) && (CurrentSelection != pNameGalleryItem->GetObjectCount()))
		{
			// Then we are interested in this named set - it is not fully selected, but it does appear
			// in our current selection (obviously) so we need to determine:
			// a) Is this named set a button bar element?
			// b) If so, are all elements on one layer selected?
			// c) What is the GDP of Bolivia, given that carrots are 50p/lb?
			// d) How is an oxbow lake formed? (see, you thought you'd never need to know that)
			String_256 StrName;
			StrName.Empty();
			pNameGalleryItem->GetNameText(&StrName);
			TemplateAttribute* pAttr;

			NodeListItem *pItem;
			Node *pNode;
			
			// For each node in the list, check if it is a member of the current named set...
			for (pItem = (NodeListItem *)pSelList->GetHead(); pItem; pItem = (NodeListItem *)pSelList->GetNext(pItem))
			{
				// Retrieve the current node from the NodeListItem...
				pNode = pItem->pNode;

				for (pAttr = (TemplateAttribute*)pNode->FindFirstAttr(Node::IsAnObjectName); pAttr; pAttr = (TemplateAttribute*)pAttr->FindNextAttr(Node::IsAnObjectName))
				{
					// This may seem a redundant check - but it's not! When we find that all of one layer has been selected
					// we select all of the layer and update the NameGallery - if we check within this 'for' loop, then
					// we no longer need to do computation when all of the named set is already selected...
					if (!pNameGalleryItem->IsAllSelected())
					{
						if ((StrName == pAttr->GetParam()) && (!(pAttr->GetQuestion().IsEmpty())))
						{
							// Then this node is a member of this named set AND it is a button bar element...
							// Now, we want to know if there are any other nodes which are NOT in the current selection
							// which form part of this named set and are on this node's layer. If not, select all of
							// this named set as that is what the user (probably) meant to do...
						
							Layer *pLayer = (Layer *)pNode->FindParent(CC_RUNTIME_CLASS(Layer));	// What layer is the current node on?
							bool AllLayerSelected = true;	// Assume all the layer is selected

							// - NOW THE (POTENTIALLY) SLOW BIT -
							// We need to go through every node in the current node's layer, checking if it is NOT SELECTED
							// AND if it is a member of the current name gallery item. As soon as both cases are true, then
							// we know we should not auto-select all of the named set and can exit quickly...
							Node * pTempNode = FindNextOfClass(pLayer, pLayer, CC_RUNTIME_CLASS(TemplateAttribute));
							while (pTempNode && AllLayerSelected)
							{
								// If this Node is part of a bar and has the same name as our name gallery item, then we need to consider it further...
								if (((TemplateAttribute *)pTempNode)->GetQuestion() && ((TemplateAttribute *)pTempNode)->GetParam() == StrName)
								{
									// Check if we are dealing with a controller node...
									if (IS_A(pTempNode->FindParent(), NodeShadowController) || IS_A(pTempNode->FindParent(), NodeContourController) || IS_A(pTempNode->FindParent(),
										NodeBevelController))
									{
										// If so, check its children...
										TRACEUSER( "Matt", _T("\nController Node found - looking at its children"));
										Node * pChild = pTempNode->FindParent()->FindFirstChild();
										while (pChild && AllLayerSelected)
										{
											if (!pChild->IsAnAttribute() && !pChild->IsNodeHidden() && !IS_A(pChild, NodeShadow) && !IS_A(pChild, NodeContour) && !IS_A(pChild,
												NodeBevel) && !IS_A(pChild, NodeBevelBegin) && !IS_A(pChild, NodeContourController) && !IS_A(pChild, NodeBevelController))
											{
												// Then consider this child... Is it unselected - if so then we have found a matching node which is unselected when it should have been!
												if (!pChild->IsSelected())
												{
													AllLayerSelected = false;
												}
											}
											pChild = pChild->FindNext();
										}
									}
									else
									{
										if (!pTempNode->FindParent()->IsSelected())
										{
											// Then we should check if this node's parent is selected or not...
											AllLayerSelected = false;
										}
									}
								}

								pTempNode = FindNextOfClass(pTempNode, pLayer, CC_RUNTIME_CLASS(TemplateAttribute));
							}


							if (AllLayerSelected)
							{
								// Save the original selection (if it hasn't already been saved)
								if (unchanged)
								{
									SliceHelper::SaveSelection();
									unchanged = false;
								}

								// Select ALL LAYERS of this named set...
								TRACEUSER( "Matt", _T("ALL of %s on layer selected\n"),StrName);

								SelectScan scanner(pNameGalleryItem, SelectScan::SELECT);
								scanner.Scan();
								pNameGallery->FastUpdateNamedSetSizes();
							}

							// END of slow bit...
						}
					}
				}
			}
		}

		// Get the next 'UsedName' from the NameGallery
		pNameGalleryItem = (SGNameItem *) pNameGalleryItem->GetNext();
	}

	// Karim 04/12/2000 - unset PromoteToParent, so we don't corrupt the app's sel-range.
	rangecontrol.PromoteToParent = FALSE;
	pSelRng->Range::SetRangeControl(rangecontrol);
	pSelRng->Update();

	// Tidy up after myself... (Now there's a first - I won't make a habit of it)
	pSelList->DeleteAll();
	delete pSelList;
	pSelList = 0;

	if (!unchanged)
	{
		// Then our procedure has altered the selection, so we'd better update it!
		GetApplication()->FindSelection()->Update();		
	}
#endif
	return TRUE;
}




/********************************************************************************************

>	static void SliceHelper::SaveSelection()

	Author:		Priestley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/11/2000
	Purpose:	Save the current selection to m_pSelNodeList
	
********************************************************************************************/

void SliceHelper::SaveSelection()
{
	if (m_pSelNodeList)
	{
		m_pSelNodeList->DeleteAll();
		delete m_pSelNodeList;
		m_pSelNodeList = 0;
	}

	// Find the current selection and store it as a list of NodeListItems for later use...
	SelRange* pSelRng = GetApplication()->FindSelection();
	m_pSelNodeList = pSelRng->MakeListOfNodes(FALSE);
}




/********************************************************************************************

>	static void OpMenuExport::RestoreSelection()

	Author:		Priestley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/11/2000
	Purpose:	Restores the current selection to that given in m_pSelNodeList
	
********************************************************************************************/
void SliceHelper::RestoreSelection()
{
	if (m_pSelNodeList)
	{
		SelRange* pSelRng = GetApplication()->FindSelection();
		NodeRenderableInk::DeselectAll(FALSE, FALSE);

		for (NodeListItem* pItem = (NodeListItem*) m_pSelNodeList->GetHead(); pItem != 0; pItem = (NodeListItem*) m_pSelNodeList->GetNext(pItem))
		{
			if (pItem->pNode != 0)
			{
				pItem->pNode->SetSelected(TRUE);
			}
		}

		m_pSelNodeList->DeleteAll();
		delete m_pSelNodeList;
		m_pSelNodeList = 0;

		RangeControl rangecontrol = pSelRng->GetRangeControlFlags();
		rangecontrol.PromoteToParent = FALSE;
		pSelRng->Range::SetRangeControl(rangecontrol);
		pSelRng->Update();
	}
}


// Matt 18/12/2000
// Quick routine to ensure that the NodeSetSentinel is correct - if it is not then this routine will
// attempt to correct it as much as possible...
void SliceHelper::ValidateNodeSetSentinel()
{
	// Get the NodeSetSentinel for this document
	NodeSetSentinel * pNodeSetSentinel = Document::GetSelected()->GetSetSentinel();

	// Ensure that each NodeSetProperty which is NOT hidden has a corresponding UNHIDDEN TemplateAttribute...
	NodeSetProperty * pNodeSetProperty = (NodeSetProperty *) pNodeSetSentinel->FindFirstChild(CC_RUNTIME_CLASS(NodeSetProperty));

	while (pNodeSetProperty)
	{
		if (!pNodeSetProperty->IsNodeHidden())
		{
			BOOL ok = TRUE;
			TemplateAttribute * pTemplateAttribute = (TemplateAttribute *) pNodeSetSentinel->FindFirstChild(CC_RUNTIME_CLASS(TemplateAttribute));
			while (ok && pTemplateAttribute)
			{
				String_256 templateattributename = pTemplateAttribute->GetParam();
				String_256 nodesetpropertyname = pNodeSetProperty->GetName();
				// If the templateattribute is both unhidden and is for the current nodesetproperty then flag it!
				if (!pTemplateAttribute->IsNodeHidden() && (templateattributename == nodesetpropertyname))
				{
					ok = FALSE;
				}

				pTemplateAttribute = (TemplateAttribute *) pTemplateAttribute->FindNext(CC_RUNTIME_CLASS(TemplateAttribute));
			}

			// At this point, if ok is TRUE then we have scanned all templateattributes and not found a matching one for our nodesetproperty - so make one !!!
			if (ok)
			{
				TRACEUSER( "Matt", _T("\nNodeSetProperty  "));
				TRACEUSER("Matt", pNodeSetProperty->GetName());
				TRACEUSER( "Matt", _T(" doesn't have a matching TemplateAttribute!!"));

				String_256 ButtonName = pNodeSetProperty->GetName();
				String_256 BarName = "";

				// Right, now we know all we need to make a new TemplateAttribute to repair the NodeSetSentinel except which bar it belongs to - let's find out...
				// If there is no MouseOff layer then we don't care...
				String_256 mouseoffstring(_R(IDS_ROLLOVER_DEFAULT));

				Spread * tempspread = Document::GetSelectedSpread();

				TemplateAttribute * temptemplate = (TemplateAttribute *) SliceHelper::FindNextNameNode(tempspread, tempspread);
				BOOL templatefound = FALSE;
				while (temptemplate && !templatefound)
				{
					if (temptemplate->GetParam() == ButtonName)
					{
						BarName = temptemplate->GetQuestion();
						templatefound = TRUE;
					}

					temptemplate = (TemplateAttribute *) SliceHelper::FindNextNameNode(temptemplate, tempspread);
				}

				// Make a new TemplateAttribute and add it into the tree...
				TemplateAttribute* pAttr = new TemplateAttribute(String_256(TEXT("ObjectName")), BarName, ButtonName);
				if (pAttr) 
				{
					pAttr->AttachNode(pNodeSetProperty, NEXT);
				}

			}
		}

		pNodeSetProperty = (NodeSetProperty *) pNodeSetProperty->FindNext(CC_RUNTIME_CLASS(NodeSetProperty));
	}
}


/********************************************************************************************
>	static void SliceHelper::BuildListOfNodesInButton(List * pList, Layer * pLayer, const StringBase & ButtonName)

	Author:		Priestley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/12/2000
	Params:		pList	-	List that gets filled with pointers to all the nodes
					pLayer	-	The layer we are interested in
					ButtonName	-	The button we are looking for
	Returns:	-
	Purpose:	Scans all of pLayer, building a list of all nodes which are part of ButonName - if a NodeShadowController
						is encountered, we add its children instead in order to get the correct bounding box info...
********************************************************************************************/
void SliceHelper::BuildListOfNodesInButton(List * pList, Layer * pLayer, const StringBase & ButtonName)
{
	ERROR3IF(!pLayer || !ButtonName || !pList, "Please don't call SliceHelper::BuildListOfNodesInButton with null params!");

	// Starting at pLayer, look for all TemplateAttributes with the correct name attached...
	Node * pNode = FindNextOfClass(pLayer, pLayer, CC_RUNTIME_CLASS(TemplateAttribute));
	while (pNode)
	{
		// If this Node is part of a bar AND has the correct name, then add it to the list...
		if (((TemplateAttribute *)pNode)->GetQuestion() && (((TemplateAttribute *)pNode)->GetParam() == ButtonName))
		{
			// Add this node to the list unless it is a nodeshadowcontroller - if it is, add all of its children individually (except the nodeshadow child)
			if (IS_A(pNode->FindParent(), NodeShadowController))
			{
				TRACEUSER( "Matt", _T("\nNodeShadowController found in BuildListOfNodesInButton - replacing list entries with its children"));
				Node * pChild = pNode->FindParent()->FindFirstChild();
				while (pChild)
				{
					if (!pChild->IsAnAttribute() && !pChild->IsNodeHidden() && !IS_A(pChild, NodeShadow))
					{
						// Then add this child to the list
						NodeListItem * pItem = new NodeListItem(pChild);
						pList->AddTail(pItem);
					}
					pChild = pChild->FindNext();
				}
			}
			else
			{
				// Then we were safe to add it to the list...
				NodeListItem * pItem = new NodeListItem(pNode->FindParent());
				pList->AddTail(pItem);
			}
		}

		pNode = FindNextOfClass(pNode, pLayer, CC_RUNTIME_CLASS(TemplateAttribute));
	}
}


/********************************************************************************************
>	static void SliceHelper::EnsureTriggerInfo()

	Author:		Priestley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/01/2001
	Purpose:	Scans the NameGallery ensuring that the triggers are in the right places...
********************************************************************************************/
void SliceHelper::EnsureTriggerInfo()
{
	PORTNOTETRACE("dialog","SliceHelper::EnsureTriggerInfo - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	NameGallery *pNameGallery = NameGallery::Instance();
	SGUsedNames* pNames = pNameGallery->GetUsedNames();

	if (!pNames) { return; }

	SGNameItem* pNameGalleryItem = (SGNameItem*) pNames->GetChild();

	// First we need to loop around and set all the triggers to false...
	while (pNameGalleryItem)
	{
		pNameGalleryItem->m_IsATrigger = FALSE;
		pNameGalleryItem = (SGNameItem *) pNameGalleryItem->GetNext();
	}

	// Now go back to the beginning and work out which named sets have triggers - mark these triggers as being such...
	pNameGalleryItem = (SGNameItem*) pNames->GetChild();
	while (pNameGalleryItem)
	{
		NodeSetProperty* pPropNode = NULL;
		NamedStretchProp* pProp = NULL;

		// Check if the named set has 'Stretch' property information...
		pPropNode = pNameGalleryItem->GetPropertyNode();
		if (pPropNode)
		{
			pProp = (NamedStretchProp*) pPropNode->GetProperty(NamedStretchProp::nIndex);
		}

		// If so, and its list of triggers is not empty, loop around ensuring that all of the triggers of this set have their 'is trigger' flag set...
		if (pProp && !pProp->GetTriggers().empty())
		{
			for (std::list<TriggerSet>::iterator pt = pProp->GetTriggers().begin(); pt != pProp->GetTriggers().end(); pt++)
			{
				SGNameItem* pTempTriggerSet = SliceHelper::LookupNameGalleryItem(pt->m_strSet);

				if (pTempTriggerSet)
				{
					pTempTriggerSet->m_IsATrigger = TRUE;
				}
			}
		}

		// Get the next item and try again...
		pNameGalleryItem = (SGNameItem *) pNameGalleryItem->GetNext();
	}
#endif
}
