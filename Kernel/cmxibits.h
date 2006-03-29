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
// bits and pieces for the CMX import filter

#ifndef CMXIBITS_H
#define CMXIBITS_H

#include "list.h"
#include "fixst256.h"
#include "colourix.h"
#include "attr.h"
#include "arrows.h"
#include "cmxistut.h"


class Node;
class Layer;
class CMXImportFilter;
class CMXImportFilterDataSet;
class DocColour;

// class for storing layer info
class CMXImportLayer : public ListItem
{
	CC_DECLARE_DYNAMIC(CMXImportLayer);

public:
	CMXImportLayer();
	~CMXImportLayer();

	// access functions
	Node *GetSubTree(void) {return pSubTree;};
	void UseSubTree(void) {Used = TRUE;};
	Node *GetLastAdded(void) {return pLastNodeAdded;};
	void SetName(StringBase *pString) {LayerName = *pString;};

	// stuff for adding bits
	BOOL AddNode(Node *pNode);
	BOOL StartGroup(void);
	BOOL EndGroup(void);

	// handy bits
	Layer *UseSubTreeAsLayer(void);

private:
	Node *pSubTree;
	Node *pLastNodeAdded;
	String_256 LayerName;
	BOOL Used;
	Node *pGroup;
	INT32 GroupLevel;
};

#define cmxiSTACKTYPE_NOTHING		0
#define cmxiSTACKTYPE_TRANSFORM		1
#define cmxiSTACKTYPE_CLIPPER		2

class CMXImportStack;

// class for stackable items
class CMXImportStackableItem : public ListItem
{
	CC_DECLARE_DYNAMIC(CMXImportStackableItem);

public:
	virtual INT32 GetStackType(void) = 0;

	// function to be called when it's popped on a stack (after being added to the list)
	virtual BOOL PlacedOnStack(CMXImportStack *pStack) {return TRUE;};
	// function to be called to activate this stack item
	virtual BOOL Activate(CMXImportStack *) = 0;
	// function to be called to deactivate this stack item
	// if it's the last thing on the stack, IsLastItemOnStack is set
	// to let the thingy know that another item isn't going to be activated
	virtual BOOL Deactivate(CMXImportStack *, BOOL IsLastItemOnStack) = 0;
	// called when the thingy is being removed from the stack
	virtual BOOL RemovedFromStack(CMXImportStack *pStack) {return TRUE;};
};

// stack class
class CMXImportStack : public List
{
	CC_DECLARE_DYNAMIC(CMXImportStack);

public:
	CMXImportStack(INT32 St, CMXImportFilter *pFil) {StackType = St; pTheFilter = pFil;};
	~CMXImportStack();

	CMXImportFilter *GetFilter(void) {return pTheFilter;};

	BOOL Push(CMXImportStackableItem *pItem);
	CMXImportStackableItem *Pop(void);

private:
	INT32 StackType;
	CMXImportFilter *pTheFilter;
};

// transform class
class CMXImportTransform : public CMXImportStackableItem
{
	CC_DECLARE_DYNAMIC(CMXImportTransform);

public:
	CMXImportTransform(Matrix *pTheMatrix, BOOL Concat) {TheMatrix = *pTheMatrix; Concatenate = Concat;};

	INT32 GetStackType(void) {return cmxiSTACKTYPE_TRANSFORM;};

	BOOL PlacedOnStack(CMXImportStack *pStack);
	BOOL Activate(CMXImportStack *pStack);
	BOOL Deactivate(CMXImportStack *pStack, BOOL IsLastItemOnStack);

private:
	Matrix TheMatrix;			// matrix found in the file
	Matrix CompoundMatrix;		// matrix made from all the previous ones and the base matrix
	BOOL Concatenate;			// whether this matrix is concatonated to previous ones, or just replaces them
};

// transform class
class CMXImportClipper : public CMXImportStackableItem
{
	CC_DECLARE_DYNAMIC(CMXImportClipper);

public:
	CMXImportClipper(Path *ptClipPath, BOOL tIntersect) {pClipPath = ptClipPath; Intersect = tIntersect;};

	INT32 GetStackType(void) {return cmxiSTACKTYPE_CLIPPER;};

	BOOL PlacedOnStack(CMXImportStack *pStack);
	BOOL Activate(CMXImportStack *pStack);
	BOOL Deactivate(CMXImportStack *pStack, BOOL IsLastItemOnStack);

private:
	Path *pClipPath;
	BOOL Intersect;				// whether this path is intersected with the previous one
};

// colour class
class CMXImportColour : public SimpleCCObject
{
public:
	CMXImportColour() {Used = FALSE; pIndexedColour = NULL; pColour = NULL;};
	~CMXImportColour() {delete pColour; if(!Used) delete pIndexedColour;};

	DocColour *GetColour(void) {Used = TRUE; return pColour;};
	BOOL IsUsed(void) {return Used;};
	IndexedColour *GetIndexedColour(void) {return pIndexedColour;};

	// if I use the proper type, none of the files which include this
	// one will compile, and try as I may, I can't get it to work.
	// I don't think this is me being silly, but I've given up on this
	// one and lost the lovely type checking. Shouldn't be too much of 
	// a problem as these functions are only called in one place.
	BOOL Convert(/*cmxiColour*/ void *pCol);

private:
	BOOL Used;
	IndexedColour *pIndexedColour;
	DocColour *pColour;
};

// dot dash pattern class
class CMXImportDotDash : public SimpleCCObject
{
public:
	CMXImportDotDash() {pElements = NULL;};
	~CMXImportDotDash() {if(pElements != NULL) delete [] pElements;};

	// see above reason why this is not properly typed
	BOOL Convert(/*cmxiDotDash*/ void *pDotDash);

	INT32 NDots;
	DashElement *pElements;
};

// arrowhead shape class
class CMXImportArrowShape : public SimpleCCObject
{
public:
	// see above reason why this is not properly typed
	BOOL Convert(/*cmxiArrowShape*/ void *pArrowShape, BOOL Is32Bit);

	ArrowRec Arrow;
	INT32 LineOffset;
};

// arrowheads class
class CMXImportArrowheads : public SimpleCCObject
{
public:
	CMXImportArrowheads() {pStart = NULL; pEnd = NULL;};

	// see above reason why this is not properly typed
	BOOL Convert(/*cmxiArrowheads*/ void *pArrowheads, CMXImportFilterDataSet *Data);

	CMXImportArrowShape *pStart;
	CMXImportArrowShape *pEnd;
};

// outline class
class CMXImportOutline : public SimpleCCObject
{
public:
	CMXImportOutline() {pDots = NULL; pArrowheads = NULL;};

	// see above reason why this is not properly typed
	BOOL Convert(/*cmxiOutline*/ void *pOut, CMXImportFilterDataSet *Data);

	INT32 ColourReference;
	INT32 Width;
	JointType Join;
	LineCapType Cap;
	CMXImportDotDash *pDots;
	CMXImportArrowheads *pArrowheads;
	BOOL NoStroke;
};

// pen class
class CMXImportPen : public SimpleCCObject
{
public:
	// see above reason why this is not properly typed
	BOOL Convert(/*cmxiPen*/ void *pPen);

	INT32 Width;
};

class CMXImportFont : public SimpleCCObject
{
public:
	BOOL Convert(/*cmxiFont*/ void *pFont);
	StringBase*	mFont;
	BOOL mIsItalic;
	BOOL mIsBold;
};


// line style class
class CMXImportLineStyle : public SimpleCCObject
{
public:
	// see above reason why this is not properly typed
	BOOL Convert(/*cmxiLineStyle*/ void *pStyle);

	JointType Join;
	LineCapType Cap;
	BOOL DotDash;
	BOOL NoStroke;
};

// colour list class
class CMXImportColourList : public List
{
	CC_DECLARE_DYNAMIC(CMXImportColourList);

public:
	CMXImportColourList() {};
	~CMXImportColourList() {DeleteAll();};

	BOOL AddColour(INT32 Reference, INT32 Position);
	INT32 GetColourRefClosestToPosition(INT32 Position);
};

// colour list item class
class CMXImportColourListItem : public ListItem
{
	CC_DECLARE_DYNAMIC(CMXImportColourListItem);

public:
	INT32 Reference;
	INT32 Position;
};

class CMXImportProcedure : public SimpleCCObject
{
public:
	CMXImportProcedure() {Position = 0;};
	
	BOOL Read(/*cmxiProcedureIndexEntry*/ void *pEn);

	INT32 Position;
};


#endif // CMXIBITS_H

