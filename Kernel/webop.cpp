// $Id: webop.cpp 662 2006-03-14 21:31:49Z alex $
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

// Operations to save in the version 2 Native/Web format.
// Loading ops are not required as the general open operation will open any format file.

/*
*/

#include "camtypes.h"

#include "webop.h"
#include "nativeop.h"
#include "filters.h"
#include "oilfltrs.h"
//#include "filtrres.h"

//#include "barsdlgs.h"

// An implement to match the Declare in the .h file.
CC_IMPLEMENT_DYNCREATE(OpSaveAsWeb, Operation)
CC_IMPLEMENT_DYNCREATE(OpSaveAsNativeV2, Operation)

// This will get Camelot to display the filename and linenumber of any memory allocations
// that are not released at program exit
#define new CAM_DEBUG_NEW

/********************************************************************************************

>	virtual UINT32 OpSaveAsWeb::GetSearchFilterId()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/7/96
	Inputs:		-
	Returns:	The id of the filter to search for.
	Purpose:	To allow different ops to just specify different filters and reuse
				the same operation code to save out the file in different file
				formats.

********************************************************************************************/

UINT32 OpSaveAsWeb::GetSearchFilterId()
{
	return FILTERID_WEB;
}

/********************************************************************************************

>	virtual BOOL OpSaveAsWeb::FixFileType()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/8/96
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if fixing is required, FALSE otherwise
	Purpose:	To determine whether this operation requires the file type or extension on
				the specified file path to be correct and match the	filter we are about to use.
				This baseclass version says no as the old native save operation did not do
				it.

********************************************************************************************/

BOOL OpSaveAsWeb::FixFileType()
{
	// Say we do not want it in this baseclass version
	return TRUE;
}

/********************************************************************************************

>	BOOL OpSaveAsWeb::Init()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/7/96
	Returns:	TRUE if it worked, FALSE if not
	Purpose:	Registers the OpDescriptor for the Save Operation.

********************************************************************************************/

BOOL OpSaveAsWeb::Init()
{
	BOOL InitOK = TRUE;

	// Register the save as web op desciptor
	InitOK = RegisterOpDescriptor(
									0, 							// Tool ID
									_R(IDS_SAVEASWEB),				// String resource ID
									CC_RUNTIME_CLASS(OpSaveAsWeb), 	// Runtime class
									OPTOKEN_SAVEASWEB,			// Token string
									GetState,					// GetState function
									0,							// help ID
									_R(IDBBL_SAVEASWEB),			// bubble help
									0, //_R(IDD_BARCONTROLSTORE),		// resource ID
									0, //_R(IDC_FILESAVETEMPLATE),		// control ID
									SYSTEMBAR_ILLEGAL,			// Bar ID
									TRUE,						// Recieve system messages
									FALSE,						// Smart duplicate operation
									TRUE,						// Clean operation
									0,							// No vertical counterpart
									0,							// String for one copy only error
									(DONT_GREY_WHEN_SELECT_INSIDE | GREY_WHEN_NO_CURRENT_DOC) // Auto state flags
 								);
	if (!InitOK) return FALSE; 

	// All worked
	return TRUE;
}

/********************************************************************************************

>	OpState OpSaveAsWeb::GetState(String_256*, OpDescriptor*)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/7/96
	Purpose:	The OpState function

********************************************************************************************/

OpState OpSaveAsWeb::GetState(String_256*, OpDescriptor*)
{
	OpState Blobby;
	return Blobby;
}



/********************************************************************************************
/ ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++/
********************************************************************************************/


/********************************************************************************************

>	virtual UINT32 OpSaveAsNativeV2::GetSearchFilterId()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/7/96
	Inputs:		-
	Returns:	The id of the filter to search for.
	Purpose:	To allow different ops to just specify different filters and reuse
				the same operation code to save out the file in different file
				formats.

********************************************************************************************/

UINT32 OpSaveAsNativeV2::GetSearchFilterId()
{
	return FILTERID_NATIVE;
}

/********************************************************************************************

>	virtual BOOL OpSaveAsNativeV2::FixFileType()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/8/96
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if fixing is required, FALSE otherwise
	Purpose:	To determine whether this operation requires the file type or extension on
				the specified file path to be correct and match the	filter we are about to use.
				This baseclass version says no as the old native save operation did not do
				it.

********************************************************************************************/

BOOL OpSaveAsNativeV2::FixFileType()
{
	// Say we do not want it in this baseclass version
	return TRUE;
}

/********************************************************************************************

>	BOOL OpSaveAsNativeV2::Init()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/7/96
	Returns:	TRUE if it worked, FALSE if not
	Purpose:	Registers the OpDescriptor for the Save Operation.

********************************************************************************************/

BOOL OpSaveAsNativeV2::Init()
{
	BOOL InitOK = TRUE;

	// Register the save as web op desciptor
	InitOK = RegisterOpDescriptor(
									0, 							// Tool ID
									_R(IDS_SAVEASNATIVEV2),			// String resource ID
									CC_RUNTIME_CLASS(OpSaveAsNativeV2), 	// Runtime class
									OPTOKEN_SAVEASNATIVEV2,		// Token string
									GetState,					// GetState function
									0,							// help ID
									_R(IDBBL_SAVEASNATIVEV2),		// bubble help
									0, //_R(IDD_BARCONTROLSTORE),		// resource ID
									0, //_R(IDC_FILESAVETEMPLATE),		// control ID
									SYSTEMBAR_ILLEGAL,			// Bar ID
									TRUE,						// Recieve system messages
									FALSE,						// Smart duplicate operation
									TRUE,						// Clean operation
									0,							// No vertical counterpart
									0,							// String for one copy only error
									(DONT_GREY_WHEN_SELECT_INSIDE | GREY_WHEN_NO_CURRENT_DOC) // Auto state flags
 								);
	if (!InitOK) return FALSE; 

	// All worked
	return TRUE;
}

/********************************************************************************************

>	OpState OpSaveAsNativeV2::GetState(String_256*, OpDescriptor*)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/7/96
	Purpose:	The OpState function

********************************************************************************************/

OpState OpSaveAsNativeV2::GetState(String_256*, OpDescriptor*)
{
	OpState Blobby;
	return Blobby;
}


