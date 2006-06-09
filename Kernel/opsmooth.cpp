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
// Implementation file for operation to smooth whole paths
// or between selected points

/*
*/

#include "camtypes.h"
//#include "resource.h"
//#include "barsdlgs.h" 
//#include "mike.h"
//#include "ops.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "opsmooth.h"
//#include "app.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "document.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "nodepath.h"
#include "ophist.h"
//#include "jim.h"

DECLARE_SOURCE("$Revision$");
CC_IMPLEMENT_DYNCREATE( OpSmoothSelection, SelOperation )

#define new CAM_DEBUG_NEW


/********************************************************************************************

>	OpSmoothSelection::OpSmoothSelection() 

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/10/94
	Purpose:	OpSmoothSelection() constructor
	SeeAlso:	-

********************************************************************************************/

OpSmoothSelection::OpSmoothSelection()
{
	// Dummy constructor
}
  


/********************************************************************************************

>	BOOL OpSmoothSelection::Init()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/7/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the operation could be successfully initialised 
				FALSE if no more memory could be allocated 
				
	Purpose:	OpDeletePoints initialiser method
	Errors:		ERROR will be called if there was insufficient memory to allocate the 
				operation.
	SeeAlso:	-

********************************************************************************************/

BOOL OpSmoothSelection::Init()
{
	return (RegisterOpDescriptor(0,									// tool ID
 								_R(IDS_SMOOTHSELECTIONOP),				// string resource ID
								CC_RUNTIME_CLASS(OpSmoothSelection),// runtime class for Op
 								OPTOKEN_SMOOTHSELECTION,			// Ptr to token string
 								OpSmoothSelection::GetState,		// GetState function
 								0,									// help ID
 								_R(IDBBL_SMOOTHSELECTIONOP),			// bubble help ID
 								_R(IDD_BARCONTROLSTORE),				// resource ID
								_R(IDC_BTN_SMOOTHSELECTIONOP),			// control ID
								SYSTEMBAR_EDIT,						// Group bar ID
								TRUE, 								// Receive messages
								FALSE,								// Smart
								FALSE,								// Clean
								0,									// One open Instance ID
								(GREY_WHEN_NO_CURRENT_DOC | GREY_WHEN_NO_SELECTION)
 								)); 

}               


/********************************************************************************************

>	OpState	OpSmoothSelection::GetState(String_256*, OpDescriptor*)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/10/94
	Inputs:		-
	Outputs:	-
	Returns:	The state of the OpSmoothSelection
	Purpose:	For finding the OpSmoothSelection state. 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

OpState	OpSmoothSelection::GetState(String_256* UIDescription, OpDescriptor*)
{
	OpState OpSt;
	String_256 DisableReason; 

   	OpSt.Greyed = FALSE;
	BOOL FoundSelected = FALSE;

	// Go through the selection until we find a selected point

	SelRange* Selected = GetApplication()->FindSelection();
	Node* pNode = Selected->FindFirst();

	while (pNode)
	{
		if (pNode->GetRuntimeClass() == CC_RUNTIME_CLASS(NodePath))
		{
			if (((NodePath*)pNode)->InkPath.NumSplinters() >0 )
			{
				FoundSelected = TRUE;
				break;
			}

		}
		pNode = Selected->FindNext(pNode);
	}

	// The operation is disabled if there are no complex paths selected

	if (!FoundSelected)
	{
		OpSt.Greyed = TRUE;
		DisableReason = String_256(_R(IDS_NEEDS_SELECTED_POINT));
		*UIDescription = DisableReason;
	}

	return(OpSt);   
}



/********************************************************************************************

>	void OpSmoothSelection::GetOpName(String_256* OpName)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/10/94
	Inputs:		-
	Outputs:	The undo string for the operation
	Returns:	
	Purpose:	The GetOpName fn is overridden so that we return back a description
				appropriate to the type of attribute that the operation applies.
					
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void OpSmoothSelection::GetOpName(String_256* OpName)
{
	*OpName = String_256(_R(IDS_UNDO_SMOOTHSELECTIONOP));
}






/********************************************************************************************

> void OpSmoothSelection::Do(OpDescriptor* opDesc)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>		
	Created:	20/10/94
	Inputs:		pointer to an OpDescriptor (unused)
	Outputs:	-
	Returns:	-
	Purpose:	This operation will smooth all selected paths within the current document
				Before smoothing any complete path, the path is searched for selected points
				The action of the smooth given a number of selected points is as follows
				No sel points:	Smooth the whole path
				One sel point:	Smooth the whole path.
								Could smooth from the start of the path to this point but
								the user does not generally known which part is the start
								of the path.
				Two sel points:	Smooth the section of path lying between these points.
				Even number:	Smooth using inside/outside section processing.
								(a,b) (c,d) .... (y,z) etc are regions for smoothing
				Odd number:		Smooth as for even number apart from last region
								(a,b) (c,d)	........ (f,g,h)
	Errors:		failandexecute will be called if the operation fails in some way, most
				likely when no memory is available. 

********************************************************************************************/

void OpSmoothSelection::Do(OpDescriptor*)
{
	// Obtain the current selections 
	SelRange* Selected = GetApplication()->FindSelection();
	NodePath* pSmoothNode;

	// Now, because we're going to be doing mad things to the selection, we have to make a list
	// of all the selected nodes, so that adding nodes into the tree won't confuse us

	List* NodeList = Selected->MakeListOfNodes();
	NodeListItem* CurItem = (NodeListItem*)(NodeList->GetHead());

	if (!CurItem)
		goto FailAndDeleteList;

	// remember the selection before the operation
	if (!DoStartSelOp(FALSE,FALSE))
		goto FailAndDeleteList;

	while (CurItem)
	{
		if (CurItem->pNode->GetRuntimeClass() == CC_RUNTIME_CLASS(NodePath))
		{
			// get a pointer to the NodePath
			NodePath* pThisNode = (NodePath*)(CurItem->pNode);

			BOOL ok;
			Node* pnode;

			// Copy the nodepath and all its children, without placing the copy in the tree
			CALL_WITH_FAIL(pThisNode->NodeCopy(&pnode), this, ok);
			if (!ok) goto DeleteList;
			pSmoothNode = (NodePath*)pnode;

			// ok, smooth the nodepaths path data
			double error = SMOOTH_CONSTANT;
			CALL_WITH_FAIL(pSmoothNode->InkPath.SmoothRegions(error, FALSE, TRUE), this, ok);
			if (!ok) goto DeleteListAndPath;

			// update the smoothed paths bounding rectangle
			pSmoothNode->InvalidateBoundingRect();
			pSmoothNode->InkPath.ClearSubSelection();

			// Now stick the new path into the tree
			CALL_WITH_FAIL(DoInsertNewNode(pSmoothNode, pThisNode, NEXT, TRUE), this, ok);
			if (!ok) goto DeleteListAndPath;
			
			// Now we've formed a smoothed path, let's hide the original
			CALL_WITH_FAIL(DoHideNode(pThisNode,TRUE), this, ok);
			if (!ok) goto DeleteList;

		}
		CurItem = (NodeListItem*)(NodeList->GetNext(CurItem));
	}

 	End();

	// delete the nodelist (and all the list items)
	while (!NodeList->IsEmpty())
		delete (NodeListItem*)(NodeList->RemoveHead());
	delete NodeList;
	return;


FailAndDeleteList:
	while (!NodeList->IsEmpty())
		delete (NodeListItem*)(NodeList->RemoveHead());
	delete NodeList;
	FailAndExecute();
	End();
	return;

DeleteListAndPath:
	pSmoothNode->CascadeDelete();
	delete pSmoothNode;
DeleteList:
	while (!NodeList->IsEmpty())
		delete (NodeListItem*)(NodeList->RemoveHead());
	delete NodeList;
	End();
	return;

}
