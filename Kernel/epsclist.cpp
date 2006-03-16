// $Id: epsclist.cpp 662 2006-03-14 21:31:49Z alex $
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
#include "camtypes.h"
#include "epsclist.h"
#include "epscdef.h"

CC_IMPLEMENT_DYNAMIC ( EPSCommentList, CCObject )

/********************************************************************************************

>	EPSCommentList::EPSCommentList ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/4/00
	Inputs:		-
	Returns:	-
	Purpose:	Creates the class, and sets all the variables to be a default value.
	SeeAlso:	EPSCommentList::~EPSCommentList

********************************************************************************************/

EPSCommentList::EPSCommentList ( void )
{
	// Set the comment list pointer to be NULL, so as to avoid any problems with adding
	// items to it.
	mpCommentList	= NULL;
	mListSize		= 0;
}

/********************************************************************************************

>	EPSCommentList::~EPSCommentList ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/4/00
	Inputs:		-
	Returns:	-
	Purpose:	Destroys the class, and all of its member variables.
	SeeAlso:	EPSCommentList::EPSCommentList

********************************************************************************************/

EPSCommentList::~EPSCommentList ( void )
{
	// Delete the list. This invokes a recursive call to remove all successive items.
	delete mpCommentList;
}

/********************************************************************************************

>	BOOL EPSCommentList::Add ( TCHAR	*pStart,
							   TCHAR	*pEnd,
							   BOOL		DoProcessComments,
							   UINT32		NestingDepth )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/4/00
	Inputs:		pStart				- Pointer to the comment starting a comment block.
				pEnd				- Pointer to the comment that ends this block.
				DoProcessComments	- Do the comments need processing?
				NestingDepth		- The depth to which comments can be nested.
	Returns:	TRUE				- Success.
				FALSE				- The object was not created.
	Purpose:	Adds a list item to the end of mpCommentList record.
	SeeAlso:	EPSCommentDef::EPSCommentDef

********************************************************************************************/

BOOL EPSCommentList::Add ( TCHAR	*pStart,
						   TCHAR	*pEnd,
						   BOOL		DoProcessComments,
						   UINT32		NestingDepth )
{
	// Create a new EPSCommentDef, and assign the appropriate values.
	EPSCommentDef	*pNewDef	= new EPSCommentDef ( pStart, pEnd, DoProcessComments,
													  NestingDepth );

	// Determine whether something was created.
	if ( pNewDef != NULL )
	{
		// Add it to the end of the list.
		if ( mpCommentList == NULL )
		{
			// The pointer is NULL, so just assign the value.
			mpCommentList = pNewDef;
		}
		else
		{
			// There already is a list, so add this to it.
			mpCommentList->Add ( pNewDef );
		}

		// Maintain a count of the number of elements in the list.
		mListSize ++;

		return TRUE;
	}
	else
	{
		// Not enough memory.
		return FALSE;
	}
}

/********************************************************************************************

>	const EPSCommentDef& EPSCommentList::ReturnElement ( INT32 Index );

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/5/00
	Inputs:		Index	- The number of the element to be accessed.
	Returns:	&pItem	- A reference to an item in the list.
	Purpose:	Returns a reference to the nth item in the linked list, where n = Index.

********************************************************************************************/

const EPSCommentDef& EPSCommentList::ReturnElement ( INT32 Index )
{
	// Set up the local variables.
	EPSCommentDef	*pItem	= mpCommentList;
	EPSCommentDef	*pNext	= NULL;
	INT32				Count	= 0;

	// Ensure that the comment list has been initialised.
	ERROR1IF ( pItem == NULL, EmptyDefinition,
			   "The comment list hasn't been initialised!" );

	// Catch out-of-bounds calls.
	if ( Index >= mListSize )
		return EmptyDefinition;

	// Loop through the list until there are no more items, or we reach the right index.
	while ( pNext = pItem->GetNext (), Count < Index && pNext != NULL )
	{
		pItem = pNext;
		Count ++;
	}

	// Return the appropriate list item.
	return *pItem;
}
