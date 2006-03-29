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
// Imagemap class

#include "camtypes.h"
#include "overlist.h"
#include "list.h"



CC_IMPLEMENT_DYNAMIC(OverrideList, List)

/********************************************************************************************

	OverrideList::OverrideList()

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/4/97
	Purpose:	Default constructor
	
********************************************************************************************/

OverrideList::OverrideList() : List()
{
}

/********************************************************************************************

	virtual void OverrideList::AddHead( OverrideListItem* poliToAdd)

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/4/97
	Inputs:		poliToAdd	The item to add
				
	Returns:	-

	Purpose:	Compares this list item with all the items in the list,
				to find out whether they override this list item or
				whether this list item overrides them.

				If this list item overrides any items already present
				in the list, the items in the list are deleted and this
				list item is added.

				If any item in the list overrides this list item,
				this list item is not added to the list.

				This function simply calls the InsertBefore function.

  SeeAlso:		OverrideList::InsertBefore()
	
********************************************************************************************/

void OverrideList::AddHead( OverrideListItem* poliToAdd)
{
	if (poliToAdd==NULL)
	{
		ERROR2RAW("OverrideList::AddHead - NULL parameter");
		return;
	}

	//Get the first item in the list
	OverrideListItem* pliFirst=(OverrideListItem*) GetHead();

	//Was there anything in the list?
	if (pliFirst!=NULL)				 
	{
		//Yes. So call our InsertBefore function
		InsertBefore(pliFirst, poliToAdd);
	}
	else
	{
		//No. So we need do no special checking - simply insert
		//the list item
		List::AddHead(poliToAdd);
	}
	
}

/********************************************************************************************

	virtual void OverrideList::AddTail( OverrideListItem* poliToAdd)

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/4/97
	Inputs:		poliToAdd	The item to add
				
	Returns:	-

	Purpose:	Compares this list item with all the items in the list,
				to find out whether they override this list item or
				whether this list item overrides them.

				If this list item overrides any items already present
				in the list, the items in the list are deleted and this
				list item is added.

				If any item in the list overrides this list item,
				this list item is not added to the list.

				This function simply calls the InsertAfter function.

  SeeAlso:		OverrideList::InsertAfter()
	
********************************************************************************************/

void OverrideList::AddTail( OverrideListItem* poliToAdd)
{
	if (poliToAdd==NULL)
	{
		ERROR2RAW("OverrideList::AddTail - NULL parameter");
		return;
	}

	//Get the last item in the list
	OverrideListItem* pliLast=(OverrideListItem*) GetTail();

	//Was there anything in the list?
	if (pliLast!=NULL)				 
	{
		//Yes. So call our InsertAfter function
		InsertAfter(pliLast, poliToAdd);
	}
	else
	{
		//No. So we need do no special checking - simply insert
		//the list item
		List::AddTail(poliToAdd);
	}

	//And add our item after it
	InsertAfter(pliLast, poliToAdd);
	
}

/********************************************************************************************

	virtual ListItem* OverrideList::InsertBefore(OverrideListItem* here, OverrideListItem* item);

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/4/97
	Inputs:		poliToAdd	The item to add
				
	Returns:	-

	Purpose:	Compares this list item with all the items in the list,
				to find out whether they override this list item or
				whether this list item overrides them.

				It does this by calling the function 
				OverrideListItem::OverrideFromBelow on all items
				below the insertion position, and MergeListItem::OverrideFromAbove
				on all items above the insertion position.

				If this list item overrides any items already present
				in the list, the items in the list are deleted and this
				list item is added.

				If any item in the list overrides this list item,
				this list item is not added to the list.

				This function simply calls the InsertBefore function.

  SeeAlso:		OverrideList::InsertBefore()
	
********************************************************************************************/

ListItem* OverrideList::InsertBefore(OverrideListItem* pliInsertPoint, OverrideListItem* pliToInsert)
{
	ERROR2IF(pliToInsert==NULL, NULL, "OverrideList::InsertBefore - NULL parameter");
		
	//This variable will tell us whether to insert pliToInsert
	//into the list
	BOOL fInsert=TRUE;

	//First compare pliToInsert with everything below pliInsertPoint in the list
	fInsert=CompareWithItemsBelow(pliInsertPoint, pliToInsert);

	//Now get the list item above pliInsertPoint
	OverrideListItem* pliTest=(OverrideListItem*) GetPrev(pliInsertPoint);

	//If there is anything above pliInsertPoint
	if (fInsert && pliTest!=NULL)
	{
		//Then test everything above pliInsertPoint
		fInsert=CompareWithItemsAbove(pliInsertPoint, pliToInsert);
	}

	//So, should we insert pliToInsert?
	if (fInsert)
		//Yes. So do it.
		return List::InsertBefore(pliInsertPoint, pliToInsert);
	else
		//No. So return an error value.
		return NULL;
	
}

/********************************************************************************************

	virtual ListItem* OverrideList::InsertBefore(OverrideListItem* here, OverrideListItem* item);

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/4/97
	Inputs:		poliToAdd	The item to add
				
	Returns:	-

	Purpose:	Compares this list item with all the items in the list,
				to find out whether they override this list item or
				whether this list item overrides them.

				This function simply calls the other InsertBefore override.
								
  SeeAlso:		OverrideList::InsertBefore()
	
********************************************************************************************/

LISTPOS OverrideList::InsertBefore(LISTPOS here, OverrideListItem* item)
{

	ERROR2IF(item==NULL, NULL, "OverrideList::InsertBefore - NULL parameter");
		

	//First find the list item at the position we have been given
	OverrideListItem* pliInsertionPoint=(OverrideListItem*) FindItem(here);

	//If we have not found it, return -1
	if (pliInsertionPoint==NULL)
		return -1;

	//Otherwise, call our other function to do the insertion
	OverrideListItem* pliResult=(OverrideListItem*) InsertBefore(pliInsertionPoint, item);

	//If we have been returned NULL, then return an error value
	if (pliResult==NULL)
		return -1;

	//Otherwise, find our newly returned list item in the list
	//and return it
	return FindPosition(pliResult);
	
}

/********************************************************************************************

	virtual ListItem* OverrideList::InsertAfter(OverrideListItem* here, OverrideListItem* item);

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/4/97
	Inputs:		poliToAdd	The item to add
				
	Returns:	-

	Purpose:	Compares this list item with all the items in the list,
				to find out whether they override this list item or
				whether this list item overrides them.

				It does this by calling the function 
				MergeListItem::OverrideFromBelow on all items
				below the insertion position, and MergeListItem::OverrideFromAbove
				on all items above the insertion position.

				If this list item overrides any items already present
				in the list, the items in the list are deleted and this
				list item is added.

				If any item in the list overrides this list item,
				this list item is not added to the list.

				This function simply calls the InsertBefore function.

  SeeAlso:		OverrideList::InsertBefore()
	
********************************************************************************************/

ListItem* OverrideList::InsertAfter(OverrideListItem* pliInsertPoint, OverrideListItem* pliToInsert)
{
	ERROR2IF(pliToInsert==NULL, NULL, "OverrideList::InsertAfter - NULL parameter");

	//This variable will tell us whether to insert pliToInsert
	//into the list
	BOOL fInsert=TRUE;

	//First compare pliToInsert with everything above pliInsertPoint in the list
	fInsert=CompareWithItemsAbove(pliInsertPoint, pliToInsert);

	//Now get the list item after pliInsertPoint
	OverrideListItem* pliTest=(OverrideListItem*) GetNext(pliInsertPoint);

	//If there is anything before pliInsertPoint
	if (fInsert && pliTest!=NULL)
	{
		//Then test everything before pliInsertPoint
		fInsert=CompareWithItemsBelow(pliInsertPoint, pliToInsert);
	}

	//So, should we insert pliToInsert?
	if (fInsert)
		//Yes. So do it.
		return List::InsertAfter(pliInsertPoint, pliToInsert);
	else
		//No. So return an error value.
		return NULL;


	
}

/********************************************************************************************

	virtual ListItem* OverrideList::InsertAfter(OverrideListItem* here, OverrideListItem* item);

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/4/97
	Inputs:		poliToAdd	The item to add
				
	Returns:	-

	Purpose:	Compares this list item with all the items in the list,
				to find out whether they override this list item or
				whether this list item overrides them.

				This function simply calls the other InsertAfter override.
								
  SeeAlso:		OverrideList::InsertAfter()
	
********************************************************************************************/

LISTPOS OverrideList::InsertAfter(LISTPOS here, OverrideListItem* item)
{
	ERROR2IF(item==NULL, 0, "OverrideList::InsertAfter - NULL parameter");

	//First find the list item at the position we have been given
	OverrideListItem* pliInsertionPoint=(OverrideListItem*) FindItem(here);

	//If we have not found it, return -1
	if (pliInsertionPoint==NULL)
		return -1;

	//Otherwise, call our other function to do the insertion
	OverrideListItem* pliResult=(OverrideListItem*) InsertAfter(pliInsertionPoint, item);

	//If we have been returned NULL, then return an error value
	if (pliResult==NULL)
		return -1;

	//Otherwise, find our newly returned list item in the list
	//and return it
	return FindPosition(pliResult);
	
}

/*****************************************************

  Toolkit functions

  ****************************************************/

/********************************************************************************************

	virtual BOOL OverrideList::CompareWithItemsAbove(OverrideListItem* pliStartPos, OverrideListItem* pliToInsert);

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/4/97
	Inputs:		pliStartPos		The position to start searching from
				pliToAdd		The list items to add
				
	Returns:	FALSE	if pliToAdd was overridden by another
						item in the list

				TRUE	if pliToAdd was not overridden

				Note that, if pliToAdd overrides another item in the list,
				this does not affect the result returned.

	Purpose:	Compares this list item with all the items 
				above pliStartPos, inclusive.
				
  SeeAlso:		OverrideList::InsertBefore(), OverrideList::After()
	
********************************************************************************************/

BOOL OverrideList::CompareWithItemsAbove(OverrideListItem* pliStartPos, OverrideListItem* pliToInsert)
{
	//This pointer will show the item we are looking at
	OverrideListItem* pliLook=pliStartPos;

	//While we are still looking at a valid item
	while (pliLook!=NULL)
	{
		//This will be the next item we look at
		//It's important to do this now - because we may delete pliLook in a moment
		OverrideListItem* pliNext=(OverrideListItem*) GetPrev(pliLook);

		//Does the item we are looking at override the item
		//we are about to insert?
		if (pliLook->OverrideFromAbove(pliToInsert))
		{
			//Yes. So return to say that we're not going to insert the
			//item
			return FALSE;
		}

		//Does the item we are about to insert override the item
		//we are looking at?
		if (pliToInsert->OverrideFromBelow(pliLook))
		{
			//Yes. So delete the item we are looking at.
			RemoveItem(pliLook);
			delete pliLook;
		}

		//And move on to the next item
		pliLook=pliNext;
	}

	//ANd return to say that pliToInsert has not been overridden
	return TRUE;
}

/********************************************************************************************

	virtual BOOL OverrideList::CompareWithItemsBelow(OverrideListItem* pliStartPos, OverrideListItem* pliToInsert)

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/4/97
	Inputs:		pliStartPos		The position to start searching from
				pliToAdd		The list items to add
				
	Returns:	FALSE	if pliToAdd was overridden by another
						item in the list

				TRUE	if pliToAdd was not overridden

				Note that, if pliToAdd overrides another item in the list,
				this does not affect the result returned.

	Purpose:	Compares this list item with all the items 
				below pliStartPos, inclusive.
				
  SeeAlso:		OverrideList::InsertBefore(), OverrideList::After()
	
********************************************************************************************/

BOOL OverrideList::CompareWithItemsBelow(OverrideListItem* pliStartPos, OverrideListItem* pliToInsert)
{
	//This pointer will show the item we are looking at
	OverrideListItem* pliLook=pliStartPos;

	//While we are still looking at a valid item
	while (pliLook!=NULL)
	{
		//This will be the next item we look at
		//It's important to do this now - because we may delete pliLook in a moment
		OverrideListItem* pliNext=(OverrideListItem*) GetNext(pliLook);

		//Does the item we are looking at override the item
		//we are about to insert?
		if (pliLook->OverrideFromBelow(pliToInsert))
		{
			//Yes. So return to say that we're not going to insert the
			//item
			return FALSE;
		}

		//Does the item we are about to insert override the item
		//we are looking at?
		if (pliToInsert->OverrideFromAbove(pliLook))
		{
			//Yes. So delete the item we are looking at.
			RemoveItem(pliLook);
			delete pliLook;
		}
				
		//And move on to the next item
		pliLook=pliNext;
	}

	//And return to say that pliToInsert has not been overridden
	return TRUE;
}



	 	
