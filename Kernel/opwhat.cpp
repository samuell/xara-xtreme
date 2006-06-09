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

#include "camtypes.h"
#include "opwhat.h"
//#include "opdesc.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "ollie.h"
// #include "helpmgr.h"

// #include <winuser.h>

DECLARE_SOURCE("$Revision$");  

CC_IMPLEMENT_DYNCREATE(OpWhatsThis, Operation)  

#define new CAM_DEBUG_NEW

     
// ------------------------------------------------------------------------------------------
// OpWhatsThis methods
    
/********************************************************************************************

>	OpWhatsThis::OpWhatsThis() 

	Author:		Ollie_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/11/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	OpWhatsThis constructor
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
            
            
OpWhatsThis::OpWhatsThis(): Operation()								
{                              
}

 /********************************************************************************************

>	BOOL OpWhatsThis::Init()

	Author:		Ollie_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/11/95
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the operation could be successfully initialised 
				FALSE if no more memory could be allocated 
				
	Purpose:	OpWhatsThis initialiser method
	Errors:		ERROR will be called if there was insufficient memory to allocate the 
				operation.
	SeeAlso:	-

********************************************************************************************/

BOOL OpWhatsThis::Init()
{
	return (RegisterOpDescriptor(0,										// Module (Tool) ID
	 							_R(IDS_WHATSTHISOP),						// String resource ID
								CC_RUNTIME_CLASS(OpWhatsThis),			// Ops runtime class
	 							OPTOKEN_WHATSTHIS,						// Optoken ID
	 							OpWhatsThis::GetState,					// Pointer to the GetState function
	 							0,	/* help ID */						// help identifier
	 							_R(IDBBL_WHATSTHIS),						// string resource for bubble help
	 							0,										// resource ID
	 							0,										// control ID
								SYSTEMBAR_ILLEGAL,						// group bar ID
								TRUE, 									// Receive messages
								FALSE,
								FALSE,
								0,
								0
								 )); 
}               

/********************************************************************************************

>	OpState	OpWhatsThis::GetState(String_256*, OpDescriptor*)

	Author:		Ollie_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/11/95
	Inputs:		-
	Outputs:	-
	Returns:	The state of the OpWhatsThis
	Purpose:	For finding the OpWhatsThis' state.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

OpState	OpWhatsThis::GetState(String_256* UIDescription, OpDescriptor*)
{
	OpState OpSt;
		
	OpSt.Greyed = FALSE;

	return(OpSt);  				  // isn't this returning a local variable????????
}

/********************************************************************************************

>	void OpWhatsThis::Do(OpDescriptor*)

	Author:		Ollie_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/11/95
	Inputs:		OpDescriptor (unused)
	Outputs:	-
	Returns:	-
	Purpose:	Puts Camelot into the "What's This?" help mode.
	Errors:		dunno yet 
	SeeAlso:	-

********************************************************************************************/

void OpWhatsThis::Do(OpDescriptor*)
{
	MessageBox(NULL, "Not implemented","Message", MB_OK );

	// Selecting this op drops the program into a mode where clicking on anything in
	// the program elicits help on it. The pointer should change when the program
	// is in this mode.
	
#ifdef _DEBUG
	TRACEUSER( "Ollie", _T("What's This Operation -> Do it!\n"));
#endif

	// Send a WM_HELP to a greyed button.
	// ?????
	
	// Drop into the context sensitive help mode.
	// BOOL Succeeded = HelpManager::EnterHelpMode();
	
	End();
}  
     
