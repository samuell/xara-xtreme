// $Id: mkstroke.cpp 662 2006-03-14 21:31:49Z alex $
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
#include "mkstroke.h" 

// code headers
#include "app.h"
#include "document.h"
#include "nodepath.h"
#include "objchge.h"
#include "mkshapes.h" 

#include "becomea.h"
#include "group.h"
#include "nodeblnd.h"

#include "progress.h"
#include "groupops.h"
#include "pathstrk.h"
#include "sgline.h"

// resource headers
#include "bubbleid.h"
//#include "richard3.h"

      
DECLARE_SOURCE("$Revision: 662 $");  

CC_IMPLEMENT_DYNCREATE(OpMakeStroke, SelOperation)  

// ------------------------------------------------------------------------------------------
// OpMakeStroke methods
            
/********************************************************************************************
>	OpMakeStroke::OpMakeStroke() 

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	04/03/97
	Purpose:	OpMakeStroke constructor
********************************************************************************************/
OpMakeStroke::OpMakeStroke(): SelOperation()								
{                              
}



/********************************************************************************************
>	BOOL OpMakeStroke::Init()

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	04/03/97
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the operation could be successfully initialised 
				FALSE if no more memory could be allocated 
				
	Purpose:	OpMakeStroke initialiser method
	Errors:		ERROR will be called if there was insufficient memory to allocate the 
				operation.
	SeeAlso:	-
********************************************************************************************/
BOOL OpMakeStroke::Init()
{
	return (RegisterOpDescriptor(0,							// tool id
	 							_R(IDS_MAKESTROKEOP),			// Ops name
								CC_RUNTIME_CLASS(OpMakeStroke),	// Ops class
	 							OPTOKEN_MAKE_STROKE,		// Op token
	 							OpMakeStroke::GetState,		// Get state function
	 							0,							// help ID
	 							_R(IDBBL_MAKESTROKE),			// Bubble help
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
>	OpState	OpMakeStroke::GetState(String_256*, OpDescriptor*)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	04/03/97
	Inputs:		-
	Outputs:	-
	Returns:	The state of the OpMakeStroke
	Purpose:	For finding the OpMakeStroke's state. 
********************************************************************************************/
OpState	OpMakeStroke::GetState(String_256* UIDescription, OpDescriptor*)
{
	OpState OpSt;

	SelRange *pSelRange = GetApplication()->FindSelection(); 

	// Set up the ObjChangeParam so we can ask the selected nodes if they mind being deleted
	ObjChangeFlags cFlags(TRUE);
	ObjChangeParam ObjChange(OBJCHANGE_STARTING,cFlags,NULL,NULL);

	// Will one or more selected nodes allow this op?
	if (!pSelRange->AllowOp(&ObjChange,FALSE))
		OpSt.Greyed = TRUE;

	return(OpSt);   
}



/********************************************************************************************
>	void OpMakeStroke::Do(OpDescriptor*)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	04/03/97
	Inputs:		OpDescriptor (unused)
	Outputs:	-
	Returns:	-
	Purpose:	Performs the MakeShapes operation. 
********************************************************************************************/
void OpMakeStroke::Do(OpDescriptor*)
{   
	// Obtain the current selections 
	Range Selection = *GetApplication()->FindSelection();
	Node* CurrentNode = Selection.FindFirst(); 
	BOOL Success = TRUE;		
	
	ERROR3IF(CurrentNode == NULL, "Make shapes called with no nodes selected"); 
	
	if (CurrentNode != NULL) // No nodes selected so End
	{                    
		// Try to record the selection state, don't render the blobs though 
		if (Success)
			Success = DoStartSelOp(FALSE,FALSE);								   

		// First, Make Shapes on everything so they're all simple paths
		String_256 Desc("Building new stroke brush...");
		Progress::Start(FALSE, &Desc);
		OpDescriptor *pOp = OpDescriptor::FindOpDescriptor(OPTOKEN_MAKE_SHAPES);
		if (pOp != NULL)
			pOp->Invoke();

		// Second, Group everything
		pOp = OpDescriptor::FindOpDescriptor(OPTOKEN_GROUP);
		if (pOp != NULL)
			pOp->Invoke();

		pOp = OpDescriptor::FindOpDescriptor(OPTOKEN_GROUP);
		if (pOp != NULL)
			pOp->Invoke();

		// Finally, create a new brush
		PathStrokerVector::BodgeRipSelection(/*(CommandIndex == 0) ? FALSE :*/ TRUE);
		Progress::Stop();

	}                   

	if (!Success)
	{
		InformError();
		FailAndExecute();
	}

 	End(); 
}			

