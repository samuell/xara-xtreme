// $Id: colormgr.h 662 2006-03-14 21:31:49Z alex $
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

// colormgr.h - Centralised colour management class

#ifndef INC_COLORMGR
#define INC_COLORMGR

#include "colourix.h"
#include "collist.h"
#include "pump.h"
#include "undoop.h" 
#include "colmsg.h"       
//#include "xaracms.h"

class Document;
class AttrFillGeometry;
class AttrStrokeColour;


/********************************************************************************************

>	class ColourManager : public MessageHandler

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/5/94
	Purpose:	This class provides miscellaneous shared functions for use by
				various colour entities. The provided functions are currently:

					* Determine the current colour list

						This is the Indexed Colour list for the SelectedDocument
						(if any). It is displayed by the colour bar, and/or edited
						by the Colour Manager Dialogue.

					* Find a suitable global or document-specific colour context to use

					* Handle colour system state changes

						Messages are sent whenever the current list changes.
						This happens when:

							1) A colour is inserted/deleted/edited in the 
							   SelectedDocument,

							2) The SelectedDocument changes,

							3) The Colour List is deselected (i.e. when there is
							   no SelectedDocument, there is no available list)

						These messages allow the colour manager dialogues and
						colour bar (and other clients) to update their contents
						(or hide, etc) when such changes occur.

						Other processing (invalidating colour caches, etc) may
						also occur in some cases. This is all handled internally by
						the ColourManager.

					* Copy or create new colours

					* Find "Useful" colours for the editor to edit and the colour
						line/gallery to display.

					* Miscellaneous other useful functions. See colormgr.h for details
					  of currently available functions


	SeeAlso:	ColourBar; ColourMgrDlg; ColourChangingMsg; ColourList
	Documentation:	HowToUse\colours95.doc

********************************************************************************************/

class ColourManager : public MessageHandler
{
CC_DECLARE_DYNAMIC( ColourManager ) 

public:		// Initialisation/Deinitialisation
	ColourManager();

	static BOOL Init(void);
	static void Deinit(void);
			// Initialisation/DeInitialisation of the colour systems


public:		// Finding an appropriate ColourList or ColourContext to use
	static ColourList *GetColourList(BOOL InvalidateCache = FALSE);
			// ColourList for the SELECTED doc

	static ColourList *GetCurrentColourList(void);
			// ColourList for the CURRENT doc (eek!)

	static ColourContext *GetColourContext(ColourModel Model, Document *ScopeDocument = NULL);
	static ColourContext *GetColourContext(ColourModel Model, View *ScopeView);
			// Find an appropriate global or document/view-specific colour context

	static void SelViewContextHasChanged(void);
			// Cause a redraw of the selected view, plus the colour editor, line, gallery,
			// etc, via a broadcast of ColourChangingMsg::SELVIEWCONTEXTCHANGED


public:		// System notification veneers
	static void ColourHasChanged(Document *ScopeDoc, ColourList *ColList, IndexedColour *Col);
	static void ColourHasChangedInvisible(Document *ScopeDoc, ColourList *ColList,
											IndexedColour *Col);
			// Broadcast the fact that a colour has changed

	static void ColourListHasChanged(ColourList *ColList);
			// Broadcast the fact that a colour list has changed

	static BOOL HideColours(ColourList *ColList, IndexedColour **ColourArray,
							BOOL ForceDelete);
			// Hide (delete-with-undo) all colours in a given NULL terminated list

	static BOOL UnHideColours(ColourList *ColList, IndexedColour **ColourArray);
			// Unhide (undelete/create-with-undo) all colours in a given NULL terminated list

	static BOOL ChangeColour(ColourList *ColList,
							IndexedColour *NewDefn, IndexedColour *Target,
							BOOL ChangeisInvisible = FALSE);
			// Change a colour's definition, with undo

	static void ForceRedrawOfChangedColours(Document *ScopeDoc, IndexedColour **Colours);
			// Force redraw any portions of the document which reference any of the
			// given NULL-terminated list of IndexedColours


public:		// Colour copying/creation
	static IndexedColour *GenerateNewNamedColour(ColourList *ParentList,
												 IndexedColour *SourceColour);
			// Create a new named IndexedColour, copying the definition from the SourceColour,
			// or from the current Fill Colour attribute, if possible.

	static IndexedColour *GenerateNewUnnamedColour(ColourList *ParentList,
													DocColour *SourceColour);
			// Creates a new unnamed colour, taking its definition from the given DocColour
			// (which may well reference an IndexedColour). The new colour will be automatically
			// added to the given colour list, and will be auto-deleted when no longer in use

	static void EnsureColourIsInDocument(Document *SourceDoc, Document *DestDoc,
											DocColour *SourceAndResultCol);
			// Ensures that the given colour is safe to use in the given document. Possibly
			// copies the parent colour and redirects the given DocColour to the copy in the
			// dest document in order to ensure that you are safe.


public:		// Finding current/useful colours
	static BOOL GetCurrentLineAndFillAttrs(AttrStrokeColour **LineAttr,
											AttrFillGeometry **FillAttr);
			// Finds the current fill/stroke attributes (semi-internal use only - see below)

	static BOOL GetCurrentLineAndFillColours(DocColour **LineCol, DocColour **FillCol,
												DocColour **EndGradFill = NULL);
			// Finds the current fill/stroke colours, if any. If EndGradFill is non-NULL, then
			// grad fills will return start and end colours if both selected.

	static void FindColourOfInterestToUser(IndexedColour **ResultCol,
											ColourList **ResultList = NULL);
	static void FindColourOfInterestToUser(DocColour *ResultCol,
											ColourList **ResultList = NULL,
											BOOL WantLineColour = FALSE);
			// Finds the colour (if any) which is of most "interest" to the user.
			// That is, sortof the 'current' colour which should be shown in the colour
			// editor, and used to copy new colours from, etc. This is a colour from a
			// weird selecion including colour of selected fill blobs, objects in the
			// selection, current colours, etc. Just trust me. Can return NULL/NULL.


protected:
	static ColourList	*CurrentColourList;
	static Document		*ScopeDocument;

protected:
	MsgResult Message(Msg* Msg);			// Overridden message reciever
};






/********************************************************************************************

>	class OpHideColours: public UndoableOperation

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/6/94
	Purpose:	An operation which will delete 

	Scope:		Private - for use only by the ColourManager

	SeeAlso:	ActionHideColours;
				ColourManager::HideColours; ColourManager::UnHideColours

********************************************************************************************/


// Structure for passing the data indicating what the Op is expected to do
typedef struct
{
	ColourList *ParentList;		// The ColourList in which the colours reside
	IndexedColour **Colours;	// Pointer to NULL terminated list of (IndexedColour *)'s
	BOOL HideThem;				// TRUE to hide, FALSE to unhide, the colours
	BOOL ForceDelete;			// TRUE  = Forcibly delete all colours in the list
								// FALSE = Only delete colours not in use
} HideColoursInfo;




#define OPTOKEN_HIDECOLOURS _T("DeleteColours")	// Optoken for the Colour hiding operation
#define OPTOKEN_SHOWCOLOURS _T("CreateColours")	// Optoken for the Colour unhiding operation

class CCAPI OpHideColours: public UndoableOperation
{
	CC_DECLARE_DYNCREATE( OpHideColours )

public:
	OpHideColours();
	static BOOL		Init();
	static OpState	GetState(String_256*, OpDescriptor*);
	void			Do(OpDescriptor *);
	void			DoWithParam(OpDescriptor *, OpParam *Param);

	virtual BOOL Undo();
	virtual BOOL Redo();
	virtual void GetOpName(String_256*);


private:
	IndexedColour **Colours;	// Pointer to NULL terminated list of IndexedColourPtrs
	ColourList *ParentColList;	// Pointer to the colour list in which these colours reside
	BOOL HideThem;				// TRUE if we're a "delete colours" op, FALSE if we're a "create colours"

private:
	NodeAttribute *HideAttrNode(Node *CurNode, DocColour *TheColour);
	void KillColourRef(DocColour *Perpetrator, IndexedColourPtr *ColourMap);
	void HideColourRefs(void);

	void RemoveColour(DocColour *Ref, INT32 *LastColour);
	void RemoveReferencedColours(void);
};



/********************************************************************************************

>	class ActionHideColours: public Action

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/6/94
	Purpose:	When executed, sets/clears an IndexedColour's 'deleted' flag, so that
				it becomes {un}hidden - this is then un/redoable by using a HideAction with
				the opposite 'HideFlag' value.

	Scope:		Private - for use only by the ColourManager

	SeeAlso:	OpHideColours

********************************************************************************************/

class CCAPI ActionHideColours : public Action
{
	CC_DECLARE_DYNCREATE( ActionHideColours )

public:
	ActionHideColours();
	~ActionHideColours();

	virtual void Slaughter(void);
	virtual ActionCode Execute();

	static ActionCode Init(Operation* const pOp,
							ActionList* pActionList,
							UINT32 ActionSize,
							ColourList *ParentList,		// ColourList in which the cols reside
							IndexedColour* Colour[],	// NULL terminated list of colours
														// to be processed
							BOOL HideThem,				// TRUE = Hide, FALSE = UnHide
							Action** NewAction);

private:
	ColourList		*ParentList;		// The list in which the colours reside
	IndexedColour	**ChangeColour;		// The Colours to change
	BOOL			HideFlag;			// How to change them (Hide/UnHide)
};








/********************************************************************************************

>	class OpColourChange: public UndoableOperation

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/6/94
	Purpose:	This class represents the ColourChange operation which is created by the 
				ColourManager to make changes to the definition of an IndexedColour. This
				allows edits of an IndexedColour to be undone/redone.

	Scope:		Private - for use only by the ColourManager

	SeeAlso:	ActionColourChange;

********************************************************************************************/

#define OPTOKEN_COLOURCHANGE _T("ColourChange")	// Optoken for the Colour change operation


typedef struct
{
	ColourList		*ParentList;		// The list in which the change is taking place
	IndexedColour 	*NewDefn;			// The new definition of the colour (undo record)
	IndexedColour 	*Target;			// The colour to be changed (redo record)
	BOOL			InvisibleChange;	// The change is 'visible' (should cause redraw of
										// objects coloured in this colour)
} ColourChangeInfo;



class CCAPI OpColourChange: public UndoableOperation
{
	CC_DECLARE_DYNCREATE( OpColourChange )

public:
	OpColourChange();
	~OpColourChange();

	static BOOL		Init();
	static OpState	GetState(String_256 *, OpDescriptor *);
	void			Do(OpDescriptor *);
	void			DoWithParam(OpDescriptor *, OpParam *);

	virtual void	PerformMergeProcessing(void);

	virtual BOOL Undo();
	virtual BOOL Redo();

protected:
	IndexedColour *UndoRecord;		// We 'own' the UndoRecord & are responsible for deleting it
	IndexedColour *RedoRecord;		// We do not own the redo record - it is just useful info
									// for operation merging purposes
};



/********************************************************************************************

>	class ActionColourChange: public Action

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/6/94
	Friends:	OpColourChange
	
	Purpose:	When executed, this copies the IndexedColour 'Defn' over the top of
				the IndexedColour 'Target', so that Target is redefined. This is used
				to undo/redo colour edits. A flag 'ChangeIsinvisible' is used to tell
				whether to broadcast a ColourChangingMsg COLOURUPDATED or
				COLOURUPDATEDINVISIBLE when the change is applied.
	
	Scope:		Private - for use only by the ColourManager

	SeeAlso:	OpColourChange

********************************************************************************************/

class CCAPI ActionColourChange : public Action
{
	CC_DECLARE_DYNCREATE( ActionColourChange )

	friend class OpColourChange;		// Allows the Op's merging code to alter 'IsInvisible'

public:
	ActionColourChange();
	~ActionColourChange();
	virtual ActionCode Execute();
	static ActionCode Init(Operation* const pOp,
							ActionList *pActionList,
							UINT32 ActionSize,
							IndexedColour *TheTarget,
							IndexedColour *TheNewDefn,
							BOOL ChangeIsInvisible,
							Action **NewAction);

protected:
	BOOL IsInvisible;

private:
	IndexedColour *Target;
	IndexedColour *NewDefn;
};





/********************************************************************************************

>	class OpRedrawColours: public UndoableOperation

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/1/96

	Purpose:	This non-undoable operation is used by the colour manager to redraw all
				nodes of the document which have been affected by changes to a colour or
				colours.

				It achieves this by doing an AllowOp pass over the document tree, marking
				all affected nodes, then causing a redraw of the marked nodes.

	Scope:		Private - for use only by the ColourManager

	SeeAlso:	ColourManager::ForceRedrawOfChangedColours

********************************************************************************************/

typedef struct
{
	Document *ScopeDoc;
	IndexedColour **Colours;
} OpRedrawColoursInfo;


#define OPTOKEN_REDRAWCOLOURS _T("RedrawColours")	// Optoken for the RedrawColours operation

class CCAPI OpRedrawColours : public Operation
{
	CC_DECLARE_DYNCREATE( OpRedrawColours )

public:
	OpRedrawColours();
	~OpRedrawColours();

	static BOOL		Init();
	static OpState	GetState(String_256 *, OpDescriptor *);
	void			Do(OpDescriptor *);
	void			DoWithParam(OpDescriptor *, OpParam *);
};


#endif


