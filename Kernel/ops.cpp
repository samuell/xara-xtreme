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
	Implementation of the following classes:

	ActionList: Class used by an Operation to store UNDO, REDO, and Smart Duplicate actions
	Operation:  Class from which all Camelot operations are derived
	Action:     Class from which all operations actions are derived
                 
*/  


/*

*/

#include "camtypes.h"
//#include "inetop.h"

DECLARE_SOURCE("$Revision$");

#ifdef RALPH
#include "nativeop.h"
#include "ralphdoc.h"
#endif

//#include "ops.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "range.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "document.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "ophist.h"
//#include "ensure.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "docview.h"  - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "resource.h" 
//#include "simon.h"
//#include "errors.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "selstate.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "noderect.h"
//#include "tranform.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "mario.h"
//#include "spread.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "bars.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "progress.h"
//#include "app.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "menuops.h"
#include "blobs.h"
#include "sprdmsg.h"
#include "layer.h"
#include "lineattr.h"
//#include "trans2d.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "objchge.h"
#include "chapter.h"
#include "nodetxts.h"
//#include "attrmgr.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "cutop.h"
#include "dbugtree.h"
//#include "ralphcri.h"
#include "layermsg.h"	// LayerMsg::LayerReason::REDRAW_LAYER

//#include "cxfrech.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "attrval.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "userattr.h"
#include "tmpltatr.h"

#include "ngcore.h"
//#include "ngsetop.h"
 
CC_IMPLEMENT_MEMDUMP(ActionList, List)
CC_IMPLEMENT_DYNAMIC(OpParam, CCObject)
CC_IMPLEMENT_DYNCREATE( Operation, MessageHandler )
CC_IMPLEMENT_DYNCREATE( Action, ListItem )
CC_IMPLEMENT_DYNCREATE( InvalidateRegionAction, Action )
CC_IMPLEMENT_DYNCREATE( InvalidateRegionIfBgRedrawAction, InvalidateRegionAction )
CC_IMPLEMENT_DYNCREATE( HideNodeAction, Action )
CC_IMPLEMENT_DYNCREATE( ShowNodeAction, Action )
CC_IMPLEMENT_DYNCREATE( RestoreSelectionsAction, Action )
CC_IMPLEMENT_DYNCREATE( SelectDeselectAction, Action )
CC_IMPLEMENT_DYNCREATE( TransformNodeAction, Action )
CC_IMPLEMENT_DYNCREATE( UnApplyAction, Action )
CC_IMPLEMENT_DYNCREATE( ApplyAction, Action )

// Define this to allow HideNode actions to detect effect attributes without
// testing specific Tags. Fixes problems related to undo after using transp slider.
#define _HIDENODE_DETECTEFFECTS

// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW

//-------------------------------------------------------------------------------------------  
// ActionList methods   
/********************************************************************************************

>	ActionList::ActionList()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/7/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	ActionList constructor
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

ActionList::ActionList(): List(){}    

/********************************************************************************************

>	ActionList::~ActionList()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/7/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	ActionList destructor, deletes the action list and all actions it contains. 
				NOTE that REDO action lists must be deleted backwrds.
	Errors:		-
	SeeAlso:	ActionList::SlaughterBackwards;

********************************************************************************************/

ActionList::~ActionList()
{   
/*
	ListItem* Current = GetHead();   		// First action     
	while (Current != NULL)        		    // While there are more actions to delete
	{
		((Action*)RemoveHead())->Slaughter();  // Delete the action 
		Current = GetHead();                   // Get next action
		ContinueSlowJob(); 
	}
*/
	ListItem* Current = GetTail();   		// Last action     
	while (Current != NULL)        		    // While there are more actions to delete
	{
		((Action*)RemoveTail())->Slaughter();  // Delete the action 
		Current = GetTail();                   // Get next action
		ContinueSlowJob(); 
	}

} 
                 



/********************************************************************************************

>	BOOL ActionList::ExecuteForwards(BOOL AllButLast)


	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/7/93
	Inputs:		AllButLast: When FALSE execute all actions in the action list. 
							When TRUE execute all actions except the last
	Outputs:	-
	Returns:	TRUE if the action list was successfully executed
				FALSE if it was not 
				
	Purpose:	This method executes all actions in the action list forwards.  
				If there is no failure then all actions are deleted 
				If an action fails whilst executing no actions are deleted and
				FALSE is returned.  
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL ActionList::ExecuteForwards(BOOL AllButLast)
{                                          
    ListItem* CurrentAction = GetHead();  
    ListItem* LastAction = GetTail(); 
    
    // If we are to execute all actions but the last then the last action to
    // execute is prev(tail)
    if ((LastAction != NULL) && (AllButLast))
    	LastAction = LastAction->Previous; 
       
//TRACE( _T("Before Redo\n"));
//((Action*)CurrentAction)->GetWorkingDoc()->FindFirstSpread()->DST();

    BOOL Failed = FALSE; 
	while ((CurrentAction != LastAction) && (!Failed))  // For each action in the list
	{    
		// Execute the action                   
		Failed = (((Action*)CurrentAction)->Execute() == AC_FAIL); 

//((Action*)CurrentAction)->Dump();
//((Action*)CurrentAction)->GetWorkingDoc()->FindFirstSpread()->DST();

		CurrentAction = CurrentAction->Next;    
	}                   
	
	if (!Failed)
	{
		// Successfully executed action list so all actions are deleted. 
		Action* HeadItem = (Action *) GetHead(); 

		Document *pDoc = HeadItem->GetWorkingDoc();
		ERROR2IF(pDoc == NULL, FALSE, "No document attached to action in Action::ExecuteForwards");
		OperationHistory& OpHistory = pDoc->GetOpHistory();

		while (HeadItem != NULL)
		{         
			// Reduce the size of the operation history by the size of the action 
			// being deleted. 
			OpHistory.DecSize(HeadItem->GetSize()); 

			// Remove the action from the action list 
			delete (RemoveItem(HeadItem));

			// Get next action 
			HeadItem = (Action *) GetHead(); 
		}
	}
	
	return (!Failed);
}    

/********************************************************************************************

>	BOOL ActionList::ExecuteBackwards(BOOL AllButLast, BOOL bIgnoreSelectActions)


	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/7/93
	Inputs:		AllButLast: When FALSE execute all actions in the action list. 
							When TRUE execute all actions except the last
	Outputs:	-
	Returns:	TRUE if the action list was successfully executed
				FALSE if it was not 
				
	Purpose:	This method executes all actions in the action list backwards.  
				If there is no failure then all actions are deleted 
				If an action fails whilst executing no actions are deleted and
				FALSE is returned.   
	Errors:		-
	SeeAlso:	ActionList::ExecuteForwards

********************************************************************************************/

BOOL ActionList::ExecuteBackwards(BOOL AllButLast, BOOL bIgnoreSelectActions) 
{        
	ListItem* CurrentAction = GetTail();   

	if (AllButLast && (CurrentAction != NULL))   
		CurrentAction = CurrentAction->Previous; // Don't execute the last action 
       
//TRACE( _T("Before Undo\n"));
//((Action*)CurrentAction)->GetWorkingDoc()->FindFirstSpread()->DST();

	BOOL Failed = FALSE;                                       
	while ((CurrentAction != NULL) && (!Failed)) // For each action in the list
	{
		// Attempt to execute action 
		if (!(bIgnoreSelectActions &&
			CurrentAction->IsKindOf(CC_RUNTIME_CLASS(RestoreSelectionsAction)))
		   )
			Failed = (((Action*)CurrentAction)->Execute() == AC_FAIL);

//((Action*)CurrentAction)->Dump();
//((Action*)CurrentAction)->GetWorkingDoc()->FindFirstSpread()->DST();

		// Get next action 
		CurrentAction = CurrentAction->Previous; 
	}    
	
	if (!Failed)     
	{ 
		// Successfully executed action list so all actions are deleted. 
		Action* HeadItem = (Action *) GetHead(); 

		Document *pDoc = HeadItem->GetWorkingDoc();
		ERROR2IF(pDoc == NULL, FALSE, "No document attached to action in Action::ExecuteBackwards");
		OperationHistory& OpHistory = pDoc->GetOpHistory();

		// Loop while there are still actions to delete
		while (HeadItem != NULL)
		{                      
			// Reduce the size of the operation history by the size of the action being deleted
			OpHistory.DecSize(((Action*)HeadItem)->GetSize()); 

			// Delete the action 
			delete (RemoveItem(HeadItem));

			// Get next action  
			HeadItem = (Action *) GetHead(); 
		}
		
	}
	
	return (!Failed); 
}

/********************************************************************************************

	Action* ActionList::FindActionOfClass(CCRuntimeClass* ClassOfActionToFind, 
										  Action* LastAction = NULL); 



	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	04/11/94
	Inputs:		ClassOfActionToFind: the runtime class of the action we are to search for
				LastAction: 		 if you need to search for more than one occurence of
									 a particular action then specifying the LastAction parameter
									 will cause the search to begin after this Action. If this
									 parameter is not specified then the search will start at
									 the head of the action list.
	Outputs:	-
	Returns:	A pointer to the first action with runtime class ClassOfActionToFind, or NULL
				if no such action was found.
				
	Purpose:	To search the action list for the first action with runtime class 
				ClassOfActionToFind
	Errors:		-
	SeeAlso:	

********************************************************************************************/

 
Action* ActionList::FindActionOfClass(CCRuntimeClass* ClassOfActionToFind,
									  Action* LastAction)
{
	Action* Current = LastAction;	// Will be NULL if LastAction is not specified
	do
	{
		if (Current == NULL)
		{
			Current = (Action*)GetHead(); 
		}
		else
		{
			Current = (Action*)GetNext(Current);
		}
		if ((Current != NULL) && (Current->GetRuntimeClass() == ClassOfActionToFind))
		{
			return Current;
		}
	} while (Current != NULL); 
	return NULL; 	
} 


// ---------------------------------------------------------------------------------------- 
// Operation methods                              

       
        
Operation* Operation::CurrentDragOp = NULL; 

static DocView* pDraggingDocView = 0;

// Vars to do with stopping auto-repeats during drags
// See StartDrag() & EndDrag()
INT32 Operation::CurKeyboardSpeed;
BOOL Operation::ResetKeyboardSpeed 			 = FALSE;
BOOL Operation::KeyboardRepeatOffDuringDrags = FALSE;

BOOL Operation::s_bQuickRender = FALSE;

/*********************************************************************************************

>    static BOOL Operation::Init()

     Author:    Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
     Created:   16/10/95
     Inputs:    -
     Outputs:   -
     Returns:   -
     Purpose: 	Does any Operation initialisation necessary.
	 			Declares an Operation INI setting
     Errors:    

**********************************************************************************************/
 
BOOL Operation::Initialise()
{
	BOOL ok = TRUE;

// I've taken out the ability to switch auto-repeats off during drags (Markn - 19/10/95)
/*
 	// Declare the section that the preferences for Operations will live in
	if (ok) ok= Camelot.DeclareSection(TEXT("Dragging"), 1);

	// declare the var
	if (ok) ok= Camelot.DeclarePref(TEXT("Dragging"), TEXT("KeyboardRepeatOffDuringDrags "),  &Operation::KeyboardRepeatOffDuringDrags,  0, 1);
*/
	return ok;
}

/********************************************************************************************

>	Operation::Operation(CCRuntimeClass* MsgHandlerClass = CC_RUNTIME_CLASS(Operation))

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/6/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Constructs a new operation object: setting default operation flags, and 
				adding it to the Operation message list. Note that the routines which create 
				operations must create an instance for every seperate input that they receive. 
				They must not re-use an existing instance because operation objects have to be 
				submitted to the undo system to represent user operations on a 1 to 1 basis.   
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

// All operations are added to the Operation Class list by default, and will receive system 
// messages      
Operation::Operation(CCRuntimeClass* MessageHandlerClass) : 
	MessageHandler(MessageHandlerClass, TRUE)
{                 
	//Set default operation flag states. The defaults are for a clean non undoable operation

	OpFlags.Failed = FALSE;                                                                  
	OpFlags.ExecuteOnEnd = FALSE;		
	OpFlags.AllButLast = FALSE; 
	OpFlags.KeepOnEnd = FALSE;	
	OpFlags.UnwindingActions = FALSE;
	OpFlags.HasOwnTimeIndicator = FALSE;  
	OpFlags.SucceedAndDiscard = FALSE;   
 	OpFlags.DeleteOnEnd = FALSE;
	OpFlags.IgnoreSelectActions = FALSE;

	DeleteUndoOpsToMakeSpace = FALSE;  
	pDocToInformOfOpStatus = NULL; 
	
	// Let the system know that we are currently doing the operation for the first time
	OpStatus = DO;

	// The only time Operations get created is when they are to be put on the "live ops"
	// list, and the only time they get put on the "live ops" lists is when they will
	// affect whatever is the current document.
	pOurDoc = Document::GetCurrent();
	pOurView = View::GetCurrent();

}

/********************************************************************************************

>	Operation::~Operation()                                                      


	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/7/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Operation destructor
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

Operation::~Operation()
{                                     
	// The destructors of the Operations action lists will automatically be called

	// However, we must ensure the REDO action list is slaughtered in the correct
	// (reverse) direction... the UNDO list will slaughter its items in the correct
	// direction in its destructor

	// Removed by Simon 19/04 All action lists now slaughter themselves backwards
	//RedoActions.SlaughterBackwards();
}

/********************************************************************************************

>	void Operation::EndOp()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/7/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	This function is called by End(), to end an operation. It removes the 
				operation from the live list and then does various things depending on the 
				state of the operation's flags. It is normally called by the operation's Do 
				function. The way the operation flags are interpreted by EndOp are described 
				by the following algorithm.      
	
	MonoOn  
				       
				IF the Failed flag is TRUE
					IF the ExecuteOnEndFlag is TRUE      
						IF the AllButLast flag is TRUE
							Execute all but the last action in the operations UNDO/REDO 
							action list.   	
						ELSE
							Execute all actions in the operation's UNDO/REDO action list   
					ELSE
						IF we are undoing 
							Delete the operation and all redo operations
						ELSE IF we are doing or redoing
							Delete the operation and all undo operations
						
					IF the OperationStatus is DO
						Delete the operation
				ELSE 
					IF the SystemUndo flag is TRUE and we are doing the operation
						Add operation to the operation history
					ELSE 
						IF the keepOnEnd flag is FALSE
							delete the operation
					IF the Clean flag is FALSE
						Inform the document that it has been modified
		
	MonoOff				
	Scope:		private			
	Errors:		-
	SeeAlso:	Operation::End

********************************************************************************************/

void Operation::EndOp()
{   
	OpFlags.DeleteOnEnd = FALSE; // Set to TRUE if the operation should be deleted

	// Some static attribute apply flags, added by Will
	AttributeManager::HaveAskedAboutContoneColours = FALSE;
	AttributeManager::UserCancelledContoneColours = FALSE;

	Document *pDocument = GetWorkingDoc();

//	if (pDocument != NULL)
//		pDocument->FlushRedraw();

	Document *pDoc = (Document *) Camelot.Documents.GetHead();

	while (pDoc != NULL)
	{
		// Force any pending redraw to be done now
		if (pDoc == pDocument)
			pDocument->FlushRedraw();

		pDoc = (Document *) Camelot.Documents.GetNext(pDoc);
	}

	// Remember that it is valid now for pOpDesc to be NULL (eg. for InformationBarOps 
	// which don't have OpDescriptors)
	/*OpDescriptor* pOpDesc =*/ OpDescriptor::FindOpDescriptor(this->GetRuntimeClass()); 
	
	OperationHistory*  OpHist;      

	// if there is not a current document and the operation in progress is undoable
	// then ensure
PORTNOTE("other","Removed OpAsynchClipartImport usage")
	if( IsKindOf( CC_RUNTIME_CLASS(UndoableOperation) ) 
#ifndef EXCLUDE_FROM_XARALX
		 && !IsKindOf(CC_RUNTIME_CLASS(OpAsynchClipartImport)))
#endif
		)
    	ENSURE(	(pDocument != NULL), "Document has been destroyed before operation has ended");

	// The operation is ending so don't send any more messages to it
	if (OpStatus == DO)
	{
		SendNoMoreMessages(this); 
	}

	// Has the operation copied any component data between documents 
	if (pDocToInformOfOpStatus != NULL)
	{
		if (OpFlags.Failed)
		{
			// We may have copied data accross but because the operation has failed
			// we must discard this.
			pDocToInformOfOpStatus->AbortComponentCopy(); 
		} 
		else
		{
			// Call EndComponentCopy to commit the new data
			if (!pDocToInformOfOpStatus->EndComponentCopy())
			{
				// Note that AbortComponentCopy has been called
				// Unable to complete the op so it must fail I'm afraid
				OpFlags.Failed = TRUE; 
			} 

		}
	}

	// if the operation is not undoable then dispose of it and exit.
	if (!IsKindOf(CC_RUNTIME_CLASS(UndoableOperation)))
	{
				
		if (!OpFlags.KeepOnEnd)
		{   
			OpFlags.DeleteOnEnd = TRUE;      
		}
		goto End;
	}
	else
	{
		// If the operation has been done but does not have any Undo actions then
		// dispose of the operation and then exit.
	   	if  ((UndoActions.GetHead() == NULL)  && (OpStatus == DO))
		{   
			if (!OpFlags.KeepOnEnd)
			{   
				OpFlags.DeleteOnEnd = TRUE; 
			}
			goto End;
		}    
	}

	OpHist = &(pDocument->GetOpHistory());   
	
	// Check if the operation failed
	if (OpFlags.Failed || OpFlags.SucceedAndDiscard)
	{            
		if (OpFlags.ExecuteOnEnd)
		{                            
		    // Set the unwindingActions flag so that no actions are created in further 
		    // calls to the Action::Init function.  
			OpFlags.UnwindingActions = TRUE;      
			 
			// Determine which action list to execute  

			// If we are UNDOING then the failure must have occured when trying to create 
			// REDO actions for the operation, so execute the RedoActions list (which will 
			// restore the UNDO actions list). 


			BOOL bOK = FALSE;
			if (OpStatus == UNDO)
			{
			    bOK = RedoActions.ExecuteBackwards(OpFlags.AllButLast, OpFlags.IgnoreSelectActions);
				ERROR3IF(!bOK, "An operation failed whilst unwinding");
			}
			// If we are DOING or REDOING then the failure must have occured when trying to 
			// create UNDO actions, so execute the UndoActions list. 
			
			else
			{
				bOK = UndoActions.ExecuteBackwards(OpFlags.AllButLast, OpFlags.IgnoreSelectActions);
				ERROR3IF(!bOK, "An operation failed whilst unwinding");
			}
		}   
		else if (OpFlags.Failed) // Discard
		{   
			if (OpStatus == UNDO)
			{     
				// Delete the operation and all redo operations
			
				// One step back in time  
				OpHist->NowPtr = OpHist->OpHistoryList.GetPrev(OpHist->NowPtr);  
				// Delete all Redo operations including the current operation                 
				OpHist->DeleteRedoableOps();   
				goto End; 
			}   
			else // REDO and DO       
			{
				 // Delete all Undo records including the current operation 
				OpHist->NowPtr = OpHist->OpHistoryList.GetNext(OpHist->NowPtr); 
				OpHist->DeleteUndoableOps();                                   
				
				// In the case of a REDO we can return because the operation has been deleted. 
				if (OpStatus == REDO)
					goto End; 
            }
            
		}            
        // If we are DOING the operation for the first time delete the operation
		if (OpStatus == DO)          
		{
			if (!OpFlags.KeepOnEnd)
			{
				OpHist->DecSize(this->GetSize()); // The size total in the operation history 
												 // will include the current size of the 
												 // operation.  
				OpFlags.DeleteOnEnd = TRUE; 
			}
			goto End;   
		}
	}
	
	if (!OpFlags.Failed)   // Operation has not failed
	{  
		// Decide if the operation should be added to the documents operation history
		if  (OpStatus == DO && !OpFlags.SucceedAndDiscard)
		{    
			// The undo action list should contain actions, the redo action list should not
			ENSURE(UndoActions.GetHead() != NULL, "The undo action list is empty"); 
			ENSURE(RedoActions.GetHead() == NULL, "The redo action list is not empty"); 

			// Add operation to document's operation history
			pOurDoc->GetOpHistory().Add(this); 
		} 
		
		// If the Operation has no OpDescriptor then we assume that the operation is clean
		//if (pOpDesc != NULL)
		//{   
			//if (!(pOpDesc->GetOpFlags().Clean)) 
			//{ 
				// BODGE We do not inspect the Clean flag, cos not everyone has set it correctly.
				// We know this is an undoable operation so it must have affected the document.
				// Therefore we set the documents modified flag.
				
				// Any operation with a FALSE clean flag has indicated that it 
				// will or may have modified the document in some way. Therefore
				// inform the document that it has been modified. 

#if !defined(EXCLUDE_FROM_RALPH)
				// The only exception to this is OpCopy			IS_A
				if (!(GetRuntimeClass() == CC_RUNTIME_CLASS(OpCopy)))
					pOurDoc->SetModified(TRUE);
#endif
			//} 
		//}
	} 

	End:

	EndSlowJob();   

#if !defined(EXCLUDE_FROM_RALPH)
	// Inform the world that the operation has ended
	// We don't want to do it at the end of an OpExit though
	if (GetRuntimeClass() != CC_RUNTIME_CLASS(OpExit))
	{
		BROADCAST_TO_ALL(OpMsg(this, OpMsg::END)); 
	}
#endif

   	// The Operation which has just been performed has probably changed the state
	// of the system in some way so we update the state of all the bars. We don't
	// do this if the operation has failed however because:
	// A. When an operation has failed the state of the system should be restored
	// 	  to the state it was in prior to the operation being performed. So it
	//	  will not be necessary.
	// B. The fact that the operation has failed implies that we are probably low on 
	// 	  memory so it's probably safer not to update it anyway. Note that if the
	//	  bar state is ever incorrect this should not be disastrous because the
	// 	  GetState fn of the operation is always called prior to Invoking an operation
	//	  anyway. 

	if (!OpFlags.Failed && !OpFlags.SucceedAndDiscard)
	{ 

		// Call the virtual PerformMergeProcessing function. This gives the operation a chance
		// to merge itself with the previous operation if appropriate.
		if(IS_KIND_OF(UndoableOperation) && !(OpFlags.DeleteOnEnd))
		{ 
			if  (OpStatus == DO)  // We only want to merge ops when Doing the operation
			{    
				// Note that the PerformMergeProcessing fn could delete this op (so be careful !)
				((UndoableOperation*)this)->PerformMergeProcessing(); 
 			}
 		}
	}

	 
	// If there was any sort of error during that Op which hasn't been reported to the user yet
	// then we'd better report it now!
	InformLastError();

	// If the operation has survived all that then reset all failure and control 
	// flags. 	
	OpFlags.Failed = FALSE;                                                                  
	OpFlags.ExecuteOnEnd = FALSE;		
	OpFlags.AllButLast = FALSE;  
	OpFlags.UnwindingActions = FALSE; 
	OpFlags.SucceedAndDiscard = FALSE; 
	OpFlags.IgnoreSelectActions = FALSE;
	DeleteUndoOpsToMakeSpace = FALSE;  


	// Be careful what is after this
	if (OpFlags.DeleteOnEnd)
		delete (this); 
	// The bar state will be refreshed  during idle time
	DialogBarOp::SetSystemStateChanged(); 
}    
 


/********************************************************************************************

>	void Operation::End()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/10/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	This function calls EndOp which performs the main end of operation processing.
				see Operation::EndOp
				
	Errors:		-
	SeeAlso:	Operation::EndOp

********************************************************************************************/

void Operation::End()
{
	// When an operation is ending all sorts of scary stuff is going on with nodes, so we 
	// do not want to be rendering at the time !..
PORTNOTE("other","Removed RalphCriticalSection usage")
#ifndef EXCLUDE_FROM_XARALX
//	RalphCriticalSection rcs;  
	if ( (OpFlags.SystemUndo) && (OpStatus == DO) && (!(OpFlags.ExecuteOnEnd)) )
	{ 
		// A call to StartUndoOperation is made which creates a RestoreSelections action 
		// which when executed 	will restore the selection state to the state which existed 
		// just after performing the operation. 
		StartUndoOperation(); 
	}
#endif

	// Slight bodge to ensure that all op permission flags of all nodes are set to 
	// PERMISSION_UNDEFINED regardless whether the op succeeded or failed.
	if (this->IS_KIND_OF(UndoableOperation))
	{
		UndoableOperation* pOp = (UndoableOperation*)this;
		ObjChangeFlags cFlags;
		ObjChangeParam ObjChange(OBJCHANGE_IGNORE,cFlags,NULL,pOp);
		pOp->UpdateChangedNodes(&ObjChange);
	}

	EndOp();

}	

/********************************************************************************************

>	Document *Operation::GetWorkingDoc()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/13/95
	Returns:	The document this operation is working on.
	Purpose:	Find out which document this operation is working on.  This will remain
				constant throughout the life of most operations with the exception of
				DialogOps. (The File open operation also changes its working document
				but this is a very rare exception).
	SeeAlso:	Operation::GetWorkingView; Operation::GetWorkingDocView

********************************************************************************************/


/********************************************************************************************

>	View *Operation::GetWorkingView()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/13/95
	Returns:	The View this operation is attached to.
	Purpose:	Returns the View that is associated with this operation. 
				Note that the majority of operations don't care what view they work on - most
				of them are document-based, and cause all views attached to a document to be
				updated.  However, some operations such as zooming, changing the quality 
				etc. only work on a particular view.
	SeeAlso:	Operation::GetWorkingDocView; Operation::GetWorkingDoc

********************************************************************************************/


/********************************************************************************************

>	DocView *Operation::GetWorkingDocView()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/13/95
	Returns:	The DocView this operation is attached to, or NULL if it is not attached
				to a DocView.
	Purpose:	Returns the DocView that is associated with this operation.  This will return
				NULL if the View the operation is attached to is not actually a DocView (e.g.
				it may be a PrintView).  Note that the majority of operations don't care
				what view they work on - most of them are document-based, and cause all
				views attached to a document to be updated.  However, some operations such
				as zooming, changing the quality etc. only work on a particular view.
	SeeAlso:	Operation::GetWorkingView; Operation::GetWorkingDoc

********************************************************************************************/

DocView *Operation::GetWorkingDocView()
{
	// Return the DocView if we are attached to one, otherwise just return NULL.
	if (pOurView->IS_KIND_OF(DocView))
		return (DocView *) pOurView;
	else
		return NULL;
}

/********************************************************************************************

>	UINT32 Operation::GetSize()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/7/93
	Inputs:		-
	Outputs:	-
	Returns:	The size of the current operation
	Purpose:	To calculate the size of the operation, i.e the sum of the sizes of the 
				operation's actions. 
	Errors:		An assertion failure will occur if the operation's UNDO and REDO action 
				lists both contain actions. 
	SeeAlso:	-

********************************************************************************************/

UINT32 Operation::GetSize()
{
	UINT32 TotalOfActionSizes = 0;   

//	Do we really need this ensure? It appears to run fine without it (sjk 29/3/00)
//	ENSURE((RedoActions.IsEmpty() || UndoActions.IsEmpty()), 
//		"An operation's UNDO and REDO action lists both contained actions");                                                        
                     
    // Calculate size of all Undo actions  
    
    // Get first action of operation                          
	ListItem* pAc = (UndoActions.GetHead());  
		
	while (pAc != NULL) // For each action
	{  
		TotalOfActionSizes += ((Action*)pAc)->GetSize(); 
		pAc = UndoActions.GetNext(pAc);   
	}  
                    
    // Calculate size of all Redo actions     
    
    pAc = (RedoActions.GetHead());  
		
	while (pAc != NULL) // For each action
	{  
		TotalOfActionSizes += ((Action*)pAc)->GetSize(); 
		pAc = RedoActions.GetNext(pAc);   
	}  

    // Calculate size of all Smart actions                  
    
    pAc = (SmartActions.GetHead());  
		
	while (pAc != NULL) // For each action
	{  
		TotalOfActionSizes += ((Action*)pAc)->GetSize(); 
		pAc = SmartActions.GetNext(pAc);   
	} 
	
	return (TotalOfActionSizes);  
}


/********************************************************************************************

>	static Operation* Operation::GetCurrentDragOp()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/7/93
	Inputs:		-
	Outputs:	-
	Returns:	IF there is  a drag going on THEN
					a pointer to the operation involved in the drag is returned
				ELSE
					NULL is returned 
	
	Purpose:	To check if there is a drag currently going on, and if there is to return 
				the operation involved in the drag. 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

Operation* Operation::GetCurrentDragOp()
{   
	return (CurrentDragOp); 
}                                                      




/********************************************************************************************

>	static BOOL Operation::GetQuickRender(Node* pNode = NULL)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/2004
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if quick rendering is required at the moment
				FALSE otherwise
	Purpose:	Gets the QuickRender state (composed of CurrentDragOp or quick render flag)
	Errors:		-
	SeeAlso:	SetQuickRender

********************************************************************************************/

BOOL Operation::GetQuickRender(Node* pNode)
{
	if (pNode==NULL)
		return (CurrentDragOp!=NULL || s_bQuickRender);
	else
		return ((CurrentDragOp!=NULL && pNode->IsDragged()) || s_bQuickRender);
}                                                      




/********************************************************************************************

>	static void Operation::SetQuickRender(BOOL bNewState, Operation* pQROp)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/2004
	Inputs:		bNewState - new state for global, static "QuickRender" flag
	Outputs:	-
	Returns:	-
	Purpose:	Sets the QuickRender flag
	Errors:		-
	SeeAlso:	GetQuickRender

********************************************************************************************/

void Operation::SetQuickRender(BOOL bNewState, Operation* pQROp)
{   
	if (s_bQuickRender && !bNewState)
	{
		// Flush any pending renders now, before we reset the quickrender flag
		DocView* pDocView = DocView::GetCurrent();
		if (pDocView)
		{
			pDocView->FlushRedraw();
//#pragma message( __LOCMSG__ "Removed RenderRegionList usage" )
			GetApplication()->ServiceRendering(TRUE);
		}
	}

	// Now reset the flag
	s_bQuickRender = bNewState;
	if (bNewState && CurrentDragOp==NULL)
		CurrentDragOp = pQROp;
	if (!bNewState && CurrentDragOp==pQROp)
		CurrentDragOp = NULL;
}                                                      




// ------------------------------------------------------------------------------------------
// The following functions are called by DocView

/********************************************************************************************

	virtual void Operation::DragPointerMove( DocCoord PointerPos,
						    		         ClickModifiers ClickMods, Spread *pSpread, BOOL bSolidDrag)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/7/93
	Inputs:		PointerPos: Position of the mouse pointer
				ClickMods:  Click modifiers
	Outputs:	-
	Returns:	-
	Purpose:	Pure virtual function which tells the operation that the mouse has moved. 
	Errors:		-
	SeeAlso:	ClickModifiers

********************************************************************************************/

void Operation::DragPointerMove( DocCoord PointerPos,
						ClickModifiers ClickMods, Spread *pSpread, BOOL bSolidDrag)
{
}  

/********************************************************************************************

	virtual void Operation::DragPointerIdle( DocCoord PointerPos,
						    		         ClickModifiers ClickMods, Spread *pSpread, BOOL bSolidDrag)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/7/93
	Inputs:		PointerPos: Position of the mouse pointer
				ClickMods:  Click modifiers
	Outputs:	-
	Returns:	-
	Purpose:	Pure virtual function which tells the operation that nothing is going on
				so that it has time to do background jobs. 
	Errors:		-
	SeeAlso:	ClickModifiers

********************************************************************************************/


void Operation::DragPointerIdle( DocCoord PointerPos,
						ClickModifiers ClickMods, Spread *pSpread, BOOL bSolidDrag)
{
}       

/********************************************************************************************

	virtual void Operation::DragFinished( DocCoord PointerPos,
						    		      ClickModifiers ClickMods, 
						    		      Spread *pSpread,
						     			  BOOL Success,
										  BOOL bSolidDrag)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/7/93
	Inputs:		PointerPos: Position of the mouse pointer
				ClickMods:  Click modifiers      
				Success:    Indicates if the drag was terminated by the user pressing escape
							(FALSE) or by the user releasing the mouse buttons (TRUE). 
	Outputs:	-
	Returns:	-
	Purpose:	Pure virtual function which tells the operation that a drag has finished. 
				Once the operation knows the drag has finished it should call EndDrag() 
				to terminate the drag. 
				 
	Errors:		-
	SeeAlso:	ClickModifiers

********************************************************************************************/

void Operation::DragFinished( DocCoord PointerPos,
							  ClickModifiers ClickMods,
							  Spread *pSpread,
				   			  BOOL Success,
							  BOOL bSolidDrag)
{
}    



/********************************************************************************************
>	virtual BOOL Operation::DragKeyPress(KeyPress* pKeyPress, BOOL bSolidDrag)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/94
	Inputs:		A pointer to the key-press event.
	Outputs:	-
	Returns:	TRUE if the Operation handles the key-press, FALSE if it isn't interested.
	Purpose:	Keyboard interface for operations.
	Errors:		-
	SeeAlso:	-
********************************************************************************************/

BOOL Operation::DragKeyPress(KeyPress* pKeyPress, BOOL bSolidDrag)
{
	return FALSE;
}



/********************************************************************************************
>	virtual void Operation::DragModeChanged(BOOL bSolidDrag)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/2003
	Inputs:		Drag mode flag
	Outputs:	-
	Returns:	-
	Purpose:	Notify drag handler of change in drag mode.
	Errors:		-
	SeeAlso:	-
********************************************************************************************/

void Operation::DragModeChanged(BOOL bSolidDrag)
{
}



/********************************************************************************************

>	DocCoord Operation::MakeRelativeToSpread(Spread *Dest, Spread *Src, DocCoord Pos)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/12/93
	Inputs:		Dest - the spread that the coord should be relative to.
				Src - the spread that the coord is currently relative to.
				Pos - the coord to convert.
	Returns:	The coordinate (now relative to Dest)_
	Purpose:	This function converts a coordinate relative to one spread so that it is
				then relative to another spread.

********************************************************************************************/

DocCoord Operation::MakeRelativeToSpread(Spread *Dest, Spread *Src, DocCoord Pos)
{
	// Translate the Spread Coord 'Pos' into Document coordinates, and thence to logical coords
	Src->SpreadCoordToDocCoord(&Pos);
	WorkCoord LogicalPos = Pos.ToLogical(Src, GetWorkingView());

	// and get the dest spread origin in logical coords
	DocCoord DestSpreadOrigin = Dest->GetSpreadCoordOrigin(TRUE, GetWorkingView());
	WorkCoord LogicalDest = DestSpreadOrigin.ToLogical(Dest, GetWorkingView());

	// Work out the difference between the two
	XLONG OffsetX = LogicalPos.x - LogicalDest.x;
	XLONG OffsetY = LogicalPos.y - LogicalDest.y;
	DocCoord Result( OffsetX, OffsetY );

	// and return the result
	return Result;
}



/********************************************************************************************

>	void Operation::RenderDragBlobs(DocRect, Spread*, BOOL bSolidDrag)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/8/93
	Purpose:	This function is called by other parts of the operation to put on
				and take off drag EORing. It will also be called as part of the
				main redraw loop in the case where part of the window has become
				invalid (due to scrolling maybe). The function should contain a
				RenderOnTop loop as described in DocView::RenderOnTop.
				This function can make no assumptions about where it is being called
				from.
	SeeAlso:	DocView::RenderOnTop

********************************************************************************************/

void Operation::RenderDragBlobs(DocRect, Spread*, BOOL bSolidDrag)
{
	// This function will do nothing. It should exist in derived classes of those
	// operations that want to render things during a drag.
}

//-------------------------------------------------------------------------------------------





// ------------------------------------------------------------------------------------------
// These functions tell the Op what to do when it ends.

/********************************************************************************************

>	void Operation::FailAndExecute()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/6/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	This fail function will cause the following to occur when the operation ends
	
				The operation will be removed from the live list. 
				All actions in the operations action list will be executed (latest first)  
				The operation will be deleted without being added to the document's operation 
				history. 

				
				
	Errors:		-  
	SeeAlso:	Operation::FailAndExecuteAllButLast
	SeeAlso:    Operation::FailAndDiscard


********************************************************************************************/

void Operation::FailAndExecute()
{           
	OpFlags.Failed = TRUE;     
	OpFlags.ExecuteOnEnd = TRUE;      
}

void Operation::FailAndExecuteIgnoreSelActions()
{           
	OpFlags.Failed = TRUE;
	OpFlags.ExecuteOnEnd = TRUE;
	OpFlags.IgnoreSelectActions = TRUE;
}

/********************************************************************************************

>	void Operation::FailAndExecuteAllButLast()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/6/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	This fail function will cause the following to occur when the operation ends
	
				The operation will be removed from the live list
				All but the last action in the operation's action list will be executed 
					(latest first) 
				The operation will be deleted without being added to the document's 
				operation history.   
				
    Errors:
    
	SeeAlso:	Operation::FailAndExecute
	SeeAlso:    OPeration::FailAndDiscard

********************************************************************************************/

void Operation::FailAndExecuteAllButLast()
{                    
	OpFlags.Failed = TRUE;           
	OpFlags.ExecuteOnEnd = TRUE; 
	OpFlags.AllButLast = TRUE; 
}                                           

/********************************************************************************************

>	void Operation::FailAndDiscard()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/6/93
	Inputs:		-
	Outputs:	-
	Returns:	-   
	Purpose:	This fail function will cause the following to occur when the operation ends
	
				The operation will be removed from the live list
				The operation will be deleted without being added to the document's 
				operation history.     
				
				Note: This  fail function can be called several times on the same operation,
					  however only the first call will have any effect on the operation's
					  flags.   
    Errors:
    
	SeeAlso:	Operation::FailAndExecute
	SeeAlso:    OPeration::FailAndExecuteAllButLast

********************************************************************************************/


void Operation::FailAndDiscard()
{   
	// Because the fail functions can be called several times on the same operation their 
	// implementation has to ensure that only the flags set by the FIRST call are used
	
	if (!OpFlags.Failed)
	{
		OpFlags.Failed = TRUE;   
	}                            
}                                                            

/********************************************************************************************

>	void Operation::SucceedAndDiscard()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/05/95
	Inputs:		-
	Outputs:	-
	Returns:	-   
	Purpose:	This function can be called to discard the operation when it ends. 
				Any hidden nodes which have been generated will remain
				in the tree.			
    Errors:
    
	SeeAlso:	Operation::FailAndExecute
	SeeAlso:    OPeration::FailAndExecuteAllButLast

********************************************************************************************/

void Operation::SucceedAndDiscard()
{
	OpFlags.SucceedAndDiscard = TRUE; 
}


//------------------------------------------------------------------------------------------

// All subclasses should provide a function named "Do" which executes 
// their primary function. It is not supplied in the base class at 
// the moment but might be one day!
//	void Do()

// These two functions execute the appropriate action list.
// They should NOT be overridden by the subclasses of Operation.   
                                        
/********************************************************************************************

>	BOOL Operation::Undo()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/7/93
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the operation was undone without failing
				false otherwise 
	
	Purpose:	To execute the operation's undo action list
	Errors:		An assertion failure will occur if the UNDO action list is empty 
	SeeAlso:	Operation::Redo

********************************************************************************************/

BOOL Operation::Undo()
{           
	ERROR2IF(UndoActions.IsEmpty(), FALSE, "Trying to execute an empty UNDO action list");

	OpStatus = UNDO;
	UndoActions.ExecuteBackwards(FALSE);  
	BOOL fSuccess = !OpFlags.Failed;
PORTNOTE("other","Removed OpChangeBarProperty usage")
#ifndef EXCLUDE_FROM_XARALX
	if (fSuccess)
	{
		NameGallery* pNameGallery = NameGallery::Instance();
		if (pNameGallery)
		{
			pNameGallery->PreTriggerEdit((UndoableOperation*) this, 0, (Node*) 0);
			if (IS_A(this, OpChangeBarProperty))
				pNameGallery->m_TouchedBar = ((OpChangeBarProperty*) this)->m_BarIndex;
		}
	}
#endif

	End(); // End the operation properly, remember that this may delete the operation 
	return fSuccess; 
}               



/********************************************************************************************

>	BOOL Operation::Redo()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/7/93
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the operation was redone without failing
				FALSE otherwise 
				
	Purpose:	To execute the operation's redo action list
	Errors:		An assertion failure will occur if the redo action list is empty 
	SeeAlso:	Operation::Undo

********************************************************************************************/

BOOL Operation::Redo()
{                         
	ERROR2IF(RedoActions.IsEmpty(), FALSE, "Trying to execute an empty REDO action list");

	OpStatus = REDO;
	RedoActions.ExecuteBackwards(FALSE);  
	BOOL fSuccess = !OpFlags.Failed; 
	if (fSuccess)
	{
PORTNOTE("other","Removed OpChangeBarProperty usage")
#ifndef EXCLUDE_FROM_XARALX
		NameGallery* pNameGallery = NameGallery::Instance();
		if (pNameGallery != 0)
		{
			pNameGallery->PreTriggerEdit((UndoableOperation*) this, 0, (Node*) 0);
			if (IS_A(this, OpChangeBarProperty))
				pNameGallery->m_TouchedBar = ((OpChangeBarProperty*) this)->m_BarIndex;
		}
#endif
	}

	End();  
	return fSuccess;
}



// This function asks the operation to Do itself again. Notice that DoSmart 
// can be polymorphic because no smart functions can take params - they 
// all work on the selection.
void Operation::DoSmart()
{
	// Empty.
}  



// Every operation must provide a virtual Do function which gets called to initiate an 
// an operation. 

/********************************************************************************************

>	virtual void Operation::Do(OpDescriptor* OpDesc)  

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/6/94
	Inputs:		OpDesc: A pointer to the OpDescriptor which invoked the operation
	Outputs:	-
	Returns:	-
	Purpose:	This function should be over-ridden to perform an operation without 
				passing a parameter
	Errors:		-
	SeeAlso:	Operation::DoWithParam

********************************************************************************************/

void Operation::Do(OpDescriptor*)
{
	// Empty.
}   

/********************************************************************************************

>	void Operation::DoWithParam(OpDescriptor*, OpParam* pOpParam)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/6/94
	Inputs:		OpDesc:     A pointer to the OpDescriptor which invoked the operation
				pOpParam:	Optional operator parameters, these are similar to a 
							window procedures (lParam, wParam). Each operation requiring
							parameters will describe what these should be. 
	Outputs:	-
	Returns:	-

	Purpose:	This function should be over-ridden to perform an operation	which requires
				parameters.

	Errors:		-
	SeeAlso:	Operation::Do

********************************************************************************************/

void Operation::DoWithParam(OpDescriptor*, OpParam* pOpParam)
{
	// empty		
}   
                    
/********************************************************************************************

>	OpFlgsStr Operation::GetOpFlgs()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/7/93
	Inputs:		-
	Outputs:	-
	Returns:	Operation's flags
	Purpose:	For finding the status of the operations flags
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

OpFlgsStr Operation::GetOpFlgs()
{
	return (OpFlags); 
}   
     



/********************************************************************************************

>	void Operation::DeleteOnEnd()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/9/96
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Sets the DeleteOnEnd OpFlag telling the operation to delete itself at
				the very end of EndOp. This is used in place of calling delete(this) for
				safety reasons.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void Operation::DeleteOnEnd()
{
	ERROR3IF(this==NULL, "DeleteOnEnd passed NULL this");

	OpFlags.DeleteOnEnd = TRUE;
}   
     



/********************************************************************************************

>	ActionList* Operation::GetUndoActionList()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/7/93
	Inputs:		-
	Outputs:	-
	Returns:	A pointer to the operations UNDO action list
	Purpose:	To find the UNDO action list of the operation.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

ActionList* Operation::GetUndoActionList()
{
	return (&UndoActions); 
}    

/********************************************************************************************

>	ActionList* Operation::GetRedoActionList()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/7/93
	Inputs:		-
	Outputs:	-
	Returns:	A pointer to the operations REDO action list
	Purpose:	To find the REDO action list of the operation.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
                 
                     
ActionList* Operation::GetRedoActionList() 
{                       
	return (&RedoActions); 
}	


// Functions called by subclasses of Operation...  

/********************************************************************************************

>	BOOL Operation::StartDrag(DragType type, 
							  DocRect* MoveBBox, DocCoord* StartPos,
							  BOOL KeepAccuracy = TRUE)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/7/93
	Inputs:		type: The drag type
				KeepAccuracy - TRUE (default) if you want the drag's response to be biased
							   	    to accurately track the user's mouse movements.
							  FALSE if you want the drag's response to be biased to be as
									up to date with the current mouse position as possible,
									at the expense of losing out on some intermediate
									mouse movements.
				bSolidDragSupported
							- FALSE (default) Do EORed outline dragging
							  TRUE	Update the tree during dragging
	Outputs:	- 
	Returns:	TRUE if the drag was started successfully otherwise FALSE. 
	Purpose:	To start a drag 
	Errors:		-    
	Scope:		protected
	SeeAlso:	-

********************************************************************************************/

BOOL Operation::StartDrag(DragType type,
						  DocRect* MoveBBox,
						  DocCoord* StartPos,
						  BOOL KeepAccuracy,
						  BOOL bSolidDragSupported /* = FALSE*/)
{                         
	if (CurrentDragOp == NULL)  // Check if any other operations are currently performing a 
								// drag
	{                           
		// No other operations are performing a drag 
		
		// Tell the OS to start dragging
		pDraggingDocView = DocView::GetCurrent();
		if (pDraggingDocView->StartDrag(this,
										type,
										MoveBBox,
										StartPos,
										KeepAccuracy,
										bSolidDragSupported))
		{
			CurrentDragOp = this; 	// Record the operation    

			// I've taken out the ability to switch auto-repeats off during drags (Markn - 19/10/95)
			/*
			if (KeyboardRepeatOffDuringDrags && !ResetKeyboardSpeed)
			{
				// Get the current keyboard repeat rate
				::SystemParametersInfo(SPI_GETKEYBOARDSPEED, 0, &CurKeyboardSpeed, 0);

				// Reset the repeat rate to 0 (i.e. no repeat)
				ResetKeyboardSpeed = ::SystemParametersInfo(SPI_SETKEYBOARDSPEED, 0, NULL, 0);
			}
			*/

			return (TRUE);    
		} 
		else
			pDraggingDocView = 0;
	}
	return (FALSE); 
}

/********************************************************************************************

>	BOOL Operation::EndDrag()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/7/93
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if a drag was successfully ended, otherwise FALSE
	Purpose:	To end a drag
	Errors:		An assertion failure will occur if EndDrag is called and there is no current
				drag taking place. 
	SeeAlso:	-

********************************************************************************************/

BOOL Operation::EndDrag()
{                         
	ERROR3IF(CurrentDragOp == NULL, "Trying to end a drag which is not taking place !");    

	// I've taken out the ability to switch auto-repeats off during drags (Markn - 19/10/95)
	/*
	// If we set the keyboard repeat rate to 0, reset it to the previous setting
	if (KeyboardRepeatOffDuringDrags && ResetKeyboardSpeed)
	{
		::SystemParametersInfo(SPI_SETKEYBOARDSPEED, CurKeyboardSpeed, NULL, 0);
		ResetKeyboardSpeed = FALSE;
	}
	*/

	// Rewritten so it won't blow up on OLE drags.  Keeping the kernel drag running during
	// the end of an OLE drag outside Camelot is a very risky business!
	BOOL fOK = FALSE;
	if (pDraggingDocView)
	{
		fOK = pDraggingDocView->EndDrag(this);
		pDraggingDocView = 0;
	}
#ifdef _DEBUG
	else
	{
		ERROR3("No dragging DocView in Operation::EndDrag");
	}
#endif

	CurrentDragOp = 0;
	return fOK;

/*
	DocView *pDocView = DocView::GetSelected();

	ERROR3IF(pDocView == NULL, "No selected DocView in Operation::EndDrag()");

	if ((pDocView != NULL) && (pDocView->EndDrag(this))) // Tell OS to end drag
	{
		CurrentDragOp = NULL;  

		return (TRUE);  // Ended drag successfully  
	}
	else
		return (FALSE); // Failed to end the drag (and don't bother resetting CurrentDragOp!?!?)
*/
}  

/********************************************************************************************

>	virtual void Operation::GetOpName(String_256* OpName)

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/10/93
	Outputs:	The name of the operation
	Purpose:	To get the name of the operation

********************************************************************************************/

void Operation::GetOpName(String_256* OpName)
{                         
	OpDescriptor *Operation = OpDescriptor::FindOpDescriptor(this->GetRuntimeClass());
	
	if (Operation)             
		Operation->GetText(OpName, OP_UNDO_TEXT);
	
}  

BOOL Operation::UserWantsToDeleteUndoOps(void)
{
	if (!DeleteUndoOpsToMakeSpace)    
	{                                     
		// Inform the user that there is not enough system memory. Give them the option of 
		// trying to make room for the action object by deleting undo records.            
		if (InformWarning(_R(IDS_DELETE_UNDO), 
						  _R(IDS_OK),          
						  _R(IDS_CANCEL)) == 2)  
		{   
			FailAndExecute(); 
			return (FALSE);
		}                   
	}                                      
	DeleteUndoOpsToMakeSpace = TRUE;      
	return (TRUE); 
}  

// Call this function from the Operation's Init method to register the operations OpDescriptor

/********************************************************************************************

>	static BOOL Operation::RegisterOpDescriptor(
							 					  UINT32 toolID,
							 					  UINT32 txID, 
							 					  CCRuntimeClass* RuntimeClass,
							 					  TCHAR* tok,
							 					  pfnGetState gs,
							 					  UINT32 helpId = 0,
												  UINT32 bubbleID = 0,
							 					  UINT32 resourceID = 0,
												  UINT32 controlID = 0,
												  SystemBarType GroupBarID = SYSTEMBAR_ILLEGAL,
							 					  BOOL ReceiveMessages = FALSE, 
							 					  BOOL Smart = FALSE, 
							 					  BOOL Clean = TRUE,
					 							  OpDescriptor* pVertOpDesc = NULL,
					 							  UINT32 OneOpenInstID = 0,		
												  UINT32 AutoStateFlags = 0)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/3/94
	Inputs:
			 	toolID 			: Tool identifier (from which the Module ID can be found)
			    txID  			: String Resource ID
				RuntimeClass	: The operation's runtime class
			    tok,			: pointer to the token string
			    gs,	 			: pointer to the GetState function
			    helpId 			: help identifier 
			    bubbleID   		: string resource for bubble help
			    resourceID,		: resource ID
				controlID,		: control ID within that resource within that tool
			  	GroupBarID		: The system bar group this op desc belongs to
			    ReceiveMessages : Does the OpDescriptor need to respond to system messages
								: NOTE ** This MUST be TRUE for ops connected to buttons, or your DO **
								:      ** function will never be called by a button-press!           **
			    Smart 			: A smart duplicate operation ?
			    Clean 			: Does the operation change the document
				pVertOpDesc		: The op desc used to describe this op desc's vertical counterpart
				OneOpenInstID	: When not 0 only one live instance
				 				  of the operation is permitted, and
								  OneOpenInstID is the string resource
								  ID describing why.This will be useful
				 				  for 1 open instance dialogs.
				AutoStateFlags  : Flags which indicate conditions when
				 				  the operation should automatically be
								  greyed/ticked etc..(This cuts down the 
								  number of tests that need to be made in the 
								  GetState function). See Opdesc.h for a description

								  
			      
	Outputs:	-
	Returns:	TRUE if successful, else ERROR is called and FALSE returned. 

	Purpose:	This function should be called from the operations Init method. It 
				creates an OpDescriptor for the operation

	SeeAlso:	Opdescriptor::OpDescriptor

********************************************************************************************/


BOOL Operation::RegisterOpDescriptor( 
			 					     UINT32 toolID,
			 					  	 UINT32 txID,
			 					  	 CCRuntimeClass* RuntimeClass,
				 					 TCHAR* tok,
				 					 pfnGetState gs,
				 					 UINT32 helpId,
									 UINT32 bubbleID,
				 					 UINT32 resourceID,
				 					 UINT32 controlID,
									 SystemBarType GroupBarID,
				 					 BOOL ReceiveMessages,
				 					 BOOL Smart,
				 					 BOOL Clean,
									 OpDescriptor* pVertOpDesc,
    	 							 UINT32 OneOpenInstID,		
									 UINT32 AutoStateFlags,
									 BOOL fCheckable /*= FALSE*/ )
{
	BOOL ok = TRUE;

	// tok is going into a string which turns out to be a string 32, so do a sanity check (Neville 26/6/97)
	size_t len = camStrclen(tok);
	ERROR2IF(len > 32,FALSE,"Operation::RegisterOpDescriptor token buffer overflow!");

	// Try to create the OpDescriptor
	OpDescriptor* pHorzOpDesc = new OpDescriptor(
				 							toolID,
				 							txID,
											RuntimeClass, 
											tok,
				 							gs,
				 							helpId,
				 							bubbleID,
											resourceID,
											controlID,
				 							ReceiveMessages, 
											Smart,
				 							Clean,
				 							OneOpenInstID,
				 							AutoStateFlags,
											fCheckable
				 						   );

	ok = (pHorzOpDesc != NULL);

#if !defined(EXCLUDE_FROM_RALPH) && !defined(EXCLUDE_FROM_XARALX)
	// If a hidden group bar has been specified try to connect the new OpDescriptor to
	// a group bar...
	if (GroupBarID && ok)
	{
		ok = SystemBarOp::Connect(GroupBarID,pHorzOpDesc,pVertOpDesc);
	}
#endif

	ERRORIF(!pHorzOpDesc, _R(IDE_NOMORE_MEMORY), FALSE);
	return TRUE; 
}  


void Operation::OperationMemoryFailure()
{
	
	// Inform the user that the operation has failed                        
	InformWarning(_R(IDS_UNDO_MEMORY_FAILURE),                                  
				  _R(IDS_OK));                                              
	FailAndExecute();                                                       
}        
 


/********************************************************************************************

>	virtual BOOL Operation::OnIdleEvent()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/09/94
	Outputs:	TRUE if more idle events are required by the op
				FALSE if it dosen't.
	Purpose:	Called on idle events if an operation has tegistered to recieve
				them.
	SeeAlso:	Application::RegisterIdleProcessor, Application::RemoveIdleProcessor
********************************************************************************************/
BOOL Operation::OnIdleEvent()
{
	return FALSE;
}


/********************************************************************************************
> 	BOOL Operation::GetStatusLineText(String_256* pText, Spread* pSpread, DocCoord DocPos, ClickModifiers Mods)

    Author: 	Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
    Created:	16/1/95
	Inputs:		pSpread - pioter to spread mouse is over
				DocPos  - position of mouse in doc
				Mods	- click modifiers
	Outputs:	pText	- buffer for StatusLine text
    Returns:   	FALSE if fails
    Purpose:    Each DragOp should override this function to return valid status line text
********************************************************************************************/

BOOL Operation::GetStatusLineText(String_256* ptext, Spread* pSpread, DocCoord DocPos, ClickModifiers ClickMods)
{
	*ptext="";		// defaults to a null string
	return TRUE;
}


/********************************************************************************************

> 	virtual BOOL Operation::UpdateChangedNodes(ObjChangeParam* pParam,Spread* pSpread = NULL)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/02/95
	Inputs:		pParam	= describes the way an op has changes the node(s)
				pSpread	= ptr to spread that contains the nodes that were effected by the op
						  NULL means use selected spread
	Outputs:	-
	Returns:	TRUE if all effected nodes were able to cope with the op, FALSE otherwise
	Purpose:	This calls OnChildChange() on all nodes that were effected by the op.
				This includes all parents of all effect nodes too.
				It calls the effected nodes in a depth-first traversal.

				Note: 	All nodes that were asked if the op could happen to them (via AllowOp()) will have
						their op permission states set to PERMISSION_UNDEFINED.  This includes all nodes
						that said no (ie. op permission == PERMISSION_DENIED) plus their parents.

						It is specified that even if this func returns FALSE, all nodes that were asked
						(i.e. all nodes that have an op permission state of either PERMISSION_ALLOWED or 
						PERMISSION_DENIED) will have their permission state reset to PERMISSION_UNDEFINED.

				This function updates all nodes on the spread's layers.  Only effected nodes up to the layer level have 
				their OnChildChange() function called.

				EXCEPTION:  If pParam's ObjChangeType is OBJCHANGE_IGNORE, the effected nodes do NOT have their
							OnChildChange() param called, but everything else applies, i.e. all effected nodes will
							have their op permission state reset to PERMISSION_UNDEFINED.
							This OBJCHANGE_IGNORE type is used by Operation::End() so that it can ensure the tree
							is in a safe state, even if an op fails, and/or an op is not implemented correctly.

	SeeAlso:	Node::AllowOp(),Node::GetOpPermission(),Node::SetOpPermission()

********************************************************************************************/

BOOL Operation::UpdateChangedNodes(ObjChangeParam* pParam,Spread* pSpread)
{
	ERROR2IF(pParam == NULL,FALSE,"pParam == NULL");
	if (pSpread == NULL) pSpread = Document::GetSelectedSpread();
	//ERROR2IF(pSpread == NULL,FALSE,"Can't find a spread to work on");

	if (pSpread == NULL)
		return TRUE;

	// We need to invalidate the sel range just in case it's been cached in the middle of an op.
	// If the sel range is cached when nodes in the tree have op permission state that's PERMISSION_DENIED
	// then nodes that should potentially be included in the range will be excluded.
	// And as the thing's cached all hell breaks loose, so yet another independent system has to know
	// about sel ranges and the fact they're cached, and yes it does scare me.
//	SelRange* pRange = GetApplication()->FindSelection();
//	if (pRange != NULL)
//		pRange->Update(FALSE,NULL);

	return UpdateChangedNode(pParam,pSpread->FindFirstChild());
}

/********************************************************************************************

>	BOOL Operation::UpdateChangedNode(ObjChangeParam* pParam,Node* pNode)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/02/95
	Inputs:		pParam	= describes the way an op has changes the node(s)
				pNode	= subtree root of nodes to update (Layer being top-level type)
	Outputs:	-
	Returns:	TRUE if all effected nodes were able to cope with the op, FALSE otherwise
	Purpose:	Helper function for UpdateChangedNodes() (which calls this for each layer in a spread).
				This will call itself recursively for all children, to ensure a depth-first traversal
				of the tree.

	SeeAlso:	UpdateChangedNodes()

********************************************************************************************/

BOOL Operation::UpdateChangedNode(ObjChangeParam* pParam,Node* pNode)
{
	if (pNode == NULL)  return TRUE;
	if (pParam == NULL) return FALSE;

	BOOL ok = TRUE;

	while (pNode != NULL)
	{
		// Get the op permission state for this node
		OpPermissionState OpState = pNode->GetOpPermission();

		// if NOT undefined, we are dealing with a node that's been involved in the op, so update children of node
		if (OpState != PERMISSION_UNDEFINED)
			ok = ok && UpdateChangedNode(pParam,pNode->FindFirstChild());

		// If the node allowed the op to happen, and the change type is NOT one to ignore,
		// inform the node via OnChildChange
		if (ok && OpState == PERMISSION_ALLOWED && pParam->GetChangeType() != OBJCHANGE_IGNORE)
			ok = (pNode->OnChildChange(pParam) == CC_OK);
		else
		{
			// BODGE TEXT - this is HORRIBLE but the safest way to clear flags in text story on failure
			if (IS_A(pNode,TextStory))
				((TextStory*)pNode)->ClearNodeAndDescendantsFlags();
		}

		// Set the op permission to undefined, as we've finished with this node now
		pNode->SetOpPermission(PERMISSION_UNDEFINED);

		// get the next sibling node
		pNode = pNode->FindNext();
	}

	return ok;
}

/********************************************************************************************

>	BOOL Operation::UpdateChangedNode(ObjChangeParam* pParam,Document* pDoc)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/02/95
	Inputs:		pParam	= describes the way an op has changes the node(s)
				pDoc	= ptr to doc 
	Outputs:	-
	Returns:	TRUE if all effected nodes were able to cope with the op, FALSE otherwise
	Purpose:	This calls the Spread version of this func for all spreads in the given doc 

	SeeAlso:	UpdateChangedNodes(...,Spread* pSpread)

********************************************************************************************/

BOOL Operation::UpdateChangedNodes(ObjChangeParam* pParam,Document* pDoc)
{
	ERROR2IF(pParam == NULL,FALSE,"pParam == NULL");
	ERROR2IF(pDoc	== NULL,FALSE,"pDoc == NULL");

	BOOL ok = TRUE;
	Chapter* pChapter = Node::FindFirstChapter(pDoc);
	while (pChapter != NULL && ok)
	{
		// Now find the first child of the Chapter, because this is the level at which Spreads hang out
		Node* pNode = pChapter->FindFirstChild();

		// Update changed nodes in each of the chapter's spreads
		while (pNode != NULL && ok)
		{
			if (pNode->IsSpread())
				ok = UpdateChangedNodes(pParam,(Spread*)pNode);

			pNode = pNode->FindNext();
		}

		pChapter = pChapter->FindNextChapter(); 
	}

	return ok;
}

/********************************************************************************************

>	BOOL Operation::UpdateAllChangedNode(ObjChangeParam* pParam)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/02/95
	Inputs:		pParam	= describes the way an op has changes the node(s)
	Outputs:	-
	Returns:	TRUE if all effected nodes were able to cope with the op, FALSE otherwise
	Purpose:	This calls the Document version of this func for all documents in the application

	SeeAlso:	UpdateChangedNodes(...,Document* pDoc)

********************************************************************************************/

BOOL Operation::UpdateAllChangedNodes(ObjChangeParam* pParam)
{
	ERROR2IF(pParam == NULL,FALSE,"pParam == NULL");

	BOOL ok = TRUE;
	Document* pDoc = (Document*)GetApplication()->Documents.GetHead();
	while (pDoc != NULL && ok)
	{
		ok   = UpdateChangedNodes(pParam,pDoc);
		pDoc = (Document*)GetApplication()->Documents.GetNext(pDoc);
	}
	return ok;
}



/********************************************************************************************

>	virtual void Operation::Dump()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/8/95
	Inputs:		-
	Outputs:	Displays information on the debugging output
	Returns:	-
	Purpose:	Displays information on this operation, and its two action lists.  Override to
				display information specific to your operation
	SeeAlso:	OperationHistory::DumpAll
				OperationHistory::DumpLast
				Action::Dump

********************************************************************************************/
void Operation::Dump()
{
#if DEBUG_TREE
	// Display ops name
	TRACEALL( _T("\nOperation : %s\n"), GetRuntimeClass()->GetClassName() );

	// Display undo actions 
	if (UndoActions.IsEmpty())
	{
		TRACEALL( _T("    No Undo actions\n") );
	}
	else
	{
		TRACEALL( _T("    %d Undo actions\n"), UndoActions.GetCount() );
		ListItem* pAction = UndoActions.GetHead();

		while (pAction != NULL)
		{
			((Action*)pAction)->Dump();
			pAction = UndoActions.GetNext(pAction);
		}
	}

	// Display redo actions 
	if (RedoActions.IsEmpty())
	{
		TRACEALL( _T("    No Redo actions\n") );
	}
	else
	{
		TRACEALL( _T("    %d Redo actions\n"), RedoActions.GetCount() );
		ListItem* pAction = RedoActions.GetHead();

		while (pAction != NULL)
		{
			((Action*)pAction)->Dump();
			pAction = RedoActions.GetNext(pAction);
		}
	}
#endif
}


//------------------------------------------------------------------------------------------
//Action methods  

//Action* Action::LastDiscardableAction = NULL;    
                
/********************************************************************************************

>	Action::Action()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/7/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Action constructor
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

Action::Action():ListItem()
{
} 

/********************************************************************************************

>	virtual Action::~Action()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/7/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Action destructor
	Errors:		-
	SeeAlso:	Action::Slaughter

********************************************************************************************/

Action::~Action()
{                                  
	//if (IsUserName("Simon"))
	//	TRACE( _T("Action being deleted\n"));   
}  
             
/********************************************************************************************

>	virtual Action::Slaughter()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/7/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	destructor which gets called when an operation is deleted  
	Errors:		-
	SeeAlso:	Action::Slaughter

********************************************************************************************/

void Action::Slaughter() 
{               
	delete (this); // Call the destructor 
}   
    
/********************************************************************************************

>	ActionCode Action::Execute()         

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/8/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	This is a pure virtual method which should be redefined for all derived
				classes of Action.                                               
				
	Errors:		If this method is ever called then an ENSURE failure will occur. 
	SeeAlso:	-

********************************************************************************************/
      
ActionCode Action::Execute()         
{
	ENSURE(FALSE, "Calling pure virtual method of abstract class Action");    
	return (AC_FAIL); // The function needs to return a value, even though it never will !. 
} 

/********************************************************************************************

	
>	ActionCode Action::Init(Operation* pOp, 
						ActionList* pActionList, 
						UINT32 ActionSize, 
						CCRuntimeClass* ActionClass, 
						Action** NewAction) 
							
	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/7/93                      
	
	Inputs:		pOp: The operation to which the action should be added
				
				pActionList: The action list in the operation object
				
				Size:  		 The size of the action in bytes. This should be the total  
							 size of the action (including any objects pointed to by the
							 action). 

 	Outputs:    NewAction:   A pointer to the action if it could be allocated. This
							 will be NULL if we are unwinding (AC_OK gets returned)

	Returns:	AC_FAIL:     There was not enough room in the operation history for the 
							 action and the user did not wish to continue. Usually 
							 End() should be called in this situation. 
				         
				AC_NORECORD: There was not enough room in the operation history for
							 the action, but the user requested that he wished to 
							 continue without undo. 

							 Currently AC_RECORD is never returned, as we never 
							 fail an operation just because we exceed the max size
							 of the operation history !. See code for a reason why
				
				AC_OK      : The action was successfully initialised and added to the 
							 operation. OR we are unwinding. In this situation 
							 NewAction will be NULL.
				         
				           
	Purpose:	To check that there is sufficient room for the action in the operation
				history, and if there is, then to add the action to the operations 
				action list. 
				
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

ActionCode Action::Init(Operation* pOp, 
						ActionList* pActionList, 
						UINT32 ActionSize, 
						CCRuntimeClass* ActionClass, 
						Action** NewAction) 
						  
{  
	
	ERROR3IF(!(pOp->IsKindOf(CC_RUNTIME_CLASS(UndoableOperation))), 
		"Trying to create an action for a non-undoable operation"); 

	// Update the hour glass
	ContinueSlowJob(); 

//	OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor(pOp->GetRuntimeClass()); 
	                                        
	OperationHistory& OpHist = pOp->GetWorkingDoc()->GetOpHistory(); 
            
	//if (Action::LastDiscardableAction != NULL) 
    //{                                          
	//	delete (LastDiscardableAction);  			// LastDescardable action
	//	Action::LastDiscardableAction = NULL; 
	//}  
 

 	// If we are unwinding an operation during Undo or Redo	then 
 	// we don't want to create an action as it will already exist on the oposite action
	// list. 
	
	// When unwinding a Do however things are a little different. Whilst we don't need
	// need to generatate Redo actions for redoing purposes, we do need them to tidy up 
	// the dying Operation. eg. A ShowNodeAction may need to delete its hidden node, and its
	// Slaughter method must be called to acheive this. However if we fail to allocate an 
	// action in this circumstance we don't want to return an AC_FAIL. Leaking memory is 
	// preferable to having nowhere to go. 

	if (pOp->GetOpFlgs().UnwindingActions && (pOp->OpStatus!=DO || pOp->GetOpFlgs().KeepOnEnd))
    {
    	(*NewAction) = NULL; 
    	return (AC_OK); 
    
    }
    
    
    //	TRACE( _T("Size of operation history = %ld\n"), OpHist.GetSize()); 


	// Try to allocate memory for the action, deleting undo if we get desperate
	ALLOC_WITH_FAIL((*NewAction),(Action*)(ActionClass->CreateObject()),pOp); 	      
	
	// oh no it's all gone horribly wrong, I wouldn't like to be in your shoes
	if ((*NewAction) == NULL)
	{
		// If we are unwinding at this point then we don't fail the action (See note above)
		if ((pOp->GetOpFlgs().UnwindingActions))
		{
			ERROR3IF(!(pOp->OpStatus == DO), "OpStatus should be Do");
			return (AC_OK);	 
		}
		else
		{      
			return (AC_FAIL);
		}
	}
	//if (IsUserName("Simon"))
	//{
	//	TRACE( _T("Adding action %s, NumBytes = %lu\n"),(*NewAction)->GetRuntimeClass()->m_lpszClassName, ActionSize);  
	//} 


	// If fail and discard then there is no need to prompt the user more than once
	if (!(pOp->GetOpFlgs().Failed))
	{                                                
		// Check that there is enough room in the operation history for the action
		if ((OpHist.GetSize() + ActionSize) > OpHist.GetMaxSize())    
		{                      
			//BOOL NoRoom = FALSE; 
			// There is not enough room so first of all check if we can make room 
			if (OpHist.GetMaxSize() > ActionSize) 
			{                          
				// See if we can make room by deleting UNDO records
				//if (OpHist.ReduceSize((OpHist.GetMaxSize() - ActionSize),TRUE) == FALSE)
				//	NoRoom = TRUE;
				// The outcome does not matter
				OpHist.ReduceSize((OpHist.GetMaxSize() - ActionSize),TRUE, TRUE);
			}
			else
			{
				// Very big action
				OpHist.ReduceSize(0,TRUE, TRUE);
			}                    
			// We used to give the user the option of either continuing and loosing all undo/redo
			// or have them abort their current operation. I didn't think this was a very good idea
			// so I have removed the code. Now we let the operation history grow to what
			// ever size it wants to, but keep the Maximum size the same. The operation history
			// will shrink towards the maximum size whenever there are undo operations which can be deleted.
			
			// I found that in practice we were reaching this NoRoom situation when the user was undoing
			// almost back to the start. If the user redoes then this shrinks the OpHist 
			// because there are undo ops to discard, if the user does then this also shrinks the OpHist because
			// all redo ops are discarded. 
			
			/*
			else
				NoRoom = TRUE; 
	 		if (NoRoom)
			{   
			        
				// We cannot make enough room in the operation history for the action
				UINT32 _R(IDS_MSG) = (pOp->OpStatus == UNDO) ? 
					(UINT32)_R(IDS_CANNOT_REDO_WARNING): (UINT32)_R(IDS_CANNOT_UNDO_WARNING);    
									
				if (InformWarning(_R(IDS_MSG),
								  _R(IDS_CONTINUE),   
								  _R(IDS_CANCEL)) == 2) 
				{      
					// The user does not want to continue, so inform the action's operation that 
					// when the operation ends the actions previously added to its action list 
					// will be executed to bring the document back to the state it was in before the
					// operation was* started.
					pOp->FailAndExecute();   
					LastDiscardableAction = *NewAction; 
					return (AC_FAIL);  
				}
				else
					// The user does want to continue, so inform the operation that when it ends 
					// it should be deleted. 
					pOp->FailAndDiscard();     
				
			}
			*/
		}
    }
	// Add the action to the operation's action list.        
	
	// Note that actions are added to the operation's action list even after a fail and discard 
	// error.  The reason for this is that if memory runs out whilst trying to execute an operation
	// after such an error, we need to be able to unwind all actions which have been performed. If 
	// we do not do this then all sorts of chaos will occur. 
	
	pActionList->AddTail(*NewAction);    
	(*NewAction)->pOperation = pOp;            // Record the operation to which the action is attached. 
	
	// Store a pointer to the opposite ActionList to that which the Action is atatched 
	(*NewAction)->pOppositeActLst = (pOp->GetUndoActionList() == pActionList) ? 
		(pOp->GetRedoActionList()) : (pOp->GetUndoActionList());
		  
	(*NewAction)->Size = ActionSize;           // Record the size of the action           
	                                         
	// Even though the operation has not yet been added to the operation history and maybe
	// never will, we increase the current size of the operation history to accomodate the 
	// action. If the operation fails then the Size of the operation history will be reduced 
	// by the same ammount in the action list's execute methods. 
		 
	OpHist.IncSize(ActionSize);                                                                     
	
	return (AC_OK); // success				  
}                                     

/********************************************************************************************

>	Document *Action::GetWorkingDoc()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/13/95
	Returns:	The document this action is working on.
	Purpose:	Find out which document this action is working on.
	SeeAlso:	Action::GetWorkingView; Action::GetWorkingDocView

********************************************************************************************/


/********************************************************************************************

>	View *Action::GetWorkingView()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/13/95
	Returns:	The View this operation is attached to.
	Purpose:	Returns the View that is associated with this action. 
				Note that the majority of actions don't care what view they work on - most
				of them are document-based, and cause all views attached to a document to be
				updated.
	SeeAlso:	Action::GetWorkingDocView; Action::GetWorkingDoc

********************************************************************************************/


/********************************************************************************************

>	DocView *Action::GetWorkingDocView()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/13/95
	Returns:	The DocView this action is attached to, or NULL if it is not attached
				to a DocView.
	Purpose:	Returns the DocView that is associated with this action.  This will return
				NULL if the View the action is attached to is not actually a DocView (e.g.
				it may be a PrintView).  Note that the majority of actions don't care
				what view they work on - most of them are document-based, and cause all
				views attached to a document to be updated.
	SeeAlso:	Action::GetWorkingView; Action::GetWorkingDoc

********************************************************************************************/
                  
/********************************************************************************************

>	UINT32 Action::GetSize()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/7/93
	Inputs:		-
	Outputs:	-
	Returns:	The size of the action in bytes
	Purpose:	For finding the total size of the action
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

UINT32 Action::GetSize()
{
	return (Size); 
}          



/********************************************************************************************

>	virtual void Action::Dump()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/8/95
	Inputs:		-
	Outputs:	Outputs debugging information to debug display
	Returns:	-
	Purpose:	Shows debug information on the action.  Override to display information
				specific to your action
	Errors:		-
	SeeAlso:	Operation::Dump

********************************************************************************************/
void Action::Dump()
{
#if DEBUG_TREE
	TRACEALL( _T("        Action %s (Size = %d bytes)\n"), GetRuntimeClass()->GetClassName(), GetSize() );
#endif
}



/********************************************************************************************
>	BOOL Action::TransferToOtherOp(Operation* pOtherOp, ActionList* pAddActions, ActionList* pOtherActions)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/1/96
	Inputs:		pOtherOp - points to the operation to add this action to.
				pAddActions - points to the action list to add this action to.
				pOtherAction - points to the other action list.
	Outputs:	-
	Returns:	-
	Purpose:	Adds this action to the end of the specified action list.
	SeeAlso:	UndoableOperation::MergeWithPrevious
********************************************************************************************/
BOOL Action::TransferToOtherOp(Operation* pOtherOp, ActionList* pAddActions, ActionList* pOtherActions)
{
	ERROR2IF((pOtherOp==NULL) || (pAddActions==NULL) || (pOtherActions==NULL), FALSE, "NULL parameter");

	pOperation = pOtherOp;
	pOppositeActLst = pOtherActions;
	
	pAddActions->AddTail(this);

	return TRUE;
}


 
// -----------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------
// All general purpose actions here   
   
 
//------------------------------------------------------------------------------------------
// InvalidateRegionAction methods
 

/********************************************************************************************

>	InvalidateRegionAction::InvalidateRegionAction()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/9/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	InvalidateRegionAction constructor
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
 
 
InvalidateRegionAction::InvalidateRegionAction()
{
} 


/********************************************************************************************

>	ActionCode InvalidateRegionAction::Execute()   

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/8/93
	Inputs:		-
	Outputs:	ActionCode indicating if the action was successfully executed or not
	Returns:	-
	Purpose:	Executes the InvalidateRegionAction which invalidates a region which is the 
				union of the bounding rectangles of each node in the range. When the 
				includeBlobs flag is TRUE the region which gets invalidated includes the 
				objects blobs.   
	Errors:		-
	SeeAlso:	-
    
    
********************************************************************************************/

ActionCode InvalidateRegionAction::Execute()
{
	InvalidateRegionAction* InvRgnAct;  
    ActionCode ActCode;  
    // Attempt to initialise the action    
    if ((ActCode = InvalidateRegionAction::Init(pOperation,                    
								     			pOppositeActLst,  
								     			NodeRange,
												pSpread, 
												IncludeBlobs, 
						 			 			(Action**)(&InvRgnAct))) != AC_FAIL) 
		
				
	{   
		// The action was successfully initialised   

		// Invalidate each nodes bounds 
	
		Document* pDocument = pOperation->GetWorkingDoc();
		ENSURE(pDocument != 0, "There was no current document in InvalidateRegionAction" );
		
		if (pDocument != NULL)
		{
			BOOL bOldPTP = NodeRange.SetPromoteToParent(TRUE);
			NodeRenderableInk* CurrentNode = (NodeRenderableInk*)NodeRange.FindFirst();

			DocRect InvalidRgn; 
		
			while (CurrentNode != NULL)
			{
				if (CurrentNode->IsAnObject() || CurrentNode->IsPaper())
				{
					// Find the region to invalidate
					InvalidRgn = (IncludeBlobs ? 
									(((NodeRenderableInk*)CurrentNode)->GetUnionBlobBoundingRect()):
									(((NodeRenderableInk*)CurrentNode)->GetBoundingRect())
								 ); 

					pDocument->ForceRedraw(pSpread, InvalidRgn, TRUE, CurrentNode);
				}
				CurrentNode = (NodeRenderableInk*)NodeRange.FindNext(CurrentNode); 
			}

			NodeRange.SetPromoteToParent(bOldPTP);
		}

	}
	return ActCode;
}   


/********************************************************************************************

>	ActionCode InvalidateRegionAction::Init(Operation* const pOp, 
						   					ActionList* pActionList, 	
						                    Range NodeRange,  
											Spread* pSpread, 
						                    BOOL IncludeBlobs,   
						                    Action** NewAction)	

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/9/93  
	
	Inputs:		pOp: The operation to which the action should be added
				
				pActionList: The action list in the operation object
				
				NodeRange:	 The range of nodes to be invalidated

				pSpread:	 The ranges spread 
				
				IncludeBlobs: When TRUE invalidate the blob bounding rectangles of the
							  nodes in the range. 				 

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
				
				The function calls the Action::Init function passing the runtime class 
				of an InvalidateRegionAction. If successful it records the NodeTag.  
	Errors:		-
	SeeAlso:	Action::Init

********************************************************************************************/
 
ActionCode InvalidateRegionAction::Init(Operation* const pOp, 
					   					ActionList* pActionList, 	
					                    Range nodeRange, 
					                    Spread* pSpread,  
					                    BOOL includeBlobs,   
					                    Action** NewAction)	
{		
	ActionCode Ac = (Action::Init(pOp,
					 			  pActionList,
					 			  sizeof(InvalidateRegionAction), 
					 			  CC_RUNTIME_CLASS(InvalidateRegionAction), 
					 			  NewAction));
	if (*NewAction != NULL)
	{
		((InvalidateRegionAction*)(*NewAction))->NodeRange = nodeRange;
		((InvalidateRegionAction*)(*NewAction))->IncludeBlobs = includeBlobs;
		((InvalidateRegionAction*)(*NewAction))->pSpread = pSpread; 
	}
	return (Ac); 
} 

/********************************************************************************************

>	InvalidateRegionIfBgRedrawAction::InvalidateRegionIfBgRedrawAction()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/5/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	InvalidateRegionIfBgRedrawAction constructor
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
 
 
InvalidateRegionIfBgRedrawAction::InvalidateRegionIfBgRedrawAction()
{
} 


/********************************************************************************************

>	ActionCode InvalidateRegionIfBgRedrawAction::Execute()   

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/5/95
	Inputs:		-
	Outputs:	ActionCode indicating if the action was successfully executed or not
	Returns:	-
	Purpose:	Executes the InvalidateRegionAction which invalidates a region which is the 
				union of the bounding rectangles of each node in the range. When the 
				includeBlobs flag is TRUE the region which gets invalidated includes the 
				objects blobs.   
	Errors:		-
	SeeAlso:	-
    
    
********************************************************************************************/

ActionCode InvalidateRegionIfBgRedrawAction::Execute()
{
	InvalidateRegionIfBgRedrawAction* InvRgnAct;  
    ActionCode ActCode;  
    // Attempt to initialise the action    
    if ((ActCode = InvalidateRegionIfBgRedrawAction::Init(pOperation,                    
								     					  pOppositeActLst,  
								     					  NodeRange,
														  pSpread, 
														  IncludeBlobs, 
						 			 					  (Action**)(&InvRgnAct))) != AC_FAIL) 
		
				
	{   
		// The action was successfully initialised   

		if (!GetApplication()->IsBgRendering())
			return AC_OK;

//		GetApplication()->DeleteRenderRegions(DocView::GetSelected());
//		GetApplication()->BgRendering = FALSE;

		// Invalidate each nodes bounds 
	
		Document* pDocument = pOperation->GetWorkingDoc();
		ENSURE(pDocument != 0, "There was no current document in InvalidateRegionAction" );
		
		if (pDocument != NULL)
		{
			NodeRenderableInk* CurrentNode = (NodeRenderableInk*)NodeRange.FindFirst();

			DocRect InvalidRgn; 
		
			while (CurrentNode != NULL)
			{
				if (CurrentNode->IsAnObject() || CurrentNode->IsPaper())
				{
					// Find the region to invalidate
					InvalidRgn = (IncludeBlobs ? 
									(((NodeRenderableInk*)CurrentNode)->GetUnionBlobBoundingRect()):
									(((NodeRenderableInk*)CurrentNode)->GetBoundingRect())
								 ); 

					pDocument->ForceRedraw(pSpread, InvalidRgn, TRUE, CurrentNode);
				}
				CurrentNode = (NodeRenderableInk*)NodeRange.FindNext(CurrentNode); 
			}
		}

	}
	return ActCode;
}   

/********************************************************************************************

>	ActionCode InvalidateRegionIfBgRedrawAction::Init(Operation* const pOp, 
						   							  ActionList* pActionList, 	
						                   	 		  Range nodeRange, 
						                    		  Spread* pSpread,  
						                    		  BOOL includeBlobs,   
						                    		  Action** NewAction)	

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/5/95
	
	Inputs:		pOp: The operation to which the action should be added
				
				pActionList: The action list in the operation object
				
				NodeRange:	 The range of nodes to be invalidated

				pSpread:	 The ranges spread 
				
				IncludeBlobs: When TRUE invalidate the blob bounding rectangles of the
							  nodes in the range. 				 

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
				
				The function calls the Action::Init function passing the runtime class 
				of an InvalidateRegionAction. If successful it records the NodeTag.  
	Errors:		-
	SeeAlso:	Action::Init

********************************************************************************************/
 
ActionCode InvalidateRegionIfBgRedrawAction::Init(Operation* const pOp, 
					   							  ActionList* pActionList, 	
					                   	 		  Range nodeRange, 
					                    		  Spread* pSpread,  
					                    		  BOOL includeBlobs,   
					                    		  Action** NewAction)	
{		
	ActionCode Ac = (Action::Init(pOp,
					 			  pActionList,
					 			  sizeof(InvalidateRegionIfBgRedrawAction), 
					 			  CC_RUNTIME_CLASS(InvalidateRegionIfBgRedrawAction), 
					 			  NewAction));
	if (*NewAction != NULL)
	{
		((InvalidateRegionIfBgRedrawAction*)(*NewAction))->NodeRange = nodeRange;
		((InvalidateRegionIfBgRedrawAction*)(*NewAction))->IncludeBlobs = includeBlobs;
		((InvalidateRegionIfBgRedrawAction*)(*NewAction))->pSpread = pSpread; 
	}
	return (Ac); 
} 

//------------------------------------------------------------------------------------------
// HideNodeAction methods
    
/********************************************************************************************

>	HideNodeAction::HideNodeAction()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/8/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	HideNodeAction constructor
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
    
    
HideNodeAction::HideNodeAction()
  :	node(0),
	ClassOfAttributeToHide(0),
	m_nHiddenWixAttrTag(0),
	IncludeSubtreeSize(FALSE),
	TellSubtree(FALSE),
	m_bEffect(FALSE)
{
	// Empty.
}   


// BODGE, to fix problems with common attrs becoming uncommon. (i.e. The node to hide is no longer there)
// (Scaling line widths)
ActionCode ExtremeTerror(CCRuntimeClass* AttrToHide, 
						 Node* pNode, 
						 Operation* pOp,
						 ActionList* pOppositeActLst,  
						 BOOL TellSubtree, 
						 BOOL IncludeSubtreeSize)
{
	ShowNodeAction* ShwNodeAct;  
    ActionCode ActCode = AC_OK;  

	// Search pNode's subtree, to hide all attributes of AttrToHide type
	Node* pSearchNode = pNode->FindFirstDepthFirst();
	Node* pNext;
	while (pSearchNode)
	{
		pNext = pSearchNode->FindNextDepthFirst(pNode);

		if (pSearchNode->IsAnAttribute())
		{
			if (pSearchNode->GetRuntimeClass() == AttrToHide)
			{
				Node* pAttrParent = pSearchNode->FindParent();
				ERROR3IF(!pAttrParent, "Attribute has no parent");
//				if (pAttrParent && (!pAttrParent->DiscardsAttributeChildren()))
				if (pAttrParent)
				{
					if (pAttrParent->DiscardsAttributeChildren())
					{
						pSearchNode->CascadeDelete();
						delete pSearchNode;
					}
					else
					{
						// Tell the node it's subtree, we are about to hide it
						if (TellSubtree)
						{
							if (!pSearchNode->HidingNode())
								return AC_FAIL;
						}

						// The bounding rect of the parents to the node may well be changing
						if (pSearchNode->GetRuntimeClass() == CC_RUNTIME_CLASS(AttrLineWidth))
						{
							// If the node being hidden is an attribute which will effect the bounds of it's parent
							// bounded object then we must invalidatate the parents bounds
							Node* Parent = pSearchNode->FindParent(); 
							if (Parent != NULL)
							{
								if(Parent->IsKindOf(CC_RUNTIME_CLASS(NodeRenderableBounded)))
								{ 
									((NodeRenderableBounded*)Parent)->InvalidateBoundingRect(); 	
								}
							}
						}                 
			
						// Attempt to hide the node 

						NodeHidden* HideNode; 
						ALLOC_WITH_FAIL(HideNode, new NodeHidden(pSearchNode),pOp); 

						if (HideNode == NULL)
						{
							// We were unable to hide the node so fail 
							return AC_FAIL; 		
						}

						// Attempt to initialise the show node action which will show the node that 
						// we have just hidden. 
						if ((ActCode = ShowNodeAction::Init(pOp,                    
												     		pOppositeActLst,  
															HideNode, 
															IncludeSubtreeSize, 
										 			 		( Action**)(&ShwNodeAct),
										 			 		TellSubtree)) == AC_FAIL) 
						{ 
							// Show the node 
							HideNode->ShowNode(); 	                
				 		}   
			 		
				 		if (ActCode == AC_FAIL)
							return AC_FAIL;
					}
				}

			}
		}
		pSearchNode = pNext;
	}
          
	return (ActCode);                   	 
}

/********************************************************************************************

>	ActionCode HideNodeAction::Execute()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/8/93
	Inputs:		-
	Outputs:	ActionCode indicating if the action was successfully executed or not
	Returns:	-
	Purpose:	Executes the HideNodeAction which hides the node. It 
				also creates a ShowNodeAction and adds it to the opposite ActionList. 
	Errors:		-
	SeeAlso:	-
                                                        
    
********************************************************************************************/

ActionCode HideNodeAction::Execute()
{
	ShowNodeAction* ShwNodeAct;  
    ActionCode ActCode;  

	NodeHidden* HideNode; 

	// Tell our parent that we have changed so any cached info he has must be removed
	// (Deals with cases where DoInvalidateRegion has been used and so no other
	//  cache release calls have been made)
	if (node->IsBounded())
		((NodeRenderableBounded*)node)->ReleaseCached(TRUE, FALSE, TRUE, TRUE);

	// Determine which node needs hiding
	Node* NodeToHide = NULL;
	if (ClassOfAttributeToHide != NULL) // An attribute is being hidden
	{
ERROR3IF(node->DiscardsAttributeChildren(), "HideNodeAction Execute under Caret\n");
		// We need to search the children of node to find the attribute
		Node* CurrentChild = node->FindFirstChild();
		while (CurrentChild != NULL)
		{
			if (CurrentChild->GetRuntimeClass() == ClassOfAttributeToHide)
			{
#ifdef _HIDENODE_DETECTEFFECTS
				if ((m_bEffect!=0) == ((NodeAttribute*)CurrentChild)->IsEffectAttribute() &&
					(m_nHiddenWixAttrTag == 0 || m_nHiddenWixAttrTag == CurrentChild->GetTag()))
#else
				if ((m_nHiddenWixAttrTag == 0 || m_nHiddenWixAttrTag == CurrentChild->GetTag()))
#endif
				{
					// We have found the attribute to hide
					NodeToHide = CurrentChild;
					break;
				}
			}
			CurrentChild = CurrentChild->FindNext();
		}

		// This bit is v. scary
		if (NodeToHide == NULL)
		{
			// The attribute isnot where we left it last time. This could be due to a 
			// bitmap being deleted.
			ERROR3("Could not find attribute node to hide (Press continue for a fix :-)");
			return (ExtremeTerror(ClassOfAttributeToHide, node, 
								  pOperation, pOppositeActLst, 
								  TellSubtree, IncludeSubtreeSize)); 
		} 
	}
	else
	{
		NodeToHide = node; 
	}

	// Tell the node it's subtree, we are about to hide it
	if (TellSubtree)
	{
		Node* pNode = NodeToHide->FindFirstDepthFirst();
		while (pNode!=NULL)
		{
			if (!pNode->HidingNode())
				return AC_FAIL;

			// And find the next node
			pNode = pNode->FindNextDepthFirst(NodeToHide);
		}
	}

	// The bounding rect of the parents to the node may well be changing
	if (NodeToHide->IsBounded())
	{
		((NodeRenderableBounded*)NodeToHide)->InvalidateBoundingRect();
	}
	else if (NodeToHide->GetRuntimeClass() == CC_RUNTIME_CLASS(AttrLineWidth))
	{
		// If the node being hidden is an attribute which will effect the bounds of it's parent
		// bounded object then we must invalidatate the parents bounds
		Node* Parent = NodeToHide->FindParent(); 
		if (Parent != NULL)
		{
			if(Parent->IsKindOf(CC_RUNTIME_CLASS(NodeRenderableBounded)))
			{ 
				((NodeRenderableBounded*)Parent)->InvalidateBoundingRect(); 	
			}
		}
	}                 
		
	// Attempt to hide the node 
	ALLOC_WITH_FAIL(HideNode, new NodeHidden(NodeToHide),pOperation); 

	if (HideNode == NULL)
	{
		// We were unable to hide the node so fail 
		return AC_FAIL; 		
	}

	if (NodeToHide->IsSelected())
	{
		// Update the selection range 
		Camelot.UpdateSelection();
	}

	Spread* ParentOfNode = NULL;

	Layer * pLayer = NULL;
	if (NodeToHide->GetRuntimeClass() == CC_RUNTIME_CLASS(Layer))
	{
		ParentOfNode = (Spread*) HideNode->FindParent(CC_RUNTIME_CLASS(Spread));
		pLayer = (Layer*)NodeToHide;
	}

	// New 31/7/97 Neville
	// Mark the parent layer as edited as something has changed on it
	// Could use SetParentLayerAsEdited but only implemented for NodeRenderableInk at present
	if (pLayer == NULL)
		pLayer = (Layer*) HideNode->FindParent(CC_RUNTIME_CLASS(Layer));
	if (pLayer)
	{
			// Note that this layer has been edited
			pLayer->SetEdited(TRUE);
#ifdef _DEBUG
			// Tell the frame gallery to update its display of the frame
			BROADCAST_TO_ALL( LayerMsg(pLayer, LayerMsg::REDRAW_LAYER ) );
#endif
	}

    // Attempt to initialise the show node action which will show the node that 
    // we have just hidden. 
    if ((ActCode = ShowNodeAction::Init(pOperation,                    
								     	pOppositeActLst,  
										HideNode, 
										IncludeSubtreeSize, 
						 			 	( Action**)(&ShwNodeAct),
						 			 	TellSubtree)) == AC_FAIL) 
	{ 
		// Show the node 
		HideNode->ShowNode(); 	                
 	}   
 	
 	// If the node being hidden is a layer then people need to know about it
	if (ParentOfNode)
  	{
   		BROADCAST_TO_ALL(SpreadMsg(ParentOfNode,SpreadMsg::LAYERCHANGES));
  	}
          
	return (ActCode);                   	 
}    

/********************************************************************************************

>	static ActionCode HideNodeAction::Init(Operation* const pOp, 
										   ActionList* pActionList,
										   Node* NodeToHide,  
										   BOOL IncludeSubtreeSize, 	
										   Action** NewAction,
										   BOOL TellSubtree); 

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/9/93  
	
	Inputs:		pOp: The operation to which the action should be added
				
				pActionList: The action list in the operation object

				NodeToHide:	 The node to hide 

				IncludeSubtreeSize:  The size of a HideNodeAction is always 
									 sizeof(HideNodeAction), however the 
									 IncludeSubtreeSize flag is required by the
									 twin ShowNodeAction. 

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
				
				The function calls the Action::Init function passing the runtime class 
				of a HideNodeAction.
	Errors:		-
	SeeAlso:	Action::Init

********************************************************************************************/
ActionCode HideNodeAction::Init(Operation* const pOp, 
						   ActionList* pActionList,
						   Node* NodeToHide,
						   BOOL IncludeSubtreeSize,   	
						   Action** NewAction,
						   BOOL TellSubtree) 

{  
	ActionCode Ac = (Action::Init(pOp,
					 pActionList,
					 sizeof(HideNodeAction), 
					 CC_RUNTIME_CLASS(HideNodeAction), 
					 NewAction)); 

	if (*NewAction != NULL) 
	{
		// Only used for attributes
		((HideNodeAction*) (*NewAction))->ClassOfAttributeToHide = 0;

		// Do special things if the node to be hidden is an attribute
		if (NodeToHide->IsAnAttribute())
		{
			// Find the parent of the attribute node
			Node* AttributeParent = NodeToHide->FindParent();

			// We store the attribute's parent
			((HideNodeAction*)(*NewAction))->node = AttributeParent; 
			ERROR2IF(AttributeParent == NULL, AC_FAIL, "Attribute has no parent"); 

			// It is bad if the attributes parent is an attribute, attributes are not safe anchors
			// they can be deleted.	
			ERROR2IF(AttributeParent->IsAnAttribute(), AC_FAIL, "Attribute's parent is an attribute"); 	

			// Store the runtime class of the attribute so that we can find it
			((HideNodeAction*)(*NewAction))->ClassOfAttributeToHide = NodeToHide->GetRuntimeClass(); 

			// If it's also a Wix attribute then store its tag so it can be uniquely identified
			// when the attribute must be hidden again.
#ifdef _HIDENODE_DETECTEFFECTS
			if (NodeToHide->IS_KIND_OF(TemplateAttribute))
#else
			if (NodeToHide->IS_KIND_OF(TemplateAttribute) || ((NodeAttribute*)NodeToHide)->IsEffectAttribute())
#endif
				((HideNodeAction*) (*NewAction))->m_nHiddenWixAttrTag = NodeToHide->GetTag();
			else
				((HideNodeAction*) (*NewAction))->m_nHiddenWixAttrTag = 0;

			((HideNodeAction*) (*NewAction))->m_bEffect = ((NodeAttribute*)NodeToHide)->IsEffectAttribute();
		}
		else
		{
			((HideNodeAction*)(*NewAction))->node = NodeToHide;
		}

		((HideNodeAction*)(*NewAction))->IncludeSubtreeSize = IncludeSubtreeSize; 
		((HideNodeAction*)(*NewAction))->TellSubtree = TellSubtree; 
	}
				  
	return (Ac); 
}

void HideNodeAction::RecordTag(Node* NodeToHide)
{
	if (NodeToHide->IsAnAttribute())
	{
		// Find the parent of the attribute node
//		Node* AttributeParent = NodeToHide->FindParent();

		// If it's also a Wix attribute then store its tag so it can be uniquely identified
		// when the attribute must be hidden again.
#ifdef _HIDENODE_DETECTEFFECTS
		if (NodeToHide->IS_KIND_OF(TemplateAttribute))
#else
		if (NodeToHide->IS_KIND_OF(TemplateAttribute) || ((NodeAttribute*)NodeToHide)->IsEffectAttribute())
#endif
			m_nHiddenWixAttrTag = NodeToHide->GetTag();
		else
			m_nHiddenWixAttrTag = 0;

		m_bEffect = ((NodeAttribute*)NodeToHide)->IsEffectAttribute();
	}
}

//------------------------------------------------------------------------------------------						   
// ShowNodeAction methods
    
/********************************************************************************************

>	ShowNodeAction::ShowNodeAction()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/9/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	ShowNodeAction constructor
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
    
ShowNodeAction::ShowNodeAction()
{
}
             
ShowNodeAction::~ShowNodeAction() 
{
}   

/********************************************************************************************

>	virtual void ShowNodeAction::Slaughter()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/7/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	destructor which gets called when an operation is deleted  
	Errors:		-
	SeeAlso:	Action::Slaughter

********************************************************************************************/

void ShowNodeAction::Slaughter() 
{               
	// If the node which the NodeHidden hides has no parent and the node is not refferenced 
	// by any other NodeHidden nodes, then the node can safely be deleted.  

	// On the other-hand if the node which the NodeHidden hides has a parent, then it must not
	// be deleted. This situation arises when we have moved a node and the NodeHidden is simply
	// used as a place holder for when we undo/redo.  
	                  
	// First delete the NodeHidden                                              
	Node* Hidden = node->HiddenNd; // Remember the node which it hides !
	node->CascadeDelete(); // Simply unlinks the node from the tree, node should 
						   // never have any children because it's a NodeHidden.  
	delete (node);  
	
	Hidden->DecHiddenCnt(); // Decrement the number of refferences to the hidden node.            
	                  
	if (Hidden->FindParent() == NULL)
	{
		// The node which is hidden has no parent but it could be reffered to by other 
		// hidden nodes which as yet have not been deleted.                              
		
		if (Hidden->GetHiddenCnt() == 0) // The node is not hidden by any NodeHidden nodes.    
		{
			Hidden->CascadeDelete(); // Delete the children of the hidden node.
			delete (Hidden); 		 // Delete the hidden node itself 
		}
		
	} 
	Action::Slaughter(); // Call base class to destroy this 
}   
 
/********************************************************************************************

>	virtual ActionCode ShowNodeAction::Execute()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/8/93
	Inputs:		-
	Outputs:	ActionCode indicating if the action was successfully executed or not
	Returns:	-
	Purpose:	Executes the ShowNodeAction which un-hides the node. It 
				also creates a HideNodeAction and adds it to the opposite ActionList. 
	Errors:		-
	SeeAlso:	-
    
    
********************************************************************************************/
 
ActionCode ShowNodeAction::Execute()
{  
	HideNodeAction* HideNodeAct;  
    ActionCode ActCode;  

	// If the HideNodeAction is for a NodeAttribute then this node must have a parent before
	// we call the HideNodeAction's Init method. However because it is hidden it does not have one at
	// the moment. So we use the SetParentDangerous function to give it the parent of the NodeHidden.
	// This is horrible but it is the safest thing to do. If we call node->ShowNode before calling
	// HideNodeAction::Init then we are shafted if this function fails. (The action will not be atomic)
  
  	node->HiddenNd->SetParentDangerous(node->FindParent()); 

     // Attempt to initialise the hide node action  
    if ((ActCode = HideNodeAction::Init(pOperation,                    
								     	pOppositeActLst, 
								     	node->HiddenNd, 
								     	IncludeSubtreeSize,  
						 			 	( Action**)(&HideNodeAct),
						 			 	TellSubtree)) != AC_FAIL) 
	{                 
		Node* BackFromTheDead = node->ShowNode();
		Node* Parent = BackFromTheDead->FindParent(); 
ERROR3IF(Parent->DiscardsAttributeChildren(), "ShowNodeAction Execute under Caret\n");

		// Urgh - we can only tell whether an attribute is an effect attr when it's in the tree
		// And we need to give the HideNodeAction the Tag of the attribute if it is an
		// Effect Attribute so that HideNodeExecute finds it correctly...
		if (HideNodeAct)
			HideNodeAct->RecordTag(BackFromTheDead);

		// Tell the node and it's subtree, we have re-shown it
		if (TellSubtree)
		{
			Node* pNode = BackFromTheDead->FindFirstDepthFirst();
			while (pNode!=NULL)
			{
				if (!pNode->ShowingNode())
					return AC_FAIL;

				// And find the next node
				pNode = pNode->FindNextDepthFirst(BackFromTheDead);
			}
		}

		if (BackFromTheDead->IsSelected())
		{
			// Update the selection range 
			Camelot.UpdateSelection();
		}

		// If the node being shown is a layer then people need to know about it
		Layer * pLayer = NULL;
		if (BackFromTheDead->GetRuntimeClass() == CC_RUNTIME_CLASS(Layer))
  		{
    		Spread* pSpread = (Spread*) BackFromTheDead->FindParent(CC_RUNTIME_CLASS(Spread)); 
   			BROADCAST_TO_ALL(SpreadMsg(pSpread,SpreadMsg::LAYERCHANGES));
			pLayer = (Layer*)BackFromTheDead;
	  	}

		// Mark the bounding rects as out of date
		if (BackFromTheDead->IsBounded())
		{
			// Since this nodes bounding rect may well be invalid while it was hidden, we
			// have to validate it to be sure the invalidation ripples up through the tree
			((NodeRenderableBounded*)BackFromTheDead)->ValidateBoundingRect();
			((NodeRenderableBounded*)BackFromTheDead)->InvalidateBoundingRect();
		}
		else if (BackFromTheDead->GetRuntimeClass() == CC_RUNTIME_CLASS(AttrLineWidth))
		{
			// If the node being shown is an attribute which will effect the bounds of it's parent
			// bounded object then we must invalidatate the parent's bounds
			if (Parent != NULL)
			{
				if(Parent->IsKindOf(CC_RUNTIME_CLASS(NodeRenderableBounded)))
				{ 
					((NodeRenderableBounded*)Parent)->InvalidateBoundingRect(); 	
				}
			}
		}

		// New 31/7/97 Neville
		// Mark the parent layer as edited as something has changed on it
		// Could use SetParentLayerAsEdited but only implemented for NodeRenderableInk at present
		if (pLayer == NULL)
			pLayer = (Layer*) BackFromTheDead->FindParent(CC_RUNTIME_CLASS(Layer));
		if (pLayer)
		{
				// Note that this layer has been edited
				pLayer->SetEdited(TRUE);
#ifdef _DEBUG
				// Tell the frame gallery to update its display of the frame
				BROADCAST_TO_ALL( LayerMsg(pLayer, LayerMsg::REDRAW_LAYER ) );
#endif

	// Tell our parent that we have changed so any cached info he has must be removed
	// (Deals with cases where DoInvalidateRegion has been used and so no other
	//  cache release calls have been made)
	if (BackFromTheDead->IsBounded())
		((NodeRenderableBounded*)BackFromTheDead)->ReleaseCached(TRUE, FALSE, TRUE, TRUE);
	else if (Parent->IsBounded())
		((NodeRenderableBounded*)Parent)->ReleaseCached(TRUE, FALSE, TRUE, TRUE);

		}
	}
	else
	{
	  	node->HiddenNd->SetParentDangerous(NULL); // The node was never un-hidden so we must give it a NULL parent

	}                 
	return (ActCode);                   
}     


/********************************************************************************************
>	static ActionCode ShowNodeAction::Init(Operation* const pOp, 
						   				   ActionList* pActionList, 
						   				   NodeHidden* HiddenToShow, 
						  				   BOOL IncludeSubtreeSize, 	
						   				   Action** NewAction,
						   				   BOOL TellSubtree) 

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/9/93  
	
	Inputs:		pOp: 				The operation to which the action should be added
				
				pActionList: 		The action list in the operation object
				
				HiddenToShow: 		The hidden node to show when the action is executed

				IncludeSubtreeSize: This flag should be set if the action size 
									should include the size of the subtree which is 
									hidden. 
							 
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
				
				The function calls the Action::Init function passing the runtime class 
				of a ShowNodeAction.
	Errors:		-
	SeeAlso:	Action::Init

********************************************************************************************/

ActionCode ShowNodeAction::Init(Operation* const pOp, 
						   		ActionList* pActionList, 
						   		NodeHidden* HiddenToShow, 
						   		BOOL IncludeSubtreeSize, 	
						   		Action** NewAction, 
								BOOL TellSubtree)
{ 
	// Determine the size of the action 
	UINT32 ActionSize; 

	ActionSize = sizeof(ShowNodeAction); 
	
	// If the subtree being hidden is to be considered to belong to the operation history 
	// then the ActionSize must include the size of the hidden subtree. 

	if (IncludeSubtreeSize)
	{
		ActionSize += ( HiddenToShow->HiddenNd->GetSubtreeSize());
		//if (IsUserName("Simon"))
		//	TRACE( _T("ShowNodeActionSize = %lu"), ActionSize);  
		
	}

	ActionCode Ac = (Action::Init(pOp,
					 pActionList,
					 ActionSize,
					 CC_RUNTIME_CLASS(ShowNodeAction), 
					 NewAction));
	if (*NewAction != NULL) 
	{
		((ShowNodeAction*)(*NewAction))->node = HiddenToShow;
		((ShowNodeAction*)(*NewAction))->IncludeSubtreeSize = IncludeSubtreeSize; 
		((ShowNodeAction*)(*NewAction))->TellSubtree = TellSubtree; 
	}

	return (Ac); 
} 
 



//------------------------------------------------------------------------------------------
// UnApplyAction methods
    
/********************************************************************************************

>	UnApplyAction::UnApplyAction()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/07/2005
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	UnApplyAction constructor
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
    
    
UnApplyAction::UnApplyAction()
{
	m_pApplyNode = NULL;
	m_pAttribute = NULL;
	m_nAttrTag = 0;
	IncludeSubtreeSize = FALSE;
	TellSubtree = FALSE;
}

UnApplyAction::~UnApplyAction()
{
	if (m_pAttribute)
	{
		delete m_pAttribute;
		m_pAttribute = NULL;
	}
}


/********************************************************************************************

>	ActionCode UnApplyAction::Execute()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/07/2005
	Inputs:		-
	Outputs:	ActionCode indicating if the action was successfully executed or not
	Returns:	-
	Purpose:	Executes the UnApplyAction which hides the node. It 
				also creates a ShowNodeAction and adds it to the opposite ActionList. 
	Errors:		-
	SeeAlso:	-
                                                        
    
********************************************************************************************/

ActionCode UnApplyAction::Execute()
{
	ApplyAction* pApplyAct;  
    ActionCode ActCode;  

//	NodeHidden* HideNode = NULL;

    // Attempt to initialise the apply action which will re-apply the node that 
    // we are about to unapply.
    if ((ActCode = ApplyAction::Init(pOperation,
								     	pOppositeActLst,
										m_pApplyNode,
										m_pAttribute,
										IncludeSubtreeSize,
						 			 	( Action**)(&pApplyAct),
						 			 	TellSubtree)) == AC_FAIL)
		return ActCode;
 	
	// Tell our parent that we have changed so any cached info he has must be removed
	// (Deals with cases where DoInvalidateRegion has been used and so no other
	//  cache release calls have been made)
	if (m_pApplyNode->IsBounded())
		((NodeRenderableBounded*)m_pApplyNode)->ReleaseCached(TRUE, FALSE, TRUE, TRUE);

	// Determine which node needs hiding
	CCRuntimeClass* pClass = m_pAttribute->GetRuntimeClass();

	// We need to search the children of node to find the attribute
	// Note that if this action has recorded a Tag, that attribute should not have been
	// mutated before trying to execute this action. If it has then the mutated attribute
	// will have a new tag and will not be found. c.f. interactive dragging...
	Node* NodeToHide = m_pApplyNode->FindFirstChild();
	BOOL bLookingForEffect = m_pApplyNode->IsAnObject() && ((NodeRenderableInk*)m_pApplyNode)->IsValidEffectAttr(m_pAttribute);
	while (NodeToHide != NULL)
	{
		if (NodeToHide->GetRuntimeClass() == pClass &&
			(m_nAttrTag == 0 || m_nAttrTag == NodeToHide->GetTag()) &&
			(bLookingForEffect == (NodeToHide->IsAnAttribute() && ((NodeAttribute*)NodeToHide)->IsEffectAttribute()))
			)
			// We have found the attribute to hide
			break;

		NodeToHide = NodeToHide->FindNext();
	}

	if (NodeToHide != NULL)
	{
		// If the node being hidden is an attribute which will effect the bounds of it's parent
		// bounded object then we must invalidate the parents bounds
		if (m_pApplyNode->IsKindOf(CC_RUNTIME_CLASS(NodeRenderableBounded)))
			((NodeRenderableBounded*)m_pApplyNode)->InvalidateBoundingRect(); 	
			
		NodeToHide->CascadeDelete();
		delete NodeToHide;
		NodeToHide = NULL;

		// Mark the parent layer as edited as something has changed on it
		// Could use SetParentLayerAsEdited but only implemented for NodeRenderableInk at present
		Layer* pLayer = (Layer*) m_pApplyNode->FindParent(CC_RUNTIME_CLASS(Layer));
		if (pLayer)
			pLayer->SetEdited(TRUE);

	} 

	return ActCode;
}    

/********************************************************************************************

>	static ActionCode UnApplyAction::Init(Operation* const pOp, 
										   ActionList* pActionList,
										   Node* pActionApplyNode,
										   NodeAttribute* pActionAttribute,
										   BOOL IncludeSubtreeSize, 	
										   Action** NewAction,
										   BOOL TellSubtree); 

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/07/2005
	Inputs:		pOp: The operation to which the action should be added
				
				pActionList: The action list in the operation object

				NodeToHide:	 The node to hide 

				IncludeSubtreeSize:  The size of a UnApplyAction is always 
									 sizeof(UnApplyAction), however the 
									 IncludeSubtreeSize flag is required by the
									 twin ShowNodeAction. 

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
				
				The function calls the Action::Init function passing the runtime class 
				of a UnApplyAction.
	Errors:		-
	SeeAlso:	Action::Init

********************************************************************************************/
ActionCode UnApplyAction::Init(Operation* const pOp, 
						   ActionList* pActionList,
						   Node* pActionApplyNode,
						   NodeAttribute* pActionAttribute,
						   BOOL IncludeSubtreeSize,   	
						   Action** NewAction,
						   BOOL TellSubtree) 

{  
	ActionCode Ac = (Action::Init(pOp,
					 pActionList,
					 sizeof(UnApplyAction) + sizeof(pActionAttribute),
					 CC_RUNTIME_CLASS(UnApplyAction), 
					 NewAction)); 

	if (*NewAction != NULL) 
	{
		UnApplyAction* pAction = (UnApplyAction*)(*NewAction);

		// Store the runtime class of the attribute so that we can find it
		pAction->m_pApplyNode = pActionApplyNode;
		pAction->m_pAttribute = (NodeAttribute*)pActionAttribute->SimpleCopy();
		pAction->IncludeSubtreeSize = IncludeSubtreeSize; 
		pAction->TellSubtree = TellSubtree; 
		pAction->RecordTag(pActionAttribute);
	}
				  
	return (Ac); 
}

void UnApplyAction::RecordTag(NodeAttribute* pAttr)
{
	// If it's also a Wix attribute then store its tag so it can be uniquely identified
	// when the attribute must be hidden again.
//	if (pAttr->IS_KIND_OF(TemplateAttribute) || pAttr->IsEffectAttribute())
	if (pAttr->IS_KIND_OF(TemplateAttribute))
		m_nAttrTag = pAttr->GetTag();
	else
		m_nAttrTag = 0;
}

//------------------------------------------------------------------------------------------						   
// ApplyAction methods
    
/********************************************************************************************

>	ApplyAction::ApplyAction()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/07/2005
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	ApplyAction constructor
	Errors:		-
	SeeAlso:	-
	Notes:		Phil, 26/07/2005
				What a mess! Attribute application needs to be rewritten
				* ShowNode/HideNode to control attribute undo is suspect because attribute
				  optimisation can remove attributes outside of the undo system.
				* UnapplyAction and ApplyAction are implemented to overcome this limitation
				  but really, they are the way attribute application should always have been
				  recorded in undo, given the above.
				* OpRepeatApplyAttribToSelected is all wrong. It makes quick, non-undoable
				  changes to a set of attributes after OpApplyAttribToSelected has applied
				  them. This means that attributes may be optimised away AFTER the undo
				  actions have been recorded. It should work the way most other solid dragging
				  works - create a temporary set of attrs for fast manipulation druing dragging
				  then remove them and do a one-shot undoable apply when the drag ends.

********************************************************************************************/
    
ApplyAction::ApplyAction()
{
	m_pApplyNode = NULL;
	m_pAttribute = NULL;
}

ApplyAction::~ApplyAction() 
{
	if (m_pAttribute)
	{
		delete m_pAttribute;
		m_pAttribute = NULL;
	}
}   

/********************************************************************************************

>	virtual ActionCode ApplyAction::Execute()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/07/2005
	Inputs:		-
	Outputs:	ActionCode indicating if the action was successfully executed or not
	Returns:	-
	Purpose:	Executes the ApplyAction which un-hides the node. It 
				also creates a UnApplyAction and adds it to the opposite ActionList. 
	Errors:		-
	SeeAlso:	-
    
    
********************************************************************************************/
 
ActionCode ApplyAction::Execute()
{  
	UnApplyAction* pUnApplyAction;  
    ActionCode ActCode;  

     // Attempt to initialise the hide node action  
    if ((ActCode = UnApplyAction::Init(pOperation,                    
								     	pOppositeActLst,
										m_pApplyNode,
								     	m_pAttribute,
								     	IncludeSubtreeSize,  
						 			 	( Action**)(&pUnApplyAction),
						 			 	TellSubtree)) != AC_FAIL) 
	{
		ERROR3IF(m_pAttribute==NULL, "ApplyAction::Execute has no attribute!\n");
		if (m_pAttribute)
		{
			NodeAttribute* pAttr = (NodeAttribute*)m_pAttribute->SimpleCopy();
			BOOL bEffectRootOnly = (m_pApplyNode->IsAnObject() && ((NodeRenderableInk*)m_pApplyNode)->IsValidEffectAttr(pAttr));
			if (bEffectRootOnly)
				pAttr->AttachNode(m_pApplyNode, LASTCHILD);
			else
				pAttr->AttachNode(m_pApplyNode, FIRSTCHILD);

			if (pUnApplyAction)
				pUnApplyAction->RecordTag(pAttr);
		}

		// If the node being shown is an attribute which will effect the bounds of it's parent
		// bounded object then we must invalidatate the parent's bounds
		if (m_pApplyNode && m_pApplyNode->IsKindOf(CC_RUNTIME_CLASS(NodeRenderableBounded)))
			((NodeRenderableBounded*)m_pApplyNode)->InvalidateBoundingRect();

		// New 31/7/97 Neville
		// Mark the parent layer as edited as something has changed on it
		// Could use SetParentLayerAsEdited but only implemented for NodeRenderableInk at present
		Layer* pLayer = (Layer*)m_pApplyNode->FindParent(CC_RUNTIME_CLASS(Layer));
		if (pLayer)
			pLayer->SetEdited(TRUE);

		// Tell our parent that we have changed so any cached info he has must be removed
		// (Deals with cases where DoInvalidateRegion has been used and so no other
		//  cache release calls have been made)
		if (m_pApplyNode->IsBounded())
			((NodeRenderableBounded*)m_pApplyNode)->ReleaseCached(TRUE, FALSE, TRUE, TRUE);
	}

	return (ActCode);                   
}     


/********************************************************************************************
>	static ActionCode ApplyAction::Init(Operation* const pOp, 
						   				   ActionList* pActionList, 
						   				   Node* pActionApplyNode, 
										   NodeAttribute* pActionAttribute,
						  				   BOOL IncludeSubtreeSize, 	
						   				   Action** NewAction,
						   				   BOOL TellSubtree) 

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/07/2005
	Inputs:		pOp: 				The operation to which the action should be added
				
				pActionList: 		The action list in the operation object
				
				HiddenToShow: 		The hidden node to show when the action is executed

				IncludeSubtreeSize: This flag should be set if the action size 
									should include the size of the subtree which is 
									hidden. 
							 
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
				
				The function calls the Action::Init function passing the runtime class 
				of a ShowNodeAction.
	Errors:		-
	SeeAlso:	Action::Init

********************************************************************************************/

ActionCode ApplyAction::Init(Operation* const pOp, 
						   		ActionList* pActionList, 
						   	    Node* pActionApplyNode, 
							    NodeAttribute* pActionAttribute,
						   		BOOL IncludeSubtreeSize, 	
						   		Action** NewAction, 
								BOOL TellSubtree)
{ 
	// Determine the size of the action 
	ActionCode Ac = (Action::Init(pOp,
					 pActionList,
					 sizeof(ApplyAction) + sizeof(pActionAttribute),
					 CC_RUNTIME_CLASS(ApplyAction), 
					 NewAction));
	if (*NewAction != NULL) 
	{
		ApplyAction* pAction = (ApplyAction*)(*NewAction);

		pAction->m_pApplyNode		= pActionApplyNode;
		pAction->m_pAttribute		= (NodeAttribute*)pActionAttribute->SimpleCopy();
		pAction->IncludeSubtreeSize = IncludeSubtreeSize; 
		pAction->TellSubtree		= TellSubtree; 
	}

	return (Ac); 
} 
 



//------------------------------------------------------------------------------------------						   
// RestoreSelectionsAction methods
    
/********************************************************************************************

>	RestoreSelectionsAction::RestoreSelectionsAction()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/9/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	RestoreSelectionsAction constructor
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
    
RestoreSelectionsAction::RestoreSelectionsAction()
{
}

RestoreSelectionsAction::~RestoreSelectionsAction() 
{ 
	// Note that the SelectionState object is not deleted in the destructor. This is because 
	// it will be used by the spawned RestoreSelectionAction generated in the Execute method.
}   

/********************************************************************************************

>	virtual RestoreSelectionsAction::Slaughter()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/7/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	All RestoreSelections actions have a pointer to a selection state. This
				selection state is passed on to the actions twin, and therefore should
				not be deleted in the actions destructor. The slaughter method is responsible
				for deleting the SelState. 

				If this is a togling action, and ToggleStatus = FALSE then the SelState 
				is not deleted, it will be deleted by the other RestoreSelectionsAction
				with ToggleStatus = TRUE. 

	Errors:		-
	SeeAlso:	Action::Slaughter

********************************************************************************************/
                      
void RestoreSelectionsAction::Slaughter() 
{   
	// Determine if the SelState should be deleted here or in another RestoreSelectionsAction
	if (SelStateShared)
	{
		ENSURE(toggle, "If the SelState is shared then the toggle flag should be set"); 
		if (toggleStatus)
		{
			ENSURE (SelState != NULL, "Trying to delete a NULL SelectionState"); 
			delete SelState; 
		}
	} 
	else // The SelState is not shared so we can safely delete the selection state
	{
		delete SelState; 
	}
	Action::Slaughter(); 
}   
 
/********************************************************************************************

>	virtual ActionCode RestoreSelectionsAction::Execute()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/8/93
	Inputs:		-
	Outputs:	ActionCode indicating if the action was successfully executed or not
	Returns:	-
	Purpose:	Executes the RestoreSelectionsAction which restores the selection state. It also
				creates another identical RestoreSelectionsAction and adds this to the  
		 		opposite ActionList.      
	Errors:		-
	SeeAlso:	-
    
    
********************************************************************************************/
 
ActionCode RestoreSelectionsAction::Execute()
{  
	RestoreSelectionsAction* RestoreAct;  
    ActionCode ActCode;  
    // Attempt to initialise the action    
    if ((ActCode = RestoreSelectionsAction::Init(pOperation,                    
								     			 pOppositeActLst,  
								     			 SelState, 
												 toggle, 
												 !toggleStatus,   // Toggle it
												 SelStateShared, 
												 RenderStartBlobs,
												 RenderEndBlobs,
												 !StartRestore,   // Will become the last restore !
						 			 			 ( Action**)(&RestoreAct))) != AC_FAIL) 
	{   
		// The action was successfully initialised. If this is a toggle 
		// action then only restore the selection state when the toggleStatus is
		// true. 
		if ((!toggle) || (toggleStatus))  // Determine if we have to do anything 
		{   
			// Only render the blobs if StartRestore is FALSE and either
			// a. RenderStartBlobs is TRUE and this action is an undo action 
			// b. RenderEndBlobs is TRUE and this is a redo action

			BOOL RenderBlobs = FALSE;
							 
			if (!StartRestore)
			{
				if  (pOppositeActLst == pOperation->GetUndoActionList())
				{
					// This action is a redo action
					RenderBlobs = RenderEndBlobs;
				}

				else
				{
					// This action is an undo action
					RenderBlobs = RenderStartBlobs;
				}
			}

			// Turn blob rendering on if this is the last restore
			//if (!StartRestore)
			//{
			//	Camelot.GetBlobManager()->BlobRenderingOn(FALSE); 
			//}

			// Always remove the selection blobs when StartRestore is TRUE           
			SelState->Restore(RenderBlobs, StartRestore); // Restore selections   
			
			// Turn blob rendering off if this is the first restore
			//if (StartRestore)
			//{
			//	Camelot.GetBlobManager()->BlobRenderingOff(FALSE); 
			//}
 
		}
	}             
	return (ActCode);                
}     


/********************************************************************************************

>	static ActionCode RestoreSelectionsAction::Init(Operation* const pOp, 
									   				ActionList* pActionList, 	
						  			   				SelectionState* SelState,
						  			   				BOOL Toggle, 
						  			   				BOOL ToggleStatus, 
						  			   				BOOL SelStateShared,  
													BOOL RenderStartBlobs,
													BOOL RenderEndBlobs,
													BOOL StartRestore, 
						   			   				Action** NewAction)	

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/9/93  
	
	Inputs:		pOp: The operation to which the action should be added
				
				pActionList: The action list in the operation object
				
				SelState:	 Pointer to a selection State - this gets destroyed 
							 in the actions slaughter method. 

				Toggle:		 When TRUE this flag indicates that the selection 
							 state should only be restored when the ToggleStatus
							 is TRUE. The ToggleStatus of the actions twin = 
							 !ToggleStatus. This mechanism allows two RestoreSelectionActions
							 to be created for an operation, only one of which does anything. 
							 Usually an operation creates two RestoreSelectionsActions one 
							 at the head and one at the tail of the operations action list.
							 When undoing it is desirable that the tail 
							 RestoreSelectionsAction restores the selection state and that
							 the head RestoreSelectionsAction does nothing (except create 
							 a twin). when redoing the RestoreSelectionsAction which
							 was originally at the head of the undo action list is now at 
							 the tail of the redo list and so it needs to restore the 
							 selections.Likewise the RestoreSelectionsAction which was at 
							 the tail of the undo action list will be at the head of the redo
							 action list and so it is this actions turn to do nothing.


							    
				ToggleStatus:  The initial toggle status

				SelStateShared:	This flag indicates that the SelState is shared by a pair of 
								RestoreSelActions. If this flag is set then the Toggle
								flag must also be set.  

				RenderStartBlobs: Should the selection blobs be rendered for the start selection
								  state ?

				RenderEndBlobs:	  Should the selection blobs be rendered for the end selection 
								  state ?

				StartRestore:  TRUE indicates that this is the first restore action executed when 
							   the operation is undone. 

							 
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
				
				The function calls the Action::Init function passing the runtime class 
				of a RestoreSelectionsAction.
	Errors:		-
	SeeAlso:	Action::Init

********************************************************************************************/

ActionCode RestoreSelectionsAction::Init(Operation* const pOp, 
						   				 ActionList* pActionList, 	
						   				 SelectionState* SelState,
						   				 BOOL Toggle, 
						   				 BOOL ToggleStatus,
						   				 BOOL SelStateShared,
						   				 BOOL RenderStartBlobs, 
										 BOOL RenderEndBlobs, 
 						   				 BOOL StartRestore,  
						   				 Action** NewAction)       
{    
	// If the SelState is shared then Toggle must be TRUE. This is so that we know when
	// to delete the SelState. 
	ENSURE( (!SelStateShared || Toggle), "Invalid flags passed to RestoreSelectionsAction"); 
    
	// Try to allocate memory for the action 
	ActionCode Ac = (Action::Init(pOp,
					 pActionList,
					 sizeof(RestoreSelectionsAction) + SelState->GetSize(),
					 CC_RUNTIME_CLASS(RestoreSelectionsAction), 
					 NewAction));    
	if (*NewAction != NULL) 
	{
		((RestoreSelectionsAction*)(*NewAction))->SelState = SelState;
		((RestoreSelectionsAction*)(*NewAction))->toggle = Toggle; 
		((RestoreSelectionsAction*)(*NewAction))->toggleStatus = ToggleStatus; 
		((RestoreSelectionsAction*)(*NewAction))->SelStateShared = SelStateShared; 
		((RestoreSelectionsAction*)(*NewAction))->RenderStartBlobs = RenderStartBlobs; 
		((RestoreSelectionsAction*)(*NewAction))->RenderEndBlobs = RenderEndBlobs; 
		((RestoreSelectionsAction*)(*NewAction))->StartRestore = StartRestore; 

	}
	return (Ac); 
}


//------------------------------------------------------------------------------------------						   
// SelectDeselectAction methods
    
/********************************************************************************************

>	SelectDeselectAction::SelectDeselectAction()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/9/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	SelectDeselectAction constructor
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
    
SelectDeselectAction::SelectDeselectAction()
{
}
             
SelectDeselectAction::~SelectDeselectAction() 
{
}
 
/********************************************************************************************

>	virtual ActionCode SelectDeselectAction::Execute()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/8/93
	Inputs:		-
	Outputs:	ActionCode indicating if the action was successfully executed or not
	Returns:	-
	Purpose:	Executes the SelectDeselectAction which will render node's selection blobs 
				(which could remove them), and set it's selection state. If node is 
				currently selected then the node is deselected. If node is currently 
				deselected it is selected. It creates a second SelectDeselectAction for the 
				node and adds it to the opposite action list.  
	Errors:		-
	SeeAlso:	-
    
    
********************************************************************************************/
 
ActionCode SelectDeselectAction::Execute()
{  
	SelectDeselectAction* SelDeAct;  
    ActionCode ActCode;  
    // Attempt to initialise the action    
    if ((ActCode = SelectDeselectAction::Init(pOperation,                    
								     		  pOppositeActLst,  
								     		  node,
											  Parent,
						 			 		  ( Action**)(&SelDeAct))) != AC_FAIL) 
	{   
		// The action was successfully initialised   
		ENSURE((node->IsKindOf(CC_RUNTIME_CLASS(NodeRenderableInk))), 
			"Cannot select/deselect a non NodeRenderableInk node"); 
			
		if (node->IsSelected())  
			((NodeRenderableInk*)node)->DeSelect(FALSE); // DeSelect   
		else 
			((NodeRenderableInk*)node)->Select(FALSE); 	// Select 
	}             
	return (ActCode);               	                    
}     


/********************************************************************************************

>	static ActionCode SelectDeselectAction::::Init(Operation* const pOp, 
												   ActionList* pActionList, 	
						 				  	 	   UINT32 ActionSize, 
						  						   Node* SelDeNode, 
						   						   Action** NewAction)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/9/93  
	
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
				
				The function calls the Action::Init function passing the runtime class 
				of a ShowNodeAction.
	Errors:		-
	SeeAlso:	Action::Init

********************************************************************************************/
		
ActionCode SelectDeselectAction::Init(Operation* const pOp, 
								ActionList* pActionList, 	
						  		Node* SelDeNode,
								Spread* pSpread,
						   		Action** NewAction)
{ 
	ActionCode Ac = (Action::Init(pOp,
					 pActionList,
					 sizeof(SelectDeselectAction), 
					 CC_RUNTIME_CLASS(SelectDeselectAction), 
					 NewAction)); 
					   
	if (Ac != AC_FAIL)
		if (*NewAction != NULL)
		{
			((SelectDeselectAction*)(*NewAction))->node = SelDeNode;
			((SelectDeselectAction*)(*NewAction))->Parent = pSpread;
		}
			             
	return (Ac); 
} 
 
//------------------------------------------------------------------------------------------						   
// TransformNodeAction methods
    
/********************************************************************************************

>	TransformNodeAction::TransformNodeAction()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/9/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	TransformNodeAction constructor
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
    
TransformNodeAction::TransformNodeAction()
{
	m_pNodeList = NULL;
	pTrans = NULL;
}
             
TransformNodeAction::~TransformNodeAction() 
{
	if (m_pNodeList)
	{
		m_pNodeList->DeleteAll();
		delete m_pNodeList;
		m_pNodeList = NULL;
	}

	if (pTrans)
	{
		delete pTrans;
		pTrans = NULL;
	}
}   


/********************************************************************************************

>	virtual ActionCode TransformNodeAction::Execute()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/8/93
	Inputs:		-
	Outputs:	ActionCode indicating if the action was successfully executed or not
	Returns:	-
	Purpose:	Executes the TransformNodeAction which transforms the range of nodes. It also 
				creates another TransformNodeAction passing it the inverse of the 
				transform 
	Errors:		-
	SeeAlso:	-
    
    
********************************************************************************************/
ActionCode TransformNodeAction::Execute()
{  
	// DMc - changed 17/8/99
	// It didn't work in its old state with compound nodes as the node range seemed to
	// be corrupted in the transformations
	// Thus, made it more robust by having a node list of the range which is first passed in

	// Further comment. Looks like things have been fiddled with too much to rely on the
	// range to be correct when redoing the op (check beveled shapes with text stories)
	// So I tidied up Daves node lists and corrected his transform matrix (sjk 9/8/00)
	TransformNodeAction* TransformNodeAct;  
    ActionCode ActCode;  

	Trans2DMatrix * pTransform = new Trans2DMatrix((const Trans2DMatrix&) *pTrans);
	pTransform->Invert();
    
    // Create an action to transform the node back to its initial position
	if ( (ActCode = TransformNodeAction::Init(pOperation, 
			 			    			  	  pOppositeActLst, 
								   			  NodeRange, 
								   			  pTransform,
											  m_pNodeList,
							  	   			  (Action**)(&TransformNodeAct)))
		!= AC_FAIL)  
	{   
		NodeListItem * pItem = (NodeListItem *)m_pNodeList->GetHead();
		Node * pNode = NULL;

		while (pItem)
		{
			pNode = pItem->pNode;
			
			if (((NodeRenderable *)pNode)->IsNodeRenderableClass())
			{		
				((NodeRenderable *)pNode)->Transform(*pTrans);				
				// set the parent layer as having being edited
				pNode->SetParentLayerAsEdited();
			}

			pItem = (NodeListItem *)m_pNodeList->GetNext(pItem);
		}		
	};  
			        
	return (ActCode);                   
}     


/********************************************************************************************

>	static ActionCode TransformNodeAction::Init(Operation* const pOp, 
						   						ActionList* pActionList, 
						   						Range NodeRangeIn, 
						   						TransformBase* Trans	
						  						Action** NewAction) 
	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/9/93  
	
	Inputs:		pOp: The operation to which the action should be added
				
				pActionList: The action list in the operation object
				
							 
				NodeRange: 	 The range of nodes to transform 

				Trans:		 The Transform to apply to the nodes in the range 

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
				
				The function calls the Action::Init function passing the runtime class 
				of a TransformNodeAction.
	Errors:		-
	SeeAlso:	Action::Init

********************************************************************************************/
ActionCode TransformNodeAction::Init(Operation* const pOp, 
						   			 ActionList* pActionList, 
						   			 Range NodeRangeIn, 
						   			 TransformBase* Trans,
									 List * pNodeList,
						  			 Action** NewAction)
{ 
	ActionCode Ac = (Action::Init(pOp,
					 			  pActionList,
					 			  sizeof(TransformNodeAction),
					 			  CC_RUNTIME_CLASS(TransformNodeAction), 
					 			  NewAction));   
						   
	if (Ac != AC_FAIL)
	{
		if (*NewAction != NULL)  
		{
			// Store the range and transform in the action 
			((TransformNodeAction*)(*NewAction))->NodeRange = NodeRangeIn;
			((TransformNodeAction*)(*NewAction))->pTrans = Trans;
			if (((TransformNodeAction*)(*NewAction))->m_pNodeList)
			{
				((TransformNodeAction*)(*NewAction))->m_pNodeList->DeleteAll();
				delete ((TransformNodeAction*)(*NewAction))->m_pNodeList;
				((TransformNodeAction*)(*NewAction))->m_pNodeList = NULL;
			}

			// run through the original node range, collecting all the selected nodes
		
			((TransformNodeAction*)(*NewAction))->m_pNodeList = new List;
			if (pNodeList == NULL)
			{
				Node *pNode = ((TransformNodeAction*)(*NewAction))->NodeRange.FindFirst();						
				while (pNode)											
				{														
					if (((NodeRenderable *)pNode)->IsNodeRenderableClass())
					{
						TRACEUSER( "SimonK", _T("trans node : %s\n"), (LPCTSTR) pNode->GetRuntimeClass()->GetClassName() );

						NodeListItem * pItem = new NodeListItem(pNode);
						if (pItem)
							((TransformNodeAction*)(*NewAction))->m_pNodeList->AddTail(pItem);
					}
					pNode = ((TransformNodeAction*)(*NewAction))->NodeRange.FindNext(pNode, FALSE);						
				}
			}
			else
			{
				NodeListItem * pItem = (NodeListItem *)(pNodeList->GetHead());
				Node * pNode = NULL;

				while (pItem)
				{
					pNode = pItem->pNode;
					
					if (((NodeRenderable *)pNode)->IsNodeRenderableClass())
					{						
						NodeListItem * pItem = new NodeListItem(pNode);
						if (pItem)
							((TransformNodeAction*)(*NewAction))->m_pNodeList->AddTail(pItem);
					}

					pItem = (NodeListItem *)pNodeList->GetNext(pItem);
				}		
			}
		}

	}            
	return (Ac); 
} 


/********************************************************************************************

>	virtual void TransformNodeAction::Slaughter()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/7/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	destructor which gets called when an operation is deleted  
	Errors:		-
	SeeAlso:	Action::Slaughter

********************************************************************************************/

void TransformNodeAction::Slaughter() 
{         
	// Destroy the Trans object      
	delete pTrans;
	pTrans = NULL;
	Action::Slaughter(); // Call base class to destroy this 
}   

/********************************************************************************************

>	TransformBase* TransformNodeAction::GetTransform()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/11/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Used to find the transform within the TransformNodeAction
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

TransformBase* TransformNodeAction::GetTransform()
{
	return (pTrans); 	
} 


/********************************************************************************************

>	void TransformNodeAction::CombineWith(TransformNodeAction* pMatrixTransformAction)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/11/94
	Inputs:		pMatrixTransformAction: A pointer to a TransformNodeAction containing a 
				Trans2DMatrix Transform
	Outputs:	-
	Returns:	-
	Purpose:	This function can only be called on a TransformNodeAction with a Trans2DMatrix
				Transform. The transform's matrix is multiplied by the matrix in the 
				pTransformAction therby combining the transforms.
	Errors:		An Error3 will occur if either this Transform or the pMatrixTransformAction
				do not contain Trans2DMatrix transforms. 
	SeeAlso:	-

********************************************************************************************/

void TransformNodeAction::CombineWith(TransformNodeAction* pMatrixTransformAction) 
{
	// First lets make sure that we can combine the transforms
	ERROR3IF(!(pMatrixTransformAction->GetTransform()->IS_KIND_OF(Trans2DMatrix)), 
			 "Cannot combine Transform actions");
	ERROR3IF(!(GetTransform()->IS_KIND_OF(Trans2DMatrix)), 
			 "Cannot combine transform actions");
	
	Trans2DMatrix* Trans1 = ((Trans2DMatrix*)GetTransform()); 
	Trans2DMatrix* Trans2 = ((Trans2DMatrix*)(pMatrixTransformAction->GetTransform()));
	(*Trans1)*=(*Trans2); 		 
}



/********************************************************************************************

	void Operation::OnClickWhileDragging(OilCoord PointerPos, ClickType Click, ClickModifiers Mods, BOOL bSolidDrag)
	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/6/96
	Inputs:		OilCoord PointerPos: position of click
				ClickType Click: single, double or triple
				ClickModifiers Mods: whether shift, ctrl etc was pressed during the click
	Outputs:	-
	Returns:	-
	Purpose:	This virtual function handles clicks with one mouse button while
				another mouse button is dragging. For example, the user drags with the
				left mouse button and clicks the right button during the drag.

				This particular case of the virtual function does nothing. It is
				overridden if the operation is a TransOperation, by TransOperation::
				OnClickWhileDragging.

				Feel free to add other overriding function to handle clicks-while-
				dragging for other operations.

	Errors:		None. 
	SeeAlso:	ClickModifiers::ClickWhileDrag (member variable); DocView::OnClick;
				ScreenView::HandleDragEvent; ScreenView::HandleButtonUp;
				TransOperation::OnClickWhileDragging

********************************************************************************************/


void Operation::OnClickWhileDragging(OilCoord PointerPos, ClickType Click, ClickModifiers Mods, BOOL bSolidDrag)
{
	//This function does nothing. It's overridden by TransOperation::OnClickWhileDragging.
}
