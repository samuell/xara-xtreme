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

// colpick.cpp - Colour picker dialogue

/*
*/


#include "camtypes.h"

#include "colcontx.h"
#include "coldlog.h"
#include "collist.h"
#include "colormgr.h"
#include "colourix.h"
#include "colpick.h"
#include "convert.h"
#include "scunit.h"
#include "ctrlhelp.h"
//#include "jason.h"
#include "reshlpid.h"
//#include "richard3.h"
//#include "ed.h"



CC_IMPLEMENT_MEMDUMP(ColourPicker, CCObject)

#define new CAM_DEBUG_NEW


// The colour picker can supply two types of picker - the standard OS picker
// or a special Camelot picker. The latter allows true editing of colours in
// several colour models, while the former may (does under windows) force the
// colour to RGB or something. The OS picker is included so a) finikity users
// can use a crap picker if they are shy of meeting new dialogues, and b) so
// we can pick new colours/edit existing colours without having to wait for
// a complete picker implementation to roll up...
static BOOL UseOSPicker = FALSE;


// Static buffer for storing bubble help from the colour editor's picker area
// until the bubble-help callback mechanism is called back.
static String_128 PickerBubbleBuffer("");
static String_256 StatusHelpBuffer("");


/********************************************************************************************

>	ColourPicker::ColourPicker()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/5/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Creates a colourpicker object
	Scope:		private
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

ColourPicker::ColourPicker()
{
}



/********************************************************************************************

>	ColourPicker::~ColourPicker()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/5/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Destructor for a Colour Picker
	Scope:		private
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

ColourPicker::~ColourPicker()
{
}


/********************************************************************************************

>	static INT32 ColourValueToByte(const ColourValue &Col)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/5/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Converts a ColourValue (0.0 to 1.0) into a byte (0..255) with clipping
				to handle out-of-gamut values.

	Notes:		This function is not as efficient as it could be. If it ever becomes
				critical in any way, it should be optimised. See comments in the function.

	Scope:		private
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
/*
	=== This code has been rewritten to use centralised functions. It is UNTESTED ===

static INT32 ColourValueToByte(const ColourValue &Col)
{
	// We want to use the centralised ColourContext::PackColour function to do this for us
	// However, it's protected. The easiest way of accessing the functionality is to
	// create a DocColour containing the value, then read back the 0..255 value.
	// This is more work than strictly necessary, but then again, this function is non critical.

	DocColour Dummy(Col, Col, Col);

	INT32 Red;
	INT32 Green;
	INT32 Blue;
	Dummy.GetRGBValue(&Red, &Green, &Blue);

	return(Red);
}
*/


/********************************************************************************************

>	BOOL ColourPicker::EditColour(ColourList *ParentList, IndexedColour *SourceAndResult,
									BOOL PreferLineColour = FALSE)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/5/94
	Inputs:		ParentList - The list in which the colour resides
					May be NULL if you want to just show the picker for whatever
					local colour is appropriate.
					SourceAndResult must also be NULL in this case

				SourceAndResult - The colour to be edited. If the user changes
				the colour, this will be set to the new definition.
					May be NULL if you want to just show the picker for whatever
					local colour is appropriate.
					ParentList must also be NULL in this case

				PreferLineColour - TRUE if you'd like to edit line colours rather than
				fill colours in local mode, FALSE to edit fill colours. This flag is
				ignored unless ParentList == SourceAndResult == NULL. (Implementation
				note: This calls ColourEditDlg::ForceLineOrFillMode)

	Outputs:	-
	Returns:	-
	Purpose:	The main colour picker function. Given an IndexedColour, sets up
				a colour picker showing that colour, allows the user to edit it,
				and returns the new colour in said IndexedColour. If the colour is
				changed, an appropriate ColourChangingMsg will be broadcast.

	Notes:		The IndexedColour is actually copied into another for editing. The
				change, if OK'd is then applied using the ColourManager::ChangeColour
				function, which provides an undo record of the event.

	Scope:		private
	Errors:		-
	SeeAlso:	ColourEditDlg::InvokeDialog

********************************************************************************************/

void ColourPicker::EditColour(ColourList *ParentList, IndexedColour *SourceAndResult,
								BOOL PreferLineColour)
{
#if FALSE
/*
// **** DEBUG - use Windows Picker if SHIFT NOT held down on entry,
//				else use the Camelot Picker
	if (ForceNativePicker)
		UseOSPicker = TRUE;
	else
		UseOSPicker = (::GetAsyncKeyState(CAMKEY(SHIFT)) & 0x8000) ? TRUE : FALSE;
// ****
	if (UseOSPicker)
	{
		IndexedColour *UndoBuffer = new IndexedColour(*SourceAndResult);
		if (UndoBuffer == NULL)		// Fail gracefully. Well... Don't crash, anyway
			return;

		ColourGeneric *TheColour = UndoBuffer->SourceColourPtr();

		Document *ScopeDoc = Document::GetSelected();
		if (ScopeDoc == NULL)
			return;

		ColourContext *ccRGB = ScopeDoc->GetDefaultColourContexts()->Context[COLOURMODEL_RGBT];
		ENSURE(ccRGB != NULL, "Default RGBT context can't be found!");

		ColourContext *ccSource = ScopeDoc->GetDefaultColourContexts()->Context[UndoBuffer->GetColourModel()];
		if (ccSource == NULL)
		{
			ENSURE(FALSE, "Unsupported source colour context - I can't edit that colour");
			return;
		}

		// Convert the given colour into RGB, using 'pure' contexts (logical rather than physica
		// conversion, i.e. no colour correction en-route)
		ColourRGBT TheColourRGB;
		ccRGB->ConvertColour(ccSource, TheColour, (ColourGeneric *) &TheColourRGB);


		// Now set up, and call the Windows colour picker to edit the colour...
		CHOOSECOLOR cc;

		// Set initial colour for the colour picker to use
		cc.rgbResult = RGB(	ColourValueToByte(TheColourRGB.Red),
							ColourValueToByte(TheColourRGB.Green),
							ColourValueToByte(TheColourRGB.Blue) );


		// Set the default set of available colours (a greyscale for the time being)
		// (it is static so that if the user changes any of the colours they will get
		// the changed palette back next time around)
		static COLORREF acrCustClr[16] =
			{
				RGB(255, 255, 255),	RGB(239, 239, 239),
				RGB(223, 223, 223),	RGB(207, 207, 207),
				RGB(191, 191, 191),	RGB(175, 175, 175),
				RGB(159, 159, 159),	RGB(143, 143, 143),
				RGB(127, 127, 127),	RGB(111, 111, 111),
				RGB(95, 95, 95),	RGB(79, 79, 79),
				RGB(63, 63, 63),	RGB(47, 47, 47),
				RGB(31, 31, 31),	RGB(15, 15, 15)
			};

		cc.lStructSize = sizeof(CHOOSECOLOR);
		cc.hwndOwner = AfxGetApp()->m_pMainWnd->m_hWnd;	// 'owner' window
		cc.lpCustColors = (LPDWORD) acrCustClr;			// List of default colours
		cc.Flags = CC_RGBINIT | CC_FULLOPEN;			// Use cc.rgbResult to init picker...
														// and picker appears fully opened - Geez that
														// 'define wossnames' button is a pain in the butt!!!

		if (ChooseColor(&cc))		// Invoke Windows colour picker (if it qualifies as a colour picker!)
		{
			// User chose a colour and hit OK - read out the return value (always RGB)
			UndoBuffer->SetSourceColourModel(COLOURMODEL_RGBT);
			((ColourRGBT *)TheColour)->Red   = ((double) GetRValue(cc.rgbResult)) / 256.0;
			((ColourRGBT *)TheColour)->Green = ((double) GetGValue(cc.rgbResult)) / 256.0;
			((ColourRGBT *)TheColour)->Blue  = ((double) GetBValue(cc.rgbResult)) / 256.0;
			((ColourRGBT *)TheColour)->Transparent = 0;

			Document::GetSelected()->SetCurrent();		// Make this document the current one

			// This swallows UndoBuffer into the undo system - we must NOT delete it
			ColourManager::ChangeColour(ParentList, UndoBuffer, SourceAndResult);
		}
		else
			delete UndoBuffer;	// UndoBuffer not used - delete it
	}
	else
*/
#endif
	{
#ifndef STANDALONE
		// If we are asking to edit an appropriate local colour, state our preference
		// for either fill or line colour.
		if (SourceAndResult == NULL)
			ColourEditDlg::ForceLineOrFillMode(PreferLineColour);

		ColourEditDlg::InvokeDialog(ParentList, SourceAndResult);
#endif
	}
}



/********************************************************************************************

>	static BOOL ColourPicker::GetStatusLineText(ColourEditDlg *Editor, UINT32 GadgetID,
													String_256 *Result)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/5/95

	Inputs:		Editor - points to the current editor
				GadgetID - indicates the gadget for which you desire help

	Outputs:	If return value is TRUE, Result is updated with an appropriate help string

	Returns:	TRUE if it changed Result, FALSE if it can offer no help

	Purpose:	Bodge to get around the fact that DialogOps don't plug into the status
				line help system. Returns help for the colour editor. Must go via the
				winoil, as we have to read the mouse position and find the window it's in.

	SeeAlso:	ColourEditDlg::GetStatusLineText

********************************************************************************************/

BOOL ColourPicker::GetStatusLineText(ColourEditDlg *Editor, UINT32 GadgetID, String_256 *Result)
{
#ifndef STANDALONE

	IndexedColour *Bob = Editor->EditingColour;
	if (Bob == NULL)
	{
		*Result = String_256(_R(IDS_EDITST_NOCOLOUR));
		return(TRUE);
	}

	ColourContext *cc = ColourContext::GetGlobalDefault(ColourEditDlg::DisplayModel);

	// Set the default help
	*Result = String_256(_R(IDS_EDITST_DEFAULT));

	switch (GadgetID)
	{
//		case IDCANCEL:
//			*Result = String_256(_R(IDS_EDITST_CANCEL));
//			break;

		case _R(IDC_EDIT_DROPMENU):
			*Result = String_256(_R(IDS_EDITST_MENU));
			break;

		case _R(IDC_EDIT_COMPONENT1):
			if (ColourEditDlg::DisplayModel == COLOURMODEL_HSVT)
			{
				*Result = String_256(_R(IDS_EDITST_COMP1));
			}
			else
			{
				if (cc != NULL)
				{
					String_64 CompName;
					cc->GetComponentName(1, &CompName, TRUE);
					Result->MakeMsg(_R(IDS_EDITST_COMP234), (TCHAR *)CompName);
				}
			}
			break;

		case _R(IDC_EDIT_COMPONENT2):
			if (cc != NULL)
			{
				String_64 CompName;
				cc->GetComponentName(2, &CompName, TRUE);
				Result->MakeMsg(_R(IDS_EDITST_COMP234), (TCHAR *)CompName);
			}
			break;

		case _R(IDC_EDIT_COMPONENT3):
			if (cc != NULL)
			{
				String_64 CompName;
				cc->GetComponentName(3, &CompName, TRUE);
				Result->MakeMsg(_R(IDS_EDITST_COMP234), (TCHAR *)CompName);
			}
			break;

// WEBSTER - markn 14/12/96
#ifndef WEBSTER
		case _R(IDC_EDIT_COMPONENT4):
			if (cc != NULL)
			{
				String_64 CompName;
				cc->GetComponentName(4, &CompName, TRUE);
				Result->MakeMsg(_R(IDS_EDITST_COMP234), (TCHAR *)CompName);
			}
			break;

		case _R(IDC_EDIT_COLMODEL):
			*Result = String_256(_R(IDS_EDITST_COLMODEL));
			break;

		case _R(IDC_EDIT_NAMEMENU):
			*Result = String_256(_R(IDS_EDITST_NAME));
			break;

		case _R(IDC_EDIT_COLTYPE):
			*Result = String_256(_R(IDS_EDITST_COLTYPE));
			break;

		case _R(IDC_EDIT_INHERIT1):
		case _R(IDC_EDIT_INHERIT2):
		case _R(IDC_EDIT_INHERIT3):
		case _R(IDC_EDIT_INHERIT4):
			*Result = String_256(_R(IDS_EDITST_INHERIT));
			break;

//		case _R(IDC_EDIT_PATCH):
//		case _R(IDC_EDIT_PARENTPATCH):
//			*Result = String_256(_R(IDS_EDITST_PARENTPATCH));
//			break;

		case _R(IDC_EDIT_PARENTCOL):
		case _R(IDC_EDIT_PARENTNAME):
			*Result = String_256(_R(IDS_EDITST_PARENTCOL));
			break;

		case _R(IDC_EDIT_TINT):
		case _R(IDC_EDIT_TINTSLIDER):
			if (Bob->GetType() == COLOURTYPE_TINT && !Bob->TintIsShade())
				*Result = String_256(_R(IDS_EDITST_TINT1));		// It's a tint
			else
				*Result = String_256(_R(IDS_EDITST_TINT2));		// It's a shade
			break;

		case _R(IDC_EDIT_SHADE):
			*Result = String_256(_R(IDS_EDITST_TINT2));			// It's a shade
			break;

		case _R(IDC_EDIT_ADVANCED):
			if (Editor->Folded)
				*Result = String_256(_R(IDS_EDITST_ADVANCED1));
			else
				*Result = String_256(_R(IDS_EDITST_ADVANCED2));
			break;

		case _R(IDC_EDIT_3D):
			*Result = String_256(_R(IDS_EDITST_3D));
			break;

		case _R(IDC_EDIT_MAKESTYLE):
			*Result = String_256(_R(IDS_EDITST_MAKESTYLE));
			break;

#endif // WEBSTER

		case _R(IDC_EDIT_PICKER):
			if (!StatusHelpBuffer.IsEmpty())
				*Result = StatusHelpBuffer;
			else
			{
				switch(Bob->GetType())
				{
				// WEBSTER - markn 14/12/96
				#ifndef WEBSTER
					case COLOURTYPE_TINT:
						if (Editor->Folded)
							*Result = String_256(_R(IDS_EDITST_PICKER1));
						else
							*Result = String_256(_R(IDS_EDITST_PICKER2));
						break;

					case COLOURTYPE_LINKED:
						*Result = String_256(_R(IDS_EDITST_PICKER3));
						break;
				#endif // WEBSTER

					case COLOURTYPE_NORMAL:
					case COLOURTYPE_SPOT:
					default:
						*Result = String_256(_R(IDS_EDITST_PICKER4));
						break;
				}
			}
			break;

		case _R(IDC_EDIT_216ONLY):
			*Result = String_256(_R(IDS_EDITST_216ONLY));
			break;

		case _R(IDC_COLOURPICKER):
			*Result = String_256(_R(IDS_STATICCOLOURPICKERTOOLHELP));
			break;

		case _R(IDC_MAKE_LOCAL):
			*Result = String_256(_R(IDS_EDITST_MAKE_LOCAL));
			break;


		case _R(IDC_EDIT_NOCOLOUR):
			*Result = String_256(_R(IDS_EDITST_SETNOCOLOUR));
			break;

		case _R(IDC_EDIT_RENAME):	 
			*Result = String_256(_R(IDS_EDITST_RENAME));
			break;


		case _R(IDC_EDIT_LINEFILL):
			*Result = String_256(_R(IDS_EDITST_LINEFILL));
			break;
	}

#endif
	return(TRUE);
}



/********************************************************************************************

>	static BOOL ColourPicker::GetStatusLineText(String_256 *Result)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/5/95

	Outputs:	If return value is TRUE, Result is updated with an appropriate help string

	Returns:	TRUE if it changed Result, FALSE if it can offer no help

	Purpose:	Bodge to get around the fact that DialogOps don't plug into the status
				line help system. Returns help for the colour editor. Must go via the
				winoil, as we have to read the mouse position and find the window it's in.

	Notes:		This is a huge bodge:
					1) DialogOp should do this for us
					2) I'm on holiday in a day's time, I don't have time to do it properly
					3) It only gives help for the picker control because I can't work out
						a quick & easy way ofgetting WindowID and GadgetID

	SeeAlso:	ColourEditDlg::GetStatusLineText

********************************************************************************************/

#ifndef STANDALONE
static UINT32 GadgetList[] = 
	{
//		IDCANCEL,
		_R(IDC_EDIT_DROPMENU),
		_R(IDC_EDIT_COMPONENT1),
		_R(IDC_EDIT_COMPONENT2),
		_R(IDC_EDIT_COMPONENT3),
		_R(IDC_EDIT_COMPONENT4),
		_R(IDC_EDIT_COLMODEL),
		_R(IDC_EDIT_NAMEMENU),
		_R(IDC_EDIT_COLTYPE),
		_R(IDC_EDIT_INHERIT1),
		_R(IDC_EDIT_INHERIT2),
		_R(IDC_EDIT_INHERIT3),
		_R(IDC_EDIT_INHERIT4),
		_R(IDC_EDIT_PICKER),
		_R(IDC_EDIT_PATCH),
		_R(IDC_EDIT_PARENTPATCH),
		_R(IDC_EDIT_PARENTCOL),
		_R(IDC_EDIT_TINT),
		_R(IDC_EDIT_SHADE),
		_R(IDC_EDIT_ADVANCED),
		_R(IDC_EDIT_PARENTNAME),
//		_R(IDC_EDIT_TINTSLIDER),
//		_R(IDC_EDIT_PICKERBORDER),
		_R(IDC_EDIT_3D),
		_R(IDC_EDIT_MAKESTYLE),

		_R(IDC_EDIT_NOCOLOUR),
		_R(IDC_EDIT_LINEFILL),
		_R(IDC_EDIT_216ONLY),
		_R(IDC_MAKE_LOCAL),
		_R(IDC_EDIT_RENAME),
		_R(IDC_COLOURPICKER),
		NULL
	};
#endif


BOOL ColourPicker::GetStatusLineText(String_256 *Result)
{
#ifndef STANDALONE

	ERROR3IF(Result == NULL, "Illegal NULL param");
	
	ColourEditDlg *Editor = ColourEditDlg::TheEditor;
	if (Editor == NULL)
		return(FALSE);

	// Find the main editor window
	HWND TheWindow = (HWND)Editor->WindowID;
	if (TheWindow == NULL)
		return(FALSE);

	// Get the cursor position in screen coords
	POINT TempPos;
	if (!::GetCursorPos(&TempPos))
		return(FALSE);

	// Convert to client coords in the main window
	ScreenToClient(TheWindow, &TempPos);

	// See if this is over the main window or any child thereof, getting it's HWND
	CPoint Pos(TempPos);	
	HWND WindowUnderPointer = ::ChildWindowFromPoint(TheWindow, Pos);
	if (WindowUnderPointer == NULL)
		return(FALSE);

	// Make sure that hidden windows do not provide status help!
	INT32 WindowStyle = ::GetWindowLong(WindowUnderPointer, GWL_STYLE);
	if ((WindowStyle & WS_VISIBLE) == 0)
		return(FALSE);

	// If the pointer is over main window background, return standard help
	if (WindowUnderPointer == TheWindow)
		return(GetStatusLineText(Editor, 0, Result));

	// Otherwise, see if we can find help for the specific gadget
	HWND hGadget;
	INT32 i = 0;
	while (GadgetList[i] != NULL)
	{
		hGadget = GetDlgItem(TheWindow, GadgetList[i]);
		if (WindowUnderPointer == hGadget && GetStatusLineText(Editor, GadgetList[i], Result))
			return(TRUE);
		i++;
	}

#endif
	return(TRUE);
}



/********************************************************************************************

>	TCHAR *ColourPicker::HelpCallbackHandler(HWND Window, UINT32 Item, void *UserData)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/9/95

	Inputs:		Window - identifies the window to get help for
				Item   - identifies the control to get help for
				UserData - User-supplied data (not used)

	Returns:	The bubble help string to use, or NULL if no help is available

	Purpose:	Help callback handler to provide bubble help for the colour editor

	SeeAlso:	ColourPicker::UpdateBubbleHelpAndPointer

********************************************************************************************/

TCHAR *ColourPicker::HelpCallbackHandler(HWND Window, UINT32 Item, void *UserData)
{
#ifndef STANDALONE
	static String_256 HelpStringStore;
	BOOL ReturnVal = FALSE;

	ColourEditDlg *Editor = ColourEditDlg::TheEditor;
	if (Editor == NULL)
		return(NULL);

	IndexedColour *Bob = Editor->EditingColour;

	ColourContext *cc = NULL;
	if (Bob != NULL)
		cc = ColourContext::GetGlobalDefault(ColourEditDlg::DisplayModel);

	switch (Item)
	{
		case _R(IDC_EDIT_DROPMENU):
			HelpStringStore = String_256(_R(IDS_EDITBH_MENU));
			ReturnVal = TRUE;
			break;

		case _R(IDC_EDIT_PATCH1):		// Fake ID for current colour patch to give it a unique ID
		case _R(IDC_EDIT_PATCH2):		// Fake ID for current colour patch to give it a unique ID
		case _R(IDC_EDIT_PATCH):
		case _R(IDC_EDIT_PICKER):
			// NOTE: This gives help for "Original/Current colour" patch
			// We will only get called for this if it is necessary (pointer in correct region of control)
			HelpStringStore = PickerBubbleBuffer; //String_256(_R(IDS_EDITBH_PARENTPATCH));
			ReturnVal = TRUE;
			break;

		case _R(IDC_EDIT_COMPONENT1):
		case _R(IDC_EDIT_COMPONENT2):
		case _R(IDC_EDIT_COMPONENT3):
		case _R(IDC_EDIT_COMPONENT4):
			// Assumes _R(IDC_EDIT_COMPONENT1)..4 are sequential
			if (cc != NULL)
			{
				ReturnVal = GetComponentHelp(cc, Item - _R(IDC_EDIT_COMPONENT1) + 1, HelpStringStore);
			}
			break;
/*
			if (ColourEditDlg::DisplayModel == COLOURMODEL_HSVT)
			{
				HelpStringStore = String_256(_R(IDS_EDITBH_COMP1));
				ReturnVal = TRUE;
			}
			else
			{
				if (cc != NULL)
				{
					String_64 CompName;
					cc->GetComponentName(1, &CompName, TRUE);
					HelpStringStore.MakeMsg(_R(IDS_EDITBH_COMP234), (TCHAR *)CompName);
					ReturnVal = TRUE;
				}
			}
			break;

		case _R(IDC_EDIT_COMPONENT2):
			if (cc != NULL)
			{
				String_64 CompName;
				cc->GetComponentName(2, &CompName, TRUE);
				HelpStringStore.MakeMsg(_R(IDS_EDITBH_COMP234), (TCHAR *)CompName);
				ReturnVal = TRUE;
			}
			break;

		case _R(IDC_EDIT_COMPONENT3):
			if (cc != NULL)
			{
				String_64 CompName;
				cc->GetComponentName(3, &CompName, TRUE);
				HelpStringStore.MakeMsg(_R(IDS_EDITBH_COMP234), (TCHAR *)CompName);
				ReturnVal = TRUE;
			}
			break;

		case _R(IDC_EDIT_COMPONENT4):
			if (cc != NULL)
			{
				String_64 CompName;
				cc->GetComponentName(4, &CompName, TRUE);
				HelpStringStore.MakeMsg(_R(IDS_EDITBH_COMP234), (TCHAR *)CompName);
				ReturnVal = TRUE;
			}
			break;
*/
		case _R(IDC_EDIT_COLMODEL):
			HelpStringStore = String_256(_R(IDS_EDITBH_COLMODEL));
			ReturnVal = TRUE;
			break;

		case _R(IDC_EDIT_NAMEMENU):
			HelpStringStore = String_256(_R(IDS_EDITBH_NAME));
			ReturnVal = TRUE;
			break;

		case _R(IDC_EDIT_COLTYPE):
			HelpStringStore = String_256(_R(IDS_EDITBH_COLTYPE));
			ReturnVal = TRUE;
			break;

		case _R(IDC_EDIT_INHERIT1):
		case _R(IDC_EDIT_INHERIT2):
		case _R(IDC_EDIT_INHERIT3):
		case _R(IDC_EDIT_INHERIT4):
			HelpStringStore = String_256(_R(IDS_EDITBH_INHERIT));
			ReturnVal = TRUE;
			break;

		case _R(IDC_EDIT_PARENTCOL):
		case _R(IDC_EDIT_PARENTNAME):
			HelpStringStore = String_256(_R(IDS_EDITBH_PARENTCOL));
			ReturnVal = TRUE;
			break;

		case _R(IDC_EDIT_TINT):
		case _R(IDC_EDIT_TINTSLIDER):
			if (Bob != NULL && Bob->GetType() == COLOURTYPE_TINT && !Bob->TintIsShade())
				HelpStringStore = String_256(_R(IDS_EDITBH_TINT1));		// It's a tint
			else
				HelpStringStore = String_256(_R(IDS_EDITBH_TINT2));		// It's a shade
			ReturnVal = TRUE;
			break;

		case _R(IDC_EDIT_SHADE):
			HelpStringStore = String_256(_R(IDS_EDITBH_TINT2));			// It's a shade
			ReturnVal = TRUE;
			break;

		case _R(IDC_EDIT_ADVANCED):
			if (Editor->Folded)
				HelpStringStore = String_256(_R(IDS_EDITBH_ADVANCED1));
			else
				HelpStringStore = String_256(_R(IDS_EDITBH_ADVANCED2));
			ReturnVal = TRUE;
			break;

		case _R(IDC_EDIT_3D):
			HelpStringStore = String_256(_R(IDS_EDITBH_3D));
			ReturnVal = TRUE;
			break;

		case _R(IDC_EDIT_MAKESTYLE):
			HelpStringStore = String_256(_R(IDS_EDITBH_MAKESTYLE));
			ReturnVal = TRUE;
			break;

		case _R(IDC_EDIT_NOCOLOUR):
			HelpStringStore = String_256(_R(IDS_COLBAR_HNOCOLOUR));
			ReturnVal = TRUE;
			break;

		case _R(IDC_MAKE_LOCAL):
			HelpStringStore = String_256(_R(IDS_EDITBH_MAKE_LOCAL));
			ReturnVal = TRUE;
			break;

		case _R(IDC_EDIT_RENAME):	
			HelpStringStore = String_256(_R(IDS_EDITBH_RENAME));
			ReturnVal = TRUE;
			break;

		case _R(IDC_EDIT_LINEFILL):
			HelpStringStore = String_256(_R(IDS_EDITBH_LINEFILL));
			ReturnVal = TRUE;
			break;

		case _R(IDC_EDIT_216ONLY):
			HelpStringStore = String_256(_R(IDS_EDITBH_216ONLY));
			ReturnVal = TRUE;
			break;

		case _R(IDC_COLOURPICKER):
			HelpStringStore = String_256(_R(IDS_STATICCOLOURPICKERTOOLHELP));
			ReturnVal = TRUE;
			break;
	}

	if (ReturnVal)
		return((TCHAR *) HelpStringStore);

#endif

	return(NULL);
}


/********************************************************************************************

>	BOOL ColourPicker::GetComponentHelp(ColourContext* const pSourceContext, 
				const UINT32 ComponentIndex, StringBase& HelpString)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/05/96

	Inputs:		pSourceContext - the ColourContext in which the component is
				ComponentIndex - the index of the component in the colour context
				(1..GetComponentCount())
	Outputs:	HelpString - a string to be displayed as help for the given component
	Returns:	TRUE : If help string constructed correctly
				FALSE : otherwise
	Purpose:	Subprocedure for HelpCallbackHandler providing help for each of the colour
				components.
	Errors:		ERROR2IF's if invalid paramters given
				ERROR3IF's for corrupt data structures
	SeeAlso:	ColourPicker::HelpCallbackHandler

********************************************************************************************/
BOOL ColourPicker::GetComponentHelp(ColourContext* const pSourceContext, const UINT32 ComponentIndex, StringBase& HelpString)
{
	ERROR2IF(pSourceContext == NULL, FALSE, "NULL parameters not allowed");
	ERROR3IF(!pSourceContext->IS_KIND_OF(ColourContext), "ColourPicker::GetComponentHelp() - pSourceContext isn't");
	ERROR2IF((ComponentIndex < 1) || (ComponentIndex > pSourceContext->GetComponentCount()), FALSE,
				"Component index out of bounds");

	// Create a string of the form "Red (0..100%)" using the colour component's name and the limits defined for
	// the component's unit
	UnitGroup* pUnitGroup = pSourceContext->GetComponentUnitGroup(ComponentIndex);
	if (pUnitGroup == NULL)
	{
		ERROR3("ColourPicker::GetComponentHelp - Model has no units");
		return FALSE;
	}
	ERROR3IF(!pUnitGroup->IS_KIND_OF(UnitGroup), "ColourPicker::GetComponentHelp() - pUnitGroup isn't");
	ScaleUnit* pUnit = pUnitGroup->GetDefaultUnit();
	if (pUnit == NULL)
	{
		ERROR3("ColourPicker::GetComponentHelp - No default units");
		return FALSE;
	}
	ERROR3IF(!pUnit->IS_KIND_OF(ScaleUnit), "ColourPicker::GetComponentHelp() - pUnit isn't");

	String_64 CompName;
	pSourceContext->GetComponentName(ComponentIndex, &CompName, TRUE);

	String_32 LowerLimit;
	Convert::DoubleToString(pUnit->GetMin(), (StringBase*)&LowerLimit, pUnit->GetDPtoShow());

	String_32 UpperLimit;
	Convert::DoubleToString(pUnit->GetMax(), (StringBase*)&UpperLimit, pUnit->GetDPtoShow());

	String_32 QualifierToken;
	const Qualifier* pQualifier = pUnit->GetQualifier();
	if (pQualifier == NULL)
	{
		ERROR3("ColourPicker::GetComponentHelp - No qualifier");
		return FALSE;
	}
	ERROR3IF(!pQualifier->IS_KIND_OF(Qualifier), "ColourPicker::GetComponentHelp - Qualifier isn't");

	if (pQualifier->IsShown())
	{
		QualifierToken = pQualifier->GetToken();
	}

	HelpString.MakeMsg(_R(IDS_EDITBH_COMP234), (TCHAR *)CompName, (TCHAR *)LowerLimit, (TCHAR *)UpperLimit, 
						(TCHAR *)QualifierToken);

	return TRUE;
}


/********************************************************************************************

>	static void ColourPicker::UpdateBubbleHelpAndPointer(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/9/95

	Purpose:	Called on Idle (or MouseMove) events to provide bubble help and update
				the mouse pointer while it is over the colour editor dialogue.

	Notes:		This is basically a bodge until a proper dialogue bubblehelp system is
				available to do everything I need to do in the colour editor.

	SeeAlso:	ColourEditDlg::GetStatusLineText

********************************************************************************************/

void ColourPicker::UpdateBubbleHelpAndPointer(void)
{
#ifndef STANDALONE
	ColourEditDlg *Editor = ColourEditDlg::TheEditor;
	if (Editor == NULL)
		return;

	static HWND TheWindow = NULL;
	
	if (TheWindow == NULL)
		TheWindow = (HWND)Editor->WindowID;

	if (TheWindow == NULL)
		return;

	// We will update the bubble help every time we are called. However, we will only
	// bother checking through all the gadgets in the window a maximum of 10 times
	// per second. This stops the colour editor making a CPU interference buzzing noise
	// in my headphones while the pointer is idling over it!!
	static MonotonicTime LastUpdate;
	static UINT32 MousePos = NULL;

	if (LastUpdate.Elapsed(100))
	{
		// Remember when we last checked
		LastUpdate.Sample();

		// Default to the mouse being "nowhere special" again
		TheWindow = (HWND)Editor->WindowID;
		MousePos = NULL;

		POINT MouseScreenPos;
		if (::GetCursorPos(&MouseScreenPos))
		{
			// Only continue processing if the window under the pointer is the colour editor
			// or a child thereof. My machine makes a funny CPU buzz in my earphones when the
			// pointer is in the editor window, and it was doing this even when I had another
			// application maximised in front of camelot! ChildWindowFromPoint does not
			// determine if the child window is actually *visible*!

			HWND WindowUnder = ::WindowFromPoint(MouseScreenPos);

			if (WindowUnder != NULL &&
				(WindowUnder == TheWindow || ::GetParent(WindowUnder) == TheWindow))
			{
				POINT TempPos;
				TempPos.x = MouseScreenPos.x;
				TempPos.y = MouseScreenPos.y;

				// Convert to client coords in the main window
				::ScreenToClient(TheWindow, &TempPos);

				CPoint Pos(TempPos);
				HWND WindowUnderPointer = ::ChildWindowFromPoint(TheWindow, Pos);
				if (WindowUnderPointer != NULL && IsWindowVisible(WindowUnderPointer))
				{
					// Make sure that hidden windows do not provide status help!
					INT32 WindowStyle = ::GetWindowLong(WindowUnderPointer, GWL_STYLE);
					if ((WindowStyle & WS_VISIBLE) != 0)
					{
						HWND hGadget;
						INT32 i = 0;
						while (GadgetList[i] != NULL && MousePos == NULL)
						{
							hGadget = GetDlgItem(TheWindow, GadgetList[i]);
							if (WindowUnderPointer == hGadget)
							{
								MousePos = (UINT32)GadgetList[i];
								TheWindow = hGadget;
							}
							i++;
						}
					}
				}

				// Special check for Picker control (BODGE is a more accurate term!)
				// We look to see if the pointer is vaguely over the area of the picker used
				// to display the current/original colour patch, and only give help for that
				// region!

				if (MousePos == _R(IDC_EDIT_PICKER))
				{
					MousePos = NULL;	// Default to no help if anything goes wrong

					// Set up a ReDrawInfoType containing the click position info, and information
					// that will come in handy (size of the gadget, and screen DPI)
					ReDrawInfoType ExtraInfo;

					ExtraInfo.pDC = NULL;		// No redraw info for mouse events
					ExtraInfo.pClipRect = NULL;

					// Get the screen DPI
					HDC ScreenDC = CreateCompatibleDC(NULL);
					if (ScreenDC == NULL)
					{
						ERROR3("ColourPicker: Unable to create screen DC");
						return;
					}
					ExtraInfo.Dpi = GetDeviceCaps(ScreenDC, LOGPIXELSY);
					DeleteDC(ScreenDC);

					// Calculate how big the window is, in MILLIPOINTS
					RECT WindowSize;
					if (!GetClientRect(TheWindow, &WindowSize))
					{
						ERROR3("GetClientRect failed in ColourPicker");
						return;
					}

					ExtraInfo.dx = (((INT32)WindowSize.right)*72000) / ExtraInfo.Dpi;
					ExtraInfo.dy = (((INT32)WindowSize.bottom)*72000) / ExtraInfo.Dpi;

					// Work out the MILLIPOINT coordinates of the mouse position
					// Note that the Y value is flipped, as the kernel-origin is at the bottom left
					// and we have to first convert the screen mouse position into client coords
					ScreenToClient(TheWindow, &MouseScreenPos);

					INT32 XPos = (INT32) MouseScreenPos.x;
					INT32 YPos = (INT32) MouseScreenPos.y;

					DocCoord MouseInfo;
					MouseInfo.x = (XPos * 72000) / ExtraInfo.Dpi;
					MouseInfo.y = ExtraInfo.dy - ((YPos * 72000) / ExtraInfo.Dpi);
					ExtraInfo.pMousePos = &MouseInfo;

					// Now call the colour editor to handle the mouse position - it will
					// set the pointer shape as appropriate and also return us some bubble help.
					PickerBubbleBuffer = String_128("");
					StatusHelpBuffer = String_256("");
					Editor->HandleIdlePointer(&ExtraInfo, &PickerBubbleBuffer, &StatusHelpBuffer, &MousePos);

					if (MousePos == NULL)
						ControlHelper::BubbleHelpDisable();		// On non-helpful bit of picker - cancel bubble help
				}
			}
		}
	}

	// Finally, tell the bubble help system what help we want, if any
	if (MousePos != NULL)
	{
		// Set up our callback handler to show the help if/when necessary
		ControlHelper::DoBubbleHelpOn(TheWindow, MousePos, ColourPicker::HelpCallbackHandler, NULL);
	}
#endif
}






/********************************************************************************************

>	static BOOL ColourPicker::GetComponentAsString(IndexedColour *Source,
												ColourContext *DestContext, INT32 ComponentIndex,
												String_8 *Result)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/11/94
	Inputs:		Source - The colour from which you wish to extract the component
				DestContext - The colour context in which you want the resulting component
				ComponentIndex - The component you wish to extract (1..4)
				
	Outputs:	Result - returns containing a decimal (0.0 .. 100.0) string representing
				the value of the given component of the colour as it is defined in the
				given colour model (Note: HSV Hue is done as 0..360).

	Returns:	TRUE if the value converted was out of gamut.

	Purpose:	Retrieves the colour definition of the given colour, converts it to the
				given colour model, and returns the desired component of the definition
				in that model as a decimal string appropriate for placing into a colour
				editor dialogue gadget.

	Notes:		At present, this function clips the value to lie within the legal 0.0 to 1.0
				gamut range.

	Scope:		private

	SeeAlso:	ColourPicker::SetComponentFromDouble;
				ColourPicker::SetComponentFromString

********************************************************************************************/

BOOL ColourPicker::GetComponentAsString(IndexedColour *Source,
								 		ColourContext *DestContext, INT32 ComponentIndex,
								 		String_8 *Result)
{
	ERROR2IF(Source == NULL || DestContext == NULL || Result == NULL, FALSE,
				"NULL parameters are illegal");

	ERROR2IF(ComponentIndex < 1 || (UINT32)ComponentIndex > DestContext->GetComponentCount(), FALSE,
				"ComponentIndex not in range");

	BOOL OutOfGamut = FALSE;
	ColourGeneric Defn;

	DestContext->ConvertColour(Source, &Defn);
	
	// Sneakily get the component from the ColourGeneric by treating it as an array
	// of 4 ColourValue components.
	ColourValue *CompPtr = (ColourValue *) &Defn;		
	double CompValue = CompPtr[ComponentIndex-1].MakeDouble();

	if (CompValue < 0.0)
	{
		CompValue = 0.0;		// **** Clip out-of-gamut values.
		OutOfGamut = TRUE;		// Remember that this was out of gamut
	}

	if (CompValue > 1.0)
	{
		CompValue = 1.0;		// **** Clip out-of-gamut values.
		OutOfGamut = TRUE;		// Remember that this was out of gamut
	}

	// Generate a 0..100 value, except for Hue, in which case it's 0..360
	UnitGroup* pPossibleUnits = DestContext->GetComponentUnitGroup(ComponentIndex);
	if (pPossibleUnits == NULL)
	{
		ERROR3("ColourPicker::GetComponentAsString - pPossibleUnits NULL");
		return FALSE;
	}
	ERROR3IF(!pPossibleUnits->IS_KIND_OF(UnitGroup), "pPossibleUnits aren't");
	ScaleUnit* pUnit = pPossibleUnits->GetDefaultUnit();
	if (pUnit == NULL)
	{
		ERROR3("ColourPicker::GetComponentAsString - Default units NULL");
		return FALSE;
	}
	ERROR3IF(!pUnit->IS_KIND_OF(ScaleUnit), "pUnits isn't");
	pUnit->StringFromScale(CompValue, Result, 6);

	// Convert to a 1 d.p. string using the global conversion function
/*	String_32 TempString;
	Convert::DoubleToString(CompValue, (StringBase *) &TempString, pUnit->GetDPtoShow());
	TempString.Left(Result, 6);

	const Qualifier* pQualifier = pUnit->GetQualifier();
	if (pQualifier == NULL || !pQualifier->IS_KIND_OF(Qualifier))
	{
		ERROR3("ColourPicker::GetComponentAsString - not a Qualifier!");
		return FALSE;
	}
	if (pQualifier->IsShown())
		*Result += pQualifier->GetToken();
*/
	return(OutOfGamut);
}



/********************************************************************************************

>	static BOOL ColourPicker::GetComponentsAsHexString(IndexedColour *Source,
								 		    ColourContext *DestContext, INT32 ComponentIndex,
								 		    String_16 *Result)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/11/99
	Inputs:		Source - The colour from which you wish to extract the component
				DestContext - The colour context in which you want the resulting component
				ComponentIndex - This should be set to -1 at present (cause we are getting
				all components; and not one in the range 1-4)
				
	Outputs:	Result - returns containing a hexadecimal string representation (rrggbb)

	Returns:	TRUE if the value converted was out of gamut.

	Purpose:	Retrieves the colour definition of the given colour, converts it to the
				given colour model, and returns the combined value of all components
				of the definition in that model as a hexadecimal string appropriate for
				placing into a colour editor dialogue gadget.

	Notes:		At present, this function clips the value to lie within the legal 0.0 to 1.0
				gamut range.

	Scope:		private

	SeeAlso:	ColourPicker::GetComponentsAsString;
				ColourPicker::SetComponentsFromHexString

********************************************************************************************/

BOOL ColourPicker::GetComponentsAsHexString(IndexedColour *Source,
								 		    ColourContext *DestContext, INT32 ComponentIndex,
								 		    String_16 *Result)
{
	ERROR2IF(Source == NULL || DestContext == NULL || Result == NULL, FALSE,
				"NULL parameters are illegal");
//	ERROR2IF(!DestContext->IS_KIND_OF(ColourContextWebRGBT), FALSE,
//				"SourceContext parameter is not of type ColourContextWebRGBT");

	if (ComponentIndex == -1)
	{
		BOOL OutOfGamut = FALSE;
		ColourGeneric Defn;

		DestContext->ConvertColour(Source, &Defn);

		// Sneakily get the component from the ColourGeneric by treating it as an array
		// of 4 ColourValue components ....

		ColourValue *CompPtr = (ColourValue *) &Defn;		
		
		String_8 ResultR, ResultG, ResultB;

		for (INT32 i = 1; i < 4; i++)
		{
			double CompValue = CompPtr[i-1].MakeDouble();

			if (CompValue < 0.0)
			{
				CompValue = 0.0;		// **** Clip out-of-gamut values.
				OutOfGamut = TRUE;		// Remember that this was out of gamut
			}

			if (CompValue > 1.0)
			{
				CompValue = 1.0;		// **** Clip out-of-gamut values.
				OutOfGamut = TRUE;		// Remember that this was out of gamut
			}

			// Generate a 0..100 value, except for Hue, in which case it's 0..360
			UnitGroup* pPossibleUnits = DestContext->GetComponentUnitGroup(i);
			if (pPossibleUnits == NULL)
			{
				ERROR3("ColourPicker::GetComponentAsString - pPossibleUnits NULL");
				return FALSE;
			}
			ERROR3IF(!pPossibleUnits->IS_KIND_OF(UnitGroup), "pPossibleUnits aren't");
			ScaleUnit* pUnit = pPossibleUnits->FindUnitFromIndex (1);//GetDefaultUnit();
			if (pUnit == NULL)
			{
				ERROR3("ColourPicker::GetComponentAsString - Default units NULL");
				return FALSE;
			}
			ERROR3IF(!pUnit->IS_KIND_OF(ScaleUnit), "pUnits isn't");
			
			switch (i)
			{
				case 1:
					pUnit->StringFromScale(CompValue, &ResultR, 6);
				break;
				case 2:
					pUnit->StringFromScale(CompValue, &ResultG, 6);
				break;
				case 3:
					pUnit->StringFromScale(CompValue, &ResultB, 6);
				break;
			}
		}

		// convert strings into integers ....
		
		INT32 rVal = atoi ((TCHAR*) ResultR);
		INT32 gVal = atoi ((TCHAR*) ResultG);
		INT32 bVal = atoi ((TCHAR*) ResultB);

		// convert integers to base 16 ....
		
		_itoa (rVal, (TCHAR*) ResultR, 16);
		_itoa (gVal, (TCHAR*) ResultG, 16);
		_itoa (bVal, (TCHAR*) ResultB, 16);

		String_8 Builder;//FinalResultR, FinalResultG, FinalResultB;

		// I don't want an _itoa one length rr's, gg's or bb's thankyou very much!
		// AND glue the results together (== rrggbb) at the same time

		if (ResultR.Length () == 1)
		{
			Builder += (String_8 (TEXT ("0"))) += ResultR;
		}
		else
		{
			Builder += ResultR;
		}

		if (ResultG.Length () == 1)
		{
			Builder += (String_8 (TEXT ("0"))) += ResultG;
		}
		else
		{
			Builder += ResultG;
		}

		if (ResultB.Length () == 1)
		{
			Builder += (String_8 (TEXT ("0"))) += ResultB;
		}
		else
		{
			Builder += ResultB;
		}

		*Result = Builder;		// take that edit box - a correctly built hex string!
		
		//if ((ResultR.Length () == 1) || (ResultG.Length () == 1) || (ResultB.Length () == 1))
		//{
		//	*Result += /*(String_8 (TEXT ("0x"))) +=*/ FinalResultR += FinalResultG += FinalResultB;
		//}
		//else
		//{
		//	*Result += /*(String_8 (TEXT ("0x"))) +=*/ ResultR += ResultG += ResultB;
		//}

		return(OutOfGamut);
	}

	return (FALSE);		// stop that annoying warning about not all paths return a value!
}



/********************************************************************************************

>	static BOOL ColourPicker::SetComponentFromString(IndexedColour *Dest,
									ColourContext *SourceContext, INT32 ComponentIndex,
									String_8 *NewValue)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/11/94
	
	Inputs:		Dest - the colour to be set
				SourceContext - the context in which the new component value is defined
				ComponentIndex - the index [1..4] of the component being set, in that context
				NewValue - new value (decimal, as 0..100 or 0..360 for Hue) of the component
	Outputs:	Dest is updated appropriately
	Returns:	TRUE if the value was out of gamut and had to be clipped

	Purpose:	Converts AND COERCES the colour into the destination context.
				Assumes that the string will be 0..100 (or 0..360 for HSV)
				Clips the string value into a 0.0 to 1.0 in-gamut range, and then sets the
				given component of the converted colour to that value.
				This thus changes the colour model in which the colour is defined as well
				as the component value.

	Scope:		private

	SeeAlso:	ColourPicker::SetComponentFromDouble;
				ColourPicker::GetComponentAsString

********************************************************************************************/

BOOL ColourPicker::SetComponentFromString(IndexedColour *Dest,
									ColourContext *SourceContext, INT32 ComponentIndex,
									String_8 *NewValue)
{
	ERROR2IF(Dest == NULL ||  SourceContext == NULL || NewValue == NULL, FALSE, 
				"NULL Parameters are illegal");
	ERROR3IF(!Dest->IS_KIND_OF(IndexedColour) || !SourceContext->IS_KIND_OF(ColourContext),
				"Parameters incorrect classes");
	ERROR2IF(ComponentIndex < 1 || (UINT32)ComponentIndex > SourceContext->GetComponentCount(), 
				FALSE, "ComponentIndex not in range");

	UnitGroup* pPossibleUnits = SourceContext->GetComponentUnitGroup(ComponentIndex);
	if (pPossibleUnits == NULL)
	{
		ERROR3("ColourPicker::SetComponentFromString - SourceContext has NULL units");
		return FALSE;
	}
	ERROR3IF(!pPossibleUnits->IS_KIND_OF(UnitGroup), "ColourPicker::SetComponentFromString - pPossibleUnits aren't");

	double dNewVal;
	if (pPossibleUnits->ScaleFromString(*NewValue, dNewVal) == FALSE)
		return FALSE;

	// And set the new value
	return(SetComponentFromDouble(Dest, SourceContext, ComponentIndex, dNewVal));
}



/********************************************************************************************

>	static BOOL ColourPicker::IsValidHexString (String_16 Hex)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/11/99
	
	Inputs:		Hex - the string to interrogate to see if all characters are trully
				hexadecimal.
	Returns:	TRUE if success (i.e.  all characters in string were hexadecimal)

	Purpose:	Internal helper function for ColourPicker::SetComponentsFromHexString ()
				UNLESS YOU KNOW WHAT YOUR DOING - DON'T CALL THIS AS IT ONLY EXISTS
				TO HELP THE FORE-MENTIONED FUNCTION - AND I CANNOT SEE WHY YOUR CALLING
				THIS FROM ELSEWHERE ANYWAY !!!!

	Scope:		private

	SeeAlso:	ColourPicker::GetComponentAsHexString

********************************************************************************************/

BOOL ColourPicker::IsValidHexString (String_16 Hex)
{
	for (INT32 i = 0; i < Hex.Length(); i++)
	{
		char x = Hex[i];
		
		if (x >= '0' && x <= '9') {}
		else if (x >= 'A' && x <= 'F') {}
		else if (x >= 'a' && x <= 'f') {}
		else
		{
			return (FALSE);
		}
	}
	return (TRUE);
}



/********************************************************************************************

>	static BOOL ColourPicker::SetComponentsFromHexString(IndexedColour *Dest,
											  ColourContext *SourceContext,
											  String_16 *NewValue)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/11/99
	
	Inputs:		Dest - the colour to be set
				SourceContext - the context in which the new component value is defined
				NewValue - new value (hexadecimal) of the components (i.e.  rrggbb or
				0xrrggbb)
	Outputs:	Dest is updated appropriately
	Returns:	TRUE if success (i.e.  valid input and we could do things)

	Purpose:	Converts AND COERCES the colour into the destination context.
				Assumes that the string will in hex format (i.e  rrggbb or 0xrrggbb)
				Clips the string value into 0.0 to 1.0 in-gamut ranges, and then sets the
				components of the converted colour to those values.
				This thus changes the colour model in which the colour is defined as well
				as the component value.

	Scope:		private

	SeeAlso:	ColourPicker::SetComponentFromDouble;
				ColourPicker::GetComponentAsHexString

********************************************************************************************/

BOOL ColourPicker::SetComponentsFromHexString(IndexedColour *Dest,
											  ColourContext *SourceContext,
											  String_16 *NewValue)
{
	ERROR2IF(Dest == NULL ||  SourceContext == NULL || NewValue == NULL, FALSE, 
				"NULL Parameters are illegal");
	ERROR3IF(!Dest->IS_KIND_OF(IndexedColour) || !SourceContext->IS_KIND_OF(ColourContext),
				"Parameters incorrect classes");
//	ERROR2IF(!SourceContext->IS_KIND_OF(ColourContextWebRGBT), FALSE,
//				"SourceContext parameter is not of type ColourContextWebRGBT");

	// take a copy of the string - since were going to have to interrogate/parse it ....
	String_16 CopyNewValue (*NewValue);
	// convert to lower case (avoid trouble with caps etc.)
	CopyNewValue.toLower ();

	INT32 StringLength = CopyNewValue.Length ();//camStrlen ((TCHAR*) CopyNewValue);

	BOOL ValidStringParseSoFar = TRUE;
	BOOL Single0xPrefix = FALSE;
	BOOL SingleHashPrefix = FALSE;
	BOOL More0xPrefixs = FALSE;
	BOOL MoreHashPrefixs = FALSE;

	// decide if we have a 0x prefix, or a # prefix ....

	TCHAR* PtrPrefix = strstr ((TCHAR*) CopyNewValue, "0x");
	Single0xPrefix = (PtrPrefix != NULL) ? TRUE : FALSE;
	TCHAR* PtrPrefix2 = strstr ((TCHAR*) CopyNewValue, "#");
	SingleHashPrefix = (PtrPrefix2 != NULL) ? TRUE : FALSE;

	if (Single0xPrefix == TRUE)
	{
		CopyNewValue = PtrPrefix += 2;

		// not quite and simple as this - since we now need to see if theres (at least) one
		// other 0x - and if there is, ABORT (since that cannot be a valid hex number)

		PtrPrefix = strstr ((TCHAR*) CopyNewValue, "0x");
		More0xPrefixs = (PtrPrefix != NULL) ? TRUE : FALSE;

		ValidStringParseSoFar = More0xPrefixs ? FALSE : TRUE;

		if (ValidStringParseSoFar)
		{
			// scan for # ....

			PtrPrefix2 = strstr ((TCHAR*) CopyNewValue, "#");
			SingleHashPrefix = (PtrPrefix2 != NULL) ? TRUE : FALSE;
			ValidStringParseSoFar = SingleHashPrefix ? FALSE : TRUE;
		}

		if (!ValidStringParseSoFar)
		{
			return (FALSE);
		}
	}
	else if (SingleHashPrefix == TRUE)
	{
		CopyNewValue = PtrPrefix2 += 1;

		// not quite and simple as this - since we now need to see if theres (at least) one
		// other # - and if there is, ABORT (since that cannot be a valid hex number)

		PtrPrefix2 = strstr ((TCHAR*) CopyNewValue, "#");
		MoreHashPrefixs = (PtrPrefix2 != NULL) ? TRUE : FALSE;

		ValidStringParseSoFar = MoreHashPrefixs ? FALSE : TRUE;

		if (ValidStringParseSoFar)
		{
			// scan for 0x ....

			PtrPrefix = strstr ((TCHAR*) CopyNewValue, "0x");
			Single0xPrefix = (PtrPrefix != NULL) ? TRUE : FALSE;
			ValidStringParseSoFar = Single0xPrefix ? FALSE : TRUE;
		}

		if (!ValidStringParseSoFar)
		{
			return (FALSE);
		}
	}

	if (ValidStringParseSoFar == FALSE) return (FALSE);

	// weve made it this far - and have removed the trailing 0x
	// so now take a copy of the this new (copied) string and lets try to break
	// the string into components ....

	String_16 No0xPrefixNewValue (CopyNewValue);
	StringLength = No0xPrefixNewValue.Length ();

	BOOL rComponent = FALSE;
	BOOL bComponent = FALSE;
	BOOL gComponent = FALSE;

	String_8 rValStr;
	String_8 gValStr;
	String_8 bValStr;

	if ((StringLength <= 0) || (StringLength > 6)) return (FALSE);

	switch (StringLength)
	{
		case 2: rComponent = TRUE; break;
		case 4: rComponent = TRUE; gComponent = TRUE; break;
		case 6: rComponent = TRUE; gComponent = TRUE; bComponent = TRUE; break;
	}

	BOOL returnVal = FALSE;

	// now break the string and do conversions ....

	// NOTE - I could have made use of a for/while loop to reduce code size here; BUT
	// when I tried the BLOODY thing kept falling over - and it didn't seem worth it
	// for all the hastle ....

	if (IsValidHexString (No0xPrefixNewValue))
	{
		if (rComponent == TRUE)		// deal with the red component
		{	
			No0xPrefixNewValue.Split (&rValStr, &No0xPrefixNewValue, 2, FALSE);
			INT32 convertedVal = (INT32) strtol ((TCHAR*) rValStr, (TCHAR**) "\0", 16);//ToDec (rValStr);//atoi ((TCHAR*) rValStr);
			_itoa (convertedVal, (TCHAR*) rValStr, 10);

			UnitGroup* pPossibleUnits = SourceContext->GetComponentUnitGroup(1);
			if (pPossibleUnits == NULL)
			{
				ERROR3("ColourPicker::SetComponentFromString - SourceContext has NULL units");
				return FALSE;
			}
			ERROR3IF(!pPossibleUnits->IS_KIND_OF(UnitGroup), "ColourPicker::SetComponentFromString - pPossibleUnits aren't");

			ScaleUnit* pUnit = pPossibleUnits->FindUnitFromIndex (1);//GetDefaultUnit();
			if (pUnit == NULL)
			{
				ERROR3("ColourPicker::GetComponentAsString - Default units NULL");
				return FALSE;
			}
			ERROR3IF(!pUnit->IS_KIND_OF(ScaleUnit), "pUnits isn't");

			double dNewVal;
			Convert::StringToDouble(&rValStr, &dNewVal);
			pUnit->ConvertTo0to1 (dNewVal);

			// And set the new value
			returnVal = SetComponentFromDouble(Dest, SourceContext, 1, dNewVal);
		}

		if (gComponent == TRUE)		// deal with the green component
		{
			No0xPrefixNewValue.Split (&gValStr, &No0xPrefixNewValue, 2, FALSE);
			INT32 convertedVal = (INT32) strtol ((TCHAR*) gValStr, (TCHAR**) "\0", 16);//ToDec (rValStr);//atoi ((TCHAR*) rValStr);
			_itoa (convertedVal, (TCHAR*) gValStr, 10);

			UnitGroup* pPossibleUnits = SourceContext->GetComponentUnitGroup(2);
			if (pPossibleUnits == NULL)
			{
				ERROR3("ColourPicker::SetComponentFromString - SourceContext has NULL units");
				return FALSE;
			}
			ERROR3IF(!pPossibleUnits->IS_KIND_OF(UnitGroup), "ColourPicker::SetComponentFromString - pPossibleUnits aren't");

			ScaleUnit* pUnit = pPossibleUnits->FindUnitFromIndex (1);//GetDefaultUnit();
			if (pUnit == NULL)
			{
				ERROR3("ColourPicker::GetComponentAsString - Default units NULL");
				return FALSE;
			}
			ERROR3IF(!pUnit->IS_KIND_OF(ScaleUnit), "pUnits isn't");

			double dNewVal;
			Convert::StringToDouble(&gValStr, &dNewVal);
			pUnit->ConvertTo0to1 (dNewVal);

			// And set the new value
			returnVal = SetComponentFromDouble(Dest, SourceContext, 2, dNewVal);
		}

		if (bComponent == TRUE)		// deal with the blue component
		{
			No0xPrefixNewValue.Split (&bValStr, &No0xPrefixNewValue, 2, FALSE);
			INT32 convertedVal = (INT32) strtol ((TCHAR*) bValStr, (TCHAR**) "\0", 16);//ToDec (rValStr);//atoi ((TCHAR*) rValStr);
			_itoa (convertedVal, (TCHAR*) bValStr, 10);

			UnitGroup* pPossibleUnits = SourceContext->GetComponentUnitGroup(3);
			if (pPossibleUnits == NULL)
			{
				ERROR3("ColourPicker::SetComponentFromString - SourceContext has NULL units");
				return FALSE;
			}
			ERROR3IF(!pPossibleUnits->IS_KIND_OF(UnitGroup), "ColourPicker::SetComponentFromString - pPossibleUnits aren't");

			ScaleUnit* pUnit = pPossibleUnits->FindUnitFromIndex (1);//GetDefaultUnit();
			if (pUnit == NULL)
			{
				ERROR3("ColourPicker::GetComponentAsString - Default units NULL");
				return FALSE;
			}
			ERROR3IF(!pUnit->IS_KIND_OF(ScaleUnit), "pUnits isn't");

			double dNewVal;
			Convert::StringToDouble(&bValStr, &dNewVal);
			pUnit->ConvertTo0to1 (dNewVal);

			// And set the new value
			returnVal = SetComponentFromDouble(Dest, SourceContext, 3, dNewVal);
		}

		return (returnVal);
	}
	else
	{
		return (FALSE);		// that don't look like hex to me!
	}
}



/********************************************************************************************

>	static BOOL ColourPicker::SetComponentFromDouble(IndexedColour *Dest,
									ColourContext *SourceContext, INT32 ComponentIndex,
									double NewValue)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/11/94
	
	Inputs:		Dest - the colour to be set
				SourceContext - the context in which the new component value is defined
				ComponentIndex - the index [1..4] of the component being set, in that context
				NewValue - the new value (0.0 - 1.0) of the component
	Outputs:	Dest is updated appropriately
	Returns:	TRUE if the value was out of gamut and had to be clipped

	Purpose:	Converts AND COERCES the colour into the destination context.
				Clips the new value into a 0.0 to 1.0 in-gamut range, and then sets the
				given component of the converted colour to that value.

				This thus changes the colour model in which the colour is defined as well
				as the component value.

				The value is rounded as it is squeezed into the Fixed24 format used in
				colour definitions.

	Scope:		private

	SeeAlso:	ColourPicker::SetComponentFromString;
				ColourPicker::GetComponentAsString

********************************************************************************************/

BOOL ColourPicker::SetComponentFromDouble(IndexedColour *Dest,
									ColourContext *SourceContext, INT32 ComponentIndex,
									double NewValue)
{
	BOOL OutOfGamut = FALSE;

	ERROR3IF(Dest == NULL || SourceContext == NULL,
				"ColourPicker::SetComponentFromDouble - NULL Parameters are illegal");

	ERROR3IF(ComponentIndex < 1 || ComponentIndex > 4,
				"ColourPicker::SetComponentFromDouble- Index should be in range 1..4!");
	
	// Ensure it is in the correct colour model, and flush its output cache
	ForceColourModel(Dest, SourceContext);

	// Add half of the least-significant-bit-value to round the new value
	NewValue += 1.0 / ((double) (1<<25));

	// Clip to lie within gamut
	if (NewValue < 0.0)
	{
		NewValue = 0.0;
		OutOfGamut = TRUE;
	}

	if (NewValue > 1.0)
	{
		NewValue = 1.0;
		OutOfGamut = TRUE;
	}

	// Sneakily place the component back into the ColourGeneric by treating it as an array
	// of 4 ColourValue components.
	ColourValue *CompPtr = (ColourValue *) Dest->SourceColourPtr();
	CompPtr[ComponentIndex-1] = FIXED24(NewValue);

	return(OutOfGamut);
}



/********************************************************************************************

>	void ColourPicker::ForceColourModel(IndexedColour *Dest, ColourContext *SourceContext)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/11/94
	
	Inputs:		Dest - the colour to be set
				SourceContext - the context in which the new colour value should be defined

	Outputs:	Dest is updated appropriately
	Returns:	-

	Purpose:	COERCES the colour into the destination context.
				This ensures that the colour is defined in the given context's colour model,
				converting its definition if necessary.

	Notes:		This method always flushes the colour's output cache.

	Scope:		private

********************************************************************************************/

void ColourPicker::ForceColourModel(IndexedColour *Dest, ColourContext *SourceContext)
{
	ERROR3IF(Dest == NULL || SourceContext == NULL,
				"ColourPicker::ForceColourModel - NULL Parameters are illegal");

	// Invalidate the colour's output cache (even if it hasn't really changed - in this
	// case we're only causing one weeny little colour conversion, so it's no big deal)
	// This is also needed because the above functions expect this call to flush the cache.
	Dest->InvalidateCache();

	if (Dest->GetColourModel() != SourceContext->GetColourModel())
	{
		ColourContext *OldContext = ColourContext::GetGlobalDefault(Dest->GetColourModel());
		if (OldContext == NULL)
		{
			ERROR2RAW("Unable to get colour context for ForceColourModel");
			return;
		}

		// The colour must be coerced into the new model using secret squirrel private
		// methods which we can access only because we are a friend of IndexedColour
		ColourGeneric NewDefn;

		Dest->GetSourceColour(&NewDefn);	// Copy into temp space & convert back into the colour
		SourceContext->ConvertColour(OldContext, &NewDefn, Dest->SourceColourPtr());

		Dest->SetSourceColourModel(SourceContext->GetColourModel());

		// and once again, flush the cache to make sure the change in model has been noticed
		Dest->InvalidateCache();
	}
}



/********************************************************************************************

>	static BOOL ColourPicker::GetTintAsString(IndexedColour *Source, StringBase *Result)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/95

	Inputs:		Source - The colour from which you wish to extract the tint
				
	Outputs:	Result - returns containing a decimal (0.0 .. 100.0) string representing
				the value of the tint component of the colour

	Returns:	TRUE if the value converted was out of gamut.

	Purpose:	Retrieves the tint of the given colour as a decimal string appropriate 
				for placing into a colour editor dialogue gadget.

	Notes:		At present, this function clips the value to lie within the legal 0.0 to 1.0
				gamut range.

				If the colour is not a tint, will ENSURE and return "0%"

	Scope:		private

	SeeAlso:	ColourPicker::SetTintFromString

********************************************************************************************/

BOOL ColourPicker::GetTintAsString(IndexedColour *Source, StringBase *Result)
{
	ERROR3IF(Source == NULL || Result == NULL,
				"ColourPicker::GetComponentAsString - NULL parameters are illegal");

	if (Source->GetType() != COLOURTYPE_TINT)
	{
		ERROR3("ColourPicker::GetTintAsString: Source colour isn't a tint");
		*Result = TEXT(String_8(_R(IDS_CONVERT_ZERO_CHAR))); //TEXT("0");
		return(FALSE);
	}

	BOOL OutOfGamut = FALSE;
	
	double CompValue;
	if (Source->TintIsShade())
		CompValue = Source->GetShadeValueY().MakeDouble();
	else
		CompValue = Source->GetTintValue().MakeDouble();

	if (CompValue < 0.0)
	{
		CompValue = 0.0;		// **** Clip out-of-gamut values.
		OutOfGamut = TRUE;		// Remember that this was out of gamut
	}

	if (CompValue > 1.0)
	{
		CompValue = 1.0;		// **** Clip out-of-gamut values.
		OutOfGamut = TRUE;		// Remember that this was out of gamut
	}

	CompValue *= 100.0;

	String_32 TempString;
	Convert::DoubleToString(CompValue, (StringBase *) &TempString, 1);

	TempString.Left(Result, 7);

	return(OutOfGamut);
}



/********************************************************************************************

>	static BOOL ColourPicker::SetTintFromString(IndexedColour *Dest, StringBase *NewValue)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/95
	
	Inputs:		Dest - the colour to be set
				NewValue - new value (decimal, as 0..100) of the tint

	Outputs:	Dest is updated appropriately
	Returns:	TRUE if the value was out of gamut and had to be clipped

	Purpose:	Assumes that the string will be 0..100
				Clips the string value into a 0.0 to 1.0 in-gamut range, and then sets the
				tint component of the converted colour to that value.

	Notes:		ENSUREs and does nothing if the colour is not already a Tint

	Scope:		private

	SeeAlso:	ColourPicker::GetTintAsString

********************************************************************************************/

BOOL ColourPicker::SetTintFromString(IndexedColour *Dest, StringBase *NewValue)
{
	ERROR3IF(Dest == NULL || NewValue == NULL,
				"ColourPicker::SetTintFromString - NULL Parameters are illegal");
	
	if (Dest->GetType() != COLOURTYPE_TINT)
	{
		ERROR3("ColourPicker::SetTintFromString - colour isn't a tint");
		return(FALSE);
	}

	// Turn all percent signs in the number to spaces so the StringToDouble doesn't yell and scream
	String_8 TempString;
	NewValue->Left(&TempString, 7);
	TempString.SwapChar((TCHAR) '%', (TCHAR) ' ');

	// Convert the string to a double using the global conversion routine
	// Note that we ignore failure, because the number is validated below, and it
	// only seems to retun false if crap was typed on the end of the number anyway
	double NewVal;
	Convert::StringToDouble(&TempString, &NewVal);

	// Convert from percentage to 0.0-1.0 range
	NewVal /= 100.0;

	BOOL OutOfGamut = FALSE;

	// Add half of the least-significant-bit-value to round the new value
	NewVal += 1.0 / ((double) (1<<25));

	// Clip to lie within gamut
	if (NewVal < 0.0)
	{
		NewVal = 0.0;
		OutOfGamut = TRUE;
	}

	if (NewVal > 1.0)
	{
		NewVal = 1.0;
		OutOfGamut = TRUE;
	}

	// And set the new value
	if (Dest->TintIsShade())
	{
//		Dest->SetShadeValue(NewVal);
	}
	else
		Dest->SetTintValue(NewVal);

	return(OutOfGamut);
}



/********************************************************************************************

>	static BOOL ColourPicker::GetShadeValueAsString(IndexedColour *Source, INT32 ValueIndex, StringBase *Result)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/10/95

	Inputs:		Source - The colour from which you wish to extract the shade value
				ValueIndex - 1 to extract the X component
							 2 to extract the Y component

	Outputs:	Result - returns containing a decimal (-100.0 .. 100.0) string representing
				the value of the given shade component of the colour

	Returns:	TRUE if the value converted was out of gamut.

	Purpose:	Retrieves the shade of the given colour as a decimal string appropriate 
				for placing into a colour editor dialogue gadget.

	Notes:		At present, this function clips the value to lie within the legal -1.0 to 1.0
				gamut range.

				If the colour is not a shade, will ENSURE and return "0%"

	Scope:		private

	SeeAlso:	ColourPicker::SetShadeFromStrings

********************************************************************************************/

BOOL ColourPicker::GetShadeValueAsString(IndexedColour *Source, INT32 ValueIndex, StringBase *Result)
{
	ERROR3IF(Source == NULL || Result == NULL,
				"ColourPicker::GetShadeValueAsString - NULL parameters are illegal");

	ERROR3IF(ValueIndex != 1 && ValueIndex != 2, "ColourPicker::GetShadeValueAsString - Illegal ValueIndex parameter");

	if (Source->GetType() != COLOURTYPE_TINT)
	{
		ERROR3("ColourPicker::GetShadeValueAsString - Source colour isn't a tint");
		*Result = TEXT(String_8(_R(IDS_CONVERT_ZERO_CHAR))); // TEXT("0");
		return(FALSE);
	}

	BOOL OutOfGamut = FALSE;
	
	double CompValue;
	if (ValueIndex == 1)
		CompValue = -Source->GetShadeValueX().MakeDouble();
	else
		CompValue = Source->GetShadeValueY().MakeDouble();

	if (CompValue < -1.0)
	{
		CompValue = -1.0;		// **** Clip out-of-gamut values.
		OutOfGamut = TRUE;		// Remember that this was out of gamut
	}

	if (CompValue > 1.0)
	{
		CompValue = 1.0;		// **** Clip out-of-gamut values.
		OutOfGamut = TRUE;		// Remember that this was out of gamut
	}

	CompValue *= 100.0;

	String_32 TempString;
	Convert::DoubleToString(CompValue, (StringBase *) &TempString, 1);

	TempString.Left(Result, 7);

	return(OutOfGamut);
}



/********************************************************************************************

>	static BOOL ColourPicker::SetShadeFromStrings(IndexedColour *Dest, StringBase *NewValueX, StringBase *NewValueY)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/10/95
	
	Inputs:		Dest - the colour to be set
				NewValueX - new value (decimal, as -100..100) of the shade X component
				NewValueY - new value (decimal, as -100..100) of the shade Y component

	Outputs:	Dest is updated appropriately
	Returns:	TRUE if the value was out of gamut and had to be clipped

	Purpose:	Assumes that the string will be -100..100
				Clips the string value into a -1.0 to 1.0 in-gamut range, and then sets the
				shade components of the converted colour to those values.

	Notes:		ENSUREs and does nothing if the colour is not already a Shade

	Scope:		private

	SeeAlso:	ColourPicker::GetShadeValueAsString

********************************************************************************************/

BOOL ColourPicker::SetShadeFromStrings(IndexedColour *Dest, StringBase *NewValueX, StringBase *NewValueY)
{
	ERROR3IF(Dest == NULL || NewValueX == NULL || NewValueY == NULL,
				"ColourPicker::SetShadeFromStrings - NULL Parameters are illegal");
	
	if (Dest->GetType() != COLOURTYPE_TINT)
	{
		ERROR3("ColourPicker::SetShadeFromStrings - colour isn't a tint/shade");
		return(FALSE);
	}


	// Turn all percent signs in the number to spaces so the StringToDouble doesn't yell and scream
	String_8 TempString;
	NewValueX->Left(&TempString, 7);
	TempString.SwapChar((TCHAR) '%', (TCHAR) ' ');

	// Convert the string to a double using the global conversion routine
	// Note that we ignore failure, because the number is validated below, and it
	// only seems to retun false if crap was typed on the end of the number anyway
	double NewValX;
	Convert::StringToDouble(&TempString, &NewValX);

	// Convert from percentage to -1.0-1.0 range
	NewValX = (-NewValX) / 100.0;

	BOOL OutOfGamut = FALSE;

	// Add half of the least-significant-bit-value to round the new value
	NewValX += 1.0 / ((double) (1<<25));

	// Clip to lie within gamut
	if (NewValX < -1.0)
	{
		NewValX = -1.0;
		OutOfGamut = TRUE;
	}

	if (NewValX > 1.0)
	{
		NewValX = 1.0;
		OutOfGamut = TRUE;
	}


	// Turn all percent signs in the number to spaces so the StringToDouble doesn't yell and scream
	NewValueY->Left(&TempString, 7);
	TempString.SwapChar((TCHAR) '%', (TCHAR) ' ');

	// Convert the string to a double using the global conversion routine
	// Note that we ignore failure, because the number is validated below, and it
	// only seems to retun false if crap was typed on the end of the number anyway
	double NewValY;
	Convert::StringToDouble(&TempString, &NewValY);

	// Convert from percentage to -1.0-1.0 range
	NewValY /= 100.0;

	// Add half of the least-significant-bit-value to round the new value
	NewValY += 1.0 / ((double) (1<<25));

	// Clip to lie within gamut
	if (NewValY < -1.0)
	{
		NewValY = -1.0;
		OutOfGamut = TRUE;
	}

	if (NewValY > 1.0)
	{
		NewValY = 1.0;
		OutOfGamut = TRUE;
	}

	Dest->SetShadeValues(FIXED24(NewValX), FIXED24(NewValY));

	return(OutOfGamut);
}



/********************************************************************************************

>	static void ColourPicker::SetWindowExtent(CWindowID WindowID,
												CGadgetID XGadgetID, CGadgetID YGadgetID)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/11/94
	
	Inputs:		WindowID - The DialogOp->WindowID member variable of the Dialogue
				XGadgetID - The Gadget ID of the gadget to be at the right side of the dlg
				YGadgetID - The Gadget ID of the gadget to be at the bottom of the dlg

	Purpose:	Private function used by the kernel ColourEditDlg editor.
				Given a WindowID and GadgetID (cf DialogOp->DialogMgr interface), this
				sets the window extent of the window to a short distance outside the right/
				bottom edges of the X/Y named gadgets respectively.

				The colour editor uses this to 'fold' and 'unfold' its window to hide/show
				the advanced editing options.

	Scope:		private

********************************************************************************************/

void ColourPicker::SetWindowExtent(CWindowID WindowID,
									CGadgetID XGadgetID, CGadgetID YGadgetID)
{
	HWND hXGadget = GetDlgItem((HWND)WindowID, (INT32)XGadgetID);
	HWND hYGadget = GetDlgItem((HWND)WindowID, (INT32)YGadgetID);

	RECT MainWinPos;
	RECT XGadgetPos;
	RECT YGadgetPos;
	BOOL ok = TRUE;

	// We can't use GetWindowRect() for the gadgets then use these values when adjusting
	// the values you get from GetWindowPlacement() for the main window, because they use two
	// different origins.
	//
	// Basically, using the above calls will result in errors if the task bar is at the top of the
	// screen, because one call uses the actual screen top-left coord, the other uses the top-ledt coord
	// of the desk top excluding the task bar.
	//
	// The method used here uses GetWindowRect() for all three gadgets to work out a relative difference
	// between the gadgets.  It then uses GetWindowPosition() & SetWindowPosition() which work consistantly
	// because they use the same coord space.

	// Collect the screen positions of the three items
	if (ok) ok = GetWindowRect((HWND)WindowID,&MainWinPos);
	if (ok) ok = GetWindowRect(hXGadget,&XGadgetPos);
	if (ok) ok = GetWindowRect(hYGadget,&YGadgetPos);

	if (ok)
	{
		// dx & dy are calculated to be the amount we need to adjust the right & bottom of the
		// dlg to come in line with the two gadgets given
		INT32 dx = MainWinPos.right  - XGadgetPos.right;
		INT32 dy = MainWinPos.bottom - YGadgetPos.bottom;

		// Get the window's screen position
		if (DialogManager::GetWindowPosition(WindowID, &MainWinPos))
		{
			// Adjust it (with added help from a bodge value)
			MainWinPos.right  -= dx - 6;
			MainWinPos.bottom -= dy - 6;

			// Set it back again
			DialogManager::SetWindowPosition(WindowID, MainWinPos);
		}
	}
}


/********************************************************************************************

>	static void ColourPicker::SetGadgetPositions(CWindowID WindowID,
												CGadgetID *Gadgets, CGadgetID MoveUnder)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/11/94
	
	Inputs:		WindowID - The DialogOp->WindowID member variable of the Dialogue
				
				Gadgets - A NULL-terminated list of Gadget IDs of the gadgets to be
				moved. The first entry in this list is the anchor gadget - it will be
				moved just under 'MoveUnder', and the others will stay in the same position
				relative to the first gadget in the list.

				MoveUnder - The gadget ID of the gadget under which all these gadgets
				should be positioned, or NULL to 'hide' the gadgets well off the window

	Purpose:	Private function used by the kernel ColourEditDlg editor.
				Given a WindowID and GadgetIDs (cf DialogOp->DialogMgr interface), this
				moves all the gadgets such that (a) All the gadgets in the group remain
				in the same relative positions, and (b) the Gadgets[0] gadget is moved
				to lie just underneath the MoveUnder gadget.

				The X positions of the gadgets are unaffected by this call.

				If MoveUnder == NULL, then the gadgets are moved a large distance away so
				that they will not appear in the visible portion of the window.

				This is used to move tint/link options to the bottom of the editor window,
				while hiding the link/tint options gadgtes respectively.

	Scope:		private

********************************************************************************************/

void ColourPicker::SetGadgetPositions(CWindowID WindowID,
										CGadgetID *Gadgets, CGadgetID MoveUnder)
{
	if (Gadgets == NULL || Gadgets[0] == NULL)
		return;

	HWND hGadget;
	RECT MoveRect;
	POINT TopLeft;
	INT32 YShift = 10000;		// If MoveUnder == NULL, move out of the way

	if (MoveUnder != NULL)
	{
		// Get the positions of the MoveUnder and Gadgets[0] gadgets, and work out
		// the Y Shift to move Gadgets[0] just under the MoveUnder gadget.
		// All calculations are done in Client coords (not screen coords)
		hGadget = GetDlgItem((HWND)WindowID, (INT32)MoveUnder);
		if (!GetWindowRect(hGadget, &MoveRect))
			return;

		TopLeft.x = MoveRect.left;
		TopLeft.y = MoveRect.bottom;
		ScreenToClient((HWND)WindowID, &TopLeft);
		YShift = -(TopLeft.y + 8);		// Remember first part of the shift

		hGadget = GetDlgItem((HWND)WindowID, (INT32)Gadgets[0]);
		if (!GetWindowRect(hGadget, &MoveRect))
			return;

		TopLeft.x = MoveRect.left;
		TopLeft.y = MoveRect.top;
		ScreenToClient((HWND)WindowID, &TopLeft);

		YShift += TopLeft.y;			// Complete the shift calculation
	}
	else
	{
		// Move the gadgets out of view: Check if we need to bother to do this
		// (are they aready out of view?)

		hGadget = GetDlgItem((HWND)WindowID, (INT32)Gadgets[0]);
		if (!GetWindowRect(hGadget, &MoveRect))
			return;

		TopLeft.x = MoveRect.left;
		TopLeft.y = MoveRect.top;
		ScreenToClient((HWND)WindowID, &TopLeft);

		if (abs(TopLeft.y) > 8000)		// Is it already out of the way?
			YShift = 0;					// Yes - so don't shift it
	}

	if (YShift == 0)					// No shift needed!
		return;

	INT32 Index = 0;
	while (Gadgets[Index] != 0)
	{
		hGadget = GetDlgItem((HWND)WindowID, (INT32)Gadgets[Index]);

		if (GetWindowRect(hGadget, &MoveRect))
		{
			TopLeft.x = MoveRect.left;		// Convert TopLeft coord into client coords
			TopLeft.y = MoveRect.top;
			ScreenToClient((HWND)WindowID, &TopLeft);
			TopLeft.y -= YShift;

			SetWindowPos(hGadget, NULL,
							TopLeft.x, TopLeft.y, 0, 0,
							SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOZORDER);
		}

		Index++;
	}
}



/********************************************************************************************

>	static void ColourPicker::SetComponentGadgets(CWindowID WindowID, CGadgetID *Gadgets,
													CGadgetID PickerGadget,
													ColourModel ModelToDisplay)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/4/95
	
	Inputs:		WindowID - The DialogOp->WindowID member variable of the Dialogue
				
				Gadgets - A NULL-terminated list of Gadget IDs of the gadgets to be
				moved/resized. There MUST be 8 gadget IDs, paired up into (name,writable)
				sets.

				PickerGadget - specifies the gadget under which the controls are spaced.
				If they can be fitted, they will maybe be centered under this control
				in order to look neater.

				ModelToDisplay - Indicates the model you're going to put into the controls
				This is used to determine how many components should be shown and stuff.

	Purpose:	Private function used by the kernel ColourEditDlg editor.

				This takes the 4 component names and component writable fields and spreads
				them out within the window to allow the names to go next to the writables.
				The name fileds are resized to fill the available spaces, so you should
				make them right-aligned to keep the text a set disatnce from the writable
				for all colour models.

				It's still up to you to set the contents of the controls, and hide any extra
				ones (so as much of this crap goes into the kernel as possible). This only
				changes the positions of the first NumComponents control-pairs.

				It also hides ALL of the windows. This means that while swapping models
				they go off and on, but it looks a lot better than the user seeing all
				the faffing arounf as they are moved, resized, and have their contents
				changed.

	Scope:		private

********************************************************************************************/

void ColourPicker::SetComponentGadgets(CWindowID WindowID, CGadgetID *Gadgets,
													CGadgetID PickerGadget,
													ColourModel ModelToDisplay)
{
	// Find out how many visible components we're dealing with...
	ColourContext *cc = ColourContext::GetGlobalDefault(ModelToDisplay);
	INT32 NumComponents = 0;
	INT32 i;
	if (cc != NULL)
	{
		for (i = 1; i <= 4; i++)
		{
			if (cc->GetComponentName(i, NULL))
				NumComponents++;
		}
	}

	if (NumComponents < 1)
		return;


	RECT TheRect;

	HWND hGadget;
	INT32 Left = 8;
	INT32 MaxWidth = 0;
//	if (NumComponents < 3)
	{
		// Use width of colour picker control
		hGadget = GetDlgItem((HWND)WindowID, (INT32)PickerGadget);
		if (hGadget && GetWindowRect(hGadget, &TheRect))
		{
			// convert screen rect to client position within the parent wnd
			::MapWindowPoints(NULL, (HWND)WindowID, (LPPOINT) &TheRect, 2);

			MaxWidth = TheRect.right - TheRect.left;
			Left = TheRect.left;
		}
	}
/*
	else
	{
		// Use width of the window
		if (GetClientRect((HWND)WindowID, &TheRect))
		{
			MaxWidth = TheRect.right - TheRect.left;
			MaxWidth -= 2 * Left;
		}
	}
*/
	if (MaxWidth < 1)
		return;


	// We must now fit NumComponents items into MaxWidth pixels
	INT32 PairWidth = MaxWidth / NumComponents;

	// Read the writable control size and y position
	static INT32 BaseWritableWidth = 0;
	hGadget = GetDlgItem((HWND)WindowID, (INT32)Gadgets[1]);
	
	if (GetWindowRect(hGadget, &TheRect))
	{
		// Only read the static writable width the first time we do this,
		// so that we remember the original size before we started resizing these controls
		if (BaseWritableWidth == 0)
			BaseWritableWidth = TheRect.right - TheRect.left;
	}

	INT32 WritableWidth = BaseWritableWidth;
	if (WritableWidth < 1)
		return;	

	// If doing 4 components, we use the width of the original writable controls
	// If doing fewer components, we make the writables a bit bigger, as there
	// is some spare room around to absorb.
	if (NumComponents < 4)
		WritableWidth += 4;

// WEBSTER - markn 9/1/97
// We make the room available to the ed fields for HSV be the same as RGB
// The WritableWidth used to only be adjusted for RGB 
//
	// The RGBT model has even more room availble ...
	if (ModelToDisplay == COLOURMODEL_RGBT || ModelToDisplay == COLOURMODEL_HSVT)
		WritableWidth += 8;


	const INT32 Gap = 3;			// Min. gap between adjacent fields
	if (PairWidth - (2*Gap + WritableWidth) > 64)
	{
		// There is heaps of space. Limit to the max size, and shift the left
		// and maxwidth to keep the controls centered.
		PairWidth = 64 + 2*Gap + WritableWidth;

		Left += (MaxWidth - (NumComponents * PairWidth)) / 2;
		MaxWidth  = NumComponents * PairWidth;
	}


	// Convert the rect from screen to client coords within the parent wnd.
	::MapWindowPoints(NULL, (HWND)WindowID, (LPPOINT) &TheRect, 2);


	// Find the top/bottom position for the text gadgets, in client coords
	RECT TextRect;
	hGadget = GetDlgItem((HWND)WindowID, (INT32)Gadgets[0]);
	if (hGadget == NULL || !GetWindowRect(hGadget, &TextRect))
		return;
	::MapWindowPoints(NULL, (HWND)WindowID, (LPPOINT) &TextRect, 2);


	// For each component, place the controls
	const INT32 NewWidth  = PairWidth - (2*Gap + WritableWidth);
	const INT32 NewHeight = ABS(TheRect.bottom - TheRect.top);	// Don't change the height
	INT32 LeftEnd;

	for (i = 0; i < NumComponents; i++)
	{
		LeftEnd = Left + (i * PairWidth);

		// Move and resize (in x axis only) the text gadget
		hGadget = GetDlgItem((HWND)WindowID, (INT32)Gadgets[i*2]);
		if (hGadget)
		{
			if (i < NumComponents)
			{
				SetWindowPos(hGadget, NULL,
								LeftEnd, TextRect.top, NewWidth, NewHeight,
								SWP_HIDEWINDOW | SWP_NOACTIVATE | SWP_NOZORDER |
								SWP_NOCOPYBITS | SWP_NOREDRAW);
			}
			else
				ShowWindow(hGadget, SW_HIDE);
		}

		LeftEnd += NewWidth + Gap;

		// Move (in x axis only) the writable gadget
		hGadget = GetDlgItem((HWND)WindowID, (INT32)Gadgets[(i*2)+1]);
		if (hGadget)
		{
			if (i < NumComponents)
			{
				SetWindowPos(hGadget, NULL,
								LeftEnd, TheRect.top, WritableWidth, NewHeight,
								SWP_HIDEWINDOW | SWP_NOACTIVATE | SWP_NOZORDER |
								SWP_NOCOPYBITS | SWP_NOREDRAW);
			}
			else
				ShowWindow(hGadget, SW_HIDE);
		}
	}

	// Finally, redraw the affected strip of the window, because the controls won't
	TheRect.left = 0;		// Left edge of window
	TheRect.right = 0x1000;	// Infinity as far as the window is concerned
	::InvalidateRect((HWND)WindowID, &TheRect, TRUE);
}




/********************************************************************************************

>	static void ColourPicker::SetFixedComponentGadgets(CWindowID WindowID, CGadgetID *Gadgets,
													CGadgetID PickerGadget,
													ColourModel ModelToDisplay)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/02/2004
	
	Inputs:		WindowID - The DialogOp->WindowID member variable of the Dialogue
				
				Gadgets - A NULL-terminated list of Gadget IDs of the gadgets to be
				moved/resized. There MUST be 8 gadget IDs, paired up into (name,writable)
				sets.

				PickerGadget - specifies the gadget under which the controls are spaced.
				If they can be fitted, they will maybe be centered under this control
				in order to look neater.

				ModelToDisplay - Indicates the model you're going to put into the controls
				This is used to determine how many components should be shown and stuff.

	Purpose:	Private function used by the kernel ColourEditDlg editor.

				This takes the 5 component names and component writable fields and spreads
				them out within the window to allow the names to go next to the writables.
				The name fileds are resized to fill the available spaces, so you should
				make them right-aligned to keep the text a set disatnce from the writable
				for all colour models.

				It's still up to you to set the contents of the controls, and hide any extra
				ones (so as much of this crap goes into the kernel as possible). This only
				changes the positions of the first NumComponents control-pairs.

				It also hides ALL of the windows. This means that while swapping models
				they go off and on, but it looks a lot better than the user seeing all
				the faffing arounf as they are moved, resized, and have their contents
				changed.

	Scope:		private

********************************************************************************************/

void ColourPicker::SetFixedComponentGadgets(CWindowID WindowID, CGadgetID *Gadgets,
													CGadgetID PickerGadget,
													ColourModel ModelToDisplay,
													INT32 TextWidth[],
													INT32 EditWidth[]
													)
{
	RECT TheRect;

	HWND hGadget;
	INT32 Left = 0;
	INT32 MaxWidth = 0;
//	if (NumComponents < 3)
	{
		// Use width of colour picker control
		hGadget = GetDlgItem((HWND)WindowID, (INT32)PickerGadget);
		if (hGadget && GetWindowRect(hGadget, &TheRect))
		{
			// convert screen rect to client position within the parent wnd
			::MapWindowPoints(NULL, (HWND)WindowID, (LPPOINT) &TheRect, 2);

			MaxWidth = TheRect.right - TheRect.left;
			Left = TheRect.left;
		}
	}

	// Use width of the window
//	if (GetClientRect((HWND)WindowID, &TheRect))
//	{
//		MaxWidth = TheRect.right - TheRect.left;
//		MaxWidth -= 2 * Left;
//		Left = 0;
//	}

//	if (MaxWidth < 1)
//		return;
//

	// Find the top/bottom position for the text gadgets, in client coords
	RECT TextRect;
	hGadget = GetDlgItem((HWND)WindowID, (INT32)Gadgets[0]);
	if (hGadget == NULL || !GetWindowRect(hGadget, &TextRect))
		return;
	::MapWindowPoints(NULL, (HWND)WindowID, (LPPOINT) &TextRect, 2);
	INT32 TextHeight = TextRect.bottom-TextRect.top;

	// Get the current position of the first Editable control
	RECT EditRect;
	hGadget = GetDlgItem((HWND)WindowID, (INT32)Gadgets[1]);
	GetWindowRect(hGadget, &EditRect);
	::MapWindowPoints(NULL, (HWND)WindowID, (LPPOINT) &EditRect, 2);
	INT32 EditHeight = EditRect.bottom-EditRect.top;

	// For each component, place the controls
	INT32 LeftEnd = Left;
	INT32 i=0;
	const INT32 Gap = 4;			// Min. gap between adjacent fields
	const INT32 LabelGap = 1;

	for (i = 0; i < 5; i++)
	{
		// Move and resize (in x axis only) the text gadget
		hGadget = GetDlgItem((HWND)WindowID, (INT32)Gadgets[i*2]);
		if (hGadget)
		{
			if (TextWidth[i]!=0)
			{
				SetWindowPos(hGadget, NULL,
								LeftEnd, TextRect.top, TextWidth[i], TextHeight,
								SWP_HIDEWINDOW | SWP_NOACTIVATE | SWP_NOZORDER |
								SWP_NOCOPYBITS | SWP_NOREDRAW);

				LeftEnd += TextWidth[i] + LabelGap;
			}
			else
				ShowWindow(hGadget, SW_HIDE);

		}

		// Move (in x axis only) the writable gadget
		hGadget = GetDlgItem((HWND)WindowID, (INT32)Gadgets[(i*2)+1]);
		if (hGadget)
		{
			if (EditWidth[i]!=0)
			{
				SetWindowPos(hGadget, NULL,
								LeftEnd, EditRect.top, EditWidth[i], EditHeight,
								SWP_HIDEWINDOW | SWP_NOACTIVATE | SWP_NOZORDER |
								SWP_NOCOPYBITS | SWP_NOREDRAW);

				LeftEnd += EditWidth[i] + Gap;
			}
			else
				ShowWindow(hGadget, SW_HIDE);

		}

	}

	// Finally, redraw the affected strip of the window, because the controls won't
	EditRect.left = 0;		// Left edge of window
	EditRect.right = 0x1000;	// Infinity as far as the window is concerned
	::InvalidateRect((HWND)WindowID, &EditRect, TRUE);
}




