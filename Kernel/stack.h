// $Id: stack.h 662 2006-03-14 21:31:49Z alex $
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
 * */

#ifndef INC_STACK
#define INC_STACK
// Declaration of the stack and MarkedStack classes

#include "list.h"

/********************************************************************************************

>	class Stack

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/2/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	A general purpose stack for storing ListItems. 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/


class Stack
{
public:
	Stack(); 				 // Constructor 
	~Stack(); 				 // Destructor
	void Push(ListItem* Item); 	 // Pushes item onto the stack
	ListItem* Pop(void); 	 // Pops item from the top of the stack	 
	ListItem* GetTop(void);  // Allows you to peek at the top item on the stack 
	INT32 Size();		 	 // Returns number of items on the stack   
	void DeleteAll(); 			 // Removes all stack items and calls their destructors
	
private:
	List stack;  		     // The stack  
	
};

/********************************************************************************************

>	class ListItemPtrItem: public ListItem

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/2/94
	Purpose:	A ListItem object which is a pointer to a ListItem, used by the MarkedStack 
				class. 
	SeeAlso:	MarkedStack

********************************************************************************************/

 
class ListItemPtrItem: public ListItem
{	
	public:
	ListItem* pListItem; 
};

/********************************************************************************************

>	class MarkedStack: public List 

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/2/94

	Purpose:	A marked stack is a special purpose stack/list. Its properties are described: 

				It has Push and Pop methods 
				It inherits from list so it can easily be searched
				
				It has a Mark() method which marks the current top of stack. Mark() can be
				called more than once to insert a number of marks into the stack. 

				It has a Release() method which restores the state of the stack to the 
				position of the last mark. 

				eg. 

				MarkedStack MS(); 

				MS->Push(a)
				MS->Mark()
				MS->Push(b) 
				MS->Push(c)
				MS->Mark()
				MS->Push(d)

				The stack looks like this 
				
				d <- Tail 
				c <- Mark
				b
				a <- Mark

				If we now perform MS->Release() the stack will look like this 

				c
				b
				a  <- Mark

				If we perform MS->Release() a second time, the stack will look like this

				a

				
	SeeAlso:	-

********************************************************************************************/


class MarkedStack: public List 
{
	public:
	MarkedStack(); 			// constructor
	~MarkedStack(); 		// destructor
	
	void Push(ListItem* Item); 	// Pushes item onto the stack
	ListItem* Pop(void); 	// Pops item from the top of the stack

	BOOL Mark(); 			// Inserts a mark into the stack 
	void Release(); 		// pops and deletes items till we reach the first mark
	
	void DeleteAll(); 		// deletes all elements and marks
	
	private:
	Stack MarkStack; 		// A stack of pointers (marks) into the list 

};
#endif
