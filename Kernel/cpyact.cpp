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
// Implementation of CopyObjectsToClipboardAction
/*
*/

#include "camtypes.h"
#include "cpyact.h"
#include "clipint.h" // Internal clipboard
//#include "app.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "ophist.h"

DECLARE_SOURCE($Revision$);

CC_IMPLEMENT_DYNCREATE( CopyObjectsToClipboardAction, Action )


#define new CAM_DEBUG_NEW

/********************************************************************************************

>	CopyObjectsToClipboardAction::CopyObjectsToClipboardAction()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/9/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	CopyObjectsToClipboardAction constructor
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
    
CopyObjectsToClipboardAction::CopyObjectsToClipboardAction()
{
}

/********************************************************************************************

>	CopyObjectsToClipboardAction::~CopyObjectsToClipboardAction()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/9/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	CopyObjectsToClipboardAction destructor
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
   
             
CopyObjectsToClipboardAction::~CopyObjectsToClipboardAction() 
{
}   


/********************************************************************************************

>	virtual ActionCode CopyObjectsToClipboardAction::Execute()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/8/93
	Inputs:		-
	Outputs:	ActionCode indicating if the action was successfully executed or not
	Returns:	-
	Purpose:	Executes the CopyObjectsToClipboardAction which when executed on an undo list
				does nothing except spawn a redo  CopyObjectsToClipboardAction

				When executed on a redo action list copies all objects to the clipboard
	Errors:		-
	SeeAlso:	-
    
    
********************************************************************************************/
 
ActionCode CopyObjectsToClipboardAction::Execute()
{  
    ActionCode ActCode;  
    
    // Create the twin action 
	CopyObjectsToClipboardAction* CpyObjAct; 

	if ( (ActCode = CopyObjectsToClipboardAction::Init(pOperation, 
			 			    			  	  pOppositeActLst, 
								   			  NodeRange, 
							  	   			  (Action**)(&CpyObjAct)))
		!= AC_FAIL)  
	{   
		// Is this a redo action ?
		if  (pOppositeActLst == pOperation->GetUndoActionList())
		{
			BOOL CopiedOK; 
		
			// After the operation ends we will need to inform all DocComponents in the clipboard
			// of the outcome.
			pOperation->InformDocComponentsOfOperationsOutcome(InternalClipboard::Instance()); 

			// Inform all DocComponents in the clipboard that a copy is about to take place
			BOOL ok;
			CALL_WITH_FAIL(InternalClipboard::Instance()->StartComponentCopy(), pOperation, ok)
			if (!ok)
			{
				// Start Component copy has failed so abort operation
				// Note that AbortComponentCopy will get called in the ops end method
				return AC_FAIL;
			} 


			// Copy the selection to the clipboard
			CALL_WITH_FAIL(InternalClipboard::CopyObjects(NodeRange, pOperation), pOperation, CopiedOK)

			if (!CopiedOK)
			{
				return AC_FAIL; 
			}

			// Now try and copy accross the component data
			CALL_WITH_FAIL(InternalClipboard::CopyComponentData(GetWorkingDoc()),
																pOperation,
																CopiedOK)
			if (!CopiedOK) return AC_FAIL; 
		}
		// else do nothing
	};  
			        
	return ActCode;
}     


/********************************************************************************************

>	static ActionCode CopyObjectsToClipboardAction::Init(Operation* const pOp, 
						   						ActionList* pActionList, 
						   						Range NodeRange, 
						  						Action** NewAction) 
	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/9/93  
	
	Inputs:		pOp: The operation to which the action should be added
				
				pActionList: The action list in the operation object
				
							 
				NodeRange: 	 The range of nodes to copy 


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
				of a CopyObjectsToClipboardAction.
	Errors:		-
	SeeAlso:	Action::Init

********************************************************************************************/

ActionCode CopyObjectsToClipboardAction::Init(Operation* const pOp, 
						   			 ActionList* pActionList, 
						   			 Range NodeRange, 
						  			 Action** NewAction) 

{ 
	ActionCode Ac = (Action::Init(pOp,
					 			  pActionList,
					 			  sizeof(CopyObjectsToClipboardAction),
					 			  CC_RUNTIME_CLASS(CopyObjectsToClipboardAction), 
					 			  NewAction));   
						   
	if (Ac != AC_FAIL)
		if (*NewAction != NULL)  
		{
			// Store the range and transform in the action 
			((CopyObjectsToClipboardAction*)(*NewAction))->NodeRange = NodeRange;
		}
                
	return (Ac); 
} 

