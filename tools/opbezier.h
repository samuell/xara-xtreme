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
// Operations supplied by the Line tool

#ifndef INC_OPBEZIER
#define INC_OPBEZIER

#include "doccoord.h"
#include "undoop.h"

class BezierTool;
class Cursor;

#define OPTOKEN_INSERTFLOATER	_T("OpInsertFloater")
#define OPTOKEN_REMOVEFLOATER	_T("OpRemoveFloater")

/********************************************************************************************

>	class OpSelectPathPoints : public UndoableOperation

	Author:		Jim_Lynn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/6/94
	Purpose:	This is the operation that marquee selects points in paths

********************************************************************************************/

class OpSelectPathPoints : public UndoableOperation
{

CC_DECLARE_DYNCREATE(OpSelectPathPoints);

public:
	// Construction/Destruction
	OpSelectPathPoints();

	// The all important Do functions
	void DoDrag( DocCoord Anchor, Spread*, BOOL isAdjust, BezierTool* toolptr, ClickModifiers );
	
	// Virtual functions needed for the dragging operations to work
	virtual void DragPointerMove( DocCoord PointerPos, ClickModifiers ClickMods, Spread*, BOOL bSolidDrag);
	virtual void DragFinished(	DocCoord PointerPos, 
								ClickModifiers ClickMods, 
								Spread*, 
								BOOL Success, BOOL bSolidDrag);

	// Some Render functions to will draw the EORed path
	void RenderDragBlobs( DocRect, Spread*, BOOL bSolidDrag);
		
	// These functions required for the OpDescriptor class
	static BOOL Declare();
	static OpState GetState(String_256* Description, OpDescriptor*);
	static BOOL Init();

	static BOOL DragInProgress();

private:
	DocCoord	StartPoint;			// The start of the rectangle
	Spread *	StartSpread;
	DocCoord	PreviousPoint;		// So that we don't update too often.
	BOOL		AdjustDrag;			// Is the drag adjust or not?
	BezierTool*	BezToolPtr;			// Points back to the bezier tool
	ClickModifiers Mods;			// which modifiers were in evidence from the original click
	static BOOL	DragUnderway;		// This op is not re-entrant.

	Cursor* pcMarqueeCursor;				// Normal freehand cursor
	void	ChangeCursor(Cursor* cursor);	// function to change cursor
	void	PushCursor(Cursor* cursor);		// Push this cursor onto the cursor stack
	void	PopCursor();					// Pop our cursor off the stack
	void	RemoveDragBlobs(BOOL, BOOL);	// Restores the selection blobs to pre-drag state
	void	RenderRubberRect(DocRect* Rect);
	void	RenderRubberSelection(RenderRegion* pRegion);
};



/********************************************************************************************

>	class OpRemoveFloater : public UndoableOperation

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	06/12/94
	Purpose:	This is the operation that the line tool uses to remove the floating endpoint
				in an undoable kind of way.

********************************************************************************************/

class OpRemoveFloater : public UndoableOperation
{

CC_DECLARE_DYNCREATE(OpRemoveFloater);

public:
	OpRemoveFloater();
	static BOOL Declare();
	static OpState GetState(String_256* Description, OpDescriptor*);
	void DoWithParam(OpDescriptor*, OpParam* pOpParam);
};



/********************************************************************************************

>	class OpInsertFloater : public UndoableOperation

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	06/12/94
	Purpose:	This is the operation that the line tool uses to insert the floating endpoint
				in an undoable kind of way.

********************************************************************************************/

class OpInsertFloater : public UndoableOperation
{

CC_DECLARE_DYNCREATE(OpInsertFloater);

public:
	OpInsertFloater();
	static BOOL Declare();
	static OpState GetState(String_256* Description, OpDescriptor*);
	void DoWithParam(OpDescriptor*, OpParam* pOpParam);
};



/********************************************************************************************

>	class InsertFloaterParam : public OpParam

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	06/12/94
	Purpose:	This is the OpParam object passed to OpInsertFloater to position the floating
				endpoint

********************************************************************************************/
class InsertFloaterParam : public OpParam
{
CC_DECLARE_MEMDUMP(InsertFloaterParam);

public:
	InsertFloaterParam(DocCoord* pInsertPos, Spread* pInsertSpread, BezierTool* pLineTool, Document* pWorkDoc);

	DocCoord	InsertPos;
	Spread*		InsertSpread;
	BezierTool*	pTool;
	Document*	pOpWorkingDocument;
};



/********************************************************************************************

>	class InsertFloaterAction : public Action

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/12/94
	Purpose:	An action to provide the Line tool with an undoable way of inserting the 
				floating endpoint.  This isn't stored in the document, which makes life
				tricky.
	SeeAlso:	

********************************************************************************************/

class InsertFloaterAction : public Action
{

CC_DECLARE_DYNCREATE(InsertFloaterAction)

public:
	InsertFloaterAction();
	~InsertFloaterAction();
	virtual ActionCode Execute();
	static ActionCode Init( Operation* pOp,
							ActionList* pActionList,
							Action** NewAction);
	static ActionCode DoInsert( Operation* pOp,
								ActionList* pActionList,
								BezierTool* pTool,
								DocCoord*	pPoint,
								Spread*		pSpread);
	// Data members
	BezierTool*	pLineTool;
	DocCoord	Point;
	Spread*		pSpread;
};


/********************************************************************************************

>	class RemoveFloaterAction : public Action

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/12/94
	Purpose:	An action to provide the Line tool with an undoable way of removing the 
				floating endpoint.  This isn't stored in the document, which makes life
				tricky.
	SeeAlso:	

********************************************************************************************/

class RemoveFloaterAction : public Action
{

CC_DECLARE_DYNCREATE(RemoveFloaterAction)

public:
	RemoveFloaterAction();
	~RemoveFloaterAction();
	virtual ActionCode Execute();
	static ActionCode Init( Operation* pOp,
							ActionList* pActionList,
							BezierTool* pTool,
							Action** NewAction);
	static ActionCode DoRemove( Operation* pOp,
								ActionList* pActionList,
								BezierTool* pTool);
protected:
	BezierTool*	pLineTool;
};


#endif 		// INC_OPBEZIER




