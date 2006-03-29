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

// collist.h - the ColourList class


#ifndef INC_COLLIST
#define INC_COLLIST

#include "errors.h"
#include "list.h"

class BaseDocument;

class IndexedColour;
class Document;



/***********************************************************************************************

>	class ColourList : public List

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Purpose:	A colour list is held by each document to store the list of available
				IndexedColours for use by that document. (This colour list is displayed
				in the ColourBar/ColourGallery via the ColGalDisplayList which references
				colours held in the ColourList).

				The ColourList also provides a couple of special access methods for
				determining the number of visible/undeleted colours in the document
				(deleted colours are still held in this list, but are marked deleted
				for undo purposes - if scanning the ColourList, remember to check
				IndexedColours IsDeleted() flag) -- See the Undeleted list operators
				provided by this class to save you loads of effort.

				The Colour list also includes another simple List of unnamed colours
				which are used for local colour attributes in a document. Unnamed
				colours are simply stored in this list, and are simpler than named 
				colours...

	SeeAlso:	IndexedColour; ColGalDisplayList; BaseDocument; List

***********************************************************************************************/

class ColourList : public List
{
CC_DECLARE_MEMDUMP(ColourList)

public:		// Construction/Initialisation
	ColourList();
	~ColourList();

	void Init(BaseDocument *ParentDocument);

public:		// Overridden List interface
	void AddHead(ListItem *);			// Overridden List members: You MUST use AddItem
	void AddTail(ListItem *);

	BOOL AddItem(IndexedColour *NewColour);	// Add a colour, ensuring it has a unique name


	// Overrides (well, they would be if the base class had made these virtual!) which call
	// the base class after first ensuring the name is unique
	LISTPOS InsertBefore(LISTPOS here, ListItem* item);
	ListItem *InsertBefore(ListItem* here, ListItem* item);

	LISTPOS InsertAfter(LISTPOS here, ListItem* item);
	ListItem *InsertAfter(ListItem* here, ListItem* item);

	// Override DeleteAll to do safe deletion of linked colours
	virtual void DeleteAll();
	

public:		// Special ColourList listy methods
	UINT32 GetUndeletedCount(void);
	IndexedColour *FindUndeletedItem(INT32 Index);
	IndexedColour *GetUndeletedHead(void);
	IndexedColour *GetUndeletedNext(IndexedColour *ThisItem);
		// Count/Find/Head/Next functions which ignore 'deleted' IndexedColours

	inline BaseDocument *GetParentDocument(void);
		// Find the document in which this colour list resides

private:
	BaseDocument *ParentDoc;

public:		// Special linking inheritance support
	IndexedColour **CompileInheritanceArray(IndexedColour *Parent);
		// Compiles a NULL-terminated array of pointers to all colours which are linked
		// to this, both directly and indirectly (i.e. an array listing the entire
		// inheritance 'subtree' from (and including) 'Parent')

	void RemoveLinksToColour(IndexedColour *DeadColour);
		// Ensures no other colours have this colour as a linked parent

	BOOL IsColourInUseInDoc(IndexedColour *TheColour, BOOL IgnoreColourGallery = FALSE);
		// Determines if a colour is used in the document either directly (DocColour Ref)
		// or indirectly (parent/ancestor of a linked/tint colour which is used in the doc)

	void InvalidateInheritanceCache(void);
		// Invalidates the cached results of the above function

private:	// Private variables for inheritance support
	IndexedColour *ArrayParent;				// Current parent of the cached array
	IndexedColour **InheritanceArray;		// Cached array of all offspring of ArrayParent

public:		// Named colour support
	BOOL NamedColourExists( PCTSTR pName );
	BOOL NamedColourExists(const StringBase *pName);
		// Returns TRUE if the given name is in use by any item in this colour list

	BOOL GenerateUniqueColourName(const StringBase *pPrefix, String_64 *pResult);
		// Outputs a name (Based on pPrefix) that is unique in this ColourList
		// Returns TRUE if the output name is different from the pPrefix string

private:	// Private data for named colour support
	INT32 LastUnnamedColour;					// Kept to optimise unnamed colour naming

public:		// Unnamed colour storage support
	inline List *GetUnnamedColours(void);

private:	// Private data for unnamed colours
	List UnnamedColours;
};



/**********************************************************************************************

>	inline BaseDocument *ColourList::GetParentDocument(void)

	Author: 	Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/11/94
	Returns:	The document in which this colour list resides
	Purpose:	To find the parent document of this colour list

**********************************************************************************************/

BaseDocument *ColourList::GetParentDocument(void)
{
	ERROR3IF(ParentDoc == NULL, "Uninitialised ColourList detected!");
	return(ParentDoc);
}



/**********************************************************************************************

>	inline List *ColourList::GetUnnamedColours(void)

	Author: 	Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/3/95
	Returns:	The storage-list containing this document's unnamed colours
	Purpose:	To find the unnamed colours for this document

**********************************************************************************************/

List *ColourList::GetUnnamedColours(void)
{
	return(&UnnamedColours);
}


#endif

