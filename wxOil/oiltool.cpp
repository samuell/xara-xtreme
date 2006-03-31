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
*/

#include "camtypes.h"
#include "oiltool.h"
#include "oilmods.h"

DECLARE_SOURCE("$Revision$");

/********************************************************************************************

>	OILTool::OILTool(UINT32 ModuleID)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/7/93
	Inputs:		ModuleID - the ID of the module that owns this Tool.
	Outputs:	-
	Returns:	-
	Purpose:	Create an OILTool object that can be linked to a Tool object (via a
				ToolListItem object).  This object (the OILTool) knows the instance handle
				of the DLL that contains the tool.
	Errors:		-
	SeeAlso:	Tool; ToolListItem; Module; ModuleListItem

********************************************************************************************/


OILTool::OILTool(UINT32 ModuleID)
{
	// Obtain and remember the Instance handle of the DLL that holds the resources
	// of the parent module of this tool (and hence holds the resources of the tool).
	hInstance = OILModule::GetInstance(ModuleID);
}

/********************************************************************************************

>	HCURSOR OILTool::LoadCursor(UINT32 ID)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/7/93
	Inputs:		ID - The resource ID of the cursor to load.
	Outputs:	-
	Returns:	The handle to the loaded cursor, or NULL if unsuccessful
	Purpose:	Find a cursor resource, which may be in the kernel's resources, or it
				may be from the tool's own resources.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
	
HCURSOR OILTool::LoadCursor(UINT32 ID)
{
	HINSTANCE Inst;
	
	if ((DLLs[0] == (HINSTANCE)HINSTANCE_ERROR || DLLs[0] == 0) || ((ID >= KID_FIRST) && (ID <= KID_LAST)))
		Inst = AfxGetResourceHandle(); 	// Kernel resource - get the HINSTANCE of our dll, or .EXE
	else
		Inst = hInstance; 				// otherwise use the HINSTANCE of the Tool's DLL

	// If the HINSTANCE we have is valid, attempt to load the resource, otherwise
	// return a NULL value.
		
	if (Inst > (HINSTANCE)HINSTANCE_ERROR)
		return ::LoadCursor(Inst, MAKEINTRESOURCE(ID));
	else
		return NULL;
}

/********************************************************************************************

>	HBITMAP OILTool::LoadBitmap(UINT32 ID)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/7/93
	Inputs:		ID - The resource ID of the bitmap to load.
	Outputs:	-
	Returns:	The handle to the loaded bitmap, or NULL if unsuccessful
	Purpose:	Find a bitmap resource, which may be in the kernel's resources, or it
				may be from the tool's own resources.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
	
HBITMAP OILTool::LoadBitmap(UINT32 ID)
{
	HINSTANCE Inst;
	
	if ((DLLs[0] == (HINSTANCE)HINSTANCE_ERROR || DLLs[0] == 0) || ((ID >= KID_FIRST) && (ID <= KID_LAST)))
		Inst = AfxGetResourceHandle(); 	// Kernel resource - get the HINSTANCE of our .EXE
	else
		Inst = hInstance; 				// otherwise use the HINSTANCE of the Tool's DLL
		
	// If the HINSTANCE we have is valid, attempt to load the resource, otherwise
	// return a NULL value.
		
	if (Inst > (HINSTANCE)HINSTANCE_ERROR)
		return ::LoadBitmap(Inst, MAKEINTRESOURCE(ID));
	else
		return NULL;
}

/********************************************************************************************

>	HICON OILTool::LoadIcon(UINT32 ID)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/7/93
	Inputs:		ID - The resource ID of the icon to load.
	Outputs:	-
	Returns:	The handle to the loaded icon, or NULL if unsuccessful
	Purpose:	Find an icon resource, which may be in the kernel's resources, or it
				may be from the tool's own resources.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
	
HICON OILTool::LoadIcon(UINT32 ID)
{
	HINSTANCE Inst;
	
	if ((DLLs[0] == (HINSTANCE)HINSTANCE_ERROR || DLLs[0] == 0) || ((ID >= KID_FIRST) && (ID <= KID_LAST)))
		Inst = AfxGetResourceHandle(); 	// Kernel resource - get the HINSTANCE of our .EXE
	else
		Inst = hInstance; 				// otherwise use the HINSTANCE of the Tool's DLL
		
	// If the HINSTANCE we have is valid, attempt to load the resource, otherwise
	// return a NULL value.
		
	if (Inst > (HINSTANCE)HINSTANCE_ERROR)
		return ::LoadIcon(Inst, MAKEINTRESOURCE(ID));
	else
		return NULL;
}
	
/********************************************************************************************

>	BOOL OILTool::LoadString(UINT32 ID, String*)

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/93
	Inputs:		ID - The resource ID of the string to load.
	Outputs:	The String that was loaded
	Returns:	TRUE if String is loaded successfully, or FALSE Otherwise
	Purpose:	Find a string resource, which may be in the kernel's resources, or it
				may be from the tool's own resources.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
	
BOOL OILTool::LoadString(UINT32 ID, char* Description)
{
	// Maximum allowable size of a windows string resource
	const INT32 MaxStringSize = 256;

	HINSTANCE Inst;
	
	if ((DLLs[0] == (HINSTANCE)HINSTANCE_ERROR || DLLs[0] == 0) || ((ID >= KID_FIRST) && (ID <= KID_LAST)))
		Inst = AfxGetResourceHandle(); 	// Kernel resource - get the HINSTANCE of our .EXE
	else
		Inst = hInstance; 				// otherwise use the HINSTANCE of the Tool's DLL
		
	// If the HINSTANCE we have is valid, attempt to load the resource, otherwise
	// return FALSE.
		
	if (Inst > (HINSTANCE)HINSTANCE_ERROR)
		return ::LoadString(Inst, ID, Description, MaxStringSize);
	else
		return FALSE;
}
	
