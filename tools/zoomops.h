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
//
// Zoom tool Operations & OpDescriptors.

#ifndef ZOOMOPS_H
#define ZOOMOPS_H

#include "opdragbx.h"
//#include "bars.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "cursor.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "wrkcoord.h" - in camtypes.h [AUTOMATICALLY REMOVED]


// These define tokens for each operation associated with "zooming".
#define OPTOKEN_ZOOMCOMBO 		TEXT("ZoomCombo")
#define OPTOKEN_ZOOMPREV		TEXT("ZoomPrev")
#define OPTOKEN_ZOOMSPREAD		TEXT("ZoomSpread")
#define OPTOKEN_ZOOMDRAWING 	TEXT("ZoomDrawing")
#define OPTOKEN_ZOOMSELECTED	TEXT("ZoomSelect")
#define OPTOKEN_ZOOMRECT		TEXT("ZoomRect")
#define OPTOKEN_ZOOMIN			TEXT("ZoomIn")
#define OPTOKEN_ZOOMOUT			TEXT("ZoomOut")
#define OPTOKEN_ZOOMTO100		TEXT("Zoom100")
#define OPTOKEN_ZOOMTO200		TEXT("Zoom200")
#define OPTOKEN_ZOOMTO300		TEXT("Zoom300")
#define OPTOKEN_ZOOMTO400		TEXT("Zoom400")

// Graeme (11/11/99) - Definition of the minimum size of the drag rectangle.
#define ZOOM_MIN_DRAG			5000

// Forward references.
class StringBase;
class String_256;
class DocView;
class Spread;
class WorkRect;
class DocCoord;
class DocRect;
class OpDescControlMsg;
class List;


// Preset zoom factors.
const INT32 cZoomTableSize       = 13;	// how many preset scale factors (percentages)
const INT32 cDefaultZoomIndex    = 8;		// the array index of the default scale, 100%

const INT32 cUninitZoomIndex 	   = -2;	// array index not set yet by zoom tool
const INT32 cFractionalZoomIndex = -1;	// scale lies between two zoom table entries

const INT32 cComboDivider1	   = cZoomTableSize;
const INT32 cFitToSpreadIndex	   = cZoomTableSize + 1;
const INT32 cFitToDrawingIndex   = cZoomTableSize + 2;
const INT32 cFitToSelectedIndex  = cZoomTableSize + 3;
const INT32 cPreviousZoomIndex   = cZoomTableSize + 4;
const INT32 cComboDivider2	   = cZoomTableSize + 5;



/********************************************************************************************
>	class OpZoom : public Operation

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/4/95
	Purpose:	This operation can perform zooms in and out of the current DocView, either
				by an immediate call to the ZoomIn/ZoomOut/ZoomInOnRect/ZoomAtPoint
				functions, or by a call to the DoDrag function, which starts a mouse drag
				to define a zoom box.
********************************************************************************************/

class OpZoom : public OpCentredDragBox
{
public:
	// Creation & destruction.
	OpZoom();

	// A do function (at INT32 last) so that menu and keyboard short cuts work.
	virtual	void Do(OpDescriptor*);

	// Nothing causes so much grief when changing the zoom code as the correct conversion
	// from spread coordinates to 64-bit view coordinates.  Here are some routines that
	// really work.
	static void SpreadToWork(Spread* pspdIn, const DocCoord& dcIn, WorkCoord* pwcOut);
	static void SpreadToWork(Spread* pspdIn, const DocRect& drIn, WorkRect* pwrOut);

	// These returns the given preset zoom factor as a percentage or a scale factor.
	static INT32 GetPresetZoomPercent(INT32 nPreset);
	static FIXED16 GetPresetZoomScale(INT32 nPreset);

	// This starts a drag (NB. hides the base-class OpDragState::DoDrag function).
	BOOL DoDrag(Spread* pStartSpread, const DocCoord& dcStartPos, ClickModifiers cmods);
	virtual BOOL SnappingDrag()				{ return FALSE; }

	// Zoom in or out by the next scale factor in the ZoomTable table (also zoom direct to 100%).  
	// These are normally called on click events by the OpZoom user.
	void ZoomOut(const WorkCoord& wcZoom, BOOL fEndOp = TRUE);
	void ZoomOut(Spread* pZoomSpread, const DocCoord& dcZoomPos, BOOL fEndOp = TRUE);

	void ZoomIn(const WorkCoord& wcZoom, BOOL fEndOp = TRUE);
	void ZoomIn(Spread* pZoomSpread, const DocCoord& dcZoomPos, BOOL fEndOp = TRUE);

	void ZoomTo( const WorkCoord& wcZoom, INT32 Percent, BOOL fEndOp = TRUE);

	// Zoom so that the given rectangle within the current view completely fills the
	// current view's view-port.
	void ZoomInOnRect(const WorkRect& wrZoom, BOOL fEndOp = TRUE);
	void ZoomInOnRect(Spread* pZoomSpread, const DocRect& dcZoomRect, BOOL fEndOp = TRUE);
		
	// Zoom in or out to show the given point in the current view at the given scale factor.
	void ZoomAtPoint(const WorkCoord& wcZoom, FIXED16 fxNewScaleFactor, BOOL fEndOp = TRUE);
	void ZoomAtPoint(Spread* pZoomSpread, const DocCoord& dcZoomPos, FIXED16 fxNewScaleFactor,
					 BOOL fEndOp = TRUE);

	// As ZoomAtPoint, but forces the auto-centre function off...
	void MouseWheelZoomAtPoint(const WorkCoord& wcZoom, FIXED16 fxNewScalePercent, BOOL fEndOp = TRUE);

	// This function registers all zoom operations/op-descriptors.
	static BOOL Declare();

private:
	// OpDragBox implementation.
	BOOL OnDragStarted(Spread*, const DocCoord&, ClickModifiers);
	BOOL OnPointerMoved(Spread*, const DocRect&, ClickModifiers);
	BOOL OnDragEnded(Spread* pBoxSpread, const DocRect& drDragBox,
					 ClickModifiers cmods, BOOL fDragOK);

	// Optionally calls OpDragBox::CalcDragBox or OpCentredDragBox::CalcDragBox,
	// depending on the preferences.
	DocRect CalcDragBox(const DocCoord& dcStartPos, const DocCoord& dcMousePos) const;

	// Called when the status line text needs refreshing.
	BOOL GetStatusLineText(String_256* ptext, Spread*, DocCoord, ClickModifiers);

	// Data members.
	Cursor    m_csrZoomIn; 							// the zoom in, zoom out and zoom drag
	Cursor    m_csrZoomOut;							// cursors.
	Cursor    m_csrZoomDrag;
	INT32   	  m_ncsrSaveDrag;						// saved old cursor ID when dragging
	WorkCoord m_wcStartPos;							// where the drag started, in workspace
	BOOL	  m_fStatusTextShown;					// flag to remind us to update text

	// Preset zoom factor percentages.
	static INT32 ZoomTable[cZoomTableSize];			

	// Preferences.
	static BOOL m_fRadialZoomDragBox;				// do drag from centre to corner
	static BOOL	m_fFixZoomClickPoint;				// fix on-screen the clicked point?
	static UINT32 m_nfShowZoomCursors;				// whether to flash cursor on zooming

	// Run-time typing.
	CC_DECLARE_DYNCREATE(OpZoom);
};

/********************************************************************************************
>	class OpZoomIn : public OpZoom

	Author:		Mark_Howitt (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/11/00
	Purpose:	This op has been written so that we can use the NumPad Plus key to fire off
				a zoom in operation. We can`t use the zoom op directly as the hotkey code
				calls the Do() function instead of the DoWithParam(). What this class does
				is basically call the ZoomOps DoWithParam() from it`s Do() function.
********************************************************************************************/
class OpZoomIn : public OpZoom
{
public:
	// Creation & destruction.
	OpZoomIn();

	// A do function (at INT32 last) so that menu and keyboard short cuts work.
	virtual	void Do(OpDescriptor*);
	static BOOL Init();
	static OpState GetState(String_256* Description, OpDescriptor*);

private:
	// Run-time typing.
	CC_DECLARE_DYNCREATE(OpZoomIn);
};

/********************************************************************************************
>	class OpZoomOut : public OpZoom

	Author:		Mark_Howitt (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/11/00
	Purpose:	This op has been written so that we can use the NumPad Minus key to fire off
				a zoom out operation. We can`t use the zoom op directly as the hotkey code
				calls the Do() function instead of the DoWithParam(). What this class does
				is basically call the ZoomOps DoWithParam() from it`s Do() function.
********************************************************************************************/
class OpZoomOut : public OpZoom
{
public:
	// Creation & destruction.
	OpZoomOut();

	// A do function (at INT32 last) so that menu and keyboard short cuts work.
	virtual	void Do(OpDescriptor*);
	static BOOL Init();
	static OpState GetState(String_256* Description, OpDescriptor*);

private:
	// Run-time typing.
	CC_DECLARE_DYNCREATE(OpZoomOut);
};

/********************************************************************************************
>	class OpZoomTo100 : public OpZoom

	Author:		Luke_Hart (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/09/06
	Purpose:	This op has been written so that we can use the NumPad Plus key to fire off
				a zoom in operation. We can`t use the zoom op directly as the hotkey code
				calls the Do() function instead of the DoWithParam(). What this class does
				is basically call the ZoomOps DoWithParam() from it`s Do() function.
********************************************************************************************/
class OpZoomTo100 : public OpZoom
{
public:
	// Creation & destruction.
	OpZoomTo100();

	// A do function (at long last) so that menu and keyboard short cuts work.
	virtual	void Do(OpDescriptor*);
	static BOOL Init();
	static OpState GetState(String_256* Description, OpDescriptor*);

private:
	// Run-time typing.
	CC_DECLARE_DYNCREATE(OpZoomTo100);
};

/********************************************************************************************
>	class OpZoomTo200 : public OpZoom

	Author:		Luke_Hart (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/09/06
	Purpose:	This op has been written so that we can use the NumPad Plus key to fire off
				a zoom in operation. We can`t use the zoom op directly as the hotkey code
				calls the Do() function instead of the DoWithParam(). What this class does
				is basically call the ZoomOps DoWithParam() from it`s Do() function.
********************************************************************************************/
class OpZoomTo200 : public OpZoom
{
public:
	// Creation & destruction.
	OpZoomTo200();

	// A do function (at long last) so that menu and keyboard short cuts work.
	virtual	void Do(OpDescriptor*);
	static BOOL Init();
	static OpState GetState(String_256* Description, OpDescriptor*);

private:
	// Run-time typing.
	CC_DECLARE_DYNCREATE(OpZoomTo200);
};

/********************************************************************************************
>	class OpZoomTo300 : public OpZoom

	Author:		Luke_Hart (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/09/06
	Purpose:	This op has been written so that we can use the NumPad Plus key to fire off
				a zoom in operation. We can`t use the zoom op directly as the hotkey code
				calls the Do() function instead of the DoWithParam(). What this class does
				is basically call the ZoomOps DoWithParam() from it`s Do() function.
********************************************************************************************/
class OpZoomTo300 : public OpZoom
{
public:
	// Creation & destruction.
	OpZoomTo300();

	// A do function (at long last) so that menu and keyboard short cuts work.
	virtual	void Do(OpDescriptor*);
	static BOOL Init();
	static OpState GetState(String_256* Description, OpDescriptor*);

private:
	// Run-time typing.
	CC_DECLARE_DYNCREATE(OpZoomTo300);
};

/********************************************************************************************
>	class OpZoomTo400 : public OpZoom

	Author:		Luke_Hart (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/09/06
	Purpose:	This op has been written so that we can use the NumPad Plus key to fire off
				a zoom in operation. We can`t use the zoom op directly as the hotkey code
				calls the Do() function instead of the DoWithParam(). What this class does
				is basically call the ZoomOps DoWithParam() from it`s Do() function.
********************************************************************************************/
class OpZoomTo400 : public OpZoom
{
public:
	// Creation & destruction.
	OpZoomTo400();

	// A do function (at long last) so that menu and keyboard short cuts work.
	virtual	void Do(OpDescriptor*);
	static BOOL Init();
	static OpState GetState(String_256* Description, OpDescriptor*);

private:
	// Run-time typing.
	CC_DECLARE_DYNCREATE(OpZoomTo400);
};

/********************************************************************************************
>	class OpZoomDescriptor : public OpDescriptor

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/5/93
	Purpose:	Base class to the two zoom tool OpDescriptors, OpZoomComboDescriptor
				and OpZoomButtonDescriptor.  Simplifies the parameters required by a
				constructor call, and provides default GetState and message handlers.
********************************************************************************************/

class OpZoomDescriptor : public OpDescriptor
{
public:
	OpZoomDescriptor(const TCHAR* pcszToken, UINT32 wStatusID, UINT32 wHelpID, UINT32 wBubbleID, UINT32 resourceID=0, UINT32 controlID=0);
	virtual MsgResult Message(Msg* pMsg);
	virtual BOOL IsAvailable();

	static void FakeInvoke(TCHAR* pszToken);
	static void FakeZoomToRect(const DocRect& rect);

	virtual BOOL DoZoom(OpZoom * pZoomOp);

protected:
	virtual Spread* GetSpread(DocView* pDocView) const;
	virtual DocRect GetRect(Spread* pSpread);
	virtual void AdjustRect(DocRect* pRect) const;

	virtual MsgResult HandleButtonMsg(DialogOp* popdlgThis, CGadgetID gidThis);
	static OpState GetState(String_256* psName, OpDescriptor* popdCandidate);
};



/********************************************************************************************
>	class OpZoomFitSpreadDescriptor : public OpZoomDescriptor

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/5/93
	Purpose:	OpDescriptor for the zoom tool's "Fit to spread" button.
********************************************************************************************/

class OpZoomFitSpreadDescriptor : public OpZoomDescriptor
{
public:
	OpZoomFitSpreadDescriptor();

protected:
	virtual DocRect GetRect(Spread* pSpread);
	virtual void AdjustRect(DocRect* pRect) const;

private:
	BOOL m_DontScale;
};



/********************************************************************************************
>	class OpZoomFitDrawingDescriptor : public OpZoomDescriptor

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/5/93
	Purpose:	OpDescriptor for the zoom tool's "Fit to drawing" button.
********************************************************************************************/

class OpZoomFitDrawingDescriptor : public OpZoomDescriptor
{
public:
	OpZoomFitDrawingDescriptor();

protected:
	virtual DocRect GetRect(Spread* pSpread);
};



/********************************************************************************************
>	class OpZoomFitSelectedDescriptor : public OpZoomDescriptor

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/5/93
	Purpose:	OpDescriptor for the zoom tool's "Fit to selected" button.
********************************************************************************************/

class OpZoomFitSelectedDescriptor : public OpZoomDescriptor
{
public:
	OpZoomFitSelectedDescriptor();

protected:
	virtual Spread* GetSpread(DocView* pDocView) const;
	virtual DocRect GetRect(Spread* pSpread);
	virtual void AdjustRect(DocRect* pRect) const;
};


/********************************************************************************************
>	class OpZoomFitRectDescriptor : public OpZoomDescriptor

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/5/93
	Purpose:	OpDescriptor used to zoom to a specific rectangle.
********************************************************************************************/

class OpZoomFitRectDescriptor : public OpZoomDescriptor
{
public:
	OpZoomFitRectDescriptor();
	void SetZoomRect(const DocRect& rect);

protected:
	virtual DocRect GetRect(Spread* pSpread);

private:
	DocRect TheRect;
};


/********************************************************************************************
>	class OpZoomPrevZoomDescriptor : public OpZoomDescriptor

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/5/93
	Purpose:	OpDescriptor for the zoom tool's "Previous Zoom" button.
********************************************************************************************/

class OpZoomPrevZoomDescriptor : public OpZoomDescriptor
{
public:
	OpZoomPrevZoomDescriptor();
	
	static void SaveZoom(DocView* pDocView);

	virtual BOOL DoZoom(OpZoom * pZoomOp);

private:
	virtual BOOL IsAvailable();
	virtual MsgResult HandleButtonMsg(DialogOp* popdlgThis, CGadgetID gidThis);
};



/********************************************************************************************
>	class OpZoomComboDescriptor : public OpZoomDescriptor

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/5/93
	Purpose:	OpDescriptor for the zoom tool's combo-box containing scale factors.
********************************************************************************************/

class OpZoomComboDescriptor : public OpZoomDescriptor
{
public:
	OpZoomComboDescriptor();
	
	virtual MsgResult Message(Msg* pMsg);
	static BOOL Update(BOOL fRefreshList = FALSE);

private:
	MsgResult HandleCreateMsg(DialogOp* popdlgThis, CGadgetID gidThis);
	MsgResult HandleViewChangeMsg(DocView* pSelectedDocView);
	void OnSelectionChange(OpDescControlMsg* pCtrlMsg, List* pGadgetList);
	
	BOOL RefreshList();
	BOOL UpdateAllCombos(String_256* pStr);
	BOOL UpdateComboWithViewScale(DocView* pDocView);

	virtual BOOL IsAvailable();
	static BOOL StringToLong(const StringBase& pszIn, INT32* pnOut);
};



/********************************************************************************************
>	class ZoomInfoBarOp : public InformationBarOp

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/3/95
	Purpose:	Handles creation/destruction and message handling for the zoom tool
				info-bar.
	SeeAlso:	-
********************************************************************************************/

class ZoomInfoBarOp : public InformationBarOp
{
	CC_DECLARE_DYNCREATE(ZoomInfoBarOp);

public:
	ZoomInfoBarOp(ResourceID r=_R(IDD_ZOOMINFO)) {DlgResID=r;}
	MsgResult Message(Msg* pMsg);

};
	

/********************************************************************************************
>	class ZoomInfoBarOpCreate : public BarCreate

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/5/94
	Purpose:	Class for creating (dummy) InformationBarOps.
				Derived classes of BarCreate are used by DialogBarOp::ReadBarsFromFile()
********************************************************************************************/

class ZoomInfoBarOpCreate : public BarCreate
{
public:
	DialogBarOp* Create();
};

#endif	// !ZOOMOPS_H
