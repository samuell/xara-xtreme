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
// Selector Tool info-bar operations

/*
*/

#include "camtypes.h"
#include "dlgtypes.h"
#include "selector.h"
#include "selinfo.h"
#include "selmsg.h"
#include "spread.h"
#include "app.h"
#include "docview.h"
#include "rndrgn.h"
#include "opnudge.h"
//#include "justin.h"
#include "fillattr.h"	// For CC_RUNTIME_CLASS(AttrFillChanged)
#include "optsmsgs.h"
#include "userrect.h"
//#include "barsdlgs.h"


//Matt 11/11/00 - I only wanted a function from the next line...
#include "slicehelper.h"	//For helper functions

//But I had to include all of these to get it to work!!...
#include "cxfrech.h"		//For CamelotRecordHandler
#include "userattr.h"		//For UserAttr
#include "tmpltatr.h"		//For TemplateAttribute


DECLARE_SOURCE( "$Revision$" );
CC_IMPLEMENT_DYNCREATE(SelectorInfoBarOp, InformationBarOp)

#define new CAM_DEBUG_NEW


// This defines a pointer to a SelectorTool member function that takes two INT32s and
// returns nothing.  It is used to present a uniform interface to the DoXXXXImmediate
// functions, which respond to changes in the info-bar edit-fields.
typedef void (SelectorTool::*SELTOOLFUNC)(INT32, INT32);


// These maintain the state of the info-bar's buttons.
BOOL SelectorInfoBarOp::fLockAspect = TRUE;
BOOL SelectorInfoBarOp::fRotateMode = FALSE;
BOOL SelectorInfoBarOp::fScaleLines = TRUE;
// BOOL SelectorInfoBarOp::fLeaveCopy = FALSE;
// BOOL SelectorInfoBarOp::fTransFills = TRUE;



/********************************************************************************************
>	SelectorInfoBarOp::SelectorInfoBarOp()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/7/94
	Purpose:	Default constructor for a SelectorInfoBarOp.
	SeeAlso:	class InformationBarOp
********************************************************************************************/

SelectorInfoBarOp::SelectorInfoBarOp()
  :	pSelectorTool(NULL)
{
	DlgResID = _R(IDD_SELECTORINFO);
}



/********************************************************************************************
>	virtual MsgResult SelectorInfoBarOp::Message(Msg* pMessage) 

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/7/94
	Inputs:		pMessage - A pointer to a dialog message.
	Returns:	The result of the message.
	Purpose:	Takes a look at the message and if it is for for selector tool info-bar
				then respond to it.
	Errors:		-
	SeeAlso:	SelectorInfoBarOp::HandleCreate; SelectorInfoBarOp::HandleButtonDown;
				SelectorInfoBarOp::HandleSelChange; SelectorTool::SelectionHasChanged
********************************************************************************************/

MsgResult SelectorInfoBarOp::Message(Msg* pMessage) 
{
	// If we aren't the current tool then pass to the base class.
	if (((SelectorTool*) Tool::GetCurrent()) != pSelectorTool /* || !HasWindow() */)
	{
		return InformationBarOp::Message(pMessage);
	}

	// Does this message mean that the selected object has changed, or its attributes (bounds)?
	if (pMessage->IsKindOf(CC_RUNTIME_CLASS(SelChangingMsg)))
	{	
		if (pSelectorTool != NULL)
		{
			switch (((SelChangingMsg*) pMessage)->State)
			{
			case SelChangingMsg::SELECTIONCHANGED:
				if (!pSelectorTool->fIgnoreSelChange) pSelectorTool->SelectionHasChanged();
				break;
			
			case SelChangingMsg::NONCOLOURATTCHANGED:
				pSelectorTool->AttributeHasChanged();
				break;
			
			default:
				break;
			}
		}
	}

	// How about a view changing message?
	else if (MESSAGE_IS_A(pMessage, DocViewMsg))
	{
		// Yes, tell the selector tool to sort its blobs out.
		if (pSelectorTool != NULL)
		{
			pSelectorTool->ViewChanged(*((DocViewMsg*) pMessage));
		}
	}

	// Or an options-changing message?
	else if (MESSAGE_IS_A(pMessage, OptionsChangingMsg))
	{
		// Yes, so force the edit-fields to reconvert themselves to the new settings.
		TRACEUSER( "JustinF", _T("Updating on options-change in selector tool info-bar\n"));
		SetEdit_OnSelectionChange();
	}

	// Otherwise, is this a dialog message for us?
	else if (IS_OUR_DIALOG_MSG(pMessage))
	{
		// Yes.  Act on the dialog message.
		DialogMsg* pDlgMsg = (DialogMsg*) pMessage;
		switch (pDlgMsg->DlgMsg)
		{			
			// Initialise the bar's controls.
			case DIM_CREATE:
				HandleCreate(pDlgMsg);
				break;
				
			// "Close" the bar.
			case DIM_CANCEL:
				SelectorTool::UnCacheInfoBar(TRUE);
				break;

			// Handle a button being clicked.
			case DIM_LFT_BN_CLICKED:
				if (!HandleButtonDown(pDlgMsg)) return DialogBarOp::Message(pMessage);
				break;

			// Handle a button being released.
			case DIM_LFT_BN_UP:
				HandleButtonUp(pDlgMsg);
				break;

			// Handle ENTER being typed in an edit field.
			case DIM_SELECTION_CHANGED:
				HandleEditCommit(pDlgMsg);
				break;

			// Handle a "telephone keypad" button message.
			case DIM_GRID_BUTTON_DOWN:
				HandleGridButton(pDlgMsg);
				break;

			// Some other message irrelevant to us.
			default:
				break;
		}
	}

	// Pass message on to base class.
	return InformationBarOp::Message(pMessage); 
}



/********************************************************************************************
>	BOOL SelectorInfoBarOp::IsAspectLocked() const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/8/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the "Lock Aspect Ratio" button is down.
	Purpose:	Enquires of the state of the "Lock Aspect Ratio" button.
	Errors:		-
	SeeAlso:	-
********************************************************************************************/

BOOL SelectorInfoBarOp::IsAspectLocked() const
{
	return fLockAspect;
}



/********************************************************************************************
>	BOOL SelectorInfoBarOp::InRotateMode() const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/8/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the "Rotate/Bounds blobs" button is down.
	Purpose:	Checks the state of the "Rotate/Bounds blobs" button.
	Errors:		-
	SeeAlso:	-
********************************************************************************************/

BOOL SelectorInfoBarOp::InRotateMode() const
{
	return fRotateMode;
}



/********************************************************************************************
>	BOOL SelectorInfoBarOp::ShouldLeaveCopy() const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/8/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the "Leave Copy" button is down.
	Purpose:	Checks the state of the "Leave Copy" button.
	Errors:		-
	SeeAlso:	-
********************************************************************************************/
/*
BOOL SelectorInfoBarOp::ShouldLeaveCopy() const
{
	return fLeaveCopy;
}
*/


/********************************************************************************************
>	BOOL SelectorInfoBarOp::ShouldScaleLines() const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/8/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the "Scale Lines" button is down.
	Purpose:	Checks the state of the "Scale Lines" button.
	Errors:		-
	SeeAlso:	-
********************************************************************************************/

BOOL SelectorInfoBarOp::ShouldScaleLines() const
{
	return fScaleLines;
}



/********************************************************************************************

>	BOOL SelectorInfoBarOp::DontConsiderAttrs(void) const

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/2/96
	Returns:	TRUE if attributes should NOT be considered in width/height displays

	Purpose:	Checks the "DontConsiderAttrs" state.
				This is used to determine if displays and editing of widths and heights
				of objects should include the effect of attributes - for example, line
				widths can significantly affect the size of an object.

	Notes:		Depending on the user preferences, this option is either slaved off the
				"Scale Line Widths" option, or is forced on/off permanently.

				The preferences controlling this behaviour are held in the SelectorTool

********************************************************************************************/

BOOL SelectorInfoBarOp::DontConsiderAttrs(void) const
{
	// If "SlaveLineWidthToButton" preference is TRUE, then we use the Scale Line Widths
	// button state to determine the return value
	if (SelectorTool::fSlaveLineWidthToButton)
		return(!ShouldScaleLines());

	// Otherwise, we take the setting from the "ConsiderLineWidths" preference
	return(!SelectorTool::fConsiderLineWidths);
}



/********************************************************************************************
>	BOOL SelectorInfoBarOp::ShouldTransFills() const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/8/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the "Transform Fills" button is down.
	Purpose:	Checks the state of the "Scale Lines" button.  NB. FEATURE REMOVED FOR
				THIS RELEASE.
	Errors:		-
	SeeAlso:	-
********************************************************************************************/
/*
BOOL SelectorInfoBarOp::ShouldTransFills() const
{
	return fTransFills;
}
*/


/********************************************************************************************
>	void SelectorInfoBarOp::SetAspectLock(BOOL fState)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/8/94
	Inputs:		fState		the new state of the "Lock Aspect Ratio" button (TRUE = down)
	Outputs:	-
	Returns:	-
	Purpose:	Sets the state of the "Lock Aspect Ratio" button.
	Errors:		-
	SeeAlso:	-
********************************************************************************************/

void SelectorInfoBarOp::SetAspectLock(BOOL fState)
{
	if(HasWindow())
		SetLongGadgetValue(_R(IDC_SEL_PADLOCK),	(fLockAspect = fState), FALSE);
}



/********************************************************************************************
>	void SelectorInfoBarOp::SetRotateMode(BOOL fState)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/8/94
	Inputs:		fState			TRUE if the button should be down
	Outputs:	-
	Returns:	-
	Purpose:	Sets the state of the "Rotate/Bounds blobs" button.
	Errors:		-
	SeeAlso:	-
********************************************************************************************/

void SelectorInfoBarOp::SetRotateMode(BOOL fState)
{
	fRotateMode = fState;
	if (HasWindow()) SetLongGadgetValue(_R(IDC_SEL_ROTATEBUTTON), fState, FALSE);
}



/********************************************************************************************
>	void SelectorInfoBarOp::SetLeaveCopy(BOOL fState)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/8/94
	Inputs:		fState			TRUE if the button should be down
	Outputs:	-
	Returns:	-
	Purpose:	Sets the state of the "Leave Copy" button.
	Errors:		-
	SeeAlso:	-
********************************************************************************************/
/*
void SelectorInfoBarOp::SetLeaveCopy(BOOL fState)
{
	fLeaveCopy = fState;
	if (HasWindow()) SetLongGadgetValue(_R(IDC_SEL_LEAVECOPY), fLeaveCopy, FALSE);
}
*/


/********************************************************************************************
>	void SelectorInfoBarOp::SetScaleLines(BOOL fState)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/8/94
	Inputs:		fState			TRUE if the button should be down
	Outputs:	-
	Returns:	-
	Purpose:	Sets the state of the "Scale Lines" button.
	Errors:		-
	SeeAlso:	-
********************************************************************************************/

void SelectorInfoBarOp::SetScaleLines(BOOL fState)
{
	if (HasWindow())
	{
		SetLongGadgetValue(_R(IDC_SEL_SCALELINES), (fScaleLines = fState), FALSE);

		// Make sure that the selector tool is displaying the correct information for
		// the new button state (which can affect the displayed size of the selection)
		if (pSelectorTool)
			pSelectorTool->SelectionHasChanged();
	}

}



/********************************************************************************************
>	void SelectorInfoBarOp::SetTransFills(BOOL fState)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/8/94
	Inputs:		fState			TRUE if the button should be down
	Outputs:	-
	Returns:	-
	Purpose:	Sets the state of the "Transform Fills" button.  NB. FEATURE REMOVED FOR
				THIS RELEASE.
	Errors:		-
	SeeAlso:	-
********************************************************************************************/
/*
void SelectorInfoBarOp::SetTransFills(BOOL fState)
{
	SetLongGadgetValue(_R(IDC_SEL_TRANSFILL), (fTransFills = fState), FALSE);
}
*/


/********************************************************************************************
>	SelEditRecord* SelectorInfoBarOp::GetRecord(UINT32 nEditControlID)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/11/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	
	Errors:		-
	SeeAlso:	-
********************************************************************************************/

SelEditRecord* SelectorInfoBarOp::GetRecord(UINT32 nEditControlID)
{
	if (!HasWindow()) return NULL;

	// Search through the array of edit-records lloking for the given ID.
	for (SelEditRecord* per = aerEdits; per < &aerEdits[8]; per++)
	{
		if (per->nControlID == nEditControlID) return per;
	}
	
	// Couldn't find the corresponding edit-field (big problem!)
	ERROR3("Couldn't find edit-field record in SelectorInfoBarOp::GetRecord!");
	return NULL;
}



/********************************************************************************************
>	void SelectorInfoBarOp::UpdateRecord(SelEditRecord* pefr)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/11/94
	Inputs:		pefr		---			pointer to the edit-field record to be updated
	Outputs:	-
	Returns:	-
	Purpose:	Called when an edit-field is initialised or changed by the user.
	Errors:		-
	SeeAlso:	-
********************************************************************************************/

void SelectorInfoBarOp::UpdateRecord(SelEditRecord* per)
{
	if (per)
	{
		// Copy the text in the edit field.
		BOOL fOk = TRUE;
		if (HasWindow())
		{
			per->strText = GetStringGadgetValue(per->nControlID, &fOk);
			if (!fOk)
			{
				TRACEUSER( "JustinF", _T("Couldn't update edit-field %u\n"), per->nControlID);
				per->strText = TEXT("");
			}
		}
		else
		{
			per->strText = TEXT("");
		}
	}
}



/********************************************************************************************
>	void SelectorInfoBarOp::UpdateRecord(UINT32 nControlID)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/11/94
	Inputs:		nControlID		resource ID the edit-control
	Outputs:	-
	Returns:	-
	Purpose:	Updates internal records of the given edit-control
	Errors:		-
	SeeAlso:	-
********************************************************************************************/

void SelectorInfoBarOp::UpdateRecord(UINT32 nControlID)
{
	UpdateRecord(GetRecord(nControlID));
}



/********************************************************************************************
>	void SelectorInfoBarOp::UpdateAllRecords()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/11/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Updates internal records of all edit-controls.
	Errors:		-
	SeeAlso:	-
********************************************************************************************/

void SelectorInfoBarOp::UpdateAllRecords()
{
	for (SelEditRecord* per = aerEdits; per < &aerEdits[8]; per++)
	{
		UpdateRecord(per);
	}
}



/********************************************************************************************
>	BOOL SelectorInfoBarOp::HasChangedRecord(INT32 i)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/11/94
	Inputs:		i			0 - 7, identifying which edit-control to report on.
	Outputs:	-
	Returns:	TRUE if edit-control [i] has been changed recently by the user
	Purpose:	Reports on whether the given edit-control has new info in it.
	Errors:		-
	SeeAlso:	-
********************************************************************************************/

BOOL SelectorInfoBarOp::HasChangedRecord(INT32 i)
{
	// Copy the text in the edit field.
	SelEditRecord* per = &aerEdits[i];
	BOOL fOk;
	String_256 str;
	if(HasWindow())
		str = GetStringGadgetValue(per->nControlID, &fOk);
	else
		fOk = FALSE;

	if (!fOk)
	{
		TRACEUSER( "JustinF", _T("Couldn't test edit-field %u\n"), per->nControlID );
		return per->fIsDirty = FALSE;
	}

	// Compare the existing text with the old record.
	return per->fIsDirty = (str != per->strText);
}



/********************************************************************************************
>	double SelectorInfoBarOp::ConvertRecord(INT32 i)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/11/94
	Inputs:		i				identifier of an edit-control, 0-7
	Outputs:	-
	Returns:	The numeric value of the contents of the edit-control.
	Purpose:	Reads the text in the given edit-control record and converts it to a INT32.
	Errors:		-
	SeeAlso:	-
********************************************************************************************/

double SelectorInfoBarOp::ConvertRecord(INT32 i)
{
	double n;
	fConvertedOK = fConvertedOK && Convert::StringToDouble( aerEdits[i].strText, &n );
	return n;
}



/********************************************************************************************
>	MILLIPOINT SelectorInfoBarOp::ConvertMillipointRecord(INT32 i)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/11/94
	Inputs:		i			identifier of an edit-control, 0-7
	Outputs:	-
	Returns:	The contents of the given edit-control as a millipoint value.
	Purpose:	Converts an edit-control to a millipoint value, allowing for unit conversions.
	Errors:		-
	SeeAlso:	-
********************************************************************************************/

MILLIPOINT SelectorInfoBarOp::ConvertMillipointRecord(INT32 i)
{
	MILLIPOINT n;
	DimScale* pDimScale = DimScale::GetPtrDimScale(pSelectorTool->GetSelectionSpread());
	ERROR2IF (pDimScale == NULL, 1,
				"Null DimScale* in SelectorInfoBarOp::ConvertMillipointRecord");
	fConvertedOK = fConvertedOK && pDimScale->ConvertToMillipoints( aerEdits[i].strText, &n );
	return n;
}



/********************************************************************************************
>	FIXED16 SelectorInfoBarOp::ConvertFixedpointRecord(INT32 i)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/11/94
	Inputs:		i			edit-control identifier, 0-7
	Outputs:	-
	Returns:	Contents of the edit-control as a fixed-point number
	Purpose:	Converts the given edit-control to a fixed-point number
	Errors:		-
	SeeAlso:	-
********************************************************************************************/

FIXED16 SelectorInfoBarOp::ConvertFixedpointRecord(INT32 i)
{
	BOOL fOk;
	double n = Convert::StringToDouble( aerEdits[i].strText, MILLIPOINTS, &fOk );		// NB. not really
	fConvertedOK = fConvertedOK && fOk;
	return (FIXED16) n;
}



/********************************************************************************************
>	void SelectorInfoBarOp::SetEdit(CGadgetID gid, INT32 nValue, Spread* pUnitSpread,
									BOOL fUpdate = TRUE)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/8/94
	Inputs:		gid				gadget identifier
				nValue			the value to set in the edit field (INT32 integer)
				pUnitSpread		the spread containing the object (so its units can be
								extracted).  If this parameter is NULL then no unit
								conversion is performed.
				fUpdate			update the internal records of the field's contents if
								TRUE (the default).
	Outputs:	-
	Returns:	-
	Purpose:	Converts the given value to the appropriate units, as contained in the
				spread, converts it to text and writes in into the edit-field.
	Errors:		-
	SeeAlso:	SelectorInfoBarOp::SetEdit_OnDrag;
				SelectorInfoBarOp::SetEdit_OnSelectionChange
********************************************************************************************/

void SelectorInfoBarOp::SetEdit(CGadgetID gid, INT32 nValue, Spread* pUnitSpread, BOOL fUpdate)
{
	if (pUnitSpread != NULL)
	{
		// Get the dimension scaling object (units) associated with the given spread
		// and convert to its units.
		DimScale* pDimScale = DimScale::GetPtrDimScale((Node*) pUnitSpread);
		ENSURE(pDimScale != NULL, "Null DimScale* in SelectorInfoBarOp::SetEdit");
	
		// Convert to units & text and display.
		String_256 str;
		pDimScale->ConvertToUnits((INT32) nValue, &str);
		if (HasWindow()) SetStringGadgetValue(gid, &str);
	}
	else
	{
		// No unit conversion necessary, just convert to text.
		if (HasWindow()) SetLongGadgetValue(gid, nValue);
	}	  

	// Grab the text for our records, if required.
	if (fUpdate) UpdateRecord((UINT32) gid);
}



/********************************************************************************************
>	void SelectorInfoBarOp::SetDoubleEdit(CGadgetID gid, double nValue, BOOL fUpdate = TRUE)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/2/95
	Inputs:		gid				gadget identifier
				nValue			the value to set in the edit field (INT32 integer)
				fUpdate			update the internal records of the field's contents if
								TRUE (the default).
	Outputs:	-
	Returns:	-
	Purpose:	Converts the given double value to text and writes in into the edit-field.
	Errors:		-
	SeeAlso:	SelectorInfoBarOp::SetEdit;
				SelectorInfoBarOp::SetEdit_OnDrag;
				SelectorInfoBarOp::SetEdit_OnSelectionChange
********************************************************************************************/

void SelectorInfoBarOp::SetDoubleEdit(CGadgetID gid, double nValue, BOOL fUpdate)
{
	// No unit conversion necessary, just convert to text.
	if (HasWindow()) SetDoubleGadgetValue(gid, nValue);

	// Grab the text for our records, if required.
	if (fUpdate) UpdateRecord((UINT32) gid);
}



/********************************************************************************************
>	void SelectorInfoBarOp::SetEdit(CGadgetID gid, FIXED16 fxValue, Spread* pUnitSpread,
									BOOL fUpdate = TRUE)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/9/94
	Inputs:		gid				the edit-field gadget (control) identifier that is to be set
				fxValue			the fixed-point value to set in the field
				pUnitSpread		the spread that is used for unit conversions.  If null then
								no unit conversion will occur (CURRENTLY NOT SUPPORTED).
	Outputs:	-
	Returns:	-
	Purpose:	Sets an edit-field in the selector info-bar to the given fixed-point
				value.  If a spread is passed then the value will be converted to the
				appropriate units for the spread.  Currently ANGLEs are always in degrees
				so this is not yet implemented.
	Errors:		-
	SeeAlso:	-
********************************************************************************************/

void SelectorInfoBarOp::SetEdit(CGadgetID gid, FIXED16 fxValue, Spread* pUnitSpread,
								BOOL fUpdate)
{
/*	if (pUnitSpread != NULL && IsUserName("JustinF"))
	{
		TRACE( _T("Warning - non-null pUnitSpread in SelectorInfoBarOp::SetEdit\n"));
	}
*/
	String str;
	fxValue.ToString(&str);
	if (HasWindow()) SetStringGadgetValue(gid, &str, FALSE);
	if (fUpdate) UpdateRecord((UINT32) gid);
}



/********************************************************************************************
>	void SelectorInfoBarOp::ClearEdit(CGadgetID gid)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/9/94
	Inputs:		gid			the gadget ID of the edit-field to clear
	Outputs:	-
	Returns:	-
	Purpose:	Blanks the given edit-field.
	Errors:		-
	SeeAlso:	-
********************************************************************************************/

void SelectorInfoBarOp::ClearEdit(CGadgetID gid)
{
	String_256 str;
	str.Empty();
	if(HasWindow())
		SetStringGadgetValue(gid, &str, FALSE);
	UpdateRecord((UINT32) gid);
}



/********************************************************************************************
>	void SelectorInfoBarOp::EnableAllEdits(BOOL fEnabled)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/8/94
	Inputs:		fEnabled		TRUE if the gadgets should be enabled, FALSE if they
								should be disabled.
	Outputs:	-
	Returns:	-
	Purpose:	Enables or disables the edit-fields in the info-bar,
				clearing them to blank if they are being disabled.
	Errors:		-
	SeeAlso:	-
********************************************************************************************/

void SelectorInfoBarOp::EnableAllEdits(BOOL fEnabled)
{
	if (!HasWindow())
	{
		TRACEUSER( "JustinF", _T("Tried to enable edit controls but they don't exist?!?\n"));
		return;
	}

	// Clear the edit fields if being disabled.
	if (!fEnabled)
	{
		ClearEdit(_R(IDC_SEL_EDIT_X));
		ClearEdit(_R(IDC_SEL_EDIT_Y));
		ClearEdit(_R(IDC_SEL_EDIT_W));
		ClearEdit(_R(IDC_SEL_EDIT_H));
		ClearEdit(_R(IDC_SEL_EDIT_XSCALE));
		ClearEdit(_R(IDC_SEL_EDIT_YSCALE));
		ClearEdit(_R(IDC_SEL_EDIT_ANGLE));
		ClearEdit(_R(IDC_SEL_EDIT_SHEAR));
	}
	
	// Enable/disable the controls.
	EnableGadget(_R(IDC_SEL_EDIT_X), 	  fEnabled);
	EnableGadget(_R(IDC_SEL_EDIT_Y), 	  fEnabled);
	EnableGadget(_R(IDC_SEL_EDIT_W), 	  fEnabled);
	EnableGadget(_R(IDC_SEL_EDIT_H), 	  fEnabled);
	EnableGadget(_R(IDC_SEL_EDIT_XSCALE), fEnabled);
	EnableGadget(_R(IDC_SEL_EDIT_YSCALE), fEnabled);
	EnableGadget(_R(IDC_SEL_EDIT_ANGLE),  fEnabled);
	EnableGadget(_R(IDC_SEL_EDIT_SHEAR),  fEnabled);
}

/********************************************************************************************
>	void SelectorInfoBarOp::EnableGrid(BOOL fEnabled)

	Author:		Alex Bligh <alex@alex.org.uk>
	Created:	03-Feb-2005
	Inputs:		fEnabled		TRUE if the gadgets should be enabled, FALSE if they
								should be disabled.
	Outputs:	-
	Returns:	-
	Purpose:	Enables or disables the rotate grid control
	Errors:		-
	SeeAlso:	-
********************************************************************************************/

void SelectorInfoBarOp::EnableGrid(BOOL fEnabled)
{
	EnableGadget(_R(IDC_SEL_GRID_NW), fEnabled);
	EnableGadget(_R(IDC_SEL_GRID_N) , fEnabled);
	EnableGadget(_R(IDC_SEL_GRID_NE), fEnabled);
	EnableGadget(_R(IDC_SEL_GRID_W) , fEnabled);
	EnableGadget(_R(IDC_SEL_GRID_CENTRE), fEnabled);
	EnableGadget(_R(IDC_SEL_GRID_E) , fEnabled);
	EnableGadget(_R(IDC_SEL_GRID_SW), fEnabled);
	EnableGadget(_R(IDC_SEL_GRID_S) , fEnabled);
	EnableGadget(_R(IDC_SEL_GRID_SE), fEnabled);
}

/********************************************************************************************
>	void SelectorInfoBarOp::PaintAllEditsNow()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/9/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Forces an immediate repaint of any edit field that has had its contents
				changed.
	Errors:		-
	SeeAlso:	-
********************************************************************************************/

void SelectorInfoBarOp::PaintAllEditsNow()
{
	if (!HasWindow()) return;
	PaintGadgetNow(_R(IDC_SEL_EDIT_X));
	PaintGadgetNow(_R(IDC_SEL_EDIT_Y));
	PaintGadgetNow(_R(IDC_SEL_EDIT_W));
	PaintGadgetNow(_R(IDC_SEL_EDIT_H));
	PaintGadgetNow(_R(IDC_SEL_EDIT_XSCALE));
	PaintGadgetNow(_R(IDC_SEL_EDIT_YSCALE));
	PaintGadgetNow(_R(IDC_SEL_EDIT_ANGLE));
	PaintGadgetNow(_R(IDC_SEL_EDIT_SHEAR));
}



/********************************************************************************************
>	void SelectorInfoBarOp::SetEdit_OnSelectionChange()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/8/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Updates the X, Y, W, H edit fields of the info-bar with the position and
				extent of the currently selected object(s), if any.  If there is no
				selection then it clears and disables the edit-fields until there is.
				Called by the selector tool when it is notified that the selection has
				changed.

				Now also greys/ungreys info-bar buttons, as appropriate.
	Errors:		-
	SeeAlso:	-
********************************************************************************************/

void SelectorInfoBarOp::SetEdit_OnSelectionChange()
{
	if (!HasWindow())
	{
//		TRACEUSER( "JustinF", _T("Tried to set edits on selection change but they don't exist?!?\n"));
		return;
	}

	Spread* pSpread = pSelectorTool->GetSelectionSpread();
	if (pSpread != NULL)
	{
		// Get extent of new selection in user coords, and Update all the numeric gadgets.
		DocRect  DRSelExtent = pSelectorTool->GetSelectionBounds(DontConsiderAttrs());
		UserRect   SelExtent = DRSelExtent.ToUser(pSpread);
		SetEdit(_R(IDC_SEL_EDIT_X), SelExtent.lo.x, pSpread);
		SetEdit(_R(IDC_SEL_EDIT_Y), SelExtent.lo.y, pSpread);
		SetEdit(_R(IDC_SEL_EDIT_W), SelExtent.Width(), pSpread);
		SetEdit(_R(IDC_SEL_EDIT_H), SelExtent.Height(), pSpread);
		SetEdit(_R(IDC_SEL_EDIT_XSCALE), (FIXED16) 100, NULL);
		SetEdit(_R(IDC_SEL_EDIT_YSCALE), (FIXED16) 100, NULL);
		SetEdit(_R(IDC_SEL_EDIT_ANGLE), (FIXED16) 0, NULL);
		SetEdit(_R(IDC_SEL_EDIT_SHEAR), (FIXED16) 0, NULL);

		// Ungrey the buttons that are now available.
		EnableGadget(_R(IDC_SEL_BUMP_X_LESS), TRUE);
		EnableGadget(_R(IDC_SEL_BUMP_X_MORE), TRUE);
		EnableGadget(_R(IDC_SEL_BUMP_Y_LESS), TRUE);
		EnableGadget(_R(IDC_SEL_BUMP_Y_MORE), TRUE);
		EnableGadget(_R(IDC_SEL_BUMP_W_LESS), TRUE);
		EnableGadget(_R(IDC_SEL_BUMP_W_MORE), TRUE);
		EnableGadget(_R(IDC_SEL_BUMP_H_LESS), TRUE);
		EnableGadget(_R(IDC_SEL_BUMP_H_MORE), TRUE);
		EnableGadget(_R(IDC_SEL_BUMP_ANGLE_LESS), TRUE);
		EnableGadget(_R(IDC_SEL_BUMP_ANGLE_MORE), TRUE);
		EnableGadget(_R(IDC_SEL_BUMP_SHEAR_LESS), TRUE);
		EnableGadget(_R(IDC_SEL_BUMP_SHEAR_MORE), TRUE);
		EnableGadget(_R(IDC_SEL_FLIPHORZ), TRUE);
		EnableGadget(_R(IDC_SEL_FLIPVERT), TRUE);
//		EnableGadget(_R(IDC_SEL_ROTATE_GRID), TRUE);
		EnableGrid(TRUE);

		// Reenable the edit-controls.
		EnableAllEdits(TRUE);
	}
	else
	{
		// Disable all the edit-controls and blank them.
		EnableAllEdits(FALSE);

		// Grey the buttons that are now unavailable.
		EnableGadget(_R(IDC_SEL_BUMP_X_LESS), FALSE);
		EnableGadget(_R(IDC_SEL_BUMP_X_MORE), FALSE);
		EnableGadget(_R(IDC_SEL_BUMP_Y_LESS), FALSE);
		EnableGadget(_R(IDC_SEL_BUMP_Y_MORE), FALSE);
		EnableGadget(_R(IDC_SEL_BUMP_W_LESS), FALSE);
		EnableGadget(_R(IDC_SEL_BUMP_W_MORE), FALSE);
		EnableGadget(_R(IDC_SEL_BUMP_H_LESS), FALSE);
		EnableGadget(_R(IDC_SEL_BUMP_H_MORE), FALSE);
		EnableGadget(_R(IDC_SEL_BUMP_ANGLE_LESS), FALSE);
		EnableGadget(_R(IDC_SEL_BUMP_ANGLE_MORE), FALSE);
		EnableGadget(_R(IDC_SEL_BUMP_SHEAR_LESS), FALSE);
		EnableGadget(_R(IDC_SEL_BUMP_SHEAR_MORE), FALSE);
		EnableGadget(_R(IDC_SEL_FLIPHORZ), FALSE);
		EnableGadget(_R(IDC_SEL_FLIPVERT), FALSE);
//		EnableGadget(_R(IDC_SEL_ROTATE_GRID), FALSE);
		EnableGrid(FALSE);
	}
}



/********************************************************************************************
>	void SelectorInfoBarOp::SetEdit_OnDrag(TransformBoundingData* pData, Spread* pSpread)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/8/94
	Inputs:		pData		pointer to the "callback" data generated by mouse-drags,
							containing a description of the dragged object's extent etc.
	Outputs:	-
	Returns:	-
	Purpose:	Dynamically updates the text in the edit-fields as an object is being
				dragged.  Works out what needs doing from the pData argument.
	Errors:		-
	SeeAlso:	SelectorTool::DragMove
********************************************************************************************/

void SelectorInfoBarOp::SetEdit_OnDrag(TransformBoundingData* pData, Spread* pSpread)
{
	// Make sure the info-bar hasn't been hidden.
	if (!HasWindow()) return;
	
	if (pData->XYChanged)
	{
		// Convert the spread coords to user coords, and update gadgets
		UserCoord UserPos = DocCoord(pData->x, pData->y).ToUser(pSpread);
		SetEdit(_R(IDC_SEL_EDIT_X), UserPos.x, pSpread);
		SetEdit(_R(IDC_SEL_EDIT_Y), UserPos.y, pSpread);
		PaintGadgetNow(_R(IDC_SEL_EDIT_X));
		PaintGadgetNow(_R(IDC_SEL_EDIT_Y));
		pData->XYChanged = FALSE;
	}

	if (pData->WHChanged)
	{
		SetEdit(_R(IDC_SEL_EDIT_W), pData->Width, pSpread);
		SetEdit(_R(IDC_SEL_EDIT_H), pData->Height, pSpread);
		PaintGadgetNow(_R(IDC_SEL_EDIT_W));
		PaintGadgetNow(_R(IDC_SEL_EDIT_H));
		pData->WHChanged = FALSE;
	}

	if (pData->ScaleChanged)
	{
		// Take the absolute value and round to the nearest percent.
		FIXED16 xfx = (pData->XScale < 0) ? -pData->XScale : pData->XScale;
		FIXED16 yfx = (pData->YScale < 0) ? -pData->YScale : pData->YScale;
		INT32 xs = (xfx * 100 + FIXED16(0.5)).MakeLong();
		INT32 ys = (yfx * 100 + FIXED16(0.5)).MakeLong();
		SetEdit(_R(IDC_SEL_EDIT_XSCALE), xs, NULL);
		SetEdit(_R(IDC_SEL_EDIT_YSCALE), ys, NULL);
		PaintGadgetNow(_R(IDC_SEL_EDIT_XSCALE));
		PaintGadgetNow(_R(IDC_SEL_EDIT_YSCALE));
		pData->ScaleChanged = FALSE;
	}

	if (pData->RotateChanged)
	{
		SetEdit(_R(IDC_SEL_EDIT_ANGLE), pData->Rotation, pSpread);
		PaintGadgetNow(_R(IDC_SEL_EDIT_ANGLE));
		pData->RotateChanged = FALSE;
	}

	if (pData->ShearChanged)
	{
		double ShearAngle = pData->Shear.MakeDouble();

		// First we muct ensure that the displayed angle is in the -90..+90 range -
		// The angles coming in will either be centered around 0 or 180 degrees.
		// We therefore move them all to the 0 origin, so that they are displayed correctly
		if (ShearAngle >= 90.0)
			ShearAngle = -180.0 + ShearAngle;

		if (ShearAngle <= -90.0)
			ShearAngle = 180.0 + ShearAngle;

		// And ensure they lie in a sensible range
		if (ShearAngle < -89.0)
			ShearAngle = -89.0;

		if (ShearAngle > 89.0)
			ShearAngle = 89.0;

		SetEdit(_R(IDC_SEL_EDIT_SHEAR), (ANGLE) ShearAngle, pSpread);
		PaintGadgetNow(_R(IDC_SEL_EDIT_SHEAR));
		pData->ShearChanged = FALSE;
	}

	if (pData->ScaleLinesChanged)
	{
		SetScaleLines(pData->ScaleLines);
		PaintGadgetNow(_R(IDC_SEL_SCALELINES));
		pData->ScaleLinesChanged = FALSE;
	}

	// Make sure all changes are immediately visible.
//	PaintAllEditsNow();
}



/********************************************************************************************
>	void SelectorInfoBarOp::HandleCreate(DialogMsg* pDlgMsg)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/7/94
	Inputs:		A pointer to the DIM_CREATE dialog message to handle (not used)
	Purpose:	Sets the initial state of the controls in the info-bar.
	SeeAlso:	SelectorInfoBarOp::Message
********************************************************************************************/

void SelectorInfoBarOp::HandleCreate(DialogMsg* /* pDlgMsg */)
{
//	TRACEUSER( "JustinF", _T("In SelectorInfoBarOp::HandleCreate\n"));

	// Get an empty list of the blob types.
	ENSURE(pSelectorTool != NULL, "No selector tool in SelectorInfoBarOp::HandleCreate");
	BlobStyle SelectorBlobs = pSelectorTool->GetBlobStyle();
	if (HasWindow())
	{
		// Set all the blob buttons in the info bar according to the information.
		SetLongGadgetValue(_R(IDC_SEL_SHOWBOUNDSBLOBS),  pSelectorTool->fShowToolBlobs, FALSE);
		SetLongGadgetValue(_R(IDC_SEL_SHOWOBJECTBLOBS),  SelectorBlobs.Object,   		FALSE);
		SetLongGadgetValue(_R(IDC_SEL_SHOWFILLBLOBS), 	 SelectorBlobs.Fill, 	 		FALSE);
//		SetLongGadgetValue(_R(IDC_SEL_SHOWARTLINEBLOBS), SelectorBlobs.Artistic, 		FALSE);

		// Set all the option buttons.
		SetLongGadgetValue(_R(IDC_SEL_ROTATEBUTTON),	 fRotateMode,			 		FALSE);
		SetLongGadgetValue(_R(IDC_SEL_SCALELINES),		 fScaleLines, 			 		FALSE);
		SetLongGadgetValue(_R(IDC_SEL_PADLOCK),		 	 fLockAspect,			 		FALSE);
//		SetLongGadgetValue(_R(IDC_SEL_LEAVECOPY),		 fLeaveCopy,			 		FALSE);
//		SetLongGadgetValue(_R(IDC_SEL_TRANSFILL),		 fTransFills,			 		FALSE);
	}
	
	// Initialise the edit-field records.
	aerEdits[0].nControlID = _R(IDC_SEL_EDIT_X);
	aerEdits[1].nControlID = _R(IDC_SEL_EDIT_Y);
	aerEdits[2].nControlID = _R(IDC_SEL_EDIT_W);
	aerEdits[3].nControlID = _R(IDC_SEL_EDIT_H);
	aerEdits[4].nControlID = _R(IDC_SEL_EDIT_XSCALE);
	aerEdits[5].nControlID = _R(IDC_SEL_EDIT_YSCALE);
	aerEdits[6].nControlID = _R(IDC_SEL_EDIT_ANGLE);
	aerEdits[7].nControlID = _R(IDC_SEL_EDIT_SHEAR);

	// Now set the edit controls.
	SetEdit_OnSelectionChange();

	// Make sure their current values are recorded.
	UpdateAllRecords();
}    



/********************************************************************************************
>	BOOL SelectorInfoBarOp::HandleButtonDown(DialogMsg* pDlgMsg)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com> / Rik
	Created:	12/7/94
	Inputs:		A pointer to a "dialogue box" message about a button click.
	Returns:	TRUE if message handled and should be passd to the InformationBarOp,
				FALSE if not handled and should be passed toDialogBarOp (which doesn't
				eat messages and can properly invoked operations).
	Purpose:	For the given button calls the appropriate functions that update the
				blob manager.
	SeeAlso:	SelectorInfoBarOp::Message; SelectorInfoBarOp::UpdateBlobManager
********************************************************************************************/

BOOL SelectorInfoBarOp::HandleButtonDown(DialogMsg* pDlgMsg)
{
	// Get ourselves an empty blob style (ie all entries set to FALSE)
	BlobStyle ChangingBlob;

	// Have a look at which button was pressed and deal with it
	// switch (pDlgMsg->GadgetID)
	
	// "Show Bounds Blobs"
	if (pDlgMsg->GadgetID == _R(IDC_SEL_SHOWBOUNDSBLOBS))
	{
		pSelectorTool->BoundsButtonChange();
	}
	// "Show Objects".
	else if (pDlgMsg->GadgetID == _R(IDC_SEL_SHOWOBJECTBLOBS))
	{	// We want to toggle the state of the Object blobs
		ChangingBlob.Object = TRUE;
	}
	// "Show Fills".
	else if (pDlgMsg->GadgetID == _R(IDC_SEL_SHOWFILLBLOBS))
	{	// We want to toggle the state of the Object blobs
		ChangingBlob.Fill = TRUE;
	}
	else if (pDlgMsg->GadgetID == _R(IDC_SEL_PADLOCK))
	{	// Toggle the state of the padlock button.
		fLockAspect = !fLockAspect;
	}
	// "Rotate mode".  Will immediately notify the tool, so it can display new blobs.
	else if (pDlgMsg->GadgetID == _R(IDC_SEL_ROTATEBUTTON))
	{
		pSelectorTool->RotateButtonChange(fRotateMode = !fRotateMode);
	}
	// "Scale lines when scaling an object".
	else if (pDlgMsg->GadgetID == _R(IDC_SEL_SCALELINES))
	{
		fScaleLines = !fScaleLines;

		// Because the "consider attributes" state (which affects the displayed width/height
		// of the selection) can be slaved off this button, we must also ensure that the
		// selector tool has cached and is displaying correct info.
		if (pSelectorTool)
			pSelectorTool->SelectionHasChanged();
	}
	// "Flip horizontally".
	else if (pDlgMsg->GadgetID == _R(IDC_SEL_FLIPHORZ))
	{
		pSelectorTool->FlipButtonChange(FALSE);
	}
	// "Flip vertically".
	else if (pDlgMsg->GadgetID == _R(IDC_SEL_FLIPVERT))
	{
		pSelectorTool->FlipButtonChange(TRUE);
	}
	// "Decrease X a bit".
	else if (pDlgMsg->GadgetID == _R(IDC_SEL_BUMP_X_LESS))
	{
		HandleBumpClickLinear(_R(IDC_SEL_EDIT_X), -1);
	}
	// "Increase X a bit".
	else if (pDlgMsg->GadgetID == _R(IDC_SEL_BUMP_X_MORE))
	{
		HandleBumpClickLinear(_R(IDC_SEL_EDIT_X), 1);
	}
	// "Decrease Y a bit".
	else if (pDlgMsg->GadgetID == _R(IDC_SEL_BUMP_Y_LESS))
	{
		HandleBumpClickLinear(_R(IDC_SEL_EDIT_Y), -1);
	}
	// "Increase Y a bit".
	else if (pDlgMsg->GadgetID == _R(IDC_SEL_BUMP_Y_MORE))
	{
		HandleBumpClickLinear(_R(IDC_SEL_EDIT_Y), 1);
	}
	// "Decrease W a bit".
	else if (pDlgMsg->GadgetID == _R(IDC_SEL_BUMP_W_LESS))
	{
		HandleBumpClickLinear(_R(IDC_SEL_EDIT_W), -1);
	}
	// "Increase W a bit".
	else if (pDlgMsg->GadgetID == _R(IDC_SEL_BUMP_W_MORE))
	{
		HandleBumpClickLinear(_R(IDC_SEL_EDIT_W), 1);
	}
	// "Decrease H a bit".
	else if (pDlgMsg->GadgetID == _R(IDC_SEL_BUMP_H_LESS))
	{
		HandleBumpClickLinear(_R(IDC_SEL_EDIT_H), -1);
	}
	// "Increase H a bit".
	else if (pDlgMsg->GadgetID == _R(IDC_SEL_BUMP_H_MORE))
	{
		HandleBumpClickLinear(_R(IDC_SEL_EDIT_H), 1);
	}
	// "Decrease angle a bit".
	else if (pDlgMsg->GadgetID == _R(IDC_SEL_BUMP_ANGLE_LESS))
	{
		HandleBumpClickAngle(_R(IDC_SEL_EDIT_ANGLE), -1, -360.0, +360.0);
	}
	// "Increase angle a bit".
	else if (pDlgMsg->GadgetID == _R(IDC_SEL_BUMP_ANGLE_MORE))
	{
		HandleBumpClickAngle(_R(IDC_SEL_EDIT_ANGLE), 1, -360.0, +360.0);
	}
	// "Decrease shear-angle a bit".
	else if (pDlgMsg->GadgetID == _R(IDC_SEL_BUMP_SHEAR_LESS))
	{
		HandleBumpClickAngle(_R(IDC_SEL_EDIT_SHEAR), -1, -89.0, +89.0);
	}
	// "Increase shear-angle a bit".
	else if (pDlgMsg->GadgetID == _R(IDC_SEL_BUMP_SHEAR_MORE))
	{		
		HandleBumpClickAngle(_R(IDC_SEL_EDIT_SHEAR), 1, -89.0, +89.0);
	}
	else if (pDlgMsg->GadgetID == _R(IDC_SEL_GRID_NW))
	{		
		pSelectorTool->SetRotateCentre(1);
	}
	else if (pDlgMsg->GadgetID == _R(IDC_SEL_GRID_N))
	{		
		pSelectorTool->SetRotateCentre(2);
	}
	else if (pDlgMsg->GadgetID == _R(IDC_SEL_GRID_NE))
	{		
		pSelectorTool->SetRotateCentre(3);
	}
	else if (pDlgMsg->GadgetID == _R(IDC_SEL_GRID_W))
	{		
		pSelectorTool->SetRotateCentre(4);
	}
	else if (pDlgMsg->GadgetID == _R(IDC_SEL_GRID_CENTRE))
	{		
		pSelectorTool->SetRotateCentre(0);
	}
	else if (pDlgMsg->GadgetID == _R(IDC_SEL_GRID_E))
	{		
		pSelectorTool->SetRotateCentre(5);
	}
	else if (pDlgMsg->GadgetID == _R(IDC_SEL_GRID_SW))
	{		
		pSelectorTool->SetRotateCentre(6);
	}
	else if (pDlgMsg->GadgetID == _R(IDC_SEL_GRID_S))
	{		
		pSelectorTool->SetRotateCentre(7);
	}
	else if (pDlgMsg->GadgetID == _R(IDC_SEL_GRID_SE))
	{		
		pSelectorTool->SetRotateCentre(8);
	}
	// Other gadgets, pass to _DialogBarOp_ base class which can properly invoke
	// ops attached to gadgets, unlike InformationBarOp which just eats everything.
	else
	{
	//	TRACEUSER( "JustinF", _T("SelectorInfoBarOp::HandleButtonDown: passing to DialogBarOp\n"));
		return FALSE;
	}

	// Ask the tool to deal with the blob manager as well as its tool blobs if needed.
	if (pSelectorTool != NULL) pSelectorTool->SelectionBlobChange(ChangingBlob);
	return TRUE;	// handled

}



/********************************************************************************************
>	void SelectorInfoBarOp::HandleButtonUp(DialogMsg* pDlgMsg)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/12/94
	Inputs:		pDlgMsg				pointer to a dialogue message from a button.
	Outputs:	-
	Returns:	-
	Purpose:	For "nudge-buttons", responds to them being released by calling the
				HandleEditCommit function, ie. acting as if the user hit the ENTER key
				within an edit-field meaning that the program should act on any changes
				made.  For all other buttons, does nothing.
	Errors:		-
	SeeAlso:	-
********************************************************************************************/

void SelectorInfoBarOp::HandleButtonUp(DialogMsg* pDlgMsg)
{
	if (
		(pDlgMsg->GadgetID == _R(IDC_SEL_BUMP_X_LESS)) ||
		(pDlgMsg->GadgetID == _R(IDC_SEL_BUMP_X_MORE)) ||
		(pDlgMsg->GadgetID == _R(IDC_SEL_BUMP_Y_LESS)) ||
		(pDlgMsg->GadgetID == _R(IDC_SEL_BUMP_Y_MORE)) ||
		(pDlgMsg->GadgetID == _R(IDC_SEL_BUMP_W_LESS)) ||
		(pDlgMsg->GadgetID == _R(IDC_SEL_BUMP_W_MORE)) ||
		(pDlgMsg->GadgetID == _R(IDC_SEL_BUMP_H_LESS)) ||
		(pDlgMsg->GadgetID == _R(IDC_SEL_BUMP_H_MORE)) ||
		(pDlgMsg->GadgetID == _R(IDC_SEL_BUMP_ANGLE_LESS)) ||
		(pDlgMsg->GadgetID == _R(IDC_SEL_BUMP_ANGLE_MORE)) ||
		(pDlgMsg->GadgetID == _R(IDC_SEL_BUMP_SHEAR_LESS)) ||
		(pDlgMsg->GadgetID == _R(IDC_SEL_BUMP_SHEAR_MORE))
		)
	{
		HandleEditCommit(NULL);
	}
	else if (
		(pDlgMsg->GadgetID == _R(IDC_SEL_GRID_NW)) ||
		(pDlgMsg->GadgetID == _R(IDC_SEL_GRID_N) ) ||
		(pDlgMsg->GadgetID == _R(IDC_SEL_GRID_NE)) ||
		(pDlgMsg->GadgetID == _R(IDC_SEL_GRID_W) ) ||
		(pDlgMsg->GadgetID == _R(IDC_SEL_GRID_CENTRE)) ||
		(pDlgMsg->GadgetID == _R(IDC_SEL_GRID_E)) ||
		(pDlgMsg->GadgetID == _R(IDC_SEL_GRID_SW)) ||
		(pDlgMsg->GadgetID == _R(IDC_SEL_GRID_S) ) ||
		(pDlgMsg->GadgetID == _R(IDC_SEL_GRID_SE))
		)
	{
		CGadgetID clicked = pDlgMsg->GadgetID;
		clicked = (CGadgetID)0; // Right now remove ALL selected ones to remove toggle functionality. 
		if (clicked != _R(IDC_SEL_GRID_NW) ) SetBoolGadgetSelected(_R(IDC_SEL_GRID_NW), FALSE);
		if (clicked != _R(IDC_SEL_GRID_N)  ) SetBoolGadgetSelected(_R(IDC_SEL_GRID_N),  FALSE);
		if (clicked != _R(IDC_SEL_GRID_NE) ) SetBoolGadgetSelected(_R(IDC_SEL_GRID_NE), FALSE);
		if (clicked != _R(IDC_SEL_GRID_W)  ) SetBoolGadgetSelected(_R(IDC_SEL_GRID_W),  FALSE);
		if (clicked != _R(IDC_SEL_GRID_CENTRE) ) SetBoolGadgetSelected(_R(IDC_SEL_GRID_CENTRE), FALSE);
		if (clicked != _R(IDC_SEL_GRID_E)  ) SetBoolGadgetSelected(_R(IDC_SEL_GRID_E) , FALSE);
		if (clicked != _R(IDC_SEL_GRID_SW) ) SetBoolGadgetSelected(_R(IDC_SEL_GRID_SW), FALSE);
		if (clicked != _R(IDC_SEL_GRID_S)  ) SetBoolGadgetSelected(_R(IDC_SEL_GRID_S) , FALSE);
		if (clicked != _R(IDC_SEL_GRID_SE) ) SetBoolGadgetSelected(_R(IDC_SEL_GRID_SE), FALSE);
	}

	return;
}




/********************************************************************************************
>	void SelectorInfoBarOp::HandleBumpClickLinear(UINT32 nEditControlID, INT32 nSign)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/11/94
	Inputs:		nEditControlID	---		the identifier of the (EDIT) control associated
										with the clicked bump button
				nSign			---		either -1 or 1, depending on which of the bumps
										was clicked (eg. less, more)
	Outputs:	-
	Returns:	-
	Purpose:	Responds to a click on a bump button ("nudge") for a "linear" control, ie.
				an X, Y, width or height field, not an angle. The value will be nudged in
				the direction indicated by nSign, by the (user preference) nudge distance.
	Errors:		-
	SeeAlso:	SelectorInfoBarOp::HandleBumpClickAngle
********************************************************************************************/

void SelectorInfoBarOp::HandleBumpClickLinear(UINT32 nEditControlID, INT32 nSign)
{
	// If there's no selection then ignore the clicks.
	Spread* pUnitSpread = pSelectorTool->GetSelectionSpread();
	if (pUnitSpread == NULL) return;

	// Get the dimension units object for the spread containing the selection.
    DimScale* pDimScale = DimScale::GetPtrDimScale((Node*) pUnitSpread);
	ERROR3IF(pDimScale == NULL, "Null DimScale* in SelectorInfoBarOp::HandleBumpClick");

	// Read the edit field and convert to millipoints.
	BOOL fOk;
	INT32 nMilli;
	String_256 str = GetStringGadgetValue(nEditControlID, &fOk);
	if (!fOk || !pDimScale->ConvertToMillipoints( str, &nMilli ))
	{
		TRACEUSER( "JustinF", _T("Couldn't convert edit control\n") );
		return;
	}

	// Write the new value to the edit-field.
	SetEdit(nEditControlID, nMilli + (nSign * OpNudge::GetNudgeStep()), pUnitSpread, FALSE);

	// Set the input focus to the associated edit-field.  NB. we don't do this anymore
	// because Charles doesn't like it.
//	SetKeyboardFocus(nEditControlID);
//	HighlightText(nEditControlID);
}



/********************************************************************************************
>	void SelectorInfoBarOp::HandleBumpClickAngle(UINT32 nEditControlID, INT32 nSign,
													double Min, double Max)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/11/94
	Inputs:		nEditControlID		identifier of the edit-control
				nSign				whether to increase (+1) or decrease (-1) the field
				Min					The minimum allowed value in this field (inclusive)
				Max					The maximum allowed angle in this field (inclusive)
	Outputs:	-
	Returns:	-
	Purpose:	Responds to a click on a bump button for angles, ie unitless numeric data.

	Notes:		The value in the icon is incremented by the value in nSign. If this value
				is less than the minimum or greater than the maximum, the value will be
				clipped to the appropriate limit.

	Errors:		-
	SeeAlso:	-
********************************************************************************************/

void SelectorInfoBarOp::HandleBumpClickAngle(UINT32 nEditControlID, INT32 nSign, double Min, double Max)
{
	// If there's nothing selected then ignore the clicks.
	if (pSelectorTool->GetSelectionSpread() == NULL) return;

	// Try to read the edit-field.
	BOOL fOk;
	String_256 str = GetStringGadgetValue(nEditControlID, &fOk);
	if (!fOk)
	{
		TRACEUSER( "JustinF", _T("Couldn't read edit control\n") );
		return;
	}

	// Convert from text to a number.
	double nAngle = Convert::StringToDouble( str, MILLIPOINTS, &fOk);
	if (!fOk)
	{
		TRACEUSER( "JustinF", _T("Couldn't convert edit control\n") );
		return;
	}

	// Increment as required
	nAngle += nSign;

	// And clip to allowable range
	if (nAngle < Min)
		nAngle = Min;

	if (nAngle > Max)
		nAngle = Max;

	// Adjust it by nSign and write the new value to the edit-field.
	SetEdit(nEditControlID, (ANGLE) nAngle, NULL, FALSE);

	// Set the input focus to the associated edit-field.  NB. we don't do this anymore
	// because Charles doesn't like it.
//	SetKeyboardFocus(nEditControlID);
//	HighlightText(nEditControlID);
}



/********************************************************************************************
>	void SelectorInfoBarOp::HandleEditCommit(DialogMsg* pDlgMsg)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/8/94
	Inputs:		pDlgMsg			the "commit" dialogue message sent when the user hits ENTER
								within an edit-field.
	Outputs:	-
	Returns:	-
	Purpose:	Reads the text from the edit-fields, converts it to millipoints, informs the
				selector tool that the user has changed the selection's position and/or
				extent.
	Errors:		-
	SeeAlso:	-
********************************************************************************************/

void SelectorInfoBarOp::HandleEditCommit(DialogMsg* pDlgMsg)
{
	// If there's nothing selected then ignore the clicks.
	if (pSelectorTool->GetSelectionSpread() == NULL) return;

	// Ignore all but "hard-commits" in proper dialogue messages (not "faked" ones).
	if (pDlgMsg != NULL && ((SelChangeMsgSubType) pDlgMsg->DlgMsgParam) != ENTER_COMMIT)
		return;


	// Tell the selector tool to ignore selection change messages until further notice.
	pSelectorTool->fIgnoreSelChange = TRUE;

	// Compare the current contents of the edit-fields against their recorded values, to
	// see if any have been modified.
	BOOL fMustTransform = FALSE;
	for (INT32 i = 0; i < 8; i++)
	{
		if (HasChangedRecord(i))
		{
			UpdateRecord(&aerEdits[i]);
			fMustTransform = TRUE;
		}
	}

	// After checking and updating our records, do we have to run a transform?  Note that
	// we check these as they appear on the bar from left to right.
	UINT32 nErrorMsg=0;
	fConvertedOK = TRUE;

	if (fMustTransform)
	{

		TRACEUSER( "Matt", _T("HandleEditCommit()\n"));
		Range* Selection = GetApplication()->FindSelection();
		RangeControl TransFlags = Selection->GetRangeControlFlags();
		TransFlags.IgnoreNoneRenderable=TRUE;
		TransFlags.IgnoreInvisibleLayers = TRUE;
		Selection->SetRangeControl(TransFlags);
		SliceHelper::ModifySelectionToContainWholeButtonElements();


		if (aerEdits[0].fIsDirty || aerEdits[1].fIsDirty)
		{
			// X or Y has been changed - run a translate.
			INT32 x = ConvertMillipointRecord(0);
			INT32 y = ConvertMillipointRecord(1);

			// Convert the entered pages coordinates to spread-relative coordinates.
			Spread* pSpread = pSelectorTool->GetSelectionSpread();
			ERROR3IF(pSpread == NULL,"No selected Spread* in SelectorInfoBarOp::HandleEditCommit");
			DocCoord dc = UserCoord(x,y).ToSpread(pSpread);

			if (!fConvertedOK)
			{
				nErrorMsg = _R(IDS_SEL_ERROR_XY);
				goto AfterXform;
			}

			pSelectorTool->DoTranslateImmediate(dc.x, dc.y);
		}

		if (aerEdits[2].fIsDirty || aerEdits[3].fIsDirty)
		{
			// W or H has been changed - run a scale.
			INT32 w = ConvertMillipointRecord(2);
			INT32 h = ConvertMillipointRecord(3);
			if (!fConvertedOK || w == 0 || h == 0)
			{
				fConvertedOK = FALSE;
				nErrorMsg = _R(IDS_SEL_ERROR_WH);
				goto AfterXform;
			}
			
			if (fLockAspect)
			{
				// The aspect ratio is locked, which means we must scale either the width or
				// height accordingly, whichever was not changed.  If both were changed then
				// we have a little problem.
				DocRect oldrect = pSelectorTool->GetSelectionBounds(DontConsiderAttrs());
				if (!aerEdits[2].fIsDirty)
				{
					w = MulDiv32By32(oldrect.Width(), h, oldrect.Height());
				}
				else
				{
					h = MulDiv32By32(w, oldrect.Height(), oldrect.Width());
				}
			}

			/// Maybe fix innacurate scaling [Harrison Ainsworth 7/11/97]
//			if( !DontConsiderAttrs()  &&  SelectorTool::fUseScalingFix )
//				pSelectorTool->DoScaleImmediate(w, h, TRUE );   // special fix
//			else
				pSelectorTool->DoScaleImmediate(w, h);          // normal
		}

		if (aerEdits[4].fIsDirty || aerEdits[5].fIsDirty)
		{
			// X-scale or Y-scale has been changed - run a scale.
			double w = ConvertRecord(4);
			double h = ConvertRecord(5);

			// Ensure that the entered value is valid. Note that values near to zero are
			// actually illegal (they'd cause a divide by zero), but these are properly handled
			// by DoScalePercentImmediate, below.
			if (!fConvertedOK)
			{
				fConvertedOK = FALSE;
				nErrorMsg = _R(IDS_SEL_ERROR_XYSCALE);
				goto AfterXform;
			}

			// Quietly limit the maximum scaling to 200 times (20,000%). This should allow more
			// than enough scaling, especially while our zoom limits are 26000%
			if (w > 19999.0)	w = 19999.0;
			if (w < -19999.0)	w = -19999.0;
			if (h > 19999.0)	h = 19999.0;
			if (h < -19999.0)	h = -19999.0;

			// Check for the "Lock Aspect" button.
			if (fLockAspect)
			{
				// If only one edit has been changed then set the other to be equal.
				if (!aerEdits[4].fIsDirty)
				{
					w = h;
					SetDoubleEdit(_R(IDC_SEL_EDIT_XSCALE), w, FALSE);
				}
				else
				{
					// One or both have been changed but "lock aspect" is set, which presents a
					// problem.  For now take the x-scale as overriding.
					h = w;
					SetDoubleEdit(_R(IDC_SEL_EDIT_YSCALE), h, FALSE);
				}
			}

			pSelectorTool->DoScalePercentImmediate(w, h);
		}

		if (aerEdits[6].fIsDirty)
		{
			// Angle has been changed - run a rotate.
			double Angle = ConvertRecord(6);
			if (!fConvertedOK)
			{
				nErrorMsg = _R(IDS_SEL_ERROR_ANGLE);
				goto AfterXform;
			}

			// Ensure the angle is between -360 and +360. We use an integer modulus to get
			// the angle back under control.
			if (fabs(Angle) >= 360.0)
			{
				// The angle is out of range, so we need to make it sensible
				BOOL Negative = (Angle < 0.0);

				Angle = ((INT32)floor(fabs(Angle))) % 360;

				if (Negative)
					Angle = -Angle;
			}

			pSelectorTool->DoRotateImmediate((FIXED16) Angle);
		}

		if (aerEdits[7].fIsDirty)
		{
			// Shear-angle has been changed - run a shear.
			double Angle = ConvertRecord(7);
			if (!fConvertedOK)
			{
				nErrorMsg = _R(IDS_SEL_ERROR_SHEAR);
				goto AfterXform;
			}

			// Clip the angle to lie between +/- 89.0 degrees
			if (fabs(Angle) > 89.0)
				Angle = (Angle < 0.0) ? -89.0 : 89.0;

			pSelectorTool->DoShearImmediate((FIXED16) Angle);
		}

		SliceHelper::RestoreSelection();
	}

AfterXform:
	// Check if we got here because an edit conversion failed.
	if (!fConvertedOK) ReportEditError(nErrorMsg);

	// Reset the selection-change handler flag.  If we ran a transform then manually
	// update the selection.
	pSelectorTool->fIgnoreSelChange = FALSE;

	if (fMustTransform && fConvertedOK)
	{
		pSelectorTool->SelectionHasChanged();
	}
}




/********************************************************************************************
>	void SelectorInfoBarOp::ReportEditError(UINT32 nErrorMsgID)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/2/95
	Inputs:		nErrorMsgID			the string resource ID of the error message to
									display in the error dialogue.
	Outputs:	-
	Returns:	-
	Purpose:	Reports errors the user makes when typing in the selector tool-bar
				edit fields.
	Errors:		-
	SeeAlso:	SelectorInfoBarOp::HandleEditCommit
********************************************************************************************/

void SelectorInfoBarOp::ReportEditError(UINT32 nErrorMsgID)
{
	// Report the error with a message box.
	ToolInformError(TOOLID_SELECTOR, nErrorMsgID);

	// Reset the edit fields to correct values.
	SetEdit_OnSelectionChange();
}



/********************************************************************************************
>	void SelectorInfoBarOp::HandleGridButton(DialogMsg* pDlgMsg)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/11/94
	Inputs:		pDlgMsg			pointer to a dialogue message.
	Outputs:	-
	Returns:	-
	Purpose:	Responds to a click on the "telephone keypad", passing the event on to
				the selector tool.
	Errors:		-
	SeeAlso:	SelectorTool::SetRotateCentre
********************************************************************************************/

void SelectorInfoBarOp::HandleGridButton(DialogMsg* pDlgMsg)
{
	pSelectorTool->SetRotateCentre((INT32) pDlgMsg->DlgMsgParam);
}
 


/********************************************************************************************
>	virtual DialogBarOp* SelectorInfoBarOpCreate::Create()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/7/94
	Inputs:		-
	Outputs:	-
	Returns:	A pointer to a SelectorBarOp object allocated on the heap.
	Purpose:	Used by the bar code to create an appropriate InformationBarOp.
	Errors:		-
	SeeAlso:	class SelectorInfoBarOp; class BarCreate
********************************************************************************************/

DialogBarOp* SelectorInfoBarOpCreate::Create()
{
	return new SelectorInfoBarOp;
}
