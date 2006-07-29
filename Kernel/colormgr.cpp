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

// colormgr.cpp - Implementation of the ColourManager class


/*
*/


#include "camtypes.h"

//#include "app.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "attrmgr.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "ccolbar.h"
#include "colcomp.h"
#include "colcontx.h"
#include "coldlog.h"
#include "colormgr.h"
//#include "docmsgs.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "document.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "docview.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "jason.h"
#include "lineattr.h"
#include "newcol.h"		// For NewColourDlg
#include "nodeblnd.h"	// For NodeBlend, for Blend bodge
#include "objchge.h"
#include "opimgset.h"
//#include "ops.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "spread.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "tool.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "fillramp.h"
//#include "bitmapcache.h" - in camtypes.h [AUTOMATICALLY REMOVED]

#include "colmsg.h"

#include "ophist.h"

// "Implement" run-time typing for the classes we're defining here
CC_IMPLEMENT_DYNAMIC(ColourChangingMsg, Msg)

CC_IMPLEMENT_DYNAMIC(ColourManager, MessageHandler)

#if !defined(EXCLUDE_FROM_RALPH)
CC_IMPLEMENT_DYNCREATE(OpHideColours, UndoableOperation)
CC_IMPLEMENT_DYNCREATE(ActionHideColours, Action)

CC_IMPLEMENT_DYNCREATE(OpColourChange, UndoableOperation)
CC_IMPLEMENT_DYNCREATE(ActionColourChange, Action)

CC_IMPLEMENT_DYNCREATE(OpRedrawColours, Operation)
#endif

// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW


// --- Initialise static member variables
ColourList	*ColourManager::CurrentColourList	= NULL;
Document	*ColourManager::ScopeDocument		= NULL;

/********************************************************************************************

>	ColourManager::ColourManager()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/5/94
	Purpose:	Constructs a ColourManager object
	SeeAlso:	ColourManager; MessageHandler

********************************************************************************************/

ColourManager::ColourManager()
	: MessageHandler(CC_RUNTIME_CLASS(MessageHandler), TRUE)
{
}



/********************************************************************************************

>	static BOOL ColourManager::Init(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/5/94
	Returns:	FALSE if initialisation failed
	Purpose:	Initialises the entire colour system. This includes the colour manager,
				colour contexts, and colour dialogues, etc.
	Notes:		Should only be called once ever. Called by Application::Init()
	SeeAlso:	ColourManager; MessageHandler

********************************************************************************************/

BOOL ColourManager::Init(void)
{
#if !defined(EXCLUDE_FROM_RALPH)
	// Init (register) all our operations
#if !defined(EXCLUDE_FROM_XARALX)
	if (!::InitImagesettingOps())
		return(FALSE);
#endif

	if (!OpHideColours::Init())
		return(FALSE);

	if (!OpColourChange::Init())
		return(FALSE);

	if (!OpRedrawColours::Init())
		return(FALSE);

	if (!OpMakeColourLocalToFrame::Init())
		return(FALSE);
#endif

	// Init the global list of active colour contexts
	if (!ColourContextList::InitColourContexts())
		return(FALSE);

	return(TRUE);
}



/********************************************************************************************

>	static void ColourManager::Deinit(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/6/94
	Purpose:	De-Initlialises the entire colour system. This includes the colour manager,
				colour contexts, and colour dialogues, etc.
	Notes:		Should only be called once ever. Called by Application::Deinit()
	SeeAlso:	ColourManager; MessageHandler

********************************************************************************************/

void ColourManager::Deinit(void)
{
	// shut down the colour contexts
	ColourContextList::DeinitColourContexts();
}



/********************************************************************************************

>	void ColourManager::ForceRedrawOfChangedColours(Document *ScopeDoc,
													IndexedColour **Colours)
	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/9/94
	Inputs:		ScopeDoc - The document in which to cause redraw
				Colours  - A NULL terminated list of pointers to IndexedColours

	Purpose:	Scans the document tree of the given scope document, and causes
				a redraw of all objects which are affected by any attributes which
				contain references to any of the colours in the given NULL-terminated
				list of colours. If none of the colours are in use in the document
				tree, nothing should result.
				(i.e. it force redraws any areas of the document containing any of
				the given colours)
	SeeAlso:	RecursivelyForceRedraw

********************************************************************************************/

void ColourManager::ForceRedrawOfChangedColours(Document *ScopeDoc, IndexedColour **Colours)
{
#if !defined(EXCLUDE_FROM_RALPH)
	// Sanity check
	if (ScopeDoc == NULL)
	{
		ERROR3("Illegal NULL param");
		return;
	}

	OpDescriptor* OpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_REDRAWCOLOURS);

	if (OpDesc != NULL)
	{
		OpRedrawColoursInfo Info;
		Info.ScopeDoc = ScopeDoc;
		Info.Colours  = Colours;

		OpDesc->Invoke((OpParam *) &Info);
	}
#endif
}



/********************************************************************************************

>	virtual MsgResult ColourManager::Message(Msg* Message)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/5/94
	Inputs:		Message: The message
	Outputs:	-
	Returns:	-
	Purpose:	Process messages sent to the ColourManager
				Some messages (DocChanging) will cause new broadcasts indicating
				that the colour system has changed (new colour list has been paged in, etc)
	Errors:		-
	SeeAlso:	ColourManager; MessageHandler; ColourChangingMsg 

********************************************************************************************/

MsgResult ColourManager::Message(Msg* Message)
{
	if (MESSAGE_IS_A(Message, DocChangingMsg))
	{
		DocChangingMsg *TheMsg = (DocChangingMsg *) Message;

		switch ( TheMsg->State )
		{
			case DocChangingMsg::SELCHANGED:
				if (TheMsg->pNewDoc == NULL)			// No document selected any more
				{
					// Send PaletteChangingMsg - DESELECTED
					CurrentColourList = NULL;			// No document present
					ScopeDocument = NULL;

					BROADCAST_TO_ALL(ColourChangingMsg(ScopeDocument, CurrentColourList, NULL,
											ColourChangingMsg::LISTDESELECTED));
				}
				else if (TheMsg->pOldDoc != TheMsg->pNewDoc)
				{
					// Send PaletteChangingMsg - PAGED
					CurrentColourList = NULL;			// Cause re-caching of this info
					ScopeDocument = NULL;

					GetColourList(TRUE);				// Find the new colour list

					BROADCAST_TO_ALL(ColourChangingMsg(ScopeDocument, CurrentColourList, NULL,
											ColourChangingMsg::LISTPAGED));
				}
				break;


			case DocChangingMsg::ABOUTTODIE:
				// Pass this around as a ColourList killed message
				if (TheMsg->pChangingDoc != NULL)
					BROADCAST_TO_ALL(ColourChangingMsg(TheMsg->pChangingDoc,
										TheMsg->pChangingDoc->GetIndexedColours(), NULL,
										ColourChangingMsg::LISTDELETED));

				if (TheMsg->pChangingDoc == ScopeDocument)
				{
					// We can't keep a cached value around, as soon after this point the
					// colour list will cease to exist
					CurrentColourList = NULL;
					ScopeDocument = NULL;
				}
				break;
			default:
				break;
		}
	}
	else if (MESSAGE_IS_A(Message, ColourChangingMsg))
	{
		ColourChangingMsg *TheMsg = (ColourChangingMsg *) Message;

		switch ( TheMsg->State )
		{
			case ColourChangingMsg::LISTUPDATED:
				// The list has changed- may involve a colour having been deleted.
				// We check if the attribute manager is using any colours which are
				// now deleted, and if so, set it to use safer ones!				
				if (TheMsg->ScopeDoc == NULL || TheMsg->NewColourList == NULL)
				{
					ERROR3("ColChangingMsg::LISTUPDATED broadcast with NULL colour list");
				}
				else
					TheMsg->ScopeDoc->GetAttributeMgr().UpdateForDeletedColours(
															TheMsg->NewColourList);
				break;


			case ColourChangingMsg::COLOURUPDATED:
				{
					// An IndexedColour has been edited.
					// We need to force an update on all colours Linked to this one, and
					// then redraw all DocViews on this document.

					if (TheMsg->ScopeDoc != NULL &&
						TheMsg->NewColourList != NULL &&
						TheMsg->ChangedColour != NULL)
					{
						IndexedColour **AffectedColours = TheMsg->NewColourList->
										CompileInheritanceArray(TheMsg->ChangedColour);

						if (AffectedColours == NULL)
						{
							// Memory error means we can't get the array							
							// Tell *all* linked colours in the list they may have changed
							List *TheList = TheMsg->NewColourList;

							IndexedColour *Ptr = (IndexedColour *) (TheList->GetHead());
							while (Ptr != NULL)
							{
								if (Ptr->FindLinkedParent() != NULL)	// If is linked
									Ptr->LinkedAncestorHasChanged();	// Warn it of the change
								Ptr = (IndexedColour *) TheList->GetNext(Ptr);
							}

							// And all unnamed colours too
							TheList = TheMsg->NewColourList->GetUnnamedColours();

							Ptr = (IndexedColour *) (TheList->GetHead());
							while (Ptr != NULL)
							{
								if (Ptr->FindLinkedParent() != NULL)	// If is linked
									Ptr->LinkedAncestorHasChanged();	// Warn it of the change
								Ptr = (IndexedColour *) TheList->GetNext(Ptr);
							}

							// And force-redraw the entire document - all views
							CBitmapCache *pBC = Camelot.GetBitmapCache();
							if( NULL != pBC )
								pBC->DeInitialise();				// Brute force cache management!
							TheMsg->ScopeDoc->ForceRedraw();
						}
						else
						{
							// Inform all the linked colours (except the first, which is the
							// actual colour which has changed) that they may have changed
							INT32 Index = 1;
							while (AffectedColours[Index] != NULL)
							{
								AffectedColours[Index]->LinkedAncestorHasChanged();
								Index++;
							}

							// And redraw all affected areas of the document
							ForceRedrawOfChangedColours(TheMsg->ScopeDoc, AffectedColours);
						}
					}
				}
				break;
			default:
				break;
		}
	}

	return OK;
}      



/********************************************************************************************

>	ColourList *ColourManager::GetColourList(BOOL InvalidateCache = FALSE)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/5/94
	Inputs:		InvalidateCache - TRUE if you want to force it to re-cache the 
				list pointer (generally only used internally; the default setting
				of FALSE is all external clients should ever need to use)
	Outputs:	-
	Returns:	NULL, or a pointer to the current colour list

	Purpose:	To obtain the ColourList of IndexedColours in the SelectedDocument (the
				colours which the user can use within the document; this is the set of
				colours displayed by the colour bar and edited by the ColourMgrDlg)
				Note that an ordered list is used so that multiple display orders can
				be used (e.g. for the ColourMgrDlg and ColourBar, etc)

	Notes:		This returns the list of colours from the SELECTED document, i.e.
				the colours displayed in the colour bar etc. Use GetCurrentColourList
				if you wish to get the colours for the CURRENT doc.

				The list is cached so this call is generally pretty efficient

********************************************************************************************/

ColourList *ColourManager::GetColourList(BOOL InvalidateCache)
{
	Document *ParentDoc = Document::GetSelected();

	if (ParentDoc != NULL)
	{
		if (InvalidateCache || ParentDoc != ScopeDocument)
		{
			CurrentColourList = ParentDoc->GetIndexedColours();
			ScopeDocument = ParentDoc;
		}
		// else
		//    Cache is up to date, so quit messing about
	}
	else
	{
		ScopeDocument = NULL;		// ParentDocument is NULL - no colour list
		CurrentColourList = NULL;		
	}

	return(CurrentColourList);
}




/********************************************************************************************

>	ColourList *ColourManager::GetCurrentColourList(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/6/94
	Inputs:		-
	Outputs:	-
	Returns:	NULL, or a pointer to the current colour list

	Purpose:	To obtain the ColourList of IndexedColours in the CurrentDocument

	Notes:		This returns the colours for the CURRENT document, NOT the Selected
				one - that is, these may not be the colours displayed in the colour
				bar/input-focus document.

	SeeAlso:	ColourManager::GetColourList

********************************************************************************************/

ColourList *ColourManager::GetCurrentColourList(void)
{
	Document *ParentDoc = Document::GetCurrent();

	if (ParentDoc != NULL)
		return(ParentDoc->GetIndexedColours());

	return(NULL);
}



/********************************************************************************************

>	static ColourContext *ColourManager::GetColourContext(ColourModel Model,
															Document *ScopeDocument = NULL)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/11/95

	Inputs:		Model - The colour model of the desired ColourContext
				ScopeDocument - NULL (to retrieve a global context) or a pointer to 
				the document for which you want the colour context.

	Returns:	NULL if it failed to find an appropriate colour context, else
				a pointer to the context

	Purpose:	To easily find an appropriate colour context to use when converting
				colours. There are global contexts, but in order to supply special
				document-specific colour corrections etc, each document actually
				has its own private set of contexts. Thus, you should call this function
				to determine the most appropriate context to use.

********************************************************************************************/

ColourContext *ColourManager::GetColourContext(ColourModel Model, Document *ScopeDocument)
{
	if (ScopeDocument == NULL)
		return(ColourContext::GetGlobalDefault(Model));

	return(ScopeDocument->GetDefaultColourContexts()->Context[Model]);
}



/********************************************************************************************

>	static ColourContext *ColourManager::GetColourContext(ColourModel Model,
															View *ScopeView)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/5/96

	Inputs:		Model - The colour model of the desired ColourContext
				ScopeView - A pointer to the view for which you want the colour context.
							This should not be NULL, but if it is, a global default
							context will be returned

	Returns:	NULL if it failed to find an appropriate colour context, else
				a pointer to the context

	Purpose:	To easily find an appropriate colour context to use when converting
				colours. There are global contexts, but in order to supply special
				view-specific colour corrections etc, each document view actually
				has its own private set of contexts. Thus, you should call this function
				to determine the most appropriate context to use.

********************************************************************************************/

ColourContext *ColourManager::GetColourContext(ColourModel Model, View *ScopeView)
{
	if (ScopeView != NULL)
		return(ScopeView->GetColourContext(Model));

	return(ColourContext::GetGlobalDefault(Model));
}



/********************************************************************************************

>	static void ColourManager::SelViewContextHasChanged(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/5/96

	Purpose:	To redraw all windows which depend upon the Selected View's colour context.
				i.e. The selected view, and	also the colour editor, gallery, & line.
				(and maybe other windows in the future, e.g. object properties, grad
				fill editors, etc)

				This redraws the selected view, then broadcasts a ColourChangingMsg message
				with a state of SELVIEWCONTEXTCHANGE to inform other interested parties.

	Notes:		This should only be called when the selected view's RGB (screen) colour
				context is changed, which really shouldn't happen terribly often.

********************************************************************************************/

void ColourManager::SelViewContextHasChanged(void)
{
	View *SelView = DocView::GetSelected();

	if (SelView != NULL && SelView->IsKindOf(CC_RUNTIME_CLASS(DocView)))
	{
		CBitmapCache* pBC = Camelot.GetBitmapCache();
		if( NULL != pBC )
			pBC->DeInitialise(); 					// Brute force cache management

		((DocView *)SelView)->ForceRedraw();
	}

	BROADCAST_TO_ALL(ColourChangingMsg(NULL, NULL, NULL,
								ColourChangingMsg::SELVIEWCONTEXTCHANGE));
}



/********************************************************************************************

>	inline Document *SetCurrentDoc(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/7/94 (Copied from ccolbar.cpp, 25/11/94)
	Inputs:		-
	Outputs:	-
	Returns:	The old Current Document

	Purpose:	The ColourBar works exclusively on the SELECTED Doc.
				Thus, on entry to any of its routines which call routines
				outside this file (ccolbar.cpp), it must ensure that CurrentDoc
				is equal to SelectedDoc.
				This local macro inline does this, and returns the old setting of
				CurrentDoc so that the caller can restore the previous CurrentDoc
				on exit.

	Scope:		private (to kernel\colormgr.cpp)
	SeeAlso:	RestoreCurrentDoc

********************************************************************************************/

inline Document *SetCurrentDoc(void)
{
	Document *OldCurrentDoc = Document::GetCurrent();
	Document *NewCurrentDoc = Document::GetSelected();

	if (NewCurrentDoc != NULL && NewCurrentDoc != OldCurrentDoc)
		NewCurrentDoc->SetCurrent();

	return(OldCurrentDoc);
}



/********************************************************************************************

>	inline void RestoreCurrentDoc(Document *OldCurrentDoc)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/7/94 (Copied from ccolbar.cpp, 25/11/94)
	Inputs:		The old current document to restore
	Outputs:	-
	Returns:	-

	Purpose:	The ColourBar works exclusively on the SELECTED Doc.
				After calling SetCurrentDoc on entry to a routine, you should call
				RestoreCurrentDoc on exit to restore the old current document./

	Scope:		private (to kernel\colormgr.cpp)
	SeeAlso:	SetCurrentDoc

********************************************************************************************/

inline void RestoreCurrentDoc(Document *OldCurrentDoc)
{
	if (OldCurrentDoc != NULL)
		OldCurrentDoc->SetCurrent();
}



/********************************************************************************************

>	static BOOL ColourManager::GetCurrentLineAndFillAttrs(AttrStrokeColour **LineAttr,
														  AttrFillGeometry **FillAttr)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/7/94 (Moved from ccolbar.cpp, 25/11/94)
	Inputs:		-
	Outputs:	LineAttr - will be filled in with NULL, or pointer to the attribute
				FillAttr - will be filled in with NULL, or pointer to the attribute
	Returns:	TRUE if these are default attributes; FALSE if the attributes are
				those common to the current selection.

	Purpose:	Determines the current line and fill colour attributes.
				These are the default ones, or if there is a selection, the attrs
				which are common to all selected objects. If there are many different
				attributes of the same type associated with the selection, NULL is
				returned.
	Notes:		The ColourBar works exclusively on the SELECTED Doc. Change with care
				This function ensures that Current == Selected during its operation

********************************************************************************************/

BOOL ColourManager::GetCurrentLineAndFillAttrs(AttrStrokeColour **LineAttr,
											AttrFillGeometry **FillAttr)
{
	ENSURE(LineAttr != NULL &&  FillAttr != NULL, 
			"ColourManager::GetCurrentLineAndFillAttrs - NULL pointers passed in!");

	*LineAttr = NULL;	// Ensure safe exit if we fail
	*FillAttr = NULL;

	// I don't think it is the colour bar's responsibility to check this, but
	// this is the only way that *I* can fix the problem of an ENSURE failure 
	// on exit (we ask for info when no tools are current... I believe this is
	// because some slurphead has thrown away the tools BEFORE the document,
	// because the Document::GetSelected still does not return NULL in this case!)
	if (Tool::GetCurrent() == NULL)
		return(TRUE);

	Document *SelectedDoc = Document::GetSelected();
	if (SelectedDoc == NULL)
		return(TRUE);

	// Save the current CurrentDoc, and set (CurrentDoc = SelectedDoc)
	Document *OldCurrentDoc = SetCurrentDoc();

	SelRange *Selection = Camelot.FindSelection();
	ENSURE(Selection != NULL, "No Selection SelRange!?!");

	SelRange::CommonAttribResult result =
		Selection->FindCommonAttribute(CC_RUNTIME_CLASS(AttrStrokeColour),
										(NodeAttribute **)LineAttr);
	if (result == SelRange::ATTR_MANY)
		*LineAttr = NULL;

	result = Selection->FindCommonAttribute(CC_RUNTIME_CLASS(AttrFillGeometry),
											(NodeAttribute **)FillAttr);
	if (result == SelRange::ATTR_MANY)
		*FillAttr = NULL;

	RestoreCurrentDoc(OldCurrentDoc);

	return(result == SelRange::ATTR_NONE);
}



/********************************************************************************************

>	static BOOL ColourManager::GetCurrentLineAndFillColours(DocColour **LineCol,
															DocColour **FillCol,
															DocColour **EndGradFill = NULL,
															DocColour **EndGradFill2 = NULL,
															DocColour **EndGradFill3 = NULL)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com> (changed by Gerry 21/8/96)
	Created:	9/9/94 (Moved from ccolbar.cpp, 25/11/94)
	Inputs:		-

	Outputs:	LineCol - NULL, or will be filled in with a pointer to the current line colour
				FillCol - NULL, or will be filled in with a pointer to the current fill colour
				EndGradFill - NULL, or will be filled in with NULL or the grad-fill end colour
				(see below)
				EndGradFill2 - NULL, or will be filled in with NULL or the second end colour
				EndGradFill3 - NULL, or will be filled in with NULL or the third end colour

	Returns:	TRUE if these are default attributes; FALSE if the attributes are
				those common to the current selection.

	Purpose:	Finds the current line and fill attributes, and dereferences them to find
				the current line and fill DocColours. These may be returned NULL if there
				are no current attributes (no document/many colours selected)

	Notes:		The ColourBar works exclusively on the SELECTED Doc. Change with care

				LineCol and/or FillCol may be passed in NULL if you are not interested in
				having these results returned to you.

				If EndGradFill is NULL and a grad fill is selected then FillColour will
				be filled in with the selected fill blob's colour (or NULL if there are
				zero or more than one selected blob).  EndGradFill2 and EndGradFill3
				will be filled in with NULL (if they are non NULL)
  
				If EndGradFill is non-NULL and a grad-fill is selected, instead of
				returning only the selected endpoint colour, it returns the start 
				and end colours in FillCol (start) and EndGradFill (end).  If the fill
				has more than two colours (ie GetEndColour2() != NULL) then both 
				FillCol and EndGradFill will bet filled in with NULL (to indicate many).

				MULTIFILL: This system needs to be changed to support multistage fills.

	SeeAlso:	CColourBar::GetCurrentLineAndFillAttrs

********************************************************************************************/

BOOL ColourManager::GetCurrentLineAndFillColours(DocColour **LineCol, DocColour **FillCol,
													DocColour **EndGradFill)
{
	if (LineCol != NULL)			// Ensure return NULL if we fail
		*LineCol = NULL;

	if (FillCol != NULL)
		*FillCol = NULL;

	if (EndGradFill != NULL)
		*EndGradFill = NULL;

	AttrStrokeColour *LineAttr = NULL;
	AttrFillGeometry *FillAttr = NULL;

	BOOL result = GetCurrentLineAndFillAttrs(&LineAttr, &FillAttr);

	if (LineAttr != NULL && LineCol != NULL)
		*LineCol = LineAttr->GetStartColour();

	if (FillAttr != NULL && FillCol != NULL)
	{
		// New bit added by will (3/11/94) to make colour bar show
		// the selected fill control point's colour.
		// Note this assumes there are only start and end colours,
		// and so will NOT work with multi-stage fills.

		if (FillAttr->GetRuntimeClass() == CC_RUNTIME_CLASS(AttrFlatColourFill))
		{
			*FillCol = FillAttr->GetStartColour();
		}
		else
		{			
			if (EndGradFill == NULL)
			{
				// The caller wants the selected point's colour or NULL (already set)
				
				if (FillAttr->GetSelectionCount() == 1)						// If there is only 1 selected blob
				{
					// Get the colour of the selected blob
					
					if (FillAttr->IsSelected(FILLCONTROL_STARTPOINT))		// Show Start Colour
						*FillCol = FillAttr->GetStartColour();
					else if (FillAttr->IsSelected(FILLCONTROL_ENDPOINT))	// Show End Colour
						*FillCol = FillAttr->GetEndColour();
					else if (FillAttr->IsSelected(FILLCONTROL_ENDPOINT2))	// Show End2 Colour
						*FillCol = FillAttr->GetEndColour2();
					else if (FillAttr->IsSelected(FILLCONTROL_ENDPOINT3))	// Show End3 Colour
						*FillCol = FillAttr->GetEndColour3();
				}
			}
			else
			{
				// User wants two colours

				if (FillAttr->GetEndColour2() == NULL)		// If the fill doesn't have a third colour
				{					
					// We know FillCol is non-NULL
					*FillCol = FillAttr->GetStartColour();

					// We know EndGradFill is non-NULL
					*EndGradFill = FillAttr->GetEndColour();
				}
			}
		}
	}

	return(result);
}



/********************************************************************************************

>	static void ColourManager::FindColourOfInterestToUser(IndexedColour **ResultCol,
														ColourList **ResultList = NULL)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/1/95
	Inputs:		-
	Outputs:	ResultCol  - NULL, or a pointer to the colour to use
				ResultList - NULL, or a pointer to the ColourList that colour resides in
	Returns:	-

	Purpose:	Several things (Colour editor whenever the selection changes, generation
				of a new colour) need to know a 'useful' colour to display/copy. 
				Rather than just showing the current default colour or something, they
				need to interactively update to show an appropriate colour from the
				selection and suchlike.

				NOTE that there is another overloaded form of this method, which returns
				a DocColour istead. This method returns a pointer to a NAMED IndexedColour,
				so may return an entirely different colour to the other function.

				The exact colour returned will be NULL (in dire emergency, usually only
				occurs if there are no documents open), or will be chosen from this
				priority list of possible contenders:
				MonoOn
					(1) Colour of first selected fill-blob found,
					(2) Fill-Colour of selection,
					(3) current/default fill colour,
					(4) line-Colour of selection,
					(5) current/default line colour,
					(6) the first active named colour in the document's colour list,
					(7) NULL
				MonoOff

	Notes:		'Colour of selection' is determined by using (if possible) the last
				object selected, or the frontmost selected object if this fails.

				If you are not interested in the ResultList return value, pass in NULL
				(use the default) for this parameter.

				In debug builds only, extra checks are made which can cause an ERROR3
				if the colour found was not in the selected document's ColourList.

********************************************************************************************/

void ColourManager::FindColourOfInterestToUser(IndexedColour **ResultCol, ColourList **ResultList)
{
	ERROR3IF(ResultCol == NULL, "ColourManager::FindColourOfInterestToUser - ResultCol is NULL!");

	// Ensure safe return values
	*ResultCol = NULL;
	if (ResultList != NULL)
		*ResultList = NULL;

	// If there are no documents available, then we can't find a colour - Return NULLs
	if (Document::GetSelected() == NULL)
		return;

	// Ensure CurrentDoc == SelectedDoc
	Document *OldCurrentDoc = SetCurrentDoc();

	// Get the colour list. Just in case, we invalidate the cache (as this method is
	// called on document chnages and suchlike, so it is best to make sure we don't use
	// cached values for a no-longer-present document!)
	ColourList *ColList = ColourManager::GetColourList(TRUE);
	IndexedColour *UsefulColour = NULL;

	// First, check if there is a VISIBLE selected fill blob, and use the colour of
	// the first one that we find, if we are lucky enough to find one.
	if (AttrFillGeometry::CountSelectionControlPoints() > 0)
	{
		// Yep. There is at least 1 visible selected fill blob...
		AttrFillGeometry* Fill = AttrFillGeometry::FindFirstSelectedAttr();
		while (Fill != NULL && UsefulColour == NULL)
		{
			if (Fill->GetSelectionCount() > 0)
			{
				if (Fill->IsSelected(FILLCONTROL_STARTPOINT) && Fill->GetStartColour() != NULL)
				{
					// Use Fill Start Colour
					UsefulColour = Fill->GetStartColour()->FindParentIndexedColour();
					break;
				}

				if (Fill->IsSelected(FILLCONTROL_ENDPOINT) && Fill->GetEndColour() != NULL)
				{
					// Use Fill End Colour
					UsefulColour = Fill->GetEndColour()->FindParentIndexedColour();
					break;
				}

				if (Fill->IsSelected(FILLCONTROL_ENDPOINT2) && Fill->GetEndColour2() != NULL)
				{
					// Use Fill End Colour
					UsefulColour = Fill->GetEndColour2()->FindParentIndexedColour();
					break;
				}

				if (Fill->IsSelected(FILLCONTROL_ENDPOINT3) && Fill->GetEndColour3() != NULL)
				{
					// Use Fill End Colour
					UsefulColour = Fill->GetEndColour3()->FindParentIndexedColour();
					break;
				}
			}				

			Fill = AttrFillGeometry::FindNextSelectedAttr();
		}
	}

	if (UsefulColour == NULL)
	{
		// Find the last node which was made selected (if any)
		// Scan back from this to find the nearest previous NodeAttribute which contains
		// colour(s), and make UsefulColour the first IndexedColour which we find in this search.
		SelRange *Selection = GetApplication()->FindSelection();

		if (Selection != NULL)
		{
			Node *pNode = Selection->GetLastSelectedNode();
			// If the SelRange doesn't know what was last made selected, then see if anything
			// is selected, and if so, grab the last (topmost) selected object instead
			if (pNode == NULL)
				pNode = Selection->FindLast();

			// Now search through the attributes applied to this node, to see if we can
			// find an IndexedColour FILL (i.e. we ignore stroke colours for now) - we
			// will use the first one that we find
			NodeAttribute *pAttr = NULL;
			if (pNode != NULL)
				pAttr = NodeAttribute::FindFirstAppliedAttr(pNode);

			while (pAttr != NULL && UsefulColour == NULL)
			{
				// Scan each NodeAttribute in turn to see if any of them contain any colours
				DocColour *pColour;
				UINT32 Context = 0;

				// Only check non-line-colour attributes
				if (!(IS_A(pAttr, AttrStrokeColour)))
				{
					do
					{
						// Get the next colour field (if any) from the attribute, and find the
						// IndexedColour (if any) to which it refers
						pColour = pAttr->EnumerateColourFields(Context++);
						if (pColour != NULL)
							UsefulColour = pColour->FindParentIndexedColour();

					} while (pColour != NULL && UsefulColour == NULL);
				}
			
				pAttr = NodeAttribute::FindPrevAppliedAttr(pAttr);
			}


			// If we still haven't found a colour, try for a line/stroke colour instead
			if (pNode != NULL && UsefulColour == NULL)
				pAttr = NodeAttribute::FindFirstAppliedAttr(pNode);

			while (pAttr != NULL && UsefulColour == NULL)
			{
				// Scan each NodeAttribute in turn to see if any of them contain any colours
				DocColour *pColour;
				UINT32 Context = 0;

				// Only check line-colour attributes
				if (IS_A(pAttr, AttrStrokeColour))
				{
					do
					{
						// Get the next colour field (if any) from the attribute, and find the
						// IndexedColour (if any) to which it refers
						pColour = pAttr->EnumerateColourFields(Context++);
						if (pColour != NULL)
							UsefulColour = pColour->FindParentIndexedColour();

					} while (pColour != NULL && UsefulColour == NULL);
				}

				pAttr = NodeAttribute::FindPrevAppliedAttr(pAttr);
			}

		}
	}

	if (UsefulColour == NULL)
	{
		// OK, then. let's see if there is a default fill colour, or failing that
		// (eg it could be 'no colour') a default line colour, which we can edit.
		AttributeManager &AttrMgr = Document::GetSelected()->GetAttributeMgr();

		DocColour LineCol;
		DocColour FillCol;
			
		AttrMgr.GetCurrentLineAndFillColour(CC_RUNTIME_CLASS(NodeRenderableInk),
												&LineCol, &FillCol);

		UsefulColour = FillCol.FindParentIndexedColour();
		if (UsefulColour == NULL)
			UsefulColour = LineCol.FindParentIndexedColour();
	}

	if (UsefulColour == NULL)
	{
		// Ok, we're rapidly approaching a moment of panic. Try the first non-deleted named
		// colour in the selected document's colour list. If nothing else, there should
		// always be a default Black that we can use

		if (ColList != NULL)
		{
			IndexedColour *Ptr = (IndexedColour *) ColList->GetHead();
			while (Ptr != NULL && UsefulColour == NULL)
			{
				if (Ptr->IsNamed() && !Ptr->IsDeleted())
					UsefulColour = Ptr;
				
				Ptr = (IndexedColour *) ColList->GetNext(Ptr);
			}
		}
	}

	// Sanity check. Should never ever ever be the case, ever.
	ERROR3IF(UsefulColour->IsDeleted(),
				"ColourManager::FindColourOfInterestToUser - Colour found is DELETED!");

	// Fill in the return results
	*ResultCol = UsefulColour;
	if (UsefulColour != NULL && ResultList != NULL)
		*ResultList = ColList;

#ifdef _DEBUG
	// In debug builds, verify that the returned colour is in the list we
	// expect it to be in!

	if (ColList != NULL && UsefulColour != NULL)
	{
		IndexedColour *Ptr = (IndexedColour *) ColList->GetHead();
		while (Ptr != NULL && Ptr != UsefulColour)
			Ptr = (IndexedColour *) ColList->GetNext(Ptr);

		if (Ptr == NULL)
		{
			Ptr = (IndexedColour *) ColList->GetUnnamedColours()->GetHead();
			while (Ptr != NULL && Ptr != UsefulColour)
				Ptr = (IndexedColour *) ColList->GetUnnamedColours()->GetNext(Ptr);
		}

		ERROR3IF(Ptr != UsefulColour,
				"ColourManager::FindColourOfInterestToUser"
				" - Colour found is not in the current colour list!");
	}
#endif

	// And restore the previous CurrentDocument state
	RestoreCurrentDoc(OldCurrentDoc);
}



/********************************************************************************************

>	static void ColourManager::FindColourOfInterestToUser(DocColour *ResultCol,
														ColourList **ResultList = NULL,
														BOOL WantLineColour = FALSE)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/1/95
	Inputs:		-
	Outputs:	ResultCol  - will be returned filled in with details of the colour to use
				ResultList - NULL, or a pointer to the ColourList of the relevant document
				WantLinecolour - TRUE if you'd rather get a line colour, FALSE if you want
				a fill colour.
	Returns:	-

	Purpose:	Several things (Colour editor whenever the selection changes, generation
				of a new colour) need to know a 'useful' colour to display/copy. 
				Rather than just showing the current default colour or something, they
				need to interactively update to show an appropriate colour from the
				selection and suchlike.

				This is very similar to the other overloaded method, but returns a DocColour.
				NOTE however, that the returned colour may not match the IndexedColour
				returned by the other method - a DocColour (which does not reference
				a NAMED IndexedColour) may be returned before a suitable IndexedColour
				would have been found.

				This is used to determine the colour definition for a colour to use when
				editing "local colours" in the colour editor.

				The exact colour returned will be NULL (in dire emergency, usually only
				occurs if there are no documents open), or will be chosen from this
				priority list of possible contenders:
				MonoOn
					(1) Colour of first selected fill-blob found (if !WantLineColour),
					(2) Fill-Colour of selection (if !WantLineColour),
					(3) current/default fill colour (if !WantLineColour),
					(4) line-Colour of selection (If WantLineColour),
					(5) current/default line colour (if WantLineColour),
					(6) COLOUR_BLACK
				MonoOff

	Notes:		'Colour of selection' is determined by using (if possible) the last
				object selected, or the frontmost selected object if this fails.

				If you are not interested in the ResultList return value, pass in NULL
				(use the default) for this parameter.

				Camelot will not return a 'no fill' default colour (conditions 3 and 6) but
				Webster does because the Webster colour editor wants to handle this
				situation.

********************************************************************************************/

void ColourManager::FindColourOfInterestToUser(DocColour *ResultCol, ColourList **ResultList,
												BOOL WantLineColour)
{
	ERROR3IF(ResultCol == NULL, "ColourManager::FindColourOfInterestToUser - ResultCol is NULL!");

	// Ensure safe return values
	*ResultCol = DocColour(COLOUR_BLACK);
	if (ResultList != NULL)
		*ResultList = NULL;

	// If there are no documents available, then we can't find a colour - Return NULLs
	if (Document::GetSelected() == NULL)
		return;

	// Ensure CurrentDoc == SelectedDoc
	Document *OldCurrentDoc = SetCurrentDoc();

	// Get the colour list. Just in case, we invalidate the cache (as this method is
	// called on document chnages and suchlike, so it is best to make sure we don't use
	// cached values for a no-longer-present document!)
	ColourList *ColList = ColourManager::GetColourList(TRUE);
	BOOL FoundUsefulColour = FALSE;

	// First, check if there is a VISIBLE selected fill blob, and use the colour of
	// the first one that we find, if we are lucky enough to find one.
	if (!WantLineColour && AttrFillGeometry::CountSelectionControlPoints() > 0)
	{
		// Yep. There is at least 1 visible selected fill blob...
		AttrFillGeometry* Fill = AttrFillGeometry::FindFirstSelectedAttr();
		while (Fill != NULL && !FoundUsefulColour)
		{
			if (Fill->GetSelectionCount() > 0)
			{
				// CGS ....

				// DMc's multi stage fills (blobs) do not dynamically update the colour edit
				// dialog when they are clicked on.  This has now been
				// fixed (by me!).  Problem was to do with the fact that this function never
				// even considered them ....

				FillRamp* pFillRamp = Fill->GetFillRamp ();

				if (pFillRamp != NULL)
				{
					UINT32 NumberSelected = pFillRamp->CountSelBlobs ();
					
					if (NumberSelected > 0)
					{
						INT32 SelectedIndex = pFillRamp->GetSelectedIndex ();
						ColRampItem* TheItem = (ColRampItem*) pFillRamp->GetValidIndexedItem (SelectedIndex);

						if (TheItem != NULL)
						{
							FoundUsefulColour = TRUE;
							*ResultCol = TheItem->GetColour ();
						}
					}
				}
				
				if (Fill->IsSelected(FILLCONTROL_STARTPOINT) && Fill->GetStartColour() != NULL)
				{
					// Use Fill Start Colour
					FoundUsefulColour = TRUE;
					*ResultCol = *(Fill->GetStartColour());
					break;
				}

				if (Fill->IsSelected(FILLCONTROL_ENDPOINT) && Fill->GetEndColour() != NULL)
				{
					// Use Fill End Colour
					FoundUsefulColour = TRUE;
					*ResultCol = *(Fill->GetEndColour());
					break;
				}

				if (Fill->IsSelected(FILLCONTROL_ENDPOINT2) && Fill->GetEndColour2() != NULL)
				{
					// Use Fill End Colour
					FoundUsefulColour = TRUE;
					*ResultCol = *(Fill->GetEndColour2());
					break;
				}

				if (Fill->IsSelected(FILLCONTROL_ENDPOINT3) && Fill->GetEndColour3() != NULL)
				{
					// Use Fill End Colour
					FoundUsefulColour = TRUE;
					*ResultCol = *(Fill->GetEndColour3());
					break;
				}
			}				

			Fill = AttrFillGeometry::FindNextSelectedAttr();
		}
	}

	if (!FoundUsefulColour)
	{
		// Find the last node which was made selected (if any)
		// Scan back from this to find the nearest previous NodeAttribute which contains
		// colour(s), and make ResultColour ref the first IndexedColour which we find in this search.
		SelRange *Selection = GetApplication()->FindSelection();

		// If there are any selected nodes, then...
		if (Selection != NULL && Selection->Count() > 0)
		{
			Node *pNode = Selection->GetLastSelectedNode();

			// If the SelRange doesn't know what was last made selected, then see if anything
			// is selected, and if so, grab the last (topmost) selected object instead
			if (pNode == NULL)
				pNode = Selection->FindLast();

			// Now search through the attributes applied to this node, to see if we can
			// find an IndexedColour FILL (i.e. we ignore stroke colours for now) - we
			// will use the first one that we find
			NodeAttribute *pAttr = NULL;
			
			if (!WantLineColour)
			{
				if (pNode != NULL)
					pAttr = NodeAttribute::FindFirstAppliedAttr(pNode);

				while (pAttr != NULL && !FoundUsefulColour)
				{
					// Scan each NodeAttribute in turn to see if any of them contain any colours
					DocColour *pColour;
					UINT32 Context = 0;

					// Only check non-line-colour attributes
					if (!(IS_A(pAttr, AttrStrokeColour)))
					{
						do
						{
							// Get the next colour field (if any) from the attribute, and find the
							// IndexedColour (if any) to which it refers
							pColour = pAttr->EnumerateColourFields(Context++);
							if (pColour != NULL)
							{
								FoundUsefulColour = TRUE;
								*ResultCol = *pColour;
							}

						} while (pColour != NULL && !FoundUsefulColour);
					}
			
					pAttr = NodeAttribute::FindPrevAppliedAttr(pAttr);
				}
			}
			else
			{
				// If we still haven't found a colour, try for a line/stroke colour instead
				if (pNode != NULL && !FoundUsefulColour)
					pAttr = NodeAttribute::FindFirstAppliedAttr(pNode);

				while (pAttr != NULL && !FoundUsefulColour)
				{
					// Scan each NodeAttribute in turn to see if any of them contain any colours
					DocColour *pColour;
					UINT32 Context = 0;

					// Only check line-colour attributes
					if (IS_A(pAttr, AttrStrokeColour))
					{
						do
						{
							// Get the next colour field (if any) from the attribute, and find the
							// IndexedColour (if any) to which it refers
							pColour = pAttr->EnumerateColourFields(Context++);
							if (pColour != NULL)
							{
								FoundUsefulColour = TRUE;
								*ResultCol = *pColour;
							}

						} while (pColour != NULL && !FoundUsefulColour);
					}

					pAttr = NodeAttribute::FindPrevAppliedAttr(pAttr);
				}
			}

		}
	}

	if (!FoundUsefulColour)
	{
		// OK, then. let's see if there is a default fill colour, or failing that
		// (eg it could be 'no colour') a default line colour, which we can edit.
		AttributeManager &AttrMgr = Document::GetSelected()->GetAttributeMgr();

		DocColour LineCol;
		DocColour FillCol;
			
		AttrMgr.GetCurrentLineAndFillColour(CC_RUNTIME_CLASS(NodeRenderableInk),
												&LineCol, &FillCol);

		if (!WantLineColour)
		{
// Webster wants the colour even if it is 'no fill'
#ifndef WEBSTER
			if (!FillCol.IsTransparent())
#endif //WEBSTER
			{
				*ResultCol = FillCol;
				FoundUsefulColour = TRUE;
			}
		}
		else
		{
#ifndef WEBSTER
			if (!LineCol.IsTransparent())
#endif //WEBSTER
			{
				*ResultCol = LineCol;
				FoundUsefulColour = TRUE;
			}
		}
	}

	if (!FoundUsefulColour)
	{
		// Ok, we're rapidly approaching a moment of panic. Try the first non-deleted named
		// colour in the selected document's colour list. If nothing else, there should
		// always be a default Black that we can use

		if (ColList != NULL)
		{
			IndexedColour *Ptr = (IndexedColour *) ColList->GetHead();
			while (Ptr != NULL && !FoundUsefulColour)
			{
				if (Ptr->IsNamed() && !Ptr->IsDeleted())
				{
					FoundUsefulColour = TRUE;
					ResultCol->MakeRefToIndexedColour(Ptr);
				}
				
				Ptr = (IndexedColour *) ColList->GetNext(Ptr);
			}
		}
	}

	// Fill in the return results
	if (ResultList != NULL)
		*ResultList = ColList;

	// And restore the previous CurrentDocument state
	RestoreCurrentDoc(OldCurrentDoc);
}



/********************************************************************************************

>	static IndexedColour *ColourManager::GenerateNewNamedColour(ColourList *ParentList,
																IndexedColour *SourceColour)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/11/94
	Inputs:		ParentList - The list in which the SourceColour resides - The resulting
				colour will also be placed into this list. May NOT be NULL.
			 	SourceColour - NULL (to copy the current attribute FillColour), or a pointer
				to the colour you wish to duplicate.
			
	Outputs:	(ParentList - will contain the new colour if all went well)
	Returns:	NULL if it failed, else
				A clone of SourceColour. This will be pretty much identical to SourceColour,
				except that it will always be a NAMED colour.

	Purpose:	Creates a new named IndexedColour. If possible, this colour is copied from
				the given SourceColour, but if this is not available, an attempt to find
				a useful colour to copy will be made - if this fails, a battleship grey
				will be created. i.e. apart from abject failure, this always creates a
				new colour.

				It generates a new colour, adds it to the ParentList, and generates an
				UNDO record for the action; this includes a ColourListChanged msg broadcast.

				NOTE that the user is prompted for a new name, and has the opportunity
				to cancel creation of the colour, in which case, NULL will be returned.
				You must therefore handle a NULL return *quietly*.
	
	Notes:		This code is used by the ColourBar and ColourGallery "new colour" code
				to generate a new named colour.

	Errors:		If the return value is NULL, an unreported memory allocation failure has
				occurred.

				If the ParentList is not the ColourList for the Selected document, and
				no source colour is given, chances are FindColourOfInterestToUser will
				get upset and ERROR3.
	
	SeeAlso:	IndexedColour::IndexedColour; ColourManager::UnHideColours;
				ColourManager::FindColourOfInterestToUser

********************************************************************************************/

IndexedColour *ColourManager::GenerateNewNamedColour(ColourList *ParentList,
													 IndexedColour *SourceColour)
{
	ERROR3IF(ParentList == NULL, "ColourManager::GenerateNamedColour - NULL ParentList!");

	IndexedColour *NewCol = NULL;

	if (SourceColour != NULL)
	{
		// Copy the source colour
		NewCol = new IndexedColour(*SourceColour);
	
		// If parent is named and not tint/linked colour, remember where we copied
		// it from so that we can give a useful default parent in the colour editor
		// if the user decides to make it a tint/linked colour later on.
		if (NewCol->IsNamed() && NewCol->FindLinkedParent() == NULL)
			NewCol->SetLinkedParent(SourceColour, NewCol->GetType());
	}
	else
	{
		IndexedColour *UsefulColour;
		FindColourOfInterestToUser(&UsefulColour);

		if (UsefulColour != NULL)
		{
			NewCol = new IndexedColour(*UsefulColour);
	
			// If parent is named and not tint/linked colour, remember where we copied
			// it from so that we can give a useful default parent in the colour editor
			// if the user decides to make it a tint/linked colour later on.
			if (NewCol->IsNamed() && NewCol->FindLinkedParent() == NULL)
				NewCol->SetLinkedParent(UsefulColour, NewCol->GetType());
		}
		else
		{
			// Create a mid-grey HSV colour for them to edit
			ColourHSVT Bob;
			Bob.Hue = Bob.Saturation = Bob.Transparent = 0;
			Bob.Value = FIXED24(0.5);

			NewCol = new INDEXEDCOLOUR_HSVT(&Bob);
		}
	}

	if (NewCol == NULL)		// Serious memory failure!
		return(NULL);

	// Set the colour's name (1 to make it named, and 2 so it is not
	// the same as the colour it was copied from)
/*
	String_64 NewName;

	if (NewCol->IsNamed())
	{
		// Is named, so is a copy: Name it "New Colour (Copy of WXYZ)"
		String_64 OldName = *NewCol->GetName();
		String_64 TruncOldName;					// Truncate old name to 32 chars
		OldName.Left(&TruncOldName, 32);		// and add it into the new name
		NewName.MakeMsg(_R(IDS_NEWCOLOUR_COPY), (TCHAR *)TruncOldName);
	}
	else
		NewName.MakeMsg(_R(IDS_NEWCOLOUR));			// No name, so make it named: "New colour"
*/

#if !defined(EXCLUDE_FROM_RALPH)
#ifndef STANDALONE
	// Ask for a new name for the colour. This also gives the user a chance
	// to back out of the deal without losing their deposit (though if they didn't
	// tick the small box in the small print, we'll continue sending them junk mail ;-)
	if (!NewColourDlg::InvokeDialog(ParentList, NewCol, NEWCOLOUR_MUSTBENAMED))
	{
		delete NewCol;		// Bummer. They cancelled their subscription
		return(NULL);
	}
#endif
#endif

//	NewCol->SetName(NewName);

	// Add the new colour (also ensuring that the colour has a unique name)
	ParentList->AddItem(NewCol);

	// Create an undo record, but only if it is a colour style (named)
	IndexedColour *UnHideList[2];
	UnHideList[0] = NewCol;
	UnHideList[1] = NULL;
	ColourManager::UnHideColours(ParentList, UnHideList);

	return(NewCol);
}



/********************************************************************************************

>	static IndexedColour *ColourManager::GenerateNewUnnamedColour(ColourList *ParentList,
																DocColour *SourceColour)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/3/95

	Inputs:		ParentList - The list in which the resulting colour will be placed.
				In debug builds an ENSURE will be triggered if the new colour would be
				created with a parent-colour in a different colour list.
				May NOT be NULL.

			 	SourceColour - A pointer to the colour you wish to duplicate.
				If NULL, some random "useful" colour will be substituted.
			
	Outputs:	(ParentList - will contain the new colour if all went well)
	Returns:	NULL if it failed, else
				A copy of SourceColour. This will be pretty much identical to SourceColour,
				except that it will always be an UNNAMED IndexedColour.

	Purpose:	Creates a new named IndexedColour. If possible, this colour is copied from
				the given SourceColour, but if this is not available, an attempt to find
				a useful colour to copy will be made - if this fails, a battleship grey
				will be created. i.e. apart from abject failure, this always creates a
				new colour.

				It generates a new colour & adds it to the ParentList unnamed-colours-list

	Notes:		This code is used by the Colour Editor to generate a new unnamed colour
				when editing "local colours".

				Unnamed colours are auto-destructed when their usagecount is decremented
				to zero, so you don't generally need to clean up these colours.

				When copying a colour style as an unnamed colour, we force it to be 'normal'
				This is so that initial attempts to edit the colour of an object delink
				it from its parent, and allow the colour to be freely edited. When editing
				the local colour, this force won't occur, which allows the user to specifically
				make locals which are links/tints.

	Errors:		If the return value is NULL, an unreported memory allocation failure has
				occurred.
	
	SeeAlso:	IndexedColour::IndexedColour; ColourManager::UnHideColours;
				ColourManager::FindColourOfInterestToUser

********************************************************************************************/

IndexedColour *ColourManager::GenerateNewUnnamedColour(ColourList *ParentList,
														DocColour *BaseSourceColour)
{
	if (ParentList == NULL)
	{
		ERROR3("ColourManager::GenerateNewUnnamedColour - NULL ParentList!");
		return(NULL);
	}

	IndexedColour *NewCol = NULL;

	DocColour SourceColour(COLOUR_BLACK);
	if (BaseSourceColour == NULL)
		FindColourOfInterestToUser(&SourceColour);		// No supplied colour, so find one
	else
		SourceColour = *BaseSourceColour;				// Use the supplied colour

	IndexedColour *SourceIxCol = SourceColour.FindParentIndexedColour();
	if (SourceIxCol == NULL)
	{
		// Copy the definition of a DocColour
		ColourGeneric ColDef;
		SourceColour.GetSourceColour(&ColDef);

		NewCol = new IndexedColour(SourceColour.GetColourModel(), &ColDef);
	}
	else
	{
		// Copy an IndexedColour
		NewCol = new IndexedColour(*SourceIxCol);

		if (NewCol != NULL)
		{
			NewCol->SetUnnamed();	// Ensure the colour is unnamed

			// When copying a colour style as an unnamed colour, we force it to be 'normal'
			// This is so that initial attempts to edit the colour of an object delink
			// it from its parent, and allow the colour to be freely edited. When editing
			// the local colour, this force won't occur, which allows the user to specifically
			// make locals which are links/tints.
			// NOTE that we make the source colour our parent, so that subsequent attempts
			// to edit this colour will choose a sensible parent.
	//		if (SourceIxCol->IsNamed())
	//		{
	// We no longer do this (Jason - 18/10/95)
	// This was a fix to the fact that the user could not edit tints/linked colours in the "simple"
	// colour editor, and thus could get trapped in a very confusing uneditable state.
	// These days they have both the ability to edit all colour types in the simple mode,
	// and the extra status line and constraint-display stuff to make it more obvious why they
	// can't change stuff when they can't.
	//			NewCol->SetLinkedParent(SourceIxCol, COLOURTYPE_NORMAL);
	//
	//		}

			// We have copied the Parent colour (hint) from the copied colour.
			// However, if this is a "normal" (unlinked) colour, we really want to set
			// the parent hint to be the colour we were copied from.
			// (But we can only use the source colour if it's named and "alive")
			if (NewCol->FindLinkedParent() == NULL && SourceIxCol->IsNamed() && !SourceIxCol->IsDeleted())
				NewCol->SetLinkedParent(SourceIxCol, NewCol->GetType());
		}
	}

	// Check that the parent colour of our new one is a legal parent (named, not deleted, and exists
	// in the target document)
	if (NewCol != NULL)
	{
		IndexedColour *Parent = NewCol->FindLastLinkedParent();
		if (Parent != NULL)
		{
			if (Parent->IsDeleted() || !Parent->IsNamed())
				Parent = NULL;			// Deleted or unnamed - forget it

			if (Parent != NULL)
			{
				// Check that the parent colour is in the same colour list as the NewCol will be going
				IndexedColour *Ptr = ParentList->GetUndeletedHead();
				while (Ptr != NULL)
				{
					if (Ptr == Parent)
						break;

					Ptr = ParentList->GetUndeletedNext(Ptr);
				}

				if (Ptr != Parent)
					Parent = NULL;
			}

			// Nope - we haven't got a safe parent, so vape the parent field altogether
			if (Parent == NULL)
			{
				ERROR3("Warning: Had to correct colour with illegal parent pointer");
				NewCol->SetLinkedParent(NULL, COLOURTYPE_NORMAL);
			}
		}

		// Add the new colour (also ensuring that the colour has a unique name)
		ParentList->GetUnnamedColours()->AddTail(NewCol);
	}

	return(NewCol);
}




/********************************************************************************************

>	static void ColourManager::EnsureColourIsInDocument(Document *SourceDoc,
															Document *DestDoc,
															DocColour *SourceAndResultCol)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/4/95 (Moved to ColourManager, 20/11/95)

	Inputs:		SourceDoc - NULL, or the document which contains the source colour

				DestDoc - The document in which you want to use the colour (NOT NULL)

				ResultCol - should point at a DocColour defining the colour you want
				to put in this document. It may be an immediate DocColour, or a reference
				to an IndexedColour. (this param may NOT be NULL)

	Outputs:	ResultCol (which may not be NULL) will be returned with a colour to
				use. This will either be COLOUR_TRANS, or will point at an IndexedColour
				in the SELECTED document which you may apply in that document.

	Purpose:	Ensures that the given DocColour references a "safe" colour to use within
				the given document. This may mean doing nothing to the colour, or it may
				be necessary to copy the colour (and possibly linked parent colour(s) as well!)
				into the DestDoc. Whatever happens, ResultCol will be filled in with something
				that you can use with confidence.

	Notes:		Uses the component copy system to copy colours across with merging.

				What do you mean this should be in the colour manager? Have you seen how many
				files it rebuilds?! Eeek! (OK, so I've moved it now)

	SeeAlso:	ColourListComponent::StartComponentCopy

********************************************************************************************/

void ColourManager::EnsureColourIsInDocument(Document *SourceDoc, Document *DestDoc,
												DocColour *ResultCol)
{
	ERROR3IF(ResultCol == NULL || DestDoc == NULL, "Illegal NULL param");

	if (SourceDoc == DestDoc)	// Completely safe - into the same doc
		return;	

	// Find the indexed colour (if any) that the given colour references
	IndexedColour *ColourToApply = ResultCol->FindParentIndexedColour();

	if (ColourToApply == NULL)	// It's a simple DocColour, so safe
		return;

	// It's an IndexedColour. We must copy it to the dest doc, and use the copy.
	if (DestDoc != NULL)
	{
		// We're applying the colour into a different document!
		// We must duplicate (with merging) this colour into the dest doc before applying
		ColourListComponent *ColComp = (ColourListComponent *)
								DestDoc->GetDocComponent(CC_RUNTIME_CLASS(ColourListComponent));

		// Start the copy
		if (ColComp != NULL && ColComp->StartComponentCopy())
		{
			DocColour RefCol;
			RefCol.MakeRefToIndexedColour(ColourToApply);

			// Copy this colour (and any linked parents if necessary). RefCol now
			// references the destination colour, so we point ColourToApply at it
			ColComp->CopyColourAcross(&RefCol);
			ColourToApply = RefCol.FindParentIndexedColour();

			// And clean up, merging the colour into the other document's colour list
			ColComp->EndComponentCopy(NULL, FALSE);

			// We've got an IndexedColour, so set ResultCol on it, and return
			ResultCol->MakeRefToIndexedColour(ColourToApply);
			return;
		}
	}

	// We've failed to copy the colour across, so set it to simple 'no colour'
	*ResultCol = DocColour(COLOUR_BLACK);
}



/********************************************************************************************

>	void ColourManager::ColourHasChanged(Document *ScopeDoc,
											ColourList *ColList, IndexedColour *Col)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/5/94
	Inputs:		ScopeDoc - the document in which this change is occurring
				ColList - the list of IndexedColours in which 'Col' resides
				Col - A pointer to the IndexedColour which has been changed
	Outputs:	-
	Returns:	-
	Purpose:	Indicates to all interested parties that the given IndexedColour
				has been changed (sends a COLOURUPDATED message)
				e.g. Causes the ColourBar and document to redraw themselves

				This variant indicates that the actual visible-colour definition has
				changed (i.e. the appearance of something filled with this colour will
				chnage, and thus must be redrawn). If 'non-visible' information only
				has changed, use ColourHasChangedInvisible instead.

	Notes:		If you are recieving messages, and wish to act on COLOURUPDATEDINVISIBLE,
				you should sit on BOTH COLOURUPDATED messages. Otherwise, you should
				only sit on COLOURUPDATED messages.

				The ColourManager will force-redraw the parst of the ScopeDoc which
				rely on the changed colour, or any colour liked to the changed colour.

				If ColList == the current colour list then entities such as the colour
				bar will redraw themselves to show the new state of this colour.

	SeeAlso:	ColourManager::ColourHasChangedInvisible

********************************************************************************************/

void ColourManager::ColourHasChanged(Document *ScopeDoc,
										ColourList *ColList, IndexedColour *Col)
{
	ERROR3IF(ScopeDoc == NULL || ColList == NULL || Col == NULL,
				"ColourManager::ColourHasChanged - NULL parameters are illegal!\n");

	ColList->InvalidateInheritanceCache();

	BROADCAST_TO_ALL(ColourChangingMsg(ScopeDoc, ColList, Col,
								ColourChangingMsg::COLOURUPDATED));
}



/********************************************************************************************

>	void ColourManager::ColourHasChangedInvisible(Document *ScopeDoc,
													ColourList *ColList, IndexedColour *Col)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/5/94
	Inputs:		ScopeDoc - NULL, or the document in which this change is occurring
				(if non-NULL, the ColourManager will force a redraw of this document)
				ColList - NULL, or a list of IndexedColours in which 'Col'
				can be found.
				Col - A pointer to the IndexedColour which has been changed
	Outputs:	-
	Returns:	-
	Purpose:	Indicates to all interested parties that the given IndexedColour
				has been changed (sends a COLOURUPDATEDINVISIBLE message)
				e.g. Causes the ColourBar to redraw itself

				This variant indicates that the actual visible-colour definition has
				NOT changed (i.e. the appearance of something filled with this colour will
				NOT change, and thus redraw is unnecessary). e.g. the 
				
				If 'visible' information has changed, use ColourHasChanged instead.

	Notes:		If you are recieving messages, and wish to act on COLOURUPDATEDINVISIBLE,
				you should sit on BOTH COLOURUPDATED messages.

				ColList really should point to the list in which the colour
				resides. If ColList == NULL or ColList == the current colour list,
				then entities such as the colour bar will redraw themselves to
				show the new state of this colour.

				If ColList is not NULL, then all its sort orders will be marked invalid
				so that they are re-sorted the next time they are used.

				(If an incorrect pointer (or NULL) is passed in, unnecessary redraws
				of colour bars etc may take place, so it is most preferable that you
				pass in the correct list pointer if you know it)

	SeeAlso:	ColourManager::ColourHasChanged

********************************************************************************************/

void ColourManager::ColourHasChangedInvisible(Document *ScopeDoc,
												ColourList *ColList, IndexedColour *Col)
{
	ColList->InvalidateInheritanceCache();
	BROADCAST_TO_ALL(ColourChangingMsg(ScopeDoc, ColList, Col,
								ColourChangingMsg::COLOURUPDATEDINVISIBLE));
}



/********************************************************************************************

>	void ColourManager::ColourListHasChanged(ColourList *ColList)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/5/94
	Inputs:		ColList - A ColourList of IndexedColours which has changed
				May NOT be NULL
	Outputs:	-
	Returns:	-
	Purpose:	To inform all interested parties that the given list of
				IndexedColours has changed by item(s) being added, deleted,
				or moved to a new position.
				e.g This causes the ColourBar and ColourMgrDlg to update their
				displays to reflect the new state of the list, if the list is the
				one which they are currently displaying.

	Notes:		Remember to invalidate the sort orders in this sorted list if you
				have done anything that could affect sorting (Adding items will
				do this automatically, but you must do it if you've changed a colour)
				This is not done by this function, as actions such as deleting items
				never affect the sort order, but do need to cause a re-display

********************************************************************************************/

void ColourManager::ColourListHasChanged(ColourList *ColList)
{
	ERROR3IF(ColList == NULL, "ColourManager::ColourListHasChanged: NULL parameter is illegal");

	// Inform the colour gallery ahead of everyone else
//	ColourGallery::ColourListHasChanged(ColList);

	// And inform the plebs
	ColList->InvalidateInheritanceCache();

	Document *ParentDoc = (Document *) ColList->GetParentDocument();
	BROADCAST_TO_ALL(ColourChangingMsg(ParentDoc, ColList, NULL,
								ColourChangingMsg::LISTUPDATED));
}



/********************************************************************************************

>	BOOL ColourManager::HideColours(ColourList *ColList, IndexedColour **ColourArray,
									BOOL ForceDelete)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/6/94
	Inputs:		ColList - The colour list in which the colours reside. Must be non-NULL.
				ColourArray - A NULL-terminated list of pointers to colours
				to be hidden. NOTE that this array is changed in the process!
				ForceDelete - TRUE if you want to forcibly hide ('delete') all
				the colours, even if they are in use in the document (in this case,
				all references in the document will be coerced to immediate DocColours
				(undoable, of course) and then the colour will be hidden as usual)
				FALSE if you only want truly unused colours to be hidden

	Outputs:	ColourArray may be changed (see Notes)

	Returns:	TRUE if it thinks it succeeded

	Purpose:	This 'deletes' colours by creating an undoable operation to
				hide them. It is assumed that the colours are not in use by the
				time you call this function - An ENSURE failure will occur if
				any are. After invoking the 'hide' operation, this function calls
				ColourListHasChanged

	Notes:		The passed ColourArray parameter is used as temporary workspace,
				and will be returned containing a list of the colours which were
				succesfully deleted. This modified list is copied for the undo system,
				so you need not keep it around after the call to this fn - i.e. it is
				your responsibility to delete the array after calling this method.
				The modified list is also guaranteed to be no larger than the original
				array, so you don't need to allocate extra space for it.

	SeeAlso:	ColourManager::UnHideColours; ColourManager::ColourListHasChanged

********************************************************************************************/

BOOL ColourManager::HideColours(ColourList *ColList, IndexedColour **ColourArray,
								BOOL ForceDelete)
{
	ERROR3IF(ColList == NULL || ColourArray == NULL,
				"ColourManager::HideColours cannot accept NULL parameters\n");

	HideColoursInfo HCInfo;

	HCInfo.ParentList	= ColList;
	HCInfo.Colours		= ColourArray;
	HCInfo.ForceDelete	= ForceDelete;
	HCInfo.HideThem		= TRUE;

	OpDescriptor* OpDesc =
				OpDescriptor::FindOpDescriptor( OPTOKEN_HIDECOLOURS );

	if (OpDesc != NULL)
		OpDesc->Invoke((OpParam *) &HCInfo);

	return(TRUE);
}



/********************************************************************************************

>	BOOL ColourManager::UnHideColours(ColourList *ColList, IndexedColour **ColourArray)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/6/94
	Inputs:		ColList - The colour list in which the colours reside. Must be non-NULL.
				ColourArray - A NULL-terminated list of pointers to colours
				to be unhidden
	Outputs:	-
	Returns:	TRUE if it thinks it succeeded

	Purpose:	This un-'deletes' colours by creating an undoable operation to
				unhide them. This is used when creating a new colour to 'unhide'
				it (which has no immediate effect on the colour) - this creates
				an undo record that will hide the colour if the 'new' is undone.

	Notes:		The undo record generated uses a COPY of the array you passed in.
				It is your responsibility to delete the original array after calling
				this method.

	SeeAlso:	ColourManager::HideColours; ColourManager::ColourListHasChanged

********************************************************************************************/

BOOL ColourManager::UnHideColours(ColourList *ColList, IndexedColour **ColourArray)
{
	ERROR3IF(ColList == NULL || ColourArray == NULL,
				"ColourManager::UnHideColours cannot accept NULL parameters\n");

	HideColoursInfo HCInfo;

	HCInfo.ParentList	= ColList;
	HCInfo.Colours		= ColourArray;
	HCInfo.ForceDelete	= FALSE;
	HCInfo.HideThem		= FALSE;

	OpDescriptor* OpDesc =
				OpDescriptor::FindOpDescriptor( OPTOKEN_SHOWCOLOURS );

	if (OpDesc != NULL)
		OpDesc->Invoke((OpParam *) &HCInfo);
	
	return(TRUE);
}



/********************************************************************************************

>	static BOOL ColourManager::ChangeColour(ColourList *ColList,
									IndexedColour *NewDefn, IndexedColour *Target,
									BOOL ChangeIsInvisible = FALSE)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/6/94
	Inputs:		ColList - The colour list in which the colours reside
				NewDefn - The new IndexedColour definition to be applied
				Target  - The colour to which the definition is to be applied
				ChangeIsInvisible - TRUE if the change does not affect 'visible' parts
				of the colour definition (i.e. if objects drawn using this colour need
				not be redrawn). FALSE if the chnage does affect such objects. 

	Outputs:	-
	Returns:	TRUE if it thinks it succeeded

	Purpose:	This changes the colour 'Target' to be the same as 'NewDefn'.
				To achieve undo of this operation, the data members of the two 
				colours are swapped (as the colour being changed must stay at the
				same memory location), and the NewDefn is used again as the undo
				record of this colour.
				Whenever the colour is changed, including during the execution
				of this funciton, and also by undo/redo, a ColourChangingMsg will
				be broadcast (either COLOURUPDATED or COLOURUPDATEDINVISIBLE, 
				depending on the state of the ChangeIsInvisible flag)

	Notes:		As the memory occupied by 'NewDefn' is used in the undo record,
				it is essential that you do NOT DELETE it after invoking this
				function. You must also make absolutely sure that you do not
				pass the same pointer in NewDefn more than once, or a crash will
				occur when Camelot throws away the undo records.

	SeeAlso:	OpColourChange; ColourChangingMsg

********************************************************************************************/

BOOL ColourManager::ChangeColour(ColourList *ColList,
									IndexedColour *NewDefn, IndexedColour *Target,
									BOOL ChangeIsInvisible)
{
#if !defined(EXCLUDE_FROM_RALPH)
	ERROR3IF(ColList == NULL || NewDefn == NULL || Target == NULL,
				"ColourManager::ChangeColour - illegal NULL param(s)");

	OpDescriptor* OpDesc =
				OpDescriptor::FindOpDescriptor(CC_RUNTIME_CLASS(OpColourChange));

	if (OpDesc != NULL)
	{
		ColourChangeInfo Param;

		Param.ParentList = ColList;
		Param.Target	 = Target;
		Param.NewDefn	 = NewDefn;
		Param.InvisibleChange = ChangeIsInvisible;

		OpDesc->Invoke((OpParam *) &Param);
	}
#endif
	return(TRUE);
}



















#if !defined(EXCLUDE_FROM_RALPH)

/********************************************************************************************

>	OpHideColours::OpHideColours() 

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/6/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	OpHideColours constructor (Creates an undoable operation)
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
            
            
OpHideColours::OpHideColours(): UndoableOperation()								
{                              
	Colours = NULL;		// For safety, ensure initialised
	ParentColList = NULL;
	HideThem = TRUE;
}



/********************************************************************************************

>	BOOL OpHideColours::Init()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/6/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the operation could be successfully initialised 
				FALSE if no more memory could be allocated 
				
	Purpose:	OpHideColours initialiser method

	Notes:		This provides TWO OpDescriptors (Hide colours and Show colours).
				The same actual Op is invoked for both hiding and showing colours,
				but it is invoked via different op descriptors, to provide different
				names for the undo ("undo delete colours" or "undo create colours")


********************************************************************************************/

BOOL OpHideColours::Init()
{
	BOOL Result = RegisterOpDescriptor(0,
	 							 _R(IDS_OPHIDECOLOURS),
								 CC_RUNTIME_CLASS(OpHideColours),
	 							 OPTOKEN_HIDECOLOURS,
	 							 OpHideColours::GetState,
	 							 0,	/* help ID */
	 							 0,	/* Bubble help ID */
	 							 0	/* bitmap ID */
	 							);
	if (Result)
		Result = RegisterOpDescriptor(0,
	 							 _R(IDS_OPSHOWCOLOURS),
								 CC_RUNTIME_CLASS(OpHideColours),
	 							 OPTOKEN_SHOWCOLOURS,
	 							 OpHideColours::GetState,
	 							 0,	/* help ID */
	 							 0,	/* Bubble help ID */
	 							 0	/* bitmap ID */
	 							);
	return(Result);
}
    


/********************************************************************************************

>	OpState	OpHideColours::GetState(String_256*, OpDescriptor*)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/6/94
	Inputs:		-
	Outputs:	-
	Returns:	The state of the OpHideColours operation
	Purpose:	For finding the OpHideColours' state. 

********************************************************************************************/

OpState	OpHideColours::GetState(String_256* UIDescription, OpDescriptor*)
{
	OpState OpSt;
 	return(OpSt);
}



/********************************************************************************************

>	void OpHideColours::Do(OpDescriptor*)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/6/94
	Inputs:		OpDescriptor (unused)
	Outputs:	-
	Returns:	-
	Purpose:	(Performs the colour change operation)
				OpHideColours has a special overloaded Do operator which takes
				parameters describing what is to be done - that version of Do must
				be used
	Errors:		Always generates an ENSURE failure because this Do doesn't.
	SeeAlso:	-

********************************************************************************************/
			
void OpHideColours::Do(OpDescriptor *NotUsed)
{
	ERROR3("OpHideColours does not provide a Do() function - use DoWithParam");
	End();
}



/********************************************************************************************

>	void OpHideColours::DoWithParam(OpDescriptor *pOp, OpParam *Param)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/6/94
	Inputs:		pOp - OpDescriptor as for all Do() functions
				Param - Actually points to a HideColoursInfo structure, detailing what to
				hide/unhide, and how to do it.
	Outputs:	-
	Returns:	-
	Purpose:	Performs the colour change operation, as described by 'Param'

********************************************************************************************/
			
void OpHideColours::DoWithParam(OpDescriptor *pOp, OpParam *Param)
{
	HideColoursInfo *WhatToDo = (HideColoursInfo *) Param;
	// If Forced-delete is desired, then ensure that all DocColours referencing
	// any of our list of IndexedColours are forced to 'immediate' definitions

	// Make sure that this operation is applied within the document which owns the colour list!
	pOurDoc = (Document *) WhatToDo->ParentList->GetParentDocument();
	pOurView = NULL;
	if (!pOurDoc->IsKindOf(CC_RUNTIME_CLASS(Document)))
	{
		pOurDoc = NULL;		// Arrrgh! It must be a basedoc or something!
		End();
		return;
	}

	ParentColList = WhatToDo->ParentList;
	Colours  = WhatToDo->Colours;
	HideThem = WhatToDo->HideThem;

	if (HideThem)
	{
		if (WhatToDo->ForceDelete)
			HideColourRefs();
		else
			RemoveReferencedColours();
	}

	if (Colours[0] == NULL)
	{
		End();
		return;
	}

	// Create an action to undo the IndexedColour hide operation, and if we 
	// succeed, then {un}hide the colours (Do) and inform the world of the change
	ActionHideColours *AHC;
	if (ActionHideColours::Init(this, &UndoActions, sizeof(ActionHideColours),
							WhatToDo->ParentList, WhatToDo->Colours,
							((WhatToDo->HideThem) ? FALSE : TRUE),
							(Action**)(&AHC)) == AC_OK)
	{
		// Now, hide/unhide all the colours remaining in the list
		INT32 Index = 0;
		while (WhatToDo->Colours[Index] != NULL)
		{
			WhatToDo->Colours[Index]->SetDeleted(WhatToDo->HideThem);
			Index++;
		}

		// We have made the change, so broadcast that the list for this doc has changed
		ColourManager::ColourListHasChanged(ParentColList);
	}

	End();
}



/********************************************************************************************

>	void OpHideColours::Undo()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/6/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Undoes whatever Do() or Redo() did

********************************************************************************************/

BOOL OpHideColours::Undo() 
{
	BOOL Result = Operation::Undo();
	if (Result)
	{
		// We have made the change, so broadcast that the list for this doc has changed
		ColourManager::ColourListHasChanged(ParentColList);
	}

	return (Result); 
}		



/********************************************************************************************

>	void OpHideColours::Redo()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/6/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Redoes whatever Undo() undid

********************************************************************************************/

BOOL OpHideColours::Redo()
{
	BOOL Result = Operation::Redo();
	if (Result)
	{
		// We have made the change, so broadcast that the list for this doc has changed
		ColourManager::ColourListHasChanged(ParentColList);
	}

	return (Result);
}



/********************************************************************************************

>	virtual void OpHideColours::GetOpName(String_256 *Result)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/6/94

	Outputs:	Result - returned filled-in with the name
	Returns:	-
	Purpose:	Redoes whatever Undo() undid

********************************************************************************************/

void OpHideColours::GetOpName(String_256 *Result)
{
	ERROR3IF(Result == NULL, "Illegal null param");

	if (HideThem)
		Result->MakeMsg(_R(IDS_DELETECOLOURS));
	else
		Result->MakeMsg(_R(IDS_CREATECOLOURS));
}



/********************************************************************************************

>	NodeAttribute *OpHideColours::HideAttrNode(Node *CurNode, DocColour *TheColour)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/6/94
	Inputs:		CurNode - The Attribute node containing the colour
				TheColour - the colour in the node which we may want to hide
	Outputs:	-
	Returns:	NULL, or the cloned node.
	Purpose:	Checks if the given colour references an IndexedColour in the
				given NULL terminated list of colours. If this is the case, then
				the node containing the colour attribute is hidden and replaced
				with a clone of itself. This clone is returned, so that the caller
				can then change the contained DocColour(s) appropriately.

********************************************************************************************/

NodeAttribute *OpHideColours::HideAttrNode(Node *CurNode, DocColour *TheColour)
{
	IndexedColour *ParentCol = TheColour->FindParentIndexedColour();

	if (ParentCol != NULL)
	{
		INT32 i = 0;
		while (Colours[i] != NULL && Colours[i] != ParentCol)
			i++;

		if (Colours[i] != ParentCol)
			return(NULL);
		
		Node *ParentNode = CurNode->FindParent();
		NodeAttribute *NodeToClone = (NodeAttribute *) CurNode;
		NodeAttribute *Clone;

		ALLOC_WITH_FAIL(Clone, ((NodeAttribute *)NodeToClone->SimpleCopy()), this);
		if (Clone != NULL)
		{
			// Hide the original Attribute
			DoHideNode(CurNode, TRUE);

			// Add the clone into the tree
			Clone->AttachNode(ParentNode, FIRSTCHILD);
			HideNodeAction* TheUndoAction; 

           	// Create an action to hide the attribute when we undo
			if (HideNodeAction::Init(this, &UndoActions, Clone, TRUE,
									 (Action**)(&TheUndoAction)) == AC_FAIL)
			{   
				Clone->CascadeDelete();		// Delink the node/subtree from the
				delete Clone;				// document tree and delete it
			}
			else
				return(Clone);
		}
	}

	return(NULL);
}



/********************************************************************************************

>	void OpHideColours::KillColourRef(DocColour *Perpetrator, IndexedColourPtr *ColourMap)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/6/94

	Inputs:		Perpetrator - The DocColour to be neutered

				ColourMap - NULL, or an array of IndexedColour pointers which map Colours[]
				into new local colour pointers. If the mapping entry is NULL, a mapping is
				set up, else the existing mapping is used. If ColourMap is NULL or memory
				cannot be allocated for the new mapping, this method will revert to making
				the colours immediate DocColours (which means it'll work in a falback mode)

	Purpose:	If the given DocColour refers to an IndexedColour, it is 
				redefined as a Local colour based on the IndexedColour's
				definition.

				This releases the DocColour's claim on the IxCol, thus allowing the latter
				to be deleted. Used for forced-delete

				A new local colour is created (if necessary) and placed in the colour map
				entry corresponding to the index of the source colour in Colours[].
				This is used for look-up, so that many colours using the same colour style
				can share the same resulting local colour definition, saving gobs of memory.

	Scope:		private

********************************************************************************************/

void OpHideColours::KillColourRef(DocColour *Perpetrator, IndexedColourPtr *ColourMap)
{
	IndexedColour *Parent = Perpetrator->FindParentIndexedColour();

	if (Parent == NULL)
		return;

	INT32 i = 0;
	if (ColourMap != NULL)
	{
		// Find the colour map entry index
		while (Colours[i] != NULL && Colours[i] != Parent)
			i++;

		// If there is no mapping yet, create a new local colour to map to this Parent colour
		if (Colours[i] != NULL && ColourMap != NULL && ColourMap[i] == NULL)
		{
			ColourMap[i] = new IndexedColour(*(Colours[i]));
			if (ColourMap[i] != NULL)
			{
				ColourMap[i]->SetUnnamed();	// Ensure it's unnamed

				ERROR3IF(GetWorkingDoc() == NULL, "NULL working doc in OpHideColours");

				// Add the local colour to the unnamed colour list
				// (A nasty sequence of derefs, but these are all guaranteed to work)
				GetWorkingDoc()->GetIndexedColours()->GetUnnamedColours()->AddTail(ColourMap[i]);

				// Make sure the new local colour is legal (if its parent is deleted or is
				// just about to become deleted, then we must make the new colour stand-alone.
				// (In the case of undo, we needen't worry about being able to restore the
				// parent link, because the undo will also hide this attribute and return to
				// the original one which references the old parent-colour chain)
				IndexedColour *LinkedParent = ColourMap[i]->FindLinkedParent();

				if (LinkedParent != NULL)
				{
					// Is the parent already deleted?
					BOOL MustMakeStandalone = LinkedParent->IsDeleted();

					// If not, is it one of the colours we are currently deleting?
					if (!MustMakeStandalone)
					{
						INT32 j = 0;
						while (Colours[j] != NULL && Colours[j] != LinkedParent)
							j++;

						if (Colours[j] != NULL)		// LinkedParent is in our list to be deleted!
							MustMakeStandalone = TRUE;
					}

					// It's parent is dodgy, so make it stand-alone
					if (MustMakeStandalone)
						ColourMap[i]->SetLinkedParent(NULL, COLOURTYPE_NORMAL);
				}
			}
		}
	}

	if (ColourMap != NULL && Colours[i] != NULL && ColourMap[i] != NULL)
	{
		// We have a mapping - change the colour to reference the relevant local colour
		Perpetrator->MakeRefToIndexedColour(ColourMap[i]);
	}
	else
	{
		// Serious failure - make the DocColour immediate
		ColourGeneric Bob;

		Parent->GetSourceColour(&Bob);
		*Perpetrator = DocColour(Parent->GetColourModel(), &Bob);
	}
}



/********************************************************************************************

>	void OpHideColours::HideColourRefs(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/6/94
	Purpose:	Traverses the entire Camelot tree and replaces all references to
				indexed colours with immediate DocColour equivalents. The original
				attributes are hidden to allow undo.

********************************************************************************************/
		
void OpHideColours::HideColourRefs(void)
{
	INT32 NumCols = 0;

	while (Colours[NumCols] != NULL)		// Find the last colour in the list
		NumCols++;


	IndexedColourPtr *ColourMap;
	ALLOC_WITH_FAIL(ColourMap, new IndexedColourPtr[NumCols+1], this);

	// If ALLOC_WITH_FAIL fails, we back down to making the colours immediate DocColours
	// rather than failing completely.

	// Clear the array to all NULL pointers
	if (ColourMap != NULL)
	{
		for (INT32 i = 0; i <= NumCols; i++)
			ColourMap[i] = NULL;
	}


	Node *CurNode = Node::DocFindFirstDepthFirst(GetWorkingDoc());
	Node *NextNode;
	Node *NewNode;

	while (CurNode != NULL)
	{
		// We may be about to chop this node out of the tree, so get the next node now
		NextNode = CurNode->DocFindNextDepthFirst();
	
		NewNode = NULL;

		// Use to scan the colour fields of the attribute.
		UINT32 Context = 0;
		DocColour *pColour;

		if (CurNode->IsAnAttribute())
		{
			NodeAttribute *pNodeAttr = (NodeAttribute *) CurNode;
	
			do
			{
				// Get the next colour field from the attribute
				pColour = pNodeAttr->EnumerateColourFields(Context++);

				// If there is another colour field, see if we need to hide it
				if (pColour != NULL && pColour->FindParentIndexedColour() != NULL)
					NewNode = HideAttrNode(CurNode, pColour);
			} 
			while ((NewNode == NULL) && (pColour != NULL));
				// Keep going until we run out of colour fields, or we find one
				// that requires the node to be hidden


			if (NewNode != NULL)
			{
				// Forcibly change the NewNode DocColours to be local
				pNodeAttr = (NodeAttribute *) NewNode;
				Context = 0;	// Start at the first colour field again

				do
				{
					// Get the next colour field from the attribute
					pColour = pNodeAttr->EnumerateColourFields(Context++);

					// If there is another colour, kill its reference, if any
					if (pColour != NULL)
						KillColourRef(pColour, ColourMap);
				} while (pColour != NULL);
			}

		}	// end if

		CurNode = NextNode;
	}	// end while

	// And get rid of our working array
	delete [] ColourMap;
} 



/********************************************************************************************

>	void OpHideColours::RemoveColour(DocColour *Ref, INT32 *LastColour)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/6/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Removes the indexedcolour (if any) referenced by the given DocColour
				from the 'Colours' list of colours-to-delete.

	Notes:		This assumes that any given colour will only appear in the list once
				It shifts all colours down by one place, as it is essential that the
				list ordering is retained.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void OpHideColours::RemoveColour(DocColour *Ref, INT32 *LastColour)
{
	IndexedColour *Col = Ref->FindParentIndexedColour();
	
	if (Col == NULL)							// No colour referenced
		return;

	INT32 i;
	for (i = 0; i <= *LastColour; i++)		// Search for colour to delete
	{
		if (Col == Colours[i])
			break;
	}

	if (Col == Colours[i])						// We found something to delete
	{
		while (i < *LastColour)					// Shift remaining list down one place
		{
			Colours[i] = Colours[i+1];
			i++;
		}

		Colours[*LastColour] = NULL;			// Add new NULL terminator
		(*LastColour)--;						// Decrement LastColour index
	}
}



/********************************************************************************************

>	void OpHideColours::RemoveReferencedColours(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/6/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Traverses the entire Camelot tree and removes all IndexedColours
				that are referenced therein from our list of colours-to-delete
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void OpHideColours::RemoveReferencedColours(void)
{
	INT32 LastColour = 0;

	while (Colours[LastColour] != NULL)		// Find the last colour in the list
		LastColour++;

	if ((--LastColour) < 0)
		return;

	// Search the entire tree, removing all IndexedColours (from the WhatToDo
	// list) which are still in use in live parts of the tree. These cannot
	// be deleted, and so should be removed from further consideration
	ERROR3IF(GetWorkingDoc() == NULL, "OpHideColours called with NULL Document");
	
	Node *CurNode = GetWorkingDoc()->GetFirstNode();
	Node *NextNode;
	while (CurNode != NULL)
	{
		// Have to remember this now, as we may be about to chop this node out of the tree!
		NextNode = CurNode->DocFindNextDepthFirst();

		if (CurNode->IsAnAttribute())
		{
			// These variables are used to scan the colour fields in the attribute.
			NodeAttribute *pNodeAttr = (NodeAttribute *) CurNode;
			DocColour *pColour;
			UINT32 Context = 0;

			do
			{
				// Get the next colour field from the attribute
				pColour = pNodeAttr->EnumerateColourFields(Context++);

				// If there is another colour field, see if we need to hide it.
				if (pColour != NULL)
					RemoveColour(pColour, &LastColour);
			} 
			// Keep going until we run out of colour fields
			while (pColour != NULL);
		}

		CurNode = NextNode;
	}
}



























/********************************************************************************************

>	ActionHideColours::ActionHideColours()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/6/94
	Purpose:	ActionHideColours constructor

********************************************************************************************/
    
ActionHideColours::ActionHideColours()
{
	ChangeColour = NULL;
	ParentList	 = NULL;
}



/********************************************************************************************

>	ActionHideColours::~ActionHideColours() 

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/6/94
	Purpose:	ActionHideColours destructor

********************************************************************************************/

ActionHideColours::~ActionHideColours()
{
	if (ChangeColour != NULL)
		delete [] ChangeColour;
}



/********************************************************************************************

>	virtual void ActionHideColours::Slaughter(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/11/94
	Purpose:	ActionHideColours Slaughter method. Cleans up any deleted colours associated
				with this action - once slaughtered, deleted colours can not come back
				(touch wood!) so are actually deleted from the document rather than just
				hidden.
	
	Notes:		Only colours which are truly 'deleted' at this point in time are destructed,
				because this action both deletes (hides) and undeletes (unhides) colours, so
				our 'undo/redo' record may point at colours which are currently alive and
				in use. We also have to be careful about this, as there can be 2 undo
				records for the same colour (creation and deletion) in the list at once,
				and obviously, only one of them should attemp to destruct the colour!

********************************************************************************************/

void ActionHideColours::Slaughter(void)
{
	// If we have some colour(s) and our last action was to hide them (thus they are
	// deleted colours rather than undeleted ones), then we are responsible for
	// slaughtering the now-unneeded colours.
	// NOTE: Remember that if we are a 'Hide' action, then the colour should currently
	// be in an UN-hidden state; thus, we delete ONLY if we are an UN-hide action.

	if (ChangeColour != NULL && !HideFlag)
	{
		INT32 Index = 0;
	
		// Delete each colour in turn from the ParentList ColourList
		while (ChangeColour[Index] != NULL)
		{
			// Remove from the list
			ParentList->RemoveItem(ChangeColour[Index]);

			// Ensure it has no children pointing at it
			ParentList->RemoveLinksToColour(ChangeColour[Index]);

			// Delink this colour from its parent, if any
			if (ChangeColour[Index]->FindLastLinkedParent() != NULL)
				ChangeColour[Index]->SetLinkedParent(NULL, COLOURTYPE_NORMAL);

			// And finally, delete it
			delete ChangeColour[Index];

			Index++;
		}
	}

	Action::Slaughter();		// Call the base class Slaughter method to delete ourself
}


/********************************************************************************************

>	virtual ActionCode ActionHideColours::Execute()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/6/94
	Inputs:		-
	Outputs:	ActionCode indicating if the action was successfully executed or not
	Returns:	-
	Purpose:	Executes the ActionHideColours (to hide/unhide an IndexedColour), creating
				an equal and opposite action to undo it with later.

********************************************************************************************/

ActionCode ActionHideColours::Execute()
{
   ActionHideColours HideColAct;
   ActionCode ActCode;

	// Create an action to restore the changes we are about to make
    if ((ActCode = ActionHideColours::Init(pOperation,
								     		pOppositeActLst,
								     		sizeof(ActionHideColours),
											ParentList, ChangeColour,
											((HideFlag) ? FALSE : TRUE),
						 			 		(Action**)(&HideColAct))) != AC_FAIL)
	{
		INT32 Index = 0;

		while (ChangeColour[Index] != NULL)		// Write all the 'deleted' flags
		{
			// Check that the caller only gave us named colours
			ERROR3IF(!ChangeColour[Index]->IsNamed(),
				"You don't need to call (Un)HideColours for Local colours! Remove ASAP!");

			ChangeColour[Index]->SetDeleted(HideFlag);
			Index++;
		}
	}

	return (ActCode);
}



/********************************************************************************************

>	static ActionCode ActionHideColours::Init(Operation* const pOp, 
									   			ActionList* pActionList, 	
						  			   			UINT32 ActionSize, 
												ColourList *TheParentList,
												IndexedColour* Colour[],
												BOOL HideThem,
												Action** NewAction);
	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/6/94

	Inputs:		pOp: The operation to which the action should be added
				pActionList: The action list in the operation object
				ActionSize:  The size of the action in bytes. This should be the total  
							 size of the action (including any objects pointed to by the
							 action).
				TheParentList: Points to the list containing the IndexedColours in Colour[]
				Colour[]:	 A NULL terminated list array of pointers to the IndexedColours
							 which are to be hidden/unhidden
				HideThem:	 TRUE to Hide, FALSE to unhide the colours.

 	Outputs:    NewAction:   A pointer to the action if it could be allocated. 

	Returns:	AC_FAIL:     There was not enough room in the operation history for the
							 action and the user did not wish to continue. Usually
							 End() should be called in this situation.

				AC_NORECORD: There was not enough room in the operation history for
							 the action, but the user requested that he wished to 
							 continue without undo. 

				AC_OK      : The action was successfully initialised and added to the 
							 operation. 
				         
				           
	Purpose:	To check that there is sufficient room for the action in the operation
				history, and if there is, then to add the action to the operations 
				action list. 
				
				The function calls the Action::Init function passing the runtime class 
				of an ActionHideColours.
	Errors:		-
	SeeAlso:	Action::Init

********************************************************************************************/
   
ActionCode ActionHideColours::Init(Operation* const pOp, 
									ActionList* pActionList, 	
						  			UINT32 ActionSize,  
									ColourList *TheParentList,
									IndexedColour* Colour[],
									BOOL HideThem,
				  					Action** NewAction)
{
	ActionCode Ac = (Action::Init(pOp,
									pActionList,
									ActionSize,
									CC_RUNTIME_CLASS(ActionHideColours), 
									NewAction));
	if (*NewAction != NULL)
	{
		INT32 Count = 0;

		while (Colour[Count] != NULL)					// Count array size
			Count++;

		ActionHideColours *AHC = (ActionHideColours*) (*NewAction);

		ALLOC_WITH_FAIL(AHC->ChangeColour, new IndexedColourPtr[Count+1], pOp);

		if (AHC->ChangeColour == NULL)					// Failed in Action Alloc
		{
			delete AHC;									// Delete the action
			*NewAction = NULL;							// Ensure return NULL
			return(AC_FAIL);							// And return 'failed'
		}

		Count = 0;
		while (Colour[Count] != NULL)					// Copy Colour array
		{
			AHC->ChangeColour[Count] = Colour[Count];
			Count++;
		}
		AHC->ChangeColour[Count] = NULL;				// And NULL terminate
		
		AHC->ParentList = TheParentList;				// Copy the parentlist pointer
		AHC->HideFlag = HideThem;						// Copy HideFlag
	}

	return (Ac);
}

























/********************************************************************************************

>	OpColourChange::OpColourChange()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/6/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	OpColourChange constructor (Creates an undoable operation)
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

OpColourChange::OpColourChange() : UndoableOperation()
{
	UndoRecord = NULL;
	RedoRecord = NULL;
}



/********************************************************************************************

>	OpColourChange::~OpColourChange() 

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/6/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	OpColourChange destructor
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

OpColourChange::~OpColourChange()
{
	// We 'own' the UNDO record, so we are responsible for deleting it
	if (UndoRecord != NULL)
		delete UndoRecord;

	// We do not own the RedoRecord, so we leave it alone
}



/********************************************************************************************

>	BOOL OpColourChange::Init()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/6/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the operation could be successfully initialised 
				FALSE if no more memory could be allocated 
				
	Purpose:	OpColourChange initialiser method

********************************************************************************************/

BOOL OpColourChange::Init()
{
	return (RegisterOpDescriptor(
	 								0,
	 								_R(IDS_COLOURCHANGEOP),
									CC_RUNTIME_CLASS(OpColourChange),
	 								OPTOKEN_COLOURCHANGE,
	 								OpColourChange::GetState,
	 								0,	/* help ID */
	 								0,	/* Bubble help ID */
	 								0	/* bitmap ID */
	 							));
}
    


/********************************************************************************************

>	OpState	OpColourChange::GetState(String_256*, OpDescriptor*)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/6/94
	Inputs:		-
	Outputs:	-
	Returns:	The state of the OpColourChange operation
	Purpose:	For finding the OpColourChange's state. 

********************************************************************************************/

OpState	OpColourChange::GetState(String_256* UIDescription, OpDescriptor*)
{
	OpState OpSt;
 	return(OpSt);
}



/********************************************************************************************

>	void OpColourChange::Do(OpDescriptor*)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/6/94
	Inputs:		OpDescriptor (unused)
	Outputs:	-
	Returns:	-
	Purpose:	(Performs the colour change operation)
				OpColourChange has a special overloaded Do operator which takes
				parameters describing what is to be done - that version of Do must
				be used
	Errors:		Always generates an ENSURE failure because this Do doesn't.
	SeeAlso:	-

********************************************************************************************/
			
void OpColourChange::Do(OpDescriptor *NotUsed)
{
	ENSURE(FALSE, "OpColourChange does not provide a Do() function - Use DoWithParam");
	End();
}



/********************************************************************************************

>	void OpColourChange::DoWithParam(OpDescriptor *pOp, OpParam *Param)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/6/94
	Inputs:		pOp - OpDescriptor as for all Do() functions
				Param - points to a ColourChangeInfo structure with parameters:
					ParentList - The List in which the colours reside
					Target - The colour to be changed
					NewDefn - The new definition for Target
					InvisibleChange - TRUE if the change is not to a visible feature of the
					colour (so objects drawn in this colour need not be redrawn)
	Outputs:	-
	Returns:	-
	Purpose:	Performs the colour change operation, which swaps (with undo)
				the data members two given colours.

	Note:		Both colours are used to provide the primary- and undo-storage,
				so you must NOT delete either of them after invoking this op.
				They will be deleted as part of the undo record if necessary.

				DO NOT call this directly - use ColourManager::ChangeColour

				Note that sequential colour changes for the same colour will be merged
				into a single undo step.

	SeeAlso:	ColourManager::ChangeColour

********************************************************************************************/
			
void OpColourChange::DoWithParam(OpDescriptor *pOp, OpParam *Param)
{
	ColourChangeInfo *Info = (ColourChangeInfo *) Param;

	// Remember the undo record so we don't get memory leakage
	UndoRecord = Info->NewDefn;		// We 'own' the undo record and must delete it
	RedoRecord = Info->Target;		// We do NOT own the target: For OpMerging info only

	ERROR3IF(UndoRecord == NULL || RedoRecord == NULL || Info->ParentList == NULL,
					"Illegal NULL OpParams for OpColourChange");

	// Set our scope variables
	pOurDoc = (Document *)Info->ParentList->GetParentDocument();
	pOurView = NULL;
	if (!pOurDoc->IsKindOf(CC_RUNTIME_CLASS(Document)))
	{
		pOurDoc = NULL;		// Arrrgh! It must be a basedoc or something!
		End();
		return;
	}

	
	// Create an action to Undo the hiding operation
	ActionColourChange *ACC;
	if (ActionColourChange::Init(this, &UndoActions, sizeof(ActionColourChange),
							RedoRecord, UndoRecord, Info->InvisibleChange,
							(Action**)(&ACC)) == AC_OK)
	{
		// Apply the initial 'Do' operation - swap the colours
		RedoRecord->SwapWith(UndoRecord);

		// We have made the change, so broadcast that the list for this doc has changed
		if (Info->InvisibleChange)
			ColourManager::ColourHasChangedInvisible(GetWorkingDoc(), Info->ParentList, RedoRecord);
		else
			ColourManager::ColourHasChanged(GetWorkingDoc(), Info->ParentList, RedoRecord);
	}

	End();
}



/********************************************************************************************

>	void OpColourChange::Undo()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/6/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Undoes whatever Do() or Redo() did

********************************************************************************************/

BOOL OpColourChange::Undo() 
{
	return (Operation::Undo()); 
}		



/********************************************************************************************

>	void OpColourChange::Redo()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/6/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Redoes whatever Undo() undid

********************************************************************************************/

BOOL OpColourChange::Redo()
{
	return (Operation::Redo());
}



/********************************************************************************************

>	virtual void OpColourChange::PerformMergeProcessing(void)


	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/11/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	This virtual function gets called to allow for the merging of
				ColourChange operations - this allows a multitude of sequential
				colour edits to be merged into a single undo stage.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void OpColourChange::PerformMergeProcessing(void)
{	 
	// Obtain a pointer to the operation history for the operation's document
	OperationHistory* pOpHist = &GetWorkingDoc()->GetOpHistory();
	ERROR3IF(pOpHist == NULL, "There's no OpHistory!?");

	// Ensure that we are the last operation added to the operation history
	// Note cannot be an ERROR2 cos this function cannot fail. 
	ERROR3IF(pOpHist->FindLastOp() != this, "Last Op should be this op"); 
	
	// OK lets see if the operation performed before this was an OpNudge operation
	Operation* pPrevOp = pOpHist->FindPrevToLastOp();

	if (pPrevOp != NULL)   // Check if there was a previous op
	{	
		if (IS_A(pPrevOp, OpColourChange))
		{
			if (((OpColourChange *)pPrevOp)->RedoRecord == RedoRecord)
			{
				// This op is a colour change for the same colour as the last op.

				// We may need to change the 'IsVisible' flag for the ActionColourChange
				// (if either change is 'visible', the overall change must be 'visible')
				
				ActionColourChange* pNewAction = (ActionColourChange *)
					GetUndoActionList()->FindActionOfClass(CC_RUNTIME_CLASS(ActionColourChange));
			
				ERROR3IF(pNewAction == NULL, "This op should have an ActionColourChange");

				// If the new action is visible, force the old one to be visible
				if (pNewAction != NULL && !pNewAction->IsInvisible)
				{
					ActionColourChange* pOldAction = (ActionColourChange *)
						((OpColourChange *)pPrevOp)->GetUndoActionList()->
									FindActionOfClass(CC_RUNTIME_CLASS(ActionColourChange));
			
					ERROR3IF(pOldAction == NULL, "This op should have an ActionColourChange");

					if (pOldAction != NULL)
						pOldAction->IsInvisible = FALSE;
				}

				// We have already overwritten the changed colour, so we just kill
				// ourselves to remove the unneeded undo step (so undo will revert
				// directly to the original state of the colour)
				pOpHist->DeleteLastOp(); 
			}
		}
	}
}




























/********************************************************************************************

>	ActionColourChange::ActionColourChange()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/6/94
	Purpose:	ActionColourChange constructor

********************************************************************************************/
    
ActionColourChange::ActionColourChange()
{
}



/********************************************************************************************

>	ActionColourChange::~ActionColourChange() 

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/6/94
	Purpose:	ActionColourChange destructor

********************************************************************************************/

ActionColourChange::~ActionColourChange()
{
}



/********************************************************************************************

>	virtual ActionCode ActionColourChange::Execute()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/6/94
	Inputs:		-
	Outputs:	ActionCode indicating if the action was successfully executed or not
	Returns:	-
	Purpose:	Executes the ActionColourChange to swap the two IndexedColours over
				and generates another ActionColourChange to undo this change.

********************************************************************************************/

ActionCode ActionColourChange::Execute()
{
   ActionColourChange ColChangeAct;
   ActionCode ActCode;

     // Create an action to restore the changes we are about to make
    if ((ActCode = ActionColourChange::Init(pOperation,
								     		pOppositeActLst,
								     		sizeof(ActionColourChange),
											Target, NewDefn, IsInvisible,
						 			 		(Action**)(&ColChangeAct))) != AC_FAIL)
	{
		// Swap the colour and its undo partner over
		Target->SwapWith(NewDefn);

		// Broadcast the change
		Document *ScopeDoc  = GetWorkingDoc();
		ColourList *ColList = ScopeDoc->GetIndexedColours();
		if (IsInvisible)
			ColourManager::ColourHasChangedInvisible(ScopeDoc, ColList, Target);
		else
			ColourManager::ColourHasChanged(ScopeDoc, ColList, Target);
	}

	return (ActCode);
}



/********************************************************************************************

>	ActionCode ActionColourChange::Init(Operation* const pOp, 
										ActionList* pActionList, 	
							  			UINT32 ActionSize,
										IndexedColour *TheTarget,
										IndexedColour *TheNewDefn,
										BOOL ChangeIsInvisible,
				  						Action** NewAction)
	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/6/94

	Inputs:		pOp: The operation to which the action should be added
				pActionList: The action list in the operation object
				ActionSize:  The size of the action in bytes. This should be the total  
							 size of the action (including any objects pointed to by the
							 action).
				TheTarget:	 The target colour which will be changed when we are executed
				TheNewDefn:	 The definition which will be used to change TheTarget
							 (These 2 colours will be swapped in actual operation)

				ChangeIsInvisible:
							 TRUE if this change to the colour is not 'visible' (i.e. objects
							 drawn in this colour will not visibly chnage - the chnage was
							 only to a flag or the name, etc.). See ColourManager::
							 ColourHasChanged and ColourHasChangedInvisible

 	Outputs:    NewAction:   A pointer to the action if it could be allocated. 

	Returns:	AC_FAIL:     There was not enough room in the operation history for the
							 action and the user did not wish to continue. Usually
							 End() should be called in this situation.

				AC_NORECORD: There was not enough room in the operation history for
							 the action, but the user requested that he wished to 
							 continue without undo. 

				AC_OK      : The action was successfully initialised and added to the 
							 operation. 
				         
				           
	Purpose:	To check that there is sufficient room for the action in the operation
				history, and if there is, then to add the action to the operations 
				action list. 
				
				The function calls the Action::Init function passing the runtime class 
				of an ActionColourChange.
	Errors:		-
	SeeAlso:	Action::Init; ColourManager::ColourHasChanged;
				ColourManager::ColourHasChangedInvisible; OpColourChange::DoWithParam

********************************************************************************************/
   
ActionCode ActionColourChange::Init(Operation* const pOp, 
									ActionList* pActionList, 	
						  			UINT32 ActionSize,
									IndexedColour *TheTarget,
									IndexedColour *TheNewDefn,
									BOOL ChangeIsInvisible,
				  					Action** NewAction)
{
	

	ActionCode Ac = (Action::Init(pOp,
									pActionList,
									ActionSize,
									CC_RUNTIME_CLASS(ActionColourChange), 
									NewAction));
	if (*NewAction != NULL)
	{
		ActionColourChange *ACC = (ActionColourChange*) (*NewAction);
		ACC->Target  = TheTarget;
		ACC->NewDefn = TheNewDefn;
		ACC->IsInvisible = ChangeIsInvisible;
	}

	return (Ac);
}










/********************************************************************************************

>	OpRedrawColours::OpRedrawColours()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/1/96

	Purpose:	OpRedrawColours constructor

********************************************************************************************/

OpRedrawColours::OpRedrawColours()
{
}



/********************************************************************************************

>	OpRedrawColours::~OpRedrawColours() 

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/1/96

	Purpose:	OpRedrawColours destructor

********************************************************************************************/

OpRedrawColours::~OpRedrawColours()
{
}



/********************************************************************************************

>	BOOL OpRedrawColours::Init()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/1/96

	Returns:	TRUE if the operation could be successfully initialised 
				FALSE if no more memory could be allocated 
				
	Purpose:	OpRedrawColours initialiser method

********************************************************************************************/

BOOL OpRedrawColours::Init()
{
	return (RegisterOpDescriptor(
	 								0,
	 								_R(IDS_OPREDRAWCOLOURS),
									CC_RUNTIME_CLASS(OpRedrawColours),
	 								OPTOKEN_REDRAWCOLOURS,
	 								OpRedrawColours::GetState,
	 								0,	/* help ID */
	 								0,	/* Bubble help ID */
	 								0	/* bitmap ID */
	 							));
}
    


/********************************************************************************************

>	OpState	OpRedrawColours::GetState(String_256*, OpDescriptor*)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/1/96

	Returns:	The state of the OpRedrawColours operation
	Purpose:	For finding the OpRedrawColours's state. 

********************************************************************************************/

OpState	OpRedrawColours::GetState(String_256* UIDescription, OpDescriptor*)
{
	OpState OpSt;
 	return(OpSt);
}



/********************************************************************************************

>	void OpRedrawColours::Do(OpDescriptor*)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/1/96

	Purpose:	Generates an ERROR3 and exits - you must ALWAYS use DoWithParam

********************************************************************************************/
			
void OpRedrawColours::Do(OpDescriptor *NotUsed)
{
	ERROR3("OpRedrawColours does not provide a Do() function - Use DoWithParam");
	End();
}



/********************************************************************************************

>	void OpRedrawColours::DoWithParam(OpDescriptor *pOp, OpParam *Param)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/1/96

	Inputs:		pOp - OpDescriptor as for all Do() functions
				Param - points to a OpRedrawColoursInfo containing:
					ScopeDoc - pointer to the scope document
					Colours  - pointer to a null-terminated array of IndexedColour pointers

	Purpose:	Scans the document tree, and redraws all nodes affected by visible
				changes to any of the colours in the provided null-terminated array.

	Scope:		Private - for use only by the ColourManager

	SeeAlso:	ColourManager::ForceRedrawOfChangedColours

********************************************************************************************/
			
void OpRedrawColours::DoWithParam(OpDescriptor *pOp, OpParam *Param)
{
	ERROR3IF(Param == NULL, "Illegal NULL param");
	ERROR3IF(pOurDoc == NULL, "pOurDoc is NULL");

	ObjChangeFlags ChangeFlags;
	ChangeFlags.Attribute = TRUE;

	ObjChangeParam ChangeParam(OBJCHANGE_STARTING, ChangeFlags, NULL, NULL);

	OpRedrawColoursInfo *Info = (OpRedrawColoursInfo *) Param;
	IndexedColour **Colours = Info->Colours;
	ERROR3IF(Colours == NULL, "Illegal NULL param");
	
	if (Info->ScopeDoc != NULL)
		pOurDoc = Info->ScopeDoc;

	Node *CurNode = Node::DocFindFirstDepthFirst(pOurDoc);
	while (CurNode != NULL)
	{
		if (CurNode->IsAnAttribute())
		{
			// Have found a NodeAttribute. Scan it to see if it contains any colours.
			// If any of these match any in our Colours list, then force a redraw of
			// our parent node, and return.
			NodeAttribute *pNodeAttr = (NodeAttribute *) CurNode;
			DocColour *pColour = NULL;
			UINT32 Context = 0;

			// CGS ....

			// DMc's multi stage fills do not dynamically update when we use the colour
			// editor (including my custom colour picker control).  This has now been
			// fixed (by me!).  Problem was to do with the fact that the following line
			// pColour = pNodeAttr->EnumerateColourFields(Context++);
			// never ever got a chance to look for a fill ramp.
			
			BOOL DoneFillRampFirst = FALSE;		// by default, we are NOT doing fillramp stuff
			BOOL FirstPass = TRUE;				// were on our first pass

			do
			{
				// do fill ramp stuff first, before anything else ....
				// (dosn't matter if we the node does not have a fill ramp, since
				// DoneFillRampFirst will NOT be held high (so processing will proceed as
				// normal)

				// NOTE:  I could have modifed EnumerateColourFields so that it worked properly
				// (thereby doing the fill ramp stuff correctly) instead of the following code
				// BUT when doing so, this seemed to have undesired affects, so for now; it is
				// done this way ....
				
				if ((CurNode->IsAFillAttr ()) && (DoneFillRampFirst == FALSE))
				{
					AttrFillGeometry* pAttrFillGeo = (AttrFillGeometry*) (CurNode);

					if (pAttrFillGeo->IsAColourFill () && !(pAttrFillGeo->IsATranspFill ()))
					{
						FillRamp* pFillRamp = pAttrFillGeo->GetFillRamp ();

						if (pFillRamp != NULL)
						{
							UINT32 NumberSelected = pFillRamp->CountSelBlobs ();
					
							if (NumberSelected > 0)
							{
								INT32 SelectedIndex = pFillRamp->GetSelectedIndex ();
								ColRampItem* TheItem = (ColRampItem*) pFillRamp->GetValidIndexedItem (SelectedIndex);

								if (TheItem != NULL)
								{
									DoneFillRampFirst = TRUE;
									pColour = TheItem->GetColourAddr ();
								}
							}
						}
					}
				}
				
				// Get the next colour field from the attribute, and find the
				// IndexedColour (if any) to which it refers
				if (DoneFillRampFirst != TRUE)
				{
					pColour = pNodeAttr->EnumerateColourFields(Context++);
				}
				else
				{
					// need to reset stuff, cause otherwise pColour is always != NULL,
					// and we get into an infinite while loop !!!!
					
					if (FirstPass == TRUE)
					{
						FirstPass = FALSE;		// for next pass ....
					}
					else
					{
						pColour = NULL;			// reset !!!!
					}
				}
		
				IndexedColour *pColourIx = (pColour == NULL) ? NULL : pColour->FindParentIndexedColour();
				if (pColourIx != NULL)
				{
					// Check if this colour matches any in our list
					INT32 i = 0;
					while (Colours[i] != NULL)
					{
						if (pColourIx == Colours[i])
						{
							Node *Parent = CurNode->FindParent();  
							if (Parent != NULL && Parent->IsBounded())
							{
								// Invoke AllowOp on the node to tag it so that parents such as blends
								// and moulds will also redraw if necessary. We ignore the return code
								// because we're not actually going to touch the node at all.
								CurNode->AllowOp(&ChangeParam);

								// And redraw the directly affected node (our parent)
								DocRect NodeBounds = ((NodeRenderableBounded *)Parent)->GetBoundingRect();
								NodeBounds = NodeBounds.Union(((NodeRenderableBounded *)Parent)->GetEffectStackBounds());
								pOurDoc->ForceRedraw(Parent->FindParentSpread(), NodeBounds, FALSE, Parent);
							}

							// There is no need to check further colour attributes for this node
							// now that we have found one that has changed
							break;
						}

						i++;			// Check the next IndexedColour
					}
				}
			} while ((pColour != NULL /*&& DoneFillRampFirst == FALSE*/));	// Check the next colour attribute
		}

		CurNode = CurNode->DocFindNextDepthFirst();
	}
	
	// And make sure that the redraw occurs right away
	pOurDoc->FlushRedraw();
	GetApplication()->ServiceRendering();	// Ensure speedy response by doing one iteration of rendering right now

	ChangeParam.Define(OBJCHANGE_FINISHED, ChangeParam.GetChangeFlags(), NULL, NULL);
	UpdateAllChangedNodes(&ChangeParam);

	End();
}


#endif // EXCLUDE_FROM_RALPH
