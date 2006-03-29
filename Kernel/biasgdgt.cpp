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

#include "ccomboud.h"			// this class now needs to know about CustomComboBoxControlDataItem
//#include "app.h"				// for the new bitmap resources
#include "fillval.h"

// we'd best have this as well ....

#define new CAM_DEBUG_NEW					// use camelot new












// statics definitions

const INT32	  CBiasGainGadget::kNumberOfPresets_s( eNumberOfPresets );
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
	: pOwningBar_m( 0 ),
	  GadgetID_m( 0 ),
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




/*CBiasGainGadget::CBiasGainGadget ( const CBiasGainGadget& Other )
{
}




CBiasGainGadget&  CBiasGainGadget::operator= ( const CBiasGainGadget& Other )
{

	return  *this;

}*/








//-------------------------------------------------------------------------------------------------
//	Author:			Harrison Ainsworth
//	Date:			06/99
//	Purpose:		ersatz constructor
//	Preconditions:	OwningBar points to a InformationBarOp, and the input IDs all refer to resource 
//					entities
//-------------------------------------------------------------------------------------------------

void  CBiasGainGadget::Init
(
	InformationBarOp*	pOwningBar,
	CGadgetID			GadgetID,
	UINT32				BubbleID,
	UINT32				StatusID
)
{ 
	// check preconditions
	if( ( pOwningBar != 0 ) && ( GadgetID != 0 ) && ( BubbleID != 0 ) && ( StatusID != 0 ) )
	{
		// set members
		{
			//pOwningBar_m = NULL;					// MUST be set via LinkInfoBar
			pOwningBar_m  =  pOwningBar;
			GadgetID_m    =  GadgetID;
			InfobarGadgetID_m = -1;
			BubbleID_m    =  BubbleID;
			StatusID_m    =  StatusID;
		}


		pOwningBar->DeleteAllValues( GadgetID );

		String_64 Str;

		CustomComboBoxControlDataItem* theItem = NULL;
			/*
	------>		CustomComboBoxControlData is a public class which contains:
				RECT itemRect;			// the bounds of our item (do not init - calculated auto)
				BOOL itemInPos;			// whether there is an item in this position
				BOOL mouseOverItem;		// whether the mouse is over this item (used to indicate
										// mouse selection focus)
				BOOL itemSelected;		// whether the item is selected or not
	------>		HBITMAP itemBmp;		// the items bitmap (if we need to directly store it)
			*/

		HINSTANCE hMain = AfxGetResourceHandle();

		for (INT32 i = 0; i < 6; i++)
		{
			theItem = new CustomComboBoxControlDataItem ();

			if (theItem)
			{	
				switch (i)
				{
					case 0:
						Str.Load( _R(IDS_PROFILE1) );
						theItem->itemBmp =
							(HBITMAP) LoadBitmap (hMain, MAKEINTRESOURCE(_R(IDB_PROFILE1)));
					break;
					case 1:
						Str.Load( _R(IDS_PROFILE2) );
						theItem->itemBmp =
							(HBITMAP) LoadBitmap (hMain, MAKEINTRESOURCE(_R(IDB_PROFILE2)));
					break;
					case 2:
						Str.Load( _R(IDS_PROFILE3) );
						theItem->itemBmp =
							(HBITMAP) LoadBitmap (hMain, MAKEINTRESOURCE(_R(IDB_PROFILE3)));
					break;
					case 3:
						Str.Load( _R(IDS_PROFILE4) );
						theItem->itemBmp =
							(HBITMAP) LoadBitmap (hMain, MAKEINTRESOURCE(_R(IDB_PROFILE4)));
					break;
					case 4:
						Str.Load( _R(IDS_PROFILE5) );
						theItem->itemBmp =
							(HBITMAP) LoadBitmap (hMain, MAKEINTRESOURCE(_R(IDB_PROFILE5)));
					break;
					case 5:
						Str.Load( _R(IDS_PROFILE6) );
						theItem->itemBmp =
							(HBITMAP) LoadBitmap (hMain, MAKEINTRESOURCE(_R(IDB_PROFILE6)));
					break;
				}

				theItem->itemName = Str;
				theItem->itemID = i;
				pOwningBar->SetCustomComboGadgetValue ( GadgetID, theItem, TRUE, 0);
				theItem = NULL;
			}
		}

		//*pOwningBar->*/SetGadgetWritable( pOwningBar->WindowID,  GadgetID, TRUE );
		HWND gadget = ::GetDlgItem(pOwningBar->WindowID, GadgetID);
		SendMessage(gadget, EM_SETREADONLY, FALSE, 0);	// Clear the Read Only Flag
		pOwningBar->SetGadgetHelp( GadgetID,  BubbleID,  StatusID);
	}

}

void  CBiasGainGadget::LinkControlButton
(
	InformationBarOp*	pOwningBar,
	CGadgetID			GadgetID,
	UINT32				BubbleID,
	UINT32				StatusID
)
{ 
	// check preconditions
	if( ( pOwningBar != 0 ) && ( GadgetID != 0 ) && ( BubbleID != 0 ) && ( StatusID != 0 ) )
	{
		// set members
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
			useFillProfile = FALSE;
			useTranspFillProfile = FALSE;
		}

		HWND gadget = ::GetDlgItem(pOwningBar->WindowID, GadgetID);
		SendMessage(gadget, EM_SETREADONLY, FALSE, 0);	// Clear the Read Only Flag
		pOwningBar->SetGadgetHelp( GadgetID,  BubbleID,  StatusID);
	}
}

//void CBiasGainGadget::LinkInfoBar (InformationBarOp* pBarOp/*, CBiasGainDlg* pMyDlg*//*, CBiasGainGadget& pBarCtrl*/)
//{
	//pOwningBar_m = pBarOp; //pDialog_m = pMyDlg; //InfobarGadgetID_m = pBarCtrl.GetGadgetID ();
//}

void  CBiasGainGadget::Init
(
	DialogOp*	pOwningDialog,
	CGadgetID	GadgetID,
	UINT32		BubbleID,
	UINT32		StatusID
)
{ 
	// check preconditions
	if( ( pOwningDialog != 0 ) && ( GadgetID != 0 ) && ( BubbleID != 0 ) && ( StatusID != 0 ) )
	{
		// set members
		{
			pOwningBar_m = NULL;					// MUST be set via LinkInfoBar
			pOwningDialog_m  =  pOwningDialog;
			GadgetID_m		 =  GadgetID;
			InfobarGadgetID_m = -1;
			BubbleID_m       =  BubbleID;
			StatusID_m       =  StatusID;
		}

		pOwningDialog->DeleteAllValues( GadgetID );

		String_64 Str;

		CustomComboBoxControlDataItem* theItem = NULL;
			/*
	------>		CustomComboBoxControlData is a public class which contains:
				RECT itemRect;			// the bounds of our item (do not init - calculated auto)
				BOOL itemInPos;			// whether there is an item in this position
				BOOL mouseOverItem;		// whether the mouse is over this item (used to indicate
										// mouse selection focus)
				BOOL itemSelected;		// whether the item is selected or not
	------>		HBITMAP itemBmp;		// the items bitmap (if we need to directly store it)
			*/

		HINSTANCE hMain = AfxGetResourceHandle();

		for (INT32 i = 0; i < 5/*6*/; i++)
		{
			theItem = new CustomComboBoxControlDataItem ();
			
			switch (i)
			{
				case 0:
					Str.Load( _R(IDS_PROFILE1) );
					theItem->itemBmp =
						(HBITMAP) LoadBitmap (hMain, MAKEINTRESOURCE(_R(IDB_PROFILE1)));
				break;
				case 1:
					Str.Load( _R(IDS_PROFILE2) );
					theItem->itemBmp =
						(HBITMAP) LoadBitmap (hMain, MAKEINTRESOURCE(_R(IDB_PROFILE2)));
				break;
				case 2:
					Str.Load( _R(IDS_PROFILE3) );
					theItem->itemBmp =
						(HBITMAP) LoadBitmap (hMain, MAKEINTRESOURCE(_R(IDB_PROFILE3)));
				break;
				case 3:
					Str.Load( _R(IDS_PROFILE5) );
					theItem->itemBmp =
						(HBITMAP) LoadBitmap (hMain, MAKEINTRESOURCE(_R(IDB_PROFILE5)));
				break;
				case 4:
					Str.Load( _R(IDS_PROFILE4) );
					theItem->itemBmp =
						(HBITMAP) LoadBitmap (hMain, MAKEINTRESOURCE(_R(IDB_PROFILE4)));
				break;
				/*case 5:
					Str.Load( _R(IDS_PROFILE6) );
					theItem->itemBmp =
						(HBITMAP) LoadBitmap (hMain, MAKEINTRESOURCE(_R(IDB_PROFILE6)));
				break;*/
			}

			theItem->itemName = Str;
			theItem->itemID = i;
			pOwningDialog->SetCustomComboGadgetValue ( GadgetID, theItem, TRUE, 0);
			theItem = NULL;
		}

		//*pOwningBar->*/SetGadgetWritable( pOwningBar->WindowID,  GadgetID, TRUE );
		HWND gadget = ::GetDlgItem(pOwningDialog->WindowID, GadgetID);
		SendMessage(gadget, EM_SETREADONLY, FALSE, 0);	// Clear the Read Only Flag
		pOwningDialog->SetGadgetHelp( GadgetID,  BubbleID,  StatusID);
	}

}


//-------------------------------------------------------------------------------------------------
//	Author:			Harrison Ainsworth
//	Date:			06/99
//	Purpose:		grey-out or enable the gadget
//-------------------------------------------------------------------------------------------------

void  CBiasGainGadget::Enable ( BOOL Enable )
{

	pOwningBar_m->EnableGadget( GadgetID_m, Enable );

}


//-------------------------------------------------------------------------------------------------
//	Author:			Harrison Ainsworth
//	Date:			06/99
//	Purpose:		
// Postconditions:	returned value is 1 greater than the maximum value passable as an Index to 
//					GetPresetBiasGainValue()
//-------------------------------------------------------------------------------------------------

WORD  CBiasGainGadget::GetNumberOfPresets ()   const
{

	return  WORD( kNumberOfPresets_s );

}


//-------------------------------------------------------------------------------------------------
//	Author:			Harrison Ainsworth
//	Date:			06/99
//	Purpose:		
// Preconditions:	Index is <= 5 and >= 0
// Postconditions:	Value is set to a preset corresponding to Index
//-------------------------------------------------------------------------------------------------

void  CBiasGainGadget::GetPresetBiasGainValue ( WORD Index,  CProfileBiasGain& Value )   const
{

	// check preconditions
	if( Index <= WORD( kNumberOfPresets_s ) )
	{
		Value.SetBiasGain( AFp( kBiasPresets_s[ Index ] ),  AFp( kGainPresets_s[ Index ] ) );
	}

}


//-------------------------------------------------------------------------------------------------
//	Author:			Chris Snook
//	Date:			18/1/2000
//	Purpose:		Allows the gadget to be responsible for finding the correct profile index
//					(which is to be displayed within the dialog)
// Preconditions:	Value is valid
// Postconditions:	Returned value is set to the corresponding index (or custom)
//-------------------------------------------------------------------------------------------------

WORD CBiasGainGadget::FindPresetBiasGain ( CProfileBiasGain const& Value )   const
{
	CProfileBiasGain preset;
	for (WORD i = 0; i < GetNumberOfPresets (); i++)
	{
		GetPresetBiasGainValue (i, preset);
		if (Value == preset)
		{
			return (i);					// is a preset
		}
	}
	
	return (GetNumberOfPresets ());		// is a custom
}


//-------------------------------------------------------------------------------------------------
//	Author:			Chris Snook
//	Date:			17/1/2000
//	Purpose:		if linked dialog is open (and the selection has changed) - then we need to reset
//					the contents of the dialog to reflect this
//	Preconditions:	ReInitOn is valid
//	Postconditions:	the linked dialog display is updated to reflect the supplied CProfileBiasGain object
//-------------------------------------------------------------------------------------------------

void CBiasGainGadget::ReInitialiseDialog (CProfileBiasGain* ReInitOn, BOOL bMany)
{
	if (/*(ReInitOn != NULL) &&*/ (pDialog_m != NULL))
	{
		if ((ReInitOn != NULL) || (bMany == TRUE))
		{
			pDialog_m->ReInitialiseDialog (ReInitOn, bMany);
		}
	}
}

//-------------------------------------------------------------------------------------------------
//	Author:			Chris Snook
//	Date:			15/3/2000
//	Purpose:		return the current biasgain profile
//	Preconditions:	-
//	Postconditions:	The linked dialog MUST be open
//-------------------------------------------------------------------------------------------------

CProfileBiasGain CBiasGainGadget::GetCurrentDialogProfile ()
{
		ERROR3IF (!IsDialogOpen (), "The dialogue isn't open - something is seriously wrong!");
		return (pDialog_m->GetCurrentDialogProfile ());
}


// handle messages --------------------------------------------------------------------------------

void  CBiasGainGadget::Message ( DialogMsg* pMessage )
{

	Message( pMessage,  CProfileBiasGain() );

}


//-------------------------------------------------------------------------------------------------
//	Author:			Harrison Ainsworth
//	Date:			06/99
//	Purpose:		handle messages directed at the gadget - currently those indicating a selection
//					of a dropdown item - which will be either a preset value or a request to launch
//					a dialog
//	Preconditions:	input message validly points to a DialogMsg
//-------------------------------------------------------------------------------------------------

void  CBiasGainGadget::Message ( DialogMsg* pMessage,  CProfileBiasGain const& BiasGain )
{

	static const WORD  kLaunchDialog( 5 );


	switch( pMessage->DlgMsg )
	{
		case DIM_SELECTION_CHANGED :
		{
			WORD  DropdownListIndex;
			if (pOwningBar_m != NULL) { pOwningBar_m->GetValueIndex( GadgetID_m, &DropdownListIndex ); }
			else { pOwningDialog_m->GetValueIndex( GadgetID_m, &DropdownListIndex ); }

			switch( DropdownListIndex )
			{
				// get preset value to match the dropdown list index
				//
				case  0 :
				case  1 :
				case  2 :
				case  3 :
				case  4 :
				{
					CProfileBiasGain  SelectedProfile;

					GetPresetBiasGainValue( DropdownListIndex, SelectedProfile );
					SelectedProfile.SetGeneratesInfiniteUndo (FALSE);
					// since a value has been chosen from the range of presets, a dialog (if present) is no longer needed
					//CloseDialog();

					// send a message back to the owning infobar to give it the chosen value
					/*if (pOwningBar_m != NULL )*/ { BROADCAST_TO_CLASS( DialogMsg( 0, DIM_COMMIT, InfobarGadgetID_m, reinterpret_cast<INT32>( &SelectedProfile ) ),  DialogBarOp ); }

					break;
				}

				// launch a dialog box to allow value to be set
				//
				/*case  kLaunchDialog :
				{
					if( pDialog_m == 0 )
					{
						OpDescriptor*  pOpDescriptor  =  OpDescriptor::FindOpDescriptor( OPTOKEN_BIASGAIN_DLG );
						if( pOpDescriptor != 0 )
						{
							pDialog_m  =  new CBiasGainDlg();

							pDialog_m->LinkInfoBar (pOwningBar_m);

							OpParam  Param( reinterpret_cast<INT32>( this ),  reinterpret_cast<INT32>( &BiasGain ) );
							pDialog_m->DoWithParam( pOpDescriptor, &Param );
						}
					}
					break;
				}*/
			}
			break;
		}
	}

}


void  CBiasGainGadget::CloseDialog ()
{

	if( pDialog_m != 0 )
	{
		DialogMsg  MessageCancel( 0, DIM_CANCEL, GadgetID_m );

		pDialog_m->Message( &MessageCancel );

		// the above also (indirectly) retoggles the controlling buttons state
		//pOwningBar_m->SetLongGadgetValue (GadgetID_m, 0);
	}

}








/*//-------------------------------------------------------------------------------------------------
//	Author:			Harrison Ainsworth
//	Date:			06/99
//	Purpose:		
//	Preconditions:	
//	Postconditions:	
//-------------------------------------------------------------------------------------------------

CProfileBiasGainDlg*  CBiasGainGadget::GetDialog ()   const
{

	return  pBiasGainDlg_m;

}*/


void  CBiasGainGadget::DialogHasOpened ()
{
}


void  CBiasGainGadget::DialogHasClosed ()
{

	delete  pDialog_m;
	pDialog_m  =  0;

	// and retoggle the controlling button
	pOwningBar_m->SetLongGadgetValue (GadgetID_m, 0);
}


CGadgetID  CBiasGainGadget::GetGadgetID ()   const
{

	return  GadgetID_m;

}


void  CBiasGainGadget::GetDialogTitle ( String_256& DlgTitle )   const
{

	DlgTitle.Load( BubbleID_m );

}

/*void CBiasGainGadget::SetDialogBiasGain(CProfileBiasGain const& Profile)
{
	if (pDialog_m)
	{
		pDialog_m->SetProfile(Profile);
	}
}*/

void CBiasGainGadget::SetGadgetWritable(CWindowID WindowID, INT32 id, BOOL enable)
{
	// Get the window handle of the gadget, from the gadget ID
	HWND gadget = ::GetDlgItem(WindowID, id);

	// See if it's got a child window (it may be a Combo Box)
	HWND hEdit = ::ChildWindowFromPoint(gadget, CPoint(1,1));

	if (hEdit)				// Was there a child window ?
		gadget = hEdit;		// Yes, so send the message to it

 	if (enable)
		::SendMessage(gadget, EM_SETREADONLY, FALSE, 0);	// Clear the Read Only Flag
	else
		::SendMessage(gadget, EM_SETREADONLY, TRUE, 0);		// Set the Read Only Flag
}

