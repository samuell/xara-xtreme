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
// Header for the Text tool implementation

// This file contains the following class definitions

// class OpTextUndoable: 	All Undoable text operations are derived from this base class. It
//						 	currently provides a set of useful DO functions for text ops.
// class OpCreateTextObject:This is the op which gets executed to create a text object
// class OpTextFormat:		This op groups together those text operations which will cause
//							the TextStory to be formatted.
// class OpFitTextToCurve:	Self explanatory

// class OpTextCaret:		Derived from Operation. This Op groups together all caret operations.

#ifndef INC_TEXTOPS
#define INC_TEXTOPS

//#include "selop.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "cutop.h"

class BaseTextClass;
class TextStory;
class TextLine;
class CaretNode;
class AbstractTextChar;
class VisibleTextNode;
class AttrTypeSet;
class AttrTxtLineSpace;
class AttrTxtJustification;

// All operations OpTokens go here
// Currently most ops do not have associated OpDescriptors, as they do not require GetState
// functions. They are unlikely to ever get added to bars or get exposed for OLE-automation.
// They are what they call 'Internal operations'

// The FitTextToPath operation needs to live on menus etc. I suspect so it does need an OpToken
#define OPTOKEN_FITTEXTTOPATH		_T("FitTextToPath")
#define OPTOKEN_KERNTEXT			_T("Kern")
#define OPTOKEN_AUTOKERNTEXT		_T("AutoKerning")
#define OPTOKEN_TEXTSELECTION		_T("TextSelect")
#define OPTOKEN_DELETESTORY			_T("TextStoryDeletion")
#define OPTOKEN_REVERSESTORYPATH	_T("ReverseStoryPath")
#define OPTOKEN_APPLYLEFTJUSTIFY	_T("ApplyLeftJustify")
#define OPTOKEN_APPLYCENTREJUSTIFY	_T("ApplyCentreJustify")
#define OPTOKEN_APPLYRIGHTJUSTIFY	_T("ApplyRightJustify")
#define OPTOKEN_APPLYFULLJUSTIFY	_T("ApplyFullJustify")
#define OPTOKEN_TEXTPASTE			_T("PasteText")
#define OPTOKEN_TOGGLEPRINTASSHAPES	_T("TogglePrintAsShapes")
#define OPTOKEN_TEXTDRAGLEFTPATHINDENT	_T("DragLeftPathIndent")
#define OPTOKEN_TEXTDRAGRIGHTPATHINDENT	_T("DragRightPathIndent")
#define OPTOKEN_TEXTDRAGLEFTNONPATHINDENT	_T("DragLeftNonPathIndent")
#define OPTOKEN_TEXTDRAGRIGHTNONPATHINDENT	_T("DragRightNonPathIndent")
#define OPTOKEN_AFFECTFONTCHANGE	_T("AffectFontChange")


/********************************************************************************************
>	class OpTextUndoable : public SelOperation

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/3/95

	Purpose:	All undoable text operations are derived from this base class. It provides 
				useful functions required by all Undoable Text Operations.

				Principally DoStartFormatOp 
							DoEndFormatOp

	SeeAlso:	OpTextUndoable::DoStartTextFormatOp
				OpTextUndoable::DoEndTextFormatOp
********************************************************************************************/

class OpTextUndoable : public SelOperation
{         

	CC_DECLARE_DYNCREATE( OpTextUndoable )    

public:
	OpTextUndoable();
	
	virtual BOOL DoStartTextOp(TextStory* pTextStory, BOOL StartBlobs = TRUE, BOOL EndBlobs = TRUE);
};  																	   


/********************************************************************************************
>	class OpCreateTextObject : public OpTextUndoable

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/03/95
	Purpose:	This operation is invoked to allow the user to insert a TextStory into the 
				document. Currently the only way the user can create a TextStory is to click
				on a spread.
********************************************************************************************/

class OpCreateTextObject : public OpTextUndoable
{

	CC_DECLARE_DYNCREATE(OpCreateTextObject);

public:
	// Construction/Destruction
	OpCreateTextObject();

	void GetOpName(String_256* OpName);

	// DoImmediate gets called after a click is received. The optional pPath parameter is a
	// path which is clicked on. It is the Do function
	void DoImmediate( Spread* pSpread, DocCoord Anchor, NodePath* pPath, ClickModifiers ClickMods, DocCoord StoryWidth = DocCoord(MinDocCoord, MinDocCoord));

	// Create a wrapping story by a drag, not on a path
	void DoDrag( Spread* pSpread, DocCoord Anchor, ClickModifiers ClickMods);
	void DragPointerMove( DocCoord PointerPos, ClickModifiers ClickMods, Spread* pSpread, BOOL bSolidDrag);
	void DragFinished( DocCoord PointerPos, ClickModifiers ClickMods, Spread* pSpread, BOOL Success, BOOL bSolidDrag);
	void RenderDragBlobs(DocRect EditPathBBox, Spread* pSpread, BOOL bSolidDrag);

protected:
	DocCoord m_CurrentOtherEnd;
	DocCoord m_AnchorPoint;
	Spread* m_pStartSpread;
};


/********************************************************************************************
>	class OpTextFormat: public OpTextUndoable

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/7/93
	Purpose:	This class implements many of the 'operations' which will cause the TextStory
				to be reformated.

				A seperate Do function is provided for each of these ops
********************************************************************************************/

class OpTextFormat: public OpTextUndoable
{         
	CC_DECLARE_DYNCREATE( OpTextFormat )    

public:
	OpTextFormat();
	
	// Op has no OpDescriptor								
	
	enum InsertStatus {INSERT, OVER};					 // Insert or overwrite
	enum DeleteType {DEL, BACKSPACE}; 					 // Same as keys 

	void DoInsertChar(WCHAR Ch, OpTextFormat::InsertStatus InsStatus);
	BOOL DoInsertCharHelper(WCHAR Ch);
	void DoDeleteChar(OpTextFormat::DeleteType DelTyp);				 
	void DoSwapCase();
	BOOL DoReturn(OpTextFormat::InsertStatus InsStatus);

	// One GetOpName fn for all flavours of this op
	void GetOpName(String_256* OpName);


private:
	BOOL DoDeleteSelection(TextStory* pTextStory, BOOL ReattachAttributes, BOOL* AllowOpRefused = NULL);
};



/********************************************************************************************
>	class OpFitTextToCurve : public OpTextUndoable

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/03/95
	Purpose:	This class is the operation used to fit a line of text to a path
********************************************************************************************/

class OpFitTextToCurve: public OpTextUndoable	  // Derived from this base class by Simon
{         

	CC_DECLARE_DYNCREATE( OpFitTextToCurve )    

public:
	OpFitTextToCurve();								
	static BOOL		Init();				
	static OpState	GetState(String_256*, OpDescriptor*);		
	void			Do(OpDescriptor*);	

private:
	static BOOL FindPathAndText(NodeRenderableInk** pPath, TextStory** pStory, UndoableOperation* pOp, UINT32* ID);
};


/********************************************************************************************
>	class OpTextKern : public OpTextUndoable

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/03/95
	Purpose:	This Operation applies a kern to the FocusStory
********************************************************************************************/

class OpTextKern: public OpTextUndoable	  
{         

	CC_DECLARE_DYNCREATE( OpTextKern )    

public:
	OpTextKern();								
	static BOOL		Init();				
	static OpState	GetState(String_256*, OpDescriptor*);		
	virtual void	DoWithParam(OpDescriptor*, OpParam* pParam);

	virtual void	PerformMergeProcessing();

private:
	// Routine to insert a Kern at the specified position in the tree, handles kern merging
	BOOL DoInsertKernWithMerge(VisibleTextNode* TTContextNode, DocCoord& Value);	
};

/********************************************************************************************
>	class OpTextAuoKern : public OpTextUndoable

	Author:		Jonathan_Payne (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/10/2000
	Purpose:	This Operation applies autokern to the FocusStory
********************************************************************************************/

class OpTextAutoKern: public OpTextUndoable	  
{         
	CC_DECLARE_DYNCREATE( OpTextAutoKern )

public:
	OpTextAutoKern();
	static BOOL		Init();				
	static OpState	GetState(String_256*, OpDescriptor*);		
	virtual void	DoWithParam(OpDescriptor*, OpParam* pParam);
};


/********************************************************************************************
>	class OpTextPaste : public OpPaste

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/05/95
	Purpose:	An undoable operation to paste the contents of the clipboard into a
				text story
********************************************************************************************/

class OpTextPaste: public OpPaste
{
	CC_DECLARE_DYNCREATE( OpTextPaste )    

	public:
		OpTextPaste();								
		static BOOL 	Init();
		static OpState	GetState(String_256*, OpDescriptor*);		
		void Do(OpDescriptor*);

	protected:
		BOOL DoPasteTextHelper();
		BOOL DoPasteText(TextStory *pPasteStory, TextStory *pFocusStory, Range* pPasteRange);
};


/********************************************************************************************
>	class OpTogglePrintTextAsShapes : public Operation

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/05/95
	Purpose:	This operation toggles the state of the "print text as shapes flag" in 
				selected TextStories
********************************************************************************************/

class OpTogglePrintTextAsShapes: public Operation	  
{         
	CC_DECLARE_DYNCREATE( OpTogglePrintTextAsShapes )    

public:
	OpTogglePrintTextAsShapes() {};								
	static BOOL		Init();				
	static OpState	GetState(String_256*, OpDescriptor*);		
	virtual void	Do(OpDescriptor*);
};



/********************************************************************************************
>	class OpDeleteTextStory : public SelOperation

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/04/95
	Purpose:	This operation should be used to hide a particular TextStory
********************************************************************************************/

class OpDeleteTextStory: public SelOperation	  
{         
	CC_DECLARE_DYNCREATE( OpDeleteTextStory )    

public:
	OpDeleteTextStory() {};								
	static BOOL		Init();				
	static OpState	GetState(String_256*, OpDescriptor*);		
	virtual void	DoWithParam(OpDescriptor*, OpParam* pParam);

	static BOOL DoActions(UndoableOperation*, TextStory*);

	static BOOL RemoveEmptyFocusStory(UndoableOperation* pOp = NULL);

	void PerformMergeProcessing();
};


/********************************************************************************************
>	class OpReverseStoryPath : public OpTextUndoable

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/04/95
	Purpose:	This operation should be used to hide a particular TextStory
********************************************************************************************/

class OpReverseStoryPath: public OpTextUndoable	  
{         
	CC_DECLARE_DYNCREATE( OpReverseStoryPath )    

public:
	OpReverseStoryPath() {};								
	static BOOL		Init();				
	static OpState	GetState(String_256*, OpDescriptor*);		
	virtual void	Do(OpDescriptor*);
};


/********************************************************************************************
>	class OpApplyJustificationToStory : public Operation

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/04/95
	Purpose:	This operation is the base class of the operations that sit on the TextStory
				pop-up menu
********************************************************************************************/

class OpApplyJustificationToStory: public Operation	  
{         
	CC_DECLARE_DYNCREATE( OpApplyJustificationToStory )    

public:
	OpApplyJustificationToStory() {};								
	static BOOL		Init();				
	static OpState	GetState(String_256*, OpDescriptor*);		
};


/********************************************************************************************
>	class OpApplyLeftJustifyToStory : public OpApplyJustificationToStory

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/04/95
	Purpose:	This operation is the left justify option on the TextStory pop-up menu.
********************************************************************************************/

class OpApplyLeftJustifyToStory: public OpApplyJustificationToStory	  
{         
	CC_DECLARE_DYNCREATE( OpApplyLeftJustifyToStory )    

public:
	OpApplyLeftJustifyToStory() {};								
	virtual void	Do(OpDescriptor*);
};


/********************************************************************************************
>	class OpApplyCentreJustifyToStory : public OpApplyJustificationToStory

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/04/95
	Purpose:	This operation is the centre justify option on the TextStory pop-up menu.
********************************************************************************************/

class OpApplyCentreJustifyToStory: public OpApplyJustificationToStory	  
{         
	CC_DECLARE_DYNCREATE( OpApplyCentreJustifyToStory )    

public:
	OpApplyCentreJustifyToStory() {};								
	virtual void	Do(OpDescriptor*);
};


/********************************************************************************************
>	class OpApplyRightJustifyToStory : public OpApplyJustificationToStory

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/04/95
	Purpose:	This operation is the right justify option on the TextStory pop-up menu.
********************************************************************************************/

class OpApplyRightJustifyToStory: public OpApplyJustificationToStory	  
{         
	CC_DECLARE_DYNCREATE( OpApplyRightJustifyToStory )    

public:
	OpApplyRightJustifyToStory() {};								
	virtual void	Do(OpDescriptor*);
};


/********************************************************************************************
>	class OpApplyFullJustifyToStory : public OpApplyJustificationToStory

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/04/95
	Purpose:	This operation is the full justify option on the TextStory pop-up menu.
********************************************************************************************/

class OpApplyFullJustifyToStory: public OpApplyJustificationToStory	  
{         
	CC_DECLARE_DYNCREATE( OpApplyFullJustifyToStory )    

public:
	OpApplyFullJustifyToStory() {};								
	virtual void	Do(OpDescriptor*);
};


/********************************************************************************************
>	class PrePostTextAction : public Action

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/03/95
	Purpose:	This action will be generated at the start and end of an OpTextUndoable 
				operation. It will perform the following tasks

				Restores the Caret position
				
				Calls FormatAndChild on the Text story on which it operates so that text 
				stories get formated when we undo/redo.

				The action toggles between being active and not active so that it only
				gets executed when it is a last executed action. 
********************************************************************************************/

class PrePostTextAction : public Action
{

CC_DECLARE_DYNCREATE(PrePostTextAction)

public:
	PrePostTextAction();
	~PrePostTextAction();
	virtual ActionCode Execute();
	static ActionCode Init( Operation* pOp,
							ActionList* pActionList,
							TextStory* pStory,
							BOOL ThisActionReformats,
							BOOL ToTail = FALSE);
	static BOOL DoFormatStory(UndoableOperation* pUndoOp, TextStory* pStory,
		BOOL ToTail = FALSE);

	// DMc inserted
	BOOL DoIReformat() { return HasEffect; }

protected:
	BOOL                HasEffect;
	VisibleTextNode*    CaretContextNode; // Restores the caret relative to this node
	AttachNodeDirection CaretAttachDir;   // direction in which to attach caret to context node
	TextStory*          pTextStoryToReformat;
	BOOL				m_bToTail;		  // DMc - tells the action whether to put itself
										  // at the head or the tail of the action
};


/********************************************************************************************
>	class PositionCaretAction : public Action

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/04/95
	Purpose:	This action is used to position a caret within a TextStory.  Its execute
				function inserts another PositionCaretAction into the undo record to return
				the caret to its current position and then moves it to the stored position
********************************************************************************************/

class PositionCaretAction : public Action
{

CC_DECLARE_DYNCREATE(PositionCaretAction)

public:
	PositionCaretAction();
	~PositionCaretAction();
	virtual ActionCode Execute();
	static ActionCode Init( Operation* pOp,
							ActionList* pActionList,
							TextStory* pStory,
							BaseTextClass* pAttachNode,
							AttachNodeDirection Direction);
	static BOOL DoStoreCaretPosition( Operation* pOp, TextStory* pStory);

protected:
	static BOOL GetCaretNeighbour(TextStory* pStory, BaseTextClass** pNeighbour, AttachNodeDirection * pDirection); 

// Member variables stored with each action
	TextStory*          pTextStory;
	BaseTextClass*      pNearNode;
	AttachNodeDirection AttachDirection;
}; 

/********************************************************************************************

>	class ToggleStoryAutoKerning : public Action

	Author:		Jonathan_Payne (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/10/2000
	Purpose:	Toggle a story's auto kerning value

********************************************************************************************/
class ToggleAutoKerningAction : public Action
{
	CC_DECLARE_DYNCREATE(ToggleAutoKerningAction)

public:
	ToggleAutoKerningAction();
	virtual ActionCode Execute();
	static ActionCode Init( Operation* pOp,
							ActionList* pActionList,
							TextStory* pStory,
							BOOL NewIsAutoKerning);

protected:
	TextStory*	pTextStory;
	bool		OldIsAutoKerning;
};


/********************************************************************************************
>	class VerticalInsetStore

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/11/95
	Purpose:	Class grouping the data for a vertical insert position.  The story and the
				inset.  As the cursor moved vertically it should keep its position along
				the line.  The OpTextCaret remembers this across each caret move. 
********************************************************************************************/
const INT32 VERTICALMOVEINSET_NONE = 0xf0000000;
class VerticalInsetStore
{
public:
	VerticalInsetStore() {HoldingInset = VERTICALMOVEINSET_NONE; LastInset = VERTICALMOVEINSET_NONE;};

	INT32 HoldingInset;
	INT32 LastInset;
};


/********************************************************************************************
>	class OpTextCaret: public Operation

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/3/95
	Purpose:	OpTextCaret groups together all caret moving ops. 
********************************************************************************************/

class OpTextCaret: public Operation
{
	CC_DECLARE_DYNCREATE(OpTextCaret)

public:
	OpTextCaret();
	void DoMoveCaretLeft(BOOL WithSelect, BOOL AWord);	// Move the caret left
	void DoMoveCaretRight(BOOL WithSelect, BOOL AWord);	// Move the caret right
	void DoMoveCaretUp(BOOL WithSelect);				// Move the caret up
	void DoMoveCaretDown(BOOL WithSelect);				// Move the caret down
	void DoMoveCaretHome(BOOL WithSelect, BOOL ToStoryStart);	// Move the caret to the start of the line
	void DoMoveCaretEnd(BOOL WithSelect, BOOL ToStoryEnd);	 	// Move the caret to the end of the line
	void DoSelectWordAtCaret();

	static VisibleTextNode* GetLineStartCharacter(VisibleTextNode* pCharacter);
	static VisibleTextNode* GetLineEndCharacter(VisibleTextNode* pCharacter);

	static void SelectStoryCharacters(VisibleTextNode*, VisibleTextNode*, TextStory*, CaretNode*);

protected:
	void MoveCaretToSelectionEnd(BOOL MoveToLeft);
	void CompleteOperation();
	VisibleTextNode* GetCharacterLineChange(VisibleTextNode* pCharacter,
											BOOL ToLeftOfChar,
											BOOL* NewToLeft,
											BOOL MoveUp);

	// Common code for cursor-key caret movement
	BOOL PreCaretMove();
	
	// Data members
	TextStory* pStory;				// Pointer to focus story
	CaretNode* pCaret;				// Pointer to caret with in.
	VisibleTextNode* pOtherSelEnd;	// Pointer to the other end of the selection (caret is at one end)
	BOOL SelToRightOfCaret;

	static VerticalInsetStore VertInset;
	VerticalInsetStore PreviousVerticalInset;
};


/********************************************************************************************
>	class OpTextSelection : public Operation

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/04/95
	Purpose:	This is the operation that positions the caret after a click, and then does
				a drag to select characters.  It also provides other functions for selecting
				groups of characters
********************************************************************************************/

class OpTextSelection : public Operation
{

CC_DECLARE_DYNCREATE(OpTextSelection);

public:
	enum SelectionPos {TO_LEFT, TO_RIGHT, NOT_NEAR};
	
	// Construction/Destruction
	OpTextSelection();

	// Call this to position the caret and start region selection
	BOOL DoDrag( DocCoord Anchor, Spread* pSpread, ClickModifiers ClickMods, Node* ClickNode);
	virtual BOOL SnappingDrag() { return FALSE; }
	
	// Virtual functions needed for the dragging operations to work
	virtual void DragPointerMove( DocCoord PointerPos, ClickModifiers ClickMods, Spread* pSpread, BOOL bSolidDrag);
	virtual void DragFinished( DocCoord PointerPos, ClickModifiers ClickMods, Spread* pSpread, BOOL Success, BOOL bSolidDrag);

	// These functions required for the OpDescriptor class
	static BOOL Declare();
	static OpState GetState(String_256* Description, OpDescriptor*);
	static BOOL Init();

	// Non-drag ways of selecting text
	BOOL DoSelectAllText(TextStory* pStory);
	BOOL DoSelectLineText();

protected:
	// Member functions
	BOOL InitialCaretPosition(VisibleTextNode*);
	BOOL ExtendSelection(VisibleTextNode*, DocCoord ClickPoint);
	BOOL SelectLine();
	SelectionPos IsClickToLeftHalfOfChar(AbstractTextChar* pClickChar, DocCoord ClickPoint, BOOL MustChoose);

	BOOL GetNewSelEndsOnPath(DocCoord Point, VisibleTextNode** pSelStart, VisibleTextNode** pSelEnd, BOOL*);
	BOOL GetNewSelEndsInStory(DocCoord Point, VisibleTextNode** pSelStart, VisibleTextNode** pSelEnd);
	BOOL GetNewSelBoundsChars(BOOL, BOOL, VisibleTextNode*, VisibleTextNode**, VisibleTextNode**);

	// Member variables
	Spread* StartSpread;
	DocCoord StartPoint;
	TextStory* pSelectionStory;	
	CaretNode* pCaret;
};


/********************************************************************************************
>	class StorePathIndentAction : public Action

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/12/95
	Purpose:	This action is used to store a text stories path indent value.
********************************************************************************************/

class StorePathIndentAction : public Action
{
	CC_DECLARE_DYNCREATE(StorePathIndentAction)
	
public:
	StorePathIndentAction();
	~StorePathIndentAction();
	virtual ActionCode Execute();
	static ActionCode Init( Operation* pOp, ActionList* pActionList, TextStory* pStory, MILLIPOINT LeftIndent, MILLIPOINT RightIndent, MILLIPOINT StoryWidth);
	static BOOL DoStoreIndent( Operation* pOp, ActionList* pActionList, TextStory* pStory);

protected:
// Member variables stored with each action
	TextStory* mpTextStory;
	MILLIPOINT mLeftIndentLength;
	MILLIPOINT mRightIndentLength;
	MILLIPOINT mStoryWidth;
}; 


/********************************************************************************************
>	class OpDragStoryIndent : public OpTextUndoable

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/12/95
	Purpose:	This is the operation that is used to drag the indent position along the path
				the text story is on
********************************************************************************************/

class OpDragStoryIndent : public OpTextUndoable
{
	CC_DECLARE_DYNAMIC(OpDragStoryIndent);

public:
		// Construction/Destruction
	OpDragStoryIndent();
	~OpDragStoryIndent();

	// Call this to start the drag going
	BOOL DoDrag( DocCoord Anchor, Spread* pSpread, ClickModifiers ClickMods, TextStory* pStory);
	
	// Virtual functions needed for the dragging operations to work
	virtual void DragPointerMove( DocCoord PointerPos, ClickModifiers ClickMods, Spread* pSpread, BOOL bSolidDrag);
	virtual void DragFinished( DocCoord PointerPos, ClickModifiers ClickMods, Spread* pSpread, BOOL Success, BOOL bSolidDrag);
	virtual void RenderDragBlobs( DocRect, Spread*, BOOL bSolidDrag );

	// These functions required for the OpDescriptor class
	static OpState GetState(String_256* Description, OpDescriptor*);

protected:
	// Functions that the derived classes must provide
	virtual DocCoord GetOtherIndent() = 0;
	virtual DocCoord GetThisIndent() = 0;
	virtual BOOL DoIndentChange() = 0; 
	virtual DocCoord InternalConstrain(DocCoord Current, ClickModifiers ClickMods) = 0;
	virtual BOOL SetCurrentPos(DocCoord MousePos, ClickModifiers ClickMods) = 0;

protected:
	// Member functions
	void RenderDraggingBlobs( DocRect, Spread* );
	static BOOL GetLeftHandLength(DocCoord Coord, NodePath* pPath, MILLIPOINT* pResult);
	static BOOL GetRightHandLength(DocCoord Coord, NodePath* pPath, MILLIPOINT* pResult);
	BOOL GetLeftHandLength(MILLIPOINT* pResult);
	BOOL GetRightHandLength(MILLIPOINT* pResult);

	// Member variables
	DocCoord mStartPoint;
	DocCoord mCurrentBlobPos;
	Spread* mpStartSpread;
	TextStory* mpDragStory;
	NodePath* mpStoryPath;
};


/********************************************************************************************
>	class OpDragStoryPathIndent : public OpDragStoryIndent

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/8/96
	Purpose:	Base class for dragging indents for stories on a path
********************************************************************************************/
class OpDragStoryPathIndent : public OpDragStoryIndent
{
	CC_DECLARE_DYNAMIC(OpDragStoryPathIndent);

public:
	// Construction/Destruction
	OpDragStoryPathIndent() {};
	~OpDragStoryPathIndent() {};

protected:
	virtual BOOL SetCurrentPos(DocCoord MousePos, ClickModifiers ClickMods);

};


/********************************************************************************************
>	class OpDragStoryPathLeftIndent : public OpDragStoryPathIndent

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/8/96
	Purpose:	This is the operation that is used to drag the left hand path indent
********************************************************************************************/
class OpDragStoryPathLeftIndent : public OpDragStoryPathIndent
{
	CC_DECLARE_DYNCREATE(OpDragStoryPathLeftIndent);

public:
	// Construction/Destruction
	OpDragStoryPathLeftIndent() {};
	~OpDragStoryPathLeftIndent() {};

	static BOOL Init();

protected:
	virtual DocCoord InternalConstrain(DocCoord Current, ClickModifiers ClickMods);

protected:
	virtual DocCoord GetOtherIndent();
	virtual DocCoord GetThisIndent();
	virtual BOOL DoIndentChange();
};


/********************************************************************************************
>	class OpDragStoryPathRightIndent : public OpDragStoryPathIndent

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/8/96
	Purpose:	This is the operation that is used to drag the right hand path indent
********************************************************************************************/
class OpDragStoryPathRightIndent : public OpDragStoryPathIndent
{
	CC_DECLARE_DYNCREATE(OpDragStoryPathRightIndent);

public:
	// Construction/Destruction
	OpDragStoryPathRightIndent() {};
	~OpDragStoryPathRightIndent() {};

	static BOOL Init();

protected:
	virtual DocCoord InternalConstrain(DocCoord Current, ClickModifiers ClickMods);

protected:
	virtual DocCoord GetOtherIndent();
	virtual DocCoord GetThisIndent();
	virtual BOOL DoIndentChange();
};



/********************************************************************************************
>	class OpDragStoryNonPathIndent : public OpDragStoryIndent

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/8/96
	Purpose:	Base class for dragging indents for stories not on a path
********************************************************************************************/
class OpDragStoryNonPathIndent : public OpDragStoryIndent
{
	CC_DECLARE_DYNAMIC(OpDragStoryNonPathIndent);

public:
	// Construction/Destruction
	OpDragStoryNonPathIndent() {};
	~OpDragStoryNonPathIndent() {};

protected:
	virtual DocCoord InternalConstrain(DocCoord Current, ClickModifiers ClickMods);
	virtual BOOL SetCurrentPos(DocCoord MousePos, ClickModifiers ClickMods);
};


/********************************************************************************************
>	class OpDragStoryNonPathLeftIndent : public OpDragStoryNonPathIndent

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/8/96
	Purpose:	This is the operation that is used to drag the left hand path indent
********************************************************************************************/
class OpDragStoryNonPathLeftIndent : public OpDragStoryNonPathIndent
{
	CC_DECLARE_DYNCREATE(OpDragStoryNonPathLeftIndent);

public:
	// Construction/Destruction
	OpDragStoryNonPathLeftIndent() {};
	~OpDragStoryNonPathLeftIndent() {};

	static BOOL Init();

protected:
	virtual DocCoord GetOtherIndent();
	virtual DocCoord GetThisIndent();
	virtual BOOL DoIndentChange();
};


/********************************************************************************************
>	class OpDragStoryNonPathRightIndent : public OpDragStoryNonPathIndent

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/8/96
	Purpose:	This is the operation that is used to drag the right hand path indent
********************************************************************************************/
class OpDragStoryNonPathRightIndent : public OpDragStoryNonPathIndent
{
	CC_DECLARE_DYNCREATE(OpDragStoryNonPathRightIndent);

public:
	// Construction/Destruction
	OpDragStoryNonPathRightIndent() {};
	~OpDragStoryNonPathRightIndent() {};

	static BOOL Init();

protected:
	virtual DocCoord GetOtherIndent();
	virtual DocCoord GetThisIndent();
	virtual BOOL DoIndentChange();
};




/********************************************************************************************
	class CCAPI OpAffectFontChange : public Operation

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/01/96
	Purpose:	An operation to scan all objects in all documents, calling a user defined
				function to perform a none undoable operation on them.
********************************************************************************************/

class CCAPI OpApplyGlobalAffect : public Operation
{
CC_DECLARE_DYNCREATE( OpApplyGlobalAffect );

protected:
	void 			DoAffectChange(ObjChangeParam* pObjChange, Document* pSearchDoc=NULL);
	virtual void 	AffectNode(Document*, Node*, ObjChangeParam*);
};


/********************************************************************************************
	class CCAPI OpAffectFontChange : public OpApplyGlobalAffect

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/01/96
	Purpose:	An operation to update all text stories in all documents
********************************************************************************************/

class CCAPI OpAffectFontChange : public OpApplyGlobalAffect
{         
CC_DECLARE_DYNCREATE( OpAffectFontChange );

public:
	static BOOL		Init();				
	static OpState	GetState(String_256*, OpDescriptor*);
	void			DoAffectFontChange();

protected:
	virtual void 	AffectNode(Document*, Node*, ObjChangeParam*);
};  


#endif 		// INC_TEXTOPS
