// $Id: opretro.cpp 662 2006-03-14 21:31:49Z alex $
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
// The Freehand Retro Fitting operation

/*
*/

#include "camtypes.h"

#include "nodepath.h"
#include "paths.h"
//#include "rik.h"
#include "opretro.h"
#include "objchge.h"


// Set things up so that the tool will be listed in the Dialog box
DECLARE_SOURCE("$Revision: 662 $");

// Declare the class
CC_IMPLEMENT_DYNCREATE(OpRetroFit, SelOperation)

// Improved memory tracking
#define new CAM_DEBUG_NEW



/********************************************************************************************

>	BOOL OpRetroFit::Declare()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/5/94
	Returns:	TRUE if all went OK, False otherwise
	Purpose:	Adds the operation to the list of all known operations

********************************************************************************************/

BOOL OpRetroFit::Init()
{
	return (RegisterOpDescriptor(
								0, 
								_R(IDS_RETROFITOP),
								CC_RUNTIME_CLASS(OpRetroFit), 
								OPTOKEN_RETROFIT,
								OpRetroFit::GetState,
								0,	/* help ID */
								0,
								0	/* bitmap ID */));
}


/********************************************************************************************

>	OpState OpRetroFit::GetState(String_256* Description, OpDescriptor*)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/5/94
	Outputs:	Description - GetState fills this string with an approriate description
				of the current state of the push tool
	Returns:	The state of the operation, so that menu items (ticks and greying can be
				done properly
	Purpose:	Find out the state of the operation at the specific time

********************************************************************************************/

OpState OpRetroFit::GetState(String_256* Description, OpDescriptor*)
{
	OpState Blobby;
	
	return Blobby;
}






/********************************************************************************************

>	BOOL OpRetroFit::BuildUndo(NodePath* pPreviousNode, NodePath* pNewNode)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/5/94
	Inputs:		pPreviousNode - The original path. We will place our new path immediatly
				after this node in the tree
				pNewNode - The new refitted path.
	Returns:	TRUE if it worked, FALSE if it did not
	Purpose:	Builds the necessary Undo information for the refitting of a freehand path

********************************************************************************************/

BOOL OpRetroFit::BuildUndo(NodePath* pPreviousNode, NodePath* pNewNode)
{
	// A few quick error checks
	ENSURE(pPreviousNode!=NULL, "Previous Node was NULL in RetroFit::BuildUndo()");
	ENSURE(pPreviousNode->IsKindOf(CC_RUNTIME_CLASS(NodePath)), "Previous Node not a path");

	ENSURE(pNewNode!=NULL, "New Node was NULL in RetroFit::BuildUndo()");
	ENSURE(pNewNode->IsKindOf(CC_RUNTIME_CLASS(NodePath)), "New Node not a path");

	// Falg to see if it has worked
	BOOL IsOperationOk = TRUE;

	// Start the undo of the selected item
	IsOperationOk = DoStartSelOp(FALSE);

	// Will the node allow this op to take place?
	ObjChangeFlags cFlags;
	cFlags.ReplaceNode = TRUE;
	ObjChangeParam ObjChange(OBJCHANGE_STARTING,cFlags,pPreviousNode,this);
	if (IsOperationOk)
		IsOperationOk = pPreviousNode->AllowOp(&ObjChange);
	
	// insert our new object
	if (IsOperationOk)
	{
		// Insert the new node into the tree
		IsOperationOk = DoInsertNewNode(pNewNode, pPreviousNode, NEXT, TRUE);
	}

	// Invalidate the region covered by the old node
	if (IsOperationOk)
		IsOperationOk = DoInvalidateNodeRegion(pPreviousNode, TRUE);

	// Make sure everything has worked
	if (IsOperationOk)
	{
		// Remove the old node
		IsOperationOk = DoHideNode(pPreviousNode, TRUE);
	}

	ObjChange.Define(OBJCHANGE_FINISHED,cFlags,pPreviousNode,this);
	IsOperationOk = UpdateChangedNodes(&ObjChange);

	// If something went wrong then fail
	if (!IsOperationOk)
		FailAndExecute();
	
	// End the operation properly
	End();

	// return a value back to the caller
	return IsOperationOk;
}





/********************************************************************************************

>	void OpRetroFit::Do(OpDescriptor* pOpDesc)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/5/94
	Inputs:		pOpDesc - An OpDescriptor thing
	Purpose:	Builds all the undo info etc for retro fitting a freehand path.

********************************************************************************************/

void OpRetroFit::Do(OpDescriptor* pOpDesc)
{
//	if (IsUserName("Rik")) TRACE( _T("Write code here.."));
}



