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
//*/

#include "camtypes.h"
#include "mkshapes.h" 

// code headers
//#include "app.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "document.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "nodepath.h"
#include "objchge.h"

//#include "becomea.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "group.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "nodeblnd.h"
#include "bubbleid.h"

// resource headers
//#include "simon.h"
//#include "extendres.h"
      
DECLARE_SOURCE("$Revision$");  

CC_IMPLEMENT_DYNCREATE(OpMakeShapes, SelOperation)  
CC_IMPLEMENT_DYNCREATE(OpMakeNodesShapes, UndoableOperation);

// ------------------------------------------------------------------------------------------
// OpMakeShapes methods
            
/********************************************************************************************
>	OpMakeShapes::OpMakeShapes() 

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/04/94
	Purpose:	OpMakeShapes constructor
********************************************************************************************/
OpMakeShapes::OpMakeShapes(): SelOperation()								
{                              
}



/********************************************************************************************
>	BOOL OpMakeShapes::Init()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/04/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the operation could be successfully initialised 
				FALSE if no more memory could be allocated 
				
	Purpose:	OpMakeShapes initialiser method
	Errors:		ERROR will be called if there was insufficient memory to allocate the 
				operation.
	SeeAlso:	-
********************************************************************************************/
BOOL OpMakeShapes::Init()
{
	return (RegisterOpDescriptor(0,							// tool id
	 							_R(IDS_MAKESHAPESOP),			// Ops name
								CC_RUNTIME_CLASS(OpMakeShapes),	// Ops class
	 							OPTOKEN_MAKE_SHAPES,		// Op token
	 							OpMakeShapes::GetState,		// Get state function
	 							0,							// help ID
	 							_R(IDBBL_MAKESHAPES),			// Bubble help
	 							0,							// Bitmap ID
								0,							// Control ID
								SYSTEMBAR_ILLEGAL,			// Bar to appear on
								TRUE,						// Recieve messages
								FALSE,						// Smart
								FALSE,						// Clean
								0,							// One open string ID
	GREY_WHEN_NO_CURRENT_DOC | GREY_WHEN_NO_SELECTION | DONT_GREY_WHEN_SELECT_INSIDE));
}



/********************************************************************************************
>	OpState	OpMakeShapes::GetState(String_256*, OpDescriptor*)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/04/94
	Inputs:		-
	Outputs:	-
	Returns:	The state of the OpMakeShapes
	Purpose:	For finding the OpMakeShapes's state. 
********************************************************************************************/
OpState	OpMakeShapes::GetState(String_256* UIDescription, OpDescriptor*)
{
	OpState OpSt;

	SelRange Sel(*(GetApplication()->FindSelection())); 
	RangeControl rg = Sel.GetRangeControlFlags();
	rg.PromoteToParent = TRUE;
	Sel.Range::SetRangeControl(rg);


	// Set up the ObjChangeParam so we can ask the selected nodes if they mind being deleted
	ObjChangeFlags cFlags(TRUE);
	ObjChangeParam ObjChange(OBJCHANGE_STARTING,cFlags,NULL,NULL);

	// Will one or more selected nodes allow this op?
	if (!Sel.AllowOp(&ObjChange,FALSE))
		OpSt.Greyed = TRUE;

	return(OpSt);   
}



/********************************************************************************************
>	void OpMakeShapes::Do(OpDescriptor*)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/04/94
	Inputs:		OpDescriptor (unused)
	Outputs:	-
	Returns:	-
	Purpose:	Performs the MakeShapes operation. 
********************************************************************************************/
void OpMakeShapes::Do(OpDescriptor*)
{   
	// Obtain the current selections 
	Range Selection(*GetApplication()->FindSelection());
	RangeControl rg = Selection.GetRangeControlFlags();
	rg.PromoteToParent = TRUE;
	Selection.Range::SetRangeControl(rg);

	// change the selection flags

	Node* CurrentNode = Selection.FindFirst(); 
	BOOL Success = TRUE;		
	
	ERROR3IF(CurrentNode == NULL, "Make shapes called with no nodes selected"); 
	
	if (CurrentNode != NULL) // No nodes selected so End
	{   
		// We need to invalidate the region
		if (Success)
			Success = DoInvalidateNodesRegions(Selection, TRUE, FALSE, FALSE, FALSE);	// Don't recache
                    
		// Try to record the selection state, don't render the blobs though 
		if (Success)
			Success = DoStartSelOp(FALSE,FALSE);								   

		// The DoMakeShapes function does all the work
		while (Success && CurrentNode!=NULL)
		{
			Node* Next = Selection.FindNext(CurrentNode);

// BODGE - since the group is selected and won't be replaced by anything else, no need to reselect it
// this fixes a bug where make shapes on grouped text stories/molds/blends don't leave the parent group
// selected but selects the objects inside!
			BOOL reselect = !IS_A(CurrentNode,NodeGroup) && !IS_A(CurrentNode,NodeBlend);
//			BOOL ok = TRUE;
		

			ObjChangeFlags cFlags(TRUE);
			ObjChangeParam ObjChange(OBJCHANGE_STARTING,cFlags,NULL,NULL);
			if (CurrentNode->AllowOp(&ObjChange))
			{
				BecomeA BecomeAPath(BECOMEA_REPLACE,CC_RUNTIME_CLASS(NodePath), this, reselect);
				BecomeAPath.SetResultsStayInPlace(TRUE);
				Success = CurrentNode->DoBecomeA(&BecomeAPath);
			}
	
			CurrentNode = Next;
		}

		// We need to invalidate the region again for the new positions
		GetApplication()->FindSelection()->Update();
		if (Success)
			Success = DoInvalidateNodesRegions(*GetApplication()->FindSelection(), TRUE);
	}                   

	if (!Success)
	{
		InformError();
		FailAndExecute();
	}

 	End(); 
}			



//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------



/********************************************************************************************

>	void OpMakeNodesShapes::DoWithParam(OpDescriptor* pOp, OpParam* pParam)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/11/1999
	Inputs:		pOp			unused pointer to an OpDescriptor.
				pParam		pointer to an OpParamMakeNodesShapes data information structure.

	Outputs:	The list of nodes, passed in through the OpParam, is converted in place
				into paths.
	Purpose:	This undoable operation converts a given list of nodes to editable shapes
				(paths), each new node retaining the selection status of the original.
	See also:	OpParamMakeNodesShapes

********************************************************************************************/
void OpMakeNodesShapes::DoWithParam(OpDescriptor* pOp, OpParam* pParam)
{
	std::list<Node*>* plpNodes = ((OpParamMakeNodesShapes*)pParam)->m_plpNodes;

	Node* pNode;
	BOOL bFirst = FALSE;
	BOOL ok = TRUE;
	for (	std::list<Node*>::iterator iterNode = plpNodes->begin();
			iterNode != plpNodes->end() && ok;
			iterNode++ )
	{
		pNode = *iterNode;
		BecomeA baInfo(BECOMEA_REPLACE, CC_RUNTIME_CLASS(NodePath), this,
											pNode->IsSelected(), bFirst);
		baInfo.SetResultsStayInPlace(TRUE);
		ok = pNode->DoBecomeA(&baInfo);

#ifdef _DEBUG
		if (ok)
			TRACEUSER( "Karim", _T("converted %s to editable shapes\n"), pNode->Name());
#endif

		bFirst = TRUE;
	}

	if (!ok)
	{
		InformError();
		FailAndExecute();
	}

	End();
	return;
}



/********************************************************************************************

>	BOOL OpMakeNodesShapes::Init()

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/11/1999
	Returns:	TRUE if successful,
				FALSE otherwise.
	Purpose:	Initialise this operation.

********************************************************************************************/
BOOL OpMakeNodesShapes::Init()
{
	return (RegisterOpDescriptor(0,				// Tool ID
 				_R(IDS_MAKENODESSHAPES),	 		// string resource ID for Op description
				CC_RUNTIME_CLASS(OpMakeNodesShapes),// Op's Runtime class
 				OPTOKEN_MAKE_NODES_SHAPES,		// Op's OPTOKEN string
 				NULL,							// Op's GetState function
				0,								// associated Help system ID
				_R(IDBBL_MAKENODESSHAPES),			// string resource ID for bubble help
				0,								// bitmap ("icon") or resource ID
				0,								// associated gadget control ID
				SYSTEMBAR_ILLEGAL,				// associated info-bar ID
				FALSE,							// receive system messages
				FALSE,							// Op does smart attr optimisation
				FALSE,							// Op doesn't modify document
				0,								// when non-0, only one instance of the
												// Op is permitted, and this is the ID
												// of a string resource explaining why.
				(DONT_GREY_WHEN_SELECT_INSIDE | GREY_WHEN_NO_CURRENT_DOC)
				));								// flags indicating conditions when the Op
												// should automatically be greyed/ticked etc.
}
