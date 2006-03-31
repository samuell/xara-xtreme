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

#include "bars.h"
#include "msg.h"




CC_IMPLEMENT_DYNCREATE( CBiasGainDlg,  DialogOp )


#define new CAM_DEBUG_NEW




/// statics ///

const	UINT32		CBiasGainDlg::IDD   =  _R(IDD_BIASGAIN);
const	CDlgMode	CBiasGainDlg::Mode  =  MODELESS;

const	INT32		CBiasGainDlg::kSliderLength_s      =  INT32( 100 );
const	INT32		CBiasGainDlg::kSliderHalfLength_s  =  kSliderLength_s / INT32( 2 );
const	INT32		CBiasGainDlg::kSliderMin_s         =  -( kSliderHalfLength_s );
const	INT32		CBiasGainDlg::kSliderMax_s         =  +( kSliderHalfLength_s );






CBiasGainDlg::CBiasGainDlg ()
	: DialogOp( CBiasGainDlg::IDD, CBiasGainDlg::Mode ),
	  pOwningGadget_m( 0 ),
	  Id_m( 0 ),
	  BiasGain_m()
{
}


CBiasGainDlg::~CBiasGainDlg ()
{
	//EndTimedProcessing();
}


/*CBiasGainDlg::CBiasGainDlg ( const CBiasGainDlg& )
{

}


CBiasGainDlg&  CBiasGainDlg::operator= ( const CBiasGainDlg& )
{

}*/


/******************************************************************************
>	BOOL CBiasGainDlg::Init()

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/9/94
	Returns:	FALSE if it fails (due to lack of memory)
	Purpose:	Creates an OpDescriptor for a CBiasGainDlg dialog
******************************************************************************/

BOOL  CBiasGainDlg::Init ()
{
	/*// init array holding coords of diagram rects on dialog in all alignments
	CBiasGainDlg::CalcDiagramRectsOneAxis(CBiasGainDlg::DiagRectX,DiagRectWidth,
		DiagRectOrderX,DiagRectGapX,DiagWidth-1);
	CBiasGainDlg::CalcDiagramRectsOneAxis(CBiasGainDlg::DiagRectY,DiagRectHeight,
		DiagRectOrderY,DiagRectGapY,DiagHeight-1);*/

	return  RegisterOpDescriptor(
		0,									// Tool ID
 		_R(IDS_BIASGAINDLG),					// String resource ID
		CC_RUNTIME_CLASS( CBiasGainDlg ),	// Runtime class
 		OPTOKEN_BIASGAIN_DLG,				// Token string
 		CBiasGainDlg::GetState,				// GetState function
		0,									// Help ID
		_R(IDBBL_BIASGAIN),						// Bubble ID
		_R(IDD_BARCONTROLSTORE),				// Resource ID
		_R(IDC_BIASGAIN),						// Control ID
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

OpState	CBiasGainDlg::GetState ( String_256* pHelpString,  OpDescriptor* )
{

	static OpState  DialogState;
	DialogState.Greyed = FALSE;
	/*DialogState.Greyed=GetApplication()->FindSelection()->Count()==0;
	if (DialogState.Greyed)
		pHelpString->MakeMsg(_R(IDS_CBiasGainDlg_GREY));*/
	return  DialogState;

}


/******************************************************************************
>	void CBiasGainDlg::Do(OpDescriptor*)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/9/94 
	Purpose:	Creates and shows a CBiasGainDlg dialog
******************************************************************************/

void  CBiasGainDlg::Do ( OpDescriptor* )
{

	Create();
	Open();

}


///-------------------------------------------------------------------------------------------------
///	Author:			Harrison Ainsworth
///	Date:			07/99
///	Purpose:		Creates and shows a CBiasGainDlg dialog
///	Preconditions:	OpParam* OwningGadgetParams param1 is really a CBiasGainGadget* and
///					param2 is really a CProfileBiasGain*
///-------------------------------------------------------------------------------------------------

void  CBiasGainDlg::DoWithParam ( OpDescriptor*,  OpParam* OwningGadgetParams )
{
	BOOL noProfile = FALSE;

	/// extract the param info: an owning gadget ptr, and input biasgain value
	{
		pOwningGadget_m  =  reinterpret_cast<CBiasGainGadget*>( OwningGadgetParams->Param1 );
		pOwningGadget_m->SetCBaisGainDlg (this);
		
		const CProfileBiasGain* invokeOn = reinterpret_cast<CProfileBiasGain const*>( OwningGadgetParams->Param2 );
		
		if (invokeOn != NULL)
		{
			BiasGain_m = *(invokeOn);
		}
		else
		{
			noProfile = TRUE;
		}
	}

	/// get the ID
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


	/// set the dialog title
	{
		if( pOwningGadget_m != 0 )
		{
			String_256  Title;
			pOwningGadget_m->GetDialogTitle( Title );
			DialogManager::SetTitlebarName( WindowID,  &Title );
		}

	}

	if (noProfile == TRUE)
	{
		SetCustomComboGadgetValue ( _R(IDC_BIASGAIN), (CustomComboBoxControlDataItem*) NULL, TRUE, -1);

		// and disable all other controls ....

		DisableAllControls ();
	}

	/// tell the owning gadget that this dialog has opened
	if( pOwningGadget_m != 0 )
	{
		pOwningGadget_m->DialogHasOpened();
	}

}


///-------------------------------------------------------------------------------------------------
///	Author:			Harrison Ainsworth
///	Date:			07/99
///	Purpose:		handle messages for this dialog - including create/cancel and slider and edit 
///					interaction
///	Preconditions:	pMessage is valid
///-------------------------------------------------------------------------------------------------

MsgResult  CBiasGainDlg::Message ( Msg* Message )
{

	if( IS_OUR_DIALOG_MSG( Message ) )
	{
		DialogMsg* Msg  =  static_cast<DialogMsg*>( Message );//dynamic_cast<DialogMsg*>( Message );   /// needs rtti enabled

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
				
				/// initialise gadgets
				InitSliders( BiasGain_m );
				InitEditBoxes( BiasGain_m );

				// just for testing ....
				BiasGainGadget.Init (this, _R(IDC_BIASGAIN), _R(IDBBL_BIASGAIN), _R(IDS_BIASGAIN));
				EnableGadget (_R(IDC_BIASGAIN), TRUE);

				InitBiasGainGadget ( BiasGain_m );

				InvalidateGadget( _R(IDC_CURVE) );
				InvalidateGadget( _R(IDC_CURVEINTERPOLATE) );

				DialogManager::DefaultKeyboardFocus();

				break;
			}

			case DIM_CANCEL :
			{
				if (!(IsCustomComboDropdownVisible (_R(IDC_BIASGAIN))))
				{
					Close();
					//End();
					DlgMgr->Delete( WindowID, this );

					/// tell the owning gadget that this dialog has closed
					if( pOwningGadget_m != 0 )
					{
						pOwningGadget_m->DialogHasClosed();
					}
				}
				else
				{
					CloseDropdown (_R(IDC_BIASGAIN), FALSE);
				}
				break;
			}

			case DIM_LFT_BN_UP:
				DialogManager::DefaultKeyboardFocus();
			break;

			/// slider interaction messages
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

			/// edit box interaction messages
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
				if (Msg->GadgetID == _R(IDC_BIASGAIN))
				{
					WORD  DropdownListIndex;
					GetValueIndex( _R(IDC_BIASGAIN), &DropdownListIndex );

					switch( DropdownListIndex )
					{
						/// get preset value to match the dropdown list index
						///
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
							
							BiasGainGadget.GetPresetBiasGainValue( DropdownListIndex, BiasGain_m );
							InitSliders( BiasGain_m );
							InitEditBoxes( BiasGain_m );
							InvalidateGadget( _R(IDC_CURVE) );
							InvalidateGadget( _R(IDC_CURVEINTERPOLATE) );
							
							// and then update all the other stuff in the document

							BiasGainGadget.SetInfobarGadgetID (pOwningGadget_m->GetGadgetID ());
							BiasGainGadget.Message (Msg);//pOwningGadget_m->Message (Msg);
						}
					}
					DialogManager::DefaultKeyboardFocus();
				}
			}
		}
	}
	/*else if( MESSAGE_IS_A( Message, SelChangingMsg ) )
	{
		// if selection has changed, update state of dialog
		if ( ((SelChangingMsg*)Message)->State == SelChangingMsg::SELECTIONCHANGED )
		{
			UpdateState();
		}
	}*/


	// Pass all unhandled messages to base class for default processing!
	return  DialogOp::Message( Message );

}  




/// protected //////////////////////////////////////////////////////////////////////////////////////

///-------------------------------------------------------------------------------------------------
///	Author:			Harrison Ainsworth
///	Date:			07/99
///	Purpose:		Draw graph/curve illustration of the profile function
///	Preconditions:	ReDrawInfoType* ExtraInfo somehow (mystery to me) refers to a dlg picture control
///					CBiasGainDlg derives from DialogOp - so that CreateGRenderRegion() is accessible
///	Postconditions:	the dlg control is drawn into
///-------------------------------------------------------------------------------------------------

void  CBiasGainDlg::DrawCurve ( const CProfileBiasGain& CurveShape,  ReDrawInfoType* ExtraInfo )
{
	/// drawing constants:  overall resolution, curve steps, curve width, colors
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

		/// set drawing quality
		Quality           QualityThing( Quality::QualityMax );
		QualityAttribute  AntiAliasQualityAttr( QualityThing );
		pRenderRegion->SetQuality( &AntiAliasQualityAttr, FALSE );
		//pRenderRegion->RRQuality.SetQuality( QUALITY_MAX );   /// doesnt enable antialiasing
		

		/// draw the background
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
			/// draw curve
			{
				/// make a copy of the biasgain object with the right scaling to calculate
				/// curve points for the drawing area
				CProfileBiasGain  DiagramMapper( CurveShape );
				DiagramMapper.SetIntervals( AFp( 0.0 ), AFp( kWidth ) );

				/// create a path to hold the geometry of the curve
				Path  CurvePath;
				CurvePath.Initialise( kWidth );
				CurvePath.IsFilled  =  FALSE;
				CurvePath.FindStartOfPath();


				/// make the first point on the curve
				static const DocCoord  StartPoint( 0, 0 );
				CurvePath.InsertMoveTo( StartPoint );

				/// step through the x axis calculating points on the curve
				for( INT32 x = kStep  ;  x < kWidth  ;  x += kStep )
				{
					const AFp       CurvePointFp  =  DiagramMapper.MapInterval( AFp( x ) );
					const DocCoord  CurrentPoint( x, INT32( CurvePointFp + AFp(0.5) ) );

					CurvePath.InsertLineTo( CurrentPoint );
				}

				/// connect the last looped step of the curve to the corner of the diagram
				static const DocCoord  EndPoint( kWidth, kWidth );
				CurvePath.InsertLineTo( EndPoint );


				/// set some line drawing choices
				pRenderRegion->SetLineWidth( kLineWidth );
				pRenderRegion->SetLineColour( kCurve );

				/// render the path of the curve
				pRenderRegion->DrawPath( &CurvePath );
			}
		}

		pRenderRegion->RestoreContext();
		DestroyGRenderRegion( pRenderRegion );
	}
}

///-------------------------------------------------------------------------------------------------
///	Author:			Chris Snook
///	Date:			17/1/2000
///	Purpose:		Draw an (interpolated) graph/curve illustration of the profile function
///	Preconditions:	ReDrawInfoType* ExtraInfo refers to a dlg picture control
///					CBiasGainDlg derives from DialogOp - so that CreateGRenderRegion() is accessible
///	Postconditions:	the dlg control is drawn into
///-------------------------------------------------------------------------------------------------

void  CBiasGainDlg::DrawCurveInterpolate ( const CProfileBiasGain& CurveShape,  ReDrawInfoType* ExtraInfo )
{
	/// drawing constants:  overall resolution, curve steps, curve width, colors
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

		/// set drawing quality
		Quality           QualityThing( Quality::QualityMax );
		QualityAttribute  AntiAliasQualityAttr( QualityThing );
		pRenderRegion->SetQuality( &AntiAliasQualityAttr, FALSE );
		//pRenderRegion->RRQuality.SetQuality( QUALITY_MAX );   /// doesnt enable antialiasing		

		/// draw the background
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

			static const xOffset = (kHalfHeight - kQuartHeight) + kHalfLineWidth;

			CProfileBiasGain  DiagramMapper( CurveShape );
			DiagramMapper.SetIntervals( AFp( 0.0 ), AFp( kWidth - (2*xOffset)) );

			/// create a path to hold the geometry of the curve
			Path  CurvePath;
			CurvePath.Initialise( kWidth );
			CurvePath.IsFilled  =  FALSE;
			CurvePath.FindStartOfPath();

			// common part of -1 to +1 curve ....

			//  |
			//  |

			static const DocCoord vc1 ( 0, kHalfHeight - kQuartHeight );
			static const DocCoord vc2 ( 0, kHalfHeight + kQuartHeight );

			static DocCoord  StartPoint (xOffset, kHalfHeight - kQuartHeight);
			CurvePath.InsertMoveTo( StartPoint );
			static DocCoord  EndStartPoint (xOffset, kHalfHeight + kQuartHeight);
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

			static DocCoord  StartEndPoint (kWidth - xOffset, kHalfHeight - kQuartHeight);
			CurvePath.InsertMoveTo( StartEndPoint );
			static DocCoord  EndPoint (kWidth - xOffset, kHalfHeight + kQuartHeight);
			CurvePath.InsertLineTo( EndPoint );

			/// set some line drawing choices
			pRenderRegion->SetLineWidth( kLineWidth );
			pRenderRegion->SetLineColour( kCurve );

			/// render the path of the curve
			pRenderRegion->DrawPath( &CurvePath );
		}

		pRenderRegion->RestoreContext();
		DestroyGRenderRegion( pRenderRegion );
	}
}



///-------------------------------------------------------------------------------------------------
///	Author:			Chris Snook
///	Date:			2/2/2000
///	Purpose:		Draw an (interpolated) fill illustration of the profile function
///	Preconditions:	ReDrawInfoType* ExtraInfo refers to a dlg picture control
///					CBiasGainDlg derives from DialogOp - so that CreateGRenderRegion() is accessible
///	Postconditions:	the dlg control is drawn into
///-------------------------------------------------------------------------------------------------

void  CBiasGainDlg::DrawFillInterpolate ( const CProfileBiasGain& CurveShape,  ReDrawInfoType* ExtraInfo )
{
	/// drawing constants:  overall resolution, curve steps, curve width, colors
	static const INT32  kWidth      =  ExtraInfo->dx;
	static const INT32  kBitWidth   =  kWidth/20;
	static const INT32  kHalfWidth  =  kWidth/2;
	static const INT32  kHeight     =  ExtraInfo->dy;
	static const INT32  kHalfHeight =  kHeight/2;
	static const INT32  kQuartHeight = kHeight/4;
	static const INT32  kLineWidth  =  2000;
	static const INT32  kHalfLineWidth = kLineWidth/2;

	static const StockColour  kBackgoundOutline  =  COLOUR_NONE;
	static const StockColour  kBackgound         =  COLOUR_WHITE;
	static const StockColour  kBackgoundMany     =  COLOUR_LTGREY;	// for the MANY case
	static const StockColour  kCurve             =  COLOUR_BLUE;
	static /*const*/ StockColour  kStartColour   =  COLOUR_BLACK;
	static /*const*/ StockColour  kEndColour     =  COLOUR_WHITE;


	DocRect  RenderRect( 0, 0,  kWidth, kHeight );

	RenderRegion*  pRenderRegion  =  CreateGRenderRegion( &RenderRect, ExtraInfo );


	if( pRenderRegion != 0 )
	{
		pRenderRegion->SaveContext();

		/// set drawing quality
		Quality           QualityThing( Quality::QualityMax );
		QualityAttribute  AntiAliasQualityAttr( QualityThing );
		pRenderRegion->SetQuality( &AntiAliasQualityAttr, FALSE );
		//pRenderRegion->RRQuality.SetQuality( QUALITY_MAX );   /// doesnt enable antialiasing		

		/// draw the background
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
	/// drawing constants:  overall resolution, curve steps, curve width, colors
	static const INT32  kWidth      =  ExtraInfo->dx;
	static const INT32  kBitWidth   =  kWidth/20;
	static const INT32  kHalfWidth  =  kWidth/2;
	static const INT32  kHeight     =  ExtraInfo->dy;
	static const INT32  kHalfHeight =  kHeight/2;
	static const INT32  kQuartHeight = kHeight/4;
	static const INT32  kLineWidth  =  2000;
	static const INT32  kHalfLineWidth = kLineWidth/2;

	static const StockColour  kBackgoundOutline  =  COLOUR_NONE;
	static const StockColour  kBackgound         =  COLOUR_WHITE;
	static const StockColour  kBackgoundMany     =  COLOUR_LTGREY;	// for the MANY case
	static const StockColour  kCurve             =  COLOUR_WHITE;
	static const StockColour  kCurve2             =  COLOUR_BLUE;//COLOUR_BLACK;//COLOUR_MAGENTA;
	static /*const*/ StockColour  kStartColour   =  COLOUR_BLACK;
	static /*const*/ StockColour  kEndColour     =  COLOUR_WHITE;


	DocRect  RenderRect( 0, 0,  kWidth, kHeight );

	RenderRegion*  pRenderRegion  =  CreateGRenderRegion( &RenderRect, ExtraInfo );


	if( pRenderRegion != 0 )
	{
		pRenderRegion->SaveContext();

		/// set drawing quality
		Quality           QualityThing( Quality::QualityMax );
		QualityAttribute  AntiAliasQualityAttr( QualityThing );
		pRenderRegion->SetQuality( &AntiAliasQualityAttr, FALSE );
		//pRenderRegion->RRQuality.SetQuality( QUALITY_MAX );   /// doesnt enable antialiasing		

		/// draw the background
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



/// sliders interface ------------------------------------------------------------------------------

///-------------------------------------------------------------------------------------------------
///	Author:			Harrison Ainsworth
///	Date:			07/99
///	Purpose:		initialise the dialog box slider controls, including setting positions
///	Preconditions:	slider control IDs are defined
///	Postconditions:	sliders have the right range and appearence, and an intial position
///-------------------------------------------------------------------------------------------------

void  CBiasGainDlg::InitSliders ( CProfileBiasGain const& BiasGain )
{

	InitOneSlider( _R(IDC_SLIDER_BIAS) );
	InitOneSlider( _R(IDC_SLIDER_GAIN) );

	WriteSliders( BiasGain );

}


///-------------------------------------------------------------------------------------------------
///	Author:			Chris Snook
///	Date:			18/1/2000
///	Purpose:		initialise the bias gain gadgets displayed (i.e.  selected/applied) image
///	Preconditions:	BiasGain is valid
///	Postconditions:	_R(IDC_BIASGAIN) displays the correct profile image
///-------------------------------------------------------------------------------------------------

void CBiasGainDlg::InitBiasGainGadget ( CProfileBiasGain const& BiasGain )
{
	WORD profileIndex;

	profileIndex = BiasGainGadget.FindPresetBiasGain (BiasGain);
	profileIndex++;			// cause we need to
	
	// *sniff* *sniff* - I smell a nasty bug !  Or at least I did before I fixed it ....

	if (profileIndex != 6)		// its a defined profile
	{
		SetCustomComboGadgetValue ( _R(IDC_BIASGAIN), (CustomComboBoxControlDataItem*) NULL, TRUE, profileIndex);
	}
	else						// its a custom profile
	{
		SetCustomComboGadgetValue ( _R(IDC_BIASGAIN), (CustomComboBoxControlDataItem*) NULL, TRUE, -2);
	}
}


///-------------------------------------------------------------------------------------------------
///	Author:			Harrison Ainsworth
///	Date:			07/99
///	Purpose:		set the dlg slider controls to express the values in the biasgain object
///	Preconditions:	slider control IDs are defined
///	Postconditions:	slider controls thumbs move to representative positions
///-------------------------------------------------------------------------------------------------

void  CBiasGainDlg::WriteSliders ( CProfileBiasGain const& BiasGain )
{

	WriteOneSlider( BiasGain.GetBias(), _R(IDC_SLIDER_BIAS) );

	WriteOneSlider( BiasGain.GetGain(), _R(IDC_SLIDER_GAIN) );

}

/*void CBiasGainDlg::SetProfile(CProfileBiasGain const &Profile)
{
	// set the variable
	BiasGain_m = Profile;
	
	// write the sliders
	WriteSliders(Profile);

	// draw the curve
	InvalidateGadget( _R(IDC_CURVE) );
	InvalidateGadget( _R(IDC_CURVEINTERPOLATE) );
	PaintGadgetNow(_R(IDC_CURVE));
	PaintGadgetNow ( _R(IDC_CURVEINTERPOLATE) );
}*/


///-------------------------------------------------------------------------------------------------
///	Author:			Chris Snook
///	Date:			17/1/2000
///	Purpose:		invoke the dialog with this CBiasGainGadget, using this CProfileBiasGain, and
///					whether the dialog needs to be configured for MANY different profile selections
///	Preconditions:	pInvokeWith and pInvokeOn are valid
///	Postconditions:	the dialog is displayed, and pInvokeWith is 'known' as where we route messages to
///-------------------------------------------------------------------------------------------------

void CBiasGainDlg::InvokeVia (CBiasGainGadget& pInvokeWith, CProfileBiasGain* pInvokeOn, BOOL bMany)
{
	OpDescriptor*  pOpDescriptor = OpDescriptor::FindOpDescriptor( OPTOKEN_BIASGAIN_DLG );
	if( pOpDescriptor != 0 )
	{
		if (pInvokeOn != NULL)
		{
			OpParam  Param( reinterpret_cast<INT32>( &pInvokeWith ),  reinterpret_cast<INT32>( pInvokeOn ) );
			DoWithParam( pOpDescriptor, &Param );
		}
		else if (bMany == TRUE)					// we need to display the many setting ....
		{
			OpParam  Param( reinterpret_cast<INT32>( &pInvokeWith ),  reinterpret_cast<INT32>( pInvokeOn ) );
			DoWithParam( pOpDescriptor, &Param );
		}
		else	// I'm not sure if this is ever called - BUT we'd best provide a 'default' anyway
		{
			CProfileBiasGain defaultInvokeOn;

			OpParam  Param( reinterpret_cast<INT32>( &pInvokeWith ),  reinterpret_cast<INT32>( &defaultInvokeOn ) );
			DoWithParam( pOpDescriptor, &Param );
		}
	}
}


///-------------------------------------------------------------------------------------------------
///	Author:			Chris Snook
///	Date:			17/1/2000
///	Purpose:		if *this* dialog is open (and the selection has changed) - then we need to reset
///					the contents of the dialog to reflect this
///	Preconditions:	ReInitOn is valid
///	Postconditions:	the dialog display is updated to reflect the supplied CProfileBiasGain object
///-------------------------------------------------------------------------------------------------

void CBiasGainDlg::ReInitialiseDialog (CProfileBiasGain* ReInitOn, BOOL bMany)
{
	if (ReInitOn != NULL)					// woopey!!!!  we have a profile - lets display it ....
	{
		BiasGain_m = *ReInitOn;				// this line does everything for us!

		// the rest is just tidying up ....

		if (manyDisabledControls == TRUE)
		{
			EnableAllControls ();
		}
	
		/// reinitialise gadgets ....
		InitSliders( BiasGain_m );
		InitEditBoxes( BiasGain_m );
		InitBiasGainGadget ( BiasGain_m );

		// redraw profile ....
		InvalidateGadget( _R(IDC_CURVE) );
		InvalidateGadget( _R(IDC_CURVEINTERPOLATE) );
	}
	else if (bMany == TRUE)					// we need to display the many setting ....
	{
		SetCustomComboGadgetValue ( _R(IDC_BIASGAIN), (CustomComboBoxControlDataItem*) NULL, TRUE, -1);

		// and disable all other controls ....

		DisableAllControls ();
	}
}


///-------------------------------------------------------------------------------------------------
///	Author:			Chris Snook
///	Date:			17/1/2000
///	Purpose:		disables all controls within the dialog in response to the MANY display mode
///					which (incedently) is also set by this function
///	Preconditions:	-
///	Postconditions:	the dialog display is updated with all controls disabled (except _R(IDC_BIASGAIN))
///-------------------------------------------------------------------------------------------------

void CBiasGainDlg::DisableAllControls ()
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


///-------------------------------------------------------------------------------------------------
///	Author:			Chris Snook
///	Date:			17/1/2000
///	Purpose:		enables all controls within the dialog in response to cancelling the MANY
///					display mode which (incedently) is also set by this function
///	Preconditions:	-
///	Postconditions:	the dialog display is updated with all controls enabled (except _R(IDC_BIASGAIN))
///-------------------------------------------------------------------------------------------------

void CBiasGainDlg::EnableAllControls ()
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

///-------------------------------------------------------------------------------------------------
///	Author:			Chris Snook
///	Date:			15/3/2000
///	Purpose:		return the current biasgain profile
///	Preconditions:	-
///	Postconditions:	-
///-------------------------------------------------------------------------------------------------

CProfileBiasGain CBiasGainDlg::GetCurrentDialogProfile ()
{
	return (BiasGain_m);
}


///-------------------------------------------------------------------------------------------------
///	Author:			Harrison Ainsworth
///	Date:			07/99
///	Purpose:		get values from the dlg slider controls and put them into a biasgain object
///	Preconditions:	slider control IDs are defined
///	Postconditions:	biasgain value 'matches' the slider positions
///-------------------------------------------------------------------------------------------------

void  CBiasGainDlg::ReadSliders ( CProfileBiasGain& BiasGain )
{

	BiasGain.SetBias( ReadOneSlider( _R(IDC_SLIDER_BIAS) ) );

	BiasGain.SetGain( ReadOneSlider( _R(IDC_SLIDER_GAIN) ) );

}


///-------------------------------------------------------------------------------------------------
///	Author:			Harrison Ainsworth
///	Date:			07/99
///	Purpose:		initialise a dialog box slider control
///	Preconditions:	slider control matching the ID does exist
///	Postconditions:	sliders have the right range and appearence
///-------------------------------------------------------------------------------------------------

void  CBiasGainDlg::InitOneSlider( CGadgetID GadgetID )
{

	SetGadgetRange( GadgetID, kSliderMin_s, kSliderMax_s );
	SetGadgetBitmaps( GadgetID, _R(IDB_SLIDERBASE), _R(IDB_SLIDERSLIDER) );

}


///-------------------------------------------------------------------------------------------------
///	Author:			Harrison Ainsworth
///	Date:			07/99
///	Purpose:		set a dlg slider control to express a [-1,+1] fp value
///	Preconditions:	ValueMinus1ToPlus1  is in the range [-1,+1]
///					slider control matching the ID does exist
///	Postconditions:	slider control thumb moves to representative position
///-------------------------------------------------------------------------------------------------

void  CBiasGainDlg::WriteOneSlider ( AFp ValueMinus1ToPlus1, CGadgetID GadgetID )
{

	const INT32  SliderValue  =  INT32( -ValueMinus1ToPlus1 * AFp( kSliderHalfLength_s ) + AFp(0.5) );
	SetLongGadgetValue( GadgetID,  SliderValue );

}


///-------------------------------------------------------------------------------------------------
///	Author:			Harrison Ainsworth
///	Date:			07/99
///	Purpose:		get the position of a dlg slider control as a [-1,+1] fp value
///	Preconditions:	slider control matching the ID does exist
///	Postconditions:	return value is in the range [-1,+1]
///-------------------------------------------------------------------------------------------------

AFp  CBiasGainDlg::ReadOneSlider ( CGadgetID GadgetID )
{

	const INT32  SliderValue         =  GetLongGadgetValue( GadgetID, kSliderMin_s, kSliderMax_s );
	const AFp   ValueMinus1ToPlus1  =  -AFp( SliderValue ) / AFp( kSliderHalfLength_s );

	return  ValueMinus1ToPlus1;

}




/// edit boxes interface ---------------------------------------------------------------------------

///-------------------------------------------------------------------------------------------------
///	Author:			Harrison Ainsworth
///	Date:			07/99
///	Purpose:		initialise the dialog box edit controls, including setting values
///	Preconditions:	
///	Postconditions:	edit controls have a value matching the biasgain input
///-------------------------------------------------------------------------------------------------

void  CBiasGainDlg::InitEditBoxes ( CProfileBiasGain const& BiasGain )
{

	WriteEditBoxes( BiasGain );

}


///-------------------------------------------------------------------------------------------------
///	Author:			Harrison Ainsworth
///	Date:			07/99
///	Purpose:		set dlg edit controls to the values of a biasgain object
///	Preconditions:	edit controls and matching defined IDs exist
///	Postconditions:	edit controls have a value matching the biasgain input
///-------------------------------------------------------------------------------------------------

void  CBiasGainDlg::WriteEditBoxes ( CProfileBiasGain const& BiasGain )
{

	/// bias edit
	{
		const AFp  BiasMinus1ToPlus1  =  BiasGain.GetBias();
		SetDoubleGadgetValue( _R(IDC_EDIT_BIAS),  BiasMinus1ToPlus1 );
	}

	/// gain edit
	{
		const AFp  GainMinus1ToPlus1  =  BiasGain.GetGain();
		SetDoubleGadgetValue( _R(IDC_EDIT_GAIN),  GainMinus1ToPlus1 );
	}

}


///-------------------------------------------------------------------------------------------------
///	Author:			Harrison Ainsworth
///	Date:			07/99
///	Purpose:		get values from the dlg edit controls and put them into a biasgain object
///					clamp the edit control values into biasgain range
///	Preconditions:	edit controls and matching defined IDs exist
///	Postconditions:	biasgain values equal the edit controls' values
///-------------------------------------------------------------------------------------------------

void  CBiasGainDlg::ReadEditBoxes ( CProfileBiasGain& BiasGain )
{

	/// read the values into the biasgain object
	{
		const AFp  BiasMinus1ToPlus1  =  AFp( GetDoubleGadgetValue( _R(IDC_EDIT_BIAS), double(-1.0), double(+1.0) ) );
		BiasGain.SetBias( BiasMinus1ToPlus1 );
	}
	{
		const AFp  GainMinus1ToPlus1  =  AFp( GetDoubleGadgetValue( _R(IDC_EDIT_GAIN), double(-1.0), double(+1.0) ) );
		BiasGain.SetGain( GainMinus1ToPlus1 );
	}


	/// write the clamped-into-range values back into the edit boxes
	WriteEditBoxes( BiasGain );

}








/// private ////////////////////////////////////////////////////////////////////////////////////////

///-------------------------------------------------------------------------------------------------
///	Author:			Harrison Ainsworth
///	Date:			07/99
///	Purpose:		carry out actions in response to a slider pos changing message
///	Preconditions:	ensured by input types
///	Postconditions:	the BiasGain input reflects the slider values, and the edit boxes, sliders and
///					diagram all match each other.
///					a message containing the BiasGain values is broadcast.
///-------------------------------------------------------------------------------------------------

void  CBiasGainDlg::HandleSliderPosChanging
(
	CDlgMessage const&	Message,
	CProfileBiasGain&	BiasGain
)
{

	{
		/// read values from sliders (both (simplifies code)) into biasgain object
		ReadSliders( BiasGain );

		/// make the edit boxes agree with the sliders
		WriteEditBoxes( BiasGain );

		/// and make the profile control show a custom image (cause after all, now we have
		/// a custom bias/gain)

		//WORD DropdownListIndex;
		//GetValueIndex( _R(IDC_BIASGAIN), &DropdownListIndex );

		//if (DropdownListIndex != 5)
		//{
			SetCustomComboGadgetValue ( _R(IDC_BIASGAIN), (CustomComboBoxControlDataItem*) NULL, TRUE, -2);
		//}
	}


	/// do general input handling
	HandleInput( Message,  BiasGain );

}


///-------------------------------------------------------------------------------------------------
///	Author:			Harrison Ainsworth
///	Date:			07/99
///	Purpose:		carry out actions in response to a slider pos set message
///	Preconditions:	ensured by input types
///	Postconditions:	the BiasGain input reflects the slider values, and the edit boxes, sliders and
///					diagram all match each other.
///					a message containing the BiasGain values is broadcast.
///-------------------------------------------------------------------------------------------------

void  CBiasGainDlg::HandleSliderPosSet
(
	CDlgMessage const&	Message,
	CProfileBiasGain&	BiasGain
)
{
	/// do the same as slider changing
	HandleSliderPosChanging( Message,  BiasGain );
}


///-------------------------------------------------------------------------------------------------
///	Author:			Harrison Ainsworth
///	Date:			07/99
///	Purpose:		carry out actions in response to a commit message
///	Preconditions:	ensured by input types
///	Postconditions:	the BiasGain input reflects the editbox values, and the edit boxes, sliders and
///					diagram all match each other.
///					a message containing the BiasGain values is broadcast.
///-------------------------------------------------------------------------------------------------

void  CBiasGainDlg::HandleCommit
(
	CDlgMessage const&	Message,
	CProfileBiasGain&	BiasGain
)
{

	{
		/// read values from edit boxes (both (simplifies code)) into biasgain object
		ReadEditBoxes( BiasGain_m );

		/// make the sliders agree with the edit boxes
		WriteSliders( BiasGain_m );
	}


	/// do general input handling
	HandleInput( Message,  BiasGain );

}


///-------------------------------------------------------------------------------------------------
///	Author:			Harrison Ainsworth
///	Date:			07/99
///	Purpose:		carry out actions in response to a general input message
///	Preconditions:	ensured by input types
///	Postconditions:	a redrawing of the diagram is 'prompted'
///					a message containing the BiasGain values is broadcast.
///-------------------------------------------------------------------------------------------------

void  CBiasGainDlg::HandleInput
(
	CDlgMessage const&	Message,
	CProfileBiasGain&	BiasGain
)
{

	/// prompt a diagram redraw to illustrate the new values
	InvalidateGadget( _R(IDC_CURVE) );
	InvalidateGadget( _R(IDC_CURVEINTERPOLATE) );

	/// send message
	BroadcastMessage( Message,  BiasGain_m );

}




///-------------------------------------------------------------------------------------------------
///	Author:			Harrison Ainsworth
///	Date:			07/99
///	Purpose:		communicate the biasgain values to the outside
///	Preconditions:	ensured by input types
///	Postconditions:	a message containing the biasgain values is sent out
///-------------------------------------------------------------------------------------------------

void  CBiasGainDlg::BroadcastMessage
(
	CDlgMessage const&			Message,
	CProfileBiasGain const&		BiasGain
)	const
{

	BROADCAST_TO_CLASS( DialogMsg( 0, Message, Id_m, reinterpret_cast<INT32>( &BiasGain ) ),  DialogBarOp );

}

