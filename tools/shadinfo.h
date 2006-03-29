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
// Header for the Shadow Tool Info Bar implementation

#ifndef INC_SOFTSHADOWTOOLINFO
#define INC_SOFTSHADOWTOOLINFO

#include "bars.h"

#ifdef BUILDSHADOWS

// all necessary #includes are taken care of by pre-compiled header.

class SoftShadowTool;

/********************************************************************************************

>	class ShadowDragData : public CCObject

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/6/99
	Purpose:	For storing the data on given nodes for dragging

********************************************************************************************/
class ShadowDragData : public ListItem
{
	CC_DECLARE_DYNCREATE(ShadowDragData);

public:
	NodeShadowController * pNode;

	MILLIPOINT	PositionShadowX;
	MILLIPOINT	PositionShadowY;
	INT32			ShadowTransparency;
	MILLIPOINT	PenumbraSize;
	double		ShadowAngle;
	float		ShadowHeight;
	float		ShadowScale;
	MILLIPOINT  GlowWidth;
	MILLIPOINT  FeatherWidth;
};

/********************************************************************************************

>	class SoftShadowInfoBarOp : public InformationBarOp

	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/96
	Purpose:	Class for handling the tool's information bar.

********************************************************************************************/

class SoftShadowInfoBarOp : public InformationBarOp
{
	CC_DECLARE_DYNCREATE( SoftShadowInfoBarOp )  
public:
	SoftShadowInfoBarOp();				

	MsgResult 	Message(Msg* Msg);		// All messages to the info bar come through here
	SoftShadowTool *pSoftShadowTool;	// Pointer to the shadow tool
	void Init();						// A few Initialisations

private:
	static Document			*pDoc;
	BOOL					ApplyButton;
	Spread*					CurrentSpread;
	BOOL					SliderDragged;
	NodePath*				CurrentNodePath;

public:
	// the current infobar object - allow static member access
	static InformationBarOp * pSoftShadowInfoBar;

	// Enable the gadgets
	void EnableGadgets(BOOL Enable);	

	// Update functions
	void UpdateGadgetPositionShadowX();
	void UpdateGadgetPositionShadowY();
	void UpdateGadgetBlurSlider(BOOL bUpdateSlider = TRUE);
	void UpdateGadgetTranspSlider(BOOL bUpdateSlider = TRUE);
	void UpdateGadgetPenumbraSize();
	void UpdateAllGadgets();
	void UpdateInfoBar();
	void UpdateGadgetHelp();
	void UpdateGadgetTypeButtons();

	// Update the document and the view according the new position of the shadow
	// Undoable!
	void DoChangeShadPos(INT32 x, INT32 y, BOOL UpdateX, BOOL UpdateY);
	void DoChangeShadowPosIncremental(MILLIPOINT incx, MILLIPOINT incy);
	void DoPenumbraSizeBumps(UINT32 Button);
	void DoChangeShadowDarkness(double dDarkness);
	void DoChangePenumbra(INT32 NewPenumbra);
	void DoChangeWidth(INT32 NewWidth);
	void DoChangeAngle(double Angle);
	void DoChangeHeight(double Height);
	void DoChangeScale(double Scale);

	// Non-undoable functions used during dragging/editing
	void ChangeShadowDarkness(double dDarkness);
	void ChangeShadowPenumbra(INT32 nBlur);
	void ChangeShadowPosIncremental(INT32 incx, INT32 incy);

	// Change the transparency of the selected object
	void SetBitmapUpdate(BOOL OnOrOff);
	BOOL SetPenumbraSize(MILLIPOINT PenumSize);

	BOOL ConvertValueToString(String_256 &In, const INT32 value, UnitType type=NOTYPE, TCHAR *unit=_T("pix"));
	BOOL ConvertStringToValue(CGadgetID ID, MILLIPOINT& value);

	// Inputs error
	// Make the error string and then display the error window
	void DoInputError(UINT32 GadgetID);

	// handles the user entering something into the editable control for the blur slider
	void HandleSliderEditChange();

	// handles the user entering something into the transparency slider edit
	void HandleSliderTranspEditChange();

	BOOL DoesWindowExist() { return m_bControlsExist; }

	// used to remove all data from the saved shadow data list
	void DeleteShadowDataList() { ShadowDataList.DeleteAll(); }

	void SaveShadowDragData(List *pList);
	void RestoreShadowDragData(List * pList, BOOL bRegen = FALSE, BOOL bForceRegen = FALSE);

	void HandleShadowTypeChange(ShadowType Type);
	void HandleShadowPosBump(DialogMsg* dmsg, INT32 incx, INT32 incy);

	BOOL SelectShadowsAlone();

private:

	// the stored data list for dragging the slider (so we can restore if necessary)
	List ShadowDataList;

	// the stored data list for dragging a blob
	ShadowDragData m_BlobDragData;

	BOOL m_bControlsExist;

	void HandleNoShadowBtnClicked();
	void HandleWallShadowBtnClicked();
	void HandleFloorShadowBtnClicked();
	void HandleGlowShadowBtnClicked();

	void SetDarknessEditGadget(double dDarkness);
	BOOL GetDarknessEditGadget(double& dDarkness, BOOL &bMany);
	void SetDarknessSlider(double dDarkness);
	double GetDarknessSlider(BOOL* pValid);

public:
PORTNOTE("BiasGain", "Removed use of CBiasGainGadget in SoftShadowInfoBarOp")
#if !defined(EXCLUDE_FROM_XARALX)
	CBiasGainGadget  BiasGainGadget_m;
#endif

private:

	void ChangeProfile(CProfileBiasGain* Profile, CGadgetID GadgetID);	// DoChangeProfile

	// returns FALSE for no profile found
	virtual void HandleProfileSelChangingMsg(CBiasGainGadget& Gadget, CGadgetID ProfileGadgetID);
	CProfileBiasGain* GetProfileFromSelection(CGadgetID GadgetID, BOOL* bMany, BOOL* bAllSameType);

	MILLIPOINT GetGlowSliderValue();
	void SetGlowSliderValue(MILLIPOINT Value);

	BOOL m_bBumping;
};
	

/********************************************************************************************

>	class SoftShadowInfoBarOpCreate : public BarCreate

	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/96
	Purpose:	Class for creating SoftShadowInfoBarOps.
				Derived classes of BarCreate are used by DialogBarOp::ReadBarsFromFile()

********************************************************************************************/

class SoftShadowInfoBarOpCreate : public BarCreate
{
public:
	DialogBarOp* Create() { return (new SoftShadowInfoBarOp); }
};

#endif		// BUILDSHADOWS

#endif 		// INC_SOFTSHADOWTOOLINFO

