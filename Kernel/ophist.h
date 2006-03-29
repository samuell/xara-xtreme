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

// Declaration of the following classes
/*
	OperationHistory : The operation history used to store undoable operations
	OpUndo           : The undo operation                                    
	OpRedo           : The redo operation
	
	UndoColAction    : BlobbyOp undo action
	RedoColAction    : BlobbyOp redo action
*/	



/*
*/        

#ifndef INC_OPHIST
#define INC_OPHIST
    
#include "list.h"
#include "undoop.h"
#include "opdesc.h"   
#include "strings.h"

#define OPTOKEN_UNDO _T("Undo")
#define OPTOKEN_REDO _T("Redo")
#define OPTOKEN_BLOBBY _T("Blobby")


/********************************************************************************************

>	class OperationHistory : public CCObject

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/7/93
	Purpose:	This class represents the operation history for a camelot document. It is a
				repository for all undoable operations performed in Camelot. It stores 
				operations in chronological order and provides methods to undo and redo 
				them. 
				
	SeeAlso:	-

********************************************************************************************/

class CCAPI OperationHistory: public CCObject
{
	CC_DECLARE_MEMDUMP( OperationHistory )        
     
	friend class ActionList;		// It is neccesary for the ActionList and Action classes to be 
	friend class Action; 			// friends of the OperationHistory class, as they both need to call 
     								// the IncCurrentSize and DecCurrentSize methods. No one else should 
     								// be allowed access to these methods as they are extremely dangerous.  

	friend class Operation;			// It is neccesary for the Operation class to be a friend of the 
	 								// OperationHistory class because it needs to call the Add method 
	 								// of the Operation history. This is another method which falls 
	 								// in the extremely dangerous category. 

	friend class OpDeleteBitmap;	// These classes need to scan all the document operation 
	friend class GlobalBitmapList;	// histories, looking for Bitmap References.
    
public:                              
	// Constructors
	OperationHistory(); 			
	OperationHistory(UINT32 MaximumSize);   
	               
	// Destructor
	~OperationHistory(); 
	             
	// Methods for managing the size of the operation history  
           
	BOOL SetNewMaxSize(UINT32 NewMaxSize); 
	UINT32 GetSize(); 
	UINT32 GetMaxSize();     
	BOOL ReduceSize(UINT32 MaxSize, BOOL ExcludeLastUndo, BOOL DeleteWhatYouCan = FALSE);   
	UINT32 GetNumUndoSteps();
	UINT32 GetNumRedoSteps();
	BOOL IsReduced();
		
	// Methods used to Undo/Redo operations
	BOOL UndoPrev(); 
	BOOL RedoNext(); 
	BOOL CanUndo();
	BOOL CanRedo();   
	
	// These methods will identify the name of the particular operation being undone
	// or redone
	void GetUndoOpName(String_256* OpName);
	void GetRedoOpName(String_256* OpName);

	// -------------------------------------------------------------------------------------
	// The following functions are useful for operation merging

	Operation* FindLastOp(); 

	// This function finds the operation which was performed prior to the last operation.
	Operation* FindPrevToLastOp(); 

	// One of these function should get called after the last operation performed has been merged with
	// the previous operation which has been performed.
	void DeleteLastOp(BOOL ReduceOpHistSize = TRUE); 
	void DeletePrevToLastOp(BOOL ReduceOpHistSize = TRUE);

	// function for dumping the operation history to the debugger
	void DumpAll();
	void DumpLast();

	// -------------------------------------------------------------------------------------

#ifdef _DEBUG
	void DebugSizeCheck();
#endif
	void DeleteUndoableOps(); 
	void DeleteRedoableOps(); 
private:            

	// methods called by the ActionList and Action classes
	void IncSize(UINT32 SizeIncrement);  
	void DecSize(UINT32 SizeDecrement);            
	
	// Add should be called by the operation's End method and no where else.
	void Add(ListItem* Operation); 
      
	UINT32 MaxSize;           // The maximum size of the operation history (bytes)
	UINT32 CurrentSize;       // The current size of the operation history (bytes)
	List OpHistoryList;      // List holding past and future operations    
	 
	ListItem* NowPtr;        // Pointer to the last operation in the OpHistoryList which has 
							 // been performed and not undone. The operation to which the NowPtr 
							 // points, and all previous operations are undoable (They have 
							 // actions on their undo action lists). All operations in the 
							 // OpHistoryList after the NowPtr are redoable (They have actions 
							 // on their redo action lists). When the NowPtr is NULL then no 
							 // undo operations exist in the operation history, in this situation
							 // if any redo operations exist then the first will be found at the 
							 // head of the OpHistoryList.  
	BOOL Reduced;			 // Set to true if records have been discarded due to lack of memory
};                

/********************************************************************************************

>	class OpUndo : public Operation

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/7/93
	Purpose:	This class represents the Undo operation.
	SeeAlso:	OpRedo

********************************************************************************************/

class CCAPI OpUndo: public Operation
{         

	CC_DECLARE_DYNCREATE( OpUndo )    

public:
	OpUndo();								// Constructor

	static BOOL		Init();					// Register an OpDescriptor
	                                        // Read the state of an operation
	static OpState	GetState(String_256*, OpDescriptor*);		

	void			Do(OpDescriptor*);		// "Do" function    
};


/********************************************************************************************

>	class OpRedo : public Operation

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/7/93
	Purpose:	This class represents the Redo operation.
	SeeAlso:	OpRedo

********************************************************************************************/

class CCAPI OpRedo: public Operation
{                           

	CC_DECLARE_DYNCREATE( OpRedo )    
    
public:
					OpRedo();				// Constructor

	static BOOL		Init();					// Register an OpDescriptor
	                                        // Read the state of an operation
	static OpState	GetState(String_256*, OpDescriptor*);		

	void			Do(OpDescriptor*);		// "Do" function

};


/********************************************************************************************/
//

#endif
