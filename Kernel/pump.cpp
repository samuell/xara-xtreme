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
// */ 

#include "camtypes.h"
//#include "pump.h"  - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "ensure.h"	 - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "errors.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "list.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "listitem.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "msg.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "document.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "docview.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "app.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "comattrmsg.h"
#include "optsmsgs.h"
#include "colmsg.h"
#include "brushmsg.h"
#include "ctrllist.h"

DECLARE_SOURCE("$Revision$");
CC_IMPLEMENT_DYNCREATE(MessageHandler, ListItem)

// All message implement macros here
CC_IMPLEMENT_DYNAMIC(Msg, CCObject) 
CC_IMPLEMENT_DYNAMIC(DialogMsg, Msg) 
CC_IMPLEMENT_DYNAMIC(DeathMsg, Msg) 
CC_IMPLEMENT_DYNAMIC(OpMsg, Msg) 
CC_IMPLEMENT_MEMDUMP(MessageHandlerList, ListItem)
CC_IMPLEMENT_DYNAMIC(BrushMsg, Msg);
CC_IMPLEMENT_DYNAMIC(ScreenChangeMsg, Msg);
CC_IMPLEMENT_DYNAMIC(BrushDefMsg, Msg);
CC_IMPLEMENT_DYNAMIC(NewBrushMsg, Msg);
CC_IMPLEMENT_DYNAMIC(StrokeMsg, Msg);

// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW                                                                    

List MessageHandler::MessageHandlerClassList; // A list of MessageHandlerClassItems 
Msg* MessageHandler::pTmpMsg; 
BOOL MessageHandler::PostDeath = FALSE;


/********************************************************************************************

>	MessageHandler::MessageHandler(CCRuntimeClass* Class, BOOL  SendMessages = TRUE)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/03/94

	Inputs:		Class:		  Specifies the MessageHandler list this MessageHandler should
							  be added to. This class must be derived from Class.  
							  (See Purpose for a description of how to use this)  
	
				SendMessages: This flag specifies if the MessageHandler should be added 
							  to a message list or not. For example certain operations may
							  not be interested in any messages. Setting SendMessages to
							  FALSE in this situation will make message dispatching 
							  more efficient. 
	Outputs:	-
	Returns:	-
	Purpose:	If the SendMessage flag is TRUE then this function adds the MessageHandler 
				to a list of MessageHandlers constructed with the same Class parameter.
				The Class specified should have been registered using RegisterClassGroup
				during startup. 

				Note: The reason for specifying a class parameter is that it allows you 
					  to send messages to selected groups of MessageHandlers. For example 
					  if a dialog message is being sent then it is more efficient to 
					  broadcast this message only to Operations derived from DialogOperation. 
					  So all DialogOpS should specify a DialogOp class in their MessageHandler 
					  constructor. 

					  It is important not to create too many groups of MessageHandlers as 
					  this can lead to inefficient message dispatching. Only create a 
					  new group if you see a situation where a message should be sent 
					  only to MessageHandlers in that group. 

	Errors:		In the retail version: If a MessageHandler list for the class specified does 
				not exist then one is created (because the constructor cannot fail). 
				In the debug version an ENSURE failure will occur. 

	SeeAlso:	MessageHandler::Broadcast
	SeeAlso:	MessageHandler::RegisterClassGroup

********************************************************************************************/

MessageHandler::MessageHandler(CCRuntimeClass* Class, BOOL SendMessages)
{
	if (SendMessages)
	{	
		// The MessageHandler is to receive messages so it needs to be added to a list grouping 
		// all MessageHandlers with the same Class. 

		ListItem* AddMessageHandlerList = NULL; // The list the message handler should be added to. 

		// Firstly determine if there is already a MessageHandlerList to place the MessageHandler on
		ListItem* CurrentMessageHandlerList = MessageHandlerClassList.GetHead(); 
		while (CurrentMessageHandlerList != NULL)
		{
			if (((MessageHandlerList*)CurrentMessageHandlerList)->MessageHandlerClass == Class)
			{
				AddMessageHandlerList = CurrentMessageHandlerList; 	
				break; 
			}
			CurrentMessageHandlerList = 
				MessageHandlerClassList.GetNext(CurrentMessageHandlerList); 
		}

		if (AddMessageHandlerList == NULL) 	// There is no suitable MessageHandlerList so we 
											// must create one.  
		{
			// this test can go off when someone forgot to register a class. It can also
			// go off in DLL builds if a default parameter of CC_RUNTIME_CLASS is used
			// as the compiler screws up the function call
			ERROR3_PF( ("Class %s not registered for MessageHandler", Class->GetClassName() ) );

			AddMessageHandlerList = new MessageHandlerList(Class); 
			MessageHandlerClassList.AddHead(AddMessageHandlerList); 
		}

		// Add the MessageHandler to the MessageHandler list 
		((MessageHandlerList*)AddMessageHandlerList)->m_List.AddHead(this); 
	}
}

/********************************************************************************************

>	static BOOL MessageHandler::RegisterClassGroup(CCRuntimeClass* Class)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/3/94
	Inputs:		Class: The class group to create. 
	Outputs:	-
	Returns:	TRUE if successful, FALSE if memory could not be allocated

	Purpose:	This function should be called to register a Class group at startup eg. 
				Application::Init. See the Message Handler constructor for more information. 

	Errors:		ERROR is called if we are out of memory, and FALSE is returned
	SeeAlso:	-

********************************************************************************************/

BOOL MessageHandler::RegisterClassGroup(CCRuntimeClass* Class)
{
	#if _DEBUG
	// Lets make sure that this class has not already been registered
	ListItem* CurrentMessageHandlerList = MessageHandlerClassList.GetHead(); 
	while (CurrentMessageHandlerList != NULL)
	{
		if (((MessageHandlerList*)CurrentMessageHandlerList)->MessageHandlerClass == Class)
		{
			ENSURE(FALSE, 
				"Trying to register a class group which has already been registered");  	
		}
		CurrentMessageHandlerList = MessageHandlerClassList.GetNext(CurrentMessageHandlerList); 
	}
	#endif

	// Create a new Message handler list 
	ListItem* NewMessageHandlerList = new MessageHandlerList(Class); 
	if (NewMessageHandlerList == NULL)
		return FALSE;
	MessageHandlerClassList.AddTail(NewMessageHandlerList); 
 	return TRUE; 
}

/********************************************************************************************

>	List* GetClassList(CCRuntimeClass* Class)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/3/94
	Inputs:		Class:	The class group
	Outputs:	-
	Returns:	The list of MessageHandlers in the Class group, NULL if the class group
				could not be found
	Purpose:	For obtaining a pointer to a list of MessageHandlers in the specified class
				group. 
	Errors:		An ENSURE will occur if the class group could not be found
	SeeAlso:	-

********************************************************************************************/

List* MessageHandler::GetClassList(CCRuntimeClass* Class)
{
	ListItem* CurrentMessageHandlerList = MessageHandlerClassList.GetHead(); 
	while (CurrentMessageHandlerList != NULL)
	{
		if (((MessageHandlerList*)CurrentMessageHandlerList)->MessageHandlerClass == Class)
		{
			return (&(((MessageHandlerList*)CurrentMessageHandlerList)->m_List)); 
		}
		CurrentMessageHandlerList = MessageHandlerClassList.GetNext(CurrentMessageHandlerList); 
	}
	ENSURE(FALSE, "Could not find class list"); 
	return NULL; 
}

/********************************************************************************************

>	static BOOL MessageHandler::MessageHandlerExists(CCRuntimeClass* Class) 

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/08/94
	Inputs:		Class: The message handler's runtime class
				
	Outputs:	-
	Returns:	TRUE if a MessageHandler exists with runtime class 'Class'
	Purpose:	Searches for a message handler with runtime class (Class). 

				eg. to determine if there is a 'live' BlobbyDlg operation
				if (MessageHandler::MessageHandlerExists(CC_RUNTIME_CLASS(DialogOp,BlobbyDlg)))
				{
					// ...
				}
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL MessageHandler::MessageHandlerExists(CCRuntimeClass* Class)
{
	const CCRuntimeClass* BaseClass;

	// Search for the class list
	MessageHandlerList* CurrentMessageHandlerList = 
		(MessageHandlerList*)(MessageHandlerClassList.GetHead()); 
	while (CurrentMessageHandlerList != NULL)
	{
		BaseClass = Class; 
		// An instance of the Class message handler could be on any Class group list
		// which it is derived from 
		while (BaseClass != NULL)
		{
			if (BaseClass == CurrentMessageHandlerList->MessageHandlerClass)
			{
				// Could be on this list !
				// Search for a message handler with class 'Class'
				MessageHandler* MHandler = 	(MessageHandler*)
					(CurrentMessageHandlerList->m_List.GetHead());
				while (MHandler != NULL)
				{
					if (MHandler->GetRuntimeClass() == Class)
					{
						return TRUE; 
					}
					MHandler = (MessageHandler*)
						(CurrentMessageHandlerList->m_List.GetNext(MHandler));

				}
			 	break;
			}
			BaseClass = BaseClass->GetBaseClass1();
		}
		CurrentMessageHandlerList = 
			(MessageHandlerList*)MessageHandlerClassList.GetNext(CurrentMessageHandlerList); 
	}
	return FALSE; 
} 


/********************************************************************************************

>	MessageHandler::~MessageHandler()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/2/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	MessageHandler destructor, removes the MessageHandler from a MessageHandler
				list, if it's on one.  
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

MessageHandler::~MessageHandler()
{

	SendNoMoreMessages(this); // Removes the Message Handler from any Message list 
						  	   // it may have been on. 

}

/********************************************************************************************
>	static MsgResult MessageHandler::Broadcast(Msg* Msg, 
										  CCRuntimeClass* Class = NULL)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/2/94
	Inputs:		Msg			- The message to send
							  this will be deleted after the message has been sent
				
				Class		- Class specifies which MessageHandlers you wish to 
							  send the message to.If Class = NULL (which is the default) then 
							  the message is sent to all MessageHandlers, otherwise the 
							  message is sent only to those MessageHandlers with a runtime 
							  class Class, or a class derived from Class. 

			
	Outputs:	-

	Returns:	A value of OK or EAT_MSG indicates that the message was successfully sent to the 
				MessageHandler objects. i.e. the MessageHandlers Message functions
				all returned either OK, or EAT_MSG. 

				OK means that message got sent to all specified MessageHandlers and all of them
				return OK. This does not imply than anyone acted on the message of course.

				EAT_MSG means that a handler processed the message and didn't want it to be
				passed on. 

				A FAIL value indicates that one or more Message handlers returned a FAIL value 
				from their Message functions. In this situation the broadcast function will call 
				InformError describing the first error which occured.  

	Purpose:	Sends a message to the specified MessageHandlers. Note that you should not use
				this function directly. Instead use one of the BROADCAST macros
	  	

	SeeAlso:	MessageHandler::Message
	SeeAlso:	MessageHandler::RegisterClassGroup
	SeeAlso:	BROADCAST_TO_ALL
	SeeAlso:	BROADCAST_TO_CLASS

********************************************************************************************/


MsgResult MessageHandler::Broadcast(Msg* Message, CCRuntimeClass* Class)
{
	// If the DeathMsg has been sent to all MessageHandlers then we won't allow any more
	// broadcasts...
	if (PostDeath)
	{
		delete Message;
		return (OK);
	} 

	// Indicate we may need to refresh button states on exit
//	ControlList::Get()->Changed();

/*	Document* pOldCurDoc = Document::GetCurrent();
	View* pOldCurView = View::GetCurrent();
*/
	if (Class == NULL)
	{
		// Send the message to all Message Handlers
		Class = CC_RUNTIME_CLASS(MessageHandler); 
	};
	 
	MessageHandlerList* CurrentMessageHandlerList = 
		(MessageHandlerList*)(MessageHandlerClassList.GetHead()); 
	ListItem* CurrentMessageHandler; 
	ListItem* NextMessageHandler; 
	BOOL Result; 

	#ifdef _DEBUG
	BOOL FoundClassGrp = FALSE; 
	#endif
	
	BOOL Failed = FALSE;  // Set to TRUE if a Message fn returns FAIL
	String_256 FirstErrStr;  // Used to save the first error description set
	UINT32 FirstErrMod=0; 
	
	while (CurrentMessageHandlerList != NULL)
	{	
		// Determine if we should send messages to the objects in the 
		// list
		const CCRuntimeClass* GroupClass = CurrentMessageHandlerList-> MessageHandlerClass; 
		while (GroupClass != NULL)
		{
			if (GroupClass == Class)
			{  
				#ifdef _DEBUG
				FoundClassGrp = TRUE; 
				#endif
				// GroupClass is equal to or derived from Class
				// Send the message to every MessageHandler in the list
				CurrentMessageHandler = CurrentMessageHandlerList->m_List.GetHead(); 
				while (CurrentMessageHandler != NULL)
				{   
					// Get the next message handler now cos there is no guarantee that 
					// CurrentMessageHandler will survive. 
					NextMessageHandler = (MessageHandlerList*)
						(CurrentMessageHandlerList->m_List.GetNext(CurrentMessageHandler)); 

					Result = ((MessageHandler*)CurrentMessageHandler)->
							  Message(Message); // Send the message 

					if (Result == EAT_MSG) 
					{
						delete Message;
/*
						// We need to restore the old current doc and view's,
						// but they may have been deleted, so we must first
						// check that they are still in the doc list.
						Document *pDoc = (Document *) Camelot.Documents.GetHead();
						while (pDoc != NULL)
						{
							if (pDoc == pOldCurDoc)
							{
								// Restore the old current values
								if (pOldCurView)
									pOldCurView->SetCurrent();
								else
									View::SetNoCurrent();
								if (pOldCurDoc)
									pOldCurDoc->SetCurrent();
								else
									Document::SetNoCurrent();

								break;
							}

							pDoc = (Document *) Camelot.Documents.GetNext(pDoc);
						}
*/
						return EAT_MSG; 	// The message has been eaten
					}
					if (Result == FAIL)
					{
						// If we have failed previously then we need not do anything special
						if (!Failed)
						{
							// Record the error which was set, we will restore this value prior
							// to returning from the function. 
							FirstErrStr = Error::GetErrorString(); 
							FirstErrMod = Error::GetErrorModule(); 
							Failed = TRUE; // So we don't record the error again	
						}
						// Continue to send the message on to other MessageHandlers
					}
					// Get the next Message Handler
					CurrentMessageHandler = NextMessageHandler;
				}
				break; 

			}
			GroupClass = GroupClass->GetBaseClass1();
		}

		CurrentMessageHandlerList = (MessageHandlerList*)
			(MessageHandlerClassList.GetNext(CurrentMessageHandlerList)); 
	}

	// If we have just sent the DeathMsg then flag that we will allow no more!
	CCRuntimeClass* MsgType = Message->GetRuntimeClass();
	if (MsgType==CC_RUNTIME_CLASS(DeathMsg))
	{
		PostDeath = TRUE;
	}

	delete Message;
	#ifdef _DEBUG 	
	ENSURE(FoundClassGrp, "Message cannot be sent"); 
	#endif

/*	// Restore the old current values
	if (pOldCurView)
		pOldCurView->SetCurrent();
	else
		View::SetNoCurrent();
	if (pOldCurDoc)
		pOldCurDoc->SetCurrent();
	else
		Document::SetNoCurrent();
*/
	if (Failed)
	{
		// Restore the error
		Error::SetError(0, FirstErrStr, FirstErrMod);
		InformError();  
		return FAIL; 
	}
	else
	{
		return OK;
	}
}


/********************************************************************************************
>	virtual MsgResult MessageHandler::Message(Msg* Message)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/2/94
	Inputs:		Message: The message 

	
	Returns:	OK	 		  Message handled ok (return this even if you don't need to 
							  respond to the message). 

				FAIL 		  Something terrible happened whilst processing the message
				 			  eg. we ran out of memory. You must set ERROR if you
				 			  are returning this value. The message will continue to
							  be sent to other MessageHandlers. 

				EAT_MSG       The Message was handled ok but don't send it to any
				 			  more MessageHandlers. 

	Purpose:	The default message handler receives global messages, extracts information
				from them and then calls virtual On<Message> handlers. 
	 
	Errors:		You must set ERROR if you are returning FAIL. 

	SeeAlso:	Operation::Broadcast
********************************************************************************************/


MsgResult MessageHandler::Message(Msg* Message)
{
	// Find out the type of the message
	CCRuntimeClass* MsgType = Message->GetRuntimeClass();

	if (MsgType==CC_RUNTIME_CLASS(DeathMsg))
	{
		return(OnDeathMsg() ? OK : FAIL);	
	}
	else if (MsgType==CC_RUNTIME_CLASS(SelChangingMsg))
	{
		return(OnSelChangingMsg( ((SelChangingMsg*)Message)->State ) ? OK: FAIL);
	}
	else if (MsgType==CC_RUNTIME_CLASS(ColourChangingMsg))
	{
		return(OnColourChangingMsg((ColourChangingMsg*)Message) ? OK: FAIL);
	}
	else if (MsgType==CC_RUNTIME_CLASS(DocViewMsg))
	{
		DocViewMsg* DVMsg = (DocViewMsg*)Message; 
		return(OnDocViewMsg(DVMsg->pNewDocView, DVMsg->State) ? OK: FAIL);
	}
#if !defined(EXCLUDE_FROM_RALPH)
	else if (MsgType==CC_RUNTIME_CLASS(BarMsg))
	{
		return(OnBarMsg((BarMsg*)Message) ? OK: FAIL);	
	}
#endif
	else  if (MsgType==CC_RUNTIME_CLASS(DocChangingMsg))
	{
		DocChangingMsg* DCMsg = (DocChangingMsg*)Message;
		return(OnDocChangingMsg(DCMsg->pChangingDoc, DCMsg->State) ? OK: FAIL); 
	} 
#if !defined(EXCLUDE_FROM_RALPH)
	else if (MsgType==CC_RUNTIME_CLASS(OptionsChangingMsg))
	{
		return(OnOptionsChangingMsg((OptionsChangingMsg*)Message) ? OK: FAIL);
	}
#endif
	else if (MsgType==CC_RUNTIME_CLASS(CommonAttrsChangedMsg))
	{
		return(OnCommonAttrsChangedMsg() ? OK: FAIL);
	}
	else return OK; 
}


/********************************************************************************************

>	static void MessageHandler::SendNoMoreMessages(MessageHandler* MessageHandlerToRemove); 

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/3/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Call this function when you want Messages to stop being sent to the 
				MessageHandler. The MessageHandler is simply removed from its 
				message handler list. 

				It is quite safe to call this function even if the MessageHandler does
				not receive messages. The Operation's End method exploits this. 

	Errors:		-
	Scope:		protected. 
	SeeAlso:	-

********************************************************************************************/

void MessageHandler::SendNoMoreMessages(MessageHandler* MessageHandlerToRemove)
{

	MessageHandlerList* CurrentMessageHandlerList 
		= (MessageHandlerList*)MessageHandlerClassList.GetHead(); 

	while (CurrentMessageHandlerList != NULL)
	{
			// The MessageHandler could be on this list
		if ((CurrentMessageHandlerList->m_List.FindPosition(MessageHandlerToRemove)) >= 0) // yuk
		{
			// The message handler is on the list
			CurrentMessageHandlerList->m_List.RemoveItem(MessageHandlerToRemove); 
			break; 

		}
		CurrentMessageHandlerList = (MessageHandlerList*)
			(MessageHandlerClassList.GetNext(CurrentMessageHandlerList)); 
	}	
}

/********************************************************************************************

>	static void	Destroy()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/3/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	This static function should be Called before we quit Camelot. It deletes all 
				message handler class lists. 

	SeeAlso:	-

********************************************************************************************/


void MessageHandler::Destroy()
{
	ListItem* CurrentMessageHandlerList = MessageHandlerClassList.GetHead(); 

	while (CurrentMessageHandlerList != NULL)
	{
		ListItem* NextItem = MessageHandlerClassList.GetNext(CurrentMessageHandlerList);

		delete( MessageHandlerClassList.RemoveItem(CurrentMessageHandlerList) ); 

		CurrentMessageHandlerList = NextItem;
	}		
}

// ------------------------------------------------------------------------------------------
// OnMessage handlers

// If you are going to add new On<msg> handlers then you should do so here
// these functions should either return a BOOL or a MsgResult value. 
// If they return a BOOL then TRUE will be equivelent to a OK MsgResult value, and FALSE
// should be equivelent to a FAIL MsgResult value. 

/********************************************************************************************

>	inline virtual BOOL OnDeathMsg(void) { return TRUE }; 

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/6/94
	Inputs:		-
	Outputs:	-
	Returns:	A FALSE  value is equivalent to the Message functions FAIL return value, 
				A TRUE  value is equivalent to the Message functions OK return value.
				refer to MessageHandler::Message
				
	Purpose:	Called by the Message fn when a death message is received
				See DeathMsg for a description

	Errors:		-
	SeeAlso:	DeathMsg
	SeeAlso:	MessageHandler::Message

********************************************************************************************/

/********************************************************************************************

>	virtual BOOL OnSelChanging(SelectionState State) {return TRUE}; 

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/6/94
	Inputs:		-
	Outputs:	-
	Returns:	A FALSE  value is equivalent to the Message functions FAIL return value, 
				A TRUE  value is equivalent to the Message functions OK return value.
				refer to MessageHandler::Message
				
	Purpose:	Called by the Message fn when a SelChangingMsg message is received

	Errors:		-
	SeeAlso:	SelChangingMsg


********************************************************************************************/

/********************************************************************************************
	
	virtual BOOL OnColourChangingMsg(ColourChangingMsg* Msg) { return TRUE; };


	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/6/94
	Inputs:		-
	Outputs:	-
	Returns:	A FALSE  value is equivalent to the Message functions FAIL return value, 
				A TRUE  value is equivalent to the Message functions OK return value.
				refer to MessageHandler::Message
				
	Purpose:	Called by the Message fn when a ColourChangingMsg message is received

	Errors:		-
	SeeAlso:	ColourChangingMsg


********************************************************************************************/


/********************************************************************************************

>	virtual BOOL OnDocViewMsg(DocView* pDocView, DocViewState State) {return TRUE};

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/6/94
	Inputs:		-
	Outputs:	-
	Returns:	A FALSE  value is equivalent to the Message functions FAIL return value, 
				A TRUE  value is equivalent to the Message functions OK return value.
				refer to MessageHandler::Message
				
	Purpose:	Called by the Message fn when a DocViewMsg message is received

	Errors:		-
	SeeAlso:	DocViewMsg


********************************************************************************************/

/********************************************************************************************

>	virtual BOOL OnBarMsg(BarMsg* Msg) {return OK};

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/6/94
	Inputs:		-
	Outputs:	-
	Returns:	A FALSE  value is equivalent to the Message functions FAIL return value, 
				A TRUE  value is equivalent to the Message functions OK return value.
				refer to MessageHandler::Message
				
	Purpose:	Called by the Message fn when a BarMsg  is received

	Errors:		-
	SeeAlso:	BarMsg


********************************************************************************************/

/********************************************************************************************

>	virtual BOOL OnDocChangingMsg(Document* pChangingDoc, DocState State) {return OK};

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/6/94
	Inputs:		-
	Outputs:	-
	Returns:	A FALSE  value is equivalent to the Message functions FAIL return value, 
				A TRUE  value is equivalent to the Message functions OK return value.
				refer to MessageHandler::Message
				
	Purpose:	Called by the Message fn when a  DocChangingMsg is received

	Errors:		-
	SeeAlso:	DocChangingMsg


********************************************************************************************/


/********************************************************************************************

>	MessageHandlerList::MessageHandlerList(CCRuntimeClass* Class)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/3/94
	Inputs:		Class:	Class group
	Outputs:	-
	Returns:	-
	Purpose:	MessageHandlerList constructor 
	Errors:		-
	SeeAlso:	MessageHandler

********************************************************************************************/

MessageHandlerList::MessageHandlerList(CCRuntimeClass* Class)
{
	MessageHandlerClass = Class; 
} 

