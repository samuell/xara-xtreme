// $Id$
//------------------------------------------------
//  CBiasGainGadget.cpp
//------------------------------------------------
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

#include "biasgdgt.h"
#include "bars.h"
//#include "biasres.h"
#include "biasdlg.h"

//#include "ccomboud.h"			// this class now needs to know about CustomComboBoxControlDataItem
//#include "app.h"				// for the new bitmap resources
#include "fillval.h"

// we'd best have this as well ....

#define new CAM_DEBUG_NEW					// use camelot new












// statics definitions

const double  CBiasGainGadget::kBiasPresets_s[ eNumberOfPresets ]  =  { 0.0,  0.5, -0.5,  0.0,  0.0 };
const double  CBiasGainGadget::kGainPresets_s[ eNumberOfPresets ]  =  { 0.0,  0.0,  0.0,  0.5, -0.5 };

// CGS //////////////////////////////////////////////////////////////////////////////////////////

// functions that set our beloved pointers ....

void CBiasGainGadget::SetStartColour(DocColour* NewCol)
{
	StartColour = NewCol;
}

void CBiasGainGadget::SetEndColour(DocColour* NewCol)
{
	EndColour = NewCol;
}

void CBiasGainGadget::SetStartTransp(UINT32* NewTransp)
{
	StartTransp = NewTransp;
}

void CBiasGainGadget::SetEndTransp(UINT32* NewTransp)
{
	EndTransp = NewTransp;
}

void CBiasGainGadget::SetFillEffect(FillEffectAttribute* NewFillEffect)
{
	if (FillEffect) delete (FillEffect);
	FillEffect = NewFillEffect;
}

// CGS //////////////////////////////////////////////////////////////////////////////////////////



	
// standard object services ///////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------------------------------------
//	Author:		Harrison Ainsworth
//	Date:		06/99
//	Purpose:	Set each member to a default value.
//-------------------------------------------------------------------------------------------------

CBiasGainGadget::CBiasGainGadget ()
//	: pOwningBar_m( 0 ),
	: GadgetID_m( 0 ),
	  BubbleID_m( 0 ),
	  StatusID_m( 0 ),
	  pDialog_m( 0 )
{
	useFillProfile = FALSE;
	useTranspFillProfile = FALSE;

	StartColour = NULL;
	EndColour = NULL;
	EndColour2 = NULL;
	EndColour3 = NULL;
	FillEffect = NULL;
	StartTransp = NULL;
	EndTransp = NULL;
	EndTransp2 = NULL;
	EndTransp3 = NULL;
}


/*//-------------------------------------------------------------------------------------------------
//	Author:		Harrison Ainsworth
//	Date:		06/99
//	Purpose:	Set each member to a default value. except set DlgTitle_m with the input
//-------------------------------------------------------------------------------------------------

CBiasGainGadget::CBiasGainGadget ( const String_64& DlgTitle )
	: pOwningBar_m( 0 ),
	  DlgTitle_m( DlgTitle ),
	  GadgetID_m( 0 ),
	  BubbleID_m( 0 ),
	  StatusID_m( 0 ),
	  pDialog_m( 0 )
{
}*/




CBiasGainGadget::~CBiasGainGadget ()
{

	if (FillEffect) delete (FillEffect);
	
	delete  pDialog_m;

}








/****************************************************************************************
Function  : CBiasGainGadget::Init
Author    : Mikhail Tatarnikov
Purpose   : Initializes the gadget
Returns   : void
Exceptions: 
Parameters: [in] DialogOp* pOwningDialog - the parent dialog;
            [in] CGadgetID GadgetID		 - the gadget ID;
            [in] UINT32	   BubbleID		 - tooltip help string ID;
            [in] UINT32	   StatusID		 - status bar string ID.
Notes     : 
****************************************************************************************/
void CBiasGainGadget::Init(DialogOp* pOwningDialog, CGadgetID GadgetID, UINT32 BubbleID, UINT32 StatusID)
{
	// check preconditions
	if( ( pOwningDialog != 0 ) && ( GadgetID != 0 ) && ( BubbleID != 0 ) && ( StatusID != 0 ) )
	{
		// set members
		pOwningDialog_m   = pOwningDialog;
		GadgetID_m		  = GadgetID;
		InfobarGadgetID_m = (CGadgetID)-1;
		BubbleID_m        = BubbleID;
		StatusID_m        = StatusID;
	}
}


/****************************************************************************************
Function  : CBiasGainGadget::Enable
Author    : Mikhail Tatarnikov
Purpose   : Enables/disables the gadget.
Returns   : void
Exceptions: 
Parameters: [in] BOOL Enable - TRUE for enabling, FALSE for disabling.
Notes     : 
****************************************************************************************/
void CBiasGainGadget::Enable(BOOL Enable)
{
	pOwningDialog_m->EnableGadget(GadgetID_m, Enable);
}




/****************************************************************************************
Function  : CBiasGainGadget::GetPresetBiasGainValue
Author    : Mikhail Tatarnikov
Purpose   : Obtains the preset value based on its index
Returns   : void
Exceptions: 
Parameters: [in]  WORD				Index - the index of preset to obtain;
            [out] CProfileBiasGain& Value - preset value for the index.
Notes     : Reverse operation for FindPresetBiasGain().
****************************************************************************************/
void CBiasGainGadget::GetPresetBiasGainValue(WORD Index, CProfileBiasGain& Value) const
{
	// check preconditions
	if( Index <= WORD(eNumberOfPresets) )
	{
		Value.SetBiasGain( AFp( kBiasPresets_s[ Index ] ),  AFp( kGainPresets_s[ Index ] ) );
	}
}



/****************************************************************************************
Function  : CBiasGainGadget::FindPresetBiasGain
Author    : Mikhail Tatarnikov
Purpose   : Finds a preset index based on the value, or -1 if couldn't find (custom preset).
Returns   : INT32 - the found preset index.
Exceptions: 
Parameters: [in] CProfileBiasGain const& Value - the preset we'll search for.
Notes     : Reverse operation for GetPresetBiasGainValue().
****************************************************************************************/
INT32 CBiasGainGadget::FindPresetBiasGain(CProfileBiasGain const& Value) const
{
	CProfileBiasGain preset;
	for (INT32 i = 0; i < (INT32)eNumberOfPresets; i++)
	{
		GetPresetBiasGainValue (i, preset);
		if (Value == preset)
			return i;					// is a preset
	}
	
	return -1;		// is a custom
}


/****************************************************************************************
Function  : CBiasGainGadget::ReInitialiseDialog
Author    : Mikhail Tatarnikov
Purpose   : Updates the dialog with a profile value
Returns   : void
Exceptions: 
Parameters: [in] CProfileBiasGain* ReInitOn - the new value to display in the dialog;
            [in] BOOL			   bMany	- 
Notes     : 
****************************************************************************************/
void CBiasGainGadget::ReInitialiseDialog(CProfileBiasGain* ReInitOn, BOOL bMany)
{
	if (/*(ReInitOn != NULL) &&*/ (pDialog_m != NULL))
	{
		if ((ReInitOn != NULL) || (bMany == TRUE))
		{
			pDialog_m->ReInitialiseDialog (ReInitOn, bMany);
		}
	}
}


/****************************************************************************************
Function  : CBiasGainGadget::GetCurrentDialogProfile
Author    : Mikhail Tatarnikov
Purpose   : Reads the dialog profile value
Returns   : CProfileBiasGain - the value currently displayed in the dialog.
Exceptions: 
Parameters: None
Notes     : The linked dialog MUST be open
****************************************************************************************/
CProfileBiasGain CBiasGainGadget::GetCurrentDialogProfile()
{
	ERROR3IF (!IsDialogOpen (), "The dialogue isn't open - something is seriously wrong!");
	return (pDialog_m->GetCurrentDialogProfile ());
}


/****************************************************************************************
Function  : CBiasGainGadget::Message
Author    : Mikhail Tatarnikov
Purpose   : Message handling routine
Returns   : void
Exceptions: 
Parameters: [in] DialogMsg* pMessage - a message to handle
Notes     : 
****************************************************************************************/
void  CBiasGainGadget::Message(DialogMsg* pMessage)
{
	Message( pMessage,  CProfileBiasGain() );
}


/****************************************************************************************
Function  : CBiasGainGadget::Message
Author    : Mikhail Tatarnikov
Purpose   : Message handling routing
Returns   : void
Exceptions: 
Parameters: [in] DialogMsg*				 pMessage - an incoming message;
            [in] CProfileBiasGain const& BiasGain -
Notes     : 
****************************************************************************************/
void CBiasGainGadget::Message(DialogMsg* pMessage, CProfileBiasGain const& BiasGain)
{
}


/****************************************************************************************
Function  : CBiasGainGadget::CloseDialog
Author    : Mikhail Tatarnikov
Purpose   : Close the related dialog
Returns   : void
Exceptions: 
Parameters: None
Notes     : 
****************************************************************************************/
void  CBiasGainGadget::CloseDialog()
{
	if( pDialog_m != 0 )
	{
		DialogMsg  MessageCancel( 0, DIM_CANCEL, GadgetID_m );

		pDialog_m->Message( &MessageCancel );
	}
}








/****************************************************************************************
Function  : CBiasGainGadget::DialogHasOpened
Author    : Mikhail Tatarnikov
Purpose   : Handling the dialog opening event
Returns   : void
Exceptions: 
Parameters: None
Notes     : 
****************************************************************************************/
void  CBiasGainGadget::DialogHasOpened()
{
	// Put the gadet into the pushed state.
	pOwningDialog_m->SetLongGadgetValue(GadgetID_m, 1);
}


/****************************************************************************************
Function  : CBiasGainGadget::DialogHasClosed()
Author    : Mikhail Tatarnikov
Purpose   : Handling the dialog closing event
Returns   : void
Exceptions: 
Parameters: None
Notes     : 
****************************************************************************************/
void  CBiasGainGadget::DialogHasClosed()
{
	delete  pDialog_m;
	pDialog_m  =  0;

	// Unpush the gadget.
	pOwningDialog_m->SetLongGadgetValue(GadgetID_m, 0);
}


/****************************************************************************************
Function  : CBiasGainGadget::GetGadgetID
Author    : Mikhail Tatarnikov
Purpose   : Returns ID of the attached adget
Returns   : CGadgetID - the attached gadget ID
Exceptions: 
Parameters: None
Notes     : 
****************************************************************************************/
CGadgetID CBiasGainGadget::GetGadgetID() const
{
	return  GadgetID_m;
}


/****************************************************************************************
Function  : CBiasGainGadget::GetDialogTitle
Author    : Mikhail Tatarnikov
Purpose   : Returns the dialog title
Returns   : void
Exceptions: 
Parameters: [out] String_256& DlgTitle - the dialog title.
Notes     : 
****************************************************************************************/
void  CBiasGainGadget::GetDialogTitle(String_256& DlgTitle) const
{
	DlgTitle.Load( BubbleID_m );
}

