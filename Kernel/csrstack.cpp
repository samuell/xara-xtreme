// $Id: csrstack.cpp 662 2006-03-14 21:31:49Z alex $
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
// The Cursor Stack (implementation).

/*
*/


#include "camtypes.h"

#include "csrstack.h"
#include "ensure.h"
#include "errors.h"


DECLARE_SOURCE("$Revision $");

// Own up in memory dumps
CC_IMPLEMENT_MEMDUMP(CursorStack, CC_CLASS_MEMDUMP)

//#pragma message( __LOCMSG__ "CAM_DEBUG_NEW won't work" )
#define new CAM_DEBUG_NEW


// As Camelot is a single-threaded application, all DocViews share one cursor stack,
// along with the rest of the application.  This is it.
CursorStack* CursorStack::Global;
INT32 CursorIdent::NextUniqueID = 1;


/********************************************************************************************
>	static BOOL CursorStack::Init()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/11/93
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if initialisation was successful.
	Purpose:	Initialises the cursor system, loading the standard OS-provided cursors,
				such as the arrow and the hour-glass, creating the application's cursor
				stack etc.
	Errors:		-
	SeeAlso:	CursorStack::DeInit; Cursor::Init
********************************************************************************************/

BOOL CursorStack::Init()
{
	Global = new CursorStack;
	if (!Global || !Global->IsValid() || !Cursor::Init())
	{
		delete Global;
		Global = 0;
		return FALSE;
	}
	Global->Push(Cursor::Arrow);	// This is about the only occurrence of Push
									// that can legitimately ignore the return value, since
									// this cursor should always be on the stack.
	Global->SetActive();
	return TRUE;
}




/********************************************************************************************
>	static void CursorStack::DeInit()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/11/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Shuts down the cursor system, releasing resources allocated by the Init()
				member function.
	Errors:		-
	SeeAlso:	CursorStack::Init; Cursor::DeInit
********************************************************************************************/

void CursorStack::DeInit()
{
	if (!Global) return;
	Global->Pop();				// undo the push done in Init()
	delete Global;
	Global = 0;
	Cursor::DeInit();
}




/********************************************************************************************
>	CursorStack::CursorStack()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/11/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Constructs an empty stack of Cursor objects, with a maximum depth (by default
				this is 10 deep).
	Errors:		The allocation of the stack may fail, if so all subsequent calls to Push
				and Pop will be ignored.
	SeeAlso:	CursorStack::CursorStack
********************************************************************************************/

CursorStack::CursorStack(INT32 maxdepth)
{
	nSentinel = maxdepth;
	nNextFreeSlot = 0;
	BusyCount = 0;
	
	pcStack = new CursorIdent[maxdepth];		// array of CursorIdent objects

#ifdef	_DEBUG
	if( !pcStack )
		TRACEUSER( "JustinF", _T("Failed to allocate stack in CursorStack::CursorStack!\n"));
#endif	// _DEBUG
}




/********************************************************************************************
>	virtual CursorStack::~CursorStack()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/11/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Destroys a CursorStack object.
	Errors:		-
	SeeAlso:	CursorStack::CursorStack
********************************************************************************************/

CursorStack::~CursorStack()
{
	#ifdef	_DEBUG
		if (nNextFreeSlot != 0)
			TRACE( _T("WARNING: Cursor Stack at 0x%p not empty (%d deep) upon destruction!\n"),
				  this, nNextFreeSlot);
	#endif	// _DEBUG

	delete[] pcStack;
}




/********************************************************************************************
>	BOOL CursorStack::IsValid() const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/11/93
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the stack was allocated OK.
	Purpose:	Tests if the object was successfully constructed, and is coherent.
	Errors:		-
	SeeAlso:	-
********************************************************************************************/

BOOL CursorStack::IsValid() const
{
	return pcStack != 0 && (nNextFreeSlot >= 0 && nNextFreeSlot <= nSentinel);
}




/********************************************************************************************
>	INT32 CursorStack::Push(Cursor* pCursor, BOOL ActivateNow = TRUE)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/11/93
	Modified:	27/1/95 by Jim
	Inputs:		pCursor - A pointer to a Cursor object.
				ActivateNow - if TRUE (the default), the cursor will be immediately activated
				(will become visible immediately). If FALSE, the cursor will not be displayed
				until the mouse pointer is moved over the appropriate window/area. (The latter
				form is used by tools so that there is no cursor-flicker when a tool is
				selected)
	Outputs:	-
	Returns:	Unique ID which should be used to reference this cursor while it's in the stack
	Purpose:	Pushes the parameter onto the stack, making it top-most and	hence the Cursor
				which will become active when SetActive() is called (this is done automatically
				by default, or if you pass in ActivateNow == TRUE)
				The Unique ID returned will be required by calls to SetTop and Pop so that
				we don't have any problems with the wrong cursors being popped and deleted
	Errors:		Tests for stack overflow.
	SeeAlso:	CursorStack::Pop; CursorStack::SetActive
********************************************************************************************/

INT32 CursorStack::Push(Cursor* pCursor, BOOL ActivateNow)
{
	ENSURE(nNextFreeSlot < nSentinel, "Overflow in CursorStack::Push!\n");

	INT32 retID = pcStack[nNextFreeSlot++].StoreNewCursor(pCursor);
	if (ActivateNow)
		SetActive();

	return retID;
}




/********************************************************************************************
>	Cursor* CursorStack::Pop(INT32 cursorID)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>/Jim
	Created:	11/11/93
	Modified:	27/1/95
	Inputs:		Unique ID of the cursor you want to pop from the stack
	Outputs:	-
	Returns:	A pointer to the Cursor object that was previously at the top of the stack.
				Returns NULL if the stack is empty or a cursor of that ID is not found
	Purpose:	Removes a cursor from the stack. Because of the strange, asynchronous nature
				of Camelot, and the way cursors are seemingly added and removed from the stack
				at random, when popping a cursor from the stack, a unique ID is used to identify
				which cursor you actually want to remove. That one is removed, whether it's at the
				top of the stack or not.
	Errors:		Checks for stack underflow. Also, it now handles stack underflow, as well.
				Will ERROR in debug builds if the ID is not found in the stack.
	SeeAlso:	CursorStack::Push; CursorStack::SetActive
********************************************************************************************/

Cursor* CursorStack::Pop(INT32 cursorID)
{
	ERROR2IF(nNextFreeSlot <= 0,NULL, "Underflow in CursorStack::Pop!\n");
	
	// For old code which doesn't pass the ID, we'll pop the top one off and hope
	if (cursorID == 0)
	{	
		Cursor* pc = pcStack[--nNextFreeSlot].pCursor;
		return pc;
	}
	else
	{
		INT32 n=0;		// Counter to go through the stack
		
		// Yes, I know this while loop could be more cleverly implemented, but I'm
		// sticking with easier to understand code, and letting the optimiser do
		// the hard work.

		// Find the ID on the stack and remove it, sealing the hole up
		while(n<nNextFreeSlot)
		{
			if (pcStack[n].UniqueID == cursorID)
			{
				Cursor* pc = pcStack[n].pCursor;	// for return

				// Now, what's the best way of closing up the gap?
				INT32 m = n+1;						// Point at the next item on stack
				while (m<nNextFreeSlot)				// Don't copy anything if n is the last one
				{
					pcStack[n].pCursor = pcStack[m].pCursor;
					pcStack[n].UniqueID = pcStack[m].UniqueID;
					n++;
					m++;
				}

				// Now the gap has been closed, let's decrement the nextfree counter
				nNextFreeSlot--;
				if (nNextFreeSlot > 0) SetActive();

				// Finally, return the pointer to the cursor we've just removed from the stack
				return pc;

			}
			n++;
		}

		// If we arrive here, there wasn't a valid cursor on the stack, so complain
		ERROR2(NULL,"Unique ID not found in cursor stack\n");
	
	}

}




/********************************************************************************************
>	Cursor* CursorStack::SetTop(Cursor* pCursor, INT32 cursorID = 0)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/11/93
	Modified:	28/1/95 by Jim
	Inputs:		pCursor points to a Cursor object.
				cursorID is the unique ID of the cursor that wants to be changed
	Outputs:	-
	Returns:	A pointer to the Cursor object that was previously occupied by the cursor replaced
				or NULL if there wasn't one.
	Purpose:	Replaces a cursor on the stack with a new Cursor object.  Combines a pop and
				a push.  Note that the function detects if the stack is empty and will skip
				the initial pop if it is. The unique ID is used to find which cursor is actually
				to be replaced. If the cursor passed to this function is the same as the one
				already occupying the slot, no switching is done, otherwise the top cursor on the
				stack will be switched on (not necessarily the one passed to this function).
	Errors:		-
	SeeAlso:	CursorStack::Push; CursorStack::Pop
********************************************************************************************/

Cursor* CursorStack::SetTop(Cursor* pCursor, INT32 cursorID)
{
	// cursorID will be zero if unmodified code is used.
	if (cursorID == 0)
	{
		ERROR3("CursorStack::SetTop called without a valid cursorID. Please change the offending code.\n");
		Cursor* pc;
		if (nNextFreeSlot > 0)
		{
			pc = pcStack[nNextFreeSlot - 1].pCursor;
			pcStack[nNextFreeSlot - 1].pCursor = pCursor;
		}
		else
		{
			pc = NULL;
			Push(pCursor);
		}
		SetActive();
		return pc;
	}
	else
	{
		// This is the new case
		INT32 n=0;		// Counter to go through the stack
		
		// Find the ID on the stack and replace it with the new cursor
		while(n<nNextFreeSlot)
		{
			if (pcStack[n].UniqueID == cursorID)
			{
				Cursor* pc = pcStack[n].pCursor;	// for return
				if (pc == pCursor)
					return pc;						// Don't try and set the cursor if it's the same

				// Replace this cursor with the new one
				pcStack[n].pCursor = pCursor;

				if (nNextFreeSlot > 0) SetActive();

				// Finally, return the pointer to the cursor we've just removed from the stack
				return pc;

			}
			n++;
		}
		// Oh dear, we seem not to have found this cursorID on the stack. 
		ERROR2(NULL,"cursorID not found on stack in SetTop.\n");
	}
}




/********************************************************************************************
>	void CursorStack::SetActive() const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/11/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Makes the Cursor object on the top of the stack the active cursor.
	Errors:		Checks that the stack isn't empty. Returns if it is
	SeeAlso:	Cursor::SetActive
********************************************************************************************/

void CursorStack::SetActive() const
{
	if (nNextFreeSlot <= 0)
	{
		ERROR3("Stack empty in CursorStack::SetActive!\n");
		return;
	}
	
	// If BusyCount > 0 show the busy cursor instead of the topmost one
	if (BusyCount > 0)
		Cursor::Busy->SetActive();
	else
		pcStack[nNextFreeSlot - 1].pCursor->SetActive();
}

Cursor* CursorStack::GetActive()
{
	if( Global->nNextFreeSlot <= 0 )
	{
		ERROR3("Stack empty in CursorStack::SetActive!\n");
		return NULL;
	}
	
	// If BusyCount > 0 show the busy cursor instead of the topmost one
	if( Global->BusyCount > 0 )
	{
		return Cursor::Busy;
	}
	else
	{
		return Global->pcStack[Global->nNextFreeSlot - 1].pCursor;
	}
}

/********************************************************************************************
>	BOOL CursorStack::IsActive(const Cursor* pCursor) const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/11/93
	Inputs:		A pointer to a (constant) Cursor object.
	Outputs:	-
	Returns:	TRUE if the top of the stack is the same as the parameter, FALSE if it isn't
				or if the stack is empty.
	Purpose:	Tests if a particular cursor is on the top of the stack, ie. is currently
				visible and active.
	Errors:		-
	SeeAlso:	-
********************************************************************************************/

BOOL CursorStack::IsActive(const Cursor* pCursor) const
{
//	return (nNextFreeSlot > 0) ? (pcStack[nNextFreeSlot - 1] == pCursor) : FALSE;

	// if BusyCount>0 that means the busy cursor is active, so check for that situation

	if (BusyCount > 0)
	{
		return (pCursor == Cursor::Busy);
	}
	else
	{
		if (nNextFreeSlot>0 && (pcStack[nNextFreeSlot - 1].pCursor == pCursor))

			return TRUE;

		return FALSE;
	}
}

/********************************************************************************************

>	void CursorStack::BeginBusy()

	Author:		Jim_Lynn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/1/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Tells the cursor stack to show the busy cursor. Multiple calls will increment
				a usage count, and the cursor won't disappear until the usage count drops
				to zero.
	Errors:		-
	SeeAlso:	CursorStack::EndBusy; CursorStack::SmashBusy

********************************************************************************************/

void CursorStack::BeginBusy()
{
	BusyCount++;
	SetActive();
}

/********************************************************************************************

>	void CursorStack::EndBusy()

	Author:		Jim_Lynn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/1/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Turns off the busy cursor (if there has only been one call to BeginBusy) for this
				cursor stack. Waits until BusyCount reaches zero before turning off the busy
				cursor.
	Errors:		ERROR3 if busy count drops below zero.
	SeeAlso:	CursorStack::BeginBusy; CursorStack::SmashBusy

********************************************************************************************/

void CursorStack::EndBusy()
{
	if ((--BusyCount) < 0)
	{
		BusyCount=0;
		ERROR3("Too many calls to CursorStack::EndBusy\n");
	}
	SetActive();
}



/********************************************************************************************

>	void CursorStack::SmashBusy()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/8/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Turns off the busy cursor (without exceptions) for this cursor stack.
				Resets the Busy count to zero.
				Called by Progress.cpp's SmashBusyJob to ensure the cursor is off.

	SeeAlso:	CursorStack::BeginBusy; CursorStack::EndBusy

********************************************************************************************/

void CursorStack::SmashBusy()
{
	if (BusyCount == 0)		// Not showing a busy cursor, so return ASAP
		return;

	// Turn off the busy cursor
	BusyCount = 0;
	SetActive();
}


/********************************************************************************************

>	void BeginBusyCursor()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/11/93
	Modified:	28/1/95 by Jim
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Tells the global cursor stack to display a busy cursor
	Errors:		-
	SeeAlso:	CursorStack::Push; CursorStack::SetActive; EndBusyCursor; SmashBusyCursor

********************************************************************************************/

void BeginBusyCursor()
{
	//CursorStack::Global->Push(Cursor::Busy);
	if (CursorStack::Global)
		CursorStack::Global->BeginBusy();
}


/********************************************************************************************

>	void EndBusyCursor()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/11/93
	Modified:	28/1/95 by Jim
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Tells the global cursor stack to stop displaying the busy cursor. If more
				than one call has been previously made to beginBusyCursor, the busy cursor
				isn't removed - a usage count is decremented.
	Errors:		ERROR3 failure if the cursor stack isn't displaying a busy cursor at all
	SeeAlso:	CursorStack::Pop; CursorStack::SetActive; BeginBusyCursor; SmashBusyCursor

********************************************************************************************/

void EndBusyCursor()
{
	if (CursorStack::Global)
		CursorStack::Global->EndBusy();
}


/********************************************************************************************

>	void SmashBusyCursor()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/8/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Tells the global cursor stack to stop displaying the busy cursor, regardless
				of how many calls have been previously made to beginBusyCursor.

	SeeAlso:	BeginBusyCursor; EndBusyCursor

********************************************************************************************/

void SmashBusyCursor()
{
	if (CursorStack::Global)
		CursorStack::Global->SmashBusy();
}


// new static functions to replace member functions. Will be properly commented in time
// (and probably have the G prefix removed too)

BOOL CursorStack::GIsValid()
{
	return Global->IsValid();
}

INT32 CursorStack::GPush(Cursor* pCursor, BOOL ActivateNow )
{
	return Global->Push( pCursor, ActivateNow );
}

Cursor* CursorStack::GPop(INT32 cursorID)
{
	return Global->Pop(cursorID);
}

Cursor* CursorStack::GSetTop(Cursor* pCursor, INT32 cursorID)
{
	return Global->SetTop(pCursor, cursorID);
}

void CursorStack::GSetActive()
{
	Global->SetActive();
}

BOOL CursorStack::GIsActive(const Cursor* pCursor)
{
	return Global->IsActive(pCursor);
}

/**************** CursorIdent class functions ************************/

/********************************************************************************************

>	CursorIdent::CursorIdent()

	Author:		Jim_Lynn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/1/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Default constructor for a CursorIdent object. Defaults fields
				to zero so that new objects are invalid.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

CursorIdent::CursorIdent()
	{
		pCursor = 0;
		UniqueID = 0;
	}

/********************************************************************************************

>	CursorIdent::~CursorIdent()

	Author:		Jim_Lynn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/1/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Default constructor for CursorIdent objects. Sets all fields back to zero
				so that a destructed object is invalid.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

CursorIdent::~CursorIdent()
	{
		pCursor = 0;
		UniqueID = 0;
	}

/********************************************************************************************

>	INT32 CursorIdent::StoreNewCursor(Cursor* NewCursor)

	Author:		Jim_Lynn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/1/95
	Inputs:		NewCursor points at the cursor that's being stored in this ident object
	Outputs:	-
	Returns:	new unique ID (which is also stored in the object)
	Purpose:	This function stores a pointer to a cursor in an ident object
				and generates a new unique ID which it returns, as well as storing it in
				the object itself. It also increments the static NextUniqueID counter.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

INT32 CursorIdent::StoreNewCursor(Cursor* NewCursor)
{
	pCursor = NewCursor;
	UniqueID = NextUniqueID++;
	return UniqueID;
}
