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
// Header file for status line class

#ifndef INC_STATUSLINE
#define INC_STATUSLINE

#include "pump.h"
#include "cursor.h"
#include "flags.h"
#include "camframe.h"
#include "monotime.h"
#include "stdbars.h"
//#include "ed.h"

class Spread;
class StringBase;

#define STATUSLINE_SELDESC_DEFAULT		1	// over dialogs, outside app window
#define STATUSLINE_SELDESC_BARDRAG		0
#define STATUSLINE_SELDESC_OTHERDOC		0
#define STATUSLINE_SELDESC_DRAGOPS		1
#define STATUSLINE_SELDESC_SELDOC		1
#define STATUSLINE_SELDESC_COLBAR		1
#define STATUSLINE_SELDESC_STATBAR		1
#define STATUSLINE_SELDESC_BUTTONS		1
#define STATUSLINE_SELDESC_MENUS		1
#define STATUSLINE_SELDESC_PREVIEWDLG	1

enum RenderState { Rendering,Paused,NotRendering,Animate };


/****************************************************************************************
>	class StatusLine : public MessageHandler

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/11/94
	Purpose:	Encapsulate the status line
********************************************************************************************/

class StatusLine : public StandardBar
{
public:
	StatusLine();
	~StatusLine();
	MsgResult Message(Msg* Msg);
	BOOL OnIdle();
	BOOL UpdateText(String_256* ptext, BOOL PrefixSelDesc=TRUE);
	BOOL UpdateTextForColourPicker(String_256* ptext, BOOL PrefixSelDesc=TRUE);
	BOOL UpdateMousePosAndSnap(DocCoord* pDocCoord, Spread* pSpread, DocView* pDocView, BOOL Snapped);
	BOOL UpdateSolidDragIndicator(BOOL bSolidAllowed, BOOL bSolid);
	BOOL SetRenderIndicator(RenderState Action);
	void FlagMousePosNeedsUpdating() { MousePosNeedsUpdatingFlag=TRUE; };
	BOOL MousePosNeedsUpdating() { return MousePosNeedsUpdatingFlag; };
	
	void FlagMousePosPaneNeedsUpdating() {MousePosPaneNeedsResizing=TRUE;};

	// This can be used to override the default behaviour of showing the selection state
	// as a prefix to the status-line.
	static void SetPrefix(const StringBase& str);
	static void SetDefaultPrefix();
	static INT32  GetPrefixWidth();

	// colour picker custom control interface functions ....

	BOOL RestrictStatusLineFunctionsToColourPicker (CWindowID colourPicker);
	BOOL RestoreStatusLineFunctions (CWindowID colourPicker);
	BOOL IsRestrictedAccessToColourPicker () { return restrictStatusLineFunctionsToColourPicker; }

	BOOL LockOutControlHelp () { DoControlHelp = FALSE; return (TRUE); }
	BOOL UnlockControlHelp () { DoControlHelp = TRUE; return (TRUE); }
	BOOL AllowControlHelp () { return (DoControlHelp); }

protected:

	BOOL SetMousePosPaneWidth(Spread* pSpread);
	BOOL RefreshHelpText();
	BOOL GetMousePosText(String_256* pText, DocCoord MousePos, Spread* pSread, INT32 dp=-1);
	BOOL TruncateTextToWidth(String_256* pText, INT32 width);

	Cursor* pSnappedCursor;
	INT32 SnappedCursorID;

protected:

	MonotonicTime TextTimer;
	MonotonicTime RenderTimer;
	BOOL MousePosPaneNeedsResizing;
	BOOL MousePosNeedsUpdatingFlag;
//	BOOL TransparencyNeedsUpdating;
	INT32  RenderAnimationState;
	INT32  MousePosDP;
	INT32 OldMemory;

private:

	// The prefix to display in the status-line - if empty, show the selection state.
	static String_256 m_strPrefix;

	static BOOL restrictStatusLineFunctionsToColourPicker;
	static BOOL DoControlHelp;								// whether controls are allowed
															// to display help text (or not)
	static StatusLine * s_pStatusLine;

public:
	static StatusLine * Get() {return s_pStatusLine;}

// Progress related functions
public:
	BOOL ShowProgress (BOOL Show=TRUE, StringBase *JobDescrip = NULL);

	INT32 GetPercent(void) const {return CurrentPercent;}
	BOOL SetPercent(INT32 Percent, BOOL RedrawBackground = FALSE, StringBase *JobDescrip = NULL);

protected:
	INT32 CurrentPercent;			// The currently displayed percentage value
	String_256 *JobDescription;	// NULL, or pointer to description of current job
	BOOL ProgressShown;
	String_256 StatusText;
	BOOL SetStatusText(const String_256 &text);

	CC_DECLARE_DYNCREATE(StatusLine);              
};

#endif  // INC_STATUSLINE
