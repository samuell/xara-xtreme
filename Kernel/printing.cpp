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

// Code to handle printing in a kernely sort of way
// (For the moment, this is just to do menu code interaction)

#include "camtypes.h"
//	WEBSTER-ranbirr-13/11/96
#ifndef WEBSTER
#include "printing.h"
//#include "ensure.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "errors.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "mario.h"
#include "menucmds.h"
#include "helpids.h"
#include "bubbleid.h"
#include "prncamvw.h"
//#include "barsdlgs.h"
//#include "nev.h"		// _R(IDS_FILE_PRINT_ONE)

CC_IMPLEMENT_DYNCREATE(OpPrint, Operation)

/********************************************************************************************

>	OpPrint::OpPrint()

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/11/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Constructs a new OpPrint object: setting default operation flags, and 
				adding it to the Live list. 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
     
OpPrint::OpPrint(): Operation()
{                 
}



/********************************************************************************************

>	OpState OpPrint::GetState(String_256*, OpDescriptor*)

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/7/93
	Inputs:		-
	Outputs:	-
	Returns:	Not ticked and not greyed
	Purpose:	Menu state for Print item
	Errors:		-

********************************************************************************************/

OpState OpPrint::GetState(String_256*, OpDescriptor*)
{
	// not ticked, greyed only if already printing (or the print dialog is open).
	return OpState( FALSE, PrintMonitor::IsPrintingActive() );
}


/********************************************************************************************

>	void OpPrint::Do(OpDescriptor* WhichOp)

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/7/93
	Inputs:		A pointer to OpDescriptor - allows selection of appropriate print routine.
	Outputs:	-
	Returns:	-
	Purpose:	Actually do appropriate print function. Print Functions available include
				Print, Print setup and Print preview.
	Errors:		-

********************************************************************************************/

void OpPrint::Do(OpDescriptor* WhichOp)
{
	if ((WhichOp->Token) == (String(OPTOKEN_PRINT)))
		PrintAction();
	else if  ((WhichOp->Token) == (String(OPTOKEN_PRINT_SETUP)))
		PrintSetupAction();
	else if  ((WhichOp->Token) == (String(OPTOKEN_PRINT_PREVIEW)))
		PrintPreviewAction();
	
	End();
}


/********************************************************************************************

>	BOOL OpPrint::Init()

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/7/93
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if successully created all necessary opdescriptors.
	Purpose:	Initialises all the print operation OpDescriptors..
	Errors:		-

********************************************************************************************/

BOOL OpPrint::Init()
{
	BOOL InitOK = TRUE;

	InitOK = RegisterOpDescriptor(
									0, 							// Tool ID
									_R(IDS_FILE_PRINT),				// String resource ID
									CC_RUNTIME_CLASS(OpPrint), 	// Runtime class
									OPTOKEN_PRINT,				// Token string
									OpPrint::GetState,			// GetState function
									HID_FILE_PRINT,				// help ID
									_R(IDBBL_PRINTOP),				// bubble help
									_R(IDD_BARCONTROLSTORE),		// resource ID
									_R(IDC_BTN_FILEPRINT),			// control ID
									SYSTEMBAR_FILE,				// Bar ID
									TRUE,						// Recieve system messages
									FALSE,						// Smart duplicate operation
									TRUE,						// Clean operation
									0,							// No vertical counterpart
									_R(IDS_FILE_PRINT_ONE),			// String for one copy only error
									(DONT_GREY_WHEN_SELECT_INSIDE | GREY_WHEN_NO_CURRENT_DOC) // Auto state flags
 								);
	ERRORIF(!InitOK, _R(IDE_NOMORE_MEMORY), FALSE);
 

	InitOK = RegisterOpDescriptor(
									0, 							// Tool ID
									_R(IDS_FILE_PRINT_SETUP),		// String resource ID
									CC_RUNTIME_CLASS(OpPrint), 	// Runtime class
									OPTOKEN_PRINT_SETUP,		// Token string
									OpPrint::GetState,			// GetState function
									HID_FILE_PRINT_SETUP,		// help ID
									_R(IDBBL_PSETUPOP),				// bubble help
									_R(IDD_BARCONTROLSTORE),		// resource ID
									_R(IDC_PRINTSETUP),				// control ID
									SYSTEMBAR_FILE,				// Bar ID
									TRUE,						// Recieve system messages
									FALSE,						// Smart duplicate operation
									TRUE,						// Clean operation
									0,							// No vertical counterpart
									0,							// String for one copy only error
									(DONT_GREY_WHEN_SELECT_INSIDE | GREY_WHEN_NO_CURRENT_DOC) // Auto state flags
 								);
	ERRORIF(!InitOK, _R(IDE_NOMORE_MEMORY), FALSE);

	OpDescriptor* PPreviewOp = new OpDescriptor(
												0, 
												_R(IDS_FILE_PRINT_PREVIEW),
												CC_RUNTIME_CLASS(OpPrint), 
												OPTOKEN_PRINT_PREVIEW,
												OpPrint::GetState,
												HID_FILE_PRINT_PREVIEW,
												_R(IDBBL_PPREVIEWOP),
												0	/* bitmap ID */);

	ERRORIF(!PPreviewOp, _R(IDE_NOMORE_MEMORY), FALSE);

	return TRUE;
}

#endif //webster
