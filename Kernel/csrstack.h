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
// The Cursor Stack (interface).


#ifndef	CSRSTACK__INC
#define CSRSTACK__INC


class Cursor;


/********************************************************************************************

>	class CursorIdent : public CC_CLASS_MEMDUMP

	Author:		Jim_Lynn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/1/95
	Purpose:	Used in the cursor stack - holds a pointer to a cursor and an identifying ID
	SeeAlso:	CursorStack

********************************************************************************************/

class CursorIdent : public CC_CLASS_MEMDUMP
{
public:
	Cursor* pCursor;				// The pointer to the cursor this object refers to
	INT32 UniqueID;					// Unique identifying ID that is used to reference objects
									// on the cursor stack
	static INT32 NextUniqueID;		// static variable holding next ID to be used

	CursorIdent(); 					// Default constructor
	~CursorIdent();					// Destructor

	INT32 StoreNewCursor(Cursor* NewCursor);

};

/************************************************************************************************

>	class CursorStack : public CC_CLASS_MEMDUMP

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15th November, 1993
	Purpose:	A stack of Cursor objects, which is part of the kernel (NB Cursor objects are
				part of the OIL).

************************************************************************************************/

class CursorStack : public CC_CLASS_MEMDUMP
{
	friend void BeginBusyCursor();
	friend void EndBusyCursor();
	friend void SmashBusyCursor();

	// Give my name in memory dumps
	CC_DECLARE_MEMDUMP(CursorStack);

public:
	CursorStack(INT32 maxdepth = 10);				// construct a (default-sized) stack of cursors
	virtual ~CursorStack();

	static BOOL Init();							// called upon application startup and shutdown
	static void DeInit();

// new static functions to replace accessing Global directly
	static BOOL GIsValid();
	static INT32 GPush(Cursor* pCursor, BOOL ActivateNow = TRUE);
	static Cursor* GPop(INT32 cursorID/* = 0 */);
	static Cursor* GSetTop(Cursor* pCursor, INT32 cursorID/*  = 0*/);
	static void GSetActive();
	static BOOL GIsActive(const Cursor* pCursor);

	static Cursor* GetActive();
	
	void BeginBusy();							// Set the busy cursor (and increment the busy count)
	void EndBusy();								// clear the busy cursor (if the busy count reaches zero)
	void SmashBusy();							// Smash the busy cursor (ensure it is off)

// these are the original member functions, which will probably go private in time
private:
	BOOL IsValid() const;

	INT32 Push(Cursor* pCursor, BOOL ActivateNow = TRUE);
	Cursor* Pop(INT32 cursorID = 0);
	Cursor* SetTop(Cursor* pCursor, INT32 cursorID = 0);

	void SetActive() const;
	BOOL IsActive(const Cursor* pCursor) const;

// IMPORTANT: DONT USE THIS VAR IN NEW CODE - IT IS DUE TO DIE SOON
	static CursorStack* Global;					// the stack shared by the whole application

	CursorIdent* pcStack;						// Pointer to array of CursorIdent objects
	INT32 nNextFreeSlot;							// index of the next available slot in the array
	INT32 nSentinel;								// one more than the last slot in the array

	INT32 BusyCount;								// if >0, show the busy cursor

};



// These global functions provide a convenient shorthand for common cursor functions.

void BeginBusyCursor();
void EndBusyCursor();
void SmashBusyCursor();


#endif	// CSRSTACK__INC
