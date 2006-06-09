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
// EditSel.cpp
//
// Edit the selected item whatever that item is...
//
/*
*/

#include "camtypes.h"
//#include "app.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "editsel.h"
//#include "phil.h"
#include "helpids.h"
//#include "resource.h"
//#include "barsdlgs.h"
//#include "ink.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "document.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "spread.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "tool.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "simon.h"
#include "blobs.h"

///////////////////////////////////////////////////////////////////////////////////////////
//									EditSelectionOp

/******************************************************************************************
>	EditSelectionOp::EditSelectionOp()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/01/2004
	Purpose:	Constructs a new EditSelectionOp object: setting default operation flags, and 
				adding it to the Live list. 
******************************************************************************************/

CC_IMPLEMENT_DYNCREATE(EditSelectionOp, Operation)

EditSelectionOp::EditSelectionOp(): Operation()
{
}


/******************************************************************************************
>	void EditSelectionOp::Do(OpDescriptor*)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/01/2004
	Inputs:		An Operation Descriptor
	Purpose:	Actually "DO" a select all operation.
********************************************************************************************/

void EditSelectionOp::Do(OpDescriptor*)
{
	Range* pSelection = GetApplication()->FindSelection();

	if (pSelection)
	{
		Node* pNode = pSelection->FindFirst();
		if (pNode)
		{
			TCHAR* pOpToken = pNode->GetDefaultOpToken();
			if (pOpToken)
			{
				OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor(pOpToken);
				if (pOpDesc)
				{
					pOpDesc->Invoke();
					return;
				}
			}
		}
	}

	End();
}


/********************************************************************************************
>	OpState EditSelectionOp::GetState(String_256* UIDescription, OpDescriptor*)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/01/2004
	Inputs:		String - representing the operation description which may be updated
				OpDescriptor - To help update the state
	Purpose:	Find the state of the EditSelectionOp operation.
********************************************************************************************/

OpState EditSelectionOp::GetState(String_256* UIDescription, OpDescriptor*)
{
	INT32 NumSelObjs=GetApplication()->FindSelection()->Count();

	// If no objects are selected, state this as the reason why it's disabled..
	if (NumSelObjs==0)
	{
		*UIDescription = String_256(_R(IDS_NO_OBJECTS_SELECTED));
	}

	// Never ticked, greyed if no selected objects
	return OpState(FALSE, NumSelObjs==0);
}


/********************************************************************************************
>	BOOL EditSelectionOp::Init()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/01/2004
	Purpose:	Create an OpDescriptor for the Select All operation
********************************************************************************************/

BOOL EditSelectionOp::Init()
{
	BTNOP(EDIT_EDITSELECTION,EditSelectionOp,EDIT);
	return(TRUE);
}


