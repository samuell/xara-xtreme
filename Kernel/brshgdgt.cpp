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


#include "camtypes.h"

#include "brshgdgt.h"
#include "brushdlg.h"

#define new CAM_DEBUG_NEW					// use camelot new

CBrushGadget::CBrushGadget ()
	: pOwningBar_m( 0 ),
	  GadgetID_m( 0 ),
	  BubbleID_m( 0 ),
	  StatusID_m( 0 ),
	  pDialog_m( 0 )
{
	DisableTimeStampingInDlg = FALSE;
	EditingBrushDefinition = FALSE;
	pDialog_m = NULL;
}

CBrushGadget::~CBrushGadget ()
{
	delete (pDialog_m);
}

void  CBrushGadget::LinkControlButton
(
	InformationBarOp*	pOwningBar,
	CGadgetID			GadgetID,
	UINT32				BubbleID,
	UINT32				StatusID
)
{ 
	if((pOwningBar != 0) && (GadgetID != 0) && (BubbleID != 0) && (StatusID != 0))
	{
		/// set members
		{
			pOwningBar_m  =  pOwningBar;
			GadgetID_m    =  GadgetID;
			InfobarGadgetID_m = -1;				// set this to an (obviously) invalid value
												// we use to route our data back to the relevant
												// infobar button ....  (which originiates from
												// within the CBiasGainGadget on the
												// CBiasGainDlg
			BubbleID_m    =  BubbleID;
			StatusID_m    =  StatusID;
			//useFillProfile = FALSE;
			//useTranspFillProfile = FALSE;
		}

		HWND gadget = ::GetDlgItem (pOwningBar->WindowID, GadgetID);
		SendMessage (gadget, EM_SETREADONLY, FALSE, 0);	// Clear the Read Only Flag
		pOwningBar->SetGadgetHelp (GadgetID, BubbleID, StatusID);
	}
}

void  CBrushGadget::Message (DialogMsg* pMessage)
{
//	Message( pMessage,  CProfileBiasGain() );
}

void  CBrushGadget::CloseDialog ()
{
	if(pDialog_m != 0)
	{
		DialogMsg  MessageCancel (0, DIM_CANCEL, GadgetID_m);

		pDialog_m->Message (&MessageCancel);

		// the above also (indirectly) retoggles the controlling buttons state
		//pOwningBar_m->SetLongGadgetValue (GadgetID_m, 0);
	}
}

void  CBrushGadget::DialogHasOpened ()
{
}


void  CBrushGadget::DialogHasClosed ()
{
	if (pDialog_m != NULL)
		delete (pDialog_m);
	pDialog_m = NULL;

	// and retoggle the controlling button if we are launched from a bar
//	if (pOwningBar_m != NULL)
//		pOwningBar_m->SetLongGadgetValue (GadgetID_m, 0);
}


CGadgetID  CBrushGadget::GetGadgetID () const
{
	return (GadgetID_m);
}


void CBrushGadget::GetDialogTitle (String_256& DlgTitle) const
{
	DlgTitle.Load (BubbleID_m);
}

void CBrushGadget::HandleBrushButtonClick (BrushData* AppliedBrush, AttrBrushType* pBrush)
{
	if (AppliedBrush == NULL)
	{
		ERROR3("Invalid input parameters");
		return;
	}
	
	if (!(IsDialogOpen ()))
	{
		pDialog_m = new CBrushEditDlg();
		BOOL bMany = FALSE, bAllSameType = TRUE;
								
		//CProfileBiasGain* appliedBiasGain = GetProfileFromSelection (ProfileGadgetID, &bMany, &bAllSameType);

		//if (bAllSameType != FALSE)
		//{
		pDialog_m->InvokeVia (*(this), AppliedBrush, pBrush, bMany);
		//}
	}
	else
	{
		CloseDialog ();
	}
}

void CBrushGadget::HandleBrushButtonClick (BrushHandle Handle)
{
	if (Handle == BrushHandle_NoBrush)
		return;
	if (!(IsDialogOpen ()))
	{
		pDialog_m = new CBrushEditDlg();
						
		pDialog_m->InvokeVia (*(this), Handle);
	}
	else
	{
		CloseDialog ();
	}
}

void CBrushGadget::HandleBrushButtonClick (BrushData* AppliedBrush, AttrBrushType* pBrush, AttrBrushType* pOrigBrush)
{
	if (AppliedBrush == NULL || pBrush == NULL)
	{
		ERROR3("Invalid input parameters");
		return;
	}
	
	if (!(IsDialogOpen ()))
	{
		pDialog_m = new CBrushEditDlg();
		BOOL bMany = FALSE, bAllSameType = TRUE;
								
		//CProfileBiasGain* appliedBiasGain = GetProfileFromSelection (ProfileGadgetID, &bMany, &bAllSameType);

		//if (bAllSameType != FALSE)
		//{
		pDialog_m->InvokeVia (*(this), AppliedBrush, pBrush, pOrigBrush, bMany);
		//}
	}
	else
	{
		CloseDialog ();
	}
}

void CBrushGadget::ReInitialiseDialog (BrushData* pData)
{
	if (/*(ReInitOn != NULL) &&*/ (pDialog_m != NULL))
	{
		if (pData != NULL )
		{
			pDialog_m->ReInitialiseDialog (pData);
		}
	}
}
