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
// Merge list item

#ifndef OVERRIDELIST
#define OVERRIDELIST

//#include "list.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "overitem.h"	//OverrideListItem

/***********************************************************************************************

>	class OverrideList

  Author:	Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
  Created:	21/4/97
  Purpose:	An override list contains items that can override each other.

			For example, imagine we are building a list of clickable areas in
			an imagemap. Our list of clickable areas already contains
			a rectangle with bottom left corner (0,0) and top right corner (200, 200),
			that links to the URL "http://www.xara.com".

			Now we add another clickable rectangle to the imagemap. This rectangle
			has bottom left corner (50,50) and top right corner (100,100) and also
			links to "http://www.xara.com".

			But our first rectangle is contained within our second rectangle! There
			is no need for the second rectangle at all.
			
			It is clear that the first rectangle overrides the second, so the 
			second rectangle should not be added to the list.

			Now imagine the small rectangle is already in the list and the large
			rectangle is added.

			Again, the large rectangle overrides the small one. So the large 
			rectangle should be added to the list and the smaller rectangle
			should be deleted.

			So, this class works as follows.

			When an OverrideListItem is added to this list, this function compares
			the item with every other item in the list. 
			
			It does this by calling either the function
			OverrideListItem::OverrideFromAbove (if the item in the list is before
			the item being added) or OverrideListItem::OverrideFromBelow(if the
			item in the list is below the item being added).

			Then:

			a. If an item in the list overrides the item being added, then the
				new item is not added to the list.

			b. If the item being added overrides an item in the list, then the
				new item is added and the item in the list is deleted.

***********************************************************************************************/

class OverrideList : public List
{   
	CC_DECLARE_DYNAMIC( OverrideList )

	//Constructor
public:
	OverrideList();

	//Overridden functions
public:
	virtual void AddHead( OverrideListItem* );	
	virtual void AddTail( OverrideListItem* );

	virtual LISTPOS InsertBefore(LISTPOS here, OverrideListItem* item);
	virtual ListItem *InsertBefore(OverrideListItem* here, OverrideListItem* item);

	virtual LISTPOS InsertAfter(LISTPOS here, OverrideListItem* item);
	virtual ListItem *InsertAfter(OverrideListItem* here, OverrideListItem* item);

	//Toolkit functions
protected:
	virtual BOOL CompareWithItemsAbove(OverrideListItem* pliStartPos, OverrideListItem* pliToInsert);
	virtual BOOL CompareWithItemsBelow(OverrideListItem* pliStartPos, OverrideListItem* pliToInsert);



};



#endif // OVERRIDELIST

