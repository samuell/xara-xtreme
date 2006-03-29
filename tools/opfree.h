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
// Header file for the Freehand operation

#ifndef INC_OPFREE
#define INC_OPFREE

#include "doccoord.h"
//#include "selop.h"
#include "nodeblnd.h"

class OpState;
class NodePath;
class Path;
class Cursor;
struct FreeHandJoinInfo;

#define OPTOKEN_FREEHAND _T("FreeHandTool")


/********************************************************************************************

<	CursorType

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/5/94
	Comments:	Used by OpFreeHand to decide which cursor to use during its life.
				Possible cases are NORMAL_CURSOR, JOIN_CURSOR, STRAIGHTLINE_CURSOR and
				RUBOUT_CURSOR

********************************************************************************************/

typedef enum
{
	NORMAL_CURSOR,
	JOIN_CURSOR,
	STRAIGHTLINE_CURSOR,
	RUBOUT_CURSOR,
	MODIFY_CURSOR
} CursorType;


/********************************************************************************************

<	SimpleJoinType

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/3/2000
	Comments:	When we have a simple join, i.e. one end of a new path joins one end of an 
				existing path, then it can be one of the following types:
********************************************************************************************/

typedef enum
{
	JOINTYPE_NONE,
	JOINTYPE_NEWSTART_TO_OLDSTART,
	JOINTYPE_NEWSTART_TO_OLDEND,
	JOINTYPE_NEWEND_TO_OLDSTART,
	JOINTYPE_NEWEND_TO_OLDEND
} SimpleJoinType;
/********************************************************************************************

>	class OpFreeHand : public SelOperation

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/7/93
	Purpose:	This is the FreeHand Operation that builds a smoothed curve

********************************************************************************************/

class OpFreeHand : public SelOperation
{

CC_DECLARE_DYNCREATE(OpFreeHand);

public:
	// Construction/Destruction
	OpFreeHand();

	// The all important Do functions
	void DoDrag(DocCoord Anchor, Spread*, INT32, FreeHandJoinInfo* pJoinInfo, Path* ToolPath);
	virtual BOOL SnappingDrag() { return FALSE;  }
	
	// We want to know about key presses so we can see when the modifers change if the
	// mouse is not moving
	virtual BOOL DragKeyPress(KeyPress* pKeyPress, BOOL bSolidDrag);

	// Virtual functions needed for the dragging operations to work
	virtual void DragPointerMove(DocCoord PointerPos, ClickModifiers ClickMods, Spread*, BOOL bSolidDrag);
	virtual void DragPointerIdle(DocCoord PointerPos, ClickModifiers ClickMods, Spread*, BOOL bSolidDrag);
	virtual void DragFinished(	DocCoord PointerPos, 
								ClickModifiers ClickMods, 
								Spread *, 
								BOOL Success, BOOL bSolidDrag);


	// Some Render functions to will draw the EORed path
	void RenderDragBlobs( DocRect, Spread*, BOOL bSolidDrag );
		
	// These functions required for the OpDescriptor class
	static BOOL Declare();
	static OpState GetState(String_256* Description, OpDescriptor*);

	virtual BOOL IsBrushOp() { return FALSE;}
protected:
	// Functions to help with the cursors
	BOOL LoadCursors();
	void RemoveCursors();
	void SetCursorOnMove(ClickModifiers ClickMods, Spread* pSpread, DocCoord* PointerPos);
	virtual void SetCursorAndStatus(CursorType CurType);
 
	// Functions to help during a drag
	BOOL PrepareTrackDataPath();
	void RubOutPath(DocCoord Pos, Spread* pSpread);
	void AddPointsToPath(DocCoord Pos, Spread* pSpread);
	BOOL CheckMouseOverSelectedPath(DocCoord* Pos, Spread* pSpread, BOOL* IsNearEndpoint);
	BOOL AreSlotsInSameSubPath(INT32 Slot1, INT32 Slot2, Path* pPath);
	
	// Functions to help with the building of a smoothed path
	BOOL CompleteOperation();
	BOOL CreateNewPath(NodePath** pNewPath);
	BOOL SmoothNewCurve(NodePath* pNewNodePath);
	BOOL InsertSmoothCurveWithUndo(NodePath* pNewNodePath);

	// Functions to help with replacing sections of existing paths
	virtual BOOL TryToReplaceSection(NodePath* pNewNodePath);
	BOOL SplitAtPoint(const DocCoord& SplitPoint, INT32*, INT32*);
	BOOL ReplaceMiddleOfPath(NodePath* pNewPath, INT32 FirstChangedIndex, INT32 NumElements);

	// updates the timestamping and pressure lists of a brush 
	virtual BOOL EditBrushLists(NodePath* pNewPath, INT32 FirstChangedIndex, INT32 NumElements);
	virtual BOOL SimpleJoinBrush(NodePath* pNodePath, Path* pPath);
	virtual BOOL ReverseBrushPressure();
	// applies retro smoother to a brush
	virtual BOOL RetroSmoothBrush(NodePath* pNodePath);

	// Functions to help with the joining of paths
	BOOL TryToJoinNewPathWithOthers(NodePath* FreePath);
	BOOL SimpleJoin(NodePath* FreePath, NodePath* OldPath);
	BOOL ComplexJoin(NodePath* FreePath, NodePath* OldPath);
	BOOL ComplexToComplexJoin(NodePath* FreePath, NodePath* OldPath);
	BOOL VeryComplexToComplexJoin(NodePath* FreePath, NodePath* TopPath, NodePath* BotPath);

	// Functions to help with the creation of new paths
	virtual BOOL ApplyAttributes(NodePath* NewPath, Document *pDocument);
	BOOL AddPressureAttribute(NodePath *NewPath);
	NodePath* MakeCopy(Node* pOriginalNode);
	BOOL InsertNewNode(NodePath* pNewNode, DocRect& Invalid, Node* pOldNode, Node* pOtherOld = NULL);
	void SetRetroPath(NodePath* pNodePath, INT32 Start, INT32 Len);

	// Functions used in Straight Line Mode
	void AddStraightLine();

	void RenderLine(RenderRegion *pRender, Path *pPath, INT32 CoordIndex, BOOL StartCoord = FALSE);
	void RenderLine(DocRect* Rect, Spread* pSpread, Path *pPath, INT32 Index, BOOL StartCoord = FALSE);

	void RenderEorStraightLine(DocRect*, Spread* pSpread, DocCoord *Start=NULL, DocCoord *End=NULL);
	void RenderEorStraightLine(RenderRegion *pRender,
								DocCoord *pStart, INT32 StartWidth,
								DocCoord *pEnd, INT32 EndWidth);

	SimpleJoinType GetSimpleJoinType(Path* pNewPath, Path* ExistingPath);
	
	INT32 GetCurrentLineWidth(void);

	// DY 9/99 if we are editing the path of a blend on a path then this function
	// retrieves the parent NodeBlend of the NodeblendPath
	NodeGroup* GetParentOfNodePath();  
	BOOL InsertChangeBlendStepsAction(NodeBlend* pNodeBlend); 
	BOOL InvalidateBrushRegion(NodePath* pNodePath);

		// General data to help with the path fitting and joining
	Path*		TrackData;			// A Path to store the mouse moves in (part of the tool)
	DocCoord	StartPoint;			// The first point in the path
	Spread*		StartSpread;		// The spread we started on
	Spread*		PreviousSpread;		// The spread that the last point was over
	INT32 		Smoothness;			// The current smoothing factor

	// Data that is maintained as we draw
	DocCoord	PreviousPoint;		// The most recently added point
	INT32 		LineSegmentCount;	// Number of elements in the path.
	BOOL		CanLineJoin;		// TRUE if the line can be joined to its start
	BOOL 		IsStraightLineMode;
	DocCoord	StraightLinePos;

	// Pressure info
	BOOL AddPressureToPath;			// TRUE is we should bother to add pressure info
	UINT32 FreeHandPressure;			// The current pen pressure

	// The paths that we are joined to, or NULL if we are joined to none
	FreeHandJoinInfo* pJoinInfo;
	NodePath*	StartPath;
	NodePath*	EndPath;
	INT32		CloseTo;			// Slot that the end path is near
	double		Mu;					// The bezier distance along the segment to the closest point
	BOOL 		IsEndNearEndpoint;

	// The Cursors used by this operation
	Cursor* pFreeHandCursor;		// Normal freehand cursor
	Cursor* pJoinCursor;			// Freehand join cursor
	Cursor* pRubOutCursor;			// Rub Out Mode Cursor
	Cursor* pStraightCursor;		// Straight Line Mode Cursor
	Cursor* pModifyCursor;			// Indicates the completion of a modify line
	Cursor*	MyCurrentCursor;		// so I don't flip cursor too often
	INT32	CurrentCursorID;		// ID of the current cursor on the stack
		

	NodePath* m_pNewNodePath;


private:

};

#endif	// INC_OPFREE
