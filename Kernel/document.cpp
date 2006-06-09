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

#include "camtypes.h"

#include <fstream>
//#include <strstream> // this seems to be deprecated
#include <ctype.h>
#include <string>   
#include <stdlib.h>  	// for rand() fn

#include "camdoc.h"
//#include "document.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "docmsgs.h" - in camtypes.h [AUTOMATICALLY REMOVED]

#include "dumbnode.h"
#include "noderect.h"
//#include "ensure.h"  - in camtypes.h [AUTOMATICALLY REMOVED]
#include "lineattr.h"        
#include "colcontx.h"
//#include "doccolor.h"
//#include "colourix.h"
//#include "docview.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "readeps.h"
//#include "app.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "errors.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "docview.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "csrstack.h"
#include "grid.h"
#include "nodedoc.h"
#include "chapter.h"
#include "page.h"
//#include "spread.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "wrkrect.h"
//#include "fixmem.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "msg.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "sprdmsg.h"
#include "layer.h"
#include "insertnd.h"
//#include "attrmgr.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "camview.h"
//#include "native.h"			// The new designed native filter, used for v2
//#include "nativeps.h"		// The old style EPS native filter, used in v1.1
//#include "saveeps.h"
#include "zoomops.h"
//#include "qualops.h"
//#include "ai_eps.h"
//#include "ai_epsrr.h"
#include "fontlist.h"
#include "fontman.h"
#include "progress.h"
#include "bmpcomp.h"
#include "ralphdoc.h"
#include "camelot.h"		// For SafeRender Critical section stuff
//#include "clipint.h"
#include "nodedoc.h"
#include "ngsentry.h"
#include "opnudge.h"
#include "ophist.h"
#include "cutop.h"

//#include "ralphvw.h"
#include "vstate.h"

DECLARE_SOURCE("$Revision$");

CC_IMPLEMENT_DYNAMIC(Document, BaseDocument)
CC_IMPLEMENT_DYNAMIC(DocChangingMsg, Msg)
CC_IMPLEMENT_DYNAMIC(SpreadMsg, Msg)
CC_IMPLEMENT_DYNAMIC(TranspModeMsg, Msg)
CC_IMPLEMENT_DYNAMIC(SafeRenderPointer, CCObject)

// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW

const MILLIPOINT PageWidth  = ( 8 * 72000);
const MILLIPOINT PageHeight = (11 * 72000);

// Default page size/position.
const MILLIPOINT Gap = 72000L;
const MILLIPOINT PasteBoardWidth  = PageWidth + (Gap * 2);
const MILLIPOINT PasteBoardHeight = PageHeight + (Gap * 2);
const MILLIPOINT DefBleedSize = 72000L/4;


Document* Document::Current  = NULL;
Document* Document::Selected = NULL;

Spread*	  Document::pGlobalSelSpread = NULL;

BOOL Document::fRestoreViewOnImport = TRUE;


static String_256 ClipboardNameText(TEXT("Clipboard"));

// the default nudge size for our documents ....
// NOTE:  if you change this, you should also update the hard-coded value within:
// GeneralRecordHandler::HandleTagNudeSizeRecord ().  I could move this to the .h; but I can't
// be bothered ....
const UINT32 DEFAULT_NUDGE_SIZE = 2835;



/***********************************************************************************************

> 	Document::Document(BOOL IsAHiddenDoc = FALSE)

    Author: 	Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
    Created:	19/5/93
	Inputs:		IsAHiddenDoc - FALSE for a normal document (see below)
								TRUE for a hidden document (including clipboards, etc)

    Purpose:    Constructor. Initialises the link to the OIL document.

    Notes:		You MUST call Document::Init after Creation of the document, to fill in
				the defaults and build the document tree root.

				HIDDEN DOCUMENTS
				A hidden document is one which is not shown to the user in any UI.
				Hidden docs are used mainly for clipboard (and OLE) data transfer operations.

				Note that if you pass in (pCCamDoc = NULL) then the document is forced to be hidden.
				SeeAlso IsAHiddenDoc() and IsNotAHiddenDoc() in document.h

				The InternalClipboard is now based upon Document. It creates a Hidden Document.
				Generally nobody should care about this, but some bits of code might have to be
				careful of the differences. Use IsAClipboard() to check if the doc is really a
				clipboard. Note also that Clipboard documents do not broadcast DocChangingMsgs.

	Errors:		None.
	SeeAlso:	Document::Init; InternalClipboard; BaseDocument

***********************************************************************************************/

Document::Document(BOOL bIsAHiddenDoc)
  :	BaseDocument(bIsAHiddenDoc),
	OilDoc(0)
{
	// NB. Make sure all the Document stuff is set up before calling routines which
	// might interrogate the Document object.
	
	// Set the default state of the flags.
	DocFlags.LayerMultilayer = FALSE;
	DocFlags.LayerAllVisible = FALSE;
	DocFlags.SaveWithUndo = TRUE;

	// Set the times
	CreationTime = ::time(NULL);
	LastSaveTime = 0;

	// Set up default bleed offset
	BleedOffset = DefBleedSize;

	// Set up our array of default colour contexts by copying the 'global' array
//#pragma message( __LOCMSG__ "DefaultColourContexts remove - no drawing yet" )
	ColourContext::GetGlobalDefaults( &DefaultColourContexts );

	// Add this document to the list of all documents in Camelot
#ifdef RALPH
	Camelot.Documents.AddHead(this);
#else
	if (IsNotAHiddenDoc())
	{
		Camelot.Documents.AddHead(this);
	}
#endif

	// Set this document to be the current one
	this->SetCurrent();

	NodesInTree = 0;       
	                             
	// Initialise pointers to sensible values (they are set up in the Init() function)
	OpHistory 			= 0;
	AttributeMgr 		= 0;
	m_pSetSentinel		= 0;
	pDocUnitList 		= 0;
	pSelSpread			= 0;

	IsARalph = FALSE;
	pRalphDocument =NULL;
	Viewmode = DOCUMENT_VIEW;

	// Always keep this flag TRUE except when Loading New Files in
	DocIsImporting = TRUE;

	// Always keep this False unless loading a template document
	DocTemplateLoading = FALSE;

	// Make sure that the loaded as old version file is set to a known state
	LoadedAsVersion1File = FALSE;

	m_docNudge = DEFAULT_NUDGE_SIZE;
	SetBitmapSmoothing(TRUE);
	m_DuplicationOffset = DocCoord(OpDuplicate::DuplicatePlacementX, OpDuplicate::DuplicatePlacementY);

	// Document is not yet "born and stable".
	m_fStable = FALSE;
}



/********************************************************************************************

>	BOOL Document::Init(CCamDoc* pOilDoc)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/7/93
	Inputs:		pOilDoc		---		the CCamDoc to attach this Document too
	Returns:	TRUE if the document initialised ok, FALSE otherwise.
	Purpose:	Sets up the document tree.  At the moment it fills it with test data
				(paths etc) but in future only the initial tree nodes will be set up.
				When the document has been completely initialised, it becomes the
				selected and current document: DocChangingMsg message broadcasts will
				occur (in the order BORN, CURRENT, SELECTED).

	Notes:		This now also sets up the default list of indexed (named) colours which
				the user can base their document upon [in the BaseDocument Init method].
				These should be set up from a default document (or whatever) in future.

	Errors:		Out of memory.
	SeeAlso:	ReadEPSData; Document::Document;
				Document::SetSelectedViewAndSpread; Document::SetCurrent

********************************************************************************************/

BOOL Document::Init(CCamDoc* pOilDoc)
{
// Ralph has a NULL oildoc...
#ifndef RALPH
	ERROR2IF(OilDoc, FALSE, "Document already attached to a CCamDoc in Document::Init");
#endif
	OilDoc = pOilDoc;

	NodeDocument* Doc;
	EndDocument* End;

	// Make this the current document - NOTE! no broadcast as of 22/7/94
	SetCurrent();
    
    // Set the busy cursor . . .
    BeginSlowJob(-1, FALSE);

	// First, call the base class Init method to set up for us.
	// This creates the StartDocument, NodeDocument, & EndDocument, and sets up
	// a default colour list and other doc components.
	if (!BaseDocument::Init()) return InitFailed();

	// Check that the document is OK
	ERROR3IF(GetFirstNode() == NULL,"StartDoc not found"); 
	Doc = (NodeDocument*)GetFirstNode()->FindNext(); 
	ERROR3IF(Doc == NULL, "NodeDocument not found"); 
	End = (EndDocument*)Doc->FindNext(); 
	ERROR3IF(End == NULL, "EndDocument not found"); 

	// Make the doc unit list
	pDocUnitList = new DocUnitList;
	if (pDocUnitList == NULL || !pDocUnitList->MakeDefaultUnits())
		return(InitFailed());

	// Create the Operation history for the document                              
	OpHistory = new OperationHistory;
	if (OpHistory == NULL)
		return(InitFailed());

	// Create the attribute manager for the document 
	AttributeMgr = new AttributeManager; 
	if (AttributeMgr == NULL || !AttributeMgr->InitInstance())
		return(InitFailed());

	// Build the basic tree structure (Chapter, spread, layer)
	if (!InitTree(Doc))
		return(InitFailed());

	if (IsNotAHiddenDoc())
	{
		// Broadcast a message to all that there's a new document on the block . . .
		BROADCAST_TO_ALL(DocChangingMsg(this, DocChangingMsg::BORN));
	}
    
	// Add the default attributes as Nodes at the start of the tree
	// The clipboard document does not need default attributes, as things copied into it
	// are always kept attribute complete. But now that we're importing into the clipboard,
	// it is a very good idea to have safe defaults lying about.
   	InitDefaultAttributeNodes(); 

	// Take the hourglass off
    EndSlowJob();
    
    return(TRUE);
}



/********************************************************************************************

>	BOOL Document::InitTree(NodeDocument *RootNode)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/4/95

	Inputs:		RootNode - points at the basic NodeDocument from which the main documet
				tree hangs.

	Returns:	TRUE if it succeeds, FALSE if it failed (in which case you should
				call InitFailed() and possibly InformError())

	Purpose:	Called by Document::Init to initialise the document tree structure.
				
				Mainly intended for use by the internal clipboard in order to delete
				and recreate the standard tree whenever it is wiped - I've done this
				here dso that any chnages to the normal document layout will also affect
				the clipboard document in an appropriate manner.

	Errors:		Lack of memory might make 'new' fail

	SeeAlso:	Document::Init

********************************************************************************************/

BOOL Document::InitTree(NodeDocument* pRootNode)
{
	if (pRootNode == NULL || !pRootNode->IsNodeDocument())
	{
		ERROR3("This doc's NodeDocument is NULL or isn't really a NodeDocument");
		return(FALSE);
	}

	// Temporary bodge to construct example document tree for Target 1.		
	Chapter* pChapter = new Chapter(pRootNode, LASTCHILD);
	if (pChapter == 0) return FALSE;

	// Create the sentinel node for all object names within the chapter, and a
	// child NodeBarProperty.
	m_pSetSentinel = new NodeSetSentinel(pChapter, FIRSTCHILD);
	if (m_pSetSentinel == 0) return FALSE;
	NodeBarProperty* pbp = new NodeBarProperty(m_pSetSentinel, LASTCHILD);
	if (pbp == 0) return FALSE;

	// Create a rectangle for the pasteboard
	DocRect PasteRect(MinDocCoord + 0x10000,
	                  MaxDocCoord - 0x10000 - 1 * PasteBoardHeight,
	                  MinDocCoord + 0x10000 + 2 * PasteBoardWidth,
	                  MaxDocCoord - 0x10000);

	// Create the spread
	Spread *pSpread = new Spread(pChapter, FIRSTCHILD, PasteRect);
	if (pSpread == NULL) return FALSE;
	
	// Create the default page and grid (JustinF says: even if we are a hidden document)
	if (!pSpread->CreateDefaultPageAndGrid(TRUE)) return FALSE;

	// Create the Insertion node which must always live as a last child of the selected
	// spreads active layer. The Insertion node holds a pointer to this document so that
	// if for whatever reason the InsertionNode is destroyed (eg. Delete layer) then 
	// we can Inform this document and it can NULLIFY the InsertPos pointer.
	InsertPos = new InsertionNode(this);
	if (InsertPos == NULL) return FALSE;
	InsertPos->AttachNode(pSpread, LASTCHILD);

	return TRUE;
}



/********************************************************************************************

>	BOOL Document::InitDefaultAttributeNodes()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/06/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the default attributes have been added to the tree OK
				FALSE if we run out of memory.
				
	Purpose:	This function adds the document's default attributes as nodes at the start
				of the tree (i.e. as first children of the NodeDocument).

				The Attribute optimisation routines will not work if the document does
				not contain the default attributes.

	Note:		This function must be called after the initial document structure has
				been set up and all default attributes have been registered.


	Errors:		The Out Of Memory Error is set if we run out of memory, FALSE is returned 
				in this situation.
				 

********************************************************************************************/

BOOL Document::InitDefaultAttributeNodes()
{
	ENSURE(AttributeMgr != NULL, "Attribute Manager pointer is NULL"); 

	// Find out the number of default attributes
	UINT32 NumDefaultAttribs = AttributeMgr->GetNumAttributes();
	ENSURE(NumDefaultAttribs != 0, "The document has no default attributes"); 
	
	AttributeEntry* DefaultAttribs = AttributeMgr->GetDefaultAttributes();
	
	if (DefaultAttribs == NULL)
		return FALSE;
	
	// ----------------------------------------------------------------------------------------
	// Try and find the NodeDocument node - to which the attributes will be added

	Node* TreeRoot; 

	ENSURE(TreeStart != NULL, "The initial tree structure has not been initialised"); 

	TreeRoot = TreeStart->FindNext(); 

	ENSURE(TreeRoot != NULL, "Can't find NodeDocument"); 
	ENSURE(TreeRoot->GetRuntimeClass() == CC_RUNTIME_CLASS(NodeDocument), "Alien tree structure"); 

	// ----------------------------------------------------------------------------------------
	// Create a NodeAttribute for each of the default attributes and add them into the
	// tree as first children of the NodeDocument.

	for(UINT32 i=0; i<NumDefaultAttribs; i++)
	{
		ENSURE(DefaultAttribs[i].pAttr != NULL, "Default attribute is NULL"); 
		
		Node* NodeAttr = DefaultAttribs[i].pAttr->MakeNode();

		if (NodeAttr != NULL)
		{
			// Add the Attribute as a first child of the NodeDocument.  Note that we suppress
			// the transparency check for default attributes.
	    	NodeAttr->AttachNode(TreeRoot, FIRSTCHILD, FALSE);

			NodeAttribute* pNa = (NodeAttribute*)NodeAttr;
			if(pNa->IsLinkedToNodeGeometry())
			{
				pNa->NewlyCreatedDefaultAttr((NodeDocument*)TreeRoot);
			}
	    } 
	}

	// ----------------------------------------------------------------------------------------

	// Default attribute array no longer required so we can safely delete it 
	CCFree(DefaultAttribs);
	return TRUE;
}



/********************************************************************************************

>	BOOL Document::ReInit()

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/4/96
	Returns:	Init ok
	Purpose:	Calls InitFailed to delete the Tree and various others and follows this with 
				an Init to put the Document back into a virgin state
				Not used just yet...
	Errors:		

********************************************************************************************/

BOOL Document::ReInit()
{
	// RALPH
	InitFailed();
	return Init(OilDoc);

/* 
	TreeStart->CascadeDelete();
	ERROR3IF(GetFirstNode() == NULL,"StartDoc not found"); 
	NodeDocument *Doc = (NodeDocument*)GetFirstNode()->FindNext(); 
	return InitTree(Doc);
*/

//	return TRUE;

}



/********************************************************************************************

>	BOOL Document::InitFailed(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/12/94
	Returns:	FALSE
	
	Purpose:	Called by Document::Init when it fails; encapsulates all necessary
				tidying up, and then returns FALSE so that it can be called as part
				of the final return statement.
				Replaces some completely unnecessary goto code.

	SeeAlso:	Document::Init

********************************************************************************************/

BOOL Document::InitFailed(void)
{
	if (TreeStart != 0)
	{
		TreeStart->CascadeDelete();
		delete TreeStart;
		m_pSetSentinel = 0;
	}

	if (OpHistory != 0)
	{
		delete OpHistory;
		OpHistory = 0;
	}
	
	if (AttributeMgr != 0)
	{
		delete AttributeMgr;
		AttributeMgr = 0;
	}

	if (pDocUnitList != 0)
	{
		delete pDocUnitList;
		pDocUnitList = 0;
	}
	
	EndSlowJob();
	return(FALSE);
}



/***********************************************************************************************

> 	Document::~Document()

    Author: 	Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
    Created:	19/5/93
    Purpose:    Destructor. Deletes the whole document tree of this object, and all memory
                is returned to the heaps whence it came.  The busy cursor is displayed while
                this is happening.
                   			                                     
***********************************************************************************************/

Document::~Document()
{
	// Set the busy cursor . . .
    BeginSlowJob(-1, FALSE);

#ifndef RALPH
	// Broadcast to everyone that this document is a dead duck.
	if (IsNotAHiddenDoc())
#endif
	{
 		// Document no longer stable.
		SetStable(FALSE);
		BROADCAST_TO_ALL(DocChangingMsg(this, DocChangingMsg::ABOUTTODIE));
	}


#ifdef _DEBUG
		// DocViews should be closed before Documents, so all render regions for this document
		// should have been deleted - make sure they have.
		Camelot.DeleteRenderRegions(this);
#endif

#ifndef RALPH
	// Remove Document from list of all documents
	if (IsNotAHiddenDoc())
#endif
	{
		Camelot.Documents.RemoveItem(this);

		// Is this the last document in the program?
		BOOL ThisIsTheLastDoc = (Camelot.Documents.GetHead() == NULL);

		if (ThisIsTheLastDoc)
		{
			// There are no more documents, so ensure that the system is in the correct state.

			// If the system is working correctly, then the following should be true:
			//   If Current  != NULL, Current  must be this document
			//   If Selected != NULL, Selected must be this document
			// We will ensure this is the case in debug builds

			ENSURE((Current  == NULL) || (Current  == this),"Current document is garbage");
			ENSURE((Selected == NULL) || (Selected == this),"Selected document is garbage");

			// Reset Current and Selected document and DocView
			SetNoCurrent();
			SetNoSelectedViewAndSpread();

			DocView::SetNoCurrent();
	//		DocView::SetNoSelected();	-- now done in SetNoSelectedViewAndSpread(), above

		}
		else
		{
			// Make sure that Current and Selected do not refer to the dying document
			if (this == Current)	SetNoCurrent();
			if (this == Selected)	SetNoSelectedViewAndSpread();
			
			// This special test added to fix a problem whereby current doc was being set to null
			// above and hence causing unit display problems (ERROR2s) in the selector tool when
			// opening a bitmap had been aborted. 18/9/96 Neville
			if (Selected != NULL && Current == NULL) Current = Selected; 
		}			
	}

	// Delete all DocViews attached to document
	DocViews.DeleteAll();

#ifndef RALPH
	// Broadcast to everyone that this document is a dead duck.
	if (IsNotAHiddenDoc())
	{
	 	BROADCAST_TO_ALL(DocChangingMsg(this, DocChangingMsg::KILLED));
	}
#endif	
	// Destroy the operation history. This must get destroyed before the tree is destroyed
	if (OpHistory != NULL)
    	delete (OpHistory);   

	// Destroy the attribute manager 
	if (AttributeMgr != NULL)
		delete (AttributeMgr); 

	// Destroy the list of units in this document
	if (pDocUnitList != NULL)
		delete (pDocUnitList);

	// All done...
    EndSlowJob(); 

	// Note that the tree is deleted in the base class destructor
}



/********************************************************************************************
>	BOOL Document::IsModified() const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/3/94
	Inputs:		-
	Outputs:	-
	Returns:	Returns TRUE if the document has been modified since last being saved.
	Purpose:	Tests the "Modified" flag.
	Errors:		-
	SeeAlso:	CCamDoc::IsModified
********************************************************************************************/

BOOL Document::IsModified() const
{
	if (OilDoc)
		return OilDoc->IsModified();

	return(FALSE);
}



/********************************************************************************************
>	BOOL Document::IsReadOnly() const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/3/94
	Inputs:		-
	Outputs:	-
	Returns:	Returns TRUE if the document is read-only, ie. cannot be modified.
	Purpose:	Tests the "Read Only" flag.
	Errors:		-
	SeeAlso:	CCamDoc::IsReadOnly
********************************************************************************************/

BOOL Document::IsReadOnly() const
{
	if (OilDoc)
		return OilDoc->IsReadOnly();

	return(FALSE);
}



/********************************************************************************************
>	BOOL Document::IsACopy() const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/3/94
	Inputs:		-
	Outputs:	-
	Returns:	Returns TRUE if the document is a copy of another document.
	Purpose:	Tests the "Copy" flag.
	Errors:		-
	SeeAlso:	CCamDoc::IsACopy
********************************************************************************************/

BOOL Document::IsACopy() const
{
	if (OilDoc)
		return OilDoc->IsACopy();

	return(FALSE);
}



/********************************************************************************************
>	void Document::SetModified(BOOL fState = TRUE)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/3/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Sets (or clears) the "document is modified" flag.
	Errors:		-
	SeeAlso:	CCamDoc::SetModified
********************************************************************************************/

void Document::SetModified(BOOL fState /* = TRUE */)
{
	if (OilDoc)
		OilDoc->SetModified( fState != FALSE );
}

/********************************************************************************************
>	void Document::IsLoadedAsVersion1File() const

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/7/96
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Returns the current state of the IsLoadedAsVersion1File flag.
				Flag to say that the document was loaded as a version 1 file
				and so the user needs to be given the option of saving it in this form
				when the document is saved
********************************************************************************************/

BOOL Document::IsLoadedAsVersion1File() const
{
	return LoadedAsVersion1File;
}

/********************************************************************************************
>	BOOL Document::ISetLoadedAsVersion1File(BOOL NewState)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/7/96
	Inputs:		New state of the flag required
	Outputs:	-
	Returns:	Old state of the flag
	Purpose:	Sets a new current state of the IsLoadedAsVersion1File flag.
				Flag to say that the document was loaded as a version 1 file
				and so the user needs to be given the option of saving it in this form
				when the document is saved
********************************************************************************************/

BOOL Document::SetLoadedAsVersion1File(BOOL NewState)
{
	BOOL OldState = LoadedAsVersion1File;
	LoadedAsVersion1File =  NewState;
	return OldState;
}





/********************************************************************************************
>	BOOL Document::CreateDragTargets()

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/1/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Calls all (OILY) views to create their drag targets.
	Errors:		-
	SeeAlso:	CCamDoc::SetModified
********************************************************************************************/

BOOL Document::CreateDragTargets(DragInformation * DragInfo)
{
	if (IsNotAClipboard())
	{
		DocView * Ptr= (DocView*) DocViews.GetHead();
		CCamView* TheView;

		while(Ptr)
		{
			TheView = Ptr->GetConnectionToOilView();
			if(TheView)
					TheView->CreateDragTarget(DragInfo);
			Ptr= (DocView*) DocViews.GetNext(Ptr);
		}
	}
	return TRUE;
}



/********************************************************************************************

>	String_256 Document::GetTitle() const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/3/94
	Returns:	A String_256 that holds the document title
	Purpose:	Returns the Title of the document. The title is the text that appears in the
				documents windows (when it is not maximised). For example 'Untitled #1' or
				'Untitled #1 (Modified)' or even 'Blobby.art (Modified)'.
	SeeAlso:	Document::GetProducer; Document::GetComment; CCamDoc::GetKernelTitle

********************************************************************************************/

String_256 Document::GetTitle() const
{
	if (OilDoc)
		return OilDoc->GetKernelTitle();

	return(ClipboardNameText);
}


/********************************************************************************************

>	String_256 Document::GetDocName(BOOL IncludeFileType = TRUE) const

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/3/95
	Inputs:		IncludeFileType - TRUE if the filetype is required. Defaults to TRUE.
	Returns:	A String_256 that holds the document name
	Purpose:	Returns the name of the document. The name will be the first part of the title
				which is the text that appears in the document windows (when it is not maximised). For example 'Untitled #1' or
				'Untitled #1 (Modified)' or even 'Blobby.art (Modified)'.
	SeeAlso:	Document::GetTitle(); CCamDoc::GetKernelTitle(); CCamDoc::GetKernelDocName();
	SeeAlso:	Document::GetProducer; Document::GetComment; 

********************************************************************************************/

String_256 Document::GetDocName(BOOL IncludeFileType) const
{
	if (OilDoc)
		return OilDoc->GetKernelDocName(IncludeFileType);

	return(ClipboardNameText);
}	


/********************************************************************************************

>	String_256 Document::GetPathName(UINT32 MaxSize = 0) const

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/3/95
	Inputs:		MaxSize - This is the size we have to fit the pathname into.
				Defaults to zero , meaning all off the pathname.
	Returns:	A String_256 that holds the pathname for the document.
	Purpose:	Returns the pathname of the document.
	SeeAlso:	CCamDoc::GetKernelPathName; PathName::GetTruncatedPathName;
	SeeAlso:	Document::GetLocation; CCamDoc::GetTruncatedLocation;

********************************************************************************************/

String_256 Document::GetPathName(UINT32 MaxSize) const
{
	if (OilDoc)
		return OilDoc->GetKernelPathName(MaxSize);

	return(ClipboardNameText);
}

/********************************************************************************************

>	String_256 Document::GetLocation(UINT32 MaxSize = 0) const

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/3/95
	Inputs:		MaxSize - This is the size we have to fit the pathname into.
				Defaults to zero , meaning all off the pathname.
	Returns:	A String_256 that holds the location for the document.
	Purpose:	Returns the location of the document, which is the pathname with no filename.
	SeeAlso:	CCamDoc::GetKernelLocation; PathName::GetTruncatedLocation;
	SeeAlso:	Document::GetPathName; CCamDoc::GetKernelPathName;

********************************************************************************************/

String_256 Document::GetLocation(UINT32 MaxSize) const
{
	if (OilDoc)
		return OilDoc->GetKernelLocation(MaxSize);

	return(ClipboardNameText);
}


/********************************************************************************************
>	const String_256& Document::GetProducer() const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/3/94
	Inputs:		-
	Outputs:	-
	Returns:	A reference to the (constant) producer's name of this document.
	Purpose:	
	Errors:		-
	SeeAlso:	Document::GetTitle; Document::GetComment
********************************************************************************************/

const String_256& Document::GetProducer() const
{
	return Producer;
}



/********************************************************************************************
>	const String_256& Document::GetComment() const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/3/94
	Inputs:		-
	Outputs:	-
	Returns:	A reference to the (constant) producer's name of this document.
	Purpose:	Getting the time the document was created.
	Errors:		-
	SeeAlso:	Document::GetTitle; Document::GetProducer
********************************************************************************************/

const String_256& Document::GetComment() const
{
	return Comment;
}



/********************************************************************************************
>	void Document::SetComment(String_256* NewComment)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/8/94
	Inputs:		A pointer to the new comment string.
	Outputs:	-
	Returns:	-
	Purpose:	Setting the document's comment string.
	Errors:		-
	SeeAlso:	Document::GetComment
********************************************************************************************/

void Document::SetComment(String_256* NewComment)
{
	Comment = *NewComment;
}



/********************************************************************************************
>	const time_t& Document::GetCreationTime() const

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/08/94
	Inputs:		-
	Outputs:	-
	Returns:	A reference to the time of the document's creation.
	Purpose:	
	Errors:		-
	SeeAlso:	Document::GetLastSaveTime
********************************************************************************************/

const time_t& Document::GetCreationTime() const
{
	return CreationTime;
}



/********************************************************************************************
>	const time_t& Document::GetLastSaveTime() const

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/08/94
	Inputs:		-
	Outputs:	-
	Returns:	A reference to the time of the document's creation.  Will be 0 if the
				document is unsaved.
	Purpose:	Getting the time the document was last saved.
	Errors:		-
	SeeAlso:	Document::GetCreationTime
********************************************************************************************/
				
const time_t& Document::GetLastSaveTime() const
{
	return LastSaveTime;
}



/********************************************************************************************
>	void Document::SetLastSaveTime() const

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/09/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Call on saving a document.  Sets the document's last saved time.
	Errors:		-
	SeeAlso:	Document::GetLastSaveTime
********************************************************************************************/

void Document::SetLastSaveTime()
{
	LastSaveTime = ::time(NULL);
}

/********************************************************************************************
>	void Document::SetLastSaveTime(time_t)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/7/96
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Allows setting of the document's last saved time.
	Errors:		-
	SeeAlso:	Document::GetLastSaveTime
********************************************************************************************/

void Document::SetLastSaveTime(time_t NewTime)
{
	LastSaveTime = NewTime;
}

/********************************************************************************************
>	void Document::SetCreationTime(time_t)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/7/96
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Allows setting of the document's creation time.
	Errors:		-
	SeeAlso:	Document::GetLastSaveTime
********************************************************************************************/

void Document::SetCreationTime(time_t NewTime)
{
	CreationTime = NewTime;
}

/********************************************************************************************
>	void Document::MakeJustCreated()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/5/95
	Inputs:		-
	Outputs:	Sets member variables
	Returns:	-
	Purpose:	Resets member variables of the document so it abbears as if the document has
				just been created (eg CreationTime is set to the current time)
	Errors:		-
	SeeAlso:	-
********************************************************************************************/
void Document::MakeJustCreated()
{
	LastSaveTime = 0;				// Zero means not saved
	CreationTime = ::time(NULL);
}



/********************************************************************************************

>	void Document::InsertNewNode( Node* NewNode, DocCoord* Point )

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/9/93
	Inputs:		NewNode - A Pointer to the New node to be inserted
				pDestSpread - pointer to the spread  which this new node should be
							  inserted, If this is NULL then the object is inserted
							  on the selected spread.

	Purpose:	Inserts the Node into the Document at the position specified.
				The node is added to the active layer on this spread 
	SeeAlso:	Document::FindEnclosingChapter(); Document::FindEnclosingPage()

********************************************************************************************/

void Document::InsertNewNode( Node* NewNode, Spread *pDestSpread )
{
	SetCurrent();

	Node *pNode; 

	// Most of the time we will be inserting onto the selected spread
	// So find out what this is.
	
	if ((pDestSpread == NULL) || (pDestSpread == Document::GetSelectedSpread()))
	{
		// We need to insert the node onto the selected spread. This is very quick because we
		// keep an Insertion node there.
		pNode = GetInsertionPosition(); 
	   	ENSURE(pNode != NULL, "Could not find insertion position in Document::InsertNewNode()");
		// Attach the node to the left of the insertion node
		NewNode -> AttachNode( pNode, PREV );

	} 
	else
	{
		// This is assuming that we are inserting into the non-selected spread and so does
		// not use the insertion position. Unfortunately, there is a chance that this MAY happen
		// as the selected spread may be in a different document and so we are inserting into a
		// single spread document with an insertion node. The code below will insert AFTER the
		// insertion node and so will be in an illegal position. We really should check this!!!
		// As we are close to release I will put an error 3 in just in case. Neville 24/9/96
		TRACE( _T("\aDocument::InsertNewNode inserting into the unselected spread. Is this correct?\n") );

		pNode = pDestSpread->FindActiveLayer();
		ENSURE(pNode != NULL, "Could not find layer in Document::InsertNewNode()");
		// Attach this node to the tree at this point
		NewNode -> AttachNode( pNode, LASTCHILD );
	}
}



/***********************************************************************************************

 	DocView *Document::GetSpareDocView()

    Author: 	Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
    Created:	19/5/93
	
	Inputs:		- 
    Outputs:    -
    Returns:   	NULL, or a spare DocView
                 
    Purpose:    Looks for a DocView with no attached OilView, so we can recycle it
               
	Errors:		None.
                   			                                     
***********************************************************************************************/

DocView* Document::GetSpareDocView()
{
	if (IsNotAClipboard())
	{
		DocView *View = (DocView *) DocViews.GetHead();
	
		while (View != NULL)
		{
			CCamView *pCamView = View->GetConnectionToOilView();
			if (pCamView == NULL) return View;
			View = (DocView *) DocViews.GetNext(View);
		}
	}

	return NULL; 		// No free views found
}



/***********************************************************************************************

> 	DocView* Document::GetNewDocView()

    Author: 	Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
    Created:	19/5/93
	
	Inputs:		- 
    Outputs:    -
    Returns:   	-
                 
    Purpose:    Returns a new DocView for the client.  If any unconnected views already exist
    			for this document, the first is returned.  Otherwise a new DocView is created
    			and returned.
				If a new DocView is obtained, it becomes the 'current' DocView before
				it is returned to the caller.
               
	Errors:		None.
                   			                                     
***********************************************************************************************/

DocView* Document::GetNewDocView()
{
	// Is there a spare view knocking about?
	DocView *pDocView = GetSpareDocView();
	
	if (pDocView != NULL)
		// Yes - so return it.
		return pDocView;

	if (IsAClipboard())
	{
		TRACEUSER( "Jason",_T("\n\nXX Attempting to add a DocView to a clipboard!\n") );
	}
	else
	{
		// No views spare - create a new one connected to this document...
		pDocView = new DocView(this);
	
		if (pDocView != NULL)
		{
			if (pDocView->Init())
				DocViews.AddTail(pDocView);		// link it into the DocView list ...
			else
			{
				delete pDocView;
				pDocView=NULL;
			}
		}

		// Make it the current DocView
		pDocView->SetCurrent();
	}

	// ...and return this new view to the caller.		
	return pDocView;
}



void Document::OnDocViewDying(DocView *pDocViewToDelete)
{
	// Look for this DocView in the list
	DocView *pDocView = (DocView *) DocViews.GetHead();
	
	// Traverse List of DocViews 
	while (pDocView != NULL)
	{
		// Is this the DocView that is closing down?
		if (pDocView == pDocViewToDelete)
		{
			// Yes - remove it from the list and return
			DocViews.RemoveItem(pDocView);
			return;
		}

		// Try the next one
		pDocView = (DocView *) DocViews.GetNext(pDocView);
	}
}



/***********************************************************************************************
>	void Document::ShowViewScrollers(BOOL fIsVisible)
	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/11/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Show or Hide Scollers on all Views
	SeeAlso:	-
*********************************************************************************************/

void Document::ShowViewScrollers(BOOL fIsVisible)
{
	if (IsNotAClipboard())
	{
		DocView* pDocView = (DocView*) DocViews.GetHead();
		while (pDocView != NULL)
		{
			pDocView->ShowViewScrollers(fIsVisible);
			pDocView = (DocView *) DocViews.GetNext(pDocView);
		}
	}
}

/***********************************************************************************************
>	void Document::ShowViewRulers(BOOL fIsVisible)
	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com> 
	Created:	2/11/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Show or Hide Rulers all Views
	SeeAlso:	-
*********************************************************************************************/

void Document::ShowViewRulers(BOOL fIsVisible)
{
#if !defined(EXCLUDE_FROM_RALPH)
	if (IsNotAClipboard())
	{
		DocView* pDocView = (DocView*) DocViews.GetHead();
		while (pDocView != NULL)
		{
			pDocView->ShowViewRulers(fIsVisible);
			pDocView = (DocView *) DocViews.GetNext(pDocView);
		}
	}
#endif
}

/***********************************************************************************************

> 	CCamDoc* Document::GetOilDoc() const

    Author: 	Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
    Created:	19/5/93
	
	Inputs:		- 
    Outputs:    -
    Returns:   	NULL or a pointer to the CCamDoc object associated with this object.

    Purpose:    Find the Oil document associated with the Document.

	Notes:		IMPORTANT: Clipboard Documents may NOT have an attached OilDoc -> will return NULL

	Errors:		None.
                   			                                     
***********************************************************************************************/

CCamDoc* Document::GetOilDoc() const
{
	return OilDoc;
}



/***********************************************************************************************

> 	static Document* Document::GetCurrent()

    Author: 	Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
    Created:	21/5/93
	
	Inputs:		- 
    Outputs:    -
    Returns:   	Pointer to the current Document object.
                 
    Purpose:    Find the current Document object.
               
	Errors:		None.
                   			                                     
***********************************************************************************************/

Document* Document::GetCurrent()
{
#ifdef RALPH

	if(::GetCurrentThreadId() == RalphDocument::GetImportingThreadID())
	{
		//TRACEUSER( "Richard", _T("+++++ GetCurrent called from Load Thread\n"));
		return RalphDocument::GetImportingDoc();
	}
#endif
	//TRACEUSER( "Richard", _T(">>> Document::GetCurrent returning Current\n"));
	return Current;
}



/***********************************************************************************************

> 	static Document* Document::GetSelected()

    Author: 	Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
    Created:	21/5/93
	
	Inputs:		- 
    Outputs:    -
    Returns:   	Pointer to the selected Document object.
                 
    Purpose:    Find the selected Document object.
               
	Errors:		None.
                   			                                     
***********************************************************************************************/

Document* Document::GetSelected()
{
#ifdef RALPH
	// if we are being called from the load thread return the current load thread document
	if(::GetCurrentThreadId() == RalphDocument::GetImportingThreadID())
	{
		//TRACEUSER( "Richard", _T("+++++ GetSelected called from Load Thread\n"));
		return RalphDocument::GetImportingDoc();
	}
#endif
	//TRACEUSER( "Richard", _T(">>> Document::GetSelected returning Selected\n"));
	return Selected;
}


	
/***********************************************************************************************

> 	void Document::SetCurrent()
    Author: 	Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
    Created:	21/5/93
	Inputs:		- 
    Outputs:    -
    Returns:   	-
    Purpose:    Make this object be the 'current' document.  
				**** Changed 22/7/94 by MarkN ****
    			Does NOT broadcasts a message to all that this is happening any more.
	Errors:		None.
                   			                                     
***********************************************************************************************/

void Document::SetCurrent()
{
#ifdef RALPH
	// if we are being called from the load thread just ignore 
	if(::GetCurrentThreadId() == RalphDocument::GetImportingThreadID())
		return ;
#endif
	Current = this;
}



/***********************************************************************************************

> 	static void Document::SetNoCurrent()

    Author: 	Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
    Created:	21/5/93
	
	Inputs:		- 
    Outputs:    -
    Returns:   	-
                 
    Purpose:    Set the current document pointer to be NULL, i.e., there is no current
    			document object.
				**** Changed 22/7/94 by MarkN ****
    			Does NOT broadcasts a message to all that this is happening any more.
               
	Errors:		None.
                   			                                     
***********************************************************************************************/

void Document::SetNoCurrent()
{
#ifdef RALPH
	// if we are being called from the load thread just ignore 
	if(::GetCurrentThreadId() == RalphDocument::GetImportingThreadID())
		return ;
#endif
	Current = NULL;
}



/***********************************************************************************************

>	void Document::SetNoSelectedViewAndSpread(void)

    Author: 	Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
    Created:	13/2/95
	
	Inputs:		-
    Outputs:    -
    Returns:   	-
                 
    Purpose:	To set the Selected Document, DocView, and Spread.
				This sets all three of these entities to NULL - no selected entity.

				These 3 items are interlinked - changing the spread may mean changing
				to a different document and view - these changes can cause several messages
				(DocChangingMsg, SpreadMsg, etc) to be broadcast, so they must be treated
				as a single indivisble operation, which only broadcasts the necessary
				messages when the entrie state has been changed.

	Notes:		If the Selected docuemnt changes, a DocChangingMsg (SELCHANGED) will be 
				broadcast, with pOldDoc == the last selected Doc, and pNewDoc == the new
				selected document. Either/both of pOldDoc and pNewDoc may be NULL!

				If the Selected DocView changes, a DocViewMsg (SELCHANGED) will be broadcast,
				with the pOldDocView and pNewDocView fields indicating the old and new DocViews.

				If the Selected spread changes, a SpreadMsg (SELCHANGED) will be broadcast.
				pOldSpread and pNewSpread will point at the spreads involved.

				This does NOT affect the Current doc.

***********************************************************************************************/

void Document::SetNoSelectedViewAndSpread(void)
{
	TRACEUSER("Gerry", _T("Document::SetNoSelectedViewAndSpread"));
#ifdef RALPH
	// if we are being called from the load thread just ignore 
	if(::GetCurrentThreadId() == RalphDocument::GetImportingThreadID())
		return ;
#endif
	// Remember if each of the selected things has chnaged as a result of this call
	Document	*OldSelDocument	= Selected;
	DocView		*OldSelView		= DocView::GetSelected();
	Spread		*OldSelSpread	= pGlobalSelSpread;

	// Tell those that care that the selected view is about to become null.
	BROADCAST_TO_ALL(DocViewMsg(OldSelView, NULL, DocViewMsg::SELABOUTTOCHANGE));

	// --- Set no selected spread
	pGlobalSelSpread = NULL;
	if (Selected != NULL)
		Selected->pSelSpread = NULL;

	// --- Set no selected document
	Selected = NULL;

	// --- Set no selected docview
	DocView::SetSelectedInternal(NULL);


	// --- And broadcast appropriate message(s)
	if (Selected != OldSelDocument)
	{
		BROADCAST_TO_ALL(DocChangingMsg(OldSelDocument, NULL,
										DocChangingMsg::SELCHANGED));
	}

	if (DocView::GetSelected() != OldSelView)
	{
		DocView::SendSelectedMessage(OldSelView, DocView::GetSelected());
	}

	if (pGlobalSelSpread != OldSelSpread)
	{
		BROADCAST_TO_ALL(SpreadMsg(OldSelSpread, NULL,
							SpreadMsg::SELCHANGED));
	}

	// Ensure any delayed Selection message broadcasts are sent now
	GetApplication()->FindSelection()->BroadcastAnyPendingMessages();
}



/***********************************************************************************************

>	static void Document::SetSelectedViewAndSpread(Document *TheDocument = NULL,
													DocView *TheView = NULL,
													Spread *TheSpread = NULL);

    Author: 	Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
    Created:	13/2/95
	
	Inputs:		TheDocument - NULL, or a pointer to the document to make selected
				TheView - NULL, or the docview for TheDocument which should be made selected
				TheSpread - NULL or the Spread in TheDocument which should be made selected

    Outputs:    -
    Returns:   	-
                 
    Purpose:	To set the Selected Document, DocView, and Spread.

				These 3 items are interlinked - changing the spread may mean changing
				to a different document and view - these changes can cause several messages
				(DocChangingMsg, SpreadMsg, etc) to be broadcast, so they must be treated
				as a single indivisble operation, which only broadcasts the necessary
				messages when the entrie state has been changed.

	Notes:		If any of the parameters are NULL, sensible defaults (The currently selected
				document, the currently selected or first docview of that document, and
				the currently selected or first spread of that document, respectively) will
				be used.

				If the Selected document changes, a DocChangingMsg (SELCHANGED) will be 
				broadcast, with pOldDoc == the last selected Doc, and pNewDoc == the new
				selected document. Either/both of pOldDoc and pNewDoc may be NULL.

				If the Selected DocView changes, a DocViewMsg (SELCHANGED) will be broadcast,
				with the pOldDocView and pNewDocView fields indicating the old and new DocViews.

				If the Selected spread changes, a SpreadMsg (SELCHANGED) will be broadcast.
				pOldSpread and pNewSpread will point at the spreads involved.
               
				This also sets CurrentDoc to equal the Selected Doc. Ths state should
				remain true during the broadcast of the DocChanging Message.

	Errors:		ERROR2's will be generated if the parameters are incompatible (TheView
				must be a view onto TheDocuemnt; TheSpread must lie within TheDocument)

***********************************************************************************************/

void Document::SetSelectedViewAndSpread(Document *TheDocument,
											DocView *TheView,
											Spread *TheSpread)
{
	TRACEUSER("Gerry", _T("Document::SetSelectedViewAndSpread(0x%08x, 0x%08x, 0x%08x)"), TheDocument, TheView, TheSpread);
#ifdef RALPH
	// if we are being called from the load thread just ignore 
	if(::GetCurrentThreadId() == RalphDocument::GetImportingThreadID())
		return ;
#endif
	// We must remember if each of the selected things has changed as a result of this call
	Document	*OldSelDocument	= Selected;
	DocView		*OldSelView		= DocView::GetSelected();
	Spread		*OldSelSpread	= pGlobalSelSpread;

	// Ensure that all parameters are valid pointers
	if (TheDocument == NULL)
		TheDocument = Selected;

	if (TheDocument == NULL)
	{
		// The caller is obviously trying to set the selected spread/view when there is
		// no valid document around
		ERROR3("Can't set selected spread/view when there is no selected doc!");
		return;
	}

	if (TheView == NULL)
	{
		// The caller supplied no docview - try the selected docview, and if that is not
		// in the document becoming selected, then use the first view for the sel doc.
		TheView = DocView::GetSelected();
		if (TheView == NULL || TheView->GetDoc() != TheDocument)
			TheView = (DocView *) TheDocument->DocViews.GetHead();
	}

	if (TheSpread == NULL)
	{
		// The caller specified no spread - Try the globally selected spread, then the
		// document's memory of the selected spread, and finally, the first spread in the
		// document.
		if (pGlobalSelSpread != NULL && SpreadBelongsToDoc(TheDocument, pGlobalSelSpread))
			TheSpread = pGlobalSelSpread;
		else
		{
			if (TheDocument->pSelSpread != NULL)
				TheSpread = TheDocument->pSelSpread;
			else
				TheSpread = TheDocument->FindFirstSpread();
		}
	}


	// Tell those that care that the selected view is about to change
	if (TheView!=OldSelView)
	{
		BROADCAST_TO_ALL(DocViewMsg(OldSelView, OldSelView, DocViewMsg::SELABOUTTOCHANGE));
	}

	// --- Set the new selected document, and also update CurrentDoc to be this one
	// (so that CurrentDoc points at selected while the broadcast goes around)
	Selected = TheDocument;
	Selected->SetCurrent();

	OpNudge::SetNudgeStep (Selected->GetDocNudge ());	// CGS:  since documents now store the nudge

	// --- Set the new selected DocView
	DocView::SetSelectedInternal(TheView);

	// --- Set the new selected spread
	ERROR3IF(TheSpread != NULL && !SpreadBelongsToDoc(Selected, TheSpread),
				"Selected spread should be in the Selected Doc!");
 	pGlobalSelSpread = TheSpread;
	if (Selected != NULL)
		Selected->pSelSpread = TheSpread;


	// --- Now, check if any have changed, and broadcast appropriate messages
	if (Selected != OldSelDocument)
	{
		BROADCAST_TO_ALL(DocChangingMsg(OldSelDocument, Selected,
							DocChangingMsg::SELCHANGED));
	}

	if (DocView::GetSelected() != OldSelView)
	{
		DocView::SendSelectedMessage(OldSelView, DocView::GetSelected());
	}

	if (pGlobalSelSpread != OldSelSpread)
	{
		BROADCAST_TO_ALL(SpreadMsg(OldSelSpread, pGlobalSelSpread,
							SpreadMsg::SELCHANGED));
	}

	// Ensure any delayed Selection message broadcasts are sent now
	GetApplication()->FindSelection()->BroadcastAnyPendingMessages();
}


	
/***********************************************************************************************

> 	void Document::SetSelected()

    Author: 	Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>/ MarkN
    Created:	21/5/93
	
	Inputs:		- 
    Outputs:    -
    Returns:   	-
                 
    Purpose:	THIS METHOD IS DEFUNCT! DO NOT CALL IT!
				Use Document::SetSelectedViewAndSpread() instead
				(This call is currently indirected to that one, and gives TRACE warnings)
    
    		    Make this object be the 'selected' document object.

				It sets the selected spread to be a relevent spread in this document.
				If there is a change in selected spread as a result, a SpreadMsg::SELCHANGED
				message is broadcast.

				A message is broadcast called SELCHANGED. On receipt of this message, pOldDoc is the one 
				being deselected, and pNewDoc is the one being selected,
				NOTE!! pOldDoc and/or pNewDoc can be NULL!!!
               
				This does NOT effect the Current doc.

				If the new selected document is not the document associated with the selected
				DocView, then DocView::SetNoSelected() is called. This is because the selected
				DocView must be a view onto the selected document.

	Errors:		None.
                   			                                     
***********************************************************************************************/

#if 0

void Document::SetSelected()
{
#if FALSE
/*
	// Defunct code - see below for replacement code
	if (Selected == this) return;
	Document* pOldDoc = Selected;

	Selected = this;

	// The selected document has changed, so ensure that the selected DocView belongs to the new
	// selected document
	// This is based on the rule that the selected DocView MUST be a view onto the selected document
	DocView* pSelDocView = DocView::GetSelected();
	if ((pSelDocView != NULL) && (pSelDocView->GetDoc() != Selected))
		DocView::SetNoSelected();

	if (pSelSpread == NULL)
		pSelSpread = FindFirstSpread();

	BROADCAST_TO_ALL(DocChangingMsg(pOldDoc,Selected,DocChangingMsg::DocState::SELCHANGED));
	Document::SetSelectedSpread(pSelSpread);
*/
#else

	// Now just indirects to the centralised Doc/DocView/Spread selector
	TRACE( _T("WARNING: Illegal call to defunct Document::SetSelected\n"));
	Document::SetSelectedViewAndSpread(this, NULL, NULL);

#endif
}

#endif


/***********************************************************************************************

> 	static void Document::SetNoSelected()

    Author: 	Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
    Created:	21/5/93
	
	Inputs:		- 
    Outputs:    -
    Returns:   	-
                 
    Purpose:	THIS METHOD IS DEFUNCT! DO NOT CALL IT!
				Use Document::SetNoSelectedViewAndSpread() instead
				(This call is currently indirected to that one, and gives TRACE warnings)
    
				Set the selected document pointer to be NULL, i.e., there is no selected
    			document object. 
				This does NOT effect the Current doc.

				**** Changed 22/7/94 by MarkN ****
				A new message is broadcast that combines the SELECTED and UNSELECTED messages
				in one called SELCHANGED. On receipt of this message, pOldDoc is the one being
				deselected, and pNewDoc is the one being selected,
				NOTE!! pOldDoc and/or pNewDoc can be NULL!!!
               
	Errors:		None.
                   			                                     
***********************************************************************************************/

#if 0

void Document::SetNoSelected()
{
#if FALSE
/*
	if (Selected == NULL) return;
	Document* pOldDoc = Selected;

	Selected = NULL;
	Document::SetSelectedSpread(NULL);

	BROADCAST_TO_ALL(DocChangingMsg(pOldDoc,NULL,DocChangingMsg::DocState::SELCHANGED));
*/
#else

	TRACE( _T("WARNING: Illegal call to defunct Document::SetNoSelected\n"));
	Document::SetNoSelectedViewAndSpread();

#endif
}                    

#endif


/***********************************************************************************************

>	static void Document::SetSelectedSpread(Spread* pNewSelSpread)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/8/94
	Inputs:		pNewSelSpread = ptr to the spread that is the new selected spread
	Outputs:	-
	Returns:	-
    Purpose:	THIS METHOD IS DEFUNCT! DO NOT CALL IT!
				Use Document::SetSelectedViewAndSpread() instead
				(This call is currently indirected to that one, and gives TRACE warnings)
    
				Sets the global instance of the selected spread. 
				pNewSelSpread must be a spread belonging to the selected document.
	Errors:		-
	SeeAlso:	-

***********************************************************************************************/

#if 0

void Document::SetSelectedSpread(Spread* pNewSelSpread)
{
#if FALSE
/*
	// if no change return
	if (pGlobalSelSpread == pNewSelSpread) return;

	Spread* pOldSelSpread = pGlobalSelSpread;

	if (Selected == NULL && pNewSelSpread == NULL)
		// If there's no selected document, then there's no selected spread
		pGlobalSelSpread = NULL;
	else
	{
		// Make sure there is at least a selected doc
		ENSURE(Selected != NULL,"Can't set selected spread if there isn't a selected document");

		// if there's a new sel spread, ensure it belongs to the selected doc
		if (pNewSelSpread != NULL)
		{
			ENSURE(SpreadBelongsToDoc(Selected,pNewSelSpread),"Trying to set the selected spread which doesn't belong to the selected doc.");
		}

		// We've ensured that there's a selected doc, and the new sel spread belongs to it

	 	pGlobalSelSpread     = pNewSelSpread;
		Selected->pSelSpread = pNewSelSpread;
	}

	// Clear all current selections, and remove the blobs
	//NodeRenderableInk::DeselectAll();

	// Inform the SelRange that it has changed, get it to broadcast
	//GetApplication()->FindSelection()->Update(TRUE); 

	BROADCAST_TO_ALL(SpreadMsg(pOldSelSpread,pNewSelSpread,SpreadMsg::SpreadReason::SELCHANGED));
*/
#else

	TRACE( _T("WARNING: Illegal call to defunct Document::SetSelectedSpread\n"));
	Document::SetSelectedViewAndSpread(Selected, NULL, pNewSelSpread);

#endif

}

#endif



/***********************************************************************************************

>	static Spread *Document::GetSelectedSpread()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/8/94
	Inputs:		-
	Outputs:	-
	Returns:	The selected spread in the system
	Purpose:	Returns the selected spread
	Errors:		-
	SeeAlso:	-

***********************************************************************************************/

Spread *Document::GetSelectedSpread()
{
#ifdef RALPH
	// if we are being called from the load thread return the first spread in the current load
	// thread document
	if(::GetCurrentThreadId() == RalphDocument::GetImportingThreadID())
	{
		//TRACEUSER( "Richard", _T("+++++ GetSelectedSpread called from Load Thread\n"));
		return RalphDocument::GetImportingDoc()->FindFirstSpread();
	}
#endif
	//TRACEUSER( "Richard", _T(">>> Document::GetSelectedSpread returning pGlobalSelSpread\n"));
	return (pGlobalSelSpread);
}



/***********************************************************************************************

>	static BOOL Document::SpreadBelongsToDoc(Document* pDoc,Spread* pSpread)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/8/94
	Inputs:		pDoc    = ptr to a document
				pSpread = ptr to a spread
	Outputs:	-
	Returns:	TRUE if pSpread is a child of the NodeDocument belonging to pDoc
				FALSE otherwise
	Purpose:	Tests to see if the spread belongs to the document
	Errors:		-
	SeeAlso:	-

***********************************************************************************************/

BOOL Document::SpreadBelongsToDoc(Document* pDoc,Spread* pSpread)
{
	if (pDoc == NULL) return FALSE;

	// Find pDoc's first node
	Node* pNode = pDoc->GetFirstNode();
	ENSURE(pNode != NULL,"No first node!");

	// The next node should be the NodeDocument node
	pNode = pNode->FindNext();
	ENSURE(pNode != NULL,"No next node!");
	ENSURE(pNode->GetRuntimeClass() == CC_RUNTIME_CLASS(NodeDocument),"Next node is not a NodeDocument");

	// pNode should now point to pDoc's NodeDocument node

	// Find pSpread's parent NodeDocument node
	Node* pParentNodeDoc = pSpread->FindParent(CC_RUNTIME_CLASS(NodeDocument));
	ENSURE(pParentNodeDoc != NULL,"The spread has no NodeDocument parent");

	// Return TRUE if pSpread's parent NodeDocument node == pDoc's NodeDocument node

	return ((pParentNodeDoc == pNode) && (pNode != NULL) && (pParentNodeDoc != NULL));
}



/***********************************************************************************************

>	Spread* Document::FindFirstSpread()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/8/94
	Inputs:		-
	Outputs:	-
	Returns:	The first spread in this document.  A document always has at least one spread
	Purpose:	Finds the first spread in this document.
	Errors:		-
	SeeAlso:	-

***********************************************************************************************/

Spread* Document::FindFirstSpread()
{
	Node* pNode = GetFirstNode();
	ERROR3IF(pNode == NULL,"Document::FindFirstSpread No first node!");
	if (pNode == NULL)
		return NULL;

	pNode = pNode->FindNext();
	if (pNode != NULL)
	{
		ERROR3IF(pNode->GetRuntimeClass() != CC_RUNTIME_CLASS(NodeDocument),"Next node is not a NodeDocument");
		pNode = pNode->FindFirstChild();
		if (pNode != NULL)
		{
			if (pNode->GetRuntimeClass() != CC_RUNTIME_CLASS(Chapter))
				pNode = pNode->FindNext(CC_RUNTIME_CLASS(Chapter));
			if (pNode != NULL)
			{
				pNode = pNode->FindFirstChild();
				if (pNode != NULL)
				{
					if (pNode->GetRuntimeClass() != CC_RUNTIME_CLASS(Spread))
						pNode = pNode->FindNext(CC_RUNTIME_CLASS(Spread));

					ERROR3IF(pNode == NULL,"Can't find a spread as a child of Chapter");
				}
				else
					ERROR3("No child of Chapter node");
			}
			else
				ERROR3("Can't find a chapter as a child of NodeDocument");
		}
		else
			ERROR3("No child of NodeDocument node");
	}
	else
		ERROR3("No next node!");

	return ((Spread*)pNode);
}



/***********************************************************************************************

>	void Document::GetExtents(DocCoord* Lo, DocCoord* Hi, DocRect* Extent, View *pView)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/5/93
	Inputs:		pView - the View to pixelise the extents to.
	Outputs:	Lo, Hi - the extents of the document.
	Returns:	-
	Purpose:	Find the extents of the document.  These are in DocCoords - Lo is the
				bottom-left corner of the last chapter, and Hi is the top-right corner
				of the first chapter.
	Errors:		-
	SeeAlso:	-

***********************************************************************************************/

void Document::GetExtents(DocCoord* Lo, DocCoord* Hi, DocRect* Extent, View *pView)
{
	NodeDocument* DocNode = (NodeDocument*) TreeStart->FindNext();
	
	// Just to be on the safe side
	ENSURE(DocNode->IsKindOf(CC_RUNTIME_CLASS(NodeDocument)),
					"Document.cpp: This node is not a NodeDocument");
	
	*Lo = DocNode->LoExtent();
	*Hi = DocNode->HiExtent();
	*Extent = DocNode->GetPasteboardRect(TRUE, pView);
}



/***********************************************************************************************

>	void Document::UpdateExtents(const DocCoord& Lo, const DocCoord& Hi)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/5/93
	Inputs:		Lo, Hi - the new Extents in document coordinates.
	Outputs:	-
	Returns:	-
	Purpose:	Change the document extents.  This will only be done by objects of class
				NodeDocument.  All related DocViews are informed of the change.
	Errors:		-
	SeeAlso:	NodeDocument
	SeeAlso:	GetExtents

***********************************************************************************************/

void Document::UpdateExtents(const DocCoord& Lo, const DocCoord& Hi)
{
	DocView *pView = (DocView*) DocViews.GetHead();

	// Scan list of all DocView of this Document	
	while (pView != NULL)
	{
		// Inform DocView of the new document extent.
		pView->SetExtent(Lo, Hi);
		pView = (DocView*) DocViews.GetNext(pView);
	}
}   



/********************************************************************************************

>	OperationHistory& Document::GetOpHistory()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/7/93
	Inputs:		-
	Outputs:	-
	Returns:	A reference to the documents operation history
	Purpose:	For finding the documents operation history
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

OperationHistory& Document::GetOpHistory()
{
	return (*OpHistory);
}



/********************************************************************************************

>	BOOL Document::EmptyOperationHistory()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/4/95
	Returns:	TRUE if the Operation History was empied ok, FALSE if there is a problem
	Purpose:	This function deletes the operation history, destroying everything in it.
				It then creates a new empty Operation History and sets it up ready for use.
				This function was really created to allow the New and Open Document options
				to get rid of the undo that is created by the file importing.

********************************************************************************************/

BOOL Document::EmptyOperationHistory()
{
	// If there was no Operation History in the first place, then there is a problem
	if (OpHistory==NULL)
		return FALSE;

	// Before we delete th history find out what its maximum size is
	UINT32 MaxSize = OpHistory->GetMaxSize();

	// There is an Operation History, so toast it
	delete OpHistory;
	OpHistory = NULL;

	// Now try and replace it with a new one
	OpHistory = new OperationHistory(MaxSize);
	
	// If we did not get the memory to create a new one, then fail
	if (OpHistory==NULL)
		return FALSE;

	// We now have a nice new empty operation history, so return happy
	return TRUE;
}



/********************************************************************************************

>	AttributeManager& GetAttributeMgr() const; 

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/7/93
	Inputs:		-
	Outputs:	-
	Returns:	A reference to the documents operation history
	Purpose:	For finding the documents operation history
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

AttributeManager& Document::GetAttributeMgr() const
{
	ENSURE(AttributeMgr != NULL,
				"Pointer to Attribute Manager is NULL in Document::GetAttributeMgr()");
	return (*AttributeMgr); 
}




/********************************************************************************************

>	BOOL Document::IsMultilayer()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/8/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the multilayer flag for the document is set. FALSE otherwise
	Purpose:	Interogate the multilayer status of this document
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL Document::IsMultilayer()
{
	return (DocFlags.LayerMultilayer != 0);
}

/********************************************************************************************

>	BOOL Document::IsAllVisible()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/8/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the "all visible" flag for the document is set. FALSE otherwise
	Purpose:	Interogate the "all visible" status of this document
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL Document::IsAllVisible()
{
	return (DocFlags.LayerAllVisible != 0);
}

/********************************************************************************************

>	void Document::SetMultilayer(BOOL state)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/8/94
	Inputs:		state = TRUE or FALSE
	Outputs:	-
	Returns:	-
	Purpose:	Lets you set the state of the multilayer document flag
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void Document::SetMultilayer(BOOL state)
{
	DocFlags.LayerMultilayer = state;
}

/********************************************************************************************

>	void Document::SetAllVisible(BOOL state)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/8/94
	Inputs:		state = TRUE or FALSE
	Outputs:	-
	Returns:	-
	Purpose:	Lets you set the state of the "all visible" document flag
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void Document::SetAllVisible(BOOL state)
{
	DocFlags.LayerAllVisible = state;
}

/********************************************************************************************

>	NodeAttribute* Document::GetDefaultAttr(CCRuntimeClass* RequiredAttr)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/10/93
	Inputs:		RequiredAttr - the type of attribute to look for.
	Returns:	Pointer to the default attribute node, or NULL if the attribute was not 
				found.
	Purpose:	Finds the default attribute node for a document, given the attribute class
				to look for.  For instance, to find out the default line width for a
				document: MonoOn
				
				AttrLineWidth *pAttr = pDoc->GetDefaultAttr(CC_RUNTIME_CLASS(AttrLineWidth));
				MonoOff

********************************************************************************************/

NodeAttribute* Document::GetDefaultAttr(CCRuntimeClass* RequiredAttr)
{
	// Search for line width attribute as a child of the NodeDocument.
	Node *pNode = TreeStart->FindNext();
	
	if (pNode == NULL)
	{
		// Error in document tree
		return NULL;
	}
	
	if (!pNode->IsKindOf(CC_RUNTIME_CLASS(NodeDocument)))
	{
		// Error in document tree
		return NULL;
	}
	
	// Find the attribute in question
	for (pNode = pNode->FindFirstChild();
		 pNode != NULL;
		 pNode = pNode->FindNext())
	{
		if (pNode->IsKindOf(RequiredAttr))
			return (NodeAttribute *) pNode;
	}
	
	// Required attribute was not found - just return NULL
	return NULL;
}

/********************************************************************************************
>	NodeSetSentinel* Document::GetSetSentinel() const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/11/93
	Returns:	A (cached) pointer to the document's NodeSetSentinel node.
	SeeAlso:	NameGallery; NodeSetSentinel; Document::InitTree
********************************************************************************************/

NodeSetSentinel* Document::GetSetSentinel() const
{
	ERROR3IF(m_pSetSentinel == 0, "Document::GetSetSentinel: no NodeSetSentinel");
	return m_pSetSentinel;
}



/********************************************************************************************

>	static void Document::IncCurrentNodeCount()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/11/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Adds one to the NodesInTree counter of the Current Document object, if it
				exists.  Called by the constructors of class Node.
	Errors:		-
	SeeAlso:	Document::DecCurrentNodeCount; Document::NodesInDocument

********************************************************************************************/

void Document::IncCurrentNodeCount()
{
	if (Current) Current->NodesInTree++;
}



/********************************************************************************************

>	static void Document::DecCurrentNodeCount()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/11/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Decrements the node counter in the current Document, if there is one.
				Called by the destructor of class Node.
	Errors:		-
	SeeAlso:	Document::IncCurrentNodeCount; Document::NodesInDocument

********************************************************************************************/

void Document::DecCurrentNodeCount()
{
	if (Current) Current->NodesInTree--;
}



/********************************************************************************************

>	void Document::DeleteContents()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/3/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	None. Nothing. Does not do anything. And don't even think about making it
				do anything!
	Errors:		-
	SeeAlso:	CCamDoc::DeleteContents

********************************************************************************************/

void Document::DeleteContents()
{
	// This function does nothing because MFC calls it in a completely brain-damaged way.
	// It calls it when the document is about to be destroyed, and ALSO calls it just
	// after a new document has just been created. Fabby eh? Those boys from Redmond strike 
	// again!  (Tim)

	// DON'T make it do anything!
}




/********************************************************************************************

>	void Document::ForceRedraw(FRDocViewFunc pFunc)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/4/94
	Inputs:		pFunc = ptr to func to equire whether a particular DocView should be redrawn
	Purpose:	Calls DocView::ForceRedraw() for each DocView attached to this
				Document.  i.e. This forces a *complete* redraw for the whole document in
				all views attached to it.
	SeeAlso:	DocView::ForceRedraw

********************************************************************************************/

void Document::ForceRedraw(FRDocViewFunc pFunc)
{
	ListItem* pItem = DocViews.GetHead();
	while (pItem != NULL)
	{
		BOOL ok = TRUE;
		DocView* pDocView = (DocView*)pItem;

		if (pFunc != NULL)
			ok = pFunc(pDocView);

		if (ok)
		{
			// Pass TRUE to DocView::ForceRedraw() so that we force an immediate update of
			// the paper, because this function is used when something pretty fundamental has
			// changed in the document, quite often related to spreads/pages etc.
			((DocView*) pItem)->ForceRedraw(TRUE);
		}

		pItem = DocViews.GetNext(pItem);
	}
}



/********************************************************************************************
>	void Document::ForceRedraw(WorkRect& forceRect)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/4/94
	Inputs:		A WorkRect bounding the area to be redrawn
	Outputs:	-
	Returns:	-
	Purpose:	Calls DocView::ForceRedraw(WorkRect&) for each DocView attached to
				this Document.
	Errors:		-
	SeeAlso:	DocView::ForceRedraw
********************************************************************************************/

void Document::ForceRedraw(WorkRect& forceRect)
{
	ListItem* pItem = DocViews.GetHead();
	while (pItem != NULL)
	{
		((DocView*) pItem)->ForceRedraw(forceRect);
		pItem = DocViews.GetNext(pItem);
	}
}



/********************************************************************************************
>	void Document::ForceRedraw(Spread* pSpread, DocRect spreadRect, BOOL Accumulate = FALSE, Node* pInvalidNode = NULL, BOOL bAutoRelease = TRUE)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/4/94
	Inputs:		A pointer to a spread and the area within that spread to redraw.
	Outputs:	-
	Returns:	-
	Purpose:	Calls DocView::ForceRedraw(Spread*, DocRect) for each DocView
				attached to this Document.
	Errors:		-
	SeeAlso:	DocView::ForceRedraw
********************************************************************************************/

void Document::ForceRedraw(Spread* pSpread, DocRect spreadRect, BOOL Accumulate, Node* pInvalidNode, BOOL bAutoRelease)
{
	// If the caller has told us about a node that has become invalid (probably causing this redraw)
	// Then make sure it doesn't hold on to any cached rendering before we force the redraw
	if (pInvalidNode && bAutoRelease)
	{
		if (pInvalidNode->IsAnObject()) ((NodeRenderableInk*)pInvalidNode)->ReleaseCached(TRUE, TRUE);
	}

	ListItem* pItem = DocViews.GetHead();
	while (pItem != NULL)
	{
		((DocView*) pItem)->ForceRedraw(pSpread, spreadRect, Accumulate, pInvalidNode);
		pItem = DocViews.GetNext(pItem);
	}
}

/********************************************************************************************

>	void Document::FlushRedraw()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/12/94
	Purpose:	Calls DocView::FlushRedraw() for each DocView
				attached to this Document.
	SeeAlso:	DocView::FlushRedraw

********************************************************************************************/

void Document::FlushRedraw()
{
	ListItem* pItem = DocViews.GetHead();
	while (pItem != NULL)
	{
		((DocView*) pItem)->FlushRedraw();
		pItem = DocViews.GetNext(pItem);
	}
}



/********************************************************************************************
>	void Document::HandleNodeDeletion(Node* pNode)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/4/94
	Inputs:		A WorkRect bounding the area to be redrawn
	Outputs:	-
	Returns:	-
	Purpose:	Calls DocView::ForceRedraw(WorkRect&) for each DocView attached to
				this Document.
	Errors:		-
	SeeAlso:	DocView::ForceRedraw
********************************************************************************************/

void Document::HandleNodeDeletion(Node* pNode)
{
	ListItem* pItem = DocViews.GetHead();
	while (pItem != NULL)
	{
		((DocView*) pItem)->HandleNodeDeletion(pNode);

		pItem = DocViews.GetNext(pItem);
	}
}



/********************************************************************************************

>	InsertionNode* Document::GetInsertionPosition(); 

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/7/94
	Returns:	The Insertion position. This will be the InsertionNode (Insert as a Previous 
				child of this)
	Purpose:	This function should be called to find the position in the document where a
				new object should be inserted. The object should always be inserted as a previous
				child of the InsertionNode which is returned.
	SeeAlso:	UndoableOperation::DoInsertNewNode

********************************************************************************************/

InsertionNode* Document::GetInsertionPosition()
{
	// Is the Insertion	node in the corrrect spread ?
	Spread* SelSpread = Document::GetSelectedSpread();
	ENSURE(SelSpread != NULL, "The selected spread is NULL");
	
	// I think it's ok to search for the active layer here cos there is not likely
	// to be zillions of layers.
	Layer* SelActiveLyr = SelSpread->FindActiveLayer(); 
	ENSURE(SelActiveLyr != NULL, "Could not find active layer on selected spread"); 
	
	Node* InsertLyr = NULL;	 // The layer containing the InsertionNode. If one
							 // exists. See the InsertionNode's destructor.
	if (InsertPos)
	{
		ERROR3IF(InsertPos->FindNext(), "InsertionNode is not the last node on a layer"); 
		InsertLyr = InsertPos->FindParent();
		// In a retail if the InsertPos' parent is NULL then we will survive this
		ERROR3IF(!InsertLyr, "The Insertion node's parent has been deleted"); 
		// If the parent of the Insertion node is not a Layer then there is probably
		// cause for concern. Not very likely this.
		ENSURE(InsertLyr->GetRuntimeClass() == CC_RUNTIME_CLASS(Layer), "Parent of insertion node is not a layer");
	}
	else
	{
		// Weve lost our Insertion Node. Create a new one
		InsertPos = new InsertionNode(this);
		if (!InsertPos)
		{
			return NULL; // Not v. likely
		}
	}

	if (SelActiveLyr != ((Layer*)InsertLyr))
	{
		// The InsertionNode should be moved
		// this is the slow bit, but it will only happen when the selected spread 
		// changes.
		InsertPos->MoveNode(SelActiveLyr, LASTCHILD); 	
	}
	// otherwise the InsertPos is in the correct position
	return InsertPos;
} 



/********************************************************************************************

>	void Document::ResetInsertionPosition()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/08/94
	Purpose:	Forces the insertion node to move to the end of the layer that it is
				currently on.  This is used by the filters so they can insert an 
				arbitrary number of nodes into the tree without worryng about the insertion 
				position being updated every time and then it can just update it at the end 
				using this function.
	SeeAlso:	Document::GetInsertionPosition

********************************************************************************************/

void Document::ResetInsertionPosition()
{
	// Is the Insertion	node in the correct spread ?
	Spread* SelSpread = Document::GetSelectedSpread();
	if (SelSpread==NULL)
	{
		SelSpread = FindFirstSpread();
		if (SelSpread==NULL)
		{
			ERROR3("Fialed to ResetInsertPosition(), unable to find a spread in this document");
			return;
		}
	}
	
	// I think it's ok to search for the active layer here cos there is not likely
	// to be zillions of layers.
	Layer* pSelActiveLyr = SelSpread->FindActiveLayer();
	if ( pSelActiveLyr )
	{
		ERROR3IF(pSelActiveLyr == NULL, "Could not find active layer on selected spread"); 

		//Node* InsertLyr = InsertPos->FindParent();

		//ENSURE(InsertLyr->GetRuntimeClass() == CC_RUNTIME_CLASS(Layer), 
		//	"Parent of insertion node is not a layer")

		if (InsertPos == NULL)
			GetInsertionPosition();

		// The InsertionNode should be moved
		if (InsertPos != NULL && pSelActiveLyr != NULL)
			InsertPos->MoveNode(pSelActiveLyr, LASTCHILD);

	/*
		Node* pInsertPosNode = GetInsertionPosition();

		ERROR3IF(pInsertPosNode == NULL,"Can't get hold of the insert node");

		// The InsertionNode should be moved
		if (pInsertPosNode != NULL)
			pInsertPosNode->MoveNode(SelActiveLyr, LASTCHILD);
	*/
	}
}



/********************************************************************************************
>	static BOOL Document::ReadPrefs()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/1/95
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if successful.
	Purpose:	Reads preferences for the Document class from the program's .INI file.
	Errors:		-
	SeeAlso:	InitKernel
********************************************************************************************/

BOOL Document::ReadPrefs()
{
	if (Camelot.DeclareSection(TEXT("Preferences"), 1))
	{
		Camelot.DeclarePref(TEXT("Preferences"), TEXT("RemoveExistingDocs"),
							   &CCamDoc::s_RemoveExistingOnNewDoc, FALSE, TRUE);
	}
	return TRUE;
}


/********************************************************************************************

>	BOOL Document::EPSStartImport(EPSFilter *pFilter)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/08/94
	Inputs:		pFilter - the EPS filter that is being used to import a file.
	Returns:	TRUE if the document was able to prepare for importing;
				FALSE if not (e.g. out of memory)
	Purpose:	Inform the document that an EPS import is about to start.
	SeeAlso:	DocComponent

********************************************************************************************/

BOOL Document::EPSStartImport(EPSFilter *pFilter)
{
	return TRUE;
}

/********************************************************************************************

>	void Document::EPSEndImport(EPSFilter *pFilter, BOOL Success)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/08/94
	Inputs:		pFilter - the EPS filter that is being used to import a file.
				Success - TRUE => The import was successful;
						 FALSE => The import failed - abandon any changes.
	Purpose:	Inform the document that an EPS import has just finished.
	SeeAlso:	DocComponent

********************************************************************************************/

void Document::EPSEndImport(EPSFilter *pFilter, BOOL Success)
{
}

/********************************************************************************************

>	BOOL Document::EPSStartExport(EPSFilter *pFilter)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/08/94
	Returns:	TRUE if the document was able to prepare for exporting;
				FALSE if not (e.g. out of memory)
	Inputs:		pFilter - the EPS filter that is being used to export a file.
	Purpose:	Inform the document that an EPS export is about to start.
	SeeAlso:	DocComponent

********************************************************************************************/

BOOL Document::EPSStartExport(EPSFilter *pFilter)
{
	return TRUE;
}

/********************************************************************************************

>	void Document::EPSEndExport(EPSFilter *pFilter)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/08/94
	Inputs:		pFilter - the EPS filter that is being used to import a file.
	Purpose:	Inform the document that an EPS export has just finished.
	SeeAlso:	DocComponent

********************************************************************************************/

void Document::EPSEndExport(EPSFilter *pFilter)
{
}


/********************************************************************************************

>	BOOL Document::WriteEPSProlog ( EPSFilter *pFilter )

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	01/08/94
	Inputs:		pFilter - the EPS filter that is being used to export a file.
	Returns:	TRUE if the Prolog for this document was written out successfully;
				FALSE if not (e.g. out of disk space etc)
	Purpose:	Write out the EPS prolog for this document, using the EPS
				filter object supplied. 
				(The base class does nothing except return TRUE)

	SeeAlso:	Document::WriteEPSSetup; 
				Document::WriteEPSComments;
				Document::ProcessEPSComment; 
				DocComponent; 
				EPSFilter

********************************************************************************************/

BOOL Document::WriteEPSProlog ( EPSFilter *pFilter )
{
	// This function is only called from the AIEPS render region.
	EPSExportDC *pDC = pFilter->GetExportDC ();
	AIExportProlog ( pDC );

	return TRUE;
}

/********************************************************************************************

>	BOOL Document::WriteEPSFonts(EPSFilter *pFilter)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/4/95
	Inputs:		pFilter - the EPS filter that is being used to export a file.
	Returns:	TRUE if the fonts for this document was written out successfully;
				FALSE if not (e.g. out of disk space etc)
	Purpose:	Write out the EPS fonts for this document, using the EPS
				filter object supplied.
				(The base class does nothing except return TRUE)

	SeeAlso:	Document::WriteEPSProlog; 
				Document::WriteEPSComments;
				Document::ProcessEPSComment; 
				DocComponent; 
				EPSFilter

********************************************************************************************/

BOOL Document::WriteEPSFonts(EPSFilter *pFilter)
{
	PORTNOTETRACE("filters","Document::WriteEPSFonts - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	if ( (pFilter->IS_KIND_OF(CamelotNativeEPSFilter)) ||
	     (pFilter->IS_KIND_OF(AIEPSFilter))
	   )
	{
		EPSExportDC *pDC = pFilter->GetExportDC();

		String_64 FontName;
		String_64 EFont;
		INT32 Style;
		FontList CurFontList;
		BOOL first=TRUE;

		if (!CurFontList.Build(this))
			return FALSE;

		FontListItem* pItem = CurFontList.GetFirstItem();
		while (pItem)
		{
			FontName = pItem->GetFontName();
			Style = pItem->GetFontStyle();
			// Graeme (31-3-00) - Map the encoded name onto the PS font name.
			FONTMANAGER->EncodeAndMapFontName(FontName, EFont, Style);

			if (first)
				pDC->OutputToken("%%DocumentFonts:");
			else
				pDC->OutputToken("%%+");	

			pDC->OutputToken((TCHAR*)EFont);
			pDC->OutputNewLine();
		
			first=FALSE;
			pItem = CurFontList.GetNextItem(pItem);	
		}
	}
#endif
	return TRUE;
}

/********************************************************************************************

>	BOOL Document::WriteEPSResources(EPSFilter *pFilter)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/4/95
	Inputs:		pFilter - the EPS filter that is being used to export a file.
	Returns:	TRUE if the resources for this document was written out successfully;
				FALSE if not (e.g. out of disk space etc)
	Purpose:	Write out the EPS resources for this document, using the EPS
				filter object supplied.
				(The base class does nothing except return TRUE)

	SeeAlso:	Document::WriteEPSProlog; 
				Document::WriteEPSComments;
				Document::ProcessEPSComment; 
				DocComponent; 
				EPSFilter

********************************************************************************************/

BOOL Document::WriteEPSResources(EPSFilter *pFilter)
{
	PORTNOTETRACE("filters","Document::WriteEPSResources - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	// Graeme (24/3/00) - Removed the IsKindOf ( AIEPSFilter ) calls because this is now
	// only called from the AI render region.
	EPSExportDC *pDC = pFilter->GetExportDC();

	pDC->OutputToken("%%DocumentNeededResources:");

	AIExportResources(pDC,TRUE);
#endif
	return TRUE;
}


/********************************************************************************************

>	BOOL Document::WriteEPSSetup(EPSFilter *pFilter)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	01/08/94
	Inputs:		pFilter - the EPS filter that is being used to export a file.
	Returns:	TRUE if the Prolog for this document was written out successfully;
				FALSE if not (e.g. out of disk space etc)
	Purpose:	Write out the EPS setup for this document, using the EPS
				filter object supplied.
				(The base class does nothing except return TRUE)

	SeeAlso:	Document::WriteEPSProlog; 
				Document::WriteEPSComments;
				Document::ProcessEPSComment; 
				DocComponent; 
				EPSFilter

********************************************************************************************/

BOOL Document::WriteEPSSetup(EPSFilter *pFilter)
{
	PORTNOTETRACE("filters","Document::WriteEPSFonts - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	// only do something if the filter is an Illustrator one
	if (pFilter->IS_KIND_OF(AIEPSFilter))
	{
		EPSExportDC *pDC = pFilter->GetExportDC();

		ExportTextSetup(pFilter);
		AIExportExtras(pDC);
		AIExportCharEncoding(pDC);
		AIExportFontEncoding(pDC);
	}
#endif
	return TRUE;
}



/********************************************************************************************

>	BOOL Document::WriteEPSComments(EPSFilter *pFilter)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	01/08/94
	Inputs:		pFilter - the EPS filter that is being used to export a file.
	Outputs:	-
	Returns:	TRUE if the EPS comments for this document were written out successfully;
				FALSE if not (e.g. out of disk space etc)
	Purpose:	Write out any comments that need to be in the initial batch of EPS
				comments (i.e. before the %%EndComments line), using the specified EPS
				Filter object.
				(The base class does nothing except return TRUE)

	SeeAlso:	Document::WriteEPSSetup; 
				Document::WriteEPSProlog;
				Document::ProcessEPSComment; 
				DocComponent; 
				EPSFIlter

********************************************************************************************/

BOOL Document::WriteEPSComments(EPSFilter *pFilter)
{
	PORTNOTETRACE("filters","Document::WriteEPSComments - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	// only do something if the filter is a Native one
	if (pFilter->IS_KIND_OF(CamelotNativeEPSFilter))
	{
		// NOTE: If you add something new then ensure you export it at the end of the list.
		// This is because older versions of the program (1.0b and before) will stop
		// processing at unknown comments and can cause immense problems.			

		// Output the colour table in ArtWorks format.
		EPSExportDC *pDC = pFilter->GetExportDC();

		// The Page List comment:
		pDC->OutputToken("%%DocumentInfo");
		pDC->OutputNewLine();

		// Export the Units. These should really be saved out as early as possible
		// as other things (such as the grid settings) can make use of them.
		ExportUnitInfo(pDC);

		// Export the page layout
		ExportPageInfo(pDC);

		// The Document Comment comment:
		ExportDocumentComment(pDC);

		// Now save out the Views ScaleFactor and Scroll Offsets
		ExportViewInfo(pDC);

		// Now save out the State vars of the view
		ExportStateInfo(pDC);

		// Save out the View Quality
		ExportQualityInfo(pDC);

		// Grid settings
		ExportGridInfo(pDC);

		// View flags (always on top, maximised etc)
		ExportFlagInfo(pDC);

		// Export the date info
		ExportDateInfo(pDC);

		// Export the undo info (document history)
		ExportUndoInfo(pDC);

		// These were all exported in version 1.00b of Studio 
		// These are new ones after the this release version.
		
		// Export the default units. Must be saved after the user units as we
		// might have defined a user unit as the default.
		ExportDefaultUnitsInfo(pDC);
		
		//Export the ruler and scroller visible states
		// removed by Chris 27/10/95
		// Ruler/Scroller state now set only from a global preference
		// see ImportRulerState();
		//ExportRulerState(pDC);

		// export the origin info - must be output after grid and page info as they both
		// reset the origin so must both be imported before the origin is finally set
		ExportOriginInfo(pDC);
	}
#endif
	return TRUE;
}

/********************************************************************************************

>	BOOL Document::WriteEPSTrailer(EPSFilter *pFilter)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/4/94
	Inputs:		pFilter - the EPS filter that is being used to export a file.
	Returns:	TRUE if the Trailer for this document was written out successfully;
				FALSE if not (e.g. out of disk space etc)
	Purpose:	Write out the EPS trailer for this document, using the EPS
				filter object supplied.
				(The base class does nothing except return TRUE)

	SeeAlso:	Document::WriteEPSProlog; 
				Document::WriteEPSComments;
				Document::ProcessEPSComment; 
				DocComponent; 
				EPSFilter

********************************************************************************************/

BOOL Document::WriteEPSTrailer(EPSFilter *pFilter)
{
	PORTNOTETRACE("filters","Document::WriteEPSTrailer - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	// only do something if the filter is an Illustrator one
	if (pFilter->IS_KIND_OF(AIEPSFilter))
	{
		EPSExportDC *pDC = pFilter->GetExportDC();
		AIExportTrailer(pDC);
	}	

	ExportTextTrailer(pFilter);
#endif
	return TRUE;
}



/********************************************************************************************

>	BOOL ReadNextNumber(INT32* Number, char* Comment, INT32* Offset)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/2/95
	Inputs:		Comment - The string we are reading from
				Offset - the offset into the string that we start from
	Outputs:	Number - holds the number in the string if the function returns TRUE
				Offset - Holds the offset to just after the number in the string if the
				function returns TRUE
	Returns:	TRUE if it worked, FALSE if it could not find a number
	Purpose:	Called by Document::ProcessEPSComment to help tidy up the code a little.
				This actually just reads a number out of a string.

********************************************************************************************/

BOOL ReadNextNumber(INT32* Number, char* Comment, INT32* Offset)
{
	INT32& i = *Offset;
	BOOL IsNegative = FALSE;

	// Skip till we get to the first number
	while (!isdigit(Comment[i]) && (Comment[i] != 0) && (Comment[i]!='-'))
		i++;

	if (Comment[i]=='-')
	{
		// Negative number
		IsNegative = TRUE;
		i++;
	}

	// Read in the number
	if (isdigit(Comment[i]))
	{
		// get the Number
		*Number = atol( Comment + i );

		// Set the sign
		if (IsNegative)
			*Number = -(*Number);

		// Skip to the next number
		while (isdigit(Comment[i]))
			i++;

		// finished
		return TRUE;
	}

	// Failed
	return FALSE;
}

/********************************************************************************************

>	BOOL ReadNextNumber(UINT32* Number, char* Comment, INT32* Offset)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/07/95
	Inputs:		Comment - The string we are reading from
				Offset - the offset into the string that we start from
	Outputs:	Number - holds the number in the string if the function returns TRUE
				Offset - Holds the offset to just after the number in the string if the
				function returns TRUE
	Returns:	TRUE if it worked, FALSE if it could not find a number
	Purpose:	Called by Document::ProcessEPSComment to help tidy up the code a little.
				This actually just reads a number out of a string.

********************************************************************************************/

BOOL ReadNextNumber(UINT32* Number, char* Comment, INT32* Offset)
{
	INT32& i = *Offset;

	// Skip till we get to the first number
	while (!isdigit(Comment[i]) && (Comment[i] != 0))
		i++;

	// Read in the number
	if (isdigit(Comment[i]))
	{
		// get the Number
		*Number = strtoul( Comment + i, NULL, 0 );

		// Skip to the next number
		while (isdigit(Comment[i]))
			i++;

		// finished
		return TRUE;
	}

	// Failed
	return FALSE;
}



/********************************************************************************************

>	BOOL ReadNextNumber(double* Number, char* Comment, INT32* Offset)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/2/95
	Inputs:		Comment - The string we are reading from
				Offset - the offset into the string that we start from
	Outputs:	Number - holds the number in the string if the function returns TRUE
				Offset - Holds the offset to just after the number in the string if the
				function returns TRUE
	Returns:	TRUE if it worked, FALSE if it could not find a number
	Purpose:	Called by Document::ProcessEPSComment to help tidy up the code a little.
				This actually just reads a number out of a string.

********************************************************************************************/

BOOL ReadNextNumber(double* Number, char* Comment, INT32* Offset)
{
	INT32& i = *Offset;

	// Skip till we get to the first number
	while (camIsspace(Comment[i]) && (Comment[i]!=0))
		i++;

	// Read in the number
	if (!camIsspace(Comment[i]))
	{
		// get the Number
		*Number = atof(Comment+i);

		// Skip to the next number
		while (!camIsspace(Comment[i]))
			i++;

		// finished
		return TRUE;
	}

	// Failed
	return FALSE;
}


/********************************************************************************************

>	BOOL GetNextString(StringBase* pString, INT32 MaxLen, char* Comment, INT32* Offset))

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/3/95
	Inputs:		MaxLen - the max length of the output string
				Comment - The string we are reading from
				Offset - the offset into the string that we start from
	Outputs:	pString - Holds the string
				Offset - Holds the offset to just after the number in the string if the
				function returns TRUE
	Returns:	TRUE if it worked, FALSE if it could not find a string
	Purpose:	Called by Document::ProcessEPSComment to help tidy up the code a little.
				This actually just reads a string out of a string.

********************************************************************************************/

BOOL ReadNextString(StringBase* pString, INT32 MaxLen, char* Comment, INT32* Offset)
{
	INT32& i = *Offset;
	TCHAR TempStr[256];

	// Skip till we get to the start of the string (Which is the '(' char)
	while ((Comment[i]!='(') && (Comment[i]!=0))
		i++;

	// ok, read the string in
	if (Comment[i]=='(')
	{
		// Eat the open bracket
		i++;

		// Start to copy the string
		TCHAR Ch;
		INT32 Dst = 0;
		do
		{
			// Get this char and store it
			Ch = Comment[i];
			TempStr[Dst] = Ch;
			Dst++;
			i++;
			
			if (Ch=='\\')
			{
				// This is the escape char, so store the char that follows it,
				// but do not change the value of Ch or it may think that the comment is ending
				TempStr[Dst-1] = Comment[i];
				i++;
			} 
		} while (((Ch!=0) && (Ch!=')')) || (Dst==MaxLen-1));

		// Terminate the string
		TempStr[Dst-1] = 0;

		// Copy the string into the StringBase
		pString->Empty();
		*pString += (TCHAR*)TempStr;
		return TRUE;
	}

	// if we get here, we did not extract the string
	return FALSE;
}



/********************************************************************************************

>	ProcessEPSResult Document::ProcessEPSComment(EPSFilter *pFilter, 
												 const char *pComment)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	01/08/94
	Inputs:		pFilter - the EPS filter that is being used to import a file.
				pComment - read only buffer containing  - This EPS comment is not recognised by the document.
				EPSCommentSy the whole EPS comment to parse.
	Returns:	EPSCommentUnknown ntaxError - This EPS comment was recognised by this document
									    but it contained an error.
				EPSCommentSystemError - This EPS comment was recognised by this document
									    but a system error occured that was not caused
										directly by the comment, e.g. out of memory.
				EPSCommentOK		  - This EPS comment was recognised as a legal comment by 
									    this document, and was processed successfully.
	Purpose:	Process an EPS comment found in the file, if it 'belongs' to the
				document.  If it does not, then the document should return EPSCommentUnknown, 
				and the comment will be passed on to the document components.
				If the function returns EPSCommentOK, and the next line in the file starts 
				with "%%+", i.e. an EPS/PostScript continuation comment, then this will be
				passed to this document immediately, without trying any of the document 
				components first.  This allows items such a colour lists
				to be imported easily, as they are usually specified on multiple lines in
				the file, using "%%+" comments.
				(The base class does nothing - it returns EPSCommentUnknown)

	SeeAlso:	Document::EndOfEPSComment; 
				Document::WriteEPSProlog; 
				Document::WriteEPSSetup; 
				Document::WriteEPSComments; 
				DocComponent; 
				ProcessEPSResult;
				EPSFIlter

********************************************************************************************/

ProcessEPSResult Document::ProcessEPSComment(EPSFilter *pFilter, 
										     const char *pComment)
{
	PORTNOTETRACE("filters","Document::ProcessEPSComment - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	// OK, to begin with, set the current doc and view to this doc and it's first view.
	SetCurrent();
	if (GetFirstDocView() != NULL){
	 	GetFirstDocView()->SetCurrent();
	}
	if ((pFilter->IS_KIND_OF(CamelotNativeEPSFilter)) && !IsImporting())
	{
		// Take a copy of the comment
		char Comment[256];
		camStrcpy(Comment, pComment);

		// Look for Document info
		if (camStrncmp(pComment, "%%NativePageList", 16) == 0)
		{
			// Found the page description table
			return EPSCommentOK;
		}
		else if (camStrncmp(pComment, "%%DocumentInfo", 14) == 0)
		{
			// Found the page description table
			// Set the Documents Comment to nothing
			String_256 Empty("");
			SetComment(&Empty);

			return EPSCommentOK;
		}
		else if (camStrncmp(pComment, "%%+", 3) == 0)
		{
			// Assume we do not know about this comment until we can prove that we do.
			//
			// NOTE: If you add something new then ensure you export it at the end of the list.
			// This is because older versions of the program (1.0b and before) will stop
			// processing at unknown comments and can cause immense problems.			

			ProcessEPSResult Result = EPSCommentUnknown;
			char InfoType = pComment[3];
			switch (InfoType)
			{
				// The Document Info Comment (as seen in the Document Info dialog in the File menu)
				case 'c' :
				{
					Result = ImportDocumentComment(Comment);
					break;
				}

				// Read in the Creation and Last Saved dates
				case 'd' :
				{
					Result = ImportDateInfo(Comment);
					break;
				}

				// Read in the View Flags
				case 'f':
				{
					break;
				}

				// Read in the Grid Settings
				case 'g':
				{
					Result = ImportGridInfo(Comment);
					break;
				}

				// Read in the Grid Settings
				case 'o':
				{
					Result = ImportOriginInfo(Comment);
					break;
				}

				// Document history which is the specified undo size
				case 'h':
				{
					Result = ImportUndoInfo(Comment);
					break;
				}

				// Page Description
				case 'p' :
				{
					// load in the new page layout
					Result = ImportPageInfo(Comment);

					// adjust the page origin to match the new page.
					pFilter->ResetImportOrigin();
					break;
				}

				// Read in the View Quality Settings
				case 'q':
				{
					// Load in the new view quality
					Result = ImportQualityInfo(Comment);

#if !defined(EXCLUDE_FROM_RALPH) && !defined(EXCLUDE_FROM_XARALX)
					// update all the view quality sliders
					QualitySliderDescriptor::Update();
#endif
					break;
				}

				// The DocView State Info (Grid on/off, Snapping on/off etc)
				case 's' :
				{
					Result = ImportStateInfo(Comment);
					break;
				}

				// The Unit settings
				case 'u' :
				{
					Result = ImportUnitInfo(Comment);
					break;
				}

				// The default display unit settings
				case 'U' :
				{
					Result = ImportDefaultUnitsInfo(Comment);
					break;
				}
				
				// The Ruler and Scroller visible state
				case 'r' :
				{
					//Result = ImportRulerState(Comment);
					// removed by Chris 27/10/95
					// just ignore this token now
					// Ruler/Scroller state now set only from a global preference
					Result = EPSCommentOK;
					break;
				}
			
				// The View Setting (Scale Factor, Scroll offsets etc)
				case 'v' :
				{
					Result = ImportViewInfo(Comment);
					break;
				}

				default :
				{
					Result = EPSCommentUnknown;
					break;
				}
			}

			// Result will hold the result of any processing of this continuation comment
			return Result;
		}
	}
	else if (pFilter->IS_KIND_OF(CamelotNativeEPSFilter))
	{
		// Importing a native file - we need to get the page origin so that the filter
		// can position the objects correctly.

		// Take a copy of the comment
		char Comment[256];
		camStrcpy(Comment, pComment);

		// Look for Document info
		if (camStrncmp(pComment, "%%NativePageList", 16) == 0)
		{
			// Found the page description table
			return EPSCommentOK;
		}
		else if (camStrncmp(pComment, "%%DocumentInfo", 14) == 0)
		{
			// Found the page description table
			// Set the Documents Comment to nothing
			String_256 Empty("");
			SetComment(&Empty);

			return EPSCommentOK;
		}
		else if (camStrncmp(pComment, "%%+", 3) == 0)
		{
			// Default to recognising this comment - all we care about is extracting the page origin.
			ProcessEPSResult Result = EPSCommentOK;
			char InfoType = pComment[3];
			if (InfoType == 'p')
			{
				// Set the default version
				INT32 Version = -1;

				// Where in the comment to start looking from (skip over %%+p)
				INT32 i = 4;

				// Read in the version number
				if (!ReadNextNumber(&Version, Comment, &i))
					return EPSCommentSyntaxError;

				// Descide what to do based on the version number
				switch (Version)
				{
					case 0 :
					{
						// Version 0 expects to find the following infomation
						// Width, Height, Margin, Bleed, Dps, ShowDropShadow
						// Get something to put all the numbers into
						MILLIPOINT Dummy, Margin;

						// Read in the Width of the page
						if (!ReadNextNumber(&Dummy, Comment, &i))
							return EPSCommentSyntaxError;

						// Read in the Height of the page
						if (!ReadNextNumber(&Dummy, Comment, &i))
							return EPSCommentSyntaxError;

						// Read in the Margin of the page
						if (!ReadNextNumber(&Margin, Comment, &i))
							return EPSCommentSyntaxError;

						// Ok, we've got the margin, so tell the EPS filter about it.
						DocCoord NewOrigin(Margin, Margin);
						pFilter->ResetImportOrigin(NewOrigin);
					}                 

					// Unknown version
					default:
					{
						// Here we have got a version number we do not know how to process
						// so just ignore this comment happy. we will say that the comment
						// was OK as an error is technically wrong in this case
					}
				}
			}

			// Result will hold the result of any processing of this continuation comment
			return Result;
		}
	}
#endif
	return EPSCommentUnknown;
}


/********************************************************************************************

>	void Document::EndOfEPSComment(EPSFilter *pFilter)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/08/94
	Inputs:		pFilter - the EPS filter that is being used to import a file.
	Purpose:	Informs the document that the comment it has been decoding has
				now finished, and it should take whatever action required to act on
				the comment.
				This call is made even for single-line comments, because by the nature of
				DSC comments, it is not known until the next comment starts or a
				non-comment line is encountered that the comment is over and is not
				multi-line.

	SeeAlso:	Document::ProcessEPSComment; 
				Document::WriteEPSProlog; 
				Document::WriteEPSSetup; 
				Document::WriteEPSComments;
				DocComponent; EPSFilter

********************************************************************************************/

void Document::EndOfEPSComment(EPSFilter *pFilter)
{
}

/********************************************************************************************

>	INT32 Document::GetSizeOfExport(Filter *pFilter)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/01/95
	Inputs:		pFilter - the export filter that will be used.
	Returns:	An approximation of the size of data that will be exported by this
				document, in terms of nodes. (see NodeRenderable::GetSizeOfExport).
	Purpose:	Find out how many nodes will be exported when this document is exported.
				The node can use the filter to find out how it will be exported.
				NB. This is virtual - the default implementation just returns 1 - only
					override if this is not accurate.
	SeeAlso:	Node::NeedsToExport; Node::ExportRender; NodeRenderable::GetSizeOfExport

********************************************************************************************/
INT32 Document::GetSizeOfExport( Filter * )
{
	// Default value is one node...
	return 1;
}

/********************************************************************************************
>	DocView* Document::GetTopmostView() const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/10/96
	Returns:	A pointer to the DocView of this Document that is top-most in the Z-order
				window list.  It's the view onto this document that was last worked in
				by the user.  Returns NULL if this document has no visible views.
	Purpose:	Finds out which of this Document's DocViews is 'selected'.
	SeeAlso:	CCamDoc::SetPageSize
********************************************************************************************/

DocView* Document::GetTopmostView() const
{
	// Search through all the views onto this document.
	DocView* pvTop = 0;
	INT32 zTop = -1;
	for (DocView* pv = GetFirstDocView(); pv; pv = GetNextDocView(pv))
	{
		// Is this view above our current highest?
		if (pv->pVState && pv->pVState->zPos > zTop)
		{
			// It is, so remember it as the new highest.
			zTop = pv->pVState->zPos;
			pvTop = pv;
		}
	}

	// Return the highest, if any.
	return pvTop;
}



/********************************************************************************************

>	MILLIPOINT Document::GetBleedOffset()	const

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/12/94
	Inputs:		-
	Outputs:	-
	Returns:	The current size in millipoints of this Document's bleed area.
	Purpose:	For getting the Document's bleed area size. 
	Errors:		-
	SeeAlso:	SetBleedOffset();

********************************************************************************************/

MILLIPOINT Document::GetBleedOffset() const
{
	return BleedOffset;
} 

/********************************************************************************************

>	BOOL Document::SetBleedOffset(MILLIPOINT Bleed)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/12/94
	Inputs:		-
	Outputs:	-
	Returns:	The current size in millipoints of this document's bleed area.
	Purpose:	For setting a new value for the Document's bleed area size. 
	Errors:		-
	SeeAlso:	Document::GetBleedOffset();

********************************************************************************************/

BOOL Document::SetBleedOffset(MILLIPOINT Bleed)
{
	BleedOffset = Bleed;	// set up new bleed value
	
	return TRUE;
} 




/********************************************************************************************

>	BOOL Document::ExportPageInfo(EPSExportDC *pDC)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/3/95
	Inputs:		pDC - The DC to render to
	Returns:	TRUE
	Purpose:	Exports a Document Info continuation comment that describes the page layout.
				It takes the following form :-
				%%+p VersionNum
				if VersionNum is zero then the following will be on the rest of the line
				Width Height Margin Bleed DPS(1 for yes, 0 for no) ShowDropShadow(1 for yes, 0 for no)

********************************************************************************************/

BOOL Document::ExportPageInfo(EPSExportDC *pDC)
{
	// Find the first spread to get the page info from
	// Don't use the selected spread as it breaks save all
	// do don't do Spread* pSpread = GetSelectedSpread();
	Node *pANode = GetFirstNode()->FindNext()->FindFirstChild();
	while ((pANode != NULL) && (!pANode->IsKindOf(CC_RUNTIME_CLASS(Chapter))))
		pANode = pANode->FindNext();
		
	ERROR2IF(!pANode->IsKindOf(CC_RUNTIME_CLASS(Chapter)),
		FALSE, "Document::Export(something)(): Could not find Chapter");

	Chapter *pChapter = (Chapter *) pANode;
	
	// pSpread is a child of pChapter
	Spread *pSpread = (Spread *) pChapter->FindFirstChild();
	ERROR2IF(!pSpread->IsKindOf(CC_RUNTIME_CLASS(Spread)),
		FALSE, "Document::Export(something)(): Could not find Spread");

	// get something to put all the page info into
	char Buffer[256];
	MILLIPOINT Width, Height;
	MILLIPOINT Margin, Bleed;
	BOOL Dps, ShowDropShadow;
	INT32 Version = 0;

	// and go and get all the page info
	BOOL Worked = pSpread->GetPageSize(&Width, &Height, &Margin, &Bleed, &Dps, &ShowDropShadow);

	// If it worked, we had better try and output some info
	if (Worked)
	{
		// Build a string with all the data in it
		sprintf(Buffer, "%d %d %d %d %d %d %d", Version, (INT32)Width, (INT32)Height, (INT32)Margin, (INT32)Bleed, Dps, ShowDropShadow);

PORTNOTE("filters","Removed EPSExportDC usage")
#ifndef EXCLUDE_FROM_XARALX
		// Output the line to the file
		pDC->OutputToken("%%+p");
		pDC->OutputToken(Buffer);
		pDC->OutputNewLine();
#endif
	}

	return TRUE;
}


/********************************************************************************************

>	BOOL Document::ExportDocumentComment(EPSExportDC *pDC)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/3/95
	Inputs:		pDC - The DC to render to
	Returns:	TRUE
	Purpose:	Will export as many Document Info continuation comments as are needed to
				save out the whole comment. Each continuation comment takes the form as follows :-
				%%+c CommentHere
				Each line will not have more that about 100 chars of comment on it (they will
				all be simple joined together when importing). If there is a new line char
				in the comment the continuation comment will end at that point, but the last
				char of the continuation comment will be ^.

********************************************************************************************/

BOOL Document::ExportDocumentComment(EPSExportDC *pDC)
{
#if !defined(EXCLUDE_FROM_RALPH)
	// Save this out in 200 Char lines, using many continuation comments if needed
	String_256 Comment = GetComment();
	INT32 StrLength = Comment.Length();
	
	// Go though the string splitting it up as needed
	TCHAR OneLine[256];
	TCHAR Buffer[256];
	camStrcpy(Buffer, (TCHAR*)Comment);

	// vars to keep track of where we are
	INT32 Src = 0;
	INT32 Dest = 0;
	
	// Loop through the string they have entered and split it up into lines of text
	while (Src<StrLength)
	{
		// should we write this out (ie have we hit a new line)
		if ((Buffer[Src]=='\n') || (Buffer[Src]=='\r') || (Dest>100))
		{
			// deal with it being the CR.
			if ((Buffer[Src]=='\n') || (Buffer[Src]=='\r'))
			{
				OneLine[Dest] = '^';
				Dest++;
			}

			// Skim till we find something that is not a CR
			while ((Buffer[Src]=='\n') || (Buffer[Src]=='\r'))
				Src++;

			// Output the line
			if (Dest>0)
			{
				OneLine[Dest] = 0;
PORTNOTE("filters","Removed EPSExportDC usage")
#ifndef EXCLUDE_FROM_XARALX
				pDC->OutputToken("%%+c");
				pDC->OutputToken(OneLine);
				pDC->OutputNewLine();
#endif
				Dest = 0;
			}
		}

		// Lets have this char
		OneLine[Dest] = Buffer[Src];
		Dest++;
		Src++;
	}

	// Output the last line
	if (Dest>0)
	{
		OneLine[Dest] = 0;
PORTNOTE("filters","Removed EPSExportDC usage")
#ifndef EXCLUDE_FROM_XARALX
		pDC->OutputToken("%%+c");
		pDC->OutputToken(OneLine);
		pDC->OutputNewLine();
#endif
		Dest = 0;
	}
#endif
	return TRUE;
}

/********************************************************************************************

>	BOOL Document::ExportRulerState(EPSExportDC *pDC)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/3/95
	Inputs:		pDC - The DC to render to
	Returns:	TRUE
	Purpose:	Exports the View Settings continuation comment (part of the Document Info
				comment). It has the following form
				%%+r VersionNum(0) ScrollersVisible RulersVisible
				

********************************************************************************************/

BOOL Document::ExportRulerState(EPSExportDC *pDC)
{
	// A buffer

	// Find the view to get the info from
	// don't do DocView* pView = DocView::GetSelected(); as it breaks save all
	DocView* pView = GetFirstDocView();	
	if (this==Document::GetSelected()) pView = DocView::GetSelected();

	if (pView!=NULL)
	{
//		INT32 Version =0;
PORTNOTE("filters","Removed EPSExportDC usage")
#ifndef EXCLUDE_FROM_XARALX
		// Export it
		pDC->OutputToken("%%+r");
		// buffer the version number,ruler and scroller state
		char Buffer[256];
		_stprintf(Buffer, "%d %ld %ld", Version, pView->AreScrollersVisible(),pView->AreRulersVisible());
		pDC->OutputToken(Buffer);
		pDC->OutputNewLine();
#endif
	}

	return TRUE;
}



/********************************************************************************************

>	BOOL Document::ExportViewInfo(EPSExportDC *pDC)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/3/95
	Inputs:		pDC - The DC to render to
	Returns:	TRUE
	Purpose:	Exports the View Settings continuation comment (part of the Document Info
				comment). It has the following form
				%%+v VersionNum(1) XScrollOffset YScrollOffset ScaleFactor Active
				DrawingScale RealScale.
				ScaleFactor is represented 1000 times the actual scale factor. eg when the 
				document is viewed at 100% the scale factor is 1.0. This is export as 1000.
				200% would be 2000 etc. Active is a flag to say if the Document Scale Factor
				is active or not. It is followed by the Drawing and Real scales in the document

********************************************************************************************/

BOOL Document::ExportViewInfo(EPSExportDC *pDC)
{
	// A buffer
	char Buffer[256];

	// Find the view to get the info from
	// don't do DocView* pView = DocView::GetSelected(); as it breaks save all
	DocView* pView = GetFirstDocView();	
	if (this==Document::GetSelected()) pView = DocView::GetSelected();

	if (pView!=NULL)
	{
		// Find out the Scroll Offsets and the Scale Factor
		WorkCoord ScrollOffsets = pView->GetScrollOffsets();
		INT32 XOffset = ScrollOffsets.x;
		INT32 YOffset = ScrollOffsets.y;
		double ScaleFactor = pView->GetViewScale().MakeDouble();
		INT32 Version = 1;

		// Find the spread that we are saving
		// Find the first spread to get the page info from
		// Don't use the selected spread as it breaks save all
		// do don't do Spread* pSpread = GetSelectedSpread();
		Node *pANode = GetFirstNode()->FindNext()->FindFirstChild();
		while ((pANode != NULL) && (!pANode->IsKindOf(CC_RUNTIME_CLASS(Chapter))))
			pANode = pANode->FindNext();
		
		ERROR2IF(!pANode->IsKindOf(CC_RUNTIME_CLASS(Chapter)),
			FALSE, "Document::Export(something)(): Could not find Chapter");

		Chapter *pChapter = (Chapter *) pANode;
	
		// pSpread is a child of pChapter
		Spread *pSpread = (Spread *) pChapter->FindFirstChild();
		ERROR2IF(!pSpread->IsKindOf(CC_RUNTIME_CLASS(Spread)),
			FALSE, "Document::Export(something)(): Could not find Spread");

		if (pSpread != NULL)
		{
			DimScale* pDimScale = pSpread->GetPtrDimScale();
			if (pDimScale != NULL)
			{
				// Find out if it is active
				BOOL Active = pDimScale->IsActive();

				// Find out about the scale factor strings
				String_32 DrawingScale = pDimScale->GetDrawingScaleStr();
				String_32 RealScale = pDimScale->GetRealScaleStr();

				// Build the comment for the scroll offsets etc
				sprintf(Buffer, "%d %d %d %d %d", Version, XOffset, YOffset, (INT32)(ScaleFactor*1000), Active);

PORTNOTE("filters","Removed EPSExportDC usage")
#ifndef EXCLUDE_FROM_XARALX
				// Export it
				pDC->OutputToken("%%+v");
				pDC->OutputToken(Buffer);
				pDC->OutputString(DrawingScale);
				pDC->OutputString(RealScale);
				pDC->OutputNewLine();
#endif
			}
		}
	}

	return TRUE;
}


/********************************************************************************************

>	BOOL Document::ExportStateInfo(EPSExportDC *pDC)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/3/95
	Inputs:		pDC - The DC to render to
	Returns:	TRUE
	Purpose:	Part of the Document Info comment. This is document state information 
				It takes the following form :-
				%%+s VersionNum 
				Version 0 comment lines follows this with 
				ShowGrid SnapToGrid SnapToMagObjects SnapToObjects ForeBackMode (and zero
				as a dummy value, see comments in the function).
				Each of the values is a flag and will hold either 0 for FALSE or 1 for TRUE

********************************************************************************************/

BOOL Document::ExportStateInfo(EPSExportDC *pDC)
{
	// A buffer
	char Buffer[512];

	// Find the view to get the info from
	// don't do DocView* pView = DocView::GetSelected(); as it breaks save all
	DocView* pView = GetFirstDocView();	
	if (this==Document::GetSelected()) pView = DocView::GetSelected();

	if (pView!=NULL)
	{
		INT32 Version = 1;

		// Find all the State flags for version 0
		BOOL ShowGrid			= pView->GetShowGridState();
		BOOL SnapToGrid			= pView->GetSnapToGridState();
		BOOL SnapToMagObjects	= pView->GetSnapToMagObjectsState();
		BOOL SnapToObjects		= pView->GetSnapToObjectsState();
		BOOL ForeBackMode		= pView->GetForeBackMode();

		// Find all the State flags for version 1
		BOOL ShowGuides			= pView->GetShowGuidesState();
		BOOL SnapToGuides		= pView->GetSnapToGuidesState();
		BOOL Multilayer			= IsMultilayer();
		BOOL AllLayersVisible	= IsAllVisible();

		// Init the buffer with the version numbet
		sprintf(Buffer,"%d ",Version);

		// Build the comment string
		switch (Version)
		{
			case 1:
				// Write verison 1 flags (*Note* the space after the last '%d' - this is important)
				sprintf( Buffer + strlen( Buffer ), "%d %d %d %d ", ShowGuides, SnapToGuides, 
					Multilayer, AllLayersVisible);
				// Fall through for version 0 flags

			case 0:
				// Note: There was a bug here - the format string had six %ds but
				// only five parameters. There were only supposed to be five parameters
				// so the sixth %d was a typo, not noticed by the compiler (or anyone
				// who looked at the output and should have wondered what that random
				// number was at the end of the line.

				// In order to reduce the possibility of bugs, I'm instituting a dummy
				// sixth parameter, and altering the code that reads it in.

				sprintf( Buffer + strlen( Buffer ), "%d %d %d %d %d %d", ShowGrid, SnapToGrid, 
					SnapToMagObjects, SnapToObjects, ForeBackMode, 0);
				break;	// Stop here. Version 0 was the first
		
			default:
				ERROR3_PF(("Dont know how to write this version for the state flags %d",Version));
				break;
		}

PORTNOTE("filters","Removed EPSExportDC usage")
#ifndef EXCLUDE_FROM_XARALX
		// Export it
		pDC->OutputToken("%%+s");
		pDC->OutputToken(Buffer);
		pDC->OutputNewLine();
#endif
	}

	return TRUE;
}


/********************************************************************************************

>	BOOL Document::ExportQualityInfo(EPSExportDC *pDC)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/3/95
	Inputs:		pDC - The DC to render to
	Returns:	TRUE
	Purpose:	Part of the Document Info comment. It takes the following form :-
				%%+q VersionNum(0) ViewQuality.
				ViewQuality is the numerical value of the view Quality
	SeeAlso:	Quality

********************************************************************************************/

BOOL Document::ExportQualityInfo(EPSExportDC *pDC)
{
	// A buffer
	char Buffer[256];

	// Find the view to get the info from
	// don't do DocView* pView = DocView::GetSelected(); as it breaks save all
	DocView* pView = GetFirstDocView();	
	if (this==Document::GetSelected()) pView = DocView::GetSelected();

	if (pView!=NULL)
	{
		// Find out what the View Quality is...
		INT32 ViewQuality = pView->RenderQuality.GetQuality();
		INT32 Version = 0;

		// Build the Comment
		sprintf(Buffer, "%d %d", Version, ViewQuality);

PORTNOTE("filters","Removed EPSExportDC usage")
#ifndef EXCLUDE_FROM_XARALX
		// Export it
		pDC->OutputToken("%%+q");
		pDC->OutputToken(Buffer);
		pDC->OutputNewLine();
#endif
	}

	return TRUE;
}


/********************************************************************************************

>	BOOL Document::ExportGridInfo(EPSExportDC *pDC)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/3/95
	Inputs:		pDC - The DC to render to
	Returns:	-
	Purpose:	

********************************************************************************************/

BOOL Document::ExportGridInfo(EPSExportDC *pDC)
{
	// A buffer
	char Buffer[256];

	// Start off from the default spread
	// Find the first spread to get the page info from
	// Don't use the selected spread as it breaks save all
	// do don't do Spread* pSpread = GetSelectedSpread();
	Node *pANode = GetFirstNode()->FindNext()->FindFirstChild();
	while ((pANode != NULL) && (!pANode->IsKindOf(CC_RUNTIME_CLASS(Chapter))))
		pANode = pANode->FindNext();
		
	ERROR2IF(!pANode->IsKindOf(CC_RUNTIME_CLASS(Chapter)),
		FALSE, "Document::Export(something)(): Could not find Chapter");

	Chapter *pChapter = (Chapter *) pANode;
	
	// pSpread is a child of pChapter
	Spread *pSpread = (Spread *) pChapter->FindFirstChild();
	ERROR2IF(!pSpread->IsKindOf(CC_RUNTIME_CLASS(Spread)),
		FALSE, "Document::Export(something)(): Could not find Spread");

	if (pSpread!=NULL)
	{
		// Find the first child of the spread
		Node* pNode = pSpread->FindFirstChild();
		while (pNode!=NULL)
		{
			// For each of the child nodes of the spread, we will check to see if it is a Node Grid
			if (pNode->IS_KIND_OF(NodeGrid))
			{
				// Get the Node Grid
				NodeGrid* pGrid = (NodeGrid*) pNode;

				// Only do this if this is the default Grid.
				// That means that it is the full size grid that is toggled
				// from the Show grid thing.
				if (pGrid->IsDefault())
				{
					// This version of the grid saving info is zero
					INT32 Version = 0;
				
					// Go and get the numbers out of the grid
					// BODGE - old builds (hence docs) save the grid spacing in divisions and units but don't
					// account for unit scaling, so as not to change doc format new docs do the same so we must
					// read the grid 'Divisions' with scaling turned off - yuk!
					BOOL Scale=FALSE;
					double   Divisions    = pGrid->GetDivisions(Scale);
					UnitType Unit         = pGrid->GetUnits(Scale);
					UINT32     SubDivisions = pGrid->GetSubdivisions();
					GridType TypeOfGrid   = pGrid->GetGridType();

					// Build the Comment
					sprintf(Buffer, "%d %f %d %d %d", Version, Divisions, SubDivisions, (INT32)Unit, (INT32)TypeOfGrid);

PORTNOTE("filters","Removed EPSExportDC usage")
#ifndef EXCLUDE_FROM_XARALX
					// Export it
					pDC->OutputToken("%%+g");
					pDC->OutputToken(Buffer);
					pDC->OutputNewLine();
#endif

					// once we have found a grid and saved it, then we want to stop.
					// This version only saves the default grid.
					return TRUE;
				}
			}

			// Oh well, that node was not a grid, so lets see if the next one is
			pNode = pNode->FindNext();
		}
	}

	// Did not save a grid...
	return FALSE;
}


/********************************************************************************************
>	BOOL Document::ExportOriginInfo(EPSExportDC *pDC)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/10/95
	Inputs:		pDC - The DC to render to
	Returns:	FALSE if fails
	Purpose:	Export grid/user origin
	Note:		Should be exported after grid and page info as when importing,
				these both reset the origin to the origin of the page
********************************************************************************************/

BOOL Document::ExportOriginInfo(EPSExportDC *pDC)
{
	// find the first (and currently) only spread
PORTNOTE("spread", "Multi-spread warning!")
	Spread* pSpread = FindFirstSpread();
	ERROR2IF(pSpread==NULL,FALSE,"Document::ExportOriginInfo() - could not find spread");
	NodeGrid* pDefaultGrid = pSpread->FindFirstDefaultGridInSpread();
	ERROR2IF(pDefaultGrid==NULL,FALSE,"Document::ExportOriginInfo() - could not find default grid");

	// read the grid (user) origin
	DocCoord Origin(0,0);
	pDefaultGrid->GetOrigin(&Origin.x, &Origin.y);

	// Build the Comment
	char Buffer[256];	// a buffer
	INT32 Version = 0;
	sprintf(Buffer, "%d %d %d", Version, Origin.x, Origin.y);

PORTNOTE("filters","Removed EPSExportDC usage")
#ifndef EXCLUDE_FROM_XARALX
	// Export it
	pDC->OutputToken("%%+o");
	pDC->OutputToken(Buffer);
	pDC->OutputNewLine();
#endif

	return TRUE;
}


/********************************************************************************************

>	BOOL Document::ExportFlagInfo(EPSExportDC *pDC)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/3/95
	Inputs:		pDC - The DC to render to
	Returns:	-
	Purpose:	

********************************************************************************************/

BOOL Document::ExportFlagInfo(EPSExportDC *pDC)
{
	return TRUE;
}



/********************************************************************************************

>	BOOL Document::ExportUnitInfo(EPSExportDC *pDC)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/3/95
	Inputs:		pDC - The DC to export the Units to
	Returns:	TRUE
	Purpose:	Saves out all the user defined units.
				These are saved in version 0 comments lines, 1 unit per line.
				The default units being used for page and fonts are exported in:-
	SeeAlso:	ExportDefaultUnitsInfo;

********************************************************************************************/

BOOL Document::ExportUnitInfo(EPSExportDC *pDC)
{
#if !defined(EXCLUDE_FROM_RALPH)
	// A buffer
	#define EUI_BUFFER_SIZE 256
	TCHAR Buffer[EUI_BUFFER_SIZE];

	// If there is no valid DocUnit list in this document then give up
	if (pDocUnitList==NULL)
		return FALSE;

	// Find the first relevant unit in the list
	Unit* pUnit = (Unit*)pDocUnitList->GetHead();

	while (pUnit!=NULL)
	{
		// HACK: Export pixel units for compatibiility with old camelot. 
		// Really need version control
		// Although somewhat crypticly coded this is handled as PIXELS has a NOTYPE base unit
		if (pUnit->IsDefault() && (pUnit->GetUnitType() != PIXELS))
		{
			// Find the next unit in the list
			pUnit = (Unit*)pDocUnitList->GetNext(pUnit);
			continue;
		}

		// OK, we have a unit, so save out its info
		// Extract the values
		INT32 Version = 0;
		MILLIPOINT Size = (MILLIPOINT) pUnit->GetMillipoints();
		UnitType BaseUnit = pUnit->GetBaseUnitType();
		double Numerator;
		double Denominator;
		// default units can have NOTYPE so supply sensible values
		if (BaseUnit == NOTYPE)		
		{
			BaseUnit = MILLIPOINTS;	// complains in previous camelots if NOTYPE used...
			Numerator = 0.0;		// set gibberish numerator so can be picked up in import
			Denominator = 1.2;		// use this for version number in which units first appeared
		}
		else
		{
			Numerator = pUnit->GetBaseNumerator();
			ERROR3IF(Numerator == 0.0, "Document::ExportUnitInfo() - Numerator == 0.0");
			Denominator = pUnit->GetBaseDenominator();
		}
		BOOL IsPrefix = pUnit->IsPrefix();

		String_32 UnitToken = pUnit->GetToken();
		String_32 UnitName = pUnit->GetSpecifier();

		// Build the Comment
		INT32 nBytes = camSnprintf(Buffer, EUI_BUFFER_SIZE, _T("%d %ld %d %f %f %d"), Version, Size, (INT32)BaseUnit, Numerator, Denominator, IsPrefix);
		if (nBytes > EUI_BUFFER_SIZE - 1)
		{
			ERROR3("Document::ExportUnitInfo - nBytes > EUI_BUFFER_SIZE - 1");
			return FALSE;
		}
PORTNOTE("filters","Removed EPSExportDC usage")
#ifndef EXCLUDE_FROM_XARALX
		// Export it
		pDC->OutputToken("%%+u");
		pDC->OutputToken(Buffer);
		pDC->OutputString(UnitToken);
		pDC->OutputString(UnitName);
		pDC->OutputNewLine();
#endif
		// Find the next unit in the list
		pUnit = (Unit*)pDocUnitList->GetNext(pUnit);
	}

#endif	
	return TRUE;
}

/********************************************************************************************

>	BOOL Document::ExportDefaultUnitsInfo(EPSExportDC *pDC)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/9/95
	Inputs:		pDC - The DC to export the Units to
	Returns:	TRUE
	Purpose:	Saves out the user default units for displaying things like page units and
				font units to the user.
				These are saved in version 0 comments lines followed by a string for the
				page units and a string for the font units. Must come after the user units
				section as the default might be one of these user units.
				The usert unit definitions are exported in:-
	SeeAlso:	ExportUnitInfo;
				
********************************************************************************************/

BOOL Document::ExportDefaultUnitsInfo(EPSExportDC *pDC)
{
#if !defined(EXCLUDE_FROM_RALPH)
	// If there is no valid DocUnit list in this document then give up
	if (pDocUnitList != NULL)
	{
		// A buffer
		TCHAR Buffer[256];

		// Now output the default units being used for page and fonts
		// We will save all this out as version 1 information, this is deemed to be all
		// the standard units info
		// Cannot use the units field as on earlier versions of Studio, before 1.00b,
		// it will be imported as a new user unit with the default settings.
		INT32 Version = 0;

		// Get the current default settings
		UnitType PageUnits   = pDocUnitList->GetPageUnits();	// The units used to display page measurements
		UnitType FontUnits   = pDocUnitList->GetFontUnits();	// The units to display font measurements

		// Find out the full names for these units
		String_32 PageUnitToken = pDocUnitList->GetToken(PageUnits);
		String_32 FontUnitToken = pDocUnitList->GetToken(FontUnits);

		// Build the Comment
		camSnprintf(Buffer, 256, _T("%d"), Version);

PORTNOTE("filters","Removed EPSExportDC usage")
#ifndef EXCLUDE_FROM_XARALX
		// Export the font and pages units as a line of info
		pDC->OutputToken("%%+U");
		pDC->OutputToken(Buffer);
		pDC->OutputString(PageUnitToken);
		pDC->OutputString(FontUnitToken);
		pDC->OutputNewLine();
#endif
	}
#endif
	return TRUE;
}


/********************************************************************************************

>	BOOL Document::ExportDateInfo(EPSExportDC *pDC)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/3/95
	Inputs:		pDC - the dc to export to
	Returns:	TRUE
	Purpose:	exports a continuation comment that holds the Last Saved Date and the
				Creation Date. It takes the follwoing form :-
				%%+d Version(0) CreationDate LastSavedDate 

********************************************************************************************/

BOOL Document::ExportDateInfo(EPSExportDC *pDC)
{
	// A buffer
	char Buffer[256];

	// Set the last saved date to now
	SetLastSaveTime();

	// Find out what they are
	time_t Creation =  GetCreationTime();
	time_t LastSaved = GetLastSaveTime();

	// Make a string out of them
	sprintf(Buffer, "%d %ld %ld", 0, Creation, LastSaved);

PORTNOTE("filters","Removed EPSExportDC usage")
#ifndef EXCLUDE_FROM_XARALX
	// Export them
	pDC->OutputToken("%%+d");
	pDC->OutputToken(Buffer);
	pDC->OutputNewLine();
#endif
	return TRUE;
}

/********************************************************************************************

>	BOOL Document::ExportTextSetup(EPSFilter* pFilter)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/4/95
	Inputs:		pFilter - the EPS filter that is being used to export a file.
	Returns:	TRUE if the text setup for this document was written out successfully;
				FALSE if not (e.g. out of disk space etc)
	Purpose:	

********************************************************************************************/

BOOL Document::ExportTextSetup(EPSFilter* pFilter)
{
	PORTNOTETRACE("filters","Document::ExportTextSetup - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	if ( (pFilter->IS_KIND_OF(CamelotNativeEPSFilter)) ||
	     (pFilter->IS_KIND_OF(AIEPSFilter))
	   )
	{
		// Output the colour table in ArtWorks format.
		EPSExportDC *pDC = pFilter->GetExportDC();

		String_64 FontName;
		String_64 EFont;
		INT32 Style;
		FontList CurFontList;

		if (!CurFontList.Build(this))
			return FALSE;

		FontListItem* pItem = CurFontList.GetFirstItem();
		while (pItem)
		{
			FontName = pItem->GetFontName();
			Style = pItem->GetFontStyle();
			// Graeme (31-3-00) - Map the encoded name onto the PS font name.
			FONTMANAGER->EncodeAndMapFontName(FontName, EFont, Style);

			// The Page List comment:
			pDC->OutputToken("%%IncludeFont:");
			pDC->OutputToken((TCHAR*)EFont);
			pDC->OutputNewLine();
		
			pItem = CurFontList.GetNextItem(pItem);	
		}
	}
#endif
	return TRUE;
}



/********************************************************************************************

>	BOOL Document::ExportTextTrailer(EPSFilter* pFilter)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/4/95
	Inputs:		pFilter - the EPS filter that is being used to export a file.
	Returns:	TRUE if the text trailer for this document was written out successfully;
				FALSE if not (e.g. out of disk space etc)
	Purpose:	

********************************************************************************************/

BOOL Document::ExportTextTrailer(EPSFilter* pFilter)
{
	return TRUE;
}

/********************************************************************************************

>	BOOL Document::ExportUndoInfo(EPSExportDC *pDC)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	06/07/95
	Inputs:		pDC - The DC to render to
	Returns:	TRUE
	Purpose:	Exports a Document Info continuation comment that describes the document's 
				OperationHistory size.
				It takes the following form :-
				%%+h VersionNum
				if VersionNum is zero then the following will be on the rest of the line
				Size(bytes)

********************************************************************************************/

BOOL Document::ExportUndoInfo(EPSExportDC *pDC)
{
	PORTNOTETRACE("filters","Document::ExportUndoInfo - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	// Find the size of the operation history
	UINT32 Size = GetOpHistory().GetMaxSize();

	// get something to put the operation history size info into
	char Buffer[256];
	INT32 Version = 0;

	// Build a string with all the data in it
	_stprintf(Buffer, "%d %lu", Version, Size);

	// Output the line to the file
	pDC->OutputToken("%%+h");
	pDC->OutputToken(Buffer);
	pDC->OutputNewLine();
#endif
	return TRUE;
}


/********************************************************************************************

>	BOOL Document::AIExportResources(EPSExportDC *pDC, BOOL first)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/4/95
	Inputs:		pDC - the dc to export to
	Returns:	TRUE
	Purpose:	Export an Illustrator standard needed resources

********************************************************************************************/

BOOL Document::AIExportResources(EPSExportDC *pDC, BOOL first)
{
	PORTNOTETRACE("filters","Document::AIExportResources - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	if (!first)
		pDC->OutputToken("%%+");

	// Graeme (28-3-00) - I've updated these to bring them in line with the procset
	// definitions from Fireworks 3 and CorelDraw 9. These have upgraded the file
	// to AI version 7.0, and should hopefully offer gradient fill support.
	pDC->OutputToken ( "procset Adobe_level2_AI5 1.2 0 " );			pDC->OutputNewLine ();
	pDC->OutputToken ( "%%+ procset Adobe_packedarray 2.0 0" );		pDC->OutputNewLine ();
	pDC->OutputToken ( "%%+ procset Adobe_ColorImage_AI6 1.1 0" );	pDC->OutputNewLine ();
	pDC->OutputToken ( "%%+ procset Adobe_cshow 2.0 8" );			pDC->OutputNewLine ();
	pDC->OutputToken ( "%%+ procset Adobe_cmykcolor 1.1 0" );		pDC->OutputNewLine ();
	pDC->OutputToken ( "%%+ procset Adobe_customcolor 1.0 0" );		pDC->OutputNewLine ();
	pDC->OutputToken ( "%%+ procset Adobe_typography_AI5 1.0 1" );	pDC->OutputNewLine ();
	pDC->OutputToken ( "%%+ procset Adobe_pattern_AI3 1.0 1" );		pDC->OutputNewLine ();
	pDC->OutputToken ( "%%+ procset Adobe_Illustrator_AI5 1.2 0" );	pDC->OutputNewLine (); 
#endif
	return TRUE;
}


/********************************************************************************************

>	BOOL Document::AIExportProlog(EPSExportDC *pDC)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/4/95
	Inputs:		pDC - the dc to export to
	Returns:	TRUE
	Purpose:	Export an Illustrator standard prolog

********************************************************************************************/

BOOL Document::AIExportProlog(EPSExportDC *pDC)
{
	PORTNOTETRACE("filters","Document::AIExportProlog - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	pDC->OutputToken("%%IncludeResource: procset Adobe_level2_AI5 1.2 0");			pDC->OutputNewLine();
	pDC->OutputToken("%%IncludeResource: procset Adobe_packedarray 2.0 0");			pDC->OutputNewLine();
	pDC->OutputToken("%%IncludeResource: procset Adobe_ColorImage_AI6 1.1 0");		pDC->OutputNewLine();
	pDC->OutputToken("%%IncludeResource: procset Adobe_cshow 2.0 8");				pDC->OutputNewLine();
	pDC->OutputToken("%%IncludeResource: procset Adobe_cmykcolor 1.1 0");			pDC->OutputNewLine();
	pDC->OutputToken("%%IncludeResource: procset Adobe_customcolor 1.0 0");			pDC->OutputNewLine();
	pDC->OutputToken("%%IncludeResource: procset Adobe_typography_AI5 1.1 0");		pDC->OutputNewLine();
	pDC->OutputToken("%%IncludeResource: procset Adobe_pattern_AI3 1.0 1");			pDC->OutputNewLine();
	pDC->OutputToken("%%IncludeResource: procset Adobe_Illustrator_AI5 1.2 0");		pDC->OutputNewLine();
#endif
	return TRUE;
}


/********************************************************************************************

>	BOOL Document::AIExportCharEncoding(EPSExportDC *pDC)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/4/95
	Inputs:		pDC - the dc to export to
	Returns:	TRUE
	Purpose:	Export an Illustrator compatible character encoding vector but only in 
				EPS format.

********************************************************************************************/

BOOL Document::AIExportCharEncoding(EPSExportDC *pDC)
{
	PORTNOTETRACE("filters","Document::AIExportCharEncoding - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	// Temporarily output an inline encoding. This should really be read from a file.
	pDC->OutputToken("["); pDC->OutputNewLine();
	pDC->OutputToken("39/quotesingle 96/grave 128/Adieresis/Aring/Ccedilla/Eacute/Ntilde/Odieresis"); pDC->OutputNewLine();
	pDC->OutputToken("/Udieresis/aacute/agrave/acircumflex/adieresis/atilde/aring/ccedilla/eacute"); pDC->OutputNewLine();
	pDC->OutputToken("/egrave/ecircumflex/edieresis/iacute/igrave/icircumflex/idieresis/ntilde"); pDC->OutputNewLine();
	pDC->OutputToken("/oacute/ograve/ocircumflex/odieresis/otilde/uacute/ugrave/ucircumflex"); pDC->OutputNewLine();
	pDC->OutputToken("/udieresis/dagger/degree/cent/sterling/section/bullet/paragraph/germandbls"); pDC->OutputNewLine();
	pDC->OutputToken("/registered/copyright/trademark/acute/dieresis/.notdef/AE/Oslash"); pDC->OutputNewLine();
	pDC->OutputToken("/.notdef/plusminus/.notdef/.notdef/yen/mu/.notdef/.notdef"); pDC->OutputNewLine();
	pDC->OutputToken("/.notdef/.notdef/.notdef/ordfeminine/ordmasculine/.notdef/ae/oslash"); pDC->OutputNewLine();
	pDC->OutputToken("/questiondown/exclamdown/logicalnot/.notdef/florin/.notdef/.notdef"); pDC->OutputNewLine();
	pDC->OutputToken("/guillemotleft/guillemotright/ellipsis/.notdef/Agrave/Atilde/Otilde/OE/oe"); pDC->OutputNewLine();
	pDC->OutputToken("/endash/emdash/quotedblleft/quotedblright/quoteleft/quoteright/divide"); pDC->OutputNewLine();
	pDC->OutputToken("/.notdef/ydieresis/Ydieresis/fraction/currency/guilsinglleft/guilsinglright"); pDC->OutputNewLine();
	pDC->OutputToken("/fi/fl/daggerdbl/periodcentered/quotesinglbase/quotedblbase/perthousand"); pDC->OutputNewLine();
	pDC->OutputToken("/Acircumflex/Ecircumflex/Aacute/Edieresis/Egrave/Iacute/Icircumflex"); pDC->OutputNewLine();
	pDC->OutputToken("/Idieresis/Igrave/Oacute/Ocircumflex/.notdef/Ograve/Uacute/Ucircumflex"); pDC->OutputNewLine();
	pDC->OutputToken("/Ugrave/dotlessi/circumflex/tilde/macron/breve/dotaccent/ring/cedilla"); pDC->OutputNewLine();
	pDC->OutputToken("/hungarumlaut/ogonek/caron"); pDC->OutputNewLine();
	pDC->OutputToken("TE");
	pDC->OutputNewLine();
#endif
	return TRUE;
}


/********************************************************************************************

>	BOOL Document::AIExportFontEncoding(EPSExportDC *pDC)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/4/95
	Inputs:		pDC - the dc to export to
	Returns:	TRUE
	Purpose:	Export an Illustrator compatible font encodings but only in EPS format.

********************************************************************************************/

BOOL Document::AIExportFontEncoding(EPSExportDC *pDC)
{
#if !defined(EXCLUDE_FROM_RALPH)
	// need to enumerate all fonts for this encodeing
	String_64 FName;
	INT32 Style;
	FontList CurFontList;

	if (!CurFontList.Build(this))
		return FALSE;

	FontListItem* pItem = CurFontList.GetFirstItem();
	while (pItem)
	{
		FName = pItem->GetFontName();
		Style = pItem->GetFontStyle();
		AIExportFontEncoding(pDC,FName,Style);
		
		pItem = CurFontList.GetNextItem(pItem);	
	}
#endif
	return TRUE;
}


/********************************************************************************************

>	BOOL Document::AIExportFontEncoding(EPSExportDC *pDC, String_64& FontName, INT32 Style)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/4/95
	Inputs:		pDC - the dc to export to
				FontName - a reference to a font name
				Style - a font style
						b0 = 1 bold
						b1 = 1 italic 
	Returns:	TRUE
	Purpose:	Export an Illustrator compatible font encoding but only in EPS format.

********************************************************************************************/

BOOL Document::AIExportFontEncoding(EPSExportDC *pDC, String_64& FontName, INT32 Style)
{
	PORTNOTETRACE("filters","Document::AIExportFontEncoding - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	String_64 EFont;

	// Graeme (31-3-00) - Map the encoded name onto the PS font name.
	FONTMANAGER->EncodeAndMapFontName(FontName, EFont, Style);

	String_256 Line("%AI3_BeginEncoding: _");

	Line+=(TCHAR*)EFont;
	Line+=" ";
	Line+=(TCHAR*)EFont;
	pDC->OutputToken((TCHAR*)Line);
	pDC->OutputNewLine();

	Line="[/_";
	Line+=(TCHAR*)EFont;
	Line+="/";
	Line+=(TCHAR*)EFont;
	Line+=" 0 0 1 TZ";
	pDC->OutputToken((TCHAR*)Line);
	pDC->OutputNewLine();

	Line="%AI3_EndEncoding";
	pDC->OutputToken((TCHAR*)Line);
	pDC->OutputNewLine();
#endif
	return TRUE;
}


/********************************************************************************************

>	BOOL Document::AIExportExtras(EPSExportDC *pDC)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/4/95
	Inputs:		pDC - the dc to export to
	Returns:	TRUE
	Purpose:	Export an Illustrator compatible trailer

********************************************************************************************/

BOOL Document::AIExportExtras(EPSExportDC *pDC)
{
	PORTNOTETRACE("filters","Document::AIExportExtras - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	pDC->OutputToken("Adobe_level2_AI5 /initialize get exec");		pDC->OutputNewLine();
	pDC->OutputToken("Adobe_packedarray /initialize get exec");		pDC->OutputNewLine();
	pDC->OutputToken("Adobe_ColorImage_AI6  /initialize get exec");	pDC->OutputNewLine();
	pDC->OutputToken("Adobe_cshow /initialize get exec");			pDC->OutputNewLine();
	pDC->OutputToken("Adobe_cmykcolor /initialize get exec");		pDC->OutputNewLine();
	pDC->OutputToken("Adobe_customcolor /initialize get exec");		pDC->OutputNewLine();
	
		// ChrisG (31/10/00) Changed typography line to match Illustrator and CorelDraw.
		//	This allows photoshop to import the file correctly.
	pDC->OutputToken("Adobe_Illustrator_AI5_vars Adobe_Illustrator_AI5");
	pDC->OutputToken("Adobe_typography_AI5 /initialize get exec");	pDC->OutputNewLine();
		// End of typography output.

	pDC->OutputToken("Adobe_pattern_AI3 /initialize get exec");		pDC->OutputNewLine();
	pDC->OutputToken("Adobe_Illustrator_AI5 /initialize get exec");	pDC->OutputNewLine();
#endif
	return TRUE;
}

/********************************************************************************************

>	BOOL Document::AIExportTrailer(EPSExportDC *pDC)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/4/95
	Inputs:		pDC - the dc to export to
	Returns:	TRUE
	Purpose:	Export an Illustrator compatible trailer

********************************************************************************************/

BOOL Document::AIExportTrailer(EPSExportDC *pDC)
{
	PORTNOTETRACE("filters","Document::AIExportTrailer - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	pDC->OutputToken("Adobe_Illustrator_AI5 /terminate get exec");	pDC->OutputNewLine();
	pDC->OutputToken("Adobe_level2_AI5 /terminate get exec");		pDC->OutputNewLine();
	pDC->OutputToken("Adobe_packedarray /terminate get exec");		pDC->OutputNewLine();
	pDC->OutputToken("Adobe_ColorImage_AI6 /terminate get exec");	pDC->OutputNewLine();
	pDC->OutputToken("Adobe_cshow /terminate get exec");			pDC->OutputNewLine();
	pDC->OutputToken("Adobe_cmykcolor /terminate get exec");		pDC->OutputNewLine();
	pDC->OutputToken("Adobe_customcolor /terminate get exec");		pDC->OutputNewLine();
	pDC->OutputToken("Adobe_typography_AI5 /terminate get exec");	pDC->OutputNewLine();
	pDC->OutputToken("Adobe_pattern_AI3 /terminate get exec");		pDC->OutputNewLine();
#endif
	return TRUE;
}


/********************************************************************************************

>	BOOL Document::ImportPageInfo()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/3/95
	Returns:	TRUE if the import worked, FALSE means that there was a problem.
	Purpose:	Tries to import the page info. This function only returns FALSE if there
				was an actual error in the file that needs reporting.

********************************************************************************************/

ProcessEPSResult Document::ImportPageInfo(char* Comment)
{
	// Set the default version
	INT32 Version = -1;

	// Where in the comment to start looking from (skip over %%+p)
	INT32 i = 4;

	// Read in the version number
	if (!ReadNextNumber(&Version, Comment, &i))
		return EPSCommentSyntaxError;

	// Descide what to do based on the version number
	switch (Version)
	{
		case 0 :
		{
			// Version 0 expects to find the following infomation
			// Width, Height, Margin, Bleed, Dps, ShowDropShadow
			// Get something to put all the numbers into
			MILLIPOINT Width, Height;
			MILLIPOINT Margin, Bleed;
			BOOL Dps, ShowDropShadow;
			Width = Height = 0;

			// Read in the Width of the page
			if (!ReadNextNumber(&Width, Comment, &i))
				return EPSCommentSyntaxError;

			// Read in the Height of the page
			if (!ReadNextNumber(&Height, Comment, &i))
				return EPSCommentSyntaxError;

			// Read in the Margin of the page
			if (!ReadNextNumber(&Margin, Comment, &i))
				return EPSCommentSyntaxError;

			// Read in the Bleed of the page
			if (!ReadNextNumber(&Bleed, Comment, &i))
				return EPSCommentSyntaxError;

			// Read in the DPS of the page
			INT32 TempNum;
			if (!ReadNextNumber(&TempNum, Comment, &i))
				return EPSCommentSyntaxError;

			// Copy it to the bool
			Dps = (BOOL) TempNum;

			// Read in the Drop shadow state of the page
			if (!ReadNextNumber(&TempNum, Comment, &i))
				return EPSCommentSyntaxError;

			// Copy it to the bool
			ShowDropShadow = (BOOL) TempNum;

			// Find the spread to change
			Spread* pSpread = GetSelectedSpread();
			if (pSpread == NULL)
				return EPSCommentSystemError;

			// Ask the pages to change themselves
			BOOL Worked = FALSE;
			if ((Width>0) && (Height>0))
			{
				Worked = pSpread->SetPageSize(Width, Height, Margin, Bleed, Dps, ShowDropShadow);
			}
		}                 

		// Unknown version
		default:
		{
			// Here we have got a version number we do not know how to process
			// so just ignore this comment happy. we will say that the comment
			// was OK as an error is technically wrong in this case
		}
	}

	// if we get here, then everything went smoothly so return Happy
	return EPSCommentOK;
}


/********************************************************************************************

>	ProcessEPSResult Document::ImportDocumentComment(char* Comment)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/3/95
	Inputs:		Comment - The EPS comment that we have to process
	Returns:	ProcessEPSResult - the result of the import - see ProcessEPSComment()
	Purpose:	

********************************************************************************************/

ProcessEPSResult Document::ImportDocumentComment(char* Comment)
{
	// Check that this comment line ended in a CR
	size_t				StrLength = strlen( Comment );
	
	// If the string was too long, then mark it as an error
	if (StrLength>=254)
		return EPSCommentSyntaxError;

	// check to see if this line should have a CR in it
	if (Comment[StrLength-1]=='^')
	{
		Comment[StrLength-1] = '\r';
		Comment[StrLength] = '\n';
		Comment[StrLength+1] = 0;
	}

	// append this line to the document comment
	String_256			DocComment( GetComment() );
	String_256			NewComment( Comment + 5 );
	DocComment += NewComment;
	
	// Set the comment back into the document and say that all went well
	SetComment(&DocComment);

	return EPSCommentOK;
}


/********************************************************************************************

>	ProcessEPSResult Document::ImportViewInfo(char* Comment)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/3/95
	Inputs:		Comment - The EPS comment that we have to process
	Returns:	ProcessEPSResult - the result of the import - see ProcessEPSComment()
	Purpose:	

********************************************************************************************/

ProcessEPSResult Document::ImportViewInfo(char* Comment)
{
#if !defined(EXCLUDE_FROM_RALPH)
	INT32 Version = -1;
	INT32 i = 4;

	// Read in the Version Number
	if (!ReadNextNumber(&Version, Comment, &i))
		return EPSCommentSyntaxError;

	// Act on the version number
	switch (Version)
	{
		case 0 :
		case 1 :	// Version 1 is the same, only it has some extra info at the end
		{
			INT32 XOffset = 0;
			INT32 YOffset = 0;
			INT32 ScaleFactor = 1000;

			// Read in the Version Number
			if (!ReadNextNumber(&XOffset, Comment, &i))
				return EPSCommentSyntaxError;

			// Read in the Version Number
			if (!ReadNextNumber(&YOffset, Comment, &i))
				return EPSCommentSyntaxError;

			// Read in the Version Number
			if (!ReadNextNumber(&ScaleFactor, Comment, &i))
				return EPSCommentSyntaxError;

			// Adjust the scale factor by a factor of 1000
			// go via a double to avoid getting negative scales when ScaleFactor > 32k
			FIXED16 RealScale = ((double)ScaleFactor) / 1000.0;

			// See if we have a View to change
//			DocView* pView = DocView::GetSelected();
			DocView* pView = GetFirstDocView();
			if (pView != NULL && fRestoreViewOnImport)
			{
				// Set the view for the following code.  The zoom ops work on the current
				// view, as do the coordinate conversion functions.
				ERROR3IF(pView != DocView::GetCurrent(),
							"DocView::Current is screwed in Document::ImportViewInfo");
				pView->SetCurrent();

				// Change the view scale
				pView->SetViewScale(RealScale);

				// Change the scroll offsets
				WorkCoord TopCorner(XOffset, YOffset);
				pView->SetScrollOffsets(TopCorner, FALSE);

				// Update the button bar
				OpZoomComboDescriptor::Update();
			}

			// Deal with the Version 1 bits
			if (Version==1)
			{
				// The params we will read in
				INT32 Active;
				String_32 DrawingScale;
				String_32 RealScale;

				// Find the DimScale to set
				DimScale* pDimScale = NULL;
				Spread* pSpread = GetSelectedSpread();
				if (pSpread!=NULL)
					pDimScale = pSpread->GetPtrDimScale();
				
				if (pDimScale!=NULL)
				{
					// Read in the Version Number
					if (!ReadNextNumber(&Active, Comment, &i))
						return EPSCommentSyntaxError;

					// And set the active state of the scaleing
					pDimScale->SetActiveState(Active);

					// And read in the two scaling strings
					if (!ReadNextString(&DrawingScale, 32, Comment, &i))
						return EPSCommentSyntaxError;

					// Set the Drawing Scale
					BOOL Worked = pDimScale->SetDrawingScaleStr(DrawingScale);
					
					if (!ReadNextString(&RealScale, 32, Comment, &i))
						return EPSCommentSyntaxError;

					// Set the real scale
					if (Worked)
						Worked = pDimScale->SetRealScaleStr(RealScale);

					// Set the scale factor using the new strings
					if (Worked)
						pDimScale->SetScaleFactor();
					// None of the the above functions report errors if there were problems
					// should really inform the user about the problem using _R(IDW_BADDOCUMENTSCALE) in nev.h/rc
				}
			}

			break;
		}

		default :
		{
			break;
		}
	}
#endif	
	return EPSCommentOK;
}


/********************************************************************************************

>	ProcessEPSResult Document::ImportStateInfo(char* Comment)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/3/95
	Inputs:		Comment - The EPS comment that we have to process
	Returns:	ProcessEPSResult - the result of the import - see ProcessEPSComment()
	Purpose:	Imports state information.
				Version 0 holds view state information such as:-
					grid showing and snap to grid
					magnetic snap to objects
					foregound background redraw modes. 

********************************************************************************************/

ProcessEPSResult Document::ImportStateInfo(char* Comment)
{
#if !defined(EXCLUDE_FROM_RALPH)
	// The version of the state info and the read position in the string
	INT32 Version = -1;
	INT32 i = 4;

	// Find the first DocView (if there is one)
	DocView* pFirstView = GetFirstDocView();

	// Read in the Version Number
	if (!ReadNextNumber(&Version, Comment, &i))
		return EPSCommentSyntaxError;

	// Act on the version number
	switch (Version)
	{
		case 1 : 
		{
			INT32 ShowGuides			= TRUE;
			INT32 SnapToGuides		= TRUE;
			INT32 Multilayer			= FALSE;
			INT32 AllLayersVisible	= FALSE;

			// Read the ShowGuides flag
			if (!ReadNextNumber(&ShowGuides, Comment, &i))
				return EPSCommentSyntaxError;

			// Read the SnapToGuides flag
			if (!ReadNextNumber(&SnapToGuides, Comment, &i))
				return EPSCommentSyntaxError;

			// Read the Multilayer flag
			if (!ReadNextNumber(&Multilayer, Comment, &i))
				return EPSCommentSyntaxError;

			// Read the AllLayersVisible flag
			if (!ReadNextNumber(&AllLayersVisible, Comment, &i))
				return EPSCommentSyntaxError;

// In viewer leave these alone
#ifndef STANDALONE
			// Now try and set the values in the view
			if (pFirstView != NULL)
			{
				// Set all the flags to either TRUE or FALSE
				pFirstView->SetShowGuidesState(ShowGuides?TRUE:FALSE);
				pFirstView->SetSnapToGuidesState(SnapToGuides?TRUE:FALSE);

				SetMultilayer(Multilayer?TRUE:FALSE);
				SetAllVisible(AllLayersVisible?TRUE:FALSE);
			}
#endif
		}
		// Fall through for version 0 flags

		case 0 :
		{
			INT32 ShowGrid = FALSE;
			INT32 SnapToGrid = FALSE;
			INT32 SnapToMagObjects = FALSE;
			INT32 SnapToObjects = FALSE;
			INT32 ForeBackMode = FALSE;

			// Read the Grid visible flag
			if (!ReadNextNumber(&ShowGrid, Comment, &i))
				return EPSCommentSyntaxError;

			// Snap to Grids yes/no
			if (!ReadNextNumber(&SnapToGrid, Comment, &i))
				return EPSCommentSyntaxError;

			// Snap to Magnetic Objects yes/no
			if (!ReadNextNumber(&SnapToMagObjects, Comment, &i))
				return EPSCommentSyntaxError;

			// Snap to Objects yes/no
			if (!ReadNextNumber(&SnapToObjects, Comment, &i))
				return EPSCommentSyntaxError;

			// Background rendering yes/no
			if (!ReadNextNumber(&ForeBackMode, Comment, &i))
				return EPSCommentSyntaxError;

			// Read in the dummy sixth value. This is necessary because
			// a printf function had too many %ds in it for the output
			// so the program always outputs a random value after the
			// rest of them. We have to skip it, just in case.
			// Notice that we ignore any errors - if the dummy value is not there, 
			// who cares?
			INT32 dummy = 0;
			ReadNextNumber(&dummy, Comment, &i);
			
// In viewer leave these alone
#ifndef STANDALONE
			// Now try and set the values in the view
			if (pFirstView != NULL)
			{
				// Set all the flags to either TRUE or FALSE
				pFirstView->SetShowGridState(ShowGrid?TRUE:FALSE);
				pFirstView->SetSnapToGridState(SnapToGrid?TRUE:FALSE);
				pFirstView->SetSnapToMagObjectsState(SnapToMagObjects?TRUE:FALSE);
				pFirstView->SetSnapToObjectsState(SnapToObjects?TRUE:FALSE);
				pFirstView->SetForeBackMode(ForeBackMode?TRUE:FALSE);
			}
#endif
			break;
		}


		// Unknown version number
		default:
		{
			// This is not an error, as this would stop the file from loading.
			// We will just ignore this little section of information
			break;
		}
	}
#endif
	return EPSCommentOK;
}


/********************************************************************************************

>	ProcessEPSResult Document::ImportQualityInfo(char* Comment)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/3/95
	Inputs:		Comment - The EPS comment that we have to process
	Returns:	ProcessEPSResult - the result of the import - see ProcessEPSComment()
	Purpose:	

********************************************************************************************/

ProcessEPSResult Document::ImportQualityInfo(char* Comment)
{
#if !defined(EXCLUDE_FROM_RALPH)
	// The version of the state info and the read position in the string
	INT32 Version = -1;
	INT32 i = 4;

	// Read in the Version Number
	if (!ReadNextNumber(&Version, Comment, &i))
		return EPSCommentSyntaxError;

	// Act on the version number
	switch (Version)
	{
		case 0 :
		{
			// The View Quality - default to something sensible
			INT32 ViewQuality = 100;

			// Read in the Version Number
			if (!ReadNextNumber(&ViewQuality, Comment, &i))
				return EPSCommentSyntaxError;

			// OK, we got the view quality ok, so try and set it
//			DocView* pView = DocView::GetSelected();
			DocView* pView = GetFirstDocView();
			if (pView!=NULL)
			{
				// Actually set the quality in the view
				/*BOOL Worked = */pView->RenderQuality.SetQuality((INT32)ViewQuality);
			}

			break;
		}

		default:
		{
			break;
		}
	}
#endif
	return EPSCommentOK;
}


/********************************************************************************************

>	ProcessEPSResult Document::ImportGridInfo(char* Comment)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/3/95
	Inputs:		Comment - The EPS comment that we have to process
	Returns:	ProcessEPSResult - the result of the import - see ProcessEPSComment()
	Purpose:	Imports the global grid that covers the whole spread

********************************************************************************************/

ProcessEPSResult Document::ImportGridInfo(char* Comment)
{
#if !defined(EXCLUDE_FROM_RALPH)
	// The version of the state info and the read position in the string
	INT32 Version = -1;
	INT32 i = 4;

	// Read in the Version Number
	if (!ReadNextNumber(&Version, Comment, &i))
		return EPSCommentSyntaxError;

	// Act on the version number
	switch (Version)
	{
		case 0 :
		{
			double Divisions;
			INT32 SubDiv, Units, Type;
			UINT32 SubDivisions;
			UnitType Unit;
			GridType TypeOfGrid;
			
			// Read in the Divisions
			if (!ReadNextNumber(&Divisions, Comment, &i))
				return EPSCommentSyntaxError;

			// read in the sub-divisions
			if (!ReadNextNumber(&SubDiv, Comment, &i))
				return EPSCommentSyntaxError;

			// Read in the unit
			if (!ReadNextNumber(&Units, Comment, &i))
				return EPSCommentSyntaxError;

			// Read in the type of grid
			if (!ReadNextNumber(&Type, Comment, &i))
				return EPSCommentSyntaxError;

			// Put everything into the correct types
			SubDivisions = (UINT32)SubDiv;
			Unit = (UnitType)Units;
			TypeOfGrid = (GridType) Type;			

			// Get rid of the old grid, if there was one
			// and find out where the page corner is, so that the origin of the grid can be set
			BOOL FoundPage = FALSE;
			DocCoord Corner(0,0);
			Spread* pSpread = GetSelectedSpread();
			if (pSpread!=NULL)
			{
				// Find the first child of the spread
				Node* pNode = pSpread->FindFirstChild();
				while (pNode!=NULL)
				{
					// Go find the next node now
					Node* pNext = pNode->FindNext();

					// For each of the child nodes of the spread, we will check to see if it is a Node Grid
					if (pNode->IS_KIND_OF(NodeGrid))
					{
						// This is a grid all right, so toast it
						pNode->CascadeDelete();
						delete pNode;				
					}

					// see if it is a page
					if ((!FoundPage) && (pNode->IS_KIND_OF(Page)))
					{
						// We have found one, so stop looking
						FoundPage = TRUE;

						// Find out the coord of the bottom corner
						DocRect PageRect = ((Page*)pNode)->GetPageRect();
						Corner = PageRect.LowCorner();
					}

					// Find the next node
					pNode = pNext;
				}

				// Set all the global variables - What a load of B********
				NodeGrid::SetDefaultDivisions(Divisions);
				NodeGrid::SetDefaultSubdivisions(SubDivisions);
				NodeGrid::SetDefaultUnits(Unit);
				NodeGrid::SetDefaultGridType(TypeOfGrid);

				// Make a grid
				// BODGE - old builds (hence docs) save the grid spacing in divisions and units but don't
				// account for unit scaling, so as not to change doc format new docs do the same so we must
				// set the grid with scaling turned off - yuk!
				BOOL Scale=FALSE;
				NodeGrid::MakeDefaultGrid(pSpread, Scale);

				// Set the origin to 0,0 for now
				if (FoundPage)
				{
					// Find the first child of the spread
					Node* pNode = pSpread->FindFirstChild();
					while (pNode!=NULL)
					{
						// For each of the child nodes of the spread, we will check to see if it is a Node Grid
						if (pNode->IS_KIND_OF(NodeGrid))
							// This is a grid all right, so Set its origin
							((NodeGrid*)pNode)->SetOrigin(Corner.x, Corner.y);

						// Find the next node
						pNode = pNode->FindNext();
					}
				}
			}
			break;
		}

		default:
		{
			break;
		}
	}
#endif	
	return EPSCommentOK;
}


/********************************************************************************************
>	ProcessEPSResult Document::ImportOriginInfo(char* Comment)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/10/95
	Inputs:		Comment - The EPS comment that we have to process
	Returns:	ProcessEPSResult - the result of the import
	Purpose:	Imports the spread grid/user origin
	Note:		The origin should already have been set up to the page origin when importing
				the grid (ie already has a sensible default if the origin import fails)
				Also since the origin does not affect the look of the doc, if possible
				don't abort importing if an error is encountered so the doc can be recovered
********************************************************************************************/

ProcessEPSResult Document::ImportOriginInfo(char* Comment)
{
#if !defined(EXCLUDE_FROM_RALPH)
	// find the selected spread in the doc, and it's default grid
	Spread* pSpread = GetSelectedSpread();
	ERROR2IF(pSpread==NULL,EPSCommentOK,"Document::ImportOriginInfo() - could not find first spread in doc");
	NodeGrid* pDefaultGrid = pSpread->FindFirstDefaultGridInSpread();
	ERROR2IF(pDefaultGrid==NULL,EPSCommentOK,"Document::ImportOriginInfo() - could not find default grid");

	// init read position in string - ie after "%%+o"
	INT32 i = 4;

	// Read in the Version Number
	INT32 Version = -1;
	if (!ReadNextNumber(&Version, Comment, &i))
		return EPSCommentSyntaxError;

	// Act on the version number
	switch (Version)
	{
		case 0 :
		{
			// Read in the origin
			DocCoord Origin(0,0);
			if (!ReadNextNumber(&Origin.x, Comment, &i))
				return EPSCommentSyntaxError;
			if (!ReadNextNumber(&Origin.y, Comment, &i))
				return EPSCommentSyntaxError;

			// set the grid (and user) origin
			pDefaultGrid->SetOrigin(Origin.x,Origin.y);
			break;
		}

		default: ERROR2(EPSCommentOK,"Document::ImportOriginInfo() - unknown origin version");
	}
#endif
	
	return EPSCommentOK;
}


/********************************************************************************************

>	ProcessEPSResult Document::ImportRulerState(char* Comment)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/3/95
	Inputs:		Comment - The EPS comment that we have to process
	Returns:	ProcessEPSResult - the result of the import - see ProcessEPSComment()
	Purpose:	

********************************************************************************************/

ProcessEPSResult Document::ImportRulerState(char* Comment)
{
#if !defined(EXCLUDE_FROM_RALPH)
	INT32 Version = -1;
	INT32 i = 4;
	// Read in the Version Number
	if (!ReadNextNumber(&Version, Comment, &i))
		return EPSCommentSyntaxError;

	switch (Version)
	{
		case 0 :
		{
		
			INT32 ScrollerState =1;
			INT32 RulerState =0;
			// Read in the Version Number
			if (!ReadNextNumber(&ScrollerState, Comment, &i))
				return EPSCommentSyntaxError;
			if (!ReadNextNumber(&RulerState, Comment, &i))
				return EPSCommentSyntaxError;
			
			// See if we have a View to change
//			DocView* pView = DocView::GetSelected();
			DocView* pView = GetFirstDocView();
			
			// we use default visibility states if this is a new file
			// (ie. we are not opening an existing file )
			// this flag is set in CCamDoc::OnNewDocument()
			// see also CCamDoc::OnOpenDocument()
			if(CCamView::GetUseVisibleDefaults())
			{
 	 			ScrollerState = CCamView::GetDefaultScrollersState();
				RulerState = CCamView::GetDefaultRulersState();
   			}
		
			if (pView != NULL)
			{
			 	pView->ShowViewScrollers((BOOL)ScrollerState);
				pView->ShowViewRulers((BOOL)RulerState);
			}
			break;
		}

		default :
		{
			break;
		}
	}
#endif	

	return EPSCommentOK;
}


/********************************************************************************************

>	ProcessEPSResult Document::ImportFlagInfo(char* Comment)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/3/95
	Inputs:		Comment - The EPS comment that we have to process
	Returns:	ProcessEPSResult - the result of the import - see ProcessEPSComment()
	Purpose:	

********************************************************************************************/

ProcessEPSResult Document::ImportFlagInfo(char* Comment)
{
	return EPSCommentOK;
}


/********************************************************************************************

>	ProcessEPSResult Document::ImportUnitInfo(char* Comment)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/3/95
	Inputs:		Comment - The comment line to process
	Returns:	EPSCommentSyntaxError if there was a systax error in the comment
				EPSCommentOK if the comment was processed ok
	Purpose:	Imports a continuation comment with details about a user defined unit on it
				There is 1 unit per comment. This comment always creates a new user unit
				in version prior to Studio 1.00b, code updated to not do this 4/9/95.

********************************************************************************************/

ProcessEPSResult Document::ImportUnitInfo(char* Comment)
{
	// If there is no valid DocUnit list in this document then give up
	ERROR3IF(pDocUnitList==NULL,"Document::ImportUnitInfo null pDocUnitList");
	if (pDocUnitList==NULL)
		return EPSCommentOK;

	// Set up some variables.
	// The version of the state info and the read position in the string
	INT32 Version = -1;
	INT32 i = 4;

	// Read in the Version Number
	if (!ReadNextNumber(&Version, Comment, &i))
		return EPSCommentSyntaxError;

	// Act on the version number
	switch (Version)
	{
		case 0 :
		{
			// We need to make a new unit
			// Older code had this outside the version loop so you cannot use the 'u'
			// comment field for other unit field without creating a new unit in older
			// versions e.g. viewer 1.00 and Studio 1.00b and before. 

			// Read in values & validate them before creating the unit

			INT32 Size;
			INT32 BaseUnit;
			double Numerator;
			double Denominator;
			INT32 IsPrefix;
			String_64 TokenStr;
			String_64 SpecifierStr;
				
			// Read in the Millipoint size of the unit
			if (!ReadNextNumber(&Size, Comment, &i))
				return EPSCommentSyntaxError;

			// Read in the base UnitType
			if (!ReadNextNumber(&BaseUnit, Comment, &i))
				return EPSCommentSyntaxError;

			// Read in the unit Numerator
			if (!ReadNextNumber(&Numerator, Comment, &i))
				return EPSCommentSyntaxError;

			// Read in the unit Denominator
			if (!ReadNextNumber(&Denominator, Comment, &i))
				return EPSCommentSyntaxError;

			// Read in whether the  unit specifier is prefixed (0 == FALSE)
			if (!ReadNextNumber(&IsPrefix, Comment, &i))
				return EPSCommentSyntaxError;

			// Finally read in the string values
			if (!ReadNextString(&TokenStr, 32, Comment, &i))
				return EPSCommentSyntaxError;

			// Read in the specifier string
			if (!ReadNextString(&SpecifierStr, 32, Comment, &i))
				return EPSCommentSyntaxError;

			// Throw away units that have been exported for compatibility with older versions (see ExportUnitInfo())
			if (Numerator == 0.0 && Denominator <= 2.0) break;

			Unit* pNewUnit = NULL;
			if (!pDocUnitList->MakeNewUnit(&pNewUnit, TRUE))
				return EPSCommentSystemError;

			// Set the values in the units up
			pNewUnit->SetMillipoints((double)Size);
			pNewUnit->SetBaseUnitType((UnitType) BaseUnit);
			pNewUnit->SetBaseNumerator(Numerator);
			pNewUnit->SetBaseDenominator(Denominator);
			pNewUnit->SetPrefixState((BOOL)IsPrefix);
			// Set the token
			pNewUnit->SetToken(TokenStr);
			// Set the Specifier
			pNewUnit->SetSpecifier(SpecifierStr);

			break;
		}

		default:
		{
			// Do nothing with unknown versions as this means info we know nothing about.
			// Do not error though as this might be bad.
			break;
		}
	}

	return EPSCommentOK;
}

/********************************************************************************************

>	ProcessEPSResult Document::ImportDefaultUnitsInfo(char* Comment)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/9/95
	Inputs:		Comment - The comment line to process
	Returns:	EPSCommentSyntaxError if there was a systax error in the comment
				EPSCommentOK if the comment was processed ok
	Purpose:	Imports a continuation comment with details about the default units which
				are going to be used for showing page and font measurements.
				Cannot use units as on older version will always create a new user unit for
				each ++u found..

********************************************************************************************/

ProcessEPSResult Document::ImportDefaultUnitsInfo(char* Comment)
{
// In viewer leave these alone as not useful
#ifndef STANDALONE

	// If there is no valid DocUnit list in this document then give up
	ERROR3IF(pDocUnitList==NULL,"Document::ImportDefaultUnitsInfo null pDocUnitList");
	if (pDocUnitList==NULL)
		return EPSCommentOK;

	// Set up some variables.
	// The version of the state info and the read position in the string
	INT32 Version = -1;
	INT32 i = 4;

	// Read in the Version Number
	if (!ReadNextNumber(&Version, Comment, &i))
		return EPSCommentSyntaxError;

	// Act on the version number
	switch (Version)
	{
		case 0 :
		{
			// Read in the string values which define the units to use
			// First the page units, the units used to display page measurements
			String_64 PageUnitsStr;
			if (!ReadNextString(&PageUnitsStr, 32, Comment, &i))
				return EPSCommentSyntaxError;

			// Then the font units, the units to display font measurements
			String_64 FontUnitsStr;
			if (!ReadNextString(&FontUnitsStr, 32, Comment, &i))
				return EPSCommentSyntaxError;

			// Find the unit in the list, if present, otherwise will return NOTYPE
			UnitType PageUnits   = pDocUnitList->FindUnitTypeFromToken(PageUnitsStr);
			UnitType FontUnits   = pDocUnitList->FindUnitTypeFromToken(FontUnitsStr);	

			// Set the new current default settings, if valid units were found
			if (PageUnits != NOTYPE)
				pDocUnitList->SetPageUnits(PageUnits);
			if (FontUnits!= NOTYPE)
				pDocUnitList->SetFontUnits(FontUnits);

			break;
		}

		default:
		{
			// Do nothing with unknown versions as this means info we know nothing about.
			// Do not error though as this might be bad.
			break;
		}
	}
#endif

	return EPSCommentOK;
}

/********************************************************************************************

>	ProcessEPSResult Document::ImportDateInfo(char* Comment)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/3/95
	Inputs:		Comment - The Comment that we have to process
	Returns:	EPSCommentOK
	Purpose:	Reads in the Last saved Date and Creation Date from the file.

********************************************************************************************/

ProcessEPSResult Document::ImportDateInfo(char* Comment)
{
#if !defined(EXCLUDE_FROM_RALPH)
	// The version of the state info and the read position in the string
	INT32 Version = -1;
	INT32 i = 4;

	// Read in the Version Number
	if (!ReadNextNumber(&Version, Comment, &i))
		return EPSCommentSyntaxError;

	// Act on the version number
	switch (Version)
	{
		case 0 :
		{
			INT32 Creation, LastSaved;

			// Read in the Creation Date
			if (!ReadNextNumber(&Creation, Comment, &i))
				return EPSCommentSyntaxError;

			// Read in the Last Saved Date
			if (!ReadNextNumber(&LastSaved, Comment, &i))
				return EPSCommentSyntaxError;

			// Set the vars in the document
			CreationTime = (time_t)Creation;
			LastSaveTime = (time_t)LastSaved;
			break;
		}

		default:
		{
			break;
		}
	}
#endif

	return EPSCommentOK;
}

/********************************************************************************************

>	ProcessEPSResult Document::ImportUndoInfo(char* Comment)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	06/07/95
	Inputs:		Comment - The Comment that we have to process
	Returns:	EPSCommentOK
	Purpose:	Reads in the Last saved operation history size from the file.

********************************************************************************************/

ProcessEPSResult Document::ImportUndoInfo(char* Comment)
{
	// The version of the state info and the read position in the string
	INT32 Version = -1;
	INT32 i = 4;

	// Read in the Version Number
	if (!ReadNextNumber(&Version, Comment, &i))
		return EPSCommentSyntaxError;

	// Act on the version number
	switch (Version)
	{
		case 0 :
		{
			UINT32 UndoSize;

			// Read the size for the operation history
			if (!ReadNextNumber(&UndoSize, Comment, &i))
				return EPSCommentSyntaxError;

			// Minimum size = 1K
			if (UndoSize < 1024)
				UndoSize = 1024;

			// Set the size of the Operation History in
			// the document.
			GetOpHistory().SetNewMaxSize(UndoSize);

			break;
		}

		default:
		{
			break;
		}
	}

	return EPSCommentOK;
}



/********************************************************************************************

>	BOOL Document::IsImporting()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/3/95
	Returns:	TRUE if the document is importing a file as apposed to Opening a file
	Purpose:	Helps work out if the document is in the process of opening an existing
				file or if it is importing data into itself. Called during the reading
				of the Document Info section of the .art files. All the DocInfo section
				is ignored if the file is actually being imported.

********************************************************************************************/

BOOL Document::IsImporting()
{
	return DocIsImporting;
}


/********************************************************************************************

>	void Document::SetIsImporting(BOOL NewIsImporting)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/3/95
	Inputs:		NewIsImporting - The new state of the importing flag
	Purpose:	Sets the IsImporting flag for the document

********************************************************************************************/

void Document::SetIsImporting(BOOL NewIsImporting)
{
	// Set the flag to either TRUE or FALSE
	if (NewIsImporting)
		DocIsImporting = TRUE;
	else
		DocIsImporting = FALSE;
}

/********************************************************************************************

>	BOOL Document::IsTemplateLoading()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com> Humprhys
	Created:	6/8/97
	Returns:	TRUE if the document is importing a file as apposed to Opening a file
	Purpose:	Helps work out if the document being opened or imported is a default or
				template document.
				At present, used in Webster to check if we need to use layers or not. Template
				documents are allowed to have layers. Non-template docs are not.
	SeeAlso:	

********************************************************************************************/

BOOL Document::IsTemplateLoading()
{
	return DocTemplateLoading;
}

/********************************************************************************************

>	void Document::SetTemplateLoading(BOOL NewTemplateLoading)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com> Humprhys
	Created:	6/8/97
	Inputs:		NewTemplateLoading - The new state of the template loading flag
	Purpose:	Sets the TemplateLoading flag for the document

********************************************************************************************/

void Document::SetTemplateLoading(BOOL NewTemplateLoading)
{
	// Set the flag to either TRUE or FALSE
	if (NewTemplateLoading)
		DocTemplateLoading = TRUE;
	else
		DocTemplateLoading = FALSE;
}

/********************************************************************************************
>	static void Document::ShouldRestoreViewOnImport(BOOL fNewState)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/10/95
	Inputs:		fNewState			new value of the flag
	Purpose:	Sets/clears a flag that if TRUE will ask this document to restore the view
				saved within the doc, if FALSE it won't bother.  Normally we want the
				view saved in the doc to be restore, but if we are restoring the entire
				workspace then this is dealt with elsewhere.
	Errors:		-
	SeeAlso:	Document::ImportViewInfo; LoadAppWindowState
********************************************************************************************/

void Document::ShouldRestoreViewOnImport(BOOL fNewState)
{
	fRestoreViewOnImport = fNewState;
}




/***********************************************************************************************

> 	Document::GetDocFontUnits()

    Author: 	Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
    Created:	6/5/95
   	Returns:	Returns Font UnitType for this document
    Purpose:	return Font Units for this document
	 			default to COMP_POINTS if no unit list available

*************************************************************************************************/                   			 

UnitType Document::GetDocFontUnits()
{	
	if(pDocUnitList)
		return pDocUnitList->GetFontUnits();
	else
		return COMP_POINTS;
}


/***********************************************************************************************

> 	BitmapList* Document::GetBitmapList()

    Author: 	Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
    Created:	20/8/96
   	Returns:	Returns this document's bitmap list
    Purpose:	Gets the bitmap list from this documents doc component

*************************************************************************************************/                   			 

BitmapList* Document::GetBitmapList()
{
	// Find this documents BitmapListComponent
	DocComponent* DocBitmapList = 
			GetDocComponent(CC_RUNTIME_CLASS(BitmapListComponent));

	ERROR2IF(DocBitmapList == NULL, FALSE, "Couldn't find document bitmap list");
	if (DocBitmapList == NULL)
		return NULL;

	// Now get the actual bitmap list
	return ((BitmapListComponent*)DocBitmapList)->GetBitmapList();
}



/********************************************************************************************

>	BOOL Document::SetDocNudge (UINT32 newVal)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/8/2000
	Inputs:		newVal - the new documents nudge size
	Returns:	-
	Purpose:	Sets the documents nudge size
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL Document::SetDocNudge (UINT32 newVal)
{
	m_docNudge = newVal;
	OpNudge::SetNudgeStep (m_docNudge);		// I need to call this directly to get camelot to
											// use the new value.
	return (TRUE);
}



/***********************************************************************************************

> 	INT32 Document::GetNumLayers() const

    Author: 	Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
    Created:	8/7/97
   	Returns:	-
    Purpose:	Counts the layers in this document.

				This function assumes there is only one spread in the document, so 
				it will fail if we ever implement multiple spread documents (chuckle).

*************************************************************************************************/                   			 

INT32 Document::GetNumLayers()
{
	//Find the first spread in the document
PORTNOTE("spread", "Multi-spread warning!")
	Spread* pSpread = FindFirstSpread();

	//Now find the first layer in that spread
	Layer* pLayer = NULL;
	if (pSpread != NULL)
		pLayer = pSpread->FindFirstLayer();

	//And count the layers
	INT32 iCount = 0;

	while (pLayer != NULL)
	{
		iCount++;
		pLayer=pLayer->FindNextLayer();
	}

	return iCount;
}

/***********************************************************************************************

> 	BOOL Document::IsAnimated() const

    Author: 	Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
    Created:	27/10/97
   	Returns:	-
    Purpose:	Finds whether this document is an animation. 

				We assume the document is an animation if it contains one or more
				frame layers

*************************************************************************************************/                   			 

BOOL Document::IsAnimated()
{
	//Find the first spread in the document
PORTNOTE("spread", "Multi-spread warning!")
	Spread* pSpread = FindFirstSpread();

	//Find the first frame layer in that spread
	Layer * pFrame = pSpread->FindFirstFrameLayer();

	//If there is one, return TRUE. Otherwise return FALSE
	return !(pFrame==NULL);
	
	
}




/***********************************************************************************************

>	Progressive Rendering

*************************************************************************************************/                   			 

/***********************************************************************************************

> 	Node* Document::GetLastSafeNodeToRender()

    Author: 	Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
    Created:	6/9/96
    Purpose:	Returns a reference to the document's 'Safe render pointer'

*************************************************************************************************/                   			 

SafeRenderPointer& Document::GetSafeRenderPointer()
{
	return TheSafeRenderPointer;
}


/***********************************************************************************************

> 	SafeRenderPointer::SafeRenderPointer()

    Author: 	Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
    Created:	6/9/96
    Purpose:	Constructs a SafeRenderPointer object

*************************************************************************************************/                   			 

SafeRenderPointer::SafeRenderPointer()
{
	m_bPointerValid = TRUE;
	pLastSafeNodeToRender = NULL;
}

/***********************************************************************************************

> 	SafeRenderPointer::~SafeRenderPointer()

    Author: 	Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
    Created:	6/9/96
    Purpose:	Destroys a SafeRenderPointer object

*************************************************************************************************/                   			 

SafeRenderPointer::~SafeRenderPointer()
{
	// Empty.
}

/***********************************************************************************************

> 	BOOL SafeRenderPointer::UpdateLastSafeNode(Node* pNewNode)

    Author: 	Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
    Created:	6/9/96
    Purpose:	Updates the last safe node that can be rendered

*************************************************************************************************/                   			 

BOOL SafeRenderPointer::UpdateLastSafeNode(Node* pNewNode)
{
	ERROR2IF(!m_bPointerValid, FALSE, "Trying to set safe pointer when it's invalid.");

#ifdef RALPH

	if ((!pLastSafeNodeToRender) && pNewNode)
	{
		NodeDocument* pNodeDoc = (NodeDocument*) pNewNode->FindParent(CC_RUNTIME_CLASS(NodeDocument)); 
		ASSERT (pNodeDoc); 
		if (pNodeDoc)
		{
			Document* TheDoc = (Document*)pNodeDoc->GetParentDoc();
			ASSERT(TheDoc); 
			if (TheDoc)
			{
				RalphDocument* pRalphDoc = TheDoc->GetRalphDoc(); 
				if (pRalphDoc)
				{
					RalphView *	pRalphView = pRalphDoc->GetRalphView();
					if(pRalphView)
					{
					/*	OpZoomDescriptor::FakeZoomToRect(&(pRalphDoc->GetViewportRect()));
						pRalphView->SetSafeToDraw(TRUE);
					*/
						//Beep(1500,15);

						SendMessage(pRalphView->GetSafeHwnd(),WM_ZOOMTORECT,NULL,(INT32)(pRalphDoc->GetViewportRect()));
				 	}
				}
			}
		}
	}

#endif

	// Make sure this pointer cannot be accessed by other threads
	// while we update it
//	CCamApp::EnterSafeRenderCriticalSection();
	
//	TRACEUSER( "Will", _T("Thread %d is setting safe node to %x\n"), GetCurrentThreadId(), pNewNode);
	pLastSafeNodeToRender = pNewNode;

	// Ok, it's safe for other threads to access us now
//	CCamApp::ExitSafeRenderCriticalSection();
	return TRUE;
}

/***********************************************************************************************

> 	Node* SafeRenderPointer::GetLastSafeNode()

    Author: 	Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
    Created:	6/9/96
    Purpose:	Returns a pointer to the last safe node that can be rendered

*************************************************************************************************/                   			 

Node* SafeRenderPointer::GetLastSafeNode()
{
	if (!m_bPointerValid)
		return NULL;

	Node* pSafeNode = NULL;

	// Make sure this pointer cannot be updated by another thread while we read it
//	CCamApp::EnterSafeRenderCriticalSection();

	pSafeNode = pLastSafeNodeToRender;
//	TRACEUSER( "Will", _T("Thread %d is reading safe node as %x\n"), GetCurrentThreadId(), pSafeNode);

	// Ok, it's safe for other threads to update it now
//	CCamApp::ExitSafeRenderCriticalSection();

	return pSafeNode;
}

/***********************************************************************************************

> 	void SafeRenderPointer::SetPointerValid()

    Author: 	Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
    Created:	6/9/96
    Purpose:	Sets the pointer as being valid

*************************************************************************************************/                   			 

void SafeRenderPointer::SetPointerValid()
{
//	TRACEUSER( "Will", _T("Thread %d is setting safe node ptr as VALID\n"), GetCurrentThreadId());
	m_bPointerValid = TRUE;
}

/***********************************************************************************************

> 	void SafeRenderPointer::SetPointerInValid()

    Author: 	Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
    Created:	6/9/96
    Purpose:	Sets the pointer as being invalid

*************************************************************************************************/                   			 

void SafeRenderPointer::SetPointerInValid()
{
//	TRACEUSER( "Will", _T("Thread %d is setting safe node ptr as INVALID\n"), GetCurrentThreadId());
	m_bPointerValid = FALSE;
}

/***********************************************************************************************

> 	BOOL SafeRenderPointer::IsPointerValid()

    Author: 	Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
    Created:	6/9/96
    Purpose:	Returns whether or not the pointer is valid

*************************************************************************************************/                   			 

BOOL SafeRenderPointer::IsPointerValid()
{
	return m_bPointerValid;
}
