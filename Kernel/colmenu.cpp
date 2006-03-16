// $Id: colmenu.cpp 662 2006-03-14 21:31:49Z alex $
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

// Context sensitive menu for the Colour Editor

/*
*/

#include "camtypes.h"

#include "coldlog.h"
#include "colmenu.h"
//#include "galstr.h"
//#include "jason.h"

//#include "will2.h"

DECLARE_SOURCE("$Revision: 662 $");


CC_IMPLEMENT_MEMDUMP(ColEditContextMenu, ContextMenu)
CC_IMPLEMENT_DYNCREATE(OpColEditCommand, Operation);


#define new CAM_DEBUG_NEW



String_32 ColCmd_Help("ColEdit:Help");
String_32 ColCmd_Name("ColEdit:Name");
String_32 ColCmd_HSV("ColEdit:HSV");
String_32 ColCmd_RGB("ColEdit:RGB");
String_32 ColCmd_CMYK("ColEdit:CMYK");
String_32 ColCmd_Grey("ColEdit:Grey");
String_32 ColCmd_EditParent("ColEdit:EditParent");
#ifdef WEBSTER
String_32 ColCmd_NewNColour("ColEdit:NewNColour");
#endif //WEBSTER



/********************************************************************************************

>	ColEditContextMenu::ColEditContextMenu()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/10/95

	Purpose:	To construct ;-)

********************************************************************************************/

ColEditContextMenu::ColEditContextMenu()
{
}



/********************************************************************************************

>	virtual BOOL ColEditContextMenu::Build(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/10/95

	Returns:	TRUE if menu built OK
				FALSE (and sets error) otherwise

	Purpose:	To build the colour editor pop-up menu. (Called when the window is adjust-clicked)

********************************************************************************************/

BOOL ColEditContextMenu::Build(void)
{
	BOOL ok = TRUE;

// WEBSTER - Martin 11/07/97
// Edit of the drop down colour model menu list
#ifdef WEBSTER
	if (ok) ok = ok && BuildCommand(ColCmd_NewNColour);			// New named colour
#endif // WEBSTER
	if (ok) ok = ok && BuildCommand(ColCmd_Name);				// Rename
	if (ok) ok = ok && BuildCommand(ColCmd_EditParent, TRUE);	// Edit Parent - With a separator

	if (ok) ok = ok && BuildCommand(ColCmd_HSV);				// Change colour model
	if (ok) ok = ok && BuildCommand(ColCmd_RGB,TRUE);

#ifndef WEBSTER
	if (ok) ok = ok && BuildCommand(ColCmd_CMYK);
	if (ok) ok = ok && BuildCommand(ColCmd_Grey, TRUE);			// Plus a separator
#endif // WEBSTER

	if (ok) ok = ok && BuildCommand(ColCmd_Help);				// Help
	
	return(ok);
}






//-------------------------------------------------------------------------------------------
//- Polymorphic gallery menu-item Ops


/********************************************************************************************

>	static BOOL OpColEditCommand::InitPolymorphicCommand(StringBase *OpToken, UINT32 MenuTextID)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/10/95

	Inputs:		OpToken - The OpToken for the new Op - use "ColEdit_Command"

				MenuTextID - A String Resource ID for the menu item text

	Returns:	TRUE if it succeeded, FALSE if not enough memory to register the OpDescriptor

	Purpose:	Given a command OpToken, this creates a new OpDescriptor for the OpColEditCommand Op.
				This in turn will provide an Upcall to the Editor containing the given command
				string, whenever the Op is invoked.

********************************************************************************************/

BOOL OpColEditCommand::InitPolymorphicCommand(StringBase *OpToken, UINT32 MenuTextID)
{
	return(RegisterOpDescriptor(NULL,								// Tool ID 
								MenuTextID,		 					// String resource ID
								CC_RUNTIME_CLASS(OpColEditCommand),	// Runtime class
								(TCHAR *) (*OpToken), 				// Token string
								OpColEditCommand::GetCommandState,	// GetState function
								NULL,								// help ID
								NULL,								// bubble help
								NULL,								// resource ID
								NULL,								// control ID
								SYSTEMBAR_ILLEGAL,					// Bar ID
								FALSE,								// Recieve system messages
								FALSE,								// Smart duplicate operation
								TRUE,								// Clean operation
								NULL,								// No vertical counterpart
								NULL,								// String for one copy only error
								DONT_GREY_WHEN_SELECT_INSIDE		// Auto state flags
								));
}



/********************************************************************************************

>	static BOOL OpColEditCommand::Init(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/10/95

	Returns:	TRUE if the operation could be successfully initialised 
				FALSE if no more memory could be allocated 

	Purpose:	OpColEditCommand initialiser method.

	Notes:		Initialises all of the commands available to the colour editor

	SeeAlso:	SuperGallery::InitMenuCommand

********************************************************************************************/

BOOL OpColEditCommand::Init(void)
{
#ifdef WEBSTER
	InitPolymorphicCommand((StringBase *) &ColCmd_NewNColour, _R(IDS_NEWCOLOUR));
#endif // WEBSTER
	InitPolymorphicCommand((StringBase *) &ColCmd_Name, _R(IDS_SGMENU_RENAME));
	InitPolymorphicCommand((StringBase *) &ColCmd_EditParent, _R(IDS_COLMENU_EDITPARENT));

	InitPolymorphicCommand((StringBase *) &ColCmd_HSV, _R(IDS_COLMODEL_HSVT));
	InitPolymorphicCommand((StringBase *) &ColCmd_RGB, _R(IDS_COLMODEL_RGBT));
	InitPolymorphicCommand((StringBase *) &ColCmd_CMYK, _R(IDS_COLMODEL_CMYK));
	InitPolymorphicCommand((StringBase *) &ColCmd_Grey, _R(IDS_COLMODEL_GREY));

	InitPolymorphicCommand((StringBase *) &ColCmd_Help, _R(IDS_COLMENU_HELP));

	return (TRUE);
}               



/********************************************************************************************

>	static OpState OpColEditCommand::GetCommandState(String_256 *UIDescription,
														OpDescriptor *pOpDesc)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/10/95

	Inputs:		UIDescription - ptr to a string to place a description of what went wrong (if it did!)
				OpDesc  - The OpDescriptor for which we need the state

	Outputs:	UIDescription is updated appropriately

	Returns:	The state of the OpColEditCommand operation for the given OpDescriptor
	Purpose:	For finding OpColEditCommand's state. 

********************************************************************************************/

OpState	OpColEditCommand::GetCommandState(String_256 *UIDescription, OpDescriptor *pOpDesc)
{
	if (ColourEditDlg::TheEditor != NULL)
		return(ColourEditDlg::TheEditor->GetCommandState(&pOpDesc->Token, UIDescription));

	// else return a "safe" shaded state
	OpState OpSt;
	OpSt.Greyed = TRUE;
	*UIDescription = String_256(_R(IDS_K_COLMENU_NOEDITOR));
	return (OpSt); 
}



/********************************************************************************************

>	void OpColEditCommand::Do(OpDescriptor* pOpDesc)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/10/95
	Inputs:		pOpDesc = ptr to the op descriptor
	Outputs:	-
	Returns:	-
	Purpose:	The nudge op's Do() function.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void OpColEditCommand::Do(OpDescriptor* pOpDesc)
{  
	if (ColourEditDlg::TheEditor != NULL)
		ColourEditDlg::TheEditor->DoCommand(&pOpDesc->Token);

	End();
}

