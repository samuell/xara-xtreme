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

// Line Component class implementation - abstract sort-of base class from 
// which to derive line components like strokes, brushes.

#ifndef INC_LINECOMP
#define INC_LINECOMP


#include "doccomp.h"
#include "camfiltr.h"
#include "linedef.h"
class LineDefinition;
class BaseCamelotFilter;
class CXaraFileRecord;
class CamelotRecordHandler;
class Node;


/* NOTES:
This superclass was created so that both the old vector stroking system and the new brush system
could be stored and retrieved in an identical manner.  Unfortunately the StrokeComponent class
was written making extensive use of friend classes calling static functions, this did not lend 
itself particularly well to inheritance so what we currently have is a load of functions in the 
LineComponent class which are ripped from StrokeComponent, however the Brush component is the only
one that makes use of them as all the stroke code is in place with these static functions.
If I have time to rework the stroke component code then I will try to sort this out. */

/********************************************************************************************

>	typedef UINT32 LineHandle;

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/99

	Purpose:	A LineHandle is an ID that is used to identify each line in the m_pLineArray.
				This saves having lots of pointers hanging around.

	SeeAlso:	BrushComponent; BrushDefinition

********************************************************************************************/

typedef UINT32 LineHandle;

const LineHandle LineHandle_NoLine = 0xffffffff;


/********************************************************************************************

>	class LineComponent : public DocComponent

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/2/97

	Purpose:	A document component for handling Line definitions.

				LineComponents share a global cache of available Line types.
				Individual instances serve as managers for the list, mainly providing
				import/export facilities for Line definitions.

	SeeAlso:	DocComponent

********************************************************************************************/

class LineComponent : public DocComponent
{
	CC_DECLARE_DYNAMIC(LineComponent)

public:
	LineComponent();
	virtual ~LineComponent();

	// must be static as it is called after all the documents are deleted
	static void DeleteList(void) { DeleteLineList();}	
protected:		// Internal functions 
	static void DeleteLineList(void);
				// One-time deinitialisation, called on exit

public:
	virtual LineHandle AddNewItem(LineDefinition *pLine, BOOL AskName = FALSE);
				// Add a new Line definition to the global list

	virtual LineDefinition *FindDefinition(UINT32 Handle, BOOL Dummy = FALSE);
				// Finds the definition of a given Line

	UINT32    GetNumberOfLines() { return m_Used;}
public:			// External V2 native file import/export handlers
	virtual BOOL StartImport(BaseCamelotFilter *pFilter);
	virtual BOOL EndImport(BaseCamelotFilter *pFilter, BOOL Success);
	
	// Start export must be overridden in the base class as it needs to
	// write out the TAG specific to the line being exported
	virtual BOOL StartExport(BaseCamelotFilter *pFilter) = 0;
	
	virtual BOOL EndExport(BaseCamelotFilter *pFilter, BOOL Success);


public:			// Internal file format handlers - called from our RecordHandler in strkattr.cpp
	
	// Start import line is provided here as it is merely concerned with setting up the spread 
	//to house the ink objects
	static BOOL StartImportLine(CamelotRecordHandler *pHandler, CXaraFileRecord *pRecord);

	// End import line must be overriden as it must create an instance of the line definition
	BOOL EndImportLine(CamelotRecordHandler *pHandler) {return TRUE;}
	

	LineHandle FindImportedLine(UINT32 ImportedHandle);
				// Finds the internal handle of the Line definition with the given imported handle

	virtual BOOL ExportLine(BaseCamelotFilter *pFilter, LineHandle Handle) = 0;
				// Exports a Line definition (if necessary) - must be overridden

	virtual BOOL		NameIsUnique(const String_32& NameString);
				// checks the definitions to see if this name is unique

	virtual String_32   GetUniqueName(String_32* pName = NULL) = 0;  

protected:			// Global Line list & management functions, must be overridden
	virtual BOOL ExpandArray(void) = 0;

protected:
	static LineDefinition ** m_pLineArray;	// Global array of available Line definitions
	static UINT32 m_CurrentSize;				// Number of entries allocated to the array
	static UINT32 m_Used;						// Number of entries used in the array (index of 1st free slot)

protected:			// Import records
	static UINT32 m_ImportHandle;				// Record the data associated with the record currently 
	static UINT32 m_ImportFlags;				// being imported
	static UINT32 m_ImportData1;
	static UINT32 m_ImportData2;
	static InsertTreeContext *m_pImportPreviousContext;
	static Node  *m_pImportNewLine;
	static String_32 m_ImportedName;
};


#endif
