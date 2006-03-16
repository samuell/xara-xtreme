// $Id: shapeops.cpp 662 2006-03-14 21:31:49Z alex $
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

#include "camtypes.h"
#include "shapeops.h"

#include "app.h"
#include "blobs.h"
#include "csrstack.h"
#include "docview.h"
#include "nodepath.h"
#include "nodershp.h"
#include "pathedit.h"
//#include "peter.h"
#include "progress.h"
//#include "resource.h"
#include "rndrgn.h"
#include "tool.h"
#include "trans2d.h"
//#include "viewrc.h"
#include "objchge.h"
#include "spread.h"

#define PATRACE if(IsUserName("Peter")) TRACE

// Useful macro, given a pointer to a CCObject (or an object derived
// from it) and a string format, it will output the class name of the object. 
// Example usage PACLASSNAME(CurrentNode,"Node class = %s\n");
// N.B. No checking is node or parameters.  Note semi-colon on end.
#define PACLASSNAME(a,b) CCRuntimeClass* Blob; Blob = a->GetRuntimeClass(); PATRACE(b,Blob->m_lpszClassName)


DECLARE_SOURCE( "$Revision: 662 $" );


CC_IMPLEMENT_MEMDUMP(EditRegularShapeParam, OpParam)	
CC_IMPLEMENT_DYNCREATE(OpEditRegularShape, SelOperation)
CC_IMPLEMENT_DYNCREATE(OpDragRegularShape, SelOperation)
CC_IMPLEMENT_DYNCREATE(OpReformShapeEdge, SelOperation)
CC_IMPLEMENT_DYNCREATE(ChangeShapeDataAction, Action)
CC_IMPLEMENT_DYNCREATE(ChangeShapePointAction, Action)
CC_IMPLEMENT_DYNCREATE(ChangeShapePathAction, Action)
CC_IMPLEMENT_DYNAMIC(ShapeEditedMsg, Msg)
CC_IMPLEMENT_DYNCREATE(OpShapeToggleBase, SelOperation)
CC_IMPLEMENT_DYNCREATE(OpShapeToggleElipPoly, OpShapeToggleBase)
CC_IMPLEMENT_DYNCREATE(OpShapeToggleStellation, OpShapeToggleBase)
CC_IMPLEMENT_DYNCREATE(OpShapeToggleCurvature, OpShapeToggleBase)
CC_IMPLEMENT_DYNCREATE(OpShapeMakeRegular, SelOperation)

CC_IMPLEMENT_DYNCREATE( OpShapeToggleSetNumSides, SelOperation )    
CC_IMPLEMENT_DYNCREATE( OpShapeToggleSetNumSides3, OpShapeToggleSetNumSides )    
CC_IMPLEMENT_DYNCREATE( OpShapeToggleSetNumSides4, OpShapeToggleSetNumSides )    
CC_IMPLEMENT_DYNCREATE( OpShapeToggleSetNumSides5, OpShapeToggleSetNumSides )    
CC_IMPLEMENT_DYNCREATE( OpShapeToggleSetNumSides6, OpShapeToggleSetNumSides )    
CC_IMPLEMENT_DYNCREATE( OpShapeToggleSetNumSides7, OpShapeToggleSetNumSides )    
CC_IMPLEMENT_DYNCREATE( OpShapeToggleSetNumSides8, OpShapeToggleSetNumSides )    
CC_IMPLEMENT_DYNCREATE( OpShapeToggleSetNumSides9, OpShapeToggleSetNumSides )    
CC_IMPLEMENT_DYNCREATE( OpShapeToggleSetNumSides10, OpShapeToggleSetNumSides )    



// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW

const DocCoord DontCare (0,0);

/********************************************************************************************

>	ShapeEditedMsg::ShapeEditedMsg(NodeRegularShape* pChangeShape, Spread* pParentSpread) 

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/1/95
	Inputs:		pChangeShape - pointer to the edited shape
				pParentSpread - pointer to the parent spread of the object
	Purpose:	initilise a ShapeEditedMsg
	SeeAlso:	ShapeEditedMsg

********************************************************************************************/
ShapeEditedMsg::ShapeEditedMsg(NodeRegularShape* pChangeShape, Spread* pParentSpread)							
{                              
	pShape = pChangeShape;
	pSpread = pParentSpread;
}


//////////////////////////////////////////////////////////////////////////////////////////////
//                                     EditRegularShapeParam                                //
//////////////////////////////////////////////////////////////////////////////////////////////



/********************************************************************************************

>	EditRegularShapeParam::EditRegularShapeParam(NodeRegularShape* ChangeShapes) 

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/12/94	
	Inputs:		ChangeShapes - pointer to the shape to change OR NULL to change all selected shapes									  
	Purpose:	EditRegularShapeParam constructor - sets all member items to unchange.
				After constructing the object, the caller should set those items that it wants
				OpEditRegularShape to change.

********************************************************************************************/
EditRegularShapeParam::EditRegularShapeParam(NodeRegularShape* ChangeShapes)								
{                              
	ShapeToChange = ChangeShapes;
	ShapesToAffect = AFFECT_ALL;
	
	NewEdgePath1 = NULL;	
	NewEdgePath2 = NULL;	
	NewNumSides = 0;		
	NewCircular = CHANGE_DONTCARE;
	NewStellated = CHANGE_DONTCARE;
	NewPrimaryCurvature = CHANGE_DONTCARE;
	NewStellationCurvature = CHANGE_DONTCARE;
	NewStellRadiusToPrimary = -1.0;	
	NewPrimaryCurveToPrimary = -1.0;	
	NewStellCurveToStell = -1.0;		
	NewStellOffsetRatio = -1000.0;
//	NewCentrePoint = DontCare;		
//	NewMajorAxes = DontCare;		
//	NewMinorAxes = DontCare;		

	TranslateTo = DontCare;		
	TranslateBy = DontCare;		
	SetRadiusLength = -1;
	AddRadiusLength = -1;
	RotateTo = -1000.0;			
	RotateBy = -1000.0;			
	SetStellationLength = -1;	
	AddStellationLength = -1;	
	SetStellationOffset = -1000.0;
	AddStellationOffset = -1000.0;
	AddPrimaryCurvature = 0.0;
	AddStellationCurvature = 0.0;
	SetMajorAxisLength = 0;
	AddMajorAxisLength = 0;
	SetMinorAxisLength = 0;
	AddMinorAxisLength = 0;
}



//////////////////////////////////////////////////////////////////////////////////////////////
//                                     OpEditRegularShape                                   //
//////////////////////////////////////////////////////////////////////////////////////////////

/********************************************************************************************

>	OpEditRegularShape::OpEditRegularShape() 

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/12/94										  
	Purpose:	OpEditRegularShape constructor - does nothing itself

********************************************************************************************/
OpEditRegularShape::OpEditRegularShape(): SelOperation()								
{                              
}



 /********************************************************************************************

>	BOOL OpEditRegularShape::Init()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/12/94
	Returns:	TRUE if the operation could be successfully initialised 
				FALSE if no more memory could be allocated 
	Purpose:	OpEditRegularShape initialiser method
	Errors:		ERROR will be called if there was insufficient memory to allocate the 
				operation.
	SeeAlso:	-

********************************************************************************************/
BOOL OpEditRegularShape::Init()
{
	BOOL Success = RegisterOpDescriptor(0, 
										_R(IDS_EDITREGULARSHAPEOP),
										CC_RUNTIME_CLASS(OpEditRegularShape), 
										OPTOKEN_EDITREGULARSHAPE,
										OpEditRegularShape::GetState);

	if (Success)
		Success = OpShapeToggleElipPoly::Init();
	if (Success)
		Success = OpShapeToggleStellation::Init();
	if (Success)
		Success = OpShapeToggleCurvature::Init();
	if (Success)
		Success = OpShapeToggleSetNumSides::Init();
   	if (Success)
		Success = OpShapeMakeRegular::Init();

	return Success;
}               
    


/********************************************************************************************

>	OpState	OpEditRegularShape::GetState(String_256*, OpDescriptor*)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/12/94
	Returns:	The state of the OpEditRegularShape
	Purpose:	For finding the OpEditRegularShape's state. 
	SeeAlso:	-

********************************************************************************************/
OpState	OpEditRegularShape::GetState(String_256* UIDescription, OpDescriptor*)
{
	OpState OpSt;

   	return OpSt;                                 
}



/********************************************************************************************

>	void OpEditRegularShape::DoWithParam(OpDescriptor* pOpDesc, OpParam* pEntryOpParam)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/12/94
	Inputs:		OpDescriptor (unused)
				pOpParam - pointer to the parameter object.
	Purpose:	Performs the edit regular shapes operation. 
	Errors:		-
	SeeAlso:	EditRegularShapeParam, OpEditRegularShape::CarryOut

********************************************************************************************/
void OpEditRegularShape::DoWithParam(OpDescriptor* pOpDesc, OpParam* pEntryOpParam)
{   
	EditRegularShapeParam* pOpParam = (EditRegularShapeParam*)pEntryOpParam;
	
	// Tell the operation system its kickoff time.
	if (DoStartSelOp(FALSE))
	{
		BOOL Complete = TRUE;

		// Data used by AllowOp() to ask the node if it's ok to do the op
		ObjChangeFlags cFlags;
		ObjChangeParam ObjChange(OBJCHANGE_STARTING,cFlags,NULL,this);

		// Either change just one shape or all selected shapes.
		if (pOpParam->ShapeToChange != NULL)
		{
			if (ShouldDoThisShape(pOpParam->ShapeToChange, pOpParam->ShapesToAffect))
			{
				if (pOpParam->ShapeToChange->AllowOp(&ObjChange))					// Will node allow the op?
					Complete = CarryOut(pOpParam->ShapeToChange, pOpParam);
			}
		}
		else
		{
			// Obtain the current selections and the first node in the selection
			SelRange*	Selected = GetApplication()->FindSelection();
			Node*		pNode = Selected->FindFirst();

			while ((pNode != NULL) && Complete)
			{
				if (IS_A(pNode,NodeRegularShape))
				{
					if (ShouldDoThisShape((NodeRegularShape*)pNode, pOpParam->ShapesToAffect))
					{
						if (pNode->AllowOp(&ObjChange))								// Will node allow the op?
							Complete = CarryOut((NodeRegularShape*)pNode, pOpParam);
					}
				}
				pNode = Selected->FindNext(pNode);
			}
		}

		GetApplication()->FindSelection()->Update(FALSE);
	
		if (Complete)
		{
			// updated effected parents
			ObjChange.Define(OBJCHANGE_FINISHED,cFlags,NULL,this);
			Complete = UpdateChangedNodes(&ObjChange);
		}

		// See if anything failed
		if (!Complete)
		{
			FailAndExecute();
		}
	}
	else
	{
		FailAndExecute();
	}

	End();
}



/********************************************************************************************

>	BOOL OpEditRegularShape::CarryOut(NodeRegularShape*	ShapeToEdit, EditRegularShapeParam* NewShapeData)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/12/94
	Inputs:		ShapeToEdit points to the Shape to change.
				NewShapeData points to the parameter object with the changes data.
	Purpose:	Performs the edit regular shapes operation on one particular shape
	Errors:		ERROR2 if there is no current DocView
	SeeAlso:	EditRegularShapeParam, OpEditRegularShape::DoWithParam

********************************************************************************************/

BOOL OpEditRegularShape::CarryOut(NodeRegularShape*	ShapeToEdit, EditRegularShapeParam* NewShapeData)
{
	// Create an action to will cause the shape to recalc its bounds
	if (RecalcBoundsAction::DoRecalc(this, &UndoActions, ShapeToEdit) == AC_FAIL)
	{
		return FALSE;
	}

	// See if we should change the polygon/circular flag
	if (NewShapeData->NewCircular != EditRegularShapeParam::CHANGE_DONTCARE)
	{
		if (ChangeShapeDataAction::DoToggle(this, &UndoActions, ShapeToEdit, ChangeShapeDataAction::CHANGE_CIRCULAR,
												ShapeToEdit->IsCircular() ? 1.0 : 0.0) == AC_FAIL)
		{
			return FALSE;
		}
		ShapeToEdit->SetCircular(NewShapeData->NewCircular == EditRegularShapeParam::CHANGE_SETTRUE);
	}

	// See if we should change the stellated flag
	if (NewShapeData->NewStellated != EditRegularShapeParam::CHANGE_DONTCARE)
	{
/*		// If were turning off stellation them we may need to fix the primary curvature ratio
		if (NewShapeData->NewStellated == EditRegularShapeParam::CHANGE_SETFALSE)
		{
			if (ShapeToEdit->GetPrimaryCurveToPrimary() > 0.5)
				NewShapeData->NewPrimaryCurveToPrimary = 0.2;
		}
		else
		{
			// If we've turned on stellation them we may need to fix both curvature ratios
			if (ShapeToEdit->GetPrimaryCurveToPrimary()+ShapeToEdit->GetStellCurveToStell() > 1.0)
			{
				NewShapeData->NewPrimaryCurveToPrimary = 0.2;
				NewShapeData->NewStellCurveToStell = 0.2;
			}
		}*/

		if (ChangeShapeDataAction::DoToggle(this, &UndoActions, ShapeToEdit, ChangeShapeDataAction::CHANGE_STELLATED,
												 ShapeToEdit->IsStellated() ? 1.0 : 0.0) == AC_FAIL)
		{
			return FALSE;
		}
		ShapeToEdit->SetStellated(NewShapeData->NewStellated == EditRegularShapeParam::CHANGE_SETTRUE);
	}

	// See if we should change the primary curvature flag
	if (NewShapeData->NewPrimaryCurvature != EditRegularShapeParam::CHANGE_DONTCARE)
	{
		if (ChangeShapeDataAction::DoToggle(this, &UndoActions, ShapeToEdit, ChangeShapeDataAction::CHANGE_PRIMARYCURVATURE,
												 ShapeToEdit->IsPrimaryCurvature() ? 1.0 : 0.0) == AC_FAIL)
		{
			return FALSE;
		}
		ShapeToEdit->SetPrimaryCurvature(NewShapeData->NewPrimaryCurvature == EditRegularShapeParam::CHANGE_SETTRUE);
	}

	// See if we should change the stellation curvature flag
	if (NewShapeData->NewStellationCurvature != EditRegularShapeParam::CHANGE_DONTCARE)
	{
		if (ChangeShapeDataAction::DoToggle(this, &UndoActions, ShapeToEdit, ChangeShapeDataAction::CHANGE_STELLATIONCURVATURE,
												 ShapeToEdit->IsStellationCurvature() ? 1.0 : 0.0) == AC_FAIL)
		{
			return FALSE;
		}
		ShapeToEdit->SetStellationCurvature(NewShapeData->NewStellationCurvature == EditRegularShapeParam::CHANGE_SETTRUE);
	}

	// See if we should change the stellation offset
	if (NewShapeData->NewStellOffsetRatio != -1000.0)
	{
		if (ChangeShapeDataAction::DoToggle(this, &UndoActions, ShapeToEdit, 
				ChangeShapeDataAction::CHANGE_STELLATIONOFFSET, ShapeToEdit->GetStellationRatio()) == AC_FAIL)
		{
			return FALSE;
		}
		ShapeToEdit->SetStellationRatio(NewShapeData->NewStellOffsetRatio);
	}

	// See if we should change the number of sides
	if (NewShapeData->NewNumSides != 0)
	{
		if (ChangeShapeDataAction::DoToggle(this, &UndoActions, ShapeToEdit, 
				ChangeShapeDataAction::CHANGE_NUMSIDES, ShapeToEdit->GetNumSides()) == AC_FAIL)
		{
			return FALSE;
		}
		ShapeToEdit->SetNumSides(NewShapeData->NewNumSides);
	}

	// See if we should change the stellation radius ratio
	if (NewShapeData->NewStellRadiusToPrimary != -1.0)
	{
		if (ChangeShapeDataAction::DoToggle(this, &UndoActions, ShapeToEdit, 
				ChangeShapeDataAction::CHANGE_STELLRADIUSTOPRIMARY, 
											ShapeToEdit->GetStellRadiusToPrimary()) == AC_FAIL)
		{
			return FALSE;
		}
		ShapeToEdit->SetStellRadiusToPrimary(NewShapeData->NewStellRadiusToPrimary);
	}

	// See if we should change the primary curvature ratio
	if (NewShapeData->NewPrimaryCurveToPrimary != -1.0)
	{
		double NewRatio = NewShapeData->NewPrimaryCurveToPrimary;
/*		if (ShapeToEdit->IsStellated())
		{
			double StellRatio = ShapeToEdit->GetStellCurveToStell();
			if (NewShapeData->NewStellCurveToStell != -1.0)
				StellRatio = NewShapeData->NewStellCurveToStell;	

			NewRatio = __min(1.0-StellRatio, NewShapeData->NewPrimaryCurveToPrimary); 
		}
		else
		{
			NewRatio = __min(0.5, NewShapeData->NewPrimaryCurveToPrimary); 
		}*/
		
		if (ChangeShapeDataAction::DoToggle(this, &UndoActions, ShapeToEdit, 
				ChangeShapeDataAction::CHANGE_PRIMARYCURVETOPRIMARY, 
											ShapeToEdit->GetPrimaryCurveToPrimary()) == AC_FAIL)
		{
			return FALSE;
		}
		ShapeToEdit->SetPrimaryCurveToPrimary(NewRatio);
	}

	// See if we should bump the primary curvature ratio
	if (NewShapeData->AddPrimaryCurvature != -0.0)
	{
		double NewRatio = ShapeToEdit->GetPrimaryCurveToPrimary() + NewShapeData->AddPrimaryCurvature;
/*		if (ShapeToEdit->IsStellated())
		{
			NewRatio = __min(1.0-ShapeToEdit->GetStellCurveToStell(), NewRatio);
		}
		else
		{
			NewRatio = __min(0.5, NewRatio); 
		}*/
		NewRatio = max( 0.0, NewRatio );
		
		if (ChangeShapeDataAction::DoToggle(this, &UndoActions, ShapeToEdit, 
				ChangeShapeDataAction::CHANGE_PRIMARYCURVETOPRIMARY, 
											ShapeToEdit->GetPrimaryCurveToPrimary()) == AC_FAIL)
		{
			return FALSE;
		}
		ShapeToEdit->SetPrimaryCurveToPrimary(NewRatio);
	}

	// See if we should change the stellation curvature ratio
	if (NewShapeData->NewStellCurveToStell != -1.0)
	{
		double NewRatio = NewShapeData->NewStellCurveToStell;
		
//		NewRatio = __min (1.0-ShapeToEdit->GetPrimaryCurveToPrimary(), NewShapeData->NewStellCurveToStell); 

		if (ChangeShapeDataAction::DoToggle(this, &UndoActions, ShapeToEdit, 
				ChangeShapeDataAction::CHANGE_STELLCURVETOSTELL, 
											ShapeToEdit->GetStellCurveToStell()) == AC_FAIL)
		{
			return FALSE;
		}
		ShapeToEdit->SetStellCurveToStell(NewRatio);
	}

	// See if we should bump the stellation curvature ratio
	if (NewShapeData->AddStellationCurvature != 0.0)
	{
		double NewRatio = ShapeToEdit->GetStellCurveToStell() + NewShapeData->AddStellationCurvature;
//		NewRatio = __min(1.0-ShapeToEdit->GetPrimaryCurveToPrimary(), NewRatio); 
		NewRatio = max( 0.0, NewRatio );
		
		if (ChangeShapeDataAction::DoToggle(this, &UndoActions, ShapeToEdit, 
				ChangeShapeDataAction::CHANGE_PRIMARYCURVETOPRIMARY, 
											ShapeToEdit->GetStellCurveToStell()) == AC_FAIL)
		{
			return FALSE;
		}
		ShapeToEdit->SetStellCurveToStell(NewRatio);
	}

	// See if we should change the minor axes
/*	if (NewShapeData->NewMinorAxes != DontCare)
	{
		if (ChangeShapePointAction::DoToggle(this, &UndoActions, ShapeToEdit, 
						ChangeShapePointAction::CHANGE_MINOR, ShapeToEdit->GetMinorAxes()) == AC_FAIL)
		{
			return FALSE;
		}
		ShapeToEdit->SetMinorAxes(NewShapeData->NewMinorAxes);
	}

	// See if we should change the major axes
	if (NewShapeData->NewMajorAxes != DontCare)
	{
		if (ChangeShapePointAction::DoToggle(this, &UndoActions, ShapeToEdit, 
						ChangeShapePointAction::CHANGE_MAJOR, ShapeToEdit->GetMajorAxes()) == AC_FAIL)
		{
			return FALSE;
		}
		ShapeToEdit->SetMajorAxes(NewShapeData->NewMajorAxes);
	}*/

	// See if we should change the size of the shape to a new length
	// OR see if we should change the size of the shape by a specified ammount
	if ((NewShapeData->SetRadiusLength != -1) || (NewShapeData->AddRadiusLength != -1) )
	{
		// Get scaling factor
		fixed16 ScaleFactor;
		const double CurrentLength = ShapeToEdit->GetCentrePoint().Distance(ShapeToEdit->GetMajorAxes()) ;
		if (CurrentLength != 0.0)
		{
			if (NewShapeData->SetRadiusLength != -1)
			{
				double RequiredLength = NewShapeData->SetRadiusLength;
				ScaleFactor = RequiredLength / CurrentLength;
			}
			else
			{
				double RequiredAddition = NewShapeData->AddRadiusLength;
				ScaleFactor = (RequiredAddition + CurrentLength) / CurrentLength;
			}		
		}
		else
			ScaleFactor = 1.0;
		
		// Translate to origin, scale and translate back
		Trans2DMatrix* TransOriginMat = new Trans2DMatrix(-ShapeToEdit->GetCentrePoint().x, -ShapeToEdit->GetCentrePoint().y);
		Trans2DMatrix* ScaleMat = new Trans2DMatrix(Matrix(ScaleFactor,ScaleFactor));
		Trans2DMatrix* TransBackMat = new Trans2DMatrix(ShapeToEdit->GetCentrePoint().x, ShapeToEdit->GetCentrePoint().y);
		if (!(DoTransformNode(ShapeToEdit, TransOriginMat) && DoTransformNode(ShapeToEdit, ScaleMat)
															&& DoTransformNode(ShapeToEdit, TransBackMat) ))
		{
			return FALSE;
		}
	}

	// See if we should change the MajorAxis of the shape to a new length
	// OR see if we should change the MajorAxis of the shape by a specified ammount
	if ((NewShapeData->SetMajorAxisLength != 0) || (NewShapeData->AddMajorAxisLength != 0) )
	{
		// Get scaling factor
		fixed16 ScaleFactor;
		const double CurrentLength = ShapeToEdit->GetCentrePoint().Distance(ShapeToEdit->GetMajorAxes()) ;
		if (CurrentLength != 0.0)
		{
			if (NewShapeData->SetMajorAxisLength != 0)
			{
				double RequiredLength = NewShapeData->SetMajorAxisLength;
				ScaleFactor = RequiredLength / CurrentLength;
			}
			else
			{
				double RequiredAddition = NewShapeData->AddMajorAxisLength;
				ScaleFactor = (RequiredAddition + CurrentLength) / CurrentLength;
			}		
		}
		else
			ScaleFactor = 1.0;
		
		// Translate to origin, rotate so major axis flat, scale, then rotate and translate back
		Matrix TransMat(-ShapeToEdit->GetCentrePoint().x, -ShapeToEdit->GetCentrePoint().y);
		DocCoord Offset = ShapeToEdit->GetMajorAxes() - ShapeToEdit->GetCentrePoint();
		double Angle = atan2((double)Offset.y, (double)Offset.x)*(180/PI);
		TransMat*=Matrix(-ANGLE(Angle));
		TransMat*=Matrix(ScaleFactor, 1.0);
		TransMat*=Matrix(ANGLE(Angle));
		TransMat*=Matrix(ShapeToEdit->GetCentrePoint().x, ShapeToEdit->GetCentrePoint().y);

		Trans2DMatrix* Trans2DMat = new Trans2DMatrix(TransMat);
		if (Trans2DMat == NULL)
			return FALSE;
		if (!DoTransformNode(ShapeToEdit, Trans2DMat))
			return FALSE;
	}

	// See if we should change the MinorAxis of the shape to a new length
	// OR see if we should change the MinorAxis of the shape by a specified ammount
	if ((NewShapeData->SetMinorAxisLength != 0) || (NewShapeData->AddMinorAxisLength != 0) )
	{
		// Get scaling factor
		fixed16 ScaleFactor;
		const double CurrentLength = ShapeToEdit->GetCentrePoint().Distance(ShapeToEdit->GetMinorAxes()) ;
		if (CurrentLength != 0.0)
		{
			if (NewShapeData->SetMinorAxisLength != 0)
			{
				double RequiredLength = NewShapeData->SetMinorAxisLength;
				ScaleFactor = RequiredLength / CurrentLength;
			}
			else
			{
				double RequiredAddition = NewShapeData->AddMinorAxisLength;
				ScaleFactor = (RequiredAddition + CurrentLength) / CurrentLength;
			}		
		}
		else
			ScaleFactor = 1.0;
		
		// Translate to origin, rotate so minor axis flat, scale, then rotate and translate back
		Matrix TransMat(-ShapeToEdit->GetCentrePoint().x, -ShapeToEdit->GetCentrePoint().y);
		DocCoord Offset = ShapeToEdit->GetMinorAxes() - ShapeToEdit->GetCentrePoint();
		double Angle = atan2((double)Offset.y, (double)Offset.x)*(180/PI);
		TransMat*=Matrix(-ANGLE(Angle));
		TransMat*=Matrix(ScaleFactor, 1.0);
		TransMat*=Matrix(ANGLE(Angle));
		TransMat*=Matrix(ShapeToEdit->GetCentrePoint().x, ShapeToEdit->GetCentrePoint().y);

		Trans2DMatrix* Trans2DMat = new Trans2DMatrix(TransMat);
		if (Trans2DMat == NULL)
			return FALSE;
		if (!DoTransformNode(ShapeToEdit, Trans2DMat))
			return FALSE;
	}

	// See if we should change the size of the stellation radius to a new length
	// OR see if we should change the size of the stellation radius by a specified ammount
	if ((NewShapeData->SetStellationLength != -1) || (NewShapeData->AddStellationLength != -1) )
	{
		const double CurrentLength = ShapeToEdit->GetCentrePoint().Distance(ShapeToEdit->GetMajorAxes()) ;
		double NewRatio;
		if (NewShapeData->SetStellationLength != -1)
		{
			// Get the current length of the shape and get the ratio from that
			NewRatio = double( min( fabs( NewShapeData->SetStellationLength / CurrentLength ), 1.0 ) );
		}
		else
		{
			double NewLength = fabs(CurrentLength - NewShapeData->AddStellationLength);
			NewRatio = double( min( fabs( ( NewLength / CurrentLength ) - 1 + ShapeToEdit->GetStellRadiusToPrimary() ), 1.0 ) );
		}

		if (ChangeShapeDataAction::DoToggle(this, &UndoActions, ShapeToEdit, 
				ChangeShapeDataAction::CHANGE_STELLRADIUSTOPRIMARY, 
											ShapeToEdit->GetStellRadiusToPrimary()) == AC_FAIL)
		{
			return FALSE;
		}
		ShapeToEdit->SetStellRadiusToPrimary(NewRatio);
	}

	// See if we should rotate the shape to a specific angle
	// OR see if we should rotate the shape by a specified ammount
	if ((NewShapeData->RotateTo != -1000.0) || (NewShapeData->RotateBy != -1000.0) )
	{
		// Get rotate matrix
		ANGLE RotateBy;
		const DocCoord MajorOrigin = ShapeToEdit->GetMajorAxes() - ShapeToEdit->GetCentrePoint();
		const double CurrentAngle = atan2((double)MajorOrigin.y, (double)MajorOrigin.x) ;
		if (NewShapeData->RotateTo != -1000.0)
		{
			RotateBy = -(CurrentAngle - NewShapeData->RotateTo) * (180/PI); 
		}
		else
		{                                                                  
			RotateBy = NewShapeData->RotateBy * (180/PI); 
		}		
		
		Trans2DMatrix* RotateMat = new Trans2DMatrix(ShapeToEdit->GetCentrePoint(), RotateBy);
		if (!DoTransformNode(ShapeToEdit, RotateMat))
		{
			return FALSE;
		}
	}

	// See if we should change the stellation offset to a new angle
	// OR see if we should change the stellation offset by a specified ammount
	if ((NewShapeData->SetStellationOffset != -1000.0) || (NewShapeData->AddStellationOffset != -1000.0) )
	{
		const UINT32 NumSides = ShapeToEdit->GetNumSides();
		const double OppositeRatio = 180.0/(360.0/NumSides);
		double NewRatio;
		if (NewShapeData->SetStellationOffset != -1000.0)
		{
			NewRatio = OppositeRatio * (NewShapeData->SetStellationOffset / 180.0);
		}
		else
		{
			double NewOffset = (ShapeToEdit->GetStellationRatio()/OppositeRatio)*180.0 + NewShapeData->AddStellationOffset;

			if (NewOffset < -180.0)
				NewOffset = 180.0 + (NewOffset + 180.0);
			if (NewOffset > 180.0)
				NewOffset = -180.0 + (NewOffset - 180.0);
			NewRatio = OppositeRatio * (NewOffset / 180.0);
		}

		if (ChangeShapeDataAction::DoToggle(this, &UndoActions, ShapeToEdit, 
				ChangeShapeDataAction::CHANGE_STELLATIONOFFSET, ShapeToEdit->GetStellationRatio()) == AC_FAIL)
		{
			return FALSE;
		}
		ShapeToEdit->SetStellationRatio(NewRatio);
	}

	// See if we should change the centre point
/*	if (NewShapeData->NewCentrePoint != DontCare)
	{
		if (ChangeShapePointAction::DoToggle(this, &UndoActions, ShapeToEdit, 
					ChangeShapePointAction::CHANGE_CENTRE, ShapeToEdit->GetCentrePoint()) == AC_FAIL)
		{
			return FALSE;
		}
		ShapeToEdit->SetCentrePoint(NewShapeData->NewCentrePoint);
	}*/

	// See if we should translate the shape to an exact location
	if (NewShapeData->TranslateTo != DontCare)
	{
		DocCoord Offset = NewShapeData->TranslateTo - ShapeToEdit->GetCentrePoint() ;

		Trans2DMatrix* TransMat = new Trans2DMatrix(Offset.x, Offset.y);
		if (!DoTransformNode(ShapeToEdit, TransMat))
			return FALSE;
	}

	// See if we should translate the shape by an offset
	if (NewShapeData->TranslateBy != DontCare)
	{
		Trans2DMatrix* TransMat = new Trans2DMatrix(NewShapeData->TranslateBy.x, NewShapeData->TranslateBy.y);
		if (!DoTransformNode(ShapeToEdit, TransMat))
			return FALSE;
	}

	// See if we should change edge path one
	if (NewShapeData->NewEdgePath1 != NULL)
	{
		if (ChangeShapePathAction::DoReshape(this, &UndoActions, ShapeToEdit, 
					ChangeShapePathAction::CHANGE_PATH1, &(ShapeToEdit->EdgePath1)) == AC_FAIL)
		{
			return FALSE;
		}
		ShapeToEdit->EdgePath1.ClearPath();
		ShapeToEdit->EdgePath1.CopyPathDataFrom(NewShapeData->NewEdgePath1);		
	}

	// See if we should change edge path two
	if (NewShapeData->NewEdgePath2 != NULL)
	{
		if (ChangeShapePathAction::DoReshape(this, &UndoActions, ShapeToEdit, 
					ChangeShapePathAction::CHANGE_PATH2, &(ShapeToEdit->EdgePath2)) == AC_FAIL)
		{
			return FALSE;
		}
		ShapeToEdit->EdgePath2.ClearPath();
		ShapeToEdit->EdgePath2.CopyPathDataFrom(NewShapeData->NewEdgePath2);		
	}

	// Invalidate the shapes bounding rect as it's changed
	ShapeToEdit->InvalidateBoundingRect();
	ShapeToEdit->InvalidateCache();

	// Cause a redraw of the shape with its end bounds
	if (RecordBoundsAction::DoRecord(this, &UndoActions, ShapeToEdit) == AC_FAIL)
	{
		return FALSE;
	}

	return TRUE;
}



/********************************************************************************************

>	BOOL OpEditRegularShape::ShouldDoThisShape(NodeRegularShape* pShape, EditRegularShapeParam::AffectShape Allowance)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/03/95
	Inputs:		pShape - points to a regular shape 
				Allowance - the current shape allowance mode
	Outputs:	-
	Returns:	TRUE if this shape should be changed in this operation
				FALSE if this shape should not be changed
	Purpose:	Do decide wether or not the shape should be edited.  The decision is based upon
				the Allowance mode.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
BOOL OpEditRegularShape::ShouldDoThisShape(NodeRegularShape* pShape, EditRegularShapeParam::AffectShape Allowance)
{
	ERROR2IF(pShape == NULL, FALSE, "Can't edit a NULL shape!");

	switch (Allowance)
	{
		case EditRegularShapeParam::AFFECT_ALL:
			return TRUE;
			break;
		case EditRegularShapeParam::AFFECT_ELLIPSES:
			return pShape->IsCircular();
			break;
		case EditRegularShapeParam::AFFECT_RECTANGLES:
			return (!pShape->IsCircular() && (pShape->GetNumSides() == 4) );
			break;
		default:
			ERROR3("Allowance was not known");
			return FALSE;
	}
}




///////////////////////////////////////////////////////////////////////////////////////////////
// The ChangeShapeDataAction class															 //
///////////////////////////////////////////////////////////////////////////////////////////////

/********************************************************************************************

>	ChangeShapeDataAction::ChangeShapeDataAction()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/12/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Constructor for the action to toggle a data item of a regular state.
				Initialises the data members to sensible defaults.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
ChangeShapeDataAction::ChangeShapeDataAction()
{
	pToggleShape = NULL;
	ChangeItemID = CHANGE_NUMSIDES;
	NewValue = 0.0;
}



/********************************************************************************************

>	ActionCode ChangeShapeDataAction::Init(	Operation* pOp,
											ActionList* pActionList,
											NodeRegularShape* pShape,
											enum ChangeItem NewItem,
											double NewData,
											Action** NewAction)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/12/94
	Inputs:		pOp is the pointer to the operation to which this action belongs
				pShape is a pointer to the NodeRegularShape to change
				NewItem identifes the data item that shoudl be changed
				NewData is the value this action should set the data item to
				pActionList is the action list to which this action should be added
	Outputs:	NewAction is a pointer to a pointer to an action, allowing the function to
				return a pointer to the created action
	Returns:	ActionCode, one of AC_OK, AC_NO_RECORD or AC_FAIL
	Purpose:	This is the function which creates an instance of this action. If there is no
				room in the undo buffer (which is determined by the base class Init function
				called within) the function will either return AC_NO_RECORD which means the
				operation can continue, but no undo information needs to be stored, or AC_OK
				which means the operation should continue AND record undo information. If
				the function returns AC_FAIL, there was not enough memory to record the undo
				information, and the user has decided not to continue with the operation.
	Errors:		-
	SeeAlso:	Action::Init()

********************************************************************************************/
ActionCode ChangeShapeDataAction::Init(	Operation* pOp,
										ActionList* pActionList,
										NodeRegularShape* pShape,
										enum ChangeItem NewItem,
										double NewData,
										Action** NewAction)
{
	UINT32 ActSize = sizeof(ChangeShapeDataAction);			

	ActionCode Ac = Action::Init( pOp, pActionList, ActSize, CC_RUNTIME_CLASS(ChangeShapeDataAction), NewAction);
	if ((Ac == AC_OK) && (*NewAction != NULL))
	{
		((ChangeShapeDataAction*)*NewAction)->pToggleShape = pShape;
		((ChangeShapeDataAction*)*NewAction)->ChangeItemID = NewItem;
		((ChangeShapeDataAction*)*NewAction)->NewValue = NewData;
	}
	return Ac;
}



/********************************************************************************************

>	ActionCode ChangeShapeDataAction::DoToggle(Operation* pOp,
											ActionList* pActionList,
											NodeRegularShape* pShape,
											enum ChangeItem NewItem,
											double NewData)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/12/94
	Inputs:		pOp is the currently running operation.
				pActionList is a pointer ot the action list to which the action should be appended.
				pShape is a pointer to the NodeRegularShape to change.
				NewItem identifes the data item that shoudl be changed
				NewData is the value this action should set the data item to
	Outputs:	-
	Returns:	Action code which indicates success or failure to create the action
	Purpose:	This static function makes it a little easier to use this action. It creates an instance
				of this action and appends it to the action list.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

ActionCode ChangeShapeDataAction::DoToggle(Operation* pOp,
										ActionList* pActionList,
										NodeRegularShape* pShape,
										enum ChangeItem NewItem,
										double NewData)
{
	// Get the runtime class info on this object and create a pointer to another object of the same type 
	ChangeShapeDataAction* NewAction; 

	// Now call the init function to set everything up
	ActionCode Act = ChangeShapeDataAction::Init(pOp, pActionList, pShape, NewItem, NewData, (Action**)&NewAction);
	return Act;
}



/********************************************************************************************

>	ActionCode ChangeShapeDataAction::Execute()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/12/94
	Inputs:		-
	Outputs:	-
	Returns:	ActionCode, either AC_OK, AC_NORECORD or AC_FAIL
	Purpose:	Changes a data item in a Regular shape, createing another
				ChangeShapeDataAction to undo the change.
	Errors:		ERROR3 if the internal shape pointer was NULL.  ERROR2 checks on individual
				changes.
	SeeAlso:	-

********************************************************************************************/
ActionCode ChangeShapeDataAction::Execute()
{
	ERROR3IF(pToggleShape == NULL, "Pointer to shape was NULL.  Did you call Init/DoToggle OR handle their failure?");

	// Get the runtime class info on this object and create a pointer to another object of the same type 
	ChangeShapeDataAction *ReAction; 
	ActionCode Act = AC_FAIL;
	double ReData = 0.0;

	switch (ChangeItemID)
	{
		case CHANGE_NUMSIDES:
			ReData = pToggleShape->GetNumSides();
			break;
		case CHANGE_CIRCULAR:
			ReData = pToggleShape->IsCircular() ? 1.0 : 0.0;	
			break;
		case CHANGE_STELLATED:
			ReData = pToggleShape->IsStellated() ? 1.0 : 0.0;	
			break;
		case CHANGE_STELLRADIUSTOPRIMARY:
			ReData = pToggleShape->GetStellRadiusToPrimary();	
			break;
		case CHANGE_PRIMARYCURVATURE:
			ReData = pToggleShape->IsPrimaryCurvature() ? 1.0 : 0.0;	
			break;
		case CHANGE_STELLATIONCURVATURE:
			ReData = pToggleShape->IsStellationCurvature() ? 1.0 : 0.0;	
			break;
		case CHANGE_PRIMARYCURVETOPRIMARY:
			ReData = pToggleShape->GetPrimaryCurveToPrimary();	
			break;
		case CHANGE_STELLCURVETOSTELL:
			ReData = pToggleShape->GetStellCurveToStell();	
			break;
		case CHANGE_STELLATIONOFFSET:
			ReData = pToggleShape->GetStellationRatio();	
			break;
		default:
			ERROR2(Act, "What was that Change ID?!");
			break;
	}
	
	// Create a redo action for this action
  	if (pToggleShape != NULL)
	{
		Act = ChangeShapeDataAction::Init(pOperation, pOppositeActLst, pToggleShape, ChangeItemID, 
																			ReData, (Action**)(&ReAction));

		if (Act == AC_FAIL)
			return AC_FAIL;

		// Now do the actual action
		switch (ChangeItemID)
		{
			case CHANGE_NUMSIDES:
				pToggleShape->SetNumSides((UINT32)NewValue);
				break;
			case CHANGE_CIRCULAR:
				pToggleShape->SetCircular(NewValue == 1.0 ? TRUE : FALSE);
				break;
			case CHANGE_STELLATED:
				pToggleShape->SetStellated(NewValue == 1.0 ? TRUE : FALSE);
				break;
			case CHANGE_STELLRADIUSTOPRIMARY:
				pToggleShape->SetStellRadiusToPrimary(NewValue);
				break;
			case CHANGE_PRIMARYCURVATURE:
				pToggleShape->SetPrimaryCurvature(NewValue == 1.0 ? TRUE : FALSE);
				break;
			case CHANGE_STELLATIONCURVATURE:
				pToggleShape->SetStellationCurvature(NewValue == 1.0 ? TRUE : FALSE);
				break;
			case CHANGE_PRIMARYCURVETOPRIMARY:
				pToggleShape->SetPrimaryCurveToPrimary(NewValue);
				break;
			case CHANGE_STELLCURVETOSTELL:
				pToggleShape->SetStellCurveToStell(NewValue);
				break;
			case CHANGE_STELLATIONOFFSET:
				pToggleShape->SetStellationRatio(NewValue);
				break;
			default:
				ERROR3("What was that Change ID?!");
				break;
		}
		pToggleShape->InvalidateBoundingRect();
		pToggleShape->InvalidateCache();
	}

	return Act;
}



///////////////////////////////////////////////////////////////////////////////////////////////
// The ChangeShapePointAction class															 //
///////////////////////////////////////////////////////////////////////////////////////////////

/********************************************************************************************

>	ChangeShapePointAction::ChangeShapePointAction()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Constructor for the action to change a point in a regular shape.
				Initialises the data members to sensible defaults.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
ChangeShapePointAction::ChangeShapePointAction()
{
	pToggleShape = NULL;
	ChangeItemID = CHANGE_MAJOR;
	NewValue = DocCoord(0,0);
}



/********************************************************************************************

>	ActionCode ChangeShapePointAction::Init(	Operation* pOp,
											ActionList* pActionList,
											NodeRegularShape* pShape,
											enum ChangeItem NewItem,
											DocCoord NewData,
											Action** NewAction)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/12/94
	Inputs:		pOp is the pointer to the operation to which this action belongs
				pShape is a pointer to the NodeRegularShape to change
				NewItem identifes the data item that shoudl be changed
				NewData is the value this action should set the data item to
				pActionList is the action list to which this action should be added
	Outputs:	NewAction is a pointer to a pointer to an action, allowing the function to
				return a pointer to the created action
	Returns:	ActionCode, one of AC_OK, AC_NO_RECORD or AC_FAIL
	Purpose:	This is the function which creates an instance of this action. If there is no
				room in the undo buffer (which is determined by the base class Init function
				called within) the function will either return AC_NO_RECORD which means the
				operation can continue, but no undo information needs to be stored, or AC_OK
				which means the operation should continue AND record undo information. If
				the function returns AC_FAIL, there was not enough memory to record the undo
				information, and the user has decided not to continue with the operation.
	Errors:		-
	SeeAlso:	Action::Init()

********************************************************************************************/
ActionCode ChangeShapePointAction::Init(	Operation* pOp,
										ActionList* pActionList,
										NodeRegularShape* pShape,
										enum ChangeItem NewItem,
										DocCoord NewData,
										Action** NewAction)
{
	UINT32 ActSize = sizeof(ChangeShapePointAction);			

	ActionCode Ac = Action::Init( pOp, pActionList, ActSize, CC_RUNTIME_CLASS(ChangeShapePointAction), NewAction);
	if ((Ac == AC_OK) && (*NewAction != NULL))
	{
		((ChangeShapePointAction*)*NewAction)->pToggleShape = pShape;
		((ChangeShapePointAction*)*NewAction)->ChangeItemID = NewItem;
		((ChangeShapePointAction*)*NewAction)->NewValue = NewData;
	}
	return Ac;
}



/********************************************************************************************

>	ActionCode ChangeShapePointAction::DoToggle(Operation* pOp,
											ActionList* pActionList,
											NodeRegularShape* pShape,
											enum ChangeItem NewItem,
											DocCoord NewData)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/12/94
	Inputs:		pOp is the currently running operation.
				pActionList is a pointer ot the action list to which the action should be appended.
				pShape is a pointer to the NodeRegularShape to change.
				NewItem identifes the data item that shoudl be changed
				NewData is the value this action should set the data item to
	Outputs:	-
	Returns:	Action code which indicates success or failure to create the action
	Purpose:	This static function makes it a little easier to use this action. It creates an instance
				of this action and appends it to the action list.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

ActionCode ChangeShapePointAction::DoToggle(Operation* pOp,
										ActionList* pActionList,
										NodeRegularShape* pShape,
										enum ChangeItem NewItem,
										DocCoord NewData)
{
	// Get the runtime class info on this object and create a pointer to another object of the same type 
	ChangeShapePointAction* NewAction; 

	// Now call the init function to set everything up
	ActionCode Act = ChangeShapePointAction::Init(pOp, pActionList, pShape, NewItem, NewData, (Action**)&NewAction);
	return Act;
}



/********************************************************************************************

>	ActionCode ChangeShapePointAction::Execute()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/12/94
	Inputs:		-
	Outputs:	-
	Returns:	ActionCode, either AC_OK, AC_NORECORD or AC_FAIL
	Purpose:	Changes a data item in a Regular shape, createing another
				ChangeShapePointAction to undo the change.
	Errors:		ERROR3 if the internal shape pointer was NULL.  ERROR2 checks on individual
				changes.
	SeeAlso:	-

********************************************************************************************/
ActionCode ChangeShapePointAction::Execute()
{
	ERROR3IF(pToggleShape == NULL, "Pointer to shape was NULL.  Did you call Init/DoToggle OR handle their failure?");

	// Get the runtime class info on this object and create a pointer to another object of the same type 
	ChangeShapePointAction *ReAction; 
	ActionCode Act = AC_FAIL;
	DocCoord ReData(0,0);

	switch (ChangeItemID)
	{
		case CHANGE_MINOR:
			ReData = pToggleShape->GetMinorAxes();
			break;
		case CHANGE_MAJOR:
			ReData = pToggleShape->GetMajorAxes();
			break;
		case CHANGE_CENTRE:
			ReData = pToggleShape->GetCentrePoint();
			break;
		default:
			ERROR2(Act, "What was that Change ID?!");
			break;
	}
	
	// Create a redo action for this action
  	if (pToggleShape != NULL)
	{
		Act = ChangeShapePointAction::Init(pOperation, pOppositeActLst, pToggleShape, ChangeItemID, 
																			ReData, (Action**)(&ReAction));

		if (Act == AC_FAIL)
			return AC_FAIL;

		// Now do the actual action
		switch (ChangeItemID)
		{
			case CHANGE_MINOR:
				pToggleShape->SetMinorAxes(NewValue);
				break;
			case CHANGE_MAJOR:
				pToggleShape->SetMajorAxes(NewValue);
				break;
			case CHANGE_CENTRE:
				pToggleShape->SetCentrePoint(NewValue);
				break;
		}
		pToggleShape->InvalidateBoundingRect();
		pToggleShape->InvalidateCache();
	}

	return Act;
}



///////////////////////////////////////////////////////////////////////////////////////////////
// The ChangeShapePathAction class															 //
///////////////////////////////////////////////////////////////////////////////////////////////

/********************************************************************************************

>	ChangeShapePathAction::ChangeShapePathAction()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/12/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Constructor for the action to change an edge path of a regular shape.
				Initialises the data members to sensible defaults.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
ChangeShapePathAction::ChangeShapePathAction()
{
	pShape = NULL;
	ChangeItemID = CHANGE_PATH1;
}



/********************************************************************************************

>	ActionCode ChangeShapePathAction::Init(	Operation* pOp,
											ActionList* pActionList,
											NodeRegularShape* pChangeShape,
											enum ChangeItem NewItem,
											Path* NewPath,
											Action** NewAction)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/12/94
	Inputs:		pOp is the pointer to the operation to which this action belongs
				pShape is a pointer to the NodeRegularShape to change
				NewItem identifes the data item that shoudl be changed
				NewPath points to the path to store as undo infomation
				pActionList is the action list to which this action should be added
	Outputs:	NewAction is a pointer to a pointer to an action, allowing the function to
				return a pointer to the created action
	Returns:	ActionCode, one of AC_OK, AC_NO_RECORD or AC_FAIL
	Purpose:	This is the function which creates an instance of this action. If there is no
				room in the undo buffer (which is determined by the base class Init function
				called within) the function will either return AC_NO_RECORD which means the
				operation can continue, but no undo information needs to be stored, or AC_OK
				which means the operation should continue AND record undo information. If
				the function returns AC_FAIL, there was not enough memory to record the undo
				information, and the user has decided not to continue with the operation.
	Errors:		-
	SeeAlso:	Action::Init()

********************************************************************************************/
ActionCode ChangeShapePathAction::Init(	Operation* pOp,
										ActionList* pActionList,
										NodeRegularShape* pChangeShape,
										enum ChangeItem NewItem,
										Path* NewPath,
										Action** NewAction)
{
	UINT32 ActSize = sizeof(ChangeShapePathAction);			

	ActionCode Ac = Action::Init( pOp, pActionList, ActSize, CC_RUNTIME_CLASS(ChangeShapePathAction), NewAction);
	if ((Ac == AC_OK) && (*NewAction != NULL))
	{
		((ChangeShapePathAction*)*NewAction)->pShape = pChangeShape;
		((ChangeShapePathAction*)*NewAction)->ChangeItemID = NewItem;
		if (!((ChangeShapePathAction*)*NewAction)->NewEdge.Initialise(NewPath->GetNumCoords()))
			return AC_FAIL;
		((ChangeShapePathAction*)*NewAction)->NewEdge.CopyPathDataFrom(NewPath);
	}
	return Ac;
}



/********************************************************************************************

>	ActionCode ChangeShapePathAction::DoReshape(Operation* pOp,
											ActionList* pActionList,
											NodeRegularShape* pShape,
											enum ChangeItem NewItem,
											Path* NewData)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/12/94
	Inputs:		pOp is the currently running operation.
				pActionList is a pointer ot the action list to which the action should be appended.
				pShape is a pointer to the NodeRegularShape to change.
				NewItem identifes the data item that shoudl be changed
				NewData poits to the path that this action should store
	Outputs:	-
	Returns:	Action code which indicates success or failure to create the action
	Purpose:	This static function makes it a little easier to use this action. It creates an instance
				of this action and appends it to the action list.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

ActionCode ChangeShapePathAction::DoReshape(Operation* pOp,
										ActionList* pActionList,
										NodeRegularShape* pShape,
										enum ChangeItem NewItem,
										Path* NewData)
{
	// Get the runtime class info on this object and create a pointer to another object of the same type 
	ChangeShapePathAction* NewAction; 

	// Now call the init function to set everything up
	ActionCode Act = ChangeShapePathAction::Init(pOp, pActionList, pShape, NewItem, NewData, (Action**)&NewAction);
	return Act;
}



/********************************************************************************************

>	ActionCode ChangeShapePathAction::Execute()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/12/94
	Inputs:		-
	Outputs:	-
	Returns:	ActionCode, either AC_OK, AC_NORECORD or AC_FAIL
	Purpose:	Changes a data item in a Regular shape, createing another
				ChangeShapePathAction to undo the change.
	Errors:		ERROR2 if the internal shape pointer was NULL.
	SeeAlso:	-

********************************************************************************************/
ActionCode ChangeShapePathAction::Execute()
{
	ERROR2IF(pShape == NULL, AC_FAIL, "Pointer to shape was NULL.  Did you call Init/DoToggle OR handle their failure?");

	// Get the runtime class info on this object and create a pointer to another object of the same type 
	ChangeShapePathAction *ReAction; 
	ActionCode Act = AC_FAIL;
	Path* ReData;

	switch (ChangeItemID)
	{
		case CHANGE_PATH1:
			ReData = &(pShape->EdgePath1);
			break;
		case CHANGE_PATH2:
			ReData = &(pShape->EdgePath2);
			break;
		default:
			ERROR2(Act, "What was that Change ID?!");
			break;
	}
	
	// Create a redo action for this action
	Act = ChangeShapePathAction::Init(pOperation, pOppositeActLst, pShape, ChangeItemID, 
																		ReData, (Action**)(&ReAction));

	if (Act == AC_FAIL)
		return AC_FAIL;

	// Now do the actual action
	switch (ChangeItemID)
	{
		case CHANGE_PATH1:
			pShape->EdgePath1.CopyPathDataFrom(&NewEdge);
			break;
		case CHANGE_PATH2:
			pShape->EdgePath2.CopyPathDataFrom(&NewEdge);
			break;
	}
	pShape->InvalidateBoundingRect();
	pShape->InvalidateCache();

	return Act;
}



////////////////////////////////////////////////////////////////////////////////////////////
//										OpDragRegularShape								  //
////////////////////////////////////////////////////////////////////////////////////////////



/********************************************************************************************

>	OpDragRegularShape::OpDragRegularShape()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/12/94
	Purpose:	Constructor. This sets a few of the operation variables.

********************************************************************************************/

OpDragRegularShape::OpDragRegularShape()
{
	pCursor = NULL;
	StartSpread = NULL;
	OriginalShape = NULL;
	EditShape = NULL;
}



/********************************************************************************************

>	void OpDragRegularShape::DoStartDragEdit(	NodeRegularShape* pShape,
												DocCoord Anchor,
												Spread *pSpread,
												DragType DragThing,
												BOOL QuickShape)
	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/12/94
	Inputs:		pShape - Pointer to the shape we are about to edit
				Anchor - The position of the mouse at the start of the Drag
				pSpread - The spread that the shape is on
				DragThing - the object on the shape that should be dragged
				QuickShape - TRUE to drag about the centres of shapes, FALSE to do special
				shape drags
	Purpose:	This is called to start a drag operation on a regular shape
	Errors:		If there was not enough memory for the setup then the user is informed and
				the op is aborted

********************************************************************************************/

void OpDragRegularShape::DoStartDragEdit( NodeRegularShape* pShape, DocCoord Anchor, Spread *pSpread,
																 DragType DragThing, BOOL QuickShape)
{
	// Adjust anchor point so this is the actual point on the shape that the drag starts from
	INT32 DragPointNumber = -1;
	pShape->DetermineClickEffect(&Anchor, pSpread, &DragPointNumber);

	// Take note of the drag parameters
	DragObject = DragThing;
	LastMousePos = Anchor;
	StartMousePos = Anchor;
	StartSpread  = pSpread;
	OriginalShape = pShape;
	DragAroundCentre = QuickShape;

	// We also need to make a version of the shape that we can change
	EditShape = (NodeRegularShape*) OriginalShape->SimpleCopy();
	BOOL Success = (EditShape != NULL);

	if (Success)
	{
		// Create the cursors and display it (if it was created ok)
		pCursor = new Cursor(TOOLID_BEZTOOL, _R(IDC_MOVEBEZIERCURSOR));

		if ( pCursor != NULL && pCursor->IsValid())
		{	// Select the cursor
			CurrentCursorID = CursorStack::GPush(pCursor);
		}
		else
		{	// Set cursor pointer to NULL to indicate its non appearence
			if (pCursor != NULL)
				delete pCursor;
			pCursor = NULL;
		}

		// And tell the Dragging system that we need drags to happen
		Success = StartDrag(DRAGTYPE_AUTOSCROLL, NULL, &LastMousePos, FALSE);

		// Render the initial shape
		if (Success)
			RenderDragBlobs(DocRect(), pSpread, FALSE);
	}

	// Do special rectangle setup
	if (Success && (DragObject == DRAG_PRIMARY) && OriginalShape->IsARectangle())
	{
		if (DragPointNumber == -1)
		{
			ERROR3("Couldn't find the rectangle drag point");
		 	Success = FALSE;
		}

		// Get the significant points
		DocCoord* PointsArray = NULL;
		INT32 NumberPoints = 0;
		Success = OriginalShape->BuildPolygonPoints(&PointsArray, &NumberPoints);

		// Transform the significant points to document space
		if (Success)
		{
			// Get the shape matrix and its inverse
			Matrix ShapeMat;
			OriginalShape->GetTransformMatrix(&ShapeMat);
			ShapeMat.transform((Coord*)PointsArray,NumberPoints);
		}

		// Get the points opposite to the drag point, the stationary point
		INT32 Opposite = 0;
		if (Success)
			Opposite = (DragPointNumber + ((NumberPoints-2)/2)) % (NumberPoints-2);

		// Copy the points into the array.  ) is the opposite point, 2 is the drag point
		if (Success)
		{
			INT32 Increment = (NumberPoints-2)/4;
			INT32 Copy = 0;
			for (INT32 i = Opposite; Copy < 4; i+=Increment)
				RectangleArray[Copy++] = PointsArray[i%(NumberPoints-2)];
			
			// Make the array go clockwise
			DocCoord temp = RectangleArray[1];
			RectangleArray[1] = RectangleArray[3];
			RectangleArray[3] = temp;
		}

		delete PointsArray;
	}

	if (!Success)
	{
		InformError();
		FailAndExecute();
		End();
	}
}



/********************************************************************************************

>	void OpDragRegularShape::DoStartDragEditCurve(NodeRegularShape* pShape, DocCoord Anchor,
							Spread *pSpread, DragType DragThing, DocCoord Start, DocCoord End)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/12/94
	Inputs:		pShape - Pointer to the shape we are about to edit
				Anchor - The position of the mouse at the start of the Drag
				pSpread - The spread that the shape is on
				DragThing - the object on the shape that should be dragged
				Start - the start of the line along which the curvature point is to be dragged
				End - the end of the line along which the curvature point is to be dragged.
	Purpose:	This is called to start a drag operation on a regular shape
	Errors:		If there was not enought memory for the setup then the user is informed and
				the op is aborted

********************************************************************************************/

void OpDragRegularShape::DoStartDragEditCurve( NodeRegularShape* pShape, DocCoord Anchor,
							Spread *pSpread, DragType DragThing, DocCoord Start, DocCoord End)
{
	DoStartDragEdit(pShape, Anchor, pSpread, DragThing, TRUE);

	// Take note of the additional drag parameters
	LineStart = Start;
	LineEnd = End;
}


/********************************************************************************************
>	virtual BOOL OpDragRegularShape::SnappingDrag()

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/10/95
	Returns:	FALSE if the current drag is not of the snapping flavor
********************************************************************************************/

BOOL OpDragRegularShape::SnappingDrag()
{
	BOOL NonSnappingDrag = (DragObject==DRAG_PRIMARYCURVE || DragObject==DRAG_STELLATIONCURVE);
	return !NonSnappingDrag;
}


/********************************************************************************************

>	void OpDragRegularShape::DragPointerMove( DocCoord PointerPos, ClickModifiers ClickMods, Spread *pSpread, BOOL bSolidDrag)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/12/94
	Inputs:		PointerPos - The current position of the mouse in Doc Coords
				ClickMods - Which key modifiers are being pressed
				pSpread - The spread containing PointerPos
	Purpose:	This is called every time the mouse moves, during a drag.  We need to render
				off the shape in its current position, calculate any changes to apply to it
				then render it back on
	SeeAlso:	ClickModifiers

********************************************************************************************/

void OpDragRegularShape::DragPointerMove( DocCoord PointerPos, ClickModifiers ClickMods, Spread *pSpread, BOOL bSolidDrag)
{
	// If the PointerPois is the same as the last one, then dont bother doing anything
	if (PointerPos == LastMousePos)
		return;
	
	// If drag has moved onto a different spread, convert the coord to be relative to the
	// original spread.
	if (pSpread != StartSpread)
		PointerPos = MakeRelativeToSpread(StartSpread, pSpread, PointerPos);

	// Rub out the old EORed version of the path
	RenderDragBlobs( DocRect(), StartSpread, bSolidDrag);

	// Now apply the changes to the EditShape, depending on the drag type
	BOOL Success = FALSE;
	switch (DragObject)
	{
		case DRAG_CENTRE:
		{
			Success = ProcessCentre(PointerPos, ClickMods);
			break;
		}
		case DRAG_ELLIPSE:
		{
			if (DragAroundCentre)
				Success = ProcessPrimary(PointerPos, ClickMods);
			else
				Success = ProcessEllipse(PointerPos, ClickMods);
			break;
		}
		case DRAG_PRIMARY:
		{
			if (EditShape->IsARectangle() && !DragAroundCentre)
				Success = ProcessRectangle(PointerPos, ClickMods);
			else
				Success = ProcessPrimary(PointerPos, ClickMods);
			break;
		}
		case DRAG_STELLATION:
		{
			Success = ProcessStellation(PointerPos, ClickMods);
			break;
		}
		case DRAG_PRIMARYCURVE:
		{
			Success = ProcessPrimaryCurve(PointerPos);
			break;
		}
		case DRAG_STELLATIONCURVE:
		{
			Success = ProcessStellationCurve(PointerPos);
			break;
		}
		default:
			ERROR3("What was the drag type?");
			break;
	}

	if (Success)
	{
		// Draw in the new version of the path
		RenderDragBlobs( DocRect(), StartSpread, bSolidDrag);

		// Update the Last Mouse Position
		LastMousePos = PointerPos;

		// Tell anyone that's interested
		BROADCAST_TO_ALL(ShapeEditedMsg(EditShape, pSpread));
	}
	else
	{
		InformError();
		EndDrag();
		FailAndExecute();
		End();
	}
}



/********************************************************************************************

>	void OpDragRegularShape::DragFinished( DocCoord PointerPos, ClickModifiers ClickMods, BOOL Success, BOOL bSolidDrag)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/14/94
	Inputs:		PointerPos - The position of the mouse at the end of the drag
				ClickMods - the key modifiers being pressed
				Success - TRUE if the drag was terminated properly, FALSE if it
				was ended with the escape key being pressed
	Purpose:	This is called when a drag operation finishes.  If the drag was sucessful then
				the OriginalShape in the document is updated.
	SeeAlso:	ClickModifiers

********************************************************************************************/

void OpDragRegularShape::DragFinished( DocCoord PointerPos, ClickModifiers ClickMods, 
									   Spread* pSpread, BOOL Success, BOOL bSolidDrag)
{
	// Do some tidying up
	RenderDragBlobs( DocRect(), StartSpread, bSolidDrag );
	if (pCursor != NULL)
	{
		CursorStack::GPop(CurrentCursorID);
		delete pCursor;
		CurrentCursorID = 0;
		pCursor = NULL;
	}
	EndDrag();
	BeginSlowJob();

	if ( Success )
	{
		BOOL Failed = !DoStartSelOp(FALSE, TRUE, FALSE, TRUE);
		
		//DocView* pDocView = DocView::GetSelected();
		//ERROR3IF( pDocView == NULL, "There was no selected docview when editing a shape" );

		// Will the original shape allow the op to happen?
		ObjChangeFlags cFlags;
		cFlags.TransformNode = TRUE;
		ObjChangeParam ObjChange(OBJCHANGE_STARTING,cFlags,OriginalShape,this);
		Failed = !OriginalShape->AllowOp(&ObjChange);

		if (!Failed)
			Failed = (RecalcBoundsAction::DoRecalc(this, &UndoActions, OriginalShape) == AC_FAIL);

		// Now update the shape depending on the dragtype
		if (!Failed)
		{
			switch (DragObject)
			{
				case DRAG_CENTRE:
				{
					// Construct a transformation matrix to move the shape
					Trans2DMatrix* TransMat = new Trans2DMatrix(EditShape->GetCentrePoint().x - OriginalShape->GetCentrePoint().x,
											EditShape->GetCentrePoint().y - OriginalShape->GetCentrePoint().y);
					DoTransformNode(OriginalShape, TransMat);
					break;
				}
				case DRAG_ELLIPSE:
				case DRAG_PRIMARY:
				{	// Transform shape using the matrix
					Trans2DMatrix* tm = new Trans2DMatrix(transMat);
					Failed = ! DoTransformNode(OriginalShape, tm) ;
					break;
				}
				case DRAG_STELLATION:
					Failed = !ChangeShapeDataAction::DoToggle(this, &UndoActions, OriginalShape, 
												ChangeShapeDataAction::CHANGE_STELLRADIUSTOPRIMARY, 
															OriginalShape->GetStellRadiusToPrimary() );
					OriginalShape->SetStellRadiusToPrimary(EditShape->GetStellRadiusToPrimary());
					Failed = !ChangeShapeDataAction::DoToggle(this, &UndoActions, OriginalShape, 
												ChangeShapeDataAction::CHANGE_STELLATIONOFFSET, 
															OriginalShape->GetStellationRatio() );
					OriginalShape->SetStellationRatio(EditShape->GetStellationRatio());
					break;
				case DRAG_PRIMARYCURVE:
					Failed = !ChangeShapeDataAction::DoToggle(this, &UndoActions, OriginalShape, 
												ChangeShapeDataAction::CHANGE_PRIMARYCURVETOPRIMARY, 
															OriginalShape->GetPrimaryCurveToPrimary() );
					OriginalShape->SetPrimaryCurveToPrimary(EditShape->GetPrimaryCurveToPrimary());
					break;
				case DRAG_STELLATIONCURVE:
					Failed = !ChangeShapeDataAction::DoToggle(this, &UndoActions, OriginalShape, 
													ChangeShapeDataAction::CHANGE_STELLCURVETOSTELL, 
															OriginalShape->GetStellCurveToStell() );
					OriginalShape->SetStellCurveToStell(EditShape->GetStellCurveToStell());
					break;
				default:
					ERROR3("What was that drag type?");
					break;
			}
		}

		// Recalculate the path's bounding box
		OriginalShape->InvalidateBoundingRect();
		OriginalShape->InvalidateCache();
		
		// tell the world that something in the selection has changed so that selection bounds are updated
		SelRange *Sel = GetApplication()->FindSelection();
		Sel->Update();

		// Try to expand the pasteboard if necessary to include the new bounds of the object
		DocRect NewBounds = Sel->GetBoundingRect();
		ERROR3IF(pSpread == NULL, "Unexpectedly NULL Spread pointer");
		pSpread->ExpandPasteboardToInclude(NewBounds);

		if (!Failed)
		{
			// Update effected parents
			ObjChange.Define(OBJCHANGE_FINISHED,cFlags,OriginalShape,this);
			Failed = !UpdateChangedNodes(&ObjChange);
		}

		if (!Failed && OriginalShape->FindParentSpread()!=NULL)
			Failed = (RecordBoundsAction::DoRecord(this, &UndoActions, OriginalShape) == AC_FAIL) ;

		// If any of the above failed the kill the op
		if (Failed)
		{
//			PATRACE( _T("An action failed!\n"));
			FailAndExecute();
		}
	}
	else
	{
		// Set up the flags that say it all went wrong
		FailAndExecute();
	}

	delete (EditShape);
	End();
}



/********************************************************************************************

>	BOOL OpDragRegularShape::ProcessPrimaryCurve( DocCoord PointerPos )

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/1/94
	Inputs:		PointerPos - The current position of the mouse in Doc Coords
	Purpose:	This is called to update the EditShape after a mouse movement in a primary
				curvature point drag.
	SeeAlso:	OpDragRegularShape::DragPointerMove

********************************************************************************************/

BOOL OpDragRegularShape::ProcessPrimaryCurve( DocCoord PointerPos )
{
	// Untransform the various things
	Matrix Mat;
	EditShape->GetTransformMatrix(&Mat);
	Mat = Mat.Inverse();
	DocCoord UTLineStart = LineStart;
	DocCoord UTLineEnd = LineEnd;
	DocCoord UTPointerPos = PointerPos;
	Mat.transform(&UTLineStart);
	Mat.transform(&UTLineEnd);
	Mat.transform(&UTPointerPos);

	// Move the point along the edge line to get new ratio
	double c = UTLineStart.Distance(UTPointerPos);
	double b = fabs(DocCoord::DistanceFromLine(UTLineStart, UTLineEnd, UTPointerPos));
	double a = sqrt(c*c - b*b);
	double NewRatio = a/EditShape->GetUTMajorAxes().Distance(EditShape->GetUTCentrePoint());

	// Detect a zero ratio case (otherwise the point moves back along the line)
	if (!IsJoinAcute(&UTLineStart, &UTLineEnd, &UTPointerPos))
	{
		NewRatio = 0.0;
	}
	else
	{
		// Restrict to fit with along the edge
		if (EditShape->IsStellated())
		{
			// Get the position of the stellation curvature point and its distance
			const double OuterMajorLength = OriginalShape->GetMajorRadiusSize();
			const double StellationCurvatureLength = OuterMajorLength * OriginalShape->GetStellCurveToStell();
			const double AvailableLength = (UTLineStart.Distance(UTLineEnd)) - StellationCurvatureLength;

			double top = a;
			if ( (AvailableLength - a) < 1.0)
				top = AvailableLength;

			NewRatio = top/EditShape->GetUTMajorAxes().Distance(EditShape->GetUTCentrePoint());
		}
		else
		{
			// We need to limit the drag to halfway along the drag line
			const double MaxLength = UTLineStart.Distance(UTLineEnd)/2.0;
			if (a > MaxLength)
				NewRatio = MaxLength/EditShape->GetUTMajorAxes().Distance(EditShape->GetUTCentrePoint());
		}
	}

	EditShape->SetPrimaryCurveToPrimary(NewRatio);

	return TRUE;
}



/********************************************************************************************

>	BOOL OpDragRegularShape::ProcessStellationCurve( DocCoord PointerPos )

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/1/94
	Inputs:		PointerPos - The current position of the mouse in Doc Coords
	Purpose:	This is called to update the EditShape after a mouse movement in a stellation
				curvature point drag.
	SeeAlso:	OpDragRegularShape::DragPointerMove

********************************************************************************************/

BOOL OpDragRegularShape::ProcessStellationCurve( DocCoord PointerPos )
{
	// Untransform the various things
	Matrix Mat;
	EditShape->GetTransformMatrix(&Mat);
	Mat = Mat.Inverse();
	DocCoord UTLineStart = LineStart;
	DocCoord UTLineEnd = LineEnd;
	DocCoord UTPointerPos = PointerPos;
	Mat.transform(&UTLineStart);
	Mat.transform(&UTLineEnd);
	Mat.transform(&UTPointerPos);

	// Move the point along the edge line to get new ratio
	double c = UTLineStart.Distance(UTPointerPos);
	double b = fabs(DocCoord::DistanceFromLine(UTLineStart, UTLineEnd, UTPointerPos));
	double a = sqrt(c*c - b*b);
	double NewRatio = a/EditShape->GetUTMajorAxes().Distance(EditShape->GetUTCentrePoint());

	// Detect a zero ratio case (otherwise the point moves back along the line)
	if (!IsJoinAcute(&UTLineStart, &UTLineEnd, &UTPointerPos))
	{
		NewRatio = 0.0;
	}
	else
	{
		// Restrict to fit with along the edge
		const double OuterMajorLength = OriginalShape->GetMajorRadiusSize();
		const double PrimaryCurvatureLength = OuterMajorLength * OriginalShape->GetPrimaryCurveToPrimary();
		const double AvailableLength = (UTLineStart.Distance(UTLineEnd)) - PrimaryCurvatureLength;

		double top = a;
		if ( (AvailableLength - a) < 1.0)
			top = AvailableLength;

		NewRatio = top/EditShape->GetUTMajorAxes().Distance(EditShape->GetUTCentrePoint());
	}

	EditShape->SetStellCurveToStell(NewRatio);

	return TRUE;
}



/********************************************************************************************

>	BOOL OpDragRegularShape::ProcessStellation( DocCoord PointerPos, ClickModifiers Mods )

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/1/94
	Inputs:		PointerPos - The current position of the mouse in Doc Coords
	Purpose:	This is called to update the EditShape after a mouse movement in a stellation
				point drag.
	SeeAlso:	OpDragRegularShape::DragPointerMove

********************************************************************************************/

BOOL OpDragRegularShape::ProcessStellation( DocCoord PointerPos, ClickModifiers Mods)
{
	// Constrain the mouse movement
	if (Mods.Constrain)
	{
		WheelConstrain(&PointerPos, StartMousePos);
	}

   	// Snap the mouse point to the active grid
	DocView::SnapCurrent(StartSpread,&PointerPos);

	// Untransform the current pointer pos
	DocCoord UTPointerPos = GetUTPosition(PointerPos);
	DocCoord UTStartPos = GetUTPosition(StartMousePos);

	// Set the new stellation radius length
	double PrimaryRadiusLength = OriginalShape->GetUTMajorAxes().Distance(OriginalShape->GetUTCentrePoint());
	double CurrentRadiusLength = UTPointerPos.Distance(OriginalShape->GetUTCentrePoint());
	double NewRadius = min( CurrentRadiusLength / PrimaryRadiusLength, 1.0 );
	EditShape->SetStellRadiusToPrimary(NewRadius);

	// Now set the new stellation point offset
	// Get angle between the Major axes point and the pointer pos
	DocCoord OldPos = UTStartPos - OriginalShape->GetUTCentrePoint();
	DocCoord NewPos = UTPointerPos - OriginalShape->GetUTCentrePoint();

	if (NewPos != DocCoord(0,0))
	{
		double OrigAngle = atan2((double)OldPos.y, (double)OldPos.x);
		double NewAngle = atan2((double)NewPos.y, (double)NewPos.x);
		double OppositeRatio = 180.0/(360.0/EditShape->GetNumSides()) ;
		double SettingRatio = ((NewAngle-OrigAngle) / ((2*PI)/EditShape->GetNumSides()) ) + OriginalShape->GetStellationRatio(); 

		while (SettingRatio > OppositeRatio*2)
			SettingRatio -= OppositeRatio*2;
		while (SettingRatio < -OppositeRatio*2)
			SettingRatio += OppositeRatio*2;

		if (SettingRatio > OppositeRatio)
			SettingRatio = -OppositeRatio + (SettingRatio-OppositeRatio);
		if (SettingRatio < -OppositeRatio)
			SettingRatio = OppositeRatio + (SettingRatio+OppositeRatio);
			
		EditShape->SetStellationRatio(SettingRatio);
	}
	else
		EditShape->SetStellationRatio(0.0);

	return TRUE;
}



/********************************************************************************************

>	BOOL OpDragRegularShape::ProcessPrimary( DocCoord PointerPos, ClickModifiers Mods )

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/1/94
	Inputs:		PointerPos - The current position of the mouse in Doc Coords
	Purpose:	This is called to update the EditShape after a mouse movement in a primary
				point drag.
	SeeAlso:	OpDragRegularShape::DragPointerMove, OpDragRegularShape::ProcessRadius

********************************************************************************************/

BOOL OpDragRegularShape::ProcessPrimary( DocCoord PointerPos, ClickModifiers Mods )
{
	if (Mods.Constrain)
	{
		WheelConstrain(&PointerPos, StartMousePos);
	}

	return ProcessRadius(PointerPos, Mods);
}



/********************************************************************************************

>	BOOL OpDragRegularShape::ProcessEllipse( DocCoord PointerPos, ClickModifiers Mods )

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/04/95
	Inputs:		PointerPos - The current position of the mouse in Doc Coords
				Mods - the drag modifiers
	Purpose:	This is called to update the ellipitical EditShape after a mouse movement during
				a drag.
	SeeAlso:	OpDragRegularShape::DragPointerMove, OpDragRegularShape::ProcessRectangle

********************************************************************************************/

BOOL OpDragRegularShape::ProcessEllipse( DocCoord PointerPos, ClickModifiers Mods )
{
	// If this is an adjust drag then work around the centre
	if (Mods.Adjust)
	{
		if (Mods.Constrain)
			WheelConstrain(&PointerPos, StartMousePos);

		return ProcessRadius(PointerPos, Mods);
	}

   	// Snap the mouse point to the active grid
	DocView::SnapCurrent(StartSpread,&PointerPos);

	// Untransform the current pointer pos
	DocCoord UTPointerPos = GetUTPosition(PointerPos);
	const DocCoord UTCentre = OriginalShape->GetUTCentrePoint();
	const DocCoord Centre = OriginalShape->GetCentrePoint();
	Matrix OriginalMat;
	OriginalShape->GetTransformMatrix(&OriginalMat);
	DocCoord UTStartPos = GetUTPosition(StartMousePos);
	DocCoord UTOppositePos = DocCoord::PositionPointFromRatio(UTCentre, UTStartPos, -1.0);
	DocCoord OppositePos = UTOppositePos;
	OriginalMat.transform(&OppositePos);

	// Get rotation of original and current dragging points
	DocCoord StartDragPoint = StartMousePos - OppositePos;
	double OrigRotation = atan2((double)StartDragPoint.y, (double)StartDragPoint.x);

	// Get the scaling factor
	const double OriginalLength = OppositePos.Distance(StartMousePos);
	const double Ax = (StartMousePos.x-OppositePos.x);
	const double Ay = (StartMousePos.y-OppositePos.y);
	const double Bx = (PointerPos.x-OppositePos.x);
	const double By = (PointerPos.y-OppositePos.y);
	const double NewLength = ((Ax*Bx) + (Ay*By)) / sqrt((Ax*Ax) + (Ay*Ay));
	double ScaleFactor = NewLength/OriginalLength;

	// Transform to origin, rotate so drag point to opposite lies on the origin,
	// scale, rotate back to current rotation, then transform back
	Matrix Transform(-OppositePos.x, -OppositePos.y);
	Transform*= Matrix(ANGLE(-(OrigRotation*(180/PI))));
	Transform*=	Matrix(fixed16(ScaleFactor),fixed16(1.0));
	Transform*= Matrix(ANGLE(OrigRotation*(180/PI)));
	Transform*= Matrix(OppositePos.x, OppositePos.y);
	Trans2DMatrix	Trans(Transform);

	// Copy the original shape then apply the transform
	delete EditShape;
	EditShape = (NodeRegularShape*)OriginalShape->SimpleCopy();
	if (EditShape != NULL)
	{
		EditShape->Transform(Trans);
		transMat = Transform;
		return TRUE;
	}
	else
		return FALSE;
}



/********************************************************************************************

>	BOOL OpDragRegularShape::ProcessRectangle( DocCoord PointerPos, ClickModifiers Mods )

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/04/95
	Inputs:		PointerPos - The current position of the mouse in Doc Coords
				Mods - the drag modifiers
	Purpose:	This is called to update a rectangular EditShape after a mouse movement 
				during a drag on one if its corners
	SeeAlso:	OpDragRegularShape::DragPointerMove, OpDragRegularShape::ProcessEllipse

********************************************************************************************/

BOOL OpDragRegularShape::ProcessRectangle( DocCoord PointerPos, ClickModifiers Mods )
{
	// If this is an adjust drag then work around the centre
	if (Mods.Adjust)
	{
		if (Mods.Constrain)
			WheelConstrain(&PointerPos, StartMousePos);

		return ProcessRadius(PointerPos, Mods);
	}

	// See which line the mouse is nearest to determine the type of the drag
	if (Mods.Constrain)
	{
		double Diagonal = DocCoord::DistanceFromLine(RectangleArray[2], RectangleArray[0], PointerPos);
		double Prev = DocCoord::DistanceFromLine(RectangleArray[2], RectangleArray[1], PointerPos);
		double Next = DocCoord::DistanceFromLine(RectangleArray[2], RectangleArray[3], PointerPos);

		if ((Diagonal <= Prev) && (Diagonal <= Next))
			return ProcessRectangleDiagonal(PointerPos, Mods);
		else
		{
			if (Prev <= Next)
				return ProcessRectangleEdge(PointerPos, RectangleArray[1]);
			else
				return ProcessRectangleEdge(PointerPos, RectangleArray[3]);
		}
	}
	else
		return ProcessRectangleDiagonal(PointerPos, Mods);
}



/********************************************************************************************
>	BOOL OpDragRegularShape::ProcessRectangleDiagonal( DocCoord PointerPos, ClickModifiers Mods )

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/95
	Inputs:		PointerPos - The current position of the mouse in Doc Coords
				Mods - the drag modifiers
	Purpose:	This is called to update a rectangular EditShape after a mouse movement 
				during a drag on one if its corners.  The rectangle is resized about the
				opposite corner to the drag corner.
	SeeAlso:	OpDragRegularShape::ProcessRectangleEdge
********************************************************************************************/
BOOL OpDragRegularShape::ProcessRectangleDiagonal( DocCoord PointerPos, ClickModifiers Mods )
{
	// Constrain the mouse so the aspect of the shape is maintained
	if (Mods.Constrain)
	{
		// Get the scaling factor
		const double RectDiagonal = RectangleArray[0].Distance(RectangleArray[2]);
		const double Ax = (RectangleArray[2].x-RectangleArray[0].x);
		const double Ay = (RectangleArray[2].y-RectangleArray[0].y);
		const double Bx = (PointerPos.x-RectangleArray[0].x);
		const double By = (PointerPos.y-RectangleArray[0].y);
		const double NewLength = ((Ax*Bx) + (Ay*By)) / sqrt((Ax*Ax) + (Ay*Ay));
		double ratio = NewLength/RectDiagonal;

		// reposition the pointer
		PointerPos = DocCoord::PositionPointFromRatio(RectangleArray[0], RectangleArray[2], ratio);
	}

   	// Snap the mouse point to the active grid
	DocView::SnapCurrent(StartSpread,&PointerPos);

	// Get the shape matrix and its inverse
	Matrix ShapeMat;
	OriginalShape->GetTransformMatrix(&ShapeMat);
	Matrix InvShapeMat = ShapeMat;
	InvShapeMat = InvShapeMat.Inverse();

	// Copy the vertext array for changing
	DocCoord VertexArray[4];
	for (INT32 loop = 0; loop < 4; loop++)
		VertexArray[loop] = RectangleArray[loop];

	// Get the matrix to take the original shape down to the unit square
	double OrigA = RectangleArray[3].x - RectangleArray[0].x;
	double OrigB = RectangleArray[3].y - RectangleArray[0].y;
	double OrigC = RectangleArray[1].x - RectangleArray[0].x;
	double OrigD = RectangleArray[1].y - RectangleArray[0].y;
	double OrigE = RectangleArray[0].x;
	double OrigF = RectangleArray[0].y;

	// Invert this
	double Det = OrigA*OrigD - OrigB*OrigC;
	ERROR2IF(Det==0.0, FALSE, "Matrix Inversion Failed");
	double InvertA = OrigD / Det;
	double InvertB = -OrigB / Det;
	double InvertC = -OrigC / Det;
	double InvertD = OrigA / Det;
	double InvertE = ((OrigC*OrigF) - (OrigE*OrigD))/Det;
	double InvertF = -(((OrigA*OrigF) - (OrigE*OrigB))/Det);

	// Now transform the vertex points
	// Calculate the offsets from the old point and the fixed point
	double dx0 = PointerPos.x - RectangleArray[2].x;
	double dy0 = PointerPos.y - RectangleArray[2].y;

	double dx3 = RectangleArray[1].x - RectangleArray[0].x;
	double dy3 = RectangleArray[1].y - RectangleArray[0].y;

	double dx4 = RectangleArray[3].x - RectangleArray[0].x;
	double dy4 = RectangleArray[3].y - RectangleArray[0].y;
	
	// Calculate the bits that we need to know
	double dx1 = dx3 * ((dy0*dx4-dy4*dx0) / (dx4*dy3-dy4*dx3));
	double dy1 = dy3 * ((dy0*dx4-dy4*dx0) / (dx4*dy3-dy4*dx3));
	double dx2 = dx4 * ((dx3*dy0-dy3*dx0) / (dy4*dx3-dx4*dy3));
	double dy2 = dy4 * ((dx3*dy0-dy3*dx0) / (dy4*dx3-dx4*dy3));

	// Change the parallelogram
	VertexArray[1].x = RectangleArray[1].x + (INT32)dx1;
	VertexArray[1].y = RectangleArray[1].y + (INT32)dy1;
	VertexArray[3].x = RectangleArray[3].x + (INT32)dx2;
	VertexArray[3].y = RectangleArray[3].y + (INT32)dy2;
	VertexArray[2] = PointerPos;

	// Get the array from unit to the new position
	double NewA = VertexArray[3].x - VertexArray[0].x;
	double NewB = VertexArray[3].y - VertexArray[0].y;
	double NewC = VertexArray[1].x - VertexArray[0].x;
	double NewD = VertexArray[1].y - VertexArray[0].y;
	double NewE = VertexArray[0].x;
	double NewF = VertexArray[0].y;

	// multiply the two matrices
	double FinalA =  (InvertA * NewA) + (InvertB * NewC);
	double FinalB =  (InvertA * NewB) + (InvertB * NewD);
	double FinalC =  (InvertC * NewA) + (InvertD * NewC);
	double FinalD =  (InvertC * NewB) + (InvertD * NewD);
	INT32 FinalE =  (INT32)(NewA*InvertE + NewC*InvertF + NewE);
	INT32 FinalF =  (INT32)(NewB*InvertE + NewD*InvertF + NewF);

	// apply to the rectangle
	Trans2DMatrix Transform(Matrix(FinalA, FinalB, FinalC, FinalD, FinalE, FinalF));

	delete EditShape;
	EditShape = new (NodeRegularShape);
	if (EditShape != NULL)
		OriginalShape->CopyNodeContents(EditShape);
	else
		return FALSE;

	EditShape->Transform(Transform);
	transMat = Transform.GetMatrix();

	return TRUE;
}



/********************************************************************************************
>	BOOL OpDragRegularShape::ProcessRectangleEdge( DocCoord PointerPos, DocCoord OtherEnd )

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/95
	Inputs:		PointerPos - The current position of the mouse in Doc Coords
				OtherEnd - the other corner of the rectangle from the corner being dragged,
				the drag is constrained along this line
	Purpose:	This is called during a rectangle drag to resize the rectangle about either
				edge
	SeeAlso:	OpDragRegularShape::ProcessRectangleDiagonally
********************************************************************************************/
BOOL OpDragRegularShape::ProcessRectangleEdge( DocCoord PointerPos, DocCoord OtherEnd )
{
	// Work in untransformed space to remove skew
	const DocCoord DraggingEnd = GetUTPosition(RectangleArray[2]); 
	OtherEnd = GetUTPosition(OtherEnd);
	PointerPos = GetUTPosition(PointerPos);

	Matrix TransMat;
	OriginalShape->GetTransformMatrix(&TransMat);
	TransMat = TransMat.Inverse();

	// Translate rectangle so other (stationary) corner is on origin
	TransMat *= Matrix(-OtherEnd.x, -OtherEnd.y);

	// Rotate rectangle so drag edge lies along x-axis
	double RotAngle = atan2((double)DraggingEnd.y-OtherEnd.y, (double)DraggingEnd.x-OtherEnd.x);
	TransMat *= Matrix(ANGLE(-RotAngle*(180/PI)));

	// Scale along x-axis
	double OrigDragEdgeLength = OtherEnd.Distance(DraggingEnd);
	double c = OtherEnd.Distance(PointerPos);
	double a = DocCoord::DistanceFromLine(OtherEnd, DraggingEnd, PointerPos);
	double NewDragEdgeLength = sqrt((c*c) - (a*a));
	double ScaleFactor = (OrigDragEdgeLength == 0.0) ? 1.0 : NewDragEdgeLength/OrigDragEdgeLength;
	if (!IsJoinAcute(&OtherEnd, &DraggingEnd, &PointerPos))
		ScaleFactor = -ScaleFactor;
	TransMat *= Matrix (FIXED16(ScaleFactor), FIXED16(1.0));

	// Rotate back
	TransMat *= Matrix(ANGLE(RotAngle*(180/PI)));

	// Translate back
	TransMat *= Matrix(OtherEnd.x, OtherEnd.y);

	// And finally apply the original transform back
	Matrix OrigMatrix;
	OriginalShape->GetTransformMatrix(&OrigMatrix);
	TransMat *= OrigMatrix;
	
	// Apply this to the edit shape
	delete EditShape;
	EditShape = new (NodeRegularShape);
	if (EditShape != NULL)
	{	
		OriginalShape->CopyNodeContents(EditShape);

		Trans2DMatrix	matrix( TransMat );
		EditShape->Transform( matrix );
		transMat = TransMat;

		return TRUE;
	}
	else
		return FALSE;
}



/********************************************************************************************

>	BOOL OpDragRegularShape::ProcessCentre( DocCoord PointerPos, ClickModifiers ClickMods )

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/12/94
	Inputs:		PointerPos - The current position of the mouse in Doc Coords
				ClickMods - Which key modifiers are being pressed
	Purpose:	This is called to update the EditShape after a mouse movement in a centre point
				drag.  The shape is being translated.  Constrain works from angles from the
				original centre point of the shape.
	SeeAlso:	OpDragRegularShape::DragPointerMove

********************************************************************************************/

BOOL OpDragRegularShape::ProcessCentre( DocCoord PointerPos, ClickModifiers ClickMods )
{
	// Constrain and snap the mouse point if required
	DocCoord OldCentre = OriginalShape->GetCentrePoint();
	if (ClickMods.Constrain)
		DocView::ConstrainToAngle(OldCentre, &PointerPos);
	DocView::SnapCurrent(StartSpread,&PointerPos);

	// Translate shape based on the movement.
	DocCoord Offset = PointerPos - EditShape->GetCentrePoint();														
	Trans2DMatrix Trans(Offset.x, Offset.y);
	EditShape->Transform(Trans);

	return TRUE;
}



/********************************************************************************************

>	BOOL OpDragRegularShape::ProcessRadius( DocCoord PointerPos, ClickModifiers ClickMods)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/2/95
	Inputs:		PointerPos - The current position of the mouse in Doc Coords
				ClickMods - Which key modifiers are being pressed
	Purpose:	This is called to process drags on the primary points and on the major axes.
	SeeAlso:	-

********************************************************************************************/

BOOL OpDragRegularShape::ProcessRadius(DocCoord Pos, ClickModifiers Mods)
{
   	// Snap the mouse point to the active grid
	DocView::SnapCurrent(StartSpread,&Pos);

	// Untransform the current pointer pos
	DocCoord UTPointerPos = GetUTPosition(Pos);
	const DocCoord UTCentre = OriginalShape->GetUTCentrePoint();
	const DocCoord Centre = OriginalShape->GetCentrePoint();

	// Get rotation of original and current primary points
	DocCoord CurrentPrimary = StartMousePos - Centre;
	DocCoord NewPrimary = Pos - Centre;
	double OrigPrimaryRotation = atan2((double)CurrentPrimary.y, (double)CurrentPrimary.x);
	double NewPrimaryRotation = atan2((double)NewPrimary.y, (double)NewPrimary.x);
	ANGLE RotBy(-(OrigPrimaryRotation - NewPrimaryRotation)*(180/PI));

	// Get the scaling factor
	double NewLength = Pos.Distance(Centre);
	double CurrentLength = StartMousePos.Distance(Centre);
	double ScaleFactor = (CurrentLength != 0.0) ? (NewLength/CurrentLength) : 0.001;

	// Transform to origin, scale, rotate then transform back
	Matrix Transform(-Centre.x, -Centre.y);
	Transform*= Matrix(RotBy);
	Transform*=	Matrix(fixed16(ScaleFactor),fixed16(ScaleFactor));
	Transform*= Matrix(Centre.x, Centre.y);
	Trans2DMatrix	Trans(Transform);

	// Copy the original shape then apply the transform
	delete EditShape;
	EditShape = (NodeRegularShape*)OriginalShape->SimpleCopy();
	if (EditShape != NULL)
	{
		EditShape->Transform(Trans);
		transMat = Transform;
		return TRUE;
	}
	else
		return FALSE;
}



/********************************************************************************************

>	void OpDragRegularShape::WheelConstrain(DocCoord* PointerPos, const DocCoord StartPos)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/1/94
	Inputs:		PointerPos - The current position of the mouse in Doc Coords
				StartPos - The outer constrain location.
	Outputs:	PointerPos is constrained to the shape described below
	Returns:	-
	Purpose:	This is called to constrain a point to a steering-wheel shape.  The wheel is
				centred on the centre of OriginalShape.  It has radius of the distance from
				the centre to StartPos.  The other constrain line is the line passing through
				the centre point and StartPos.
	SeeAlso:	-

********************************************************************************************/

void OpDragRegularShape::WheelConstrain(DocCoord* PointerPos, const DocCoord StartPos)
{
	// Obtain perpendicular distance from StartPos
	DocCoord LineConstrainPoint = StartPos;
	DocCoord RotStartPos = StartPos;
	Matrix Mat(-OriginalShape->GetCentrePoint().x, -OriginalShape->GetCentrePoint().y);
	Mat *= Matrix(ANGLE(90));
	Mat *= Matrix(OriginalShape->GetCentrePoint().x, OriginalShape->GetCentrePoint().y);
	Mat.transform(&RotStartPos);
	double LineDistance1 = DocCoord::DistanceFromLine(OriginalShape->GetCentrePoint(), StartPos, *PointerPos);
	double LineDistance2 = DocCoord::DistanceFromLine(OriginalShape->GetCentrePoint(), RotStartPos, *PointerPos);
	BOOL UseRotated = FALSE;
	double LineDistance = LineDistance1;
	if (LineDistance2 < LineDistance)
	{
		UseRotated = TRUE;
		LineDistance = LineDistance2;
	}

	// Now obtain distance the pointer position is from the circumference of the circle
	double Radius = OriginalShape->GetCentrePoint().Distance(StartPos);
	const double CurrentRadius = OriginalShape->GetCentrePoint().Distance(*PointerPos);
	const double CircleDistance = fabs(Radius - CurrentRadius);

	Radius = OriginalShape->GetCentrePoint().Distance(StartPos);

/* 	// Now if the pointer is past the centre point on the other side of the shape from StartPos
	// then we want the distance from the centre point
	if (!OriginalShape->IsCircular())
	{
		DocCoord StartOrig = StartPos - OriginalShape->GetCentrePoint();
		DocCoord PointerOrig = *PointerPos - OriginalShape->GetCentrePoint();
		double StartAngle = atan2((double)StartOrig.y, (double)StartOrig.x);
		double PointerAngle = atan2((double)PointerOrig.y, (double)PointerOrig.x);
		if ((PointerAngle < StartAngle-(PI/2)) || (PointerAngle > StartAngle+(PI/2)) )
			LineDistance = CircleDistance*2;
	}*/

	// Now constrain to either the line or the circumference
	if ((CircleDistance < LineDistance) && (CurrentRadius >= 0.0001))
	{
		// Constrain to the circle circumference
		*PointerPos = DocCoord::PositionPointFromRatio(OriginalShape->GetCentrePoint(), *PointerPos, Radius/CurrentRadius);
	}
	else
	{
		// Constrain to the line
		if (UseRotated)
			*PointerPos = DocCoord::PositionPointFromRatio(OriginalShape->GetCentrePoint(), RotStartPos, CurrentRadius/Radius);
		else
			*PointerPos = DocCoord::PositionPointFromRatio(OriginalShape->GetCentrePoint(), StartPos, CurrentRadius/Radius);
	}
}






/********************************************************************************************

>	void OpDragRegularShape::GetUTPosition(DocCoord TransedPos)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/2/95
	Inputs:		TransedPos - A position in the OriginalShape's translated space
	Outputs:	-
	Returns:	The position untranslated back to the OriginalShape
	Purpose:	This is called to convert a current mouse postion to untransform back to the
				coordinates used by the untranslated OriginalShape.
	SeeAlso:	-

********************************************************************************************/

DocCoord OpDragRegularShape::GetUTPosition(DocCoord TransedPos)
{
	Matrix TransMat;
	OriginalShape->GetTransformMatrix(&TransMat);
	TransMat = TransMat.Inverse();
	TransMat.transform(&TransedPos);

	return TransedPos;
}



/********************************************************************************************

>	void OpDragRegularShape::RenderDragBlobs( DocRect Rect, SPread* pSpread, BOOL bSolidDrag )

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/12/94
	Purpose:	Renders the current version of the shape to the window.

********************************************************************************************/

void OpDragRegularShape::RenderDragBlobs( DocRect Rect, Spread *pSpread, BOOL bSolidDrag )
{
	RenderRegion* pRegion = DocView::RenderOnTop(NULL, pSpread, ClippedEOR );
	while ( pRegion )
	{
		// Set the line colour 
		pRegion -> SetFillColour(COLOUR_NONE);
		pRegion -> SetLineColour(COLOUR_XOREDIT);

		// And draw an EORed version of how the shape will turn out
		EditShape->RenderEorDrag( pRegion );

		// Get the Next render region
		pRegion = DocView::GetNextOnTop(NULL);
	}
}



/********************************************************************************************
>	BOOL OpDragRegularShape::IsJoinAcute(const DocCoord* pJoin, const DocCoord* pOtherEndOfJoin, const DocCoord* pOtherPoint);

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/95
	Inputs:		pJoin - the coordinate where two lines join
				pOtherEndOfJoin - another point that with pJoin defines a line
				pOtherPoint - another point that with pJoin defines another line
	Outputs:	-
	Returns:	TRUE if the angle between the two lines is an acute angle
	Purpose:	For seeing the perpendicual distance from pOtherPoint to the line
				pJoin-pOtherEndOfJoin end on the line or before pJoin.  Used when dragging
				along a line to see if the drag point has gon back beyond the start of the
				line.
********************************************************************************************/
BOOL OpDragRegularShape::IsJoinAcute(const DocCoord* pJoin, const DocCoord* pOtherEndOfJoin, const DocCoord* pOtherPoint)
{
	return DocCoord::IsJoinAcute(pJoin, pOtherEndOfJoin, pOtherPoint);
}



/********************************************************************************************

>	BOOL OpDragRegularShape::Init()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/12/94
	Returns:	TRUE if all went OK, False otherwise
	Purpose:	Adds the operation to the list of all known operations

********************************************************************************************/

BOOL OpDragRegularShape::Init()
{
	return (RegisterOpDescriptor(	0, 
									_R(IDS_EDITREGULARSHAPEOP),
									CC_RUNTIME_CLASS(OpDragRegularShape), 
									OPTOKEN_DRAGREGULARSHAPE,
									OpDragRegularShape::GetState));
}


/********************************************************************************************

>	OpState OpDragRegularShape::GetState(String_256* Description, OpDescriptor*)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/12/94
	Outputs:	-
	Returns:	The state of the operation, so that menu items (ticks and greying can be
				done properly
	Purpose:	Find out the state of the operation at the specific time

********************************************************************************************/

OpState OpDragRegularShape::GetState(String_256* Description, OpDescriptor*)
{
	OpState Blobby;
	
	return Blobby;
}



/********************************************************************************************

>	OpReformShapeEdge::OpReformShapeEdge()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/12/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Constructor - set member variables to default values.

********************************************************************************************/

OpReformShapeEdge::OpReformShapeEdge()
{
	StartSpread = NULL;
	EditShape = NULL;
	EditEdge = NULL;
}



/********************************************************************************************

>	void OpReformShapeEdge::DoStartDragEdit(NodeRegularShape* OrigShape, DocCoord Anchor,
												Spread *pSpread, INT32 SegmentIndex, double pdist)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com> - from Jim code in OpReshapeOrAddPoint
	Created:	19/12/95
	Inputs:		OrigShape - The original shape object
				Anchor - The position of the mouse at the start of the Drag
				pSpread - the spread that the drag started on
				SegmentIndex - The Coord number of the control point that we are dragging
				pdist - parametric distance along the curve we're dragging from (0 to 1)
	Purpose:	This is called when a Drag operation has been started on the edge of a shape.
				It will try and reshape the edge as it's dragged according to the maths in the 
				function RecalculatePath
	SeeAlso:	OpReformShapeEdge::RecalculatePath
				

********************************************************************************************/

void OpReformShapeEdge::DoStartDragEdit(NodeRegularShape* OrigShape, BOOL EdgeOne, DocCoord Anchor,
											    Spread *pSpread, INT32 SegmentIndex)
{
	// We had better take a note of the starting point of the drag
	StartMousePos = Anchor;
	UTStartMousePos = Anchor;			// We'll untransform this later in BuildEditShape
	StartSpread  = pSpread;
	OriginalShape = OrigShape;
	Index = SegmentIndex;
	EdgeDrag = EdgeOne ? DRAG_EDGE1 : DRAG_EDGE2;

	// We need to make a version of the shape that we can change
	if (!BuildEditEdge())
	{
		// Inform the person doing the clicking that life is not looking so good
		InformError( _R(IDS_OUT_OF_MEMORY), _R(IDS_OK) );
		End();
		return;
	}

	// Render the bits of the path that are different
	DocRect EditPathBBox = EditShape->GetBoundingRect();
	RenderDragBlobs(EditPathBBox, pSpread, FALSE);

	// And tell the Dragging system that we need drags to happen
	StartDrag(DRAGTYPE_AUTOSCROLL, &EditPathBBox, &StartMousePos);
}



/********************************************************************************************

>	void OpReformShapeEdge::DragPointerMove( DocCoord PointerPos, 
												ClickModifiers ClickMods, Spread *pSpread, BOOL bSolidDrag)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/12/94
	Inputs:		PointerPos - The current position of the mouse in Doc Coords
				ClickMods - Which key modifiers are being pressed
				pSpread - the spread that the cursor is over now
	Purpose:	This is called every time the mouse moves, during a drag.
	SeeAlso:	ClickModifiers

********************************************************************************************/

void OpReformShapeEdge::DragPointerMove( DocCoord PointerPos, ClickModifiers ClickMods, Spread *pSpread, BOOL bSolidDrag)
{
	// If drag has moved onto a different spread, convert the coord to be relative to the
	// original spread.
	if (pSpread != StartSpread)
		PointerPos = MakeRelativeToSpread(StartSpread, pSpread, PointerPos);

	// Now snap it to the current grid
	DocView::SnapCurrent(StartSpread,&PointerPos);

	// Rub out the old EORed version of the path
	RenderDragBlobs( EditShape->GetBoundingRect(), StartSpread, bSolidDrag );

	// This is the bit where we go off and re-calculate the edge path,
	// based on how much the mouse has moved.  We need to work in untransfomed coordinates
	DocCoord Offset = PointerPos;
	Matrix TransMat;
	EditShape->GetTransformMatrix(&TransMat);
	TransMat = TransMat.Inverse();
	TransMat.transform(&Offset);
	Offset = Offset-UTStartMousePos;

	RecalculateEdge( Offset );
	if (!BuildEditShape(ClickMods.Constrain || !OriginalShape->IsStellated(), ClickMods.Adjust))
	{
		InformError();
		End();
	}

	// Draw in the new version of the path and update the Last Mouse Position
	RenderDragBlobs( EditShape->GetBoundingRect(), StartSpread, bSolidDrag );
}


/********************************************************************************************

>	void OpReformShapeEdge::DragFinished( DocCoord PointerPos, ClickModifiers ClickMods, Spread* pSpread, BOOL Success, BOOL bSolidDrag)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/12/94
	Inputs:		PointerPos - The position of the mouse at the end of the drag
				ClickMods - the key modifiers being pressed
				pSpread - pointer to the spared the drag ended on.
				Success - TRUE if the drag was terminated properly, FALSE if it
				was ended with the escape key being pressed
	Purpose:	This is called when a drag operation finishes.
	SeeAlso:	ClickModifiers

********************************************************************************************/

void OpReformShapeEdge::DragFinished( DocCoord PointerPos, ClickModifiers ClickMods, Spread* pSpread, BOOL Success, BOOL bSolidDrag)
{
	RenderDragBlobs( EditEdge->GetBoundingRect(), StartSpread, bSolidDrag );
	EndDrag();
	BeginSlowJob();

	BOOL Failed = FALSE;

	if ( Success )
	{
		// Start the undo ball rolling
		Failed = !DoStartSelOp(FALSE, TRUE, FALSE, TRUE);

		if (!Failed)
		{
			Failed = RecalcBoundsAction::DoRecalc(this, &UndoActions, OriginalShape) == AC_FAIL;
		}

		// Will the original shape allow the op to happen?
		ObjChangeFlags cFlags;
		ObjChangeParam ObjChange(OBJCHANGE_STARTING,cFlags,OriginalShape,this);
		if (!Failed)
			Failed = !OriginalShape->AllowOp(&ObjChange);

		if (!Failed)
		{
			if ((EdgeDrag == DRAG_EDGE1) || ClickMods.Constrain)
			{
				Failed = ChangeShapePathAction::DoReshape(this, &UndoActions, OriginalShape, 
					ChangeShapePathAction::CHANGE_PATH1, &(OriginalShape->EdgePath1)) == AC_FAIL ;
			}
			if ((EdgeDrag == DRAG_EDGE2) || ClickMods.Constrain || !OriginalShape->IsStellated())
			{
				if (!Failed)
				{
					Failed = ChangeShapePathAction::DoReshape(this, &UndoActions, OriginalShape, 
						ChangeShapePathAction::CHANGE_PATH2, &(OriginalShape->EdgePath2)) == AC_FAIL ;
				}
			}
		}
										
		// Now do the change
		if (!Failed)
		{
			if ((EdgeDrag == DRAG_EDGE1) || ClickMods.Constrain )
			{
				OriginalShape->EdgePath1.ClearPath();
				OriginalShape->EdgePath1.CopyPathDataFrom(&(EditShape->EdgePath1));		
			}
			if ((EdgeDrag == DRAG_EDGE2) || ClickMods.Constrain || !OriginalShape->IsStellated() )
			{
				OriginalShape->EdgePath2.ClearPath();
				OriginalShape->EdgePath2.CopyPathDataFrom(&(EditShape->EdgePath2));		
			}

			// Recalculate the path's bounding box
			OriginalShape->InvalidateBoundingRect();
			OriginalShape->InvalidateCache();
	
			// tell the world that something in the selection has changed 
			// so that selection bounds are updated
			GetApplication()->FindSelection()->Update(TRUE);

			Failed = RecordBoundsAction::DoRecord(this, &UndoActions, OriginalShape) == AC_FAIL ;
		}

		if (!Failed)
		{
			// Update effected parents
			ObjChange.Define(OBJCHANGE_FINISHED,cFlags,OriginalShape,this);
			Failed = !UpdateChangedNodes(&ObjChange);
		}
	}
	else
	{
		// Set up the flags that say it all went wrong
		Failed = TRUE;
	}

	delete (EditShape);
	delete (EditEdge);

	if (Failed)
		FailAndExecute();
	End();		
}



/********************************************************************************************

>	BOOL OpReformShapeEdge::BuildEditEdge()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/12/94
	Returns:	TRUE if it managed to build the path, FALSE if it failed
	Purpose:	Builds a copy of the shape and the edge path that we can edit, without
				destroying the original data
	Errors:		If it runs out of memory then it will return FALSE

********************************************************************************************/

BOOL OpReformShapeEdge::BuildEditEdge()
{
	// Copy the shape
	if ( (EditShape = new (NodeRegularShape)) == NULL)
		return FALSE;
	OriginalShape->CopyNodeContents(EditShape);

	// Copy the edge path
	if ( (EditEdge = new (Path)) == NULL)
		return FALSE;

	// We need to make a copy of the edge path as it appears in the shape.
	INT32 Count1 = OriginalShape->EdgePath1.GetNumCoords();
	INT32 Count2 = OriginalShape->EdgePath2.GetNumCoords();
	if (Count1 == 2)
		Count1 = 4;
	if (Count2 == 2)
		Count2 = 4;
	Path* RenderPath;

	if (!OriginalShape->BuildShapePath(&RenderPath))
		return FALSE;

	// Run through the path makeing linetos into straight curvetos
	BOOL FoundLine = TRUE;
	while (FoundLine)
	{
		INT32 NumCoords = RenderPath->GetNumCoords();
		
		// find a lineto
		INT32 loop = 0;
		while ((loop < NumCoords) && ((RenderPath->GetVerbArray()[loop] & ~PT_CLOSEFIGURE) != PT_LINETO))
		{
			loop ++;
		}

		if (loop < NumCoords)
		{
			DocCoord* EdgeCoords = RenderPath->GetCoordArray();
			DocCoord CP1 (EdgeCoords[loop-1].x + (EdgeCoords[loop].x-EdgeCoords[loop-1].x)/3,
										EdgeCoords[loop-1].y + (EdgeCoords[loop].y-EdgeCoords[loop-1].y)/3);
			DocCoord CP2 (EdgeCoords[loop].x + (EdgeCoords[loop-1].x-EdgeCoords[loop].x)/3,
										EdgeCoords[loop].y + (EdgeCoords[loop-1].y-EdgeCoords[loop].y)/3);
			DocCoord End = EdgeCoords[loop];
			RenderPath->SetPathPosition(loop);
			RenderPath->InsertCurveTo(CP1,CP2,End);
			RenderPath->DeleteSection(loop+3,1);
		}
		else
			FoundLine = FALSE;
	}

	// Now calculate paramdist
	RenderPath->SqrDistanceToPoint(StartMousePos, &Index, &paramdist);

	// Now set Index to point to the first element in the edge
	PathFlags*	RenderFlags = RenderPath->GetFlagArray();
	while ((!RenderFlags[Index].IsEndPoint) && (Index >= 0) )
	{
		Index--;
	}

	INT32 EndCount = EdgeDrag==DRAG_EDGE1 ? Count1 : Count2;

	// Ask for a path with the required number of free slots in it
	if (!EditEdge->Initialise(EndCount, 1))
		return FALSE;

	// now copy the data from the appropitae place in RenderPath into EdgeEdge
	if (!EditEdge->CopySectionFrom(*RenderPath, Index, EndCount))
		return FALSE;

	// Set the first verb to be a MoveTo and clear any closefigure from the last verb
	PathVerb*	EdgeVerbs = EditEdge->GetVerbArray();
	DocCoord*	EdgeCoords = EditEdge->GetCoordArray();
	EdgeVerbs[0] = PT_MOVETO;
	EdgeVerbs[EditEdge->GetNumCoords()-1] = EdgeVerbs[EditEdge->GetNumCoords()-1] & ~PT_CLOSEFIGURE;

	// Untransform the path 
	Matrix TransMat;
	EditShape->GetTransformMatrix(&TransMat);
	TransMat = TransMat.Inverse();
	TransMat.transform((Coord*)EdgeCoords, EditEdge->GetNumCoords());

	// We also need an untransformed starting point of the drag
	TransMat.transform(&UTStartMousePos);

	// Record the original positions of the two control points of the drag segment
	// NOTE Assumes that the edit path consists of one segment
	if (EdgeVerbs[1] == PT_BEZIERTO)
	{
		OrigControl1 = EdgeCoords[1];
		OrigControl2 = EdgeCoords[2];
	}
	Index = 1;
 
	return TRUE;
}



/********************************************************************************************

>	BOOL OpReformShapeEdge::BuildEditShape(BOOL CopyToBoth, BOOL ShouldMirror)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/12/94
	Inputs:		CopyToBoth - TRUE if the status of EdgeDrag is to be ignored and the new edge
				should be applied to both edges of the shape.
	Outputs:	-
	Returns:	TRUE if the build succeded, FALSE if it failed
	Purpose:	Builds a copy of the shape into EditShape based upon the current edge path
	Errors:		-

********************************************************************************************/

BOOL OpReformShapeEdge::BuildEditShape(BOOL CopyToBoth, BOOL ShouldMirror)
{
	ERROR2IF(EditShape == NULL, FALSE, "Can't build into a non-existant shape!");

	BOOL Success = TRUE;
	if (CopyToBoth)
	{
		Path* RevPath = new Path;
		Path* Path1;
		Path* Path2;

		if ((RevPath == NULL) || !ReversePath(RevPath, ShouldMirror))
		{
			if (RevPath != NULL)
				delete RevPath;
			RevPath = EditEdge;
		}

		if (EdgeDrag == DRAG_EDGE1)
		{
			Path1 = EditEdge;
			Path2 = RevPath;
		}
		else
		{
			Path1 = RevPath;
			Path2 = EditEdge;
		}

		Success = EditShape->EdgePath1.CopyPathDataFrom(Path1);
		if (Success)
			Success = EditShape->EdgePath2.CopyPathDataFrom(Path2);	

		if (RevPath != EditEdge)
			delete RevPath;
	}
	else
	{
		if (EdgeDrag == DRAG_EDGE1)
		{
			Success = EditShape->EdgePath1.CopyPathDataFrom(EditEdge);
			if (Success)
				Success = EditShape->EdgePath2.CopyPathDataFrom(&(OriginalShape->EdgePath2));		
		}
		else
		{
			Success = EditShape->EdgePath2.CopyPathDataFrom(EditEdge);
			if (Success)
				Success = EditShape->EdgePath1.CopyPathDataFrom(&(OriginalShape->EdgePath1));		
		}
	}

	if (!Success)
	{
		Error::SetError(_R(IDS_OUT_OF_MEMORY));
	}

	EditShape->InvalidateCache();

	return Success;
}



/********************************************************************************************

>	void OpReformShapeEdge::RecalculateEdge( DocCoord Offset )

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com> (stolen from Jim code)
	Created:	20/12/94
	Inputs:		Offset - The Amount the pointer has moved from the *original* position (i.e. not
						the last position).
	Purpose:	Will reshape the edge being edited using Jims curve distorting method
	SeeAlso:	OpReshapeOrAddPoint::RecalculatePath

********************************************************************************************/

void OpReformShapeEdge::RecalculateEdge( DocCoord Offset )
{
	const double factor = 0.656875;
	PathVerb* Verbs = EditEdge->GetVerbArray();
	DocCoord* Coords = EditEdge->GetCoordArray();
	if (Verbs[Index] == PT_BEZIERTO)		// first ctrl pt won't have closefigure set
	{
		DocCoord ControlOffset;
		ControlOffset.x = (INT32)(OrigControl1.x + (Offset.x) * factor / paramdist);
		ControlOffset.y = (INT32)(OrigControl1.y + (Offset.y) * factor / paramdist);

		ControlOffset.x -= Coords[Index].x;
		ControlOffset.y -= Coords[Index].y;

		Coords[Index].x += ControlOffset.x;
		Coords[Index].y += ControlOffset.y;
		
		ControlOffset.x = (INT32)(OrigControl2.x + (Offset.x) * factor / (1.0-paramdist));
		ControlOffset.y = (INT32)(OrigControl2.y + (Offset.y) * factor / (1.0-paramdist));

		ControlOffset.x -= Coords[Index+1].x;
		ControlOffset.y -= Coords[Index+1].y;

		Coords[Index+1].x += ControlOffset.x;
		Coords[Index+1].y += ControlOffset.y;
	}
}



/********************************************************************************************

>	void OpReformShapeEdge::ReversePath(Path* NewPath, BOOL ShouldMirror)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/1/95
	Inputs:		NewPath - Points to an uninitialsed path.
				ShouldMirror - TRUE if the path should be mirrored
	Outputs:	NewPath contains a mirror image of EditEdge
	Returns:	TRUE for success, FALSE for failure
	Purpose:	Use to reverse an edit path before copying it to the other edge path
				NOTE: This function assumes that the edge path consists of a MoveTo-BezierTo
	SeeAlso:	-

********************************************************************************************/

BOOL OpReformShapeEdge::ReversePath(Path* NewPath, BOOL ShouldMirror)
{
	const INT32 NumCoords = EditEdge->GetNumCoords();
	ERROR3IF(NumCoords != 4, "Edit edge was not a single Bezier section");

	if (!( NewPath->Initialise(NumCoords,4) && NewPath->CopyPathDataFrom(EditEdge) ))
		return FALSE;

	if (!ShouldMirror)
		NewPath->Reverse();

/*	if (ShouldMirror)
	{
		DocCoord* Points = NewPath->GetCoordArray();
		// Transform point 1
		double angle1 = atan2((double)Points[3].y-Points[0].y, (double)Points[3].x-Points[0].x) * (180/PI);
		double angle2 = atan2((double)Points[1].y-Points[0].y, (double)Points[1].x-Points[0].x) * (180/PI);

		Matrix Trans(-Points[0].x, -Points[0].x);
		Trans *= Matrix(-ANGLE((angle2-angle1)));
		Trans *= Matrix(Points[0].x, Points[0].x);

		Trans.transform(&(Points[1]));

		// Transform point 2
		angle1 = atan2((double)Points[0].y-Points[3].y, (double)Points[0].x-Points[3].x) * (180/PI);
		angle2 = atan2((double)Points[2].y-Points[3].y, (double)Points[2].x-Points[3].x) * (180/PI);

		Trans = Matrix(-Points[3].x, -Points[3].x);
		Trans *= Matrix(-ANGLE((angle2-angle1)));
		Trans *= Matrix(Points[3].x, Points[3].x);

		Trans.transform(&(Points[2]));
	}*/

	return TRUE;
}



/********************************************************************************************

>	BOOL OpReformShapeEdge::Init()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/12/94
	Returns:	TRUE if all went OK, False otherwise
	Purpose:	Adds the operation to the list of all known operations

********************************************************************************************/

BOOL OpReformShapeEdge::Init()
{
	return (RegisterOpDescriptor(	0, 
									_R(IDS_EDITREGULARSHAPEOP),
									CC_RUNTIME_CLASS(OpReformShapeEdge), 
									OPTOKEN_RESHAPESHAPEEDGE,
									OpReformShapeEdge::GetState));
}



/********************************************************************************************

>	OpState OpReformShapeEdge::GetState(String_256* Description, OpDescriptor*)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/12/94
	Outputs:	Description - GetState fills this string with an approriate description
				of the current state of the selector tool
	Returns:	The state of the operation, so that menu items (ticks and greying can be
				done properly
	Purpose:	Find out the state of the operation at the specific time

********************************************************************************************/

OpState OpReformShapeEdge::GetState(String_256* Description, OpDescriptor*)
{
	OpState Blobby;
	
	return Blobby;
}



/********************************************************************************************

>	virtual void OpReformShapeEdge::RenderDragBlobs( DocRect Rect, Spread* pSpread, BOOL bSolidDrag )

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/12/94
	Inputs:		Rect - the clipping rectangle
				pSpread - the spread to draw onto
	Outputs:	-
	Returns:	-
	Purpose:	Renders the drag blobs of this operation.

********************************************************************************************/

void OpReformShapeEdge::RenderDragBlobs( DocRect Rect, Spread* pSpread, BOOL bSolidDrag )
{
	// If being called from DocView::RenderView, then the spread could be wrong - so
	// convert the rectangle if necessary.
	if (pSpread != StartSpread)
	{
		Rect.lo = MakeRelativeToSpread(StartSpread, pSpread, Rect.lo);
		Rect.hi = MakeRelativeToSpread(StartSpread, pSpread, Rect.hi);
	}

	ERROR3IF(EditShape == NULL, "Can't render a non-existant shape!");
	if (EditShape == NULL)
		return;

	// start a rendering loop
	RenderRegion* pRegion = DocView::RenderOnTop(NULL, pSpread, ClippedEOR);
	while (pRegion)
	{
		// Set the line colour 
		pRegion -> SetFillColour(COLOUR_NONE);
		pRegion -> SetLineColour(COLOUR_XOREDIT);

		// Draw the outline
		EditShape->RenderEorDrag(pRegion);

		// Get the Next render region
		pRegion = DocView::GetNextOnTop(NULL);
	}
}




///////////////////////////////////////////////////////////////////////////////////////////
// OpShapeToggleBase


/********************************************************************************************

>	OpShapeToggleBase::OpShapeToggleBase(): SelOperation()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/04/95
	Purpose:	Constructs an OpShapeToggleBase object.

********************************************************************************************/

OpShapeToggleBase::OpShapeToggleBase(): SelOperation()
{
}



/********************************************************************************************

>	void OpShapeToggleBase::Do(OpDescriptor*)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/04/95
	Inputs:		An Operation Descriptor (unused)
	Outputs:	-
	Returns:	-
	Purpose:	Runs through all selected Quickshapes, toggling their one of their member states
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void OpShapeToggleBase::Do(OpDescriptor*)
{
	// Start the operation
	BOOL Success = DoStartSelOp(FALSE, FALSE, FALSE, FALSE);

	// Check with our parent nodes
	ObjChangeParam ObjChange(OBJCHANGE_STARTING,ObjChangeFlags(),NULL,this);

	// Run through the selected nodes
	if (Success)
	{
		SelRange* pSelection = GetApplication()->FindSelection();
		Node* pNode = pSelection->FindFirst();

		while ((pNode != NULL) && Success)
		{
			if (IS_A(pNode, NodeRegularShape))
			{
				NodeRegularShape* pShape = (NodeRegularShape*)pNode;
			
				// Convert this shape if it's parents allow
				if (pShape->AllowOp(&ObjChange))				
				{
					// Create an action to will cause the shape to recalc its bounds
					if (Success)
						Success = (RecalcBoundsAction::DoRecalc(this, &UndoActions, pShape) != AC_FAIL);
					
					// Toggle its ellipseness
					if (Success)
						Success = DoAction(pShape);

					// Invalidate the shapes bounding rect as it's changed
					if (Success)
					{
						pShape->InvalidateBoundingRect();
						pShape->InvalidateCache();
					}

					// And redraw its final state
					if (Success)
						Success = (RecordBoundsAction::DoRecord(this, &UndoActions, pShape) != AC_FAIL);

					// Tell our parents we have finished
					if (Success)
					{
						ObjChange.Define(OBJCHANGE_FINISHED,ObjChangeFlags(),NULL,this);
						Success = UpdateChangedNodes(&ObjChange);
					}
				}
			}

			// Get the next selected thing
			pNode = pSelection->FindNext(pNode);
		}
	}

	// Clean up after any failure
	if (!Success)
	{
		InformError();
		FailAndExecute();
	}

	End();
}



BOOL OpShapeToggleBase::DoAction(NodeRegularShape* pShape)
{
	ERROR3("Should not call OpShapeToggleBase::DoAction"); 
	return FALSE;
}




///////////////////////////////////////////////////////////////////////////////////////////
// OpShapeToggleElipPoly



/********************************************************************************************

>	OpShapeToggleElipPoly::OpShapeToggleElipPoly(): OpShapeToggleBase()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/04/95
	Purpose:	Constructs an OpShapeToggleElipPoly object.

********************************************************************************************/

OpShapeToggleElipPoly::OpShapeToggleElipPoly(): OpShapeToggleBase()
{
}



/********************************************************************************************

>	OpState OpShapeToggleElipPoly::GetState(String_256* UIDescription, OpDescriptor*)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/04/95
	Inputs:		UIDescription - the name of the opertaion to display
				OpDescriptor - To help update the state
	Outputs:	UIDescription - The name of the operation
	Returns:	The state of the operation
	Purpose:	Find the state of the OpShapeToggleElipPoly operation.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

OpState OpShapeToggleElipPoly::GetState(String_256* UIDescription, OpDescriptor*)
{
	OpState OpSt;  
	OpSt.Greyed = TRUE; 
	BOOL FoundEllipse = FALSE;  
	BOOL FoundPolygon = FALSE;  
	ObjChangeFlags cFlags;
	ObjChangeParam ObjChange(OBJCHANGE_STARTING,cFlags,NULL,NULL);

	SelRange* pSelection = GetApplication()->FindSelection();
	Node* pNode = pSelection->FindFirst();

	while (pNode != NULL)
	{
		if (IS_A(pNode, NodeRegularShape))
		{
			NodeRegularShape* pShape = (NodeRegularShape*)pNode;
		
			// Convert this shape if it's parents allow
			if (pShape->AllowOp(&ObjChange))				
			{
				if (pShape->IsCircular())
					FoundEllipse = TRUE;
				else
					FoundPolygon = TRUE;
			}
		}

		// Get the next selected thing
		pNode = pSelection->FindNext(pNode);
	}

	if (FoundEllipse || FoundPolygon)
		OpSt.Greyed = FALSE;
	if (FoundEllipse && !FoundPolygon)
		UIDescription->Load(_R(IDS_OPSHAPETOGGLE_ELIP_PO));
	if (!FoundEllipse && FoundPolygon)
		UIDescription->Load(_R(IDS_OPSHAPETOGGLE_ELIP_EP));

	return(OpSt);	
}                                    



/********************************************************************************************

>	BOOL OpShapeToggleElipPoly::Init()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/4/95
	Inputs:		-
	Outputs:	-
	Returns:	TRUE/FALSE for successful init
	Purpose:	Initialise the toggle ellipseness operation by registering it
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL OpShapeToggleElipPoly::Init()
{
	return(RegisterOpDescriptor(0, 
								_R(IDS_OPSHAPETOGGLE_ELIP),
								CC_RUNTIME_CLASS(OpShapeToggleElipPoly), 
								OPTOKEN_TOGGLEELIPPOLY,
								OpShapeToggleElipPoly::GetState)); 
}



/********************************************************************************************

>	BOOL OpShapeToggleElipPoly::DoAction(NodeRegularShape* pShape)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/4/95
	Inputs:		pShape - points to the shape to toggle
	Outputs:	-
	Returns:	TRUE/FALSE for successful toggle
	Purpose:	Toggles the ellipse/polygon state of a shape
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
BOOL OpShapeToggleElipPoly::DoAction(NodeRegularShape* pShape)
{
	BOOL Success = TRUE;

	if (Success)
		Success = (ChangeShapeDataAction::DoToggle(this, &UndoActions, pShape,
				ChangeShapeDataAction::CHANGE_CIRCULAR, pShape->IsCircular() ? 1.0 : 0.0) != AC_FAIL);
	if (Success)
		pShape->SetCircular(!pShape->IsCircular());

	return Success;
}




///////////////////////////////////////////////////////////////////////////////////////////
// OpShapeToggleCurvature



/********************************************************************************************

>	OpShapeToggleStellation::OpShapeToggleStellation(): OpShapeToggleBase()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/04/95
	Purpose:	Constructs an OpShapeToggleStellation object.

********************************************************************************************/

OpShapeToggleStellation::OpShapeToggleStellation(): OpShapeToggleBase()
{
}



/********************************************************************************************

>	OpState OpShapeToggleStellation::GetState(String_256* UIDescription, OpDescriptor*)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/04/95
	Inputs:		UIDescription - the name of the opertaion to display
				OpDescriptor - To help update the state
	Outputs:	UIDescription - The name of the operation
	Returns:	The state of the operation
	Purpose:	Find the state of the OpShapeToggleStellation operation.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

OpState OpShapeToggleStellation::GetState(String_256* UIDescription, OpDescriptor*)
{
	OpState OpSt;  
	OpSt.Greyed = TRUE; 
	BOOL FoundStellated = FALSE;  
	BOOL FoundNotStellated = FALSE;  
	BOOL AllEllipses = TRUE;
	ObjChangeFlags cFlags;
	ObjChangeParam ObjChange(OBJCHANGE_STARTING,cFlags,NULL,NULL);

	SelRange* pSelection = GetApplication()->FindSelection();
	Node* pNode = pSelection->FindFirst();

	while (pNode != NULL)
	{
		if (IS_A(pNode, NodeRegularShape))
		{
			NodeRegularShape* pShape = (NodeRegularShape*)pNode;
		
			AllEllipses = AllEllipses && pShape->IsCircular();

			// Convert this shape if it's parents allow
			if (pShape->AllowOp(&ObjChange))				
			{
				if (pShape->IsStellated())
					FoundStellated = TRUE;
				else
					FoundNotStellated = TRUE;
			}
		}

		// Get the next selected thing
		pNode = pSelection->FindNext(pNode);
	}

	if (AllEllipses)
		UIDescription->Load(_R(IDS_REGSHAPE_ALLELLIPSES));
	else
	{
		if (FoundStellated || FoundNotStellated)
			OpSt.Greyed = FALSE;
		if (FoundStellated && !FoundNotStellated)
			UIDescription->Load(_R(IDS_OPSHAPETOGGLE_STELL_REM));
		if (!FoundStellated && FoundNotStellated)
			UIDescription->Load(_R(IDS_OPSHAPETOGGLE_STELL_ADD));
	}

	return(OpSt);	
}                                    



/********************************************************************************************

>	BOOL OpShapeToggleStellation::Init()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/4/95
	Inputs:		-
	Outputs:	-
	Returns:	TRUE/FALSE for successful init
	Purpose:	Initialise the toggle stellation operation by registering it
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL OpShapeToggleStellation::Init()
{
	return(RegisterOpDescriptor(0, 
								_R(IDS_OPSHAPETOGGLE_STELL),
								CC_RUNTIME_CLASS(OpShapeToggleStellation), 
								OPTOKEN_TOGGLESTELLATION,
								OpShapeToggleStellation::GetState)); 
}



/********************************************************************************************

>	BOOL OpShapeToggleStellation::DoAction(NodeRegularShape* pShape)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/4/95
	Inputs:		pShape - points to the shape to toggle
	Outputs:	-
	Returns:	TRUE/FALSE for successful toggle
	Purpose:	Toggles the stellation state of a shape
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
BOOL OpShapeToggleStellation::DoAction(NodeRegularShape* pShape)
{
	BOOL Success = TRUE;

	if (Success)
		Success = (ChangeShapeDataAction::DoToggle(this, &UndoActions, pShape,
				ChangeShapeDataAction::CHANGE_STELLATED, pShape->IsStellated() ? 1.0 : 0.0) != AC_FAIL);
	if (Success)
		pShape->SetStellated(!pShape->IsStellated());

	return Success;
}




///////////////////////////////////////////////////////////////////////////////////////////
// OpShapeToggleCurvature



/********************************************************************************************

>	OpShapeToggleCurvature::OpShapeToggleCurvature(): OpShapeToggleBase()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/04/95
	Purpose:	Constructs an OpShapeToggleCurvature object.

********************************************************************************************/

OpShapeToggleCurvature::OpShapeToggleCurvature(): OpShapeToggleBase()
{
}



/********************************************************************************************

>	OpState OpShapeToggleCurvature::GetState(String_256* UIDescription, OpDescriptor*)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/04/95
	Inputs:		UIDescription - the name of the opertaion to display
				OpDescriptor - To help update the state
	Outputs:	UIDescription - The name of the operation
	Returns:	The state of the operation
	Purpose:	Find the state of the OpShapeToggleCurvature operation.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

OpState OpShapeToggleCurvature::GetState(String_256* UIDescription, OpDescriptor*)
{
	OpState OpSt;  
	OpSt.Greyed = TRUE; 
	BOOL FoundCurved = FALSE;  
	BOOL FoundNotCurved = FALSE;
	BOOL AllEllipses = TRUE;
	ObjChangeFlags cFlags;
	ObjChangeParam ObjChange(OBJCHANGE_STARTING,cFlags,NULL,NULL);

	SelRange* pSelection = GetApplication()->FindSelection();
	Node* pNode = pSelection->FindFirst();

	while (pNode != NULL)
	{
		if (IS_A(pNode, NodeRegularShape))
		{
			NodeRegularShape* pShape = (NodeRegularShape*)pNode;

			AllEllipses = AllEllipses && pShape->IsCircular();
		
			// Convert this shape if it's parents allow
			if (pShape->AllowOp(&ObjChange))				
			{
				if (pShape->IsPrimaryCurvature() && pShape->IsStellationCurvature())
					FoundCurved = TRUE;
				else
					FoundNotCurved = TRUE;
			}
		}

		// Get the next selected thing
		pNode = pSelection->FindNext(pNode);
	}

	if (AllEllipses)
		UIDescription->Load(_R(IDS_REGSHAPE_ALLELLIPSES));
	else
	{
		if (FoundCurved || FoundNotCurved)
			OpSt.Greyed = FALSE;
		if (FoundCurved && !FoundNotCurved)
			UIDescription->Load(_R(IDS_OPSHAPETOGGLE_CURVE_REM));
		if (!FoundCurved && FoundNotCurved)
			UIDescription->Load(_R(IDS_OPSHAPETOGGLE_CURVE_ADD));
	}

	return(OpSt);	
}                                    



/********************************************************************************************

>	BOOL OpShapeToggleCurvature::Init()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/4/95
	Inputs:		-
	Outputs:	-
	Returns:	TRUE/FALSE for successful init
	Purpose:	Initialise the toggle curvature operation by registering it
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL OpShapeToggleCurvature::Init()
{
	return(RegisterOpDescriptor(0, 
								_R(IDS_OPSHAPETOGGLE_CURVE),
								CC_RUNTIME_CLASS(OpShapeToggleCurvature), 
								OPTOKEN_TOGGLECURVATURE,
								OpShapeToggleCurvature::GetState)); 
}



/********************************************************************************************

>	BOOL OpShapeToggleCurvature::DoAction(NodeRegularShape* pShape)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/4/95
	Inputs:		pShape - points to the shape to toggle
	Outputs:	-
	Returns:	TRUE/FALSE for successful toggle
	Purpose:	Toggles the curvature state of a shape
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
BOOL OpShapeToggleCurvature::DoAction(NodeRegularShape* pShape)
{
	BOOL Success = TRUE;

	// Do the primary curvature
	if (Success)
		Success = (ChangeShapeDataAction::DoToggle(this, &UndoActions, pShape,
				ChangeShapeDataAction::CHANGE_PRIMARYCURVATURE, pShape->IsPrimaryCurvature() ? 1.0 : 0.0) != AC_FAIL);
	if (Success)
		pShape->SetPrimaryCurvature(!pShape->IsPrimaryCurvature());

	// Do the stellation curvature
	if (Success)
		Success = (ChangeShapeDataAction::DoToggle(this, &UndoActions, pShape,
				ChangeShapeDataAction::CHANGE_STELLATIONCURVATURE, pShape->IsStellationCurvature() ? 1.0 : 0.0) != AC_FAIL);
	if (Success)
		pShape->SetStellationCurvature(!pShape->IsStellationCurvature());

	return Success;
}



///////////////////////////////////////////////////////////////////////////////////////////
// OpShapeToggleSetNumSides



/********************************************************************************************

>	OpShapeToggleSetNumSides::OpShapeToggleSetNumSides(): SelOperation()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/04/95
	Purpose:	Constructs an OpShapeToggleSetNumSides object.

********************************************************************************************/

OpShapeToggleSetNumSides::OpShapeToggleSetNumSides(): SelOperation()
{
}



/********************************************************************************************

>	OpState OpShapeToggleSetNumSides::GetState(String_256* UIDescription, OpDescriptor*)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/04/95
	Inputs:		UIDescription - the name of the opertaion to display
				OpDescriptor - To help update the state
	Outputs:	UIDescription - The name of the operation
	Returns:	The state of the operation
	Purpose:	Find the state of the OpShapeToggleCurvature operation.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

OpState OpShapeToggleSetNumSides::GetState(String_256* UIDescription, OpDescriptor*)
{
	OpState OpSt; 

	BOOL FoundEditable = FALSE;
	ObjChangeParam ObjChange(OBJCHANGE_STARTING,ObjChangeFlags(),NULL,NULL);

	SelRange* pSelection = GetApplication()->FindSelection();
	Node* pNode = pSelection->FindFirst();

	while (pNode != NULL)
	{
		if (IS_A(pNode, NodeRegularShape))
		{
			NodeRegularShape* pShape = (NodeRegularShape*)pNode;
		
			// Convert this shape if it's parents allow
			if (pNode->AllowOp(&ObjChange) && !pShape->IsCircular())
				FoundEditable = TRUE;
		}

		// Get the next selected thing
		pNode = pSelection->FindNext(pNode);
	}  

	if (!FoundEditable)
		OpSt.Greyed = TRUE;

	return OpSt;	
}                                    



/********************************************************************************************

>	BOOL OpShapeToggleSetNumSides::Init()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/4/95
	Inputs:		-
	Outputs:	-
	Returns:	TRUE/FALSE for successful init
	Purpose:	Initialise the set number of sides operation by registering it
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL OpShapeToggleSetNumSides::Init()
{
	BOOL ok = TRUE;
	
	ok = ok && RegisterOpDescriptor(0, 
									_R(IDS_QUICKSHAPES_NUMBERSIDES),
									CC_RUNTIME_CLASS(OpShapeToggleSetNumSides), 
									OPTOKEN_QUICKSHAPE_NUMBERSIDES,
									OpShapeToggleSetNumSides::GetState); 
	ok = ok && RegisterOpDescriptor(0, 
									_R(IDS_QUICKSHAPES_NUMBERSIDES3),
									CC_RUNTIME_CLASS(OpShapeToggleSetNumSides3), 
									OPTOKEN_QUICKSHAPE_NUMBERSIDES3,
									OpShapeToggleSetNumSides::GetState); 
	ok = ok && RegisterOpDescriptor(0, 
									_R(IDS_QUICKSHAPES_NUMBERSIDES4),
									CC_RUNTIME_CLASS(OpShapeToggleSetNumSides4), 
									OPTOKEN_QUICKSHAPE_NUMBERSIDES4,
									OpShapeToggleSetNumSides::GetState); 
	ok = ok && RegisterOpDescriptor(0, 
									_R(IDS_QUICKSHAPES_NUMBERSIDES5),
									CC_RUNTIME_CLASS(OpShapeToggleSetNumSides5), 
									OPTOKEN_QUICKSHAPE_NUMBERSIDES5,
									OpShapeToggleSetNumSides::GetState); 
	ok = ok && RegisterOpDescriptor(0, 
									_R(IDS_QUICKSHAPES_NUMBERSIDES6),
									CC_RUNTIME_CLASS(OpShapeToggleSetNumSides6), 
									OPTOKEN_QUICKSHAPE_NUMBERSIDES6,
									OpShapeToggleSetNumSides::GetState); 
	ok = ok && RegisterOpDescriptor(0, 
									_R(IDS_QUICKSHAPES_NUMBERSIDES7),
									CC_RUNTIME_CLASS(OpShapeToggleSetNumSides7), 
									OPTOKEN_QUICKSHAPE_NUMBERSIDES7,
									OpShapeToggleSetNumSides::GetState); 
	ok = ok && RegisterOpDescriptor(0, 
									_R(IDS_QUICKSHAPES_NUMBERSIDES8),
									CC_RUNTIME_CLASS(OpShapeToggleSetNumSides8), 
									OPTOKEN_QUICKSHAPE_NUMBERSIDES8,
									OpShapeToggleSetNumSides::GetState); 
	ok = ok && RegisterOpDescriptor(0, 
									_R(IDS_QUICKSHAPES_NUMBERSIDES9),
									CC_RUNTIME_CLASS(OpShapeToggleSetNumSides9), 
									OPTOKEN_QUICKSHAPE_NUMBERSIDES9,
									OpShapeToggleSetNumSides::GetState); 
	ok = ok && RegisterOpDescriptor(0, 
									_R(IDS_QUICKSHAPES_NUMBERSIDES10),
									CC_RUNTIME_CLASS(OpShapeToggleSetNumSides10), 
									OPTOKEN_QUICKSHAPE_NUMBERSIDES10,
									OpShapeToggleSetNumSides::GetState); 

	return ok;
}



/********************************************************************************************

>	BOOL OpShapeToggleSetNumSides::DoAction(INT32 NumSides)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/4/95
	Inputs:		NumSides - the number of sides to set the shape to
	Outputs:	-
	Returns:	TRUE/FALSE for successful toggle
	Purpose:	Sets the number of sides of a shape
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
BOOL OpShapeToggleSetNumSides::DoAction(INT32 NumSides)
{
	BOOL ok = DoStartSelOp(FALSE,FALSE);

	ObjChangeParam ObjChange(OBJCHANGE_STARTING,ObjChangeFlags(), NULL, this);
	SelRange* Selection = GetApplication()->FindSelection();
	Node* pNode = Selection->FindFirst();

	while (ok && (pNode != NULL)) 
	{
	 	if (IS_A(pNode,NodeRegularShape))
		{
			NodeRegularShape* pShape = (NodeRegularShape*)pNode;
		
			if (pShape->AllowOp(&ObjChange) && !pShape->IsCircular())
			{
				// Record the bounds
				if (ok)
					ok = (RecalcBoundsAction::DoRecalc(this, &UndoActions, pShape) != AC_FAIL);
				
				// Change the shape
				ok = (ChangeShapeDataAction::DoToggle(this, &UndoActions, pShape, 
							ChangeShapeDataAction::CHANGE_NUMSIDES, pShape->GetNumSides()) != AC_FAIL);
				if (ok)
					pShape->SetNumSides(NumSides);

				// Invalidate the shapes bounding rect as it's changed
				if (ok)
				{
					pShape->InvalidateBoundingRect();
					pShape->InvalidateCache();
				}

				// And redraw its final state
				if (ok)
					ok = (RecordBoundsAction::DoRecord(this, &UndoActions, pShape) != AC_FAIL);

				// Say we have finished
				if (ok)
				{
					ObjChange.Define(OBJCHANGE_FINISHED,ObjChangeFlags(),NULL,this);
					ok = UpdateChangedNodes(&ObjChange);
				}
			}
		}

		pNode = Selection->FindNext(pNode);
	}

	if (!ok)
	{
		InformError();
		FailAndExecute();
	}

	End();

	return ok;
}



void OpShapeToggleSetNumSides3::Do(OpDescriptor* pOpDesc)
{
	if (!DoAction(3))
		InformError();
}
void OpShapeToggleSetNumSides4::Do(OpDescriptor* pOpDesc)
{
	if (!DoAction(4))
		InformError();
}
void OpShapeToggleSetNumSides5::Do(OpDescriptor* pOpDesc)
{
	if (!DoAction(5))
		InformError();
}
void OpShapeToggleSetNumSides6::Do(OpDescriptor* pOpDesc)
{
	if (!DoAction(6))
		InformError();
}
void OpShapeToggleSetNumSides7::Do(OpDescriptor* pOpDesc)
{
	if (!DoAction(7))
		InformError();
}
void OpShapeToggleSetNumSides8::Do(OpDescriptor* pOpDesc)
{
	if (!DoAction(8))
		InformError();
}
void OpShapeToggleSetNumSides9::Do(OpDescriptor* pOpDesc)
{
	if (!DoAction(9))
		InformError();
}
void OpShapeToggleSetNumSides10::Do(OpDescriptor* pOpDesc)
{
	if (!DoAction(10))
		InformError();
}



/////////////////////////////////////////////////////////////////////////////////////////////
//	OpShapeMakeRegular 


/********************************************************************************************

>	static OpState OpShapeMakeRegular::GetState(String_256* UIDesc, OpDescriptor* OpDesc)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/4/95
	Inputs:		UIDesc - 
				OpDesc -
	Outputs:	-
	Returns:	The tick/grey state of this op
	Purpose:	Returns the state of this operation
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
OpState OpShapeMakeRegular::GetState(String_256* UIDesc, OpDescriptor* OpDesc)
{
	OpState Blobby;
	
	return Blobby;
}



/********************************************************************************************

>	static BOOL	OpShapeMakeRegular::Init()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/4/95
	Inputs:		-
	Outputs:	-
	Returns:	TRUE/FALSE for successful initialise
	Purpose:	Registers this op
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
BOOL OpShapeMakeRegular::Init()
{
	return RegisterOpDescriptor(0, 
								_R(IDS_QUICKSHAPE_MAKEREGULAR),
								CC_RUNTIME_CLASS(OpShapeMakeRegular), 
								OPTOKEN_QUICKSHAPE_MAKEREGULAR,
								OpShapeMakeRegular::GetState); 
}					



/********************************************************************************************

>	void OpShapeMakeRegular::DoMakeRegular(NodeRegularShape* pShape, DocCoord RegularPoint)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/4/95
	Inputs:		pShape - points to the shape to make regular
				RegularPoint - defines the new size of the shape, from the centre to this point
	Outputs:	-
	Returns:	-
	Purpose:	Makes a QuickShape regular, ie 100% aspect ratio, no skew, shear etc...
	Errors:		Assorted pointer checks
	SeeAlso:	-

********************************************************************************************/
void OpShapeMakeRegular::DoMakeRegular(NodeRegularShape* pShape, DocCoord RegularPoint)
{
	if (!DoActions(pShape, RegularPoint))
	{
		InformError();
		FailAndExecute();
	}

	End();
}



/********************************************************************************************

>	BOOL OpShapeMakeRegular::DoActions(NodeRegularShape* pShape, DocCoord RegularPoint)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/4/95
	Inputs:		pShape - points to the shape to make regular
				RegularPoint - defines the new size of the shape, from the centre to this point
	Outputs:	-
	Returns:	-
	Purpose:	Makes a QuickShape regular, ie 100% aspect ratio, no skew, shear etc...
	Errors:		Assorted pointer checks
	SeeAlso:	-

********************************************************************************************/
BOOL OpShapeMakeRegular::DoActions(NodeRegularShape* pShape, DocCoord RegularPoint)
{
	ERROR2IF(pShape == NULL, FALSE, "Shape pointer was NULL");
	ERROR3IF(!pShape->IsCircular(), "Make regular not implemented for non-circular QuickShapes");

	BOOL ok = DoStartSelOp(FALSE, FALSE);

	// Get required variables
	DocCoord ShapeCentre = pShape->GetCentrePoint();
	DocCoord UTShapeCentre = pShape->GetUTCentrePoint();
	const double RequiredLength = ShapeCentre.Distance(RegularPoint);
	const double MajorLength = pShape->GetUTMajorAxes().Distance(UTShapeCentre);
	const double MinorLength = pShape->GetUTMinorAxes().Distance(UTShapeCentre);
	const double RequiredAngle = atan2((double)(RegularPoint.y-ShapeCentre.y),(double)(RegularPoint.x-ShapeCentre.x));
	const double MajorAxisAngle = atan2((double)(pShape->GetUTMajorAxes().y-UTShapeCentre.y),
												(double)(pShape->GetUTMajorAxes().x-UTShapeCentre.x));
	Matrix ShapeMat;
	pShape->GetTransformMatrix(&ShapeMat);
	Trans2DMatrix* Translation = new Trans2DMatrix(ShapeMat);
	Translation->Invert();
	
	// Translate shape to origin
	if (ok)
		*Translation *= Trans2DMatrix(-UTShapeCentre.x, -UTShapeCentre.y);

	// Rotate so major axis points along +ve x-axis (not y!)
	if (ok)
		*Translation *= Trans2DMatrix(Matrix(-ANGLE(MajorAxisAngle*(180/PI))));

	// Scale it so it is regular
	if (ok)
		*Translation *= Trans2DMatrix(Matrix(FIXED16(RequiredLength/MajorLength), FIXED16(RequiredLength/MinorLength)));
	
	// Rotate so major axis is under click point
	if (ok)
		*Translation *= Trans2DMatrix(Matrix(ANGLE(RequiredAngle*(180/PI))));

	// Translate shape to old centre point
	if (ok)
		*Translation *= Trans2DMatrix(ShapeCentre.x, ShapeCentre.y);

	ObjChangeParam ObjChange(OBJCHANGE_STARTING,ObjChangeFlags(), NULL, this);
	if (pShape->AllowOp(&ObjChange))
	{
		// Record the bounds
		if (ok)
			ok = (RecalcBoundsAction::DoRecalc(this, &UndoActions, pShape) != AC_FAIL);

		// Now actually transform the shape in the document
		if (ok)
			ok = DoTransformNode(pShape, Translation);

		// And redraw its final state
		if (ok)
			ok = (RecordBoundsAction::DoRecord(this, &UndoActions, pShape) != AC_FAIL);

		GetApplication()->FindSelection()->Update();

		// Say we have finished
		if (ok)									  
		{
			ObjChange.Define(OBJCHANGE_FINISHED,ObjChangeFlags(),NULL,this);
			ok = UpdateChangedNodes(&ObjChange);
		}
	}

	return ok;
}
