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
// Implementation of Effects Stack.
//

#include "camtypes.h"			// pre-compiled header
#include "effects_stack.h"

#include "nodeliveeffect.h"
#include "opliveeffects.h"

// dynamic class creation stuff.
CC_IMPLEMENT_DYNAMIC(PPStackLevel, ListItem);
CC_IMPLEMENT_DYNAMIC(EffectsStack, List);

DECLARE_SOURCE("$Revision$");

#define new CAM_DEBUG_NEW




///////////////////////////////////////////////////////////////////////////////////////////////
// The EffectsStack class															    	 //
///////////////////////////////////////////////////////////////////////////////////////////////

/********************************************************************************************

>	EffectsStack::EffectsStack()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/11/2004
	Purpose:	Constructor.
	Errors:		
	See also:	

********************************************************************************************/
EffectsStack::EffectsStack()
{
	bConsistent = FALSE;
	m_pNewLevelRange = NULL;
}



/********************************************************************************************

>	EffectsStack::~EffectsStack()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/11/2004
	Purpose:	Destructor.
	Errors:		
	See also:	

********************************************************************************************/
EffectsStack::~EffectsStack()
{
	// We own m_pNewLevelRange so we must delete it...
	if (m_pNewLevelRange)
	{
		delete m_pNewLevelRange;
		m_pNewLevelRange = NULL;
	}

	Clear();
}



/********************************************************************************************

>	EffectsStack* EffectsStack::GetEffectsStackFromSelection(Range* pRange = NULL, BOOL bEscapeDerived = TRUE, BOOL bIncludeLocked = FALSE)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/10/2004
	Inputs:		-
	Outputs:	-
	Returns:	pStack - pointer to ListRange representing stack applied consistently to selection
	Purpose:	Get a list of LE nodes that are common to all selected nodes
				Only returns a list of ALL the selected nodes have identical stacks
				above them...

********************************************************************************************/

EffectsStack* EffectsStack::GetEffectsStackFromSelection(Range* pRange, BOOL bEscapeDerived, BOOL bIncludeLocked)
{
	// Loop through selection ensuring that all nodes are LiveEffects
	// and that they all share the same edit list
	if (pRange==NULL)
		pRange = GetApplication()->FindSelection();
	ENSURE(pRange, "!");
	ENSURE(pRange->GetRangeControlFlags().PromoteToParent==FALSE, "GetEffectsStackFromSel given a bad range");

	EffectsStack* pPPStack = new EffectsStack();		// New stack is marked as being "inconsistent"
	BOOL bStartNewStack = TRUE;
	ListRange* pLocalStack = NULL;
	BOOL bLiveEffectsAreApplied = FALSE;

	// Ensure that pEditedLE has a useful value...
	// Loop through the current selection and search the parents of each selected node for LiveEffects...
	Node* pNode = pRange->FindFirst();
	Node* pLastNode = NULL;
	while (pNode!=NULL)
	{
		if (pNode->IsAnObject())
		{
			// Don't try to add the same stack to the system more than once
			// (For instance NodeShadows and their originals are siblings and can both be selected
			//  below a NodeShadowController)
			if (!NodesSharePPStack(pNode, pLastNode, TRUE, bIncludeLocked))
			{
				// Get the local stack
				pLocalStack = GetEffectsStackFromNode(pNode, TRUE, bEscapeDerived, bIncludeLocked);		// If this node is in the middle of a stack, find the whole stack

				if (pLocalStack)
				{
					bLiveEffectsAreApplied = TRUE;

					if (bStartNewStack)
					{
						if (pPPStack->IsEmpty())
						{
							pPPStack->Initialise(pLocalStack);
						}
						else
						{
							BOOL bOK = pPPStack->Intersect(pLocalStack);
							if (!bOK)
							{
								delete pLocalStack;
								pPPStack->Clear();						// Just to make sure!
								return pPPStack;
							}
						}
					}

					delete pLocalStack;
				}
				else
				{
					// A node in the selection doesn't have any LiveEffects applied but we can't stop
					// because we have to keep scanning to see whether ANY nodes have LiveEffects
					// Clear the effect stack out because this node has none.
					pPPStack->Clear();
					bStartNewStack = FALSE;

					// Could test for lengthy scanning here and if it takes too long return <UNKNOWN>
					// if (taking too long) code = SELTYPE_UNKNOWN;
				}
			}
			pLastNode = pNode;
		}

		pNode = pRange->FindNext(pNode);
	}

	if (pPPStack->IsEmpty() && bLiveEffectsAreApplied)
		pPPStack->bConsistent = FALSE;
	else
		pPPStack->bConsistent = TRUE;

	return pPPStack;
}




/********************************************************************************************

>	BOOL EffectsStack::Initialise(ListRange* pRange)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/10/2004
	Inputs:		
	Outputs:	
	Returns:	
	Purpose:
	Errors:		
	See also:	

********************************************************************************************/
BOOL EffectsStack::Initialise(ListRange* pRange)
{
	ENSURE(pRange, "EffectsStack::Initialise must be given a range");

	Node* pNode = pRange->FindFirst();
	while (pNode)
	{
		ENSURE(pNode->IsEffect(), "PostProcessor list contains non-PostProcessor in EffectsStack::Initialise");
		NodeEffect* pLE = (NodeEffect*)pNode;

		PPStackLevel* pNewItem = new PPStackLevel();

		pNewItem->pPPNode = pLE;

		pNewItem->listSelNodes.AddNode(pLE);
		pNewItem->strPostProID = pLE->GetPostProcessorID();

		AddTail(pNewItem);

		pNode = pRange->FindNext(pNode);
	}

	return TRUE;
}




/********************************************************************************************

>	BOOL EffectsStack::Intersect(ListRange* pRange)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/10/2004
	Inputs:		pRange - A stack of LiveEffects above a node in the selection
	Outputs:	
	Returns:	
	Purpose:	Find whether the LE nodes in the presented range match those already in this
				LE stack. Remove any mismatches from the PPStack, add new nodes to the
				stack level lists.
	Errors:		
	See also:	

********************************************************************************************/
BOOL EffectsStack::Intersect(ListRange* pRange)
{
	ENSURE(pRange, "EffectsStack::Intersect must be given a range");

	Node* pNode = pRange->FindFirst();
	while (pNode)
	{
		ENSURE(pNode->IsEffect(), "PostProcessor list contains non-PostProcessor in EffectsStack::Intersect");
		NodeEffect* pLE = (NodeEffect*)pNode;

		PPStackLevel* pLastMatchItem = NULL;
		PPStackLevel* pFirstMatchItem = (PPStackLevel*)GetHead();
		NodeEffect* pStackSample;
		while (pFirstMatchItem!=NULL && pLastMatchItem==NULL)
		{
			pStackSample = pFirstMatchItem->pPPNode;

			pLastMatchItem = FindMatch(pFirstMatchItem, pRange, pLE);
			if (pLastMatchItem)
				break;												// Stop as soon as we find a match!

			pFirstMatchItem = (PPStackLevel*)GetNext(pFirstMatchItem);
		}

		if (pLastMatchItem)
		{
			// ------------------------------------------------------------
			// We have a match so remove the stack levels that didn't match
			PPStackLevel* pItem = (PPStackLevel*)GetHead();
			while (pItem != pFirstMatchItem)
			{
				PPStackLevel* pNextItem = (PPStackLevel*)GetNext(pItem);

				RemoveItem(pItem);

				pItem = pNextItem;
			}

			// ------------------------------------------------------------
			// Add the nodes that do match to the ListRanges in the levels
			do
			{
				pItem->listSelNodes.AddNode(pLE);

				pLE = (NodeEffect*)pRange->FindNext(pLE);
				pItem = (PPStackLevel*)GetNext(pItem);
			}
			while (pItem!=NULL && pItem!=pLastMatchItem && pLE!=NULL);

			if (pItem!=NULL && pItem==pLastMatchItem && pLE!=NULL)
			{
				pItem->listSelNodes.AddNode(pLE);
			}

			// ------------------------------------------------------------
			// Remove further levels that didn't match
			if (pItem)
			{
				pItem = (PPStackLevel*)GetNext(pItem);
				while (pItem != NULL)
				{
					PPStackLevel* pNextItem = (PPStackLevel*)GetNext(pItem);

					RemoveItem(pItem);

					pItem = pNextItem;
				}
			}

			return TRUE;
		}

		pNode = pRange->FindNext(pNode);
	}

	Clear();
	return FALSE;
}




/********************************************************************************************

>	BOOL EffectsStack::FindBestProcessor(String_256* pstrEffectID, INT32* piStackPos)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/10/2004
	Inputs:		
	Outputs:	
	Returns:	TRUE if an effect was found
				FALSE otherwise
	Purpose:	Find the most appropriate effect in this effect stack mathcing the given
				name and stack position.
	Errors:		
	See also:	

********************************************************************************************/
BOOL EffectsStack::FindBestProcessor(String_256* pstrEffectID, INT32* piStackPos)
{
	INT32 pos = *piStackPos;
	PPStackLevel* pLevel = NULL;

	// Search up from the last known stack position for an effect of the same name
	do
	{
		pLevel = (PPStackLevel*)FindItem(pos);

		if (pLevel && pLevel->strPostProID==*pstrEffectID)
		{
			*piStackPos = pos;
			return TRUE;
		}

		pos++;
	}
	while (pLevel);

	// Search down from the last known stack position for an effect of the same name
	pos = *piStackPos-1;
	do
	{
		pLevel = (PPStackLevel*)FindItem(pos);

		if (pLevel && pLevel->strPostProID==*pstrEffectID)
		{
			*piStackPos = pos;
			return TRUE;
		}

		pos--;
	}
	while (pLevel);

	return FALSE;
}




/********************************************************************************************

>	BOOL EffectsStack::GetLevelInfo(String_256* pstrEffectID, INT32* piStackPos)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/11/2004
	Inputs:		
	Outputs:	
	Returns:	TRUE if an effect was found
				FALSE otherwise
	Purpose:	Find the most appropriate effect in this effect stack mathcing the given
				name and stack position.
	Errors:		
	See also:	

********************************************************************************************/
BOOL EffectsStack::GetLevelInfo(String_256* pstrEffectID, INT32* piStackPos)
{
	PPStackLevel* pLevel = NULL;

	if (*piStackPos>=STACKPOS_TOP)
		pLevel = (PPStackLevel*)GetTail();
	else
		pLevel = (PPStackLevel*)FindItem(*piStackPos);

	if (pLevel)
	{
		*piStackPos = (INT32) this->FindPosition(pLevel);
		*pstrEffectID = pLevel->strPostProID;
		return TRUE;
	}

	return FALSE;
}




/********************************************************************************************

>	void EffectsStack::Clear()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/10/2004
	Inputs:		
	Outputs:	
	Returns:	
	Purpose:	
	Errors:		
	See also:	

********************************************************************************************/
void EffectsStack::Clear()
{
	// No match so clear out the Stack and return
	while (GetCount()!=0)
	{
		PPStackLevel* pItem = (PPStackLevel*)RemoveTail();
		delete pItem;
	}
}




/********************************************************************************************

>	PPStackLevel* EffectsStack::FindMatch(PPStackLevel* pFirstItem, ListRange* pRange, NodeEffect* pPP)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/10/2004
	Inputs:		pRange - A stack of LiveEffects above a node in the selection
	Outputs:	
	Returns:	
	Purpose:	Find whether the LE nodes in the presented range match those already in this
				LE stack. Remove any mismatches from the PPStack, add new nodes to the
				stack level lists.
	Errors:		
	See also:	

********************************************************************************************/
PPStackLevel* EffectsStack::FindMatch(PPStackLevel* pFirstItem, ListRange* pRange, NodeEffect* pPP)
{
	PPStackLevel* pItem = pFirstItem;
	PPStackLevel* pLastMatchItem = NULL;
	BOOL bMatch = TRUE;
//	HRESULT hr;

	do
	{
		bMatch = pPP->CompareState(pItem->pPPNode);			// Polymorphic compare for PostProcessors
		if (bMatch)
			pLastMatchItem = pItem;
// Possible future change to detect inconsistent levels. But this would require changes elsewhere
//		bMatch = (pPP->GetRuntimeClass() == pItem->pPPNode->GetRuntimeClass());
//		if (pItem->bConsistent)
//		{
//			pItem->bConsistent = pPP->CompareState(pItem->pPPNode);
//		}

		pPP = (NodeEffect*)pRange->FindNext(pPP);
		pItem = (PPStackLevel*)GetNext(pItem);
	}
	while (bMatch && pItem!=NULL && pPP);

	return pLastMatchItem;
}




/********************************************************************************************

>	ListRange* EffectsStack::GetEffectsStackFromNode(Node* pNode, BOOL bNodeMaybeMidStack = FALSE, BOOL bEscapeDerived = TRUE, bIncludeLocked = FALSE)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/10/2004
	Inputs:		pNode - pointer to node in tree which may have a LiveEffect stack above it
	Outputs:	-
	Returns:	pPPStack	- NULL if <None>
							- Empty if <Many>
							- Populated if consistent stack (caller becomes owner of this list!)
	Purpose:	Get a list of LE nodes that are common to all selected nodes
				Only returns a list of ALL the selected nodes have identical stacks
				above them...
				Does nothing about setting current stack pos or current effect name
	SeeAlso:	LiveEffectsTool::GetLEListFromSelAndStack

********************************************************************************************/

ListRange* EffectsStack::GetEffectsStackFromNode(Node* pNode, BOOL bNodeMaybeMidStack, BOOL bEscapeDerived, BOOL bIncludeLocked)
{
	// First, test whether node CAN be in a PPStack or stands aside from it
	// (E.g. shadows)
	//
	if (!bEscapeDerived && pNode->NeedsParent(pNode->FindParent()))
		return NULL;

	ListRange* pStack = new ListRange();
	Node* pUpNode = pNode;

	// If the node could be in the middle of a stack then we need to find the base
	// of the stack before we start our scan
	// (E.g. the node could be a selected Shadow node somewhere high above the
	//  object it applies to)
	if (bNodeMaybeMidStack)
	{
		pUpNode = FindStackBaseFrom(pUpNode, bIncludeLocked);
	}

	// Special case handling for old legacy effects until they become true
	// PostProcessors...
	// Search for Contour, Bevel, ClipView controllers above selected node
	// and if found, make them the base for PostProcessor scanning
	pUpNode = EscapeOldControllers(pUpNode);

	// Now go up the post processor stack, adding every one we find to the list
	do
	{
		if (pUpNode->IsEffect())
			pStack->AddNode(pUpNode);

		pUpNode = pUpNode->FindParent();
	}
	while (pUpNode && pUpNode->IsEffect());

	// Best to get rid of the list here if it's empty (don't trust caller)
	if (pStack->IsEmpty())
	{
		delete pStack;
		pStack = NULL;
	}

	return pStack;
}




/********************************************************************************************

>	BOOL EffectsStack::NodesSharePPStack(Node* pNode, Node* pLastNode, BOOL bNodeMaybeMidStack = FALSE, BOOL bIncludeLocked = FALSE)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/11/2004
	Inputs:		pNode - pointer to node in tree which may have a LiveEffect stack above it
				pLastNode - pointer to another node whicyh may have a LiveEffect stack
	Outputs:	-
	Returns:	TRUE if both nodes have the same stack above them
				FALSE if the nodes have different stacks
	Purpose:	Find out whether two sibling nodes in the tree share the same stack
				(So that we can avoid adding the same node stack to the combined stack
				 more than once)
	SeeAlso:	LiveEffectsTool::GetLEListFromSelAndStack

********************************************************************************************/

BOOL EffectsStack::NodesSharePPStack(Node* pNode, Node* pLastNode, BOOL bNodeMaybeMidStack, BOOL bIncludeLocked)
{
	if (pNode==NULL || pLastNode==NULL)
		return FALSE;

	if (bNodeMaybeMidStack)
	{
		// Find the true stack base for both these
		pNode = FindStackBaseFrom(pNode, bIncludeLocked);
		pLastNode = FindStackBaseFrom(pLastNode, bIncludeLocked);
	}

	// Special case handling for old legacy effects until they become true
	// PostProcessors...
	// Search for Contour, Bevel, ClipView controllers above selected node
	// and if found, make them the base for PostProcessor scanning
	pNode = EscapeOldControllers(pNode);
	pLastNode = EscapeOldControllers(pLastNode);

	if (!bIncludeLocked)
	{
		if (pNode->IsEffect() && ((NodeEffect*)pNode)->IsLockedEffect())
			return FALSE;

		if (pLastNode->IsEffect() && ((NodeEffect*)pLastNode)->IsLockedEffect())
			return FALSE;
	}

	if (pNode->IsEffect() && !((NodeEffect*)pNode)->CanBeUnlocked())
		return FALSE;

	if (pLastNode->IsEffect() && !((NodeEffect*)pLastNode)->CanBeUnlocked())
		return FALSE;

	Node* pParent = pNode->FindParent();
	if (pParent==NULL)
		return FALSE;

	return (pParent == pLastNode->FindParent() && pParent->IsEffect());
}




/********************************************************************************************

>	Node* EffectsStack::FindStackBaseFrom(Node* pNode, BOOL bIncludeLocked)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/11/2004
	Inputs:		pNode - pointer to node in tree which may have a LiveEffect stack above it
						and below it!
	Outputs:	-
	Returns:	Pointer to node at the base of the stack containing the input node
	Purpose:	From the specified node, find the base of the local stack containing that
				node

********************************************************************************************/

Node* EffectsStack::FindStackBaseFrom(Node* pNode, BOOL bIncludeLocked)
{
	Node* pStackNode = pNode;

	if (!pStackNode->IsEffect())
	{
		if (pStackNode->GetParentController())
			pStackNode = pStackNode->GetParentController();
	}

	while (pStackNode && pStackNode->IsEffect())
	{
		// Stop immediately if we find a destructive post pro - the user cannot be
		// allowed to see or manipulate anything underneath one of those...
		if (!bIncludeLocked && ((NodeEffect*)pStackNode)->IsLockedEffect())
			break;

		if (!((NodeEffect*)pStackNode)->CanBeUnlocked())
			break;

		pStackNode = ((NodeEffect*)pStackNode)->GetInkNodeFromController();
	}

	return pStackNode;
}

		
		
		
/********************************************************************************************

>	Node* EffectsStack::EscapeOldControllers(Node* pUserNode)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/11/2004
	Inputs:		pNode - pointer to node in tree which may have a LiveEffect stack above it
	Outputs:	-
	Returns:	Node* - pointer to node above presented node which is an old style
				non-postprocessor controller node
	Purpose:	Find the node which PostProcessors should be wrapped around, taking
				old style Controller nodes into account.

********************************************************************************************/
Node* EffectsStack::EscapeOldControllers(Node* pUserNode)
{
	Node* pTestNode = pUserNode->FindParent();
	while (pTestNode && pTestNode->IsController() && !pTestNode->IsEffect())
	{
		pUserNode = pTestNode;
		pTestNode = pTestNode->FindParent();
	}

	return pUserNode;
}

	
	
	
/********************************************************************************************

>	BOOL EffectsStack::LockableNonEmpty()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/10/2004
	Inputs:		pNode - pointer to node in tree which may have a LiveEffect stack above it
	Outputs:	-
	Returns:	TRUE if stack has some lockable effects layers
				FALSE if no levels or any level contains an unlockable effect
	Purpose:	Determine whether stack is empty or has destructive effects in it
	SeeAlso:	LiveEffectsTool::GetLEListFromSelAndStack

********************************************************************************************/

BOOL EffectsStack::LockableNonEmpty()
{
	if (IsEmpty())
		return FALSE;

	PPStackLevel* pItem = (PPStackLevel*)GetHead();
	NodeEffect* pLENode;
	while (pItem!=NULL)
	{
		pLENode = pItem->pPPNode;
		ENSURE(pLENode, "PPStackLevel node pointer is NULL in LockableNonEmpty");

		if (!pLENode->CanBeUnlocked())
			return FALSE;

		pItem = (PPStackLevel*)GetNext(pItem);
	}

	return TRUE;
}




/********************************************************************************************

>	ListRange* EffectsStack::GetLevelRange(INT32* piStackPos, BOOL bEscapeOldControllers = TRUE)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/09/2004
	Inputs:		iStackPos - position in LiveEffect stack to find nodes
							0 means first stack pos above selection
	Outputs:	
	Returns:	A pointer to a list range owned by the EffectsStack - i.e. you don't have to 
				delete it when you're finished.
	Purpose:	
	Errors:		
	See also:	

********************************************************************************************/
ListRange* EffectsStack::GetLevelRange(INT32* piStackPos, BOOL bEscapeOldControllers)
{
	ENSURE(*piStackPos>=0, "Invalid stack pos in GetLevelRange!");

	// If the stack is inconsistent then we need to make a range of nodes that arch
	// over the selection and any stacked PostProcessors that might be on them
	// In this case we expect our caller to not have specified a valid stack position
	if (!bConsistent || IsEmpty() || *piStackPos>=STACKPOS_TOP)
	{
		ENSURE(*piStackPos>=STACKPOS_TOP, "Incorrect attempt to insert into an inconsistent PostPro stack");

		// By definition, if someone had previously requested a new level range
		// during the lifetime of this effects stack then the same range must
		// still apply (otherwise the EffectsStack would have been destroyed).
		// So just return the range we already found...
		if (m_pNewLevelRange)
		{
			return m_pNewLevelRange;
//			delete m_pNewLevelRange;
//			m_pNewLevelRange = NULL;
		}

		m_pNewLevelRange = GetNewLevelRange(NULL, bEscapeOldControllers);			// We own this listrange, so we can delete it later

		// Return new stack pos; correct number for consistent stack, INVALID for inconsistent stack
		if (bConsistent)
			*piStackPos = ((INT32)GetCount())-1;
		else
			*piStackPos = STACKPOS_INVALID;

		return m_pNewLevelRange;
	}

	// This is the typical case: Adding/Inserting to a consistent stack
	if (*piStackPos>=(INT32)GetCount())
		*piStackPos = ((INT32)GetCount())-1;

	PPStackLevel* pLevel = (PPStackLevel*)FindItem(*piStackPos);
	ENSURE(pLevel, "Argh something's got out of line!");
	ListRange* pLERange = &pLevel->listSelNodes;

	return pLERange;										// The EffectsStack owns this listrange
}




/********************************************************************************************

>	void EffectsStack::AttrsHaveChanged()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	01/10/2005
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Tell any stored ranges that any cahced attr data trhey have is now out
				of date

********************************************************************************************/

void EffectsStack::AttrsHaveChanged()
{
	if (m_pNewLevelRange)
	{
		m_pNewLevelRange->AttrsHaveChanged();
	}
}




/********************************************************************************************

>	static ListRange* EffectsStack::GetNewLevelRange(Range* pRange, BOOL bEscapeOldControllers = TRUE)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/10/2004
	Inputs:		-
	Outputs:	
	Returns:	A pointer to a list range owned by you!
	Purpose:	Get a range of nodes forming a surface above the selection and above
				any live effects applied to the selection
	Errors:		
	See also:	

********************************************************************************************/
ListRange* EffectsStack::GetNewLevelRange(Range* pRange, BOOL bEscapeOldControllers)
{
	// Loop through selection ensuring that all nodes are LiveEffects
	// and that they all share the same edit list
	if (pRange==NULL)
		pRange = GetApplication()->FindSelection();
	ENSURE(pRange, "No selection in GetNewLevelRange!");

	// We want to apply the LiveEffect to the entire selection, however inconsistent it is
	//
	// Loop through the current selection and search the parents of each selected node for LiveEffects...
	ListRange* pLERange = new ListRange();
	Node* pNode = pRange->FindFirst();
	INT32 pos = 0;
	while (pNode!=NULL)
	{
		NodeEffect* pFirstEditedLE = NULL;
		Node* pBaseNode = pNode;

		// Only traverse the effect stack if the selected node is 'in' the effect stack
		// (it could be offset, e.g. NodeShadow)
		if (!pNode->NeedsParent(pNode->FindParent()))
		{
			if (bEscapeOldControllers)
				pBaseNode = EscapeOldControllers(pNode);

			Node* pUpNode = pBaseNode;

			pos = 0;
			do
			{
				pUpNode = pUpNode->FindParent();

				if (pUpNode && pUpNode->IsEffect())
					pFirstEditedLE = (NodeEffect*)pUpNode;

				pos++;
			}
			while (pUpNode && pUpNode->IsEffect());
		}

		if (pFirstEditedLE)
		{
			pLERange->AddNode(pFirstEditedLE);
		}
		else
		{
			pLERange->AddNode(pBaseNode);
		}

		pNode = pRange->FindNext(pNode);
	}

	return pLERange;
}




/********************************************************************************************

>	static ListRange* EffectsStack::GetTopClassRange(CCRuntimeClass* pClass,
														BOOL bClassOnly = TRUE,
														BOOL bIgnorePassThroughEffects = FALSE
														Node** pMasterNode = NULL,
														INT32* piStackPos = NULL,
														Range* pRange = NULL,
														BOOL bEscapeOldControllers = TRUE
														)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/05/2005
	Inputs:		pClass		- pointer to class of node to find in effects stack
				bClassOnly	- Only allow nodes of the specified class to be added to the list
				pRange		- pointer to range of nodes to scan
				bEscapeConts- TRUE if old controller nodes should be escaped
	Outputs:	pMasterNode - pointer to one example of this class of node in the returned range
				piStackPos	- pointer to stack pos INT32
	Returns:	A pointer to a list range owned by you!
	Purpose:	Get a range of nodes forming a surface above the selection and within
				any live effects applied to the selection consisting of all effect nodes of type
				"class" applied to the selection
	Errors:		

********************************************************************************************/
ListRange* EffectsStack::GetTopClassRange(CCRuntimeClass* pClass,
											 BOOL bClassOnly,
											 BOOL bIgnorePassThroughEffects,
											 Node** ppMasterNode,
											 INT32* piStackPos,
											 Range* pRange,
											 BOOL bEscapeOldControllers
											 )
{
	// Loop through selection ensuring that all nodes are LiveEffects
	// and that they all share the same edit list
	if (pRange==NULL)
		pRange = GetApplication()->FindSelection();
	ENSURE(pRange, "No selection in GetClassRange!");

	if (ppMasterNode)
		*ppMasterNode = NULL;

	// We want to apply the LiveEffect to the entire selection, however inconsistent it is
	//
	// Loop through the current selection and search the parents of each selected node for LiveEffects...
	ListRange* pLERange = new ListRange();
	Node* pNode = pRange->FindFirst();
	INT32 pos = 0;
	while (pNode!=NULL)
	{
		NodeEffect* pFirstEditedLE = NULL;
		NodeEffect* pFirstEditedEffectClass = NULL;
		Node* pBaseNode = pNode;

		// Only traverse the effect stack if the selected node is 'in' the effect stack
		// (it could be offset/derived, e.g. NodeShadow)
// In this context we know that we always want to go up effects stacks whether the selected node
// is mainstream or derived...
//		if (!pNode->NeedsParent(pNode->FindParent()))
//		{
			if (bEscapeOldControllers)
				pBaseNode = EscapeOldControllers(pNode);

			Node* pUpNode = pBaseNode;

			pos = 0;
			do
			{
				pUpNode = pUpNode->FindParent();

				if (pUpNode && pUpNode->IsEffect())
//					if (!(bIgnorePassThroughEffects && ((NodeEffect*)pUpNode)->IsPassThroughEffect()))
//	The meaning of the bIgnorePassThroughEffects has changed to mean "ignore all effects"
					if (!bIgnorePassThroughEffects)
						pFirstEditedLE = (NodeEffect*)pUpNode;

				if (pUpNode && pUpNode->IsEffect() && pUpNode->GetRuntimeClass() == pClass)
				{
					pFirstEditedEffectClass = (NodeEffect*)pUpNode;
//					iClassPos = pos;
				}

				pos++;
			}
			while (pUpNode && pUpNode->IsEffect());
//		}

		// Decide what effect node should be added to the listrange for this node in the input range
		// Add a node of the specified class if we found one
		if (pFirstEditedEffectClass)
		{
			pLERange->AddNode(pFirstEditedEffectClass);

			if (ppMasterNode && *ppMasterNode==NULL)
				*ppMasterNode = pFirstEditedEffectClass;
		}
		// Add the last effect if effects were found
		else if (!bClassOnly)
		{
			if (pFirstEditedLE)
			{
				pLERange->AddNode(pFirstEditedLE);
			}
			// Add the input node itself if no effects were found
			else
			{
				pLERange->AddNode(pBaseNode);
			}
		}

		pNode = pRange->FindNext(pNode);
	}

	// Temp hack
	if (piStackPos)
		*piStackPos = STACKPOS_INVALID;

	return pLERange;
}




/********************************************************************************************

>	ListRange* EffectsStack::GetBaseLevelRange()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/12/2004
	Inputs:		-
	Outputs:	
	Returns:	A pointer to a list range owned by you!
	Purpose:	Get a range of nodes forming a surface above the selection and BELOW
				any live effects applied to the selection!
				(Used by legacy effects like Bevel, Contour)
	Errors:		
	See also:	

********************************************************************************************/
ListRange* EffectsStack::GetBaseLevelRange()
{
	// Loop through selection ensuring that all nodes are LiveEffects
	// and that they all share the same edit list
	Range* pRange = GetApplication()->FindSelection();
	ENSURE(pRange, "No selection in GetNewLevelRange!");

	// We want to apply the LiveEffect to the entire selection, underneath any PostProcessors
	//
	// Loop through the current selection and search the parents of each selected node
	ListRange* pLERange = new ListRange();
	Node* pNode = pRange->FindFirst();
//	INT32 pos = 0;
	while (pNode!=NULL)
	{
		Node* pEscNode = pNode;

		// Only traverse the effect stack if the selected node is 'in' the effect stack
		// (it could be offset, e.g. NodeShadow)
		if (!pNode->NeedsParent(pNode->FindParent()))
			pEscNode = EscapeOldControllers(pNode);

		pLERange->AddNode(pEscNode);

		pNode = pRange->FindNext(pNode);
	}

	return pLERange;
}




/********************************************************************************************

>	ListRange* EffectsStack::FindLevelContaining(NodeEffect* pNode, INT32* piStackPos)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/2004
	Inputs:		pNode - Pointer to node to search for
	Outputs:	-
	Returns:	A pointer to a list range owned by the PostProcessor stack describing
				the layer of consistent nodes in the stack of which Node was a member
				NULL if couldn't find node in stack
	Purpose:	Find a given node in the post processor stack
	Errors:		
	See also:	

********************************************************************************************/
ListRange* EffectsStack::FindLevelContaining(NodeEffect* pNode, INT32* piStackPos)
{
	*piStackPos = STACKPOS_TOP;
	if (IsEmpty())
		return NULL;

	PPStackLevel* pItem = (PPStackLevel*)GetHead();
	*piStackPos = 0;
	while (pItem!=NULL)
	{
		Node* pTestNode = pItem->listSelNodes.FindFirst();
		while (pTestNode)
		{
			if (pNode==pTestNode)
				return &pItem->listSelNodes;

			pTestNode = pItem->listSelNodes.FindNext(pTestNode);
		}

		(*piStackPos)++;
		pItem = (PPStackLevel*)GetNext(pItem);
	}

	*piStackPos = STACKPOS_TOP;
	return NULL;
}




/********************************************************************************************

>	BOOL EffectsStack::NodeInTopLevel(NodeEffect* pNode)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/11/2004
	Inputs:		pNode - Pointer to node to search for
	Outputs:	-
	Returns:	TRUE if the node is in the top level of the stack
				FALSE otherwise
	Purpose:	Find a given node in the post processor stack
	Errors:		
	See also:	

********************************************************************************************/
BOOL EffectsStack::NodeInTopLevel(NodeEffect* pNode)
{
	if (IsEmpty())
		return TRUE;

	PPStackLevel* pItem = (PPStackLevel*)GetTail();
	Node* pTestNode = pItem->listSelNodes.FindFirst();
	while (pTestNode)
	{
		if (pNode==pTestNode)
			return TRUE;

		pTestNode = pItem->listSelNodes.FindNext(pTestNode);
	}

	return FALSE;
}




/********************************************************************************************

>	INT32 EffectsStack::FindSelectionPos()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/11/2004
	Inputs:		-
	Outputs:	-
	Returns:	The stack position of the selection or -1
	Purpose:	Find whether the selection has a consistent level INT32 he stack other
				than the usual base level, -1
	Errors:		
	See also:	

********************************************************************************************/
INT32 EffectsStack::FindSelectionPos()
{
	INT32 Pos = -1;

	Range* pRange = GetApplication()->FindSelection();
	ENSURE(pRange, "!");
	ENSURE(pRange->GetRangeControlFlags().PromoteToParent==FALSE, "GetEffectsStackFromSel given a bad range");

	PPStackLevel* pFoundLevel = NULL;
	Node* pNode = pRange->FindFirst();
	while (pNode)
	{
		Node* pStackNode = pNode->GetParentController();
		if (pStackNode==NULL)
			return -1;

		if (pFoundLevel==NULL)
		{
			PPStackLevel* pItem = (PPStackLevel*)GetHead();
			Pos = 0;
			while (pItem!=NULL && pFoundLevel==NULL)
			{
				Node* pTestNode = pItem->listSelNodes.FindFirst();
				while (pTestNode && pTestNode!=pStackNode)
				{
					pTestNode = pItem->listSelNodes.FindNext(pTestNode);
				}

				if (pTestNode==pStackNode)
				{
					pFoundLevel = pItem;
					break;
				}

				Pos++;
				pItem = (PPStackLevel*)GetNext(pItem);
			}

			if (pFoundLevel==NULL)
				return -1;
		}
		else
		{
			// Just look in the level we have already identified...
			Node* pTestNode = pFoundLevel->listSelNodes.FindFirst();
			while (pTestNode && pTestNode!=pStackNode)
			{
				pTestNode = pFoundLevel->listSelNodes.FindNext(pTestNode);
			}

			if (pTestNode!=pStackNode)
			{
				return -1;
			}
		}

		pNode = pRange->FindNext(pNode);
	}

	return Pos;
}




/********************************************************************************************

>	BOOL EffectsStack::BuildEffectMenu(ContextMenu* pMenu)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/06/2005
	Inputs:		pMenu - Pointer to ContextMenu we should add items to
	Outputs:	-
	Returns:	TRUE if everything is OK
				FALSE if something went wrong enough to require caller to stop building menus
	Purpose:	Find whether the selection has a consistent level INT32 he stack other
				than the usual base level, -1
	Errors:		
	See also:	

********************************************************************************************/
BOOL EffectsStack::BuildEffectMenu(ContextMenu* pMenu)
{
PORTNOTE("menu", "BuildEffectMenu can't do anything until the menu system works");
#ifndef EXCLUDE_FROM_XARALX
	PPStackLevel* pItem = (PPStackLevel*)GetHead();
//	PPStackLevel* pNextItem = NULL;
	INT32 pos = 0;

	pMenu->BuildCommand(OPTOKEN_EDITEFFECTS);
	MenuItem* pRootItem = pMenu->GetLastItem();

	while (pItem!=NULL)
	{
//		pNextItem = (PPStackLevel*)GetNext(pItem);

		String_64 strDisplayName;
		BOOL bDestructive = FALSE;
		XPEHost::GetEffectDetails(pItem->strPostProID, &strDisplayName, &bDestructive);
		OpLiveEffectParam* pParam = new OpLiveEffectParam();
		if (pParam==NULL)
			return FALSE;

		pParam->strOpUnique = pItem->strPostProID;
		pParam->bIsDestructive = bDestructive;
		pParam->StackPosition = pos;
		pParam->pPPStack = this;

		if (pItem->strPostProID==String_256(POSTPRO_ID_SHADOW))
			pMenu->BuildCommand(OPTOKEN_EDIT_LEGACYEFFECT, FALSE, pRootItem, strDisplayName, pParam);

		else if (pItem->strPostProID==String_256(POSTPRO_ID_FEATHER))
			pMenu->BuildCommand(OPTOKEN_EDIT_LEGACYEFFECT, FALSE, pRootItem, strDisplayName, pParam);

		else
			pMenu->BuildCommand(OPTOKEN_EDIT_LIVEEFFECT, FALSE, pRootItem, strDisplayName, pParam);

		pItem = (PPStackLevel*)GetNext(pItem);
		pos++;
	}

#endif
	return TRUE;
}




