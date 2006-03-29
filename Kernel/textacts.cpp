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
// Actions used by text operations

/*
*/

#include "camtypes.h"
#include "textacts.h"

// Code headers
#include "nodetext.h"
#include "ndtxtpth.h"

// Resource headers


DECLARE_SOURCE("$Revision: 662 $");

CC_IMPLEMENT_DYNCREATE(BaseTextAction, Action)
CC_IMPLEMENT_DYNCREATE(StoreCharCodeAction, BaseTextAction)
CC_IMPLEMENT_DYNCREATE(StoreKernCodeAction, BaseTextAction)
CC_IMPLEMENT_DYNCREATE(ReversePathAction, BaseTextAction)
CC_IMPLEMENT_DYNCREATE(MatrixRemoveFromPathAction, BaseTextAction)
CC_IMPLEMENT_DYNCREATE(MatrixFitToPathAction, BaseTextAction)

#define new CAM_DEBUG_NEW



/********************************************************************************************
>	BaseTextAction::BaseTextAction()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/5/95
	Purpose:	Constructor - initialises the member variables
********************************************************************************************/
BaseTextAction::BaseTextAction()
{
	pStory = NULL;
}



////////////////////////////////////////////////////////////////////////////////////////////
// StoreCharCodeAction

/********************************************************************************************
>	StoreCharCodeAction::StoreCharCodeAction()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/5/95
	Purpose:	Constructor - initialises the member variables
********************************************************************************************/
StoreCharCodeAction::StoreCharCodeAction()
{
	pChar = NULL;
	CharCode = 0;
}


/********************************************************************************************
>	StoreCharCodeAction::~StoreCharCodeAction()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/5/95
	Purpose:	Destructor
********************************************************************************************/
StoreCharCodeAction::~StoreCharCodeAction()
{
}


/********************************************************************************************
>	virtual ActionCode StoreCharCodeAction::Execute()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/5/95
	Purpose:	Called during undo/redo to restore a character code to a particular character
				Inserts StoreCharCodeAction to restore the current character code then
				sets it to the value in the action
********************************************************************************************/
ActionCode StoreCharCodeAction::Execute()
{
	// Check the member variables
	ERROR2IF(pStory == NULL, AC_FAIL, "The TextStory pointer is NULL");
	ERROR2IF(pChar == NULL, AC_FAIL, "TextChar pointer is NULL");

	// Insert an action to restore the character code to its current value on redo
	ActionCode ActCode;
	ActCode = StoreCharCodeAction::Init(pOperation, pOppositeActLst, pStory, pChar);

	// Now set the character code the old value
	if (ActCode != AC_FAIL)
		pChar->SetUnicodeValue(CharCode);

	return ActCode;
}


/********************************************************************************************
>	static ActionCode StoreCharCodeAction::Init(Operation* pOp,
												ActionList* pActionList,
												TextStory* pTextStory,
												TextChar* pTextChar)
	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/5/95
	Inputs:		pOp - points to the operation to add the action to
				pActionList - points to the action list to add the action to
				pTextStory - points to the story containing pChar
				pTextChar - points to the character to store the character code of
	Outputs:	Constructs a StoreCharCodeAction, initialises it and inserts in into the action list
	Returns:	An action code depending on the success of this function
	Purpose:	Called to constuct a StoreCharCodeAction, set it up and and insert it into
				an operations action list
	Errors:		Checks the parameters
********************************************************************************************/
ActionCode StoreCharCodeAction::Init(Operation* pOp, ActionList* pActionList, TextStory* pTextStory, TextChar* pTextChar)
{
	// Check the parameters
	ERROR2IF(pOp == NULL, AC_FAIL, "The operation pointer is NULL");
	ERROR2IF(pActionList == NULL, AC_FAIL, "The action list pointer is NULL");
	ERROR2IF(pTextStory == NULL, AC_FAIL, "The TextStory pointer is NULL");
	ERROR2IF(pTextChar == NULL, AC_FAIL, "TextChar pointer is NULL");

	// Create a new action and insert it in the action list
	UINT32 ActSize = sizeof(StoreCharCodeAction);
	StoreCharCodeAction* pNewAction = NULL;
	ActionCode Ac = Action::Init( pOp, pActionList, ActSize, CC_RUNTIME_CLASS(StoreCharCodeAction), (Action**)&pNewAction);

	// Set the members of the new action
	if ((Ac == AC_OK) && (pNewAction != NULL))
	{
		pNewAction->pStory = pTextStory;
		pNewAction->pChar = pTextChar;
		pNewAction->CharCode = pTextChar->GetUnicodeValue();
	}

	return Ac;
}


/********************************************************************************************
>	static BOOL StoreCharCodeAction::DoStoreCharacterCode(Operation* pOp,
														  ActionList* pActionList,
														  TextChar* pTextChar)
	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/5/95
	Inputs:		pOp - points to the operation to add the action to
				pActionList - points to the action list to add the action to
				pTextChar - points to the character to store the character code of
	Outputs:	Adds an action to the operations action list
	Returns:	TRUE/FALSE for success/failure
	Purpose:	Before changing a characters UniCode value in your op, call this function to
				store the characters current value so it is restored on undo
	Errors:		Checks on parameters and structure of TextStory
********************************************************************************************/
BOOL StoreCharCodeAction::DoStoreCharacterCode( Operation* pOp, ActionList* pActionList, TextChar* pTextChar)
{
	// Check the parameters
	ERROR2IF(pOp == NULL, AC_FAIL, "The operation pointer is NULL");
	ERROR2IF(pActionList == NULL, AC_FAIL, "The action list pointer is NULL");
	ERROR2IF(pTextChar == NULL, AC_FAIL, "TextChar pointer is NULL");

	// Get a pointer to the characters parent TextStory
	TextStory* pParentStory = (TextStory*)pTextChar->FindParent(CC_RUNTIME_CLASS(TextStory));
	ERROR2IF(pParentStory == NULL, AC_FAIL, "TextChar has no parent TextStory");

	// Insert the action
	ActionCode Ac;
	Ac = Init(pOp, pActionList, pParentStory, pTextChar);

	return (Ac != AC_FAIL);
}



////////////////////////////////////////////////////////////////////////////////////////////
// StoreKernCodeAction

/********************************************************************************************
>	StoreKernCodeAction::StoreKernCodeAction()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/5/95
	Purpose:	Constructor - initialises the member variables
********************************************************************************************/
StoreKernCodeAction::StoreKernCodeAction()
{
	pKernCode = NULL;
	KernValue = DocCoord(0,0);
}


/********************************************************************************************
>	StoreKernCodeAction::~StoreKernCodeAction()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/5/95
	Purpose:	Destructor
********************************************************************************************/
StoreKernCodeAction::~StoreKernCodeAction()
{
}


/********************************************************************************************
>	virtual ActionCode StoreKernCodeAction::Execute()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/5/95
	Purpose:	Called during undo/redo to restore a kern code value
				Inserts a StoreKernCodeAction to restore the current kern value then
				sets it to the value in the action
********************************************************************************************/
ActionCode StoreKernCodeAction::Execute()
{
	// Check the member variables
	ERROR2IF(pStory == NULL, AC_FAIL, "The TextStory pointer is NULL");
	ERROR2IF(pKernCode == NULL, AC_FAIL, "TextChar pointer is NULL");

	// Insert an action to restore the kern code to its current value on redo
	ActionCode ActCode;
	ActCode = StoreKernCodeAction::Init(pOperation, pOppositeActLst, pStory, pKernCode);

	// Now set the kern code the old value
	if (ActCode != AC_FAIL)
		pKernCode->SetValue(KernValue);

	return ActCode;
}


/********************************************************************************************
>	static ActionCode StoreKernCodeAction::Init(Operation* pOp,
												ActionList* pActionList,
												TextStory* pTextStory,
												KernCode* pKern)
	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/5/95
	Inputs:		pOp - points to the operation to add the action to
				pActionList - points to the action list to add the action to
				pTextStory - points to the story containing pChar
				pKernCode - points to the kern code to store the value of
	Outputs:	Constructs a StoreKernCodeAction, initialises it and inserts in into the action list
	Returns:	An action code depending on the success of this function
	Purpose:	Called to constuct a StoreKernCodeAction, set it up and and insert it into
				an operations action list
	Errors:		Checks the parameters
********************************************************************************************/
ActionCode StoreKernCodeAction::Init(Operation* pOp, ActionList* pActionList, TextStory* pTextStory, KernCode* pKern)
{
	// Check the parameters
	ERROR2IF(pOp == NULL, AC_FAIL, "The operation pointer is NULL");
	ERROR2IF(pActionList == NULL, AC_FAIL, "The action list pointer is NULL");
	ERROR2IF(pTextStory == NULL, AC_FAIL, "The TextStory pointer is NULL");
	ERROR2IF(pKern == NULL, AC_FAIL, "KernCode pointer is NULL");

	// Create a new action and insert it in the action list
	UINT32 ActSize = sizeof(StoreKernCodeAction);
	StoreKernCodeAction* pNewAction = NULL;
	ActionCode Ac = Action::Init( pOp, pActionList, ActSize, CC_RUNTIME_CLASS(StoreKernCodeAction), (Action**)&pNewAction);

	// Set the members of the new action
	if ((Ac == AC_OK) && (pNewAction != NULL))
	{
		pNewAction->pStory = pTextStory;
		pNewAction->pKernCode = pKern;
		pNewAction->KernValue = pKern->GetValue();
	}

	return Ac;
}


/********************************************************************************************
>	static BOOL StoreKernCodeAction::DoStoreKernValue(Operation* pOp,
													  ActionList* pActionList,
													  KernCode* pKern)
	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/5/95
	Inputs:		pOp - points to the operation to add the action to
				pActionList - points to the action list to add the action to
				pKern - points to the kern code to store the value of
	Outputs:	Adds an action to the operations action list
	Returns:	TRUE/FALSE for success/failure
	Purpose:	Before changing a kern code value in your op, call this function to store the
				current value so it is restored on undo
	Errors:		Checks on parameters and structure of TextStory
********************************************************************************************/
BOOL StoreKernCodeAction::DoStoreKernValue( Operation* pOp, ActionList* pActionList, KernCode* pKern)
{
	// Check the parameters
	ERROR2IF(pOp == NULL, AC_FAIL, "The operation pointer is NULL");
	ERROR2IF(pActionList == NULL, AC_FAIL, "The action list pointer is NULL");
	ERROR2IF(pKern == NULL, AC_FAIL, "TextChar pointer is NULL");

	// Get a pointer to the characters parent TextStory
	TextStory* pParentStory = (TextStory*)pKern->FindParent(CC_RUNTIME_CLASS(TextStory));
	ERROR2IF(pParentStory == NULL, AC_FAIL, "TextChar has no parent TextStory");

	// Insert the action
	ActionCode Ac;
	Ac = Init(pOp, pActionList, pParentStory, pKern);

	return (Ac != AC_FAIL);
}



////////////////////////////////////////////////////////////////////////////////////////////
// ReversePathAction

/********************************************************************************************
>	ReversePathAction::ReversePathAction()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/5/95
	Purpose:	Constructor - initialises the member variables
********************************************************************************************/
ReversePathAction::ReversePathAction()
{
}


/********************************************************************************************
>	ReversePathAction::~ReversePathAction()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/5/95
	Purpose:	Destructor
********************************************************************************************/
ReversePathAction::~ReversePathAction()
{
}


/********************************************************************************************
>	virtual ActionCode ReversePathAction::Execute()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/5/95
	Purpose:	Called during undo/redo to reverse the path inside a TextStory
				Inserts a similar action into the opposite action list then reverses the path
********************************************************************************************/
ActionCode ReversePathAction::Execute()
{
	// Check the member variables
	ERROR2IF(pStory == NULL, AC_FAIL, "The TextStory pointer is NULL");
	ERROR2IF(pOperation == NULL, AC_FAIL, "The operation pointer is NULL(!)");

	// Insert an action to reverse the path again on redo
	ActionCode ActCode;
	ActCode = ReversePathAction::Init(pOperation, pOppositeActLst, pStory);

	// Now reverse the path
	if (ActCode != AC_FAIL)
		pStory->ReverseTextOnPath();

	return ActCode;
}


/********************************************************************************************
>	static ActionCode ReversePathAction::Init(Operation* pOp,
												ActionList* pActionList,
												TextStory* pTextStory)
	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/5/95
	Inputs:		pOp - points to the operation to add the action to
				pActionList - points to the action list to add the action to
				pTextStory - points to the story containing pChar
	Outputs:	Constructs a ReversePathAction, initialises it and inserts in into the action list
	Returns:	An action code depending on the success of this function
	Purpose:	Called to constuct a ReversePathAction, set it up and and insert it into
				an operations action list
	Errors:		Checks the parameters
********************************************************************************************/
ActionCode ReversePathAction::Init(Operation* pOp, ActionList* pActionList, TextStory* pTextStory)
{
	// Check the parameters
	ERROR2IF(pOp == NULL, AC_FAIL, "The operation pointer is NULL");
	ERROR2IF(pActionList == NULL, AC_FAIL, "The action list pointer is NULL");
	ERROR2IF(pTextStory == NULL, AC_FAIL, "The TextStory pointer is NULL");

	// Create a new action and insert it in the action list
	UINT32 ActSize = sizeof(ReversePathAction);
	ReversePathAction* pNewAction = NULL;
	ActionCode Ac = Action::Init( pOp, pActionList, ActSize, CC_RUNTIME_CLASS(ReversePathAction), (Action**)&pNewAction);

	// Set the members of the new action
	if ((Ac == AC_OK) && (pNewAction != NULL))
		pNewAction->pStory = pTextStory;

	return Ac;
}


/********************************************************************************************
>	static BOOL ReversePathAction::DoReversePath(Operation* pOp,
												 ActionList* pActionList,
												 TextStory* pTextStory)
	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/5/95
	Inputs:		pOp - points to the operation to add the action to
				pActionList - points to the action list to add the action to
				pTextStory - points to the TextStory to reverse the path in
	Outputs:	Adds an action to the operations action list
	Returns:	TRUE/FALSE for success/failure
	Purpose:	Call this function to reverse the "Text on a path" path in a your op.  It
				inserts an action to undo the change, then reverses the path for you
	Errors:		Checks on parameters and structure of TextStory
********************************************************************************************/
BOOL ReversePathAction::DoReversePath(Operation* pOp, ActionList* pActionList, TextStory* pTextStory)
{
	// Check the parameters
	ERROR2IF(pOp == NULL, FALSE, "The operation pointer is NULL");
	ERROR2IF(pActionList == NULL, FALSE, "The action list pointer is NULL");
	ERROR2IF(pTextStory == NULL, FALSE, "The TextStory pointer is NULL");
	ERROR3IF(pTextStory->GetTextPath() == NULL, "TextStory was not on a path");

	// Insert an action to restore the path on undo (just reverse it back)
	ActionCode ActCode;
	ActCode = ReversePathAction::Init(pOp, pActionList, pTextStory);

	// Now reverse the path
	if (ActCode != AC_FAIL)
		pTextStory->ReverseTextOnPath();

	return (ActCode != AC_FAIL);
}



////////////////////////////////////////////////////////////////////////////////////////////
// MatrixFitToPathAction

/********************************************************************************************
>	MatrixFitToPathAction::MatrixFitToPathAction()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/5/95
	Purpose:	Constructor - initialises the member variables
********************************************************************************************/
MatrixFitToPathAction::MatrixFitToPathAction()
{
}


/********************************************************************************************
>	MatrixFitToPathAction::~MatrixFitToPathAction()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/5/95
	Purpose:	Destructor
********************************************************************************************/
MatrixFitToPathAction::~MatrixFitToPathAction()
{
}


/********************************************************************************************
>	virtual ActionCode MatrixFitToPathAction::Execute()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/5/95
	Purpose:	Called during undo/redo to restore the TextStory matrices
				Inserts an opposite action into the opposite action list then changes the
				matrices
********************************************************************************************/
ActionCode MatrixFitToPathAction::Execute()
{
	// Check the member variables
	ERROR2IF(pStory == NULL, AC_FAIL, "The TextStory pointer is NULL");
	ERROR2IF(pOperation == NULL, AC_FAIL, "The operation pointer is NULL(!)");

	// Insert an action to reverse the path again on redo
	ActionCode ActCode;
	ActCode = MatrixRemoveFromPathAction::Init(pOperation, pOppositeActLst, pStory);

	// Now set the matrices
	if (ActCode != AC_FAIL)
		pStory->MatrixFitToPath();

	return ActCode;
}


/********************************************************************************************
>	static ActionCode MatrixFitToPathAction::Init(Operation* pOp,
												ActionList* pActionList,
												TextStory* pTextStory)
	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/5/95
	Inputs:		pOp - points to the operation to add the action to
				pActionList - points to the action list to add the action to
				pTextStory - points to the story containing pChar
	Outputs:	Constructs a MatrixFitToPathAction, initialises it and inserts in into the action list
	Returns:	An action code depending on the success of this function
	Purpose:	Called to constuct a MatrixFitToPathAction, set it up and and insert it into
				an operations action list
	Errors:		Checks the parameters
********************************************************************************************/
ActionCode MatrixFitToPathAction::Init(Operation* pOp, ActionList* pActionList, TextStory* pTextStory)
{
	// Check the parameters
	ERROR2IF(pOp == NULL, AC_FAIL, "The operation pointer is NULL");
	ERROR2IF(pActionList == NULL, AC_FAIL, "The action list pointer is NULL");
	ERROR2IF(pTextStory == NULL, AC_FAIL, "The TextStory pointer is NULL");

	// Create a new action and insert it in the action list
	UINT32 ActSize = sizeof(MatrixFitToPathAction);
	MatrixFitToPathAction* pNewAction = NULL;
	ActionCode Ac = Action::Init( pOp, pActionList, ActSize, CC_RUNTIME_CLASS(MatrixFitToPathAction), (Action**)&pNewAction);

	// Set the members of the new action
	if ((Ac == AC_OK) && (pNewAction != NULL))
		pNewAction->pStory = pTextStory;

	return Ac;
}


/********************************************************************************************
>	static BOOL MatrixFitToPathAction::DoMatrixRemoveFromPath(Operation* pOp,
															  ActionList* pActionList,
															  TextStory* pTextStory)
	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/5/95
	Inputs:		pOp - points to the operation to add the action to
				pActionList - points to the action list to add the action to
				pTextStory - points to the TextStory to reverse the path in
	Outputs:	Adds an action to the operations action list
	Returns:	TRUE/FALSE for success/failure
	Purpose:	Call this function when removing text from a path.  It sets the story matrix
				to the character matrix and sets the character matrix to unity.  It also
				inserts an action to reverse these changes.
	Errors:		Checks on parameters and structure of TextStory
********************************************************************************************/
BOOL MatrixFitToPathAction::DoMatrixRemoveFromPath(Operation* pOp, ActionList* pActionList, TextStory* pTextStory)
{
	// Check the parameters
	ERROR2IF(pOp == NULL, FALSE, "The operation pointer is NULL");
	ERROR2IF(pActionList == NULL, FALSE, "The action list pointer is NULL");
	ERROR2IF(pTextStory == NULL, FALSE, "The TextStory pointer is NULL");

	// Insert an action to restore matrices on undo 
	ActionCode ActCode;
	ActCode = MatrixFitToPathAction::Init(pOp, pActionList, pTextStory);

	// Now set the matrices
	if (ActCode != AC_FAIL)
		pTextStory->MatrixRemoveFromPath();

	return (ActCode != AC_FAIL);
}



////////////////////////////////////////////////////////////////////////////////////////////
// MatrixRemoveFromPathAction

/********************************************************************************************
>	MatrixRemoveFromPathAction::MatrixRemoveFromPathAction()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/5/95
	Purpose:	Constructor - initialises the member variables
********************************************************************************************/
MatrixRemoveFromPathAction::MatrixRemoveFromPathAction()
{
}


/********************************************************************************************
>	MatrixRemoveFromPathAction::~MatrixRemoveFromPathAction()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/5/95
	Purpose:	Destructor
********************************************************************************************/
MatrixRemoveFromPathAction::~MatrixRemoveFromPathAction()
{
}


/********************************************************************************************
>	virtual ActionCode MatrixRemoveFromPathAction::Execute()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/5/95
	Purpose:	Called during undo/redo to restore the TextStory matrices
				Inserts an opposite action into the opposite action list then changes the
				matrices
********************************************************************************************/
ActionCode MatrixRemoveFromPathAction::Execute()
{
	// Check the member variables
	ERROR2IF(pStory == NULL, AC_FAIL, "The TextStory pointer is NULL");
	ERROR2IF(pOperation == NULL, AC_FAIL, "The operation pointer is NULL(!)");

	// Insert an action to reverse the path again on redo
	ActionCode ActCode;
	ActCode = MatrixFitToPathAction::Init(pOperation, pOppositeActLst, pStory);

	// Now set the matrices
	if (ActCode != AC_FAIL)
		pStory->MatrixRemoveFromPath();

	return ActCode;
}


/********************************************************************************************
>	static ActionCode MatrixRemoveFromPathAction::Init(Operation* pOp,
												ActionList* pActionList,
												TextStory* pTextStory)
	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/5/95
	Inputs:		pOp - points to the operation to add the action to
				pActionList - points to the action list to add the action to
				pTextStory - points to the story containing pChar
	Outputs:	Constructs a MatrixRemoveFromPathAction, initialises it and inserts in into
				the action list
	Returns:	An action code depending on the success of this function
	Purpose:	Called to constuct a MatrixRemoveFromPathAction, set it up and and insert it
				into an operations action list
	Errors:		Checks the parameters
********************************************************************************************/
ActionCode MatrixRemoveFromPathAction::Init(Operation* pOp, ActionList* pActionList, TextStory* pTextStory)
{
	// Check the parameters
	ERROR2IF(pOp == NULL, AC_FAIL, "The operation pointer is NULL");
	ERROR2IF(pActionList == NULL, AC_FAIL, "The action list pointer is NULL");
	ERROR2IF(pTextStory == NULL, AC_FAIL, "The TextStory pointer is NULL");

	// Create a new action and insert it in the action list
	UINT32 ActSize = sizeof(MatrixRemoveFromPathAction);
	MatrixRemoveFromPathAction* pNewAction = NULL;
	ActionCode Ac = Action::Init( pOp, pActionList, ActSize, CC_RUNTIME_CLASS(MatrixRemoveFromPathAction), (Action**)&pNewAction);

	// Set the members of the new action
	if ((Ac == AC_OK) && (pNewAction != NULL))
		pNewAction->pStory = pTextStory;

	return Ac;
}


/********************************************************************************************
>	static BOOL MatrixRemoveFromPathAction::DoMatrixFitToPath(Operation* pOp,
															  ActionList* pActionList,
															  TextStory* pTextStory)
	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/5/95
	Inputs:		pOp - points to the operation to add the action to
				pActionList - points to the action list to add the action to
				pTextStory - points to the TextStory to reverse the path in
	Outputs:	Adds an action to the operations action list
	Returns:	TRUE/FALSE for success/failure
	Purpose:	Call this function when fitting text to a path.  It sets the character matrix
				to the story matrix and sets the story matrix to unity.  It also inserts an
				action to reverse these changes.
	Errors:		Checks on parameters and structure of TextStory
********************************************************************************************/
BOOL MatrixRemoveFromPathAction::DoMatrixFitToPath(Operation* pOp, ActionList* pActionList, TextStory* pTextStory)
{
	// Check the parameters
	ERROR2IF(pOp == NULL, FALSE, "The operation pointer is NULL");
	ERROR2IF(pActionList == NULL, FALSE, "The action list pointer is NULL");
	ERROR2IF(pTextStory == NULL, FALSE, "The TextStory pointer is NULL");

	// Insert an action to restore matrices on undo 
	ActionCode ActCode;
	ActCode = MatrixRemoveFromPathAction::Init(pOp, pActionList, pTextStory);

	// Now set the matrices
	if (ActCode != AC_FAIL)
		pTextStory->MatrixFitToPath();

	return (ActCode != AC_FAIL);
}
