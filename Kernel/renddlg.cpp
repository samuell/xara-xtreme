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
// A simple Dialog That does some Gavin Rendering into itself

/*
*/

#include "camtypes.h"
#include "renddlg.h"

//#include "mario.h"
//#include "rikdlg.h"
//#include "rik.h"
#include "menuops.h"

#include "ccdc.h"		// specific #includes needed for kernel-rendered dialogues
#include "dlgcol.h"
#include "fillval.h"
#include "grnddib.h"

// This is not compulsory, but you may as well put it in so that the correct version
// of your file can be registered in the .exe
DECLARE_SOURCE("$Revision$");

// An implement to match the Declare in the .h file.
// If you have many classes, it is recommended to place them all together, here at the start of the file
CC_IMPLEMENT_DYNCREATE(RenderDemoDlg, DialogOp)

// This will get Camelot to display the filename and linenumber of any memory allocations
// that are not released at program exit
#define new CAM_DEBUG_NEW


// Set the static vars of the render dialog
const CDlgMode RenderDemoDlg::Mode = MODELESS;
const UINT32 RenderDemoDlg::IDD = _R(IDD_RENDERDEMO);




/*******************************************************************************************

>	RenderDemoDlg::RenderDemoDlg() : DialogOp(RenderDlg::IDD, RenderDlg::Mode) 

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/94
	Purpose:	Constructs a Render Dialog

*******************************************************************************************/

RenderDemoDlg::RenderDemoDlg() : DialogOp(RenderDemoDlg::IDD, RenderDemoDlg::Mode) 
{
	// Set the 2 colours that we can toggle between
	First = DocColour(255,0,250);
	Second = DocColour(0,255,255);
	ShowFirst = TRUE;
}       



/*******************************************************************************************

>	RenderDemoDlg::~RenderDemoDlg()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/94
	Purpose:	Destructor, does nothing

*******************************************************************************************/

RenderDemoDlg::~RenderDemoDlg()
{
} 



/*******************************************************************************************

>	MsgResult RenderDemoDlg::Message(Msg* Message)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/94
	Purpose:	Handles all the Render dialog's messages 

*******************************************************************************************/

MsgResult RenderDemoDlg::Message(Msg* Message)
{
	// See if it is for us
	if (IS_OUR_DIALOG_MSG(Message))
	{
		// it is
		DialogMsg* Msg = (DialogMsg*)Message;

		// decide what to do
		switch (Msg->DlgMsg)
		{
			// Create message
			case DIM_CREATE :
				break;

			// Close and destroy the dialog 
			case DIM_COMMIT :
			case DIM_CANCEL :
			{
				Close();
				End();
				break;
			}

			case DIM_LFT_BN_CLICKED :
			{
				// See which button was pressed
				switch (Msg->GadgetID)
				{
					case _R(IDC_REDRAWBTN) :
					{
						// Toggle the colour
						ShowFirst = ShowFirst ? FALSE : TRUE;

						// invalidate the gadget with the picture in it
						InvalidateGadget(_R(IDC_REDRAW_ME));
						break;
					}
				}
				break;
			}

			case DIM_REDRAW :
			{
				// This is where all the redrawing is done
				// Which control in the window is sending the redraw message (if there are many
				// grdraw controls you can tell which is which from the Gadget ID
				switch (Msg->GadgetID)
				{
					// Draw the redraw_me control in here
					case _R(IDC_REDRAW_ME) :
					{
						// Render this control
						RenderControl((ReDrawInfoType*) Msg->DlgMsgParam);
						break;
					}

					// there are no other controls that should get a redraw message ever
					default :
					{
						// give out an error in debug builds, ignore in retail builds
						ERROR3("Got a redraw message for a control I don't know about");
						break;
					}
				}

				break;
			}
		}
		
		// Return
		return (DLG_EAT_IF_HUNGRY(Msg));
	}

	return OK;  
}  




/********************************************************************************************

>	void RenderDemoDlg::RenderControl(ReDrawInfoType* ExtraInfo)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/10/94
	Inputs:		ExtraInfo - The structure that has the extra data we need to start rendering
	Purpose:	Renders a grad fill into the main control in this dialog.

********************************************************************************************/

void RenderDemoDlg::RenderControl(ReDrawInfoType* ExtraInfo)
{
	// Go get a render region
	DocRect VirtualSize(-ExtraInfo->dx/2, -ExtraInfo->dy/2, ExtraInfo->dx/2, ExtraInfo->dy/2);
	RenderRegion* pRender = CreateGRenderRegion(&VirtualSize, ExtraInfo);
	if (pRender!=NULL)
	{
		DialogColourInfo RedrawColours;		// Get a supplier for default dlg colours

		// Render stuff in here
		// Build a Linear fill attribute
		LinearFillAttribute MyGradFill;
		MyGradFill.Colour = DocColour(255, 255, 0);
		MyGradFill.EndColour = DocColour(0, 255, 255);
		MyGradFill.StartPoint = DocCoord(0, ExtraInfo->dy);
		MyGradFill.EndPoint = DocCoord(ExtraInfo->dx, 0);

		// Build a path
		Path InkPath;
		InkPath.Initialise(12,12);
		InkPath.FindStartOfPath();

		// Get the coords used to build a shape
		INT32 dx = ExtraInfo->dx / 2;
		INT32 dy = ExtraInfo->dy / 2;
		INT32 Midx = ExtraInfo->dx / 4;
		INT32 Midy = ExtraInfo->dy / 4;

		// build a circle in the middle of the control
		InkPath.InsertMoveTo(DocCoord(Midx, dy));
		InkPath.InsertCurveTo(DocCoord(Midx+Midx/2, dy), DocCoord(dx, Midy+Midy/2), DocCoord(dx, Midy));
		InkPath.InsertCurveTo(DocCoord(dx, Midy-Midy/2), DocCoord(Midx+Midx/2, 0), DocCoord(Midx, 0));
		InkPath.InsertCurveTo(DocCoord(Midx-Midx/2, 0), DocCoord(0, Midy-Midy/2), DocCoord(0, Midy));
		InkPath.InsertCurveTo(DocCoord(0, Midy+Midy/2), DocCoord(Midx-Midx/2, dy), DocCoord(Midx, dy));
		InkPath.IsFilled = TRUE;
		
		// A Grey colour [...hmmm, it's not a very grey grey any more... oragnge more like]
		DocColour Grey(255,200,0);

		// Render the attributes and the a rectangle
		pRender->SaveContext();
		pRender->SetLineColour(Grey);

		// Draw a rectangle to fill in the background - Fill with Dialogue Background colour
		DocRect DrawMe(0, 0, ExtraInfo->dx, ExtraInfo->dy);
		pRender->SetFillColour(RedrawColours.DialogBack());
		pRender->DrawRect(&VirtualSize);

		// Draw some shapes and stuff
		pRender->SetFillGeometry(&MyGradFill, FALSE);
		pRender->DrawPath(&InkPath);

		// Build a path
		Path TriPath;
		TriPath.Initialise(12,12);
		TriPath.FindStartOfPath();

		// build a circle in the middle of the control
		TriPath.InsertMoveTo(VirtualSize.lo);
		TriPath.InsertLineTo(DocCoord(VirtualSize.hi.x, VirtualSize.lo.y));
		TriPath.InsertLineTo(DocCoord(0, VirtualSize.hi.y));
		TriPath.InsertLineTo(VirtualSize.lo);
		TriPath.IsFilled = TRUE;

		LinearFillAttribute MyTriFill;
		MyTriFill.Colour = ShowFirst ? First : Second;
		MyTriFill.EndColour = DocColour(0,0,0);
		MyTriFill.StartPoint = DocCoord(ExtraInfo->dx, 0);
		MyTriFill.EndPoint = DocCoord(0, ExtraInfo->dy);

		pRender->SetFillGeometry(&MyTriFill, FALSE);
		pRender->DrawPath(&TriPath);

		pRender->RestoreContext();

		// Get rid of the render region
		DestroyGRenderRegion(pRender);
	}

	// and animate it!
	if (ShowFirst)
	{
		INT32 Red, Green, Blue;
		First.GetRGBValue(&Red, &Green, &Blue);

		if (Blue>0)
		{
			// Set the colour back again
			Blue -= 10;
			First.SetRGBValue(Red, Green, Blue);

			// redraw it
			InvalidateGadget(_R(IDC_REDRAW_ME));
		}
	}
	else
	{
		// Set the colour back to how it was
		First.SetRGBValue(255, 0, 250);
	}
}



/*******************************************************************************************

>	OpState	RenderDemoDlg::GetState(String_256*, OpDescriptor*)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/94
	Purpose:	Returns the OpState of the Render dialogue operation

*******************************************************************************************/

OpState	RenderDemoDlg::GetState(String_256*, OpDescriptor*)
{
	return OpState(FALSE, FALSE);
}

		 

/*******************************************************************************************

>	BOOL RenderDemoDlg::Init()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/94
	Returns:	FALSE if it fails (due to lack of memory)
	Purpose:	Creates an OpDescriptor for a Render Dialog

*******************************************************************************************/
BOOL RenderDemoDlg::Init()
{  
	return RegisterOpDescriptor(0,								// Tool ID
 								_R(IDS_RENDERDEMO), 				// String resouirce ID
								CC_RUNTIME_CLASS(RenderDemoDlg),// Runtime class
 								OPTOKEN_RENDERDEMODLG,			// Token string
 								RenderDemoDlg::GetState,		// GetState function
								0,								// Help ID
								0,								// Bubble ID
								0,								// Resource ID
								0,								// Control ID
								SYSTEMBAR_ILLEGAL,				// Bar ID
								FALSE,							// Recieve system messages
								FALSE,							// Smart duplicate operation
								TRUE,							// Clean operation
								0,								// No vertical counterpart
								0);								// String for one copy only
}   
 

         

/*******************************************************************************************

>	void RenderDemoDlg::Do(OpDescriptor*)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/94
	Purpose:	Creates and shows a Render Demo dialog

*******************************************************************************************/
void RenderDemoDlg::Do(OpDescriptor*)
{
	// if we can create the dialog, then open it
	if (Create())
		Open();
	else
		TRACE( _T("Failed to create Render Demo Dialog\n"));
}


