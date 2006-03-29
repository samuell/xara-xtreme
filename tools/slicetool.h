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
// Header for the Slice tool implementation

#ifndef INC_SLICETOOL
#define INC_SLICETOOL

#define OPTOKEN_SLICE_DRAGBOX	_T("SliceDragBox")
#define OPTOKEN_SLICE_TRANSLATE	_T("SliceTranslate")

#include "dragtool.h"
#include "doccoord.h"
#include "bars.h"
#include "selector.h"	// to derive OpSliceDragBox from OpSelectorDragBox.
#include "ngscan.h"		// to derive CompileCollisionBoxScan from NodeScan.
#include "listitem.h"	// to derive RectList from List.
#include "list.h"		// for List.
#include "docrect.h"	// for DocRect.
#include "transop.h"	// to derive OpSliceTranslate from TransOperation.

class Cursor;
class Spread;
class SliceInfoBarOp;



/********************************************************************************************

>	class SliceTool : public DragTool

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com> / Simon Knight
	Created:	September 1999
	Purpose:	SliceTool is actually a misnomer.
				This tool is used for creating rollover buttons and bars for web pages.

********************************************************************************************/
class CCAPI SliceTool : public DragTool
{
// Give tool name in memory dumps
CC_DECLARE_MEMDUMP(SliceTool);

friend class OpSliceDragBox;
friend class OpSliceTranslate;

public:
	SliceTool();
	virtual ~SliceTool();

	// Standard calls your tool must provide
	virtual BOOL Init();
	virtual void Describe(void* InfoPtr);
	virtual UINT32 GetID() { return TOOLID_SLICETOOL; };

	// called by our infobar to send us view-change messages.
	void ViewChanged(const DocViewMsg& msg);

	// Render some tool blobs for us.
	virtual void RenderToolBlobs(Spread* pSpread, DocRect* pClipRect);

	// my tool blobs _are_ rendered on the selection, thank you very much :o)
	virtual BOOL AreToolBlobsRenderedOnSelection() { return TRUE; }


	// Some very common event handlers
	virtual void SelectChange(BOOL isSelected);
	virtual void OnClick( DocCoord, ClickType, ClickModifiers, Spread* );
	virtual BOOL OnKeyPress(KeyPress* pKey);
	virtual void OnMouseMove(DocCoord PointerPos,Spread* pSpread, ClickModifiers ClickMods);
	virtual BOOL GetStatusLineText(String_256* ptext, Spread* pSpread, DocCoord DocPos, ClickModifiers ClickMods);

	// Called when the view or selection changes.
	virtual void SelectionHasChanged();

	// Some useful static functions
	static SliceInfoBarOp* 	GetSliceInfoBarOp()  { return pSliceInfoBarOp; }
	static BOOL 			IsCurrentTool() 	 { return CurrentTool; }

protected:
	// updates the slice tool's information about the current selection.
	virtual BOOL UpdateSelectionInfo();

	// pre-, mid- and post-processing for mouse clicks.
	virtual BOOL PreProcessClick();
	virtual BOOL ProcessObjectClick();
	virtual void PostProcessClick();
	virtual NodeRenderableInk* FindPreProcessClickNode(	Spread* pSpread,
														DocCoord ClickPos,
														BOOL Interruptible = FALSE );

	// the SliceTool's mouse-click handlers.
	virtual void HandleSingleClick();
	virtual void HandleDoubleClick();
	virtual void HandleDragClick();
	virtual void HandleButtonUp();

	// to let the user tab through the selection.
	virtual void HandleTabKey(BOOL bIsShifted);

	// feedback functions.
	static void SetStatusText(UINT32 nStringID);
	static void SetStatusText(String_256* pStr);
	virtual void SetKeyDownCursor(ClickModifiers cmMods);
	virtual void FigureUserFeedback(Spread* pSpread,
									DocCoord dcPos,
									ClickModifiers cmods,
									BOOL DoSlowTests,
									String_256* pStr,
									Cursor** ppPointerShape);
	BOOL Append(String_256* pStr, UINT32 StringID);
	BOOL Append(String_256* pStr, String_256 String);
	BOOL Append(String_256* pStr, ClickModifiers cmods,
								  UINT32 SelectID,
								  UINT32 AddID,
								  UINT32 RemoveID,
								  NodeRenderableInk* pActionNode = NULL);
	BOOL Append(String_256* pStr, ClickModifiers cmods,
								  String_256 SelectTemplate,
								  String_256 AddTemplate,
								  String_256 RemoveTemplate,
								  NodeRenderableInk* pActionNode = NULL);

	BOOL IsTranslateShortcut(ClickModifiers cmods) const;
	BOOL IsClickModified(ClickModifiers cmods) const;
	BOOL IsSelectUnderClick(ClickModifiers cmods) const;
	BOOL IsSelectMemberClick(ClickModifiers cmods) const;
	BOOL IsSelectLeafClick(ClickModifiers cmods) const;

	// drag handling functions.
	void DoTranslate();					// mouse-drag transform.
	void DoDragBox();					// non-transform drag.
	void MakeSelectionValidForDrag();	// checks & may alter the selection, for dragging.

	// These provide some shorthand to the FindSimple/FindCompound hit-testing functions in
	// NodeRenderableInk.  They are "hard-wired" to use the current click position.
	NodeRenderableInk* FindFrom(NodeRenderableInk* pSimpleNode) const;

	// Forward declaration of click action code...
	enum ClickActionCode;

	// Determine what action needs to be taken in response to a click
	ClickActionCode DetermineClickAction(NodeRenderableInk** ppActionNode,
										 NodeRenderableInk* pLastClickNode,
										 NodeRenderableInk* pClickSimpleNode,
										 NodeRenderableInk* pClickCompoundNode,
										 Spread* pStartSpread,
										 DocCoord ClickStart,
										 ClickModifiers ClickMods);
	ClickActionCode CycleClickAction(NodeRenderableInk** ppActionNode,
									 NodeRenderableInk* pClickCompoundNode,
									 ClickActionCode foundAction,
									 ClickActionCode cycleAction);
	BOOL ValidateLastClickUnder(NodeRenderableInk* pLastClickNode, Spread* pStartSpread, DocCoord ClickStart);
	BOOL ValidateLastClickInside(NodeRenderableInk* pLastClickNode, NodeRenderableInk* pClickSimpleNode);

/////////////////////////////////////////////////////////////////////////////////////////////
//
// protected member variables
//

	// cursors.
	Cursor* pcNormalSliceCursor;
	Cursor* pcAdjustCursor;
	Cursor* pcUnderCursor;
	Cursor* pcInsideCursor;
	Cursor* pcUnderAdjustCursor;
	Cursor* pcInsideAdjustCursor;
	Cursor* pcLeafCursor;
	Cursor* pcLeafAdjustCursor;

	Cursor* pcALLCursor;
	Cursor* pcHorzGuideCursor;
	Cursor* pcVertGuideCursor;

	// click state variables.
	DocCoord 		ClickStart;
	Spread*  		StartSpread;
	ClickModifiers 	ClickMods;
	ClickType		TypeOfClick;

	// general selection state variables.
	Spread*		SelectionSpread;
	SelRange*	SelectRange;
	DocRect		SelectionRect;
	BOOL		m_bSliceToolSetSel;

	// the rectangle which we use to draw our tool blobs.
	DocRect		m_drBlobRect;

	// select-under state variables.
	NodeRenderableInk*	pClickSimpleNode;
	NodeRenderableInk*	pClickCompoundNode;
	NodeRenderableInk*	pLastClickNode;
	NodeRenderableInk*	pPreProcClickNode;

	// various codes, for modifying click type.
	enum ClickActionCode {
							CLICKACTION_NONE,
							CLICKACTION_SELNONE,
							CLICKACTION_SELNODE,
							CLICKACTION_SELUNDER,
							CLICKACTION_SELUNDERCYCLE,
							CLICKACTION_SELUNDERFAIL,
							CLICKACTION_SELUNDERFAIL2,
							CLICKACTION_SELINSIDE,
							CLICKACTION_SELINSIDECYCLE,
							CLICKACTION_SELINSIDEFAIL,
							CLICKACTION_SELINSIDEFAIL2,
							CLICKACTION_SELLEAF
						};

	// general description of named sets currently selected.
	enum SetSelectionState
	{
		NoSetsSelected,
		HalfSetsSelected,
		FullSetsSelected
	};

	// what named sets are currently selected.
	enum SetSelectionState m_SetSelectionState;

	// this must be a member variable, as it is accessed during
	// slice-tool drag operation.
	TransformBoundingData BoundingData;

	// called to notify the tool that a drag has finished,
	// so it can tidy up cursors etc. appropriately.
	virtual BOOL DragFinished(DragEndType det);

private:
	void DisplayStatusBarHelp(DocCoord DocPos, Spread* pSpread, ClickModifiers ClickMods);
	void GetCurrentStatusText(String_256* ptext, Spread* pSpread, DocCoord DocPos, ClickModifiers ClickMods);

	BOOL CreateCursors();						// Create  your tool's cursors in this func
	void DestroyCursors();						// Destroy your tool's cursors in this func

	static BOOL 			CurrentTool;		// Can be useful to keep a "is my tool current?" flag
	static SliceInfoBarOp*	pSliceInfoBarOp;	// Ptr to your tool's infobar

	Cursor*	pcCurrentCursor;					// The cursor your tool is currently displaying
	INT32 CurrentCursorID;						// ID of the current cursor you're displaying
	// Standard tool static vars
	static 	char* FamilyName;					// The Tools Family Name
	static 	char* ToolName;						// The Tool Name
	static 	char* Purpose;						// What the tool is for
	static 	char* Author;						// Who wrote it

	// whether we're currently ignoring selchanged messages or not.
	BOOL	m_fIgnoreSelChange;
};


//-----------------------------------------------
//-----------------------------------------------
//-----------------------------------------------
//-----------------------------------------------



/********************************************************************************************

>	class SliceInfoBarOp : public InformationBarOp

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/10/94
	Purpose:	Class for handling the tool's information bar.

********************************************************************************************/

class TemplateAttribute;

class SliceInfoBarOp : public InformationBarOp
{
	CC_DECLARE_DYNCREATE( SliceInfoBarOp )  
public:
	SliceInfoBarOp() { m_InfoBarCreated = FALSE; m_TopVisibleState = -1; m_EditingBar = FALSE; pSliceTool = NULL; };
	INT32 FindCurrentState(); // no longer static

	BOOL m_InfoBarCreated; // this refers to the info bar confusingly no the bar the info bar makes

	MsgResult 	Message(Msg* Msg);	// All messages to the info bar come through here

	void OnSelectionChanged();
	static BOOL ScanFromNodeForBarMembers(Node * pNode, String_256 &BarName, String_256 * pButtonName = NULL);

	// set the SliceInfoBarOp's SliceTool pointer.
	void SetSliceTool(SliceTool* pTool) { pSliceTool = pTool; };
	void UpdateCurrentStateGadget();


protected:
	String_256 GetNextUnusedBarName(Range * pSel, BOOL * pFromSel);
	BOOL m_EditingBar;
	INT32 m_TopVisibleState; // the state layer that the user can see - ie the one they are most likely to be refering to

	// Karim 19/10/1999 - a pointer to the slice tool which is associated with this info-bar.
	SliceTool* pSliceTool;
};
	


/********************************************************************************************

>	class SliceInfoBarOpCreate : public BarCreate

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/10/94
	Purpose:	Class for creating SliceInfoBarOps.
				Derived classes of BarCreate are used by DialogBarOp::ReadBarsFromFile()

********************************************************************************************/

class SliceInfoBarOpCreate : public BarCreate
{
public:
	DialogBarOp* Create() { return (new SliceInfoBarOp); }
};



//-----------------------------------------------
//-----------------------------------------------
//-----------------------------------------------
//-----------------------------------------------



/*******************************************************************

>	class OpSliceDragBox : public OpSelectorDragBox

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/10/1999
	Purpose:	Drag box class for the SliceTool.
				Allows the user to select a number of named sets, by dragging a box
				around some objects on the document. All objects sharing the same names
				as those inside the box will then be selected.

				NB funnily enough, this class inherits functionality from OpSelectorDragBox.

*******************************************************************/
class OpSliceDragBox : public OpSelectorDragBox
{
// allow dynamic creation of this class.
CC_DECLARE_DYNCREATE(OpSliceDragBox);

public:

	// The all important Do functions
	void StartDragBox( Spread* pSpread, DocCoord Anchor, ClickModifiers);

	// Virtual function needed for the dragging operations to work
	virtual void DragFinished(DocCoord, ClickModifiers, Spread*, BOOL, BOOL bSolidDrag);

	// These functions required for the OpDescriptor class
	static BOOL Declare();
	static OpState GetState(String_256* Description, OpDescriptor*);
};



//-----------------------------------------------
//-----------------------------------------------
//-----------------------------------------------
//-----------------------------------------------



class CCAPI RectListItem : public ListItem
{
public:
	RectListItem(DocRect source);
	DocRect GetRect();

protected:
	DocRect m_Rect;
};



//-----------------------------------------------
//-----------------------------------------------
//-----------------------------------------------
//-----------------------------------------------



/********************************************************************************************

>	class OpSliceTranslate : public TransOperation

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>, from Rik's OpTranslateTrans code.
	Created:	13/10/1999
	Purpose:	This is the Slice tool's Translate operation, which is similar in behaviour
				to OpTranslateTrans, except that we inform the user during the drag if the
				current selection's bounds overlap those of an unselected named set (button).
				When doing an immediate transform the params to DoWithParams() are :-
					Param1 - All the standard transform data
					Param2 - A pointer to a DocCoord that holds to offset translate by.

********************************************************************************************/

class OpSliceTranslate : public TransOperation
{
	// Give my name in memory dumps
	CC_DECLARE_DYNCREATE(OpSliceTranslate);

public:
	// Construction/Destruction
	OpSliceTranslate();

	// These functions required for the OpDescriptor class
	static BOOL Declare();
	virtual BOOL ShouldPointerBeOffset();

	// Entry point to start an interactive version of the transform
	virtual void DragStarted(	TransformData*,
								DragTool*,
								TransformBoundingData*,
								DocCoord,
								Spread*,
								ClickModifiers,
								DocCoord ClickOffset = DocCoord(0, 0),
								Node* NodeToTrans = NULL,
								DragType dragtype = DRAGTYPE_AUTOSCROLL );
	virtual void DragFinished(DocCoord, ClickModifiers, Spread*, BOOL, BOOL bSolidDrag);

	// render blobs on or off the page.
	// Karim MacDonald 07/02/2000; removed - see slicetool.cpp
//	virtual void RenderDragBlobs(DocRect ClipRect, Spread* pSpread);

protected:
	// Routines for checking bounding-box collisions against
	// other named sets during a drag.
	void CompileCollisionBoxList();
	BOOL TestBoundingBoxCollision(DocRect* pTestRect, Spread* pSpread);

	// snap functionality, so the selection can snap to one of our list of collision rects.
	BOOL SnapRectToBoxList(Spread* pSpread, DocRect* pDocRect, const DocCoord& PrevCoord,
																const DocCoord& CurCoord);

private:
	// Function to help at the start of an immediate transform
	virtual void InitTransformImmediate(OpParam*);

	// function to build the appropriate transform matrix
	virtual void InitTransformOnDrag(DocCoord, ClickModifiers);
	virtual void UpdateTransformOnDrag(DocCoord, Spread*, ClickModifiers&);
	virtual void BuildMatrix();
	virtual void ConstrainDrag(DocCoord*);

/////////////////////////////////////////////////////////////////////////////////////////////
//
// Member Variables
//

protected:
	// a list of bounding boxes to collision-detect against.
	// I've got a feeling this should really be a RectList or some such thing.
	List	m_lBoxes;

	// state variables, used for rendering collision rectangle blobs if
	// the dragged selection hits the bounding box of an unselected named set.
	BOOL	m_bDrawingBlobs;
	DocRect	m_DragRect;

private:
	DocCoord LastPos;
	DocCoord OriginalGridOffset;
};



#endif 		// INC_SliceTool
