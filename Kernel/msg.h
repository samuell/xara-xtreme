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
 * */ 

// This file contains a hierarchy of messages and broadcast macros 

#ifndef INC_MSG
#define INC_MSG        

#include "dlgtypes.h"
#include "errors.h"

class Operation;

// Place the CC_IMPLEMENT_DYNAMIC macros in pump.cpp if you like

/********************************************************************************************

>	class Msg: public CCObject

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/3/94
	Purpose:	The base Msg class from which all messages must be derived 
	SeeAlso:	-

********************************************************************************************/

class Msg: public CCObject
{
	CC_DECLARE_DYNAMIC(Msg); 
	Msg() { }
	Msg(const Msg & msg) { }
};

/********************************************************************************************

>	class OpMsg: public Msg

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/3/94
	Purpose:	This message is sent whenever something happens to an operation
	SeeAlso:	-

********************************************************************************************/

class OpMsg: public Msg
{
	CC_DECLARE_DYNAMIC(OpMsg);
public:

	enum OpMsgType
	{
		BEGIN,			// An operation is about to be performed
		END, 			// An operation has successfully ended
		BEFORE_UNDO,	// Sent prior to the operation being undone
		AFTER_UNDO,		// Sent after the operation has been undone
		BEFORE_REDO,	// Sent prior to the operation being redone
		AFTER_REDO		// Sent after the operation has been redone
	};

	OpMsgType MsgType; 	// The message type
	Operation* pOp; 	// A pointer to the operation, can be NULL 

	OpMsg(Operation* pOperation, OpMsgType TypeMsg)
	  : MsgType(TypeMsg), pOp(pOperation)
		{ /* empty */ }
};


/********************************************************************************************

>	class DialogMsg: public Msg

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/3/94
	Purpose:	A DialogMsg is sent whenever the user interacts with the gadgets in
				a dialog box, It is only sensible to send this message to DialogOp objects.

				** Important **
				
				When processing the DialogMessage you should always use the IS_OUR_DIALOG_MSG
				macro. In normal circumstances this will return TRUE if the DlgWndID of 
				the message is the same as the DialogOps window ID. However sometimes it 
				is neccessary to send a DialogMsg to all DialogOps. For example when Camelot is
				dying we need to send a DIM_CANCEL to all open dialogs. The IS_DIALOG_MSG macro
				provides clever handling to ensure that all open DialogOps receive this message.
				in this situation the DlgWndID will be NULL. So when handling the DIM_CANCEL
				message you should not assume that DlgWndID is a valid window identifier. 

				after processing the dialog message you should always 
				
				return(DLG_EAT_IF_HUNGRY(DlgMsg)); 
				
				The DLG_EAT_IF_HUNGRY macro will return EAT_MSG if the message should not be
				sent on to other dialogOps and OK if it should.


				Syntax of constructor is:
				DialogMsg(CWindowID DlgID, CDlgMessage Msg, CGadgetID Gadget, INT32 LongParam=0)


				  
	SeeAlso:	IS_OUR_DIALOG_MSG
	SeeAlso:	EAT_IF_HUNGRY

********************************************************************************************/

class DialogMsg: public Msg
{
	CC_DECLARE_DYNAMIC(DialogMsg); 
public:
	CWindowID 	DlgWndID;
	CDlgMessage DlgMsg;
	CGadgetID 	GadgetID;
	UINT_PTR	DlgMsgParam;		// Message specific data will be placed in here
									// Note: if more complex data needs to be stored in the
									// 		 future this field ought to become a pointer
									//		 to a struct which holds the data (rather than
									//		 adding huge structs into this class).
	CDlgResID PageID;				// Only relevent for tabbed dialogs. Specifies which page
									// (in the tabbed dialog) generated the message. If the dialog
									// itself sent the message then this will be 0.

	DialogMsg(CWindowID DlgID, CDlgMessage Msg, CGadgetID Gadget, UINT_PTR LongParam=0, CDlgResID Pageid=0): 
		DlgWndID(DlgID), DlgMsg(Msg), GadgetID(Gadget), DlgMsgParam(LongParam), PageID(Pageid)
	{
	}
	DialogMsg(const DialogMsg & msg) :
		DlgWndID(msg.DlgWndID), DlgMsg(msg.DlgMsg), GadgetID(msg.GadgetID), DlgMsgParam(msg.DlgMsgParam), PageID(msg.PageID) { } // copy constructor
		
};



/********************************************************************************************

>	class DeathMsg: public Msg

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/3/94
	Purpose:	This message is sent just before camelot's death. 
				When a long-life operation receives this message it should tidy-up then call
				End(). 
	SeeAlso:	-

********************************************************************************************/

class DeathMsg: public Msg
{
	CC_DECLARE_DYNAMIC(DeathMsg); 
	DeathMsg() { }
};

/********************************************************************************************

>	MsgResult BROADCAST_TO_ALL(Msg) 

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/3/94
	Inputs:		-
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

	Purpose:	The BROADCAST_TO_ALL macro is used to send a message to all MessageHandler objects
				in the system. If any MessageHandler returns a FAIL value then InformError is
				called.

				Usage:

				BROADCAST_TO_ALL(AMsg(p1,p2,p3))
			
	SeeAlso:	BROADCAST_TO_CLASS
	SeeAlso:	MessageHandler::Broadcast

********************************************************************************************/
#define BROADCAST_TO_ALL(Message) \
	MessageHandler::pTmpMsg = new Message, \
		(MessageHandler::pTmpMsg == NULL) ? (FAIL) :	\
	MessageHandler::Broadcast(MessageHandler::pTmpMsg)	 \


/********************************************************************************************

>	MsgResult BROADCAST_TO_CLASS(Msg,Class) 

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/3/94
	Inputs:		-
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
			
	Purpose:	The BROADCAST_TO_ALL macro is used to send a message to all MessageHandler objects
				with a class derived from or equal to Class.  

				
				To send a message to all DialogOp objects

				BROADCAST_TO_CLASS(DialogMsg(p1,p2,p3), DialogOp)

				To send a message to all Operation objects, including DialogOp objects. 

				BROADCAST_TO_CLASS(AMsg(p1,p2,p3), Operation)

	Errors:		-
	SeeAlso:	BROADCAST_TO_ALL
	SeeAlso:	MessageHandler::Broadcast

********************************************************************************************/

#define BROADCAST_TO_CLASS(Message, Class) \
	MessageHandler::pTmpMsg = new Message, \
		(MessageHandler::pTmpMsg == NULL) ? (FAIL) :	\
	MessageHandler::Broadcast(MessageHandler::pTmpMsg, CC_RUNTIME_CLASS(Class))	 \

// The following are useful little macros for use in a MessageHandler's message function

/********************************************************************************************

>	IS_OUR_DIALOG_MSG(Message)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/3/94
	Inputs:		Message: Msg* 
	Outputs:	-
	Returns:	-
	Purpose:	This macro for use in DialogOp classes evaluates to TRUE if the Message is
				a DialogMsg for the DialogOp. See DialogMsg for a full description of its
				usage. 
	Errors:		-
	SeeAlso:	DialogMsg
	SeeAlso:	DLG_EAT_IF_HUNGRY

********************************************************************************************/
// Returns TRUE if
//		The DialogOps WindowID != NULL and    // The DialogOp is visible (may not be the case if it's a bar)
// 		(The message is a DialogMsg and
//			((The DialogOps WindowID == The message's Window ID) or
// 			 (The message's Window ID is NULL)) // A message for all DialogOps
//		) 
// ** We need all this nonsense so that we can send CANCEL messages sensibly 

#define IS_OUR_DIALOG_MSG(Message)											\
	(																		\
		(WindowID != NULL) && 												\
			(																\
				((Message)->IsKindOf(CC_RUNTIME_CLASS(DialogMsg))) && 		\
					( 														\
						(WindowID == ((DialogMsg*)(Message))->DlgWndID) || 	\
						( ((DialogMsg*)(Message))->DlgWndID == NULL)		\
					)														\
			) 																\
	)
#endif 

/********************************************************************************************

>   DLG_EAT_IF_HUNGRY(DialogMsg)


	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/4/94
	Inputs:		DialogMsg: The dialog message (DialogMsg)
	Outputs:	-
	Returns:	-
	Purpose:	You should use this macro to return from a DialogOp's Message function after
				a DialogMsg has been processed. See DialogMsg for a full description of its use
				
				return(DLG_EAT_IF_HUNGRY(Msg)); 
				 
	Errors:		-
	SeeAlso:	IS_OUR_DIALOG_MSG

********************************************************************************************/

// If the DlgWndID is NULL then the message should be broadcast to all DialogOps so return OK
// otherwise return EAT_MSG

#define DLG_EAT_IF_HUNGRY(DialogMsg)	\
	((DialogMsg)->DlgWndID) == NULL ? OK : EAT_MSG 

/********************************************************************************************

>	MESSAGE_IS_A(Message, MsgClass)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/3/94
	Inputs:		Message: Msg*
				MsgClass: A class eg. DialogMsg
	Outputs:	-
	Returns:	
	Purpose:	This macro determines if Message is a kind of MsgClass
	Errors:		-
	SeeAlso:	-

********************************************************************************************/


#define MESSAGE_IS_A(Message,MsgClass) \
	(Message->IsKindOf(CC_RUNTIME_CLASS(MsgClass)))
