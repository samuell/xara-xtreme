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
// The Rectangle Tool
// Created by Mario Shamtani on 13/9/93

/*
*/


#include "camtypes.h"
#include "rectangl.h"

#include "oilfiles.h"
#include "oprshape.h"
//#include "mario.h"
//#include "markn.h"
//#include "viewrc.h"

//#include "will2.h"

DECLARE_SOURCE( "$Revision$" );

// These are still char* while we wait for resource technology to be developed for modules
TCHAR *		RectangleTool::FamilyName = _T("Shape Tools");
TCHAR * 		RectangleTool::ToolName = _T("Rectangle Tool");
TCHAR *		RectangleTool::Purpose = _T("Manipulating rectangles");
TCHAR *		RectangleTool::Author = _T("Peter Arnold");


CC_IMPLEMENT_MEMDUMP( RectangleTool, QuickShapeBase )

/********************************************************************************************

>	RectangleTool::RectangleTool()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/03/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Constructor for the QuickShape tool
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
RectangleTool::RectangleTool() : QuickShapeBase()
{
	CreationMode = OpNewRegShape::BOUNDS;
}



/********************************************************************************************

>	RectangleTool::~RectangleTool()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/03/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	QuickShapes tools destructor
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
RectangleTool::~RectangleTool()
{

}



/********************************************************************************************

>	void RectangleTool::Describe(void *InfoPtr)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/11/94
	Inputs:		InfoPtr - 	A pointer to a tool info block.
	Outputs:	InfoPtr - 	The structure pointed to by InfoPtr will have had all the info
							that this version of the Tool knows about
	Purpose:	Allows the tool manager to extract information about the tool

********************************************************************************************/

void RectangleTool::Describe(void *InfoPtr)
{
	// Cast structure into the latest one we understand.
	ToolInfo_v1 *Info = (ToolInfo_v1 *) InfoPtr;

	Info->InfoVersion = 1;
	
	Info->InterfaceVersion = GetToolInterfaceVersion();
		
	Info->Version = 1;
	Info->ID      = GetID();
	Info->TextID  = _R(IDS_RECTANGLE_TOOL);

	Info->Family  = FamilyName;
	Info->Name    = ToolName;
	Info->Purpose = Purpose;
	Info->Author  = Author;

	Info->BubbleID = _R(IDBBL_RECT_TOOLBOX);
}



/********************************************************************************************

>	BOOL RectangleTool::Init()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/03/95
	Inputs:		-
	Outputs:	-
	Returns:	TRUE/FALSE for success/failure
	Purpose:	Called to initialise the ellipse tool.
	Errors:		-
	SeeAlso:	QuickShapeBase::Init

********************************************************************************************/
BOOL RectangleTool::Init()
{
	pQuickShapeBaseInfoBarOp = new QuickShapeBaseInfoBarOp(this, _R(IDD_RECTTOOLBAR));
	return pQuickShapeBaseInfoBarOp != NULL;

PORTNOTE("dialog", "Removed Bar reading")
#if 0
	BOOL ok = TRUE;

	CCResTextFile file;									// Resource File
	QuickShapeBaseInfoBarOpCreate BarCreate;			// Object that creates QuickShapeBaseInfoBarOp objects

	 		ok = file.open(_R(IDM_RECT_BAR), _R(IDT_INFO_BAR_RES));	// Open resource
	if (ok) ok = DialogBarOp::ReadBarsFromFile(file,BarCreate);	// Read and create info bar
	if (ok) file.close();									 	// Close resource

	ERROR2IF(!ok,FALSE,"Unable to load RectTool.ini from resource"); 

	// Info bar now exists.  Now get a pointer to it
	String_32 str = String_32(_R(IDS_RECTTOOL_INFOBARNAME));
	DialogBarOp* pDialogBarOp = DialogBarOp::FindDialogBarOp(str);
	
	ERROR2IF(pDialogBarOp==NULL, FALSE, "Rectangle infobar not found\n");

	ok = pDialogBarOp->IsKindOf(CC_RUNTIME_CLASS(QuickShapeBaseInfoBarOp));
	if (ok)
	{
		pQuickShapeBaseInfoBarOp = (QuickShapeBaseInfoBarOp*)pDialogBarOp;
		pQuickShapeBaseInfoBarOp->pQuickShapeBase = this;
	}

	ERROR2IF(!ok,FALSE,"Error finding the Rectangle tool info bar");

	return (ok);
#endif
}



/********************************************************************************************

>	virtual BOOL RectangleTool::IsInterestingShape(NodeRegularShape* pShape)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/03/95
	Inputs:		pShape - points to a regular shape in the document
	Outputs:	-
	Returns:	TRUE if the shapes IsCircular flag is set
	Purpose:	For saying that the ellipse tool only processed elipitical shapes.

********************************************************************************************/
BOOL RectangleTool::IsInterestingShape(NodeRegularShape* pShape)
{
	return (pShape->IsARectangle());
}


