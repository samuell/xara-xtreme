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
// Implementation of BaseDocument class

/*
*/

#include "camtypes.h"

#include "app.h"
#include "camelot.h"
#include "basedoc.h"
#include "colcomp.h"
#include "doccomp.h"
#include "dumbnode.h"
//#include "jason.h"
#include "nodedoc.h"
#include "progress.h"
#include "fontlist.h"
//#include "textinfo.h"
//#include "mario.h"
//#include "resource.h"
#include "fontman.h"
#include "collist.h"
#include "camdoc.h"
#include "exphint.h"
#include "nodeliveeffect.h"
//#include "xpehost.h"
//#include "peter.h"
//#include "simon.h"

#ifdef RALPH
#include "ralphcri.h"
#endif

DECLARE_SOURCE($Revision: 662 $)
CC_IMPLEMENT_DYNAMIC(BaseDocument, ListItem)

// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW


/********************************************************************************************

>	BaseDocument::BaseDocument(BOOL fHide = TRUE)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/7/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	BaseDocument constructor
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BaseDocument::BaseDocument(BOOL fHide)
  :	IsHidden(fHide),
	m_pExportHint(NULL),
	TreeStart(0),
	DocComponents(0),
	TagCounter(1)
{
	// Empty.
} 



/********************************************************************************************

>	BaseDocument::~BaseDocument()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/7/94
	Purpose:	The BaseDocument destructor deletes all nodes in the document.
				It also deletes all the document components.

********************************************************************************************/

BaseDocument::~BaseDocument()
{
	// Destroy the tree if it exists.  This routine deletes all nodes to the right of
	// the StartDocument node.  This is done by continually deleting the node to the
	// right (which shuffles all the others left one) until there are no more nodes.
	// Then the StartDocument node itself is deleted.
	if (TreeStart != NULL)
	{
		for (Node* pn = TreeStart->FindNext(); pn != NULL; pn = TreeStart->FindNext())
		{
			// Delete the node's children then delete the node.  The TreeStart
			// variable will be updated to point to the new first node.
			pn->CascadeDelete();
			delete pn;
		}
		// Tree has only one node left - waste it.
		delete TreeStart;
		TreeStart = NULL;
	}

	// The SelRange might still contain cached indexed colours. We must delete these
	// before destroying the DocComponents.
	SelRange* Sel = GetApplication()->FindSelection();
	if (Sel) 
		Sel->AttrsHaveChanged(); 

	// Destroy doc components (colours, etc)
	// (this must be left until after all possible things in the document that could
	// contain references to components (such as IndexedColours) have been deleted)
	DestroyDocComponents();

	if (m_pExportHint)
	{
		delete m_pExportHint;
		m_pExportHint = NULL;
	}
}



/********************************************************************************************

>	void BaseDocument::DestroyDocComponents(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/4/95
	Purpose:	Deletes all DocComponents and related junk from this document
				Used by the destructor, and also by the InternalClipboard document
				when it wipes the clipboard.

	SeeAlso:	BaseDocument::InitDocComponents

********************************************************************************************/

void BaseDocument::DestroyDocComponents(void)
{
	// And delete all the document components
	if (DocComponents != NULL)
	{		
		DocComponents->DeleteAll();
		delete DocComponents;
		DocComponents = NULL;
	}
}



/********************************************************************************************

>	BOOL BaseDocument::Init()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/7/94
	Purpose:	Creates StartDocument, NodeDocument, and EndDocument nodes.
				Initialises the doc components.

********************************************************************************************/

BOOL BaseDocument::Init()
{  
	NodeDocument   *Doc;
	EndDocument    *End;

	// Get a new StartDocument node.
	TreeStart = new StartDocument;
	if (TreeStart == NULL)
		goto NoMemory;

	// Add a NodeDocument
	Doc = new NodeDocument(TreeStart, NEXT); 
	if (Doc == NULL)
		goto NoMemory;

	// Tell the NodeDocument which document it is attached to.
	Doc->SetParentDoc(this);
		
	// Join a new EndDocument node after it in the tree.
	End = new EndDocument(Doc, NEXT);
	if (End == NULL)
		goto NoMemory;

	if (!InitDocComponents())
		goto NoMemory;

	// We don't really mind if this fails but check it anyway
	m_pExportHint = new ExportHint;
	if (m_pExportHint == NULL)
		goto NoMemory;

	// All is well
	return TRUE;

NoMemory:

	// Failed to initialise due to running out of memory.
	if (TreeStart != NULL)
	{
		TreeStart->CascadeDelete();
		delete TreeStart;
		TreeStart = NULL; 
	}

	return FALSE; 
}




/********************************************************************************************

>	BOOL BaseDocument::InitDocComponents(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/4/95
	
	Returns:	TRUE if it succeeds

	Purpose:	Creates and initialises all doc components

				This is a subroutine called by Init(), and is also used by the
				InternalClipboard document when clearing the clipboard in order to wipe
				all contents of the document.

	SeeAlso:	BaseDocument::DestroyDocComponents

********************************************************************************************/

BOOL BaseDocument::InitDocComponents(void)
{
	// Get a new set of document components for this document...
	DocComponents = new List;
	if (DocComponents == NULL)
		return(FALSE);

	// Get each of the registered document components to add themselves to the document.
	if (!GetApplication()->ApplyDocComponents(this))
		return(FALSE);

	return(TRUE);
}



/***********************************************************************************************

> 	TAG BaseDocument::NewTag()

    Author: 	Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
    Created:	19/5/93
    Purpose:    Returns a new unique TAG for this Document.
                   			                                     
***********************************************************************************************/

TAG BaseDocument::NewTag()
{
	// Increment and check for loop around.
	if (TagCounter == TAG_LAST_VALID)
		TagCounter = 1; // NB 0 is not used (not sure why - Tim).
	else
		TagCounter++;

	// Return a unique tag		
	return TagCounter;
}


/********************************************************************************************

>	void BaseDocument::IncNodeCount()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/02/95
	Purpose:	Adds one to the NodesInTree counter of this Document object.  
				Called by the constructors of class Node.
	SeeAlso:	BaseDocument::DecCurrentNodeCount; BaseDocument::NodesInDocument

********************************************************************************************/

void BaseDocument::IncNodeCount()
{
	NodesInTree++;
}



/********************************************************************************************

>	void BaseDocument::DecNodeCount()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/11/93
	Purpose:	Decrements the node counter in this Document.
				Called by the destructor of class Node.
	SeeAlso:	BaseDocument::IncNodeCount; BaseDocument::NodesInDocument

********************************************************************************************/

void BaseDocument::DecNodeCount()
{
	NodesInTree--;
}



/********************************************************************************************

>	INT32 BaseDocument::NodesInDocument() const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/11/93
	Returns:	The number of nodes in this Document's tree.
	Purpose:	Returns the nodes in the document's tree, ho hum.
	SeeAlso:	BaseDocument::IncNodeCount; BaseDocument::DecNodeCount

********************************************************************************************/

INT32 BaseDocument::NodesInDocument() const
{
	return NodesInTree;
}



/***********************************************************************************************
> 	Node* BaseDocument::GetFirstNode() const

    Author: 	Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
    Created:	19/5/93
	Inputs:		- 
    Outputs:    -
    Returns:   	First node in the tree.
    Purpose:    Get the first node in the document tree - this will always be a StartDocument
    			node. 

				(* Moved from Document to BaseDocument by SimonM *)

	Errors:		None.
***********************************************************************************************/

Node* BaseDocument::GetFirstNode() const
{
	return TreeStart;
}



/********************************************************************************************

>	void BaseDocument::AddDocComponent(DocComponent *pComponent)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/08/94
	Inputs:		pComponent - the document component to add.
	Purpose:	Add the given document component to the document - no check is made to see
				if such a component is already in the document - the caller should ensure
				that they do not overwrite the existing component.
	Errors:		ENSURE of no document component list exists in the document.
	SeeAlso:	BaseDocument::GetDocComponent; BaseDocument::GetDocComponentKindOf

********************************************************************************************/

void BaseDocument::AddDocComponent(DocComponent *pComponent)
{
	// Basic sanity/safety checks.
	ENSURE(DocComponents != NULL, "No document component list in the document!");
	if (DocComponents == NULL)
		return;

	// Just add the component to the document's list.
	DocComponents->AddTail(pComponent);
}



/********************************************************************************************

>	DocComponent *BaseDocument::GetDocComponent(CCRuntimeClass *pClass)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/08/94
	Inputs:		pClass - the class of component to look for.
	Returns:	A pointer to the required component, or NULL if no match was found.
	Purpose:	Find a document component of a specific type in this document.
	SeeAlso:	BaseDocument::GetDocComponentKindOf;
				DocComponent

********************************************************************************************/

DocComponent *BaseDocument::GetDocComponent(CCRuntimeClass *pClass)
{
	DocComponent *pItem = (DocComponent *) DocComponents->GetHead();

	while (pItem != NULL)
	{
		// Is this an exact match to the given runtime class?
		if (pItem->GetRuntimeClass() == pClass)
			// Yes - return to caller.
			return pItem;

		// Try the next component
		pItem = (DocComponent *) DocComponents->GetNext(pItem);
	}

	// Didn't find the component - report this to caller
	return NULL;
}

/********************************************************************************************

>	DocComponent *BaseDocument::GetDocComponentKindOf(CCRuntimeClass *pClass)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/08/94
	Inputs:		pClass - the base class of the component to look for.
	Returns:	A pointer to the required component, or NULL if no match was found.
	Purpose:	Find a kind of document component of in this document - it must be
				derived from the class described by pClass.
	SeeAlso:	BaseDocument::GetDocComponent;
				DocComponent

********************************************************************************************/

DocComponent *BaseDocument::GetDocComponentKindOf(CCRuntimeClass *pClass)
{
	DocComponent *pItem = (DocComponent *) DocComponents->GetHead();

	while (pItem != NULL)
	{
		// Is this derived from the base class specified?
		if (pItem->IsKindOf(pClass))
			// Yes - return to caller.
			return pItem;

		// Try the next component
		pItem = (DocComponent *) DocComponents->GetNext(pItem);
	}

	// Didn't find the component - report this to caller
	return NULL;
}



/********************************************************************************************

>	DocComponent *BaseDocument::EnumerateDocComponents(DocComponent *pContext)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/08/94
	Inputs:		pContext: NULL => Get the first document component
					  non-NULL => Get the next document component after the one pointed to
								  by pContext.
	Returns:	Pointer to the required document component.
	Purpose:	Allow the caller to examine/use all the document components attached to this
				document..
				pContext should be NULL for the first call, which will return the first
				document component.  After that, the returned pointer should be passed back 
				in to this function, which will return the next one, and so on.
				For example:
				MonoOn
					DocComponent *pDocComp = NULL;
					do
					{
						pDocComp = pDoc->EnumerateDocComponents(pDocComp);

						// Do something groovy with pDocComp...
					}
					while (pDocComp != NULL);
				MonoOff

	SeeAlso:	DocComponent

********************************************************************************************/

DocComponent *BaseDocument::EnumerateDocComponents(DocComponent *pContext)
{
	if (pContext == NULL)
	{
		// Return the first item in the list...
		return (DocComponent *) (DocComponents->GetHead());
	}
	else
	{
		// Return the next document component...
		return (DocComponent *) (DocComponents->GetNext(pContext));
	}
}

/********************************************************************************************

>	virtual BOOL BaseDocument::StartComponentCopy()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/8/94
	Inputs:		-
	Outputs:	-
	Returns:	FALSE if a call to DocComponent::StartComponentCopy fails. In this case
				AbortComponentCopy gets called.
	Purpose:	This function traverses the documents list of DocComponents and calls
				the virtual StartComponentCopy method on them. It calls AbortComponent copy
				on all DocComponents if we fail.
	Errors:		-
	SeeAlso:	DocComponent::StartComponentCopy

********************************************************************************************/

BOOL BaseDocument::StartComponentCopy()
{
	 DocComponent* Current = (DocComponent*) DocComponents->GetHead();
	 DocComponent* Next; 
	 while (Current != NULL)
	 {
	 	Next = (DocComponent*) DocComponents->GetNext(Current); 
	 	if (!Current->StartComponentCopy())
	 	{
			AbortComponentCopy(); 
			return FALSE; 
	 	} 
		Current = Next; 
	 }
	 return TRUE;   
}


/********************************************************************************************

>	virtual BOOL BaseDocument::EndComponentCopy()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/8/94
	Inputs:		-
	Outputs:	-
	Returns:	FALSE if a call to DocComponent::EndComponentCopy fails. In this situation
				Abort component copy is called.
	Purpose:	This function traverses the documents list of DocComponents and calls
				the virtual EndComponentCopy method on them
	Errors:		-
	SeeAlso:	DocComponent::EndComponentCopy

********************************************************************************************/

BOOL BaseDocument::EndComponentCopy()
{
	 DocComponent* Current = (DocComponent*) DocComponents->GetHead();
	 DocComponent* Next; 
	 while (Current != NULL)
	 {
	 	Next = (DocComponent*) DocComponents->GetNext(Current); 
	 	if (!Current->EndComponentCopy())
	 	{
			AbortComponentCopy(); 
			return FALSE; 
	 	} 
		Current = Next; 
	 }  
	 return TRUE; 
} 

/********************************************************************************************

>	virtual void BaseDocument::AbortComponentCopy()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/8/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	This function traverses the documents list of DocComponents and calls
				the virtual AbortComponentCopy method on them
	Errors:		-
	SeeAlso:	DocComponent::AbortComponentCopy

********************************************************************************************/

void BaseDocument::AbortComponentCopy()
{
	 DocComponent* Current = (DocComponent*) DocComponents->GetHead();
	 DocComponent* Next; 
	 while (Current != NULL)
	 {
	 	Next = (DocComponent*) DocComponents->GetNext(Current); 
	 	Current->AbortComponentCopy(); 
		Current = Next; 
	 }  

}  


/********************************************************************************************

>	ColourList *BaseDocument::GetIndexedColours(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/05/94 (Moved here from NodeDocument 1/8/94)
	Inputs:		-
	Outputs:	-
	Returns:	A pointer to this documents list of IndexedColours
	Purpose:	Returns a pointer to a list of the IndexedColours available to the
				user within this document.
	Scope:		Private (intended only for colour manager to access)
	Errors:		-
	SeeAlso:	class IndexedColour; class ColourList

********************************************************************************************/

ColourList *BaseDocument::GetIndexedColours(void)
{
	// Get the colour list component
	ColourListComponent *pComponent = 
		(ColourListComponent *) GetDocComponent(CC_RUNTIME_CLASS(ColourListComponent));

	if(pComponent == NULL)
		return NULL;

	// Extract the colour list
	return pComponent->GetColourList();
}


/********************************************************************************************

>	ColourContextArray *BaseDocument::GetDefaultColourContexts(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/04/94
	Inputs:		-
	Outputs:	-
	Returns:	A pointer to this documents array of default colour contexts.
	Purpose:	Gives access to the Documents array of default colour contexts
				This is used internally by the DocColour system, and I shouldn't
				think it will be of much interest to anyone else.
	Errors:		-
	SeeAlso:	ColourContext

********************************************************************************************/

ColourContextArray *BaseDocument::GetDefaultColourContexts(void)
{
	return(&DefaultColourContexts);
}



/********************************************************************************************
>	BOOL BaseDocument::CheckReferences()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/01/2005
	Inputs:		-
	Outputs:	-
	Returns:	TRUE/FALSE for Success/Failure
	Purpose:	Checks for references to resources outside the document that may be dangling
				because the resources are not present.

	Errors:		-
	SeeAlso:	-
********************************************************************************************/

BOOL BaseDocument::CheckReferences()
{
	// URGH! This should be implemented in Document, not BaseDocument!
	CCamDoc* pOilDoc = NULL;
	if (IS_KIND_OF(Document))
		pOilDoc = ((Document*) this)->GetOilDoc();
	if (pOilDoc && pOilDoc->IsVisible())
	{
		// We don't want to show the user a sequence of boxes so collapse all reference
		// checks into a single warning...
		// (Argh but it's too much trouble at the mo...)
		PORTNOTETRACE("text","BaseDocument::CheckReferences - removed CheckFontSubstitution call");
#ifndef EXCLUDE_FROM_XARALX
		BOOL bFontsOK	= CheckFontSubstitution();
#endif
		BOOL bFontsOK	= true;
		BOOL bEffectsOK = CheckEffects();

#ifndef EXCLUDE_FROM_XARALX
		BOOL bFontsOK	= CheckFontSubstitution(FALSE);
		BOOL bEffectsOK = CheckEffects(FALSE);

		if (!bFontsOK || !bEffectsOK)
		{
			ErrorInfo warning;
			String_256 strWarning;
			strWarning.MakeMsg(_R(IDS_MISSINGREFERENCES), strFontsWarning, strEffectsWarning);
			warning.ErrorMsg = strWarning;
			warning.Title = String(_R(IDS_MISSINGREFS_TITLE));
			warning.Button[0] = _R(IDB_CONTINUE);
			InformWarning(warning);
		}
#endif
		return (bFontsOK && bEffectsOK);
	}

	return FALSE;
}




/********************************************************************************************
>	BOOL BaseDocument::CheckFontSubstitution()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/7/96
	Inputs:		-
	Outputs:	-
	Returns:	TRUE/FALSE for Success/Failure
	Purpose:	Checks for substituted fonts, and if it finds any, in warns the user.

				(This code has been separated from PostImport() so it can be called independantly)

	Errors:		-
	SeeAlso:	-
********************************************************************************************/

BOOL BaseDocument::CheckFontSubstitution()
{
	// check whether any fonts have been substituted
	BOOL  FontSubstitution = FALSE;
	FontList DocFonts;
	DocFonts.Build((Document*)this);
	FontListItem* FontItem = DocFonts.GetFirstItem();
	if (FontItem!=NULL)
	{
		// fill up the list
		while (FontItem)
		{
			// get the name
			String_64 Name = FontItem->GetFontName();
			FontClass Class = FontItem->GetFontClass();
			if (FONTMANAGER->IsFontReplaced(&Name, Class))
			{			
				FontSubstitution=TRUE;
				break;
			}
	 		FontItem = DocFonts.GetNextItem(FontItem);
		}
	}
#if !defined(EXCLUDE_FROM_RALPH) && !defined(EXCLUDE_FROM_XARALX)
	if (FontSubstitution)
	{
#if (_OLE_VER >= 0x200)
		// Only warn about this if the document is visible to the user.
		CCamDoc* pOilDoc = 0;
		if (IS_KIND_OF(Document)) pOilDoc = ((Document*) this)->GetOilDoc();
		if (pOilDoc && pOilDoc->IsVisible())
#endif
			// Inform the user that some fonts are not installed
			Operation::SetQuickRender(TRUE);	// Prevent cacheing while this warning is on screen
			InformWarning(_R(IDS_FONTSSUBSTITUTED), _R(IDS_CONTINUE));
			Operation::SetQuickRender(FALSE);
	}
#endif

	return TRUE;
}




/********************************************************************************************
>	BOOL BaseDocument::CheckEffects()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/01/2005
	Inputs:		-
	Outputs:	-
	Returns:	TRUE/FALSE for Success/Failure
	Purpose:	Checks for missing effects, and warn the user.

	Errors:		-
	SeeAlso:	-
********************************************************************************************/

BOOL BaseDocument::CheckEffects()
{
	ListRange* pEffectList = GetEffectsList(500000);	// Arbitrary large number
	BOOL bFound = TRUE;

	if (pEffectList)
	{
		Node* pNode = pEffectList->FindFirst();
		while (pNode)
		{
			if (pNode->IsBitmapEffect())
			{
				NodeBitmapEffect* pEffect = (NodeBitmapEffect*)pNode;

				if (!pEffect->IsLockedEffect())
				{

PORTNOTE("other","Removed XPEHost usage")
#ifndef EXCLUDE_FROM_XARALX
					// Live effects need to regenerate their bitmaps so we must
					// test whether the handler is present and warn the user if not
					String_64 strDisplayName;
					bFound = XPEHost::GetEffectDetails(pEffect->GetPostProcessorID(), &strDisplayName);
					if (!bFound)
					{
						break;
					}
#endif
				}
			}

			pNode = pEffectList->FindNext(pNode);
		}

		delete pEffectList;
	}

	if (!bFound)
	{
		Operation::SetQuickRender(TRUE);	// Prevent cacheing while this warning is on screen
		InformWarning(_R(IDS_FINFO_MISSINGEFFECTS), _R(IDS_CONTINUE));
		Operation::SetQuickRender(FALSE);
	}

	return TRUE;
}




/*******************************************************************************************

>	ListRange* BaseDocument::GetEffectsList(UINT32 iMaxItems)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/01/2005
	Inputs:		-
	Outputs:	-
	Returns:	True if completed ok, False otherwise.
	Purpose:	Updates the effects list showing referred and missing effects
	Errors:		-
	SeeAlso:	SetDocInfo; FontDropDown; DocumentFontDropDown;

*******************************************************************************************/

ListRange* BaseDocument::GetEffectsList(UINT32 iMaxItems)
{
	// Make a list of all unique effects used in the doc
	// Add each list item to the listbox, marking it if the referred effect can not
	// be found in the installed effect list

	ListRange* pEffectList = new ListRange();
	Node* pNode = GetFirstNode();
	UINT32 i = 0;
	while (pNode && i<iMaxItems)
	{
		if (pNode->IsBitmapEffect())
		{
			NodeBitmapEffect* pEffect = (NodeBitmapEffect*)pNode;

			// Search for this type of effect in the list already
			Node* pListNode = pEffectList->FindFirst();
			while (pListNode)
			{
				if (((NodeBitmapEffect*)pListNode)->GetPostProcessorID() == pEffect->GetPostProcessorID())
					break;

				pListNode = pEffectList->FindNext(pListNode);
			}

			// If this effect type is not already in the list, add it
			if (pListNode==NULL)
			{
				pEffectList->AddNode(pNode);
				i++;
			}
		}

		pNode = pNode->FindNextDepthFirst(NULL);
	}

	return pEffectList;
}




/********************************************************************************************
>	void BaseDocument::PostImport()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	04/04/95
	Inputs:		-
	Outputs:	-
	Returns:	TRUE/FALSE for Success/Failure (you what? Surely some mistake? Sack the programmer)
	Purpose:	Scans the entire tree, calling all nodes PostImport function.  If any of
				functions returns FALSE the error is reported and the scan continues.
	Errors:		-
	SeeAlso:	-
********************************************************************************************/

void BaseDocument::PostImport()
{
#ifdef RALPH
	// We don't want to be rendering at the same time as doing this, because we get
	// blobby text problems - fun, but bad !
	RalphCriticalSection RalphCS;
#endif

	BeginSlowJob();

	// Get a pointer to the StartDocument node
	Node* pStartNode = GetFirstNode();
 	ERROR3IF(pStartNode == NULL, "(BaseDocument::PostImport) TreeStart was NULL");
	
	// Get a pointer to the NodeDocument node
	if (pStartNode != NULL)
	{
		pStartNode = pStartNode->FindNext();
	 	ERROR3IF(pStartNode == NULL, "(BaseDocument::PostImport) No NodeDocument after StartDocument");
	}

	// Scan through the tree, calling all PostImport functions
	if (pStartNode != NULL)
	{
		Node* pCurrentNode = pStartNode->FindFirstDepthFirst();

		while (pCurrentNode != NULL)
		{
			if (!pCurrentNode->PostImport())
			{
	   			InformError();
			}
			pCurrentNode = pCurrentNode->FindNextDepthFirst(pStartNode);
		}
	}
	
	// check whether any fonts have been substituted, and warn the user if they have
//	CheckFontSubstitution();
	CheckReferences();

	EndSlowJob();
}

