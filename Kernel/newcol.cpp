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
// newcol.cpp - The NewColourDlg class

#include "camtypes.h"

#include "coldrop.h"
#include "collist.h"
#include "colpick.h"
#include "colourix.h"
//#include "jason.h"
#include "keypress.h"
#include "newcol.h"
//#include "nwcolres.h"
//#include "resource.h"	// For _R(IDS_CANCEL)
//#include "justin2.h"

#include "ccdc.h"		// For render-into-dialogue support
#include "fillval.h"
#include "grnddib.h"


CC_IMPLEMENT_DYNCREATE(NewColourDlg, DialogOp)
CC_IMPLEMENT_MEMDUMP(NewColourParam, OpParam)

#define new CAM_DEBUG_NEW


const INT32 NewColourDlg::IDD = _R(IDD_NEWCOLOURDLG);
const CDlgMode NewColourDlg::Mode = MODAL;

BOOL NewColourDlg::MakeShade = FALSE;


/********************************************************************************************

>	NewColourDlg::NewColourDlg(): DialogOp(NewColourDlg::IDD, NewColourDlg::Mode) 


	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/9/95

	Purpose:	NewColourDlg constructor.

********************************************************************************************/

NewColourDlg::NewColourDlg() : DialogOp(NewColourDlg::IDD, NewColourDlg::Mode) 
{
	ParentDropDown = NULL;
}        



/********************************************************************************************

>	NewColourDlg::~NewColourDlg()


	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/9/95

	Purpose:	You'll never guess...

********************************************************************************************/

NewColourDlg::~NewColourDlg()
{
	if (ParentDropDown != NULL)
		delete ParentDropDown;
}        



/********************************************************************************************

>	MsgResult NewColourDlg::Message( CDlgMessage DlgMsg, CGadgetID Gadget)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/9/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Handles all the scale dialog's messages 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

MsgResult NewColourDlg::Message(Msg* Message)
{
	if (IS_OUR_DIALOG_MSG(Message))
	{
		DialogMsg* Msg = (DialogMsg*)Message;
		BOOL EndDialog = FALSE;

		switch (Msg->DlgMsg)
		{
			case DIM_CREATE:
				{
					// Tick the "make shade" button if it was on the last time
					SetBoolGadgetSelected(_R(IDC_NEWCOL_SHADE), MakeShade);

					// Set up the colour drop-down menu
					BOOL ok = FALSE;
					ParentDropDown = new ColourDropDown;
					if (ParentDropDown != NULL && ParentDropDown->Init(WindowID, _R(IDC_NEWCOL_COLLIST)))
					{
						IndexedColour *ParentColour = Info->TheColour->FindLastLinkedParent();
						
						// Make sure the parent colour we're setting is legal (not deleted)
						if (ParentColour != NULL && ParentColour->IsDeleted())
							ParentColour = NULL;

						ok = ParentDropDown->FillInColourList(ParentColour, -1, Info->TheColour);
					}

					// Shade controls appropriately, and fill in the name text field
					ShadeControls();
					SetNameText();
					SetKeyboardFocus(_R(IDC_NEWCOL_NAME));
					HighlightText(_R(IDC_NEWCOL_NAME));

					// If the colour deropdown creation failed, then shade it to be safe
					if (!ok)
						EnableGadget(_R(IDC_NEWCOL_COLLIST), FALSE);
				}
				break;

				
			case DIM_COMMIT:
			case DIM_SOFT_COMMIT:
				CommitDetails();
				break;


			case DIM_CANCEL:
				Close();
				End();
				break;


			case DIM_LFT_BN_CLICKED:
				switch(Msg->GadgetID)
				{
					case _R(IDC_NEWCOL_SHADE):
						SetNameText();			// Ensure name text is updated if necessary
						ShadeControls();		// And make sure shading of controls is updated
						break;
				}
				break;


			case DIM_SELECTION_CHANGED:
				if (Msg->GadgetID == _R(IDC_NEWCOL_COLLIST) && ParentDropDown != NULL)
				{
					INT32 Index = GetSelectedValueIndex(_R(IDC_NEWCOL_COLLIST));

					IndexedColour *NewParent = ParentDropDown->DecodeSelection(Index);
					Info->TheColour->SetLinkedParent(NewParent, Info->TheColour->GetType());

					ShadeControls();
					SetNameText();
				}
				break;
/*
			case DIM_REDRAW:
				if (Msg->GadgetID == _R(IDC_COLNAME_PATCH))
				{
					ReDrawInfoType *RedrawInfo = (ReDrawInfoType*) Msg->DlgMsgParam;
					DocRect VirtualSize(0, 0, RedrawInfo->dx, RedrawInfo->dy);
					RenderRegion *pRender = CreateOSRenderRegion(&VirtualSize, RedrawInfo);
					if (pRender)
					{
						pRender->SaveContext();

						DocColour PatchCol(128L, 128L, 128L);
						if (Info->TheColour != NULL)
							PatchCol.MakeRefToIndexedColour(Info->TheColour);

						pRender->SetLineWidth(0);
						pRender->SetLineColour(PatchCol);
						pRender->SetFillColour(PatchCol);
						pRender->DrawRect(&VirtualSize);

						pRender->RestoreContext();

						DestroyOSRenderRegion(pRender);
					}
				}
				break;
*/
		}

		return (DLG_EAT_IF_HUNGRY(Msg)); 
	}

	return OK; 
}  



/********************************************************************************************

>	void NewColourDlg::ShadeControls(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/9/95

	Purpose:	Shades all controls appropriately to the current selection

********************************************************************************************/

void NewColourDlg::ShadeControls(void)
{
	// If there are no valid parent colours in the document, shade the "make shade" controls
	if (Info->TheList->GetUndeletedHead() == NULL)
	{
		EnableGadget(_R(IDC_NEWCOL_COLLIST), FALSE);
		EnableGadget(_R(IDC_NEWCOL_SHADE), FALSE);
	}
	else
	{
		// Parent colour for local colours is only available if you turn on "make shade"
		// but it is always available (copy-of or shade-of) for named colours
		if (Info->TheColour->IsNamed())
			EnableGadget(_R(IDC_NEWCOL_COLLIST), TRUE);
		else
			EnableGadget(_R(IDC_NEWCOL_COLLIST), GetBoolGadgetSelected(_R(IDC_NEWCOL_SHADE)));
	}
}



/********************************************************************************************

>	void NewColourDlg::SetNameText(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/9/95

	Purpose:	Sets the name field appropriately to the current selection
				If the user has changed the name field, it will not be touched

********************************************************************************************/

void NewColourDlg::SetNameText(void)
{
	String_256 CurrentText;

	CurrentText = GetStringGadgetValue(_R(IDC_NEWCOL_NAME));
	
	// Only change the text if it is blank or if the user has not changed it
	if (CurrentText.IsEmpty() || CurrentText == AutoName)
	{
		IndexedColour *ParentColour = Info->TheColour->FindLastLinkedParent();

		// Make sure the parent colour we're setting is legal (not deleted)
		if (ParentColour != NULL && ParentColour->IsDeleted())
			ParentColour = NULL;

		if (Info->TheColour->IsNamed() && ParentColour != NULL)
		{
			String_64 *ParentName = ParentColour->GetName();

			if (GetBoolGadgetSelected(_R(IDC_NEWCOL_SHADE)))
				AutoName.MakeMsg(_R(IDS_SHADEOFCOLOUR), (TCHAR *) *ParentName);					// "Shade of red"
			else
				AutoName.MakeMsg(_R(IDS_COPYOFCOLOUR), (TCHAR *) *ParentName);					// "Copy of red"
		}
		else
		{
			if (GetBoolGadgetSelected(_R(IDC_NEWCOL_SHADE)) && ParentColour != NULL)
				AutoName.MakeMsg(_R(IDS_SHADEOFCOLOUR), (TCHAR *) *(ParentColour->GetName()));	// "Shade of red"
			else
				AutoName.MakeMsg(_R(IDS_NEWCOLOUR));											// "New Colour"
		}

		SetStringGadgetValue(_R(IDC_NEWCOL_NAME), (StringBase *) &AutoName);
	}
}



/********************************************************************************************

>	void NewColourDlg::CommitDetails(BOOL Force = FALSE, BOOL ForceMakeShade = FALSE)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/9/95

	Inputs:		Force - TRUE to override the settings in the window (the window need not
				actually exist if this is TRUE)
				ForceMakeShade - ignored if Force is FALSE, else
								 TRUE to make a Linked "shade", FALSE to make a copy of the parent

	Purpose:	Commits to the current setting for the colour
				Reads the settings the user has chosen and changes the result colour
				to suit those settings. Sets the Result to TRUE if it succeeds.
				Then closes and ends the dialogue.

	Notes:		If the result is placed into TheColour, then a ColourChangingMsg broadcast
				will be sent. Thus, if a caller wants only to rename a colour, they need
				only call up the dialogue upon that colour - set and forget.

********************************************************************************************/

void NewColourDlg::CommitDetails(BOOL Force, BOOL ForceMakeShade)
{
	// Set result to 'success'
	Info->Result = TRUE;

	// Set the colour type etc as appropriate
	IndexedColour *ParentColour = Info->TheColour->FindLastLinkedParent();

	// Make sure the parent colour we're setting is legal (not deleted or in another doc)
	if (ParentColour != NULL)
	{
		IndexedColour *Ptr = Info->TheList->GetUndeletedHead();
		while (Ptr != NULL)
		{
			if (Ptr == ParentColour)
				break;

			Ptr = Info->TheList->GetUndeletedNext(Ptr);
		}

		if (Ptr != ParentColour)	// ParentColour not found in the intended destination list!
			ParentColour = NULL;	// Set it to NULL, and we'll find a safe ParentColour below

		if (ParentColour != NULL && ParentColour->IsDeleted())
			ParentColour = NULL;
	}

	if (ParentColour == NULL)
	{
		// If the parent was illegal, it's a big screwup. Oh bums.
		// Luckily, we can fix the problem and continue happily
		ERROR3IF(Info->TheColour->FindLastLinkedParent() != NULL,
					"NewColourDlg - had to correct colour with illegal parent pointer!");

		// Set the parent to NULL before we do ourselves an injury!
		Info->TheColour->SetLinkedParent(NULL, COLOURTYPE_NORMAL);
	}
	else
	{
		// Get the parent and child colour definitions in HSV, as this info is useful
		ColourContext *cc = ColourContext::GetGlobalDefault(COLOURMODEL_HSVT);

		if (Force)
			MakeShade = ForceMakeShade;
		else
			MakeShade = GetBoolGadgetSelected(_R(IDC_NEWCOL_SHADE));

		if (MakeShade)
		{
/*
			Info->TheColour->SetLinkedParent(ParentColour, COLOURTYPE_LINKED);

			// Coerce the colour to be an HSV colour
			ColourPicker::ForceColourModel(Info->TheColour, cc);

			// And set it to inherit Hue only
			Info->TheColour->SetInheritsComponent(1, TRUE);
			Info->TheColour->SetInheritsComponent(2, FALSE);
			Info->TheColour->SetInheritsComponent(3, FALSE);
			Info->TheColour->SetInheritsComponent(4, FALSE);
*/
			// Calculate the shading values to keep this colour looking correct
			FIXED24 NewX(0.0);
			FIXED24 NewY(0.0);

			// Work out saturation scaling factor
			ColourHSVT ParentDef;
			ColourHSVT ColourDef;
			cc->ConvertColour(ParentColour, (ColourGeneric *)&ParentDef);
			cc->ConvertColour(Info->TheColour, (ColourGeneric *)&ColourDef);

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
			Info->TheColour->SetLinkedParent(ParentColour, COLOURTYPE_TINT);
			Info->TheColour->SetShadeValues(NewX, NewY);
		}
		else
		{
			// Only change the colour if it is already named - otherwise, we're making
			// a local colour into a named colour, and we don't want to alter it.
			if (Info->TheColour->IsNamed())
			{
				// Copy the colour from the parent - the name will be fixed up below
				*Info->TheColour = *ParentColour;
				Info->TheColour->SetLinkedParent(ParentColour, Info->TheColour->GetType());
			}
		}
	}

	String_256 NewName("");
	if (!Force)
		NewName = GetStringGadgetValue(_R(IDC_NEWCOL_NAME));

	if (NewName.IsEmpty())				// User gave NULL name. Use 'New Colour' instead.
		NewName.MakeMsg(_R(IDS_NEWCOLOUR));

	// We must set the new name for the colour - first make sure it's unique
	String_64 UniqueName;
	if (Info->TheList->GenerateUniqueColourName((const StringBase *) &NewName, &UniqueName) && !Force)
	{
		// If the name had to be changed, and the force flag is off (the user brought up the dialogue
		// and entered a name for themselves) then warn them that the name has been changed to
		// protect the innocent. If you can call the CamEPS filter innocent...

		TCHAR TempStr[256];
		String_256 jcf(_R(IDS_NEWCOL_RENAMED));
		wsprintf(TempStr, jcf, (LPCTSTR) UniqueName);

		// We still don't have a f***ing way of reporting a f***ing message to the user which uses
		// a STRING so we can f***ing include f***ing useful information at runtime!
		// Thus, we InformError to do it. This will use the error beep and a scary huge pling icon,
		// and the user will brown his pants thinking that an access violation complete death
		// scenario is upon him.
		Error::SetError(0, TempStr, 0);
		InformError();
	}

	Info->TheColour->SetName(UniqueName);

	if (!Force)
		Close();

	End();
}



/********************************************************************************************

>	OpState	NewColourDlg::GetState(String_256*, OpDescriptor*)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/9/95
	Purpose:	NewColourDlg GetState method

********************************************************************************************/

OpState	NewColourDlg::GetState(String_256*, OpDescriptor*)
{    
	OpState OpSt;
	return(OpSt);
}


		 
/********************************************************************************************

>	BOOL NewColourDlg::Init(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/9/95

	Purpose:	NewColourDlg Init method. Called by sginit.cpp

********************************************************************************************/

BOOL NewColourDlg::Init(void)
{  
	return (RegisterOpDescriptor(
								0,
								_R(IDS_NEWCOLOURDLG),
								CC_RUNTIME_CLASS(NewColourDlg),
								OPTOKEN_NEWCOLOURDLG,
								NewColourDlg::GetState,
								0,	// help ID
								0   // bubble ID
								)
			); 
}   
 


/********************************************************************************************

>	BOOL NewColourDlg::Create()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/9/95
	Returns:	TRUE if successful, else FALSE
	Purpose:	NewColourDlg create method 

********************************************************************************************/

BOOL NewColourDlg::Create()
{
	return(DialogOp::Create());
}



/********************************************************************************************

>	void NewColourDlg::DoWithParam(OpDescriptor*,OpParam* pOpParam)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/9/95
	Inputs:		pOpParam = ptr to a NewColourParam struct

	Purpose:	Creates then opens the dialog 

********************************************************************************************/


void NewColourDlg::DoWithParam(OpDescriptor*, OpParam* pOpParam)
{
	ERROR3IF(pOpParam == NULL, "Come on, play by the rules");

	Info = (NewColourParam *) pOpParam;

	if (Info->TheColour == NULL || Info->TheList == NULL)
	{
		ERROR3("NewColourDlg parameters are unhelpful to say the least");
		End();
		return;
	}

	// Check that the colour's intended parent is safe (in the destination document)
	IndexedColour *Parent = Info->TheColour->FindLastLinkedParent();
	if (Parent != NULL)
	{
		IndexedColour *Ptr = Info->TheList->GetUndeletedHead();
		while (Ptr != NULL)
		{
			if (Ptr == Parent)
				break;

			Ptr = Info->TheList->GetUndeletedNext(Ptr);
		}

		if (Ptr != Parent)	// Parent not found in the intended destination list!
			Parent = NULL;	// Set it to NULL, and we'll find a safe parent below
	}

	// --- Make sure the colour defaults to having some sort of safe parent hint
	if (Parent == NULL || Parent->IsDeleted())
	{
		IndexedColour *Bob = (IndexedColour *) Info->TheList->GetUndeletedHead();
		Info->TheColour->SetLinkedParent(Bob, Info->TheColour->GetType());
	}

	// --- Check for key shortcuts
	// If various modifier keys are held down when the dlg is invoked, then it will
	// shortcut the process by proceeding with certain options set.
	if (KeyPress::IsConstrainPressed())
	{
		if (KeyPress::IsAdjustPressed())
			CommitDetails(TRUE, MakeShade);		// Force previously used setting (ctrl-shift)
		else
			CommitDetails(TRUE, TRUE);			// Force "shade of"	(ctrl only)
		return;
	}

	if (KeyPress::IsAdjustPressed())
	{
		CommitDetails(TRUE, FALSE);				// Force "a copy of" (shift only)
		return;
	}

	// --- Otherwise, create and open the dialogue
	if (!Create())
	{
		InformError();
  		End();
	}
}



/********************************************************************************************

>	static BOOL NewColourDlg::InvokeDialog(ColourList *ParentList, IndexedColour *SourceAndResult,
											NewColourDlgType Flags = NEWCOLOUR_NAMEDORUNNAMED)

	Author:	Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/9/95

	Inputs:		ParentList	 - the list in which the colour resides (not NULL)
				SourceAndResult	- the colour which is being created
				Flags - Indicates if the colour can be either named or unnamed, or if it should
				limit the result to specifically a named or unnamed colour.

	Outputs:	SourceAndResult is updated depending on its original state and the
				users subsequent choices. Normal colours may be made linked (and
				will use the parent colour pointer "hint" if available to provide
				a default parent colour).

				NOTE: It will be returned as either a local colour or a colour style - check
				IsNamed() to distinguish this case, and add it to the colour list in
				an appropriate manner!

	Returns:	TRUE if it succeeded - in this case, you should add your SourceAndResult colour
				to the colour list in an *appropriate* manner.

				FALSE if it failed or the user hit Cancel - in this case, you should delete
				your SourceAndResult colour.

	Purpose:	Opens the new colour dialogue on screen, elicits a response, 
				and returns having set up the new colour according to the users instructions.
				This is a MODAL dlg.

********************************************************************************************/

BOOL NewColourDlg::InvokeDialog(ColourList *ParentList, IndexedColour *Result,
								NewColourDlgType Flags)
{
	ERROR3IF(ParentList == NULL || Result == NULL, "Come on, play by the rules");

	NewColourParam Info(ParentList, Result, Flags);
	OpDescriptor *TheDlg = OpDescriptor::FindOpDescriptor(CC_RUNTIME_CLASS(NewColourDlg));

	ERROR3IF(TheDlg == NULL, "NewColourDlg::InvokeDialog is unable to find the Dlg OpDescriptor");

	if (TheDlg != NULL)
		TheDlg->Invoke(&Info);

	return(Info.Result);
}

