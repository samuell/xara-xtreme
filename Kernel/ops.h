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
	Declaration of the following classes:

	ActionList: Class used by an Operation to store UNDO, REDO, and Smart Duplicate actions
	Operation:  Class from which all Camelot operations are derived
	Action:     Class from which all operations actions are derived

*/  
 
 
/*
 * */    

#ifndef INC_OPS
#define INC_OPS         

#include "listitem.h"   
#include "list.h"
#include "binds.h"
#include "clikmods.h" 
#include "doccoord.h"
#include "docrect.h"
#include "opdesc.h"      
#include "range.h"
//#include "basedoc.h"
#include "selstate.h"

//#include "bars.h"

class Document; 
class DocView;
class View;
class OperationHistory;
class Spread;
class Node;
class NodeHidden;
class TransformBase;
class KeyPress;
class Action;
class BaseDocument;


/*******************************************************************************************

< SystemBarType

	Comment:	A number of hidden bars are created by the kernel during startup to group
	together OpDescriptors in sensible groups for the user. This enum allows each of those
	hidden bars to be uniquely identified.

enum SystemBarType { SYSTEMBAR_ILLEGAL = 0,
					 SYSTEMBAR_FILE,
					 SYSTEMBAR_EDIT,
					 SYSTEMBAR_ATTRIBUTE,
					 SYSTEMBAR_ARRANGE,
					 SYSTEMBAR_UTILITIES,
					 SYSTEMBAR_WINDOW,
					 SYSTEMBAR_VIEW = SYSTEMBAR_WINDOW,
					 SYSTEMBAR_HELP,
					 SYSTEMBAR_DEBUG
					 SYSTEMBAR_FEATHER
					};

	SeeAlso:	SystemBarOp::FindType

********************************************************************************************/ 

enum SystemBarType { SYSTEMBAR_ILLEGAL = 0,
					 SYSTEMBAR_FILE,
					 SYSTEMBAR_EDIT,
					 SYSTEMBAR_ATTRIBUTE,
					 SYSTEMBAR_ARRANGE,
					 SYSTEMBAR_UTILITIES,
					 SYSTEMBAR_WINDOW,
					 SYSTEMBAR_VIEW = SYSTEMBAR_WINDOW,	// Alias VIEW to WINDOW for now...
					 SYSTEMBAR_HELP,
					 SYSTEMBAR_ANIMATION,
					 SYSTEMBAR_TYPESETTING,
					 SYSTEMBAR_DEBUG,
					 SYSTEMBAR_FEATHER
					};


/********************************************************************************************

>	class ActionList: public List

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/7/93
	Purpose:	An ActionList is a list object which holds actions which can be
				executed. It is used by Operation objects to hold UNDO, REDO and Smart 
				duplicate actions.   
				
	SeeAlso:	Operation  
	SeeAlso:    Action

********************************************************************************************/

class CCAPI ActionList: public List
{    
	CC_DECLARE_MEMDUMP( ActionList ) 
public:     
	ActionList(); 
	~ActionList(); 
	BOOL ExecuteForwards(BOOL AllButLast); 
	BOOL ExecuteBackwards(BOOL AllButLast, BOOL bIgnoreSelectActions = FALSE);

	// This function can be used to search the action list to see if it contains an action
	// with a specific runtime class.
	Action* FindActionOfClass(CCRuntimeClass* ClassOfActionToFind, Action* LastAction = NULL); 

}; 
  
// The Opflags structure is used by the operation class to hold information about 
// how an operation should behave when it ends. 
struct CCAPI OpFlgsStr
{							 // TRUE when:
	BOOL Failed : 1;		 // Action could not be created
	BOOL ExecuteOnEnd : 1;	 // Execute all actions in End
	BOOL AllButLast : 1;	 // Execute all acts but last one in End
	BOOL KeepOnEnd : 1;		 // Not deleted in End
	BOOL UnwindingActions:1; // Op is being unwound
	BOOL HasOwnTimeIndicator:1; // Usually FALSE by default, however when TRUE indicates that
								// the operation should not use the default hourglass 
	BOOL SucceedAndDiscard:1;	// After the operation has finished all undo actions will be discarded
	BOOL DeleteOnEnd : 1;		// After the operation has ended it will be deleted
	BOOL IgnoreSelectActions : 1;	// Ignore selections when executing (for use when executing undo due to selection changes)
							  							
};

                  
// Current status of operation:-
enum OperationStatus { UNDO, REDO, DO };

/********************************************************************************************
>   class CUniversalParam

	Author:		Luke_Hart (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/10/05

	Purpose:	Simple class that makes it possible to have parameters that can either be
				a pointer or an INT32 in a 64bit safe manner.

********************************************************************************************/

class CUniversalParam
{
private:
	union
	{
		void		   *m_pVal;
		INT32			m_lVal;
	};

public:
	CUniversalParam() : m_pVal( NULL )
	{}
	CUniversalParam( void *p ) : m_pVal( p )
	{}
	CUniversalParam( INT32 l ) : m_lVal( l )
	{}

	void *operator =( void *p )
	{
		return m_pVal = p;
	}
	INT32 operator =( INT32 l )
	{
		return m_lVal = l;
	}

	operator void *()
	{
		return m_pVal;
	}
	operator INT32()
	{
		return m_lVal;
	}
};

/********************************************************************************************
>   class OpParam

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/6/94

	Purpose:	Operation parameter structure. The meaning of the structure's fields are
				defined by each operation. Note that sensible Operations will derive a
				special OpParam for their own specific needs, so you should use the
				derived class where it is available.

	SeeAlso:	Operation::DoWithParam

********************************************************************************************/

class OpParam : public CCObject
{
CC_DECLARE_DYNAMIC(OpParam)

public:
	OpParam() { Param1 = Param2 = INT32(0); Output = NULL; }
	OpParam(CUniversalParam P1, CUniversalParam P2) { Param1 = P1; Param2 = P2; Output = 0; }
	OpParam(CUniversalParam P1, CUniversalParam P2, BOOL* Out) { Param1 = P1; Param2 = P2; Output = Out; }
	CUniversalParam		Param1;
	CUniversalParam		Param2;

	BOOL			   *Output; 
};

/********************************************************************************************

>	class Operation : public MessageHandler

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/7/93
	Purpose:	This abstract class represents an operation which is a single job initiated 
				by the user. You should derive all non undoable operations directly from 
				this base class. 
				
	SeeAlso:    OperationHistory
	SeeAlso:	UndoableOperation

********************************************************************************************/

class CCAPI Operation : public MessageHandler
{   
	CC_DECLARE_DYNCREATE( Operation )   
	
public:
	virtual void OnClickWhileDragging(OilCoord PointerPos, ClickType Click, ClickModifiers Mods, BOOL bSolidDrag);

	Operation(CCRuntimeClass* MsgHandlerClass = CC_RUNTIME_CLASS(Operation)); 
	virtual ~Operation();

	static BOOL Initialise();

	virtual void End(); 

	// Mark H
	virtual Matrix GetCurrentMatrix() { return Matrix(); };
	
	UINT32 GetSize(); 
 
	static Operation* GetCurrentDragOp();
	virtual BOOL SnappingDrag() { return TRUE; }

	// Event handler functions called by DocView/CCamView...
	virtual void DragPointerMove( DocCoord PointerPos,
						ClickModifiers ClickMods, Spread *pSpread, BOOL bSolidDrag);
	virtual void DragPointerIdle( DocCoord PointerPos,
						ClickModifiers ClickMods, Spread *pSpread, BOOL bSolidDrag);
	virtual void DragFinished( DocCoord PointerPos,
					   ClickModifiers ClickMods, Spread *pSpread,
					   BOOL Success, BOOL bSolidDrag);
	virtual BOOL DragKeyPress(KeyPress* pKeyPress, BOOL bSolidDrag);
	virtual void DragModeChanged(BOOL bSolidDrag);
					   
	// Function to translate coord from one spread to another.
	DocCoord MakeRelativeToSpread(Spread *Dest, Spread *Src, DocCoord);

	// Function to allow operations to render something during a drag
	virtual void RenderDragBlobs(DocRect, Spread*, BOOL bSolidDrag);

	// Function called on idle events if required
	virtual BOOL OnIdleEvent();

	// These Fail functions tell the Op what to do when it ends.
	void FailAndExecute();
	void FailAndExecuteAllButLast();
	void FailAndDiscard();
	void FailAndExecuteIgnoreSelActions();

	// When called operation will not be undoable 
	void SucceedAndDiscard();

	// These two functions execute the appropriate action list.
	virtual BOOL Undo();
	virtual BOOL Redo();

	// This function asks the operation to Do itself again. Notice that DoSmart 
	// can be polymorphic because no smart functions can take params - they 
	// all work on the selection.
	virtual void DoSmart();
	
	// This function is the polymorphic one to be used to Do all simple Operations. All subclasses 
	// of Operation will implement there own DO function maintaining the same interface as
	// below in order for the Do function to work with all simple Operations. (This will be
	// particularly useful for implementing Menu Operations!)
	virtual void Do(OpDescriptor*);   
    
	// Same as Do except it allows a parameter to be passed - Yes I know this is a bit yuk
	// but at least is saves me changing every Do function in Camelot.
    virtual void DoWithParam(OpDescriptor*, OpParam* pOpParam);            

	OpFlgsStr	GetOpFlgs();
	BOOL		OpHasFailed() const {return OpFlags.Failed;}
	void		DeleteOnEnd();
	void		KeepOnEnd() {OpFlags.KeepOnEnd = TRUE;}

	ActionList* GetUndoActionList(); 
	ActionList* GetRedoActionList(); 
	
	OperationStatus OpStatus; // When doing the operation for the first time equals DO 
							  // When UNDOING the operation equals UNDO 
							  // when REDOING the operation equals REDO

	// Utility methods required by the ALLOC_WITH_FAIL macro
	BOOL UserWantsToDeleteUndoOps(); 
	void OperationMemoryFailure();

	// Find out which document/(doc)view this operation is working on.
	Document *GetWorkingDoc()		{ return pOurDoc; }
	View     *GetWorkingView() 		{ return pOurView; }
	DocView  *GetWorkingDocView();

	// Retrieve The Operations Name
	virtual void GetOpName(String_256*);

	// interogates the current (drag) Operation for statusLine text
	virtual BOOL GetStatusLineText(String_256* pText, Spread* pSpread, DocCoord DocPos, ClickModifiers ClickMods);

	// Call this function from the Operation's Init method to register the operations OpDescriptor
   	static BOOL RegisterOpDescriptor(
				 					  UINT32 toolID,       // Module (Tool) Identifier
				 					  UINT32 txID,         // String Resource ID
									  CCRuntimeClass* RuntimeClass, // The operations runtime class
				 					  TCHAR* tok,		 // pointer to the token string
				 					  pfnGetState gs,	 // pointer to the GetState function
				 					  UINT32 helpId = 0,	 // help identifier 
									  UINT32 bubbleID = 0, // string resource for bubble help
				 					  UINT32 resourceID = 0,	// resource ID
									  UINT32 controlID = 0,	// control ID
									  SystemBarType GroupBarID = SYSTEMBAR_ILLEGAL,	// group bar ID
				 					  BOOL ReceiveMessages = TRUE,	// BODGE
				 					  BOOL Smart = FALSE,
				 					  BOOL Clean = TRUE,   
									  OpDescriptor *pVertOpDesc = NULL,
									  UINT32 OneOpenInstID = 0,		
									  UINT32 AutoStateFlags = 0,
									  BOOL fCheckable = FALSE
				 					  );

	// These functions should be used in conjuction with Node::AllowOp() or Range::AllowOp() to allow
	// the op to work when select-inside is present.
	// Calling these functions should ensure that parent nodes update themselves when an op is applied to
	// their children
	virtual BOOL UpdateChangedNodes(ObjChangeParam* pParam,Spread* pSpread = NULL);
	virtual BOOL UpdateChangedNode(ObjChangeParam* pParam,Node* pNode);

	// This varient will update all nodes in all the docs in the application
	virtual BOOL UpdateAllChangedNodes(ObjChangeParam* pParam);
	// This will update all nodes in the given document
	virtual BOOL UpdateChangedNodes(ObjChangeParam* pParam,Document* pDoc);

	// Dumps the undo/redo action lists to the debugger
	virtual void Dump();

	static BOOL GetQuickRender(Node* pNode);
	static void SetQuickRender(BOOL bNewState, Operation* pQROp = NULL);

protected: 

	// Functions called by subclasses of Operation...
	BOOL StartDrag(	DragType type, 
					DocRect* MoveBBox = NULL,
					DocCoord* StartPos = NULL,
					BOOL KeepAccuracy = TRUE,
					BOOL bSolidDragSupported = FALSE);
	BOOL EndDrag(void);  
	
	void EndOp(); // The old End() function   

	// Operation flags structure  
	OpFlgsStr OpFlags;      

	// Points to the document/view this Operation is associated with when "live".
	//
	// NB. pOurView is usually NULL as many operations don't care which or many views are
	//     associated with the document, but some do - e.g. changing rendering quality,
	//	   zooming in and out, etc.
	Document* pOurDoc;
	View*  	  pOurView;
     
	// UNDO/REDO lists.         
	// Note: At any given time ONE of these lists MUST be empty. This means that there is no 
	// need to store flags to indicate if the operation is for UNDO or REDO. If UndoActions
	// contains any actions then the operation can be undone, else the operation can be 
	// redone. 
    
	ActionList UndoActions;		
	ActionList RedoActions;		   
     
    // Actions for smart duplicate
	ActionList SmartActions;	      

public:	
	// Somebody might want to invoke an action given an undoable op pointer. At this point, they
	// will need access to the action lists. So give them a public access function.
	// Only use these if you know what you are doing!
	// Should be const but of course then nothing would work!
	ActionList * GetUndoActions() { return &UndoActions; }
	ActionList * GetRedoActions() { return &RedoActions; }
	ActionList * GetSmartActions() { return &SmartActions; }

private:
	
	// The string OpName should be defined in a subclass of operation and be returned 
	// by the GetName() function. It is not defined for the operation class. 
	
	// static String OpName; // The name of the operation object    
	
	// For drag operations CurrentDragOp will be a pointer to the Operation which should 
	// receive mouse movement events during the drag. It will ensure that only one
	// operation can perform a drag at a time and that keypress events cannot change the
	// state of the document while a drag is taking place.    
	
	static Operation* CurrentDragOp; 

	// The DeleteUndoOpsToMake space flag is set to TRUE after the user responds YES to the
	// question "do you want to delete undo operations to make space for the current operation ?". 
	// It is required so that the user is not asked the same question more than once in performing 
	// the operation (The next time YES will be taken as the default). It will be reset to FALSE
	//  when the operation ends. 
	BOOL DeleteUndoOpsToMakeSpace;

	
	// In the operations End method if pDocToInformOfOpStatus is not NULL then
	// If the operation has failed AbortComponent copy is called on the document
	// else EndComponentCopy is called. 
	BaseDocument* pDocToInformOfOpStatus;   

	// Vars to do with stopping auto-repeat keys during drags
	static INT32 CurKeyboardSpeed;
	static BOOL ResetKeyboardSpeed;
	static BOOL KeyboardRepeatOffDuringDrags;

	static BOOL s_bQuickRender;

public:
	void InformDocComponentsOfOperationsOutcome(BaseDocument* pDoc) { pDocToInformOfOpStatus = pDoc; }; 
    
};


enum ActionCode { AC_FAIL, AC_OK, AC_NORECORD }; 

/********************************************************************************************

>	class Action: public ListItem

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/7/93
	Purpose:	This class represents an Action which is an individual job which is executed
				to accomplish an operation. An operation contains lists of actions to 
				perform Undo, Redo and Smart Duplication. 
				
	SeeAlso:	Operation

********************************************************************************************/

class CCAPI Action: public ListItem
{        
	CC_DECLARE_DYNCREATE( Action ) 

	friend class OperationHistory; 
	
public: 

	Action(); 
	virtual ~Action();   
	virtual void Slaughter(); // A more extreme form of destruction 
	virtual ActionCode Execute();          
	static ActionCode Init(Operation* pOp, 
						   ActionList* pActionList, 
						   UINT32 Size, 
						   CCRuntimeClass* ActionClass, 
						   Action** NewAction); 
		
	// If IsDiscardableAction returns TRUE then it indicates that it can be deleted after the 
	// operation ends. It indicates that the action never does anything and so is dead-wood.				   
	virtual BOOL IsADiscardableAction() { return FALSE; }  

	// Find out which document/(doc)view this action is working on.
	Document *GetWorkingDoc()		{ return pOperation->GetWorkingDoc(); }
	View     *GetWorkingView() 		{ return pOperation->GetWorkingView(); }
	DocView  *GetWorkingDocView()	{ return pOperation->GetWorkingDocView(); }
  
	UINT32 GetSize(); 
	
	// Dumps the action list to the dubugger
	virtual void Dump();

	BOOL TransferToOtherOp(Operation* pOtherOp, ActionList* pAddActions, ActionList* pOtherActions);

protected: 

	Operation* pOperation; // Pointer to the operation to which the action is attached. It 
						   // is neccessary to store this because executing an action will 
						   // usually cause a new action to be created, and Operation is
						   // required as an argument to the new action's Init method.   
						   
	ActionList* pOppositeActLst; // A pointer to the opposite action list. This is really useful 
						   // to know, especially when an Action is used both to Undo and Redo.  
						   // If the action is on the UndoActionList then pOppositeActLst 
						   // will point to the RedoActionList, and visa versa. 

	UINT32 Size;            // The total size of the action in bytes. It is neccessary to 
						   // store this because before the action is deleted the size of the 
						   // operation history needs to be reduced by the size of the action. 					   
					 

	// *** This is what LastDiscardableAction USED to be - see below for it's new friendly
	// purpose.
	// When an Action fails in the Init function this can be due to either there not being 
	// enough system memory, or because there is not enough space in the Operation History for
	// the Action. In the later case an instance of the action will exist and 
	// LastDiscardable action will be set to point at the action. When Init is called the next 
	// time it will be deleted.
	


	//static Action* LastDiscardableAction;  
						  
};    


/********************************************************************************************

>	ALLOC_WITH_FAIL(Ptr, AllocExpr, pOp) 											  \

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/8/93
	Inputs:		Ptr: 	   The pointer or handle to the object being allocated
				AllocExpr: The expression to allocate memory (e.g. new Blobby(x,y,z) )
				pOp: 	   A pointer to the operation for which memory is being allocated.   
	Outputs:	-
	Returns:	-
	Purpose:	This macro should be used whenever you want to allocate any memory in an 
				operation.It initially tries to allocate system memory, if it fails then it 
				prompts the user to see if they want to delete undo operations to try and make
				room. If after deleting undo operations memory still cannot be allocated then 
				FailAndExecute() is called and a NULL Ptr is returned. 
    
    			The macro is fairly long and may be replaced by a series of functions 
    			one for each type of memmory allocation. 
    
	Errors:		-
	SeeAlso:	CALL_WITH_FAIL

********************************************************************************************/


#define ALLOC_WITH_FAIL(Ptr, AllocExpr, pOp) 											  \
{																						  \
	(Ptr) = (AllocExpr); 																  \
	if ( (Ptr) == NULL && (pOp) != NULL ) 												  \
	{		                                                                              \
		OperationHistory& OpHist = pOp->GetWorkingDoc()->GetOpHistory();				  \
		/* If the operation is unwinding then we can't afford the luxury of asking the */ \
		/* user */																		  \
	    if (((pOp)->GetOpFlgs().UnwindingActions) || ((pOp)->UserWantsToDeleteUndoOps())) \
	    {                                       										  \
			/* The user wants to try deleting undo records to make space for the action*/ \
			BOOL NoMoreUndoOpsToDelete = FALSE; 									      \
			/* Repeatedly delete undo operations to try and create enough room for the    \
			 allocation. */                                                               \
			do                                     									      \
			{                                      									      \
				/* Reduce the size of the operation history by deleting a single undo */  \
				/* operation                                                          */  \
				if  ((OpHist.ReduceSize((OpHist.GetSize()-1), TRUE)) == FALSE)            \
					NoMoreUndoOpsToDelete = TRUE;                                 	      \
															  	                  		  \
			} while ( (((Ptr) = (AllocExpr))  == NULL)      							  \
					&& (!NoMoreUndoOpsToDelete) ); 									      \
		                                              									  \
			if (NoMoreUndoOpsToDelete)           								          \
				(pOp)->OperationMemoryFailure(); 		  								  \
		}																				  \
		else 																			  \
		{																				  \
			pOp->FailAndExecute();														  \
		} 			 																	  \
	}																					  \
} 

/********************************************************************************************

>	CALL_WITH_FAIL(fn, pOp, Success) 											  

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/8/93
	Inputs:		fn: 	   the function to call
				pOp: 	   A pointer to the operation for which memory is being allocated.   
			
	Outputs:	Success:   A BOOL variable which is TRUE if the function could be executed
						   with a TRUE return value. 
	Returns:	-
	Purpose:	This MACRO is the same as the ALLOC_WITH_FAIL except that it repeatedly 
				calls a function which tries to allocate memory. The function must
				return a TRUE/FALSE success status. 
			
    
	Errors:		-
	SeeAlso:	ALLOC_WITH_FAIL										 

********************************************************************************************/


#define CALL_WITH_FAIL(fn,pOp,Success) 											          	  \
{	                                                                                      	  \
	Success = FALSE; 																	  	  \
	if ( !(fn) )																			  \
	{																						  \
		if ( (pOp) != NULL ) 												          		  \
		{		                                                                              \
			OperationHistory& OpHist = (pOp)->GetWorkingDoc()->GetOpHistory();				  \
		    if ( ((pOp)->GetOpFlgs().UnwindingActions) || (pOp)->UserWantsToDeleteUndoOps())  \
		    {                                       										  \
				/* The user wants to try deleting undo records to make space for the action*/ \
				BOOL NoMoreUndoOpsToDelete = FALSE; 									      \
				/* Repeatedly delete undo operations to try and create enough room for the    \
				 allocation. */                                                               \
				do                                     									      \
				{                                      									      \
					/* Reduce the size of the operation history by deleting a single undo */  \
					/* operation                                                          */  \
					if  ((OpHist.ReduceSize((OpHist.GetSize()-1), TRUE)) == FALSE)            \
						NoMoreUndoOpsToDelete = TRUE;                                 	      \
																  	                  		  \
				} while ( (!(fn))      							  							  \
						&& (!NoMoreUndoOpsToDelete) ); 									      \
			                                              									  \
				if (NoMoreUndoOpsToDelete)           								          \
				{																			  \
					(pOp)->OperationMemoryFailure(); 		  								  \
				}																			  \
				else																		  \
				{																			  \
					Success = TRUE; 														  \
				}																			  \
			}																				  \
			else																			  \
			{																				  \
				(pOp)->FailAndExecute(); 													  \
			}	 																			  \
		}																					  \
	}																						  \
	else 																					  \
	{																						  \
		Success = TRUE; 																	  \
	}																						  \
}																							  \

/********************************************************************************************

>	class InvalidateRegionAction: public Action

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/9/93
	Purpose:	When executed this action will Force a redraw of the bounding rectangles of 
				each node in a range. 
	SeeAlso:	-

********************************************************************************************/

class CCAPI InvalidateRegionAction: public Action
{                                
	CC_DECLARE_DYNCREATE( InvalidateRegionAction )

public:
	InvalidateRegionAction(); 
	virtual ActionCode Execute();   
	 
	static ActionCode Init(Operation* const pOp, 
						   ActionList* pActionList, 	
						   Range nodeRange, // The range on nodes we want to invalidate
						   Spread* pSpread, 
						   BOOL includeBlobs, 
						   Action** NewAction); 
protected:
	Range NodeRange; 
	BOOL IncludeBlobs:1; 
	Spread* pSpread; 	// We need to be able to invalidate the region of a hidden node, 
						// so we must store a copy of the spread. 
};
 
/********************************************************************************************

>	class InvalidateRegionIfBgRedrawAction: public InvalidateRegionAction

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/5/95
	Purpose:	When executed this action will Force a redraw of the bounding rectangles of 
				each node in a range.
				This version will only invalidate the region if Background Redraw is
				in progress.
	SeeAlso:	-

********************************************************************************************/

class InvalidateRegionIfBgRedrawAction: public InvalidateRegionAction
{                                
	CC_DECLARE_DYNCREATE( InvalidateRegionIfBgRedrawAction )

public:
	InvalidateRegionIfBgRedrawAction(); 
	virtual ActionCode Execute();   

	static ActionCode Init(Operation* const pOp, 
						   ActionList* pActionList, 	
						   Range nodeRange, // The range on nodes we want to invalidate
						   Spread* pSpread, 
						   BOOL includeBlobs, 
						   Action** NewAction); 
};

/********************************************************************************************

>   class HideNodeAction: public Action

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/9/93
	Purpose:	When executed this action will hide a node. It will
				also generate a ShowNodeAction action adding it to the opposite Action list. 
				 
	SeeAlso:	ShowNodeAction

********************************************************************************************/

class CCAPI HideNodeAction: public Action
{
	CC_DECLARE_DYNCREATE( HideNodeAction )

public:
	HideNodeAction(); 
	virtual ActionCode Execute();    
	static ActionCode Init(Operation* const pOp, 
						   ActionList* pActionList,
						   Node* NodeToHide, 
						   BOOL IncludeSubtreeSize,  	
						   Action** NewAction,
						   BOOL TellSubtree = TRUE); 

	CCRuntimeClass* GetClassOfAttrNodeToHide() const
		{ return ClassOfAttributeToHide; }

	void RecordTag(Node* NodeToHide);

private: 

	// When we generate a HideNodeAction for a non attribute node, we need to simply store
	// a pointer to the node, as we can be sure that this node will be around when we come to
	// execute this action.

	// However if we are generating a HideNodeAction for an attribute then things are different. 
	// During attribute optimisation attribute nodes can get deleted, so storing a pointer to 
	// the attribute node is a bad idea. Instead we store a pointer to the attribute's parent (which can't 
	// be an attribute, so can't get deleted), and the RuntimeClass of the attribute. We can then
	// find the attribute by searching for it.

	Node* node; // node which should be hidden when HideNodeAction is executed. If the node
				// to be hidden is an attribute however then this node will be the attribute's
				// parent (see the notes below).

	CCRuntimeClass* ClassOfAttributeToHide; // If the node being hidden is a NodeAttribute then
											// node will point to the attribute's parent (see above)
											// and ClassOfAttributeToHide will be the runtime class
											// of the attribute which should be hidden.	When this
											// action is executed if ClassOfAttributeToHide is
											// non NULL then we know that it is an attribute that we
											// must hide, so we search for it.

	TAG m_nHiddenWixAttrTag;				// If the node being hidden is a Wix attribute then this
											// holds its tag so it can be properly identified by the
											// Execute() function.  Wix attributes are not optimised
											// and also unlike other attributes, more than one of the
											// same kind can apply to an object.

	// The size of a HideNodeAction is always sizeof(HideNodeAction), however the 
	// IncludeSubtreeSize flag is required to pass to the twin ShowNodeAction. 

	BOOL IncludeSubtreeSize:1; 	
	BOOL TellSubtree:1; 
	BOOL m_bEffect:1;
};																									   

 
/********************************************************************************************

>   class ShowNodeAction: public Action

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/9/93
	Purpose:	When executed this action will show a node. It will also generate a 
				HideNodeAction action adding it to the opposite Action list.
				
	SeeAlso:	HideNodeAction

********************************************************************************************/

class CCAPI ShowNodeAction: public Action
{                                
	CC_DECLARE_DYNCREATE( ShowNodeAction )

public:
	ShowNodeAction();             
	virtual ~ShowNodeAction();  
	virtual void Slaughter(); 
	virtual ActionCode Execute();    
	static ActionCode Init(Operation* const pOp, 
						   ActionList* pActionList, 
						   NodeHidden* HiddenToShow, 
						   BOOL IncludeSubtreeSize, 	
						   Action** NewAction, 
						   BOOL TellSubtree = TRUE); 
private: 
	NodeHidden* node; 

	// We need to store an IncludeSubtreeSize flag because when we create the twin
	// HideNodeAction it is required as a parameter. 
	BOOL IncludeSubtreeSize:1; 
	BOOL TellSubtree:1; 
};  




/********************************************************************************************

>   class UnApplyAction: public Action

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/07/2005
	Purpose:	When executed this action will hide a node. It will
				also generate a ShowNodeAction action adding it to the opposite Action list. 
				 
	SeeAlso:	ShowNodeAction

********************************************************************************************/

class CCAPI UnApplyAction: public Action
{
	CC_DECLARE_DYNCREATE( UnApplyAction )

	friend class OpApplyAttrInteractive;

public:
	UnApplyAction();
	~UnApplyAction();
	virtual ActionCode Execute();
	static ActionCode Init(Operation* const pOp, 
						   ActionList* pActionList,
						   Node* pActionNode,
						   NodeAttribute* pActionAttribute,
						   BOOL IncludeSubtreeSize,
						   Action** NewAction,
						   BOOL TellSubtree = TRUE); 

//	CCRuntimeClass* GetClassOfAttrNodeToHide() const
//		{ return ClassOfAttributeToHide; }

	void RecordTag(NodeAttribute* pAttr);

private: 
	Node* m_pApplyNode;
	NodeAttribute* m_pAttribute;
	TAG m_nAttrTag;							// If the node being hidden is a Wix attribute then this
											// holds its tag so it can be properly identified by the
											// Execute() function.  Wix attributes are not optimised
											// and also unlike other attributes, more than one of the
											// same kind can apply to an object.

	// The size of a HideNodeAction is always sizeof(HideNodeAction), however the 
	// IncludeSubtreeSize flag is required to pass to the twin ShowNodeAction. 

	BOOL IncludeSubtreeSize:1; 	
	BOOL TellSubtree:1; 
};																									   

 


/********************************************************************************************

>   class ApplyAction: public Action

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/07/2005
	Purpose:	When executed this action will show a node. It will also generate a 
				HideNodeAction action adding it to the opposite Action list.
				
	SeeAlso:	HideNodeAction

********************************************************************************************/

class CCAPI ApplyAction: public Action
{                                
	CC_DECLARE_DYNCREATE( ApplyAction )

	friend class OpApplyAttrInteractive;

public:
	ApplyAction();             
	virtual ~ApplyAction();  
	virtual ActionCode Execute();    
	static ActionCode Init(Operation* const pOp, 
						   ActionList* pActionList, 
						   Node* pActionApplyNode,
						   NodeAttribute* pActionAttribute,
						   BOOL IncludeSubtreeSize, 	
						   Action** NewAction, 
						   BOOL TellSubtree = TRUE); 
private: 
	Node* m_pApplyNode;
	NodeAttribute* m_pAttribute;

	// We need to store an IncludeSubtreeSize flag because when we create the twin
	// HideNodeAction it is required as a parameter. 
	BOOL IncludeSubtreeSize:1; 
	BOOL TellSubtree:1; 
};  




/********************************************************************************************

>	class RestoreSelectionsAction: public Action

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/9/93
	Purpose:	When executed this action will restore the selection state of the document. 
				It will also spawn another identical RestoreSelectionsAction adding and add it to 
				the opposite action list. 
			
	SeeAlso:	-

********************************************************************************************/

class CCAPI RestoreSelectionsAction: public Action
{                                
	CC_DECLARE_DYNCREATE( RestoreSelectionsAction )

public:
	RestoreSelectionsAction(); 
	~RestoreSelectionsAction();  
	void Slaughter();
	virtual ActionCode Execute();    
	static ActionCode Init(Operation* const pOp, 
						   ActionList* pActionList, 	
						   SelectionState* SelState, 
						   BOOL Toggle, 
						   BOOL ToggleStatus, 
						   BOOL SelStateShared,  
						   BOOL RenderStartBlobs, 
						   BOOL RenderEndBlobs,
						   BOOL StartRestore,   
						   Action** NewAction);

	SelectionState* GetSelState() { return SelState; };      
						  
private:
	SelectionState* SelState; //  Selection state which will be restored when the action is 
							  //  executed.

	// If toggle is true then the ToggleStatus determines if the selections are restored 
	// or not. See Init method if you are confused. 
	BOOL toggle:1; 
	BOOL toggleStatus:1; 
	BOOL SelStateShared:1; // If the SelState is shared by a pair of RestoreSelActions 
						 // then the SelState will only be deleted when ToggleStatus = TRUE 
	
	BOOL RenderStartBlobs:1; 
	BOOL RenderEndBlobs:1;

	BOOL StartRestore:1;   // Is this the first restore executed when the operation is undone/redone
};   


/********************************************************************************************

>	class SelectDeselectAction: public Action

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/9/93
	Purpose:	When executed this action will render node's selection blobs (which could remove them)
				and set it's selection state. If the node is currently selected then the 
				node is deselected. If the node is currently deselected it is selected.
				It creates a second SelectDeselectAction for the node and adds it to the 
				opposite action list. 
			
	SeeAlso:	-

********************************************************************************************/

class CCAPI SelectDeselectAction: public Action
{                                
	CC_DECLARE_DYNCREATE( SelectDeselectAction )

public:
	SelectDeselectAction(); 
	~SelectDeselectAction();
	virtual ActionCode Execute();    
	static ActionCode Init(Operation* const pOp, 
						   ActionList* pActionList, 	
						   Node* SelDeNode,
						   Spread* pSpread,
						   Action** NewAction); 
		
	Node* node; // The node which will be selected/deselected 
	Spread *Parent;
};   

 
/********************************************************************************************

>	class TransformNodeAction: public Action

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/9/93
	Purpose:	When executed this action will transform a range of NodeRenderableInk nodes. 
				It also creates another TransformNodeAction to 'undo' the transformation just 
				performed. It adds this action to the opposite action list.   
	
	SeeAlso:	-

********************************************************************************************/


class CCAPI TransformNodeAction: public Action
{                                
	CC_DECLARE_DYNCREATE( TransformNodeAction )

public:
	TransformNodeAction(); 
	~TransformNodeAction();
	void Slaughter();  

	virtual ActionCode Execute();  
	static ActionCode Init(Operation* const pOp, 
						   ActionList* pActionList, 
						   Range NodeRangeIn, 
						   TransformBase* Trans,
						   List * pNodeList,
						   Action** NewAction); 

	// For op merging
	void CombineWith(TransformNodeAction* pMatrixTransformAction);
	TransformBase* GetTransform(); 
	const Range* GetActionRange() const {return &NodeRange;}

private:
		
	Range NodeRange;
	
	// the list of parent-promoted nodes in the first range
	List * m_pNodeList;
	
	TransformBase* pTrans; // The transform to apply to every node in the range 
};   
       
#endif


