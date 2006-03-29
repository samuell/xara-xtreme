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
// A simple Dialog That does some Gavin Rendering into itself

/*
Code for the colour selector tool added 22/07/99 ap
*/

#include "camtypes.h"

#include "prvwmenu.h"
#include "bmpexprw.h"
//#include "exprwres.h"


// This is not compulsory, but you may as well put it in so that the correct version
// of your file can be registered in the .exe
DECLARE_SOURCE("$Revision: 662 $");

// An implement to match the Declare in the .h file.
// If you have many classes, it is recommended to place them all together, here at the start of the file
CC_IMPLEMENT_DYNCREATE(OpPreviewPopupCommand, Operation)

BitmapExportPreviewDialog *OpPreviewPopupCommand::m_pDialog = NULL;

// This will get Camelot to display the filename and linenumber of any memory allocations
// that are not released at program exit
#define new CAM_DEBUG_NEW



/********************************************************************************************
>	virtual	BOOL PreviewContextMenu :: Build()

	Author:		Stefan_Stoykov (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/5/97
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Builds the context menu
********************************************************************************************/

BOOL PreviewContextMenu :: Build()
{
	BOOL ok = TRUE;

	if (ok) ok = ok && BuildCommand(OPTOKEN_PREVIEW_ZOOM_TOOL);
	if (ok) ok = ok && BuildCommand(OPTOKEN_PREVIEW_PUSH_TOOL);
	if (ok) ok = ok && BuildCommand(OPTOKEN_PREVIEW_COLOUR_SELECTOR_TOOL);
	if (ok) ok = ok && BuildCommand(OPTOKEN_PREVIEW_ZOOM_TO_FIT);
	if (ok) ok = ok && BuildCommand(OPTOKEN_PREVIEW_ZOOM_TO_100);
	if (ok) ok = ok && BuildCommand(OPTOKEN_PREVIEW_1TO1, TRUE);
	if (ok) ok = ok && BuildCommand(OPTOKEN_PREVIEW_HELP);

	return (ok);
}




	
/********************************************************************************************
>	static BOOL OpPreviewPopupCommand::InitPolymorphicCommand(TCHAR *OpToken, UINT32 MenuTextID)

	Author:		Stefan_Stoykov (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/5/97
	Inputs:		OpToken - the token of a menu operation
				MenuTextID - the resource string of the menu command
	Outputs:	-
	Returns:	-
	Purpose:	Registeres a menu operation
********************************************************************************************/

BOOL OpPreviewPopupCommand::InitPolymorphicCommand(TCHAR *OpToken, UINT32 MenuTextID)
{
	return(RegisterOpDescriptor(NULL,								// Tool ID 
								MenuTextID,		 					// String resource ID
								CC_RUNTIME_CLASS(OpPreviewPopupCommand),// Runtime class
								OpToken, 							// Token string
								OpPreviewPopupCommand::GetCommandState,	// GetState function
								NULL,								// help ID
								NULL,								// bubble help
								NULL,								// resource ID
								NULL,								// control ID
								SYSTEMBAR_ILLEGAL,					// Bar ID
								FALSE,								// Receive system messages
								FALSE,								// Smart duplicate operation
								TRUE,								// Clean operation
								NULL,								// No vertical counterpart
								NULL,								// String for one copy only error
								DONT_GREY_WHEN_SELECT_INSIDE		// Auto state flags
								));
}



/********************************************************************************************
>	static BOOL OpPreviewPopupCommand::Init(void)

	Author:		Stefan_Stoykov (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/5/97
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Registeres all the menu operations by calling InitPolymorphicCommand
********************************************************************************************/

BOOL OpPreviewPopupCommand::Init(void)
{
	InitPolymorphicCommand(OPTOKEN_PREVIEW_ZOOM_TOOL,			 _R(IDS_MENU_ZOOM));
	InitPolymorphicCommand(OPTOKEN_PREVIEW_PUSH_TOOL,			 _R(IDS_MENU_PUSH));
	InitPolymorphicCommand(OPTOKEN_PREVIEW_COLOUR_SELECTOR_TOOL, _R(IDS_MENU_SELECTOR));
	InitPolymorphicCommand(OPTOKEN_PREVIEW_ZOOM_TO_FIT,			 _R(IDS_MENU_FIT));
	InitPolymorphicCommand(OPTOKEN_PREVIEW_ZOOM_TO_100,			 _R(IDS_MENU_100));
	InitPolymorphicCommand(OPTOKEN_PREVIEW_1TO1,				 _R(IDS_MENU_1TO1));
	InitPolymorphicCommand(OPTOKEN_PREVIEW_HELP, _R(IDS_PREVIEW_MENU_HELP));

	return (TRUE);
}               



/********************************************************************************************

>	static OpState	OpPreviewPopupCommand::GetCommandState(String_256 *UIDescription, 
															OpDescriptor *pOpDesc)

	Author:		Stefan_Stoykov (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/5/97

	Inputs:		UIDescription - ptr to a string to place a description of what went wrong 
								(if it did!)
				OpDesc  - The OpDescriptor for which we need the state

	Outputs:	UIDescription is updated appropriately

	Returns:	The state of the OpPreviewPopupCommand operation for the given OpDescriptor
	Purpose:	For finding OpPreviewPopupCommand's state. Used only to tick the proper tool, 
				depending on the current mode

********************************************************************************************/

OpState	OpPreviewPopupCommand::GetCommandState(String_256 *UIDescription, OpDescriptor *pOpDesc)
{
	OpState OpSt;
	OpSt.Greyed = FALSE; //always active
	// tick either the push or the zoom tool depending on the current mode
	if (((pOpDesc->Token == String_256(OPTOKEN_PREVIEW_PUSH_TOOL)) && 
				(m_pDialog->GetCurrentTool() == PREVIEW_PUSH_TOOL)) ||
		((pOpDesc->Token == String_256(OPTOKEN_PREVIEW_ZOOM_TOOL)) && 
				(m_pDialog->GetCurrentTool() == PREVIEW_ZOOM_TOOL)) ||
		((pOpDesc->Token == String_256(OPTOKEN_PREVIEW_COLOUR_SELECTOR_TOOL)) &&
				(m_pDialog->GetCurrentTool() == PREVIEW_COLOUR_SELECTOR_TOOL)))
		OpSt.Ticked = TRUE;

	return (OpSt); 
}


/********************************************************************************************
>	virtual void OpPreviewPopupCommand::Do(OpDescriptor* pOpDesc)

	Author:		Stefan_Stoykov (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/5/97
	Inputs:		pOpDesc - the descriptor of the operation
	Outputs:	-
	Returns:	-
	Purpose:	Passes the token of the selected menu command to the dialog so it can be 
				handled there
********************************************************************************************/

void OpPreviewPopupCommand::Do(OpDescriptor* pOpDesc)
{  
	// get the token
	String_256 s(pOpDesc->Token);

	// ask the dialog to handle it
	m_pDialog->DoCommand(&s);

	End();
}

