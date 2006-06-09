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

// brush operation implementation

#include "camtypes.h"
#include "brushop.h"
//#include "rik.h"
//#include "markn.h"
#include "brushmsg.h"
#include "brushdlg.h"
#include "freehand.h"
//#include "tool.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "sgline.h"
#include "freeinfo.h"
//#include "tool.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "toollist.h"
//#include "bitmapcache.h" - in camtypes.h [AUTOMATICALLY REMOVED]

CC_IMPLEMENT_DYNCREATE( OpChangeBrushDefinition, UndoableOperation );
CC_IMPLEMENT_DYNCREATE(ChangeBrushDefAction, Action);
CC_IMPLEMENT_DYNAMIC(ChangeBrushDefOpParam, ChangeBrushOpParam);
CC_IMPLEMENT_DYNCREATE(OpSelectBrush, UndoableOperation);
CC_IMPLEMENT_DYNCREATE(OpDeactivateBrush, UndoableOperation);
CC_IMPLEMENT_DYNCREATE(DeactivateBrushDefAction, Action);

//////////////////////////////////////////////////////////////////////////////////////////////
//  OpChangeBrushDefinition  - to change the features of the brush definition
/////////////////////////////////////////////////////////////////////////////////////////////


/********************************************************************************************

>	OpChangeBrushDefinition::OpChangeBrushDefinition()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/99
	Returns:	-
	Purpose:	Constructor

********************************************************************************************/

OpChangeBrushDefinition::OpChangeBrushDefinition()
{
	 ChangeType = CHANGEBRUSH_NONE; 
}


/********************************************************************************************

>	OpChangeBrushDefinition::~OpChangeBrushDefinition()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/99
	Returns:	-
	Purpose:	destructor
********************************************************************************************/

OpChangeBrushDefinition::~OpChangeBrushDefinition()
{
	
}

/********************************************************************************************

>	BOOL OpChangeBrushDefinition::Declare()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/99
	Returns:	TRUE if all went OK, FALSE otherwise
	Purpose:	Adds the operation to the list of all known operations

********************************************************************************************/

BOOL OpChangeBrushDefinition::Declare()
{
	return (RegisterOpDescriptor(
								0, 
								0,
								CC_RUNTIME_CLASS(OpChangeBrushDefinition), 
								OPTOKEN_CHANGEBRUSHDEF,
								OpChangeBrushDefinition::GetState,
								0,  /* help ID */
								0,	/* bubble ID */
								0	/* bitmap ID */
								));
}



/********************************************************************************************

>	static OpState OpChangeBrushDefinition::GetState(String_256* Description, OpDescriptor*)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/99
	Outputs:	Description - GetState fills this string with an approriate description
				of the current state of the operation 
	Returns:	The state of the operation, so that menu items (ticks and greying) can be
				done properly
	Purpose:	Find out the state of the operation at the specific time

********************************************************************************************/

OpState OpChangeBrushDefinition::GetState(String_256* Description, OpDescriptor*)
{
	OpState Bob;
	

	return Bob;

}

/********************************************************************************************

>	virtual void OpChangeBrushDefinition::GetOpName(String_256* OpName) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/99
	Inputs:		-
	Outputs:	The undo string for the operation
	Returns:	
	Purpose:	The GetOpName fn is overridden so that we return back a description 
				appropriate to the type of attribute that the operation applies.
				Although this looks identical to the OpChangeBrush fn. it is worth
				keeping this separate because the ChangeAll string needs to be used
				for saving the brush definition
	Errors:		-
	SeeAlso:	-

********************************************************************************************/


void OpChangeBrushDefinition::GetOpName(String_256* OpName) 
{ 
	UINT32 IDS = _R(IDS_MARKN_EMPTY);

	switch (ChangeType)
	{
		case CHANGEBRUSH_NONE:			  	break;
		case CHANGEBRUSH_SPACING:			IDS = _R(IDS_BRUSHSPACING_UNDO); 	break;
		case CHANGEBRUSH_SPACING_INCRPROP:  IDS = _R(IDS_BRUSHSPACING_INCR);	break;
		case CHANGEBRUSH_SPACING_INCRCONST:	IDS = _R(IDS_BRUSHSPACING_INCR);	break;
		case CHANGEBRUSH_SPACING_MAXRAND:	IDS = _R(IDS_BRUSHSPACING_MAXRAND); break;
		case CHANGEBRUSH_SPACING_RANDSEED:	IDS = _R(IDS_BRUSHSPACING_RANDSEED); break;
		case CHANGEBRUSH_SCALING:			IDS = _R(IDS_BRUSHSCALING_UNDO);	break;
		case CHANGEBRUSH_SCALING_INCR:		IDS = _R(IDS_BRUSHSCALING_INCR);	break;
		case CHANGEBRUSH_SCALING_INCRCONST:	IDS = _R(IDS_BRUSHSCALING_INCR);	break;
		case CHANGEBRUSH_SCALING_MAXRAND:	IDS = _R(IDS_BRUSHSCALING_MAXRAND); break;
		case CHANGEBRUSH_SCALING_RANDSEED:	IDS = _R(IDS_BRUSHSCALING_RANDSEED); break;
		case CHANGEBRUSH_OFFSET_TYPE:		IDS = _R(IDS_BRUSHOFFSET_TYPE_UNDO);break;
		case CHANGEBRUSH_OFFSET_TYPE_SEED:	IDS = _R(IDS_OFFSETTYPE_RANDSEED);	break;
		case CHANGEBRUSH_OFFSET_VAL:		IDS = _R(IDS_BRUSHOFFSET_VAL_UNDO);	break;
		case CHANGEBRUSH_OFFSET_VALSEED:	IDS = _R(IDS_OFFSETVAL_RANDSEED);	break;
		case CHANGEBRUSH_OFFSET_INCRPROP:	IDS = _R(IDS_BRUSHOFFSET_INCR);		break;
		case CHANGEBRUSH_OFFSET_INCRCONST:	IDS = _R(IDS_BRUSHOFFSET_INCR);		break;
		case CHANGEBRUSH_OFFSET_MAXRAND:	IDS = _R(IDS_OFFSETVAL_MAXRAND);	break;
		case CHANGEBRUSH_TILED:				IDS = _R(IDS_BRUSHTILE_UNDO);		break;
		case CHANGEBRUSH_TANGENTIAL:		IDS = _R(IDS_BRUSHROTATE_UNDO);		break;
		case CHANGEBRUSH_ROTATE_ANGLE:		IDS = _R(IDS_BRUSHROTATE_ANGLE_UNDO); break;
		case CHANGEBRUSH_ROTATE_PRESSURE:	IDS = _R(IDS_BRUSHROTATE_PRESSURE);	break;
		case CHANGEBRUSH_NAME:				IDS = _R(IDS_BRUSHNAME_UNDO);		break;
		case CHANGEBRUSH_SEQUENCE:			IDS = _R(IDS_BRUSHNAME_UNDO);		break;
		case CHANGEBRUSH_SEQUENCE_RANDSEED:	IDS = _R(IDS_SEQUENCE_RANDSEED);	break;
		case CHANGEBRUSH_USELOCALFILLCOL:	IDS = _R(IDS_BRUSH_USELOCAL_FILLCOL);		break;
		case CHANGEBRUSH_ALL:				IDS = _R(IDS_BRUSH_SAVEDEFINITION);		break;
		case CHANGEBRUSH_ROTATE_MAXRAND:	IDS = _R(IDS_ROTATE_MAXRAND);		break;
		case CHANGEBRUSH_ROTATE_RANDSEED:	IDS = _R(IDS_ROTATE_RANDSEED);		break;
		case CHANGEBRUSH_TIMESTAMP:			IDS = _R(IDS_TIMESTAMP_PERIOD);		break;
		case CHANGEBRUSH_REGEN:				break;
		
		default: ERROR3_PF(("Unknown flag type (%d)",ChangeType));  break;
	}

	*OpName = String_256(IDS);
}  


/********************************************************************************************

>	void OpChangeBrushDefinition::DoWithParam(OpDescriptor*,OpParam* pOpParam)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/99
	Returns:	-
	Purpose:	This changes the flag specified in FlagType (given in pOpParam->Param1) in all the 
				selected brush objects to have the same as state of pOpParam->Param2.

********************************************************************************************/

void OpChangeBrushDefinition::DoWithParam(OpDescriptor*,OpParam* pOpParam)
{
	ERROR3IF(pOpParam == NULL,"NULL OpParam ptr");
	if (pOpParam == NULL) 
	{
		FailAndExecute();
		return;
	}

	ChangeBrushDefOpParam* pChangeParam = (ChangeBrushDefOpParam*)pOpParam;
	
	// no point doing anything if we have a default brush
	if (pChangeParam->m_Handle == BrushHandle_NoBrush)
	{
		FailAndExecute();
		return;
	}

	ChangeBrushDefAction* pAction;

	// change the brush in an undoable way
	BOOL ok = ChangeBrushDefAction::Init(this,&UndoActions,
										pChangeParam,&pAction) != AC_FAIL;
	
	if (ok) 
	{
		// Note the way the selected brush were changed
		ChangeType = pChangeParam->ChangeType;
	}
	else
		FailAndExecute();

	End();
}

//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
// The ChangeBrushDefAction class

/********************************************************************************************

>	ChangeBrushDefAction::ChangeBrushDefAction()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/99
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Constructor for the action
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

ChangeBrushDefAction::ChangeBrushDefAction()
{
}


/********************************************************************************************

>	ChangeBrushDefAction::~ChangeBrushDefAction()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/99
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	destructor for the action
	Errors:		-
	SeeAlso:	-

********************************************************************************************/


ChangeBrushDefAction::~ChangeBrushDefAction()
{
}

/********************************************************************************************

>	ActionCode ChangeBrushDefAction::Init( 	Operation* 		pOp,
											ActionList* 	pActionList,
											NodeRenderableInk * pNodeBrush,
											ChangeBrushOpParam	*pChangeParam,
											ChangeBrushDefAction** ppNewAction);

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/99
	Inputs:		pOp  			= ptr to the operation to which this action belongs
				pActionList 	=  ptr to action list to which this action should be added
				pBrushedNode 	= ptr to node with brush attribute to change 
				pChangeParam	= ptr to class that details how the blend should be changed.
	Outputs:	ppNewAction  	= ptr to a ptr to an action, allowing the function to return
								  a pointer to the created action
	Returns:	ActionCode, one of AC_OK, AC_NO_RECORD or AC_FAIL
	Purpose:	This is the function which creates an instance of this action. If there is no room 
				in the undo buffer (which is determined by the base class Init function called within)
				the function will either return AC_NO_RECORD which means the operation can continue, 
				but no undo information needs to be stored, or AC_OK which means the operation should
				continue AND record undo information. If the function returns AC_FAIL, there was not 
				enough memory to record the undo information, and the user has decided not to continue
				with the operation.

				This function actually changes the blend node in a way specified in pChangeParam
	Errors:		-
	SeeAlso:	Action::Init()

********************************************************************************************/



ActionCode ChangeBrushDefAction::Init(	Operation* 			pOp,
									ActionList* 		pActionList,
									ChangeBrushDefOpParam*	pChangeParam,
									ChangeBrushDefAction** ppNewAction)
{
	ERROR2IF(pChangeParam == NULL,AC_FAIL,"pChangeParam is NULL");

	// just check that we do actually have a brush definition
	
	// get the brush component from the document
	Document* pDoc = Document::GetCurrent();
	ERROR2IF(pDoc == NULL, AC_FAIL, "Wheres the document?");
	BrushComponent* pBrushComp = (BrushComponent*)pDoc->GetDocComponent(CC_RUNTIME_CLASS(BrushComponent));
	ERROR2IF(pBrushComp == NULL, AC_FAIL, "No brush component");

	BrushDefinition* pBrushDef = pBrushComp->FindDefinition(pChangeParam->m_Handle);
	if (pBrushDef == NULL)
		return AC_FAIL;

	// make the new action
	UINT32 ActSize = sizeof(ChangeBrushDefAction);
	ChangeBrushDefAction* pNewAction;
	ActionCode Ac = Action::Init(pOp,pActionList,ActSize,CC_RUNTIME_CLASS(ChangeBrushDefAction),(Action**)&pNewAction);
	*ppNewAction = pNewAction;

	if (Ac != AC_FAIL && pNewAction != NULL)
	{
	
			ChangeBrushType ChangeType = pChangeParam->ChangeType;
			// set data in the new action
			pNewAction->m_ChangeParam.ChangeType = ChangeType;
			pNewAction->m_ChangeParam.SetOldValues(pBrushDef);
			pNewAction->m_ChangeParam.m_Handle = pChangeParam->m_Handle;
			
			switch (ChangeType)
			{
				case CHANGEBRUSH_SPACING:			
					if (ChangeBrushSpacing(pChangeParam->m_NewSpacing, pBrushDef) == FALSE)
						Ac = AC_FAIL;
					break;
				case CHANGEBRUSH_SPACING_INCRPROP:
					if (!ChangeSpacingIncrProp(pChangeParam->m_NewSpacingIncrProp,  pBrushDef))
						Ac = AC_FAIL;
					break;
				case CHANGEBRUSH_SPACING_INCRCONST:
					if (!ChangeSpacingIncrConst(pChangeParam->m_NewSpacingIncrConst,  pBrushDef))
						Ac = AC_FAIL;
					break;
				case CHANGEBRUSH_SCALING_INCR:
					if (!ChangeScalingIncr(pChangeParam->m_NewScalingIncr, pBrushDef))
						Ac = AC_FAIL;
					break;
				case CHANGEBRUSH_SCALING_INCRCONST:
					if (!ChangeScalingIncrConst(pChangeParam->m_NewScalingIncrConst, pBrushDef))
						Ac = AC_FAIL;
					break;
				case CHANGEBRUSH_SCALING:
					if (ChangeBrushScaling(pChangeParam->m_NewScaling, pBrushDef) == FALSE)
						Ac = AC_FAIL;
					break;
				case CHANGEBRUSH_SCALING_MAXRAND:
					if (!ChangeScalingMaxRand(pChangeParam->m_NewScalingMaxRand, pBrushDef))
						Ac = AC_FAIL;
					break;
				case CHANGEBRUSH_SCALING_RANDSEED:
					if (!ChangeScalingRandSeed(pChangeParam->m_NewScalingRandSeed, pBrushDef))
						Ac = AC_FAIL;
					break;
				case CHANGEBRUSH_SCALING_PRESSURE:
					if (!ChangeScalingPressure(pChangeParam->m_NewScalingMaxPressure, pBrushDef))
						Ac = AC_FAIL;
					break;
				case CHANGEBRUSH_TILED:
					ChangeBrushTile(pChangeParam->m_bNewTiled, pBrushDef);
					break;
				case CHANGEBRUSH_TANGENTIAL:
					ChangeTangential(pChangeParam->m_bNewRotated, pBrushDef);
					break;
				case CHANGEBRUSH_OFFSET_TYPE:
					if (!ChangeOffsetType(pChangeParam->m_NewPathOffsetType, pBrushDef))
						Ac = AC_FAIL;
					break;
				case CHANGEBRUSH_OFFSET_TYPE_SEED:
					if (!ChangeOffsetTypeSeed(pChangeParam->m_NewOffsetTypeRandSeed, pBrushDef))
						Ac = AC_FAIL;
					break;
				case CHANGEBRUSH_OFFSET_VAL:
					if (!ChangeOffsetValue(pChangeParam->m_NewPathOffsetVal, pBrushDef))
						Ac = AC_FAIL;
					break;
				case CHANGEBRUSH_OFFSET_INCRPROP:
					if (!ChangeOffsetIncrProp(pChangeParam->m_NewOffsetIncrProp, pBrushDef))
						Ac = AC_FAIL;
					break;
				case CHANGEBRUSH_OFFSET_INCRCONST:
					if (!ChangeOffsetIncrConst(pChangeParam->m_NewOffsetIncrConst, pBrushDef))
						Ac = AC_FAIL;
					break;
				case CHANGEBRUSH_OFFSET_MAXRAND:
					if (!ChangeOffsetValMaxRand(pChangeParam->m_NewOffsetValMaxRand, pBrushDef))
						Ac = AC_FAIL;
					break;
				case CHANGEBRUSH_OFFSET_VALSEED:
					if (!ChangeOffsetValRandSeed(pChangeParam->m_NewOffsetValRandSeed, pBrushDef))
						Ac = AC_FAIL;
					break;
				case CHANGEBRUSH_ROTATE_ANGLE:
					if (!ChangeRotation(pChangeParam->m_NewRotateAngle, pBrushDef))
						Ac = AC_FAIL;
					break;
				case CHANGEBRUSH_ROTATE_MAXRAND:
					if (!ChangeRotationMaxRand(pChangeParam->m_NewRotateMaxRand, pBrushDef))
						Ac = AC_FAIL;
					break;
				case CHANGEBRUSH_ROTATE_RANDSEED:
					if (!ChangeRotationRandSeed(pChangeParam->m_NewRotateRandSeed, pBrushDef))
						Ac = AC_FAIL;
					break;
				case CHANGEBRUSH_ROTATE_PRESSURE:
					if (!ChangeRotationMaxPressure(pChangeParam->m_NewRotationMaxPressure, pBrushDef))
						Ac = AC_FAIL;
					break;
				case CHANGEBRUSH_SPACING_MAXRAND:
					if (!ChangeSpacingMaxRand(pChangeParam->m_NewSpacingMaxRand, pBrushDef))
						Ac = AC_FAIL;
					break;
				case CHANGEBRUSH_SPACING_RANDSEED:
					if (!ChangeSpacingRandSeed(pChangeParam->m_NewSpacingRandSeed, pBrushDef))
						Ac = AC_FAIL;
					break;
				case CHANGEBRUSH_USELOCALFILLCOL:
					if (!ChangeUseLocalFillColour(pChangeParam->m_bNewUseLocalFillColour, pBrushDef))
						Ac = AC_FAIL;
					break;
				case CHANGEBRUSH_SEQUENCE:
					if (!ChangeSequenceType(pChangeParam->m_NewSequenceType, pBrushDef))
						Ac = AC_FAIL;
					break;
				case CHANGEBRUSH_SEQUENCE_RANDSEED:
					if (!ChangeSequenceSeed(pChangeParam->m_NewSequenceRandSeed, pBrushDef))
						Ac = AC_FAIL;
					break;
				case CHANGEBRUSH_TIMESTAMP:
					if (!ChangeTimeStamp(pChangeParam->m_NewTimeStampValue, pBrushDef))
						Ac = AC_FAIL;
					break;
				case CHANGEBRUSH_NAME:
					if (!ChangeBrushName(pChangeParam->m_NewBrushName, pBrushDef))
						Ac = AC_FAIL;
					break;
				case CHANGEBRUSH_ALL:
					if (!ChangeAll(pChangeParam, pBrushDef))
						Ac = AC_FAIL;
					break;
			default : ERROR2(AC_FAIL,"Unknown change brush type"); break;
			
			} // end switch
	
		// set the new values for the undo
		pNewAction->m_ChangeParam.SetNewValues(pBrushDef);

		// send a message to whoever is interested
		BROADCAST_TO_ALL(BrushDefMsg(pChangeParam->m_Handle));
	} // end if (AC != ..


	LineGallery* pGallery = LineGallery::GetInstance();
	if (pGallery != NULL)
		pGallery->UpdateBrushItem(pChangeParam->m_Handle);

	return Ac;
}



/********************************************************************************************

>	ActionCode ChangeBrushDefAction::Execute();

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/99
	Inputs:		-
	Outputs:	-
	Returns:	ActionCode, one of AC_OK, AC_NO_RECORD or AC_FAIL
	Purpose:	Executes the action.  This will reset the brush variables to their old values
	Errors:		-
	SeeAlso:	Action::Init()

********************************************************************************************/

ActionCode ChangeBrushDefAction::Execute()
{
	m_ChangeParam.SwapOldAndNew();

	ActionCode Act;
	ChangeBrushDefAction* pAction;
	Act = ChangeBrushDefAction::Init(pOperation,pOppositeActLst,&m_ChangeParam,&pAction);

	return Act;
}



/********************************************************************************************

>	BOOL ChangeBrushDefAction::ChangeBrushName(String_32 Name, BrushDefinition* pBrushDef)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/99
	Inputs:		Name - the new name to set	
	Outputs:	pBrushDef - the brush attribute to change
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	Sets the name member for this brush
	SeeAlso:	-

********************************************************************************************/

BOOL ChangeBrushDefAction::ChangeBrushName(String_32 Name, BrushDefinition* pBrushDef)
{
	ERROR2IF(pBrushDef == NULL, FALSE, "brush attribute is NULL");
	
	pBrushDef->SetLineName(&Name);

	return TRUE;
}


/********************************************************************************************

>	BOOL ChangeBrushDefAction::ChangeBrushSpacing(MILLIPOINT NewSpacing, BrushDefinition* pBrushDef)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/99
	Inputs:		NewSpacing - the value to set		
	Outputs:	pBrushDef - the brush attribute to change
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	To set the brush spacing of this attribute node
	SeeAlso:	-

********************************************************************************************/

BOOL ChangeBrushDefAction::ChangeBrushSpacing(MILLIPOINT NewSpacing, BrushDefinition* pBrushDef)
{
	ERROR2IF(pBrushDef == NULL, FALSE, "brush attribute is NULL");
	
	return pBrushDef->SetSpacing(NewSpacing);
}


/********************************************************************************************

>	BOOL ChangeBrushDefAction::ChangeSpacingIncrProp(double NewIncrProp, BrushDefinition* pBrushDef)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/99
	Inputs:		NewSpacing - the value to set		
	Outputs:	pBrushDef - the brush attribute to change
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	To set the brush spacing of this attribute node
	SeeAlso:	-

********************************************************************************************/

BOOL ChangeBrushDefAction::ChangeSpacingIncrProp(double NewIncr,  BrushDefinition* pBrushDef)
{
	ERROR2IF(pBrushDef == NULL, FALSE, "brush attribute is NULL");
	
	return pBrushDef->SetSpacingIncrProp(NewIncr);
}


/********************************************************************************************

>	BOOL ChangeBrushDefAction::ChangeSpacingIncrConst(MILLIPOINT NewIncr, BrushDefinition* pBrushDef)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/99
	Inputs:		NewSpacing - the value to set		
	Outputs:	pBrushDef - the brush attribute to change
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	To set the brush spacing of this attribute node
	SeeAlso:	-

********************************************************************************************/

BOOL ChangeBrushDefAction::ChangeSpacingIncrConst(MILLIPOINT NewIncr,  BrushDefinition* pBrushDef)
{
	ERROR2IF(pBrushDef == NULL, FALSE, "brush attribute is NULL");
	
	return pBrushDef->SetSpacingIncrConst(NewIncr);
}


/********************************************************************************************

>	BOOL ChangeBrushDefAction::ChangeBrushScaling(double NewScaling, BrushDefinition* pBrushDef)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/99
	Inputs:		NewScaling - the value to set		
	Outputs:	pBrushDef - the brush attribute to change
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	To set the brush scaling of this attribute node
	SeeAlso:	-

********************************************************************************************/

BOOL ChangeBrushDefAction::ChangeBrushScaling(double NewScaling, BrushDefinition* pBrushDef)
{
	ERROR2IF(pBrushDef == NULL, FALSE, "brush attribute is NULL");
	
	return pBrushDef->SetBrushScaling(NewScaling);
}


/********************************************************************************************

>	BOOL ChangeBrushDefAction::ChangeScalingIncr(double NewScaling, BrushDefinition* pBrushDef)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/99
	Inputs:		NewScaling - the value to set		
	Outputs:	pBrushDef - the brush attribute to change
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	To set the brush scaling of this attribute node
	SeeAlso:	-

********************************************************************************************/

BOOL ChangeBrushDefAction::ChangeScalingIncr(double NewIncr, BrushDefinition* pBrushDef)
{
	ERROR2IF(pBrushDef == NULL, FALSE, "brush attribute is NULL");

	return pBrushDef->SetBrushScalingIncr(NewIncr);
}


/********************************************************************************************

>	BOOL ChangeBrushDefAction::ChangeScalingIncrConst(double Incr, BrushDefinition* pBrushDef)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/99
	Inputs:		Incr - the value to set		
	Outputs:	pBrushDef - the brush definition to change
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	To set the brush scaling of this attribute node
	SeeAlso:	-

********************************************************************************************/

BOOL ChangeBrushDefAction::ChangeScalingIncrConst(double Incr, BrushDefinition* pBrushDef)
{
	ERROR2IF(pBrushDef == NULL, FALSE, "brush definition is NULL");

	return pBrushDef->SetBrushScalingIncrConst(Incr);
}

/********************************************************************************************

>	BOOL ChangeBrushDefAction::ChangeScalingMaxRand(UINT32 NewMax, BrushDefinition* pBrushDef)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/99
	Inputs:		NewMax - the value to set		
	Outputs:	pBrushDef - the brush attribute to change
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	To set the amount of brush scaling determined by randomness
	SeeAlso:	-

********************************************************************************************/

BOOL ChangeBrushDefAction::ChangeScalingMaxRand(UINT32 NewMax, BrushDefinition* pBrushDef)
{
	ERROR2IF(pBrushDef == NULL, FALSE, "brush attribute is NULL");

	return pBrushDef->SetScalingMaxRand(NewMax);
}


/********************************************************************************************

>	BOOL ChangeBrushDefAction::ChangeScalingRandSeed(UINT32 NewSeed, BrushDefinition* pBrushDef)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/99
	Inputs:		NewSeed - the value to set		
	Outputs:	pBrushDef - the brush attribute to change
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	Sets a new seed for the scaling RNG
	SeeAlso:	-

********************************************************************************************/

BOOL ChangeBrushDefAction::ChangeScalingRandSeed(UINT32 NewSeed, BrushDefinition* pBrushDef)
{
	ERROR2IF(pBrushDef == NULL, FALSE, "brush attribute is NULL");

	pBrushDef->SetScalingRandSeed(NewSeed);
	return TRUE;
}


/********************************************************************************************

>	BOOL ChangeBrushDefAction::ChangeScalingRandSeed(UINT32 NewSeed, BrushDefinition* pBrushDef)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/99
	Inputs:		NewSeed - the value to set		
	Outputs:	pBrushDef - the brush attribute to change
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	Sets a new seed for the scaling RNG
	SeeAlso:	-

********************************************************************************************/

BOOL ChangeBrushDefAction::ChangeScalingPressure(UINT32 Pressure, BrushDefinition* pBrushDef)
{
	ERROR2IF(pBrushDef == NULL, FALSE, "brush attribute is NULL");

	return pBrushDef->SetScalingMaxPressure(Pressure);
}

/********************************************************************************************

>	BOOL ChangeBrushDefAction::Rotation(double NewAngle, BrushDefinition* pBrushDef)
	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/99
	Inputs:		NewAngle - the value to set		
	Outputs:	pBrushDef - the brush attribute to change
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	To set the brush scaling of this attribute node
	SeeAlso:	-

********************************************************************************************/

BOOL ChangeBrushDefAction::ChangeRotation(double NewAngle, BrushDefinition* pBrushDef)
{
	ERROR2IF(pBrushDef == NULL, FALSE, "brush attribute is NULL");

	return pBrushDef->SetRotationAngle(NewAngle);
}


/********************************************************************************************

>	BOOL ChangeBrushDefAction::ChangeRotationMaxRand(UINT32 NewMax, BrushDefinition* pBrushDef)
	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/99
	Inputs:		NewMax - the value to set		
	Outputs:	pBrushDef - the brush attribute to change
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	To set the maximum random effect on the rotation of this brush
	SeeAlso:	-

********************************************************************************************/

BOOL ChangeBrushDefAction::ChangeRotationMaxRand(UINT32 NewMax, BrushDefinition* pBrushDef)
{
	ERROR2IF(pBrushDef == NULL, FALSE, "brush attribute is NULL");

	return pBrushDef->SetRotationMaxRand(NewMax);
}


/********************************************************************************************

>	BOOL ChangeBrushDefAction::ChangeRotationRandSeed(UINT32 NewSeed, BrushDefinition* pBrushDef)
	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/99
	Inputs:		NewSeed - the value to set		
	Outputs:	pBrushDef - the brush attribute to change
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	To set the seed for the rotation random number generator
	SeeAlso:	-

********************************************************************************************/

BOOL ChangeBrushDefAction::ChangeRotationRandSeed(UINT32 NewSeed, BrushDefinition* pBrushDef)
{
	ERROR2IF(pBrushDef == NULL, FALSE, "brush attribute is NULL");
	
	pBrushDef->SetRotationRandSeed(NewSeed);

	return TRUE;
}


/********************************************************************************************

>	BOOL ChangeBrushDefAction::ChangeRotationRandSeed(UINT32 NewSeed, BrushDefinition* pBrushDef)
	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/99
	Inputs:		NewSeed - the value to set		
	Outputs:	pBrushDef - the brush attribute to change
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	To set the seed for the rotation random number generator
	SeeAlso:	-

********************************************************************************************/

BOOL ChangeBrushDefAction::ChangeRotationMaxPressure(UINT32 Pressure, BrushDefinition* pBrushDef)
{
	ERROR2IF(pBrushDef == NULL, FALSE, "brush attribute is NULL");

	return pBrushDef->SetRotationMaxPressure(Pressure);
}

/********************************************************************************************

>	BOOL ChangeBrushDefAction::ChangeTiled(BOOL bTile, BrushDefinition* pBrushDef)
	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/99
	Inputs:		bTile, to tile, or not	
	Outputs:	pBrushDef - the brush attribute to change
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	To set the tiling of this attribute node
	SeeAlso:	-

********************************************************************************************/
BOOL ChangeBrushDefAction::ChangeBrushTile(BOOL bTile, BrushDefinition* pBrushDef)
{
	ERROR2IF(pBrushDef == NULL, FALSE, "brush attribute is NULL");
	pBrushDef->SetTiling(bTile);

	return TRUE;
}


/********************************************************************************************

>	BOOL ChangeBrushDefAction::ChangeTangential(BOOL bRotate, BrushDefinition* pBrushDef)
	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/99
	Inputs:		bTile, to tile, or not	
	Outputs:	pBrushDef - the brush attribute to change
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	To set the tiling of this attribute node
	SeeAlso:	-

********************************************************************************************/

BOOL ChangeBrushDefAction::ChangeTangential(BOOL bRotate, BrushDefinition* pBrushDef)
{
	ERROR2IF(pBrushDef == NULL, FALSE, "brush attribute is NULL");
	
	pBrushDef->SetRotated(bRotate);

	return TRUE;
}


/********************************************************************************************

>	BOOL ChangeBrushDefAction::ChangeOffsetValue(MILLIPOINT NewOfdfset, BrushDefinition* pBrushDef)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/99
	Inputs:		NewOffset - the value to set		
	Outputs:	pBrushDef - the brush attribute to change
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	To set the offset value of this attribute node
	SeeAlso:	-

********************************************************************************************/

BOOL ChangeBrushDefAction::ChangeOffsetValue(MILLIPOINT NewOffset, BrushDefinition* pBrushDef)
{
	ERROR2IF(pBrushDef == NULL, FALSE, "brush attribute is NULL");
	
	pBrushDef->SetPathOffsetValue(NewOffset);

	return TRUE;
}


/********************************************************************************************

>	BOOL ChangeBrushDefAction::ChangeOffsetIncrProp(double NewIncrProp, BrushDefinition* pBrushDef)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/99
	Inputs:		NewIncrProp - the value to set		
	Outputs:	pBrushDef - the brush attribute to change
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	To set the offset value of this attribute node
	SeeAlso:	-

********************************************************************************************/

BOOL ChangeBrushDefAction::ChangeOffsetIncrProp(double NewIncrProp, BrushDefinition* pBrushDef)
{
	ERROR2IF(pBrushDef == NULL, FALSE, "brush attribute is NULL");
	
	pBrushDef->SetPathOffsetIncrProp(NewIncrProp);

	return TRUE;
}



/********************************************************************************************

>	BOOL ChangeBrushDefAction::ChangeOffsetIncrConst(MILLIPOINT NewIncr, BrushDefinition* pBrushDef)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/99
	Inputs:		NewIncrProp - the value to set		
	Outputs:	pBrushDef - the brush attribute to change
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	To set the offset value of this attribute node
	SeeAlso:	-

********************************************************************************************/

BOOL ChangeBrushDefAction::ChangeOffsetIncrConst(MILLIPOINT NewIncrConst, BrushDefinition* pBrushDef)
{
	ERROR2IF(pBrushDef == NULL, FALSE, "brush attribute is NULL");
	
	pBrushDef->SetPathOffsetIncrConst(NewIncrConst);

	return TRUE;
}



/********************************************************************************************

>	BOOL ChangeBrushDefAction::ChangeOffsetValMaxRand(UINT32 NewMax, BrushDefinition* pBrushDef)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/99
	Inputs:		NewMax - the value to set		
	Outputs:	pBrushDef - the brush attribute to change
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	To set how much the offset is determined randomly
	SeeAlso:	-

********************************************************************************************/

BOOL ChangeBrushDefAction::ChangeOffsetValMaxRand(UINT32 NewMax, BrushDefinition* pBrushDef)
{
	ERROR2IF(pBrushDef == NULL, FALSE, "brush attribute is NULL");
	
	pBrushDef->SetOffsetValueMaxRand(NewMax);

	return TRUE;
}


/********************************************************************************************

>	BOOL ChangeBrushDefAction::ChangeOffsetValRandSeed(UINT32 NewSeed, BrushDefinition* pBrushDef)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/99
	Inputs:		NewSeed - the value to set		
	Outputs:	pBrushDef - the brush attribute to change
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	To set the seed for the offset value RNG
	SeeAlso:	-

********************************************************************************************/

BOOL ChangeBrushDefAction::ChangeOffsetValRandSeed(UINT32 NewSeed, BrushDefinition* pBrushDef)
{
	ERROR2IF(pBrushDef == NULL, FALSE, "brush attribute is NULL");
	
	pBrushDef->SetOffsetValueRandSeed(NewSeed);

	return TRUE;
}

/********************************************************************************************

>	BOOL ChangeBrushDefAction::ChangeOffsetType(PathOffset NewOffset, BrushDefinition* pBrushDef)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/99
	Inputs:		NewOffset - the offset type to set		
	Outputs:	pBrushDef - the brush attribute to change
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	To set the offset value of this attribute node
	SeeAlso:	-

********************************************************************************************/

BOOL ChangeBrushDefAction::ChangeOffsetType(PathOffset NewOffset, BrushDefinition* pBrushDef)
{
	ERROR2IF(pBrushDef == NULL, FALSE, "brush attribute is NULL");
		
	pBrushDef->SetPathOffsetType(NewOffset);
	return TRUE;
}


/********************************************************************************************

>	BOOL ChangeBrushDefAction::ChangeOffsetTypeRandSeed(UINT32 NewSeed, BrushDefinition* pBrushDef)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/99
	Inputs:		NewSeed - the offset type to set		
	Outputs:	pBrushDef - the brush attribute to change
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	Sets the seed for the offset type RNG
	SeeAlso:	-

********************************************************************************************/

BOOL ChangeBrushDefAction::ChangeOffsetTypeSeed(UINT32 NewSeed, BrushDefinition* pBrushDef)
{
	ERROR2IF(pBrushDef == NULL, FALSE, "brush attribute is NULL");

	pBrushDef->SetOffsetTypeRandSeed(NewSeed);

	return TRUE;
}




/********************************************************************************************

>	BOOL ChangeBrushDefAction::ChangeSpacingMaxRand(UINT32 MaxRand, BrushDefinition* pBrushDef)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/99
	Inputs:		MaxRand - the new maximum random number
	Outputs:	pBrushDef - the brush attribute to change
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	To set the maximum random value for spacing for this attribute node
	SeeAlso:	-

********************************************************************************************/

BOOL ChangeBrushDefAction::ChangeSpacingMaxRand(UINT32 NewMaxRand, BrushDefinition* pBrushDef)
{
	ERROR2IF(pBrushDef == NULL, FALSE, "brush attribute is NULL");
	
	pBrushDef->SetSpacingMaxRand(NewMaxRand);

	return TRUE;
}


/********************************************************************************************

>	BOOL ChangeBrushDefAction::ChangeSpacingRandSeed(UINT32 RandSeed, BrushDefinition* pBrushDef)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/99
	Inputs:		RandSeed - seed to use for random spacing
	Outputs:	pBrushDef - the brush attribute to change
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	To set the random seed for spacing for this attribute node
	SeeAlso:	-

********************************************************************************************/

BOOL ChangeBrushDefAction::ChangeSpacingRandSeed(UINT32 NewSeed, BrushDefinition* pBrushDef)
{
	ERROR2IF(pBrushDef == NULL, FALSE, "brush attribute is NULL");
	
	pBrushDef->SetSpacingRandSeed(NewSeed);
	
	return TRUE;
}


/********************************************************************************************

>	BOOL ChangeBrushDefAction::ChangeUseLocalfillColour(BOOL UseLocal, BrushDefinition* pBrushDef)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/99
	Inputs:		UseLocal - the flag to set
	Outputs:	pBrushDef - the brush attribute to change
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	Sets the flag telling this attribute whether to use local attributes
	SeeAlso:	-

********************************************************************************************/

BOOL ChangeBrushDefAction::ChangeUseLocalFillColour(BOOL bUseLocal, BrushDefinition* pBrushDef)
{
	ERROR2IF(pBrushDef == NULL, FALSE, "brush attribute is NULL");
	
	return TRUE;
}


/********************************************************************************************

>	BOOL ChangeBrushDefAction::ChangeSequenceType(SequenceType NewSeq, BrushDefinition* pBrushDef)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/99
	Inputs:		NewSeq - the sequence type to set
	Outputs:	pBrushDef - the brush attribute to change
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	Sets the flag telling this attribute what type of object sequence to use
	SeeAlso:	-

********************************************************************************************/

BOOL ChangeBrushDefAction::ChangeSequenceType(SequenceType NewSeq, BrushDefinition* pBrushDef)
{
	ERROR2IF(pBrushDef == NULL, FALSE, "brush attribute is NULL");

	pBrushDef->SetSequenceType(NewSeq);
	return TRUE;
}



/********************************************************************************************

>	BOOL ChangeBrushDefAction::ChangeSequenceType(UINT32 NewSeed, BrushDefinition* pBrushDef)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/99
	Inputs:		NewSeed - the seed to set
	Outputs:	pBrushDef - the brush attribute to change
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	Seeds the sequence random number generator
	SeeAlso:	-

********************************************************************************************/

BOOL ChangeBrushDefAction::ChangeSequenceSeed(UINT32 NewSeed, BrushDefinition* pBrushDef)
{
	ERROR2IF(pBrushDef == NULL, FALSE, "brush attribute is NULL");

	pBrushDef->SetSequenceSeed(NewSeed);
	return TRUE;
}



/********************************************************************************************

>	BOOL ChangeBrushDefAction::ChangeTimeStamp(double NewTime, BrushDefinition* pBrushDef)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/99
	Inputs:		NewTime - the timestamping value to set (in ms)
	Outputs:	pBrushDef - the brush attribute to change
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	sets the timestamping value for this definition
	SeeAlso:	-

********************************************************************************************/

BOOL ChangeBrushDefAction::ChangeTimeStamp(double NewTime, BrushDefinition* pBrushDef)
{
	ERROR2IF(pBrushDef == NULL, FALSE, "brush attribute is NULL");

	return pBrushDef->SetTimeStampingPeriod(NewTime);
}

/********************************************************************************************

>	BOOL ChangeBrushDefAction::ChangeAll(ChangeBrushOpParam* pParam, BrushDefinition* pBrushDef)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/99
	Inputs:		pParam - the opparam containing the data to change in our brush
	Outputs:	pBrushDef - the brush attribute to change
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	Sets all the variables in the opparam
	SeeAlso:	-

********************************************************************************************/

BOOL ChangeBrushDefAction::ChangeAll(ChangeBrushOpParam* pParam, BrushDefinition* pBrushDef)
{
	ERROR2IF(pBrushDef == NULL, FALSE, "brush attribute is NULL in ChangeBrushDefAction::ChangeAll");
	ERROR2IF(pParam == NULL, FALSE, "OpParam is NULL in ChangeBrushDefAction::ChangeAll");
	

	// scaling members
	BOOL ok = pBrushDef->SetBrushScaling(pParam->m_NewScaling);
	if (ok) ok = pBrushDef->SetBrushScalingIncr(pParam->m_NewScalingIncr);
	if (ok) ok = pBrushDef->SetBrushScalingIncrConst(pParam->m_NewScalingIncrConst);
	pBrushDef->SetScalingMaxRand(pParam->m_NewScalingMaxRand);
	 pBrushDef->SetScalingRandSeed(pParam->m_NewScalingRandSeed);
	if (ok) ok = pBrushDef->SetScalingMaxPressure(pParam->m_NewScalingMaxPressure);

	// spacing members
	if (ok) ok = pBrushDef->SetSpacing(pParam->m_NewSpacing);
	if (ok) ok = pBrushDef->SetSpacingIncrConst(pParam->m_NewSpacingIncrConst);
	if (ok) ok = pBrushDef->SetSpacingIncrProp(pParam->m_NewSpacingIncrProp);
	pBrushDef->SetSpacingMaxRand(pParam->m_NewSpacingMaxRand);
	pBrushDef->SetSpacingRandSeed(pParam->m_NewSpacingRandSeed);
	
	
	// offset members
	pBrushDef->SetPathOffsetType(pParam->m_NewPathOffsetType);
	pBrushDef->SetPathOffsetValue(pParam->m_NewPathOffsetVal);
	if (ok) ok = pBrushDef->SetPathOffsetIncrConst(pParam->m_NewOffsetIncrConst);
	if (ok) ok = pBrushDef->SetPathOffsetIncrProp(pParam->m_NewOffsetIncrProp);
	pBrushDef->SetOffsetTypeRandSeed(pParam->m_NewOffsetTypeRandSeed);
	if (ok) ok = pBrushDef->SetOffsetValueMaxRand(pParam->m_NewOffsetValMaxRand);
	pBrushDef->SetOffsetValueRandSeed(pParam->m_NewOffsetValRandSeed);
	
	// rotation members
	pBrushDef->SetRotated(pParam->m_bNewRotated);
	if (ok) ok = pBrushDef->SetRotationAngle(pParam->m_NewRotateAngle);
	pBrushDef->SetRotationIncrConst(pParam->m_NewRotationIncrConst);
	pBrushDef->SetRotationIncrProp(pParam->m_NewRotationIncrProp);
	pBrushDef->SetRotationMaxRand(pParam->m_NewRotateMaxRand);
	pBrushDef->SetRotationMaxPressure(pParam->m_NewRotationMaxPressure);
	
	// sequence members
	pBrushDef->SetSequenceType(pParam->m_NewSequenceType);
	pBrushDef->SetSequenceSeed(pParam->m_NewSequenceRandSeed);
	pBrushDef->SetTiling(pParam->m_bNewTiled);
	
	if (ok) ok = pBrushDef->SetHueIncrement(1.0);
	if (ok) ok = pBrushDef->SetHueMaxRand(pParam->m_NewHueMaxRand);
	if (ok) ok = pBrushDef->SetHueRandSeed(pParam->m_NewHueRandSeed);
	if (ok) ok = pBrushDef->SetSatIncrement(0.0); 
	if (ok) ok = pBrushDef->SetSatMaxRand(pParam->m_NewSatMaxRand);
	if (ok) ok = pBrushDef->SetSatRandSeed(pParam->m_NewSatRandSeed);

	if (ok) ok = pBrushDef->SetBrushTransparency(pParam->m_NewTransp);
	if (ok) ok = pBrushDef->SetTransparencyPressure(pParam->m_NewTranspPressure);
	return ok;
}

/*------------------------------------------------------------------------------------------
------------------------CHANGEBRUSHDEFOPPARAM CLASS IMPLEMENTATION--------------------------
--------------------------------------------------------------------------------------------*/


/********************************************************************************************

>	ChangeBrushDefOpParam::ChangeBrushDefOpParam(const ChangeBrushOpParam& Other)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/3/95
	Inputs:		Other the ChangeBrushOpParam whose values we wish to assign
	Outputs:	-
	Returns:	-
	Purpose:	kind of a copy constructor, but takes a member of the base class 
	Errors:		-

********************************************************************************************/

ChangeBrushDefOpParam::ChangeBrushDefOpParam(const ChangeBrushOpParam& Other) 
{
	
}
/********************************************************************************************

>	ChangeBrushDefOpParam ChangeBrushDefOpParam::operator=(const ChangeBrushOpParam& Other)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/3/95
	Inputs:		Other the ChangeBrushOpParam whose values we wish to assign
	Outputs:	-
	Returns:	-
	Purpose:	overrides assignment operator
	Errors:		-

********************************************************************************************/

ChangeBrushOpParam ChangeBrushDefOpParam::operator=(const ChangeBrushOpParam& Other) 
{
	ChangeBrushOpParam::operator =(Other);
	if (Other.IsKindOf(CC_RUNTIME_CLASS(ChangeBrushDefOpParam)))
	{
		ChangeBrushDefOpParam* pOther = (ChangeBrushDefOpParam*)&Other;
		m_Handle = pOther->m_Handle;
	}
	return *this;
}


/********************************************************************************************

>	void ChangeBrushDefOpParam::SetOldValues(BrushDefinition* pBrushDef)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/3/95
	Inputs:		pAttrBrush - pointer to brush attribute which is being changed
	Outputs:	-
	Returns:	-
	Purpose:	Copies the current state of the brush node into this classes Old member vars
	Errors:		-

********************************************************************************************/

void ChangeBrushDefOpParam::SetOldValues(BrushDefinition* pBrushDef)
{
	if (pBrushDef == NULL)
	{
		ERROR3("Attribute is NULL");
		return;
	}

	// spacing
	m_OldSpacing          = pBrushDef->GetSpacing();
	m_OldSpacingMaxRand   = pBrushDef->GetSpacingMaxRand();
	m_OldSpacingRandSeed  = pBrushDef->GetSpacingRandSeed();
	m_OldSpacingIncrProp  = pBrushDef->GetSpacingIncrProp();
	m_OldSpacingIncrConst = pBrushDef->GetSpacingIncrConst();
	
	// scaling
	m_OldScaling          = pBrushDef->GetBrushScaling();
	m_OldScalingIncr	  = pBrushDef->GetBrushScalingIncr();
	m_OldScalingIncrConst = pBrushDef->GetBrushScalingIncrConst();
	m_OldScalingMaxPressure = pBrushDef->GetScalingMaxPressure();
	m_OldScalingMaxRand   = pBrushDef->GetScalingMaxRand();
	m_OldScalingRandSeed  = pBrushDef->GetScalingRandSeed();

	// offset
	m_OldPathOffsetVal    = pBrushDef->GetPathOffsetValue();
	m_OldOffsetIncrProp   = pBrushDef->GetPathOffsetIncrProp();
	m_OldOffsetIncrConst  = pBrushDef->GetPathOffsetIncrConst();
	m_OldPathOffsetType   = pBrushDef->GetPathOffsetType();
	m_OldOffsetValMaxRand = pBrushDef->GetOffsetValueMaxRand();
	m_OldOffsetValRandSeed = pBrushDef->GetOffsetValueRandSeed();
	m_OldOffsetTypeRandSeed = pBrushDef->GetOffsetTypeRandSeed();
	
	m_bOldTiled           = pBrushDef->IsTiled();
	
	// rotation
	m_bOldRotated         = pBrushDef->IsRotated();
	m_OldRotateAngle      = pBrushDef->GetRotationAngle();
	m_OldRotateRandSeed   = pBrushDef->GetRotationRandSeed();
	m_OldRotateMaxRand    = pBrushDef->GetRotationMaxRand();
	m_OldRotationMaxPressure = pBrushDef->GetRotationMaxPressure();
	m_OldRotationIncrConst = pBrushDef->GetRotationIncrConst();

	// sequence
	m_OldSequenceRandSeed   = pBrushDef->GetSequenceSeed();
	m_OldSequenceType       = pBrushDef->GetSequenceType();

	// fills
	m_OldHueIncrement		= pBrushDef->GetHueIncrement();
	m_OldHueMaxRand			= pBrushDef->GetHueMaxRand();
	m_OldHueRandSeed		= pBrushDef->GetHueRandSeed();
	
	m_OldSatMaxRand			= pBrushDef->GetSatMaxRand();
	m_OldSatRandSeed		= pBrushDef->GetSatRandSeed();

	m_OldTransp				= pBrushDef->GetBrushTransparency();
	m_OldTranspPressure		= pBrushDef->GetTransparencyPressure();
}


/********************************************************************************************

>	void ChangeBrushDefOpParam::SetNewValues(BrushDefinition* pBrushDef)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/3/95
	Inputs:		pAttrBrush - the attribute being changed by the operation
	Outputs:	-
	Returns:	-
	Purpose:	Copies the current state of the brush node into this classes New member vars
	Errors:		-

********************************************************************************************/

void ChangeBrushDefOpParam::SetNewValues(BrushDefinition* pBrushDef)
{
	if (pBrushDef == NULL)
	{
		ERROR3("Brush Definition is NULL");
		return;
	}

	// spacing
	m_NewSpacing          = pBrushDef->GetSpacing();
	m_NewSpacingMaxRand   = pBrushDef->GetSpacingMaxRand();
	m_NewSpacingRandSeed  = pBrushDef->GetSpacingRandSeed();
	m_NewSpacingIncrProp  = pBrushDef->GetSpacingIncrProp();
	m_NewSpacingIncrConst = pBrushDef->GetSpacingIncrConst();
	
	// scaling
	m_NewScaling          = pBrushDef->GetBrushScaling();
	m_NewScalingIncr	  = pBrushDef->GetBrushScalingIncr();
	m_NewScalingIncrConst = pBrushDef->GetBrushScalingIncrConst();
	m_NewScalingMaxPressure = pBrushDef->GetScalingMaxPressure();

	m_NewPathOffsetVal    = pBrushDef->GetPathOffsetValue();
	m_NewOffsetIncrProp   = pBrushDef->GetPathOffsetIncrProp();
	m_NewOffsetIncrConst  = pBrushDef->GetPathOffsetIncrConst();
	m_NewPathOffsetType   = pBrushDef->GetPathOffsetType();
	m_bNewTiled           = pBrushDef->IsTiled();
	m_bNewRotated         = pBrushDef->IsRotated();
	m_NewRotateAngle      = pBrushDef->GetRotationAngle();

	m_NewOffsetTypeRandSeed = pBrushDef->GetOffsetTypeRandSeed();
	m_NewSequenceRandSeed   = pBrushDef->GetSequenceSeed();
	m_NewSequenceType		= pBrushDef->GetSequenceType();
	m_NewTimeStampValue     = (UINT32)pBrushDef->GetTimeStampingPeriod();


}

/*------------------------------------------------------------------------------------------
--------------------------------------------------------------------------------------------
------------------------OpSelectBrush-------------------------------------------------------
------------------------------------------------------------------------------------------*/


/********************************************************************************************

>	OpSelectBrush::OpSelectBrush()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/99
	Returns:	-
	Purpose:	Constructor

********************************************************************************************/

OpSelectBrush::OpSelectBrush()
{
	 
}


/********************************************************************************************

>	OpSelectBrush::~OpSelectBrush()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/99
	Returns:	-
	Purpose:	destructor
********************************************************************************************/

OpSelectBrush::~OpSelectBrush()
{
	
}

/********************************************************************************************

>	BOOL OpSelectBrush::Declare()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/99
	Returns:	TRUE if all went OK, FALSE otherwise
	Purpose:	Adds the operation to the list of all known operations

********************************************************************************************/

BOOL OpSelectBrush::Declare()
{
	return (RegisterOpDescriptor(
								0, 
								_R(IDS_BRUSH_MAKESELECTED),
								CC_RUNTIME_CLASS(OpSelectBrush), 
								OPTOKEN_SELECTBRUSH,
								OpSelectBrush::GetState,
								0,  /* help ID */
								0,	/* bubble ID */
								0	/* bitmap ID */
								));
}



/********************************************************************************************

>	static OpState OpSelectBrush::GetState(String_256* Description, OpDescriptor*)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/99
	Outputs:	Description - GetState fills this string with an approriate description
				of the current state of the operation 
	Returns:	The state of the operation, so that menu items (ticks and greying) can be
				done properly
	Purpose:	Find out the state of the operation at the specific time, in this instance
				the operation can only be launched when we are in the freehand tool.

********************************************************************************************/

OpState OpSelectBrush::GetState(String_256* Description, OpDescriptor*)
{
	OpState State;
	
	// make sure we have one and only one brush in the selection before we launch the op
	AttrBrushType* pAttrBrush = CBrushEditDlg::GetSingleSelectedBrushAttribute();
	BrushHandle Handle = BrushHandle_NoBrush;
	if (pAttrBrush != NULL)
		Handle = pAttrBrush->GetBrushHandle();

	// likewise we must be in the freehand tool
	Tool* pTool = Tool::GetCurrent();
	if (pTool != NULL && pTool->GetID() != TOOLID_FREEHAND)
		pTool = NULL;

	// plus its only worth doing if we are selecting an actual brush
	if (Handle != BrushHandle_NoBrush && pTool != NULL)
		State.Greyed = FALSE;
	else
		State.Greyed = TRUE;
	
	return State;

}

/********************************************************************************************

>	virtual void OpSelectBrush::GetOpName(String_256* OpName) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/99
	Inputs:		-
	Outputs:	The undo string for the operation
	Returns:	
	Purpose:	like the title says
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void OpSelectBrush::GetOpName(String_256* OpName)
{
	*OpName = String_256(_R(IDS_BRUSH_MAKESELECTED));
}  


/********************************************************************************************

>	void OpSelectBrush::Do(OpDescriptor* pOpDesc)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/99
	Inputs:		-
	Outputs:	-
	Returns:	
	Purpose:	the all-important Do function
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void OpSelectBrush::Do(OpDescriptor* pOpDesc)
{

	AttrBrushType* pSelBrush = CBrushEditDlg::GetSingleSelectedBrushAttribute();

	if (pSelBrush == NULL)
	{
		ERROR3("No selected brush in OpSelectBrush::Do");
		return;
	}

	BrushHandle SelHandle = pSelBrush->GetBrushHandle();
	
	// all we need to do is send a message
	BROADCAST_TO_ALL(BrushMsg(SelHandle));

	End();
}


/*--------------------------------------------------------------------------------------------
-----------------------OpDeactivateBrush implementation---------------------------------------
---------------------------------------------------------------------------------------------*/


/********************************************************************************************

>	OpDeactivateBrush::OpDeactivateBrush()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/99
	Returns:	-
	Purpose:	Constructor

********************************************************************************************/

OpDeactivateBrush::OpDeactivateBrush()
{
	 
}


/********************************************************************************************

>	OpDeactivateBrush::~OpDeactivateBrush()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/99
	Returns:	-
	Purpose:	destructor
********************************************************************************************/

OpDeactivateBrush::~OpDeactivateBrush()
{
	
}

/********************************************************************************************

>	BOOL OpDeactivateBrush::Declare()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/99
	Returns:	TRUE if all went OK, FALSE otherwise
	Purpose:	Adds the operation to the list of all known operations

********************************************************************************************/

BOOL OpDeactivateBrush::Declare()
{
	return (RegisterOpDescriptor(
								0, 
								0,
								CC_RUNTIME_CLASS(OpDeactivateBrush), 
								OPTOKEN_DELETEBRUSH,
								OpDeactivateBrush::GetState,
								0,  /* help ID */
								0,	/* bubble ID */
								0	/* bitmap ID */
								));
}



/********************************************************************************************

>	static OpState OpDeactivateBrush::GetState(String_256* Description, OpDescriptor*)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/99
	Outputs:	Description - GetState fills this string with an approriate description
				of the current state of the operation 
	Returns:	The state of the operation, so that menu items (ticks and greying) can be
				done properly
	Purpose:	Find out the state of the operation at the specific time

********************************************************************************************/

OpState OpDeactivateBrush::GetState(String_256* Description, OpDescriptor*)
{
	OpState Bob;
	

	return Bob;

}


/********************************************************************************************

>	virtual void OpDeactivateBrush::GetOpName(String_256* OpName) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/99
	Inputs:		-
	Outputs:	The undo string for the operation
	Returns:	
	Purpose:	like the title says
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void OpDeactivateBrush::GetOpName(String_256* OpName)
{
	*OpName = String_256(_R(IDS_BRUSHDELETE));
}

/********************************************************************************************

>	void OpDeactivateBrush::DoWithParam(OpDescriptor*,OpParam* pOpParam)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/99
	Returns:	-
	Purpose:	Deactivates a brush definition so that it no longer appears in the line gallery
				or brush combo, and all existing attributes that make use of it will appear
				as if they were a straight line.

********************************************************************************************/

void OpDeactivateBrush::DoWithParam(OpDescriptor*,OpParam* pParam)
{
	BrushHandle Handle = static_cast<BrushHandle> (pParam->Param1);

	if (Handle != BrushHandle_NoBrush)
	{
		DeactivateBrushDefAction* pAction;
		if (DeactivateBrushDefAction::Init(this,&UndoActions,Handle, &pAction) == AC_FAIL)
			FailAndExecute();
		End();
	}

}




//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
// The DeactivateBrushDefAction class


/********************************************************************************************

>	ActionCode DeactivateBrushDefAction::Init(Operation* pOp, ActionList* pActionList, BrushHandle Handle,
										  DeactivateBrushDefAction** ppNewAction)


	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/99
	Inputs:		pOp  			= ptr to the operation to which this action belongs
				pActionList 	=  ptr to action list to which this action should be added
				Handle			- handle to the definition to (de)activate
	Outputs:	ppNewAction  	= ptr to a ptr to an action, allowing the function to return
								  a pointer to the created action
	Returns:	ActionCode, one of AC_OK, AC_NO_RECORD or AC_FAIL
	Purpose:	To de/reactivate a brush definition in an undoable way
	Errors:		-
	SeeAlso:	Action::Init()

********************************************************************************************/

ActionCode DeactivateBrushDefAction::Init(Operation* pOp, ActionList* pActionList, BrushHandle Handle,
										  DeactivateBrushDefAction** ppNewAction)


{
	BrushDefinition* pDef = BrushComponent::FindDefinition(Handle, TRUE);
	ERROR2IF(pDef == NULL, AC_FAIL, "Invalid handle passed to DeactivateBrushDefAction::Init");
	
	
	// make the new action
	UINT32 ActSize = sizeof(DeactivateBrushDefAction);
	DeactivateBrushDefAction* pNewAction;
	ActionCode Ac = Action::Init(pOp,pActionList,ActSize,CC_RUNTIME_CLASS(DeactivateBrushDefAction),(Action**)&pNewAction);
	*ppNewAction = pNewAction;

	pNewAction->m_Handle = Handle;
	
	// get the line gallery
	LineGallery* pGallery = LineGallery::GetInstance();
	ERROR2IF(pGallery == NULL, AC_FAIL, "Unable to get line gallery in DeactivateBrushDefAction::Init");

	// get the freehand tool
	ToolListItem* pToolItem = Tool::Find(TOOLID_FREEHAND);
	ERROR2IF(pToolItem == NULL, AC_FAIL, "Unable to get tool item in DeactivateBrushDefAction::Init");

	FreeHandTool* pTool = (FreeHandTool*)pToolItem->pTool;
	ERROR2IF(pTool == NULL, AC_FAIL, "Unable to get tool in DeactivateBrushDefAction::Init");

	if (pDef->IsActivated())
	{
		// we are deactivating, we need to delete from the line gallery and freehandinfo bar
		pDef->SetActivated(FALSE);
		
		pGallery->DeleteBrushItem(Handle);

		pTool->BrushDeleted(Handle);

	}
	else
	{
		// we are activating:
		pDef->SetActivated(TRUE);
		
		// make a new atttibute to add to the line gallery
		PathProcessorBrush* pNewPathProc = new PathProcessorBrush;
		if (pNewPathProc == NULL)
		{
			ERROR3("Failed to allocate path processor");
			return AC_FAIL;
		}

		// tell it which brush definition to use
		pNewPathProc->SetBrushDefinition(Handle);

		// transfer all the data
		pDef->CopyDataToProcessor(pNewPathProc);

		// make a new attribute value for use by the line gallery
		BrushAttrValue* pVal = new BrushAttrValue(pNewPathProc);
		if (pVal == NULL)
		{
			ERROR3("Unable to allocate new BrushAttrVal");
			return AC_FAIL;
		}
	
		// add it to the line gallery
		LineGallery::AddNewBrushItem(pVal);	

		
		// get the infobar
		FreeHandInfoBarOp* pInfoBar = ((FreeHandTool*)pTool)->GetInfoBar();
		ERROR2IF(pInfoBar == NULL, AC_FAIL, "Unable to get InfoBar in DeactivateBrushDefAction::Init");

		// add to the freehand infobar
		pInfoBar->AddBrush(Handle);
	}
	
	// force a redraw of the document
	Document* pDoc = Document::GetCurrent();
	if (pDoc != NULL)
	{
		CBitmapCache* pBC = Camelot.GetBitmapCache();
		if (pBC) pBC->DeInitialise();
		pDoc->ForceRedraw();
	}

	return Ac;
}




/********************************************************************************************

>	ActionCode ChangeBrushDefAction::Execute();

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/99
	Inputs:		-
	Outputs:	-
	Returns:	ActionCode, one of AC_OK, AC_NO_RECORD or AC_FAIL
	Purpose:	Executes the action.  This will reset the brush variables to their old values
	Errors:		-
	SeeAlso:	Action::Init()

********************************************************************************************/

ActionCode DeactivateBrushDefAction::Execute()
{
	ActionCode Act;
	DeactivateBrushDefAction* pAction;
	Act = DeactivateBrushDefAction::Init(pOperation,pOppositeActLst,m_Handle,&pAction);

	return Act;
}
