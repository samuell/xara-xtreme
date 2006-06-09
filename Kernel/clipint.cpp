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
//*/
// Camelot's Internal clipboard


#include "camtypes.h"

//#include "app.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "chapter.h"
//#include "clipext.h"	// External (oil) clipboard stuff
#include "clipint.h"	// Internal clipboard stuff
//#include "group.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "layer.h"
//#include "simon.h"
//#include "spread.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "colourix.h"
#include "collist.h"
#include "cliptype.h"

#include "camdoc.h"
#include "optspage.h"	// for PageResizeInfo page settings

#include <math.h>		// for floor()

#include "nodetxts.h"	//To add text to the clipboard
#include "nodedoc.h"
//#include "srvritem.h"	//for justin's clever "add-text-to-the-clipboard" function

//#include "will2.h"
//#include "mario.h"
//#include "justin3.h"

#include "bmpcomp.h"	// BitmapList

DECLARE_SOURCE("$Revision$"); 


CC_IMPLEMENT_DYNAMIC(InternalClipboard, Document)
CC_IMPLEMENT_MEMDUMP(InternalClipboardFormat, CC_CLASS_MEMDUMP)

#define new CAM_DEBUG_NEW


InternalClipboard* InternalClipboard::pInstance = 0;
InternalClipboard* InternalClipboard::pOther = 0;



/********************************************************************************************

>	InternalClipboard::InternalClipboard(InternalClipboard::ID id, BOOL fHide = TRUE)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/7/94

	Purpose:	InternalClipboard constructor. 

********************************************************************************************/

InternalClipboard::InternalClipboard(InternalClipboard::ID id, BOOL fHide)
  : Document(fHide),
	pClipboardRange(new Range),
	ClipOp(FALSE),
	m_id(id)
{
	// Empty.
}


/********************************************************************************************

>	virtual InternalClipboard::~InternalClipboard(); // Destructor

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/7/94

	Purpose:	InternalClipboard destructor

********************************************************************************************/

InternalClipboard::~InternalClipboard()
{
	delete pClipboardRange;
	pClipboardRange = 0;
}



/********************************************************************************************
>	static InternalClipboard::ID InternalClipboard::GetCurrentID()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/10/96
	Returns:	The ID of the 'current' clipboard, the one return by Instance().
********************************************************************************************/

InternalClipboard::ID InternalClipboard::GetCurrentID()
{
	return pInstance->m_id;
}



/********************************************************************************************

>	static BOOL InternalClipboard::Initialise() 				 

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/8/94

	Purpose:	This method creates the one and only instance of the InternalClipboard and
				then initialises it. Called by main3.cpp

********************************************************************************************/

BOOL InternalClipboard::Initialise() 				 
{
	// Create the clipboards.
	if (!(pInstance = new InternalClipboard(InternalClipboard::CLIPBOARD))) return FALSE;

	if (!(pOther = new InternalClipboard(InternalClipboard::DRAGDROP)))
	{
		delete pInstance;
		return FALSE;
	}

//	if (!CCamDoc::Create(pInstance))
PORTNOTE("other", "Clipboard doc should be hidden")
	CCamDoc* pOilDoc1 = new CCamDoc(pInstance);
	if (pOilDoc1 == NULL)
	{
		delete pInstance;
		delete pOther;
		return FALSE;
	}

//	if (!CCamDoc::Create(pOther))
PORTNOTE("other", "Clipboard doc should be hidden")
	CCamDoc* pOilDoc2 = new CCamDoc(pOther);
	if (pOilDoc2 == NULL)
	{
		delete pInstance->GetOilDoc();
		delete pOther;
		return FALSE;
	}

	pInstance->OriginalCentreLock = TRUE;
	 
	return TRUE;
}



/********************************************************************************************

>	BOOL InternalClipboard::Init(CCamDoc* pOilDoc)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/8/94
	
	Inputs:		pOilDoc	- the CCamDoc to attach the cliboard to, if any.
	
	Returns:	FALSE if it failed

	Purpose:	This method initialises the Internal clipboard.
				It is an internal method, called by Initialise()
				It calls down to Document::Init, then adds an insertion Layer node to the
				bottom of the basic doc tree.

	SeeAlso:	InternalClipboard::Initialise(); Document::Init; Document::InitTree

********************************************************************************************/

BOOL InternalClipboard::Init(CCamDoc* pOilDoc)
{
	if (!Document::Init(pOilDoc)) return FALSE;

	Node *pSpread = FindFirstSpread();
	ERROR3IF(pSpread == NULL, "No Spread in document!");

	Node *InsPos = pSpread->FindFirstChild();
	ERROR3IF(InsPos == NULL, "No Spread-child in document!");

	// Now scan the children of the first spread until we find a layer, or run out of nodes
	while (InsPos != NULL && !InsPos->IsLayer())
		InsPos = InsPos->FindNext();

	if (InsPos == NULL)		// No Layer, so we'd better add one for ourselves
	{
		String_256 LayerID = String_256(_R(IDS_K_CLIPINT_LAYERNAME));
		Layer *pLayer = new Layer(pSpread, LASTCHILD, LayerID);
		ERRORIF(!pLayer, _R(IDE_NOMORE_MEMORY), InitFailed())
	}

	OriginalCentreLock = TRUE;

	// Sucess.
	return(TRUE);;
}



/********************************************************************************************

>	static void InternalClipboard::Deinit() 				 

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/8/94

	Purpose:	This method destroys the InternalClipboard
				It is called by main.cpp on shutdown
				(and now by CCamDocTemplate::CloseAllDocuments immediately prior to shutdown)

	Notes:		This code may be called multiple times without problems. Please keep it that way.

********************************************************************************************/

void InternalClipboard::Deinit()
{
	// Make sure the clipboards are empty.
	// Sort out the main clipboard first
	Clear();

	// Now sort out the alternate clipborad
	Swap();
	Clear();
	Swap();
	
	if (pInstance)
	{
		delete pInstance->GetOilDoc();
		pInstance = 0;
	}

	if (pOther)
	{
		delete pOther->GetOilDoc();
		pOther = 0;
	}
}



/********************************************************************************************

>	static void InternalClipboard::Swap()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/9/96
	Purpose:	Swaps the two internal clipboards, pInstance and pOther.  Used to allow
				drag'n'drop operations to happen without disturbing the internal
				clipboard.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void InternalClipboard::Swap()
{
	InternalClipboard* pcb = pInstance;
	pInstance = pOther;
	pOther = pcb;
}



/********************************************************************************************

>	static Layer *InternalClipboard::GetInsertionLayer()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com> (recoded from Simon's for new clipboard document layout)
	Created:	23/4/95 (21/7/94)

	Returns:	A pointer to the layer which is the parent of all objects placed on the 
				clipboard

	Purpose:	Returns a pointer to the clipboard's layer node which is the parent of all
				nodes which get added to the clipboard. 

	Errors:		ERROR3 and returns NULL if an internal consistency check fails

********************************************************************************************/

Layer* InternalClipboard::GetInsertionLayer()
{
	// Is there a tree (this gets called during destruction, which is a real pain).
	if (!pInstance || !pInstance->GetFirstNode())
	{
		TRACEUSER( "JustinF", _T("No first node in InternalClipboard::GetInsertionLayer\n"));
		return 0;
	}

	// Get the first spread
	Node* pSpread = pInstance->FindFirstSpread();
	ERROR3IF(pSpread == NULL, "No spread found in clipboard doc");

	// Get the spread's first child...
	Node *InsPos = pSpread->FindFirstChild();
	ERROR3IF(InsPos == NULL, "No spread-child found in clipboard doc");

	// And now search across until we find the first layer
	while (InsPos != NULL && !InsPos->IsLayer())
		InsPos = InsPos->FindNext();

	ERROR3IF(InsPos == NULL, "No insertion layer found in clipboard doc");
	return (Layer*) InsPos;
}



/********************************************************************************************

>	void InternalClipboard::Clear(); 																		 

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/7/94

	Purpose:	Deletes the contents of the internal clipboard

********************************************************************************************/

void InternalClipboard::Clear()
{
	Layer* Root = GetInsertionLayer();
	if (Root == NULL || Root->FindFirstChild() == NULL)
		return;		// The clipboard is empty already

	// The clipboard range is no longer valid
	if (pInstance->pClipboardRange) delete pInstance->pClipboardRange;

	// An empty range. We need to create this cos the operation could fail
	pInstance->pClipboardRange = new Range;

	// Delete all objects in clipboard
	Root->DeleteChildren(Root->FindFirstChild());


	// Ensure that the top level of the tree has bounding rect caches invalidated,
	// as we' ve just vaped the entire subtree below it
	Root->InvalidateBoundingRect();

  // We can't do this at the moment because by the time we're called it seems the component
  // copy is already under way, and so we're well screwed

	// Destroy and re-initialise the doc-components (to chuck away unneeded stuff)
//	DestroyDocComponents();	
//	if (!InitDocComponents())
//	{
//		ERROR3("DocComponent initialisation failed in InternalClipboard::ClearClipboard");
//	}

	// Instead, we are really only concerned about stacks of colours building up, so
	// we'll just delete any *unused* IndexedColours from the document's list. (Some colours
	// are kept in-use by the attribute manager and stuff...)

	// First, all named colours (styles)
	ColourList *ColList = pInstance->GetIndexedColours();
	IndexedColour *Ptr = (IndexedColour *) ColList->GetHead();
	IndexedColour *Next;
	while (Ptr != NULL)
	{
		Next = (IndexedColour *) ColList->GetNext((ListItem*)Ptr);
		if (!Ptr->IsInUse())
		{
			// Remove this colour from the list
			ColList->RemoveItem(Ptr);

			// Ensure this is not in use as a parent of other colours
			ColList->RemoveLinksToColour(Ptr);

			// And vape it
			delete Ptr;
		}

		Ptr = Next;
	}

	// Now all unnamed colours (locals)
	List *Locals = ColList->GetUnnamedColours();
	Ptr = (IndexedColour *) Locals->GetHead();
	while (Ptr != NULL)
	{
		Next = (IndexedColour *) Locals->GetNext(Ptr);
		if (!Ptr->IsInUse())
		{
			// Remove this colour from the list
			Locals->RemoveItem(Ptr);

			// And vape it (locals can't be parent colours, so no need to
			// RemoveLinksToColour() for them)
			delete Ptr;
		}

		Ptr = Next;
	}

	// Now do the same for this document's bitmap list
	BitmapList *pBmpList = pInstance->GetBitmapList();
	if (pBmpList)
	{
		ListItem* pItem = pBmpList->GetHead();
		ListItem* pNextItem;
		while (pItem)
		{
			pNextItem = pBmpList->GetNext(pItem);

			KernelBitmap* pBmpItem = (KernelBitmap*)pItem;
			// check if it is the default bitmap, if so then leave in the list
			if (!pBmpItem->IsDefaultBitmap())
			{			
				pBmpList->RemoveItem(pBmpItem);
				delete pBmpItem;
			}

			pItem = pNextItem;
		}
	}
}



/********************************************************************************************

>	static BOOL InternalClipboard::AttrsHaveChanged()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Purpose:	This function should be called if the attributes of the Internal clipboard
				have changed in some way. eg. if a bitmap has been deleted.  It destroys
				the cache of common attributes.

********************************************************************************************/

void InternalClipboard::AttrsHaveChanged()
{
	if (pInstance && pInstance->pClipboardRange)
	{
		pInstance->pClipboardRange->AttrsHaveChanged();
	}
}



/********************************************************************************************

>	BOOL InternalClipboard::CopyObjects(Range& RangeToCopy, Operation * pOp)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/7/94
	Inputs:		RangeToCopy: The objects to copy to the clipboard

	Returns:	FALSE if we could not copy 

	Purpose:	Copies a range of objects to the clipboard, all previous clipboard contents
				are deleted. If the copy fails the clipboard is emptied and FALSE returned.
				It can be called via CALL_WITH_FAIL.

	SeeAlso:	InternalClipboard::CopyComponentData()

********************************************************************************************/



BOOL InternalClipboard::CopyObjects(Range& RangeToCopy, Operation * pOp)
{
	 // Delete the current contents of the clipboard
	pInstance->ClipOp = TRUE;
	Clear();
	
	// All objects to be copied to the clipboard need to be made attribute complete or else
	// they may loose important attributes. This is especially true of objects within groups.

	if (!(RangeToCopy.MakeAttributeComplete(TRUE,FALSE,TRUE)))
		return(FALSE); // Failed

	BOOL Failed = FALSE;  // until we know better

	Node* Root = GetInsertionLayer(); 
	ENSURE(Root != NULL, "Root of clipboard is NULL"); 

 	if (!Root->CopyComplexRange(RangeToCopy))
	{
		Failed = TRUE;
		Clear();	// Tidy up
		ERROR3("Failed to copy objects");
	}

// Matt (+Karim, + MarkH + loads of other people I whined to) - 02/02/2001
// Removed David McClarnon's fix for 'copy-inside on bevelled blends' as it causes the operation history to
// reference the object in the clipboard - which is deleted when the clipboard is changed !!! -> AccessViolation-ahoy!
/*
	// call post duplicate on all nodes
	Node * pChildNode = Root->FindFirstDepthFirst();

	while (pChildNode && pChildNode != Root)
	{
		if (pChildNode->IsAnObject())
		{
			if (pOp != NULL)
			{
				if (pOp->IS_KIND_OF(UndoableOperation))
				{
					((NodeRenderableInk *)pChildNode)->PostDuplicate((UndoableOperation *)pOp);
				}
				else
				{
					if (!pChildNode->IsABaseTextClass())
						((NodeRenderableInk *)pChildNode)->PostDuplicate(NULL);
				}
			}
		}

		pChildNode = pChildNode->FindNextDepthFirst(Root);
	}
*/
	// We now need to optimise the attributes on all objects copied to the clipboard
	if (!Failed)
	{
		Node* FirstNode = pInstance->GetFirstNode();
		ERROR3IF(!FirstNode, "Cannot find first clipboard node"); 
		if (FirstNode)
		{
			Node* Root = FirstNode->FindNext(); 
			ERROR3IF(!Root, "Clipboard has no root");
			ERROR3IF(!(IS_A(Root, NodeDocument)), "This should be the root of the clipboard"); 
			if (Root)
			{ 
				if (!Root->OptimiseAttributes())
				{
					Failed = TRUE; 
					Clear(); 
				}
			} 
		} 
	
	}
 
		
	// Ensure that the top level of the tree has bounding rect caches invalidated,
	// as we've just plugged an entire new subtree in!
	((Layer *)Root)->InvalidateBoundingRect();

	// We now need to Normalise the attributes of all objects copied
	RangeToCopy.NormaliseAttributes(FALSE,TRUE); 

	pInstance->ClipOp = FALSE;
	return (!Failed); 
} 





/********************************************************************************************

>	BOOL InternalClipboard::CopyComponentData(BaseDocument* pSrcDoc)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/7/94
	Inputs:		pSrcDoc: The source document

	Returns:	FALSE if the component data has not successfuly been copied accross. In this
				situation AbortComponentCopy gets called. 
				 
	Purpose:	This function should get called after CopyObjects has been called. It asks
				all objects to copy accross their component data to the clipboard. It can
				be called via CALL_WITH_FAIL.

********************************************************************************************/

BOOL InternalClipboard::CopyComponentData(BaseDocument* pSrcDoc)
{
	if (IsEmpty(FALSE))
	{
		// The CopyObjects either was not called, or failed
		ERROR3("InternalClipboard::CopyComponentData called when clipboard is empty!");
		return(FALSE);
	}
	
	// Scan all clipboard nodes
	Node* Root = GetInsertionLayer();
	Node* Current = Root->FindFirstDepthFirst();
	while (Current != NULL)
	{
		// Ask the current node if it would copy it's data to the relevant DocComponents
		if (Current->IsKindOf(CC_RUNTIME_CLASS(NodeRenderable)))
		{
			if (!((NodeRenderable*) Current)->CopyComponentData(pSrcDoc, pInstance))
			{
				// No luck
				pInstance->AbortComponentCopy();	// Cancel all data which has been copied
				Clear();		// Tidy up
				return FALSE; 
			}
		}

		Current = Current->FindNextDepthFirst(Root); 
	}

	return TRUE; // Success
}



/********************************************************************************************

>	DocRect InternalClipboard::GetObjectsBounds()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/7/94

	Returns:	The bounds of all objects in the clipboard

	Purpose:	For finding the bounds of all objects in the clipboard

********************************************************************************************/

DocRect InternalClipboard::GetObjectsBounds()
{
	DocRect Bounds;

	ERROR3IF(IsEmpty(FALSE), "InternalClipboard::GetObjectsBounds called when CB is empty");

	Node* CurrentNode = GetInsertionLayer()->FindFirstChild();
	while (CurrentNode  != NULL)
	{
		if (CurrentNode->IsKindOf(CC_RUNTIME_CLASS(NodeRenderableBounded)))
		{
			Bounds = Bounds.Union(
				( ((NodeRenderableBounded*)CurrentNode)->GetBoundingRect(TRUE, FALSE)) );
		}
		CurrentNode = CurrentNode->FindNext();
	}

	return Bounds;
}



/********************************************************************************************

>	static BOOL InternalClipboard::CopyInProgress(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/4/95

	Returns:	TRUE if we are currently busy copying

	Purpose:	Allows external entities to determine if they were called during a
				clipboard copy operation.

********************************************************************************************/

BOOL InternalClipboard::CopyInProgress()
{
	return pInstance->ClipOp;
}


/********************************************************************************************

>	static BOOL InternalClipboard::IsEmpty(BOOL AskExternalClipboard = TRUE)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/4/95

	Inputs:		AskExternalClipboard - if TRUE, this method will tell you if there is ANY
				data available for pasting. If FALSE, it simply checks if the internal
				clipboard (esentially an internal cache) is empty. ONLY the external clipboard
				(or internal calls in this class) should ever pass in FALSE for this parameter!

	Returns:	TRUE if the clipboard is empty (no data to paste)
				FALSE if the clipboard is non-empty (there is data available to paste)

	Purpose:	To determine if there is anything currently stored on the internal clipboard

	Notes:		If there is no internal clipboard, it will return TRUE

********************************************************************************************/

BOOL InternalClipboard::IsEmpty(BOOL AskExternalClipboard)
{
	if (pInstance == NULL)
		return(TRUE);

	Layer *templayer = pInstance->GetInsertionLayer();
	if (!templayer) {	ERROR3("Can't get clipboard insertion layer!"); return TRUE; }

	Node* n = templayer->FindFirstChild();
	BOOL Empty = (n == NULL);

	if (Empty && AskExternalClipboard)
	{
PORTNOTE("extclipboard", "Removed use of External Clipboard")
#ifndef EXCLUDE_FROM_XARALX
		// We're empty, so ask the External clipboard if it can supply us with data
		ExternalClipboard *Clipboard = ExternalClipboard::GetExternalClipboard();

		if (Clipboard == NULL)
			return(TRUE);

		return(Clipboard->IsEmpty());
#else
		return TRUE;
#endif
	}

	return(Empty);
}



/********************************************************************************************

>	BOOL InternalClipboard::PrepareForCopy(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/4/95

	Returns:	TRUE if the clipboard is ready to accept new data
				FALSE if the copy must be aborted

	Purpose:	To ready the clipboard for a cut/copy operation (copy data TO the clipboard).
				This will interact with the external clipboard in order to claim the global
				clipboard, and readies the clipboard for use.

	Notes:		After you have copied data to the clipboard, you MUST call CopyCompleted

	SeeAlso:	InternalClipboard::CopyCompleted; InternalClipboard::PrepareForPaste;
				ExternalClipboard::PrepareForCopy

********************************************************************************************/

BOOL InternalClipboard::PrepareForCopy(void)
{
	Clear();
PORTNOTE("extclipboard", "Removed use of External Clipboard")
#ifndef EXCLUDE_FROM_XARALX
	ExternalClipboard* pcb = ExternalClipboard::GetExternalClipboard();
	return pcb ? pcb->PrepareForCopy() : FALSE;
#else
	return TRUE;
#endif
}



/********************************************************************************************

>	BOOL InternalClipboard::CopyCompleted(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/4/95

	Returns:	TRUE if all went well
				FALSE if the copy failed in some way

	Purpose:	Tells the InternalClipboard that you have finished copying data to it.
				This interacts with the external clipboard in order to make the new data
				available to the outside world.

	Notes:		Before starting to copy data to the clipboard you MUST call PrepareForCopy

	SeeAlso:	InternalClipboard::PrepareForCopy; ExternalClipboard::CopyCompleted

********************************************************************************************/

BOOL InternalClipboard::CopyCompleted(void)
{
	if (IsEmpty(FALSE))
	{
		ERROR3("That copy wasn't much fun - the clipboard's still empty!");
		return(FALSE);
	}

PORTNOTE("extclipboard", "Removed use of External Clipboard")
#ifndef EXCLUDE_FROM_XARALX
	ExternalClipboard *Clipboard = ExternalClipboard::GetExternalClipboard();
	if (Clipboard == NULL) return(FALSE);

	if (Clipboard->CopyCompleted())
	{
#endif
		// Objects have successfully been moved to clipboard. Perform neccessary processing
		if (!pInstance->OnNewClipboardObjects()) InformError();
		return TRUE; 
#ifndef EXCLUDE_FROM_XARALX
	}	
	else
	{
		return FALSE;
	}
#endif
}



/********************************************************************************************

>	BOOL InternalClipboard::PrepareForPaste(BOOL* ExternalSource = NULL)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/4/95

	Outputs:	ExternalSource: When the function returns TRUE, ExternalSource will be TRUE
								if the data has come from the external clipboard, and
								FALSE if not.

	Returns:	TRUE if the clipboard is ready to provide data for pasting
				FALSE if the paste must be aborted (nothing to paste)

	Purpose:	To ready the clipboard for a paste operation (copy data FROM the clipboard).
				This may interact with the external clipboard in order to provide the
				necessary data.

	Notes:		After copying data from the clipboard you MUST call PasteCompleted

	SeeAlso:	InternalClipboard::PasteCompleted; InternalClipboard::PrepareForCopy;
				ExternalClipboard::PrepareForPaste

********************************************************************************************/

BOOL InternalClipboard::PrepareForPaste(BOOL* ExternalSource)
{
	if (ExternalSource)
		*ExternalSource = FALSE;   // Until we know better

	pInstance->OriginalCentreLock = TRUE;						// Don't allow centre to be reset

	// If we are empty, then we'll have to go and look outside for something to put onto
	// the internal clipboard.
	if (IsEmpty(FALSE))
	{
PORTNOTE("extclipboard", "Removed use of External Clipboard")
#ifndef EXCLUDE_FROM_XARALX
		ExternalClipboard *Clipboard = ExternalClipboard::GetExternalClipboard();
		if (Clipboard == NULL || !Clipboard->PrepareForPaste())
			return(FALSE);

		// Optimise attributes on the clipboard
		Node* FirstNode = pInstance->GetFirstNode();
		ERROR3IF(!FirstNode, "Cannot find first clipboard node"); 
		if (FirstNode)
		{
			Node* Root = FirstNode->FindNext(); 
			ERROR3IF(!Root, "Clipboard has no root");
			ERROR3IF(!(IS_A(Root, NodeDocument)), "This should be the root of the clipboard"); 
			if (Root && !Root->OptimiseAttributes()) return FALSE;
		}

		// Objects successfully transferred to clipboard. Perform neccessary processing
		if (!pInstance->OnNewClipboardObjects()) InformError();
		
		pInstance->OriginalCentreLock = FALSE;						// Allow centre to be reset on first Paste

		// Data has come from external clipboard 
		if (ExternalSource) *ExternalSource = TRUE;
#endif
	}

	return(TRUE);
}



/********************************************************************************************

>	BOOL InternalClipboard::PasteCompleted(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/4/95

	Returns:	TRUE if all went well
				FALSE if the paste failed in some way

	Purpose:	Tells the InternalClipboard that you have finished copying data from it.
				This provides interaction with the external clipboard.

	Notes:		Before starting to copy data to the clipboard you MUST call PrepareForCopy

	SeeAlso:	InternalClipboard::PrepareForPaste; ExternalClipboard::PasteCompleted

********************************************************************************************/

BOOL InternalClipboard::PasteCompleted(void)
{
	if (IsEmpty(FALSE))
	{
		ERROR3("That paste wasn't much fun - the clipboard's empty now!");
	}

PORTNOTE("extclipboard", "Removed use of External Clipboard")
#ifndef EXCLUDE_FROM_XARALX
	ExternalClipboard *Clipboard = ExternalClipboard::GetExternalClipboard();

	if (Clipboard == NULL)
		return(FALSE);

	return(Clipboard->PasteCompleted());
#else
	return TRUE;
#endif
}



/********************************************************************************************

>	static void InternalClipboard::DescribeContents(String_64 *Result);

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/5/95

	Outputs:	Result will be filled in with either a blank string, or a description of the
				current internal clipboard contents. 

	Purpose:	To determine the description of the clipboard contents

	Notes:		Redirects the call to the external clipboard, as it has a better idea 
				of the overall picture.

	SeeAlso:	ExternalClipboard::DescribeContents

********************************************************************************************/

void InternalClipboard::DescribeContents(String_64 *Result)
{
	ERROR3IF(Result == NULL, "Illegal NULL param");
PORTNOTE("extclipboard", "Removed use of External Clipboard")
#ifndef EXCLUDE_FROM_XARALX
	ExternalClipboard::DescribeContents(Result);
#else
	static String_64 strDesc = String_64(_T("No description (see InternalClipboard::DescribeContents)"));
	Result = &strDesc;
#endif
}



/********************************************************************************************

>	static void InternalClipboard::DescribeContentsInternal(String_64* Result);

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/5/95

	Outputs:	Result will be filled in with either a blank string, or a description of the
				current internal clipboard contents. 

	Purpose:	To determine the description of the clipboard contents

	Notes:		Internal version of this call, for the external clipboard only.

	SeeAlso:	InternalClipboard::DescribeContents

********************************************************************************************/

void InternalClipboard::DescribeContentsInternal(String_64* Result)
{
	ERROR3IF(Result == NULL, "Illegal NULL param");

	*Result = TEXT("");
	if (IsEmpty(FALSE))	return;

	// --- Scan the document tree to determine the description. Note that this should be
	// done by calling a range, except that the code is in SelRange, so I've rewritten it here
	// to save myself half a day's delay waiting for a range.h rebuild when I can't afford it...

	Node* Subtree = GetInsertionLayer();
	if (Subtree == NULL)
		return;

	Range ClipRange(Subtree->FindFirstChild(), Subtree->FindLastChild(),
						RangeControl(TRUE,TRUE,TRUE,TRUE));

	String_256 Description;
	Node* Current = ClipRange.FindFirst();
	if (Current == NULL) return;

	// Implementation
	Node* FirstInRange = Current; 
	CCRuntimeClass* NodeType = Current->GetRuntimeClass();  
	
	// --------------------------------------------------------------------------------
	// Determine if there is more than one type of object selected

	BOOL OneTypeOfObject = TRUE;                          // TRUE if range contains only
														  // one type of object. 
	do
	{   
		if (Current->GetRuntimeClass() != NodeType)	      // Range contains more than one
														  // type of object
		{
			// Current object is not the same type as the first so we know there is more
			// than one type of object.
			OneTypeOfObject = FALSE;
			break;
		}

		Current = ClipRange.FindNext(Current);
	}     
	while (Current);
	
	UINT32 NumObjectsInRange = ClipRange.Count(); 					 // Count is cached
	ERROR3IF(NumObjectsInRange <= 0, "No objects selected"); 

	if (OneTypeOfObject)
	{
		// All objects in the selection are of the same type

		String_32 Name;
		BOOL UseClassToDescribe	= TRUE;

		NodeGroup* FirstGroup = NULL; 

		if (FirstInRange->GetRuntimeClass() == CC_RUNTIME_CLASS(NodeGroup))
		{
			UseClassToDescribe = FALSE; 
			// If all selected groups have the same name then the name is used in the 
			// description
			FirstGroup = (NodeGroup*)FirstInRange;
			NodeGroup* CurrentGroup = FirstGroup;
			Name = CurrentGroup->GetName(); 
			if (!(Name.IsEmpty()))
			{
				do
				{
					CurrentGroup=(NodeGroup*)ClipRange.FindNext(CurrentGroup); 
			
					if (CurrentGroup != NULL)
					{
						if (CurrentGroup->GetName() != Name)
						{
							// A group has been found with a different name. Stop the search and
							// use the class to describe the selection.
							UseClassToDescribe = TRUE; 
							break;					
						}
					}

				} while (CurrentGroup != NULL); 
			}
			else
			{
				UseClassToDescribe = TRUE;
			}
		}
		if (UseClassToDescribe)
		{
			Description = (FirstInRange->Describe(NumObjectsInRange>1));                                   
		}
		else 
		{
			// Use the group name to describe the selection
			ERROR3IF(FirstGroup == NULL, "FirstGroup should be the first selected group"); 
			if (NumObjectsInRange > 1)
			{
				Description = SelRange::Pluralise(FirstGroup->GetName()); 
			}
			else
			{
				Description = FirstGroup->GetName();
			}
		}
	}
	else
	{
		// There must be more than one type of selected objects
		ERROR3IF(NumObjectsInRange <= 1, "Only a single object is in range"); 
		// Cos there are multiple objects of different types we describe them as 'objects'
		Description = String(_R(IDS_OBJECTS)); 	
	}

	Description.Left(Result, 63);
}



/********************************************************************************************

>	BOOL InternalClipboard::OnNewClipboardObjects()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/9/95
	Returns:	FALSE if something has gone wrong. An InformError should be called if this
				occurs. The Cut/Copy/Paste Operation being performed should not be aborted though. 

	Purpose:	This fn will be called whenever data has successfuly been transfered to 
				the InternalClipboard. i.e. After a Cut, Copy, or (paste from External Clipboard)
		
	
	Scope:		private

********************************************************************************************/

BOOL InternalClipboard::OnNewClipboardObjects()
{
	// Set the bounds of the spread containing the objects to the bounds of the objects.
	Spread* pSpread = FindFirstSpread();
	ERROR2IF(!pSpread, FALSE, "No first Spread in InternalClipboard::OnNewClipboardObjects");

	// Calculate the bounds of all the objects on the spread (this code is based on
	// Spread::GetPageVisibleBounds, ie. the function that Zoom to Drawing uses).
	DocRect drBounds;
	drBounds.MakeEmpty();

	// Search through all the children of the spread, looking for layers.
	Layer* pLayer = pSpread->FindFirstLayer();
	while (pLayer)
	{
		// We ignore the guide layers, which always have an empty bounding box
		// (according to Mark Neves!)
		if (!pLayer->IsGuide())
		{
			// Include the bounds of the objects on this layer.
			drBounds = drBounds.Union(pLayer->GetBoundingRect());
		}

		// And onto the next layer ...
		pLayer = pLayer->FindNextLayer();
	}

	// If the bounds aren't empty then set the page size to be those bounds, so the
	// page is the smallest size that encloses the objects it contains.
	if (!drBounds.IsEmpty())
	{
		// Remember the centre of the bounds of the objects, so Paste-In-Place will
		// still work.
		dcCentre.x = drBounds.lo.x + drBounds.Width() / 2;
		dcCentre.y = drBounds.lo.y + drBounds.Height() / 2;

		// Now convert to document coordinates.
		pSpread->SpreadCoordToDocCoord(&drBounds);

		// Work out the new dimensions of the page.
/*		MILLIPOINT mpWidth  = drBounds.Width();
		MILLIPOINT mpHeight = drBounds.Height();
		
		// Inflate the size of the page by 5% to give a pleasing border around the objects.
		const double fpInflate = 1.024695076596;
		mpWidth  = (MILLIPOINT) floor(mpWidth  *  fpInflate + 0.5);
		mpHeight = (MILLIPOINT) floor(mpHeight *  fpInflate + 0.5);
*/

PORTNOTE("other", "Removed use of CCamDoc::SetPageSize")
#ifndef EXCLUDE_FROM_XARALX
//#if (_OLE_VER >= 0x200)		
		// Resize the page to just surround the objects, but don't scale the objects
		// by the same amount.
		ERROR2IF(!GetOilDoc()->SetPageSize(mpWidth, mpHeight, FALSE),
				 FALSE, "SetPageSize failed in InternalClipboard::OnNewClipboardObjects");
//#endif
#endif

		// Discard any undo information in this clipboard document.
		EmptyOperationHistory();
	}
#ifdef _DEBUG
	else
	{
		// This should be investigated if you see this TRACE.  Perhaps because the selection
		// consisted of invisible layers?
		TRACE( _T("Warning: no bounds for SetPageSize in InternalClipboard::OnNewClipboardObjects (?)\n"));
	}
#endif

	// Currently this routine creates a new Range of objects on the clipboard
	delete pClipboardRange;
	pClipboardRange = NULL; 

	// Find the first child of the clipboard's layer
	Node* pNode = Node::FindFirstChapter(this);  
	NodeRenderableInk* pInkNode = NULL;
	if (pNode) pNode = ((Chapter*) pNode)->FindFirstSpread(); 
	if (pNode) pNode = ((Spread*) pNode)->FindFirstLayer();
	if (pNode)
	{
		pInkNode = (NodeRenderableInk*)pNode->FindFirstChild(CC_RUNTIME_CLASS(NodeRenderableInk));
		while (pInkNode!=NULL && !pInkNode->CanSelectAsCompoundParent())
		{
			if (pInkNode->IsCompoundClass() && ((NodeCompound*)pInkNode)->GetInkNodeFromController())
				pInkNode = ((NodeCompound*)pInkNode)->GetInkNodeFromController();
			else
				pInkNode = (NodeRenderableInk*)pInkNode->FindFirstChild(CC_RUNTIME_CLASS(NodeRenderableInk));
		}
	}

	ERROR3IF(!pInkNode, "InternalClipboard::OnNewClipboardObjects, Where has the clipboard gone ??");
	if (!pInkNode) return FALSE; // What else can we do ?
	
	// Try to create a range of all objects.
	pClipboardRange = new Range(pInkNode, NULL, RangeControl(TRUE, TRUE));
	return pClipboardRange != 0;
}



/********************************************************************************************
>	DocCoord InternalClipboard::GetOriginalCentrePoint()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/10/96
	Returns:	The original centre point of the objects on the clipboard before they were
				copied.
	Purpose:	Paste-in-place centres the objects when they are pasted at this point.
	SeeAlso:	InternalClipboard::OnNewClipboardObjects;
********************************************************************************************/

DocCoord InternalClipboard::GetOriginalCentrePoint()
{
	return pInstance->dcCentre;
}




/********************************************************************************************
>	void InternalClipboard::SetOriginalCentrePoint(DocCoord NewCentre)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/10/96
	Input:		NewCentre	A position that the caller would like to record as being the
							"original centre" of the objects on the clipboard.
	Returns:	-
	Purpose:	Paste-in-place centres the objects when they are pasted at this point.
				Reset the centre point but only do so when the centre point is unlocked.
	SeeAlso:	InternalClipboard::OnNewClipboardObjects, OpPaste::DoPasteStandard;
********************************************************************************************/

void InternalClipboard::SetOriginalCentrePoint(DocCoord NewCentre)
{
	if (!pInstance->OriginalCentreLock)
	{
		pInstance->dcCentre = NewCentre;
		pInstance->OriginalCentreLock = TRUE;
	}
}

/********************************************************************************************
>	void InternalClipboard::CopyText(TCHAR* pcBuffer)

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>, liberally copied from BodgeTextMap::HandleImport
	Created:	24/4/97
	Input:		pcBuffer	The text to copy on to the clipboard
	Returns:	-
	Purpose:	Copies the text on to the clipboard (by converting it
				to a text object).
	SeeAlso:	TextStory::CreateFromChars, BodgeTextMap::HandleImport
********************************************************************************************/

BOOL InternalClipboard::CopyText(TCHAR* pcBuffer)
{
PORTNOTE("other", "InternalClipboard::CopyText disabled due to use of CCamSrvrItem")
#ifndef EXCLUDE_FROM_XARALX
	//We need to tell CCamSrvrItem that what we are about to put on the clipboard 
	//should be treated as text

	//To do this, we call RenderProprietary Formats, which sets a flag. The
	//returned value will be the old state of this flag, which we must remember
	//cos otherwise Justin will beat me up
	BOOL fOldState = CCamSrvrItem::RenderProprietaryFormats(FALSE);

	//If we have some text
	if (pcBuffer != NULL && pcBuffer[0] != '\0')
	{
		//Create a text story, which we position arbitarily on the clipboard
		TextStory* pTextStory=TextStory::CreateFromChars(DocCoord(100,100), pcBuffer,
					NULL, InternalClipboard::Instance(), NULL, TRUE);

		if (pTextStory!=NULL)
		{
			//Prepare to copy...this will clear the clipboard
			PrepareForCopy();
	
			// Attach the text story to the clipboard
			pTextStory->AttachNode(InternalClipboard::GetInsertionLayer(), LASTCHILD);
			pTextStory->NormaliseAttributes();

			// And format the text story properly
			pTextStory->FormatAndChildren(NULL,FALSE,FALSE);

			CopyCompleted();

			//And reset the old state of the flag
			CCamSrvrItem::RenderProprietaryFormats(fOldState);

			return(TRUE);
		}

		// We failed, so clear the clipboard
		InternalClipboard::Clear();

		//And reset the old state of the flag
		CCamSrvrItem::RenderProprietaryFormats(fOldState);

		return FALSE;
	}
#endif

	return FALSE;
		
}




