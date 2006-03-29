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
#include "camtypes.h"
#include "epscdef.h"

/********************************************************************************************

>	EPSCommentDef::EPSCommentDef ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/4/00
	Inputs:		-
	Returns:	-
	Purpose:	Creates the class, and sets all the variables to be a default value.
	SeeAlso:	EPSCommentDef::~EPSCommentDef

********************************************************************************************/

EPSCommentDef::EPSCommentDef ( void )
{
	// Pointers to next and previous items.
	mpNext			= NULL;

	// State variables.
	StartMarker.Empty ();
	EndMarker.Empty ();
	ProcessContents	= FALSE;
	Nesting			= 0;
}

/********************************************************************************************

>	EPSCommentDef::EPSCommentDef ( const EPSCommentDef &NewValue )


	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/5/00
	Inputs:		NewValue		- The values to be assigned.
	Returns:	-
	Purpose:	Copy constructor.
	SeeAlso:	EPSCommentList::EPSCommentDef

********************************************************************************************/

EPSCommentDef::EPSCommentDef ( const EPSCommentDef &NewValue )
{
	// Copy the data across.
	StartMarker		= NewValue.StartMarker;
	EndMarker		= NewValue.EndMarker;
	ProcessContents	= NewValue.ProcessContents;
	Nesting			= NewValue.Nesting;

	// Default value for the pointer.
	mpNext			= NULL;
}

/********************************************************************************************

>	EPSCommentDef::EPSCommentDef ( TCHAR	*pStart,
								   TCHAR	*pEnd,
								   BOOL		DoProcessComments,
								   UINT32		NestingDepth )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/4/00
	Inputs:		pStart				- Pointer to the comment starting a comment block.
				pEnd				- Pointer to the comment that ends this block.
				DoProcessComments	- Do the comments need processing?
				NestingDepth		- The depth to which comments can be nested.
	Returns:	-
	Purpose:	Creates the class, and sets all the variables to be a passed value.
	SeeAlso:	EPSCommentDef::~EPSCommentDef

********************************************************************************************/

EPSCommentDef::EPSCommentDef ( TCHAR	*pStart,
							   TCHAR	*pEnd,
							   BOOL		DoProcessContents,
							   UINT32		NestingDepth )
{
	// Set the variables according to the passed-in values.
	StartMarker		= pStart;
	EndMarker		= pEnd;
	ProcessContents	= DoProcessContents;
	Nesting			= NestingDepth;

	// Set the values of the pointers.
	mpNext			= NULL;
}

/********************************************************************************************

>	EPSCommentDef::~EPSCommentDef ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/4/00
	Inputs:		-
	Returns:	-
	Purpose:	Destroys the class and all member data.
	SeeAlso:	EPSCommentDef::~EPSCommentDef

********************************************************************************************/

EPSCommentDef::~EPSCommentDef ( void )
{
	// Recursively call down the list.
	if ( mpNext != NULL )
	{
		delete mpNext;
	}
}

/********************************************************************************************

>	void EPSCommentDef::Add ( EPSCommentDef *pNewItem )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/4/00
	Inputs:		pNewItem	- A pointer to the item to be added.
	Returns:	-
	Purpose:	Adds a new item onto the end of the list.
	SeeAlso:	EPSCommentList::Add

********************************************************************************************/

void EPSCommentDef::Add ( EPSCommentDef *pNewItem )
{
	// Recursively call down the list until mpNext is NULL.
	if ( mpNext != NULL )
	{
		mpNext->Add ( pNewItem );
	}
	else
	{
		mpNext = pNewItem;
	}
}

/********************************************************************************************

>	EPSCommentDef& EPSCommentDef::operator= ( const EPSCommentDef &NewValue )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/5/00
	Inputs:		NewValue		- The values to be assigned.
	Returns:	EPSCommentDef&	- The value being assigned (to allow for chaining).
	Purpose:	Sets the value of an EPSCommentDef to be equal to the passed in value.
	SeeAlso:	EPSCommentList::Add

********************************************************************************************/

EPSCommentDef& EPSCommentDef::operator= ( const EPSCommentDef &NewValue )
{
	StartMarker		= NewValue.StartMarker;
	EndMarker		= NewValue.EndMarker;
	ProcessContents	= NewValue.ProcessContents;
	Nesting			= NewValue.Nesting;

	// Ignore the pointers to avoid confusion with the linked list.

	// Return a reference to the new value to allow chaining of = operators.
	return *this;
}

/********************************************************************************************

>	EPSCommentDef* EPSCommentDef::GetNext ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/4/00
	Inputs:		-
	Returns:	mpNext	- A pointer to the next item in the list.
	Purpose:	Gets the pointer to the next item in the linked list.
	SeeAlso:	EPSCommentDef::Add

********************************************************************************************/

EPSCommentDef* EPSCommentDef::GetNext ( void )
{
	// Return a pointer to the next item in the list.
	return mpNext;
}
