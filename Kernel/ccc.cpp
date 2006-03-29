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
// Implementation file for the ConcurrencyController

#include "camtypes.h"
#include "ccc.h"
#include "gdrawasm.h"

// Place any IMPLEMENT type statements here

CC_IMPLEMENT_MEMDUMP(ConcurrencyController, CC_CLASS_MEMDUMP)
//CC_IMPLEMENT_MEMDUMP(CriticalSection, CC_CLASS_MEMDUMP)

// We want better memory tracking
#define new CAM_DEBUG_NEW

// Change this so TRACEUSERs work for you
#define ME	"Colin"

// Ooh Er! Static Members

ConcurrencyController::UserID ConcurrencyController::s_NextFreeUserID = 0;
ConcurrencyController::UserID ConcurrencyController::s_OneAndOnlyUser = 0;
UINT32 ConcurrencyController::s_NumberOfUsers = 0;

UINT32 ConcurrencyController::s_NumberOfActiveOperations = 0;
const Operation* ConcurrencyController::s_pTheActiveOperation = NULL;

// Functions follow



/********************************************************************************************

>	ConcurrencyController::ConcurrencyController() 

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/03/97

	Purpose:	Default constructor for the ConcurrencyController

********************************************************************************************/
ConcurrencyController::ConcurrencyController() 
{
}

BOOL ConcurrencyController::Init()
{
//	CriticalSection::Init();

	return TRUE;
}

/********************************************************************************************

>	ConcurrencyController::~ConcurrencyController() 

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/03/97

	Purpose:	Default destructor for the ConcurrencyController

********************************************************************************************/
ConcurrencyController::~ConcurrencyController() 
{
	if (s_NumberOfUsers > 1)
	{
		TRACEUSER( ME, _T("ConcurrencyController: Users still active\n"));
	}
//	CriticalSection::DeInit();
}


/********************************************************************************************

>	ConcurrencyController::RESULT ConcurrencyController::StartUser(UserID* pNewUserID)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/03/97

	Purpose:	Default destructor for the ConcurrencyController

********************************************************************************************/
ConcurrencyController::RESULT ConcurrencyController::StartUser(UserID* pNewUserID)
{
	RESULT Result = CCR_OK;

	CriticalSection UpdateUserID;
//	UpdateUserID.Start();

	if (s_NumberOfUsers == 0)
	{
		UserID NewUserID = GetNewUserID();
		if (NewUserID != NULL)
		{
			++s_NumberOfUsers;
		}
		else
		{
			Result = CCR_TOO_MANY_USERS;
		}
		*pNewUserID = NewUserID;
	}
	else
	{
		Result = CCR_TOO_MANY_USERS;
	}
//	UpdateUserID.End();

	return Result;
}

	
/********************************************************************************************

>	ConcurrencyController::RESULT ConcurrencyController::EndUser(UserID UserWhosDone)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/03/97

	Purpose:	Default destructor for the ConcurrencyController

********************************************************************************************/
ConcurrencyController::RESULT ConcurrencyController::EndUser(UserID UserWhosDone)
{
	RESULT Result = CCR_OK;

	CriticalSection UpdateUserID;
//	UpdateUserID.Start();

	if (UserWhosDone == s_OneAndOnlyUser)
	{
		s_OneAndOnlyUser = NULL;
		--s_NumberOfUsers;
	}
	else
	{
		Result = CCR_INVALID_USER;
	}

//	UpdateUserID.End();

	return Result;
}


/********************************************************************************************

>	ConcurrencyController::RESULT ConcurrencyController::PrepareForOperation(
															const Operation* const pOperation)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/03/97

	Purpose:	Allows or disallows access to Operations.

	Notes:		En ce moment only one op is allowed per application (static scope)

********************************************************************************************/
ConcurrencyController::RESULT ConcurrencyController::PrepareForOperation(
														const Operation* const pOperation)
{
	RESULT Result = CCR_OK;

	CriticalSection UpdateOpTable;
//	UpdateOpTable.Start();

	if (s_NumberOfActiveOperations == 0)
	{
		ERROR2IF(s_pTheActiveOperation != NULL, CCR_INTERNAL_ERROR, "ActiveOp not NULL");
		
		s_pTheActiveOperation = pOperation;
		++s_NumberOfActiveOperations;
	}
	else
	{
		Result = CCR_TOO_MANY_USERS;
	}
//	UpdateOpTable.End();

	return Result;
}


/********************************************************************************************

>	ConcurrencyController::RESULT ConcurrencyController::FinalizeOperation(
														const Operation* const pOperation)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/03/97

	Purpose:	Default destructor for the ConcurrencyController

********************************************************************************************/
ConcurrencyController::RESULT ConcurrencyController::FinalizeOperation(
														const Operation* const pOperation)
{
	RESULT Result = CCR_OK;

	CriticalSection UpdateOpTable;

	if (pOperation == s_pTheActiveOperation)
	{
		ERROR2IF(s_NumberOfActiveOperations != 1, CCR_INTERNAL_ERROR, "ActiveOps not Unity");

		s_pTheActiveOperation = NULL;
		--s_NumberOfActiveOperations;
	}
	else
	{
		Result = CCR_INVALID_ARGUMENT;
	}

	return Result;
}


/*********************************************************************************************
           
>	ConcurrencyController::RESULT ConcurrencyController::PrepareNodeForUpdate(Node* const pNode)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/02/97

	Inputs:		pNode:	The node to be updated
	Returns:	TRUE if the node can be updated
		FALSE otherwise
	Purpose:	Before updating a node, call this function to ensure the integrity of the 
		document tree.
		This call should be accompanied by a subsequent call to FinalizeNodeUpdate().
		The results of the update are undefined if not.
	See Also:	PrepareNodeForRead()
	
**********************************************************************************************/  
ConcurrencyController::RESULT ConcurrencyController::PrepareNodeForUpdate(Node* const pNode);


/*********************************************************************************************
           
>	ConcurrencyController::RESULT ConcurrencyController::FinalizeNodeUpdate(Node* const pNode)

	Author:	Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/02/97

	Inputs:	pNode:	The node passed in a prior call to PrepareNodeForUpdate()
	Returns:	TRUE if the node was successfully updated
		FALSE otherwise
	Purpose:	Once the given node has been updated in the tree call this function to ensure
		its update is complete.
		This call should be accompanied by a prior call to PrepareForNodeUpdate(). The
		results of the update are undefined if not.
	See Also:	FinalizeNodeRead()
	
**********************************************************************************************/  
ConcurrencyController::RESULT ConcurrencyController::FinalizeNodeUpdate(Node* const pNode);


/*********************************************************************************************
           
>	ConcurrencyController::RESULT ConcurrencyController::PrepareNodeForRead(Node* const pNode)

	Author:	Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/02/97

	Inputs:	pNode:	The node to be read
	Returns:	TRUE if the node can be read
		FALSE otherwise
	Purpose:	Before reading data in a node or passing it as a parameter to a function,
		call this function to ensure the integrity of the document tree.
		This call should be accompanied by a subsequent call to FinalizeNodeRead().
		The results of the read are undefined if not.
	See Also:	PrepareNodeForUpdate()
	
**********************************************************************************************/  
ConcurrencyController::RESULT ConcurrencyController::PrepareNodeForRead(Node* const pNode);


/*********************************************************************************************
           
>	ConcurrencyController::RESULT ConcurrencyController::FinalizeNodeRead(Node* const pNode)

	Author:	Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/02/97

	Inputs:	pNode:	The node that was passed in a previous call to PrepareNodeForRead()
	Returns:	TRUE if the node can be read or updated again.
		FALSE otherwise
	Purpose:	Once the given node has been read or passed as a parameter, call this function 
		to free it for updates.
		This call should be accompanied by a prior call to PrepareNodeForRead(). The
		results of the read are undefined if not.
	See Also:	FinalizeNodeUpdate()
	
**********************************************************************************************/  
ConcurrencyController::RESULT ConcurrencyController::FinalizeNodeRead(Node* const pNode);


/********************************************************************************************

>	UserID ConcurrencyController::GetNewUserID()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/03/97

	Purpose:	Support function to provide a new user id

	Notes:		Assumes thread-safe access to static members

********************************************************************************************/
ConcurrencyController::UserID ConcurrencyController::GetNewUserID()
{
	UserID NextID = ++s_NextFreeUserID;

	// if the NextID is zero, fail it always
	if (NextID == 0)
	{
		--s_NextFreeUserID;
	}
	return NextID;
}


