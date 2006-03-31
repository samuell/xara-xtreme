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
// This file contains code to handle ObjChange parameter classes.
// An ObjChange parameter is used to pass to a parent node informing
// it that a child has changed.

/*
*/

#include "camtypes.h"
//#include "resource.h"
#include "objchge.h"
#include "errors.h"
#include "paths.h"
#include "undoop.h"
#include "spread.h"

DECLARE_SOURCE("$Revision$");

CC_IMPLEMENT_DYNAMIC(ObjChangeParam, CCObject)
CC_IMPLEMENT_DYNAMIC(ObjChangePathEdit, ObjChangeParam)
CC_IMPLEMENT_DYNAMIC(ObjChangeParamWithToken, ObjChangeParam)

// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW


/********************************************************************************************

>	ObjChangeFlags::ObjChangeFlags(	BOOL Delete 		= FALSE,
									BOOL Replace 		= FALSE,
									BOOL Move 			= FALSE,
									BOOL Attr 			= FALSE,
									BOOL MultiReplace 	= FALSE,
									BOOL Transform	 	= FALSE,
									BOOL Copy			= FALSE,
									BOOL Regen			= FALSE);

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/01/95
	Purpose:	constructor for ObjChange flags class

********************************************************************************************/

ObjChangeFlags::ObjChangeFlags(	BOOL Delete,
								BOOL Replace,
								BOOL Move,
								BOOL Attrib,
								BOOL MultiReplace,
								BOOL Transform,
								BOOL Copy,
								BOOL Regen)
{
 	DeleteNode 			= Delete;
 	ReplaceNode 		= Replace;
 	MoveNode 			= Move;
	Attribute			= Attrib;
	MultiReplaceNode 	= MultiReplace;
	TransformNode 		= Transform;
	CopyNode			= Copy;
	RegenerateNode		= Regen;
}


/********************************************************************************************

>  ObjChangeMask::ObjChangeMask()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/01/95
	Purpose:	constructor for ObjChangeMask. This sets the various maskable changes to
				false, ie they will not be sent during and interactive change.

********************************************************************************************/

ObjChangeMask::ObjChangeMask()
{
	// set all flags to false;
	EorBlobs = FALSE;
	Finished = FALSE;
}

/********************************************************************************************

>  void ObjChangeMask::ClaimAll()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/01/95
	Purpose:	A quick way of setting all the mask bits to true, for a particular
				ObjChangeMask. This starts forcing all ObjChange messages for a particular
				op to be sent.

********************************************************************************************/

void ObjChangeMask::ClaimAll()
{
	// set all flags to false;
	EorBlobs = TRUE;
	Finished = TRUE;
}

/********************************************************************************************

>  void ObjChangeMask::CorrectMask()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/01/95
	Purpose:	Checks the ObjChangeMask settings and corrects them where necessary.
				It thus ensures object change messages are not called out of sequence or
				or missed out when necessary.

********************************************************************************************/

void ObjChangeMask::CorrectMask()
{
	// does nothing at present
}


/********************************************************************************************

>	ObjChangeParam::ObjChangeParam()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/01/95
	Purpose:	Constructor for the ObjChangeParam class

********************************************************************************************/

ObjChangeParam::ObjChangeParam()
{
	ChangeType			= OBJCHANGE_UNDEFINED;
	pChangeOp			= NULL;
	pChangeNode			= NULL;	
	DenialReason		= 0;
	ChangeDirection		= OBJCHANGE_CALLEDBYOP;
	m_pCallingChild		= NULL;
	m_bRetainCachedData = FALSE;
}


/********************************************************************************************

>	ObjChangeParam::ObjChangeParam(	ObjChangeType cType,
									ObjChangeFlags cFlags,
									Node* cNode,
									UndoableOperation* cOp,
									ObjChangeDirection direction=OBJCHANGE_CALLEDBYOP,
									Node* pCallingChild = NULL)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/2/95
	Inputs:		cType	= a change type
				cFlags	= change flags
				cNode	= a pointer to the node thats changing
				cOp		= a pointer to the current operation (NULL if the change is occuring
						  in a none undoable way)
				direction = direction in which the AllowOp is being called
				pCallingChild = the node calling the AllowOp, if OBJCHANGE_CALLEDBYCHILD set.
	Purpose:	Initialises an ObjChange parameter block.

********************************************************************************************/

ObjChangeParam::ObjChangeParam(	ObjChangeType cType,
								ObjChangeFlags cFlags,
								Node* cNode,
								UndoableOperation* cOp,
								ObjChangeDirection direction,
								Node* pCallingChild)
{
	DenialReason = 0;
	Define(cType,cFlags,cNode,cOp,direction,pCallingChild);
}



/********************************************************************************************

>	ObjChangeParam::~ObjChangeParam()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/01/95
	Purpose:	Destructor for the ObjChangeParam class

********************************************************************************************/

ObjChangeParam::~ObjChangeParam()
{
}


/********************************************************************************************

>	void ObjChangeParam::Define(ObjChangeType cType,
								ObjChangeFlags cFlags,
								Node* cNode,
								UndoableOperation* cOp,
								ObjChangeDirection direction=OBJCHANGE_CALLEDBYOP,
								pCallingChild = NULL)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/01/95
	Inputs:		cType	= a change type
				cFlags	= change flags
				cNode	= a pointer to the node thats changing
				cOp		= a pointer to the current operation (NULL if the change is occuring
						  in a none undoable way)
				direction = direction in which AllowOp is called
				pCallingChild = the node calling the AllowOp, if OBJCHANGE_CALLEDBYCHILD set.
	Returns:	-
	Purpose:	Initialises an ObjChange parameter block.

********************************************************************************************/

void ObjChangeParam::Define(ObjChangeType cType,
							ObjChangeFlags cFlags,
							Node* cNode,
							UndoableOperation* cOp,
							ObjChangeDirection direction,
							Node* pCallingChild)
{
	ERROR3IF(cType==OBJCHANGE_UNDEFINED,"ObjChangeParam::Define() called with an illegal change type");
	//ERROR3IF(cNode==NULL,"ObjChangeParam::Define() called with a null child node pointer");

	// Now set the internal variables
	ChangeType			= cType;
	ChangeFlags			= cFlags;
	pChangeOp			= cOp;
	pChangeNode			= cNode;
	ChangeDirection		= direction;
	m_pCallingChild		= pCallingChild;
	m_bRetainCachedData = FALSE;
}


/********************************************************************************************

>	ObjChangePathEdit::ObjChangePathEdit()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/01/95
	Purpose:	Constructor for the ObjChangePathEdit class

********************************************************************************************/

ObjChangePathEdit::ObjChangePathEdit() : ObjChangeParam()
{
	pChangePath = NULL;
	pChangeSpread = NULL;
}


/********************************************************************************************

>	ObjChangePathEdit::~ObjChangePathEdit()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/01/95
	Purpose:	Destructor for the ObjChangePathEdit class

********************************************************************************************/

ObjChangePathEdit::~ObjChangePathEdit()
{
}

/********************************************************************************************

>	void ObjChangePathEdit::Define(	ObjChangeType cType,
									ObjChangeFlags cFlags,
									Node* cNode,
									UndoableOperation* cOp
									Path* cPath
									Spread* cSpread)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/01/95
	Inputs:		cType	= a change type
				cFlags	= change flags
				cNode	= a pointer to the node thats changing
				cOp		= a pointer to the current operation 
						  (NULL if the change is occuring in a none undoable way)
				cPath	= a pointer to the current version of the path
						  (can be NULL for all but PRE and POSTCHANGE parameters)
				cSpread = a pointer to a spread
						  (can be NULL for all but PRE and POSTCHANGE parameters)

	Returns:	-
	Purpose:	Initialises an ObjChangePathEdit parameter block.

********************************************************************************************/

void ObjChangePathEdit::Define(	ObjChangeType cType,
								ObjChangeFlags cFlags,
								Node* cNode,
								UndoableOperation* cOp,
								Path* cPath,
								Spread* cSpread)
{
	ERROR3IF(cPath==NULL,"ObjChangePathEdit::Define() called with a null edit path");
	ERROR3IF(cSpread==NULL,"ObjChangePathEdit::Define() called with a null edit Spread");

	ObjChangeParam::Define(cType,cFlags,cNode,cOp);

	// Now set the internal variables
	pChangePath = cPath;
	pChangeSpread = cSpread;
}



/********************************************************************************************

>	ChangeCode ObjChangePathEdit::Define(Node* cNode,
										 UndoableOperation* cOp
										 Path* cPath
										 Spread* cSpread
										 BOOL All)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/01/95
	Inputs:		cNode	= a pointer to the node thats changing
				cOp		= a pointer to the current operation 
						  (NULL if the change is occuring in a none undoable way)
				cPath	= a pointer to the current version of the path
						  (can be NULL for all but PRE and POSTCHANGE parameters)
				cSpread = a pointer to a spread
						  (can be NULL for all but PRE and POSTCHANGE parameters)
				All		= a bool, used to send the message to all of cNodes parents
						  if TRUE or just its immediate parent if FALSE;
	Returns:	ChangeCode
	Purpose:	Initialises an ObjChangePathEdit parameter block with a ChangeStarting
				message.

********************************************************************************************/

ChangeCode ObjChangePathEdit::ObjChangeStarting(Node* cNode,
												UndoableOperation* cOp,
												Path* cPath,
												Spread* cSpread,
												BOOL All)
{
	// Create some change flags
	ObjChangeFlags ChgeFlags;
	// Now define the change block				   
	Define(OBJCHANGE_STARTING,ChgeFlags,cNode,cOp,cPath,cSpread);
	// Send the message to the nodes parents
	ChangeCode Chge = cNode->WarnParentOfChange(this,All);
	// Correct the message mask returned
	ChangeMask.CorrectMask();

	return Chge;
}



/********************************************************************************************

>	ChangeCode ObjChangePathEdit::RenderCurrentBlobs(Node* cNode,
													 UndoableOperation* cOp,
													 Path* cPath,
													 Spread* cSpread,
													 BOOL All)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/01/95
	Inputs:		See ObjChangeStarting

********************************************************************************************/

ChangeCode ObjChangePathEdit::RenderCurrentBlobs(Node* cNode,
												 UndoableOperation* cOp,
												 Path* cPath,
												 Spread* cSpread,
												 BOOL All)
{
	// Create some change flags
	ObjChangeFlags ChgeFlags;
	// Now define the change block				   
	Define(OBJCHANGE_RENDERCURRENTBLOBS,ChgeFlags,cNode,cOp,cPath,cSpread);
	return (cNode->WarnParentOfChange(this,All));
}



/********************************************************************************************

>	ChangeCode ObjChangePathEdit::RenderChangedBlobs(Node* cNode,
													 UndoableOperation* cOp,
													 Path* cPath,
													 Spread* cSpread,
													 BOOL All)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/01/95
	Inputs:		See ObjChangeStarting

********************************************************************************************/

ChangeCode ObjChangePathEdit::RenderChangedBlobs(Node* cNode,
												 UndoableOperation* cOp,
												 Path* cPath,
												 Spread* cSpread,
												 BOOL All)
{
	// Create some change flags
	ObjChangeFlags ChgeFlags;
	// Now define the change block				   
	Define(OBJCHANGE_RENDERCHANGEDBLOBS,ChgeFlags,cNode,cOp,cPath,cSpread);
	return (cNode->WarnParentOfChange(this,All));
}



/********************************************************************************************

>	ChangeCode ObjChangePathEdit::ObjChangeFinished(Node* cNode,
										 			UndoableOperation* cOp,
										 			Path* cPath,
										 			Spread* cSpread,
										 			BOOL All)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/01/95
	Inputs:		See ObjChangeStarting

********************************************************************************************/

ChangeCode ObjChangePathEdit::ObjChangeFinished(Node* cNode,
												UndoableOperation* cOp,
												Path* cPath,
												Spread* cSpread,
												BOOL All)
{
	// Create some change flags
	ObjChangeFlags ChgeFlags;
	// Now define the change block				   
	Define(OBJCHANGE_FINISHED,ChgeFlags,cNode,cOp,cPath,cSpread);
	return (cNode->WarnParentOfChange(this,All));
}



/********************************************************************************************

>	ChangeCode ObjChangePathEdit::ObjChangeFailed(	Node* cNode,
													UndoableOperation* cOp,
													Path* cPath,
													Spread* cSpread,
													BOOL All)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/01/95
	Inputs:		See ObjChangeStarting
	Purpose:	Send the parents of cNode an OBJCHANGE_FAILED message.

********************************************************************************************/

ChangeCode ObjChangePathEdit::ObjChangeFailed(	Node* cNode,
												UndoableOperation* cOp,
												Path* cPath,
												Spread* cSpread,
												BOOL All)
{
	// Create some change flags
	ObjChangeFlags ChgeFlags;
	// Now define the change block				   
	Define(OBJCHANGE_FAILED,ChgeFlags,cNode,cOp,cPath,cSpread);
	return (cNode->WarnParentOfChange(this,All));
}


/////////////////////////////////////////////////////////////////////////////////////////////
///		ObjChangeParamWithToken


/********************************************************************************************

>	ObjChangeParamWithToken::ObjChangeParamWithToken()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	06/03/95
	Purpose:	Constructor for the ObjChangeParamWithToken class.  This is almost idential
				to ObjChangeParam, except that it always has an OpToken of the operation
				that will be applied.  The pointer to the operation may still be NULL.
				This gives you a way of seeing what the operation will be before it starts.
				Most usefully in an operations GetState.

********************************************************************************************/

ObjChangeParamWithToken::ObjChangeParamWithToken(	ObjChangeType cType,
													ObjChangeFlags cFlags,
													Node* cNode,
													UndoableOperation* cOp,
													String* pToken)
						: ObjChangeParam(	cType,
											cFlags,
											cNode,
											cOp)
{
	OperationToken = *pToken;
}



/********************************************************************************************

>	ObjChangeParamWithToken::~ObjChangeParamWithToken()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	06/03/95
	Purpose:	Destructor for the ObjChangeParamWithToken class

********************************************************************************************/

ObjChangeParamWithToken::~ObjChangeParamWithToken()
{
}
