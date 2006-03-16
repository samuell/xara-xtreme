// $Id: opdesc.cpp 662 2006-03-14 21:31:49Z alex $
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
// OpDescriptor allows camelot to maintain a list of all the operations 
// which can be performed

/*
*/

#include "camtypes.h"
#include "opdesc.h"
#include "ensure.h"
#include "ops.h"
//#include "dialogop.h"
#include "menuops.h"
#include "bars.h"
//#include "oilprog.h"			// For Beep fn
#include "progress.h"
//#include "simon.h"
//#include "phil.h"
#include "document.h"
#include "camdoc.h"
#include "app.h"
#include "tool.h"
#include "ctrllist.h"

DECLARE_SOURCE("$Revision: 662 $");


CC_IMPLEMENT_DYNAMIC(OpDescriptor, MessageHandler)
CC_IMPLEMENT_DYNAMIC(ToolOpDescriptor, OpDescriptor)
CC_IMPLEMENT_DYNAMIC(UndoableOpDescriptor, OpDescriptor)
CC_IMPLEMENT_DYNAMIC(ParamOpDescriptor, OpDescriptor);

// OpDescriptor messages
CC_IMPLEMENT_DYNAMIC(OpDescMsg, MessageHandler) 
CC_IMPLEMENT_DYNAMIC(OpDescControlMsg, OpDescMsg)
CC_IMPLEMENT_DYNAMIC(OpDescControlDestroyMsg, OpDescMsg) 
CC_IMPLEMENT_DYNAMIC(OpDescControlCreateMsg, OpDescMsg) 

CC_IMPLEMENT_MEMDUMP(OpListItem, ListItem)
CC_IMPLEMENT_MEMDUMP(ListItemOpPtr, ListItem)
CC_IMPLEMENT_MEMDUMP(GadgetListItem, ListItem)
// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW


void FixFPControlRegister();

/////////////////////////////////////////////////////////////////////////////////
// Class OpState 

/********************************************************************************************

>	OpState::OpState( BOOL tick, BOOL grey )

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/7/93
	Inputs:		tick - TRUE if the item is ticked
				grey - TRUE if the item is greyed
	Purpose:	This is meant to supply default values to the OpState flags so that they
				don't have random values when they are created. By default they are
				both set to FALSE so if you want something different you will have to 
				change them.

********************************************************************************************/

OpState::OpState( BOOL tick, BOOL grey, BOOL Remove )
{
	Ticked = tick;
	Greyed = grey;
	RemoveFromMenu = Remove;
}



/////////////////////////////////////////////////////////////////////////////////
// Class OpDescriptor


// Here is the static list that belongs to OpDescriptor
List OpDescriptor::OpList;
//OpFlgs	OpDescriptor::Flags; 	
 
/********************************************************************************************

>	OpFlgs OpDescriptor::GetOpFlags()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/3/94
	Inputs:		-
	Outputs:	-
	Returns:	The OpFlags
	Purpose:	To obtain the OpFlags describing the type of the operation
	Errors:		-
	SeeAlso:	-

********************************************************************************************/


OpFlgs OpDescriptor::GetOpFlags()
{
	return Flags; 
} 



/********************************************************************************************

>	OpDescriptor::OpDescriptor( 
								UINT32 toolID,
								UINT32 txID,
								CCRuntimeClass *OpClass,
								TCHAR* token,
								pfnGetState gs,
								UINT32 helpId = 0,
								UINT32 bubbleID = 0,
								UINT32 resourceID = 0,
								UINT32 controlID = 0,
								BOOL ReceiveMessages = FALSE, 
								BOOL Smart = FALSE, 
				 				BOOL Clean = TRUE,
				 				UINT32 OneOpenInstID = 0,
				 				UINT32 AutoStateFlags = 0				
 

							  );

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/8/93
	Inputs:		
				toolID - Module resource Identifier
				txID - Text description resource Identifier
				tok - pointer to a static string that describe the operation
				gs - pointer to the GetState function in your operation

				The following flags specify the type of the operation. They have default 
				values for a normal non-undoable operation. 

				ReceiveMessages - should the OpDescriptor receive system messages 
				Smart - Smart Duplicate
				Clean - does the operation change the document

				helpId - Help Identifier that acts as an index into a help file
				bubbleID - string resource for "bubble help" text.
				resourceID - resource identifier of the resource containing the control.
				controlID - control identifier within that resource.

				OneOpenInstID - When non 0 only one live instance of the operation is 
								permitted, and OneOpenInstID is the string resource
								ID describing why.This is useful for 1 open instance 
								dialogs.

				AutoStateFlags - These flags are used to specify when the Operation should 
								 automatically be greyed, ticked etc. they cut down the number 
								 of tests that need to be made in the GetState fn. See the
								 top of OpDesc.h for an uptodate list of these. The flags
								 should be or'd	
								 
								 eg. GREY_WHEN_NO_CURRENT_DOC | GREY_WHEN_NO_SELECTION 


	Purpose:	Construct a new OpDescriptor object and link it into the list
				of all OpDescriptors
				Both the resource and control IDs may be zero (the default case) in which
				case the OpDescriptor is not connected to any form of control.
				If the resource ID alone is supplied it is taken to refer to a bitmap and
				the Opdescriptor will be connected to a bitmap button.
				If both resource and control IDs are supplied then they refer to an
				arbitrary control.

	Errors:		ENSURE fails if any of the params are NULL (ie leaving params off
				is NOT allowed.

********************************************************************************************/

OpDescriptor::OpDescriptor(
						   UINT32 toolID,                      
						   UINT32 txID,
						   CCRuntimeClass *Op,			
						   TCHAR* tok,
						   pfnGetState gs,
						   UINT32 helpID,
						   UINT32 bubbleID,
						   UINT32 resourceID,
						   UINT32 controlID,
						   BOOL ReceiveMessages,  
     					   BOOL Smart,
				 		   BOOL Clean,
						   UINT32 OneOpenInstID,
						   UINT32 AutoStateFlags,
						   BOOL fCheckable /*= FALSE*/
)
  : MessageHandler(CC_RUNTIME_CLASS(OpDescriptor), ReceiveMessages),
    ModuleID(Tool::GetModuleID(toolID)), 
  	TextID(txID),
  	HelpID(helpID),
  	BubbleID(bubbleID),
	BarCtrlInfo(resourceID,controlID,toolID),
  	OpClass(Op),
	OneInstID(OneOpenInstID),
	AutoStateFlgs(AutoStateFlags),
  	GetState(gs),
  	Token(tok)
{
	Flags.Smart = Smart;
	Flags.Clean = Clean;
	Flags.fCheckable = fCheckable;

	Aliased = FALSE;
	m_bHotKeyEnabled = TRUE;

	GetParamState = NULL;
		 
	ENSURE( tok /*&& gs*/, "OpDescriptor: new OpDescriptor called with a NULL parameter" );

	LinkDescriptor(this); 

}


/**************************************************************************************

>	OpDescriptor::~OpDescriptor()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/12/96
	Purpose:	Standard destructor 

**************************************************************************************/

OpDescriptor::~OpDescriptor()
{
	// Could now call the DelinkDescriptor
}

/********************************************************************************************

>	static void OpDescriptor::LinkDescriptor( OpDescriptor* ThisOp )

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/03/94
	Inputs:		pointer to the OpDescriptor
	Outputs:	-
	Returns:	-
	Purpose:	Add the OpDescriptor to the list of all OpDescriptors
	Errors:		-
	SeeAlso:	-
	Scope:		private

	Note:		Moved from header file to here 16/12/96
********************************************************************************************/

void OpDescriptor::LinkDescriptor( OpDescriptor* ThisOp )
{
	OpListItem* OpItem = new OpListItem;	 // A bit naughty I know but its got to be called from
											 // the constructor (during initialisation)
	OpItem->pOpDesc = ThisOp; 
	OpList.AddTail(OpItem); 
}

/********************************************************************************************

>	static void OpDescriptor::DelinkDescriptor( OpDescriptor* ThisOp )

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/12/96
	Inputs:		pointer to the OpDescriptor
	Outputs:	-
	Returns:	-
	Purpose:	Delete the OpDescriptor from the list of all OpDescriptors
	Errors:		-
	SeeAlso:	-
	Scope:		private

********************************************************************************************/

BOOL OpDescriptor::DelinkDescriptor( OpDescriptor* pThisOp )
{
	// Search the list of ops and remove this item from the list
	OpListItem* pOpListItem = OpDescriptor::GetFirstDescriptor();
	while (pOpListItem)
	{
		if (pThisOp == pOpListItem->pOpDesc)
		{
			// remove this item from the list
			OpList.RemoveItem(pOpListItem);
			delete pOpListItem;
			return TRUE;
		}

		pOpListItem = OpDescriptor::GetNextDescriptor(pOpListItem);
	}

	// Didn't find anything or do anything
	return FALSE;
}

/********************************************************************************************

>	void OpDescriptor::Invoke(OpParam* pOpParam, BOOL fWithUndo = TRUE);

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/8/93

	Inputs:	
				pOpParam:	  Optional operation parameters. If pOpParam is NULL then
							  the operations Do function is called, if pOpParam is
							  not NULL then DoWithParam is called.

				fWithUndo	  if FALSE then don't put this Operation in the undo buffer.
							  USE WITH CAUTION (this must be tested on an Op by Op
							  basis).  By default TRUE, ie. make undo info.

	Purpose:	This Function uses the Operation runtime class to create an instance of the
				operation object and call its DO function.

	Errors:		As a safeguard before the operation is invoked its GetState fn is invoked
				to make sure that the Operation is executable. 
				
				If the Operation is not executable then an ENSURE failure will occur in a 
				debug build. 
				
				In a retail build DialogBarOp::UpdateStateOfAllBars is called and the
				operation is not executed.
	
	SeeAlso:	OpDescriptor::OpDescriptor
	SeeAlso:	OpParam
	SeeAlso:	Operation::Do
	SeeAlso:	Operation::DoWithParam

********************************************************************************************/

void OpDescriptor::Invoke(OpParam* pOpParam, BOOL fWithUndo)
{
	// Before we invoke the operation we call the operation's GetState fn to make sure
	// that it is executable. 
	String_256 Dummy; 
	OpState State = GetOpsState(&Dummy); 
	if (!State.Greyed)
	{

		Operation* Op;
		CCRuntimeClass* TheOpClass;
		
		// Find out which operation this OpDesc should invoke, i.e. has it been aliased
		if (Aliased)
		{
			if (AliasOpClass != NULL)
				TheOpClass = AliasOpClass;
			else
				TheOpClass = OpClass;
		}
		else
			TheOpClass = OpClass;

		// Use the TheOpClass pointer to a runtime class in order to generate an instance of
		// the appropriate operation object 
		Op = (Operation*) TheOpClass->CreateObject();
	
		if (Op)
		{
#if !defined(EXCLUDE_FROM_RALPH)
			// Update the state of the bars
			DialogBarOp::SetSystemStateChanged(TRUE); 

			// If the op has not got its own way of showing the progress
			BOOL ShowProgressIndicator = !(Op->GetOpFlgs().HasOwnTimeIndicator);
		    if (ShowProgressIndicator)
			{
				// Bring up the progress indicator (This has been made re-enterant)
		    	BeginSlowJob(); 
			}
#endif
			// On all but OpExit, send around an OpMsg::START.
			if (!IS_A(Op, OpExit)) BROADCAST_TO_ALL(OpMsg(Op, OpMsg::BEGIN));

			// Discard undo information if appropriate.
			if (!fWithUndo) Op->SucceedAndDiscard();

			// Call either parameterised or default Do function.
			if (pOpParam == 0)
				Op->Do(this);
			else
				Op->DoWithParam(this, pOpParam);

#if !defined(EXCLUDE_FROM_RALPH)
			if (ShowProgressIndicator)
			{
				EndSlowJob();
			}
#endif
		}
	}
	else
	{
#if !defined(EXCLUDE_FROM_RALPH)
		// The operation is disabled. This means that DialogBarOp::UpdateStateOfAllBars 
		// was not called after a change in the state of the system.

		// If ShouldUpdateBarState returns TRUE then the operation was probably invoked before
		// the Idle event had a chance to update it.

		// If this ENSURE goes bang it indicates  that SetSystemStateChanged was not called when it
		// should have been.
		ENSURE(DialogBarOp::ShouldUpdateBarState(), "Trying to execute operation which should be disabled"); 

		// This ENSURE does not indicate a real error but I think its worth keeping it in as it indicates
		// that the idle update processing may not be frequent enough.
		//ENSURE(FALSE, "Trying to execute Operation before we have had a chance to disable it");  

		// Update the bar state and leave 
		DialogBarOp::SetSystemStateChanged();
		DialogBarOp::UpdateStateOfAllBars(); 
//		Beep(); // Just to annoy him !
#endif
	}
}


/********************************************************************************************

>	OpDescriptor* OpDescriptor::FindOpDescriptor(TCHAR* Token)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/7/93
	Inputs:		Token - a pointer to a string to find
	Returns:	a pointer to the OpDescriptor that contains Token, or NULL if it
				could not be found
	Purpose:	To search the OpDescriptor list and find the OpDescriptor described
				by Token

********************************************************************************************/

OpDescriptor* OpDescriptor::FindOpDescriptor(TCHAR* Token)
{
	OpListItem* CurrentItem = GetFirstDescriptor();
		
	while (CurrentItem != NULL)
	{
		// if we have found a match, return a pointer to it
		if (_tcscmp( CurrentItem -> pOpDesc -> Token, Token ) == 0)
			return CurrentItem->pOpDesc;
		
		CurrentItem = GetNextDescriptor( CurrentItem );		// next item in the list
	}
	
	return NULL;		// failed to find a match, so return null
}

/********************************************************************************************

>	OpDescriptor* OpDescriptor::FindOpDescriptor(CCRuntimeClass* Op)

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/10/93
	Inputs:		CCRuntimeClass - a pointer to a RuntimeClass 
	Returns:	a pointer to the OpDescriptor that contains Token, or NULL if it
				could not be found
	Purpose:	To search the OpDescriptot list and find the OpDescriptor described
				by Token

********************************************************************************************/

OpDescriptor* OpDescriptor::FindOpDescriptor(CCRuntimeClass* Op)
{
	OpListItem* CurrentItem = GetFirstDescriptor();
		
	while (CurrentItem != NULL)
	{
		// if we have found a match, return a pointer to it
		if ( CurrentItem -> pOpDesc -> OpClass == Op )
			return CurrentItem -> pOpDesc;
		
		CurrentItem = GetNextDescriptor( CurrentItem );		// next item in the list
	}
	
	return NULL;		// failed to find a match, so return null
}

/********************************************************************************************

>	OpDescriptor* OpDescriptor::FindOpDescriptor(ResourceID res)

	Author:		Alex Bligh <alex@alex.org.uk>
	Created:	17/01/2005
	Inputs:		res - ID of resource to find
	Returns:	a pointer to the OpDescriptor that contains Token, or NULL if it
				could not be found
	Purpose:	To search the OpDescriptor list and find an entry with the given resource
				ID. Note it is compared against the 'control' entry

********************************************************************************************/

OpDescriptor* OpDescriptor::FindOpDescriptor(ResourceID res)
{
	OpListItem* CurrentItem = GetFirstDescriptor();
		
	while (CurrentItem != NULL)
	{
		// if we have found a match, return a pointer to it
		if ( CurrentItem -> pOpDesc -> BarCtrlInfo.ControlID == res )
			return CurrentItem -> pOpDesc;
		
		CurrentItem = GetNextDescriptor( CurrentItem );		// next item in the list
	}
	
	return NULL;		// failed to find a match, so return null
}

/********************************************************************************************

>	void OpDescriptor::AliasOperation(	CCRuntimeClass* AliasOp,
										pfnGetState AliasGetState,
										UINT32 AliasAutoStateFlags = 0,
										UINT32 AliasTextID = 0,
										UINT32 AliasBubbleID = 0);

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/7/94
	Inputs:		AliasOp    	  		- The runtime class of the operation you wish this OpDesc to invoke
						  			  as an alternative to the one that was registered with the OpDesc
									  If NULL, then the registered op will be invoked
				AliasGetState 		- Alternative GetState function used to gather info on the op
									  such as whether menu items should be greyed, etc
									  If NULL, then the registered GetState fn will be used
				AliasAutoStateFlags	- Auto state flags to use instead of the ones provided when op was registered.
									  Only used if param AliasGetState is NOT NULL.
				AliasTextID			- Alternative text ID (0 if you don't want this changed)
				AliasBubbleID		- Alternative bubble help ID (0 if you don't want this changed)
	Returns:	-
	Purpose:	Allows you to alias an op descriptor, i.e. provide an alternative operation
				and GetState function for a registered op descriptor.
				This mechanism allows a standard operation, or user action, to have a different
				internal action or implementation.

				When aliasing an OpDesc, you can provide just one of the two params.

				AliasOperation(NULL,MyGetState,MyAutoStateFlags) means you are quite happy with the standard op
				but you want to replace the way the state of the OpDesc is determined.
				The registered auto state flags are replaced with MyAutoStateFlags

				AliasOperation(MyOp,NULL,MyAutoStateFlags) means you want to change the internal implementation
				but the state function is OK and does the job.
				The registered auto state flags is retained. I.e. MyAutoStateFlags param IS IGNORED!!!!!

				AliasOperation(NULL,NULL) causes an error in debug builds - i.e. what is the point?

				IMPORTANT:
				When aliasing an OpDesc it is very important that the high level functionality
				of the op remains the same.  E.g. Aliasing the "Cut" op should not suddenly create
				a flower in the current layer.  It should "Cut" something in a way that is consistant
				with the user's perception of the op.

				E.g. "Delete" has a menu item and button. When in the selector tool, he'd
				expect Delete to delete the selected objects. When in the grid tool, he'd 
				expect Delete to delete the selected grids.
				However, the mechanisms of the two tools are very different. So the grid tool
				needs to alias the Delete op descriptor, so an alternative operation is invoked
				when the "Delete" menu item or button (or keypress!) is activated by the user.

				The grid tool may do something like this when it becomes active:

					// Find the pre-registered OpDesc that we're interested in
					OpDescriptor* pOpDesc = FindOpDescriptor(OPTOKEN_DELETE);

					// Alias it with an alternative op and GetState
					pOpDesc->AliasOperation(CC_RUNTIME_CLASS(GridDeleteOp),GridDeleteOpGetState,0);

				NOTE:
				At the time of writing this mechanism, it was asumed only tools would be allowed
				to alias OpDescs. To aid this, a call to RemoveAllAliases() has been placed in the
				tool mechanism at the point where the tool is deselected.

	Errors:		In debug builds it will ENSURE if the OpDesc is already aliased, or if both
				params are NULL.

********************************************************************************************/

void OpDescriptor::AliasOperation(CCRuntimeClass* AliasOp,pfnGetState AliasGetState,UINT32 AliasAutoStateFlags,UINT32 AliasTextID,UINT32 AliasBubbleID)
{
	ENSURE(Aliased == FALSE,"This OpDesc is already aliased");
	if (Aliased) return;

	ENSURE((AliasOp != NULL) || (AliasGetState != NULL),"What is the point of aliasing an OpDesc when both params are NULL?");
	if ((AliasOp == NULL) && (AliasGetState == NULL)) return;

	// Note the alias op class
	AliasOpClass 		= AliasOp;

	// Save the old state
	OldGetState  		= GetState;
	OldAutoStateFlags 	= AutoStateFlgs;
	OldTextID			= TextID;
	OldBubbleID			= BubbleID;

	// Have we an alternative GetState func?
	if (AliasGetState != NULL)
	{
		// Only replace the GetState and AutoStateFlags if an alternative has been provided
		GetState 		= AliasGetState;
		AutoStateFlgs	= AliasAutoStateFlags;
	}

	// Only replace the text & bubble help IDs if they have been provided
	if (AliasTextID > 0)
		TextID = AliasTextID;

	if (AliasBubbleID > 0)
		BubbleID = AliasBubbleID;

	Aliased = TRUE;
}


/********************************************************************************************

>	void OpDescriptor::RemoveAlias()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/7/94
	Inputs:		-
	Returns:	-
	Purpose:	Removes the alias for this operation.
	Errors:		In debug builds, it will ENSURE if the OpDesc is not aliased.

********************************************************************************************/

void OpDescriptor::RemoveAlias()
{
	ENSURE(Aliased,"This OpDesc is NOT aliased");
	if (!Aliased) return;

	GetState 		= OldGetState;
	AutoStateFlgs	= OldAutoStateFlags;
	TextID			= OldTextID;
	BubbleID		= OldBubbleID;
	
	Aliased = FALSE;
}

/********************************************************************************************

>	BOOL OpDescriptor::IsAliased()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/7/94
	Inputs:		-
	Returns:	TRUE if it is aliased, FALSE otherwise.
	Purpose:	Tells you whether the OpDesc is aliased or not

********************************************************************************************/

BOOL OpDescriptor::IsAliased()
{
	return (Aliased);
}

/********************************************************************************************

>	static void OpDescriptor::RemoveAllAliases()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/7/94
	Inputs:		-
	Returns:	-
	Purpose:	Goes through the list of op descriptors, removing all aliases

********************************************************************************************/

void OpDescriptor::RemoveAllAliases()
{
	ListItem* Item = OpList.GetHead();

	while (Item != NULL)
	{
		OpDescriptor* pOpDesc =((OpListItem*)Item)->pOpDesc;

		if (pOpDesc->IsAliased())	pOpDesc->RemoveAlias();

		Item = OpList.GetNext(Item);
	}
}

/********************************************************************************************

>	BOOL OpDescriptor::IsHotKeyEnabled() const

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/09/96
	Purpose:	Determines whether or not the hot key associated with this op should be shown
				to the user via HotKey::FindHotKey()
	Notes:		Presently used to disable Ctrl-S when save op is used for update.
	See Also:	DisableHotKey(), EnableHotKey()

********************************************************************************************/
BOOL OpDescriptor::IsHotKeyEnabled() const
{
	return m_bHotKeyEnabled;
}


/********************************************************************************************

>	void OpDescriptor::DisableHotKey()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/09/96
	Purpose:	Disables the hot key associated with this op
	See Also:	IsHotKeyEnabled(), EnableHotKey()

********************************************************************************************/
void OpDescriptor::DisableHotKey()
{
	m_bHotKeyEnabled = FALSE;
}


/********************************************************************************************

>	void OpDescriptor::EnableHotKey()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/09/96
	Purpose:	Enables the hot key associated with this op
	See Also:	DisableHotKey(), IsHotKeyEnabled()

********************************************************************************************/
void OpDescriptor::EnableHotKey()
{
	m_bHotKeyEnabled = TRUE;
}


/********************************************************************************************

>	static void OpDescriptor::DestroyAll()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/7/93
	Purpose:	Since the OpDescriptor maintains its own static list of all the
				OpDescriptors created in the program, and since none of them are actually
				destroyed with a call to a destructor, we have to do it ourselves by
				a call to this function in the DeInit kernel type of function. Basically
				it walks through the list of OpDescriptors and destroys them all.

********************************************************************************************/

void OpDescriptor::DestroyAll()
{
	ListItem* Item;
	
	while( (Item = OpList.RemoveHead()) != NULL )
	{
		delete ((OpListItem*)Item)->pOpDesc;
		delete Item; 
	}
} 


/********************************************************************************************

>	virtual void OpDescriptor::GetText(String_256* Description, OpTextFlags WhichText)

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/93
	Inputs:		TextFlag - Identifies which text string to retrieve from the string resource
	Outputs:	Description - Operation description string if it is found or
				NULL otherwise.
	Returns:	TRUE if successfully retrieves the string and FALSE othersise.
	Purpose:	This function will use the TextID and ModuleID values to obtain a String
				resource describing an operation. String resources may have one or more 
				text descriptions in them, therefore, a TextFlag can be used to identify the
				the appropriate text required.
	SeeAlso:	GetDescription

	Note:		Made virtual by Neville 11/12/96

********************************************************************************************/

BOOL OpDescriptor::GetText(String_256* Description, OpTextFlags WhichText)
{
	String_256 ResourceText( TextID, ModuleID );
	TCHAR* ok;

	// Explicitly cast return value from GetDescription from a TCHAR* to a String_256
	ok = GetDescription((TCHAR*) ResourceText, WhichText);
	
	// if description is found then return true else return false
	if (ok)        
	{
		*Description = String_256(ok);
		return TRUE;
	}
	else
		return FALSE;
} 

/********************************************************************************************

>	UINT32 OpDescriptor::GetHelpId()

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/1/94
	Purpose:	returns the Help Id

********************************************************************************************/
UINT32 OpDescriptor::GetHelpId()
{ 
	return HelpID; 
}

/********************************************************************************************

>	UINT32 OpDescriptor::GetToolID()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/04/94
	Purpose:	Returns the Tool ID

********************************************************************************************/
UINT32 OpDescriptor::GetToolID()
{ 
	return BarCtrlInfo.ToolID;
}

/********************************************************************************************

>	UINT32 OpDescriptor::GetBubbleId()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com> F
	Created:	16/2/94
	Purpose:	returns the string resource ID of the "bubble-help" text.

********************************************************************************************/
UINT32 OpDescriptor::GetBubbleId()
{ 
	return BubbleID; 
}

/********************************************************************************************

>	const BarControlInfo* OpDescriptor::GetBarControlInfo()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com> F (modified by Phil 13/04/94)
	Created:	16/2/94
	Purpose:	Returns the bar control info of the OpDescriptor. This is a platform-indy
	representation of the control which can be placed on toolbars to invoke this Operation.

********************************************************************************************/

const BarControlInfo* OpDescriptor::GetBarControlInfo()
{ 
	return (&BarCtrlInfo);
}

/********************************************************************************************

>	MsgResult OpDescriptor::Message(Msg* Msg)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/4/94
	Inputs:		Msg:	The message to handle
	Outputs:	-
	Returns:	-
	Purpose:	The default OpDescriptor message handler 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

MsgResult OpDescriptor::Message(Msg* Msg)
{
	// Is this a message for the OpDescriptor ??
	if (MESSAGE_IS_A(Msg,OpDescMsg))
	{
		if ( ((OpDescMsg*)Msg)->OpDesc == this)
		{

			if (MESSAGE_IS_A(Msg,OpDescControlCreateMsg))
			{
				// The control associated with the OpDescriptor is being created
				// call the virtual OnControlCreate	method to allow the control 
				// to be initialised.
				OnControlCreate((OpDescControlCreateMsg*)Msg);
			}

			if (MESSAGE_IS_A(Msg,OpDescControlMsg))
			{
				OpDescControlMsg* CtlMsg =  (OpDescControlMsg*)Msg;
				switch((CtlMsg)->DlgMsg)
				{
					case  DIM_LFT_BN_CLICKED:
					{
						// Invoke the Operation
						Invoke(); 
						break;
					}
					
					case DIM_SELECTION_CHANGED:
					//case DIM_SELECTION_CHANGED_COMMIT:  Removed by Simon cos Jason changed the 
					// 									  semantics of DIM_SELECTION_CHANGED_COMMIT for combos
					{
						// Combo and slider messages handled here
						OnSelectionChange(CtlMsg, NULL);
						break;
					}

					case DIM_COMMIT:
					{
						// profile dialog message boxes handled here
						OnSelectionChange(CtlMsg, NULL);
					}
					break;

					case DIM_SLIDER_POS_CHANGING:
					{
						OnSliderChanging(CtlMsg);
					 	break;
					}

					case DIM_SLIDER_POS_SET:
					{
						OnSliderSet(CtlMsg);
					 	break;
					}

					case DIM_SLIDER_POS_CANCELLED:
					{
						OnSliderCancelled(CtlMsg);
					 	break;
					}

					default:
						break;
				}
			}
			return EAT_MSG;
		}
	}
	return (MessageHandler::Message(Msg)); 
}



/********************************************************************************************
>	BOOL OpDescriptor::BuildGadgetList(List* pOutputList)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/5/94
	Inputs:		A pointer to a list which will hold the gadgets.
	Outputs:	A list of gadgets.
	Returns:	TRUE if there are some gadgets associated with this OpDescriptor,
				FALSE if there aren't any.
	Purpose:	Builds a list of GadgetListItems, each item holding a DialogBarOp* and a
				CGadgetID for a control associated with this OpDescriptor.  This allows
				the caller to manipulate the controls even if there isn't a message from
				them needing processing.  NB. the caller is responsible for allocating
				and deallocating this list!
	Errors:		-
	SeeAlso:	class GadgetListItem
********************************************************************************************/

BOOL OpDescriptor::BuildGadgetList(List* pOutputList)
{
#if !defined(EXCLUDE_FROM_RALPH)
	return ControlList::Get()->BuildGadgetList(pOutputList, this);
#else
	return FALSE;
#endif


// Old code
#if 0
	ENSURE(pOutputList != NULL, "Null output list in OpDescriptor::BuildGadgetList");

	PORTNOTETRACE("other","OpDescriptor::BuildGadgetList - do nothing");
	// Get a pointer to the list of DialogBarOps, if there is one.
	List* pDialogBarOpList = MessageHandler::GetClassList(CC_RUNTIME_CLASS(DialogBarOp));
	if (pDialogBarOpList == NULL) return FALSE;
	
	// Get the first item on the DialogBarOp list.
	DialogBarOp* pDialogBarOp = (DialogBarOp*) pDialogBarOpList->GetHead();
	while (pDialogBarOp != NULL)
	{
		if (pDialogBarOp->WindowID != 0)
		{
			BarItem* pBarItem = pDialogBarOp->GetPtrBarHead();
			while (pBarItem != NULL)
			{
				if (pBarItem->IsKindOf(CC_RUNTIME_CLASS(BarControl)))
				{
					BarControl* pBarControl = (BarControl*) pBarItem;
					BOOL bIsHorizontal = pDialogBarOp->IsHorizontal(); 
					if (pBarControl->GetOpDescriptor(bIsHorizontal)	== this)
					{
						// Found a control associated with this OpDescriptor, so add it
						// to our list of gadgets.
						CGadgetID gid = pBarControl->GetUniqueGadgetID();
						GadgetListItem* pgListItem = new GadgetListItem(pDialogBarOp, gid);
						if (pgListItem == NULL)
						{
							pOutputList->DeleteAll();
							return FALSE;
						}

						pOutputList->AddHead(pgListItem);
					}
				}
				pBarItem = pDialogBarOp->GetPtrBarNext(pBarItem);
			}
		}
		pDialogBarOp = (DialogBarOp*) pDialogBarOpList->GetNext(pDialogBarOp);
	}

	// Return TRUE/FALSE depending on if there are any entries on the list.
	return !pOutputList->IsEmpty();
#endif
}

/********************************************************************************************

>	OpState OpDescriptor::GetOpsState(String_256* Desc, OpParam* pOpParam = NULL)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/8/94
	Inputs:		-
	Outputs:	-
	Returns:	Desc: A description of why the op is greyed. 

	Purpose:	This function will get called to determine the greyed/ticked state of the 
				operation. It tests various conditions (based on the AutoStateFlgs) and 
				then if the op survives this, calls the GetState fn which was registered
			    with the operation.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

OpState OpDescriptor::GetOpsState(String_256* Desc, OpParam* pOpParam)
{  
	OpState OpSt;
	String_256 DisableReason;

	// Test the AutoStateFlgs to determine if any automatic checks are required for this
	// OpDescriptor.

	if (AutoStateFlgs & GREY_WHEN_NO_CURRENT_DOC)
	{
		// Check if there is a selected (user) document
		if (Document::GetSelected() == NULL)
		{
			// There is no current document
			OpSt.Greyed = TRUE;

			// Load reason why operation is disabled
			DisableReason = String_256(_R(IDS_NO_DOC));
		   	*Desc = DisableReason;	     
		   	goto End;                                 
		}

	}

	if (AutoStateFlgs & GREY_WHEN_NO_SELECTION)
	{
		// Find the first node which is selected 
		Node* FirstSelectedNode = GetApplication()->FindSelection()->FindFirst(); 

		if (FirstSelectedNode == NULL)
		{
			// No nodes are selected
			OpSt.Greyed = TRUE; 
			// Load reason why operation is disabled
			DisableReason = String_256(_R(IDS_NO_OBJECTS_SELECTED));
		   	*Desc = DisableReason;	
		   	goto End;                                      
		}
	}

// Changed 10/1/95 by MarkN & Phil
//
// The new 'if' statement is a temp fix until we are in a position to put correct logic in and
// change all the ops to cope with the change.
//
// This fix allows ops that have "grey when no current doc" and/or "grey when no selection" to
// still work when select-inside is present.
//
//	if (AutoStateFlgs & ~DONT_GREY_WHEN_SELECT_INSIDE)
	if (!(AutoStateFlgs & DONT_GREY_WHEN_SELECT_INSIDE)
		&& ((AutoStateFlgs & GREY_WHEN_NO_CURRENT_DOC)
			|| (AutoStateFlgs & GREY_WHEN_NO_SELECTION)
			)
		)
	{
		// Find the selection range
		Range Sel(*(GetApplication()->FindSelection()));
		RangeControl rg = Sel.GetRangeControlFlags();
		rg.PromoteToParent = TRUE;
		Sel.Range::SetRangeControl(rg);
	
		if ( Sel.ContainsSelectInside() )
		{
			// The selection contains selection inside
			OpSt.Greyed = TRUE; 
			// Load reason why operation is disabled
			DisableReason = String_256(_R(IDS_GREY_WHEN_SELECT_INSIDE));
		   	*Desc = DisableReason;	
		   	goto End;                                      
		}
	}

	if (OneInstID != 0)
	{
		// Only one live instance of the operation is allowed. It's probably a dialog 
		if (MessageHandler::MessageHandlerExists(OpClass))
		{
			OpSt.Greyed = TRUE; 
			*Desc = String_256(OneInstID); 
			goto End;
		} 
	}

	// The operation survived all auto tests, so call the GetState fn
	// First, try to newer, parameterised way fo getting state info
	if (GetParamState != NULL && pOpParam!=NULL)
	{
		return (GetParamState(Desc, this, pOpParam));
	}

	// If that didn't do anything fall back to the older way of doing things
	if (GetState != NULL)  
	{
		return (GetState(Desc, this));
	}

	End: // We jump here when we know the correct state, if there are ever any
		 // auto tick test's things will be a bit different cos an op could be
		 // ticked and greyed (well its possible, but not very likely)

	return OpSt; 
}

	
// -----------------------------------------------------------------------------------------
// On Message handlers called from the default Message handler for the OpDescriptor 

/********************************************************************************************
>	virtual void OpDescriptor::OnControlCreate(OpDescControlCreateMsg* CreateMsg);

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/06/94
	Inputs:		CreateMsg:
	Returns:	
	Purpose:	The OnControlCreate method is called by the OpDescriptor's default Message 
				handler when a new control has just been created. You should override
				this method to perform gadget initialisation.

				This base class fn does nothing
	Errors:		-
	SeeAlso:	
********************************************************************************************/

void OpDescriptor::OnControlCreate(OpDescControlCreateMsg* CreateMsg)
{
	// Override this fn is a derived class to initialise a control which has just been created
	// - Most useful for combo/listbox/edit controls.
}


/********************************************************************************************
>	virtual void OpDescriptor::OnSelectionChange(OpDescControlMsg* SelChangedMsg, List* GadgetList)


	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/06/94
	Inputs:		SelChangedMessage: The selection changed message
				GadgetList:		   A list of all gadgets associated with this 
								   OpDescriptor
	Returns:	
	Purpose:	The OnSelectionChange method is called whenever the user selects a new
				value from the gadget .. (blah blah  

				** Should Enter/Tab and selection handling be bundled ?????? - possibly 

				Because there can be more than one control associated with an OpDescriptor
				When the user makes a selection  all gadgets need to be updated. 
				The GadgetList parameter allows this ........

				(Incomplete fn header)

	Errors:		-
	SeeAlso:	
********************************************************************************************/

void OpDescriptor::OnSelectionChange(OpDescControlMsg* SelChangedMsg, List* GadgetList)
{
	// 
}


/********************************************************************************************

>	void OpDescriptor::OnSliderSet(OpDescControlMsg* SelChangedMsg)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/11/94
	Inputs:		SelChangedMsg - The message details
	Purpose:	Called when the sliders position changes (if its associated with this
				opdescriptor)

********************************************************************************************/

void OpDescriptor::OnSliderSet(OpDescControlMsg* SelChangedMsg)
{
	// Base class does nothing
}


/********************************************************************************************

>	void OpDescriptor::OnSliderChanging(OpDescControlMsg* SliderChangingMsg)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/3/2000
	Inputs:		SliderChangingMsg - The message details
	Purpose:	Called when the sliders position is being changed (if its associated with this
				opdescriptor)

********************************************************************************************/

void OpDescriptor::OnSliderChanging(OpDescControlMsg* SliderChangingMsg)
{
	// Base class does nothing
}


/********************************************************************************************

>	void OpDescriptor::OnSliderCancelled(OpDescControlMsg* SliderChangingMsg)

	Author:		Marc_Power (Xara Group Ltd) <camelotdev@xara.com>
	Created:	04/11/05
	Inputs:		SliderChangingMsg - The message details
	Purpose:	Called when the sliders position is being cancelled (if its associated with this
				opdescriptor)

********************************************************************************************/

void OpDescriptor::OnSliderCancelled(OpDescControlMsg* SliderChangingMsg)
{
	// Base class does nothing
}


// Forward Reference

// Placed as class  members, to allow access to derived classes. - Ranbir.

//TCHAR* ReadString(TCHAR* pDesc);
//TCHAR* GetMenuNameString(TCHAR* pDesc);
//void SetStringPos(TCHAR** pDesc, OpTextFlags WhichText);

/********************************************************************************************

>	TCHAR* OpDescriptor::GetDescription(TCHAR* pDesc, OpTextFlags WhichText)

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/93
	Inputs:		TextFlag - Identifies which text string to retrieve from the string resource
				Description - Operation description string.
	Returns:	Pointer to string description or NULL otherwize
	Purpose:	Scans through a string and retrieves the text associated with the text flag
				passed in as a parameter.

	Note:		Put as a class function by Neville 11/12/96

********************************************************************************************/

TCHAR* OpDescriptor::GetDescription(TCHAR* pDesc, OpTextFlags WhichText)
{
	/**** Has this man never heard of the switch statement ??? ****/
	                                                               
	// if Menu Text is required then return text string.
	if (WhichText == OP_MENU_TEXT)
		return ReadString(pDesc);
	else 

	// if Help Description is required then return text string
	if (WhichText == OP_DESC_TEXT)      
	{
		SetStringPos(&pDesc, WhichText);
		return ReadString(pDesc);
	}
	else
        
	// if Undo Text is required then return text string.
	if (WhichText == OP_UNDO_TEXT)
	{
		SetStringPos(&pDesc, WhichText);
		return ReadString(pDesc);
	}
			
	// if Menu Name is required then return text string.
	if (WhichText == OP_MENU_NAME)
    {
		return GetMenuNameString(pDesc);
    }                        
    
	// Add next string type here!!!
	else
		return NULL;
}                                                    

/********************************************************************************************

>	void OpDescriptor::SetStringPos(TCHAR** pDesc, OpTextFlags WhichText);

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/93
	Inputs:		WhichText - used to determine the position of the text to be obtained
				Description - Operation description string.
	Purpose:	
		Scans through a string and sets the pDesc pointer to the appropriate text in the 
		string resource. The text position is determined by the ordinal value of the 
		WhichText value

********************************************************************************************/

void OpDescriptor::SetStringPos(TCHAR** pDesc, OpTextFlags WhichText)
{
    UINT32 StringPos = 0;
    
	while (	(StringPos < (UINT32) WhichText) &&
			(**pDesc != END_OF_STRING)) 

	{
		// Read until next string delimeter is reached
		while (	(**pDesc != END_OF_STRING) && 
				(**pDesc != STRING_DELIMETER))
		{
			(*pDesc)++;
		}
   		
		if	(**pDesc != END_OF_STRING) 
	  		// Move pointer to beginning of next string
			(*pDesc)++;   

		//Increment String position by 1
		StringPos++;
	}	
}                   

/********************************************************************************************

>	TCHAR * OpDescriptor::ReadString(TCHAR* pDesc)

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/93
	Inputs:		Description - Operation description string.
	Purpose:	Reads the string until delimeter or end of string.

********************************************************************************************/

TCHAR * OpDescriptor::ReadString(TCHAR* pDesc)
{
	static TCHAR 	pText[MAX_TEXT_SIZE];	
	UINT32 	pos = 0;

	// Get Text String
	while (	(*pDesc != END_OF_STRING) && 
			(*pDesc != STRING_DELIMETER) && 
			(pos <= (MAX_TEXT_SIZE - 1)))
	{
		pText[pos++] = *pDesc;
		pDesc++;
	}
	
	// Set end of string
	pText[pos] = END_OF_STRING;
	
	return pText;
}

/********************************************************************************************

>	TCHAR * OpDescriptor::GetMenuNameString(TCHAR* pDesc)

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/10/93
	Inputs:		Description - Operation description string.
	Purpose:	
		Reads the menu text in position one and removes any ampersands denoting an
		accelerator.
********************************************************************************************/

TCHAR * OpDescriptor::GetMenuNameString(TCHAR* pDesc)
{
	const 	TCHAR ACCELERATOR_LETTER = '&';
	
	static TCHAR 	pText[MAX_TEXT_SIZE];
	UINT32 	pos = 0;

	// Get Text String
	while (	(*pDesc != END_OF_STRING) && 
			(*pDesc != STRING_DELIMETER) && 
			(pos <= (MAX_TEXT_SIZE - 1)))
	{
		if (*pDesc != ACCELERATOR_LETTER)
			pText[pos++] = *pDesc;
		pDesc++;
	}
	
	// Set end of string
	pText[pos] = END_OF_STRING;
	
	return pText;
}



/*******************************************************************************************
							UNDOABLE OPDESCRIPTOR CLASS

*******************************************************************************************/


/********************************************************************************************

>	UndoableOpDescriptor::UndoableOpDescriptor( 
												UINT32 toolID,                      
												UINT32 txID,
												CCRuntimeClass *OpClass,
												TCHAR* token,
												pfnGetState gs,
												UINT32 helpId = 0,
												UINT32 bubbleID = 0,
												UINT32 resourceID = 0,
												UINT32 controlID = 0,
												BOOL ReceiveMessages = FALSE, 
												BOOL Smart = FALSE, 
								 				BOOL Clean = TRUE,
								 				UINT32 OneOpenInstID = 0,		
				 								UINT32 AutoStateFlags = 0,
												BOOL fCheckable = FALSE
							  					);

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/8/93
	Inputs:	
				toolID - Tool Identifier defining which tool (0 for kernel) holds resource
				txID - Text description resource Identifier
				tok - pointer to a static string that describe the operation
				gs - pointer to the GetState function in your operation

				The following flags specify the type of the operation. They have default 
				values for a normal undoable operation. 

				ReceiveMessages - Should the Opdescriptor receive messages 
				Smart - Smart Duplicate
				Clean - does the operation change the document

				helpId - Help Identifier that acts as an index into a help file
				bubbleID - string resource for "bubble help" text.
				resourceID - resource identifier in which to find control to be associated
							 with this OpDescriptor.
				controlID - control in resource.

	Purpose:	Construct a new UndoableOpDescriptor object and link it into the list
				of all OpDescriptors
	Errors:		ENSURE fails if any of the params are NULL (ie leaving params off
				is NOT allowed.

********************************************************************************************/

UndoableOpDescriptor::UndoableOpDescriptor(
						   UINT32 toolID,                      
						   UINT32 txID,
						   CCRuntimeClass *Op,			
						   TCHAR* tok,
						   pfnGetState gs,
						   UINT32 helpID,
						   UINT32 bubbleID,
						   UINT32 resourceID,
						   UINT32 controlID,
						   BOOL ReceiveMessages, 
     					   BOOL Smart,
				 		   BOOL Clean,
				 		   UINT32 OneOpenInstID ,		
				 		   UINT32 AutoStateFlags,
						   BOOL fCheckable /*= FALSE*/
)
 
  :OpDescriptor( toolID,  txID, Op, tok, gs, helpID, bubbleID, resourceID, controlID, ReceiveMessages, 
  				Smart, Clean, OneOpenInstID, AutoStateFlags, fCheckable) 
{
	
}

/*******************************************************************************************
							TOOL OPDESCRIPTOR CLASS

*******************************************************************************************/

/********************************************************************************************

>	ToolOpDescriptor::ToolOpDescriptor(	
										UINT32 toolID,                      
										UINT32 txID,
										CCRuntimeClass *OpClass,			
										TCHAR* token,					
										pfnGetState gs,
										UINT32 hlpID,
										UINT32 bubID,
										UINT32 resID,
										UINT32 ctlID,
										BOOL ReceiveMessages = FALSE, 
										BOOL Smart = FALSE, 
						 				BOOL Clean = TRUE,
										BOOL fCheckable = FALSE
									);

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/8/93
	Inputs:		
				toolID - Identifier of tool in which resources can be found
				txID - Text description resource Identifier
				[ token - Operation Token string (???????????????????? eh Mario?????) ]
				tok - pointer to a static string that describe the operation ("token")
				gs - pointer to the GetState function in your operation
				hlpID - helpfile "jump" ID for this tool (?)
				bubID - bubble help string resource ID
				resID - resource ID associated with this ToolOp
				ctlID - control ID associated with this ToolOp
				
	Purpose:	Construct a new ToolOpDescriptor object and link it into the list
				of all OpDescriptors
	Errors:		

********************************************************************************************/

ToolOpDescriptor::ToolOpDescriptor( 
									UINT32 toolID,
									UINT32 txID,
									CCRuntimeClass *Op,			
									TCHAR* tok,					
									pfnGetState gs,
									UINT32 hlpID,
									UINT32 bubID,
									UINT32 resID,
									UINT32 ctlID,
									BOOL ReceiveMessages , 
									BOOL Smart, 
						 			BOOL Clean,
									BOOL fCheckable /*= FALSE*/ 
								   )
 : OpDescriptor(toolID,  txID, Op, tok, gs, hlpID, bubID, resID, ctlID, ReceiveMessages,
  				Smart, Clean, fCheckable )
{
	CurrentTool = FALSE;		
}




///////////////////////////////////////////////////////////////////////////////////////////////
// The ParamOpDescriptor class													    	 //
///////////////////////////////////////////////////////////////////////////////////////////////

/********************************************************************************************

>	ParamOpDescriptor::ParamOpDescriptor(const TCHAR* pcszToken,
										   CCRuntimeClass* pClass,
										   pfnGetParanState gps)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/12/96
	Inputs:		pcszToken		the "OpToken" (OLE verb?) of the associated Operation
				NewMenuText		string resource of menu text
				pClass		the runtime class to use, defaults to PlugInOp.
				gs			the GetState function to use
	Outputs:	-
	Returns:	-
	Purpose:	Constructs the new ParamOpDescriptor
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

ParamOpDescriptor::ParamOpDescriptor(const TCHAR* pcszToken,
										   CCRuntimeClass* pClass,
										   pfnGetParamState gps)
  :	OpDescriptor(	0,							// tool ID
					0,							// String resource ID (use same for all)
					pClass,						//CC_RUNTIME_CLASS(PlugInOp),	// Runtime class
					(TCHAR*) pcszToken,			// OpToken
					NULL,							// GetState function
					0,							// help ID
					0,							// bubble help
					0,							// resource ID
					0,							// control ID
					TRUE,						// Recieve system messages
					FALSE,						// Smart duplicate operation
					TRUE,						// Clean operation
					0,							// String for one copy only error
					(DONT_GREY_WHEN_SELECT_INSIDE | GREY_WHEN_NO_CURRENT_DOC) // Auto state flags
				)
{
	GetParamState = gps;
}




/********************************************************************************************

>	static OpState ParamOpDescriptor::GetState(String_256* pDesc, OpDescriptor* pOpDesc)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/12/96
	Inputs:		pDesc       --- A pointer to a String. GetState fills this with the
								appropriate details for the conditions arising eg. why
								"Previous Zoom" is greyed out.
				pOpDesc		--- A pointer to the OpDescriptor whose state is being
								queried.
	Returns:	An OpState containing the flags that show what is valid.
	Purpose:	Returns the state that this operation should appear in the menus 
				or as a buttom, for example - greyed out, or ticked.

********************************************************************************************/

OpState ParamOpDescriptor::GetState(String_256*, OpDescriptor* pOpDesc)
{
	// At present, this item is always available.
	OpState OpSt;

//	OpSt.Greyed = (XPEHost::IsEditSessionRunning());

	return OpSt;
}




/********************************************************************************************

>	virtual void ParamOpDescriptor::GetText(String_256* Description, OpTextFlags WhichText)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/12/96
	Inputs:		TextFlag - Identifies which text string to retrieve from the string resource
	Outputs:	Description - Operation description string if it is found or
				NULL otherwise.
	Returns:	TRUE if successfully retrieves the string and FALSE othersise.
	Purpose:	This function will use the TextID and ModuleID values to obtain a String
				resource describing an operation. String resources may have one or more 
				text descriptions in them, therefore, a TextFlag can be used to identify the
				the appropriate text required.
	SeeAlso:	OpDescriptor::GetText
	SeeAlso:	GetDescription

	Note:		Made virtual by Neville 11/12/96
				We do this so that we do not have to store a string with every OpDesciptor.
				This would be ok if we had dynamic strings but of course we don't. The overhead
				would be unacceptable. So we use this overriding class instead.

********************************************************************************************/

BOOL ParamOpDescriptor::GetText(String_256* Description, OpTextFlags WhichText)
{
	// Leave menu text alone
	return TRUE;
} 




