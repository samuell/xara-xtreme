// $Id: invalid.cpp 662 2006-03-14 21:31:49Z alex $
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
// The Invalidate an arbitary region action

/*
*/

#include "camtypes.h"
#include "invalid.h"
#include "document.h"

// declare the source
DECLARE_SOURCE("$Revision: 662 $");

// An implement to match the Declare in the .h file.
// If you have many classes, it is recommended to place them all together, here at the start of the file
CC_IMPLEMENT_DYNCREATE(InvalidateArbitaryAction, Action)

// This will get Camelot to display the filename and linenumber of any memory allocations
// that are not released at program exit
// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW



/********************************************************************************************

>	InvalidateArbitaryAction::InvalidateArbitaryAction()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/8/94
	Purpose:	dummy constructor

********************************************************************************************/

InvalidateArbitaryAction::InvalidateArbitaryAction()
{
}
	

/********************************************************************************************

>	ActionCode InvalidateArbitaryAction::Init(Operation* const pOp, ActionList* pActionList,
								Spread* pSpread, DocRect& InvalidRect, Action** NewAction)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/8/94
	Inputs:		pOp - The operation that this action is being created from
				pActionList - The action list that it should be in
				pSpread - The spread that the region to invalidate is on
				InvalidateRect - The region that requires invalidating
	Outputs:	NewAction - Will hold a pointer to an action
	Returns:	an action code
	Purpose:	Creates a new InvalidateArbitaryAction that invalidates an arbitary region
				in the document

********************************************************************************************/

ActionCode InvalidateArbitaryAction::Init(Operation* const pOp, ActionList* pActionList, Spread* pSpread, 
						   const DocRect& InvalidRect, Action** NewAction)
{
	// go and ask the base class to do all its fabby stuff
	ActionCode Ac = Action::Init(pOp, pActionList, sizeof(InvalidateArbitaryAction), 
								 CC_RUNTIME_CLASS(InvalidateArbitaryAction), NewAction);
	
	// If we got something resonable back, then set up our infomation
	if (*NewAction!=NULL)
	{
		((InvalidateArbitaryAction*)(*NewAction))->InvalidRect = InvalidRect;
		((InvalidateArbitaryAction*)(*NewAction))->pSpread = pSpread; 
	}

	// Whatever happen, we have to tell someone
	return Ac; 
}


/********************************************************************************************

>	virtual ActionCode InvalidateArbitaryAction::Execute()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/8/94
	Returns:	An Action Code
	Purpose:	Invalidates the region in the action and builds the appropriate undo/redo
				info to do it again later.

********************************************************************************************/

ActionCode InvalidateArbitaryAction::Execute()
{
	// Attempt to initialise the action    
	InvalidateArbitaryAction* InvRgnAct;  
    ActionCode ActCode = InvalidateArbitaryAction::Init(pOperation, pOppositeActLst, pSpread,
    					 InvalidRect, (Action**)(&InvRgnAct));
    
	// See if it worked
    if (ActCode != AC_FAIL)
	{   
		// Find out about the current document
		Document* pDocument = GetWorkingDoc();
		ENSURE(pDocument!=NULL, "There was no document in InvalidateArbitaryAction::Execute()" );
		
		// actually invalidate the region
		if (pDocument != NULL)
			pDocument->ForceRedraw(pSpread, InvalidRect, TRUE);
	}

	// return the action code we got back earlier
	return ActCode;
}
