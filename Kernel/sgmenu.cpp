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

// Context sensitive menu for SuperGalleries

/*
*/

#include "camtypes.h"

#include "sgmenu.h"
#include "sgallery.h"

DECLARE_SOURCE("$Revision$");

CC_IMPLEMENT_MEMDUMP(GalleryContextMenu, ContextMenu)
CC_IMPLEMENT_DYNCREATE(OpGalleryCommand, Operation);

#define new CAM_DEBUG_NEW

/********************************************************************************************

>	GalleryContextMenu::GalleryContextMenu(SGMenuID TheMenuType, SuperGallery *ParentGallery)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/9/95

	Inputs:		TheMenuType - Determines whether this will be a gallery-options dropdown menu
				or a gallery-item/group pop-up menu.

				ParentGallery - The gallery whose options are to be shown

	Purpose:	To construct ;-)

	Notes:		The default constructor will ERROR3 - always use this variant

********************************************************************************************/

GalleryContextMenu::GalleryContextMenu()
{
	ERROR3("GalleryContextMenu - DON'T call the default constructor!");
	MenuType = SGMENU_OPTIONS;
	ParentGallery = NULL;
}

GalleryContextMenu::GalleryContextMenu(SGMenuID TheMenuType, SuperGallery *ParentGal)
{
	MenuType = TheMenuType;
	ParentGallery = ParentGal;
}



/********************************************************************************************

>	virtual BOOL GalleryContextMenu::Build(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/9/95

	Returns:	TRUE if menu built OK
				FALSE (and sets error) otherwise

	Purpose:	To build the SuperGallery pop-up menu. (Called whn the list is adjust-clicked)
				The exact type (options or item-pop-up) of menu depends on the value passed\
				into the constructor.

********************************************************************************************/

BOOL GalleryContextMenu::Build(void)
{
	if (ParentGallery == NULL)
	{
		ERROR3("GalleryContextMenu incorrectly constructed");
		return(FALSE);
	}

	// It's all up to the gallery to fill us in appropriately
	return(ParentGallery->BuildCommandMenu(this, MenuType));
}






//-------------------------------------------------------------------------------------------
//- Polymorphic gallery menu-item Ops


/********************************************************************************************

>	static BOOL OpGalleryCommand::InitPolymorphicCommand(StringBase *OpToken, UINT32 MenuTextID)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/9/95

	Inputs:		OpToken - The OpToken for the new Op
				This must be of the form "Gallery name:Gallery command"
				Each portion must be no more than 31 characters long

				MenuTextID - A String Resource ID for the menu item text

	Returns:	TRUE if it succeeded, FALSE if not enough memory to register the OpDescriptor

	Purpose:	Given a Gallery command OpToken of the form "Gallery name:Command name"
				this creates a new OpDescriptor for the OpGalleryCommand Op. This in turn
				will provide an Upcall to the named Gallery containing the given command
				string, whenever the Op is invoked.

********************************************************************************************/

BOOL OpGalleryCommand::InitPolymorphicCommand(StringBase *OpToken, UINT32 MenuTextID)
{
	return(RegisterOpDescriptor(0,									// Tool ID 
								MenuTextID,		 					// String resource ID
								CC_RUNTIME_CLASS(OpGalleryCommand),	// Runtime class
								(TCHAR *) (*OpToken), 				// Token string
								OpGalleryCommand::GetCommandState,	// GetState function
								0,									// help ID
								0,									// bubble help
								0,									// resource ID
								0,									// control ID
								SYSTEMBAR_ILLEGAL,					// Bar ID
								FALSE,								// Recieve system messages
								FALSE,								// Smart duplicate operation
								TRUE,								// Clean operation
								0,									// No vertical counterpart
								0,									// String for one copy only error
								DONT_GREY_WHEN_SELECT_INSIDE		// Auto state flags
								));
}



/********************************************************************************************

>	static SuperGallery *OpGalleryCommand::GetGalleryAndCommand(OpDescriptor *pOpDesc,
																String_32 *Command)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/9/95
	Inputs:		pOpDesc - The Op Descriptor for the Op

	Outputs:	Command - returned as "" or as the command string from the OpToken

	Returns:	NULL if the gallery wasn't found, or a pointer to the SuperGallery which
				should process the command

	Purpose:	Given a Gallery command OpDescriptor of the form "Gallery name:Command name"
				this returns a pointer to the gallery to use and the string "Command name"
				which is the command that gallery should apply.

********************************************************************************************/

SuperGallery *OpGalleryCommand::GetGalleryAndCommand(OpDescriptor *pOpDesc, String_32 *Command)
{
	String_256 Temp = pOpDesc->Token;
	TCHAR *Str = (TCHAR *)Temp;

	*Command = TEXT("");

	while (*Str != '\0' && *Str != ':')
		Str++;

	if (*Str == '\0')
		return(NULL);

	// Copy the right end of the string into Command for return
	*Command = (TCHAR *) (Str+1);

	// Find the gallery referenced by the left end of the string
	*Str = '\0';		// NULL Terminate the left end of the string

	String_32 Bob;
	Temp.Left(&Bob, 31);
	SuperGallery* pSuperGallery = SuperGallery::FindSuperGallery(Bob , GalleryBarNameLimitSize);

	if (pSuperGallery != NULL && pSuperGallery->IsKindOf(CC_RUNTIME_CLASS(SuperGallery)))
		return((SuperGallery *) pSuperGallery);

	// Not found, or isn't a gallery?! (eep!)
	return(NULL);
}



/********************************************************************************************

>	static BOOL OpGalleryCommand::Init(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/9/95

	Returns:	TRUE if the operation could be successfully initialised 
				FALSE if no more memory could be allocated 

	Purpose:	OpGalleryCommand initialiser method.

	Notes:		This Init does nothing - The Op does not have its own OpDescriptor.
				Instead, each gallery will add an OpDescriptor for each command they
				wish to add to their menu, all of which will reference this Op.

	SeeAlso:	SuperGallery::InitMenuCommand

********************************************************************************************/

//BOOL OpGalleryCommand::Init(void)
//{
//	return (TRUE);
//}               



/********************************************************************************************

>	static OpState OpGalleryCommand::GetCommandState(String_256 *UIDescription,
														OpDescriptor *pOpDesc)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/9/95

	Inputs:		UIDescriptoin - ptr to a string to place a description of what went wrong (if it did!)
				OpDesc  - The OpDescriptor for which we need the state

	Outputs:	UIDescription is updated appropriately

	Returns:	The state of the OpGalleryCommand operation for the given OpDescriptor
	Purpose:	For finding OpGalleryCommand's state. 

********************************************************************************************/

OpState	OpGalleryCommand::GetCommandState(String_256 *UIDescription, OpDescriptor *pOpDesc)
{
	String_32 Command;
	SuperGallery *ParentGallery = GetGalleryAndCommand(pOpDesc, &Command);

	if (ParentGallery != NULL)
		return(ParentGallery->GetCommandState(&Command, UIDescription));

	ERROR3("Attempt to get state for non-existent Gallery command");

	// Return a "safe" shaded state
	OpState OpSt;
	OpSt.Greyed = TRUE;
	*UIDescription = "";
	return (OpSt);   
}



/********************************************************************************************

>	void OpGalleryCommand::Do(OpDescriptor* pOpDesc)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/9/95
	Inputs:		pOpDesc = ptr to the op descriptor
	Outputs:	-
	Returns:	-
	Purpose:	The nudge op's Do() function.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void OpGalleryCommand::Do(OpDescriptor* pOpDesc)
{  
	String_32 Command;
	SuperGallery *ParentGallery = GetGalleryAndCommand(pOpDesc, &Command);

	if (ParentGallery != NULL)
		ParentGallery->DoCommand(&Command);

	End();
}

