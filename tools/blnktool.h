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
// Header for the blank tool implementation

#ifndef INC_BLANKTOOL
#define INC_BLANKTOOL

#include "tool.h"
#include "doccoord.h"
#include "bars.h"

class Cursor;
class Spread;
class BlankInfoBarOp;


/********************************************************************************************

>	class BlankTool : public Tool_v1

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/10/94
	Purpose:	A template tool class

********************************************************************************************/

class CCAPI BlankTool : public Tool_v1
{
// Give tool name in memory dumps
CC_DECLARE_MEMDUMP(BlankTool);

public:
	BlankTool();
	~BlankTool();

	// Standard calls your tool must provide
	BOOL Init();
	void Describe(void* InfoPtr);
	UINT32 GetID() { return TOOLID_BLANK; };	// <****** CHANGE THIS TO YOUR TOOL'S ID!!! (defined in kernel\tool.h)
	
	// Some very common event handlers
	void SelectChange(BOOL isSelected);
	void OnClick( DocCoord, ClickType, ClickModifiers, Spread* );
	void OnMouseMove(DocCoord PointerPos,Spread* pSpread, ClickModifiers ClickMods);
	void RenderToolBlobs(Spread* pSpread,DocRect* pDocRect);
	BOOL GetStatusLineText(String_256* ptext, Spread* pSpread, DocCoord DocPos, ClickModifiers ClickMods);

	// Some useful static functions
	static BlankInfoBarOp* 	GetBlankInfoBarOp()  { return s_pBlankInfoBarOp; }
	static BOOL 			IsCurrentTool() 	 { return s_bCurrentTool; }

private:
	void DisplayStatusBarHelp(DocCoord DocPos, Spread* pSpread, ClickModifiers ClickMods);
	void GetCurrentStatusText(String_256* ptext, Spread* pSpread, DocCoord DocPos, ClickModifiers ClickMods);

	BOOL CreateCursors();							// Create  your tool's cursors in this func
	void DestroyCursors();							// Destroy your tool's cursors in this func

	static BOOL 			s_bCurrentTool;			// Can be useful to keep a "is my tool currenmt?" flag
	static BlankInfoBarOp*	s_pBlankInfoBarOp;		// Ptr to your tool's infobar

	Cursor*					m_pcNormalBlankCursor;	// Your standard cursor to use when your tool becomes active
	Cursor*					m_pcCurrentCursor;		// The cursor your tool is currently displaying
	INT32 					m_CurrentCursorID;		// ID of the current cursor you're displaying

	// Standard tool static vars
	static 	LPTSTR FamilyName;						// The Tools Family Name
	static 	LPTSTR ToolName;						// The Tool Name
	static 	LPTSTR Purpose;							// What the tool is for
	static 	LPTSTR Author;							// Who wrote it
};


//-----------------------------------------------
//-----------------------------------------------
//-----------------------------------------------
//-----------------------------------------------


/********************************************************************************************

>	class BlankInfoBarOp : public InformationBarOp

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/10/94
	Purpose:	Class for handling the tool's information bar.

********************************************************************************************/

class BlankInfoBarOp : public InformationBarOp
{
	CC_DECLARE_DYNCREATE( BlankInfoBarOp )  
public:
	BlankInfoBarOp(BlankTool* pTool=NULL)
	{
		m_pBlankTool = pTool;
		DlgResID = _R(IDD_BLANKTOOLBAR);
	}

	MsgResult 	Message(Msg* Msg);	// All messages to the info bar come through here

private:
	BlankTool*	m_pBlankTool;
};
	

#endif 		// INC_BLANKTOOL
