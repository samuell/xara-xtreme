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

// Encapsulates a set of colours exported from a file.

#ifndef INC_EXPCOL
#define INC_EXPCOL

#include "list.h"
#include "listitem.h"
#include "colmodel.h"	// PColourRGBT

class ColourListComponent;
class IndexedColour;

/********************************************************************************************

>	class ExportColour : public ListItem

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/5/96
	Purpose:	Represents a colour object that has been saved out into the file already.
				This enables any other references to the colour to be output as we have a
				mechanism to remember that a colour definition has already been saved into
				the file and the record number it was saved under.
	SeeAlso:	ColourListComponent;

********************************************************************************************/

class ExportColour : public ListItem
{
	CC_DECLARE_MEMDUMP(ExportColour)

public:
	// Adds a newly saved colour to the list
	ExportColour( IndexedColour *pNewCol,  INT32 NewRecordNumber);

	// Adds a newly exported simple colour to the list.
	ExportColour( PColourRGBT *pNewRGBCol,  INT32 NewRecordNumber);

	// Pointer to the colour that has been exported.
	IndexedColour *pCol;

	// If it is a simple colour then pCol will be NULL and we will need to save out the RGB
	// colour. This will be saved in a *packed* (8-bit components) colour structure
	PColourRGBT SimpleRGBColour;

	// If saved		- this will be the record number that the colour definition is saved in
	// If unsaved	- this will be zero to indicate this fact
	INT32 RecordNumber;
};

/********************************************************************************************

>	class ExportColourList : public List

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/5/96
	Purpose:	Holds a list of colours that have been exported when we are saving a Version
				2 Native Camelot document out to file.
	SeeAlso:	ExportColour; 

********************************************************************************************/

class ExportColourList : public List
{
	CC_DECLARE_MEMDUMP(ExportColourList)

public:
	~ExportColourList();

public:
	ExportColour *AddColour( IndexedColour *pCol,  INT32 RecordNumber);	
	ExportColour *AddColour( PColourRGBT *pNewRGBCol,  INT32 RecordNumber);	
};

/********************************************************************************************

>	class ExportedColours : public CCObject

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/5/96
	Purpose:	Encapsulate the process of maintaining an list of colours already exported 
				from the current document out to file.
	SeeAlso:	NewExportColour; ExportColourList

********************************************************************************************/

class ExportedColours : public CCObject
{
	CC_DECLARE_MEMDUMP(ExportedColours)

public:
	ExportedColours();
	~ExportedColours();

	// Functions to add a newly exported colour to the list.
	BOOL AddColour( IndexedColour *pNewCol,  INT32 RecordNumber);

	// Functions to add a newly exported simple colour to the list.
	BOOL AddColour( PColourRGBT *pNewRGBCol,  INT32 RecordNumber);

	// Finds if we already have this colour in the list
	ExportColour *GetColour( IndexedColour *pSearchCol);

	// Finds if we already have this simple RGB colour in the list
	ExportColour *GetColour( PColourRGBT *pNewRGBCol);

protected:
	// The list of colours to add
	ExportColourList Colours;
};


#endif  // INC_EXPCOL


