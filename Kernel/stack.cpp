// $Id: stack.cpp 662 2006-03-14 21:31:49Z alex $
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

// Implementation of the Stack and MarkedStack classes 

#include "camtypes.h" 
#include "stack.h"
#include "errors.h"
#include "ensure.h"
#include "listitem.h"

DECLARE_SOURCE("$Revision: 662 $");

// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW

/********************************************************************************************

>	Stack::Stack()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/2/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Stack constructor
	Errors:		-
	SeeAlso:	-

********************************************************************************************/


Stack::Stack()
{
}

/********************************************************************************************

>	Stack::~Stack()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/2/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Stack destructor. The items on the stack are not deleted
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

Stack::~Stack()
{
	
}

/********************************************************************************************

>	void Stack::Push(ListItem* Item)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/2/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Pushes Item onto the stack 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void Stack::Push(ListItem* Item)
{
	stack.AddTail(Item); // Add the item to the top of the stack 
}

/********************************************************************************************

>	ListItem* Stack::Pop(void)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/2/94
	Inputs:		-
	Outputs:	-
	Returns:	The top item on the stack, or NULL if the stack is empty 
	Purpose:	Removes the item from the top of the stack, and returns it 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

ListItem* Stack::Pop(void)
{	
	return (stack.RemoveTail()); 
}

/********************************************************************************************

>	ListItem* Stack::GetTop(void)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/2/94
	Inputs:		-
	Outputs:	-
	Returns:	The item at the top of the stack
	Purpose:	Allows you to find out what item is on the top of the stack (the item is not
				removed). 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

ListItem* Stack::GetTop(void)
{
	return (stack.GetTail()); 
}


/********************************************************************************************

>	void Stack::DeleteAll()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/2/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Removes and deletes all items on the stack 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void Stack::DeleteAll()
{
	stack.DeleteAll(); // Call the list's DeleteAll method 
}

/********************************************************************************************

>	INT32 Stack::Size()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/2/94
	Inputs:		-
	Outputs:	-
	Returns:	The number of items on the stack 
	Purpose:	For finding the size of the stack 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

INT32 Stack::Size()
{
	return stack.GetCount(); 
}

// ------------------------------------------------------------------------------------------
// MarkedStack methods 

/********************************************************************************************

>	MarkedStack::MarkedStack()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/2/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	MarkedStack constructor 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

MarkedStack::MarkedStack()
{
}

/********************************************************************************************

>	MarkedStack::~MarkedStack()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/2/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	MarkedStack destructor. The items on the stack are not deleted
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

MarkedStack::~MarkedStack()
{
}

/********************************************************************************************

>	MarkedStack::Push(ListItem* Item)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/2/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Pushes Item onto the stack 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/


void MarkedStack::Push(ListItem* Item)
{
	AddTail(Item); // Add the item to the top of the stack 
}

/********************************************************************************************

>	ListItem* MarkedStack::Pop(void)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/2/94
	Inputs:		-
	Outputs:	-
	Returns:	The top item on the stack, or NULL if the stack is empty 
	Purpose:	Removes the item from the top of the stack, and returns it 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

ListItem* MarkedStack::Pop(void)
{	
	return (RemoveTail()); 
}


/********************************************************************************************

>	BOOL MarkedStack::Mark(void)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/2/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if we could create Mark, FALSE if we are out of memory 

	Purpose:	Marks the current Top of the stack so that we can easily return to the 
				same state by calling Release(). 

	Errors:		ERRORIF is called if we are out of memory 
	SeeAlso:	MarkedStack::Release()

********************************************************************************************/

BOOL MarkedStack::Mark(void)
{			
	// Firstly create a mark 													
	ListItemPtrItem* pMark = new ListItemPtrItem(); 
	// Return with an error if we could not create the mark
	if (pMark == NULL)
		return FALSE; 
	pMark->pListItem = GetTail();	// This will be NULL if the list is empty 

   	// Push the mark onto the MarkStack
	MarkStack.Push(pMark); 
	return TRUE; // Success
} 	

/********************************************************************************************

>	void MarkedStack::Release(void)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/2/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Pops and then deletes items from the stack until we find the first mark. 
				If there are no marks then all items on the stack are removed and deleted
	Errors:		-
	SeeAlso:	MarkedStack::Mark

********************************************************************************************/

		 
void MarkedStack::Release(void)
{
	// Find the mark to stop at 
	ListItemPtrItem* MarkRec = (ListItemPtrItem*)MarkStack.Pop();
	ListItem* Mark; 
	if (MarkRec == NULL) 						// There are no marks to find 
	{
		Mark = NULL; 
	} else 
	{
		Mark = MarkRec->pListItem; 
	}  
	ListItem* TopOfStack = GetTail(); 
	ListItem* AsGoodAsDead; 

	while (TopOfStack != Mark)			// Loop until we find Mark
	{
		ENSURE(TopOfStack != NULL, "A Mark could not be found in the Marked stack"); 
		TopOfStack = GetPrev(TopOfStack);
		AsGoodAsDead = RemoveTail();
		delete AsGoodAsDead; 
	}
	if (MarkRec != NULL)
	{
		delete (MarkRec); // Restored State to that pointed to by Mark, so delete it 
	}
} 

/********************************************************************************************

>	void MarkedStack::DeleteAll() 	

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/2/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Deletes all marks and elements in the MarkedStack
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void MarkedStack::DeleteAll()
{
	ListItem* TopOfStack;
	TopOfStack = GetTail(); 
	while (TopOfStack != NULL)	 // loop while the stack is not empty 
	{
		Release(); // Deletes mark and all items before the mark		
		TopOfStack = GetTail(); 
	}	
	// Make sure there are no marks left
	ENSURE(MarkStack.Pop() == NULL, "All marks have not been deleted from the MarkedStack"); 
}	

