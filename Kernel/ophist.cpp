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
	
*/	

/*
*/ 
#include "camtypes.h"  
#include "ops.h"

DECLARE_SOURCE("$Revision$");
             
#include "ophist.h"  
#include "ensure.h"
#include "errors.h"
//#include "mario.h"
#include "helpids.h"
#include "document.h"
//#include "resource.h"
//#include "barsdlgs.h"
#include <limits.h>
//#include "bubbleid.h"

CC_IMPLEMENT_MEMDUMP(OperationHistory, CCObject)
#if !defined(EXCLUDE_FROM_RALPH)
CC_IMPLEMENT_DYNCREATE(OpRedo, Operation)  
CC_IMPLEMENT_DYNCREATE(OpUndo, Operation)  
#endif

// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW
     
/********************************************************************************************

>	OperationHistory::OperationHistory()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/7/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Constructs an OperationHistory object with a maximum size of UINT32_MAX
				bytes.  
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

OperationHistory::OperationHistory()
{        
	NowPtr = NULL;				// History initially has no operations
	CurrentSize = 0;          
	//MaxSize = 512000;			// 500K
	MaxSize = 2048000;			// 2000K (2MB)		CGS (18/5/2000)
								// I have changed this because it is too small for some
								// of the operations that we can now perform
	Reduced = FALSE;
}                                     

/********************************************************************************************

>	OperationHistory::~OperationHistory()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/8/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	OperationHistory destructor
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

OperationHistory::~OperationHistory()
{
	//if (Action::LastDiscardableAction != NULL)  
	//	delete (Action::LastDiscardableAction); 

	// Delete all operations in the OpHistoryList    
	// This is done using by calling the normal methods, to ensure deletion order
	// is correct
	DeleteUndoableOps();
	DeleteRedoableOps();	
}

/********************************************************************************************

>	OperationHistory::OperationHistory(UINT32 MaximumSize)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/7/93
	Inputs:		MaximumSize: The maximum size of the operation history being constructed
	Outputs:	-
	Returns:	-
	Purpose:	Constructs an operationHistory object with a maximum size of MaximumSize
				bytes. 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

OperationHistory::OperationHistory(UINT32 MaximumSize)
{                            
	NowPtr = NULL;           // History initially has no operations
	CurrentSize = 0; 
	MaxSize = MaximumSize; 
	Reduced = FALSE;
}    

/********************************************************************************************

>	void OperationHistory::SetNewMaxSize(UINT32 NewMaxSize)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/7/93
	Inputs:		NewMaxSize: The new maximum size of the operation history
							in bytes. 
	Outputs:	-
	Returns:	FALSE only if we are reducing the maximum size, and deleting all undo ops would
				not reduce the current size of the operation history to NewMaxSize bytes or less.
				
				TRUE otherwise
				
	Purpose:	For setting a new maximum size for the operation history. 
				
				
	Errors:		-
	SeeAlso:	OperationHistory::ReduceSize

********************************************************************************************/

BOOL OperationHistory::SetNewMaxSize(UINT32 NewMaxSize)
{   
	
	if (NewMaxSize < CurrentSize)    
	{  
		// We must try to reduce the current size of the OperationHistory to NewMaxSize 
		// bytes or less.
		if (!ReduceSize(NewMaxSize, FALSE))
		{
			return FALSE; 
		} 
	}
	MaxSize = NewMaxSize; 
	return (TRUE); 
} 


/********************************************************************************************

>	UINT32 OperationHistory::GetSize()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/7/93
	Inputs:		-
	Outputs:	-
	Returns:	The current size of the operation history in bytes. 
	Purpose:	For finding the current size of the operation history
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

UINT32 OperationHistory::GetSize()
{   
	return (CurrentSize);  
} 

/********************************************************************************************

>	UINT32 OperationHistory::GetMaxSize()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/7/93
	Inputs:		-
	Outputs:	-
	Returns:	The maximum size of the operation history
	Purpose:	To find the maximum size of the operation history
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

UINT32 OperationHistory::GetMaxSize()
{    
	return (MaxSize);  
}  


/********************************************************************************************

>	void OperationHistory::IncSize(UINT32 SizeDecrement)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/7/93
	Inputs:		SizeIncrement: Amount by which the current size of the operation history 
							   is to be increased. 
	Outputs:	-
	Returns:	-
	Purpose:	To increase the current size of the operation history. 
	Errors:		-      
	Scope:		private
	SeeAlso:	-

********************************************************************************************/

                              
void OperationHistory::IncSize(UINT32 SizeIncrement)
{
	CurrentSize += SizeIncrement; 
}  

/********************************************************************************************

>	void OperationHistory::DecSize(UINT32 SizeDecrement)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/7/93
	Inputs:		SizeDecrement: Amount by which the current size of the operation history 
							   is to be decreased. 
	Outputs:	-
	Returns:	-
	Purpose:	To decrease the current size of the operation history. 
	Errors:		-   
	Scope:		private
	SeeAlso:	-

********************************************************************************************/


void OperationHistory::DecSize(UINT32 SizeDecrement)
{ 
	CurrentSize -= SizeDecrement; 
} 



/********************************************************************************************

>	UINT32 OperationHistory::GetNumUndoSteps()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/11/94
	Inputs:		-
	Outputs:	-
	Returns:	The number of undo steps in this history.
	Purpose:	To return the number of undo steps stored in this history. 
	Errors:		-   
	Scope:		public
	SeeAlso:	OperationHistory::GetNumRedoSteps

********************************************************************************************/
UINT32 OperationHistory::GetNumUndoSteps()
{ 
	if (NowPtr == 0)
		return 0;

	ListItem* pOp = OpHistoryList.GetHead();   
	
	UINT32 count = 1;

	while ((pOp != NowPtr) && (pOp != NULL))
	{      
		count ++;
		pOp = OpHistoryList.GetNext(pOp);       
	}	
	return count;
} 



/********************************************************************************************

>	UINT32 OperationHistory::GetNumRedoSteps()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/11/94
	Inputs:		-
	Outputs:	-
	Returns:	The number of redo steps in this history.
	Purpose:	To return the number of redo steps stored in this history. 
	Errors:		-   
	Scope:		public
	SeeAlso:	OperationHistory::GetNumUndoSteps

********************************************************************************************/
UINT32 OperationHistory::GetNumRedoSteps()
{ 
	ListItem* pOp = NULL;

	if (NowPtr == NULL)
	{
		// If Now is NULL then there are no undo operations
		pOp	= OpHistoryList.GetHead();
	}
	else
	{
		// Get the pointer to the operation following the NowPtr (which points at last undo op)
		pOp = OpHistoryList.GetNext(NowPtr);
	}
		
	UINT32 count = 0;

	while (pOp != NULL)
	{      
		count ++;
		pOp = OpHistoryList.GetNext(pOp);       
	}	
	return count;
} 



/********************************************************************************************

>	BOOL OperationHistory::IsReduced()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/11/94
	Inputs:		-
	Outputs:	-
	Returns:	True if operations have been discared to free memory, otherwise FALSE
	Purpose:	To discover if operations have been discared, reducing the number of
				undo/redo actions.
	Errors:		-   
	Scope:		public
	SeeAlso:	

********************************************************************************************/
BOOL OperationHistory::IsReduced()
{ 
	return Reduced;
} 



/********************************************************************************************

>	BOOL OperationHistory::ReduceSize(UINT32 MaxSize, BOOL ExcludeLastUndo, 
									  BOOL DeleteWhatYouCan = FALSE)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/7/93
	Inputs:		MaxSize: The maximum size that the operation history should 
						 be reduced to.                       
						 
				ExcludeLastUndo: If this flag is TRUE then the routine will 
								 never delete the last undo operation in trying
								 to reduce the size of the operation history.
								 
				DeleteWhatYouCan: If we cannot reduce the size of the operation history as low 
								  as MaxSize bytes, and this flag is TRUE, then as many ops as 
								  possible are deleted.	
	Outputs:	-
	Returns:	FALSE if the size of the operation history cannot be reduced to MaxSize bytes 
				or less, Otherwise returns TRUE.        
	
	Purpose:	Tries to Reduce the current size of the operation history so that it is at most 
				MaxSize bytes long. The size is reduced by deleting undo operations 
				(Oldest first), redo operations are never deleted!. If the Operation history
				would be larger than MaxSize bytes after deleting all undo operations, then 
				no operations are deleted and the function returns FALSE.  
	
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL OperationHistory::ReduceSize(UINT32 MaxHistorySize, BOOL ExcludeLastUndo, BOOL DeleteWhatYouCan /*= FALSE*/)
{                      
	// If the Current size of the operation history is less than or equal to MaxHistorySize then 
	// there is no need to do anything. 
	if ((CurrentSize <= MaxHistorySize))
    	return (TRUE);          
    
    // If The NowPtr is NULL then there are no undo operations to be deleted so return FALSE
    if (NowPtr == NULL)
    	return (FALSE); 
    	
  	// Calculate how many bytes we need to reduce the size of the Operation history by 
    UINT32 Reduction = (CurrentSize - MaxHistorySize);                                
    
    //-------------------------------------------------------------------------------
	// Check if the operation history can be reduced to MaxHistorySize bytes or less
                 
    // The OpSize total is the count of the number of bytes we can reduce the Operation 
    // history by. 
	UINT32 OpSizeTotal = 0;                           
                   
    // We know that the NowPtr is not NULL so the oldest undo operation will be found at 
    // the head of the OpHistoryList. 
	ListItem* pOp = OpHistoryList.GetHead();   
	
	// We are allowed to delete all operations from the head of the list 
	// upto and excluding StopOp. StopOp is the last undo operation if the 
	// ExcludeLastUndo flag is TRUE, else it is the first redo operation.                       
	ListItem* StopOp = (ExcludeLastUndo) ? NowPtr: OpHistoryList.GetNext(NowPtr); 
	
	
	// Loop until we either hit StopOp or we have found enough operations to delete 
	while ((pOp != StopOp) && (OpSizeTotal < Reduction))
	{                   
		// In a sane world this should always be true
		ENSURE(	pOp != NULL, 
				"OperationHistory::ReduceSize: Pointer OperationHistory is NULL"); 
		
		// Increase the OpSizeTotal by the number of bytes of the current operation 
		OpSizeTotal += ((Operation*)pOp)->GetSize();                               
		
		// Get the next operation
		pOp = OpHistoryList.GetNext(pOp);    
	};    
	
	//-------------------------------------------------------------------------------
	// Now if we can,  reduce the operation history size 
	
	if ((OpSizeTotal >= Reduction) || (DeleteWhatYouCan && (OpSizeTotal != 0))) // The size can be reduced 
	{                 
		// Start at the head of the OpHistoryList
		ListItem* pDeleteOp = OpHistoryList.GetHead();   
		
		#ifdef _DEBUG      
		UINT32 TotalChk = 0; 
		#endif
		
		while (pDeleteOp != pOp)
		{      
			DecSize(((Operation*)pDeleteOp)->GetSize());  // Reduce history size     
			
			#ifdef _DEBUG      
			TotalChk += ((Operation*)pDeleteOp)->GetSize(); 
			#endif
			ListItem* pNextDeleteOp = OpHistoryList.GetNext(pDeleteOp);       
			
			// If the operation which is about to be deleted is the operation pointed to by the NowPtr
			// then it is the last undo operation, so set NowPtr to NULL. 
			if (NowPtr == pDeleteOp) 
				NowPtr = NULL; 
			delete(OpHistoryList.RemoveItem(pDeleteOp));         // Delete the operation   
			pDeleteOp = pNextDeleteOp; 
		}	       

		// Defensive programming 

		#ifdef _DEBUG                       // Required because of TotalChk variable
		ENSURE(	OpSizeTotal == TotalChk, 
				"OperationHistory::ReduceSize: OpSizeTotal != TotalChk");  
		#endif             
		             

		Reduced = TRUE;
		
		return (TRUE); 
	}                                                     
	else
		return (FALSE); // Cannot reduce size of history to MaxHistorySize bytes or less
} 


/********************************************************************************************

>	BOOL OperationHistory::UndoPrev()
                                       
	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/7/93
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if an operation was successfully undone, FALSE otherwise. 
	Purpose:	To UNDO the previous operation 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
              
BOOL OperationHistory::UndoPrev()
{                       
	//TRACE( _T("Called Undo\n"));   
	if (NowPtr != NULL) // Check if there is an operation to UNDO                 
	{
		// Tell the world that the op is about to be undone
		BROADCAST_TO_ALL(OpMsg(((Operation*)NowPtr),OpMsg::BEFORE_UNDO)); 
		Operation* pOp = (Operation*)NowPtr;

		if (pOp->Undo()) // Undo the operation
        {
			ERROR3IF(NowPtr == NULL, "The operation which has just been undone has been deleted"); 
			if (NowPtr != NULL)
			{
				// We used to find the prev op before the undo, this was wrong because it may have been deleted.
				NowPtr = OpHistoryList.GetPrev(NowPtr);
			}
			Operation* NextOp;
			if (NowPtr != NULL)
			{
				NextOp = ((Operation*)OpHistoryList.GetNext(NowPtr));	  
			}
			else
			{
				NextOp = ((Operation*)OpHistoryList.GetHead());
			}
			BROADCAST_TO_ALL(OpMsg(NextOp,OpMsg::AFTER_UNDO)); 
			return (TRUE);  
		} 
	}
	return (FALSE); // There are no operations to UNDO, or the Operation failed to undo	
}


/********************************************************************************************
                       
>	BOOL OperationHistory::RedoNext() 
	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/7/93
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if an operation was successfully redone, FALSE otherwise. 
	Purpose:	To REDO the next operation
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
 
BOOL OperationHistory::RedoNext() 
{             
	ListItem* next;                   
	
	if (NowPtr == NULL)  
	// If the NowPtr is NULL then if there are any redo operations then the first will be 
	// found at the head of the OpHistoryList            
		next = OpHistoryList.GetHead(); 
	else           
		// The first redo operation will be found after the NowPtr
		next = OpHistoryList.GetNext(NowPtr);
	if ( next != NULL) 
	{
		Operation* pOp = (Operation*)next;
	   	BROADCAST_TO_ALL(OpMsg(pOp,OpMsg::BEFORE_REDO)); 
		// There is an operation to redo 
		if (pOp->Redo()) // REDO the operation    
		{
			NowPtr = next;
		   	BROADCAST_TO_ALL(OpMsg(((Operation*)NowPtr), OpMsg::AFTER_REDO)); 
			return (TRUE);   
		}
	}                  
	return (FALSE); // There are no operations to REDO
}                   

/********************************************************************************************

>	BOOL OperationHistory::CanUndo() 
	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/7/93
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if there are any operations to UNDO, FALSE otherwise.
	Purpose:	To find out whether undo is possible.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
 
BOOL OperationHistory::CanUndo() 
{
	return ( NowPtr != NULL );
}

/********************************************************************************************

>	BOOL OperationHistory::CanRedo() 
	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/7/93
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if there are any operations to REDO, FALSE otherwise.
	Purpose:	To find out whether a redo is possible.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
 
BOOL OperationHistory::CanRedo() 
{                    
	if (NowPtr == NULL)    
		// If the NowPtr is NULL then the first redo operation will be found at the head of the 
		// OpHistory list
		return (OpHistoryList.GetHead() != NULL); 
	else             
		// The first redo operation will be found at the node after NowPtr
		return ((OpHistoryList.GetNext(NowPtr)) != NULL);
}               


/********************************************************************************************

>	void OperationHistory::GetUndoOpName(String_256* OpName)

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/10/93
	Outputs:	OpName String - name of the operation to be undone next
	Purpose:	To get the name of the currently undoable operation

********************************************************************************************/

void OperationHistory::GetUndoOpName(String_256* OpName)
{
	((Operation*) NowPtr)->GetOpName(OpName);
}

/********************************************************************************************

>	void OperationHistory::GetRedoOpName(String_256* OpName)

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/10/93
	Outputs:	OpName String - name of the operation to be undone next
	Purpose:	To get the name of the currently undoable operation

********************************************************************************************/
 
void OperationHistory::GetRedoOpName(String_256* OpName) 
{                          
	if (NowPtr == NULL)    
		// If the NowPtr is NULL then the first redo operation will be found at the head of the 
		// OpHistory list
		((Operation*) OpHistoryList.GetHead())->GetOpName(OpName);
	else             
		// The first redo operation will be found at the node after NowPtr
		((Operation*) OpHistoryList.GetNext(NowPtr))->GetOpName(OpName);
}                   

#ifdef _DEBUG

/********************************************************************************************

>	void OperationHistory::DebugSizeCheck()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/7/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	For validating that the sum of the sizes of actions in the operation history
				equals the current size of the operation history. 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

 
void OperationHistory::DebugSizeCheck()
{
	UINT32 TotalOfActionSizes = 0; 
	ListItem* pOp = OpHistoryList.GetHead();  // Get first operation
	
	while (pOp != NULL)                        // For each operation
	{       
		// Add current operation size to the action total 
		TotalOfActionSizes += ((Operation*)pOp)->GetSize();  
		pOp = OpHistoryList.GetNext(pOp); 
	}                        
	//if (IsUserName("Simon"))
	//{        
		//TRACE( _T("Current Size = %lu, Action Total = %lu\n"), CurrentSize, TotalOfActionSizes); 
	//}                   
	// Cause a blobby if the total of all action sizes does not equal the current 
	// size of the operation history. 
	ENSURE(TotalOfActionSizes == CurrentSize, "The size of the operation history is invalid");   
}                

#endif  
   
/********************************************************************************************

>	void OperationHistory::DeleteUndoableOps()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/7/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	If any undoable operations exist then the method deletes them all.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
   
void OperationHistory::DeleteUndoableOps()
{
#if FALSE
// Removed by Jason
// This was deleting backwards to the desired direction!
// Direction of deletion has been reversed in the #else section below

/*
	if 	(NowPtr != NULL)  // Undoable operations exist
	{	 
		ListItem* CurrentOp = NowPtr; // First undoable operation 
		// Loop until all undoables have been deleted
        do
		{                       
			ListItem* PrevOp = OpHistoryList.GetPrev(CurrentOp); 
			// The size of the operation history is reduced by the size of the  
			// operation just about to be deleted.   
			DecSize(((Operation*)CurrentOp)->GetSize());  
			// Actually delete the current operation 
			delete((Operation*)(OpHistoryList.RemoveItem(CurrentOp)));       
			// Get next op
			CurrentOp = PrevOp; 
		}  while (CurrentOp != NULL); 
    	NowPtr = NULL; 					// No undoable operations
    }   
*/
#else
	if 	(NowPtr != NULL)  // Undoable operations exist
	{	 
		ListItem* CurrentOp = OpHistoryList.GetHead(); // First undoable operation 
		// Loop until all undoables have been deleted

		if (CurrentOp == NULL)		// No Undo records?! (Shouldn't happen if NowPtr != NULL)
			return;

        do
		{                       
			ListItem* NextOp = OpHistoryList.GetNext(CurrentOp);
			// The size of the operation history is reduced by the size of the  
			// operation just about to be deleted.   
			DecSize(((Operation*)CurrentOp)->GetSize());  
			// Actually delete the current operation 
			
			delete((Operation*)(OpHistoryList.RemoveItem(CurrentOp)));
			if (CurrentOp == NowPtr)	// Delete NowPtr item as well
				CurrentOp = NULL;		// it is gone, so we MUST stop now
			else
				CurrentOp = NextOp;		// Continue with the next Op

		}  while (CurrentOp != NULL);
    	NowPtr = NULL; 					// No undoable operations
    }
#endif
} 
   
/********************************************************************************************

>	void OperationHistory::DeleteRedoableOps()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/7/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	If any redoable operations exist then the method deletes them all.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
    
void OperationHistory::DeleteRedoableOps()
{   
#if FALSE
// Removed by Jason
// This was deleting backwards to the desired direction!
// Direction of deletion has been reversed in the #else section below

/*
	ListItem* CurrentOp;    
		        
	// Find the first redoable operation        
	if (NowPtr == NULL)                        
	// If the NowPtr is NULL then the first redoable operation will be found at the 
	// head of the OpHistoryList
		CurrentOp = OpHistoryList.GetHead(); // Head of list is first redoable
	else                        
	// The first redoable operation will be found at the node after NowPtr
		CurrentOp = OpHistoryList.GetNext(NowPtr); 
      
    // Loop while there are redo operations to delete  
	while(CurrentOp != NULL)
	{              
		ListItem* NextOp = OpHistoryList.GetNext(CurrentOp); 
		// The size of the operation history is reduced by the size of the  
		// deleted operation.   
		DecSize(((Operation*)CurrentOp)->GetSize());   
		// Actually delete the operation 
		delete( (Operation*)((OpHistoryList.RemoveItem(CurrentOp))) );   
		// Get the next op      
		CurrentOp = NextOp; 
	}
*/
#else
	ListItem* CurrentOp;
	ListItem* LastOp;
		        
	// Find the first redoable operation        
	if (NowPtr == NULL)                        
	// If the NowPtr is NULL then the first redoable operation will be found at the 
	// head of the OpHistoryList
		LastOp = OpHistoryList.GetHead(); // Head of list is first redoable
	else                        
	// The first redoable operation will be found at the node after NowPtr
		LastOp = OpHistoryList.GetNext(NowPtr); 

	if (LastOp == NULL)		// No redo list at all
		return;

	CurrentOp = OpHistoryList.GetTail();
    
    // Loop while there are redo operations to delete  
	while(CurrentOp != LastOp)
	{              
		ListItem* PrevOp = OpHistoryList.GetPrev(CurrentOp); 
		// The size of the operation history is reduced by the size of the  
		// deleted operation.   
		DecSize(((Operation*)CurrentOp)->GetSize());
		// Actually delete the operation
		delete( (Operation*)((OpHistoryList.RemoveItem(CurrentOp))) );
		// Get the next op
		CurrentOp = PrevOp;
	}

	// And also delete the last redoable op
	DecSize(((Operation*)LastOp)->GetSize());
	delete( (Operation*)((OpHistoryList.RemoveItem(LastOp))) );
#endif
}

/********************************************************************************************

>	void OperationHistory::Add(ListItem* Op)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/7/93
	Inputs:		Op: New operation to be added to the history
	Outputs:	-
	Returns:	-
	Purpose:	To add operation Op to the operation history. At present this function
				also deletes all 'REDO' operations.
				
				Note that when an operation is added to the operation history we do not 
				increase its size. The size will have already been increased when actions
				were initialised. 
				
	Errors:		-       
	Scope:		private
	SeeAlso:	-

********************************************************************************************/
/* 
Technical Notes: 

Currently when a new operation is added to the operation history, all redoable operations
are deleted. This may change if we implement a more advanced UNDO system. 
*/     
    
void OperationHistory::Add(ListItem* Op)
{ 
	if (NowPtr == NULL)
	{             
		// There are currently no undo operations so add the new Op to the head of the 
		// OpHistory list. 
		NowPtr = Op; 
		OpHistoryList.AddHead(Op);  
 	}
	else                
	{                      
		// Insert the new operation after the current undoable operation 
		OpHistoryList.InsertAfter(NowPtr, Op);                           
		// The new operation becomes the current undoable operation 
		NowPtr = Op; 
	}	
	// Delete all REDOable operations for now (This may change if we start getting clever) 
    DeleteRedoableOps();
    //if (IsUserName("Simon"))
    //{
	//	TRACE( _T("Size of operation history = %lu"), CurrentSize); 
    //}     
   
}

// -------------------------------------------------------------------------------------
// The following functions are useful for operation merging

/********************************************************************************************

>	Operation* OperationHistory::FindLastOp() 

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/11/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	This function returns a pointer to the last operation which was performed 
				(but not undone). i.e. The NowPtr. It is provided to allow the merging of 
				operations. NULL is returned if no such operation exists

	Errors:		-
	SeeAlso:	OperationHistory::FindPrevToLastOp

********************************************************************************************/

Operation* OperationHistory::FindLastOp()
{
	return (Operation*)NowPtr;
}


/********************************************************************************************

>	Operation* OperationHistory::FindPrevToLastOp() 

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/11/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	This function returns a pointer to the operation which was performed 
				(and not undone) prior to the last operation. This is the op which will 
				usually need to be changed/extended when we are merging operations.	 NULL
				is returned if no such operation exists.

	Errors:		-
	SeeAlso:	OperationHistory::FindLastOp

********************************************************************************************/


Operation* OperationHistory::FindPrevToLastOp() 
{
	if (NowPtr != NULL)
	{
		return ((Operation*)(OpHistoryList.GetPrev(NowPtr)));
	}
	else
	{
		return NULL; 
	}
}

/********************************************************************************************

>	void OperationHistory::DeleteLastOp(BOOL ReduceOpHistSize = TRUE)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/11/94
	Inputs:		ReduceOpHistSize: When TRUE the size of the OperationHistory is reduced by 
				the size of the operation being deleted.
	Outputs:	-
	Returns:	-
	Purpose:	This function deletes the last operation which was performed. It is useful
				to do this after we have merged this last operation with the previous operation.
				The size of the Operation history is reduced by the size of this operation if
				the ReduceOpHistSize flag is TRUE. 

	Errors:		-
	SeeAlso:	OperationHistory::FindLastOp
				OperationHistory::FindPrevToLastOp

********************************************************************************************/

void OperationHistory::DeleteLastOp(BOOL ReduceOpHistSize)
{  
	Operation* pOpToDelete = FindLastOp(); 

	// The NowPtr needs to be set to the previous operation - if one exists
	// if not then NULL is an OK state for the NowPtr
	NowPtr = OpHistoryList.GetPrev(NowPtr); 

	if (ReduceOpHistSize)
	{
		DecSize(pOpToDelete->GetSize());  // Reduce history size 
	}    
			
	OpHistoryList.RemoveItem((ListItem*)pOpToDelete);   // Remove the operation from the
														// history list but don't delete it yet
	pOpToDelete->DeleteOnEnd();							// Mark it for deletion when it's safe
														// to do so.
} 


/********************************************************************************************

>	void OperationHistory::DeletePrevToLastOp(BOOL ReduceOpHistSize)

	Author:		Stefan_Stoykov (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/08/97
	Inputs:		ReduceOpHistSize: When TRUE the size of the OperationHistory is reduced by 
				the size of the operation being deleted.
	Outputs:	-
	Returns:	-
	Purpose:	This function deletes the previous to last operation which was performed. It is 
				useful to do this after we have merged the last operation with the previous 
				operation. The size of the Operation history is reduced by the size of this 
				operation if the ReduceOpHistSize flag is TRUE. 

	Errors:		-
	SeeAlso:	OperationHistory::FindLastOp
				OperationHistory::FindPrevToLastOp

********************************************************************************************/

void OperationHistory::DeletePrevToLastOp(BOOL ReduceOpHistSize)
{  
	// find the previous operation
	Operation *pPrev = (Operation *)OpHistoryList.GetPrev(NowPtr); // get the PrevToLastOp

	if (pPrev != NULL)
	{
		if (ReduceOpHistSize)
		{
			DecSize(pPrev->GetSize());  // Reduce history size 
		}    

		// remove it from the list			
		OpHistoryList.RemoveItem((ListItem*)pPrev);   // Remove the operation from the
													  // history list

		// delete it
		delete pPrev;
	}
}


/********************************************************************************************

>	void OperationHistory::DumpAll()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/8/95
	Inputs:		-
	Outputs:	Dumps debugging information to the debugger display
	Returns:	-
	Purpose:	This function displays debugging information on all the operations in the
				history list
	Errors:		-
	SeeAlso:	OperationHistory::DumpLast
				Operation::Dump

********************************************************************************************/
void OperationHistory::DumpAll()
{
#if DEBUG_TREE
	// loop through all the ops in the history, calling their dump functions
	ListItem* CurrentOp = OpHistoryList.GetHead();

	TRACEALL( _T("******* Complete Operation history dump *******\n") );

	if (CurrentOp == NULL)
		TRACEALL( _T("No operations in history\n") );
	else
	{
	    do
		{
			((Operation*)CurrentOp)->Dump();

			CurrentOp = OpHistoryList.GetNext(CurrentOp);
		} while (CurrentOp != NULL);
	}

	TRACEALL( _T("******* End of history dump *******\n\n") );

#endif
}



/********************************************************************************************

>	void OperationHistory::DumpAll()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/8/95
	Inputs:		-
	Outputs:	Dumps debugging information to the debugger display
	Returns:	-
	Purpose:	This function displays debugging information on the last operation in the
				operation history.  This is the "current" one.
	Errors:		-
	SeeAlso:	OperationHistory::DumpAll
				Operation::Dump

********************************************************************************************/
void OperationHistory::DumpLast()
{
#if DEBUG_TREE
	Operation* CurrentOp = FindLastOp();

	TRACEALL( _T("\n******* Dump of last operation *******\n") );

	if (CurrentOp == NULL)
		TRACE( _T("No operations in history\n") );
	else
		CurrentOp->Dump();

	TRACEALL( _T("\n******* End of last operation dump *******\n\n") );

#endif
}


#if !defined(EXCLUDE_FROM_RALPH)

// -------------------------------------------------------------------------------------


//********************************************************************************************
//									UNDO OPERATION
  
/********************************************************************************************
   
>	OpUndo::OpUndo()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/7/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Constructs an OpUndo object.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

OpUndo::OpUndo(): Operation()
{
}

/********************************************************************************************

>	void OpUndo::Do(OpDescriptor*)

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com> Shamatani
	Created:	11/8/93
	Inputs:		Pointer to an OpDescriptor (not used)
	Outputs:	-
	Returns:	-
	Purpose:	Actually "DO" the undo operation.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void OpUndo::Do(OpDescriptor*)
{
	// Remember our document, because in debug builds we need to access it after End()
	// has been called, and End() deletes the operation.
	Document *pDoc = pOurDoc;

//	#ifdef _DEBUG      
//	// Ensure that the size of the operation history is valid before the undo
//	pDoc->GetOpHistory().DebugSizeCheck();   
//	#endif

	pDoc->GetOpHistory().UndoPrev(); // Undo the previous operation
	End();     
	
	// if there are no more actions to undo and the operation history has not been reduced
	// then mark the document as un-modified.
	if ( (!(pDoc->GetOpHistory().CanUndo())) && (!(pDoc->GetOpHistory().IsReduced())) )
	{
		pDoc->SetModified(FALSE); 
	}

	#ifdef _DEBUG      
	// Ensure that the size of the operation history is valid after the undo
	pDoc->GetOpHistory().DebugSizeCheck();   
	#endif
}

/********************************************************************************************

>	OpState OpUndo::GetState(String_256* UIDescription, OpDescriptor*)

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/8/93
	Inputs:		OpDescriptor - To help update the state
	Outputs:	String - if MenuItem is enabled Then return the name of the operation to 
							be undone Else return the reason why it is disabled
	Returns:	-
	Purpose:	Find the state of the OpUndo operation.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

OpState OpUndo::GetState(String_256* UIDescription, OpDescriptor*)
{
	OpState OpSt;

	// Undo always works on the selected document...
	Document *pDocument = Document::GetSelected();

	if (pDocument)
	{
    	OperationHistory *pOpHist = &(pDocument->GetOpHistory());
    	
		// Is there an Operation History?
	   	if (pOpHist)
		{
		    // The Undo operation can only be invoked if there are any operations to undo.             
			if (pOpHist->CanUndo())
			{
				// Temporary String used for Operation Name
				String_256 OpName;
			
				// Get the name of operation to be undone
				pOpHist->GetUndoOpName(&OpName);
		
				//Concatenate The Menu Description with the operation name
				String Space = _T(" ");
				*UIDescription += Space;
				*UIDescription += OpName;
				
				return OpSt;
			}
		}
	}

	//ELSE set to state to disabled

	OpSt.Greyed = TRUE;
		
	// Load reason why operation is disabled
	String_256 DisableReason(_R(IDS_UNDO_DISABLED));
		
   	*UIDescription = DisableReason;
    
	return(OpSt);
}


/********************************************************************************************

>	BOOL OpUndo::Init()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/7/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Create an OpDescriptor for the Undo operation
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL OpUndo::Init()
{
	return(
			Operation::RegisterOpDescriptor(
									0, 
									_R(IDS_EDIT_UNDO),
									CC_RUNTIME_CLASS(OpUndo), 
									OPTOKEN_UNDO,
									OpUndo::GetState,
									HID_EDIT_UNDO,
									_R(IDBBL_UNDOOP),
									_R(IDD_BARCONTROLSTORE),
									_R(IDC_BTN_EDITUNDO),
									SYSTEMBAR_EDIT
								)
		); 

}


//********************************************************************************************
//									REDO OPERATION




/********************************************************************************************

>	OpRedo::OpRedo(): Operation()


	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/7/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Constructs an OpRedo object.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

OpRedo::OpRedo(): Operation()
{
}

/********************************************************************************************

>	void OpRedo::Do(OpDescriptor*)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/7/93
	Inputs:		An Operation Descriptor
	Outputs:	-
	Returns:	-
	Purpose:	Actually "DO" a redo operation.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void OpRedo::Do(OpDescriptor*)
{
//	ENSURE(FALSE,"Someone's trying to Redo!");

	// Remember our document, because in debug builds we need to access it after End()
	// has been called, and End() deletes the operation.
	Document *pDoc = pOurDoc;
	pDoc->GetOpHistory().RedoNext();
	End();          
	
	#ifdef _DEBUG      
	// Ensure that the size of the operation history is valid 
	pDoc->GetOpHistory().DebugSizeCheck();   
	#endif
}



/********************************************************************************************

>	OpState OpRedo::GetState(String_256* UIDescription, OpDescriptor*)

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/8/93
	Inputs:		OpDescriptor - To help update the state
	Outputs:	String - if MenuItem is enabled Then return the name of the operation to 
							be undone Else return the reason why it is disabled
	Returns:	-
	Purpose:	Find the state of the OpRedo operation.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

OpState OpRedo::GetState(String_256* UIDescription, OpDescriptor*)
{
	OpState OpSt;     

	// Undo always works on the selected document...
	Document *pDocument = Document::GetSelected();

	if (pDocument)
	{
    	OperationHistory *pOpHist = &(pDocument->GetOpHistory());
    	
		// Is there an Operation History?
	   	if (pOpHist)
		{
		    // The Redo operation can only be invoked if there are any operations to redo.             
			if (pOpHist->CanRedo())
			{
				// Temporary String used for Operation Name
				String_256 OpName;
			
				// Get the name of operation to be undone
				pOpHist->GetRedoOpName(&OpName);
		
				//Concatenate The Menu Description with the operation name
				String Space = _T(" ");
				*UIDescription += Space;
				*UIDescription += OpName;
				
				return OpSt;
			}
		}
	}

	//ELSE set to state to disabled

	OpSt.Greyed = TRUE;
		
	// Load reason why operation is disabled
	String_256 DisableReason(_R(IDS_REDO_DISABLED));
		
   	*UIDescription = DisableReason;
    
	return(OpSt);	
}                                    



/********************************************************************************************

>	BOOL OpRedo::Init()

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/8/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Create an OpDescriptor for the Redo operation
	Errors:		ERROR is called if there was insufficient memory to create the operation 
	SeeAlso:	-

********************************************************************************************/

BOOL OpRedo::Init()
{
	return(Operation::RegisterOpDescriptor( 
											0, 
											_R(IDS_EDIT_REDO),
											CC_RUNTIME_CLASS(OpRedo), 
											OPTOKEN_REDO,
											OpRedo::GetState,
											HID_EDIT_REDO,
											_R(IDBBL_REDOOP),
											_R(IDD_BARCONTROLSTORE),
											_R(IDC_BTN_EDITREDO),
											SYSTEMBAR_EDIT
											)
		  ); 

}




#endif
