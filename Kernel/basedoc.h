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
/*
 * */
// The BaseDocument class

#ifndef INC_BASEDOC
#define INC_BASEDOC

#include "listitem.h"
#include "colcarry.h"

class DocComponent;
class ColourList;
class Node;

class ExportHint;

/********************************************************************************************

< TAG_LAST_VALID

	Comment:	This tag value indicate the range of valid tags that can be used in a
				document.  0 is not used, so the tags 1 to TAG_LAST_VALID are the only ones
				that can be used in a document.
	SeeAlso:	Node::GetTag; TAG_NOT_IN_DOC

********************************************************************************************/ 

#define TAG_LAST_VALID 	(0xFFFFFFFE)


/********************************************************************************************

< TAG_NOT_IN_DOC

	Comment:	This tag value is used to indicate that a node is not in a document yet.
	SeeAlso:	Node::GetTag; TAG_LAST_VALID

********************************************************************************************/ 

#define TAG_NOT_IN_DOC 	(0xFFFFFFFF)


/*********************************************************************************************
>	class BaseDocument: public ListItem

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com> 
	Created:	20/07/94
	Purpose:	A simple base class document object 
	Errors:		-
*********************************************************************************************/

class BaseDocument : public ListItem
{
	CC_DECLARE_DYNAMIC(BaseDocument);

///////////////////////////////////////////////////////////////////////////////////////
// Construction & destruction

public:
	BaseDocument(BOOL fHide = TRUE);
	virtual ~BaseDocument();
	BOOL Init();			// Initialise document, cf. Document::Init
	
	BOOL FixColourRefsAfterCopy(BaseDocument* SourceDocument);

	// Post import related funcs
	void PostImport();
	BOOL CheckReferences();
	BOOL CheckFontSubstitution();
	BOOL CheckEffects();

	ListRange* GetEffectsList(UINT32 iMaxItems);

///////////////////////////////////////////////////////////////////////////////////////
// Visibility - determining if a document is hidden, or is the clipboard
// (Hidden/clipboard docs have no associated Views, and are not in the application
// list of documents. Some code may need to be careful when in the clipboard.
// Hidden/Clipboard documents may or may not have a CCamDoc attached)

protected:
	BOOL IsHidden;

public:
	BOOL IsAHiddenDoc() const				{ return IsHidden; };
			// Returns TRUE if this document is hidden (is not shown in any UI)

	BOOL IsNotAHiddenDoc() const			{ return !IsHidden; };
			// Returns TRUE if this document is NOT hidden (is not shown in any UI)

public:
	ExportHint* GetExportHint(void) { return(m_pExportHint); };

protected:
	ExportHint* m_pExportHint;



///////////////////////////////////////////////////////////////////////////////////////
// All about Nodes . . .

protected:
	Node* TreeStart;						// address of first object in our tree
	INT32 NodesInTree;						// count of objects in our tree

public:
	Node* GetFirstNode() const;

	void DeleteContents(); 

	TAG NewTag();

	void IncNodeCount();
	void DecNodeCount();

	INT32 NodesInDocument() const;


///////////////////////////////////////////////////////////////////////////////////////
// The document components for this document

public:
	BOOL InitDocComponents(void);		// Internal init/deiit of components. Also
	void DestroyDocComponents(void);	// used by when clearing the clipboard doc.

	void AddDocComponent(DocComponent *);
	DocComponent *GetDocComponent(CCRuntimeClass *);
	DocComponent *GetDocComponentKindOf(CCRuntimeClass *);
	DocComponent *EnumerateDocComponents(DocComponent*);


///////////////////////////////////////////////////////////////////////////////////////
// Colour: Default colour context array, document IndexedColour list

protected:
	ColourContextArray	DefaultColourContexts;

public:
	ColourContextArray *GetDefaultColourContexts(void);
	ColourList *GetIndexedColours(void);

	virtual BOOL StartComponentCopy();
	virtual BOOL EndComponentCopy(); 
	virtual void AbortComponentCopy();  

protected:
	List *DocComponents;

private:
	TAG TagCounter;						// unique ID for nodes
};

#endif	// INC_BASEDOC
