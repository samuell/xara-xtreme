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

// Implementation of the following classes
//
//	ColourEditDlg	Colour editor dialogue box

/*
*/



//-----------------------------------------------------------------------------------------
// Include files

#include "camtypes.h"

//#include "app.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "attrmgr.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "barsdlgs.h"
#include "bubbleid.h" 
#include "camelot.h" 
#include "ccolbar.h"
#include "colcontx.h"
#include "coldlog.h"
#include "coldrop.h"
#include "colmenu.h"
#include "colormgr.h"
#include "colourix.h"
#include "colpick.h"
#include "ctrlhelp.h"
//#include "cursor.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "dlgmgr.h"
//#include "dlgtypes.h"  - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "document.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "docview.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "dragcol.h"
#include "dragmgr.h"
#include "dragpick.h"
//#include "ensure.h"   - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "errors.h"  - in camtypes.h [AUTOMATICALLY REMOVED]
#include "helpuser.h"		// For HelpUser()
//#include "ink.h"			// For NodeRenderableInk - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "jason.h"
#include "keypress.h"		// For constrain, escape, and OnKeyPress stuff
#include "lineattr.h"		// For AttrStrokeColour
//#include "msg.h"  - in camtypes.h [AUTOMATICALLY REMOVED]
#include "newcol.h"			// New colour dlg
#include "nodedoc.h"
#include "oilprog.h"		// For Beep()
//#include "ops.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "optsmsgs.h"		// For OptionsChangingMsg
#include "progress.h"
//#include "resource.h"
#include "scunit.h"			// For colour units
#include "optsunit.h"		// For default colour units
#include "sgcolour.h"		// For ColourNameDlg
//#include "will2.h"			// For string resources (v1.1)

#include "ccdc.h"			// For render-into-dialogue support
#include "dlgcol.h"
//#include "fillattr.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "grnddib.h"
#include "palman.h"

//#include "spread.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "objchge.h"

// include files necessary for correctly supporting camelots CColourPicker custom control
//#include "mainfrm.h"
#include "statline.h"
#include "ophist.h"
#include "layer.h"

const ResourceID ColourEditDlg::IDD = _R(IDD_COLOUREDITDLG);

//-----------------------------------------------------------------------------------------

// The kernel colour picker - called via the winoil ColourPicker class
CC_IMPLEMENT_DYNCREATE(ColourEditDlg, DialogOp)
CC_IMPLEMENT_DYNAMIC(ColEditorDragTarget, KernelDragTarget)
CC_IMPLEMENT_DYNCREATE(ColEditorDragInfo, ColourDragInformation)
CC_IMPLEMENT_DYNCREATE(OpMakeColourLocalToFrame, UndoableOperation)


// CAM_DEBUG_NEW is defined a couple of pages further down...

//-----------------------------------------------------------------------------------------

/********************************************************************************************

	Preference:		ColourEditorAutoModel
	Section:		Displays
	Range:			TRUE or FALSE
	Purpose:		If TRUE, the colour editor will automatically switch the display colour
					model whenever it switches to a new LOCAL colour (to that colour's model)
					If FALSE, the only time the display colour model will change is when
					the user explicitly sets a different colour model from the dialogue.
	Notes:			Normal preference, available from the options dialogue.
	SeeAlso:		ColourEditDlg

********************************************************************************************/

/********************************************************************************************

	Preference:		ColourEditorAutoModelN
	Section:		Displays
	Range:			TRUE or FALSE
	Purpose:		If TRUE, the colour editor will automatically switch the display colour
					model whenever it switches to a new NAMED colour (Colour STYLE) (to that
					colour's model)
					If FALSE, the only time the display colour model will change is when
					the user explicitly sets a different colour model from the dialogue.
	Notes:			Normal preference, available from the options dialogue.
	SeeAlso:		ColourEditDlg

********************************************************************************************/

/********************************************************************************************

	Preference:		ColourEditorDisplayModel
	Section:		Displays
	Range:			1..15 (COLOURMODEL_XXXX enum). If an invalid setting is chosen,
					then COLOURMODEL_HSVT will be selected by default
	Purpose:		Retains a 'memory' of the last used Colour Editor display model
					between sessions in Camelot. Used for LOCAL COLOURS
	Notes:			Internal 'memory' preference, not directly settable by the user
	SeeAlso:		ColourEditDlg; ColourModel

********************************************************************************************/

/********************************************************************************************

	Preference:		ColourEditorDisplayModelN
	Section:		Displays
	Range:			1..15 (COLOURMODEL_XXXX enum). If an invalid setting is chosen,
					then COLOURMODEL_HSVT will be selected by default
	Purpose:		Retains a 'memory' of the last used Colour Editor display model
					between sessions in Camelot. used for COLOUR STYLES
	Notes:			Internal 'memory' preference, not directly settable by the user
	SeeAlso:		ColourEditDlg; ColourModel

********************************************************************************************/

/********************************************************************************************

	Preference:		ColourEditorFolded
	Section:		Displays
	Range:			TRUE or FALSE
	Purpose:		Retains a 'memory' of the colour editor 'folded' state between
					sessions in Camelot.
	Notes:			Internal 'memory' preference, not directly settable by the user
	SeeAlso:		ColourEditDlg

********************************************************************************************/

/********************************************************************************************

	Preference:		ColourEditorSplitLine
	Section:		Displays
	Range:			TRUE or FALSE
	Purpose:		Determines if the sliders in the colour editor show a 'split line'
					display. If FALSE, a simple display is given where each slider just
					shows the component being set (e.g. in RGB, the Red slider shades
					between black and red)
					If TRUE, the split mode is enabled, where the slider shows the normal
					grad-fill in the top half (black to red), but fills the bottom half
					with the range of colours which will result from changing that 
					component of the edited colour (i.e. at any point on the slider it
					shows what colour you will get if you click on that spot)
	Notes:			Secret squirrel preference! THis may become available in the options
					dialogue if it is decided that it is useful.
	SeeAlso:		ColourEditDlg

********************************************************************************************/

/********************************************************************************************

	Preference:		ColourEditorUse3D
	Section:		Displays
	Range:			TRUE or FALSE
	Purpose:		If FALSE, the RGB/CMYK colour pickers in the colour editor will be
					simple sliders. If TRUE, the '3d colour cube' display will be used instead
	SeeAlso:		ColourEditDlg

********************************************************************************************/

/********************************************************************************************

	Preference:		ColourEditorHSVHueTop
	Section:		Displays
	Range:			TRUE or FALSE
	Purpose:		If TRUE, the HSV colour picker shows the SV Pane with full hue at the top
					(as all version did prior to Xara X1)
					If FALSE, the HSV colour picker shows the SV Pane with full hue at the bottom
	SeeAlso:		ColourEditDlg

********************************************************************************************/

/********************************************************************************************

	Preference:		UsePrintCMYK
	Section:		Displays
	Range:			TRUE or FALSE
	Purpose:		If TRUE, use print profile to do RGB-CMYK colour conversion
	SeeAlso:		ColourEditDlg

********************************************************************************************/


// PREFERENCES
INT32 ColourEditDlg::DefaultDisplayModel  = (INT32) COLOURMODEL_HSVT;
INT32 ColourEditDlg::DefaultDisplayModelN = (INT32) COLOURMODEL_HSVT;
INT32 ColourEditDlg::AutoModelChange	= FALSE;			// When new colour set, change Displaymodel (LOCAL)

// WEBSTER - markn 31/1/97
// Don't autochange named colour display model (i.e. set to FALSE instead of TRUE
#ifndef WEBSTER
INT32 ColourEditDlg::AutoModelChangeN = TRUE;			// When new colour set, change Displaymodel (STYLE)
#else
INT32 ColourEditDlg::AutoModelChangeN = FALSE;			// When new colour set, change Displaymodel (STYLE)
#endif // WEBSTER

INT32 ColourEditDlg::Folded			= TRUE;				// Is the dlg folded?
INT32 ColourEditDlg::SplitLineDisplay	= FALSE;			// Show sliders in split gradfill way
INT32 ColourEditDlg::Use3DDisplay		= FALSE;			// Show sliders rather than colourcube

// If FALSE turns the SV square upside down compared to previous versions
BOOL ColourEditDlg::bHSVHueAtTop = TRUE;

BOOL ColourEditDlg::bUsePrintCMYK = TRUE;

//-----------------------------------------------------------------------------------------

// Statics
BOOL ColourEditDlg::EditingLineColour = FALSE;			// Default to editing local fill colours
BOOL ColourEditDlg::UpdateOnNextIdle  = FALSE;			// Callback - Update the dialogue on the next idle event

BOOL ColourEditDlg::resetColPickPos;					// by default - we don't need to reset
RECT ColourEditDlg::colPickOrigRect;					// store the original location of
														// custom colour picker control
BOOL ColourEditDlg::needColPickHidden;
BOOL ColourEditDlg::colPickHidden;

AbortColourState ColourEditDlg::abortColourState;

BOOL ColourEditDlg::EscapeKeyWasPressed = FALSE;		// record the fact of whether the
														// escape key has been pressed.

// NOTE:  the above variable is used as a bodge fix.  For some reason, ColourEditDlg::OnKeyPress ()
//		  (which internally calls SetNewValueFromMousePos ()) seems to be failing to detect that
//		  the escape key is pressed.  Since OnKeyPress already knows this, lets record this fact
//		  and take things from there.  Although this is a bodge fix, this new method actually
//		  appears to work better than the original (I can't find the problem) one.

/***********************************************************************************************

>	class ColourEditDlgParam : public OpParam

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/10/94

	Purpose:	The OpParam to use when Invoke()ing the ColourEditDlg
				You shouldn't ever need to use this - just call the static function
				ColourEditDlg::InvokeDialog if you want a colour editor

	Scope:		private
	SeeAlso:	ColourEditDlg::InvokeDialog; ColourPicker::EditColour

***********************************************************************************************/

class ColourEditDlgParam : public OpParam
{
CC_DECLARE_MEMDUMP(ColourEditDlgParam)

public:
	ColourEditDlgParam(ColourList *TheParentList, IndexedColour *TheColourToEdit)	\
					: OpParam(0, 0)													\
			{ ParentList = TheParentList; ColourToEdit = TheColourToEdit; };

	ColourList		*ParentList;
	IndexedColour	*ColourToEdit;
};


CC_IMPLEMENT_MEMDUMP(ColourEditDlgParam, OpParam)



//-----------------------------------------------------------------------------------------
// This must be defined AFTER all CC_IMPLEMENT_DYNCREATE calls
#define new CAM_DEBUG_NEW


//-----------------------------------------------------------------------------------------
// Local (file or class scope) constants/statics

const INT32 CROSSRADIUS = 6000;		// Marker cross is 1/6th of an inch across (millipoints)
const INT32 PCROSSRADIUS = 3500;		// Marker cross to indicate parent position is smaller
const INT32 ZSLIDERSIZE = 8000;		// Size of Z slider in 3d mode
const INT32 PATCHSIZE   = 13500;		// Size of original/current colour patch square

const CDlgMode ColourEditDlg::Mode = MODELESS; 			// Mode of the dialog  

ColourEditDlg *ColourEditDlg::TheEditor = NULL;			// THE instantiation of this dlg
ColourModel ColourEditDlg::DisplayModel = COLOURMODEL_HSVT;	// Default DisplayModel
INT32 ColourEditDlg::ColourPickerMode = 0;				// Default picker axis orientation


static CGadgetID TintGadgetIDs[] =	// NULL terminated list of special tint gadgets
{
	_R(IDC_EDIT_TINTNAME),				// This first item is used to determine positioning
	_R(IDC_EDIT_TINT),
	_R(IDC_EDIT_TINTPERCENT),
//	_R(IDC_EDIT_TINTSLIDER),
	_R(IDC_EDIT_SHADE),
	_R(IDC_EDIT_SHADEPERCENT),
	0
};


static CGadgetID LinkGadgetIDs[] =	// NULL terminated list of special linked-colour gadgets
{
	_R(IDC_EDIT_INHERITNAME),			// This first item is used to determine positioning
	_R(IDC_EDIT_INHERIT1),
	_R(IDC_EDIT_INHERIT2),
	_R(IDC_EDIT_INHERIT3),
	_R(IDC_EDIT_INHERIT4),
	0
};


static CGadgetID OtherGadgetIDs[] =	// NULL terminated list of all non tint/link gadgets
{									// which should be shaded to shade the dialogue
	_R(IDC_EDIT_NAMEMENU),
	_R(IDC_EDIT_COMPONENT1),
	_R(IDC_EDIT_COMPONENT2),
	_R(IDC_EDIT_COMPONENT3),
	_R(IDC_EDIT_COMPONENT4),
	_R(IDC_EDIT_WEBHEX),
	_R(IDC_NAME_COMPONENT1),
	_R(IDC_NAME_COMPONENT2),
	_R(IDC_NAME_COMPONENT3),
	_R(IDC_NAME_COMPONENT4),
	_R(IDC_NAME_WEBHEX),
	_R(IDC_EDIT_COLMODEL),
	_R(IDC_EDIT_3D),
	_R(IDC_EDIT_COLTYPE),
	_R(IDC_EDIT_MAKESTYLE),
#ifndef WEBSTER
	_R(IDC_EDIT_ADVANCED),	// WEBSTER - markn 11/12/96
#endif // WEBSTER
	_R(IDC_EDIT_DROPMENU),
	_R(IDC_EDIT_PARENTCOL),
	_R(IDC_EDIT_PARENTNAME),
	_R(IDC_EDIT_216ONLY),
//	WEBSTER-ranbirr-27/03/97
	_R(IDC_EDIT_NOCOLOUR),
	_R(IDC_EDIT_LINEFILL),
	_R(IDC_MAKE_LOCAL),
	_R(IDC_EDIT_RENAME),
	_R(IDC_COLOURPICKER),
	0
};

typedef struct
{
	CGadgetID	Gadget;						// Gadget ID
	UINT32		BubbleID;					//Bubble help ID for this gadget
} GadgetHelpInfo;

static GadgetHelpInfo GadgetHelp[] = 
{
	{ _R(IDC_EDIT_DROPMENU),	_R(IDS_EDITBH_MENU) },
	{ _R(IDC_EDIT_COLMODEL),	_R(IDS_EDITBH_COLMODEL) },
	{ _R(IDC_EDIT_NAMEMENU),	_R(IDS_EDITBH_NAME) },
	{ _R(IDC_EDIT_COLTYPE),		_R(IDS_EDITBH_COLTYPE) },
	{ _R(IDC_EDIT_INHERIT1),	_R(IDS_EDITBH_INHERIT) },
	{ _R(IDC_EDIT_INHERIT2),	_R(IDS_EDITBH_INHERIT) },
	{ _R(IDC_EDIT_INHERIT3),	_R(IDS_EDITBH_INHERIT) },
	{ _R(IDC_EDIT_INHERIT4),	_R(IDS_EDITBH_INHERIT) },
	{ _R(IDC_EDIT_PARENTCOL),	_R(IDS_EDITBH_PARENTCOL) },
	{ _R(IDC_EDIT_3D),			_R(IDS_EDITBH_3D) },
	{ _R(IDC_EDIT_MAKESTYLE),	_R(IDS_EDITBH_MAKESTYLE) },
	{ _R(IDC_EDIT_NOCOLOUR),	_R(IDS_COLBAR_HNOCOLOUR) },
	{ _R(IDC_EDIT_LINEFILL),	_R(IDS_EDITBH_LINEFILL)	},
	{ _R(IDC_EDIT_216ONLY),		_R(IDS_EDITBH_216ONLY)	},
	{ _R(IDC_MAKE_LOCAL),		_R(IDS_EDITBH_MAKE_LOCAL)	},
	{ _R(IDC_EDIT_RENAME),		_R(IDS_COLBAR_HNOCOLOUR) },
	{ _R(IDC_COLOURPICKER),		_R(IDS_STATICCOLOURPICKERTOOLHELP) },
	{ 0, 0 }							// List terminator
};


// Two utility functions to get slider sizing right

INT32 GetSliderHeight(INT32 RectHeight, INT32 Num)
{
	INT32 AvailableHeight = (RectHeight / Num) - 4000; // subtract the minimum gap size
	if (AvailableHeight<2000)
		AvailableHeight = 2000;	// minimum height
	return (AvailableHeight>18000)?18000:AvailableHeight;
}

INT32 GetSliderGap(INT32 RectHeight, INT32 Num)
{
	INT32 SliderGap = (RectHeight / Num) - GetSliderHeight(RectHeight, Num);
	if (SliderGap < 4000)
		SliderGap = 4000;
	return SliderGap;
}


/********************************************************************************************

>	void ColourEditDlg::LoseKeyboardFocus(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/5/95

	Purpose:	Colour Editor focus bodge thingummy
				Returns the input focus back to the selected document (mainframe window)

********************************************************************************************/

static BOOL TextMayBeWrong = FALSE;						// Flag indicating that we might need to update
														// a component field when losing the keyboard focus

static BOOL LoseKbdFocusPending = FALSE;				// Flag used to indicate to the OnIdleEvent
														// handler that we wish to lose the kbd focus

static BOOL LockLoseFocus = FALSE;						// TRUE if focus-losing is locked (we are nto allowed
														// to try to throw away the input focus while it's TRUE)

static BOOL LoseFocusFromEditControls = FALSE;			// TRUE if we should lose the focus if it's inside an
														// edit/writable control. Usually FALSE except on commit.

void ColourEditDlg::LoseKeyboardFocus(void)
{
//	ControlHelper::BubbleHelpDisable();					// Ensure any open bubble help is killed

	if (!LockLoseFocus)
	{
		// If we don't want to throw away the focus from edit controls, we'll check for that case
		if (!LoseFocusFromEditControls)
		{
			// AWOOGA! NASTY WINOIL CODE HACK NASTINESS YUCK YUCK YUCK !!!! ****
			CWindowID Focus = wxWindow::FindFocus();
			if (Focus != NULL)
			{
				static UINT32 WritableGadgets[] = 
				{
					_R(IDC_EDIT_COMPONENT1),
					_R(IDC_EDIT_COMPONENT2),
					_R(IDC_EDIT_COMPONENT3),
					_R(IDC_EDIT_COMPONENT4),
					_R(IDC_EDIT_WEBHEX),
					_R(IDC_EDIT_TINT),
					_R(IDC_EDIT_SHADE),
					_R(IDC_COLOURPICKER),
					//_R(IDC_EDIT_216ONLY),
					0
				};

				// Look to see if the input focus belonhgs to an edit control in our window.
				// If it does, then we won't throw the focus away - we'll return immediately.
				INT32 i = 0;
				while (WritableGadgets[i])
				{
					if (Focus == DialogManager::GetGadget(WindowID, WritableGadgets[i]))
						return;

					i++;
				}
			}
		}

		DialogManager::DefaultKeyboardFocus();

		// And ensure that all component gadgets are updated - if the user was editing
		// in one, then we need to chop it back to displaying the value to 1 d.p. with a %, etc
		if (EditingColour != NULL && TextMayBeWrong)
		{
			SetComponentInfo(1, _R(IDC_NAME_COMPONENT1), _R(IDC_EDIT_COMPONENT1), _R(IDC_EDIT_INHERIT1));
			SetComponentInfo(2, _R(IDC_NAME_COMPONENT2), _R(IDC_EDIT_COMPONENT2), _R(IDC_EDIT_INHERIT2));
			SetComponentInfo(3, _R(IDC_NAME_COMPONENT3), _R(IDC_EDIT_COMPONENT3), _R(IDC_EDIT_INHERIT3));
			SetComponentInfo(4, _R(IDC_NAME_COMPONENT4), _R(IDC_EDIT_COMPONENT4), _R(IDC_EDIT_INHERIT4));	

			SetAllHexComponentsInfo ((UINT32)-1, _R(IDC_NAME_WEBHEX), _R(IDC_EDIT_WEBHEX));
		}

		TextMayBeWrong = FALSE;

		LoseKbdFocusPending = FALSE;		// And clear the pending flag
	}
}


/********************************************************************************************

> BOOL ColourEditDlg::SetUnitGroupDefaults(ColourModel ColModel)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/05/96
	Imputs:		ColModel = colour model we're editing (for WEBSTER - markn 9/1/97)
	Purpose:	Sets the default ScaleUnits for the Colour Editor (actually sets
				StandardUnit groups.
	Errors:		ERROR3 if unable to initialize defaults
	Scope:		static
	SeeAlso:	class StandardUnit

********************************************************************************************/
BOOL ColourEditDlg::SetUnitGroupDefaults(ColourModel ColModel)
{
// WEBSTER - markn 9/1/97
// Rather than get the unit preference from the units tab (which doesn't exist in Webster)
// it chooses between percent ('%') and 0-255 ('d') unit types dependant on the colour model.
// HSV shows its values in '%' format
// RGB shows its values in 0-255 format
//

#ifdef WEBSTER
// WEBSTER - markn 23/1/97
// We've decided to make both models use percentages as the default unit type
	ScaleUnit* pUnit = StandardUnit::PercentGroup.FindUnitFromIndex(0);	// percentage '%'
#else
	ScaleUnit* pUnit = UnitsTab::GetColourUnitPreference();
#endif // WEBSTER

	if (pUnit == NULL || !pUnit->IS_KIND_OF(ScaleUnit))
	{
		ERROR3("Failed to get a default unit");
		return FALSE;
	}
	StandardUnit::PercentGroup.SetDefaultUnit(pUnit);

	StandardUnit::AngleGroup.SetDefaultUnit(&StandardUnit::UnitDegree);
	return TRUE;
}


/********************************************************************************************

>	ColourEditDlg::ColourEditDlg(): DialogOp(ColourEditDlg::IDD, ColourEditDlg::Mode) 

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/6/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Default constructor for the colour editor dialogue class
	Notes:		You shouldn't really use this constructor if you can avoid it
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

ColourEditDlg::ColourEditDlg(): DialogOp(ColourEditDlg::IDD, ColourEditDlg::Mode) 
{
	// If there is no existing editor, make us "the" editor instantiation
	// Otherwise, we'll realise our mistake (in Do/DoWithParam) and kill ourself
	if (TheEditor == NULL)
		TheEditor = this;

	ParentList		= NULL;
	ResultColour	= NULL;
	EditingColour	= NULL;
	ISentTheMessage = FALSE;
	AmShaded 		= FALSE;

	DragStartArea	  = CEDRAG_NONE;
	DragUpdatedOnIdle = FALSE;
	FirstEdit		  = TRUE;

	ResetState();

// WEBSTER - markn 31/1/97
// Bodge - uses the Named default value to keep that model preserved
#ifndef WEBSTER
	DisplayModel	= (ColourModel) DefaultDisplayModel;
#else
	DisplayModel	= (ColourModel) DefaultDisplayModelN;
#endif // WEBSTER

	AbortColour = NULL;

	NameDropDown	= NULL;
	ParentDropDown	= NULL;

	CurrentCursorID	= 0;
	CurrentCursor	= NULL;

	m_bDoingSetGadget = FALSE;
	m_NeedsResize = FALSE;
	GetApplication()->RegisterIdleProcessor(IDLEPRIORITY_LOW, this);
}



/********************************************************************************************

>	ColourEditDlg::~ColourEditDlg()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/6/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Destructor for the colour editor dialogue class
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

ColourEditDlg::~ColourEditDlg()
{
	GetApplication()->RemoveIdleProcessor(IDLEPRIORITY_LOW, this);
	EndTimedProcessing();

	if (EditingColour != NULL)
	{
		// Ensure we remember the current model as the preferred default
		if (EditingColour->IsNamed())
			DefaultDisplayModelN = (INT32) DisplayModel;
// But do not overwrite the preference for local colours - this must be set in the options dlg
//		else
//			DefaultDisplayModel = (INT32) DisplayModel;

		delete EditingColour;
	}

	if (AbortColour != NULL)
	{
		delete AbortColour;
		AbortColour = NULL;
	}

	// If we were the current active editor instantiation then we indicate there
	// is no longer an editor open.
	if (TheEditor == this)
		TheEditor = NULL;

	// Delete our drop-down colour list support objects
	if (NameDropDown != NULL)
		delete NameDropDown;

	if (ParentDropDown != NULL)
		delete ParentDropDown;

	if (CurrentCursor != NULL)
		delete CurrentCursor;
}



/********************************************************************************************

>	void ColourEditDlg::SetColourNameList(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/94
	Purpose:	Initialises the colour editor dialogue name combo-box list
	Scope:		private

********************************************************************************************/

void ColourEditDlg::SetColourNameList(void)
{
	if (State.ParentListOK)
		return;

	// Get a description of the current local colour
	SelRange *Selection = GetApplication()->FindSelection();
//
//	String_256 SelDesc = Selection->Describe(MENU);
//	String_256 ColourDesc(TEXT("(No selection colour)"));;
//	if (Selection->Count() != 0)
//		ColourDesc._MakeMsg("Colour of #1%s", (TCHAR *)SelDesc);


	Progress Hourglass;		// Start an hourglass running. On destruction it'll turn it off again

	BOOL OldSentState = ISentTheMessage;
	ISentTheMessage = TRUE;

	if (NameDropDown != NULL)
		delete NameDropDown;
	NameDropDown = new ColourDropDown;

	BOOL ok = TRUE;
	if (NameDropDown == NULL || !NameDropDown->Init(WindowID, _R(IDC_EDIT_NAMEMENU)))
		ok = FALSE;

	if (ok)
	{
		String_256 FillColDesc(_R(IDS_COLEDIT_FILL));
		String_256 LineColDesc(_R(IDS_COLEDIT_LINE));

		if (Selection->Count() == 0)		// No selection, so will use current colours
		{
			FillColDesc = String_256(_R(IDS_COLEDIT_CURFILL));
			LineColDesc = String_256(_R(IDS_COLEDIT_CURLINE));
		}

		// Ensure there are no special entries in the list, then add the 2 we want this time
		NameDropDown->ClearAllSpecialEntries();

		ColourList	*NewParentList  = NULL;
		DocColour	DocColourToEdit;
		ColourManager::FindColourOfInterestToUser(&DocColourToEdit, &NewParentList, FALSE);
		
		ok = NameDropDown->AddSpecialEntry(&FillColDesc, &DocColourToEdit);
		if (ok)
		{
			ColourManager::FindColourOfInterestToUser(&DocColourToEdit, &NewParentList, TRUE);
			ok = NameDropDown->AddSpecialEntry(&LineColDesc, &DocColourToEdit);
		}

		if (ok)
		{
			// Determine if we're editing a named colour, or a current colour
			IndexedColour *Ptr = NULL;
			if (ParentList != NULL)
			{
				Ptr = (IndexedColour *) ParentList->GetHead();
				while (Ptr != NULL)
				{
					if (!Ptr->IsDeleted() && Ptr->IsNamed())
					{
						if (Ptr == ResultColour)
							break;
					}
					Ptr = (IndexedColour *) ParentList->GetNext(Ptr);
				}
			}

			// If we aren't editing a named colour (Ptr == NULL) then we're editing fill (0) or line (1) colour
			INT32 SelIndex = -1;
			if (Ptr == NULL)
				SelIndex = (EditingLineColour) ? 1 : 0;

			// Fill in the list
			ok = NameDropDown->FillInColourList(Ptr, SelIndex);

			// NOTE: We could do a SetComboListLength here, except that this causes awful
			// redraws of everything "behind" the combo-list extent. I can't stop it doing this,
			// other than by not setting the list length... so I don't
		}
	}

	// If we failed, then we shade this control so that some semblance of stability remains
	if (!ok)
		EnableGadget(_R(IDC_EDIT_NAMEMENU), FALSE);

/*
	INT32 Index = 0;
	INT32 SelectedIndex = 0;

	// Compile a list of all editable colours into the colour name combo-list
	GadgetRedraw(_R(IDC_EDIT_NAMEMENU), FALSE);
	DeleteAllValues(_R(IDC_EDIT_NAMEMENU));

	String_256 FillColDesc(TEXT("Fill colour"));
	String_256 LineColDesc(TEXT("Line colour"));
	String_256 DottedLine(TEXT("---------------------------------------"));

	if (Selection->Count() == 0)		// No selection, so will use current colours
	{
		FillColDesc = TEXT("Current fill colour");
		LineColDesc = TEXT("Current line colour");
	}

	SetStringGadgetValue(_R(IDC_EDIT_NAMEMENU), &FillColDesc, TRUE);	
	SetStringGadgetValue(_R(IDC_EDIT_NAMEMENU), &LineColDesc, TRUE);
	SetStringGadgetValue(_R(IDC_EDIT_NAMEMENU), &DottedLine, TRUE);
	Index += 3;
	if (EditingLineColour)
		SelectedIndex++;

	if (ParentList != NULL)
	{
		IndexedColour *Ptr = (IndexedColour *) ParentList->GetHead();
		while (Ptr != NULL)
		{
			if (!Ptr->IsDeleted() && Ptr->IsNamed())
			{
				SetStringGadgetValue(_R(IDC_EDIT_NAMEMENU), Ptr->GetName(), TRUE);
				if (Ptr == ResultColour)
					SelectedIndex = Index;

				Index++;
			}

			Ptr = (IndexedColour *) ParentList->GetNext(Ptr);
		}
	}

	GadgetRedraw(_R(IDC_EDIT_NAMEMENU), TRUE);

	// Set the writable colour name (combo box editable) field
	SetSelectedValueIndex(_R(IDC_EDIT_NAMEMENU), SelectedIndex);
*/

	ISentTheMessage = OldSentState;

	State.ParentListOK = TRUE;
}



/********************************************************************************************

>	void ColourEditDlg::RedrawColourNameList(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/10/95

	Purpose:	Ensures that the colour splodge in the colour name dropdown list is
				redrawn using the correct new appearance of the colour. Far more
				efficient than rebuilding the colour list from scratch.

********************************************************************************************/

void ColourEditDlg::RedrawColourNameList(void)
{
	if (NameDropDown == NULL)
		return;

	String_256 FillColDesc(_R(IDS_COLEDIT_FILL));
	String_256 LineColDesc(_R(IDS_COLEDIT_LINE));

	SelRange *Selection = GetApplication()->FindSelection();
	if (Selection->Count() == 0)		// No selection, so will use current colours
	{
		FillColDesc = String_256(_R(IDS_COLEDIT_CURFILL));
		LineColDesc = String_256(_R(IDS_COLEDIT_CURLINE));
	}

	ColourList	*NewParentList  = NULL;
	DocColour	DocColourToEdit;

	ColourManager::FindColourOfInterestToUser(&DocColourToEdit, &NewParentList, FALSE);
	NameDropDown->UpdateSpecialEntry(&FillColDesc, &DocColourToEdit);

	ColourManager::FindColourOfInterestToUser(&DocColourToEdit, &NewParentList, TRUE);
	NameDropDown->UpdateSpecialEntry(&LineColDesc, &DocColourToEdit);

	// And ensure that the dropdown is redrawn
	InvalidateGadget(_R(IDC_EDIT_NAMEMENU));
}



/********************************************************************************************

>	void ColourEditDlg::ResetState(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/1/95
	Purpose:	Initialises the colour editor dialogue State variable to be 'unknown'.
				The next SetControls() call after this will set all the controls in the
				window, rather than only those which are known to have changed.
	Scope:		private

********************************************************************************************/

// We fake a special colour type for shades for the State information *only*
#define COLOURTYPE_SHADE ((IndexedColourType) 32)

void ColourEditDlg::ResetState(void)
{
	State.Folded		= 2;
	State.ColType		= (IndexedColourType) 99;	// An invalid value!
	State.DisplayModel	= MAX_COLOURMODELS;
	State.ResultColour	= NULL;
	State.Initialised	= FALSE;
	State.ParentListOK  = FALSE;
}



/********************************************************************************************

>	void ColourEditDlg::SetExtent(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/1/95

	Purpose:	If necessary, changes the size of the editor window and shuffles controls
				around to show folded/unfolded tint/linked information appropriately.
				If the State has not been reset or changed, this may do nothing, to minimise
				unnecessary redraws.

	Scope:		private

********************************************************************************************/

void ColourEditDlg::SetExtent(void)
{
	// Set size to small (cut off below 'more' button) or large (full-size) as appropriate
	// NOTE that this relies on the gadgets passed to SetWindowExtent being at the
	// bottom of the folded/unfolded layouts of the dialogue respectively.

	if (EditingColour == NULL)
	{
		// WEBSTER - markn 1/2/97
		#ifdef WEBSTER
		ColourPicker::SetWindowExtent(WindowID, _R(IDC_EDIT_PICKER), _R(IDC_EDIT_PICKER));
		#endif // WEBSTER
		HideGadget(_R(IDC_EDIT_ADVANCEDPANEL), TRUE);
		NeedsResize();
		return;
	}

	// If there has been no change in the colour type, we should return.
	// This is compllicated by shades, which internally are a variant of TINTs, but
	// externally are different "types".
	IndexedColourType CurrentColType = EditingColour->GetType();
	if (CurrentColType == COLOURTYPE_TINT && EditingColour->TintIsShade())
		CurrentColType = COLOURTYPE_SHADE;	// Shade is a special "Bodge" colour type

#ifndef WEBSTER
	if (State.Folded == Folded && State.ColType == CurrentColType)
		return;
#else
	// WEBSTER - markn 24/4/97
	// Always unfolded.
	Folded = FALSE; // always unfolded
#endif // WEBSTER

	NeedsResize();

	if (Folded)
	{		
		// WEBSTER - markn 11/12/96
		// Always unfolded.
		// Changed folded to show ed fields
#ifndef WEBSTER
		ColourPicker::SetWindowExtent(WindowID, _R(IDC_EDIT_PICKER), _R(IDC_EDIT_PICKER));
		//		SetStringGadgetValue(_R(IDC_EDIT_ADVANCED), _R(IDS_EDIT_MORE));
		SetBoolGadgetSelected(_R(IDC_EDIT_ADVANCED), FALSE);
#else
		ColourPicker::SetWindowExtent(WindowID, _R(IDC_EDIT_PICKER), _R(IDC_EDIT_COLTYPE));
#endif // WEBSTER
		HideGadget(_R(IDC_EDIT_ADVANCEDPANEL), TRUE);
	}			
	else
	{
		HideGadget(_R(IDC_EDIT_ADVANCEDPANEL), FALSE);
		CGadgetID Gadget = _R(IDC_EDIT_COLTYPE);

		if (EditingColour != NULL)	// If we have an editing colour, set tint/link controls
		{
			switch (EditingColour->GetType())
			{
				case COLOURTYPE_TINT:
					{
						Gadget = _R(IDC_EDIT_TINT);
						HideGadgetList(LinkGadgetIDs, TRUE);
						HideGadgetList(TintGadgetIDs, FALSE);
						HideGadget (_R(IDC_EDIT_PARENTNAME), FALSE);
						HideGadget (_R(IDC_EDIT_PARENTCOL), FALSE);
						// ColourPicker::SetGadgetPositions(WindowID, LinkGadgetIDs, 0);
						// ColourPicker::SetGadgetPositions(WindowID, TintGadgetIDs, _R(IDC_EDIT_PARENTCOL));

						// And if it's not a shade, hide the extra writable field
						HideGadget(_R(IDC_EDIT_SHADE), !EditingColour->TintIsShade());
						HideGadget(_R(IDC_EDIT_SHADEPERCENT), !EditingColour->TintIsShade());
					}
					break;

				case COLOURTYPE_LINKED:
					Gadget = _R(IDC_EDIT_INHERIT4);
					HideGadgetList(LinkGadgetIDs, FALSE);
					HideGadgetList(TintGadgetIDs, TRUE);
					HideGadget (_R(IDC_EDIT_PARENTNAME), FALSE);
					HideGadget (_R(IDC_EDIT_PARENTCOL), FALSE);
					// ColourPicker::SetGadgetPositions(WindowID, TintGadgetIDs, 0);
					// ColourPicker::SetGadgetPositions(WindowID, LinkGadgetIDs, _R(IDC_EDIT_PARENTCOL));
					break;

				default:
					HideGadget (_R(IDC_EDIT_PARENTNAME), TRUE);
					HideGadget (_R(IDC_EDIT_PARENTCOL), TRUE);
					HideGadgetList(LinkGadgetIDs, TRUE);
					HideGadgetList(TintGadgetIDs, TRUE);
					break;
			}
		}

		ColourPicker::SetWindowExtent(WindowID, _R(IDC_EDIT_PICKER), Gadget);
#ifndef WEBSTER // not included in Webster
		SetBoolGadgetSelected(_R(IDC_EDIT_ADVANCED), TRUE);
//		SetStringGadgetValue(_R(IDC_EDIT_ADVANCED), _R(IDS_EDIT_LESS));
#endif
	}

}


/********************************************************************************************

>	void ColourEditDlg::HideGadgetList(CGadgetID * Gadgets, BOOL Hide=TRUE)

	Author:		Alex Bligh
	Created:	30/5/2005

	Inputs:		Gadgets - NULL terminated list of gadgets
				Hide - TRUE to hide else FALSE to show

	Purpose:	Hides / shows the gadgets on the list

	Scope:		Protected

********************************************************************************************/

void ColourEditDlg::HideGadgetList(CGadgetID * Gadgets, BOOL Hide /*=TRUE*/)
{
	CGadgetID Gadget;
	while ((Gadget=*(Gadgets++))) // assignment
	{
		HideGadget(Gadget, Hide);	
	}
}

/*****************************************************************************
>	BOOL ColourEditDlg::OnIdleEvent()

	Author:		Alex Bligh
	Created:	30/5/2005
	Purpose:	Idle event handler for the colour dialog
				
	Returns:	FALSE (to indicate we want no more idle events)
				The wakeup from the timer will create one for us
	Errors:		-


*****************************************************************************/

BOOL ColourEditDlg::OnIdleEvent()
{
	return ColourPicker::OnIdleEvent(WindowID);
}

/********************************************************************************************

>	void ColourEditDlg::HideOrShowColourPicker()

	Author:		Alex Bligh
	Created:	30/5/2005

	Inputs:		-

	Purpose:	Synchronize state of colour picker gadget

	Scope:		Protected

********************************************************************************************/

void ColourEditDlg::HideOrShowColourPicker()
{
	if (needColPickHidden != colPickHidden)
	{
		// this used to use Hide() but that was boring
		EnableGadget(_R(IDC_COLOURPICKER), !needColPickHidden);
		colPickHidden=needColPickHidden;
		
		//CheckDialogSize();
	}
}

/********************************************************************************************

>	void ColourEditDlg::CheckDialogSize()

	Author:		Alex Bligh
	Created:	30/5/2005
	Inputs:		-
	Purpose:	Ensure the dialog is a sensible size
	Scope:		Protected

********************************************************************************************/

void ColourEditDlg::CheckDialogSize()
{
	if (!m_NeedsResize)
		return;

	m_NeedsResize=FALSE;

	// Set up bubble help
	ColourPicker::SetBubbleHelp(TintGadgetIDs);
	ColourPicker::SetBubbleHelp(LinkGadgetIDs);
	ColourPicker::SetBubbleHelp(OtherGadgetIDs);

	ColourPicker::RelayoutDialog(WindowID);
}


/********************************************************************************************

>	void ColourEditDlg::SetControls(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/6/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Initialises the colour editor dialogue controls/gadgets
	Scope:		private
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

static UINT32 CurrentTypingGadget = 0;

void ColourEditDlg::SetControls(void)
{
	// If there is no colour to edit, desperately seek a new colour to edit - avoid
	// shading the dialogue at all costs. (We should then only shade if there are no documents)
	if (EditingColour == NULL)
	{
		FindUsefulColourToEdit(EditingLineColour);
		if (EditingColour != NULL)
			return;						// Avoid infinite recursion!
	}

	SetExtent();	

	if (EditingColour == NULL)
	{
		ShadeMyself();			// No colour to edit - ensure we're shaded
		return;
	}


	BOOL OldSentState = ISentTheMessage;
	ISentTheMessage = TRUE;		// Try to lock out DIM_TEXT_CHANGED messages during update

	ShadeMyself(TRUE, TRUE);	// We have a colour - ensure we are not shaded

	SetColourNameList();		// Set the name writable and dropdown list of colours

	//Set the transparency button
	if( EditingColour->IsNamed() )
	{
		EnableGadget( _R(IDC_EDIT_NOCOLOUR),  FALSE);
	}
	else
	{
		EnableGadget( _R(IDC_EDIT_NOCOLOUR),  TRUE);
		SetBoolGadgetSelected( _R(IDC_EDIT_NOCOLOUR), NoFillButtonDown);
	}

	//Graham 16/10/97: The rename button
	String_256 empty;
	OpState opstate=GetCommandState(&ColCmd_Name, &empty);
	BOOL fRenameCommandIsAvailable=opstate.Greyed;
	EnableGadget( _R(IDC_EDIT_RENAME),  !fRenameCommandIsAvailable);

	// Set tint information
	BOOL Enable = (EditingColour->GetType() == COLOURTYPE_TINT);
	IndexedColourType CurrentColType = EditingColour->GetType();
	if (CurrentColType == COLOURTYPE_TINT && EditingColour->TintIsShade())
		CurrentColType = COLOURTYPE_SHADE;	// Shade is a special "Bodge" colour type

	if (State.ColType != CurrentColType)
	{
		NeedsResize();
		EnableGadget(_R(IDC_EDIT_TINT), Enable);

		EnableGadget(_R(IDC_EDIT_TINTNAME), Enable);
		EnableGadget(_R(IDC_EDIT_TINTPERCENT), Enable);

//		EnableGadget(_R(IDC_EDIT_TINTSLIDER), Enable);
//		SetGadgetRange(_R(IDC_EDIT_TINTSLIDER), 0, 100, 1);
//		SetGadgetBitmaps(_R(IDC_EDIT_TINTSLIDER), _R(IDB_SLIDERBASE), _R(IDB_SLIDERSLIDER));

		if (CurrentColType == COLOURTYPE_SHADE)
		{
			EnableGadget(_R(IDC_EDIT_SHADE), Enable);
			EnableGadget(_R(IDC_EDIT_SHADEPERCENT), Enable);
		}
	
		// The Colour model dropdown and 3d button are not available while editing a tint/shade
		// (The 3d button is set below, but we need to enable it here too just in case the code below
		//  is not called (when we only change the colour model)
		EnableGadget(_R(IDC_EDIT_COLMODEL), !Enable);
		EnableGadget(_R(IDC_EDIT_3D), !Enable);
	}
	
	if (Enable)
	{
		// It's a tint/shade. Set the "Tint:" or "Shade:" text as appropriate
		if (EditingColour->TintIsShade())
			SetStringGadgetValue(_R(IDC_EDIT_TINTNAME), _R(IDS_EDIT_TEXTSHADE));
		else
			SetStringGadgetValue(_R(IDC_EDIT_TINTNAME), _R(IDS_EDIT_TEXTTINT));

		// If we're not currently typing into this field and it is displaying the wrong thing,
		// we'd better update it to be correct.
		if (CurrentTypingGadget != _R(IDC_EDIT_TINT))
		{
			String_256 CurrentValue = GetStringGadgetValue(_R(IDC_EDIT_TINT));
			String_8 NewValue;

			if (CurrentColType == COLOURTYPE_SHADE)
				ColourPicker::GetShadeValueAsString(EditingColour, 1, &NewValue);
			else
				ColourPicker::GetTintAsString(EditingColour, &NewValue);

			if (NewValue != CurrentValue)
				SetStringGadgetValue(_R(IDC_EDIT_TINT), NewValue);
		}

		if (CurrentColType == COLOURTYPE_SHADE && CurrentTypingGadget != _R(IDC_EDIT_SHADE))
		{
			String_256 CurrentValue = GetStringGadgetValue(_R(IDC_EDIT_SHADE));
			String_8 NewValue;

			ColourPicker::GetShadeValueAsString(EditingColour, 2, &NewValue);

			if (NewValue != CurrentValue)
				SetStringGadgetValue(_R(IDC_EDIT_SHADE), NewValue);
		}
				
		// And set the slider to the nearest position to this value
		INT32 SliderValue = (INT32) ((100.0 * (EditingColour->GetTintValue().MakeDouble())) + 0.5);
		if (SliderValue > 100)		// Just ensure rounding doesn't give a 101%!
			SliderValue = 100;

//		SetLongGadgetValue(_R(IDC_EDIT_TINTSLIDER), 100-SliderValue);
	}
//	else
//		SetLongGadgetValue(_R(IDC_EDIT_TINTSLIDER), 100-0);

	// Set up the combo box of available colour models
	if (State.DisplayModel != DisplayModel)
	{
		NeedsResize();
		// Shift around the component name and writable gadgets until Windows doesn't
		// know if it's coming or going. 
		static CGadgetID CompGadgetIDs[10] =
		{
			_R(IDC_NAME_COMPONENT1),
			_R(IDC_EDIT_COMPONENT1),
			_R(IDC_NAME_COMPONENT2),
			_R(IDC_EDIT_COMPONENT2),
			_R(IDC_NAME_COMPONENT3),
			_R(IDC_EDIT_COMPONENT3),
			_R(IDC_NAME_COMPONENT4),
			_R(IDC_EDIT_COMPONENT4),
			_R(IDC_NAME_WEBHEX),
			_R(IDC_EDIT_WEBHEX)
		};// CGS CGS CGS

		// Special case for certain colour models but fall back to general formatter for everything else
		switch (DisplayModel)
		{
			case COLOURMODEL_RGBT:
				{
					INT32 TextWidth[5] = {7, 7, 7, 0, 7};
					INT32 EditWidth[5] = {35, 35, 35, 0, 48};
					ColourPicker::SetFixedComponentGadgets(WindowID, CompGadgetIDs, _R(IDC_EDIT_PICKER), DisplayModel, TextWidth, EditWidth);
				}
				break;
			case COLOURMODEL_HSVT:
				{
					INT32 TextWidth[5] = {7, 7, 7, 0, 7};
					INT32 EditWidth[5] = {35, 35, 35, 0, 48};
					ColourPicker::SetFixedComponentGadgets(WindowID, CompGadgetIDs, _R(IDC_EDIT_PICKER), DisplayModel, TextWidth, EditWidth);
				}
				break;
			default:
				ColourPicker::SetComponentGadgets(WindowID, CompGadgetIDs, _R(IDC_EDIT_PICKER), DisplayModel);
				break;
		}

		// Enable the '3d display' button if it is relevant (CMYK, RGB or WebRGB mode, and not showing a tint/shade
		EnableGadget(_R(IDC_EDIT_3D),	(DisplayModel == COLOURMODEL_CMYK || DisplayModel == COLOURMODEL_RGBT || DisplayModel == COLOURMODEL_WEBRGBT) &&
									(CurrentColType != COLOURTYPE_TINT && CurrentColType != COLOURTYPE_SHADE));
		SetBoolGadgetSelected(_R(IDC_EDIT_3D), Use3DDisplay);

		GadgetRedraw(_R(IDC_EDIT_COLMODEL), FALSE);
		DeleteAllValues(_R(IDC_EDIT_COLMODEL));
		ColourContextArray ColContexts;
		ColourContext::GetGlobalDefaults(&ColContexts);

		String_64 NameString;
		INT32 Index = 0;
		INT32 SelectedIndex = 0;

		for (INT32 i = 0; i < MAX_COLOURMODELS; i++)
		{
			if (ColContexts.Context[i] != NULL)
			{
				String_32 ModelName;
				ColContexts.Context[i]->GetModelName(&ModelName);

				NameString.MakeMsg(_R(IDS_COLCONTEXTNAME), (TCHAR *) ModelName);
				SetStringGadgetValue(_R(IDC_EDIT_COLMODEL), NameString, FALSE, Index);

				if (i == (INT32)DisplayModel)
					SelectedIndex = Index;

				Index++;
			}
		}		

		GadgetRedraw(_R(IDC_EDIT_COLMODEL), TRUE);
		SetSelectedValueIndex(_R(IDC_EDIT_COLMODEL), SelectedIndex);
		SetComboListLength(_R(IDC_EDIT_COLMODEL));
	}

	if (DisplayModel == COLOURMODEL_RGBT || DisplayModel == COLOURMODEL_HSVT)
	{
		//HideGadget (_R(IDC_NAME_COMPONENT4), TRUE);
		//HideGadget (_R(IDC_EDIT_COMPONENT4), TRUE);
		HideGadget (_R(IDC_NAME_WEBHEX), FALSE);
		HideGadget (_R(IDC_EDIT_WEBHEX), FALSE);
	}
	else
	{
		//HideGadget (_R(IDC_NAME_COMPONENT4), FALSE);
		//HideGadget (_R(IDC_EDIT_COMPONENT4), FALSE);

		HideGadget (_R(IDC_NAME_WEBHEX), TRUE);
		HideGadget (_R(IDC_EDIT_WEBHEX), TRUE);
	}

	// Set component name strings & writables, and inheritance gadgets
	// Note: This MUST occur AFTER the call to ColourPicker::SetComponentGadgets (up 1 page)
	SetComponentInfo(1, _R(IDC_NAME_COMPONENT1), _R(IDC_EDIT_COMPONENT1), _R(IDC_EDIT_INHERIT1));
	SetComponentInfo(2, _R(IDC_NAME_COMPONENT2), _R(IDC_EDIT_COMPONENT2), _R(IDC_EDIT_INHERIT2));
	SetComponentInfo(3, _R(IDC_NAME_COMPONENT3), _R(IDC_EDIT_COMPONENT3), _R(IDC_EDIT_INHERIT3));
	SetComponentInfo(4, _R(IDC_NAME_COMPONENT4), _R(IDC_EDIT_COMPONENT4), _R(IDC_EDIT_INHERIT4));

	if (DisplayModel == COLOURMODEL_WEBRGBT || DisplayModel==COLOURMODEL_RGBT || DisplayModel==COLOURMODEL_HSVT)
	{
		SetAllHexComponentsInfo ((UINT32)-1, _R(IDC_NAME_WEBHEX), _R(IDC_EDIT_WEBHEX));
	}

	// Find a safe parent for the colour to link to. If we can't find one, then
	// we remove the linking options from the drop down list.
	IndexedColour *SafeParent;
	SafeParent = FindSafeParent(EditingColour->FindLastLinkedParent(),
								(CurrentColType == COLOURTYPE_TINT));

	// Ensure the editing colour has a parent which is safe to link to
	if (SafeParent != EditingColour->FindLastLinkedParent())
		EditingColour->SetLinkedParent(SafeParent, EditingColour->GetType());

	// And make sure CurrentColType is still correct!
	CurrentColType = EditingColour->GetType();
	if (CurrentColType == COLOURTYPE_TINT && EditingColour->TintIsShade())
		CurrentColType = COLOURTYPE_SHADE;	// Shade is a special "Bodge" colour type

	// Set up the colour type combo-box gadget whenever the type or the colour itself is changed,
	// or if we've not initialised it before
	if (State.ColType != CurrentColType || State.ResultColour != ResultColour || !State.Initialised)
	{
		GadgetRedraw(_R(IDC_EDIT_COLTYPE), FALSE);
		DeleteAllValues(_R(IDC_EDIT_COLTYPE));

		INT32 Index = 0;				// Compute an index for each item as we build the list
		INT32 SelIndex = 0;			// Default SelIndex, for COLOURTYPE_NORMAL

		SetStringGadgetValue(_R(IDC_EDIT_COLTYPE), _R(IDS_COLTYPE_NORMAL),	FALSE, Index++);

//		if (ResultColour->IsNamed())		// "Spot colour" only available for named colours
//		This option is now still shown in the list, but when the user tries to select it,
//		we force them to cancel or make a new named spot colour.
#ifndef WEBSTER
		{
			if (CurrentColType == COLOURTYPE_SPOT)
				SelIndex = Index;

			SetStringGadgetValue(_R(IDC_EDIT_COLTYPE), _R(IDS_COLTYPE_SPOT),	FALSE, Index++);
		}
#endif //WEBSTER

		if (SafeParent != NULL)		// We can make the colour linked
		{
			if (CurrentColType == COLOURTYPE_TINT)
				SelIndex = Index;
			SetStringGadgetValue(_R(IDC_EDIT_COLTYPE), _R(IDS_COLTYPE_TINT),	FALSE, Index++);

			if (CurrentColType == COLOURTYPE_SHADE)
				SelIndex = Index;
			SetStringGadgetValue(_R(IDC_EDIT_COLTYPE), _R(IDS_COLTYPE_SHADE),	FALSE, Index++);

			if (CurrentColType == COLOURTYPE_LINKED)
				SelIndex = Index;
			SetStringGadgetValue(_R(IDC_EDIT_COLTYPE), _R(IDS_COLTYPE_LINKED),	FALSE, Index++);
		}

		GadgetRedraw(_R(IDC_EDIT_COLTYPE), TRUE);
		SetComboListLength(_R(IDC_EDIT_COLTYPE));

		SetSelectedValueIndex(_R(IDC_EDIT_COLTYPE), SelIndex);
	}

	// Enable/Disable extra options gadgets as appropriate to EditingColour settings
	// and set up the list of potential parent colours
	if (State.ColType != CurrentColType)
	{
		Enable = (EditingColour->GetType() == COLOURTYPE_TINT ||
					EditingColour->GetType() == COLOURTYPE_LINKED);


		EnableGadget(_R(IDC_EDIT_PARENTNAME), Enable);
		EnableGadget(_R(IDC_EDIT_PARENTCOL), Enable);
	}
	CompileParentColourList(_R(IDC_EDIT_PARENTCOL));		// Set the combo box list

	if ((EditingColour != NULL) && (EditingColour->IsNamed()))
		EnableGadget(_R(IDC_MAKE_LOCAL), TRUE);
	else
		EnableGadget(_R(IDC_MAKE_LOCAL), FALSE);
	
	// And finish	
	ISentTheMessage = OldSentState;		// Unlock the message handler again

	// And remember the new state of the controls/extent etc so we don't have to redraw
	// next time where these things have not changed
	State.Folded		= (Folded) ? 1 : 0;
	State.ColType		= CurrentColType;
	State.DisplayModel	= DisplayModel;
	State.ResultColour	= ResultColour;
	State.Initialised	= TRUE;

	CheckDialogSize();
}



/********************************************************************************************

>	void ColourEditDlg::SetComponentInfo(UINT32 ComponentID, UINT32 NameGadget,
										 UINT32 WritableGadget, UINT32 InheritGadget)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/7/94
	Inputs:		ComponentID - Which component name is being set (1..4)
				NameGadget - Gadget ID for gadget displaying the name text
				WritableGadget - Writable gadget to also be shaded/unshaded
				InheritGadget - The checkbox gadget for linked-colour inheritance

	Purpose:	Finds and sets the component name for one gadget. If the component
				is not used in the EditingColours colour model, then the name is
				blanked and the related writable gadget is shaded.
				
	Scope:		private

********************************************************************************************/

void ColourEditDlg::SetComponentInfo(UINT32 ComponentID, UINT32 Gadget,
									 UINT32 WritableGadget, UINT32 InheritGadget)
{
	if (EditingColour == NULL)		// This should never happen
	{
		ERROR3("Unexpectedly NULL EditingColour in ColourEditDlg::SetComponentInfo");
		return;
	}

	if (CurrentTypingGadget != WritableGadget)
	{
//		ColourContext *cc = ColourContext::GetGlobalDefault(DisplayModel);
		ColourContext *cc = NULL;
		BOOL bDeleteCC = GetColourContext(DisplayModel, &cc);	
		String_32 CompName;

		if (cc != NULL && cc->GetComponentName(ComponentID, &CompName))
		{
			if (DisplayModel != COLOURMODEL_WEBRGBT)
			{
				SetStringGadgetValue(Gadget, CompName);
				SetStringGadgetValue(InheritGadget, CompName);
				EnableGadget(Gadget, TRUE);
				HideGadget(Gadget, FALSE);

				String_8 NewText;
				ColourPicker::GetComponentAsString(EditingColour, cc, ComponentID, &NewText);
				EnteredSetGadgetValue();
				SetStringGadgetValue(WritableGadget, NewText);
				ExitedSetGadgetValue();

				BOOL InheritsComponent = EditingColour->InheritsComponent(ComponentID);

				EnableGadget(WritableGadget, (!InheritsComponent) &&
											(EditingColour->GetType() != COLOURTYPE_TINT));
				HideGadget(WritableGadget, FALSE);

				// Set up the inheritance gadget
				EnableGadget(InheritGadget, EditingColour->GetType() == COLOURTYPE_LINKED);
				HideGadget(InheritGadget, EditingColour->GetType() != COLOURTYPE_LINKED);
				SetBoolGadgetSelected(InheritGadget, InheritsComponent);
			}
			else
			{
				//SetStringGadgetValue(Gadget, &CompName);
				SetStringGadgetValue(InheritGadget, CompName);
				//EnableGadget(Gadget, TRUE);
				//HideGadget(Gadget, FALSE);

				BOOL InheritsComponent = EditingColour->InheritsComponent(ComponentID);

				// Set up the inheritance gadget
				EnableGadget(InheritGadget, EditingColour->GetType() == COLOURTYPE_LINKED);
				HideGadget(InheritGadget, EditingColour->GetType() != COLOURTYPE_LINKED);
				SetBoolGadgetSelected(InheritGadget, InheritsComponent);
			}
		}
		else
		{
			HideGadget(Gadget, TRUE);
			HideGadget(WritableGadget, TRUE);
			HideGadget(InheritGadget, TRUE);
		}

		// Delete the colour context if necessary
		if (bDeleteCC)
			ColourContextList::GetList()->RemoveContext(&cc);			// Have finished with it

	}
	//CheckDialogSize();
}



/********************************************************************************************

>	void ColourEditDlg::SetAllHexComponentsInfo(UINT32 ComponentID, UINT32 NameGadget,
										    UINT32 WritableGadget)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/11/99
	Inputs:		ComponentID - Which component name is being set (must be set at -1).
				NameGadget - Gadget ID for gadget displaying the name text (must be
							 _R(IDC_NAME_WEBHEX)).
				WritableGadget - Writable gadget to also be shaded/unshaded (must be
							     _R(IDC_EDIT_WEBHEX))

	Purpose:	Finds and sets all component names for the hex gadget.  This actually
				gets the individual rgb values, and concatenates into a single string
				of the form rrggbb for display.
				
	Scope:		private

	Note:		Should only be called in a: if (DisplayModel == COLOURMODEL_WEBRGBT) ....
				SetComponentInfo should also be called for all colour components before
				this function, since SetComponentInfo sets up all of the colour component
				inheritace stuff.

********************************************************************************************/

void ColourEditDlg::SetAllHexComponentsInfo(UINT32 ComponentID, UINT32 NameGadget,
										    UINT32 WritableGadget)
{
	if (EditingColour == NULL)		// This should never happen
	{
		ERROR3("Unexpectedly NULL EditingColour in ColourEditDlg::SetComponentInfo");
		return;
	}

	if (!(ComponentID == (UINT32)-1) && (NameGadget == _R(IDC_NAME_WEBHEX)) &&
		 (WritableGadget == _R(IDC_EDIT_WEBHEX)))
	{
		ERROR3("Calling ColourEditDlg::SetAllHexComponentsInfo for incorrect control");
		return;
	}

	if (CurrentTypingGadget != WritableGadget)
	{
		// We know that GetComponentsAsHexString only works if the colour context is RGBT
		// So why get anything else???
		ColourContext *cc = ColourContext::GetGlobalDefault(COLOURMODEL_RGBT);
		String_32 CompName (_R(IDS_COLCOMPL_WEBHEX));

		if (cc != NULL)
		{
			if (DisplayModel == COLOURMODEL_RGBT || DisplayModel == COLOURMODEL_HSVT)
			{
				SetStringGadgetValue(NameGadget, CompName);
				EnableGadget(NameGadget, TRUE);
				HideGadget(NameGadget, FALSE);

				String_16 NewText;
				ColourPicker::GetComponentsAsHexString(EditingColour, cc, ComponentID, &NewText);
				EnteredSetGadgetValue();
				SetStringGadgetValue(WritableGadget, NewText);
				ExitedSetGadgetValue();

				EnableGadget(WritableGadget, /*(!InheritsComponent) &&*/
							 (EditingColour->GetType() != COLOURTYPE_TINT));
				HideGadget(WritableGadget, FALSE);
			}
		}
	}
	//CheckDialogSize();
}



/********************************************************************************************

>	void ColourEditDlg::SetColour(BOOL SetComponents = TRUE)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/6/94

	Inputs:		SetComponents - TRUE if you wish to read all information, including the
				component values - this may change the appearance of the colour, and thus
				causes much of the dialogue to be redrawn (and may also coerce the colour
				model). If the only thing which has changed is the name (for example) we
				don't want to change the model and/or redraw stuff, so we set this value
				to FALSE to stop it faffing around.
				This parameter is ignored for Tints, for which the components are read-only

	Purpose:	Reads the colour definition from the window into the colour which
				is currently being edited, and broadcasts a colour changing message
				so everybody knows about the change.

	Scope:		private

********************************************************************************************/

void ColourEditDlg::SetColour(BOOL SetComponents)
{
	if (EditingColour == NULL)
		return;

	// Did we just get the gadget's value?
	if (IsSetGadgetValueCalled())
		return;

	BOOL Clipped = FALSE;

//	ColourContext *cc = ColourContext::GetGlobalDefault(DisplayModel);
	ColourContext *cc = NULL;
	BOOL bDeleteCC = GetColourContext(DisplayModel, &cc);	
	if (cc == NULL)
		return;

	// Make sure any chnage to the display colour model is committed
	ColourPicker::ForceColourModel(EditingColour, cc);

	// Read back the new component values. This is not done for tints, as their components
	// rely entirely upon the parent colour - reading out components is just going to
	// corrupt tinting values and stuff, and generally be a bad thing (tm).
	// Just does the set for the CurrentTypingGadget
	if (SetComponents && EditingColour->GetType() != COLOURTYPE_TINT)
	{
		String_32 CompName;

		if (cc == NULL)
			return;

		String_256 NewValue;

		if (CurrentTypingGadget!=_R(IDC_EDIT_WEBHEX))
		{
			// Work out the index number
			INT32 nIndex=0;
			if (CurrentTypingGadget  == _R(IDC_EDIT_COMPONENT1))
				nIndex=1;
			else if (CurrentTypingGadget  == _R(IDC_EDIT_COMPONENT2))
				nIndex=2;
			else if (CurrentTypingGadget  == _R(IDC_EDIT_COMPONENT3))
				nIndex=3;
			else if (CurrentTypingGadget  == _R(IDC_EDIT_COMPONENT4))
				nIndex=4;

			ERROR3IF(nIndex < 1 || nIndex > 4, "ColourEditDlg::SetColour - nIndex invalid");
			if (!nIndex)
				nIndex=1;

			if (cc->GetComponentName(nIndex, &NewValue))
			{
				NewValue = GetStringGadgetValue(CurrentTypingGadget);
			
				Clipped |= ColourPicker::SetComponentFromString(EditingColour, cc, nIndex, (String_8 *) &NewValue);
			}
		}
		else
		{
			NewValue = GetStringGadgetValue(CurrentTypingGadget);
			
			// we need to set all of the components ....
			cc = ColourContext::GetGlobalDefault(COLOURMODEL_RGBT);
			Clipped |= ColourPicker::SetComponentsFromHexString(EditingColour, cc, (String_16 *) &NewValue);
		}
		
/*
		if (cc->GetComponentName(2, &NewValue) != NULL)
		{
			NewValue = GetStringGadgetValue(_R(IDC_EDIT_COMPONENT2));
			Clipped |= ColourPicker::SetComponentFromString(EditingColour, cc, 2, (String_8 *) &NewValue);
		}

		if (cc->GetComponentName(3, &NewValue) != NULL)
		{
			NewValue = GetStringGadgetValue(_R(IDC_EDIT_COMPONENT3));
			Clipped |= ColourPicker::SetComponentFromString(EditingColour, cc, 3, (String_8 *) &NewValue);
		}

		if (cc->GetComponentName(4, &NewValue) != NULL)
		{
			NewValue = GetStringGadgetValue(_R(IDC_EDIT_COMPONENT4));
			Clipped |= ColourPicker::SetComponentFromString(EditingColour, cc, 4, (String_8 *) &NewValue);
		}
*/
	}

	// Delete the colour context if necessary
	if (bDeleteCC)
		ColourContextList::GetList()->RemoveContext(&cc);			// Have finished with it


/*
	// Set the new name of the colour - this is not done if the colour is unnamed,
	// because we don't want to accidentally convert an unnamed colour into a named one!
	BOOL NameMadeUnique = FALSE;		// Have we had to change the name that was entered?
	BOOL NameHasChanged = FALSE;		// Has the name been changed at all?
	if (EditingColour->IsNamed())
	{
		String_256 NewName = GetStringGadgetValue(_R(IDC_EDIT_NAMEMENU), NULL);

		// If the name is not any different, then we must not try to update it
		NameHasChanged = (NewName.CompareTo(*((StringBase *)ResultColour->GetName())) != 0);
		if (NameHasChanged)
		{
			String_64 UniqueName;
			NameMadeUnique = ColourManager::GetCurrentColourList()->
									GenerateUniqueColourName(&NewName, &UniqueName);

			EditingColour->SetName(UniqueName);
		}
	}
*/

	switch(EditingColour->GetType())
	{
		case COLOURTYPE_TINT:
			{
				if (EditingColour->TintIsShade())
				{
					String_256 XValue = GetStringGadgetValue(_R(IDC_EDIT_TINT));
					String_256 YValue = GetStringGadgetValue(_R(IDC_EDIT_SHADE));
					ColourPicker::SetShadeFromStrings(EditingColour, &XValue, &YValue);
				}
				else
				{
					String_256 NewValue = GetStringGadgetValue(_R(IDC_EDIT_TINT));
					ColourPicker::SetTintFromString(EditingColour, &NewValue);
				}

				Clipped = TRUE;		// Ensure controls are updated too
			}
			break;

		case COLOURTYPE_LINKED:			
			EditingColour->SetInheritsComponent(1, GetBoolGadgetSelected(_R(IDC_EDIT_INHERIT1)));
			EditingColour->SetInheritsComponent(2, GetBoolGadgetSelected(_R(IDC_EDIT_INHERIT2)));
			EditingColour->SetInheritsComponent(3, GetBoolGadgetSelected(_R(IDC_EDIT_INHERIT3)));
			EditingColour->SetInheritsComponent(4, GetBoolGadgetSelected(_R(IDC_EDIT_INHERIT4)));
			break;

		default:
			break;
	}

	if (EditingColour->IsDifferent(*ResultColour))
		EditingColourHasChanged((SetComponents && Clipped), SetComponents);
/*
	else if (NameHasChanged)
	{
		EditingColourHasChanged(FALSE, FALSE);

		SetColourNameList();	// The name has changed, so update the name list
		
//		if (NameMadeUnique)		// We read the name and then changed it to ensure it was
//			SetControls();		// a unique name, so we must ensure the control is updated
	}
*/
}



/********************************************************************************************

>	void ColourEditDlg::EditThisColour(ColourList *NewParentList,
										IndexedColour *ColourToEdit,
										BOOL TheWindowIsOpen = TRUE)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/11/94
	Inputs:		NewParentList - The list in which the ColourToEdit resides

				ColourToEdit - The colour to be edited

				TheWindowIsOpen - TRUE if the window is believed to be open now, FALSE
				if it is not. Used by the create handler to stop us from poking at the
				window when it isn't actually open yet!
	Outputs:	-
	Returns:	-
	Purpose:	Switches the colour editor, when it is open, to edit a different colour
				If the ColourToEdit is the colour currently being edited, nothing happens
	Scope:		protected
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void ColourEditDlg::EditThisColour(ColourList *NewParentList, IndexedColour *ColourToEdit,
									BOOL TheWindowIsOpen)
{
	if (ColourToEdit == ResultColour)					// We're already editing it!
		return;

	if (Document::GetSelected() == NULL || Document::GetSelected()->GetIndexedColours() != NewParentList)
	{
		ERROR3("Colour editor: No selected doc or Colour not in selected doc");
		return;
	}

	if (ColourToEdit == NULL)
	{
		ERROR3("Colour editor EditThisColour: What colour?! It's NULL!");
		return;
	}

	// Check that the colour given really truly is in the parent list
	if (NewParentList != NULL &&													// Have valid pointers
			NewParentList->FindPosition(ColourToEdit) < 0 &&						// Isn't in named colours
			NewParentList->GetUnnamedColours()->FindPosition(ColourToEdit) < 0 )	// Isn't in unnamed colours
	{
		ERROR3("Colour editor: attempt to edit colour with bogus parent list pointer");
		return;
	}

	State.ParentListOK = FALSE;		// Swapping colours - ensure we update parent list

	if (TheWindowIsOpen)	// Only poke at the window if it's actually open!
	{
		if (NewParentList == NULL || ColourToEdit == NULL || ColourToEdit->IsDeleted())
		{
			// It's DELETED! Shade the window and exit
			ShadeMyself();
			return;
		}

		ShadeMyself(TRUE, TRUE);				// Unshade the window contents
	}

	ParentList = NewParentList;

	if (EditingColour != NULL)
		delete EditingColour;

	FirstEdit = TRUE;					// Remember that we haven't applied this colour yet

	EditingColour	= new IndexedColour(*ColourToEdit);	// The colour we're working on for now
	if (EditingColour == NULL)
	{
		InformError();
		ShadeMyself();
		return;
	}

	ResultColour	= ColourToEdit;						// Where the result goes when 'OK'd

	// Copy the original colour into the OriginalColour.
	// We only need the colour definition, but we do need the full accuracy of an
	// IndexedColour to ensure rounding errors (and thus slight dither differences in
	// redraw) do not occur.
	OriginalColour = *ResultColour;

	StatusLine* pStatusLine = StatusLine::Get();

	if (EditingColour->IsNamed())
	{
		if (!pStatusLine || !pStatusLine->IsRestrictedAccessToColourPicker ())
		{
			if (AutoModelChangeN)
				DisplayModel = OriginalColour.GetColourModel();
			else
				DisplayModel = (ColourModel) DefaultDisplayModelN;
		}
	}
	else
	{	
		if (!pStatusLine || !pStatusLine->IsRestrictedAccessToColourPicker ())
		{
			if (AutoModelChange)
				DisplayModel = OriginalColour.GetColourModel();
			else
				DisplayModel = (ColourModel) DefaultDisplayModel;
		}
	}
// WEBSTER - markn 14/1/97
// Make sure the display model is either HSV or RGB
// If it's neither RGB or HSV, force it to HSV
#ifdef WEBSTER
	if (DisplayModel != COLOURMODEL_RGBT && DisplayModel != COLOURMODEL_HSVT)
		DisplayModel = COLOURMODEL_HSVT;
#endif // WEBSTER
												   
	// Ensure the OriginalColour does not reference any other colour
	OriginalColour.SetLinkedParent(NULL, COLOURTYPE_NORMAL);

	if (TheWindowIsOpen)
		InvalidateAndSetControls();
}



/********************************************************************************************

>	static void ColourEditDlg::SetEditingColour (IndexedColour* newEditingColour, Node* pSourceNode = NULL)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/11/99
	Inputs:		newEditingColour - the IndexedColour that is to be set as EditingColour
	Outputs:	-
	Returns:	-
	Purpose:	Camelots new custom colour picker control (also written by me) needs to be
				able to change the value EditingColour on the fly (based upon the
				IndexedColour that is has just read/generated) so that we do not generate
				infinite amounts of undo information (which would be so if we always applied
				the supplied colour).  This function allows us to achieve this functionality.
				Since this function is static, to access non-static member functions from
				within, we need to use TheEditor->function () otherwise VC6 complains.
	Scope:		public
	Errors:		-
	SeeAlso:	ColourEditDlg::ApplyEditingColourNow ()

********************************************************************************************/

void ColourEditDlg::SetEditingColour (IndexedColour* newEditingColour, Node* pSourceNode)
{	
	// if we havn't saved an abort colour yet - then we save one here
	// otherwise we use the colour that weve already saved as our abort colour ....
	if (TheEditor->abortColourState == SAVE_ABORTCOLOUR)
	{
		if (TheEditor->EditingColour != NULL)
		{
			if (TheEditor->AbortColour != NULL)
			{
				if (!(TheEditor->AbortColour->IsNamed ()))
				{
					delete (TheEditor->AbortColour);
				}
			}
			TheEditor->AbortColour = new IndexedColour (*(TheEditor->EditingColour));
			TheEditor->abortColourState = SAVED_ABORTCOLOUR;
		}
	}
	
	if (!(TheEditor->OriginalColour.IsNamed ()))
	{
		if (!(newEditingColour->IsNamed()))
		{	
			if (TheEditor->EditingColour != NULL)
			{
				delete (TheEditor->EditingColour);
			}

			TheEditor->EditingColour = new IndexedColour (*newEditingColour);
		}
		else
		{	
			if (TheEditor->EditingColour != NULL)
			{
				delete (TheEditor->EditingColour);
			}

			// we do some nasty stuff here - we use an unnamed colour to represent named colours
			// internally; and then convert (well actually we don't ever convert back again
			// cause this makes things more difficult - and I have more important things to be
			// doing with my time than working out why applying a named colour from this stuff
			// is such a pain in the but!)
			
			TheEditor->EditingColour = new IndexedColour (*newEditingColour);
			TheEditor->EditingColour->SetUnnamed ();
		}
	}
	else
	{
		if (TheEditor->EditingColour != NULL)
		{
			delete (TheEditor->EditingColour);
		}

		TheEditor->EditingColour = new IndexedColour (*newEditingColour);
		TheEditor->EditingColour->SetName (*(TheEditor->OriginalColour.GetName ()));
	}

	if (TheEditor->EditingColour == NULL)
	{
		InformError();
		TheEditor->ShadeMyself();
		return;
	}

	TheEditor->EditingColourHasChanged (TRUE, TRUE, TRUE, pSourceNode);		// force everything to redraw
}


/********************************************************************************************

>	void ColourEditDlg::CloseColourEditor()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/5/2000
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	A public method to close the colour editor dialog
	Scope:		public
	Errors:		-
	SeeAlso:

********************************************************************************************/

void ColourEditDlg::CloseColourEditor()
{
	CloseMyself();
}


/********************************************************************************************

>	void ColourEditDlg::SetDoTimeProcessing(BOOL Value)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/5/2000
	Inputs:		boolean
	Outputs:	-
	Returns:	-
	Purpose:	sets a flag which indicates whether or not the colour picker will do idle 
				processing.  If this is set to false the colour picker is effectively disabled.
	Scope:		public
	Errors:		-
	SeeAlso:

********************************************************************************************/

void ColourEditDlg::SetDoTimerProcessing(BOOL Value)
{
	m_bDoTimerProcessing = Value;
	if (!Value)
		EndTimedProcessing();
	else
		BeginTimedProcessing();
}



/********************************************************************************************

>	BOOL ColourEditDlg::GetDoTimeProcessing()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/5/2000
	Inputs:		-
	Outputs:	-
	Returns:	Whether or not timer processing is currently enabled
	Purpose:    Access fn.
	Scope:		public
	Errors:		-
	SeeAlso:

********************************************************************************************/

BOOL ColourEditDlg::GetDoTimerProcessing()
{
	return m_bDoTimerProcessing;
}

/********************************************************************************************

>	static void ColourEditDlg::ApplyEditingColourNow ()

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/11/99
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Camelots new custom colour picker control (also written by me) needs to be
				able to change the value EditingColour on the fly (based upon the
				IndexedColour that is has just read/generated) so that we do not generate
				infinite amounts of undo information (which would be so if we always applied
				the supplied colour).  But we also need to be able to apply the edited colour
				to the selection when the user has finished dragging my colour picker control.
				This function allows us to achieve this functionality.

				The function also (directly) applies named colours because of some nastiness
				that I have found it necessary to treat named colours as unnamed colours; and
				then revert at the end.
	Scope:		public
	Errors:		-
	SeeAlso:	ColourEditDlg::SetEditingColour ()

********************************************************************************************/

void ColourEditDlg::ApplyEditingColourNow (BOOL UseAbortColour)
{
	if (TheEditor->EditingColour != NULL)
	{
		TheEditor->SetColour (FALSE);
	}
}



/********************************************************************************************

>	static void AbortColourPickerDrag

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/03/2004
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Terminate the colour picker drag with extreme prejudice
				(I.e. pass non-zero WPARAM to prevent it calling us back)
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void ColourEditDlg::AbortColourPickerDrag()
{
	if (TheEditor)
		TheEditor->ColourPickerAbort(_R(IDC_COLOURPICKER), 1);
}



/********************************************************************************************

>	static BOOL DoAbortColourNow (CWindowID colourPicker)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/2/2000
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Allows camelots new custom colour picker control to abort the changing of
				a colour (by way of the user pressing the escape key).  This function simply
				applies the abort colour (which was already 'saved' within SetEditingColour ().
	Scope:		public
	Errors:		-
	SeeAlso:	ColourEditDlg::SetEditingColour ()

********************************************************************************************/

BOOL ColourEditDlg::DoAbortColourNow (CWindowID colourPicker)
{
	// firstly, lets check for obvious insanity
	
	String_256 ClassNameStr;  // The control type

	// Check passed a sensible window ID
	if (TheEditor && (TheEditor->WindowID == colourPicker))
	{		
		// only reset colour if we have a prior one to select (AbortColour). fixes #10649 - 
		// crash when escape is pressed before the dropper has selected a colour...
		if (TheEditor->AbortColour != NULL)
		{
			if (TheEditor->EditingColour != NULL)
			{
				delete (TheEditor->EditingColour);
			}
			TheEditor->EditingColour = new IndexedColour (*(TheEditor->AbortColour));
			ApplyEditingColourNow (TRUE);		// this generates two bits of undo - which is undesirable!
		}
		return (TRUE);
	}

	ENSURE(FALSE, "DoColourPickerColour called for invalid colour picker control");
	return (FALSE);
}



/********************************************************************************************

>	static void ColourEditDlg::SetDefaultDisplayModel (INT32 newDisplayModel)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/11/99
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Camelots new custom colour picker control has changed the way in which the
				old default colour space stuff worked.  If a user selects a colour space on
				the combobox - then we SHOULD remember this - and NOT jump straight back
				into the default (i.e. we should now set the selection as the default).
				We need to do this in two places:  1) internally from within
				ColourEditDlg::Message (); and 2) when the user sets the defaut model
				for (local colours) from within the camelot options dialog.
				This function allows us to achieve the second of these.
	Scope:		public
	Errors:		-
	SeeAlso:	ColourEditDlg::Message () (if (Msg->GadgetID == _R(IDC_EDIT_COLMODEL)) stuff)

********************************************************************************************/

void ColourEditDlg::SetDefaultDisplayModel (INT32 newDisplayModel)
{
	TheEditor->DefaultDisplayModel = TheEditor->DisplayModel = (ColourModel) newDisplayModel;
	TheEditor->InvalidateAndSetControls ();
}



/********************************************************************************************

>	void ColourEditDlg::InvalidateAndSetControls(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/11/94
	Purpose:	Invalidates all of the GDraw colour-display areas (picker, patch,
				parent patch) and also calls SetControls to set up all the controls.
				Called whenever the colour has changed so significantly that the entire
				window needs to be updated.

********************************************************************************************/

void ColourEditDlg::InvalidateAndSetControls(void)
{
	InvalidateGadget(_R(IDC_EDIT_PICKER));					// Redraw picker and colour patches
	RedrawColourNameList();								// Redraw colour name dropdown

	SetControls();										// Re-init the window controls
}



/********************************************************************************************

>	inline INT32 HalfGridLock(INT32 Position, INT32 GridSize)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/11/94

	Inputs:		Position - The X/Y position, in millipoints
				GridSize - the size of the grid to lock to, in millipoints

	Returns:	Position, locked (by rounding) to a grid of the given size, and offset by
				half a grid.

	Purpose:	Grid-locks a given plotting poisition to lie over a grid (usually the
				output device-pixel grid). The result is shifted by half the grid size to
				grid-lock to the _center_ of a pixel rather than the edges.

	Scope:		private (to ColourEditDlg rendering routines)

********************************************************************************************/

inline INT32 HalfGridLock(INT32 Position, INT32 GridSize)
{
	// By truncating down to the nearest grid point, and adding half the grid value,
	// we achieve rounding to the nearest offset-grid position.	

	// NOTE:
	// The original algorithm incorrectly rounded negative numbers towards
	// zero. Negative numbers should be rounded towards negative infinity.
	// The algorithm has been corrected by always rounding a positive number
	// and restoring the original sign of the number after rounding.

	BOOL bNegative = FALSE;				// Assume the number is positive

	if (Position < 0)					// If the number if not positive
	{									// note the fact and make positive
		bNegative	= TRUE;
		Position	= -Position;
	}

	Position -= Position % GridSize;
	Position += GridSize / 2;

	if (bNegative)						// If the number was negative
		Position = -Position;			// restore the sign

	return (Position);
}



/********************************************************************************************

>	static void GridLockRect(DocRect *TheRect, INT32 GridSize)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/11/94

	Inputs:		TheRect - A MILLIPOINT DocRect to be grid-locked
				GridSize - the size of the grid to lock to, in millipoints

	Outputs:	TheRect is updated as appropriate

	Purpose:	GridLocks all 4 coordinates of the rectangle using HalfGridLock

	Scope:		private (to ColourEditDlg rendering routines)

********************************************************************************************/

static void GridLockRect(DocRect *TheRect, INT32 GridSize)
{
	TheRect->lo.x = HalfGridLock(TheRect->lo.x, GridSize);
	TheRect->lo.y = HalfGridLock(TheRect->lo.y, GridSize);
	TheRect->hi.x = HalfGridLock(TheRect->hi.x, GridSize);
	TheRect->hi.y = HalfGridLock(TheRect->hi.y, GridSize);
}



/********************************************************************************************

>	void ColourEditDlg::InvalidatePatchGadget(ReDrawInfoType *Info = NULL)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/9/95

	Inputs:		Info - if non-NULL, points to the redraw info for the picker gadget
				(If NULL, this method will find out the info for itself)

	Purpose:	Invalidates the current colour patch to ensure it is redrawn

********************************************************************************************/

void ColourEditDlg::InvalidatePatchGadget(ReDrawInfoType *Info)
{
	ReDrawInfoType LocalInfo;

	if (Info == NULL)
		GetKernelRenderedGadgetInfo(_R(IDC_EDIT_PICKER), &LocalInfo);
	else
		memcpy(&LocalInfo, Info, sizeof(ReDrawInfoType));

	INT32 PixelSize = 72000 / LocalInfo.Dpi;		// Size of output pixel in millipoints

	DocRect PatchRect(0, 0, LocalInfo.dx, LocalInfo.dy);
	PatchRect.Inflate(-PixelSize * 4);			// Allow for the indented border

	// Now draw the original/current colour patch in the top right corner
	PatchRect.lo.x = PatchRect.hi.x - PATCHSIZE;
	PatchRect.lo.y = PatchRect.hi.y - PATCHSIZE;
	GridLockRect(&PatchRect, PixelSize);

	// Patches are horizontal if the colour model is not HSV
	BOOL HorzPatch = (DisplayModel != COLOURMODEL_HSVT);
	
	// But this setting is overridden for the special tint and shade modes
	if (EditingColour != NULL && EditingColour->GetType() == COLOURTYPE_TINT)
	{
		if (EditingColour->TintIsShade())
			HorzPatch = FALSE;
		else
			HorzPatch = TRUE;
	}

	// Translate the rect across or down as appropriate to 'HorzPatch'
	if (!HorzPatch)
		PatchRect.Translate(0, -PATCHSIZE);

	InvalidateGadget(_R(IDC_EDIT_PICKER), &LocalInfo, &PatchRect);
}



/********************************************************************************************

>	void ColourEditDlg::CloseMyself(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/11/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Cleans up, discards data on the colour being edited, and closes the
				editor window. Used to close the editor in response to different messages,
				e.g. when the selected document changes, or when the colour is deleted.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void ColourEditDlg::CloseMyself(void)
{
	if (EditingColour != NULL)
	{
		// Ensure we remember the current model as the preferred default
		if (EditingColour->IsNamed())
			DefaultDisplayModelN = (INT32) DisplayModel;
// But do not overwrite the preference for local colours - this must be set in the options dlg
//		else
//			DefaultDisplayModel = (INT32) DisplayModel;

		delete EditingColour;
		EditingColour = NULL;
	}
	ParentList = NULL;
	State.ParentListOK = FALSE;		// Swapping colours - ensure we update parent list

	Close();
	End();
}




/********************************************************************************************

>	void ColourEditDlg::ShadeMyself(BOOL UnShade = FALSE, BOOL OtherGadgetsOnly = FALSE)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/12/94
	Inputs:		UnShade - FALSE to shade, TRUE to un-shade the dialogue
				OtherGadgetsOnly - FALSE to shade all gadgets. TRUE to only shade the
				'ordinary' gadgets (those which are nothing to do with tint/link stuff)
	Outputs:	-
	Returns:	-
	Purpose:	Shades/Unshades the colour editor dialogue.
				Used to shade the dialogue while there is nothing available to edit,
				and return it to a useful state when something becomes available for
				editing.

********************************************************************************************/

void ColourEditDlg::ShadeMyself(BOOL UnShade, BOOL OtherGadgetsOnly)
{
	if (AmShaded != UnShade)		// Already in the required state. Leave it alone
		return;

	State.ParentListOK = FALSE;		// Ensure we update parent list

	AmShaded = (UnShade) ? FALSE : TRUE;

	if (AmShaded)
	{
		if (EditingColour != NULL)
			delete EditingColour;

		EditingColour = NULL;
		ResultColour = NULL;
		ParentList = NULL;

		if (NameDropDown != NULL)
		{
			NameDropDown->ClearList();
			NameDropDown->ClearAllSpecialEntries();
		}

		if (ParentDropDown != NULL)
			ParentDropDown->ClearList();
	}

	INT32 i = 0;
	while (OtherGadgetIDs[i])
	{
		EnableGadget(OtherGadgetIDs[i], UnShade);
		i++;
	}
					// Force-redraw the colour patches and picker
	InvalidateGadget(_R(IDC_EDIT_PICKER));

//	if (AmShaded)	// If we are shading the dialogue, set the colour name to 'No colour'
//		SetStringGadgetValue(_R(IDC_EDIT_NAMEMENU), _R(IDS_NOCOLOUR), FALSE, -1);

	if (OtherGadgetsOnly)			// Don't do tint/link gadgets - return now
		return;	

	i = 0;
	while (TintGadgetIDs[i])
	{
		EnableGadget(TintGadgetIDs[i], UnShade);
		i++;
	}

	i = 0;
	while (LinkGadgetIDs[i])
	{
		EnableGadget(LinkGadgetIDs[i], UnShade);
		i++;
	}
}


/********************************************************************************************

>	void ColourEditDlg::ApplyNoColour(BOOL LineColour)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/12/96

	Inputs:		LineColour - TRUE if you want to set the line colour, FALSE if you want to set the fill colour

	Purpose:	Sets either the fill or the line colour to "No colour"

				Needed for WEBSTER

	SeeAlso:	-

********************************************************************************************/

void ColourEditDlg::ApplyNoColour(BOOL LineColour)
{
	DocColour ColourToApply(COLOUR_TRANS);

	if (LineColour)
	{
		// Line colour selected so create a line colour attribute
		AttrStrokeColourChange* pAttrib = new AttrStrokeColourChange;
		if (pAttrib == NULL)
			return;

		pAttrib->SetStartColour(&ColourToApply);

		// AttributeSelected knows what to do with a selected attribute
		AttributeManager::AttributeSelected(NULL, pAttrib); 
	}
	else
	{	
		// Rather than setting the flat fill colour, we do an AttrColourChange, which
		// (I assume) will handle things like selected grad fill points, etc.
		AttrColourChange* pAttrib = new AttrColourChange;
		if (pAttrib == NULL)
			return;

		pAttrib->SetStartColour(&ColourToApply);

		// AttributeSelected knows what to do with a selected attribute
		AttributeManager::AttributeSelected(NULL, pAttrib); 
	}
}

/********************************************************************************************

>	FIXED24 ColourEditDlg::Nearest20Percent(FIXED24 n)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/12/96
	Inputs:		n = the colour component (number between 0.0 & 1.0 inclusive
	Returns:	The colour component value that is a multiple of 20%, nearest to the inout value
	Purpose:	Finds the nearest 20% value for this component, e.g. if n=0.23, the result will be 0.2
				as this is the closest 20% value.

				Needed for WEBSTER

	SeeAlso:	-

********************************************************************************************/

/* Not needed anymore (Markn 16/7/97)
// Superceded by PaletteManager::FindNearestBrowserColour()

FIXED24 ColourEditDlg::Nearest20Percent(FIXED24 n)
{
	double d = n.MakeDouble();

	ERROR3IF(d > 1.0 || d < 0.0,"I thought this value should be between 0.0 and 1.0 inclusive");

	double r = fmod(d,0.2);

	if (r > 0.1)
		d += (0.2-r);
	else
		d -= r;

	return (FIXED24(d));
}
*/

/********************************************************************************************

>	void ColourEditDlg::LimitTo216Only()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/12/96

	Inputs:		-

	Purpose:	Snaps the current colour to the closest colour in the web browseer palette.

				Needed for WEBSTER

	SeeAlso:	-

********************************************************************************************/

void ColourEditDlg::LimitTo216Only()
{
	if (EditingColour == NULL)
		return;

	ColourContext *CCrgbt = ColourManager::GetColourContext(COLOURMODEL_RGBT);

	ColourRGBT RGBColour,result;
	CCrgbt->ConvertColour(EditingColour, (ColourGeneric *)&RGBColour);

	PaletteManager::FindNearestBrowserColour(RGBColour,&result);

//	IndexedColour NewRGBIndexedColour(COLOURMODEL_RGBT, (ColourGeneric *)&result);
//
//	*EditingColour = NewRGBIndexedColour;

	if (EditingColour->GetColourModel() == COLOURMODEL_RGBT)
	{
		ColourPicker::SetComponentFromDouble(EditingColour, CCrgbt, 1, result.Red.MakeDouble());
		ColourPicker::SetComponentFromDouble(EditingColour, CCrgbt, 2, result.Green.MakeDouble());
		ColourPicker::SetComponentFromDouble(EditingColour, CCrgbt, 3, result.Blue.MakeDouble());
	}
	else
	{
		IndexedColour NewRGBIndexedColour(COLOURMODEL_RGBT, (ColourGeneric *)&result);

		ColourContext *CChsvt = ColourManager::GetColourContext(COLOURMODEL_HSVT);

		ColourHSVT hsv;
		CChsvt->ConvertColour(&NewRGBIndexedColour, (ColourGeneric *)&hsv);

		ColourPicker::SetComponentFromDouble(EditingColour, CChsvt, 1, hsv.Hue.MakeDouble());
		ColourPicker::SetComponentFromDouble(EditingColour, CChsvt, 2, hsv.Saturation.MakeDouble());
		ColourPicker::SetComponentFromDouble(EditingColour, CChsvt, 3, hsv.Value.MakeDouble());
	}

	EditingColourHasChanged(TRUE,TRUE,TRUE);

//	InvalidateAndSetControls();	// And redraw the picker, and put the new values in the writables

/// 
//search for "DisplayModel =" to find where it sets the display colour model
//search for "Command" to find all the menu handling code
}

/********************************************************************************************

>	void ColourEditDlg::FindUsefulColourToEdit(BOOL WantLineColour = FALSE,
												BOOL TheWindowIsOpen = TRUE)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/1/95

	Inputs:		WantLineColour - TRUE if you want  line colour, FALSE if you want a fill colour
				
				TheWindowIsOpen - FALSE if the window is closed, TRUE if it's open
				(used by the create code to stop us poking at the window when it's closed!

	Purpose:	Calls the ColourManager method (cf SeeAlso) to determine which colour should
				be displayed in the editor, and swaps to editing this colour (or if no
				colour is available, shades the editor)

	SeeAlso:	ColourManager::FindColourOfInterestToUser; ColourEditDlg::EditThisColour;
				ColourEditDlg::ShadeMyself

********************************************************************************************/

void ColourEditDlg::FindUsefulColourToEdit(BOOL WantLineColour, BOOL TheWindowIsOpen)
{
	State.ParentListOK = FALSE;		// Swapping colours - ensure we update parent list

	ColourList		*NewParentList  = NULL;
	IndexedColour	*IxColourToEdit = NULL;
	DocColour		DocColourToEdit;

	EditingLineColour = WantLineColour;		// Remember whether editing line or fill

	ColourManager::FindColourOfInterestToUser(&DocColourToEdit, &NewParentList, WantLineColour);

	if (NewParentList == NULL)
	{
		if (TheWindowIsOpen)
			ShadeMyself();
	}
	else
	{
		NoFillButtonDown = DocColourToEdit.IsTransparent();

		if (NoFillButtonDown)
		{
			AttributeManager &AttrMgr = Document::GetSelected()->GetAttributeMgr();

			DocColour LineCol;
			DocColour FillCol;
				
			AttrMgr.GetCurrentLineAndFillColour(CC_RUNTIME_CLASS(NodeRenderableInk),
													&LineCol, &FillCol);
			if (WantLineColour)
				DocColourToEdit = LineCol;
			else
				DocColourToEdit = FillCol;

			// if the default is no fill DON'T set that because this is the colour we get
			// when we are explicitly removing no fill
			if ( DocColourToEdit.IsTransparent() )
				DocColourToEdit = DocColour(COLOUR_WHITE);//prob should set to page colour
		}
		IxColourToEdit = ColourManager::GenerateNewUnnamedColour(NewParentList, &DocColourToEdit);

		if (EditingColour == NULL || IxColourToEdit != NULL)
			EditThisColour(NewParentList, IxColourToEdit, TheWindowIsOpen);
		else
		{
			if (TheWindowIsOpen)
				ShadeMyself();
		}
	}
}



/********************************************************************************************

>	MsgResult ColourEditDlg::Message( Msg* Message)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/6/94
	Inputs:		Msg - The dialogue manager message to handle
	Outputs:	-
	Returns:	A MsgResult
	Purpose:	Standard DialogOp message handler, for the Colour Editor dialogue

	Notes:		All code in this method must be able to handle being called when
				EditingColour == NULL (the window is shaded)

	Errors:		-
	SeeAlso:	-

********************************************************************************************/

MsgResult ColourEditDlg::Message( Msg* Message )
{
	// DY 5/5/2000 this is really ugly but I'm in a hurry.  Basically I need to disable background processing
	// whilst a brush stroke is in progress, at the end of the drag normal servive will be resumed
	if (!m_bDoTimerProcessing)
		return(DialogOp::Message(Message));

	if (MESSAGE_IS_A(Message, ColourChangingMsg))
	{
		ColourChangingMsg *TheMsg = (ColourChangingMsg *) Message;

		switch ( TheMsg->State )
		{
			case ColourChangingMsg::COLOURUPDATED:
			case ColourChangingMsg::COLOURUPDATEDINVISIBLE:
				// If I didn't send the message and the colour is the one I'm editing,
				// reset the controls to reflect whatever the change was.
				// (This mainly happens as a result of undo)
				if (!ISentTheMessage && TheMsg->ChangedColour == ResultColour)
				{
					// Delete our current copy of the EditingColour, and get a fresh one
					// as a copy of the newly-changed ResultColour
					if (EditingColour != NULL)
						delete EditingColour;

					EditingColour = new IndexedColour(*ResultColour);
					if (EditingColour == NULL)
						InformError();	// Report the memory error and continue

					InvalidateAndSetControls();
				}
				else if (ResultColour != NULL && ResultColour->IsADescendantOf(TheMsg->ChangedColour))
				{
					// If an ancestor of the colour being edited has changed, redraw the editor
					// to take on board the change in appearance
					InvalidateAndSetControls();
				}
				break;

			case ColourChangingMsg::LISTPAGED:		// May have swapped to another list
				if (TheMsg->NewColourList != ParentList)
				{
					ResultColour = NULL;						// Lose this colour
					if (EditingColour != NULL)
						delete EditingColour;
					EditingColour = NULL;

					UpdateOnNextIdle = TRUE;
					UpdateColourEditor ();
				}
/*
				if (TheMsg->NewColourList != ParentList)
				{
					ResultColour = NULL;						// Lose this colour
					if (EditingColour != NULL)
						delete EditingColour;
					EditingColour = NULL;

					FindUsefulColourToEdit(EditingLineColour);	// Try to find a replacement
					
					if (EditingColour == NULL)					// We failed - shade the dialogue
						ShadeMyself();
				}
*/
				break;

			case ColourChangingMsg::LISTDELETED:	// May have deleted this colour
				// The ColourList we're editing in has been deleted from under us!
				if (TheMsg->NewColourList == ParentList)
					ShadeMyself();
				break;

			case ColourChangingMsg::LISTDESELECTED:// No document available
				ShadeMyself();
				break;

			case ColourChangingMsg::LISTUPDATED:	// Colour may have been deleted
				{
					//BOOL SetControls = FALSE;
	
					// If the colour is deleted out from under us, try to find another one to edit
					if (ResultColour != NULL && ResultColour->IsDeleted())
					{
						ResultColour = NULL;						// Lose this colour
						if (EditingColour != NULL)
							delete EditingColour;
						EditingColour = NULL;

						FindUsefulColourToEdit(EditingLineColour);	// Try to find a replacement
					
						if (EditingColour == NULL)					// We failed - shade the dialogue
							ShadeMyself();
					}

					if (EditingColour != NULL)
					{
						if (EditingColour->FindLastLinkedParent() != NULL &&
							EditingColour->FindLastLinkedParent()->IsDeleted())
						{
							// If the parent of EditingColour has been deleted, then make standalone
							IndexedColourType Type = EditingColour->GetType();
							if (Type != COLOURTYPE_SPOT)
								Type = COLOURTYPE_NORMAL;

							EditingColour->SetLinkedParent(NULL, Type);
							//SetControls = TRUE;
						}
						else if (EditingColour->GetType() == COLOURTYPE_NORMAL &&
								 ResultColour != NULL && ResultColour->FindLinkedParent() != NULL)
						{
							// ResultColour's parent may have just become un-deleted (by an UNDO).
							// If this could be the case, and EditingColour has no parent, then
							// we must have set EditingColour to have no parent just above (i.e. the
							// user deleted the parent and then hit UNDO)
							// In this case, set EditingColour back to the values in ResultColour
							EditingColour->SetLinkedParent(ResultColour->FindLinkedParent(), ResultColour->GetType());
							//SetControls = TRUE;
						}
					}

					// Ensure the colour name and type dropdowns are updated to offer sensible options
					State.ParentListOK = FALSE;
					State.Initialised = FALSE;
					InvalidateAndSetControls();
				}
				break;

			case ColourChangingMsg::SELVIEWCONTEXTCHANGE:
				// The selected view's colour context has chnaged, which probably affects the
				// colour correction/separation options. We redraw using these options, so we
				// need to redraw to reflect the new settings.
				InvalidateAndSetControls();
				break;

			default:
				break;
		}

		return(DialogOp::Message(Message));
	}

	if (MESSAGE_IS_A(Message, SelChangingMsg))		// Selection changed - edit new colour
	{
		SelChangingMsg *Msg = (SelChangingMsg *) Message;
		switch ( Msg->State )
		{
			case SelChangingMsg::COLOURATTCHANGED:
			case SelChangingMsg::SELECTIONCHANGED:
			case SelChangingMsg::NODECHANGED:
				if (!ISentTheMessage)
				{
					UpdateOnNextIdle = TRUE;
					UpdateColourEditor ();
				}
//					FindUsefulColourToEdit(EditingLineColour);
				break;

			default:
				break;
		}
	}
	else if (MESSAGE_IS_A(Message, CurrentAttrChangedMsg))	// Current attrs changed - edit new colour
	{
		if (!ISentTheMessage)
		{
			UpdateOnNextIdle = TRUE;
			UpdateColourEditor ();
		}
//			FindUsefulColourToEdit(EditingLineColour);
	}
	else if (MESSAGE_IS_A(Message, DragMessage))
	{
		// If a drag starting message comes around, pass it on to the tree
		DragMessage *Msg = (DragMessage *) Message;
		if (Msg->State == DragMessage::DRAGSTARTED)
		{
			// If it's a colour drag which did NOT originate from the editor, then attach
			// a drag target so it can be dropped into the editor.
			if (Msg->pInfo->IsKindOf(CC_RUNTIME_CLASS(ColourDragInformation)) &&
				!IS_A(Msg->pInfo, ColEditorDragInfo))
			{
				ColourDragInformation *CDI = (ColourDragInformation *)Msg->pInfo;
				
				// Check if it's a library colour, or else it must be a colour in the selected doc
				if (CDI->GetParentDoc() == NULL || CDI->GetParentDoc() == Document::GetSelected())
				{
					// Create targets for all the interesting bits of the window
					// **** !!!! ToDo (maybe) Targets for colour picker and colour patches

					// Last, add one for the whole window - this is lower priority than the
					// others, so will only be active for any areas which are not claimed by
					// the above targets
					/*ColEditorDragTarget * NewTarget = */ new ColEditorDragTarget(this, 0);
				}
				// We don't really care if this failed...
			}			
		}
	}

// WEBSTER - markn 9/1/97
// This message shouldn't need servicing in Webster.
//#ifndef WEBSTER
	else if (MESSAGE_IS_A(Message, OptionsChangingMsg))
	{
		OptionsChangingMsg *Msg = (OptionsChangingMsg *) Message;
		if (Msg->State == OptionsChangingMsg::NEWUNITS)
		{
			// A unit has changed. This can include the decimal point character, so we need
			// to set the values in all writable fields again & get the default.
			SetUnitGroupDefaults(DisplayModel);
			SetControls();
		}
	}
//#endif // WEBSTER
	else if (MESSAGE_IS_A(Message, DocViewMsg))
	{
		DocViewMsg *Msg = (DocViewMsg *) Message;

		if (Msg->State == DocViewMsg::SELCHANGED)
		{
			// Selected DocView is changing - redraw to use the new DocView's colour context
			BOOL DoRedraw = TRUE;
			if (Msg->pOldDocView != NULL && Msg->pNewDocView != NULL)
			{
				// If we know the old & new views, then see if they have the same colour
				// context attached - if they do, there's no need to redraw. This eliminates
				// flicker when swapping normal views (the most common view-swap action)
				// We only check the RGB context because we assume the screen is always RGB
				ColourContext *OldCC = Msg->pOldDocView->GetColourContext(COLOURMODEL_RGBT, TRUE);
				ColourContext *NewCC = Msg->pNewDocView->GetColourContext(COLOURMODEL_RGBT, TRUE);

				if (OldCC == NewCC)
					DoRedraw = FALSE;
			}

			if (DoRedraw)
			{
				UpdateOnNextIdle = TRUE;
				UpdateColourEditor ();
			}
//				InvalidateAndSetControls();
		}
	}


	if (!(IS_OUR_DIALOG_MSG(Message)))
		return(DialogOp::Message(Message));

	DialogMsg* Msg = (DialogMsg*)Message;

#if _DEBUG
/*
	if ((INT32)Msg->DlgMsg == (INT32)DIM_TITLEFOCUSWARN + 1)
	{
		if (CurrentCursor != NULL)
		{
			delete CurrentCursor;
			CurrentCursor = NULL;
		}

		if (CurrentCursorID != NULL)
		{
			CurrentCursor = new Cursor(CurrentCursorID);
			if (CurrentCursor != NULL)
				CurrentCursor->SetActive();
		}
		else
		{
 			if (Cursor::Arrow != NULL)
				Cursor::Arrow->SetActive();
		}
		return(DialogOp::Message(Message));
	}
*/
#endif

	switch(Msg->DlgMsg)
	{
		case DIM_TITLEFOCUSWARN:
			// --- Input focus bodge ---
			// The window has been moved or the titlebar clicked. This means we've been given
			// the input focus, and we don't want that. So we throw the focus away again.
			// Unfortunately this does not work as we immediately get the focus again! Thus,
			// we set up an idle processor to be called back once all the moving has been
			// finished, so that we lose the focus shortly after the drag completes, and
			// the effect is more permanent!			
			DragStartArea = CEDRAG_NONE;	// Tell idle system we're not in the middle of a picker drag

			//LoseKbdFocusPending = TRUE;		// And flag the fact that we want to lose the focus
			//BeginTimedProcessing();
			//LoseKeyboardFocus ();
			DialogManager::DefaultKeyboardFocus();
			break;

		case DIM_CREATE:
			ColourPicker::OnCreate(WindowID);
			SetGadgetBitmaps(_R(IDC_EDIT_DROPMENU), 0, 0);
			#ifndef WEBSTER
			SetGadgetBitmaps(_R(IDC_EDIT_MAKESTYLE), 0, 0);
			SetGadgetBitmaps(_R(IDC_EDIT_ADVANCED), 0, 0);
			#endif
			// WEBSTER - markn 11/12/96
			// Changes to the bitmap buttons at the top of the colour editor
			// Set up our bitmap button gadgets with appropriate bitmaps
			//SetGadgetBitmaps(_R(IDC_EDIT_LINEFILL), 0, 0);	
			SetGadgetBitmaps(_R(IDC_EDIT_NOCOLOUR), 0, 0);	
			SetGadgetBitmaps(_R(IDC_EDIT_216ONLY), 0, 0);	
			SetGadgetBitmaps(_R(IDC_MAKE_LOCAL), 0, 0);
			SetGadgetBitmaps(_R(IDC_EDIT_RENAME), 0, 0);	
			
			
			// WEBSTER - markn 31/1/97
			// Make sure the line/fill switch is set correctly
			//#ifdef WEBSTER
			//SetBoolGadgetSelected(_R(IDC_EDIT_LINEFILL), EditingLineColour);
			//#endif // WEBSTER

			ResetState();				// Ensure all controls and window extent are updated
			SetUnitGroupDefaults(DisplayModel);	// and defaults are set
			SetControls();				// ...and update them

			ColourPicker::RelayoutDialog(WindowID);

			// And then lob away the input focus again - put it back into the mainframe
			LockLoseFocus = FALSE;		// Ensure the focus lock is reset to off whenever we open
			LoseFocusFromEditControls = FALSE;
			LoseKeyboardFocus();

			{
				INT32 i = 0;
				while (GadgetHelp[i].Gadget)
				{
					SetGadgetHelp(GadgetHelp[i].Gadget, GadgetHelp[i].BubbleID, GadgetHelp[i].BubbleID);
					i++;
				}
			}
			BeginTimedProcessing();
			break;

		case DIM_COMMIT:
//			// NOTE - this dialogue does NOT have an 'OK' (commit) button
//			// The only way that we can therefore recieve this message is if return
//			// is pressed. If this is done in the name text field, we must update
//			// the name list to show this colour's name correctly
			if (!ISentTheMessage)
			{
				SetColour(FALSE);		// Update the colour

				LoseFocusFromEditControls = TRUE;
				LoseKeyboardFocus();	// And we've committed, so lose the keyboard focus
				LoseFocusFromEditControls = FALSE;
//
//				State.ParentListOK = FALSE;
//				SetColourNameList();	// Update the parent list
			}
			break;

		case DIM_CANCEL:				// Cancel clicked
		{
			StatusLine* pStatusLine = StatusLine::Get();
			if (!pStatusLine || !pStatusLine->IsRestrictedAccessToColourPicker())
			{
				CloseMyself();			// And close the window
			}
			else
			{
				ColourPickerAbort (_R(IDC_COLOURPICKER));
			}
		}
 		return(OK);					// ... making sure the base class handler is NOT called (it blows up)

		case DIM_CTRL_RESIZED:
		{
			if (Msg->GadgetID == _R(IDC_EDIT_PICKER))
				ColourPicker::OnSize(WindowID);
		}
		break;

		case DIM_REDRAW:				// Kernel-redraw of colour patch or picker controls
		{
			RenderControl(Msg->GadgetID, (ReDrawInfoType*) Msg->DlgMsgParam);
		}
		break;

		case DIM_LFT_BN_CLICKED:
			// Remove bubble help whenever the user clicks
PORTNOTE("other", "Disabled BubbleHelp stuff")
#ifndef EXCLUDE_FROM_XARALX
			ControlHelper::BubbleHelpDisable();
#endif

			if (FALSE) {}
#if 0
			else if (Msg->GadgetID == _R(IDC_NATIVEPICKER))
			{
				if (EditingColour != NULL)
				{
					Close();		// Close ourself, and replace with the native colour picker

					ColourPicker NewPicker;
					NewPicker.EditColour(ParentList, ResultColour, TRUE);

					if (EditingColour != NULL)
					{
						delete EditingColour;
						EditingColour = NULL;
					}
					End();
				}
			}
#endif

// WEBSTER - markn 11/12/96
#ifndef WEBSTER
			else if (Msg->GadgetID == _R(IDC_EDIT_ADVANCED))
			{
				// Toggling size is allowed even when we're shaded
				Folded = !Folded;			// Toggle the folded state of the window
				SetControls();				// And re-set the window size
			}
#endif // WEBSTER

#if 0 // WEBSTER - markn 11/12/96
			else if (Msg->GadgetID == _R(IDC_EDIT_LINEFILL))
			{
				FindUsefulColourToEdit(GetBoolGadgetSelected(_R(IDC_EDIT_LINEFILL)));
			}
#endif
			
			// WEBSTER - markn 14/12/96	
			else if (Msg->GadgetID == _R(IDC_EDIT_NOCOLOUR))
			{
				if ( NoFillButtonDown )
				{
					NoFillButtonDown = FALSE;	// remember it's up
					EditingColourHasChanged( TRUE, FALSE );
				}
				else
					ApplyNoColour(EditingLineColour);//push it down
			}
			else if (Msg->GadgetID == _R(IDC_EDIT_RENAME))
			{
				DoCommand(&ColCmd_Name);
			}
			// WEBSTER - markn 14/12/96	
			else if (Msg->GadgetID == _R(IDC_EDIT_216ONLY))
			{
				LimitTo216Only();
				DialogManager::DefaultKeyboardFocus();  // remove the input focus from the button (yuk!)
			}
			else if (Msg->GadgetID == _R(IDC_EDIT_DROPMENU))
			{
				// Show drop-down menu
				// Chuck up a context sensitive menu
				ColEditContextMenu *Bob = new ColEditContextMenu;
				if (Bob != NULL)
					Bob->Show();
			}
			else if ((Msg->GadgetID == _R(IDC_EDIT_INHERIT1)) ||
					 (Msg->GadgetID == _R(IDC_EDIT_INHERIT2)) ||
					 (Msg->GadgetID == _R(IDC_EDIT_INHERIT3)) ||
					 (Msg->GadgetID == _R(IDC_EDIT_INHERIT4)))
			{
				SetColour(FALSE);			// Read the new settings from the window
				SetControls();				// Ensure controls shade/unshade as appropriate
			}
// WEBSTER - markn 14/1/97
// Removed click handling on unused buttons
			else if (Msg->GadgetID == _R(IDC_EDIT_3D))
			{
				// 3D display mode turned on/off. Redraw the picker control to show it
				// in the new mode.
				Use3DDisplay = GetBoolGadgetSelected(_R(IDC_EDIT_3D));
				InvalidateGadget(_R(IDC_EDIT_PICKER));
			}
			else if (Msg->GadgetID == _R(IDC_EDIT_MAKESTYLE))
			{
				MakeNewNColour();
			}
// can make named colours in webster #endif // WEBSTER
			else if (Msg->GadgetID == _R(IDC_MAKE_LOCAL))
			{
				// handles the webster only "Make Local to Frame" button
				OnMakeLocalToFrame();
			}

			// And then lob away the input focus again - put it back into the mainframe
			LoseKeyboardFocus();
			break;

		case DIM_LFT_BN_DOWN:
//		case DIM_RGT_BN_DOWN:
			// Remove bubble help whenever the user clicks
PORTNOTE("other", "Disabled BubbleHelp stuff")
#ifndef EXCLUDE_FROM_XARALX
			ControlHelper::BubbleHelpDisable();
#endif
			if (Msg->GadgetID == _R(IDC_COLOURPICKER))
			{
				// This little wheeze is enough to remove hover
				EnableGadget(_R(IDC_COLOURPICKER), FALSE);
				EnableGadget(_R(IDC_COLOURPICKER), TRUE);
				SetBoolGadgetSelected(_R(IDC_COLOURPICKER), FALSE);
				InvalidateGadget(_R(IDC_COLOURPICKER));

				ColourPickerDragInformation * DragCol = new ColourPickerDragInformation();
				DragManagerOp::StartDrag(DragCol, GetReadWriteWindowID());
				break;
			}

			// Drag methods all cope with shaded condition (EditingColour == NULL)
			if (Msg->DlgMsgParam)
			{
				if (Msg->GadgetID == _R(IDC_EDIT_PICKER))
				{
					StartDrag((ReDrawInfoType*) Msg->DlgMsgParam);
					NoFillButtonDown = FALSE;
				}
#if FALSE
/*
				else if (Msg->GadgetID == _R(IDC_EDIT_PATCH) && EditingColour != NULL && ParentList != NULL)
				{
					ReDrawInfoType* Info = (ReDrawInfoType *) Msg->DlgMsgParam;
					INT32 PatchHeight = Info->dy / 3;

					IndexedColour *TheColour = ResultColour;	// Middle patch/default
					if (Info->pMousePos->y < PatchHeight)
					{
						// Bottom patch - parent colour
						if (EditingColour->FindLinkedParent() != NULL)
							TheColour = EditingColour->FindLinkedParent();
					}
					else if (Info->pMousePos->y > PatchHeight * 2)
					{
						// Top patch - OriginalColour
						TheColour = new IndexedColour(OriginalColour);
						if (TheColour != NULL)
						{
							TheColour->SetUnnamed();

							// Ensure we don't get a memory leak
							ParentList->AddItem(TheColour);
						}
					}

					if (TheColour != NULL)
					{
						ColourDragInformation *DragCol;
						DragCol = new ColourDragInformation(TheColour, FALSE, (Document *)ParentList->GetParentDocument());
						DragManagerOp::StartDrag(DragCol, GetReadWriteWindowID());
					}
				}
*/
#endif
			}
			
			break;

		case DIM_MOUSE_DRAG:
			// Drag methods all cope with shaded condition (EditingColour == NULL)
			if (Msg->GadgetID == _R(IDC_EDIT_PICKER) && DragStartArea != CEDRAG_NONE)
				UpdateDrag((ReDrawInfoType*) Msg->DlgMsgParam);
			break;


/*
		case DIM_MOUSE_MOVE:
			ColourPicker::UpdateBubbleHelpAndPointer();
			break;
*/

		case DIM_LFT_BN_UP:
			// Drag methods all cope with shaded condition (EditingColour == NULL)
			if (Msg->GadgetID == _R(IDC_EDIT_PICKER) && DragStartArea != CEDRAG_NONE)
				EndDrag((ReDrawInfoType*) Msg->DlgMsgParam);
			else if (Msg->GadgetID != _R(IDC_EDIT_DROPMENU))	// Don't lose focus if dropping a menu!
				LoseKeyboardFocus();
			break;


		case DIM_RGT_BN_UP:
			if (EditingColour != NULL && !AmShaded)
			{
				// Chuck up a context sensitive menu
				ColEditContextMenu *Bob = new ColEditContextMenu;
				if (Bob != NULL)
					Bob->Show();
			}
			break;

	
		case DIM_TEXT_CHANGED:					// Text in a writable icon has changed
			if (!ISentTheMessage && EditingColour != NULL)
			{
				// If we are editing a colour, and we are not responsible for the
				// change, we update the EditingColour, and possibly force-redraw the
				// current-colour indicator patch, as appropriate to the change

				if (( Msg->GadgetID == _R(IDC_EDIT_COMPONENT1)) ||
					( Msg->GadgetID == _R(IDC_EDIT_COMPONENT2)) ||
					( Msg->GadgetID == _R(IDC_EDIT_COMPONENT3)) ||
					( Msg->GadgetID == _R(IDC_EDIT_COMPONENT4)) ||
					( Msg->GadgetID == _R(IDC_EDIT_WEBHEX)))
				{
					// While setting the colour, make sure we don't try to update the
					// field that the user is currently typing into!
					CurrentTypingGadget = Msg->GadgetID;
					SetColour();
					CurrentTypingGadget = 0;

					// We have to set the transparency button manually here 'cos SetColour
					// does not write the gadgets when we're typing (it would over write 
					// whatever we are typing!)
					if (NoFillButtonDown)	// if it's up already don't bother redrawing
					{
						NoFillButtonDown = FALSE;
						SetBoolGadgetSelected( _R(IDC_EDIT_NOCOLOUR), NoFillButtonDown);
					}


					// Set flag to tell the 'lose focus' function to also update the
					// text components once while it's at it - this just tidies up the
					// appearance of the controls as soon as we lose focus.
					TextMayBeWrong = TRUE;
				}
				else if (Msg->GadgetID == _R(IDC_EDIT_TINT))
				{
					INT32 MinValue = (EditingColour->TintIsShade()) ? -100 : 0;
					INT32 NewValue = GetLongGadgetValue(_R(IDC_EDIT_TINT), MinValue, 100);
					if (NewValue < MinValue) NewValue = MinValue;
					if (NewValue > 100)	 	 NewValue = 100;

					CurrentTypingGadget = _R(IDC_EDIT_TINT);
					SetColour(FALSE);
					CurrentTypingGadget = 0;
				}
				else if (Msg->GadgetID == _R(IDC_EDIT_SHADE))
				{
					INT32 NewValue = GetLongGadgetValue(_R(IDC_EDIT_SHADE), -100, 100);
					if (NewValue < -100) NewValue = -100;
					if (NewValue > 100)	 NewValue = 100;

					CurrentTypingGadget = _R(IDC_EDIT_SHADE);
					SetColour(FALSE);
					CurrentTypingGadget = 0;
				}
			}
			break;

//		case DIM_FOCUS_LOST:
//			// This message is sent from the Name Combo box when it loses the input focus.
//			// This occurs when we move to a different field, close the dialogue, press
//			// return, etc. It means any change to the name is only set when the user
//			// finishes editing the text, at which point it is reasonable to ensure the
//			// name is unique
//
//			if (Msg->GadgetID == _R(IDC_EDIT_NAMEMENU))
//				SetColour(FALSE);		// Update the colour
//			break;


/*
		case DIM_SLIDER_POS_CHANGING:
		case DIM_SLIDER_POS_SET:
			if (EditingColour != NULL && !ISentTheMessage)
			{
				INT32 NewValue = GetLongGadgetValue(_R(IDC_EDIT_TINTSLIDER), 0, 100);
				if (NewValue < 0)	 NewValue = 0;
				if (NewValue > 100)	 NewValue = 100;
				NewValue = 100-NewValue;

				INT32 WritableValue = GetLongGadgetValue(_R(IDC_EDIT_TINT), 0, 100);
				if (WritableValue != NewValue)
				{
					BOOL LastSentState = ISentTheMessage;	// Lock: Ignore DIM_TEXT_CHANGED
					ISentTheMessage = TRUE;
					SetLongGadgetValue(_R(IDC_EDIT_TINT), NewValue);
					ISentTheMessage = LastSentState;
					
					SetColour();
				}
			}

			// And then lob away the input focus again - put it back into the mainframe
			LoseKeyboardFocus();
			break;
*/

		case DIM_LISTDROPPED:
			// A dropdown list is about to drop down- turn on our focus-losing lock
			LockLoseFocus = TRUE;
			break;

		case DIM_TIMER:
			if(Msg->DlgMsgParam == COLED_TIMER_ID)
					//
			//UpdateOnNextIdle = TRUE;
		
			TimedProcessing();
			//UpdateColourEditor ();
		
			break;

		
		case DIM_SELECTION_CHANGED:					// Combo-box selection
		case DIM_SELECTION_CHANGED_COMMIT:			// Combo-box selection
			if (ISentTheMessage)
				break;

			if ((Msg->GadgetID == _R(IDC_EDIT_NAMEMENU)) ||
				(Msg->GadgetID == _R(IDC_EDIT_COLMODEL)) ||
				(Msg->GadgetID == _R(IDC_EDIT_COLTYPE)) ||
				(Msg->GadgetID == _R(IDC_EDIT_PARENTCOL)))
			{
				// A dropdown list has been closed up again - turn off our focus-losing lock
				LockLoseFocus = FALSE;
			}

// WEBSTER - markn 14/1/97
// Commented out the combo box handling
//#ifndef WEBSTER
			// Handle selections in the colour name combobox. This will switch to
			// editing the chosen colour
			if (Msg->GadgetID == _R(IDC_EDIT_NAMEMENU) && ParentList != NULL)
			{
				WORD SelIndex;
				GetValueIndex(_R(IDC_EDIT_NAMEMENU), &SelIndex);

				if (NameDropDown != NULL)
				{
					IndexedColour *Selected = NameDropDown->DecodeSelection((INT32)SelIndex);

					if (Selected == NULL)
					{
						if (SelIndex < 2)			// Have chosen a valid special item
						{
							// Edit fill colour or line colour
							FindUsefulColourToEdit((SelIndex == 1));
						}
						else
						{
							// Must have selected the separator line. Re-jig the list to make
							// sure the selection is shown correctly
							State.ParentListOK = FALSE;
							SetColourNameList();
						}
					}
					else
					{
						// Have selected an IndexedColour
						if (!Selected->IsDeleted() && Selected->IsNamed())	// Safety check
							EditThisColour(ParentList, Selected);
					}
				}

				// And then lob away the input focus again - put it back into the mainframe
				LoseKeyboardFocus();
				break;
			}
//#endif
			if (EditingColour == NULL)
				break;

			if (Msg->GadgetID == _R(IDC_EDIT_COLMODEL))
			{
				// A colour model has been selected. If it is different from the
				// current model, we convert the colour into the new model, and set
				// up the window to edit the colour in this new model

				WORD Index;
				GetValueIndex(_R(IDC_EDIT_COLMODEL), &Index);

				if (Index < MAX_COLOURMODELS)
				{
					ColourContextArray ColContexts;
					ColourContext::GetGlobalDefaults(&ColContexts);

					INT32 NewModelNum;

					for (NewModelNum = 0; NewModelNum < MAX_COLOURMODELS; NewModelNum++)
					{
						if (ColContexts.Context[NewModelNum] != NULL)
						{
							if ((Index--) <= 0)
								break;
						}
					}

					INT32 OldModelNum = (INT32) DisplayModel;
					if (NewModelNum < MAX_COLOURMODELS && NewModelNum != OldModelNum)
					{
						DefaultDisplayModel = DisplayModel = (ColourModel) NewModelNum;		// Switch display to the new model
						
						if (EditingColour->IsNamed())
							DefaultDisplayModelN = NewModelNum;			// And remember preference
// But do not overwrite the preference for local colours - this must be set in the options dlg
//						else
//							DefaultDisplayModel = NewModelNum;			// And remember preference

						// And set up the controls as appropriate. If we did not make the
						// change, we still need to ensure the colour model setting is
						// returned to displaying the current model name.
						// We also invalidate the colour picker to redraw it in the new display model
						InvalidateAndSetControls();
					}
				}
			}
			else if (Msg->GadgetID == _R(IDC_EDIT_COLTYPE))
			{
				// A colour model as been selected. If it is different from the
				// current model, we convert the colour into the new model, and set
				// up the window to edit the colour in this new model

				WORD Index;
				GetValueIndex(_R(IDC_EDIT_COLTYPE), &Index);

#ifdef WEBSTER 
				// "spot colour" option is not present in Webster - adjust the index appropriately
				// Index now 0-normal, 1-tint, 2-shade, 3-linked
				if (Index > 0)
					Index++;
				// Index now 0-normal, 2-tint, 3-shade, 4-linked
#endif // WEBSTER

				IndexedColour *LastParent = EditingColour->FindLastLinkedParent();

				// Check if any safe parent colours are available
				LastParent = FindSafeParent(LastParent, (Index == 2));


				// Correct index to allow for Shade colour entry (we handle tint/shade in the same 'case')
				BOOL IsShadeColour = (Index == 3) ? TRUE : FALSE;
				if (Index >= 3)
					Index--;
				// Index now 0-normal, 1-spot, 2-tint, 2-shade, 3-linked


				// If it's tint/shade(2) or linked(3) and there's no parent, scream blue murder
				if (LastParent == NULL && (Index == 2 || Index == 3))
				{
					// No suitable parents are available! Disallow change to tint/linked
					InformMessage(_R(IDS_EDIT_NOPARENT), _R(IDS_OK));
					SetControls();		// Revert the gadget to the previous type
					break;
				}

				// Get the colour definition so that we can make a sensible attempt
				// at converting it into the desired colour type without losing its appearance
				ColourContext *cc = ColourContext::GetGlobalDefault(COLOURMODEL_HSVT);
				ColourHSVT ParentDef;
				ColourHSVT ColourDef;
				cc->ConvertColour((LastParent == NULL) ? EditingColour : LastParent, (ColourGeneric *)&ParentDef);
				cc->ConvertColour(EditingColour, (ColourGeneric *)&ColourDef);

				// we need to hide/show camelots custom colour picker control depending upon what we get here ....

				if (Index != 0)		// != Normal colour
				{
					needColPickHidden = TRUE;
				}
				else
				{
					needColPickHidden = FALSE;
				}

				HideOrShowColourPicker();

				BOOL Changed = FALSE;

				switch (Index)
				{
					case 1:		// Spot
						Changed = MakeColourASpot();

						if (!Changed)
						{
							// The colour hasn't changed - make sure that the list does not show
							// "Spot colour" when it's not allowed
							State.ColType = (IndexedColourType) 99;	// Invalid! Make sure it updates
							SetControls();
						}
						break;

					case 2:		// Tint (or Shade)
						// Determine if this will change the colour - attempts to change
						// a tint to a tint or a shade to a shade must be ignored.

						if (EditingColour->GetType() != COLOURTYPE_TINT)
							Changed = TRUE;			// It was not a tint or a shade, so we must change it
						else
						{
							if (IsShadeColour && !EditingColour->TintIsShade())
								Changed = TRUE;		// We're changing a Tint into a Shade

							if (!IsShadeColour && EditingColour->TintIsShade())
								Changed = TRUE;		// We're chnaging a Shade into a Tint
						}

						if (Changed)		// If the type needs to be changed, do it
						{
							// Not already a tint/shade, so make it so
							EditingColour->SetLinkedParent(LastParent, COLOURTYPE_TINT);

							if (IsShadeColour)
							{
								// Not already the right kind (tint/shade) of tint, so make it so
								EditingColour->SetTintOrShade(IsShadeColour);
								Changed = TRUE;

								FIXED24 NewX(0.0);
								FIXED24 NewY(0.0);

								double ParentVal = ParentDef.Saturation.MakeDouble();
								double ColourVal = ColourDef.Saturation.MakeDouble();

								if (ParentVal > ColourVal)
								{
									if (ParentVal > 0.0)
										NewX = -((ParentVal - ColourVal) / ParentVal);
								}
								else
								{
									if (ParentVal < 1.0)
										NewX = (ColourVal - ParentVal) / (1.0 - ParentVal);
								}

								// Work out Value scaling factor
								ParentVal = ParentDef.Value.MakeDouble();
								ColourVal = ColourDef.Value.MakeDouble();
								if (ParentVal > ColourVal)
								{
									if (ParentVal > 0.0)
										NewY = -((ParentVal - ColourVal) / ParentVal);
								}
								else
								{
									if (ParentVal < 1.0)
										NewY = (ColourVal - ParentVal) / (1.0 - ParentVal);
								}

								// And make the colour a shade (a special type of tint)
								EditingColour->SetShadeValues(NewX, NewY);
							}
							else
							{
								double TintValue = 1.0;

								if (ParentDef.Saturation.MakeDouble() != 0.0)
									TintValue = ColourDef.Saturation.MakeDouble() / ParentDef.Saturation.MakeDouble();

								EditingColour->SetTintValue(FIXED24(TintValue));
							}
						}
						break;

					case 3:		// Linked
						if (EditingColour->GetType() != COLOURTYPE_LINKED)
						{
							EditingColour->SetLinkedParent(LastParent, COLOURTYPE_LINKED);
							EditingColour->SetInheritsComponent(1, TRUE);
							EditingColour->SetInheritsComponent(2, TRUE);
							EditingColour->SetInheritsComponent(3, TRUE);
							EditingColour->SetInheritsComponent(4, TRUE);
							Changed = TRUE;
						}
						break;

					default:	// Normal
						if (EditingColour->GetType() != COLOURTYPE_NORMAL)
						{
							EditingColour->SetLinkedParent(LastParent, COLOURTYPE_NORMAL);
							Changed = TRUE;
						}
						break;
				}

				if (Changed)
					EditingColourHasChanged();	// Inform world and reset/redraw our window
			}
			else if (Msg->GadgetID == _R(IDC_EDIT_PARENTCOL))
			{
				WORD Index;
				GetValueIndex(_R(IDC_EDIT_PARENTCOL), &Index);

				// If the old parent is now 'not in use' then we'd better broadcast a message
				// to ensure that the colour gallery redraws if necessary. Remember the old
				// parent for later on...
				IndexedColour *OldParent = EditingColour->FindLinkedParent();

				// Find the new parent colour
				IndexedColour *NewParent = DecodeParentColourListSelection((INT32)Index);

				// And broadcast for the new parent, if it is now about to become in-use!
				if (NewParent != NULL && !NewParent->HasLinkedChildren())
					ColourManager::ColourHasChangedInvisible((Document *)ParentList->GetParentDocument(), ParentList, NewParent);

				// Finally, set the new parent of this colour, and inform the world and fix up our window
				EditingColour->SetLinkedParent(NewParent, EditingColour->GetType());
				EditingColourHasChanged();

				// Now do the broadcast for the old parent (we waited until after SetLinkedParent so we
				// only bother with the broadcast if this has removed the *last* child of that parent)
				if (OldParent != NULL && !OldParent->HasLinkedChildren())
					ColourManager::ColourHasChangedInvisible((Document *)ParentList->GetParentDocument(), ParentList, OldParent);
			}

			// And then lob away the input focus again - put it back into the mainframe
			LoseKeyboardFocus();

			break;

		default:
			break;
	}

	return(DialogOp::Message(Message));
}



/********************************************************************************************

>	BOOL ColourEditDlg::MakeNewNColour(void)

	Author:		Martin_Bell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/7/97

	Returns:	TRUE if EditingColour has changed

	Purpose:	Moves a lump of code out of the switch statement in the message handler, so
				it can be executed by the button in Camelot and the menu item in Webster

				Makes a new named colour or shade.

	See also:	ColourEditDlg::MakeColourASpot for stuff about reentrancy

********************************************************************************************/

BOOL ColourEditDlg::MakeNewNColour(void)
{
	// We might be about to show an InformError dialogue. This causes the dropdown list to
	// close, and that in turn causes a re-entrant call to this message handler,
	// the net result being that we create the new colour twice, once before
	// the user has even decided which button to press!!! AAAAARARRRRGGH!
	// Nasty sounding buglet there. But let's fix the symptom...
	static BOOL GoneReentrant = FALSE;

	if (GoneReentrant)				// Eek! Reentrancy! Oh, no you don't, matey!
		return(FALSE);

	// Create a colour (style or local) from the current EditingColour
	if (EditingColour != NULL && ParentList != NULL)// && !EditingColour->IsNamed())
	{
		// Make sure the new colour has a suitable parent colour
		if (EditingColour->FindLastLinkedParent() == NULL ||
			(EditingColour->GetType() != COLOURTYPE_TINT && EditingColour->GetType() != COLOURTYPE_LINKED))
		{
			// Make sure we don't try to make an unnamed/deleted colour our parent!
			IndexedColour *NewParent = ResultColour;

			if (NewParent != NULL && !NewParent->IsNamed())
				NewParent = NewParent->FindLastLinkedParent();

			// Ensure the parent is legal
			if (NewParent != NULL && (!NewParent->IsNamed() || NewParent->IsDeleted()))
				NewParent = NULL;

			// And set a legal parent, or NULL if we didn't find one
			EditingColour->SetLinkedParent(NewParent, EditingColour->GetType());
		}

		// EditingColour will now be put into use as an undo record, so we must get
		// a new one for our own use... We do this first in case re-entrant calls (to
		// Message !) occur which might make us delete EditingColour or something. We
		// must handle the other reentrancy problem of closing the menu if an error
		// dialog pops up as well by setting GoneReentrant.
		IndexedColour *OldResultColour = ResultColour;
		IndexedColour *NewColour = new IndexedColour(*EditingColour);
		if (NewColour == NULL)
		{
			GoneReentrant = TRUE;
			InformError();
			GoneReentrant = FALSE;
			return(FALSE);
		}

		// Ask the user for new-colour settings, and allow them to cancel the
		// make-style if they like
		if (!NewColourDlg::InvokeDialog(ParentList, NewColour))
		{
			// They've cancelled the operation. Go back to editing what they were
			// editing before.
			EditThisColour(ParentList, OldResultColour);
			return(FALSE);
		}

		if (NewColour->IsNamed())
		{
			// Add it to the colour list, with undo
			ParentList->AddItem(NewColour);

			IndexedColour *NewCols[2];
			NewCols[0] = NewColour;
			NewCols[1] = NULL;

			ColourManager::UnHideColours(ParentList, NewCols);
		}
		else
		{
			// This should now never happen. However, if it does, we might
			// as well add it to the unnamed colours so we don't memory leak it.
			ERROR3("'New named colour' dialogue returned an UNNAMED colour");

			// Just add it to the colour list
			ParentList->GetUnnamedColours()->AddTail(NewColour);
			FirstEdit = FALSE;
		}

		// Force the new colour into the currently displayed colour model (wysiwyg)
//		ColourContext *cc = ColourContext::GetGlobalDefault(DisplayModel);
		ColourContext *cc = NULL;
		BOOL bDeleteCC = GetColourContext(DisplayModel, &cc);	
		ERROR3IF(cc == NULL, "Necessary document default colour context not defined?!");
		ColourPicker::ForceColourModel(NewColour, cc);

		// Delete the colour context if necessary
		if (bDeleteCC)
			ColourContextList::GetList()->RemoveContext(&cc);			// Have finished with it

		// Apply it to the selection
		ApplyColourNow(NewColour);

		DisplayModel = NewColour->GetColourModel();
		EditThisColour(ParentList, NewColour);
	}

	GoneReentrant = FALSE;

	return TRUE;
}


/********************************************************************************************

>	BOOL ColourEditDlg::MakeColourASpot(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/9/96

	Returns:	TRUE if EditingColour has changed

	Purpose:	Moves a lump of code out of the switch statement in the message handler

				Simply changes this colour's colour type to be COLOURTYPE_SPOT.
				However, there is a complication in that local colours can't be spots,
				so we prompt the user to see if they really want a spot, in which case 
				we have to convert their colour into a named colour before setting
				the type to spot. They have the chance to cancel and act as if nothing
				happened.

********************************************************************************************/

BOOL ColourEditDlg::MakeColourASpot(void)
{
	// We might be about to show an InformError dialogue. This causes the dropdown list to
	// close, and that in turn causes a re-entrant call to this message handler,
	// the net result being that we create the new colour twice, once before
	// the user has even decided which button to press!!! AAAAARARRRRGGH!
	// Nasty sounding buglet there. But let's fix the symptom...
	static BOOL GoneReentrant = FALSE;

	if (GoneReentrant)				// Eek! Reentrancy! Oh, no you don't, matey!
		return(FALSE);

	if (IndexedColour::AreSpotsForcedToProcess())
	{
		// Hey, you can't create spots now - they're all process colours!
		GoneReentrant = TRUE;
		InformError(_R(IDE_SPOTSAREPROCESS), _R(IDS_OK));
		GoneReentrant = FALSE;
		return(FALSE);
	}

	if (EditingColour == NULL || ParentList == NULL || ResultColour == NULL)
		return(FALSE);

	BOOL Changed = FALSE;

	if (!ResultColour->IsNamed())	// Unnamed colour - must convert to a named colour
	{
		GoneReentrant = TRUE;
		LoseKeyboardFocus();

		// This is an unnamed (local) colour, so it can't be a spot colour.
		// Ask the user if they want to cancel or convert the colour to be a named spot
		if (InformError(_R(IDE_LOCALCANTBESPOT), _R(IDS_MAKENAMEDSPOT), _R(IDS_CANCEL)) == 1)
		{
			// Rightoh, we should be safe from the reentrancy demon now
			GoneReentrant = FALSE;

			// Create a colour (style or local) from the current EditingColour
			// Make sure the new colour has a suitable parent hint colour
			// Make sure we don't try to make an unnamed/deleted colour our parent!
			IndexedColour *NewParent = ResultColour;
			if (NewParent != NULL && !NewParent->IsNamed())
				NewParent = NewParent->FindLastLinkedParent();

			// Ensure the parent is legal
			if (NewParent != NULL && (!NewParent->IsNamed() || NewParent->IsDeleted()))
				NewParent = NULL;

			// EditingColour will now be put into use as an undo record, so we must get
			// a new one for our own use... We do this first in case re-entrant calls
			// occur which might make us delete EditingColour or something.
			IndexedColour *OldResultColour = ResultColour;
			IndexedColour *NewColour = new IndexedColour(*EditingColour);
			if (NewColour == NULL)
			{
				InformError();
				return(FALSE);
			}

			// And set a legal parent, or NULL if we didn't find one
			NewColour->SetLinkedParent(NewParent, COLOURTYPE_SPOT);

			// Ask the user for new-colour settings, and allow them to cancel the
			// make-style if they like
			if (!NewColourDlg::InvokeDialog(ParentList, NewColour))
			{
				// They've cancelled the operation. Go back to editing what they were
				// editing before.
				EditThisColour(ParentList, OldResultColour);
				return(FALSE);
			}

			if (NewColour->IsNamed())
			{
				// Add it to the colour list, with undo
				ParentList->AddItem(NewColour);

				IndexedColour *NewCols[2];
				NewCols[0] = NewColour;
				NewCols[1] = NULL;

				ColourManager::UnHideColours(ParentList, NewCols);
			}
			else
			{
				// This should now never happen. However, if it does, we might
				// as well add it to the unnamed colours so we don't memory leak it.
				ERROR3("'New named colour' dialogue returned an UNNAMED colour");

				// Just add it to the colour list
				ParentList->GetUnnamedColours()->AddTail(NewColour);
				FirstEdit = FALSE;
			}

			// Apply it to the selection
			ApplyColourNow(NewColour);
			EditThisColour(ParentList, NewColour);

			// And make sure we know the colour changed in some way
			Changed = TRUE;
		}
		// else
		//     user cancelled, so just drop through to return FALSE
	}
	else
	{
		// It's a named colour, so we can just make it a spot with no fuss
		IndexedColour *LastParent = EditingColour->FindLastLinkedParent();

		// Check if any safe parent colours are available
		LastParent = FindSafeParent(LastParent, FALSE);

		if (EditingColour->GetType() != COLOURTYPE_SPOT)
		{
			EditingColour->SetLinkedParent(LastParent, COLOURTYPE_SPOT);
			Changed = TRUE;
		}
	}

	GoneReentrant = FALSE;

	return(Changed);
}



/********************************************************************************************

>	void ColourEditDlg::RenderCross(RenderRegion *pRender, DocRect *CrossRect,
									INT32 PixelSize, BOOL Shaded = FALSE)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/11/94

	Inputs:		pRender - The RenderRegion to render the marker cross into
				CrossRect - The rectangle (in millipoints, within the RenderRegion) in which
				to draw the cross. This should be a square, CROSSRADIUS*2 millipoints wide/high.
				PixelSize - The size of a device pixel, in millipoints
				Shaded - TRUE if the cross should be drawn 'shaded' (not draggable)

	Purpose:	Renders a colour picker marker cross (a 'plus' shape)

********************************************************************************************/

void ColourEditDlg::RenderCross(RenderRegion *pRender, DocRect *CrossRect,
								INT32 PixelSize, BOOL Shaded)
{
	pRender->SaveContext();		// Save the current rendering context

	DocColour Trans(COLOUR_TRANS);
	DocColour Black(COLOUR_BLACK);
	DocColour White(COLOUR_WHITE);

	if (Shaded)		// Cross becomes more faint if it can't be dragged
	{
//		Black = DocColour(80L, 80L, 80L);
//		White = DocColour(176L, 176L, 176L);

		// Now it just gets a constraint circle around it
		DocCoord Center((CrossRect->lo.x + CrossRect->hi.x) / 2, (CrossRect->lo.y + CrossRect->hi.y) / 2);
		RenderConstraint(pRender, Center, Center);
	}

	pRender->SetLineWidth(0);
	pRender->SetLineColour(White);

	// Render a crosshair shape
	pRender->SetFillColour(Black);

	DocRect CrossLine;

	INT32 LineWidth  = CrossRect->Width() / 2;
	INT32 LineHeight = CrossRect->Height() / 2;

	CrossLine.lo.y  = CrossRect->lo.y + LineHeight - PixelSize;
	CrossLine.hi.y  = CrossRect->lo.y + LineHeight + PixelSize;

	CrossLine.lo.x = CrossRect->lo.x;
	CrossLine.hi.x = CrossRect->lo.x + LineWidth/2;
	pRender->DrawRect(&CrossLine);

	CrossLine.lo.x = CrossRect->hi.x - LineWidth/2;
	CrossLine.hi.x = CrossRect->hi.x;
	pRender->DrawRect(&CrossLine);

	CrossLine.lo.x = CrossRect->lo.x + LineWidth - PixelSize;
	CrossLine.hi.x = CrossRect->lo.x + LineWidth + PixelSize;

	CrossLine.lo.y = CrossRect->lo.y;
	CrossLine.hi.y = CrossRect->lo.y + LineHeight/2;
	pRender->DrawRect(&CrossLine);

	CrossLine.lo.y = CrossRect->hi.y - LineHeight/2;
	CrossLine.hi.y = CrossRect->hi.y;
	pRender->DrawRect(&CrossLine);

	pRender->RestoreContext();		// Restore the previous rendering context
}



/********************************************************************************************

>	void ColourEditDlg::RenderParentCross(RenderRegion *pRender, DocRect *CrossRect,
									INT32 PixelSize)
	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/10/95

	Inputs:		pRender - The RenderRegion to render the marker cross into
				CrossRect - The rectangle (in millipoints, within the RenderRegion) in which
				to draw the cross. This should be a square, CROSSRADIUS*2 millipoints wide/high.
				PixelSize - The size of a device pixel, in millipoints

	Purpose:	Renders a colour picker marker cross (an 'x' shape) to indicate the position
				of the PARENT colour on the colour's picker. Used for shades and maybe for
				linked colours if they ever become relative.

********************************************************************************************/

void ColourEditDlg::RenderParentCross(RenderRegion *pRender, DocRect *CrossRect,
								INT32 PixelSize)
{
	pRender->SaveContext();		// Save the current rendering context

	pRender->SetLineWidth(0);
	pRender->SetLineColour(COLOUR_WHITE);
	pRender->SetFillColour(COLOUR_TRANS);

	pRender->DrawLine(CrossRect->lo, CrossRect->hi);

	pRender->DrawLine(DocCoord(CrossRect->lo.x, CrossRect->hi.y),
						DocCoord(CrossRect->hi.x, CrossRect->lo.y));

	pRender->RestoreContext();		// Restore the previous rendering context
}



/********************************************************************************************

>	void ColourEditDlg::RenderConstraint(RenderRegion *pRender, DocCoord End1, DocCoord End2)
	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/9/95

	Inputs:		pRender - The RenderRegion to render the constraint into
				End1 - The point at one end of the constraint bubble
				End2 - The point at the other end of the constraint bubble

	Purpose:	Renders a colour picker constraint bubble. This is a "sausage" in a
				straight line around two endpoints, used to mark drag constraints
				on screen. 

	Notes:		if (End1 == End2) a circle will be drawn at that point

				This may draw the semicircular caps the wrong way around. It works
				for the cases I've needed so far, so I couldn't be bothered tidying it up.

********************************************************************************************/

void ColourEditDlg::RenderConstraint(RenderRegion *pRender, DocCoord End1, DocCoord End2)
{
	if (End1 == End2)
	{
		const INT32 CircleSize = CROSSRADIUS + 1000;
		const INT32 CPDist = (const INT32) ( ((double)CircleSize) * 0.552 );

		// The constraint bubble is a circle. Special-case the redraw
		pRender->SetLineColour(COLOUR_WHITE);

		Path Bubble;
		Bubble.Initialise(12, 12);
		Bubble.FindStartOfPath();

		Bubble.InsertMoveTo(DocCoord(End1.x+CircleSize, End1.y));
		Bubble.InsertCurveTo(DocCoord(End1.x+CircleSize, End1.y+CPDist),
								DocCoord(End1.x+CPDist, End1.y+CircleSize),
								DocCoord(End1.x, End1.y+CircleSize));
		Bubble.InsertCurveTo(DocCoord(End1.x-CPDist, End1.y+CircleSize),
								DocCoord(End1.x-CircleSize, End1.y+CPDist),
								DocCoord(End1.x-CircleSize, End1.y));
		Bubble.InsertCurveTo(DocCoord(End1.x-CircleSize, End1.y-CPDist),
								DocCoord(End1.x-CPDist, End1.y-CircleSize),
								DocCoord(End1.x, End1.y-CircleSize));
		Bubble.InsertCurveTo(DocCoord(End1.x+CPDist, End1.y-CircleSize),
								DocCoord(End1.x+CircleSize, End1.y-CPDist),
								DocCoord(End1.x+CircleSize, End1.y));

		Bubble.IsFilled = FALSE;
		pRender->DrawPath(&Bubble);		// Render the value square
	}
	else
	{
		const INT32 CircleSize = (CROSSRADIUS + 1000) / 2;
//		const INT32 CPDist = (const INT32) ( ((double)CircleSize) * 0.552 );

		DocCoord Dist = End1 - End2;
		double Angle = 0.0;

		if (Dist.x == 0)
		{
			if (Dist.y < 0)
				Angle = PI;
		}
		else
			Angle = atan2((double)Dist.y, (double)Dist.x) - (PI/2);

		Dist.x = (INT32) ((double)CircleSize * cos(Angle));
		Dist.y = (INT32) ((double)CircleSize * sin(Angle));

		DocCoord ArcStart1(End2 + Dist);
		DocCoord ArcEnd1(End2 - Dist);

		DocCoord ArcStart2(End1 - Dist);
		DocCoord ArcEnd2(End1 + Dist);


		pRender->SetLineColour(COLOUR_WHITE);

		Path Bubble;
		Bubble.Initialise(12, 12);
		Bubble.FindStartOfPath();

		DocCoord RevDist(Dist.y, Dist.x);

		// BODGE to fix vertical case - only horizontal/vertical cases are guaranteed to work
		if (End1.y != End2.y)
			RevDist = DocCoord(-Dist.y, -Dist.x);

		Bubble.InsertMoveTo(ArcStart1);
		Bubble.InsertCurveTo(ArcStart1 + RevDist, ArcEnd1 + RevDist, ArcEnd1);

		Bubble.InsertLineTo(ArcStart2);
		Bubble.InsertCurveTo(ArcStart2 - RevDist, ArcEnd2 - RevDist, ArcEnd2);

		Bubble.InsertLineTo(ArcStart1);	// Close the path

		Bubble.IsFilled = FALSE;
		pRender->DrawPath(&Bubble);		// Render the value square
	}
}



/********************************************************************************************

>	static void CalculateHSVPickerRects(DocRect *VirtualSize, INT32 PixelSize,
										DocRect *HueRect, DocRect *ValSatSquare)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/2/95

	Inputs:		VirtualSize - The rectangle defining the coordinate space to draw into
				
				PixelSize - The size of a pixel as mapped into the MILLIPOINT VirtualSize
				coordinate space; ensures things line up on display pixel boundaries.

	Outputs:	HueRect - The rectangle (a strip along the bottom of VirtualSize) in
				which the Hue slider should be displayed

				ValSatSquare - The Square (above the hue strip) in which the value vs. Satn.
				square should be displayed.

	Purpose:	Calculates where the important regions are in the square HSV picker

********************************************************************************************/

static void CalculateHSVPickerRects(DocRect *VirtualSize, INT32 PixelSize,
										DocRect *HueRect, DocRect *ValSatSquare)
{
	*HueRect = *VirtualSize;
	HueRect->hi.y = HueRect->lo.y + (CROSSRADIUS+PixelSize)*2;
	GridLockRect(HueRect, PixelSize);

	*ValSatSquare = *VirtualSize;
	ValSatSquare->lo.y = HueRect->hi.y + 4000;	// Above the hue slider, with a gap
	ValSatSquare->hi.x -= PATCHSIZE; // center within the area left by the patches

	INT32 SquareSize = ValSatSquare->Height();
	if (SquareSize > ValSatSquare->Width())
	{
		SquareSize = ValSatSquare->Width();
	
		// Center it vertically
		ValSatSquare->lo.y += (ValSatSquare->Height() - SquareSize) / 2;
		ValSatSquare->hi.y = ValSatSquare->lo.y + SquareSize;
	}
	else
	{
		// Center it horizontally
		ValSatSquare->lo.x += (ValSatSquare->Width() - SquareSize) / 2;
		ValSatSquare->hi.x = ValSatSquare->lo.x + SquareSize;
	}

	GridLockRect(ValSatSquare, PixelSize);
}



/********************************************************************************************

>	void ColourEditDlg::RenderPickerHSV(RenderRegion *pRender, DocRect *VirtualSize,
							INT32 PixelSize, DialogColourInfo *RedrawColours,
							DocRect *pClipRect)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/94

	Inputs:		pRender - The render region to render the picker in
				
				VirtualSize - The rectangle defining the coordinate space to draw into
				
				PixelSize - The size of a pixel as mapped into the MILLIPOINT VirtualSize
				coordinate space; ensures things line up on display pixel boundaries.
				
				RedrawColours - A pointer to a DialogColourInfo object describing
				OS-friendly drawing colours to be used in redrawing the control.

				pClipRect - The area to be redrawn, in (0,0)->(dx,dy) dialog millipoint coords

	Purpose:	Redraws a colour picker control for the colour editor dialogue.
				This is used in the given display mode to display the colour information
				in a useful format. e.g. in HSV mode, a wheel and slider arrangement.

	SeeAlso:	ColourEditDlg::RenderControl; ColourEditDlg::RenderPickerDefault

********************************************************************************************/

void ColourEditDlg::RenderPickerHSV(RenderRegion *pRender, DocRect *VirtualSize,
									INT32 PixelSize, DialogColourInfo *RedrawColours,
									DocRect *pClipRect)
{
	pRender->SaveContext();		// Save the current rendering context

	// Set defaults: Fill paths only, and fill background with window grey
	DocColour Trans(COLOUR_TRANS);

	pRender->SetLineWidth(0);
	pRender->SetLineColour(Trans);
//	pRender->SetFillColour(RedrawColours->DialogBack());
//	pRender->DrawRect(VirtualSize);

	// First, do a Hue slider (a rainbow grad filled rectangle along the bottom)
	DocRect HueRect;
	DocRect ValSatSquare;
	CalculateHSVPickerRects(VirtualSize, PixelSize, &HueRect, &ValSatSquare);

	// Draw the Saturation vs. Value square
	Path SquarePath;
	LinearFillAttribute ValueGradFill;
	LinearTranspFillAttribute TransFill;

	ColourHSVT ColourDef;
	if (EditingColour != NULL)
	{
		BaseDocument *ScopeDocument = ParentList->GetParentDocument();
		ColourContextArray *Contexts = ScopeDocument->GetDefaultColourContexts();
		ERROR3IF(Contexts == NULL, "Document default contexts not defined?!");

		ColourContextHSVT *ccHSV = (ColourContextHSVT *)
										Contexts->Context[COLOURMODEL_HSVT];
		ERROR3IF(ccHSV == NULL, "Document default context for HSV not defined?!");
		ccHSV->ConvertColour(EditingColour, (ColourGeneric *) &ColourDef);
	}
	else
	{
		// No colour - default to white
		ColourDef.Hue = 0;
		ColourDef.Saturation = ColourDef.Value = FIXED24(1.0);
	}

	if (pClipRect->IsIntersectedWith(ValSatSquare))		// If we need to redraw this bit...
	{		
		// Copy the colour def, and force it's Value & Sat components to 1.0
		ColourHSVT TempColourDef;
		TempColourDef.Hue	= ColourDef.Hue;
		TempColourDef.Value	= TempColourDef.Saturation = FIXED24(1.0);

		pRender->SaveContext();
		
		ValueGradFill.Colour	= DOCCOLOUR_HSVT(&TempColourDef);
		ValueGradFill.EndColour	= DocColour(255L, 255L, 255L);

		DocCoord ThePoint;
		ThePoint = DocCoord(ValSatSquare.lo.x, ValSatSquare.hi.y);
		ValueGradFill.SetStartPoint(&ThePoint);
		ThePoint = DocCoord(ValSatSquare.hi.x, ValSatSquare.hi.y);
		ValueGradFill.SetEndPoint(&ThePoint);
		ValueGradFill.SetEndPoint2(NULL);

		pRender->SetFillGeometry(&ValueGradFill, FALSE);			// Set Grad-filled

		SquarePath.Initialise(12, 12);
		SquarePath.FindStartOfPath();

		SquarePath.InsertMoveTo(ValSatSquare.lo);
		SquarePath.InsertLineTo(DocCoord(ValSatSquare.hi.x, ValSatSquare.lo.y));
		SquarePath.InsertLineTo(ValSatSquare.hi);
		SquarePath.InsertLineTo(DocCoord(ValSatSquare.lo.x, ValSatSquare.hi.y));
		SquarePath.IsFilled = TRUE;

		pRender->DrawPath(&SquarePath);		// Render the value square

		// Render a white linear-grad-transparency square over the top to get the
		// effect of the Saturation gradient
		DocColour black(0,0,0);
		pRender->SetFillColour(black);

		// Set transparency to circular 0% at center, 100% at radius, && plot it
		if (bHSVHueAtTop)
		{
			TransFill.SetStartPoint(&ValSatSquare.hi);
			ThePoint = DocCoord(ValSatSquare.hi.x, ValSatSquare.lo.y);
			TransFill.SetEndPoint(&ThePoint);
			TransFill.SetEndPoint2(NULL);
		}
		else
		{
			TransFill.SetStartPoint(&ValSatSquare.lo);
			ThePoint = DocCoord(ValSatSquare.lo.x, ValSatSquare.hi.y);
			TransFill.SetEndPoint(&ThePoint);
			TransFill.SetEndPoint2(NULL);
		}
		TransFill.Transp		= 255;
		TransFill.EndTransp		= 0;
		TransFill.TranspType	= TT_StainGlass;		// 'Stained glass' transparency

		pRender->SetTranspFillGeometry(&TransFill, FALSE);

		pRender->DrawPath(&SquarePath);		// Render the saturation square

		pRender->RestoreContext();
	}

	// (Declare variables now to stop render region going tits up)
	Path HuePath;
	FillEffectAltRainbowAttribute AltRainbow;
	LinearFillAttribute HueGradFill;

	if (pClipRect->IsIntersectedWith(HueRect))		// If we need to redraw this bit...
	{
		pRender->SaveContext();

		HueGradFill.Colour		= DocColour(255L, 0L, 0L);		// Red to Red, alt-rainbow
		HueGradFill.EndColour	= DocColour(255L, 0L, 0L);

		HueGradFill.SetStartPoint(&HueRect.hi);
		DocCoord ThePoint(HueRect.lo.x, HueRect.hi.y);
		HueGradFill.SetEndPoint(&ThePoint);
		HueGradFill.SetEndPoint2(NULL);

		pRender->SetFillGeometry(&HueGradFill, FALSE);			// Set Grad-filled

		pRender->SetFillEffect(&AltRainbow, FALSE);				// Set alt-rainbow fill

		// Draw the rectangle - done as a path so we can draw with grad-fill
		HuePath.Initialise(12, 12);
		HuePath.FindStartOfPath();

		HuePath.InsertMoveTo(HueRect.lo);
		HuePath.InsertLineTo(DocCoord(HueRect.hi.x, HueRect.lo.y));
		HuePath.InsertLineTo(HueRect.hi);
		HuePath.InsertLineTo(DocCoord(HueRect.lo.x, HueRect.hi.y));
		HuePath.IsFilled = TRUE;

		pRender->DrawPath(&HuePath);		// Render the hue strip

		pRender->RestoreContext();
	}

	if (EditingColour != NULL)
	{
		// Render marker crosses in the appropriate places for the definition
		// of the current editing colour - First, the Hue cross
		DocRect CrossRect(-CROSSRADIUS, -CROSSRADIUS, CROSSRADIUS, CROSSRADIUS);

		INT32 TransX = (INT32) ((double) HueRect.Width() * ColourDef.Hue.MakeDouble());

		CrossRect.Translate(HalfGridLock(HueRect.lo.x + TransX, PixelSize),
							HalfGridLock((HueRect.lo.y + HueRect.hi.y) / 2, PixelSize));
	
		BOOL CrossShaded = (EditingColour->GetType() == COLOURTYPE_TINT) ||
						(EditingColour->GetType() == COLOURTYPE_LINKED &&
						 EditingColour->InheritsComponent(1));

		RenderCross(pRender, &CrossRect, PixelSize, CrossShaded);

		// And the Sat/Val cross
		CrossRect = DocRect(-CROSSRADIUS, -CROSSRADIUS, CROSSRADIUS, CROSSRADIUS);
		TransX = ValSatSquare.hi.x - (INT32)
					((double)ValSatSquare.Width() * ColourDef.Saturation.MakeDouble());
		INT32 TransY;
if (bHSVHueAtTop)
{
		TransY = ValSatSquare.lo.y + (INT32)
					((double)ValSatSquare.Height() * ColourDef.Value.MakeDouble());
}
else
{
		TransY = ValSatSquare.hi.y - (INT32)
					((double)ValSatSquare.Height() * ColourDef.Value.MakeDouble());
}

		CrossRect.Translate(HalfGridLock(TransX, PixelSize), HalfGridLock(TransY, PixelSize));

		CrossShaded = (EditingColour->GetType() == COLOURTYPE_TINT) ||
							(EditingColour->GetType() == COLOURTYPE_LINKED &&
							 EditingColour->InheritsComponent(2) &&
							 EditingColour->InheritsComponent(3));
		RenderCross(pRender, &CrossRect, PixelSize, CrossShaded);

		if (!CrossShaded && EditingColour->GetType() == COLOURTYPE_LINKED)
		{
			if (EditingColour->InheritsComponent(2))
			{
				// Saturation is locked, so constrain to Value axis
				RenderConstraint(pRender, DocCoord(TransX, ValSatSquare.hi.y), 
											DocCoord(TransX, ValSatSquare.lo.y));
			}
			else if (EditingColour->InheritsComponent(3))
			{
				// Value is locked, so constrain to Saturation axis
				RenderConstraint(pRender, DocCoord(ValSatSquare.lo.x, TransY), 
											DocCoord(ValSatSquare.hi.x, TransY));
			}
		}
	}
	return;

#if FALSE
	// Replacement code (developmental) to do a 3-d HSV colour cone display

	// Diameter is the width of the gadget - the value rectangle - a gap
	const INT32 MaxDiameter = (VirtualSize->Height() * 100) / 128;
	const INT32 MaxRadius = HalfGridLock(MaxDiameter/2, PixelSize);
	const INT32 TopMidY = HalfGridLock(VirtualSize->hi.y - MaxRadius, PixelSize);
	const INT32 TopMidX = HalfGridLock(VirtualSize->hi.x - MaxRadius, PixelSize);

	// The tip of the cone is 45 degrees down & to the left of the center.
	// It occurs just far enough away from the edge of the gadget to accomodate a Z Slider box
	const INT32 TipY = VirtualSize->lo.y + ZSLIDERSIZE;
	const INT32 TipX = TopMidX - (TopMidY - TipY);

	// Get the colour we want to display as an HSV colour definition
	ColourHSVT ColourDef;					
	ColourHSVT TempColourDef;
	if (EditingColour != NULL)
	{
		BaseDocument *ScopeDocument = ParentList->GetParentDocument();
		ColourContextArray *Contexts = ScopeDocument->GetDefaultColourContexts();
		ERROR3IF(Contexts == NULL, "Document default contexts not defined?!");

		ColourContextHSVT *ccHSV = (ColourContextHSVT *)
										Contexts->Context[COLOURMODEL_HSVT];
		ERROR3IF(ccHSV == NULL, "Document default context for HSV not defined?!");
		ccHSV->ConvertColour(EditingColour, (ColourGeneric *) &ColourDef);

		// --- Draw the value slider rectangle
	
		// Create a colour from this, but with Value = 1.0
		memcpy(&TempColourDef, &ColourDef, sizeof(ColourHSVT));
	}
	else
		TempColourDef.Value = FIXED24(1.0);

	const INT32 Diameter = (const INT32) ((double) MaxDiameter * TempColourDef.Value.MakeDouble());
	const INT32 Radius = HalfGridLock(Diameter/2, PixelSize);

	INT32 Offset = (const INT32) ( ((double) (TopMidY - TipY)) * 
								 (1.0 - TempColourDef.Value.MakeDouble()) );

	const INT32 MidY = HalfGridLock(TopMidY - Offset, PixelSize);
	const INT32 MidX = HalfGridLock(TopMidX - Offset, PixelSize);

	// --- Draw the HSV colour wheel - two semicircles with conical rainbow
	//     fills (this will be overlaid with a transparent-filled white
	//     circle to get the final colour wheel image)
	//	   The entire HSV picker is now drawn on top of a '3d cone shadow' display

	// Draw the cone 'shadow' (A triangle with a circle over one end)
	Path ShadowT;
	ShadowT.Initialise(12, 12);
	ShadowT.InsertMoveTo(DocCoord(TipX, TipY));

	Offset = (INT32) (0.866025403 * (double)MaxRadius);			// MaxRadius * sin(60)
	INT32 Offset2 =  (INT32) (0.50 * (double)MaxRadius);			// MaxRadius * cos(60)

	ShadowT.InsertLineTo(DocCoord(TopMidX-Offset, TopMidY+Offset2));
	ShadowT.InsertLineTo(DocCoord(TopMidX+Offset2, TopMidY-Offset));
	ShadowT.InsertLineTo(DocCoord(TipX, TipY));
	ShadowT.IsFilled = TRUE;

	DocColour ShadowTFill(176L, 176L, 176L);
	pRender->SetFillColour(ShadowTFill);
	pRender->DrawPath(&ShadowT);

	// CPDist = the distance of the bezier control points from the line
	// endpoints to give us a circle
	const INT32 TopCPDist = (const INT32) ( ((double)MaxRadius) * 0.552 );

	Path ShadowC;
	ShadowC.Initialise(12, 12);
	ShadowC.FindStartOfPath();
	ShadowC.InsertMoveTo(DocCoord(TopMidX+MaxRadius, TopMidY));
	ShadowC.InsertCurveTo(DocCoord(TopMidX+MaxRadius, TopMidY+TopCPDist),
							DocCoord(TopMidX+TopCPDist, TopMidY+MaxRadius),
							DocCoord(TopMidX, TopMidY+MaxRadius));
	ShadowC.InsertCurveTo(DocCoord(TopMidX-TopCPDist, TopMidY+MaxRadius),
							DocCoord(TopMidX-MaxRadius, TopMidY+TopCPDist),
							DocCoord(TopMidX-MaxRadius, TopMidY));
	ShadowC.InsertCurveTo(DocCoord(TopMidX-MaxRadius, TopMidY-TopCPDist),
							DocCoord(TopMidX-TopCPDist, TopMidY-MaxRadius),
							DocCoord(TopMidX, TopMidY-MaxRadius));
	ShadowC.InsertCurveTo(DocCoord(TopMidX+TopCPDist, TopMidY-MaxRadius),
							DocCoord(TopMidX+MaxRadius, TopMidY-TopCPDist),
							DocCoord(TopMidX+MaxRadius, TopMidY));
	ShadowC.IsFilled = TRUE;

	DocColour ShadowCFill(168L, 168L, 168L);
	pRender->SetFillColour(ShadowCFill);
	pRender->DrawPath(&ShadowC);
	

	// CPDist = the distance of the bezier control points from the line
	// endpoints to give us a circle
	const INT32 CPDist = (const INT32) ( ((double)Radius) * 0.552 );

	// Generate the fill attribute for the HSV rainbow fill
	ConicalFillAttribute Conical;

	Conical.SetStartPoint(DocCoord(MidX, MidY));
	Conical.SetEndPoint(DocCoord(MidX-Radius, MidY));
	Conical.SetEndPoint2(NULL);

	// Fill: conical, (alt)rainbow, Hue 0.25 to 0.75, Value == SourceColour Value
	TempColourDef.Hue = FIXED24(0.25);
	TempColourDef.Saturation = FIXED24(1.0);
	Conical.Colour = DOCCOLOUR_HSVT(&TempColourDef);

	TempColourDef.Hue = FIXED24(0.75);
	Conical.EndColour = DOCCOLOUR_HSVT(&TempColourDef);

	pRender->SetFillGeometry(&Conical, FALSE);

	// Build and draw the Top (CircleT) and Bottom (CircleB) Semicircles
	Path CircleT;
	CircleT.Initialise(12, 12);
	CircleT.FindStartOfPath();
	CircleT.InsertMoveTo(DocCoord(MidX+Radius, MidY));
	CircleT.InsertCurveTo(DocCoord(MidX+Radius, MidY+CPDist), DocCoord(MidX+CPDist, MidY+Radius), DocCoord(MidX, MidY+Radius));
	CircleT.InsertCurveTo(DocCoord(MidX-CPDist, MidY+Radius), DocCoord(MidX-Radius, MidY+CPDist), DocCoord(MidX-Radius, MidY));
	CircleT.InsertLineTo(DocCoord(MidX+Radius, MidY));
	CircleT.IsFilled = TRUE;
	
	FillEffectAltRainbowAttribute AltRainbow;
	pRender->SetFillEffect(&AltRainbow, FALSE);	// Set alt-rainbow fill
	pRender->DrawPath(&CircleT);				// And draw topmost semicircle

	Path CircleB;
	CircleB.Initialise(12, 12);
	CircleB.FindStartOfPath();
	CircleB.InsertMoveTo(DocCoord(MidX+Radius, MidY));
	CircleB.InsertCurveTo(DocCoord(MidX+Radius, MidY-CPDist), DocCoord(MidX+CPDist, MidY-Radius), DocCoord(MidX, MidY-Radius));
	CircleB.InsertCurveTo(DocCoord(MidX-CPDist, MidY-Radius), DocCoord(MidX-Radius, MidY-CPDist), DocCoord(MidX-Radius, MidY));
	CircleB.InsertLineTo(DocCoord(MidX+Radius, MidY));
	CircleB.IsFilled = TRUE;

	FillEffectRainbowAttribute Rainbow;
	pRender->SetFillEffect(&Rainbow, FALSE);	// Set rainbow fill
	pRender->DrawPath(&CircleB);				// And draw bottom semicircle

	pRender->RestoreContext();					// Get attrs back to normality!
	pRender->SaveContext();
	
	// --- Finally, draw a (rectangle) over the colour wheel, filled with
	//     a grey of the same intensity/Value as the source colour (e.g
	//     if source Value == 1.0, then fill with white). This is overlaid
	//     using a circular transparent fill which is 0% transparent in the
	//     center (white) and 100% at the edge of the wheel (wheel-colour)
	Path TransPath;
	TransPath.Initialise(12, 12);
	TransPath.FindStartOfPath();
	TransPath.InsertMoveTo(DocCoord(MidX-Radius, MidY-Radius));
	TransPath.InsertLineTo(DocCoord(MidX-Radius, MidY+Radius));
	TransPath.InsertLineTo(DocCoord(MidX+Radius, MidY+Radius));
	TransPath.InsertLineTo(DocCoord(MidX+Radius, MidY-Radius));
	TransPath.InsertLineTo(DocCoord(MidX-Radius, MidY-Radius));
	TransPath.IsFilled = TRUE;

	// Render the transpath
	TempColourDef.Hue = 0;
	TempColourDef.Saturation = 0;
	TempColourDef.Value = (EditingColour == NULL) ?
									 FIXED24(1.0) : ColourDef.Value;					
	DocColour MaxWhiteness = DOCCOLOUR_HSVT(&TempColourDef);

	pRender->SetLineWidth(0);			// Ensure fill-only
	pRender->SetLineColour(Trans);
	pRender->SetFillColour(MaxWhiteness);

	// Set transparency to circular 0% at center, 100% at radius, && plot it
	RadialTranspFillAttribute TransFill;
	TransFill.SetStartPoint(DocCoord(MidX, MidY));
	TransFill.SetEndPoint(DocCoord(MidX, MidY+Radius));
	TransFill.SetEndPoint2(DocCoord(MidX+Radius, MidY));
	TransFill.Transp		= 0;
	TransFill.EndTransp		= 255;
	TransFill.TranspType	= TT_Mix;		// 'Mix' transparency

	pRender->SetTranspFillGeometry(&TransFill, FALSE);
	pRender->DrawPath(&TransPath);

	

	// Finally, render an outlined circle over the top, to give the 'jelly cone' appearance
	pRender->SetFillColour(Trans);
	pRender->SetLineColour(ShadowCFill);
	pRender->DrawPath(&ShadowC);


	pRender->RestoreContext();			// Restore original attribute context

	
	if (EditingColour != NULL)
	{
		// And draw a drag icon on the current Z position **** !!!! ToDo: Should be a bitmap icon
		DocRect ZButton(TipX-ZSLIDERSIZE+PixelSize, TipY-ZSLIDERSIZE+PixelSize,
						TipX-PixelSize, TipY-PixelSize);

		// The drag icon should touch the edge of the HSV circle (at 45 degrees)
		Offset = (INT32) (((double) (TopMidY - TipY) - (0.707106781 * (double) MaxRadius)) *
							TempColourDef.Value.MakeDouble());
		
		ZButton.Translate(Offset, Offset);
		GridLockRect(&ZButton, PixelSize);


		DocColour MidGrey(128L, 128L, 128L);
		pRender->SetFillColour(MidGrey);
		pRender->DrawRect(&ZButton);


		// Render marker crosses in the appropriate places for the definition
		// of the current editing colour
		DocRect CrossRect(-CROSSRADIUS, -CROSSRADIUS, CROSSRADIUS, CROSSRADIUS);
	
		// Convert Hue (0.0 -> 1.0) into an angle in radians (0 -> 2Pi)
		const double theta  = 2.0 * PI * ColourDef.Hue.MakeDouble();
	
		// Convert Saturation into a distance from the center of the colour wheel
		const double length = (double)Radius * ColourDef.Saturation.MakeDouble();

		// Translate the cross to the polar coordinate (theta, length)
		CrossRect.Translate(HalfGridLock(MidX + (INT32)(length * sin(theta)), PixelSize),
							HalfGridLock(MidY + (INT32)(length * cos(theta)), PixelSize));
		RenderCross(pRender, &CrossRect, PixelSize);
	}

//#else

	// Calculate where the colour wheel circle & brightness slider rect will go
	// Note that we always try for an even number of pixels wide, to ensure that
	// the marker cross appears exactly centered (rather than 1 pixel more space
	// on one side of it)
	DocRect ValueRect;
	ValueRect.hi.x = HalfGridLock(VirtualSize->hi.x, PixelSize);
	ValueRect.lo.x = HalfGridLock(ValueRect.hi.x - (VirtualSize->Width() / 8), PixelSize * 2);


	// Diameter is the width of the gadget - the value rectangle - a gap
	INT32 Diameter = VirtualSize->Width() - ValueRect.Width() - (ValueRect.Width() / 3);
	if (Diameter > VirtualSize->Height())
		Diameter = VirtualSize->Height();

	const INT32 Radius = HalfGridLock(Diameter/2, PixelSize);
	const INT32 MidY = HalfGridLock(VirtualSize->Height() / 2, PixelSize);
	const INT32 MidX = HalfGridLock(VirtualSize->lo.x + Radius, PixelSize);

	ValueRect.lo.y = MidY - Radius;	// Rect only as high as the circle
	ValueRect.hi.y = MidY + Radius;

	// Get the colour we want to display as an HSV colour definition
	ColourHSVT ColourDef;					
	ColourHSVT TempColourDef;
	if (EditingColour != NULL)
	{
		BaseDocument *ScopeDocument = ParentList->GetParentDocument();
		ColourContextArray *Contexts = ScopeDocument->GetDefaultColourContexts();
		ERROR3IF(Contexts == NULL, "Document default contexts not defined?!");

		ColourContextHSVT *ccHSV = (ColourContextHSVT *)
										Contexts->Context[COLOURMODEL_HSVT];
		ERROR3IF(ccHSV == NULL, "Document default context for HSV not defined?!");
		ccHSV->ConvertColour(EditingColour, (ColourGeneric *) &ColourDef);

		// --- Draw the value slider rectangle
	
		// Create a colour from this, but with Value = 1.0
		memcpy(&TempColourDef, &ColourDef, sizeof(ColourHSVT));
		TempColourDef.Value = FIXED24(1.0);
	}

	// Create a grad fill from this colour (V=1.0) to black (V=0.0)
	// (Declare variables now to stop render region going tits up)
	LinearFillAttribute ValueGradFill;
	Path ValuePath;

	if (pClipRect->IsIntersectedWith(ValueRect))
	{
		if (EditingColour != NULL)
			ValueGradFill.Colour = DOCCOLOUR_HSVT(&TempColourDef);	
		else
			ValueGradFill.Colour = DocColour(255L, 255L, 255L);

		ValueGradFill.EndColour	 = DocColour(0L, 0L, 0L);
		ValueGradFill.SetStartPoint(DocCoord((ValueRect.lo.x + ValueRect.hi.x) / 2,
											ValueRect.hi.y));
		ValueGradFill.SetEndPoint(DocCoord((ValueRect.lo.x + ValueRect.hi.x) / 2,
											ValueRect.lo.y));
		ValueGradFill.SetEndPoint2(NULL);

		pRender->SetFillGeometry(&ValueGradFill, FALSE);

		// Draw the rectangle - done as a path so we can draw with grad-fill
		ValuePath.Initialise(12, 12);
		ValuePath.FindStartOfPath();

		ValuePath.InsertMoveTo(ValueRect.lo);
		ValuePath.InsertLineTo(DocCoord(ValueRect.hi.x, ValueRect.lo.y));
		ValuePath.InsertLineTo(ValueRect.hi);
		ValuePath.InsertLineTo(DocCoord(ValueRect.lo.x, ValueRect.hi.y));
		ValuePath.IsFilled = TRUE;

		pRender->DrawPath(&ValuePath);		// Render the value/brightness strip
	}

	// --- Draw the HSV colour wheel - two semicircles with conical rainbow
	//     fills (this will be overlaid with a transparent-filled white
	//     circle to get the final colour wheel image)

	// CPDist = the distance of the bezier control points from the line
	// endpoints to give us a circle
	const INT32 CPDist = (const INT32) ( ((double)Radius) * 0.552 );

	// Generate the fill attribute for the HSV rainbow fill
	// Again, create all the objects at this scope level to keep render region alive
	ConicalFillAttribute Conical;
	FillEffectAltRainbowAttribute AltRainbow;
	Path CircleT;
	Path CircleB;
	Path TransPath;
	RadialTranspFillAttribute TransFill;

	DocRect WheelRect(MidX - Radius, MidY - Radius, MidX + Radius, MidY + Radius);
	if (pClipRect->IsIntersectedWith(WheelRect))
	{
		Conical.SetStartPoint(DocCoord(MidX, MidY));
		Conical.SetEndPoint(DocCoord(MidX-Radius, MidY));
		Conical.SetEndPoint2(NULL);

		// Fill: conical, (alt)rainbow, Hue 0.25 to 0.75, Value == SourceColour Value
		TempColourDef.Hue = FIXED24(0.25);
		TempColourDef.Saturation = FIXED24(1.0);
		if (EditingColour != NULL)
			TempColourDef.Value = ColourDef.Value;
		else
			TempColourDef.Value = FIXED24(1.0);

		Conical.Colour = DOCCOLOUR_HSVT(&TempColourDef);

		TempColourDef.Hue = FIXED24(0.75);
		Conical.EndColour = DOCCOLOUR_HSVT(&TempColourDef);

		pRender->SetFillGeometry(&Conical, FALSE);

		// Build and draw the Top (CircleT) and Bottom (CircleB) Semicircles
		CircleT.Initialise(12, 12);
		CircleT.FindStartOfPath();
		CircleT.InsertMoveTo(DocCoord(MidX+Radius, MidY));
		CircleT.InsertCurveTo(DocCoord(MidX+Radius, MidY+CPDist), DocCoord(MidX+CPDist, MidY+Radius), DocCoord(MidX, MidY+Radius));
		CircleT.InsertCurveTo(DocCoord(MidX-CPDist, MidY+Radius), DocCoord(MidX-Radius, MidY+CPDist), DocCoord(MidX-Radius, MidY));
		CircleT.InsertLineTo(DocCoord(MidX+Radius, MidY));
		CircleT.IsFilled = TRUE;
	
		pRender->SetFillEffect(&AltRainbow, FALSE);	// Set alt-rainbow fill
		pRender->DrawPath(&CircleT);				// And draw topmost semicircle

		CircleB.Initialise(12, 12);
		CircleB.FindStartOfPath();
		CircleB.InsertMoveTo(DocCoord(MidX+Radius, MidY));
		CircleB.InsertCurveTo(DocCoord(MidX+Radius, MidY-CPDist), DocCoord(MidX+CPDist, MidY-Radius), DocCoord(MidX, MidY-Radius));
		CircleB.InsertCurveTo(DocCoord(MidX-CPDist, MidY-Radius), DocCoord(MidX-Radius, MidY-CPDist), DocCoord(MidX-Radius, MidY));
		CircleB.InsertLineTo(DocCoord(MidX+Radius, MidY));
		CircleB.IsFilled = TRUE;

		FillEffectRainbowAttribute Rainbow;
		pRender->SetFillEffect(&Rainbow, FALSE);	// Set rainbow fill
		pRender->DrawPath(&CircleB);				// And draw bottom semicircle

		pRender->RestoreContext();					// Get attrs back to normality!
		pRender->SaveContext();
	
		// --- Finally, draw a (rectangle) over the colour wheel, filled with
		//     a grey of the same intensity/Value as the source colour (e.g
		//     if source Value == 1.0, then fill with white). This is overlaid
		//     using a circular transparent fill which is 0% transparent in the
		//     center (white) and 100% at the edge of the wheel (wheel-colour)
		TransPath.Initialise(12, 12);
		TransPath.FindStartOfPath();
		TransPath.InsertMoveTo(DocCoord(MidX-Radius, MidY-Radius));
		TransPath.InsertLineTo(DocCoord(MidX-Radius, MidY+Radius));
		TransPath.InsertLineTo(DocCoord(MidX+Radius, MidY+Radius));
		TransPath.InsertLineTo(DocCoord(MidX+Radius, MidY-Radius));
		TransPath.InsertLineTo(DocCoord(MidX-Radius, MidY-Radius));
		TransPath.IsFilled = TRUE;

		// Render the transpath
		TempColourDef.Hue = 0;
		TempColourDef.Saturation = 0;
		TempColourDef.Value = (EditingColour == NULL) ?
										 FIXED24(1.0) : ColourDef.Value;					
		DocColour MaxWhiteness = DOCCOLOUR_HSVT(&TempColourDef);

		pRender->SetLineWidth(0);			// Ensure fill-only
		pRender->SetLineColour(Trans);
		pRender->SetFillColour(MaxWhiteness);

		// Set transparency to circular 0% at center, 100% at radius, && plot it
		TransFill.SetStartPoint(DocCoord(MidX, MidY));
		TransFill.SetEndPoint(DocCoord(MidX, MidY+Radius));
		TransFill.SetEndPoint2(DocCoord(MidX+Radius, MidY));
		TransFill.Transp		= 0;
		TransFill.EndTransp		= 255;
		TransFill.TranspType	= TT_Mix;		// 'Mix' transparency

		pRender->SetTranspFillGeometry(&TransFill, FALSE);
		pRender->DrawPath(&TransPath);
	}

	pRender->RestoreContext();			// Restore original attribute context

	
	if (EditingColour != NULL)
	{
		// Render marker crosses in the appropriate places for the definition
		// of the current editing colour
		DocRect CrossRect(-CROSSRADIUS, -CROSSRADIUS, CROSSRADIUS, CROSSRADIUS);
	
		// Convert Hue (0.0 -> 1.0) into an angle in radians (0 -> 2Pi)
		const double theta  = 2.0 * PI * ColourDef.Hue.MakeDouble();
	
		// Convert Saturation into a distance from the center of the colour wheel
		const double length = (double)Radius * ColourDef.Saturation.MakeDouble();

		// Translate the cross to the polar coordinate (theta, length)
		CrossRect.Translate(HalfGridLock(MidX + (INT32)(length * sin(theta)), PixelSize),
							HalfGridLock(MidY + (INT32)(length * cos(theta)), PixelSize));
		RenderCross(pRender, &CrossRect, PixelSize);

		CrossRect = DocRect(-CROSSRADIUS, -CROSSRADIUS, CROSSRADIUS, CROSSRADIUS);
		INT32 TransX = (ValueRect.lo.x + ValueRect.hi.x) / 2;
		INT32 TransY = ValueRect.lo.y + (INT32) ((double)ValueRect.Height() *
												ColourDef.Value.MakeDouble());
		CrossRect.Translate(HalfGridLock(TransX, PixelSize), HalfGridLock(TransY, PixelSize));
		RenderCross(pRender, &CrossRect, PixelSize);
	}

#endif
}




/********************************************************************************************

>	static void RenderPickerSquare(RenderRegion *pRender, DocRect *SquareRect,
						ColourGeneric *BaseCol, ColourGeneric *MidCol, ColourGeneric *TopCol,
						ColourModel TheColourModel, UINT32 TransType)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/12/94

	Inputs:		pRender - The render region to render the picker in
				SquareRect - The rectangle to fill with the RGB/CMYK cube slice
				BaseCol - The colour to use on the base (Z axis)
				MidCol - The second colour (X axis)
				TopCol - The third colour (Y axis)
				TheColourModel - COLOURMODEL_RGBT or COLOURMODEL_CMYK
				TransType - 3 (stain/additive) for RGB, 2 (stain/filtering) for CMY

	Purpose:	Redraws the front face of the RGB/CMY colour cube pickers.
				This can redraw an axis-aligned planar cut through the RGB/CMY colourspaces,
				if the input parameters are correct.

	Scope:		private (to kernel\coldlog.cpp)

	SeeAlso:	ColourEditDlg::RenderControl; ColourEditDlg::RenderPickerDefault

********************************************************************************************/

static void RenderPickerSquare(RenderRegion *pRender, DocRect *SquareRect,
						ColourGeneric *BaseCol, ColourGeneric *MidCol, ColourGeneric *TopCol,
						ColourModel TheColourModel, UINT32 TransType)
{
	pRender->SaveContext();		// Save the current rendering context

	// First, render the BaseSquare in BaseColour
	DocColour BaseFillCol(TheColourModel, BaseCol);
	pRender->SetFillColour(BaseFillCol);
	pRender->DrawRect(SquareRect);


	// Next, create a path for the rectangle so we can fill it with grad fills
	Path SquarePath;
	SquarePath.Initialise(12, 12);
	SquarePath.FindStartOfPath();

	SquarePath.InsertMoveTo(SquareRect->lo);
	SquarePath.InsertLineTo(DocCoord(SquareRect->hi.x, SquareRect->lo.y));
	SquarePath.InsertLineTo(SquareRect->hi);
	SquarePath.InsertLineTo(DocCoord(SquareRect->lo.x, SquareRect->hi.y));
	SquarePath.IsFilled = TRUE;

	// Create a vertical transparent fill from 100% transparent to 0% transparent
	LinearTranspFillAttribute SquareTransFill1;
	SquareTransFill1.Transp	    = 0;
	SquareTransFill1.EndTransp  = 255;
	SquareTransFill1.TranspType = TransType;
	DocCoord ThePoint(SquareRect->hi.x, (SquareRect->lo.y + SquareRect->hi.y) / 2);
	SquareTransFill1.SetStartPoint(&ThePoint);
	ThePoint = DocCoord(SquareRect->lo.x, (SquareRect->lo.y + SquareRect->hi.y) / 2);
	SquareTransFill1.SetEndPoint(&ThePoint);
	SquareTransFill1.SetEndPoint2(NULL);

	DocColour MidFillCol(TheColourModel, MidCol);		// And flat-fill with the trans fill
	pRender->SetFillColour(MidFillCol);

	pRender->SetTranspFillGeometry(&SquareTransFill1, FALSE);

	pRender->DrawPath(&SquarePath);		// Render the Square


	LinearTranspFillAttribute SquareTransFill2;
	SquareTransFill2.Transp	    = 0;
	SquareTransFill2.EndTransp  = 255;
	SquareTransFill2.TranspType = TransType;

	ThePoint = DocCoord((SquareRect->lo.x + SquareRect->hi.x) / 2, SquareRect->hi.y);
	SquareTransFill2.SetStartPoint(&ThePoint);
	ThePoint = DocCoord((SquareRect->lo.x + SquareRect->hi.x) / 2, SquareRect->lo.y);
	SquareTransFill2.SetEndPoint(&ThePoint);
	SquareTransFill2.SetEndPoint2(NULL);

	DocColour TopFillCol(TheColourModel, TopCol);		// And flat-fill with the trans fill
	pRender->SetFillColour(TopFillCol);

	pRender->SetTranspFillGeometry(&SquareTransFill2, FALSE);
	
	pRender->DrawPath(&SquarePath);		// Render the Square


	pRender->RestoreContext();
}



/********************************************************************************************

>	static void DrawCubeShadowAndCalcValues(RenderRegion *pRender, DocRect *VirtualSize,
											INT32 PixelSize, DialogColourInfo *RedrawColours,
											DocRect *ResultRect, INT32 *ResultSizeZ);

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/12/94

	Inputs:		pRender - The render region to render the picker in, or NULL if you want to
				just calc the values without rendering anything.
				VirtualSize - The rectangle defining the coordinate space to draw into
				PixelSize - The size of a pixel as mapped into the MILLIPOINT VirtualSize
				coordinate space; ensures things line up on display pixel boundaries.
				RedrawColours - A pointer to a DialogColourInfo object describing
				OS-friendly drawing colours to be used in redrawing the control (or NULL if
				pRender is also NULL)

	Outputs:	ResultRect - On return will contain the cube face rectangle
				ResultSizeZ - On return will contain the projected size of the Z axis

	Purpose:	Clears the background of the picker control, and draws the 'shadow' of the
				colour cube into the control, in preparation for RGB/CMYK colour slices
				and stuff to be drawn on top. For convenience, returns the screen area
				square of the cube face and the size of the Z axis as projected onscreen.

	Scope:		private (static in coldlog.cpp)

	SeeAlso:	ColourEditDlg::RenderPickerRGB; ColourEditDlg::RenderPickerCMYK

********************************************************************************************/

static void DrawCubeShadowAndCalcValues(RenderRegion *pRender, DocRect *VirtualSize,
										INT32 PixelSize, DialogColourInfo *RedrawColours,
										DocRect *ResultRect, INT32 *ResultSizeZ)
{
	DocColour Trans(COLOUR_TRANS);

	// Calculate the basic cube shape
	INT32 MaxSize = VirtualSize->Height();
	if (VirtualSize->Width() < MaxSize)
		MaxSize = VirtualSize->Width();

	const INT32 SizeXY = (100 * MaxSize) / 135;		// Size of the X and Y projections
	INT32 SizeZ  = MaxSize - SizeXY;					// Size of the Z projection

	// Ensure it's a square, aligned in the left side of the available area
	DocRect SquareRect;
	SquareRect.lo.x = HalfGridLock(VirtualSize->lo.x,	  				PixelSize);
	SquareRect.hi.x = HalfGridLock(VirtualSize->lo.x + SizeXY,			PixelSize);
	SquareRect.lo.y = HalfGridLock(VirtualSize->lo.y + SizeZ,			PixelSize);
	SquareRect.hi.y = HalfGridLock(VirtualSize->lo.y + SizeXY + SizeZ,	PixelSize);

	if (VirtualSize->Height() > MaxSize)
	{
		// The area we're in is higher than it is wide, so center the square vertically
		SquareRect.Translate(0, (VirtualSize->Height() - MaxSize) / 2);
	}

	GridLockRect(&SquareRect, PixelSize);

	SizeZ = HalfGridLock(SizeZ-ZSLIDERSIZE, PixelSize);	// Gridlock & Leave gap at bot-rt for Z-slider drag icon

	if (pRender != NULL)
	{
		pRender->SaveContext();
#if FALSE
/*
		// Draw the cube 'shadow'
		DocColour CubeFaceGrey(168L, 168L, 168L);
		pRender->SetFillColour(CubeFaceGrey);
		pRender->DrawRect(&SquareRect);

		DocColour CubeFillGrey1(176L, 176L, 176L);		// **** !!!! Perhaps we should use transparent black!
		pRender->SetFillColour(CubeFillGrey1);

		// Draw the Z extent of the 'cube' - right side
		DocCoord BottomRight(HalfGridLock(SquareRect.hi.x + SizeZ, PixelSize),
							 HalfGridLock(SquareRect.lo.y - SizeZ, PixelSize));

		Path ZPath1;
		ZPath1.Initialise(12, 12);
		ZPath1.FindStartOfPath();
		ZPath1.InsertMoveTo(BottomRight);
		ZPath1.InsertLineTo(DocCoord(SquareRect.hi.x, SquareRect.lo.y));
		ZPath1.InsertLineTo(DocCoord(SquareRect.hi.x, SquareRect.hi.y));
		ZPath1.InsertLineTo(DocCoord(SquareRect.hi.x + SizeZ, SquareRect.hi.y - SizeZ));
		ZPath1.IsFilled = TRUE;

		pRender->DrawPath(&ZPath1);


		DocColour CubeFillGrey2(160L, 160L, 160L);		// **** !!!! Perhaps we should use transparent black!
		pRender->SetFillColour(CubeFillGrey2);

		// Draw the Z extent of the 'cube' - bottom side
		Path ZPath2;
		ZPath2.Initialise(12, 12);
		ZPath2.FindStartOfPath();
		ZPath2.InsertMoveTo(BottomRight);
		ZPath2.InsertLineTo(DocCoord(SquareRect.lo.x + SizeZ, SquareRect.lo.y - SizeZ));
		ZPath2.InsertLineTo(SquareRect.lo);
		ZPath2.InsertLineTo(DocCoord(SquareRect.hi.x, SquareRect.lo.y));
		ZPath2.IsFilled = TRUE;

		pRender->DrawPath(&ZPath2);

		// Draw the 3 inside-edge lines at the back of the cube
		DocRect BackFaceRect(SquareRect);
		BackFaceRect.Translate(SizeZ, -SizeZ);

		// Draw the 3 back edges of the cube (The front edges are added by the caller,
		// on top of the the colour slice square)
		DocColour BackEdgeGrey(144L, 144L, 144L);
		pRender->SetLineColour(BackEdgeGrey);
		pRender->SetFillColour(Trans);

		DocCoord TLBPoint(BackFaceRect.lo.x, BackFaceRect.hi.y);
		pRender->DrawLine(TLBPoint, BackFaceRect.hi);
		pRender->DrawLine(TLBPoint, BackFaceRect.lo);
		pRender->DrawLine(TLBPoint, DocCoord(SquareRect.lo.x, SquareRect.hi.y));
*/
#else
		// Draw the Back Face
		DocRect BackFaceRect(SquareRect);
		BackFaceRect.Translate(SizeZ, -SizeZ);

		DocColour CubeFaceGrey(136L, 136L, 136L);
		pRender->SetFillColour(CubeFaceGrey);
		pRender->DrawRect(&BackFaceRect);

		// Draw the (Back-)Left face
		DocCoord BackTopLeft(HalfGridLock(SquareRect.lo.x + SizeZ, PixelSize),
							 HalfGridLock(SquareRect.hi.y - SizeZ, PixelSize));
		Path ZPath1;
		ZPath1.Initialise(12, 12);
		ZPath1.FindStartOfPath();
		ZPath1.InsertMoveTo(BackTopLeft);
		ZPath1.InsertLineTo(DocCoord(SquareRect.lo.x, SquareRect.hi.y));
		ZPath1.InsertLineTo(SquareRect.lo);
		ZPath1.InsertLineTo(DocCoord(SquareRect.lo.x + SizeZ, SquareRect.lo.y - SizeZ));
		ZPath1.IsFilled = TRUE;

		LinearFillAttribute DepthFill1;
		DepthFill1.Colour		= DocColour(180L, 180L, 180L);
		DepthFill1.EndColour	= DocColour(144L, 144L, 144L);
		DocCoord ThePoint(SquareRect.lo.x, BackTopLeft.y);
		DepthFill1.SetStartPoint(&ThePoint);
		DepthFill1.SetEndPoint(&BackTopLeft);
		DepthFill1.SetEndPoint2(NULL);
		pRender->SetFillGeometry(&DepthFill1, FALSE);

		pRender->DrawPath(&ZPath1);


		// Draw the (Back-)Top face
		Path ZPath2;
		ZPath2.Initialise(12, 12);
		ZPath2.FindStartOfPath();
		ZPath2.InsertMoveTo(BackTopLeft);
		ZPath2.InsertLineTo(DocCoord(SquareRect.hi.x + SizeZ, BackTopLeft.y));
		ZPath2.InsertLineTo(SquareRect.hi);
		ZPath2.InsertLineTo(DocCoord(SquareRect.lo.x, SquareRect.hi.y));
		ZPath2.IsFilled = TRUE;

		LinearFillAttribute DepthFill2;
		DepthFill2.Colour		= DocColour(182L, 182L, 182L);
		DepthFill2.EndColour	= DocColour(144L, 144L, 144L);
		ThePoint = DocCoord(BackTopLeft.x, SquareRect.hi.y);
		DepthFill2.SetStartPoint(&ThePoint);
		DepthFill2.SetEndPoint(&BackTopLeft);
		DepthFill2.SetEndPoint2(NULL);
		pRender->SetFillGeometry(&DepthFill2, FALSE);

		pRender->DrawPath(&ZPath2);
#endif

		pRender->RestoreContext();
	}

	// And finally, return the useful values we've calculated
	*ResultRect  = SquareRect;
	*ResultSizeZ = SizeZ;
}



/********************************************************************************************

>	CrossConstraint ColourEditDlg::CalcCrossRectRGB(ColourRGBT *SourceColour, DocRect *SquareRect,
											INT32 PixelSize, DocRect *Result)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/12/94

	Inputs:		SourceColour - the colour definition for which the cross rect must be calc'd
				SquareRect - The rect containing th 3-d cube 'slice' to plot the cross in
				PixelSize - the usual (size of a pixel in millipoints)
	
	Outputs:	Result will be returned containing the CrossRect (in which to draw the cross)

	Returns:	A value indicating how the cross is constrained, so you can draw constraint
				bubbles as appropriate.

	Purpose:	Determines the 3-D RGB picker cross position for the 2-component picker
				slice-through-the-cube display

	SeeAlso:	ColourEditDlg::SetNewValueRGB; ColourEditDlg::RenderPickerRGB

********************************************************************************************/

CrossConstraint ColourEditDlg::CalcCrossRectRGB(ColourRGBT *SourceColour, DocRect *SquareRect,
										INT32 PixelSize, DocRect *Result)
{
	DocRect CrossRect(-CROSSRADIUS, -CROSSRADIUS, CROSSRADIUS, CROSSRADIUS);

	FIXED24 XComponent;
	FIXED24 YComponent;

	CrossConstraint ReturnVal = CROSSCONSTRAINED_NONE;

	switch(ColourPickerMode)
	{
		case 1:		// XYZ = R,B,G
			XComponent = SourceColour->Red;
			YComponent = SourceColour->Blue;

			if (EditingColour->InheritsComponent(3))
				ReturnVal = CROSSCONSTRAINED_HORZ;
				 
			if (EditingColour->InheritsComponent(1))
			{
				if (ReturnVal == CROSSCONSTRAINED_NONE)
					ReturnVal = CROSSCONSTRAINED_VERT;
				else
					ReturnVal = CROSSCONSTRAINED_BOTH;
			}
			break;

		case 2:		// XYZ = B,G,R
			XComponent = SourceColour->Blue;
			YComponent = SourceColour->Green;

			if (EditingColour->InheritsComponent(2))
				ReturnVal = CROSSCONSTRAINED_HORZ;
				 
			if (EditingColour->InheritsComponent(3))
			{
				if (ReturnVal == CROSSCONSTRAINED_NONE)
					ReturnVal = CROSSCONSTRAINED_VERT;
				else
					ReturnVal = CROSSCONSTRAINED_BOTH;
			}
			break;

		default:	// XYZ = G,R,B
			XComponent = SourceColour->Green;
			YComponent = SourceColour->Red;

			if (EditingColour->InheritsComponent(1))
				ReturnVal = CROSSCONSTRAINED_HORZ;
				 
			if (EditingColour->InheritsComponent(2))
			{
				if (ReturnVal == CROSSCONSTRAINED_NONE)
					ReturnVal = CROSSCONSTRAINED_VERT;
				else
					ReturnVal = CROSSCONSTRAINED_BOTH;
			}
			break;
	}

	INT32 SizeXY = SquareRect->Height();
	INT32 XTrans = (INT32) ((double)SizeXY * XComponent.MakeDouble());
	INT32 YTrans = (INT32) ((double)SizeXY * YComponent.MakeDouble());

	// Translate the cross to the appropriate position
	CrossRect.Translate(HalfGridLock(SquareRect->lo.x + XTrans, PixelSize),
						HalfGridLock(SquareRect->lo.y + YTrans, PixelSize));

	*Result = CrossRect;

	if (EditingColour->GetType() == COLOURTYPE_TINT)
		return(CROSSCONSTRAINED_BOTH);

	return(ReturnVal);
}



/********************************************************************************************

>	void ColourEditDlg::RenderPickerRGB(RenderRegion *pRender, DocRect *VirtualSize,
										INT32 PixelSize, DialogColourInfo *RedrawColours)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/12/94

	Inputs:		pRender - The render region to render the picker in
				VirtualSize - The rectangle defining the coordinate space to draw into
				PixelSize - The size of a pixel as mapped into the MILLIPOINT VirtualSize
				coordinate space; ensures things line up on display pixel boundaries.
				RedrawColours - A pointer to a DialogColourInfo object describing
				OS-friendly drawing colours to be used in redrawing the control.

	Purpose:	Redraws a colour picker control for the colour editor dialogue.
				This is used in the given display mode to display the colour information
				in a useful format. e.g. in HSV mode, a wheel and slider arrangement.

	SeeAlso:	ColourEditDlg::RenderControl; ColourEditDlg::RenderPickerDefault

********************************************************************************************/

void ColourEditDlg::RenderPickerRGB(RenderRegion *pRender, DocRect *VirtualSize,
									INT32 PixelSize, DialogColourInfo *RedrawColours)
{
	pRender->SaveContext();		// Save the current rendering context
	DocColour Trans(COLOUR_TRANS);

	// First, set up the render region, and fill the background with Dialogue BG colour
	pRender->SetLineWidth(0);
	pRender->SetLineColour(Trans);
//	pRender->SetFillColour(RedrawColours->DialogBack());
//	pRender->DrawRect(VirtualSize);

	// Draw the cube 'shadow', and calculate where the cube sits
	DocRect SquareRect;
	INT32 SizeZ;
	DrawCubeShadowAndCalcValues(pRender, VirtualSize, PixelSize, RedrawColours,
								/* TO */ &SquareRect, &SizeZ);

	DocRect FrontFaceRect(SquareRect);		// Remember the old squarerect for later

#if FALSE
/*
	Old code. Works fine normally, but due to using bleach trnsparency, fails to work
	properly in "show printer colours" preview mode. Also, the new 4-colour fill
	method below is faster, as it only does one path fill rather than 3 (plus transparency)

	// Find the source colour definition in RGB space
	ColourRGBT SourceColour;
	if (EditingColour != NULL)
	{
		ColourContextRGBT *ccRGB = (ColourContextRGBT *)
									ColourContext::GetGlobalDefault(COLOURMODEL_RGBT);
		ERROR3IF(ccRGB == NULL, "Can't get an RGB colour context");

		ccRGB->ConvertColour(EditingColour, (ColourGeneric *) &SourceColour);
	}
	else
	{
		SourceColour.Red =SourceColour.Green = SourceColour.Blue = FIXED24(1.0);
		SourceColour.Transparent = 0;
	}

	// Now, Move the 'SquareRect' down and right so its bottom corner touches the Z axis
	// at the appropriate point for the current Z value. (i.e. slice the cube through at
	// the place where the slice really is)
	if (EditingColour != NULL)
	{
		FIXED24 ZComponent = SourceColour.Blue;
		if (ColourPickerMode == 1)
			ZComponent = SourceColour.Green;
		else if (ColourPickerMode == 2)
			ZComponent = SourceColour.Red;

		INT32 ZTrans = (INT32) ((double)SizeZ * ZComponent.MakeDouble()) - SizeZ;
		SquareRect.Translate(-ZTrans, ZTrans);
		GridLockRect(&SquareRect, PixelSize);
	}

	// Now, composite up the picker square, which is made of 3 squares on top of each
	// other, all flat-filled in primary colours, the top 2 having horz/vert trans fills
	ColourRGBT BaseColour;
	ColourRGBT MidColour;
	ColourRGBT TopColour;

	BaseColour.Red = BaseColour.Green = BaseColour.Blue = BaseColour.Transparent = 0;
	memcpy(&MidColour, &BaseColour, sizeof(ColourRGBT));
	memcpy(&TopColour, &BaseColour, sizeof(ColourRGBT));

	switch(ColourPickerMode)
	{
		case 1:		// XYZ = R,B,G
			MidColour.Red		= FIXED24(1.0);
			TopColour.Blue		= FIXED24(1.0);
			BaseColour.Green	= SourceColour.Green;
			break;

		case 2:		// XYZ = B,G,R
			MidColour.Blue		= FIXED24(1.0);
			TopColour.Green		= FIXED24(1.0);
			BaseColour.Red		= SourceColour.Red;
			break;

		default:	// XYZ = G,R,B
			MidColour.Green	= FIXED24(1.0);
			TopColour.Red	= FIXED24(1.0);
			BaseColour.Blue	= SourceColour.Blue;

			ColourPickerMode = 0;		// Ensure any out of value setting is made valid
			break;
	}

	RenderPickerSquare(pRender, &SquareRect,
						(ColourGeneric *) &BaseColour, (ColourGeneric *) &MidColour,
						(ColourGeneric *) &TopColour,
						COLOURMODEL_RGBT, 3);	// Use RGB + 'bleach' (additive) transparency
*/
#else
	// Find the source colour definition in RGB space
	PColourRGBT SourceColour;
	if (EditingColour != NULL)
	{
		ColourContextRGBT *ccRGB = (ColourContextRGBT *)
									ColourContext::GetGlobalDefault(COLOURMODEL_RGBT);
		ERROR3IF(ccRGB == NULL, "Can't get an RGB colour context");

		DocColour TempRef;
		TempRef.MakeRefToIndexedColour(EditingColour);
		ccRGB->ConvertColour(&TempRef, (ColourPacked *) &SourceColour);
	}
	else
	{
		SourceColour.Red =SourceColour.Green = SourceColour.Blue = 255;
		SourceColour.Transparent = 0;
	}

	// Now, Move the 'SquareRect' down and right so its bottom corner touches the Z axis
	// at the appropriate point for the current Z value. (i.e. slice the cube through at
	// the place where the slice really is)

	if (EditingColour != NULL)
	{
		INT32 ZComponent = SourceColour.Blue;
		if (ColourPickerMode == 1)
			ZComponent = SourceColour.Green;
		else if (ColourPickerMode == 2)
			ZComponent = SourceColour.Red;

		INT32 ZTrans = (INT32) (((double)SizeZ * ZComponent) / 255.0) - SizeZ;
		SquareRect.Translate(-ZTrans, ZTrans);
		GridLockRect(&SquareRect, PixelSize);
	}

	// Draw a 4-colour fill for the cube face
	FourColFillAttribute Fill;

	// Set the points to be in the corners of the square rect
	Fill.SetStartPoint(&SquareRect.lo);								// Bottom left
	DocCoord Corner = DocCoord(SquareRect.hi.x, SquareRect.lo.y);
	Fill.SetEndPoint(&Corner);										// Bottom right
	Corner = DocCoord(SquareRect.lo.x, SquareRect.hi.y);
	Fill.SetEndPoint2(&Corner);										// Top left
	Fill.SetEndPoint3(&SquareRect.hi);								// Top right

	// And set the corner colours
	DocColour CornerCol;
	switch(ColourPickerMode)
	{
		case 1:		// XYZ = R,B,G
			CornerCol = DocColour(0L, SourceColour.Green, 0L);				// Bottom left
			Fill.SetStartColour(&CornerCol);
			CornerCol = DocColour(255L, SourceColour.Green, 0L);			// Bottom right
			Fill.SetEndColour(&CornerCol);
			CornerCol = DocColour(0L, SourceColour.Green, 255L);			// Top left
			Fill.SetEndColour2(&CornerCol);
			CornerCol = DocColour(255L, SourceColour.Green, 255L);			// Top right
			Fill.SetEndColour3(&CornerCol);
			break;

		case 2:		// XYZ = B,G,R
			CornerCol = DocColour(SourceColour.Red, 0L, 0L);				// Bottom left
			Fill.SetStartColour(&CornerCol);
			CornerCol = DocColour(SourceColour.Red, 0L, 255L);				// Bottom right
			Fill.SetEndColour(&CornerCol);
			CornerCol = DocColour(SourceColour.Red, 255L, 0L);				// Top left
			Fill.SetEndColour2(&CornerCol);
			CornerCol = DocColour(SourceColour.Red, 255L, 255L);			// Top right
			Fill.SetEndColour3(&CornerCol);
			break;

		default:	// XYZ = G,R,B
			CornerCol = DocColour(0L, 0L, SourceColour.Blue);				// Bottom left
			Fill.SetStartColour(&CornerCol);
			CornerCol = DocColour(0L, 255L, SourceColour.Blue);				// Bottom right
			Fill.SetEndColour(&CornerCol);
			CornerCol = DocColour(255L, 0L, SourceColour.Blue);				// Top left
			Fill.SetEndColour2(&CornerCol);
			CornerCol = DocColour(255L, 255L, SourceColour.Blue);			// Top right
			Fill.SetEndColour3(&CornerCol);
			break;
	}

	// Build a path for the rectangle (DrawRect doesn't do grad fills)
	Path SquarePath;
	SquarePath.Initialise(12, 12);
	SquarePath.FindStartOfPath();
	SquarePath.InsertMoveTo(SquareRect.lo);
	SquarePath.InsertLineTo(DocCoord(SquareRect.lo.x, SquareRect.hi.y));
	SquarePath.InsertLineTo(SquareRect.hi);
	SquarePath.InsertLineTo(DocCoord(SquareRect.hi.x, SquareRect.lo.y));
	SquarePath.IsFilled = TRUE;

	pRender->SetLineColour(Trans);
	pRender->SetFillGeometry(&Fill, FALSE);

	// And render it
	pRender->DrawPath(&SquarePath);
#endif

	// Place the outline of the front of the cube on top
	DocColour CubeFaceGrey(180L, 180L, 180L);
	pRender->SetLineColour(CubeFaceGrey);
	pRender->SetFillColour(Trans);

	DocCoord BotRightFront(FrontFaceRect.hi.x, FrontFaceRect.lo.y);
	pRender->DrawLine(BotRightFront, FrontFaceRect.lo);
	pRender->DrawLine(BotRightFront, FrontFaceRect.hi);
	pRender->DrawLine(BotRightFront,
					DocCoord(FrontFaceRect.hi.x + SizeZ, FrontFaceRect.lo.y - SizeZ));

	// Draw the swap-axes button
	DocCoord BitmapPos;
	BitmapPos.x = HalfGridLock(FrontFaceRect.hi.x + SizeZ/2, PixelSize);
	BitmapPos.y = HalfGridLock(FrontFaceRect.hi.y - SizeZ/2, PixelSize);

	const INT32 BitmapSize = 14 * PixelSize;
	DocRect SwapAxesStrip(BitmapPos.x + BitmapSize, BitmapPos.y,
							BitmapPos.x + BitmapSize + PixelSize * 3, BitmapPos.y + BitmapSize);

	GridLockRect(&SwapAxesStrip, PixelSize);

	DocColour SwapAxesFore(0L, 0L, 0L);
	DocColour SwapAxesFill(0L, 0L, 255L);
	if (ColourPickerMode == 1)
		SwapAxesFill = DocColour(0L, 255L, 0L);
	else if (ColourPickerMode == 2)
		SwapAxesFill = DocColour(255L, 0L, 0L);

	pRender->SetFillColour(SwapAxesFill);
	pRender->SetLineColour(SwapAxesFore);
	pRender->DrawRect(&SwapAxesStrip);

	// Render the bitmap last, to ensure we don't overwrite it
	pRender->DrawBitmap(BitmapPos, _R(IDB_PICKER_SWAPAXIS));


	DocColour MidGrey(128L, 128L, 128L);

	if (EditingColour != NULL)
	{
		// And draw a drag icon on the current Z position
		DocRect ZButton(SquareRect);
		ZButton.lo.x = ZButton.hi.x;
		ZButton.hi.x += ZSLIDERSIZE - (PixelSize * 2);
		ZButton.hi.y = ZButton.lo.y;
		ZButton.lo.y -= ZSLIDERSIZE - (PixelSize * 2);
		GridLockRect(&ZButton, PixelSize);
		
		pRender->DrawBitmap(ZButton.lo, _R(IDB_PICKER_ZSLIDER));

		DocRect CrossRect;
		ColourRGBT SourceDef;
		if (EditingColour != NULL)
		{
			ColourContextRGBT *ccRGB = (ColourContextRGBT *)
										ColourContext::GetGlobalDefault(COLOURMODEL_RGBT);
			ERROR3IF(ccRGB == NULL, "Can't get an RGB colour context");

			ccRGB->ConvertColour(EditingColour, (ColourGeneric *) &SourceDef);
		}
		else
		{
			SourceDef.Red =SourceDef.Green = SourceDef.Blue = FIXED24(1.0);
			SourceDef.Transparent = 0;
		}

		CrossConstraint Constraint = CalcCrossRectRGB(&SourceDef, &SquareRect, PixelSize, &CrossRect);
		RenderCross(pRender, &CrossRect, PixelSize, (Constraint == CROSSCONSTRAINED_BOTH));

		if (Constraint == CROSSCONSTRAINED_HORZ)
		{
			INT32 MidY = (CrossRect.lo.y + CrossRect.hi.y) / 2;
			RenderConstraint(pRender, DocCoord(SquareRect.lo.x, MidY), DocCoord(SquareRect.hi.x, MidY));
		}
		else if (Constraint == CROSSCONSTRAINED_VERT)
		{
			INT32 MidX = (CrossRect.lo.x + CrossRect.hi.x) / 2;
			RenderConstraint(pRender, DocCoord(MidX, SquareRect.lo.y), DocCoord(MidX, SquareRect.hi.y));
		}
	}

	pRender->RestoreContext();
}



/********************************************************************************************

>	CrossConstraint ColourEditDlg::CalcCrossRectCMYK(ColourCMYK *SourceColour, DocRect *SquareRect,
											INT32 PixelSize, DocRect *Result)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/12/94

	Inputs:		SourceColour - the colour definition for which the cross rect must be calc'd
				SquareRect - The rect containing th 3-d cube 'slice' to plot the cross in
				PixelSize - the usual (size of a pixel in millipoints)
	
	Outputs:	Result will be returned containing the CrossRect (in which to draw the cross)

	Returns:	A value indicating how the cross is constrained, so you can draw constraint
				bubbles as appropriate.

	Purpose:	Determines the 3-D CMYK picker cross position for the 2-component picker
				slice-through-the-cube display

	SeeAlso:	ColourEditDlg::SetNewValueCMYK; ColourEditDlg::RenderPickerCMYK

********************************************************************************************/

CrossConstraint ColourEditDlg::CalcCrossRectCMYK(ColourCMYK *SourceColour, DocRect *SquareRect,
										INT32 PixelSize, DocRect *Result)
{
	DocRect CrossRect(-CROSSRADIUS, -CROSSRADIUS, CROSSRADIUS, CROSSRADIUS);

	FIXED24 XComponent;
	FIXED24 YComponent;
	CrossConstraint ReturnVal = CROSSCONSTRAINED_NONE;

	switch(ColourPickerMode)
	{
		case 1:		// XYZ = Y,M,C
			XComponent = SourceColour->Yellow;
			YComponent = SourceColour->Magenta;

			if (EditingColour->InheritsComponent(2))
				ReturnVal = CROSSCONSTRAINED_HORZ;
				 
			if (EditingColour->InheritsComponent(3))
			{
				if (ReturnVal == CROSSCONSTRAINED_NONE)
					ReturnVal = CROSSCONSTRAINED_VERT;
				else
					ReturnVal = CROSSCONSTRAINED_BOTH;
			}
			break;

		case 2:		// XYZ = M,C,Y
			XComponent = SourceColour->Magenta;
			YComponent = SourceColour->Cyan;

			if (EditingColour->InheritsComponent(1))
				ReturnVal = CROSSCONSTRAINED_HORZ;
				 
			if (EditingColour->InheritsComponent(2))
			{
				if (ReturnVal == CROSSCONSTRAINED_NONE)
					ReturnVal = CROSSCONSTRAINED_VERT;
				else
					ReturnVal = CROSSCONSTRAINED_BOTH;
			}
			break;

		default:	// XYZ = C,Y,M
			XComponent = SourceColour->Cyan;
			YComponent = SourceColour->Yellow;

			if (EditingColour->InheritsComponent(3))
				ReturnVal = CROSSCONSTRAINED_HORZ;
				 
			if (EditingColour->InheritsComponent(1))
			{
				if (ReturnVal == CROSSCONSTRAINED_NONE)
					ReturnVal = CROSSCONSTRAINED_VERT;
				else
					ReturnVal = CROSSCONSTRAINED_BOTH;
			}
			break;
	}

	INT32 SizeXY = SquareRect->Height();
	INT32 XTrans = (INT32) ((double)SizeXY * XComponent.MakeDouble());
	INT32 YTrans = (INT32) ((double)SizeXY * YComponent.MakeDouble());

	// Translate the cross to the appropriate position
	CrossRect.Translate(HalfGridLock(SquareRect->lo.x + XTrans, PixelSize),
						HalfGridLock(SquareRect->lo.y + YTrans, PixelSize));

	*Result = CrossRect;

	if (EditingColour->GetType() == COLOURTYPE_TINT)
		return(CROSSCONSTRAINED_BOTH);

	return(ReturnVal);
}



/********************************************************************************************

>	void ColourEditDlg::RenderPickerCMYK(RenderRegion *pRender, DocRect *VirtualSize,
										INT32 PixelSize, DialogColourInfo *RedrawColours)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/12/94

	Inputs:		pRender - The render region to render the picker in
				VirtualSize - The rectangle defining the coordinate space to draw into
				PixelSize - The size of a pixel as mapped into the MILLIPOINT VirtualSize
				coordinate space; ensures things line up on display pixel boundaries.
				RedrawColours - A pointer to a DialogColourInfo object describing
				OS-friendly drawing colours to be used in redrawing the control.

	Purpose:	Redraws a colour picker control for the colour editor dialogue.
				This is used in the given display mode to display the colour information
				in a useful format. e.g. in HSV mode, a wheel and slider arrangement.

	SeeAlso:	ColourEditDlg::RenderControl; ColourEditDlg::RenderPickerDefault

********************************************************************************************/

void ColourEditDlg::RenderPickerCMYK(RenderRegion *pRender, DocRect *VirtualSize,
									INT32 PixelSize, DialogColourInfo *RedrawColours)
{
	pRender->SaveContext();		// Save the current rendering context
	DocColour Trans(COLOUR_TRANS);

	// Determine how much space is left over after the key slider has been placed
	INT32 KeySliderLeft = VirtualSize->hi.x - (CROSSRADIUS+PixelSize)*2;

	ERROR3IF(KeySliderLeft < VirtualSize->lo.x,
				"Not enough room to render the colour picker!");

	// First, set up the render region, and fill the background with Dialogue BG colour
	pRender->SetLineWidth(0);
	pRender->SetLineColour(Trans);
//	pRender->SetFillColour(RedrawColours->DialogBack());
//	pRender->DrawRect(VirtualSize);

	// Draw the cube 'shadow', and calculate where the cube sits
	DocRect CubeAvailableSpace(*VirtualSize);
	CubeAvailableSpace.hi.x = KeySliderLeft - 2000;

	DocRect SquareRect;
	INT32 SizeZ;
	DrawCubeShadowAndCalcValues(pRender, &CubeAvailableSpace, PixelSize, RedrawColours,
								/* TO */ &SquareRect, &SizeZ);

	// Find the source colour definition in RGB space
	ColourCMYK SourceColour;
	if (EditingColour != NULL)
	{
//		ColourContextCMYK *ccCMYK = (ColourContextCMYK*)ColourContext::GetGlobalDefault(COLOURMODEL_CMYK);
		ColourContext *ccCMYK = NULL;
		BOOL bDeleteCC = GetColourContext(COLOURMODEL_CMYK, &ccCMYK);	
		ERROR3IF(ccCMYK == NULL, "Can't get a CMYK colour context");

		ccCMYK->ConvertColour(EditingColour, (ColourGeneric *) &SourceColour);

		// Delete the colour context if necessary
		if (bDeleteCC)
			ColourContextList::GetList()->RemoveContext(&ccCMYK);			// Have finished with it
	}
	else
	{
		SourceColour.Cyan =SourceColour.Magenta = SourceColour.Yellow = SourceColour.Key = 0;
	}

	DocRect FrontFaceRect(SquareRect);		// Remember the old squarerect for later

	// Now, Move the 'SquareRect' down and left so its bottom corner touches the Z axis
	// at the appropriate point for the current Z value. (i.e. slice the cube through at
	// the place where the slice really is)
	if (EditingColour != NULL)
	{
		FIXED24 ZComponent = SourceColour.Magenta;
		if (ColourPickerMode == 1)
			ZComponent = SourceColour.Cyan;
		else if (ColourPickerMode == 2)
			ZComponent = SourceColour.Yellow;

		INT32 ZTrans = (INT32) ((double)SizeZ * ZComponent.MakeDouble()) - SizeZ;
		SquareRect.Translate(-ZTrans, ZTrans);
		GridLockRect(&SquareRect, PixelSize);
	}


	// Now, composite up the picker square, which is made of 3 squares on top of each
	// other, all flat-filled in primary colours, the top 2 having horz/vert trans fills
	ColourCMYK BaseColour;
	ColourCMYK MidColour;
	ColourCMYK TopColour;

	BaseColour.Cyan = BaseColour.Magenta = BaseColour.Yellow = 0;
	BaseColour.Key = SourceColour.Key;
	memcpy(&MidColour, &BaseColour, sizeof(ColourCMYK));
	memcpy(&TopColour, &BaseColour, sizeof(ColourCMYK));

	switch(ColourPickerMode)
	{
		case 1:		// XYZ = Y,M,C
			MidColour.Yellow	= FIXED24(1.0);
			TopColour.Magenta	= FIXED24(1.0);
			BaseColour.Cyan		= SourceColour.Cyan;
			break;

		case 2:		// XYZ = M,C,Y
			MidColour.Magenta	= FIXED24(1.0);
			TopColour.Cyan		= FIXED24(1.0);
			BaseColour.Yellow	= SourceColour.Yellow;
			break;

		default:	// XYZ = C,Y,M
			MidColour.Cyan		= FIXED24(1.0);
			TopColour.Yellow	= FIXED24(1.0);
			BaseColour.Magenta	= SourceColour.Magenta;

			ColourPickerMode = 0;		// Ensure any out of value setting is made valid
			break;
	}

	RenderPickerSquare(pRender, &SquareRect,
						(ColourGeneric *) &BaseColour, (ColourGeneric *) &MidColour,
						(ColourGeneric *) &TopColour,
						COLOURMODEL_CMYK, 2);	// Use CMYK + 'Stain' (Filtering) transparency

	// Place the outline of the front of the cube on top
	DocColour CubeFaceGrey(180L, 180L, 180L);
	pRender->SetLineColour(CubeFaceGrey);
	pRender->SetFillColour(Trans);

	DocCoord BotRightFront(FrontFaceRect.hi.x, FrontFaceRect.lo.y);
	pRender->DrawLine(BotRightFront, FrontFaceRect.lo);
	pRender->DrawLine(BotRightFront, FrontFaceRect.hi);
	pRender->DrawLine(BotRightFront,
					DocCoord(FrontFaceRect.hi.x + SizeZ, FrontFaceRect.lo.y - SizeZ));


	// Render the Key slider - a white->black grad filled rectangle
	LinearFillAttribute KeyGradFill;
	KeyGradFill.Colour		= DocColour(0L, 0L, 0L);
	KeyGradFill.EndColour	= DocColour(255L, 255L, 255L);
	DocCoord ThePoint(VirtualSize->hi.x, VirtualSize->hi.y - (PATCHSIZE + PixelSize*2));
	KeyGradFill.SetStartPoint(&ThePoint);
	ThePoint = DocCoord(VirtualSize->hi.x, VirtualSize->lo.y);
	KeyGradFill.SetEndPoint(&ThePoint);
	KeyGradFill.SetEndPoint2(NULL);
	pRender->SetFillGeometry(&KeyGradFill, FALSE);
	pRender->SetLineColour(Trans);

	Path KeyPath;
	KeyPath.Initialise(12, 12);
	KeyPath.FindStartOfPath();

	KeyPath.InsertMoveTo(DocCoord(KeySliderLeft, VirtualSize->lo.y));
	KeyPath.InsertLineTo(DocCoord(VirtualSize->hi.x, VirtualSize->lo.y));
	KeyPath.InsertLineTo(DocCoord(VirtualSize->hi.x, VirtualSize->hi.y - (PATCHSIZE + PixelSize*2)));
	KeyPath.InsertLineTo(DocCoord(KeySliderLeft, VirtualSize->hi.y - (PATCHSIZE + PixelSize*2)));
	KeyPath.IsFilled = TRUE;
	pRender->DrawPath(&KeyPath);		// Render the value/brightness strip


	// Draw the swap-axes button
	DocCoord BitmapPos;
	BitmapPos.x = HalfGridLock(FrontFaceRect.hi.x + SizeZ/2, PixelSize);
	BitmapPos.y = HalfGridLock(FrontFaceRect.hi.y - SizeZ/2, PixelSize);

	const INT32 BitmapSize = 14 * PixelSize;
	DocRect SwapAxesStrip(BitmapPos.x + BitmapSize, BitmapPos.y,
							BitmapPos.x + BitmapSize + PixelSize * 3, BitmapPos.y + BitmapSize);

	GridLockRect(&SwapAxesStrip, PixelSize);

	DocColour SwapAxesFore(0L, 0L, 0L);
	DocColour SwapAxesFill(255L, 0L, 255L);
	if (ColourPickerMode == 1)
		SwapAxesFill = DocColour(0L, 255L, 255L);
	else if (ColourPickerMode == 2)
		SwapAxesFill = DocColour(255L, 255L, 0L);

	pRender->SetFillColour(SwapAxesFill);
	pRender->SetLineColour(SwapAxesFore);
	pRender->DrawRect(&SwapAxesStrip);

	// Render the bitmap last, to ensure we don't overwrite it
	pRender->DrawBitmap(BitmapPos, _R(IDB_PICKER_SWAPAXIS));


	DocColour MidGrey(128L, 128L, 128L);
	if (EditingColour != NULL)
	{
		// And draw a drag icon on the current Z position **** !!!! ToDo: Should be a bitmap icon
		DocRect ZButton(SquareRect);
		ZButton.lo.x = ZButton.hi.x;
		ZButton.hi.x += ZSLIDERSIZE - (PixelSize * 2);
		ZButton.hi.y = ZButton.lo.y;
		ZButton.lo.y -= ZSLIDERSIZE - (PixelSize * 2);
		GridLockRect(&ZButton, PixelSize);
		
		pRender->DrawBitmap(ZButton.lo, _R(IDB_PICKER_ZSLIDER));

		// Render marker cross in the appropriate X-Y place for the definition
		// of the current editing colour
		DocRect CrossRect;
		CrossConstraint Constraint = CalcCrossRectCMYK(&SourceColour, &SquareRect, PixelSize, &CrossRect);
		RenderCross(pRender, &CrossRect, PixelSize, (Constraint == CROSSCONSTRAINED_BOTH));

		// And render constraint bubbles if necessary
		if (Constraint == CROSSCONSTRAINED_HORZ)
		{
			INT32 MidY = (CrossRect.lo.y + CrossRect.hi.y) / 2;
			RenderConstraint(pRender, DocCoord(SquareRect.lo.x, MidY), DocCoord(SquareRect.hi.x, MidY));
		}
		else if (Constraint == CROSSCONSTRAINED_VERT)
		{
			INT32 MidX = (CrossRect.lo.x + CrossRect.hi.x) / 2;
			RenderConstraint(pRender, DocCoord(MidX, SquareRect.lo.y), DocCoord(MidX, SquareRect.hi.y));
		}

		// And a cross on the Key slider too
		INT32 YTrans = (INT32) ((double)(VirtualSize->Height() - (PATCHSIZE + PixelSize*2)) *
											SourceColour.Key.MakeDouble());
		CrossRect = DocRect(-CROSSRADIUS, -CROSSRADIUS, CROSSRADIUS, CROSSRADIUS);
		CrossRect.Translate(HalfGridLock(VirtualSize->hi.x - (CROSSRADIUS+PixelSize), PixelSize),
							HalfGridLock(VirtualSize->lo.y + YTrans, PixelSize));

		BOOL Shaded = (EditingColour->GetType() == COLOURTYPE_TINT) ||
						(EditingColour->InheritsComponent(4));
		RenderCross(pRender, &CrossRect, PixelSize, Shaded);
	}

	pRender->RestoreContext();
}



/********************************************************************************************

	void ColourEditDlg::RenderPickerSliderH(RenderRegion *pRender, DocRect *SliderRect,
											INT32 PixelSize, FIXED24 DisplayValue,
											DocColour *LeftCol, DocColour *RightCol
											DocColour *TopLeftCol, DocColour *TopRightCol,
											BOOL Shaded = FALSE)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/94

	Inputs:		pRender - The render region to render the picker in
				SliderRect - The rectangle in which the slider is to be rendered
				DisplayValue - The value (0.0 - 1.0) to display on the slider
				LeftCol - NULL or The colour to pot at the (bottom) left end of the slider
				RightCol - NULL or The colour to plot at the (bottom) right end of the slider
				TopLeftCol - The colour to pot at the (top) left end of the slider
				TopRightCol - The colour to plot at the (top) right end of the slider
				Shaded - TRUE if the marker cross should be shown shaded

	Purpose:	Redraws a horizontal slider in a colour picker control.
				This generic method is used to draw a slider for a colour component.
				Generally, the slider will be a linear grad filled rectangle which
				shades from a colour with that component = 0.0 to 1.0	

	Notes:		The slider is a split-display.
				It shows two horizontal grad-fills: The top half shows a fill from
				TopLeftCol to TopRight Col, while the bottom half shows a fill from
				LeftCol to RightCol. If LeftCol and/or RightCol are NULL, the strip
				will be filled with a single fill, from TopLeft to TopRight colours.

				This will use the line width/colour as set on entry, so you should
				set line width = 0 and line colour = COLOUR_TRANS before calling this fn

				The marker cross will only be rendered if EditingColour != NULL

	SeeAlso:	ColourEditDlg::RenderControl; ColourEditDlg::RenderPickerDefault

********************************************************************************************/

void ColourEditDlg::RenderPickerSliderH(RenderRegion *pRender, DocRect *SliderRect,
										INT32 PixelSize, FIXED24 DisplayValue,
										DocColour *LeftCol, DocColour *RightCol,
										DocColour *TopLeftCol, DocColour *TopRightCol,
										BOOL Shaded)
{
	ERROR3IF(pRender == NULL || SliderRect == NULL ||
			 TopLeftCol == NULL || TopRightCol == NULL,
			 "ColourEditDlg::RenderPickerSliderH - NULL parameters are illegal");

	pRender->SaveContext();		// Save the current rendering context

	INT32 MidY = (SliderRect->lo.y + SliderRect->hi.y) / 2;

	LinearFillAttribute BottomGradFill;
	Path RectPath;
	if (LeftCol != NULL && RightCol != NULL)
	{
		// Are in strip mode, so... create a grad fill for the bottom strip
		BottomGradFill.Colour		= *LeftCol;
		BottomGradFill.EndColour	= *RightCol;
		DocCoord ThePoint(SliderRect->lo.x, (SliderRect->lo.y + SliderRect->hi.y) / 2);
		BottomGradFill.SetStartPoint(&ThePoint);
		ThePoint = DocCoord(SliderRect->hi.x, (SliderRect->lo.y + SliderRect->hi.y) / 2);
		BottomGradFill.SetEndPoint(&ThePoint);
		BottomGradFill.SetEndPoint2(NULL);
		pRender->SetFillGeometry(&BottomGradFill, FALSE);

		// Draw the rectangle - done as a path so we can draw with grad-fill
		RectPath.Initialise(12, 12);
		RectPath.FindStartOfPath();

		RectPath.InsertMoveTo(SliderRect->lo);
		RectPath.InsertLineTo(DocCoord(SliderRect->hi.x, SliderRect->lo.y));
		RectPath.InsertLineTo(DocCoord(SliderRect->hi.x, MidY));
		RectPath.InsertLineTo(DocCoord(SliderRect->lo.x, MidY));
		RectPath.IsFilled = TRUE;

		pRender->DrawPath(&RectPath);			// Render the bottom strip
	}

	// Create a grad fill for the top strip
	LinearFillAttribute TopGradFill;
	TopGradFill.Colour		= *TopLeftCol;
	TopGradFill.EndColour	= *TopRightCol;
	DocCoord ThePoint(SliderRect->lo.x, (SliderRect->lo.y + SliderRect->hi.y) / 2);
	TopGradFill.SetStartPoint(&ThePoint);
	ThePoint = DocCoord(SliderRect->hi.x, (SliderRect->lo.y + SliderRect->hi.y) / 2);
	TopGradFill.SetEndPoint(&ThePoint);
	TopGradFill.SetEndPoint2(NULL);

	pRender->SetFillGeometry(&TopGradFill, FALSE);

	Path TopRectPath;
	TopRectPath.Initialise(12, 12);
	TopRectPath.FindStartOfPath();

	if (LeftCol != NULL && RightCol != NULL)
	{
		// Doing split-line display, so rect is bottom half of slider
		TopRectPath.InsertMoveTo(DocCoord(SliderRect->lo.x, MidY));
		TopRectPath.InsertLineTo(DocCoord(SliderRect->hi.x, MidY));
		TopRectPath.InsertLineTo(SliderRect->hi);
		TopRectPath.InsertLineTo(DocCoord(SliderRect->lo.x, SliderRect->hi.y));
	}
	else
	{
		// Not doing split-line display, so rect is entire slider
		TopRectPath.InsertMoveTo(SliderRect->lo);
		TopRectPath.InsertLineTo(DocCoord(SliderRect->hi.x, SliderRect->lo.y));
		TopRectPath.InsertLineTo(SliderRect->hi);
		TopRectPath.InsertLineTo(DocCoord(SliderRect->lo.x, SliderRect->hi.y));
	}

	TopRectPath.IsFilled = TRUE;
	pRender->DrawPath(&TopRectPath);		// Render the top strip


	pRender->RestoreContext();				// Restore original attribute context

	
	if (EditingColour != NULL)
	{
		if (DisplayValue < FIXED24(0.0))	// Clip the value to 0-1 range
			DisplayValue = 0;

		if (DisplayValue > FIXED24(1.0))
			DisplayValue = FIXED24(1.0);

		DocRect CrossRect(-CROSSRADIUS, -CROSSRADIUS, CROSSRADIUS, CROSSRADIUS);
		INT32 TransX = SliderRect->lo.x + (INT32) ((double)SliderRect->Width() *
												  DisplayValue.MakeDouble());
		INT32 TransY = (SliderRect->lo.y + SliderRect->hi.y) / 2;
		CrossRect.Translate(HalfGridLock(TransX, PixelSize), HalfGridLock(TransY, PixelSize));
		RenderCross(pRender, &CrossRect, PixelSize, Shaded);
	}
}



/********************************************************************************************

>	void ColourEditDlg::RenderPickerDefault(RenderRegion *pRender, DocRect *VirtualSize,
										INT32 PixelSize, DialogColourInfo *RedrawColours)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/94

	Inputs:		pRender - The render region to render the picker in
				VirtualSize - The rectangle defining the coordinate space to draw into
				PixelSize - The size of a pixel as mapped into the MILLIPOINT VirtualSize
				coordinate space; ensures things line up on display pixel boundaries.
				RedrawColours - A pointer to a DialogColourInfo object describing
				OS-friendly drawing colours to be used in redrawing the control.

	Purpose:	Redraws a colour picker control for the colour editor dialogue.
				This is used in the given display mode to display the colour information
				in a useful format. e.g. in HSV mode, a wheel and slider arrangement.

	Notes:		This is called as a default method for all colour models which are not
				specifically provided for by the editor. It shows generic horizontal
				sliders for each of the available colour components. Some colour models
				(e.g. Greyscale) will always use this default, as it is fine for them.

	SeeAlso:	ColourEditDlg::RenderControl; ColourEditDlg::RenderPickerDefault

********************************************************************************************/

void ColourEditDlg::RenderPickerDefault(RenderRegion *pRender, DocRect *VirtualSize,
										INT32 PixelSize, DialogColourInfo *RedrawColours)
{
	pRender->SaveContext();		// Save the current rendering context

	// Set rendering defaults: Fill paths only, and fill background with window grey
	DocColour Trans(COLOUR_TRANS);
	pRender->SetLineWidth(0);
	pRender->SetLineColour(Trans);
//	pRender->SetFillColour(RedrawColours->DialogBack());
//	pRender->DrawRect(VirtualSize);

	ColourGeneric GradFillDef;				// Colour for slider gradfill endpoint
	GradFillDef.Component1 = 				// Initialise to all components = 0
		GradFillDef.Component2 = 
		GradFillDef.Component3 = 
		GradFillDef.Component4 = 0;

	ColourGeneric EditingColDef;			// Colour holding the current component values
	EditingColDef.Component1 =				// Default to all comps=0 in case of error
		EditingColDef.Component2 = 			// (and in case EditingCOlour == NULL)
		EditingColDef.Component3 = 
		EditingColDef.Component4 = 0;

//	ColourContext *cc = ColourContext::GetGlobalDefault(DisplayModel);
	ColourContext *cc = NULL;
	BOOL bDeleteCC = GetColourContext(DisplayModel, &cc);	
	if (cc == NULL)
	{
		pRender->RestoreContext();
		ERROR2RAW("Can't get a colour context for the DisplayModel!");
		return;
	}

	// Convert the editing colour (if any) into the current display model
	if (EditingColour != NULL)
		cc->ConvertColour(EditingColour, &EditingColDef);

	// Get a sneaky 'array of 4 components' to allow array-access into the colour definition
	ColourValue *ECComponent = (ColourValue *) &EditingColDef;	// EC =  EditingColour
	ColourValue *GFComponent = (ColourValue *) &GradFillDef;	// GF = GradFill Colour

	// Create a colour in the current DisplayModel with all components zero (probably
	// black (RGB/Grey) or white (CMYK))
	DocColour LeftColour(DisplayModel, &GradFillDef);
	DocColour TopLeftColour(DisplayModel, &GradFillDef);
	DocColour RightColour;
	DocColour TopRightColour;

	// Get the slider rectangle widths. The height is fixed/moved during the loop below
	DocRect SliderRect(*VirtualSize);
	SliderRect.hi.y -= PATCHSIZE + (PixelSize * 2);		// Allow space for the current colour patch

	// Count how many components we have to display
	INT32 NumComponents = 0;
	INT32 ComponentIndex;
	for (ComponentIndex = 1; ComponentIndex <= 4; ComponentIndex++)
	{
		if (cc->GetComponentName(ComponentIndex, NULL))
			NumComponents++;
	}

	// Calculate slider sizes and spacing
	INT32 SliderHeight = GetSliderHeight(SliderRect.Height(), NumComponents);
	INT32 SliderGap = GetSliderGap(SliderRect.Height(), NumComponents);

	// And move the top down by half a SliderGap, so the sliders are centered vertically
	SliderRect.hi.y -= SliderGap / 2;

	// Draw a slider for each component supplied in the current DisplayModel
	for (ComponentIndex = 0; ComponentIndex <= 3; ComponentIndex++)
	{
		// Ensure slider rect is the correct height
		SliderRect.lo.y = SliderRect.hi.y - SliderHeight;

		// If this component is available/used in this colour model, render it
		if (cc->GetComponentName(ComponentIndex+1, NULL))
		{
			// TopLeft colour is static (always all zeros)
			// We calculate the other 3 colours here: 
			//   TopRight is all zeros except this component is 1.0
			//   Left/Right go from EditingColour with this component = 0.0 to this = 1.0
			GradFillDef.Component1 = 		// Initialise to all components = 0
				GradFillDef.Component2 = 
				GradFillDef.Component3 = 
				GradFillDef.Component4 = 0;
			GFComponent[ComponentIndex] = FIXED24(1.0);
			TopRightColour = DocColour(DisplayModel, &GradFillDef);

			// Make the slider use the colour definition with this component from 0 to 1
			// (e.g. a red will give a green slider which shades from red to yellow)
			memcpy(&GradFillDef, &EditingColDef, sizeof(ColourGeneric));
			GFComponent[ComponentIndex] = FIXED24(0.0);
			LeftColour = DocColour(DisplayModel, &GradFillDef);

			GFComponent[ComponentIndex] = FIXED24(1.0);
			RightColour = DocColour(DisplayModel, &GradFillDef);

			BOOL Shaded = EditingColour == NULL ||
							((EditingColour->GetType() == COLOURTYPE_TINT) ||
							EditingColour->InheritsComponent(ComponentIndex+1));

			// Render the slider
			if (SplitLineDisplay && NumComponents > 1)
			{
				// We want a split display for each line, with a gradfill showing
				// only this component (on top), and a gradfill showing the range of
				// colours you can get by changing only this component (below).
				RenderPickerSliderH(pRender, &SliderRect,
									PixelSize, ECComponent[ComponentIndex],
									&LeftColour, &RightColour,
									&TopLeftColour, &TopRightColour,
									Shaded);
			}
			else
			{
				// We want a non-split display. This redraws faster, as it only shows
				// the grad fill for this component. If there is only one colour component
				// (e.g. Greyscale) then the split display will look identical, so we
				// use the faster redraw method.
				RenderPickerSliderH(pRender, &SliderRect,
									PixelSize, ECComponent[ComponentIndex],
									NULL, NULL,
									&TopLeftColour, &TopRightColour,
									Shaded);
			}
		}

		// Move down to the next slider rectangle position
		SliderRect.hi.y = SliderRect.lo.y - SliderGap;
	}

	// Finally, return the render region context to its original state
	pRender->RestoreContext();

	// Delete the colour context if necessary
	if (bDeleteCC)
	{
		ColourContextList::GetList()->RemoveContext(&cc);			// Have finished with it
	}
}



/********************************************************************************************

>	void ColourEditDlg::RenderPickerTint(RenderRegion *pRender, DocRect *VirtualSize,
										INT32 PixelSize, DialogColourInfo *RedrawColours)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/10/95

	Inputs:		pRender - The render region to render the picker in
				VirtualSize - The rectangle defining the coordinate space to draw into
				PixelSize - The size of a pixel as mapped into the MILLIPOINT VirtualSize
				coordinate space; ensures things line up on display pixel boundaries.
				RedrawColours - A pointer to a DialogColourInfo object describing
				OS-friendly drawing colours to be used in redrawing the control.

	Purpose:	Redraws a colour picker control for the colour editor dialogue.
				This is used in the given display mode to display the colour information
				in a useful format. e.g. in HSV mode, a wheel and slider arrangement.

	Notes:		This is called to render the picker for link and shade colours
				It simply gives a single slider which goes between the parent colour and
				white/black.

	SeeAlso:	ColourEditDlg::RenderControl; ColourEditDlg::RenderPickerDefault

********************************************************************************************/

void ColourEditDlg::RenderPickerTint(RenderRegion *pRender, DocRect *VirtualSize,
										INT32 PixelSize, DialogColourInfo *RedrawColours)
{
	ERROR3IF(EditingColour->GetType() != COLOURTYPE_TINT,
				"ColourEditDlg::RenderPickerTint called on non-tint/shade colour!");

	pRender->SaveContext();		// Save the current rendering context

	// Set rendering defaults: Fill paths only, and fill background with window grey
	pRender->SetLineWidth(0);
	pRender->SetLineColour(COLOUR_TRANS);

	// Calculate the slider rectangle
	const INT32 SliderHeight = 18000;
	DocRect SliderRect(*VirtualSize);
	SliderRect.Inflate(0, -(VirtualSize->Height() - SliderHeight) / 2);

	// And the grad fill colours, and the text to be drawn
	String_64 SliderText;

	DocColour LeftColour;
//	if (EditingColour->TintIsShade())
//	{
//		LeftColour = DocColour(COLOUR_BLACK);
//		SliderText.MakeMsg(_R(IDS_SHADEOFCOLOUR), (TCHAR *) *(EditingColour->FindLinkedParent()->GetName()));
//	}
//	else
	{
		LeftColour = DocColour(255L, 255L, 255L);
		SliderText.MakeMsg(_R(IDS_TINTOFCOLOUR), (TCHAR *) *(EditingColour->FindLinkedParent()->GetName()));
	}

	DocColour RightColour;
	RightColour.MakeRefToIndexedColour(EditingColour->FindLinkedParent());

	DocRect TextRect(SliderRect);
	TextRect.Translate(0, SliderHeight);
	pRender->SetFixedSystemTextColours(&RedrawColours->TextFore(), &RedrawColours->DialogBack());
	pRender->DrawFixedSystemText((StringBase *) &SliderText, TextRect);

	FIXED24 DisplayValue = (EditingColour->TintIsShade()) ?
								EditingColour->GetShadeValueY() : EditingColour->GetTintValue();

	// And render the slider
	RenderPickerSliderH(pRender, &SliderRect,
						PixelSize, DisplayValue,
						NULL, NULL,
						&LeftColour, &RightColour,
						FALSE);

	// Finally, return the render region context to its original state
	pRender->RestoreContext();
}



/********************************************************************************************

>	void ColourEditDlg::RenderPickerShade(RenderRegion *pRender, DocRect *VirtualSize,
							INT32 PixelSize, DialogColourInfo *RedrawColours,
							DocRect *pClipRect)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/10/95

	Inputs:		pRender - The render region to render the picker in
				
				VirtualSize - The rectangle defining the coordinate space to draw into
				
				PixelSize - The size of a pixel as mapped into the MILLIPOINT VirtualSize
				coordinate space; ensures things line up on display pixel boundaries.
				
				RedrawColours - A pointer to a DialogColourInfo object describing
				OS-friendly drawing colours to be used in redrawing the control.

				pClipRect - The area to be redrawn, in (0,0)->(dx,dy) dialog millipoint coords

	Purpose:	Redraws a colour picker control for the colour editor dialogue.
				This is used in the given display mode to display the colour information
				in a useful format. e.g. in HSV mode, a wheel and slider arrangement.

	SeeAlso:	ColourEditDlg::RenderControl; ColourEditDlg::RenderPickerDefault

********************************************************************************************/

void ColourEditDlg::RenderPickerShade(RenderRegion *pRender, DocRect *VirtualSize,
									INT32 PixelSize, DialogColourInfo *RedrawColours,
									DocRect *pClipRect)
{
	pRender->SaveContext();		// Save the current rendering context

	// Set defaults: Fill paths only, and fill background with window grey
	DocColour Trans(COLOUR_TRANS);

	pRender->SetLineWidth(0);
	pRender->SetLineColour(Trans);

	// First, do a Hue slider (a rainbow grad filled rectangle along the bottom)
	DocRect HueRect;
	DocRect ValSatSquare;
	CalculateHSVPickerRects(VirtualSize, PixelSize, &HueRect, &ValSatSquare);

	// Draw the Saturation vs. Value square
	Path SquarePath;
	LinearFillAttribute ValueGradFill;
	LinearTranspFillAttribute TransFill;

	ColourHSVT ColourDef;
	BaseDocument *ScopeDocument = ParentList->GetParentDocument();
	ColourContextArray *Contexts = ScopeDocument->GetDefaultColourContexts();
	ERROR3IF(Contexts == NULL, "Document default contexts not defined?!");

	ColourContextHSVT *ccHSV = (ColourContextHSVT *)
										Contexts->Context[COLOURMODEL_HSVT];
	ERROR3IF(ccHSV == NULL, "Document default context for HSV not defined?!");

	// We need to use the PARENT's Hue as a basis for our shade display, because otherwise
	// as we change the child colour to a greyscale/black, it loses its Hue and the picker
	// flashes different colours. We also use this definition below to show the parent "cross"
	ColourHSVT ParentDef;
	ParentDef.Hue = 0;
	ParentDef.Saturation = ParentDef.Value = FIXED24(1.0);

	if (EditingColour != NULL)
	{
		ccHSV->ConvertColour(EditingColour, (ColourGeneric *) &ColourDef);

		if (EditingColour->FindLastLinkedParent() != NULL)
			ccHSV->ConvertColour(EditingColour->FindLastLinkedParent(), (ColourGeneric *) &ParentDef);
	}
	else
	{
		// No colour - default to white
		ColourDef.Hue = 0;
		ColourDef.Saturation = ColourDef.Value = FIXED24(1.0);
	}

	if (pClipRect->IsIntersectedWith(ValSatSquare))		// If we need to redraw this bit...
	{
		// Copy the colour def, and force it's Value & Sat components to 1.0
		ColourHSVT TempColourDef;
		TempColourDef.Hue	= ParentDef.Hue;
		TempColourDef.Value	= TempColourDef.Saturation = FIXED24(1.0);

		pRender->SaveContext();
		
		ValueGradFill.Colour	= DOCCOLOUR_HSVT(&TempColourDef);
		ValueGradFill.EndColour	= DocColour(255L, 255L, 255L);

		DocCoord ThePoint(ValSatSquare.lo.x, ValSatSquare.hi.y);
		ValueGradFill.SetStartPoint(&ThePoint);
		ThePoint = DocCoord(ValSatSquare.hi.x, ValSatSquare.hi.y);
		ValueGradFill.SetEndPoint(&ThePoint);
		ValueGradFill.SetEndPoint2(NULL);

		pRender->SetFillGeometry(&ValueGradFill, FALSE);			// Set Grad-filled

		SquarePath.Initialise(12, 12);
		SquarePath.FindStartOfPath();

		SquarePath.InsertMoveTo(ValSatSquare.lo);
		SquarePath.InsertLineTo(DocCoord(ValSatSquare.hi.x, ValSatSquare.lo.y));
		SquarePath.InsertLineTo(ValSatSquare.hi);
		SquarePath.InsertLineTo(DocCoord(ValSatSquare.lo.x, ValSatSquare.hi.y));
		SquarePath.IsFilled = TRUE;

		pRender->DrawPath(&SquarePath);		// Render the value square

		// Render a white linear-grad-transparency square over the top to get the
		// effect of the Saturation gradient
		DocColour black(0,0,0);
		pRender->SetFillColour(black);

		// Set transparency to circular 0% at center, 100% at radius, && plot it
		TransFill.SetStartPoint(&ValSatSquare.hi);
		ThePoint = DocCoord(ValSatSquare.hi.x, ValSatSquare.lo.y);
		TransFill.SetEndPoint(&ThePoint);
		TransFill.SetEndPoint2(NULL);
		TransFill.Transp		= 255;
		TransFill.EndTransp		= 0;
		TransFill.TranspType	= TT_StainGlass;		// 'Stained glass' transparency

		pRender->SetTranspFillGeometry(&TransFill, FALSE);

		pRender->DrawPath(&SquarePath);		// Render the saturation square

		pRender->RestoreContext();
	}

	if (EditingColour != NULL)
	{
		// Render a parent colour marker to remind the user where the parent definition is

		DocRect CrossRect = DocRect(-PCROSSRADIUS, -PCROSSRADIUS, PCROSSRADIUS, PCROSSRADIUS);
		INT32 TransX = ValSatSquare.hi.x - (INT32)
					((double)ValSatSquare.Width() * ParentDef.Saturation.MakeDouble());
		INT32 TransY = ValSatSquare.lo.y + (INT32)
					((double)ValSatSquare.Height() * ParentDef.Value.MakeDouble());
		CrossRect.Translate(HalfGridLock(TransX, PixelSize), HalfGridLock(TransY, PixelSize));

		RenderParentCross(pRender, &CrossRect, PixelSize);


		// Render marker crosses in the appropriate places for the definition
		// of the current editing colour - the shade Sat/Val crosses
		CrossRect = DocRect(-CROSSRADIUS, -CROSSRADIUS, CROSSRADIUS, CROSSRADIUS);
		TransX = ValSatSquare.hi.x - (INT32)
					((double)ValSatSquare.Width() * ColourDef.Saturation.MakeDouble());
		TransY = ValSatSquare.lo.y + (INT32)
					((double)ValSatSquare.Height() * ColourDef.Value.MakeDouble());

		CrossRect.Translate(HalfGridLock(TransX, PixelSize), HalfGridLock(TransY, PixelSize));

		RenderCross(pRender, &CrossRect, PixelSize, FALSE);
	}
	return;
}



/********************************************************************************************

>	void ColourEditDlg::RenderControl(UINT32 GadgetToRender, ReDrawInfoType* RedrawInfo)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/11/94

	Inputs:		GadgetToRender - The gadget that needs to be redrawn
				ExtraInfo - The structure that has the extra data we need to start rendering

	Purpose:	Renders the controls that are kernel-redrawn (The colour 'picker' section,
				and the two (original and new colour definitions) colour patches)

	Notes:		If EditingColour is NULL (as is the case when the window is shaded) the
				redraw is changed appropriately (pickers show values appropriate to 
				display of 'white', position marker crosses are not drawn, an colour
				patches show mid grey).

********************************************************************************************/

void ColourEditDlg::RenderControl(UINT32 GadgetToRender, ReDrawInfoType* RedrawInfo)
{
	// Use a virtual coord space of (0,0) to (dx, dy)
	DocRect VirtualSize(0, 0, RedrawInfo->dx, RedrawInfo->dy);

	// Get a render region to use. The TRUE parameter specifies that we want to use
	// the same colour correction/separation contexts as the currently selected DocView,
	// so that we display "document colour" in an appropriate manner.
	RenderRegion *pRender;
	if (GadgetToRender == _R(IDC_EDIT_PICKER))
		pRender = CreateGRenderRegion(&VirtualSize, RedrawInfo, TRUE);
	else
		pRender = CreateOSRenderRegion(&VirtualSize, RedrawInfo, TRUE);
	// I would ENSURE pRender != NULL, but it's perfectly legal if we get a 0-width
	// redraw cliprect, etc, so we basically have to ignore NULL returns.

	INT32 PixelSize = 72000 / RedrawInfo->Dpi;		// Size of output pixel in millipoints

	if (pRender != NULL)
	{
		// Interlock redraw with the drag manager to ensure we don't redraw over any drag blobs
		DragManagerOp::RedrawStarting(WindowID, GadgetToRender);

		DialogColourInfo RedrawColours;			// Object supplying Host OS redraw colours

		if (GadgetToRender == _R(IDC_EDIT_PICKER))
		{
			// Draw a narrow plinth around the edge of the control
			pRender->SaveContext();

			pRender->SetLineWidth(0);
			pRender->SetFillColour(RedrawColours.DialogBack());
			pRender->SetLineColour(RedrawColours.ButtonShadow());
			pRender->DrawRect(&VirtualSize);

			pRender->SetLineColour(RedrawColours.ButtonHighlight());
			pRender->DrawLine(VirtualSize.hi, DocCoord(VirtualSize.hi.x, VirtualSize.lo.y));
			pRender->DrawLine(DocCoord(VirtualSize.hi.x, VirtualSize.lo.y), VirtualSize.lo);

			// And deflate the rect by 2 pixels
			VirtualSize.Inflate(-PixelSize * 4);

			// Now draw the original/current colour patch in the top right corner
			DocRect PatchRect(VirtualSize);
			PatchRect.lo.x = PatchRect.hi.x - PATCHSIZE;
			PatchRect.lo.y = PatchRect.hi.y - PATCHSIZE;
			GridLockRect(&PatchRect, PixelSize);

			DocColour Orig;
			if (EditingColour == NULL)
				Orig = DocColour(128, 128, 128);
			else
				Orig.MakeRefToIndexedColour(&OriginalColour);

			DocColour PatchCol;
			if (EditingColour == NULL)
				PatchCol = DocColour(128, 128, 128);
			else
				PatchCol.MakeRefToIndexedColour(EditingColour);

			// Patches are horizontal if the colour model is not HSV
			BOOL HorzPatch = (DisplayModel != COLOURMODEL_HSVT);
			
			// But this setting is overridden for the special tint and shade modes
			if (EditingColour != NULL && EditingColour->GetType() == COLOURTYPE_TINT)
			{
				if (EditingColour->TintIsShade())
					HorzPatch = FALSE;
				else
					HorzPatch = TRUE;
			}

			pRender->SetLineWidth(0);
			pRender->SetLineColour(COLOUR_BLACK);
			if (HorzPatch)
				pRender->SetFillColour(PatchCol);
			else
				pRender->SetFillColour(Orig);
			pRender->DrawRect(&PatchRect);

			if (HorzPatch)
			{
				// Move the patch to the left for the second square
				PatchRect.Translate(-PATCHSIZE, 0);
				pRender->SetFillColour(Orig);
			}
			else
			{
				// Move the patch down for the second square
				PatchRect.Translate(0, -PATCHSIZE);
				pRender->SetFillColour(PatchCol);
			}

			pRender->DrawRect(&PatchRect);
/*
//			DocColour Trans(COLOUR_TRANS);
//			pRender->SetLineColour(Trans);
			pRender->SetFillColour(Orig);
			Path TriPath;
			TriPath.Initialise(12, 12);
			TriPath.FindStartOfPath();
			TriPath.InsertMoveTo(PatchRect.lo);
			TriPath.InsertLineTo(DocCoord(PatchRect.lo.x, PatchRect.hi.y));
			TriPath.InsertLineTo(PatchRect.hi);
			TriPath.IsFilled = TRUE;
			pRender->DrawPath(&TriPath);		// Render the value square
*/
			pRender->RestoreContext();
		

			if (EditingColour != NULL && EditingColour->GetType() == COLOURTYPE_TINT)	// Tint or shade
			{
				if (EditingColour->TintIsShade())
				{
					RenderPickerShade(pRender, &VirtualSize, PixelSize, &RedrawColours,
											RedrawInfo->pClipRect);
				}
				else
					RenderPickerTint(pRender, &VirtualSize, PixelSize, &RedrawColours);
			}
			else
			{
				switch (DisplayModel)
				{
					case COLOURMODEL_RGBT:
					case COLOURMODEL_WEBRGBT:
						if (Use3DDisplay)
							RenderPickerRGB(pRender, &VirtualSize, PixelSize, &RedrawColours);
						else
							RenderPickerDefault(pRender, &VirtualSize, PixelSize, &RedrawColours);
						break;
	
					case COLOURMODEL_CMYK:
						if (Use3DDisplay)
							RenderPickerCMYK(pRender, &VirtualSize, PixelSize, &RedrawColours);
						else
							RenderPickerDefault(pRender, &VirtualSize, PixelSize, &RedrawColours);
						break;
	
					case COLOURMODEL_HSVT:
						RenderPickerHSV(pRender, &VirtualSize, PixelSize, &RedrawColours,
											RedrawInfo->pClipRect);
						break;
	
					default:
						RenderPickerDefault(pRender, &VirtualSize, PixelSize, &RedrawColours);
						break;
				}
			}
		}
#if 0
		else if (GadgetToRender == _R(IDC_EDIT_PATCH))						// Old/New/Parent-colour patch
		{
			pRender->SaveContext();

			DocColour Trans(COLOUR_TRANS);
			DocColour Orig;
			if (EditingColour == NULL)
				Orig = DocColour(128, 128, 128);
			else
				Orig.MakeRefToIndexedColour(&OriginalColour);

			pRender->SetLineWidth(0);
			pRender->SetLineColour(Trans);

			DocColour PatchCol;
			if (EditingColour == NULL)
				PatchCol = DocColour(128, 128, 128);
			else
				PatchCol.MakeRefToIndexedColour(EditingColour);

			pRender->SetFillColour(PatchCol);
			pRender->DrawRect(&VirtualSize);

			pRender->SetFillColour(Orig);
			Path TriPath;
			TriPath.Initialise(12, 12);
			TriPath.FindStartOfPath();
			TriPath.InsertMoveTo(VirtualSize.lo);
			TriPath.InsertLineTo(DocCoord(VirtualSize.lo.x, VirtualSize.hi.y));
			TriPath.InsertLineTo(DocCoord(VirtualSize.hi.x, VirtualSize.lo.y));
			TriPath.IsFilled = TRUE;
			pRender->DrawPath(&TriPath);		// Render the value square


#if 0
			INT32 PatchHeight = VirtualSize.Height() / 3;

			DocRect TheRect(VirtualSize);
			TheRect.lo.y = TheRect.hi.y - PatchHeight;
			pRender->DrawRect(&TheRect);

			DocColour PatchCol;
			if (EditingColour == NULL)
				PatchCol = DocColour(128, 128, 128);
			else
				PatchCol.MakeRefToIndexedColour(EditingColour);

			pRender->SetFillColour(PatchCol);
			TheRect.Translate(0, -PatchHeight);	// Move the patch rect down and plot
			pRender->DrawRect(&TheRect);

			if (EditingColour != NULL && EditingColour->FindLinkedParent() != NULL)
				PatchCol.MakeRefToIndexedColour(EditingColour->FindLinkedParent());

			pRender->SetFillColour(PatchCol);
			TheRect.Translate(0, -PatchHeight);	// Move the patch rect down and plot
			pRender->DrawRect(&TheRect);
#endif

			pRender->RestoreContext();
		}
#endif
		else
		{
			ERROR3("Render request for unsupported kernel-rendered control!");
		}


		// Get rid of the render region
		if (GadgetToRender == _R(IDC_EDIT_PICKER))
			DestroyGRenderRegion(pRender);
		else
			DestroyOSRenderRegion(pRender);

		// And turn off the drag redraw interlock
		DragManagerOp::RedrawFinished();
	}
}



/********************************************************************************************

>	inline BOOL CanSetColour(IndexedColour *EditingColour)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/11/94
	Inputs:		EditingColour - the colour to check
	Returns:	TRUE if you may chnage the colour, FALSE if you may not

	Purpose:	Called by the SetNewValue methods to determine if they are allowed to
				set a new value for the EditingColour. This is so they may have display-only
				click/drag handling when the colour is non-editable (for example, when it's
				a tint). This method just makes it easier for us to modify the allow-edit
				rulesat a later date.

	Scope:		private (ColourEditDlg, kernel\coldlog.cpp)

********************************************************************************************/

inline BOOL CanSetColour(IndexedColour *EditingColour)
{
	return(EditingColour != NULL && EditingColour->GetType() != COLOURTYPE_TINT);
}



/********************************************************************************************

>	void ColourEditDlg::StartDrag(ReDrawInfoType *Info)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/11/94
	Inputs:		Info - The mouse-position information from the control
	Purpose:	Starts a drag from the colour picker control in the colour editor window

	Notes:		May be called when EditingColour is NULL (the window is shaded)

********************************************************************************************/

//DocCoord OldMousePos;

void ColourEditDlg::StartDrag(ReDrawInfoType *Info)
{
	if (DragStartArea != CEDRAG_NONE)	// We are already dragging! Ignore it
		return;

	// Lob away the input focus again - put it back into the mainframe
	LoseFocusFromEditControls = TRUE;
	LoseKeyboardFocus();
	LoseFocusFromEditControls = FALSE;

	// Check if the drag is starting from a colour patch
	if (ParentList != NULL && EditingColour != NULL && ResultColour != NULL && !AmShaded)
	{	
		//OldMousePos = *(Info->pMousePos);
		//HideGadget (_R(IDC_COLOURPICKER), TRUE);
		//CWindowID hwndColPick = DialogManager::GetGadget (GetReadWriteWindowID (), _R(IDC_COLOURPICKER));
		//ASSERT (hwndColPick);
				
		//ShowWindow (hwndColPick, SW_HIDE);
		
		INT32 PixelSize = 72000 / Info->Dpi;			// Size of output pixel in millipoints

		DocRect VirtualSize(0, 0, Info->dx, Info->dy);
		VirtualSize.Inflate(-PixelSize * 4);

		// Now draw the original/current colour patch in the top right corner
		DocRect PatchRect(VirtualSize);
		PatchRect.lo.x = PatchRect.hi.x - PATCHSIZE;
		PatchRect.lo.y = PatchRect.hi.y - PATCHSIZE;
		GridLockRect(&PatchRect, PixelSize);

		BOOL HorzPatch = (DisplayModel != COLOURMODEL_HSVT);

		// But this setting is overridden for the special tint and shade modes
		if (EditingColour != NULL && EditingColour->GetType() == COLOURTYPE_TINT)
		{
			if (EditingColour->TintIsShade())
				HorzPatch = FALSE;
			else
				HorzPatch = TRUE;
		}

		INT32 InPatch = 0;
		if (PatchRect.ContainsCoord(*(Info->pMousePos)))
			InPatch = 1;
		else
		{
			if (HorzPatch)
				PatchRect.Translate(-PATCHSIZE, 0);
			else
				PatchRect.Translate(0, -PATCHSIZE);

			if (PatchRect.ContainsCoord(*(Info->pMousePos)))
				InPatch = 2;
		}

		if (InPatch != 0)
		{
			// Invert the logic of Horz in 1 case, so it is FALSE (original colour) or TRUE (current colour)
			if (InPatch == 1)
				HorzPatch = !HorzPatch;

			// It was in one of the 2 patches
			IndexedColour *TheColour = ResultColour;			// Default to current colour
			if (HorzPatch)
			{
				TheColour = new IndexedColour(OriginalColour);	// They are dragging original colour
				if (TheColour != NULL)
				{
					TheColour->SetUnnamed();

					// Ensure we don't get a memory leak
					ParentList->AddItem(TheColour);
				}
			}

			if (TheColour != NULL)								// If have colour to drag, drag it
			{
				ColEditorDragInfo *DragCol;
				DragCol = new ColEditorDragInfo(TheColour, FALSE, (Document *)ParentList->GetParentDocument());
				DragManagerOp::StartDrag(DragCol, GetReadWriteWindowID());
			}

			return;												// Don't pass the call on to the picker
		}
	}

	// Pass the click/drag onto the appropriate colour picker, and start a picker drag
	DragUpdatedOnIdle = FALSE;
	if (EditingColour != NULL && !AmShaded)
	{
//		SetColour(FALSE);			// Ensure name updated (the name control doesn't lose
									// kbd focus in this case, so we must force it)
		SetNewValueFromMousePos(Info, TRUE);

		// If we started dragging, then register for idle-event updates
		// (first, ensuring that we are not already on the list)

		EndTimedProcessing();
		if (DragStartArea != CEDRAG_NONE)
			BeginTimedProcessing();
	}
}



/********************************************************************************************

>	void ColourEditDlg::UpdateDrag(ReDrawInfoType *Info)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/11/94
	Inputs:		Info - The mouse-position information from the control
	Purpose:	Updates during a drag from the colour picker control in the colour
				editor window

	Notes:		May be called when EditingColour is NULL (the window is shaded)

********************************************************************************************/

void ColourEditDlg::UpdateDrag(ReDrawInfoType *Info)
{
	DragUpdatedOnIdle = FALSE;
	if (DragStartArea != CEDRAG_NONE && EditingColour != NULL && !AmShaded)
	{
		SetNewValueFromMousePos(Info, FALSE);
	}
}


BOOL ColourEditDlg::TimedProcessing(void)
{
	return (UpdateColourEditor ());
}


/********************************************************************************************

>	virtual BOOL ColourEditDlg::UpdateColourEditor ()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/12/94
	Inputs:		-
	Returns:	

	Purpose:	This function was originally was a override of the virtual OnIdle() 
				function and was called whist the process had idle time to spare, which
				slowed down every other process on the computer. This function was modified
				and renamed so that it is now called by timer messages at fixed periods.
				
				Used during drags to detect when the mouse has stopped moving (we won't
				get idle events unless we have finished updating displays, etc)
				When we find idle time, we start a complete update of the colour window
				and the document, rather than the normal during-drag minimal updates.

	Notes:		May be called when EditingColour is NULL (the window is shaded).
				See also: ColourEditDlg::BeginTimedProcessing, ColourEditDlg::EndTimedProcessing

********************************************************************************************/

BOOL ColourEditDlg::UpdateColourEditor ()//TimedProcessing(void)
{

	// Ensure the focus is shunted back to the document (during a drag, and also after
	// the window has been moved)
//	if (LoseKbdFocusPending)
//		LoseKeyboardFocus();

	// If something has changed while the program was busy, we will have flagged the fact
	// that we should update as soon as the busy job is finished and we start getting idles
	// once again. We find a safe colour to edit. (This usually happens when we load or swap
	// docs. This mechanism allows many changes to occur during loading while only triggering
	// one final colour editor redraw)
	if (UpdateOnNextIdle)
	{
		UpdateOnNextIdle = FALSE;

		FindUsefulColourToEdit(EditingLineColour);	// Try to find a new colour to edit
		
		if (EditingColour == NULL)					// We failed - shade the dialogue
			ShadeMyself();
		
		return(FALSE);
	}

	// If we're not dragging, then update bubble help and stuff, and return
	if (DragStartArea == CEDRAG_NONE)
	{
		ColourPicker::UpdateBubbleHelpAndPointer();
		return(FALSE);		// Let low-priority idle handlers get a look in
	}

	// Otherwise we are dragging...

	// Check for escape being pressed
	if (AbortColour != NULL)
	{
		// Call SetNewValueFromMousePos to handle escape being pressed
		SetNewValueFromMousePos(NULL, FALSE);

		if (DragStartArea == CEDRAG_NONE)	// Has the drag now been aborted?
			return(FALSE);					// Yup, so exit
	}

	// If we've already updated on an idle and the mouse has not yet moved again, then
	// we won't bother with unnecessary redraws and suchlike.
	if (DragUpdatedOnIdle)
		return(FALSE);		// Let low-priority idle handlers get a look in

	if (DragStartArea != CEDRAG_NONE && EditingColour != NULL)
	{
		// Flag that we have updated on an idle event - we won't respond to idles
		// again until another drag update occurs (as until a drag update occurs, there
		// can be no change in the colour)
		DragUpdatedOnIdle = TRUE;

		// Force complete update of the window and document, etc
		EditingColourHasChanged(TRUE, TRUE, TRUE);

		// The above call explicitly does not redraw the picker - it should be up to
		// date anyway, so why bother? However, the parent patch does need redrawing.
		InvalidatePatchGadget();

		return(TRUE);	// Don't waste any time on low-priority idle handlers this time around
	}

	// Deregister for idles, because we should not be getting them now
//	GetApplication()->RemoveIdleProcessor(IDLEPRIORITY_HIGH, this);		// Leave idles on for bubble help

	return(FALSE);		// And let the low-priority idle handlers do their stuff
}



/********************************************************************************************

>	OpState ColourEditDlg::GetCommandState(StringBase *Command, StringBase *ShadeReason)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/10/95

	Inputs:		Command - String indicating the command to apply (see colmenu.h)

	Outputs:	ShadeReason - if OpState.Greyed is returned TRUE, this describes why
				the op is greyed.

	Returns:	An OpState indicating the state of this command (shaded etc)

	Purpose:	Upcall from the menu system to determine te state of a menu command

********************************************************************************************/

OpState ColourEditDlg::GetCommandState(StringBase *Command, StringBase *ShadeReason)
{
	OpState State;

	if (*Command == ColCmd_Name)								// Name only available for named colour
	{
		if (ResultColour == NULL || ParentList == NULL || !ResultColour->IsNamed())
		{
			State.Greyed = TRUE;
			ShadeReason->MakeMsg(_R(IDS_K_COLDLOG_NONAMELOCCOLS));
		}
	}
	else if (*Command == ColCmd_EditParent)						// Edit parent only available if have parent
	{
		if (ResultColour == NULL || ParentList == NULL || ResultColour->FindLinkedParent() == NULL)
		{
			State.Greyed = TRUE;
			ShadeReason->MakeMsg(_R(IDS_COLMENU_NOPARENT));
		}
	}
	else if (*Command == ColCmd_HSV)							// Tick appropriate colour model
		State.Ticked = (DisplayModel == COLOURMODEL_HSVT);
	else if (*Command == ColCmd_RGB)
		State.Ticked = (DisplayModel == COLOURMODEL_RGBT);
	else if (*Command == ColCmd_CMYK)
		State.Ticked = (DisplayModel == COLOURMODEL_CMYK);
	else if (*Command == ColCmd_Grey)
		State.Ticked = (DisplayModel == COLOURMODEL_GREYT);

	return(State);
}



/********************************************************************************************

>	void ColourEditDlg::DoCommand(StringBase *Command)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/10/95

	Inputs:		Command - String indicating the command to apply (see colmenu.h)

	Purpose:	Applies a given editor menu command

********************************************************************************************/

void ColourEditDlg::DoCommand(StringBase *Command)
{
	ColourModel NewColourModel = COLOURMODEL_INDEXED;

	if (*Command == ColCmd_Help)
		HelpUser(*this);
	else if (*Command == ColCmd_Name)
	{
		if (ResultColour != NULL && ParentList != NULL && ResultColour->IsNamed())
		{
			// Lock ourselves so we don't update on any message broadcasts
			BOOL ISentState = ISentTheMessage;
			ISentTheMessage = TRUE;

// Nasty, but ColourNameDlg is in sgcolour
#ifndef EXCLUDE_GALS
				// Rename the result colour
				if (ColourNameDlg::InvokeDialog(ParentList, ResultColour))
				{
					// Commit any colour model change to this colour now
//					ColourContext *cc = ColourContext::GetGlobalDefault(DisplayModel);
					ColourContext *cc = NULL;
					BOOL bDeleteCC = GetColourContext(DisplayModel, &cc);	
					if (cc != NULL)
					{
						ColourPicker::ForceColourModel(ResultColour, cc);

						// Delete the colour context if necessary
						if (bDeleteCC)
							ColourContextList::GetList()->RemoveContext(&cc);			// Have finished with it
					}

					// Make sure the editor swaps (it'll ignore us if ResultColour == ColToEdit)
					IndexedColour *ColToEdit = ResultColour;
					ResultColour = NULL;

					// And start the editor up again on the original colour
					EditThisColour(ParentList, ColToEdit);
				}
#endif
			ISentTheMessage = ISentState;
		}
	}
	else if (*Command == ColCmd_EditParent)						// Edit parent only available if have parent
	{
		if (ResultColour != NULL && ParentList != NULL && ResultColour->FindLinkedParent() != NULL)
			EditThisColour(ParentList, ResultColour->FindLinkedParent());
	}
	else if (*Command == ColCmd_HSV)
		NewColourModel = COLOURMODEL_HSVT;
	else if (*Command == ColCmd_RGB)
		NewColourModel = COLOURMODEL_RGBT;
	else if (*Command == ColCmd_CMYK)
		NewColourModel = COLOURMODEL_CMYK;
	else if (*Command == ColCmd_Grey)
		NewColourModel = COLOURMODEL_GREYT;
#ifdef WEBSTER // Martin 16/07/97
	else if (*Command == ColCmd_NewNColour)
		MakeNewNColour(); //do the same thing as pressing the button!
#endif //WEBSTER

	if (NewColourModel != COLOURMODEL_INDEXED && NewColourModel != DisplayModel)
	{
		DisplayModel = (ColourModel) NewColourModel;		// Switch display to the new model
		
// WEBSTER - markn 31/1/97
// Always set the default colour models to the display mode
#ifndef WEBSTER
		if (EditingColour->IsNamed())
			DefaultDisplayModelN = NewColourModel;			// And remember preference
#else
		DefaultDisplayModelN = NewColourModel;
		DefaultDisplayModel = NewColourModel;
#endif // WEBSTER

		ColourEditDlg::SetUnitGroupDefaults(DisplayModel);

		InvalidateAndSetControls();							// And update the displays
	}	
}



/********************************************************************************************

>	static BOOL ColourEditDlg::OnKeyPress(KeyPress* pKeyPress)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/2/95
	Inputs:		pKeyPress - points to a keypress object describing the key press

	Returns:	TRUE if it has processed the keypress (claimed it)
				FALSE if it has not used the keypress, in which case it may be passed on

	Purpose:	To handle keypresses. This is called by Application::OnKeyPress before
				all other keypress handling. Normally the colour editor will just return
				FALSE as fast as possible, but during drags in the editor, it will 
				return TRUE to stop keypresses getting through to the rest of camelot.

	Notes:		Not intended to be called by anyone other than Application::OnKeypress

				May be called at any time

	SeeAlso:	Application::OnKeyPress; ColourEditDlg::StartDrag

********************************************************************************************/

BOOL ColourEditDlg::OnKeyPress(KeyPress* pKeyPress)
{
	ERROR3IF(pKeyPress == NULL, "Illegal NULL param!");

	if (TheEditor == NULL || TheEditor->DragStartArea == CEDRAG_NONE)
		return(FALSE);		// No current drag, so leave the key alone

	// We are dragging...
	
	// Check for escape being pressed
	if ((TheEditor->AbortColour != NULL) && ((pKeyPress->GetVirtKey () == CAMKEY(ESCAPE)) || (pKeyPress->GetVirtKey () == CAMKEY(CANCEL))))
	{
		EscapeKeyWasPressed = TRUE;
		TheEditor->SetNewValueFromMousePos(NULL, FALSE);
	}

	// Sit on all keypresses to make sure nobody else gets them
	return(TRUE);
}




/********************************************************************************************

>	void ColourEditDlg::EndDrag(ReDrawInfoType *Info)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/11/94
	Inputs:		Info - The mouse-position information from the control
	Purpose:	Ends a drag from the colour picker control in the colour editor window

	Notes:		May be called when EditingColour is NULL (the window is shaded)

********************************************************************************************/

void ColourEditDlg::EndDrag(ReDrawInfoType *Info)
{
	// Only update if we think we're dragging (as for the other Drag methods)
	// However, if the last update was on an idle event, then the mouse has not moved
	// and so the colour has not changed since we last broadcast, so there is no
	// need to broadcast again, with all the flickery redraw etc. that will cause
	if (DragStartArea != CEDRAG_NONE && EditingColour != NULL && !DragUpdatedOnIdle)
	{
//		EndTimedProcessing ();
		
		SetNewValueFromMousePos(Info, FALSE);		// Update colour one last time

		if (CanSetColour(EditingColour) || EditingColour->GetType() == COLOURTYPE_TINT)
			EditingColourHasChanged(TRUE, TRUE, TRUE);	// Ensure window etc fully updated
	}

	DragUpdatedOnIdle = FALSE;
//	GetApplication()->RemoveIdleProcessor(IDLEPRIORITY_HIGH, this);		// Leave idles on for bubblehelp
	DragStartArea = CEDRAG_NONE;					// We aren't dragging any more

	if (AbortColour != NULL)
	{
		delete AbortColour;
		AbortColour= NULL;
	}
}



/********************************************************************************************

>	void ColourEditDlg::SetNewValueFromMousePos(ReDrawInfoType *Info, BOOL StartingNewDrag)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/11/94
	Inputs:		Info -	The redraw info for the kernel-drawn control's mouse event
						
						This may be NULL if you merely want to check for escape being
						pressed to abort the drag, but note that if this is the case,
						all that will be done is the escape check. NOTE that StartingNewDrag
						MUST also be FALSE in this case.

				StartingNewDrag - TRUE if this is the first call to this function for a
				new drag operation (in which case, the DragStartArea is set appropriately)

	Purpose:	Handles drag updates - determines the new colour values from the current
				mouse position, and sets the colour as appropriate.

				Will also check for escape being pressed, in which case the drag will
				be aborted, and the colour will return to its pre-drag state.

	Notes:		This will generate an ERROR3 if called with EditingColour==NULL

				For those who haven't followed the progress of the colour editor,
				you will soon realise that it is one huge non-OO bodge, which grew
				from a tiny little thing in fits and starts and design retrofits.
				One day, I'll be allowed to write it properly, from scratch, instead
				of just bolting on more and more bits of ticky tacky.

********************************************************************************************/

void ColourEditDlg::SetNewValueFromMousePos(ReDrawInfoType *Info, BOOL StartingNewDrag)
{
	if (EditingColour == NULL)		// We are shaded - spurious call - shouldn't happen
	{
		ERROR3("ColourEditDlg::SetNewValueFromMousePos called when EditingColour == NULL");
		return;
	}

	if (StartingNewDrag)
	{
		DragStartArea = CEDRAG_NONE;		// For now, we are not dragging

		// And copy the original colour in case of an abort
		// NOTE: If new fails, then this pointer will be NULL, which merely disables abort checks.
		if (AbortColour != NULL)
		{
			delete AbortColour;
			AbortColour = NULL;
		}
		AbortColour = new IndexedColour(*EditingColour);
	}
	else
	{
		// Continuing a drag we've already started - check for escape being pressed
		if ((AbortColour != NULL && KeyPress::IsEscapePressed()) || (AbortColour != NULL && EscapeKeyWasPressed))
		{
			// Beep to make user aware they've succeeded in aborting the drag
			Beep();

			// Reset the definition of the EditingColour
			*EditingColour = *AbortColour;

			// And reset the drag state variables
			delete AbortColour;
			AbortColour = NULL;

			DragUpdatedOnIdle = FALSE;
			DragStartArea = CEDRAG_NONE;

			// Inform the editor of the change, and redraw everything (document and editor)
			EditingColourHasChanged(TRUE, TRUE, TRUE);
			InvalidatePatchGadget();
//			EndTimedProcessing ();
			EscapeKeyWasPressed = FALSE;
			return;
		}
	}

	// If Info was NULL, the caller only wanted to check for the abort (escape) key being pressed
	if (Info == NULL)
		return;

	// Finally, call the appropriate colour model update routine to get the job done...
	// Yeah, yeah, I know. Where's the objects? Where's the virtual functions?

	INT32 PixelSize = 72000 / Info->Dpi;		// Size of output pixel in millipoints

	if (EditingColour->GetType() == COLOURTYPE_TINT)
	{
		if (EditingColour->TintIsShade())
			SetNewValueShade(Info, PixelSize, StartingNewDrag);
		else
			SetNewValueTint(Info, PixelSize, StartingNewDrag);
	}
	else
	{
		switch (DisplayModel)
		{
			case COLOURMODEL_HSVT:
				if (CanSetColour(EditingColour))
					SetNewValueHSV(Info, PixelSize, StartingNewDrag);
				break;

			case COLOURMODEL_RGBT:
			case COLOURMODEL_WEBRGBT:
				// RGB has the ability to change display modes, even when the colour is uneditable
				// so we call the handler even when CanSetColour() == FALSE
				if (Use3DDisplay)
					SetNewValueRGB(Info, PixelSize, StartingNewDrag);
				else
				{
					if (CanSetColour(EditingColour))
						SetNewValueDefault(Info, PixelSize, StartingNewDrag);
				}
				break;

			case COLOURMODEL_CMYK:
				// CMYK has the ability to change display modes, even when the colour is uneditable
				// so we call the handler even when CanSetColour() == FALSE
				if (Use3DDisplay)
					SetNewValueCMYK(Info, PixelSize, StartingNewDrag);
				else
				{
					if (CanSetColour(EditingColour))
						SetNewValueDefault(Info, PixelSize, StartingNewDrag);
				}
				break;

			default:
				if (CanSetColour(EditingColour))
					SetNewValueDefault(Info, PixelSize, StartingNewDrag);
				break;
		}
	}
}



/********************************************************************************************

>	static FIXED24 GetComponent(ColourGeneric *SourceDef, const INT32 ComponentID)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/12/94
	Inputs:		SourceDef - A Generic colour definition to extract a component from
				ComponentID - The component to extract [1..4]

	Purpose:	Extracts the given component from a colour definition.

	Scope:		private (static in kernel\coldlog.cpp)

********************************************************************************************/

static FIXED24 GetComponent(ColourGeneric *SourceDef, const INT32 ComponentID)
{
	switch(ComponentID)
	{
		case 1:
			return(SourceDef->Component1);

		case 2:
			return(SourceDef->Component2);

		case 3:
			return(SourceDef->Component3);
	}

	// Assume any other componentID value is for component 4
	return(SourceDef->Component4);
}



/********************************************************************************************

>	static BOOL MousePosInCross(DocCoord *MousePos, DocRect *Area, FIXED24 *XPos, FIXED24 *YPos)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/10/95
	Inputs:		MousePos - Points to the mouse position

				Area - Points to the picker area in which the cross resides

				XPos - A value between 0.0 and 1.0 indicating the center position of the
				cross in the X direction across the given area.
				If NULL, the value 0.5 will be used

				YPos - A value between 0.0 and 1.0 indicating the center position of the
				cross in the Y direction up the given area.
				If NULL, the value 0.5 will be used

	Purpose:	To check if a click near a given area has "hit" a marker cross sticking
				out of a side of the area.

********************************************************************************************/

static BOOL MousePosInCross(DocCoord *MousePos, DocRect *Area, FIXED24 *XPos, FIXED24 *YPos)
{
	FIXED24 HalfWay(0.5);

	if (XPos == NULL)	XPos = &HalfWay;
	if (YPos == NULL)	YPos = &HalfWay;

	DocRect CrossRect(-CROSSRADIUS, -CROSSRADIUS, CROSSRADIUS, CROSSRADIUS);

	INT32 TransX = Area->lo.x + (INT32) ((double)Area->Width() * XPos->MakeDouble());
	INT32 TransY = Area->lo.y + (INT32) ((double)Area->Height() * YPos->MakeDouble());

	CrossRect.Translate(TransX, TransY);

	return(CrossRect.ContainsCoord(*MousePos));
}



/********************************************************************************************

>	void ColourEditDlg::SetNewValueHSV(ReDrawInfoType *Info, INT32 PixelSize,
										BOOL StartingNewDrag)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/94
	Inputs:		Info - The redraw info for the kernel-drawn control's mouse event
				PixelSize - The size of display pixels as they appear when mapped into
				the MILLIPOINT VirtualRect coordinate space (as used in the RenderControl
				methods)
				StartingNewDrag - TRUE if this is the first call to this function for a
				new drag operation (in which case, the DragStartArea is set appropriately)

	Purpose:	Handles drag updates - determines the new colour values from the current
				mouse position, and sets the colour as appropriate. Just how it interprets
				the mouse position is dependant upon which region the drag was started
				within, and which DisplayMode is in operation. A different routine like
				this one will be called for each DisplayMode.

	SeeAlso:	ColourEditDlg::SetNewValueFromMousePos; ColourEditDlg::SetNewValueDefault

********************************************************************************************/

void ColourEditDlg::SetNewValueHSV(ReDrawInfoType *Info, INT32 PixelSize, BOOL StartingNewDrag)
{
#if TRUE
	DocRect VirtualSize(0, 0, Info->dx, Info->dy);
	VirtualSize.Inflate(-PixelSize * 4);		// And exclude the border

	// Calculate the two important rectangles - the hue slider, and Val/Sat square
	DocRect HueRect;
	DocRect ValSatSquare;
	CalculateHSVPickerRects(&VirtualSize, PixelSize, &HueRect, &ValSatSquare);

	// Get a colour context for our conversions and stuff
	ColourContext *cc = ColourContext::GetGlobalDefault(COLOURMODEL_HSVT);

	// Remember the old cross-position values from EditingColour
	ColourHSVT ColourDef;
	cc->ConvertColour(EditingColour, (ColourGeneric *) &ColourDef);

	// If starting a new drag, determine which area the drag is in...
	if (StartingNewDrag)
	{
		if (HueRect.ContainsCoord(*(Info->pMousePos)))
			DragStartArea = CEDRAG_HSVPICKER_HSLIDER;
		else if (ValSatSquare.ContainsCoord(*(Info->pMousePos)))
		{
			DragStartArea = CEDRAG_HSVPICKER_VSSQUARE;
		}
		else
		{
			// The user didn't hit inside a picker area, but have they tried to drag
			// a region outside that lies under a protuding marker cross?
			FIXED24 ReverseSat = FIXED24(1.0) - ColourDef.Saturation;

			if (MousePosInCross(Info->pMousePos, &ValSatSquare, &ReverseSat, &ColourDef.Value))
				DragStartArea = CEDRAG_HSVPICKER_VSSQUARE;
			else if (MousePosInCross(Info->pMousePos, &HueRect, &ColourDef.Hue, NULL))
				DragStartArea = CEDRAG_HSVPICKER_HSLIDER;
			else
				return;		// Nope - just ignore the click
		}

		// Remember what the colour was as we started the drag
		cc->ConvertColour(ResultColour, &ColourBeforeDrag);
	}


	BOOL ColourHasChanged = FALSE;
	INT32 XPos=0;
	INT32 YPos=0;

	// Handle the new mouse position, using the area the drag started in (rather than
	// the area it may now be over) to determine which components to alter
	switch(DragStartArea)
	{
		case CEDRAG_HSVPICKER_HSLIDER:
		{
			XPos = Info->pMousePos->x - HueRect.lo.x;
			if (XPos < 0)				XPos = 0;
			if (XPos > HueRect.Width())	XPos = HueRect.Width();

			double NewValue = ((double) XPos) /	((double) HueRect.Width());
			ColourPicker::SetComponentFromDouble(EditingColour, cc, 1, NewValue);

			DisplayModel = COLOURMODEL_HSVT;	// Force display back to HSV model

			if (XPos != LastDragPos.x)
				ColourHasChanged = TRUE;
			break;
		}


		case CEDRAG_HSVPICKER_VSSQUARE:
		{
			INT32 Size = ValSatSquare.Width();

			XPos = ValSatSquare.hi.x - Info->pMousePos->x;
			if (XPos < 0)		XPos = 0;
			if (XPos > Size)	XPos = Size;

			if (bHSVHueAtTop)
			{
				YPos = Info->pMousePos->y - ValSatSquare.lo.y;
				if (YPos < 0)		YPos = 0;
				if (YPos > Size)	YPos = Size;
			}
			else
			{
				YPos = ValSatSquare.hi.y - Info->pMousePos->y;
				if (YPos < 0)		YPos = 0;
				if (YPos > Size)	YPos = Size;
			}

			double NewSat = ((double)XPos) / ((double) Size);
			double NewVal = ((double)YPos) / ((double) Size);

			if (KeyPress::IsConstrainPressed())
			{
				// If we should constrain this value, then we check the original
				// values for Sat/Val, and we only change the one which is furthest
				// away from the original value (i.e. if you drag near the old
				// Saturation value, only the Value will change & vice versa)
				ColourHSVT *ConstrainDef = (ColourHSVT *) &ColourBeforeDrag;

				double TempSat = ConstrainDef->Saturation.MakeDouble() - NewSat;
				if (TempSat < 0.0) TempSat = -TempSat;

				double TempVal = ConstrainDef->Value.MakeDouble() - NewVal;
				if (TempVal < 0.0) TempVal = -TempVal;

				if (TempSat < TempVal)
					NewSat = ConstrainDef->Saturation.MakeDouble();
				else
					NewVal = ConstrainDef->Value.MakeDouble();
			}

			DisplayModel = COLOURMODEL_HSVT;	// Force display back to HSV model

			ColourPicker::SetComponentFromDouble(EditingColour, cc, 2, NewSat);
			ColourPicker::SetComponentFromDouble(EditingColour, cc, 3, NewVal);

			if (StartingNewDrag)
				ColourHasChanged = TRUE;	// Always do full redraw when drag starts
			else
			{
				// --- Invalidate the old cross position
				DocRect CrossRect(-CROSSRADIUS, -CROSSRADIUS, CROSSRADIUS, CROSSRADIUS);

				INT32 TransX = ValSatSquare.hi.x - (INT32)
							((double)ValSatSquare.Width() * ColourDef.Saturation.MakeDouble());
				INT32 TransY = ValSatSquare.lo.y + (INT32)
							((double)ValSatSquare.Height() * ColourDef.Value.MakeDouble());

				CrossRect.Translate(HalfGridLock(TransX, PixelSize), HalfGridLock(TransY, PixelSize));

				// Inflate by 2 pixels (the cross can actually draw marginally outside the crossrect ;-(
				CrossRect.Inflate(4*PixelSize);
				InvalidateGadget(_R(IDC_EDIT_PICKER), Info, &CrossRect);


				// --- And invalidate the new cross position
				CrossRect = DocRect(-CROSSRADIUS, -CROSSRADIUS, CROSSRADIUS, CROSSRADIUS);

				cc->ConvertColour(EditingColour, (ColourGeneric *) &ColourDef);
				TransX = ValSatSquare.hi.x - (INT32)
							((double)ValSatSquare.Width() * ColourDef.Saturation.MakeDouble());
				TransY = ValSatSquare.lo.y + (INT32)
							((double)ValSatSquare.Height() * ColourDef.Value.MakeDouble());

				CrossRect.Translate(HalfGridLock(TransX, PixelSize), HalfGridLock(TransY, PixelSize));

				// Inflate by 2 pixels (the cross can actually draw marginally outside the crossrect ;-(
				CrossRect.Inflate(4*PixelSize);
				InvalidateGadget(_R(IDC_EDIT_PICKER), Info, &CrossRect);

				PaintGadgetNow(_R(IDC_EDIT_PICKER));
	
				InvalidatePatchGadget(Info);

				// And leave ColourHasChanged as FALSE so that the code below does not do a ful redraw!
			}
			break;
		}


		default:
			return;		// Illegal drag -= just ignore it
	}


	// If necessary, redraw the picker and the colour patch
	if (ColourHasChanged || StartingNewDrag)
	{
		InvalidateGadget(_R(IDC_EDIT_PICKER));
		PaintGadgetNow(_R(IDC_EDIT_PICKER));
	}

	// Remember the last position at which we updated
	LastDragPos.y = YPos;
	LastDragPos.x = XPos;

#else
/*
	DocRect ValueRect(0, 0, Info->dx, Info->dy);
	ValueRect.hi.x = HalfGridLock(ValueRect.hi.x, PixelSize);
	ValueRect.lo.x = HalfGridLock(ValueRect.hi.x - (ValueRect.Width() / 8), PixelSize * 2);


	// Diameter is the width of the gadget - the value rectangle - a gap
	INT32 Diameter = Info->dx - ValueRect.Width() - (ValueRect.Width() / 3);
	if (Diameter > abs(Info->dy))
		Diameter = abs(Info->dy);

	const INT32 Radius = HalfGridLock(Diameter/2, PixelSize);
	const INT32 MidY = HalfGridLock(abs(Info->dy) / 2, PixelSize);
	const INT32 MidX = HalfGridLock(0 + Radius, PixelSize);

	ValueRect.lo.y = MidY - Radius;	// Rect only as high as the circle
	ValueRect.hi.y = MidY + Radius;

	// Get a colour context for our conversions and stuff
	ColourContext *cc = ColourContext::GetGlobalDefault(COLOURMODEL_HSVT);

	// If starting a new drag, determine which area the drag is in...
	if (StartingNewDrag)
	{
		if (ValueRect.ContainsCoord(*(Info->pMousePos)))
			DragStartArea = CEDRAG_HSVPICKER_VSLIDER;
		else
		{
			DocRect HSRect (ValueRect);
			HSRect.lo.x = MidX - Radius;
			HSRect.hi.x = MidX + Radius;
			
			// Not in the square containing the HS circle either, so ignore it
			if (!HSRect.ContainsCoord(*(Info->pMousePos)))
				return;
			
			DragStartArea = CEDRAG_HSVPICKER_HSWHEEL;
		}

		// Remember what the colour was as we started the drag
		cc->ConvertColour(ResultColour, &ColourBeforeDrag);
	}


	BOOL ColourHasChanged = FALSE;
	INT32 XPos;
	INT32 YPos;

	// Handle the new mouse position, using the area the drag started in (rather than
	// the area it may now be over) to determine which components to alter
	switch(DragStartArea)
	{
		case CEDRAG_HSVPICKER_VSLIDER:
		{
			YPos = Info->pMousePos->y - ValueRect.lo.y;
			if (YPos < 0)			YPos = 0;
			if (YPos > Diameter)	YPos = Diameter;

			double NewValue = ((double) YPos) /	((double) Diameter);
			ColourPicker::SetComponentFromDouble(EditingColour, cc, 3, NewValue);

			DisplayModel = COLOURMODEL_HSVT;	// Force display back to HSV model

			if (YPos != LastDragPos.y)
				ColourHasChanged = TRUE;
			break;
		}


		case CEDRAG_HSVPICKER_HSWHEEL:
		{
			YPos = Info->pMousePos->y - MidY;
			if (YPos < -Radius)	YPos = -Radius;
			if (YPos > Radius)	YPos = Radius;

			XPos = Info->pMousePos->x - MidX;
			if (XPos < -Radius)	XPos = -Radius;
			if (XPos > Radius)	XPos = Radius;

			double Temp = (double) XPos;		// NewSat = sqrt(X^2 + Y^2)
			double NewSat = Temp * Temp;
			Temp = (double) YPos;
			NewSat += Temp * Temp;
			NewSat = sqrt(NewSat) / Radius;
			if (NewSat > 1.0)					// Clip to within gamut
				NewSat = 1.0;

			double NewHue = 0.25;				// (90 degrees of hue) in 0.0-1.0 space
			if (YPos == 0)
			{
				if (XPos < 0) NewHue = 0.75;	// (270 degrees of hue) in 0.0-1.0 space
				// (else it is 0.25, the default from above)
			}
			else
			{
				// Get angle (in radians) within the 0 to (PI/2) quadrant
				NewHue = atan( fabs( ((double)XPos) / ((double)YPos) ) );

				// And fix the angle into the appropriate quadrant...
				if (YPos < 0)
					NewHue = PI - NewHue;

				if (XPos < 0)
					NewHue = (2.0 * PI) - NewHue;

				// And convert 0 to 2PI radians value into a 0.0 to 1.0 Hue value
				NewHue /= 2.0 * PI;
			}

			if (KeyPress::IsConstrainPressed())
			{
				// If we should constrain this value, then we check the original
				// values for Hue/Sat, and we only change the one which is furthest
				// away from the original value (i.e. if you drag near the old
				// Saturation value, only the Hue will change & vice versa)
				ColourHSVT *ConstrainDef = (ColourHSVT *) &ColourBeforeDrag;

				// NOTE:
				// The desired behaviour has several strange consequences
				// The "distance" from the old value should be the same on-screen
				// distance (e.g. 5 pixels around the circular Hue arc should be
				// the same as 5 pixels along the linear saturation radial. 
				// Also, Hue wraps back around (1.0 is adjacent to 0.0) so we need to
				// take the "closest" direction around the circle!
				// Thus, we find the difference in hue, taking the shortest route, and
				// turn it into the (0 to PI) range.
				// This is compared to a value of (0 to 1.0) for saturation

				// Get Hue distance in terms of 0..PI
				Temp = NewHue - ConstrainDef->Hue.MakeDouble();
				if (Temp < 0.0)
					Temp = -Temp;			// Ensure difference is positive

				if (Temp > 1.0)				// "Out of Gamut", so make distance = 0
					Temp = 0.0;
				else
				{
					if (Temp >= 0.5)		// Ensure difference is shortest distance
						Temp = 1.0 - Temp;	// around the circle

					Temp *= PI;				// And get as a 0..PI value
				}

				// Get Saturation in the range 0.0 to 1.0
				double TempSat = NewSat - ConstrainDef->Saturation.MakeDouble();
				if (TempSat < 0.0)
					TempSat = -TempSat;		// Ensure difference is positive

				if (TempSat > 1.0)			// Ensure it is clipped at 1.0
					TempSat = 1.0;

				// Finally, if the saturation difference is larger than the Hue
				// difference, then lock the hue, else lock the saturation
				if (TempSat > Temp)
					NewHue = ConstrainDef->Hue.MakeDouble();
				else
					NewSat = ConstrainDef->Saturation.MakeDouble();
			}

			DisplayModel = COLOURMODEL_HSVT;	// Force display back to HSV model

			if (!StartingNewDrag)	// When starting a new drag we always do a complete redraw
			{
				// Get the old cross-position values from EditingColour
				ColourHSVT ColourDef;
				cc->ConvertColour(EditingColour, (ColourGeneric *) &ColourDef);

				// Set the new values
				ColourPicker::SetComponentFromDouble(EditingColour, cc, 1, NewHue);
				ColourPicker::SetComponentFromDouble(EditingColour, cc, 2, NewSat);


				// --- Invalidate the old cross position
				// RedrawSize gives the 'radius' of the cross rectangle to redraw. NOTE however,
				// that the cross lines can move up to 1 pixel outside the CrossRect, so we have to
				// invalidate one pixel more, just to make sure we always get the bugger!
				INT32 RedrawSize = CROSSRADIUS + (PixelSize * 2);
				DocRect CrossRect(-RedrawSize, -RedrawSize, RedrawSize, RedrawSize);
	
				// Convert Hue (0.0 -> 1.0) into an angle in radians (0 -> 2Pi)
				double theta  = 2.0 * PI * ColourDef.Hue.MakeDouble();
	
				// Convert Saturation into a distance from the center of the colour wheel
				double length = (double)Radius * ColourDef.Saturation.MakeDouble();

				// Translate the cross to the polar coordinate (theta, length)
				CrossRect.Translate(HalfGridLock(MidX + (INT32)(length * sin(theta)), PixelSize),
									HalfGridLock(MidY + (INT32)(length * cos(theta)), PixelSize));

				InvalidateGadget(_R(IDC_EDIT_PICKER), Info, &CrossRect);


				// --- Now invalidate the new cross position
				CrossRect = DocRect(-RedrawSize, -RedrawSize, RedrawSize, RedrawSize);

				// Convert Hue (0.0 -> 1.0) into an angle in radians (0 -> 2Pi)
				theta  = 2.0 * PI * NewHue;
	
				// Convert Saturation into a distance from the center of the colour wheel
				length = (double)Radius * NewSat;

				// Translate the cross to the polar coordinate (theta, length)
				CrossRect.Translate(HalfGridLock(MidX + (INT32)(length * sin(theta)), PixelSize),
									HalfGridLock(MidY + (INT32)(length * cos(theta)), PixelSize));

				InvalidateGadget(_R(IDC_EDIT_PICKER), Info, &CrossRect);

				// And finally, invalidate the value slider rectangle
				InvalidateGadget(_R(IDC_EDIT_PICKER), Info, &ValueRect);
				PaintGadgetNow(_R(IDC_EDIT_PICKER));
	
				InvalidatePatchGadget(Info);

				// We leave "ColourHasChanged" FALSE here so that it doesn't force redraw
				// everything again, below!
			}
			else
			{
				ColourPicker::SetComponentFromDouble(EditingColour, cc, 1, NewHue);
				ColourPicker::SetComponentFromDouble(EditingColour, cc, 2, NewSat);
				ColourHasChanged = TRUE;
			}
			break;
		}


		default:
			return;		// Illegal drag -= just ignore it
	}


	// If necessary, redraw the picker and the colour patch
	if (ColourHasChanged || StartingNewDrag)
	{
		InvalidateGadget(_R(IDC_EDIT_PICKER));
		PaintGadgetNow(_R(IDC_EDIT_PICKER));
	}

	// Remember the last position at which we updated
	LastDragPos.y = YPos;
	LastDragPos.x = XPos;
*/
#endif
}



/********************************************************************************************

>	void ColourEditDlg::SetNewValueRGB(ReDrawInfoType *Info, INT32 PixelSize,
										BOOL StartingNewDrag)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/12/94
	Inputs:		Info - The redraw info for the kernel-drawn control's mouse event
				PixelSize - The size of display pixels as they appear when mapped into
				the MILLIPOINT VirtualRect coordinate space (as used in the RenderControl
				methods)
				StartingNewDrag - TRUE if this is the first call to this function for a
				new drag operation (in which case, the DragStartArea is set appropriately)

	Purpose:	Handles drag updates - determines the new colour values from the current
				mouse position, and sets the colour as appropriate. Just how it interprets
				the mouse position is dependant upon which region the drag was started
				within, and which DisplayMode is in operation. A different routine like
				this one will be called for each DisplayMode.

	Notes:		The RGB and CMYK pickers are almost identical in operation. It should be
				possible to tidy up the code considerably and make a generic routine
				for handling most of the common stuff.

	SeeAlso:	ColourEditDlg::SetNewValueFromMousePos; ColourEditDlg::SetNewValueDefault

********************************************************************************************/

void ColourEditDlg::SetNewValueRGB(ReDrawInfoType *Info, INT32 PixelSize, BOOL StartingNewDrag)
{
	// Fill the control background, and calculate the cube params (without redraw)
	DocRect VirtualSize(0, 0, Info->dx, Info->dy);
	VirtualSize.Inflate(-PixelSize * 4);		// And exclude the border

	DocRect SquareRect;
	INT32 SizeZ;
	DrawCubeShadowAndCalcValues(NULL, &VirtualSize, PixelSize, NULL,
								/* TO */ &SquareRect, &SizeZ);

	DocRect CubeFaceRect(SquareRect);		// Remember the rect of the front face for below

	// Now, shift the square down from the front of the cube by an amount appropriate to
	// the Z-axis component of the colour
	ColourContextRGBT *cc = (ColourContextRGBT *)
							ColourContext::GetGlobalDefault(COLOURMODEL_RGBT);
	ColourRGBT SourceColour;
	if (EditingColour != NULL)
		cc->ConvertColour(EditingColour, (ColourGeneric *) &SourceColour);

	if (CanSetColour(EditingColour))
	{
		FIXED24 ZComponent = SourceColour.Blue;
		if (ColourPickerMode == 1)
			ZComponent = SourceColour.Green;
		else if (ColourPickerMode == 2)
			ZComponent = SourceColour.Red;

		INT32 ZTrans = (INT32) ((double)SizeZ * ZComponent.MakeDouble()) - SizeZ;
		SquareRect.Translate(-ZTrans, ZTrans);
		GridLockRect(&SquareRect, PixelSize);
	}

	if (StartingNewDrag)
	{
		// Remember what the colour was as we started the drag
		cc->ConvertColour(ResultColour, &ColourBeforeDrag);

		DocCoord BitmapPos;
		BitmapPos.x = HalfGridLock(CubeFaceRect.hi.x + SizeZ/2, PixelSize);
		BitmapPos.y = HalfGridLock(CubeFaceRect.hi.y - SizeZ/2, PixelSize);

		const INT32 BitmapSize = 14 * PixelSize;
		DocRect SwapAxesRect(BitmapPos.x, BitmapPos.y,
								BitmapPos.x + BitmapSize + PixelSize * 3, BitmapPos.y + BitmapSize);
		GridLockRect(&SwapAxesRect, PixelSize);

		if (SwapAxesRect.ContainsCoord(*(Info->pMousePos)))
		{
			// Swap the axes around - this is treated as a click (the rest of the drag ignored)
			DragStartArea = CEDRAG_NONE;
			ColourPickerMode = (ColourPickerMode + 1) % 3;
			InvalidateGadget(_R(IDC_EDIT_PICKER));
			return;
		}

		if (SquareRect.ContainsCoord(*(Info->pMousePos)))
			DragStartArea = CEDRAG_RGBPICKER_XY;
		else
		{
			// Check if the pointer is in the Z 'drag button'
			DocRect ZButton(SquareRect);
			ZButton.lo.x = ZButton.hi.x;
			ZButton.hi.x += ZSLIDERSIZE - (PixelSize * 2);
			ZButton.hi.y = ZButton.lo.y;
			ZButton.lo.y -= ZSLIDERSIZE - (PixelSize * 2);
			GridLockRect(&ZButton, PixelSize);

			if (ZButton.ContainsCoord(*(Info->pMousePos)))
			{
				DragStartArea = CEDRAG_RGBPICKER_Z;

				// And remember the offset from the bottom right of the handle to the dragged point
				DragAnchor.x = Info->pMousePos->x - ZButton.lo.x;
				DragAnchor.y = Info->pMousePos->y - ZButton.hi.y;
			}
			else
			{
				// OK, then. Is the click in a protuding bit of the marker cross?
				DocRect CrossRect;
				CalcCrossRectRGB(&SourceColour, &SquareRect, PixelSize, &CrossRect);

				if (CrossRect.ContainsCoord(*(Info->pMousePos)))
					DragStartArea = CEDRAG_RGBPICKER_XY;
			}
		}
	}

	if (!CanSetColour(EditingColour))		// We can't change the colour - abort
	{
		DragStartArea = CEDRAG_NONE;
		return;
	}

	BOOL ColourHasChanged = TRUE;

	INT32 XPos = Info->pMousePos->x;
	INT32 YPos = Info->pMousePos->y;

	switch(DragStartArea)
	{
		case CEDRAG_RGBPICKER_XY:
			{
				if (XPos < SquareRect.lo.x)  XPos = SquareRect.lo.x;
				if (XPos > SquareRect.hi.x)  XPos = SquareRect.hi.x;

				if (YPos < SquareRect.lo.y)  YPos = SquareRect.lo.y;
				if (YPos > SquareRect.hi.y)  YPos = SquareRect.hi.y;

				INT32 XComponent = 2;		// XYZ = G,R,B
				INT32 YComponent = 1;

				switch(ColourPickerMode)
				{
					case 1:				// XYZ = R,B,G
						XComponent = 1;
						YComponent = 3;
						break;

					case 2:				// XYZ = B,G,R
						XComponent = 3;
						YComponent = 2;
						break;
				}
			
				double NewX = ((double) (XPos - SquareRect.lo.x)) / 
								((double) SquareRect.Width());
				if (NewX < 0.0)
					NewX = 0.0;
				if (NewX > 1.0)
					NewX = 1.0;

				double NewY = ((double) (YPos - SquareRect.lo.y)) / 
								((double) SquareRect.Height());
				if (NewY < 0.0)
					NewY = 0.0;
				if (NewY > 1.0)
					NewY = 1.0;

				if (KeyPress::IsConstrainPressed())
				{
//					ColourRGBT *ConstrainDef = (ColourRGBT *) &ColourBeforeDrag;

					double XDiff = GetComponent(&ColourBeforeDrag, XComponent).MakeDouble() - NewX;
					double YDiff = GetComponent(&ColourBeforeDrag, YComponent).MakeDouble() - NewY;

					if (XDiff < 0)  XDiff = -XDiff;
					if (YDiff < 0)  YDiff = -YDiff;

					if (XDiff < YDiff)
						NewX = GetComponent(&ColourBeforeDrag, XComponent).MakeDouble();
					else
						NewY = GetComponent(&ColourBeforeDrag, YComponent).MakeDouble();
				}

				// Set the new values
				ColourPicker::SetComponentFromDouble(EditingColour, cc, XComponent, NewX);
				ColourPicker::SetComponentFromDouble(EditingColour, cc, YComponent, NewY);

				DisplayModel = COLOURMODEL_RGBT;	// Force display back to RGB model

				if (!StartingNewDrag)
				{
					// Redraw the area where the cross used to be shown
					DocRect CrossRect;
					CalcCrossRectRGB(&SourceColour, &SquareRect, PixelSize, &CrossRect);
					CrossRect.Inflate(4*PixelSize);		// Inflate to ensure we redraw all of the cross
					InvalidateGadget(_R(IDC_EDIT_PICKER), Info, &CrossRect);

					// And force redraw the new cross position
					cc->ConvertColour(EditingColour, (ColourGeneric *) &SourceColour);
					CalcCrossRectRGB(&SourceColour, &SquareRect, PixelSize, &CrossRect);
					CrossRect.Inflate(4*PixelSize);		// Inflate to ensure we redraw all of the cross
					InvalidateGadget(_R(IDC_EDIT_PICKER), Info, &CrossRect);

					// Redraw the controls...
					InvalidatePatchGadget(Info);
					PaintGadgetNow(_R(IDC_EDIT_PICKER));

					// And clear this flag so that the redraw below (of entire gadget) is not done.					
					ColourHasChanged = FALSE;
				}
			}
			break;


		case CEDRAG_RGBPICKER_Z:
			{
				XPos -= DragAnchor.x;	// Correct the mouse position for anchoring
				YPos -= DragAnchor.y;

				INT32 ZComponent = 3;		// XYZ = G,R,B

				switch(ColourPickerMode)
				{
					case 1:				// XYZ = R,B,G
						ZComponent = 2;
						break;

					case 2:				// XYZ = B,G,R
						ZComponent = 1;
						break;
				}

				INT32 Offset = CubeFaceRect.hi.x + SizeZ - XPos;
				if (Offset > YPos - (CubeFaceRect.lo.y - SizeZ))
					Offset = YPos - (CubeFaceRect.lo.y - SizeZ);

				double NewZ =  ((double) Offset) / ((double) SizeZ);
				if (NewZ < 0.0)
					NewZ = 0.0;
				if (NewZ > 1.0)
					NewZ = 1.0;

				DisplayModel = COLOURMODEL_RGBT;	// Force display back to RGB model

				ColourPicker::SetComponentFromDouble(EditingColour, cc, ZComponent, NewZ);
			}
			break;


		default:
			return;		// Not a legal drag, so we ignore it
	}

	// If necessary, redraw the entire colour picker control and the colour patch
	if (ColourHasChanged)
	{
		if (StartingNewDrag || YPos != LastDragPos.y || XPos != LastDragPos.x)
		{
			InvalidateGadget(_R(IDC_EDIT_PICKER));
			PaintGadgetNow(_R(IDC_EDIT_PICKER));
		}
	}

	// Remember the last position at which we updated
	LastDragPos.y = YPos;
	LastDragPos.x = XPos;
}



/********************************************************************************************

>	void ColourEditDlg::SetNewValueCMYK(ReDrawInfoType *Info, INT32 PixelSize,
										BOOL StartingNewDrag)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/12/94
	Inputs:		Info - The redraw info for the kernel-drawn control's mouse event
				PixelSize - The size of display pixels as they appear when mapped into
				the MILLIPOINT VirtualRect coordinate space (as used in the RenderControl
				methods)
				StartingNewDrag - TRUE if this is the first call to this function for a
				new drag operation (in which case, the DragStartArea is set appropriately)

	Purpose:	Handles drag updates - determines the new colour values from the current
				mouse position, and sets the colour as appropriate. Just how it interprets
				the mouse position is dependant upon which region the drag was started
				within, and which DisplayMode is in operation. A different routine like
				this one will be called for each DisplayMode.

	Notes:		The RGB and CMYK pickers are almost identical in operation. It should be
				possible to tidy up the code considerably and make a generic routine
				for handling most of the common stuff.

	SeeAlso:	ColourEditDlg::SetNewValueFromMousePos; ColourEditDlg::SetNewValueDefault

********************************************************************************************/

void ColourEditDlg::SetNewValueCMYK(ReDrawInfoType *Info, INT32 PixelSize, BOOL StartingNewDrag)
{
	DocRect VirtualSize(0, 0, Info->dx, Info->dy);
	VirtualSize.Inflate(-PixelSize * 4);		// And exclude the border

	// Determine how much space is left over after the key slider has been placed
	INT32 KeySliderLeft = VirtualSize.hi.x - (CROSSRADIUS+PixelSize)*2;
	ERROR3IF(KeySliderLeft < VirtualSize.lo.x,
				"Not enough room to render the colour picker!");

	// Fill the control background, and draw the cube 'shadow'
	DocRect CubeAvailableSpace(VirtualSize);
	CubeAvailableSpace.hi.x = KeySliderLeft - 2000;

	DocRect SquareRect;
	INT32 SizeZ;
	DrawCubeShadowAndCalcValues(NULL, &CubeAvailableSpace, PixelSize, NULL,
								/* TO */ &SquareRect, &SizeZ);


	DocRect CubeFaceRect(SquareRect);		// Remember the rect of the front face for below

	// Now, shift the square down from the front of the cube by an amount appropriate to
	// the Z-axis component of the colour
//	ColourContextCMYK *cc = (ColourContextCMYK*)ColourContext::GetGlobalDefault(COLOURMODEL_CMYK);
	ColourContext *cc = NULL;
	BOOL bDeleteCC = GetColourContext(DisplayModel, &cc);	
	ColourCMYK SourceColour;

	if (CanSetColour(EditingColour))
	{
		cc->ConvertColour(EditingColour, (ColourGeneric *) &SourceColour);
		FIXED24 ZComponent = SourceColour.Magenta;
		if (ColourPickerMode == 1)
			ZComponent = SourceColour.Cyan;
		else if (ColourPickerMode == 2)
			ZComponent = SourceColour.Yellow;

		INT32 ZTrans = (INT32) ((double)SizeZ * ZComponent.MakeDouble()) - SizeZ;
		SquareRect.Translate(-ZTrans, ZTrans);
		GridLockRect(&SquareRect, PixelSize);
	}

	if (StartingNewDrag)
	{
		// Remember what the colour was as we started the drag
		cc->ConvertColour(ResultColour, &ColourBeforeDrag);

		DocCoord BitmapPos;
		BitmapPos.x = HalfGridLock(CubeFaceRect.hi.x + SizeZ/2, PixelSize);
		BitmapPos.y = HalfGridLock(CubeFaceRect.hi.y - SizeZ/2, PixelSize);

		const INT32 BitmapSize = 14 * PixelSize;
		DocRect SwapAxesRect(BitmapPos.x, BitmapPos.y,
								BitmapPos.x + BitmapSize + PixelSize * 3, BitmapPos.y + BitmapSize);
		GridLockRect(&SwapAxesRect, PixelSize);

		if (SwapAxesRect.ContainsCoord(*(Info->pMousePos)))
		{
			// Swap the axes around - this is treated as a click (the rest of the drag ignored)
			DragStartArea = CEDRAG_NONE;
			ColourPickerMode = (ColourPickerMode + 1) % 3;
			InvalidateGadget(_R(IDC_EDIT_PICKER));
			return;
		}

		if (SquareRect.ContainsCoord(*(Info->pMousePos)))
			DragStartArea = CEDRAG_CMYKPICKER_XY;
		else
		{
			// Check if the pointer is in the Z 'drag button'
			DocRect ZButton(SquareRect);
			ZButton.lo.x = ZButton.hi.x;
			ZButton.hi.x += ZSLIDERSIZE - (PixelSize * 2);
			ZButton.hi.y = ZButton.lo.y;
			ZButton.lo.y -= ZSLIDERSIZE - (PixelSize * 2);
			GridLockRect(&ZButton, PixelSize);

			if (ZButton.ContainsCoord(*(Info->pMousePos)))
			{
				DragStartArea = CEDRAG_CMYKPICKER_Z;

				// And remember the offset from the bottom right of the handle to the dragged point
				DragAnchor.x = Info->pMousePos->x - ZButton.lo.x;
				DragAnchor.y = Info->pMousePos->y - ZButton.hi.y;
			}
			else
			{
				// Check if the pointer is in the Key slider
				DocRect KeyRect(KeySliderLeft, 0, Info->dx, Info->dy - (PATCHSIZE + PixelSize*2));
				if (KeyRect.ContainsCoord(*(Info->pMousePos)))
					DragStartArea = CEDRAG_CMYKPICKER_KEY;
				else
				{
					// OK, then. Is the click in a protuding bit of the marker cross?
					DocRect CrossRect;
					CalcCrossRectCMYK(&SourceColour, &SquareRect, PixelSize, &CrossRect);

					if (CrossRect.ContainsCoord(*(Info->pMousePos)))
						DragStartArea = CEDRAG_CMYKPICKER_XY;
				}
			}
		}
	}

	if (!CanSetColour(EditingColour))		// We can't change the colour - abort
	{
		DragStartArea = CEDRAG_NONE;
		return;
	}

	BOOL ColourHasChanged = TRUE;

	INT32 XPos = Info->pMousePos->x;
	INT32 YPos = Info->pMousePos->y;

	switch(DragStartArea)
	{
		case CEDRAG_CMYKPICKER_XY:
			{
				if (XPos < SquareRect.lo.x)  XPos = SquareRect.lo.x;
				if (XPos > SquareRect.hi.x)  XPos = SquareRect.hi.x;

				if (YPos < SquareRect.lo.y)  YPos = SquareRect.lo.y;
				if (YPos > SquareRect.hi.y)  YPos = SquareRect.hi.y;

				INT32 XComponent = 1;		// XYZ = C,Y,M
				INT32 YComponent = 3;

				switch(ColourPickerMode)
				{
					case 1:				// XYZ = Y,M,C
						XComponent = 3;
						YComponent = 2;
						break;

					case 2:				// XYZ = M,C,Y
						XComponent = 2;
						YComponent = 1;
						break;
				}
			
				double NewX = ((double) (XPos - SquareRect.lo.x)) / 
								((double) SquareRect.Width());
				if (NewX < 0.0)
					NewX = 0.0;
				if (NewX > 1.0)
					NewX = 1.0;

				double NewY = ((double) (YPos - SquareRect.lo.y)) / 
								((double) SquareRect.Height());
				if (NewY < 0.0)
					NewY = 0.0;
				if (NewY > 1.0)
					NewY = 1.0;

				if (KeyPress::IsConstrainPressed())
				{
//					ColourCMYK *ConstrainDef = (ColourCMYK *) &ColourBeforeDrag;

					double XDiff = GetComponent(&ColourBeforeDrag, XComponent).MakeDouble() - NewX;
					double YDiff = GetComponent(&ColourBeforeDrag, YComponent).MakeDouble() - NewY;

					if (XDiff < 0)  XDiff = -XDiff;
					if (YDiff < 0)  YDiff = -YDiff;

					if (XDiff < YDiff)
						NewX = GetComponent(&ColourBeforeDrag, XComponent).MakeDouble();
					else
						NewY = GetComponent(&ColourBeforeDrag, YComponent).MakeDouble();
				}

				ColourPicker::SetComponentFromDouble(EditingColour, cc, XComponent, NewX);
				ColourPicker::SetComponentFromDouble(EditingColour, cc, YComponent, NewY);

				DisplayModel = COLOURMODEL_CMYK;	// Force display back to CMYK model

				if (!StartingNewDrag)
				{
					// Redraw the area where the cross used to be shown
					DocRect CrossRect;
					CalcCrossRectCMYK(&SourceColour, &SquareRect, PixelSize, &CrossRect);
					CrossRect.Inflate(4*PixelSize);		// Inflate to ensure we redraw all of the cross
					InvalidateGadget(_R(IDC_EDIT_PICKER), Info, &CrossRect);

					// And force redraw the new cross position
					cc->ConvertColour(EditingColour, (ColourGeneric *) &SourceColour);
					CalcCrossRectCMYK(&SourceColour, &SquareRect, PixelSize, &CrossRect);
					CrossRect.Inflate(4*PixelSize);		// Inflate to ensure we redraw all of the cross
					InvalidateGadget(_R(IDC_EDIT_PICKER), Info, &CrossRect);

					// Redraw the controls...
					InvalidatePatchGadget(Info);
					PaintGadgetNow(_R(IDC_EDIT_PICKER));

					// And clear this flag so that the redraw below (of entire gadget) is not done.					
					ColourHasChanged = FALSE;
				}
			}
			break;


		case CEDRAG_CMYKPICKER_Z:
			{
				XPos -= DragAnchor.x;	// Correct the mouse position for anchoring
				YPos -= DragAnchor.y;

				INT32 ZComponent = 2;		// XYZ = C,Y,M

				switch(ColourPickerMode)
				{
					case 1:				// XYZ = Y,M,C
						ZComponent = 1;
						break;

					case 2:				// XYZ = M,C,Y
						ZComponent = 3;
						break;
				}

				INT32 Offset = CubeFaceRect.hi.x + SizeZ - XPos;
				if (Offset > YPos - (CubeFaceRect.lo.y - SizeZ))
					Offset = YPos - (CubeFaceRect.lo.y - SizeZ);

				double NewZ =  ((double) Offset) / ((double) SizeZ);
				if (NewZ < 0.0)
					NewZ = 0.0;
				if (NewZ > 1.0)
					NewZ = 1.0;

				DisplayModel = COLOURMODEL_CMYK;	// Force display back to CMYK model
				ColourPicker::SetComponentFromDouble(EditingColour, cc, ZComponent, NewZ);
			}
			break;


		case CEDRAG_CMYKPICKER_KEY:
			{
				XPos = LastDragPos.x = 0;			// Make sure X move does not cause redraw

				if (YPos < 0)
					YPos = 0;
				if (YPos > Info->dy - (PATCHSIZE + PixelSize*2))
					YPos = Info->dy - (PATCHSIZE + PixelSize*2);

				double NewValue = ((double)YPos) / ((double)(Info->dy - (PATCHSIZE + PixelSize*2)));
				DisplayModel = COLOURMODEL_CMYK;	// Force display back to CMYK model
				ColourPicker::SetComponentFromDouble(EditingColour, cc, 4, NewValue);
			}
			break;


		default:
			return;		// Not a legal drag, so we ignore it
	}

	// Delete the colour context if necessary
	if (bDeleteCC)
		ColourContextList::GetList()->RemoveContext(&cc);			// Have finished with it

	// If necessary, redraw the colour picker control and the colour patch
	if (ColourHasChanged)
	{
		if (StartingNewDrag || YPos != LastDragPos.y || XPos != LastDragPos.x)
		{
			InvalidateGadget(_R(IDC_EDIT_PICKER));
			PaintGadgetNow(_R(IDC_EDIT_PICKER));
		}
	}

	// Remember the last position at which we updated
	LastDragPos.y = YPos;
	LastDragPos.x = XPos;
}



/********************************************************************************************

>	void ColourEditDlg::SetNewValueDefault(ReDrawInfoType *Info, INT32 PixelSize,
										BOOL StartingNewDrag)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/94
	Inputs:		Info - The redraw info for the kernel-drawn control's mouse event
				PixelSize - The size of display pixels as they appear when mapped into
				the MILLIPOINT VirtualRect coordinate space (as used in the RenderControl
				methods)
				StartingNewDrag - TRUE if this is the first call to this function for a
				new drag operation (in which case, the DragStartArea is set appropriately)

	Purpose:	Handles drag updates - determines the new colour values from the current
				mouse position, and sets the colour as appropriate. Just how it interprets
				the mouse position is dependant upon which region the drag was started
				within, and which DisplayMode is in operation. A different routine like
				this one will be called for each DisplayMode.

	Notes:		The default method is called for all DisplayModes which are not
				specifically handled: It interprets mouse positions within the display
				drawn by its sister redraw method, which provides up to 4 horizontal
				component sliders.

	SeeAlso:	ColourEditDlg::SetNewValueFromMousePos; ColourEditDlg::SetNewValueDefault;
				ColourEditDlg::RenderPickerDefault

********************************************************************************************/

void ColourEditDlg::SetNewValueDefault(ReDrawInfoType *Info, INT32 PixelSize, BOOL StartingNewDrag)
{
//	ColourContext *cc = ColourContext::GetGlobalDefault(DisplayModel);
	ColourContext *cc = NULL;
	BOOL bDeleteCC = GetColourContext(DisplayModel, &cc);	
	if (cc == NULL)
	{
		ERROR2RAW("Can't get a colour context for the DisplayModel!");
		return;
	}

	INT32 CurrentComponentIndex = 0;		// Work out the index of the slider being dragged
	if (!StartingNewDrag)
	{
		switch (DragStartArea)
		{
			case CEDRAG_DEFAULT_COMPONENT2:
				CurrentComponentIndex = 1;
				break;

			case CEDRAG_DEFAULT_COMPONENT3:
				CurrentComponentIndex = 2;
				break;

			case CEDRAG_DEFAULT_COMPONENT4:
				CurrentComponentIndex = 3;
				break;

			default:
				break;
		}
	}

	// Get the slider rectangle widths. The height is fixed/moved during the loop below
	DocRect SliderRect(0, 0, Info->dx, Info->dy);
	SliderRect.hi.y -= PATCHSIZE + (PixelSize * 2);		// Allow space for the current colour patch

	// Count how many components we have to display
	INT32 NumComponents = 0;
	INT32 ComponentIndex;
	for (ComponentIndex = 1; ComponentIndex <= 4; ComponentIndex++)
	{
		if (cc->GetComponentName(ComponentIndex, NULL))
			NumComponents++;
	}

	// Calculate slider sizes and spacing
	INT32 SliderHeight = GetSliderHeight(SliderRect.Height(), NumComponents);
	INT32 SliderGap = GetSliderGap(SliderRect.Height(), NumComponents);

	// And move the top down by half a SliderGap, so the sliders are centered vertically
	SliderRect.hi.y -= SliderGap / 2;

	// Check the slider for each component supplied in the current DisplayModel
	for (ComponentIndex = 0; ComponentIndex <= 3; ComponentIndex++)
	{
		// Ensure slider rect is the correct height
		SliderRect.lo.y = SliderRect.hi.y - SliderHeight;

		// If we are just starting a new drag, we must determine which slider the mouse is
		// dragging. Otherwise, we need just go round the loop until we find the position
		// of the correct slider to use (so we can redraw only the changed slider)
		if (StartingNewDrag)
		{
			// If this component is available/used in this colour model, see if we are dragging it
			if (cc->GetComponentName(ComponentIndex+1, NULL))
			{
				if (SliderRect.ContainsCoord(*(Info->pMousePos)))
					break;
			}
		}
		else
		{
			// We've calculated where the slider rect is now, so we can continue on
			if (ComponentIndex == CurrentComponentIndex)
				break;
		}

		// Move down to the next slider rectangle position
		SliderRect.hi.y = SliderRect.lo.y - SliderGap;
	}

	// ComponentIndex now contains the index of the component currently being
	// dragged (or it has the value 4 if no match was found)
	CurrentComponentIndex = ComponentIndex;

	if (StartingNewDrag)		// Work out which slider we are dragging
	{
		switch (CurrentComponentIndex)
		{
			case 0:
				DragStartArea = CEDRAG_DEFAULT_COMPONENT1;
				break;

			case 1:
				DragStartArea = CEDRAG_DEFAULT_COMPONENT2;
				break;

			case 2:
				DragStartArea = CEDRAG_DEFAULT_COMPONENT3;
				break;

			case 3:
				DragStartArea = CEDRAG_DEFAULT_COMPONENT4;
				break;

			default:
				return;		// Not dragging anything draggable, so ignore them
		}
	}


	// Now, if the mouse position has changed, calculate the new component value, and
	// set it, as appropriate for the CurrentComponentIndex component
	if (StartingNewDrag || Info->pMousePos->x != LastDragPos.x)
	{
		INT32 Offset = Info->pMousePos->x;
		if (Offset < 0)
			Offset = 0;

		if (Offset > Info->dx)
			Offset = Info->dx;

		ERROR3IF(Info->dx == 0, "Control width is zero! Divide by zero about to occur!");
		double NewValue = (double)Offset / (double)Info->dx;

		ColourPicker::SetComponentFromDouble(EditingColour, cc, CurrentComponentIndex+1, NewValue);

		// If we are starting a new drag, or if we are in split-slider mode, we redraw everything
		if (StartingNewDrag || SplitLineDisplay)
			InvalidateGadget(_R(IDC_EDIT_PICKER));						// Invalidate all sliders
		else
		{
			InvalidateGadget(_R(IDC_EDIT_PICKER), Info, &SliderRect);	// Only invalidate changed slider
			InvalidatePatchGadget(Info);
		}
		PaintGadgetNow(_R(IDC_EDIT_PICKER));	
	}

	LastDragPos.x = Info->pMousePos->x;
	LastDragPos.y = 0;

	// Delete the colour context if necessary
	if (bDeleteCC)
		ColourContextList::GetList()->RemoveContext(&cc);			// Have finished with it
}



/********************************************************************************************

>	void ColourEditDlg::SetNewValueTint(ReDrawInfoType *Info, INT32 PixelSize,
										BOOL StartingNewDrag)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/10/95

	Inputs:		Info - The redraw info for the kernel-drawn control's mouse event
				PixelSize - The size of display pixels as they appear when mapped into
				the MILLIPOINT VirtualRect coordinate space (as used in the RenderControl
				methods)
				StartingNewDrag - TRUE if this is the first call to this function for a
				new drag operation (in which case, the DragStartArea is set appropriately)

	Purpose:	Handles drag updates - determines the new colour values from the current
				mouse position, and sets the colour as appropriate. 

				Handles drags when editing Tints and Shades.

	SeeAlso:	ColourEditDlg::SetNewValueFromMousePos; ColourEditDlg::SetNewValueDefault;
				ColourEditDlg::RenderPickerDefault

********************************************************************************************/

void ColourEditDlg::SetNewValueTint(ReDrawInfoType *Info, INT32 PixelSize, BOOL StartingNewDrag)
{
	ERROR3IF(EditingColour->GetType() != COLOURTYPE_TINT,
				"ColourEditDlg::SetNewValuTint called on non-tint/shade colour!");

	// Calculate the slider rectangle
	const INT32 SliderHeight = 18000;
	DocRect SliderRect(0, 0, Info->dx, Info->dy);
	SliderRect.Inflate(0, -(Info->dy - SliderHeight) / 2);

	if (StartingNewDrag)		// Work out which slider we are dragging
	{
		if (!SliderRect.ContainsCoord(*(Info->pMousePos)))
			return;

		DragStartArea = CEDRAG_TINTORSHADE;
	}

	// Now, if the mouse position has changed, calculate the new component value, and
	// set it, as appropriate for the CurrentComponentIndex component
	if (StartingNewDrag || Info->pMousePos->x != LastDragPos.x)
	{
		INT32 Offset = Info->pMousePos->x;
		if (Offset < 0)
			Offset = 0;

		if (Offset > Info->dx)
			Offset = Info->dx;

		ERROR3IF(Info->dx == 0, "Control width is zero! Divide by zero about to occur!");
		double NewValue = (double)Offset / (double)Info->dx;
		NewValue += 1.0 / ((double) (1<<25));

		// Clip to lie within gamut
		if (NewValue < 0.0)
			NewValue = 0.0;

		if (NewValue > 1.0)
			NewValue = 1.0;

		FIXED24 NewValue24(NewValue);
		EditingColour->SetTintValue(NewValue24);

		// If we are starting a new drag, or if we are in split-slider mode, we redraw everything
		if (StartingNewDrag)
			InvalidateGadget(_R(IDC_EDIT_PICKER));						// Invalidate all sliders
		else
		{
			InvalidateGadget(_R(IDC_EDIT_PICKER), Info, &SliderRect);	// Only invalidate changed slider area
			InvalidatePatchGadget(Info);
		}
		PaintGadgetNow(_R(IDC_EDIT_PICKER));	
	}

	LastDragPos.x = Info->pMousePos->x;
	LastDragPos.y = 0;
}



/********************************************************************************************

>	void ColourEditDlg::SetNewValueShade(ReDrawInfoType *Info, INT32 PixelSize,
										BOOL StartingNewDrag)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/10/95
	Inputs:		Info - The redraw info for the kernel-drawn control's mouse event
				PixelSize - The size of display pixels as they appear when mapped into
				the MILLIPOINT VirtualRect coordinate space (as used in the RenderControl
				methods)
				StartingNewDrag - TRUE if this is the first call to this function for a
				new drag operation (in which case, the DragStartArea is set appropriately)

	Purpose:	Handles drag updates - determines the new colour values from the current
				mouse position, and sets the colour as appropriate. Just how it interprets
				the mouse position is dependant upon which region the drag was started
				within, and which DisplayMode is in operation. A different routine like
				this one will be called for each DisplayMode.

	SeeAlso:	ColourEditDlg::SetNewValueFromMousePos; ColourEditDlg::SetNewValueDefault

********************************************************************************************/

void ColourEditDlg::SetNewValueShade(ReDrawInfoType *Info, INT32 PixelSize, BOOL StartingNewDrag)
{
	DocRect VirtualSize(0, 0, Info->dx, Info->dy);
	VirtualSize.Inflate(-PixelSize * 4);		// And exclude the border

	// Calculate the two important rectangles - the hue slider, and Val/Sat square
	DocRect HueRect;
	DocRect ValSatSquare;
	CalculateHSVPickerRects(&VirtualSize, PixelSize, &HueRect, &ValSatSquare);

	// Get a colour context for our conversions and stuff
	ColourContext *cc = ColourContext::GetGlobalDefault(COLOURMODEL_HSVT);

	// Remember the old cross-position values from EditingColour
	ColourHSVT ColourDef;
	cc->ConvertColour(EditingColour, (ColourGeneric *) &ColourDef);

	// If starting a new drag, determine which area the drag is in...
	if (StartingNewDrag)
	{
		if (!ValSatSquare.ContainsCoord(*(Info->pMousePos)))
		{
			// The user didn't hit inside a picker area, but have they tried to drag
			// a region outside that lies under a protuding marker cross?
			FIXED24 ReverseSat = FIXED24(1.0) - ColourDef.Saturation;

			if (!MousePosInCross(Info->pMousePos, &ValSatSquare, &ReverseSat, &ColourDef.Value))
				return;		// Nope - just ignore the click
		}

		DragStartArea = CEDRAG_HSVPICKER_VSSQUARE;

		// Remember what the colour was as we started the drag
		cc->ConvertColour(ResultColour, &ColourBeforeDrag);
	}


	BOOL ColourHasChanged = FALSE;
	INT32 XPos;
	INT32 YPos;

	// Handle the new mouse position, using the area the drag started in (rather than
	// the area it may now be over) to determine which components to alter
	switch(DragStartArea)
	{
		case CEDRAG_HSVPICKER_VSSQUARE:
		{
			INT32 Size = ValSatSquare.Width();

			XPos = ValSatSquare.hi.x - Info->pMousePos->x;
			if (XPos < 0)		XPos = 0;
			if (XPos > Size)	XPos = Size;

			YPos = Info->pMousePos->y - ValSatSquare.lo.y;
			if (YPos < 0)		YPos = 0;
			if (YPos > Size)	YPos = Size;

			double NewSat = ((double)XPos) / ((double) Size);
			double NewVal = ((double)YPos) / ((double) Size);

			ColourHSVT ParentDef;
			cc->ConvertColour(EditingColour->FindLastLinkedParent(), (ColourGeneric *) &ParentDef);

			if (KeyPress::IsConstrainPressed())
			{
				// If we should constrain this value, then we check the original
				// values for Sat/Val, and we only change the one which is furthest
				// away from the original value (i.e. if you drag near the old
				// Saturation value, only the Value will change & vice versa)

				// Shades constrain to their *parent* colour rather than their last definition
				ColourHSVT *ConstrainDef = &ParentDef; //(ColourHSVT *) &ColourBeforeDrag;

				double TempSat = ConstrainDef->Saturation.MakeDouble() - NewSat;
				if (TempSat < 0.0) TempSat = -TempSat;

				double TempVal = ConstrainDef->Value.MakeDouble() - NewVal;
				if (TempVal < 0.0) TempVal = -TempVal;

				if (TempSat < TempVal)
					NewSat = ConstrainDef->Saturation.MakeDouble();
				else
					NewVal = ConstrainDef->Value.MakeDouble();
			}

			FIXED24 NewX(0.0);
			FIXED24 NewY(0.0);

			// Work out saturation scaling factor
			double ParentVal = ParentDef.Saturation.MakeDouble();

			if (ParentVal > NewSat)
			{
				if (ParentVal > 0.0)
					NewX = -((ParentVal - NewSat) / ParentVal);
			}
			else
			{
				if (ParentVal < 1.0)
					NewX = (NewSat - ParentVal) / (1.0 - ParentVal);
			}

			// Work out Value scaling factor
			ParentVal = ParentDef.Value.MakeDouble();
			if (ParentVal > NewVal)
			{
				if (ParentVal > 0.0)
					NewY = -((ParentVal - NewVal) / ParentVal);
			}
			else
			{
				if (ParentVal < 1.0)
					NewY = (NewVal - ParentVal) / (1.0 - ParentVal);
			}


			EditingColour->SetShadeValues(NewX, NewY);

			if (StartingNewDrag)
				ColourHasChanged = TRUE;	// Always do full redraw when drag starts
			else
			{
				// --- Invalidate the old cross position
				DocRect CrossRect(-CROSSRADIUS, -CROSSRADIUS, CROSSRADIUS, CROSSRADIUS);

				INT32 TransX = ValSatSquare.hi.x - (INT32)
							((double)ValSatSquare.Width() * ColourDef.Saturation.MakeDouble());
				INT32 TransY = ValSatSquare.lo.y + (INT32)
							((double)ValSatSquare.Height() * ColourDef.Value.MakeDouble());

				CrossRect.Translate(HalfGridLock(TransX, PixelSize), HalfGridLock(TransY, PixelSize));

				// Inflate by 2 pixels (the cross can actually draw marginally outside the crossrect ;-(
				CrossRect.Inflate(4*PixelSize);
				InvalidateGadget(_R(IDC_EDIT_PICKER), Info, &CrossRect);


				// --- And invalidate the new cross position
				CrossRect = DocRect(-CROSSRADIUS, -CROSSRADIUS, CROSSRADIUS, CROSSRADIUS);

				cc->ConvertColour(EditingColour, (ColourGeneric *) &ColourDef);
				TransX = ValSatSquare.hi.x - (INT32)
							((double)ValSatSquare.Width() * ColourDef.Saturation.MakeDouble());
				TransY = ValSatSquare.lo.y + (INT32)
							((double)ValSatSquare.Height() * ColourDef.Value.MakeDouble());

				CrossRect.Translate(HalfGridLock(TransX, PixelSize), HalfGridLock(TransY, PixelSize));

				// Inflate by 2 pixels (the cross can actually draw marginally outside the crossrect ;-(
				CrossRect.Inflate(4*PixelSize);
				InvalidateGadget(_R(IDC_EDIT_PICKER), Info, &CrossRect);

				PaintGadgetNow(_R(IDC_EDIT_PICKER));
	
				InvalidatePatchGadget(Info);

				// And leave ColourHasChanged as FALSE so that the code below does not do a ful redraw!
			}
			break;
		}


		default:
			return;		// Illegal drag -= just ignore it
	}


	// If necessary, redraw the picker and the colour patch
	if (ColourHasChanged || StartingNewDrag)
	{
		InvalidateGadget(_R(IDC_EDIT_PICKER));
		PaintGadgetNow(_R(IDC_EDIT_PICKER));
	}

	// Remember the last position at which we updated
	LastDragPos.y = YPos;
	LastDragPos.x = XPos;
}



/********************************************************************************************

>	BOOL ColourEditDlg::HandleIdlePointer(ReDrawInfoType *Info, String_128 *BubbleHelp,
											String_256 *StatusHelp)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/10/95
	Inputs:		Info - Kernel-rendered gadget info indicating where the mouse pointer is
				over the colour picker (_R(IDC_EDIT_PICKER)) control

	Outputs:	BubbleHelp - Returned containing the new bubble help string to use for this
				mouse position.

				StatusHelp - Returned containing appropriate status-line help.

				ControlID - Returned with a unique "control" ID number for the "control" which
				the pointer is over. Currently this means _R(IDC_EDIT_PATCH1) or _R(IDC_EDIT_PATCH2),
				which are not real controls, but fake ones just to get a unique ID number for
				the current/original colour patches.

	Returns:	TRUE if the bubble help is valid, FALSE if there is no new bubble help
				(Bubble help will always be set to a valid string, possibly blank)

	Purpose:	Called when the mouse is idling over the colour picker control.
				Sets appropriate pointer shapes, and also returns bubble help for
				appropriate regions (probably only the original/current colour patches)

********************************************************************************************/

BOOL ColourEditDlg::HandleIdlePointer(ReDrawInfoType *Info, String_128 *BubbleHelp, String_256 *StatusHelp,
										UINT32 *ControlID)
{
	BOOL Result = FALSE;

	ERROR3IF(Info == NULL || BubbleHelp == NULL || ControlID == NULL, "Illegal NULL params");

	// Return a valid string no matter what happens
	*BubbleHelp = String_128(_T(""));
	*StatusHelp = String_256(_T(""));
	*ControlID = 0;							// No bubble help "control"

	if (EditingColour == NULL || AmShaded)		// We are shaded - abort
		return(FALSE);

	// Reset the cursor ID to none. If nobody changes this before the end of this function,
	// then the cursor will reset to the default (arrow or whatever). See SetCursor
//	UINT32 OldCurrentCursor = CurrentCursorID;
	CurrentCursorID = 0;	

	// First, calculate all the regions and stuff...
	INT32 PixelSize = 72000 / Info->Dpi;		// Size of output pixel in millipoints


	// If the pointer is over the colour patches then handle it and return immediately, without
	// passing the call onto the specific picker.
//	if (pointer in the current/original colour patches)
//		Set up bubble help
//		Change pointer?
//		return(TRUE);
	DocRect VirtualSize(0, 0, Info->dx, Info->dy);
	VirtualSize.Inflate(-PixelSize * 4);

	// Now draw the original/current colour patch in the top right corner
	DocRect PatchRect(VirtualSize);
	PatchRect.lo.x = PatchRect.hi.x - PATCHSIZE;
	PatchRect.lo.y = PatchRect.hi.y - PATCHSIZE;
	GridLockRect(&PatchRect, PixelSize);

	// Patches are horizontal if the colour model is not HSV
	BOOL HorzPatch = (DisplayModel != COLOURMODEL_HSVT);

	// But this setting is overridden for the special tint and shade modes
	if (EditingColour != NULL && EditingColour->GetType() == COLOURTYPE_TINT)
	{
		if (EditingColour->TintIsShade())
			HorzPatch = FALSE;
		else
			HorzPatch = TRUE;
	}

	{
		INT32 OverWhat = 0;

		if (PatchRect.ContainsCoord(*(Info->pMousePos)))
		{
			if (HorzPatch)
				OverWhat = 1;
			else
				OverWhat = 2;
		}
		else
		{
			if (HorzPatch)
				PatchRect.Translate(-PATCHSIZE, 0);
			else
				PatchRect.Translate(0, -PATCHSIZE);

			if (PatchRect.ContainsCoord(*(Info->pMousePos)))
			{
				if (HorzPatch)
					OverWhat = 2;
				else
					OverWhat = 1;
			}
		}

		if (OverWhat != 0)
		{
			if (OverWhat == 1)
			{
				BubbleHelp->MakeMsg(_R(IDS_K_COLDLOG_CURRCOLBBL));
				StatusHelp->MakeMsg(_R(IDS_K_COLDLOG_CURRCOLSTAT));
				*ControlID = _R(IDC_EDIT_PATCH1);
			}
			else
			{
				BubbleHelp->MakeMsg(_R(IDS_K_COLDLOG_ORIGCOLBBL));
				StatusHelp->MakeMsg(_R(IDS_K_COLDLOG_ORIGCOLSTAT));
				*ControlID = _R(IDC_EDIT_PATCH2);
			}

			Result = TRUE;
		}
	}

	// Finally, if we haven't sorted ourselves out, call the relevant colour picker handler
	// for the current display mode etc

	if (BubbleHelp->IsEmpty())
	{
		if (EditingColour->GetType() == COLOURTYPE_TINT)
		{
			if (EditingColour->TintIsShade())
				Result = HandleIdlePointerShade(Info, BubbleHelp, StatusHelp);
			else
				Result = HandleIdlePointerTint(Info, BubbleHelp, StatusHelp);
		}
		else
		{
			switch (DisplayModel)
			{
				case COLOURMODEL_HSVT:
					Result = HandleIdlePointerHSV(Info, BubbleHelp, StatusHelp);
					break;

				case COLOURMODEL_RGBT:
				case COLOURMODEL_WEBRGBT:
					// RGB has the ability to change display modes, even when the colour is uneditable
					// so we call the handler even when CanSetColour() == FALSE
					if (Use3DDisplay)
						Result = HandleIdlePointerRGB(Info, BubbleHelp, StatusHelp);
					else
						Result = HandleIdlePointerDefault(Info, BubbleHelp, StatusHelp);
					break;

				case COLOURMODEL_CMYK:
					// CMYK has the ability to change display modes, even when the colour is uneditable
					// so we call the handler even when CanSetColour() == FALSE
					if (Use3DDisplay)
						Result = HandleIdlePointerCMYK(Info, BubbleHelp, StatusHelp);
					else
						Result = HandleIdlePointerDefault(Info, BubbleHelp, StatusHelp);
					break;

				default:
					Result = HandleIdlePointerDefault(Info, BubbleHelp, StatusHelp);
					break;
			}
		}
	}

	// Last, set the appropriate cursor shape
/*
	if (OldCurrentCursor != CurrentCursorID)
	{
TRACEUSER( "Jason", _T("Cursor Change=%ld to %ld\n"), OldCurrentCursor, CurrentCursorID);
		if (CurrentCursor != NULL)
		{
			delete CurrentCursor;
			CurrentCursor = NULL;
		}

		if (CurrentCursorID != NULL)
		{
			CurrentCursor = new Cursor(CurrentCursorID);
			if (CurrentCursor != NULL)
				CurrentCursor->SetActive();
		}
		else
		{
 			if (Cursor::Arrow != NULL)
				Cursor::Arrow->SetActive();
		}
	}
*/
	return(Result);
}



/********************************************************************************************

>	void ColourEditDlg::SetCursor(UINT32 CursorResID)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/10/95
	Inputs:		CursorResID - NULL (arrow pointer), or a resource ID that specifies a cursor

	Purpose:	Called by HandleIdlePointerXXX functions to set the cursor shape they desire.


	Notes:		The base HandleIdlePointer function always resets the desired cursor ID to
				a standard Arrow cursor before calling the picker-specific code. If the
				called function does not set a cursor, then it will immediately revert.

				This function does *not* set the cursor, just remembers what cursor is wanted
				(Cursor setting is done in HandleIdlepointer)

********************************************************************************************/

void ColourEditDlg::SetCursor(UINT32 CursorResID)
{
//	CurrentCursorID = CursorResID;

//TRACEUSER( "Jason", _T("Cursor=%ld\n"), CursorResID);
}



/********************************************************************************************

>	BOOL ColourEditDlg::HandleIdlePointerHSV(ReDrawInfoType *Info, String_128 *BubbleHelp,
											String_256 *StatusHelp)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/10/95
	Inputs:		Info - Kernel-rendered gadget info indicating where the mouse pointer is
				over the colour picker (_R(IDC_EDIT_PICKER)) control

	Outputs:	BubbleHelp - Returned containing the new bubble help string to use for this
				mouse position.

				StatusHelp - Returned containing appropriate status-line help.

	Returns:	TRUE if the bubble help is valid, FALSE if there is no new bubble help

	Purpose:	Called when the mouse is idling over the colour picker control.
				Sets appropriate pointer shapes, and also returns bubble help for
				appropriate regions (probably only the original/current colour patches)

********************************************************************************************/

BOOL ColourEditDlg::HandleIdlePointerHSV(ReDrawInfoType *Info, String_128 *BubbleHelp, String_256 *StatusHelp)
{
	if (EditingColour->GetType() != COLOURTYPE_LINKED)
		return(FALSE);

	INT32 PixelSize = 72000 / Info->Dpi;			// Size of output pixel in millipoints
	DocRect VirtualSize(0, 0, Info->dx, Info->dy);
	VirtualSize.Inflate(-PixelSize * 4);		// And exclude the border

	// Calculate the two important rectangles - the hue slider, and Val/Sat square
	DocRect HueRect;
	DocRect ValSatSquare;
	CalculateHSVPickerRects(&VirtualSize, PixelSize, &HueRect, &ValSatSquare);

	if (HueRect.ContainsCoord(*(Info->pMousePos)) && EditingColour->InheritsComponent(1))
	{
		// SetCursor(No_can_drag_me_mate);
		StatusHelp->MakeMsg(_R(IDS_K_COLDLOG_NOHUECHANGE));
	}
	else if (ValSatSquare.ContainsCoord(*(Info->pMousePos)))
	{
		if (EditingColour->InheritsComponent(2))
		{
			if (EditingColour->InheritsComponent(3))
				StatusHelp->MakeMsg(_R(IDS_K_COLDLOG_NOSATVALCHANGE));
			else
				StatusHelp->MakeMsg(_R(IDS_K_COLDLOG_NOSATCHANGE));
		}
		else if (EditingColour->InheritsComponent(3))
		{
			StatusHelp->MakeMsg(_R(IDS_K_COLDLOG_NOVALCHANGE));
		}
	}

	return(StatusHelp->IsEmpty());
}



/********************************************************************************************

>	BOOL ColourEditDlg::HandleIdlePointerRGB(ReDrawInfoType *Info, String_128 *BubbleHelp,
											String_256 *StatusHelp)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/10/95
	Inputs:		Info - Kernel-rendered gadget info indicating where the mouse pointer is
				over the colour picker (_R(IDC_EDIT_PICKER)) control

	Outputs:	BubbleHelp - Returned containing the new bubble help string to use for this
				mouse position.

				StatusHelp - Returned containing appropriate status-line help.

	Returns:	TRUE if the bubble help is valid, FALSE if there is no new bubble help

	Purpose:	Called when the mouse is idling over the colour picker control.
				Sets appropriate pointer shapes, and also returns bubble help for
				appropriate regions (probably only the original/current colour patches)

********************************************************************************************/

BOOL ColourEditDlg::HandleIdlePointerRGB(ReDrawInfoType *Info, String_128 *BubbleHelp, String_256 *StatusHelp)
{
	INT32 PixelSize = 72000 / Info->Dpi;			// Size of output pixel in millipoints

	// Fill the control background, and calculate the cube params (without redraw)
	DocRect VirtualSize(0, 0, Info->dx, Info->dy);
	VirtualSize.Inflate(-PixelSize * 4);		// And exclude the border

	DocRect SquareRect;
	INT32 SizeZ;
	DrawCubeShadowAndCalcValues(NULL, &VirtualSize, PixelSize, NULL,
								/* TO */ &SquareRect, &SizeZ);

	DocRect CubeFaceRect(SquareRect);		// Remember the rect of the front face for below

	// Now, shift the square down from the front of the cube by an amount appropriate to
	// the Z-axis component of the colour
	ColourContextRGBT *cc = (ColourContextRGBT *)
							ColourContext::GetGlobalDefault(COLOURMODEL_RGBT);
	ColourRGBT SourceColour;
	if (EditingColour != NULL)
		cc->ConvertColour(EditingColour, (ColourGeneric *) &SourceColour);

	if (CanSetColour(EditingColour))
	{
		FIXED24 ZComponent = SourceColour.Blue;
		if (ColourPickerMode == 1)
			ZComponent = SourceColour.Green;
		else if (ColourPickerMode == 2)
			ZComponent = SourceColour.Red;

		INT32 ZTrans = (INT32) ((double)SizeZ * ZComponent.MakeDouble()) - SizeZ;
		SquareRect.Translate(-ZTrans, ZTrans);
		GridLockRect(&SquareRect, PixelSize);
	}

	DocCoord BitmapPos;
	BitmapPos.x = HalfGridLock(CubeFaceRect.hi.x + SizeZ/2, PixelSize);
	BitmapPos.y = HalfGridLock(CubeFaceRect.hi.y - SizeZ/2, PixelSize);

	const INT32 BitmapSize = 14 * PixelSize;
	DocRect SwapAxesRect(BitmapPos.x, BitmapPos.y,
							BitmapPos.x + BitmapSize + PixelSize * 3, BitmapPos.y + BitmapSize);
	GridLockRect(&SwapAxesRect, PixelSize);

	if (SwapAxesRect.ContainsCoord(*(Info->pMousePos)))
	{
		StatusHelp->MakeMsg(_R(IDS_K_COLDLOG_CLICKTOCYCLE));
		return(TRUE);
	}

	if (EditingColour->GetType() != COLOURTYPE_LINKED)
		return(FALSE);

	if (SquareRect.ContainsCoord(*(Info->pMousePos)))
	{
		INT32 C1 = 1;
		INT32 C2 = 2;
		if (ColourPickerMode == 1)
			C1 = 3;
		else if (ColourPickerMode == 2)
			C2 = 3;

		INT32 Count = 0;

		if (EditingColour->InheritsComponent(C1))
			Count++;

		if (EditingColour->InheritsComponent(C2))
			Count++;

		if (Count == 1)
			StatusHelp->MakeMsg(_R(IDS_K_COLDLOG_DRAGINONEDIR));
		else if (Count == 2)
			StatusHelp->MakeMsg(_R(IDS_K_COLDLOG_CANTDRAG));
	}
	else
	{
		// Check if the pointer is in the Z 'drag button'
		DocRect ZButton(SquareRect);
		ZButton.lo.x = ZButton.hi.x;
		ZButton.hi.x += ZSLIDERSIZE - (PixelSize * 2);
		ZButton.hi.y = ZButton.lo.y;
		ZButton.lo.y -= ZSLIDERSIZE - (PixelSize * 2);
		GridLockRect(&ZButton, PixelSize);

		if (ZButton.ContainsCoord(*(Info->pMousePos)))
		{
			INT32 ComponentID = ColourPickerMode;
			if (ComponentID == 0)
				ComponentID = 3;

			if (EditingColour->InheritsComponent(ComponentID))
				StatusHelp->MakeMsg(_R(IDS_K_COLDLOG_CANTDRAG2));
		}
	}

	return(StatusHelp->IsEmpty());
}



/********************************************************************************************

>	BOOL ColourEditDlg::HandleIdlePointerCMYK(ReDrawInfoType *Info, String_128 *BubbleHelp,
											String_256 *StatusHelp)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/10/95
	Inputs:		Info - Kernel-rendered gadget info indicating where the mouse pointer is
				over the colour picker (_R(IDC_EDIT_PICKER)) control

	Outputs:	BubbleHelp - Returned containing the new bubble help string to use for this
				mouse position.

				StatusHelp - Returned containing appropriate status-line help.

	Returns:	TRUE if the bubble help is valid, FALSE if there is no new bubble help

	Purpose:	Called when the mouse is idling over the colour picker control.
				Sets appropriate pointer shapes, and also returns bubble help for
				appropriate regions (probably only the original/current colour patches)

********************************************************************************************/

BOOL ColourEditDlg::HandleIdlePointerCMYK(ReDrawInfoType *Info, String_128 *BubbleHelp, String_256 *StatusHelp)
{
	INT32 PixelSize = 72000 / Info->Dpi;			// Size of output pixel in millipoints

	DocRect VirtualSize(0, 0, Info->dx, Info->dy);
	VirtualSize.Inflate(-PixelSize * 4);		// And exclude the border

	// Determine how much space is left over after the key slider has been placed
	INT32 KeySliderLeft = VirtualSize.hi.x - (CROSSRADIUS+PixelSize)*2;
	ERROR3IF(KeySliderLeft < VirtualSize.lo.x,
				"Not enough room to render the colour picker!");

	// Fill the control background, and draw the cube 'shadow'
	DocRect CubeAvailableSpace(VirtualSize);
	CubeAvailableSpace.hi.x = KeySliderLeft - 2000;

	DocRect SquareRect;
	INT32 SizeZ;
	DrawCubeShadowAndCalcValues(NULL, &CubeAvailableSpace, PixelSize, NULL,
								 &SquareRect, &SizeZ);

	DocRect CubeFaceRect(SquareRect);		// Remember the rect of the front face for below


	// Now, shift the square down from the front of the cube by an amount appropriate to
	// the Z-axis component of the colour
//	ColourContextCMYK *cc = (ColourContextCMYK *)ColourContext::GetGlobalDefault(COLOURMODEL_CMYK);
	ColourContext *cc = NULL;
	BOOL bDeleteCC = GetColourContext(DisplayModel, &cc);	
	ColourCMYK SourceColour;

	if (EditingColour != NULL)
		cc->ConvertColour(EditingColour, (ColourGeneric *) &SourceColour);

	// Delete the colour context if necessary
	if (bDeleteCC)
		ColourContextList::GetList()->RemoveContext(&cc);			// Have finished with it

	if (CanSetColour(EditingColour))
	{
		FIXED24 ZComponent = SourceColour.Magenta;
		if (ColourPickerMode == 1)
			ZComponent = SourceColour.Cyan;
		else if (ColourPickerMode == 2)
			ZComponent = SourceColour.Yellow;

		INT32 ZTrans = (INT32) ((double)SizeZ * ZComponent.MakeDouble()) - SizeZ;
		SquareRect.Translate(-ZTrans, ZTrans);
		GridLockRect(&SquareRect, PixelSize);
	}

	DocCoord BitmapPos;
	BitmapPos.x = HalfGridLock(CubeFaceRect.hi.x + SizeZ/2, PixelSize);
	BitmapPos.y = HalfGridLock(CubeFaceRect.hi.y - SizeZ/2, PixelSize);

	const INT32 BitmapSize = 14 * PixelSize;
	DocRect SwapAxesRect(BitmapPos.x, BitmapPos.y,
							BitmapPos.x + BitmapSize + PixelSize * 3, BitmapPos.y + BitmapSize);
	GridLockRect(&SwapAxesRect, PixelSize);

	if (SwapAxesRect.ContainsCoord(*(Info->pMousePos)))
	{
		StatusHelp->MakeMsg(_R(IDS_K_COLDLOG_CLICKTOCYCLE2));
		return(TRUE);
	}

	if (EditingColour->GetType() != COLOURTYPE_LINKED)
		return(FALSE);

	if (SquareRect.ContainsCoord(*(Info->pMousePos)))
	{
		INT32 C1 = 1;
		INT32 C2 = 3;
		if (ColourPickerMode == 1)
			C1 = 2;
		else if (ColourPickerMode == 2)
			C2 = 2;

		INT32 Count = 0;

		if (EditingColour->InheritsComponent(C1))
			Count++;

		if (EditingColour->InheritsComponent(C2))
			Count++;

		if (Count == 1)
			StatusHelp->MakeMsg(_R(IDS_K_COLDLOG_DRAGINONEDIR));
		else if (Count == 2)
			StatusHelp->MakeMsg(_R(IDS_K_COLDLOG_CANTDRAG));
	}
	else
	{
		// Check if the pointer is in the Z 'drag button'
		DocRect ZButton(SquareRect);
		ZButton.lo.x = ZButton.hi.x;
		ZButton.hi.x += ZSLIDERSIZE - (PixelSize * 2);
		ZButton.hi.y = ZButton.lo.y;
		ZButton.lo.y -= ZSLIDERSIZE - (PixelSize * 2);
		GridLockRect(&ZButton, PixelSize);

		if (ZButton.ContainsCoord(*(Info->pMousePos)))
		{
			// How on earth did all of this get so out of hand?!
			INT32 ComponentID = 2;
			if (ColourPickerMode == 1)
				ComponentID = 1;
			else if (ColourPickerMode == 2)
				ComponentID = 3;

			if (EditingColour->InheritsComponent(ComponentID))
				StatusHelp->MakeMsg(_R(IDS_K_COLDLOG_CANTDRAG2));
		}
		else
		{
			// Check if the pointer is in the Key slider
			DocRect KeyRect(KeySliderLeft, 0, Info->dx, Info->dy - (PATCHSIZE + PixelSize*2));
			if (KeyRect.ContainsCoord(*(Info->pMousePos)))
			{
				if (EditingColour->InheritsComponent(4))
					StatusHelp->MakeMsg(_R(IDS_K_COLDLOG_CANTDRAGKEY));
			}
		}
	}

	return(StatusHelp->IsEmpty());
}



/********************************************************************************************

>	BOOL ColourEditDlg::HandleIdlePointerDefault(ReDrawInfoType *Info, String_128 *BubbleHelp,
													String_256 *StatusHelp)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/10/95
	Inputs:		Info - Kernel-rendered gadget info indicating where the mouse pointer is
				over the colour picker (_R(IDC_EDIT_PICKER)) control

	Outputs:	BubbleHelp - Returned containing the new bubble help string to use for this
				mouse position.

				StatusHelp - Returned containing appropriate status-line help.

	Returns:	TRUE if the bubble help is valid, FALSE if there is no new bubble help

	Purpose:	Called when the mouse is idling over the colour picker control.
				Sets appropriate pointer shapes, and also returns bubble help for
				appropriate regions (probably only the original/current colour patches)

********************************************************************************************/

BOOL ColourEditDlg::HandleIdlePointerDefault(ReDrawInfoType *Info, String_128 *BubbleHelp, String_256 *StatusHelp)
{
	if (EditingColour->GetType() != COLOURTYPE_LINKED)
		return(FALSE);

//	ColourContext *cc = ColourContext::GetGlobalDefault(DisplayModel);
	ColourContext *cc = NULL;
	BOOL bDeleteCC = GetColourContext(DisplayModel, &cc);	
	if (cc == NULL)
		return(FALSE);

	// Get the slider rectangle widths. The height is fixed/moved during the loop below
	INT32 PixelSize = 72000 / Info->Dpi;					// Size of output pixel in millipoints
	DocRect SliderRect(0, 0, Info->dx, Info->dy);
	SliderRect.hi.y -= PATCHSIZE + (PixelSize * 2);		// Allow space for the current colour patch

	// Count how many components we have to display
	INT32 NumComponents = 0;
	INT32 ComponentIndex;
	for (ComponentIndex = 1; ComponentIndex <= 4; ComponentIndex++)
	{
		if (cc->GetComponentName(ComponentIndex, NULL))
			NumComponents++;
	}

	// Calculate slider sizes and spacing
	INT32 SliderHeight = GetSliderHeight(SliderRect.Height(), NumComponents);
	INT32 SliderGap = GetSliderGap(SliderRect.Height(), NumComponents);

	// And move the top down by half a SliderGap, so the sliders are centered vertically
	SliderRect.hi.y -= SliderGap / 2;

	// Check the slider for each component supplied in the current DisplayModel
	for (ComponentIndex = 0; ComponentIndex <= 3; ComponentIndex++)
	{
		// Ensure slider rect is the correct height
		SliderRect.lo.y = SliderRect.hi.y - SliderHeight;

		// If this component is available/used in this colour model, see if we are dragging it
		if (cc->GetComponentName(ComponentIndex+1, NULL))
		{
			if (SliderRect.ContainsCoord(*(Info->pMousePos)))
			{
				if (EditingColour->InheritsComponent(ComponentIndex+1))
					StatusHelp->MakeMsg(_R(IDS_K_COLDLOG_CANTDRAG2));
				return(StatusHelp->IsEmpty());
			}
		}

		// Move down to the next slider rectangle position
		SliderRect.hi.y = SliderRect.lo.y - SliderGap;
	}

	// Delete the colour context if necessary
	if (bDeleteCC)
		ColourContextList::GetList()->RemoveContext(&cc);			// Have finished with it

	return(FALSE);
}



/********************************************************************************************

>	BOOL ColourEditDlg::HandleIdlePointerTint(ReDrawInfoType *Info, String_128 *BubbleHelp,
											String_256 *StatusHelp)


	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/10/95
	Inputs:		Info - Kernel-rendered gadget info indicating where the mouse pointer is
				over the colour picker (_R(IDC_EDIT_PICKER)) control

	Outputs:	BubbleHelp - Returned containing the new bubble help string to use for this
				mouse position.

				StatusHelp - Returned containing appropriate status-line help.

	Returns:	TRUE if the bubble help is valid, FALSE if there is no new bubble help

	Purpose:	Called when the mouse is idling over the colour picker control.
				Sets appropriate pointer shapes, and also returns bubble help for
				appropriate regions (probably only the original/current colour patches)

********************************************************************************************/

BOOL ColourEditDlg::HandleIdlePointerTint(ReDrawInfoType *Info, String_128 *BubbleHelp, String_256 *StatusHelp)
{
	return(FALSE);
}



/********************************************************************************************

>	BOOL ColourEditDlg::HandleIdlePointerShade(ReDrawInfoType *Info, String_128 *BubbleHelp,
											String_256 *StatusHelp)


	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/10/95
	Inputs:		Info - Kernel-rendered gadget info indicating where the mouse pointer is
				over the colour picker (_R(IDC_EDIT_PICKER)) control

	Outputs:	BubbleHelp - Returned containing the new bubble help string to use for this
				mouse position.

	Returns:	TRUE if the bubble help is valid, FALSE if there is no new bubble help

				StatusHelp - Returned containing appropriate status-line help.

	Purpose:	Called when the mouse is idling over the colour picker control.
				Sets appropriate pointer shapes, and also returns bubble help for
				appropriate regions (probably only the original/current colour patches)

********************************************************************************************/

BOOL ColourEditDlg::HandleIdlePointerShade(ReDrawInfoType *Info, String_128 *BubbleHelp, String_256 *StatusHelp)
{
/*
	DocRect VirtualSize(0, 0, Info->dx, Info->dy);
	VirtualSize.Inflate(-PixelSize * 4);		// And exclude the border

	// Calculate the two important rectangles - the hue slider, and Val/Sat square
	DocRect HueRect;
	DocRect ValSatSquare;
	CalculateHSVPickerRects(&VirtualSize, PixelSize, &HueRect, &ValSatSquare);

	if (!ValSatSquare.ContainsCoord(*(Info->pMousePos)))
		return;

	*/

	return(FALSE);
}





/********************************************************************************************

>	OpState	ColourEditDlg::GetState(String_256*, OpDescriptor*)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/6/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Get the state of the Colour editor dialogue op
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

OpState	ColourEditDlg::GetState(String_256*, OpDescriptor*)
{    
	OpState OpSt;

	// Tick the menu while the editor is open
	if (TheEditor != NULL)
		OpSt.Ticked = TRUE;

	return(OpSt);
}



/********************************************************************************************

>	static BOOL ColourEditDlg::Init()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/6/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Initialises the colour editor dialogue op
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL ColourEditDlg::Init()
{
	Camelot.DeclareSection(TEXT("Displays"), 9);
	Camelot.DeclarePref(TEXT("Displays"), TEXT("ColourEditorDisplayModel"), &DefaultDisplayModel);
	Camelot.DeclarePref(TEXT("Displays"), TEXT("ColourEditorDisplayModelS"), &DefaultDisplayModelN);
	Camelot.DeclarePref(TEXT("Displays"), TEXT("ColourEditorAutoModel"), &AutoModelChange);
	Camelot.DeclarePref(TEXT("Displays"), TEXT("ColourEditorAutoModelS"), &AutoModelChangeN);
	Camelot.DeclarePref(TEXT("Displays"), TEXT("ColourEditorFolded"), &Folded);
	Camelot.DeclarePref(TEXT("Displays"), TEXT("ColourEditorSplitLine"), &SplitLineDisplay);
	Camelot.DeclarePref(TEXT("Displays"), TEXT("ColourEditorUse3D"), &Use3DDisplay);
	Camelot.DeclarePref(TEXT("Displays"), TEXT("ColourEditorHSVHueTop"), &bHSVHueAtTop);
	Camelot.DeclarePref(TEXT("Displays"), TEXT("UsePrintCMYK"), &bUsePrintCMYK);

	DisplayModel = (ColourModel) DefaultDisplayModel;
	ColourContext *DefCC = NULL;
	BOOL bDeleteCC = GetColourContext(DisplayModel, &DefCC);
	if (DefCC == NULL)
	{
		DisplayModel = COLOURMODEL_HSVT;
		DefaultDisplayModel = (INT32) COLOURMODEL_HSVT;
	}
	// Delete the colour context if necessary
	if (bDeleteCC)
		ColourContextList::GetList()->RemoveContext(&DefCC);			// Have finished with it

	bDeleteCC = GetColourContext((ColourModel)DefaultDisplayModelN, &DefCC);
	if (DefCC == NULL)
		DefaultDisplayModelN = (INT32) COLOURMODEL_HSVT;

	// Delete the colour context if necessary
	if (bDeleteCC)
		ColourContextList::GetList()->RemoveContext(&DefCC);			// Have finished with it

#ifdef DISABLE_WEBHEXRGBT
	if (DisplayModel==COLOURMODEL_WEBHEXRGBT)
		DisplayModel = COLOURMODEL_RGBT;
#endif

	// Initialise all of our menu command Ops
	if (!OpColEditCommand::Init())
		return(FALSE);

	// And initialise our own op
	return(RegisterOpDescriptor(0,									// Tool ID 
								_R(IDS_COLOUREDITDLG), 					// String resource ID
								CC_RUNTIME_CLASS(ColourEditDlg),	// Runtime class
								OPTOKEN_COLOUREDITDLG, 				// Token string
								ColourEditDlg::GetState,			// GetState function
								0,									// help ID
								_R(IDBBL_COLOUREDITOR),					// bubble help
								_R(IDD_BARCONTROLSTORE),				// resource ID
								_R(IDC_COLOUREDITOR),					// control ID
								SYSTEMBAR_UTILITIES,				// Bar ID
								TRUE,								// Recieve system messages
								FALSE,								// Smart duplicate operation
								TRUE,								// Clean operation
								0,									// No vertical counterpart
								0,									// String for one copy only error
								DONT_GREY_WHEN_SELECT_INSIDE,		// Auto state flags
								TRUE								// Tickable
								));
}



/********************************************************************************************

>	BOOL ColourEditDlg::Create()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/6/94
	Inputs:		-
	Outputs:	-
	Returns:	FALSE if it fails
	Purpose:	Creates a colour editor dialogue box. Fails if DialogOp::Create
				fails to create the box.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL ColourEditDlg::Create()
{
	ISentTheMessage = FALSE;	// Ensure our message lock is disabled
	AmShaded 		= FALSE;	// We are not currently in a shaded state
	m_bDoTimerProcessing = TRUE;
	if (TheEditor != this)		// Allow only one instance of this dialogue open at once
		return(FALSE);


	if (EditingColour != NULL && ParentList == NULL)
	{
		ERROR3("ColourEditDlg not correctly initialised before call to Create()");
		return(FALSE);
	}

	if (DialogOp::Create())
	{ 
//		SetControls();		-- now done by DIM_CREATE handler
		return(TRUE);
	}

	return(FALSE);
}



/********************************************************************************************

>	void ColourEditDlg::Do(OpDescriptor*)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/6/94

	Purpose:	Toggles the colour editor, and if opening it, finds a useful colour to edit

	SeeAlso:	ColourEditDlg::DoWithParam

********************************************************************************************/

void ColourEditDlg::Do(OpDescriptor*)
{
	if (TheEditor != NULL && TheEditor != this)
	{
		// If there is an editor open, just ask it to die then kill ourself off, as
		// we don't want another editor to then pop up!
		TheEditor->Close();
		TheEditor->End();	// Kill existing editor

		End();				// Kill ourself
		return;
	}


	if (EditingColour != NULL)
	{
		delete EditingColour;
		EditingColour = NULL;
	}
	
	// WEBSTER - markn 31/1/97
	// Bodge - uses the Named default value to keep that model preserved
#ifndef WEBSTER
	DisplayModel = (ColourModel) DefaultDisplayModel;
#else
	DisplayModel = (ColourModel) DefaultDisplayModelN;
#endif // WEBSTER

	FindUsefulColourToEdit(EditingLineColour, FALSE);

	Progress Hourglass;		// Start an hourglass running

	if (Create())
	{
		Open();								// Open the dialogue (& set the controls etc)
		PaintGadgetNow(_R(IDC_EDIT_PICKER));	// Redraw picker while hourglass is still up
	}
	else
		End();
}



/********************************************************************************************

>	void ColourEditDlg::DoWithParam(OpDescriptor *MyOpDesc, OpParam *Param)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/6/94
	Inputs:		MyOpDesc - as usual, the opdescriptor of this thingo
				Param - points to a ColourEditDlgParam object
	Outputs:	-
	Returns:	-
	Purpose:	'Does' a colour editor dialogue op. Shows the dialogue.
	Scope:		private
	Errors:		-
	SeeAlso:	ColourEditDlgParam::ColourEditDlgParam

********************************************************************************************/

void ColourEditDlg::DoWithParam(OpDescriptor *MyOpDedsc, OpParam *Param)
{
	ERROR3IF(Param == NULL, "ColourEditDlg::DoWithParam - NULL Param passed in");
	ERROR3IF(!Param->IsKindOf(CC_RUNTIME_CLASS(ColourEditDlgParam)),
				"ColourEditDlg::DoWithParam - Param is illegal");

	// Initialise ourselves from the supplied parameters
	ColourEditDlgParam *Info = (ColourEditDlgParam *) Param;

	if (Info->ParentList == NULL)
		Info->ParentList = ColourManager::GetColourList();

	// Determine if we can safely edit this colour
	// NOTE that the TRUE is used to cause side effects on the EditingLineColour flag,
	// and we will always edit a colour, even if this function returns FALSE. (But if
	// it is FALSE, we will edit a "useful" colour)
	if (!CanYouEditThis(Info->ParentList, Info->ColourToEdit, TRUE))
		Info->ColourToEdit = NULL;	// You can't do that! Choose a "useful" colour to edit instead

	if (TheEditor != NULL && TheEditor != this)
	{
		// If there is an editor open, bring it to the top of the window stack, and ask it
		// to swap to the ColourToEdit, or a useful local colour if ColourToEdit is NULL
		TheEditor->BringToTop();

		if (Info->ColourToEdit == NULL)
			TheEditor->FindUsefulColourToEdit(TheEditor->EditingLineColour, TRUE);
		else
			TheEditor->EditThisColour(Info->ParentList, Info->ColourToEdit);

		// And kill ourself off, as we don't want multiple instantiations of the editor open
		End();
		return;
	}

	if (Document::GetSelected() == NULL ||
		(Info->ParentList != NULL && Document::GetSelected()->GetIndexedColours() != Info->ParentList))
	{
		ERROR3("Colour editor: No selected doc or Colour not in selected doc");
		End();
		return;
	}

	// Check that the colour given really truly is in the parent list, but only if we've got a colour/list
	if (Info->ColourToEdit != NULL && Info->ParentList != NULL &&															// Have a valid pointer
			Info->ParentList->FindPosition(Info->ColourToEdit) < 0 &&						// Isn't in named colours
			Info->ParentList->GetUnnamedColours()->FindPosition(Info->ColourToEdit) < 0 )	// Isn't in unnamed colours
	{
		ERROR3("Colour editor: attempt to edit colour with bogus parent list pointer");
		End();
		return;
	}

	ParentList		= Info->ParentList;					// The list in which the edited colour resides
	ResultColour	= Info->ColourToEdit;				// The colour to recieve the result

	if (EditingColour != NULL)
	{
		delete EditingColour;
		EditingColour = NULL;
	}
	
	if (ResultColour != NULL)
	{
		FirstEdit = TRUE;
		EditingColour	= new IndexedColour(*ResultColour);	// The colour we're working on for now
			
		if (EditingColour == NULL)
		{
			InformError();
			End();
			return;
		}

		// Copy the original colour into the OriginalColour.
		// We only need the colour definition, but we do need the full accuracy of an
		// IndexedColour to ensure rounding errors (and thus slight dither differences in
		// redraw) do not occur.
		OriginalColour = *ResultColour;

		// Ensure the OriginalColour does not reference any other colour
		OriginalColour.SetLinkedParent(NULL, COLOURTYPE_NORMAL);
	}
	else
	{
		FindUsefulColourToEdit(EditingLineColour, FALSE);
	}

	if (EditingColour != NULL)
	{
		StatusLine* pStatusLine = StatusLine::Get();
		
		if (EditingColour->IsNamed())
		{
			if (!pStatusLine || (pStatusLine->IsRestrictedAccessToColourPicker () == FALSE))
			{
				if (AutoModelChangeN)
				{
					DisplayModel = OriginalColour.GetColourModel();
					// WEBSTER - markn 14/1/97
					// Make sure the display model is either HSV or RGB
					// If it's neither RGB or HSV, force it to HSV
					#ifdef WEBSTER
					if (DisplayModel != COLOURMODEL_RGBT && DisplayModel != COLOURMODEL_HSVT)
						DisplayModel = COLOURMODEL_HSVT;
					#endif // WEBSTER
#ifdef DISABLE_WEBHEXRGBT
					if (DisplayModel == COLOURMODEL_WEBHEXRGBT)
						DisplayModel == COLOURMODEL_RGBT;
#endif
				}
			}
		}
		else
		{
			if (pStatusLine->IsRestrictedAccessToColourPicker () == FALSE)
			{
				if (AutoModelChange)
					DisplayModel = OriginalColour.GetColourModel();
#ifdef DISABLE_WEBHEXRGBT
					if (DisplayModel == COLOURMODEL_WEBHEXRGBT)
						DisplayModel == COLOURMODEL_RGBT;
#endif
			}
		}
	}

	Progress Hourglass;			// Start an hourglass running

	if (Create())
	{
		Open();								// Open the dialogue (& set the controls etc)
		PaintGadgetNow(_R(IDC_EDIT_PICKER));	// Redraw picker while hourglass is still up
	}
	else
		End();
}



/********************************************************************************************

>	static void ColourEditDlg::InvokeDialog(ColourList *ParentList,
											IndexedColour *ColourToEdit)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/6/94
	Inputs:		ParentList - the ColourList in which the colour to be edited resides
				May be NULL to use the Selected Document's colour list

				ColourToEdit - the IndexedColour which you wish to edit. May be NULL
				in which case the editor will try to find a useful local colour to
				edit, or shade itself.

	Purpose:	Causes a modeless colour editor to appear for the given colour. Some time
				down the track, if the user commits a change to that colour, the system
				will be notified of that change via a ColourChangingMsg broadcast.

	SeeAlso:	ColourEditDlg::CanYouEditThis; ColourPicker::EditColour

********************************************************************************************/

void ColourEditDlg::InvokeDialog(ColourList *ParentList, IndexedColour *ColourToEdit)
{	
	if (ParentList == NULL)
		ParentList = ColourManager::GetColourList();

	// Determine if we can safely edit this colour
	// NOTE that the TRUE is used to cause side effects on the EditingLineColour flag,
	// and we will always edit a colour, even if this function returns FALSE. (But if
	// it is FALSE, we will edit a "useful" colour)
	if (!CanYouEditThis(ParentList, ColourToEdit, TRUE))
		ColourToEdit = NULL;	// You can't do that! Choose a "useful" colour to edit instead

	if (TheEditor != NULL)
	{
		// If there is an editor open, bring it to the top of the window stack, and ask it
		// to swap to the ColourToEdit, or a useful local colour if ColourToEdit is NULL
		TheEditor->BringToTop();

		if (ColourToEdit == NULL)
			TheEditor->FindUsefulColourToEdit(TheEditor->EditingLineColour, TRUE);
		else
			TheEditor->EditThisColour(ParentList, ColourToEdit);
		return;
	}

	// need to set/reset static variables that control where my (as in Chris Snook) custom
	// colour picker control appears .....

	resetColPickPos = FALSE;
	needColPickHidden = FALSE;
	colPickHidden = FALSE;

	// Otherwise, invoke the dialogue to bring it up
	ColourEditDlgParam EditInfo(ParentList, ColourToEdit);
	OpDescriptor *EditDlg = OpDescriptor::FindOpDescriptor(CC_RUNTIME_CLASS(ColourEditDlg));

	ERROR3IF(EditDlg == NULL,
			 "ColourEditDlg::InvokeDialog is unable to find the ColourEditDlg OpDescriptor");

	if (EditDlg != NULL)
		EditDlg->Invoke(&EditInfo);
}



/********************************************************************************************

>	static BOOL ColourEditDlg::CanYouEditThis(ColourList *ParentList, IndexedColour *ColourToEdit,
												BOOL ForceNewMode = FALSE)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/3/96

	Inputs:		ParentList - the ColourList in which the colour to be edited resides
				May be NULL to use the Selected Document's colour list

				ColourToEdit - the IndexedColour which you wish to edit. May be NULL
				if you just want it to find a useful local colour to edit, in which
				case it will return TRUE unless there are no documents open.

				ForceNewMode - DO NOT USE - this is an internal flag causing side effects
				(if TRUE, and the colour is local, the return value will be FALSE, and
				the EditingLineColour member flag will be changed to indicate which
				colour should be edited - see the InvokeDialog code)

	Returns:	TRUE if the colour can be edited. If this returns FALSE, then a call
				to InvokeDialog with these parameters will fail (specifically, it will
				revert to editing a "useful" local colour)

	Purpose:	Determines if the colour editor will be happy with you calling
				InvokeDialog with the given parameters. Allows you to enable/disable
				any UI mechanism that links onto this method. (e.g. colour line
				edit colour button uses this to determine if drags can be dropped 
				onto it)

	Notes:		The colour must be in the ParentList
				Named colours in the ParentList can always be edited
				Unnamed colours can only be edited if they happen to coincide with
				the current line or fill colour - otherwise it makes no sense to edit
				a local colour (because you can only edit a local colour _on_ an object)

	SeeAlso:	ColourEditDlg::InvokeDialog

********************************************************************************************/

BOOL ColourEditDlg::CanYouEditThis(ColourList *ParentList, IndexedColour *ColourToEdit,
									BOOL ForceNewMode)
{
	// If no list specified, assume the one in the selected doc
	if (ParentList == NULL)
		ParentList = ColourManager::GetColourList();

	// If they specified no colour, then we need to find a "useful" colour to edit.
	// This is always possible, except when there are no documents (SelDoc == NULL)
	if (ColourToEdit == NULL)
		return(Document::GetSelected() != NULL);

	if (ParentList != NULL)
	{
		if (ColourToEdit->IsNamed())
		{
			// Named colour
			// Check that the colour resides in the given colour list
			if (ParentList->FindPosition(ColourToEdit) >= 0)
				return(TRUE);

			ERROR3("CanYouEditThis - Named IndexedColour not in provided ParentList!");
		}
		else
		{
			// Local colour
			// It is deeply scary to allow local colours to be dragged to the editor.
			// Local colours do not exist on their own - they can only be edited ON
			// an object. The only objects we know of like this are the current selection,
			// so we can only edit local line and fill colours.

			StatusLine* pStatusLine = StatusLine::Get();

			if (!pStatusLine || (pStatusLine->IsRestrictedAccessToColourPicker () == FALSE))
			{
				// Get the current line/fill colours, and see if they match the colour we were asked for
				DocColour DocColourToCheck;

				// Try for current fill colour
				ColourManager::FindColourOfInterestToUser(&DocColourToCheck, NULL, FALSE);
				if (DocColourToCheck.FindParentIndexedColour() == ColourToEdit)
				{
					if (ForceNewMode)
						ForceLineOrFillMode(FALSE);		// If appropriate, set line/fill mode and return FALSE
					else
						return(TRUE);
				}

				// Try for current line colour
				ColourManager::FindColourOfInterestToUser(&DocColourToCheck, NULL, TRUE);
				if (DocColourToCheck.FindParentIndexedColour() == ColourToEdit)
				{
					if (ForceNewMode)
						ForceLineOrFillMode(TRUE);		// If appropriate, set line/fill mode and return FALSE
					else
						return(TRUE);
				}
			}
			else
			{
				// the above code may have been valid BEFORE camelots custom colour picker
				// control, but NOW we need to allow the colour editor to display/edit
				// local index colours ....

				return (TRUE);
			}
		}
	}

	// For whatever reason, we can't allow this to be edited
	return(FALSE);
}



/********************************************************************************************

>	static void ColourEditDlg::ForceLineOrFillMode(BOOL PreferLineColour = FALSE);

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/4/95

	Inputs:		PreferLineColour - TRUE to set the colour to defaulting to editing
				local line colours, FALSE to make it default to editing fill colours

	Purpose:	Sets the editing mode for local colours in the future. Whenever a new
				colour is chosen for editing, this will specify whether a line or
				fill colour should be chosen by preference.

********************************************************************************************/

void ColourEditDlg::ForceLineOrFillMode(BOOL PreferLineColour)
{
	EditingLineColour = PreferLineColour;
}



/********************************************************************************************

>	void ColourEditDlg::ApplyColourNow(IndexedColour *IxColourToApply)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/4/95

	Inputs:		IxColourToApply- the colour to be applied

	Purpose:	Applies the EditingColour to the document selection. Whether named
				or unnamed, it is applied as a new colour attribute, thus changing
				the colour used by the selected objects (rather than just redrawing
				static references to a colour which has been modified)

				Make sure that EditingColour is in either the named or unnamed colour
				list when it is applied, so that the reference is not left pointing
				a something which can be easily deleted out from under it.

********************************************************************************************/

void ColourEditDlg::ApplyColourNow(IndexedColour *IxColourToApply)
{
	// Apply it to the selection as a colour attrib mutator - this will thus affect
	// selected objects through selected grad-fill blobs etc
	NodeAttribute *Attrib = NULL;
	DocColour ColourToApply;
	ColourToApply.MakeRefToIndexedColour(IxColourToApply);
	if (EditingLineColour)
	{
		// Apply it as a Line colour mutator
		Attrib = new AttrStrokeColourChange;
		if (Attrib == NULL)
			return;

		((AttrStrokeColourChange *)Attrib)->SetStartColour(&ColourToApply);
	}
	else
	{
		// Apply it as a fill mutator
		Attrib = new AttrColourChange;
		if (Attrib == NULL)
			return;

		((AttrColourChange *)Attrib)->SetStartColour(&ColourToApply);
	}


	// Apply the attribute. While applying, we lock ourselves so we don't respond to the
	// attribute-changing messages, and also ensure that the attribute manager doesn't
	// ask the user if they want to set the current attribute (if you're editing the current
	// colour, you kindof expect the current colour to change!)
	BOOL OldSentState = ISentTheMessage;
	ISentTheMessage = TRUE;

		BOOL OldAskState = AttributeManager::AskBeforeSettingCurrentAttr;
		AttributeManager::AskBeforeSettingCurrentAttr = FALSE;

			// AttributeSelected knows what to do with a selected attribute
			AttributeManager::AttributeSelected(NULL, Attrib); 

		AttributeManager::AskBeforeSettingCurrentAttr = OldAskState;
	ISentTheMessage = OldSentState;
}



/********************************************************************************************

>	void ColourEditDlg::EditingColourHasChanged(BOOL RedrawControls = TRUE,
												BOOL RedrawPicker   = TRUE,
												BOOL FastTrackPicker = FALSE,
												Node* pSourceNode = NULL)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/6/94
	Inputs:		RedrawControls - TRUE to update the editor controls (name, components etc)
				to show the new values for this colour
				
				RedrawPicker - TRUE if the colour picker area should be redrawn to display
				the change. (NOTE that this determines whether the message broadcast is
				a COLOURUPDATED (TRUE) or a COLOURUPDATEDINVISIBLE (FALSE))

				FastTrackPicker - TRUE if you want the picker section to redraw immediately
				to get instant-effect on the update.

	Returns:	-

	Purpose:	Generic code to handle changes to EditingColour. 

	Notes:		Quietly ignores calls when EditingColour == NULL

********************************************************************************************/

void ColourEditDlg::EditingColourHasChanged(BOOL RedrawControls, BOOL RedrawPicker,
											BOOL FastTrackPicker,
											Node* pSourceNode)
{
	if (EditingColour == NULL)
		return;

	if (RedrawControls)
	{
		SetControls();
		RedrawColourNameList();		// And ensure the colour list redraws its colour patch
	}

	if (RedrawPicker)
	{
		InvalidateGadget(_R(IDC_EDIT_PICKER)); 

		if (FastTrackPicker)
			PaintGadgetNow(_R(IDC_EDIT_PICKER));
	}

	if (EditingColour->IsNamed())
	{
		// Apply the change, generating UNDO. This uses 'EditingColour' for an UNDO
		// record, so we must not touch/delete EditingColour after this call.
		//
		// Thus, during processing of the ChangeColour call, it is highly advisable that
		// EditingColour does not point at the UndoRecord that we passed in, as any
		// re-entrant call to our code could potentially delete EditingColour or apply
		// another ChangeColour with the same UNDO buffer! Bad!
		// 
		// NOTE that we copy UndoRecord into the new EditingColour buffer, because
		// ResultColour is out of date (it will shortly be changed to be the same as
		// the UndoRecord). Obviously, we don't want to suddenly swap the
		// definition back to the previous (undo) state of ResultColour; we want it
		// to effectively stay exactly as it is now.
		IndexedColour *UndoRecord = EditingColour;
		EditingColour = new IndexedColour(*UndoRecord);
		if (EditingColour == NULL)
		InformError();

		// Set the new colour definition, locking out ColourChangingMsgs while it happens
		BOOL OldSentState = ISentTheMessage;
		ISentTheMessage = TRUE;
		ColourManager::ChangeColour(ParentList, UndoRecord, ResultColour, !RedrawPicker);
		ISentTheMessage = OldSentState;
	}
	else
	{
		if (FirstEdit)
		{
			// We have not yet changed this colour, so we must add the unnamed colour to
			// the colour list, and apply it to the selection.

			// Remember the colour to apply, and make a new copy to continue editing
			IndexedColour *IxColourToApply = EditingColour;
			EditingColour = new IndexedColour(*IxColourToApply);
			if (EditingColour == NULL)
				InformError();

			// Add the new colour to the list of unnamed colours
			ParentList->GetUnnamedColours()->AddTail(IxColourToApply);

			// And apply the colour to the selection as a new attribute
			ApplyColourNow(IxColourToApply);

			// And now remember that we've applied it once, and remember the new resultcol
			FirstEdit = FALSE;
			ResultColour = IxColourToApply;

			// Make the colour list redraw the first time we change a local colour.
			// This is necessary because on the first edit, the colour in the list
			// (the physical colour pointer, not just the appearance) is changed.
			RedrawColourNameList();
		}
		else
		{
			// We are changing the same unnamed colour as last time. Rather than
			// applying it to the selection and causing another undo record, we
			// want to poke at the first new unnamed colour we added, which conveniently
			// happens to be pointed at by 'ResultColour'.

			// Copy the new definition into ResultColour
			*ResultColour = *EditingColour;

			// And redraw the affected parts of the document tree
			ColourManager::ColourHasChanged((Document *)ParentList->GetParentDocument(), ParentList, ResultColour);
		}
	}
}



/********************************************************************************************

>	IndexedColour *ColourEditDlg::FindSafeParent(IndexedColour *SuggestedParent,
												 BOOL ChildIsATint)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/6/94
	Inputs:		SuggestedParent - NULL, or a pointer to a suggested parent colour
				ChildIsATint - TRUE if the prospective child colour is (or will be)
				a tint colour. (FALSE if it's normal/spot/shade/linked)

	Returns:	NULL if there is no safe parent, else a pointer to the colour to use.

	Purpose:	Determines a safe parent colour to use for a tint/link colour.
				If the suggested parent will do, it is returned, otherwise the first
				safe parent in the ParentList ColourList will be returned.

	Notes:		A colour is declared safe if it is alive, named, and will not result in a
				circular parent reference chain.

				Quietly ignores calls when EditingColour == NULL

********************************************************************************************/

IndexedColour *ColourEditDlg::FindSafeParent(IndexedColour *SuggestedParent, BOOL ChildIsATint)
{
	if (ParentList == NULL || EditingColour == NULL || ResultColour == NULL)
		return(NULL);

	if (SuggestedParent != NULL)
	{
		// The caller has suggested a colour - is it safe to use it?
		// Conditions are:
		//	Not deleted
		//	Must be a named colour
		//	Must not be a descendant of the ResultColour (avoid circular references)
		//	Spot colours may only have tints as children
		if (SuggestedParent->IsDeleted() || !SuggestedParent->IsNamed() ||
			SuggestedParent->IsADescendantOf(ResultColour))
		{
			SuggestedParent = NULL;
		}
	}

	if (SuggestedParent != NULL)		// SuggestedParent passed the test: let 'em use it
		return(SuggestedParent);

	IndexedColour *Ptr = (IndexedColour *) ParentList->GetHead();
	while (Ptr != NULL)
	{
		if (!Ptr->IsDeleted() && Ptr->IsNamed() && !Ptr->IsADescendantOf(ResultColour))
			return(Ptr);

		Ptr = (IndexedColour *) ParentList->GetNext(Ptr);
	}

	return(NULL);
}



/********************************************************************************************

>	void ColourEditDlg::CompileParentColourList(CGadgetID TargetGadget)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/6/94
	Inputs:		TargetGadget - The list gadget to write the list into

	Purpose:	Sets up the given combo box gadget to provide a list of all the colours
				which can be used as a parent of an IndexedColour.
				This creates the list in the dialogue gadget specified, and selects
				the appropriate parent (of EditingColour) by default.

	Notes:		Quietly ignores calls when EditingColour == NULL

	SeeAlso:	ColourEditDlg::DecodeParentColourListSelection

********************************************************************************************/

void ColourEditDlg::CompileParentColourList(CGadgetID TargetGadget)
{
	if (ParentList == NULL || EditingColour == NULL)
		return;

	if (ParentDropDown != NULL)
		delete ParentDropDown;

	ParentDropDown = new ColourDropDown;

	if (ParentDropDown != NULL)
	{
		if (ParentDropDown->Init(WindowID, TargetGadget))
		{
			if (ParentDropDown->FillInColourList(EditingColour->FindLastLinkedParent(), -1, ResultColour))
				return;

			// NOTE: We could do a SetComboListLength here, except that this causes awful
			// redraws of everything "behind" the combo-list extent. I can't stop it doing this,
			// other than by not setting the list length.
		}
	}

	// We must have failed in order to reach here, so shade the gadget and hang our head in shame
	EnableGadget(TargetGadget, FALSE);
}



/********************************************************************************************

>	IndexedColour *ColourEditDlg::DecodeParentColourListSelection(INT32 Index)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/6/94
	Inputs:		Index - the index of the item in the list gadget which was selected
	Returns:	A pointer to the IndexedColour to be used as the new parent of the
				EditingColour, or NULL if the item wasn't found(!?)

	Purpose:	Shared code used by ColourTint and CoourLink Dlgs to decode selctions
				in the list of colours created by CompileParentColourList

	Notes:		Quietly ignores calls when EditingColour == NULL

	SeeAlso:	ColourEditDlg::CompileParentColourList

|********************************************************************************************/

IndexedColour *ColourEditDlg::DecodeParentColourListSelection(INT32 Index)
{
	if (ParentList == NULL || EditingColour == NULL)
		return(NULL);

	if (ParentDropDown != NULL)
		return(ParentDropDown->DecodeSelection(Index));

/*
	IndexedColour *Ptr = (IndexedColour *) ParentList->GetHead();
	while (Ptr != NULL)
	{
		// If this colour is a possible parent:
		//	 * Not deleted
		//	 * Named
		//	 * Not a Spot colour if EditingColour is LINKED
		//	 * Is not a child/descendant of the ResultColour (circular reference! no!)
		//		(Note that this includes it being the ResultColour)
		// then we will add it to the list

		if (!Ptr->IsDeleted() && Ptr->IsNamed() && !Ptr->IsADescendantOf(ResultColour))
		{
			if (EditingColour->GetType() != COLOURTYPE_LINKED || Ptr->GetType() != COLOURTYPE_SPOT)
			{
				Index--;

				if (Index < 0)
					return(Ptr);
			}
		}
		Ptr = (IndexedColour *) ParentList->GetNext(Ptr);
	}
*/
	return(NULL);		// Not found?!
}





BOOL ColourEditDlg::IsSetGadgetValueCalled() const
{
	return m_bDoingSetGadget;
}

BOOL ColourEditDlg::EnteredSetGadgetValue()
{
	m_bDoingSetGadget = TRUE;
	return FALSE;
}

BOOL ColourEditDlg::ExitedSetGadgetValue()
{
	m_bDoingSetGadget = FALSE;
	return TRUE;
}





/********************************************************************************************

>	ColEditorDragTarget::ColEditorDragTarget(CWindowID TheWindow, CRect *ClientArea)
						: WinoilDragTarget(TheWindow, ClientArea)
	 
	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/3/95		  
	Inputs:		TheDilaog	- The window for which this target is active
				ClientArea	- NULL, or the gadget ID of the area in the window to target

	Purpose:	Constructs and registers this drag target with the current drag manager

********************************************************************************************/

ColEditorDragTarget::ColEditorDragTarget(DialogOp *TheDialog, CGadgetID TheGadget)
					: KernelDragTarget(TheDialog, TheGadget)
{
	
}



/********************************************************************************************

>	void ColEditorDragTarget::ProcessEvent(DragEventType Event,
											DragInformation *pDragInfo,
											OilCoord *pMousePos, KeyPress* pKeyPress) 
	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/3/95		  

	Inputs:		Event		- The drag event to be processed
				pDragInfo	- Information describing the current drag
				pMousePos	- The current mouse pos, in client coords
				pKeyPress	- NULL, or keypress event info

	Returns:	TRUE to claim the event (in which case it will not be passed on)

	Purpose:	Event Handler for dragging colours over the colour editor

********************************************************************************************/

BOOL ColEditorDragTarget::ProcessEvent(DragEventType Event,
										DragInformation *pDragInfo,
										OilCoord *pMousePos, KeyPress* pKeyPress)
{
	// Not a colour drag? That is kindof unexpected, but lets exit before
	// we embarrass ourselves
	if (pDragInfo == NULL || !pDragInfo->IsKindOf(CC_RUNTIME_CLASS(ColourDragInformation)))
		return(FALSE);

	ColourDragInformation *ColDragInfo = (ColourDragInformation *) pDragInfo;

	BOOL IsLibColour = ColDragInfo->IsLibraryColour();

	IndexedColour *Col = ColDragInfo->GetInitiallyDraggedColour();
	if (!IsLibColour && Col == NULL)				// Don't allow dropping of "No colour"
		return(FALSE);

	switch(Event)
	{
		case DRAGEVENT_COMPLETED:
			if (ColourEditDlg::TheEditor != NULL && CanDropHere(pDragInfo))
			{
				Document *ParentDoc = NULL;
				if (!IsLibColour)
					ParentDoc = ColDragInfo->GetParentDoc();

				if (ParentDoc == NULL)
					ParentDoc = Document::GetSelected();

				if (ParentDoc != NULL)
				{
					ColourList *ColList = ParentDoc->GetIndexedColours();

					if (IsLibColour)
					{
						Col = NULL;		// Just to be on the safe side

						// We must copy the library colour into the document, but first check with
						// the user that this is what they intended.
						// WEBSTER - markn 30/1/97
						// Don't ask in webster, just copy it over
						#ifndef WEBSTER
						if (InformError(_R(IDE_CANTEDITLIBCOLOUR), _R(IDS_COPYLIBCOLOUR), _R(IDS_CANCEL)) == 1)
							Col = ColDragInfo->GetColourForDocument(ParentDoc);
						#else
							Col = ColDragInfo->GetColourForDocument(ParentDoc);
						#endif // WEBSTER
					}

					if (Col != NULL && ColourEditDlg::CanYouEditThis(ColList, Col))
						ColourEditDlg::InvokeDialog(ColList, Col);
				}

				return(TRUE);
			}
			break;


		case DRAGEVENT_MOUSESTOPPED:
		case DRAGEVENT_MOUSEMOVED:
		case DRAGEVENT_MOUSEIDLE:
			// Return TRUE to claim the mouse while over our target area, so that
			// our cursor shape is used
			return(TRUE);

		default:
			break;
 	}

	// Allow unknown/unwanted events to pass on to other targets
	return(FALSE);
}



/********************************************************************************************

>	void ColEditorDragTarget::GetCursorID()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/1/95

	Purpose:	Base Method to set cursor over this target

********************************************************************************************/

UINT32 ColEditorDragTarget::GetCursorID()
{
	if (CanDropHere(DragManagerOp::GetCurrentDragInfo()))
		return _R(IDC_CANDROP_EDITBUTTON);

	return 0;
}



/********************************************************************************************

>	virtual BOOL ColEditorDragTarget::GetStatusLineText(String_256 * TheText)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/3/95

	Returns:	TRUE if TheText has been filled in with a valid help string

	Purpose:	provide status line text for this target
   
********************************************************************************************/

BOOL ColEditorDragTarget::GetStatusLineText(String_256 * TheText)
{
	ERROR2IF(TheText==NULL,FALSE,"NULL string in GetStatusLineText()");

	if (CanDropHere(DragManagerOp::GetCurrentDragInfo()))
	{
		String_256 DragString(_R(IDS_K_COLDLOG_DROPTOEDIT));
		*TheText = DragString;
		return TRUE;
	}

	return FALSE;
}



/********************************************************************************************

>	BOOL ColEditorDragTarget::CanDropHere(DragInformation *pDragInfo)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/3/96

	Inputs:		pDragInfo - info on the drag (it's safe if this is NULL)

	Returns:	TRUE if we'll accept this dragged thing dropped on us

	Purpose:	To determine if we should accept the given drag on this target

********************************************************************************************/

BOOL ColEditorDragTarget::CanDropHere(DragInformation *pDragInfo)
{
	if (pDragInfo != NULL && pDragInfo->IsKindOf(CC_RUNTIME_CLASS(ColourDragInformation)))
	{
		ColourDragInformation *CDInfo = (ColourDragInformation *) pDragInfo;

		// Library colours can always be edited, as they're copied into the document first
		if (CDInfo->IsLibraryColour())
			return(TRUE);

		// Get the dragged colour and the current colour list
		IndexedColour *Col = CDInfo->GetInitiallyDraggedColour();

		if (Col != NULL)	// If it's not "no colour", see if we're happy to edit it
		{
			ColourList *ColList = NULL;
			if (CDInfo->GetParentDoc() != NULL)
				ColList = CDInfo->GetParentDoc()->GetIndexedColours();

			return(ColourEditDlg::CanYouEditThis(ColList, Col));
		}	
	}

	return(FALSE);
}







/********************************************************************************************

>	ColEditorDragInfo::ColEditorDragInfo(IndexedColour *Colour, BOOL IsAdjust, Document* pParent)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/10/95

	Purpose:	constructor

	Notes:		This is a completely normal ColourDragInformation class, but overridden
				so that we can detect drags from the editor and make sure they can't be
				dropped back into the editor

********************************************************************************************/

ColEditorDragInfo::ColEditorDragInfo()
{
}

ColEditorDragInfo::ColEditorDragInfo(IndexedColour *Colour, BOOL IsAdjust, Document* pParent)
				  : ColourDragInformation(Colour, IsAdjust, pParent)
{
}









/********************************************************************************************

>	OpMakeColourLocalToFrame::OpMakeColourLocalToFrame()

	Author:		Stefan_Stoykov (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/7/97
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	OpMakeColourLocalToFrame constructor (Creates an undoable operation)
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

OpMakeColourLocalToFrame::OpMakeColourLocalToFrame() : UndoableOperation()
{
}



/********************************************************************************************

>	OpMakeColourLocalToFrame::OpMakeColourLocalToFrame()

	Author:		Stefan_Stoykov (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/7/97
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	OpMakeColourLocalToFrame destructor
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

OpMakeColourLocalToFrame::~OpMakeColourLocalToFrame()
{
}



/********************************************************************************************

>	BOOL OpMakeColourLocalToFrame::Init()

	Author:		Stefan_Stoykov (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/7/97
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the operation could be successfully initialised 
				FALSE if no more memory could be allocated 
				
	Purpose:	OpMakeColourLocalToFrame initialiser method

********************************************************************************************/
#define OPTOKEN_MAKECOLOURLOCALTOFRAME _T("MakeColourLocalToFrame")	// Optoken for the operation

BOOL OpMakeColourLocalToFrame::Init()
{
	return (RegisterOpDescriptor(
	 								0,
	 								_R(IDS_MAKECOLOURLOCALTOFRAMEOP),
									CC_RUNTIME_CLASS(OpMakeColourLocalToFrame),
	 								OPTOKEN_MAKECOLOURLOCALTOFRAME,
	 								OpMakeColourLocalToFrame::GetState,
	 								0,	/* help ID */
	 								0,	/* Bubble help ID */
	 								0	/* bitmap ID */
	 							));
}
    


/********************************************************************************************

>	OpState	OpMakeColourLocalToFrame::GetState(String_256*, OpDescriptor*)

	Author:		Stefan_Stoykov (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/7/97
	Inputs:		-
	Outputs:	-
	Returns:	The state of the OpMakeColourLocalToFrame operation
	Purpose:	For finding the OpMakeColourLocalToFrame's state. 

********************************************************************************************/

OpState	OpMakeColourLocalToFrame::GetState(String_256* UIDescription, OpDescriptor*)
{
	OpState OpSt;
 	return(OpSt);
}



/********************************************************************************************

>	void OpMakeColourLocalToFrame::PerformMergeProcessing()

	Author:		Stefan_Stoykov (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/7/97
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Called during EndOp to perform merging with the previous operation.

********************************************************************************************/

void OpMakeColourLocalToFrame::PerformMergeProcessing()
{
	// Obtain a pointer to the operation history for the operation's document
	OperationHistory* pOpHist = &GetWorkingDoc()->GetOpHistory();
	if (pOpHist == NULL)
	{
		ERROR3("There's no OpHistory!?");
		return;
	}

	// Ensure that we are the last operation added to the operation history
	if (pOpHist->FindLastOp() != this)
	{
		ERROR3("Last Op should be this op");
		return;
	}
	
	// obtain a pointer to the previous operation
	Operation* pPrevOp = pOpHist->FindPrevToLastOp();

	// do the merging only if we have a previous op, different then the operation before 
	// us in the history list at the start of this operation, and that operation
	// is a OpHideColours operation
	if ((pPrevOp != NULL) && (pPrevOp != m_pPreviousOp) && IS_A(pPrevOp, OpHideColours))
	{	
		// Copy all the actions from us to the end of the action list in the previous op.
		ActionList* pPrevActions = pPrevOp->GetUndoActionList();
		ActionList* pPrevOtherActions = pPrevOp->GetRedoActionList();
		if ((pPrevActions == NULL) || (pPrevOtherActions == NULL))
		{
			ERROR3("Previous actions list pointer was NULL");
			return;
		}

		// Copy actions across
		Action* pCurrentAction = (Action*)pPrevActions->RemoveHead();
		while (pCurrentAction != NULL)
		{
			pCurrentAction->TransferToOtherOp(this, &UndoActions, &RedoActions);
			pCurrentAction = (Action*)pPrevActions->RemoveHead();
		}

		// And remove the previous operation
		pOpHist->DeletePrevToLastOp(); 
	}

}

/********************************************************************************************

>	void OpMakeColourLocalToFrame::Do(OpDescriptor*)

	Author:		Stefan_Stoykov (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/7/97
	Inputs:		OpDescriptor (unused)
	Outputs:	-
	Returns:	-
	Purpose:	OpMakeColourLocalToFrame has a special overloaded DoWithParam operator which takes
				parameters describing what is to be done - that version of Do must be used
	Errors:		Always generates an ENSURE failure because this Do doesn't.
	SeeAlso:	-

********************************************************************************************/
			
void OpMakeColourLocalToFrame::Do(OpDescriptor *NotUsed)
{
	ENSURE(FALSE, "OpMakeColourLocalToFrame does not provide a Do() function - Use DoWithParam");
	End();
}



/********************************************************************************************

>	void OpMakeColourLocalToFrame::DoWithParam(OpDescriptor *pOp, OpParam *Param)

	Author:		Stefan_Stoykov (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/7/97
	Inputs:		pOp - OpDescriptor as for all Do() functions
				Param - points to a ColourChangeInfo structure with parameters:
					ParentList - The List in which the colours reside
					Target - The colour to be changed
					NewDefn - The returned copy of Target
					InvisibleChange - unused
					colour - unused
	Outputs:	-
	Returns:	-
	Purpose:	Performs the Make colour local to frame operation, which makes copies of Target 
				and all the children of it and replaces the references to them with references 
				to the copies, but only on the active frame.

********************************************************************************************/
			
void OpMakeColourLocalToFrame::DoWithParam(OpDescriptor *pOp, OpParam *Param)
{
	// get our info from the operation parameter
	ColourChangeInfo *Info = (ColourChangeInfo *) Param;

	// Set our scope variables
	pOurDoc = (Document *)Info->ParentList->GetParentDocument();
	pOurView = NULL;
	if (!pOurDoc->IsKindOf(CC_RUNTIME_CLASS(Document)))
	{
		pOurDoc = NULL;		// Arrrgh! It must be a basedoc or something!
		End();
		return;
	}
	
	// remember the previous operation
	OperationHistory* pOpHist = &GetWorkingDoc()->GetOpHistory();
	if (pOpHist == NULL)
		m_pPreviousOp = NULL;
	else
		m_pPreviousOp = pOpHist->FindLastOp();
	
	// Get the colour list component
	ColComp = (ColourListComponent *)
				pOurDoc->GetDocComponent(CC_RUNTIME_CLASS(ColourListComponent));

	// Start the copy
	if (ColComp != NULL && ColComp->StartComponentCopy())
	{
	
		// get the parent list
		ParentList = Info->ParentList;

		// get the spread
		Spread *pSpread = pOurDoc->GetSelectedSpread();
		if (pSpread != NULL)
		{
			// find the active layer
			pActiveLayer = pSpread->FindActiveLayer();
			if (pActiveLayer != NULL)
			{
				// do the work
				Info->NewDefn = MakeColourLocalToFrame(Info->Target);
			}
		}

		// And clean up, merging the new colours into the document's colour list
		ColComp->EndComponentCopy(NULL, FALSE);
	}

	End();
}



/********************************************************************************************

>	void OpMakeColourLocalToFrame::Undo()

	Author:		Stefan_Stoykov (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/7/97
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Undoes whatever Do() or Redo() did

********************************************************************************************/

BOOL OpMakeColourLocalToFrame::Undo() 
{
	if (Operation::Undo() == FALSE) 
		return FALSE;

	// send the colour changing message
	ColourManager::ColourListHasChanged(ParentList);

	return TRUE;
}		



/********************************************************************************************

>	void OpMakeColourLocalToFrame::Redo()

	Author:		Stefan_Stoykov (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/7/97
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Redoes whatever Undo() undid

********************************************************************************************/

BOOL OpMakeColourLocalToFrame::Redo()
{
	if (!Operation::Redo())
		return FALSE;

	// send the colour changing message
	ColourManager::ColourListHasChanged(ParentList);

	return TRUE;
}




/********************************************************************************************

>	BOOL OpMakeColourLocalToFrame::ReplaceColourOnActiveLayer(IndexedColour *pOld, 
																IndexedColour *pNew)

	Author:		Stefan_Stoykov (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/7/97
	Inputs:		pOld - the old indexed colour 
				pNew - the new indexed colour
	Outputs:	-
	Returns:	-
	Purpose:	Replaces all the references to pOld with references to pNew on the active frame.

********************************************************************************************/

BOOL OpMakeColourLocalToFrame::ReplaceColourOnActiveLayer(IndexedColour *pOld, IndexedColour *pNew)
{
	// sanity checks
	if ((pOld == NULL) || (pNew == NULL)) 
		return FALSE;

	ERROR3IF(pOurDoc == NULL, "No doc");
	if (pOurDoc == NULL)
		return FALSE;

	// start the attribute changing
	ObjChangeFlags ChangeFlags;
	ChangeFlags.Attribute = TRUE;

	ObjChangeParam ChangeParam(OBJCHANGE_STARTING, ChangeFlags, NULL, NULL);

	// get the first child of the active layer
	Node *CurNode = pActiveLayer->FindFirstDepthFirst();

	while (CurNode != NULL) // for all the nodes on the active layer
	{
		if (CurNode->IsAnAttribute()) // check only attribute nodes
		{
			// Have found a NodeAttribute. Scan it to see if it contains colours matching the old colour.

			NodeAttribute *pNodeAttr = (NodeAttribute *) CurNode;
			DocColour *pColour;
			DocColour *pColourCopy;
			UINT32 Context = 0;

			BOOL Found = FALSE; // whether our colour was referenced on this node

			// scan the attribute for the colour
			do
			{
				// scan all the colour fields
				pColour = pNodeAttr->EnumerateColourFields(Context++);
		
				// get to the indexed colour
				IndexedColour *pColourIx = (pColour == NULL) ? NULL : pColour->FindParentIndexedColour();

				// Check if this colour matches our colour
				if (pColourIx == pOld)
				{
					Found = TRUE; // found it
					break;
				}
			} while (pColour != NULL);	// Check the next colour attribute

			
			if (Found) // we have found a our colour in this attribute
			{
				// make a copy of the attribute
				NodeAttribute *Clone = NULL;
				NodeAttribute *NodeToClone = (NodeAttribute *) CurNode;
				ALLOC_WITH_FAIL(Clone, ((NodeAttribute *)NodeToClone->SimpleCopy()), this);
				if (Clone == NULL) // failed
					break;

				Context = 0;

				// for all the colour fields which match our old colour, 
				// replace them with references to the new colour
				do
				{
					// Get the next colour field from the attribute, and find the
					// IndexedColour (if any) to which it refers
					pColour = pNodeAttr->EnumerateColourFields(Context);
					pColourCopy = Clone->EnumerateColourFields(Context);
					Context++;
		
					// get the indexed colour
					IndexedColour *pColourIx = (pColour == NULL) ? NULL : pColour->FindParentIndexedColour();

					// Check if this colour matches our colour
					if (pColourIx == pOld)
					{
						// set the new indexed colour
						pColourCopy->MakeRefToIndexedColour(pNew);
					}
				} while (pColour != NULL);	// Check the next colour attribute


				// we need to hide the current attribute node and add the coped node to the tree

				// find the parent of the node
				Node *Parent = CurNode->FindParent();  
				if (Parent != NULL && Parent->IsBounded())
				{
					// Invoke AllowOp on the node to tag it so that parents such as blends
					// and moulds will also redraw if necessary. 
					if (CurNode->AllowOp(&ChangeParam))
					{
						// Hide the original Attribute
						DoHideNode(CurNode, TRUE);

						// Add the clone into the tree
						Clone->AttachNode(Parent, FIRSTCHILD);
						HideNodeAction* TheUndoAction; 

           				// Create an action to hide the attribute when we undo
						if (HideNodeAction::Init(this, &UndoActions, Clone, TRUE,
												 (Action**)(&TheUndoAction)) == AC_FAIL)
						{   
							Clone->CascadeDelete();		// Delink the node/subtree from the
							delete Clone;				// document tree and delete it
						}

						// make the clone the current node so we can carry on walking the tree
						CurNode = Clone;
					}
				}
			}
		}

		// go to the next node in the layer
		CurNode = CurNode->FindNextDepthFirst(pActiveLayer);
	}
	
	// finished changing the tree
	ChangeParam.Define(OBJCHANGE_FINISHED, ChangeParam.GetChangeFlags(), NULL, NULL);
	UpdateAllChangedNodes(&ChangeParam);

	return TRUE;
}



/********************************************************************************************

>	IndexedColour *OpMakeColourLocalToFrame::MakeColourLocalToFrame(IndexedColour *pCol)

	Author:		Stefan_Stoykov (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/7/97
	Inputs:		pCol - pointer to an indexed colour 
	Outputs:	-
	Returns:	The created copy of pCol, local to the active frame
	Purpose:	Creates a new colour, which is a copy of the pCol. The name of the new colour 
				is <FrameName> (<ColourName>), where FrameName is the name of the active frame, 
				and ColurName is the name of pCol. ReplaceColourOnActiveLayer is called to 
				replace pCol with the new copy on the active frame. Afterthat the colour list 
				for the current document is to find all the children of this colour and this 
				function is recursivelly called for them.

********************************************************************************************/

IndexedColour *OpMakeColourLocalToFrame::MakeColourLocalToFrame(IndexedColour *pCol)
{
	// sanity check
	if ((pActiveLayer == NULL) || (pCol == NULL))
		return NULL;

	// generate the name for the new colour
	String_256 DefName;

	// get the layer name
	DefName = pActiveLayer->GetLayerID();

	// add the name of the colour
	DefName += _T(" (");
	DefName += (TCHAR *)(*(pCol->GetName()));
	DefName += _T(")");

	String_64 OrigName;
	String_64 NewName;

	// use the first 63 chars only
	DefName.Left(&OrigName, 63);

	// ask for a unique name
	ParentList->GenerateUniqueColourName(&OrigName, &NewName);

	// create a copy of the original colour
	IndexedColour *Copy = new IndexedColour(*pCol); // make a copy of current
	if (Copy == NULL)
		return NULL;

	// set the new name
	Copy->SetName(NewName);

	//Add the colour to the document
	// Copy the colour into the destination document (merging it with existing
	// colours so we won't keep creating new copies of the same colour as it's applied)
	DocColour ColourToApply;
	ColourToApply.MakeRefToIndexedColour(Copy);
	ColComp->CopyColourAcross(&ColourToApply, TRUE);

	delete Copy; // delete the temp copy
	
	// get the new colour (now part of the document colours)
	Copy = ColourToApply.FindParentIndexedColour();

	// Replace the references of the original colour with the new colour in the active layer
	ReplaceColourOnActiveLayer(pCol, Copy);

	// now create local copies for all the children

	// scan the listto find all the children
	IndexedColour *C = ParentList->GetUndeletedHead();
	while (C != NULL)
	{
		// a child found
		if ((C->FindLinkedParent() == pCol) && (C->IsDeleted() == FALSE))
		{
			// make local copy (recursive call)
			IndexedColour *pResult = MakeColourLocalToFrame(C);

			// set the new parent to it
			if (pResult != NULL)
				pResult->SetLinkedParent(Copy, C->GetType());
		}

		// go to the next colour in the list
		C = ParentList->GetUndeletedNext(C);
	}

	return Copy;
}


/********************************************************************************************

>	BOOL ColourEditDlg::OnMakeLocalToFrame()

	Author:		Stefan_Stoykov (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/7/97
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Invokes the OpMakeColourLocalToFrame operation to make local copy of the current 
				edited colour.

********************************************************************************************/

BOOL ColourEditDlg::OnMakeLocalToFrame()
{
#if !defined(EXCLUDE_FROM_RALPH) && !defined(EXCLUDE_FROM_XARALX)
	// find our operation
	OpDescriptor* OpDesc =
				OpDescriptor::FindOpDescriptor(CC_RUNTIME_CLASS(OpMakeColourLocalToFrame));

	if (OpDesc != NULL)
	{
		// set up the info to pass to the operation
		ColourChangeInfo Param;
		Param.ParentList = ParentList;
		Param.Target	 = ResultColour;

		// invoke the operation
		OpDesc->Invoke((OpParam *) &Param);

		// set the colour in the colour editor
		EditThisColour(ParentList, Param.NewDefn, TRUE);
		return TRUE;
	}
#endif

	return FALSE;
}



#if FALSE
BOOL OpMakeColourLocalToFrame::IsColourLocalToFrame(Layer *pTheLayer, IndexedColour *pCol)
{
	if (pOurDoc == NULL)
		return TRUE;
	
	// get the selected spread
	Spread *pSpread = pOurDoc->GetSelectedSpread();
	ERROR2IF(pSpread == NULL, FALSE, "Doc with no spread found");
	
	// get the first frame/layer
	Layer *pLayer = pSpread->FindFirstLayer();
	ERROR2IF(pLayer == NULL, FALSE, "Spread with no layer found");

	// for all the layers
	while (pLayer != NULL)
	{
		if (pLayer != pTheLayer) // if not our layer
		{
			Node *CurNode = pLayer->FindFirstDepthFirst();
			while (CurNode != NULL)
			{
				if (CurNode->IsAnAttribute())
				{
					// Have found a NodeAttribute. Scan it to see if it contains any colours.
					// If any of these match any in our Colours list, then force a redraw of
					// our parent node, and return.
					NodeAttribute *pNodeAttr = (NodeAttribute *) CurNode;
					DocColour *pColour;
					UINT32 Context = 0;

					do
					{
						// Get the next colour field from the attribute, and find the
						// IndexedColour (if any) to which it refers
						pColour = pNodeAttr->EnumerateColourFields(Context++);
				
						IndexedColour *pColourIx = (pColour == NULL) ? NULL : pColour->FindParentIndexedColour();
						if (pColourIx != NULL)
						{
							// Check if this colour matches our colour
							INT32 i = 0;
							if (pColourIx == pCol)
							{
								// There is no need to check further colour attributes for this node

								return FALSE;
							}
						}
					} while (pColour != NULL);	// Check the next colour attribute
				}

				CurNode = CurNode->FindNextDepthFirst(pLayer); // get the next node
			}
		}

		pLayer = pLayer->FindNextLayer(); // go to the next layer
	}

	return TRUE;
}
#endif

/********************************************************************************************

>	void ColourEditDlg::BeginTimedProcessing()

	Author:		Noel_Someone (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/6/99
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Sets a message based timer which indirectly calls 
				ColourEditDlg::TimedProcessing.

********************************************************************************************/

void ColourEditDlg::BeginTimedProcessing()
{
	KillTimer(COLED_TIMER_ID);
	SetTimer(COLED_TIMER_ID, COLED_TIMER_FREQUENCY, NULL);
}

/********************************************************************************************

>	void ColourEditDlg::EndTimedProcessing()

	Author:		Noel_Someone (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/6/99
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Kills the message based timer which is used to call
				ColourEditDlg::TimedProcessing.

********************************************************************************************/

void ColourEditDlg::EndTimedProcessing()
{
	KillTimer(COLED_TIMER_ID);
}




/********************************************************************************************

>	static BOOL ColourEditDlg::GetColourContext(ColourModel ColModel, ColourContext** ppContext)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/02/04
	Inputs:		ColModel - the colour model to get a context for
	Outputs:	ppContext - location to write the context pointer
	Returns:	TRUE if context should be delete'd after use 
	Purpose:	Gets an appropriate colour context for displaying the colour picker
				This is so that CMYK can use the printer profile

********************************************************************************************/

BOOL ColourEditDlg::GetColourContext(ColourModel ColModel, ColourContext** ppContext)
{
	// If we want a CMYK context then try to set one up with the printer profile
	if (ColourEditDlg::bUsePrintCMYK && ColModel == COLOURMODEL_CMYK)
	{
PORTNOTE("other", "Disabled CMS usage")
#ifndef EXCLUDE_FROM_XARALX
		ColourContext *pContext;

		XaraCMS* ptheCMS = GetApplication()->GetCMSManager();

		if (ptheCMS != NULL)
		{
			// First try to set the printer context specified in the ini file read
			// when we started up
			String_256 PrintProfile;
			ptheCMS->GetPrinterProfile(&PrintProfile);
			pContext = new ColourContextCMYK(NULL, &PrintProfile);
		
			if (pContext == NULL || !(pContext->IsDeviceContext()))
			{
				// Eeek, we failed to build the physical device context
				// which means something rather nasty went on.

				delete pContext;
				pContext=NULL;

				// Now try with the default printer profile
				ptheCMS->GetDefaultPrinterProfile(&PrintProfile);
				pContext = new ColourContextCMYK(NULL, &PrintProfile);

				if (pContext == NULL || !(pContext->IsDeviceContext()))
				{
					// We failed to get the default printer profile so delete the context and fall through
					delete pContext;
					pContext=NULL;
				}
			}

			if (pContext)
			{
				ColourContextList::GetList()->AddContext(&pContext);		// Add it to the list so it works properly
				*ppContext = pContext;
				return(TRUE);
			}
		}
#endif
	}

	// Otherwise just fall back to getting a default context for the required model
	*ppContext = ColourContext::GetGlobalDefault(ColModel);
	return(FALSE);
}

