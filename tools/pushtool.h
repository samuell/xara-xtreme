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
// Class definition for the push tool

/*
*/


#ifndef INC_PUSHTOOL
#define INC_PUSHTOOL


//#include "tool.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "ops.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "doccoord.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "wrkrect.h"
//#include "binds.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "bars.h"


// Forward references:-
class OpState;			// #include "opdesc.h"
class Cursor;			// #include "csrstack.h"
class ZoomInfoBarOp;	// #include "zoomops.h"


#define OPTOKEN_PUSH _T("PushTool")


/********************************************************************************************

>	class PushTool : public Tool_v1

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/7/93
	Purpose:	This class handles the Push Tool that is used to slide the document
				about in a more convienient way than using the scroll bars. Most of the
				hard work is done in the OpPush (The push operation).

********************************************************************************************/

class CCAPI PushTool : public Tool_v1
{
// Give my name in memory dumps
CC_DECLARE_MEMDUMP(PushTool);

public:
	PushTool();
	virtual ~PushTool();
	
	virtual BOOL Init();
	virtual void Describe(void* InfoPtr);
	virtual UINT32 GetID() { return TOOLID_PUSH; };
	
	// Event Handlers
	virtual void SelectChange(BOOL);
	virtual void OnClick( DocCoord, ClickType, ClickModifiers, Spread* );
	BOOL GetStatusLineText(String_256* ptext, Spread*, DocCoord, ClickModifiers);
	void OnMouseMove(DocCoord, Spread*, ClickModifiers);
	
private:
	Cursor*			pcPushCursor;		// points to our cursor object

	
#ifdef RALPH

	Cursor*				pcHotLinkCursor;		// Graham 20/9/96: The "pointing hand" cursor
	Cursor*				MyCurrentCursor;				// so I don't flip cursor too often
	void				ChangeCursor(Cursor* cursor);	// function to change cursor
//	BOOL				OnKeyPress(KeyPress* pKey);


#endif //Ralph

	INT32 			CurrentCursorID;	// ID of the current cursor on the net
	ZoomInfoBarOp* 	pPushInfoBarOp;		// points to our info-bar operation

	// The descrition of the tool
	static TCHAR* FamilyName;
	static TCHAR* ToolName;
	static TCHAR* Purpose;
	static TCHAR* Author;
};





/********************************************************************************************

>	class OpPush : public Operation

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/7/93
	Purpose:	This is the Push operation that slides the document about

********************************************************************************************/

class OpPush : public Operation
{

CC_DECLARE_DYNCREATE(OpPush);

public:
	// Construction/Destruction
	OpPush();

	// The all important Do functions
	void DoDrag( DocCoord Anchor);
	virtual BOOL SnappingDrag() { return FALSE; }
	
	// Virtual functions needed for the dragging operations to work
	virtual void DragPointerMove( DocCoord PointerPos, ClickModifiers ClickMods, Spread*, BOOL bSolidDrag);
	virtual void DragFinished(	DocCoord Pos, ClickModifiers Mods, Spread*, BOOL Success, BOOL bSolidDrag);
	
	// These functions required for the OpDescriptor class
	static BOOL Declare();
	static OpState GetState(String_256* Description, OpDescriptor*);
	BOOL GetStatusLineText(String_256* ptext, Spread*, DocCoord, ClickModifiers);

private:
	// The Doc point that we clicked on to start the drag
	WorkCoord AnchorPoint;
};

#endif  //INC_PUSHTOOL
