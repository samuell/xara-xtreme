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

/*
*/


#ifndef INC_SHAPEOPS
#define INC_SHAPEOPS

//#include "doccoord.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "paths.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "ops.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "selop.h"						  - in camtypes.h [AUTOMATICALLY REMOVED]

#define OPTOKEN_EDITREGULARSHAPE	_T("EditRegularShape")
#define OPTOKEN_DRAGREGULARSHAPE	_T("DragRegularShape")
#define OPTOKEN_RESHAPESHAPEEDGE	_T("ReshapeShapeEdge")
#define OPTOKEN_TOGGLEELIPPOLY		_T("ShapeToggleElipPoly")
#define OPTOKEN_TOGGLECURVATURE		_T("ShapeToggleCurvature")
#define OPTOKEN_TOGGLESTELLATION	_T("ShapeToggleStellation")
#define OPTOKEN_QUICKSHAPE_MAKEREGULAR	_T("QuickShapeMakeRegular")

#define OPTOKEN_QUICKSHAPE_NUMBERSIDES _T("QuickShapeNumberSides")
#define OPTOKEN_QUICKSHAPE_NUMBERSIDES3 _T("QuickShapeNumberSides3")
#define OPTOKEN_QUICKSHAPE_NUMBERSIDES4 _T("QuickShapeNumberSides4")
#define OPTOKEN_QUICKSHAPE_NUMBERSIDES5 _T("QuickShapeNumberSides5")
#define OPTOKEN_QUICKSHAPE_NUMBERSIDES6 _T("QuickShapeNumberSides6")
#define OPTOKEN_QUICKSHAPE_NUMBERSIDES7 _T("QuickShapeNumberSides7")
#define OPTOKEN_QUICKSHAPE_NUMBERSIDES8 _T("QuickShapeNumberSides8")
#define OPTOKEN_QUICKSHAPE_NUMBERSIDES9 _T("QuickShapeNumberSides9")
#define OPTOKEN_QUICKSHAPE_NUMBERSIDES10 _T("QuickShapeNumberSides10")

class Cursor;
class NodeRegularShape;

/***********************************************************************************************

>	class EditRegularShapeParam : public OpParam

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/12/94
	Purpose:	The OpParam to use when calling OpEditRegularShape.  Used to pass in the shape(s)
				to edit and what to change them to.
				ChangeShape points to a specific NodeRegularShape to edit or is NULL to indicate
				all slected shapes should be changed
				ShapeData points to a NodeRegularShape whose data members are copied into
				ChangeShape.
	SeeAlso:	EditRegularShapeParam::DoWithParam

***********************************************************************************************/
class EditRegularShapeParam : public OpParam
{
	CC_DECLARE_MEMDUMP(EditRegularShapeParam)	

public:
	enum BooleanChange {CHANGE_DONTCARE, CHANGE_SETFALSE, CHANGE_SETTRUE};
	enum AffectShape {AFFECT_ALL, AFFECT_ELLIPSES, AFFECT_RECTANGLES};
	
	EditRegularShapeParam(NodeRegularShape* ChangeShapes);

	NodeRegularShape* ShapeToChange;
	AffectShape ShapesToAffect;
	Path*	NewEdgePath1;		// NULL to ignore
	Path*	NewEdgePath2;		// NULL to ignore
	UINT32	NewNumSides;		// 0 to ignore
	BooleanChange	NewCircular;
	BooleanChange	NewStellated;
	BooleanChange	NewPrimaryCurvature;
	BooleanChange	NewStellationCurvature;
	double	NewStellRadiusToPrimary;	// -1.0 to ignore
	double	NewPrimaryCurveToPrimary;	// -1.0 to ignore
	double	NewStellCurveToStell;		// -1.0 to ignore
	double	NewStellOffsetRatio;		// -1000 to ignore

	DocCoord	TranslateTo;			// Position to translate the shape to
	DocCoord	TranslateBy;			// Offset to translate the shape by 
	INT32		SetRadiusLength;		// Ammount to set the shapes major axes length to (scaling the shape)
	INT32		AddRadiusLength;		// Ammount to [inc/dec]rement the shapes major axes length by
	double		RotateTo;				// Angle to rotate each shape to (PI to -PI 0 along +ve x axis)
	double		RotateBy;				// Angle (in radians) to rotate each shape by
	INT32		SetStellationLength;	// Ammount to set the shapes stellation radius to
	INT32		AddStellationLength;	// Ammount to [inc/dec]rement the shapes stellation radius by
	double		SetStellationOffset;	// New value of the stellation offset (-180 to 180)
	double		AddStellationOffset;	// Value to increase the stellation offset by (degrees)
	double		AddPrimaryCurvature;	// Value to increase the primary curvature by
	double		AddStellationCurvature;	// Value to increase the stellation curvature by
	INT32		SetMajorAxisLength;		// New length of the shapes MajorAxis
	INT32		AddMajorAxisLength;		// Ammount to add to the shapes MajorAxis
	INT32		SetMinorAxisLength;		// New length of the shapes MinorAxis
	INT32		AddMinorAxisLength;		// Ammount to add to the shapes MinorAxis
};



/********************************************************************************************

>	class OpEditRegularShape : public SelOperation

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/12/94  
	Purpose:	This operation can be used to change the internal data members of
				NodeRegularShapes in an undoable way.  It either changes one particular shape
				or all selected shapes.  The data members are changed to those supplied by
				the parameter object.
	SeeAlso:	OpEditRegularShape::DoWithParam

********************************************************************************************/

class OpEditRegularShape: public SelOperation
{         
	CC_DECLARE_DYNCREATE( OpEditRegularShape )    

public:
	OpEditRegularShape();								
	static BOOL		Init();				
	static OpState	GetState(String_256*, OpDescriptor*);		
	void			DoWithParam(OpDescriptor* pOpDesc, OpParam* pEntryOpParam);	

protected:
	BOOL CarryOut(NodeRegularShape*	ShapeToEdit, EditRegularShapeParam* pOpParam);	
	BOOL ShouldDoThisShape(NodeRegularShape* pShape, EditRegularShapeParam::AffectShape Allowance);
};  



/********************************************************************************************

>	class OpDragRegularShape : public SelOperation

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/12/94
	Purpose:	This operation is started by a NodeRegularShape when someone drags on one of
				its control points. As the control point is dragged about the new version of
				the shape, corosponding to the new blob positions, will be EOR rendered to
				the window.

********************************************************************************************/

class OpDragRegularShape : public SelOperation
{

CC_DECLARE_DYNCREATE(OpDragRegularShape)

	enum DragType {DRAG_CENTRE, DRAG_ELLIPSE, DRAG_PRIMARY, DRAG_STELLATION,
												DRAG_PRIMARYCURVE, DRAG_STELLATIONCURVE};

public:
	// Construction/Destruction
	OpDragRegularShape();

	// The all important Do functions
	void DoStartDragEdit( NodeRegularShape*, DocCoord, Spread *, DragType, BOOL QuickShape);
	void DoStartDragEditCurve( NodeRegularShape*, DocCoord, Spread *, DragType, DocCoord Start, DocCoord End);
	virtual BOOL SnappingDrag();
	
	// Virtual functions needed for the dragging operations to work
	virtual void DragPointerMove( DocCoord Pos, ClickModifiers Mods, Spread* pSpread, BOOL bSolidDrag);
	virtual void DragFinished(	DocCoord Pos, ClickModifiers Mods, Spread* pSpread,	BOOL Success, BOOL bSolidDrag);

	// Some Render functions to draw the EORed path
	virtual void RenderDragBlobs( DocRect, Spread * , BOOL bSolidDrag);
		
	// These functions required for the OpDescriptor class
	static BOOL Init();
	static OpState GetState(String_256* Description, OpDescriptor*);

private:
	// Functions for in-drag processing
	BOOL ProcessCentre(DocCoord Pos, ClickModifiers Mods);
	BOOL ProcessPrimary(DocCoord Pos, ClickModifiers Mods);
	BOOL ProcessRectangle(DocCoord Pos, ClickModifiers Mods);
	BOOL ProcessRectangleDiagonal(DocCoord Pos, ClickModifiers Mods);
	BOOL ProcessRectangleEdge(DocCoord Pos, DocCoord OtherEnd);
	BOOL ProcessEllipse(DocCoord Pos, ClickModifiers Mods);
	BOOL ProcessStellation(DocCoord Pos, ClickModifiers Mods);
	BOOL ProcessRadius(DocCoord Pos, ClickModifiers Mods);
	BOOL ProcessPrimaryCurve(DocCoord Pos);
	BOOL ProcessStellationCurve(DocCoord Pos);
	void WheelConstrain(DocCoord* PointerPos, const DocCoord StartPos);
	DocCoord GetUTPosition(DocCoord TransedPos);
	BOOL IsJoinAcute(const DocCoord* pJoin, const DocCoord* pOtherEndOfJoin, const DocCoord* pOtherPoint);

	// Data members
	DocCoord	LastMousePos;				// The position of the mouse at the last mouse move message
	DocCoord	StartMousePos;				// Mouse position of initial click
	Spread*		StartSpread;				// The spread that the drag started on
	NodeRegularShape*	OriginalShape;		// The original shape 
	NodeRegularShape*	EditShape;			// and a copy of it that we can change
	Cursor* pCursor;						// Cursor to display during dragging
	INT32 CurrentCursorID;
	DragType DragObject;					// Type of drag

	DocCoord LineStart;
	DocCoord LineEnd;
	Matrix transMat;						// the current tranform from Original to edit
	DocCoord RectangleArray[4];				// Used in rectangle dragging
	BOOL DragAroundCentre;
};



/********************************************************************************************

>	class OpReformShapeEdge : public SelOperation

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/12/94
	Purpose:	This operation is started by a NodeRegularShape when someone drags on one of
				its edges.  It is used to interactively reform the path used to form the edge
				of the shape

********************************************************************************************/

class OpReformShapeEdge : public SelOperation
{

CC_DECLARE_DYNCREATE(OpReformShapeEdge)


public:
	// Construction/Destruction
	OpReformShapeEdge();

	// The all important Do functions
	void DoStartDragEdit(NodeRegularShape* OrigShape, BOOL EdgeOne, DocCoord Anchor,
											    Spread *pSpread, INT32 SegmentIndex);

	// Virtual functions needed for the dragging operations to work
	virtual void DragPointerMove( DocCoord Pos, ClickModifiers Mods, Spread* pSpread, BOOL bSolidDrag);
	virtual void DragFinished(	DocCoord Pos, ClickModifiers Mods, Spread* pSpread,	BOOL Success, BOOL bSolidDrag);

	// Some Render functions to draw the EORed path
	virtual void RenderDragBlobs( DocRect, Spread * , BOOL bSolidDrag);
		
	// These functions required for the OpDescriptor class
	static BOOL Init();
	static OpState GetState(String_256* Description, OpDescriptor*);

private:
	enum DragType {DRAG_EDGE1, DRAG_EDGE2};
	void RecalculateEdge( DocCoord Offset);
	BOOL BuildEditEdge();
	BOOL BuildEditShape(BOOL CopyToBoth, BOOL ShouldMirror);
	BOOL ReversePath(Path* NewPath, BOOL ShouldMirror);

	NodeRegularShape* EditShape;
	NodeRegularShape* OriginalShape;
	DocCoord StartMousePos;
	DocCoord UTStartMousePos;
	DocCoord OrigControl1;
	DocCoord OrigControl2;
	Spread* StartSpread;
	Path* EditEdge;
	INT32 Index;
	double paramdist;
	DragType EdgeDrag;

};



/********************************************************************************************

>	class ChangeShapeDataAction : public Action

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/12/94
	Purpose:	An action to toggle a (Boolean or double) value of a regular shape.
	SeeAlso:	ChangeShapePointAction

********************************************************************************************/

class ChangeShapeDataAction : public Action
{

CC_DECLARE_DYNCREATE(ChangeShapeDataAction)

public:

	enum ChangeItem {CHANGE_NUMSIDES, CHANGE_CIRCULAR, CHANGE_STELLATED, CHANGE_PRIMARYCURVATURE,
					 CHANGE_STELLATIONCURVATURE, CHANGE_STELLRADIUSTOPRIMARY,
					 CHANGE_PRIMARYCURVETOPRIMARY, CHANGE_STELLCURVETOSTELL, CHANGE_STELLATIONOFFSET};

	ChangeShapeDataAction();
	virtual ActionCode Execute();
	static ActionCode Init( Operation* pOp,
							ActionList* pActionList,
							NodeRegularShape* pShape,
							enum ChangeItem NewItem,
							double NewData,
							Action** NewAction);
	static ActionCode DoToggle( Operation* pOp,
								ActionList* pActionList,
								NodeRegularShape* pShape,
								enum ChangeItem NewItem,
								double NewData);

private:
	NodeRegularShape*	pToggleShape;  
	enum ChangeItem		ChangeItemID;
	double				NewValue;
};



/********************************************************************************************

>	class ChangeShapePointAction : public Action

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/94
	Purpose:	An action to change a point in a regular shape.
	SeeAlso:	ChangeShapeDataAction

********************************************************************************************/

class ChangeShapePointAction : public Action
{

CC_DECLARE_DYNCREATE(ChangeShapePointAction)

public:
	enum ChangeItem {CHANGE_MAJOR, CHANGE_CENTRE, CHANGE_MINOR};

	ChangeShapePointAction();
	virtual ActionCode Execute();
	static ActionCode Init( Operation* pOp,
							ActionList* pActionList,
							NodeRegularShape* pShape,
							enum ChangeItem NewItem,
							DocCoord NewData,
							Action** NewAction);
	static ActionCode DoToggle( Operation* pOp,
								ActionList* pActionList,
								NodeRegularShape* pShape,
								enum ChangeItem NewItem,
								DocCoord NewData);


private:
	NodeRegularShape*	pToggleShape;  
	enum ChangeItem		ChangeItemID;
	DocCoord			NewValue;
};



/********************************************************************************************

>	class ChangeShapePathAction : public Action

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/12/94
	Purpose:	An action to change a path in a regular shape.
	SeeAlso:	ChangeShapeDataAction

********************************************************************************************/

class ChangeShapePathAction : public Action
{

CC_DECLARE_DYNCREATE(ChangeShapePathAction)

public:
	enum ChangeItem {CHANGE_PATH1, CHANGE_PATH2};

	ChangeShapePathAction();
	virtual ActionCode Execute();
	static ActionCode Init( Operation* pOp,
							ActionList* pActionList,
							NodeRegularShape* pShape,
							enum ChangeItem NewItem,
							Path* NewPath,
							Action** NewAction);
	static ActionCode DoReshape( Operation* pOp,
								ActionList* pActionList,
								NodeRegularShape* pShape,
								enum ChangeItem NewItem,
								Path* NewPath);
private:
	NodeRegularShape*	pShape;  
	enum ChangeItem		ChangeItemID;
	Path				NewEdge;
};



/*********************************************************************************************
>	class ShapeEditedMsg : public Msg

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/1/95
	Purpose:	This message is sent whenever a shape is edited.
	SeeAlso:	-

*********************************************************************************************/

class ShapeEditedMsg : public Msg
{
CC_DECLARE_DYNAMIC(ShapeEditedMsg)

public:
	NodeRegularShape*	pShape;			// Points to the changed shape
	Spread* pSpread;					// Points to the spread containing the shape

	ShapeEditedMsg(NodeRegularShape* pChangeShape, Spread* pParentSpread);
};




/********************************************************************************************

>	class OpShapeToggleBase : public SelOperation

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/04/95
	Purpose:	This is a base class for operations that toggle certain states of a QuickShape
	SeeAlso:	-

********************************************************************************************/

class CCAPI OpShapeToggleBase: public SelOperation
{                           
	CC_DECLARE_DYNCREATE( OpShapeToggleBase )    
    
public:
	OpShapeToggleBase();				
	void			Do(OpDescriptor*);	

protected:
	virtual BOOL DoAction(NodeRegularShape* pShape);
};



/********************************************************************************************

>	class OpShapeToggleElipPoly : public OpShapeToggleBase

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/04/95
	Purpose:	This class is an operation that toggles the ellipse/polygon state of all
				selected QuickShapes.
	SeeAlso:	OpShapeToggleBase

********************************************************************************************/

class CCAPI OpShapeToggleElipPoly: public OpShapeToggleBase
{                           
	CC_DECLARE_DYNCREATE( OpShapeToggleElipPoly )    
    
public:
	OpShapeToggleElipPoly();				
	static BOOL		Init();					
	static OpState	GetState(String_256*, OpDescriptor*);		
protected:
	BOOL DoAction(NodeRegularShape* pShape);
};



/********************************************************************************************

>	class OpShapeToggleCurvature : public OpShapeToggleBase

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/04/95
	Purpose:	This class is an operation that toggles the curvature state of all
				selected QuickShapes.
	SeeAlso:	OpShapeToggleBase

********************************************************************************************/

class CCAPI OpShapeToggleCurvature: public OpShapeToggleBase
{                           
	CC_DECLARE_DYNCREATE( OpShapeToggleCurvature )    
    
public:
	OpShapeToggleCurvature();				
	static BOOL		Init();					
	static OpState	GetState(String_256*, OpDescriptor*);		
protected:
	BOOL DoAction(NodeRegularShape* pShape);
};



/********************************************************************************************

>	class OpShapeToggleStellation : public OpShapeToggleBase

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/04/95
	Purpose:	This class is an operation that toggles the stellation state of all
				selected QuickShapes.
	SeeAlso:	OpShapeToggleBase

********************************************************************************************/

class CCAPI OpShapeToggleStellation: public OpShapeToggleBase
{                           
	CC_DECLARE_DYNCREATE( OpShapeToggleStellation )    
    
public:
	OpShapeToggleStellation();				
	static BOOL		Init();					
	static OpState	GetState(String_256*, OpDescriptor*);		
protected:
	BOOL DoAction(NodeRegularShape* pShape);
};



/********************************************************************************************

>	class OpShapeToggleSetNumSides : public SelOperation

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/04/95
	Purpose:	This class is an operation that sets the number of sides of all
				selected QuickShapes.
	SeeAlso:	-

********************************************************************************************/

class CCAPI OpShapeToggleSetNumSides: public SelOperation
{                           
	CC_DECLARE_DYNCREATE( OpShapeToggleSetNumSides )    
    
public:
	OpShapeToggleSetNumSides();				
	static OpState	GetState(String_256*, OpDescriptor*);
	static BOOL		Init();					
			
protected:
	BOOL DoAction(INT32 NewNumSides);
};



/********************************************************************************************

>	class OpShapeToggleSetNumSides3 : public OpShapeToggleSetNumSides

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/04/95
	Purpose:	This class is an operation that sets the number of sides of all
				selected QuickShapes.
	SeeAlso:	-

********************************************************************************************/

class CCAPI OpShapeToggleSetNumSides3: public OpShapeToggleSetNumSides
{                           
	CC_DECLARE_DYNCREATE( OpShapeToggleSetNumSides3 )    
    
public:
	OpShapeToggleSetNumSides3() {};				
	void Do(OpDescriptor*);	
};



/********************************************************************************************

>	class OpShapeToggleSetNumSides4 : public OpShapeToggleSetNumSides

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/04/95
	Purpose:	This class is an operation that sets the number of sides of all
				selected QuickShapes.
	SeeAlso:	-

********************************************************************************************/

class CCAPI OpShapeToggleSetNumSides4: public OpShapeToggleSetNumSides
{                           
	CC_DECLARE_DYNCREATE( OpShapeToggleSetNumSides4 )    
    
public:
	OpShapeToggleSetNumSides4() {};				
	void Do(OpDescriptor*);	
};



/********************************************************************************************

>	class OpShapeToggleSetNumSides5 : public OpShapeToggleSetNumSides

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/04/95
	Purpose:	This class is an operation that sets the number of sides of all
				selected QuickShapes.
	SeeAlso:	-

********************************************************************************************/

class CCAPI OpShapeToggleSetNumSides5: public OpShapeToggleSetNumSides
{                           
	CC_DECLARE_DYNCREATE( OpShapeToggleSetNumSides5 )    
    
public:
	OpShapeToggleSetNumSides5() {};				
	void Do(OpDescriptor*);	
};



/********************************************************************************************

>	class OpShapeToggleSetNumSides6 : public OpShapeToggleSetNumSides

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/04/95
	Purpose:	This class is an operation that sets the number of sides of all
				selected QuickShapes.
	SeeAlso:	-

********************************************************************************************/

class CCAPI OpShapeToggleSetNumSides6: public OpShapeToggleSetNumSides
{                           
	CC_DECLARE_DYNCREATE( OpShapeToggleSetNumSides6 )    
    
public:
	OpShapeToggleSetNumSides6() {};				
	void Do(OpDescriptor*);	
};



/********************************************************************************************

>	class OpShapeToggleSetNumSides7 : public OpShapeToggleSetNumSides

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/04/95
	Purpose:	This class is an operation that sets the number of sides of all
				selected QuickShapes.
	SeeAlso:	-

********************************************************************************************/

class CCAPI OpShapeToggleSetNumSides7: public OpShapeToggleSetNumSides
{                           
	CC_DECLARE_DYNCREATE( OpShapeToggleSetNumSides7 )    
    
public:
	OpShapeToggleSetNumSides7() {};				
	void Do(OpDescriptor*);	
};



/********************************************************************************************

>	class OpShapeToggleSetNumSides8 : public OpShapeToggleSetNumSides

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/04/95
	Purpose:	This class is an operation that sets the number of sides of all
				selected QuickShapes.
	SeeAlso:	-

********************************************************************************************/

class CCAPI OpShapeToggleSetNumSides8: public OpShapeToggleSetNumSides
{                           
	CC_DECLARE_DYNCREATE( OpShapeToggleSetNumSides8 )    
    
public:
	OpShapeToggleSetNumSides8() {};				
	void Do(OpDescriptor*);	
};



/********************************************************************************************

>	class OpShapeToggleSetNumSides9 : public OpShapeToggleSetNumSides

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/04/95
	Purpose:	This class is an operation that sets the number of sides of all
				selected QuickShapes.
	SeeAlso:	-

********************************************************************************************/

class CCAPI OpShapeToggleSetNumSides9: public OpShapeToggleSetNumSides
{                           
	CC_DECLARE_DYNCREATE( OpShapeToggleSetNumSides9 )    
    
public:
	OpShapeToggleSetNumSides9() {};				
	void Do(OpDescriptor*);	
};



/********************************************************************************************

>	class OpShapeToggleSetNumSides10 : public OpShapeToggleSetNumSides

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/04/95
	Purpose:	This class is an operation that sets the number of sides of all
				selected QuickShapes.
	SeeAlso:	-

********************************************************************************************/

class CCAPI OpShapeToggleSetNumSides10: public OpShapeToggleSetNumSides
{                           
	CC_DECLARE_DYNCREATE( OpShapeToggleSetNumSides10 )    
    
public:
	OpShapeToggleSetNumSides10() {};				
	void Do(OpDescriptor*);	
};



/********************************************************************************************

>	class OpShapeMakeRegular : public SelOperation

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/04/95
	Purpose:	This is an operation that makes one QuickShape regular
	SeeAlso:	-

********************************************************************************************/

class CCAPI OpShapeMakeRegular: public SelOperation
{                           
	CC_DECLARE_DYNCREATE( OpShapeMakeRegular )    
    
public:
	OpShapeMakeRegular() {};
	static OpState	GetState(String_256*, OpDescriptor*);
	static BOOL		Init();					
	void			DoMakeRegular(NodeRegularShape* pShape, DocCoord RegularPoint);	

protected:
	BOOL DoActions(NodeRegularShape* pShape, DocCoord RegularPoint);

};




#endif  // INC_SHAPEOPS
