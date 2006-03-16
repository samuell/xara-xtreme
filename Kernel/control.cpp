// $Id: control.cpp 662 2006-03-14 21:31:49Z alex $
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
// Implementation of NodeController.
//

// header files.
#include "camtypes.h"
#include "control.h"
/*
#include "objchge.h" 
#include "textops.h"

// source-safe version number.
DECLARE_SOURCE("$Revision: 662 $");

// dynamic runtime type info.
CC_IMPLEMENT_DYNAMIC(NodeController, NodeGroup)

// Use debugging 'new' when necessary.
#define new CAM_DEBUG_NEW
*/


/********************************************************************************************

>	virtual BOOL NodeController::AllowOp_AccountForCompound(ObjChangeParam* pParam,
															BOOL SetOpPermissionState,
															BOOL DoPreTriggerEdit)
	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/10/2000
	Inputs:		pParam					information about the requested change, eg which Op.
				SetOpPermissionState	whether flags should be set ready for OnChildChange.
				DoPreTriggerEdit		if TRUE then NameGallery::PreTriggerEdit is called.
										*Must* be TRUE if the calling Op may make any nodes
										change their bounds, eg move, line width, cut.
										Use TRUE if unsure.

	Returns:	TRUE if ANY of the objects in the compound node have allowed the op.

	Purpose:	Overrides Node::AllowOp_AccountForCompound - see definition of that function
				for a general explanation.

				Controller nodes need to inform all of their other children whenever one
				child is queried via AllowOp. This is because unlike a normal group, editing
				one child may well affect its siblings, who must be informed of the edit.

	See Also:	Node::AllowOp, Node::AllowOp_AccountForCompound

	Notes:		Karim 20/12/2000
				Added the OpTextFormat bodge for feathers on bevelled text - sorry!

********************************************************************************************/
/*BOOL NodeController::AllowOp_AccountForCompound(ObjChangeParam* pParam,
												BOOL SetOpPermissionState,
												BOOL DoPreTriggerEdit)
{
	BOOL AnyAllowed = FALSE;

	Node* pCallingChild = (pParam->GetDirection() == OBJCHANGE_CALLEDBYCHILD) ?
													pParam->GetCallingChild() : NULL;
	pParam->SetCallingChild(NULL);

	ObjChangeFlags Flags = pParam->GetChangeFlags();
	UndoableOperation* pChangeOp = pParam->GetOpPointer();

	if (Flags.Attribute || Flags.TransformNode || Flags.RegenerateNode ||
		(pChangeOp != NULL && pChangeOp->IS_KIND_OF(OpTextUndoable)) )
	{

		ObjChangeDirection OldDirection = pParam->GetDirection();
		pParam->SetDirection(OBJCHANGE_CALLEDBYPARENT);
		Node* pNode = FindFirstChild();

		BOOL InformGeomLinkedAttrs =	SetOpPermissionState &&
										pChangeOp != NULL &&
										pChangeOp->MayChangeNodeBounds();

		while (pNode != NULL)
		{
			if (pNode != pCallingChild)
			{
				if (pNode->IsAnObject())
					AnyAllowed |= pNode->AllowOp(pParam, SetOpPermissionState, DoPreTriggerEdit);
				else
				{
					if (pNode->IsAnAttribute() && ((NodeAttribute*)pNode)->IsLinkedToNodeGeometry())
						if (InformGeomLinkedAttrs)
							((NodeAttribute*)pNode)->LinkedNodeGeometryHasChanged(pChangeOp);
				}
			}

			pNode = pNode->FindNext();
		}

		pParam->SetDirection(OldDirection);
	}

	// if setting flags and any child allowed it, set this permission allowed
	if (SetOpPermissionState && AnyAllowed)
		SetOpPermission(PERMISSION_ALLOWED);

	return AnyAllowed;
}
*/
