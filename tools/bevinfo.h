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
// FreeHand Tool Info bar operations


#ifndef FREEINFO_INC
#define FREEINFO_INC

#include "bars.h"
class BevelTool;
class BevelInfo;
class BevelBlob;

/********************************************************************************************

>	class BevelInfoBarOp : public InformationBarOp

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/2/99
	Purpose:	Class for handling the Bevel tool's information bar.

********************************************************************************************/

class BevelInfoBarOp : public InformationBarOp
{
	CC_DECLARE_DYNCREATE( BevelInfoBarOp )  
public:
	// Dummy default constructor for DYNCREATE
	BevelInfoBarOp();

	// All messages to the info bar come through here
	MsgResult 	Message(Msg* Msg);

	// Functions to allow the tool to talk to the info bar
	void SetToolActiveState(BOOL IsToolActive);
	void SetRetroState(BOOL CanRetroFit);
	void SetRetroString();

	virtual void UpdateState(); // Called during idle processing to disable controls

	// this updates the controls depending on the current selection
	void UpdateControls();

	// updates the variables depending on the slider if bIntoVariables is TRUE,
	// if bIntoVariables is FALSE then updates the slider depending on the variables
	void SliderUpdate(BOOL bIntoVariables, BOOL bTestMany = TRUE);

	// Keep a pointer to the Freehand tool in here
	BevelTool	*	pTool;
	BOOL 			CanRetroFit;

	// Since we get Selection Changing message before the controls are made
	// we need to keep a flag to indicate if the controls have been built
	BOOL 			ControlsExist;

	// creates/updates bevel on selected objects
	void DoBevel();

	// slider values
	// public so external classes can set them
	INT32 m_Depth;
	INT32 m_LightAngle;
	INT32 m_Contrast;
	INT32 m_SliderComboIndex;
	INT32 m_LightTilt;

	// sets up a bevelinfo class dependent on the settings of the dialog
	// public because the tool needs to use this too for the blob dragging code
	void SetUpBevelInfo(BevelInfo * pBevelInfo);

	// used by the bevel controller node to determine if the bevel info bar
	// is active or not
	static BevelInfoBarOp * GetCurrentBevelInfoBar() { return pCurrentBar; }

	// starts the drag on a particular blob
	void BeginDrag(BevelBlob * pBlob);

	// deals with pointer movement messages when dragging
	void DragPointerMove( DocCoord PointerPos, ClickModifiers ClickMods, 
								   Spread* pSpread, BOOL bSolidDrag);

	// finishes the drag & updates the variables in the particular blobs list
	void DragFinished( DocCoord PointerPos, ClickModifiers ClickMods, 
								Spread* pSpread, BOOL Success, BOOL bSolidDrag);

	// returns TRUE for being in the middle of a drag
	BOOL AmDragging() { return m_bAmDragging; }

	// returns TRUE for dragging the slider
	BOOL AmDraggingSlider() { return m_bAmDraggingSlider; }

	void UpdateInnerOuterButtons(BOOL bMany, BOOL bOuter, BOOL bNoSelection);

	// when the slider is a depth slider, these set & get its values (the slider
	// is logarithmic)
	MILLIPOINT GetSliderDepth();
	void SetSliderDepth(MILLIPOINT Depth);

	static BevelInfoBarOp * pCurrentBar;

	BOOL m_BetweenViews;

private:
	// same as for filltool
	void SetGadgetWritable(INT32 id, BOOL enable);

	// conversion functions for the edit box
	BOOL ConvertValueToString(String_256 &In, const INT32 value, UnitType type=NOTYPE);
	BOOL ConvertStringToValue(CGadgetID ID,INT32 &value);

	void HandleBevelTypeChanged();
	void HandleSliderChanged();
	void HandleBevelDirectionChanged();

	void HandleBevelDirectionsToInner();
	void HandleBevelDirectionsToOuter();

	/////////////////////////////////////////////////////////////////////////////////////////
	// Karim 11/09/2000
	// New methods to support the new join-type infobar buttons.

	void HandleJoinTypeMitreClicked();
	void HandleJoinTypeRoundClicked();
	void HandleJoinTypeBevelClicked();
	void ChangeJoinType(JointType jt);
	void UpdateJoinTypeControls();
	void EnableDisableJoinTypeControls();

	/////////////////////////////////////////////////////////////////////////////////////////

	// control enabling stuff
	BOOL m_bControlsEnabled;
	BOOL m_bComboChanged;

	void RedrawBlobs(BOOL bAngle = TRUE, BOOL bTilt = FALSE);

	BevelBlob * m_pDragBlob;

	// the angle of the blob before dragging
	INT32 m_BlobAngle;

	BOOL m_bAmDragging;

	void SetBlobNodesAngle(INT32 Angle);

	// does the AttributeSelected with the given parameters
	void DoBevelAttrChange(BevelInfo *pBI);

	// has the user set the inner/outer ?
	BOOL m_bUserSet;
	INT32 m_UserIndent;

	// functions for loading strings into combos
	void LoadBevelTypeComboStrings(BOOL bMany);
	void LoadBevelDirectionComboStrings(BOOL bMany);

	BOOL m_bButtonIsInner;
	BOOL m_bButtonIsOuter;

	BOOL m_bAmDraggingSlider;

	MILLIPOINT m_LastDragWidth;
};
	

/********************************************************************************************

>	class BevelInfoBarOpCreate : public BarCreate

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/4/94
	Purpose:	Class for creating BevelInfoBarOps.
				Derived classes of BarCreate are used by DialogBarOp::ReadBarsFromFile()

********************************************************************************************/

class BevelInfoBarOpCreate : public BarCreate
{
public:
	DialogBarOp*	Create() { return (new BevelInfoBarOp); }
};



#endif	// FREEINFO_INC






