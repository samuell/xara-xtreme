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
// Selector Tool Info bar operations


#ifndef SELINFO_INC
#define SELINFO_INC

//#include "bars.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "blobs.h"

class KeyPress;
class SelChangingMsg;


// This maintains a record of the contents of an edit-field, so it can be detected when
// a field's text has been changed.
struct SelEditRecord
{
	UINT32 		nControlID;
	String_256 	strText;
	BOOL		fIsDirty;
};


/********************************************************************************************
>	class SelectorInfoBarOp : public InformationBarOp

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/4/94
	Purpose:	Class for handling the Selector tool's information bar.
********************************************************************************************/

class SelectorInfoBarOp : public InformationBarOp
{
	CC_DECLARE_DYNCREATE(SelectorInfoBarOp)
	
public:

	// This is public for easy sharing.
	SelectorTool* pSelectorTool;

	// Dummy default constructor for DYNCREATE
	SelectorInfoBarOp();

	// The selector tool calls these to get the info-bar's settings.
	BOOL IsAspectLocked() const;
	BOOL InRotateMode() const;
	BOOL ShouldScaleLines() const;
	BOOL DontConsiderAttrs(void) const;
//	BOOL ShouldLeaveCopy() const;
//	BOOL ShouldTransFills() const;

	// The selector tool calls these to set the info-bar's gadgets.
	void SetAspectLock(BOOL fState);
	void SetRotateMode(BOOL fState);
	void SetScaleLines(BOOL fState);
//	void SetLeaveCopy(BOOL fState);
//	void SetTransFills(BOOL fState);

	// These set etc the edit-fields in the info-bar.
	void SetEdit(CGadgetID gid, INT32 nValue, Spread* pUnitSpread, BOOL fUpdate = TRUE);
	void SetDoubleEdit(CGadgetID gid, double nValue, BOOL fUpdate = TRUE);
	void SetEdit(CGadgetID gid, FIXED16 fxValue, Spread* pUnitSpread, BOOL fUpdate = TRUE);
	void ClearEdit(CGadgetID gid);

	void SetEdit_OnSelectionChange();
	void SetEdit_OnDrag(TransformBoundingData* pData, Spread* pSpread);

	void EnableAllEdits(BOOL fEnabled);
	void EnableGrid(BOOL fEnabled);
	void PaintAllEditsNow();

	void UpdateAllRecords();

	// This reports the given error message to the user, who has entered junk in an
	// edit-field.
	void ReportEditError(UINT32 nErrorMsgID);

protected:

	// All messages to the info bar come through here
	virtual MsgResult Message(Msg* pMessage);

private:

	// For various reasons - none to do with efficiency - these must have direct access
	// to the bar's static data.
	friend BOOL SelectorTool::ReadPrefs();
	friend void SelectorTool::SelectChange(BOOL);
	friend BOOL SelectorTool::OnKeyPress(KeyPress*);
	friend void SelectorTool::HandleButtonUp();
	friend void SelectorTool::FigureUserFeedback(Spread* pSpread,
												DocCoord dcPos,
												ClickModifiers cmods,
												BOOL DoSlowTests,
												String_256* pStr,
												Cursor** ppPointerShape
												);

	// These handle specific messages.
	void HandleCreate(DialogMsg* pDlgMsg);
	BOOL HandleButtonDown(DialogMsg* pDlgMsg);
	void HandleButtonUp(DialogMsg* pDlgMsg);
	void HandleEditCommit(DialogMsg* pDlgMsg);
	void HandleGridButton(DialogMsg* pDlgMsg);
	void HandleBumpClickLinear(UINT32 nEditControlID, INT32 nSign);
	void HandleBumpClickAngle(UINT32 nEditControlID, INT32 nSign, double Min, double Max);

	// This returns a pointer to the SelEditRecord corresponding to the given ID.
	SelEditRecord* GetRecord(UINT32 nEditControlID);

	// These update the edit-field records when the PROGRAM changes their text.
	void UpdateRecord(SelEditRecord* per);
	void UpdateRecord(UINT32 nControlID);

	// This returns TRUE if the given edit-field has been changed since its value was
	// last recorded.  If it has then it will be updated to contain the new, changed, text.
	BOOL HasChangedRecord(INT32 i);

	// These extract the text from the given edit control and convert it to the required
	// (numeric) form.  If they require a Spread* to do this they use the spread of the
	// selection.
	double	   ConvertRecord(INT32 i);				// straight text --> number
	MILLIPOINT ConvertMillipointRecord(INT32 i);		// text/units --> millipoints
	FIXED16    ConvertFixedpointRecord(INT32 i);		// text/units --> fixed-point

	// The selector info bar has eight edit-fields.
	SelEditRecord aerEdits[8];
	BOOL fConvertedOK;

	// Data members.  These must be static so that the preferences system can use them
	// even if it saves them after this object is destroyed.
	static BOOL fLockAspect;
	static BOOL fRotateMode;
	static BOOL fScaleLines;
//	static BOOL fLeaveCopy;
//	static BOOL fTransFills;
};
	


/********************************************************************************************
>	class SelectorInfoBarOpCreate : public BarCreate

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/4/94
	Purpose:	Class for creating SelectorInfoBarOps.
				Derived classes of BarCreate are used by DialogBarOp::ReadBarsFromFile()
********************************************************************************************/

class SelectorInfoBarOpCreate : public BarCreate
{
public:
	virtual DialogBarOp* Create();
};

#endif	// !SELINFO_INC
