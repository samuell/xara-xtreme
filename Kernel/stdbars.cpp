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

#include "camtypes.h"
#include "ops.h"

DECLARE_SOURCE("$Revision$");

#include "stdbars.h"

CC_IMPLEMENT_DYNCREATE(StandardBar, DialogOp)

#define new CAM_DEBUG_NEW


/********************************************************************************************

>	class StandardBarMsg: public Msg

	Author:		Alex Bligh <alex@alex.org.uk>
	Created:	17/01/2006
	Purpose:	A tiny message local to the standard bar class
	SeeAlso:	-

********************************************************************************************/

class StandardBarMsg: public Msg
{
	CC_DECLARE_DYNAMIC(StandardBarMsg);

public:

	enum StandardBarMsgType
	{
		HELLO,			// See if one exists
		CLOSE,			// Close one
		LAST
	};

	StandardBarMsgType m_MsgType; 	// The message type
	OpDescriptor* m_pOpDescriptor; 	// A pointer to the operation, can be NULL 

	StandardBarMsg(OpDescriptor* pOpDescriptor, StandardBarMsgType MsgType)
	  : m_MsgType(MsgType), m_pOpDescriptor(pOpDescriptor)
		{ /* empty */ }
};

CC_IMPLEMENT_DYNAMIC(StandardBarMsg, Msg);

/********************************************************************************************

>	static OpState StandardBar::GetState(String_256*, OpDescriptor*)

	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	17/01/2006
	Inputs:		(ignored)
	Outputs:	-
	Returns:	OpState
	Purpose:	GetState method
	Errors:		-
	SeeAlso:	-

This needs to manually grey the OpDesc when there is a particular instance up. We have
to use this because there legitimately more than one instance of this class about.

********************************************************************************************/

OpState StandardBar::GetState(String_256*, OpDescriptor* pOpDesc)
{
	// Note this is a STATIC method, else life would be far too easy
	OpState state;	

	// We can only send a message if at least one of our class exists (who knows why)
	if (MessageHandler::MessageHandlerExists(CC_RUNTIME_CLASS(StandardBar)))
	{
		// Send all message handlers of our class a message
		// For some reason we have to send it to all DialogOps - apparently we can't broadcast to a subclass
		MsgResult m = (BROADCAST_TO_CLASS(StandardBarMsg(pOpDesc, StandardBarMsg::HELLO), DialogOp));
		if (m != OK) state.Ticked=TRUE;
	}
	return state;
}

/********************************************************************************************

>	static void StandardBar::BroadcastClose(OpDescriptor* pOpDesc)

	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	04-Mar-2006
	Inputs:		pOpDesc - opDescriptor of the standard bar type to close
	Outputs:	-
	Returns:	OpState
	Purpose:	GetState method
	Errors:		-
	SeeAlso:	-

Close all StandardBars with the given OpDescriptor

********************************************************************************************/

void StandardBar::BroadcastClose(OpDescriptor* pOpDesc)
{
	if (MessageHandler::MessageHandlerExists(CC_RUNTIME_CLASS(StandardBar)))
	{
		// Send all message handlers of our class a message
		// For some reason we have to send it to all DialogOps - apparently we can't broadcast to a subclass
		BROADCAST_TO_CLASS(StandardBarMsg(pOpDesc, StandardBarMsg::CLOSE), DialogOp);
	}
}

/********************************************************************************************

>	MsgResult StandardBar::Message( Msg* Message)

	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	17/01/2006
	Inputs:		Msg - pointer to message
	Outputs:	-
	Returns:	MsgResult
	Purpose:	Standard Message processor
	Errors:		-
	SeeAlso:	-

This doesn't need to do much as the bitmap button controls handle their own clicks

********************************************************************************************/

MsgResult StandardBar::Message( Msg* Msg)
{
	if (Msg->IsKindOf(CC_RUNTIME_CLASS(StandardBarMsg)))
	{
		StandardBarMsg * pMsg = ((StandardBarMsg *)(Msg));

		// If we're already got one, EAT the message. Else OK, so pass on.
		if (pMsg->m_MsgType == StandardBarMsg::HELLO)
			return (pMsg->m_pOpDescriptor == m_pOpDesc)?EAT_MSG:OK;
		else if (pMsg->m_MsgType == StandardBarMsg::CLOSE)
		{
			if (pMsg->m_pOpDescriptor == m_pOpDesc)
			{
				Close(); // Hide the dialog
				m_pOpDesc = NULL; // zap out OpDescriptor pointer
				End();
				return OK;
			}
		}
		else
			return OK; // who knows what that is all about...
	}

	if (IS_OUR_DIALOG_MSG(Msg))
	{
		DialogMsg* DMsg = (DialogMsg*)Msg;
		// Handle ok button
		if ((DMsg->DlgMsg == DIM_COMMIT) || (DMsg->DlgMsg == DIM_CANCEL))
		{
			Close(); // Hide the dialog
			m_pOpDesc = NULL; // zap out OpDescriptor pointer
			End();
			return OK;
		}
		// Else fall through
	}
	return DialogOp::Message(Msg);
}


/********************************************************************************************

>	static BOOL StandardBar::Init()

	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	17/01/2006
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if successful
	Purpose:	Show the bar (when invoked)
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL StandardBar::Init()
{

// Note we can't use the "String for only one copy" feature because we allow multiple instances
// of this class, provided they are all for different bars (sigh).
#define RegisterStandardBarOpDescriptor(name) \
	(RegisterOpDescriptor( \
						0, \
						_R(IDS_BUTTBAR_M_ ## name ),				/*NORESOURCEFIX*/ \
						CC_RUNTIME_CLASS(StandardBar), \
						_T("IDD_BUTTBAR_" # name),			/* Use Dialog ID as Optoken */	/*NORESOURCEFIX*/ \
						StandardBar::GetState, 				/* GetState					*/ \
						0,									/* help ID 					*/ \
						_R(IDBBL_BAR_ ## name), 			/* Bubble help				*/	/*NORESOURCEFIX*/ \
						0, 									/* Resource ID 				*/ \
						_R(IDC_BTN_BAR_ ## name),			/* Control ID 				*/	/*NORESOURCEFIX*/ \
						SYSTEMBAR_EDIT,						/* Bar ID 					*/ \
						TRUE,								/* Recieve system messages 	*/ \
						FALSE,								/* Smart duplicate operation*/ \
						TRUE,								/* Clean operation 			*/ \
						0,									/* No vertical counterpart 	*/ \
						0,									/* String for one copy only	*/ \
/*						_R(IDS_BAR_ ## name ## _ONE),		   String for one copy only	*/ 	/*NORESOURCEFIX*/ \
						0,									/* Auto state flags 		*/ \
						TRUE								/* Tickable			 		*/ \
					))

	return (
			RegisterStandardBarOpDescriptor(GENERAL) &&
			RegisterStandardBarOpDescriptor(IMAGESETTING) &&
			RegisterStandardBarOpDescriptor(WINDOW) &&
			RegisterStandardBarOpDescriptor(ARRANGE) &&
			RegisterStandardBarOpDescriptor(EDIT) &&
			RegisterStandardBarOpDescriptor(FILE) &&
			RegisterStandardBarOpDescriptor(GALLERIES) &&
			RegisterStandardBarOpDescriptor(STANDARD) &&
			RegisterStandardBarOpDescriptor(TOOLBAR) &&
			RegisterStandardBarOpDescriptor(ANIMATION) &&
			RegisterStandardBarOpDescriptor(FEATHER) &&
			TRUE);

}

/********************************************************************************************

>	void StandardBar::Create(ResourceID dlg)

	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	17/01/2006
	Inputs:		dlg - Resource ID of dialog to use
	Outputs:	-
	Returns:	-
	Purpose:	Show the bar (when invoked)
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL StandardBar::Create(ResourceID dlg)
{
	DlgResID = dlg; // Fortunately nothing needs this earlier

	// A quick hack to make the toolbar dock vertically
	m_Vertical = (dlg == _R(IDD_BUTTBAR_TOOLBAR))?TRUE:FALSE;

	if (DialogOp::Create())
	{
		// Set up  your gadgets here

		return TRUE;
    }
    else return FALSE;
}

/********************************************************************************************

>	void StandardBar::Do(OpDescriptor* pOpDesc)

	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	17/01/2006
	Inputs:		pOpDesc - pointer to the OpDescriptor
	Outputs:	-
	Returns:	-
	Purpose:	Show or hide the bar (when invoked)
	Errors:		-
	SeeAlso:	-

********************************************************************************************/


void StandardBar::Do(OpDescriptor* pOpDesc)
{
	// First translate the bitmap button ID (which we use to determine which opdescriptor invoked
	// us) into a dialog ID

	ResourceID dlg = CamResource::GetResourceID((const TCHAR *)(pOpDesc->Token));

	if (dlg)
	{
		String_256 dummy;
		if (!GetState(&dummy,pOpDesc).Ticked)
		{
			m_pOpDesc = pOpDesc; // For identifying the OpDescriptor
			Create(dlg);
			Open();
		}
		else
		{
			StandardBar::BroadcastClose(pOpDesc);
			End(); // End this Op - The above won't do it because m_pOpDesc is NULL
		}
	}
}


