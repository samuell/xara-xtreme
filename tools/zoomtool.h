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
// The Zoom Tool

#ifndef INC_ZOOMTOOL
#define INC_ZOOMTOOL

//#include "tool.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "ccmaths.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "doccoord.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "binds.h" - in camtypes.h [AUTOMATICALLY REMOVED]

class Cursor;
class Spread;
class ZoomInfoBarOp;
class OpZoom;



/********************************************************************************************
>	class ZoomTool : public Tool_v1

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/7/93
	Purpose:	This class is to allow the user to Zoom in and out of the document
				at will.  It provides an interface to the tool system for the zoom
				mouse-drag Operation.
	SeeAlso:	class OpZoom
********************************************************************************************/

class CCAPI ZoomTool : public Tool_v1
{
private:

	// Allow memory dumps.
	CC_DECLARE_MEMDUMP(ZoomTool);

public:

	// Creation & destruction
	ZoomTool();
	virtual ~ZoomTool();
	virtual BOOL Init();

	virtual void Describe(void* InfoPtr);
	virtual UINT32 GetID();
	
	// Event Handlers
	virtual void SelectChange(BOOL fIsSelected);
	virtual BOOL OnKeyPress(KeyPress* pKey);
	virtual void OnMouseMove(DocCoord dcMousePos, Spread* pSpread, ClickModifiers mods);
	virtual void OnClick(DocCoord, ClickType, ClickModifiers, Spread*);
	virtual BOOL GetStatusLineText(String_256* ptext, Spread* pSpread,
								   DocCoord DocPos, ClickModifiers ClickMods);
	void ValidateStatusText();

	// Used by helper classes.
	static BOOL IsSelectedTool();
	static void InvalidateStatusText();

private:

	BOOL				Inited;				// TRUE when initialisation has been attempted
	Cursor*				pcZoomCursor;		// pointer to this tool's cursor
	Cursor*				pcZoomOutCursor;	// pointer to this tool's zoom out cursor
	Cursor*				MyCurrentCursor;				// so I don't flip cursor too often

#ifdef RALPH

	Cursor*				pcHotLinkCursor;		// Graham 20/9/96: The "pointing hand" cursor
//	Cursor*				MyCurrentCursor;				// so I don't flip cursor too often
	void				ChangeCursor(Cursor* cursor);	// function to change cursor
//	BOOL				OnKeyPress(KeyPress* pKey);

#endif //Ralph

	INT32				CurrentCursorID;	// ID of the current cursor on the stack
	ZoomInfoBarOp*		pZoomInfoBarOp;		// points to our info bar
	String_256*			psStatusText;		// text "mask" for status-bar text	
	BOOL				bStatusTextValid;	// whether status-bar text needs updating

	static TCHAR	   *FamilyName;			// The Tools Family Name
	static TCHAR	   *ToolName;			// The Tool Name
	static TCHAR	   *Purpose;			// What the tool is for
	static TCHAR	   *Author;				// Who wrote it
	static BOOL			bIsActive;			// Whether the zoom tool is "selected"
};

#endif   // !INC_ZOOMTOOL
