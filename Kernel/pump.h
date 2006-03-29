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
 *  */ 

#ifndef INC_PUMP
#define INC_PUMP       

#include "ccobject.h"
#include "listitem.h"
#include "list.h"
#include "msg.h"
#include "selmsg.h"
//#include "colmsg.h"
#include "docvmsg.h"
#include "barmsg.h"
#include "docmsgs.h"
//#include "optsmsgs.h"

class BarMsg;
class ColourChangingMsg;
class OptionsChangingMsg;

// MsgResult is returned by the MessageHandler's Message function 

enum MsgResult
{ 
	FAIL = 0,	// Something terrible happened whilst processing the message
	 			// eg. we run out of memory 
	OK, 		// Message handled ok
	EAT_MSG   	// The Message was handled ok but don't send it to any
	 			// more MessageHandlers. 
};



/********************************************************************************************

>	class MessageHandler: public ListItem

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/03/93

	Purpose: 	If a system object must respond to system messages then its class should be
				derived from the abstract MessageHandler class. When a MessageHandler is 
				constructed it gets added to a list of MessageHandlers which are all targets
				of system messages. To send a message to all or just a subset of 
				MessageHandlers, you should use the static Broadcast method. The virtual 
				'Message' function is called whenever the MessageHandler is sent a message.   

	SeeAlso:	

********************************************************************************************/
class MessageHandler: public ListItem	 
{
	CC_DECLARE_DYNCREATE( MessageHandler )
public: 
	// If you wish to create a class which does not get sent system messages then set the 
	// SendMessages flag to FALSE. 
  	MessageHandler(CCRuntimeClass* Class = CC_RUNTIME_CLASS(MessageHandler), BOOL SendMessages = TRUE); 

	// Destructor
	virtual ~MessageHandler(); 

	// Broadcast is used to send a message to all MessageHandlers which are derived from the 
	// given runtime class. 
	static MsgResult Broadcast(Msg* Message,
						 CCRuntimeClass* Class = NULL  // NULL = Send message to all
						 							   // MessageHandlers. 
								 );

	static BOOL RegisterClassGroup(CCRuntimeClass* Class); 

	// GetClassList can be used to get a pointer to the list of MessageHandlers in the same 
	// class group.  
	static List* GetClassList(CCRuntimeClass* Class); 

	static void	Destroy(); // This function must be called to delete the MessageHandler lists
						   // at system termination time. 

	// Used to determine if there is a live MessageHandler with class Class  
	static BOOL MessageHandlerExists(CCRuntimeClass* Class); 
	
	// The default constructor has to be provided for DYNCREATE to work. It should 
	// never get called. 

	// This silly variable is required by the BROADCAST macros 
	static Msg* pTmpMsg; 


	
protected:
	// The virtual Message function. This base class function decodes the message and calls 
	// the appropriate On message fn. (See below).    
	virtual MsgResult Message(Msg* Msg);

	// -------------------------------------------------------------------------------------
	// Add On message handlers here for all messages which are likely to be of interest to
	// all MessageHandler objects. The name format should be On<MessageName>
	// These will be the messages broadcast using BROADCAST_TO_ALL

	// Called by the Message fn when a death message is received
	virtual BOOL OnDeathMsg(void) {return TRUE;}; 
	// Called when a SelChangingMessage is received
	virtual BOOL OnSelChangingMsg(SelChangingMsg::SelectionState State) {return TRUE;}; 
	// Called when a ColourChangingMsg is received
	virtual BOOL OnColourChangingMsg(ColourChangingMsg* Msg) { return TRUE; };
	// Called when a DocViewMsg is received
	virtual BOOL OnDocViewMsg(DocView* pDocView, DocViewMsg::DocViewState State) {return TRUE;};
	// Called whenever a BarMsg is received
	virtual BOOL OnBarMsg(BarMsg* Msg) {return TRUE;};
	// Called whenever a DocChangingMsg is received
	virtual BOOL OnDocChangingMsg(Document* pChangingDoc, DocChangingMsg::DocState State) {return TRUE;};
	// Called whenever an OptionsChangingMsg is received
	virtual BOOL OnOptionsChangingMsg(OptionsChangingMsg* Msg) {return TRUE;};
	// Called whenever a CommonAttrsChangedMsg is received
	virtual BOOL OnCommonAttrsChangedMsg(void) {return TRUE;};
			
	// --------------------------------------------------------------------------------------

	// The SendNoMoreMessages method removes a MessageHandler from the MessageHandler list.
	// UndoableOperations need to call this function whenever they get added to the 
	// OperationHistory. 
	static void SendNoMoreMessages(MessageHandler* MessageHandlerToRemove); 

	// The MessageHandlerClassList is a list of lists of MessageHandlers. The MessageHandlers
	// are grouped according to the class specified in the MessageHandler's constructor.  
	static List MessageHandlerClassList; // A list of MessageHandlerClassItems 

	// The PostDeath flag indicates that the DeathMsg has been sent and that therfore
	// no more messages will be allowed to be sent from this point on!
	static BOOL PostDeath;
};		

/********************************************************************************************

>	class MessageHandlerList: public Listitem					  

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/3/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	A MessageHandlerList contains a List of MessageHandler objects derived 
				from the same runtime class 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

class MessageHandlerList: public ListItem					  
{
	CC_DECLARE_MEMDUMP(MessageHandlerList);

public:
	MessageHandlerList(CCRuntimeClass* Class); 
	CCRuntimeClass*  MessageHandlerClass; 
	List 				m_List;	// A list of all MessageHandler objects derived from 
								// MessageHandlerClass. 
}; 
#endif 
