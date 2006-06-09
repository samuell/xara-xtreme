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
// 

#include "camtypes.h" 
#include "davedlg.h"

//#include "bevres.h"		//URL dialog resources
//#include "resimmap.h"	//URL import string resources

//#include "msg.h" - in camtypes.h [AUTOMATICALLY REMOVED]

#include "xshelpid.h"	//For the help ID

// includes for additions to the rendering tree
//#include "nodeattr.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "lineattr.h"
//#include "attrmgr.h" - in camtypes.h [AUTOMATICALLY REMOVED]

//#include "app.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "document.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "page.h"
//#include "spread.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "nodebmp.h"
#include "bitmpinf.h"
#include "bmpfiltr.h"

//#include "rndrgn.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "grnddib.h"

#include "bmpexdoc.h"

//#include "docview.h" - in camtypes.h [AUTOMATICALLY REMOVED]

#include "chapter.h"

#include "beveler.h"

//#include "oilcoord.h" - in camtypes.h [AUTOMATICALLY REMOVED]

//#include "justin3.h"

//#include "selop.h"        - in camtypes.h [AUTOMATICALLY REMOVED]
#include "mkshapes.h" 
#include "groupops.h"

DECLARE_SOURCE("$Revision$");

CC_IMPLEMENT_DYNCREATE(DaveDlg, DialogOp)

#define new CAM_DEBUG_NEW

// Set up the dialog box details stored as statics in the class
CDlgResID DaveDlg::IDD = _R(IDD_DAVES_DLG);
const CDlgMode DaveDlg::Mode = MODELESS;

BOOL DaveDlg::DontHandleNextMessage = FALSE;
BOOL DaveDlg::DialogWasCancelled = FALSE;

WebAddress DaveDlg::ms_url = WebAddress();

/******************************************************************************************

>	BOOL DaveDlg::OpenAndGetURL(WebAddress* purlToReturn) 


	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/6/97
	Inputs:		-

	Outputs:	purlToReturn	Pointer to a Web Address object

	Returns:	TRUE - Import was pressed
				FALSE - Cancel was pressed
	Purpose:	Starts up the Import from Internet dialog box
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

BOOL DaveDlg::DoCreate() 
{
	return TRUE;
}


/******************************************************************************************

>	DaveDlg::DaveDlg(): DialogOp(DaveDlg::IDD, DaveDlg::Mode) 


	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/4/97
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Default constructor
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

DaveDlg::DaveDlg(): DialogOp(DaveDlg::IDD, DaveDlg::Mode) 
{
	m_Indent = 0;
	m_Contrast = 0;
	m_Angle = 0;
	m_Type = 0;
}

DaveDlg::~DaveDlg()
{
}

/******************************************************************************************

>	MsgResult DaveDlg::Message(CDlgMessage DlgMsg, CGadgetID Gadget) 


	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/4/97
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Handles all messages.

				This function simply maps the message to one of our message handling
				functions below.

	Errors:		-
	SeeAlso:	-

******************************************************************************************/

MsgResult DaveDlg::Message(Msg* Message)
{	
	return DialogOp::Message(Message); 
}

/********************************************************************************************

	DaveDlg message handling functions

	The following group of functions are called from DaveDlg::Message
	when a message is received.

	They call other functions which do whatever needs to be done.

********************************************************************************************/


/********************************************************************************************

	void DaveDlg::OnCreate()

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/4/97
	Inputs:		-
	Outputs:	-
	Returns:	-
	
	Purpose:	Handles a message that the dialog has just been created.

	Errors:		-
	SeeAlso:	DaveDlg::Message()

********************************************************************************************/

void DaveDlg::OnCreate()
{	
	//Put the keyboard focus in the edit field
	// SetKeyboardFocus(_R(IDC_DAVE_LW_EDIT));
}

/********************************************************************************************

	void DaveDlg::OnCommit()

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/4/97
	Inputs:		-
	Outputs:	-
	Returns:	-
	
	Purpose:	Handles a message that the "Import" button has been clicked

	Errors:		-
	SeeAlso:	DaveDlg::Message()

********************************************************************************************/

void DaveDlg::OnCommit()
{	
	//Get the string from the edit field
	/*
	String_256 strUser=GetStringGadgetValue(_R(IDC_URLIMPORT_URL));

	//Now we want to correct this string. To do this we
	//need to set up some correction flags
	WebCorrectFlags wcfToUse;
	wcfToUse.SetForURLImport();

	//Now make a Web Address object out of the string
	//This will correct the string appropriately
	WebAddress urlToFetch(strUser, wcfToUse);

	//And set our member variable
	ms_url=urlToFetch;
	*/


}

/********************************************************************************************

	void DaveDlg::OnURLChanged()

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/4/97
	Inputs:		-
	Outputs:	-
	Returns:	-
	
	Purpose:	Handles a message that the user has typed in the "URL" field

	Errors:		-
	SeeAlso:	DaveDlg::Message()

********************************************************************************************/

void DaveDlg::OnURLChanged()
{	
	//Does nothing at present
	//We may need to handle this if we want the Import... button to be greyed
	//until the user types in the field
}


/******************************************************************************************

>	static OpState	DaveDlg::GetState(String_256*, OpDescriptor*)

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/4/97
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	DaveDlg GetState method
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

OpState	DaveDlg::GetState(String_256*, OpDescriptor*)
{    
	OpState OpSt;
	return(OpSt);
}




/******************************************************************************************

>	BOOL DaveDlg::Init()

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/4/97
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	DaveDlg Init method
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

BOOL DaveDlg::Init()
{
	BOOL InitOK;

	InitOK = RegisterOpDescriptor(
								0,					/* Tool ID */
								_R(IDS_DAVEDLG),
								CC_RUNTIME_CLASS(DaveDlg),
								OPTOKEN_DAVEDLG,
								GetState,
								_R(IDH_Command_Import_from_Web),	/* help ID */
								0,  				/* bubble help */
								0,					/* resource ID */
								0					/* control ID */
								);

	return (InitOK);
}


/******************************************************************************************

>	void DaveDlg::Do(OpDescriptor*)

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/4/97
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Creates then opens the dialog in response to a request from the user.
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

void DaveDlg::Do(OpDescriptor*)
{
	
}

/******************************************************************************************

>	void DaveDlg::DoWithParam(OpDescriptor*)

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/4/97
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Creates then opens the dialog in response to a request from the user and
				allows values to be passed in and returned to the caller via the
				DaveDlgParam class.
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

void DaveDlg::DoWithParam(OpDescriptor*, OpParam* Param)
{
	
}

/******************************************************************************************

>	void DaveDlg::DoChangeInValue()

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/12/98
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Changes the attribute value/s
	Errors:		-
	SeeAlso:	-

******************************************************************************************/


void DaveDlg::DoChangeInValue()
{
}

BOOL DaveDlg::DoBevel()
{
	return TRUE;
}

BOOL DaveDlg::RemoveBevel()
{
	return TRUE;
}

void DaveDlg::UpdateControls()
{
}


