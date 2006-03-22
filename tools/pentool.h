// $Id: pentool.h 662 2006-03-14 21:31:49Z alex $
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
// The Pen Tool
// Created by Mike on 19/9/94

#ifndef INC_PENTOOL
#define INC_PENTOOL

#include "tool.h"
#include "doccoord.h"
#include "wrkrect.h"
#include "binds.h"
#include "bars.h"
//#include "viewrc.h"
#include "nodepath.h"
#include "penedit.h"

class Cursor;
class PenToolInfoBarOp;

#define OPTOKEN_PENTOOL _T("PenTool")


enum penstate
{
	IS_Undefined,
	IS_MoveTo,
	IS_DragTo,
};

enum penopstate
{
	OS_Undefined,
	OS_EditInternal,
	OS_CreatePath,
	OS_AddElement,
	OS_ClosePath
};

typedef enum penclick
{
 	PenNewPath, 
 	PenOnPoint,
 	PenAddSegment,
	PenEditInternalMove,
	PenClosePath,
	PenReshapeLine
};



/********************************************************************************************

>	class PenTool : public Tool_v1

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/9/94
	Purpose:	This class handles the Pen Tool that is used to draw lines and curves via
				clicking and draging on the workarea.

********************************************************************************************/

class CCAPI PenTool : public Tool_v1
{
// Give my name in memory dumps
CC_DECLARE_MEMDUMP(PenTool);

public:
	PenTool();
	~PenTool();
	
	BOOL Init();
	void Describe(void* InfoPtr);
	UINT32 GetID() { return TOOLID_PEN; };
	
	// Event Handlers
	void SelectChange(BOOL);
	virtual void OnClick( DocCoord, ClickType, ClickModifiers, Spread* );
	virtual void RenderToolBlobs(Spread*, DocRect*);
	virtual BOOL GetStatusLineText(String_256*, Spread*, DocCoord, ClickModifiers);
	virtual void OnMouseMove(DocCoord, Spread*, ClickModifiers);
	virtual BOOL OnKeyPress(KeyPress* pKeyPress);
	void GenerateStatusLineText(NodePath* pNodePath, penclick WhatToDo, String_256* pStatusMsg);
	
	void SetInternalState();
	void ClearInternalState();

	BOOL OverPathEnd(DocCoord, Spread*, NodePath**);
	penclick DetermineClickEffect(DocCoord, Spread*, NodePath**);
	void RemoveSelection(DocCoord PointerPos, Spread *pSpread, NodePath* pNodePath=NULL);
	void ClearPath();
	void ClearOp();
	void CreateNewPath();
	void AddElementToPath();
	void CloseWithPath();

	penopstate GetPenOpState() const;

private:
	void ClickOnEndPoint(ClickType Click, ClickModifiers ClickMods, Spread *pSpread, NodePath* pNodePath );
	BOOL CreatePenCursors();
	void DeletePenCursors();

private:
	static 	TCHAR* FamilyName;		// The Tools Family Name
	static 	TCHAR* ToolName;		// The Tool Name
	static 	TCHAR* Purpose;			// What the tool is for
	static 	TCHAR* Author;			// Who wrote it

	penstate 	CurrPenState;		// current internal tool state
	penopstate	CurrPenOpState;		// current internal tool op state

	Path		EditPath;			// temporary path for eor editing
	ControlPts  EditHandles;		// handle coordinates for eoring

	NodePath*	pNodePath;
	INT32		NodeIndex;

	// The tools cursors
	Cursor* 	pcPenCursor;
	Cursor* 	pcPenAdjustCursor;
	Cursor*		pcPenReshapeCursor;
	Cursor*		pcMoveBezCursor;
	Cursor* 	MyCurrentCursor;

	INT32 		CurrentCursorID;				// ID of the current cursor on the stack
	void		ChangeCursor(Cursor* cursor);	// function to change cursor

	// The info bar
	PenToolInfoBarOp* pPenInfoBarOp;


};



/********************************************************************************************

>	class PenToolInfoBarOp : public InformationBarOp

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/9/94
	Purpose:	Class for handling the pen tool's information bar.

********************************************************************************************/

class PenToolInfoBarOp : public InformationBarOp
{
 	CC_DECLARE_DYNCREATE(PenToolInfoBarOp)
public:
	PenToolInfoBarOp() {DlgResID = _R(IDD_PENTOOLBAR);};	// Dummy default constructor for DYNCREATE
	MsgResult Message(Msg*);			// All messages to the info bar come through here
	// void	InitControls();				// Initialises the controls in the info bar
	// void	EnableControls();			// Enables/disables the controls in the info bar
	// virtual void UpdateState();		// Overrides the empty function in InformationBarOp

	PenTool* pPenTool;					// Pointer to instance of pen tool

};


/********************************************************************************************

>	class PenToolInfoBarOpCreate : public BarCreate

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/9/94
	Purpose:	Class for creating PenToolInfoBarOps.
				Derived classes of BarCreate are used by DialogBarOp::ReadBarsFromFile()

********************************************************************************************/

class PenToolInfoBarOpCreate : public BarCreate
{
public:
	DialogBarOp*	Create() { return (new PenToolInfoBarOp); }
};



#endif  //INC_PENTOOL
