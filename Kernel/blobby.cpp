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

// Implemetation of the following classes
/*
	OperationHistory : The operation history used to store undoable operations
	OpUndo           : The undo operation
	OpRedo           : The redo operation

	BlobbyOp         : Demonstration temporary operation which changes the colour of a
					   documents paper.

	UndoColAction    : BlobbyOp undo action
	RedoColAction    : BlobbyOp redo action
*/

/*
*/
#include "camtypes.h"
//#include "ops.h" - in camtypes.h [AUTOMATICALLY REMOVED]

DECLARE_SOURCE("$Revision$");

#include "blobby.h"
#include "ophist.h"
#include "lineattr.h"
//#include "resource.h"
#include "page.h"
//#include "docview.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "document.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "mario.h"
//#include "simon.h"
//#include "docview.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "blobtab.h"
#include <stdlib.h>

#include "bfxatom.h"
#include "tracergn.h"
#include "bubbleid.h"

CC_IMPLEMENT_DYNCREATE(BlobbyOp, UndoableOperation)
CC_IMPLEMENT_DYNCREATE(UndoColAction, Action)
CC_IMPLEMENT_DYNCREATE(RedoColAction, Action)
CC_IMPLEMENT_DYNCREATE(UndoAction2, Action)
CC_IMPLEMENT_DYNCREATE(UndoAction3, Action)
CC_IMPLEMENT_DYNCREATE(RedoAction2, Action)
CC_IMPLEMENT_DYNCREATE(RedoAction3, Action)
CC_IMPLEMENT_DYNCREATE(BlobbyDlg, DialogOp)
CC_IMPLEMENT_DYNCREATE(BlobbyBar, DialogOp)
CC_IMPLEMENT_DYNCREATE(BlobbyTabDlg, DialogTabOp)

#define new CAM_DEBUG_NEW

//-------------------------------------------------------------------------------------------
// Temporary Blobby operation to demonstrate the Operation history
// BlobbyOp is a simple undoable operation which changes the current colour of
// a document's pages. It is implemented using 3 pairs of Undo and Redo actions.
// Only the UndoColAction and RedoColAction perform any redo/undo, the other
// actions do nothing when executed. They exist only to test an operation's
// ability to cope with more than a single action on each action list.

/********************************************************************************************

>	BlobbyOp::BlobbyOp()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/8/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	BlobbyOp constructor
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BlobbyOp::BlobbyOp(): UndoableOperation()
{
}

/********************************************************************************************

>	BOOL BlobbyOp::Init()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/8/93
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the operation could be successfully initialised
				FALSE if no more memory could be allocated

	Purpose:	BlobyOp initialiser method
	Errors:		ERROR will be called if there was insufficient memory to allocate the
				operation.
	SeeAlso:	-

********************************************************************************************/

BOOL BlobbyOp::Init()
{
	return (UndoableOperation::RegisterOpDescriptor(
				 							0,
				 							_R(IDS_BLOBBY_OP),
											CC_RUNTIME_CLASS(BlobbyOp),
				 							OPTOKEN_BLOBBY,
				 							BlobbyOp::GetState,
				 							0,	/* help ID */
				 							_R(IDBBL_BLOBBYOP),
				 							0	/* bitmap ID */));
}

/********************************************************************************************

>	OpState	BlobbyOp::GetState(String_256*, OpDescriptor*)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/8/93
	Inputs:		-
	Outputs:	-
	Returns:	The state of the BlobbyOp
	Purpose:	For finding the BlobbyOp's state.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

OpState	BlobbyOp::GetState(String_256*, OpDescriptor*)
{
	OpState OpSt;
	return(OpSt);
}

/********************************************************************************************

>	void BlobbyOp::Do(OpDescriptor*)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/8/93
	Inputs:		OpDescriptor (unused)
	Outputs:	-
	Returns:	-
	Purpose:	Performs the BlobbyOp for the first time
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void BlobbyOp::Do(OpDescriptor*)
{
#if 1
	ActionCode ActCode;
	UndoColAction* UndoColAct;

	// Attempt to initialise the first action for blobby op
	if ((ActCode = UndoColAction::Init(this,
								         &UndoActions,
						 				 10,
						 				(Action**)(&UndoColAct))) != AC_FAIL) // 10 is bodge
		// If the action was successfully initialised i.e. AC_OK or AC_NO_RECORD then
		// record the current colour of a page in the undo action.
		UndoColAct->Color = Page::GetPageColour();

	if (ActCode != AC_FAIL)  // i.e. The ActCode = AC_OK or AC_NO_RECORD
	{
		DocColour Col ((INT32)(rand()%255),
    				 					(INT32)(rand()%255),
	   				  					(INT32)(rand()%255));
		// Give the page a random colour
		Page::SetPageColour(Col);
	// Redraw the document to see the change in the page's colour
		(DocView::GetSelected())->ForceRedraw();

		//if (IsUserName("Simon"))
		// TRACE( _T("Created undo action 1\n"));

	 	UndoAction2* UndoAct2;

	 	// Try to init a second undo action (this one will perform nothing when executed)
	 	if  (UndoAction2::Init(this,
							  &UndoActions,
						 	  10,
						 	  (Action**)(&UndoAct2)) != AC_FAIL)
		{
			// The Undo action was successfully initialised
			//if (IsUserName("Simon"))
			//{
			//	TRACE( _T("Created undo action 2\n"));
			//}
			UndoAction3* UndoAct3;

			// Try to Init a third undo action (again this will perform nothing when executed)
			UndoAction3::Init(this,
								 &UndoActions,
								 10,
								 (Action**)(&UndoAct3));
		}
    }

	#ifdef _DEBUG
	// Remember our working document
	Document *pDoc = GetWorkingDoc();
	#endif

    // End the operation
	End();

	#ifdef _DEBUG
	// Ensure that the size of the operation history is valid
	pDoc->GetOpHistory().DebugSizeCheck();
	#endif
#else
	//BitmapEffectAtom::Test2(this);
	TraceRegion::Test(this);
	// End the operation
	End();

#endif
}

/********************************************************************************************

>	UndoColAction::UndoColAction()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/8/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	UndoColAction constructor
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

UndoColAction::UndoColAction():Action()
{
}

/********************************************************************************************

>	ActionCode UndoColAction::Init(Operation* const pOp,
							   	   ActionList* pActionList,
							   	   UINT32 ActionSize,
							  	   Action** NewAction)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/8/93

	Inputs:		pOp: The operation to which the action should be added

				pActionList: The action list in the operation object

				Size:  		 The size of the action in bytes. This should be the total
							 size of the action (including any objects pointed to by the
							 action).

 	Outputs:    NewAction:   A pointer to the action if it could be allocated.

	Returns:	AC_FAIL:     There was not enough room in the operation history for the
							 action and the user did not wish to continue. Usually
							 End() should be called in this situation.

				AC_NORECORD: There was not enough room in the operation history for
							 the action, but the user requested that he wished to
							 continue without undo.

				AC_OK      : The action was successfully initialised and added to the
							 operation.


	Purpose:	To check that there is sufficient room for the action in the operation
				history, and if there is, then to add the action to the operations
				action list.

				The function simply calls the Action::Init function passing the runtime class
				of an UndoColAction.
	Errors:		-
	SeeAlso:	Action::Init

********************************************************************************************/

ActionCode UndoColAction::Init(Operation* const pOp,
							   ActionList* pActionList,
							   UINT32 ActionSize,
							   Action** NewAction)
{

	return(Action::Init(pOp,
						pActionList,
						ActionSize,
						CC_RUNTIME_CLASS(UndoColAction),
						NewAction));
}

/********************************************************************************************

>	ActionCode UndoColAction::Execute()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/8/93
	Inputs:		-
	Outputs:	ActionCode indicating if the action was successfully executed or not
	Returns:	-
	Purpose:	Executes the UndoColAction setting the paper colour to the colour it was
				prior to the operation being done or redone.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

ActionCode UndoColAction::Execute()
{
	RedoColAction* RedoColAct;
	ActionCode ActCode;
	// Attempt to initialise the action
	if ((ActCode = RedoColAction::Init(pOperation,
						 			   pOperation->GetRedoActionList(),
						 			   10,
						 			   ((Action**)&RedoColAct))) != AC_FAIL) // 10 is bodge
		if (RedoColAct != NULL)	// Check we are not unwinding
			RedoColAct->Color = Page::GetPageColour();

	if (ActCode != AC_FAIL)
	{
		// The action did not fail to initialise
		Page::SetPageColour(Color);               // Set the paper colour to the colour it was
												  // prior to the operation being done or redone
		(DocView::GetSelected())->ForceRedraw();   // Show the new colour
	}
	return (ActCode);
}


/********************************************************************************************

>	RedoColAction::RedoColAction()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/8/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	RedoColAction constructor
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

RedoColAction::RedoColAction():Action()
{
}

/********************************************************************************************

>	ActionCode RedoColAction::Init(Operation* const pOp,
							   	   ActionList* pActionList,
							   	   UINT32 ActionSize,
							  	   Action** NewAction)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/8/93

	Inputs:		pOp: The operation to which the action should be added

				pActionList: The action list in the operation object

				Size:  		 The size of the action in bytes. This should be the total
							 size of the action (including any objects pointed to by the
							 action).

 	Outputs:    NewAction:   A pointer to the action if it could be allocated.

	Returns:	AC_FAIL:     There was not enough room in the operation history for the
							 action and the user did not wish to continue. Usually
							 End() should be called in this situation.

				AC_NORECORD: There was not enough room in the operation history for
							 the action, but the user requested that he wished to
							 continue without undo.

				AC_OK      : The action was successfully initialised and added to the
							 operation.


	Purpose:	To check that there is sufficient room for the action in the operation
				history, and if there is, then to add the action to the operations
				action list.

				The function simply calls the Action::Init function passing the runtime class
				of a RedoColAction.
	Errors:		-
	SeeAlso:	Action::Init

********************************************************************************************/


ActionCode RedoColAction::Init(Operation* const pOp,
							   ActionList* pActionList,
							   UINT32 ActionSize,
							   Action**	NewAction)
{
	return(Action::Init(pOp,
						pActionList,
						ActionSize,
						CC_RUNTIME_CLASS(RedoColAction),
						NewAction));
}

/********************************************************************************************

>	ActionCode RedoColAction::Execute()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/8/93
	Inputs:		-
	Outputs:	ActionCode indicating if the action was successfully executed or not
	Returns:	-
	Purpose:	Executes the RedoColAction setting the paper colour to the colour it was
				prior to the operation being undone.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/


ActionCode RedoColAction::Execute()
{
    UndoColAction* UndoColAct;
    ActionCode ActCode;
    // Attempt to initialise the action
	if ((ActCode = UndoColAction::Init(pOperation,
									   pOperation->GetUndoActionList(),
									   10,
									   ((Action**)&UndoColAct))) == AC_OK) // 10 is bodge
		if (UndoColAct != NULL)// Check we are not unwinding
			UndoColAct->Color = Page::GetPageColour();

	if (ActCode != AC_FAIL)
	{
		// The action was successfully initialised
		Page::SetPageColour(Color);                 // Set the page colour to the colour it was
													// prior to the operation being undone.
		(DocView::GetSelected())->ForceRedraw(); 	// Show the colour change
	}
	return (ActCode);
}

// Redo Action 2 & 3 do nothing, they exist to test an operation with more than a single action

/********************************************************************************************

>	RedoAction2::RedoAction2()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/8/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	RedoAction2 constructor
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

RedoAction2::RedoAction2():Action()
{
}


/********************************************************************************************

>	ActionCode RedoAction2::Init(Operation* const pOp,
							   	   ActionList* pActionList,
							   	   UINT32 ActionSize,
							  	   Action** NewAction)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/8/93

	Inputs:		pOp: The operation to which the action should be added

				pActionList: The action list in the operation object

				Size:  		 The size of the action in bytes. This should be the total
							 size of the action (including any objects pointed to by the
							 action).

 	Outputs:    NewAction:   A pointer to the action if it could be allocated.

	Returns:	AC_FAIL:     There was not enough room in the operation history for the
							 action and the user did not wish to continue. Usually
							 End() should be called in this situation.

				AC_NORECORD: There was not enough room in the operation history for
							 the action, but the user requested that he wished to
							 continue without undo.

				AC_OK      : The action was successfully initialised and added to the
							 operation.


	Purpose:	To check that there is sufficient room for the action in the operation
				history, and if there is, then to add the action to the operations
				action list.

				The function simply calls the Action::Init function passing the runtime class
				of a RedoAction2.
	Errors:		-
	SeeAlso:	Action::Init

********************************************************************************************/


ActionCode RedoAction2::Init(Operation* const pOp,
							   ActionList* pActionList,
							   UINT32 ActionSize,
							   Action**	NewAction)
{
	return(Action::Init(pOp,
						pActionList,
						ActionSize,
						CC_RUNTIME_CLASS(RedoAction2),
						NewAction));
}

/********************************************************************************************

>	ActionCode RedoAction2::Execute()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/8/93
	Inputs:		-
	Outputs:	ActionCode indicating if the action was successfully executed or not
	Returns:	-
	Purpose:	To execute RedoAction2. Does nothing except spawn an UndoAction2 action.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

ActionCode RedoAction2::Execute()
{
    UndoAction2* UndoAct;
    ActionCode ActCode;
	ActCode = UndoAction2::Init(pOperation,
								pOperation->GetUndoActionList(),
								10,
								((Action**)&UndoAct)); // 10 is bodge

	if (ActCode != AC_FAIL)
	{
		//if (IsUserName("Simon"))
		//	TRACE( _T("Executing REDO Action 2\n"));
	}
	return (ActCode);
}


/********************************************************************************************

>	RedoAction3::RedoAction3():Action()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/8/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	RedoAction3 constructor
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

RedoAction3::RedoAction3():Action()
{
}

/********************************************************************************************

>	ActionCode RedoAction3::Init(Operation* const pOp,
							   	   ActionList* pActionList,
							   	   UINT32 ActionSize,
							  	   Action** NewAction)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/8/93

	Inputs:		pOp: The operation to which the action should be added

				pActionList: The action list in the operation object

				Size:  		 The size of the action in bytes. This should be the total
							 size of the action (including any objects pointed to by the
							 action).

 	Outputs:    NewAction:   A pointer to the action if it could be allocated.

	Returns:	AC_FAIL:     There was not enough room in the operation history for the
							 action and the user did not wish to continue. Usually
							 End() should be called in this situation.

				AC_NORECORD: There was not enough room in the operation history for
							 the action, but the user requested that he wished to
							 continue without undo.

				AC_OK      : The action was successfully initialised and added to the
							 operation.


	Purpose:	To check that there is sufficient room for the action in the operation
				history, and if there is, then to add the action to the operations
				action list.

				The function simply calls the Action::Init function passing the runtime class
				of a RedoAction3.
	Errors:		-
	SeeAlso:	Action::Init

********************************************************************************************/


ActionCode RedoAction3::Init(Operation* const pOp,
							   ActionList* pActionList,
							   UINT32 ActionSize,
							   Action**	NewAction)
{
	return(Action::Init(pOp,
						pActionList,
						ActionSize,
						CC_RUNTIME_CLASS(RedoAction3),
						NewAction));
}


/********************************************************************************************

>	ActionCode RedoAction3::Execute()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/8/93
	Inputs:		-
	Outputs:	-
	Returns:	ActionCode indicating if the action was successfully executed or not
	Purpose:	To execute RedoAction3. Does nothing except spawn an UndoAction3 action.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/


ActionCode RedoAction3::Execute()
{
    UndoAction3* UndoAct;
    ActionCode ActCode;
	ActCode = UndoAction3::Init(pOperation,
								pOperation->GetUndoActionList(),
								10,
								((Action**)&UndoAct)); // 10 is bodge

	if (ActCode != AC_FAIL)
	{
		//if (IsUserName("Simon"))
		//	TRACE( _T("Executing REDO Action 3\n"));
	}
	return (ActCode);
}

// Likewise undo action 2 & 3 are also for debugging and do nothing

/********************************************************************************************

>	UndoAction2::UndoAction2():Action()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/8/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	UndoAction2 constructor
	Errors:		-
	SeeAlso:	-

********************************************************************************************/


UndoAction2::UndoAction2():Action()
{
}

/********************************************************************************************

>	ActionCode UndoAction2::Init(Operation* const pOp,
							   	   ActionList* pActionList,
							   	   UINT32 ActionSize,
							  	   Action** NewAction)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/8/93

	Inputs:		pOp: The operation to which the action should be added

				pActionList: The action list in the operation object

				Size:  		 The size of the action in bytes. This should be the total
							 size of the action (including any objects pointed to by the
							 action).

 	Outputs:    NewAction:   A pointer to the action if it could be allocated.

	Returns:	AC_FAIL:     There was not enough room in the operation history for the
							 action and the user did not wish to continue. Usually
							 End() should be called in this situation.

				AC_NORECORD: There was not enough room in the operation history for
							 the action, but the user requested that he wished to
							 continue without undo.

				AC_OK      : The action was successfully initialised and added to the
							 operation.


	Purpose:	To check that there is sufficient room for the action in the operation
				history, and if there is, then to add the action to the operations
				action list.

				The function simply calls the Action::Init function passing the runtime class
				of an UndoAction2.
	Errors:		-
	SeeAlso:	Action::Init

********************************************************************************************/


ActionCode UndoAction2::Init(Operation* const pOp,
							   ActionList* pActionList,
							   UINT32 ActionSize,
							   Action**	NewAction)
{
	return(Action::Init(pOp,
						pActionList,
						ActionSize,
						CC_RUNTIME_CLASS(UndoAction2),
						NewAction));
}

/********************************************************************************************

>	ActionCode UndoAction2::Execute()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/8/93
	Inputs:		-
	Outputs:	-
	Returns:	ActionCode indicating if the action was successfully executed or not
	Purpose:	To execute UndoAction2. Does nothing except spawn a RedoAction2 action.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/


ActionCode UndoAction2::Execute()
{
    RedoAction2* RedoAct;
    ActionCode ActCode;
	ActCode = RedoAction2::Init(pOperation,
								pOperation->GetRedoActionList(),
								10,
								((Action**)&RedoAct)); // 10 is bodge

	if (ActCode != AC_FAIL)
	{
		//if (IsUserName("Simon"))
		//	TRACE( _T("Executing Undo Action 2\n"));
	}
	return (ActCode);
}


/********************************************************************************************

>	UndoAction3::UndoAction3():Action()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/8/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	UndoAction3 constructor
	Errors:		-
	SeeAlso:	-

********************************************************************************************/


UndoAction3::UndoAction3():Action()
{
}

/********************************************************************************************

>	ActionCode UndoAction3::Init(Operation* const pOp,
							   	   ActionList* pActionList,
							   	   UINT32 ActionSize,
							  	   Action** NewAction)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/8/93

	Inputs:		pOp: The operation to which the action should be added

				pActionList: The action list in the operation object

				Size:  		 The size of the action in bytes. This should be the total
							 size of the action (including any objects pointed to by the
							 action).

 	Outputs:    NewAction:   A pointer to the action if it could be allocated.

	Returns:	AC_FAIL:     There was not enough room in the operation history for the
							 action and the user did not wish to continue. Usually
							 End() should be called in this situation.

				AC_NORECORD: There was not enough room in the operation history for
							 the action, but the user requested that he wished to
							 continue without undo.

				AC_OK      : The action was successfully initialised and added to the
							 operation.


	Purpose:	To check that there is sufficient room for the action in the operation
				history, and if there is, then to add the action to the operations
				action list.

				The function simply calls the Action::Init function passing the runtime class
				of an UndoAction3.
	Errors:		-
	SeeAlso:	Action::Init

********************************************************************************************/


ActionCode UndoAction3::Init(Operation* const pOp,
							   ActionList* pActionList,
							   UINT32 ActionSize,
							   Action**	NewAction)
{
	return(Action::Init(pOp,
						pActionList,
						ActionSize,
						CC_RUNTIME_CLASS(UndoAction3),
						NewAction));
}

/********************************************************************************************

>	ActionCode UndoAction3::Execute()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/8/93
	Inputs:		-
	Outputs:	-
	Returns:	ActionCode indicating if the action was successfully executed or not
	Purpose:	To execute UndoAction3. Does nothing except spawn a RedoAction3 action.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/


ActionCode UndoAction3::Execute()
{
    RedoAction3* RedoAct;
    ActionCode ActCode;
	ActCode = RedoAction3::Init(pOperation,
								pOperation->GetRedoActionList(),
								10,
								((Action**)&RedoAct)); // 10 is bodge

	if (ActCode != AC_FAIL)
	{
		//if (IsUserName("Simon"))
		//	TRACE( _T("Executing Undo Action 3\n"));
	}
	return (ActCode);
}

//------------------------------------------------------------------------------------------
// Demo Dialog

const CDlgMode BlobbyDlg::Mode = MODELESS;// Mode of the dialog

const UINT32 BlobbyDlg::IDD = _R(IDD_BLOBBYDLG);

BlobbyDlg::BlobbyDlg(): DialogOp(BlobbyDlg::IDD, BlobbyDlg::Mode)
{
}

// -----------------------------------------------------------------------------------------

void InitValues(BlobbyDlg* pBlob)
{

	String_256 Wibble("WIBBLE");
	String_256 Wobble("WOBBLE");
	pBlob->SetStringGadgetValue(_R(IDC_BUTTON1),
			  			   		 Wibble);
	pBlob->SetStringGadgetValue(_R(IDC_BUTTON2),
				   		 		Wobble);

	// Fill lists
	String_256 StrList[5*5];
	char a, b;
	INT32 i=0;

	for(a='e'; a>='a';a--)
	{
		for (b='a'; b<='e';b++)
		{
			((TCHAR*)StrList[i])[0] = a;
			((TCHAR*)StrList[i])[1] = b;
			((TCHAR*)StrList[i])[2] = 0;
			pBlob->SetStringGadgetValue(_R(IDC_LIST1), StrList[i],FALSE,i);
			i++;
		}
	}
	// Scroll bars
	pBlob->SetLongGadgetValue(_R(IDC_SCROLLBAR1), 60, 10);
	pBlob->SetLongGadgetValue(_R(IDC_SCROLLBAR2), 60, 10);
	BOOL Valid;
	MILLIPOINT Value = (MILLIPOINT)(pBlob->GetLongGadgetValue(_R(IDC_SCROLLBAR1),10,100,0,&Valid)*72000);
	pBlob->SetUnitGadgetValue(_R(IDC_XVAL), INCHES, Value);
	Value = (MILLIPOINT)(pBlob->GetLongGadgetValue(_R(IDC_SCROLLBAR2),10,100,0,&Valid)*72000);
	pBlob->SetUnitGadgetValue(_R(IDC_YVAL), INCHES, Value);

	// Combos
	pBlob->DefaultUnit = INCHES;   	// Default unit
	pBlob->ConvUnit = MILLIMETRES;  // Conversion unit

	// RadioBoxes
	pBlob->SetLongGadgetValue(_R(IDC_RADIO11), TRUE);
	pBlob->SetLongGadgetValue(_R(IDC_RADIO12), FALSE);
	pBlob->SetLongGadgetValue(_R(IDC_RADIO13), FALSE);

	pBlob->SetLongGadgetValue(_R(IDC_RADIO21), FALSE);
	pBlob->SetLongGadgetValue(_R(IDC_RADIO22), FALSE);
	pBlob->SetLongGadgetValue(_R(IDC_RADIO23), TRUE);
}

// -----------------------------------------------------------------------------------------

void HideGadgets(BlobbyDlg* pBlob)
{
	BOOL Valid;
	BOOL Hide = (BOOL)(pBlob->GetLongGadgetValue(_R(IDC_CHECK2),0,1,0,&Valid));
	pBlob->HideGadget(_R(IDC_STATIC1), Hide);
	pBlob->HideGadget(_R(IDC_STATIC2), Hide);
	pBlob->HideGadget(_R(IDC_STATIC3), Hide);
	pBlob->HideGadget(_R(IDC_STATIC4), Hide);
	pBlob->HideGadget(_R(IDC_BUTTON1), Hide);
	pBlob->HideGadget(_R(IDC_BUTTON2), Hide);
	pBlob->HideGadget(_R(IDC_SCROLLBAR1), Hide);
	pBlob->HideGadget(_R(IDC_SCROLLBAR2), Hide);
	pBlob->HideGadget(_R(IDOK), Hide);
	pBlob->HideGadget(_R(IDCANCEL), Hide);
	pBlob->HideGadget(_R(IDC_CHECK1), Hide);
	pBlob->HideGadget(_R(IDC_STATIC6), Hide);
	pBlob->HideGadget(_R(IDC_LIST1), Hide);
	pBlob->HideGadget(_R(IDC_LIST2), Hide);

	pBlob->HideGadget(_R(IDC_RADIO11), Hide);
	pBlob->HideGadget(_R(IDC_RADIO12), Hide);
	pBlob->HideGadget(_R(IDC_RADIO13), Hide);
	pBlob->HideGadget(_R(IDC_RADIO21), Hide);
	pBlob->HideGadget(_R(IDC_RADIO22), Hide);
	pBlob->HideGadget(_R(IDC_RADIO23), Hide);

	pBlob->HideGadget(_R(IDC_EDIT1), Hide);
	pBlob->HideGadget(_R(IDC_EDIT2), Hide);
	pBlob->HideGadget(_R(IDC_EDIT3), Hide);

	pBlob->HideGadget(_R(IDC_STCONV), Hide);
	pBlob->HideGadget(_R(IDC_STDEF), Hide);
	pBlob->HideGadget(_R(IDC_STCT), Hide);
	pBlob->HideGadget(_R(IDC_UNITVAL), Hide);
	pBlob->HideGadget(_R(IDC_CONVVAL), Hide);
	pBlob->HideGadget(_R(IDC_STUV), Hide);
	pBlob->HideGadget(_R(IDC_CONV), Hide);
	pBlob->HideGadget(_R(IDC_BUTTON3), Hide);
	pBlob->HideGadget(_R(IDC_STRD), Hide);

	pBlob->HideGadget(_R(IDC_STRD), Hide);
	pBlob->HideGadget(_R(IDC_YVAL), Hide);
	pBlob->HideGadget(_R(IDC_XVAL), Hide);
	pBlob->HideGadget(_R(IDC_STDEF), Hide);
	pBlob->HideGadget(_R(IDC_COMBO3), Hide);
	pBlob->HideGadget(_R(IDC_COMBO4), Hide);
	pBlob->HideGadget(_R(IDC_STCT), Hide);
	pBlob->HideGadget(_R(IDC_STUV), Hide);

	pBlob->HideGadget(_R(IDC_VLD), Hide);
	pBlob->HideGadget(_R(ID_STINT), Hide);
	pBlob->HideGadget(_R(IDC_STRL), Hide);
	pBlob->HideGadget(_R(IDC_STXYZ), Hide);
	pBlob->HideGadget(_R(IDC_EDIT1), Hide);
	pBlob->HideGadget(_R(IDC_EDIT2), Hide);
	pBlob->HideGadget(_R(IDC_EDIT3), Hide);
}

// -----------------------------------------------------------------------------------------

void EnableGadgets(BlobbyDlg* pBlob)
{
	BOOL Valid;
	BOOL Enable = !((BOOL)(pBlob->GetLongGadgetValue(_R(IDC_CHECK1), 0,1,0,&Valid)));
	pBlob->EnableGadget(_R(IDC_STATIC1), Enable);
	pBlob->EnableGadget(_R(IDC_STATIC2), Enable);
	pBlob->EnableGadget(_R(IDC_STATIC3), Enable);
	pBlob->EnableGadget(_R(IDC_STATIC4), Enable);
	pBlob->EnableGadget(_R(IDC_BUTTON1), Enable);
	pBlob->EnableGadget(_R(IDC_BUTTON2), Enable);
	pBlob->EnableGadget(_R(IDC_SCROLLBAR1), Enable);
	pBlob->EnableGadget(_R(IDC_SCROLLBAR2), Enable);
	pBlob->EnableGadget(_R(IDOK), Enable);
	pBlob->EnableGadget(_R(IDCANCEL), Enable);
	pBlob->EnableGadget(_R(IDC_CHECK2), Enable);
	pBlob->EnableGadget(_R(IDC_STATIC6), Enable);
	pBlob->EnableGadget(_R(IDC_LIST1), Enable);
	pBlob->EnableGadget(_R(IDC_LIST2), Enable);

	pBlob->EnableGadget(_R(IDC_RADIO11), Enable);
	pBlob->EnableGadget(_R(IDC_RADIO12), Enable);
	pBlob->EnableGadget(_R(IDC_RADIO13), Enable);
	pBlob->EnableGadget(_R(IDC_RADIO21), Enable);
	pBlob->EnableGadget(_R(IDC_RADIO22), Enable);
	pBlob->EnableGadget(_R(IDC_RADIO23), Enable);

	pBlob->EnableGadget(_R(IDC_EDIT1), Enable);
	pBlob->EnableGadget(_R(IDC_EDIT2), Enable);
	pBlob->EnableGadget(_R(IDC_EDIT3), Enable);

	pBlob->EnableGadget(_R(IDC_STCONV), Enable);
	pBlob->EnableGadget(_R(IDC_STDEF), Enable);
	pBlob->EnableGadget(_R(IDC_STCT), Enable);
	pBlob->EnableGadget(_R(IDC_UNITVAL), Enable);
	pBlob->EnableGadget(_R(IDC_CONVVAL), Enable);
	pBlob->EnableGadget(_R(IDC_STUV), Enable);
	pBlob->EnableGadget(_R(IDC_CONV), Enable);
	pBlob->EnableGadget(_R(IDC_BUTTON3), Enable);
	pBlob->EnableGadget(_R(IDC_STRD), Enable);

	pBlob->EnableGadget(_R(IDC_STRD), Enable);
	pBlob->EnableGadget(_R(IDC_YVAL), Enable);
	pBlob->EnableGadget(_R(IDC_XVAL), Enable);
	pBlob->EnableGadget(_R(IDC_STDEF), Enable);
	pBlob->EnableGadget(_R(IDC_COMBO3), Enable);
	pBlob->EnableGadget(_R(IDC_COMBO4), Enable);
	pBlob->EnableGadget(_R(IDC_STCT), Enable);
	pBlob->EnableGadget(_R(IDC_STUV), Enable);

	pBlob->EnableGadget(_R(IDC_VLD), Enable);
	pBlob->EnableGadget(_R(ID_STINT), Enable);
	pBlob->EnableGadget(_R(IDC_STRL), Enable);
	pBlob->EnableGadget(_R(IDC_STXYZ), Enable);
	pBlob->EnableGadget(_R(IDC_EDIT1), Enable);
	pBlob->EnableGadget(_R(IDC_EDIT2), Enable);
	pBlob->EnableGadget(_R(IDC_EDIT3), Enable);
}

// -----------------------------------------------------------------------------------------

MsgResult BlobbyDlg::Message( Msg* Message)
{

	if (IS_OUR_DIALOG_MSG(Message))
	{
		DialogMsg* Msg = (DialogMsg*)Message;
		// Handle ok button
		if ((Msg->DlgMsg == DIM_COMMIT) || (Msg->DlgMsg == DIM_CANCEL))
		{

#if 0
BOOL Valid=0;
INT32 Value = (INT32) GetUnitGadgetValue(_R(IDC_UNITVAL),DefaultUnit,-10000000,10000000,0,&Valid);
BitmapEffectAtom::Test(Value);
#endif

			Close(); // Hide the dialog
			End();
		}
		else if (Msg->DlgMsg == DIM_SOFT_COMMIT)
		{
			InitValues(this);
		}

		// wibblewobble
		if (((Msg->GadgetID == _R(IDC_BUTTON1)) || (Msg->GadgetID == _R(IDC_BUTTON2)))
			 && (Msg->DlgMsg == DIM_LFT_BN_CLICKED))
		{
			BOOL Valid =0;
			String_256 Temp = GetStringGadgetValue(_R(IDC_BUTTON1), &Valid);
		    String_256 Temp2 = GetStringGadgetValue(_R(IDC_BUTTON2), &Valid);
			SetStringGadgetValue(_R(IDC_BUTTON1),
							   	 Temp2);

			SetStringGadgetValue(_R(IDC_BUTTON2),
							   	 Temp);
			SetTitlebarName(&Temp); // Returns a BOOL !!!
		}
		// Hide and enable
		else if ((Msg->GadgetID == _R(IDC_CHECK2)) && (Msg->DlgMsg == DIM_LFT_BN_CLICKED))
		{
			HideGadgets(this);
		}
		else if ((Msg->GadgetID == _R(IDC_CHECK1)) && (Msg->DlgMsg == DIM_LFT_BN_CLICKED))
		{
			EnableGadgets(this);
		}
		// List sorting
		else if ((Msg->GadgetID == _R(IDC_LIST1)) && (Msg->DlgMsg == DIM_SELECTION_CHANGED_COMMIT))
		{
			// Add the selected item to the sorted list
			BOOL Valid =0;
			String_256 MoveStr = GetStringGadgetValue(_R(IDC_LIST1), &Valid);
			SetStringGadgetValue(_R(IDC_LIST2), MoveStr);
			// Delete the selected item
			WORD Index;
			GetValueIndex(_R(IDC_LIST1), &Index);
			DeleteValue(_R(IDC_LIST1), FALSE, Index);
		}
		else if ((Msg->GadgetID == _R(IDC_LIST2)) && (Msg->DlgMsg == DIM_SELECTION_CHANGED))
		{
			// Add the selected item to LIST1
			BOOL Valid =0;
			String_256 MoveStr = GetStringGadgetValue(_R(IDC_LIST2), &Valid);
			SetStringGadgetValue(_R(IDC_LIST1), MoveStr);
			// Delete the selected item
			WORD Index;
			GetValueIndex(_R(IDC_LIST2), &Index);
			DeleteValue(_R(IDC_LIST2), FALSE, Index);
		}
		// Scrollbars
		else if (Msg->GadgetID == _R(IDC_SCROLLBAR1))
		{
			BOOL Valid=0;
			INT32 Value = (MILLIPOINT)(GetLongGadgetValue(_R(IDC_SCROLLBAR1),0,100,0,&Valid)*72000);
			SetUnitGadgetValue(_R(IDC_XVAL), INCHES, Value);
		}
		else if (Msg->GadgetID == _R(IDC_SCROLLBAR2))
		{
			BOOL Valid=0;
			INT32 Value = (MILLIPOINT)(GetLongGadgetValue(_R(IDC_SCROLLBAR2),0,100,0,&Valid)*72000);
			SetUnitGadgetValue(_R(IDC_YVAL), INCHES, Value);
		}
		// Unit Conversion
		else if ((Msg->GadgetID == _R(IDC_COMBO3)) && (Msg->DlgMsg == DIM_SELECTION_CHANGED))
		{
//			BOOL Valid=0;
			INT32 Index;
			GetValueIndex(_R(IDC_COMBO3),&Index);
			if ((Index >= 0) && (Index <= 8))
				DefaultUnit = UnitList[Index];
		}
		else if ((Msg->GadgetID == _R(IDC_COMBO4)) && (Msg->DlgMsg == DIM_SELECTION_CHANGED))
		{
//			BOOL Valid=0;
			INT32 Index;
			GetValueIndex(_R(IDC_COMBO4),&Index);
			if ((Index >= 0) && (Index <= 8))
				ConvUnit = UnitList[Index];
		}
		else if ((Msg->GadgetID == _R(IDC_BUTTON3)) && (Msg->DlgMsg == DIM_LFT_BN_CLICKED))
		{
			BOOL Valid=0;
			MILLIPOINT Value = GetUnitGadgetValue(_R(IDC_UNITVAL),DefaultUnit,-10000000,10000000,0,&Valid);
			SetUnitGadgetValue(_R(IDC_CONVVAL), ConvUnit, Value);
		}
		// Radio buttons
		else if ((Msg->GadgetID == _R(IDC_RADIO11)) && (Msg->DlgMsg == DIM_LFT_BN_CLICKED))
		{
			SetLongGadgetValue(_R(IDC_RADIO23), TRUE);
			SetLongGadgetValue(_R(IDC_RADIO21), FALSE);
			SetLongGadgetValue(_R(IDC_RADIO22), FALSE);

		}
		else if ((Msg->GadgetID == _R(IDC_RADIO12)) && (Msg->DlgMsg == DIM_LFT_BN_CLICKED))
		{
			SetLongGadgetValue(_R(IDC_RADIO22), TRUE);
			SetLongGadgetValue(_R(IDC_RADIO21), FALSE);
			SetLongGadgetValue(_R(IDC_RADIO23), FALSE);

		}
		else if ((Msg->GadgetID == _R(IDC_RADIO13)) && (Msg->DlgMsg == DIM_LFT_BN_CLICKED))
		{
			SetLongGadgetValue(_R(IDC_RADIO21), TRUE);
			SetLongGadgetValue(_R(IDC_RADIO22), FALSE);
			SetLongGadgetValue(_R(IDC_RADIO23), FALSE);
		}
		else if ((Msg->GadgetID == _R(IDC_RADIO21)) && (Msg->DlgMsg == DIM_LFT_BN_CLICKED))
		{
			SetLongGadgetValue(_R(IDC_RADIO13), TRUE);
			SetLongGadgetValue(_R(IDC_RADIO11), FALSE);
			SetLongGadgetValue(_R(IDC_RADIO12), FALSE);

		}
		else if ((Msg->GadgetID == _R(IDC_RADIO22)) && (Msg->DlgMsg == DIM_LFT_BN_CLICKED))
		{
			SetLongGadgetValue(_R(IDC_RADIO12), TRUE);
			SetLongGadgetValue(_R(IDC_RADIO11), FALSE);
			SetLongGadgetValue(_R(IDC_RADIO13), FALSE);

		}
		else if ((Msg->GadgetID == _R(IDC_RADIO23)) && (Msg->DlgMsg == DIM_LFT_BN_CLICKED))
		{
			SetLongGadgetValue(_R(IDC_RADIO11), TRUE);
			SetLongGadgetValue(_R(IDC_RADIO12), FALSE);
			SetLongGadgetValue(_R(IDC_RADIO13), FALSE);
		}

		return (DLG_EAT_IF_HUNGRY(Msg));
	}
	return OK;
}

// -----------------------------------------------------------------------------------------

OpState	BlobbyDlg::GetState(String_256*, OpDescriptor*)
{
	OpState OpSt;
	return(OpSt);
}

// -----------------------------------------------------------------------------------------

BOOL BlobbyDlg::Init()
{

return (RegisterOpDescriptor(
 								0,
 								_R(IDS_BLOBBYDLG),
								CC_RUNTIME_CLASS(BlobbyDlg),
 								OPTOKEN_BLOBBYDLG,
 								BlobbyDlg::GetState,
 								0,	/* help ID */
 								_R(IDBBL_BLOBBYDLG),
 								0	/* bitmap ID */
								));

}

// -----------------------------------------------------------------------------------------

BOOL BlobbyDlg::Create()
{
	if (DialogOp::Create())
	{
		SetGadgetRange(_R(IDC_SCROLLBAR1), /*10*/ 0, 100, 10);
		SetGadgetRange(_R(IDC_SCROLLBAR2), /*10*/ 0, 100, 10);

		//DualFunctionButton(IDOK);
		SetEditGadgetType(_R(IDC_UNITVAL), UNIT_NUMERIC);
		SetEditGadgetType(_R(IDC_EDIT1), INT_NUMERIC);
		SetEditGadgetType(_R(IDC_EDIT2), REAL_NUMERIC);
		SetEditGadgetType(_R(IDC_EDIT3), _R(IDS_BLOBBYXYZ));

		UnitStrList[0] = String(TEXT("Millimetres"));
		UnitStrList[1] = String(TEXT("Centimetres"));
		UnitStrList[2] = String(TEXT("Metres"));
		UnitStrList[3] = String(TEXT("Inches"));
		UnitStrList[4] = String(TEXT("Feet"));
		UnitStrList[5] = String(TEXT("Yards"));
		UnitStrList[6] = String(TEXT("Points"));
		UnitStrList[7] = String(TEXT("Picas"));
		UnitStrList[8] = String(TEXT("Millipoints"));

        UnitList[0] = MILLIMETRES;
        UnitList[1] = CENTIMETRES;
		UnitList[2] = METRES;
		UnitList[3] = INCHES;
		UnitList[4] = FEET;
		UnitList[5] = YARDS;
		UnitList[6] = COMP_POINTS;
		UnitList[7] = PICAS;
		UnitList[8] = MILLIPOINTS;

		for (INT32 i=0; i<9; i++)
		{
			SetStringGadgetValue(_R(IDC_COMBO3), UnitStrList[i],FALSE,i);
			SetStringGadgetValue(_R(IDC_COMBO4), UnitStrList[i],FALSE,i);
		}


		InitValues(this);

		return TRUE;
    }
    else return FALSE;
}

// -----------------------------------------------------------------------------------------

void BlobbyDlg::Do(OpDescriptor*)
{
	Create();
	Open();
}


// -----------------------------------------------------------------------------------------
// BlobbyTabDialog methods, BlobbyTabDialog is an example implementation of a tabbed dialog


const CDlgMode BlobbyTabDlg::Mode = MODAL; // Mode of the dialog
const UINT32 BlobbyTabDlg::IDD = _R(IDD_BLOBBYTABDLG); // Used to uniquely identify the tabbed dialog
												 // there is no actual resource associated with
												 // this ID.



/********************************************************************************************
>	BlobbyTabDlg::BlobbyTabDlg(): DialogTabOp(BlobbyTabDlg::IDD, BlobbyTabDlg::Mode)


	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/11/94
	Purpose:	The Constructor of the DialogTabOp derived class simply sets modality of the
				dialog, and it's ID.

********************************************************************************************/



BlobbyTabDlg::BlobbyTabDlg(): DialogTabOp(BlobbyTabDlg::IDD, BlobbyTabDlg::Mode)
{

	// Init Internals for conversion page
	UnitStrList[0] = String(TEXT("Millimetres"));
	UnitStrList[1] = String(TEXT("Centimetres"));
	UnitStrList[2] = String(TEXT("Metres"));
	UnitStrList[3] = String(TEXT("Inches"));
	UnitStrList[4] = String(TEXT("Feet"));
	UnitStrList[5] = String(TEXT("Yards"));
	UnitStrList[6] = String(TEXT("Points"));
	UnitStrList[7] = String(TEXT("Picas"));
	UnitStrList[8] = String(TEXT("Millipoints"));

    UnitList[0] = MILLIMETRES;
    UnitList[1] = CENTIMETRES;
	UnitList[2] = METRES;
	UnitList[3] = INCHES;
	UnitList[4] = FEET;
	UnitList[5] = YARDS;
	UnitList[6] = COMP_POINTS;
	UnitList[7] = PICAS;
	UnitList[8] = MILLIPOINTS;

}


/********************************************************************************************
>	MsgResult BlobbyTabDlg::Message( Msg* Message)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/2/94
	Inputs:		Message: The message


	Returns:	OK	 		  Message handled ok (return this even if you don't need to
							  respond to the message).

				FAIL 		  Something terrible happened whilst processing the message
				 			  eg. we ran out of memory. You must set ERROR if you
				 			  are returning this value.

				EAT_MSG       The Message was handled ok but don't send it to any
				 			  more MessageHandlers.

	Purpose:	This is the message handler for the BlobbyTabDlg

********************************************************************************************/


MsgResult BlobbyTabDlg::Message( Msg* Message)
{
	if (IS_OUR_DIALOG_MSG(Message))
	{
		DialogMsg* Msg = ((DialogMsg*) Message);

		BOOL EndDialog = FALSE;		// TRUE if we should quit the dialog
		BOOL CommitValues = FALSE; 	// TRUE if we should commit the dialog values

		// Determine from what page the message originated
		if (Msg->PageID == _R(IDD_BLOBTAB_PG1))
		{
			HandleCoordsMsg(Msg);
		} else if (Msg->PageID == _R(IDD_BLOBTAB_PG2))
		{
		    HandleRadioDazeMsg(Msg);
		} else if (Msg->PageID == _R(IDD_BLOBTAB_PG3))
		{
			HandleConvertMsg(Msg);
		} else if (Msg->PageID == 0)
		{
			// A message generated from the tabbed dialog itself
			switch (Msg->DlgMsg)
			{
				case DIM_COMMIT:		// Want to commit and quit
					EndDialog = TRUE;
					CommitValues = TRUE;
					break;

				case DIM_SOFT_COMMIT:	// Want to commit
					CommitValues = TRUE;
					break;

				case DIM_CANCEL:		// Want to quit
					EndDialog = TRUE;
					break;

				default:
					break;
			}
		} else
		{
			ERROR3("Message from unknown tab dialog page");
		}

		// Commit values here
		if (CommitValues)
		{
			TRACE( _T("BlobbyTabDlg::Message - If I wasn't blobby I would commit at this point"));
		}

		// End dialog here
		if (EndDialog)	// Dialog communication over
		{
			Close();
			End(); 	 	// End of dialog
	   	}
		return (DLG_EAT_IF_HUNGRY(Msg));   // I return EAT_MSG unless the message needs to be sent to all dialogs
	}
	return OK;
}


// -----------------------------------------------------------------------------------------
// Message handlers for individual pages

/********************************************************************************************

>	void BlobbyTabDlg::HandleCoordsMsg(DialogMsg* Msg)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/11/94
	Inputs:		Msg: The message sent from page 1 of the dialog
	Outputs:	-
	Returns:	-
	Purpose:	All messages generated from the tabbed dialog's page 1 get processed here
	Errors:		-
	Scope:		private
	SeeAlso:	BlobbyTabDlg::Message

********************************************************************************************/


void BlobbyTabDlg::HandleCoordsMsg(DialogMsg* Msg)
{
	TalkToPage(_R(IDD_BLOBTAB_PG1));  // The Coords Page identifier
	MILLIPOINT Value = 0;
	switch(Msg->DlgMsg)
	{
		case DIM_CREATE:
			// Init coords controls here

			// Set the scrollers range
			SetGadgetRange(_R(IDC_TSCROLLBAR1), 10, 100, 10);
			SetGadgetRange(_R(IDC_TSCROLLBAR2), 10, 100, 10);

			// Set initial scroll bar positions
			SetLongGadgetValue(_R(IDC_TSCROLLBAR1), 60, 10);
			SetLongGadgetValue(_R(IDC_TSCROLLBAR2), 60, 10);

			BOOL Valid;
			Value = (MILLIPOINT)(GetLongGadgetValue(_R(IDC_TSCROLLBAR1),10,100,0,&Valid)*72000);
			SetUnitGadgetValue(_R(IDC_TXVAL), INCHES, Value);
			Value = (MILLIPOINT)(GetLongGadgetValue(_R(IDC_TSCROLLBAR2),10,100,0,&Valid)*72000);
			SetUnitGadgetValue(_R(IDC_TYVAL), INCHES, Value);
			break;
		case DIM_SELECTION_CHANGED:
			if (Msg->GadgetID == _R(IDC_TSCROLLBAR1))
			{
				Value = (MILLIPOINT)(GetLongGadgetValue(_R(IDC_TSCROLLBAR1),10,100,0,&Valid)*72000);
				SetUnitGadgetValue(_R(IDC_TXVAL), INCHES, Value);
			} else if (Msg->GadgetID == _R(IDC_TSCROLLBAR2))
			{
				Value = (MILLIPOINT)(GetLongGadgetValue(_R(IDC_TSCROLLBAR2),10,100,0,&Valid)*72000);
				SetUnitGadgetValue(_R(IDC_TYVAL), INCHES, Value);
			}
			break;
		default:
			break;
	}
}

/********************************************************************************************

>	void BlobbyTabDlg::HandleRadioDazeMsg(DialogMsg* Msg)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/11/94
	Inputs:		Msg: The message sent from page 2 of the dialog
	Outputs:	-
	Returns:	-
	Purpose:	All messages generated from the tabbed dialog's page 2 get processed here
	Errors:		-
	Scope:		private
	SeeAlso:	BlobbyTabDlg::Message

********************************************************************************************/


void BlobbyTabDlg::HandleRadioDazeMsg(DialogMsg* Msg)
{
	TalkToPage(_R(IDD_BLOBTAB_PG2));  // The Coords Page identifier
	switch(Msg->DlgMsg)
	{
		case DIM_CREATE:
			// Init RadioDaze controls here
			SetLongGadgetValue(_R(IDC_TRADIO11), TRUE);
			SetLongGadgetValue(_R(IDC_TRADIO12), FALSE);
			SetLongGadgetValue(_R(IDC_TRADIO13), FALSE);

			SetLongGadgetValue(_R(IDC_TRADIO21), FALSE);
			SetLongGadgetValue(_R(IDC_TRADIO22), FALSE);
			SetLongGadgetValue(_R(IDC_TRADIO23), TRUE);

			break;
		case DIM_LFT_BN_CLICKED:
			if (Msg->GadgetID==_R(IDC_TRADIO11))
			{
				SetLongGadgetValue(_R(IDC_TRADIO23), TRUE);
				SetLongGadgetValue(_R(IDC_TRADIO21), FALSE);
				SetLongGadgetValue(_R(IDC_TRADIO22), FALSE);
			}
			else if (Msg->GadgetID==_R(IDC_TRADIO12))
			{
				SetLongGadgetValue(_R(IDC_TRADIO22), TRUE);
				SetLongGadgetValue(_R(IDC_TRADIO21), FALSE);
				SetLongGadgetValue(_R(IDC_TRADIO23), FALSE);
			}
			else if (Msg->GadgetID==_R(IDC_TRADIO13))
			{
				SetLongGadgetValue(_R(IDC_TRADIO21), TRUE);
				SetLongGadgetValue(_R(IDC_TRADIO22), FALSE);
				SetLongGadgetValue(_R(IDC_TRADIO23), FALSE);
			}
			else if (Msg->GadgetID==_R(IDC_TRADIO21))
			{
				SetLongGadgetValue(_R(IDC_TRADIO13), TRUE);
				SetLongGadgetValue(_R(IDC_TRADIO11), FALSE);
				SetLongGadgetValue(_R(IDC_TRADIO12), FALSE);
			}
			else if (Msg->GadgetID==_R(IDC_TRADIO22))
			{
				SetLongGadgetValue(_R(IDC_TRADIO12), TRUE);
				SetLongGadgetValue(_R(IDC_TRADIO11), FALSE);
				SetLongGadgetValue(_R(IDC_TRADIO13), FALSE);
			}
			else if (Msg->GadgetID==_R(IDC_TRADIO23))
			{
				SetLongGadgetValue(_R(IDC_TRADIO11), TRUE);
				SetLongGadgetValue(_R(IDC_TRADIO12), FALSE);
				SetLongGadgetValue(_R(IDC_TRADIO13), FALSE);
			}
			break;
		default:
			break;
	}
}

/********************************************************************************************

>	void BlobbyTabDlg::HandleConvertMsg(DialogMsg* Msg)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/11/94
	Inputs:		Msg: The message sent from page 3 of the dialog
	Outputs:	-
	Returns:	-
	Purpose:	All messages generated from the tabbed dialog's page 3 get processed here
	Errors:		-
	Scope:		private
	SeeAlso:	BlobbyTabDlg::Message

********************************************************************************************/


void BlobbyTabDlg::HandleConvertMsg(DialogMsg* Msg)
{
	TalkToPage(_R(IDD_BLOBTAB_PG3));  // The Coords Page identifier
	switch(Msg->DlgMsg)
	{
		case DIM_CREATE:
		{
			DefaultUnit = INCHES;   	// Default unit
			ConvUnit = MILLIMETRES;  // Conversion unit

			// Setup combos
			for (INT32 i=0; i<9; i++)
			{
				SetStringGadgetValue(_R(IDC_TCOMBO3), UnitStrList[i],FALSE,i);
				SetStringGadgetValue(_R(IDC_TCOMBO4), UnitStrList[i],FALSE,i);
			}
		}
		break;
		// Init coords controls here
		case DIM_SELECTION_CHANGED:
		{
			if (Msg->GadgetID == _R(IDC_TCOMBO3))
			{
//				BOOL Valid=0;
				WORD Index;
				GetValueIndex(_R(IDC_TCOMBO3),&Index);
				if (Index <= 8)
					DefaultUnit = UnitList[Index];

			}
			else if (Msg->GadgetID == _R(IDC_TCOMBO4))
			{
//				BOOL Valid=0;
				WORD Index;
				GetValueIndex(_R(IDC_TCOMBO4),&Index);
				if (Index <= 8)
					ConvUnit = UnitList[Index];
			}
		}
		break;
		case DIM_LFT_BN_CLICKED:
		{
			if (Msg->GadgetID == _R(IDC_TBUTTON3))
			{
				BOOL Valid=0;
				MILLIPOINT Value = GetUnitGadgetValue(_R(IDC_TUNITVAL),DefaultUnit,-10000000,10000000,0,&Valid);
				SetUnitGadgetValue(_R(IDC_TCONVVAL), ConvUnit, Value);
			}
		}
		break;
		default: break;
	}
}


/********************************************************************************************

>	OpState	BlobbyTabDlg::GetState(String_256*, OpDescriptor*)


	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/11/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	The standard GetState fn, does nothing
	Errors:		-
	SeeAlso:	-

********************************************************************************************/


OpState	BlobbyTabDlg::GetState(String_256*, OpDescriptor*)
{
	OpState OpSt;
	return(OpSt);
}

/********************************************************************************************

>	BOOL BlobbyTabDlg::Init()


	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/11/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	The standard Init fn for the op.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/


BOOL BlobbyTabDlg::Init()
{

return (RegisterOpDescriptor(
 								0,
 								_R(IDS_BLOBBYTABDLG),
								CC_RUNTIME_CLASS(BlobbyTabDlg),
 								OPTOKEN_BLOBBYTABDLG,
 								BlobbyTabDlg::GetState,
 								0,	/* help ID */
 								_R(IDBBL_BLOBBYDLG),
 								0,
 								0,
								SYSTEMBAR_ILLEGAL,
								TRUE,
								FALSE,
								TRUE,
								0,
								0, // One open instance
								GREY_WHEN_NO_CURRENT_DOC
								));

}

/********************************************************************************************

>	void BlobbyTabDlg::Do(OpDescriptor*)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/11/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Like all Do functions invokes the BlobbyTabDlg
	Errors:		-
	SeeAlso:	-

********************************************************************************************/



void BlobbyTabDlg::Do(OpDescriptor*)
{
	// Note: For a real tabbed dialog you would not even think about including an explicit
	// string in the dialog would you ?
	String_256 Name(_T("Blobby tab dialog"));
	SetName(&Name);
	BOOL ok = Create(); // Create the TAB dialog please
	if (!ok)
	{
		/*InformError(blobby)*/;	// Display an error if this is a real dialog please
		End();
	}
}
/********************************************************************************************

>	virtual BOOL BlobbyTabDlg::RegisterYourPagesInOrderPlease()


	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/11/94
	Inputs:		-
	Outputs:	-
	Returns:	return TRUE if all calls to AddAPage returned TRUE
	Purpose:	This virtual function will get called to give you a chance to add pages to your
				DialogTabOp. This function will usually make multiple calls to AddAPage to
				register the initial set of pages to be contained in the tab dialog. The pages
				should all be the same size and be registered in the left to right order in which
				you wish them to appear.

				This function get's called directly from the Create method, Don't call it
				explicitly yourself. If you return FALSE from this method then the Create
				method will fail.

	Errors:		-
	SeeAlso:	DialogTabOp::AddPage
	SeeAlso:	DialogOp::Create

********************************************************************************************/


BOOL BlobbyTabDlg::RegisterYourPagesInOrderPlease()
{
	BOOL ok = AddAPage(_R(IDD_BLOBTAB_PG1)) &&
			  AddAPage(_R(IDD_BLOBTAB_PG2)) &&
			  AddAPage(_R(IDD_BLOBTAB_PG3));
	return (ok);
}



//------------------------------------------------------------------------------------------
// Demo Dialog

const CDlgMode BlobbyBar::Mode = MODELESS;// Mode of the dialog

const UINT32 BlobbyBar::IDD = _R(IDD_SELECTORINFO);

BlobbyBar::BlobbyBar(): DialogOp(BlobbyBar::IDD, BlobbyBar::Mode)
{


}
MsgResult BlobbyBar::Message( Msg* Message)
{

	if (IS_OUR_DIALOG_MSG(Message))
	{
		DialogMsg* Msg = (DialogMsg*)Message;
		// Handle ok button
		if ((Msg->DlgMsg == DIM_COMMIT) || (Msg->DlgMsg == DIM_CANCEL))
		{
			Close(); // Hide the dialog
			End();
		}

		if ((Msg->DlgMsg == DIM_LFT_BN_CLICKED) || (Msg->DlgMsg == DIM_SELECTION_CHANGED))
		{
			if ( (Msg->GadgetID == _R(IDC_SEL_GRID_NW) ) ||
				 (Msg->GadgetID == _R(IDC_SEL_GRID_N)  ) ||
				 (Msg->GadgetID == _R(IDC_SEL_GRID_NE) ) ||
				 (Msg->GadgetID == _R(IDC_SEL_GRID_W)  ) ||
				 (Msg->GadgetID == _R(IDC_SEL_GRID_CENTRE) ) ||
				 (Msg->GadgetID == _R(IDC_SEL_GRID_E)  ) ||
				 (Msg->GadgetID == _R(IDC_SEL_GRID_SW) ) ||
				 (Msg->GadgetID == _R(IDC_SEL_GRID_S)  ) ||
				 (Msg->GadgetID == _R(IDC_SEL_GRID_SE) )
				)
			{
#define SetGrid(x) do {SetLongGadgetValue(x, (x==Msg->GadgetID)?1:0);} while(0)
				SetGrid(_R(IDC_SEL_GRID_NW));
				SetGrid(_R(IDC_SEL_GRID_N) );
				SetGrid(_R(IDC_SEL_GRID_NE));
				SetGrid(_R(IDC_SEL_GRID_W) );
				SetGrid(_R(IDC_SEL_GRID_CENTRE));
				SetGrid(_R(IDC_SEL_GRID_E) );
				SetGrid(_R(IDC_SEL_GRID_SW));
				SetGrid(_R(IDC_SEL_GRID_S) );
				SetGrid(_R(IDC_SEL_GRID_SE));
			}
#define SetBump(x,y) do {SetLongGadgetValue(x, GetLongGadgetValue(x,0,100)+y);} while(0)
			if (Msg->GadgetID == _R(IDC_SEL_BUMP_X_MORE))
				SetBump(_R(IDC_SEL_EDIT_X),1);
			if (Msg->GadgetID == _R(IDC_SEL_BUMP_X_LESS))
				SetBump(_R(IDC_SEL_EDIT_X),-1);
			if (Msg->GadgetID == _R(IDC_SEL_BUMP_Y_MORE))
				SetBump(_R(IDC_SEL_EDIT_Y),1);
			if (Msg->GadgetID == _R(IDC_SEL_BUMP_Y_LESS))
				SetBump(_R(IDC_SEL_EDIT_Y),-1);
			if (Msg->GadgetID == _R(IDC_SEL_BUMP_H_MORE))
				SetBump(_R(IDC_SEL_EDIT_H),1);
			if (Msg->GadgetID == _R(IDC_SEL_BUMP_H_LESS))
				SetBump(_R(IDC_SEL_EDIT_H),-1);
			if (Msg->GadgetID == _R(IDC_SEL_BUMP_W_MORE))
				SetBump(_R(IDC_SEL_EDIT_W),1);
			if (Msg->GadgetID == _R(IDC_SEL_BUMP_W_LESS))
				SetBump(_R(IDC_SEL_EDIT_W),-1);
			if (Msg->GadgetID == _R(IDC_SEL_BUMP_ANGLE_MORE))
				SetBump(_R(IDC_SEL_EDIT_ANGLE),1);
			if (Msg->GadgetID == _R(IDC_SEL_BUMP_ANGLE_LESS))
				SetBump(_R(IDC_SEL_EDIT_ANGLE),-1);
			if (Msg->GadgetID == _R(IDC_SEL_BUMP_SHEAR_MORE))
				SetBump(_R(IDC_SEL_EDIT_SHEAR),1);	
			if (Msg->GadgetID == _R(IDC_SEL_BUMP_SHEAR_LESS))
				SetBump(_R(IDC_SEL_EDIT_SHEAR),-1);

			if (Msg->GadgetID == _R(IDC_SEL_SHOWFILLBLOBS))
			{
				BOOL Enable=!GetLongGadgetValue(_R(IDC_SEL_SHOWFILLBLOBS),0,1);

//				EnableGadget(_R(IDC_SEL_BRACKETSYMBOL), Enable);
				EnableGadget(_R(IDC_SEL_BUMP_ANGLE_LESS), Enable);
				EnableGadget(_R(IDC_SEL_BUMP_ANGLE_MORE), Enable);
				EnableGadget(_R(IDC_SEL_BUMP_H_LESS), Enable);
				EnableGadget(_R(IDC_SEL_BUMP_H_MORE), Enable);
				EnableGadget(_R(IDC_SEL_BUMP_SHEAR_LESS), Enable);
				EnableGadget(_R(IDC_SEL_BUMP_SHEAR_MORE), Enable);
				EnableGadget(_R(IDC_SEL_BUMP_W_LESS), Enable);
				EnableGadget(_R(IDC_SEL_BUMP_W_MORE), Enable);
				EnableGadget(_R(IDC_SEL_BUMP_X_LESS), Enable);
				EnableGadget(_R(IDC_SEL_BUMP_X_MORE), Enable);
				EnableGadget(_R(IDC_SEL_BUMP_Y_LESS), Enable);
				EnableGadget(_R(IDC_SEL_BUMP_Y_MORE), Enable);
				EnableGadget(_R(IDC_SEL_EDIT_ANGLE), Enable);
				EnableGadget(_R(IDC_SEL_EDIT_H), Enable);
				EnableGadget(_R(IDC_SEL_EDIT_SHEAR), Enable);
				EnableGadget(_R(IDC_SEL_EDIT_W), Enable);
				EnableGadget(_R(IDC_SEL_EDIT_X), Enable);
				EnableGadget(_R(IDC_SEL_EDIT_XSCALE), Enable);
				EnableGadget(_R(IDC_SEL_EDIT_Y), Enable);
				EnableGadget(_R(IDC_SEL_EDIT_YSCALE), Enable);
				EnableGadget(_R(IDC_SEL_FLIPHORZ), Enable);
				EnableGadget(_R(IDC_SEL_FLIPVERT), Enable);
				EnableGadget(_R(IDC_SEL_GRID_CENTRE), Enable);
				EnableGadget(_R(IDC_SEL_GRID_E), Enable);
				EnableGadget(_R(IDC_SEL_GRID_N), Enable);
				EnableGadget(_R(IDC_SEL_GRID_NE), Enable);
				EnableGadget(_R(IDC_SEL_GRID_NW), Enable);
				EnableGadget(_R(IDC_SEL_GRID_S), Enable);
				EnableGadget(_R(IDC_SEL_GRID_SE), Enable);
				EnableGadget(_R(IDC_SEL_GRID_SW), Enable);
				EnableGadget(_R(IDC_SEL_GRID_W), Enable);
				EnableGadget(_R(IDC_SEL_PADLOCK), Enable);
				EnableGadget(_R(IDC_SEL_ROTATEBUTTON), Enable);
//				EnableGadget(_R(IDC_SEL_ROTATESYMBOL), Enable);
				EnableGadget(_R(IDC_SEL_SCALELINES), Enable);
//				EnableGadget(_R(IDC_SEL_SHEARSYMBOL), Enable);
				EnableGadget(_R(IDC_SEL_SHOWBOUNDSBLOBS), Enable);
//				EnableGadget(_R(IDC_SEL_SHOWFILLBLOBS), Enable);
				EnableGadget(_R(IDC_SEL_SHOWOBJECTBLOBS), Enable);
//				EnableGadget(_R(IDC_SEL_STATIC_WH), Enable);
//				EnableGadget(_R(IDC_SEL_STATIC_XY), Enable);
				EnableGadget(_R(IDPB_NAMEDLG_RUN_BUTTON), Enable);
			}
		}

		// Do some other fun stuff here

		return (DLG_EAT_IF_HUNGRY(Msg));
	}
	return OK;
}

// -----------------------------------------------------------------------------------------

OpState	BlobbyBar::GetState(String_256*, OpDescriptor*)
{
	OpState OpSt;
	return(OpSt);
}

// -----------------------------------------------------------------------------------------

BOOL BlobbyBar::Init()
{
	return (RegisterOpDescriptor(
								0,
								_R(IDS_BLOBBYBAR),
								CC_RUNTIME_CLASS(BlobbyBar),
								OPTOKEN_BLOBBYBAR,
								BlobbyBar::GetState,
								0,									// Help ID
								_R(IDBBL_BLOBBYBAR),				// Bubble help
								0,									// Resource ID
								0, 									// Control ID
								SYSTEMBAR_EDIT,						// Bar ID
								TRUE,								// Recieve system messages
								FALSE,								// Smart duplicate operation
								TRUE,								// Clean operation
								0,									// No vertical counterpart
								_R(IDS_BLOBBYBAR_ONE),				// String for one copy only
								0									// Auto state flags
								));

}

// -----------------------------------------------------------------------------------------

BOOL BlobbyBar::Create()
{
	if (DialogOp::Create())
	{
		// Set up  your gadgets here
		SetLongGadgetValue(_R(IDC_SEL_SHOWBOUNDSBLOBS),0);
		SetLongGadgetValue(_R(IDC_SEL_SHOWOBJECTBLOBS),0);
		SetLongGadgetValue(_R(IDC_SEL_SHOWFILLBLOBS),0);
		
		SetLongGadgetValue(_R(IDC_SEL_ROTATEBUTTON),0);

		SetLongGadgetValue(_R(IDC_SEL_GRID_NW),0);
		SetLongGadgetValue(_R(IDC_SEL_GRID_N),0);
		SetLongGadgetValue(_R(IDC_SEL_GRID_NE),0);
		SetLongGadgetValue(_R(IDC_SEL_GRID_W),0);
		SetLongGadgetValue(_R(IDC_SEL_GRID_CENTRE),0);
		SetLongGadgetValue(_R(IDC_SEL_GRID_E),0);
		SetLongGadgetValue(_R(IDC_SEL_GRID_SW),0);
		SetLongGadgetValue(_R(IDC_SEL_GRID_S),0);
		SetLongGadgetValue(_R(IDC_SEL_GRID_SE),0);
		
		SetLongGadgetValue(_R(IDC_SEL_EDIT_X),0);
		SetLongGadgetValue(_R(IDC_SEL_EDIT_Y),0);

		SetLongGadgetValue(_R(IDC_SEL_EDIT_W),0);
		SetLongGadgetValue(_R(IDC_SEL_EDIT_H),0);

		SetLongGadgetValue(_R(IDC_SEL_EDIT_XSCALE),0);
		SetLongGadgetValue(_R(IDC_SEL_EDIT_YSCALE),0);
		SetLongGadgetValue(_R(IDC_SEL_PADLOCK),0);

		SetLongGadgetValue(_R(IDC_SEL_EDIT_ANGLE),0);
		SetLongGadgetValue(_R(IDC_SEL_EDIT_SHEAR),0);

		SetLongGadgetValue(_R(IDC_SEL_SCALELINES),0);

		return TRUE;
    }
    else return FALSE;
}

// -----------------------------------------------------------------------------------------

void BlobbyBar::Do(OpDescriptor*)
{
	Create();
	Open();
}


