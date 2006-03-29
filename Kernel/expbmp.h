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

// Encapsulates a set of bitmaps exported from a file.

#ifndef INC_EXPBMP
#define INC_EXPBMP

#include "list.h"
#include "listitem.h"

class BitmapListComponent;
class KernelBitmap;

/********************************************************************************************

>	class ExportBitmap : public ListItem

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/6/96
	Purpose:	Represents a bitmap object that has been saved out into the file already.
				This enables any other references to the bitmap to be output as we have a
				mechanism to remember that a bitmap definition has already been saved into
				the file and the record number it was saved under.
	SeeAlso:	BitmapListComponent;

********************************************************************************************/

class ExportBitmap : public ListItem
{
	CC_DECLARE_MEMDUMP(ExportBitmap)

public:
	// Adds a newly saved Bitmap to the list
	ExportBitmap( KernelBitmap *pNewBmp,  INT32 NewRecordNumber);

	// Pointer to the Bitmap that has been exported.
	KernelBitmap *pBmp;

	// If saved		- this will be the record number that the Bitmap definition is saved in
	// If unsaved	- this will be zero to indicate this fact
	INT32 RecordNumber;
};

/********************************************************************************************

>	class ExportBitmapList : public List

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/6/96
	Purpose:	Holds a list of Bitmaps that have been exported when we are saving a Version
				2 Native Camelot document out to file.
	SeeAlso:	ExportBitmap; 

********************************************************************************************/

class ExportBitmapList : public List
{
	CC_DECLARE_MEMDUMP(ExportBitmapList)

public:
	~ExportBitmapList();

public:
	ExportBitmap *AddBitmap( KernelBitmap *pBmp,  INT32 RecordNumber);	
};

/********************************************************************************************

>	class ExportedBitmaps : public CCObject

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/6/96
	Purpose:	Encapsulate the process of maintaining an list of Bitmaps already exported 
				from the current document out to file.
	SeeAlso:	NewExportBitmap; ExportBitmapList

********************************************************************************************/

class ExportedBitmaps : public CCObject
{
	CC_DECLARE_MEMDUMP(ExportedBitmaps)

public:
	ExportedBitmaps();
	~ExportedBitmaps();

	// Functions to add a newly exported Bitmap to the list.
	BOOL AddBitmap( KernelBitmap *pNewBmp,  INT32 RecordNumber);

	// Finds if we already have this Bitmap in the list
	ExportBitmap *GetBitmap( KernelBitmap *pSearchBmp);

	// Function for EPS export, so it can enumerate all the bitmaps
	ExportBitmap* EnumerateBitmaps(ExportBitmap* pExpBmp = NULL);

	// How many bitmaps are we exporting ?
	UINT32 GetCount() { return Bitmaps.GetCount(); }

protected:
	// The list of Bitmaps to add
	ExportBitmapList Bitmaps;
};


#endif  // INC_EXPBMP


