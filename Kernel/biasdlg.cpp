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


DECLARE_SOURCE("$Revision$");


#include "biasdlg.h"
//#include "biasres.h"
//#include "blobtab.h"		// cause our (original) _R(IDC_STATIC1) through _R(IDC_STATIC4) + _R(IDC_STATIC6)
							// clashed with those in this; so we cannot have our own => use these
							// values instead
#include "nodershp.h"
#include "lineattr.h"
#include "biasgdgt.h"

//#include "dialogop.h"
//#include "barsdlgs.h"
#include "rndrgn.h"
#include "stockcol.h"
#include "qualattr.h"
//#include "rikdlg.h"

// #include "bars.h"
// #include "msg.h"
#include "dlgmgr.h"



CC_IMPLEMENT_DYNCREATE( CBiasGainDlg,  DialogOp )


#define new CAM_DEBUG_NEW




// statics //

const	UINT32		CBiasGainDlg::IDD   =  _R(IDD_BIASGAIN);
const	CDlgMode	CBiasGainDlg::Mode  =  MODELESS;

const	INT32		CBiasGainDlg::kSliderLength_s      =  INT32( 200 );
const	INT32		CBiasGainDlg::kSliderHalfLength_s  =  kSliderLength_s / INT32( 2 );
const	INT32		CBiasGainDlg::kSliderMin_s         =  -( kSliderHalfLength_s );
const	INT32		CBiasGainDlg::kSliderMax_s         =  +( kSliderHalfLength_s );






CBiasGainDlg::CBiasGainDlg ()
	: DialogOp( CBiasGainDlg::IDD, CBiasGainDlg::Mode ),
	  pOwningGadget_m( 0 ),
	  Id_m( 0 ),
	  BiasGain_m()
{
	m_pobddStandardProfile = NULL;
}


CBiasGainDlg::~CBiasGainDlg ()
{
	delete m_pobddStandardProfile;
	//EndTimedProcessing();
}




/****************************************************************************************
Function  : CBiasGainDlg::Init
Author    : Mikhail Tatarnikov
Purpose   : Creates an OpDescriptor for a CBiasGainDlg dialog
Returns   : BOOL - TRUE if succeeded, FALSE otherwise.
Exceptions: 
Parameters: None
Notes     : 
****************************************************************************************/
BOOL CBiasGainDlg::Init()
{
	return  RegisterOpDescriptor(
		0,									// Tool ID
 		_R(IDS_BIASGAINDLG),					// String resource ID
		CC_RUNTIME_CLASS( CBiasGainDlg ),	// Runtime class
 		OPTOKEN_BIASGAIN_DLG,				// Token string
 		CBiasGainDlg::GetState,				// GetState function
		0,									// Help ID
		_R(IDBBL_BIASGAIN),					// Bubble ID
		_R(IDD_BARCONTROLSTORE),			// Resource ID
		0,									// _R(IDC_BIASGAIN) - We don't need the automatic enabling/disabling control -
											// everything can be done by the toolbar only, since no one else knows about the fill type
											// (the control state depends on in).
		SYSTEMBAR_EDIT,						// Bar ID
		TRUE,								// Recieve system messages
		FALSE,								// Smart duplicate operation
		TRUE,								// Clean operation
		0,									// No vertical counterpart
		0,									// String for one copy only
		( DONT_GREY_WHEN_SELECT_INSIDE | GREY_WHEN_NO_CURRENT_DOC ) // Auto state flags
	);
}



/******************************************************************************
>	OpState	CBiasGainDlg::GetState(String_256* pHelpString, OpDescriptor*)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/9/94
	Purpose:	Returns the OpState of the CBiasGainDlg dialogue operation
				ie. whether or not it is greyed on the menu etc!!!
******************************************************************************/

/****************************************************************************************
Function  : OpStateCBiasGainDlg::GetState
Author    : Mikhail Tatarnikov
Purpose   : Returns the OpState of the CBiasGainDlg dialogue operation
			(whether or not it should be greyed on the menu etc)
Returns   : void
Exceptions: 
Parameters: [in] String_256*   pHelpString - 
            [in] OpDescriptor*			   - 
Notes     : 
****************************************************************************************/
OpState	CBiasGainDlg::GetState(String_256* pHelpString, OpDescriptor*)
{

	static OpState  DialogState;
	DialogState.Greyed = FALSE;

	return  DialogState;

}



/****************************************************************************************
Function  : CBiasGainDlg::Do
Author    : Mikhail Tatarnikov
Purpose   : Creates and shows a CBiasGainDlg dialog
Returns   : void
Exceptions: 
Parameters: [in/out] OpDescriptor* - 
Notes     : 
****************************************************************************************/
void CBiasGainDlg::Do(OpDescriptor*)
{
	Create();
	Open();
}


//-------------------------------------------------------------------------------------------------
//	Author:			Harrison Ainsworth
//	Date:			07/99
//	Purpose:		Creates and shows a CBiasGainDlg dialog
//	Preconditions:	OpParam* OwningGadgetParams param1 is really a CBiasGainGadget* and
//					param2 is really a CProfileBiasGain*
//-------------------------------------------------------------------------------------------------

/****************************************************************************************
Function  : CBiasGainDlg::DoWithParam
Author    : Mikhail Tatarnikov
Purpose   : Creates and shows a CBiasGainDlg dialog
Returns   : void
Exceptions: 
Parameters: [in] OpDescriptor*				 - 
            [in] OpParam*	   OwningGadgetParams - the dialog parameters (param1 is a CBiasGainGadget*
													and param2 is really a CProfileBiasGain*)
Notes     : 
****************************************************************************************/
void CBiasGainDlg::DoWithParam(OpDescriptor*, OpParam* OwningGadgetParams)
{
	BOOL noProfile = FALSE;
	
	// extract the param info: an owning gadget ptr, and input biasgain value
	pOwningGadget_m  =  reinterpret_cast<CBiasGainGadget*>((void*)(OwningGadgetParams->Param1));
	pOwningGadget_m->SetCBaisGainDlg (this);
	
	const CProfileBiasGain* invokeOn = reinterpret_cast<CProfileBiasGain const*>((void*)(OwningGadgetParams->Param2));
	
	if (invokeOn != NULL)
	{
		BiasGain_m = *(invokeOn);
	}
	else
	{
		noProfile = TRUE;
	}

	// get the ID
	if( pOwningGadget_m != 0 )
	{
		Id_m  =  pOwningGadget_m->GetGadgetID();
	}


	Create();
	Open();

	if (pOwningGadget_m->GetUseFillProfile () == TRUE)
	{
		ToggleFillProfile ();
	}

	if (pOwningGadget_m->GetUseTranspFillProfile () == TRUE)
	{
		ToggleTranspFillProfile ();
	}


	// set the dialog title
	if( pOwningGadget_m != 0 )
	{
		String_256  Title;
		pOwningGadget_m->GetDialogTitle( Title );
		DialogManager::SetTitlebarName( WindowID,  &Title );
	}

	if (noProfile == TRUE)
	{
//		SetCustomComboGadgetValue ( _R(IDC_BIASGAINCOMBO), (CustomComboBoxControlDataItem*) NULL, TRUE, -1);
		m_pobddStandardProfile->SetSelected(-1);

		// and disable all other controls ....
		DisableAllControls ();
	}

	// tell the owning gadget that this dialog has opened
	if( pOwningGadget_m != 0 )
	{
		pOwningGadget_m->DialogHasOpened();
	}
}


/****************************************************************************************
Function  : CBiasGainDlg::Message
Author    : Mikhail Tatarnikov
Purpose   : handle messages for this dialog - including create/cancel and slider and edit
Returns   : MsgResult -
Exceptions: 
Parameters: [in] Msg* Message - an incoming message to handle
Notes     : 
****************************************************************************************/
MsgResult CBiasGainDlg::Message(Msg* Message)
{
	if( IS_OUR_DIALOG_MSG( Message ) )
	{
		DialogMsg* Msg  =  static_cast<DialogMsg*>( Message );//dynamic_cast<DialogMsg*>( Message );   // needs rtti enabled

		switch( Msg->DlgMsg )
		{	
			case DIM_TITLEFOCUSWARN:
			{
				DialogManager::DefaultKeyboardFocus();
			}
			break;
			
			case DIM_CREATE :
			{
				manyDisabledControls = FALSE;
				useFillProfile = FALSE;
				useTranspFillProfile = FALSE;
				
				// initialise gadgets
				InitSliders( BiasGain_m );
				InitEditBoxes( BiasGain_m );
				InitiProfileCombobox();

				InitBiasGainGadget(BiasGain_m);	// Setup combobox with the current bias.

				InvalidateGadget( _R(IDC_CURVE) );
				InvalidateGadget( _R(IDC_CURVEINTERPOLATE) );

				DialogManager::DefaultKeyboardFocus();

				break;
			}

			case DIM_CANCEL :
			{
				if (!(IsCustomComboDropdownVisible (_R(IDC_BIASGAINCOMBO))))
				{
					Close();
					//End();
					//DlgMgr->Delete( WindowID, this );

					// tell the owning gadget that this dialog has closed
					if( pOwningGadget_m != 0 )
					{
						pOwningGadget_m->DialogHasClosed();
						return DLG_EAT_IF_HUNGRY(Msg);	// We cna't do anything else here, since the dialog has just been deleted.
					}
				}
				else
				{
					CloseDropdown (_R(IDC_BIASGAINCOMBO), FALSE);
				}
				break;
			}

			case DIM_LFT_BN_UP:
				DialogManager::DefaultKeyboardFocus();
			break;

			// slider interaction messages
			case DIM_SLIDER_POS_CHANGING :
			{
				BiasGain_m.SetGeneratesInfiniteUndo (TRUE);
				HandleSliderPosChanging( Msg->DlgMsg,  BiasGain_m );
				break;
			}

			case DIM_SLIDER_POS_SET:
			{
				BiasGain_m.SetGeneratesInfiniteUndo (FALSE);
				HandleSliderPosSet( Msg->DlgMsg,  BiasGain_m );
				DialogManager::DefaultKeyboardFocus();
				break;
			}

			case DIM_SLIDER_POS_IDLE:
			{
				BiasGain_m.SetGeneratesInfiniteUndo (FALSE);
				HandleSliderPosSet( Msg->DlgMsg,  BiasGain_m );
				break;
			}

			// edit box interaction messages
			case DIM_COMMIT :
			{
				HandleCommit( Msg->DlgMsg,  BiasGain_m );
				DialogManager::DefaultKeyboardFocus();

				break;
			}

			case DIM_REDRAW :
			{
				if( Msg->GadgetID == _R(IDC_CURVE) )
				{
					DrawCurve( BiasGain_m,  reinterpret_cast<ReDrawInfoType*>( Msg->DlgMsgParam ) );
				}
				else if ( Msg->GadgetID == _R(IDC_CURVEINTERPOLATE))
				{
					if ((useFillProfile == FALSE) && (useTranspFillProfile == FALSE))
					{
						DrawCurveInterpolate( BiasGain_m,  reinterpret_cast<ReDrawInfoType*>( Msg->DlgMsgParam ) );
					}
					else if (useFillProfile == TRUE)
					{
						DrawFillInterpolate( BiasGain_m,  reinterpret_cast<ReDrawInfoType*>( Msg->DlgMsgParam ) );
					}
					else
					{
						DrawTranspFillInterpolate( BiasGain_m,  reinterpret_cast<ReDrawInfoType*>( Msg->DlgMsgParam ) );
					}
				}

				break;
			}

			case DIM_SELECTION_CHANGED:
			{
				if (Msg->GadgetID == _R(IDC_BIASGAINCOMBO))
				{
//					WORD  DropdownListIndex;
//					GetValueIndex( _R(IDC_BIASGAINCOMBO), &DropdownListIndex );
					int iSelected = m_pobddStandardProfile->GetSelected();
					if (iSelected == -1)
						break;
						
					DWORD dwSelected = (DWORD)iSelected;

					switch(dwSelected)
					{
						// get preset value to match the dropdown list index
						//
						case  0 :
						case  1 :
						case  2 :
						case  3 :
						case  4 :
						{
							// first we need to update our dialog to reflect this change

							if (manyDisabledControls == TRUE)
							{
								EnableAllControls ();
							}
							
							BiasGainGadget.GetPresetBiasGainValue(dwSelected, BiasGain_m );
							InitSliders( BiasGain_m );
							InitEditBoxes( BiasGain_m );
							InvalidateGadget( _R(IDC_CURVE) );
							InvalidateGadget( _R(IDC_CURVEINTERPOLATE) );
							
							// and then update all the other stuff in the document

							BiasGainGadget.SetInfobarGadgetID (pOwningGadget_m->GetGadgetID ());
							BiasGainGadget.Message (Msg);//pOwningGadget_m->Message (Msg);
						}
						HandleInput(Msg->DlgMsg, BiasGain_m);
					}
					DialogManager::DefaultKeyboardFocus();
				}
			}
			
			default:
				break;
		}
	
	}

	// Pass all unhandled messages to base class for default processing!
	return  DialogOp::Message( Message );

}  




// protected //////////////////////////////////////////////////////////

//-------------------------------------------------------------------------------------------------
//	Author:			Harrison Ainsworth
//	Date:			07/99
//	Purpose:		Draw graph/curve illustration of the profile function
//	Preconditions:	ReDrawInfoType* ExtraInfo somehow (mystery to me) refers to a dlg picture control
//					CBiasGainDlg derives from DialogOp - so that CreateGRenderRegion() is accessible
//	Postconditions:	the dlg control is drawn into
//-------------------------------------------------------------------------------------------------

void  CBiasGainDlg::DrawCurve( const CProfileBiasGain& CurveShape,  ReDrawInfoType* ExtraInfo )
{
	// drawing constants:  overall resolution, curve steps, curve width, colors
	static const INT32  kWidth      =  15000;
	static const INT32  kHeight     =  15000;
	static const INT32  kStep       =     20;
	static const INT32  kLineWidth  =    500;

	static const StockColour  kBackgoundOutline  =  COLOUR_NONE;
	static const StockColour  kBackgound         =  COLOUR_WHITE;
	static const StockColour  kBackgoundMany     =  COLOUR_LTGREY;	// for the MANY case
	static const StockColour  kCurve             =  COLOUR_DKGREY;


	DocRect  RenderRect( 0, 0,  kWidth, kHeight );

	RenderRegion*  pRenderRegion  =  CreateGRenderRegion( &RenderRect, ExtraInfo );

	if( pRenderRegion != 0 )
	{
		pRenderRegion->SaveContext();

		// set drawing quality
		Quality           QualityThing( Quality::QualityMax );
		QualityAttribute  AntiAliasQualityAttr( QualityThing );
		pRenderRegion->SetQuality( &AntiAliasQualityAttr, FALSE );
		//pRenderRegion->RRQuality.SetQuality( QUALITY_MAX );   // doesnt enable antialiasing
		

		// draw the background
		pRenderRegion->SetLineColour( kBackgoundOutline );
		if (manyDisabledControls == FALSE)
		{
			pRenderRegion->SetFillColour( kBackgound );
		}
		else
		{
			pRenderRegion->SetFillColour( kBackgoundMany );
		}
		pRenderRegion->DrawRect( &RenderRect );

		if (manyDisabledControls == FALSE)
		{
			// draw curve
			{
				// make a copy of the biasgain object with the right scaling to calculate
				// curve points for the drawing area
				CProfileBiasGain  DiagramMapper( CurveShape );
				DiagramMapper.SetIntervals( AFp( 0.0 ), AFp( kWidth ) );

				// create a path to hold the geometry of the curve
				Path  CurvePath;
				CurvePath.Initialise( kWidth );
				CurvePath.IsFilled  =  FALSE;
				CurvePath.FindStartOfPath();


				// make the first point on the curve
				static const DocCoord  StartPoint( 0, 0 );
				CurvePath.InsertMoveTo( StartPoint );

				// step through the x axis calculating points on the curve
				for( INT32 x = kStep  ;  x < kWidth  ;  x += kStep )
				{
					const AFp       CurvePointFp  =  DiagramMapper.MapInterval( AFp( x ) );
					const DocCoord  CurrentPoint( x, INT32( CurvePointFp + AFp(0.5) ) );

					CurvePath.InsertLineTo( CurrentPoint );
				}

				// connect the last looped step of the curve to the corner of the diagram
				static const DocCoord  EndPoint( kWidth, kWidth );
				CurvePath.InsertLineTo( EndPoint );


				// set some line drawing choices
				pRenderRegion->SetLineWidth( kLineWidth );
				pRenderRegion->SetLineColour( kCurve );

				// render the path of the curve
				pRenderRegion->DrawPath( &CurvePath );
			}
		}

		pRenderRegion->RestoreContext();
		DestroyGRenderRegion( pRenderRegion );
	}
}

//-------------------------------------------------------------------------------------------------
//	Author:			Chris Snook
//	Date:			17/1/2000
//	Purpose:		Draw an (interpolated) graph/curve illustration of the profile function
//	Preconditions:	ReDrawInfoType* ExtraInfo refers to a dlg picture control
//					CBiasGainDlg derives from DialogOp - so that CreateGRenderRegion() is accessible
//	Postconditions:	the dlg control is drawn into
//-------------------------------------------------------------------------------------------------

void  CBiasGainDlg::DrawCurveInterpolate ( const CProfileBiasGain& CurveShape,  ReDrawInfoType* ExtraInfo )
{
	// drawing constants:  overall resolution, curve steps, curve width, colors
	static const INT32  kWidth      =  ExtraInfo->dx;
	static const INT32  kBitWidth   =  kWidth/14;
	static const INT32  kHeight     =  ExtraInfo->dy;
	static const INT32  kHalfHeight =  kHeight/2;
	static const INT32  kQuartHeight = kHeight/4;
	static const INT32  kLineWidth  =  1500;
	static const INT32  kHalfLineWidth = kLineWidth/2;

	static const StockColour  kBackgoundOutline  =  COLOUR_NONE;
	static const StockColour  kBackgound         =  COLOUR_WHITE;
	static const StockColour  kBackgoundMany     =  COLOUR_LTGREY;	// for the MANY case
	static const StockColour  kCurve             =  COLOUR_BLACK;//COLOUR_BLUE;


	DocRect  RenderRect( 0, 0,  kWidth, kHeight );

	RenderRegion*  pRenderRegion  =  CreateGRenderRegion( &RenderRect, ExtraInfo );


	if( pRenderRegion != 0 )
	{
		pRenderRegion->SaveContext();

		// set drawing quality
		Quality           QualityThing( Quality::QualityMax );
		QualityAttribute  AntiAliasQualityAttr( QualityThing );
		pRenderRegion->SetQuality( &AntiAliasQualityAttr, FALSE );
		//pRenderRegion->RRQuality.SetQuality( QUALITY_MAX );   // doesnt enable antialiasing		

		// draw the background
		pRenderRegion->SetLineColour( kBackgoundOutline );
		if (manyDisabledControls == FALSE)
		{
			pRenderRegion->SetFillColour( kBackgound );
		}
		else
		{
			pRenderRegion->SetFillColour( kBackgoundMany );
		}
		pRenderRegion->DrawRect( &RenderRect );

		if (manyDisabledControls == FALSE)
		{
			// render ....

			static const AFp xOffset = (kHalfHeight - kQuartHeight) + kHalfLineWidth;

			CProfileBiasGain  DiagramMapper( CurveShape );
			DiagramMapper.SetIntervals( AFp( 0.0 ), AFp( kWidth - (2*xOffset)) );

			// create a path to hold the geometry of the curve
			Path  CurvePath;
			CurvePath.Initialise( kWidth );
			CurvePath.IsFilled  =  FALSE;
			CurvePath.FindStartOfPath();

			// common part of -1 to +1 curve ....

			//  |
			//  |

			static const DocCoord vc1 ( 0, kHalfHeight - kQuartHeight );
			static const DocCoord vc2 ( 0, kHalfHeight + kQuartHeight );

			static DocCoord  StartPoint ((INT32)xOffset, kHalfHeight - kQuartHeight);
			CurvePath.InsertMoveTo( StartPoint );
			static DocCoord  EndStartPoint ((INT32)xOffset, kHalfHeight + kQuartHeight);
			CurvePath.InsertLineTo( EndStartPoint );

			for (INT32 i = 1; i < 13/*19*/; i++)
			{	
				static DocCoord vi1;
				vi1 = vc1;
				vi1.x += (i) * kBitWidth;
				vi1.x = (INT32)(xOffset + DiagramMapper.MapInterval( AFp( vi1.x ) ) );
				CurvePath.InsertMoveTo( vi1 );
				static DocCoord vi2;
				vi2 = vc2;
				vi2.x += (i) * kBitWidth;
				vi2.x = (INT32)(xOffset + DiagramMapper.MapInterval( AFp( vi2.x ) ) );
				CurvePath.InsertLineTo( vi2 );
			}

			static DocCoord  StartEndPoint (kWidth - (INT32)xOffset, kHalfHeight - kQuartHeight);
			CurvePath.InsertMoveTo( StartEndPoint );
			static DocCoord  EndPoint (kWidth - (INT32)xOffset, kHalfHeight + kQuartHeight);
			CurvePath.InsertLineTo( EndPoint );

			// set some line drawing choices
			pRenderRegion->SetLineWidth( kLineWidth );
			pRenderRegion->SetLineColour( kCurve );

			// render the path of the curve
			pRenderRegion->DrawPath( &CurvePath );
		}

		pRenderRegion->RestoreContext();
		DestroyGRenderRegion( pRenderRegion );
	}
}



//-------------------------------------------------------------------------------------------------
//	Author:			Chris Snook
//	Date:			2/2/2000
//	Purpose:		Draw an (interpolated) fill illustration of the profile function
//	Preconditions:	ReDrawInfoType* ExtraInfo refers to a dlg picture control
//					CBiasGainDlg derives from DialogOp - so that CreateGRenderRegion() is accessible
//	Postconditions:	the dlg control is drawn into
//-------------------------------------------------------------------------------------------------

void  CBiasGainDlg::DrawFillInterpolate ( const CProfileBiasGain& CurveShape,  ReDrawInfoType* ExtraInfo )
{
	// drawing constants:  overall resolution, curve steps, curve width, colors
	static const INT32  kWidth      =  ExtraInfo->dx;
//	static const INT32  kBitWidth   =  kWidth/20;
	static const INT32  kHalfWidth  =  kWidth/2;
	static const INT32  kHeight     =  ExtraInfo->dy;
	static const INT32  kHalfHeight =  kHeight/2;
//	static const INT32  kQuartHeight = kHeight/4;
	static const INT32  kLineWidth  =  2000;
//	static const INT32  kHalfLineWidth = kLineWidth/2;

	static const StockColour  kBackgoundOutline  =  COLOUR_NONE;
	static const StockColour  kBackgound         =  COLOUR_WHITE;
	static const StockColour  kBackgoundMany     =  COLOUR_LTGREY;	// for the MANY case
//	static const StockColour  kCurve             =  COLOUR_BLUE;
	static /*const*/ StockColour  kStartColour   =  COLOUR_BLACK;
	static /*const*/ StockColour  kEndColour     =  COLOUR_WHITE;


	DocRect  RenderRect( 0, 0,  kWidth, kHeight );

	RenderRegion*  pRenderRegion  =  CreateGRenderRegion( &RenderRect, ExtraInfo );


	if( pRenderRegion != 0 )
	{
		pRenderRegion->SaveContext();

		// set drawing quality
		Quality           QualityThing( Quality::QualityMax );
		QualityAttribute  AntiAliasQualityAttr( QualityThing );
		pRenderRegion->SetQuality( &AntiAliasQualityAttr, FALSE );
		//pRenderRegion->RRQuality.SetQuality( QUALITY_MAX );   // doesnt enable antialiasing		

		// draw the background
		pRenderRegion->SetLineColour( kBackgoundOutline );
		if (manyDisabledControls == FALSE)
		{
			pRenderRegion->SetFillColour( kBackgound );
		}
		else
		{
			pRenderRegion->SetFillColour( kBackgoundMany );
		}
		pRenderRegion->DrawRect( &RenderRect );

		LinearFillAttribute DummyFillAttribute;//= new LinearFillAttribute ();

		if (manyDisabledControls == FALSE)
		{
			NodeRegularShape* DummyShape = new NodeRegularShape ();

			if (DummyShape != NULL)
			{
				DummyShape->SetUpShape ();
				DummyShape->MakeRectangle (kWidth, kHeight, 0);
				Matrix* TransformMatrix = new Matrix ();
				if (TransformMatrix != NULL)
				{
					TransformMatrix->SetTranslation (kHalfWidth, kHalfHeight);
					DummyShape->SetTransformMatrix (TransformMatrix);
					delete (TransformMatrix);
				}

				//JoinTypeAttribute DummyJoinType;		DummyJoinType.JoinType = MitreJoin;
				//AttrJoinType* DummyJoin = (AttrJoinType*) DummyJoinType.MakeNode ();
				//DummyJoin->AttachNode (DummyShape, FIRSTCHILD);

				// set up the start point ....
				
				DocCoord StartPoint (0, kHalfHeight);
				DummyFillAttribute.SetStartPoint (&StartPoint);
				
				DocColour* StartColour = pOwningGadget_m->GetStartColour ();
				DocColour vStartColour (kStartColour);
				if (StartColour != NULL) { DummyFillAttribute.SetStartColour (StartColour); }
				else { DummyFillAttribute.SetStartColour (&vStartColour); }
				
				// set up the end point ....
				
				DocCoord EndPoint (kWidth, kHalfHeight);
				DummyFillAttribute.SetEndPoint (&EndPoint);
				
				DocColour* EndColour = pOwningGadget_m->GetEndColour ();
				DocColour vEndColour (kEndColour);
				if (EndColour != NULL) { DummyFillAttribute.SetEndColour (EndColour); }
				else { DummyFillAttribute.SetEndColour (&vEndColour); }

				// set these guys to NULL ....
				
				DummyFillAttribute.SetEndPoint2 (NULL);
				DummyFillAttribute.SetEndPoint3 (NULL);
				
				CProfileBiasGain  DiagramMapper( CurveShape );
				DummyFillAttribute.SetProfile (DiagramMapper);

				pRenderRegion->SetFillGeometry((ColourFillAttribute*) &DummyFillAttribute, FALSE);			// Set Grad-filled

				// now lets set-up the fill effect (if there is one) ....
				
				if (pOwningGadget_m->GetFillEffect () != NULL)
				{
					pRenderRegion->SetFillEffect (pOwningGadget_m->GetFillEffect (), FALSE);
				}

				pRenderRegion->SetLineWidth( kLineWidth );
				//pRenderRegion->SetLineColour( kCurve );
				//pRenderRegion->SetFillColour (kCurve);

				DummyShape->Render (pRenderRegion);
				delete (DummyShape);
			}
		}

		pRenderRegion->RestoreContext();
		DestroyGRenderRegion( pRenderRegion );
	}
}

void  CBiasGainDlg::DrawTranspFillInterpolate ( const CProfileBiasGain& CurveShape,  ReDrawInfoType* ExtraInfo )
{
	// drawing constants:  overall resolution, curve steps, curve width, colors
	static const INT32  kWidth      =  ExtraInfo->dx;
//	static const INT32  kBitWidth   =  kWidth/20;
	static const INT32  kHalfWidth  =  kWidth/2;
	static const INT32  kHeight     =  ExtraInfo->dy;
	static const INT32  kHalfHeight =  kHeight/2;
//	static const INT32  kQuartHeight = kHeight/4;
	static const INT32  kLineWidth  =  2000;
//	static const INT32  kHalfLineWidth = kLineWidth/2;

	static const StockColour  kBackgoundOutline  =  COLOUR_NONE;
	static const StockColour  kBackgound         =  COLOUR_WHITE;
	static const StockColour  kBackgoundMany     =  COLOUR_LTGREY;	// for the MANY case
	static const StockColour  kCurve             =  COLOUR_WHITE;
	static const StockColour  kCurve2             =  COLOUR_BLUE;//COLOUR_BLACK;//COLOUR_MAGENTA;
//	static /*const*/ StockColour  kStartColour   =  COLOUR_BLACK;
//	static /*const*/ StockColour  kEndColour     =  COLOUR_WHITE;


	DocRect  RenderRect( 0, 0,  kWidth, kHeight );

	RenderRegion*  pRenderRegion  =  CreateGRenderRegion( &RenderRect, ExtraInfo );


	if( pRenderRegion != 0 )
	{
		pRenderRegion->SaveContext();

		// set drawing quality
		Quality           QualityThing( Quality::QualityMax );
		QualityAttribute  AntiAliasQualityAttr( QualityThing );
		pRenderRegion->SetQuality( &AntiAliasQualityAttr, FALSE );
		//pRenderRegion->RRQuality.SetQuality( QUALITY_MAX );   // doesnt enable antialiasing		

		// draw the background
		pRenderRegion->SetLineColour( kBackgoundOutline );
		if (manyDisabledControls == FALSE)
		{
			pRenderRegion->SetFillColour( kBackgound );
		}
		else
		{
			pRenderRegion->SetFillColour( kBackgoundMany );
		}
		pRenderRegion->DrawRect( &RenderRect );

		LinearTranspFillAttribute DummyFillAttribute;

		if (manyDisabledControls == FALSE)
		{
			NodeRegularShape* DummyShape = new NodeRegularShape ();

			if (DummyShape != NULL)
			{
				NodeRegularShape* DummyShape2 = new NodeRegularShape ();

				if (DummyShape2 != NULL)
				{
					DummyShape->SetUpShape ();
					DummyShape->MakeRectangle (kWidth, kHeight, 0);
					Matrix* TransformMatrix = new Matrix ();
					TransformMatrix->SetTranslation (kHalfWidth, kHalfHeight);
					DummyShape->SetTransformMatrix (TransformMatrix);
					delete (TransformMatrix);

					DummyShape2->SetUpShape ();
					DummyShape2->MakeRectangle (kWidth, kHalfHeight, 0);
					Matrix* TransformMatrix2 = new Matrix ();
					TransformMatrix2->SetTranslation (kHalfWidth, kHalfHeight);
					DummyShape2->SetTransformMatrix (TransformMatrix2);
					delete (TransformMatrix2);

					//JoinTypeAttribute DummyJoinType;		DummyJoinType.JoinType = MitreJoin;
					//AttrJoinType* DummyJoin = (AttrJoinType*) DummyJoinType.MakeNode ();
					//DummyJoin->AttachNode (DummyShape, FIRSTCHILD);

					DocCoord StartPoint (0, kHalfHeight);
					DummyFillAttribute.SetStartPoint (&StartPoint);
					DocCoord EndPoint (kWidth, kHalfHeight);
					DummyFillAttribute.SetEndPoint (&EndPoint);
					DummyFillAttribute.SetEndPoint2 (NULL);
					DummyFillAttribute.SetEndPoint3 (NULL);
					
					// set up the start transparency ....
					
					UINT32* StartVal = pOwningGadget_m->GetStartTransp ();
					UINT32 vStartVal (0);
					if (StartVal != NULL) { DummyFillAttribute.SetStartTransp (StartVal); }
					else { DummyFillAttribute.SetStartTransp (&vStartVal); }

					// set up the end transparency ....
					
					UINT32* EndVal = pOwningGadget_m->GetEndTransp ();
					UINT32 vEndVal (255);
					if (EndVal != NULL) { DummyFillAttribute.SetEndTransp (EndVal); }
					else { DummyFillAttribute.SetEndTransp (&vEndVal); }

					// and set these guys to NULL ....
					
					DummyFillAttribute.SetEndTransp2 (NULL);
					DummyFillAttribute.SetEndTransp3 (NULL);
				
					CProfileBiasGain  DiagramMapper( CurveShape );
					DummyFillAttribute.SetProfile (DiagramMapper);

					pRenderRegion->SetLineWidth( kLineWidth );
					//pRenderRegion->SetLineColour( kCurve );
					pRenderRegion->SetFillColour (kCurve2);

					DummyShape2->Render (pRenderRegion);

					pRenderRegion->SetTranspFillGeometry((TranspFillAttribute*) &DummyFillAttribute, FALSE);			// Set Grad-filled

					pRenderRegion->SetFillColour (kCurve);
					DummyShape->Render (pRenderRegion);

					delete (DummyShape2);
					delete (DummyShape);
				}
			}
		}

		pRenderRegion->RestoreContext();
		DestroyGRenderRegion( pRenderRegion );
	}
}




/****************************************************************************************
Function  : CBiasGainDlg::InitSliders
Author    : Mikhail Tatarnikov
Purpose   : Initializes and updates the sliders
Returns   : void
Exceptions: 
Parameters: [in] CProfileBiasGain const& BiasGain - a value to update sliders wiht.
Notes     : 
****************************************************************************************/
void CBiasGainDlg::InitSliders(CProfileBiasGain const& BiasGain)
{

	InitOneSlider( _R(IDC_SLIDER_BIAS) );
	InitOneSlider( _R(IDC_SLIDER_GAIN) );

	WriteSliders( BiasGain );

}

/****************************************************************************************
Function  : CBiasGainDlg::InitiProfileCombobox
Author    : Mikhail Tatarnikov
Purpose   : Initializes the profile combobox
Returns   : void
Exceptions: 
Parameters: None
Notes     : 
****************************************************************************************/
void CBiasGainDlg::InitiProfileCombobox()
{
	// Check if we have already been initialized.
	if (m_pobddStandardProfile)
	{
		delete m_pobddStandardProfile;
		m_pobddStandardProfile = NULL;
	}

	m_pobddStandardProfile = new CBitmapGridDropDown;
	m_pobddStandardProfile->Init(WindowID, _R(IDC_BIASGAINCOMBO));
	
	m_pobddStandardProfile->SetColumns(1);
	m_pobddStandardProfile->SetItemSize(wxSize(21, 21));
	
	m_pobddStandardProfile->AddItem(_R(IDB_PROFILE1));
	m_pobddStandardProfile->AddItem(_R(IDB_PROFILE2));
	m_pobddStandardProfile->AddItem(_R(IDB_PROFILE3));
	m_pobddStandardProfile->AddItem(_R(IDB_PROFILE4));
	m_pobddStandardProfile->AddItem(_R(IDB_PROFILE5));
	
	m_pobddStandardProfile->SetUnselectedItem(_R(IDB_PROFILE6));
}


/****************************************************************************************
Function  : CBiasGainDlg::InitBiasGainGadget
Author    : Mikhail Tatarnikov
Purpose   : Updates profile combobox with a value.
Returns   : void
Exceptions: 
Parameters: [in] CProfileBiasGain const& BiasGain - the value to initialize with
Notes     : 
****************************************************************************************/
void CBiasGainDlg::InitBiasGainGadget(CProfileBiasGain const& BiasGain)
{
	INT32 iProfileIndex = BiasGainGadget.FindPresetBiasGain(BiasGain);
	
	m_pobddStandardProfile->SetSelected(iProfileIndex);
}


/****************************************************************************************
Function  : CBiasGainDlg::WriteSliders
Author    : Mikhail Tatarnikov
Purpose   : Updates slider to display the supplied bias/gain value
Returns   : void
Exceptions: 
Parameters: [in] CProfileBiasGain const& BiasGain - the value to initialize sliders with.
Notes     : 
****************************************************************************************/
void CBiasGainDlg::WriteSliders(CProfileBiasGain const& BiasGain)
{

	WriteOneSlider( BiasGain.GetBias(), _R(IDC_SLIDER_BIAS) );

	WriteOneSlider( BiasGain.GetGain(), _R(IDC_SLIDER_GAIN) );

}


/****************************************************************************************
Function  : CBiasGainDlg::InvokeVia
Author    : Mikhail Tatarnikov
Purpose   : Invoke the bias/gain dialog
Returns   : void
Exceptions: 
Parameters: [in] CBiasGainGadget&  pInvokeWith - the parent control (used for routing messages);
            [in] CProfileBiasGain* pInvokeOn   - an initial value to display;
            [in] BOOL			   bMany	   - 
Notes     : 
****************************************************************************************/
void CBiasGainDlg::InvokeVia(CBiasGainGadget& pInvokeWith, CProfileBiasGain* pInvokeOn, BOOL bMany)
{
	OpDescriptor*  pOpDescriptor = OpDescriptor::FindOpDescriptor( OPTOKEN_BIASGAIN_DLG );
	if( pOpDescriptor != 0 )
	{
		if (pInvokeOn != NULL)
		{
			OpParam  Param( reinterpret_cast<void*>( &pInvokeWith ),  reinterpret_cast<void*>( pInvokeOn ) );
			DoWithParam( pOpDescriptor, &Param );
		}
		else if (bMany == TRUE)					// we need to display the many setting ....
		{
			OpParam  Param( reinterpret_cast<void*>( &pInvokeWith ),  reinterpret_cast<void*>( pInvokeOn ) );
			DoWithParam( pOpDescriptor, &Param );
		}
		else	// I'm not sure if this is ever called - BUT we'd best provide a 'default' anyway
		{
			CProfileBiasGain defaultInvokeOn;

			OpParam  Param( reinterpret_cast<void*>( &pInvokeWith ),  reinterpret_cast<void*>( &defaultInvokeOn ) );
			DoWithParam( pOpDescriptor, &Param );
		}
	}
}



/****************************************************************************************
Function  : CBiasGainDlg::ReInitialiseDialog
Author    : Mikhail Tatarnikov
Purpose   : Initializes the dialog with new bias/gain value
Returns   : void
Exceptions: 
Parameters: [in] CProfileBiasGain* ReInitOn - a bias/gain value to initialize with;
            [in] BOOL			   bMany	- 
Notes     : 
****************************************************************************************/
void CBiasGainDlg::ReInitialiseDialog(CProfileBiasGain* ReInitOn, BOOL bMany)
{
	if (ReInitOn != NULL)					// woopey!!!!  we have a profile - lets display it ....
	{
		BiasGain_m = *ReInitOn;				// this line does everything for us!

		// the rest is just tidying up ....

		if (manyDisabledControls == TRUE)
		{
			EnableAllControls ();
		}
	
		// reinitialise gadgets ....
		InitSliders( BiasGain_m );
		InitEditBoxes( BiasGain_m );
		InitiProfileCombobox();
		InitBiasGainGadget ( BiasGain_m );

		// redraw profile ....
		InvalidateGadget( _R(IDC_CURVE) );
		InvalidateGadget( _R(IDC_CURVEINTERPOLATE) );
	}
	else if (bMany == TRUE)					// we need to display the many setting ....
	{
//		SetCustomComboGadgetValue ( _R(IDC_BIASGAINCOMBO), (CustomComboBoxControlDataItem*) NULL, TRUE, -1);
		m_pobddStandardProfile->SetSelected(-1);
		
		// and disable all other controls ....
		DisableAllControls ();
	}
}


/****************************************************************************************
Function  : CBiasGainDlg::DisableAllControls
Author    : Mikhail Tatarnikov
Purpose   : Disable all dialog controls.
Returns   : void
Exceptions: 
Parameters: None
Notes     : 
****************************************************************************************/
void CBiasGainDlg::DisableAllControls()
{
	manyDisabledControls = TRUE;

	EnableGadget (_R(IDC_STATIC1), FALSE);
	EnableGadget (_R(IDC_STATIC2), FALSE);
	EnableGadget (_R(IDC_STATIC3), FALSE);
	EnableGadget (_R(IDC_STATIC4), FALSE);
	EnableGadget (_R(IDC_STATIC5), FALSE);
	EnableGadget (_R(IDC_STATIC6), FALSE);
	//EnableGadget (_R(IDC_STATIC7), FALSE);
	EnableGadget (_R(IDC_STATIC8), FALSE);
	EnableGadget (_R(IDC_STATIC9), FALSE);
	EnableGadget (_R(IDC_SLIDER_BIAS), FALSE);
	EnableGadget (_R(IDC_SLIDER_GAIN), FALSE);
	EnableGadget (_R(IDC_EDIT_BIAS), FALSE);
	EnableGadget (_R(IDC_EDIT_GAIN), FALSE);
	//EnableGadget (_R(IDC_CURVE), FALSE);
	InvalidateGadget( _R(IDC_CURVE) );
	InvalidateGadget( _R(IDC_CURVEINTERPOLATE) );
}



/****************************************************************************************
Function  : CBiasGainDlg::EnableAllControls
Author    : Mikhail Tatarnikov
Purpose   : Enable all dialog controls
Returns   : void
Exceptions: 
Parameters: None
Notes     : 
****************************************************************************************/
void CBiasGainDlg::EnableAllControls()
{
	manyDisabledControls = FALSE;

	EnableGadget (_R(IDC_STATIC1), TRUE);
	EnableGadget (_R(IDC_STATIC2), TRUE);
	EnableGadget (_R(IDC_STATIC3), TRUE);
	EnableGadget (_R(IDC_STATIC4), TRUE);
	EnableGadget (_R(IDC_STATIC5), TRUE);
	EnableGadget (_R(IDC_STATIC6), TRUE);
	//EnableGadget (_R(IDC_STATIC7), TRUE);
	EnableGadget (_R(IDC_STATIC8), TRUE);
	EnableGadget (_R(IDC_STATIC9), TRUE);
	EnableGadget (_R(IDC_SLIDER_BIAS), TRUE);
	EnableGadget (_R(IDC_SLIDER_GAIN), TRUE);
	EnableGadget (_R(IDC_EDIT_BIAS), TRUE);
	EnableGadget (_R(IDC_EDIT_GAIN), TRUE);
	//EnableGadget (_R(IDC_CURVE), TRUE);
	InvalidateGadget( _R(IDC_CURVE) );
	InvalidateGadget( _R(IDC_CURVEINTERPOLATE) );
}


/****************************************************************************************
Function  : CBiasGainDlg::GetCurrentDialogProfile
Author    : Mikhail Tatarnikov
Purpose   : Return the current biasgain profile
Returns   : CProfileBiasGain - the current bias/gain value.
Exceptions: 
Parameters: None
Notes     : 
****************************************************************************************/
CProfileBiasGain CBiasGainDlg::GetCurrentDialogProfile()
{
	return (BiasGain_m);
}



/****************************************************************************************
Function  : CBiasGainDlg::ReadSliders
Author    : Mikhail Tatarnikov
Purpose   : Reads slider values
Returns   : void
Exceptions: 
Parameters: [out] CProfileBiasGain& BiasGain - the bias/gain value to read from sliders to.
Notes     : 
****************************************************************************************/
void CBiasGainDlg::ReadSliders(CProfileBiasGain& BiasGain)
{
	BiasGain.SetBias( ReadOneSlider( _R(IDC_SLIDER_BIAS) ) );
	BiasGain.SetGain( ReadOneSlider( _R(IDC_SLIDER_GAIN) ) );
}



/****************************************************************************************
Function  : CBiasGainDlg::InitOneSlider
Author    : Mikhail Tatarnikov
Purpose   : Initialize a slider
Returns   : void
Exceptions: 
Parameters: [in] CGadgetID GadgetID - a slider to initialize.
Notes     : 
****************************************************************************************/
void CBiasGainDlg::InitOneSlider(CGadgetID GadgetID)
{

	SetGadgetRange( GadgetID, kSliderMin_s, kSliderMax_s );
	SetGadgetBitmaps( GadgetID, _R(IDB_SLIDERBASE), _R(IDB_SLIDERSLIDER) );

}

/****************************************************************************************
Function  : CBiasGainDlg::WriteOneSlider
Author    : Mikhail Tatarnikov
Purpose   : Set a value to a slider.
Returns   : void
Exceptions: 
Parameters: [in] AFp	   ValueMinus1ToPlus1 - a new slider value;
            [in] CGadgetID GadgetID			  - a slider to update.
Notes     : 
****************************************************************************************/
void CBiasGainDlg::WriteOneSlider(AFp ValueMinus1ToPlus1, CGadgetID GadgetID)
{
	const INT32  SliderValue  =  INT32( ValueMinus1ToPlus1 * AFp( kSliderHalfLength_s ) + AFp(0.5) );
	SetLongGadgetValue( GadgetID,  SliderValue );
}

/****************************************************************************************
Function  : CBiasGainDlg::ReadOneSlider
Author    : Mikhail Tatarnikov
Purpose   : Reads the value from a slider
Returns   : AFp - the slider value.
Exceptions: 
Parameters: [in] CGadgetID GadgetID - the slider to read from.
Notes     : Return value is in the range [-1,+1]
****************************************************************************************/
AFp CBiasGainDlg::ReadOneSlider(CGadgetID GadgetID)
{

	const INT32 iSliderValue	   =  GetLongGadgetValue( GadgetID, kSliderMin_s, kSliderMax_s );
	const AFp   ValueMinus1ToPlus1 =  AFp(iSliderValue) / AFp(kSliderHalfLength_s);

	return  ValueMinus1ToPlus1;

}




/****************************************************************************************
Function  : CBiasGainDlg::InitEditBoxes
Author    : Mikhail Tatarnikov
Purpose   : initialise the dialog box edit controls, including setting values
Returns   : void
Exceptions: 
Parameters: [in] CProfileBiasGain const& BiasGain - the value to initalize the edit boxes with.
Notes     : 
****************************************************************************************/
void CBiasGainDlg::InitEditBoxes(CProfileBiasGain const& BiasGain)
{
	WriteEditBoxes( BiasGain );
}


/****************************************************************************************
Function  : CBiasGainDlg::WriteEditBoxes
Author    : Mikhail Tatarnikov
Purpose   : Update the edit boxes
Returns   : void
Exceptions: 
Parameters: [in] CProfileBiasGain const& BiasGain - the value to update the edit boxes with.
Notes     : 
****************************************************************************************/
void CBiasGainDlg::WriteEditBoxes(CProfileBiasGain const& BiasGain)
{
	const AFp  BiasMinus1ToPlus1  =  BiasGain.GetBias();
	SetDoubleGadgetValue( _R(IDC_EDIT_BIAS),  BiasMinus1ToPlus1 );

	const AFp  GainMinus1ToPlus1  =  BiasGain.GetGain();
	SetDoubleGadgetValue( _R(IDC_EDIT_GAIN),  GainMinus1ToPlus1 );
}


/****************************************************************************************
Function  : CBiasGainDlg::ReadEditBoxes
Author    : Mikhail Tatarnikov
Purpose   : Reads the bias/gain value from the edit box controls
Returns   : void
Exceptions: 
Parameters: [out] CProfileBiasGain& BiasGain - the value read.
Notes     : 
****************************************************************************************/
void CBiasGainDlg::ReadEditBoxes(CProfileBiasGain& BiasGain)
{

	const AFp  BiasMinus1ToPlus1  =  AFp( GetDoubleGadgetValue( _R(IDC_EDIT_BIAS), double(-1.0), double(+1.0) ) );
	BiasGain.SetBias( BiasMinus1ToPlus1 );

	const AFp  GainMinus1ToPlus1  =  AFp( GetDoubleGadgetValue( _R(IDC_EDIT_GAIN), double(-1.0), double(+1.0) ) );
	BiasGain.SetGain( GainMinus1ToPlus1 );

	// write the clamped-into-range values back into the edit boxes
	WriteEditBoxes( BiasGain );

}







/****************************************************************************************
Function  : CBiasGainDlg::HandleSliderPosChanging
Author    : Mikhail Tatarnikov
Purpose   : Handles slider DIM_SLIDER_POS_CHANGING messages
Returns   : void
Exceptions: 
Parameters: [in] CDlgMessage const& Message  - a message from edit box controls;
            [in] CProfileBiasGain&  BiasGain - the current bias/gain value.
Notes     : 
****************************************************************************************/
void  CBiasGainDlg::HandleSliderPosChanging(CDlgMessage const& Message, CProfileBiasGain& BiasGain)
{

	// read values from sliders (both (simplifies code)) into biasgain object
	ReadSliders( BiasGain );

	// make the edit boxes agree with the sliders
	WriteEditBoxes( BiasGain );

	// Make the profile control show a custom image (cause after all, now we have
	// a custom bias/gain).
	
	m_pobddStandardProfile->SetSelected(-1);

	// do general input handling
	HandleInput( Message,  BiasGain );
}


/****************************************************************************************
Function  : CBiasGainDlg::HandleSliderPosSet
Author    : Mikhail Tatarnikov
Purpose   : Handles slider DIM_SLIDER_POS_SET messages
Returns   : void
Exceptions: 
Parameters: [in] CDlgMessage const& Message  - a message from edit box controls;
            [in] CProfileBiasGain&  BiasGain - the current bias/gain value.
Notes     : 
****************************************************************************************/
void  CBiasGainDlg::HandleSliderPosSet(CDlgMessage const& Message, CProfileBiasGain& BiasGain)
{
	// do the same as slider changing
	HandleSliderPosChanging( Message,  BiasGain );
}


/****************************************************************************************
Function  : CBiasGainDlg::HandleCommit
Author    : Mikhail Tatarnikov
Purpose   : Handles DIM_COMMIT message from the dialog's edit boxes
Returns   : void
Exceptions: 
Parameters: [in] CDlgMessage const& Message  - a message from edit box controls;
            [in] CProfileBiasGain&  BiasGain - the current bias/gain value.
Notes     : 
****************************************************************************************/
void CBiasGainDlg::HandleCommit(CDlgMessage const& Message, CProfileBiasGain& BiasGain)
{
	// read values from edit boxes (both (simplifies code)) into biasgain object
	ReadEditBoxes( BiasGain_m );

	// make the sliders agree with the edit boxes
	WriteSliders( BiasGain_m );

	// do general input handling
	HandleInput( Message,  BiasGain );

}


/****************************************************************************************
Function  : CBiasGainDlg::HandleInput
Author    : Mikhail Tatarnikov
Purpose   : Handles a message from the dialog controls.
Returns   : void
Exceptions: 
Parameters: [in] CDlgMessage const& Message  - an input message to handle;
            [in] CProfileBiasGain&  BiasGain - the current bias/gain value.
Notes     : 
****************************************************************************************/
void CBiasGainDlg::HandleInput(CDlgMessage const& Message, CProfileBiasGain& BiasGain)
{

	// prompt a diagram redraw to illustrate the new values
	InvalidateGadget( _R(IDC_CURVE) );
	InvalidateGadget( _R(IDC_CURVEINTERPOLATE) );

	// Broadcast a special profile change message, allowing all interested parties
	// to handle it. Depending on the input type we send either interactive or final change.
	if (Message == DIM_SLIDER_POS_CHANGING)
		BroadcastMessage(DIM_PROFILE_CHANGING,  BiasGain_m);
	else if (Message == DIM_SLIDER_POS_IDLE)
		BroadcastMessage(DIM_PROFILE_CHANGEIDLE,  BiasGain_m);
	else
		BroadcastMessage(DIM_PROFILE_CHANGED,  BiasGain_m);

}


/****************************************************************************************
Function  : CBiasGainDlg::BroadcastMessage
Author    : Mikhail Tatarnikov
Purpose   : Broadcasts a message to all DialogOp-derived classes, sending the current profile as parameter
Returns   : void
Exceptions: 
Parameters: [in] CDlgMessage const&		 Message  - a message to broadcast;
            [in] CProfileBiasGain const& BiasGain - the profile value to send with the message.
Notes     : 
****************************************************************************************/
void CBiasGainDlg::BroadcastMessage(CDlgMessage const& Message, CProfileBiasGain const& BiasGain) const
{
	BROADCAST_TO_CLASS(DialogMsg(0, Message, Id_m, reinterpret_cast<UINT_PTR>(&BiasGain)), DialogOp);
}

