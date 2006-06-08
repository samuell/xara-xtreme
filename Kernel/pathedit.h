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
// Header file for path editing routines

#ifndef INC_PATHEDIT
#define INC_PATHEDIT

#include "paths.h"
#include "rndrgn.h"
#include "selop.h"
#include "objchge.h"

#define OPTOKEN_JOINSHAPEOP		_T("JoinShapes")
#define OPTOKEN_BREAKSHAPEOP	_T("BreakShapes")
#define OPTOKEN_DELETEPOINTSOP	_T("DeletePoints")

#define OPTOKEN_ADDENDPOINT		_T("AddEndpoint")
#define OPTOKEN_AUTOCLOSEPATHS	_T("AutoClosePaths")

class NodePath;
class Document; 
class OperationHistory;
class SelectionState;     
class Spread;
class Node;
class NodeHidden;
class TransformBase;
class BezierTool;
class Cursor;
class AttrBrushType;
class NodeBlend;
class NodeGroup;

/********************************************************************************************

>	class OpNodePathEditBlob : public SelOperation

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/7/93
	Purpose:	This operation is started by the NodePath when it receives a drag message
				from one of the tools. It uses it to drag one (or many) of the paths control
				points around the screen to edit the path. As the control points are dragged
				about the new version of the path, corosponding to the new blob positions,
				will be EOR rendered to the window.

********************************************************************************************/

class OpNodePathEditBlob : public SelOperation
{

CC_DECLARE_DYNCREATE(OpNodePathEditBlob)

public:
	// Construction/Destruction
	OpNodePathEditBlob();

	// The all important Do functions
	void DoStartDragEdit( NodePath*, DocCoord Anchor, Spread *);
	
	// Virtual functions needed for the dragging operations to work
	virtual void DragPointerMove( DocCoord Pos, ClickModifiers Mods, Spread* pSpread, BOOL bSolidDrag);
	virtual void DragFinished(	DocCoord Pos, ClickModifiers Mods, Spread* pSpread,	BOOL Success, BOOL bSolidDrag);

	// Some Render functions to draw the EORed path
	virtual void RenderDragBlobs (DocRect, Spread*, BOOL bSolidDrag);		// Called by camelot

	// These functions required for the OpDescriptor class
	static BOOL Init();
	static OpState GetState(String_256* Description, OpDescriptor*);

protected:
	// Functions to build and keep the path smooth during editing
	BOOL BuildEditPath();
	BOOL BuildEditPaths();		// CGS 19/4/2000
	BOOL CopyEditedPathBack();
	BOOL CopyEditedPathBack(NodePath* pOrigPath, Path* pEditPath);			// CGS 20/4/2000
	BOOL CopyNeedToRenderFlags();
	BOOL CopyNeedToRenderFlags(NodePath* pOrigPath, Path* pEditPath);		// CGS 20/4/2000
	void RecalculatePath(DocCoord Offset, BOOL SnapEnds = FALSE, INT32 SnapIndex = 0);
	void RecalculatePaths(Path* pEditPath, DocCoord Offset, BOOL SnapEnds = FALSE, INT32 SnapIndex = 0);	// CGS 20/4/2000
	void SnapEndsTogether();
	void SnapEndsTogether(Path* pEditPath);
	BOOL JoinWithOtherPath();
	BOOL JoinWithOtherPath(NodePath** pOrigPath);
	BOOL FillPathIfEndsSnapped();
	BOOL FillPathIfEndsSnapped(NodePath* pOrigPath);
	virtual void RenderDraggingBlobs (DocRect, Spread*);	// Called by this/derived operation
	virtual void RenderDraggingBlobs (Path* pEditPath, Spread*);	// Called by this/derived operation	// CGS 20/4/2000
	void RenderPathEditBlobs( DocRect Rect, Spread *pSpread );
	void RenderPathEditBlobs( Path* pEditPath, /*DocRect Rect,*/ Spread *pSpread );		//CGS 20/4/2000
	virtual void SetStatusLineHelp();

	// DY 9/99 if we are editing the path of a blend on a path then this function
	// retrieves the parent NodeBlend of the NodeblendPath
	NodeGroup* GetGroupParentOfCurve();  
	NodeGroup* GetGroupParentOfCurve(NodePath* pOrigPath);				// CGS 20/4/2000
	BOOL InsertChangeBlendStepsAction(NodeBlend* pNodeBlend); 
	AttrBrushType* GetAppliedBrush();


	virtual BOOL EditBrushAttribute(INT32 FirstIndex, INT32 LastIndex, AttrBrushType* pAttrBrush); 
	MILLIPOINT GetLengthOfPathSection(Path* pPath, INT32 FirstIndex, INT32 LastIndex);
	MILLIPOINT GetDistanceToPathIndex(Path* pPath, INT32 Index);

	Spread*	 StartSpread;		// The spread that the drag started on
	DocCoord StartMousePos;		// Mouse position of initial click
	DocCoord LastMousePos;		// The position of the mouse at the last mouse move message
	DocCoord ConstrainPoint; 	// The coord around which to constrain the drag
	DocCoord ConstrainPrevPoint;	// The coordinate of the previous point on the path
	DocCoord ConstrainNextPoint;	// The coordinate of the next point on the path

	// The original path and a copy of it that we can change
	NodePath*	OriginalPath;
	Path		EditPath;

	// The original node paths and a copies of them that we can change
	List	OriginalPaths;
	List	EditPaths;
	List	PathsDragStarted;

	BOOL	MultiplePaths;		// whether were dealing with multiple paths
	
	// Two flags to determine how we're snapping. We can either snap to
	// the other end of the path, or to the end of another path, or neither
	// but never both.	
	BOOL		EndSnapped;		// if the start snapped to the end
 	BOOL		SnapToAnother;	// If the end snapped to another end
	NodePath* 	SnapToPath;		// Pointer to path we might be snapping to
	INT32		SnapToIndex;	// Index of element we're snapping to
	BOOL		SnapToLineOrCurve;	// TRUE is snapping to a curve, FALSE if line
	DocCoord	SnapToCoords[4];// Array of coords for the other snapping path

	Cursor* pMoveCursor;			
	Cursor* pCloseCursor;	
	Cursor* pCrossHairCursor;				
	Cursor*	MyCurrentCursor;				// so I don't flip cursor too often
	INT32 	CurrentCursorID;				// ID of the current cursor on the stack
	void	ChangeCursor(Cursor* cursor);	// function to change cursor
	BOOL	CreateCursors();
	void	DestroyCursors();
	
	ObjChangePathEdit EditObjChange;

	List	ObjChanges;						// for the multiple case ....

	INT32	DragPoint;			// Two indexes to the point(s) currently being
	INT32	UpdatePoint;		// dragged

	BOOL	DragStarted;

};





/********************************************************************************************

>	class OpNodePathEditControlBlob : public OpNodePathEditBlob

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/7/93
	Purpose:	This operation is started by the NodePath when it receives a drag message
				from one of the tools. It uses it to drag one of the paths Bezier control
				points around the screen to edit the path. As the control points are dragged
				about the new version of the path, corosponding to the new blob positions,
				will be EOR rendered to the window.
				This class is derived from OpNodePathEditBlob which drags the normal selected
				points around. This drags the Bezier control points around of one of the 
				selected points on the curve. Most of the functions are not needed as it
				uses the base class versions.

********************************************************************************************/

class OpNodePathEditControlBlob : public OpNodePathEditBlob
{

CC_DECLARE_DYNCREATE(OpNodePathEditControlBlob)

public:
	// Construction/Destruction - Simply call the base class constructor
	OpNodePathEditControlBlob() : OpNodePathEditBlob() { };

	// The all important Do functions
	void DoStartDragEdit( NodePath*, DocCoord Anchor, Spread *pSpread, INT32);

	// Virtual functions needed for the dragging operations to work
	virtual void DragPointerMove( DocCoord PointerPos, ClickModifiers ClickMods, Spread*, BOOL bSolidDrag);
	virtual void RenderDragBlobs(DocRect, Spread*, BOOL bSolidDrag);
			
	// These functions required for the OpDescriptor class
	static BOOL Init();
	static OpState GetState(String_256* Description, OpDescriptor*);

protected:
	// Private functions that help out the functions above
	BOOL BuildEditPath();
	void RecalculatePath( DocCoord Offset );
	virtual void RenderDraggingBlobs (DocRect, Spread*);	// Called by this/derived operation
	void RenderPathEditControlBlobs( DocRect Rect, Spread *pSpread );
	void SetStatusLineHelp();

	// Private member vars
	INT32 Index;				// Index of the dragging control 
	INT32 EndPointIndex;		// Index of the nearest Endpoint
	INT32 OppositeIndex;		// Index of the control point opposite the endpoint
};

/********************************************************************************************

>	class OpNodePathAddEndpoint : public OpNodePathEditBlob

	Author:		Jim_Lynn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/6/94
	Purpose:	This operation is started by the bezier editing tool when it wants to add
				a point to the start or end of a particular path. It uses the normal dragging
				of the base operation, but the action when the operation finishes is different.
				This class is derived from OpNodePathEditBlob which drags the normal selected
				points around. 

********************************************************************************************/

class OpNodePathAddEndpoint : public OpNodePathEditBlob
{

CC_DECLARE_DYNCREATE(OpNodePathAddEndpoint)

public:
	// Construction/Destruction - Simply call the base class constructor
	OpNodePathAddEndpoint() : OpNodePathEditBlob() {};

	// The all important Do functions
	void DoStartDragEdit( NodePath*, DocCoord Anchor, Spread *pSpread, ClickModifiers Mods, INT32 PathIndex, BOOL CurveOrLine, BOOL CloseThePath = FALSE, BOOL SmoothOrCusp= TRUE);

	// Virtual functions needed for the dragging operations to work
	// We don't need to override the normal dragging functions...

	//virtual void DragPointerMove( DocCoord PointerPos, ClickModifiers ClickMods, Spread* );

	// but we do need to override the DragFinished event in order to add the endpoint
	// before we put in the smoothed points

	virtual void DragFinished(	DocCoord Pos, ClickModifiers Mods, Spread* pSpread,	BOOL Success, BOOL bSolidDrag);
				
	// These functions required for the OpDescriptor class
	static BOOL Init();
	static OpState GetState(String_256* Description, OpDescriptor*);

protected:
	// Private functions that help out the functions above
	BOOL BuildEditPath(DocCoord NewPoint);

	BOOL InsertElement(DocCoord NewPoint, Path* DestPath, INT32 InsPos, INT32* NewPosition,
						BOOL RecordUndo = FALSE, NodePath* UndoPath = NULL);
	BOOL Complete(DocCoord AddElementPoint);
	BOOL CompleteThisPath(DocCoord AddElementPoint);
	virtual void SetStatusLineHelp();
	virtual BOOL EditBrushAttribute(INT32 FirstIndex, INT32 LastIndex, AttrBrushType* pAttrBrush); 


	// Private member vars
	INT32 NewPointIndex;		// Index of point added to temporary path
	INT32 InsertPosition;	// Index of point to which new point was added
	BOOL IsPathClosing;		// Tells the operation if the added segment should close the path
	BOOL AddCurveFlag;		// TRUE if adding curve, FALSE if adding line
	BOOL AddSmoothFlag;		// TRUE if adding smooth point, FALSE if adding cusp.

};



/********************************************************************************************
>	class OpCloseNodePaths : public OpNodePathAddEndpoint

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/12/95
	Purpose:	This operation is for closing all the open selected paths.
********************************************************************************************/
class OpCloseNodePaths : public OpNodePathAddEndpoint
{
	CC_DECLARE_DYNCREATE(OpCloseNodePaths)

public:
	OpCloseNodePaths() : OpNodePathAddEndpoint() { };

	void DoWithParam(OpDescriptor* pOpDesc, OpParam* pOpParam);
	static BOOL Init();
	static OpState GetState(String_256* Description, OpDescriptor*);

protected:
	static BOOL IsThisPathCloseable(NodePath* pPath);
	BOOL CloseThisPath(NodePath* pPath);
};


/********************************************************************************************

>	class OpReshapeOrAddPoint : public OpNodePathEditBlob

	Author:		Jim_Lynn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/8/94
	Purpose:	This operation serves two purposes - while dragging, it will reshape a bezier
				segment, changing the path on ending the drag. However, if the drag is 
				released immediately after it is initiated, a point is added to the bezier
				curve (or line) at the click point.

********************************************************************************************/

class OpReshapeOrAddPoint : public OpNodePathEditBlob
{

CC_DECLARE_DYNCREATE(OpReshapeOrAddPoint)

public:
	// Construction/Destruction 
	OpReshapeOrAddPoint();

	// The all important Do functions
	void DoStartDragEdit( NodePath*, DocCoord Anchor, Spread *pSpread, INT32, double);

	// Virtual functions needed for the dragging operations to work
	virtual void DragPointerMove( DocCoord PointerPos, ClickModifiers ClickMods, Spread*, BOOL bSolidDrag);
			
	// We need to override the DragFinished event in order to add a point
	// if the mouse didn't move very much.

	virtual void DragFinished(	DocCoord Pos, ClickModifiers Mods, Spread* pSpread,	BOOL Success, BOOL bSolidDrag);
				
	// These functions required for the OpDescriptor class
	static BOOL Init();
	static OpState GetState(String_256* Description, OpDescriptor*);

protected:
	// Private functions that help out the functions above
	BOOL BuildEditPath();
	void RecalculatePath( DocCoord Offset );
	BOOL DragFinishedAddPoint( );
	BOOL DragFinishedReshapeLine( );
	void SetStatusLineHelp();

	// Private member vars
	INT32 Index;				// Index of the dragging segment
	double paramdist;		// parametric distance along the segment from where we drag
	DocCoord OrigControl1;	// Coords of first control point on curve
	DocCoord OrigControl2;	// Coords of second ctrl pt on curve
	DocCoord FurthestPoint;	// Coords of furthest drag point away from start
	BOOL HaveMadeCurve;		// True if the dragging segment started as a line
};

/********************************************************************************************

>	class OpNewPath : public OpNodePathEditBlob

	Author:		Jim_Lynn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/7/94
	Purpose:	This operation will create a new path of a single line or curve and put it in
				the tree. 

********************************************************************************************/

class OpNewPath : public OpNodePathEditBlob
{

CC_DECLARE_DYNCREATE(OpNewPath)

public:
	// Construction/Destruction - Simply call the base class constructor
	OpNewPath() : OpNodePathEditBlob() { };

	// The all important Do functions
	void DoStartDragEdit( DocCoord, DocCoord, Spread*, ClickModifiers Mods, BOOL MakeCurve, BOOL MakeSmooth);

	// Virtual functions needed for the dragging operations to work
	// We don't need to override the normal dragging functions...
	// but we do need to override the DragFinished event in order to add the endpoint
	// before we put in the smoothed points

	virtual void DragFinished(	DocCoord Pos, ClickModifiers Mods, Spread* pSpread,	BOOL Success, BOOL bSolidDrag);
				
	// These functions required for the OpDescriptor class
	static BOOL Init();
	static OpState GetState(String_256* Description, OpDescriptor*);

protected:
	// Private functions that help out the functions above
	BOOL BuildEditPath(DocCoord FirstPoint, DocCoord LastPoint);

	// Private member vars
	INT32 NewPointIndex;		// Index of point added to temporary path
	INT32 InsertPosition;	// Index of point to which new point was added
	BezierTool* pBezTool;	// Pointer to tool which called the operation
	BOOL AddCurveFlag;		// TRUE if adding a curve, FALSE if adding a line
	BOOL AddSmoothFlag;		// TRUE to set rotate flags, FALSE to clear
};

/********************************************************************************************

>	class OpToggleSmooth : public SelOperation

	Author:		Jim_Lynn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/6/94
	Purpose:	When a path point is double-clicked on, this operation is invoked to toggle
				the smoothness of the point. This works much like the equivalent feature in 
				ArtWorks except that making a smooth point into a cusp will change the position
				of the control points.

********************************************************************************************/

class OpToggleSmooth : public SelOperation
{

CC_DECLARE_DYNCREATE(OpToggleSmooth)

public:
	// Construction/Destruction
	OpToggleSmooth();

	// The all important Do functions
	void DoTogglePoint( NodePath*, INT32 Index, Spread *, BOOL AllSmooth, BOOL DontMoveOnCusp);
		
	// These functions required for the OpDescriptor class
	static BOOL Init();
	static OpState GetState(String_256* Description, OpDescriptor*);

protected:
	BOOL TogglePathPoints(NodePath* CurrentNode, INT32 Index, BOOL AllSmooth, BOOL DontMoveOnCusp);
	BOOL MakePointCusp(NodePath* CurrentNode, INT32 Index, BOOL DontMoveOnCusp);
	BOOL MakePointSmooth(NodePath* CurrentNode, INT32 Index);
};

/********************************************************************************************

>	class OpJoinShapes : public SelOperation

	Author:		Jim_Lynn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/7/94
	Purpose:	This class takes two or more selected paths and combines them into one path
	SeeAlso:	-

********************************************************************************************/

class OpJoinShapes: public SelOperation
{         

	CC_DECLARE_DYNCREATE( OpJoinShapes )    

public:
	OpJoinShapes();								
	static BOOL		Init();				
	static OpState	GetState(String_256*, OpDescriptor*);		
	void			Do(OpDescriptor*);	

};  
 

/********************************************************************************************

>	class OpBreakShapes : public SelOperation

	Author:		Jim_Lynn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/7/94
	Purpose:	This class will break compound paths into two or more simple paths. It will
				work when there are one or more compound paths selected.
	SeeAlso:	-

********************************************************************************************/

class OpBreakShapes: public SelOperation
{         

	CC_DECLARE_DYNCREATE( OpBreakShapes )    

public:
	OpBreakShapes();								
	static BOOL		Init();				
	static OpState	GetState(String_256*, OpDescriptor*);		
	void			Do(OpDescriptor*);	

	// Karim 21/01/2000 - see UndoableOperation base implementation.
	virtual BOOL MayChangeNodeBounds() const { return FALSE; }
};  
 

/********************************************************************************************

>	class OpDeletePoints : public SelOperation

	Author:		Jim_Lynn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/7/94
	Purpose:	This class will delete any selected points from selected paths.
	SeeAlso:	-

********************************************************************************************/

class OpDeletePoints: public SelOperation
{         

	CC_DECLARE_DYNCREATE( OpDeletePoints )    

public:
	OpDeletePoints();								
	static BOOL		Init();				
	static OpState	GetState(String_256*, OpDescriptor*);		
	void			Do(OpDescriptor*);	

private:
	BOOL DeleteLineTo(NodePath* pPath, INT32 Index, BOOL* PathExists);
	BOOL DeleteMoveTo(NodePath* pPath, INT32 Index, BOOL* PathExists);
	BOOL DeleteBezierTo(NodePath* pPath, INT32 Index, BOOL* PathExists);
	
	// Functions to determine wether the entire path will be deleted or not
	static BOOL WillDeleteEntirePath(NodePath* pPath);
	static BOOL TryDeleteLineTo(NodePath* pPath, INT32 Index, BOOL* PathExists);
	static BOOL TryDeleteMoveTo(NodePath* pPath, INT32 Index, BOOL* PathExists);
	static BOOL TryDeleteBezierTo(NodePath* pPath, INT32 Index, BOOL* PathExists);
};  



/********************************************************************************************

>	class ModifyPathAction : public Action

	Author:		Jim_Lynn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/5/94
	Purpose:	An action which undoes the modification of path data. This action can be
				used for undoing changing coordinates or flags or verbs. Other actions will 
				exist for adding or deleting elements from paths. This action is useful for
				undoing large amounts of path editing (e.g. dragging several points around)
				but is less useful for undoing the alteration of single coordinates. Another
				Action might be created later for undoing single element alterations.
	SeeAlso:	-

********************************************************************************************/

class ModifyPathAction : public Action
{

CC_DECLARE_DYNCREATE(ModifyPathAction)

public:
	ModifyPathAction();
	~ModifyPathAction();
	virtual ActionCode Execute();
	static ActionCode Init( Operation* pOp,
							ActionList* pActionList,
							INT32 NumChangedElements,
							Action** NewAction);
	void StoreArrays(PathVerb* Verbs, PathFlags* Flags, DocCoord* Coords, 
					INT32* Indices, NodePath* WhichPath);

protected:
	INT32 		NumElements;
	NodePath* 	ChangedPath;
	PathVerb* 	ChangedVerbs;
	PathFlags* 	ChangedFlags;
	DocCoord* 	ChangedCoords;
	INT32*		ChangedIndices;
};


/********************************************************************************************

>	class RemovePathElementAction : public Action

	Author:		Jim_Lynn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/6/94
	Purpose:	An action which undoes the insertion of an element into path data (hence
				removing that element from the path). 
	SeeAlso:	-

********************************************************************************************/

class RemovePathElementAction : public Action
{
	  
CC_DECLARE_DYNCREATE(RemovePathElementAction)

public:
	RemovePathElementAction();
	~RemovePathElementAction();
	virtual ActionCode Execute();
	static ActionCode Init( Operation* pOp,
							ActionList* pActionList,
							INT32 NumChangedElements,
							INT32 ChangedIndex,
							Action** NewAction);
	void RecordPath(NodePath* WhichPath);

protected:
	INT32 		NumElements;
	INT32 		FirstChangedIndex;
	NodePath* 	ChangedPath;
};


/********************************************************************************************

>	class InsertPathElementAction : public Action

	Author:		Jim_Lynn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/6/94
	Purpose:	An action which undoes the removal of an element into path data (hence
				replacing the deleted data).
	SeeAlso:	-

********************************************************************************************/

class InsertPathElementAction : public Action
{

CC_DECLARE_DYNCREATE(InsertPathElementAction)

public:
	InsertPathElementAction();
	~InsertPathElementAction();
	virtual ActionCode Execute();
	static ActionCode Init( Operation* pOp,
							ActionList* pActionList,
							INT32 NumChangedElements,
							INT32 ChangedIndex,
							Action** NewAction);
	void RecordPath(PathVerb* Verbs, PathFlags* Flags, DocCoord* Coords, 
					NodePath* WhichPath);

protected:
	INT32 		NumElements;
	INT32 		InsertionPoint;
	NodePath* 	ChangedPath;

	PathVerb* 	ChangedVerbs;
	PathFlags* 	ChangedFlags;
	DocCoord* 	ChangedCoords;
};


/********************************************************************************************

>	class ModifyElementAction : public Action

	Author:		Jim_Lynn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/6/94
	Purpose:	An action which undoes the modification of a single element of path data. This 
				action can be used for undoing changing coordinates or flags or verbs. This action
				is not really suitable if you have a lot of path data to change - ModifyPathAction 
				is more efficient because it encodes multiple elements. This action was written because
				for single elements, ModifyPathAction is too complicated to call.
	SeeAlso:	-

********************************************************************************************/

class ModifyElementAction : public Action
{

CC_DECLARE_DYNCREATE(ModifyElementAction)

public:
	ModifyElementAction();
	~ModifyElementAction();
	virtual ActionCode Execute();
	static ActionCode Init( Operation* pOp,
							ActionList* pActionList,
							PathVerb Verb, 
							PathFlags Flags, 
							DocCoord Coord, 
							INT32 Index, 
							NodePath* WhichPath,
							Action** NewAction);

	// Access functions for op merging
	NodePath* GetActionPath() const {return ChangedPath;}
	INT32 GetActionIndex() const {return ChangedIndex;}


protected:
	NodePath* 	ChangedPath;
	PathVerb 	ChangedVerb;
	PathFlags 	ChangedFlags;
	DocCoord 	ChangedCoord;
	INT32		ChangedIndex;
};

/********************************************************************************************

>	class ModifyFlagsAction : public Action

	Author:		Jim_Lynn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/7/94
	Purpose:	An action which undoes the modification of a path element's flags. This is a
				smaller action that ModifyElementAction and is more economic for undoing 
				selection change
	SeeAlso:	-

********************************************************************************************/

class ModifyFlagsAction : public Action
{

CC_DECLARE_DYNCREATE(ModifyFlagsAction)

public:
	ModifyFlagsAction();
	~ModifyFlagsAction();
	virtual ActionCode Execute();
	static ActionCode Init( Operation* pOp,
							ActionList* pActionList,
							PathFlags Flags, 
							INT32 Index, 
							NodePath* WhichPath,
							Action** NewAction);
protected:
	NodePath* 	ChangedPath;
	PathFlags 	ChangedFlags;
	INT32		ChangedIndex;
};


/********************************************************************************************

>	class ModifyFilledAction : public Action

	Author:		Jim_Lynn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/7/94
	Purpose:	An action which undoes the modification of a path's filled bit (or redoes it)
				It can also be used to undo the IsStroked flag as well.
	SeeAlso:	-

********************************************************************************************/

class ModifyFilledAction : public Action
{

CC_DECLARE_DYNCREATE(ModifyFilledAction)

public:
	ModifyFilledAction();
	~ModifyFilledAction();
	virtual ActionCode Execute();
	static ActionCode Init( Operation* pOp,
							ActionList* pActionList,
							BOOL UndoFillOrStroke,
							BOOL FilledFlag,
							NodePath* WhichPath,
							Action** NewAction);
protected:
	NodePath* ChangedPath;
	BOOL UndoFilled;		// TRUE if undoing filled, FALSE if undoing stroke
	BOOL OldFilledState;
};


/********************************************************************************************

>	class RecalcBoundsAction : public Action

	Author:		Jim_Lynn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/7/94
	Purpose:	An action which records the bounding box of a node, and replaces it on Undo,
				invalidating that area. This action should be added to the action list before 
				modifying the object's bounding box. Please note that for undo and redo to work
				correctly, after you have modified the bounding box, you should create a 
				RecordBoundsAction object and append that to the action list. This action is the 
				opposite action to Recalc, and the two should always be used in tandem.
	SeeAlso:	RecordBoundsAction

********************************************************************************************/

class RecalcBoundsAction : public Action
{

CC_DECLARE_DYNCREATE(RecalcBoundsAction)

public:
	RecalcBoundsAction();
	~RecalcBoundsAction();
	virtual ActionCode Execute();
	static ActionCode Init( Operation* pOp,
							ActionList* pActionList,
							NodeRenderableBounded* WhichNode,
							Action** NewAction);
	static ActionCode DoRecalc( Operation* pOp,
								ActionList* pActionList,
								NodeRenderableBounded* WhichNode,
								BOOL OptimiseRedraw = FALSE);	   
	static BOOL CauseOptimisedRedraw(NodePath* pChangedPath, Document* pDoc, Spread* pSpread);

protected:
	NodeRenderableBounded* ChangedNode;
	DocRect OldBounds;
};

/********************************************************************************************

>	class RecordBoundsAction : public Action

	Author:		Jim_Lynn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/7/94
	Purpose:	An action which simply marks the fact that, when undone, it has to create a
				RecalcBoundsAction, and invalidate the existing bounds. It is the opposite action 
				to RecalcBounds and should be used after creating a Recalc object and modifying 
				the bounding box. Even when executed, all the action does is invalidate the screen
				and create a Recalc action.
	SeeAlso:	RecalcBoundsAction

********************************************************************************************/

class RecordBoundsAction : public Action
{

CC_DECLARE_DYNCREATE(RecordBoundsAction)

public:
	RecordBoundsAction();
	~RecordBoundsAction();
	virtual ActionCode Execute();
	static ActionCode Init( Operation* pOp,
							ActionList* pActionList,
							NodeRenderableBounded* WhichNode,
							Action** NewAction);
	static ActionCode DoRecord( Operation* pOp,
								ActionList* pActionList,
								NodeRenderableBounded* WhichNode,
								BOOL OptimiseRedraw = FALSE);
protected:
	NodeRenderableBounded* ChangedNode;
};


/********************************************************************************************

>	class SavePathArraysAction : public Action

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/01/95
	Purpose:	Given a path, save the paths array data, ie all its verbs, coords and flags
				on the undo.
				Use DoRecord(Operation*, ActionList*,Path*) to save the path.

********************************************************************************************/

class SavePathArraysAction : public Action
{

	CC_DECLARE_DYNCREATE(SavePathArraysAction)

	public:
		 SavePathArraysAction();
		~SavePathArraysAction();
		virtual ActionCode Execute();
		 static ActionCode DoRecord(Operation* pOp,
									ActionList* pActionList, 
									Path* pPath);

		 static ActionCode Init(Operation* pOp,
								ActionList* pActionList,
								Path* Path,
								Action** NewAction,
								BOOL CreateArrays = TRUE);

	private:
		ActionCode SetUpArrays(Path* pPath, Operation* pOp);
		BOOL SavePathArrays(Path* pPath, 
					  		Operation* pOp,
					  		PathFlags** pFlags,
					  		PathVerb** pVerbs,
					  		DocCoord** pCoords);
		void SwapPathArrays();

	private:
		Path* 		ChangedPath;
		PathVerb* 	ChangedVerbs;
		PathFlags* 	ChangedFlags;
		DocCoord* 	ChangedCoords;

};



//********************************************************************************************
//>	class StorePathSubSelStateAction : public Action
//
//	Author:		Peter
//	Created:	9/11/95
//	Purpose:	Stores the sub-selection state of the path.  If your operation does not
//				affect the selection state then you only need to insert one action.  If you
//				do change the selection state you can either put one of these actions at the
//				start and end of the op, or record the complete start at the start and use
//				DoChangeSelection to store individual changes.
//				Call DoRecord(Operation*, ActionList*, Path*) to store the selection state
//	SeeAlso:	UndoableOperation::DoChangeSelection
//********************************************************************************************/
//class StorePathSubSelStateAction : public Action
//{
//	CC_DECLARE_DYNCREATE(StorePathSubSelStateAction)
//
//public:
//	StorePathSubSelStateAction();
//	~StorePathSubSelStateAction();
//	virtual ActionCode Execute();
//	static ActionCode DoRecord(Operation* pOp, ActionList* pActionList, Path* pPath);
//	static ActionCode Init(Operation* pOp, ActionList* pActionList, Path* Path,	Action** NewAction);
//
//protected:
//	Path* 	pStatePath;			// Points to the path whose selection state is recorded
//	UINT32*	pIndexArray;		// Points to the array of path indexes
//	UINT32	NumStoredIndexes;	// The number of indexes in the array
//	BOOL	RecordingSelected;	// TRUE if the indexes are selected, FALSE if not
//};


#endif 		// INC_PATHEDIT
