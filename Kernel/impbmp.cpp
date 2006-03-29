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

// Encapsulates a set of bitmaps exported to a file.

/*
*/

#include "camtypes.h"

#include "impbmp.h"
#include "bitmap.h"

DECLARE_SOURCE("$Revision: 662 $");

CC_IMPLEMENT_MEMDUMP(ImportBitmap, ListItem)
CC_IMPLEMENT_MEMDUMP(ImportBitmapList, List)
CC_IMPLEMENT_MEMDUMP(ImportedBitmaps, CCObject)

// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW

/********************************************************************************************

>	ImportBitmap::ImportBitmap(KernelBitmap *pNewCol, INT32 NewRecordNumber)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/6/96
	Inputs:		pNewBmp			- the Bitmap to put in the list.
				RecordNumber	- the number of the record that this Bitmap definition has been
								  saved in.
	Purpose:	Create a new export Bitmap list item from the given indexed Bitmap.
	SeeAlso:	ImportBitmapList;

********************************************************************************************/

ImportBitmap::ImportBitmap(KernelBitmap *pNewBmp, INT32 NewRecordNumber)
{
	// Initialise our class variables to these specified values.
	pBmp = pNewBmp;
	RecordNumber = NewRecordNumber;
}

/********************************************************************************************

>	ImportBitmapList::~ImportBitmapList()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/6/96
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Destroy the list of exported Bitmaps.
	SeeAlso:	ImportedBitmaps

********************************************************************************************/

ImportBitmapList::~ImportBitmapList()
{
	// Destroy the list of Bitmaps which we may have
	//ImportBitmap* pExportedBitmap = (ImportBitmap *)GetHead();
	//while (pExportedBitmap)
	//{
	//	delete pExportedBitmap;

		// Try the next Bitmap in the list
//		pExportedBitmap = (ImportBitmap *)Bitmaps.GetNext(pExportedBitmap);
//	}
	
	DeleteAll();
}

/********************************************************************************************

>	ImportBitmap *ImportBitmapList::AddBitmap(KernelBitmap *pBmp, INT32 NewRecordNumber)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/6/96
	Inputs:		pNewBmp			- the Bitmap to put in the list.
				RecordNumber	- the number of the record that this Bitmap definition has been
								  saved in.
	Returns:	Pointer to the newly added Bitmap, or 
				NULL if out of memory => ERROR1
	Purpose:	Add a new Bitmap to the list of exported Bitmaps.
	Errors:		Out of memory => ERROR1
	SeeAlso:	ImportBitmap

********************************************************************************************/

ImportBitmap *ImportBitmapList::AddBitmap(KernelBitmap *pBmp, INT32 NewRecordNumber)
{
	ImportBitmap *pNewBmp = new ImportBitmap(pBmp, NewRecordNumber);

	// Check for out of memory
	if (pNewBmp == NULL)
		return NULL;

	// Add to the list and return success
	AddTail(pNewBmp);
	return pNewBmp;
}

/********************************************************************************************

>	ImportedBitmaps::ImportedBitmaps()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/5/96
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Construct the class which encapsulates a list of exported Bitmaps.
	SeeAlso:	ImportedBitmaps

********************************************************************************************/

ImportedBitmaps::ImportedBitmaps()
{
}

/********************************************************************************************

>	ImportedBitmaps::~ImportedBitmaps()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/6/96
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Destroy the class which encapsulates a list of exported Bitmaps.
	SeeAlso:	ImportedBitmaps

********************************************************************************************/

ImportedBitmaps::~ImportedBitmaps()
{
	// Delete all the bitmaps in the list, as copies will have been made to
	// put in the tree

	ImportBitmap* pBitmap = (ImportBitmap *)Bitmaps.GetHead();
	while (pBitmap != NULL)
	{
		if (pBitmap->pBmp &&
			pBitmap->pBmp->GetParentBitmapList() == NULL)
		{
			// Only delete the bitmap if it has a NULL parent bitmap list,
			// because otherwise it means they are being used
			delete pBitmap->pBmp;
		}

		// Try the next Bitmap in the list
		pBitmap = (ImportBitmap *)Bitmaps.GetNext(pBitmap);
	}

}

/********************************************************************************************

>	BOOL ImportedBitmaps::AddBitmap(KernelBitmap *pBmp, INT32 RecordNumber)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/6/96
	Inputs:		pNewBmp			- the Bitmap to put in the list.
				RecordNumber	- the number of the record that this Bitmap definition has been
								  saved in.
	Returns:	TRUE if the Bitmap could be added;
				FALSE if not
	Purpose:	Add the specified Bitmap to the list of already exported Bitmaps.
	Errors:		-
	SeeAlso:	ImportBitmapList;

********************************************************************************************/

BOOL ImportedBitmaps::AddBitmap(KernelBitmap *pBmp, INT32 RecordNumber)
{
	// Add the Bitmap onto our list
	ImportBitmap * pNewBmp = Bitmaps.AddBitmap(pBmp, RecordNumber);
	if (pNewBmp)
		return TRUE;
	
	return FALSE;
}

/********************************************************************************************

>	KernelBitmap *ImportedBitmaps::GetBitmap(INT32 ReqRecordNumber)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/6/96
	Inputs:		ReqRecordNumber	- the record number to search the list for
	Returns:	Pointer to the KernelBitmap if successful;
				NULL if not found.
	Purpose:	Searches for a previously imported bitmap definition. Used by the native/web
				filter to convert a bitmap reference in a record to an KernelBitmap. This
				must have already been imported in the file as the rule is that reference to
				bitmaps can only be made if the bitmap definition has already been output.
	Errors:		-
	SeeAlso:	ImportedBitmaps; BitmapListComponent;

********************************************************************************************/

KernelBitmap *ImportedBitmaps::GetBitmap(INT32 ReqRecordNumber)
{
	// Work our way through the list of imported bitmaps until we find the required record number
	ImportBitmap* pBitmap = (ImportBitmap *)Bitmaps.GetHead();
	while (pBitmap != NULL)
	{
		if (pBitmap->RecordNumber == ReqRecordNumber)
			return pBitmap->pBmp;

		// Try the next Bitmap in the list
		pBitmap = (ImportBitmap *)Bitmaps.GetNext(pBitmap);
	}

	// Something went a bit wrong and we didn't find the requested record number
	// return NULL to the caller 
	return NULL;
}

