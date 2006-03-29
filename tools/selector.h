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
// The selector tool and associated operations.

#ifndef INC_SELECTOR
#define INC_SELECTOR

#include "dragtool.h"
#include "transop.h"
#include "doccoord.h"
#include "blobs.h"
#include "binds.h"

#define OPTOKEN_SELECTOR_DRAGBOX		_T("SelectorDragBox")
#define OPTOKEN_SELECTOR_DRAGCENTRE		_T("SelectorDragCentre")

class OpState;			// #include "opdesc.h"
class Cursor;			// #include "csrstack.h"
class Spread;
class SelectorInfoBarOp;
class SelChangingMsg;
class DocViewMsg;
class KeyPress;


/********************************************************************************************
>	class OpSelectorDragBox : public Operation

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/7/93
	Purpose:	This operation is started when the user does a drag over the document, that
				started in empty space. A drag box will be drawn out over the document as the
				user moves the mouse about and when the drag ends, all the objects wholly
				inside the box will be selected.
********************************************************************************************/

class OpSelectorDragBox : public Operation
{
public:

	// Construction/Destruction
	OpSelectorDragBox();

	// The all important Do functions
	void StartDragBox( Spread* pSpread, DocCoord Anchor, ClickModifiers);
	virtual BOOL SnappingDrag() { return FALSE; }
	
	// Virtual functions needed for the dragging operations to work
	virtual void DragPointerMove(DocCoord, ClickModifiers, Spread*, BOOL bSolidDrag);
	virtual void DragFinished(DocCoord, ClickModifiers, Spread*, BOOL, BOOL bSolidDrag);

	// Some Render functions to will draw the EORed path
	void RenderDragBlobs(DocRect, Spread*, BOOL bSolidDrag);
		
	// These functions required for the OpDescriptor class
	static BOOL Declare();
	static OpState GetState(String_256* Description, OpDescriptor*);

private:
	
	CC_DECLARE_DYNCREATE(OpSelectorDragBox);

	// Member vars of the operation
	Spread*  StartSpread;			// The spread where the drag started
	DocCoord StartPoint;			// The point where the drag started
	DocCoord LastMousePosition;		// The last position the mouse was in
};



/********************************************************************************************
>	class OpDragRotateCentre : public Operation

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	several deadlines previous, I can assure you
	Purpose:	Allows you to drag the selector tool's centre of rotattion around
	SeeAlso:	-
********************************************************************************************/

class OpDragRotateCentre : public Operation
{
public:

	// Construction/Destruction
	OpDragRotateCentre();

	// This is called by the tool when a drag has been started.
	void StartDragCentreBlob(Spread*, const DocCoord&, ClickModifiers);
	
	// Virtual functions needed for the dragging operations to work
	virtual void DragPointerMove(DocCoord, ClickModifiers, Spread*, BOOL bSolidDrag);
	virtual void DragFinished(DocCoord, ClickModifiers, Spread*, BOOL, BOOL bSolidDrag);
	virtual BOOL DragKeyPress(KeyPress* pKey, BOOL bSolidDrag);

	// Some Render functions to will draw the EORed path
	void RenderDragBlobs(DocRect, Spread*, BOOL bSolidDrag);
		
	// These functions required for the OpDescriptor class
	static BOOL Declare();
	static OpState GetState(String_256* Description, OpDescriptor*);

	// This generates a little rectangle enclosing the rotation centre blob.
	static DocRect CalcBlobClipRect(const DocCoord& dcPos);

private:

	CC_DECLARE_DYNCREATE(OpDragRotateCentre);

	// Helper functions.
	void GetNearestBlob(const DocCoord& dcPos, DocCoord* pPos, INT32* pBlob) const;

	// Data members.
	Spread*		  m_pStartSpread;
	DocCoord	  m_dcFirstPos;
	DocCoord	  m_dcLastPos;
	DocCoord	  m_dcLastMousePos;
};



/********************************************************************************************
>	class SelectorTool : public DragTool

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/7/93
	Purpose:	The Selector tool is used to change the selection, edit objects and
				transform them.
********************************************************************************************/

/********************************************************************************************

>	class CCAPI SelectorTool : public DragTool

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/10/1999
	Purpose:	The Selector tool now derives from the DragTool, so that TransOperation
				may refer to a general DragTool when an object drag is in progress.
				At a later date, drag functionality may be pulled up into DragTool,
				to allow future tools to implement dragging of objects more easily.

********************************************************************************************/
class CCAPI SelectorTool : public DragTool
{

////////////////////////////////////////////////////////////////////////////////////////////
// Tool interface

public:

	// Creation & destruction.
	SelectorTool();
	virtual ~SelectorTool();
	BOOL Init();

	// Attributes.
	void Describe(void* InfoPtr);
	UINT32 GetID() { return TOOLID_SELECTOR; };
	
	// Event handlers etc.
	virtual void SelectChange(BOOL);
	virtual void OnClick(DocCoord, ClickType, ClickModifiers, Spread*);
	virtual void OnMouseMove(DocCoord, Spread*, ClickModifiers);
	virtual BOOL OnKeyPress(KeyPress* pKey);
	virtual BOOL OnIdle();
	virtual void RenderToolBlobs(Spread* pSpread, DocRect* pClipRect);

	// Function that allows the transform to tell the tool how things are going
	void DragMove(TransformBoundingData* pBoundingData);
	
	// This is called to notify the tool when a drag has finished so it can tidy up
	// appropriately.
	virtual BOOL DragFinished(DragEndType det);

	// This function can can called by Operations to turn idle processing on or off.
	// They need to do this to prevent the tool overwriting status bar text etc.
	static void AllowIdleWork(BOOL fIsAllowed);
	
	static BOOL IsSelectorCaching(){ return fSelectorIsCaching;};

	// This function returns TRUE if the given floating-point value exceeds the range
	// that FIXED16 numbers can represent.
	static BOOL BeyondFixedRange(double fpFixedVal);

	virtual BOOL AreToolBlobsRenderedOnSelection() {return TRUE;}

	// CGS:  support functions for profile dialogs - see NOTES where variable are defined

	static BOOL GetNormalClick_CheckProfileDialog () { return (bNormalClickCheckProfileDialog); }
	static void SetNormalClick_CheckProfileDialog (BOOL newVal) { bNormalClickCheckProfileDialog = newVal; }

private:

	BOOL fAllowIdleProcessing;			// if TRUE then perform OnIdle stuff
	BOOL fMouseHasMoved;				// sort of ditto
	static BOOL fSelectorIsCaching;		// set during a cache - avoids bar redraws

	// More attributes.
	static TCHAR* FamilyName;			// The Tools Family Name
	static TCHAR* ToolName;				// The Tool Name
	static TCHAR* Purpose;				// What the tool is for
	static TCHAR* Author;				// Who wrote it

	static INT32 CursorStackID;			// My cursor (stack position) handle
//	static INT32 pCursor;				// Pointer to my current cursor

	// own up in memory dumps
	CC_DECLARE_MEMDUMP(SelectorTool);


////////////////////////////////////////////////////////////////////////////////////////////
// View & selection message handling.

public:

	// Called when the view or selection changes.
	void SelectionHasChanged(BOOL DontUpdate =0);
	void AttributeHasChanged();
	void ViewChanged(const DocViewMsg& msg);
	BOOL UpdateSelectionInfo();

	// Attributes.
	Spread* GetSelectionSpread() const;
	const DocRect& GetSelectionBounds(BOOL WithNoAttrs = FALSE) const;
	const BOOL GetAreaDetails(BOOL WithNoAttrs, XLONG* pxlArea, XLONG* pxlPerim) const;

private:

	// Information about the selected object(s).
	Spread*   	SelectionSpread;
	SelRange* 	SelectRange;
	DocRect 	SelectionRect;
	DocRect		SelectionRectNoAttr;

	XLONG		xlSelectionArea;
	XLONG		xlSelectionPerim;

	// When true we will ignore selection changed messages.
	BOOL 		fIgnoreSelChange;


////////////////////////////////////////////////////////////////////////////////////////////
// Click & drag handling.

public:
	static NodeRenderableInk* FindPreProcessClickNode(Spread* pSpread,
													  DocCoord ClickPos,
													  BOOL Interruptible = FALSE);

protected:

	// Top-level mouse handlers.
	virtual BOOL PreProcessClick();
	virtual BOOL ProcessObjectClick();
	virtual void PostProcessClick();

	// Click event handlers.
	virtual void HandleSingleClick();
	virtual void HandleDoubleClick();
	virtual void HandleDragClick();
	virtual void HandleButtonUp();

	// Key handler.
	virtual void HandleTabKey(BOOL fIsShifted);

	// The IterateFlags structure provides a little control over tab-iteration of nodes.
	// It currently contains flags to indicate whether to ignore locked or invisible layers.
	struct IterateFlags
	{
		IterateFlags(	BOOL IgInvisibleLayers	= FALSE,
						BOOL IgLockedLayers		= FALSE )
		{
			IgnoreInvisibleLayers	= IgInvisibleLayers;
			IgnoreLockedLayers		= IgLockedLayers;
		}

		BOOL IgnoreInvisibleLayers	:1;
		BOOL IgnoreLockedLayers		:1;
	};

	// we need to maintain a record of the layer we started on, so we don't infinitely loop
	// around the same layers again and again.
	Layer* m_pIterStartLayer;

	// These functions are for tabbing through the selection.

	Node* IteratePreProcess(Node* pCurrent);
	NodeRenderableInk* IterateNextInk(Node* pCurrent, IterateFlags iterFlags);
	NodeRenderableInk* IteratePrevInk(Node* pCurrent, IterateFlags iterFlags);
	NodeRenderableInk* IteratePostProcessNext(	Node* pCurrent, Node* pParent,
												IterateFlags iterFlags );
	NodeRenderableInk* IteratePostProcessPrev(	Node* pCurrent, Node* pParent,
												IterateFlags iterFlags );

private:

	// Returns TRUE if the current click is on the rotation centre blob.
	BOOL IsRotateCentreClicked(DocCoord ClickStart) const;

	// Returns TRUE if the user is holding down the translation op shortcut keys.
	BOOL IsTranslateShortcut(ClickModifiers cmods) const;
	
	// Returns TRUE if the current mouse click has been "modified", eg. Adjust, Constrain etc.
	BOOL IsClickModified(ClickModifiers cmods) const;

	// This detects whether the current click means "select-under", allowing for the
	// SelectUnderLikeArtWorks preference.
	BOOL IsSelectUnderClick(ClickModifiers cmods) const;

	// Returns TRUE if the current set of click modifiers indicates "select-inside".
	BOOL IsSelectMemberClick(ClickModifiers cmods) const;

	// Returns TRUE if the current set of click modifiers indicates "select-leaf".
	BOOL IsSelectLeafClick(ClickModifiers cmods) const;

	// These provide some shorthand to the FindSimple/FindCompound hit-testing functions in
	// NodeRenderableInk.  They are "hard-wired" to use the current click position.
	NodeRenderableInk* FindFrom(NodeRenderableInk* pSimpleNode) const;

	// Like the above, but for FindInnerCompound and unrelated to the click position.
	NodeRenderableInk* EnsureInkNext(Range* range, Node* pNode) const;
	NodeRenderableInk* EnsureInkPrev(Range* range, Node* pNode) const;

	// Declaration of click action code...
	enum	ClickActionCode	{
							CLICKACTION_NONE,
							CLICKACTION_BOUNDTOPLEFT,
							CLICKACTION_BOUNDTOP,
							CLICKACTION_BOUNDTOPRIGHT,
							CLICKACTION_BOUNDLEFT,
							CLICKACTION_BOUNDRIGHT,
							CLICKACTION_BOUNDBOTTOMLEFT,
							CLICKACTION_BOUNDBOTTOM,
							CLICKACTION_BOUNDBOTTOMRIGHT,
							CLICKACTION_BOUNDTRANSFORMORIGIN,
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

	// Data members.
	DocCoord 			ClickStart;
	Spread*  			StartSpread;
	ClickModifiers 		ClickMods;
	ClickType			TypeOfClick;
	INT32 				nClickedBoundsBlob;
	BOOL				fPossibleToggleClick;
	BOOL				fIsBlobDrag;

	// These are used for the "select-under" feature, which must maintain data across clicks.
/*	NodeRenderableInk* 	pClickedNode;
	NodeRenderableInk*	pSimpleClickedNode;
	NodeRenderableInk*	pLastClickedNode;
	NodeRenderableInk*	pLastUnderNode;
	NodeRenderableInk*  pLastSimpleNode;
	NodeRenderableInk*  pLastInsideNode;
*/
	NodeRenderableInk*	pClickSimpleNode;
	NodeRenderableInk*	pClickCompoundNode;
	NodeRenderableInk*	pLastClickNode;
	NodeRenderableInk* 	pPreProcClickNode;

////////////////////////////////////////////////////////////////////////////////////////////
// Transformations (and the other drags).

public:

	// This is called when a drag has moved the rotation centre.
	void RotationCentreDragged(const DocCoord& dcNewPos);

	// This tells the selector tool to restore the rotation centre to the middle of the
	// selection bounds at the next opportunity.
	void InvalidateRotationCentre();

private:

	// This gets default settings for the transform params from the info-bar.
	void GetUserParams();

	// These initialise a mouse-drag transform.
	void DoScale();
	void DoSquash();
	void DoShear();
	void DoTranslate();
	void DoRotate();

	// These are non-transform drags.
	void DoDragBox();
	void DoDragRotateCentre();

	// These start a mouse-drag transform & an immediate (info-bar) transform, respectively.
	BOOL StartXformDrag(TransOperation* pXformDragOp, DragType dragtype);
	BOOL StartXformImmediate(const TCHAR* chOpToken, void* pvParam2);

	// Parameters for a transformation.
	TransformData 	tdParams;
	DocCoord	  	dcRotateCentre;
	BOOL			fRotateCentreIsValid;

	// A structure that contains the position, size etc of the selection
	// (ie whats in the info bar)
	TransformBoundingData BoundingData;


////////////////////////////////////////////////////////////////////////////////////////////
// Info-bar data & functions.

public:
	
	// Called by the info-bar when the user changes certain buttons.
	void SelectionBlobChange(BlobStyle ChangingBlobs);
	void BoundsButtonChange();
	void RotateButtonChange(BOOL fNewState);
	void FlipButtonChange(BOOL fIsVertical);
	void SetRotateCentre(INT32 nBlob);

	// These run keyboard transformations, ie. those started by typing in an edit-field.
	void DoTranslateImmediate(MILLIPOINT nXpos, MILLIPOINT nYpos);
	void DoScaleImmediate(MILLIPOINT nWinc, MILLIPOINT nHinc);
//	void DoScaleImmediate(MILLIPOINT nWinc, MILLIPOINT nHinc, BOOL);
	void DoScalePercentImmediate(double nWpercent, double nHpercent);
	void DoRotateImmediate(ANGLE nAngle);
	void DoShearImmediate(ANGLE nAngle);

	// A public interface onto the DoTranslate() method, so that somebody other than the
	// selector tool can tell the selector tool to drag objects around.
	void PublicDoTranslate();

	// This function switches the settings of the info-bar back to known defaults, eg.
	// rotate blobs OFF, don't leave copies etc.
	void ResetDefaults();

	// This sets the status bar text to the given string.
	static void SetStatusText(UINT32 nStringID);
	static void SetStatusText(String_256* pStr);
	static void UnCacheInfoBar(BOOL = FALSE);

	// Interogate the current tool for status line text
	virtual BOOL GetStatusLineText(String_256* ptext, Spread* pSpread,
								   DocCoord DocPos, ClickModifiers cmods);

	void FigureUserFeedback(Spread* pSpread,
							DocCoord dcMousePos,
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

private:

	// This reads preferences from the .INI file, returning TRUE if they are all there etc.
	static BOOL ReadPrefs();

	// Points to this tool's info-bar
	static SelectorInfoBarOp* pInfoBarOp;

	// This is our friend . . .
	friend class SelectorInfoBarOp;



////////////////////////////////////////////////////////////////////////////////////////////
// Blob management.

public:

	// This defines constants to indicate what kind of tool blobs the selector can
	// display.
	enum BlobType
	{
		NO_BLOBS,				// use the fShowToolBlobs flag instead of this!
		BOUNDS_BLOBS,
		ROTATE_BLOBS
	};

	// Called by the info-bar when it needs to know the settings of the blob buttons.
	BlobStyle GetBlobStyle() const;
	BlobType  GetCurrentToolBlobType() const;

	// This inflates the given rectangle by the "blob border", ie. the amount that bounds blobs
	// are offset by when drawing them around a selected object.  This function is also useful
	// in the zoom tool.
	static INT32 GetBlobBorderSize(void);
	static void InflateByBlobBorder(DocRect* pdrRect);

	static void AllowGuidelineScan (BOOL newVal = TRUE) { bGlineSAllowed = newVal; }

private:

	// These draw the appropriate blobs in the right places.
	void RenderBoundBlobs(Spread*, DocRect*, const DocRect&);
	void RenderRotateBlobs(Spread*, DocRect*, const DocRect&);
	void RenderRotateCentre(RenderRegion*, const DocCoord&);

	// These calculate the position of blobs around the selection.
	DocRect  InflatedSelRect() const;
	DocCoord GetBoundsBlobPos(INT32 nBlob) const;
	DocCoord GetSelPosNearBlob(INT32 nBlob) const;

	// These convert a mouse coordinate into the appropriate blob number, if any.
	INT32 BoundsBlobHitTest(const DocCoord& dcMousePos) const;
	static BOOL IsNearBlob(const DocCoord& dcBlobOrigin, const DocCoord& dcTestPoint);

	// This handles changing the cursor when it moves over a blob.
	void ChangeCursorAndText(INT32 nBlobID, String_256* pStr, Cursor** pPointerShape);

	// These handle changing the cursor according to which modifier keys are up or down.
	void SetKeyDownCursor(ClickModifiers cmods);
	void ResetCursorNow();

	// Func that gets the tool to render it's blobs when the selection changes, but only if the current
	// tool is not the selector tool.
	void RenderOtherToolBlobs();

	// Data members.
	static BlobManager* pBlobManager;
	BlobStyle  			bsBlobStyle;
	BlobType			eCurrentBlobs;
	BOOL 				fShowToolBlobs;

	INT32					nLastSelectedBlob;
	BOOL				fValidSelectedBlob;
	BOOL				m_bComputeAreaDetails;

	// Cursor stuff.
//	unsigned			fKeyStates;					// used as a bit-field
//	BOOL				fCursorOverBlob;
//	BOOL				fCursorOverGradFillBlob;
//	BOOL				fKeyDownCursor;

	Cursor*				pNormalCursor;
	Cursor*				pAdjustCursor;
	Cursor*				pUnderCursor;
	Cursor*				pInsideCursor;
	Cursor*				pUnderAdjustCursor;
	Cursor*				pInsideAdjustCursor;
	Cursor*				pLeafCursor;
	Cursor*				pLeafAdjustCursor;

	Cursor*				pALLCursor;
	Cursor*				pNWSECursor;
	Cursor*				pNESWCursor;
	Cursor*				pNSCursor;
	Cursor*				pWECursor;
	Cursor*				pGradFillCursor;
	Cursor*				pDragRotateCursor;
	Cursor*				pHorzGuideCursor;
	Cursor*				pVertGuideCursor;
//	Cursor*				pLastKeyDownCursor;

	// Various preferences.
	static INT32 			nBlobBorder;
	static unsigned		fBlobPref;
	static BOOL			fAllowCorelToggleClick;
	static BOOL			fSelectUnderLikeArtWorks;

	// CGS:  wo there!  when selecting another object, everything is deselected - and then just that node
	// selected.  This is a problem, since if we proceed regardless, the profile dialog closes (when it
	// possibly shouldn't have).  As such, we now check for this occurring ....
	// NOTE:  it has to be done within the selector tool cause the problem occurs internally!
	static BOOL			bNormalClickCheckProfileDialog;		// you need to check the profile dialog again buddy!

	// CGS:  wo there!  now that I've implemented selection capability for all tools, it is undesirable to allow
	// them to move guidlines about!

	static BOOL			bGlineSAllowed;

protected:
	// Preferences used by our friend InfoBarOp class
	static BOOL			fSlaveLineWidthToButton;
	static BOOL			fConsiderLineWidths;
	static BOOL			fUseScalingFix;
	static BOOL			bPageDoubleClickOpenFile;

	// These classes have special access.
	friend class OpDragRotateCentre;
};

#endif	// INC_SELECTOR
