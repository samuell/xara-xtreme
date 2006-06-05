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
// The base class for all the transformation operations

/*
 * */ 


#ifndef INC_TRANSOP
#define INC_TRANSOP         

#include "binds.h"
#include "undoop.h"

// Karim MacDonald 14/10/1999
// we don't hold a pointer to a SelectorTool any more - it's now a pointer to a DragTool.
// class SelectorTool;
class DragTool;

/********************************************************************************************

<	struct TransformData

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/7/94
	Purpose:	Holds the flags and additional data for the transform operations that are
				started by the Selector Tool.
				MonoOn
					DocCoord CentreOfTrans;	// The centre of the transform
					INT32 StartBlob;			// The blob that the transform was started from
					BOOL LockAspect;		// Should the aspect ratio be locked
					BOOL LeaveCopy;			// Should we leave a copy of the selection
					BOOL ScaleLines;		// Should line widths be scaled as well
					BOOL TransFills;		// Should fills be transformed as well
					Range* pRange;			// range to transform; if NULL, use the selection
				MonoOff


********************************************************************************************/

struct TransformData
{
	// The Centre of Transformation and which blob was clicked on (0 for none)
	DocCoord CentreOfTrans;
	INT32 	 StartBlob;
	Range*	 pRange;			// range to transform; if NULL, use the selection

	// Various flags that effect the transform
	UINT32 LockAspect : 1;
	UINT32 LeaveCopy  : 1;
	UINT32 ScaleLines : 1;
	UINT32 TransFills : 1;
};



/********************************************************************************************

<	struct TransformBoundingData

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/8/94
	Purpose:	This structure holds information about the position of the selection as
				well as the current scale and angles
				MonoOn
					// Position and size
					INT32	x;
					INT32	y;
					INT32	Width;
					INT32	Height;

					// Scale
					FIXED16	XScale;
					FIXED16	YScale;

					// Rotation angles
					ANGLE	Rotation;
					ANGLE	Shear;
				MonoOff

********************************************************************************************/

struct TransformBoundingData
{
	// Position and size
	INT32	x;
	INT32	y;
	INT32	Width;
	INT32	Height;

	// Scale
	FIXED16	XScale;
	FIXED16	YScale;

	// Rotation angles
	ANGLE	Rotation;
	ANGLE	Shear;

	// Bools giving current state of control flags
	UINT32	ScaleLines			: 1;
	UINT32	LeaveCopy			: 1;
	UINT32	TransFills			: 1;

	// Bools to show which of these params have changed
	UINT32	XYChanged			: 1;
	UINT32	WHChanged			: 1;
	UINT32	ScaleChanged		: 1;
	UINT32	RotateChanged		: 1;
	UINT32	ShearChanged		: 1;
	UINT32	ScaleLinesChanged	: 1;
	UINT32	LeaveCopyChanged	: 1;
	UINT32	TransFillsChanged	: 1;

};



/********************************************************************************************

>	class TransOperation : public UndoableOperation

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com> / Rik
	Created:	15/2/94
	Purpose:	The base class for all the Transform Operations. This class should provide
				most of the functionality for all the transform operations such as handling
				the eored drag events etc. At the end of the day it will transform the
				selection is some way (depending on the derived class)

********************************************************************************************/

class CCAPI TransOperation : public UndoableOperation
{
	CC_DECLARE_DYNCREATE(TransOperation)

public: 
	// Constructor / destructor things
  	TransOperation(); 
	virtual ~TransOperation(); 
																			   	
	// Function to call at the start of a transOperation and at the end
 	BOOL DoStartTransOp(BOOL RecordSelTwice, Node* NodeToTransform = NULL,
 						Range* RangeToTransform = NULL);
	virtual void End(); 

	virtual Matrix GetCurrentMatrix() { return Transform; };
//	Node* GetNode() { return TransformNode; }
	Range* GetTransformRange() { return m_pTransformRange; }

	// Entry point to start an instant version of the transform
	virtual void DoWithParam(OpDescriptor*, OpParam* pOpParam);

 	// Entry point to start an interactive version of the transform
	//-----------------------------------------------------------------
	// Karim MacDonald 14/10/1999
	// 
	//	Previous use of SelectorTool* in the prototype below replaced
	//	with DragTool*, to allow for changes to TransOperation
	//	involving pSelTool.
	//-----------------------------------------------------------------
	virtual void DragStarted(TransformData*, DragTool*, TransformBoundingData*,
							 DocCoord, Spread*, ClickModifiers,
							 DocCoord ClickOffset = DocCoord(0, 0),
							 Node* NodeToTrans = NULL,
							 DragType dragtype = DRAGTYPE_AUTOSCROLL,
							 BOOL bSolidDrag = FALSE);

	// Functions to handle a drag
	virtual BOOL ShouldPointerBeOffset();
	virtual void DragPointerMove(DocCoord, ClickModifiers, Spread*, BOOL bSolidDrag);
	virtual void DragPointerIdle(DocCoord, ClickModifiers, Spread*, BOOL bSolidDrag);
	virtual void DragFinished(DocCoord, ClickModifiers, Spread*, BOOL, BOOL bSolidDrag);
	virtual BOOL DragKeyPress(KeyPress* pKeyPress, BOOL bSolidDrag);
	virtual void DragModeChanged(BOOL bSolidDrag);

	//Graham: Functions to handle clicks while dragging
	virtual void OnClickWhileDragging(OilCoord PointerPos, ClickType Click, ClickModifiers Mods, BOOL bSolidDrag);
	BOOL DragCopyAction(BOOL bSolidDrag);
	BOOL DropCopy(BOOL bSolidDrag);
	BOOL SetLeaveCopy();

	// Virtual rendering functions
	virtual void RenderDragBlobs(DocRect ClipRect, Spread *ParentSpread, BOOL bSolidDrag);

	// Function that actually does the transforming and builds undo
	BOOL CompleteTransformation();

	// Function that applies the matrix to the OriginalBounds to produce new ones
	void ComputeNewBounds();

	//Set preferences
	static BOOL DeclarePrefs();

	// These functions required for the OpDescriptor class
	static BOOL Declare();
	static OpState GetState(String_256* Description, OpDescriptor*);

	//Graham 30/9/96: A list of nodes that we need to select at the end of the operation
	//Not exactly elegant code this, I'm afraid.
	List lstNodesToSelect;	
	void SelectNodeAfterDrag (Node* NodeToSelect);
	void ClearNodesToSelect ();

	// Karim 21/01/2000 - see UndoableOperation base implementation.
	virtual BOOL MayChangeNodeBounds() const { return TRUE; }

protected:
	DocCoord GetStartPos();

private:
	// Functions to help at the start of an immediate transform
	virtual void InitTransformImmediate(OpParam*);

	// Functions to help with the dragging
	virtual void InitTransformOnDrag(DocCoord, ClickModifiers);
	virtual void UpdateTransformOnDrag(DocCoord, Spread*, ClickModifiers&);
	virtual void ConstrainDrag(DocCoord*);
	virtual void SetStartBlob(INT32 StartBlob);
	virtual void UpdateTransformBoundingData();
	virtual void DragInputChanged(DocCoord, ClickModifiers, Spread*);
	virtual void DragSolidChanged(DocCoord PointerPos,
								  ClickModifiers ClickMods,
								  Spread* pSpread,
								  BOOL bForceRecopy = FALSE);
	virtual BOOL CanChangeSpread();

	// function to build the appropriate transform matrix
	virtual void BuildMatrix();

	// Function to help separate out the code that builds the undo information
	BOOL LeaveCopyOfSelection();

	// Called by End()
	BOOL DoEndTransOp();
	BOOL DoSmartInvalidate();

	virtual BOOL GetStatusLineText(String_256*, Spread*, DocCoord, ClickModifiers);

	void FigureStatusText(String_256*);

	BOOL SolidDragTransform(BOOL bForceRecopy = FALSE, BOOL bForceRedraw = TRUE);
	BOOL SetTransformRange(Range* pTransformRange, Node* pTransformNode);

	BOOL TransformOverlapsOriginal();

protected:
	// Member vars
	// The transforms are always started from the SelectorTool and we need to talk to it
	// changed by Karim MacDonald 14/10/1999 - pSelTool is now a DragTool*,
	// so that we can talk to SliceTool too if we need to.
//	SelectorTool* pSelTool;
	DragTool* pSelTool;

	// The current position, size, angle etc of the selection being dragged
	TransformBoundingData BoundingData;

	// The original bounding coordinates expressed as an array of four coords
	// so that it's faster to compute the transformed bounds
	Coord OriginalBounds[4];

	DocCoord	StartPos;				// The coord where the drag started (including offset)
	DocCoord	MagStartPos;			// The magnetically snapped coord where drag started
	DocCoord	RawStartPos;			// The raw coord from the event that started the drag
	DocCoord	RawPos;					// The current drag coord
	DocCoord	LastRawPos;				// The last coord the drag was at

	DocCoord 	CentreOfTrans;
	Spread*  	StartSpread;
	Spread*  	CurrentSpread;
	DocCoord 	Offset;
	BOOL		MagneticGripPoint;		// The grip point of the drag is magnetically significant

	// If Adjust is held down we use the centre of the objects as the centre of the
	// transform. We need to note the old value of the centre of transform
	DocCoord BoundingCentre;
	DocCoord OriginalCentre;

	// The transform matrix and info about the equivalent scale factor of the transform
	Matrix 	Transform;
	Matrix	OriginalTransform;

	// Cached ptr to the global selection object
//	Range*	Selection;
	BOOL	RecordTwice;

	// TransformNode specifies the Node that is going to be transformed. When NULL the
	// entire selection will be transformed. This is specified in DoStartTransOp
//	Node* TransformNode; 
	Node* ObjectUnderCursor;

	// TransformRange allows the user to specify a range other than the selection.
//	Range* TransformRange;

	// Pointer to a local copy of the range being transformed
	Range* m_pTransformRange;

	// A pointer to a selection state object required by RestoreSelections actions. 
	// Created in DoStartSelectionOp()	
	SelectionState* SelState;

	// Flags to indicate the type of extra things we might need to be doing
	BOOL LockAspect;
	BOOL LeaveCopy;
	BOOL ScaleLines;
	BOOL TransFills;
	BOOL MouseHasMoved;
	BOOL CanScaleLines;

	//Graham: If AbortTransAtEnd is set, there will be no transformation when the drag ends
	//See TransOperation::OnClickWhileDragging
	BOOL AbortTransAtEnd;

	//Preference variable: sets what happens if the user clicks during a drag
	static INT32 ClickWhileDragFunc;

	// String resource ID of status line help text
	UINT32 StatusHelpID;

	//Graham 27/6/96: ID of help text for SHIFT-drag, CTRL-drag etc
	UINT32 StatusHelpID2;

	// David Mc 12/5/99
	// The selection when entering DoStartTransOp
	List* m_pSelList;

	// This range represents the copied and transformed nodes that are used when solid dragging
	Range* m_pDraggedRange;

	// This is the spread that contains m_pDraggedRange
	Spread* m_pDragSpread;

	BOOL m_bFirstRedraw;
	BOOL m_bShowDraggedOutlines;
	MonotonicTime m_timeIdle;
	BOOL m_bShowOriginalOutlines;
	BOOL m_bRangeCacheTransformed;
	BOOL m_bRangeFullyTransformed;

	// Members to help solid dragging work efficiently and accurately...
	Trans2DMatrix m_PrevTransform;
	DocRect m_AccuracyTestRect;

};							  
 
#endif 
	 
