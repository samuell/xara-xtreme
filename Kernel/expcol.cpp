// $Id: expcol.cpp 662 2006-03-14 21:31:49Z alex $
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

// Encapsulates a set of colours exported to a file.

/*
*/

#include "camtypes.h"

#include "expcol.h"
#include "colourix.h"
#include "colmodel.h"

DECLARE_SOURCE("$Revision: 662 $");

CC_IMPLEMENT_MEMDUMP(ExportColour, ListItem)
CC_IMPLEMENT_MEMDUMP(ExportColourList, List)
CC_IMPLEMENT_MEMDUMP(ExportedColours, CCObject)

#define new CAM_DEBUG_NEW

/********************************************************************************************

>	ExportColour::ExportColour(IndexedColour *pNewCol, INT32 NewRecordNumber)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/5/96
	Inputs:		pNewCol			- the colour to put in the list.
				RecordNumber	- the number of the record that this colour definition has been
								  saved in.
	Purpose:	Create a new export colour list item from the given indexed colour.
	SeeAlso:	ExportColourList;

********************************************************************************************/

ExportColour::ExportColour(IndexedColour *pNewCol, INT32 NewRecordNumber)
{
	// Initialise our class variables to these specified values.
	pCol = pNewCol;
	RecordNumber = NewRecordNumber;
}

/********************************************************************************************

>	ExportColour::ExportColour(PColourRGBT *pNewRGBCol, INT32 NewRecordNumber)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/6/96
	Inputs:		pNewRGBCol		- the simple RGB colour to put in the list.
				RecordNumber	- the number of the record that this colour definition has been
								  saved in.
	Purpose:	Create a new export colour list item from the given indexed colour.
	SeeAlso:	ExportColourList;

********************************************************************************************/

ExportColour::ExportColour(PColourRGBT *pNewRGBCol, INT32 NewRecordNumber)
{
	// Initialise our class variables to these specified values.
	// Indexed colour is NULL, which means use the simple colour in searching
	pCol = NULL;
	SimpleRGBColour = *pNewRGBCol;
	RecordNumber = NewRecordNumber;
}

/********************************************************************************************

>	ExportColourList::~ExportColourList()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/5/96
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Destroy the list of exported colours.
	SeeAlso:	ExportedColours

********************************************************************************************/

ExportColourList::~ExportColourList()
{
	// Destroy the list of colours which we may have
	//ExportColour* pExportedColour = (ExportColour *)GetHead();
	//while (pExportedColour)
	//{
	//	delete pExportedColour;

		// Try the next colour in the list
//		pExportedColour = (ExportColour *)Colours.GetNext(pExportedColour);
//	}
	
	DeleteAll();
}

/********************************************************************************************

>	ExportColour *ExportColourList::AddColour(IndexedColour *pCol, INT32 NewRecordNumber)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/5/96
	Inputs:		pNewCol			- the colour to put in the list.
				RecordNumber	- the number of the record that this colour definition has been
								  saved in.
	Returns:	Pointer to the newly added colour, or 
				NULL if out of memory => ERROR1
	Purpose:	Add a new colour to the list of exported colours.
	Errors:		Out of memory => ERROR1
	SeeAlso:	ExportColour

********************************************************************************************/

ExportColour *ExportColourList::AddColour(IndexedColour *pCol, INT32 NewRecordNumber)
{
	ExportColour *pNewCol = new ExportColour(pCol, NewRecordNumber);

	// Check for out of memory
	if (pNewCol == NULL)
		return NULL;

	// Add to the list and return success
	AddTail(pNewCol);
	return pNewCol;
}

/********************************************************************************************

>	ExportColour *ExportColourList::AddColour(PColourRGBT *pNewRGBCol, INT32 NewRecordNumber)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/6/96
	Inputs:		pNewRGBCol		- the simple RGB colour to put in the list.
				RecordNumber	- the number of the record that this colour definition has been
								  saved in.
	Returns:	Pointer to the newly added colour, or 
				NULL if out of memory => ERROR1
	Purpose:	Add a new colour to the list of exported colours.
	Errors:		Out of memory => ERROR1
	SeeAlso:	ExportColour

********************************************************************************************/

ExportColour *ExportColourList::AddColour(PColourRGBT *pNewRGBCol, INT32 NewRecordNumber)
{
	ExportColour *pNewCol = new ExportColour(pNewRGBCol, NewRecordNumber);

	// Check for out of memory
	if (pNewCol == NULL)
		return NULL;

	// Add to the list and return success
	AddTail(pNewCol);
	return pNewCol;
}

/********************************************************************************************

>	ExportedColours::ExportedColours()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/5/96
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Construct the class which encapsulates a list of exported colours.
	SeeAlso:	ExportedColours

********************************************************************************************/

ExportedColours::ExportedColours()
{
}

/********************************************************************************************

>	ExportedColours::~ExportedColours()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/5/96
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Destroy the class which encapsulates a list of exported colours.
	SeeAlso:	ExportedColours

********************************************************************************************/

ExportedColours::~ExportedColours()
{
}

/********************************************************************************************

>	BOOL ExportedColours::AddColour(IndexedColour *pNewCol, INT32 RecordNumber)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/5/96
	Inputs:		pNewCol			- the colour to put in the list.
				RecordNumber	- the number of the record that this colour definition has been
								  saved in.
	Returns:	TRUE if the colour could be added;
				FALSE if not
	Purpose:	Add the specified colour to the list of already exported colours.
	Errors:		-
	SeeAlso:	ExportColourList;

********************************************************************************************/

BOOL ExportedColours::AddColour(IndexedColour *pNewCol, INT32 RecordNumber)
{
	// Add the colour onto our list
	ExportColour * pExpCol = Colours.AddColour(pNewCol, RecordNumber);
	if (pExpCol)
		return TRUE;
	
	return FALSE;
}

/********************************************************************************************

>	BOOL ExportedColours::AddColour(PColourRGBT *pNewRGBCol, INT32 RecordNumber)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/6/96
	Inputs:		pNewRGBCol		- the simple RGB colour to put in the list.
				RecordNumber	- the number of the record that this colour definition has been
								  saved in.
	Returns:	TRUE if the colour could be added;
				FALSE if not
	Purpose:	Add the specified colour to the list of already exported colours.
	Errors:		-
	SeeAlso:	ExportColourList;

********************************************************************************************/

BOOL ExportedColours::AddColour(PColourRGBT *pNewRGBCol, INT32 RecordNumber)
{
	// Add the colour onto our list
	ExportColour * pExpCol = Colours.AddColour(pNewRGBCol, RecordNumber);
	if (pExpCol)
		return TRUE;
	
	return FALSE;
}

/********************************************************************************************

>	ExportColour *ExportedColours::GetColour(IndexedColour *pSearchCol)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/5/96
	Inputs:		pSearchCol			- the colour to search for in the list.
	Returns:	if the item is found then returns		- Pointer to the exported colour list item
				if the item is not found then returns	- NULL.
	Purpose:	Find an indexed colour in the exported list.
	SeeAlso:	ExportColourList

********************************************************************************************/

ExportColour *ExportedColours::GetColour(IndexedColour *pSearchCol)
{
	if (pSearchCol == NULL)
	{
		ERROR3("ExportedColours::GetColour - searching for a null indexed colour!");
	}

	ExportColour* pExportedColour = (ExportColour *)Colours.GetHead();
	while (pExportedColour)
	{
		// Compare the indexed colour pointers and return the match if they are the same
		if (pExportedColour->pCol == pSearchCol)
			return pExportedColour;
		
		// Try the next colour in the list
		pExportedColour = (ExportColour *)Colours.GetNext(pExportedColour);
	}

	// No - return failure.
	return NULL;
}

/********************************************************************************************

>	ExportColour *ExportedColours::GetColour(PColourRGBT *pSearchRGBCol)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/6/96
	Inputs:		pSearchRGBCol		- the simple RGB colour to search for in the list.
	Returns:	if the item is found then returns		- Pointer to the exported colour list item
				if the item is not found then returns	- NULL.
	Purpose:	Find a simpple RGB colour in the exported list.
	SeeAlso:	ExportColourList

********************************************************************************************/

ExportColour *ExportedColours::GetColour(PColourRGBT *pSearchRGBCol)
{
	if (pSearchRGBCol == NULL)
	{
		ERROR3("ExportedColours::GetColour (Simple )- searching for a null indexed colour!");
	}

	ExportColour* pExportedColour = (ExportColour *)Colours.GetHead();
	while (pExportedColour)
	{
		// Only check if this is a simple colour
		if (pExportedColour->pCol == NULL)
		{
			// Compare the simple RGB colours and return the match if they are the same
			if (
				pExportedColour->SimpleRGBColour.Red == pSearchRGBCol->Red &&
				pExportedColour->SimpleRGBColour.Green == pSearchRGBCol->Green &&
				pExportedColour->SimpleRGBColour.Blue == pSearchRGBCol->Blue &&
				pExportedColour->SimpleRGBColour.Transparent == pSearchRGBCol->Transparent
			   )
				return pExportedColour;
		}
		
		// Try the next colour in the list
		pExportedColour = (ExportColour *)Colours.GetNext(pExportedColour);
	}

	// No - return failure.
	return NULL;
}
